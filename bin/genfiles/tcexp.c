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
({void*_tmp0=0U;({unsigned int _tmp578=loc;struct _dyneither_ptr _tmp577=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp578,_tmp577,_tag_dyneither(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 55
return*topt;}}
# 64
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 68
if(topt == 0){
# 70
({void*_tmp579=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=26U,_tmp1->f1=des;_tmp1;});e->r=_tmp579;});
return;}{
# 73
void*t=*topt;
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;void*_tmpE;struct Cyc_Absyn_Tqual _tmpD;union Cyc_Absyn_AggrInfoU _tmpC;switch(*((int*)_tmp3)){case 11U: _LL1: _tmpC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3)->f1).aggr_info;_LL2:
# 76
{union Cyc_Absyn_AggrInfoU _tmp4=_tmpC;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp4.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp57A=({const char*_tmp6="struct type not properly set";_tag_dyneither(_tmp6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp57A,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp4.KnownAggr).val;_LLD:
({void*_tmp57B=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=29U,_tmp7->f1=_tmp8->name,_tmp7->f2=0,_tmp7->f3=des,_tmp7->f4=_tmp8;_tmp7;});e->r=_tmp57B;});}_LL9:;}
# 80
goto _LL0;case 8U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4:
({void*_tmp57C=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=26U,_tmp9->f1=des;_tmp9;});e->r=_tmp57C;});goto _LL0;case 12U: _LL5: _LL6:
({void*_tmp57D=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=30U,_tmpA->f1=t,_tmpA->f2=des;_tmpA;});e->r=_tmp57D;});goto _LL0;default: _LL7: _LL8:
({void*_tmp57E=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp57E;});goto _LL0;}_LL0:;};}
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
 if(Cyc_Tc_aggressive_warn)({void*_tmp11=0U;({unsigned int _tmp580=e->loc;struct _dyneither_ptr _tmp57F=({const char*_tmp12="assignment in test";_tag_dyneither(_tmp12,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp580,_tmp57F,_tag_dyneither(_tmp11,sizeof(void*),0U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 110
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U,_tmp4EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);_tmp4EF;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,({
struct _dyneither_ptr _tmp581=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4EE.f1=_tmp581;});_tmp4EE;});void*_tmp13[2U];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16;({unsigned int _tmp583=e->loc;struct _dyneither_ptr _tmp582=({const char*_tmp14="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp14,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp583,_tmp582,_tag_dyneither(_tmp13,sizeof(void*),2U));});});
# 119
{void*_tmp17=e->r;void*_tmp18=_tmp17;enum Cyc_Absyn_Primop _tmp1F;struct Cyc_List_List*_tmp1E;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->tag == 3U){_LL1: _tmp1F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f1;_tmp1E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f2;_LL2:
# 121
 if(_tmp1F == Cyc_Absyn_Eq  || _tmp1F == Cyc_Absyn_Neq){
struct _tuple0 _tmp19=({struct _tuple0 _tmp4F1;_tmp4F1.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E))->hd)->topt),_tmp4F1.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E->tl))->hd)->topt);_tmp4F1;});struct _tuple0 _tmp1A=_tmp19;void*_tmp1D;void*_tmp1C;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->tag == 15U){_LL6: _tmp1D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->f1;_tmp1C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->f1;_LL7:
# 124
 return({struct Cyc_Tcexp_TestEnv _tmp4F0;({struct _tuple0*_tmp584=({struct _tuple0*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->f1=_tmp1D,_tmp1B->f2=_tmp1C;_tmp1B;});_tmp4F0.eq=_tmp584;}),_tmp4F0.isTrue=_tmp1F == Cyc_Absyn_Eq;_tmp4F0;});}else{goto _LL8;}}else{_LL8: _LL9:
# 126
 goto _LL5;}_LL5:;}
# 129
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 132
return({struct Cyc_Tcexp_TestEnv _tmp4F2;_tmp4F2.eq=0,_tmp4F2.isTrue=0;_tmp4F2;});}
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
({void*_tmp585=Cyc_Absyn_wchar_typ();t=_tmp585;});goto _LL0;case 4U: _LL9: _tmp50=((_tmp21.Short_c).val).f1;_LLA:
# 168
 t=_tmp50 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL0;case 6U: _LLB: _tmp51=((_tmp21.LongLong_c).val).f1;_LLC:
# 170
 t=_tmp51 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL0;case 7U: _LLD: _tmp52=((_tmp21.Float_c).val).f2;_LLE:
# 172
 if(topt == 0)({void*_tmp586=Cyc_Absyn_float_typ(_tmp52);t=_tmp586;});else{
# 176
void*_tmp22=Cyc_Tcutil_compress(*topt);void*_tmp23=_tmp22;int _tmp24;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp23)->tag == 7U)switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp23)->f1){case 0U: _LL18: _LL19:
({void*_tmp587=Cyc_Absyn_float_typ(0);t=_tmp587;});goto _LL17;case 1U: _LL1A: _LL1B:
({void*_tmp588=Cyc_Absyn_float_typ(1);t=_tmp588;});goto _LL17;default: _LL1C: _tmp24=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp23)->f1;_LL1D:
({void*_tmp589=Cyc_Absyn_float_typ(_tmp24);t=_tmp589;});goto _LL17;}else{_LL1E: _LL1F:
# 182
({void*_tmp58A=Cyc_Absyn_float_typ(_tmp52);t=_tmp58A;});goto _LL17;}_LL17:;}
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
({union Cyc_Absyn_Cnst _tmp58B=Cyc_Absyn_Char_c(_tmp30,(char)_tmp53);*c=_tmp58B;});
goto _LL20;case Cyc_Absyn_Short_sz: _LL23: _tmp31=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f1;_LL24:
# 204
 t=_tmp31 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp58C=Cyc_Absyn_Short_c(_tmp31,(short)_tmp53);*c=_tmp58C;});
goto _LL20;case Cyc_Absyn_Int_sz: _LL25: _tmp32=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f1;_LL26:
# 208
 t=_tmp32 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 211
({union Cyc_Absyn_Cnst _tmp58D=Cyc_Absyn_Int_c(_tmp32,_tmp53);*c=_tmp58D;});
goto _LL20;case Cyc_Absyn_Long_sz: _LL27: _tmp33=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp26)->f1;_LL28:
# 214
 t=_tmp33 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 217
