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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 379
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 316
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 428 "absyn.h"
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 430
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
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 932
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 940
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 946
void*Cyc_Absyn_compress_kb(void*);
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 953
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 956
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 958
extern void*Cyc_Absyn_sint_typ;
# 960
void*Cyc_Absyn_float_typ(int);
# 962
extern void*Cyc_Absyn_empty_effect;
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 72
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 137
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 53
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 58
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 64
int Cyc_Tcutil_coerceable(void*);
# 71
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 147
int Cyc_Tcutil_unify(void*,void*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 181
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 223 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 226
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 241
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 266
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 274
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 294
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 358
int Cyc_Tcutil_is_array(void*t);
# 362
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
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
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 113
void Cyc_Tcpat_print_decision_tree(void*);
# 115
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 42 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Aggrdecl**_tmp2B;struct Cyc_List_List*_tmp2A;struct Cyc_List_List**_tmp29;struct Cyc_List_List*_tmp28;int _tmp27;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp25;int _tmp24;switch(*((int*)_tmp1)){case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp26=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp25=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp24=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2:
# 48
 if(topt == 0)
({void*_tmp2=0U;({unsigned int _tmp3E0=p->loc;struct _dyneither_ptr _tmp3DF=({const char*_tmp3="cannot determine pattern type";_tag_dyneither(_tmp3,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp3E0,_tmp3DF,_tag_dyneither(_tmp2,sizeof(void*),0U));});});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp4=t;struct Cyc_Absyn_AggrInfo _tmpA;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->tag == 11U){_LL10: _tmpA=((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1;_LL11:
# 53
({void*_tmp3E2=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=7U,({struct Cyc_Absyn_AggrInfo*_tmp3E1=({struct Cyc_Absyn_AggrInfo*_tmp5=_cycalloc(sizeof(*_tmp5));*_tmp5=_tmpA;_tmp5;});_tmp6->f1=_tmp3E1;}),_tmp6->f2=_tmp26,_tmp6->f3=_tmp25,_tmp6->f4=_tmp24;_tmp6;});p->r=_tmp3E2;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}else{_LL12: _LL13:
# 57
({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,({struct _dyneither_ptr _tmp3E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp38E.f1=_tmp3E3;});_tmp38E;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({unsigned int _tmp3E5=p->loc;struct _dyneither_ptr _tmp3E4=({const char*_tmp8="pattern expects aggregate type instead of %s";_tag_dyneither(_tmp8,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp3E5,_tmp3E4,_tag_dyneither(_tmp7,sizeof(void*),1U));});});
goto _LLF;}_LLF:;}
# 60
return;};}else{if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).tag == 2){_LL3: _tmp2B=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).val;_tmp2A=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->targs;_tmp29=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp28=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp27=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {
# 63
struct Cyc_Absyn_Aggrdecl*_tmpB=*_tmp2B;
if(_tmpB->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,({
struct _dyneither_ptr _tmp3E6=(struct _dyneither_ptr)(_tmpB->kind == Cyc_Absyn_StructA?({const char*_tmpF="struct";_tag_dyneither(_tmpF,sizeof(char),7U);}):({const char*_tmp10="union";_tag_dyneither(_tmp10,sizeof(char),6U);}));_tmp38F.f1=_tmp3E6;});_tmp38F;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned int _tmp3E8=p->loc;struct _dyneither_ptr _tmp3E7=({const char*_tmpD="can't destructure an abstract %s";_tag_dyneither(_tmpD,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp3E8,_tmp3E7,_tag_dyneither(_tmpC,sizeof(void*),1U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 70
int more_exists=({int _tmp3E9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);_tmp3E9 - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp29);});
if(more_exists < 0){
({void*_tmp11=0U;({unsigned int _tmp3EB=p->loc;struct _dyneither_ptr _tmp3EA=({const char*_tmp12="too many existentially bound type variables in pattern";_tag_dyneither(_tmp12,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp3EB,_tmp3EA,_tag_dyneither(_tmp11,sizeof(void*),0U));});});{
struct Cyc_List_List**_tmp13=_tmp29;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);for(0;n != 0;-- n){
_tmp13=&((struct Cyc_List_List*)_check_null(*_tmp13))->tl;}}
*_tmp13=0;};}else{
if(more_exists > 0){
# 79
struct Cyc_List_List*_tmp14=0;
for(0;more_exists != 0;-- more_exists){
({struct Cyc_List_List*_tmp3ED=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Absyn_Tvar*_tmp3EC=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=1U,_tmp15->f1=0;_tmp15;}));_tmp16->hd=_tmp3EC;}),_tmp16->tl=_tmp14;_tmp16;});_tmp14=_tmp3ED;});}
({struct Cyc_List_List*_tmp3EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp29,_tmp14);*_tmp29=_tmp3EE;});}}
# 84
return;};}}else{_LLB: _LLC:
# 97
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp3EF=({const char*_tmp22="resolve_pat unknownAggr";_tag_dyneither(_tmp22,sizeof(char),24U);});_tmp23->f1=_tmp3EF;});_tmp23;}));}}case 17U: _LL5: _tmp2C=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6:
# 86
 Cyc_Tcexp_tcExp(te,0,_tmp2C);
if(!Cyc_Tcutil_is_const_exp(_tmp2C)){
({void*_tmp17=0U;({unsigned int _tmp3F1=p->loc;struct _dyneither_ptr _tmp3F0=({const char*_tmp18="non-constant expression in case pattern";_tag_dyneither(_tmp18,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp3F1,_tmp3F0,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 91
struct _tuple14 _tmp19=Cyc_Evexp_eval_const_uint_exp(_tmp2C);struct _tuple14 _tmp1A=_tmp19;unsigned int _tmp1D;int _tmp1C;_LL15: _tmp1D=_tmp1A.f1;_tmp1C=_tmp1A.f2;_LL16:;
({void*_tmp3F2=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=10U,_tmp1B->f1=Cyc_Absyn_None,_tmp1B->f2=(int)_tmp1D;_tmp1B;});p->r=_tmp3F2;});
return;};case 15U: _LL7: _LL8:
# 95
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp3F3=({const char*_tmp1E="resolve_pat UnknownId_p";_tag_dyneither(_tmp1E,sizeof(char),24U);});_tmp1F->f1=_tmp3F3;});_tmp1F;}));case 16U: _LL9: _LLA:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp3F4=({const char*_tmp20="resolve_pat UnknownCall_p";_tag_dyneither(_tmp20,sizeof(char),26U);});_tmp21->f1=_tmp3F4;});_tmp21;}));default: _LLD: _LLE:
# 99
 return;}_LL0:;}
# 103
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 106
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->v=s;_tmp2D;}));}
# 111
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 115
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 118
{void*_tmp2E=Cyc_Tcutil_compress(numt);void*_tmp2F=_tmp2E;switch(*((int*)_tmp2F)){case 13U: _LL1: _LL2:
 goto _LL4;case 14U: _LL3: _LL4:
 if(topt != 0)return*topt;goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 123
return numt;}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 126
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 128
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp3F5=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp3F5;});}
# 132
({struct Cyc_List_List*_tmp3F7=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _tuple15*_tmp3F6=({struct _tuple15*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->f1=vd,_tmp30->f2=e;_tmp30;});_tmp31->hd=_tmp3F6;}),_tmp31->tl=*v_result_ptr;_tmp31;});*v_result_ptr=_tmp3F7;});}
# 134
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 136
struct Cyc_Tcpat_TcPatResult _tmp32=res1;struct _tuple1*_tmp3A;struct Cyc_List_List*_tmp39;_LL1: _tmp3A=_tmp32.tvars_and_bounds_opt;_tmp39=_tmp32.patvars;_LL2:;{
struct Cyc_Tcpat_TcPatResult _tmp33=res2;struct _tuple1*_tmp38;struct Cyc_List_List*_tmp37;_LL4: _tmp38=_tmp33.tvars_and_bounds_opt;_tmp37=_tmp33.patvars;_LL5:;
if(_tmp3A != 0  || _tmp38 != 0){
if(_tmp3A == 0)
({struct _tuple1*_tmp3F8=({struct _tuple1*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->f1=0,_tmp34->f2=0;_tmp34;});_tmp3A=_tmp3F8;});
if(_tmp38 == 0)
({struct _tuple1*_tmp3F9=({struct _tuple1*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->f1=0,_tmp35->f2=0;_tmp35;});_tmp38=_tmp3F9;});
return({struct Cyc_Tcpat_TcPatResult _tmp390;({struct _tuple1*_tmp3FD=({struct _tuple1*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_List_List*_tmp3FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp3A).f1,(*_tmp38).f1);_tmp36->f1=_tmp3FC;}),({
struct Cyc_List_List*_tmp3FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp3A).f2,(*_tmp38).f2);_tmp36->f2=_tmp3FB;});_tmp36;});
# 143
_tmp390.tvars_and_bounds_opt=_tmp3FD;}),({
# 145
struct Cyc_List_List*_tmp3FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,_tmp37);_tmp390.patvars=_tmp3FA;});_tmp390;});}
# 147
return({struct Cyc_Tcpat_TcPatResult _tmp391;_tmp391.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp3FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,_tmp37);_tmp391.patvars=_tmp3FE;});_tmp391;});};}
# 150
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
return({struct Cyc_Absyn_Pat*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->loc=loc,_tmp3B->topt=0,_tmp3B->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp3B;});}
# 155
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array(t)?({
void*_tmp400=t;Cyc_Tcutil_promote_array(_tmp400,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp3C=_cycalloc(sizeof(*_tmp3C));({struct Cyc_List_List*_tmp3FF=Cyc_Tcenv_lookup_type_vars(te);_tmp3C->v=_tmp3FF;});_tmp3C;})): rgn_opt,0);}): t;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};
# 162
static struct _tuple16*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple16*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->f1=tv,_tmp3D->f2=0;_tmp3D;});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 166
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 169
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp3A4;_tmp3A4.tvars_and_bounds_opt=0,_tmp3A4.patvars=0;_tmp3A4;});
# 174
{void*_tmp3E=p->r;void*_tmp3F=_tmp3E;struct Cyc_Absyn_Datatypedecl*_tmpFA;struct Cyc_Absyn_Datatypefield*_tmpF9;struct Cyc_List_List**_tmpF8;int _tmpF7;struct Cyc_Absyn_Aggrdecl*_tmpF6;struct Cyc_List_List**_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_List_List**_tmpF3;int _tmpF2;struct Cyc_List_List**_tmpF1;int _tmpF0;struct Cyc_Absyn_Pat*_tmpEF;void*_tmpEE;struct Cyc_Absyn_Enumdecl*_tmpED;int _tmpEC;struct Cyc_Absyn_Tvar*_tmpEB;struct Cyc_Absyn_Vardecl*_tmpEA;struct Cyc_Absyn_Vardecl*_tmpE9;struct Cyc_Absyn_Pat*_tmpE8;struct Cyc_Absyn_Tvar*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE5;struct Cyc_Absyn_Pat*_tmpE4;switch(*((int*)_tmp3F)){case 0U: _LL1: _LL2:
# 177
 if(topt != 0)
t=*topt;else{
# 180
({void*_tmp402=({struct Cyc_List_List*_tmp401=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp401,topt);});t=_tmp402;});}
goto _LL0;case 1U: _LL3: _tmpE5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_tmpE4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_LL4: {
# 184
struct _tuple2*_tmp40=_tmpE5->name;struct _tuple2*_tmp41=_tmp40;struct _dyneither_ptr _tmp4E;_LL2E: _tmp4E=*_tmp41->f2;_LL2F:;
if(({struct _dyneither_ptr _tmp404=(struct _dyneither_ptr)_tmp4E;Cyc_strcmp(_tmp404,({const char*_tmp42="true";_tag_dyneither(_tmp42,sizeof(char),5U);}));})== 0  || ({struct _dyneither_ptr _tmp403=(struct _dyneither_ptr)_tmp4E;Cyc_strcmp(_tmp403,({const char*_tmp43="false";_tag_dyneither(_tmp43,sizeof(char),6U);}));})== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp46=({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0U,_tmp392.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4E);_tmp392;});void*_tmp44[1U];_tmp44[0]=& _tmp46;({unsigned int _tmp406=p->loc;struct _dyneither_ptr _tmp405=({const char*_tmp45="you probably do not want to use %s as a local variable";_tag_dyneither(_tmp45,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp406,_tmp405,_tag_dyneither(_tmp44,sizeof(void*),1U));});});
({struct Cyc_Tcpat_TcPatResult _tmp407=Cyc_Tcpat_tcPatRec(te,_tmpE4,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp407;});
t=(void*)_check_null(_tmpE4->topt);
# 191
{void*_tmp47=Cyc_Tcutil_compress(t);void*_tmp48=_tmp47;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->tag == 8U){_LL31: _LL32:
# 193
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp49=0U;({unsigned int _tmp409=p->loc;struct _dyneither_ptr _tmp408=({const char*_tmp4A="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp4A,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp409,_tmp408,_tag_dyneither(_tmp49,sizeof(void*),0U));});});
goto _LL30;}else{_LL33: _LL34:
# 197
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp4B=0U;({unsigned int _tmp40B=p->loc;struct _dyneither_ptr _tmp40A=({const char*_tmp4C="pattern would point to an abstract member";_tag_dyneither(_tmp4C,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp40B,_tmp40A,_tag_dyneither(_tmp4B,sizeof(void*),0U));});});
goto _LL30;}_LL30:;}
# 201
({struct Cyc_Absyn_Vardecl**_tmp40E=({struct Cyc_Absyn_Vardecl**_tmp4D=_cycalloc(sizeof(*_tmp4D));*_tmp4D=_tmpE5;_tmp4D;});struct Cyc_Absyn_Exp*_tmp40D=access_exp;struct Cyc_List_List**_tmp40C=& res.patvars;Cyc_Tcpat_set_vd(_tmp40E,_tmp40D,_tmp40C,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}case 2U: _LL5: _tmpE7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_tmpE6=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_LL6: {
# 204
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp410=p->loc;struct Cyc_Tcenv_Tenv*_tmp40F=te;Cyc_Tcenv_add_type_vars(_tmp410,_tmp40F,({struct Cyc_Absyn_Tvar*_tmp59[1U];_tmp59[0]=_tmpE7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp411=({struct _tuple1*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=0,_tmp4F->f2=0;_tmp4F;});res.tvars_and_bounds_opt=_tmp411;});
({struct Cyc_List_List*_tmp414=({
struct Cyc_List_List*_tmp413=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp413,({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({struct _tuple16*_tmp412=({struct _tuple16*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->f1=_tmpE7,_tmp50->f2=1;_tmp50;});_tmp51->hd=_tmp412;}),_tmp51->tl=0;_tmp51;}));});
# 207
(*res.tvars_and_bounds_opt).f1=_tmp414;});
# 209
({unsigned int _tmp417=p->loc;struct Cyc_Tcenv_Tenv*_tmp416=te2;struct Cyc_List_List*_tmp415=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp417,_tmp416,_tmp415,& Cyc_Tcutil_tmk,1,0,_tmpE6->type);});
# 212
if(topt != 0)t=*topt;else{
({void*_tmp419=({struct Cyc_List_List*_tmp418=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp418,topt);});t=_tmp419;});}
{void*_tmp52=Cyc_Tcutil_compress(t);void*_tmp53=_tmp52;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53)->tag == 8U){_LL36: _LL37:
# 216
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp54=0U;({unsigned int _tmp41B=p->loc;struct _dyneither_ptr _tmp41A=({const char*_tmp55="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp55,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp41B,_tmp41A,_tag_dyneither(_tmp54,sizeof(void*),0U));});});
goto _LL35;}else{_LL38: _LL39:
# 220
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp56=0U;({unsigned int _tmp41D=p->loc;struct _dyneither_ptr _tmp41C=({const char*_tmp57="pattern would point to an abstract member";_tag_dyneither(_tmp57,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp41D,_tmp41C,_tag_dyneither(_tmp56,sizeof(void*),0U));});});
goto _LL35;}_LL35:;}
# 224
({struct Cyc_Absyn_Vardecl**_tmp420=({struct Cyc_Absyn_Vardecl**_tmp58=_cycalloc(sizeof(*_tmp58));*_tmp58=_tmpE6;_tmp58;});struct Cyc_Absyn_Exp*_tmp41F=access_exp;struct Cyc_List_List**_tmp41E=& res.patvars;Cyc_Tcpat_set_vd(_tmp420,_tmp41F,_tmp41E,_tmpE6->type);});
goto _LL0;}case 3U: _LL7: _tmpE9=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_tmpE8=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_LL8:
# 228
({struct Cyc_Tcpat_TcPatResult _tmp421=Cyc_Tcpat_tcPatRec(te,_tmpE8,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp421;});
t=(void*)_check_null(_tmpE8->topt);
if(!allow_ref_pat  || rgn_pat == 0){
({void*_tmp5A=0U;({unsigned int _tmp423=p->loc;struct _dyneither_ptr _tmp422=({const char*_tmp5B="* pattern would point into an unknown/unallowed region";_tag_dyneither(_tmp5B,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp423,_tmp422,_tag_dyneither(_tmp5A,sizeof(void*),0U));});});
goto _LL0;}else{
# 235
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({void*_tmp5C=0U;({unsigned int _tmp425=p->loc;struct _dyneither_ptr _tmp424=({const char*_tmp5D="* pattern cannot take the address of an alias-free path";_tag_dyneither(_tmp5D,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp425,_tmp424,_tag_dyneither(_tmp5C,sizeof(void*),0U));});});}{
# 238
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=5U,(_tmp5F->f1).elt_typ=t,({struct Cyc_Absyn_Tqual _tmp428=Cyc_Absyn_empty_tqual(0U);(_tmp5F->f1).elt_tq=_tmp428;}),
((_tmp5F->f1).ptr_atts).rgn=rgn_pat,((_tmp5F->f1).ptr_atts).nullable=Cyc_Absyn_false_conref,({
union Cyc_Absyn_Constraint*_tmp427=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp5F->f1).ptr_atts).bounds=_tmp427;}),({union Cyc_Absyn_Constraint*_tmp426=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp5F->f1).ptr_atts).zero_term=_tmp426;}),((_tmp5F->f1).ptr_atts).ptrloc=0;_tmp5F;});
# 243
if((unsigned int)access_exp){
({struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_address_exp(access_exp,0U);new_access_exp=_tmp429;});
new_access_exp->topt=t2;}
# 247
({struct Cyc_Absyn_Vardecl**_tmp42C=({struct Cyc_Absyn_Vardecl**_tmp5E=_cycalloc(sizeof(*_tmp5E));*_tmp5E=_tmpE9;_tmp5E;});struct Cyc_Absyn_Exp*_tmp42B=new_access_exp;struct Cyc_List_List**_tmp42A=& res.patvars;Cyc_Tcpat_set_vd(_tmp42C,_tmp42B,_tmp42A,t2);});
goto _LL0;};case 4U: _LL9: _tmpEB=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_tmpEA=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_LLA:
# 253
({struct Cyc_Absyn_Vardecl**_tmp42F=({struct Cyc_Absyn_Vardecl**_tmp60=_cycalloc(sizeof(*_tmp60));*_tmp60=_tmpEA;_tmp60;});struct Cyc_Absyn_Exp*_tmp42E=access_exp;struct Cyc_List_List**_tmp42D=& res.patvars;Cyc_Tcpat_set_vd(_tmp42F,_tmp42E,_tmp42D,_tmpEA->type);});
# 257
({unsigned int _tmp431=p->loc;struct Cyc_Tcenv_Tenv*_tmp430=te;Cyc_Tcenv_add_type_vars(_tmp431,_tmp430,({struct Cyc_Absyn_Tvar*_tmp61[1U];_tmp61[0]=_tmpEB;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp432=({struct _tuple1*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=0,_tmp62->f2=0;_tmp62;});res.tvars_and_bounds_opt=_tmp432;});
({struct Cyc_List_List*_tmp434=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));
({struct _tuple16*_tmp433=({struct _tuple16*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->f1=_tmpEB,_tmp63->f2=0;_tmp63;});_tmp64->hd=_tmp433;}),_tmp64->tl=(*res.tvars_and_bounds_opt).f1;_tmp64;});
# 260
(*res.tvars_and_bounds_opt).f1=_tmp434;});
# 262
t=Cyc_Absyn_uint_typ;
goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3F)->f1){case Cyc_Absyn_Unsigned: _LLB: _LLC:
# 265
({void*_tmp435=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);t=_tmp435;});goto _LL0;case Cyc_Absyn_None: _LLD: _LLE:
 goto _LL10;default: _LLF: _LL10:
({void*_tmp436=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);t=_tmp436;});goto _LL0;}case 11U: _LL11: _LL12:
({void*_tmp437=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);t=_tmp437;});goto _LL0;case 12U: _LL13: _tmpEC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_LL14:
({void*_tmp439=({void**_tmp438=topt;Cyc_Tcpat_num_type(_tmp438,Cyc_Absyn_float_typ(_tmpEC));});t=_tmp439;});goto _LL0;case 13U: _LL15: _tmpED=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_LL16:
# 271
({void*_tmp43B=({void**_tmp43A=topt;Cyc_Tcpat_num_type(_tmp43A,(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=13U,_tmp65->f1=_tmpED->name,_tmp65->f2=_tmpED;_tmp65;}));});t=_tmp43B;});
goto _LL0;case 14U: _LL17: _tmpEE=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_LL18:
({void*_tmp43C=Cyc_Tcpat_num_type(topt,_tmpEE);t=_tmp43C;});goto _LL0;case 9U: _LL19: _LL1A:
# 275
 if(topt != 0){
void*_tmp66=Cyc_Tcutil_compress(*topt);void*_tmp67=_tmp66;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67)->tag == 5U){_LL3B: _LL3C:
 t=*topt;goto tcpat_end;}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}{
# 280
struct Cyc_Core_Opt*_tmp68=Cyc_Tcenv_lookup_opt_type_vars(te);
({void*_tmp442=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=5U,({void*_tmp441=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp68);(_tmp69->f1).elt_typ=_tmp441;}),({
struct Cyc_Absyn_Tqual _tmp440=Cyc_Absyn_empty_tqual(0U);(_tmp69->f1).elt_tq=_tmp440;}),
({void*_tmp43F=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp68);((_tmp69->f1).ptr_atts).rgn=_tmp43F;}),((_tmp69->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 285
union Cyc_Absyn_Constraint*_tmp43E=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp69->f1).ptr_atts).bounds=_tmp43E;}),({union Cyc_Absyn_Constraint*_tmp43D=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp69->f1).ptr_atts).zero_term=_tmp43D;}),((_tmp69->f1).ptr_atts).ptrloc=0;_tmp69;});
# 281
t=_tmp442;});
# 286
goto _LL0;};case 6U: _LL1B: _tmpEF=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_LL1C: {
# 291
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmp6A=0;
int elt_const=0;
if(topt != 0){
void*_tmp6B=Cyc_Tcutil_compress(*topt);void*_tmp6C=_tmp6B;void*_tmp6E;struct Cyc_Absyn_Tqual _tmp6D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C)->tag == 5U){_LL40: _tmp6E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C)->f1).elt_typ;_tmp6D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C)->f1).elt_tq;_LL41:
# 297
 inner_typ=_tmp6E;
