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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 924 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 930
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 933
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 951
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 956
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 958
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 964
void*Cyc_Absyn_exn_typ();
# 973
extern void*Cyc_Absyn_bounds_one;
# 978
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 981
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1074
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1150
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1152
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 74 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 88
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 110
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 119
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 126
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
# 133
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 137
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 139
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 144
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Tcutil_copy_type(void*t);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_coerceable(void*);
# 67
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 69
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 75
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 83
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 86
int Cyc_Tcutil_is_pointer_type(void*t);
# 89
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
# 92
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 139
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 141
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 155
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 181
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 223 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 233
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 235
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 237
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 241
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 266
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 269
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 274
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple14{int f1;void*f2;};
# 283
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 310
int Cyc_Tcutil_bits_only(void*t);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 342
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 345
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 358
int Cyc_Tcutil_is_array(void*t);
# 362
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 365
int Cyc_Tcutil_zeroable_type(void*t);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 33
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 36
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 46 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 50
({void*_tmp0=0U;({unsigned int _tmp573=loc;struct _dyneither_ptr _tmp572=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp573,_tmp572,_tag_dyneither(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 54
return*topt;}}
# 63
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 67
if(topt == 0){
# 69
({void*_tmp574=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=26U,_tmp1->f1=des;_tmp1;});e->r=_tmp574;});
return;}{
# 72
void*t=*topt;
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;void*_tmpE;struct Cyc_Absyn_Tqual _tmpD;union Cyc_Absyn_AggrInfoU _tmpC;switch(*((int*)_tmp3)){case 11U: _LL1: _tmpC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3)->f1).aggr_info;_LL2:
# 75
{union Cyc_Absyn_AggrInfoU _tmp4=_tmpC;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp4.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp575=({const char*_tmp6="struct type not properly set";_tag_dyneither(_tmp6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp575,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp4.KnownAggr).val;_LLD:
({void*_tmp576=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=29U,_tmp7->f1=_tmp8->name,_tmp7->f2=0,_tmp7->f3=des,_tmp7->f4=_tmp8;_tmp7;});e->r=_tmp576;});}_LL9:;}
# 79
goto _LL0;case 8U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4:
({void*_tmp577=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=26U,_tmp9->f1=des;_tmp9;});e->r=_tmp577;});goto _LL0;case 12U: _LL5: _LL6:
({void*_tmp578=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=30U,_tmpA->f1=t,_tmpA->f2=des;_tmpA;});e->r=_tmp578;});goto _LL0;default: _LL7: _LL8:
({void*_tmp579=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp579;});goto _LL0;}_LL0:;};}
# 89
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 93
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 99
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_tmp10=_tmpF;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->f2 == 0){_LL1: _LL2:
({void*_tmp11=0U;({unsigned int _tmp57B=e->loc;struct _dyneither_ptr _tmp57A=({const char*_tmp12="assignment in test";_tag_dyneither(_tmp12,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp57B,_tmp57A,_tag_dyneither(_tmp11,sizeof(void*),0U));});});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 107
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=0U,_tmp4EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);_tmp4EA;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp4E9;_tmp4E9.tag=0U,({
struct _dyneither_ptr _tmp57C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4E9.f1=_tmp57C;});_tmp4E9;});void*_tmp13[2U];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16;({unsigned int _tmp57E=e->loc;struct _dyneither_ptr _tmp57D=({const char*_tmp14="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp14,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp57E,_tmp57D,_tag_dyneither(_tmp13,sizeof(void*),2U));});});
# 116
{void*_tmp17=e->r;void*_tmp18=_tmp17;enum Cyc_Absyn_Primop _tmp1F;struct Cyc_List_List*_tmp1E;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->tag == 3U){_LL1: _tmp1F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f1;_tmp1E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f2;_LL2:
# 118
 if(_tmp1F == Cyc_Absyn_Eq  || _tmp1F == Cyc_Absyn_Neq){
struct _tuple0 _tmp19=({struct _tuple0 _tmp4EC;_tmp4EC.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E))->hd)->topt),_tmp4EC.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E->tl))->hd)->topt);_tmp4EC;});struct _tuple0 _tmp1A=_tmp19;void*_tmp1D;void*_tmp1C;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->tag == 15U){_LL6: _tmp1D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->f1;_tmp1C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->f1;_LL7:
# 121
 return({struct Cyc_Tcexp_TestEnv _tmp4EB;({struct _tuple0*_tmp57F=({struct _tuple0*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->f1=_tmp1D,_tmp1B->f2=_tmp1C;_tmp1B;});_tmp4EB.eq=_tmp57F;}),_tmp4EB.isTrue=_tmp1F == Cyc_Absyn_Eq;_tmp4EB;});}else{goto _LL8;}}else{_LL8: _LL9:
# 123
 goto _LL5;}_LL5:;}
# 126
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 129
return({struct Cyc_Tcexp_TestEnv _tmp4ED;_tmp4ED.eq=0,_tmp4ED.isTrue=0;_tmp4ED;});}
# 150 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 155
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp20=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp21=_tmp20;struct _dyneither_ptr _tmp53;struct _dyneither_ptr _tmp52;enum Cyc_Absyn_Sign _tmp51;int _tmp50;int _tmp4F;enum Cyc_Absyn_Sign _tmp4E;enum Cyc_Absyn_Sign _tmp4D;switch((_tmp21.Wstring_c).tag){case 2U: switch(((_tmp21.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_typ;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_typ;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_typ;goto _LL0;}case 3U: _LL7: _LL8:
({void*_tmp580=Cyc_Absyn_wchar_typ();t=_tmp580;});goto _LL0;case 4U: _LL9: _tmp4D=((_tmp21.Short_c).val).f1;_LLA:
# 165
 t=_tmp4D == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL0;case 6U: _LLB: _tmp4E=((_tmp21.LongLong_c).val).f1;_LLC:
# 167
 t=_tmp4E == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL0;case 7U: _LLD: _tmp4F=((_tmp21.Float_c).val).f2;_LLE:
({void*_tmp581=Cyc_Absyn_float_typ(_tmp4F);t=_tmp581;});goto _LL0;case 5U: _LLF: _tmp51=((_tmp21.Int_c).val).f1;_tmp50=((_tmp21.Int_c).val).f2;_LL10:
# 170
 if(topt == 0)
t=_tmp51 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 177
void*_tmp22=Cyc_Tcutil_compress(*topt);void*_tmp23=_tmp22;void*_tmp37;void*_tmp36;struct Cyc_Absyn_Tqual _tmp35;void*_tmp34;union Cyc_Absyn_Constraint*_tmp33;union Cyc_Absyn_Constraint*_tmp32;union Cyc_Absyn_Constraint*_tmp31;enum Cyc_Absyn_Sign _tmp30;enum Cyc_Absyn_Sign _tmp2F;enum Cyc_Absyn_Sign _tmp2E;enum Cyc_Absyn_Sign _tmp2D;switch(*((int*)_tmp23)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f2){case Cyc_Absyn_Char_sz: _LL18: _tmp2D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL19:
# 179
{enum Cyc_Absyn_Sign _tmp24=_tmp2D;switch(_tmp24){case Cyc_Absyn_Unsigned: _LL27: _LL28:
 t=Cyc_Absyn_uchar_typ;goto _LL26;case Cyc_Absyn_Signed: _LL29: _LL2A:
 t=Cyc_Absyn_schar_typ;goto _LL26;default: _LL2B: _LL2C:
 t=Cyc_Absyn_char_typ;goto _LL26;}_LL26:;}
# 184
({union Cyc_Absyn_Cnst _tmp582=Cyc_Absyn_Char_c(_tmp2D,(char)_tmp50);*c=_tmp582;});
goto _LL17;case Cyc_Absyn_Short_sz: _LL1A: _tmp2E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL1B:
# 187
 t=_tmp2E == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp583=Cyc_Absyn_Short_c(_tmp2E,(short)_tmp50);*c=_tmp583;});
goto _LL17;case Cyc_Absyn_Int_sz: _LL1C: _tmp2F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL1D:
# 191
 t=_tmp2F == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 194
({union Cyc_Absyn_Cnst _tmp584=Cyc_Absyn_Int_c(_tmp2F,_tmp50);*c=_tmp584;});
goto _LL17;case Cyc_Absyn_Long_sz: _LL1E: _tmp30=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL1F:
# 197
 t=_tmp30 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 200
({union Cyc_Absyn_Cnst _tmp585=Cyc_Absyn_Int_c(_tmp30,_tmp50);*c=_tmp585;});
goto _LL17;default: goto _LL24;}case 5U: _LL20: _tmp36=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).elt_typ;_tmp35=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).elt_tq;_tmp34=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).rgn;_tmp33=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).nullable;_tmp32=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).bounds;_tmp31=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).zero_term;if(_tmp50 == 0){_LL21: {
# 204
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp33))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,({
struct _dyneither_ptr _tmp586=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4EE.f1=_tmp586;});_tmp4EE;});void*_tmp25[1U];_tmp25[0]=& _tmp27;({unsigned int _tmp588=e->loc;struct _dyneither_ptr _tmp587=({const char*_tmp26="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp26,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp588,_tmp587,_tag_dyneither(_tmp25,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp28=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp589=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=5U,(_tmp29->f1).elt_typ=_tmp36,(_tmp29->f1).elt_tq=_tmp35,
((_tmp29->f1).ptr_atts).rgn=_tmp34,((_tmp29->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp29->f1).ptr_atts).bounds=_tmp32,((_tmp29->f1).ptr_atts).zero_term=_tmp31,((_tmp29->f1).ptr_atts).ptrloc=0;_tmp29;});
# 210
t=_tmp589;});
# 212
goto _LL17;};}}else{goto _LL24;}case 19U: _LL22: _tmp37=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp23)->f1;_LL23: {
# 214
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2A=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=18U,({struct Cyc_Absyn_Exp*_tmp58A=Cyc_Absyn_uint_exp((unsigned int)_tmp50,0U);_tmp2C->f1=_tmp58A;});_tmp2C;});
# 221
({void*_tmp58B=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=19U,_tmp2B->f1=(void*)_tmp2A;_tmp2B;});t=_tmp58B;});
goto _LL17;}default: _LL24: _LL25:
# 224
 t=_tmp51 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL17;}_LL17:;}
# 227
goto _LL0;case 8U: _LL11: _tmp52=(_tmp21.String_c).val;_LL12:
# 229
 string_numelts=(int)_get_dyneither_size(_tmp52,sizeof(char));
_tmp53=_tmp52;goto _LL14;case 9U: _LL13: _tmp53=(_tmp21.Wstring_c).val;_LL14:
# 232
 if(string_numelts == 0){
({int _tmp58C=Cyc_Tcexp_wchar_numelts(_tmp53);string_numelts=_tmp58C;});
({void*_tmp58D=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp58D;});}{
# 236
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp58E=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp58E,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 241
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp38=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->tag=1U,_tmp3C->f1=elen;_tmp3C;});
({void*_tmp592=({void*_tmp591=string_elt_typ;struct Cyc_Absyn_Tqual _tmp590=Cyc_Absyn_const_tqual(0U);void*_tmp58F=(void*)_tmp38;Cyc_Absyn_atb_typ(_tmp591,(void*)& Cyc_Absyn_HeapRgn_val,_tmp590,_tmp58F,Cyc_Absyn_true_conref);});t=_tmp592;});
# 244
if(topt != 0){
void*_tmp39=Cyc_Tcutil_compress(*topt);void*_tmp3A=_tmp39;struct Cyc_Absyn_Tqual _tmp3B;switch(*((int*)_tmp3A)){case 8U: _LL2E: _tmp3B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A)->f1).tq;_LL2F:
# 249
 return({void*_tmp595=string_elt_typ;struct Cyc_Absyn_Tqual _tmp594=_tmp3B;struct Cyc_Absyn_Exp*_tmp593=elen;Cyc_Absyn_array_typ(_tmp595,_tmp594,_tmp593,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0U);});case 5U: _LL30: _LL31:
# 251
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 257
({void*_tmp59A=({void*_tmp599=string_elt_typ;void*_tmp598=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp597=
Cyc_Absyn_const_tqual(0U);
# 257
void*_tmp596=(void*)_tmp38;Cyc_Absyn_atb_typ(_tmp599,_tmp598,_tmp597,_tmp596,Cyc_Absyn_true_conref);});t=_tmp59A;});
# 259
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 265
goto _LL2D;default: _LL32: _LL33:
 goto _LL2D;}_LL2D:;}
# 269
return t;};};default: _LL15: _LL16:
# 271
 if(topt != 0){
void*_tmp3D=Cyc_Tcutil_compress(*topt);void*_tmp3E=_tmp3D;void*_tmp48;struct Cyc_Absyn_Tqual _tmp47;void*_tmp46;union Cyc_Absyn_Constraint*_tmp45;union Cyc_Absyn_Constraint*_tmp44;union Cyc_Absyn_Constraint*_tmp43;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->tag == 5U){_LL35: _tmp48=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).elt_typ;_tmp47=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).elt_tq;_tmp46=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).rgn;_tmp45=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).nullable;_tmp44=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).bounds;_tmp43=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).zero_term;_LL36:
# 275
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp45))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U,({
struct _dyneither_ptr _tmp59B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4EF.f1=_tmp59B;});_tmp4EF;});void*_tmp3F[1U];_tmp3F[0]=& _tmp41;({unsigned int _tmp59D=e->loc;struct _dyneither_ptr _tmp59C=({const char*_tmp40="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp40,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp59D,_tmp59C,_tag_dyneither(_tmp3F,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->tag=5U,(_tmp42->f1).elt_typ=_tmp48,(_tmp42->f1).elt_tq=_tmp47,((_tmp42->f1).ptr_atts).rgn=_tmp46,((_tmp42->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp42->f1).ptr_atts).bounds=_tmp44,((_tmp42->f1).ptr_atts).zero_term=_tmp43,((_tmp42->f1).ptr_atts).ptrloc=0;_tmp42;});}else{_LL37: _LL38:
# 280
 goto _LL34;}_LL34:;}{
# 283
struct Cyc_List_List*_tmp49=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp5A3=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=5U,({void*_tmp5A2=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->v=_tmp49;_tmp4A;}));(_tmp4C->f1).elt_typ=_tmp5A2;}),({
struct Cyc_Absyn_Tqual _tmp5A1=Cyc_Absyn_empty_tqual(0U);(_tmp4C->f1).elt_tq=_tmp5A1;}),
({void*_tmp5A0=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->v=_tmp49;_tmp4B;}));((_tmp4C->f1).ptr_atts).rgn=_tmp5A0;}),((_tmp4C->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 288
union Cyc_Absyn_Constraint*_tmp59F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4C->f1).ptr_atts).bounds=_tmp59F;}),({union Cyc_Absyn_Constraint*_tmp59E=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4C->f1).ptr_atts).zero_term=_tmp59E;}),((_tmp4C->f1).ptr_atts).ptrloc=0;_tmp4C;});
# 284
t=_tmp5A3;});
# 290
goto _LL0;};}_LL0:;}
# 292
return t;}
# 296
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 301
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp54=*((void**)_check_null(b));void*_tmp55=_tmp54;struct Cyc_Absyn_Vardecl*_tmp61;struct Cyc_Absyn_Vardecl*_tmp60;struct Cyc_Absyn_Vardecl*_tmp5F;struct Cyc_Absyn_Fndecl*_tmp5E;struct Cyc_Absyn_Vardecl*_tmp5D;struct _tuple1*_tmp5C;switch(*((int*)_tmp55)){case 0U: _LL1: _tmp5C=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp55)->f1;_LL2:
# 304
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp5A4=({const char*_tmp56="unresolved binding in tcVar";_tag_dyneither(_tmp56,sizeof(char),28U);});_tmp57->f1=_tmp5A4;});_tmp57;}));case 1U: _LL3: _tmp5D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp55)->f1;_LL4:
# 308
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp5D->name,1);
return _tmp5D->type;case 2U: _LL5: _tmp5E=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp55)->f1;_LL6:
# 315
 if(_tmp5E->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp5E->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp5E);case 5U: _LL7: _tmp5F=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp55)->f1;_LL8:
 _tmp60=_tmp5F;goto _LLA;case 4U: _LL9: _tmp60=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp55)->f1;_LLA:
 _tmp61=_tmp60;goto _LLC;default: _LLB: _tmp61=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp55)->f1;_LLC:
# 321
 if(te->allow_valueof){
void*_tmp58=Cyc_Tcutil_compress(_tmp61->type);void*_tmp59=_tmp58;void*_tmp5B;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59)->tag == 19U){_LLE: _tmp5B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59)->f1;_LLF:
# 324
({void*_tmp5A5=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=39U,_tmp5A->f1=_tmp5B;_tmp5A;});e->r=_tmp5A5;});
goto _LLD;}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 329
return _tmp61->type;}_LL0:;}
# 333
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 340
struct Cyc_List_List*desc_types;
{void*_tmp62=fmt->r;void*_tmp63=_tmp62;struct _dyneither_ptr _tmp68;struct _dyneither_ptr _tmp67;switch(*((int*)_tmp63)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp63)->f1).String_c).tag == 8){_LL1: _tmp67=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp63)->f1).String_c).val;_LL2:
 _tmp68=_tmp67;goto _LL4;}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp63)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp63)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp68=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp63)->f2)->r)->f1).String_c).val;_LL4:
