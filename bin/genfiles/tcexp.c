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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 926 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 932
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 935
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 940
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 953
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 956
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 958
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 960
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 966
void*Cyc_Absyn_exn_typ();
# 975
extern void*Cyc_Absyn_bounds_one;
# 980
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 983
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 986
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 991
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1002
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1077
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1153
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1155
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1158
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1160
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
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*,int do_warn);
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
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
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
# 47 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 51
({void*_tmp0=0U;({unsigned int _tmp577=loc;struct _dyneither_ptr _tmp576=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp577,_tmp576,_tag_dyneither(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 55
return*topt;}}
# 64
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 68
if(topt == 0){
# 70
({void*_tmp578=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=26U,_tmp1->f1=des;_tmp1;});e->r=_tmp578;});
return;}{
# 73
void*t=*topt;
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;void*_tmpE;struct Cyc_Absyn_Tqual _tmpD;union Cyc_Absyn_AggrInfoU _tmpC;switch(*((int*)_tmp3)){case 11U: _LL1: _tmpC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3)->f1).aggr_info;_LL2:
# 76
{union Cyc_Absyn_AggrInfoU _tmp4=_tmpC;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp4.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp579=({const char*_tmp6="struct type not properly set";_tag_dyneither(_tmp6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp579,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp4.KnownAggr).val;_LLD:
({void*_tmp57A=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=29U,_tmp7->f1=_tmp8->name,_tmp7->f2=0,_tmp7->f3=des,_tmp7->f4=_tmp8;_tmp7;});e->r=_tmp57A;});}_LL9:;}
# 80
goto _LL0;case 8U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4:
({void*_tmp57B=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=26U,_tmp9->f1=des;_tmp9;});e->r=_tmp57B;});goto _LL0;case 12U: _LL5: _LL6:
({void*_tmp57C=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=30U,_tmpA->f1=t,_tmpA->f2=des;_tmpA;});e->r=_tmp57C;});goto _LL0;default: _LL7: _LL8:
({void*_tmp57D=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp57D;});goto _LL0;}_LL0:;};}
# 90
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 94
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 100
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_tmp10=_tmpF;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->f2 == 0){_LL1: _LL2:
# 103
 if(Cyc_Tc_aggressive_warn)({void*_tmp11=0U;({unsigned int _tmp57F=e->loc;struct _dyneither_ptr _tmp57E=({const char*_tmp12="assignment in test";_tag_dyneither(_tmp12,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp57F,_tmp57E,_tag_dyneither(_tmp11,sizeof(void*),0U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 110
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,_tmp4EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);_tmp4EE;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=0U,({
struct _dyneither_ptr _tmp580=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4ED.f1=_tmp580;});_tmp4ED;});void*_tmp13[2U];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16;({unsigned int _tmp582=e->loc;struct _dyneither_ptr _tmp581=({const char*_tmp14="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp14,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp582,_tmp581,_tag_dyneither(_tmp13,sizeof(void*),2U));});});
# 119
{void*_tmp17=e->r;void*_tmp18=_tmp17;enum Cyc_Absyn_Primop _tmp1F;struct Cyc_List_List*_tmp1E;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->tag == 3U){_LL1: _tmp1F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f1;_tmp1E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f2;_LL2:
# 121
 if(_tmp1F == Cyc_Absyn_Eq  || _tmp1F == Cyc_Absyn_Neq){
struct _tuple0 _tmp19=({struct _tuple0 _tmp4F0;_tmp4F0.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E))->hd)->topt),_tmp4F0.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E->tl))->hd)->topt);_tmp4F0;});struct _tuple0 _tmp1A=_tmp19;void*_tmp1D;void*_tmp1C;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->tag == 15U){_LL6: _tmp1D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->f1;_tmp1C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->f1;_LL7:
# 124
 return({struct Cyc_Tcexp_TestEnv _tmp4EF;({struct _tuple0*_tmp583=({struct _tuple0*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->f1=_tmp1D,_tmp1B->f2=_tmp1C;_tmp1B;});_tmp4EF.eq=_tmp583;}),_tmp4EF.isTrue=_tmp1F == Cyc_Absyn_Eq;_tmp4EF;});}else{goto _LL8;}}else{_LL8: _LL9:
# 126
 goto _LL5;}_LL5:;}
# 129
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 132
return({struct Cyc_Tcexp_TestEnv _tmp4F1;_tmp4F1.eq=0,_tmp4F1.isTrue=0;_tmp4F1;});}
# 153 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 158
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp20=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp21=_tmp20;struct _dyneither_ptr _tmp56;struct _dyneither_ptr _tmp55;enum Cyc_Absyn_Sign _tmp54;int _tmp53;int _tmp52;enum Cyc_Absyn_Sign _tmp51;enum Cyc_Absyn_Sign _tmp50;switch((_tmp21.Wstring_c).tag){case 2U: switch(((_tmp21.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_typ;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_typ;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_typ;goto _LL0;}case 3U: _LL7: _LL8:
({void*_tmp584=Cyc_Absyn_wchar_typ();t=_tmp584;});goto _LL0;case 4U: _LL9: _tmp50=((_tmp21.Short_c).val).f1;_LLA:
# 168
 t=_tmp50 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL0;case 6U: _LLB: _tmp51=((_tmp21.LongLong_c).val).f1;_LLC:
# 170
 t=_tmp51 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL0;case 7U: _LLD: _tmp52=((_tmp21.Float_c).val).f2;_LLE:
# 172
 if(topt == 0)({void*_tmp585=Cyc_Absyn_float_typ(_tmp52);t=_tmp585;});else{
# 176
void*_tmp22=Cyc_Tcutil_compress(*topt);void*_tmp23=_tmp22;int _tmp24;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp23)->tag == 7U)switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp23)->f1){case 0U: _LL18: _LL19:
({void*_tmp586=Cyc_Absyn_float_typ(0);t=_tmp586;});goto _LL17;case 1U: _LL1A: _LL1B:
({void*_tmp587=Cyc_Absyn_float_typ(1);t=_tmp587;});goto _LL17;default: _LL1C: _tmp24=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp23)->f1;_LL1D:
({void*_tmp588=Cyc_Absyn_float_typ(_tmp24);t=_tmp588;});goto _LL17;}else{_LL1E: _LL1F:
# 182
({void*_tmp589=Cyc_Absyn_float_typ(_tmp52);t=_tmp589;});goto _LL17;}_LL17:;}
# 185
goto _LL0;case 5U: _LLF: _tmp54=((_tmp21.Int_c).val).f1;_tmp53=((_tmp21.Int_c).val).f2;_LL10:
# 187
 if(topt == 0)
t=_tmp54 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 194
void*_tmp25=Cyc_Tcutil_compress(*topt);void*_tmp26=_tmp25;void*_tmp3A;void*_tmp39;struct Cyc_Absyn_Tqual _tmp38;void*_tmp37;union Cyc_Absyn_Constraint*_tmp36;union Cyc_Absyn_Constraint*_tmp35;union Cyc_Absyn_Constraint*_tmp34;enum Cyc_Absyn_Sign _tmp33;enum Cyc_Absyn_Sign _tmp32;enum Cyc_Absyn_Sign _tmp31;enum Cyc_Absyn_Sign _tmp30;switch(*((int*)_tmp26)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f2){case Cyc_Absyn_Char_sz: _LL21: _tmp30=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f1;_LL22:
# 196
{enum Cyc_Absyn_Sign _tmp27=_tmp30;switch(_tmp27){case Cyc_Absyn_Unsigned: _LL30: _LL31:
 t=Cyc_Absyn_uchar_typ;goto _LL2F;case Cyc_Absyn_Signed: _LL32: _LL33:
 t=Cyc_Absyn_schar_typ;goto _LL2F;default: _LL34: _LL35:
 t=Cyc_Absyn_char_typ;goto _LL2F;}_LL2F:;}
# 201
({union Cyc_Absyn_Cnst _tmp58A=Cyc_Absyn_Char_c(_tmp30,(char)_tmp53);*c=_tmp58A;});
goto _LL20;case Cyc_Absyn_Short_sz: _LL23: _tmp31=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f1;_LL24:
# 204
 t=_tmp31 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp58B=Cyc_Absyn_Short_c(_tmp31,(short)_tmp53);*c=_tmp58B;});
goto _LL20;case Cyc_Absyn_Int_sz: _LL25: _tmp32=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f1;_LL26:
# 208
 t=_tmp32 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 211
({union Cyc_Absyn_Cnst _tmp58C=Cyc_Absyn_Int_c(_tmp32,_tmp53);*c=_tmp58C;});
goto _LL20;case Cyc_Absyn_Long_sz: _LL27: _tmp33=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f1;_LL28:
# 214
 t=_tmp33 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 217
({union Cyc_Absyn_Cnst _tmp58D=Cyc_Absyn_Int_c(_tmp33,_tmp53);*c=_tmp58D;});
goto _LL20;default: goto _LL2D;}case 5U: _LL29: _tmp39=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).elt_typ;_tmp38=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).elt_tq;_tmp37=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).rgn;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).nullable;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).bounds;_tmp34=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).zero_term;if(_tmp53 == 0){_LL2A: {
# 221
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp36))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0U,({
struct _dyneither_ptr _tmp58E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4F2.f1=_tmp58E;});_tmp4F2;});void*_tmp28[1U];_tmp28[0]=& _tmp2A;({unsigned int _tmp590=e->loc;struct _dyneither_ptr _tmp58F=({const char*_tmp29="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp29,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp590,_tmp58F,_tag_dyneither(_tmp28,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp2B=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp591=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=5U,(_tmp2C->f1).elt_typ=_tmp39,(_tmp2C->f1).elt_tq=_tmp38,
((_tmp2C->f1).ptr_atts).rgn=_tmp37,((_tmp2C->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp2C->f1).ptr_atts).bounds=_tmp35,((_tmp2C->f1).ptr_atts).zero_term=_tmp34,((_tmp2C->f1).ptr_atts).ptrloc=0;_tmp2C;});
# 227
t=_tmp591;});
# 229
goto _LL20;};}}else{goto _LL2D;}case 19U: _LL2B: _tmp3A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp26)->f1;_LL2C: {
# 231
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=18U,({struct Cyc_Absyn_Exp*_tmp592=Cyc_Absyn_uint_exp((unsigned int)_tmp53,0U);_tmp2F->f1=_tmp592;});_tmp2F;});
# 238
({void*_tmp593=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=19U,_tmp2E->f1=(void*)_tmp2D;_tmp2E;});t=_tmp593;});
goto _LL20;}default: _LL2D: _LL2E:
# 241
 t=_tmp54 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL20;}_LL20:;}
# 244
goto _LL0;case 8U: _LL11: _tmp55=(_tmp21.String_c).val;_LL12:
# 246
 string_numelts=(int)_get_dyneither_size(_tmp55,sizeof(char));
_tmp56=_tmp55;goto _LL14;case 9U: _LL13: _tmp56=(_tmp21.Wstring_c).val;_LL14:
# 249
 if(string_numelts == 0){
({int _tmp594=Cyc_Tcexp_wchar_numelts(_tmp56);string_numelts=_tmp594;});
({void*_tmp595=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp595;});}{
# 253
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp596=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp596,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 258
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3B=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=1U,_tmp3F->f1=elen;_tmp3F;});
({void*_tmp59A=({void*_tmp599=string_elt_typ;struct Cyc_Absyn_Tqual _tmp598=Cyc_Absyn_const_tqual(0U);void*_tmp597=(void*)_tmp3B;Cyc_Absyn_atb_typ(_tmp599,(void*)& Cyc_Absyn_HeapRgn_val,_tmp598,_tmp597,Cyc_Absyn_true_conref);});t=_tmp59A;});
# 261
if(topt != 0){
void*_tmp3C=Cyc_Tcutil_compress(*topt);void*_tmp3D=_tmp3C;struct Cyc_Absyn_Tqual _tmp3E;switch(*((int*)_tmp3D)){case 8U: _LL37: _tmp3E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).tq;_LL38:
# 266
 return({void*_tmp59D=string_elt_typ;struct Cyc_Absyn_Tqual _tmp59C=_tmp3E;struct Cyc_Absyn_Exp*_tmp59B=elen;Cyc_Absyn_array_typ(_tmp59D,_tmp59C,_tmp59B,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0U);});case 5U: _LL39: _LL3A:
# 268
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 274
({void*_tmp5A2=({void*_tmp5A1=string_elt_typ;void*_tmp5A0=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp59F=
Cyc_Absyn_const_tqual(0U);
# 274
void*_tmp59E=(void*)_tmp3B;Cyc_Absyn_atb_typ(_tmp5A1,_tmp5A0,_tmp59F,_tmp59E,Cyc_Absyn_true_conref);});t=_tmp5A2;});
# 276
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 282
goto _LL36;default: _LL3B: _LL3C:
 goto _LL36;}_LL36:;}
# 286
return t;};};default: _LL15: _LL16:
# 288
 if(topt != 0){
void*_tmp40=Cyc_Tcutil_compress(*topt);void*_tmp41=_tmp40;void*_tmp4B;struct Cyc_Absyn_Tqual _tmp4A;void*_tmp49;union Cyc_Absyn_Constraint*_tmp48;union Cyc_Absyn_Constraint*_tmp47;union Cyc_Absyn_Constraint*_tmp46;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->tag == 5U){_LL3E: _tmp4B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->f1).elt_typ;_tmp4A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->f1).elt_tq;_tmp49=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->f1).ptr_atts).rgn;_tmp48=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->f1).ptr_atts).nullable;_tmp47=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->f1).ptr_atts).bounds;_tmp46=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41)->f1).ptr_atts).zero_term;_LL3F:
# 292
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp48))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp44=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({
struct _dyneither_ptr _tmp5A3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4F3.f1=_tmp5A3;});_tmp4F3;});void*_tmp42[1U];_tmp42[0]=& _tmp44;({unsigned int _tmp5A5=e->loc;struct _dyneither_ptr _tmp5A4=({const char*_tmp43="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp43,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp5A5,_tmp5A4,_tag_dyneither(_tmp42,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->tag=5U,(_tmp45->f1).elt_typ=_tmp4B,(_tmp45->f1).elt_tq=_tmp4A,((_tmp45->f1).ptr_atts).rgn=_tmp49,((_tmp45->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp45->f1).ptr_atts).bounds=_tmp47,((_tmp45->f1).ptr_atts).zero_term=_tmp46,((_tmp45->f1).ptr_atts).ptrloc=0;_tmp45;});}else{_LL40: _LL41:
# 297
 goto _LL3D;}_LL3D:;}{
# 300
struct Cyc_List_List*_tmp4C=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp5AB=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=5U,({void*_tmp5AA=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->v=_tmp4C;_tmp4D;}));(_tmp4F->f1).elt_typ=_tmp5AA;}),({
struct Cyc_Absyn_Tqual _tmp5A9=Cyc_Absyn_empty_tqual(0U);(_tmp4F->f1).elt_tq=_tmp5A9;}),
({void*_tmp5A8=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->v=_tmp4C;_tmp4E;}));((_tmp4F->f1).ptr_atts).rgn=_tmp5A8;}),((_tmp4F->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 305
union Cyc_Absyn_Constraint*_tmp5A7=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4F->f1).ptr_atts).bounds=_tmp5A7;}),({union Cyc_Absyn_Constraint*_tmp5A6=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4F->f1).ptr_atts).zero_term=_tmp5A6;}),((_tmp4F->f1).ptr_atts).ptrloc=0;_tmp4F;});
# 301
t=_tmp5AB;});
# 307
goto _LL0;};}_LL0:;}
# 309
return t;}
# 313
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 318
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp57=*((void**)_check_null(b));void*_tmp58=_tmp57;struct Cyc_Absyn_Vardecl*_tmp64;struct Cyc_Absyn_Vardecl*_tmp63;struct Cyc_Absyn_Vardecl*_tmp62;struct Cyc_Absyn_Fndecl*_tmp61;struct Cyc_Absyn_Vardecl*_tmp60;struct _tuple1*_tmp5F;switch(*((int*)_tmp58)){case 0U: _LL1: _tmp5F=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp58)->f1;_LL2:
# 321
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp5AC=({const char*_tmp59="unresolved binding in tcVar";_tag_dyneither(_tmp59,sizeof(char),28U);});_tmp5A->f1=_tmp5AC;});_tmp5A;}));case 1U: _LL3: _tmp60=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp58)->f1;_LL4:
# 325
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp60->name,1);
return _tmp60->type;case 2U: _LL5: _tmp61=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp58)->f1;_LL6:
# 332
 if(_tmp61->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp61->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp61);case 5U: _LL7: _tmp62=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp58)->f1;_LL8:
 _tmp63=_tmp62;goto _LLA;case 4U: _LL9: _tmp63=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp58)->f1;_LLA:
 _tmp64=_tmp63;goto _LLC;default: _LLB: _tmp64=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp58)->f1;_LLC:
# 338
 if(te->allow_valueof){
void*_tmp5B=Cyc_Tcutil_compress(_tmp64->type);void*_tmp5C=_tmp5B;void*_tmp5E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C)->tag == 19U){_LLE: _tmp5E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C)->f1;_LLF:
# 341
({void*_tmp5AD=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=39U,_tmp5D->f1=_tmp5E;_tmp5D;});e->r=_tmp5AD;});
goto _LLD;}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 346
return _tmp64->type;}_LL0:;}
# 350
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 357
struct Cyc_List_List*desc_types;
{void*_tmp65=fmt->r;void*_tmp66=_tmp65;struct _dyneither_ptr _tmp6B;struct _dyneither_ptr _tmp6A;switch(*((int*)_tmp66)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66)->f1).String_c).tag == 8){_LL1: _tmp6A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66)->f1).String_c).val;_LL2:
 _tmp6B=_tmp6A;goto _LL4;}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp6B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66)->f2)->r)->f1).String_c).val;_LL4:
# 361
({struct Cyc_List_List*_tmp5AE=type_getter(te,(struct _dyneither_ptr)_tmp6B,fmt->loc);desc_types=_tmp5AE;});goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 365
 if(opt_args != 0){
struct Cyc_List_List*_tmp67=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp67 != 0;_tmp67=_tmp67->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp67->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp67->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp67->hd))
({void*_tmp68=0U;({unsigned int _tmp5B0=((struct Cyc_Absyn_Exp*)_tmp67->hd)->loc;struct _dyneither_ptr _tmp5AF=({const char*_tmp69="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp69,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp5B0,_tmp5AF,_tag_dyneither(_tmp68,sizeof(void*),0U));});});}}
# 374
return;}_LL0:;}
# 376
if(opt_args != 0){
struct Cyc_List_List*_tmp6C=(struct Cyc_List_List*)opt_args->v;
# 379
for(0;desc_types != 0  && _tmp6C != 0;(
desc_types=desc_types->tl,_tmp6C=_tmp6C->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp6C->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp4F5;_tmp4F5.tag=0U,({
struct _dyneither_ptr _tmp5B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F5.f1=_tmp5B1;});_tmp4F5;});struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U,({struct _dyneither_ptr _tmp5B2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4F4.f1=_tmp5B2;});_tmp4F4;});void*_tmp6D[2U];_tmp6D[0]=& _tmp6F,_tmp6D[1]=& _tmp70;({unsigned int _tmp5B4=e->loc;struct _dyneither_ptr _tmp5B3=({const char*_tmp6E="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp6E,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp5B4,_tmp5B3,_tag_dyneither(_tmp6D,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 390
if(alias_coercion)
({struct Cyc_List_List*_tmp5B5=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=(void*)arg_cnt,_tmp71->tl=*alias_arg_exps;_tmp71;});*alias_arg_exps=_tmp5B5;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp72=0U;({unsigned int _tmp5B7=((struct Cyc_Absyn_Exp*)_tmp6C->hd)->loc;struct _dyneither_ptr _tmp5B6=({const char*_tmp73="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp73,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp5B7,_tmp5B6,_tag_dyneither(_tmp72,sizeof(void*),0U));});});}
# 397
if(desc_types != 0)
({void*_tmp74=0U;({unsigned int _tmp5B9=fmt->loc;struct _dyneither_ptr _tmp5B8=({const char*_tmp75="too few arguments";_tag_dyneither(_tmp75,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp5B9,_tmp5B8,_tag_dyneither(_tmp74,sizeof(void*),0U));});});
if(_tmp6C != 0){
({void*_tmp76=0U;({unsigned int _tmp5BB=((struct Cyc_Absyn_Exp*)_tmp6C->hd)->loc;struct _dyneither_ptr _tmp5BA=({const char*_tmp77="too many arguments";_tag_dyneither(_tmp77,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp5BB,_tmp5BA,_tag_dyneither(_tmp76,sizeof(void*),0U));});});
# 402
for(0;_tmp6C != 0;_tmp6C=_tmp6C->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp6C->hd);}}}}
# 407
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 409
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp78=p;switch(_tmp78){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 413
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0U,({struct _dyneither_ptr _tmp5BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F6.f1=_tmp5BC;});_tmp4F6;});void*_tmp79[1U];_tmp79[0]=& _tmp7B;({unsigned int _tmp5BE=loc;struct _dyneither_ptr _tmp5BD=({const char*_tmp7A="expecting numeric type but found %s";_tag_dyneither(_tmp7A,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5BE,_tmp5BD,_tag_dyneither(_tmp79,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 417
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U,({struct _dyneither_ptr _tmp5BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F7.f1=_tmp5BF;});_tmp4F7;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp5C1=loc;struct _dyneither_ptr _tmp5C0=({const char*_tmp7D="expecting integral or * type but found %s";_tag_dyneither(_tmp7D,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp5C1,_tmp5C0,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 422
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp81=({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U,({struct _dyneither_ptr _tmp5C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F8.f1=_tmp5C2;});_tmp4F8;});void*_tmp7F[1U];_tmp7F[0]=& _tmp81;({unsigned int _tmp5C4=loc;struct _dyneither_ptr _tmp5C3=({const char*_tmp80="expecting integral type but found %s";_tag_dyneither(_tmp80,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp5C4,_tmp5C3,_tag_dyneither(_tmp7F,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 426
{void*_tmp82=t;union Cyc_Absyn_Constraint*_tmp8B;switch(*((int*)_tmp82)){case 8U: _LLE: _LLF:
 goto _LLD;case 5U: _LL10: _tmp8B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82)->f1).ptr_atts).bounds;_LL11:
# 429
{void*_tmp83=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8B);void*_tmp84=_tmp83;struct Cyc_Absyn_Exp*_tmp87;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp84)->tag == 0U){_LL15: _LL16:
 goto _LL14;}else{_LL17: _tmp87=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp84)->f1;_LL18:
# 432
 if(!Cyc_Evexp_c_can_eval(_tmp87) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp85=0U;({unsigned int _tmp5C6=loc;struct _dyneither_ptr _tmp5C5=({const char*_tmp86="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp86,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp5C6,_tmp5C5,_tag_dyneither(_tmp85,sizeof(void*),0U));});});
goto _LL14;}_LL14:;}
# 436
goto _LLD;default: _LL12: _LL13:
# 438
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,({struct _dyneither_ptr _tmp5C7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F9.f1=_tmp5C7;});_tmp4F9;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({unsigned int _tmp5C9=loc;struct _dyneither_ptr _tmp5C8=({const char*_tmp89="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp89,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp5C9,_tmp5C8,_tag_dyneither(_tmp88,sizeof(void*),1U));});});}_LLD:;}
# 440
return Cyc_Absyn_uint_typ;default: _LLB: _LLC:
({void*_tmp8C=0U;({struct _dyneither_ptr _tmp5CA=({const char*_tmp8D="Non-unary primop";_tag_dyneither(_tmp8D,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5CA,_tag_dyneither(_tmp8C,sizeof(void*),0U));});});}_LL0:;}
# 446
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 449
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,({struct _dyneither_ptr _tmp5CB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4FA.f1=_tmp5CB;});_tmp4FA;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({unsigned int _tmp5CD=e1->loc;struct _dyneither_ptr _tmp5CC=({const char*_tmp8F="type %s cannot be used here";_tag_dyneither(_tmp8F,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5CD,_tmp5CC,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 453
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,({struct _dyneither_ptr _tmp5CE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4FB.f1=_tmp5CE;});_tmp4FB;});void*_tmp91[1U];_tmp91[0]=& _tmp93;({unsigned int _tmp5D0=e2->loc;struct _dyneither_ptr _tmp5CF=({const char*_tmp92="type %s cannot be used here";_tag_dyneither(_tmp92,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5D0,_tmp5CF,_tag_dyneither(_tmp91,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 457
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 462
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp94=t1;void*_tmpAC;struct Cyc_Absyn_Tqual _tmpAB;void*_tmpAA;union Cyc_Absyn_Constraint*_tmpA9;union Cyc_Absyn_Constraint*_tmpA8;union Cyc_Absyn_Constraint*_tmpA7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->tag == 5U){_LL1: _tmpAC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_typ;_tmpAB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_tq;_tmpAA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).rgn;_tmpA9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).nullable;_tmpA8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).bounds;_tmpA7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).zero_term;_LL2:
# 467
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpAC),& Cyc_Tcutil_tmk))
({void*_tmp95=0U;({unsigned int _tmp5D2=e1->loc;struct _dyneither_ptr _tmp5D1=({const char*_tmp96="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp96,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5D2,_tmp5D1,_tag_dyneither(_tmp95,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp97=0U;({unsigned int _tmp5D4=e1->loc;struct _dyneither_ptr _tmp5D3=({const char*_tmp98="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp98,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5D4,_tmp5D3,_tag_dyneither(_tmp97,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmp9B=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,({struct _dyneither_ptr _tmp5D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4FC.f1=_tmp5D5;});_tmp4FC;});void*_tmp99[1U];_tmp99[0]=& _tmp9B;({unsigned int _tmp5D7=e2->loc;struct _dyneither_ptr _tmp5D6=({const char*_tmp9A="expecting int but found %s";_tag_dyneither(_tmp9A,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5D7,_tmp5D6,_tag_dyneither(_tmp99,sizeof(void*),1U));});});{
void*_tmp9C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA8);void*_tmp9D=_tmp9C;struct Cyc_Absyn_Exp*_tmpA6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9D)->tag == 0U){_LL6: _LL7:
 return t1;}else{_LL8: _tmpA6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9D)->f1;_LL9:
# 478
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA7)){
struct _tuple15 _tmp9E=Cyc_Evexp_eval_const_uint_exp(_tmpA6);struct _tuple15 _tmp9F=_tmp9E;unsigned int _tmpA3;int _tmpA2;_LLB: _tmpA3=_tmp9F.f1;_tmpA2=_tmp9F.f2;_LLC:;
if(_tmpA2  && _tmpA3 == 1)
({void*_tmpA0=0U;({unsigned int _tmp5D9=e1->loc;struct _dyneither_ptr _tmp5D8=({const char*_tmpA1="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpA1,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5D9,_tmp5D8,_tag_dyneither(_tmpA0,sizeof(void*),0U));});});}{
# 489
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA4=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=5U,(_tmpA5->f1).elt_typ=_tmpAC,(_tmpA5->f1).elt_tq=_tmpAB,
((_tmpA5->f1).ptr_atts).rgn=_tmpAA,((_tmpA5->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmpA5->f1).ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmpA5->f1).ptr_atts).zero_term=_tmpA7,((_tmpA5->f1).ptr_atts).ptrloc=0;_tmpA5;});
# 493
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpA4,Cyc_Absyn_Other_coercion);
return(void*)_tmpA4;};}_LL5:;};}else{_LL3: _LL4:
# 496
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 501
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,({
# 511
struct _dyneither_ptr _tmp5DA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FE.f1=_tmp5DA;});_tmp4FE;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,({struct _dyneither_ptr _tmp5DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4FD.f1=_tmp5DB;});_tmp4FD;});void*_tmpAD[2U];_tmpAD[0]=& _tmpAF,_tmpAD[1]=& _tmpB0;({unsigned int _tmp5DD=e1->loc;struct _dyneither_ptr _tmp5DC=({const char*_tmpAE="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpAE,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5DD,_tmp5DC,_tag_dyneither(_tmpAD,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 514
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp5DE=t1_elt;Cyc_Tcutil_unify(_tmp5DE,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpB3=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,({
# 519
struct _dyneither_ptr _tmp5DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp500.f1=_tmp5DF;});_tmp500;});struct Cyc_String_pa_PrintArg_struct _tmpB4=({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0U,({struct _dyneither_ptr _tmp5E0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4FF.f1=_tmp5E0;});_tmp4FF;});void*_tmpB1[2U];_tmpB1[0]=& _tmpB3,_tmpB1[1]=& _tmpB4;({unsigned int _tmp5E2=e1->loc;struct _dyneither_ptr _tmp5E1=({const char*_tmpB2="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpB2,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5E2,_tmp5E1,_tag_dyneither(_tmpB1,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 523
({void*_tmpB5=0U;({unsigned int _tmp5E4=e1->loc;struct _dyneither_ptr _tmp5E3=({const char*_tmpB6="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpB6,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5E4,_tmp5E3,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5E9=te;struct Cyc_Absyn_Exp*_tmp5E8=e1;Cyc_Tcutil_unchecked_cast(_tmp5E9,_tmp5E8,({void*_tmp5E7=t1_elt;void*_tmp5E6=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5E5=
Cyc_Absyn_empty_tqual(0U);
# 524
Cyc_Absyn_star_typ(_tmp5E7,_tmp5E6,_tmp5E5,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 527
return Cyc_Absyn_sint_typ;}else{
# 529
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpB7=0U;({unsigned int _tmp5EB=e1->loc;struct _dyneither_ptr _tmp5EA=({const char*_tmpB8="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpB8,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5EB,_tmp5EA,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpB9=0U;({unsigned int _tmp5ED=e1->loc;struct _dyneither_ptr _tmp5EC=({const char*_tmpBA="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpBA,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5ED,_tmp5EC,_tag_dyneither(_tmpB9,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpBD=({struct Cyc_String_pa_PrintArg_struct _tmp502;_tmp502.tag=0U,({
struct _dyneither_ptr _tmp5EE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp502.f1=_tmp5EE;});_tmp502;});struct Cyc_String_pa_PrintArg_struct _tmpBE=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,({struct _dyneither_ptr _tmp5EF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp501.f1=_tmp5EF;});_tmp501;});void*_tmpBB[2U];_tmpBB[0]=& _tmpBD,_tmpBB[1]=& _tmpBE;({unsigned int _tmp5F1=e2->loc;struct _dyneither_ptr _tmp5F0=({const char*_tmpBC="expecting either %s or int but found %s";_tag_dyneither(_tmpBC,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5F1,_tmp5F0,_tag_dyneither(_tmpBB,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 538
return t1;}}}
# 542
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5F2=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5F2,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpBF=0U;({unsigned int _tmp5F4=e1->loc;struct _dyneither_ptr _tmp5F3=({const char*_tmpC0="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC0,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5F4,_tmp5F3,_tag_dyneither(_tmpBF,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5F9=te;struct Cyc_Absyn_Exp*_tmp5F8=e2;Cyc_Tcutil_unchecked_cast(_tmp5F9,_tmp5F8,({void*_tmp5F7=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5F6=
Cyc_Tcutil_pointer_region(t2);
# 547
struct Cyc_Absyn_Tqual _tmp5F5=
# 549
Cyc_Absyn_empty_tqual(0U);
# 547
Cyc_Absyn_star_typ(_tmp5F7,_tmp5F6,_tmp5F5,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 552
({void*_tmpC1=0U;({unsigned int _tmp5FB=e1->loc;struct _dyneither_ptr _tmp5FA=({const char*_tmpC2="thin pointer subtraction!";_tag_dyneither(_tmpC2,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5FB,_tmp5FA,_tag_dyneither(_tmpC1,sizeof(void*),0U));});});
return Cyc_Absyn_sint_typ;}
# 555
({void*_tmpC3=0U;({unsigned int _tmp5FD=e1->loc;struct _dyneither_ptr _tmp5FC=({const char*_tmpC4="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpC4,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5FD,_tmp5FC,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 558
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpC5=0U;({unsigned int _tmp5FF=e1->loc;struct _dyneither_ptr _tmp5FE=({const char*_tmpC6="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpC6,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp5FF,_tmp5FE,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 563
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 566
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 574
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || ({
void*_tmp600=t1;Cyc_Tcutil_unify(_tmp600,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 579
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 592
pointer_cmp: {
struct _tuple0 _tmpC7=({struct _tuple0 _tmp503;({void*_tmp602=Cyc_Tcutil_compress(t1);_tmp503.f1=_tmp602;}),({void*_tmp601=Cyc_Tcutil_compress(t2);_tmp503.f2=_tmp601;});_tmp503;});struct _tuple0 _tmpC8=_tmpC7;void*_tmpCA;void*_tmpC9;switch(*((int*)_tmpC8.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8.f2)->tag == 5U){_LL1: _tmpCA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8.f1)->f1).elt_typ;_tmpC9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8.f2)->f1).elt_typ;_LL2:
# 596
 if(Cyc_Tcutil_unify(_tmpCA,_tmpC9))return Cyc_Absyn_sint_typ;goto _LL0;}else{goto _LL5;}case 15U: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC8.f2)->tag == 15U){_LL3: _LL4:
# 598
 return Cyc_Absyn_sint_typ;}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 601
({struct Cyc_String_pa_PrintArg_struct _tmpCD=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({
struct _dyneither_ptr _tmp603=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp505.f1=_tmp603;});_tmp505;});struct Cyc_String_pa_PrintArg_struct _tmpCE=({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U,({struct _dyneither_ptr _tmp604=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp504.f1=_tmp604;});_tmp504;});void*_tmpCB[2U];_tmpCB[0]=& _tmpCD,_tmpCB[1]=& _tmpCE;({unsigned int _tmp606=loc;struct _dyneither_ptr _tmp605=({const char*_tmpCC="comparison not allowed between %s and %s";_tag_dyneither(_tmpCC,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp606,_tmp605,_tag_dyneither(_tmpCB,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 608
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 610
if(({void*_tmp607=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp607 == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 616
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpCF=({struct _tuple0 _tmp506;_tmp506.f1=t1,_tmp506.f2=t2;_tmp506;});struct _tuple0 _tmpD0=_tmpCF;void*_tmpD2;void*_tmpD1;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f2)->tag == 15U){_LL1: _tmpD2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f1)->f1;_tmpD1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f2)->f1;_LL2:
# 621
 return Cyc_Absyn_sint_typ;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 624
({struct Cyc_String_pa_PrintArg_struct _tmpD5=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({
struct _dyneither_ptr _tmp608=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp508.f1=_tmp608;});_tmp508;});struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0U,({struct _dyneither_ptr _tmp609=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp507.f1=_tmp609;});_tmp507;});void*_tmpD3[2U];_tmpD3[0]=& _tmpD5,_tmpD3[1]=& _tmpD6;({unsigned int _tmp60B=loc;struct _dyneither_ptr _tmp60A=({const char*_tmpD4="comparison not allowed between %s and %s";_tag_dyneither(_tmpD4,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp60B,_tmp60A,_tag_dyneither(_tmpD3,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 632
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 634
enum Cyc_Absyn_Primop _tmpD7=p;switch(_tmpD7){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 638
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 641
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Bitarshift: _LL15: _LL16:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL17: _LL18:
# 651
 goto _LL1A;case Cyc_Absyn_Neq: _LL19: _LL1A:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LL1B: _LL1C:
# 654
 goto _LL1E;case Cyc_Absyn_Lt: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Gte: _LL1F: _LL20:
 goto _LL22;case Cyc_Absyn_Lte: _LL21: _LL22:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LL23: _LL24:
# 659
({void*_tmpD8=0U;({struct _dyneither_ptr _tmp60C=({const char*_tmpD9="bad binary primop";_tag_dyneither(_tmpD9,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp60C,_tag_dyneither(_tmpD8,sizeof(void*),0U));});});}_LL0:;}
# 663
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 671
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpDA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpDB=Cyc_Tcexp_tcExp(te,topt,_tmpDA);
if(!Cyc_Tcutil_is_numeric(_tmpDA))
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({struct _dyneither_ptr _tmp60D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpDB));_tmp509.f1=_tmp60D;});_tmp509;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({unsigned int _tmp60F=_tmpDA->loc;struct _dyneither_ptr _tmp60E=({const char*_tmpDD="expecting numeric type but found %s";_tag_dyneither(_tmpDD,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp60F,_tmp60E,_tag_dyneither(_tmpDC,sizeof(void*),1U));});});
return _tmpDB;}
# 678
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpDF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpE0=_tmpDF;switch(_tmpE0){case 0U: _LL1: _LL2:
 return({void*_tmpE1=0U;({struct Cyc_Tcenv_Tenv*_tmp613=te;unsigned int _tmp612=loc;void**_tmp611=topt;struct _dyneither_ptr _tmp610=({const char*_tmpE2="primitive operator has 0 arguments";_tag_dyneither(_tmpE2,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp613,_tmp612,_tmp611,_tmp610,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
({void*_tmp614=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp614;});goto _LL0;case 2U: _LL5: _LL6:
({void*_tmp61A=({struct Cyc_Tcenv_Tenv*_tmp619=te;unsigned int _tmp618=loc;void**_tmp617=topt;enum Cyc_Absyn_Primop _tmp616=p;struct Cyc_Absyn_Exp*_tmp615=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp619,_tmp618,_tmp617,_tmp616,_tmp615,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp61A;});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpE3=0U;({struct Cyc_Tcenv_Tenv*_tmp61E=te;unsigned int _tmp61D=loc;void**_tmp61C=topt;struct _dyneither_ptr _tmp61B=({const char*_tmpE4="primitive operator has > 2 arguments";_tag_dyneither(_tmpE4,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp61E,_tmp61D,_tmp61C,_tmp61B,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});}_LL0:;}
# 686
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 689
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpE5=Cyc_Tcutil_compress(t);
void*_tmpE6=_tmpE5;struct Cyc_List_List*_tmp102;void*_tmp101;struct Cyc_Absyn_Tqual _tmp100;struct Cyc_Absyn_Datatypefield*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Aggrdecl*_tmpFD;switch(*((int*)_tmpE6)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE6)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmpFD=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE6)->f1).aggr_info).KnownAggr).val;_LL2:
# 693
 if(_tmpFD->impl == 0){
({void*_tmpE7=0U;({unsigned int _tmp620=loc;struct _dyneither_ptr _tmp61F=({const char*_tmpE8="attempt to write an abstract aggregate";_tag_dyneither(_tmpE8,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp620,_tmp61F,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});
return 0;}else{
# 697
_tmpFE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFD->impl))->fields;goto _LL4;}}else{goto _LLB;}case 12U: _LL3: _tmpFE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE6)->f2;_LL4:
# 699
 for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct Cyc_Absyn_Aggrfield*_tmpE9=(struct Cyc_Absyn_Aggrfield*)_tmpFE->hd;
if((_tmpE9->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpEC=({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0U,_tmp50A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE9->name);_tmp50A;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEC;({unsigned int _tmp622=loc;struct _dyneither_ptr _tmp621=({const char*_tmpEB="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpEB,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp622,_tmp621,_tag_dyneither(_tmpEA,sizeof(void*),1U));});});
return 0;}
# 705
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpE9->type))return 0;}
# 707
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE6)->f1).field_info).KnownDatatypefield).tag == 2){_LL5: _tmpFF=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE6)->f1).field_info).KnownDatatypefield).val).f2;_LL6:
# 709
{struct Cyc_List_List*_tmpED=_tmpFF->typs;for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _tuple17*_tmpEE=(struct _tuple17*)_tmpED->hd;struct _tuple17*_tmpEF=_tmpEE;struct Cyc_Absyn_Tqual _tmpF4;void*_tmpF3;_LLE: _tmpF4=_tmpEF->f1;_tmpF3=_tmpEF->f2;_LLF:;
if(_tmpF4.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,({struct _dyneither_ptr _tmp623=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpFF->name));_tmp50B.f1=_tmp623;});_tmp50B;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF2;({unsigned int _tmp625=loc;struct _dyneither_ptr _tmp624=({const char*_tmpF1="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpF1,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp625,_tmp624,_tag_dyneither(_tmpF0,sizeof(void*),1U));});});
return 0;}
# 715
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF3))return 0;}}
# 717
return 1;}else{goto _LLB;}case 8U: _LL7: _tmp101=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE6)->f1).elt_type;_tmp100=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE6)->f1).tq;_LL8:
# 719
 if(_tmp100.real_const){
({void*_tmpF5=0U;({unsigned int _tmp627=loc;struct _dyneither_ptr _tmp626=({const char*_tmpF6="attempt to over-write a const array";_tag_dyneither(_tmpF6,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp627,_tmp626,_tag_dyneither(_tmpF5,sizeof(void*),0U));});});
return 0;}
# 723
return Cyc_Tcexp_check_writable_aggr(loc,_tmp101);case 10U: _LL9: _tmp102=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE6)->f1;_LLA:
# 725
 for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
struct _tuple17*_tmpF7=(struct _tuple17*)_tmp102->hd;struct _tuple17*_tmpF8=_tmpF7;struct Cyc_Absyn_Tqual _tmpFC;void*_tmpFB;_LL11: _tmpFC=_tmpF8->f1;_tmpFB=_tmpF8->f2;_LL12:;
if(_tmpFC.real_const){
({void*_tmpF9=0U;({unsigned int _tmp629=loc;struct _dyneither_ptr _tmp628=({const char*_tmpFA="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmpFA,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp629,_tmp628,_tag_dyneither(_tmpF9,sizeof(void*),0U));});});
return 0;}
# 731
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpFB))return 0;}
# 733
return 1;default: _LLB: _LLC:
 return 1;}_LL0:;}
# 741
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 744
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp103=e->r;void*_tmp104=_tmp103;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct _dyneither_ptr*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct _dyneither_ptr*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Vardecl*_tmp12A;struct Cyc_Absyn_Vardecl*_tmp129;struct Cyc_Absyn_Vardecl*_tmp128;struct Cyc_Absyn_Vardecl*_tmp127;switch(*((int*)_tmp104)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)){case 3U: _LL1: _tmp127=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL2:
 _tmp128=_tmp127;goto _LL4;case 4U: _LL3: _tmp128=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL4:
 _tmp129=_tmp128;goto _LL6;case 5U: _LL5: _tmp129=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL6:
 _tmp12A=_tmp129;goto _LL8;case 1U: _LL7: _tmp12A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL8:
 if(!(_tmp12A->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp12C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_tmp12B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp104)->f2;_LLA:
# 751
{void*_tmp105=Cyc_Tcutil_compress((void*)_check_null(_tmp12C->topt));void*_tmp106=_tmp105;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Tqual _tmp116;struct Cyc_Absyn_Tqual _tmp115;switch(*((int*)_tmp106)){case 5U: _LL18: _tmp115=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp106)->f1).elt_tq;_LL19:
 _tmp116=_tmp115;goto _LL1B;case 8U: _LL1A: _tmp116=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp106)->f1).tq;_LL1B:
 if(!_tmp116.real_const)return;goto _LL17;case 10U: _LL1C: _tmp117=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp106)->f1;_LL1D: {
# 755
struct _tuple15 _tmp107=Cyc_Evexp_eval_const_uint_exp(_tmp12B);struct _tuple15 _tmp108=_tmp107;unsigned int _tmp114;int _tmp113;_LL21: _tmp114=_tmp108.f1;_tmp113=_tmp108.f2;_LL22:;
if(!_tmp113){
({void*_tmp109=0U;({unsigned int _tmp62B=e->loc;struct _dyneither_ptr _tmp62A=({const char*_tmp10A="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp10A,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp62B,_tmp62A,_tag_dyneither(_tmp109,sizeof(void*),0U));});});
return;}
# 760
{struct _handler_cons _tmp10B;_push_handler(& _tmp10B);{int _tmp10D=0;if(setjmp(_tmp10B.handler))_tmp10D=1;if(!_tmp10D){
{struct _tuple17*_tmp10E=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp117,(int)_tmp114);struct _tuple17*_tmp10F=_tmp10E;struct Cyc_Absyn_Tqual _tmp110;_LL24: _tmp110=_tmp10F->f1;_LL25:;
if(!_tmp110.real_const){_npop_handler(0U);return;}}
# 761
;_pop_handler();}else{void*_tmp10C=(void*)_exn_thrown;void*_tmp111=_tmp10C;void*_tmp112;if(((struct Cyc_List_Nth_exn_struct*)_tmp111)->tag == Cyc_List_Nth){_LL27: _LL28:
# 763
 return;}else{_LL29: _tmp112=_tmp111;_LL2A:(int)_rethrow(_tmp112);}_LL26:;}};}
goto _LL17;}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 767
goto _LL0;case 21U: _LLB: _tmp12E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_tmp12D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp104)->f2;_LLC:
# 769
{void*_tmp118=Cyc_Tcutil_compress((void*)_check_null(_tmp12E->topt));void*_tmp119=_tmp118;struct Cyc_List_List*_tmp11B;struct Cyc_Absyn_Aggrdecl**_tmp11A;switch(*((int*)_tmp119)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp119)->f1).aggr_info).KnownAggr).tag == 2){_LL2C: _tmp11A=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp119)->f1).aggr_info).KnownAggr).val;_LL2D: {
# 771
struct Cyc_Absyn_Aggrfield*sf=
_tmp11A == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp11A,_tmp12D);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}case 12U: _LL2E: _tmp11B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp119)->f2;_LL2F: {
# 776
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp11B,_tmp12D);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 781
goto _LL0;case 22U: _LLD: _tmp130=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_tmp12F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp104)->f2;_LLE:
# 783
{void*_tmp11C=Cyc_Tcutil_compress((void*)_check_null(_tmp130->topt));void*_tmp11D=_tmp11C;void*_tmp122;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11D)->tag == 5U){_LL33: _tmp122=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11D)->f1).elt_typ;_LL34:
# 785
{void*_tmp11E=Cyc_Tcutil_compress(_tmp122);void*_tmp11F=_tmp11E;struct Cyc_List_List*_tmp121;struct Cyc_Absyn_Aggrdecl**_tmp120;switch(*((int*)_tmp11F)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11F)->f1).aggr_info).KnownAggr).tag == 2){_LL38: _tmp120=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11F)->f1).aggr_info).KnownAggr).val;_LL39: {
# 787
struct Cyc_Absyn_Aggrfield*sf=
_tmp120 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp120,_tmp12F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}case 12U: _LL3A: _tmp121=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11F)->f2;_LL3B: {
# 792
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp121,_tmp12F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 797
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 800
goto _LL0;case 20U: _LLF: _tmp131=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_LL10:
# 802
{void*_tmp123=Cyc_Tcutil_compress((void*)_check_null(_tmp131->topt));void*_tmp124=_tmp123;struct Cyc_Absyn_Tqual _tmp126;struct Cyc_Absyn_Tqual _tmp125;switch(*((int*)_tmp124)){case 5U: _LL3F: _tmp125=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp124)->f1).elt_tq;_LL40:
 _tmp126=_tmp125;goto _LL42;case 8U: _LL41: _tmp126=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp124)->f1).tq;_LL42:
 if(!_tmp126.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 807
goto _LL0;case 12U: _LL11: _tmp132=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_LL12:
 _tmp133=_tmp132;goto _LL14;case 13U: _LL13: _tmp133=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp133);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 812
({struct Cyc_String_pa_PrintArg_struct _tmp136=({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0U,({struct _dyneither_ptr _tmp62C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp50C.f1=_tmp62C;});_tmp50C;});void*_tmp134[1U];_tmp134[0]=& _tmp136;({unsigned int _tmp62E=e->loc;struct _dyneither_ptr _tmp62D=({const char*_tmp135="attempt to write a const location: %s";_tag_dyneither(_tmp135,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp62E,_tmp62D,_tag_dyneither(_tmp134,sizeof(void*),1U));});});}
# 815
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 818
({struct Cyc_Tcenv_Tenv*_tmp62F=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp62F,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp137=0U;({struct Cyc_Tcenv_Tenv*_tmp633=te;unsigned int _tmp632=loc;void**_tmp631=topt;struct _dyneither_ptr _tmp630=({const char*_tmp138="increment/decrement of non-lvalue";_tag_dyneither(_tmp138,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp633,_tmp632,_tmp631,_tmp630,_tag_dyneither(_tmp137,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 824
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp139=0U;({unsigned int _tmp635=e->loc;struct _dyneither_ptr _tmp634=({const char*_tmp13A="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp13A,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp635,_tmp634,_tag_dyneither(_tmp139,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp13B=0U;({unsigned int _tmp637=e->loc;struct _dyneither_ptr _tmp636=({const char*_tmp13C="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp13C,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp637,_tmp636,_tag_dyneither(_tmp13B,sizeof(void*),0U));});});}else{
# 833
({struct Cyc_String_pa_PrintArg_struct _tmp13F=({struct Cyc_String_pa_PrintArg_struct _tmp50D;_tmp50D.tag=0U,({struct _dyneither_ptr _tmp638=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp50D.f1=_tmp638;});_tmp50D;});void*_tmp13D[1U];_tmp13D[0]=& _tmp13F;({unsigned int _tmp63A=e->loc;struct _dyneither_ptr _tmp639=({const char*_tmp13E="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp13E,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp63A,_tmp639,_tag_dyneither(_tmp13D,sizeof(void*),1U));});});}}
# 835
return t;};}
# 839
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 841
({struct Cyc_Tcenv_Tenv*_tmp63C=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp63B=e1;Cyc_Tcexp_tcTest(_tmp63C,_tmp63B,({const char*_tmp140="conditional expression";_tag_dyneither(_tmp140,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp63D=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp63D;});else{
# 848
({void*_tmp63E=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp63E;});}{
struct Cyc_List_List _tmp141=({struct Cyc_List_List _tmp511;_tmp511.hd=e3,_tmp511.tl=0;_tmp511;});
struct Cyc_List_List _tmp142=({struct Cyc_List_List _tmp510;_tmp510.hd=e2,_tmp510.tl=& _tmp141;_tmp510;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp142)){
({struct Cyc_String_pa_PrintArg_struct _tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0U,({
struct _dyneither_ptr _tmp63F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp50F.f1=_tmp63F;});_tmp50F;});struct Cyc_String_pa_PrintArg_struct _tmp146=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0U,({struct _dyneither_ptr _tmp640=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp50E.f1=_tmp640;});_tmp50E;});void*_tmp143[2U];_tmp143[0]=& _tmp145,_tmp143[1]=& _tmp146;({unsigned int _tmp642=loc;struct _dyneither_ptr _tmp641=({const char*_tmp144="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp144,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp642,_tmp641,_tag_dyneither(_tmp143,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 856
return t;};};}
# 860
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 862
({struct Cyc_Tcenv_Tenv*_tmp644=te;struct Cyc_Absyn_Exp*_tmp643=e1;Cyc_Tcexp_tcTest(_tmp644,_tmp643,({const char*_tmp147="logical-and expression";_tag_dyneither(_tmp147,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp646=te;struct Cyc_Absyn_Exp*_tmp645=e2;Cyc_Tcexp_tcTest(_tmp646,_tmp645,({const char*_tmp148="logical-and expression";_tag_dyneither(_tmp148,sizeof(char),23U);}));});
return Cyc_Absyn_sint_typ;}
# 868
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 870
({struct Cyc_Tcenv_Tenv*_tmp648=te;struct Cyc_Absyn_Exp*_tmp647=e1;Cyc_Tcexp_tcTest(_tmp648,_tmp647,({const char*_tmp149="logical-or expression";_tag_dyneither(_tmp149,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp64A=te;struct Cyc_Absyn_Exp*_tmp649=e2;Cyc_Tcexp_tcTest(_tmp64A,_tmp649,({const char*_tmp14A="logical-or expression";_tag_dyneither(_tmp14A,sizeof(char),22U);}));});
return Cyc_Absyn_sint_typ;}
# 876
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 883
({struct Cyc_Tcenv_Tenv*_tmp64B=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp64B,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 888
{void*_tmp14B=Cyc_Tcutil_compress(t1);void*_tmp14C=_tmp14B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14C)->tag == 8U){_LL1: _LL2:
({void*_tmp14D=0U;({unsigned int _tmp64D=loc;struct _dyneither_ptr _tmp64C=({const char*_tmp14E="cannot assign to an array";_tag_dyneither(_tmp14E,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp64D,_tmp64C,_tag_dyneither(_tmp14D,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 893
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp14F=0U;({unsigned int _tmp64F=loc;struct _dyneither_ptr _tmp64E=({const char*_tmp150="type is abstract (can't determine size).";_tag_dyneither(_tmp150,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp64F,_tmp64E,_tag_dyneither(_tmp14F,sizeof(void*),0U));});});
# 897
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp151=0U;({struct Cyc_Tcenv_Tenv*_tmp653=te;unsigned int _tmp652=loc;void**_tmp651=topt;struct _dyneither_ptr _tmp650=({const char*_tmp152="assignment to non-lvalue";_tag_dyneither(_tmp152,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp653,_tmp652,_tmp651,_tmp650,_tag_dyneither(_tmp151,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp153=0U;({unsigned int _tmp655=e2->loc;struct _dyneither_ptr _tmp654=({const char*_tmp154="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp154,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp655,_tmp654,_tag_dyneither(_tmp153,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp155=({struct Cyc_String_pa_PrintArg_struct _tmp158=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,({
struct _dyneither_ptr _tmp656=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp513.f1=_tmp656;});_tmp513;});struct Cyc_String_pa_PrintArg_struct _tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0U,({struct _dyneither_ptr _tmp657=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp512.f1=_tmp657;});_tmp512;});void*_tmp156[2U];_tmp156[0]=& _tmp158,_tmp156[1]=& _tmp159;({struct Cyc_Tcenv_Tenv*_tmp65B=te;unsigned int _tmp65A=loc;void**_tmp659=topt;struct _dyneither_ptr _tmp658=({const char*_tmp157="type mismatch: %s != %s";_tag_dyneither(_tmp157,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp65B,_tmp65A,_tmp659,_tmp658,_tag_dyneither(_tmp156,sizeof(void*),2U));});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp155;}}else{
# 911
enum Cyc_Absyn_Primop _tmp15A=(enum Cyc_Absyn_Primop)po->v;
void*_tmp15B=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp15A,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp15B,t1) || Cyc_Tcutil_coerceable(_tmp15B) && Cyc_Tcutil_coerceable(t1))){
void*_tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp15F=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,({
# 916
struct _dyneither_ptr _tmp65C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp515.f1=_tmp65C;});_tmp515;});struct Cyc_String_pa_PrintArg_struct _tmp160=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,({
struct _dyneither_ptr _tmp65D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp514.f1=_tmp65D;});_tmp514;});void*_tmp15D[2U];_tmp15D[0]=& _tmp15F,_tmp15D[1]=& _tmp160;({struct Cyc_Tcenv_Tenv*_tmp661=te;unsigned int _tmp660=loc;void**_tmp65F=topt;struct _dyneither_ptr _tmp65E=({const char*_tmp15E="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp15E,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp661,_tmp660,_tmp65F,_tmp65E,_tag_dyneither(_tmp15D,sizeof(void*),2U));});});
Cyc_Tcutil_unify(_tmp15B,t1);
Cyc_Tcutil_explain_failure();
return _tmp15C;}
# 922
return _tmp15B;}
# 924
return t1;};};}
# 928
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp662=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp662,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp664=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp663=topt;Cyc_Tcexp_tcExp(_tmp664,_tmp663,e2);});
return(void*)_check_null(e2->topt);}
# 935
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 938
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 941
{void*_tmp161=Cyc_Tcutil_compress(t1);void*_tmp162=_tmp161;switch(*((int*)_tmp162)){case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp162)->f1 == 0){_LL1: _LL2:
({struct Cyc_Tcenv_Tenv*_tmp666=te;struct Cyc_Absyn_Exp*_tmp665=e;Cyc_Tcutil_unchecked_cast(_tmp666,_tmp665,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp667=Cyc_Absyn_float_typ(1);t1=_tmp667;});goto _LL0;}else{goto _LL7;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp162)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL0;default: goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 948
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp163=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp164=_tmp163;struct _tuple1*_tmp168;struct Cyc_List_List*_tmp167;unsigned int _tmp166;enum Cyc_Absyn_Scope _tmp165;_LLA: _tmp168=_tmp164.name;_tmp167=_tmp164.typs;_tmp166=_tmp164.loc;_tmp165=_tmp164.sc;_LLB:;
# 951
if(_tmp167 == 0  || _tmp167->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp167->hd)).f2);
# 954
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 959
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp169=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp16A=_tmp169;struct _tuple1*_tmp16E;struct Cyc_List_List*_tmp16D;unsigned int _tmp16C;enum Cyc_Absyn_Scope _tmp16B;_LLD: _tmp16E=_tmp16A.name;_tmp16D=_tmp16A.typs;_tmp16C=_tmp16A.loc;_tmp16B=_tmp16A.sc;_LLE:;
# 962
if(_tmp16D == 0  || _tmp16D->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp16D->hd)).f2);
# 966
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 971
({struct Cyc_String_pa_PrintArg_struct _tmp171=({struct Cyc_String_pa_PrintArg_struct _tmp517;_tmp517.tag=0U,({
struct _dyneither_ptr _tmp668=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp517.f1=_tmp668;});_tmp517;});struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp516;_tmp516.tag=0U,({struct _dyneither_ptr _tmp669=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp516.f1=_tmp669;});_tmp516;});void*_tmp16F[2U];_tmp16F[0]=& _tmp171,_tmp16F[1]=& _tmp172;({unsigned int _tmp66B=e->loc;struct _dyneither_ptr _tmp66A=({const char*_tmp170="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp170,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp66B,_tmp66A,_tag_dyneither(_tmp16F,sizeof(void*),2U));});});
return 0;}
# 977
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 983
struct Cyc_List_List*_tmp173=args;
int _tmp174=0;
struct Cyc_Tcenv_Tenv*_tmp175=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp176=Cyc_Tcenv_clear_abstract_val_ok(_tmp175);
Cyc_Tcexp_tcExp(_tmp176,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 992
void*_tmp177=t;void*_tmp1DA;struct Cyc_Absyn_Tqual _tmp1D9;void*_tmp1D8;union Cyc_Absyn_Constraint*_tmp1D7;union Cyc_Absyn_Constraint*_tmp1D6;union Cyc_Absyn_Constraint*_tmp1D5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp177)->tag == 5U){_LL1: _tmp1DA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp177)->f1).elt_typ;_tmp1D9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp177)->f1).elt_tq;_tmp1D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp177)->f1).ptr_atts).rgn;_tmp1D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp177)->f1).ptr_atts).nullable;_tmp1D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp177)->f1).ptr_atts).bounds;_tmp1D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp177)->f1).ptr_atts).zero_term;_LL2:
# 997
 Cyc_Tcenv_check_rgn_accessible(_tmp176,loc,_tmp1D8);
# 999
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1D6);{
void*_tmp178=Cyc_Tcutil_compress(_tmp1DA);void*_tmp179=_tmp178;struct Cyc_List_List*_tmp1D1;void*_tmp1D0;struct Cyc_Absyn_Tqual _tmp1CF;void*_tmp1CE;struct Cyc_List_List*_tmp1CD;int _tmp1CC;struct Cyc_Absyn_VarargInfo*_tmp1CB;struct Cyc_List_List*_tmp1CA;struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_List_List*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_List_List*_tmp1C5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->tag == 9U){_LL6: _tmp1D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).tvars;_tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).effect;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_tqual;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_typ;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).args;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).c_varargs;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).cyc_varargs;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).rgn_po;_tmp1C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).attributes;_tmp1C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).requires_clause;_tmp1C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).requires_relns;_tmp1C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ensures_clause;_tmp1C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ensures_relns;_LL7:
# 1004
 if(_tmp1D1 != 0  || _tmp1CA != 0)
({void*_tmp17A=0U;({unsigned int _tmp66D=e->loc;struct _dyneither_ptr _tmp66C=({const char*_tmp17B="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp17B,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp66D,_tmp66C,_tag_dyneither(_tmp17A,sizeof(void*),0U));});});
# 1008
if(topt != 0)Cyc_Tcutil_unify(_tmp1CE,*topt);
# 1010
while(_tmp173 != 0  && _tmp1CD != 0){
# 1012
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp173->hd;
void*t2=(*((struct _tuple9*)_tmp1CD->hd)).f3;
Cyc_Tcexp_tcExp(_tmp176,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp176,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp18B="actual argument has type ";_tag_dyneither(_tmp18B,sizeof(char),26U);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp18A=" but formal has type ";_tag_dyneither(_tmp18A,sizeof(char),22U);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp670=({unsigned long _tmp66F=({unsigned long _tmp66E=Cyc_strlen((struct _dyneither_ptr)s0);_tmp66E + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp66F + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp670 + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp17C=0U;({unsigned int _tmp673=e1->loc;struct _dyneither_ptr _tmp672=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp51B;_tmp51B.tag=0U,_tmp51B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp51B;});struct Cyc_String_pa_PrintArg_struct _tmp180=({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0U,_tmp51A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp51A;});struct Cyc_String_pa_PrintArg_struct _tmp181=({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0U,_tmp519.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp519;});struct Cyc_String_pa_PrintArg_struct _tmp182=({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0U,_tmp518.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp518;});void*_tmp17D[4U];_tmp17D[0]=& _tmp17F,_tmp17D[1]=& _tmp180,_tmp17D[2]=& _tmp181,_tmp17D[3]=& _tmp182;({struct _dyneither_ptr _tmp671=({const char*_tmp17E="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp17E,sizeof(char),15U);});Cyc_aprintf(_tmp671,_tag_dyneither(_tmp17D,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp673,_tmp672,_tag_dyneither(_tmp17C,sizeof(void*),0U));});});else{
# 1024
({void*_tmp183=0U;({unsigned int _tmp676=e1->loc;struct _dyneither_ptr _tmp675=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp186=({struct Cyc_String_pa_PrintArg_struct _tmp51F;_tmp51F.tag=0U,_tmp51F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp51F;});struct Cyc_String_pa_PrintArg_struct _tmp187=({struct Cyc_String_pa_PrintArg_struct _tmp51E;_tmp51E.tag=0U,_tmp51E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp51E;});struct Cyc_String_pa_PrintArg_struct _tmp188=({struct Cyc_String_pa_PrintArg_struct _tmp51D;_tmp51D.tag=0U,_tmp51D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp51D;});struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp51C;_tmp51C.tag=0U,_tmp51C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp51C;});void*_tmp184[4U];_tmp184[0]=& _tmp186,_tmp184[1]=& _tmp187,_tmp184[2]=& _tmp188,_tmp184[3]=& _tmp189;({struct _dyneither_ptr _tmp674=({const char*_tmp185="%s%s%s%s.";_tag_dyneither(_tmp185,sizeof(char),10U);});Cyc_aprintf(_tmp674,_tag_dyneither(_tmp184,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp676,_tmp675,_tag_dyneither(_tmp183,sizeof(void*),0U));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1029
if(alias_coercion)
({struct Cyc_List_List*_tmp677=({struct Cyc_List_List*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->hd=(void*)_tmp174,_tmp18C->tl=*alias_arg_exps;_tmp18C;});*alias_arg_exps=_tmp677;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp18D=0U;({unsigned int _tmp679=e1->loc;struct _dyneither_ptr _tmp678=({const char*_tmp18E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp18E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp679,_tmp678,_tag_dyneither(_tmp18D,sizeof(void*),0U));});});
_tmp173=_tmp173->tl;
_tmp1CD=_tmp1CD->tl;
++ _tmp174;}{
# 1040
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1C9;for(0;a != 0;a=a->tl){
void*_tmp18F=(void*)a->hd;void*_tmp190=_tmp18F;enum Cyc_Absyn_Format_Type _tmp19D;int _tmp19C;int _tmp19B;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp190)->tag == 19U){_LLB: _tmp19D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp190)->f1;_tmp19C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp190)->f2;_tmp19B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp190)->f3;_LLC:
# 1044
{struct _handler_cons _tmp191;_push_handler(& _tmp191);{int _tmp193=0;if(setjmp(_tmp191.handler))_tmp193=1;if(!_tmp193){
# 1046
{struct Cyc_Absyn_Exp*_tmp194=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp19C - 1);
# 1048
struct Cyc_Core_Opt*fmt_args;
if(_tmp19B == 0)
fmt_args=0;else{
# 1052
({struct Cyc_Core_Opt*_tmp67B=({struct Cyc_Core_Opt*_tmp195=_cycalloc(sizeof(*_tmp195));({struct Cyc_List_List*_tmp67A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp19B - 1);_tmp195->v=_tmp67A;});_tmp195;});fmt_args=_tmp67B;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp196=_tmp19D;if(_tmp196 == Cyc_Absyn_Printf_ft){_LL10: _LL11:
# 1056
 Cyc_Tcexp_check_format_args(_tmp176,_tmp194,fmt_args,_tmp19B - 1,alias_arg_exps,Cyc_Formatstr_get_format_typs);
# 1059
goto _LLF;}else{_LL12: _LL13:
# 1061
 Cyc_Tcexp_check_format_args(_tmp176,_tmp194,fmt_args,_tmp19B - 1,alias_arg_exps,Cyc_Formatstr_get_scanf_typs);
# 1064
goto _LLF;}_LLF:;};}
# 1046
;_pop_handler();}else{void*_tmp192=(void*)_exn_thrown;void*_tmp197=_tmp192;void*_tmp19A;if(((struct Cyc_List_Nth_exn_struct*)_tmp197)->tag == Cyc_List_Nth){_LL15: _LL16:
# 1066
({void*_tmp198=0U;({unsigned int _tmp67D=loc;struct _dyneither_ptr _tmp67C=({const char*_tmp199="bad format arguments";_tag_dyneither(_tmp199,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp67D,_tmp67C,_tag_dyneither(_tmp198,sizeof(void*),0U));});});goto _LL14;}else{_LL17: _tmp19A=_tmp197;_LL18:(int)_rethrow(_tmp19A);}_LL14:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1071
if(_tmp1CD != 0)({void*_tmp19E=0U;({unsigned int _tmp67F=loc;struct _dyneither_ptr _tmp67E=({const char*_tmp19F="too few arguments for function";_tag_dyneither(_tmp19F,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp67F,_tmp67E,_tag_dyneither(_tmp19E,sizeof(void*),0U));});});else{
# 1073
if((_tmp173 != 0  || _tmp1CC) || _tmp1CB != 0){
if(_tmp1CC)
for(0;_tmp173 != 0;_tmp173=_tmp173->tl){
Cyc_Tcexp_tcExp(_tmp176,0,(struct Cyc_Absyn_Exp*)_tmp173->hd);}else{
if(_tmp1CB == 0)
({void*_tmp1A0=0U;({unsigned int _tmp681=loc;struct _dyneither_ptr _tmp680=({const char*_tmp1A1="too many arguments for function";_tag_dyneither(_tmp1A1,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp681,_tmp680,_tag_dyneither(_tmp1A0,sizeof(void*),0U));});});else{
# 1080
struct Cyc_Absyn_VarargInfo _tmp1A2=*_tmp1CB;struct Cyc_Absyn_VarargInfo _tmp1A3=_tmp1A2;void*_tmp1C1;int _tmp1C0;_LL1A: _tmp1C1=_tmp1A3.type;_tmp1C0=_tmp1A3.inject;_LL1B:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1A4=({struct Cyc_Absyn_VarargCallInfo*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->num_varargs=0,_tmp1BF->injectors=0,_tmp1BF->vai=_tmp1CB;_tmp1BF;});
# 1084
*vararg_call_info=_tmp1A4;
# 1086
if(!_tmp1C0)
# 1088
for(0;_tmp173 != 0;(_tmp173=_tmp173->tl,_tmp174 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp173->hd;
++ _tmp1A4->num_varargs;
Cyc_Tcexp_tcExp(_tmp176,& _tmp1C1,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp176,e1,_tmp1C1,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1A7=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0U,({
struct _dyneither_ptr _tmp682=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C1));_tmp521.f1=_tmp682;});_tmp521;});struct Cyc_String_pa_PrintArg_struct _tmp1A8=({struct Cyc_String_pa_PrintArg_struct _tmp520;_tmp520.tag=0U,({struct _dyneither_ptr _tmp683=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp520.f1=_tmp683;});_tmp520;});void*_tmp1A5[2U];_tmp1A5[0]=& _tmp1A7,_tmp1A5[1]=& _tmp1A8;({unsigned int _tmp685=loc;struct _dyneither_ptr _tmp684=({const char*_tmp1A6="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1A6,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp685,_tmp684,_tag_dyneither(_tmp1A5,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 1098
if(alias_coercion)
({struct Cyc_List_List*_tmp686=({struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->hd=(void*)_tmp174,_tmp1A9->tl=*alias_arg_exps;_tmp1A9;});*alias_arg_exps=_tmp686;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1C1) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1AA=0U;({unsigned int _tmp688=e1->loc;struct _dyneither_ptr _tmp687=({const char*_tmp1AB="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1AB,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp688,_tmp687,_tag_dyneither(_tmp1AA,sizeof(void*),0U));});});}else{
# 1107
void*_tmp1AC=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1C1));void*_tmp1AD=_tmp1AC;struct Cyc_Absyn_Datatypedecl*_tmp1BE;struct Cyc_List_List*_tmp1BD;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AD)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AD)->f1).datatype_info).KnownDatatype).tag == 2){_LL1D: _tmp1BE=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AD)->f1).datatype_info).KnownDatatype).val;_tmp1BD=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AD)->f1).targs;_LL1E: {
# 1111
struct Cyc_Absyn_Datatypedecl*_tmp1AE=*Cyc_Tcenv_lookup_datatypedecl(_tmp176,loc,_tmp1BE->name);
struct Cyc_List_List*fields=0;
if(_tmp1AE->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp522;_tmp522.tag=0U,({
struct _dyneither_ptr _tmp689=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C1));_tmp522.f1=_tmp689;});_tmp522;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({unsigned int _tmp68B=loc;struct _dyneither_ptr _tmp68A=({const char*_tmp1B0="can't inject into abstract datatype %s";_tag_dyneither(_tmp1B0,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp68B,_tmp68A,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AE->fields))->v;}
# 1122
if(!({void*_tmp68C=Cyc_Tcutil_pointer_region(_tmp1C1);Cyc_Tcutil_unify(_tmp68C,Cyc_Tcenv_curr_rgn(_tmp176));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,({
struct _dyneither_ptr _tmp68D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C1));_tmp524.f1=_tmp68D;});_tmp524;});struct Cyc_String_pa_PrintArg_struct _tmp1B5=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0U,({struct _dyneither_ptr _tmp68E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp176)));_tmp523.f1=_tmp68E;});_tmp523;});void*_tmp1B2[2U];_tmp1B2[0]=& _tmp1B4,_tmp1B2[1]=& _tmp1B5;({unsigned int _tmp690=loc;struct _dyneither_ptr _tmp68F=({const char*_tmp1B3="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1B3,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp690,_tmp68F,_tag_dyneither(_tmp1B2,sizeof(void*),2U));});});{
# 1126
struct Cyc_List_List*_tmp1B6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1AE->tvs,_tmp1BD);
for(0;_tmp173 != 0;_tmp173=_tmp173->tl){
++ _tmp1A4->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp173->hd;
# 1131
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp176,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1B7=0U;({unsigned int _tmp692=e1->loc;struct _dyneither_ptr _tmp691=({const char*_tmp1B8="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1B8,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp692,_tmp691,_tag_dyneither(_tmp1B7,sizeof(void*),0U));});});}{
# 1137
struct Cyc_Absyn_Datatypefield*_tmp1B9=({struct Cyc_Tcenv_Tenv*_tmp696=_tmp176;struct Cyc_Absyn_Exp*_tmp695=e1;void*_tmp694=Cyc_Tcutil_pointer_elt_type(_tmp1C1);struct Cyc_List_List*_tmp693=_tmp1B6;Cyc_Tcexp_tcInjection(_tmp696,_tmp695,_tmp694,_tmp693,fields);});
if(_tmp1B9 != 0)
({struct Cyc_List_List*_tmp698=({
struct Cyc_List_List*_tmp697=_tmp1A4->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp697,({struct Cyc_List_List*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));
_tmp1BA->hd=_tmp1B9,_tmp1BA->tl=0;_tmp1BA;}));});
# 1139
_tmp1A4->injectors=_tmp698;});};};}
# 1143
goto _LL1C;};}}else{goto _LL1F;}}else{_LL1F: _LL20:
({void*_tmp1BB=0U;({unsigned int _tmp69A=loc;struct _dyneither_ptr _tmp699=({const char*_tmp1BC="bad inject vararg type";_tag_dyneither(_tmp1BC,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp69A,_tmp699,_tag_dyneither(_tmp1BB,sizeof(void*),0U));});});goto _LL1C;}_LL1C:;}};}}}}
# 1149
if(*alias_arg_exps == 0)
# 1158 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp176,loc,(void*)_check_null(_tmp1D0));
# 1160
return _tmp1CE;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1C4=({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0U,({struct _dyneither_ptr _tmp69B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp525.f1=_tmp69B;});_tmp525;});void*_tmp1C2[1U];_tmp1C2[0]=& _tmp1C4;({struct Cyc_Tcenv_Tenv*_tmp69F=_tmp176;unsigned int _tmp69E=loc;void**_tmp69D=topt;struct _dyneither_ptr _tmp69C=({const char*_tmp1C3="expected pointer to function but found %s";_tag_dyneither(_tmp1C3,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp69F,_tmp69E,_tmp69D,_tmp69C,_tag_dyneither(_tmp1C2,sizeof(void*),1U));});});}_LL5:;};}else{_LL3: _LL4:
# 1163
 return({struct Cyc_String_pa_PrintArg_struct _tmp1D4=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,({struct _dyneither_ptr _tmp6A0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp526.f1=_tmp6A0;});_tmp526;});void*_tmp1D2[1U];_tmp1D2[0]=& _tmp1D4;({struct Cyc_Tcenv_Tenv*_tmp6A4=_tmp176;unsigned int _tmp6A3=loc;void**_tmp6A2=topt;struct _dyneither_ptr _tmp6A1=({const char*_tmp1D3="expected pointer to function but found %s";_tag_dyneither(_tmp1D3,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp6A4,_tmp6A3,_tmp6A2,_tmp6A1,_tag_dyneither(_tmp1D2,sizeof(void*),1U));});});}_LL0:;};}
# 1167
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1DB=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1DB,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1DB,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1DE=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0U,({struct _dyneither_ptr _tmp6A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1DB));_tmp528.f1=_tmp6A5;});_tmp528;});struct Cyc_String_pa_PrintArg_struct _tmp1DF=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,({
struct _dyneither_ptr _tmp6A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp527.f1=_tmp6A6;});_tmp527;});void*_tmp1DC[2U];_tmp1DC[0]=& _tmp1DE,_tmp1DC[1]=& _tmp1DF;({unsigned int _tmp6A8=loc;struct _dyneither_ptr _tmp6A7=({const char*_tmp1DD="expected %s but found %s";_tag_dyneither(_tmp1DD,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp6A8,_tmp6A7,_tag_dyneither(_tmp1DC,sizeof(void*),2U));});});
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1179
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1181
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1183
({void*_tmp6A9=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp6A9;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1E0=t1;void*_tmp1FB;struct Cyc_Absyn_Tqual _tmp1FA;void*_tmp1F9;union Cyc_Absyn_Constraint*_tmp1F8;union Cyc_Absyn_Constraint*_tmp1F7;union Cyc_Absyn_Constraint*_tmp1F6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->tag == 5U){_LL1: _tmp1FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).elt_typ;_tmp1FA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).elt_tq;_tmp1F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).ptr_atts).rgn;_tmp1F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).ptr_atts).nullable;_tmp1F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).ptr_atts).bounds;_tmp1F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0)->f1).ptr_atts).zero_term;_LL2:
# 1187
{void*_tmp1E1=Cyc_Tcutil_compress(_tmp1FB);void*_tmp1E2=_tmp1E1;struct Cyc_List_List*_tmp1F5;void*_tmp1F4;struct Cyc_Absyn_Tqual _tmp1F3;void*_tmp1F2;struct Cyc_List_List*_tmp1F1;int _tmp1F0;struct Cyc_Absyn_VarargInfo*_tmp1EF;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_List_List*_tmp1E9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->tag == 9U){_LL6: _tmp1F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).tvars;_tmp1F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).effect;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).ret_tqual;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).ret_typ;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).args;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).c_varargs;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).cyc_varargs;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).rgn_po;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).attributes;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).requires_clause;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).requires_relns;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).ensures_clause;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E2)->f1).ensures_relns;_LL7: {
# 1189
struct Cyc_List_List*instantiation=0;
# 1191
for(0;ts != 0  && _tmp1F5 != 0;(ts=ts->tl,_tmp1F5=_tmp1F5->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1F5->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp6AD=loc;struct Cyc_Tcenv_Tenv*_tmp6AC=te;struct Cyc_List_List*_tmp6AB=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6AA=k;Cyc_Tcutil_check_type(_tmp6AD,_tmp6AC,_tmp6AB,_tmp6AA,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp6AF=({struct Cyc_List_List*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));({struct _tuple13*_tmp6AE=({struct _tuple13*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->f1=(struct Cyc_Absyn_Tvar*)_tmp1F5->hd,_tmp1E3->f2=(void*)ts->hd;_tmp1E3;});_tmp1E4->hd=_tmp6AE;}),_tmp1E4->tl=instantiation;_tmp1E4;});instantiation=_tmp6AF;});}
# 1197
if(ts != 0)
return({void*_tmp1E5=0U;({struct Cyc_Tcenv_Tenv*_tmp6B3=te;unsigned int _tmp6B2=loc;void**_tmp6B1=topt;struct _dyneither_ptr _tmp6B0=({const char*_tmp1E6="too many type variables in instantiation";_tag_dyneither(_tmp1E6,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp6B3,_tmp6B2,_tmp6B1,_tmp6B0,_tag_dyneither(_tmp1E5,sizeof(void*),0U));});});
# 1202
if(_tmp1F5 == 0){
({struct Cyc_List_List*_tmp6B4=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1EE);_tmp1EE=_tmp6B4;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1EE);
_tmp1EE=0;}{
# 1207
void*new_fn_typ=({
struct Cyc_List_List*_tmp6B5=instantiation;Cyc_Tcutil_substitute(_tmp6B5,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));
_tmp1E8->tag=9U,(_tmp1E8->f1).tvars=_tmp1F5,(_tmp1E8->f1).effect=_tmp1F4,(_tmp1E8->f1).ret_tqual=_tmp1F3,(_tmp1E8->f1).ret_typ=_tmp1F2,(_tmp1E8->f1).args=_tmp1F1,(_tmp1E8->f1).c_varargs=_tmp1F0,(_tmp1E8->f1).cyc_varargs=_tmp1EF,(_tmp1E8->f1).rgn_po=_tmp1EE,(_tmp1E8->f1).attributes=_tmp1ED,(_tmp1E8->f1).requires_clause=_tmp1EC,(_tmp1E8->f1).requires_relns=_tmp1EB,(_tmp1E8->f1).ensures_clause=_tmp1EA,(_tmp1E8->f1).ensures_relns=_tmp1E9;_tmp1E8;}));});
# 1212
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->tag=5U,(_tmp1E7->f1).elt_typ=new_fn_typ,(_tmp1E7->f1).elt_tq=_tmp1FA,((_tmp1E7->f1).ptr_atts).rgn=_tmp1F9,((_tmp1E7->f1).ptr_atts).nullable=_tmp1F8,((_tmp1E7->f1).ptr_atts).bounds=_tmp1F7,((_tmp1E7->f1).ptr_atts).zero_term=_tmp1F6,((_tmp1E7->f1).ptr_atts).ptrloc=0;_tmp1E7;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1215
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1218
return({struct Cyc_String_pa_PrintArg_struct _tmp1FE=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,({
struct _dyneither_ptr _tmp6B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp529.f1=_tmp6B6;});_tmp529;});void*_tmp1FC[1U];_tmp1FC[0]=& _tmp1FE;({struct Cyc_Tcenv_Tenv*_tmp6BA=te;unsigned int _tmp6B9=loc;void**_tmp6B8=topt;struct _dyneither_ptr _tmp6B7=({const char*_tmp1FD="expecting polymorphic type but found %s";_tag_dyneither(_tmp1FD,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp6BA,_tmp6B9,_tmp6B8,_tmp6B7,_tag_dyneither(_tmp1FC,sizeof(void*),1U));});});};}
# 1223
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1225
({unsigned int _tmp6BE=loc;struct Cyc_Tcenv_Tenv*_tmp6BD=te;struct Cyc_List_List*_tmp6BC=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6BB=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1225
Cyc_Tcutil_check_type(_tmp6BE,_tmp6BD,_tmp6BC,_tmp6BB,1,0,t);});
# 1227
Cyc_Tcutil_check_no_qual(loc,t);
# 1229
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1234
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1241
Cyc_Tcutil_unify(t2,t);{
void*_tmp1FF=({struct Cyc_String_pa_PrintArg_struct _tmp202=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0U,({
struct _dyneither_ptr _tmp6BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp52B.f1=_tmp6BF;});_tmp52B;});struct Cyc_String_pa_PrintArg_struct _tmp203=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0U,({struct _dyneither_ptr _tmp6C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp52A.f1=_tmp6C0;});_tmp52A;});void*_tmp200[2U];_tmp200[0]=& _tmp202,_tmp200[1]=& _tmp203;({struct Cyc_Tcenv_Tenv*_tmp6C3=te;unsigned int _tmp6C2=loc;struct _dyneither_ptr _tmp6C1=({const char*_tmp201="cannot cast %s to %s";_tag_dyneither(_tmp201,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6C3,_tmp6C2,& t,_tmp6C1,_tag_dyneither(_tmp200,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return _tmp1FF;};}}}
# 1251
{struct _tuple0 _tmp204=({struct _tuple0 _tmp52C;_tmp52C.f1=e->r,({void*_tmp6C4=Cyc_Tcutil_compress(t);_tmp52C.f2=_tmp6C4;});_tmp52C;});struct _tuple0 _tmp205=_tmp204;int _tmp20E;union Cyc_Absyn_Constraint*_tmp20D;union Cyc_Absyn_Constraint*_tmp20C;union Cyc_Absyn_Constraint*_tmp20B;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp205.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205.f2)->tag == 5U){_LL1: _tmp20E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp205.f1)->f1).fat_result;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205.f2)->f1).ptr_atts).nullable;_tmp20C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205.f2)->f1).ptr_atts).bounds;_tmp20B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205.f2)->f1).ptr_atts).zero_term;_LL2:
# 1255
 if((_tmp20E  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp20B)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp20D)){
void*_tmp206=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp20C);void*_tmp207=_tmp206;struct Cyc_Absyn_Exp*_tmp20A;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp207)->tag == 1U){_LL6: _tmp20A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp207)->f1;_LL7:
# 1258
 if((Cyc_Evexp_eval_const_uint_exp(_tmp20A)).f1 == 1)
({void*_tmp208=0U;({unsigned int _tmp6C6=loc;struct _dyneither_ptr _tmp6C5=({const char*_tmp209="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp209,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6C6,_tmp6C5,_tag_dyneither(_tmp208,sizeof(void*),0U));});});
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1264
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1267
return t;};}
# 1271
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp20F=0;
struct Cyc_Absyn_Tqual _tmp210=Cyc_Absyn_empty_tqual(0U);
int _tmp211=0;
if(topt != 0){
void*_tmp212=Cyc_Tcutil_compress(*topt);void*_tmp213=_tmp212;void**_tmp216;struct Cyc_Absyn_Tqual _tmp215;union Cyc_Absyn_Constraint*_tmp214;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213)->tag == 5U){_LL1: _tmp216=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213)->f1).elt_typ;_tmp215=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213)->f1).elt_tq;_tmp214=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213)->f1).ptr_atts).nullable;_LL2:
# 1278
 _tmp20F=_tmp216;
_tmp210=_tmp215;
({int _tmp6C7=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp214);_tmp211=_tmp6C7;});
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1292
({struct Cyc_Tcenv_Tenv*_tmp6C9=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp6C8=_tmp20F;Cyc_Tcexp_tcExpNoInst(_tmp6C9,_tmp6C8,e);});
# 1295
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp217=0U;({unsigned int _tmp6CB=e->loc;struct _dyneither_ptr _tmp6CA=({const char*_tmp218="Cannot take the address of an alias-free path";_tag_dyneither(_tmp218,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6CB,_tmp6CA,_tag_dyneither(_tmp217,sizeof(void*),0U));});});
# 1301
{void*_tmp219=e->r;void*_tmp21A=_tmp219;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21D;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21A)->tag == 23U){_LL6: _tmp21E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21A)->f1;_tmp21D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21A)->f2;_LL7:
# 1303
{void*_tmp21B=Cyc_Tcutil_compress((void*)_check_null(_tmp21E->topt));void*_tmp21C=_tmp21B;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp21C)->tag == 10U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1308
({void*_tmp6CC=(Cyc_Absyn_add_exp(_tmp21E,_tmp21D,0U))->r;e0->r=_tmp6CC;});
return Cyc_Tcexp_tcPlus(te,_tmp21E,_tmp21D);}_LLA:;}
# 1311
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1316
{void*_tmp21F=e->r;void*_tmp220=_tmp21F;switch(*((int*)_tmp220)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp220)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp220)->f3 == 1){_LL12: _LL13:
# 1319
({void*_tmp221=0U;({unsigned int _tmp6CE=e->loc;struct _dyneither_ptr _tmp6CD=({const char*_tmp222="cannot take the address of a @tagged union member";_tag_dyneither(_tmp222,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6CE,_tmp6CD,_tag_dyneither(_tmp221,sizeof(void*),0U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1325
struct _tuple14 _tmp223=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp224=_tmp223;int _tmp228;void*_tmp227;_LL17: _tmp228=_tmp224.f1;_tmp227=_tmp224.f2;_LL18:;
# 1327
if(Cyc_Tcutil_is_noalias_region(_tmp227,0))
({void*_tmp225=0U;({unsigned int _tmp6D0=e->loc;struct _dyneither_ptr _tmp6CF=({const char*_tmp226="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp226,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp6D0,_tmp6CF,_tag_dyneither(_tmp225,sizeof(void*),0U));});});{
# 1330
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp228){
tq.print_const=1;
tq.real_const=1;}{
# 1336
void*t=_tmp211?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp227,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp227,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1343
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1347
return Cyc_Absyn_uint_typ;
# 1349
({unsigned int _tmp6D3=loc;struct Cyc_Tcenv_Tenv*_tmp6D2=te;struct Cyc_List_List*_tmp6D1=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6D3,_tmp6D2,_tmp6D1,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp22B=({struct Cyc_String_pa_PrintArg_struct _tmp52D;_tmp52D.tag=0U,({
struct _dyneither_ptr _tmp6D4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp52D.f1=_tmp6D4;});_tmp52D;});void*_tmp229[1U];_tmp229[0]=& _tmp22B;({unsigned int _tmp6D6=loc;struct _dyneither_ptr _tmp6D5=({const char*_tmp22A="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp22A,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp6D6,_tmp6D5,_tag_dyneither(_tmp229,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp22C=Cyc_Tcutil_compress(*topt);void*_tmp22D=_tmp22C;void*_tmp232;void*_tmp231;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp22D)->tag == 19U){_LL1: _tmp232=_tmp22D;_tmp231=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp22D)->f1;_LL2: {
# 1358
struct Cyc_Absyn_Exp*_tmp22E=Cyc_Absyn_sizeoftyp_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp22F=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp230=_cycalloc(sizeof(*_tmp230));_tmp230->tag=18U,_tmp230->f1=_tmp22E;_tmp230;});
if(Cyc_Tcutil_unify(_tmp231,(void*)_tmp22F))return _tmp232;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1364
return Cyc_Absyn_uint_typ;}
# 1367
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1374
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1376
({unsigned int _tmp6D9=loc;struct Cyc_Tcenv_Tenv*_tmp6D8=te;struct Cyc_List_List*_tmp6D7=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6D9,_tmp6D8,_tmp6D7,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp233=fs;
void*_tmp234=t_orig;
for(0;_tmp233 != 0;_tmp233=_tmp233->tl){
void*_tmp235=(void*)_tmp233->hd;
void*_tmp236=_tmp235;unsigned int _tmp270;struct _dyneither_ptr*_tmp26F;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp236)->tag == 0U){_LL1: _tmp26F=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp236)->f1;_LL2: {
# 1384
int bad_type=1;
{void*_tmp237=Cyc_Tcutil_compress(_tmp234);void*_tmp238=_tmp237;struct Cyc_List_List*_tmp240;struct Cyc_Absyn_Aggrdecl**_tmp23F;switch(*((int*)_tmp238)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp238)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp23F=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp238)->f1).aggr_info).KnownAggr).val;_LL7:
# 1387
 if((*_tmp23F)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp26F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp23F)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp23B=({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0U,_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp26F);_tmp52E;});void*_tmp239[1U];_tmp239[0]=& _tmp23B;({unsigned int _tmp6DB=loc;struct _dyneither_ptr _tmp6DA=({const char*_tmp23A="no field of struct/union has name %s";_tag_dyneither(_tmp23A,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6DB,_tmp6DA,_tag_dyneither(_tmp239,sizeof(void*),1U));});});else{
# 1392
_tmp234=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}case 12U: _LL8: _tmp240=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp238)->f2;_LL9: {
# 1396
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp26F,_tmp240);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp23E=({struct Cyc_String_pa_PrintArg_struct _tmp52F;_tmp52F.tag=0U,_tmp52F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp26F);_tmp52F;});void*_tmp23C[1U];_tmp23C[0]=& _tmp23E;({unsigned int _tmp6DD=loc;struct _dyneither_ptr _tmp6DC=({const char*_tmp23D="no field of struct/union has name %s";_tag_dyneither(_tmp23D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6DD,_tmp6DC,_tag_dyneither(_tmp23C,sizeof(void*),1U));});});else{
# 1400
_tmp234=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1405
if(bad_type){
if(_tmp233 == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp243=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,({struct _dyneither_ptr _tmp6DE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp234));_tmp530.f1=_tmp6DE;});_tmp530;});void*_tmp241[1U];_tmp241[0]=& _tmp243;({unsigned int _tmp6E0=loc;struct _dyneither_ptr _tmp6DF=({const char*_tmp242="%s is not a known struct/union type";_tag_dyneither(_tmp242,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6E0,_tmp6DF,_tag_dyneither(_tmp241,sizeof(void*),1U));});});else{
# 1409
struct _dyneither_ptr _tmp244=({struct Cyc_String_pa_PrintArg_struct _tmp257=({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0U,({struct _dyneither_ptr _tmp6E1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp537.f1=_tmp6E1;});_tmp537;});void*_tmp255[1U];_tmp255[0]=& _tmp257;({struct _dyneither_ptr _tmp6E2=({const char*_tmp256="(%s)";_tag_dyneither(_tmp256,sizeof(char),5U);});Cyc_aprintf(_tmp6E2,_tag_dyneither(_tmp255,sizeof(void*),1U));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp233;x=x->tl){
void*_tmp245=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp246=_tmp245;unsigned int _tmp250;struct _dyneither_ptr*_tmp24F;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp246)->tag == 0U){_LLD: _tmp24F=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp246)->f1;_LLE:
# 1414
({struct _dyneither_ptr _tmp6E4=({struct Cyc_String_pa_PrintArg_struct _tmp249=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,_tmp532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp244);_tmp532;});struct Cyc_String_pa_PrintArg_struct _tmp24A=({struct Cyc_String_pa_PrintArg_struct _tmp531;_tmp531.tag=0U,_tmp531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp24F);_tmp531;});void*_tmp247[2U];_tmp247[0]=& _tmp249,_tmp247[1]=& _tmp24A;({struct _dyneither_ptr _tmp6E3=({const char*_tmp248="%s.%s";_tag_dyneither(_tmp248,sizeof(char),6U);});Cyc_aprintf(_tmp6E3,_tag_dyneither(_tmp247,sizeof(void*),2U));});});_tmp244=_tmp6E4;});goto _LLC;}else{_LLF: _tmp250=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp246)->f1;_LL10:
# 1416
({struct _dyneither_ptr _tmp6E6=({struct Cyc_String_pa_PrintArg_struct _tmp24D=({struct Cyc_String_pa_PrintArg_struct _tmp534;_tmp534.tag=0U,_tmp534.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp244);_tmp534;});struct Cyc_Int_pa_PrintArg_struct _tmp24E=({struct Cyc_Int_pa_PrintArg_struct _tmp533;_tmp533.tag=1U,_tmp533.f1=(unsigned long)((int)_tmp250);_tmp533;});void*_tmp24B[2U];_tmp24B[0]=& _tmp24D,_tmp24B[1]=& _tmp24E;({struct _dyneither_ptr _tmp6E5=({const char*_tmp24C="%s.%d";_tag_dyneither(_tmp24C,sizeof(char),6U);});Cyc_aprintf(_tmp6E5,_tag_dyneither(_tmp24B,sizeof(void*),2U));});});_tmp244=_tmp6E6;});goto _LLC;}_LLC:;}
# 1418
({struct Cyc_String_pa_PrintArg_struct _tmp253=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,_tmp536.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp244);_tmp536;});struct Cyc_String_pa_PrintArg_struct _tmp254=({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U,({struct _dyneither_ptr _tmp6E7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp234));_tmp535.f1=_tmp6E7;});_tmp535;});void*_tmp251[2U];_tmp251[0]=& _tmp253,_tmp251[1]=& _tmp254;({unsigned int _tmp6E9=loc;struct _dyneither_ptr _tmp6E8=({const char*_tmp252="%s == %s is not a struct/union type";_tag_dyneither(_tmp252,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6E9,_tmp6E8,_tag_dyneither(_tmp251,sizeof(void*),2U));});});}}
# 1421
goto _LL0;}}else{_LL3: _tmp270=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp236)->f1;_LL4: {
# 1423
int bad_type=1;
{void*_tmp258=Cyc_Tcutil_compress(_tmp234);void*_tmp259=_tmp258;struct Cyc_Absyn_Datatypefield*_tmp26B;struct Cyc_List_List*_tmp26A;struct Cyc_List_List*_tmp269;struct Cyc_Absyn_Aggrdecl**_tmp268;switch(*((int*)_tmp259)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp259)->f1).aggr_info).KnownAggr).tag == 2){_LL12: _tmp268=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp259)->f1).aggr_info).KnownAggr).val;_LL13:
# 1426
 if((*_tmp268)->impl == 0)
goto _LL11;
_tmp269=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp268)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 12U: _LL14: _tmp269=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp259)->f2;_LL15:
# 1430
 if(({int _tmp6EA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp269);_tmp6EA <= _tmp270;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp25C=({struct Cyc_Int_pa_PrintArg_struct _tmp539;_tmp539.tag=1U,({
unsigned long _tmp6EB=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp269);_tmp539.f1=_tmp6EB;});_tmp539;});struct Cyc_Int_pa_PrintArg_struct _tmp25D=({struct Cyc_Int_pa_PrintArg_struct _tmp538;_tmp538.tag=1U,_tmp538.f1=(unsigned long)((int)_tmp270);_tmp538;});void*_tmp25A[2U];_tmp25A[0]=& _tmp25C,_tmp25A[1]=& _tmp25D;({unsigned int _tmp6ED=loc;struct _dyneither_ptr _tmp6EC=({const char*_tmp25B="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp25B,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6ED,_tmp6EC,_tag_dyneither(_tmp25A,sizeof(void*),2U));});});else{
# 1434
({void*_tmp6EE=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp269,(int)_tmp270))->type;_tmp234=_tmp6EE;});}
bad_type=0;
goto _LL11;case 10U: _LL16: _tmp26A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp259)->f1;_LL17:
# 1438
 if(({int _tmp6EF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26A);_tmp6EF <= _tmp270;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp260=({struct Cyc_Int_pa_PrintArg_struct _tmp53B;_tmp53B.tag=1U,({
unsigned long _tmp6F0=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26A);_tmp53B.f1=_tmp6F0;});_tmp53B;});struct Cyc_Int_pa_PrintArg_struct _tmp261=({struct Cyc_Int_pa_PrintArg_struct _tmp53A;_tmp53A.tag=1U,_tmp53A.f1=(unsigned long)((int)_tmp270);_tmp53A;});void*_tmp25E[2U];_tmp25E[0]=& _tmp260,_tmp25E[1]=& _tmp261;({unsigned int _tmp6F2=loc;struct _dyneither_ptr _tmp6F1=({const char*_tmp25F="tuple has too few components: %d <= %d";_tag_dyneither(_tmp25F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6F2,_tmp6F1,_tag_dyneither(_tmp25E,sizeof(void*),2U));});});else{
# 1442
({void*_tmp6F3=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp26A,(int)_tmp270)).f2;_tmp234=_tmp6F3;});}
bad_type=0;
goto _LL11;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp259)->f1).field_info).KnownDatatypefield).tag == 2){_LL18: _tmp26B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp259)->f1).field_info).KnownDatatypefield).val).f2;_LL19:
# 1446
 if(({int _tmp6F4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26B->typs);_tmp6F4 < _tmp270;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp264=({struct Cyc_Int_pa_PrintArg_struct _tmp53D;_tmp53D.tag=1U,({
unsigned long _tmp6F5=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26B->typs);_tmp53D.f1=_tmp6F5;});_tmp53D;});struct Cyc_Int_pa_PrintArg_struct _tmp265=({struct Cyc_Int_pa_PrintArg_struct _tmp53C;_tmp53C.tag=1U,_tmp53C.f1=(unsigned long)((int)_tmp270);_tmp53C;});void*_tmp262[2U];_tmp262[0]=& _tmp264,_tmp262[1]=& _tmp265;({unsigned int _tmp6F7=loc;struct _dyneither_ptr _tmp6F6=({const char*_tmp263="datatype field has too few components: %d < %d";_tag_dyneither(_tmp263,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6F7,_tmp6F6,_tag_dyneither(_tmp262,sizeof(void*),2U));});});else{
# 1450
if(_tmp270 != 0)
({void*_tmp6F8=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp26B->typs,(int)(_tmp270 - 1))).f2;_tmp234=_tmp6F8;});else{
if(_tmp233->tl != 0)
({void*_tmp266=0U;({unsigned int _tmp6FA=loc;struct _dyneither_ptr _tmp6F9=({const char*_tmp267="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp267,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6FA,_tmp6F9,_tag_dyneither(_tmp266,sizeof(void*),0U));});});}}
# 1455
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: _LL1A: _LL1B:
 goto _LL11;}_LL11:;}
# 1459
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp26E=({struct Cyc_String_pa_PrintArg_struct _tmp53E;_tmp53E.tag=0U,({struct _dyneither_ptr _tmp6FB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp234));_tmp53E.f1=_tmp6FB;});_tmp53E;});void*_tmp26C[1U];_tmp26C[0]=& _tmp26E;({unsigned int _tmp6FD=loc;struct _dyneither_ptr _tmp6FC=({const char*_tmp26D="%s is not a known type";_tag_dyneither(_tmp26D,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6FD,_tmp6FC,_tag_dyneither(_tmp26C,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1464
return Cyc_Absyn_uint_typ;};}
# 1468
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp271=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp271,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp272=t;void*_tmp289;void*_tmp288;union Cyc_Absyn_Constraint*_tmp287;union Cyc_Absyn_Constraint*_tmp286;switch(*((int*)_tmp272)){case 1U: _LL1: _LL2: {
# 1474
struct Cyc_List_List*_tmp273=Cyc_Tcenv_lookup_type_vars(_tmp271);
void*_tmp274=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->v=_tmp273;_tmp27C;}));
void*_tmp275=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->v=_tmp273;_tmp27B;}));
union Cyc_Absyn_Constraint*_tmp276=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp277=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp278=({struct Cyc_Absyn_PtrAtts _tmp53F;_tmp53F.rgn=_tmp275,({union Cyc_Absyn_Constraint*_tmp6FE=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp53F.nullable=_tmp6FE;}),_tmp53F.bounds=_tmp276,_tmp53F.zero_term=_tmp277,_tmp53F.ptrloc=0;_tmp53F;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp279=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A->tag=5U,(_tmp27A->f1).elt_typ=_tmp274,({struct Cyc_Absyn_Tqual _tmp6FF=Cyc_Absyn_empty_tqual(0U);(_tmp27A->f1).elt_tq=_tmp6FF;}),(_tmp27A->f1).ptr_atts=_tmp278;_tmp27A;});
Cyc_Tcutil_unify(t,(void*)_tmp279);
_tmp289=_tmp274;_tmp288=_tmp275;_tmp287=_tmp276;_tmp286=_tmp277;goto _LL4;}case 5U: _LL3: _tmp289=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp272)->f1).elt_typ;_tmp288=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp272)->f1).ptr_atts).rgn;_tmp287=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp272)->f1).ptr_atts).bounds;_tmp286=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp272)->f1).ptr_atts).zero_term;_LL4:
# 1484
 Cyc_Tcenv_check_rgn_accessible(_tmp271,loc,_tmp288);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp287);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp289),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp271)){
void*_tmp27D=Cyc_Tcutil_compress(_tmp289);void*_tmp27E=_tmp27D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp27E)->tag == 9U){_LL8: _LL9:
# 1489
 if(Cyc_Tc_aggressive_warn)
({void*_tmp27F=0U;({unsigned int _tmp701=loc;struct _dyneither_ptr _tmp700=({const char*_tmp280="unnecessary dereference for function type";_tag_dyneither(_tmp280,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp701,_tmp700,_tag_dyneither(_tmp27F,sizeof(void*),0U));});});
return t;}else{_LLA: _LLB:
# 1493
({void*_tmp281=0U;({unsigned int _tmp703=loc;struct _dyneither_ptr _tmp702=({const char*_tmp282="can't dereference abstract pointer type";_tag_dyneither(_tmp282,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp703,_tmp702,_tag_dyneither(_tmp281,sizeof(void*),0U));});});}_LL7:;}
# 1496
return _tmp289;default: _LL5: _LL6:
# 1498
 return({struct Cyc_String_pa_PrintArg_struct _tmp285=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,({struct _dyneither_ptr _tmp704=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp540.f1=_tmp704;});_tmp540;});void*_tmp283[1U];_tmp283[0]=& _tmp285;({struct Cyc_Tcenv_Tenv*_tmp708=_tmp271;unsigned int _tmp707=loc;void**_tmp706=topt;struct _dyneither_ptr _tmp705=({const char*_tmp284="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp284,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp708,_tmp707,_tmp706,_tmp705,_tag_dyneither(_tmp283,sizeof(void*),1U));});});}_LL0:;};}
# 1503
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1507
({struct Cyc_Tcenv_Tenv*_tmp709=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp709,0,e);});
# 1509
({int _tmp70A=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp70A;});{
void*_tmp28A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp28B=_tmp28A;enum Cyc_Absyn_AggrKind _tmp2A5;struct Cyc_List_List*_tmp2A4;struct Cyc_Absyn_Aggrdecl*_tmp2A3;struct Cyc_List_List*_tmp2A2;switch(*((int*)_tmp28B)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp28B)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp2A3=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp28B)->f1).aggr_info).KnownAggr).val;_tmp2A2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp28B)->f1).targs;_LL2: {
# 1512
struct Cyc_Absyn_Aggrfield*_tmp28C=Cyc_Absyn_lookup_decl_field(_tmp2A3,f);
if(_tmp28C == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp28F=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,({
struct _dyneither_ptr _tmp70B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2A3->name));_tmp542.f1=_tmp70B;});_tmp542;});struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,_tmp541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp541;});void*_tmp28D[2U];_tmp28D[0]=& _tmp28F,_tmp28D[1]=& _tmp290;({struct Cyc_Tcenv_Tenv*_tmp70F=te;unsigned int _tmp70E=loc;void**_tmp70D=topt;struct _dyneither_ptr _tmp70C=({const char*_tmp28E="%s has no %s member";_tag_dyneither(_tmp28E,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp70F,_tmp70E,_tmp70D,_tmp70C,_tag_dyneither(_tmp28D,sizeof(void*),2U));});});
# 1517
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A3->impl))->tagged)*is_tagged=1;{
void*t2=_tmp28C->type;
if(_tmp2A2 != 0){
struct Cyc_List_List*_tmp291=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2A3->tvs,_tmp2A2);
({void*_tmp710=Cyc_Tcutil_substitute(_tmp291,_tmp28C->type);t2=_tmp710;});}
# 1525
if((((_tmp2A3->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A3->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp28C->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp294=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,_tmp543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp543;});void*_tmp292[1U];_tmp292[0]=& _tmp294;({struct Cyc_Tcenv_Tenv*_tmp714=te;unsigned int _tmp713=loc;void**_tmp712=topt;struct _dyneither_ptr _tmp711=({const char*_tmp293="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp293,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp714,_tmp713,_tmp712,_tmp711,_tag_dyneither(_tmp292,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A3->impl))->exist_vars != 0){
# 1530
if(!({void*_tmp715=t2;Cyc_Tcutil_unify(_tmp715,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp297=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp544;});void*_tmp295[1U];_tmp295[0]=& _tmp297;({struct Cyc_Tcenv_Tenv*_tmp719=te;unsigned int _tmp718=loc;void**_tmp717=topt;struct _dyneither_ptr _tmp716=({const char*_tmp296="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp296,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp719,_tmp718,_tmp717,_tmp716,_tag_dyneither(_tmp295,sizeof(void*),1U));});});}
# 1533
return t2;};}}else{goto _LL5;}case 12U: _LL3: _tmp2A5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp28B)->f1;_tmp2A4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp28B)->f2;_LL4: {
# 1535
struct Cyc_Absyn_Aggrfield*_tmp298=Cyc_Absyn_lookup_field(_tmp2A4,f);
if(_tmp298 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp29B=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,_tmp545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp545;});void*_tmp299[1U];_tmp299[0]=& _tmp29B;({struct Cyc_Tcenv_Tenv*_tmp71D=te;unsigned int _tmp71C=loc;void**_tmp71B=topt;struct _dyneither_ptr _tmp71A=({const char*_tmp29A="type has no %s member";_tag_dyneither(_tmp29A,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp71D,_tmp71C,_tmp71B,_tmp71A,_tag_dyneither(_tmp299,sizeof(void*),1U));});});
# 1540
if(((_tmp2A5 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp298->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp298->requires_clause == 0)
# 1542
return({struct Cyc_String_pa_PrintArg_struct _tmp29E=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,_tmp546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp546;});void*_tmp29C[1U];_tmp29C[0]=& _tmp29E;({struct Cyc_Tcenv_Tenv*_tmp721=te;unsigned int _tmp720=loc;void**_tmp71F=topt;struct _dyneither_ptr _tmp71E=({const char*_tmp29D="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp29D,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp721,_tmp720,_tmp71F,_tmp71E,_tag_dyneither(_tmp29C,sizeof(void*),1U));});});
return _tmp298->type;}default: _LL5: _LL6:
# 1545
 return({struct Cyc_String_pa_PrintArg_struct _tmp2A1=({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0U,({
struct _dyneither_ptr _tmp722=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp547.f1=_tmp722;});_tmp547;});void*_tmp29F[1U];_tmp29F[0]=& _tmp2A1;({struct Cyc_Tcenv_Tenv*_tmp726=te;unsigned int _tmp725=loc;void**_tmp724=topt;struct _dyneither_ptr _tmp723=({const char*_tmp2A0="expecting struct or union, found %s";_tag_dyneither(_tmp2A0,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp726,_tmp725,_tmp724,_tmp723,_tag_dyneither(_tmp29F,sizeof(void*),1U));});});}_LL0:;};}
# 1551
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1554
({struct Cyc_Tcenv_Tenv*_tmp727=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp727,0,e);});
# 1556
({int _tmp728=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp728;});
{void*_tmp2A6=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A7=_tmp2A6;void*_tmp2CA;void*_tmp2C9;union Cyc_Absyn_Constraint*_tmp2C8;union Cyc_Absyn_Constraint*_tmp2C7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->tag == 5U){_LL1: _tmp2CA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->f1).elt_typ;_tmp2C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->f1).ptr_atts).rgn;_tmp2C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->f1).ptr_atts).bounds;_tmp2C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->f1).ptr_atts).zero_term;_LL2:
# 1559
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2C8);
{void*_tmp2A8=Cyc_Tcutil_compress(_tmp2CA);void*_tmp2A9=_tmp2A8;enum Cyc_Absyn_AggrKind _tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_Absyn_Aggrdecl*_tmp2C4;struct Cyc_List_List*_tmp2C3;switch(*((int*)_tmp2A9)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A9)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp2C4=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A9)->f1).aggr_info).KnownAggr).val;_tmp2C3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A9)->f1).targs;_LL7: {
# 1562
struct Cyc_Absyn_Aggrfield*_tmp2AA=Cyc_Absyn_lookup_decl_field(_tmp2C4,f);
if(_tmp2AA == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AD=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,({
struct _dyneither_ptr _tmp729=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C4->name));_tmp549.f1=_tmp729;});_tmp549;});struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,_tmp548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp548;});void*_tmp2AB[2U];_tmp2AB[0]=& _tmp2AD,_tmp2AB[1]=& _tmp2AE;({struct Cyc_Tcenv_Tenv*_tmp72D=te;unsigned int _tmp72C=loc;void**_tmp72B=topt;struct _dyneither_ptr _tmp72A=({const char*_tmp2AC="type %s has no %s member";_tag_dyneither(_tmp2AC,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp72D,_tmp72C,_tmp72B,_tmp72A,_tag_dyneither(_tmp2AB,sizeof(void*),2U));});});
# 1567
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C4->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2AA->type;
if(_tmp2C3 != 0){
struct Cyc_List_List*_tmp2AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2C4->tvs,_tmp2C3);
({void*_tmp72E=Cyc_Tcutil_substitute(_tmp2AF,_tmp2AA->type);t3=_tmp72E;});}{
# 1573
struct Cyc_Absyn_Kind*_tmp2B0=Cyc_Tcutil_typ_kind(t3);
# 1576
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2B0) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2B1=Cyc_Tcutil_compress(t3);void*_tmp2B2=_tmp2B1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B2)->tag == 8U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1580
 return({struct Cyc_String_pa_PrintArg_struct _tmp2B5=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,_tmp54A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54A;});void*_tmp2B3[1U];_tmp2B3[0]=& _tmp2B5;({struct Cyc_Tcenv_Tenv*_tmp732=te;unsigned int _tmp731=loc;void**_tmp730=topt;struct _dyneither_ptr _tmp72F=({const char*_tmp2B4="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2B4,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp732,_tmp731,_tmp730,_tmp72F,_tag_dyneither(_tmp2B3,sizeof(void*),1U));});});}_LLC:;}
# 1585
if((((_tmp2C4->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C4->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2AA->requires_clause == 0)
# 1588
return({struct Cyc_String_pa_PrintArg_struct _tmp2B8=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,_tmp54B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54B;});void*_tmp2B6[1U];_tmp2B6[0]=& _tmp2B8;({struct Cyc_Tcenv_Tenv*_tmp736=te;unsigned int _tmp735=loc;void**_tmp734=topt;struct _dyneither_ptr _tmp733=({const char*_tmp2B7="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B7,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp736,_tmp735,_tmp734,_tmp733,_tag_dyneither(_tmp2B6,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C4->impl))->exist_vars != 0){
if(!({void*_tmp737=t3;Cyc_Tcutil_unify(_tmp737,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2BB=({struct Cyc_String_pa_PrintArg_struct _tmp54C;_tmp54C.tag=0U,_tmp54C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54C;});void*_tmp2B9[1U];_tmp2B9[0]=& _tmp2BB;({struct Cyc_Tcenv_Tenv*_tmp73B=te;unsigned int _tmp73A=loc;void**_tmp739=topt;struct _dyneither_ptr _tmp738=({const char*_tmp2BA="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2BA,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp73B,_tmp73A,_tmp739,_tmp738,_tag_dyneither(_tmp2B9,sizeof(void*),1U));});});}
# 1595
return t3;};};}}else{goto _LLA;}case 12U: _LL8: _tmp2C6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A9)->f1;_tmp2C5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A9)->f2;_LL9: {
# 1597
struct Cyc_Absyn_Aggrfield*_tmp2BC=Cyc_Absyn_lookup_field(_tmp2C5,f);
if(_tmp2BC == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp54D;_tmp54D.tag=0U,_tmp54D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54D;});void*_tmp2BD[1U];_tmp2BD[0]=& _tmp2BF;({struct Cyc_Tcenv_Tenv*_tmp73F=te;unsigned int _tmp73E=loc;void**_tmp73D=topt;struct _dyneither_ptr _tmp73C=({const char*_tmp2BE="type has no %s field";_tag_dyneither(_tmp2BE,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp73F,_tmp73E,_tmp73D,_tmp73C,_tag_dyneither(_tmp2BD,sizeof(void*),1U));});});
# 1602
if((_tmp2C6 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2BC->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2C2=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,_tmp54E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54E;});void*_tmp2C0[1U];_tmp2C0[0]=& _tmp2C2;({struct Cyc_Tcenv_Tenv*_tmp743=te;unsigned int _tmp742=loc;void**_tmp741=topt;struct _dyneither_ptr _tmp740=({const char*_tmp2C1="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2C1,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp743,_tmp742,_tmp741,_tmp740,_tag_dyneither(_tmp2C0,sizeof(void*),1U));});});
return _tmp2BC->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1607
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1610
return({struct Cyc_String_pa_PrintArg_struct _tmp2CD=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({
struct _dyneither_ptr _tmp744=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp54F.f1=_tmp744;});_tmp54F;});void*_tmp2CB[1U];_tmp2CB[0]=& _tmp2CD;({struct Cyc_Tcenv_Tenv*_tmp748=te;unsigned int _tmp747=loc;void**_tmp746=topt;struct _dyneither_ptr _tmp745=({const char*_tmp2CC="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2CC,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp748,_tmp747,_tmp746,_tmp745,_tag_dyneither(_tmp2CB,sizeof(void*),1U));});});}
# 1615
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1617
struct _tuple15 _tmp2CE=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2CF=_tmp2CE;unsigned int _tmp2DD;int _tmp2DC;_LL1: _tmp2DD=_tmp2CF.f1;_tmp2DC=_tmp2CF.f2;_LL2:;
if(!_tmp2DC)
return({void*_tmp2D0=0U;({struct Cyc_Tcenv_Tenv*_tmp74B=te;unsigned int _tmp74A=loc;struct _dyneither_ptr _tmp749=({const char*_tmp2D1="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2D1,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp74B,_tmp74A,0,_tmp749,_tag_dyneither(_tmp2D0,sizeof(void*),0U));});});{
# 1621
struct _handler_cons _tmp2D2;_push_handler(& _tmp2D2);{int _tmp2D4=0;if(setjmp(_tmp2D2.handler))_tmp2D4=1;if(!_tmp2D4){
{void*_tmp2D5=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2DD)).f2;_npop_handler(0U);return _tmp2D5;};_pop_handler();}else{void*_tmp2D3=(void*)_exn_thrown;void*_tmp2D6=_tmp2D3;void*_tmp2DB;if(((struct Cyc_List_Nth_exn_struct*)_tmp2D6)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1624
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2D9=({struct Cyc_Int_pa_PrintArg_struct _tmp551;_tmp551.tag=1U,_tmp551.f1=(unsigned long)((int)_tmp2DD);_tmp551;});struct Cyc_Int_pa_PrintArg_struct _tmp2DA=({struct Cyc_Int_pa_PrintArg_struct _tmp550;_tmp550.tag=1U,({
unsigned long _tmp74C=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp550.f1=_tmp74C;});_tmp550;});void*_tmp2D7[2U];_tmp2D7[0]=& _tmp2D9,_tmp2D7[1]=& _tmp2DA;({struct Cyc_Tcenv_Tenv*_tmp74F=te;unsigned int _tmp74E=loc;struct _dyneither_ptr _tmp74D=({const char*_tmp2D8="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2D8,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp74F,_tmp74E,0,_tmp74D,_tag_dyneither(_tmp2D7,sizeof(void*),2U));});});}else{_LL6: _tmp2DB=_tmp2D6;_LL7:(int)_rethrow(_tmp2DB);}_LL3:;}};};}
# 1629
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1631
struct Cyc_Tcenv_Tenv*_tmp2DE=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp750=Cyc_Tcenv_clear_abstract_val_ok(_tmp2DE);Cyc_Tcexp_tcExp(_tmp750,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp751=Cyc_Tcenv_clear_abstract_val_ok(_tmp2DE);Cyc_Tcexp_tcExp(_tmp751,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2DE,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2E1=({struct Cyc_String_pa_PrintArg_struct _tmp552;_tmp552.tag=0U,({
struct _dyneither_ptr _tmp752=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp552.f1=_tmp752;});_tmp552;});void*_tmp2DF[1U];_tmp2DF[0]=& _tmp2E1;({struct Cyc_Tcenv_Tenv*_tmp756=_tmp2DE;unsigned int _tmp755=e2->loc;void**_tmp754=topt;struct _dyneither_ptr _tmp753=({const char*_tmp2E0="expecting int subscript, found %s";_tag_dyneither(_tmp2E0,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp756,_tmp755,_tmp754,_tmp753,_tag_dyneither(_tmp2DF,sizeof(void*),1U));});});{
# 1641
void*_tmp2E2=t1;struct Cyc_List_List*_tmp306;void*_tmp305;struct Cyc_Absyn_Tqual _tmp304;void*_tmp303;union Cyc_Absyn_Constraint*_tmp302;union Cyc_Absyn_Constraint*_tmp301;switch(*((int*)_tmp2E2)){case 5U: _LL1: _tmp305=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E2)->f1).elt_typ;_tmp304=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E2)->f1).elt_tq;_tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E2)->f1).ptr_atts).rgn;_tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E2)->f1).ptr_atts).bounds;_tmp301=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E2)->f1).ptr_atts).zero_term;_LL2:
# 1645
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp301)){
int emit_warning=0;
{void*_tmp2E3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp302);void*_tmp2E4=_tmp2E3;struct Cyc_Absyn_Exp*_tmp2F3;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2E4)->tag == 1U){_LL8: _tmp2F3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2E4)->f1;_LL9: {
# 1649
struct _tuple15 _tmp2E5=Cyc_Evexp_eval_const_uint_exp(_tmp2F3);struct _tuple15 _tmp2E6=_tmp2E5;unsigned int _tmp2F2;int _tmp2F1;_LLD: _tmp2F2=_tmp2E6.f1;_tmp2F1=_tmp2E6.f2;_LLE:;
if(_tmp2F1  && _tmp2F2 == 1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2E7=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2E8=_tmp2E7;unsigned int _tmp2F0;int _tmp2EF;_LL10: _tmp2F0=_tmp2E8.f1;_tmp2EF=_tmp2E8.f2;_LL11:;
if(_tmp2EF){
struct _tuple15 _tmp2E9=Cyc_Evexp_eval_const_uint_exp(_tmp2F3);struct _tuple15 _tmp2EA=_tmp2E9;unsigned int _tmp2EE;int _tmp2ED;_LL13: _tmp2EE=_tmp2EA.f1;_tmp2ED=_tmp2EA.f2;_LL14:;
if((_tmp2F1  && _tmp2EE > _tmp2F2) && _tmp2F2 != 1)
({void*_tmp2EB=0U;({unsigned int _tmp758=loc;struct _dyneither_ptr _tmp757=({const char*_tmp2EC="subscript will fail at run-time";_tag_dyneither(_tmp2EC,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp758,_tmp757,_tag_dyneither(_tmp2EB,sizeof(void*),0U));});});}}
# 1659
goto _LL7;}}else{_LLA: _LLB:
 emit_warning=0;goto _LL7;}_LL7:;}
# 1662
if(emit_warning)
({void*_tmp2F4=0U;({unsigned int _tmp75A=e2->loc;struct _dyneither_ptr _tmp759=({const char*_tmp2F5="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp2F5,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp75A,_tmp759,_tag_dyneither(_tmp2F4,sizeof(void*),0U));});});}else{
# 1666
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2F6=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2F7=_tmp2F6;unsigned int _tmp2F9;int _tmp2F8;_LL16: _tmp2F9=_tmp2F7.f1;_tmp2F8=_tmp2F7.f2;_LL17:;
if(_tmp2F8)
# 1671
({unsigned int _tmp75D=loc;unsigned int _tmp75C=_tmp2F9;union Cyc_Absyn_Constraint*_tmp75B=_tmp302;Cyc_Tcutil_check_bound(_tmp75D,_tmp75C,_tmp75B,Cyc_Tcenv_abstract_val_ok(_tmp2DE));});}else{
# 1674
if(Cyc_Tcutil_is_bound_one(_tmp302) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp301))
({void*_tmp2FA=0U;({unsigned int _tmp75F=e1->loc;struct _dyneither_ptr _tmp75E=({const char*_tmp2FB="subscript applied to pointer to one object";_tag_dyneither(_tmp2FB,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp75F,_tmp75E,_tag_dyneither(_tmp2FA,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp302);}}
# 1679
Cyc_Tcenv_check_rgn_accessible(_tmp2DE,loc,_tmp303);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp305),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2DE))
({void*_tmp2FC=0U;({unsigned int _tmp761=e1->loc;struct _dyneither_ptr _tmp760=({const char*_tmp2FD="can't subscript an abstract pointer";_tag_dyneither(_tmp2FD,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp761,_tmp760,_tag_dyneither(_tmp2FC,sizeof(void*),0U));});});
return _tmp305;case 10U: _LL3: _tmp306=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2E2)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2DE,loc,_tmp306,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp300=({struct Cyc_String_pa_PrintArg_struct _tmp553;_tmp553.tag=0U,({struct _dyneither_ptr _tmp762=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp553.f1=_tmp762;});_tmp553;});void*_tmp2FE[1U];_tmp2FE[0]=& _tmp300;({struct Cyc_Tcenv_Tenv*_tmp766=_tmp2DE;unsigned int _tmp765=loc;void**_tmp764=topt;struct _dyneither_ptr _tmp763=({const char*_tmp2FF="subscript applied to %s";_tag_dyneither(_tmp2FF,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp766,_tmp765,_tmp764,_tmp763,_tag_dyneither(_tmp2FE,sizeof(void*),1U));});});}_LL0:;};};}
# 1689
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp307=Cyc_Tcutil_compress(*topt);void*_tmp308=_tmp307;struct Cyc_List_List*_tmp30C;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp308)->tag == 10U){_LL1: _tmp30C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp308)->f1;_LL2:
# 1695
 if(({int _tmp767=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30C);_tmp767 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1698
goto _LL0;
# 1700
for(0;es != 0;(es=es->tl,_tmp30C=_tmp30C->tl)){
int bogus=0;
void*_tmp309=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp30C))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp768=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp768,& _tmp309,(struct Cyc_Absyn_Exp*)es->hd);});
# 1705
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp30C->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp76A=({struct Cyc_List_List*_tmp30B=_cycalloc(sizeof(*_tmp30B));({struct _tuple17*_tmp769=({struct _tuple17*_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A->f1=(*((struct _tuple17*)_tmp30C->hd)).f1,_tmp30A->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp30A;});_tmp30B->hd=_tmp769;}),_tmp30B->tl=fields;_tmp30B;});fields=_tmp76A;});}
# 1708
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1712
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp76B=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp76B,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp76E=({struct Cyc_List_List*_tmp30E=_cycalloc(sizeof(*_tmp30E));({struct _tuple17*_tmp76D=({struct _tuple17*_tmp30D=_cycalloc(sizeof(*_tmp30D));({struct Cyc_Absyn_Tqual _tmp76C=Cyc_Absyn_empty_tqual(0U);_tmp30D->f1=_tmp76C;}),_tmp30D->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp30D;});_tmp30E->hd=_tmp76D;}),_tmp30E->tl=fields;_tmp30E;});fields=_tmp76E;});}
# 1717
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->tag=10U,({struct Cyc_List_List*_tmp76F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp30F->f1=_tmp76F;});_tmp30F;});}
# 1721
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1724
return({void*_tmp310=0U;({struct Cyc_Tcenv_Tenv*_tmp773=te;unsigned int _tmp772=loc;void**_tmp771=topt;struct _dyneither_ptr _tmp770=({const char*_tmp311="tcCompoundLit";_tag_dyneither(_tmp311,sizeof(char),14U);});Cyc_Tcexp_expr_err(_tmp773,_tmp772,_tmp771,_tmp770,_tag_dyneither(_tmp310,sizeof(void*),0U));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1734 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1737
void*res_t2;
int _tmp312=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp313=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->tag=0U,({union Cyc_Absyn_Cnst _tmp774=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp312);_tmp329->f1=_tmp774;});_tmp329;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp313,loc);
# 1745
if(zero_term){
struct Cyc_Absyn_Exp*_tmp314=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp312 - 1);
if(!Cyc_Tcutil_is_zero(_tmp314))
({void*_tmp315=0U;({unsigned int _tmp776=_tmp314->loc;struct _dyneither_ptr _tmp775=({const char*_tmp316="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp316,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp776,_tmp775,_tag_dyneither(_tmp315,sizeof(void*),0U));});});}
# 1750
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp77A=({void*_tmp779=res;struct Cyc_Absyn_Tqual _tmp778=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1751
struct Cyc_Absyn_Exp*_tmp777=sz_exp;Cyc_Absyn_array_typ(_tmp779,_tmp778,_tmp777,
# 1753
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0U);});
# 1751
res_t2=_tmp77A;});
# 1755
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1758
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1760
({struct Cyc_String_pa_PrintArg_struct _tmp319=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,({
struct _dyneither_ptr _tmp77B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));_tmp554.f1=_tmp77B;});_tmp554;});void*_tmp317[1U];_tmp317[0]=& _tmp319;({unsigned int _tmp77D=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp77C=({const char*_tmp318="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp318,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp77D,_tmp77C,_tag_dyneither(_tmp317,sizeof(void*),1U));});});
# 1763
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1768
void*_tmp31A=(void*)ds->hd;void*_tmp31B=_tmp31A;struct Cyc_Absyn_Exp*_tmp328;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp31B)->tag == 1U){_LL1: _LL2:
# 1770
({void*_tmp31C=0U;({unsigned int _tmp77F=loc;struct _dyneither_ptr _tmp77E=({const char*_tmp31D="only array index designators are supported";_tag_dyneither(_tmp31D,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp77F,_tmp77E,_tag_dyneither(_tmp31C,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp328=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp31B)->f1;_LL4:
# 1773
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp328);{
struct _tuple15 _tmp31E=Cyc_Evexp_eval_const_uint_exp(_tmp328);struct _tuple15 _tmp31F=_tmp31E;unsigned int _tmp327;int _tmp326;_LL6: _tmp327=_tmp31F.f1;_tmp326=_tmp31F.f2;_LL7:;
if(!_tmp326)
({void*_tmp320=0U;({unsigned int _tmp781=_tmp328->loc;struct _dyneither_ptr _tmp780=({const char*_tmp321="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp321,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp781,_tmp780,_tag_dyneither(_tmp320,sizeof(void*),0U));});});else{
if(_tmp327 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp324=({struct Cyc_Int_pa_PrintArg_struct _tmp556;_tmp556.tag=1U,_tmp556.f1=(unsigned long)offset;_tmp556;});struct Cyc_Int_pa_PrintArg_struct _tmp325=({struct Cyc_Int_pa_PrintArg_struct _tmp555;_tmp555.tag=1U,_tmp555.f1=(unsigned long)((int)_tmp327);_tmp555;});void*_tmp322[2U];_tmp322[0]=& _tmp324,_tmp322[1]=& _tmp325;({unsigned int _tmp783=_tmp328->loc;struct _dyneither_ptr _tmp782=({const char*_tmp323="expecting index designator %d but found %d";_tag_dyneither(_tmp323,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp783,_tmp782,_tag_dyneither(_tmp322,sizeof(void*),2U));});});}
# 1780
goto _LL0;};}_LL0:;}}}
# 1784
return res_t2;}
# 1788
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1791
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp32A=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp32B=_tmp32A;void*_tmp330;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp32B)->tag == 19U){_LL1: _tmp330=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp32B)->f1;_LL2:
# 1796
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp32C=({void*_tmp784=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp784,Cyc_Absyn_valueof_exp(_tmp330,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp32C->topt=bound->topt;
# 1801
bound=_tmp32C;}
# 1803
goto _LL0;}else{_LL3: _LL4:
# 1805
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp32F=({struct Cyc_String_pa_PrintArg_struct _tmp557;_tmp557.tag=0U,({
struct _dyneither_ptr _tmp785=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp557.f1=_tmp785;});_tmp557;});void*_tmp32D[1U];_tmp32D[0]=& _tmp32F;({unsigned int _tmp787=bound->loc;struct _dyneither_ptr _tmp786=({const char*_tmp32E="expecting unsigned int, found %s";_tag_dyneither(_tmp32E,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp787,_tmp786,_tag_dyneither(_tmp32D,sizeof(void*),1U));});});}_LL0:;}
# 1810
if(!(vd->tq).real_const)
({void*_tmp331=0U;({struct _dyneither_ptr _tmp788=({const char*_tmp332="comprehension index variable is not declared const!";_tag_dyneither(_tmp332,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp788,_tag_dyneither(_tmp331,sizeof(void*),0U));});});
# 1813
if(te->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp789=Cyc_Tcenv_new_block(loc,te);te=_tmp789;});{
void**_tmp333=0;
struct Cyc_Absyn_Tqual*_tmp334=0;
union Cyc_Absyn_Constraint**_tmp335=0;
# 1819
if(topt != 0){
void*_tmp336=Cyc_Tcutil_compress(*topt);void*_tmp337=_tmp336;void*_tmp342;struct Cyc_Absyn_Tqual _tmp341;struct Cyc_Absyn_Exp*_tmp340;union Cyc_Absyn_Constraint*_tmp33F;struct Cyc_Absyn_PtrInfo _tmp33E;switch(*((int*)_tmp337)){case 5U: _LL6: _tmp33E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp337)->f1;_LL7:
# 1822
({void**_tmp78A=({void**_tmp338=_cycalloc(sizeof(*_tmp338));*_tmp338=_tmp33E.elt_typ;_tmp338;});_tmp333=_tmp78A;});
({struct Cyc_Absyn_Tqual*_tmp78B=({struct Cyc_Absyn_Tqual*_tmp339=_cycalloc(sizeof(*_tmp339));*_tmp339=_tmp33E.elt_tq;_tmp339;});_tmp334=_tmp78B;});
({union Cyc_Absyn_Constraint**_tmp78C=({union Cyc_Absyn_Constraint**_tmp33A=_cycalloc(sizeof(*_tmp33A));*_tmp33A=(_tmp33E.ptr_atts).zero_term;_tmp33A;});_tmp335=_tmp78C;});
goto _LL5;case 8U: _LL8: _tmp342=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp337)->f1).elt_type;_tmp341=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp337)->f1).tq;_tmp340=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp337)->f1).num_elts;_tmp33F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp337)->f1).zero_term;_LL9:
# 1827
({void**_tmp78D=({void**_tmp33B=_cycalloc(sizeof(*_tmp33B));*_tmp33B=_tmp342;_tmp33B;});_tmp333=_tmp78D;});
({struct Cyc_Absyn_Tqual*_tmp78E=({struct Cyc_Absyn_Tqual*_tmp33C=_cycalloc(sizeof(*_tmp33C));*_tmp33C=_tmp341;_tmp33C;});_tmp334=_tmp78E;});
({union Cyc_Absyn_Constraint**_tmp78F=({union Cyc_Absyn_Constraint**_tmp33D=_cycalloc(sizeof(*_tmp33D));*_tmp33D=_tmp33F;_tmp33D;});_tmp335=_tmp78F;});
goto _LL5;default: _LLA: _LLB:
# 1832
 goto _LL5;}_LL5:;}{
# 1835
void*t=Cyc_Tcexp_tcExp(te,_tmp333,body);
# 1837
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp343=0U;({unsigned int _tmp791=body->loc;struct _dyneither_ptr _tmp790=({const char*_tmp344="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp344,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp791,_tmp790,_tag_dyneither(_tmp343,sizeof(void*),0U));});});
if(te->le == 0){
# 1841
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp345=0U;({unsigned int _tmp793=bound->loc;struct _dyneither_ptr _tmp792=({const char*_tmp346="bound is not constant";_tag_dyneither(_tmp346,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp793,_tmp792,_tag_dyneither(_tmp345,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp347=0U;({unsigned int _tmp795=bound->loc;struct _dyneither_ptr _tmp794=({const char*_tmp348="body is not constant";_tag_dyneither(_tmp348,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp795,_tmp794,_tag_dyneither(_tmp347,sizeof(void*),0U));});});}
# 1846
if(_tmp335 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp335)){
# 1849
struct Cyc_Absyn_Exp*_tmp349=Cyc_Absyn_uint_exp(1U,0U);_tmp349->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp796=Cyc_Absyn_add_exp(bound,_tmp349,0U);bound=_tmp796;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}
# 1853
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp34A=0U;({unsigned int _tmp798=body->loc;struct _dyneither_ptr _tmp797=({const char*_tmp34B="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp34B,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp798,_tmp797,_tag_dyneither(_tmp34A,sizeof(void*),0U));});});{
# 1857
void*_tmp34C=({void*_tmp79B=t;struct Cyc_Absyn_Tqual _tmp79A=_tmp334 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp334;struct Cyc_Absyn_Exp*_tmp799=bound;Cyc_Absyn_array_typ(_tmp79B,_tmp79A,_tmp799,
_tmp335 == 0?Cyc_Absyn_false_conref:*_tmp335,0U);});
return _tmp34C;};};};}
# 1863
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1866
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp34D=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp34E=_tmp34D;void*_tmp353;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp34E)->tag == 19U){_LL1: _tmp353=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp34E)->f1;_LL2:
# 1871
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp34F=({void*_tmp79C=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp79C,Cyc_Absyn_valueof_exp(_tmp353,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp34F->topt=bound->topt;
# 1876
bound=_tmp34F;}
# 1878
goto _LL0;}else{_LL3: _LL4:
# 1880
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp352=({struct Cyc_String_pa_PrintArg_struct _tmp558;_tmp558.tag=0U,({
struct _dyneither_ptr _tmp79D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp558.f1=_tmp79D;});_tmp558;});void*_tmp350[1U];_tmp350[0]=& _tmp352;({unsigned int _tmp79F=bound->loc;struct _dyneither_ptr _tmp79E=({const char*_tmp351="expecting unsigned int, found %s";_tag_dyneither(_tmp351,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp79F,_tmp79E,_tag_dyneither(_tmp350,sizeof(void*),1U));});});}_LL0:;}{
# 1885
void**_tmp354=0;
struct Cyc_Absyn_Tqual*_tmp355=0;
union Cyc_Absyn_Constraint**_tmp356=0;
# 1889
if(topt != 0){
void*_tmp357=Cyc_Tcutil_compress(*topt);void*_tmp358=_tmp357;void*_tmp363;struct Cyc_Absyn_Tqual _tmp362;struct Cyc_Absyn_Exp*_tmp361;union Cyc_Absyn_Constraint*_tmp360;struct Cyc_Absyn_PtrInfo _tmp35F;switch(*((int*)_tmp358)){case 5U: _LL6: _tmp35F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp358)->f1;_LL7:
# 1892
({void**_tmp7A0=({void**_tmp359=_cycalloc(sizeof(*_tmp359));*_tmp359=_tmp35F.elt_typ;_tmp359;});_tmp354=_tmp7A0;});
({struct Cyc_Absyn_Tqual*_tmp7A1=({struct Cyc_Absyn_Tqual*_tmp35A=_cycalloc(sizeof(*_tmp35A));*_tmp35A=_tmp35F.elt_tq;_tmp35A;});_tmp355=_tmp7A1;});
({union Cyc_Absyn_Constraint**_tmp7A2=({union Cyc_Absyn_Constraint**_tmp35B=_cycalloc(sizeof(*_tmp35B));*_tmp35B=(_tmp35F.ptr_atts).zero_term;_tmp35B;});_tmp356=_tmp7A2;});
goto _LL5;case 8U: _LL8: _tmp363=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).elt_type;_tmp362=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).tq;_tmp361=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).num_elts;_tmp360=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp358)->f1).zero_term;_LL9:
# 1897
({void**_tmp7A3=({void**_tmp35C=_cycalloc(sizeof(*_tmp35C));*_tmp35C=_tmp363;_tmp35C;});_tmp354=_tmp7A3;});
({struct Cyc_Absyn_Tqual*_tmp7A4=({struct Cyc_Absyn_Tqual*_tmp35D=_cycalloc(sizeof(*_tmp35D));*_tmp35D=_tmp362;_tmp35D;});_tmp355=_tmp7A4;});
({union Cyc_Absyn_Constraint**_tmp7A5=({union Cyc_Absyn_Constraint**_tmp35E=_cycalloc(sizeof(*_tmp35E));*_tmp35E=_tmp360;_tmp35E;});_tmp356=_tmp7A5;});
goto _LL5;default: _LLA: _LLB:
# 1902
 goto _LL5;}_LL5:;}
# 1905
({unsigned int _tmp7A8=loc;struct Cyc_Tcenv_Tenv*_tmp7A7=te;struct Cyc_List_List*_tmp7A6=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7A8,_tmp7A7,_tmp7A6,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp354 != 0)Cyc_Tcutil_unify(*_tmp354,t);
# 1908
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp364=0U;({unsigned int _tmp7AA=bound->loc;struct _dyneither_ptr _tmp7A9=({const char*_tmp365="bound is not constant";_tag_dyneither(_tmp365,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp7AA,_tmp7A9,_tag_dyneither(_tmp364,sizeof(void*),0U));});});}
# 1912
if(_tmp356 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp356)){
# 1915
struct Cyc_Absyn_Exp*_tmp366=Cyc_Absyn_uint_exp(1U,0U);_tmp366->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp7AB=Cyc_Absyn_add_exp(bound,_tmp366,0U);bound=_tmp7AB;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1919
({void*_tmp367=0U;({unsigned int _tmp7AD=loc;struct _dyneither_ptr _tmp7AC=({const char*_tmp368="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp368,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp7AD,_tmp7AC,_tag_dyneither(_tmp367,sizeof(void*),0U));});});}{
# 1922
void*_tmp369=({void*_tmp7B0=t;struct Cyc_Absyn_Tqual _tmp7AF=_tmp355 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp355;struct Cyc_Absyn_Exp*_tmp7AE=bound;Cyc_Absyn_array_typ(_tmp7B0,_tmp7AF,_tmp7AE,
_tmp356 == 0?Cyc_Absyn_false_conref:*_tmp356,0U);});
return _tmp369;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1937 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1941
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp7B1=({struct Cyc_Absyn_Aggrdecl**_tmp36A=_cycalloc(sizeof(*_tmp36A));*_tmp36A=ad;_tmp36A;});adptr=_tmp7B1;});}else{
# 1947
{struct _handler_cons _tmp36B;_push_handler(& _tmp36B);{int _tmp36D=0;if(setjmp(_tmp36B.handler))_tmp36D=1;if(!_tmp36D){({struct Cyc_Absyn_Aggrdecl**_tmp7B2=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp7B2;});
ad=*adptr;
# 1947
;_pop_handler();}else{void*_tmp36C=(void*)_exn_thrown;void*_tmp36E=_tmp36C;void*_tmp372;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp36E)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1950
({struct Cyc_String_pa_PrintArg_struct _tmp371=({struct Cyc_String_pa_PrintArg_struct _tmp559;_tmp559.tag=0U,({struct _dyneither_ptr _tmp7B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp559.f1=_tmp7B3;});_tmp559;});void*_tmp36F[1U];_tmp36F[0]=& _tmp371;({unsigned int _tmp7B5=loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp370="unbound struct/union name %s";_tag_dyneither(_tmp370,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp36F,sizeof(void*),1U));});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL3: _tmp372=_tmp36E;_LL4:(int)_rethrow(_tmp372);}_LL0:;}};}
# 1953
*ad_opt=ad;
*tn=ad->name;}
# 1956
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp375=({struct Cyc_String_pa_PrintArg_struct _tmp55A;_tmp55A.tag=0U,({struct _dyneither_ptr _tmp7B6=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp376="struct";_tag_dyneither(_tmp376,sizeof(char),7U);}):({const char*_tmp377="union";_tag_dyneither(_tmp377,sizeof(char),6U);}));_tmp55A.f1=_tmp7B6;});_tmp55A;});void*_tmp373[1U];_tmp373[0]=& _tmp375;({unsigned int _tmp7B8=loc;struct _dyneither_ptr _tmp7B7=({const char*_tmp374="can't construct abstract %s";_tag_dyneither(_tmp374,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp7B8,_tmp7B7,_tag_dyneither(_tmp373,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1964
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp378=Cyc_Tcenv_new_status(te);
if(_tmp378 == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp7B9=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp7B9;});else{
# 1973
({struct Cyc_Tcenv_Tenv*_tmp7BA=Cyc_Tcenv_set_new_status(_tmp378,te);te2=_tmp7BA;});}{
# 1975
struct _tuple12 _tmp379=({struct _tuple12 _tmp560;({struct Cyc_List_List*_tmp7BB=Cyc_Tcenv_lookup_type_vars(te2);_tmp560.f1=_tmp7BB;}),_tmp560.f2=Cyc_Core_heap_region;_tmp560;});
struct Cyc_List_List*_tmp37A=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp379,ad->tvs);
struct Cyc_List_List*_tmp37B=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp379,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp37C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp37A);
struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp37B);
struct Cyc_List_List*_tmp37E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp37A,_tmp37B);
void*res_typ;
# 1986
if(topt != 0){
void*_tmp37F=Cyc_Tcutil_compress(*topt);void*_tmp380=_tmp37F;struct Cyc_Absyn_Aggrdecl**_tmp384;struct Cyc_List_List*_tmp383;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp380)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp380)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp384=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp380)->f1).aggr_info).KnownAggr).val;_tmp383=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp380)->f1).targs;_LL7:
# 1989
 if(*_tmp384 == *adptr){
{struct Cyc_List_List*_tmp381=_tmp37C;for(0;_tmp381 != 0  && _tmp383 != 0;(
_tmp381=_tmp381->tl,_tmp383=_tmp383->tl)){
Cyc_Tcutil_unify((void*)_tmp381->hd,(void*)_tmp383->hd);}}
# 1994
res_typ=*topt;
goto _LL5;}
# 1997
goto _LL9;}else{goto _LL8;}}else{_LL8: _LL9:
# 1999
({void*_tmp7BD=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7BC=Cyc_Absyn_KnownAggr(adptr);(_tmp382->f1).aggr_info=_tmp7BC;}),(_tmp382->f1).targs=_tmp37C;_tmp382;});res_typ=_tmp7BD;});}_LL5:;}else{
# 2002
({void*_tmp7BF=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7BE=Cyc_Absyn_KnownAggr(adptr);(_tmp385->f1).aggr_info=_tmp7BE;}),(_tmp385->f1).targs=_tmp37C;_tmp385;});res_typ=_tmp7BF;});}{
# 2005
struct Cyc_List_List*_tmp386=*ts;
struct Cyc_List_List*_tmp387=_tmp37D;
while(_tmp386 != 0  && _tmp387 != 0){
# 2009
({unsigned int _tmp7C2=loc;struct Cyc_Tcenv_Tenv*_tmp7C1=te2;struct Cyc_List_List*_tmp7C0=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp7C2,_tmp7C1,_tmp7C0,& Cyc_Tcutil_ak,1,0,(void*)_tmp386->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp386->hd);
Cyc_Tcutil_unify((void*)_tmp386->hd,(void*)_tmp387->hd);
_tmp386=_tmp386->tl;
_tmp387=_tmp387->tl;}
# 2015
if(_tmp386 != 0)
({void*_tmp388=0U;({unsigned int _tmp7C4=loc;struct _dyneither_ptr _tmp7C3=({const char*_tmp389="too many explicit witness types";_tag_dyneither(_tmp389,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7C4,_tmp7C3,_tag_dyneither(_tmp388,sizeof(void*),0U));});});
# 2018
*ts=_tmp37D;{
# 2021
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp38A=(struct _tuple19*)fields->hd;struct _tuple19*_tmp38B=_tmp38A;struct Cyc_Absyn_Aggrfield*_tmp397;struct Cyc_Absyn_Exp*_tmp396;_LLB: _tmp397=_tmp38B->f1;_tmp396=_tmp38B->f2;_LLC:;{
void*_tmp38C=Cyc_Tcutil_substitute(_tmp37E,_tmp397->type);
({struct Cyc_Tcenv_Tenv*_tmp7C5=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7C5,& _tmp38C,_tmp396);});
# 2032
if(!Cyc_Tcutil_coerce_arg(te2,_tmp396,_tmp38C,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp38F=({struct Cyc_String_pa_PrintArg_struct _tmp55F;_tmp55F.tag=0U,_tmp55F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp397->name);_tmp55F;});struct Cyc_String_pa_PrintArg_struct _tmp390=({struct Cyc_String_pa_PrintArg_struct _tmp55E;_tmp55E.tag=0U,({
struct _dyneither_ptr _tmp7C6=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp394="struct";_tag_dyneither(_tmp394,sizeof(char),7U);}):({const char*_tmp395="union";_tag_dyneither(_tmp395,sizeof(char),6U);}));_tmp55E.f1=_tmp7C6;});_tmp55E;});struct Cyc_String_pa_PrintArg_struct _tmp391=({struct Cyc_String_pa_PrintArg_struct _tmp55D;_tmp55D.tag=0U,({
struct _dyneither_ptr _tmp7C7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp55D.f1=_tmp7C7;});_tmp55D;});struct Cyc_String_pa_PrintArg_struct _tmp392=({struct Cyc_String_pa_PrintArg_struct _tmp55C;_tmp55C.tag=0U,({struct _dyneither_ptr _tmp7C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp38C));_tmp55C.f1=_tmp7C8;});_tmp55C;});struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp55B;_tmp55B.tag=0U,({
struct _dyneither_ptr _tmp7C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp396->topt)));_tmp55B.f1=_tmp7C9;});_tmp55B;});void*_tmp38D[5U];_tmp38D[0]=& _tmp38F,_tmp38D[1]=& _tmp390,_tmp38D[2]=& _tmp391,_tmp38D[3]=& _tmp392,_tmp38D[4]=& _tmp393;({unsigned int _tmp7CB=_tmp396->loc;struct _dyneither_ptr _tmp7CA=({const char*_tmp38E="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp38E,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp7CB,_tmp7CA,_tag_dyneither(_tmp38D,sizeof(void*),5U));});});
Cyc_Tcutil_explain_failure();}};}{
# 2041
struct Cyc_List_List*_tmp398=0;
{struct Cyc_List_List*_tmp399=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp399 != 0;_tmp399=_tmp399->tl){
({struct Cyc_List_List*_tmp7CF=({struct Cyc_List_List*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct _tuple0*_tmp7CE=({struct _tuple0*_tmp39A=_cycalloc(sizeof(*_tmp39A));({void*_tmp7CD=Cyc_Tcutil_substitute(_tmp37E,(*((struct _tuple0*)_tmp399->hd)).f1);_tmp39A->f1=_tmp7CD;}),({
void*_tmp7CC=Cyc_Tcutil_substitute(_tmp37E,(*((struct _tuple0*)_tmp399->hd)).f2);_tmp39A->f2=_tmp7CC;});_tmp39A;});
# 2043
_tmp39B->hd=_tmp7CE;}),_tmp39B->tl=_tmp398;_tmp39B;});_tmp398=_tmp7CF;});}}
# 2046
({struct Cyc_List_List*_tmp7D0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp398);_tmp398=_tmp7D0;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp398);
return res_typ;};};};};};}
# 2052
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2054
{void*_tmp39C=Cyc_Tcutil_compress(ts);void*_tmp39D=_tmp39C;enum Cyc_Absyn_AggrKind _tmp3AC;struct Cyc_List_List*_tmp3AB;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp39D)->tag == 12U){_LL1: _tmp3AC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp39D)->f1;_tmp3AB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp39D)->f2;_LL2:
# 2056
 if(_tmp3AC == Cyc_Absyn_UnionA)