_tmp6A=& inner_typ;
elt_const=_tmp6D.real_const;
goto _LL3F;}else{_LL42: _LL43:
 goto _LL3F;}_LL3F:;}{
# 306
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)({struct Cyc_Absyn_Exp*_tmp443=Cyc_Absyn_deref_exp(access_exp,0U);new_access_exp=_tmp443;});
({struct Cyc_Tcpat_TcPatResult _tmp445=({struct Cyc_Tcpat_TcPatResult _tmp444=res;Cyc_Tcpat_combine_results(_tmp444,Cyc_Tcpat_tcPatRec(te,_tmpEF,_tmp6A,ptr_rgn,1,new_access_exp));});res=_tmp445;});
# 315
{void*_tmp6F=Cyc_Tcutil_compress((void*)_check_null(_tmpEF->topt));void*_tmp70=_tmp6F;struct Cyc_Absyn_Datatypedecl*_tmp79;struct Cyc_Absyn_Datatypefield*_tmp78;struct Cyc_List_List*_tmp77;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70)->tag == 4U){if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70)->f1).field_info).KnownDatatypefield).tag == 2){_LL45: _tmp79=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70)->f1).field_info).KnownDatatypefield).val).f1;_tmp78=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70)->f1).field_info).KnownDatatypefield).val).f2;_tmp77=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70)->f1).targs;_LL46:
# 319
{void*_tmp71=Cyc_Tcutil_compress(inner_typ);void*_tmp72=_tmp71;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp72)->tag == 4U){_LL4A: _LL4B:
# 321
 goto DONT_PROMOTE;}else{_LL4C: _LL4D:
 goto _LL49;}_LL49:;}{
# 325
void*new_type=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=3U,({union Cyc_Absyn_DatatypeInfoU _tmp446=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp74=_cycalloc(sizeof(*_tmp74));*_tmp74=_tmp79;_tmp74;}));(_tmp75->f1).datatype_info=_tmp446;}),(_tmp75->f1).targs=_tmp77;_tmp75;});
# 328
_tmpEF->topt=new_type;
({void*_tmp44A=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=5U,(_tmp73->f1).elt_typ=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp449=Cyc_Absyn_const_tqual(0U);(_tmp73->f1).elt_tq=_tmp449;}):({
struct Cyc_Absyn_Tqual _tmp448=Cyc_Absyn_empty_tqual(0U);(_tmp73->f1).elt_tq=_tmp448;}),
((_tmp73->f1).ptr_atts).rgn=ptr_rgn,({union Cyc_Absyn_Constraint*_tmp447=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp73->f1).ptr_atts).nullable=_tmp447;}),((_tmp73->f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,((_tmp73->f1).ptr_atts).zero_term=Cyc_Absyn_false_conref,((_tmp73->f1).ptr_atts).ptrloc=0;_tmp73;});
# 329
t=_tmp44A;});
# 335
goto _LL44;};}else{goto _LL47;}}else{_LL47: _LL48:
# 337
 DONT_PROMOTE:
({void*_tmp450=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=5U,(_tmp76->f1).elt_typ=(void*)_check_null(_tmpEF->topt),
elt_const?({struct Cyc_Absyn_Tqual _tmp44F=Cyc_Absyn_const_tqual(0U);(_tmp76->f1).elt_tq=_tmp44F;}):({
struct Cyc_Absyn_Tqual _tmp44E=Cyc_Absyn_empty_tqual(0U);(_tmp76->f1).elt_tq=_tmp44E;}),
((_tmp76->f1).ptr_atts).rgn=ptr_rgn,({union Cyc_Absyn_Constraint*_tmp44D=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp76->f1).ptr_atts).nullable=_tmp44D;}),({
union Cyc_Absyn_Constraint*_tmp44C=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp76->f1).ptr_atts).bounds=_tmp44C;}),({union Cyc_Absyn_Constraint*_tmp44B=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp76->f1).ptr_atts).zero_term=_tmp44B;}),((_tmp76->f1).ptr_atts).ptrloc=0;_tmp76;});
# 338
t=_tmp450;});}_LL44:;}
# 345
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpEF->topt;
goto _LL0;};}case 5U: _LL1D: _tmpF1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_tmpF0=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_LL1E: {
# 349
struct Cyc_List_List*_tmp7A=*_tmpF1;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp7B=Cyc_Tcutil_compress(*topt);void*_tmp7C=_tmp7B;struct Cyc_List_List*_tmp82;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7C)->tag == 10U){_LL4F: _tmp82=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp7C)->f1;_LL50:
# 355
 topt_ts=_tmp82;
if(_tmpF0){
# 358
int _tmp7D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A);
int _tmp7E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp82);
if(_tmp7D < _tmp7E){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp7E - _tmp7D;++ i){
({struct Cyc_List_List*_tmp452=({struct Cyc_List_List*_tmp7F=_cycalloc(sizeof(*_tmp7F));({struct Cyc_Absyn_Pat*_tmp451=Cyc_Tcpat_wild_pat(p->loc);_tmp7F->hd=_tmp451;}),_tmp7F->tl=wild_ps;_tmp7F;});wild_ps=_tmp452;});}}
({struct Cyc_List_List*_tmp453=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7A,wild_ps);*_tmpF1=_tmp453;});
_tmp7A=*_tmpF1;}else{
if(({int _tmp454=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A);_tmp454 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp82);}))
({void*_tmp80=0U;({unsigned int _tmp456=p->loc;struct _dyneither_ptr _tmp455=({const char*_tmp81="unnecessary ... in tuple pattern";_tag_dyneither(_tmp81,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp456,_tmp455,_tag_dyneither(_tmp80,sizeof(void*),0U));});});}}
# 370
goto _LL4E;}else{_LL51: _LL52:
# 372
 goto _LL4E;}_LL4E:;}else{
# 374
if(_tmpF0)
({void*_tmp83=0U;({unsigned int _tmp458=p->loc;struct _dyneither_ptr _tmp457=({const char*_tmp84="cannot determine missing fields for ... in tuple pattern";_tag_dyneither(_tmp84,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp458,_tmp457,_tag_dyneither(_tmp83,sizeof(void*),0U));});});}
{int i=0;for(0;_tmp7A != 0;(_tmp7A=_tmp7A->tl,i ++)){
void**_tmp85=0;
if(topt_ts != 0){
_tmp85=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 382
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp45A=({struct Cyc_Absyn_Exp*_tmp459=access_exp;Cyc_Absyn_subscript_exp(_tmp459,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp393;(_tmp393.Int_c).tag=5U,((_tmp393.Int_c).val).f1=Cyc_Absyn_Unsigned,((_tmp393.Int_c).val).f2=i;_tmp393;}),0U),0U);});
# 384
new_access_exp=_tmp45A;});
# 387
({struct Cyc_Tcpat_TcPatResult _tmp45C=({struct Cyc_Tcpat_TcPatResult _tmp45B=res;Cyc_Tcpat_combine_results(_tmp45B,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp7A->hd,_tmp85,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp45C;});
# 390
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp7A->hd)->topt;
({struct Cyc_List_List*_tmp45F=({struct Cyc_List_List*_tmp87=_cycalloc(sizeof(*_tmp87));({struct _tuple17*_tmp45E=({struct _tuple17*_tmp86=_cycalloc(sizeof(*_tmp86));({struct Cyc_Absyn_Tqual _tmp45D=Cyc_Absyn_empty_tqual(0U);_tmp86->f1=_tmp45D;}),_tmp86->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp7A->hd)->topt);_tmp86;});_tmp87->hd=_tmp45E;}),_tmp87->tl=pat_ts;_tmp87;});pat_ts=_tmp45F;});};}}
# 393
({void*_tmp461=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=10U,({struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp88->f1=_tmp460;});_tmp88;});t=_tmp461;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3F)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3F)->f1)->aggr_info).KnownAggr).tag == 2){_LL1F: _tmpF6=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3F)->f1)->aggr_info).KnownAggr).val;_tmpF5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3F)->f1)->targs;_tmpF4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_tmpF3=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3F)->f3;_tmpF2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3F)->f4;_LL20: {
# 397
struct Cyc_List_List*_tmp89=*_tmpF3;
struct _dyneither_ptr aggr_str=_tmpF6->kind == Cyc_Absyn_StructA?({const char*_tmpC9="struct";_tag_dyneither(_tmpC9,sizeof(char),7U);}):({const char*_tmpCA="union";_tag_dyneither(_tmpCA,sizeof(char),6U);});
if(_tmpF6->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8C=({struct Cyc_String_pa_PrintArg_struct _tmp394;_tmp394.tag=0U,_tmp394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp394;});void*_tmp8A[1U];_tmp8A[0]=& _tmp8C;({unsigned int _tmp463=p->loc;struct _dyneither_ptr _tmp462=({const char*_tmp8B="can't destructure an abstract %s";_tag_dyneither(_tmp8B,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp463,_tmp462,_tag_dyneither(_tmp8A,sizeof(void*),1U));});});
({void*_tmp464=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp464;});
goto _LL0;}
# 406
if(_tmpF6->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->tagged)
allow_ref_pat=0;
if(_tmpF4 != 0){
# 412
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}
# 415
{struct _RegionHandle _tmp8D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8D;_push_region(rgn);
# 417
{struct Cyc_List_List*_tmp8E=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp8F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpF4;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp8F))->hd;
_tmp8F=_tmp8F->tl;{
void*_tmp90=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp91=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp92=_tmp91;struct Cyc_Absyn_Kind*_tmp96;struct Cyc_Absyn_Kind*_tmp95;switch(*((int*)_tmp92)){case 2U: _LL54: _tmp95=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92)->f2;_LL55:
 _tmp96=_tmp95;goto _LL57;case 0U: _LL56: _tmp96=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp92)->f1;_LL57:
 k2=_tmp96;goto _LL53;default: _LL58: _LL59:
({void*_tmp93=0U;({struct _dyneither_ptr _tmp465=({const char*_tmp94="unconstrained existential type variable in aggregate";_tag_dyneither(_tmp94,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp465,_tag_dyneither(_tmp93,sizeof(void*),0U));});});}_LL53:;}
# 433
{void*_tmp97=_tmp90;struct Cyc_Core_Opt**_tmp9C;struct Cyc_Core_Opt**_tmp9B;struct Cyc_Absyn_Kind*_tmp9A;struct Cyc_Absyn_Kind*_tmp99;switch(*((int*)_tmp97)){case 0U: _LL5B: _tmp99=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97)->f1;_LL5C:
# 436
 if(!Cyc_Tcutil_kind_leq(k2,_tmp99))
error=1;
goto _LL5A;case 2U: _LL5D: _tmp9B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp97)->f1;_tmp9A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp97)->f2;_LL5E:
 _tmp9C=_tmp9B;goto _LL60;default: _LL5F: _tmp9C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp97)->f1;_LL60:
({struct Cyc_Core_Opt*_tmp466=({struct Cyc_Core_Opt*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->v=_tmp91;_tmp98;});*_tmp9C=_tmp466;});goto _LL5A;}_LL5A:;}
# 442
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmp9F=({struct Cyc_String_pa_PrintArg_struct _tmp397;_tmp397.tag=0U,_tmp397.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmp397;});struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp396;_tmp396.tag=0U,({
# 446
struct _dyneither_ptr _tmp467=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp90));_tmp396.f1=_tmp467;});_tmp396;});struct Cyc_String_pa_PrintArg_struct _tmpA1=({struct Cyc_String_pa_PrintArg_struct _tmp395;_tmp395.tag=0U,({struct _dyneither_ptr _tmp468=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(k2));_tmp395.f1=_tmp468;});_tmp395;});void*_tmp9D[3U];_tmp9D[0]=& _tmp9F,_tmp9D[1]=& _tmpA0,_tmp9D[2]=& _tmpA1;({unsigned int _tmp46A=p->loc;struct _dyneither_ptr _tmp469=({const char*_tmp9E="type variable %s has kind %s but must have at least kind %s";_tag_dyneither(_tmp9E,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp46A,_tmp469,_tag_dyneither(_tmp9D,sizeof(void*),3U));});});{
# 448
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=2U,_tmpA7->f1=tv;_tmpA7;});
({struct Cyc_List_List*_tmp46B=({struct Cyc_List_List*_tmpA2=_region_malloc(rgn,sizeof(*_tmpA2));_tmpA2->hd=vartype,_tmpA2->tl=_tmp8E;_tmpA2;});_tmp8E=_tmp46B;});
# 452
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable)
({struct Cyc_List_List*_tmp46D=({struct Cyc_List_List*_tmpA4=_cycalloc(sizeof(*_tmpA4));
({struct _tuple0*_tmp46C=({struct _tuple0*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->f1=Cyc_Absyn_empty_effect,_tmpA3->f2=vartype;_tmpA3;});_tmpA4->hd=_tmp46C;}),_tmpA4->tl=outlives_constraints;_tmpA4;});
# 454
outlives_constraints=_tmp46D;});else{
# 457
({void*_tmpA5=0U;({struct _dyneither_ptr _tmp46E=({const char*_tmpA6="opened existential had unique or top region kind";_tag_dyneither(_tmpA6,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp46E,_tag_dyneither(_tmpA5,sizeof(void*),0U));});});}}};};}}
# 461
({struct Cyc_List_List*_tmp46F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8E);_tmp8E=_tmp46F;});{
# 463
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmpF4);
# 465
struct Cyc_List_List*_tmpA8=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmpA9=({struct _tuple12 _tmp39C;_tmp39C.f1=_tmpA8,_tmp39C.f2=rgn;_tmp39C;});
struct Cyc_List_List*_tmpAA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpA9,_tmpF6->tvs);
struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->exist_vars,_tmp8E);
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpAA);
struct Cyc_List_List*_tmpAD=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpAB);
struct Cyc_List_List*_tmpAE=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpAA,_tmpAB);
# 473
if(_tmpF4 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp470=({struct _tuple1*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->f1=0,_tmpAF->f2=0;_tmpAF;});res.tvars_and_bounds_opt=_tmp470;});
({struct Cyc_List_List*_tmp472=({
struct Cyc_List_List*_tmp471=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp471,((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmpF4));});
# 476
(*res.tvars_and_bounds_opt).f1=_tmp472;});
# 478
({struct Cyc_List_List*_tmp473=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 478
(*res.tvars_and_bounds_opt).f2=_tmp473;});{
# 480
struct Cyc_List_List*_tmpB0=0;
{struct Cyc_List_List*_tmpB1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->rgn_po;for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){
({struct Cyc_List_List*_tmp477=({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct _tuple0*_tmp476=({struct _tuple0*_tmpB2=_cycalloc(sizeof(*_tmpB2));({void*_tmp475=Cyc_Tcutil_rsubstitute(rgn,_tmpAE,(*((struct _tuple0*)_tmpB1->hd)).f1);_tmpB2->f1=_tmp475;}),({
void*_tmp474=Cyc_Tcutil_rsubstitute(rgn,_tmpAE,(*((struct _tuple0*)_tmpB1->hd)).f2);_tmpB2->f2=_tmp474;});_tmpB2;});
# 482
_tmpB3->hd=_tmp476;}),_tmpB3->tl=_tmpB0;_tmpB3;});_tmpB0=_tmp477;});}}
# 485
({struct Cyc_List_List*_tmp478=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB0);_tmpB0=_tmp478;});
({struct Cyc_List_List*_tmp479=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpB0);
# 486
(*res.tvars_and_bounds_opt).f2=_tmp479;});};}
# 490
*_tmpF5=_tmpAC;
({void*_tmp47B=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp47A=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpB4=_cycalloc(sizeof(*_tmpB4));*_tmpB4=_tmpF6;_tmpB4;}));(_tmpB5->f1).aggr_info=_tmp47A;}),(_tmpB5->f1).targs=*_tmpF5;_tmpB5;});t=_tmp47B;});
if(_tmpF2  && _tmpF6->kind == Cyc_Absyn_UnionA)
({void*_tmpB6=0U;({unsigned int _tmp47D=p->loc;struct _dyneither_ptr _tmp47C=({const char*_tmpB7="`...' pattern not allowed in union pattern";_tag_dyneither(_tmpB7,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp47D,_tmp47C,_tag_dyneither(_tmpB6,sizeof(void*),0U));});});else{
if(_tmpF2){
# 496
int _tmpB8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp89);
int _tmpB9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->fields);
if(_tmpB8 < _tmpB9){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmpB9 - _tmpB8;++ i){
({struct Cyc_List_List*_tmp480=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _tuple18*_tmp47F=({struct _tuple18*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=0,({struct Cyc_Absyn_Pat*_tmp47E=Cyc_Tcpat_wild_pat(p->loc);_tmpBA->f2=_tmp47E;});_tmpBA;});_tmpBB->hd=_tmp47F;}),_tmpBB->tl=wild_dps;_tmpBB;});wild_dps=_tmp480;});}}
({struct Cyc_List_List*_tmp481=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp89,wild_dps);*_tmpF3=_tmp481;});
_tmp89=*_tmpF3;}else{
if(_tmpB8 == _tmpB9)
({void*_tmpBC=0U;({unsigned int _tmp483=p->loc;struct _dyneither_ptr _tmp482=({const char*_tmpBD="unnecessary ... in struct pattern";_tag_dyneither(_tmpBD,sizeof(char),34U);});Cyc_Tcutil_warn(_tmp483,_tmp482,_tag_dyneither(_tmpBC,sizeof(void*),0U));});});}}}{
# 507
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp89,_tmpF6->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmpBE=(struct _tuple19*)fields->hd;struct _tuple19*_tmpBF=_tmpBE;struct Cyc_Absyn_Aggrfield*_tmpC8;struct Cyc_Absyn_Pat*_tmpC7;_LL62: _tmpC8=_tmpBF->f1;_tmpC7=_tmpBF->f2;_LL63:;{
void*_tmpC0=Cyc_Tcutil_rsubstitute(rgn,_tmpAE,_tmpC8->type);
# 513
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp484=Cyc_Absyn_aggrmember_exp(access_exp,_tmpC8->name,0U);new_access_exp=_tmp484;});
({struct Cyc_Tcpat_TcPatResult _tmp486=({struct Cyc_Tcpat_TcPatResult _tmp485=res;Cyc_Tcpat_combine_results(_tmp485,Cyc_Tcpat_tcPatRec(te2,_tmpC7,& _tmpC0,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp486;});
# 521
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpC7->topt),_tmpC0))
({struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp39B;_tmp39B.tag=0U,_tmp39B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC8->name);_tmp39B;});struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0U,_tmp39A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp39A;});struct Cyc_String_pa_PrintArg_struct _tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0U,({
struct _dyneither_ptr _tmp487=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpC0));_tmp399.f1=_tmp487;});_tmp399;});struct Cyc_String_pa_PrintArg_struct _tmpC6=({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0U,({
struct _dyneither_ptr _tmp488=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpC7->topt)));_tmp398.f1=_tmp488;});_tmp398;});void*_tmpC1[4U];_tmpC1[0]=& _tmpC3,_tmpC1[1]=& _tmpC4,_tmpC1[2]=& _tmpC5,_tmpC1[3]=& _tmpC6;({unsigned int _tmp48A=p->loc;struct _dyneither_ptr _tmp489=({const char*_tmpC2="field %s of %s pattern expects type %s != %s";_tag_dyneither(_tmpC2,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp48A,_tmp489,_tag_dyneither(_tmpC1,sizeof(void*),4U));});});
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpC7->topt;};}};};}
# 417
;_pop_region(rgn);}
# 528
goto _LL0;}}else{_LL25: _LL26:
# 582
 goto _LL28;}}else{_LL23: _LL24:
# 581
 goto _LL26;}case 8U: _LL21: _tmpFA=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3F)->f1;_tmpF9=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3F)->f2;_tmpF8=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3F)->f3;_tmpF7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3F)->f4;_LL22: {
# 531
struct Cyc_List_List*_tmpCB=*_tmpF8;
struct Cyc_List_List*tqts=_tmpF9->typs;
# 534
struct Cyc_List_List*_tmpCC=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmpCD=({struct _tuple12 _tmp3A3;_tmp3A3.f1=_tmpCC,_tmp3A3.f2=Cyc_Core_heap_region;_tmp3A3;});
struct Cyc_List_List*_tmpCE=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmpCD,_tmpFA->tvs);
struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpCE);
({void*_tmp48C=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->tag=4U,({union Cyc_Absyn_DatatypeFieldInfoU _tmp48B=Cyc_Absyn_KnownDatatypefield(_tmpFA,_tmpF9);(_tmpD0->f1).field_info=_tmp48B;}),(_tmpD0->f1).targs=_tmpCF;_tmpD0;});t=_tmp48C;});
# 540
if(_tmpF7){
# 542
int _tmpD1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCB);
int _tmpD2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmpD1 < _tmpD2){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpD2 - _tmpD1;++ i){
({struct Cyc_List_List*_tmp48E=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));({struct Cyc_Absyn_Pat*_tmp48D=Cyc_Tcpat_wild_pat(p->loc);_tmpD3->hd=_tmp48D;}),_tmpD3->tl=wild_ps;_tmpD3;});wild_ps=_tmp48E;});}}
({struct Cyc_List_List*_tmp48F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCB,wild_ps);*_tmpF8=_tmp48F;});
_tmpCB=*_tmpF8;}else{
if(_tmpD1 == _tmpD2)
({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0U,({
struct _dyneither_ptr _tmp490=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpFA->name));_tmp39D.f1=_tmp490;});_tmp39D;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({unsigned int _tmp492=p->loc;struct _dyneither_ptr _tmp491=({const char*_tmpD5="unnecessary ... in datatype field %s";_tag_dyneither(_tmpD5,sizeof(char),37U);});Cyc_Tcutil_warn(_tmp492,_tmp491,_tag_dyneither(_tmpD4,sizeof(void*),1U));});});}}
# 554
for(0;_tmpCB != 0  && tqts != 0;(_tmpCB=_tmpCB->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpD7=(struct Cyc_Absyn_Pat*)_tmpCB->hd;
# 558
void*_tmpD8=Cyc_Tcutil_substitute(_tmpCE,(*((struct _tuple17*)tqts->hd)).f2);
# 561
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
({struct Cyc_Tcpat_TcPatResult _tmp494=({struct Cyc_Tcpat_TcPatResult _tmp493=res;Cyc_Tcpat_combine_results(_tmp493,Cyc_Tcpat_tcPatRec(te,_tmpD7,& _tmpD8,rgn_pat,allow_ref_pat,0));});res=_tmp494;});
# 568
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpD7->topt),_tmpD8))
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp3A0;_tmp3A0.tag=0U,({
struct _dyneither_ptr _tmp495=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF9->name));_tmp3A0.f1=_tmp495;});_tmp3A0;});struct Cyc_String_pa_PrintArg_struct _tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmp39F;_tmp39F.tag=0U,({struct _dyneither_ptr _tmp496=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD8));_tmp39F.f1=_tmp496;});_tmp39F;});struct Cyc_String_pa_PrintArg_struct _tmpDD=({struct Cyc_String_pa_PrintArg_struct _tmp39E;_tmp39E.tag=0U,({
struct _dyneither_ptr _tmp497=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpD7->topt)));_tmp39E.f1=_tmp497;});_tmp39E;});void*_tmpD9[3U];_tmpD9[0]=& _tmpDB,_tmpD9[1]=& _tmpDC,_tmpD9[2]=& _tmpDD;({unsigned int _tmp499=_tmpD7->loc;struct _dyneither_ptr _tmp498=({const char*_tmpDA="%s expects argument type %s, not %s";_tag_dyneither(_tmpDA,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp499,_tmp498,_tag_dyneither(_tmpD9,sizeof(void*),3U));});});}
# 573
if(_tmpCB != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpE0=({struct Cyc_String_pa_PrintArg_struct _tmp3A1;_tmp3A1.tag=0U,({
struct _dyneither_ptr _tmp49A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF9->name));_tmp3A1.f1=_tmp49A;});_tmp3A1;});void*_tmpDE[1U];_tmpDE[0]=& _tmpE0;({unsigned int _tmp49C=p->loc;struct _dyneither_ptr _tmp49B=({const char*_tmpDF="too many arguments for datatype constructor %s";_tag_dyneither(_tmpDF,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp49C,_tmp49B,_tag_dyneither(_tmpDE,sizeof(void*),1U));});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpE3=({struct Cyc_String_pa_PrintArg_struct _tmp3A2;_tmp3A2.tag=0U,({
struct _dyneither_ptr _tmp49D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF9->name));_tmp3A2.f1=_tmp49D;});_tmp3A2;});void*_tmpE1[1U];_tmpE1[0]=& _tmpE3;({unsigned int _tmp49F=p->loc;struct _dyneither_ptr _tmp49E=({const char*_tmpE2="too few arguments for datatype constructor %s";_tag_dyneither(_tmpE2,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp49F,_tmp49E,_tag_dyneither(_tmpE1,sizeof(void*),1U));});});
goto _LL0;}case 15U: _LL27: _LL28:
# 583
 goto _LL2A;case 17U: _LL29: _LL2A:
 goto _LL2C;default: _LL2B: _LL2C:
# 586
({void*_tmp4A0=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp4A0;});goto _LL0;}_LL0:;}
# 588
tcpat_end:
 p->topt=t;
return res;};}
# 593
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 595
struct Cyc_Tcpat_TcPatResult _tmpFB=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 597
struct _tuple1 _tmpFC=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpFB.patvars);struct _tuple1 _tmpFD=_tmpFC;struct Cyc_List_List*_tmp105;_LL1: _tmp105=_tmpFD.f1;_LL2:;{
struct Cyc_List_List*_tmpFE=0;
{struct Cyc_List_List*x=_tmp105;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)({struct Cyc_List_List*_tmp4A1=({struct Cyc_List_List*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmpFF->tl=_tmpFE;_tmpFF;});_tmpFE=_tmp4A1;});}}
({struct Cyc_List_List*_tmp4A3=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_name,_tmpFE);unsigned int _tmp4A2=p->loc;Cyc_Tcutil_check_unique_vars(_tmp4A3,_tmp4A2,({const char*_tmp100="pattern contains a repeated variable";_tag_dyneither(_tmp100,sizeof(char),37U);}));});
# 606
{struct Cyc_List_List*x=_tmpFB.patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp101=(struct _tuple15*)x->hd;struct _tuple15*_tmp102=_tmp101;struct Cyc_Absyn_Vardecl**_tmp104;struct Cyc_Absyn_Exp**_tmp103;_LL4: _tmp104=_tmp102->f1;_tmp103=(struct Cyc_Absyn_Exp**)& _tmp102->f2;_LL5:;
if(*_tmp103 != 0  && *_tmp103 != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp103));*_tmp103=_tmp4A4;});}}
# 611
return _tmpFB;};}
# 617
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 620
struct _tuple0 _tmp106=({struct _tuple0 _tmp3A5;({void*_tmp4A6=Cyc_Tcutil_compress(old_type);_tmp3A5.f1=_tmp4A6;}),({void*_tmp4A5=Cyc_Tcutil_compress(new_type);_tmp3A5.f2=_tmp4A5;});_tmp3A5;});struct _tuple0 _tmp107=_tmp106;struct Cyc_Absyn_PtrInfo _tmp10B;struct Cyc_Absyn_PtrInfo _tmp10A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f1)->tag == 5U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f2)->tag == 5U){_LL1: _tmp10B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f1)->f1;_tmp10A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f2)->f1;_LL2: {
# 622
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp108=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->tag=5U,(_tmp109->f1).elt_typ=_tmp10B.elt_typ,(_tmp109->f1).elt_tq=_tmp10A.elt_tq,
((_tmp109->f1).ptr_atts).rgn=(_tmp10B.ptr_atts).rgn,((_tmp109->f1).ptr_atts).nullable=(_tmp10A.ptr_atts).nullable,((_tmp109->f1).ptr_atts).bounds=(_tmp10A.ptr_atts).bounds,((_tmp109->f1).ptr_atts).zero_term=(_tmp10A.ptr_atts).zero_term,((_tmp109->f1).ptr_atts).ptrloc=(_tmp10B.ptr_atts).ptrloc;_tmp109;});
# 628
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp108,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp108);}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 637
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 640
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _tuple0*_tmp4A8=({struct _tuple0*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->f1=(void*)& Cyc_Absyn_UniqueRgn_val,({void*_tmp4A7=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=2U,_tmp116->f1=tv;_tmp116;});_tmp117->f2=_tmp4A7;});_tmp117;});_tmp118->hd=_tmp4A8;}),_tmp118->tl=0;_tmp118;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 658 "tcpat.cyc"
struct _tuple0 _tmp10C=({struct _tuple0 _tmp3A6;({void*_tmp4AA=Cyc_Tcutil_compress(old_type);_tmp3A6.f1=_tmp4AA;}),({void*_tmp4A9=Cyc_Tcutil_compress(new_type);_tmp3A6.f2=_tmp4A9;});_tmp3A6;});struct _tuple0 _tmp10D=_tmp10C;struct Cyc_Absyn_PtrInfo _tmp111;struct Cyc_Absyn_PtrInfo _tmp110;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10D.f1)->tag == 5U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10D.f2)->tag == 5U){_LL1: _tmp111=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10D.f1)->f1;_tmp110=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10D.f2)->f1;_LL2:
 goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp10E=0U;({unsigned int _tmp4AC=loc;struct _dyneither_ptr _tmp4AB=({const char*_tmp10F="alias requires pointer type";_tag_dyneither(_tmp10F,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp4AC,_tmp4AB,_tag_dyneither(_tmp10E,sizeof(void*),0U));});});goto _LL0;}_LL0:;}else{
# 663
({struct Cyc_String_pa_PrintArg_struct _tmp114=({struct Cyc_String_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=0U,({
struct _dyneither_ptr _tmp4AD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type));_tmp3A8.f1=_tmp4AD;});_tmp3A8;});struct Cyc_String_pa_PrintArg_struct _tmp115=({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0U,({struct _dyneither_ptr _tmp4AE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(new_type));_tmp3A7.f1=_tmp4AE;});_tmp3A7;});void*_tmp112[2U];_tmp112[0]=& _tmp114,_tmp112[1]=& _tmp115;({unsigned int _tmp4B0=loc;struct _dyneither_ptr _tmp4AF=({const char*_tmp113="cannot alias value of type %s to type %s";_tag_dyneither(_tmp113,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp4B0,_tmp4AF,_tag_dyneither(_tmp112,sizeof(void*),2U));});});}}
# 670
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 674
void*_tmp119=p->r;void*_tmp11A=_tmp119;struct Cyc_Absyn_Tvar*_tmp142;struct Cyc_Absyn_Vardecl*_tmp141;struct Cyc_Absyn_Vardecl*_tmp140;struct Cyc_Absyn_Pat*_tmp13F;struct Cyc_Absyn_Vardecl*_tmp13E;struct Cyc_Absyn_Pat*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_Absyn_AggrInfo*_tmp13A;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Pat*_tmp137;switch(*((int*)_tmp11A)){case 6U: _LL1: _tmp137=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp11A)->f1;_LL2: {
# 676
void*_tmp11B=(void*)_check_null(p->topt);void*_tmp11C=_tmp11B;void*_tmp11F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C)->tag == 5U){_LL12: _tmp11F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11C)->f1).ptr_atts).rgn;_LL13:
# 678
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp11F);
({struct Cyc_Tcenv_Tenv*_tmp4B3=te;struct Cyc_Absyn_Pat*_tmp4B2=_tmp137;int _tmp4B1=Cyc_Tcutil_is_noalias_region(_tmp11F,0);Cyc_Tcpat_check_pat_regions_rec(_tmp4B3,_tmp4B2,_tmp4B1,patvars);});
return;}else{_LL14: _LL15:
({void*_tmp11D=0U;({struct _dyneither_ptr _tmp4B4=({const char*_tmp11E="check_pat_regions: bad pointer type";_tag_dyneither(_tmp11E,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4B4,_tag_dyneither(_tmp11D,sizeof(void*),0U));});});}_LL11:;}case 7U: _LL3: _tmp13A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp11A)->f1;_tmp139=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp11A)->f2;_tmp138=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp11A)->f3;_LL4:
# 684
 for(0;_tmp138 != 0;_tmp138=_tmp138->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple18*)_tmp138->hd)).f2,did_noalias_deref,patvars);}
return;case 8U: _LL5: _tmp13B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp11A)->f3;_LL6:
# 688
 did_noalias_deref=0;_tmp13C=_tmp13B;goto _LL8;case 5U: _LL7: _tmp13C=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp11A)->f1;_LL8:
# 690
 for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp13C->hd,did_noalias_deref,patvars);}