({union Cyc_Absyn_Cnst _tmp58E=Cyc_Absyn_Int_c(_tmp33,_tmp53);*c=_tmp58E;});
goto _LL20;default: goto _LL2D;}case 5U: _LL29: _tmp39=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).elt_typ;_tmp38=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).elt_tq;_tmp37=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).rgn;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).nullable;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).bounds;_tmp34=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).zero_term;if(_tmp53 == 0){_LL2A: {
# 221
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp36))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({
struct _dyneither_ptr _tmp58F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4F3.f1=_tmp58F;});_tmp4F3;});void*_tmp28[1U];_tmp28[0]=& _tmp2A;({unsigned int _tmp591=e->loc;struct _dyneither_ptr _tmp590=({const char*_tmp29="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp29,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp591,_tmp590,_tag_dyneither(_tmp28,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp2B=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp592=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=5U,(_tmp2C->f1).elt_typ=_tmp39,(_tmp2C->f1).elt_tq=_tmp38,
((_tmp2C->f1).ptr_atts).rgn=_tmp37,((_tmp2C->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp2C->f1).ptr_atts).bounds=_tmp35,((_tmp2C->f1).ptr_atts).zero_term=_tmp34,((_tmp2C->f1).ptr_atts).ptrloc=0;_tmp2C;});
# 227
t=_tmp592;});
# 229
goto _LL20;};}}else{goto _LL2D;}case 19U: _LL2B: _tmp3A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp26)->f1;_LL2C: {
# 231
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=18U,({struct Cyc_Absyn_Exp*_tmp593=Cyc_Absyn_uint_exp((unsigned int)_tmp53,0U);_tmp2F->f1=_tmp593;});_tmp2F;});
# 238
({void*_tmp594=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=19U,_tmp2E->f1=(void*)_tmp2D;_tmp2E;});t=_tmp594;});
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
({int _tmp595=Cyc_Tcexp_wchar_numelts(_tmp56);string_numelts=_tmp595;});
({void*_tmp596=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp596;});}{
# 253
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp597=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp597,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 258
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3B=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=1U,_tmp3F->f1=elen;_tmp3F;});
({void*_tmp59B=({void*_tmp59A=string_elt_typ;struct Cyc_Absyn_Tqual _tmp599=Cyc_Absyn_const_tqual(0U);void*_tmp598=(void*)_tmp3B;Cyc_Absyn_atb_typ(_tmp59A,(void*)& Cyc_Absyn_HeapRgn_val,_tmp599,_tmp598,Cyc_Absyn_true_conref);});t=_tmp59B;});
# 261
if(topt != 0){
void*_tmp3C=Cyc_Tcutil_compress(*topt);void*_tmp3D=_tmp3C;struct Cyc_Absyn_Tqual _tmp3E;switch(*((int*)_tmp3D)){case 8U: _LL37: _tmp3E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).tq;_LL38:
# 266
 return({void*_tmp59E=string_elt_typ;struct Cyc_Absyn_Tqual _tmp59D=_tmp3E;struct Cyc_Absyn_Exp*_tmp59C=elen;Cyc_Absyn_array_typ(_tmp59E,_tmp59D,_tmp59C,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0U);});case 5U: _LL39: _LL3A:
# 268
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 274
({void*_tmp5A3=({void*_tmp5A2=string_elt_typ;void*_tmp5A1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp5A0=
Cyc_Absyn_const_tqual(0U);
# 274
void*_tmp59F=(void*)_tmp3B;Cyc_Absyn_atb_typ(_tmp5A2,_tmp5A1,_tmp5A0,_tmp59F,Cyc_Absyn_true_conref);});t=_tmp5A3;});
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
({struct Cyc_String_pa_PrintArg_struct _tmp44=({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U,({
struct _dyneither_ptr _tmp5A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4F4.f1=_tmp5A4;});_tmp4F4;});void*_tmp42[1U];_tmp42[0]=& _tmp44;({unsigned int _tmp5A6=e->loc;struct _dyneither_ptr _tmp5A5=({const char*_tmp43="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp43,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp5A6,_tmp5A5,_tag_dyneither(_tmp42,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->tag=5U,(_tmp45->f1).elt_typ=_tmp4B,(_tmp45->f1).elt_tq=_tmp4A,((_tmp45->f1).ptr_atts).rgn=_tmp49,((_tmp45->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp45->f1).ptr_atts).bounds=_tmp47,((_tmp45->f1).ptr_atts).zero_term=_tmp46,((_tmp45->f1).ptr_atts).ptrloc=0;_tmp45;});}else{_LL40: _LL41:
# 297
 goto _LL3D;}_LL3D:;}{
# 300
struct Cyc_List_List*_tmp4C=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp5AC=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=5U,({void*_tmp5AB=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->v=_tmp4C;_tmp4D;}));(_tmp4F->f1).elt_typ=_tmp5AB;}),({
struct Cyc_Absyn_Tqual _tmp5AA=Cyc_Absyn_empty_tqual(0U);(_tmp4F->f1).elt_tq=_tmp5AA;}),
({void*_tmp5A9=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->v=_tmp4C;_tmp4E;}));((_tmp4F->f1).ptr_atts).rgn=_tmp5A9;}),((_tmp4F->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 305
union Cyc_Absyn_Constraint*_tmp5A8=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4F->f1).ptr_atts).bounds=_tmp5A8;}),({union Cyc_Absyn_Constraint*_tmp5A7=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4F->f1).ptr_atts).zero_term=_tmp5A7;}),((_tmp4F->f1).ptr_atts).ptrloc=0;_tmp4F;});
# 301
t=_tmp5AC;});
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
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp5AD=({const char*_tmp59="unresolved binding in tcVar";_tag_dyneither(_tmp59,sizeof(char),28U);});_tmp5A->f1=_tmp5AD;});_tmp5A;}));case 1U: _LL3: _tmp60=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp58)->f1;_LL4:
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
({void*_tmp5AE=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=39U,_tmp5D->f1=_tmp5E;_tmp5D;});e->r=_tmp5AE;});
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
({struct Cyc_List_List*_tmp5AF=type_getter(te,(struct _dyneither_ptr)_tmp6B,fmt->loc);desc_types=_tmp5AF;});goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 365
 if(opt_args != 0){
struct Cyc_List_List*_tmp67=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp67 != 0;_tmp67=_tmp67->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp67->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp67->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp67->hd))
({void*_tmp68=0U;({unsigned int _tmp5B1=((struct Cyc_Absyn_Exp*)_tmp67->hd)->loc;struct _dyneither_ptr _tmp5B0=({const char*_tmp69="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp69,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp5B1,_tmp5B0,_tag_dyneither(_tmp68,sizeof(void*),0U));});});}}
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
({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0U,({
struct _dyneither_ptr _tmp5B2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F6.f1=_tmp5B2;});_tmp4F6;});struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp4F5;_tmp4F5.tag=0U,({struct _dyneither_ptr _tmp5B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4F5.f1=_tmp5B3;});_tmp4F5;});void*_tmp6D[2U];_tmp6D[0]=& _tmp6F,_tmp6D[1]=& _tmp70;({unsigned int _tmp5B5=e->loc;struct _dyneither_ptr _tmp5B4=({const char*_tmp6E="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp6E,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp5B5,_tmp5B4,_tag_dyneither(_tmp6D,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 390
if(alias_coercion)
({struct Cyc_List_List*_tmp5B6=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=(void*)arg_cnt,_tmp71->tl=*alias_arg_exps;_tmp71;});*alias_arg_exps=_tmp5B6;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp72=0U;({unsigned int _tmp5B8=((struct Cyc_Absyn_Exp*)_tmp6C->hd)->loc;struct _dyneither_ptr _tmp5B7=({const char*_tmp73="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp73,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp5B8,_tmp5B7,_tag_dyneither(_tmp72,sizeof(void*),0U));});});}
# 397
if(desc_types != 0)
({void*_tmp74=0U;({unsigned int _tmp5BA=fmt->loc;struct _dyneither_ptr _tmp5B9=({const char*_tmp75="too few arguments";_tag_dyneither(_tmp75,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp5BA,_tmp5B9,_tag_dyneither(_tmp74,sizeof(void*),0U));});});
if(_tmp6C != 0){
({void*_tmp76=0U;({unsigned int _tmp5BC=((struct Cyc_Absyn_Exp*)_tmp6C->hd)->loc;struct _dyneither_ptr _tmp5BB=({const char*_tmp77="too many arguments";_tag_dyneither(_tmp77,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp5BC,_tmp5BB,_tag_dyneither(_tmp76,sizeof(void*),0U));});});
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
({struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U,({struct _dyneither_ptr _tmp5BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F7.f1=_tmp5BD;});_tmp4F7;});void*_tmp79[1U];_tmp79[0]=& _tmp7B;({unsigned int _tmp5BF=loc;struct _dyneither_ptr _tmp5BE=({const char*_tmp7A="expecting numeric type but found %s";_tag_dyneither(_tmp7A,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5BF,_tmp5BE,_tag_dyneither(_tmp79,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 417
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U,({struct _dyneither_ptr _tmp5C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F8.f1=_tmp5C0;});_tmp4F8;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp5C2=loc;struct _dyneither_ptr _tmp5C1=({const char*_tmp7D="expecting integral or * type but found %s";_tag_dyneither(_tmp7D,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp5C2,_tmp5C1,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 422
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp81=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,({struct _dyneither_ptr _tmp5C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F9.f1=_tmp5C3;});_tmp4F9;});void*_tmp7F[1U];_tmp7F[0]=& _tmp81;({unsigned int _tmp5C5=loc;struct _dyneither_ptr _tmp5C4=({const char*_tmp80="expecting integral type but found %s";_tag_dyneither(_tmp80,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp5C5,_tmp5C4,_tag_dyneither(_tmp7F,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 426
{void*_tmp82=t;union Cyc_Absyn_Constraint*_tmp8B;switch(*((int*)_tmp82)){case 8U: _LLE: _LLF:
 goto _LLD;case 5U: _LL10: _tmp8B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82)->f1).ptr_atts).bounds;_LL11:
# 429
{void*_tmp83=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8B);void*_tmp84=_tmp83;struct Cyc_Absyn_Exp*_tmp87;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp84)->tag == 0U){_LL15: _LL16:
 goto _LL14;}else{_LL17: _tmp87=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp84)->f1;_LL18:
# 432
 if(!Cyc_Evexp_c_can_eval(_tmp87) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp85=0U;({unsigned int _tmp5C7=loc;struct _dyneither_ptr _tmp5C6=({const char*_tmp86="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp86,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp5C7,_tmp5C6,_tag_dyneither(_tmp85,sizeof(void*),0U));});});
goto _LL14;}_LL14:;}
# 436
goto _LLD;default: _LL12: _LL13:
# 438
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,({struct _dyneither_ptr _tmp5C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4FA.f1=_tmp5C8;});_tmp4FA;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({unsigned int _tmp5CA=loc;struct _dyneither_ptr _tmp5C9=({const char*_tmp89="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp89,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp5CA,_tmp5C9,_tag_dyneither(_tmp88,sizeof(void*),1U));});});}_LLD:;}
# 440
return Cyc_Absyn_uint_typ;default: _LLB: _LLC:
({void*_tmp8C=0U;({struct _dyneither_ptr _tmp5CB=({const char*_tmp8D="Non-unary primop";_tag_dyneither(_tmp8D,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5CB,_tag_dyneither(_tmp8C,sizeof(void*),0U));});});}_LL0:;}
# 446
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 449
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,({struct _dyneither_ptr _tmp5CC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4FB.f1=_tmp5CC;});_tmp4FB;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({unsigned int _tmp5CE=e1->loc;struct _dyneither_ptr _tmp5CD=({const char*_tmp8F="type %s cannot be used here";_tag_dyneither(_tmp8F,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5CE,_tmp5CD,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 453
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,({struct _dyneither_ptr _tmp5CF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4FC.f1=_tmp5CF;});_tmp4FC;});void*_tmp91[1U];_tmp91[0]=& _tmp93;({unsigned int _tmp5D1=e2->loc;struct _dyneither_ptr _tmp5D0=({const char*_tmp92="type %s cannot be used here";_tag_dyneither(_tmp92,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5D1,_tmp5D0,_tag_dyneither(_tmp91,sizeof(void*),1U));});});
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
({void*_tmp95=0U;({unsigned int _tmp5D3=e1->loc;struct _dyneither_ptr _tmp5D2=({const char*_tmp96="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp96,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5D3,_tmp5D2,_tag_dyneither(_tmp95,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp97=0U;({unsigned int _tmp5D5=e1->loc;struct _dyneither_ptr _tmp5D4=({const char*_tmp98="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp98,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5D5,_tmp5D4,_tag_dyneither(_tmp97,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmp9B=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,({struct _dyneither_ptr _tmp5D6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4FD.f1=_tmp5D6;});_tmp4FD;});void*_tmp99[1U];_tmp99[0]=& _tmp9B;({unsigned int _tmp5D8=e2->loc;struct _dyneither_ptr _tmp5D7=({const char*_tmp9A="expecting int but found %s";_tag_dyneither(_tmp9A,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5D8,_tmp5D7,_tag_dyneither(_tmp99,sizeof(void*),1U));});});{
void*_tmp9C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA8);void*_tmp9D=_tmp9C;struct Cyc_Absyn_Exp*_tmpA6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9D)->tag == 0U){_LL6: _LL7:
 return t1;}else{_LL8: _tmpA6=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9D)->f1;_LL9:
# 478
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA7)){
struct _tuple15 _tmp9E=Cyc_Evexp_eval_const_uint_exp(_tmpA6);struct _tuple15 _tmp9F=_tmp9E;unsigned int _tmpA3;int _tmpA2;_LLB: _tmpA3=_tmp9F.f1;_tmpA2=_tmp9F.f2;_LLC:;
if(_tmpA2  && _tmpA3 == 1)
({void*_tmpA0=0U;({unsigned int _tmp5DA=e1->loc;struct _dyneither_ptr _tmp5D9=({const char*_tmpA1="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpA1,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5DA,_tmp5D9,_tag_dyneither(_tmpA0,sizeof(void*),0U));});});}{
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
({struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0U,({
# 511
struct _dyneither_ptr _tmp5DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FF.f1=_tmp5DB;});_tmp4FF;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,({struct _dyneither_ptr _tmp5DC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4FE.f1=_tmp5DC;});_tmp4FE;});void*_tmpAD[2U];_tmpAD[0]=& _tmpAF,_tmpAD[1]=& _tmpB0;({unsigned int _tmp5DE=e1->loc;struct _dyneither_ptr _tmp5DD=({const char*_tmpAE="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpAE,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5DE,_tmp5DD,_tag_dyneither(_tmpAD,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 514
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp5DF=t1_elt;Cyc_Tcutil_unify(_tmp5DF,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpB3=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,({
# 519
struct _dyneither_ptr _tmp5E0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp501.f1=_tmp5E0;});_tmp501;});struct Cyc_String_pa_PrintArg_struct _tmpB4=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,({struct _dyneither_ptr _tmp5E1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp500.f1=_tmp5E1;});_tmp500;});void*_tmpB1[2U];_tmpB1[0]=& _tmpB3,_tmpB1[1]=& _tmpB4;({unsigned int _tmp5E3=e1->loc;struct _dyneither_ptr _tmp5E2=({const char*_tmpB2="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpB2,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5E3,_tmp5E2,_tag_dyneither(_tmpB1,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 523
({void*_tmpB5=0U;({unsigned int _tmp5E5=e1->loc;struct _dyneither_ptr _tmp5E4=({const char*_tmpB6="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpB6,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5E5,_tmp5E4,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5EA=te;struct Cyc_Absyn_Exp*_tmp5E9=e1;Cyc_Tcutil_unchecked_cast(_tmp5EA,_tmp5E9,({void*_tmp5E8=t1_elt;void*_tmp5E7=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5E6=
Cyc_Absyn_empty_tqual(0U);
# 524
Cyc_Absyn_star_typ(_tmp5E8,_tmp5E7,_tmp5E6,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 527
return Cyc_Absyn_sint_typ;}else{
# 529
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpB7=0U;({unsigned int _tmp5EC=e1->loc;struct _dyneither_ptr _tmp5EB=({const char*_tmpB8="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpB8,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5EC,_tmp5EB,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpB9=0U;({unsigned int _tmp5EE=e1->loc;struct _dyneither_ptr _tmp5ED=({const char*_tmpBA="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpBA,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5EE,_tmp5ED,_tag_dyneither(_tmpB9,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpBD=({struct Cyc_String_pa_PrintArg_struct _tmp503;_tmp503.tag=0U,({
struct _dyneither_ptr _tmp5EF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp503.f1=_tmp5EF;});_tmp503;});struct Cyc_String_pa_PrintArg_struct _tmpBE=({struct Cyc_String_pa_PrintArg_struct _tmp502;_tmp502.tag=0U,({struct _dyneither_ptr _tmp5F0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp502.f1=_tmp5F0;});_tmp502;});void*_tmpBB[2U];_tmpBB[0]=& _tmpBD,_tmpBB[1]=& _tmpBE;({unsigned int _tmp5F2=e2->loc;struct _dyneither_ptr _tmp5F1=({const char*_tmpBC="expecting either %s or int but found %s";_tag_dyneither(_tmpBC,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5F2,_tmp5F1,_tag_dyneither(_tmpBB,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 538
return t1;}}}
# 542
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5F3=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5F3,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpBF=0U;({unsigned int _tmp5F5=e1->loc;struct _dyneither_ptr _tmp5F4=({const char*_tmpC0="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC0,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5F5,_tmp5F4,_tag_dyneither(_tmpBF,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5FA=te;struct Cyc_Absyn_Exp*_tmp5F9=e2;Cyc_Tcutil_unchecked_cast(_tmp5FA,_tmp5F9,({void*_tmp5F8=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5F7=
Cyc_Tcutil_pointer_region(t2);
# 547
struct Cyc_Absyn_Tqual _tmp5F6=
# 549
Cyc_Absyn_empty_tqual(0U);
# 547
Cyc_Absyn_star_typ(_tmp5F8,_tmp5F7,_tmp5F6,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 552
({void*_tmpC1=0U;({unsigned int _tmp5FC=e1->loc;struct _dyneither_ptr _tmp5FB=({const char*_tmpC2="thin pointer subtraction!";_tag_dyneither(_tmpC2,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5FC,_tmp5FB,_tag_dyneither(_tmpC1,sizeof(void*),0U));});});
return Cyc_Absyn_sint_typ;}
# 555
({void*_tmpC3=0U;({unsigned int _tmp5FE=e1->loc;struct _dyneither_ptr _tmp5FD=({const char*_tmpC4="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpC4,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5FE,_tmp5FD,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 558
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpC5=0U;({unsigned int _tmp600=e1->loc;struct _dyneither_ptr _tmp5FF=({const char*_tmpC6="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpC6,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp600,_tmp5FF,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});
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
void*_tmp601=t1;Cyc_Tcutil_unify(_tmp601,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
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
struct _tuple0 _tmpC7=({struct _tuple0 _tmp504;({void*_tmp603=Cyc_Tcutil_compress(t1);_tmp504.f1=_tmp603;}),({void*_tmp602=Cyc_Tcutil_compress(t2);_tmp504.f2=_tmp602;});_tmp504;});struct _tuple0 _tmpC8=_tmpC7;void*_tmpCA;void*_tmpC9;switch(*((int*)_tmpC8.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8.f2)->tag == 5U){_LL1: _tmpCA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8.f1)->f1).elt_typ;_tmpC9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8.f2)->f1).elt_typ;_LL2:
# 596
 if(Cyc_Tcutil_unify(_tmpCA,_tmpC9))return Cyc_Absyn_sint_typ;goto _LL0;}else{goto _LL5;}case 15U: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC8.f2)->tag == 15U){_LL3: _LL4:
# 598
 return Cyc_Absyn_sint_typ;}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 601
({struct Cyc_String_pa_PrintArg_struct _tmpCD=({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U,({
struct _dyneither_ptr _tmp604=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp506.f1=_tmp604;});_tmp506;});struct Cyc_String_pa_PrintArg_struct _tmpCE=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({struct _dyneither_ptr _tmp605=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp505.f1=_tmp605;});_tmp505;});void*_tmpCB[2U];_tmpCB[0]=& _tmpCD,_tmpCB[1]=& _tmpCE;({unsigned int _tmp607=loc;struct _dyneither_ptr _tmp606=({const char*_tmpCC="comparison not allowed between %s and %s";_tag_dyneither(_tmpCC,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp607,_tmp606,_tag_dyneither(_tmpCB,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 608
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 610
if(({void*_tmp608=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp608 == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 616
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpCF=({struct _tuple0 _tmp507;_tmp507.f1=t1,_tmp507.f2=t2;_tmp507;});struct _tuple0 _tmpD0=_tmpCF;void*_tmpD2;void*_tmpD1;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f2)->tag == 15U){_LL1: _tmpD2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f1)->f1;_tmpD1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD0.f2)->f1;_LL2:
# 621
 return Cyc_Absyn_sint_typ;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 624
({struct Cyc_String_pa_PrintArg_struct _tmpD5=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({
struct _dyneither_ptr _tmp609=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp509.f1=_tmp609;});_tmp509;});struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({struct _dyneither_ptr _tmp60A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp508.f1=_tmp60A;});_tmp508;});void*_tmpD3[2U];_tmpD3[0]=& _tmpD5,_tmpD3[1]=& _tmpD6;({unsigned int _tmp60C=loc;struct _dyneither_ptr _tmp60B=({const char*_tmpD4="comparison not allowed between %s and %s";_tag_dyneither(_tmpD4,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp60C,_tmp60B,_tag_dyneither(_tmpD3,sizeof(void*),2U));});});
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
({void*_tmpD8=0U;({struct _dyneither_ptr _tmp60D=({const char*_tmpD9="bad binary primop";_tag_dyneither(_tmpD9,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp60D,_tag_dyneither(_tmpD8,sizeof(void*),0U));});});}_LL0:;}
# 663
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 671
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpDA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpDB=Cyc_Tcexp_tcExp(te,topt,_tmpDA);
if(!Cyc_Tcutil_is_numeric(_tmpDA))
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0U,({struct _dyneither_ptr _tmp60E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpDB));_tmp50A.f1=_tmp60E;});_tmp50A;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({unsigned int _tmp610=_tmpDA->loc;struct _dyneither_ptr _tmp60F=({const char*_tmpDD="expecting numeric type but found %s";_tag_dyneither(_tmpDD,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp610,_tmp60F,_tag_dyneither(_tmpDC,sizeof(void*),1U));});});
return _tmpDB;}
# 678
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpDF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpE0=_tmpDF;switch(_tmpE0){case 0U: _LL1: _LL2:
 return({void*_tmpE1=0U;({struct Cyc_Tcenv_Tenv*_tmp614=te;unsigned int _tmp613=loc;void**_tmp612=topt;struct _dyneither_ptr _tmp611=({const char*_tmpE2="primitive operator has 0 arguments";_tag_dyneither(_tmpE2,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp614,_tmp613,_tmp612,_tmp611,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
({void*_tmp615=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp615;});goto _LL0;case 2U: _LL5: _LL6:
({void*_tmp61B=({struct Cyc_Tcenv_Tenv*_tmp61A=te;unsigned int _tmp619=loc;void**_tmp618=topt;enum Cyc_Absyn_Primop _tmp617=p;struct Cyc_Absyn_Exp*_tmp616=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp61A,_tmp619,_tmp618,_tmp617,_tmp616,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp61B;});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpE3=0U;({struct Cyc_Tcenv_Tenv*_tmp61F=te;unsigned int _tmp61E=loc;void**_tmp61D=topt;struct _dyneither_ptr _tmp61C=({const char*_tmpE4="primitive operator has > 2 arguments";_tag_dyneither(_tmpE4,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp61F,_tmp61E,_tmp61D,_tmp61C,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});}_LL0:;}
# 686
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 689
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpE5=Cyc_Tcutil_compress(t);
void*_tmpE6=_tmpE5;struct Cyc_List_List*_tmp102;void*_tmp101;struct Cyc_Absyn_Tqual _tmp100;struct Cyc_Absyn_Datatypefield*_tmpFF;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Aggrdecl*_tmpFD;switch(*((int*)_tmpE6)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE6)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmpFD=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE6)->f1).aggr_info).KnownAggr).val;_LL2:
# 693
 if(_tmpFD->impl == 0){
({void*_tmpE7=0U;({unsigned int _tmp621=loc;struct _dyneither_ptr _tmp620=({const char*_tmpE8="attempt to write an abstract aggregate";_tag_dyneither(_tmpE8,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp621,_tmp620,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});
return 0;}else{
# 697
_tmpFE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFD->impl))->fields;goto _LL4;}}else{goto _LLB;}case 12U: _LL3: _tmpFE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE6)->f2;_LL4:
# 699
 for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct Cyc_Absyn_Aggrfield*_tmpE9=(struct Cyc_Absyn_Aggrfield*)_tmpFE->hd;
if((_tmpE9->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpEC=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,_tmp50B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE9->name);_tmp50B;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEC;({unsigned int _tmp623=loc;struct _dyneither_ptr _tmp622=({const char*_tmpEB="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpEB,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp623,_tmp622,_tag_dyneither(_tmpEA,sizeof(void*),1U));});});
return 0;}
# 705
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpE9->type))return 0;}
# 707
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE6)->f1).field_info).KnownDatatypefield).tag == 2){_LL5: _tmpFF=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE6)->f1).field_info).KnownDatatypefield).val).f2;_LL6:
# 709
{struct Cyc_List_List*_tmpED=_tmpFF->typs;for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _tuple17*_tmpEE=(struct _tuple17*)_tmpED->hd;struct _tuple17*_tmpEF=_tmpEE;struct Cyc_Absyn_Tqual _tmpF4;void*_tmpF3;_LLE: _tmpF4=_tmpEF->f1;_tmpF3=_tmpEF->f2;_LLF:;
if(_tmpF4.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0U,({struct _dyneither_ptr _tmp624=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpFF->name));_tmp50C.f1=_tmp624;});_tmp50C;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF2;({unsigned int _tmp626=loc;struct _dyneither_ptr _tmp625=({const char*_tmpF1="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpF1,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp626,_tmp625,_tag_dyneither(_tmpF0,sizeof(void*),1U));});});
return 0;}
# 715
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF3))return 0;}}
# 717
return 1;}else{goto _LLB;}case 8U: _LL7: _tmp101=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE6)->f1).elt_type;_tmp100=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE6)->f1).tq;_LL8:
# 719
 if(_tmp100.real_const){
({void*_tmpF5=0U;({unsigned int _tmp628=loc;struct _dyneither_ptr _tmp627=({const char*_tmpF6="attempt to over-write a const array";_tag_dyneither(_tmpF6,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp628,_tmp627,_tag_dyneither(_tmpF5,sizeof(void*),0U));});});
return 0;}
# 723
return Cyc_Tcexp_check_writable_aggr(loc,_tmp101);case 10U: _LL9: _tmp102=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE6)->f1;_LLA:
# 725
 for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
struct _tuple17*_tmpF7=(struct _tuple17*)_tmp102->hd;struct _tuple17*_tmpF8=_tmpF7;struct Cyc_Absyn_Tqual _tmpFC;void*_tmpFB;_LL11: _tmpFC=_tmpF8->f1;_tmpFB=_tmpF8->f2;_LL12:;
if(_tmpFC.real_const){
({void*_tmpF9=0U;({unsigned int _tmp62A=loc;struct _dyneither_ptr _tmp629=({const char*_tmpFA="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmpFA,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp62A,_tmp629,_tag_dyneither(_tmpF9,sizeof(void*),0U));});});
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
{void*_tmp103=e->r;void*_tmp104=_tmp103;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp131;struct _dyneither_ptr*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct _dyneither_ptr*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Vardecl*_tmp12B;struct Cyc_Absyn_Vardecl*_tmp12A;struct Cyc_Absyn_Vardecl*_tmp129;struct Cyc_Absyn_Vardecl*_tmp128;switch(*((int*)_tmp104)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)){case 3U: _LL1: _tmp128=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL2:
 _tmp129=_tmp128;goto _LL4;case 4U: _LL3: _tmp129=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL4:
 _tmp12A=_tmp129;goto _LL6;case 5U: _LL5: _tmp12A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL6:
 _tmp12B=_tmp12A;goto _LL8;case 1U: _LL7: _tmp12B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp104)->f1)->f1;_LL8:
 if(!(_tmp12B->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp12D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_tmp12C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp104)->f2;_LLA:
# 751
{void*_tmp105=Cyc_Tcutil_compress((void*)_check_null(_tmp12D->topt));void*_tmp106=_tmp105;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Tqual _tmp116;struct Cyc_Absyn_Tqual _tmp115;switch(*((int*)_tmp106)){case 5U: _LL18: _tmp115=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp106)->f1).elt_tq;_LL19:
 _tmp116=_tmp115;goto _LL1B;case 8U: _LL1A: _tmp116=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp106)->f1).tq;_LL1B:
 if(!_tmp116.real_const)return;goto _LL17;case 10U: _LL1C: _tmp117=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp106)->f1;_LL1D: {
# 755
struct _tuple15 _tmp107=Cyc_Evexp_eval_const_uint_exp(_tmp12C);struct _tuple15 _tmp108=_tmp107;unsigned int _tmp114;int _tmp113;_LL21: _tmp114=_tmp108.f1;_tmp113=_tmp108.f2;_LL22:;
if(!_tmp113){
({void*_tmp109=0U;({unsigned int _tmp62C=e->loc;struct _dyneither_ptr _tmp62B=({const char*_tmp10A="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp10A,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp62C,_tmp62B,_tag_dyneither(_tmp109,sizeof(void*),0U));});});
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
goto _LL0;case 21U: _LLB: _tmp12F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_tmp12E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp104)->f2;_LLC:
# 769
{void*_tmp118=Cyc_Tcutil_compress((void*)_check_null(_tmp12F->topt));void*_tmp119=_tmp118;struct Cyc_List_List*_tmp11B;struct Cyc_Absyn_Aggrdecl**_tmp11A;switch(*((int*)_tmp119)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp119)->f1).aggr_info).KnownAggr).tag == 2){_LL2C: _tmp11A=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp119)->f1).aggr_info).KnownAggr).val;_LL2D: {
# 771
struct Cyc_Absyn_Aggrfield*sf=
_tmp11A == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp11A,_tmp12E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}case 12U: _LL2E: _tmp11B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp119)->f2;_LL2F: {
# 776
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp11B,_tmp12E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 781
goto _LL0;case 22U: _LLD: _tmp131=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_tmp130=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp104)->f2;_LLE:
# 783
{void*_tmp11C=Cyc_Tcutil_compress((void*)_check_null(_tmp131->topt));void*_tmp11D=_tmp11C;void*_tmp123;struct Cyc_Absyn_Tqual _tmp122;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11D)->tag == 5U){_LL33: _tmp123=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11D)->f1).elt_typ;_tmp122=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11D)->f1).elt_tq;_LL34:
# 785
 if(!_tmp122.real_const){
void*_tmp11E=Cyc_Tcutil_compress(_tmp123);void*_tmp11F=_tmp11E;struct Cyc_List_List*_tmp121;struct Cyc_Absyn_Aggrdecl**_tmp120;switch(*((int*)_tmp11F)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11F)->f1).aggr_info).KnownAggr).tag == 2){_LL38: _tmp120=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11F)->f1).aggr_info).KnownAggr).val;_LL39: {
# 788
struct Cyc_Absyn_Aggrfield*sf=
_tmp120 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp120,_tmp130);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}case 12U: _LL3A: _tmp121=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11F)->f2;_LL3B: {
# 793
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp121,_tmp130);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 799
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 802
goto _LL0;case 20U: _LLF: _tmp132=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_LL10:
# 804
{void*_tmp124=Cyc_Tcutil_compress((void*)_check_null(_tmp132->topt));void*_tmp125=_tmp124;struct Cyc_Absyn_Tqual _tmp127;struct Cyc_Absyn_Tqual _tmp126;switch(*((int*)_tmp125)){case 5U: _LL3F: _tmp126=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp125)->f1).elt_tq;_LL40:
 _tmp127=_tmp126;goto _LL42;case 8U: _LL41: _tmp127=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp125)->f1).tq;_LL42:
 if(!_tmp127.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 809
goto _LL0;case 12U: _LL11: _tmp133=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_LL12:
 _tmp134=_tmp133;goto _LL14;case 13U: _LL13: _tmp134=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp104)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp134);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 814
({struct Cyc_String_pa_PrintArg_struct _tmp137=({struct Cyc_String_pa_PrintArg_struct _tmp50D;_tmp50D.tag=0U,({struct _dyneither_ptr _tmp62D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp50D.f1=_tmp62D;});_tmp50D;});void*_tmp135[1U];_tmp135[0]=& _tmp137;({unsigned int _tmp62F=e->loc;struct _dyneither_ptr _tmp62E=({const char*_tmp136="attempt to write a const location: %s";_tag_dyneither(_tmp136,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp62F,_tmp62E,_tag_dyneither(_tmp135,sizeof(void*),1U));});});}
# 817
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 820
({struct Cyc_Tcenv_Tenv*_tmp630=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp630,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp138=0U;({struct Cyc_Tcenv_Tenv*_tmp634=te;unsigned int _tmp633=loc;void**_tmp632=topt;struct _dyneither_ptr _tmp631=({const char*_tmp139="increment/decrement of non-lvalue";_tag_dyneither(_tmp139,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp634,_tmp633,_tmp632,_tmp631,_tag_dyneither(_tmp138,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 826
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp13A=0U;({unsigned int _tmp636=e->loc;struct _dyneither_ptr _tmp635=({const char*_tmp13B="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp13B,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp636,_tmp635,_tag_dyneither(_tmp13A,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp13C=0U;({unsigned int _tmp638=e->loc;struct _dyneither_ptr _tmp637=({const char*_tmp13D="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp13D,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp638,_tmp637,_tag_dyneither(_tmp13C,sizeof(void*),0U));});});}else{
# 835
({struct Cyc_String_pa_PrintArg_struct _tmp140=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0U,({struct _dyneither_ptr _tmp639=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp50E.f1=_tmp639;});_tmp50E;});void*_tmp13E[1U];_tmp13E[0]=& _tmp140;({unsigned int _tmp63B=e->loc;struct _dyneither_ptr _tmp63A=({const char*_tmp13F="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp13F,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp63B,_tmp63A,_tag_dyneither(_tmp13E,sizeof(void*),1U));});});}}
# 837
return t;};}
# 841
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 843
({struct Cyc_Tcenv_Tenv*_tmp63D=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp63C=e1;Cyc_Tcexp_tcTest(_tmp63D,_tmp63C,({const char*_tmp141="conditional expression";_tag_dyneither(_tmp141,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp63E=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp63E;});else{
# 850
({void*_tmp63F=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp63F;});}{
struct Cyc_List_List _tmp142=({struct Cyc_List_List _tmp512;_tmp512.hd=e3,_tmp512.tl=0;_tmp512;});
struct Cyc_List_List _tmp143=({struct Cyc_List_List _tmp511;_tmp511.hd=e2,_tmp511.tl=& _tmp142;_tmp511;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp143)){
({struct Cyc_String_pa_PrintArg_struct _tmp146=({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0U,({
struct _dyneither_ptr _tmp640=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp510.f1=_tmp640;});_tmp510;});struct Cyc_String_pa_PrintArg_struct _tmp147=({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0U,({struct _dyneither_ptr _tmp641=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp50F.f1=_tmp641;});_tmp50F;});void*_tmp144[2U];_tmp144[0]=& _tmp146,_tmp144[1]=& _tmp147;({unsigned int _tmp643=loc;struct _dyneither_ptr _tmp642=({const char*_tmp145="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp145,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp643,_tmp642,_tag_dyneither(_tmp144,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 858
return t;};};}
# 862
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 864
({struct Cyc_Tcenv_Tenv*_tmp645=te;struct Cyc_Absyn_Exp*_tmp644=e1;Cyc_Tcexp_tcTest(_tmp645,_tmp644,({const char*_tmp148="logical-and expression";_tag_dyneither(_tmp148,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp647=te;struct Cyc_Absyn_Exp*_tmp646=e2;Cyc_Tcexp_tcTest(_tmp647,_tmp646,({const char*_tmp149="logical-and expression";_tag_dyneither(_tmp149,sizeof(char),23U);}));});
return Cyc_Absyn_sint_typ;}
# 870
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 872
({struct Cyc_Tcenv_Tenv*_tmp649=te;struct Cyc_Absyn_Exp*_tmp648=e1;Cyc_Tcexp_tcTest(_tmp649,_tmp648,({const char*_tmp14A="logical-or expression";_tag_dyneither(_tmp14A,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp64B=te;struct Cyc_Absyn_Exp*_tmp64A=e2;Cyc_Tcexp_tcTest(_tmp64B,_tmp64A,({const char*_tmp14B="logical-or expression";_tag_dyneither(_tmp14B,sizeof(char),22U);}));});
return Cyc_Absyn_sint_typ;}
# 878
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 885
({struct Cyc_Tcenv_Tenv*_tmp64C=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp64C,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 890
{void*_tmp14C=Cyc_Tcutil_compress(t1);void*_tmp14D=_tmp14C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14D)->tag == 8U){_LL1: _LL2:
({void*_tmp14E=0U;({unsigned int _tmp64E=loc;struct _dyneither_ptr _tmp64D=({const char*_tmp14F="cannot assign to an array";_tag_dyneither(_tmp14F,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp64E,_tmp64D,_tag_dyneither(_tmp14E,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 895
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp150=0U;({unsigned int _tmp650=loc;struct _dyneither_ptr _tmp64F=({const char*_tmp151="type is abstract (can't determine size).";_tag_dyneither(_tmp151,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp650,_tmp64F,_tag_dyneither(_tmp150,sizeof(void*),0U));});});
# 899
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp152=0U;({struct Cyc_Tcenv_Tenv*_tmp654=te;unsigned int _tmp653=loc;void**_tmp652=topt;struct _dyneither_ptr _tmp651=({const char*_tmp153="assignment to non-lvalue";_tag_dyneither(_tmp153,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp654,_tmp653,_tmp652,_tmp651,_tag_dyneither(_tmp152,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp154=0U;({unsigned int _tmp656=e2->loc;struct _dyneither_ptr _tmp655=({const char*_tmp155="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp155,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp656,_tmp655,_tag_dyneither(_tmp154,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp156=({struct Cyc_String_pa_PrintArg_struct _tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,({
struct _dyneither_ptr _tmp657=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp514.f1=_tmp657;});_tmp514;});struct Cyc_String_pa_PrintArg_struct _tmp15A=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,({struct _dyneither_ptr _tmp658=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp513.f1=_tmp658;});_tmp513;});void*_tmp157[2U];_tmp157[0]=& _tmp159,_tmp157[1]=& _tmp15A;({struct Cyc_Tcenv_Tenv*_tmp65C=te;unsigned int _tmp65B=loc;void**_tmp65A=topt;struct _dyneither_ptr _tmp659=({const char*_tmp158="type mismatch: %s != %s";_tag_dyneither(_tmp158,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp65C,_tmp65B,_tmp65A,_tmp659,_tag_dyneither(_tmp157,sizeof(void*),2U));});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp156;}}else{
# 913
enum Cyc_Absyn_Primop _tmp15B=(enum Cyc_Absyn_Primop)po->v;
void*_tmp15C=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp15B,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp15C,t1) || Cyc_Tcutil_coerceable(_tmp15C) && Cyc_Tcutil_coerceable(t1))){
void*_tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp160=({struct Cyc_String_pa_PrintArg_struct _tmp516;_tmp516.tag=0U,({
# 918
struct _dyneither_ptr _tmp65D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp516.f1=_tmp65D;});_tmp516;});struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,({
struct _dyneither_ptr _tmp65E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp515.f1=_tmp65E;});_tmp515;});void*_tmp15E[2U];_tmp15E[0]=& _tmp160,_tmp15E[1]=& _tmp161;({struct Cyc_Tcenv_Tenv*_tmp662=te;unsigned int _tmp661=loc;void**_tmp660=topt;struct _dyneither_ptr _tmp65F=({const char*_tmp15F="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp15F,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp662,_tmp661,_tmp660,_tmp65F,_tag_dyneither(_tmp15E,sizeof(void*),2U));});});
Cyc_Tcutil_unify(_tmp15C,t1);
Cyc_Tcutil_explain_failure();
return _tmp15D;}
# 924
return _tmp15C;}
# 926
return t1;};};}
# 930
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp663=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp663,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp665=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp664=topt;Cyc_Tcexp_tcExp(_tmp665,_tmp664,e2);});
return(void*)_check_null(e2->topt);}
# 937
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 940
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 943
{void*_tmp162=Cyc_Tcutil_compress(t1);void*_tmp163=_tmp162;switch(*((int*)_tmp163)){case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp163)->f1 == 0){_LL1: _LL2:
({struct Cyc_Tcenv_Tenv*_tmp667=te;struct Cyc_Absyn_Exp*_tmp666=e;Cyc_Tcutil_unchecked_cast(_tmp667,_tmp666,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp668=Cyc_Absyn_float_typ(1);t1=_tmp668;});goto _LL0;}else{goto _LL7;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp163)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL0;default: goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 950
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp164=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp165=_tmp164;struct _tuple1*_tmp169;struct Cyc_List_List*_tmp168;unsigned int _tmp167;enum Cyc_Absyn_Scope _tmp166;_LLA: _tmp169=_tmp165.name;_tmp168=_tmp165.typs;_tmp167=_tmp165.loc;_tmp166=_tmp165.sc;_LLB:;
# 953
if(_tmp168 == 0  || _tmp168->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp168->hd)).f2);
# 956
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 961
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp16A=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp16B=_tmp16A;struct _tuple1*_tmp16F;struct Cyc_List_List*_tmp16E;unsigned int _tmp16D;enum Cyc_Absyn_Scope _tmp16C;_LLD: _tmp16F=_tmp16B.name;_tmp16E=_tmp16B.typs;_tmp16D=_tmp16B.loc;_tmp16C=_tmp16B.sc;_LLE:;
# 964
if(_tmp16E == 0  || _tmp16E->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp16E->hd)).f2);
# 968
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 973
({struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0U,({
struct _dyneither_ptr _tmp669=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp518.f1=_tmp669;});_tmp518;});struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp517;_tmp517.tag=0U,({struct _dyneither_ptr _tmp66A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp517.f1=_tmp66A;});_tmp517;});void*_tmp170[2U];_tmp170[0]=& _tmp172,_tmp170[1]=& _tmp173;({unsigned int _tmp66C=e->loc;struct _dyneither_ptr _tmp66B=({const char*_tmp171="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp171,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp66C,_tmp66B,_tag_dyneither(_tmp170,sizeof(void*),2U));});});
return 0;}
# 979
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 985
struct Cyc_List_List*_tmp174=args;
int _tmp175=0;
struct Cyc_Tcenv_Tenv*_tmp176=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp177=Cyc_Tcenv_clear_abstract_val_ok(_tmp176);
Cyc_Tcexp_tcExp(_tmp177,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 994
void*_tmp178=t;void*_tmp1DB;struct Cyc_Absyn_Tqual _tmp1DA;void*_tmp1D9;union Cyc_Absyn_Constraint*_tmp1D8;union Cyc_Absyn_Constraint*_tmp1D7;union Cyc_Absyn_Constraint*_tmp1D6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->tag == 5U){_LL1: _tmp1DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).elt_typ;_tmp1DA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).elt_tq;_tmp1D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).ptr_atts).rgn;_tmp1D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).ptr_atts).nullable;_tmp1D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).ptr_atts).bounds;_tmp1D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp178)->f1).ptr_atts).zero_term;_LL2:
# 999
 Cyc_Tcenv_check_rgn_accessible(_tmp177,loc,_tmp1D9);
# 1001
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1D7);{
void*_tmp179=Cyc_Tcutil_compress(_tmp1DB);void*_tmp17A=_tmp179;struct Cyc_List_List*_tmp1D2;void*_tmp1D1;struct Cyc_Absyn_Tqual _tmp1D0;void*_tmp1CF;struct Cyc_List_List*_tmp1CE;int _tmp1CD;struct Cyc_Absyn_VarargInfo*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_List_List*_tmp1C6;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->tag == 9U){_LL6: _tmp1D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).tvars;_tmp1D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).effect;_tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).ret_tqual;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).ret_typ;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).args;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).c_varargs;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).cyc_varargs;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).rgn_po;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).attributes;_tmp1C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).requires_clause;_tmp1C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).requires_relns;_tmp1C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).ensures_clause;_tmp1C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17A)->f1).ensures_relns;_LL7:
# 1006
 if(_tmp1D2 != 0  || _tmp1CB != 0)
({void*_tmp17B=0U;({unsigned int _tmp66E=e->loc;struct _dyneither_ptr _tmp66D=({const char*_tmp17C="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp17C,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp66E,_tmp66D,_tag_dyneither(_tmp17B,sizeof(void*),0U));});});
# 1010
if(topt != 0)Cyc_Tcutil_unify(_tmp1CF,*topt);
# 1012
while(_tmp174 != 0  && _tmp1CE != 0){
# 1014
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp174->hd;
void*t2=(*((struct _tuple9*)_tmp1CE->hd)).f3;
Cyc_Tcexp_tcExp(_tmp177,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp177,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp18C="actual argument has type ";_tag_dyneither(_tmp18C,sizeof(char),26U);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp18B=" but formal has type ";_tag_dyneither(_tmp18B,sizeof(char),22U);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp671=({unsigned long _tmp670=({unsigned long _tmp66F=Cyc_strlen((struct _dyneither_ptr)s0);_tmp66F + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp670 + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp671 + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp17D=0U;({unsigned int _tmp674=e1->loc;struct _dyneither_ptr _tmp673=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp180=({struct Cyc_String_pa_PrintArg_struct _tmp51C;_tmp51C.tag=0U,_tmp51C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp51C;});struct Cyc_String_pa_PrintArg_struct _tmp181=({struct Cyc_String_pa_PrintArg_struct _tmp51B;_tmp51B.tag=0U,_tmp51B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp51B;});struct Cyc_String_pa_PrintArg_struct _tmp182=({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0U,_tmp51A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp51A;});struct Cyc_String_pa_PrintArg_struct _tmp183=({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0U,_tmp519.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp519;});void*_tmp17E[4U];_tmp17E[0]=& _tmp180,_tmp17E[1]=& _tmp181,_tmp17E[2]=& _tmp182,_tmp17E[3]=& _tmp183;({struct _dyneither_ptr _tmp672=({const char*_tmp17F="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp17F,sizeof(char),15U);});Cyc_aprintf(_tmp672,_tag_dyneither(_tmp17E,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp674,_tmp673,_tag_dyneither(_tmp17D,sizeof(void*),0U));});});else{
# 1026
({void*_tmp184=0U;({unsigned int _tmp677=e1->loc;struct _dyneither_ptr _tmp676=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp187=({struct Cyc_String_pa_PrintArg_struct _tmp520;_tmp520.tag=0U,_tmp520.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp520;});struct Cyc_String_pa_PrintArg_struct _tmp188=({struct Cyc_String_pa_PrintArg_struct _tmp51F;_tmp51F.tag=0U,_tmp51F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp51F;});struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp51E;_tmp51E.tag=0U,_tmp51E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp51E;});struct Cyc_String_pa_PrintArg_struct _tmp18A=({struct Cyc_String_pa_PrintArg_struct _tmp51D;_tmp51D.tag=0U,_tmp51D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp51D;});void*_tmp185[4U];_tmp185[0]=& _tmp187,_tmp185[1]=& _tmp188,_tmp185[2]=& _tmp189,_tmp185[3]=& _tmp18A;({struct _dyneither_ptr _tmp675=({const char*_tmp186="%s%s%s%s.";_tag_dyneither(_tmp186,sizeof(char),10U);});Cyc_aprintf(_tmp675,_tag_dyneither(_tmp185,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp677,_tmp676,_tag_dyneither(_tmp184,sizeof(void*),0U));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1031
if(alias_coercion)
({struct Cyc_List_List*_tmp678=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=(void*)_tmp175,_tmp18D->tl=*alias_arg_exps;_tmp18D;});*alias_arg_exps=_tmp678;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp18E=0U;({unsigned int _tmp67A=e1->loc;struct _dyneither_ptr _tmp679=({const char*_tmp18F="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp18F,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp67A,_tmp679,_tag_dyneither(_tmp18E,sizeof(void*),0U));});});
_tmp174=_tmp174->tl;
_tmp1CE=_tmp1CE->tl;
++ _tmp175;}{
# 1042
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1CA;for(0;a != 0;a=a->tl){
void*_tmp190=(void*)a->hd;void*_tmp191=_tmp190;enum Cyc_Absyn_Format_Type _tmp19E;int _tmp19D;int _tmp19C;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp191)->tag == 19U){_LLB: _tmp19E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp191)->f1;_tmp19D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp191)->f2;_tmp19C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp191)->f3;_LLC:
# 1046
{struct _handler_cons _tmp192;_push_handler(& _tmp192);{int _tmp194=0;if(setjmp(_tmp192.handler))_tmp194=1;if(!_tmp194){
# 1048
{struct Cyc_Absyn_Exp*_tmp195=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp19D - 1);
# 1050
struct Cyc_Core_Opt*fmt_args;
if(_tmp19C == 0)
fmt_args=0;else{
# 1054
({struct Cyc_Core_Opt*_tmp67C=({struct Cyc_Core_Opt*_tmp196=_cycalloc(sizeof(*_tmp196));({struct Cyc_List_List*_tmp67B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp19C - 1);_tmp196->v=_tmp67B;});_tmp196;});fmt_args=_tmp67C;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp197=_tmp19E;if(_tmp197 == Cyc_Absyn_Printf_ft){_LL10: _LL11:
# 1058
 Cyc_Tcexp_check_format_args(_tmp177,_tmp195,fmt_args,_tmp19C - 1,alias_arg_exps,Cyc_Formatstr_get_format_typs);
# 1061
goto _LLF;}else{_LL12: _LL13:
# 1063
 Cyc_Tcexp_check_format_args(_tmp177,_tmp195,fmt_args,_tmp19C - 1,alias_arg_exps,Cyc_Formatstr_get_scanf_typs);
# 1066
goto _LLF;}_LLF:;};}
# 1048
;_pop_handler();}else{void*_tmp193=(void*)_exn_thrown;void*_tmp198=_tmp193;void*_tmp19B;if(((struct Cyc_List_Nth_exn_struct*)_tmp198)->tag == Cyc_List_Nth){_LL15: _LL16:
# 1068
({void*_tmp199=0U;({unsigned int _tmp67E=loc;struct _dyneither_ptr _tmp67D=({const char*_tmp19A="bad format arguments";_tag_dyneither(_tmp19A,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp67E,_tmp67D,_tag_dyneither(_tmp199,sizeof(void*),0U));});});goto _LL14;}else{_LL17: _tmp19B=_tmp198;_LL18:(int)_rethrow(_tmp19B);}_LL14:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1073
if(_tmp1CE != 0)({void*_tmp19F=0U;({unsigned int _tmp680=loc;struct _dyneither_ptr _tmp67F=({const char*_tmp1A0="too few arguments for function";_tag_dyneither(_tmp1A0,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp680,_tmp67F,_tag_dyneither(_tmp19F,sizeof(void*),0U));});});else{
# 1075
if((_tmp174 != 0  || _tmp1CD) || _tmp1CC != 0){
if(_tmp1CD)
for(0;_tmp174 != 0;_tmp174=_tmp174->tl){
Cyc_Tcexp_tcExp(_tmp177,0,(struct Cyc_Absyn_Exp*)_tmp174->hd);}else{
if(_tmp1CC == 0)
({void*_tmp1A1=0U;({unsigned int _tmp682=loc;struct _dyneither_ptr _tmp681=({const char*_tmp1A2="too many arguments for function";_tag_dyneither(_tmp1A2,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp682,_tmp681,_tag_dyneither(_tmp1A1,sizeof(void*),0U));});});else{
# 1082
struct Cyc_Absyn_VarargInfo _tmp1A3=*_tmp1CC;struct Cyc_Absyn_VarargInfo _tmp1A4=_tmp1A3;void*_tmp1C2;int _tmp1C1;_LL1A: _tmp1C2=_tmp1A4.type;_tmp1C1=_tmp1A4.inject;_LL1B:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1A5=({struct Cyc_Absyn_VarargCallInfo*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->num_varargs=0,_tmp1C0->injectors=0,_tmp1C0->vai=_tmp1CC;_tmp1C0;});
# 1086
*vararg_call_info=_tmp1A5;
# 1088
if(!_tmp1C1)
# 1090
for(0;_tmp174 != 0;(_tmp174=_tmp174->tl,_tmp175 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp174->hd;
++ _tmp1A5->num_varargs;
Cyc_Tcexp_tcExp(_tmp177,& _tmp1C2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp177,e1,_tmp1C2,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1A8=({struct Cyc_String_pa_PrintArg_struct _tmp522;_tmp522.tag=0U,({
struct _dyneither_ptr _tmp683=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C2));_tmp522.f1=_tmp683;});_tmp522;});struct Cyc_String_pa_PrintArg_struct _tmp1A9=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0U,({struct _dyneither_ptr _tmp684=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp521.f1=_tmp684;});_tmp521;});void*_tmp1A6[2U];_tmp1A6[0]=& _tmp1A8,_tmp1A6[1]=& _tmp1A9;({unsigned int _tmp686=loc;struct _dyneither_ptr _tmp685=({const char*_tmp1A7="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1A7,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp686,_tmp685,_tag_dyneither(_tmp1A6,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 1100
if(alias_coercion)
({struct Cyc_List_List*_tmp687=({struct Cyc_List_List*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->hd=(void*)_tmp175,_tmp1AA->tl=*alias_arg_exps;_tmp1AA;});*alias_arg_exps=_tmp687;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1C2) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1AB=0U;({unsigned int _tmp689=e1->loc;struct _dyneither_ptr _tmp688=({const char*_tmp1AC="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1AC,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp689,_tmp688,_tag_dyneither(_tmp1AB,sizeof(void*),0U));});});}else{
# 1109
void*_tmp1AD=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1C2));void*_tmp1AE=_tmp1AD;struct Cyc_Absyn_Datatypedecl*_tmp1BF;struct Cyc_List_List*_tmp1BE;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AE)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AE)->f1).datatype_info).KnownDatatype).tag == 2){_LL1D: _tmp1BF=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AE)->f1).datatype_info).KnownDatatype).val;_tmp1BE=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1AE)->f1).targs;_LL1E: {
# 1113
struct Cyc_Absyn_Datatypedecl*_tmp1AF=*Cyc_Tcenv_lookup_datatypedecl(_tmp177,loc,_tmp1BF->name);
struct Cyc_List_List*fields=0;
if(_tmp1AF->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B2=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0U,({
struct _dyneither_ptr _tmp68A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C2));_tmp523.f1=_tmp68A;});_tmp523;});void*_tmp1B0[1U];_tmp1B0[0]=& _tmp1B2;({unsigned int _tmp68C=loc;struct _dyneither_ptr _tmp68B=({const char*_tmp1B1="can't inject into abstract datatype %s";_tag_dyneither(_tmp1B1,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp68C,_tmp68B,_tag_dyneither(_tmp1B0,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AF->fields))->v;}
# 1124
if(!({void*_tmp68D=Cyc_Tcutil_pointer_region(_tmp1C2);Cyc_Tcutil_unify(_tmp68D,Cyc_Tcenv_curr_rgn(_tmp177));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1B5=({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0U,({
struct _dyneither_ptr _tmp68E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C2));_tmp525.f1=_tmp68E;});_tmp525;});struct Cyc_String_pa_PrintArg_struct _tmp1B6=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,({struct _dyneither_ptr _tmp68F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp177)));_tmp524.f1=_tmp68F;});_tmp524;});void*_tmp1B3[2U];_tmp1B3[0]=& _tmp1B5,_tmp1B3[1]=& _tmp1B6;({unsigned int _tmp691=loc;struct _dyneither_ptr _tmp690=({const char*_tmp1B4="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1B4,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp691,_tmp690,_tag_dyneither(_tmp1B3,sizeof(void*),2U));});});{
# 1128
struct Cyc_List_List*_tmp1B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1AF->tvs,_tmp1BE);
for(0;_tmp174 != 0;_tmp174=_tmp174->tl){
++ _tmp1A5->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp174->hd;
# 1133
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp177,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1B8=0U;({unsigned int _tmp693=e1->loc;struct _dyneither_ptr _tmp692=({const char*_tmp1B9="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1B9,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp693,_tmp692,_tag_dyneither(_tmp1B8,sizeof(void*),0U));});});}{
# 1139
struct Cyc_Absyn_Datatypefield*_tmp1BA=({struct Cyc_Tcenv_Tenv*_tmp697=_tmp177;struct Cyc_Absyn_Exp*_tmp696=e1;void*_tmp695=Cyc_Tcutil_pointer_elt_type(_tmp1C2);struct Cyc_List_List*_tmp694=_tmp1B7;Cyc_Tcexp_tcInjection(_tmp697,_tmp696,_tmp695,_tmp694,fields);});
if(_tmp1BA != 0)
({struct Cyc_List_List*_tmp699=({
struct Cyc_List_List*_tmp698=_tmp1A5->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp698,({struct Cyc_List_List*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));
_tmp1BB->hd=_tmp1BA,_tmp1BB->tl=0;_tmp1BB;}));});
# 1141
_tmp1A5->injectors=_tmp699;});};};}
# 1145
goto _LL1C;};}}else{goto _LL1F;}}else{_LL1F: _LL20:
({void*_tmp1BC=0U;({unsigned int _tmp69B=loc;struct _dyneither_ptr _tmp69A=({const char*_tmp1BD="bad inject vararg type";_tag_dyneither(_tmp1BD,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp69B,_tmp69A,_tag_dyneither(_tmp1BC,sizeof(void*),0U));});});goto _LL1C;}_LL1C:;}};}}}}
# 1151
if(*alias_arg_exps == 0)
# 1160 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp177,loc,(void*)_check_null(_tmp1D1));
# 1162
return _tmp1CF;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1C5=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,({struct _dyneither_ptr _tmp69C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp526.f1=_tmp69C;});_tmp526;});void*_tmp1C3[1U];_tmp1C3[0]=& _tmp1C5;({struct Cyc_Tcenv_Tenv*_tmp6A0=_tmp177;unsigned int _tmp69F=loc;void**_tmp69E=topt;struct _dyneither_ptr _tmp69D=({const char*_tmp1C4="expected pointer to function but found %s";_tag_dyneither(_tmp1C4,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp6A0,_tmp69F,_tmp69E,_tmp69D,_tag_dyneither(_tmp1C3,sizeof(void*),1U));});});}_LL5:;};}else{_LL3: _LL4:
# 1165
 return({struct Cyc_String_pa_PrintArg_struct _tmp1D5=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,({struct _dyneither_ptr _tmp6A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp527.f1=_tmp6A1;});_tmp527;});void*_tmp1D3[1U];_tmp1D3[0]=& _tmp1D5;({struct Cyc_Tcenv_Tenv*_tmp6A5=_tmp177;unsigned int _tmp6A4=loc;void**_tmp6A3=topt;struct _dyneither_ptr _tmp6A2=({const char*_tmp1D4="expected pointer to function but found %s";_tag_dyneither(_tmp1D4,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp6A5,_tmp6A4,_tmp6A3,_tmp6A2,_tag_dyneither(_tmp1D3,sizeof(void*),1U));});});}_LL0:;};}
# 1169
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1DC=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1DC,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1DC,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1DF=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,({struct _dyneither_ptr _tmp6A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1DC));_tmp529.f1=_tmp6A6;});_tmp529;});struct Cyc_String_pa_PrintArg_struct _tmp1E0=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0U,({
struct _dyneither_ptr _tmp6A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp528.f1=_tmp6A7;});_tmp528;});void*_tmp1DD[2U];_tmp1DD[0]=& _tmp1DF,_tmp1DD[1]=& _tmp1E0;({unsigned int _tmp6A9=loc;struct _dyneither_ptr _tmp6A8=({const char*_tmp1DE="expected %s but found %s";_tag_dyneither(_tmp1DE,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp6A9,_tmp6A8,_tag_dyneither(_tmp1DD,sizeof(void*),2U));});});
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1181
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1183
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1185
({void*_tmp6AA=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp6AA;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1E1=t1;void*_tmp1FC;struct Cyc_Absyn_Tqual _tmp1FB;void*_tmp1FA;union Cyc_Absyn_Constraint*_tmp1F9;union Cyc_Absyn_Constraint*_tmp1F8;union Cyc_Absyn_Constraint*_tmp1F7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E1)->tag == 5U){_LL1: _tmp1FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E1)->f1).elt_typ;_tmp1FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E1)->f1).elt_tq;_tmp1FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E1)->f1).ptr_atts).rgn;_tmp1F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E1)->f1).ptr_atts).nullable;_tmp1F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E1)->f1).ptr_atts).bounds;_tmp1F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E1)->f1).ptr_atts).zero_term;_LL2:
# 1189
{void*_tmp1E2=Cyc_Tcutil_compress(_tmp1FC);void*_tmp1E3=_tmp1E2;struct Cyc_List_List*_tmp1F6;void*_tmp1F5;struct Cyc_Absyn_Tqual _tmp1F4;void*_tmp1F3;struct Cyc_List_List*_tmp1F2;int _tmp1F1;struct Cyc_Absyn_VarargInfo*_tmp1F0;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_List_List*_tmp1EA;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->tag == 9U){_LL6: _tmp1F6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).tvars;_tmp1F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).effect;_tmp1F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).ret_tqual;_tmp1F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).ret_typ;_tmp1F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).args;_tmp1F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).c_varargs;_tmp1F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).cyc_varargs;_tmp1EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).rgn_po;_tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).attributes;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).requires_clause;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).requires_relns;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).ensures_clause;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1E3)->f1).ensures_relns;_LL7: {
# 1191
struct Cyc_List_List*instantiation=0;
# 1193
for(0;ts != 0  && _tmp1F6 != 0;(ts=ts->tl,_tmp1F6=_tmp1F6->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1F6->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp6AE=loc;struct Cyc_Tcenv_Tenv*_tmp6AD=te;struct Cyc_List_List*_tmp6AC=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6AB=k;Cyc_Tcutil_check_type(_tmp6AE,_tmp6AD,_tmp6AC,_tmp6AB,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp6B0=({struct Cyc_List_List*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));({struct _tuple13*_tmp6AF=({struct _tuple13*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));_tmp1E4->f1=(struct Cyc_Absyn_Tvar*)_tmp1F6->hd,_tmp1E4->f2=(void*)ts->hd;_tmp1E4;});_tmp1E5->hd=_tmp6AF;}),_tmp1E5->tl=instantiation;_tmp1E5;});instantiation=_tmp6B0;});}
# 1199
if(ts != 0)
return({void*_tmp1E6=0U;({struct Cyc_Tcenv_Tenv*_tmp6B4=te;unsigned int _tmp6B3=loc;void**_tmp6B2=topt;struct _dyneither_ptr _tmp6B1=({const char*_tmp1E7="too many type variables in instantiation";_tag_dyneither(_tmp1E7,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp6B4,_tmp6B3,_tmp6B2,_tmp6B1,_tag_dyneither(_tmp1E6,sizeof(void*),0U));});});
# 1204
if(_tmp1F6 == 0){
({struct Cyc_List_List*_tmp6B5=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1EF);_tmp1EF=_tmp6B5;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1EF);
_tmp1EF=0;}{
# 1209
void*new_fn_typ=({
struct Cyc_List_List*_tmp6B6=instantiation;Cyc_Tcutil_substitute(_tmp6B6,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));
_tmp1E9->tag=9U,(_tmp1E9->f1).tvars=_tmp1F6,(_tmp1E9->f1).effect=_tmp1F5,(_tmp1E9->f1).ret_tqual=_tmp1F4,(_tmp1E9->f1).ret_typ=_tmp1F3,(_tmp1E9->f1).args=_tmp1F2,(_tmp1E9->f1).c_varargs=_tmp1F1,(_tmp1E9->f1).cyc_varargs=_tmp1F0,(_tmp1E9->f1).rgn_po=_tmp1EF,(_tmp1E9->f1).attributes=_tmp1EE,(_tmp1E9->f1).requires_clause=_tmp1ED,(_tmp1E9->f1).requires_relns=_tmp1EC,(_tmp1E9->f1).ensures_clause=_tmp1EB,(_tmp1E9->f1).ensures_relns=_tmp1EA;_tmp1E9;}));});
# 1214
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->tag=5U,(_tmp1E8->f1).elt_typ=new_fn_typ,(_tmp1E8->f1).elt_tq=_tmp1FB,((_tmp1E8->f1).ptr_atts).rgn=_tmp1FA,((_tmp1E8->f1).ptr_atts).nullable=_tmp1F9,((_tmp1E8->f1).ptr_atts).bounds=_tmp1F8,((_tmp1E8->f1).ptr_atts).zero_term=_tmp1F7,((_tmp1E8->f1).ptr_atts).ptrloc=0;_tmp1E8;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1217
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1220
return({struct Cyc_String_pa_PrintArg_struct _tmp1FF=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0U,({
struct _dyneither_ptr _tmp6B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp52A.f1=_tmp6B7;});_tmp52A;});void*_tmp1FD[1U];_tmp1FD[0]=& _tmp1FF;({struct Cyc_Tcenv_Tenv*_tmp6BB=te;unsigned int _tmp6BA=loc;void**_tmp6B9=topt;struct _dyneither_ptr _tmp6B8=({const char*_tmp1FE="expecting polymorphic type but found %s";_tag_dyneither(_tmp1FE,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp6BB,_tmp6BA,_tmp6B9,_tmp6B8,_tag_dyneither(_tmp1FD,sizeof(void*),1U));});});};}
# 1225
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1227
({unsigned int _tmp6BF=loc;struct Cyc_Tcenv_Tenv*_tmp6BE=te;struct Cyc_List_List*_tmp6BD=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6BC=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1227
Cyc_Tcutil_check_type(_tmp6BF,_tmp6BE,_tmp6BD,_tmp6BC,1,0,t);});
# 1229
Cyc_Tcutil_check_no_qual(loc,t);
# 1231
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1236
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1243
Cyc_Tcutil_unify(t2,t);{
void*_tmp200=({struct Cyc_String_pa_PrintArg_struct _tmp203=({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0U,({
struct _dyneither_ptr _tmp6C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp52C.f1=_tmp6C0;});_tmp52C;});struct Cyc_String_pa_PrintArg_struct _tmp204=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0U,({struct _dyneither_ptr _tmp6C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp52B.f1=_tmp6C1;});_tmp52B;});void*_tmp201[2U];_tmp201[0]=& _tmp203,_tmp201[1]=& _tmp204;({struct Cyc_Tcenv_Tenv*_tmp6C4=te;unsigned int _tmp6C3=loc;struct _dyneither_ptr _tmp6C2=({const char*_tmp202="cannot cast %s to %s";_tag_dyneither(_tmp202,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6C4,_tmp6C3,& t,_tmp6C2,_tag_dyneither(_tmp201,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return _tmp200;};}}}
# 1253
{struct _tuple0 _tmp205=({struct _tuple0 _tmp52D;_tmp52D.f1=e->r,({void*_tmp6C5=Cyc_Tcutil_compress(t);_tmp52D.f2=_tmp6C5;});_tmp52D;});struct _tuple0 _tmp206=_tmp205;int _tmp20F;union Cyc_Absyn_Constraint*_tmp20E;union Cyc_Absyn_Constraint*_tmp20D;union Cyc_Absyn_Constraint*_tmp20C;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp206.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp206.f2)->tag == 5U){_LL1: _tmp20F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp206.f1)->f1).fat_result;_tmp20E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp206.f2)->f1).ptr_atts).nullable;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp206.f2)->f1).ptr_atts).bounds;_tmp20C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp206.f2)->f1).ptr_atts).zero_term;_LL2:
# 1257
 if((_tmp20F  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp20C)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp20E)){
void*_tmp207=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp20D);void*_tmp208=_tmp207;struct Cyc_Absyn_Exp*_tmp20B;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp208)->tag == 1U){_LL6: _tmp20B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp208)->f1;_LL7:
# 1260
 if((Cyc_Evexp_eval_const_uint_exp(_tmp20B)).f1 == 1)
({void*_tmp209=0U;({unsigned int _tmp6C7=loc;struct _dyneither_ptr _tmp6C6=({const char*_tmp20A="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp20A,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6C7,_tmp6C6,_tag_dyneither(_tmp209,sizeof(void*),0U));});});
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1266
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1269
return t;};}
# 1273
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp210=0;
struct Cyc_Absyn_Tqual _tmp211=Cyc_Absyn_empty_tqual(0U);
int _tmp212=0;
if(topt != 0){
void*_tmp213=Cyc_Tcutil_compress(*topt);void*_tmp214=_tmp213;void**_tmp217;struct Cyc_Absyn_Tqual _tmp216;union Cyc_Absyn_Constraint*_tmp215;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->tag == 5U){_LL1: _tmp217=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).elt_typ;_tmp216=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).elt_tq;_tmp215=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp214)->f1).ptr_atts).nullable;_LL2:
# 1280
 _tmp210=_tmp217;
_tmp211=_tmp216;
({int _tmp6C8=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp215);_tmp212=_tmp6C8;});
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1294
({struct Cyc_Tcenv_Tenv*_tmp6CA=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp6C9=_tmp210;Cyc_Tcexp_tcExpNoInst(_tmp6CA,_tmp6C9,e);});
# 1297
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp218=0U;({unsigned int _tmp6CC=e->loc;struct _dyneither_ptr _tmp6CB=({const char*_tmp219="Cannot take the address of an alias-free path";_tag_dyneither(_tmp219,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6CC,_tmp6CB,_tag_dyneither(_tmp218,sizeof(void*),0U));});});
# 1303
{void*_tmp21A=e->r;void*_tmp21B=_tmp21A;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Exp*_tmp21E;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21B)->tag == 23U){_LL6: _tmp21F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21B)->f1;_tmp21E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21B)->f2;_LL7:
# 1305
{void*_tmp21C=Cyc_Tcutil_compress((void*)_check_null(_tmp21F->topt));void*_tmp21D=_tmp21C;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp21D)->tag == 10U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1310
({void*_tmp6CD=(Cyc_Absyn_add_exp(_tmp21F,_tmp21E,0U))->r;e0->r=_tmp6CD;});
return Cyc_Tcexp_tcPlus(te,_tmp21F,_tmp21E);}_LLA:;}
# 1313
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1318
{void*_tmp220=e->r;void*_tmp221=_tmp220;switch(*((int*)_tmp221)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp221)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp221)->f3 == 1){_LL12: _LL13:
# 1321
({void*_tmp222=0U;({unsigned int _tmp6CF=e->loc;struct _dyneither_ptr _tmp6CE=({const char*_tmp223="cannot take the address of a @tagged union member";_tag_dyneither(_tmp223,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6CF,_tmp6CE,_tag_dyneither(_tmp222,sizeof(void*),0U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1327
struct _tuple14 _tmp224=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp225=_tmp224;int _tmp229;void*_tmp228;_LL17: _tmp229=_tmp225.f1;_tmp228=_tmp225.f2;_LL18:;
# 1329
if(Cyc_Tcutil_is_noalias_region(_tmp228,0))
({void*_tmp226=0U;({unsigned int _tmp6D1=e->loc;struct _dyneither_ptr _tmp6D0=({const char*_tmp227="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp227,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp6D1,_tmp6D0,_tag_dyneither(_tmp226,sizeof(void*),0U));});});{
# 1332
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp229){
tq.print_const=1;
tq.real_const=1;}{
# 1338
void*t=_tmp212?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp228,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp228,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1345
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1349
return Cyc_Absyn_uint_typ;
# 1351
({unsigned int _tmp6D4=loc;struct Cyc_Tcenv_Tenv*_tmp6D3=te;struct Cyc_List_List*_tmp6D2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6D4,_tmp6D3,_tmp6D2,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp22C=({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0U,({
struct _dyneither_ptr _tmp6D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp52E.f1=_tmp6D5;});_tmp52E;});void*_tmp22A[1U];_tmp22A[0]=& _tmp22C;({unsigned int _tmp6D7=loc;struct _dyneither_ptr _tmp6D6=({const char*_tmp22B="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp22B,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp6D7,_tmp6D6,_tag_dyneither(_tmp22A,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp22D=Cyc_Tcutil_compress(*topt);void*_tmp22E=_tmp22D;void*_tmp233;void*_tmp232;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp22E)->tag == 19U){_LL1: _tmp233=_tmp22E;_tmp232=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp22E)->f1;_LL2: {
# 1360
struct Cyc_Absyn_Exp*_tmp22F=Cyc_Absyn_sizeoftyp_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp230=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp231=_cycalloc(sizeof(*_tmp231));_tmp231->tag=18U,_tmp231->f1=_tmp22F;_tmp231;});
if(Cyc_Tcutil_unify(_tmp232,(void*)_tmp230))return _tmp233;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1366
return Cyc_Absyn_uint_typ;}
# 1369
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1376
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1378
({unsigned int _tmp6DA=loc;struct Cyc_Tcenv_Tenv*_tmp6D9=te;struct Cyc_List_List*_tmp6D8=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6DA,_tmp6D9,_tmp6D8,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp234=fs;
void*_tmp235=t_orig;
for(0;_tmp234 != 0;_tmp234=_tmp234->tl){
void*_tmp236=(void*)_tmp234->hd;
void*_tmp237=_tmp236;unsigned int _tmp271;struct _dyneither_ptr*_tmp270;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp237)->tag == 0U){_LL1: _tmp270=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp237)->f1;_LL2: {
# 1386
int bad_type=1;
{void*_tmp238=Cyc_Tcutil_compress(_tmp235);void*_tmp239=_tmp238;struct Cyc_List_List*_tmp241;struct Cyc_Absyn_Aggrdecl**_tmp240;switch(*((int*)_tmp239)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp240=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp239)->f1).aggr_info).KnownAggr).val;_LL7:
# 1389
 if((*_tmp240)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp270,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp240)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp23C=({struct Cyc_String_pa_PrintArg_struct _tmp52F;_tmp52F.tag=0U,_tmp52F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp270);_tmp52F;});void*_tmp23A[1U];_tmp23A[0]=& _tmp23C;({unsigned int _tmp6DC=loc;struct _dyneither_ptr _tmp6DB=({const char*_tmp23B="no field of struct/union has name %s";_tag_dyneither(_tmp23B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6DC,_tmp6DB,_tag_dyneither(_tmp23A,sizeof(void*),1U));});});else{
# 1394
_tmp235=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}case 12U: _LL8: _tmp241=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp239)->f2;_LL9: {
# 1398
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp270,_tmp241);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp23F=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,_tmp530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp270);_tmp530;});void*_tmp23D[1U];_tmp23D[0]=& _tmp23F;({unsigned int _tmp6DE=loc;struct _dyneither_ptr _tmp6DD=({const char*_tmp23E="no field of struct/union has name %s";_tag_dyneither(_tmp23E,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6DE,_tmp6DD,_tag_dyneither(_tmp23D,sizeof(void*),1U));});});else{
# 1402
_tmp235=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1407
if(bad_type){
if(_tmp234 == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp244=({struct Cyc_String_pa_PrintArg_struct _tmp531;_tmp531.tag=0U,({struct _dyneither_ptr _tmp6DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp235));_tmp531.f1=_tmp6DF;});_tmp531;});void*_tmp242[1U];_tmp242[0]=& _tmp244;({unsigned int _tmp6E1=loc;struct _dyneither_ptr _tmp6E0=({const char*_tmp243="%s is not a known struct/union type";_tag_dyneither(_tmp243,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6E1,_tmp6E0,_tag_dyneither(_tmp242,sizeof(void*),1U));});});else{
# 1411
struct _dyneither_ptr _tmp245=({struct Cyc_String_pa_PrintArg_struct _tmp258=({struct Cyc_String_pa_PrintArg_struct _tmp538;_tmp538.tag=0U,({struct _dyneither_ptr _tmp6E2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp538.f1=_tmp6E2;});_tmp538;});void*_tmp256[1U];_tmp256[0]=& _tmp258;({struct _dyneither_ptr _tmp6E3=({const char*_tmp257="(%s)";_tag_dyneither(_tmp257,sizeof(char),5U);});Cyc_aprintf(_tmp6E3,_tag_dyneither(_tmp256,sizeof(void*),1U));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp234;x=x->tl){
void*_tmp246=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp247=_tmp246;unsigned int _tmp251;struct _dyneither_ptr*_tmp250;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp247)->tag == 0U){_LLD: _tmp250=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp247)->f1;_LLE:
# 1416
({struct _dyneither_ptr _tmp6E5=({struct Cyc_String_pa_PrintArg_struct _tmp24A=({struct Cyc_String_pa_PrintArg_struct _tmp533;_tmp533.tag=0U,_tmp533.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp245);_tmp533;});struct Cyc_String_pa_PrintArg_struct _tmp24B=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,_tmp532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp250);_tmp532;});void*_tmp248[2U];_tmp248[0]=& _tmp24A,_tmp248[1]=& _tmp24B;({struct _dyneither_ptr _tmp6E4=({const char*_tmp249="%s.%s";_tag_dyneither(_tmp249,sizeof(char),6U);});Cyc_aprintf(_tmp6E4,_tag_dyneither(_tmp248,sizeof(void*),2U));});});_tmp245=_tmp6E5;});goto _LLC;}else{_LLF: _tmp251=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp247)->f1;_LL10:
# 1418
({struct _dyneither_ptr _tmp6E7=({struct Cyc_String_pa_PrintArg_struct _tmp24E=({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U,_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp245);_tmp535;});struct Cyc_Int_pa_PrintArg_struct _tmp24F=({struct Cyc_Int_pa_PrintArg_struct _tmp534;_tmp534.tag=1U,_tmp534.f1=(unsigned long)((int)_tmp251);_tmp534;});void*_tmp24C[2U];_tmp24C[0]=& _tmp24E,_tmp24C[1]=& _tmp24F;({struct _dyneither_ptr _tmp6E6=({const char*_tmp24D="%s.%d";_tag_dyneither(_tmp24D,sizeof(char),6U);});Cyc_aprintf(_tmp6E6,_tag_dyneither(_tmp24C,sizeof(void*),2U));});});_tmp245=_tmp6E7;});goto _LLC;}_LLC:;}
# 1420
({struct Cyc_String_pa_PrintArg_struct _tmp254=({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0U,_tmp537.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp245);_tmp537;});struct Cyc_String_pa_PrintArg_struct _tmp255=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,({struct _dyneither_ptr _tmp6E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp235));_tmp536.f1=_tmp6E8;});_tmp536;});void*_tmp252[2U];_tmp252[0]=& _tmp254,_tmp252[1]=& _tmp255;({unsigned int _tmp6EA=loc;struct _dyneither_ptr _tmp6E9=({const char*_tmp253="%s == %s is not a struct/union type";_tag_dyneither(_tmp253,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6EA,_tmp6E9,_tag_dyneither(_tmp252,sizeof(void*),2U));});});}}
# 1423
goto _LL0;}}else{_LL3: _tmp271=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp237)->f1;_LL4: {
# 1425
int bad_type=1;
{void*_tmp259=Cyc_Tcutil_compress(_tmp235);void*_tmp25A=_tmp259;struct Cyc_Absyn_Datatypefield*_tmp26C;struct Cyc_List_List*_tmp26B;struct Cyc_List_List*_tmp26A;struct Cyc_Absyn_Aggrdecl**_tmp269;switch(*((int*)_tmp25A)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp25A)->f1).aggr_info).KnownAggr).tag == 2){_LL12: _tmp269=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp25A)->f1).aggr_info).KnownAggr).val;_LL13:
# 1428
 if((*_tmp269)->impl == 0)
goto _LL11;
_tmp26A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp269)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 12U: _LL14: _tmp26A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp25A)->f2;_LL15:
# 1432
 if(({int _tmp6EB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26A);_tmp6EB <= _tmp271;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp25D=({struct Cyc_Int_pa_PrintArg_struct _tmp53A;_tmp53A.tag=1U,({
unsigned long _tmp6EC=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26A);_tmp53A.f1=_tmp6EC;});_tmp53A;});struct Cyc_Int_pa_PrintArg_struct _tmp25E=({struct Cyc_Int_pa_PrintArg_struct _tmp539;_tmp539.tag=1U,_tmp539.f1=(unsigned long)((int)_tmp271);_tmp539;});void*_tmp25B[2U];_tmp25B[0]=& _tmp25D,_tmp25B[1]=& _tmp25E;({unsigned int _tmp6EE=loc;struct _dyneither_ptr _tmp6ED=({const char*_tmp25C="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp25C,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6EE,_tmp6ED,_tag_dyneither(_tmp25B,sizeof(void*),2U));});});else{
# 1436
({void*_tmp6EF=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp26A,(int)_tmp271))->type;_tmp235=_tmp6EF;});}
bad_type=0;
goto _LL11;case 10U: _LL16: _tmp26B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp25A)->f1;_LL17:
# 1440
 if(({int _tmp6F0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26B);_tmp6F0 <= _tmp271;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp261=({struct Cyc_Int_pa_PrintArg_struct _tmp53C;_tmp53C.tag=1U,({
unsigned long _tmp6F1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26B);_tmp53C.f1=_tmp6F1;});_tmp53C;});struct Cyc_Int_pa_PrintArg_struct _tmp262=({struct Cyc_Int_pa_PrintArg_struct _tmp53B;_tmp53B.tag=1U,_tmp53B.f1=(unsigned long)((int)_tmp271);_tmp53B;});void*_tmp25F[2U];_tmp25F[0]=& _tmp261,_tmp25F[1]=& _tmp262;({unsigned int _tmp6F3=loc;struct _dyneither_ptr _tmp6F2=({const char*_tmp260="tuple has too few components: %d <= %d";_tag_dyneither(_tmp260,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6F3,_tmp6F2,_tag_dyneither(_tmp25F,sizeof(void*),2U));});});else{
# 1444
({void*_tmp6F4=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp26B,(int)_tmp271)).f2;_tmp235=_tmp6F4;});}
bad_type=0;
goto _LL11;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp25A)->f1).field_info).KnownDatatypefield).tag == 2){_LL18: _tmp26C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp25A)->f1).field_info).KnownDatatypefield).val).f2;_LL19:
# 1448
 if(({int _tmp6F5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26C->typs);_tmp6F5 < _tmp271;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp265=({struct Cyc_Int_pa_PrintArg_struct _tmp53E;_tmp53E.tag=1U,({
unsigned long _tmp6F6=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp26C->typs);_tmp53E.f1=_tmp6F6;});_tmp53E;});struct Cyc_Int_pa_PrintArg_struct _tmp266=({struct Cyc_Int_pa_PrintArg_struct _tmp53D;_tmp53D.tag=1U,_tmp53D.f1=(unsigned long)((int)_tmp271);_tmp53D;});void*_tmp263[2U];_tmp263[0]=& _tmp265,_tmp263[1]=& _tmp266;({unsigned int _tmp6F8=loc;struct _dyneither_ptr _tmp6F7=({const char*_tmp264="datatype field has too few components: %d < %d";_tag_dyneither(_tmp264,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6F8,_tmp6F7,_tag_dyneither(_tmp263,sizeof(void*),2U));});});else{
# 1452
if(_tmp271 != 0)
({void*_tmp6F9=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp26C->typs,(int)(_tmp271 - 1))).f2;_tmp235=_tmp6F9;});else{
if(_tmp234->tl != 0)
({void*_tmp267=0U;({unsigned int _tmp6FB=loc;struct _dyneither_ptr _tmp6FA=({const char*_tmp268="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp268,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6FB,_tmp6FA,_tag_dyneither(_tmp267,sizeof(void*),0U));});});}}
# 1457
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: _LL1A: _LL1B:
 goto _LL11;}_LL11:;}
# 1461
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,({struct _dyneither_ptr _tmp6FC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp235));_tmp53F.f1=_tmp6FC;});_tmp53F;});void*_tmp26D[1U];_tmp26D[0]=& _tmp26F;({unsigned int _tmp6FE=loc;struct _dyneither_ptr _tmp6FD=({const char*_tmp26E="%s is not a known type";_tag_dyneither(_tmp26E,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6FE,_tmp6FD,_tag_dyneither(_tmp26D,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1466
return Cyc_Absyn_uint_typ;};}
# 1470
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp272=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp272,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp273=t;void*_tmp28A;void*_tmp289;union Cyc_Absyn_Constraint*_tmp288;union Cyc_Absyn_Constraint*_tmp287;switch(*((int*)_tmp273)){case 1U: _LL1: _LL2: {
# 1476
struct Cyc_List_List*_tmp274=Cyc_Tcenv_lookup_type_vars(_tmp272);
void*_tmp275=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->v=_tmp274;_tmp27D;}));
void*_tmp276=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->v=_tmp274;_tmp27C;}));
union Cyc_Absyn_Constraint*_tmp277=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp278=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp279=({struct Cyc_Absyn_PtrAtts _tmp540;_tmp540.rgn=_tmp276,({union Cyc_Absyn_Constraint*_tmp6FF=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp540.nullable=_tmp6FF;}),_tmp540.bounds=_tmp277,_tmp540.zero_term=_tmp278,_tmp540.ptrloc=0;_tmp540;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp27A=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->tag=5U,(_tmp27B->f1).elt_typ=_tmp275,({struct Cyc_Absyn_Tqual _tmp700=Cyc_Absyn_empty_tqual(0U);(_tmp27B->f1).elt_tq=_tmp700;}),(_tmp27B->f1).ptr_atts=_tmp279;_tmp27B;});
Cyc_Tcutil_unify(t,(void*)_tmp27A);
_tmp28A=_tmp275;_tmp289=_tmp276;_tmp288=_tmp277;_tmp287=_tmp278;goto _LL4;}case 5U: _LL3: _tmp28A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp273)->f1).elt_typ;_tmp289=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp273)->f1).ptr_atts).rgn;_tmp288=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp273)->f1).ptr_atts).bounds;_tmp287=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp273)->f1).ptr_atts).zero_term;_LL4:
# 1486
 Cyc_Tcenv_check_rgn_accessible(_tmp272,loc,_tmp289);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp288);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp28A),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp272)){
void*_tmp27E=Cyc_Tcutil_compress(_tmp28A);void*_tmp27F=_tmp27E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp27F)->tag == 9U){_LL8: _LL9:
# 1491
 if(Cyc_Tc_aggressive_warn)
({void*_tmp280=0U;({unsigned int _tmp702=loc;struct _dyneither_ptr _tmp701=({const char*_tmp281="unnecessary dereference for function type";_tag_dyneither(_tmp281,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp702,_tmp701,_tag_dyneither(_tmp280,sizeof(void*),0U));});});
return t;}else{_LLA: _LLB:
# 1495
({void*_tmp282=0U;({unsigned int _tmp704=loc;struct _dyneither_ptr _tmp703=({const char*_tmp283="can't dereference abstract pointer type";_tag_dyneither(_tmp283,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp704,_tmp703,_tag_dyneither(_tmp282,sizeof(void*),0U));});});}_LL7:;}
# 1498
return _tmp28A;default: _LL5: _LL6:
# 1500
 return({struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,({struct _dyneither_ptr _tmp705=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp541.f1=_tmp705;});_tmp541;});void*_tmp284[1U];_tmp284[0]=& _tmp286;({struct Cyc_Tcenv_Tenv*_tmp709=_tmp272;unsigned int _tmp708=loc;void**_tmp707=topt;struct _dyneither_ptr _tmp706=({const char*_tmp285="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp285,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp709,_tmp708,_tmp707,_tmp706,_tag_dyneither(_tmp284,sizeof(void*),1U));});});}_LL0:;};}
# 1505
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1509
({struct Cyc_Tcenv_Tenv*_tmp70A=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp70A,0,e);});
# 1511
({int _tmp70B=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp70B;});{
void*_tmp28B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp28C=_tmp28B;enum Cyc_Absyn_AggrKind _tmp2A6;struct Cyc_List_List*_tmp2A5;struct Cyc_Absyn_Aggrdecl*_tmp2A4;struct Cyc_List_List*_tmp2A3;switch(*((int*)_tmp28C)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp28C)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp2A4=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp28C)->f1).aggr_info).KnownAggr).val;_tmp2A3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp28C)->f1).targs;_LL2: {
# 1514
struct Cyc_Absyn_Aggrfield*_tmp28D=Cyc_Absyn_lookup_decl_field(_tmp2A4,f);
if(_tmp28D == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,({
struct _dyneither_ptr _tmp70C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2A4->name));_tmp543.f1=_tmp70C;});_tmp543;});struct Cyc_String_pa_PrintArg_struct _tmp291=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp542;});void*_tmp28E[2U];_tmp28E[0]=& _tmp290,_tmp28E[1]=& _tmp291;({struct Cyc_Tcenv_Tenv*_tmp710=te;unsigned int _tmp70F=loc;void**_tmp70E=topt;struct _dyneither_ptr _tmp70D=({const char*_tmp28F="%s has no %s member";_tag_dyneither(_tmp28F,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp710,_tmp70F,_tmp70E,_tmp70D,_tag_dyneither(_tmp28E,sizeof(void*),2U));});});
# 1519
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A4->impl))->tagged)*is_tagged=1;{
void*t2=_tmp28D->type;
if(_tmp2A3 != 0){
struct Cyc_List_List*_tmp292=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2A4->tvs,_tmp2A3);
({void*_tmp711=Cyc_Tcutil_substitute(_tmp292,_tmp28D->type);t2=_tmp711;});}
# 1527
if((((_tmp2A4->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A4->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp28D->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp295=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp544;});void*_tmp293[1U];_tmp293[0]=& _tmp295;({struct Cyc_Tcenv_Tenv*_tmp715=te;unsigned int _tmp714=loc;void**_tmp713=topt;struct _dyneither_ptr _tmp712=({const char*_tmp294="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp294,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp715,_tmp714,_tmp713,_tmp712,_tag_dyneither(_tmp293,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2A4->impl))->exist_vars != 0){
# 1532
if(!({void*_tmp716=t2;Cyc_Tcutil_unify(_tmp716,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp298=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,_tmp545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp545;});void*_tmp296[1U];_tmp296[0]=& _tmp298;({struct Cyc_Tcenv_Tenv*_tmp71A=te;unsigned int _tmp719=loc;void**_tmp718=topt;struct _dyneither_ptr _tmp717=({const char*_tmp297="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp297,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp71A,_tmp719,_tmp718,_tmp717,_tag_dyneither(_tmp296,sizeof(void*),1U));});});}
# 1535
return t2;};}}else{goto _LL5;}case 12U: _LL3: _tmp2A6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp28C)->f1;_tmp2A5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp28C)->f2;_LL4: {
# 1537
struct Cyc_Absyn_Aggrfield*_tmp299=Cyc_Absyn_lookup_field(_tmp2A5,f);
if(_tmp299 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp29C=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,_tmp546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp546;});void*_tmp29A[1U];_tmp29A[0]=& _tmp29C;({struct Cyc_Tcenv_Tenv*_tmp71E=te;unsigned int _tmp71D=loc;void**_tmp71C=topt;struct _dyneither_ptr _tmp71B=({const char*_tmp29B="type has no %s member";_tag_dyneither(_tmp29B,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp71E,_tmp71D,_tmp71C,_tmp71B,_tag_dyneither(_tmp29A,sizeof(void*),1U));});});
# 1542
if(((_tmp2A6 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp299->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp299->requires_clause == 0)
# 1544
return({struct Cyc_String_pa_PrintArg_struct _tmp29F=({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0U,_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp547;});void*_tmp29D[1U];_tmp29D[0]=& _tmp29F;({struct Cyc_Tcenv_Tenv*_tmp722=te;unsigned int _tmp721=loc;void**_tmp720=topt;struct _dyneither_ptr _tmp71F=({const char*_tmp29E="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp29E,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp722,_tmp721,_tmp720,_tmp71F,_tag_dyneither(_tmp29D,sizeof(void*),1U));});});
return _tmp299->type;}default: _LL5: _LL6:
# 1547
 return({struct Cyc_String_pa_PrintArg_struct _tmp2A2=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,({
struct _dyneither_ptr _tmp723=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp548.f1=_tmp723;});_tmp548;});void*_tmp2A0[1U];_tmp2A0[0]=& _tmp2A2;({struct Cyc_Tcenv_Tenv*_tmp727=te;unsigned int _tmp726=loc;void**_tmp725=topt;struct _dyneither_ptr _tmp724=({const char*_tmp2A1="expecting struct or union, found %s";_tag_dyneither(_tmp2A1,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp727,_tmp726,_tmp725,_tmp724,_tag_dyneither(_tmp2A0,sizeof(void*),1U));});});}_LL0:;};}
# 1553
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1556
({struct Cyc_Tcenv_Tenv*_tmp728=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp728,0,e);});
# 1558
({int _tmp729=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp729;});
{void*_tmp2A7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A8=_tmp2A7;void*_tmp2CB;void*_tmp2CA;union Cyc_Absyn_Constraint*_tmp2C9;union Cyc_Absyn_Constraint*_tmp2C8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->tag == 5U){_LL1: _tmp2CB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->f1).elt_typ;_tmp2CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->f1).ptr_atts).rgn;_tmp2C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->f1).ptr_atts).bounds;_tmp2C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->f1).ptr_atts).zero_term;_LL2:
# 1561
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2C9);
{void*_tmp2A9=Cyc_Tcutil_compress(_tmp2CB);void*_tmp2AA=_tmp2A9;enum Cyc_Absyn_AggrKind _tmp2C7;struct Cyc_List_List*_tmp2C6;struct Cyc_Absyn_Aggrdecl*_tmp2C5;struct Cyc_List_List*_tmp2C4;switch(*((int*)_tmp2AA)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2AA)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp2C5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2AA)->f1).aggr_info).KnownAggr).val;_tmp2C4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2AA)->f1).targs;_LL7: {
# 1564
struct Cyc_Absyn_Aggrfield*_tmp2AB=Cyc_Absyn_lookup_decl_field(_tmp2C5,f);
if(_tmp2AB == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,({
struct _dyneither_ptr _tmp72A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C5->name));_tmp54A.f1=_tmp72A;});_tmp54A;});struct Cyc_String_pa_PrintArg_struct _tmp2AF=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,_tmp549.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp549;});void*_tmp2AC[2U];_tmp2AC[0]=& _tmp2AE,_tmp2AC[1]=& _tmp2AF;({struct Cyc_Tcenv_Tenv*_tmp72E=te;unsigned int _tmp72D=loc;void**_tmp72C=topt;struct _dyneither_ptr _tmp72B=({const char*_tmp2AD="type %s has no %s member";_tag_dyneither(_tmp2AD,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp72E,_tmp72D,_tmp72C,_tmp72B,_tag_dyneither(_tmp2AC,sizeof(void*),2U));});});
# 1569
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C5->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2AB->type;
if(_tmp2C4 != 0){
struct Cyc_List_List*_tmp2B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2C5->tvs,_tmp2C4);
({void*_tmp72F=Cyc_Tcutil_substitute(_tmp2B0,_tmp2AB->type);t3=_tmp72F;});}{
# 1575
struct Cyc_Absyn_Kind*_tmp2B1=Cyc_Tcutil_typ_kind(t3);
# 1578
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2B1) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2B2=Cyc_Tcutil_compress(t3);void*_tmp2B3=_tmp2B2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B3)->tag == 8U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1582
 return({struct Cyc_String_pa_PrintArg_struct _tmp2B6=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,_tmp54B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54B;});void*_tmp2B4[1U];_tmp2B4[0]=& _tmp2B6;({struct Cyc_Tcenv_Tenv*_tmp733=te;unsigned int _tmp732=loc;void**_tmp731=topt;struct _dyneither_ptr _tmp730=({const char*_tmp2B5="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2B5,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp733,_tmp732,_tmp731,_tmp730,_tag_dyneither(_tmp2B4,sizeof(void*),1U));});});}_LLC:;}
# 1587
if((((_tmp2C5->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C5->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2AB->requires_clause == 0)
# 1590
return({struct Cyc_String_pa_PrintArg_struct _tmp2B9=({struct Cyc_String_pa_PrintArg_struct _tmp54C;_tmp54C.tag=0U,_tmp54C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54C;});void*_tmp2B7[1U];_tmp2B7[0]=& _tmp2B9;({struct Cyc_Tcenv_Tenv*_tmp737=te;unsigned int _tmp736=loc;void**_tmp735=topt;struct _dyneither_ptr _tmp734=({const char*_tmp2B8="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B8,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp737,_tmp736,_tmp735,_tmp734,_tag_dyneither(_tmp2B7,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C5->impl))->exist_vars != 0){
if(!({void*_tmp738=t3;Cyc_Tcutil_unify(_tmp738,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2BC=({struct Cyc_String_pa_PrintArg_struct _tmp54D;_tmp54D.tag=0U,_tmp54D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54D;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BC;({struct Cyc_Tcenv_Tenv*_tmp73C=te;unsigned int _tmp73B=loc;void**_tmp73A=topt;struct _dyneither_ptr _tmp739=({const char*_tmp2BB="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2BB,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp73C,_tmp73B,_tmp73A,_tmp739,_tag_dyneither(_tmp2BA,sizeof(void*),1U));});});}
# 1597
return t3;};};}}else{goto _LLA;}case 12U: _LL8: _tmp2C7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2AA)->f1;_tmp2C6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2AA)->f2;_LL9: {
# 1599
struct Cyc_Absyn_Aggrfield*_tmp2BD=Cyc_Absyn_lookup_field(_tmp2C6,f);
if(_tmp2BD == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2C0=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,_tmp54E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54E;});void*_tmp2BE[1U];_tmp2BE[0]=& _tmp2C0;({struct Cyc_Tcenv_Tenv*_tmp740=te;unsigned int _tmp73F=loc;void**_tmp73E=topt;struct _dyneither_ptr _tmp73D=({const char*_tmp2BF="type has no %s field";_tag_dyneither(_tmp2BF,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp740,_tmp73F,_tmp73E,_tmp73D,_tag_dyneither(_tmp2BE,sizeof(void*),1U));});});
# 1604
if((_tmp2C7 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2BD->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2C3=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,_tmp54F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp54F;});void*_tmp2C1[1U];_tmp2C1[0]=& _tmp2C3;({struct Cyc_Tcenv_Tenv*_tmp744=te;unsigned int _tmp743=loc;void**_tmp742=topt;struct _dyneither_ptr _tmp741=({const char*_tmp2C2="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2C2,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp744,_tmp743,_tmp742,_tmp741,_tag_dyneither(_tmp2C1,sizeof(void*),1U));});});
return _tmp2BD->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1609
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1612
return({struct Cyc_String_pa_PrintArg_struct _tmp2CE=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({
struct _dyneither_ptr _tmp745=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp550.f1=_tmp745;});_tmp550;});void*_tmp2CC[1U];_tmp2CC[0]=& _tmp2CE;({struct Cyc_Tcenv_Tenv*_tmp749=te;unsigned int _tmp748=loc;void**_tmp747=topt;struct _dyneither_ptr _tmp746=({const char*_tmp2CD="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2CD,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp749,_tmp748,_tmp747,_tmp746,_tag_dyneither(_tmp2CC,sizeof(void*),1U));});});}
# 1617
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1619
struct _tuple15 _tmp2CF=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2D0=_tmp2CF;unsigned int _tmp2DE;int _tmp2DD;_LL1: _tmp2DE=_tmp2D0.f1;_tmp2DD=_tmp2D0.f2;_LL2:;
if(!_tmp2DD)
return({void*_tmp2D1=0U;({struct Cyc_Tcenv_Tenv*_tmp74C=te;unsigned int _tmp74B=loc;struct _dyneither_ptr _tmp74A=({const char*_tmp2D2="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2D2,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp74C,_tmp74B,0,_tmp74A,_tag_dyneither(_tmp2D1,sizeof(void*),0U));});});{
# 1623
struct _handler_cons _tmp2D3;_push_handler(& _tmp2D3);{int _tmp2D5=0;if(setjmp(_tmp2D3.handler))_tmp2D5=1;if(!_tmp2D5){
{void*_tmp2D6=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2DE)).f2;_npop_handler(0U);return _tmp2D6;};_pop_handler();}else{void*_tmp2D4=(void*)_exn_thrown;void*_tmp2D7=_tmp2D4;void*_tmp2DC;if(((struct Cyc_List_Nth_exn_struct*)_tmp2D7)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1626
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2DA=({struct Cyc_Int_pa_PrintArg_struct _tmp552;_tmp552.tag=1U,_tmp552.f1=(unsigned long)((int)_tmp2DE);_tmp552;});struct Cyc_Int_pa_PrintArg_struct _tmp2DB=({struct Cyc_Int_pa_PrintArg_struct _tmp551;_tmp551.tag=1U,({
unsigned long _tmp74D=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp551.f1=_tmp74D;});_tmp551;});void*_tmp2D8[2U];_tmp2D8[0]=& _tmp2DA,_tmp2D8[1]=& _tmp2DB;({struct Cyc_Tcenv_Tenv*_tmp750=te;unsigned int _tmp74F=loc;struct _dyneither_ptr _tmp74E=({const char*_tmp2D9="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2D9,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp750,_tmp74F,0,_tmp74E,_tag_dyneither(_tmp2D8,sizeof(void*),2U));});});}else{_LL6: _tmp2DC=_tmp2D7;_LL7:(int)_rethrow(_tmp2DC);}_LL3:;}};};}
# 1631
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1633
struct Cyc_Tcenv_Tenv*_tmp2DF=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp751=Cyc_Tcenv_clear_abstract_val_ok(_tmp2DF);Cyc_Tcexp_tcExp(_tmp751,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp752=Cyc_Tcenv_clear_abstract_val_ok(_tmp2DF);Cyc_Tcexp_tcExp(_tmp752,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2DF,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2E2=({struct Cyc_String_pa_PrintArg_struct _tmp553;_tmp553.tag=0U,({
struct _dyneither_ptr _tmp753=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp553.f1=_tmp753;});_tmp553;});void*_tmp2E0[1U];_tmp2E0[0]=& _tmp2E2;({struct Cyc_Tcenv_Tenv*_tmp757=_tmp2DF;unsigned int _tmp756=e2->loc;void**_tmp755=topt;struct _dyneither_ptr _tmp754=({const char*_tmp2E1="expecting int subscript, found %s";_tag_dyneither(_tmp2E1,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp757,_tmp756,_tmp755,_tmp754,_tag_dyneither(_tmp2E0,sizeof(void*),1U));});});{
# 1643
void*_tmp2E3=t1;struct Cyc_List_List*_tmp307;void*_tmp306;struct Cyc_Absyn_Tqual _tmp305;void*_tmp304;union Cyc_Absyn_Constraint*_tmp303;union Cyc_Absyn_Constraint*_tmp302;switch(*((int*)_tmp2E3)){case 5U: _LL1: _tmp306=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->f1).elt_typ;_tmp305=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->f1).elt_tq;_tmp304=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->f1).ptr_atts).rgn;_tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->f1).ptr_atts).bounds;_tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->f1).ptr_atts).zero_term;_LL2:
# 1647
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp302)){
int emit_warning=0;
{void*_tmp2E4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp303);void*_tmp2E5=_tmp2E4;struct Cyc_Absyn_Exp*_tmp2F4;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2E5)->tag == 1U){_LL8: _tmp2F4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2E5)->f1;_LL9: {
# 1651
struct _tuple15 _tmp2E6=Cyc_Evexp_eval_const_uint_exp(_tmp2F4);struct _tuple15 _tmp2E7=_tmp2E6;unsigned int _tmp2F3;int _tmp2F2;_LLD: _tmp2F3=_tmp2E7.f1;_tmp2F2=_tmp2E7.f2;_LLE:;
if(_tmp2F2  && _tmp2F3 == 1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2E8=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2E9=_tmp2E8;unsigned int _tmp2F1;int _tmp2F0;_LL10: _tmp2F1=_tmp2E9.f1;_tmp2F0=_tmp2E9.f2;_LL11:;
if(_tmp2F0){
struct _tuple15 _tmp2EA=Cyc_Evexp_eval_const_uint_exp(_tmp2F4);struct _tuple15 _tmp2EB=_tmp2EA;unsigned int _tmp2EF;int _tmp2EE;_LL13: _tmp2EF=_tmp2EB.f1;_tmp2EE=_tmp2EB.f2;_LL14:;
if((_tmp2F2  && _tmp2EF > _tmp2F3) && _tmp2F3 != 1)
({void*_tmp2EC=0U;({unsigned int _tmp759=loc;struct _dyneither_ptr _tmp758=({const char*_tmp2ED="subscript will fail at run-time";_tag_dyneither(_tmp2ED,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp759,_tmp758,_tag_dyneither(_tmp2EC,sizeof(void*),0U));});});}}
# 1661
goto _LL7;}}else{_LLA: _LLB:
 emit_warning=0;goto _LL7;}_LL7:;}
# 1664
if(emit_warning)
({void*_tmp2F5=0U;({unsigned int _tmp75B=e2->loc;struct _dyneither_ptr _tmp75A=({const char*_tmp2F6="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp2F6,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp75B,_tmp75A,_tag_dyneither(_tmp2F5,sizeof(void*),0U));});});}else{
# 1668
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2F7=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2F8=_tmp2F7;unsigned int _tmp2FA;int _tmp2F9;_LL16: _tmp2FA=_tmp2F8.f1;_tmp2F9=_tmp2F8.f2;_LL17:;
if(_tmp2F9)
# 1673
({unsigned int _tmp75E=loc;unsigned int _tmp75D=_tmp2FA;union Cyc_Absyn_Constraint*_tmp75C=_tmp303;Cyc_Tcutil_check_bound(_tmp75E,_tmp75D,_tmp75C,Cyc_Tcenv_abstract_val_ok(_tmp2DF));});}else{
# 1676
if(Cyc_Tcutil_is_bound_one(_tmp303) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp302))
({void*_tmp2FB=0U;({unsigned int _tmp760=e1->loc;struct _dyneither_ptr _tmp75F=({const char*_tmp2FC="subscript applied to pointer to one object";_tag_dyneither(_tmp2FC,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp760,_tmp75F,_tag_dyneither(_tmp2FB,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp303);}}
# 1681
Cyc_Tcenv_check_rgn_accessible(_tmp2DF,loc,_tmp304);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp306),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2DF))
({void*_tmp2FD=0U;({unsigned int _tmp762=e1->loc;struct _dyneither_ptr _tmp761=({const char*_tmp2FE="can't subscript an abstract pointer";_tag_dyneither(_tmp2FE,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp762,_tmp761,_tag_dyneither(_tmp2FD,sizeof(void*),0U));});});
return _tmp306;case 10U: _LL3: _tmp307=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2E3)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2DF,loc,_tmp307,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp301=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,({struct _dyneither_ptr _tmp763=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp554.f1=_tmp763;});_tmp554;});void*_tmp2FF[1U];_tmp2FF[0]=& _tmp301;({struct Cyc_Tcenv_Tenv*_tmp767=_tmp2DF;unsigned int _tmp766=loc;void**_tmp765=topt;struct _dyneither_ptr _tmp764=({const char*_tmp300="subscript applied to %s";_tag_dyneither(_tmp300,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp767,_tmp766,_tmp765,_tmp764,_tag_dyneither(_tmp2FF,sizeof(void*),1U));});});}_LL0:;};};}
# 1691
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp308=Cyc_Tcutil_compress(*topt);void*_tmp309=_tmp308;struct Cyc_List_List*_tmp30D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp309)->tag == 10U){_LL1: _tmp30D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp309)->f1;_LL2:
# 1697
 if(({int _tmp768=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30D);_tmp768 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1700
goto _LL0;
# 1702
for(0;es != 0;(es=es->tl,_tmp30D=_tmp30D->tl)){
int bogus=0;
void*_tmp30A=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp30D))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp769=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp769,& _tmp30A,(struct Cyc_Absyn_Exp*)es->hd);});
# 1707
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp30D->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp76B=({struct Cyc_List_List*_tmp30C=_cycalloc(sizeof(*_tmp30C));({struct _tuple17*_tmp76A=({struct _tuple17*_tmp30B=_cycalloc(sizeof(*_tmp30B));_tmp30B->f1=(*((struct _tuple17*)_tmp30D->hd)).f1,_tmp30B->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp30B;});_tmp30C->hd=_tmp76A;}),_tmp30C->tl=fields;_tmp30C;});fields=_tmp76B;});}
# 1710
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1714
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp76C=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp76C,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp76F=({struct Cyc_List_List*_tmp30F=_cycalloc(sizeof(*_tmp30F));({struct _tuple17*_tmp76E=({struct _tuple17*_tmp30E=_cycalloc(sizeof(*_tmp30E));({struct Cyc_Absyn_Tqual _tmp76D=Cyc_Absyn_empty_tqual(0U);_tmp30E->f1=_tmp76D;}),_tmp30E->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp30E;});_tmp30F->hd=_tmp76E;}),_tmp30F->tl=fields;_tmp30F;});fields=_tmp76F;});}
# 1719
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp310=_cycalloc(sizeof(*_tmp310));_tmp310->tag=10U,({struct Cyc_List_List*_tmp770=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp310->f1=_tmp770;});_tmp310;});}
# 1723
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1726
return({void*_tmp311=0U;({struct Cyc_Tcenv_Tenv*_tmp774=te;unsigned int _tmp773=loc;void**_tmp772=topt;struct _dyneither_ptr _tmp771=({const char*_tmp312="tcCompoundLit";_tag_dyneither(_tmp312,sizeof(char),14U);});Cyc_Tcexp_expr_err(_tmp774,_tmp773,_tmp772,_tmp771,_tag_dyneither(_tmp311,sizeof(void*),0U));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1736 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1739
void*res_t2;
int _tmp313=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp314=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->tag=0U,({union Cyc_Absyn_Cnst _tmp775=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp313);_tmp32A->f1=_tmp775;});_tmp32A;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp314,loc);
# 1747
if(zero_term){
struct Cyc_Absyn_Exp*_tmp315=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp313 - 1);
if(!Cyc_Tcutil_is_zero(_tmp315))
({void*_tmp316=0U;({unsigned int _tmp777=_tmp315->loc;struct _dyneither_ptr _tmp776=({const char*_tmp317="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp317,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp777,_tmp776,_tag_dyneither(_tmp316,sizeof(void*),0U));});});}
# 1752
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp77B=({void*_tmp77A=res;struct Cyc_Absyn_Tqual _tmp779=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1753
struct Cyc_Absyn_Exp*_tmp778=sz_exp;Cyc_Absyn_array_typ(_tmp77A,_tmp779,_tmp778,
# 1755
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0U);});
# 1753
res_t2=_tmp77B;});
# 1757
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1760
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1762
({struct Cyc_String_pa_PrintArg_struct _tmp31A=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0U,({
struct _dyneither_ptr _tmp77C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));_tmp555.f1=_tmp77C;});_tmp555;});void*_tmp318[1U];_tmp318[0]=& _tmp31A;({unsigned int _tmp77E=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp77D=({const char*_tmp319="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp319,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp77E,_tmp77D,_tag_dyneither(_tmp318,sizeof(void*),1U));});});
# 1765
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1770
void*_tmp31B=(void*)ds->hd;void*_tmp31C=_tmp31B;struct Cyc_Absyn_Exp*_tmp329;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp31C)->tag == 1U){_LL1: _LL2:
# 1772
({void*_tmp31D=0U;({unsigned int _tmp780=loc;struct _dyneither_ptr _tmp77F=({const char*_tmp31E="only array index designators are supported";_tag_dyneither(_tmp31E,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp780,_tmp77F,_tag_dyneither(_tmp31D,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp329=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp31C)->f1;_LL4:
# 1775
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp329);{
struct _tuple15 _tmp31F=Cyc_Evexp_eval_const_uint_exp(_tmp329);struct _tuple15 _tmp320=_tmp31F;unsigned int _tmp328;int _tmp327;_LL6: _tmp328=_tmp320.f1;_tmp327=_tmp320.f2;_LL7:;
if(!_tmp327)
({void*_tmp321=0U;({unsigned int _tmp782=_tmp329->loc;struct _dyneither_ptr _tmp781=({const char*_tmp322="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp322,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp782,_tmp781,_tag_dyneither(_tmp321,sizeof(void*),0U));});});else{
if(_tmp328 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp325=({struct Cyc_Int_pa_PrintArg_struct _tmp557;_tmp557.tag=1U,_tmp557.f1=(unsigned long)offset;_tmp557;});struct Cyc_Int_pa_PrintArg_struct _tmp326=({struct Cyc_Int_pa_PrintArg_struct _tmp556;_tmp556.tag=1U,_tmp556.f1=(unsigned long)((int)_tmp328);_tmp556;});void*_tmp323[2U];_tmp323[0]=& _tmp325,_tmp323[1]=& _tmp326;({unsigned int _tmp784=_tmp329->loc;struct _dyneither_ptr _tmp783=({const char*_tmp324="expecting index designator %d but found %d";_tag_dyneither(_tmp324,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp784,_tmp783,_tag_dyneither(_tmp323,sizeof(void*),2U));});});}
# 1782
goto _LL0;};}_LL0:;}}}
# 1786
return res_t2;}
# 1790
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1793
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp32B=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp32C=_tmp32B;void*_tmp331;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp32C)->tag == 19U){_LL1: _tmp331=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp32C)->f1;_LL2:
# 1798
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp32D=({void*_tmp785=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp785,Cyc_Absyn_valueof_exp(_tmp331,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp32D->topt=bound->topt;
# 1803
bound=_tmp32D;}
# 1805
goto _LL0;}else{_LL3: _LL4:
# 1807
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp558;_tmp558.tag=0U,({
struct _dyneither_ptr _tmp786=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp558.f1=_tmp786;});_tmp558;});void*_tmp32E[1U];_tmp32E[0]=& _tmp330;({unsigned int _tmp788=bound->loc;struct _dyneither_ptr _tmp787=({const char*_tmp32F="expecting unsigned int, found %s";_tag_dyneither(_tmp32F,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp788,_tmp787,_tag_dyneither(_tmp32E,sizeof(void*),1U));});});}_LL0:;}
# 1812
if(!(vd->tq).real_const)
({void*_tmp332=0U;({struct _dyneither_ptr _tmp789=({const char*_tmp333="comprehension index variable is not declared const!";_tag_dyneither(_tmp333,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp789,_tag_dyneither(_tmp332,sizeof(void*),0U));});});
# 1815
if(te->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp78A=Cyc_Tcenv_new_block(loc,te);te=_tmp78A;});{
void**_tmp334=0;
struct Cyc_Absyn_Tqual*_tmp335=0;
union Cyc_Absyn_Constraint**_tmp336=0;
# 1821
if(topt != 0){
void*_tmp337=Cyc_Tcutil_compress(*topt);void*_tmp338=_tmp337;void*_tmp343;struct Cyc_Absyn_Tqual _tmp342;struct Cyc_Absyn_Exp*_tmp341;union Cyc_Absyn_Constraint*_tmp340;struct Cyc_Absyn_PtrInfo _tmp33F;switch(*((int*)_tmp338)){case 5U: _LL6: _tmp33F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp338)->f1;_LL7:
# 1824
({void**_tmp78B=({void**_tmp339=_cycalloc(sizeof(*_tmp339));*_tmp339=_tmp33F.elt_typ;_tmp339;});_tmp334=_tmp78B;});
({struct Cyc_Absyn_Tqual*_tmp78C=({struct Cyc_Absyn_Tqual*_tmp33A=_cycalloc(sizeof(*_tmp33A));*_tmp33A=_tmp33F.elt_tq;_tmp33A;});_tmp335=_tmp78C;});
({union Cyc_Absyn_Constraint**_tmp78D=({union Cyc_Absyn_Constraint**_tmp33B=_cycalloc(sizeof(*_tmp33B));*_tmp33B=(_tmp33F.ptr_atts).zero_term;_tmp33B;});_tmp336=_tmp78D;});
goto _LL5;case 8U: _LL8: _tmp343=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp338)->f1).elt_type;_tmp342=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp338)->f1).tq;_tmp341=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp338)->f1).num_elts;_tmp340=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp338)->f1).zero_term;_LL9:
# 1829
({void**_tmp78E=({void**_tmp33C=_cycalloc(sizeof(*_tmp33C));*_tmp33C=_tmp343;_tmp33C;});_tmp334=_tmp78E;});
({struct Cyc_Absyn_Tqual*_tmp78F=({struct Cyc_Absyn_Tqual*_tmp33D=_cycalloc(sizeof(*_tmp33D));*_tmp33D=_tmp342;_tmp33D;});_tmp335=_tmp78F;});
({union Cyc_Absyn_Constraint**_tmp790=({union Cyc_Absyn_Constraint**_tmp33E=_cycalloc(sizeof(*_tmp33E));*_tmp33E=_tmp340;_tmp33E;});_tmp336=_tmp790;});
goto _LL5;default: _LLA: _LLB:
# 1834
 goto _LL5;}_LL5:;}{
# 1837
void*t=Cyc_Tcexp_tcExp(te,_tmp334,body);
# 1839
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp344=0U;({unsigned int _tmp792=body->loc;struct _dyneither_ptr _tmp791=({const char*_tmp345="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp345,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp792,_tmp791,_tag_dyneither(_tmp344,sizeof(void*),0U));});});
if(te->le == 0){
# 1843
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp346=0U;({unsigned int _tmp794=bound->loc;struct _dyneither_ptr _tmp793=({const char*_tmp347="bound is not constant";_tag_dyneither(_tmp347,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp794,_tmp793,_tag_dyneither(_tmp346,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp348=0U;({unsigned int _tmp796=bound->loc;struct _dyneither_ptr _tmp795=({const char*_tmp349="body is not constant";_tag_dyneither(_tmp349,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp796,_tmp795,_tag_dyneither(_tmp348,sizeof(void*),0U));});});}
# 1848
if(_tmp336 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp336)){
# 1851
struct Cyc_Absyn_Exp*_tmp34A=Cyc_Absyn_uint_exp(1U,0U);_tmp34A->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp797=Cyc_Absyn_add_exp(bound,_tmp34A,0U);bound=_tmp797;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}
# 1855
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp34B=0U;({unsigned int _tmp799=body->loc;struct _dyneither_ptr _tmp798=({const char*_tmp34C="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp34C,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp799,_tmp798,_tag_dyneither(_tmp34B,sizeof(void*),0U));});});{
# 1859
void*_tmp34D=({void*_tmp79C=t;struct Cyc_Absyn_Tqual _tmp79B=_tmp335 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp335;struct Cyc_Absyn_Exp*_tmp79A=bound;Cyc_Absyn_array_typ(_tmp79C,_tmp79B,_tmp79A,
_tmp336 == 0?Cyc_Absyn_false_conref:*_tmp336,0U);});
return _tmp34D;};};};}
# 1865
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1868
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp34E=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp34F=_tmp34E;void*_tmp354;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp34F)->tag == 19U){_LL1: _tmp354=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp34F)->f1;_LL2:
# 1873
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp350=({void*_tmp79D=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp79D,Cyc_Absyn_valueof_exp(_tmp354,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp350->topt=bound->topt;
# 1878
bound=_tmp350;}
# 1880
goto _LL0;}else{_LL3: _LL4:
# 1882
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp353=({struct Cyc_String_pa_PrintArg_struct _tmp559;_tmp559.tag=0U,({
struct _dyneither_ptr _tmp79E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp559.f1=_tmp79E;});_tmp559;});void*_tmp351[1U];_tmp351[0]=& _tmp353;({unsigned int _tmp7A0=bound->loc;struct _dyneither_ptr _tmp79F=({const char*_tmp352="expecting unsigned int, found %s";_tag_dyneither(_tmp352,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7A0,_tmp79F,_tag_dyneither(_tmp351,sizeof(void*),1U));});});}_LL0:;}{
# 1887
void**_tmp355=0;
struct Cyc_Absyn_Tqual*_tmp356=0;
union Cyc_Absyn_Constraint**_tmp357=0;
# 1891
if(topt != 0){
void*_tmp358=Cyc_Tcutil_compress(*topt);void*_tmp359=_tmp358;void*_tmp364;struct Cyc_Absyn_Tqual _tmp363;struct Cyc_Absyn_Exp*_tmp362;union Cyc_Absyn_Constraint*_tmp361;struct Cyc_Absyn_PtrInfo _tmp360;switch(*((int*)_tmp359)){case 5U: _LL6: _tmp360=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359)->f1;_LL7:
# 1894
({void**_tmp7A1=({void**_tmp35A=_cycalloc(sizeof(*_tmp35A));*_tmp35A=_tmp360.elt_typ;_tmp35A;});_tmp355=_tmp7A1;});
({struct Cyc_Absyn_Tqual*_tmp7A2=({struct Cyc_Absyn_Tqual*_tmp35B=_cycalloc(sizeof(*_tmp35B));*_tmp35B=_tmp360.elt_tq;_tmp35B;});_tmp356=_tmp7A2;});
({union Cyc_Absyn_Constraint**_tmp7A3=({union Cyc_Absyn_Constraint**_tmp35C=_cycalloc(sizeof(*_tmp35C));*_tmp35C=(_tmp360.ptr_atts).zero_term;_tmp35C;});_tmp357=_tmp7A3;});
goto _LL5;case 8U: _LL8: _tmp364=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359)->f1).elt_type;_tmp363=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359)->f1).tq;_tmp362=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359)->f1).num_elts;_tmp361=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359)->f1).zero_term;_LL9:
# 1899
({void**_tmp7A4=({void**_tmp35D=_cycalloc(sizeof(*_tmp35D));*_tmp35D=_tmp364;_tmp35D;});_tmp355=_tmp7A4;});
({struct Cyc_Absyn_Tqual*_tmp7A5=({struct Cyc_Absyn_Tqual*_tmp35E=_cycalloc(sizeof(*_tmp35E));*_tmp35E=_tmp363;_tmp35E;});_tmp356=_tmp7A5;});
({union Cyc_Absyn_Constraint**_tmp7A6=({union Cyc_Absyn_Constraint**_tmp35F=_cycalloc(sizeof(*_tmp35F));*_tmp35F=_tmp361;_tmp35F;});_tmp357=_tmp7A6;});
goto _LL5;default: _LLA: _LLB:
# 1904
 goto _LL5;}_LL5:;}
# 1907
({unsigned int _tmp7A9=loc;struct Cyc_Tcenv_Tenv*_tmp7A8=te;struct Cyc_List_List*_tmp7A7=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7A9,_tmp7A8,_tmp7A7,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp355 != 0)Cyc_Tcutil_unify(*_tmp355,t);
# 1910
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp365=0U;({unsigned int _tmp7AB=bound->loc;struct _dyneither_ptr _tmp7AA=({const char*_tmp366="bound is not constant";_tag_dyneither(_tmp366,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp7AB,_tmp7AA,_tag_dyneither(_tmp365,sizeof(void*),0U));});});}
# 1914
if(_tmp357 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp357)){
# 1917
struct Cyc_Absyn_Exp*_tmp367=Cyc_Absyn_uint_exp(1U,0U);_tmp367->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp7AC=Cyc_Absyn_add_exp(bound,_tmp367,0U);bound=_tmp7AC;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1921
({void*_tmp368=0U;({unsigned int _tmp7AE=loc;struct _dyneither_ptr _tmp7AD=({const char*_tmp369="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp369,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp7AE,_tmp7AD,_tag_dyneither(_tmp368,sizeof(void*),0U));});});}{
# 1924
void*_tmp36A=({void*_tmp7B1=t;struct Cyc_Absyn_Tqual _tmp7B0=_tmp356 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp356;struct Cyc_Absyn_Exp*_tmp7AF=bound;Cyc_Absyn_array_typ(_tmp7B1,_tmp7B0,_tmp7AF,
_tmp357 == 0?Cyc_Absyn_false_conref:*_tmp357,0U);});
return _tmp36A;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1939 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1943
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp7B2=({struct Cyc_Absyn_Aggrdecl**_tmp36B=_cycalloc(sizeof(*_tmp36B));*_tmp36B=ad;_tmp36B;});adptr=_tmp7B2;});}else{
# 1949
{struct _handler_cons _tmp36C;_push_handler(& _tmp36C);{int _tmp36E=0;if(setjmp(_tmp36C.handler))_tmp36E=1;if(!_tmp36E){({struct Cyc_Absyn_Aggrdecl**_tmp7B3=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp7B3;});
ad=*adptr;
# 1949
;_pop_handler();}else{void*_tmp36D=(void*)_exn_thrown;void*_tmp36F=_tmp36D;void*_tmp373;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp36F)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1952
({struct Cyc_String_pa_PrintArg_struct _tmp372=({struct Cyc_String_pa_PrintArg_struct _tmp55A;_tmp55A.tag=0U,({struct _dyneither_ptr _tmp7B4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp55A.f1=_tmp7B4;});_tmp55A;});void*_tmp370[1U];_tmp370[0]=& _tmp372;({unsigned int _tmp7B6=loc;struct _dyneither_ptr _tmp7B5=({const char*_tmp371="unbound struct/union name %s";_tag_dyneither(_tmp371,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp7B6,_tmp7B5,_tag_dyneither(_tmp370,sizeof(void*),1U));});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL3: _tmp373=_tmp36F;_LL4:(int)_rethrow(_tmp373);}_LL0:;}};}
# 1955
*ad_opt=ad;
*tn=ad->name;}
# 1958
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp376=({struct Cyc_String_pa_PrintArg_struct _tmp55B;_tmp55B.tag=0U,({struct _dyneither_ptr _tmp7B7=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp377="struct";_tag_dyneither(_tmp377,sizeof(char),7U);}):({const char*_tmp378="union";_tag_dyneither(_tmp378,sizeof(char),6U);}));_tmp55B.f1=_tmp7B7;});_tmp55B;});void*_tmp374[1U];_tmp374[0]=& _tmp376;({unsigned int _tmp7B9=loc;struct _dyneither_ptr _tmp7B8=({const char*_tmp375="can't construct abstract %s";_tag_dyneither(_tmp375,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp7B9,_tmp7B8,_tag_dyneither(_tmp374,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1966
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp379=Cyc_Tcenv_new_status(te);
if(_tmp379 == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp7BA=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp7BA;});else{
# 1975
({struct Cyc_Tcenv_Tenv*_tmp7BB=Cyc_Tcenv_set_new_status(_tmp379,te);te2=_tmp7BB;});}{
# 1977
struct _tuple12 _tmp37A=({struct _tuple12 _tmp561;({struct Cyc_List_List*_tmp7BC=Cyc_Tcenv_lookup_type_vars(te2);_tmp561.f1=_tmp7BC;}),_tmp561.f2=Cyc_Core_heap_region;_tmp561;});
struct Cyc_List_List*_tmp37B=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp37A,ad->tvs);
struct Cyc_List_List*_tmp37C=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp37A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp37D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp37B);
struct Cyc_List_List*_tmp37E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp37C);
struct Cyc_List_List*_tmp37F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp37B,_tmp37C);
void*res_typ;
# 1988
if(topt != 0){
void*_tmp380=Cyc_Tcutil_compress(*topt);void*_tmp381=_tmp380;struct Cyc_Absyn_Aggrdecl**_tmp385;struct Cyc_List_List*_tmp384;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp381)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp381)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp385=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp381)->f1).aggr_info).KnownAggr).val;_tmp384=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp381)->f1).targs;_LL7:
# 1991
 if(*_tmp385 == *adptr){
{struct Cyc_List_List*_tmp382=_tmp37D;for(0;_tmp382 != 0  && _tmp384 != 0;(
_tmp382=_tmp382->tl,_tmp384=_tmp384->tl)){
Cyc_Tcutil_unify((void*)_tmp382->hd,(void*)_tmp384->hd);}}
# 1996
res_typ=*topt;
goto _LL5;}
# 1999
goto _LL9;}else{goto _LL8;}}else{_LL8: _LL9:
# 2001
({void*_tmp7BE=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7BD=Cyc_Absyn_KnownAggr(adptr);(_tmp383->f1).aggr_info=_tmp7BD;}),(_tmp383->f1).targs=_tmp37D;_tmp383;});res_typ=_tmp7BE;});}_LL5:;}else{
# 2004
({void*_tmp7C0=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7BF=Cyc_Absyn_KnownAggr(adptr);(_tmp386->f1).aggr_info=_tmp7BF;}),(_tmp386->f1).targs=_tmp37D;_tmp386;});res_typ=_tmp7C0;});}{
# 2007
struct Cyc_List_List*_tmp387=*ts;
struct Cyc_List_List*_tmp388=_tmp37E;
while(_tmp387 != 0  && _tmp388 != 0){
# 2011
({unsigned int _tmp7C3=loc;struct Cyc_Tcenv_Tenv*_tmp7C2=te2;struct Cyc_List_List*_tmp7C1=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp7C3,_tmp7C2,_tmp7C1,& Cyc_Tcutil_ak,1,0,(void*)_tmp387->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp387->hd);
Cyc_Tcutil_unify((void*)_tmp387->hd,(void*)_tmp388->hd);
_tmp387=_tmp387->tl;
_tmp388=_tmp388->tl;}
# 2017
if(_tmp387 != 0)
({void*_tmp389=0U;({unsigned int _tmp7C5=loc;struct _dyneither_ptr _tmp7C4=({const char*_tmp38A="too many explicit witness types";_tag_dyneither(_tmp38A,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7C5,_tmp7C4,_tag_dyneither(_tmp389,sizeof(void*),0U));});});
# 2020
*ts=_tmp37E;{
# 2023
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp38B=(struct _tuple19*)fields->hd;struct _tuple19*_tmp38C=_tmp38B;struct Cyc_Absyn_Aggrfield*_tmp398;struct Cyc_Absyn_Exp*_tmp397;_LLB: _tmp398=_tmp38C->f1;_tmp397=_tmp38C->f2;_LLC:;{
void*_tmp38D=Cyc_Tcutil_substitute(_tmp37F,_tmp398->type);
({struct Cyc_Tcenv_Tenv*_tmp7C6=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7C6,& _tmp38D,_tmp397);});
# 2034
if(!Cyc_Tcutil_coerce_arg(te2,_tmp397,_tmp38D,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp390=({struct Cyc_String_pa_PrintArg_struct _tmp560;_tmp560.tag=0U,_tmp560.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp398->name);_tmp560;});struct Cyc_String_pa_PrintArg_struct _tmp391=({struct Cyc_String_pa_PrintArg_struct _tmp55F;_tmp55F.tag=0U,({
struct _dyneither_ptr _tmp7C7=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp395="struct";_tag_dyneither(_tmp395,sizeof(char),7U);}):({const char*_tmp396="union";_tag_dyneither(_tmp396,sizeof(char),6U);}));_tmp55F.f1=_tmp7C7;});_tmp55F;});struct Cyc_String_pa_PrintArg_struct _tmp392=({struct Cyc_String_pa_PrintArg_struct _tmp55E;_tmp55E.tag=0U,({
struct _dyneither_ptr _tmp7C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp55E.f1=_tmp7C8;});_tmp55E;});struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp55D;_tmp55D.tag=0U,({struct _dyneither_ptr _tmp7C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp38D));_tmp55D.f1=_tmp7C9;});_tmp55D;});struct Cyc_String_pa_PrintArg_struct _tmp394=({struct Cyc_String_pa_PrintArg_struct _tmp55C;_tmp55C.tag=0U,({
struct _dyneither_ptr _tmp7CA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp397->topt)));_tmp55C.f1=_tmp7CA;});_tmp55C;});void*_tmp38E[5U];_tmp38E[0]=& _tmp390,_tmp38E[1]=& _tmp391,_tmp38E[2]=& _tmp392,_tmp38E[3]=& _tmp393,_tmp38E[4]=& _tmp394;({unsigned int _tmp7CC=_tmp397->loc;struct _dyneither_ptr _tmp7CB=({const char*_tmp38F="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp38F,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp7CC,_tmp7CB,_tag_dyneither(_tmp38E,sizeof(void*),5U));});});
Cyc_Tcutil_explain_failure();}};}{
# 2043
struct Cyc_List_List*_tmp399=0;
{struct Cyc_List_List*_tmp39A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp39A != 0;_tmp39A=_tmp39A->tl){
({struct Cyc_List_List*_tmp7D0=({struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));({struct _tuple0*_tmp7CF=({struct _tuple0*_tmp39B=_cycalloc(sizeof(*_tmp39B));({void*_tmp7CE=Cyc_Tcutil_substitute(_tmp37F,(*((struct _tuple0*)_tmp39A->hd)).f1);_tmp39B->f1=_tmp7CE;}),({
void*_tmp7CD=Cyc_Tcutil_substitute(_tmp37F,(*((struct _tuple0*)_tmp39A->hd)).f2);_tmp39B->f2=_tmp7CD;});_tmp39B;});
# 2045
_tmp39C->hd=_tmp7CF;}),_tmp39C->tl=_tmp399;_tmp39C;});_tmp399=_tmp7D0;});}}
# 2048
({struct Cyc_List_List*_tmp7D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp399);_tmp399=_tmp7D1;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp399);
return res_typ;};};};};};}
# 2054
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2056
{void*_tmp39D=Cyc_Tcutil_compress(ts);void*_tmp39E=_tmp39D;enum Cyc_Absyn_AggrKind _tmp3AD;struct Cyc_List_List*_tmp3AC;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp39E)->tag == 12U){_LL1: _tmp3AD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp39E)->f1;_tmp3AC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp39E)->f2;_LL2:
# 2058
 if(_tmp3AD == Cyc_Absyn_UnionA)