({void*_tmp39E=0U;({unsigned int _tmp7D2=loc;struct _dyneither_ptr _tmp7D1=({const char*_tmp39F="expecting struct but found union";_tag_dyneither(_tmp39F,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7D2,_tmp7D1,_tag_dyneither(_tmp39E,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp3AB);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3A0=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3A1=_tmp3A0;struct Cyc_Absyn_Aggrfield*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3A7;_LL6: _tmp3A8=_tmp3A1->f1;_tmp3A7=_tmp3A1->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp7D4=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp7D3=& _tmp3A8->type;Cyc_Tcexp_tcExpInitializer(_tmp7D4,_tmp7D3,_tmp3A7);});
# 2065
if(!Cyc_Tcutil_coerce_arg(te,_tmp3A7,_tmp3A8->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3A4=({struct Cyc_String_pa_PrintArg_struct _tmp563;_tmp563.tag=0U,_tmp563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3A8->name);_tmp563;});struct Cyc_String_pa_PrintArg_struct _tmp3A5=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0U,({
struct _dyneither_ptr _tmp7D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3A8->type));_tmp562.f1=_tmp7D5;});_tmp562;});struct Cyc_String_pa_PrintArg_struct _tmp3A6=({struct Cyc_String_pa_PrintArg_struct _tmp561;_tmp561.tag=0U,({
struct _dyneither_ptr _tmp7D6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp3A7->topt)));_tmp561.f1=_tmp7D6;});_tmp561;});void*_tmp3A2[3U];_tmp3A2[0]=& _tmp3A4,_tmp3A2[1]=& _tmp3A5,_tmp3A2[2]=& _tmp3A6;({unsigned int _tmp7D8=_tmp3A7->loc;struct _dyneither_ptr _tmp7D7=({const char*_tmp3A3="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3A3,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp7D8,_tmp7D7,_tag_dyneither(_tmp3A2,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2072
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp3A9=0U;({struct _dyneither_ptr _tmp7D9=({const char*_tmp3AA="tcAnonStruct: wrong type";_tag_dyneither(_tmp3AA,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D9,_tag_dyneither(_tmp3A9,sizeof(void*),0U));});});}_LL0:;}
# 2075
return ts;}
# 2079
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2082
struct _tuple12 _tmp3AD=({struct _tuple12 _tmp569;({struct Cyc_List_List*_tmp7DA=Cyc_Tcenv_lookup_type_vars(te);_tmp569.f1=_tmp7DA;}),_tmp569.f2=Cyc_Core_heap_region;_tmp569;});
struct Cyc_List_List*_tmp3AE=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp3AD,tud->tvs);
struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3AE);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));
_tmp3BE->tag=4U,({union Cyc_Absyn_DatatypeFieldInfoU _tmp7DB=Cyc_Absyn_KnownDatatypefield(tud,tuf);(_tmp3BE->f1).field_info=_tmp7DB;}),(_tmp3BE->f1).targs=_tmp3AF;_tmp3BE;});
# 2089
if(topt != 0){
void*_tmp3B0=Cyc_Tcutil_compress(*topt);void*_tmp3B1=_tmp3B0;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3B1)->tag == 4U){_LL1: _LL2:
 Cyc_Tcutil_unify(*topt,res);goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2095
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp3AE != 0)({void*_tmp7DC=Cyc_Tcutil_substitute(_tmp3AE,t);t=_tmp7DC;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B4=({struct Cyc_String_pa_PrintArg_struct _tmp566;_tmp566.tag=0U,({
# 2105
struct _dyneither_ptr _tmp7DD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp566.f1=_tmp7DD;});_tmp566;});struct Cyc_String_pa_PrintArg_struct _tmp3B5=({struct Cyc_String_pa_PrintArg_struct _tmp565;_tmp565.tag=0U,({struct _dyneither_ptr _tmp7DE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp565.f1=_tmp7DE;});_tmp565;});struct Cyc_String_pa_PrintArg_struct _tmp3B6=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0U,({
struct _dyneither_ptr _tmp7DF=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3B7="?";_tag_dyneither(_tmp3B7,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp564.f1=_tmp7DF;});_tmp564;});void*_tmp3B2[3U];_tmp3B2[0]=& _tmp3B4,_tmp3B2[1]=& _tmp3B5,_tmp3B2[2]=& _tmp3B6;({unsigned int _tmp7E1=e->loc;struct _dyneither_ptr _tmp7E0=({const char*_tmp3B3="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3B3,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp7E1,_tmp7E0,_tag_dyneither(_tmp3B2,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2110
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3BA=({struct Cyc_String_pa_PrintArg_struct _tmp567;_tmp567.tag=0U,({
# 2113
struct _dyneither_ptr _tmp7E2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp567.f1=_tmp7E2;});_tmp567;});void*_tmp3B8[1U];_tmp3B8[0]=& _tmp3BA;({struct Cyc_Tcenv_Tenv*_tmp7E6=te;unsigned int _tmp7E5=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7E4=topt;struct _dyneither_ptr _tmp7E3=({const char*_tmp3B9="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3B9,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp7E6,_tmp7E5,_tmp7E4,_tmp7E3,_tag_dyneither(_tmp3B8,sizeof(void*),1U));});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3BD=({struct Cyc_String_pa_PrintArg_struct _tmp568;_tmp568.tag=0U,({
struct _dyneither_ptr _tmp7E7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp568.f1=_tmp7E7;});_tmp568;});void*_tmp3BB[1U];_tmp3BB[0]=& _tmp3BD;({struct Cyc_Tcenv_Tenv*_tmp7EB=te;unsigned int _tmp7EA=loc;void**_tmp7E9=topt;struct _dyneither_ptr _tmp7E8=({const char*_tmp3BC="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3BC,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7EB,_tmp7EA,_tmp7E9,_tmp7E8,_tag_dyneither(_tmp3BB,sizeof(void*),1U));});});
return res;};}
# 2120
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2122
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2124
if(topt != 0){
void*_tmp3BF=Cyc_Tcutil_compress(*topt);void*_tmp3C0=_tmp3BF;void*_tmp3C1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C0)->tag == 5U){_LL1: _tmp3C1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C0)->f1).elt_typ;_LL2:
# 2127
 Cyc_Tcutil_unify(_tmp3C1,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2133
({struct Cyc_String_pa_PrintArg_struct _tmp3C4=({struct Cyc_String_pa_PrintArg_struct _tmp56B;_tmp56B.tag=0U,({
struct _dyneither_ptr _tmp7EC=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3C6="calloc";_tag_dyneither(_tmp3C6,sizeof(char),7U);}):(struct _dyneither_ptr)({const char*_tmp3C7="malloc";_tag_dyneither(_tmp3C7,sizeof(char),7U);})));_tmp56B.f1=_tmp7EC;});_tmp56B;});struct Cyc_String_pa_PrintArg_struct _tmp3C5=({struct Cyc_String_pa_PrintArg_struct _tmp56A;_tmp56A.tag=0U,({struct _dyneither_ptr _tmp7ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp56A.f1=_tmp7ED;});_tmp56A;});void*_tmp3C2[2U];_tmp3C2[0]=& _tmp3C4,_tmp3C2[1]=& _tmp3C5;({unsigned int _tmp7EF=loc;struct _dyneither_ptr _tmp7EE=({const char*_tmp3C3="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3C3,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp7EF,_tmp7EE,_tag_dyneither(_tmp3C2,sizeof(void*),2U));});});}
# 2137
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2139
enum Cyc_Absyn_AliasQual _tmp3C8=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3C9=_tmp3C8;if(_tmp3C9 == Cyc_Absyn_Unique){_LL1: _LL2:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL3: _LL4:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL0:;}
# 2145
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2150
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2153
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));
_tmp3D0->tag=15U,({void*_tmp7F0=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3D0->f1=_tmp7F0;});_tmp3D0;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3CA=Cyc_Tcutil_compress(handle_type);void*_tmp3CB=_tmp3CA;void*_tmp3CF;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3CB)->tag == 15U){_LL1: _tmp3CF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3CB)->f1;_LL2:
# 2158
 rgn=_tmp3CF;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2162
({struct Cyc_String_pa_PrintArg_struct _tmp3CE=({struct Cyc_String_pa_PrintArg_struct _tmp56C;_tmp56C.tag=0U,({
struct _dyneither_ptr _tmp7F1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp56C.f1=_tmp7F1;});_tmp56C;});void*_tmp3CC[1U];_tmp3CC[0]=& _tmp3CE;({unsigned int _tmp7F3=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7F2=({const char*_tmp3CD="expecting region_t type but found %s";_tag_dyneither(_tmp3CD,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7F3,_tmp7F2,_tag_dyneither(_tmp3CC,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2169
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7F4=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7F4;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2176
({struct Cyc_Tcenv_Tenv*_tmp7F5=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F5,& Cyc_Absyn_uint_typ,*e);});{
# 2185 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3D1=0U;({struct _dyneither_ptr _tmp7F6=({const char*_tmp3D2="calloc with empty type";_tag_dyneither(_tmp3D2,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F6,_tag_dyneither(_tmp3D1,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7F9=loc;struct Cyc_Tcenv_Tenv*_tmp7F8=te;struct Cyc_List_List*_tmp7F7=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7F9,_tmp7F8,_tmp7F7,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2197
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3D3=er;struct Cyc_Absyn_Exp*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E7;void*_tmp3E6;switch(*((int*)_tmp3D3)){case 17U: _LL6: _tmp3E6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D3)->f1;_LL7:
# 2201
 elt_type=_tmp3E6;
({void**_tmp7FA=({void**_tmp3D4=_cycalloc(sizeof(*_tmp3D4));*_tmp3D4=elt_type;_tmp3D4;});*t=_tmp7FA;});
({struct Cyc_Absyn_Exp*_tmp7FB=Cyc_Absyn_uint_exp(1U,0U);num_elts=_tmp7FB;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D3)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D3)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D3)->f2)->tl)->tl == 0){_LL8: _tmp3E8=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D3)->f2)->hd;_tmp3E7=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D3)->f2)->tl)->hd;_LL9:
# 2208
{struct _tuple0 _tmp3D5=({struct _tuple0 _tmp56D;_tmp56D.f1=_tmp3E8->r,_tmp56D.f2=_tmp3E7->r;_tmp56D;});struct _tuple0 _tmp3D6=_tmp3D5;void*_tmp3DA;void*_tmp3D9;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D6.f1)->tag == 17U){_LLD: _tmp3D9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D6.f1)->f1;_LLE:
# 2210
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3D9);
elt_type=_tmp3D9;
({void**_tmp7FC=({void**_tmp3D7=_cycalloc(sizeof(*_tmp3D7));*_tmp3D7=elt_type;_tmp3D7;});*t=_tmp7FC;});
num_elts=_tmp3E7;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D6.f2)->tag == 17U){_LLF: _tmp3DA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D6.f2)->f1;_LL10:
# 2217
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3DA);
elt_type=_tmp3DA;
({void**_tmp7FD=({void**_tmp3D8=_cycalloc(sizeof(*_tmp3D8));*_tmp3D8=elt_type;_tmp3D8;});*t=_tmp7FD;});
num_elts=_tmp3E8;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2225
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2227
 No_sizeof: {
# 2230
struct Cyc_Absyn_Exp*_tmp3DB=*e;
{void*_tmp3DC=_tmp3DB->r;void*_tmp3DD=_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DD)->tag == 14U){_LL14: _tmp3DE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DD)->f2;_LL15:
 _tmp3DB=_tmp3DE;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2235
{void*_tmp3DF=Cyc_Tcutil_compress((void*)_check_null(_tmp3DB->topt));void*_tmp3E0=_tmp3DF;void*_tmp3E4;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E0)->tag == 19U){_LL19: _tmp3E4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E0)->f1;_LL1A:
# 2237
{void*_tmp3E1=Cyc_Tcutil_compress(_tmp3E4);void*_tmp3E2=_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E3;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2)->tag == 18U){_LL1E: _tmp3E3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2)->f1;_LL1F:
# 2239
 er=_tmp3E3->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2242