return;case 3U: _LL9: _tmp13E=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp11A)->f1;_tmp13D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp11A)->f2;_LLA:
# 694
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp120=(struct _tuple15*)x->hd;struct _tuple15*_tmp121=_tmp120;struct Cyc_Absyn_Vardecl**_tmp12A;struct Cyc_Absyn_Exp*_tmp129;_LL17: _tmp12A=_tmp121->f1;_tmp129=_tmp121->f2;_LL18:;
# 700
if((_tmp12A != 0  && *_tmp12A == _tmp13E) && _tmp129 != 0){
{void*_tmp122=_tmp129->r;void*_tmp123=_tmp122;struct Cyc_Absyn_Exp*_tmp128;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp123)->tag == 15U){_LL1A: _tmp128=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp123)->f1;_LL1B:
# 703
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp128->topt)))
({void*_tmp124=0U;({unsigned int _tmp4B6=p->loc;struct _dyneither_ptr _tmp4B5=({const char*_tmp125="reference pattern not allowed on alias-free pointers";_tag_dyneither(_tmp125,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp4B6,_tmp4B5,_tag_dyneither(_tmp124,sizeof(void*),0U));});});
goto _LL19;}else{_LL1C: _LL1D:
# 707
({void*_tmp126=0U;({struct _dyneither_ptr _tmp4B7=({const char*_tmp127="check_pat_regions: bad reference access expression";_tag_dyneither(_tmp127,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4B7,_tag_dyneither(_tmp126,sizeof(void*),0U));});});}_LL19:;}
# 709
break;}}}
# 712
Cyc_Tcpat_check_pat_regions_rec(te,_tmp13D,did_noalias_deref,patvars);
return;case 1U: _LLB: _tmp140=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp11A)->f1;_tmp13F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp11A)->f2;_LLC:
# 715
{void*_tmp12B=p->topt;void*_tmp12C=_tmp12B;if(_tmp12C != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C)->tag == 8U){_LL1F: _LL20:
# 717
 if(did_noalias_deref){
({void*_tmp12D=0U;({unsigned int _tmp4B9=p->loc;struct _dyneither_ptr _tmp4B8=({const char*_tmp12E="pattern to array would create alias of no-alias pointer";_tag_dyneither(_tmp12E,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp4B9,_tmp4B8,_tag_dyneither(_tmp12D,sizeof(void*),0U));});});
return;}
# 721
goto _LL1E;}else{goto _LL21;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 724
Cyc_Tcpat_check_pat_regions_rec(te,_tmp13F,did_noalias_deref,patvars);
return;case 2U: _LLD: _tmp142=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp11A)->f1;_tmp141=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp11A)->f2;_LLE:
# 727
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp12F=(struct _tuple15*)x->hd;struct _tuple15*_tmp130=_tmp12F;struct Cyc_Absyn_Vardecl**_tmp136;struct Cyc_Absyn_Exp*_tmp135;_LL24: _tmp136=_tmp130->f1;_tmp135=_tmp130->f2;_LL25:;
# 731
if(_tmp136 != 0  && *_tmp136 == _tmp141){
if(_tmp135 == 0)
({void*_tmp131=0U;({unsigned int _tmp4BB=p->loc;struct _dyneither_ptr _tmp4BA=({const char*_tmp132="cannot alias pattern expression in datatype";_tag_dyneither(_tmp132,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp4BB,_tmp4BA,_tag_dyneither(_tmp131,sizeof(void*),0U));});});else{
# 735
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp4BD=p->loc;struct Cyc_Tcenv_Tenv*_tmp4BC=te;Cyc_Tcenv_add_type_vars(_tmp4BD,_tmp4BC,({struct Cyc_Absyn_Tvar*_tmp134[1U];_tmp134[0]=_tmp142;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp134,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
({struct Cyc_Tcenv_Tenv*_tmp4BF=({struct Cyc_Tcenv_Tenv*_tmp4BE=te2;Cyc_Tcenv_add_region(_tmp4BE,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->tag=2U,_tmp133->f1=_tmp142;_tmp133;}),0);});te2=_tmp4BF;});
# 738
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp135->topt),_tmp142,_tmp141->type,_tmp135);}
# 741
break;}}}
# 744
goto _LL0;default: _LLF: _LL10:
 return;}_LL0:;}
# 760 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 762
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp143=(struct _tuple15*)x->hd;struct _tuple15*_tmp144=_tmp143;struct Cyc_Absyn_Vardecl**_tmp14E;struct Cyc_Absyn_Exp*_tmp14D;_LL1: _tmp14E=_tmp144->f1;_tmp14D=_tmp144->f2;_LL2:;
if(_tmp14D != 0){
struct Cyc_Absyn_Exp*_tmp145=_tmp14D;
# 769
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp145->topt)) && !Cyc_Tcutil_is_noalias_path(_tmp145))
# 771
({struct Cyc_String_pa_PrintArg_struct _tmp148=({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0U,({
# 773
struct _dyneither_ptr _tmp4C2=(struct _dyneither_ptr)(_tmp14E != 0?(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0U,({
# 775
struct _dyneither_ptr _tmp4C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*_tmp14E)->name));_tmp3AA.f1=_tmp4C0;});_tmp3AA;});void*_tmp149[1U];_tmp149[0]=& _tmp14B;({struct _dyneither_ptr _tmp4C1=({const char*_tmp14A="for variable %s";_tag_dyneither(_tmp14A,sizeof(char),16U);});Cyc_aprintf(_tmp4C1,_tag_dyneither(_tmp149,sizeof(void*),1U));});}):({const char*_tmp14C="";_tag_dyneither(_tmp14C,sizeof(char),1U);}));
# 773
_tmp3A9.f1=_tmp4C2;});_tmp3A9;});void*_tmp146[1U];_tmp146[0]=& _tmp148;({unsigned int _tmp4C4=p->loc;struct _dyneither_ptr _tmp4C3=({const char*_tmp147="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_dyneither(_tmp147,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp4C4,_tmp4C3,_tag_dyneither(_tmp146,sizeof(void*),1U));});});}}};}
# 829 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1U};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2U};
# 839
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0U};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1U};union Cyc_Tcpat_PatOrWhere;struct Cyc_Tcpat_PathNode;struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 853
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp3AB;(_tmp3AB.Name_v).tag=1U,(_tmp3AB.Name_v).val=s;_tmp3AB;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp3AC;(_tmp3AC.Int_v).tag=2U,(_tmp3AC.Int_v).val=i;_tmp3AC;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 870
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp14F=c1->name;union Cyc_Tcpat_Name_value _tmp150=_tmp14F;int _tmp158;struct _dyneither_ptr _tmp157;if((_tmp150.Name_v).tag == 1){_LL1: _tmp157=(_tmp150.Name_v).val;_LL2: {
# 873
union Cyc_Tcpat_Name_value _tmp151=c2->name;union Cyc_Tcpat_Name_value _tmp152=_tmp151;struct _dyneither_ptr _tmp153;if((_tmp152.Name_v).tag == 1){_LL6: _tmp153=(_tmp152.Name_v).val;_LL7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp157,(struct _dyneither_ptr)_tmp153);}else{_LL8: _LL9:
 return - 1;}_LL5:;}}else{_LL3: _tmp158=(_tmp150.Int_v).val;_LL4: {
# 878
union Cyc_Tcpat_Name_value _tmp154=c2->name;union Cyc_Tcpat_Name_value _tmp155=_tmp154;int _tmp156;if((_tmp155.Name_v).tag == 1){_LLB: _LLC:
 return 1;}else{_LLD: _tmp156=(_tmp155.Int_v).val;_LLE:
 return _tmp158 - _tmp156;}_LLA:;}}_LL0:;}
# 886
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 890
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 894
static unsigned int Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned int ans=0U;
struct Cyc_List_List*_tmp159=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp159))->hd)->name)!= 0){
++ ans;
_tmp159=_tmp159->tl;}
# 901
return ans;}
# 904
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp15A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp15A != 0;_tmp15A=_tmp15A->tl){
struct Cyc_Absyn_Aggrfield*_tmp15B=(struct Cyc_Absyn_Aggrfield*)_tmp15A->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp15B->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 911
({void*_tmp15C=0U;({struct _dyneither_ptr _tmp4C6=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15F=({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0U,_tmp3AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp3AD;});void*_tmp15D[1U];_tmp15D[0]=& _tmp15F;({struct _dyneither_ptr _tmp4C5=({const char*_tmp15E="get_member_offset %s failed";_tag_dyneither(_tmp15E,sizeof(char),28U);});Cyc_aprintf(_tmp4C5,_tag_dyneither(_tmp15D,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C6,_tag_dyneither(_tmp15C,sizeof(void*),0U));});});}
# 914
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp160=pw;struct Cyc_Absyn_Pat*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;if((_tmp160.where_clause).tag == 2){_LL1: _tmp18E=(_tmp160.where_clause).val;_LL2:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->tag=0U,_tmp161->f1=_tmp18E;_tmp161;});}else{_LL3: _tmp18F=(_tmp160.pattern).val;_LL4: {
# 918
void*_tmp162=_tmp18F->r;void*_tmp163=_tmp162;union Cyc_Absyn_AggrInfoU _tmp18D;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Datatypedecl*_tmp18B;struct Cyc_Absyn_Datatypefield*_tmp18A;void*_tmp189;struct Cyc_Absyn_Enumfield*_tmp188;struct Cyc_Absyn_Enumdecl*_tmp187;struct Cyc_Absyn_Enumfield*_tmp186;struct _dyneither_ptr _tmp185;int _tmp184;char _tmp183;enum Cyc_Absyn_Sign _tmp182;int _tmp181;struct Cyc_Absyn_Pat*_tmp180;struct Cyc_Absyn_Pat*_tmp17F;switch(*((int*)_tmp163)){case 1U: _LL6: _tmp17F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp163)->f2;_LL7:
 _tmp180=_tmp17F;goto _LL9;case 3U: _LL8: _tmp180=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp163)->f2;_LL9:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp3AE;(_tmp3AE.pattern).tag=1U,(_tmp3AE.pattern).val=_tmp180;_tmp3AE;}));case 9U: _LLA: _LLB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10U: _LLC: _tmp182=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp163)->f1;_tmp181=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp163)->f2;_LLD:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->tag=6U,_tmp164->f1=(unsigned int)_tmp181;_tmp164;});case 11U: _LLE: _tmp183=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp163)->f1;_LLF:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=6U,_tmp165->f1=(unsigned int)_tmp183;_tmp165;});case 12U: _LL10: _tmp185=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp163)->f1;_tmp184=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp163)->f2;_LL11:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166->tag=5U,_tmp166->f1=_tmp185,_tmp166->f2=_tmp184;_tmp166;});case 13U: _LL12: _tmp187=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp163)->f1;_tmp186=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp163)->f2;_LL13:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167->tag=3U,_tmp167->f1=_tmp187,_tmp167->f2=_tmp186;_tmp167;});case 14U: _LL14: _tmp189=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp163)->f1;_tmp188=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp163)->f2;_LL15:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->tag=4U,_tmp168->f1=_tmp189,_tmp168->f2=_tmp188;_tmp168;});case 6U: _LL16: _LL17:
# 928
{void*_tmp169=Cyc_Tcutil_compress((void*)_check_null(_tmp18F->topt));void*_tmp16A=_tmp169;union Cyc_Absyn_Constraint*_tmp16B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->tag == 5U){_LL1F: _tmp16B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).nullable;_LL20:
# 930
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp16B))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 935
({void*_tmp16C=0U;({struct _dyneither_ptr _tmp4C7=({const char*_tmp16D="non-null pointer type or non-pointer type in pointer pattern";_tag_dyneither(_tmp16D,sizeof(char),61U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C7,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});case 8U: _LL18: _tmp18B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp163)->f1;_tmp18A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp163)->f2;_LL19:
# 937
 if(_tmp18B->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->tag=9U,_tmp16E->f1=_tmp18B,_tmp16E->f2=_tmp18A;_tmp16E;});else{
# 940
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=7U,({int _tmp4C8=(int)Cyc_Tcpat_datatype_tag_number(_tmp18B,_tmp18A->name);_tmp16F->f1=_tmp4C8;}),_tmp16F->f2=_tmp18B,_tmp16F->f3=_tmp18A;_tmp16F;});}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp163)->f1 != 0){_LL1A: _tmp18D=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp163)->f1)->aggr_info;_tmp18C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp163)->f3;_LL1B: {
# 942
struct Cyc_Absyn_Aggrdecl*_tmp170=Cyc_Absyn_get_known_aggrdecl(_tmp18D);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp170->impl))->tagged){
struct _tuple18*_tmp171=(struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp18C))->hd;struct _tuple18*_tmp172=_tmp171;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Pat*_tmp179;_LL24: _tmp17A=_tmp172->f1;_tmp179=_tmp172->f2;_LL25:;{
struct _dyneither_ptr*f;
{void*_tmp173=(void*)((struct Cyc_List_List*)_check_null(_tmp17A))->hd;void*_tmp174=_tmp173;struct _dyneither_ptr*_tmp177;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp174)->tag == 1U){_LL27: _tmp177=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp174)->f1;_LL28:
 f=_tmp177;goto _LL26;}else{_LL29: _LL2A:
({void*_tmp175=0U;({struct _dyneither_ptr _tmp4C9=({const char*_tmp176="no field name in tagged union pattern";_tag_dyneither(_tmp176,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C9,_tag_dyneither(_tmp175,sizeof(void*),0U));});});}_LL26:;}
# 950
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->tag=8U,_tmp178->f1=f,({int _tmp4CA=Cyc_Tcpat_get_member_offset(_tmp170,f);_tmp178->f2=_tmp4CA;});_tmp178;});};}else{
# 952
({void*_tmp17B=0U;({struct _dyneither_ptr _tmp4CB=({const char*_tmp17C="non-tagged aggregate in pattern test";_tag_dyneither(_tmp17C,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4CB,_tag_dyneither(_tmp17B,sizeof(void*),0U));});});}}}else{goto _LL1C;}default: _LL1C: _LL1D:
({void*_tmp17D=0U;({struct _dyneither_ptr _tmp4CC=({const char*_tmp17E="non-test pattern in pattern test";_tag_dyneither(_tmp17E,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4CC,_tag_dyneither(_tmp17D,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}
# 958
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp3AF;(_tmp3AF.pattern).tag=1U,(_tmp3AF.pattern).val=p;_tmp3AF;});}
# 962
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp191=_cycalloc(sizeof(*_tmp191));({union Cyc_Tcpat_Name_value _tmp4CE=Cyc_Tcpat_Name_v(({const char*_tmp190="NULL";_tag_dyneither(_tmp190,sizeof(char),5U);}));_tmp191->name=_tmp4CE;}),_tmp191->arity=0,_tmp191->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4CD=Cyc_Tcpat_pw(p);_tmp191->orig_pat=_tmp4CD;});_tmp191;});}
# 965
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp193=_cycalloc(sizeof(*_tmp193));({union Cyc_Tcpat_Name_value _tmp4D0=Cyc_Tcpat_Name_v(({const char*_tmp192="&";_tag_dyneither(_tmp192,sizeof(char),2U);}));_tmp193->name=_tmp4D0;}),_tmp193->arity=1,_tmp193->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4CF=Cyc_Tcpat_pw(p);_tmp193->orig_pat=_tmp4CF;});_tmp193;});}
# 968
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp195=_cycalloc(sizeof(*_tmp195));({union Cyc_Tcpat_Name_value _tmp4D2=Cyc_Tcpat_Name_v(({const char*_tmp194="&";_tag_dyneither(_tmp194,sizeof(char),2U);}));_tmp195->name=_tmp4D2;}),_tmp195->arity=1,_tmp195->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp4D1=Cyc_Tcpat_pw(p);_tmp195->orig_pat=_tmp4D1;});_tmp195;});}
# 971
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp196=_cycalloc(sizeof(*_tmp196));({union Cyc_Tcpat_Name_value _tmp4D3=Cyc_Tcpat_Int_v(i);_tmp196->name=_tmp4D3;}),_tmp196->arity=0,_tmp196->span=0,_tmp196->orig_pat=p;_tmp196;});}
# 974
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp197=_cycalloc(sizeof(*_tmp197));({union Cyc_Tcpat_Name_value _tmp4D5=Cyc_Tcpat_Name_v(f);_tmp197->name=_tmp4D5;}),_tmp197->arity=0,_tmp197->span=0,({union Cyc_Tcpat_PatOrWhere _tmp4D4=Cyc_Tcpat_pw(p);_tmp197->orig_pat=_tmp4D4;});_tmp197;});}
# 977
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp198=_cycalloc(sizeof(*_tmp198));({union Cyc_Tcpat_Name_value _tmp4D7=Cyc_Tcpat_Int_v((int)c);_tmp198->name=_tmp4D7;}),_tmp198->arity=0,_tmp198->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp4D6=Cyc_Tcpat_pw(p);_tmp198->orig_pat=_tmp4D6;});_tmp198;});}
# 980
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp19A=_cycalloc(sizeof(*_tmp19A));({union Cyc_Tcpat_Name_value _tmp4D8=Cyc_Tcpat_Name_v(({const char*_tmp199="$";_tag_dyneither(_tmp199,sizeof(char),2U);}));_tmp19A->name=_tmp4D8;}),_tmp19A->arity=i,_tmp19A->span=& Cyc_Tcpat_one_opt,_tmp19A->orig_pat=p;_tmp19A;});}
# 985
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D9=Cyc_Tcpat_null_con(p);_tmp19B->f1=_tmp4D9;}),_tmp19B->f2=0;_tmp19B;});}
# 988
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4DA=Cyc_Tcpat_int_con(i,p);_tmp19C->f1=_tmp4DA;}),_tmp19C->f2=0;_tmp19C;});}
# 991
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4DB=Cyc_Tcpat_char_con(c,p);_tmp19D->f1=_tmp4DB;}),_tmp19D->f2=0;_tmp19D;});}
# 994
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4DC=Cyc_Tcpat_float_con(f,p);_tmp19E->f1=_tmp4DC;}),_tmp19E->f2=0;_tmp19E;});}
# 997
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4DE=Cyc_Tcpat_null_ptr_con(p0);_tmp1A0->f1=_tmp4DE;}),({struct Cyc_List_List*_tmp4DD=({struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=p,_tmp19F->tl=0;_tmp19F;});_tmp1A0->f2=_tmp4DD;});_tmp1A0;});}
# 1000
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4E0=Cyc_Tcpat_ptr_con(p0);_tmp1A2->f1=_tmp4E0;}),({struct Cyc_List_List*_tmp4DF=({struct Cyc_List_List*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->hd=p,_tmp1A1->tl=0;_tmp1A1;});_tmp1A2->f2=_tmp4DF;});_tmp1A2;});}
# 1003
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4E2=({int _tmp4E1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp4E1,p);});_tmp1A3->f1=_tmp4E2;}),_tmp1A3->f2=ss;_tmp1A3;});}
# 1006
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1008
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));({union Cyc_Tcpat_Name_value _tmp4E5=Cyc_Tcpat_Name_v(con_name);_tmp1A5->name=_tmp4E5;}),({int _tmp4E4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp1A5->arity=_tmp4E4;}),_tmp1A5->span=span,({union Cyc_Tcpat_PatOrWhere _tmp4E3=Cyc_Tcpat_pw(p);_tmp1A5->orig_pat=_tmp4E3;});_tmp1A5;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->tag=1U,_tmp1A4->f1=c,_tmp1A4->f2=ps;_tmp1A4;});}
# 1013
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1A6=p->r;void*_tmp1A7=_tmp1A6;void*_tmp1E8;struct Cyc_Absyn_Enumfield*_tmp1E7;struct Cyc_Absyn_Enumdecl*_tmp1E6;struct Cyc_Absyn_Enumfield*_tmp1E5;struct Cyc_Absyn_Aggrdecl*_tmp1E4;struct Cyc_List_List*_tmp1E3;struct Cyc_List_List*_tmp1E2;struct Cyc_Absyn_Datatypedecl*_tmp1E1;struct Cyc_Absyn_Datatypefield*_tmp1E0;struct Cyc_List_List*_tmp1DF;struct Cyc_Absyn_Pat*_tmp1DE;struct Cyc_Absyn_Pat*_tmp1DD;struct Cyc_Absyn_Pat*_tmp1DC;struct _dyneither_ptr _tmp1DB;char _tmp1DA;enum Cyc_Absyn_Sign _tmp1D9;int _tmp1D8;switch(*((int*)_tmp1A7)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
({void*_tmp4E6=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->tag=0U;_tmp1A8;});s=_tmp4E6;});goto _LL0;case 9U: _LL7: _LL8:
({void*_tmp4E7=Cyc_Tcpat_null_pat(p);s=_tmp4E7;});goto _LL0;case 10U: _LL9: _tmp1D9=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_tmp1D8=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A7)->f2;_LLA:
({void*_tmp4E9=({int _tmp4E8=_tmp1D8;Cyc_Tcpat_int_pat(_tmp4E8,Cyc_Tcpat_pw(p));});s=_tmp4E9;});goto _LL0;case 11U: _LLB: _tmp1DA=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_LLC:
({void*_tmp4EA=Cyc_Tcpat_char_pat(_tmp1DA,p);s=_tmp4EA;});goto _LL0;case 12U: _LLD: _tmp1DB=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_LLE:
({void*_tmp4EB=Cyc_Tcpat_float_pat(_tmp1DB,p);s=_tmp4EB;});goto _LL0;case 1U: _LLF: _tmp1DC=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1A7)->f2;_LL10:
({void*_tmp4EC=Cyc_Tcpat_compile_pat(_tmp1DC);s=_tmp4EC;});goto _LL0;case 3U: _LL11: _tmp1DD=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1A7)->f2;_LL12:
({void*_tmp4ED=Cyc_Tcpat_compile_pat(_tmp1DD);s=_tmp4ED;});goto _LL0;case 6U: _LL13: _tmp1DE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_LL14:
# 1026
{void*_tmp1A9=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1AA=_tmp1A9;union Cyc_Absyn_Constraint*_tmp1B1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->tag == 5U){_LL28: _tmp1B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->f1).ptr_atts).nullable;_LL29: {
# 1028
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1AB=_tmp1B1;int _tmp1AE;union Cyc_Absyn_Constraint*_tmp1AD;switch((((union Cyc_Absyn_Constraint*)_tmp1AB)->No_constr).tag){case 2U: _LL2D: _tmp1AD=(_tmp1AB->Forward_constr).val;_LL2E:
# 1033
*_tmp1B1=*_tmp1AD;
continue;case 3U: _LL2F: _LL30:
# 1036
({struct _union_Constraint_Eq_constr*_tmp1AC=& _tmp1B1->Eq_constr;_tmp1AC->tag=1;_tmp1AC->val=0;});
is_nullable=0;
still_working=0;
goto _LL2C;default: _LL31: _tmp1AE=(int)(_tmp1AB->Eq_constr).val;_LL32:
# 1041
 is_nullable=_tmp1AE;
still_working=0;
goto _LL2C;}_LL2C:;}{
# 1046
void*ss=Cyc_Tcpat_compile_pat(_tmp1DE);
if(is_nullable)
({void*_tmp4EE=Cyc_Tcpat_null_ptr_pat(ss,p);s=_tmp4EE;});else{
# 1050
({void*_tmp4EF=Cyc_Tcpat_ptr_pat(ss,p);s=_tmp4EF;});}
goto _LL27;};}}else{_LL2A: _LL2B:
({void*_tmp1AF=0U;({struct _dyneither_ptr _tmp4F0=({const char*_tmp1B0="expecting pointertype for pattern!";_tag_dyneither(_tmp1B0,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F0,_tag_dyneither(_tmp1AF,sizeof(void*),0U));});});}_LL27:;}
# 1054
goto _LL0;case 8U: _LL15: _tmp1E1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_tmp1E0=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A7)->f2;_tmp1DF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A7)->f3;_LL16: {
# 1056
int*span;
{void*_tmp1B2=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1B3=_tmp1B2;switch(*((int*)_tmp1B3)){case 3U: _LL34: _LL35:
# 1059
 if(_tmp1E1->is_extensible)
span=0;else{
# 1062
({int*_tmp4F2=({int*_tmp1B4=_cycalloc_atomic(sizeof(*_tmp1B4));({int _tmp4F1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E1->fields))->v);*_tmp1B4=_tmp4F1;});_tmp1B4;});span=_tmp4F2;});}
goto _LL33;case 4U: _LL36: _LL37:
 span=& Cyc_Tcpat_one_opt;goto _LL33;default: _LL38: _LL39:
({int*_tmp4F4=({void*_tmp1B5=0U;({struct _dyneither_ptr _tmp4F3=({const char*_tmp1B6="void datatype pattern has bad type";_tag_dyneither(_tmp1B6,sizeof(char),35U);});((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F3,_tag_dyneither(_tmp1B5,sizeof(void*),0U));});});span=_tmp4F4;});goto _LL33;}_LL33:;}
# 1067
({void*_tmp4F8=({struct _dyneither_ptr _tmp4F7=*(*_tmp1E0->name).f2;int*_tmp4F6=span;struct Cyc_List_List*_tmp4F5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1DF);Cyc_Tcpat_con_pat(_tmp4F7,_tmp4F6,_tmp4F5,p);});s=_tmp4F8;});
goto _LL0;}case 5U: _LL17: _tmp1E2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_LL18:
# 1071
({void*_tmp4FA=({struct Cyc_List_List*_tmp4F9=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1E2);Cyc_Tcpat_tuple_pat(_tmp4F9,Cyc_Tcpat_pw(p));});s=_tmp4FA;});goto _LL0;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1)->aggr_info).KnownAggr).tag == 2){_LL19: _tmp1E4=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1)->aggr_info).KnownAggr).val;_tmp1E3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A7)->f3;_LL1A:
# 1076
 if(_tmp1E4->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E4->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1080
int found=({struct _dyneither_ptr _tmp4FB=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp4FB,({const char*_tmp1C2="";_tag_dyneither(_tmp1C2,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=_tmp1E3;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple18*_tmp1B7=(struct _tuple18*)dlps0->hd;struct _tuple18*_tmp1B8=_tmp1B7;struct Cyc_List_List*_tmp1BF;struct Cyc_Absyn_Pat*_tmp1BE;_LL3B: _tmp1BF=_tmp1B8->f1;_tmp1BE=_tmp1B8->f2;_LL3C:;{
struct Cyc_List_List*_tmp1B9=_tmp1BF;struct _dyneither_ptr*_tmp1BD;if(_tmp1B9 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1B9)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp1B9)->tl == 0){_LL3E: _tmp1BD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1B9->hd)->f1;_LL3F:
# 1085
 if(Cyc_strptrcmp(_tmp1BD,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
({struct Cyc_List_List*_tmp4FD=({struct Cyc_List_List*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));({void*_tmp4FC=Cyc_Tcpat_compile_pat(_tmp1BE);_tmp1BA->hd=_tmp4FC;}),_tmp1BA->tl=ps;_tmp1BA;});ps=_tmp4FD;});
found=1;}
# 1089
goto _LL3D;}else{goto _LL40;}}else{goto _LL40;}}else{_LL40: _LL41:
({void*_tmp1BB=0U;({struct _dyneither_ptr _tmp4FE=({const char*_tmp1BC="bad designator(s)";_tag_dyneither(_tmp1BC,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FE,_tag_dyneither(_tmp1BB,sizeof(void*),0U));});});}_LL3D:;};}}
# 1093
if(!found)
({void*_tmp1C0=0U;({struct _dyneither_ptr _tmp4FF=({const char*_tmp1C1="bad designator";_tag_dyneither(_tmp1C1,sizeof(char),15U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FF,_tag_dyneither(_tmp1C0,sizeof(void*),0U));});});}}
# 1096
({void*_tmp501=({struct Cyc_List_List*_tmp500=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp500,Cyc_Tcpat_pw(p));});s=_tmp501;});}else{
# 1099
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E4->impl))->tagged)
({void*_tmp1C3=0U;({unsigned int _tmp503=p->loc;struct _dyneither_ptr _tmp502=({const char*_tmp1C4="patterns on untagged unions not yet supported.";_tag_dyneither(_tmp1C4,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp503,_tmp502,_tag_dyneither(_tmp1C3,sizeof(void*),0U));});});{
int*span=({int*_tmp1CB=_cycalloc_atomic(sizeof(*_tmp1CB));({int _tmp504=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E4->impl))->fields);*_tmp1CB=_tmp504;});_tmp1CB;});
struct Cyc_List_List*_tmp1C5=_tmp1E3;struct _dyneither_ptr*_tmp1CA;struct Cyc_Absyn_Pat*_tmp1C9;if(_tmp1C5 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp1C5)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1C5)->hd)->f1)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1C5)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp1C5)->tl == 0){_LL43: _tmp1CA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp1C5->hd)->f1)->hd)->f1;_tmp1C9=((struct _tuple18*)_tmp1C5->hd)->f2;_LL44:
# 1104
({void*_tmp509=({struct _dyneither_ptr _tmp508=*_tmp1CA;int*_tmp507=span;struct Cyc_List_List*_tmp506=({struct Cyc_List_List*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));({void*_tmp505=Cyc_Tcpat_compile_pat(_tmp1C9);_tmp1C6->hd=_tmp505;}),_tmp1C6->tl=0;_tmp1C6;});Cyc_Tcpat_con_pat(_tmp508,_tmp507,_tmp506,p);});s=_tmp509;});
goto _LL42;}else{goto _LL45;}}else{goto _LL45;}}else{goto _LL45;}}else{goto _LL45;}}else{_LL45: _LL46:
({void*_tmp1C7=0U;({struct _dyneither_ptr _tmp50A=({const char*_tmp1C8="bad union pattern";_tag_dyneither(_tmp1C8,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50A,_tag_dyneither(_tmp1C7,sizeof(void*),0U));});});}_LL42:;};}
# 1109
goto _LL0;}else{goto _LL23;}}else{_LL23: _LL24:
# 1151
 goto _LL26;}case 13U: _LL1B: _tmp1E6=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_tmp1E5=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A7)->f2;_LL1C:
# 1114
{void*_tmp1CC=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1CD=_tmp1CC;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1CD)->tag == 6U){_LL48: _LL49:
# 1118
({void*_tmp50B=Cyc_Tcpat_con_pat(*(*_tmp1E5->name).f2,0,0,p);s=_tmp50B;});
goto _LL47;}else{_LL4A: _LL4B: {
# 1121
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E6->fields))->v);
({void*_tmp50E=({struct _dyneither_ptr _tmp50D=*(*_tmp1E5->name).f2;int*_tmp50C=({int*_tmp1CE=_cycalloc_atomic(sizeof(*_tmp1CE));*_tmp1CE=span;_tmp1CE;});Cyc_Tcpat_con_pat(_tmp50D,_tmp50C,0,p);});s=_tmp50E;});
goto _LL47;}}_LL47:;}
# 1125
goto _LL0;case 14U: _LL1D: _tmp1E8=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A7)->f1;_tmp1E7=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A7)->f2;_LL1E: {
# 1130
struct Cyc_List_List*fields;
{void*_tmp1CF=Cyc_Tcutil_compress(_tmp1E8);void*_tmp1D0=_tmp1CF;struct Cyc_List_List*_tmp1D3;if(((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1D0)->tag == 14U){_LL4D: _tmp1D3=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1D0)->f1;_LL4E:
 fields=_tmp1D3;goto _LL4C;}else{_LL4F: _LL50:
({void*_tmp1D1=0U;({struct _dyneither_ptr _tmp50F=({const char*_tmp1D2="bad type in AnonEnum_p";_tag_dyneither(_tmp1D2,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50F,_tag_dyneither(_tmp1D1,sizeof(void*),0U));});});}_LL4C:;}
# 1137
{void*_tmp1D4=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1D5=_tmp1D4;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1D5)->tag == 6U){_LL52: _LL53:
# 1141
({void*_tmp510=Cyc_Tcpat_con_pat(*(*_tmp1E7->name).f2,0,0,p);s=_tmp510;});
goto _LL51;}else{_LL54: _LL55: {
# 1144
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
({void*_tmp513=({struct _dyneither_ptr _tmp512=*(*_tmp1E7->name).f2;int*_tmp511=({int*_tmp1D6=_cycalloc_atomic(sizeof(*_tmp1D6));*_tmp1D6=span;_tmp1D6;});Cyc_Tcpat_con_pat(_tmp512,_tmp511,0,p);});s=_tmp513;});
goto _LL51;}}_LL51:;}
# 1148
goto _LL0;}case 15U: _LL1F: _LL20:
 goto _LL22;case 16U: _LL21: _LL22:
 goto _LL24;default: _LL25: _LL26:
# 1152
({void*_tmp514=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->tag=0U;_tmp1D7;});s=_tmp514;});}_LL0:;}
# 1154
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1183
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp1E9=({struct _tuple0 _tmp3B0;_tmp3B0.f1=a1,_tmp3B0.f2=a2;_tmp3B0;});struct _tuple0 _tmp1EA=_tmp1E9;int _tmp1F4;struct _dyneither_ptr*_tmp1F3;int _tmp1F2;struct _dyneither_ptr*_tmp1F1;struct Cyc_Absyn_Datatypefield*_tmp1F0;unsigned int _tmp1EF;struct Cyc_Absyn_Datatypefield*_tmp1EE;unsigned int _tmp1ED;unsigned int _tmp1EC;unsigned int _tmp1EB;switch(*((int*)_tmp1EA.f1)){case 0U: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp1EA.f2)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LLB;}case 1U: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp1EA.f2)->tag == 1U){_LL3: _LL4:
 return 1;}else{goto _LLB;}case 2U: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1EA.f2)->tag == 2U){_LL5: _tmp1EC=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1EA.f1)->f1;_tmp1EB=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1EA.f2)->f1;_LL6:
 return _tmp1EC == _tmp1EB;}else{goto _LLB;}case 3U: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1EA.f2)->tag == 3U){_LL7: _tmp1F0=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1EA.f1)->f2;_tmp1EF=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1EA.f1)->f3;_tmp1EE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1EA.f2)->f2;_tmp1ED=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1EA.f2)->f3;_LL8:
# 1189
 return _tmp1F0 == _tmp1EE  && _tmp1EF == _tmp1ED;}else{goto _LLB;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1EA.f2)->tag == 4U){_LL9: _tmp1F4=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1EA.f1)->f1;_tmp1F3=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1EA.f1)->f2;_tmp1F2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1EA.f2)->f1;_tmp1F1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1EA.f2)->f2;_LLA:
# 1191
 return _tmp1F4 == _tmp1F2  && Cyc_strptrcmp(_tmp1F3,_tmp1F1)== 0;}else{_LLB: _LLC:
 return 0;}}_LL0:;}
# 1196
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0  && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1202
if(p1 != p2)return 0;
return 1;}
# 1206
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp1F5=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp1F6=d2;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FB;void*_tmp1FA;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1F6)->tag == 2U){_LL1: _tmp1FC=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1F6)->f1;_tmp1FB=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1F6)->f2;_tmp1FA=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1F6)->f3;_LL2:
# 1210
 if(Cyc_Tcpat_same_path(access,_tmp1FC) && (int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->tag=2U,_tmp1F9->f1=_tmp1FC,({
struct Cyc_List_List*_tmp516=({struct Cyc_List_List*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));({struct _tuple0*_tmp515=({struct _tuple0*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->f1=_tmp1F5,_tmp1F7->f2=d1;_tmp1F7;});_tmp1F8->hd=_tmp515;}),_tmp1F8->tl=_tmp1FB;_tmp1F8;});_tmp1F9->f2=_tmp516;}),_tmp1F9->f3=_tmp1FA;_tmp1F9;});else{
# 1214
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1217
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->tag=2U,_tmp1FF->f1=access,({struct Cyc_List_List*_tmp518=({struct Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));({struct _tuple0*_tmp517=({struct _tuple0*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->f1=_tmp1F5,_tmp1FD->f2=d1;_tmp1FD;});_tmp1FE->hd=_tmp517;}),_tmp1FE->tl=0;_tmp1FE;});_tmp1FF->f2=_tmp518;}),_tmp1FF->f3=d2;_tmp1FF;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1228
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0U,Cyc_Tcpat_No  = 1U,Cyc_Tcpat_Maybe  = 2U};
# 1231
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp200=0U;({struct Cyc___cycFILE*_tmp51A=Cyc_stderr;struct _dyneither_ptr _tmp519=({const char*_tmp201=" ";_tag_dyneither(_tmp201,sizeof(char),2U);});Cyc_fprintf(_tmp51A,_tmp519,_tag_dyneither(_tmp200,sizeof(void*),0U));});});}}
# 1237
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp202=c->name;
union Cyc_Tcpat_Name_value _tmp203=_tmp202;int _tmp20B;struct _dyneither_ptr _tmp20A;if((_tmp203.Name_v).tag == 1){_LL1: _tmp20A=(_tmp203.Name_v).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp206=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0U,_tmp3B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20A);_tmp3B1;});void*_tmp204[1U];_tmp204[0]=& _tmp206;({struct Cyc___cycFILE*_tmp51C=Cyc_stderr;struct _dyneither_ptr _tmp51B=({const char*_tmp205="%s";_tag_dyneither(_tmp205,sizeof(char),3U);});Cyc_fprintf(_tmp51C,_tmp51B,_tag_dyneither(_tmp204,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp20B=(_tmp203.Int_v).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp209=({struct Cyc_Int_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=1U,_tmp3B2.f1=(unsigned long)_tmp20B;_tmp3B2;});void*_tmp207[1U];_tmp207[0]=& _tmp209;({struct Cyc___cycFILE*_tmp51E=Cyc_stderr;struct _dyneither_ptr _tmp51D=({const char*_tmp208="%d";_tag_dyneither(_tmp208,sizeof(char),3U);});Cyc_fprintf(_tmp51E,_tmp51D,_tag_dyneither(_tmp207,sizeof(void*),1U));});});goto _LL0;}_LL0:;}
# 1245
static void Cyc_Tcpat_print_access(void*a){
void*_tmp20C=a;int _tmp222;struct _dyneither_ptr*_tmp221;struct Cyc_Absyn_Datatypefield*_tmp220;unsigned int _tmp21F;unsigned int _tmp21E;switch(*((int*)_tmp20C)){case 0U: _LL1: _LL2:
({void*_tmp20D=0U;({struct Cyc___cycFILE*_tmp520=Cyc_stderr;struct _dyneither_ptr _tmp51F=({const char*_tmp20E="DUMMY";_tag_dyneither(_tmp20E,sizeof(char),6U);});Cyc_fprintf(_tmp520,_tmp51F,_tag_dyneither(_tmp20D,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp20F=0U;({struct Cyc___cycFILE*_tmp522=Cyc_stderr;struct _dyneither_ptr _tmp521=({const char*_tmp210="*";_tag_dyneither(_tmp210,sizeof(char),2U);});Cyc_fprintf(_tmp522,_tmp521,_tag_dyneither(_tmp20F,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp21E=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp20C)->f1;_LL6:
({struct Cyc_Int_pa_PrintArg_struct _tmp213=({struct Cyc_Int_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=1U,_tmp3B3.f1=(unsigned long)((int)_tmp21E);_tmp3B3;});void*_tmp211[1U];_tmp211[0]=& _tmp213;({struct Cyc___cycFILE*_tmp524=Cyc_stderr;struct _dyneither_ptr _tmp523=({const char*_tmp212="[%d]";_tag_dyneither(_tmp212,sizeof(char),5U);});Cyc_fprintf(_tmp524,_tmp523,_tag_dyneither(_tmp211,sizeof(void*),1U));});});goto _LL0;case 3U: _LL7: _tmp220=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp20C)->f2;_tmp21F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp20C)->f3;_LL8:
# 1251
({struct Cyc_String_pa_PrintArg_struct _tmp216=({struct Cyc_String_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=0U,({struct _dyneither_ptr _tmp525=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp220->name));_tmp3B5.f1=_tmp525;});_tmp3B5;});struct Cyc_Int_pa_PrintArg_struct _tmp217=({struct Cyc_Int_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=1U,_tmp3B4.f1=(unsigned long)((int)_tmp21F);_tmp3B4;});void*_tmp214[2U];_tmp214[0]=& _tmp216,_tmp214[1]=& _tmp217;({struct Cyc___cycFILE*_tmp527=Cyc_stderr;struct _dyneither_ptr _tmp526=({const char*_tmp215="%s[%d]";_tag_dyneither(_tmp215,sizeof(char),7U);});Cyc_fprintf(_tmp527,_tmp526,_tag_dyneither(_tmp214,sizeof(void*),2U));});});goto _LL0;default: _LL9: _tmp222=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp20C)->f1;_tmp221=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp20C)->f2;_LLA:
# 1253
 if(_tmp222)
({struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0U,_tmp3B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp221);_tmp3B6;});void*_tmp218[1U];_tmp218[0]=& _tmp21A;({struct Cyc___cycFILE*_tmp529=Cyc_stderr;struct _dyneither_ptr _tmp528=({const char*_tmp219=".tagunion.%s";_tag_dyneither(_tmp219,sizeof(char),13U);});Cyc_fprintf(_tmp529,_tmp528,_tag_dyneither(_tmp218,sizeof(void*),1U));});});else{
# 1256
({struct Cyc_String_pa_PrintArg_struct _tmp21D=({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0U,_tmp3B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp221);_tmp3B7;});void*_tmp21B[1U];_tmp21B[0]=& _tmp21D;({struct Cyc___cycFILE*_tmp52B=Cyc_stderr;struct _dyneither_ptr _tmp52A=({const char*_tmp21C=".%s";_tag_dyneither(_tmp21C,sizeof(char),4U);});Cyc_fprintf(_tmp52B,_tmp52A,_tag_dyneither(_tmp21B,sizeof(void*),1U));});});}
goto _LL0;}_LL0:;}
# 1261
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp223=p;struct Cyc_Absyn_Datatypefield*_tmp248;struct _dyneither_ptr*_tmp247;int _tmp246;int _tmp245;unsigned int _tmp244;struct _dyneither_ptr _tmp243;struct Cyc_Absyn_Enumfield*_tmp242;struct Cyc_Absyn_Enumfield*_tmp241;struct Cyc_Absyn_Exp*_tmp240;switch(*((int*)_tmp223)){case 0U: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp223)->f1 == 0){_LL1: _LL2:
({void*_tmp224=0U;({struct Cyc___cycFILE*_tmp52D=Cyc_stderr;struct _dyneither_ptr _tmp52C=({const char*_tmp225="where(NULL)";_tag_dyneither(_tmp225,sizeof(char),12U);});Cyc_fprintf(_tmp52D,_tmp52C,_tag_dyneither(_tmp224,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp240=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp223)->f1;_LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0U,({struct _dyneither_ptr _tmp52E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp240)));_tmp3B8.f1=_tmp52E;});_tmp3B8;});void*_tmp226[1U];_tmp226[0]=& _tmp228;({struct Cyc___cycFILE*_tmp530=Cyc_stderr;struct _dyneither_ptr _tmp52F=({const char*_tmp227="where(%s)";_tag_dyneither(_tmp227,sizeof(char),10U);});Cyc_fprintf(_tmp530,_tmp52F,_tag_dyneither(_tmp226,sizeof(void*),1U));});});goto _LL0;}case 1U: _LL5: _LL6:
({void*_tmp229=0U;({struct Cyc___cycFILE*_tmp532=Cyc_stderr;struct _dyneither_ptr _tmp531=({const char*_tmp22A="NULL";_tag_dyneither(_tmp22A,sizeof(char),5U);});Cyc_fprintf(_tmp532,_tmp531,_tag_dyneither(_tmp229,sizeof(void*),0U));});});goto _LL0;case 2U: _LL7: _LL8:
({void*_tmp22B=0U;({struct Cyc___cycFILE*_tmp534=Cyc_stderr;struct _dyneither_ptr _tmp533=({const char*_tmp22C="NOT-NULL:";_tag_dyneither(_tmp22C,sizeof(char),10U);});Cyc_fprintf(_tmp534,_tmp533,_tag_dyneither(_tmp22B,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp241=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp223)->f2;_LLA:
 _tmp242=_tmp241;goto _LLC;case 3U: _LLB: _tmp242=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp223)->f2;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp22F=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0U,({struct _dyneither_ptr _tmp535=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp242->name));_tmp3B9.f1=_tmp535;});_tmp3B9;});void*_tmp22D[1U];_tmp22D[0]=& _tmp22F;({struct Cyc___cycFILE*_tmp537=Cyc_stderr;struct _dyneither_ptr _tmp536=({const char*_tmp22E="%s";_tag_dyneither(_tmp22E,sizeof(char),3U);});Cyc_fprintf(_tmp537,_tmp536,_tag_dyneither(_tmp22D,sizeof(void*),1U));});});goto _LL0;case 5U: _LLD: _tmp243=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp223)->f1;_LLE:
({struct Cyc_String_pa_PrintArg_struct _tmp232=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0U,_tmp3BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp243);_tmp3BA;});void*_tmp230[1U];_tmp230[0]=& _tmp232;({struct Cyc___cycFILE*_tmp539=Cyc_stderr;struct _dyneither_ptr _tmp538=({const char*_tmp231="%s";_tag_dyneither(_tmp231,sizeof(char),3U);});Cyc_fprintf(_tmp539,_tmp538,_tag_dyneither(_tmp230,sizeof(void*),1U));});});goto _LL0;case 6U: _LLF: _tmp244=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp223)->f1;_LL10:
({struct Cyc_Int_pa_PrintArg_struct _tmp235=({struct Cyc_Int_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=1U,_tmp3BB.f1=(unsigned long)((int)_tmp244);_tmp3BB;});void*_tmp233[1U];_tmp233[0]=& _tmp235;({struct Cyc___cycFILE*_tmp53B=Cyc_stderr;struct _dyneither_ptr _tmp53A=({const char*_tmp234="%d";_tag_dyneither(_tmp234,sizeof(char),3U);});Cyc_fprintf(_tmp53B,_tmp53A,_tag_dyneither(_tmp233,sizeof(void*),1U));});});goto _LL0;case 7U: _LL11: _tmp245=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp223)->f1;_LL12:
({struct Cyc_Int_pa_PrintArg_struct _tmp238=({struct Cyc_Int_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=1U,_tmp3BC.f1=(unsigned long)_tmp245;_tmp3BC;});void*_tmp236[1U];_tmp236[0]=& _tmp238;({struct Cyc___cycFILE*_tmp53D=Cyc_stderr;struct _dyneither_ptr _tmp53C=({const char*_tmp237="datatypetag(%d)";_tag_dyneither(_tmp237,sizeof(char),16U);});Cyc_fprintf(_tmp53D,_tmp53C,_tag_dyneither(_tmp236,sizeof(void*),1U));});});goto _LL0;case 8U: _LL13: _tmp247=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp223)->f1;_tmp246=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp223)->f2;_LL14:
({struct Cyc_String_pa_PrintArg_struct _tmp23B=({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0U,_tmp3BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp247);_tmp3BE;});struct Cyc_Int_pa_PrintArg_struct _tmp23C=({struct Cyc_Int_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=1U,_tmp3BD.f1=(unsigned long)_tmp246;_tmp3BD;});void*_tmp239[2U];_tmp239[0]=& _tmp23B,_tmp239[1]=& _tmp23C;({struct Cyc___cycFILE*_tmp53F=Cyc_stderr;struct _dyneither_ptr _tmp53E=({const char*_tmp23A="uniontag[%s](%d)";_tag_dyneither(_tmp23A,sizeof(char),17U);});Cyc_fprintf(_tmp53F,_tmp53E,_tag_dyneither(_tmp239,sizeof(void*),2U));});});goto _LL0;default: _LL15: _tmp248=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp223)->f2;_LL16:
# 1274
({struct Cyc_String_pa_PrintArg_struct _tmp23F=({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0U,({struct _dyneither_ptr _tmp540=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp248->name));_tmp3BF.f1=_tmp540;});_tmp3BF;});void*_tmp23D[1U];_tmp23D[0]=& _tmp23F;({struct Cyc___cycFILE*_tmp542=Cyc_stderr;struct _dyneither_ptr _tmp541=({const char*_tmp23E="datatypefield(%s)";_tag_dyneither(_tmp23E,sizeof(char),18U);});Cyc_fprintf(_tmp542,_tmp541,_tag_dyneither(_tmp23D,sizeof(void*),1U));});});}_LL0:;}
# 1278
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp24B=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0U,({struct _dyneither_ptr _tmp543=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp3C0.f1=_tmp543;});_tmp3C0;});void*_tmp249[1U];_tmp249[0]=& _tmp24B;({struct Cyc___cycFILE*_tmp545=Cyc_stderr;struct _dyneither_ptr _tmp544=({const char*_tmp24A="%s";_tag_dyneither(_tmp24A,sizeof(char),3U);});Cyc_fprintf(_tmp545,_tmp544,_tag_dyneither(_tmp249,sizeof(void*),1U));});});}
# 1282
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp24C=d;struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp267;void*_tmp266;struct Cyc_Tcpat_Rhs*_tmp265;switch(*((int*)_tmp24C)){case 1U: _LL1: _tmp265=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp24C)->f1;_LL2:
# 1285
 Cyc_Tcpat_print_tab(tab);
({void*_tmp24D=0U;({struct Cyc___cycFILE*_tmp547=Cyc_stderr;struct _dyneither_ptr _tmp546=({const char*_tmp24E="Success(";_tag_dyneither(_tmp24E,sizeof(char),9U);});Cyc_fprintf(_tmp547,_tmp546,_tag_dyneither(_tmp24D,sizeof(void*),0U));});});Cyc_Tcpat_print_rhs(_tmp265);({void*_tmp24F=0U;({struct Cyc___cycFILE*_tmp549=Cyc_stderr;struct _dyneither_ptr _tmp548=({const char*_tmp250=")\n";_tag_dyneither(_tmp250,sizeof(char),3U);});Cyc_fprintf(_tmp549,_tmp548,_tag_dyneither(_tmp24F,sizeof(void*),0U));});});
goto _LL0;case 0U: _LL3: _LL4:
({void*_tmp251=0U;({struct Cyc___cycFILE*_tmp54B=Cyc_stderr;struct _dyneither_ptr _tmp54A=({const char*_tmp252="Failure\n";_tag_dyneither(_tmp252,sizeof(char),9U);});Cyc_fprintf(_tmp54B,_tmp54A,_tag_dyneither(_tmp251,sizeof(void*),0U));});});goto _LL0;default: _LL5: _tmp268=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp24C)->f1;_tmp267=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp24C)->f2;_tmp266=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp24C)->f3;_LL6:
# 1290
 Cyc_Tcpat_print_tab(tab);
({void*_tmp253=0U;({struct Cyc___cycFILE*_tmp54D=Cyc_stderr;struct _dyneither_ptr _tmp54C=({const char*_tmp254="Switch[";_tag_dyneither(_tmp254,sizeof(char),8U);});Cyc_fprintf(_tmp54D,_tmp54C,_tag_dyneither(_tmp253,sizeof(void*),0U));});});
for(0;_tmp268 != 0;_tmp268=_tmp268->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp268->hd)->access);
if(_tmp268->tl != 0)({void*_tmp255=0U;({struct Cyc___cycFILE*_tmp54F=Cyc_stderr;struct _dyneither_ptr _tmp54E=({const char*_tmp256=",";_tag_dyneither(_tmp256,sizeof(char),2U);});Cyc_fprintf(_tmp54F,_tmp54E,_tag_dyneither(_tmp255,sizeof(void*),0U));});});}
# 1296
({void*_tmp257=0U;({struct Cyc___cycFILE*_tmp551=Cyc_stderr;struct _dyneither_ptr _tmp550=({const char*_tmp258="] {\n";_tag_dyneither(_tmp258,sizeof(char),5U);});Cyc_fprintf(_tmp551,_tmp550,_tag_dyneither(_tmp257,sizeof(void*),0U));});});
for(0;_tmp267 != 0;_tmp267=_tmp267->tl){
struct _tuple0 _tmp259=*((struct _tuple0*)_tmp267->hd);struct _tuple0 _tmp25A=_tmp259;void*_tmp260;void*_tmp25F;_LL8: _tmp260=_tmp25A.f1;_tmp25F=_tmp25A.f2;_LL9:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp25B=0U;({struct Cyc___cycFILE*_tmp553=Cyc_stderr;struct _dyneither_ptr _tmp552=({const char*_tmp25C="case ";_tag_dyneither(_tmp25C,sizeof(char),6U);});Cyc_fprintf(_tmp553,_tmp552,_tag_dyneither(_tmp25B,sizeof(void*),0U));});});
Cyc_Tcpat_print_pat_test(_tmp260);
({void*_tmp25D=0U;({struct Cyc___cycFILE*_tmp555=Cyc_stderr;struct _dyneither_ptr _tmp554=({const char*_tmp25E=":\n";_tag_dyneither(_tmp25E,sizeof(char),3U);});Cyc_fprintf(_tmp555,_tmp554,_tag_dyneither(_tmp25D,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp25F,tab + 7);}
# 1305
Cyc_Tcpat_print_tab(tab);
({void*_tmp261=0U;({struct Cyc___cycFILE*_tmp557=Cyc_stderr;struct _dyneither_ptr _tmp556=({const char*_tmp262="default:\n";_tag_dyneither(_tmp262,sizeof(char),10U);});Cyc_fprintf(_tmp557,_tmp556,_tag_dyneither(_tmp261,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp266,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp263=0U;({struct Cyc___cycFILE*_tmp559=Cyc_stderr;struct _dyneither_ptr _tmp558=({const char*_tmp264="}\n";_tag_dyneither(_tmp264,sizeof(char),3U);});Cyc_fprintf(_tmp559,_tmp558,_tag_dyneither(_tmp263,sizeof(void*),0U));});});}_LL0:;}
# 1313
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1321
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp269=td;struct Cyc_Set_Set*_tmp26D;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp269)->tag == 1U){_LL1: _tmp26D=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp269)->f1;_LL2:
# 1331
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->tag=1U,({struct Cyc_Set_Set*_tmp55A=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp26D,c);_tmp26A->f1=_tmp55A;});_tmp26A;});}else{_LL3: _LL4:
({void*_tmp26B=0U;({struct _dyneither_ptr _tmp55B=({const char*_tmp26C="add_neg called when td is Positive";_tag_dyneither(_tmp26C,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp55B,_tag_dyneither(_tmp26B,sizeof(void*),0U));});});}_LL0:;}
# 1338
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp26E=td;struct Cyc_Set_Set*_tmp270;struct Cyc_Tcpat_Con_s*_tmp26F;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp26E)->tag == 0U){_LL1: _tmp26F=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp26E)->f1;_LL2:
# 1342
 if(Cyc_Tcpat_compare_con(c,_tmp26F)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL3: _tmp270=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp26E)->f1;_LL4:
# 1346
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp270,c))return Cyc_Tcpat_No;else{
# 1349
if(c->span != 0  && ({int _tmp55C=*((int*)_check_null(c->span));_tmp55C == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp270)+ 1;}))
return Cyc_Tcpat_Yes;else{
# 1352
return Cyc_Tcpat_Maybe;}}}_LL0:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1360
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp271=ctxt;struct Cyc_Tcpat_Con_s*_tmp277;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp275;if(_tmp271 == 0){_LL1: _LL2:
 return 0;}else{_LL3: _tmp277=((struct _tuple21*)_tmp271->hd)->f1;_tmp276=((struct _tuple21*)_tmp271->hd)->f2;_tmp275=_tmp271->tl;_LL4:
# 1364
 return({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));
({struct _tuple21*_tmp55E=({struct _tuple21*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->f1=_tmp277,({struct Cyc_List_List*_tmp55D=({struct Cyc_List_List*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->hd=dsc,_tmp272->tl=_tmp276;_tmp272;});_tmp273->f2=_tmp55D;});_tmp273;});_tmp274->hd=_tmp55E;}),_tmp274->tl=_tmp275;_tmp274;});}_LL0:;}
# 1372
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp278=ctxt;struct Cyc_Tcpat_Con_s*_tmp27E;struct Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp27C;if(_tmp278 == 0){_LL1: _LL2:
({void*_tmp279=0U;({struct _dyneither_ptr _tmp55F=({const char*_tmp27A="norm_context: empty context";_tag_dyneither(_tmp27A,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp55F,_tag_dyneither(_tmp279,sizeof(void*),0U));});});}else{_LL3: _tmp27E=((struct _tuple21*)_tmp278->hd)->f1;_tmp27D=((struct _tuple21*)_tmp278->hd)->f2;_tmp27C=_tmp278->tl;_LL4:
# 1376
 return({struct Cyc_List_List*_tmp561=_tmp27C;Cyc_Tcpat_augment(_tmp561,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->tag=0U,_tmp27B->f1=_tmp27E,({struct Cyc_List_List*_tmp560=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp27D);_tmp27B->f2=_tmp560;});_tmp27B;}));});}_LL0:;}
# 1385
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1387
struct _tuple1 _tmp27F=({struct _tuple1 _tmp3C1;_tmp3C1.f1=ctxt,_tmp3C1.f2=work;_tmp3C1;});struct _tuple1 _tmp280=_tmp27F;struct Cyc_Tcpat_Con_s*_tmp28A;struct Cyc_List_List*_tmp289;struct Cyc_List_List*_tmp288;struct Cyc_List_List*_tmp287;struct Cyc_List_List*_tmp286;if(_tmp280.f1 == 0){if(_tmp280.f2 == 0){_LL1: _LL2:
 return dsc;}else{_LL3: _LL4:
 goto _LL6;}}else{if(_tmp280.f2 == 0){_LL5: _LL6:
({void*_tmp281=0U;({struct _dyneither_ptr _tmp562=({const char*_tmp282="build_desc: ctxt and work don't match";_tag_dyneither(_tmp282,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp562,_tag_dyneither(_tmp281,sizeof(void*),0U));});});}else{_LL7: _tmp28A=((struct _tuple21*)(_tmp280.f1)->hd)->f1;_tmp289=((struct _tuple21*)(_tmp280.f1)->hd)->f2;_tmp288=(_tmp280.f1)->tl;_tmp287=((struct _tuple20*)(_tmp280.f2)->hd)->f3;_tmp286=(_tmp280.f2)->tl;_LL8: {
# 1392
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp283=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285->tag=0U,_tmp285->f1=_tmp28A,({struct Cyc_List_List*_tmp564=({struct Cyc_List_List*_tmp563=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp289);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp563,({struct Cyc_List_List*_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284->hd=dsc,_tmp284->tl=_tmp287;_tmp284;}));});_tmp285->f2=_tmp564;});_tmp285;});
return Cyc_Tcpat_build_desc(_tmp288,(void*)_tmp283,_tmp286);}}}_LL0:;}
# 1397
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1404
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp28B=allmrules;void*_tmp28F;struct Cyc_Tcpat_Rhs*_tmp28E;struct Cyc_List_List*_tmp28D;if(_tmp28B == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->tag=0U,_tmp28C->f1=dsc;_tmp28C;});}else{_LL3: _tmp28F=((struct _tuple22*)_tmp28B->hd)->f1;_tmp28E=((struct _tuple22*)_tmp28B->hd)->f2;_tmp28D=_tmp28B->tl;_LL4:
# 1408
 return Cyc_Tcpat_match(_tmp28F,0,dsc,0,0,_tmp28E,_tmp28D);}_LL0:;}