# 344
({struct Cyc_List_List*_tmp5A6=type_getter(te,(struct _dyneither_ptr)_tmp68,fmt->loc);desc_types=_tmp5A6;});goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 348
 if(opt_args != 0){
struct Cyc_List_List*_tmp64=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp64 != 0;_tmp64=_tmp64->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp64->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp64->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp64->hd))
({void*_tmp65=0U;({unsigned int _tmp5A8=((struct Cyc_Absyn_Exp*)_tmp64->hd)->loc;struct _dyneither_ptr _tmp5A7=({const char*_tmp66="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp66,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp5A8,_tmp5A7,_tag_dyneither(_tmp65,sizeof(void*),0U));});});}}
# 357
return;}_LL0:;}
# 359
if(opt_args != 0){
struct Cyc_List_List*_tmp69=(struct Cyc_List_List*)opt_args->v;
# 362
for(0;desc_types != 0  && _tmp69 != 0;(
desc_types=desc_types->tl,_tmp69=_tmp69->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp69->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp4F1;_tmp4F1.tag=0U,({
struct _dyneither_ptr _tmp5A9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F1.f1=_tmp5A9;});_tmp4F1;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=0U,({struct _dyneither_ptr _tmp5AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4F0.f1=_tmp5AA;});_tmp4F0;});void*_tmp6A[2U];_tmp6A[0]=& _tmp6C,_tmp6A[1]=& _tmp6D;({unsigned int _tmp5AC=e->loc;struct _dyneither_ptr _tmp5AB=({const char*_tmp6B="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp6B,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp5AC,_tmp5AB,_tag_dyneither(_tmp6A,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 373
if(alias_coercion)
({struct Cyc_List_List*_tmp5AD=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=(void*)arg_cnt,_tmp6E->tl=*alias_arg_exps;_tmp6E;});*alias_arg_exps=_tmp5AD;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp6F=0U;({unsigned int _tmp5AF=((struct Cyc_Absyn_Exp*)_tmp69->hd)->loc;struct _dyneither_ptr _tmp5AE=({const char*_tmp70="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp70,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp5AF,_tmp5AE,_tag_dyneither(_tmp6F,sizeof(void*),0U));});});}
# 380
if(desc_types != 0)
({void*_tmp71=0U;({unsigned int _tmp5B1=fmt->loc;struct _dyneither_ptr _tmp5B0=({const char*_tmp72="too few arguments";_tag_dyneither(_tmp72,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp5B1,_tmp5B0,_tag_dyneither(_tmp71,sizeof(void*),0U));});});
if(_tmp69 != 0){
({void*_tmp73=0U;({unsigned int _tmp5B3=((struct Cyc_Absyn_Exp*)_tmp69->hd)->loc;struct _dyneither_ptr _tmp5B2=({const char*_tmp74="too many arguments";_tag_dyneither(_tmp74,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp5B3,_tmp5B2,_tag_dyneither(_tmp73,sizeof(void*),0U));});});
# 385
for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp69->hd);}}}}
# 390
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 392
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp75=p;switch(_tmp75){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 396
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp78=({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0U,({struct _dyneither_ptr _tmp5B4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F2.f1=_tmp5B4;});_tmp4F2;});void*_tmp76[1U];_tmp76[0]=& _tmp78;({unsigned int _tmp5B6=loc;struct _dyneither_ptr _tmp5B5=({const char*_tmp77="expecting numeric type but found %s";_tag_dyneither(_tmp77,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5B6,_tmp5B5,_tag_dyneither(_tmp76,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 400
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({struct _dyneither_ptr _tmp5B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F3.f1=_tmp5B7;});_tmp4F3;});void*_tmp79[1U];_tmp79[0]=& _tmp7B;({unsigned int _tmp5B9=loc;struct _dyneither_ptr _tmp5B8=({const char*_tmp7A="expecting integral or * type but found %s";_tag_dyneither(_tmp7A,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp5B9,_tmp5B8,_tag_dyneither(_tmp79,sizeof(void*),1U));});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 405
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U,({struct _dyneither_ptr _tmp5BA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F4.f1=_tmp5BA;});_tmp4F4;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp5BC=loc;struct _dyneither_ptr _tmp5BB=({const char*_tmp7D="expecting integral type but found %s";_tag_dyneither(_tmp7D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp5BC,_tmp5BB,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 409
{void*_tmp7F=t;union Cyc_Absyn_Constraint*_tmp88;switch(*((int*)_tmp7F)){case 8U: _LLE: _LLF:
 goto _LLD;case 5U: _LL10: _tmp88=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F)->f1).ptr_atts).bounds;_LL11:
# 412
{void*_tmp80=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp88);void*_tmp81=_tmp80;struct Cyc_Absyn_Exp*_tmp84;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp81)->tag == 0U){_LL15: _LL16:
 goto _LL14;}else{_LL17: _tmp84=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp81)->f1;_LL18:
# 415
 if(!Cyc_Evexp_c_can_eval(_tmp84) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp82=0U;({unsigned int _tmp5BE=loc;struct _dyneither_ptr _tmp5BD=({const char*_tmp83="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp83,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp5BE,_tmp5BD,_tag_dyneither(_tmp82,sizeof(void*),0U));});});
goto _LL14;}_LL14:;}
# 419
goto _LLD;default: _LL12: _LL13:
# 421
({struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp4F5;_tmp4F5.tag=0U,({struct _dyneither_ptr _tmp5BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F5.f1=_tmp5BF;});_tmp4F5;});void*_tmp85[1U];_tmp85[0]=& _tmp87;({unsigned int _tmp5C1=loc;struct _dyneither_ptr _tmp5C0=({const char*_tmp86="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp86,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp5C1,_tmp5C0,_tag_dyneither(_tmp85,sizeof(void*),1U));});});}_LLD:;}
# 423
return Cyc_Absyn_uint_typ;default: _LLB: _LLC:
({void*_tmp89=0U;({struct _dyneither_ptr _tmp5C2=({const char*_tmp8A="Non-unary primop";_tag_dyneither(_tmp8A,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5C2,_tag_dyneither(_tmp89,sizeof(void*),0U));});});}_LL0:;}
# 429
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 432
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp8D=({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0U,({struct _dyneither_ptr _tmp5C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4F6.f1=_tmp5C3;});_tmp4F6;});void*_tmp8B[1U];_tmp8B[0]=& _tmp8D;({unsigned int _tmp5C5=e1->loc;struct _dyneither_ptr _tmp5C4=({const char*_tmp8C="type %s cannot be used here";_tag_dyneither(_tmp8C,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5C5,_tmp5C4,_tag_dyneither(_tmp8B,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 436
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U,({struct _dyneither_ptr _tmp5C6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4F7.f1=_tmp5C6;});_tmp4F7;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({unsigned int _tmp5C8=e2->loc;struct _dyneither_ptr _tmp5C7=({const char*_tmp8F="type %s cannot be used here";_tag_dyneither(_tmp8F,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5C8,_tmp5C7,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 440
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 445
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp91=t1;void*_tmpA9;struct Cyc_Absyn_Tqual _tmpA8;void*_tmpA7;union Cyc_Absyn_Constraint*_tmpA6;union Cyc_Absyn_Constraint*_tmpA5;union Cyc_Absyn_Constraint*_tmpA4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 5U){_LL1: _tmpA9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_typ;_tmpA8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_tq;_tmpA7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).rgn;_tmpA6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).nullable;_tmpA5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).bounds;_tmpA4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).zero_term;_LL2:
# 450
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpA9),& Cyc_Tcutil_tmk))
({void*_tmp92=0U;({unsigned int _tmp5CA=e1->loc;struct _dyneither_ptr _tmp5C9=({const char*_tmp93="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp93,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5CA,_tmp5C9,_tag_dyneither(_tmp92,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp94=0U;({unsigned int _tmp5CC=e1->loc;struct _dyneither_ptr _tmp5CB=({const char*_tmp95="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp95,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5CC,_tmp5CB,_tag_dyneither(_tmp94,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmp98=({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U,({struct _dyneither_ptr _tmp5CD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F8.f1=_tmp5CD;});_tmp4F8;});void*_tmp96[1U];_tmp96[0]=& _tmp98;({unsigned int _tmp5CF=e2->loc;struct _dyneither_ptr _tmp5CE=({const char*_tmp97="expecting int but found %s";_tag_dyneither(_tmp97,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5CF,_tmp5CE,_tag_dyneither(_tmp96,sizeof(void*),1U));});});{
void*_tmp99=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA5);void*_tmp9A=_tmp99;struct Cyc_Absyn_Exp*_tmpA3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9A)->tag == 0U){_LL6: _LL7:
 return t1;}else{_LL8: _tmpA3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9A)->f1;_LL9:
# 461
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA4)){
struct _tuple15 _tmp9B=Cyc_Evexp_eval_const_uint_exp(_tmpA3);struct _tuple15 _tmp9C=_tmp9B;unsigned int _tmpA0;int _tmp9F;_LLB: _tmpA0=_tmp9C.f1;_tmp9F=_tmp9C.f2;_LLC:;
if(_tmp9F  && _tmpA0 == 1)
({void*_tmp9D=0U;({unsigned int _tmp5D1=e1->loc;struct _dyneither_ptr _tmp5D0=({const char*_tmp9E="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmp9E,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5D1,_tmp5D0,_tag_dyneither(_tmp9D,sizeof(void*),0U));});});}{
# 472
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA1=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=5U,(_tmpA2->f1).elt_typ=_tmpA9,(_tmpA2->f1).elt_tq=_tmpA8,
((_tmpA2->f1).ptr_atts).rgn=_tmpA7,((_tmpA2->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmpA2->f1).ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmpA2->f1).ptr_atts).zero_term=_tmpA4,((_tmpA2->f1).ptr_atts).ptrloc=0;_tmpA2;});
# 476
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpA1,Cyc_Absyn_Other_coercion);
return(void*)_tmpA1;};}_LL5:;};}else{_LL3: _LL4:
# 479
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 484
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,({
# 494
struct _dyneither_ptr _tmp5D2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FA.f1=_tmp5D2;});_tmp4FA;});struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,({struct _dyneither_ptr _tmp5D3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F9.f1=_tmp5D3;});_tmp4F9;});void*_tmpAA[2U];_tmpAA[0]=& _tmpAC,_tmpAA[1]=& _tmpAD;({unsigned int _tmp5D5=e1->loc;struct _dyneither_ptr _tmp5D4=({const char*_tmpAB="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpAB,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5D5,_tmp5D4,_tag_dyneither(_tmpAA,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 497
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp5D6=t1_elt;Cyc_Tcutil_unify(_tmp5D6,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,({
# 502
struct _dyneither_ptr _tmp5D7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FC.f1=_tmp5D7;});_tmp4FC;});struct Cyc_String_pa_PrintArg_struct _tmpB1=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,({struct _dyneither_ptr _tmp5D8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4FB.f1=_tmp5D8;});_tmp4FB;});void*_tmpAE[2U];_tmpAE[0]=& _tmpB0,_tmpAE[1]=& _tmpB1;({unsigned int _tmp5DA=e1->loc;struct _dyneither_ptr _tmp5D9=({const char*_tmpAF="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpAF,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5DA,_tmp5D9,_tag_dyneither(_tmpAE,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 506
({void*_tmpB2=0U;({unsigned int _tmp5DC=e1->loc;struct _dyneither_ptr _tmp5DB=({const char*_tmpB3="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpB3,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5DC,_tmp5DB,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5E1=te;struct Cyc_Absyn_Exp*_tmp5E0=e1;Cyc_Tcutil_unchecked_cast(_tmp5E1,_tmp5E0,({void*_tmp5DF=t1_elt;void*_tmp5DE=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5DD=
Cyc_Absyn_empty_tqual(0U);
# 507
Cyc_Absyn_star_typ(_tmp5DF,_tmp5DE,_tmp5DD,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 510
return Cyc_Absyn_sint_typ;}else{
# 512
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpB4=0U;({unsigned int _tmp5E3=e1->loc;struct _dyneither_ptr _tmp5E2=({const char*_tmpB5="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpB5,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5E3,_tmp5E2,_tag_dyneither(_tmpB4,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpB6=0U;({unsigned int _tmp5E5=e1->loc;struct _dyneither_ptr _tmp5E4=({const char*_tmpB7="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpB7,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5E5,_tmp5E4,_tag_dyneither(_tmpB6,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpBA=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,({
struct _dyneither_ptr _tmp5E6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FE.f1=_tmp5E6;});_tmp4FE;});struct Cyc_String_pa_PrintArg_struct _tmpBB=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,({struct _dyneither_ptr _tmp5E7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4FD.f1=_tmp5E7;});_tmp4FD;});void*_tmpB8[2U];_tmpB8[0]=& _tmpBA,_tmpB8[1]=& _tmpBB;({unsigned int _tmp5E9=e2->loc;struct _dyneither_ptr _tmp5E8=({const char*_tmpB9="expecting either %s or int but found %s";_tag_dyneither(_tmpB9,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5E9,_tmp5E8,_tag_dyneither(_tmpB8,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 521
return t1;}}}
# 525
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5EA=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5EA,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpBC=0U;({unsigned int _tmp5EC=e1->loc;struct _dyneither_ptr _tmp5EB=({const char*_tmpBD="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpBD,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5EC,_tmp5EB,_tag_dyneither(_tmpBC,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5F1=te;struct Cyc_Absyn_Exp*_tmp5F0=e2;Cyc_Tcutil_unchecked_cast(_tmp5F1,_tmp5F0,({void*_tmp5EF=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5EE=
Cyc_Tcutil_pointer_region(t2);
# 530
struct Cyc_Absyn_Tqual _tmp5ED=
# 532
Cyc_Absyn_empty_tqual(0U);
# 530
Cyc_Absyn_star_typ(_tmp5EF,_tmp5EE,_tmp5ED,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 535
({void*_tmpBE=0U;({unsigned int _tmp5F3=e1->loc;struct _dyneither_ptr _tmp5F2=({const char*_tmpBF="thin pointer subtraction!";_tag_dyneither(_tmpBF,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5F3,_tmp5F2,_tag_dyneither(_tmpBE,sizeof(void*),0U));});});
return Cyc_Absyn_sint_typ;}
# 538
({void*_tmpC0=0U;({unsigned int _tmp5F5=e1->loc;struct _dyneither_ptr _tmp5F4=({const char*_tmpC1="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpC1,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5F5,_tmp5F4,_tag_dyneither(_tmpC0,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 541
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpC2=0U;({unsigned int _tmp5F7=e1->loc;struct _dyneither_ptr _tmp5F6=({const char*_tmpC3="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpC3,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp5F7,_tmp5F6,_tag_dyneither(_tmpC2,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 546
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 549
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 557
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || ({
void*_tmp5F8=t1;Cyc_Tcutil_unify(_tmp5F8,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 562
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 575
pointer_cmp: {
struct _tuple0 _tmpC4=({struct _tuple0 _tmp4FF;({void*_tmp5FA=Cyc_Tcutil_compress(t1);_tmp4FF.f1=_tmp5FA;}),({void*_tmp5F9=Cyc_Tcutil_compress(t2);_tmp4FF.f2=_tmp5F9;});_tmp4FF;});struct _tuple0 _tmpC5=_tmpC4;void*_tmpC7;void*_tmpC6;switch(*((int*)_tmpC5.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC5.f2)->tag == 5U){_LL1: _tmpC7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC5.f1)->f1).elt_typ;_tmpC6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC5.f2)->f1).elt_typ;_LL2:
# 579
 if(Cyc_Tcutil_unify(_tmpC7,_tmpC6))return Cyc_Absyn_sint_typ;goto _LL0;}else{goto _LL5;}case 15U: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC5.f2)->tag == 15U){_LL3: _LL4:
# 581
 return Cyc_Absyn_sint_typ;}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 584
({struct Cyc_String_pa_PrintArg_struct _tmpCA=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,({
struct _dyneither_ptr _tmp5FB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp501.f1=_tmp5FB;});_tmp501;});struct Cyc_String_pa_PrintArg_struct _tmpCB=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,({struct _dyneither_ptr _tmp5FC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp500.f1=_tmp5FC;});_tmp500;});void*_tmpC8[2U];_tmpC8[0]=& _tmpCA,_tmpC8[1]=& _tmpCB;({unsigned int _tmp5FE=loc;struct _dyneither_ptr _tmp5FD=({const char*_tmpC9="comparison not allowed between %s and %s";_tag_dyneither(_tmpC9,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5FE,_tmp5FD,_tag_dyneither(_tmpC8,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 591
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 593
if(({void*_tmp5FF=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5FF == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 599
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpCC=({struct _tuple0 _tmp502;_tmp502.f1=t1,_tmp502.f2=t2;_tmp502;});struct _tuple0 _tmpCD=_tmpCC;void*_tmpCF;void*_tmpCE;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpCD.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpCD.f2)->tag == 15U){_LL1: _tmpCF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpCD.f1)->f1;_tmpCE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpCD.f2)->f1;_LL2:
# 604
 return Cyc_Absyn_sint_typ;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 607
({struct Cyc_String_pa_PrintArg_struct _tmpD2=({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U,({
struct _dyneither_ptr _tmp600=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp504.f1=_tmp600;});_tmp504;});struct Cyc_String_pa_PrintArg_struct _tmpD3=({struct Cyc_String_pa_PrintArg_struct _tmp503;_tmp503.tag=0U,({struct _dyneither_ptr _tmp601=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp503.f1=_tmp601;});_tmp503;});void*_tmpD0[2U];_tmpD0[0]=& _tmpD2,_tmpD0[1]=& _tmpD3;({unsigned int _tmp603=loc;struct _dyneither_ptr _tmp602=({const char*_tmpD1="comparison not allowed between %s and %s";_tag_dyneither(_tmpD1,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp603,_tmp602,_tag_dyneither(_tmpD0,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 615
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 617
enum Cyc_Absyn_Primop _tmpD4=p;switch(_tmpD4){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 621
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 624
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Bitarshift: _LL15: _LL16:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL17: _LL18:
# 634
 goto _LL1A;case Cyc_Absyn_Neq: _LL19: _LL1A:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LL1B: _LL1C:
# 637
 goto _LL1E;case Cyc_Absyn_Lt: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Gte: _LL1F: _LL20:
 goto _LL22;case Cyc_Absyn_Lte: _LL21: _LL22:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LL23: _LL24:
# 642
({void*_tmpD5=0U;({struct _dyneither_ptr _tmp604=({const char*_tmpD6="bad binary primop";_tag_dyneither(_tmpD6,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp604,_tag_dyneither(_tmpD5,sizeof(void*),0U));});});}_LL0:;}
# 646
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 654
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpD7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpD8=Cyc_Tcexp_tcExp(te,topt,_tmpD7);
if(!Cyc_Tcutil_is_numeric(_tmpD7))
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({struct _dyneither_ptr _tmp605=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD8));_tmp505.f1=_tmp605;});_tmp505;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({unsigned int _tmp607=_tmpD7->loc;struct _dyneither_ptr _tmp606=({const char*_tmpDA="expecting numeric type but found %s";_tag_dyneither(_tmpDA,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp607,_tmp606,_tag_dyneither(_tmpD9,sizeof(void*),1U));});});
return _tmpD8;}
# 661
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpDC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpDD=_tmpDC;switch(_tmpDD){case 0U: _LL1: _LL2:
 return({void*_tmpDE=0U;({struct Cyc_Tcenv_Tenv*_tmp60B=te;unsigned int _tmp60A=loc;void**_tmp609=topt;struct _dyneither_ptr _tmp608=({const char*_tmpDF="primitive operator has 0 arguments";_tag_dyneither(_tmpDF,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp60B,_tmp60A,_tmp609,_tmp608,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
({void*_tmp60C=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp60C;});goto _LL0;case 2U: _LL5: _LL6:
({void*_tmp612=({struct Cyc_Tcenv_Tenv*_tmp611=te;unsigned int _tmp610=loc;void**_tmp60F=topt;enum Cyc_Absyn_Primop _tmp60E=p;struct Cyc_Absyn_Exp*_tmp60D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp611,_tmp610,_tmp60F,_tmp60E,_tmp60D,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp612;});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpE0=0U;({struct Cyc_Tcenv_Tenv*_tmp616=te;unsigned int _tmp615=loc;void**_tmp614=topt;struct _dyneither_ptr _tmp613=({const char*_tmpE1="primitive operator has > 2 arguments";_tag_dyneither(_tmpE1,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp616,_tmp615,_tmp614,_tmp613,_tag_dyneither(_tmpE0,sizeof(void*),0U));});});}_LL0:;}
# 669
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 672
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpE2=Cyc_Tcutil_compress(t);
void*_tmpE3=_tmpE2;struct Cyc_List_List*_tmpFF;void*_tmpFE;struct Cyc_Absyn_Tqual _tmpFD;struct Cyc_Absyn_Datatypefield*_tmpFC;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Aggrdecl*_tmpFA;switch(*((int*)_tmpE3)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE3)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmpFA=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE3)->f1).aggr_info).KnownAggr).val;_LL2:
# 676
 if(_tmpFA->impl == 0){
({void*_tmpE4=0U;({unsigned int _tmp618=loc;struct _dyneither_ptr _tmp617=({const char*_tmpE5="attempt to write an abstract aggregate";_tag_dyneither(_tmpE5,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp618,_tmp617,_tag_dyneither(_tmpE4,sizeof(void*),0U));});});
return 0;}else{
# 680
_tmpFB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFA->impl))->fields;goto _LL4;}}else{goto _LLB;}case 12U: _LL3: _tmpFB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE3)->f2;_LL4:
# 682
 for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){
struct Cyc_Absyn_Aggrfield*_tmpE6=(struct Cyc_Absyn_Aggrfield*)_tmpFB->hd;
if((_tmpE6->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpE9=({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U,_tmp506.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE6->name);_tmp506;});void*_tmpE7[1U];_tmpE7[0]=& _tmpE9;({unsigned int _tmp61A=loc;struct _dyneither_ptr _tmp619=({const char*_tmpE8="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpE8,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp61A,_tmp619,_tag_dyneither(_tmpE7,sizeof(void*),1U));});});
return 0;}
# 688
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpE6->type))return 0;}
# 690
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE3)->f1).field_info).KnownDatatypefield).tag == 2){_LL5: _tmpFC=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE3)->f1).field_info).KnownDatatypefield).val).f2;_LL6:
# 692
{struct Cyc_List_List*_tmpEA=_tmpFC->typs;for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){
struct _tuple17*_tmpEB=(struct _tuple17*)_tmpEA->hd;struct _tuple17*_tmpEC=_tmpEB;struct Cyc_Absyn_Tqual _tmpF1;void*_tmpF0;_LLE: _tmpF1=_tmpEC->f1;_tmpF0=_tmpEC->f2;_LLF:;
if(_tmpF1.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpEF=({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0U,({struct _dyneither_ptr _tmp61B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpFC->name));_tmp507.f1=_tmp61B;});_tmp507;});void*_tmpED[1U];_tmpED[0]=& _tmpEF;({unsigned int _tmp61D=loc;struct _dyneither_ptr _tmp61C=({const char*_tmpEE="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpEE,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp61D,_tmp61C,_tag_dyneither(_tmpED,sizeof(void*),1U));});});
return 0;}
# 698
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF0))return 0;}}
# 700
return 1;}else{goto _LLB;}case 8U: _LL7: _tmpFE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE3)->f1).elt_type;_tmpFD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE3)->f1).tq;_LL8:
# 702
 if(_tmpFD.real_const){
({void*_tmpF2=0U;({unsigned int _tmp61F=loc;struct _dyneither_ptr _tmp61E=({const char*_tmpF3="attempt to over-write a const array";_tag_dyneither(_tmpF3,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp61F,_tmp61E,_tag_dyneither(_tmpF2,sizeof(void*),0U));});});
return 0;}
# 706
return Cyc_Tcexp_check_writable_aggr(loc,_tmpFE);case 10U: _LL9: _tmpFF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE3)->f1;_LLA:
# 708
 for(0;_tmpFF != 0;_tmpFF=_tmpFF->tl){
struct _tuple17*_tmpF4=(struct _tuple17*)_tmpFF->hd;struct _tuple17*_tmpF5=_tmpF4;struct Cyc_Absyn_Tqual _tmpF9;void*_tmpF8;_LL11: _tmpF9=_tmpF5->f1;_tmpF8=_tmpF5->f2;_LL12:;
if(_tmpF9.real_const){
({void*_tmpF6=0U;({unsigned int _tmp621=loc;struct _dyneither_ptr _tmp620=({const char*_tmpF7="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmpF7,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp621,_tmp620,_tag_dyneither(_tmpF6,sizeof(void*),0U));});});
return 0;}
# 714
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF8))return 0;}
# 716
return 1;default: _LLB: _LLC:
 return 1;}_LL0:;}
# 724
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 727
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp100=e->r;void*_tmp101=_tmp100;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct _dyneither_ptr*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct _dyneither_ptr*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Vardecl*_tmp127;struct Cyc_Absyn_Vardecl*_tmp126;struct Cyc_Absyn_Vardecl*_tmp125;struct Cyc_Absyn_Vardecl*_tmp124;switch(*((int*)_tmp101)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp101)->f1)){case 3U: _LL1: _tmp124=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp101)->f1)->f1;_LL2:
 _tmp125=_tmp124;goto _LL4;case 4U: _LL3: _tmp125=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp101)->f1)->f1;_LL4:
 _tmp126=_tmp125;goto _LL6;case 5U: _LL5: _tmp126=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp101)->f1)->f1;_LL6:
 _tmp127=_tmp126;goto _LL8;case 1U: _LL7: _tmp127=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp101)->f1)->f1;_LL8:
 if(!(_tmp127->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp129=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp101)->f1;_tmp128=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp101)->f2;_LLA:
# 734
{void*_tmp102=Cyc_Tcutil_compress((void*)_check_null(_tmp129->topt));void*_tmp103=_tmp102;struct Cyc_List_List*_tmp114;struct Cyc_Absyn_Tqual _tmp113;struct Cyc_Absyn_Tqual _tmp112;switch(*((int*)_tmp103)){case 5U: _LL18: _tmp112=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp103)->f1).elt_tq;_LL19:
 _tmp113=_tmp112;goto _LL1B;case 8U: _LL1A: _tmp113=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp103)->f1).tq;_LL1B:
 if(!_tmp113.real_const)return;goto _LL17;case 10U: _LL1C: _tmp114=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp103)->f1;_LL1D: {
# 738
struct _tuple15 _tmp104=Cyc_Evexp_eval_const_uint_exp(_tmp128);struct _tuple15 _tmp105=_tmp104;unsigned int _tmp111;int _tmp110;_LL21: _tmp111=_tmp105.f1;_tmp110=_tmp105.f2;_LL22:;
if(!_tmp110){
({void*_tmp106=0U;({unsigned int _tmp623=e->loc;struct _dyneither_ptr _tmp622=({const char*_tmp107="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp107,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp623,_tmp622,_tag_dyneither(_tmp106,sizeof(void*),0U));});});
return;}
# 743
{struct _handler_cons _tmp108;_push_handler(& _tmp108);{int _tmp10A=0;if(setjmp(_tmp108.handler))_tmp10A=1;if(!_tmp10A){
{struct _tuple17*_tmp10B=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp114,(int)_tmp111);struct _tuple17*_tmp10C=_tmp10B;struct Cyc_Absyn_Tqual _tmp10D;_LL24: _tmp10D=_tmp10C->f1;_LL25:;
if(!_tmp10D.real_const){_npop_handler(0U);return;}}
# 744
;_pop_handler();}else{void*_tmp109=(void*)_exn_thrown;void*_tmp10E=_tmp109;void*_tmp10F;if(((struct Cyc_List_Nth_exn_struct*)_tmp10E)->tag == Cyc_List_Nth){_LL27: _LL28:
# 746
 return;}else{_LL29: _tmp10F=_tmp10E;_LL2A:(int)_rethrow(_tmp10F);}_LL26:;}};}
goto _LL17;}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 750
goto _LL0;case 21U: _LLB: _tmp12B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp101)->f1;_tmp12A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp101)->f2;_LLC:
# 752
{void*_tmp115=Cyc_Tcutil_compress((void*)_check_null(_tmp12B->topt));void*_tmp116=_tmp115;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_Aggrdecl**_tmp117;switch(*((int*)_tmp116)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp116)->f1).aggr_info).KnownAggr).tag == 2){_LL2C: _tmp117=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp116)->f1).aggr_info).KnownAggr).val;_LL2D: {
# 754
struct Cyc_Absyn_Aggrfield*sf=
_tmp117 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp117,_tmp12A);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}case 12U: _LL2E: _tmp118=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp116)->f2;_LL2F: {
# 759
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp118,_tmp12A);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 764
goto _LL0;case 22U: _LLD: _tmp12D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp101)->f1;_tmp12C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp101)->f2;_LLE:
# 766
{void*_tmp119=Cyc_Tcutil_compress((void*)_check_null(_tmp12D->topt));void*_tmp11A=_tmp119;void*_tmp11F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11A)->tag == 5U){_LL33: _tmp11F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11A)->f1).elt_typ;_LL34:
# 768
{void*_tmp11B=Cyc_Tcutil_compress(_tmp11F);void*_tmp11C=_tmp11B;struct Cyc_List_List*_tmp11E;struct Cyc_Absyn_Aggrdecl**_tmp11D;switch(*((int*)_tmp11C)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11C)->f1).aggr_info).KnownAggr).tag == 2){_LL38: _tmp11D=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11C)->f1).aggr_info).KnownAggr).val;_LL39: {
# 770
struct Cyc_Absyn_Aggrfield*sf=
_tmp11D == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp11D,_tmp12C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}case 12U: _LL3A: _tmp11E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11C)->f2;_LL3B: {
# 775
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp11E,_tmp12C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 780
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 783
goto _LL0;case 20U: _LLF: _tmp12E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp101)->f1;_LL10:
# 785
{void*_tmp120=Cyc_Tcutil_compress((void*)_check_null(_tmp12E->topt));void*_tmp121=_tmp120;struct Cyc_Absyn_Tqual _tmp123;struct Cyc_Absyn_Tqual _tmp122;switch(*((int*)_tmp121)){case 5U: _LL3F: _tmp122=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp121)->f1).elt_tq;_LL40:
 _tmp123=_tmp122;goto _LL42;case 8U: _LL41: _tmp123=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp121)->f1).tq;_LL42:
 if(!_tmp123.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 790
goto _LL0;case 12U: _LL11: _tmp12F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp101)->f1;_LL12:
 _tmp130=_tmp12F;goto _LL14;case 13U: _LL13: _tmp130=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp101)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp130);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 795
({struct Cyc_String_pa_PrintArg_struct _tmp133=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({struct _dyneither_ptr _tmp624=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp508.f1=_tmp624;});_tmp508;});void*_tmp131[1U];_tmp131[0]=& _tmp133;({unsigned int _tmp626=e->loc;struct _dyneither_ptr _tmp625=({const char*_tmp132="attempt to write a const location: %s";_tag_dyneither(_tmp132,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp626,_tmp625,_tag_dyneither(_tmp131,sizeof(void*),1U));});});}
# 798
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 801
({struct Cyc_Tcenv_Tenv*_tmp627=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp627,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp134=0U;({struct Cyc_Tcenv_Tenv*_tmp62B=te;unsigned int _tmp62A=loc;void**_tmp629=topt;struct _dyneither_ptr _tmp628=({const char*_tmp135="increment/decrement of non-lvalue";_tag_dyneither(_tmp135,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp62B,_tmp62A,_tmp629,_tmp628,_tag_dyneither(_tmp134,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 807
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp136=0U;({unsigned int _tmp62D=e->loc;struct _dyneither_ptr _tmp62C=({const char*_tmp137="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp137,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp62D,_tmp62C,_tag_dyneither(_tmp136,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp138=0U;({unsigned int _tmp62F=e->loc;struct _dyneither_ptr _tmp62E=({const char*_tmp139="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp139,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp62F,_tmp62E,_tag_dyneither(_tmp138,sizeof(void*),0U));});});}else{
# 816
({struct Cyc_String_pa_PrintArg_struct _tmp13C=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({struct _dyneither_ptr _tmp630=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp509.f1=_tmp630;});_tmp509;});void*_tmp13A[1U];_tmp13A[0]=& _tmp13C;({unsigned int _tmp632=e->loc;struct _dyneither_ptr _tmp631=({const char*_tmp13B="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp13B,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp632,_tmp631,_tag_dyneither(_tmp13A,sizeof(void*),1U));});});}}
# 818
return t;};}
# 822
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 824
({struct Cyc_Tcenv_Tenv*_tmp634=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp633=e1;Cyc_Tcexp_tcTest(_tmp634,_tmp633,({const char*_tmp13D="conditional expression";_tag_dyneither(_tmp13D,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp635=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp635;});else{
# 831
({void*_tmp636=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp636;});}{
struct Cyc_List_List _tmp13E=({struct Cyc_List_List _tmp50D;_tmp50D.hd=e3,_tmp50D.tl=0;_tmp50D;});
struct Cyc_List_List _tmp13F=({struct Cyc_List_List _tmp50C;_tmp50C.hd=e2,_tmp50C.tl=& _tmp13E;_tmp50C;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp13F)){
({struct Cyc_String_pa_PrintArg_struct _tmp142=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,({
struct _dyneither_ptr _tmp637=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp50B.f1=_tmp637;});_tmp50B;});struct Cyc_String_pa_PrintArg_struct _tmp143=({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0U,({struct _dyneither_ptr _tmp638=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp50A.f1=_tmp638;});_tmp50A;});void*_tmp140[2U];_tmp140[0]=& _tmp142,_tmp140[1]=& _tmp143;({unsigned int _tmp63A=loc;struct _dyneither_ptr _tmp639=({const char*_tmp141="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp141,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp63A,_tmp639,_tag_dyneither(_tmp140,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 839
return t;};};}
# 843
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 845
({struct Cyc_Tcenv_Tenv*_tmp63C=te;struct Cyc_Absyn_Exp*_tmp63B=e1;Cyc_Tcexp_tcTest(_tmp63C,_tmp63B,({const char*_tmp144="logical-and expression";_tag_dyneither(_tmp144,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp63E=te;struct Cyc_Absyn_Exp*_tmp63D=e2;Cyc_Tcexp_tcTest(_tmp63E,_tmp63D,({const char*_tmp145="logical-and expression";_tag_dyneither(_tmp145,sizeof(char),23U);}));});
return Cyc_Absyn_sint_typ;}
# 851
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 853
({struct Cyc_Tcenv_Tenv*_tmp640=te;struct Cyc_Absyn_Exp*_tmp63F=e1;Cyc_Tcexp_tcTest(_tmp640,_tmp63F,({const char*_tmp146="logical-or expression";_tag_dyneither(_tmp146,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp642=te;struct Cyc_Absyn_Exp*_tmp641=e2;Cyc_Tcexp_tcTest(_tmp642,_tmp641,({const char*_tmp147="logical-or expression";_tag_dyneither(_tmp147,sizeof(char),22U);}));});
return Cyc_Absyn_sint_typ;}
# 859
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 866
({struct Cyc_Tcenv_Tenv*_tmp643=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp643,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 871
{void*_tmp148=Cyc_Tcutil_compress(t1);void*_tmp149=_tmp148;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->tag == 8U){_LL1: _LL2:
({void*_tmp14A=0U;({unsigned int _tmp645=loc;struct _dyneither_ptr _tmp644=({const char*_tmp14B="cannot assign to an array";_tag_dyneither(_tmp14B,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp645,_tmp644,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 876
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp14C=0U;({unsigned int _tmp647=loc;struct _dyneither_ptr _tmp646=({const char*_tmp14D="type is abstract (can't determine size).";_tag_dyneither(_tmp14D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp647,_tmp646,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});
# 880
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp14E=0U;({struct Cyc_Tcenv_Tenv*_tmp64B=te;unsigned int _tmp64A=loc;void**_tmp649=topt;struct _dyneither_ptr _tmp648=({const char*_tmp14F="assignment to non-lvalue";_tag_dyneither(_tmp14F,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp64B,_tmp64A,_tmp649,_tmp648,_tag_dyneither(_tmp14E,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp150=0U;({unsigned int _tmp64D=e2->loc;struct _dyneither_ptr _tmp64C=({const char*_tmp151="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp151,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp64D,_tmp64C,_tag_dyneither(_tmp150,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp155=({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0U,({
struct _dyneither_ptr _tmp64E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp50F.f1=_tmp64E;});_tmp50F;});struct Cyc_String_pa_PrintArg_struct _tmp156=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0U,({struct _dyneither_ptr _tmp64F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp50E.f1=_tmp64F;});_tmp50E;});void*_tmp153[2U];_tmp153[0]=& _tmp155,_tmp153[1]=& _tmp156;({struct Cyc_Tcenv_Tenv*_tmp653=te;unsigned int _tmp652=loc;void**_tmp651=topt;struct _dyneither_ptr _tmp650=({const char*_tmp154="type mismatch: %s != %s";_tag_dyneither(_tmp154,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp653,_tmp652,_tmp651,_tmp650,_tag_dyneither(_tmp153,sizeof(void*),2U));});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp152;}}else{
# 894
enum Cyc_Absyn_Primop _tmp157=(enum Cyc_Absyn_Primop)po->v;
void*_tmp158=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp157,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp158,t1) || Cyc_Tcutil_coerceable(_tmp158) && Cyc_Tcutil_coerceable(t1))){
void*_tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0U,({
# 899
struct _dyneither_ptr _tmp654=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp511.f1=_tmp654;});_tmp511;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0U,({
struct _dyneither_ptr _tmp655=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp510.f1=_tmp655;});_tmp510;});void*_tmp15A[2U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D;({struct Cyc_Tcenv_Tenv*_tmp659=te;unsigned int _tmp658=loc;void**_tmp657=topt;struct _dyneither_ptr _tmp656=({const char*_tmp15B="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp15B,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp659,_tmp658,_tmp657,_tmp656,_tag_dyneither(_tmp15A,sizeof(void*),2U));});});
Cyc_Tcutil_unify(_tmp158,t1);
Cyc_Tcutil_explain_failure();
return _tmp159;}
# 905
return _tmp158;}
# 907
return t1;};};}
# 911
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp65A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp65A,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp65C=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp65B=topt;Cyc_Tcexp_tcExp(_tmp65C,_tmp65B,e2);});
return(void*)_check_null(e2->topt);}
# 918
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 921
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 924
{void*_tmp15E=Cyc_Tcutil_compress(t1);void*_tmp15F=_tmp15E;switch(*((int*)_tmp15F)){case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp15F)->f1 == 0){_LL1: _LL2:
({struct Cyc_Tcenv_Tenv*_tmp65E=te;struct Cyc_Absyn_Exp*_tmp65D=e;Cyc_Tcutil_unchecked_cast(_tmp65E,_tmp65D,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp65F=Cyc_Absyn_float_typ(1);t1=_tmp65F;});goto _LL0;}else{goto _LL7;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp15F)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL0;default: goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 931
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp160=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp161=_tmp160;struct _tuple1*_tmp165;struct Cyc_List_List*_tmp164;unsigned int _tmp163;enum Cyc_Absyn_Scope _tmp162;_LLA: _tmp165=_tmp161.name;_tmp164=_tmp161.typs;_tmp163=_tmp161.loc;_tmp162=_tmp161.sc;_LLB:;
# 934
if(_tmp164 == 0  || _tmp164->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp164->hd)).f2);
# 937
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 942
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp166=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp167=_tmp166;struct _tuple1*_tmp16B;struct Cyc_List_List*_tmp16A;unsigned int _tmp169;enum Cyc_Absyn_Scope _tmp168;_LLD: _tmp16B=_tmp167.name;_tmp16A=_tmp167.typs;_tmp169=_tmp167.loc;_tmp168=_tmp167.sc;_LLE:;
# 945
if(_tmp16A == 0  || _tmp16A->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp16A->hd)).f2);
# 949
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 954
({struct Cyc_String_pa_PrintArg_struct _tmp16E=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,({
struct _dyneither_ptr _tmp660=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp513.f1=_tmp660;});_tmp513;});struct Cyc_String_pa_PrintArg_struct _tmp16F=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0U,({struct _dyneither_ptr _tmp661=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp512.f1=_tmp661;});_tmp512;});void*_tmp16C[2U];_tmp16C[0]=& _tmp16E,_tmp16C[1]=& _tmp16F;({unsigned int _tmp663=e->loc;struct _dyneither_ptr _tmp662=({const char*_tmp16D="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp16D,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp663,_tmp662,_tag_dyneither(_tmp16C,sizeof(void*),2U));});});
return 0;}
# 960
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 966
struct Cyc_List_List*_tmp170=args;
int _tmp171=0;
struct Cyc_Tcenv_Tenv*_tmp172=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp173=Cyc_Tcenv_clear_abstract_val_ok(_tmp172);
Cyc_Tcexp_tcExp(_tmp173,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 975
void*_tmp174=t;void*_tmp1D7;struct Cyc_Absyn_Tqual _tmp1D6;void*_tmp1D5;union Cyc_Absyn_Constraint*_tmp1D4;union Cyc_Absyn_Constraint*_tmp1D3;union Cyc_Absyn_Constraint*_tmp1D2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->tag == 5U){_LL1: _tmp1D7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).elt_typ;_tmp1D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).elt_tq;_tmp1D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).ptr_atts).rgn;_tmp1D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).ptr_atts).nullable;_tmp1D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).ptr_atts).bounds;_tmp1D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).ptr_atts).zero_term;_LL2:
# 980
 Cyc_Tcenv_check_rgn_accessible(_tmp173,loc,_tmp1D5);
# 982
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1D3);{
void*_tmp175=Cyc_Tcutil_compress(_tmp1D7);void*_tmp176=_tmp175;struct Cyc_List_List*_tmp1CE;void*_tmp1CD;struct Cyc_Absyn_Tqual _tmp1CC;void*_tmp1CB;struct Cyc_List_List*_tmp1CA;int _tmp1C9;struct Cyc_Absyn_VarargInfo*_tmp1C8;struct Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_List_List*_tmp1C2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->tag == 9U){_LL6: _tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).tvars;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).effect;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).ret_tqual;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).ret_typ;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).args;_tmp1C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).c_varargs;_tmp1C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).cyc_varargs;_tmp1C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).rgn_po;_tmp1C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).attributes;_tmp1C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).requires_clause;_tmp1C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).requires_relns;_tmp1C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).ensures_clause;_tmp1C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp176)->f1).ensures_relns;_LL7:
# 987
 if(_tmp1CE != 0  || _tmp1C7 != 0)
({void*_tmp177=0U;({unsigned int _tmp665=e->loc;struct _dyneither_ptr _tmp664=({const char*_tmp178="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp178,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp665,_tmp664,_tag_dyneither(_tmp177,sizeof(void*),0U));});});
# 991
if(topt != 0)Cyc_Tcutil_unify(_tmp1CB,*topt);
# 993
while(_tmp170 != 0  && _tmp1CA != 0){
# 995
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp170->hd;
void*t2=(*((struct _tuple9*)_tmp1CA->hd)).f3;
Cyc_Tcexp_tcExp(_tmp173,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp173,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp188="actual argument has type ";_tag_dyneither(_tmp188,sizeof(char),26U);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp187=" but formal has type ";_tag_dyneither(_tmp187,sizeof(char),22U);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp668=({unsigned long _tmp667=({unsigned long _tmp666=Cyc_strlen((struct _dyneither_ptr)s0);_tmp666 + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp667 + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp668 + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp179=0U;({unsigned int _tmp66B=e1->loc;struct _dyneither_ptr _tmp66A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17C=({struct Cyc_String_pa_PrintArg_struct _tmp517;_tmp517.tag=0U,_tmp517.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp517;});struct Cyc_String_pa_PrintArg_struct _tmp17D=({struct Cyc_String_pa_PrintArg_struct _tmp516;_tmp516.tag=0U,_tmp516.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp516;});struct Cyc_String_pa_PrintArg_struct _tmp17E=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp515;});struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,_tmp514.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp514;});void*_tmp17A[4U];_tmp17A[0]=& _tmp17C,_tmp17A[1]=& _tmp17D,_tmp17A[2]=& _tmp17E,_tmp17A[3]=& _tmp17F;({struct _dyneither_ptr _tmp669=({const char*_tmp17B="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp17B,sizeof(char),15U);});Cyc_aprintf(_tmp669,_tag_dyneither(_tmp17A,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp66B,_tmp66A,_tag_dyneither(_tmp179,sizeof(void*),0U));});});else{
# 1007
({void*_tmp180=0U;({unsigned int _tmp66E=e1->loc;struct _dyneither_ptr _tmp66D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp183=({struct Cyc_String_pa_PrintArg_struct _tmp51B;_tmp51B.tag=0U,_tmp51B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp51B;});struct Cyc_String_pa_PrintArg_struct _tmp184=({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0U,_tmp51A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp51A;});struct Cyc_String_pa_PrintArg_struct _tmp185=({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0U,_tmp519.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp519;});struct Cyc_String_pa_PrintArg_struct _tmp186=({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0U,_tmp518.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp518;});void*_tmp181[4U];_tmp181[0]=& _tmp183,_tmp181[1]=& _tmp184,_tmp181[2]=& _tmp185,_tmp181[3]=& _tmp186;({struct _dyneither_ptr _tmp66C=({const char*_tmp182="%s%s%s%s.";_tag_dyneither(_tmp182,sizeof(char),10U);});Cyc_aprintf(_tmp66C,_tag_dyneither(_tmp181,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp66E,_tmp66D,_tag_dyneither(_tmp180,sizeof(void*),0U));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1012
if(alias_coercion)
({struct Cyc_List_List*_tmp66F=({struct Cyc_List_List*_tmp189=_cycalloc(sizeof(*_tmp189));_tmp189->hd=(void*)_tmp171,_tmp189->tl=*alias_arg_exps;_tmp189;});*alias_arg_exps=_tmp66F;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp18A=0U;({unsigned int _tmp671=e1->loc;struct _dyneither_ptr _tmp670=({const char*_tmp18B="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp18B,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp671,_tmp670,_tag_dyneither(_tmp18A,sizeof(void*),0U));});});
_tmp170=_tmp170->tl;
_tmp1CA=_tmp1CA->tl;
++ _tmp171;}{
# 1023
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1C6;for(0;a != 0;a=a->tl){
void*_tmp18C=(void*)a->hd;void*_tmp18D=_tmp18C;enum Cyc_Absyn_Format_Type _tmp19A;int _tmp199;int _tmp198;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp18D)->tag == 19U){_LLB: _tmp19A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp18D)->f1;_tmp199=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp18D)->f2;_tmp198=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp18D)->f3;_LLC:
# 1027
{struct _handler_cons _tmp18E;_push_handler(& _tmp18E);{int _tmp190=0;if(setjmp(_tmp18E.handler))_tmp190=1;if(!_tmp190){
# 1029
{struct Cyc_Absyn_Exp*_tmp191=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp199 - 1);
# 1031
struct Cyc_Core_Opt*fmt_args;
if(_tmp198 == 0)
fmt_args=0;else{
# 1035
({struct Cyc_Core_Opt*_tmp673=({struct Cyc_Core_Opt*_tmp192=_cycalloc(sizeof(*_tmp192));({struct Cyc_List_List*_tmp672=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp198 - 1);_tmp192->v=_tmp672;});_tmp192;});fmt_args=_tmp673;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp193=_tmp19A;if(_tmp193 == Cyc_Absyn_Printf_ft){_LL10: _LL11:
# 1039
 Cyc_Tcexp_check_format_args(_tmp173,_tmp191,fmt_args,_tmp198 - 1,alias_arg_exps,Cyc_Formatstr_get_format_typs);
# 1042
goto _LLF;}else{_LL12: _LL13:
# 1044
 Cyc_Tcexp_check_format_args(_tmp173,_tmp191,fmt_args,_tmp198 - 1,alias_arg_exps,Cyc_Formatstr_get_scanf_typs);
# 1047
goto _LLF;}_LLF:;};}
# 1029
;_pop_handler();}else{void*_tmp18F=(void*)_exn_thrown;void*_tmp194=_tmp18F;void*_tmp197;if(((struct Cyc_List_Nth_exn_struct*)_tmp194)->tag == Cyc_List_Nth){_LL15: _LL16:
# 1049
({void*_tmp195=0U;({unsigned int _tmp675=loc;struct _dyneither_ptr _tmp674=({const char*_tmp196="bad format arguments";_tag_dyneither(_tmp196,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp675,_tmp674,_tag_dyneither(_tmp195,sizeof(void*),0U));});});goto _LL14;}else{_LL17: _tmp197=_tmp194;_LL18:(int)_rethrow(_tmp197);}_LL14:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1054
if(_tmp1CA != 0)({void*_tmp19B=0U;({unsigned int _tmp677=loc;struct _dyneither_ptr _tmp676=({const char*_tmp19C="too few arguments for function";_tag_dyneither(_tmp19C,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp677,_tmp676,_tag_dyneither(_tmp19B,sizeof(void*),0U));});});else{
# 1056
if((_tmp170 != 0  || _tmp1C9) || _tmp1C8 != 0){
if(_tmp1C9)
for(0;_tmp170 != 0;_tmp170=_tmp170->tl){
Cyc_Tcexp_tcExp(_tmp173,0,(struct Cyc_Absyn_Exp*)_tmp170->hd);}else{
if(_tmp1C8 == 0)
({void*_tmp19D=0U;({unsigned int _tmp679=loc;struct _dyneither_ptr _tmp678=({const char*_tmp19E="too many arguments for function";_tag_dyneither(_tmp19E,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp679,_tmp678,_tag_dyneither(_tmp19D,sizeof(void*),0U));});});else{
# 1063
struct Cyc_Absyn_VarargInfo _tmp19F=*_tmp1C8;struct Cyc_Absyn_VarargInfo _tmp1A0=_tmp19F;void*_tmp1BE;int _tmp1BD;_LL1A: _tmp1BE=_tmp1A0.type;_tmp1BD=_tmp1A0.inject;_LL1B:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1A1=({struct Cyc_Absyn_VarargCallInfo*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->num_varargs=0,_tmp1BC->injectors=0,_tmp1BC->vai=_tmp1C8;_tmp1BC;});
# 1067
*vararg_call_info=_tmp1A1;
# 1069
if(!_tmp1BD)
# 1071
for(0;_tmp170 != 0;(_tmp170=_tmp170->tl,_tmp171 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp170->hd;
++ _tmp1A1->num_varargs;
Cyc_Tcexp_tcExp(_tmp173,& _tmp1BE,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp173,e1,_tmp1BE,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1A4=({struct Cyc_String_pa_PrintArg_struct _tmp51D;_tmp51D.tag=0U,({
struct _dyneither_ptr _tmp67A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1BE));_tmp51D.f1=_tmp67A;});_tmp51D;});struct Cyc_String_pa_PrintArg_struct _tmp1A5=({struct Cyc_String_pa_PrintArg_struct _tmp51C;_tmp51C.tag=0U,({struct _dyneither_ptr _tmp67B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp51C.f1=_tmp67B;});_tmp51C;});void*_tmp1A2[2U];_tmp1A2[0]=& _tmp1A4,_tmp1A2[1]=& _tmp1A5;({unsigned int _tmp67D=loc;struct _dyneither_ptr _tmp67C=({const char*_tmp1A3="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1A3,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp67D,_tmp67C,_tag_dyneither(_tmp1A2,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 1081
if(alias_coercion)
({struct Cyc_List_List*_tmp67E=({struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->hd=(void*)_tmp171,_tmp1A6->tl=*alias_arg_exps;_tmp1A6;});*alias_arg_exps=_tmp67E;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1BE) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1A7=0U;({unsigned int _tmp680=e1->loc;struct _dyneither_ptr _tmp67F=({const char*_tmp1A8="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1A8,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp680,_tmp67F,_tag_dyneither(_tmp1A7,sizeof(void*),0U));});});}else{
# 1090
void*_tmp1A9=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1BE));void*_tmp1AA=_tmp1A9;struct Cyc_Absyn_Datatypedecl*_tmp1BB;struct Cyc_List_List*_tmp1BA;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AA)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AA)->f1).datatype_info).KnownDatatype).tag == 2){_LL1D: _tmp1BB=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AA)->f1).datatype_info).KnownDatatype).val;_tmp1BA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AA)->f1).targs;_LL1E: {
# 1094
struct Cyc_Absyn_Datatypedecl*_tmp1AB=*Cyc_Tcenv_lookup_datatypedecl(_tmp173,loc,_tmp1BB->name);
struct Cyc_List_List*fields=0;
if(_tmp1AB->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1AE=({struct Cyc_String_pa_PrintArg_struct _tmp51E;_tmp51E.tag=0U,({
struct _dyneither_ptr _tmp681=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1BE));_tmp51E.f1=_tmp681;});_tmp51E;});void*_tmp1AC[1U];_tmp1AC[0]=& _tmp1AE;({unsigned int _tmp683=loc;struct _dyneither_ptr _tmp682=({const char*_tmp1AD="can't inject into abstract datatype %s";_tag_dyneither(_tmp1AD,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp683,_tmp682,_tag_dyneither(_tmp1AC,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AB->fields))->v;}
# 1105
if(!({void*_tmp684=Cyc_Tcutil_pointer_region(_tmp1BE);Cyc_Tcutil_unify(_tmp684,Cyc_Tcenv_curr_rgn(_tmp173));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp520;_tmp520.tag=0U,({
struct _dyneither_ptr _tmp685=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1BE));_tmp520.f1=_tmp685;});_tmp520;});struct Cyc_String_pa_PrintArg_struct _tmp1B2=({struct Cyc_String_pa_PrintArg_struct _tmp51F;_tmp51F.tag=0U,({struct _dyneither_ptr _tmp686=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp173)));_tmp51F.f1=_tmp686;});_tmp51F;});void*_tmp1AF[2U];_tmp1AF[0]=& _tmp1B1,_tmp1AF[1]=& _tmp1B2;({unsigned int _tmp688=loc;struct _dyneither_ptr _tmp687=({const char*_tmp1B0="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1B0,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp688,_tmp687,_tag_dyneither(_tmp1AF,sizeof(void*),2U));});});{
# 1109
struct Cyc_List_List*_tmp1B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1AB->tvs,_tmp1BA);
for(0;_tmp170 != 0;_tmp170=_tmp170->tl){
++ _tmp1A1->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp170->hd;
# 1114
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp173,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1B4=0U;({unsigned int _tmp68A=e1->loc;struct _dyneither_ptr _tmp689=({const char*_tmp1B5="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1B5,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp68A,_tmp689,_tag_dyneither(_tmp1B4,sizeof(void*),0U));});});}{
# 1120
struct Cyc_Absyn_Datatypefield*_tmp1B6=({struct Cyc_Tcenv_Tenv*_tmp68E=_tmp173;struct Cyc_Absyn_Exp*_tmp68D=e1;void*_tmp68C=Cyc_Tcutil_pointer_elt_type(_tmp1BE);struct Cyc_List_List*_tmp68B=_tmp1B3;Cyc_Tcexp_tcInjection(_tmp68E,_tmp68D,_tmp68C,_tmp68B,fields);});
if(_tmp1B6 != 0)
({struct Cyc_List_List*_tmp690=({
struct Cyc_List_List*_tmp68F=_tmp1A1->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp68F,({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));
_tmp1B7->hd=_tmp1B6,_tmp1B7->tl=0;_tmp1B7;}));});
# 1122
_tmp1A1->injectors=_tmp690;});};};}
# 1126
goto _LL1C;};}}else{goto _LL1F;}}else{_LL1F: _LL20:
({void*_tmp1B8=0U;({unsigned int _tmp692=loc;struct _dyneither_ptr _tmp691=({const char*_tmp1B9="bad inject vararg type";_tag_dyneither(_tmp1B9,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp692,_tmp691,_tag_dyneither(_tmp1B8,sizeof(void*),0U));});});goto _LL1C;}_LL1C:;}};}}}}
# 1132
if(*alias_arg_exps == 0)
# 1141 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp173,loc,(void*)_check_null(_tmp1CD));
# 1143
return _tmp1CB;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1C1=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0U,({struct _dyneither_ptr _tmp693=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp521.f1=_tmp693;});_tmp521;});void*_tmp1BF[1U];_tmp1BF[0]=& _tmp1C1;({struct Cyc_Tcenv_Tenv*_tmp697=_tmp173;unsigned int _tmp696=loc;void**_tmp695=topt;struct _dyneither_ptr _tmp694=({const char*_tmp1C0="expected pointer to function but found %s";_tag_dyneither(_tmp1C0,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp697,_tmp696,_tmp695,_tmp694,_tag_dyneither(_tmp1BF,sizeof(void*),1U));});});}_LL5:;};}else{_LL3: _LL4:
# 1146
 return({struct Cyc_String_pa_PrintArg_struct _tmp1D1=({struct Cyc_String_pa_PrintArg_struct _tmp522;_tmp522.tag=0U,({struct _dyneither_ptr _tmp698=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp522.f1=_tmp698;});_tmp522;});void*_tmp1CF[1U];_tmp1CF[0]=& _tmp1D1;({struct Cyc_Tcenv_Tenv*_tmp69C=_tmp173;unsigned int _tmp69B=loc;void**_tmp69A=topt;struct _dyneither_ptr _tmp699=({const char*_tmp1D0="expected pointer to function but found %s";_tag_dyneither(_tmp1D0,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp69C,_tmp69B,_tmp69A,_tmp699,_tag_dyneither(_tmp1CF,sizeof(void*),1U));});});}_LL0:;};}
# 1150
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1D8=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1D8,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1D8,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1DB=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,({struct _dyneither_ptr _tmp69D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D8));_tmp524.f1=_tmp69D;});_tmp524;});struct Cyc_String_pa_PrintArg_struct _tmp1DC=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0U,({
struct _dyneither_ptr _tmp69E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp523.f1=_tmp69E;});_tmp523;});void*_tmp1D9[2U];_tmp1D9[0]=& _tmp1DB,_tmp1D9[1]=& _tmp1DC;({unsigned int _tmp6A0=loc;struct _dyneither_ptr _tmp69F=({const char*_tmp1DA="expected %s but found %s";_tag_dyneither(_tmp1DA,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp6A0,_tmp69F,_tag_dyneither(_tmp1D9,sizeof(void*),2U));});});
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1162
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1164
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1166
({void*_tmp6A1=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp6A1;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1DD=t1;void*_tmp1F8;struct Cyc_Absyn_Tqual _tmp1F7;void*_tmp1F6;union Cyc_Absyn_Constraint*_tmp1F5;union Cyc_Absyn_Constraint*_tmp1F4;union Cyc_Absyn_Constraint*_tmp1F3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->tag == 5U){_LL1: _tmp1F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1).elt_typ;_tmp1F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1).elt_tq;_tmp1F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1).ptr_atts).rgn;_tmp1F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1).ptr_atts).nullable;_tmp1F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1).ptr_atts).bounds;_tmp1F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1DD)->f1).ptr_atts).zero_term;_LL2:
# 1170
{void*_tmp1DE=Cyc_Tcutil_compress(_tmp1F8);void*_tmp1DF=_tmp1DE;struct Cyc_List_List*_tmp1F2;void*_tmp1F1;struct Cyc_Absyn_Tqual _tmp1F0;void*_tmp1EF;struct Cyc_List_List*_tmp1EE;int _tmp1ED;struct Cyc_Absyn_VarargInfo*_tmp1EC;struct Cyc_List_List*_tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_List_List*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_List_List*_tmp1E6;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->tag == 9U){_LL6: _tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).tvars;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).effect;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).ret_tqual;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).ret_typ;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).args;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).c_varargs;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).cyc_varargs;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).rgn_po;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).attributes;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).requires_clause;_tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).requires_relns;_tmp1E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).ensures_clause;_tmp1E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DF)->f1).ensures_relns;_LL7: {
# 1172
struct Cyc_List_List*instantiation=0;
# 1174
for(0;ts != 0  && _tmp1F2 != 0;(ts=ts->tl,_tmp1F2=_tmp1F2->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1F2->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp6A5=loc;struct Cyc_Tcenv_Tenv*_tmp6A4=te;struct Cyc_List_List*_tmp6A3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6A2=k;Cyc_Tcutil_check_type(_tmp6A5,_tmp6A4,_tmp6A3,_tmp6A2,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp6A7=({struct Cyc_List_List*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));({struct _tuple13*_tmp6A6=({struct _tuple13*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->f1=(struct Cyc_Absyn_Tvar*)_tmp1F2->hd,_tmp1E0->f2=(void*)ts->hd;_tmp1E0;});_tmp1E1->hd=_tmp6A6;}),_tmp1E1->tl=instantiation;_tmp1E1;});instantiation=_tmp6A7;});}
# 1180
if(ts != 0)
return({void*_tmp1E2=0U;({struct Cyc_Tcenv_Tenv*_tmp6AB=te;unsigned int _tmp6AA=loc;void**_tmp6A9=topt;struct _dyneither_ptr _tmp6A8=({const char*_tmp1E3="too many type variables in instantiation";_tag_dyneither(_tmp1E3,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp6AB,_tmp6AA,_tmp6A9,_tmp6A8,_tag_dyneither(_tmp1E2,sizeof(void*),0U));});});
# 1185
if(_tmp1F2 == 0){
({struct Cyc_List_List*_tmp6AC=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1EB);_tmp1EB=_tmp6AC;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1EB);
_tmp1EB=0;}{
# 1190
void*new_fn_typ=({
struct Cyc_List_List*_tmp6AD=instantiation;Cyc_Tcutil_substitute(_tmp6AD,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));
_tmp1E5->tag=9U,(_tmp1E5->f1).tvars=_tmp1F2,(_tmp1E5->f1).effect=_tmp1F1,(_tmp1E5->f1).ret_tqual=_tmp1F0,(_tmp1E5->f1).ret_typ=_tmp1EF,(_tmp1E5->f1).args=_tmp1EE,(_tmp1E5->f1).c_varargs=_tmp1ED,(_tmp1E5->f1).cyc_varargs=_tmp1EC,(_tmp1E5->f1).rgn_po=_tmp1EB,(_tmp1E5->f1).attributes=_tmp1EA,(_tmp1E5->f1).requires_clause=_tmp1E9,(_tmp1E5->f1).requires_relns=_tmp1E8,(_tmp1E5->f1).ensures_clause=_tmp1E7,(_tmp1E5->f1).ensures_relns=_tmp1E6;_tmp1E5;}));});
# 1195
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->tag=5U,(_tmp1E4->f1).elt_typ=new_fn_typ,(_tmp1E4->f1).elt_tq=_tmp1F7,((_tmp1E4->f1).ptr_atts).rgn=_tmp1F6,((_tmp1E4->f1).ptr_atts).nullable=_tmp1F5,((_tmp1E4->f1).ptr_atts).bounds=_tmp1F4,((_tmp1E4->f1).ptr_atts).zero_term=_tmp1F3,((_tmp1E4->f1).ptr_atts).ptrloc=0;_tmp1E4;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1198
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1201
return({struct Cyc_String_pa_PrintArg_struct _tmp1FB=({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0U,({
struct _dyneither_ptr _tmp6AE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp525.f1=_tmp6AE;});_tmp525;});void*_tmp1F9[1U];_tmp1F9[0]=& _tmp1FB;({struct Cyc_Tcenv_Tenv*_tmp6B2=te;unsigned int _tmp6B1=loc;void**_tmp6B0=topt;struct _dyneither_ptr _tmp6AF=({const char*_tmp1FA="expecting polymorphic type but found %s";_tag_dyneither(_tmp1FA,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp6B2,_tmp6B1,_tmp6B0,_tmp6AF,_tag_dyneither(_tmp1F9,sizeof(void*),1U));});});};}
# 1206
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1208
({unsigned int _tmp6B6=loc;struct Cyc_Tcenv_Tenv*_tmp6B5=te;struct Cyc_List_List*_tmp6B4=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6B3=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1208
Cyc_Tcutil_check_type(_tmp6B6,_tmp6B5,_tmp6B4,_tmp6B3,1,0,t);});
# 1210
Cyc_Tcutil_check_no_qual(loc,t);
# 1212
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1217
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1224
Cyc_Tcutil_unify(t2,t);{
void*_tmp1FC=({struct Cyc_String_pa_PrintArg_struct _tmp1FF=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,({
struct _dyneither_ptr _tmp6B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp527.f1=_tmp6B7;});_tmp527;});struct Cyc_String_pa_PrintArg_struct _tmp200=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,({struct _dyneither_ptr _tmp6B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp526.f1=_tmp6B8;});_tmp526;});void*_tmp1FD[2U];_tmp1FD[0]=& _tmp1FF,_tmp1FD[1]=& _tmp200;({struct Cyc_Tcenv_Tenv*_tmp6BB=te;unsigned int _tmp6BA=loc;struct _dyneither_ptr _tmp6B9=({const char*_tmp1FE="cannot cast %s to %s";_tag_dyneither(_tmp1FE,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6BB,_tmp6BA,& t,_tmp6B9,_tag_dyneither(_tmp1FD,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return _tmp1FC;};}}}
# 1234
{struct _tuple0 _tmp201=({struct _tuple0 _tmp528;_tmp528.f1=e->r,({void*_tmp6BC=Cyc_Tcutil_compress(t);_tmp528.f2=_tmp6BC;});_tmp528;});struct _tuple0 _tmp202=_tmp201;int _tmp20B;union Cyc_Absyn_Constraint*_tmp20A;union Cyc_Absyn_Constraint*_tmp209;union Cyc_Absyn_Constraint*_tmp208;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp202.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202.f2)->tag == 5U){_LL1: _tmp20B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp202.f1)->f1).fat_result;_tmp20A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202.f2)->f1).ptr_atts).nullable;_tmp209=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202.f2)->f1).ptr_atts).bounds;_tmp208=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp202.f2)->f1).ptr_atts).zero_term;_LL2:
# 1238
 if((_tmp20B  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp208)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp20A)){
void*_tmp203=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp209);void*_tmp204=_tmp203;struct Cyc_Absyn_Exp*_tmp207;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp204)->tag == 1U){_LL6: _tmp207=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp204)->f1;_LL7:
# 1241
 if((Cyc_Evexp_eval_const_uint_exp(_tmp207)).f1 == 1)
({void*_tmp205=0U;({unsigned int _tmp6BE=loc;struct _dyneither_ptr _tmp6BD=({const char*_tmp206="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp206,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6BE,_tmp6BD,_tag_dyneither(_tmp205,sizeof(void*),0U));});});
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1247
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1250
return t;};}
# 1254
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp20C=0;
struct Cyc_Absyn_Tqual _tmp20D=Cyc_Absyn_empty_tqual(0U);
int _tmp20E=0;
if(topt != 0){
void*_tmp20F=Cyc_Tcutil_compress(*topt);void*_tmp210=_tmp20F;void**_tmp213;struct Cyc_Absyn_Tqual _tmp212;union Cyc_Absyn_Constraint*_tmp211;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->tag == 5U){_LL1: _tmp213=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->f1).elt_typ;_tmp212=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->f1).elt_tq;_tmp211=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp210)->f1).ptr_atts).nullable;_LL2:
# 1261
 _tmp20C=_tmp213;
_tmp20D=_tmp212;
({int _tmp6BF=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp211);_tmp20E=_tmp6BF;});
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1275
({struct Cyc_Tcenv_Tenv*_tmp6C1=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp6C0=_tmp20C;Cyc_Tcexp_tcExpNoInst(_tmp6C1,_tmp6C0,e);});
# 1278
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp214=0U;({unsigned int _tmp6C3=e->loc;struct _dyneither_ptr _tmp6C2=({const char*_tmp215="Cannot take the address of an alias-free path";_tag_dyneither(_tmp215,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6C3,_tmp6C2,_tag_dyneither(_tmp214,sizeof(void*),0U));});});
# 1284
{void*_tmp216=e->r;void*_tmp217=_tmp216;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*_tmp21A;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp217)->tag == 23U){_LL6: _tmp21B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp217)->f1;_tmp21A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp217)->f2;_LL7:
# 1286
{void*_tmp218=Cyc_Tcutil_compress((void*)_check_null(_tmp21B->topt));void*_tmp219=_tmp218;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp219)->tag == 10U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1291
({void*_tmp6C4=(Cyc_Absyn_add_exp(_tmp21B,_tmp21A,0U))->r;e0->r=_tmp6C4;});
return Cyc_Tcexp_tcPlus(te,_tmp21B,_tmp21A);}_LLA:;}
# 1294
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1299
{void*_tmp21C=e->r;void*_tmp21D=_tmp21C;switch(*((int*)_tmp21D)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp21D)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp21D)->f3 == 1){_LL12: _LL13:
# 1302
({void*_tmp21E=0U;({unsigned int _tmp6C6=e->loc;struct _dyneither_ptr _tmp6C5=({const char*_tmp21F="cannot take the address of a @tagged union member";_tag_dyneither(_tmp21F,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6C6,_tmp6C5,_tag_dyneither(_tmp21E,sizeof(void*),0U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1308
struct _tuple14 _tmp220=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp221=_tmp220;int _tmp225;void*_tmp224;_LL17: _tmp225=_tmp221.f1;_tmp224=_tmp221.f2;_LL18:;
# 1310
if(Cyc_Tcutil_is_noalias_region(_tmp224,0))
({void*_tmp222=0U;({unsigned int _tmp6C8=e->loc;struct _dyneither_ptr _tmp6C7=({const char*_tmp223="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp223,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp6C8,_tmp6C7,_tag_dyneither(_tmp222,sizeof(void*),0U));});});{
# 1313
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp225){
tq.print_const=1;
tq.real_const=1;}{
# 1319
void*t=_tmp20E?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp224,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp224,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1326
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1330
return Cyc_Absyn_uint_typ;
# 1332
({unsigned int _tmp6CB=loc;struct Cyc_Tcenv_Tenv*_tmp6CA=te;struct Cyc_List_List*_tmp6C9=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6CB,_tmp6CA,_tmp6C9,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,({
struct _dyneither_ptr _tmp6CC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp529.f1=_tmp6CC;});_tmp529;});void*_tmp226[1U];_tmp226[0]=& _tmp228;({unsigned int _tmp6CE=loc;struct _dyneither_ptr _tmp6CD=({const char*_tmp227="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp227,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp6CE,_tmp6CD,_tag_dyneither(_tmp226,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp229=Cyc_Tcutil_compress(*topt);void*_tmp22A=_tmp229;void*_tmp22F;void*_tmp22E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp22A)->tag == 19U){_LL1: _tmp22F=_tmp22A;_tmp22E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp22A)->f1;_LL2: {
# 1341
struct Cyc_Absyn_Exp*_tmp22B=Cyc_Absyn_sizeoftyp_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp22C=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->tag=18U,_tmp22D->f1=_tmp22B;_tmp22D;});
if(Cyc_Tcutil_unify(_tmp22E,(void*)_tmp22C))return _tmp22F;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1347
return Cyc_Absyn_uint_typ;}
# 1350
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1357
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1359
({unsigned int _tmp6D1=loc;struct Cyc_Tcenv_Tenv*_tmp6D0=te;struct Cyc_List_List*_tmp6CF=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6D1,_tmp6D0,_tmp6CF,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp230=fs;
void*_tmp231=t_orig;
for(0;_tmp230 != 0;_tmp230=_tmp230->tl){
void*_tmp232=(void*)_tmp230->hd;
void*_tmp233=_tmp232;unsigned int _tmp26D;struct _dyneither_ptr*_tmp26C;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp233)->tag == 0U){_LL1: _tmp26C=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp233)->f1;_LL2: {
# 1367
int bad_type=1;
{void*_tmp234=Cyc_Tcutil_compress(_tmp231);void*_tmp235=_tmp234;struct Cyc_List_List*_tmp23D;struct Cyc_Absyn_Aggrdecl**_tmp23C;switch(*((int*)_tmp235)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp235)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp23C=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp235)->f1).aggr_info).KnownAggr).val;_LL7:
# 1370
 if((*_tmp23C)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp26C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp23C)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp238=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0U,_tmp52A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp26C);_tmp52A;});void*_tmp236[1U];_tmp236[0]=& _tmp238;({unsigned int _tmp6D3=loc;struct _dyneither_ptr _tmp6D2=({const char*_tmp237="no field of struct/union has name %s";_tag_dyneither(_tmp237,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6D3,_tmp6D2,_tag_dyneither(_tmp236,sizeof(void*),1U));});});else{
# 1375
_tmp231=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}case 12U: _LL8: _tmp23D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp235)->f2;_LL9: {
# 1379
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp26C,_tmp23D);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp23B=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0U,_tmp52B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp26C);_tmp52B;});void*_tmp239[1U];_tmp239[0]=& _tmp23B;({unsigned int _tmp6D5=loc;struct _dyneither_ptr _tmp6D4=({const char*_tmp23A="no field of struct/union has name %s";_tag_dyneither(_tmp23A,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6D5,_tmp6D4,_tag_dyneither(_tmp239,sizeof(void*),1U));});});else{
# 1383
_tmp231=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1388
if(bad_type){
if(_tmp230 == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp240=({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0U,({struct _dyneither_ptr _tmp6D6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp231));_tmp52C.f1=_tmp6D6;});_tmp52C;});void*_tmp23E[1U];_tmp23E[0]=& _tmp240;({unsigned int _tmp6D8=loc;struct _dyneither_ptr _tmp6D7=({const char*_tmp23F="%s is not a known struct/union type";_tag_dyneither(_tmp23F,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6D8,_tmp6D7,_tag_dyneither(_tmp23E,sizeof(void*),1U));});});else{
# 1392
struct _dyneither_ptr _tmp241=({struct Cyc_String_pa_PrintArg_struct _tmp254=({struct Cyc_String_pa_PrintArg_struct _tmp533;_tmp533.tag=0U,({struct _dyneither_ptr _tmp6D9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp533.f1=_tmp6D9;});_tmp533;});void*_tmp252[1U];_tmp252[0]=& _tmp254;({struct _dyneither_ptr _tmp6DA=({const char*_tmp253="(%s)";_tag_dyneither(_tmp253,sizeof(char),5U);});Cyc_aprintf(_tmp6DA,_tag_dyneither(_tmp252,sizeof(void*),1U));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp230;x=x->tl){
void*_tmp242=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp243=_tmp242;unsigned int _tmp24D;struct _dyneither_ptr*_tmp24C;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp243)->tag == 0U){_LLD: _tmp24C=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp243)->f1;_LLE:
# 1397
({struct _dyneither_ptr _tmp6DC=({struct Cyc_String_pa_PrintArg_struct _tmp246=({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0U,_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp241);_tmp52E;});struct Cyc_String_pa_PrintArg_struct _tmp247=({struct Cyc_String_pa_PrintArg_struct _tmp52D;_tmp52D.tag=0U,_tmp52D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp24C);_tmp52D;});void*_tmp244[2U];_tmp244[0]=& _tmp246,_tmp244[1]=& _tmp247;({struct _dyneither_ptr _tmp6DB=({const char*_tmp245="%s.%s";_tag_dyneither(_tmp245,sizeof(char),6U);});Cyc_aprintf(_tmp6DB,_tag_dyneither(_tmp244,sizeof(void*),2U));});});_tmp241=_tmp6DC;});goto _LLC;}else{_LLF: _tmp24D=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp243)->f1;_LL10:
# 1399
({struct _dyneither_ptr _tmp6DE=({struct Cyc_String_pa_PrintArg_struct _tmp24A=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,_tmp530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp241);_tmp530;});struct Cyc_Int_pa_PrintArg_struct _tmp24B=({struct Cyc_Int_pa_PrintArg_struct _tmp52F;_tmp52F.tag=1U,_tmp52F.f1=(unsigned long)((int)_tmp24D);_tmp52F;});void*_tmp248[2U];_tmp248[0]=& _tmp24A,_tmp248[1]=& _tmp24B;({struct _dyneither_ptr _tmp6DD=({const char*_tmp249="%s.%d";_tag_dyneither(_tmp249,sizeof(char),6U);});Cyc_aprintf(_tmp6DD,_tag_dyneither(_tmp248,sizeof(void*),2U));});});_tmp241=_tmp6DE;});goto _LLC;}_LLC:;}
# 1401
({struct Cyc_String_pa_PrintArg_struct _tmp250=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,_tmp532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp241);_tmp532;});struct Cyc_String_pa_PrintArg_struct _tmp251=({struct Cyc_String_pa_PrintArg_struct _tmp531;_tmp531.tag=0U,({struct _dyneither_ptr _tmp6DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp231));_tmp531.f1=_tmp6DF;});_tmp531;});void*_tmp24E[2U];_tmp24E[0]=& _tmp250,_tmp24E[1]=& _tmp251;({unsigned int _tmp6E1=loc;struct _dyneither_ptr _tmp6E0=({const char*_tmp24F="%s == %s is not a struct/union type";_tag_dyneither(_tmp24F,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6E1,_tmp6E0,_tag_dyneither(_tmp24E,sizeof(void*),2U));});});}}
# 1404
goto _LL0;}}else{_LL3: _tmp26D=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp233)->f1;_LL4: {
# 1406
int bad_type=1;
{void*_tmp255=Cyc_Tcutil_compress(_tmp231);void*_tmp256=_tmp255;struct Cyc_Absyn_Datatypefield*_tmp268;struct Cyc_List_List*_tmp267;struct Cyc_List_List*_tmp266;struct Cyc_Absyn_Aggrdecl**_tmp265;switch(*((int*)_tmp256)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp256)->f1).aggr_info).KnownAggr).tag == 2){_LL12: _tmp265=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp256)->f1).aggr_info).KnownAggr).val;_LL13:
# 1409
 if((*_tmp265)->impl == 0)
goto _LL11;
_tmp266=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp265)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 12U: _LL14: _tmp266=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp256)->f2;_LL15:
# 1413
 if(({int _tmp6E2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp266);_tmp6E2 <= _tmp26D;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp259=({struct Cyc_Int_pa_PrintArg_struct _tmp535;_tmp535.tag=1U,({
unsigned long _tmp6E3=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp266);_tmp535.f1=_tmp6E3;});_tmp535;});struct Cyc_Int_pa_PrintArg_struct _tmp25A=({struct Cyc_Int_pa_PrintArg_struct _tmp534;_tmp534.tag=1U,_tmp534.f1=(unsigned long)((int)_tmp26D);_tmp534;});void*_tmp257[2U];_tmp257[0]=& _tmp259,_tmp257[1]=& _tmp25A;({unsigned int _tmp6E5=loc;struct _dyneither_ptr _tmp6E4=({const char*_tmp258="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp258,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6E5,_tmp6E4,_tag_dyneither(_tmp257,sizeof(void*),2U));});});else{
# 1417
({void*_tmp6E6=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp266,(int)_tmp26D))->type;_tmp231=_tmp6E6;});}
bad_type=0;
goto _LL11;case 10U: _LL16: _tmp267=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp256)->f1;_LL17:
# 1421
 if(({int _tmp6E7=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp267);_tmp6E7 <= _tmp26D;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp25D=({struct Cyc_Int_pa_PrintArg_struct _tmp537;_tmp537.tag=1U,({
unsigned long _tmp6E8=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp267);_tmp537.f1=_tmp6E8;});_tmp537;});struct Cyc_Int_pa_PrintArg_struct _tmp25E=({struct Cyc_Int_pa_PrintArg_struct _tmp536;_tmp536.tag=1U,_tmp536.f1=(unsigned long)((int)_tmp26D);_tmp536;});void*_tmp25B[2U];_tmp25B[0]=& _tmp25D,_tmp25B[1]=& _tmp25E;({unsigned int _tmp6EA=loc;struct _dyneither_ptr _tmp6E9=({const char*_tmp25C="tuple has too few components: %d <= %d";_tag_dyneither(_tmp25C,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6EA,_tmp6E9,_tag_dyneither(_tmp25B,sizeof(void*),2U));});});else{
# 1425
({void*_tmp6EB=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp267,(int)_tmp26D)).f2;_tmp231=_tmp6EB;});}
bad_type=0;
goto _LL11;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp256)->f1).field_info).KnownDatatypefield).tag == 2){_LL18: _tmp268=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp256)->f1).field_info).KnownDatatypefield).val).f2;_LL19:
# 1429
 if(({int _tmp6EC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp268->typs);_tmp6EC < _tmp26D;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp261=({struct Cyc_Int_pa_PrintArg_struct _tmp539;_tmp539.tag=1U,({
unsigned long _tmp6ED=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp268->typs);_tmp539.f1=_tmp6ED;});_tmp539;});struct Cyc_Int_pa_PrintArg_struct _tmp262=({struct Cyc_Int_pa_PrintArg_struct _tmp538;_tmp538.tag=1U,_tmp538.f1=(unsigned long)((int)_tmp26D);_tmp538;});void*_tmp25F[2U];_tmp25F[0]=& _tmp261,_tmp25F[1]=& _tmp262;({unsigned int _tmp6EF=loc;struct _dyneither_ptr _tmp6EE=({const char*_tmp260="datatype field has too few components: %d < %d";_tag_dyneither(_tmp260,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6EF,_tmp6EE,_tag_dyneither(_tmp25F,sizeof(void*),2U));});});else{
# 1433
if(_tmp26D != 0)
({void*_tmp6F0=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp268->typs,(int)(_tmp26D - 1))).f2;_tmp231=_tmp6F0;});else{
if(_tmp230->tl != 0)
({void*_tmp263=0U;({unsigned int _tmp6F2=loc;struct _dyneither_ptr _tmp6F1=({const char*_tmp264="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp264,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6F2,_tmp6F1,_tag_dyneither(_tmp263,sizeof(void*),0U));});});}}
# 1438
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: _LL1A: _LL1B:
 goto _LL11;}_LL11:;}
# 1442
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp26B=({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U,({struct _dyneither_ptr _tmp6F3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp231));_tmp53A.f1=_tmp6F3;});_tmp53A;});void*_tmp269[1U];_tmp269[0]=& _tmp26B;({unsigned int _tmp6F5=loc;struct _dyneither_ptr _tmp6F4=({const char*_tmp26A="%s is not a known type";_tag_dyneither(_tmp26A,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6F5,_tmp6F4,_tag_dyneither(_tmp269,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1447
return Cyc_Absyn_uint_typ;};}
# 1451
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp26E=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp26E,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp26F=t;void*_tmp282;void*_tmp281;union Cyc_Absyn_Constraint*_tmp280;union Cyc_Absyn_Constraint*_tmp27F;switch(*((int*)_tmp26F)){case 1U: _LL1: _LL2: {
# 1457
struct Cyc_List_List*_tmp270=Cyc_Tcenv_lookup_type_vars(_tmp26E);
void*_tmp271=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->v=_tmp270;_tmp279;}));
void*_tmp272=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->v=_tmp270;_tmp278;}));
union Cyc_Absyn_Constraint*_tmp273=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp274=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp275=({struct Cyc_Absyn_PtrAtts _tmp53B;_tmp53B.rgn=_tmp272,({union Cyc_Absyn_Constraint*_tmp6F6=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp53B.nullable=_tmp6F6;}),_tmp53B.bounds=_tmp273,_tmp53B.zero_term=_tmp274,_tmp53B.ptrloc=0;_tmp53B;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp276=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->tag=5U,(_tmp277->f1).elt_typ=_tmp271,({struct Cyc_Absyn_Tqual _tmp6F7=Cyc_Absyn_empty_tqual(0U);(_tmp277->f1).elt_tq=_tmp6F7;}),(_tmp277->f1).ptr_atts=_tmp275;_tmp277;});
Cyc_Tcutil_unify(t,(void*)_tmp276);
_tmp282=_tmp271;_tmp281=_tmp272;_tmp280=_tmp273;_tmp27F=_tmp274;goto _LL4;}case 5U: _LL3: _tmp282=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26F)->f1).elt_typ;_tmp281=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26F)->f1).ptr_atts).rgn;_tmp280=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26F)->f1).ptr_atts).bounds;_tmp27F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26F)->f1).ptr_atts).zero_term;_LL4:
# 1467
 Cyc_Tcenv_check_rgn_accessible(_tmp26E,loc,_tmp281);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp280);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp282),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp26E))