goto _LL18;}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2245
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7FE=({void**_tmp3E5=_cycalloc(sizeof(*_tmp3E5));*_tmp3E5=elt_type;_tmp3E5;});*t=_tmp7FE;});
num_elts=*e;
one_elt=0;}
# 2250
goto _LL5;}_LL5:;}}
# 2254
*is_fat=!one_elt;
# 2257
{void*_tmp3E9=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3EC;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E9)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E9)->f1).aggr_info).KnownAggr).tag == 2){_LL23: _tmp3EC=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E9)->f1).aggr_info).KnownAggr).val;_LL24:
# 2259
 if(_tmp3EC->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3EC->impl))->exist_vars != 0)
({void*_tmp3EA=0U;({unsigned int _tmp800=loc;struct _dyneither_ptr _tmp7FF=({const char*_tmp3EB="malloc with existential types not yet implemented";_tag_dyneither(_tmp3EB,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp800,_tmp7FF,_tag_dyneither(_tmp3EA,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2266
void*(*_tmp3ED)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp3EE=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp3EF=Cyc_Tcutil_compress(*topt);void*_tmp3F0=_tmp3EF;union Cyc_Absyn_Constraint*_tmp3FD;union Cyc_Absyn_Constraint*_tmp3FC;union Cyc_Absyn_Constraint*_tmp3FB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0)->tag == 5U){_LL28: _tmp3FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0)->f1).ptr_atts).nullable;_tmp3FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0)->f1).ptr_atts).bounds;_tmp3FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0)->f1).ptr_atts).zero_term;_LL29:
# 2271
 _tmp3EE=_tmp3FB;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3FD))