# 1413
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp566=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp290=_cycalloc(sizeof(*_tmp290));_tmp290->tag=1U,({struct Cyc_Set_Set*_tmp565=Cyc_Tcpat_empty_con_set();_tmp290->f1=_tmp565;});_tmp290;});Cyc_Tcpat_or_match(_tmp566,allmrules);});}
# 1420
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1423
struct Cyc_List_List*_tmp291=work;struct Cyc_List_List*_tmp2A8;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A6;struct Cyc_List_List*_tmp2A5;struct Cyc_List_List*_tmp2A4;if(_tmp291 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->tag=1U,_tmp292->f1=right_hand_side;_tmp292;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp291)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp291)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp291)->hd)->f3 == 0){_LL3: _tmp2A4=_tmp291->tl;_LL4:
# 1426
 return({struct Cyc_List_List*_tmp569=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp568=_tmp2A4;struct Cyc_Tcpat_Rhs*_tmp567=right_hand_side;Cyc_Tcpat_and_match(_tmp569,_tmp568,_tmp567,rules);});}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp2A8=((struct _tuple20*)_tmp291->hd)->f1;_tmp2A7=((struct _tuple20*)_tmp291->hd)->f2;_tmp2A6=((struct _tuple20*)_tmp291->hd)->f3;_tmp2A5=_tmp291->tl;_LL6:
# 1428
 if((_tmp2A8 == 0  || _tmp2A7 == 0) || _tmp2A6 == 0)
({void*_tmp293=0U;({struct _dyneither_ptr _tmp56A=({const char*_tmp294="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp294,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp56A,_tag_dyneither(_tmp293,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp295=_tmp2A8;struct Cyc_List_List*_tmp296=_tmp295;void*_tmp2A3;struct Cyc_List_List*_tmp2A2;_LL8: _tmp2A3=(void*)_tmp296->hd;_tmp2A2=_tmp296->tl;_LL9:;{
struct Cyc_List_List*_tmp297=_tmp2A7;struct Cyc_List_List*_tmp298=_tmp297;struct Cyc_List_List*_tmp2A1;struct Cyc_List_List*_tmp2A0;_LLB: _tmp2A1=(struct Cyc_List_List*)_tmp298->hd;_tmp2A0=_tmp298->tl;_LLC:;{
struct Cyc_List_List*_tmp299=_tmp2A6;struct Cyc_List_List*_tmp29A=_tmp299;void*_tmp29F;struct Cyc_List_List*_tmp29E;_LLE: _tmp29F=(void*)_tmp29A->hd;_tmp29E=_tmp29A->tl;_LLF:;{
struct _tuple20*_tmp29B=({struct _tuple20*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->f1=_tmp2A2,_tmp29D->f2=_tmp2A0,_tmp29D->f3=_tmp29E;_tmp29D;});
return({void*_tmp570=_tmp2A3;struct Cyc_List_List*_tmp56F=_tmp2A1;void*_tmp56E=_tmp29F;struct Cyc_List_List*_tmp56D=ctx;struct Cyc_List_List*_tmp56C=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->hd=_tmp29B,_tmp29C->tl=_tmp2A5;_tmp29C;});struct Cyc_Tcpat_Rhs*_tmp56B=right_hand_side;Cyc_Tcpat_match(_tmp570,_tmp56F,_tmp56E,_tmp56D,_tmp56C,_tmp56B,rules);});};};};};}}_LL0:;}
# 1439
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp2A9=dsc;struct Cyc_List_List*_tmp2AE;struct Cyc_Set_Set*_tmp2AD;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A9)->tag == 1U){_LL1: _tmp2AD=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A9)->f1;_LL2: {
# 1445
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->tag=1U,({struct Cyc_Set_Set*_tmp571=Cyc_Tcpat_empty_con_set();_tmp2AC->f1=_tmp571;});_tmp2AC;});
struct Cyc_List_List*_tmp2AA=0;
{int i=0;for(0;i < pcon->arity;++ i){
({struct Cyc_List_List*_tmp572=({struct Cyc_List_List*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->hd=any,_tmp2AB->tl=_tmp2AA;_tmp2AB;});_tmp2AA=_tmp572;});}}
return _tmp2AA;}}else{_LL3: _tmp2AE=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A9)->f2;_LL4:
 return _tmp2AE;}_LL0:;}
# 1454
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2AF=pw;struct Cyc_Absyn_Pat*_tmp2CC;if((_tmp2AF.where_clause).tag == 2){_LL1: _LL2:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL3: _tmp2CC=(_tmp2AF.pattern).val;_LL4: {
# 1458
void*_tmp2B0=_tmp2CC->r;void*_tmp2B1=_tmp2B0;union Cyc_Absyn_AggrInfoU _tmp2CB;struct Cyc_List_List*_tmp2CA;struct Cyc_Absyn_Datatypedecl*_tmp2C9;struct Cyc_Absyn_Datatypefield*_tmp2C8;switch(*((int*)_tmp2B1)){case 6U: _LL6: _LL7:
# 1460
 if(i != 0)
({void*_tmp2B2=0U;({struct _dyneither_ptr _tmp574=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2B5=({struct Cyc_Int_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=1U,_tmp3C2.f1=(unsigned long)i;_tmp3C2;});void*_tmp2B3[1U];_tmp2B3[0]=& _tmp2B5;({struct _dyneither_ptr _tmp573=({const char*_tmp2B4="get_access on pointer pattern with offset %d\n";_tag_dyneither(_tmp2B4,sizeof(char),46U);});Cyc_aprintf(_tmp573,_tag_dyneither(_tmp2B3,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp574,_tag_dyneither(_tmp2B2,sizeof(void*),0U));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5U: _LL8: _LL9:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->tag=2U,_tmp2B6->f1=(unsigned int)i;_tmp2B6;});case 8U: _LLA: _tmp2C9=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2B1)->f1;_tmp2C8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2B1)->f2;_LLB:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->tag=3U,_tmp2B7->f1=_tmp2C9,_tmp2B7->f2=_tmp2C8,_tmp2B7->f3=(unsigned int)i;_tmp2B7;});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2B1)->f1 != 0){_LLC: _tmp2CB=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2B1)->f1)->aggr_info;_tmp2CA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2B1)->f3;_LLD: {
# 1466
struct Cyc_Absyn_Aggrdecl*_tmp2B8=Cyc_Absyn_get_known_aggrdecl(_tmp2CB);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B8->impl))->tagged){
struct Cyc_List_List*_tmp2B9=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp2CA))->hd)).f1;struct Cyc_List_List*_tmp2BA=_tmp2B9;struct _dyneither_ptr*_tmp2C0;if(_tmp2BA != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp2BA)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp2BA)->tl == 0){_LL11: _tmp2C0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2BA->hd)->f1;_LL12:
# 1470
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->tag=4U,_tmp2BB->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B8->impl))->tagged,_tmp2BB->f2=_tmp2C0;_tmp2BB;});}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _LL14:
({void*_tmp2BC=0U;({struct _dyneither_ptr _tmp577=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=0U,({struct _dyneither_ptr _tmp575=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp2CC));_tmp3C3.f1=_tmp575;});_tmp3C3;});void*_tmp2BD[1U];_tmp2BD[0]=& _tmp2BF;({struct _dyneither_ptr _tmp576=({const char*_tmp2BE="get_access on bad aggr pattern: %s";_tag_dyneither(_tmp2BE,sizeof(char),35U);});Cyc_aprintf(_tmp576,_tag_dyneither(_tmp2BD,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp577,_tag_dyneither(_tmp2BC,sizeof(void*),0U));});});}_LL10:;}{
# 1474
struct Cyc_List_List*_tmp2C1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B8->impl))->fields;
int _tmp2C2=i;
for(0;i != 0;-- i){_tmp2C1=((struct Cyc_List_List*)_check_null(_tmp2C1))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));_tmp2C3->tag=4U,_tmp2C3->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B8->impl))->tagged,_tmp2C3->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2C1))->hd)->name;_tmp2C3;});};}}else{goto _LLE;}default: _LLE: _LLF:
({void*_tmp2C4=0U;({struct _dyneither_ptr _tmp57A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C7=({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0U,({struct _dyneither_ptr _tmp578=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp2CC));_tmp3C4.f1=_tmp578;});_tmp3C4;});void*_tmp2C5[1U];_tmp2C5[0]=& _tmp2C7;({struct _dyneither_ptr _tmp579=({const char*_tmp2C6="get_access on bad pattern: %s";_tag_dyneither(_tmp2C6,sizeof(char),30U);});Cyc_aprintf(_tmp579,_tag_dyneither(_tmp2C5,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp57A,_tag_dyneither(_tmp2C4,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1484
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp2CD=env;struct Cyc_List_List*_tmp2D1;struct Cyc_Tcpat_Con_s*_tmp2D0;_LL1: _tmp2D1=_tmp2CD->f1;_tmp2D0=_tmp2CD->f2;_LL2:;{
void*acc=Cyc_Tcpat_get_access(_tmp2D0->orig_pat,i);
return({struct Cyc_List_List*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));({struct Cyc_Tcpat_PathNode*_tmp57B=({struct Cyc_Tcpat_PathNode*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->orig_pat=_tmp2D0->orig_pat,_tmp2CE->access=acc;_tmp2CE;});_tmp2CF->hd=_tmp57B;}),_tmp2CF->tl=_tmp2D1;_tmp2CF;});};}
# 1489
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp2D2=({struct _tuple23 _tmp3C5;_tmp3C5.f1=obj,_tmp3C5.f2=pcon;_tmp3C5;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp2D2);}
# 1497
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1501
void*_tmp2D3=p;struct Cyc_Tcpat_Con_s*_tmp2E1;struct Cyc_List_List*_tmp2E0;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2D3)->tag == 0U){_LL1: _LL2:
# 1503
 return({struct Cyc_List_List*_tmp57E=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp57D=work;struct Cyc_Tcpat_Rhs*_tmp57C=right_hand_side;Cyc_Tcpat_and_match(_tmp57E,_tmp57D,_tmp57C,rules);});}else{_LL3: _tmp2E1=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2D3)->f1;_tmp2E0=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2D3)->f2;_LL4: {
# 1505
enum Cyc_Tcpat_Answer _tmp2D4=Cyc_Tcpat_static_match(_tmp2E1,dsc);enum Cyc_Tcpat_Answer _tmp2D5=_tmp2D4;switch(_tmp2D5){case Cyc_Tcpat_Yes: _LL6: _LL7: {
# 1507
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));({struct _tuple21*_tmp57F=({struct _tuple21*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8->f1=_tmp2E1,_tmp2D8->f2=0;_tmp2D8;});_tmp2D9->hd=_tmp57F;}),_tmp2D9->tl=ctx;_tmp2D9;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->f1=_tmp2E0,({struct Cyc_List_List*_tmp581=Cyc_Tcpat_getoargs(_tmp2E1,obj);_tmp2D7->f2=_tmp581;}),({
struct Cyc_List_List*_tmp580=Cyc_Tcpat_getdargs(_tmp2E1,dsc);_tmp2D7->f3=_tmp580;});_tmp2D7;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->hd=work_frame,_tmp2D6->tl=work;_tmp2D6;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL8: _LL9:
# 1513
 return({void*_tmp582=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp582,rules);});default: _LLA: _LLB: {
# 1515
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));({struct _tuple21*_tmp583=({struct _tuple21*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));_tmp2DE->f1=_tmp2E1,_tmp2DE->f2=0;_tmp2DE;});_tmp2DF->hd=_tmp583;}),_tmp2DF->tl=ctx;_tmp2DF;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->f1=_tmp2E0,({struct Cyc_List_List*_tmp585=Cyc_Tcpat_getoargs(_tmp2E1,obj);_tmp2DD->f2=_tmp585;}),({
struct Cyc_List_List*_tmp584=Cyc_Tcpat_getdargs(_tmp2E1,dsc);_tmp2DD->f3=_tmp584;});_tmp2DD;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));_tmp2DC->hd=work_frame,_tmp2DC->tl=work;_tmp2DC;});
void*_tmp2DA=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp2DB=({void*_tmp588=({struct Cyc_List_List*_tmp587=ctx;void*_tmp586=Cyc_Tcpat_add_neg(dsc,_tmp2E1);Cyc_Tcpat_build_desc(_tmp587,_tmp586,work);});Cyc_Tcpat_or_match(_tmp588,rules);});
# 1522
return Cyc_Tcpat_ifeq(obj,_tmp2E1,_tmp2DA,_tmp2DB);}}_LL5:;}}_LL0:;}
# 1532
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1537
void*_tmp2E2=d;struct Cyc_List_List*_tmp2E9;void*_tmp2E8;struct Cyc_Tcpat_Rhs*_tmp2E7;void*_tmp2E6;switch(*((int*)_tmp2E2)){case 0U: _LL1: _tmp2E6=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2E2)->f1;_LL2:
# 1539
 if(!(*exhaust_done)){not_exhaust(env1,_tmp2E6);*exhaust_done=1;}
goto _LL0;case 1U: _LL3: _tmp2E7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2E2)->f1;_LL4:
 rhs_appears(env2,_tmp2E7);goto _LL0;default: _LL5: _tmp2E9=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2E2)->f2;_tmp2E8=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2E2)->f3;_LL6:
# 1543
 for(0;_tmp2E9 != 0;_tmp2E9=_tmp2E9->tl){
struct _tuple0 _tmp2E3=*((struct _tuple0*)_tmp2E9->hd);struct _tuple0 _tmp2E4=_tmp2E3;void*_tmp2E5;_LL8: _tmp2E5=_tmp2E4.f2;_LL9:;
Cyc_Tcpat_check_exhaust_overlap(_tmp2E5,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1548
Cyc_Tcpat_check_exhaust_overlap(_tmp2E8,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1550
goto _LL0;}_LL0:;}
# 1558
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1560
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));_tmp2EF->used=0,_tmp2EF->pat_loc=(swc->pattern)->loc,_tmp2EF->rhs=swc->body;_tmp2EF;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp2EA=({union Cyc_Tcpat_PatOrWhere _tmp3C6;(_tmp3C6.where_clause).tag=2U,(_tmp3C6.where_clause).val=swc->where_clause;_tmp3C6;});
({void*_tmp58B=({struct Cyc_List_List*_tmp58A=({void*_tmp2EB[2U];_tmp2EB[0]=sp0,({void*_tmp589=Cyc_Tcpat_int_pat(*ctr,_tmp2EA);_tmp2EB[1]=_tmp589;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2EB,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp58A,_tmp2EA);});sp=_tmp58B;});
*ctr=*ctr + 1;}else{
# 1568
({void*_tmp58E=({struct Cyc_List_List*_tmp58D=({void*_tmp2EC[2U];_tmp2EC[0]=sp0,({void*_tmp58C=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->tag=0U;_tmp2ED;});_tmp2EC[1]=_tmp58C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2EC,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp58D,({union Cyc_Tcpat_PatOrWhere _tmp3C7;(_tmp3C7.where_clause).tag=2U,(_tmp3C7.where_clause).val=0;_tmp3C7;}));});sp=_tmp58E;});}
return({struct _tuple22*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->f1=sp,_tmp2EE->f2=rhs;_tmp2EE;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1574
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1576
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp2F0=d;struct Cyc_Set_Set*_tmp351;struct Cyc_Tcpat_Con_s*_tmp350;struct Cyc_List_List*_tmp34F;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F0)->tag == 0U){_LL1: _tmp350=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F0)->f1;_tmp34F=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F0)->f2;_LL2: {
# 1580
union Cyc_Tcpat_Name_value _tmp2F1=_tmp350->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp2F2=_tmp350->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp2F3=_tmp2F2;struct Cyc_Absyn_Pat*_tmp2F4;if((_tmp2F3.where_clause).tag == 2){_LL6: _LL7:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp34F))->hd);}else{_LL8: _tmp2F4=(_tmp2F3.pattern).val;_LL9:
 p=_tmp2F4;goto _LL5;}_LL5:;}{
# 1586
void*_tmp2F5=p->r;void*_tmp2F6=_tmp2F5;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Enumfield*_tmp32A;struct Cyc_Absyn_Enumfield*_tmp329;struct _dyneither_ptr _tmp328;int _tmp327;char _tmp326;int _tmp325;struct Cyc_Absyn_Datatypefield*_tmp324;struct Cyc_Absyn_Aggrdecl*_tmp323;struct Cyc_List_List*_tmp322;struct Cyc_Absyn_Tvar*_tmp321;struct Cyc_Absyn_Vardecl*_tmp320;struct Cyc_Absyn_Vardecl*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp31E;switch(*((int*)_tmp2F6)){case 0U: _LLB: _LLC:
 return({const char*_tmp2F7="_";_tag_dyneither(_tmp2F7,sizeof(char),2U);});case 1U: _LLD: _tmp31E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1;_LLE:
 return Cyc_Absynpp_qvar2string(_tmp31E->name);case 3U: _LLF: _tmp31F=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1;_LL10:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FA=({struct Cyc_String_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=0U,({struct _dyneither_ptr _tmp58F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31F->name));_tmp3C8.f1=_tmp58F;});_tmp3C8;});void*_tmp2F8[1U];_tmp2F8[0]=& _tmp2FA;({struct _dyneither_ptr _tmp590=({const char*_tmp2F9="*%s";_tag_dyneither(_tmp2F9,sizeof(char),4U);});Cyc_aprintf(_tmp590,_tag_dyneither(_tmp2F8,sizeof(void*),1U));});});case 4U: _LL11: _tmp321=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1;_tmp320=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2F6)->f2;_LL12:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FD=({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0U,({struct _dyneither_ptr _tmp591=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp320->name));_tmp3CA.f1=_tmp591;});_tmp3CA;});struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0U,_tmp3C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp321->name);_tmp3C9;});void*_tmp2FB[2U];_tmp2FB[0]=& _tmp2FD,_tmp2FB[1]=& _tmp2FE;({struct _dyneither_ptr _tmp592=({const char*_tmp2FC="%s<`%s>";_tag_dyneither(_tmp2FC,sizeof(char),8U);});Cyc_aprintf(_tmp592,_tag_dyneither(_tmp2FB,sizeof(void*),2U));});});case 5U: _LL13: _LL14:
# 1592
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp301=({struct Cyc_String_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=0U,({struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp34F));_tmp3CB.f1=_tmp593;});_tmp3CB;});void*_tmp2FF[1U];_tmp2FF[0]=& _tmp301;({struct _dyneither_ptr _tmp594=({const char*_tmp300="$(%s)";_tag_dyneither(_tmp300,sizeof(char),6U);});Cyc_aprintf(_tmp594,_tag_dyneither(_tmp2FF,sizeof(void*),1U));});});case 6U: _LL15: _LL16:
# 1594
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp304=({struct Cyc_String_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=0U,({struct _dyneither_ptr _tmp595=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp34F));_tmp3CC.f1=_tmp595;});_tmp3CC;});void*_tmp302[1U];_tmp302[0]=& _tmp304;({struct _dyneither_ptr _tmp596=({const char*_tmp303="&%s";_tag_dyneither(_tmp303,sizeof(char),4U);});Cyc_aprintf(_tmp596,_tag_dyneither(_tmp302,sizeof(void*),1U));});});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1)->aggr_info).KnownAggr).tag == 2){_LL17: _tmp323=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1)->aggr_info).KnownAggr).val;_tmp322=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2F6)->f3;_LL18:
# 1596
 if(_tmp323->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp305=_tmp322;struct _dyneither_ptr*_tmp30B;if(_tmp305 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp305)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp305)->hd)->f1)->hd)->tag == 1U){_LL2C: _tmp30B=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp305->hd)->f1)->hd)->f1;_LL2D:
# 1599
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0U,({struct _dyneither_ptr _tmp597=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp323->name));_tmp3CF.f1=_tmp597;});_tmp3CF;});struct Cyc_String_pa_PrintArg_struct _tmp309=({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0U,_tmp3CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp30B);_tmp3CE;});struct Cyc_String_pa_PrintArg_struct _tmp30A=({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0U,({
struct _dyneither_ptr _tmp598=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp34F));_tmp3CD.f1=_tmp598;});_tmp3CD;});void*_tmp306[3U];_tmp306[0]=& _tmp308,_tmp306[1]=& _tmp309,_tmp306[2]=& _tmp30A;({struct _dyneither_ptr _tmp599=({const char*_tmp307="%s{.%s = %s}";_tag_dyneither(_tmp307,sizeof(char),13U);});Cyc_aprintf(_tmp599,_tag_dyneither(_tmp306,sizeof(void*),3U));});});}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 1604
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30E=({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0U,({struct _dyneither_ptr _tmp59A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp323->name));_tmp3D1.f1=_tmp59A;});_tmp3D1;});struct Cyc_String_pa_PrintArg_struct _tmp30F=({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0U,({struct _dyneither_ptr _tmp59B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp34F));_tmp3D0.f1=_tmp59B;});_tmp3D0;});void*_tmp30C[2U];_tmp30C[0]=& _tmp30E,_tmp30C[1]=& _tmp30F;({struct _dyneither_ptr _tmp59C=({const char*_tmp30D="%s{%s}";_tag_dyneither(_tmp30D,sizeof(char),7U);});Cyc_aprintf(_tmp59C,_tag_dyneither(_tmp30C,sizeof(void*),2U));});});}else{goto _LL29;}}else{goto _LL29;}case 8U: _LL19: _tmp324=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2F6)->f2;_LL1A:
# 1606
 if(_tmp34F == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp312=({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0U,({struct _dyneither_ptr _tmp59D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp324->name));_tmp3D2.f1=_tmp59D;});_tmp3D2;});void*_tmp310[1U];_tmp310[0]=& _tmp312;({struct _dyneither_ptr _tmp59E=({const char*_tmp311="%s";_tag_dyneither(_tmp311,sizeof(char),3U);});Cyc_aprintf(_tmp59E,_tag_dyneither(_tmp310,sizeof(void*),1U));});});else{
# 1609
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp315=({struct Cyc_String_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=0U,({struct _dyneither_ptr _tmp59F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp324->name));_tmp3D4.f1=_tmp59F;});_tmp3D4;});struct Cyc_String_pa_PrintArg_struct _tmp316=({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0U,({struct _dyneither_ptr _tmp5A0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp34F));_tmp3D3.f1=_tmp5A0;});_tmp3D3;});void*_tmp313[2U];_tmp313[0]=& _tmp315,_tmp313[1]=& _tmp316;({struct _dyneither_ptr _tmp5A1=({const char*_tmp314="%s(%s)";_tag_dyneither(_tmp314,sizeof(char),7U);});Cyc_aprintf(_tmp5A1,_tag_dyneither(_tmp313,sizeof(void*),2U));});});}case 9U: _LL1B: _LL1C:
 return({const char*_tmp317="NULL";_tag_dyneither(_tmp317,sizeof(char),5U);});case 10U: _LL1D: _tmp325=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2F6)->f2;_LL1E:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp31A=({struct Cyc_Int_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=1U,_tmp3D5.f1=(unsigned long)_tmp325;_tmp3D5;});void*_tmp318[1U];_tmp318[0]=& _tmp31A;({struct _dyneither_ptr _tmp5A2=({const char*_tmp319="%d";_tag_dyneither(_tmp319,sizeof(char),3U);});Cyc_aprintf(_tmp5A2,_tag_dyneither(_tmp318,sizeof(void*),1U));});});case 11U: _LL1F: _tmp326=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1;_LL20:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp31D=({struct Cyc_Int_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=1U,_tmp3D6.f1=(unsigned long)((int)_tmp326);_tmp3D6;});void*_tmp31B[1U];_tmp31B[0]=& _tmp31D;({struct _dyneither_ptr _tmp5A3=({const char*_tmp31C="%d";_tag_dyneither(_tmp31C,sizeof(char),3U);});Cyc_aprintf(_tmp5A3,_tag_dyneither(_tmp31B,sizeof(void*),1U));});});case 12U: _LL21: _tmp328=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1;_tmp327=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2F6)->f2;_LL22:
 return _tmp328;case 13U: _LL23: _tmp329=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2F6)->f2;_LL24:
 _tmp32A=_tmp329;goto _LL26;case 14U: _LL25: _tmp32A=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2F6)->f2;_LL26:
 return Cyc_Absynpp_qvar2string(_tmp32A->name);case 17U: _LL27: _tmp32B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2F6)->f1;_LL28:
 return Cyc_Absynpp_exp2string(_tmp32B);default: _LL29: _LL2A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LLA:;};}}else{_LL3: _tmp351=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F0)->f1;_LL4:
# 1620
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp351))return({const char*_tmp32C="_";_tag_dyneither(_tmp32C,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp32D=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp351);
# 1624
if((int)(((_tmp32D->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp32E=({union Cyc_Tcpat_PatOrWhere _tmp34E=_tmp32D->orig_pat;if((_tmp34E.pattern).tag != 1)_throw_match();(_tmp34E.pattern).val;});
void*_tmp32F=Cyc_Tcutil_compress((void*)_check_null(_tmp32E->topt));void*_tmp330=_tmp32F;struct Cyc_Absyn_Aggrdecl*_tmp34D;struct Cyc_Absyn_Datatypedecl*_tmp34C;struct Cyc_Absyn_PtrAtts _tmp34B;switch(*((int*)_tmp330)){case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp330)->f2 == Cyc_Absyn_Char_sz){_LL31: _LL32:
# 1629
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp331=Cyc_Tcpat_char_con((char)i,_tmp32E);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp351,_tmp331))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp334=({struct Cyc_Int_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=1U,_tmp3D7.f1=(unsigned long)i;_tmp3D7;});void*_tmp332[1U];_tmp332[0]=& _tmp334;({struct _dyneither_ptr _tmp5A4=({const char*_tmp333="%d";_tag_dyneither(_tmp333,sizeof(char),3U);});Cyc_aprintf(_tmp5A4,_tag_dyneither(_tmp332,sizeof(void*),1U));});});}}
# 1634
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL33: _LL34:
# 1637
{unsigned int i=0U;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp335=Cyc_Tcpat_int_con((int)i,_tmp32D->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp351,_tmp335))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp338=({struct Cyc_Int_pa_PrintArg_struct _tmp3D8;_tmp3D8.tag=1U,_tmp3D8.f1=(unsigned long)((int)i);_tmp3D8;});void*_tmp336[1U];_tmp336[0]=& _tmp338;({struct _dyneither_ptr _tmp5A5=({const char*_tmp337="%d";_tag_dyneither(_tmp337,sizeof(char),3U);});Cyc_aprintf(_tmp5A5,_tag_dyneither(_tmp336,sizeof(void*),1U));});});}}
# 1642
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 5U: _LL35: _tmp34B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp330)->f1).ptr_atts;_LL36: {
# 1644
union Cyc_Absyn_Constraint*_tmp339=_tmp34B.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp339);
if(is_nullable){
if(!({struct Cyc_Set_Set*_tmp5A6=_tmp351;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5A6,Cyc_Tcpat_null_con(_tmp32E));}))
return({const char*_tmp33A="NULL";_tag_dyneither(_tmp33A,sizeof(char),5U);});}
# 1650
return({const char*_tmp33B="&_";_tag_dyneither(_tmp33B,sizeof(char),3U);});}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp330)->f1).datatype_info).KnownDatatype).tag == 2){_LL37: _tmp34C=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp330)->f1).datatype_info).KnownDatatype).val;_LL38:
# 1652
 if(_tmp34C->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp33C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp34C->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp33C);
for(0;(unsigned int)_tmp33C;_tmp33C=_tmp33C->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp33C->hd)->name).f2;
struct Cyc_List_List*_tmp33D=((struct Cyc_Absyn_Datatypefield*)_tmp33C->hd)->typs;
if(!({struct Cyc_Set_Set*_tmp5A8=_tmp351;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5A8,({struct Cyc_Tcpat_Con_s*_tmp33E=_cycalloc(sizeof(*_tmp33E));({union Cyc_Tcpat_Name_value _tmp5A7=Cyc_Tcpat_Name_v(n);_tmp33E->name=_tmp5A7;}),_tmp33E->arity=0,_tmp33E->span=0,_tmp33E->orig_pat=_tmp32D->orig_pat;_tmp33E;}));})){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp33D)== 0)
return n;else{
# 1662
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0U,_tmp3D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);_tmp3D9;});void*_tmp33F[1U];_tmp33F[0]=& _tmp341;({struct _dyneither_ptr _tmp5A9=({const char*_tmp340="%s(...)";_tag_dyneither(_tmp340,sizeof(char),8U);});Cyc_aprintf(_tmp5A9,_tag_dyneither(_tmp33F,sizeof(void*),1U));});});}}}
# 1665
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL3B;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp330)->f1).aggr_info).KnownAggr).tag == 2){_LL39: _tmp34D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp330)->f1).aggr_info).KnownAggr).val;if(_tmp34D->kind == Cyc_Absyn_UnionA){_LL3A: {
# 1667
struct Cyc_List_List*_tmp342=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp34D->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp342);
struct _tuple2*_tmp343=_tmp34D->name;struct _tuple2*_tmp344=_tmp343;struct _dyneither_ptr _tmp34A;_LL3E: _tmp34A=*_tmp344->f2;_LL3F:;
for(0;(unsigned int)_tmp342;_tmp342=_tmp342->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp342->hd)->name;
if(!({struct Cyc_Set_Set*_tmp5AB=_tmp351;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5AB,({struct Cyc_Tcpat_Con_s*_tmp345=_cycalloc(sizeof(*_tmp345));({union Cyc_Tcpat_Name_value _tmp5AA=Cyc_Tcpat_Name_v(n);_tmp345->name=_tmp5AA;}),_tmp345->arity=0,_tmp345->span=0,_tmp345->orig_pat=_tmp32D->orig_pat;_tmp345;}));}))
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp348=({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0U,_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34A);_tmp3DB;});struct Cyc_String_pa_PrintArg_struct _tmp349=({struct Cyc_String_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=0U,_tmp3DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);_tmp3DA;});void*_tmp346[2U];_tmp346[0]=& _tmp348,_tmp346[1]=& _tmp349;({struct _dyneither_ptr _tmp5AC=({const char*_tmp347="%s{.%s = _}";_tag_dyneither(_tmp347,sizeof(char),12U);});Cyc_aprintf(_tmp5AC,_tag_dyneither(_tmp346,sizeof(void*),2U));});});}
# 1675
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL30:;};};}_LL0:;}
# 1680
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dyneither_ptr*_tmp352=_cycalloc(sizeof(*_tmp352));({struct _dyneither_ptr _tmp5AD=Cyc_Tcpat_desc2string(d);*_tmp352=_tmp5AD;});_tmp352;});}
# 1683
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp353=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=({struct _dyneither_ptr*_tmp357=_cycalloc(sizeof(*_tmp357));({struct _dyneither_ptr _tmp5AE=({const char*_tmp356=",";_tag_dyneither(_tmp356,sizeof(char),2U);});*_tmp357=_tmp5AE;});_tmp357;});
{struct Cyc_List_List*_tmp354=_tmp353;for(0;_tmp354 != 0;_tmp354=((struct Cyc_List_List*)_check_null(_tmp354))->tl){
if(_tmp354->tl != 0){
({struct Cyc_List_List*_tmp5AF=({struct Cyc_List_List*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355->hd=comma,_tmp355->tl=_tmp354->tl;_tmp355;});_tmp354->tl=_tmp5AF;});
_tmp354=_tmp354->tl;}}}
# 1692
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp353);}
# 1695
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp358;_push_handler(& _tmp358);{int _tmp35A=0;if(setjmp(_tmp358.handler))_tmp35A=1;if(!_tmp35A){
{struct _dyneither_ptr _tmp35B=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp35E=({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0U,_tmp3DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35B);_tmp3DC;});void*_tmp35C[1U];_tmp35C[0]=& _tmp35E;({unsigned int _tmp5B1=loc;struct _dyneither_ptr _tmp5B0=({const char*_tmp35D="patterns may not be exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp35D,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp5B1,_tmp5B0,_tag_dyneither(_tmp35C,sizeof(void*),1U));});});}
# 1697
;_pop_handler();}else{void*_tmp359=(void*)_exn_thrown;void*_tmp35F=_tmp359;void*_tmp362;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp35F)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1701
({void*_tmp360=0U;({unsigned int _tmp5B3=loc;struct _dyneither_ptr _tmp5B2=({const char*_tmp361="patterns may not be exhaustive.";_tag_dyneither(_tmp361,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp5B3,_tmp5B2,_tag_dyneither(_tmp360,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp362=_tmp35F;_LL4:(int)_rethrow(_tmp362);}_LL0:;}};}
# 1705
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1709
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1715
int _tmp363=0;
int*_tmp364=& _tmp363;
struct Cyc_List_List*_tmp365=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp364,swcs);
void*_tmp366=Cyc_Tcpat_match_compile(_tmp365);
*dopt=_tmp366;{
# 1721
int _tmp367=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp366,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp367);
# 1724
for(0;_tmp365 != 0;_tmp365=_tmp365->tl){
struct _tuple22*_tmp368=(struct _tuple22*)_tmp365->hd;struct _tuple22*_tmp369=_tmp368;int _tmp36D;unsigned int _tmp36C;_LL1: _tmp36D=(_tmp369->f2)->used;_tmp36C=(_tmp369->f2)->pat_loc;_LL2:;
if(!_tmp36D){
({void*_tmp36A=0U;({unsigned int _tmp5B5=_tmp36C;struct _dyneither_ptr _tmp5B4=({const char*_tmp36B="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp36B,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp5B5,_tmp5B4,_tag_dyneither(_tmp36A,sizeof(void*),0U));});});
# 1729
break;}}};}
# 1738
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp36E;_push_handler(& _tmp36E);{int _tmp370=0;if(setjmp(_tmp36E.handler))_tmp370=1;if(!_tmp370){
{struct _dyneither_ptr _tmp371=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp374=({struct Cyc_String_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=0U,_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp371);_tmp3DD;});void*_tmp372[1U];_tmp372[0]=& _tmp374;({unsigned int _tmp5B7=(*pr).f1;struct _dyneither_ptr _tmp5B6=({const char*_tmp373="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp373,sizeof(char),45U);});Cyc_Tcutil_warn(_tmp5B7,_tmp5B6,_tag_dyneither(_tmp372,sizeof(void*),1U));});});}
# 1741
;_pop_handler();}else{void*_tmp36F=(void*)_exn_thrown;void*_tmp375=_tmp36F;void*_tmp378;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp375)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1745
({void*_tmp376=0U;({unsigned int _tmp5B9=(*pr).f1;struct _dyneither_ptr _tmp5B8=({const char*_tmp377="pattern not exhaustive.";_tag_dyneither(_tmp377,sizeof(char),24U);});Cyc_Tcutil_warn(_tmp5B9,_tmp5B8,_tag_dyneither(_tmp376,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp378=_tmp375;_LL4:(int)_rethrow(_tmp378);}_LL0:;}};};}
# 1749
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1753
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F->used=0,_tmp37F->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp5BA=Cyc_Absyn_skip_stmt(0U);_tmp37F->rhs=_tmp5BA;});_tmp37F;});
struct Cyc_List_List*_tmp379=({struct Cyc_List_List*_tmp37E=_cycalloc(sizeof(*_tmp37E));({struct _tuple22*_tmp5BC=({struct _tuple22*_tmp37D=_cycalloc(sizeof(*_tmp37D));({void*_tmp5BB=Cyc_Tcpat_compile_pat(p);_tmp37D->f1=_tmp5BB;}),_tmp37D->f2=rhs;_tmp37D;});_tmp37E->hd=_tmp5BC;}),_tmp37E->tl=0;_tmp37E;});
void*_tmp37A=Cyc_Tcpat_match_compile(_tmp379);
struct _tuple14 _tmp37B=({struct _tuple14 _tmp3DE;_tmp3DE.f1=loc,_tmp3DE.f2=1;_tmp3DE;});
int _tmp37C=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp37A,Cyc_Tcpat_not_exhaust_warn,& _tmp37B,Cyc_Tcpat_dummy_fn,0,& _tmp37C);
# 1761
*dopt=_tmp37A;
return _tmp37B.f2;}
# 1769
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1773
if(swc->where_clause != 0)
({void*_tmp380=0U;({unsigned int _tmp5BE=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;struct _dyneither_ptr _tmp5BD=({const char*_tmp381="&&-clauses not supported in exception handlers.";_tag_dyneither(_tmp381,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp5BE,_tmp5BD,_tag_dyneither(_tmp380,sizeof(void*),0U));});});{
# 1776
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->used=0,_tmp383->pat_loc=(swc->pattern)->loc,_tmp383->rhs=swc->body;_tmp383;});
return({struct _tuple22*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->f1=sp0,_tmp382->f2=rhs;_tmp382;});};}
# 1779
static void Cyc_Tcpat_not_exhaust_err2(unsigned int loc,void*d){;}
# 1782
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1786
struct Cyc_List_List*_tmp384=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp385=Cyc_Tcpat_match_compile(_tmp384);
*dopt=_tmp385;{
int _tmp386=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp385,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp386);
# 1792
for(0;_tmp384 != 0;_tmp384=_tmp384->tl){
# 1794
if(_tmp384->tl == 0)break;{
struct _tuple22*_tmp387=(struct _tuple22*)_tmp384->hd;struct _tuple22*_tmp388=_tmp387;int _tmp38C;unsigned int _tmp38B;_LL1: _tmp38C=(_tmp388->f2)->used;_tmp38B=(_tmp388->f2)->pat_loc;_LL2:;
if(!_tmp38C){
({void*_tmp389=0U;({unsigned int _tmp5C0=_tmp38B;struct _dyneither_ptr _tmp5BF=({const char*_tmp38A="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp38A,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp5C0,_tmp5BF,_tag_dyneither(_tmp389,sizeof(void*),0U));});});
break;}};}};}
# 1803
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp38D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp38D != 0;_tmp38D=_tmp38D->tl){
if((*((struct _tuple15*)_tmp38D->hd)).f1 != 0)
return 1;}}
return 0;}