({void*_tmp27A=0U;({unsigned int _tmp6F9=loc;struct _dyneither_ptr _tmp6F8=({const char*_tmp27B="can't dereference abstract pointer type";_tag_dyneither(_tmp27B,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6F9,_tmp6F8,_tag_dyneither(_tmp27A,sizeof(void*),0U));});});
return _tmp282;default: _LL5: _LL6:
# 1473
 return({struct Cyc_String_pa_PrintArg_struct _tmp27E=({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0U,({struct _dyneither_ptr _tmp6FA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp53C.f1=_tmp6FA;});_tmp53C;});void*_tmp27C[1U];_tmp27C[0]=& _tmp27E;({struct Cyc_Tcenv_Tenv*_tmp6FE=_tmp26E;unsigned int _tmp6FD=loc;void**_tmp6FC=topt;struct _dyneither_ptr _tmp6FB=({const char*_tmp27D="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp27D,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp6FE,_tmp6FD,_tmp6FC,_tmp6FB,_tag_dyneither(_tmp27C,sizeof(void*),1U));});});}_LL0:;};}
# 1478
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1482
({struct Cyc_Tcenv_Tenv*_tmp6FF=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6FF,0,e);});
# 1484
({int _tmp700=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp700;});{
void*_tmp283=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp284=_tmp283;enum Cyc_Absyn_AggrKind _tmp29E;struct Cyc_List_List*_tmp29D;struct Cyc_Absyn_Aggrdecl*_tmp29C;struct Cyc_List_List*_tmp29B;switch(*((int*)_tmp284)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp284)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp29C=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp284)->f1).aggr_info).KnownAggr).val;_tmp29B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp284)->f1).targs;_LL2: {
# 1487
struct Cyc_Absyn_Aggrfield*_tmp285=Cyc_Absyn_lookup_decl_field(_tmp29C,f);
if(_tmp285 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp53E;_tmp53E.tag=0U,({
struct _dyneither_ptr _tmp701=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29C->name));_tmp53E.f1=_tmp701;});_tmp53E;});struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,_tmp53D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp53D;});void*_tmp286[2U];_tmp286[0]=& _tmp288,_tmp286[1]=& _tmp289;({struct Cyc_Tcenv_Tenv*_tmp705=te;unsigned int _tmp704=loc;void**_tmp703=topt;struct _dyneither_ptr _tmp702=({const char*_tmp287="%s has no %s member";_tag_dyneither(_tmp287,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp705,_tmp704,_tmp703,_tmp702,_tag_dyneither(_tmp286,sizeof(void*),2U));});});
# 1492
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp29C->impl))->tagged)*is_tagged=1;{
void*t2=_tmp285->type;
if(_tmp29B != 0){
struct Cyc_List_List*_tmp28A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp29C->tvs,_tmp29B);
({void*_tmp706=Cyc_Tcutil_substitute(_tmp28A,_tmp285->type);t2=_tmp706;});}
# 1500
if((((_tmp29C->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp29C->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp285->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp28D=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,_tmp53F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp53F;});void*_tmp28B[1U];_tmp28B[0]=& _tmp28D;({struct Cyc_Tcenv_Tenv*_tmp70A=te;unsigned int _tmp709=loc;void**_tmp708=topt;struct _dyneither_ptr _tmp707=({const char*_tmp28C="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp28C,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp70A,_tmp709,_tmp708,_tmp707,_tag_dyneither(_tmp28B,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp29C->impl))->exist_vars != 0){
# 1505
if(!({void*_tmp70B=t2;Cyc_Tcutil_unify(_tmp70B,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,_tmp540.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp540;});void*_tmp28E[1U];_tmp28E[0]=& _tmp290;({struct Cyc_Tcenv_Tenv*_tmp70F=te;unsigned int _tmp70E=loc;void**_tmp70D=topt;struct _dyneither_ptr _tmp70C=({const char*_tmp28F="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp28F,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp70F,_tmp70E,_tmp70D,_tmp70C,_tag_dyneither(_tmp28E,sizeof(void*),1U));});});}
# 1508
return t2;};}}else{goto _LL5;}case 12U: _LL3: _tmp29E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp284)->f1;_tmp29D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp284)->f2;_LL4: {
# 1510
struct Cyc_Absyn_Aggrfield*_tmp291=Cyc_Absyn_lookup_field(_tmp29D,f);
if(_tmp291 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp294=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,_tmp541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp541;});void*_tmp292[1U];_tmp292[0]=& _tmp294;({struct Cyc_Tcenv_Tenv*_tmp713=te;unsigned int _tmp712=loc;void**_tmp711=topt;struct _dyneither_ptr _tmp710=({const char*_tmp293="type has no %s member";_tag_dyneither(_tmp293,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp713,_tmp712,_tmp711,_tmp710,_tag_dyneither(_tmp292,sizeof(void*),1U));});});
# 1515
if(((_tmp29E == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp291->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp291->requires_clause == 0)
# 1517
return({struct Cyc_String_pa_PrintArg_struct _tmp297=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp542;});void*_tmp295[1U];_tmp295[0]=& _tmp297;({struct Cyc_Tcenv_Tenv*_tmp717=te;unsigned int _tmp716=loc;void**_tmp715=topt;struct _dyneither_ptr _tmp714=({const char*_tmp296="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp296,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp717,_tmp716,_tmp715,_tmp714,_tag_dyneither(_tmp295,sizeof(void*),1U));});});
return _tmp291->type;}default: _LL5: _LL6:
# 1520
 return({struct Cyc_String_pa_PrintArg_struct _tmp29A=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,({
struct _dyneither_ptr _tmp718=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp543.f1=_tmp718;});_tmp543;});void*_tmp298[1U];_tmp298[0]=& _tmp29A;({struct Cyc_Tcenv_Tenv*_tmp71C=te;unsigned int _tmp71B=loc;void**_tmp71A=topt;struct _dyneither_ptr _tmp719=({const char*_tmp299="expecting struct or union, found %s";_tag_dyneither(_tmp299,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp71C,_tmp71B,_tmp71A,_tmp719,_tag_dyneither(_tmp298,sizeof(void*),1U));});});}_LL0:;};}
# 1526
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1529
({struct Cyc_Tcenv_Tenv*_tmp71D=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp71D,0,e);});
# 1531
({int _tmp71E=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp71E;});
{void*_tmp29F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A0=_tmp29F;void*_tmp2C3;void*_tmp2C2;union Cyc_Absyn_Constraint*_tmp2C1;union Cyc_Absyn_Constraint*_tmp2C0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A0)->tag == 5U){_LL1: _tmp2C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A0)->f1).elt_typ;_tmp2C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A0)->f1).ptr_atts).rgn;_tmp2C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A0)->f1).ptr_atts).bounds;_tmp2C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A0)->f1).ptr_atts).zero_term;_LL2:
# 1534
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2C1);
{void*_tmp2A1=Cyc_Tcutil_compress(_tmp2C3);void*_tmp2A2=_tmp2A1;enum Cyc_Absyn_AggrKind _tmp2BF;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Aggrdecl*_tmp2BD;struct Cyc_List_List*_tmp2BC;switch(*((int*)_tmp2A2)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A2)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp2BD=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A2)->f1).aggr_info).KnownAggr).val;_tmp2BC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A2)->f1).targs;_LL7: {
# 1537
struct Cyc_Absyn_Aggrfield*_tmp2A3=Cyc_Absyn_lookup_decl_field(_tmp2BD,f);
if(_tmp2A3 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A6=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,({
struct _dyneither_ptr _tmp71F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2BD->name));_tmp545.f1=_tmp71F;});_tmp545;});struct Cyc_String_pa_PrintArg_struct _tmp2A7=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp544;});void*_tmp2A4[2U];_tmp2A4[0]=& _tmp2A6,_tmp2A4[1]=& _tmp2A7;({struct Cyc_Tcenv_Tenv*_tmp723=te;unsigned int _tmp722=loc;void**_tmp721=topt;struct _dyneither_ptr _tmp720=({const char*_tmp2A5="type %s has no %s member";_tag_dyneither(_tmp2A5,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp723,_tmp722,_tmp721,_tmp720,_tag_dyneither(_tmp2A4,sizeof(void*),2U));});});
# 1542
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BD->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2A3->type;
if(_tmp2BC != 0){
struct Cyc_List_List*_tmp2A8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2BD->tvs,_tmp2BC);
({void*_tmp724=Cyc_Tcutil_substitute(_tmp2A8,_tmp2A3->type);t3=_tmp724;});}{
# 1548
struct Cyc_Absyn_Kind*_tmp2A9=Cyc_Tcutil_typ_kind(t3);
# 1551
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2A9) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2AA=Cyc_Tcutil_compress(t3);void*_tmp2AB=_tmp2AA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB)->tag == 8U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1555
 return({struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,_tmp546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp546;});void*_tmp2AC[1U];_tmp2AC[0]=& _tmp2AE;({struct Cyc_Tcenv_Tenv*_tmp728=te;unsigned int _tmp727=loc;void**_tmp726=topt;struct _dyneither_ptr _tmp725=({const char*_tmp2AD="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2AD,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp728,_tmp727,_tmp726,_tmp725,_tag_dyneither(_tmp2AC,sizeof(void*),1U));});});}_LLC:;}
# 1560
if((((_tmp2BD->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BD->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2A3->requires_clause == 0)
# 1563
return({struct Cyc_String_pa_PrintArg_struct _tmp2B1=({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0U,_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp547;});void*_tmp2AF[1U];_tmp2AF[0]=& _tmp2B1;({struct Cyc_Tcenv_Tenv*_tmp72C=te;unsigned int _tmp72B=loc;void**_tmp72A=topt;struct _dyneither_ptr _tmp729=({const char*_tmp2B0="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B0,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp72C,_tmp72B,_tmp72A,_tmp729,_tag_dyneither(_tmp2AF,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BD->impl))->exist_vars != 0){
if(!({void*_tmp72D=t3;Cyc_Tcutil_unify(_tmp72D,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B4=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,_tmp548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp548;});void*_tmp2B2[1U];_tmp2B2[0]=& _tmp2B4;({struct Cyc_Tcenv_Tenv*_tmp731=te;unsigned int _tmp730=loc;void**_tmp72F=topt;struct _dyneither_ptr _tmp72E=({const char*_tmp2B3="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2B3,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp731,_tmp730,_tmp72F,_tmp72E,_tag_dyneither(_tmp2B2,sizeof(void*),1U));});});}
# 1570
return t3;};};}}else{goto _LLA;}case 12U: _LL8: _tmp2BF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A2)->f1;_tmp2BE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A2)->f2;_LL9: {
# 1572
struct Cyc_Absyn_Aggrfield*_tmp2B5=Cyc_Absyn_lookup_field(_tmp2BE,f);
if(_tmp2B5 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2B8=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,_tmp549.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp549;});void*_tmp2B6[1U];_tmp2B6[0]=& _tmp2B8;({struct Cyc_Tcenv_Tenv*_tmp735=te;unsigned int _tmp734=loc;void**_tmp733=topt;struct _dyneither_ptr _tmp732=({const char*_tmp2B7="type has no %s field";_tag_dyneither(_tmp2B7,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp735,_tmp734,_tmp733,_tmp732,_tag_dyneither(_tmp2B6,sizeof(void*),1U));});});
# 1577
if((_tmp2BF == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2B5->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2BB=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,_tmp54A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54A;});void*_tmp2B9[1U];_tmp2B9[0]=& _tmp2BB;({struct Cyc_Tcenv_Tenv*_tmp739=te;unsigned int _tmp738=loc;void**_tmp737=topt;struct _dyneither_ptr _tmp736=({const char*_tmp2BA="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2BA,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp739,_tmp738,_tmp737,_tmp736,_tag_dyneither(_tmp2B9,sizeof(void*),1U));});});
return _tmp2B5->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1582
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1585
return({struct Cyc_String_pa_PrintArg_struct _tmp2C6=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,({
struct _dyneither_ptr _tmp73A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp54B.f1=_tmp73A;});_tmp54B;});void*_tmp2C4[1U];_tmp2C4[0]=& _tmp2C6;({struct Cyc_Tcenv_Tenv*_tmp73E=te;unsigned int _tmp73D=loc;void**_tmp73C=topt;struct _dyneither_ptr _tmp73B=({const char*_tmp2C5="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2C5,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp73E,_tmp73D,_tmp73C,_tmp73B,_tag_dyneither(_tmp2C4,sizeof(void*),1U));});});}
# 1590
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1592
struct _tuple15 _tmp2C7=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2C8=_tmp2C7;unsigned int _tmp2D6;int _tmp2D5;_LL1: _tmp2D6=_tmp2C8.f1;_tmp2D5=_tmp2C8.f2;_LL2:;
if(!_tmp2D5)
return({void*_tmp2C9=0U;({struct Cyc_Tcenv_Tenv*_tmp741=te;unsigned int _tmp740=loc;struct _dyneither_ptr _tmp73F=({const char*_tmp2CA="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2CA,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp741,_tmp740,0,_tmp73F,_tag_dyneither(_tmp2C9,sizeof(void*),0U));});});{
# 1596
struct _handler_cons _tmp2CB;_push_handler(& _tmp2CB);{int _tmp2CD=0;if(setjmp(_tmp2CB.handler))_tmp2CD=1;if(!_tmp2CD){
{void*_tmp2CE=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2D6)).f2;_npop_handler(0U);return _tmp2CE;};_pop_handler();}else{void*_tmp2CC=(void*)_exn_thrown;void*_tmp2CF=_tmp2CC;void*_tmp2D4;if(((struct Cyc_List_Nth_exn_struct*)_tmp2CF)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1599
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2D2=({struct Cyc_Int_pa_PrintArg_struct _tmp54D;_tmp54D.tag=1U,_tmp54D.f1=(unsigned long)((int)_tmp2D6);_tmp54D;});struct Cyc_Int_pa_PrintArg_struct _tmp2D3=({struct Cyc_Int_pa_PrintArg_struct _tmp54C;_tmp54C.tag=1U,({
unsigned long _tmp742=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp54C.f1=_tmp742;});_tmp54C;});void*_tmp2D0[2U];_tmp2D0[0]=& _tmp2D2,_tmp2D0[1]=& _tmp2D3;({struct Cyc_Tcenv_Tenv*_tmp745=te;unsigned int _tmp744=loc;struct _dyneither_ptr _tmp743=({const char*_tmp2D1="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2D1,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp745,_tmp744,0,_tmp743,_tag_dyneither(_tmp2D0,sizeof(void*),2U));});});}else{_LL6: _tmp2D4=_tmp2CF;_LL7:(int)_rethrow(_tmp2D4);}_LL3:;}};};}
# 1604
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1606
struct Cyc_Tcenv_Tenv*_tmp2D7=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp746=Cyc_Tcenv_clear_abstract_val_ok(_tmp2D7);Cyc_Tcexp_tcExp(_tmp746,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp747=Cyc_Tcenv_clear_abstract_val_ok(_tmp2D7);Cyc_Tcexp_tcExp(_tmp747,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2D7,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2DA=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,({
struct _dyneither_ptr _tmp748=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp54E.f1=_tmp748;});_tmp54E;});void*_tmp2D8[1U];_tmp2D8[0]=& _tmp2DA;({struct Cyc_Tcenv_Tenv*_tmp74C=_tmp2D7;unsigned int _tmp74B=e2->loc;void**_tmp74A=topt;struct _dyneither_ptr _tmp749=({const char*_tmp2D9="expecting int subscript, found %s";_tag_dyneither(_tmp2D9,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp74C,_tmp74B,_tmp74A,_tmp749,_tag_dyneither(_tmp2D8,sizeof(void*),1U));});});{
# 1616
void*_tmp2DB=t1;struct Cyc_List_List*_tmp2FF;void*_tmp2FE;struct Cyc_Absyn_Tqual _tmp2FD;void*_tmp2FC;union Cyc_Absyn_Constraint*_tmp2FB;union Cyc_Absyn_Constraint*_tmp2FA;switch(*((int*)_tmp2DB)){case 5U: _LL1: _tmp2FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->f1).elt_typ;_tmp2FD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->f1).elt_tq;_tmp2FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->f1).ptr_atts).rgn;_tmp2FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->f1).ptr_atts).bounds;_tmp2FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->f1).ptr_atts).zero_term;_LL2:
# 1620
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2FA)){
int emit_warning=0;
{void*_tmp2DC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp2FB);void*_tmp2DD=_tmp2DC;struct Cyc_Absyn_Exp*_tmp2EC;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2DD)->tag == 1U){_LL8: _tmp2EC=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2DD)->f1;_LL9: {
# 1624
struct _tuple15 _tmp2DE=Cyc_Evexp_eval_const_uint_exp(_tmp2EC);struct _tuple15 _tmp2DF=_tmp2DE;unsigned int _tmp2EB;int _tmp2EA;_LLD: _tmp2EB=_tmp2DF.f1;_tmp2EA=_tmp2DF.f2;_LLE:;
if(_tmp2EA  && _tmp2EB == 1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2E0=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2E1=_tmp2E0;unsigned int _tmp2E9;int _tmp2E8;_LL10: _tmp2E9=_tmp2E1.f1;_tmp2E8=_tmp2E1.f2;_LL11:;
if(_tmp2E8){
struct _tuple15 _tmp2E2=Cyc_Evexp_eval_const_uint_exp(_tmp2EC);struct _tuple15 _tmp2E3=_tmp2E2;unsigned int _tmp2E7;int _tmp2E6;_LL13: _tmp2E7=_tmp2E3.f1;_tmp2E6=_tmp2E3.f2;_LL14:;
if((_tmp2EA  && _tmp2E7 > _tmp2EB) && _tmp2EB != 1)
({void*_tmp2E4=0U;({unsigned int _tmp74E=loc;struct _dyneither_ptr _tmp74D=({const char*_tmp2E5="subscript will fail at run-time";_tag_dyneither(_tmp2E5,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp74E,_tmp74D,_tag_dyneither(_tmp2E4,sizeof(void*),0U));});});}}
# 1634
goto _LL7;}}else{_LLA: _LLB:
 emit_warning=0;goto _LL7;}_LL7:;}
# 1637
if(emit_warning)
({void*_tmp2ED=0U;({unsigned int _tmp750=e2->loc;struct _dyneither_ptr _tmp74F=({const char*_tmp2EE="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp2EE,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp750,_tmp74F,_tag_dyneither(_tmp2ED,sizeof(void*),0U));});});}else{
# 1641
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2EF=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2F0=_tmp2EF;unsigned int _tmp2F2;int _tmp2F1;_LL16: _tmp2F2=_tmp2F0.f1;_tmp2F1=_tmp2F0.f2;_LL17:;
if(_tmp2F1)
Cyc_Tcutil_check_bound(loc,_tmp2F2,_tmp2FB);}else{
# 1647
if(Cyc_Tcutil_is_bound_one(_tmp2FB) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2FA))
({void*_tmp2F3=0U;({unsigned int _tmp752=e1->loc;struct _dyneither_ptr _tmp751=({const char*_tmp2F4="subscript applied to pointer to one object";_tag_dyneither(_tmp2F4,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp752,_tmp751,_tag_dyneither(_tmp2F3,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2FB);}}
# 1652
Cyc_Tcenv_check_rgn_accessible(_tmp2D7,loc,_tmp2FC);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2FE),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2D7))
({void*_tmp2F5=0U;({unsigned int _tmp754=e1->loc;struct _dyneither_ptr _tmp753=({const char*_tmp2F6="can't subscript an abstract pointer";_tag_dyneither(_tmp2F6,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp754,_tmp753,_tag_dyneither(_tmp2F5,sizeof(void*),0U));});});
return _tmp2FE;case 10U: _LL3: _tmp2FF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2DB)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2D7,loc,_tmp2FF,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp2F9=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({struct _dyneither_ptr _tmp755=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp54F.f1=_tmp755;});_tmp54F;});void*_tmp2F7[1U];_tmp2F7[0]=& _tmp2F9;({struct Cyc_Tcenv_Tenv*_tmp759=_tmp2D7;unsigned int _tmp758=loc;void**_tmp757=topt;struct _dyneither_ptr _tmp756=({const char*_tmp2F8="subscript applied to %s";_tag_dyneither(_tmp2F8,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp759,_tmp758,_tmp757,_tmp756,_tag_dyneither(_tmp2F7,sizeof(void*),1U));});});}_LL0:;};};}
# 1662
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp300=Cyc_Tcutil_compress(*topt);void*_tmp301=_tmp300;struct Cyc_List_List*_tmp305;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp301)->tag == 10U){_LL1: _tmp305=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp301)->f1;_LL2:
# 1668
 if(({int _tmp75A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp305);_tmp75A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1671
goto _LL0;
# 1673
for(0;es != 0;(es=es->tl,_tmp305=_tmp305->tl)){
int bogus=0;
void*_tmp302=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp305))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp75B=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp75B,& _tmp302,(struct Cyc_Absyn_Exp*)es->hd);});
# 1678
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp305->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp75D=({struct Cyc_List_List*_tmp304=_cycalloc(sizeof(*_tmp304));({struct _tuple17*_tmp75C=({struct _tuple17*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->f1=(*((struct _tuple17*)_tmp305->hd)).f1,_tmp303->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp303;});_tmp304->hd=_tmp75C;}),_tmp304->tl=fields;_tmp304;});fields=_tmp75D;});}
# 1681
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1685
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp75E=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp75E,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp761=({struct Cyc_List_List*_tmp307=_cycalloc(sizeof(*_tmp307));({struct _tuple17*_tmp760=({struct _tuple17*_tmp306=_cycalloc(sizeof(*_tmp306));({struct Cyc_Absyn_Tqual _tmp75F=Cyc_Absyn_empty_tqual(0U);_tmp306->f1=_tmp75F;}),_tmp306->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp306;});_tmp307->hd=_tmp760;}),_tmp307->tl=fields;_tmp307;});fields=_tmp761;});}
# 1690
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->tag=10U,({struct Cyc_List_List*_tmp762=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp308->f1=_tmp762;});_tmp308;});}
# 1694
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1697
return({void*_tmp309=0U;({struct Cyc_Tcenv_Tenv*_tmp766=te;unsigned int _tmp765=loc;void**_tmp764=topt;struct _dyneither_ptr _tmp763=({const char*_tmp30A="tcCompoundLit";_tag_dyneither(_tmp30A,sizeof(char),14U);});Cyc_Tcexp_expr_err(_tmp766,_tmp765,_tmp764,_tmp763,_tag_dyneither(_tmp309,sizeof(void*),0U));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1707 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1710
void*res_t2;
int _tmp30B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp30C=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->tag=0U,({union Cyc_Absyn_Cnst _tmp767=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp30B);_tmp322->f1=_tmp767;});_tmp322;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp30C,loc);
# 1718
if(zero_term){
struct Cyc_Absyn_Exp*_tmp30D=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp30B - 1);
if(!Cyc_Tcutil_is_zero(_tmp30D))
({void*_tmp30E=0U;({unsigned int _tmp769=_tmp30D->loc;struct _dyneither_ptr _tmp768=({const char*_tmp30F="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp30F,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp769,_tmp768,_tag_dyneither(_tmp30E,sizeof(void*),0U));});});}
# 1723
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp76D=({void*_tmp76C=res;struct Cyc_Absyn_Tqual _tmp76B=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1724
struct Cyc_Absyn_Exp*_tmp76A=sz_exp;Cyc_Absyn_array_typ(_tmp76C,_tmp76B,_tmp76A,
# 1726
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0U);});
# 1724
res_t2=_tmp76D;});
# 1728
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1731
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1733
({struct Cyc_String_pa_PrintArg_struct _tmp312=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({
struct _dyneither_ptr _tmp76E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));_tmp550.f1=_tmp76E;});_tmp550;});void*_tmp310[1U];_tmp310[0]=& _tmp312;({unsigned int _tmp770=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp76F=({const char*_tmp311="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp311,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp770,_tmp76F,_tag_dyneither(_tmp310,sizeof(void*),1U));});});
# 1736
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1741
void*_tmp313=(void*)ds->hd;void*_tmp314=_tmp313;struct Cyc_Absyn_Exp*_tmp321;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp314)->tag == 1U){_LL1: _LL2:
# 1743
({void*_tmp315=0U;({unsigned int _tmp772=loc;struct _dyneither_ptr _tmp771=({const char*_tmp316="only array index designators are supported";_tag_dyneither(_tmp316,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp772,_tmp771,_tag_dyneither(_tmp315,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp321=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp314)->f1;_LL4:
# 1746
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp321);{
struct _tuple15 _tmp317=Cyc_Evexp_eval_const_uint_exp(_tmp321);struct _tuple15 _tmp318=_tmp317;unsigned int _tmp320;int _tmp31F;_LL6: _tmp320=_tmp318.f1;_tmp31F=_tmp318.f2;_LL7:;
if(!_tmp31F)
({void*_tmp319=0U;({unsigned int _tmp774=_tmp321->loc;struct _dyneither_ptr _tmp773=({const char*_tmp31A="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp31A,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp774,_tmp773,_tag_dyneither(_tmp319,sizeof(void*),0U));});});else{
if(_tmp320 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp31D=({struct Cyc_Int_pa_PrintArg_struct _tmp552;_tmp552.tag=1U,_tmp552.f1=(unsigned long)offset;_tmp552;});struct Cyc_Int_pa_PrintArg_struct _tmp31E=({struct Cyc_Int_pa_PrintArg_struct _tmp551;_tmp551.tag=1U,_tmp551.f1=(unsigned long)((int)_tmp320);_tmp551;});void*_tmp31B[2U];_tmp31B[0]=& _tmp31D,_tmp31B[1]=& _tmp31E;({unsigned int _tmp776=_tmp321->loc;struct _dyneither_ptr _tmp775=({const char*_tmp31C="expecting index designator %d but found %d";_tag_dyneither(_tmp31C,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp776,_tmp775,_tag_dyneither(_tmp31B,sizeof(void*),2U));});});}
# 1753
goto _LL0;};}_LL0:;}}}
# 1757
return res_t2;}
# 1761
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1764
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp323=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp324=_tmp323;void*_tmp329;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp324)->tag == 19U){_LL1: _tmp329=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp324)->f1;_LL2:
# 1769
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp325=({void*_tmp777=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp777,Cyc_Absyn_valueof_exp(_tmp329,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp325->topt=bound->topt;
# 1774
bound=_tmp325;}
# 1776
goto _LL0;}else{_LL3: _LL4:
# 1778
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp328=({struct Cyc_String_pa_PrintArg_struct _tmp553;_tmp553.tag=0U,({
struct _dyneither_ptr _tmp778=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp553.f1=_tmp778;});_tmp553;});void*_tmp326[1U];_tmp326[0]=& _tmp328;({unsigned int _tmp77A=bound->loc;struct _dyneither_ptr _tmp779=({const char*_tmp327="expecting unsigned int, found %s";_tag_dyneither(_tmp327,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_dyneither(_tmp326,sizeof(void*),1U));});});}_LL0:;}
# 1783
if(!(vd->tq).real_const)
({void*_tmp32A=0U;({struct _dyneither_ptr _tmp77B=({const char*_tmp32B="comprehension index variable is not declared const!";_tag_dyneither(_tmp32B,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp77B,_tag_dyneither(_tmp32A,sizeof(void*),0U));});});
# 1786
if(te->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp77C=Cyc_Tcenv_new_block(loc,te);te=_tmp77C;});{
void**_tmp32C=0;
struct Cyc_Absyn_Tqual*_tmp32D=0;
union Cyc_Absyn_Constraint**_tmp32E=0;
# 1792
if(topt != 0){
void*_tmp32F=Cyc_Tcutil_compress(*topt);void*_tmp330=_tmp32F;void*_tmp33B;struct Cyc_Absyn_Tqual _tmp33A;struct Cyc_Absyn_Exp*_tmp339;union Cyc_Absyn_Constraint*_tmp338;struct Cyc_Absyn_PtrInfo _tmp337;switch(*((int*)_tmp330)){case 5U: _LL6: _tmp337=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp330)->f1;_LL7:
# 1795
({void**_tmp77D=({void**_tmp331=_cycalloc(sizeof(*_tmp331));*_tmp331=_tmp337.elt_typ;_tmp331;});_tmp32C=_tmp77D;});
({struct Cyc_Absyn_Tqual*_tmp77E=({struct Cyc_Absyn_Tqual*_tmp332=_cycalloc(sizeof(*_tmp332));*_tmp332=_tmp337.elt_tq;_tmp332;});_tmp32D=_tmp77E;});
({union Cyc_Absyn_Constraint**_tmp77F=({union Cyc_Absyn_Constraint**_tmp333=_cycalloc(sizeof(*_tmp333));*_tmp333=(_tmp337.ptr_atts).zero_term;_tmp333;});_tmp32E=_tmp77F;});
goto _LL5;case 8U: _LL8: _tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp330)->f1).elt_type;_tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp330)->f1).tq;_tmp339=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp330)->f1).num_elts;_tmp338=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp330)->f1).zero_term;_LL9:
# 1800
({void**_tmp780=({void**_tmp334=_cycalloc(sizeof(*_tmp334));*_tmp334=_tmp33B;_tmp334;});_tmp32C=_tmp780;});
({struct Cyc_Absyn_Tqual*_tmp781=({struct Cyc_Absyn_Tqual*_tmp335=_cycalloc(sizeof(*_tmp335));*_tmp335=_tmp33A;_tmp335;});_tmp32D=_tmp781;});
({union Cyc_Absyn_Constraint**_tmp782=({union Cyc_Absyn_Constraint**_tmp336=_cycalloc(sizeof(*_tmp336));*_tmp336=_tmp338;_tmp336;});_tmp32E=_tmp782;});
goto _LL5;default: _LLA: _LLB:
# 1805
 goto _LL5;}_LL5:;}{
# 1808
void*t=Cyc_Tcexp_tcExp(te,_tmp32C,body);
# 1810
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp33C=0U;({unsigned int _tmp784=body->loc;struct _dyneither_ptr _tmp783=({const char*_tmp33D="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp33D,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp784,_tmp783,_tag_dyneither(_tmp33C,sizeof(void*),0U));});});
if(te->le == 0){
# 1814
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp33E=0U;({unsigned int _tmp786=bound->loc;struct _dyneither_ptr _tmp785=({const char*_tmp33F="bound is not constant";_tag_dyneither(_tmp33F,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp786,_tmp785,_tag_dyneither(_tmp33E,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp340=0U;({unsigned int _tmp788=bound->loc;struct _dyneither_ptr _tmp787=({const char*_tmp341="body is not constant";_tag_dyneither(_tmp341,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp788,_tmp787,_tag_dyneither(_tmp340,sizeof(void*),0U));});});}
# 1819
if(_tmp32E != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp32E)){
# 1822
struct Cyc_Absyn_Exp*_tmp342=Cyc_Absyn_uint_exp(1U,0U);_tmp342->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp789=Cyc_Absyn_add_exp(bound,_tmp342,0U);bound=_tmp789;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}
# 1826
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp343=0U;({unsigned int _tmp78B=body->loc;struct _dyneither_ptr _tmp78A=({const char*_tmp344="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp344,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp78B,_tmp78A,_tag_dyneither(_tmp343,sizeof(void*),0U));});});{
# 1830
void*_tmp345=({void*_tmp78E=t;struct Cyc_Absyn_Tqual _tmp78D=_tmp32D == 0?Cyc_Absyn_empty_tqual(0U):*_tmp32D;struct Cyc_Absyn_Exp*_tmp78C=bound;Cyc_Absyn_array_typ(_tmp78E,_tmp78D,_tmp78C,
_tmp32E == 0?Cyc_Absyn_false_conref:*_tmp32E,0U);});
return _tmp345;};};};}
# 1836
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1839
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp346=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp347=_tmp346;void*_tmp34C;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp347)->tag == 19U){_LL1: _tmp34C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp347)->f1;_LL2:
# 1844
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp348=({void*_tmp78F=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp78F,Cyc_Absyn_valueof_exp(_tmp34C,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp348->topt=bound->topt;
# 1849
bound=_tmp348;}
# 1851
goto _LL0;}else{_LL3: _LL4:
# 1853
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp34B=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,({
struct _dyneither_ptr _tmp790=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp554.f1=_tmp790;});_tmp554;});void*_tmp349[1U];_tmp349[0]=& _tmp34B;({unsigned int _tmp792=bound->loc;struct _dyneither_ptr _tmp791=({const char*_tmp34A="expecting unsigned int, found %s";_tag_dyneither(_tmp34A,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp792,_tmp791,_tag_dyneither(_tmp349,sizeof(void*),1U));});});}_LL0:;}{
# 1858
void**_tmp34D=0;
struct Cyc_Absyn_Tqual*_tmp34E=0;
union Cyc_Absyn_Constraint**_tmp34F=0;
# 1862
if(topt != 0){
void*_tmp350=Cyc_Tcutil_compress(*topt);void*_tmp351=_tmp350;void*_tmp35C;struct Cyc_Absyn_Tqual _tmp35B;struct Cyc_Absyn_Exp*_tmp35A;union Cyc_Absyn_Constraint*_tmp359;struct Cyc_Absyn_PtrInfo _tmp358;switch(*((int*)_tmp351)){case 5U: _LL6: _tmp358=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp351)->f1;_LL7:
# 1865
({void**_tmp793=({void**_tmp352=_cycalloc(sizeof(*_tmp352));*_tmp352=_tmp358.elt_typ;_tmp352;});_tmp34D=_tmp793;});
({struct Cyc_Absyn_Tqual*_tmp794=({struct Cyc_Absyn_Tqual*_tmp353=_cycalloc(sizeof(*_tmp353));*_tmp353=_tmp358.elt_tq;_tmp353;});_tmp34E=_tmp794;});
({union Cyc_Absyn_Constraint**_tmp795=({union Cyc_Absyn_Constraint**_tmp354=_cycalloc(sizeof(*_tmp354));*_tmp354=(_tmp358.ptr_atts).zero_term;_tmp354;});_tmp34F=_tmp795;});
goto _LL5;case 8U: _LL8: _tmp35C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp351)->f1).elt_type;_tmp35B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp351)->f1).tq;_tmp35A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp351)->f1).num_elts;_tmp359=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp351)->f1).zero_term;_LL9:
# 1870
({void**_tmp796=({void**_tmp355=_cycalloc(sizeof(*_tmp355));*_tmp355=_tmp35C;_tmp355;});_tmp34D=_tmp796;});
({struct Cyc_Absyn_Tqual*_tmp797=({struct Cyc_Absyn_Tqual*_tmp356=_cycalloc(sizeof(*_tmp356));*_tmp356=_tmp35B;_tmp356;});_tmp34E=_tmp797;});
({union Cyc_Absyn_Constraint**_tmp798=({union Cyc_Absyn_Constraint**_tmp357=_cycalloc(sizeof(*_tmp357));*_tmp357=_tmp359;_tmp357;});_tmp34F=_tmp798;});
goto _LL5;default: _LLA: _LLB:
# 1875
 goto _LL5;}_LL5:;}
# 1878
({unsigned int _tmp79B=loc;struct Cyc_Tcenv_Tenv*_tmp79A=te;struct Cyc_List_List*_tmp799=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp79B,_tmp79A,_tmp799,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp34D != 0)Cyc_Tcutil_unify(*_tmp34D,t);
# 1881
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp35D=0U;({unsigned int _tmp79D=bound->loc;struct _dyneither_ptr _tmp79C=({const char*_tmp35E="bound is not constant";_tag_dyneither(_tmp35E,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp79D,_tmp79C,_tag_dyneither(_tmp35D,sizeof(void*),0U));});});}
# 1885
if(_tmp34F != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp34F)){
# 1888
struct Cyc_Absyn_Exp*_tmp35F=Cyc_Absyn_uint_exp(1U,0U);_tmp35F->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp79E=Cyc_Absyn_add_exp(bound,_tmp35F,0U);bound=_tmp79E;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1892
({void*_tmp360=0U;({unsigned int _tmp7A0=loc;struct _dyneither_ptr _tmp79F=({const char*_tmp361="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp361,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp7A0,_tmp79F,_tag_dyneither(_tmp360,sizeof(void*),0U));});});}{
# 1895
void*_tmp362=({void*_tmp7A3=t;struct Cyc_Absyn_Tqual _tmp7A2=_tmp34E == 0?Cyc_Absyn_empty_tqual(0U):*_tmp34E;struct Cyc_Absyn_Exp*_tmp7A1=bound;Cyc_Absyn_array_typ(_tmp7A3,_tmp7A2,_tmp7A1,
_tmp34F == 0?Cyc_Absyn_false_conref:*_tmp34F,0U);});
return _tmp362;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1910 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1914
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp7A4=({struct Cyc_Absyn_Aggrdecl**_tmp363=_cycalloc(sizeof(*_tmp363));*_tmp363=ad;_tmp363;});adptr=_tmp7A4;});}else{
# 1920
{struct _handler_cons _tmp364;_push_handler(& _tmp364);{int _tmp366=0;if(setjmp(_tmp364.handler))_tmp366=1;if(!_tmp366){({struct Cyc_Absyn_Aggrdecl**_tmp7A5=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp7A5;});
ad=*adptr;
# 1920
;_pop_handler();}else{void*_tmp365=(void*)_exn_thrown;void*_tmp367=_tmp365;void*_tmp36B;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp367)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1923
({struct Cyc_String_pa_PrintArg_struct _tmp36A=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0U,({struct _dyneither_ptr _tmp7A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp555.f1=_tmp7A6;});_tmp555;});void*_tmp368[1U];_tmp368[0]=& _tmp36A;({unsigned int _tmp7A8=loc;struct _dyneither_ptr _tmp7A7=({const char*_tmp369="unbound struct/union name %s";_tag_dyneither(_tmp369,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp7A8,_tmp7A7,_tag_dyneither(_tmp368,sizeof(void*),1U));});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL3: _tmp36B=_tmp367;_LL4:(int)_rethrow(_tmp36B);}_LL0:;}};}
# 1926
*ad_opt=ad;
*tn=ad->name;}
# 1929
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp36E=({struct Cyc_String_pa_PrintArg_struct _tmp556;_tmp556.tag=0U,({struct _dyneither_ptr _tmp7A9=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp36F="struct";_tag_dyneither(_tmp36F,sizeof(char),7U);}):({const char*_tmp370="union";_tag_dyneither(_tmp370,sizeof(char),6U);}));_tmp556.f1=_tmp7A9;});_tmp556;});void*_tmp36C[1U];_tmp36C[0]=& _tmp36E;({unsigned int _tmp7AB=loc;struct _dyneither_ptr _tmp7AA=({const char*_tmp36D="can't construct abstract %s";_tag_dyneither(_tmp36D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp7AB,_tmp7AA,_tag_dyneither(_tmp36C,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1937
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp371=Cyc_Tcenv_new_status(te);
if(_tmp371 == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp7AC=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp7AC;});else{
# 1946
({struct Cyc_Tcenv_Tenv*_tmp7AD=Cyc_Tcenv_set_new_status(_tmp371,te);te2=_tmp7AD;});}{
# 1948
struct _tuple12 _tmp372=({struct _tuple12 _tmp55C;({struct Cyc_List_List*_tmp7AE=Cyc_Tcenv_lookup_type_vars(te2);_tmp55C.f1=_tmp7AE;}),_tmp55C.f2=Cyc_Core_heap_region;_tmp55C;});
struct Cyc_List_List*_tmp373=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp372,ad->tvs);
struct Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp372,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp375=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp373);
struct Cyc_List_List*_tmp376=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp374);
struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp373,_tmp374);
void*res_typ;
# 1959
if(topt != 0){
void*_tmp378=Cyc_Tcutil_compress(*topt);void*_tmp379=_tmp378;struct Cyc_Absyn_Aggrdecl**_tmp37D;struct Cyc_List_List*_tmp37C;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp379)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp379)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp37D=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp379)->f1).aggr_info).KnownAggr).val;_tmp37C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp379)->f1).targs;_LL7:
# 1962
 if(*_tmp37D == *adptr){
{struct Cyc_List_List*_tmp37A=_tmp375;for(0;_tmp37A != 0  && _tmp37C != 0;(
_tmp37A=_tmp37A->tl,_tmp37C=_tmp37C->tl)){
Cyc_Tcutil_unify((void*)_tmp37A->hd,(void*)_tmp37C->hd);}}
# 1967
res_typ=*topt;
goto _LL5;}
# 1970
goto _LL9;}else{goto _LL8;}}else{_LL8: _LL9:
# 1972
({void*_tmp7B0=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7AF=Cyc_Absyn_KnownAggr(adptr);(_tmp37B->f1).aggr_info=_tmp7AF;}),(_tmp37B->f1).targs=_tmp375;_tmp37B;});res_typ=_tmp7B0;});}_LL5:;}else{
# 1975
({void*_tmp7B2=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7B1=Cyc_Absyn_KnownAggr(adptr);(_tmp37E->f1).aggr_info=_tmp7B1;}),(_tmp37E->f1).targs=_tmp375;_tmp37E;});res_typ=_tmp7B2;});}{
# 1978
struct Cyc_List_List*_tmp37F=*ts;
struct Cyc_List_List*_tmp380=_tmp376;
while(_tmp37F != 0  && _tmp380 != 0){
# 1982
({unsigned int _tmp7B5=loc;struct Cyc_Tcenv_Tenv*_tmp7B4=te2;struct Cyc_List_List*_tmp7B3=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp7B5,_tmp7B4,_tmp7B3,& Cyc_Tcutil_ak,1,0,(void*)_tmp37F->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp37F->hd);
Cyc_Tcutil_unify((void*)_tmp37F->hd,(void*)_tmp380->hd);
_tmp37F=_tmp37F->tl;
_tmp380=_tmp380->tl;}
# 1988
if(_tmp37F != 0)
({void*_tmp381=0U;({unsigned int _tmp7B7=loc;struct _dyneither_ptr _tmp7B6=({const char*_tmp382="too many explicit witness types";_tag_dyneither(_tmp382,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7B7,_tmp7B6,_tag_dyneither(_tmp381,sizeof(void*),0U));});});
# 1991
*ts=_tmp376;{
# 1994
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp383=(struct _tuple19*)fields->hd;struct _tuple19*_tmp384=_tmp383;struct Cyc_Absyn_Aggrfield*_tmp390;struct Cyc_Absyn_Exp*_tmp38F;_LLB: _tmp390=_tmp384->f1;_tmp38F=_tmp384->f2;_LLC:;{
void*_tmp385=Cyc_Tcutil_substitute(_tmp377,_tmp390->type);
({struct Cyc_Tcenv_Tenv*_tmp7B8=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7B8,& _tmp385,_tmp38F);});
# 2005
if(!Cyc_Tcutil_coerce_arg(te2,_tmp38F,_tmp385,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp388=({struct Cyc_String_pa_PrintArg_struct _tmp55B;_tmp55B.tag=0U,_tmp55B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp390->name);_tmp55B;});struct Cyc_String_pa_PrintArg_struct _tmp389=({struct Cyc_String_pa_PrintArg_struct _tmp55A;_tmp55A.tag=0U,({
struct _dyneither_ptr _tmp7B9=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp38D="struct";_tag_dyneither(_tmp38D,sizeof(char),7U);}):({const char*_tmp38E="union";_tag_dyneither(_tmp38E,sizeof(char),6U);}));_tmp55A.f1=_tmp7B9;});_tmp55A;});struct Cyc_String_pa_PrintArg_struct _tmp38A=({struct Cyc_String_pa_PrintArg_struct _tmp559;_tmp559.tag=0U,({
struct _dyneither_ptr _tmp7BA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp559.f1=_tmp7BA;});_tmp559;});struct Cyc_String_pa_PrintArg_struct _tmp38B=({struct Cyc_String_pa_PrintArg_struct _tmp558;_tmp558.tag=0U,({struct _dyneither_ptr _tmp7BB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp385));_tmp558.f1=_tmp7BB;});_tmp558;});struct Cyc_String_pa_PrintArg_struct _tmp38C=({struct Cyc_String_pa_PrintArg_struct _tmp557;_tmp557.tag=0U,({
struct _dyneither_ptr _tmp7BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp38F->topt)));_tmp557.f1=_tmp7BC;});_tmp557;});void*_tmp386[5U];_tmp386[0]=& _tmp388,_tmp386[1]=& _tmp389,_tmp386[2]=& _tmp38A,_tmp386[3]=& _tmp38B,_tmp386[4]=& _tmp38C;({unsigned int _tmp7BE=_tmp38F->loc;struct _dyneither_ptr _tmp7BD=({const char*_tmp387="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp387,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp7BE,_tmp7BD,_tag_dyneither(_tmp386,sizeof(void*),5U));});});
Cyc_Tcutil_explain_failure();}};}{
# 2014
struct Cyc_List_List*_tmp391=0;
{struct Cyc_List_List*_tmp392=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp392 != 0;_tmp392=_tmp392->tl){
({struct Cyc_List_List*_tmp7C2=({struct Cyc_List_List*_tmp394=_cycalloc(sizeof(*_tmp394));({struct _tuple0*_tmp7C1=({struct _tuple0*_tmp393=_cycalloc(sizeof(*_tmp393));({void*_tmp7C0=Cyc_Tcutil_substitute(_tmp377,(*((struct _tuple0*)_tmp392->hd)).f1);_tmp393->f1=_tmp7C0;}),({
void*_tmp7BF=Cyc_Tcutil_substitute(_tmp377,(*((struct _tuple0*)_tmp392->hd)).f2);_tmp393->f2=_tmp7BF;});_tmp393;});
# 2016
_tmp394->hd=_tmp7C1;}),_tmp394->tl=_tmp391;_tmp394;});_tmp391=_tmp7C2;});}}
# 2019
({struct Cyc_List_List*_tmp7C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp391);_tmp391=_tmp7C3;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp391);
return res_typ;};};};};};}
# 2025
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2027
{void*_tmp395=Cyc_Tcutil_compress(ts);void*_tmp396=_tmp395;enum Cyc_Absyn_AggrKind _tmp3A5;struct Cyc_List_List*_tmp3A4;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp396)->tag == 12U){_LL1: _tmp3A5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp396)->f1;_tmp3A4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp396)->f2;_LL2:
# 2029
 if(_tmp3A5 == Cyc_Absyn_UnionA)