_tmp3ED=Cyc_Absyn_star_typ;
# 2276
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3FB) && !(*is_calloc)){
({void*_tmp3F1=0U;({unsigned int _tmp802=loc;struct _dyneither_ptr _tmp801=({const char*_tmp3F2="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp3F2,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp802,_tmp801,_tag_dyneither(_tmp3F1,sizeof(void*),0U));});});
*is_calloc=1;}
# 2282
{void*_tmp3F3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3FC);void*_tmp3F4=_tmp3F3;struct Cyc_Absyn_Exp*_tmp3FA;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3F4)->tag == 0U){_LL2D: _LL2E:
 goto _LL2C;}else{_LL2F: _tmp3FA=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F4)->f1;if(!one_elt){_LL30: {
# 2285
int _tmp3F5=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp3F5  && Cyc_Evexp_same_const_exp(_tmp3FA,num_elts)){
*is_fat=0;
return({void*_tmp806=elt_type;void*_tmp805=rgn;struct Cyc_Absyn_Tqual _tmp804=Cyc_Absyn_empty_tqual(0U);void*_tmp803=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3FC);Cyc_Absyn_atb_typ(_tmp806,_tmp805,_tmp804,_tmp803,_tmp3EE);});}
# 2291
{void*_tmp3F6=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp3F7=_tmp3F6;void*_tmp3F9;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F7)->tag == 19U){_LL34: _tmp3F9=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F7)->f1;_LL35: {
# 2293
struct Cyc_Absyn_Exp*_tmp3F8=({void*_tmp807=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp807,Cyc_Absyn_valueof_exp(_tmp3F9,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2295
if(Cyc_Evexp_same_const_exp(_tmp3F8,_tmp3FA)){
*is_fat=0;
return({void*_tmp80B=elt_type;void*_tmp80A=rgn;struct Cyc_Absyn_Tqual _tmp809=Cyc_Absyn_empty_tqual(0U);void*_tmp808=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3FC);Cyc_Absyn_atb_typ(_tmp80B,_tmp80A,_tmp809,_tmp808,_tmp3EE);});}
# 2300
goto _LL33;}}else{_LL36: _LL37:
 goto _LL33;}_LL33:;}
# 2303
goto _LL2C;}}else{_LL31: _LL32:
 goto _LL2C;}}_LL2C:;}