({void*_tmp39F=0U;({unsigned int _tmp7D3=loc;struct _dyneither_ptr _tmp7D2=({const char*_tmp3A0="expecting struct but found union";_tag_dyneither(_tmp3A0,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7D3,_tmp7D2,_tag_dyneither(_tmp39F,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp3AC);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3A1=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3A2=_tmp3A1;struct Cyc_Absyn_Aggrfield*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3A8;_LL6: _tmp3A9=_tmp3A2->f1;_tmp3A8=_tmp3A2->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp7D5=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp7D4=& _tmp3A9->type;Cyc_Tcexp_tcExpInitializer(_tmp7D5,_tmp7D4,_tmp3A8);});
# 2067
if(!Cyc_Tcutil_coerce_arg(te,_tmp3A8,_tmp3A9->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3A5=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0U,_tmp564.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3A9->name);_tmp564;});struct Cyc_String_pa_PrintArg_struct _tmp3A6=({struct Cyc_String_pa_PrintArg_struct _tmp563;_tmp563.tag=0U,({
struct _dyneither_ptr _tmp7D6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3A9->type));_tmp563.f1=_tmp7D6;});_tmp563;});struct Cyc_String_pa_PrintArg_struct _tmp3A7=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0U,({
struct _dyneither_ptr _tmp7D7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp3A8->topt)));_tmp562.f1=_tmp7D7;});_tmp562;});void*_tmp3A3[3U];_tmp3A3[0]=& _tmp3A5,_tmp3A3[1]=& _tmp3A6,_tmp3A3[2]=& _tmp3A7;({unsigned int _tmp7D9=_tmp3A8->loc;struct _dyneither_ptr _tmp7D8=({const char*_tmp3A4="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3A4,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp7D9,_tmp7D8,_tag_dyneither(_tmp3A3,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2074
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp3AA=0U;({struct _dyneither_ptr _tmp7DA=({const char*_tmp3AB="tcAnonStruct: wrong type";_tag_dyneither(_tmp3AB,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7DA,_tag_dyneither(_tmp3AA,sizeof(void*),0U));});});}_LL0:;}
# 2077
return ts;}
# 2081
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2084
struct _tuple12 _tmp3AE=({struct _tuple12 _tmp56A;({struct Cyc_List_List*_tmp7DB=Cyc_Tcenv_lookup_type_vars(te);_tmp56A.f1=_tmp7DB;}),_tmp56A.f2=Cyc_Core_heap_region;_tmp56A;});
struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp3AE,tud->tvs);
struct Cyc_List_List*_tmp3B0=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3AF);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));
_tmp3BF->tag=4U,({union Cyc_Absyn_DatatypeFieldInfoU _tmp7DC=Cyc_Absyn_KnownDatatypefield(tud,tuf);(_tmp3BF->f1).field_info=_tmp7DC;}),(_tmp3BF->f1).targs=_tmp3B0;_tmp3BF;});
# 2091
if(topt != 0){
void*_tmp3B1=Cyc_Tcutil_compress(*topt);void*_tmp3B2=_tmp3B1;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3B2)->tag == 4U){_LL1: _LL2:
 Cyc_Tcutil_unify(*topt,res);goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2097
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp3AF != 0)({void*_tmp7DD=Cyc_Tcutil_substitute(_tmp3AF,t);t=_tmp7DD;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B5=({struct Cyc_String_pa_PrintArg_struct _tmp567;_tmp567.tag=0U,({
# 2107
struct _dyneither_ptr _tmp7DE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp567.f1=_tmp7DE;});_tmp567;});struct Cyc_String_pa_PrintArg_struct _tmp3B6=({struct Cyc_String_pa_PrintArg_struct _tmp566;_tmp566.tag=0U,({struct _dyneither_ptr _tmp7DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp566.f1=_tmp7DF;});_tmp566;});struct Cyc_String_pa_PrintArg_struct _tmp3B7=({struct Cyc_String_pa_PrintArg_struct _tmp565;_tmp565.tag=0U,({
struct _dyneither_ptr _tmp7E0=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3B8="?";_tag_dyneither(_tmp3B8,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp565.f1=_tmp7E0;});_tmp565;});void*_tmp3B3[3U];_tmp3B3[0]=& _tmp3B5,_tmp3B3[1]=& _tmp3B6,_tmp3B3[2]=& _tmp3B7;({unsigned int _tmp7E2=e->loc;struct _dyneither_ptr _tmp7E1=({const char*_tmp3B4="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3B4,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp7E2,_tmp7E1,_tag_dyneither(_tmp3B3,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2112
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3BB=({struct Cyc_String_pa_PrintArg_struct _tmp568;_tmp568.tag=0U,({
# 2115
struct _dyneither_ptr _tmp7E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp568.f1=_tmp7E3;});_tmp568;});void*_tmp3B9[1U];_tmp3B9[0]=& _tmp3BB;({struct Cyc_Tcenv_Tenv*_tmp7E7=te;unsigned int _tmp7E6=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7E5=topt;struct _dyneither_ptr _tmp7E4=({const char*_tmp3BA="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3BA,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp7E7,_tmp7E6,_tmp7E5,_tmp7E4,_tag_dyneither(_tmp3B9,sizeof(void*),1U));});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3BE=({struct Cyc_String_pa_PrintArg_struct _tmp569;_tmp569.tag=0U,({
struct _dyneither_ptr _tmp7E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp569.f1=_tmp7E8;});_tmp569;});void*_tmp3BC[1U];_tmp3BC[0]=& _tmp3BE;({struct Cyc_Tcenv_Tenv*_tmp7EC=te;unsigned int _tmp7EB=loc;void**_tmp7EA=topt;struct _dyneither_ptr _tmp7E9=({const char*_tmp3BD="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3BD,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7EC,_tmp7EB,_tmp7EA,_tmp7E9,_tag_dyneither(_tmp3BC,sizeof(void*),1U));});});
return res;};}
# 2122
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2124
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2126
if(topt != 0){
void*_tmp3C0=Cyc_Tcutil_compress(*topt);void*_tmp3C1=_tmp3C0;void*_tmp3C2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C1)->tag == 5U){_LL1: _tmp3C2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C1)->f1).elt_typ;_LL2:
# 2129
 Cyc_Tcutil_unify(_tmp3C2,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2135
({struct Cyc_String_pa_PrintArg_struct _tmp3C5=({struct Cyc_String_pa_PrintArg_struct _tmp56C;_tmp56C.tag=0U,({
struct _dyneither_ptr _tmp7ED=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3C7="calloc";_tag_dyneither(_tmp3C7,sizeof(char),7U);}):(struct _dyneither_ptr)({const char*_tmp3C8="malloc";_tag_dyneither(_tmp3C8,sizeof(char),7U);})));_tmp56C.f1=_tmp7ED;});_tmp56C;});struct Cyc_String_pa_PrintArg_struct _tmp3C6=({struct Cyc_String_pa_PrintArg_struct _tmp56B;_tmp56B.tag=0U,({struct _dyneither_ptr _tmp7EE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp56B.f1=_tmp7EE;});_tmp56B;});void*_tmp3C3[2U];_tmp3C3[0]=& _tmp3C5,_tmp3C3[1]=& _tmp3C6;({unsigned int _tmp7F0=loc;struct _dyneither_ptr _tmp7EF=({const char*_tmp3C4="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3C4,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp7F0,_tmp7EF,_tag_dyneither(_tmp3C3,sizeof(void*),2U));});});}
# 2139
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2141
enum Cyc_Absyn_AliasQual _tmp3C9=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3CA=_tmp3C9;if(_tmp3CA == Cyc_Absyn_Unique){_LL1: _LL2:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL3: _LL4:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL0:;}
# 2147
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2152
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2155
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));
_tmp3D1->tag=15U,({void*_tmp7F1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3D1->f1=_tmp7F1;});_tmp3D1;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3CB=Cyc_Tcutil_compress(handle_type);void*_tmp3CC=_tmp3CB;void*_tmp3D0;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3CC)->tag == 15U){_LL1: _tmp3D0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3CC)->f1;_LL2:
# 2160
 rgn=_tmp3D0;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2164
({struct Cyc_String_pa_PrintArg_struct _tmp3CF=({struct Cyc_String_pa_PrintArg_struct _tmp56D;_tmp56D.tag=0U,({
struct _dyneither_ptr _tmp7F2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp56D.f1=_tmp7F2;});_tmp56D;});void*_tmp3CD[1U];_tmp3CD[0]=& _tmp3CF;({unsigned int _tmp7F4=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7F3=({const char*_tmp3CE="expecting region_t type but found %s";_tag_dyneither(_tmp3CE,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7F4,_tmp7F3,_tag_dyneither(_tmp3CD,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2171
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7F5=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7F5;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2178
({struct Cyc_Tcenv_Tenv*_tmp7F6=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F6,& Cyc_Absyn_uint_typ,*e);});{
# 2187 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3D2=0U;({struct _dyneither_ptr _tmp7F7=({const char*_tmp3D3="calloc with empty type";_tag_dyneither(_tmp3D3,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F7,_tag_dyneither(_tmp3D2,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7FA=loc;struct Cyc_Tcenv_Tenv*_tmp7F9=te;struct Cyc_List_List*_tmp7F8=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7FA,_tmp7F9,_tmp7F8,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2199
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3D4=er;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3E8;void*_tmp3E7;switch(*((int*)_tmp3D4)){case 17U: _LL6: _tmp3E7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D4)->f1;_LL7:
# 2203
 elt_type=_tmp3E7;
({void**_tmp7FB=({void**_tmp3D5=_cycalloc(sizeof(*_tmp3D5));*_tmp3D5=elt_type;_tmp3D5;});*t=_tmp7FB;});
({struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_uint_exp(1U,0U);num_elts=_tmp7FC;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D4)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D4)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D4)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D4)->f2)->tl)->tl == 0){_LL8: _tmp3E9=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D4)->f2)->hd;_tmp3E8=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D4)->f2)->tl)->hd;_LL9:
# 2210
{struct _tuple0 _tmp3D6=({struct _tuple0 _tmp56E;_tmp56E.f1=_tmp3E9->r,_tmp56E.f2=_tmp3E8->r;_tmp56E;});struct _tuple0 _tmp3D7=_tmp3D6;void*_tmp3DB;void*_tmp3DA;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D7.f1)->tag == 17U){_LLD: _tmp3DA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D7.f1)->f1;_LLE:
# 2212
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3DA);
elt_type=_tmp3DA;
({void**_tmp7FD=({void**_tmp3D8=_cycalloc(sizeof(*_tmp3D8));*_tmp3D8=elt_type;_tmp3D8;});*t=_tmp7FD;});
num_elts=_tmp3E8;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D7.f2)->tag == 17U){_LLF: _tmp3DB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3D7.f2)->f1;_LL10:
# 2219
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3DB);
elt_type=_tmp3DB;
({void**_tmp7FE=({void**_tmp3D9=_cycalloc(sizeof(*_tmp3D9));*_tmp3D9=elt_type;_tmp3D9;});*t=_tmp7FE;});
num_elts=_tmp3E9;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2227
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2229
 No_sizeof: {
# 2232
struct Cyc_Absyn_Exp*_tmp3DC=*e;
{void*_tmp3DD=_tmp3DC->r;void*_tmp3DE=_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DF;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DE)->tag == 14U){_LL14: _tmp3DF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3DE)->f2;_LL15:
 _tmp3DC=_tmp3DF;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2237
{void*_tmp3E0=Cyc_Tcutil_compress((void*)_check_null(_tmp3DC->topt));void*_tmp3E1=_tmp3E0;void*_tmp3E5;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E1)->tag == 19U){_LL19: _tmp3E5=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E1)->f1;_LL1A:
# 2239
{void*_tmp3E2=Cyc_Tcutil_compress(_tmp3E5);void*_tmp3E3=_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E4;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E3)->tag == 18U){_LL1E: _tmp3E4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E3)->f1;_LL1F:
# 2241
 er=_tmp3E4->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2244