({void*_tmp397=0U;({unsigned int _tmp7C5=loc;struct _dyneither_ptr _tmp7C4=({const char*_tmp398="expecting struct but found union";_tag_dyneither(_tmp398,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7C5,_tmp7C4,_tag_dyneither(_tmp397,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp3A4);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp399=(struct _tuple19*)fields->hd;struct _tuple19*_tmp39A=_tmp399;struct Cyc_Absyn_Aggrfield*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A0;_LL6: _tmp3A1=_tmp39A->f1;_tmp3A0=_tmp39A->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp7C7=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp7C6=& _tmp3A1->type;Cyc_Tcexp_tcExpInitializer(_tmp7C7,_tmp7C6,_tmp3A0);});
# 2038
if(!Cyc_Tcutil_coerce_arg(te,_tmp3A0,_tmp3A1->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp39D=({struct Cyc_String_pa_PrintArg_struct _tmp55F;_tmp55F.tag=0U,_tmp55F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3A1->name);_tmp55F;});struct Cyc_String_pa_PrintArg_struct _tmp39E=({struct Cyc_String_pa_PrintArg_struct _tmp55E;_tmp55E.tag=0U,({
struct _dyneither_ptr _tmp7C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3A1->type));_tmp55E.f1=_tmp7C8;});_tmp55E;});struct Cyc_String_pa_PrintArg_struct _tmp39F=({struct Cyc_String_pa_PrintArg_struct _tmp55D;_tmp55D.tag=0U,({
struct _dyneither_ptr _tmp7C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp3A0->topt)));_tmp55D.f1=_tmp7C9;});_tmp55D;});void*_tmp39B[3U];_tmp39B[0]=& _tmp39D,_tmp39B[1]=& _tmp39E,_tmp39B[2]=& _tmp39F;({unsigned int _tmp7CB=_tmp3A0->loc;struct _dyneither_ptr _tmp7CA=({const char*_tmp39C="field %s of struct expects type %s != %s";_tag_dyneither(_tmp39C,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp7CB,_tmp7CA,_tag_dyneither(_tmp39B,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2045
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp3A2=0U;({struct _dyneither_ptr _tmp7CC=({const char*_tmp3A3="tcAnonStruct: wrong type";_tag_dyneither(_tmp3A3,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7CC,_tag_dyneither(_tmp3A2,sizeof(void*),0U));});});}_LL0:;}
# 2048
return ts;}
# 2052
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2055
struct _tuple12 _tmp3A6=({struct _tuple12 _tmp565;({struct Cyc_List_List*_tmp7CD=Cyc_Tcenv_lookup_type_vars(te);_tmp565.f1=_tmp7CD;}),_tmp565.f2=Cyc_Core_heap_region;_tmp565;});
struct Cyc_List_List*_tmp3A7=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp3A6,tud->tvs);
struct Cyc_List_List*_tmp3A8=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3A7);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));
_tmp3B7->tag=4U,({union Cyc_Absyn_DatatypeFieldInfoU _tmp7CE=Cyc_Absyn_KnownDatatypefield(tud,tuf);(_tmp3B7->f1).field_info=_tmp7CE;}),(_tmp3B7->f1).targs=_tmp3A8;_tmp3B7;});
# 2062
if(topt != 0){
void*_tmp3A9=Cyc_Tcutil_compress(*topt);void*_tmp3AA=_tmp3A9;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3AA)->tag == 4U){_LL1: _LL2:
 Cyc_Tcutil_unify(*topt,res);goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2068
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp3A7 != 0)({void*_tmp7CF=Cyc_Tcutil_substitute(_tmp3A7,t);t=_tmp7CF;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3AD=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0U,({
# 2078
struct _dyneither_ptr _tmp7D0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp562.f1=_tmp7D0;});_tmp562;});struct Cyc_String_pa_PrintArg_struct _tmp3AE=({struct Cyc_String_pa_PrintArg_struct _tmp561;_tmp561.tag=0U,({struct _dyneither_ptr _tmp7D1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp561.f1=_tmp7D1;});_tmp561;});struct Cyc_String_pa_PrintArg_struct _tmp3AF=({struct Cyc_String_pa_PrintArg_struct _tmp560;_tmp560.tag=0U,({
struct _dyneither_ptr _tmp7D2=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3B0="?";_tag_dyneither(_tmp3B0,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp560.f1=_tmp7D2;});_tmp560;});void*_tmp3AB[3U];_tmp3AB[0]=& _tmp3AD,_tmp3AB[1]=& _tmp3AE,_tmp3AB[2]=& _tmp3AF;({unsigned int _tmp7D4=e->loc;struct _dyneither_ptr _tmp7D3=({const char*_tmp3AC="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3AC,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp7D4,_tmp7D3,_tag_dyneither(_tmp3AB,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2083
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3B3=({struct Cyc_String_pa_PrintArg_struct _tmp563;_tmp563.tag=0U,({
# 2086
struct _dyneither_ptr _tmp7D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp563.f1=_tmp7D5;});_tmp563;});void*_tmp3B1[1U];_tmp3B1[0]=& _tmp3B3;({struct Cyc_Tcenv_Tenv*_tmp7D9=te;unsigned int _tmp7D8=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7D7=topt;struct _dyneither_ptr _tmp7D6=({const char*_tmp3B2="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3B2,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp7D9,_tmp7D8,_tmp7D7,_tmp7D6,_tag_dyneither(_tmp3B1,sizeof(void*),1U));});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3B6=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0U,({
struct _dyneither_ptr _tmp7DA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp564.f1=_tmp7DA;});_tmp564;});void*_tmp3B4[1U];_tmp3B4[0]=& _tmp3B6;({struct Cyc_Tcenv_Tenv*_tmp7DE=te;unsigned int _tmp7DD=loc;void**_tmp7DC=topt;struct _dyneither_ptr _tmp7DB=({const char*_tmp3B5="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3B5,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7DE,_tmp7DD,_tmp7DC,_tmp7DB,_tag_dyneither(_tmp3B4,sizeof(void*),1U));});});
return res;};}
# 2093
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2095
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2097
if(topt != 0){
void*_tmp3B8=Cyc_Tcutil_compress(*topt);void*_tmp3B9=_tmp3B8;void*_tmp3BA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->tag == 5U){_LL1: _tmp3BA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->f1).elt_typ;_LL2:
# 2100
 Cyc_Tcutil_unify(_tmp3BA,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2106
({struct Cyc_String_pa_PrintArg_struct _tmp3BD=({struct Cyc_String_pa_PrintArg_struct _tmp567;_tmp567.tag=0U,({
struct _dyneither_ptr _tmp7DF=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3BF="calloc";_tag_dyneither(_tmp3BF,sizeof(char),7U);}):(struct _dyneither_ptr)({const char*_tmp3C0="malloc";_tag_dyneither(_tmp3C0,sizeof(char),7U);})));_tmp567.f1=_tmp7DF;});_tmp567;});struct Cyc_String_pa_PrintArg_struct _tmp3BE=({struct Cyc_String_pa_PrintArg_struct _tmp566;_tmp566.tag=0U,({struct _dyneither_ptr _tmp7E0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp566.f1=_tmp7E0;});_tmp566;});void*_tmp3BB[2U];_tmp3BB[0]=& _tmp3BD,_tmp3BB[1]=& _tmp3BE;({unsigned int _tmp7E2=loc;struct _dyneither_ptr _tmp7E1=({const char*_tmp3BC="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3BC,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp7E2,_tmp7E1,_tag_dyneither(_tmp3BB,sizeof(void*),2U));});});}
# 2110
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2112
enum Cyc_Absyn_AliasQual _tmp3C1=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3C2=_tmp3C1;if(_tmp3C2 == Cyc_Absyn_Unique){_LL1: _LL2:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL3: _LL4:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL0:;}
# 2118
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2123
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2126
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));
_tmp3C9->tag=15U,({void*_tmp7E3=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3C9->f1=_tmp7E3;});_tmp3C9;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3C3=Cyc_Tcutil_compress(handle_type);void*_tmp3C4=_tmp3C3;void*_tmp3C8;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3C4)->tag == 15U){_LL1: _tmp3C8=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3C4)->f1;_LL2:
# 2131
 rgn=_tmp3C8;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2135
({struct Cyc_String_pa_PrintArg_struct _tmp3C7=({struct Cyc_String_pa_PrintArg_struct _tmp568;_tmp568.tag=0U,({
struct _dyneither_ptr _tmp7E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp568.f1=_tmp7E4;});_tmp568;});void*_tmp3C5[1U];_tmp3C5[0]=& _tmp3C7;({unsigned int _tmp7E6=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7E5=({const char*_tmp3C6="expecting region_t type but found %s";_tag_dyneither(_tmp3C6,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7E6,_tmp7E5,_tag_dyneither(_tmp3C5,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2142
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7E7=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7E7;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2149
({struct Cyc_Tcenv_Tenv*_tmp7E8=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7E8,& Cyc_Absyn_uint_typ,*e);});{
# 2158 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3CA=0U;({struct _dyneither_ptr _tmp7E9=({const char*_tmp3CB="calloc with empty type";_tag_dyneither(_tmp3CB,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7E9,_tag_dyneither(_tmp3CA,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7EC=loc;struct Cyc_Tcenv_Tenv*_tmp7EB=te;struct Cyc_List_List*_tmp7EA=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7EC,_tmp7EB,_tmp7EA,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2170
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3CC=er;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E0;void*_tmp3DF;switch(*((int*)_tmp3CC)){case 17U: _LL6: _tmp3DF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3CC)->f1;_LL7:
# 2174
 elt_type=_tmp3DF;
({void**_tmp7ED=({void**_tmp3CD=_cycalloc(sizeof(*_tmp3CD));*_tmp3CD=elt_type;_tmp3CD;});*t=_tmp7ED;});
({struct Cyc_Absyn_Exp*_tmp7EE=Cyc_Absyn_uint_exp(1U,0U);num_elts=_tmp7EE;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CC)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CC)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CC)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CC)->f2)->tl)->tl == 0){_LL8: _tmp3E1=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CC)->f2)->hd;_tmp3E0=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CC)->f2)->tl)->hd;_LL9:
# 2181
{struct _tuple0 _tmp3CE=({struct _tuple0 _tmp569;_tmp569.f1=_tmp3E1->r,_tmp569.f2=_tmp3E0->r;_tmp569;});struct _tuple0 _tmp3CF=_tmp3CE;void*_tmp3D3;void*_tmp3D2;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3CF.f1)->tag == 17U){_LLD: _tmp3D2=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3CF.f1)->f1;_LLE:
# 2183
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3D2);
elt_type=_tmp3D2;
({void**_tmp7EF=({void**_tmp3D0=_cycalloc(sizeof(*_tmp3D0));*_tmp3D0=elt_type;_tmp3D0;});*t=_tmp7EF;});
num_elts=_tmp3E0;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3CF.f2)->tag == 17U){_LLF: _tmp3D3=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3CF.f2)->f1;_LL10:
# 2190
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3D3);
elt_type=_tmp3D3;
({void**_tmp7F0=({void**_tmp3D1=_cycalloc(sizeof(*_tmp3D1));*_tmp3D1=elt_type;_tmp3D1;});*t=_tmp7F0;});
num_elts=_tmp3E1;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2198
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2200
 No_sizeof: {
# 2203
struct Cyc_Absyn_Exp*_tmp3D4=*e;
{void*_tmp3D5=_tmp3D4->r;void*_tmp3D6=_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D7;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->tag == 14U){_LL14: _tmp3D7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL15:
 _tmp3D4=_tmp3D7;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2208
{void*_tmp3D8=Cyc_Tcutil_compress((void*)_check_null(_tmp3D4->topt));void*_tmp3D9=_tmp3D8;void*_tmp3DD;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3D9)->tag == 19U){_LL19: _tmp3DD=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3D9)->f1;_LL1A:
# 2210
{void*_tmp3DA=Cyc_Tcutil_compress(_tmp3DD);void*_tmp3DB=_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DC;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3DB)->tag == 18U){_LL1E: _tmp3DC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3DB)->f1;_LL1F:
# 2212
 er=_tmp3DC->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2215
