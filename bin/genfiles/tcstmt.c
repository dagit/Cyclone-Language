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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 113
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 942
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 966
void*Cyc_Absyn_exn_typ();
# 986
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1002
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1103
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1112
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1164
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 68 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 92
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 96
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 98
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 102
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 120
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 123
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 126
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned int);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple13**oldtv,unsigned int loc);
# 133
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 147
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 57
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 98
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 131
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 180
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 215 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 223
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 274
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 296
int Cyc_Tcutil_new_tvar_id();
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 316
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 334
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 338
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 36
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 107
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 109
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 111
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 48 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp5;struct Cyc_Absyn_Exp*_tmp4;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
({void*_tmp16B=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=5U,_tmp2->f1=_tmp4,_tmp2->f2=Cyc_Absyn_PreInc;_tmp2;});e->r=_tmp16B;});goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
({void*_tmp16C=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=5U,_tmp3->f1=_tmp5,_tmp3->f2=Cyc_Absyn_PreDec;_tmp3;});e->r=_tmp16C;});goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 61
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmp6=e->r;void*_tmp7=_tmp6;struct Cyc_List_List*_tmp2F;struct Cyc_List_List*_tmp2E;struct Cyc_List_List*_tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_Absyn_Exp*_tmpC;struct Cyc_Absyn_Exp*_tmpB;switch(*((int*)_tmp7)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 70
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmpD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmpC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_tmpB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL1E:
# 80
 if(!Cyc_Tcstmt_noassign_exp(_tmpD))return 0;
_tmpF=_tmpC;_tmpE=_tmpB;goto _LL20;case 27U: _LL1F: _tmpF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_tmpE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL20:
 _tmp11=_tmpF;_tmp10=_tmpE;goto _LL22;case 7U: _LL21: _tmp11=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp10=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL22:
 _tmp13=_tmp11;_tmp12=_tmp10;goto _LL24;case 8U: _LL23: _tmp13=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp12=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL24:
 _tmp15=_tmp13;_tmp14=_tmp12;goto _LL26;case 23U: _LL25: _tmp15=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp14=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL26:
 _tmp17=_tmp15;_tmp16=_tmp14;goto _LL28;case 9U: _LL27: _tmp17=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp16=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL28:
 return Cyc_Tcstmt_noassign_exp(_tmp17) && Cyc_Tcstmt_noassign_exp(_tmp16);case 41U: _LL29: _tmp18=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2A:
# 88
 _tmp19=_tmp18;goto _LL2C;case 38U: _LL2B: _tmp19=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2C:
 _tmp1A=_tmp19;goto _LL2E;case 11U: _LL2D: _tmp1A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2E:
 _tmp1B=_tmp1A;goto _LL30;case 12U: _LL2F: _tmp1B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL30:
 _tmp1C=_tmp1B;goto _LL32;case 13U: _LL31: _tmp1C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL32:
 _tmp1D=_tmp1C;goto _LL34;case 14U: _LL33: _tmp1D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL34:
 _tmp1E=_tmp1D;goto _LL36;case 18U: _LL35: _tmp1E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL36:
 _tmp1F=_tmp1E;goto _LL38;case 20U: _LL37: _tmp1F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL38:
 _tmp20=_tmp1F;goto _LL3A;case 21U: _LL39: _tmp20=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3A:
 _tmp21=_tmp20;goto _LL3C;case 22U: _LL3B: _tmp21=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3C:
 _tmp22=_tmp21;goto _LL3E;case 28U: _LL3D: _tmp22=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3E:
 _tmp23=_tmp22;goto _LL40;case 15U: _LL3F: _tmp23=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL40:
 return Cyc_Tcstmt_noassign_exp(_tmp23);case 34U: _LL41: _tmp25=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp7)->f1).rgn;_tmp24=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp7)->f1).num_elts;_LL42:
# 102
 _tmp27=_tmp25;_tmp26=_tmp24;goto _LL44;case 16U: _LL43: _tmp27=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp26=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL44:
# 104
 if(!Cyc_Tcstmt_noassign_exp(_tmp26))return 0;
if(_tmp27 != 0)return Cyc_Tcstmt_noassign_exp(_tmp27);else{
return 1;}case 3U: _LL45: _tmp28=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL46:
# 108
 _tmp29=_tmp28;goto _LL48;case 31U: _LL47: _tmp29=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL48:
 _tmp2A=_tmp29;goto _LL4A;case 24U: _LL49: _tmp2A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL4A:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,_tmp2A);case 36U: _LL4B: _tmp2B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL4C:
# 112
 _tmp2C=_tmp2B;goto _LL4E;case 29U: _LL4D: _tmp2C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL4E:
 _tmp2D=_tmp2C;goto _LL50;case 30U: _LL4F: _tmp2D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL50:
 _tmp2E=_tmp2D;goto _LL52;case 26U: _LL51: _tmp2E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL52:
 _tmp2F=_tmp2E;goto _LL54;default: _LL53: _tmp2F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL54:
# 117
 for(0;_tmp2F != 0;_tmp2F=_tmp2F->tl){
struct _tuple15*_tmp8=(struct _tuple15*)_tmp2F->hd;struct _tuple15*_tmp9=_tmp8;struct Cyc_Absyn_Exp*_tmpA;_LL56: _tmpA=_tmp9->f2;_LL57:;
if(!Cyc_Tcstmt_noassign_exp(_tmpA))return 0;}
# 121
return 1;}_LL0:;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 126
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 128
struct Cyc_Tcpat_TcPatResult _tmp30=pat_res;struct _tuple1*_tmp3C;struct Cyc_List_List*_tmp3B;_LL1: _tmp3C=_tmp30.tvars_and_bounds_opt;_tmp3B=_tmp30.patvars;_LL2:;{
struct Cyc_List_List*_tmp31=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple17*))Cyc_Core_fst,_tmp3B);
struct Cyc_List_List*_tmp32=_tmp3C == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*_tmp3C).f1);
struct Cyc_List_List*_tmp33=_tmp3C == 0?0:(*_tmp3C).f2;
struct Cyc_List_List*_tmp34=_tmp3C == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple16*))Cyc_Core_snd,(*_tmp3C).f1));
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp32);
for(0;_tmp34 != 0;_tmp34=_tmp34->tl){
({struct Cyc_Tcenv_Tenv*_tmp16E=({struct Cyc_Tcenv_Tenv*_tmp16D=te2;Cyc_Tcenv_add_region(_tmp16D,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=2U,_tmp35->f1=(struct Cyc_Absyn_Tvar*)_tmp34->hd;_tmp35;}),0);});te2=_tmp16E;});}
({struct Cyc_Tcenv_Tenv*_tmp16F=Cyc_Tcenv_new_outlives_constraints(te2,_tmp33,loc);te2=_tmp16F;});
if(new_block)
({struct Cyc_Tcenv_Tenv*_tmp170=Cyc_Tcenv_new_block(loc,te2);te2=_tmp170;});{
void*_tmp36=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp37=_tmp31;for(0;_tmp37 != 0;_tmp37=_tmp37->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp37->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp37->hd)))->rgn=_tmp36;}}
# 144
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp172=te2;struct Cyc_Absyn_Exp*_tmp171=where_opt;Cyc_Tcexp_tcTest(_tmp172,_tmp171,({const char*_tmp38="switch clause guard";_tag_dyneither(_tmp38,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp39=0U;({unsigned int _tmp174=where_opt->loc;struct _dyneither_ptr _tmp173=({const char*_tmp3A="cannot show &&-clause in pattern has no effects";_tag_dyneither(_tmp3A,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp174,_tmp173,_tag_dyneither(_tmp39,sizeof(void*),0U));});});}
# 149
Cyc_Tcstmt_tcStmt(te2,s,0);};};}
# 154
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 156
struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp3D=Cyc_Tcenv_add_region_equality(te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp3D,s0,new_block);
if(tk != 0)
({void*_tmp175=(*((struct _tuple13*)_check_null(tk))).f2;((*((struct _tuple13*)_check_null(tk))).f1)->kind=_tmp175;});}
# 163
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp3E=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _tuple2*res=({struct _tuple2*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp178=({struct _dyneither_ptr*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _dyneither_ptr _tmp177=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp41=({struct Cyc_Int_pa_PrintArg_struct _tmp150;_tmp150.tag=1U,_tmp150.f1=(unsigned int)_tmp3E;_tmp150;});void*_tmp3F[1U];_tmp3F[0]=& _tmp41;({struct _dyneither_ptr _tmp176=({const char*_tmp40="_stmttmp%X";_tag_dyneither(_tmp40,sizeof(char),11U);});Cyc_aprintf(_tmp176,_tag_dyneither(_tmp3F,sizeof(void*),1U));});});*_tmp42=_tmp177;});_tmp42;});_tmp43->f2=_tmp178;});_tmp43;});
return res;}
# 177 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 179
void*_tmp44=s0->r;void*_tmp45=_tmp44;struct Cyc_Absyn_Decl*_tmp14F;struct Cyc_Absyn_Stmt*_tmp14E;struct Cyc_Absyn_Stmt*_tmp14D;struct Cyc_List_List**_tmp14C;void**_tmp14B;struct Cyc_Absyn_Exp**_tmp14A;struct Cyc_List_List*_tmp149;void**_tmp148;struct _dyneither_ptr*_tmp147;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_List_List*_tmp145;struct Cyc_Absyn_Switch_clause***_tmp144;struct Cyc_Absyn_Stmt*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Stmt*_tmp141;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Stmt*_tmp13C;struct Cyc_Absyn_Stmt*_tmp13B;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Stmt*_tmp139;struct Cyc_Absyn_Stmt*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Stmt*_tmp136;struct Cyc_Absyn_Stmt*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Stmt*_tmp133;struct Cyc_Absyn_Stmt*_tmp132;struct Cyc_Absyn_Exp*_tmp131;switch(*((int*)_tmp45)){case 0U: _LL1: _LL2:
# 181
 return;case 1U: _LL3: _tmp131=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_LL4:
# 184
 Cyc_Tcexp_tcExp(te,0,_tmp131);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(_tmp131);
return;case 2U: _LL5: _tmp133=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp132=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL6:
# 190
 Cyc_Tcstmt_tcStmt(te,_tmp133,1);
Cyc_Tcstmt_tcStmt(te,_tmp132,1);
return;case 3U: _LL7: _tmp134=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_LL8: {
# 195
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp134 == 0){
void*_tmp46=Cyc_Tcutil_compress(t);void*_tmp47=_tmp46;switch(*((int*)_tmp47)){case 0U: _LL22: _LL23:
 goto _LL21;case 7U: _LL24: _LL25:
 goto _LL27;case 6U: _LL26: _LL27:
# 201
({struct Cyc_String_pa_PrintArg_struct _tmp4A=({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0U,({struct _dyneither_ptr _tmp179=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp151.f1=_tmp179;});_tmp151;});void*_tmp48[1U];_tmp48[0]=& _tmp4A;({unsigned int _tmp17B=s0->loc;struct _dyneither_ptr _tmp17A=({const char*_tmp49="should return a value of type %s";_tag_dyneither(_tmp49,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp17B,_tmp17A,_tag_dyneither(_tmp48,sizeof(void*),1U));});});goto _LL21;default: _LL28: _LL29:
# 203
({struct Cyc_String_pa_PrintArg_struct _tmp4D=({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0U,({struct _dyneither_ptr _tmp17C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp152.f1=_tmp17C;});_tmp152;});void*_tmp4B[1U];_tmp4B[0]=& _tmp4D;({unsigned int _tmp17E=s0->loc;struct _dyneither_ptr _tmp17D=({const char*_tmp4C="must return a value of type %s";_tag_dyneither(_tmp4C,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp17E,_tmp17D,_tag_dyneither(_tmp4B,sizeof(void*),1U));});});goto _LL21;}_LL21:;}else{
# 207
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp134;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,({
struct _dyneither_ptr _tmp17F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp154.f1=_tmp17F;});_tmp154;});struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,({struct _dyneither_ptr _tmp180=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp153.f1=_tmp180;});_tmp153;});void*_tmp4E[2U];_tmp4E[0]=& _tmp50,_tmp4E[1]=& _tmp51;({unsigned int _tmp182=s0->loc;struct _dyneither_ptr _tmp181=({const char*_tmp4F="returns value of type %s but requires %s";_tag_dyneither(_tmp4F,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp182,_tmp181,_tag_dyneither(_tmp4E,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 215
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp52=0U;({unsigned int _tmp184=e->loc;struct _dyneither_ptr _tmp183=({const char*_tmp53="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp53,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp184,_tmp183,_tag_dyneither(_tmp52,sizeof(void*),0U));});});}
# 218
return;}case 4U: _LL9: _tmp137=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp136=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_tmp135=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f3;_LLA: {
# 221
struct Cyc_Tcexp_TestEnv _tmp54=({struct Cyc_Tcenv_Tenv*_tmp186=te;struct Cyc_Absyn_Exp*_tmp185=_tmp137;Cyc_Tcexp_tcTest(_tmp186,_tmp185,({const char*_tmp5B="if statement";_tag_dyneither(_tmp5B,sizeof(char),13U);}));});struct Cyc_Tcexp_TestEnv _tmp55=_tmp54;struct _tuple0*_tmp5A;int _tmp59;_LL2B: _tmp5A=_tmp55.eq;_tmp59=_tmp55.isTrue;_LL2C:;
{struct _tuple0*_tmp56=_tmp5A;void*_tmp58;void*_tmp57;if(_tmp56 != 0){_LL2E: _tmp58=_tmp56->f1;_tmp57=_tmp56->f2;_LL2F:
# 224
 if(_tmp59){
Cyc_Tcstmt_tcStmtRefine(te,_tmp136,_tmp58,_tmp57,1);
Cyc_Tcstmt_tcStmt(te,_tmp135,1);}else{
# 229
Cyc_Tcstmt_tcStmt(te,_tmp136,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp135,_tmp58,_tmp57,1);}
# 232
goto _LL2D;}else{_LL30: _LL31:
# 234
 Cyc_Tcstmt_tcStmt(te,_tmp136,1);
Cyc_Tcstmt_tcStmt(te,_tmp135,1);
goto _LL2D;}_LL2D:;}
# 238
return;}case 5U: _LLB: _tmp13A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f1).f1;_tmp139=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f1).f2;_tmp138=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LLC:
# 241
({struct Cyc_Tcenv_Tenv*_tmp188=te;struct Cyc_Absyn_Exp*_tmp187=_tmp13A;Cyc_Tcexp_tcTest(_tmp188,_tmp187,({const char*_tmp5C="while loop";_tag_dyneither(_tmp5C,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp138,1);
return;case 9U: _LLD: _tmp140=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp13F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f1;_tmp13E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f2;_tmp13D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f3).f1;_tmp13C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f3).f2;_tmp13B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f4;_LLE:
# 246
 Cyc_Tcexp_tcExp(te,0,_tmp140);
({struct Cyc_Tcenv_Tenv*_tmp18A=te;struct Cyc_Absyn_Exp*_tmp189=_tmp13F;Cyc_Tcexp_tcTest(_tmp18A,_tmp189,({const char*_tmp5D="for loop";_tag_dyneither(_tmp5D,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp13B,1);
Cyc_Tcexp_tcExp(te,0,_tmp13D);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp13D);
return;case 14U: _LLF: _tmp143=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp142=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f1;_tmp141=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f2;_LL10:
# 254
 Cyc_Tcstmt_tcStmt(te,_tmp143,1);
({struct Cyc_Tcenv_Tenv*_tmp18C=te;struct Cyc_Absyn_Exp*_tmp18B=_tmp142;Cyc_Tcexp_tcTest(_tmp18C,_tmp18B,({const char*_tmp5E="do loop";_tag_dyneither(_tmp5E,sizeof(char),8U);}));});
return;case 6U: _LL11: _LL12:
# 258
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp145=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp144=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL18: {
# 263
const struct _tuple12*_tmp5F=Cyc_Tcenv_process_fallthru(te,s0,_tmp144);
if(_tmp5F == (const struct _tuple12*)0){
({void*_tmp60=0U;({unsigned int _tmp18E=s0->loc;struct _dyneither_ptr _tmp18D=({const char*_tmp61="fallthru not in a non-last case";_tag_dyneither(_tmp61,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp18E,_tmp18D,_tag_dyneither(_tmp60,sizeof(void*),0U));});});
return;}{
# 268
struct Cyc_List_List*_tmp62=(*_tmp5F).f2;
struct Cyc_List_List*_tmp63=(*_tmp5F).f3;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp18F=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp18F,_tmp62);});
struct Cyc_List_List*_tmp64=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,_tmp63);
for(0;_tmp64 != 0  && _tmp145 != 0;(_tmp64=_tmp64->tl,_tmp145=_tmp145->tl)){
# 275
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp145->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp145->hd,(void*)_tmp64->hd,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U,({
# 280
struct _dyneither_ptr _tmp190=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp145->hd)->topt)));_tmp156.f1=_tmp190;});_tmp156;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U,({struct _dyneither_ptr _tmp191=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp64->hd));_tmp155.f1=_tmp191;});_tmp155;});void*_tmp65[2U];_tmp65[0]=& _tmp67,_tmp65[1]=& _tmp68;({unsigned int _tmp193=s0->loc;struct _dyneither_ptr _tmp192=({const char*_tmp66="fallthru argument has type %s but pattern variable has type %s";_tag_dyneither(_tmp66,sizeof(char),63U);});Cyc_Tcutil_terr(_tmp193,_tmp192,_tag_dyneither(_tmp65,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 284
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp64->hd) && !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp145->hd))
({void*_tmp69=0U;({unsigned int _tmp195=((struct Cyc_Absyn_Exp*)_tmp145->hd)->loc;struct _dyneither_ptr _tmp194=({const char*_tmp6A="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp6A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp195,_tmp194,_tag_dyneither(_tmp69,sizeof(void*),0U));});});}
# 287
if(_tmp145 != 0)
({void*_tmp6B=0U;({unsigned int _tmp197=s0->loc;struct _dyneither_ptr _tmp196=({const char*_tmp6C="too many arguments in explicit fallthru";_tag_dyneither(_tmp6C,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp197,_tmp196,_tag_dyneither(_tmp6B,sizeof(void*),0U));});});
if(_tmp64 != 0)
({void*_tmp6D=0U;({unsigned int _tmp199=s0->loc;struct _dyneither_ptr _tmp198=({const char*_tmp6E="too few arguments in explicit fallthru";_tag_dyneither(_tmp6E,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp199,_tmp198,_tag_dyneither(_tmp6D,sizeof(void*),0U));});});
return;};}case 13U: _LL19: _tmp147=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp146=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL1A:
# 297
({struct Cyc_Tcenv_Tenv*_tmp1A1=({unsigned int _tmp1A0=s0->loc;struct Cyc_Tcenv_Tenv*_tmp19F=te;Cyc_Tcenv_new_named_block(_tmp1A0,_tmp19F,({struct Cyc_Absyn_Tvar*_tmp73=_cycalloc(sizeof(*_tmp73));
({struct _dyneither_ptr*_tmp19E=({struct _dyneither_ptr*_tmp72=_cycalloc(sizeof(*_tmp72));({struct _dyneither_ptr _tmp19D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp71=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U,_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp147);_tmp157;});void*_tmp6F[1U];_tmp6F[0]=& _tmp71;({struct _dyneither_ptr _tmp19C=({const char*_tmp70="`%s";_tag_dyneither(_tmp70,sizeof(char),4U);});Cyc_aprintf(_tmp19C,_tag_dyneither(_tmp6F,sizeof(void*),1U));});});*_tmp72=_tmp19D;});_tmp72;});_tmp73->name=_tmp19E;}),({
int _tmp19B=Cyc_Tcutil_new_tvar_id();_tmp73->identity=_tmp19B;}),({
void*_tmp19A=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp73->kind=_tmp19A;});_tmp73;}));});
# 297
Cyc_Tcstmt_tcStmt(_tmp1A1,_tmp146,0);});
# 301
return;case 10U: _LL1B: _tmp14A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp149=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_tmp148=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f3;_LL1C: {
# 306
struct Cyc_Absyn_Exp*_tmp74=*_tmp14A;
if(!Cyc_Tcutil_is_var_exp(_tmp74)){
struct Cyc_Absyn_Vardecl*_tmp75=({struct _tuple2*_tmp1A2=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1A2,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmp76=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmp77=({struct Cyc_Absyn_Pat*_tmp1A6=({void*_tmp1A4=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=1U,_tmp7A->f1=_tmp75,({struct Cyc_Absyn_Pat*_tmp1A3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp74->loc);_tmp7A->f2=_tmp1A3;});_tmp7A;});Cyc_Absyn_new_pat(_tmp1A4,_tmp74->loc);});struct Cyc_Absyn_Exp*_tmp1A5=_tmp74;Cyc_Absyn_let_decl(_tmp1A6,_tmp1A5,s0->loc);});
# 313
struct Cyc_Absyn_Stmt*_tmp78=Cyc_Absyn_decl_stmt(_tmp77,_tmp76,s0->loc);
s0->r=_tmp78->r;
({struct Cyc_Absyn_Exp*_tmp1A8=({void*_tmp1A7=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=4U,_tmp79->f1=_tmp75;_tmp79;});Cyc_Absyn_varb_exp(_tmp1A7,_tmp74->loc);});*_tmp14A=_tmp1A8;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 320
void*_tmp7B=Cyc_Tcexp_tcExp(te,0,_tmp74);
# 322
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp7B) && !Cyc_Tcutil_is_noalias_path(_tmp74))
({void*_tmp7C=0U;({unsigned int _tmp1AA=_tmp74->loc;struct _dyneither_ptr _tmp1A9=({const char*_tmp7D="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp7D,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp1AA,_tmp1A9,_tag_dyneither(_tmp7C,sizeof(void*),0U));});});{
# 327
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp7E=_tmp149;for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp1AB=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->pattern,& _tmp7B,_tmp74);this_pattern=_tmp1AB;});}else{
# 335
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 337
if(_tmp7E->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp1AD=({struct Cyc_Tcpat_TcPatResult*_tmp7F=_cycalloc(sizeof(*_tmp7F));({struct Cyc_Tcpat_TcPatResult _tmp1AC=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp7E->tl))->hd)->pattern,& _tmp7B,_tmp74);*_tmp7F=_tmp1AC;});_tmp7F;});next_pattern=_tmp1AD;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 341
({struct Cyc_Tcenv_Tenv*_tmp1AE=Cyc_Tcenv_clear_fallthru(te);te=_tmp1AE;});else{
# 343
struct Cyc_List_List*_tmp80=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp81=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1AF=Cyc_Tcenv_set_fallthru(te,_tmp81,_tmp80,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp7E->tl))->hd);te=_tmp1AF;});}}else{
# 349
({struct Cyc_Tcenv_Tenv*_tmp1B0=Cyc_Tcenv_clear_fallthru(te);te=_tmp1B0;});}{
# 351
struct Cyc_Absyn_Pat*_tmp82=((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp82->topt),_tmp7B)){
({struct Cyc_String_pa_PrintArg_struct _tmp85=({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0U,({
struct _dyneither_ptr _tmp1B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7B));_tmp159.f1=_tmp1B1;});_tmp159;});struct Cyc_String_pa_PrintArg_struct _tmp86=({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0U,({struct _dyneither_ptr _tmp1B2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp82->topt)));_tmp158.f1=_tmp1B2;});_tmp158;});void*_tmp83[2U];_tmp83[0]=& _tmp85,_tmp83[1]=& _tmp86;({unsigned int _tmp1B4=((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->loc;struct _dyneither_ptr _tmp1B3=({const char*_tmp84="switch on type %s, but case expects type %s";_tag_dyneither(_tmp84,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1B4,_tmp1B3,_tag_dyneither(_tmp83,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 358
Cyc_Tcpat_check_pat_regions(te,_tmp82,this_pattern.patvars);}
# 360
({struct Cyc_Core_Opt*_tmp1B5=({struct Cyc_Core_Opt*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->v=this_pattern.patvars;_tmp87;});((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->pat_vars=_tmp1B5;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp7E->hd)->where_clause,1);};}}
# 364
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp149,_tmp148);
return;};};}case 15U: _LL1D: _tmp14D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp14C=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_tmp14B=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp45)->f3;_LL1E:
# 372
({void*_tmp1B9=({void*_tmp1B8=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1B7=Cyc_Absyn_new_stmt(_tmp14D->r,_tmp14D->loc);_tmp88->f1=_tmp1B7;}),({struct Cyc_Absyn_Stmt*_tmp1B6=Cyc_Absyn_skip_stmt(_tmp14D->loc);_tmp88->f2=_tmp1B6;});_tmp88;});Cyc_Absyn_new_stmt(_tmp1B8,_tmp14D->loc);})->r;_tmp14D->r=_tmp1B9;});
# 374
Cyc_Tcstmt_tcStmt(te,_tmp14D,1);{
# 377
struct _tuple2*def_v=({struct _tuple2*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp1BB=({struct _dyneither_ptr*_tmp9E=_cycalloc(sizeof(*_tmp9E));({struct _dyneither_ptr _tmp1BA=({const char*_tmp9D="exn";_tag_dyneither(_tmp9D,sizeof(char),4U);});*_tmp9E=_tmp1BA;});_tmp9E;});_tmp9F->f2=_tmp1BB;});_tmp9F;});
struct Cyc_Absyn_Vardecl*_tmp89=Cyc_Absyn_new_vardecl(0U,def_v,(void*)& Cyc_Absyn_VoidType_val,0);
struct Cyc_Absyn_Pat*_tmp8A=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=1U,_tmp9C->f1=_tmp89,({struct Cyc_Absyn_Pat*_tmp1BC=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp9C->f2=_tmp1BC;});_tmp9C;}),0U);
struct Cyc_Absyn_Stmt*_tmp8B=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=11U,({struct Cyc_Absyn_Exp*_tmp1BD=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=4U,_tmp9A->f1=_tmp89;_tmp9A;}),0U);_tmp9B->f1=_tmp1BD;}),_tmp9B->f2=1;_tmp9B;}),0U),0U);
struct Cyc_Absyn_Switch_clause*_tmp8C=({struct Cyc_Absyn_Switch_clause*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->pattern=_tmp8A,_tmp99->pat_vars=0,_tmp99->where_clause=0,_tmp99->body=_tmp8B,_tmp99->loc=0U;_tmp99;});
({struct Cyc_List_List*_tmp1BF=({struct Cyc_List_List*_tmp1BE=*_tmp14C;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BE,({struct Cyc_Absyn_Switch_clause*_tmp8D[1U];_tmp8D[0]=_tmp8C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp8D,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*_tmp14C=_tmp1BF;});{
# 389
void*_tmp8E=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp8F=*_tmp14C;for(0;_tmp8F != 0;_tmp8F=_tmp8F->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp1C0=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp8F->hd)->pattern,& _tmp8E,0);this_pattern=_tmp1C0;});}else{
# 398
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 400
if(_tmp8F->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp1C2=({struct Cyc_Tcpat_TcPatResult*_tmp90=_cycalloc(sizeof(*_tmp90));({struct Cyc_Tcpat_TcPatResult _tmp1C1=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp8F->tl))->hd)->pattern,& _tmp8E,0);*_tmp90=_tmp1C1;});_tmp90;});next_pattern=_tmp1C2;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 404
({struct Cyc_Tcenv_Tenv*_tmp1C3=Cyc_Tcenv_clear_fallthru(te);te=_tmp1C3;});else{
# 406
struct Cyc_List_List*_tmp91=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp92=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1C4=Cyc_Tcenv_set_fallthru(te,_tmp92,_tmp91,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp8F->tl))->hd);te=_tmp1C4;});}}else{
# 412
({struct Cyc_Tcenv_Tenv*_tmp1C5=Cyc_Tcenv_clear_fallthru(te);te=_tmp1C5;});}{
# 414
struct Cyc_Absyn_Pat*_tmp93=((struct Cyc_Absyn_Switch_clause*)_tmp8F->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp93->topt),_tmp8E)){
({struct Cyc_String_pa_PrintArg_struct _tmp96=({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0U,({
struct _dyneither_ptr _tmp1C6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp8E));_tmp15B.f1=_tmp1C6;});_tmp15B;});struct Cyc_String_pa_PrintArg_struct _tmp97=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0U,({struct _dyneither_ptr _tmp1C7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp93->topt)));_tmp15A.f1=_tmp1C7;});_tmp15A;});void*_tmp94[2U];_tmp94[0]=& _tmp96,_tmp94[1]=& _tmp97;({unsigned int _tmp1C9=((struct Cyc_Absyn_Switch_clause*)_tmp8F->hd)->loc;struct _dyneither_ptr _tmp1C8=({const char*_tmp95="switch on type %s, but case expects type %s";_tag_dyneither(_tmp95,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1C9,_tmp1C8,_tag_dyneither(_tmp94,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 421
Cyc_Tcpat_check_pat_regions(te,_tmp93,this_pattern.patvars);}
# 423
({struct Cyc_Core_Opt*_tmp1CA=({struct Cyc_Core_Opt*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->v=this_pattern.patvars;_tmp98;});((struct Cyc_Absyn_Switch_clause*)_tmp8F->hd)->pat_vars=_tmp1CA;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp8F->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp8F->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp8F->hd)->where_clause,1);};}}
# 427
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*_tmp14C,_tmp14B);
return;};};default: _LL1F: _tmp14F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp14E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL20: {
# 431
struct _dyneither_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmpA0=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;
{void*_tmpA1=_tmp14F->r;void*_tmpA2=_tmpA1;struct _tuple2*_tmp12D;struct Cyc_List_List*_tmp12C;struct _dyneither_ptr*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Fndecl*_tmp129;struct Cyc_Absyn_Tvar*_tmp128;struct Cyc_Absyn_Vardecl*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_List_List*_tmp125;struct Cyc_Absyn_Pat*_tmp124;struct Cyc_Core_Opt**_tmp123;struct Cyc_Absyn_Exp**_tmp122;void**_tmp121;struct Cyc_Absyn_Vardecl*_tmp120;switch(*((int*)_tmpA2)){case 0U: _LL33: _tmp120=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA2)->f1;_LL34: {
# 435
struct Cyc_Absyn_Vardecl*_tmpA3=_tmp120;enum Cyc_Absyn_Scope _tmpD6;union Cyc_Absyn_Nmspace _tmpD5;struct _dyneither_ptr*_tmpD4;struct Cyc_Absyn_Tqual _tmpD3;void*_tmpD2;struct Cyc_Absyn_Exp*_tmpD1;void**_tmpD0;struct Cyc_List_List*_tmpCF;_LL52: _tmpD6=_tmpA3->sc;_tmpD5=(_tmpA3->name)->f1;_tmpD4=(_tmpA3->name)->f2;_tmpD3=_tmpA3->tq;_tmpD2=_tmpA3->type;_tmpD1=_tmpA3->initializer;_tmpD0=(void**)& _tmpA3->rgn;_tmpCF=_tmpA3->attributes;_LL53:;{
void*_tmpA4=Cyc_Tcenv_curr_rgn(_tmpA0);
int is_local;
{enum Cyc_Absyn_Scope _tmpA5=_tmpD6;switch(_tmpA5){case Cyc_Absyn_Static: _LL55: _LL56:
 goto _LL58;case Cyc_Absyn_Extern: _LL57: _LL58:
 goto _LL5A;case Cyc_Absyn_ExternC: _LL59: _LL5A:
# 442
 _tmp120->escapes=1;
is_local=0;goto _LL54;case Cyc_Absyn_Abstract: _LL5B: _LL5C:
# 445
({void*_tmpA6=0U;({unsigned int _tmp1CC=_tmp14F->loc;struct _dyneither_ptr _tmp1CB=({const char*_tmpA7="bad abstract scope for local variable";_tag_dyneither(_tmpA7,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp1CC,_tmp1CB,_tag_dyneither(_tmpA6,sizeof(void*),0U));});});
goto _LL5E;case Cyc_Absyn_Register: _LL5D: _LL5E:
 goto _LL60;default: _LL5F: _LL60:
 is_local=1;goto _LL54;}_LL54:;}
# 451
*_tmpD0=is_local?_tmpA4:(void*)& Cyc_Absyn_HeapRgn_val;
# 453
{void*_tmpA8=Cyc_Tcutil_compress(_tmpD2);void*_tmpA9=_tmpA8;void*_tmpB5;struct Cyc_Absyn_Tqual _tmpB4;union Cyc_Absyn_Constraint*_tmpB3;unsigned int _tmpB2;switch(*((int*)_tmpA9)){case 9U: _LL62: if(is_local){_LL63:
# 456
 _tmp120->escapes=1;
_tmpD6=Cyc_Absyn_Extern;
is_local=0;
goto _LL61;}else{goto _LL66;}case 8U: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9)->f1).num_elts == 0){_LL64: _tmpB5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9)->f1).elt_type;_tmpB4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9)->f1).tq;_tmpB3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9)->f1).zero_term;_tmpB2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9)->f1).zt_loc;if(_tmp120->initializer != 0){_LL65:
# 461
{void*_tmpAA=((struct Cyc_Absyn_Exp*)_check_null(_tmp120->initializer))->r;void*_tmpAB=_tmpAA;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct _dyneither_ptr _tmpAD;struct _dyneither_ptr _tmpAC;switch(*((int*)_tmpAB)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAB)->f1).Wstring_c).tag){case 8U: _LL69: _tmpAC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAB)->f1).String_c).val;_LL6A:
# 463
({void*_tmp1D2=({void*_tmp1D1=({void*_tmp1D0=_tmpB5;struct Cyc_Absyn_Tqual _tmp1CF=_tmpB4;struct Cyc_Absyn_Exp*_tmp1CE=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpAC,sizeof(char)),0U);union Cyc_Absyn_Constraint*_tmp1CD=_tmpB3;Cyc_Absyn_array_typ(_tmp1D0,_tmp1CF,_tmp1CE,_tmp1CD,_tmpB2);});_tmp120->type=_tmp1D1;});_tmpD2=_tmp1D2;});
goto _LL68;case 9U: _LL6B: _tmpAD=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAB)->f1).Wstring_c).val;_LL6C:
# 466
({void*_tmp1D8=({void*_tmp1D7=({void*_tmp1D6=_tmpB5;struct Cyc_Absyn_Tqual _tmp1D5=_tmpB4;struct Cyc_Absyn_Exp*_tmp1D4=Cyc_Absyn_uint_exp(1U,0U);union Cyc_Absyn_Constraint*_tmp1D3=_tmpB3;Cyc_Absyn_array_typ(_tmp1D6,_tmp1D5,_tmp1D4,_tmp1D3,_tmpB2);});_tmp120->type=_tmp1D7;});_tmpD2=_tmp1D8;});
goto _LL68;default: goto _LL75;}case 27U: _LL6D: _tmpAE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAB)->f2;_LL6E:
 _tmpAF=_tmpAE;goto _LL70;case 28U: _LL6F: _tmpAF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAB)->f1;_LL70:
# 471
({void*_tmp1DA=({void*_tmp1D9=Cyc_Absyn_array_typ(_tmpB5,_tmpB4,_tmpAF,_tmpB3,_tmpB2);_tmp120->type=_tmp1D9;});_tmpD2=_tmp1DA;});
goto _LL68;case 36U: _LL71: _tmpB0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAB)->f2;_LL72:
 _tmpB1=_tmpB0;goto _LL74;case 26U: _LL73: _tmpB1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAB)->f1;_LL74:
# 475
({void*_tmp1E0=({void*_tmp1DF=({void*_tmp1DE=_tmpB5;struct Cyc_Absyn_Tqual _tmp1DD=_tmpB4;struct Cyc_Absyn_Exp*_tmp1DC=Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB1),0U);union Cyc_Absyn_Constraint*_tmp1DB=_tmpB3;Cyc_Absyn_array_typ(_tmp1DE,_tmp1DD,_tmp1DC,_tmp1DB,_tmpB2);});_tmp120->type=_tmp1DF;});_tmpD2=_tmp1E0;});
goto _LL68;default: _LL75: _LL76:
 goto _LL68;}_LL68:;}
# 479
goto _LL61;}else{goto _LL66;}}else{goto _LL66;}default: _LL66: _LL67:
 goto _LL61;}_LL61:;}{
# 486
struct Cyc_List_List*_tmpB6=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpA0);
int _tmpB7=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpA0,_tmpB6,& Cyc_Tcutil_tmk,_tmpB7,1,_tmpD2);
({int _tmp1E1=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmp120->tq).print_const,_tmpD2);(_tmp120->tq).real_const=_tmp1E1;});{
# 491
struct Cyc_Tcenv_Tenv*_tmpB8=_tmpA0;
if(_tmpD6 == Cyc_Absyn_Extern  || _tmpD6 == Cyc_Absyn_ExternC)
({void*_tmpB9=0U;({unsigned int _tmp1E3=_tmp14F->loc;struct _dyneither_ptr _tmp1E2=({const char*_tmpBA="extern declarations are not yet supported within functions";_tag_dyneither(_tmpBA,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1E3,_tmp1E2,_tag_dyneither(_tmpB9,sizeof(void*),0U));});});
if(_tmpD1 != 0){
Cyc_Tcexp_tcExpInitializer(_tmpB8,& _tmpD2,_tmpD1);
# 497
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpD1))
({void*_tmpBB=0U;({unsigned int _tmp1E5=_tmp14F->loc;struct _dyneither_ptr _tmp1E4=({const char*_tmpBC="initializer for static variable needs to be a constant expression";_tag_dyneither(_tmpBC,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1E5,_tmp1E4,_tag_dyneither(_tmpBB,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(_tmpB8,_tmpD1,_tmpD2)){
struct _dyneither_ptr _tmpBD=*_tmpD4;
struct _dyneither_ptr _tmpBE=Cyc_Absynpp_typ2string(_tmpD2);
struct _dyneither_ptr _tmpBF=Cyc_Absynpp_typ2string((void*)_check_null(_tmpD1->topt));
if(((_get_dyneither_size(_tmpBD,sizeof(char))+ _get_dyneither_size(_tmpBE,sizeof(char)))+ _get_dyneither_size(_tmpBF,sizeof(char)))+ 50 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0U,_tmp15E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBD);_tmp15E;});struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0U,_tmp15D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE);_tmp15D;});struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0U,_tmp15C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBF);_tmp15C;});void*_tmpC0[3U];_tmpC0[0]=& _tmpC2,_tmpC0[1]=& _tmpC3,_tmpC0[2]=& _tmpC4;({unsigned int _tmp1E7=_tmp14F->loc;struct _dyneither_ptr _tmp1E6=({const char*_tmpC1="%s was declared with type %s but initialized with type %s.";_tag_dyneither(_tmpC1,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1E7,_tmp1E6,_tag_dyneither(_tmpC0,sizeof(void*),3U));});});else{
# 506
if((_get_dyneither_size(_tmpBD,sizeof(char))+ _get_dyneither_size(_tmpBE,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmpBF,sizeof(char))+ 25 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0U,_tmp161.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBD);_tmp161;});struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0U,_tmp160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE);_tmp160;});struct Cyc_String_pa_PrintArg_struct _tmpC9=({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0U,_tmp15F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBF);_tmp15F;});void*_tmpC5[3U];_tmpC5[0]=& _tmpC7,_tmpC5[1]=& _tmpC8,_tmpC5[2]=& _tmpC9;({unsigned int _tmp1E9=_tmp14F->loc;struct _dyneither_ptr _tmp1E8=({const char*_tmpC6="%s was declared with type %s\n but initialized with type %s.";_tag_dyneither(_tmpC6,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp1E9,_tmp1E8,_tag_dyneither(_tmpC5,sizeof(void*),3U));});});else{
# 511
({struct Cyc_String_pa_PrintArg_struct _tmpCC=({struct Cyc_String_pa_PrintArg_struct _tmp164;_tmp164.tag=0U,_tmp164.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBD);_tmp164;});struct Cyc_String_pa_PrintArg_struct _tmpCD=({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0U,_tmp163.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE);_tmp163;});struct Cyc_String_pa_PrintArg_struct _tmpCE=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0U,_tmp162.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBF);_tmp162;});void*_tmpCA[3U];_tmpCA[0]=& _tmpCC,_tmpCA[1]=& _tmpCD,_tmpCA[2]=& _tmpCE;({unsigned int _tmp1EB=_tmp14F->loc;struct _dyneither_ptr _tmp1EA=({const char*_tmpCB="%s declared with type \n%s\n but initialized with type \n%s.";_tag_dyneither(_tmpCB,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp1EB,_tmp1EA,_tag_dyneither(_tmpCA,sizeof(void*),3U));});});}}
# 513
Cyc_Tcutil_unify(_tmpD2,(void*)_check_null(_tmpD1->topt));
Cyc_Tcutil_explain_failure();}}
# 517
Cyc_Tcstmt_tcStmt(_tmpB8,_tmp14E,0);
return;};};};}case 2U: _LL35: _tmp124=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA2)->f1;_tmp123=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA2)->f2;_tmp122=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA2)->f3;_tmp121=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA2)->f4;_LL36: {
# 525
struct Cyc_Absyn_Exp*_tmpD7=*_tmp122;
{void*_tmpD8=_tmp124->r;void*_tmpD9=_tmpD8;switch(*((int*)_tmpD9)){case 1U: _LL78: _LL79:
 goto _LL7B;case 2U: _LL7A: _LL7B:
 goto _LL7D;case 15U: _LL7C: _LL7D:
 goto _LL77;default: _LL7E: _LL7F:
# 531
 if(!Cyc_Tcutil_is_var_exp(_tmpD7)){
struct Cyc_Absyn_Vardecl*_tmpDA=({struct _tuple2*_tmp1EC=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1EC,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmpDB=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmpDC=({struct Cyc_Absyn_Pat*_tmp1F0=({void*_tmp1EE=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->tag=1U,_tmpDF->f1=_tmpDA,({struct Cyc_Absyn_Pat*_tmp1ED=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpD7->loc);_tmpDF->f2=_tmp1ED;});_tmpDF;});Cyc_Absyn_new_pat(_tmp1EE,_tmpD7->loc);});struct Cyc_Absyn_Exp*_tmp1EF=_tmpD7;Cyc_Absyn_let_decl(_tmp1F0,_tmp1EF,s0->loc);});
# 537
struct Cyc_Absyn_Stmt*_tmpDD=Cyc_Absyn_decl_stmt(_tmpDC,_tmpDB,s0->loc);
# 539
s0->r=_tmpDD->r;
({struct Cyc_Absyn_Exp*_tmp1F2=({void*_tmp1F1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=4U,_tmpDE->f1=_tmpDA;_tmpDE;});Cyc_Absyn_varb_exp(_tmp1F1,_tmpD7->loc);});*_tmp122=_tmp1F2;});
Cyc_Tcstmt_tcStmt(_tmpA0,s0,new_block);
return;}}_LL77:;}
# 545
Cyc_Tcexp_tcExpInitializer(_tmpA0,0,_tmpD7);{
# 547
void*pat_type=(void*)_check_null(_tmpD7->topt);
# 549
struct Cyc_Tcpat_TcPatResult _tmpE0=Cyc_Tcpat_tcPat(_tmpA0,_tmp124,& pat_type,_tmpD7);
({struct Cyc_Core_Opt*_tmp1F3=({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=_tmpE0.patvars;_tmpE1;});*_tmp123=_tmp1F3;});
if(!({void*_tmp1F4=(void*)_check_null(_tmp124->topt);Cyc_Tcutil_unify(_tmp1F4,(void*)_check_null(_tmpD7->topt));}) && !
Cyc_Tcutil_coerce_assign(_tmpA0,_tmpD7,(void*)_check_null(_tmp124->topt))){
({struct Cyc_String_pa_PrintArg_struct _tmpE4=({struct Cyc_String_pa_PrintArg_struct _tmp166;_tmp166.tag=0U,({
struct _dyneither_ptr _tmp1F5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp124->topt)));_tmp166.f1=_tmp1F5;});_tmp166;});struct Cyc_String_pa_PrintArg_struct _tmpE5=({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0U,({struct _dyneither_ptr _tmp1F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpD7->topt)));_tmp165.f1=_tmp1F6;});_tmp165;});void*_tmpE2[2U];_tmpE2[0]=& _tmpE4,_tmpE2[1]=& _tmpE5;({unsigned int _tmp1F8=_tmp14F->loc;struct _dyneither_ptr _tmp1F7=({const char*_tmpE3="pattern type %s does not match definition type %s";_tag_dyneither(_tmpE3,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp1F8,_tmp1F7,_tag_dyneither(_tmpE2,sizeof(void*),2U));});});
({void*_tmp1F9=(void*)_check_null(_tmp124->topt);Cyc_Tcutil_unify(_tmp1F9,(void*)_check_null(_tmpD7->topt));});
Cyc_Tcutil_explain_failure();}else{
# 559
Cyc_Tcpat_check_pat_regions(_tmpA0,_tmp124,_tmpE0.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmp124->loc,_tmpA0,_tmp124,_tmp121);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpA0,_tmpE0,_tmp14E,0,0);
return;};}case 3U: _LL37: _tmp125=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA2)->f1;_LL38: {
# 565
void*_tmpE6=Cyc_Tcenv_curr_rgn(_tmpA0);
struct Cyc_Tcenv_Tenv*_tmpE7=_tmpA0;
for(0;_tmp125 != 0;_tmp125=_tmp125->tl){
struct Cyc_Absyn_Vardecl*_tmpE8=(struct Cyc_Absyn_Vardecl*)_tmp125->hd;
struct Cyc_Absyn_Vardecl*_tmpE9=_tmpE8;union Cyc_Absyn_Nmspace _tmpEC;void*_tmpEB;void**_tmpEA;_LL81: _tmpEC=(_tmpE9->name)->f1;_tmpEB=_tmpE9->type;_tmpEA=(void**)& _tmpE9->rgn;_LL82:;
*_tmpEA=_tmpE6;
({unsigned int _tmp1FC=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FB=_tmpE7;struct Cyc_List_List*_tmp1FA=Cyc_Tcenv_lookup_type_vars(_tmpE7);Cyc_Tcutil_check_type(_tmp1FC,_tmp1FB,_tmp1FA,& Cyc_Tcutil_tmk,1,1,_tmpEB);});}
# 573
Cyc_Tcstmt_tcStmt(_tmpE7,_tmp14E,0);
return;}case 4U: _LL39: _tmp128=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA2)->f1;_tmp127=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA2)->f2;_tmp126=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA2)->f3;_LL3A:
# 579
({void*_tmp200=({void*_tmp1FF=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1FE=Cyc_Absyn_new_stmt(_tmp14E->r,_tmp14E->loc);_tmpED->f1=_tmp1FE;}),({struct Cyc_Absyn_Stmt*_tmp1FD=Cyc_Absyn_skip_stmt(_tmp14E->loc);_tmpED->f2=_tmp1FD;});_tmpED;});Cyc_Absyn_new_stmt(_tmp1FF,_tmp14E->loc);})->r;_tmp14E->r=_tmp200;});{
# 581
struct Cyc_Tcenv_Tenv*_tmpEE=_tmpA0;
struct Cyc_Absyn_Vardecl*_tmpEF=_tmp127;void**_tmp10C;void**_tmp10B;_LL84: _tmp10C=(void**)& _tmpEF->type;_tmp10B=(void**)& _tmpEF->rgn;_LL85:;{
void*_tmpF0=Cyc_Tcenv_curr_rgn(_tmpA0);
*_tmp10B=_tmpF0;{
void*rgn_typ;
if((unsigned int)_tmp126){
# 589
struct _tuple2*drname=({struct _tuple2*_tmp105=_cycalloc(sizeof(*_tmp105));((_tmp105->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp205=({struct _dyneither_ptr*_tmp100[1U];({struct _dyneither_ptr*_tmp204=({struct _dyneither_ptr*_tmp102=_cycalloc(sizeof(*_tmp102));({struct _dyneither_ptr _tmp203=({const char*_tmp101="Core";_tag_dyneither(_tmp101,sizeof(char),5U);});*_tmp102=_tmp203;});_tmp102;});_tmp100[0]=_tmp204;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp100,sizeof(struct _dyneither_ptr*),1U));});((_tmp105->f1).Abs_n).val=_tmp205;}),({
struct _dyneither_ptr*_tmp202=({struct _dyneither_ptr*_tmp104=_cycalloc(sizeof(*_tmp104));({struct _dyneither_ptr _tmp201=({const char*_tmp103="DynamicRegion";_tag_dyneither(_tmp103,sizeof(char),14U);});*_tmp104=_tmp201;});_tmp104;});_tmp105->f2=_tmp202;});_tmp105;});
void*_tmpF1=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->v=& Cyc_Tcutil_trk;_tmpFF;}),0);
({void*_tmp206=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->v=& Cyc_Tcutil_rk;_tmpF2;}),0);rgn_typ=_tmp206;});{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF3=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));
_tmpFE->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp208=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);(_tmpFE->f1).aggr_info=_tmp208;}),({
struct Cyc_List_List*_tmp207=({void*_tmpFD[1U];_tmpFD[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFD,sizeof(void*),1U));});(_tmpFE->f1).targs=_tmp207;});_tmpFE;});
void*_tmpF4=({void*_tmp20B=(void*)_tmpF3;void*_tmp20A=_tmpF1;struct Cyc_Absyn_Tqual _tmp209=
Cyc_Absyn_empty_tqual(0U);
# 596
Cyc_Absyn_at_typ(_tmp20B,_tmp20A,_tmp209,Cyc_Absyn_false_conref);});
# 598
({unsigned int _tmp20E=s0->loc;struct Cyc_Tcenv_Tenv*_tmp20D=_tmpA0;struct Cyc_List_List*_tmp20C=Cyc_Tcenv_lookup_type_vars(_tmpA0);Cyc_Tcutil_check_type(_tmp20E,_tmp20D,_tmp20C,& Cyc_Tcutil_tmk,1,0,_tmpF4);});{
void*_tmpF5=Cyc_Tcexp_tcExp(_tmpA0,& _tmpF4,_tmp126);
if(!Cyc_Tcutil_unify(_tmpF4,_tmpF5) && !
Cyc_Tcutil_coerce_assign(_tmpA0,_tmp126,_tmpF4))
({struct Cyc_String_pa_PrintArg_struct _tmpF8=({struct Cyc_String_pa_PrintArg_struct _tmp168;_tmp168.tag=0U,({
struct _dyneither_ptr _tmp20F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpF4));_tmp168.f1=_tmp20F;});_tmp168;});struct Cyc_String_pa_PrintArg_struct _tmpF9=({struct Cyc_String_pa_PrintArg_struct _tmp167;_tmp167.tag=0U,({struct _dyneither_ptr _tmp210=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpF5));_tmp167.f1=_tmp210;});_tmp167;});void*_tmpF6[2U];_tmpF6[0]=& _tmpF8,_tmpF6[1]=& _tmpF9;({unsigned int _tmp212=s0->loc;struct _dyneither_ptr _tmp211=({const char*_tmpF7="expected %s but found %s";_tag_dyneither(_tmpF7,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp212,_tmp211,_tag_dyneither(_tmpF6,sizeof(void*),2U));});});
if(!Cyc_Tcutil_unify(_tmpF1,(void*)& Cyc_Absyn_UniqueRgn_val) && !
Cyc_Tcutil_unify(_tmpF1,(void*)& Cyc_Absyn_RefCntRgn_val))
({void*_tmpFA=0U;({unsigned int _tmp214=s0->loc;struct _dyneither_ptr _tmp213=({const char*_tmpFB="open is only allowed on unique or reference-counted keys";_tag_dyneither(_tmpFB,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp214,_tmp213,_tag_dyneither(_tmpFA,sizeof(void*),0U));});});
({void*_tmp215=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=15U,_tmpFC->f1=rgn_typ;_tmpFC;});*_tmp10C=_tmp215;});
({struct Cyc_Tcenv_Tenv*_tmp216=Cyc_Tcenv_add_region(_tmpEE,rgn_typ,1);_tmpEE=_tmp216;});};};}else{
# 610
({void*_tmp217=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=2U,_tmp106->f1=_tmp128;_tmp106;});rgn_typ=_tmp217;});
({struct Cyc_Tcenv_Tenv*_tmp21A=({unsigned int _tmp219=s0->loc;struct Cyc_Tcenv_Tenv*_tmp218=_tmpEE;Cyc_Tcenv_add_type_vars(_tmp219,_tmp218,({struct Cyc_List_List*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->hd=_tmp128,_tmp107->tl=0;_tmp107;}));});_tmpEE=_tmp21A;});
({struct Cyc_Tcenv_Tenv*_tmp21B=Cyc_Tcenv_add_region(_tmpEE,rgn_typ,0);_tmpEE=_tmp21B;});}
# 614
({unsigned int _tmp21E=s0->loc;struct Cyc_Tcenv_Tenv*_tmp21D=_tmpA0;struct Cyc_List_List*_tmp21C=Cyc_Tcenv_lookup_type_vars(_tmpEE);Cyc_Tcutil_check_type(_tmp21E,_tmp21D,_tmp21C,& Cyc_Tcutil_bk,1,0,*_tmp10C);});
if(!({void*_tmp21F=*_tmp10C;Cyc_Tcutil_unify(_tmp21F,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->tag=15U,_tmp108->f1=rgn_typ;_tmp108;}));}))
({void*_tmp109=0U;({struct _dyneither_ptr _tmp220=({const char*_tmp10A="region stmt: type of region handle is wrong!";_tag_dyneither(_tmp10A,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp220,_tag_dyneither(_tmp109,sizeof(void*),0U));});});
# 618
Cyc_Tcstmt_tcStmt(_tmpEE,_tmp14E,0);
return;};};};case 1U: _LL3B: _tmp129=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA2)->f1;_LL3C: {
# 623
void*_tmp10D=Cyc_Tcenv_curr_rgn(_tmpA0);
if(_tmp129->sc != Cyc_Absyn_Public)
({void*_tmp10E=0U;({unsigned int _tmp222=_tmp14F->loc;struct _dyneither_ptr _tmp221=({const char*_tmp10F="bad storage class for inner function";_tag_dyneither(_tmp10F,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp222,_tmp221,_tag_dyneither(_tmp10E,sizeof(void*),0U));});});
Cyc_Tcutil_check_fndecl_valid_type(_tmp14F->loc,_tmpA0,_tmp129);{
void*t=Cyc_Tcutil_fndecl2typ(_tmp129);
({struct Cyc_List_List*_tmp223=Cyc_Tcutil_transfer_fn_type_atts(t,_tmp129->attributes);_tmp129->attributes=_tmp223;});
# 630
{struct Cyc_List_List*atts=_tmp129->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp110=(void*)atts->hd;void*_tmp111=_tmp110;switch(*((int*)_tmp111)){case 7U: _LL87: _LL88:
 goto _LL8A;case 6U: _LL89: _LL8A:
({struct Cyc_String_pa_PrintArg_struct _tmp114=({struct Cyc_String_pa_PrintArg_struct _tmp169;_tmp169.tag=0U,({
struct _dyneither_ptr _tmp224=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp169.f1=_tmp224;});_tmp169;});void*_tmp112[1U];_tmp112[0]=& _tmp114;({unsigned int _tmp226=_tmp14F->loc;struct _dyneither_ptr _tmp225=({const char*_tmp113="bad attribute %s for function";_tag_dyneither(_tmp113,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp226,_tmp225,_tag_dyneither(_tmp112,sizeof(void*),1U));});});
goto _LL86;default: _LL8B: _LL8C:
 goto _LL86;}_LL86:;}}{
# 638
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->sc=_tmp129->sc,_tmp115->name=_tmp129->name,_tmp115->varloc=0U,({struct Cyc_Absyn_Tqual _tmp22B=Cyc_Absyn_const_tqual(0U);_tmp115->tq=_tmp22B;}),({
void*_tmp22A=({void*_tmp229=t;void*_tmp228=_tmp10D;struct Cyc_Absyn_Tqual _tmp227=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_typ(_tmp229,_tmp228,_tmp227,Cyc_Absyn_false_conref);});_tmp115->type=_tmp22A;}),_tmp115->initializer=0,_tmp115->rgn=_tmp10D,_tmp115->attributes=0,_tmp115->escapes=0;_tmp115;});
# 642
_tmp129->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpA0->le);
({struct Cyc_Tcenv_Fenv*_tmp22C=Cyc_Tcenv_nested_fenv(_tmp14F->loc,old_fenv,_tmp129);_tmpA0->le=_tmp22C;});
Cyc_Tcstmt_tcStmt(_tmpA0,_tmp129->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpA0);
Cyc_Tcenv_check_delayed_constraints(_tmpA0);
_tmpA0->le=old_fenv;
Cyc_Tcstmt_tcStmt(_tmpA0,_tmp14E,0);
return;};};};}case 9U: _LL3D: _tmp12B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA2)->f1;_tmp12A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA2)->f2;_LL3E:
({struct _dyneither_ptr _tmp22D=({const char*_tmp116="namespace";_tag_dyneither(_tmp116,sizeof(char),10U);});unimp_msg_part=_tmp22D;});goto _LL32;case 10U: _LL3F: _tmp12D=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA2)->f1;_tmp12C=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA2)->f2;_LL40:
({struct _dyneither_ptr _tmp22E=({const char*_tmp117="using";_tag_dyneither(_tmp117,sizeof(char),6U);});unimp_msg_part=_tmp22E;});goto _LL32;case 5U: _LL41: _LL42:
({struct _dyneither_ptr _tmp22F=({const char*_tmp118="type";_tag_dyneither(_tmp118,sizeof(char),5U);});unimp_msg_part=_tmp22F;});goto _LL32;case 6U: _LL43: _LL44:
({struct _dyneither_ptr _tmp230=({const char*_tmp119="datatype";_tag_dyneither(_tmp119,sizeof(char),9U);});unimp_msg_part=_tmp230;});goto _LL32;case 7U: _LL45: _LL46:
({struct _dyneither_ptr _tmp231=({const char*_tmp11A="enum";_tag_dyneither(_tmp11A,sizeof(char),5U);});unimp_msg_part=_tmp231;});goto _LL32;case 8U: _LL47: _LL48:
({struct _dyneither_ptr _tmp232=({const char*_tmp11B="typedef";_tag_dyneither(_tmp11B,sizeof(char),8U);});unimp_msg_part=_tmp232;});goto _LL32;case 11U: _LL49: _LL4A:
({struct _dyneither_ptr _tmp233=({const char*_tmp11C="extern \"C\"";_tag_dyneither(_tmp11C,sizeof(char),11U);});unimp_msg_part=_tmp233;});goto _LL32;case 12U: _LL4B: _LL4C:
# 659
({struct _dyneither_ptr _tmp234=({const char*_tmp11D="extern \"C include\"";_tag_dyneither(_tmp11D,sizeof(char),19U);});unimp_msg_part=_tmp234;});goto _LL32;case 13U: _LL4D: _LL4E:
({struct _dyneither_ptr _tmp235=({const char*_tmp11E="__cyclone_port_on__";_tag_dyneither(_tmp11E,sizeof(char),20U);});unimp_msg_part=_tmp235;});goto _LL32;default: _LL4F: _LL50:
({struct _dyneither_ptr _tmp236=({const char*_tmp11F="__cyclone_port_off__";_tag_dyneither(_tmp11F,sizeof(char),21U);});unimp_msg_part=_tmp236;});goto _LL32;}_LL32:;}
# 663
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp130=({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0U,_tmp16A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part);_tmp16A;});void*_tmp12E[1U];_tmp12E[0]=& _tmp130;({struct _dyneither_ptr _tmp237=({const char*_tmp12F="tcStmt: nested %s declarations unimplemented";_tag_dyneither(_tmp12F,sizeof(char),45U);});((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp237,_tag_dyneither(_tmp12E,sizeof(void*),1U));});}));}}_LL0:;}