# 2306
goto _LL27;}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2309
if(!one_elt)_tmp3ED=Cyc_Absyn_dyneither_typ;
return({void*(*_tmp80F)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=_tmp3ED;void*_tmp80E=elt_type;void*_tmp80D=rgn;struct Cyc_Absyn_Tqual _tmp80C=Cyc_Absyn_empty_tqual(0U);_tmp80F(_tmp80E,_tmp80D,_tmp80C,_tmp3EE);});};};}
# 2314
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2316
struct Cyc_Tcenv_Tenv*_tmp3FE=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp3FE,0,e1);{
void*_tmp3FF=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp3FE,& _tmp3FF,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2323
{void*_tmp400=Cyc_Tcutil_compress(t1);void*_tmp401=_tmp400;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401)->tag == 8U){_LL1: _LL2:
({void*_tmp402=0U;({unsigned int _tmp811=loc;struct _dyneither_ptr _tmp810=({const char*_tmp403="cannot assign to an array";_tag_dyneither(_tmp403,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp811,_tmp810,_tag_dyneither(_tmp402,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2328
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp404=0U;({unsigned int _tmp813=loc;struct _dyneither_ptr _tmp812=({const char*_tmp405="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp405,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp813,_tmp812,_tag_dyneither(_tmp404,sizeof(void*),0U));});});
# 2332
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp406=0U;({struct Cyc_Tcenv_Tenv*_tmp817=te;unsigned int _tmp816=e1->loc;void**_tmp815=topt;struct _dyneither_ptr _tmp814=({const char*_tmp407="swap non-lvalue";_tag_dyneither(_tmp407,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp817,_tmp816,_tmp815,_tmp814,_tag_dyneither(_tmp406,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp408=0U;({struct Cyc_Tcenv_Tenv*_tmp81B=te;unsigned int _tmp81A=e2->loc;void**_tmp819=topt;struct _dyneither_ptr _tmp818=({const char*_tmp409="swap non-lvalue";_tag_dyneither(_tmp409,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp81B,_tmp81A,_tmp819,_tmp818,_tag_dyneither(_tmp408,sizeof(void*),0U));});});
# 2337
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp40A=({struct Cyc_String_pa_PrintArg_struct _tmp40D=({struct Cyc_String_pa_PrintArg_struct _tmp56F;_tmp56F.tag=0U,({
struct _dyneither_ptr _tmp81C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp56F.f1=_tmp81C;});_tmp56F;});struct Cyc_String_pa_PrintArg_struct _tmp40E=({struct Cyc_String_pa_PrintArg_struct _tmp56E;_tmp56E.tag=0U,({struct _dyneither_ptr _tmp81D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp56E.f1=_tmp81D;});_tmp56E;});void*_tmp40B[2U];_tmp40B[0]=& _tmp40D,_tmp40B[1]=& _tmp40E;({struct Cyc_Tcenv_Tenv*_tmp821=te;unsigned int _tmp820=loc;void**_tmp81F=topt;struct _dyneither_ptr _tmp81E=({const char*_tmp40C="type mismatch: %s != %s";_tag_dyneither(_tmp40C,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp821,_tmp820,_tmp81F,_tmp81E,_tag_dyneither(_tmp40B,sizeof(void*),2U));});});
return _tmp40A;}
# 2344
return(void*)& Cyc_Absyn_VoidType_val;};};}
# 2349
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp822=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp822,s,1);});
# 2352
while(1){
void*_tmp40F=s->r;void*_tmp410=_tmp40F;struct Cyc_Absyn_Decl*_tmp41B;struct Cyc_Absyn_Stmt*_tmp41A;struct Cyc_Absyn_Stmt*_tmp419;struct Cyc_Absyn_Stmt*_tmp418;struct Cyc_Absyn_Exp*_tmp417;switch(*((int*)_tmp410)){case 1U: _LL1: _tmp417=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp410)->f1;_LL2: {
# 2356
void*_tmp411=(void*)_check_null(_tmp417->topt);
if(!({void*_tmp823=_tmp411;Cyc_Tcutil_unify(_tmp823,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp414=({struct Cyc_String_pa_PrintArg_struct _tmp570;_tmp570.tag=0U,({
struct _dyneither_ptr _tmp824=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp411));_tmp570.f1=_tmp824;});_tmp570;});void*_tmp412[1U];_tmp412[0]=& _tmp414;({unsigned int _tmp826=loc;struct _dyneither_ptr _tmp825=({const char*_tmp413="statement expression returns type %s";_tag_dyneither(_tmp413,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp826,_tmp825,_tag_dyneither(_tmp412,sizeof(void*),1U));});});
Cyc_Tcutil_explain_failure();}
# 2362
return _tmp411;}case 2U: _LL3: _tmp419=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp410)->f1;_tmp418=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp410)->f2;_LL4:
 s=_tmp418;continue;case 12U: _LL5: _tmp41B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp410)->f1;_tmp41A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp410)->f2;_LL6:
 s=_tmp41A;continue;default: _LL7: _LL8:
# 2366
 return({void*_tmp415=0U;({struct Cyc_Tcenv_Tenv*_tmp82A=te;unsigned int _tmp829=loc;void**_tmp828=topt;struct _dyneither_ptr _tmp827=({const char*_tmp416="statement expression must end with expression";_tag_dyneither(_tmp416,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp82A,_tmp829,_tmp828,_tmp827,_tag_dyneither(_tmp415,sizeof(void*),0U));});});}_LL0:;}}
# 2371
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp82B=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp82B,0,e);}));
{void*_tmp41C=t;struct Cyc_Absyn_Aggrdecl*_tmp420;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41C)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41C)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp420=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41C)->f1).aggr_info).KnownAggr).val;_LL2:
# 2375
 if((_tmp420->kind == Cyc_Absyn_UnionA  && _tmp420->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp420->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{_LL3: _LL4:
# 2378
({struct Cyc_String_pa_PrintArg_struct _tmp41F=({struct Cyc_String_pa_PrintArg_struct _tmp571;_tmp571.tag=0U,({struct _dyneither_ptr _tmp82C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp571.f1=_tmp82C;});_tmp571;});void*_tmp41D[1U];_tmp41D[0]=& _tmp41F;({unsigned int _tmp82E=loc;struct _dyneither_ptr _tmp82D=({const char*_tmp41E="expecting @tagged union but found %s";_tag_dyneither(_tmp41E,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp82E,_tmp82D,_tag_dyneither(_tmp41D,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2381
return Cyc_Absyn_uint_typ;}
# 2385
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2389
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct Cyc_Tcenv_Tenv*_tmp421=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2393
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp428=_cycalloc(sizeof(*_tmp428));
_tmp428->tag=15U,({void*_tmp82F=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp421));_tmp428->f1=_tmp82F;});_tmp428;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp421,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp422=Cyc_Tcutil_compress(handle_type);void*_tmp423=_tmp422;void*_tmp427;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp423)->tag == 15U){_LL1: _tmp427=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp423)->f1;_LL2:
# 2398
 rgn=_tmp427;
Cyc_Tcenv_check_rgn_accessible(_tmp421,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2402
({struct Cyc_String_pa_PrintArg_struct _tmp426=({struct Cyc_String_pa_PrintArg_struct _tmp572;_tmp572.tag=0U,({
struct _dyneither_ptr _tmp830=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp572.f1=_tmp830;});_tmp572;});void*_tmp424[1U];_tmp424[0]=& _tmp426;({unsigned int _tmp832=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp831=({const char*_tmp425="expecting region_t type but found %s";_tag_dyneither(_tmp425,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp832,_tmp831,_tag_dyneither(_tmp424,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2409
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp833=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp833;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2417
void*_tmp429=e1->r;void*_tmp42A=_tmp429;struct Cyc_List_List*_tmp444;struct Cyc_Core_Opt*_tmp443;struct Cyc_List_List*_tmp442;switch(*((int*)_tmp42A)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2422
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp421,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp42B=0U;({struct _dyneither_ptr _tmp834=({const char*_tmp42C="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp42C,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp834,_tag_dyneither(_tmp42B,sizeof(void*),0U));});});
({void*_tmp835=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp835;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp421,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp421,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2433
return res_typ;}case 36U: _LLA: _tmp443=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp42A)->f1;_tmp442=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp42A)->f2;_LLB:
# 2435
({void*_tmp836=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->tag=26U,_tmp42D->f1=_tmp442;_tmp42D;});e1->r=_tmp836;});
_tmp444=_tmp442;goto _LLD;case 26U: _LLC: _tmp444=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp42A)->f1;_LLD: {
# 2438
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp42E=Cyc_Tcutil_compress(*topt);void*_tmp42F=_tmp42E;void**_tmp432;struct Cyc_Absyn_Tqual _tmp431;union Cyc_Absyn_Constraint*_tmp430;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->tag == 5U){_LL15: _tmp432=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->f1).elt_typ;_tmp431=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->f1).elt_tq;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->f1).ptr_atts).zero_term;_LL16:
# 2444
 elt_typ_opt=_tmp432;
({int _tmp837=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp430);zero_term=_tmp837;});
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2450
void*res_typ=Cyc_Tcexp_tcArray(_tmp421,e1->loc,elt_typ_opt,0,zero_term,_tmp444);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp433=0U;({struct _dyneither_ptr _tmp838=({const char*_tmp434="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp434,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp838,_tag_dyneither(_tmp433,sizeof(void*),0U));});});
({void*_tmp839=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp839;});
if(topt != 0){
# 2459
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp421,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp421,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2465
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp42A)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2470
void*_tmp435=({void*_tmp83C=Cyc_Absyn_char_typ;void*_tmp83B=rgn;struct Cyc_Absyn_Tqual _tmp83A=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp83C,_tmp83B,_tmp83A,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2472
void*_tmp436=Cyc_Tcexp_tcExp(_tmp421,& _tmp435,e1);
return({void*_tmp841=_tmp436;void*_tmp840=rgn;struct Cyc_Absyn_Tqual _tmp83F=Cyc_Absyn_empty_tqual(0U);void*_tmp83E=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=1U,({struct Cyc_Absyn_Exp*_tmp83D=Cyc_Absyn_uint_exp(1U,0U);_tmp437->f1=_tmp83D;});_tmp437;});Cyc_Absyn_atb_typ(_tmp841,_tmp840,_tmp83F,_tmp83E,Cyc_Absyn_false_conref);});}case 9U: _LL10: _LL11: {
# 2477
void*_tmp438=({void*_tmp844=Cyc_Absyn_wchar_typ();void*_tmp843=rgn;struct Cyc_Absyn_Tqual _tmp842=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp844,_tmp843,_tmp842,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2479
void*_tmp439=Cyc_Tcexp_tcExp(_tmp421,& _tmp438,e1);
return({void*_tmp849=_tmp439;void*_tmp848=rgn;struct Cyc_Absyn_Tqual _tmp847=Cyc_Absyn_empty_tqual(0U);void*_tmp846=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=1U,({struct Cyc_Absyn_Exp*_tmp845=Cyc_Absyn_uint_exp(1U,0U);_tmp43A->f1=_tmp845;});_tmp43A;});Cyc_Absyn_atb_typ(_tmp849,_tmp848,_tmp847,_tmp846,Cyc_Absyn_false_conref);});}default: goto _LL12;}default: _LL12: _LL13:
# 2486
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp43B=Cyc_Tcutil_compress(*topt);void*_tmp43C=_tmp43B;void**_tmp43E;struct Cyc_Absyn_Tqual _tmp43D;switch(*((int*)_tmp43C)){case 5U: _LL1A: _tmp43E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43C)->f1).elt_typ;_tmp43D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43C)->f1).elt_tq;_LL1B:
# 2492
 topt2=_tmp43E;goto _LL19;case 3U: _LL1C: _LL1D:
# 2496
 bogus=*topt;
topt2=& bogus;
goto _LL19;default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2502
void*telt=Cyc_Tcexp_tcExp(_tmp421,topt2,e1);
# 2504
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp43F=0U;({unsigned int _tmp84B=e1->loc;struct _dyneither_ptr _tmp84A=({const char*_tmp440="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp440,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp84B,_tmp84A,_tag_dyneither(_tmp43F,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp441=_cycalloc(sizeof(*_tmp441));
_tmp441->tag=5U,(_tmp441->f1).elt_typ=telt,({struct Cyc_Absyn_Tqual _tmp84D=Cyc_Absyn_empty_tqual(0U);(_tmp441->f1).elt_tq=_tmp84D;}),
((_tmp441->f1).ptr_atts).rgn=rgn,({union Cyc_Absyn_Constraint*_tmp84C=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp441->f1).ptr_atts).nullable=_tmp84C;}),((_tmp441->f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,((_tmp441->f1).ptr_atts).zero_term=Cyc_Absyn_false_conref,((_tmp441->f1).ptr_atts).ptrloc=0;_tmp441;});
# 2511
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp421,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp421,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2518
return res_typ;};};}}_LL5:;};}
# 2524
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp850=({void*_tmp84F=({void*_tmp84E=t;Cyc_Tcutil_promote_array(_tmp84E,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp84F;});e->topt=_tmp850;});
return t;}
# 2534
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2538
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2543
({void*_tmp445=0U;({unsigned int _tmp852=e->loc;struct _dyneither_ptr _tmp851=({const char*_tmp446="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp446,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp852,_tmp851,_tag_dyneither(_tmp445,sizeof(void*),0U));});});{
# 2545
void*_tmp447=e->r;void*_tmp448=_tmp447;switch(*((int*)_tmp448)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp448)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2552
({void*_tmp853=Cyc_Tcutil_compress(t);t=_tmp853;});
if(Cyc_Tcutil_is_array(t)){
({void*_tmp855=({void*_tmp854=t;Cyc_Tcutil_promote_array(_tmp854,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp855;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2557
return t;}_LL0:;};}
# 2568 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp449=e->r;void*_tmp44A=_tmp449;struct Cyc_Absyn_Exp*_tmp468;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp44A)->tag == 12U){_LL1: _tmp468=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp44A)->f1;_LL2:
# 2572
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp468);
({void*_tmp856=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp468->topt),0);_tmp468->topt=_tmp856;});
e->topt=_tmp468->topt;
goto _LL0;}else{_LL3: _LL4:
# 2578
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp857=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp857;});
# 2581
{void*_tmp44B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp44C=_tmp44B;void*_tmp467;struct Cyc_Absyn_Tqual _tmp466;void*_tmp465;union Cyc_Absyn_Constraint*_tmp464;union Cyc_Absyn_Constraint*_tmp463;union Cyc_Absyn_Constraint*_tmp462;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->tag == 5U){_LL6: _tmp467=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).elt_typ;_tmp466=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).elt_tq;_tmp465=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).ptr_atts).rgn;_tmp464=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).ptr_atts).nullable;_tmp463=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).ptr_atts).bounds;_tmp462=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C)->f1).ptr_atts).zero_term;_LL7:
# 2583
{void*_tmp44D=Cyc_Tcutil_compress(_tmp467);void*_tmp44E=_tmp44D;struct Cyc_List_List*_tmp461;void*_tmp460;struct Cyc_Absyn_Tqual _tmp45F;void*_tmp45E;struct Cyc_List_List*_tmp45D;int _tmp45C;struct Cyc_Absyn_VarargInfo*_tmp45B;struct Cyc_List_List*_tmp45A;struct Cyc_List_List*_tmp459;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_List_List*_tmp457;struct Cyc_Absyn_Exp*_tmp456;struct Cyc_List_List*_tmp455;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->tag == 9U){_LLB: _tmp461=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).tvars;_tmp460=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).effect;_tmp45F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).ret_tqual;_tmp45E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).ret_typ;_tmp45D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).args;_tmp45C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).c_varargs;_tmp45B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).cyc_varargs;_tmp45A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).rgn_po;_tmp459=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).attributes;_tmp458=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).requires_clause;_tmp457=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).requires_relns;_tmp456=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).ensures_clause;_tmp455=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44E)->f1).ensures_relns;_LLC:
# 2585
 if(_tmp461 != 0){
struct _tuple12 _tmp44F=({struct _tuple12 _tmp573;({struct Cyc_List_List*_tmp858=Cyc_Tcenv_lookup_type_vars(te);_tmp573.f1=_tmp858;}),_tmp573.f2=Cyc_Core_heap_region;_tmp573;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp44F,_tmp461);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2593
({struct Cyc_List_List*_tmp859=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp45A);_tmp45A=_tmp859;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp45A);{
void*ftyp=({struct Cyc_List_List*_tmp85A=inst;Cyc_Tcutil_substitute(_tmp85A,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp454=_cycalloc(sizeof(*_tmp454));
_tmp454->tag=9U,(_tmp454->f1).tvars=0,(_tmp454->f1).effect=_tmp460,(_tmp454->f1).ret_tqual=_tmp45F,(_tmp454->f1).ret_typ=_tmp45E,(_tmp454->f1).args=_tmp45D,(_tmp454->f1).c_varargs=_tmp45C,(_tmp454->f1).cyc_varargs=_tmp45B,(_tmp454->f1).rgn_po=0,(_tmp454->f1).attributes=_tmp459,(_tmp454->f1).requires_clause=_tmp458,(_tmp454->f1).requires_relns=_tmp457,(_tmp454->f1).ensures_clause=_tmp456,(_tmp454->f1).ensures_relns=_tmp455;_tmp454;}));});
# 2600
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp450=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->tag=5U,(_tmp453->f1).elt_typ=ftyp,(_tmp453->f1).elt_tq=_tmp466,((_tmp453->f1).ptr_atts).rgn=_tmp465,((_tmp453->f1).ptr_atts).nullable=_tmp464,((_tmp453->f1).ptr_atts).bounds=_tmp463,((_tmp453->f1).ptr_atts).zero_term=_tmp462,((_tmp453->f1).ptr_atts).ptrloc=0;_tmp453;});
# 2602
struct Cyc_Absyn_Exp*_tmp451=Cyc_Absyn_copy_exp(e);
({void*_tmp85B=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->tag=13U,_tmp452->f1=_tmp451,_tmp452->f2=ts;_tmp452;});e->r=_tmp85B;});
e->topt=(void*)_tmp450;};}
# 2606
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2609
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2612
goto _LL0;}_LL0:;}
# 2614
return(void*)_check_null(e->topt);}
# 2622
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2624
struct Cyc_List_List*_tmp469=0;
# 2626
{void*_tmp46A=fn_exp->r;void*_tmp46B=_tmp46A;struct Cyc_List_List*_tmp47F;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->tag == 10U){_LL1: _tmp47F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL2:
# 2628
{void*_tmp46C=e->r;void*_tmp46D=_tmp46C;struct Cyc_List_List*_tmp47C;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46D)->tag == 10U){_LL6: _tmp47C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46D)->f2;_LL7: {
# 2630
struct Cyc_List_List*_tmp46E=alias_arg_exps;
int _tmp46F=0;
while(_tmp46E != 0){
while(_tmp46F != (int)_tmp46E->hd){
if(_tmp47F == 0)
({void*_tmp470=0U;({struct _dyneither_ptr _tmp85D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp473=({struct Cyc_Int_pa_PrintArg_struct _tmp575;_tmp575.tag=1U,_tmp575.f1=(unsigned long)_tmp46F;_tmp575;});struct Cyc_Int_pa_PrintArg_struct _tmp474=({struct Cyc_Int_pa_PrintArg_struct _tmp574;_tmp574.tag=1U,_tmp574.f1=(unsigned long)((int)_tmp46E->hd);_tmp574;});void*_tmp471[2U];_tmp471[0]=& _tmp473,_tmp471[1]=& _tmp474;({struct _dyneither_ptr _tmp85C=({const char*_tmp472="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp472,sizeof(char),36U);});Cyc_aprintf(_tmp85C,_tag_dyneither(_tmp471,sizeof(void*),2U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp85D,_tag_dyneither(_tmp470,sizeof(void*),0U));});});
# 2637
++ _tmp46F;
_tmp47F=_tmp47F->tl;
_tmp47C=((struct Cyc_List_List*)_check_null(_tmp47C))->tl;}{
# 2642
struct _tuple11 _tmp475=({struct Cyc_Absyn_Exp*_tmp85E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47F))->hd;Cyc_Tcutil_insert_alias(_tmp85E,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47C))->hd)->topt)));});struct _tuple11 _tmp476=_tmp475;struct Cyc_Absyn_Decl*_tmp479;struct Cyc_Absyn_Exp*_tmp478;_LLB: _tmp479=_tmp476.f1;_tmp478=_tmp476.f2;_LLC:;
_tmp47F->hd=(void*)_tmp478;
({struct Cyc_List_List*_tmp85F=({struct Cyc_List_List*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->hd=_tmp479,_tmp477->tl=_tmp469;_tmp477;});_tmp469=_tmp85F;});
_tmp46E=_tmp46E->tl;};}
# 2647
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp47A=0U;({struct _dyneither_ptr _tmp860=({const char*_tmp47B="not a function call!";_tag_dyneither(_tmp47B,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp860,_tag_dyneither(_tmp47A,sizeof(void*),0U));});});}_LL5:;}
# 2650
goto _LL0;}else{_LL3: _LL4:
({void*_tmp47D=0U;({struct _dyneither_ptr _tmp861=({const char*_tmp47E="not a function call!";_tag_dyneither(_tmp47E,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp861,_tag_dyneither(_tmp47D,sizeof(void*),0U));});});}_LL0:;}
# 2655
while(_tmp469 != 0){
struct Cyc_Absyn_Decl*_tmp480=(struct Cyc_Absyn_Decl*)_tmp469->hd;
({struct Cyc_Absyn_Exp*_tmp865=({struct Cyc_Absyn_Stmt*_tmp864=({struct Cyc_Absyn_Decl*_tmp863=_tmp480;struct Cyc_Absyn_Stmt*_tmp862=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp863,_tmp862,e->loc);});Cyc_Absyn_stmt_exp(_tmp864,e->loc);});fn_exp=_tmp865;});
_tmp469=_tmp469->tl;}
# 2662
e->topt=0;
e->r=fn_exp->r;}
# 2668
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2672
{void*_tmp481=e->r;void*_tmp482=_tmp481;void*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct _dyneither_ptr*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;int*_tmp4E6;struct Cyc_Absyn_Exp**_tmp4E5;void***_tmp4E4;struct Cyc_Absyn_Exp**_tmp4E3;int*_tmp4E2;void*_tmp4E1;struct Cyc_Absyn_Enumfield*_tmp4E0;struct Cyc_Absyn_Enumdecl*_tmp4DF;struct Cyc_Absyn_Enumfield*_tmp4DE;struct Cyc_List_List*_tmp4DD;struct Cyc_Absyn_Datatypedecl*_tmp4DC;struct Cyc_Absyn_Datatypefield*_tmp4DB;void*_tmp4DA;struct Cyc_List_List*_tmp4D9;struct _tuple1**_tmp4D8;struct Cyc_List_List**_tmp4D7;struct Cyc_List_List*_tmp4D6;struct Cyc_Absyn_Aggrdecl**_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D4;void*_tmp4D3;int*_tmp4D2;struct Cyc_Absyn_Vardecl*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;int*_tmp4CE;struct Cyc_Absyn_Stmt*_tmp4CD;struct Cyc_List_List*_tmp4CC;struct _tuple9*_tmp4CB;struct Cyc_List_List*_tmp4CA;struct Cyc_List_List*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C6;struct _dyneither_ptr*_tmp4C5;int*_tmp4C4;int*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C2;struct _dyneither_ptr*_tmp4C1;int*_tmp4C0;int*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4BE;void*_tmp4BD;struct Cyc_List_List*_tmp4BC;void*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BA;struct Cyc_Absyn_Exp**_tmp4B9;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;void*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B5;enum Cyc_Absyn_Coercion*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Core_Opt*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;enum Cyc_Absyn_Incrementor _tmp4A3;enum Cyc_Absyn_Primop _tmp4A2;struct Cyc_List_List*_tmp4A1;void**_tmp4A0;union Cyc_Absyn_Cnst*_tmp49F;struct Cyc_Core_Opt*_tmp49E;struct Cyc_List_List*_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_VarargCallInfo**_tmp49A;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_List_List*_tmp498;struct Cyc_Absyn_VarargCallInfo**_tmp497;int*_tmp496;struct Cyc_Absyn_Exp*_tmp495;switch(*((int*)_tmp482)){case 12U: _LL1: _tmp495=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL2:
# 2677
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp495);
return;case 10U: _LL3: _tmp499=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp498=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp497=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_tmp496=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp482)->f4;if(!(*_tmp496)){_LL4:
# 2681
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp866=({const char*_tmp483="unresolved function in tcExpNoInst";_tag_dyneither(_tmp483,sizeof(char),35U);});_tmp484->f1=_tmp866;});_tmp484;}));}else{_LL1B: _tmp49C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp49B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp49A=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_LL1C: {
# 2714
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp485;_push_handler(& _tmp485);{int _tmp487=0;if(setjmp(_tmp485.handler))_tmp487=1;if(!_tmp487){
({struct Cyc_Absyn_Exp*_tmp867=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp867;});;_pop_handler();}else{void*_tmp486=(void*)_exn_thrown;void*_tmp488=_tmp486;void*_tmp489;if(((struct Cyc_Core_Failure_exn_struct*)_tmp488)->tag == Cyc_Core_Failure){_LL58: _LL59:
# 2721
 ok=0;
fn_exp=e;
goto _LL57;}else{_LL5A: _tmp489=_tmp488;_LL5B:(int)_rethrow(_tmp489);}_LL57:;}};}
# 2725
({void*_tmp868=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp49C,_tmp49B,_tmp49A,& alias_arg_exps);t=_tmp868;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp869=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp869;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2732
goto _LL0;}}case 36U: _LL5: _tmp49E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp49D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL6:
# 2685
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp49D);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp49F=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL8:
# 2690
({void*_tmp86A=Cyc_Tcexp_tcConst(te,loc,topt,_tmp49F,e);t=_tmp86A;});goto _LL0;case 1U: _LL9: _tmp4A0=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LLA:
# 2692
({void*_tmp86B=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp4A0);t=_tmp86B;});goto _LL0;case 2U: _LLB: _LLC:
# 2694
 t=Cyc_Absyn_sint_typ;goto _LL0;case 3U: _LLD: _tmp4A2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4A1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LLE:
# 2696
({void*_tmp86C=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4A2,_tmp4A1);t=_tmp86C;});goto _LL0;case 5U: _LLF: _tmp4A4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4A3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL10:
# 2698
({void*_tmp86D=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp4A4,_tmp4A3);t=_tmp86D;});goto _LL0;case 4U: _LL11: _tmp4A7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4A6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4A5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_LL12:
# 2700
({void*_tmp86E=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4A7,_tmp4A6,_tmp4A5);t=_tmp86E;});goto _LL0;case 6U: _LL13: _tmp4AA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4A9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4A8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_LL14:
# 2702
({void*_tmp86F=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp4AA,_tmp4A9,_tmp4A8);t=_tmp86F;});goto _LL0;case 7U: _LL15: _tmp4AC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4AB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL16:
# 2704
({void*_tmp870=Cyc_Tcexp_tcAnd(te,loc,_tmp4AC,_tmp4AB);t=_tmp870;});goto _LL0;case 8U: _LL17: _tmp4AE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4AD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL18:
# 2706
({void*_tmp871=Cyc_Tcexp_tcOr(te,loc,_tmp4AE,_tmp4AD);t=_tmp871;});goto _LL0;case 9U: _LL19: _tmp4B0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4AF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL1A:
# 2708
({void*_tmp872=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4B0,_tmp4AF);t=_tmp872;});goto _LL0;case 11U: _LL1D: _tmp4B1=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL1E:
# 2734
({void*_tmp873=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4B1);t=_tmp873;});goto _LL0;case 13U: _LL1F: _tmp4B3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4B2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL20:
# 2736
({void*_tmp874=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4B3,_tmp4B2);t=_tmp874;});goto _LL0;case 14U: _LL21: _tmp4B6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4B5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4B4=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp482)->f4;_LL22:
# 2738
({void*_tmp875=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4B6,_tmp4B5,_tmp4B4);t=_tmp875;});goto _LL0;case 15U: _LL23: _tmp4B7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL24:
# 2740
({void*_tmp876=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4B7);t=_tmp876;});goto _LL0;case 16U: _LL25: _tmp4B9=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4B8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL26:
# 2742
({void*_tmp877=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4B9,e,_tmp4B8);t=_tmp877;});goto _LL0;case 18U: _LL27: _tmp4BA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL28: {
# 2744
void*_tmp48A=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4BA);
({void*_tmp878=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp48A);t=_tmp878;});goto _LL0;}case 17U: _LL29: _tmp4BB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL2A:
# 2747
({void*_tmp879=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4BB);t=_tmp879;});goto _LL0;case 19U: _LL2B: _tmp4BD=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4BC=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL2C:
# 2749
({void*_tmp87A=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4BD,_tmp4BC);t=_tmp87A;});goto _LL0;case 20U: _LL2D: _tmp4BE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL2E:
# 2751
({void*_tmp87B=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4BE);t=_tmp87B;});goto _LL0;case 21U: _LL2F: _tmp4C2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4C1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4C0=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_tmp4BF=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp482)->f4;_LL30:
# 2753
({void*_tmp87C=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4C2,_tmp4C1,_tmp4C0,_tmp4BF);t=_tmp87C;});goto _LL0;case 22U: _LL31: _tmp4C6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4C5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4C4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_tmp4C3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp482)->f4;_LL32:
# 2755
({void*_tmp87D=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4C6,_tmp4C5,_tmp4C4,_tmp4C3);t=_tmp87D;});goto _LL0;case 23U: _LL33: _tmp4C8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4C7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL34:
# 2757
({void*_tmp87E=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4C8,_tmp4C7);t=_tmp87E;});goto _LL0;case 24U: _LL35: _tmp4C9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL36:
# 2759
({void*_tmp87F=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4C9);t=_tmp87F;});goto _LL0;case 25U: _LL37: _tmp4CB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4CA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL38:
# 2761
({void*_tmp880=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4CB,_tmp4CA);t=_tmp880;});goto _LL0;case 26U: _LL39: _tmp4CC=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL3A: {
# 2765
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp48B=Cyc_Tcutil_compress(*topt);void*_tmp48C=_tmp48B;void**_tmp48F;struct Cyc_Absyn_Tqual*_tmp48E;union Cyc_Absyn_Constraint*_tmp48D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48C)->tag == 8U){_LL5D: _tmp48F=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48C)->f1).elt_type;_tmp48E=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48C)->f1).tq;_tmp48D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48C)->f1).zero_term;_LL5E:
# 2771
 elt_topt=_tmp48F;