goto _LL18;}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2218
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7F1=({void**_tmp3DE=_cycalloc(sizeof(*_tmp3DE));*_tmp3DE=elt_type;_tmp3DE;});*t=_tmp7F1;});
num_elts=*e;
one_elt=0;}
# 2223
goto _LL5;}_LL5:;}}
# 2227
*is_fat=!one_elt;
# 2230
{void*_tmp3E2=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3E5;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2)->f1).aggr_info).KnownAggr).tag == 2){_LL23: _tmp3E5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2)->f1).aggr_info).KnownAggr).val;_LL24:
# 2232
 if(_tmp3E5->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3E5->impl))->exist_vars != 0)
({void*_tmp3E3=0U;({unsigned int _tmp7F3=loc;struct _dyneither_ptr _tmp7F2=({const char*_tmp3E4="malloc with existential types not yet implemented";_tag_dyneither(_tmp3E4,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp7F3,_tmp7F2,_tag_dyneither(_tmp3E3,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2239
void*(*_tmp3E6)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp3E7=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp3E8=Cyc_Tcutil_compress(*topt);void*_tmp3E9=_tmp3E8;union Cyc_Absyn_Constraint*_tmp3F6;union Cyc_Absyn_Constraint*_tmp3F5;union Cyc_Absyn_Constraint*_tmp3F4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9)->tag == 5U){_LL28: _tmp3F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9)->f1).ptr_atts).nullable;_tmp3F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9)->f1).ptr_atts).bounds;_tmp3F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9)->f1).ptr_atts).zero_term;_LL29:
# 2244
 _tmp3E7=_tmp3F4;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F6))