goto _LL18;}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2247
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7FF=({void**_tmp3E6=_cycalloc(sizeof(*_tmp3E6));*_tmp3E6=elt_type;_tmp3E6;});*t=_tmp7FF;});
num_elts=*e;
one_elt=0;}
# 2252
goto _LL5;}_LL5:;}}
# 2256
*is_fat=!one_elt;
# 2259
{void*_tmp3EA=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3ED;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA)->f1).aggr_info).KnownAggr).tag == 2){_LL23: _tmp3ED=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3EA)->f1).aggr_info).KnownAggr).val;_LL24:
# 2261
 if(_tmp3ED->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3ED->impl))->exist_vars != 0)
({void*_tmp3EB=0U;({unsigned int _tmp801=loc;struct _dyneither_ptr _tmp800=({const char*_tmp3EC="malloc with existential types not yet implemented";_tag_dyneither(_tmp3EC,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp801,_tmp800,_tag_dyneither(_tmp3EB,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2268
void*(*_tmp3EE)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp3EF=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp3F0=Cyc_Tcutil_compress(*topt);void*_tmp3F1=_tmp3F0;union Cyc_Absyn_Constraint*_tmp3FE;union Cyc_Absyn_Constraint*_tmp3FD;union Cyc_Absyn_Constraint*_tmp3FC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1)->tag == 5U){_LL28: _tmp3FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1)->f1).ptr_atts).nullable;_tmp3FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1)->f1).ptr_atts).bounds;_tmp3FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1)->f1).ptr_atts).zero_term;_LL29:
# 2273
 _tmp3EF=_tmp3FC;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3FE))