elt_tqopt=_tmp48E;
({int _tmp881=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp48D);zero_term=_tmp881;});
goto _LL5C;}else{_LL5F: _LL60:
 goto _LL5C;}_LL5C:;}
# 2778
({void*_tmp882=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4CC);t=_tmp882;});goto _LL0;}case 37U: _LL3B: _tmp4CD=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL3C:
# 2780
({void*_tmp883=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4CD);t=_tmp883;});goto _LL0;case 27U: _LL3D: _tmp4D1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4D0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4CF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_tmp4CE=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp482)->f4;_LL3E:
# 2782
({void*_tmp884=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4D1,_tmp4D0,_tmp4CF,_tmp4CE);t=_tmp884;});goto _LL0;case 28U: _LL3F: _tmp4D4=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4D3=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4D2=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_LL40:
# 2784
({void*_tmp885=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4D4,_tmp4D3,_tmp4D2);t=_tmp885;});goto _LL0;case 29U: _LL41: _tmp4D8=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4D7=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4D6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_tmp4D5=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp482)->f4;_LL42:
# 2786
({void*_tmp886=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4D8,_tmp4D7,_tmp4D6,_tmp4D5);t=_tmp886;});goto _LL0;case 30U: _LL43: _tmp4DA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4D9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL44:
# 2788
({void*_tmp887=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4DA,_tmp4D9);t=_tmp887;});goto _LL0;case 31U: _LL45: _tmp4DD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4DC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_tmp4DB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp482)->f3;_LL46:
# 2790
({void*_tmp888=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp4DD,_tmp4DC,_tmp4DB);t=_tmp888;});goto _LL0;case 32U: _LL47: _tmp4DF=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4DE=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL48:
# 2792
({void*_tmp889=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->tag=13U,_tmp490->f1=_tmp4DF->name,_tmp490->f2=_tmp4DF;_tmp490;});t=_tmp889;});goto _LL0;case 33U: _LL49: _tmp4E1=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4E0=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL4A:
# 2794
 t=_tmp4E1;goto _LL0;case 34U: _LL4B: _tmp4E6=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp482)->f1).is_calloc;_tmp4E5=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp482)->f1).rgn;_tmp4E4=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp482)->f1).elt_type;_tmp4E3=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp482)->f1).num_elts;_tmp4E2=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp482)->f1).fat_result;_LL4C:
# 2796
({void*_tmp88A=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4E5,_tmp4E4,_tmp4E3,_tmp4E6,_tmp4E2);t=_tmp88A;});goto _LL0;case 35U: _LL4D: _tmp4E8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4E7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL4E:
# 2798
({void*_tmp88B=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4E8,_tmp4E7);t=_tmp88B;});goto _LL0;case 38U: _LL4F: _tmp4EA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_tmp4E9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp482)->f2;_LL50:
# 2800
({void*_tmp88C=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp4EA,_tmp4E9);t=_tmp88C;});goto _LL0;case 41U: _LL51: _tmp4EB=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL52:
# 2802
({void*_tmp88D=Cyc_Tcexp_tcExp(te,topt,_tmp4EB);t=_tmp88D;});goto _LL0;case 39U: _LL53: _tmp4EC=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL54:
# 2804
 if(!te->allow_valueof)
({void*_tmp491=0U;({unsigned int _tmp88F=e->loc;struct _dyneither_ptr _tmp88E=({const char*_tmp492="valueof(-) can only occur within types";_tag_dyneither(_tmp492,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp88F,_tmp88E,_tag_dyneither(_tmp491,sizeof(void*),0U));});});
# 2812
t=Cyc_Absyn_sint_typ;
goto _LL0;default: _LL55: _LL56:
# 2815
({void*_tmp493=0U;({unsigned int _tmp891=e->loc;struct _dyneither_ptr _tmp890=({const char*_tmp494="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp494,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp891,_tmp890,_tag_dyneither(_tmp493,sizeof(void*),0U));});});
({void*_tmp892=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp892;});}_LL0:;}
# 2818
e->topt=t;}