_tmp3E6=Cyc_Absyn_star_typ;
# 2249
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F4) && !(*is_calloc)){
({void*_tmp3EA=0U;({unsigned int _tmp7F5=loc;struct _dyneither_ptr _tmp7F4=({const char*_tmp3EB="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp3EB,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp7F5,_tmp7F4,_tag_dyneither(_tmp3EA,sizeof(void*),0U));});});
*is_calloc=1;}
# 2255
{void*_tmp3EC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3F5);void*_tmp3ED=_tmp3EC;struct Cyc_Absyn_Exp*_tmp3F3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3ED)->tag == 0U){_LL2D: _LL2E:
 goto _LL2C;}else{_LL2F: _tmp3F3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3ED)->f1;if(!one_elt){_LL30: {
# 2258
int _tmp3EE=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp3EE  && Cyc_Evexp_same_const_exp(_tmp3F3,num_elts)){
*is_fat=0;
return({void*_tmp7F9=elt_type;void*_tmp7F8=rgn;struct Cyc_Absyn_Tqual _tmp7F7=Cyc_Absyn_empty_tqual(0U);void*_tmp7F6=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3F5);Cyc_Absyn_atb_typ(_tmp7F9,_tmp7F8,_tmp7F7,_tmp7F6,_tmp3E7);});}
# 2264
{void*_tmp3EF=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp3F0=_tmp3EF;void*_tmp3F2;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F0)->tag == 19U){_LL34: _tmp3F2=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F0)->f1;_LL35: {
# 2266
struct Cyc_Absyn_Exp*_tmp3F1=({void*_tmp7FA=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp7FA,Cyc_Absyn_valueof_exp(_tmp3F2,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2268
if(Cyc_Evexp_same_const_exp(_tmp3F1,_tmp3F3)){
*is_fat=0;
return({void*_tmp7FE=elt_type;void*_tmp7FD=rgn;struct Cyc_Absyn_Tqual _tmp7FC=Cyc_Absyn_empty_tqual(0U);void*_tmp7FB=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3F5);Cyc_Absyn_atb_typ(_tmp7FE,_tmp7FD,_tmp7FC,_tmp7FB,_tmp3E7);});}
# 2273
goto _LL33;}}else{_LL36: _LL37:
 goto _LL33;}_LL33:;}
# 2276
goto _LL2C;}}else{_LL31: _LL32:
 goto _LL2C;}}_LL2C:;}