_tmp3EE=Cyc_Absyn_star_typ;
# 2278
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3FC) && !(*is_calloc)){
({void*_tmp3F2=0U;({unsigned int _tmp803=loc;struct _dyneither_ptr _tmp802=({const char*_tmp3F3="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp3F3,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp803,_tmp802,_tag_dyneither(_tmp3F2,sizeof(void*),0U));});});
*is_calloc=1;}
# 2284
{void*_tmp3F4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3FD);void*_tmp3F5=_tmp3F4;struct Cyc_Absyn_Exp*_tmp3FB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3F5)->tag == 0U){_LL2D: _LL2E:
 goto _LL2C;}else{_LL2F: _tmp3FB=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F5)->f1;if(!one_elt){_LL30: {
# 2287
int _tmp3F6=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp3F6  && Cyc_Evexp_same_const_exp(_tmp3FB,num_elts)){
*is_fat=0;
return({void*_tmp807=elt_type;void*_tmp806=rgn;struct Cyc_Absyn_Tqual _tmp805=Cyc_Absyn_empty_tqual(0U);void*_tmp804=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3FD);Cyc_Absyn_atb_typ(_tmp807,_tmp806,_tmp805,_tmp804,_tmp3EF);});}
# 2293
{void*_tmp3F7=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp3F8=_tmp3F7;void*_tmp3FA;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F8)->tag == 19U){_LL34: _tmp3FA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F8)->f1;_LL35: {
# 2295
struct Cyc_Absyn_Exp*_tmp3F9=({void*_tmp808=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp808,Cyc_Absyn_valueof_exp(_tmp3FA,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2297
if(Cyc_Evexp_same_const_exp(_tmp3F9,_tmp3FB)){
*is_fat=0;
return({void*_tmp80C=elt_type;void*_tmp80B=rgn;struct Cyc_Absyn_Tqual _tmp80A=Cyc_Absyn_empty_tqual(0U);void*_tmp809=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3FD);Cyc_Absyn_atb_typ(_tmp80C,_tmp80B,_tmp80A,_tmp809,_tmp3EF);});}
# 2302
goto _LL33;}}else{_LL36: _LL37:
 goto _LL33;}_LL33:;}
# 2305
goto _LL2C;}}else{_LL31: _LL32:
 goto _LL2C;}}_LL2C:;}
# 2308
goto _LL27;}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2311
if(!one_elt)_tmp3EE=Cyc_Absyn_dyneither_typ;
return({void*(*_tmp810)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=_tmp3EE;void*_tmp80F=elt_type;void*_tmp80E=rgn;struct Cyc_Absyn_Tqual _tmp80D=Cyc_Absyn_empty_tqual(0U);_tmp810(_tmp80F,_tmp80E,_tmp80D,_tmp3EF);});};};}
# 2316
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2318
struct Cyc_Tcenv_Tenv*_tmp3FF=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp3FF,0,e1);{
void*_tmp400=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp3FF,& _tmp400,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2325
{void*_tmp401=Cyc_Tcutil_compress(t1);void*_tmp402=_tmp401;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp402)->tag == 8U){_LL1: _LL2:
({void*_tmp403=0U;({unsigned int _tmp812=loc;struct _dyneither_ptr _tmp811=({const char*_tmp404="cannot assign to an array";_tag_dyneither(_tmp404,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp812,_tmp811,_tag_dyneither(_tmp403,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2330
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp405=0U;({unsigned int _tmp814=loc;struct _dyneither_ptr _tmp813=({const char*_tmp406="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp406,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp814,_tmp813,_tag_dyneither(_tmp405,sizeof(void*),0U));});});
# 2334
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp407=0U;({struct Cyc_Tcenv_Tenv*_tmp818=te;unsigned int _tmp817=e1->loc;void**_tmp816=topt;struct _dyneither_ptr _tmp815=({const char*_tmp408="swap non-lvalue";_tag_dyneither(_tmp408,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp818,_tmp817,_tmp816,_tmp815,_tag_dyneither(_tmp407,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp409=0U;({struct Cyc_Tcenv_Tenv*_tmp81C=te;unsigned int _tmp81B=e2->loc;void**_tmp81A=topt;struct _dyneither_ptr _tmp819=({const char*_tmp40A="swap non-lvalue";_tag_dyneither(_tmp40A,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp81C,_tmp81B,_tmp81A,_tmp819,_tag_dyneither(_tmp409,sizeof(void*),0U));});});
# 2339
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmp40E=({struct Cyc_String_pa_PrintArg_struct _tmp570;_tmp570.tag=0U,({
struct _dyneither_ptr _tmp81D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp570.f1=_tmp81D;});_tmp570;});struct Cyc_String_pa_PrintArg_struct _tmp40F=({struct Cyc_String_pa_PrintArg_struct _tmp56F;_tmp56F.tag=0U,({struct _dyneither_ptr _tmp81E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp56F.f1=_tmp81E;});_tmp56F;});void*_tmp40C[2U];_tmp40C[0]=& _tmp40E,_tmp40C[1]=& _tmp40F;({struct Cyc_Tcenv_Tenv*_tmp822=te;unsigned int _tmp821=loc;void**_tmp820=topt;struct _dyneither_ptr _tmp81F=({const char*_tmp40D="type mismatch: %s != %s";_tag_dyneither(_tmp40D,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp822,_tmp821,_tmp820,_tmp81F,_tag_dyneither(_tmp40C,sizeof(void*),2U));});});
return _tmp40B;}
# 2346
return(void*)& Cyc_Absyn_VoidType_val;};};}
# 2351
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp823=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp823,s,1);});
# 2354
while(1){
void*_tmp410=s->r;void*_tmp411=_tmp410;struct Cyc_Absyn_Decl*_tmp41C;struct Cyc_Absyn_Stmt*_tmp41B;struct Cyc_Absyn_Stmt*_tmp41A;struct Cyc_Absyn_Stmt*_tmp419;struct Cyc_Absyn_Exp*_tmp418;switch(*((int*)_tmp411)){case 1U: _LL1: _tmp418=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp411)->f1;_LL2: {
# 2358
void*_tmp412=(void*)_check_null(_tmp418->topt);
if(!({void*_tmp824=_tmp412;Cyc_Tcutil_unify(_tmp824,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp415=({struct Cyc_String_pa_PrintArg_struct _tmp571;_tmp571.tag=0U,({
struct _dyneither_ptr _tmp825=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp412));_tmp571.f1=_tmp825;});_tmp571;});void*_tmp413[1U];_tmp413[0]=& _tmp415;({unsigned int _tmp827=loc;struct _dyneither_ptr _tmp826=({const char*_tmp414="statement expression returns type %s";_tag_dyneither(_tmp414,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp827,_tmp826,_tag_dyneither(_tmp413,sizeof(void*),1U));});});
Cyc_Tcutil_explain_failure();}
# 2364
return _tmp412;}case 2U: _LL3: _tmp41A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp411)->f1;_tmp419=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp411)->f2;_LL4:
 s=_tmp419;continue;case 12U: _LL5: _tmp41C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp411)->f1;_tmp41B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp411)->f2;_LL6:
 s=_tmp41B;continue;default: _LL7: _LL8:
# 2368
 return({void*_tmp416=0U;({struct Cyc_Tcenv_Tenv*_tmp82B=te;unsigned int _tmp82A=loc;void**_tmp829=topt;struct _dyneither_ptr _tmp828=({const char*_tmp417="statement expression must end with expression";_tag_dyneither(_tmp417,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp82B,_tmp82A,_tmp829,_tmp828,_tag_dyneither(_tmp416,sizeof(void*),0U));});});}_LL0:;}}
# 2373
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp82C=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp82C,0,e);}));
{void*_tmp41D=t;struct Cyc_Absyn_Aggrdecl*_tmp421;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41D)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41D)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp421=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp41D)->f1).aggr_info).KnownAggr).val;_LL2:
# 2377
 if((_tmp421->kind == Cyc_Absyn_UnionA  && _tmp421->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp421->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{_LL3: _LL4:
# 2380
({struct Cyc_String_pa_PrintArg_struct _tmp420=({struct Cyc_String_pa_PrintArg_struct _tmp572;_tmp572.tag=0U,({struct _dyneither_ptr _tmp82D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp572.f1=_tmp82D;});_tmp572;});void*_tmp41E[1U];_tmp41E[0]=& _tmp420;({unsigned int _tmp82F=loc;struct _dyneither_ptr _tmp82E=({const char*_tmp41F="expecting @tagged union but found %s";_tag_dyneither(_tmp41F,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp82F,_tmp82E,_tag_dyneither(_tmp41E,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2383
return Cyc_Absyn_uint_typ;}
# 2387
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2391
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct Cyc_Tcenv_Tenv*_tmp422=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2395
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp429=_cycalloc(sizeof(*_tmp429));
_tmp429->tag=15U,({void*_tmp830=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp422));_tmp429->f1=_tmp830;});_tmp429;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp422,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp423=Cyc_Tcutil_compress(handle_type);void*_tmp424=_tmp423;void*_tmp428;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp424)->tag == 15U){_LL1: _tmp428=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp424)->f1;_LL2:
# 2400
 rgn=_tmp428;
Cyc_Tcenv_check_rgn_accessible(_tmp422,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2404
({struct Cyc_String_pa_PrintArg_struct _tmp427=({struct Cyc_String_pa_PrintArg_struct _tmp573;_tmp573.tag=0U,({
struct _dyneither_ptr _tmp831=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp573.f1=_tmp831;});_tmp573;});void*_tmp425[1U];_tmp425[0]=& _tmp427;({unsigned int _tmp833=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp832=({const char*_tmp426="expecting region_t type but found %s";_tag_dyneither(_tmp426,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp833,_tmp832,_tag_dyneither(_tmp425,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2411
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp834=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp834;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2419
void*_tmp42A=e1->r;void*_tmp42B=_tmp42A;struct Cyc_List_List*_tmp445;struct Cyc_Core_Opt*_tmp444;struct Cyc_List_List*_tmp443;switch(*((int*)_tmp42B)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2424
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp422,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp42C=0U;({struct _dyneither_ptr _tmp835=({const char*_tmp42D="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp42D,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp835,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});
({void*_tmp836=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp836;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp422,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp422,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2435
return res_typ;}case 36U: _LLA: _tmp444=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp42B)->f1;_tmp443=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp42B)->f2;_LLB:
# 2437
({void*_tmp837=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->tag=26U,_tmp42E->f1=_tmp443;_tmp42E;});e1->r=_tmp837;});
_tmp445=_tmp443;goto _LLD;case 26U: _LLC: _tmp445=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp42B)->f1;_LLD: {
# 2440
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp42F=Cyc_Tcutil_compress(*topt);void*_tmp430=_tmp42F;void**_tmp433;struct Cyc_Absyn_Tqual _tmp432;union Cyc_Absyn_Constraint*_tmp431;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->tag == 5U){_LL15: _tmp433=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).elt_typ;_tmp432=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).elt_tq;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).ptr_atts).zero_term;_LL16:
# 2446
 elt_typ_opt=_tmp433;
({int _tmp838=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp431);zero_term=_tmp838;});
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2452
void*res_typ=Cyc_Tcexp_tcArray(_tmp422,e1->loc,elt_typ_opt,0,zero_term,_tmp445);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp434=0U;({struct _dyneither_ptr _tmp839=({const char*_tmp435="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp435,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp839,_tag_dyneither(_tmp434,sizeof(void*),0U));});});
({void*_tmp83A=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp83A;});
if(topt != 0){
# 2461
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp422,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp422,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2467
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp42B)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2472
void*_tmp436=({void*_tmp83D=Cyc_Absyn_char_typ;void*_tmp83C=rgn;struct Cyc_Absyn_Tqual _tmp83B=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp83D,_tmp83C,_tmp83B,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2474
void*_tmp437=Cyc_Tcexp_tcExp(_tmp422,& _tmp436,e1);
return({void*_tmp842=_tmp437;void*_tmp841=rgn;struct Cyc_Absyn_Tqual _tmp840=Cyc_Absyn_empty_tqual(0U);void*_tmp83F=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=1U,({struct Cyc_Absyn_Exp*_tmp83E=Cyc_Absyn_uint_exp(1U,0U);_tmp438->f1=_tmp83E;});_tmp438;});Cyc_Absyn_atb_typ(_tmp842,_tmp841,_tmp840,_tmp83F,Cyc_Absyn_false_conref);});}case 9U: _LL10: _LL11: {
# 2479
void*_tmp439=({void*_tmp845=Cyc_Absyn_wchar_typ();void*_tmp844=rgn;struct Cyc_Absyn_Tqual _tmp843=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp845,_tmp844,_tmp843,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2481
void*_tmp43A=Cyc_Tcexp_tcExp(_tmp422,& _tmp439,e1);
return({void*_tmp84A=_tmp43A;void*_tmp849=rgn;struct Cyc_Absyn_Tqual _tmp848=Cyc_Absyn_empty_tqual(0U);void*_tmp847=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->tag=1U,({struct Cyc_Absyn_Exp*_tmp846=Cyc_Absyn_uint_exp(1U,0U);_tmp43B->f1=_tmp846;});_tmp43B;});Cyc_Absyn_atb_typ(_tmp84A,_tmp849,_tmp848,_tmp847,Cyc_Absyn_false_conref);});}default: goto _LL12;}default: _LL12: _LL13:
# 2488
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp43C=Cyc_Tcutil_compress(*topt);void*_tmp43D=_tmp43C;void**_tmp43F;struct Cyc_Absyn_Tqual _tmp43E;switch(*((int*)_tmp43D)){case 5U: _LL1A: _tmp43F=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->f1).elt_typ;_tmp43E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->f1).elt_tq;_LL1B:
# 2494
 topt2=_tmp43F;goto _LL19;case 3U: _LL1C: _LL1D:
# 2498
 bogus=*topt;
topt2=& bogus;
goto _LL19;default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2504
void*telt=Cyc_Tcexp_tcExp(_tmp422,topt2,e1);
# 2506
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp440=0U;({unsigned int _tmp84C=e1->loc;struct _dyneither_ptr _tmp84B=({const char*_tmp441="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp441,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp84C,_tmp84B,_tag_dyneither(_tmp440,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp442=_cycalloc(sizeof(*_tmp442));
_tmp442->tag=5U,(_tmp442->f1).elt_typ=telt,({struct Cyc_Absyn_Tqual _tmp84E=Cyc_Absyn_empty_tqual(0U);(_tmp442->f1).elt_tq=_tmp84E;}),
((_tmp442->f1).ptr_atts).rgn=rgn,({union Cyc_Absyn_Constraint*_tmp84D=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp442->f1).ptr_atts).nullable=_tmp84D;}),((_tmp442->f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,((_tmp442->f1).ptr_atts).zero_term=Cyc_Absyn_false_conref,((_tmp442->f1).ptr_atts).ptrloc=0;_tmp442;});
# 2513
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp422,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp422,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2520
return res_typ;};};}}_LL5:;};}
# 2526
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp851=({void*_tmp850=({void*_tmp84F=t;Cyc_Tcutil_promote_array(_tmp84F,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp850;});e->topt=_tmp851;});
return t;}
# 2536
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2540
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2545
({void*_tmp446=0U;({unsigned int _tmp853=e->loc;struct _dyneither_ptr _tmp852=({const char*_tmp447="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp447,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp853,_tmp852,_tag_dyneither(_tmp446,sizeof(void*),0U));});});{
# 2547
void*_tmp448=e->r;void*_tmp449=_tmp448;switch(*((int*)_tmp449)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp449)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2554
({void*_tmp854=Cyc_Tcutil_compress(t);t=_tmp854;});
if(Cyc_Tcutil_is_array(t)){
({void*_tmp856=({void*_tmp855=t;Cyc_Tcutil_promote_array(_tmp855,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp856;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2559
return t;}_LL0:;};}
# 2570 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp44A=e->r;void*_tmp44B=_tmp44A;struct Cyc_Absyn_Exp*_tmp469;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp44B)->tag == 12U){_LL1: _tmp469=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp44B)->f1;_LL2:
# 2574
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp469);
({void*_tmp857=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp469->topt),0);_tmp469->topt=_tmp857;});
e->topt=_tmp469->topt;
goto _LL0;}else{_LL3: _LL4:
# 2580
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp858=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp858;});
# 2583
{void*_tmp44C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp44D=_tmp44C;void*_tmp468;struct Cyc_Absyn_Tqual _tmp467;void*_tmp466;union Cyc_Absyn_Constraint*_tmp465;union Cyc_Absyn_Constraint*_tmp464;union Cyc_Absyn_Constraint*_tmp463;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->tag == 5U){_LL6: _tmp468=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->f1).elt_typ;_tmp467=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->f1).elt_tq;_tmp466=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->f1).ptr_atts).rgn;_tmp465=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->f1).ptr_atts).nullable;_tmp464=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->f1).ptr_atts).bounds;_tmp463=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44D)->f1).ptr_atts).zero_term;_LL7:
# 2585
{void*_tmp44E=Cyc_Tcutil_compress(_tmp468);void*_tmp44F=_tmp44E;struct Cyc_List_List*_tmp462;void*_tmp461;struct Cyc_Absyn_Tqual _tmp460;void*_tmp45F;struct Cyc_List_List*_tmp45E;int _tmp45D;struct Cyc_Absyn_VarargInfo*_tmp45C;struct Cyc_List_List*_tmp45B;struct Cyc_List_List*_tmp45A;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_List_List*_tmp458;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_List_List*_tmp456;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->tag == 9U){_LLB: _tmp462=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).tvars;_tmp461=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).effect;_tmp460=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).ret_tqual;_tmp45F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).ret_typ;_tmp45E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).args;_tmp45D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).c_varargs;_tmp45C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).cyc_varargs;_tmp45B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).rgn_po;_tmp45A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).attributes;_tmp459=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).requires_clause;_tmp458=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).requires_relns;_tmp457=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).ensures_clause;_tmp456=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).ensures_relns;_LLC:
# 2587
 if(_tmp462 != 0){
struct _tuple12 _tmp450=({struct _tuple12 _tmp574;({struct Cyc_List_List*_tmp859=Cyc_Tcenv_lookup_type_vars(te);_tmp574.f1=_tmp859;}),_tmp574.f2=Cyc_Core_heap_region;_tmp574;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp450,_tmp462);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2595
({struct Cyc_List_List*_tmp85A=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp45B);_tmp45B=_tmp85A;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp45B);{
void*ftyp=({struct Cyc_List_List*_tmp85B=inst;Cyc_Tcutil_substitute(_tmp85B,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp455=_cycalloc(sizeof(*_tmp455));
_tmp455->tag=9U,(_tmp455->f1).tvars=0,(_tmp455->f1).effect=_tmp461,(_tmp455->f1).ret_tqual=_tmp460,(_tmp455->f1).ret_typ=_tmp45F,(_tmp455->f1).args=_tmp45E,(_tmp455->f1).c_varargs=_tmp45D,(_tmp455->f1).cyc_varargs=_tmp45C,(_tmp455->f1).rgn_po=0,(_tmp455->f1).attributes=_tmp45A,(_tmp455->f1).requires_clause=_tmp459,(_tmp455->f1).requires_relns=_tmp458,(_tmp455->f1).ensures_clause=_tmp457,(_tmp455->f1).ensures_relns=_tmp456;_tmp455;}));});
# 2602
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp451=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->tag=5U,(_tmp454->f1).elt_typ=ftyp,(_tmp454->f1).elt_tq=_tmp467,((_tmp454->f1).ptr_atts).rgn=_tmp466,((_tmp454->f1).ptr_atts).nullable=_tmp465,((_tmp454->f1).ptr_atts).bounds=_tmp464,((_tmp454->f1).ptr_atts).zero_term=_tmp463,((_tmp454->f1).ptr_atts).ptrloc=0;_tmp454;});
# 2604
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_copy_exp(e);
({void*_tmp85C=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->tag=13U,_tmp453->f1=_tmp452,_tmp453->f2=ts;_tmp453;});e->r=_tmp85C;});
e->topt=(void*)_tmp451;};}
# 2608
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2611
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2614
goto _LL0;}_LL0:;}
# 2616
return(void*)_check_null(e->topt);}
# 2624
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2626
struct Cyc_List_List*_tmp46A=0;
# 2628
{void*_tmp46B=fn_exp->r;void*_tmp46C=_tmp46B;struct Cyc_List_List*_tmp480;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->tag == 10U){_LL1: _tmp480=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46C)->f2;_LL2:
# 2630
{void*_tmp46D=e->r;void*_tmp46E=_tmp46D;struct Cyc_List_List*_tmp47D;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->tag == 10U){_LL6: _tmp47D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46E)->f2;_LL7: {
# 2632
struct Cyc_List_List*_tmp46F=alias_arg_exps;
int _tmp470=0;
while(_tmp46F != 0){
while(_tmp470 != (int)_tmp46F->hd){
if(_tmp480 == 0)
({void*_tmp471=0U;({struct _dyneither_ptr _tmp85E=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp474=({struct Cyc_Int_pa_PrintArg_struct _tmp576;_tmp576.tag=1U,_tmp576.f1=(unsigned long)_tmp470;_tmp576;});struct Cyc_Int_pa_PrintArg_struct _tmp475=({struct Cyc_Int_pa_PrintArg_struct _tmp575;_tmp575.tag=1U,_tmp575.f1=(unsigned long)((int)_tmp46F->hd);_tmp575;});void*_tmp472[2U];_tmp472[0]=& _tmp474,_tmp472[1]=& _tmp475;({struct _dyneither_ptr _tmp85D=({const char*_tmp473="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp473,sizeof(char),36U);});Cyc_aprintf(_tmp85D,_tag_dyneither(_tmp472,sizeof(void*),2U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp85E,_tag_dyneither(_tmp471,sizeof(void*),0U));});});
# 2639
++ _tmp470;
_tmp480=_tmp480->tl;
_tmp47D=((struct Cyc_List_List*)_check_null(_tmp47D))->tl;}{
# 2644
struct _tuple11 _tmp476=({struct Cyc_Absyn_Exp*_tmp85F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp480))->hd;Cyc_Tcutil_insert_alias(_tmp85F,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp47D))->hd)->topt)));});struct _tuple11 _tmp477=_tmp476;struct Cyc_Absyn_Decl*_tmp47A;struct Cyc_Absyn_Exp*_tmp479;_LLB: _tmp47A=_tmp477.f1;_tmp479=_tmp477.f2;_LLC:;
_tmp480->hd=(void*)_tmp479;
({struct Cyc_List_List*_tmp860=({struct Cyc_List_List*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->hd=_tmp47A,_tmp478->tl=_tmp46A;_tmp478;});_tmp46A=_tmp860;});
_tmp46F=_tmp46F->tl;};}
# 2649
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp47B=0U;({struct _dyneither_ptr _tmp861=({const char*_tmp47C="not a function call!";_tag_dyneither(_tmp47C,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp861,_tag_dyneither(_tmp47B,sizeof(void*),0U));});});}_LL5:;}
# 2652
goto _LL0;}else{_LL3: _LL4:
({void*_tmp47E=0U;({struct _dyneither_ptr _tmp862=({const char*_tmp47F="not a function call!";_tag_dyneither(_tmp47F,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp862,_tag_dyneither(_tmp47E,sizeof(void*),0U));});});}_LL0:;}
# 2657
while(_tmp46A != 0){
struct Cyc_Absyn_Decl*_tmp481=(struct Cyc_Absyn_Decl*)_tmp46A->hd;
({struct Cyc_Absyn_Exp*_tmp866=({struct Cyc_Absyn_Stmt*_tmp865=({struct Cyc_Absyn_Decl*_tmp864=_tmp481;struct Cyc_Absyn_Stmt*_tmp863=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp864,_tmp863,e->loc);});Cyc_Absyn_stmt_exp(_tmp865,e->loc);});fn_exp=_tmp866;});
_tmp46A=_tmp46A->tl;}
# 2664
e->topt=0;
e->r=fn_exp->r;}
# 2670
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2674
{void*_tmp482=e->r;void*_tmp483=_tmp482;void*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct _dyneither_ptr*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;int*_tmp4E7;struct Cyc_Absyn_Exp**_tmp4E6;void***_tmp4E5;struct Cyc_Absyn_Exp**_tmp4E4;int*_tmp4E3;void*_tmp4E2;struct Cyc_Absyn_Enumfield*_tmp4E1;struct Cyc_Absyn_Enumdecl*_tmp4E0;struct Cyc_Absyn_Enumfield*_tmp4DF;struct Cyc_List_List*_tmp4DE;struct Cyc_Absyn_Datatypedecl*_tmp4DD;struct Cyc_Absyn_Datatypefield*_tmp4DC;void*_tmp4DB;struct Cyc_List_List*_tmp4DA;struct _tuple1**_tmp4D9;struct Cyc_List_List**_tmp4D8;struct Cyc_List_List*_tmp4D7;struct Cyc_Absyn_Aggrdecl**_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;void*_tmp4D4;int*_tmp4D3;struct Cyc_Absyn_Vardecl*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D0;int*_tmp4CF;struct Cyc_Absyn_Stmt*_tmp4CE;struct Cyc_List_List*_tmp4CD;struct _tuple9*_tmp4CC;struct Cyc_List_List*_tmp4CB;struct Cyc_List_List*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C7;struct _dyneither_ptr*_tmp4C6;int*_tmp4C5;int*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C3;struct _dyneither_ptr*_tmp4C2;int*_tmp4C1;int*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4BF;void*_tmp4BE;struct Cyc_List_List*_tmp4BD;void*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp**_tmp4BA;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4B8;void*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B6;enum Cyc_Absyn_Coercion*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_List_List*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Core_Opt*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;enum Cyc_Absyn_Incrementor _tmp4A4;enum Cyc_Absyn_Primop _tmp4A3;struct Cyc_List_List*_tmp4A2;void**_tmp4A1;union Cyc_Absyn_Cnst*_tmp4A0;struct Cyc_Core_Opt*_tmp49F;struct Cyc_List_List*_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_List_List*_tmp49C;struct Cyc_Absyn_VarargCallInfo**_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_VarargCallInfo**_tmp498;int*_tmp497;struct Cyc_Absyn_Exp*_tmp496;switch(*((int*)_tmp483)){case 12U: _LL1: _tmp496=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL2:
# 2679
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp496);
return;case 10U: _LL3: _tmp49A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp499=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp498=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_tmp497=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp483)->f4;if(!(*_tmp497)){_LL4:
# 2683
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp867=({const char*_tmp484="unresolved function in tcExpNoInst";_tag_dyneither(_tmp484,sizeof(char),35U);});_tmp485->f1=_tmp867;});_tmp485;}));}else{_LL1B: _tmp49D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp49C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp49B=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_LL1C: {
# 2716
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp486;_push_handler(& _tmp486);{int _tmp488=0;if(setjmp(_tmp486.handler))_tmp488=1;if(!_tmp488){
({struct Cyc_Absyn_Exp*_tmp868=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp868;});;_pop_handler();}else{void*_tmp487=(void*)_exn_thrown;void*_tmp489=_tmp487;void*_tmp48A;if(((struct Cyc_Core_Failure_exn_struct*)_tmp489)->tag == Cyc_Core_Failure){_LL58: _LL59:
# 2723
 ok=0;
fn_exp=e;
goto _LL57;}else{_LL5A: _tmp48A=_tmp489;_LL5B:(int)_rethrow(_tmp48A);}_LL57:;}};}
# 2727
({void*_tmp869=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp49D,_tmp49C,_tmp49B,& alias_arg_exps);t=_tmp869;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp86A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp86A;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2734
goto _LL0;}}case 36U: _LL5: _tmp49F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp49E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL6:
# 2687
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp49E);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp4A0=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL8:
# 2692
({void*_tmp86B=Cyc_Tcexp_tcConst(te,loc,topt,_tmp4A0,e);t=_tmp86B;});goto _LL0;case 1U: _LL9: _tmp4A1=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LLA:
# 2694
({void*_tmp86C=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp4A1);t=_tmp86C;});goto _LL0;case 2U: _LLB: _LLC:
# 2696
 t=Cyc_Absyn_sint_typ;goto _LL0;case 3U: _LLD: _tmp4A3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4A2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LLE:
# 2698
({void*_tmp86D=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4A3,_tmp4A2);t=_tmp86D;});goto _LL0;case 5U: _LLF: _tmp4A5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4A4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL10:
# 2700
({void*_tmp86E=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp4A5,_tmp4A4);t=_tmp86E;});goto _LL0;case 4U: _LL11: _tmp4A8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4A7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4A6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_LL12:
# 2702
({void*_tmp86F=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4A8,_tmp4A7,_tmp4A6);t=_tmp86F;});goto _LL0;case 6U: _LL13: _tmp4AB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4AA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4A9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_LL14:
# 2704
({void*_tmp870=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp4AB,_tmp4AA,_tmp4A9);t=_tmp870;});goto _LL0;case 7U: _LL15: _tmp4AD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4AC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL16:
# 2706
({void*_tmp871=Cyc_Tcexp_tcAnd(te,loc,_tmp4AD,_tmp4AC);t=_tmp871;});goto _LL0;case 8U: _LL17: _tmp4AF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4AE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL18:
# 2708
({void*_tmp872=Cyc_Tcexp_tcOr(te,loc,_tmp4AF,_tmp4AE);t=_tmp872;});goto _LL0;case 9U: _LL19: _tmp4B1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4B0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL1A:
# 2710
({void*_tmp873=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4B1,_tmp4B0);t=_tmp873;});goto _LL0;case 11U: _LL1D: _tmp4B2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL1E:
# 2736
({void*_tmp874=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4B2);t=_tmp874;});goto _LL0;case 13U: _LL1F: _tmp4B4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4B3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL20:
# 2738
({void*_tmp875=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4B4,_tmp4B3);t=_tmp875;});goto _LL0;case 14U: _LL21: _tmp4B7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4B6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4B5=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp483)->f4;_LL22:
# 2740
({void*_tmp876=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4B7,_tmp4B6,_tmp4B5);t=_tmp876;});goto _LL0;case 15U: _LL23: _tmp4B8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL24:
# 2742
({void*_tmp877=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4B8);t=_tmp877;});goto _LL0;case 16U: _LL25: _tmp4BA=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4B9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL26:
# 2744
({void*_tmp878=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4BA,e,_tmp4B9);t=_tmp878;});goto _LL0;case 18U: _LL27: _tmp4BB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL28: {
# 2746
void*_tmp48B=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4BB);
({void*_tmp879=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp48B);t=_tmp879;});goto _LL0;}case 17U: _LL29: _tmp4BC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL2A:
# 2749
({void*_tmp87A=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4BC);t=_tmp87A;});goto _LL0;case 19U: _LL2B: _tmp4BE=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4BD=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL2C:
# 2751
({void*_tmp87B=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4BE,_tmp4BD);t=_tmp87B;});goto _LL0;case 20U: _LL2D: _tmp4BF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL2E:
# 2753
({void*_tmp87C=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4BF);t=_tmp87C;});goto _LL0;case 21U: _LL2F: _tmp4C3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4C2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4C1=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_tmp4C0=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp483)->f4;_LL30:
# 2755
({void*_tmp87D=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4C3,_tmp4C2,_tmp4C1,_tmp4C0);t=_tmp87D;});goto _LL0;case 22U: _LL31: _tmp4C7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4C6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4C5=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_tmp4C4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp483)->f4;_LL32:
# 2757
({void*_tmp87E=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4C7,_tmp4C6,_tmp4C5,_tmp4C4);t=_tmp87E;});goto _LL0;case 23U: _LL33: _tmp4C9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4C8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL34:
# 2759
({void*_tmp87F=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4C9,_tmp4C8);t=_tmp87F;});goto _LL0;case 24U: _LL35: _tmp4CA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL36:
# 2761
({void*_tmp880=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4CA);t=_tmp880;});goto _LL0;case 25U: _LL37: _tmp4CC=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4CB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL38:
# 2763
({void*_tmp881=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4CC,_tmp4CB);t=_tmp881;});goto _LL0;case 26U: _LL39: _tmp4CD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL3A: {
# 2767
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp48C=Cyc_Tcutil_compress(*topt);void*_tmp48D=_tmp48C;void**_tmp490;struct Cyc_Absyn_Tqual*_tmp48F;union Cyc_Absyn_Constraint*_tmp48E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48D)->tag == 8U){_LL5D: _tmp490=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48D)->f1).elt_type;_tmp48F=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48D)->f1).tq;_tmp48E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48D)->f1).zero_term;_LL5E:
# 2773
 elt_topt=_tmp490;