# 2279
goto _LL27;}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2282
if(!one_elt)_tmp3E6=Cyc_Absyn_dyneither_typ;
return({void*(*_tmp802)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=_tmp3E6;void*_tmp801=elt_type;void*_tmp800=rgn;struct Cyc_Absyn_Tqual _tmp7FF=Cyc_Absyn_empty_tqual(0U);_tmp802(_tmp801,_tmp800,_tmp7FF,_tmp3E7);});};};}
# 2287
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2289
struct Cyc_Tcenv_Tenv*_tmp3F7=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp3F7,0,e1);{
void*_tmp3F8=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp3F7,& _tmp3F8,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2296
{void*_tmp3F9=Cyc_Tcutil_compress(t1);void*_tmp3FA=_tmp3F9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3FA)->tag == 8U){_LL1: _LL2:
({void*_tmp3FB=0U;({unsigned int _tmp804=loc;struct _dyneither_ptr _tmp803=({const char*_tmp3FC="cannot assign to an array";_tag_dyneither(_tmp3FC,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp804,_tmp803,_tag_dyneither(_tmp3FB,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2301
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp3FD=0U;({unsigned int _tmp806=loc;struct _dyneither_ptr _tmp805=({const char*_tmp3FE="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp3FE,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp806,_tmp805,_tag_dyneither(_tmp3FD,sizeof(void*),0U));});});
# 2305
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp3FF=0U;({struct Cyc_Tcenv_Tenv*_tmp80A=te;unsigned int _tmp809=e1->loc;void**_tmp808=topt;struct _dyneither_ptr _tmp807=({const char*_tmp400="swap non-lvalue";_tag_dyneither(_tmp400,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp80A,_tmp809,_tmp808,_tmp807,_tag_dyneither(_tmp3FF,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp401=0U;({struct Cyc_Tcenv_Tenv*_tmp80E=te;unsigned int _tmp80D=e2->loc;void**_tmp80C=topt;struct _dyneither_ptr _tmp80B=({const char*_tmp402="swap non-lvalue";_tag_dyneither(_tmp402,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp80E,_tmp80D,_tmp80C,_tmp80B,_tag_dyneither(_tmp401,sizeof(void*),0U));});});{
# 2310
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp403=0U;({struct Cyc_Tcenv_Tenv*_tmp812=te;unsigned int _tmp811=e1->loc;void**_tmp810=topt;struct _dyneither_ptr _tmp80F=({const char*_tmp404="swap value in zeroterm array";_tag_dyneither(_tmp404,sizeof(char),29U);});Cyc_Tcexp_expr_err(_tmp812,_tmp811,_tmp810,_tmp80F,_tag_dyneither(_tmp403,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp405=0U;({struct Cyc_Tcenv_Tenv*_tmp816=te;unsigned int _tmp815=e2->loc;void**_tmp814=topt;struct _dyneither_ptr _tmp813=({const char*_tmp406="swap value in zeroterm array";_tag_dyneither(_tmp406,sizeof(char),29U);});Cyc_Tcexp_expr_err(_tmp816,_tmp815,_tmp814,_tmp813,_tag_dyneither(_tmp405,sizeof(void*),0U));});});
# 2317
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp407=({struct Cyc_String_pa_PrintArg_struct _tmp40A=({struct Cyc_String_pa_PrintArg_struct _tmp56B;_tmp56B.tag=0U,({
struct _dyneither_ptr _tmp817=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp56B.f1=_tmp817;});_tmp56B;});struct Cyc_String_pa_PrintArg_struct _tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmp56A;_tmp56A.tag=0U,({struct _dyneither_ptr _tmp818=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp56A.f1=_tmp818;});_tmp56A;});void*_tmp408[2U];_tmp408[0]=& _tmp40A,_tmp408[1]=& _tmp40B;({struct Cyc_Tcenv_Tenv*_tmp81C=te;unsigned int _tmp81B=loc;void**_tmp81A=topt;struct _dyneither_ptr _tmp819=({const char*_tmp409="type mismatch: %s != %s";_tag_dyneither(_tmp409,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp81C,_tmp81B,_tmp81A,_tmp819,_tag_dyneither(_tmp408,sizeof(void*),2U));});});
return _tmp407;}
# 2324
return(void*)& Cyc_Absyn_VoidType_val;};};};}
# 2329
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp81D=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp81D,s,1);});
# 2332
while(1){
void*_tmp40C=s->r;void*_tmp40D=_tmp40C;struct Cyc_Absyn_Decl*_tmp418;struct Cyc_Absyn_Stmt*_tmp417;struct Cyc_Absyn_Stmt*_tmp416;struct Cyc_Absyn_Stmt*_tmp415;struct Cyc_Absyn_Exp*_tmp414;switch(*((int*)_tmp40D)){case 1U: _LL1: _tmp414=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp40D)->f1;_LL2: {
# 2336
void*_tmp40E=(void*)_check_null(_tmp414->topt);
if(!({void*_tmp81E=_tmp40E;Cyc_Tcutil_unify(_tmp81E,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp411=({struct Cyc_String_pa_PrintArg_struct _tmp56C;_tmp56C.tag=0U,({
struct _dyneither_ptr _tmp81F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp40E));_tmp56C.f1=_tmp81F;});_tmp56C;});void*_tmp40F[1U];_tmp40F[0]=& _tmp411;({unsigned int _tmp821=loc;struct _dyneither_ptr _tmp820=({const char*_tmp410="statement expression returns type %s";_tag_dyneither(_tmp410,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp821,_tmp820,_tag_dyneither(_tmp40F,sizeof(void*),1U));});});
Cyc_Tcutil_explain_failure();}
# 2342
return _tmp40E;}case 2U: _LL3: _tmp416=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp40D)->f1;_tmp415=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp40D)->f2;_LL4:
 s=_tmp415;continue;case 12U: _LL5: _tmp418=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp40D)->f1;_tmp417=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp40D)->f2;_LL6:
 s=_tmp417;continue;default: _LL7: _LL8:
# 2346
 return({void*_tmp412=0U;({struct Cyc_Tcenv_Tenv*_tmp825=te;unsigned int _tmp824=loc;void**_tmp823=topt;struct _dyneither_ptr _tmp822=({const char*_tmp413="statement expression must end with expression";_tag_dyneither(_tmp413,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp825,_tmp824,_tmp823,_tmp822,_tag_dyneither(_tmp412,sizeof(void*),0U));});});}_LL0:;}}
# 2351
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp826=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp826,0,e);}));
{void*_tmp419=t;struct Cyc_Absyn_Aggrdecl*_tmp41D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp419)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp419)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp41D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp419)->f1).aggr_info).KnownAggr).val;_LL2:
# 2355
 if((_tmp41D->kind == Cyc_Absyn_UnionA  && _tmp41D->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp41D->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{_LL3: _LL4:
# 2358
({struct Cyc_String_pa_PrintArg_struct _tmp41C=({struct Cyc_String_pa_PrintArg_struct _tmp56D;_tmp56D.tag=0U,({struct _dyneither_ptr _tmp827=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp56D.f1=_tmp827;});_tmp56D;});void*_tmp41A[1U];_tmp41A[0]=& _tmp41C;({unsigned int _tmp829=loc;struct _dyneither_ptr _tmp828=({const char*_tmp41B="expecting @tagged union but found %s";_tag_dyneither(_tmp41B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp829,_tmp828,_tag_dyneither(_tmp41A,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2361
return Cyc_Absyn_uint_typ;}
# 2365
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2369
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct Cyc_Tcenv_Tenv*_tmp41E=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2373
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp425=_cycalloc(sizeof(*_tmp425));
_tmp425->tag=15U,({void*_tmp82A=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp41E));_tmp425->f1=_tmp82A;});_tmp425;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp41E,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp41F=Cyc_Tcutil_compress(handle_type);void*_tmp420=_tmp41F;void*_tmp424;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp420)->tag == 15U){_LL1: _tmp424=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp420)->f1;_LL2:
# 2378
 rgn=_tmp424;
Cyc_Tcenv_check_rgn_accessible(_tmp41E,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2382
({struct Cyc_String_pa_PrintArg_struct _tmp423=({struct Cyc_String_pa_PrintArg_struct _tmp56E;_tmp56E.tag=0U,({
struct _dyneither_ptr _tmp82B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp56E.f1=_tmp82B;});_tmp56E;});void*_tmp421[1U];_tmp421[0]=& _tmp423;({unsigned int _tmp82D=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp82C=({const char*_tmp422="expecting region_t type but found %s";_tag_dyneither(_tmp422,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp82D,_tmp82C,_tag_dyneither(_tmp421,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2389
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp82E=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp82E;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2397
void*_tmp426=e1->r;void*_tmp427=_tmp426;struct Cyc_List_List*_tmp441;struct Cyc_Core_Opt*_tmp440;struct Cyc_List_List*_tmp43F;switch(*((int*)_tmp427)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2402
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp41E,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp428=0U;({struct _dyneither_ptr _tmp82F=({const char*_tmp429="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp429,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp82F,_tag_dyneither(_tmp428,sizeof(void*),0U));});});
({void*_tmp830=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp830;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp41E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp41E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2413
return res_typ;}case 36U: _LLA: _tmp440=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp427)->f1;_tmp43F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp427)->f2;_LLB:
# 2415
({void*_tmp831=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->tag=26U,_tmp42A->f1=_tmp43F;_tmp42A;});e1->r=_tmp831;});
_tmp441=_tmp43F;goto _LLD;case 26U: _LLC: _tmp441=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp427)->f1;_LLD: {
# 2418
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp42B=Cyc_Tcutil_compress(*topt);void*_tmp42C=_tmp42B;void**_tmp42F;struct Cyc_Absyn_Tqual _tmp42E;union Cyc_Absyn_Constraint*_tmp42D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42C)->tag == 5U){_LL15: _tmp42F=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42C)->f1).elt_typ;_tmp42E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42C)->f1).elt_tq;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42C)->f1).ptr_atts).zero_term;_LL16:
# 2424
 elt_typ_opt=_tmp42F;
({int _tmp832=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp42D);zero_term=_tmp832;});
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2430
void*res_typ=Cyc_Tcexp_tcArray(_tmp41E,e1->loc,elt_typ_opt,0,zero_term,_tmp441);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp430=0U;({struct _dyneither_ptr _tmp833=({const char*_tmp431="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp431,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp833,_tag_dyneither(_tmp430,sizeof(void*),0U));});});
({void*_tmp834=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp834;});
if(topt != 0){
# 2439
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp41E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp41E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2445
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp427)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2450
void*_tmp432=({void*_tmp837=Cyc_Absyn_char_typ;void*_tmp836=rgn;struct Cyc_Absyn_Tqual _tmp835=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp837,_tmp836,_tmp835,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2452
void*_tmp433=Cyc_Tcexp_tcExp(_tmp41E,& _tmp432,e1);
return({void*_tmp83C=_tmp433;void*_tmp83B=rgn;struct Cyc_Absyn_Tqual _tmp83A=Cyc_Absyn_empty_tqual(0U);void*_tmp839=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->tag=1U,({struct Cyc_Absyn_Exp*_tmp838=Cyc_Absyn_uint_exp(1U,0U);_tmp434->f1=_tmp838;});_tmp434;});Cyc_Absyn_atb_typ(_tmp83C,_tmp83B,_tmp83A,_tmp839,Cyc_Absyn_false_conref);});}case 9U: _LL10: _LL11: {
# 2457
void*_tmp435=({void*_tmp83F=Cyc_Absyn_wchar_typ();void*_tmp83E=rgn;struct Cyc_Absyn_Tqual _tmp83D=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp83F,_tmp83E,_tmp83D,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2459
void*_tmp436=Cyc_Tcexp_tcExp(_tmp41E,& _tmp435,e1);
return({void*_tmp844=_tmp436;void*_tmp843=rgn;struct Cyc_Absyn_Tqual _tmp842=Cyc_Absyn_empty_tqual(0U);void*_tmp841=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=1U,({struct Cyc_Absyn_Exp*_tmp840=Cyc_Absyn_uint_exp(1U,0U);_tmp437->f1=_tmp840;});_tmp437;});Cyc_Absyn_atb_typ(_tmp844,_tmp843,_tmp842,_tmp841,Cyc_Absyn_false_conref);});}default: goto _LL12;}default: _LL12: _LL13:
# 2466
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp438=Cyc_Tcutil_compress(*topt);void*_tmp439=_tmp438;void**_tmp43B;struct Cyc_Absyn_Tqual _tmp43A;switch(*((int*)_tmp439)){case 5U: _LL1A: _tmp43B=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp439)->f1).elt_typ;_tmp43A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp439)->f1).elt_tq;_LL1B:
# 2472
 topt2=_tmp43B;goto _LL19;case 3U: _LL1C: _LL1D:
# 2476
 bogus=*topt;
topt2=& bogus;
goto _LL19;default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2482
void*telt=Cyc_Tcexp_tcExp(_tmp41E,topt2,e1);
# 2484
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp43C=0U;({unsigned int _tmp846=e1->loc;struct _dyneither_ptr _tmp845=({const char*_tmp43D="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp43D,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp846,_tmp845,_tag_dyneither(_tmp43C,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));
_tmp43E->tag=5U,(_tmp43E->f1).elt_typ=telt,({struct Cyc_Absyn_Tqual _tmp848=Cyc_Absyn_empty_tqual(0U);(_tmp43E->f1).elt_tq=_tmp848;}),
((_tmp43E->f1).ptr_atts).rgn=rgn,({union Cyc_Absyn_Constraint*_tmp847=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp43E->f1).ptr_atts).nullable=_tmp847;}),((_tmp43E->f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,((_tmp43E->f1).ptr_atts).zero_term=Cyc_Absyn_false_conref,((_tmp43E->f1).ptr_atts).ptrloc=0;_tmp43E;});
# 2491
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp41E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp41E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2498
return res_typ;};};}}_LL5:;};}
# 2504
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp84B=({void*_tmp84A=({void*_tmp849=t;Cyc_Tcutil_promote_array(_tmp849,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp84A;});e->topt=_tmp84B;});
return t;}
# 2514
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2518
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2523
({void*_tmp442=0U;({unsigned int _tmp84D=e->loc;struct _dyneither_ptr _tmp84C=({const char*_tmp443="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp443,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp84D,_tmp84C,_tag_dyneither(_tmp442,sizeof(void*),0U));});});{
# 2525
void*_tmp444=e->r;void*_tmp445=_tmp444;switch(*((int*)_tmp445)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp445)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2532
({void*_tmp84E=Cyc_Tcutil_compress(t);t=_tmp84E;});
if(Cyc_Tcutil_is_array(t)){
({void*_tmp850=({void*_tmp84F=t;Cyc_Tcutil_promote_array(_tmp84F,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp850;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2537
return t;}_LL0:;};}
# 2548 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp446=e->r;void*_tmp447=_tmp446;struct Cyc_Absyn_Exp*_tmp465;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp447)->tag == 12U){_LL1: _tmp465=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp447)->f1;_LL2:
# 2552
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp465);
({void*_tmp851=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp465->topt),0);_tmp465->topt=_tmp851;});
e->topt=_tmp465->topt;
goto _LL0;}else{_LL3: _LL4:
# 2558
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp852=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp852;});
# 2561
{void*_tmp448=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp449=_tmp448;void*_tmp464;struct Cyc_Absyn_Tqual _tmp463;void*_tmp462;union Cyc_Absyn_Constraint*_tmp461;union Cyc_Absyn_Constraint*_tmp460;union Cyc_Absyn_Constraint*_tmp45F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449)->tag == 5U){_LL6: _tmp464=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449)->f1).elt_typ;_tmp463=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449)->f1).elt_tq;_tmp462=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449)->f1).ptr_atts).rgn;_tmp461=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449)->f1).ptr_atts).nullable;_tmp460=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449)->f1).ptr_atts).bounds;_tmp45F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449)->f1).ptr_atts).zero_term;_LL7:
# 2563
{void*_tmp44A=Cyc_Tcutil_compress(_tmp464);void*_tmp44B=_tmp44A;struct Cyc_List_List*_tmp45E;void*_tmp45D;struct Cyc_Absyn_Tqual _tmp45C;void*_tmp45B;struct Cyc_List_List*_tmp45A;int _tmp459;struct Cyc_Absyn_VarargInfo*_tmp458;struct Cyc_List_List*_tmp457;struct Cyc_List_List*_tmp456;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_List_List*_tmp454;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_List_List*_tmp452;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->tag == 9U){_LLB: _tmp45E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).tvars;_tmp45D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).effect;_tmp45C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).ret_tqual;_tmp45B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).ret_typ;_tmp45A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).args;_tmp459=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).c_varargs;_tmp458=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).cyc_varargs;_tmp457=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).rgn_po;_tmp456=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).attributes;_tmp455=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).requires_clause;_tmp454=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).requires_relns;_tmp453=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).ensures_clause;_tmp452=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44B)->f1).ensures_relns;_LLC:
# 2565
 if(_tmp45E != 0){
struct _tuple12 _tmp44C=({struct _tuple12 _tmp56F;({struct Cyc_List_List*_tmp853=Cyc_Tcenv_lookup_type_vars(te);_tmp56F.f1=_tmp853;}),_tmp56F.f2=Cyc_Core_heap_region;_tmp56F;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp44C,_tmp45E);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2573
({struct Cyc_List_List*_tmp854=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp457);_tmp457=_tmp854;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp457);{
void*ftyp=({struct Cyc_List_List*_tmp855=inst;Cyc_Tcutil_substitute(_tmp855,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp451=_cycalloc(sizeof(*_tmp451));
_tmp451->tag=9U,(_tmp451->f1).tvars=0,(_tmp451->f1).effect=_tmp45D,(_tmp451->f1).ret_tqual=_tmp45C,(_tmp451->f1).ret_typ=_tmp45B,(_tmp451->f1).args=_tmp45A,(_tmp451->f1).c_varargs=_tmp459,(_tmp451->f1).cyc_varargs=_tmp458,(_tmp451->f1).rgn_po=0,(_tmp451->f1).attributes=_tmp456,(_tmp451->f1).requires_clause=_tmp455,(_tmp451->f1).requires_relns=_tmp454,(_tmp451->f1).ensures_clause=_tmp453,(_tmp451->f1).ensures_relns=_tmp452;_tmp451;}));});
# 2580
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44D=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->tag=5U,(_tmp450->f1).elt_typ=ftyp,(_tmp450->f1).elt_tq=_tmp463,((_tmp450->f1).ptr_atts).rgn=_tmp462,((_tmp450->f1).ptr_atts).nullable=_tmp461,((_tmp450->f1).ptr_atts).bounds=_tmp460,((_tmp450->f1).ptr_atts).zero_term=_tmp45F,((_tmp450->f1).ptr_atts).ptrloc=0;_tmp450;});
# 2582
struct Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_copy_exp(e);
({void*_tmp856=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->tag=13U,_tmp44F->f1=_tmp44E,_tmp44F->f2=ts;_tmp44F;});e->r=_tmp856;});
e->topt=(void*)_tmp44D;};}
# 2586
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2589
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2592
goto _LL0;}_LL0:;}
# 2594
return(void*)_check_null(e->topt);}
# 2602
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2604
struct Cyc_List_List*_tmp466=0;
# 2606
{void*_tmp467=fn_exp->r;void*_tmp468=_tmp467;struct Cyc_List_List*_tmp47C;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp468)->tag == 10U){_LL1: _tmp47C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp468)->f2;_LL2:
# 2608
{void*_tmp469=e->r;void*_tmp46A=_tmp469;struct Cyc_List_List*_tmp479;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46A)->tag == 10U){_LL6: _tmp479=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46A)->f2;_LL7: {
# 2610
struct Cyc_List_List*_tmp46B=alias_arg_exps;
int _tmp46C=0;
while(_tmp46B != 0){
while(_tmp46C != (int)_tmp46B->hd){
if(_tmp47C == 0)
({void*_tmp46D=0U;({struct _dyneither_ptr _tmp858=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp470=({struct Cyc_Int_pa_PrintArg_struct _tmp571;_tmp571.tag=1U,_tmp571.f1=(unsigned long)_tmp46C;_tmp571;});struct Cyc_Int_pa_PrintArg_struct _tmp471=({struct Cyc_Int_pa_PrintArg_struct _tmp570;_tmp570.tag=1U,_tmp570.f1=(unsigned long)((int)_tmp46B->hd);_tmp570;});void*_tmp46E[2U];_tmp46E[0]=& _tmp470,_tmp46E[1]=& _tmp471;({struct _dyneither_ptr _tmp857=({const char*_tmp46F="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp46F,sizeof(char),36U);});Cyc_aprintf(_tmp857,_tag_dyneither(_tmp46E,sizeof(void*),2U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp858,_tag_dyneither(_tmp46D,sizeof(void*),0U));});});
# 2617
++ _tmp46C;
_tmp47C=_tmp47C->tl;
_tmp479=((struct Cyc_List_List*)_check_null(_tmp479))->tl;}{
# 2622
struct _tuple11 _tmp472=({struct Cyc_Absyn_Exp*_tmp859=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47C))->hd;Cyc_Tcutil_insert_alias(_tmp859,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp479))->hd)->topt)));});struct _tuple11 _tmp473=_tmp472;struct Cyc_Absyn_Decl*_tmp476;struct Cyc_Absyn_Exp*_tmp475;_LLB: _tmp476=_tmp473.f1;_tmp475=_tmp473.f2;_LLC:;
_tmp47C->hd=(void*)_tmp475;
({struct Cyc_List_List*_tmp85A=({struct Cyc_List_List*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->hd=_tmp476,_tmp474->tl=_tmp466;_tmp474;});_tmp466=_tmp85A;});
_tmp46B=_tmp46B->tl;};}
# 2627
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp477=0U;({struct _dyneither_ptr _tmp85B=({const char*_tmp478="not a function call!";_tag_dyneither(_tmp478,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp85B,_tag_dyneither(_tmp477,sizeof(void*),0U));});});}_LL5:;}
# 2630
goto _LL0;}else{_LL3: _LL4:
({void*_tmp47A=0U;({struct _dyneither_ptr _tmp85C=({const char*_tmp47B="not a function call!";_tag_dyneither(_tmp47B,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp85C,_tag_dyneither(_tmp47A,sizeof(void*),0U));});});}_LL0:;}
# 2635
while(_tmp466 != 0){
struct Cyc_Absyn_Decl*_tmp47D=(struct Cyc_Absyn_Decl*)_tmp466->hd;
({struct Cyc_Absyn_Exp*_tmp860=({struct Cyc_Absyn_Stmt*_tmp85F=({struct Cyc_Absyn_Decl*_tmp85E=_tmp47D;struct Cyc_Absyn_Stmt*_tmp85D=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp85E,_tmp85D,e->loc);});Cyc_Absyn_stmt_exp(_tmp85F,e->loc);});fn_exp=_tmp860;});
_tmp466=_tmp466->tl;}
# 2642
e->topt=0;
e->r=fn_exp->r;}
# 2648
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2652
{void*_tmp47E=e->r;void*_tmp47F=_tmp47E;void*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;struct _dyneither_ptr*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E4;int*_tmp4E3;struct Cyc_Absyn_Exp**_tmp4E2;void***_tmp4E1;struct Cyc_Absyn_Exp**_tmp4E0;int*_tmp4DF;void*_tmp4DE;struct Cyc_Absyn_Enumfield*_tmp4DD;struct Cyc_Absyn_Enumdecl*_tmp4DC;struct Cyc_Absyn_Enumfield*_tmp4DB;struct Cyc_List_List*_tmp4DA;struct Cyc_Absyn_Datatypedecl*_tmp4D9;struct Cyc_Absyn_Datatypefield*_tmp4D8;void*_tmp4D7;struct Cyc_List_List*_tmp4D6;struct _tuple1**_tmp4D5;struct Cyc_List_List**_tmp4D4;struct Cyc_List_List*_tmp4D3;struct Cyc_Absyn_Aggrdecl**_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D1;void*_tmp4D0;int*_tmp4CF;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;int*_tmp4CB;struct Cyc_Absyn_Stmt*_tmp4CA;struct Cyc_List_List*_tmp4C9;struct _tuple9*_tmp4C8;struct Cyc_List_List*_tmp4C7;struct Cyc_List_List*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C3;struct _dyneither_ptr*_tmp4C2;int*_tmp4C1;int*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4BF;struct _dyneither_ptr*_tmp4BE;int*_tmp4BD;int*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BB;void*_tmp4BA;struct Cyc_List_List*_tmp4B9;void*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_Absyn_Exp**_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B4;void*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B2;enum Cyc_Absyn_Coercion*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Core_Opt*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;enum Cyc_Absyn_Incrementor _tmp4A0;enum Cyc_Absyn_Primop _tmp49F;struct Cyc_List_List*_tmp49E;void**_tmp49D;union Cyc_Absyn_Cnst*_tmp49C;struct Cyc_Core_Opt*_tmp49B;struct Cyc_List_List*_tmp49A;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_List_List*_tmp498;struct Cyc_Absyn_VarargCallInfo**_tmp497;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_List_List*_tmp495;struct Cyc_Absyn_VarargCallInfo**_tmp494;int*_tmp493;struct Cyc_Absyn_Exp*_tmp492;switch(*((int*)_tmp47F)){case 12U: _LL1: _tmp492=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL2:
# 2657
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp492);
return;case 10U: _LL3: _tmp496=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp495=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp494=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_tmp493=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47F)->f4;if(!(*_tmp493)){_LL4:
# 2661
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp861=({const char*_tmp480="unresolved function in tcExpNoInst";_tag_dyneither(_tmp480,sizeof(char),35U);});_tmp481->f1=_tmp861;});_tmp481;}));}else{_LL1B: _tmp499=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp498=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp497=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_LL1C: {
# 2694
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp482;_push_handler(& _tmp482);{int _tmp484=0;if(setjmp(_tmp482.handler))_tmp484=1;if(!_tmp484){
({struct Cyc_Absyn_Exp*_tmp862=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp862;});;_pop_handler();}else{void*_tmp483=(void*)_exn_thrown;void*_tmp485=_tmp483;void*_tmp486;if(((struct Cyc_Core_Failure_exn_struct*)_tmp485)->tag == Cyc_Core_Failure){_LL56: _LL57:
# 2701
 ok=0;
fn_exp=e;
goto _LL55;}else{_LL58: _tmp486=_tmp485;_LL59:(int)_rethrow(_tmp486);}_LL55:;}};}
# 2705
({void*_tmp863=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp499,_tmp498,_tmp497,& alias_arg_exps);t=_tmp863;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp864=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp864;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2712
goto _LL0;}}case 36U: _LL5: _tmp49B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp49A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL6:
# 2665
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp49A);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp49C=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL8:
# 2670
({void*_tmp865=Cyc_Tcexp_tcConst(te,loc,topt,_tmp49C,e);t=_tmp865;});goto _LL0;case 1U: _LL9: _tmp49D=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LLA:
# 2672
({void*_tmp866=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp49D);t=_tmp866;});goto _LL0;case 2U: _LLB: _LLC:
# 2674
 t=Cyc_Absyn_sint_typ;goto _LL0;case 3U: _LLD: _tmp49F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp49E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LLE:
# 2676
({void*_tmp867=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp49F,_tmp49E);t=_tmp867;});goto _LL0;case 5U: _LLF: _tmp4A1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4A0=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL10:
# 2678
({void*_tmp868=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp4A1,_tmp4A0);t=_tmp868;});goto _LL0;case 4U: _LL11: _tmp4A4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4A3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4A2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_LL12:
# 2680
({void*_tmp869=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4A4,_tmp4A3,_tmp4A2);t=_tmp869;});goto _LL0;case 6U: _LL13: _tmp4A7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4A6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4A5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_LL14:
# 2682
({void*_tmp86A=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp4A7,_tmp4A6,_tmp4A5);t=_tmp86A;});goto _LL0;case 7U: _LL15: _tmp4A9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4A8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL16:
# 2684
({void*_tmp86B=Cyc_Tcexp_tcAnd(te,loc,_tmp4A9,_tmp4A8);t=_tmp86B;});goto _LL0;case 8U: _LL17: _tmp4AB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4AA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL18:
# 2686
({void*_tmp86C=Cyc_Tcexp_tcOr(te,loc,_tmp4AB,_tmp4AA);t=_tmp86C;});goto _LL0;case 9U: _LL19: _tmp4AD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4AC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL1A:
# 2688
({void*_tmp86D=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4AD,_tmp4AC);t=_tmp86D;});goto _LL0;case 11U: _LL1D: _tmp4AE=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL1E:
# 2714
({void*_tmp86E=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4AE);t=_tmp86E;});goto _LL0;case 13U: _LL1F: _tmp4B0=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4AF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL20:
# 2716
({void*_tmp86F=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4B0,_tmp4AF);t=_tmp86F;});goto _LL0;case 14U: _LL21: _tmp4B3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4B2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4B1=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp47F)->f4;_LL22:
# 2718
({void*_tmp870=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4B3,_tmp4B2,_tmp4B1);t=_tmp870;});goto _LL0;case 15U: _LL23: _tmp4B4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL24:
# 2720
({void*_tmp871=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4B4);t=_tmp871;});goto _LL0;case 16U: _LL25: _tmp4B6=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4B5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL26:
# 2722
({void*_tmp872=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4B6,e,_tmp4B5);t=_tmp872;});goto _LL0;case 18U: _LL27: _tmp4B7=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL28: {
# 2724
void*_tmp487=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4B7);
({void*_tmp873=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp487);t=_tmp873;});goto _LL0;}case 17U: _LL29: _tmp4B8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL2A:
# 2727
({void*_tmp874=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4B8);t=_tmp874;});goto _LL0;case 19U: _LL2B: _tmp4BA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4B9=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL2C:
# 2729
({void*_tmp875=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4BA,_tmp4B9);t=_tmp875;});goto _LL0;case 20U: _LL2D: _tmp4BB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL2E:
# 2731
({void*_tmp876=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4BB);t=_tmp876;});goto _LL0;case 21U: _LL2F: _tmp4BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4BE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4BD=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_tmp4BC=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp47F)->f4;_LL30:
# 2733
({void*_tmp877=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4BF,_tmp4BE,_tmp4BD,_tmp4BC);t=_tmp877;});goto _LL0;case 22U: _LL31: _tmp4C3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4C1=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_tmp4C0=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp47F)->f4;_LL32:
# 2735
({void*_tmp878=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4C3,_tmp4C2,_tmp4C1,_tmp4C0);t=_tmp878;});goto _LL0;case 23U: _LL33: _tmp4C5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4C4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL34:
# 2737
({void*_tmp879=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4C5,_tmp4C4);t=_tmp879;});goto _LL0;case 24U: _LL35: _tmp4C6=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL36:
# 2739
({void*_tmp87A=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4C6);t=_tmp87A;});goto _LL0;case 25U: _LL37: _tmp4C8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4C7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL38:
# 2741
({void*_tmp87B=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4C8,_tmp4C7);t=_tmp87B;});goto _LL0;case 26U: _LL39: _tmp4C9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL3A: {
# 2745
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp488=Cyc_Tcutil_compress(*topt);void*_tmp489=_tmp488;void**_tmp48C;struct Cyc_Absyn_Tqual*_tmp48B;union Cyc_Absyn_Constraint*_tmp48A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp489)->tag == 8U){_LL5B: _tmp48C=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp489)->f1).elt_type;_tmp48B=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp489)->f1).tq;_tmp48A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp489)->f1).zero_term;_LL5C:
# 2751
 elt_topt=_tmp48C;
elt_tqopt=_tmp48B;
({int _tmp87C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp48A);zero_term=_tmp87C;});
goto _LL5A;}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}
# 2758
({void*_tmp87D=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4C9);t=_tmp87D;});goto _LL0;}case 37U: _LL3B: _tmp4CA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL3C:
# 2760
({void*_tmp87E=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4CA);t=_tmp87E;});goto _LL0;case 27U: _LL3D: _tmp4CE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4CD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4CC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_tmp4CB=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp47F)->f4;_LL3E:
# 2762
({void*_tmp87F=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4CE,_tmp4CD,_tmp4CC,_tmp4CB);t=_tmp87F;});goto _LL0;case 28U: _LL3F: _tmp4D1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4D0=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4CF=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_LL40:
# 2764
({void*_tmp880=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4D1,_tmp4D0,_tmp4CF);t=_tmp880;});goto _LL0;case 29U: _LL41: _tmp4D5=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4D4=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4D3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_tmp4D2=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp47F)->f4;_LL42:
# 2766
({void*_tmp881=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4D5,_tmp4D4,_tmp4D3,_tmp4D2);t=_tmp881;});goto _LL0;case 30U: _LL43: _tmp4D7=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4D6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL44:
# 2768
({void*_tmp882=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4D7,_tmp4D6);t=_tmp882;});goto _LL0;case 31U: _LL45: _tmp4DA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4D9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_tmp4D8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp47F)->f3;_LL46:
# 2770
({void*_tmp883=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp4DA,_tmp4D9,_tmp4D8);t=_tmp883;});goto _LL0;case 32U: _LL47: _tmp4DC=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4DB=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL48:
# 2772
({void*_tmp884=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->tag=13U,_tmp48D->f1=_tmp4DC->name,_tmp48D->f2=_tmp4DC;_tmp48D;});t=_tmp884;});goto _LL0;case 33U: _LL49: _tmp4DE=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4DD=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL4A:
# 2774
 t=_tmp4DE;goto _LL0;case 34U: _LL4B: _tmp4E3=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp47F)->f1).is_calloc;_tmp4E2=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp47F)->f1).rgn;_tmp4E1=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp47F)->f1).elt_type;_tmp4E0=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp47F)->f1).num_elts;_tmp4DF=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp47F)->f1).fat_result;_LL4C:
# 2776
({void*_tmp885=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4E2,_tmp4E1,_tmp4E0,_tmp4E3,_tmp4DF);t=_tmp885;});goto _LL0;case 35U: _LL4D: _tmp4E5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4E4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL4E:
# 2778
({void*_tmp886=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4E5,_tmp4E4);t=_tmp886;});goto _LL0;case 38U: _LL4F: _tmp4E7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_tmp4E6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp47F)->f2;_LL50:
# 2780
({void*_tmp887=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp4E7,_tmp4E6);t=_tmp887;});goto _LL0;case 39U: _LL51: _tmp4E8=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp47F)->f1;_LL52:
# 2782
 if(!te->allow_valueof)
({void*_tmp48E=0U;({unsigned int _tmp889=e->loc;struct _dyneither_ptr _tmp888=({const char*_tmp48F="valueof(-) can only occur within types";_tag_dyneither(_tmp48F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp889,_tmp888,_tag_dyneither(_tmp48E,sizeof(void*),0U));});});
# 2790
t=Cyc_Absyn_sint_typ;
goto _LL0;default: _LL53: _LL54:
# 2793
({void*_tmp490=0U;({unsigned int _tmp88B=e->loc;struct _dyneither_ptr _tmp88A=({const char*_tmp491="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp491,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp88B,_tmp88A,_tag_dyneither(_tmp490,sizeof(void*),0U));});});
({void*_tmp88C=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp88C;});}_LL0:;}
# 2796
e->topt=t;}