elt_tqopt=_tmp48F;
({int _tmp882=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp48E);zero_term=_tmp882;});
goto _LL5C;}else{_LL5F: _LL60:
 goto _LL5C;}_LL5C:;}
# 2780
({void*_tmp883=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4CD);t=_tmp883;});goto _LL0;}case 37U: _LL3B: _tmp4CE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL3C:
# 2782
({void*_tmp884=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4CE);t=_tmp884;});goto _LL0;case 27U: _LL3D: _tmp4D2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4D1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4D0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_tmp4CF=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp483)->f4;_LL3E:
# 2784
({void*_tmp885=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4D2,_tmp4D1,_tmp4D0,_tmp4CF);t=_tmp885;});goto _LL0;case 28U: _LL3F: _tmp4D5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4D4=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4D3=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_LL40:
# 2786
({void*_tmp886=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4D5,_tmp4D4,_tmp4D3);t=_tmp886;});goto _LL0;case 29U: _LL41: _tmp4D9=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4D8=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4D7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_tmp4D6=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp483)->f4;_LL42:
# 2788
({void*_tmp887=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4D9,_tmp4D8,_tmp4D7,_tmp4D6);t=_tmp887;});goto _LL0;case 30U: _LL43: _tmp4DB=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4DA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL44:
# 2790
({void*_tmp888=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4DB,_tmp4DA);t=_tmp888;});goto _LL0;case 31U: _LL45: _tmp4DE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4DD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_tmp4DC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp483)->f3;_LL46:
# 2792
({void*_tmp889=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp4DE,_tmp4DD,_tmp4DC);t=_tmp889;});goto _LL0;case 32U: _LL47: _tmp4E0=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4DF=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL48:
# 2794
({void*_tmp88A=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->tag=13U,_tmp491->f1=_tmp4E0->name,_tmp491->f2=_tmp4E0;_tmp491;});t=_tmp88A;});goto _LL0;case 33U: _LL49: _tmp4E2=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4E1=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL4A:
# 2796
 t=_tmp4E2;goto _LL0;case 34U: _LL4B: _tmp4E7=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp483)->f1).is_calloc;_tmp4E6=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp483)->f1).rgn;_tmp4E5=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp483)->f1).elt_type;_tmp4E4=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp483)->f1).num_elts;_tmp4E3=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp483)->f1).fat_result;_LL4C:
# 2798
({void*_tmp88B=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4E6,_tmp4E5,_tmp4E4,_tmp4E7,_tmp4E3);t=_tmp88B;});goto _LL0;case 35U: _LL4D: _tmp4E9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4E8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL4E:
# 2800
({void*_tmp88C=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4E9,_tmp4E8);t=_tmp88C;});goto _LL0;case 38U: _LL4F: _tmp4EB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_tmp4EA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp483)->f2;_LL50:
# 2802
({void*_tmp88D=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp4EB,_tmp4EA);t=_tmp88D;});goto _LL0;case 41U: _LL51: _tmp4EC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL52:
# 2804
({void*_tmp88E=Cyc_Tcexp_tcExp(te,topt,_tmp4EC);t=_tmp88E;});goto _LL0;case 39U: _LL53: _tmp4ED=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp483)->f1;_LL54:
# 2806
 if(!te->allow_valueof)
({void*_tmp492=0U;({unsigned int _tmp890=e->loc;struct _dyneither_ptr _tmp88F=({const char*_tmp493="valueof(-) can only occur within types";_tag_dyneither(_tmp493,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp890,_tmp88F,_tag_dyneither(_tmp492,sizeof(void*),0U));});});
# 2814
t=Cyc_Absyn_sint_typ;
goto _LL0;default: _LL55: _LL56:
# 2817
({void*_tmp494=0U;({unsigned int _tmp892=e->loc;struct _dyneither_ptr _tmp891=({const char*_tmp495="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp495,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp892,_tmp891,_tag_dyneither(_tmp494,sizeof(void*),0U));});});
({void*_tmp893=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp893;});}_LL0:;}
# 2820
e->topt=t;}
