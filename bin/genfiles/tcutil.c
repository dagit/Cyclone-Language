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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 525
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 920
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 935
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
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
# 956
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 958
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 962
extern void*Cyc_Absyn_empty_effect;
# 968
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 975
extern void*Cyc_Absyn_bounds_one;
# 980
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
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
int Cyc_Absyn_fntype_att(void*);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1170
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
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
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
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
int Cyc_Tcutil_is_arithmetic_type(void*);
# 67
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 69
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 71
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 75
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 77
extern int Cyc_Tcutil_warn_alias_coerce;
# 80
extern int Cyc_Tcutil_warn_region_coerce;
# 83
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 125
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 130
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 134
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 139
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 141
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 149
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 159
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 163
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 167
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 170
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 174
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 180
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 186
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 213 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 215
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 223
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 226
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 233
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 235
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 237
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 239
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 241
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 255
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 266
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 269
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 274
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple18{int f1;void*f2;};
# 283
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 291
void*Cyc_Tcutil_normalize_effect(void*e);
# 294
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 296
int Cyc_Tcutil_new_tvar_id();
# 298
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 301
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 303
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 306
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 310
int Cyc_Tcutil_bits_only(void*t);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 316
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 319
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 323
int Cyc_Tcutil_supports_default(void*);
# 327
int Cyc_Tcutil_admits_zero(void*t);
# 330
int Cyc_Tcutil_is_noreturn(void*);
# 334
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 338
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 342
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 345
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 358
int Cyc_Tcutil_is_array(void*t);
# 362
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 365
int Cyc_Tcutil_zeroable_type(void*t);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 36
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
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
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 40 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 42
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 46
int Cyc_Tcutil_warn_region_coerce=0;
# 49
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 54
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 58
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 63
if(({struct _dyneither_ptr _tmpAD1=({const char*_tmp0="(qualifiers don't match)";_tag_dyneither(_tmp0,sizeof(char),25U);});Cyc_strcmp(_tmpAD1,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmpA43;_tmpA43.tag=0U,_tmpA43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmpA43;});void*_tmp1[1U];_tmp1[0]=& _tmp3;({struct Cyc___cycFILE*_tmpAD3=Cyc_stderr;struct _dyneither_ptr _tmpAD2=({const char*_tmp2="  %s\n";_tag_dyneither(_tmp2,sizeof(char),6U);});Cyc_fprintf(_tmpAD3,_tmpAD2,_tag_dyneither(_tmp1,sizeof(void*),1U));});});
return;}
# 68
if(({struct _dyneither_ptr _tmpAD4=({const char*_tmp4="(function effects do not match)";_tag_dyneither(_tmp4,sizeof(char),32U);});Cyc_strcmp(_tmpAD4,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp5=Cyc_Absynpp_tc_params_r;
_tmp5.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp5);}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmpA44;_tmpA44.tag=0U,_tmpA44.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmpA44;});void*_tmp6[1U];_tmp6[0]=& _tmp8;({struct Cyc___cycFILE*_tmpAD6=Cyc_stderr;struct _dyneither_ptr _tmpAD5=({const char*_tmp7="  %s";_tag_dyneither(_tmp7,sizeof(char),5U);});Cyc_fprintf(_tmpAD6,_tmpAD5,_tag_dyneither(_tmp6,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp9=0U;({struct Cyc___cycFILE*_tmpAD8=Cyc_stderr;struct _dyneither_ptr _tmpAD7=({const char*_tmpA="\n\t";_tag_dyneither(_tmpA,sizeof(char),3U);});Cyc_fprintf(_tmpAD8,_tmpAD7,_tag_dyneither(_tmp9,sizeof(void*),0U));});});
pos=8;}else{
# 82
({void*_tmpB=0U;({struct Cyc___cycFILE*_tmpADA=Cyc_stderr;struct _dyneither_ptr _tmpAD9=({const char*_tmpC=" ";_tag_dyneither(_tmpC,sizeof(char),2U);});Cyc_fprintf(_tmpADA,_tmpAD9,_tag_dyneither(_tmpB,sizeof(void*),0U));});});
++ pos;}
# 85
({void*_tmpD=0U;({struct Cyc___cycFILE*_tmpADC=Cyc_stderr;struct _dyneither_ptr _tmpADB=({const char*_tmpE="and ";_tag_dyneither(_tmpE,sizeof(char),5U);});Cyc_fprintf(_tmpADC,_tmpADB,_tag_dyneither(_tmpD,sizeof(void*),0U));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmpF=0U;({struct Cyc___cycFILE*_tmpADE=Cyc_stderr;struct _dyneither_ptr _tmpADD=({const char*_tmp10="\n\t";_tag_dyneither(_tmp10,sizeof(char),3U);});Cyc_fprintf(_tmpADE,_tmpADD,_tag_dyneither(_tmpF,sizeof(void*),0U));});});
pos=8;}
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpA45;_tmpA45.tag=0U,_tmpA45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmpA45;});void*_tmp11[1U];_tmp11[0]=& _tmp13;({struct Cyc___cycFILE*_tmpAE0=Cyc_stderr;struct _dyneither_ptr _tmpADF=({const char*_tmp12="%s ";_tag_dyneither(_tmp12,sizeof(char),4U);});Cyc_fprintf(_tmpAE0,_tmpADF,_tag_dyneither(_tmp11,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp14=0U;({struct Cyc___cycFILE*_tmpAE2=Cyc_stderr;struct _dyneither_ptr _tmpAE1=({const char*_tmp15="\n\t";_tag_dyneither(_tmp15,sizeof(char),3U);});Cyc_fprintf(_tmpAE2,_tmpAE1,_tag_dyneither(_tmp14,sizeof(void*),0U));});});
pos=8;}
# 97
({void*_tmp16=0U;({struct Cyc___cycFILE*_tmpAE4=Cyc_stderr;struct _dyneither_ptr _tmpAE3=({const char*_tmp17="are not compatible. ";_tag_dyneither(_tmp17,sizeof(char),21U);});Cyc_fprintf(_tmpAE4,_tmpAE3,_tag_dyneither(_tmp16,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpAE5=(char*)Cyc_Tcutil_failure_reason.curr;_tmpAE5 != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({int _tmpAE6=pos;_tmpAE6 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80)
({void*_tmp18=0U;({struct Cyc___cycFILE*_tmpAE8=Cyc_stderr;struct _dyneither_ptr _tmpAE7=({const char*_tmp19="\n\t";_tag_dyneither(_tmp19,sizeof(char),3U);});Cyc_fprintf(_tmpAE8,_tmpAE7,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmpA46;_tmpA46.tag=0U,_tmpA46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmpA46;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct Cyc___cycFILE*_tmpAEA=Cyc_stderr;struct _dyneither_ptr _tmpAE9=({const char*_tmp1B="%s";_tag_dyneither(_tmp1B,sizeof(char),3U);});Cyc_fprintf(_tmpAEA,_tmpAE9,_tag_dyneither(_tmp1A,sizeof(void*),1U));});});}
# 105
({void*_tmp1D=0U;({struct Cyc___cycFILE*_tmpAEC=Cyc_stderr;struct _dyneither_ptr _tmpAEB=({const char*_tmp1E="\n";_tag_dyneither(_tmp1E,sizeof(char),2U);});Cyc_fprintf(_tmpAEC,_tmpAEB,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 111
Cyc_Position_post_error(({unsigned int _tmpAED=loc;Cyc_Position_mk_err(_tmpAED,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 113
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 116
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
if(!Cyc_Position_error_p()){
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmpA47;_tmpA47.tag=0U,_tmpA47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpA47;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({struct Cyc___cycFILE*_tmpAEF=Cyc_stderr;struct _dyneither_ptr _tmpAEE=({const char*_tmp20="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp20,sizeof(char),36U);});Cyc_fprintf(_tmpAEF,_tmpAEE,_tag_dyneither(_tmp1F,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);}
# 121
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=Cyc_Core_Impossible,_tmp22->f1=msg;_tmp22;}));}
# 123
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 125
Cyc_Warn_vwarn(sg,fmt,ap);}
# 129
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 134
void*Cyc_Tcutil_compress(void*t){
void*_tmp23=t;void*_tmp2C;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;void**_tmp29;void**_tmp28;switch(*((int*)_tmp23)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp28=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23)->f2;_LL8: {
# 145
void*ta=(void*)_check_null(*_tmp28);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp28=t2;
return t2;}}case 17U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23)->f4 == 0){_LL3: _LL4:
# 137
 return t;}else{_LL5: _tmp29=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23)->f4;_LL6: {
# 139
void*ta=(void*)_check_null(*_tmp29);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp29=t2;
return t2;}}case 18U: _LL9: _tmp2A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp23)->f1;_LLA:
# 151
 Cyc_Evexp_eval_const_uint_exp(_tmp2A);{
void*_tmp24=_tmp2A->r;void*_tmp25=_tmp24;void*_tmp26;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp25)->tag == 39U){_LL12: _tmp26=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp25)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp26);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27U: _LLB: _tmp2B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp23)->f1;_LLC: {
# 157
void*_tmp27=_tmp2B->topt;
if(_tmp27 != 0)return _tmp27;else{
return t;}}case 26U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp23)->f2 != 0){_LLD: _tmp2C=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp23)->f2;_LLE:
# 161
 return Cyc_Tcutil_compress(_tmp2C);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 169
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 173
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp2D=cptr;union Cyc_Absyn_Constraint*_tmp2F;void*_tmp2E;switch((((union Cyc_Absyn_Constraint*)_tmp2D)->Eq_constr).tag){case 3U: _LL1: _LL2:
 return Cyc_Absyn_empty_conref();case 1U: _LL3: _tmp2E=(void*)(_tmp2D->Eq_constr).val;_LL4:
 return Cyc_Absyn_new_conref(_tmp2E);default: _LL5: _tmp2F=(_tmp2D->Forward_constr).val;_LL6:
 return Cyc_Tcutil_copy_conref(_tmp2F);}_LL0:;}
# 180
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp30=Cyc_Absyn_compress_kb(kb);void*_tmp31=_tmp30;struct Cyc_Absyn_Kind*_tmp34;switch(*((int*)_tmp31)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=1U,_tmp32->f1=0;_tmp32;});case 2U: _LL3: _tmp34=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp31)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=2U,_tmp33->f1=0,_tmp33->f2=_tmp34;_tmp33;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 187
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 189
return({struct Cyc_Absyn_Tvar*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->name=tv->name,_tmp35->identity=- 1,({void*_tmpAF0=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp35->kind=_tmpAF0;});_tmp35;});}
# 191
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 193
struct _tuple10*_tmp36=arg;struct _dyneither_ptr*_tmp3A;struct Cyc_Absyn_Tqual _tmp39;void*_tmp38;_LL1: _tmp3A=_tmp36->f1;_tmp39=_tmp36->f2;_tmp38=_tmp36->f3;_LL2:;
return({struct _tuple10*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=_tmp3A,_tmp37->f2=_tmp39,({void*_tmpAF1=Cyc_Tcutil_copy_type(_tmp38);_tmp37->f3=_tmpAF1;});_tmp37;});}
# 196
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3B=arg;struct Cyc_Absyn_Tqual _tmp3E;void*_tmp3D;_LL1: _tmp3E=_tmp3B->f1;_tmp3D=_tmp3B->f2;_LL2:;
return({struct _tuple12*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=_tmp3E,({void*_tmpAF2=Cyc_Tcutil_copy_type(_tmp3D);_tmp3C->f2=_tmpAF2;});_tmp3C;});}
# 200
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 202
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=f->name,_tmp3F->tq=f->tq,({void*_tmpAF4=Cyc_Tcutil_copy_type(f->type);_tmp3F->type=_tmpAF4;}),_tmp3F->width=f->width,_tmp3F->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpAF3=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp3F->requires_clause=_tmpAF3;});_tmp3F;});}
# 206
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp40=x;void*_tmp43;void*_tmp42;_LL1: _tmp43=_tmp40->f1;_tmp42=_tmp40->f2;_LL2:;
return({struct _tuple0*_tmp41=_cycalloc(sizeof(*_tmp41));({void*_tmpAF6=Cyc_Tcutil_copy_type(_tmp43);_tmp41->f1=_tmpAF6;}),({void*_tmpAF5=Cyc_Tcutil_copy_type(_tmp42);_tmp41->f2=_tmpAF5;});_tmp41;});}
# 210
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->name=f->name,_tmp44->tag=f->tag,_tmp44->loc=f->loc;_tmp44;});}
# 213
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->tag=2U,({struct Cyc_Absyn_Tvar*_tmpAF7=Cyc_Tcutil_copy_tvar(t);_tmp45->f1=_tmpAF7;});_tmp45;});}
# 217
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp46=Cyc_Tcutil_compress(t);void*_tmp47=_tmp46;struct Cyc_Absyn_Datatypedecl*_tmpAB;struct Cyc_Absyn_Enumdecl*_tmpAA;struct Cyc_Absyn_Aggrdecl*_tmpA9;void*_tmpA8;struct Cyc_List_List*_tmpA7;void*_tmpA6;struct _tuple2*_tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_Absyn_Typedefdecl*_tmpA3;void*_tmpA2;void*_tmpA1;void*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;void*_tmp9D;struct Cyc_List_List*_tmp9C;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9A;enum Cyc_Absyn_AggrKind _tmp99;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Aggrdecl**_tmp97;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp95;struct _tuple2*_tmp94;struct Cyc_Core_Opt*_tmp93;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp90;void*_tmp8F;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8D;struct Cyc_List_List*_tmp8C;int _tmp8B;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_List_List*_tmp84;void*_tmp83;struct Cyc_Absyn_Tqual _tmp82;struct Cyc_Absyn_Exp*_tmp81;union Cyc_Absyn_Constraint*_tmp80;unsigned int _tmp7F;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7D;void*_tmp7C;union Cyc_Absyn_Constraint*_tmp7B;union Cyc_Absyn_Constraint*_tmp7A;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp78;union Cyc_Absyn_DatatypeFieldInfoU _tmp77;struct Cyc_List_List*_tmp76;union Cyc_Absyn_DatatypeInfoU _tmp75;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Tvar*_tmp73;switch(*((int*)_tmp47)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return t;case 2U: _LL5: _tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp47)->f1;_LL6:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=2U,({struct Cyc_Absyn_Tvar*_tmpAF8=Cyc_Tcutil_copy_tvar(_tmp73);_tmp48->f1=_tmpAF8;});_tmp48;});case 3U: _LL7: _tmp75=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47)->f1).datatype_info;_tmp74=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL8:
# 223
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=3U,(_tmp49->f1).datatype_info=_tmp75,({struct Cyc_List_List*_tmpAF9=Cyc_Tcutil_copy_types(_tmp74);(_tmp49->f1).targs=_tmpAF9;});_tmp49;});case 4U: _LL9: _tmp77=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47)->f1).field_info;_tmp76=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47)->f1).targs;_LLA:
# 225
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=4U,(_tmp4A->f1).field_info=_tmp77,({struct Cyc_List_List*_tmpAFA=Cyc_Tcutil_copy_types(_tmp76);(_tmp4A->f1).targs=_tmpAFA;});_tmp4A;});case 5U: _LLB: _tmp7E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).elt_typ;_tmp7D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).elt_tq;_tmp7C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).rgn;_tmp7B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).nullable;_tmp7A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).bounds;_tmp79=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).zero_term;_tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).ptrloc;_LLC: {
# 227
void*_tmp4B=Cyc_Tcutil_copy_type(_tmp7E);
void*_tmp4C=Cyc_Tcutil_copy_type(_tmp7C);
union Cyc_Absyn_Constraint*_tmp4D=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp7B);
struct Cyc_Absyn_Tqual _tmp4E=_tmp7D;
# 232
union Cyc_Absyn_Constraint*_tmp4F=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp7A);
union Cyc_Absyn_Constraint*_tmp50=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=5U,(_tmp51->f1).elt_typ=_tmp4B,(_tmp51->f1).elt_tq=_tmp4E,((_tmp51->f1).ptr_atts).rgn=_tmp4C,((_tmp51->f1).ptr_atts).nullable=_tmp4D,((_tmp51->f1).ptr_atts).bounds=_tmp4F,((_tmp51->f1).ptr_atts).zero_term=_tmp50,((_tmp51->f1).ptr_atts).ptrloc=_tmp78;_tmp51;});}case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return t;case 8U: _LL11: _tmp83=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->f1).elt_type;_tmp82=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->f1).tq;_tmp81=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->f1).num_elts;_tmp80=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->f1).zero_term;_tmp7F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->f1).zt_loc;_LL12: {
# 238
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp81);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=8U,({void*_tmpAFC=Cyc_Tcutil_copy_type(_tmp83);(_tmp52->f1).elt_type=_tmpAFC;}),(_tmp52->f1).tq=_tmp82,(_tmp52->f1).num_elts=eopt2,({
union Cyc_Absyn_Constraint*_tmpAFB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp80);(_tmp52->f1).zero_term=_tmpAFB;}),(_tmp52->f1).zt_loc=_tmp7F;_tmp52;});}case 9U: _LL13: _tmp90=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).tvars;_tmp8F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).effect;_tmp8E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ret_tqual;_tmp8D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ret_typ;_tmp8C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).args;_tmp8B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).c_varargs;_tmp8A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).cyc_varargs;_tmp89=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).rgn_po;_tmp88=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).attributes;_tmp87=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).requires_clause;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).requires_relns;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ensures_clause;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ensures_relns;_LL14: {
# 242
struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp90);
void*effopt2=_tmp8F == 0?0: Cyc_Tcutil_copy_type(_tmp8F);
void*_tmp54=Cyc_Tcutil_copy_type(_tmp8D);
struct Cyc_List_List*_tmp55=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp8C);
int _tmp56=_tmp8B;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
({struct Cyc_Absyn_VarargInfo*_tmpAFE=({struct Cyc_Absyn_VarargInfo*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->name=cv->name,_tmp57->tq=cv->tq,({void*_tmpAFD=Cyc_Tcutil_copy_type(cv->type);_tmp57->type=_tmpAFD;}),_tmp57->inject=cv->inject;_tmp57;});cyc_varargs2=_tmpAFE;});}{
# 253
struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp89);
struct Cyc_List_List*_tmp59=_tmp88;
struct Cyc_Absyn_Exp*_tmp5A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp87);
struct Cyc_List_List*_tmp5B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp86);
struct Cyc_Absyn_Exp*_tmp5C=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp85);
struct Cyc_List_List*_tmp5D=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp84);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=9U,(_tmp5E->f1).tvars=_tmp53,(_tmp5E->f1).effect=effopt2,(_tmp5E->f1).ret_tqual=_tmp8E,(_tmp5E->f1).ret_typ=_tmp54,(_tmp5E->f1).args=_tmp55,(_tmp5E->f1).c_varargs=_tmp56,(_tmp5E->f1).cyc_varargs=cyc_varargs2,(_tmp5E->f1).rgn_po=_tmp58,(_tmp5E->f1).attributes=_tmp59,(_tmp5E->f1).requires_clause=_tmp5A,(_tmp5E->f1).requires_relns=_tmp5B,(_tmp5E->f1).ensures_clause=_tmp5C,(_tmp5E->f1).ensures_relns=_tmp5D;_tmp5E;});};}case 10U: _LL15: _tmp91=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp47)->f1;_LL16:
# 262
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=10U,({struct Cyc_List_List*_tmpAFF=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp91);_tmp5F->f1=_tmpAFF;});_tmp5F;});case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).tag == 1){_LL17: _tmp95=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f1;_tmp94=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f2;_tmp93=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f3;_tmp92=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL18:
# 264
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpB01=Cyc_Absyn_UnknownAggr(_tmp95,_tmp94,_tmp93);(_tmp60->f1).aggr_info=_tmpB01;}),({struct Cyc_List_List*_tmpB00=Cyc_Tcutil_copy_types(_tmp92);(_tmp60->f1).targs=_tmpB00;});_tmp60;});}else{_LL19: _tmp97=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).KnownAggr).val;_tmp96=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL1A:
# 266
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpB03=Cyc_Absyn_KnownAggr(_tmp97);(_tmp61->f1).aggr_info=_tmpB03;}),({struct Cyc_List_List*_tmpB02=Cyc_Tcutil_copy_types(_tmp96);(_tmp61->f1).targs=_tmpB02;});_tmp61;});}case 12U: _LL1B: _tmp99=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47)->f1;_tmp98=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47)->f2;_LL1C:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=12U,_tmp62->f1=_tmp99,({struct Cyc_List_List*_tmpB04=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp98);_tmp62->f2=_tmpB04;});_tmp62;});case 13U: _LL1D: _tmp9B=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47)->f1;_tmp9A=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47)->f2;_LL1E:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=13U,_tmp63->f1=_tmp9B,_tmp63->f2=_tmp9A;_tmp63;});case 14U: _LL1F: _tmp9C=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp47)->f1;_LL20:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=14U,({struct Cyc_List_List*_tmpB05=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9C);_tmp64->f1=_tmpB05;});_tmp64;});case 19U: _LL21: _tmp9D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp47)->f1;_LL22:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=19U,({void*_tmpB06=Cyc_Tcutil_copy_type(_tmp9D);_tmp65->f1=_tmpB06;});_tmp65;});case 18U: _LL23: _tmp9E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp47)->f1;_LL24:
# 273
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=18U,_tmp66->f1=_tmp9E;_tmp66;});case 27U: _LL25: _tmp9F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp47)->f1;_LL26:
# 276
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=27U,_tmp67->f1=_tmp9F;_tmp67;});case 15U: _LL27: _tmpA0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp47)->f1;_LL28:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=15U,({void*_tmpB07=Cyc_Tcutil_copy_type(_tmpA0);_tmp68->f1=_tmpB07;});_tmp68;});case 16U: _LL29: _tmpA2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47)->f1;_tmpA1=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47)->f2;_LL2A:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=16U,({void*_tmpB09=Cyc_Tcutil_copy_type(_tmpA2);_tmp69->f1=_tmpB09;}),({void*_tmpB08=Cyc_Tcutil_copy_type(_tmpA1);_tmp69->f2=_tmpB08;});_tmp69;});case 17U: _LL2B: _tmpA5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f1;_tmpA4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f2;_tmpA3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f3;_LL2C:
# 280
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=17U,_tmp6A->f1=_tmpA5,({struct Cyc_List_List*_tmpB0A=Cyc_Tcutil_copy_types(_tmpA4);_tmp6A->f2=_tmpB0A;}),_tmp6A->f3=_tmpA3,_tmp6A->f4=0;_tmp6A;});case 21U: _LL2D: _LL2E:
 goto _LL30;case 22U: _LL2F: _LL30:
 goto _LL32;case 28U: _LL31: _LL32:
 goto _LL34;case 20U: _LL33: _LL34:
 return t;case 23U: _LL35: _tmpA6=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp47)->f1;_LL36:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=23U,({void*_tmpB0B=Cyc_Tcutil_copy_type(_tmpA6);_tmp6B->f1=_tmpB0B;});_tmp6B;});case 24U: _LL37: _tmpA7=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp47)->f1;_LL38:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=24U,({struct Cyc_List_List*_tmpB0C=Cyc_Tcutil_copy_types(_tmpA7);_tmp6C->f1=_tmpB0C;});_tmp6C;});case 25U: _LL39: _tmpA8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp47)->f1;_LL3A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=25U,({void*_tmpB0D=Cyc_Tcutil_copy_type(_tmpA8);_tmp6D->f1=_tmpB0D;});_tmp6D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)){case 0U: _LL3B: _tmpA9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL3C: {
# 290
struct Cyc_List_List*_tmp6E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpA9->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpB0E=Cyc_Absyn_UnknownAggr(_tmpA9->kind,_tmpA9->name,0);(_tmp6F->f1).aggr_info=_tmpB0E;}),(_tmp6F->f1).targs=_tmp6E;_tmp6F;});}case 1U: _LL3D: _tmpAA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL3E:
# 293
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=13U,_tmp70->f1=_tmpAA->name,_tmp70->f2=0;_tmp70;});default: _LL3F: _tmpAB=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL40: {
# 295
struct Cyc_List_List*_tmp71=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAB->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=3U,({union Cyc_Absyn_DatatypeInfoU _tmpB0F=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmpA48;_tmpA48.name=_tmpAB->name,_tmpA48.is_extensible=0;_tmpA48;}));(_tmp72->f1).datatype_info=_tmpB0F;}),(_tmp72->f1).targs=_tmp71;_tmp72;});}}}_LL0:;}
# 310 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpAC=d;struct _dyneither_ptr*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC)->tag == 0U){_LL1: _tmpAE=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=0U,({struct Cyc_Absyn_Exp*_tmpB10=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpAE);_tmpAD->f1=_tmpB10;});_tmpAD;});}else{_LL3: _tmpAF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpAC)->f1;_LL4:
 return d;}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 316
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 318
return({struct _tuple19*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct Cyc_List_List*_tmpB12=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB0->f1=_tmpB12;}),({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB0->f2=_tmpB11;});_tmpB0;});}
# 321
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB1=preserve_types;
{void*_tmpB2=e->r;void*_tmpB3=_tmpB2;int _tmp130;struct _dyneither_ptr _tmp12F;void*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct _dyneither_ptr*_tmp12C;struct Cyc_Core_Opt*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;int _tmp127;struct Cyc_Absyn_Exp*_tmp126;void**_tmp125;struct Cyc_Absyn_Exp*_tmp124;int _tmp123;int _tmp122;void*_tmp121;struct Cyc_Absyn_Enumfield*_tmp120;struct Cyc_Absyn_Enumdecl*_tmp11F;struct Cyc_Absyn_Enumfield*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_Absyn_Datatypedecl*_tmp11C;struct Cyc_Absyn_Datatypefield*_tmp11B;void*_tmp11A;struct Cyc_List_List*_tmp119;struct _tuple2*_tmp118;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp116;struct Cyc_Absyn_Aggrdecl*_tmp115;struct Cyc_Absyn_Exp*_tmp114;void*_tmp113;int _tmp112;struct Cyc_Absyn_Vardecl*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;int _tmp10E;struct Cyc_List_List*_tmp10D;struct _dyneither_ptr*_tmp10C;struct Cyc_Absyn_Tqual _tmp10B;void*_tmp10A;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct _dyneither_ptr*_tmp104;int _tmp103;int _tmp102;struct Cyc_Absyn_Exp*_tmp101;struct _dyneither_ptr*_tmp100;int _tmpFF;int _tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;void*_tmpF4;struct Cyc_Absyn_Exp*_tmpF3;int _tmpF2;enum Cyc_Absyn_Coercion _tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpED;int _tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargCallInfo*_tmpE9;int _tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;enum Cyc_Absyn_Incrementor _tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Core_Opt*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;enum Cyc_Absyn_Primop _tmpD9;struct Cyc_List_List*_tmpD8;struct _dyneither_ptr _tmpD7;void*_tmpD6;union Cyc_Absyn_Cnst _tmpD5;switch(*((int*)_tmpB3)){case 0U: _LL1: _tmpD5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL2:
({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Absyn_const_exp(_tmpD5,e->loc);new_e=_tmpB13;});goto _LL0;case 1U: _LL3: _tmpD6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpB14=Cyc_Absyn_varb_exp(_tmpD6,e->loc);new_e=_tmpB14;});goto _LL0;case 2U: _LL5: _tmpD7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Absyn_pragma_exp(_tmpD7,e->loc);new_e=_tmpB15;});goto _LL0;case 3U: _LL7: _tmpD9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpD8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL8:
({struct Cyc_Absyn_Exp*_tmpB18=({enum Cyc_Absyn_Primop _tmpB17=_tmpD9;struct Cyc_List_List*_tmpB16=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpD8);Cyc_Absyn_primop_exp(_tmpB17,_tmpB16,e->loc);});new_e=_tmpB18;});goto _LL0;case 4U: _LL9: _tmpDC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpDB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpDA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LLA:
# 330
({struct Cyc_Absyn_Exp*_tmpB1C=({struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDC);struct Cyc_Core_Opt*_tmpB1A=(unsigned int)_tmpDB?({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->v=(void*)_tmpDB->v;_tmpB4;}): 0;struct Cyc_Absyn_Exp*_tmpB19=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDA);Cyc_Absyn_assignop_exp(_tmpB1B,_tmpB1A,_tmpB19,e->loc);});new_e=_tmpB1C;});
goto _LL0;case 5U: _LLB: _tmpDE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpDD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LLC:
({struct Cyc_Absyn_Exp*_tmpB1F=({struct Cyc_Absyn_Exp*_tmpB1E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDE);enum Cyc_Absyn_Incrementor _tmpB1D=_tmpDD;Cyc_Absyn_increment_exp(_tmpB1E,_tmpB1D,e->loc);});new_e=_tmpB1F;});goto _LL0;case 6U: _LLD: _tmpE1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpDF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LLE:
# 334
({struct Cyc_Absyn_Exp*_tmpB23=({struct Cyc_Absyn_Exp*_tmpB22=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE1);struct Cyc_Absyn_Exp*_tmpB21=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE0);struct Cyc_Absyn_Exp*_tmpB20=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDF);Cyc_Absyn_conditional_exp(_tmpB22,_tmpB21,_tmpB20,e->loc);});new_e=_tmpB23;});goto _LL0;case 7U: _LLF: _tmpE3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL10:
({struct Cyc_Absyn_Exp*_tmpB26=({struct Cyc_Absyn_Exp*_tmpB25=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE3);struct Cyc_Absyn_Exp*_tmpB24=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE2);Cyc_Absyn_and_exp(_tmpB25,_tmpB24,e->loc);});new_e=_tmpB26;});goto _LL0;case 8U: _LL11: _tmpE5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL12:
({struct Cyc_Absyn_Exp*_tmpB29=({struct Cyc_Absyn_Exp*_tmpB28=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE5);struct Cyc_Absyn_Exp*_tmpB27=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE4);Cyc_Absyn_or_exp(_tmpB28,_tmpB27,e->loc);});new_e=_tmpB29;});goto _LL0;case 9U: _LL13: _tmpE7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL14:
({struct Cyc_Absyn_Exp*_tmpB2C=({struct Cyc_Absyn_Exp*_tmpB2B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE7);struct Cyc_Absyn_Exp*_tmpB2A=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE6);Cyc_Absyn_seq_exp(_tmpB2B,_tmpB2A,e->loc);});new_e=_tmpB2C;});goto _LL0;case 10U: _LL15: _tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpE9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpE8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL16:
# 339
{struct Cyc_Absyn_VarargCallInfo*_tmpB5=_tmpE9;int _tmpC1;struct Cyc_List_List*_tmpC0;struct Cyc_Absyn_VarargInfo*_tmpBF;if(_tmpB5 != 0){_LL56: _tmpC1=_tmpB5->num_varargs;_tmpC0=_tmpB5->injectors;_tmpBF=_tmpB5->vai;_LL57: {
# 341
struct Cyc_Absyn_VarargInfo*_tmpB6=_tmpBF;struct _dyneither_ptr*_tmpBD;struct Cyc_Absyn_Tqual _tmpBC;void*_tmpBB;int _tmpBA;_LL5B: _tmpBD=_tmpB6->name;_tmpBC=_tmpB6->tq;_tmpBB=_tmpB6->type;_tmpBA=_tmpB6->inject;_LL5C:;
({struct Cyc_Absyn_Exp*_tmpB33=({void*_tmpB32=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->tag=10U,({
struct Cyc_Absyn_Exp*_tmpB31=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEB);_tmpB9->f1=_tmpB31;}),({struct Cyc_List_List*_tmpB30=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpEA);_tmpB9->f2=_tmpB30;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpB2F=({struct Cyc_Absyn_VarargCallInfo*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->num_varargs=_tmpC1,_tmpB8->injectors=_tmpC0,({
struct Cyc_Absyn_VarargInfo*_tmpB2E=({struct Cyc_Absyn_VarargInfo*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->name=_tmpBD,_tmpB7->tq=_tmpBC,({void*_tmpB2D=Cyc_Tcutil_copy_type(_tmpBB);_tmpB7->type=_tmpB2D;}),_tmpB7->inject=_tmpBA;_tmpB7;});_tmpB8->vai=_tmpB2E;});_tmpB8;});
# 344
_tmpB9->f3=_tmpB2F;}),_tmpB9->f4=_tmpE8;_tmpB9;});
# 342
Cyc_Absyn_new_exp(_tmpB32,e->loc);});new_e=_tmpB33;});
# 347
goto _LL55;}}else{_LL58: _LL59:
# 349
({struct Cyc_Absyn_Exp*_tmpB37=({void*_tmpB36=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=10U,({struct Cyc_Absyn_Exp*_tmpB35=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEB);_tmpBE->f1=_tmpB35;}),({struct Cyc_List_List*_tmpB34=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpEA);_tmpBE->f2=_tmpB34;}),_tmpBE->f3=_tmpE9,_tmpBE->f4=_tmpE8;_tmpBE;});Cyc_Absyn_new_exp(_tmpB36,e->loc);});new_e=_tmpB37;});
goto _LL55;}_LL55:;}
# 352
goto _LL0;case 11U: _LL17: _tmpED=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL18:
# 354
({struct Cyc_Absyn_Exp*_tmpB3A=_tmpEC?({struct Cyc_Absyn_Exp*_tmpB39=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpED);Cyc_Absyn_rethrow_exp(_tmpB39,e->loc);}):({struct Cyc_Absyn_Exp*_tmpB38=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpED);Cyc_Absyn_throw_exp(_tmpB38,e->loc);});new_e=_tmpB3A;});
goto _LL0;case 12U: _LL19: _tmpEE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL1A:
({struct Cyc_Absyn_Exp*_tmpB3C=({struct Cyc_Absyn_Exp*_tmpB3B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEE);Cyc_Absyn_noinstantiate_exp(_tmpB3B,e->loc);});new_e=_tmpB3C;});
goto _LL0;case 13U: _LL1B: _tmpF0=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL1C:
# 359
({struct Cyc_Absyn_Exp*_tmpB3F=({struct Cyc_Absyn_Exp*_tmpB3E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF0);struct Cyc_List_List*_tmpB3D=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmpEF);Cyc_Absyn_instantiate_exp(_tmpB3E,_tmpB3D,e->loc);});new_e=_tmpB3F;});
goto _LL0;case 14U: _LL1D: _tmpF4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpF3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpF2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpF1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL1E:
# 362
({struct Cyc_Absyn_Exp*_tmpB44=({void*_tmpB43=Cyc_Tcutil_copy_type(_tmpF4);struct Cyc_Absyn_Exp*_tmpB42=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF3);int _tmpB41=_tmpF2;enum Cyc_Absyn_Coercion _tmpB40=_tmpF1;Cyc_Absyn_cast_exp(_tmpB43,_tmpB42,_tmpB41,_tmpB40,e->loc);});new_e=_tmpB44;});goto _LL0;case 15U: _LL1F: _tmpF5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL20:
({struct Cyc_Absyn_Exp*_tmpB46=({struct Cyc_Absyn_Exp*_tmpB45=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF5);Cyc_Absyn_address_exp(_tmpB45,e->loc);});new_e=_tmpB46;});goto _LL0;case 16U: _LL21: _tmpF7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpF6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL22: {
# 365
struct Cyc_Absyn_Exp*eo1=_tmpF7;if(_tmpF7 != 0)({struct Cyc_Absyn_Exp*_tmpB47=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF7);eo1=_tmpB47;});
({struct Cyc_Absyn_Exp*_tmpB4A=({struct Cyc_Absyn_Exp*_tmpB49=eo1;struct Cyc_Absyn_Exp*_tmpB48=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF6);Cyc_Absyn_New_exp(_tmpB49,_tmpB48,e->loc);});new_e=_tmpB4A;});
goto _LL0;}case 17U: _LL23: _tmpF8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL24:
({struct Cyc_Absyn_Exp*_tmpB4C=({void*_tmpB4B=Cyc_Tcutil_copy_type(_tmpF8);Cyc_Absyn_sizeoftyp_exp(_tmpB4B,e->loc);});new_e=_tmpB4C;});
goto _LL0;case 18U: _LL25: _tmpF9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL26:
({struct Cyc_Absyn_Exp*_tmpB4E=({struct Cyc_Absyn_Exp*_tmpB4D=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF9);Cyc_Absyn_sizeofexp_exp(_tmpB4D,e->loc);});new_e=_tmpB4E;});goto _LL0;case 19U: _LL27: _tmpFB=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpFA=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL28:
# 372
({struct Cyc_Absyn_Exp*_tmpB51=({void*_tmpB50=Cyc_Tcutil_copy_type(_tmpFB);struct Cyc_List_List*_tmpB4F=_tmpFA;Cyc_Absyn_offsetof_exp(_tmpB50,_tmpB4F,e->loc);});new_e=_tmpB51;});goto _LL0;case 20U: _LL29: _tmpFC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL2A:
({struct Cyc_Absyn_Exp*_tmpB53=({struct Cyc_Absyn_Exp*_tmpB52=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpFC);Cyc_Absyn_deref_exp(_tmpB52,e->loc);});new_e=_tmpB53;});goto _LL0;case 41U: _LL2B: _tmpFD=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL2C:
({struct Cyc_Absyn_Exp*_tmpB55=({struct Cyc_Absyn_Exp*_tmpB54=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpFD);Cyc_Absyn_extension_exp(_tmpB54,e->loc);});new_e=_tmpB55;});goto _LL0;case 21U: _LL2D: _tmp101=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp100=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpFF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpFE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL2E:
# 376
({struct Cyc_Absyn_Exp*_tmpB58=({void*_tmpB57=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=21U,({struct Cyc_Absyn_Exp*_tmpB56=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp101);_tmpC2->f1=_tmpB56;}),_tmpC2->f2=_tmp100,_tmpC2->f3=_tmpFF,_tmpC2->f4=_tmpFE;_tmpC2;});Cyc_Absyn_new_exp(_tmpB57,e->loc);});new_e=_tmpB58;});goto _LL0;case 22U: _LL2F: _tmp105=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp104=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp103=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp102=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL30:
# 378
({struct Cyc_Absyn_Exp*_tmpB5B=({void*_tmpB5A=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=22U,({struct Cyc_Absyn_Exp*_tmpB59=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp105);_tmpC3->f1=_tmpB59;}),_tmpC3->f2=_tmp104,_tmpC3->f3=_tmp103,_tmpC3->f4=_tmp102;_tmpC3;});Cyc_Absyn_new_exp(_tmpB5A,e->loc);});new_e=_tmpB5B;});goto _LL0;case 23U: _LL31: _tmp107=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp106=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL32:
({struct Cyc_Absyn_Exp*_tmpB5E=({struct Cyc_Absyn_Exp*_tmpB5D=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp107);struct Cyc_Absyn_Exp*_tmpB5C=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp106);Cyc_Absyn_subscript_exp(_tmpB5D,_tmpB5C,e->loc);});new_e=_tmpB5E;});
goto _LL0;case 24U: _LL33: _tmp108=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL34:
({struct Cyc_Absyn_Exp*_tmpB60=({struct Cyc_List_List*_tmpB5F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmp108);Cyc_Absyn_tuple_exp(_tmpB5F,e->loc);});new_e=_tmpB60;});goto _LL0;case 25U: _LL35: _tmp10C=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f1;_tmp10B=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f2;_tmp10A=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f3;_tmp109=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL36: {
# 383
struct _dyneither_ptr*vopt1=_tmp10C;
if(_tmp10C != 0)vopt1=_tmp10C;
({struct Cyc_Absyn_Exp*_tmpB65=({void*_tmpB64=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->tag=25U,({struct _tuple10*_tmpB63=({struct _tuple10*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->f1=vopt1,_tmpC4->f2=_tmp10B,({void*_tmpB62=Cyc_Tcutil_copy_type(_tmp10A);_tmpC4->f3=_tmpB62;});_tmpC4;});_tmpC5->f1=_tmpB63;}),({
struct Cyc_List_List*_tmpB61=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp109);_tmpC5->f2=_tmpB61;});_tmpC5;});
# 385
Cyc_Absyn_new_exp(_tmpB64,e->loc);});new_e=_tmpB65;});
# 387
goto _LL0;}case 26U: _LL37: _tmp10D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL38:
({struct Cyc_Absyn_Exp*_tmpB68=({void*_tmpB67=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=26U,({struct Cyc_List_List*_tmpB66=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp10D);_tmpC6->f1=_tmpB66;});_tmpC6;});Cyc_Absyn_new_exp(_tmpB67,e->loc);});new_e=_tmpB68;});
goto _LL0;case 27U: _LL39: _tmp111=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp110=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp10F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp10E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL3A:
# 391
({struct Cyc_Absyn_Exp*_tmpB6C=({void*_tmpB6B=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->tag=27U,_tmpC7->f1=_tmp111,({struct Cyc_Absyn_Exp*_tmpB6A=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp110);_tmpC7->f2=_tmpB6A;}),({struct Cyc_Absyn_Exp*_tmpB69=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp10F);_tmpC7->f3=_tmpB69;}),_tmpC7->f4=_tmp10E;_tmpC7;});Cyc_Absyn_new_exp(_tmpB6B,e->loc);});new_e=_tmpB6C;});
goto _LL0;case 28U: _LL3B: _tmp114=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp113=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp112=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LL3C:
# 394
({struct Cyc_Absyn_Exp*_tmpB70=({void*_tmpB6F=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=28U,({struct Cyc_Absyn_Exp*_tmpB6E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp114);_tmpC8->f1=_tmpB6E;}),({void*_tmpB6D=Cyc_Tcutil_copy_type(_tmp113);_tmpC8->f2=_tmpB6D;}),_tmpC8->f3=_tmp112;_tmpC8;});Cyc_Absyn_new_exp(_tmpB6F,_tmp114->loc);});new_e=_tmpB70;});
# 396
goto _LL0;case 29U: _LL3D: _tmp118=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp117=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp116=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp115=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL3E:
# 398
({struct Cyc_Absyn_Exp*_tmpB74=({void*_tmpB73=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=29U,_tmpC9->f1=_tmp118,({struct Cyc_List_List*_tmpB72=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp117);_tmpC9->f2=_tmpB72;}),({struct Cyc_List_List*_tmpB71=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp116);_tmpC9->f3=_tmpB71;}),_tmpC9->f4=_tmp115;_tmpC9;});Cyc_Absyn_new_exp(_tmpB73,e->loc);});new_e=_tmpB74;});
# 400
goto _LL0;case 30U: _LL3F: _tmp11A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp119=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL40:
# 402
({struct Cyc_Absyn_Exp*_tmpB78=({void*_tmpB77=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->tag=30U,({void*_tmpB76=Cyc_Tcutil_copy_type(_tmp11A);_tmpCA->f1=_tmpB76;}),({struct Cyc_List_List*_tmpB75=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp119);_tmpCA->f2=_tmpB75;});_tmpCA;});Cyc_Absyn_new_exp(_tmpB77,e->loc);});new_e=_tmpB78;});
goto _LL0;case 31U: _LL41: _tmp11D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp11B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LL42:
# 405
({struct Cyc_Absyn_Exp*_tmpB7B=({void*_tmpB7A=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=31U,({struct Cyc_List_List*_tmpB79=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmp11D);_tmpCB->f1=_tmpB79;}),_tmpCB->f2=_tmp11C,_tmpCB->f3=_tmp11B;_tmpCB;});Cyc_Absyn_new_exp(_tmpB7A,e->loc);});new_e=_tmpB7B;});
goto _LL0;case 32U: _LL43: _tmp11F=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11E=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL44:
 new_e=e;goto _LL0;case 33U: _LL45: _tmp121=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp120=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL46:
# 409
({struct Cyc_Absyn_Exp*_tmpB7E=({void*_tmpB7D=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->tag=33U,({void*_tmpB7C=Cyc_Tcutil_copy_type(_tmp121);_tmpCC->f1=_tmpB7C;}),_tmpCC->f2=_tmp120;_tmpCC;});Cyc_Absyn_new_exp(_tmpB7D,e->loc);});new_e=_tmpB7E;});
goto _LL0;case 34U: _LL47: _tmp127=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).is_calloc;_tmp126=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).rgn;_tmp125=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).elt_type;_tmp124=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).num_elts;_tmp123=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).fat_result;_tmp122=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).inline_call;_LL48: {
# 412
struct Cyc_Absyn_Exp*_tmpCD=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp126;if(_tmp126 != 0)({struct Cyc_Absyn_Exp*_tmpB7F=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp126);r1=_tmpB7F;});{
void**t1=_tmp125;if(_tmp125 != 0)({void**_tmpB81=({void**_tmpCE=_cycalloc(sizeof(*_tmpCE));({void*_tmpB80=Cyc_Tcutil_copy_type(*_tmp125);*_tmpCE=_tmpB80;});_tmpCE;});t1=_tmpB81;});
({void*_tmpB82=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=34U,(_tmpCF->f1).is_calloc=_tmp127,(_tmpCF->f1).rgn=r1,(_tmpCF->f1).elt_type=t1,(_tmpCF->f1).num_elts=_tmp124,(_tmpCF->f1).fat_result=_tmp123,(_tmpCF->f1).inline_call=_tmp122;_tmpCF;});_tmpCD->r=_tmpB82;});
new_e=_tmpCD;
goto _LL0;};}case 35U: _LL49: _tmp129=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp128=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL4A:
({struct Cyc_Absyn_Exp*_tmpB85=({struct Cyc_Absyn_Exp*_tmpB84=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp129);struct Cyc_Absyn_Exp*_tmpB83=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp128);Cyc_Absyn_swap_exp(_tmpB84,_tmpB83,e->loc);});new_e=_tmpB85;});goto _LL0;case 36U: _LL4B: _tmp12B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp12A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL4C: {
# 420
struct Cyc_Core_Opt*nopt1=_tmp12B;
if(_tmp12B != 0)({struct Cyc_Core_Opt*_tmpB86=({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=(struct _tuple2*)_tmp12B->v;_tmpD0;});nopt1=_tmpB86;});
({struct Cyc_Absyn_Exp*_tmpB89=({void*_tmpB88=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=36U,_tmpD1->f1=nopt1,({struct Cyc_List_List*_tmpB87=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp12A);_tmpD1->f2=_tmpB87;});_tmpD1;});Cyc_Absyn_new_exp(_tmpB88,e->loc);});new_e=_tmpB89;});
goto _LL0;}case 37U: _LL4D: _LL4E:
# 425
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpB8A=({const char*_tmpD2="deep_copy: statement expressions unsupported";_tag_dyneither(_tmpD2,sizeof(char),45U);});_tmpD3->f1=_tmpB8A;});_tmpD3;}));case 38U: _LL4F: _tmp12D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp12C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL50:
({struct Cyc_Absyn_Exp*_tmpB8D=({void*_tmpB8C=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->tag=38U,({struct Cyc_Absyn_Exp*_tmpB8B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp12D);_tmpD4->f1=_tmpB8B;}),_tmpD4->f2=_tmp12C;_tmpD4;});Cyc_Absyn_new_exp(_tmpB8C,e->loc);});new_e=_tmpB8D;});
goto _LL0;case 39U: _LL51: _tmp12E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL52:
({struct Cyc_Absyn_Exp*_tmpB8F=({void*_tmpB8E=Cyc_Tcutil_copy_type(_tmp12E);Cyc_Absyn_valueof_exp(_tmpB8E,e->loc);});new_e=_tmpB8F;});
goto _LL0;default: _LL53: _tmp130=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp12F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL54:
({struct Cyc_Absyn_Exp*_tmpB90=Cyc_Absyn_asm_exp(_tmp130,_tmp12F,e->loc);new_e=_tmpB90;});goto _LL0;}_LL0:;}
# 433
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 437
return new_e;}
# 440
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 453 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp131=ka1;enum Cyc_Absyn_KindQual _tmp13A;enum Cyc_Absyn_AliasQual _tmp139;_LL1: _tmp13A=_tmp131->kind;_tmp139=_tmp131->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp132=ka2;enum Cyc_Absyn_KindQual _tmp138;enum Cyc_Absyn_AliasQual _tmp137;_LL4: _tmp138=_tmp132->kind;_tmp137=_tmp132->aliasqual;_LL5:;
# 457
if(_tmp13A != _tmp138){
struct _tuple20 _tmp133=({struct _tuple20 _tmpA49;_tmpA49.f1=_tmp13A,_tmpA49.f2=_tmp138;_tmpA49;});struct _tuple20 _tmp134=_tmp133;switch(_tmp134.f1){case Cyc_Absyn_BoxKind: switch(_tmp134.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp134.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 466
if(_tmp139 != _tmp137){
struct _tuple21 _tmp135=({struct _tuple21 _tmpA4A;_tmpA4A.f1=_tmp139,_tmpA4A.f2=_tmp137;_tmpA4A;});struct _tuple21 _tmp136=_tmp135;switch(_tmp136.f1){case Cyc_Absyn_Aliasable: if(_tmp136.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp136.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 473
return 1;};}
# 476
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp13B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp13C=_tmp13B;struct Cyc_Absyn_Kind*_tmp13F;struct Cyc_Absyn_Kind*_tmp13E;switch(*((int*)_tmp13C)){case 0U: _LL1: _tmp13E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp13C)->f1;_LL2:
 return _tmp13E;case 2U: _LL3: _tmp13F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp13C)->f2;_LL4:
 return _tmp13F;default: _LL5: _LL6:
# 481
({void*_tmpB91=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->tag=2U,_tmp13D->f1=0,_tmp13D->f2=def;_tmp13D;});tv->kind=_tmpB91;});
return def;}_LL0:;}
# 486
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp140=({struct _tuple0 _tmpA4B;({void*_tmpB93=Cyc_Absyn_compress_kb(kb1);_tmpA4B.f1=_tmpB93;}),({void*_tmpB92=Cyc_Absyn_compress_kb(kb2);_tmpA4B.f2=_tmpB92;});_tmpA4B;});struct _tuple0 _tmp141=_tmp140;struct Cyc_Core_Opt**_tmp156;void*_tmp155;void*_tmp154;struct Cyc_Core_Opt**_tmp153;struct Cyc_Core_Opt**_tmp152;struct Cyc_Absyn_Kind*_tmp151;struct Cyc_Core_Opt**_tmp150;struct Cyc_Absyn_Kind*_tmp14F;struct Cyc_Core_Opt**_tmp14E;struct Cyc_Absyn_Kind*_tmp14D;struct Cyc_Absyn_Kind*_tmp14C;struct Cyc_Absyn_Kind*_tmp14B;struct Cyc_Core_Opt**_tmp14A;struct Cyc_Absyn_Kind*_tmp149;struct Cyc_Absyn_Kind*_tmp148;struct Cyc_Absyn_Kind*_tmp147;switch(*((int*)_tmp141.f1)){case 0U: switch(*((int*)_tmp141.f2)){case 0U: _LL1: _tmp148=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp141.f1)->f1;_tmp147=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp141.f2)->f1;_LL2:
 return _tmp148 == _tmp147;case 2U: _LL5: _tmp14B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp141.f1)->f1;_tmp14A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f2)->f1;_tmp149=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f2)->f2;_LL6:
# 495
 if(Cyc_Tcutil_kind_leq(_tmp14B,_tmp149)){
({struct Cyc_Core_Opt*_tmpB94=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->v=kb1;_tmp143;});*_tmp14A=_tmpB94;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp141.f2)){case 0U: _LL3: _tmp14E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f1)->f1;_tmp14D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f1)->f2;_tmp14C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp141.f2)->f1;_LL4:
# 490
 if(Cyc_Tcutil_kind_leq(_tmp14C,_tmp14D)){
({struct Cyc_Core_Opt*_tmpB95=({struct Cyc_Core_Opt*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->v=kb2;_tmp142;});*_tmp14E=_tmpB95;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp152=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f1)->f1;_tmp151=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f1)->f2;_tmp150=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f2)->f1;_tmp14F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141.f2)->f2;_LL8:
# 500
 if(Cyc_Tcutil_kind_leq(_tmp151,_tmp14F)){
({struct Cyc_Core_Opt*_tmpB96=({struct Cyc_Core_Opt*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->v=kb1;_tmp144;});*_tmp150=_tmpB96;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp14F,_tmp151)){
({struct Cyc_Core_Opt*_tmpB97=({struct Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->v=kb2;_tmp145;});*_tmp152=_tmpB97;});
return 1;}else{
return 0;}}default: _LLB: _tmp154=_tmp141.f1;_tmp153=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp141.f2)->f1;_LLC:
# 509
({struct Cyc_Core_Opt*_tmpB98=({struct Cyc_Core_Opt*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->v=_tmp154;_tmp146;});*_tmp153=_tmpB98;});
return 1;}default: _LL9: _tmp156=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp141.f1)->f1;_tmp155=_tmp141.f2;_LLA:
# 507
 _tmp154=_tmp155;_tmp153=_tmp156;goto _LLC;}_LL0:;}
# 514
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp157=Cyc_Tcutil_compress(t);void*_tmp158=_tmp157;struct Cyc_Absyn_Tvar*_tmp15D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp158)->tag == 2U){_LL1: _tmp15D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp158)->f1;_LL2: {
# 517
void*_tmp159=_tmp15D->kind;
_tmp15D->kind=kb;
return({struct _tuple16 _tmpA4C;_tmpA4C.f1=_tmp15D,_tmpA4C.f2=_tmp159;_tmpA4C;});}}else{_LL3: _LL4:
# 521
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmpA4D;_tmpA4D.tag=0U,({struct _dyneither_ptr _tmpB99=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA4D.f1=_tmpB99;});_tmpA4D;});void*_tmp15A[1U];_tmp15A[0]=& _tmp15C;({struct _dyneither_ptr _tmpB9A=({const char*_tmp15B="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp15B,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB9A,_tag_dyneither(_tmp15A,sizeof(void*),1U));});});}_LL0:;}
# 527
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 529
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 533
struct _RegionHandle _tmp15E=_new_region("temp");struct _RegionHandle*temp=& _tmp15E;_push_region(temp);
{struct Cyc_List_List*_tmp15F=0;
# 536
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp160=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp161=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp162=Cyc_Tcutil_tvar_kind(_tmp160,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp163=_tmp162;switch(((struct Cyc_Absyn_Kind*)_tmp163)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 542
({struct Cyc_List_List*_tmpB9C=({struct Cyc_List_List*_tmp165=_region_malloc(temp,sizeof(*_tmp165));({struct _tuple16*_tmpB9B=({struct _tuple16*_tmp164=_region_malloc(temp,sizeof(*_tmp164));_tmp164->f1=_tmp160,_tmp164->f2=_tmp161;_tmp164;});_tmp165->hd=_tmpB9B;}),_tmp165->tl=_tmp15F;_tmp165;});_tmp15F=_tmpB9C;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 546
if(_tmp15F != 0){
({void*_tmpB9F=({struct _RegionHandle*_tmpB9E=temp;struct Cyc_List_List*_tmpB9D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15F);Cyc_Tcutil_rsubstitute(_tmpB9E,_tmpB9D,field_typ);});field_typ=_tmpB9F;});
({struct Cyc_Absyn_Kind*_tmpBA0=Cyc_Tcutil_typ_kind(field_typ);k=_tmpBA0;});}}
# 534
;_pop_region(temp);}
# 551
return k;}
# 558
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp166=Cyc_Tcutil_compress(t);void*_tmp167=_tmp166;struct Cyc_Absyn_Typedefdecl*_tmp185;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Kind*_tmp183;struct Cyc_Absyn_PtrInfo _tmp182;enum Cyc_Absyn_AggrKind _tmp181;struct Cyc_List_List*_tmp180;struct Cyc_Absyn_AggrdeclImpl*_tmp17F;int _tmp17E;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Datatypedecl*_tmp17C;struct Cyc_Absyn_Datatypefield*_tmp17B;enum Cyc_Absyn_Size_of _tmp17A;struct Cyc_Absyn_Tvar*_tmp179;struct Cyc_Core_Opt*_tmp178;switch(*((int*)_tmp167)){case 1U: _LL1: _tmp178=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp167)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp178))->v;case 2U: _LL3: _tmp179=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp167)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp179,& Cyc_Tcutil_bk);case 0U: _LL5: _LL6:
 return& Cyc_Tcutil_mk;case 6U: _LL7: _tmp17A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp167)->f2;_LL8:
# 564
 return(_tmp17A == Cyc_Absyn_Int_sz  || _tmp17A == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7U: _LL9: _LLA:
# 566
 return& Cyc_Tcutil_mk;case 9U: _LLB: _LLC:
 return& Cyc_Tcutil_ak;case 16U: _LLD: _LLE:
 goto _LL10;case 13U: _LLF: _LL10:
 goto _LL12;case 14U: _LL11: _LL12:
 goto _LL14;case 15U: _LL13: _LL14:
 return& Cyc_Tcutil_bk;case 21U: _LL15: _LL16:
 return& Cyc_Tcutil_urk;case 20U: _LL17: _LL18:
 return& Cyc_Tcutil_rk;case 22U: _LL19: _LL1A:
 return& Cyc_Tcutil_trk;case 3U: _LL1B: _LL1C:
# 577
 return& Cyc_Tcutil_ak;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp167)->f1).field_info).KnownDatatypefield).tag == 2){_LL1D: _tmp17C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp167)->f1).field_info).KnownDatatypefield).val).f1;_tmp17B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp167)->f1).field_info).KnownDatatypefield).val).f2;_LL1E:
# 579
 return& Cyc_Tcutil_mk;}else{_LL1F: _LL20:
# 581
({void*_tmp168=0U;({struct _dyneither_ptr _tmpBA1=({const char*_tmp169="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp169,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA1,_tag_dyneither(_tmp168,sizeof(void*),0U));});});}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp167)->f1).aggr_info).UnknownAggr).tag == 1){_LL21: _LL22:
# 584
 return& Cyc_Tcutil_ak;}else{_LL23: _tmp181=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp167)->f1).aggr_info).KnownAggr).val)->kind;_tmp180=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp167)->f1).aggr_info).KnownAggr).val)->tvs;_tmp17F=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp167)->f1).aggr_info).KnownAggr).val)->impl;_tmp17E=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp167)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp17D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp167)->f1).targs;_LL24:
# 586
 if(_tmp17F == 0){
if(_tmp17E)
return& Cyc_Tcutil_mk;else{
# 590
return& Cyc_Tcutil_ak;}}{
# 592
struct Cyc_List_List*_tmp16A=_tmp17F->fields;
if(_tmp16A == 0)return& Cyc_Tcutil_mk;
# 595
if(_tmp181 == Cyc_Absyn_StructA){
for(0;((struct Cyc_List_List*)_check_null(_tmp16A))->tl != 0;_tmp16A=_tmp16A->tl){;}{
void*_tmp16B=((struct Cyc_Absyn_Aggrfield*)_tmp16A->hd)->type;
struct Cyc_Absyn_Kind*_tmp16C=Cyc_Tcutil_field_kind(_tmp16B,_tmp17D,_tmp180);
if(_tmp16C == & Cyc_Tcutil_ak  || _tmp16C == & Cyc_Tcutil_tak)return _tmp16C;};}else{
# 603
for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
void*_tmp16D=((struct Cyc_Absyn_Aggrfield*)_tmp16A->hd)->type;
struct Cyc_Absyn_Kind*_tmp16E=Cyc_Tcutil_field_kind(_tmp16D,_tmp17D,_tmp180);
if(_tmp16E == & Cyc_Tcutil_ak  || _tmp16E == & Cyc_Tcutil_tak)return _tmp16E;}}
# 609
return& Cyc_Tcutil_mk;};}case 12U: _LL25: _LL26:
 return& Cyc_Tcutil_mk;case 5U: _LL27: _tmp182=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->f1;_LL28: {
# 612
void*_tmp16F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp182.ptr_atts).bounds);void*_tmp170=_tmp16F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp170)->tag == 0U){_LL44: _LL45: {
# 614
enum Cyc_Absyn_AliasQual _tmp171=(Cyc_Tcutil_typ_kind((_tmp182.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp172=_tmp171;switch(_tmp172){case Cyc_Absyn_Aliasable: _LL49: _LL4A:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL4B: _LL4C:
 return& Cyc_Tcutil_umk;default: _LL4D: _LL4E:
 return& Cyc_Tcutil_tmk;}_LL48:;}}else{_LL46: _LL47: {
# 620
enum Cyc_Absyn_AliasQual _tmp173=(Cyc_Tcutil_typ_kind((_tmp182.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp174=_tmp173;switch(_tmp174){case Cyc_Absyn_Aliasable: _LL50: _LL51:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL52: _LL53:
 return& Cyc_Tcutil_ubk;default: _LL54: _LL55:
 return& Cyc_Tcutil_tbk;}_LL4F:;}}_LL43:;}case 18U: _LL29: _LL2A:
# 626
 return& Cyc_Tcutil_ik;case 28U: _LL2B: _tmp183=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp167)->f2;_LL2C:
 return _tmp183;case 27U: _LL2D: _LL2E:
# 631
 return& Cyc_Tcutil_ak;case 19U: _LL2F: _LL30:
 return& Cyc_Tcutil_bk;case 8U: _LL31: _tmp184=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp167)->f1).num_elts;_LL32:
# 634
 if(_tmp184 == 0  || Cyc_Tcutil_is_const_exp(_tmp184))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10U: _LL33: _LL34:
 return& Cyc_Tcutil_mk;case 17U: _LL35: _tmp185=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp167)->f3;_LL36:
# 638
 if(_tmp185 == 0  || _tmp185->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp177=({struct Cyc_String_pa_PrintArg_struct _tmpA4E;_tmpA4E.tag=0U,({struct _dyneither_ptr _tmpBA2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA4E.f1=_tmpBA2;});_tmpA4E;});void*_tmp175[1U];_tmp175[0]=& _tmp177;({struct _dyneither_ptr _tmpBA3=({const char*_tmp176="typ_kind: typedef found: %s";_tag_dyneither(_tmp176,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA3,_tag_dyneither(_tmp175,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp185->kind))->v;case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp167)->f1)->r)){case 0U: _LL37: _LL38:
 return& Cyc_Tcutil_ak;case 1U: _LL39: _LL3A:
 return& Cyc_Tcutil_bk;default: _LL3B: _LL3C:
 return& Cyc_Tcutil_ak;}case 23U: _LL3D: _LL3E:
 goto _LL40;case 24U: _LL3F: _LL40:
 goto _LL42;default: _LL41: _LL42:
 return& Cyc_Tcutil_ek;}_LL0:;}
# 650
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 655
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp186;_push_handler(& _tmp186);{int _tmp188=0;if(setjmp(_tmp186.handler))_tmp188=1;if(!_tmp188){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp189=1;_npop_handler(0U);return _tmp189;};
# 657
;_pop_handler();}else{void*_tmp187=(void*)_exn_thrown;void*_tmp18A=_tmp187;void*_tmp18B;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp18A)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 659
 return 0;}else{_LL3: _tmp18B=_tmp18A;_LL4:(int)_rethrow(_tmp18B);}_LL0:;}};}
# 664
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
({void*_tmpBA4=Cyc_Tcutil_compress(t);t=_tmpBA4;});{
void*_tmp18C=t;struct Cyc_List_List*_tmp1B2;void*_tmp1B1;void*_tmp1B0;void*_tmp1AF;void*_tmp1AE;struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AB;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*_tmp1A7;void*_tmp1A6;struct Cyc_Absyn_Tqual _tmp1A5;void*_tmp1A4;struct Cyc_List_List*_tmp1A3;int _tmp1A2;struct Cyc_Absyn_VarargInfo*_tmp1A1;struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_List_List*_tmp19B;void*_tmp19A;struct Cyc_Absyn_PtrInfo _tmp199;void*_tmp198;struct Cyc_Core_Opt**_tmp197;struct Cyc_Absyn_Tvar*_tmp196;switch(*((int*)_tmp18C)){case 2U: _LL1: _tmp196=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp18C)->f1;_LL2:
# 669
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp196)){
({struct _dyneither_ptr _tmpBA5=({const char*_tmp18D="(type variable would escape scope)";_tag_dyneither(_tmp18D,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpBA5;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 673
goto _LL0;case 1U: _LL3: _tmp198=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18C)->f2;_tmp197=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18C)->f4;_LL4:
# 675
 if(t == evar){
({struct _dyneither_ptr _tmpBA6=({const char*_tmp18E="(occurs check)";_tag_dyneither(_tmp18E,sizeof(char),15U);});Cyc_Tcutil_failure_reason=_tmpBA6;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 679
if(_tmp198 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp198);else{
# 682
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp197))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 689
if(problem){
struct Cyc_List_List*_tmp18F=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp197))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
({struct Cyc_List_List*_tmpBA7=({struct Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp190->tl=_tmp18F;_tmp190;});_tmp18F=_tmpBA7;});}}
# 695
({struct Cyc_Core_Opt*_tmpBA8=({struct Cyc_Core_Opt*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->v=_tmp18F;_tmp191;});*_tmp197=_tmpBA8;});}}}
# 698
goto _LL0;case 5U: _LL5: _tmp199=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18C)->f1;_LL6:
# 701
 Cyc_Tcutil_occurs(evar,r,env,_tmp199.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp199.ptr_atts).rgn);
goto _LL0;case 8U: _LL7: _tmp19A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18C)->f1).elt_type;_LL8:
# 705
 Cyc_Tcutil_occurs(evar,r,env,_tmp19A);goto _LL0;case 9U: _LL9: _tmp1A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).tvars;_tmp1A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).effect;_tmp1A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ret_tqual;_tmp1A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ret_typ;_tmp1A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).args;_tmp1A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).c_varargs;_tmp1A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).cyc_varargs;_tmp1A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).rgn_po;_tmp19F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).attributes;_tmp19E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).requires_clause;_tmp19D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).requires_relns;_tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ensures_clause;_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ensures_relns;_LLA:
# 708
({struct Cyc_List_List*_tmpBA9=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1A7,env);env=_tmpBA9;});
if(_tmp1A6 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1A6);
Cyc_Tcutil_occurs(evar,r,env,_tmp1A4);
for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1A3->hd)).f3);}
if(_tmp1A1 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1A1->type);
for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){
struct _tuple0*_tmp192=(struct _tuple0*)_tmp1A0->hd;struct _tuple0*_tmp193=_tmp192;void*_tmp195;void*_tmp194;_LL24: _tmp195=_tmp193->f1;_tmp194=_tmp193->f2;_LL25:;
Cyc_Tcutil_occurs(evar,r,env,_tmp195);
Cyc_Tcutil_occurs(evar,r,env,_tmp194);}
# 720
goto _LL0;case 10U: _LLB: _tmp1A8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18C)->f1;_LLC:
# 722
 for(0;_tmp1A8 != 0;_tmp1A8=_tmp1A8->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1A8->hd)).f2);}
goto _LL0;case 3U: _LLD: _tmp1A9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp18C)->f1).targs;_LLE:
# 726
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1A9);goto _LL0;case 17U: _LLF: _tmp1AA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp18C)->f2;_LL10:
 _tmp1AB=_tmp1AA;goto _LL12;case 4U: _LL11: _tmp1AB=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp18C)->f1).targs;_LL12:
 _tmp1AC=_tmp1AB;goto _LL14;case 11U: _LL13: _tmp1AC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp18C)->f1).targs;_LL14:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1AC);goto _LL0;case 12U: _LL15: _tmp1AD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18C)->f2;_LL16:
# 732
 for(0;_tmp1AD != 0;_tmp1AD=_tmp1AD->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1AD->hd)->type);}
goto _LL0;case 19U: _LL17: _tmp1AE=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp18C)->f1;_LL18:
 _tmp1AF=_tmp1AE;goto _LL1A;case 23U: _LL19: _tmp1AF=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp18C)->f1;_LL1A:
 _tmp1B0=_tmp1AF;goto _LL1C;case 15U: _LL1B: _tmp1B0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp18C)->f1;_LL1C:
 _tmp1B1=_tmp1B0;goto _LL1E;case 25U: _LL1D: _tmp1B1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp18C)->f1;_LL1E:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1B1);goto _LL0;case 24U: _LL1F: _tmp1B2=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp18C)->f1;_LL20:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1B2);goto _LL0;default: _LL21: _LL22:
# 742
 goto _LL0;}_LL0:;};}
# 745
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 747
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 752
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 760
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1B3=0U;({struct _dyneither_ptr _tmpBAA=({const char*_tmp1B4="tq1 real_const not set.";_tag_dyneither(_tmp1B4,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBAA,_tag_dyneither(_tmp1B3,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1B5=0U;({struct _dyneither_ptr _tmpBAB=({const char*_tmp1B6="tq2 real_const not set.";_tag_dyneither(_tmp1B6,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBAB,_tag_dyneither(_tmp1B5,sizeof(void*),0U));});});
# 766
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 769
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpBAC=({const char*_tmp1B7="(qualifiers don't match)";_tag_dyneither(_tmp1B7,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBAC;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 777
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 781
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 788
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 790
({union Cyc_Absyn_Constraint*_tmpBAD=Cyc_Absyn_compress_conref(x);x=_tmpBAD;});
({union Cyc_Absyn_Constraint*_tmpBAE=Cyc_Absyn_compress_conref(y);y=_tmpBAE;});
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1B8=x;void*_tmp1BF;switch((((union Cyc_Absyn_Constraint*)_tmp1B8)->Eq_constr).tag){case 3U: _LL1: _LL2:
# 795
({union Cyc_Absyn_Constraint _tmpBAF=({union Cyc_Absyn_Constraint _tmpA4F;(_tmpA4F.Forward_constr).tag=2U,(_tmpA4F.Forward_constr).val=y;_tmpA4F;});*x=_tmpBAF;});return;case 1U: _LL3: _tmp1BF=(void*)(_tmp1B8->Eq_constr).val;_LL4: {
# 797
union Cyc_Absyn_Constraint*_tmp1B9=y;void*_tmp1BC;switch((((union Cyc_Absyn_Constraint*)_tmp1B9)->Eq_constr).tag){case 3U: _LL8: _LL9:
*y=*x;return;case 1U: _LLA: _tmp1BC=(void*)(_tmp1B9->Eq_constr).val;_LLB:
# 800
 if(cmp(_tmp1BF,_tmp1BC)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 804
return;default: _LLC: _LLD:
({void*_tmp1BA=0U;({struct _dyneither_ptr _tmpBB0=({const char*_tmp1BB="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1BB,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpBB0,_tag_dyneither(_tmp1BA,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 807
({void*_tmp1BD=0U;({struct _dyneither_ptr _tmpBB1=({const char*_tmp1BE="unify_conref: forward after compress";_tag_dyneither(_tmp1BE,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpBB1,_tag_dyneither(_tmp1BD,sizeof(void*),0U));});});}_LL0:;};}
# 811
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1C0;_push_handler(& _tmp1C0);{int _tmp1C2=0;if(setjmp(_tmp1C0.handler))_tmp1C2=1;if(!_tmp1C2){
({int(*_tmpBB4)(void*,void*)=cmp;union Cyc_Absyn_Constraint*_tmpBB3=x;union Cyc_Absyn_Constraint*_tmpBB2=y;Cyc_Tcutil_unify_it_conrefs(_tmpBB4,_tmpBB3,_tmpBB2,_tag_dyneither(0,0,0));});{
int _tmp1C3=1;_npop_handler(0U);return _tmp1C3;};
# 813
;_pop_handler();}else{void*_tmp1C1=(void*)_exn_thrown;void*_tmp1C4=_tmp1C1;void*_tmp1C5;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1C4)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 815
 return 0;}else{_LL3: _tmp1C5=_tmp1C4;_LL4:(int)_rethrow(_tmp1C5);}_LL0:;}};}
# 818
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp1C6=({struct _tuple0 _tmpA50;_tmpA50.f1=b1,_tmpA50.f2=b2;_tmpA50;});struct _tuple0 _tmp1C7=_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1C8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C7.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C7.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C7.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{_LL7: _tmp1C9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1C7.f1)->f1;_tmp1C8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1C7.f2)->f1;_LL8:
 return Cyc_Evexp_const_exp_cmp(_tmp1C9,_tmp1C8);}}_LL0:;}
# 827
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1CA=({struct _tuple0 _tmpA51;_tmpA51.f1=e1->r,_tmpA51.f2=e2->r;_tmpA51;});struct _tuple0 _tmp1CB=_tmp1CA;void*_tmp1D5;void*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D2;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CB.f1)->tag == 14U){_LL1: _tmp1D2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CB.f1)->f2;_LL2:
# 832
 Cyc_Tcutil_unify_cmp_exp(_tmp1D2,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CB.f2)->tag == 14U){_LL3: _tmp1D3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CB.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1D3);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CB.f1)->tag == 39U){_LL5: _tmp1D4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CB.f1)->f1;_LL6: {
# 835
void*_tmp1CC=Cyc_Tcutil_compress(_tmp1D4);void*_tmp1CD=_tmp1CC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CD)->tag == 1U){_LLC: _LLD:
({void*_tmpBB5=_tmp1D4;Cyc_Tcutil_unify_it(_tmpBB5,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->tag=18U,_tmp1CE->f1=e2;_tmp1CE;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CB.f2)->tag == 39U){_LL7: _tmp1D5=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CB.f2)->f1;_LL8: {
# 840
void*_tmp1CF=Cyc_Tcutil_compress(_tmp1D5);void*_tmp1D0=_tmp1CF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D0)->tag == 1U){_LL11: _LL12:
({void*_tmpBB6=_tmp1D5;Cyc_Tcutil_unify_it(_tmpBB6,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->tag=18U,_tmp1D1->f1=e1;_tmp1D1;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 844
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 848
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp1D6=({struct _tuple0 _tmpA52;_tmpA52.f1=b1,_tmpA52.f2=b2;_tmpA52;});struct _tuple0 _tmp1D7=_tmp1D6;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1DE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D7.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D7.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D7.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{_LL7: _tmp1DF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1D7.f1)->f1;_tmp1DE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1D7.f2)->f1;_LL8: {
# 854
int r=Cyc_Evexp_const_exp_cmp(_tmp1DF,_tmp1DE);
if(r == 0)return 0;{
struct _handler_cons _tmp1D8;_push_handler(& _tmp1D8);{int _tmp1DA=0;if(setjmp(_tmp1D8.handler))_tmp1DA=1;if(!_tmp1DA){
Cyc_Tcutil_unify_cmp_exp(_tmp1DF,_tmp1DE);{
int _tmp1DB=0;_npop_handler(0U);return _tmp1DB;};
# 857
;_pop_handler();}else{void*_tmp1D9=(void*)_exn_thrown;void*_tmp1DC=_tmp1D9;void*_tmp1DD;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1DC)->tag == Cyc_Tcutil_Unify){_LLA: _LLB:
# 860
 return r;}else{_LLC: _tmp1DD=_tmp1DC;_LLD:(int)_rethrow(_tmp1DD);}_LL9:;}};};}}}_LL0:;}
# 865
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp1E0=att;switch(*((int*)_tmp1E0)){case 0U: _LL1: _LL2:
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
# 892
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp1E1=({struct _tuple0 _tmpA53;_tmpA53.f1=att1,_tmpA53.f2=att2;_tmpA53;});struct _tuple0 _tmp1E2=_tmp1E1;enum Cyc_Absyn_Format_Type _tmp1F2;int _tmp1F1;int _tmp1F0;enum Cyc_Absyn_Format_Type _tmp1EF;int _tmp1EE;int _tmp1ED;struct _dyneither_ptr _tmp1EC;struct _dyneither_ptr _tmp1EB;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1E9;int _tmp1E8;int _tmp1E7;int _tmp1E6;int _tmp1E5;switch(*((int*)_tmp1E2.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E2.f2)->tag == 0U){_LL1: _tmp1E6=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E2.f1)->f1;_tmp1E5=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E2.f2)->f1;_LL2:
 _tmp1E8=_tmp1E6;_tmp1E7=_tmp1E5;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1E2.f2)->tag == 20U){_LL3: _tmp1E8=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1E2.f1)->f1;_tmp1E7=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1E2.f2)->f1;_LL4:
# 896
 return Cyc_Core_intcmp(_tmp1E8,_tmp1E7);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1E2.f2)->tag == 6U){_LL5: _tmp1EA=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1E2.f1)->f1;_tmp1E9=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1E2.f2)->f1;_LL6:
# 898
 if(_tmp1EA == _tmp1E9)return 0;
if(_tmp1EA == 0)return - 1;
if(_tmp1E9 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp1EA,_tmp1E9);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1E2.f2)->tag == 8U){_LL7: _tmp1EC=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1E2.f1)->f1;_tmp1EB=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1E2.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1EC,(struct _dyneither_ptr)_tmp1EB);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E2.f2)->tag == 19U){_LL9: _tmp1F2=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E2.f1)->f1;_tmp1F1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E2.f1)->f2;_tmp1F0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E2.f1)->f3;_tmp1EF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E2.f2)->f1;_tmp1EE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E2.f2)->f2;_tmp1ED=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E2.f2)->f3;_LLA: {
# 904
int _tmp1E3=Cyc_Core_intcmp((int)((unsigned int)_tmp1F2),(int)((unsigned int)_tmp1EF));
if(_tmp1E3 != 0)return _tmp1E3;{
int _tmp1E4=Cyc_Core_intcmp(_tmp1F1,_tmp1EE);
if(_tmp1E4 != 0)return _tmp1E4;
return Cyc_Core_intcmp(_tmp1F0,_tmp1ED);};}}else{goto _LLB;}default: _LLB: _LLC:
# 910
 return({int _tmpBB7=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpBB7,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
# 914
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 918
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 927
static void*Cyc_Tcutil_rgns_of(void*t);
# 929
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 933
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp1F3=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1F4=_tmp1F3;switch(((struct Cyc_Absyn_Kind*)_tmp1F4)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp1F4)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL0;default: goto _LL9;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
({void*_tmpBB9=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->tag=18U,({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_Absyn_uint_exp(0U,0U);_tmp1F5->f1=_tmpBB8;});_tmp1F5;});t=_tmpBB9;});goto _LL0;default: _LL9: _LLA:
 t=Cyc_Absyn_sint_typ;goto _LL0;}_LL0:;}
# 942
return({struct _tuple16*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->f1=tv,_tmp1F6->f2=t;_tmp1F6;});}
# 949
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1F7=Cyc_Tcutil_compress(t);void*_tmp1F8=_tmp1F7;struct Cyc_List_List*_tmp21E;void*_tmp21D;struct Cyc_List_List*_tmp21C;void*_tmp21B;struct Cyc_Absyn_Tqual _tmp21A;void*_tmp219;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_VarargInfo*_tmp217;struct Cyc_List_List*_tmp216;struct Cyc_List_List*_tmp215;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp212;void*_tmp211;void*_tmp210;void*_tmp20F;struct Cyc_List_List*_tmp20E;void*_tmp20D;void*_tmp20C;void*_tmp20B;switch(*((int*)_tmp1F8)){case 0U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 goto _LLA;case 18U: _LL9: _LLA:
 goto _LLC;case 27U: _LLB: _LLC:
 goto _LLE;case 28U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 return Cyc_Absyn_empty_effect;case 1U: _LL11: _LL12:
 goto _LL14;case 2U: _LL13: _LL14: {
# 961
struct Cyc_Absyn_Kind*_tmp1F9=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1FA=_tmp1F9;switch(((struct Cyc_Absyn_Kind*)_tmp1FA)->kind){case Cyc_Absyn_RgnKind: _LL3C: _LL3D:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->tag=23U,_tmp1FB->f1=t;_tmp1FB;});case Cyc_Absyn_EffKind: _LL3E: _LL3F:
 return t;case Cyc_Absyn_IntKind: _LL40: _LL41:
 return Cyc_Absyn_empty_effect;default: _LL42: _LL43:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->tag=25U,_tmp1FC->f1=t;_tmp1FC;});}_LL3B:;}case 15U: _LL15: _tmp20B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F8)->f1;_LL16:
# 967
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->tag=23U,_tmp1FD->f1=_tmp20B;_tmp1FD;});case 16U: _LL17: _tmp20D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F8)->f1;_tmp20C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F8)->f2;_LL18:
# 971
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->tag=23U,_tmp1FE->f1=_tmp20C;_tmp1FE;});case 3U: _LL19: _tmp20E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F8)->f1).targs;_LL1A: {
# 973
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp20E);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));_tmp1FF->tag=24U,_tmp1FF->f1=ts;_tmp1FF;}));}case 5U: _LL1B: _tmp210=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8)->f1).elt_typ;_tmp20F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8)->f1).ptr_atts).rgn;_LL1C:
# 977
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->tag=24U,({struct Cyc_List_List*_tmpBBC=({void*_tmp200[2U];({void*_tmpBBB=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->tag=23U,_tmp201->f1=_tmp20F;_tmp201;});_tmp200[0]=_tmpBBB;}),({void*_tmpBBA=Cyc_Tcutil_rgns_of(_tmp210);_tmp200[1]=_tmpBBA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp200,sizeof(void*),2U));});_tmp202->f1=_tmpBBC;});_tmp202;}));case 8U: _LL1D: _tmp211=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F8)->f1).elt_type;_LL1E:
# 979
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp211));case 10U: _LL1F: _tmp212=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F8)->f1;_LL20: {
# 981
struct Cyc_List_List*_tmp203=0;
for(0;_tmp212 != 0;_tmp212=_tmp212->tl){
({struct Cyc_List_List*_tmpBBD=({struct Cyc_List_List*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->hd=(*((struct _tuple12*)_tmp212->hd)).f2,_tmp204->tl=_tmp203;_tmp204;});_tmp203=_tmpBBD;});}
_tmp213=_tmp203;goto _LL22;}case 4U: _LL21: _tmp213=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F8)->f1).targs;_LL22:
 _tmp214=_tmp213;goto _LL24;case 11U: _LL23: _tmp214=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F8)->f1).targs;_LL24:
# 987
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->tag=24U,({struct Cyc_List_List*_tmpBBE=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp214);_tmp205->f1=_tmpBBE;});_tmp205;}));case 12U: _LL25: _tmp215=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F8)->f2;_LL26:
# 989
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->tag=24U,({struct Cyc_List_List*_tmpBBF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp215);_tmp206->f1=_tmpBBF;});_tmp206;}));case 19U: _LL27: _LL28:
 return Cyc_Absyn_empty_effect;case 9U: _LL29: _tmp21C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8)->f1).tvars;_tmp21B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8)->f1).effect;_tmp21A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8)->f1).ret_tqual;_tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8)->f1).ret_typ;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8)->f1).args;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8)->f1).cyc_varargs;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F8)->f1).rgn_po;_LL2A: {
# 999
void*_tmp207=({struct Cyc_List_List*_tmpBC0=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp21C);Cyc_Tcutil_substitute(_tmpBC0,(void*)_check_null(_tmp21B));});
return Cyc_Tcutil_normalize_effect(_tmp207);}case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 return Cyc_Absyn_empty_effect;case 23U: _LL31: _LL32:
 goto _LL34;case 24U: _LL33: _LL34:
 return t;case 25U: _LL35: _tmp21D=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1F8)->f1;_LL36:
 return Cyc_Tcutil_rgns_of(_tmp21D);case 17U: _LL37: _tmp21E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F8)->f2;_LL38:
# 1008
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->tag=24U,({struct Cyc_List_List*_tmpBC1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp21E);_tmp208->f1=_tmpBC1;});_tmp208;}));default: _LL39: _LL3A:
({void*_tmp209=0U;({struct _dyneither_ptr _tmpBC2=({const char*_tmp20A="typedecl in rgns_of";_tag_dyneither(_tmp20A,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC2,_tag_dyneither(_tmp209,sizeof(void*),0U));});});}_LL0:;}
# 1016
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpBC3=Cyc_Tcutil_compress(e);e=_tmpBC3;});{
void*_tmp21F=e;void*_tmp22B;struct Cyc_List_List**_tmp22A;switch(*((int*)_tmp21F)){case 24U: _LL1: _tmp22A=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp21F)->f1;_LL2: {
# 1020
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp22A;for(0;effs != 0;effs=effs->tl){
void*_tmp220=(void*)effs->hd;
({void*_tmpBC4=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp220));effs->hd=_tmpBC4;});{
void*_tmp221=(void*)effs->hd;void*_tmp222=_tmp221;switch(*((int*)_tmp222)){case 24U: _LL8: _LL9:
 goto _LLB;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp222)->f1)){case 20U: _LLA: _LLB:
 goto _LLD;case 22U: _LLC: _LLD:
 goto _LLF;case 21U: _LLE: _LLF:
 redo_join=1;goto _LL7;default: goto _LL10;}default: _LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1032
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp22A;for(0;effs != 0;effs=effs->tl){
void*_tmp223=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp224=_tmp223;void*_tmp227;struct Cyc_List_List*_tmp226;switch(*((int*)_tmp224)){case 24U: _LL13: _tmp226=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp224)->f1;_LL14:
# 1037
({struct Cyc_List_List*_tmpBC5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp226,effects);effects=_tmpBC5;});
goto _LL12;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp224)->f1)){case 20U: _LL15: _LL16:
 goto _LL18;case 22U: _LL17: _LL18:
 goto _LL1A;case 21U: _LL19: _LL1A:
 goto _LL12;default: goto _LL1B;}default: _LL1B: _tmp227=_tmp224;_LL1C:
({struct Cyc_List_List*_tmpBC6=({struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->hd=_tmp227,_tmp225->tl=effects;_tmp225;});effects=_tmpBC6;});goto _LL12;}_LL12:;}}
# 1045
({struct Cyc_List_List*_tmpBC7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp22A=_tmpBC7;});
return e;};}case 25U: _LL3: _tmp22B=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp21F)->f1;_LL4: {
# 1048
void*_tmp228=Cyc_Tcutil_compress(_tmp22B);void*_tmp229=_tmp228;switch(*((int*)_tmp229)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp22B);}_LL1D:;}default: _LL5: _LL6:
# 1053
 return e;}_LL0:;};}
# 1058
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22C=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->tag=9U,(_tmp22D->f1).tvars=0,(_tmp22D->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmpBC8=Cyc_Absyn_empty_tqual(0U);(_tmp22D->f1).ret_tqual=_tmpBC8;}),(_tmp22D->f1).ret_typ=(void*)& Cyc_Absyn_VoidType_val,(_tmp22D->f1).args=0,(_tmp22D->f1).c_varargs=0,(_tmp22D->f1).cyc_varargs=0,(_tmp22D->f1).rgn_po=0,(_tmp22D->f1).attributes=0,(_tmp22D->f1).requires_clause=0,(_tmp22D->f1).requires_relns=0,(_tmp22D->f1).ensures_clause=0,(_tmp22D->f1).ensures_relns=0;_tmp22D;});
# 1068
return({void*_tmpBCB=(void*)_tmp22C;struct Cyc_Absyn_Tqual _tmpBCA=Cyc_Absyn_empty_tqual(0U);void*_tmpBC9=Cyc_Absyn_bounds_one;Cyc_Absyn_atb_typ(_tmpBCB,(void*)& Cyc_Absyn_HeapRgn_val,_tmpBCA,_tmpBC9,Cyc_Absyn_false_conref);});}
# 1075
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpBCC=Cyc_Tcutil_compress(r);r=_tmpBCC;});
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp22E=Cyc_Tcutil_compress(e);void*_tmp22F=_tmp22E;struct Cyc_Core_Opt*_tmp24F;void**_tmp24E;struct Cyc_Core_Opt*_tmp24D;void*_tmp24C;struct Cyc_List_List*_tmp24B;void*_tmp24A;switch(*((int*)_tmp22F)){case 23U: _LL1: _tmp24A=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp22F)->f1;_LL2:
# 1082
 if(constrain)return Cyc_Tcutil_unify(r,_tmp24A);
({void*_tmpBCD=Cyc_Tcutil_compress(_tmp24A);_tmp24A=_tmpBCD;});
if(r == _tmp24A)return 1;{
struct _tuple0 _tmp230=({struct _tuple0 _tmpA54;_tmpA54.f1=r,_tmpA54.f2=_tmp24A;_tmpA54;});struct _tuple0 _tmp231=_tmp230;struct Cyc_Absyn_Tvar*_tmp233;struct Cyc_Absyn_Tvar*_tmp232;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f2)->tag == 2U){_LLC: _tmp233=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f1)->f1;_tmp232=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp233,_tmp232)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};case 24U: _LL3: _tmp24B=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp22F)->f1;_LL4:
# 1090
 for(0;_tmp24B != 0;_tmp24B=_tmp24B->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp24B->hd))return 1;}
return 0;case 25U: _LL5: _tmp24C=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp22F)->f1;_LL6: {
# 1094
void*_tmp234=Cyc_Tcutil_rgns_of(_tmp24C);void*_tmp235=_tmp234;void*_tmp241;void*_tmp240;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp235)->tag == 25U){_LL11: _tmp240=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp235)->f1;_LL12:
# 1096
 if(!constrain)return 0;{
void*_tmp236=Cyc_Tcutil_compress(_tmp240);void*_tmp237=_tmp236;struct Cyc_Core_Opt*_tmp23F;void**_tmp23E;struct Cyc_Core_Opt*_tmp23D;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->tag == 1U){_LL16: _tmp23F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->f1;_tmp23E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->f2;_tmp23D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->f4;_LL17: {
# 1101
void*_tmp238=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp23D);
# 1104
Cyc_Tcutil_occurs(_tmp238,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23D))->v,r);{
void*_tmp239=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C->tag=24U,({struct Cyc_List_List*_tmpBCF=({void*_tmp23A[2U];_tmp23A[0]=_tmp238,({void*_tmpBCE=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->tag=23U,_tmp23B->f1=r;_tmp23B;});_tmp23A[1]=_tmpBCE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp23A,sizeof(void*),2U));});_tmp23C->f1=_tmpBCF;});_tmp23C;}));
*_tmp23E=_tmp239;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{_LL13: _tmp241=_tmp235;_LL14:
# 1110
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp241);}_LL10:;}case 1U: _LL7: _tmp24F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22F)->f1;_tmp24E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22F)->f2;_tmp24D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22F)->f4;_LL8:
# 1113
 if(_tmp24F == 0  || ((struct Cyc_Absyn_Kind*)_tmp24F->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp242=0U;({struct _dyneither_ptr _tmpBD0=({const char*_tmp243="effect evar has wrong kind";_tag_dyneither(_tmp243,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBD0,_tag_dyneither(_tmp242,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1118
void*_tmp244=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp24D);
# 1121
Cyc_Tcutil_occurs(_tmp244,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24D))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp245=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249->tag=24U,({struct Cyc_List_List*_tmpBD3=({struct Cyc_List_List*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->hd=_tmp244,({struct Cyc_List_List*_tmpBD2=({struct Cyc_List_List*_tmp247=_cycalloc(sizeof(*_tmp247));({void*_tmpBD1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246->tag=23U,_tmp246->f1=r;_tmp246;});_tmp247->hd=_tmpBD1;}),_tmp247->tl=0;_tmp247;});_tmp248->tl=_tmpBD2;});_tmp248;});_tmp249->f1=_tmpBD3;});_tmp249;});
*_tmp24E=(void*)_tmp245;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1132
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpBD4=Cyc_Tcutil_compress(t);t=_tmpBD4;});{
void*_tmp250=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp251=_tmp250;struct Cyc_Core_Opt*_tmp266;void**_tmp265;struct Cyc_Core_Opt*_tmp264;void*_tmp263;struct Cyc_List_List*_tmp262;switch(*((int*)_tmp251)){case 23U: _LL1: _LL2:
 return 0;case 24U: _LL3: _tmp262=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp251)->f1;_LL4:
# 1137
 for(0;_tmp262 != 0;_tmp262=_tmp262->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp262->hd))
return 1;}
return 0;case 25U: _LL5: _tmp263=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp251)->f1;_LL6:
# 1142
({void*_tmpBD5=Cyc_Tcutil_compress(_tmp263);_tmp263=_tmpBD5;});
if(t == _tmp263)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp263);{
void*_tmp252=Cyc_Tcutil_rgns_of(t);void*_tmp253=_tmp252;void*_tmp259;void*_tmp258;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp253)->tag == 25U){_LLC: _tmp258=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp253)->f1;_LLD: {
# 1147
struct _tuple0 _tmp254=({struct _tuple0 _tmpA55;({void*_tmpBD6=Cyc_Tcutil_compress(_tmp258);_tmpA55.f1=_tmpBD6;}),_tmpA55.f2=_tmp263;_tmpA55;});struct _tuple0 _tmp255=_tmp254;struct Cyc_Absyn_Tvar*_tmp257;struct Cyc_Absyn_Tvar*_tmp256;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp255.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp255.f2)->tag == 2U){_LL11: _tmp257=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp255.f1)->f1;_tmp256=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp255.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp263);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp258 == _tmp263;}_LL10:;}}else{_LLE: _tmp259=_tmp253;_LLF:
# 1151
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp259);}_LLB:;};case 1U: _LL7: _tmp266=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp251)->f1;_tmp265=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp251)->f2;_tmp264=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp251)->f4;_LL8:
# 1154
 if(_tmp266 == 0  || ((struct Cyc_Absyn_Kind*)_tmp266->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp25A=0U;({struct _dyneither_ptr _tmpBD7=({const char*_tmp25B="effect evar has wrong kind";_tag_dyneither(_tmp25B,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBD7,_tag_dyneither(_tmp25A,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1159
void*_tmp25C=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp264);
# 1162
Cyc_Tcutil_occurs(_tmp25C,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp264))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25D=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp261=_cycalloc(sizeof(*_tmp261));_tmp261->tag=24U,({struct Cyc_List_List*_tmpBDA=({struct Cyc_List_List*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->hd=_tmp25C,({struct Cyc_List_List*_tmpBD9=({struct Cyc_List_List*_tmp25F=_cycalloc(sizeof(*_tmp25F));({void*_tmpBD8=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->tag=25U,_tmp25E->f1=t;_tmp25E;});_tmp25F->hd=_tmpBD8;}),_tmp25F->tl=0;_tmp25F;});_tmp260->tl=_tmpBD9;});_tmp260;});_tmp261->f1=_tmpBDA;});_tmp261;});
*_tmp265=(void*)_tmp25D;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1173
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpBDB=Cyc_Tcutil_compress(e);e=_tmpBDB;});{
void*_tmp267=e;struct Cyc_Core_Opt*_tmp283;void**_tmp282;struct Cyc_Core_Opt*_tmp281;void*_tmp280;struct Cyc_List_List*_tmp27F;struct Cyc_Absyn_Tvar*_tmp27E;switch(*((int*)_tmp267)){case 2U: _LL1: _tmp27E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp267)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp27E)== 0;case 24U: _LL3: _tmp27F=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp267)->f1;_LL4:
# 1178
 for(0;_tmp27F != 0;_tmp27F=_tmp27F->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp27F->hd))
return 1;}
return 0;case 25U: _LL5: _tmp280=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp267)->f1;_LL6: {
# 1183
void*_tmp268=Cyc_Tcutil_rgns_of(_tmp280);void*_tmp269=_tmp268;void*_tmp275;void*_tmp274;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp269)->tag == 25U){_LLC: _tmp274=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp269)->f1;_LLD:
# 1185
 if(!may_constrain_evars)return 0;{
void*_tmp26A=Cyc_Tcutil_compress(_tmp274);void*_tmp26B=_tmp26A;struct Cyc_Core_Opt*_tmp273;void**_tmp272;struct Cyc_Core_Opt*_tmp271;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26B)->tag == 1U){_LL11: _tmp273=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26B)->f1;_tmp272=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26B)->f2;_tmp271=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26B)->f4;_LL12: {
# 1191
void*_tmp26C=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp271);
# 1193
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp271))->v,v))return 0;{
void*_tmp26D=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->tag=24U,({struct Cyc_List_List*_tmpBDD=({void*_tmp26E[2U];_tmp26E[0]=_tmp26C,({void*_tmpBDC=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->tag=2U,_tmp26F->f1=v;_tmp26F;});_tmp26E[1]=_tmpBDC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26E,sizeof(void*),2U));});_tmp270->f1=_tmpBDD;});_tmp270;}));
*_tmp272=_tmp26D;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{_LLE: _tmp275=_tmp269;_LLF:
# 1199
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp275);}_LLB:;}case 1U: _LL7: _tmp283=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp267)->f1;_tmp282=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp267)->f2;_tmp281=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp267)->f4;_LL8:
# 1202
 if(_tmp283 == 0  || ((struct Cyc_Absyn_Kind*)_tmp283->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp276=0U;({struct _dyneither_ptr _tmpBDE=({const char*_tmp277="effect evar has wrong kind";_tag_dyneither(_tmp277,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBDE,_tag_dyneither(_tmp276,sizeof(void*),0U));});});{
# 1206
void*_tmp278=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp281);
# 1208
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp281))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp279=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->tag=24U,({struct Cyc_List_List*_tmpBE1=({struct Cyc_List_List*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->hd=_tmp278,({struct Cyc_List_List*_tmpBE0=({struct Cyc_List_List*_tmp27B=_cycalloc(sizeof(*_tmp27B));({void*_tmpBDF=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A->tag=2U,_tmp27A->f1=v;_tmp27A;});_tmp27B->hd=_tmpBDF;}),_tmp27B->tl=0;_tmp27B;});_tmp27C->tl=_tmpBE0;});_tmp27C;});_tmp27D->f1=_tmpBE1;});_tmp27D;});
*_tmp282=(void*)_tmp279;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1218
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpBE2=Cyc_Tcutil_compress(e);e=_tmpBE2;});{
void*_tmp284=e;void*_tmp28A;struct Cyc_List_List*_tmp289;switch(*((int*)_tmp284)){case 24U: _LL1: _tmp289=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp284)->f1;_LL2:
# 1222
 for(0;_tmp289 != 0;_tmp289=_tmp289->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp289->hd))
return 1;}
return 0;case 25U: _LL3: _tmp28A=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp284)->f1;_LL4: {
# 1227
void*_tmp285=Cyc_Tcutil_rgns_of(_tmp28A);void*_tmp286=_tmp285;void*_tmp288;void*_tmp287;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp286)->tag == 25U){_LLA: _tmp287=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp286)->f1;_LLB:
 return 0;}else{_LLC: _tmp288=_tmp286;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp288);}_LL9:;}case 1U: _LL5: _LL6:
# 1231
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1245 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1250
void*_tmp28B=Cyc_Tcutil_compress(e1);void*_tmp28C=_tmp28B;void**_tmp299;struct Cyc_Core_Opt*_tmp298;void*_tmp297;struct Cyc_Absyn_Tvar*_tmp296;void*_tmp295;struct Cyc_List_List*_tmp294;switch(*((int*)_tmp28C)){case 24U: _LL1: _tmp294=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp28C)->f1;_LL2:
# 1252
 for(0;_tmp294 != 0;_tmp294=_tmp294->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp294->hd,e2))
return 0;}
return 1;case 23U: _LL3: _tmp295=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp28C)->f1;_LL4:
# 1263
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp295,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp295,(void*)& Cyc_Absyn_HeapRgn_val);case 2U: _LL5: _tmp296=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp28C)->f1;_LL6:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp296,e2);case 25U: _LL7: _tmp297=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28C)->f1;_LL8: {
# 1267
void*_tmp28D=Cyc_Tcutil_rgns_of(_tmp297);void*_tmp28E=_tmp28D;void*_tmp290;void*_tmp28F;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28E)->tag == 25U){_LLE: _tmp28F=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28E)->f1;_LLF:
# 1272
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp28F,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp28F,Cyc_Absyn_sint_typ);}else{_LL10: _tmp290=_tmp28E;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp290,e2);}_LLD:;}case 1U: _LL9: _tmp299=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28C)->f2;_tmp298=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28C)->f4;_LLA:
# 1277
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1281
*_tmp299=Cyc_Absyn_empty_effect;
# 1284
return 1;}else{
# 1286
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp293=({struct Cyc_String_pa_PrintArg_struct _tmpA56;_tmpA56.tag=0U,({struct _dyneither_ptr _tmpBE3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmpA56.f1=_tmpBE3;});_tmpA56;});void*_tmp291[1U];_tmp291[0]=& _tmp293;({struct _dyneither_ptr _tmpBE4=({const char*_tmp292="subset_effect: bad effect: %s";_tag_dyneither(_tmp292,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBE4,_tag_dyneither(_tmp291,sizeof(void*),1U));});});}_LL0:;}
# 1302 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpBE5=Cyc_Tcutil_normalize_effect(e1);e1=_tmpBE5;});
({void*_tmpBE6=Cyc_Tcutil_normalize_effect(e2);e2=_tmpBE6;});
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1318
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1320
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp29A=(struct _tuple0*)r1->hd;struct _tuple0*_tmp29B=_tmp29A;void*_tmp2A1;void*_tmp2A0;_LL1: _tmp2A1=_tmp29B->f1;_tmp2A0=_tmp29B->f2;_LL2:;{
int found=_tmp2A1 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp29C=(struct _tuple0*)r2->hd;struct _tuple0*_tmp29D=_tmp29C;void*_tmp29F;void*_tmp29E;_LL4: _tmp29F=_tmp29D->f1;_tmp29E=_tmp29D->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp2A1,_tmp29F) && Cyc_Tcutil_unify(_tmp2A0,_tmp29E)){
found=1;
break;}}}
# 1330
if(!found)return 0;};}}
# 1332
return 1;}
# 1339
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp2A2=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp2A3=({struct Cyc_List_List*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4->hd=_tmp2A2,_tmp2A4->tl=r1;_tmp2A4;});
if(Cyc_Relations_consistent_relations(_tmp2A3))return 0;}
# 1345
return 1;}
# 1350
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1356
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1358
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1362
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpBE7=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpBE7;});
({void*_tmpBE8=Cyc_Tcutil_compress(t1);t1=_tmpBE8;});
({void*_tmpBE9=Cyc_Tcutil_compress(t2);t2=_tmpBE9;});
if(t1 == t2)return;
{void*_tmp2A5=t1;struct Cyc_Core_Opt*_tmp2B4;void**_tmp2B3;struct Cyc_Core_Opt*_tmp2B2;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->tag == 1U){_LL1: _tmp2B4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f1;_tmp2B3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f2;_tmp2B2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A5)->f4;_LL2:
# 1373
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B2))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2A6=Cyc_Tcutil_typ_kind(t2);
# 1378
if(Cyc_Tcutil_kind_leq(_tmp2A6,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2B4))->v)){
*_tmp2B3=t2;
return;}else{
# 1382
{void*_tmp2A7=t2;struct Cyc_Absyn_PtrInfo _tmp2B0;void**_tmp2AF;struct Cyc_Core_Opt*_tmp2AE;switch(*((int*)_tmp2A7)){case 1U: _LL6: _tmp2AF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f2;_tmp2AE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f4;_LL7: {
# 1385
struct Cyc_List_List*_tmp2A8=(struct Cyc_List_List*)_tmp2B2->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2AE))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2A8,(struct Cyc_Absyn_Tvar*)s2->hd)){
({struct _dyneither_ptr _tmpBEA=({const char*_tmp2A9="(type variable would escape scope)";_tag_dyneither(_tmp2A9,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpBEA;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1392
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2B4->v,_tmp2A6)){
*_tmp2AF=t1;return;}
# 1395
({struct _dyneither_ptr _tmpBEB=({const char*_tmp2AA="(kinds are incompatible)";_tag_dyneither(_tmp2AA,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBEB;});
goto _LL5;}case 5U: _LL8: _tmp2B0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7)->f1;if(((struct Cyc_Absyn_Kind*)_tmp2B4->v)->kind == Cyc_Absyn_BoxKind){_LL9: {
# 1401
union Cyc_Absyn_Constraint*_tmp2AB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp2B0.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp2AC=_tmp2AB;if((((union Cyc_Absyn_Constraint*)_tmp2AC)->No_constr).tag == 3){_LLD: _LLE:
# 1405
({struct _union_Constraint_Eq_constr*_tmp2AD=& _tmp2AB->Eq_constr;_tmp2AD->tag=1;_tmp2AD->val=Cyc_Absyn_bounds_one;});
*_tmp2B3=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1410
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1413
({struct _dyneither_ptr _tmpBEC=({const char*_tmp2B1="(kinds are incompatible)";_tag_dyneither(_tmp2B1,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBEC;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1416
 goto _LL0;}_LL0:;}
# 1421
{struct _tuple0 _tmp2B5=({struct _tuple0 _tmpA5C;_tmpA5C.f1=t2,_tmpA5C.f2=t1;_tmpA5C;});struct _tuple0 _tmp2B6=_tmp2B5;void*_tmp372;void*_tmp371;void*_tmp370;void*_tmp36F;void*_tmp36E;void*_tmp36D;struct Cyc_List_List*_tmp36C;struct Cyc_Absyn_Typedefdecl*_tmp36B;struct Cyc_List_List*_tmp36A;struct Cyc_Absyn_Typedefdecl*_tmp369;enum Cyc_Absyn_AggrKind _tmp368;struct Cyc_List_List*_tmp367;enum Cyc_Absyn_AggrKind _tmp366;struct Cyc_List_List*_tmp365;struct Cyc_List_List*_tmp364;struct Cyc_List_List*_tmp363;struct Cyc_List_List*_tmp362;void*_tmp361;struct Cyc_Absyn_Tqual _tmp360;void*_tmp35F;struct Cyc_List_List*_tmp35E;int _tmp35D;struct Cyc_Absyn_VarargInfo*_tmp35C;struct Cyc_List_List*_tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_List_List*_tmp358;struct Cyc_Absyn_Exp*_tmp357;struct Cyc_List_List*_tmp356;struct Cyc_List_List*_tmp355;void*_tmp354;struct Cyc_Absyn_Tqual _tmp353;void*_tmp352;struct Cyc_List_List*_tmp351;int _tmp350;struct Cyc_Absyn_VarargInfo*_tmp34F;struct Cyc_List_List*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_List_List*_tmp34B;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_List_List*_tmp349;void*_tmp348;struct Cyc_Absyn_Tqual _tmp347;struct Cyc_Absyn_Exp*_tmp346;union Cyc_Absyn_Constraint*_tmp345;void*_tmp344;struct Cyc_Absyn_Tqual _tmp343;struct Cyc_Absyn_Exp*_tmp342;union Cyc_Absyn_Constraint*_tmp341;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;void*_tmp33E;void*_tmp33D;int _tmp33C;int _tmp33B;enum Cyc_Absyn_Sign _tmp33A;enum Cyc_Absyn_Size_of _tmp339;enum Cyc_Absyn_Sign _tmp338;enum Cyc_Absyn_Size_of _tmp337;void*_tmp336;struct Cyc_Absyn_Tqual _tmp335;void*_tmp334;union Cyc_Absyn_Constraint*_tmp333;union Cyc_Absyn_Constraint*_tmp332;union Cyc_Absyn_Constraint*_tmp331;void*_tmp330;struct Cyc_Absyn_Tqual _tmp32F;void*_tmp32E;union Cyc_Absyn_Constraint*_tmp32D;union Cyc_Absyn_Constraint*_tmp32C;union Cyc_Absyn_Constraint*_tmp32B;struct Cyc_Absyn_Datatypedecl*_tmp32A;struct Cyc_Absyn_Datatypefield*_tmp329;struct Cyc_List_List*_tmp328;struct Cyc_Absyn_Datatypedecl*_tmp327;struct Cyc_Absyn_Datatypefield*_tmp326;struct Cyc_List_List*_tmp325;struct Cyc_Absyn_Datatypedecl*_tmp324;struct Cyc_List_List*_tmp323;struct Cyc_Absyn_Datatypedecl*_tmp322;struct Cyc_List_List*_tmp321;struct Cyc_List_List*_tmp320;struct Cyc_List_List*_tmp31F;struct _tuple2*_tmp31E;struct _tuple2*_tmp31D;union Cyc_Absyn_AggrInfoU _tmp31C;struct Cyc_List_List*_tmp31B;union Cyc_Absyn_AggrInfoU _tmp31A;struct Cyc_List_List*_tmp319;struct Cyc_Absyn_Tvar*_tmp318;struct Cyc_Absyn_Tvar*_tmp317;switch(*((int*)_tmp2B6.f1)){case 1U: _LL12: _LL13:
# 1424
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: switch(*((int*)_tmp2B6.f2)){case 0U: _LL14: _LL15:
# 1427
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 2U: switch(*((int*)_tmp2B6.f2)){case 2U: _LL16: _tmp318=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp317=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL17: {
# 1430
struct _dyneither_ptr*_tmp2B7=_tmp318->name;
struct _dyneither_ptr*_tmp2B8=_tmp317->name;
# 1433
int _tmp2B9=_tmp318->identity;
int _tmp2BA=_tmp317->identity;
if(_tmp2BA == _tmp2B9)return;
({struct _dyneither_ptr _tmpBED=({const char*_tmp2BB="(variable types are not the same)";_tag_dyneither(_tmp2BB,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpBED;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 11U: switch(*((int*)_tmp2B6.f2)){case 11U: _LL18: _tmp31C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B6.f1)->f1).aggr_info;_tmp31B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B6.f1)->f1).targs;_tmp31A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B6.f2)->f1).aggr_info;_tmp319=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B6.f2)->f1).targs;_LL19: {
# 1440
struct _tuple13 _tmp2BC=Cyc_Absyn_aggr_kinded_name(_tmp31A);struct _tuple13 _tmp2BD=_tmp2BC;enum Cyc_Absyn_AggrKind _tmp2C5;struct _tuple2*_tmp2C4;_LL4F: _tmp2C5=_tmp2BD.f1;_tmp2C4=_tmp2BD.f2;_LL50:;{
struct _tuple13 _tmp2BE=Cyc_Absyn_aggr_kinded_name(_tmp31C);struct _tuple13 _tmp2BF=_tmp2BE;enum Cyc_Absyn_AggrKind _tmp2C3;struct _tuple2*_tmp2C2;_LL52: _tmp2C3=_tmp2BF.f1;_tmp2C2=_tmp2BF.f2;_LL53:;
if(_tmp2C5 != _tmp2C3){({struct _dyneither_ptr _tmpBEE=({const char*_tmp2C0="(struct and union type)";_tag_dyneither(_tmp2C0,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpBEE;});goto _LL11;}
if(Cyc_Absyn_qvar_cmp(_tmp2C4,_tmp2C2)!= 0){({struct _dyneither_ptr _tmpBEF=({const char*_tmp2C1="(different type name)";_tag_dyneither(_tmp2C1,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpBEF;});goto _LL11;}
Cyc_Tcutil_unify_list(_tmp319,_tmp31B);
return;};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 13U: switch(*((int*)_tmp2B6.f2)){case 13U: _LL1A: _tmp31E=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp31D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL1B:
# 1449
 if(Cyc_Absyn_qvar_cmp(_tmp31E,_tmp31D)== 0)return;
({struct _dyneither_ptr _tmpBF0=({const char*_tmp2C6="(different enum types)";_tag_dyneither(_tmp2C6,sizeof(char),23U);});Cyc_Tcutil_failure_reason=_tmpBF0;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 14U: switch(*((int*)_tmp2B6.f2)){case 14U: _LL1C: _tmp320=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp31F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL1D: {
# 1454
int bad=0;
for(0;_tmp320 != 0  && _tmp31F != 0;(_tmp320=_tmp320->tl,_tmp31F=_tmp31F->tl)){
struct Cyc_Absyn_Enumfield*_tmp2C7=(struct Cyc_Absyn_Enumfield*)_tmp320->hd;
struct Cyc_Absyn_Enumfield*_tmp2C8=(struct Cyc_Absyn_Enumfield*)_tmp31F->hd;
if(Cyc_Absyn_qvar_cmp(_tmp2C7->name,_tmp2C8->name)!= 0){
({struct _dyneither_ptr _tmpBF1=({const char*_tmp2C9="(different names for enum fields)";_tag_dyneither(_tmp2C9,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpBF1;});
bad=1;
break;}
# 1463
if(_tmp2C7->tag == _tmp2C8->tag)continue;
if(_tmp2C7->tag == 0  || _tmp2C8->tag == 0){
({struct _dyneither_ptr _tmpBF2=({const char*_tmp2CA="(different tag values for enum fields)";_tag_dyneither(_tmp2CA,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBF2;});
bad=1;
break;}
# 1469
if(!({struct Cyc_Absyn_Exp*_tmpBF3=(struct Cyc_Absyn_Exp*)_check_null(_tmp2C7->tag);Cyc_Evexp_same_const_exp(_tmpBF3,(struct Cyc_Absyn_Exp*)_check_null(_tmp2C8->tag));})){
({struct _dyneither_ptr _tmpBF4=({const char*_tmp2CB="(different tag values for enum fields)";_tag_dyneither(_tmp2CB,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBF4;});
bad=1;
break;}}
# 1475
if(bad)goto _LL11;
if(_tmp320 == 0  && _tmp31F == 0)return;
({struct _dyneither_ptr _tmpBF5=({const char*_tmp2CC="(different number of fields for enums)";_tag_dyneither(_tmp2CC,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBF5;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B6.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp2B6.f2)){case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B6.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL1E: _tmp324=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B6.f1)->f1).datatype_info).KnownDatatype).val;_tmp323=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B6.f1)->f1).targs;_tmp322=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B6.f2)->f1).datatype_info).KnownDatatype).val;_tmp321=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B6.f2)->f1).targs;_LL1F:
# 1482
 if(_tmp324 == _tmp322  || Cyc_Absyn_qvar_cmp(_tmp324->name,_tmp322->name)== 0){
Cyc_Tcutil_unify_list(_tmp321,_tmp323);
return;}
# 1486
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp2B6.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp2B6.f2)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL20: _tmp32A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp329=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp328=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f1)->f1).targs;_tmp327=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp326=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp325=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B6.f2)->f1).targs;_LL21:
# 1490
 if((_tmp32A == _tmp327  || Cyc_Absyn_qvar_cmp(_tmp32A->name,_tmp327->name)== 0) && (
_tmp329 == _tmp326  || Cyc_Absyn_qvar_cmp(_tmp329->name,_tmp326->name)== 0)){
Cyc_Tcutil_unify_list(_tmp325,_tmp328);
return;}
# 1495
({struct _dyneither_ptr _tmpBF6=({const char*_tmp2CD="(different datatype field types)";_tag_dyneither(_tmp2CD,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpBF6;});
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp2B6.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 5U: switch(*((int*)_tmp2B6.f2)){case 5U: _LL22: _tmp336=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f1)->f1).elt_typ;_tmp335=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f1)->f1).elt_tq;_tmp334=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ptr_atts).rgn;_tmp333=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ptr_atts).nullable;_tmp332=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ptr_atts).bounds;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ptr_atts).zero_term;_tmp330=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f2)->f1).elt_typ;_tmp32F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f2)->f1).elt_tq;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ptr_atts).rgn;_tmp32D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ptr_atts).nullable;_tmp32C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ptr_atts).bounds;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ptr_atts).zero_term;_LL23:
# 1500
 Cyc_Tcutil_unify_it(_tmp330,_tmp336);
Cyc_Tcutil_unify_it(_tmp334,_tmp32E);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpBF8=_tmp32B;union Cyc_Absyn_Constraint*_tmpBF7=_tmp331;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBF8,_tmpBF7,({const char*_tmp2CE="(not both zero terminated)";_tag_dyneither(_tmp2CE,sizeof(char),27U);}));});
Cyc_Tcutil_unify_tqual(_tmp32F,_tmp330,_tmp335,_tmp336);
({union Cyc_Absyn_Constraint*_tmpBFA=_tmp32C;union Cyc_Absyn_Constraint*_tmpBF9=_tmp332;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmpBFA,_tmpBF9,({const char*_tmp2CF="(different pointer bounds)";_tag_dyneither(_tmp2CF,sizeof(char),27U);}));});
# 1508
{void*_tmp2D0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32C);void*_tmp2D1=_tmp2D0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D1)->tag == 0U){_LL55: _LL56:
 return;}else{_LL57: _LL58:
 goto _LL54;}_LL54:;}
# 1512
({union Cyc_Absyn_Constraint*_tmpBFC=_tmp32D;union Cyc_Absyn_Constraint*_tmpBFB=_tmp333;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBFC,_tmpBFB,({const char*_tmp2D2="(incompatible pointer types)";_tag_dyneither(_tmp2D2,sizeof(char),29U);}));});
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 6U: switch(*((int*)_tmp2B6.f2)){case 6U: _LL24: _tmp33A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp339=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B6.f1)->f2;_tmp338=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_tmp337=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B6.f2)->f2;_LL25:
# 1517
 if(_tmp338 == _tmp33A  && ((_tmp337 == _tmp339  || _tmp337 == Cyc_Absyn_Int_sz  && _tmp339 == Cyc_Absyn_Long_sz) || 
_tmp337 == Cyc_Absyn_Long_sz  && _tmp339 == Cyc_Absyn_Int_sz))return;
({struct _dyneither_ptr _tmpBFD=({const char*_tmp2D3="(different integral types)";_tag_dyneither(_tmp2D3,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpBFD;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 7U: switch(*((int*)_tmp2B6.f2)){case 7U: _LL26: _tmp33C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp33B=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL27:
# 1523
 if(_tmp33B == 0  && _tmp33C == 0)return;else{
if(_tmp33B == 1  && _tmp33C == 1)return;else{
# 1526
if(((_tmp33B != 0  && _tmp33B != 1) && _tmp33C != 0) && _tmp33C != 1)return;}}
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 19U: switch(*((int*)_tmp2B6.f2)){case 19U: _LL28: _tmp33E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp33D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL29:
# 1529
 Cyc_Tcutil_unify_it(_tmp33E,_tmp33D);return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 18U: switch(*((int*)_tmp2B6.f2)){case 18U: _LL2A: _tmp340=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp33F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL2B:
# 1532
 if(!Cyc_Evexp_same_const_exp(_tmp340,_tmp33F)){
({struct _dyneither_ptr _tmpBFE=({const char*_tmp2D4="(cannot prove expressions are the same)";_tag_dyneither(_tmp2D4,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpBFE;});
goto _LL11;}
# 1536
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 8U: switch(*((int*)_tmp2B6.f2)){case 8U: _LL2C: _tmp348=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f1)->f1).elt_type;_tmp347=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f1)->f1).tq;_tmp346=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f1)->f1).num_elts;_tmp345=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f1)->f1).zero_term;_tmp344=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f2)->f1).elt_type;_tmp343=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f2)->f1).tq;_tmp342=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f2)->f1).num_elts;_tmp341=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B6.f2)->f1).zero_term;_LL2D:
# 1540
 Cyc_Tcutil_unify_it(_tmp344,_tmp348);
Cyc_Tcutil_unify_tqual(_tmp343,_tmp344,_tmp347,_tmp348);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpC00=_tmp345;union Cyc_Absyn_Constraint*_tmpBFF=_tmp341;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC00,_tmpBFF,({const char*_tmp2D5="(not both zero terminated)";_tag_dyneither(_tmp2D5,sizeof(char),27U);}));});
if(_tmp346 == _tmp342)return;
if(_tmp346 == 0  || _tmp342 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp346,_tmp342))
return;
({struct _dyneither_ptr _tmpC01=({const char*_tmp2D6="(different array sizes)";_tag_dyneither(_tmp2D6,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC01;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 9U: switch(*((int*)_tmp2B6.f2)){case 9U: _LL2E: _tmp362=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).tvars;_tmp361=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).effect;_tmp360=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ret_tqual;_tmp35F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ret_typ;_tmp35E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).args;_tmp35D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).c_varargs;_tmp35C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).cyc_varargs;_tmp35B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).rgn_po;_tmp35A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).attributes;_tmp359=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).requires_clause;_tmp358=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).requires_relns;_tmp357=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ensures_clause;_tmp356=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f1)->f1).ensures_relns;_tmp355=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).tvars;_tmp354=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).effect;_tmp353=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ret_tqual;_tmp352=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ret_typ;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).args;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).c_varargs;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).cyc_varargs;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).rgn_po;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).attributes;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).requires_clause;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).requires_relns;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ensures_clause;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B6.f2)->f1).ensures_relns;_LL2F: {
# 1554
int done=0;
{struct _RegionHandle _tmp2D7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2D7;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp355 != 0){
if(_tmp362 == 0){
({struct _dyneither_ptr _tmpC02=({const char*_tmp2D8="(second function type has too few type variables)";_tag_dyneither(_tmp2D8,sizeof(char),50U);});Cyc_Tcutil_failure_reason=_tmpC02;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1562
void*_tmp2D9=((struct Cyc_Absyn_Tvar*)_tmp355->hd)->kind;
void*_tmp2DA=((struct Cyc_Absyn_Tvar*)_tmp362->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2D9,_tmp2DA)){
({struct _dyneither_ptr _tmpC07=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DD=({struct Cyc_String_pa_PrintArg_struct _tmpA59;_tmpA59.tag=0U,({
struct _dyneither_ptr _tmpC03=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp355->hd));_tmpA59.f1=_tmpC03;});_tmpA59;});struct Cyc_String_pa_PrintArg_struct _tmp2DE=({struct Cyc_String_pa_PrintArg_struct _tmpA58;_tmpA58.tag=0U,({
struct _dyneither_ptr _tmpC04=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp355->hd,& Cyc_Tcutil_bk)));_tmpA58.f1=_tmpC04;});_tmpA58;});struct Cyc_String_pa_PrintArg_struct _tmp2DF=({struct Cyc_String_pa_PrintArg_struct _tmpA57;_tmpA57.tag=0U,({
struct _dyneither_ptr _tmpC05=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp362->hd,& Cyc_Tcutil_bk)));_tmpA57.f1=_tmpC05;});_tmpA57;});void*_tmp2DB[3U];_tmp2DB[0]=& _tmp2DD,_tmp2DB[1]=& _tmp2DE,_tmp2DB[2]=& _tmp2DF;({struct _dyneither_ptr _tmpC06=({const char*_tmp2DC="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2DC,sizeof(char),44U);});Cyc_aprintf(_tmpC06,_tag_dyneither(_tmp2DB,sizeof(void*),3U));});});
# 1565
Cyc_Tcutil_failure_reason=_tmpC07;});
# 1569
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1571
({struct Cyc_List_List*_tmpC0A=({struct Cyc_List_List*_tmp2E2=_region_malloc(rgn,sizeof(*_tmp2E2));({struct _tuple16*_tmpC09=({struct _tuple16*_tmp2E1=_region_malloc(rgn,sizeof(*_tmp2E1));_tmp2E1->f1=(struct Cyc_Absyn_Tvar*)_tmp362->hd,({void*_tmpC08=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2E0=_cycalloc(sizeof(*_tmp2E0));_tmp2E0->tag=2U,_tmp2E0->f1=(struct Cyc_Absyn_Tvar*)_tmp355->hd;_tmp2E0;});_tmp2E1->f2=_tmpC08;});_tmp2E1;});_tmp2E2->hd=_tmpC09;}),_tmp2E2->tl=inst;_tmp2E2;});inst=_tmpC0A;});
_tmp355=_tmp355->tl;
_tmp362=_tmp362->tl;};}
# 1575
if(_tmp362 != 0){
({struct _dyneither_ptr _tmpC0B=({const char*_tmp2E3="(second function type has too many type variables)";_tag_dyneither(_tmp2E3,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpC0B;});
_npop_handler(0U);goto _LL11;}
# 1579
if(inst != 0){
({void*_tmpC0E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E4=_cycalloc(sizeof(*_tmp2E4));_tmp2E4->tag=9U,(_tmp2E4->f1).tvars=0,(_tmp2E4->f1).effect=_tmp354,(_tmp2E4->f1).ret_tqual=_tmp353,(_tmp2E4->f1).ret_typ=_tmp352,(_tmp2E4->f1).args=_tmp351,(_tmp2E4->f1).c_varargs=_tmp350,(_tmp2E4->f1).cyc_varargs=_tmp34F,(_tmp2E4->f1).rgn_po=_tmp34E,(_tmp2E4->f1).attributes=_tmp34D,(_tmp2E4->f1).requires_clause=_tmp359,(_tmp2E4->f1).requires_relns=_tmp358,(_tmp2E4->f1).ensures_clause=_tmp357,(_tmp2E4->f1).ensures_relns=_tmp356;_tmp2E4;});Cyc_Tcutil_unify_it(_tmpC0E,({
# 1583
struct _RegionHandle*_tmpC0D=rgn;struct Cyc_List_List*_tmpC0C=inst;Cyc_Tcutil_rsubstitute(_tmpC0D,_tmpC0C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));
_tmp2E5->tag=9U,(_tmp2E5->f1).tvars=0,(_tmp2E5->f1).effect=_tmp361,(_tmp2E5->f1).ret_tqual=_tmp360,(_tmp2E5->f1).ret_typ=_tmp35F,(_tmp2E5->f1).args=_tmp35E,(_tmp2E5->f1).c_varargs=_tmp35D,(_tmp2E5->f1).cyc_varargs=_tmp35C,(_tmp2E5->f1).rgn_po=_tmp35B,(_tmp2E5->f1).attributes=_tmp35A,(_tmp2E5->f1).requires_clause=_tmp34C,(_tmp2E5->f1).requires_relns=_tmp34B,(_tmp2E5->f1).ensures_clause=_tmp34A,(_tmp2E5->f1).ensures_relns=_tmp349;_tmp2E5;}));}));});
# 1588
done=1;}}
# 1556
;_pop_region(rgn);}
# 1591
if(done)
return;
Cyc_Tcutil_unify_it(_tmp352,_tmp35F);
Cyc_Tcutil_unify_tqual(_tmp353,_tmp352,_tmp360,_tmp35F);
for(0;_tmp351 != 0  && _tmp35E != 0;(_tmp351=_tmp351->tl,_tmp35E=_tmp35E->tl)){
struct _tuple10 _tmp2E6=*((struct _tuple10*)_tmp351->hd);struct _tuple10 _tmp2E7=_tmp2E6;struct Cyc_Absyn_Tqual _tmp2ED;void*_tmp2EC;_LL5A: _tmp2ED=_tmp2E7.f2;_tmp2EC=_tmp2E7.f3;_LL5B:;{
struct _tuple10 _tmp2E8=*((struct _tuple10*)_tmp35E->hd);struct _tuple10 _tmp2E9=_tmp2E8;struct Cyc_Absyn_Tqual _tmp2EB;void*_tmp2EA;_LL5D: _tmp2EB=_tmp2E9.f2;_tmp2EA=_tmp2E9.f3;_LL5E:;
Cyc_Tcutil_unify_it(_tmp2EC,_tmp2EA);
Cyc_Tcutil_unify_tqual(_tmp2ED,_tmp2EC,_tmp2EB,_tmp2EA);};}
# 1601
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp351 != 0  || _tmp35E != 0){
({struct _dyneither_ptr _tmpC0F=({const char*_tmp2EE="(function types have different number of arguments)";_tag_dyneither(_tmp2EE,sizeof(char),52U);});Cyc_Tcutil_failure_reason=_tmpC0F;});
goto _LL11;}
# 1607
if(_tmp350 != _tmp35D){
({struct _dyneither_ptr _tmpC10=({const char*_tmp2EF="(only one function type takes C varargs)";_tag_dyneither(_tmp2EF,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC10;});
goto _LL11;}{
# 1612
int bad_cyc_vararg=0;
{struct _tuple22 _tmp2F0=({struct _tuple22 _tmpA5A;_tmpA5A.f1=_tmp34F,_tmpA5A.f2=_tmp35C;_tmpA5A;});struct _tuple22 _tmp2F1=_tmp2F0;struct _dyneither_ptr*_tmp2FB;struct Cyc_Absyn_Tqual _tmp2FA;void*_tmp2F9;int _tmp2F8;struct _dyneither_ptr*_tmp2F7;struct Cyc_Absyn_Tqual _tmp2F6;void*_tmp2F5;int _tmp2F4;if(_tmp2F1.f1 == 0){if(_tmp2F1.f2 == 0){_LL60: _LL61:
 goto _LL5F;}else{_LL62: _LL63:
 goto _LL65;}}else{if(_tmp2F1.f2 == 0){_LL64: _LL65:
# 1617
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC11=({const char*_tmp2F2="(only one function type takes varargs)";_tag_dyneither(_tmp2F2,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC11;});
goto _LL5F;}else{_LL66: _tmp2FB=(_tmp2F1.f1)->name;_tmp2FA=(_tmp2F1.f1)->tq;_tmp2F9=(_tmp2F1.f1)->type;_tmp2F8=(_tmp2F1.f1)->inject;_tmp2F7=(_tmp2F1.f2)->name;_tmp2F6=(_tmp2F1.f2)->tq;_tmp2F5=(_tmp2F1.f2)->type;_tmp2F4=(_tmp2F1.f2)->inject;_LL67:
# 1621
 Cyc_Tcutil_unify_it(_tmp2F9,_tmp2F5);
Cyc_Tcutil_unify_tqual(_tmp2FA,_tmp2F9,_tmp2F6,_tmp2F5);
if(_tmp2F8 != _tmp2F4){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC12=({const char*_tmp2F3="(only one function type injects varargs)";_tag_dyneither(_tmp2F3,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC12;});}
# 1627
goto _LL5F;}}_LL5F:;}
# 1629
if(bad_cyc_vararg)goto _LL11;{
# 1632
int bad_effect=0;
{struct _tuple0 _tmp2FC=({struct _tuple0 _tmpA5B;_tmpA5B.f1=_tmp354,_tmpA5B.f2=_tmp361;_tmpA5B;});struct _tuple0 _tmp2FD=_tmp2FC;if(_tmp2FD.f1 == 0){if(_tmp2FD.f2 == 0){_LL69: _LL6A:
 goto _LL68;}else{_LL6B: _LL6C:
 goto _LL6E;}}else{if(_tmp2FD.f2 == 0){_LL6D: _LL6E:
 bad_effect=1;goto _LL68;}else{_LL6F: _LL70:
({int _tmpC14=!({void*_tmpC13=(void*)_check_null(_tmp354);Cyc_Tcutil_unify_effect(_tmpC13,(void*)_check_null(_tmp361));});bad_effect=_tmpC14;});goto _LL68;}}_LL68:;}
# 1639
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpC15=({const char*_tmp2FE="(function effects do not match)";_tag_dyneither(_tmp2FE,sizeof(char),32U);});Cyc_Tcutil_failure_reason=_tmpC15;});
goto _LL11;}
# 1645
if(!Cyc_Tcutil_same_atts(_tmp35A,_tmp34D)){
({struct _dyneither_ptr _tmpC16=({const char*_tmp2FF="(function types have different attributes)";_tag_dyneither(_tmp2FF,sizeof(char),43U);});Cyc_Tcutil_failure_reason=_tmpC16;});
goto _LL11;}
# 1649
if(!Cyc_Tcutil_same_rgn_po(_tmp35B,_tmp34E)){
({struct _dyneither_ptr _tmpC17=({const char*_tmp300="(function types have different region lifetime orderings)";_tag_dyneither(_tmp300,sizeof(char),58U);});Cyc_Tcutil_failure_reason=_tmpC17;});
goto _LL11;}
# 1653
if(!Cyc_Tcutil_check_logical_equivalence(_tmp358,_tmp34B)){
({struct _dyneither_ptr _tmpC18=({const char*_tmp301="(@requires clauses not equivalent)";_tag_dyneither(_tmp301,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC18;});
goto _LL11;}
# 1657
if(!Cyc_Tcutil_check_logical_equivalence(_tmp356,_tmp349)){
({struct _dyneither_ptr _tmpC19=({const char*_tmp302="(@ensures clauses not equivalent)";_tag_dyneither(_tmp302,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC19;});
goto _LL11;}
# 1661
return;};};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 10U: switch(*((int*)_tmp2B6.f2)){case 10U: _LL30: _tmp364=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp363=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL31:
# 1664
 for(0;_tmp363 != 0  && _tmp364 != 0;(_tmp363=_tmp363->tl,_tmp364=_tmp364->tl)){
struct _tuple12 _tmp303=*((struct _tuple12*)_tmp363->hd);struct _tuple12 _tmp304=_tmp303;struct Cyc_Absyn_Tqual _tmp30A;void*_tmp309;_LL72: _tmp30A=_tmp304.f1;_tmp309=_tmp304.f2;_LL73:;{
struct _tuple12 _tmp305=*((struct _tuple12*)_tmp364->hd);struct _tuple12 _tmp306=_tmp305;struct Cyc_Absyn_Tqual _tmp308;void*_tmp307;_LL75: _tmp308=_tmp306.f1;_tmp307=_tmp306.f2;_LL76:;
Cyc_Tcutil_unify_it(_tmp309,_tmp307);
Cyc_Tcutil_unify_tqual(_tmp30A,_tmp309,_tmp308,_tmp307);};}
# 1670
if(_tmp363 == 0  && _tmp364 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC1A=({const char*_tmp30B="(tuple types have different numbers of components)";_tag_dyneither(_tmp30B,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpC1A;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 12U: switch(*((int*)_tmp2B6.f2)){case 12U: _LL32: _tmp368=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp367=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B6.f1)->f2;_tmp366=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_tmp365=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B6.f2)->f2;_LL33:
# 1677
 if(_tmp366 != _tmp368){({struct _dyneither_ptr _tmpC1B=({const char*_tmp30C="(struct and union type)";_tag_dyneither(_tmp30C,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC1B;});goto _LL11;}
for(0;_tmp365 != 0  && _tmp367 != 0;(_tmp365=_tmp365->tl,_tmp367=_tmp367->tl)){
struct Cyc_Absyn_Aggrfield*_tmp30D=(struct Cyc_Absyn_Aggrfield*)_tmp365->hd;
struct Cyc_Absyn_Aggrfield*_tmp30E=(struct Cyc_Absyn_Aggrfield*)_tmp367->hd;
if(Cyc_strptrcmp(_tmp30D->name,_tmp30E->name)!= 0){
({struct _dyneither_ptr _tmpC1C=({const char*_tmp30F="(different member names)";_tag_dyneither(_tmp30F,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC1C;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1685
Cyc_Tcutil_unify_it(_tmp30D->type,_tmp30E->type);
Cyc_Tcutil_unify_tqual(_tmp30D->tq,_tmp30D->type,_tmp30E->tq,_tmp30E->type);
if(!Cyc_Tcutil_same_atts(_tmp30D->attributes,_tmp30E->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC1D=({const char*_tmp310="(different attributes on member)";_tag_dyneither(_tmp310,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpC1D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1693
if((_tmp30D->width != 0  && _tmp30E->width == 0  || 
_tmp30E->width != 0  && _tmp30D->width == 0) || 
(_tmp30D->width != 0  && _tmp30E->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpC1E=(struct Cyc_Absyn_Exp*)_check_null(_tmp30D->width);Cyc_Evexp_same_const_exp(_tmpC1E,(struct Cyc_Absyn_Exp*)_check_null(_tmp30E->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC1F=({const char*_tmp311="(different bitfield widths on member)";_tag_dyneither(_tmp311,sizeof(char),38U);});Cyc_Tcutil_failure_reason=_tmpC1F;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1702
if((_tmp30D->requires_clause != 0  && _tmp30E->requires_clause == 0  || 
_tmp30D->requires_clause == 0  && _tmp30E->requires_clause != 0) || 
(_tmp30D->requires_clause == 0  && _tmp30E->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpC20=(struct Cyc_Absyn_Exp*)_check_null(_tmp30D->requires_clause);Cyc_Evexp_same_const_exp(_tmpC20,(struct Cyc_Absyn_Exp*)_check_null(_tmp30E->requires_clause));})){
# 1707
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC21=({const char*_tmp312="(different @requires clauses on member)";_tag_dyneither(_tmp312,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpC21;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1713
if(_tmp365 == 0  && _tmp367 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC22=({const char*_tmp313="(different number of members)";_tag_dyneither(_tmp313,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpC22;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 17U: switch(*((int*)_tmp2B6.f2)){case 17U: _LL34: _tmp36C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B6.f1)->f2;_tmp36B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B6.f1)->f3;_tmp36A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B6.f2)->f2;_tmp369=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B6.f2)->f3;_LL35:
# 1719
 if(_tmp36B != _tmp369){
({struct _dyneither_ptr _tmpC23=({const char*_tmp314="(different abstract typedefs)";_tag_dyneither(_tmp314,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpC23;});
goto _LL11;}
# 1723
({struct _dyneither_ptr _tmpC24=({const char*_tmp315="(type parameters to typedef differ)";_tag_dyneither(_tmp315,sizeof(char),36U);});Cyc_Tcutil_failure_reason=_tmpC24;});
Cyc_Tcutil_unify_list(_tmp36C,_tmp36A);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 20U: switch(*((int*)_tmp2B6.f2)){case 20U: _LL36: _LL37:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 21U: switch(*((int*)_tmp2B6.f2)){case 21U: _LL38: _LL39:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 22U: switch(*((int*)_tmp2B6.f2)){case 22U: _LL3A: _LL3B:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 15U: switch(*((int*)_tmp2B6.f2)){case 15U: _LL3C: _tmp36E=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp36D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_LL3D:
# 1730
 Cyc_Tcutil_unify_it(_tmp36E,_tmp36D);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 16U: switch(*((int*)_tmp2B6.f2)){case 16U: _LL3E: _tmp372=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B6.f1)->f1;_tmp371=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B6.f1)->f2;_tmp370=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B6.f2)->f1;_tmp36F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B6.f2)->f2;_LL3F:
# 1733
 Cyc_Tcutil_unify_it(_tmp372,_tmp370);
Cyc_Tcutil_unify_it(_tmp371,_tmp36F);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 24U: _LL40: _LL41:
 goto _LL43;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2B6.f2)->tag == 24U){_LL42: _LL43:
 goto _LL45;}else{switch(*((int*)_tmp2B6.f1)){case 23U: _LL44: _LL45:
 goto _LL47;case 25U: _LL46: _LL47:
 goto _LL49;default: switch(*((int*)_tmp2B6.f2)){case 25U: _LL48: _LL49:
 goto _LL4B;case 23U: _LL4A: _LL4B:
# 1742
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
({struct _dyneither_ptr _tmpC25=({const char*_tmp316="(effects don't unify)";_tag_dyneither(_tmp316,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpC25;});
goto _LL11;default: _LL4C: _LL4D:
 goto _LL11;}}}}_LL11:;}
# 1747
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1750
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpC27)(void*,void*)=cmp;void*_tmpC26=(void*)_check_null(a1);_tmpC27(_tmpC26,(void*)_check_null(a2));});}
# 1757
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp373=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp374=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp373,_tmp374);}
# 1764
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
({union Cyc_Absyn_Constraint*_tmpC28=Cyc_Absyn_compress_conref(x);x=_tmpC28;});
({union Cyc_Absyn_Constraint*_tmpC29=Cyc_Absyn_compress_conref(y);y=_tmpC29;});
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp375=x;void*_tmp37C;switch((((union Cyc_Absyn_Constraint*)_tmp375)->Eq_constr).tag){case 3U: _LL1: _LL2:
 return - 1;case 1U: _LL3: _tmp37C=(void*)(_tmp375->Eq_constr).val;_LL4: {
# 1771
union Cyc_Absyn_Constraint*_tmp376=y;void*_tmp379;switch((((union Cyc_Absyn_Constraint*)_tmp376)->Eq_constr).tag){case 3U: _LL8: _LL9:
 return 1;case 1U: _LLA: _tmp379=(void*)(_tmp376->Eq_constr).val;_LLB:
 return cmp(_tmp37C,_tmp379);default: _LLC: _LLD:
({void*_tmp377=0U;({struct _dyneither_ptr _tmpC2A=({const char*_tmp378="unify_conref: forward after compress(2)";_tag_dyneither(_tmp378,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpC2A,_tag_dyneither(_tmp377,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 1776
({void*_tmp37A=0U;({struct _dyneither_ptr _tmpC2B=({const char*_tmp37B="unify_conref: forward after compress";_tag_dyneither(_tmp37B,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpC2B,_tag_dyneither(_tmp37A,sizeof(void*),0U));});});}_LL0:;};}
# 1780
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp37D=tqt1;struct Cyc_Absyn_Tqual _tmp383;void*_tmp382;_LL1: _tmp383=_tmp37D->f1;_tmp382=_tmp37D->f2;_LL2:;{
struct _tuple12*_tmp37E=tqt2;struct Cyc_Absyn_Tqual _tmp381;void*_tmp380;_LL4: _tmp381=_tmp37E->f1;_tmp380=_tmp37E->f2;_LL5:;{
int _tmp37F=Cyc_Tcutil_tqual_cmp(_tmp383,_tmp381);
if(_tmp37F != 0)return _tmp37F;
return Cyc_Tcutil_typecmp(_tmp382,_tmp380);};};}
# 1788
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp384=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp384 != 0)return _tmp384;{
int _tmp385=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp385 != 0)return _tmp385;{
int _tmp386=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp386 != 0)return _tmp386;{
int _tmp387=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp387 != 0)return _tmp387;
({int _tmpC2C=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp387=_tmpC2C;});
if(_tmp387 != 0)return _tmp387;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1802
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp388=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp388 != 0)return _tmp388;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1808
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp389=t;switch(*((int*)_tmp389)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp389)->f1 == 0){_LLF: _LL10:
 return 7;}else{_LL11: _LL12:
 return 8;}case 8U: _LL13: _LL14:
 return 9;case 9U: _LL15: _LL16:
 return 10;case 10U: _LL17: _LL18:
 return 11;case 11U: _LL19: _LL1A:
 return 12;case 12U: _LL1B: _LL1C:
 return 14;case 13U: _LL1D: _LL1E:
 return 16;case 14U: _LL1F: _LL20:
 return 17;case 15U: _LL21: _LL22:
 return 18;case 17U: _LL23: _LL24:
 return 19;case 21U: _LL25: _LL26:
 return 20;case 20U: _LL27: _LL28:
 return 21;case 23U: _LL29: _LL2A:
 return 22;case 24U: _LL2B: _LL2C:
 return 23;case 25U: _LL2D: _LL2E:
 return 24;case 22U: _LL2F: _LL30:
 return 25;case 19U: _LL31: _LL32:
 return 26;case 16U: _LL33: _LL34:
 return 27;case 18U: _LL35: _LL36:
 return 28;case 26U: _LL37: _LL38:
 return 29;case 27U: _LL39: _LL3A:
 return 30;default: _LL3B: _LL3C:
 return 31;}_LL0:;}
# 1845
int Cyc_Tcutil_typecmp(void*t1,void*t2){
({void*_tmpC2D=Cyc_Tcutil_compress(t1);t1=_tmpC2D;});
({void*_tmpC2E=Cyc_Tcutil_compress(t2);t2=_tmpC2E;});
if(t1 == t2)return 0;{
int _tmp38A=({int _tmpC2F=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpC2F,Cyc_Tcutil_type_case_number(t2));});
if(_tmp38A != 0)
return _tmp38A;{
# 1854
struct _tuple0 _tmp38B=({struct _tuple0 _tmpA5D;_tmpA5D.f1=t2,_tmpA5D.f2=t1;_tmpA5D;});struct _tuple0 _tmp38C=_tmp38B;struct _dyneither_ptr _tmp401;struct _dyneither_ptr _tmp400;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;void*_tmp3FB;void*_tmp3FA;void*_tmp3F9;void*_tmp3F8;void*_tmp3F7;void*_tmp3F6;void*_tmp3F5;void*_tmp3F4;enum Cyc_Absyn_AggrKind _tmp3F3;struct Cyc_List_List*_tmp3F2;enum Cyc_Absyn_AggrKind _tmp3F1;struct Cyc_List_List*_tmp3F0;struct Cyc_List_List*_tmp3EF;struct Cyc_List_List*_tmp3EE;struct Cyc_Absyn_FnInfo _tmp3ED;struct Cyc_Absyn_FnInfo _tmp3EC;void*_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EA;struct Cyc_Absyn_Exp*_tmp3E9;union Cyc_Absyn_Constraint*_tmp3E8;void*_tmp3E7;struct Cyc_Absyn_Tqual _tmp3E6;struct Cyc_Absyn_Exp*_tmp3E5;union Cyc_Absyn_Constraint*_tmp3E4;int _tmp3E3;int _tmp3E2;enum Cyc_Absyn_Sign _tmp3E1;enum Cyc_Absyn_Size_of _tmp3E0;enum Cyc_Absyn_Sign _tmp3DF;enum Cyc_Absyn_Size_of _tmp3DE;void*_tmp3DD;struct Cyc_Absyn_Tqual _tmp3DC;void*_tmp3DB;union Cyc_Absyn_Constraint*_tmp3DA;union Cyc_Absyn_Constraint*_tmp3D9;union Cyc_Absyn_Constraint*_tmp3D8;void*_tmp3D7;struct Cyc_Absyn_Tqual _tmp3D6;void*_tmp3D5;union Cyc_Absyn_Constraint*_tmp3D4;union Cyc_Absyn_Constraint*_tmp3D3;union Cyc_Absyn_Constraint*_tmp3D2;struct Cyc_Absyn_Datatypedecl*_tmp3D1;struct Cyc_Absyn_Datatypefield*_tmp3D0;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Datatypedecl*_tmp3CE;struct Cyc_Absyn_Datatypefield*_tmp3CD;struct Cyc_List_List*_tmp3CC;struct Cyc_Absyn_Datatypedecl*_tmp3CB;struct Cyc_List_List*_tmp3CA;struct Cyc_Absyn_Datatypedecl*_tmp3C9;struct Cyc_List_List*_tmp3C8;struct Cyc_List_List*_tmp3C7;struct Cyc_List_List*_tmp3C6;struct _tuple2*_tmp3C5;struct _tuple2*_tmp3C4;union Cyc_Absyn_AggrInfoU _tmp3C3;struct Cyc_List_List*_tmp3C2;union Cyc_Absyn_AggrInfoU _tmp3C1;struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Tvar*_tmp3BF;struct Cyc_Absyn_Tvar*_tmp3BE;switch(*((int*)_tmp38C.f1)){case 1U: switch(*((int*)_tmp38C.f2)){case 1U: _LL1: _LL2:
# 1856
({void*_tmp38D=0U;({struct _dyneither_ptr _tmpC30=({const char*_tmp38E="typecmp: can only compare closed types";_tag_dyneither(_tmp38E,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC30,_tag_dyneither(_tmp38D,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 2U: switch(*((int*)_tmp38C.f2)){case 2U: _LL3: _tmp3BF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3BE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL4:
# 1860
 return Cyc_Core_intcmp(_tmp3BE->identity,_tmp3BF->identity);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 11U: switch(*((int*)_tmp38C.f2)){case 11U: _LL5: _tmp3C3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C.f1)->f1).aggr_info;_tmp3C2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C.f1)->f1).targs;_tmp3C1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C.f2)->f1).aggr_info;_tmp3C0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38C.f2)->f1).targs;_LL6: {
# 1864
struct _tuple13 _tmp38F=Cyc_Absyn_aggr_kinded_name(_tmp3C3);struct _tuple13 _tmp390=_tmp38F;struct _tuple2*_tmp395;_LL38: _tmp395=_tmp390.f2;_LL39:;{
struct _tuple13 _tmp391=Cyc_Absyn_aggr_kinded_name(_tmp3C1);struct _tuple13 _tmp392=_tmp391;struct _tuple2*_tmp394;_LL3B: _tmp394=_tmp392.f2;_LL3C:;{
int _tmp393=Cyc_Absyn_qvar_cmp(_tmp395,_tmp394);
if(_tmp393 != 0)return _tmp393;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3C2,_tmp3C0);}};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 13U: switch(*((int*)_tmp38C.f2)){case 13U: _LL7: _tmp3C5=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3C4=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL8:
# 1871
 return Cyc_Absyn_qvar_cmp(_tmp3C5,_tmp3C4);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 14U: switch(*((int*)_tmp38C.f2)){case 14U: _LL9: _tmp3C7=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3C6=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LLA:
# 1874
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3C7,_tmp3C6);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp38C.f2)){case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLB: _tmp3CB=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C.f1)->f1).datatype_info).KnownDatatype).val;_tmp3CA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C.f1)->f1).targs;_tmp3C9=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C.f2)->f1).datatype_info).KnownDatatype).val;_tmp3C8=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38C.f2)->f1).targs;_LLC:
# 1878
 if(_tmp3C9 == _tmp3CB)return 0;{
int _tmp396=Cyc_Absyn_qvar_cmp(_tmp3C9->name,_tmp3CB->name);
if(_tmp396 != 0)return _tmp396;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3C8,_tmp3CA);};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp38C.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp38C.f2)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LLD: _tmp3D1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp3D0=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp3CF=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f1)->f1).targs;_tmp3CE=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp3CD=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp3CC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38C.f2)->f1).targs;_LLE:
# 1885
 if(_tmp3CE == _tmp3D1)return 0;{
int _tmp397=Cyc_Absyn_qvar_cmp(_tmp3D1->name,_tmp3CE->name);
if(_tmp397 != 0)return _tmp397;{
int _tmp398=Cyc_Absyn_qvar_cmp(_tmp3D0->name,_tmp3CD->name);
if(_tmp398 != 0)return _tmp398;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3CC,_tmp3CF);};};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp38C.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 5U: switch(*((int*)_tmp38C.f2)){case 5U: _LLF: _tmp3DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f1)->f1).elt_typ;_tmp3DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f1)->f1).elt_tq;_tmp3DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f1)->f1).ptr_atts).rgn;_tmp3DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f1)->f1).ptr_atts).nullable;_tmp3D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f1)->f1).ptr_atts).bounds;_tmp3D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f1)->f1).ptr_atts).zero_term;_tmp3D7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f2)->f1).elt_typ;_tmp3D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f2)->f1).elt_tq;_tmp3D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f2)->f1).ptr_atts).rgn;_tmp3D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f2)->f1).ptr_atts).nullable;_tmp3D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f2)->f1).ptr_atts).bounds;_tmp3D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38C.f2)->f1).ptr_atts).zero_term;_LL10: {
# 1894
int _tmp399=Cyc_Tcutil_typecmp(_tmp3D7,_tmp3DD);
if(_tmp399 != 0)return _tmp399;{
int _tmp39A=Cyc_Tcutil_typecmp(_tmp3D5,_tmp3DB);
if(_tmp39A != 0)return _tmp39A;{
int _tmp39B=Cyc_Tcutil_tqual_cmp(_tmp3D6,_tmp3DC);
if(_tmp39B != 0)return _tmp39B;{
int _tmp39C=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp3D3,_tmp3D9);
if(_tmp39C != 0)return _tmp39C;{
int _tmp39D=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3D2,_tmp3D8);
if(_tmp39D != 0)return _tmp39D;
{void*_tmp39E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3D3);void*_tmp39F=_tmp39E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp39F)->tag == 0U){_LL3E: _LL3F:
 return 0;}else{_LL40: _LL41:
 goto _LL3D;}_LL3D:;}
# 1908
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3D4,_tmp3DA);};};};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 6U: switch(*((int*)_tmp38C.f2)){case 6U: _LL11: _tmp3E1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3E0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38C.f1)->f2;_tmp3DF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38C.f2)->f1;_tmp3DE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38C.f2)->f2;_LL12:
# 1911
 if(_tmp3DF != _tmp3E1)return Cyc_Core_intcmp((int)((unsigned int)_tmp3DF),(int)((unsigned int)_tmp3E1));
if(_tmp3DE != _tmp3E0)return Cyc_Core_intcmp((int)((unsigned int)_tmp3DE),(int)((unsigned int)_tmp3E0));
return 0;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 7U: switch(*((int*)_tmp38C.f2)){case 7U: _LL13: _tmp3E3=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3E2=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL14:
# 1916
 if(_tmp3E3 == _tmp3E2)return 0;else{
if(_tmp3E2 == 0)return - 1;else{
if(_tmp3E2 == 1  && _tmp3E3 == 0)return - 1;else{
return 1;}}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 8U: switch(*((int*)_tmp38C.f2)){case 8U: _LL15: _tmp3EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f1)->f1).elt_type;_tmp3EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f1)->f1).tq;_tmp3E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f1)->f1).num_elts;_tmp3E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f1)->f1).zero_term;_tmp3E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f2)->f1).elt_type;_tmp3E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f2)->f1).tq;_tmp3E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f2)->f1).num_elts;_tmp3E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38C.f2)->f1).zero_term;_LL16: {
# 1923
int _tmp3A0=Cyc_Tcutil_tqual_cmp(_tmp3E6,_tmp3EA);
if(_tmp3A0 != 0)return _tmp3A0;{
int _tmp3A1=Cyc_Tcutil_typecmp(_tmp3E7,_tmp3EB);
if(_tmp3A1 != 0)return _tmp3A1;{
int _tmp3A2=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E8,_tmp3E4);
if(_tmp3A2 != 0)return _tmp3A2;
if(_tmp3E9 == _tmp3E5)return 0;
if(_tmp3E9 == 0  || _tmp3E5 == 0)
({void*_tmp3A3=0U;({struct _dyneither_ptr _tmpC31=({const char*_tmp3A4="missing expression in array index";_tag_dyneither(_tmp3A4,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC31,_tag_dyneither(_tmp3A3,sizeof(void*),0U));});});
# 1933
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3E9,_tmp3E5);};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 9U: switch(*((int*)_tmp38C.f2)){case 9U: _LL17: _tmp3ED=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3EC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL18:
# 1936
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp3ED.ret_typ,_tmp3EC.ret_typ);
if(r != 0)return r;
({int _tmpC32=Cyc_Tcutil_tqual_cmp(_tmp3ED.ret_tqual,_tmp3EC.ret_tqual);r=_tmpC32;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp3A5=_tmp3ED.args;
struct Cyc_List_List*_tmp3A6=_tmp3EC.args;
for(0;_tmp3A5 != 0  && _tmp3A6 != 0;(_tmp3A5=_tmp3A5->tl,_tmp3A6=_tmp3A6->tl)){
struct _tuple10 _tmp3A7=*((struct _tuple10*)_tmp3A5->hd);struct _tuple10 _tmp3A8=_tmp3A7;struct Cyc_Absyn_Tqual _tmp3AE;void*_tmp3AD;_LL43: _tmp3AE=_tmp3A8.f2;_tmp3AD=_tmp3A8.f3;_LL44:;{
struct _tuple10 _tmp3A9=*((struct _tuple10*)_tmp3A6->hd);struct _tuple10 _tmp3AA=_tmp3A9;struct Cyc_Absyn_Tqual _tmp3AC;void*_tmp3AB;_LL46: _tmp3AC=_tmp3AA.f2;_tmp3AB=_tmp3AA.f3;_LL47:;
({int _tmpC33=Cyc_Tcutil_tqual_cmp(_tmp3AE,_tmp3AC);r=_tmpC33;});
if(r != 0)return r;
({int _tmpC34=Cyc_Tcutil_typecmp(_tmp3AD,_tmp3AB);r=_tmpC34;});
if(r != 0)return r;};}
# 1951
if(_tmp3A5 != 0)return 1;
if(_tmp3A6 != 0)return - 1;
if(_tmp3ED.c_varargs  && !_tmp3EC.c_varargs)return 1;
if(!_tmp3ED.c_varargs  && _tmp3EC.c_varargs)return - 1;
if(_tmp3ED.cyc_varargs != 0 & _tmp3EC.cyc_varargs == 0)return 1;
if(_tmp3ED.cyc_varargs == 0 & _tmp3EC.cyc_varargs != 0)return - 1;
if(_tmp3ED.cyc_varargs != 0 & _tmp3EC.cyc_varargs != 0){
({int _tmpC36=({struct Cyc_Absyn_Tqual _tmpC35=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3ED.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmpC35,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3EC.cyc_varargs))->tq);});r=_tmpC36;});
if(r != 0)return r;
({int _tmpC37=Cyc_Tcutil_typecmp((_tmp3ED.cyc_varargs)->type,(_tmp3EC.cyc_varargs)->type);r=_tmpC37;});
if(r != 0)return r;
if((_tmp3ED.cyc_varargs)->inject  && !(_tmp3EC.cyc_varargs)->inject)return 1;
if(!(_tmp3ED.cyc_varargs)->inject  && (_tmp3EC.cyc_varargs)->inject)return - 1;}
# 1965
({int _tmpC38=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp3ED.effect,_tmp3EC.effect);r=_tmpC38;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp3AF=_tmp3ED.rgn_po;
struct Cyc_List_List*_tmp3B0=_tmp3EC.rgn_po;
for(0;_tmp3AF != 0  && _tmp3B0 != 0;(_tmp3AF=_tmp3AF->tl,_tmp3B0=_tmp3B0->tl)){
struct _tuple0 _tmp3B1=*((struct _tuple0*)_tmp3AF->hd);struct _tuple0 _tmp3B2=_tmp3B1;void*_tmp3B8;void*_tmp3B7;_LL49: _tmp3B8=_tmp3B2.f1;_tmp3B7=_tmp3B2.f2;_LL4A:;{
struct _tuple0 _tmp3B3=*((struct _tuple0*)_tmp3B0->hd);struct _tuple0 _tmp3B4=_tmp3B3;void*_tmp3B6;void*_tmp3B5;_LL4C: _tmp3B6=_tmp3B4.f1;_tmp3B5=_tmp3B4.f2;_LL4D:;
({int _tmpC39=Cyc_Tcutil_typecmp(_tmp3B8,_tmp3B6);r=_tmpC39;});if(r != 0)return r;
({int _tmpC3A=Cyc_Tcutil_typecmp(_tmp3B7,_tmp3B5);r=_tmpC3A;});if(r != 0)return r;};}
# 1975
if(_tmp3AF != 0)return 1;
if(_tmp3B0 != 0)return - 1;
({int _tmpC3B=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3ED.requires_clause,_tmp3EC.requires_clause);r=_tmpC3B;});
if(r != 0)return r;
({int _tmpC3C=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3ED.ensures_clause,_tmp3EC.ensures_clause);r=_tmpC3C;});
if(r != 0)return r;
# 1983
({void*_tmp3B9=0U;({struct _dyneither_ptr _tmpC3D=({const char*_tmp3BA="typecmp: function type comparison should never get here!";_tag_dyneither(_tmp3BA,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC3D,_tag_dyneither(_tmp3B9,sizeof(void*),0U));});});};};};case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 10U: switch(*((int*)_tmp38C.f2)){case 10U: _LL19: _tmp3EF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3EE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL1A:
# 1986
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3EE,_tmp3EF);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 12U: switch(*((int*)_tmp38C.f2)){case 12U: _LL1B: _tmp3F3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3F2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C.f1)->f2;_tmp3F1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C.f2)->f1;_tmp3F0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C.f2)->f2;_LL1C:
# 1989
 if(_tmp3F1 != _tmp3F3){
if(_tmp3F1 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3F0,_tmp3F2);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 15U: switch(*((int*)_tmp38C.f2)){case 15U: _LL1D: _tmp3F5=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3F4=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL1E:
# 1994
 return Cyc_Tcutil_typecmp(_tmp3F5,_tmp3F4);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 16U: switch(*((int*)_tmp38C.f2)){case 16U: _LL1F: _tmp3F9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3F8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38C.f1)->f2;_tmp3F7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38C.f2)->f1;_tmp3F6=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38C.f2)->f2;_LL20: {
# 1996
int _tmp3BB=Cyc_Tcutil_typecmp(_tmp3F9,_tmp3F7);
if(_tmp3BB != 0)return _tmp3BB;else{
return Cyc_Tcutil_typecmp(_tmp3F8,_tmp3F6);}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 19U: switch(*((int*)_tmp38C.f2)){case 19U: _LL21: _tmp3FB=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3FA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL22:
 return Cyc_Tcutil_typecmp(_tmp3FB,_tmp3FA);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 18U: switch(*((int*)_tmp38C.f2)){case 18U: _LL23: _tmp3FD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3FC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL24:
 _tmp3FF=_tmp3FD;_tmp3FE=_tmp3FC;goto _LL26;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 27U: switch(*((int*)_tmp38C.f2)){case 27U: _LL25: _tmp3FF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp3FE=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL26:
# 2002
 return Cyc_Evexp_const_exp_cmp(_tmp3FF,_tmp3FE);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 28U: switch(*((int*)_tmp38C.f2)){case 28U: _LL27: _tmp401=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp38C.f1)->f1;_tmp400=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp38C.f2)->f1;_LL28:
# 2004
 return Cyc_strcmp((struct _dyneither_ptr)_tmp401,(struct _dyneither_ptr)_tmp400);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 24U: _LL29: _LL2A:
 goto _LL2C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp38C.f2)->tag == 24U){_LL2B: _LL2C:
 goto _LL2E;}else{switch(*((int*)_tmp38C.f1)){case 23U: _LL2D: _LL2E:
 goto _LL30;case 25U: _LL2F: _LL30:
 goto _LL32;default: switch(*((int*)_tmp38C.f2)){case 25U: _LL31: _LL32:
 goto _LL34;case 23U: _LL33: _LL34:
# 2012
 if(Cyc_Tcutil_unify(t1,t2))return 0;
if((int)t1 < (int)t2)return - 1;
return 1;default: _LL35: _LL36:
({void*_tmp3BC=0U;({struct _dyneither_ptr _tmpC3E=({const char*_tmp3BD="Unmatched case in typecmp";_tag_dyneither(_tmp3BD,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC3E,_tag_dyneither(_tmp3BC,sizeof(void*),0U));});});}}}}_LL0:;};};}
# 2019
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp402=Cyc_Tcutil_compress(t);void*_tmp403=_tmp402;switch(*((int*)_tmp403)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2031
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
({void*_tmpC3F=Cyc_Tcutil_compress(t1);t1=_tmpC3F;});
({void*_tmpC40=Cyc_Tcutil_compress(t2);t2=_tmpC40;});{
struct _tuple0 _tmp404=({struct _tuple0 _tmpA5E;_tmpA5E.f1=t1,_tmpA5E.f2=t2;_tmpA5E;});struct _tuple0 _tmp405=_tmp404;int _tmp407;int _tmp406;switch(*((int*)_tmp405.f1)){case 7U: switch(*((int*)_tmp405.f2)){case 7U: _LL1: _tmp407=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f1)->f1;_tmp406=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f2)->f1;_LL2:
# 2036
 return _tmp406 == 0  && _tmp407 != 0  || (_tmp406 == 1  && _tmp407 != 0) && _tmp407 != 1;case 6U: _LL3: _LL4:
 goto _LL6;case 19U: _LL5: _LL6:
 return 1;default: goto _LL21;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 return 0;}else{goto _LL9;}}else{_LL9: _LLA:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp405.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f2){case Cyc_Absyn_Int_sz: _LLB: _LLC:
# 2043
 goto _LLE;case Cyc_Absyn_Short_sz: _LL13: _LL14:
# 2048
 goto _LL16;case Cyc_Absyn_Char_sz: _LL19: _LL1A:
# 2051
 goto _LL1C;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f2)->f1 == 0){_LLF: _LL10:
# 2046
 goto _LL12;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp405.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f2){case Cyc_Absyn_Long_sz: _LLD: _LLE:
# 2044
 return 0;case Cyc_Absyn_Short_sz: _LL15: _LL16:
# 2049
 goto _LL18;case Cyc_Absyn_Char_sz: _LL1B: _LL1C:
# 2052
 goto _LL1E;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp405.f2)->f1 == 0){_LL11: _LL12:
# 2047
 goto _LL14;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f2 == Cyc_Absyn_Char_sz){_LL1D: _LL1E:
# 2053
 goto _LL20;}else{goto _LL21;}}else{goto _LL21;}default: goto _LL21;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp405.f2)->f2){case Cyc_Absyn_Short_sz: _LL17: _LL18:
# 2050
 goto _LL1A;case Cyc_Absyn_Char_sz: _LL1F: _LL20:
# 2054
 return 1;default: goto _LL21;}else{goto _LL21;}default: _LL21: _LL22:
# 2056
 return 0;}_LL0:;};}
# 2062
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2065
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
({struct Cyc_Core_Opt*_tmpC41=({struct Cyc_Core_Opt*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->v=t1;_tmp408;});max_arith_type=_tmpC41;});}}}
# 2074
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2078
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmpA60;_tmpA60.tag=0U,({
struct _dyneither_ptr _tmpC42=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA60.f1=_tmpC42;});_tmpA60;});struct Cyc_String_pa_PrintArg_struct _tmp40C=({struct Cyc_String_pa_PrintArg_struct _tmpA5F;_tmpA5F.tag=0U,({struct _dyneither_ptr _tmpC43=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmpA5F.f1=_tmpC43;});_tmpA5F;});void*_tmp409[2U];_tmp409[0]=& _tmp40B,_tmp409[1]=& _tmp40C;({unsigned int _tmpC45=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpC44=({const char*_tmp40A="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp40A,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpC45,_tmpC44,_tag_dyneither(_tmp409,sizeof(void*),2U));});});
return 0;}}}
# 2084
return 1;}
# 2089
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp40D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp40E=_tmp40D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->tag == 5U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2095
return 1;}
# 2098
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp40F=Cyc_Tcutil_compress(t);void*_tmp410=_tmp40F;switch(*((int*)_tmp410)){case 6U: _LL1: _LL2:
 goto _LL4;case 19U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2109
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2113
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp411=0U;({unsigned int _tmpC47=e->loc;struct _dyneither_ptr _tmpC46=({const char*_tmp412="integral size mismatch; conversion supplied";_tag_dyneither(_tmp412,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpC47,_tmpC46,_tag_dyneither(_tmp411,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2119
return 0;}
# 2123
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2127
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp413=0U;({unsigned int _tmpC49=e->loc;struct _dyneither_ptr _tmpC48=({const char*_tmp414="integral size mismatch; conversion supplied";_tag_dyneither(_tmp414,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpC49,_tmpC48,_tag_dyneither(_tmp413,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2133
return 0;}
# 2138
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2146
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2148
({void*_tmpC4A=Cyc_Tcutil_compress(t1);t1=_tmpC4A;});
({void*_tmpC4B=Cyc_Tcutil_compress(t2);t2=_tmpC4B;});{
struct _tuple0 _tmp415=({struct _tuple0 _tmpA64;_tmpA64.f1=t1,_tmpA64.f2=t2;_tmpA64;});struct _tuple0 _tmp416=_tmp415;void*_tmp42C;struct Cyc_Absyn_Tqual _tmp42B;struct Cyc_Absyn_Exp*_tmp42A;union Cyc_Absyn_Constraint*_tmp429;void*_tmp428;struct Cyc_Absyn_Tqual _tmp427;struct Cyc_Absyn_Exp*_tmp426;union Cyc_Absyn_Constraint*_tmp425;struct Cyc_Absyn_PtrInfo _tmp424;struct Cyc_Absyn_PtrInfo _tmp423;switch(*((int*)_tmp416.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416.f2)->tag == 5U){_LL1: _tmp424=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416.f1)->f1;_tmp423=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416.f2)->f1;_LL2: {
# 2152
int okay=1;
# 2154
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp424.ptr_atts).nullable,(_tmp423.ptr_atts).nullable))
# 2157
({int _tmpC4C=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp424.ptr_atts).nullable);okay=_tmpC4C;});
# 2159
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp424.ptr_atts).bounds,(_tmp423.ptr_atts).bounds)){
# 2162
struct _tuple0 _tmp417=({struct _tuple0 _tmpA61;({void*_tmpC4E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp424.ptr_atts).bounds);_tmpA61.f1=_tmpC4E;}),({
void*_tmpC4D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp423.ptr_atts).bounds);_tmpA61.f2=_tmpC4D;});_tmpA61;});
# 2162
struct _tuple0 _tmp418=_tmp417;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41B;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418.f1)->tag == 1U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp418.f2)->tag == 0U){_LLA: _LLB:
# 2164
 goto _LLD;}else{_LLE: _tmp41C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418.f1)->f1;_tmp41B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418.f2)->f1;_LLF:
# 2167
({int _tmpC4F=okay  && Cyc_Evexp_lte_const_exp(_tmp41B,_tmp41C);okay=_tmpC4F;});
# 2171
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp423.ptr_atts).zero_term))
({void*_tmp419=0U;({unsigned int _tmpC51=loc;struct _dyneither_ptr _tmpC50=({const char*_tmp41A="implicit cast to shorter array";_tag_dyneither(_tmp41A,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpC51,_tmpC50,_tag_dyneither(_tmp419,sizeof(void*),0U));});});
goto _LL9;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp418.f2)->tag == 0U){_LLC: _LLD:
# 2165
 okay=1;goto _LL9;}else{_LL10: _tmp41D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418.f2)->f1;_LL11:
# 2176
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp424.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp423.ptr_atts).bounds))
goto _LL9;
okay=0;
goto _LL9;}}_LL9:;}
# 2185
okay=okay  && (!(_tmp424.elt_tq).real_const  || (_tmp423.elt_tq).real_const);
# 2188
if(!Cyc_Tcutil_unify((_tmp424.ptr_atts).rgn,(_tmp423.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp424.ptr_atts).rgn,(_tmp423.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp420=({struct Cyc_String_pa_PrintArg_struct _tmpA63;_tmpA63.tag=0U,({
struct _dyneither_ptr _tmpC52=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp424.ptr_atts).rgn));_tmpA63.f1=_tmpC52;});_tmpA63;});struct Cyc_String_pa_PrintArg_struct _tmp421=({struct Cyc_String_pa_PrintArg_struct _tmpA62;_tmpA62.tag=0U,({
struct _dyneither_ptr _tmpC53=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp423.ptr_atts).rgn));_tmpA62.f1=_tmpC53;});_tmpA62;});void*_tmp41E[2U];_tmp41E[0]=& _tmp420,_tmp41E[1]=& _tmp421;({unsigned int _tmpC55=loc;struct _dyneither_ptr _tmpC54=({const char*_tmp41F="implicit cast from region %s to region %s";_tag_dyneither(_tmp41F,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpC55,_tmpC54,_tag_dyneither(_tmp41E,sizeof(void*),2U));});});}else{
okay=0;}}
# 2198
({int _tmpC56=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp424.ptr_atts).zero_term,(_tmp423.ptr_atts).zero_term) || 
# 2201
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp424.ptr_atts).zero_term) && (_tmp423.elt_tq).real_const);
# 2198
okay=_tmpC56;});{
# 2209
int _tmp422=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp423.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2212
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp423.ptr_atts).zero_term);
# 2216
({int _tmpC57=okay  && (Cyc_Tcutil_unify(_tmp424.elt_typ,_tmp423.elt_typ) || 
(_tmp422  && ((_tmp423.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp423.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp424.elt_typ,_tmp423.elt_typ));
# 2216
okay=_tmpC57;});
# 2219
return okay;};}}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f2)->tag == 8U){_LL3: _tmp42C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f1)->f1).elt_type;_tmp42B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f1)->f1).tq;_tmp42A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f1)->f1).num_elts;_tmp429=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f1)->f1).zero_term;_tmp428=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f2)->f1).elt_type;_tmp427=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f2)->f1).tq;_tmp426=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f2)->f1).num_elts;_tmp425=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp416.f2)->f1).zero_term;_LL4: {
# 2223
int okay;
# 2226
({int _tmpC58=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp429,_tmp425) && (
(_tmp42A != 0  && _tmp426 != 0) && Cyc_Evexp_same_const_exp(_tmp42A,_tmp426));
# 2226
okay=_tmpC58;});
# 2229
return(okay  && Cyc_Tcutil_unify(_tmp42C,_tmp428)) && (!_tmp42B.real_const  || _tmp427.real_const);}}else{goto _LL7;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp416.f2)->tag == 6U){_LL5: _LL6:
# 2231
 return 0;}else{goto _LL7;}default: _LL7: _LL8:
# 2233
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2237
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp42D=Cyc_Tcutil_compress(t);void*_tmp42E=_tmp42D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42E)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2244
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp42F=Cyc_Tcutil_compress(t);void*_tmp430=_tmp42F;union Cyc_Absyn_Constraint*_tmp431;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->tag == 5U){_LL1: _tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp430)->f1).ptr_atts).zero_term;_LL2:
# 2247
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp431);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2252
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp432=Cyc_Tcutil_compress(t);void*_tmp433=_tmp432;union Cyc_Absyn_Constraint*_tmp434;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp433)->tag == 5U){_LL1: _tmp434=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp433)->f1).ptr_atts).nullable;_LL2:
# 2255
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp434);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2261
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp435=Cyc_Tcutil_compress(t);void*_tmp436=_tmp435;void*_tmp439;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->tag == 5U){_LL1: _tmp439=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp436)->f1).elt_typ;_LL2:
 return _tmp439;}else{_LL3: _LL4:
({void*_tmp437=0U;({struct _dyneither_ptr _tmpC59=({const char*_tmp438="pointer_elt_type";_tag_dyneither(_tmp438,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC59,_tag_dyneither(_tmp437,sizeof(void*),0U));});});}_LL0:;}
# 2267
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp43A=Cyc_Tcutil_compress(t);void*_tmp43B=_tmp43A;struct Cyc_Absyn_PtrAtts*_tmp43E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->tag == 5U){_LL1: _tmp43E=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->f1).ptr_atts;_LL2:
 return _tmp43E->rgn;}else{_LL3: _LL4:
({void*_tmp43C=0U;({struct _dyneither_ptr _tmpC5A=({const char*_tmp43D="pointer_elt_type";_tag_dyneither(_tmp43D,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5A,_tag_dyneither(_tmp43C,sizeof(void*),0U));});});}_LL0:;}
# 2274
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp43F=Cyc_Tcutil_compress(t);void*_tmp440=_tmp43F;void*_tmp441;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp440)->tag == 5U){_LL1: _tmp441=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp440)->f1).ptr_atts).rgn;_LL2:
# 2277
*rgn=_tmp441;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2284
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2288
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp442=Cyc_Tcutil_compress(t);void*_tmp443=_tmp442;union Cyc_Absyn_Constraint*_tmp444;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp443)->tag == 5U){_LL1: _tmp444=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp443)->f1).ptr_atts).bounds;_LL2:
# 2291
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp444)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2297
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp445=e->r;void*_tmp446=_tmp445;void*_tmp44A;struct Cyc_Absyn_Exp*_tmp449;struct _dyneither_ptr _tmp448;switch(*((int*)_tmp446)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp446)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp446)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp446)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp446)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp446)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp448=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp446)->f1).Wchar_c).val;_LLC: {
# 2306
unsigned long _tmp447=Cyc_strlen((struct _dyneither_ptr)_tmp448);
int i=0;
if(_tmp447 >= 2  && *((const char*)_check_dyneither_subscript(_tmp448,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp448,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp448.curr)[1]== 'x'  && _tmp447 >= 3) && *((const char*)_check_dyneither_subscript(_tmp448,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp447;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp448,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2316
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2304
 return 1;case 14U: _LLD: _tmp44A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp446)->f1;_tmp449=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp446)->f2;_LLE:
# 2317
 return Cyc_Tcutil_is_zero(_tmp449) && Cyc_Tcutil_admits_zero(_tmp44A);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2322
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2329
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2334
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2339
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2346
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2351
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2356
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp44B=ka;enum Cyc_Absyn_KindQual _tmp454;enum Cyc_Absyn_AliasQual _tmp453;_LL1: _tmp454=_tmp44B->kind;_tmp453=_tmp44B->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp44C=_tmp453;switch(_tmp44C){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2360
enum Cyc_Absyn_KindQual _tmp44D=_tmp454;switch(_tmp44D){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& Cyc_Tcutil_eko;default: _LL15: _LL16:
 return& Cyc_Tcutil_iko;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2369
{enum Cyc_Absyn_KindQual _tmp44E=_tmp454;switch(_tmp44E){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& Cyc_Tcutil_urko;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2376
goto _LL3;default: _LL8: _LL9:
# 2378
{enum Cyc_Absyn_KindQual _tmp44F=_tmp454;switch(_tmp44F){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& Cyc_Tcutil_trko;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2385
goto _LL3;}_LL3:;}
# 2387
({struct Cyc_String_pa_PrintArg_struct _tmp452=({struct Cyc_String_pa_PrintArg_struct _tmpA65;_tmpA65.tag=0U,({struct _dyneither_ptr _tmpC5B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmpA65.f1=_tmpC5B;});_tmpA65;});void*_tmp450[1U];_tmp450[0]=& _tmp452;({struct _dyneither_ptr _tmpC5C=({const char*_tmp451="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp451,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5C,_tag_dyneither(_tmp450,sizeof(void*),1U));});});}
# 2390
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0U,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0U,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0U,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0U,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0U,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0U,& Cyc_Tcutil_ik};
# 2398
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2405
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0U,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0U,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0U,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0U,& Cyc_Tcutil_trk};
# 2410
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2415
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0U,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0U,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0U,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0U,& Cyc_Tcutil_urk};
# 2420
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2425
struct Cyc_Absyn_Kind*_tmp455=ka;enum Cyc_Absyn_KindQual _tmp45E;enum Cyc_Absyn_AliasQual _tmp45D;_LL1: _tmp45E=_tmp455->kind;_tmp45D=_tmp455->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp456=_tmp45D;switch(_tmp456){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2428
enum Cyc_Absyn_KindQual _tmp457=_tmp45E;switch(_tmp457){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& ab;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& mb;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& bb;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& rb;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& eb;default: _LL15: _LL16:
 return& ib;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2437
{enum Cyc_Absyn_KindQual _tmp458=_tmp45E;switch(_tmp458){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& uab;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& umb;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& urb;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2444
goto _LL3;default: _LL8: _LL9:
# 2446
{enum Cyc_Absyn_KindQual _tmp459=_tmp45E;switch(_tmp459){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& tab;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& tmb;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& tbb;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& trb;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2453
goto _LL3;}_LL3:;}
# 2455
({struct Cyc_String_pa_PrintArg_struct _tmp45C=({struct Cyc_String_pa_PrintArg_struct _tmpA66;_tmpA66.tag=0U,({struct _dyneither_ptr _tmpC5D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmpA66.f1=_tmpC5D;});_tmpA66;});void*_tmp45A[1U];_tmp45A[0]=& _tmp45C;({struct _dyneither_ptr _tmpC5E=({const char*_tmp45B="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp45B,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5E,_tag_dyneither(_tmp45A,sizeof(void*),1U));});});}
# 2458
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2461
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2463
return({struct Cyc_Core_Opt*_tmp45F=_cycalloc(sizeof(*_tmp45F));({void*_tmpC5F=Cyc_Tcutil_kind_to_bound(k);_tmp45F->v=_tmpC5F;});_tmp45F;});}
# 2468
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpC60=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->tag=0U,_tmp460->f1=Cyc_Absyn_Null_c;_tmp460;});e1->r=_tmpC60;});{
struct Cyc_Core_Opt*_tmp461=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp462=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=5U,({void*_tmpC65=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp461);(_tmp467->f1).elt_typ=_tmpC65;}),({
struct Cyc_Absyn_Tqual _tmpC64=Cyc_Absyn_empty_tqual(0U);(_tmp467->f1).elt_tq=_tmpC64;}),
({void*_tmpC63=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp461);((_tmp467->f1).ptr_atts).rgn=_tmpC63;}),((_tmp467->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 2476
union Cyc_Absyn_Constraint*_tmpC62=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp467->f1).ptr_atts).bounds=_tmpC62;}),({
union Cyc_Absyn_Constraint*_tmpC61=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp467->f1).ptr_atts).zero_term=_tmpC61;}),((_tmp467->f1).ptr_atts).ptrloc=0;_tmp467;});
e1->topt=(void*)_tmp462;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp465=({struct Cyc_String_pa_PrintArg_struct _tmpA68;_tmpA68.tag=0U,({
struct _dyneither_ptr _tmpC66=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA68.f1=_tmpC66;});_tmpA68;});struct Cyc_String_pa_PrintArg_struct _tmp466=({struct Cyc_String_pa_PrintArg_struct _tmpA67;_tmpA67.tag=0U,({struct _dyneither_ptr _tmpC67=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmpA67.f1=_tmpC67;});_tmpA67;});void*_tmp463[2U];_tmp463[0]=& _tmp465,_tmp463[1]=& _tmp466;({struct _dyneither_ptr _tmpC68=({const char*_tmp464="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp464,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC68,_tag_dyneither(_tmp463,sizeof(void*),2U));});});
return retv;};};}
# 2486
return 0;}
# 2489
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp468=c;switch(_tmp468){case Cyc_Absyn_Unknown_coercion: _LL1: _LL2:
 return({const char*_tmp469="unknown";_tag_dyneither(_tmp469,sizeof(char),8U);});case Cyc_Absyn_No_coercion: _LL3: _LL4:
 return({const char*_tmp46A="no coercion";_tag_dyneither(_tmp46A,sizeof(char),12U);});case Cyc_Absyn_Null_to_NonNull: _LL5: _LL6:
 return({const char*_tmp46B="null check";_tag_dyneither(_tmp46B,sizeof(char),11U);});default: _LL7: _LL8:
 return({const char*_tmp46C="other coercion";_tag_dyneither(_tmp46C,sizeof(char),15U);});}_LL0:;}
# 2498
int Cyc_Tcutil_warn_alias_coerce=0;
# 2504
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2508
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpC6B=({struct _dyneither_ptr*_tmp480=_cycalloc(sizeof(*_tmp480));({struct _dyneither_ptr _tmpC6A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp47F=({struct Cyc_Int_pa_PrintArg_struct _tmpA6A;_tmpA6A.tag=1U,_tmpA6A.f1=(unsigned long)counter ++;_tmpA6A;});void*_tmp47D[1U];_tmp47D[0]=& _tmp47F;({struct _dyneither_ptr _tmpC69=({const char*_tmp47E="__aliasvar%d";_tag_dyneither(_tmp47E,sizeof(char),13U);});Cyc_aprintf(_tmpC69,_tag_dyneither(_tmp47D,sizeof(void*),1U));});});*_tmp480=_tmpC6A;});_tmp480;});_tmp481->f2=_tmpC6B;});_tmp481;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_typ,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpC6C=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->tag=4U,_tmp47C->f1=vd;_tmp47C;});Cyc_Absyn_varb_exp(_tmpC6C,e->loc);});
# 2517
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2519
{void*_tmp46D=Cyc_Tcutil_compress(e_typ);void*_tmp46E=_tmp46D;void*_tmp47B;struct Cyc_Absyn_Tqual _tmp47A;void*_tmp479;union Cyc_Absyn_Constraint*_tmp478;union Cyc_Absyn_Constraint*_tmp477;union Cyc_Absyn_Constraint*_tmp476;struct Cyc_Absyn_PtrLoc*_tmp475;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->tag == 5U){_LL1: _tmp47B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).elt_typ;_tmp47A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).elt_tq;_tmp479=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).ptr_atts).rgn;_tmp478=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).ptr_atts).nullable;_tmp477=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).ptr_atts).bounds;_tmp476=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).ptr_atts).zero_term;_tmp475=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).ptr_atts).ptrloc;_LL2:
# 2521
{void*_tmp46F=Cyc_Tcutil_compress(_tmp479);void*_tmp470=_tmp46F;void**_tmp474;struct Cyc_Core_Opt*_tmp473;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp470)->tag == 1U){_LL6: _tmp474=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp470)->f2;_tmp473=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp470)->f4;_LL7: {
# 2523
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp471=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->tag=2U,_tmp472->f1=tv;_tmp472;});
*_tmp474=(void*)_tmp471;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2528
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2532
e->topt=0;
vd->initializer=0;{
# 2536
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2538
return({struct _tuple15 _tmpA69;_tmpA69.f1=d,_tmpA69.f2=ve;_tmpA69;});};}
# 2543
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2546
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2551
void*_tmp482=Cyc_Tcutil_compress(wants_typ);void*_tmp483=_tmp482;void*_tmp487;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp483)->tag == 5U){_LL1: _tmp487=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp483)->f1).ptr_atts).rgn;_LL2: {
# 2553
void*_tmp484=Cyc_Tcutil_compress(_tmp487);void*_tmp485=_tmp484;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp485)->tag == 20U){_LL6: _LL7:
 return 0;}else{_LL8: _LL9: {
# 2556
struct Cyc_Absyn_Kind*_tmp486=Cyc_Tcutil_typ_kind(_tmp487);
return _tmp486->kind == Cyc_Absyn_RgnKind  && _tmp486->aliasqual == Cyc_Absyn_Aliasable;}}_LL5:;}}else{_LL3: _LL4:
# 2559
 return 0;}_LL0:;}
# 2562
return 0;}
# 2566
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2571
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2573
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2575
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp48A=({struct Cyc_String_pa_PrintArg_struct _tmpA6C;_tmpA6C.tag=0U,({
struct _dyneither_ptr _tmpC6D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA6C.f1=_tmpC6D;});_tmpA6C;});struct Cyc_String_pa_PrintArg_struct _tmp48B=({struct Cyc_String_pa_PrintArg_struct _tmpA6B;_tmpA6B.tag=0U,({struct _dyneither_ptr _tmpC6E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA6B.f1=_tmpC6E;});_tmpA6B;});void*_tmp488[2U];_tmp488[0]=& _tmp48A,_tmp488[1]=& _tmp48B;({unsigned int _tmpC70=e->loc;struct _dyneither_ptr _tmpC6F=({const char*_tmp489="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp489,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpC70,_tmpC6F,_tag_dyneither(_tmp488,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2583
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp48E=({struct Cyc_String_pa_PrintArg_struct _tmpA6F;_tmpA6F.tag=0U,({
struct _dyneither_ptr _tmpC71=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpA6F.f1=_tmpC71;});_tmpA6F;});struct Cyc_String_pa_PrintArg_struct _tmp48F=({struct Cyc_String_pa_PrintArg_struct _tmpA6E;_tmpA6E.tag=0U,({struct _dyneither_ptr _tmpC72=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA6E.f1=_tmpC72;});_tmpA6E;});struct Cyc_String_pa_PrintArg_struct _tmp490=({struct Cyc_String_pa_PrintArg_struct _tmpA6D;_tmpA6D.tag=0U,({struct _dyneither_ptr _tmpC73=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA6D.f1=_tmpC73;});_tmpA6D;});void*_tmp48C[3U];_tmp48C[0]=& _tmp48E,_tmp48C[1]=& _tmp48F,_tmp48C[2]=& _tmp490;({unsigned int _tmpC75=e->loc;struct _dyneither_ptr _tmpC74=({const char*_tmp48D="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp48D,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpC75,_tmpC74,_tag_dyneither(_tmp48C,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2590
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({enum Cyc_Absyn_Coercion _tmpC76=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpC76;})!= Cyc_Absyn_Unknown_coercion){
# 2598
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp493=({struct Cyc_String_pa_PrintArg_struct _tmpA71;_tmpA71.tag=0U,({
struct _dyneither_ptr _tmpC77=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA71.f1=_tmpC77;});_tmpA71;});struct Cyc_String_pa_PrintArg_struct _tmp494=({struct Cyc_String_pa_PrintArg_struct _tmpA70;_tmpA70.tag=0U,({struct _dyneither_ptr _tmpC78=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA70.f1=_tmpC78;});_tmpA70;});void*_tmp491[2U];_tmp491[0]=& _tmp493,_tmp491[1]=& _tmp494;({unsigned int _tmpC7A=e->loc;struct _dyneither_ptr _tmpC79=({const char*_tmp492="implicit cast from %s to %s";_tag_dyneither(_tmp492,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpC7A,_tmpC79,_tag_dyneither(_tmp491,sizeof(void*),2U));});});
return 1;}else{
# 2604
return 0;}}}}}
# 2611
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2614
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2618
int Cyc_Tcutil_coerceable(void*t){
void*_tmp495=Cyc_Tcutil_compress(t);void*_tmp496=_tmp495;switch(*((int*)_tmp496)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2636 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2640
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2643
struct _tuple23 _tmp497=*env;struct _tuple23 _tmp498=_tmp497;struct Cyc_List_List*_tmp4A0;struct _RegionHandle*_tmp49F;struct Cyc_Tcenv_Tenv*_tmp49E;int _tmp49D;_LL1: _tmp4A0=_tmp498.f1;_tmp49F=_tmp498.f2;_tmp49E=_tmp498.f3;_tmp49D=_tmp498.f4;_LL2:;{
# 2645
void*_tmp499=_tmp4A0 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp49F,_tmp4A0,x->type);
struct Cyc_List_List*_tmp49A=Cyc_Tcutil_flatten_typ(_tmp49F,_tmp49D,_tmp49E,_tmp499);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49A)== 1)
return({struct Cyc_List_List*_tmp49C=_region_malloc(_tmp49F,sizeof(*_tmp49C));({struct _tuple12*_tmpC7B=({struct _tuple12*_tmp49B=_region_malloc(_tmp49F,sizeof(*_tmp49B));_tmp49B->f1=x->tq,_tmp49B->f2=_tmp499;_tmp49B;});_tmp49C->hd=_tmpC7B;}),_tmp49C->tl=0;_tmp49C;});else{
return _tmp49A;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2651
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2653
struct _tuple24 _tmp4A1=*env;struct _tuple24 _tmp4A2=_tmp4A1;struct _RegionHandle*_tmp4AC;struct Cyc_Tcenv_Tenv*_tmp4AB;int _tmp4AA;_LL1: _tmp4AC=_tmp4A2.f1;_tmp4AB=_tmp4A2.f2;_tmp4AA=_tmp4A2.f3;_LL2:;{
struct _tuple12 _tmp4A3=*x;struct _tuple12 _tmp4A4=_tmp4A3;struct Cyc_Absyn_Tqual _tmp4A9;void*_tmp4A8;_LL4: _tmp4A9=_tmp4A4.f1;_tmp4A8=_tmp4A4.f2;_LL5:;{
struct Cyc_List_List*_tmp4A5=Cyc_Tcutil_flatten_typ(_tmp4AC,_tmp4AA,_tmp4AB,_tmp4A8);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4A5)== 1)
return({struct Cyc_List_List*_tmp4A7=_region_malloc(_tmp4AC,sizeof(*_tmp4A7));({struct _tuple12*_tmpC7C=({struct _tuple12*_tmp4A6=_region_malloc(_tmp4AC,sizeof(*_tmp4A6));_tmp4A6->f1=_tmp4A9,_tmp4A6->f2=_tmp4A8;_tmp4A6;});_tmp4A7->hd=_tmpC7C;}),_tmp4A7->tl=0;_tmp4A7;});else{
return _tmp4A5;}};};}
# 2660
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2664
if(flatten){
({void*_tmpC7D=Cyc_Tcutil_compress(t1);t1=_tmpC7D;});{
void*_tmp4AD=t1;struct Cyc_List_List*_tmp4CC;struct Cyc_Absyn_Aggrdecl*_tmp4CB;struct Cyc_List_List*_tmp4CA;struct Cyc_List_List*_tmp4C9;switch(*((int*)_tmp4AD)){case 0U: _LL1: _LL2:
 return 0;case 10U: _LL3: _tmp4C9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4AD)->f1;_LL4: {
# 2669
struct _tuple24 _tmp4AE=({struct _tuple24 _tmpA72;_tmpA72.f1=r,_tmpA72.f2=te,_tmpA72.f3=flatten;_tmpA72;});
# 2671
struct Cyc_List_List*_tmp4AF=_tmp4C9;struct _tuple12*_tmp4B6;struct Cyc_List_List*_tmp4B5;if(_tmp4AF == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp4B6=(struct _tuple12*)_tmp4AF->hd;_tmp4B5=_tmp4AF->tl;_LLF: {
# 2674
struct Cyc_List_List*_tmp4B0=Cyc_Tcutil_rcopy_tqt(& _tmp4AE,_tmp4B6);
_tmp4AE.f3=0;{
struct Cyc_List_List*_tmp4B1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp4AE,_tmp4C9);
struct Cyc_List_List*_tmp4B2=({struct Cyc_List_List*_tmp4B4=_region_malloc(r,sizeof(*_tmp4B4));_tmp4B4->hd=_tmp4B0,_tmp4B4->tl=_tmp4B1;_tmp4B4;});
return({struct _RegionHandle*_tmpC7E=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpC7E,({struct Cyc_List_List*_tmp4B3=_region_malloc(r,sizeof(*_tmp4B3));_tmp4B3->hd=_tmp4B0,_tmp4B3->tl=_tmp4B1;_tmp4B3;}));});};}}_LLB:;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AD)->f1).aggr_info).KnownAggr).tag == 2){_LL5: _tmp4CB=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AD)->f1).aggr_info).KnownAggr).val;_tmp4CA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AD)->f1).targs;_LL6:
# 2682
 if(((_tmp4CB->kind == Cyc_Absyn_UnionA  || _tmp4CB->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->rgn_po != 0)
# 2684
return({struct Cyc_List_List*_tmp4B8=_region_malloc(r,sizeof(*_tmp4B8));({struct _tuple12*_tmpC80=({struct _tuple12*_tmp4B7=_region_malloc(r,sizeof(*_tmp4B7));({struct Cyc_Absyn_Tqual _tmpC7F=Cyc_Absyn_empty_tqual(0U);_tmp4B7->f1=_tmpC7F;}),_tmp4B7->f2=t1;_tmp4B7;});_tmp4B8->hd=_tmpC80;}),_tmp4B8->tl=0;_tmp4B8;});{
struct Cyc_List_List*_tmp4B9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4CB->tvs,_tmp4CA);
struct _tuple23 env=({struct _tuple23 _tmpA73;_tmpA73.f1=_tmp4B9,_tmpA73.f2=r,_tmpA73.f3=te,_tmpA73.f4=flatten;_tmpA73;});
struct Cyc_List_List*_tmp4BA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->fields;struct Cyc_List_List*_tmp4BB=_tmp4BA;struct Cyc_Absyn_Aggrfield*_tmp4C1;struct Cyc_List_List*_tmp4C0;if(_tmp4BB == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp4C1=(struct Cyc_Absyn_Aggrfield*)_tmp4BB->hd;_tmp4C0=_tmp4BB->tl;_LL14: {
# 2690
struct Cyc_List_List*_tmp4BC=Cyc_Tcutil_flatten_typ_f(& env,_tmp4C1);
env.f4=0;{
struct Cyc_List_List*_tmp4BD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp4C0);
struct Cyc_List_List*_tmp4BE=({struct Cyc_List_List*_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF));_tmp4BF->hd=_tmp4BC,_tmp4BF->tl=_tmp4BD;_tmp4BF;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp4BE);};}}_LL10:;};}else{goto _LL9;}case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4AD)->f1 == Cyc_Absyn_StructA){_LL7: _tmp4CC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4AD)->f2;_LL8: {
# 2697
struct _tuple23 env=({struct _tuple23 _tmpA74;_tmpA74.f1=0,_tmpA74.f2=r,_tmpA74.f3=te,_tmpA74.f4=flatten;_tmpA74;});
struct Cyc_List_List*_tmp4C2=_tmp4CC;struct Cyc_Absyn_Aggrfield*_tmp4C8;struct Cyc_List_List*_tmp4C7;if(_tmp4C2 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp4C8=(struct Cyc_Absyn_Aggrfield*)_tmp4C2->hd;_tmp4C7=_tmp4C2->tl;_LL19: {
# 2701
struct Cyc_List_List*_tmp4C3=Cyc_Tcutil_flatten_typ_f(& env,_tmp4C8);
env.f4=0;{
struct Cyc_List_List*_tmp4C4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp4C7);
struct Cyc_List_List*_tmp4C5=({struct Cyc_List_List*_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6));_tmp4C6->hd=_tmp4C3,_tmp4C6->tl=_tmp4C4;_tmp4C6;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp4C5);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2707
 goto _LL0;}_LL0:;};}
# 2710
return({struct Cyc_List_List*_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE));({struct _tuple12*_tmpC82=({struct _tuple12*_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD));({struct Cyc_Absyn_Tqual _tmpC81=Cyc_Absyn_empty_tqual(0U);_tmp4CD->f1=_tmpC81;}),_tmp4CD->f2=t1;_tmp4CD;});_tmp4CE->hd=_tmpC82;}),_tmp4CE->tl=0;_tmp4CE;});}
# 2714
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp4CF=(void*)t->hd;void*_tmp4D0=_tmp4CF;switch(*((int*)_tmp4D0)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2720
 continue;default: _LL7: _LL8:
# 2722
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2725
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2728
return 1;}
# 2731
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp4D1=({struct _tuple0 _tmpA75;({void*_tmpC84=Cyc_Tcutil_compress(t1);_tmpA75.f1=_tmpC84;}),({void*_tmpC83=Cyc_Tcutil_compress(t2);_tmpA75.f2=_tmpC83;});_tmpA75;});struct _tuple0 _tmp4D2=_tmp4D1;enum Cyc_Absyn_Size_of _tmp4D4;enum Cyc_Absyn_Size_of _tmp4D3;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D2.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D2.f2)->tag == 6U){_LL1: _tmp4D4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D2.f1)->f2;_tmp4D3=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D2.f2)->f2;_LL2:
# 2734
 return(_tmp4D4 == _tmp4D3  || _tmp4D4 == Cyc_Absyn_Int_sz  && _tmp4D3 == Cyc_Absyn_Long_sz) || 
_tmp4D4 == Cyc_Absyn_Long_sz  && _tmp4D3 == Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2742
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2745
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
({void*_tmpC85=Cyc_Tcutil_compress(t1);t1=_tmpC85;});
({void*_tmpC86=Cyc_Tcutil_compress(t2);t2=_tmpC86;});{
struct _tuple0 _tmp4D5=({struct _tuple0 _tmpA77;_tmpA77.f1=t1,_tmpA77.f2=t2;_tmpA77;});struct _tuple0 _tmp4D6=_tmp4D5;struct Cyc_Absyn_FnInfo _tmp503;struct Cyc_Absyn_FnInfo _tmp502;struct Cyc_Absyn_Datatypedecl*_tmp501;struct Cyc_Absyn_Datatypefield*_tmp500;struct Cyc_List_List*_tmp4FF;struct Cyc_Absyn_Datatypedecl*_tmp4FE;struct Cyc_List_List*_tmp4FD;void*_tmp4FC;struct Cyc_Absyn_Tqual _tmp4FB;void*_tmp4FA;union Cyc_Absyn_Constraint*_tmp4F9;union Cyc_Absyn_Constraint*_tmp4F8;union Cyc_Absyn_Constraint*_tmp4F7;void*_tmp4F6;struct Cyc_Absyn_Tqual _tmp4F5;void*_tmp4F4;union Cyc_Absyn_Constraint*_tmp4F3;union Cyc_Absyn_Constraint*_tmp4F2;union Cyc_Absyn_Constraint*_tmp4F1;switch(*((int*)_tmp4D6.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f2)->tag == 5U){_LL1: _tmp4FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f1)->f1).elt_typ;_tmp4FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f1)->f1).elt_tq;_tmp4FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f1)->f1).ptr_atts).rgn;_tmp4F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f1)->f1).ptr_atts).nullable;_tmp4F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f1)->f1).ptr_atts).bounds;_tmp4F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f1)->f1).ptr_atts).zero_term;_tmp4F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f2)->f1).elt_typ;_tmp4F5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f2)->f1).elt_tq;_tmp4F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f2)->f1).ptr_atts).rgn;_tmp4F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f2)->f1).ptr_atts).nullable;_tmp4F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f2)->f1).ptr_atts).bounds;_tmp4F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6.f2)->f1).ptr_atts).zero_term;_LL2:
# 2757
 if(_tmp4FB.real_const  && !_tmp4F5.real_const)
return 0;
# 2760
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F9,_tmp4F3) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F9)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F3))
return 0;
# 2764
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F7,_tmp4F1) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F7)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F1))
return 0;
# 2768
if((!Cyc_Tcutil_unify(_tmp4FA,_tmp4F4) && !Cyc_Tcenv_region_outlives(te,_tmp4FA,_tmp4F4)) && !
Cyc_Tcutil_subtype(te,assume,_tmp4FA,_tmp4F4))
return 0;
# 2772
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4F8,_tmp4F2)){
struct _tuple0 _tmp4D7=({struct _tuple0 _tmpA76;({void*_tmpC88=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4F8);_tmpA76.f1=_tmpC88;}),({void*_tmpC87=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4F2);_tmpA76.f2=_tmpC87;});_tmpA76;});struct _tuple0 _tmp4D8=_tmp4D7;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D8.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D8.f2)->tag == 0U){_LLA: _LLB:
 goto _LL9;}else{goto _LLE;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D8.f2)->tag == 1U){_LLC: _tmp4DA=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D8.f1)->f1;_tmp4D9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D8.f2)->f1;_LLD:
# 2777
 if(!Cyc_Evexp_lte_const_exp(_tmp4D9,_tmp4DA))
return 0;
goto _LL9;}else{_LLE: _LLF:
 return 0;}}_LL9:;}
# 2785
if(!_tmp4F5.real_const  && _tmp4FB.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp4F6)))
return 0;}{
# 2791
int _tmp4DB=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4F2,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp4F1);
# 2797
return(_tmp4DB  && ({struct Cyc_Tcenv_Tenv*_tmpC8C=te;struct Cyc_List_List*_tmpC8B=({struct Cyc_List_List*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));({struct _tuple0*_tmpC89=({struct _tuple0*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->f1=t1,_tmp4DC->f2=t2;_tmp4DC;});_tmp4DD->hd=_tmpC89;}),_tmp4DD->tl=assume;_tmp4DD;});void*_tmpC8A=_tmp4FC;Cyc_Tcutil_ptrsubtype(_tmpC8C,_tmpC8B,_tmpC8A,_tmp4F6);}) || Cyc_Tcutil_unify(_tmp4FC,_tmp4F6)) || Cyc_Tcutil_isomorphic(_tmp4FC,_tmp4F6);};}else{goto _LL7;}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D6.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D6.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D6.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3: _tmp501=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D6.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp500=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D6.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp4FF=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D6.f1)->f1).targs;_tmp4FE=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D6.f2)->f1).datatype_info).KnownDatatype).val;_tmp4FD=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D6.f2)->f1).targs;_LL4:
# 2803
 if(_tmp501 != _tmp4FE  && Cyc_Absyn_qvar_cmp(_tmp501->name,_tmp4FE->name)!= 0)return 0;
# 2805
if(({int _tmpC8D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4FF);_tmpC8D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4FD);}))return 0;
for(0;_tmp4FF != 0;(_tmp4FF=_tmp4FF->tl,_tmp4FD=_tmp4FD->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4FF->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp4FD))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 9U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D6.f2)->tag == 9U){_LL5: _tmp503=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D6.f1)->f1;_tmp502=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D6.f2)->f1;_LL6:
# 2812
 if(_tmp503.tvars != 0  || _tmp502.tvars != 0){
struct Cyc_List_List*_tmp4DE=_tmp503.tvars;
struct Cyc_List_List*_tmp4DF=_tmp502.tvars;
if(({int _tmpC8E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4DE);_tmpC8E != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4DF);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp4DE != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp4DE->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpC91=({struct Cyc_List_List*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));({struct _tuple16*_tmpC90=({struct _tuple16*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->f1=(struct Cyc_Absyn_Tvar*)_tmp4DF->hd,({void*_tmpC8F=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->tag=2U,_tmp4E0->f1=(struct Cyc_Absyn_Tvar*)_tmp4DE->hd;_tmp4E0;});_tmp4E1->f2=_tmpC8F;});_tmp4E1;});_tmp4E2->hd=_tmpC90;}),_tmp4E2->tl=inst;_tmp4E2;});inst=_tmpC91;});
_tmp4DE=_tmp4DE->tl;
_tmp4DF=_tmp4DF->tl;}
# 2823
if(inst != 0){
_tmp503.tvars=0;
_tmp502.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpC94=te;struct Cyc_List_List*_tmpC93=assume;void*_tmpC92=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->tag=9U,_tmp4E3->f1=_tmp503;_tmp4E3;});Cyc_Tcutil_subtype(_tmpC94,_tmpC93,_tmpC92,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->tag=9U,_tmp4E4->f1=_tmp502;_tmp4E4;}));});}};}
# 2830
if(!Cyc_Tcutil_subtype(te,assume,_tmp503.ret_typ,_tmp502.ret_typ))return 0;{
struct Cyc_List_List*_tmp4E5=_tmp503.args;
struct Cyc_List_List*_tmp4E6=_tmp502.args;
# 2835
if(({int _tmpC95=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E5);_tmpC95 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E6);}))return 0;
# 2837
for(0;_tmp4E5 != 0;(_tmp4E5=_tmp4E5->tl,_tmp4E6=_tmp4E6->tl)){
struct _tuple10 _tmp4E7=*((struct _tuple10*)_tmp4E5->hd);struct _tuple10 _tmp4E8=_tmp4E7;struct Cyc_Absyn_Tqual _tmp4EE;void*_tmp4ED;_LL11: _tmp4EE=_tmp4E8.f2;_tmp4ED=_tmp4E8.f3;_LL12:;{
struct _tuple10 _tmp4E9=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd);struct _tuple10 _tmp4EA=_tmp4E9;struct Cyc_Absyn_Tqual _tmp4EC;void*_tmp4EB;_LL14: _tmp4EC=_tmp4EA.f2;_tmp4EB=_tmp4EA.f3;_LL15:;
# 2841
if(_tmp4EC.real_const  && !_tmp4EE.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp4EB,_tmp4ED))
return 0;};}
# 2845
if(_tmp503.c_varargs != _tmp502.c_varargs)return 0;
if(_tmp503.cyc_varargs != 0  && _tmp502.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp4EF=*_tmp503.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp4F0=*_tmp502.cyc_varargs;
# 2850
if((_tmp4F0.tq).real_const  && !(_tmp4EF.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp4F0.type,_tmp4EF.type))
return 0;}else{
if(_tmp503.cyc_varargs != 0  || _tmp502.cyc_varargs != 0)return 0;}
# 2855
if(!({void*_tmpC96=(void*)_check_null(_tmp503.effect);Cyc_Tcutil_subset_effect(1,_tmpC96,(void*)_check_null(_tmp502.effect));}))return 0;
# 2857
if(!Cyc_Tcutil_sub_rgnpo(_tmp503.rgn_po,_tmp502.rgn_po))return 0;
# 2859
if(!Cyc_Tcutil_sub_attributes(_tmp503.attributes,_tmp502.attributes))return 0;
# 2861
if(!Cyc_Tcutil_check_logical_implication(_tmp502.requires_relns,_tmp503.requires_relns))
return 0;
# 2864
if(!Cyc_Tcutil_check_logical_implication(_tmp503.ensures_relns,_tmp502.ensures_relns))
return 0;
# 2867
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2879 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2881
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp504=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp505=_tmp504;struct Cyc_Absyn_Tqual _tmp50B;void*_tmp50A;_LL1: _tmp50B=_tmp505->f1;_tmp50A=_tmp505->f2;_LL2:;{
struct _tuple12*_tmp506=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp507=_tmp506;struct Cyc_Absyn_Tqual _tmp509;void*_tmp508;_LL4: _tmp509=_tmp507->f1;_tmp508=_tmp507->f2;_LL5:;
# 2888
if(_tmp50B.real_const  && !_tmp509.real_const)return 0;
# 2890
if((_tmp509.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp508))) && 
Cyc_Tcutil_subtype(te,assume,_tmp50A,_tmp508))
# 2893
continue;
# 2895
if(Cyc_Tcutil_unify(_tmp50A,_tmp508))
# 2897
continue;
# 2899
if(Cyc_Tcutil_isomorphic(_tmp50A,_tmp508))
# 2901
continue;
# 2904
return 0;};};}
# 2906
return 1;}
# 2909
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp50C=Cyc_Tcutil_compress(t);void*_tmp50D=_tmp50C;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50D)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50D)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2918
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpC97=Cyc_Tcutil_compress(t1);t1=_tmpC97;});
({void*_tmpC98=Cyc_Tcutil_compress(t2);t2=_tmpC98;});
# 2924
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp50E=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50E)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50E)->f2){case Cyc_Absyn_Int_sz: _LL1: _LL2:
# 2928
 goto _LL4;case Cyc_Absyn_Long_sz: _LL3: _LL4:
# 2930
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 2934
void*_tmp50F=t1;void*_tmp536;struct Cyc_Absyn_Enumdecl*_tmp535;void*_tmp534;struct Cyc_Absyn_Tqual _tmp533;struct Cyc_Absyn_Exp*_tmp532;union Cyc_Absyn_Constraint*_tmp531;void*_tmp530;struct Cyc_Absyn_Tqual _tmp52F;void*_tmp52E;union Cyc_Absyn_Constraint*_tmp52D;union Cyc_Absyn_Constraint*_tmp52C;union Cyc_Absyn_Constraint*_tmp52B;switch(*((int*)_tmp50F)){case 5U: _LL8: _tmp530=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).elt_typ;_tmp52F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).elt_tq;_tmp52E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).rgn;_tmp52D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).nullable;_tmp52C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).bounds;_tmp52B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).zero_term;_LL9:
# 2943
{void*_tmp510=t2;void*_tmp521;struct Cyc_Absyn_Tqual _tmp520;void*_tmp51F;union Cyc_Absyn_Constraint*_tmp51E;union Cyc_Absyn_Constraint*_tmp51D;union Cyc_Absyn_Constraint*_tmp51C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->tag == 5U){_LL17: _tmp521=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->f1).elt_typ;_tmp520=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->f1).elt_tq;_tmp51F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->f1).ptr_atts).rgn;_tmp51E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->f1).ptr_atts).nullable;_tmp51D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->f1).ptr_atts).bounds;_tmp51C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->f1).ptr_atts).zero_term;_LL18: {
# 2947
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp511=({struct Cyc_List_List*_tmp51B=_cycalloc(sizeof(*_tmp51B));({struct _tuple0*_tmpC99=({struct _tuple0*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->f1=t1,_tmp51A->f2=t2;_tmp51A;});_tmp51B->hd=_tmpC99;}),_tmp51B->tl=0;_tmp51B;});
int _tmp512=_tmp520.real_const  || !_tmp52F.real_const;
# 2961 "tcutil.cyc"
int _tmp513=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp51D,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp51C);
# 2965
int _tmp514=_tmp512  && (
((_tmp513  && Cyc_Tcutil_ptrsubtype(te,_tmp511,_tmp530,_tmp521) || 
Cyc_Tcutil_unify(_tmp530,_tmp521)) || Cyc_Tcutil_isomorphic(_tmp530,_tmp521)) || Cyc_Tcutil_unify(_tmp521,(void*)& Cyc_Absyn_VoidType_val));
# 2969
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp52B,_tmp51C) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp51C);
# 2973
int _tmp515=_tmp514?0:((Cyc_Tcutil_bits_only(_tmp530) && Cyc_Tcutil_is_char_type(_tmp521)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp51C)) && (
_tmp520.real_const  || !_tmp52F.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp52C,_tmp51D);
if(!bounds_ok  && !_tmp515){
struct _tuple0 _tmp516=({struct _tuple0 _tmpA78;({void*_tmpC9B=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp52C);_tmpA78.f1=_tmpC9B;}),({void*_tmpC9A=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp51D);_tmpA78.f2=_tmpC9A;});_tmpA78;});struct _tuple0 _tmp517=_tmp516;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp518;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp517.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp517.f2)->tag == 1U){_LL1C: _tmp519=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp517.f1)->f1;_tmp518=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp517.f2)->f1;_LL1D:
# 2983
 if(Cyc_Evexp_lte_const_exp(_tmp518,_tmp519))
bounds_ok=1;
goto _LL1B;}else{goto _LL1E;}}else{_LL1E: _LL1F:
# 2988
 bounds_ok=1;goto _LL1B;}_LL1B:;}{
# 2990
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52D);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp51E);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2997
if(((bounds_ok  && zeroterm_ok) && (_tmp514  || _tmp515)) && (
Cyc_Tcutil_unify(_tmp52E,_tmp51F) || Cyc_Tcenv_region_outlives(te,_tmp52E,_tmp51F)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 3003
return Cyc_Absyn_Unknown_coercion;case 8U: _LLA: _tmp534=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50F)->f1).elt_type;_tmp533=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50F)->f1).tq;_tmp532=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50F)->f1).num_elts;_tmp531=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50F)->f1).zero_term;_LLB:
# 3005
{void*_tmp522=t2;void*_tmp526;struct Cyc_Absyn_Tqual _tmp525;struct Cyc_Absyn_Exp*_tmp524;union Cyc_Absyn_Constraint*_tmp523;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp522)->tag == 8U){_LL21: _tmp526=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp522)->f1).elt_type;_tmp525=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp522)->f1).tq;_tmp524=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp522)->f1).num_elts;_tmp523=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp522)->f1).zero_term;_LL22: {
# 3007
int okay;
({int _tmpC9C=
(((_tmp532 != 0  && _tmp524 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp531,_tmp523)) && 
Cyc_Evexp_lte_const_exp(_tmp524,_tmp532)) && 
Cyc_Evexp_lte_const_exp(_tmp532,_tmp524);
# 3008
okay=_tmpC9C;});
# 3012
return
# 3014
(okay  && Cyc_Tcutil_unify(_tmp534,_tmp526)) && (!_tmp533.real_const  || _tmp525.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL23: _LL24:
# 3016
 return Cyc_Absyn_Unknown_coercion;}_LL20:;}
# 3018
return Cyc_Absyn_Unknown_coercion;case 13U: _LLC: _tmp535=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp50F)->f2;_LLD:
# 3022
{void*_tmp527=t2;struct Cyc_Absyn_Enumdecl*_tmp528;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp527)->tag == 13U){_LL26: _tmp528=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp527)->f2;_LL27:
# 3024
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp535))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp528))->fields != 0) && ({
int _tmpC9D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp535->fields))->v);_tmpC9D >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp528->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL25;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}
# 3030
goto _LLF;case 6U: _LLE: _LLF:
 goto _LL11;case 7U: _LL10: _LL11:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15U: _LL12: _tmp536=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp50F)->f1;_LL13:
# 3035
{void*_tmp529=t2;void*_tmp52A;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp529)->tag == 15U){_LL2B: _tmp52A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp529)->f1;_LL2C:
# 3037
 if(Cyc_Tcenv_region_outlives(te,_tmp536,_tmp52A))return Cyc_Absyn_No_coercion;
goto _LL2A;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}
# 3041
return Cyc_Absyn_Unknown_coercion;default: _LL14: _LL15:
 return Cyc_Absyn_Unknown_coercion;}_LL7:;};}
# 3047
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp537=Cyc_Absyn_copy_exp(e);
({void*_tmpC9E=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->tag=14U,_tmp538->f1=t,_tmp538->f2=_tmp537,_tmp538->f3=0,_tmp538->f4=c;_tmp538;});e->r=_tmpC9E;});
e->topt=t;}}
# 3055
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp539=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp53A=_tmp539;switch(*((int*)_tmp53A)){case 6U: _LL1: _LL2:
 goto _LL4;case 13U: _LL3: _LL4:
 goto _LL6;case 14U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 return 1;case 1U: _LL9: _LLA:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3066
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp53B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp53C=_tmp53B;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp53C)->tag == 7U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 3075
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp53D=Cyc_Tcutil_compress(t);void*_tmp53E=_tmp53D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53E)->tag == 9U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 3082
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp53F=({struct _tuple0 _tmpA79;_tmpA79.f1=t1,_tmpA79.f2=t2;_tmpA79;});struct _tuple0 _tmp540=_tmp53F;int _tmp542;int _tmp541;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp540.f1)->tag == 7U){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp540.f2)->tag == 7U){_LL1: _tmp542=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp540.f1)->f1;_tmp541=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp540.f2)->f1;_LL2:
# 3085
 if(_tmp542 != 0  && _tmp542 != 1)return t1;else{
if(_tmp541 != 0  && _tmp541 != 1)return t2;else{
if(_tmp542 >= _tmp541)return t1;else{
return t2;}}}}else{_LL3: _LL4:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp540.f2)->tag == 7U){_LL5: _LL6:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 goto _LLA;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2 == Cyc_Absyn_Long_sz)goto _LLF;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL11;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL13;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL15;else{goto _LL1B;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2){case Cyc_Absyn_Long_sz: goto _LLF;case Cyc_Absyn_Int_sz: goto _LL13;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2){case Cyc_Absyn_Long_sz: _LLF: _LL10:
# 3095
 goto _LL12;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 3098
 goto _LL16;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL11;case Cyc_Absyn_Int_sz: goto _LL15;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{goto _LL1B;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f1)->f2){case Cyc_Absyn_LongLong_sz: _LLB: _LLC:
# 3093
 goto _LLE;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 3100
 goto _LL1A;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL9: _LLA:
# 3092
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL11: _LL12:
# 3096
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL15: _LL16:
# 3099
 return Cyc_Absyn_uint_typ;default: goto _LL1B;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp540.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLD: _LLE:
# 3094
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 3101
 return Cyc_Absyn_slong_typ;default: goto _LL1B;}}}else{_LL1B: _LL1C:
 return Cyc_Absyn_sint_typ;}}}}_LL0:;}
# 3113 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpC9F=Cyc_Absyn_compress_kb(c1);c1=_tmpC9F;});
({void*_tmpCA0=Cyc_Absyn_compress_kb(c2);c2=_tmpCA0;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp543=({struct _tuple0 _tmpA7A;_tmpA7A.f1=c1,_tmpA7A.f2=c2;_tmpA7A;});struct _tuple0 _tmp544=_tmp543;struct Cyc_Core_Opt**_tmp558;struct Cyc_Absyn_Kind*_tmp557;struct Cyc_Core_Opt**_tmp556;struct Cyc_Absyn_Kind*_tmp555;struct Cyc_Core_Opt**_tmp554;struct Cyc_Absyn_Kind*_tmp553;struct Cyc_Absyn_Kind*_tmp552;struct Cyc_Core_Opt**_tmp551;struct Cyc_Core_Opt**_tmp550;struct Cyc_Absyn_Kind*_tmp54F;struct Cyc_Core_Opt**_tmp54E;struct Cyc_Absyn_Kind*_tmp54D;struct Cyc_Absyn_Kind*_tmp54C;struct Cyc_Absyn_Kind*_tmp54B;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp544.f1)->tag == 0U)switch(*((int*)_tmp544.f2)){case 0U: _LL1: _tmp54C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp544.f1)->f1;_tmp54B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp544.f2)->f1;_LL2:
 return _tmp54C == _tmp54B;case 1U: goto _LL3;default: _LL9: _tmp54F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp544.f1)->f1;_tmp54E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f2)->f1;_tmp54D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f2)->f2;_LLA:
# 3126
 if(Cyc_Tcutil_kind_leq(_tmp54F,_tmp54D)){
({struct Cyc_Core_Opt*_tmpCA1=({struct Cyc_Core_Opt*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548->v=c1;_tmp548;});*_tmp54E=_tmpCA1;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp544.f2)->tag == 1U){_LL3: _tmp550=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp544.f2)->f1;_LL4:
# 3119
({struct Cyc_Core_Opt*_tmpCA2=({struct Cyc_Core_Opt*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->v=c1;_tmp545;});*_tmp550=_tmpCA2;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp544.f1)->tag == 1U){_LL5: _tmp551=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp544.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpCA3=({struct Cyc_Core_Opt*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->v=c2;_tmp546;});*_tmp551=_tmpCA3;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp544.f2)->tag == 0U){_LL7: _tmp554=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f1)->f1;_tmp553=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f1)->f2;_tmp552=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp544.f2)->f1;_LL8:
# 3122
 if(Cyc_Tcutil_kind_leq(_tmp552,_tmp553)){
({struct Cyc_Core_Opt*_tmpCA4=({struct Cyc_Core_Opt*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->v=c2;_tmp547;});*_tmp554=_tmpCA4;});return 1;}else{
return 0;}}else{_LLB: _tmp558=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f1)->f1;_tmp557=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f1)->f2;_tmp556=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f2)->f1;_tmp555=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544.f2)->f2;_LLC:
# 3130
 if(Cyc_Tcutil_kind_leq(_tmp557,_tmp555)){
({struct Cyc_Core_Opt*_tmpCA5=({struct Cyc_Core_Opt*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->v=c1;_tmp549;});*_tmp556=_tmpCA5;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp555,_tmp557)){
({struct Cyc_Core_Opt*_tmpCA6=({struct Cyc_Core_Opt*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->v=c2;_tmp54A;});*_tmp558=_tmpCA6;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3139
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3144
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp55E=({struct Cyc_Int_pa_PrintArg_struct _tmpA7B;_tmpA7B.tag=1U,_tmpA7B.f1=(unsigned long)i;_tmpA7B;});void*_tmp55C[1U];_tmp55C[0]=& _tmp55E;({struct _dyneither_ptr _tmpCA7=({const char*_tmp55D="#%d";_tag_dyneither(_tmp55D,sizeof(char),4U);});Cyc_aprintf(_tmpCA7,_tag_dyneither(_tmp55C,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp55B=_cycalloc(sizeof(*_tmp55B));({struct _dyneither_ptr*_tmpCA8=({unsigned int _tmp55A=1;struct _dyneither_ptr*_tmp559=_cycalloc(_check_times(_tmp55A,sizeof(struct _dyneither_ptr)));_tmp559[0]=s;_tmp559;});_tmp55B->name=_tmpCA8;}),_tmp55B->identity=- 1,_tmp55B->kind=k;_tmp55B;});}
# 3151
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp55F=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp55F,sizeof(char),0))== '#';}
# 3156
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp562=({struct Cyc_String_pa_PrintArg_struct _tmpA7C;_tmpA7C.tag=0U,_tmpA7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmpA7C;});void*_tmp560[1U];_tmp560[0]=& _tmp562;({struct _dyneither_ptr _tmpCA9=({const char*_tmp561="%s";_tag_dyneither(_tmp561,sizeof(char),3U);});Cyc_printf(_tmpCA9,_tag_dyneither(_tmp560,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp563=({struct _dyneither_ptr _tmpCAA=({const char*_tmp569="`";_tag_dyneither(_tmp569,sizeof(char),2U);});Cyc_strconcat(_tmpCAA,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp564=_dyneither_ptr_plus(_tmp563,sizeof(char),1);char _tmp565=*((char*)_check_dyneither_subscript(_tmp564,sizeof(char),0U));char _tmp566='t';if(_get_dyneither_size(_tmp564,sizeof(char))== 1U  && (_tmp565 == '\000'  && _tmp566 != '\000'))_throw_arraybounds();*((char*)_tmp564.curr)=_tmp566;});
({struct _dyneither_ptr*_tmpCAB=({unsigned int _tmp568=1;struct _dyneither_ptr*_tmp567=_cycalloc(_check_times(_tmp568,sizeof(struct _dyneither_ptr)));_tmp567[0]=(struct _dyneither_ptr)_tmp563;_tmp567;});t->name=_tmpCAB;});};}
# 3165
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3167
return({struct _tuple10*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->f1=(*x).f1,_tmp56A->f2=(*x).f2,_tmp56A->f3=(*x).f3;_tmp56A;});}
# 3170
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3177
struct Cyc_List_List*_tmp56B=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpCAC=({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->hd=(void*)atts->hd,_tmp56C->tl=_tmp56B;_tmp56C;});_tmp56B=_tmpCAC;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->tag=9U,(_tmp56D->f1).tvars=fd->tvs,(_tmp56D->f1).effect=fd->effect,(_tmp56D->f1).ret_tqual=fd->ret_tqual,(_tmp56D->f1).ret_typ=fd->ret_type,({
struct Cyc_List_List*_tmpCAD=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);(_tmp56D->f1).args=_tmpCAD;}),(_tmp56D->f1).c_varargs=fd->c_varargs,(_tmp56D->f1).cyc_varargs=fd->cyc_varargs,(_tmp56D->f1).rgn_po=fd->rgn_po,(_tmp56D->f1).attributes=_tmp56B,(_tmp56D->f1).requires_clause=fd->requires_clause,(_tmp56D->f1).requires_relns=fd->requires_relns,(_tmp56D->f1).ensures_clause=fd->ensures_clause,(_tmp56D->f1).ensures_relns=fd->ensures_relns;_tmp56D;});}
# 3190
return(void*)_check_null(fd->cached_typ);}
# 3196
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3198
union Cyc_Relations_RelnOp _tmp56E=*rop;union Cyc_Relations_RelnOp _tmp56F=_tmp56E;struct Cyc_Absyn_Vardecl*_tmp582;struct Cyc_Absyn_Vardecl*_tmp581;switch((_tmp56F.RNumelts).tag){case 2U: _LL1: _tmp581=(_tmp56F.RVar).val;_LL2: {
# 3200
struct _tuple2 _tmp570=*_tmp581->name;struct _tuple2 _tmp571=_tmp570;union Cyc_Absyn_Nmspace _tmp578;struct _dyneither_ptr*_tmp577;_LL8: _tmp578=_tmp571.f1;_tmp577=_tmp571.f2;_LL9:;
if(!((int)((_tmp578.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpCAE=(struct _dyneither_ptr)*_tmp577;Cyc_strcmp(_tmpCAE,({const char*_tmp572="return_value";_tag_dyneither(_tmp572,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpCAF=Cyc_Relations_RReturn();*rop=_tmpCAF;});
goto _LL0;}{
# 3206
unsigned int c=0U;
{struct Cyc_List_List*_tmp573=args;for(0;_tmp573 != 0;(_tmp573=_tmp573->tl,c ++)){
struct _tuple10*_tmp574=(struct _tuple10*)_tmp573->hd;struct _tuple10*_tmp575=_tmp574;struct _dyneither_ptr*_tmp576;_LLB: _tmp576=_tmp575->f1;_LLC:;
if(_tmp576 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp577,(struct _dyneither_ptr)*_tmp576)== 0){
({union Cyc_Relations_RelnOp _tmpCB0=Cyc_Relations_RParam(c);*rop=_tmpCB0;});
break;}}}}
# 3216
goto _LL0;};}case 3U: _LL3: _tmp582=(_tmp56F.RNumelts).val;_LL4: {
# 3218
struct _tuple2 _tmp579=*_tmp582->name;struct _tuple2 _tmp57A=_tmp579;union Cyc_Absyn_Nmspace _tmp580;struct _dyneither_ptr*_tmp57F;_LLE: _tmp580=_tmp57A.f1;_tmp57F=_tmp57A.f2;_LLF:;
if(!((int)((_tmp580.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp57B=args;for(0;_tmp57B != 0;(_tmp57B=_tmp57B->tl,c ++)){
struct _tuple10*_tmp57C=(struct _tuple10*)_tmp57B->hd;struct _tuple10*_tmp57D=_tmp57C;struct _dyneither_ptr*_tmp57E;_LL11: _tmp57E=_tmp57D->f1;_LL12:;
if(_tmp57E != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp57F,(struct _dyneither_ptr)*_tmp57E)== 0){
({union Cyc_Relations_RelnOp _tmpCB1=Cyc_Relations_RParamNumelts(c);*rop=_tmpCB1;});
break;}}}}
# 3230
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3235
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3237
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3241
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3243
if(e == 0)return 0;{
struct Cyc_List_List*_tmp583=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp583);
return _tmp583;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3250
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp584=pr;struct Cyc_Absyn_Tqual _tmp587;void*_tmp586;_LL1: _tmp587=_tmp584->f1;_tmp586=_tmp584->f2;_LL2:;
if(_tmp586 == t)return pr;else{
return({struct _tuple12*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->f1=_tmp587,_tmp585->f2=t;_tmp585;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3257
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3259
return({struct _tuple27*_tmp589=_region_malloc(rgn,sizeof(*_tmp589));({struct _tuple26*_tmpCB2=({struct _tuple26*_tmp588=_region_malloc(rgn,sizeof(*_tmp588));_tmp588->f1=(*y).f1,_tmp588->f2=(*y).f2;_tmp588;});_tmp589->f1=_tmpCB2;}),_tmp589->f2=(*y).f3;_tmp589;});}
# 3261
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3263
struct _tuple10 _tmp58A=*orig_arg;struct _tuple10 _tmp58B=_tmp58A;struct _dyneither_ptr*_tmp58F;struct Cyc_Absyn_Tqual _tmp58E;void*_tmp58D;_LL1: _tmp58F=_tmp58B.f1;_tmp58E=_tmp58B.f2;_tmp58D=_tmp58B.f3;_LL2:;
if(t == _tmp58D)return orig_arg;
return({struct _tuple10*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->f1=_tmp58F,_tmp58C->f2=_tmp58E,_tmp58C->f3=t;_tmp58C;});}
# 3267
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3270
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3275
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3277
return({struct Cyc_Absyn_Exp*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->topt=old->topt,_tmp590->r=r,_tmp590->loc=old->loc,_tmp590->annot=old->annot;_tmp590;});}
# 3282
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp591=e->r;void*_tmp592=_tmp591;void*_tmp5D2;void*_tmp5D1;struct Cyc_List_List*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5CE;void*_tmp5CD;void*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CB;int _tmp5CA;enum Cyc_Absyn_Coercion _tmp5C9;struct Cyc_Absyn_Exp*_tmp5C8;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C4;struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C1;struct Cyc_Absyn_Exp*_tmp5C0;enum Cyc_Absyn_Primop _tmp5BF;struct Cyc_List_List*_tmp5BE;switch(*((int*)_tmp592)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp5BF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_tmp5BE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp592)->f2;_LLC:
# 3291
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5BE)== 1){
struct Cyc_Absyn_Exp*_tmp593=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BE))->hd;
struct Cyc_Absyn_Exp*_tmp594=Cyc_Tcutil_rsubsexp(r,inst,_tmp593);
if(_tmp594 == _tmp593)return e;
return({struct Cyc_Absyn_Exp*_tmpCB4=e;Cyc_Tcutil_copye(_tmpCB4,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->tag=3U,_tmp596->f1=_tmp5BF,({struct Cyc_List_List*_tmpCB3=({struct Cyc_Absyn_Exp*_tmp595[1U];_tmp595[0]=_tmp594;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp595,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp596->f2=_tmpCB3;});_tmp596;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5BE)== 2){
struct Cyc_Absyn_Exp*_tmp597=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BE))->hd;
struct Cyc_Absyn_Exp*_tmp598=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BE->tl))->hd;
struct Cyc_Absyn_Exp*_tmp599=Cyc_Tcutil_rsubsexp(r,inst,_tmp597);
struct Cyc_Absyn_Exp*_tmp59A=Cyc_Tcutil_rsubsexp(r,inst,_tmp598);
if(_tmp599 == _tmp597  && _tmp59A == _tmp598)return e;
return({struct Cyc_Absyn_Exp*_tmpCB6=e;Cyc_Tcutil_copye(_tmpCB6,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->tag=3U,_tmp59C->f1=_tmp5BF,({struct Cyc_List_List*_tmpCB5=({struct Cyc_Absyn_Exp*_tmp59B[2U];_tmp59B[0]=_tmp599,_tmp59B[1]=_tmp59A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59B,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp59C->f2=_tmpCB5;});_tmp59C;}));});}else{
return({void*_tmp59D=0U;({struct _dyneither_ptr _tmpCB7=({const char*_tmp59E="primop does not have 1 or 2 args!";_tag_dyneither(_tmp59E,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB7,_tag_dyneither(_tmp59D,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp5C2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_tmp5C1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp592)->f2;_tmp5C0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp592)->f3;_LLE: {
# 3305
struct Cyc_Absyn_Exp*_tmp59F=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C2);
struct Cyc_Absyn_Exp*_tmp5A0=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C1);
struct Cyc_Absyn_Exp*_tmp5A1=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C0);
if((_tmp59F == _tmp5C2  && _tmp5A0 == _tmp5C1) && _tmp5A1 == _tmp5C0)return e;
return({struct Cyc_Absyn_Exp*_tmpCB8=e;Cyc_Tcutil_copye(_tmpCB8,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->tag=6U,_tmp5A2->f1=_tmp59F,_tmp5A2->f2=_tmp5A0,_tmp5A2->f3=_tmp5A1;_tmp5A2;}));});}case 7U: _LLF: _tmp5C4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_tmp5C3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp592)->f2;_LL10: {
# 3311
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C4);
struct Cyc_Absyn_Exp*_tmp5A4=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C3);
if(_tmp5A3 == _tmp5C4  && _tmp5A4 == _tmp5C3)return e;
return({struct Cyc_Absyn_Exp*_tmpCB9=e;Cyc_Tcutil_copye(_tmpCB9,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->tag=7U,_tmp5A5->f1=_tmp5A3,_tmp5A5->f2=_tmp5A4;_tmp5A5;}));});}case 8U: _LL11: _tmp5C6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_tmp5C5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp592)->f2;_LL12: {
# 3316
struct Cyc_Absyn_Exp*_tmp5A6=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C6);
struct Cyc_Absyn_Exp*_tmp5A7=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C5);
if(_tmp5A6 == _tmp5C6  && _tmp5A7 == _tmp5C5)return e;
return({struct Cyc_Absyn_Exp*_tmpCBA=e;Cyc_Tcutil_copye(_tmpCBA,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->tag=8U,_tmp5A8->f1=_tmp5A6,_tmp5A8->f2=_tmp5A7;_tmp5A8;}));});}case 9U: _LL13: _tmp5C8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_tmp5C7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp592)->f2;_LL14: {
# 3321
struct Cyc_Absyn_Exp*_tmp5A9=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C8);
struct Cyc_Absyn_Exp*_tmp5AA=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C7);
if(_tmp5A9 == _tmp5C8  && _tmp5AA == _tmp5C7)return e;
return({struct Cyc_Absyn_Exp*_tmpCBB=e;Cyc_Tcutil_copye(_tmpCBB,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->tag=9U,_tmp5AB->f1=_tmp5A9,_tmp5AB->f2=_tmp5AA;_tmp5AB;}));});}case 14U: _LL15: _tmp5CC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_tmp5CB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp592)->f2;_tmp5CA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp592)->f3;_tmp5C9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp592)->f4;_LL16: {
# 3326
struct Cyc_Absyn_Exp*_tmp5AC=Cyc_Tcutil_rsubsexp(r,inst,_tmp5CB);
void*_tmp5AD=Cyc_Tcutil_rsubstitute(r,inst,_tmp5CC);
if(_tmp5AC == _tmp5CB  && _tmp5AD == _tmp5CC)return e;
return({struct Cyc_Absyn_Exp*_tmpCBC=e;Cyc_Tcutil_copye(_tmpCBC,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->tag=14U,_tmp5AE->f1=_tmp5AD,_tmp5AE->f2=_tmp5AC,_tmp5AE->f3=_tmp5CA,_tmp5AE->f4=_tmp5C9;_tmp5AE;}));});}case 17U: _LL17: _tmp5CD=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_LL18: {
# 3331
void*_tmp5AF=Cyc_Tcutil_rsubstitute(r,inst,_tmp5CD);
if(_tmp5AF == _tmp5CD)return e;
return({struct Cyc_Absyn_Exp*_tmpCBD=e;Cyc_Tcutil_copye(_tmpCBD,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->tag=17U,_tmp5B0->f1=_tmp5AF;_tmp5B0;}));});}case 18U: _LL19: _tmp5CE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_LL1A: {
# 3335
struct Cyc_Absyn_Exp*_tmp5B1=Cyc_Tcutil_rsubsexp(r,inst,_tmp5CE);
if(_tmp5B1 == _tmp5CE)return e;
return({struct Cyc_Absyn_Exp*_tmpCBE=e;Cyc_Tcutil_copye(_tmpCBE,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2->tag=18U,_tmp5B2->f1=_tmp5B1;_tmp5B2;}));});}case 41U: _LL1B: _tmp5CF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_LL1C: {
# 3339
struct Cyc_Absyn_Exp*_tmp5B3=Cyc_Tcutil_rsubsexp(r,inst,_tmp5CF);
if(_tmp5B3 == _tmp5CF)return e;
return({struct Cyc_Absyn_Exp*_tmpCBF=e;Cyc_Tcutil_copye(_tmpCBF,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->tag=41U,_tmp5B4->f1=_tmp5B3;_tmp5B4;}));});}case 19U: _LL1D: _tmp5D1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_tmp5D0=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp592)->f2;_LL1E: {
# 3343
void*_tmp5B5=Cyc_Tcutil_rsubstitute(r,inst,_tmp5D1);
if(_tmp5B5 == _tmp5D1)return e;
return({struct Cyc_Absyn_Exp*_tmpCC0=e;Cyc_Tcutil_copye(_tmpCC0,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->tag=19U,_tmp5B6->f1=_tmp5B5,_tmp5B6->f2=_tmp5D0;_tmp5B6;}));});}case 39U: _LL1F: _tmp5D2=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp592)->f1;_LL20: {
# 3347
void*_tmp5B7=Cyc_Tcutil_rsubstitute(r,inst,_tmp5D2);
if(_tmp5B7 == _tmp5D2)return e;{
# 3350
void*_tmp5B8=Cyc_Tcutil_compress(_tmp5B7);void*_tmp5B9=_tmp5B8;struct Cyc_Absyn_Exp*_tmp5BB;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5B9)->tag == 18U){_LL24: _tmp5BB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5B9)->f1;_LL25:
 return _tmp5BB;}else{_LL26: _LL27:
# 3353
 return({struct Cyc_Absyn_Exp*_tmpCC1=e;Cyc_Tcutil_copye(_tmpCC1,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->tag=39U,_tmp5BA->f1=_tmp5B7;_tmp5BA;}));});}_LL23:;};}default: _LL21: _LL22:
# 3356
 return({void*_tmp5BC=0U;({struct _dyneither_ptr _tmpCC2=({const char*_tmp5BD="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp5BD,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCC2,_tag_dyneither(_tmp5BC,sizeof(void*),0U));});});}_LL0:;}
# 3360
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3363
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3367
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3370
void*_tmp5D3=f->type;
struct Cyc_Absyn_Exp*_tmp5D4=f->requires_clause;
void*_tmp5D5=Cyc_Tcutil_rsubstitute(r,inst,_tmp5D3);
struct Cyc_Absyn_Exp*_tmp5D6=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp5D4);
if(_tmp5D3 == _tmp5D5  && _tmp5D4 == _tmp5D6)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->name=f->name,_tmp5D7->tq=f->tq,_tmp5D7->type=_tmp5D5,_tmp5D7->width=f->width,_tmp5D7->attributes=f->attributes,_tmp5D7->requires_clause=_tmp5D6;_tmp5D7;});}}
# 3380
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3383
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp5D8=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp5D9=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp5DA=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp5D8);
struct Cyc_List_List*_tmp5DB=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp5D9);
if(_tmp5DA == _tmp5D8  && _tmp5DB == _tmp5D9)return fs;
# 3390
return({struct Cyc_List_List*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->hd=_tmp5DA,_tmp5DC->tl=_tmp5DB;_tmp5DC;});};}
# 3393
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3396
struct _tuple1 _tmp5DD=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp5DE=_tmp5DD;struct Cyc_List_List*_tmp5E2;struct Cyc_List_List*_tmp5E1;_LL1: _tmp5E2=_tmp5DE.f1;_tmp5E1=_tmp5DE.f2;_LL2:;{
struct Cyc_List_List*_tmp5DF=Cyc_Tcutil_substs(rgn,inst,_tmp5E2);
struct Cyc_List_List*_tmp5E0=Cyc_Tcutil_substs(rgn,inst,_tmp5E1);
if(_tmp5DF == _tmp5E2  && _tmp5E0 == _tmp5E1)
return rgn_po;else{
# 3402
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp5DF,_tmp5E0);}};}
# 3405
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3408
void*_tmp5E3=Cyc_Tcutil_compress(t);void*_tmp5E4=_tmp5E3;struct Cyc_List_List*_tmp65C;void*_tmp65B;void*_tmp65A;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Exp*_tmp658;void*_tmp657;void*_tmp656;void*_tmp655;void*_tmp654;void*_tmp653;enum Cyc_Absyn_AggrKind _tmp652;struct Cyc_List_List*_tmp651;struct Cyc_List_List*_tmp650;struct Cyc_List_List*_tmp64F;void*_tmp64E;struct Cyc_Absyn_Tqual _tmp64D;void*_tmp64C;struct Cyc_List_List*_tmp64B;int _tmp64A;struct Cyc_Absyn_VarargInfo*_tmp649;struct Cyc_List_List*_tmp648;struct Cyc_List_List*_tmp647;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*_tmp645;void*_tmp644;struct Cyc_Absyn_Tqual _tmp643;void*_tmp642;union Cyc_Absyn_Constraint*_tmp641;union Cyc_Absyn_Constraint*_tmp640;union Cyc_Absyn_Constraint*_tmp63F;void*_tmp63E;struct Cyc_Absyn_Tqual _tmp63D;struct Cyc_Absyn_Exp*_tmp63C;union Cyc_Absyn_Constraint*_tmp63B;unsigned int _tmp63A;struct _tuple2*_tmp639;struct Cyc_List_List*_tmp638;struct Cyc_Absyn_Typedefdecl*_tmp637;void*_tmp636;union Cyc_Absyn_DatatypeFieldInfoU _tmp635;struct Cyc_List_List*_tmp634;union Cyc_Absyn_DatatypeInfoU _tmp633;struct Cyc_List_List*_tmp632;union Cyc_Absyn_AggrInfoU _tmp631;struct Cyc_List_List*_tmp630;struct Cyc_Absyn_Tvar*_tmp62F;switch(*((int*)_tmp5E4)){case 2U: _LL1: _tmp62F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E4)->f1;_LL2: {
# 3411
struct _handler_cons _tmp5E5;_push_handler(& _tmp5E5);{int _tmp5E7=0;if(setjmp(_tmp5E5.handler))_tmp5E7=1;if(!_tmp5E7){{void*_tmp5E8=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp62F);_npop_handler(0U);return _tmp5E8;};_pop_handler();}else{void*_tmp5E6=(void*)_exn_thrown;void*_tmp5E9=_tmp5E6;void*_tmp5EA;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp5E9)->tag == Cyc_Core_Not_found){_LL3C: _LL3D:
 return t;}else{_LL3E: _tmp5EA=_tmp5E9;_LL3F:(int)_rethrow(_tmp5EA);}_LL3B:;}};}case 11U: _LL3: _tmp631=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E4)->f1).aggr_info;_tmp630=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E4)->f1).targs;_LL4: {
# 3414
struct Cyc_List_List*_tmp5EB=Cyc_Tcutil_substs(rgn,inst,_tmp630);
return _tmp5EB == _tmp630?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->tag=11U,(_tmp5EC->f1).aggr_info=_tmp631,(_tmp5EC->f1).targs=_tmp5EB;_tmp5EC;});}case 3U: _LL5: _tmp633=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E4)->f1).datatype_info;_tmp632=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E4)->f1).targs;_LL6: {
# 3417
struct Cyc_List_List*_tmp5ED=Cyc_Tcutil_substs(rgn,inst,_tmp632);
return _tmp5ED == _tmp632?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));
_tmp5EE->tag=3U,(_tmp5EE->f1).datatype_info=_tmp633,(_tmp5EE->f1).targs=_tmp5ED;_tmp5EE;});}case 4U: _LL7: _tmp635=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E4)->f1).field_info;_tmp634=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E4)->f1).targs;_LL8: {
# 3421
struct Cyc_List_List*_tmp5EF=Cyc_Tcutil_substs(rgn,inst,_tmp634);
return _tmp5EF == _tmp634?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));
_tmp5F0->tag=4U,(_tmp5F0->f1).field_info=_tmp635,(_tmp5F0->f1).targs=_tmp5EF;_tmp5F0;});}case 17U: _LL9: _tmp639=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E4)->f1;_tmp638=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E4)->f2;_tmp637=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E4)->f3;_tmp636=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E4)->f4;_LLA: {
# 3425
struct Cyc_List_List*_tmp5F1=Cyc_Tcutil_substs(rgn,inst,_tmp638);
return _tmp5F1 == _tmp638?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->tag=17U,_tmp5F2->f1=_tmp639,_tmp5F2->f2=_tmp5F1,_tmp5F2->f3=_tmp637,_tmp5F2->f4=_tmp636;_tmp5F2;});}case 8U: _LLB: _tmp63E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E4)->f1).elt_type;_tmp63D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E4)->f1).tq;_tmp63C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E4)->f1).num_elts;_tmp63B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E4)->f1).zero_term;_tmp63A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E4)->f1).zt_loc;_LLC: {
# 3428
void*_tmp5F3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63E);
struct Cyc_Absyn_Exp*_tmp5F4=_tmp63C == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp63C);
return _tmp5F3 == _tmp63E  && _tmp5F4 == _tmp63C?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));
_tmp5F5->tag=8U,(_tmp5F5->f1).elt_type=_tmp5F3,(_tmp5F5->f1).tq=_tmp63D,(_tmp5F5->f1).num_elts=_tmp5F4,(_tmp5F5->f1).zero_term=_tmp63B,(_tmp5F5->f1).zt_loc=_tmp63A;_tmp5F5;});}case 5U: _LLD: _tmp644=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4)->f1).elt_typ;_tmp643=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4)->f1).elt_tq;_tmp642=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4)->f1).ptr_atts).rgn;_tmp641=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4)->f1).ptr_atts).nullable;_tmp640=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4)->f1).ptr_atts).bounds;_tmp63F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4)->f1).ptr_atts).zero_term;_LLE: {
# 3433
void*_tmp5F6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp644);
void*_tmp5F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp642);
union Cyc_Absyn_Constraint*_tmp5F8=_tmp640;
{void*_tmp5F9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp640);void*_tmp5FA=_tmp5F9;struct Cyc_Absyn_Exp*_tmp5FD;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5FA)->tag == 1U){_LL41: _tmp5FD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5FA)->f1;_LL42: {
# 3438
struct Cyc_Absyn_Exp*_tmp5FB=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5FD);
if(_tmp5FB != _tmp5FD)
({union Cyc_Absyn_Constraint*_tmpCC3=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->tag=1U,_tmp5FC->f1=_tmp5FB;_tmp5FC;}));_tmp5F8=_tmpCC3;});
goto _LL40;}}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3444
if((_tmp5F6 == _tmp644  && _tmp5F7 == _tmp642) && _tmp5F8 == _tmp640)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->tag=5U,(_tmp5FE->f1).elt_typ=_tmp5F6,(_tmp5FE->f1).elt_tq=_tmp643,((_tmp5FE->f1).ptr_atts).rgn=_tmp5F7,((_tmp5FE->f1).ptr_atts).nullable=_tmp641,((_tmp5FE->f1).ptr_atts).bounds=_tmp5F8,((_tmp5FE->f1).ptr_atts).zero_term=_tmp63F,((_tmp5FE->f1).ptr_atts).ptrloc=0;_tmp5FE;});}case 9U: _LLF: _tmp64F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).tvars;_tmp64E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).effect;_tmp64D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).ret_tqual;_tmp64C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).ret_typ;_tmp64B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).args;_tmp64A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).c_varargs;_tmp649=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).cyc_varargs;_tmp648=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).rgn_po;_tmp647=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).attributes;_tmp646=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).requires_clause;_tmp645=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).ensures_clause;_LL10:
# 3450
{struct Cyc_List_List*_tmp5FF=_tmp64F;for(0;_tmp5FF != 0;_tmp5FF=_tmp5FF->tl){
({struct Cyc_List_List*_tmpCC6=({struct Cyc_List_List*_tmp602=_region_malloc(rgn,sizeof(*_tmp602));({struct _tuple16*_tmpCC5=({struct _tuple16*_tmp601=_region_malloc(rgn,sizeof(*_tmp601));_tmp601->f1=(struct Cyc_Absyn_Tvar*)_tmp5FF->hd,({void*_tmpCC4=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=2U,_tmp600->f1=(struct Cyc_Absyn_Tvar*)_tmp5FF->hd;_tmp600;});_tmp601->f2=_tmpCC4;});_tmp601;});_tmp602->hd=_tmpCC5;}),_tmp602->tl=inst;_tmp602;});inst=_tmpCC6;});}}{
struct _tuple1 _tmp603=({struct _RegionHandle*_tmpCC8=rgn;struct _RegionHandle*_tmpCC7=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpCC8,_tmpCC7,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp64B));});
# 3452
struct _tuple1 _tmp604=_tmp603;struct Cyc_List_List*_tmp613;struct Cyc_List_List*_tmp612;_LL46: _tmp613=_tmp604.f1;_tmp612=_tmp604.f2;_LL47:;{
# 3454
struct Cyc_List_List*_tmp605=_tmp64B;
struct Cyc_List_List*_tmp606=Cyc_Tcutil_substs(rgn,inst,_tmp612);
if(_tmp606 != _tmp612)
({struct Cyc_List_List*_tmpCC9=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp64B,_tmp606);_tmp605=_tmpCC9;});{
void*eff2;
if(_tmp64E == 0)
eff2=0;else{
# 3462
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64E);
if(new_eff == _tmp64E)
eff2=_tmp64E;else{
# 3466
eff2=new_eff;}}{
# 3468
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp649 == 0)
cyc_varargs2=0;else{
# 3472
struct Cyc_Absyn_VarargInfo _tmp607=*_tmp649;struct Cyc_Absyn_VarargInfo _tmp608=_tmp607;struct _dyneither_ptr*_tmp60E;struct Cyc_Absyn_Tqual _tmp60D;void*_tmp60C;int _tmp60B;_LL49: _tmp60E=_tmp608.name;_tmp60D=_tmp608.tq;_tmp60C=_tmp608.type;_tmp60B=_tmp608.inject;_LL4A:;{
void*_tmp609=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp60C);
if(_tmp609 == _tmp60C)cyc_varargs2=_tmp649;else{
# 3476
({struct Cyc_Absyn_VarargInfo*_tmpCCA=({struct Cyc_Absyn_VarargInfo*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->name=_tmp60E,_tmp60A->tq=_tmp60D,_tmp60A->type=_tmp609,_tmp60A->inject=_tmp60B;_tmp60A;});cyc_varargs2=_tmpCCA;});}};}{
# 3478
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp648);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp646);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp645);
struct Cyc_List_List*_tmp60F=Cyc_Tcutil_extract_relns(_tmp605,req2);
struct Cyc_List_List*_tmp610=Cyc_Tcutil_extract_relns(_tmp605,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp611=_cycalloc(sizeof(*_tmp611));
_tmp611->tag=9U,(_tmp611->f1).tvars=_tmp64F,(_tmp611->f1).effect=eff2,(_tmp611->f1).ret_tqual=_tmp64D,({void*_tmpCCB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64C);(_tmp611->f1).ret_typ=_tmpCCB;}),(_tmp611->f1).args=_tmp605,(_tmp611->f1).c_varargs=_tmp64A,(_tmp611->f1).cyc_varargs=cyc_varargs2,(_tmp611->f1).rgn_po=rgn_po2,(_tmp611->f1).attributes=_tmp647,(_tmp611->f1).requires_clause=req2,(_tmp611->f1).requires_relns=_tmp60F,(_tmp611->f1).ensures_clause=ens2,(_tmp611->f1).ensures_relns=_tmp610;_tmp611;});};};};};};case 10U: _LL11: _tmp650=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E4)->f1;_LL12: {
# 3488
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp614=_tmp650;for(0;_tmp614 != 0;_tmp614=_tmp614->tl){
void*_tmp615=(*((struct _tuple12*)_tmp614->hd)).f2;
void*_tmp616=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp615);
if(_tmp615 != _tmp616)
change=1;
# 3496
({struct Cyc_List_List*_tmpCCC=({struct Cyc_List_List*_tmp617=_region_malloc(rgn,sizeof(*_tmp617));_tmp617->hd=_tmp616,_tmp617->tl=ts2;_tmp617;});ts2=_tmpCCC;});}}
# 3498
if(!change)
return t;{
struct Cyc_List_List*_tmp618=({struct Cyc_List_List*_tmpCCD=_tmp650;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpCCD,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=10U,_tmp619->f1=_tmp618;_tmp619;});};}case 12U: _LL13: _tmp652=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E4)->f1;_tmp651=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E4)->f2;_LL14: {
# 3503
struct Cyc_List_List*_tmp61A=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp651);
if(_tmp651 == _tmp61A)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->tag=12U,_tmp61B->f1=_tmp652,_tmp61B->f2=_tmp61A;_tmp61B;});}case 1U: _LL15: _tmp653=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E4)->f2;_LL16:
# 3507
 if(_tmp653 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp653);else{
return t;}case 15U: _LL17: _tmp654=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E4)->f1;_LL18: {
# 3510
void*_tmp61C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp654);
return _tmp61C == _tmp654?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->tag=15U,_tmp61D->f1=_tmp61C;_tmp61D;});}case 16U: _LL19: _tmp656=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E4)->f1;_tmp655=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E4)->f2;_LL1A: {
# 3513
void*_tmp61E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp656);
void*_tmp61F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp655);
return _tmp61E == _tmp656  && _tmp61F == _tmp655?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tag=16U,_tmp620->f1=_tmp61E,_tmp620->f2=_tmp61F;_tmp620;});}case 19U: _LL1B: _tmp657=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E4)->f1;_LL1C: {
# 3517
void*_tmp621=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp657);
return _tmp621 == _tmp657?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->tag=19U,_tmp622->f1=_tmp621;_tmp622;});}case 18U: _LL1D: _tmp658=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E4)->f1;_LL1E: {
# 3520
struct Cyc_Absyn_Exp*_tmp623=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp658);
return _tmp623 == _tmp658?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=18U,_tmp624->f1=_tmp623;_tmp624;});}case 27U: _LL1F: _tmp659=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E4)->f1;_LL20: {
# 3523
struct Cyc_Absyn_Exp*_tmp625=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp659);
return _tmp625 == _tmp659?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tag=27U,_tmp626->f1=_tmp625;_tmp626;});}case 13U: _LL21: _LL22:
 goto _LL24;case 14U: _LL23: _LL24:
 goto _LL26;case 0U: _LL25: _LL26:
 goto _LL28;case 6U: _LL27: _LL28:
 goto _LL2A;case 7U: _LL29: _LL2A:
 goto _LL2C;case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 28U: _LL2F: _LL30:
 goto _LL32;case 20U: _LL31: _LL32:
 return t;case 25U: _LL33: _tmp65A=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E4)->f1;_LL34: {
# 3535
void*_tmp627=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp65A);
return _tmp627 == _tmp65A?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=25U,_tmp628->f1=_tmp627;_tmp628;});}case 23U: _LL35: _tmp65B=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E4)->f1;_LL36: {
# 3538
void*_tmp629=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp65B);
return _tmp629 == _tmp65B?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->tag=23U,_tmp62A->f1=_tmp629;_tmp62A;});}case 24U: _LL37: _tmp65C=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E4)->f1;_LL38: {
# 3541
struct Cyc_List_List*_tmp62B=Cyc_Tcutil_substs(rgn,inst,_tmp65C);
return _tmp62B == _tmp65C?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=24U,_tmp62C->f1=_tmp62B;_tmp62C;});}default: _LL39: _LL3A:
({void*_tmp62D=0U;({struct _dyneither_ptr _tmpCCE=({const char*_tmp62E="found typedecltype in rsubs";_tag_dyneither(_tmp62E,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCCE,_tag_dyneither(_tmp62D,sizeof(void*),0U));});});}_LL0:;}
# 3547
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3550
if(ts == 0)
return 0;{
void*_tmp65D=(void*)ts->hd;
struct Cyc_List_List*_tmp65E=ts->tl;
void*_tmp65F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp65D);
struct Cyc_List_List*_tmp660=Cyc_Tcutil_substs(rgn,inst,_tmp65E);
if(_tmp65D == _tmp65F  && _tmp65E == _tmp660)
return ts;
return({struct Cyc_List_List*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->hd=_tmp65F,_tmp661->tl=_tmp660;_tmp661;});};}
# 3561
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3568
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp662=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->f1=tv,({void*_tmpCD0=({struct Cyc_Core_Opt*_tmpCCF=_tmp662;Cyc_Absyn_new_evar(_tmpCCF,({struct Cyc_Core_Opt*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->v=s;_tmp663;}));});_tmp664->f2=_tmpCD0;});_tmp664;});}
# 3573
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3575
struct _tuple17*_tmp665=env;struct Cyc_List_List*_tmp66A;struct _RegionHandle*_tmp669;_LL1: _tmp66A=_tmp665->f1;_tmp669=_tmp665->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp666=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp668=_region_malloc(_tmp669,sizeof(*_tmp668));_tmp668->f1=tv,({void*_tmpCD2=({struct Cyc_Core_Opt*_tmpCD1=_tmp666;Cyc_Absyn_new_evar(_tmpCD1,({struct Cyc_Core_Opt*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->v=_tmp66A;_tmp667;}));});_tmp668->f2=_tmpCD2;});_tmp668;});};}
# 3585
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3589
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp66D=({struct Cyc_String_pa_PrintArg_struct _tmpA7F;_tmpA7F.tag=0U,_tmpA7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmpA7F;});struct Cyc_String_pa_PrintArg_struct _tmp66E=({struct Cyc_String_pa_PrintArg_struct _tmpA7E;_tmpA7E.tag=0U,({
struct _dyneither_ptr _tmpCD3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmpA7E.f1=_tmpCD3;});_tmpA7E;});struct Cyc_String_pa_PrintArg_struct _tmp66F=({struct Cyc_String_pa_PrintArg_struct _tmpA7D;_tmpA7D.tag=0U,({struct _dyneither_ptr _tmpCD4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmpA7D.f1=_tmpCD4;});_tmpA7D;});void*_tmp66B[3U];_tmp66B[0]=& _tmp66D,_tmp66B[1]=& _tmp66E,_tmp66B[2]=& _tmp66F;({unsigned int _tmpCD6=loc;struct _dyneither_ptr _tmpCD5=({const char*_tmp66C="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp66C,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpCD6,_tmpCD5,_tag_dyneither(_tmp66B,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp670=0U;({struct _dyneither_ptr _tmpCD7=({const char*_tmp671="same type variable has different identity!";_tag_dyneither(_tmp671,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD7,_tag_dyneither(_tmp670,sizeof(void*),0U));});});}
return tvs;}}}
# 3603
({int _tmpCD8=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpCD8;});
return({struct Cyc_List_List*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->hd=tv,_tmp672->tl=tvs;_tmp672;});}
# 3609
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3611
if(tv->identity == - 1)
({void*_tmp673=0U;({struct _dyneither_ptr _tmpCD9=({const char*_tmp674="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp674,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD9,_tag_dyneither(_tmp673,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3615
struct Cyc_Absyn_Tvar*_tmp675=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp675->identity == - 1)
({void*_tmp676=0U;({struct _dyneither_ptr _tmpCDA=({const char*_tmp677="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp677,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDA,_tag_dyneither(_tmp676,sizeof(void*),0U));});});
if(_tmp675->identity == tv->identity)
return tvs;}}
# 3622
return({struct Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->hd=tv,_tmp678->tl=tvs;_tmp678;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3628
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3633
if(tv->identity == - 1)
({void*_tmp679=0U;({struct _dyneither_ptr _tmpCDB=({const char*_tmp67A="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp67A,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDB,_tag_dyneither(_tmp679,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3637
struct _tuple28*_tmp67B=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp67C=_tmp67B;struct Cyc_Absyn_Tvar*_tmp680;int*_tmp67F;_LL1: _tmp680=_tmp67C->f1;_tmp67F=(int*)& _tmp67C->f2;_LL2:;
if(_tmp680->identity == - 1)
({void*_tmp67D=0U;({struct _dyneither_ptr _tmpCDC=({const char*_tmp67E="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp67E,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDC,_tag_dyneither(_tmp67D,sizeof(void*),0U));});});
if(_tmp680->identity == tv->identity){
*_tmp67F=*_tmp67F  || b;
return tvs;}}}
# 3645
return({struct Cyc_List_List*_tmp682=_region_malloc(r,sizeof(*_tmp682));({struct _tuple28*_tmpCDD=({struct _tuple28*_tmp681=_region_malloc(r,sizeof(*_tmp681));_tmp681->f1=tv,_tmp681->f2=b;_tmp681;});_tmp682->hd=_tmpCDD;}),_tmp682->tl=tvs;_tmp682;});}
# 3649
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3651
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp685=({struct Cyc_String_pa_PrintArg_struct _tmpA80;_tmpA80.tag=0U,({struct _dyneither_ptr _tmpCDE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmpA80.f1=_tmpCDE;});_tmpA80;});void*_tmp683[1U];_tmp683[0]=& _tmp685;({struct _dyneither_ptr _tmpCDF=({const char*_tmp684="bound tvar id for %s is NULL";_tag_dyneither(_tmp684,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDF,_tag_dyneither(_tmp683,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->hd=tv,_tmp686->tl=tvs;_tmp686;});}struct _tuple29{void*f1;int f2;};
# 3660
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3663
void*_tmp687=Cyc_Tcutil_compress(e);void*_tmp688=_tmp687;int _tmp692;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp688)->tag == 1U){_LL1: _tmp692=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp688)->f3;_LL2:
# 3665
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp689=(struct _tuple29*)es2->hd;struct _tuple29*_tmp68A=_tmp689;void*_tmp68F;int*_tmp68E;_LL6: _tmp68F=_tmp68A->f1;_tmp68E=(int*)& _tmp68A->f2;_LL7:;{
void*_tmp68B=Cyc_Tcutil_compress(_tmp68F);void*_tmp68C=_tmp68B;int _tmp68D;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68C)->tag == 1U){_LL9: _tmp68D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68C)->f3;_LLA:
# 3669
 if(_tmp692 == _tmp68D){
if(b != *_tmp68E)*_tmp68E=1;
return es;}
# 3673
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3677
return({struct Cyc_List_List*_tmp691=_region_malloc(r,sizeof(*_tmp691));({struct _tuple29*_tmpCE0=({struct _tuple29*_tmp690=_region_malloc(r,sizeof(*_tmp690));_tmp690->f1=e,_tmp690->f2=b;_tmp690;});_tmp691->hd=_tmpCE0;}),_tmp691->tl=es;_tmp691;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3682
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3685
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3694
if(!present)({struct Cyc_List_List*_tmpCE1=({struct Cyc_List_List*_tmp693=_region_malloc(rgn,sizeof(*_tmp693));_tmp693->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp693->tl=r;_tmp693;});r=_tmpCE1;});}
# 3696
({struct Cyc_List_List*_tmpCE2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpCE2;});
return r;}
# 3701
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3705
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp694=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp695=_tmp694;struct Cyc_Absyn_Tvar*_tmp698;int _tmp697;_LL1: _tmp698=_tmp695.f1;_tmp697=_tmp695.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp698->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3715
if(!present)({struct Cyc_List_List*_tmpCE3=({struct Cyc_List_List*_tmp696=_region_malloc(r,sizeof(*_tmp696));_tmp696->hd=(struct _tuple28*)tvs->hd,_tmp696->tl=res;_tmp696;});res=_tmpCE3;});};}
# 3717
({struct Cyc_List_List*_tmpCE4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpCE4;});
return res;}
# 3721
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3723
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp69B=({struct Cyc_String_pa_PrintArg_struct _tmpA81;_tmpA81.tag=0U,_tmpA81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA81;});void*_tmp699[1U];_tmp699[0]=& _tmp69B;({unsigned int _tmpCE6=loc;struct _dyneither_ptr _tmpCE5=({const char*_tmp69A="bitfield %s does not have constant width";_tag_dyneither(_tmp69A,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpCE6,_tmpCE5,_tag_dyneither(_tmp699,sizeof(void*),1U));});});else{
# 3728
struct _tuple14 _tmp69C=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp69D=_tmp69C;unsigned int _tmp6A1;int _tmp6A0;_LL1: _tmp6A1=_tmp69D.f1;_tmp6A0=_tmp69D.f2;_LL2:;
if(!_tmp6A0)
({void*_tmp69E=0U;({unsigned int _tmpCE8=loc;struct _dyneither_ptr _tmpCE7=({const char*_tmp69F="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp69F,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpCE8,_tmpCE7,_tag_dyneither(_tmp69E,sizeof(void*),0U));});});
w=_tmp6A1;}{
# 3733
void*_tmp6A2=Cyc_Tcutil_compress(field_typ);void*_tmp6A3=_tmp6A2;enum Cyc_Absyn_Size_of _tmp6B1;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6A3)->tag == 6U){_LL4: _tmp6B1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6A3)->f2;_LL5:
# 3736
{enum Cyc_Absyn_Size_of _tmp6A4=_tmp6B1;switch(_tmp6A4){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > 8)({void*_tmp6A5=0U;({unsigned int _tmpCEA=loc;struct _dyneither_ptr _tmpCE9=({const char*_tmp6A6="bitfield larger than type";_tag_dyneither(_tmp6A6,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCEA,_tmpCE9,_tag_dyneither(_tmp6A5,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > 16)({void*_tmp6A7=0U;({unsigned int _tmpCEC=loc;struct _dyneither_ptr _tmpCEB=({const char*_tmp6A8="bitfield larger than type";_tag_dyneither(_tmp6A8,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCEC,_tmpCEB,_tag_dyneither(_tmp6A7,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3741
 if(w > 32)({void*_tmp6A9=0U;({unsigned int _tmpCEE=loc;struct _dyneither_ptr _tmpCED=({const char*_tmp6AA="bitfield larger than type";_tag_dyneither(_tmp6AA,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCEE,_tmpCED,_tag_dyneither(_tmp6A9,sizeof(void*),0U));});});goto _LL8;default: _LL11: _LL12:
# 3743
 if(w > 64)({void*_tmp6AB=0U;({unsigned int _tmpCF0=loc;struct _dyneither_ptr _tmpCEF=({const char*_tmp6AC="bitfield larger than type";_tag_dyneither(_tmp6AC,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCF0,_tmpCEF,_tag_dyneither(_tmp6AB,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3745
goto _LL3;}else{_LL6: _LL7:
# 3747
({struct Cyc_String_pa_PrintArg_struct _tmp6AF=({struct Cyc_String_pa_PrintArg_struct _tmpA83;_tmpA83.tag=0U,_tmpA83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA83;});struct Cyc_String_pa_PrintArg_struct _tmp6B0=({struct Cyc_String_pa_PrintArg_struct _tmpA82;_tmpA82.tag=0U,({
struct _dyneither_ptr _tmpCF1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ));_tmpA82.f1=_tmpCF1;});_tmpA82;});void*_tmp6AD[2U];_tmp6AD[0]=& _tmp6AF,_tmp6AD[1]=& _tmp6B0;({unsigned int _tmpCF3=loc;struct _dyneither_ptr _tmpCF2=({const char*_tmp6AE="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp6AE,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpCF3,_tmpCF2,_tag_dyneither(_tmp6AD,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3754
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp6B2=(void*)atts->hd;void*_tmp6B3=_tmp6B2;switch(*((int*)_tmp6B3)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp6B6=({struct Cyc_String_pa_PrintArg_struct _tmpA85;_tmpA85.tag=0U,({
struct _dyneither_ptr _tmpCF4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmpA85.f1=_tmpCF4;});_tmpA85;});struct Cyc_String_pa_PrintArg_struct _tmp6B7=({struct Cyc_String_pa_PrintArg_struct _tmpA84;_tmpA84.tag=0U,_tmpA84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA84;});void*_tmp6B4[2U];_tmp6B4[0]=& _tmp6B6,_tmp6B4[1]=& _tmp6B7;({unsigned int _tmpCF6=loc;struct _dyneither_ptr _tmpCF5=({const char*_tmp6B5="bad attribute %s on member %s";_tag_dyneither(_tmp6B5,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpCF6,_tmpCF5,_tag_dyneither(_tmp6B4,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3782
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp6B8=t;struct Cyc_Absyn_Typedefdecl*_tmp6BC;void*_tmp6BB;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B8)->tag == 17U){_LL1: _tmp6BC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B8)->f3;_tmp6BB=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B8)->f4;_LL2:
# 3785
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp6BC))->tq).real_const  || (_tmp6BC->tq).print_const){
if(declared_const)({void*_tmp6B9=0U;({unsigned int _tmpCF8=loc;struct _dyneither_ptr _tmpCF7=({const char*_tmp6BA="extra const";_tag_dyneither(_tmp6BA,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpCF8,_tmpCF7,_tag_dyneither(_tmp6B9,sizeof(void*),0U));});});
return 1;}
# 3790
if((unsigned int)_tmp6BB)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp6BB);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3797
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp6BD=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp6BE=_tmp6BD;void*_tmp6C2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BE)->tag == 5U){_LL1: _tmp6C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BE)->f1).ptr_atts).rgn;_LL2:
# 3802
{void*_tmp6BF=Cyc_Tcutil_compress(_tmp6C2);void*_tmp6C0=_tmp6BF;struct Cyc_Absyn_Tvar*_tmp6C1;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6C0)->tag == 2U){_LL6: _tmp6C1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6C0)->f1;_LL7:
# 3804
 return Cyc_Absyn_tvar_cmp(tvar,_tmp6C1)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3807
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3813
return 1;}
return 0;}
# 3817
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3820
void*_tmp6C3=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp6C4=_tmp6C3;switch(*((int*)_tmp6C4)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6C4)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6C4)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp6C4)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp6C4)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3829
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3832
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3838
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3844
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3848
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3851
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp6C7=({struct Cyc_String_pa_PrintArg_struct _tmpA87;_tmpA87.tag=0U,_tmpA87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmpA87;});struct Cyc_String_pa_PrintArg_struct _tmp6C8=({struct Cyc_String_pa_PrintArg_struct _tmpA86;_tmpA86.tag=0U,({
struct _dyneither_ptr _tmpCF9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmpA86.f1=_tmpCF9;});_tmpA86;});void*_tmp6C5[2U];_tmp6C5[0]=& _tmp6C7,_tmp6C5[1]=& _tmp6C8;({unsigned int _tmpCFB=loc;struct _dyneither_ptr _tmpCFA=({const char*_tmp6C6="%s clause has type %s instead of integral type";_tag_dyneither(_tmp6C6,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCFB,_tmpCFA,_tag_dyneither(_tmp6C5,sizeof(void*),2U));});});
({struct Cyc_Tcutil_CVTEnv _tmpCFC=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpCFC;});
({struct Cyc_List_List*_tmpCFD=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpCFD;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp6CB=({struct Cyc_String_pa_PrintArg_struct _tmpA89;_tmpA89.tag=0U,_tmpA89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmpA89;});struct Cyc_String_pa_PrintArg_struct _tmp6CC=({struct Cyc_String_pa_PrintArg_struct _tmpA88;_tmpA88.tag=0U,({
struct _dyneither_ptr _tmpCFE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmpA88.f1=_tmpCFE;});_tmpA88;});void*_tmp6C9[2U];_tmp6C9[0]=& _tmp6CB,_tmp6C9[1]=& _tmp6CC;({unsigned int _tmpD00=clause->loc;struct _dyneither_ptr _tmpCFF=({const char*_tmp6CA="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp6CA,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpD00,_tmpCFF,_tag_dyneither(_tmp6C9,sizeof(void*),2U));});});}
# 3863
return({struct _tuple30 _tmpA8A;_tmpA8A.f1=cvtenv,_tmpA8A.f2=relns;_tmpA8A;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3894 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3902
{void*_tmp6CD=Cyc_Tcutil_compress(t);void*_tmp6CE=_tmp6CD;struct Cyc_List_List*_tmp85B;void*_tmp85A;void*_tmp859;void*_tmp858;void*_tmp857;void*_tmp856;struct _tuple2*_tmp855;struct Cyc_List_List**_tmp854;struct Cyc_Absyn_Typedefdecl**_tmp853;void**_tmp852;union Cyc_Absyn_AggrInfoU*_tmp851;struct Cyc_List_List**_tmp850;enum Cyc_Absyn_AggrKind _tmp84F;struct Cyc_List_List*_tmp84E;struct Cyc_List_List*_tmp84D;struct Cyc_List_List**_tmp84C;void**_tmp84B;struct Cyc_Absyn_Tqual*_tmp84A;void*_tmp849;struct Cyc_List_List*_tmp848;int _tmp847;struct Cyc_Absyn_VarargInfo*_tmp846;struct Cyc_List_List*_tmp845;struct Cyc_List_List*_tmp844;struct Cyc_Absyn_Exp*_tmp843;struct Cyc_List_List**_tmp842;struct Cyc_Absyn_Exp*_tmp841;struct Cyc_List_List**_tmp840;void*_tmp83F;struct Cyc_Absyn_Tqual*_tmp83E;struct Cyc_Absyn_Exp**_tmp83D;union Cyc_Absyn_Constraint*_tmp83C;unsigned int _tmp83B;struct Cyc_Absyn_Exp*_tmp83A;struct Cyc_Absyn_Exp*_tmp839;void*_tmp838;void*_tmp837;struct Cyc_Absyn_Tqual*_tmp836;void*_tmp835;union Cyc_Absyn_Constraint*_tmp834;union Cyc_Absyn_Constraint*_tmp833;union Cyc_Absyn_Constraint*_tmp832;union Cyc_Absyn_DatatypeFieldInfoU*_tmp831;struct Cyc_List_List*_tmp830;union Cyc_Absyn_DatatypeInfoU*_tmp82F;struct Cyc_List_List**_tmp82E;struct _tuple2*_tmp82D;struct Cyc_Absyn_Enumdecl**_tmp82C;struct Cyc_List_List*_tmp82B;void*_tmp82A;void***_tmp829;struct Cyc_Absyn_Tvar*_tmp828;struct Cyc_Core_Opt**_tmp827;void**_tmp826;switch(*((int*)_tmp6CE)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp827=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6CE)->f1;_tmp826=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6CE)->f2;_LL4:
# 3906
 if(*_tmp827 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp827))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp827))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpD01=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp827=_tmpD01;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3911
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp826=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3914
*_tmp826=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp6CF=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->tag=2U,_tmp6D1->f1=0,_tmp6D1->f2=expected_kind;_tmp6D1;}));
({void*_tmpD02=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->tag=2U,_tmp6D0->f1=_tmp6CF;_tmp6D0;});*_tmp826=_tmpD02;});
_tmp828=_tmp6CF;goto _LL6;}else{
# 3920
({struct Cyc_List_List*_tmpD03=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpD03;});}}
# 3922
goto _LL0;case 2U: _LL5: _tmp828=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6CE)->f1;_LL6:
# 3924
{void*_tmp6D2=Cyc_Absyn_compress_kb(_tmp828->kind);void*_tmp6D3=_tmp6D2;struct Cyc_Core_Opt**_tmp6D6;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6D3)->tag == 1U){_LL3C: _tmp6D6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6D3)->f1;_LL3D:
# 3926
({struct Cyc_Core_Opt*_tmpD05=({struct Cyc_Core_Opt*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));({void*_tmpD04=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->tag=2U,_tmp6D4->f1=0,_tmp6D4->f2=expected_kind;_tmp6D4;});_tmp6D5->v=_tmpD04;});_tmp6D5;});*_tmp6D6=_tmpD05;});goto _LL3B;}else{_LL3E: _LL3F:
 goto _LL3B;}_LL3B:;}
# 3931
({struct Cyc_List_List*_tmpD06=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp828);cvtenv.kind_env=_tmpD06;});
# 3934
({struct Cyc_List_List*_tmpD07=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp828,put_in_effect);cvtenv.free_vars=_tmpD07;});
# 3936
{void*_tmp6D7=Cyc_Absyn_compress_kb(_tmp828->kind);void*_tmp6D8=_tmp6D7;struct Cyc_Core_Opt**_tmp6DC;struct Cyc_Absyn_Kind*_tmp6DB;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6D8)->tag == 2U){_LL41: _tmp6DC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6D8)->f1;_tmp6DB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6D8)->f2;_LL42:
# 3938
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6DB))
({struct Cyc_Core_Opt*_tmpD09=({struct Cyc_Core_Opt*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));({void*_tmpD08=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9->tag=2U,_tmp6D9->f1=0,_tmp6D9->f2=expected_kind;_tmp6D9;});_tmp6DA->v=_tmpD08;});_tmp6DA;});*_tmp6DC=_tmpD09;});
goto _LL40;}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3943
goto _LL0;case 26U: _LL7: _tmp82A=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CE)->f1)->r;_tmp829=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CE)->f2;_LL8: {
# 3949
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6DD=_tmp82A;struct Cyc_Absyn_Datatypedecl*_tmp6E0;struct Cyc_Absyn_Enumdecl*_tmp6DF;struct Cyc_Absyn_Aggrdecl*_tmp6DE;switch(*((int*)_tmp6DD)){case 0U: _LL46: _tmp6DE=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6DD)->f1;_LL47:
# 3952
 if(te->in_extern_c_include)
_tmp6DE->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6DE);goto _LL45;case 1U: _LL48: _tmp6DF=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6DD)->f1;_LL49:
# 3956
 if(te->in_extern_c_include)
_tmp6DF->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6DF);goto _LL45;default: _LL4A: _tmp6E0=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6DD)->f1;_LL4B:
# 3960
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6E0);goto _LL45;}_LL45:;}
# 3962
({void**_tmpD0A=({void**_tmp6E1=_cycalloc(sizeof(*_tmp6E1));*_tmp6E1=new_t;_tmp6E1;});*_tmp829=_tmpD0A;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14U: _LL9: _tmp82B=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6CE)->f1;_LLA: {
# 3968
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp82B != 0;_tmp82B=_tmp82B->tl){
struct Cyc_Absyn_Enumfield*_tmp6E2=(struct Cyc_Absyn_Enumfield*)_tmp82B->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6E2->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6E5=({struct Cyc_String_pa_PrintArg_struct _tmpA8B;_tmpA8B.tag=0U,_tmpA8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6E2->name).f2);_tmpA8B;});void*_tmp6E3[1U];_tmp6E3[0]=& _tmp6E5;({unsigned int _tmpD0C=_tmp6E2->loc;struct _dyneither_ptr _tmpD0B=({const char*_tmp6E4="duplicate enum field name %s";_tag_dyneither(_tmp6E4,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpD0C,_tmpD0B,_tag_dyneither(_tmp6E3,sizeof(void*),1U));});});else{
# 3975
({struct Cyc_List_List*_tmpD0D=({struct Cyc_List_List*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->hd=(*_tmp6E2->name).f2,_tmp6E6->tl=prev_fields;_tmp6E6;});prev_fields=_tmpD0D;});}
# 3977
if(_tmp6E2->tag == 0)
({struct Cyc_Absyn_Exp*_tmpD0E=Cyc_Absyn_uint_exp(tag_count,_tmp6E2->loc);_tmp6E2->tag=_tmpD0E;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6E2->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6E9=({struct Cyc_String_pa_PrintArg_struct _tmpA8C;_tmpA8C.tag=0U,_tmpA8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6E2->name).f2);_tmpA8C;});void*_tmp6E7[1U];_tmp6E7[0]=& _tmp6E9;({unsigned int _tmpD10=loc;struct _dyneither_ptr _tmpD0F=({const char*_tmp6E8="enum field %s: expression is not constant";_tag_dyneither(_tmp6E8,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpD10,_tmpD0F,_tag_dyneither(_tmp6E7,sizeof(void*),1U));});});}{
# 3982
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6E2->tag))).f1;
tag_count=t1 + 1;};}
# 3985
goto _LL0;}case 13U: _LLB: _tmp82D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6CE)->f1;_tmp82C=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6CE)->f2;_LLC:
# 3987
 if(*_tmp82C == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp82C))->fields == 0){
struct _handler_cons _tmp6EA;_push_handler(& _tmp6EA);{int _tmp6EC=0;if(setjmp(_tmp6EA.handler))_tmp6EC=1;if(!_tmp6EC){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp82D);
*_tmp82C=*ed;}
# 3989
;_pop_handler();}else{void*_tmp6EB=(void*)_exn_thrown;void*_tmp6ED=_tmp6EB;void*_tmp6F0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6ED)->tag == Cyc_Dict_Absent){_LL4D: _LL4E: {
# 3993
struct Cyc_Absyn_Enumdecl*_tmp6EE=({struct Cyc_Absyn_Enumdecl*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->sc=Cyc_Absyn_Extern,_tmp6EF->name=_tmp82D,_tmp6EF->fields=0;_tmp6EF;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6EE);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp82D);
*_tmp82C=*ed;
goto _LL4C;};}}else{_LL4F: _tmp6F0=_tmp6ED;_LL50:(int)_rethrow(_tmp6F0);}_LL4C:;}};}{
# 3999
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp82C);
goto _LL0;};case 3U: _LLD: _tmp82F=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6CE)->f1).datatype_info;_tmp82E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6CE)->f1).targs;_LLE: {
# 4002
struct Cyc_List_List*_tmp6F1=*_tmp82E;
{union Cyc_Absyn_DatatypeInfoU _tmp6F2=*_tmp82F;union Cyc_Absyn_DatatypeInfoU _tmp6F3=_tmp6F2;struct Cyc_Absyn_Datatypedecl*_tmp70A;struct _tuple2*_tmp709;int _tmp708;if((_tmp6F3.UnknownDatatype).tag == 1){_LL52: _tmp709=((_tmp6F3.UnknownDatatype).val).name;_tmp708=((_tmp6F3.UnknownDatatype).val).is_extensible;_LL53: {
# 4005
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp6F4;_push_handler(& _tmp6F4);{int _tmp6F6=0;if(setjmp(_tmp6F4.handler))_tmp6F6=1;if(!_tmp6F6){({struct Cyc_Absyn_Datatypedecl**_tmpD11=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp709);tudp=_tmpD11;});;_pop_handler();}else{void*_tmp6F5=(void*)_exn_thrown;void*_tmp6F7=_tmp6F5;void*_tmp6FD;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6F7)->tag == Cyc_Dict_Absent){_LL57: _LL58: {
# 4009
struct Cyc_Absyn_Datatypedecl*_tmp6F8=({struct Cyc_Absyn_Datatypedecl*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->sc=Cyc_Absyn_Extern,_tmp6FC->name=_tmp709,_tmp6FC->tvs=0,_tmp6FC->fields=0,_tmp6FC->is_extensible=_tmp708;_tmp6FC;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp6F8);
({struct Cyc_Absyn_Datatypedecl**_tmpD12=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp709);tudp=_tmpD12;});
# 4013
if(_tmp6F1 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp6FB=({struct Cyc_String_pa_PrintArg_struct _tmpA8D;_tmpA8D.tag=0U,({
struct _dyneither_ptr _tmpD13=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp709));_tmpA8D.f1=_tmpD13;});_tmpA8D;});void*_tmp6F9[1U];_tmp6F9[0]=& _tmp6FB;({unsigned int _tmpD15=loc;struct _dyneither_ptr _tmpD14=({const char*_tmp6FA="declare parameterized datatype %s before using";_tag_dyneither(_tmp6FA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD15,_tmpD14,_tag_dyneither(_tmp6F9,sizeof(void*),1U));});});
return cvtenv;}
# 4018
goto _LL56;}}else{_LL59: _tmp6FD=_tmp6F7;_LL5A:(int)_rethrow(_tmp6FD);}_LL56:;}};}
# 4022
if(_tmp708  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp700=({struct Cyc_String_pa_PrintArg_struct _tmpA8E;_tmpA8E.tag=0U,({
struct _dyneither_ptr _tmpD16=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp709));_tmpA8E.f1=_tmpD16;});_tmpA8E;});void*_tmp6FE[1U];_tmp6FE[0]=& _tmp700;({unsigned int _tmpD18=loc;struct _dyneither_ptr _tmpD17=({const char*_tmp6FF="datatype %s was not declared @extensible";_tag_dyneither(_tmp6FF,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpD18,_tmpD17,_tag_dyneither(_tmp6FE,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfoU _tmpD19=Cyc_Absyn_KnownDatatype(tudp);*_tmp82F=_tmpD19;});
_tmp70A=*tudp;goto _LL55;}}else{_LL54: _tmp70A=*(_tmp6F3.KnownDatatype).val;_LL55: {
# 4029
struct Cyc_List_List*tvs=_tmp70A->tvs;
for(0;_tmp6F1 != 0  && tvs != 0;(_tmp6F1=_tmp6F1->tl,tvs=tvs->tl)){
void*t=(void*)_tmp6F1->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4035
{struct _tuple0 _tmp701=({struct _tuple0 _tmpA8F;({void*_tmpD1A=Cyc_Absyn_compress_kb(tv->kind);_tmpA8F.f1=_tmpD1A;}),_tmpA8F.f2=t;_tmpA8F;});struct _tuple0 _tmp702=_tmp701;struct Cyc_Absyn_Tvar*_tmp703;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp702.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp702.f2)->tag == 2U){_LL5C: _tmp703=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp702.f2)->f1;_LL5D:
# 4037
({struct Cyc_List_List*_tmpD1B=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp703);cvtenv.kind_env=_tmpD1B;});
({struct Cyc_List_List*_tmpD1C=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp703,1);cvtenv.free_vars=_tmpD1C;});
continue;}else{goto _LL5E;}}else{_LL5E: _LL5F:
 goto _LL5B;}_LL5B:;}{
# 4042
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpD1D=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpD1D;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4046
if(_tmp6F1 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp706=({struct Cyc_String_pa_PrintArg_struct _tmpA90;_tmpA90.tag=0U,({
struct _dyneither_ptr _tmpD1E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp70A->name));_tmpA90.f1=_tmpD1E;});_tmpA90;});void*_tmp704[1U];_tmp704[0]=& _tmp706;({unsigned int _tmpD20=loc;struct _dyneither_ptr _tmpD1F=({const char*_tmp705="too many type arguments for datatype %s";_tag_dyneither(_tmp705,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpD20,_tmpD1F,_tag_dyneither(_tmp704,sizeof(void*),1U));});});
if(tvs != 0){
# 4051
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpD21=({struct Cyc_List_List*_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707->hd=e,_tmp707->tl=hidden_ts;_tmp707;});hidden_ts=_tmpD21;});
({struct Cyc_Tcutil_CVTEnv _tmpD22=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpD22;});}
# 4058
({struct Cyc_List_List*_tmpD24=({struct Cyc_List_List*_tmpD23=*_tmp82E;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD23,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp82E=_tmpD24;});}
# 4060
goto _LL51;}}_LL51:;}
# 4062
goto _LL0;}case 4U: _LLF: _tmp831=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6CE)->f1).field_info;_tmp830=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6CE)->f1).targs;_LL10:
# 4065
{union Cyc_Absyn_DatatypeFieldInfoU _tmp70B=*_tmp831;union Cyc_Absyn_DatatypeFieldInfoU _tmp70C=_tmp70B;struct Cyc_Absyn_Datatypedecl*_tmp71F;struct Cyc_Absyn_Datatypefield*_tmp71E;struct _tuple2*_tmp71D;struct _tuple2*_tmp71C;int _tmp71B;if((_tmp70C.UnknownDatatypefield).tag == 1){_LL61: _tmp71D=((_tmp70C.UnknownDatatypefield).val).datatype_name;_tmp71C=((_tmp70C.UnknownDatatypefield).val).field_name;_tmp71B=((_tmp70C.UnknownDatatypefield).val).is_extensible;_LL62: {
# 4068
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp71D);
struct Cyc_Absyn_Datatypefield*tuf;
# 4073
{struct Cyc_List_List*_tmp70D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp70D=_tmp70D->tl){
if(_tmp70D == 0)
({void*_tmp70E=0U;({struct _dyneither_ptr _tmpD25=({const char*_tmp70F="Tcutil found a bad datatypefield";_tag_dyneither(_tmp70F,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD25,_tag_dyneither(_tmp70E,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp70D->hd)->name,_tmp71C)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp70D->hd;
break;}}}
# 4081
({union Cyc_Absyn_DatatypeFieldInfoU _tmpD26=Cyc_Absyn_KnownDatatypefield(tud,tuf);*_tmp831=_tmpD26;});
_tmp71F=tud;_tmp71E=tuf;goto _LL64;}}else{_LL63: _tmp71F=((_tmp70C.KnownDatatypefield).val).f1;_tmp71E=((_tmp70C.KnownDatatypefield).val).f2;_LL64: {
# 4085
struct Cyc_List_List*tvs=_tmp71F->tvs;
for(0;_tmp830 != 0  && tvs != 0;(_tmp830=_tmp830->tl,tvs=tvs->tl)){
void*t=(void*)_tmp830->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4091
{struct _tuple0 _tmp710=({struct _tuple0 _tmpA91;({void*_tmpD27=Cyc_Absyn_compress_kb(tv->kind);_tmpA91.f1=_tmpD27;}),_tmpA91.f2=t;_tmpA91;});struct _tuple0 _tmp711=_tmp710;struct Cyc_Absyn_Tvar*_tmp712;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp711.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp711.f2)->tag == 2U){_LL66: _tmp712=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp711.f2)->f1;_LL67:
# 4093
({struct Cyc_List_List*_tmpD28=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp712);cvtenv.kind_env=_tmpD28;});
({struct Cyc_List_List*_tmpD29=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp712,1);cvtenv.free_vars=_tmpD29;});
continue;}else{goto _LL68;}}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}{
# 4098
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpD2A=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpD2A;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4102
if(_tmp830 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp715=({struct Cyc_String_pa_PrintArg_struct _tmpA93;_tmpA93.tag=0U,({
struct _dyneither_ptr _tmpD2B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71F->name));_tmpA93.f1=_tmpD2B;});_tmpA93;});struct Cyc_String_pa_PrintArg_struct _tmp716=({struct Cyc_String_pa_PrintArg_struct _tmpA92;_tmpA92.tag=0U,({struct _dyneither_ptr _tmpD2C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71E->name));_tmpA92.f1=_tmpD2C;});_tmpA92;});void*_tmp713[2U];_tmp713[0]=& _tmp715,_tmp713[1]=& _tmp716;({unsigned int _tmpD2E=loc;struct _dyneither_ptr _tmpD2D=({const char*_tmp714="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp714,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpD2E,_tmpD2D,_tag_dyneither(_tmp713,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp719=({struct Cyc_String_pa_PrintArg_struct _tmpA95;_tmpA95.tag=0U,({
struct _dyneither_ptr _tmpD2F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71F->name));_tmpA95.f1=_tmpD2F;});_tmpA95;});struct Cyc_String_pa_PrintArg_struct _tmp71A=({struct Cyc_String_pa_PrintArg_struct _tmpA94;_tmpA94.tag=0U,({struct _dyneither_ptr _tmpD30=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71E->name));_tmpA94.f1=_tmpD30;});_tmpA94;});void*_tmp717[2U];_tmp717[0]=& _tmp719,_tmp717[1]=& _tmp71A;({unsigned int _tmpD32=loc;struct _dyneither_ptr _tmpD31=({const char*_tmp718="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp718,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpD32,_tmpD31,_tag_dyneither(_tmp717,sizeof(void*),2U));});});
goto _LL60;}}_LL60:;}
# 4110
goto _LL0;case 5U: _LL11: _tmp837=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CE)->f1).elt_typ;_tmp836=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CE)->f1).elt_tq;_tmp835=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CE)->f1).ptr_atts).rgn;_tmp834=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CE)->f1).ptr_atts).nullable;_tmp833=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CE)->f1).ptr_atts).bounds;_tmp832=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CE)->f1).ptr_atts).zero_term;_LL12: {
# 4113
int is_zero_terminated;
# 4115
({struct Cyc_Tcutil_CVTEnv _tmpD33=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp837,1,1);cvtenv=_tmpD33;});
({int _tmpD34=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp836->print_const,_tmp837);_tmp836->real_const=_tmpD34;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp720=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp721=_tmp720;switch(_tmp721){case Cyc_Absyn_Aliasable: _LL6B: _LL6C:
 k=& Cyc_Tcutil_rk;goto _LL6A;case Cyc_Absyn_Unique: _LL6D: _LL6E:
 k=& Cyc_Tcutil_urk;goto _LL6A;default: _LL6F: _LL70:
 k=& Cyc_Tcutil_trk;goto _LL6A;}_LL6A:;}
# 4123
({struct Cyc_Tcutil_CVTEnv _tmpD35=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp835,1,1);cvtenv=_tmpD35;});
{union Cyc_Absyn_Constraint*_tmp722=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp832);union Cyc_Absyn_Constraint*_tmp723=_tmp722;int _tmp729;switch((((union Cyc_Absyn_Constraint*)_tmp723)->Eq_constr).tag){case 3U: _LL72: _LL73:
# 4128
{void*_tmp724=Cyc_Tcutil_compress(_tmp837);void*_tmp725=_tmp724;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp725)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp725)->f2 == Cyc_Absyn_Char_sz){_LL79: _LL7A:
# 4130
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp832,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL78;}else{goto _LL7B;}}else{_LL7B: _LL7C:
# 4134
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp832,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL78;}_LL78:;}
# 4138
goto _LL71;case 1U: _LL74: _tmp729=(int)(_tmp723->Eq_constr).val;if(_tmp729){_LL75:
# 4141
 if(!Cyc_Tcutil_admits_zero(_tmp837))
({struct Cyc_String_pa_PrintArg_struct _tmp728=({struct Cyc_String_pa_PrintArg_struct _tmpA96;_tmpA96.tag=0U,({
struct _dyneither_ptr _tmpD36=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp837));_tmpA96.f1=_tmpD36;});_tmpA96;});void*_tmp726[1U];_tmp726[0]=& _tmp728;({unsigned int _tmpD38=loc;struct _dyneither_ptr _tmpD37=({const char*_tmp727="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp727,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpD38,_tmpD37,_tag_dyneither(_tmp726,sizeof(void*),1U));});});
is_zero_terminated=1;
goto _LL71;}else{goto _LL76;}default: _LL76: _LL77:
# 4147
 is_zero_terminated=0;
goto _LL71;}_LL71:;}
# 4151
{void*_tmp72A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp833);void*_tmp72B=_tmp72A;struct Cyc_Absyn_Exp*_tmp734;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp72B)->tag == 0U){_LL7E: _LL7F:
 goto _LL7D;}else{_LL80: _tmp734=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp72B)->f1;_LL81:
# 4154
({struct Cyc_Tcenv_Tenv*_tmpD39=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD39,0,_tmp734);});
({struct Cyc_Tcutil_CVTEnv _tmpD3A=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp734,te,cvtenv);cvtenv=_tmpD3A;});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp734))
({void*_tmp72C=0U;({unsigned int _tmpD3C=loc;struct _dyneither_ptr _tmpD3B=({const char*_tmp72D="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp72D,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpD3C,_tmpD3B,_tag_dyneither(_tmp72C,sizeof(void*),0U));});});{
struct _tuple14 _tmp72E=Cyc_Evexp_eval_const_uint_exp(_tmp734);struct _tuple14 _tmp72F=_tmp72E;unsigned int _tmp733;int _tmp732;_LL83: _tmp733=_tmp72F.f1;_tmp732=_tmp72F.f2;_LL84:;
if(is_zero_terminated  && (!_tmp732  || _tmp733 < 1))
({void*_tmp730=0U;({unsigned int _tmpD3E=loc;struct _dyneither_ptr _tmpD3D=({const char*_tmp731="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp731,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpD3E,_tmpD3D,_tag_dyneither(_tmp730,sizeof(void*),0U));});});
goto _LL7D;};}_LL7D:;}
# 4163
goto _LL0;};}case 19U: _LL13: _tmp838=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6CE)->f1;_LL14:
# 4165
({struct Cyc_Tcutil_CVTEnv _tmpD3F=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp838,1,1);cvtenv=_tmpD3F;});goto _LL0;case 28U: _LL15: _LL16:
 goto _LL0;case 18U: _LL17: _tmp839=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6CE)->f1;_LL18:
# 4171
({struct Cyc_Tcenv_Tenv*_tmpD40=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD40,0,_tmp839);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp839))
({void*_tmp735=0U;({unsigned int _tmpD42=loc;struct _dyneither_ptr _tmpD41=({const char*_tmp736="valueof_t requires an int expression";_tag_dyneither(_tmp736,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD42,_tmpD41,_tag_dyneither(_tmp735,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD43=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp839,te,cvtenv);cvtenv=_tmpD43;});
goto _LL0;case 27U: _LL19: _tmp83A=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6CE)->f1;_LL1A:
# 4180
({struct Cyc_Tcenv_Tenv*_tmpD44=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD44,0,_tmp83A);});
goto _LL0;case 6U: _LL1B: _LL1C:
 goto _LL1E;case 7U: _LL1D: _LL1E:
 goto _LL0;case 8U: _LL1F: _tmp83F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).elt_type;_tmp83E=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).tq;_tmp83D=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).num_elts;_tmp83C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).zero_term;_tmp83B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE)->f1).zt_loc;_LL20: {
# 4187
struct Cyc_Absyn_Exp*_tmp737=*_tmp83D;
({struct Cyc_Tcutil_CVTEnv _tmpD45=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp83F,1,allow_abs_aggr);cvtenv=_tmpD45;});
({int _tmpD46=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp83E->print_const,_tmp83F);_tmp83E->real_const=_tmpD46;});{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp738=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp83C);union Cyc_Absyn_Constraint*_tmp739=_tmp738;int _tmp73D;switch((((union Cyc_Absyn_Constraint*)_tmp739)->Eq_constr).tag){case 3U: _LL86: _LL87:
# 4194
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp83C,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4209 "tcutil.cyc"
goto _LL85;case 1U: _LL88: _tmp73D=(int)(_tmp739->Eq_constr).val;if(_tmp73D){_LL89:
# 4212
 if(!Cyc_Tcutil_admits_zero(_tmp83F))
({struct Cyc_String_pa_PrintArg_struct _tmp73C=({struct Cyc_String_pa_PrintArg_struct _tmpA97;_tmpA97.tag=0U,({
struct _dyneither_ptr _tmpD47=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp83F));_tmpA97.f1=_tmpD47;});_tmpA97;});void*_tmp73A[1U];_tmp73A[0]=& _tmp73C;({unsigned int _tmpD49=loc;struct _dyneither_ptr _tmpD48=({const char*_tmp73B="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp73B,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD49,_tmpD48,_tag_dyneither(_tmp73A,sizeof(void*),1U));});});
is_zero_terminated=1;
goto _LL85;}else{goto _LL8A;}default: _LL8A: _LL8B:
# 4218
 is_zero_terminated=0;
goto _LL85;}_LL85:;}
# 4223
if(_tmp737 == 0){
# 4225
if(is_zero_terminated)
# 4227
({struct Cyc_Absyn_Exp*_tmpD4B=({struct Cyc_Absyn_Exp*_tmpD4A=Cyc_Absyn_uint_exp(1U,0U);_tmp737=_tmpD4A;});*_tmp83D=_tmpD4B;});else{
# 4230
({void*_tmp73E=0U;({unsigned int _tmpD4D=loc;struct _dyneither_ptr _tmpD4C=({const char*_tmp73F="array bound defaults to 1 here";_tag_dyneither(_tmp73F,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpD4D,_tmpD4C,_tag_dyneither(_tmp73E,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpD4F=({struct Cyc_Absyn_Exp*_tmpD4E=Cyc_Absyn_uint_exp(1U,0U);_tmp737=_tmpD4E;});*_tmp83D=_tmpD4F;});}}
# 4234
({struct Cyc_Tcenv_Tenv*_tmpD50=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD50,0,_tmp737);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp737))
({void*_tmp740=0U;({unsigned int _tmpD52=loc;struct _dyneither_ptr _tmpD51=({const char*_tmp741="array bounds expression is not an unsigned int";_tag_dyneither(_tmp741,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD52,_tmpD51,_tag_dyneither(_tmp740,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD53=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp737,te,cvtenv);cvtenv=_tmpD53;});{
# 4242
struct _tuple14 _tmp742=Cyc_Evexp_eval_const_uint_exp(_tmp737);struct _tuple14 _tmp743=_tmp742;unsigned int _tmp749;int _tmp748;_LL8D: _tmp749=_tmp743.f1;_tmp748=_tmp743.f2;_LL8E:;
# 4244
if((is_zero_terminated  && _tmp748) && _tmp749 < 1)
({void*_tmp744=0U;({unsigned int _tmpD55=loc;struct _dyneither_ptr _tmpD54=({const char*_tmp745="zero terminated array cannot have zero elements";_tag_dyneither(_tmp745,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpD55,_tmpD54,_tag_dyneither(_tmp744,sizeof(void*),0U));});});
# 4247
if((_tmp748  && _tmp749 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp746=0U;({unsigned int _tmpD57=loc;struct _dyneither_ptr _tmpD56=({const char*_tmp747="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp747,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpD57,_tmpD56,_tag_dyneither(_tmp746,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpD58=Cyc_Absyn_uint_exp(1U,0U);*_tmp83D=_tmpD58;});}
# 4251
goto _LL0;};};}case 9U: _LL21: _tmp84C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).tvars;_tmp84B=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).effect;_tmp84A=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).ret_tqual;_tmp849=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).ret_typ;_tmp848=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).args;_tmp847=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).c_varargs;_tmp846=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).cyc_varargs;_tmp845=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).rgn_po;_tmp844=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).attributes;_tmp843=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).requires_clause;_tmp842=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).requires_relns;_tmp841=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).ensures_clause;_tmp840=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CE)->f1).ensures_relns;_LL22: {
# 4258
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp844 != 0;_tmp844=_tmp844->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp844->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp74C=({struct Cyc_String_pa_PrintArg_struct _tmpA98;_tmpA98.tag=0U,({struct _dyneither_ptr _tmpD59=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp844->hd));_tmpA98.f1=_tmpD59;});_tmpA98;});void*_tmp74A[1U];_tmp74A[0]=& _tmp74C;({unsigned int _tmpD5B=loc;struct _dyneither_ptr _tmpD5A=({const char*_tmp74B="bad function type attribute %s";_tag_dyneither(_tmp74B,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpD5B,_tmpD5A,_tag_dyneither(_tmp74A,sizeof(void*),1U));});});{
void*_tmp74D=(void*)_tmp844->hd;void*_tmp74E=_tmp74D;enum Cyc_Absyn_Format_Type _tmp753;int _tmp752;int _tmp751;switch(*((int*)_tmp74E)){case 1U: _LL90: _LL91:
# 4271
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL8F;case 2U: _LL92: _LL93:
# 4273
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL8F;case 3U: _LL94: _LL95:
# 4275
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL8F;case 19U: _LL96: _tmp753=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp74E)->f1;_tmp752=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp74E)->f2;_tmp751=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp74E)->f3;_LL97:
# 4277
 if(!seen_format){
seen_format=1;
ft=_tmp753;
fmt_desc_arg=_tmp752;
fmt_arg_start=_tmp751;}else{
# 4283
({void*_tmp74F=0U;({unsigned int _tmpD5D=loc;struct _dyneither_ptr _tmpD5C=({const char*_tmp750="function can't have multiple format attributes";_tag_dyneither(_tmp750,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD5D,_tmpD5C,_tag_dyneither(_tmp74F,sizeof(void*),0U));});});}
goto _LL8F;default: _LL98: _LL99:
 goto _LL8F;}_LL8F:;};}
# 4288
if(num_convs > 1)
({void*_tmp754=0U;({unsigned int _tmpD5F=loc;struct _dyneither_ptr _tmpD5E=({const char*_tmp755="function can't have multiple calling conventions";_tag_dyneither(_tmp755,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpD5F,_tmpD5E,_tag_dyneither(_tmp754,sizeof(void*),0U));});});
# 4293
Cyc_Tcutil_check_unique_tvars(loc,*_tmp84C);
{struct Cyc_List_List*b=*_tmp84C;for(0;b != 0;b=b->tl){
({int _tmpD60=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpD60;});
({struct Cyc_List_List*_tmpD61=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpD61;});{
void*_tmp756=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp757=_tmp756;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp757)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp757)->f1)->kind == Cyc_Absyn_MemKind){_LL9B: _LL9C:
# 4299
({struct Cyc_String_pa_PrintArg_struct _tmp75A=({struct Cyc_String_pa_PrintArg_struct _tmpA99;_tmpA99.tag=0U,_tmpA99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmpA99;});void*_tmp758[1U];_tmp758[0]=& _tmp75A;({unsigned int _tmpD63=loc;struct _dyneither_ptr _tmpD62=({const char*_tmp759="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp759,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD63,_tmpD62,_tag_dyneither(_tmp758,sizeof(void*),1U));});});
goto _LL9A;}else{goto _LL9D;}}else{_LL9D: _LL9E:
 goto _LL9A;}_LL9A:;};}}{
# 4307
struct Cyc_Tcutil_CVTEnv _tmp75B=({struct Cyc_Tcutil_CVTEnv _tmpA9D;_tmpA9D.r=Cyc_Core_heap_region,_tmpA9D.kind_env=cvtenv.kind_env,_tmpA9D.free_vars=0,_tmpA9D.free_evars=0,_tmpA9D.generalize_evars=cvtenv.generalize_evars,_tmpA9D.fn_result=1;_tmpA9D;});
# 4312
({struct Cyc_Tcutil_CVTEnv _tmpD64=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp75B,& Cyc_Tcutil_tmk,_tmp849,1,1);_tmp75B=_tmpD64;});
({int _tmpD65=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp84A->print_const,_tmp849);_tmp84A->real_const=_tmpD65;});
_tmp75B.fn_result=0;
{struct Cyc_List_List*a=_tmp848;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp75C=(struct _tuple10*)a->hd;
void*_tmp75D=(*_tmp75C).f3;
({struct Cyc_Tcutil_CVTEnv _tmpD66=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp75B,& Cyc_Tcutil_tmk,_tmp75D,1,1);_tmp75B=_tmpD66;});{
int _tmp75E=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp75C).f2).print_const,_tmp75D);
((*_tmp75C).f2).real_const=_tmp75E;
# 4323
if(Cyc_Tcutil_is_array(_tmp75D)){
# 4325
void*_tmp75F=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpD67=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp75B,& Cyc_Tcutil_rk,_tmp75F,1,1);_tmp75B=_tmpD67;});
({void*_tmpD68=Cyc_Tcutil_promote_array(_tmp75D,_tmp75F,0);(*_tmp75C).f3=_tmpD68;});}};}}
# 4332
if(_tmp846 != 0){
if(_tmp847)({void*_tmp760=0U;({struct _dyneither_ptr _tmpD69=({const char*_tmp761="both c_vararg and cyc_vararg";_tag_dyneither(_tmp761,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD69,_tag_dyneither(_tmp760,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp762=*_tmp846;struct Cyc_Absyn_VarargInfo _tmp763=_tmp762;void*_tmp776;int _tmp775;_LLA0: _tmp776=_tmp763.type;_tmp775=_tmp763.inject;_LLA1:;
({struct Cyc_Tcutil_CVTEnv _tmpD6A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp75B,& Cyc_Tcutil_tmk,_tmp776,1,1);_tmp75B=_tmpD6A;});
({int _tmpD6B=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp846->tq).print_const,_tmp776);(_tmp846->tq).real_const=_tmpD6B;});
# 4338
if(_tmp775){
void*_tmp764=Cyc_Tcutil_compress(_tmp776);void*_tmp765=_tmp764;void*_tmp774;union Cyc_Absyn_Constraint*_tmp773;union Cyc_Absyn_Constraint*_tmp772;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->tag == 5U){_LLA3: _tmp774=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->f1).elt_typ;_tmp773=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->f1).ptr_atts).bounds;_tmp772=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->f1).ptr_atts).zero_term;_LLA4:
# 4341
{void*_tmp766=Cyc_Tcutil_compress(_tmp774);void*_tmp767=_tmp766;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp767)->tag == 3U){_LLA8: _LLA9:
# 4343
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp772))
({void*_tmp768=0U;({unsigned int _tmpD6D=loc;struct _dyneither_ptr _tmpD6C=({const char*_tmp769="can't inject into a zeroterm pointer";_tag_dyneither(_tmp769,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD6D,_tmpD6C,_tag_dyneither(_tmp768,sizeof(void*),0U));});});
{void*_tmp76A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp773);void*_tmp76B=_tmp76A;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp76B)->tag == 0U){_LLAD: _LLAE:
# 4347
({void*_tmp76C=0U;({unsigned int _tmpD6F=loc;struct _dyneither_ptr _tmpD6E=({const char*_tmp76D="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp76D,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpD6F,_tmpD6E,_tag_dyneither(_tmp76C,sizeof(void*),0U));});});
goto _LLAC;}else{_LLAF: _LLB0:
 goto _LLAC;}_LLAC:;}
# 4351
goto _LLA7;}else{_LLAA: _LLAB:
({void*_tmp76E=0U;({unsigned int _tmpD71=loc;struct _dyneither_ptr _tmpD70=({const char*_tmp76F="can't inject a non-datatype type";_tag_dyneither(_tmp76F,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpD71,_tmpD70,_tag_dyneither(_tmp76E,sizeof(void*),0U));});});goto _LLA7;}_LLA7:;}
# 4354
goto _LLA2;}else{_LLA5: _LLA6:
({void*_tmp770=0U;({unsigned int _tmpD73=loc;struct _dyneither_ptr _tmpD72=({const char*_tmp771="expecting a datatype pointer type";_tag_dyneither(_tmp771,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpD73,_tmpD72,_tag_dyneither(_tmp770,sizeof(void*),0U));});});goto _LLA2;}_LLA2:;}};}
# 4360
if(seen_format){
int _tmp777=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp848);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp777) || fmt_arg_start < 0) || 
# 4364
(_tmp846 == 0  && !_tmp847) && fmt_arg_start != 0) || 
(_tmp846 != 0  || _tmp847) && fmt_arg_start != _tmp777 + 1)
# 4367
({void*_tmp778=0U;({unsigned int _tmpD75=loc;struct _dyneither_ptr _tmpD74=({const char*_tmp779="bad format descriptor";_tag_dyneither(_tmp779,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpD75,_tmpD74,_tag_dyneither(_tmp778,sizeof(void*),0U));});});else{
# 4370
struct _tuple10 _tmp77A=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp848,fmt_desc_arg - 1);struct _tuple10 _tmp77B=_tmp77A;void*_tmp791;_LLB2: _tmp791=_tmp77B.f3;_LLB3:;
# 4372
{void*_tmp77C=Cyc_Tcutil_compress(_tmp791);void*_tmp77D=_tmp77C;void*_tmp78A;union Cyc_Absyn_Constraint*_tmp789;union Cyc_Absyn_Constraint*_tmp788;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->tag == 5U){_LLB5: _tmp78A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->f1).elt_typ;_tmp789=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->f1).ptr_atts).bounds;_tmp788=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77D)->f1).ptr_atts).zero_term;_LLB6:
# 4375
{struct _tuple0 _tmp77E=({struct _tuple0 _tmpA9A;({void*_tmpD77=Cyc_Tcutil_compress(_tmp78A);_tmpA9A.f1=_tmpD77;}),({void*_tmpD76=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp789);_tmpA9A.f2=_tmpD76;});_tmpA9A;});struct _tuple0 _tmp77F=_tmp77E;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77F.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77F.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp77F.f2)->tag == 0U){_LLBA: _LLBB:
# 4377
 if(_tmp847)
({void*_tmp780=0U;({unsigned int _tmpD79=loc;struct _dyneither_ptr _tmpD78=({const char*_tmp781="format descriptor is not a char * type";_tag_dyneither(_tmp781,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD79,_tmpD78,_tag_dyneither(_tmp780,sizeof(void*),0U));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 4381
 if(!_tmp847)
({void*_tmp782=0U;({unsigned int _tmpD7B=loc;struct _dyneither_ptr _tmpD7A=({const char*_tmp783="format descriptor is not a char ? type";_tag_dyneither(_tmp783,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD7B,_tmpD7A,_tag_dyneither(_tmp782,sizeof(void*),0U));});});
goto _LLB9;}}else{goto _LLBE;}}else{_LLBE: _LLBF:
# 4385
({void*_tmp784=0U;({unsigned int _tmpD7D=loc;struct _dyneither_ptr _tmpD7C=({const char*_tmp785="format descriptor is not a string type";_tag_dyneither(_tmp785,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD7D,_tmpD7C,_tag_dyneither(_tmp784,sizeof(void*),0U));});});
goto _LLB9;}_LLB9:;}
# 4388
goto _LLB4;}else{_LLB7: _LLB8:
({void*_tmp786=0U;({unsigned int _tmpD7F=loc;struct _dyneither_ptr _tmpD7E=({const char*_tmp787="format descriptor is not a string type";_tag_dyneither(_tmp787,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD7F,_tmpD7E,_tag_dyneither(_tmp786,sizeof(void*),0U));});});goto _LLB4;}_LLB4:;}
# 4391
if(fmt_arg_start != 0  && !_tmp847){
# 4395
int problem;
{struct _tuple31 _tmp78B=({struct _tuple31 _tmpA9B;_tmpA9B.f1=ft,({void*_tmpD80=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp846))->type));_tmpA9B.f2=_tmpD80;});_tmpA9B;});struct _tuple31 _tmp78C=_tmp78B;struct Cyc_Absyn_Datatypedecl*_tmp78E;struct Cyc_Absyn_Datatypedecl*_tmp78D;if(_tmp78C.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78C.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78C.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC1: _tmp78D=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78C.f2)->f1).datatype_info).KnownDatatype).val;_LLC2:
# 4398
({int _tmpD81=Cyc_Absyn_qvar_cmp(_tmp78D->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;problem=_tmpD81;});goto _LLC0;}else{goto _LLC5;}}else{goto _LLC5;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78C.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78C.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC3: _tmp78E=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp78C.f2)->f1).datatype_info).KnownDatatype).val;_LLC4:
# 4400
({int _tmpD82=Cyc_Absyn_qvar_cmp(_tmp78E->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;problem=_tmpD82;});goto _LLC0;}else{goto _LLC5;}}else{_LLC5: _LLC6:
# 4402
 problem=1;goto _LLC0;}}_LLC0:;}
# 4404
if(problem)
({void*_tmp78F=0U;({unsigned int _tmpD84=loc;struct _dyneither_ptr _tmpD83=({const char*_tmp790="format attribute and vararg types don't match";_tag_dyneither(_tmp790,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpD84,_tmpD83,_tag_dyneither(_tmp78F,sizeof(void*),0U));});});}}}
# 4412
{struct Cyc_List_List*rpo=_tmp845;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp792=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp793=_tmp792;void*_tmp795;void*_tmp794;_LLC8: _tmp795=_tmp793->f1;_tmp794=_tmp793->f2;_LLC9:;
({struct Cyc_Tcutil_CVTEnv _tmpD85=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp75B,& Cyc_Tcutil_ek,_tmp795,1,1);_tmp75B=_tmpD85;});
({struct Cyc_Tcutil_CVTEnv _tmpD86=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp75B,& Cyc_Tcutil_trk,_tmp794,1,1);_tmp75B=_tmpD86;});}}{
# 4420
struct Cyc_Tcenv_Fenv*_tmp796=Cyc_Tcenv_bogus_fenv(_tmp849,_tmp848);
struct Cyc_Tcenv_Tenv*_tmp797=({struct Cyc_Tcenv_Tenv*_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD->ns=te->ns,_tmp7DD->ae=te->ae,_tmp7DD->le=_tmp796,_tmp7DD->allow_valueof=1,_tmp7DD->in_extern_c_include=te->in_extern_c_include;_tmp7DD;});
struct _tuple30 _tmp798=({unsigned int _tmpD8A=loc;struct Cyc_Tcenv_Tenv*_tmpD89=_tmp797;struct Cyc_Tcutil_CVTEnv _tmpD88=_tmp75B;struct _dyneither_ptr _tmpD87=({const char*_tmp7DC="@requires";_tag_dyneither(_tmp7DC,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpD8A,_tmpD89,_tmpD88,_tmpD87,_tmp843);});struct _tuple30 _tmp799=_tmp798;struct Cyc_Tcutil_CVTEnv _tmp7DB;struct Cyc_List_List*_tmp7DA;_LLCB: _tmp7DB=_tmp799.f1;_tmp7DA=_tmp799.f2;_LLCC:;
_tmp75B=_tmp7DB;
*_tmp842=_tmp7DA;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp848,_tmp7DA);{
# 4433
struct _tuple30 _tmp79A=({unsigned int _tmpD8E=loc;struct Cyc_Tcenv_Tenv*_tmpD8D=_tmp797;struct Cyc_Tcutil_CVTEnv _tmpD8C=_tmp75B;struct _dyneither_ptr _tmpD8B=({const char*_tmp7D9="@ensures";_tag_dyneither(_tmp7D9,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpD8E,_tmpD8D,_tmpD8C,_tmpD8B,_tmp841);});struct _tuple30 _tmp79B=_tmp79A;struct Cyc_Tcutil_CVTEnv _tmp7D8;struct Cyc_List_List*_tmp7D7;_LLCE: _tmp7D8=_tmp79B.f1;_tmp7D7=_tmp79B.f2;_LLCF:;
_tmp75B=_tmp7D8;
*_tmp840=_tmp7D7;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp848,_tmp7D7);
# 4438
if(*_tmp84B != 0)
({struct Cyc_Tcutil_CVTEnv _tmpD8F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp75B,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp84B),1,1);_tmp75B=_tmpD8F;});else{
# 4441
struct Cyc_List_List*effect=0;
# 4446
{struct Cyc_List_List*tvs=_tmp75B.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp79C=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp79D=_tmp79C;struct Cyc_Absyn_Tvar*_tmp7B0;int _tmp7AF;_LLD1: _tmp7B0=_tmp79D.f1;_tmp7AF=_tmp79D.f2;_LLD2:;
if(!_tmp7AF)continue;{
void*_tmp79E=Cyc_Absyn_compress_kb(_tmp7B0->kind);void*_tmp79F=_tmp79E;struct Cyc_Core_Opt**_tmp7AE;struct Cyc_Absyn_Kind*_tmp7AD;struct Cyc_Core_Opt**_tmp7AC;struct Cyc_Core_Opt**_tmp7AB;struct Cyc_Absyn_Kind*_tmp7AA;switch(*((int*)_tmp79F)){case 2U: _LLD4: _tmp7AB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79F)->f1;_tmp7AA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79F)->f2;if(_tmp7AA->kind == Cyc_Absyn_RgnKind){_LLD5:
# 4452
 if(_tmp7AA->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpD90=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp7AB=_tmpD90;});_tmp7AD=_tmp7AA;goto _LLD7;}
# 4455
({struct Cyc_Core_Opt*_tmpD91=Cyc_Tcutil_kind_to_bound_opt(_tmp7AA);*_tmp7AB=_tmpD91;});_tmp7AD=_tmp7AA;goto _LLD7;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79F)->f2)->kind){case Cyc_Absyn_IntKind: _LLD8: _LLD9:
# 4458
 goto _LLDB;case Cyc_Absyn_EffKind: _LLDC: _tmp7AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79F)->f1;_LLDD:
# 4461
({struct Cyc_Core_Opt*_tmpD92=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp7AC=_tmpD92;});goto _LLDF;default: goto _LLE2;}}case 0U: _LLD6: _tmp7AD=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp79F)->f1;if(_tmp7AD->kind == Cyc_Absyn_RgnKind){_LLD7:
# 4457
({struct Cyc_List_List*_tmpD95=({struct Cyc_List_List*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));({void*_tmpD94=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1->tag=23U,({void*_tmpD93=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0->tag=2U,_tmp7A0->f1=_tmp7B0;_tmp7A0;});_tmp7A1->f1=_tmpD93;});_tmp7A1;});_tmp7A2->hd=_tmpD94;}),_tmp7A2->tl=effect;_tmp7A2;});effect=_tmpD95;});goto _LLD3;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp79F)->f1)->kind){case Cyc_Absyn_IntKind: _LLDA: _LLDB:
# 4459
 goto _LLD3;case Cyc_Absyn_EffKind: _LLDE: _LLDF:
# 4463
({struct Cyc_List_List*_tmpD97=({struct Cyc_List_List*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));({void*_tmpD96=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));_tmp7A3->tag=2U,_tmp7A3->f1=_tmp7B0;_tmp7A3;});_tmp7A4->hd=_tmpD96;}),_tmp7A4->tl=effect;_tmp7A4;});effect=_tmpD97;});goto _LLD3;default: _LLE2: _LLE3:
# 4468
({struct Cyc_List_List*_tmpD9A=({struct Cyc_List_List*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));({void*_tmpD99=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp7A8=_cycalloc(sizeof(*_tmp7A8));_tmp7A8->tag=25U,({void*_tmpD98=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7->tag=2U,_tmp7A7->f1=_tmp7B0;_tmp7A7;});_tmp7A8->f1=_tmpD98;});_tmp7A8;});_tmp7A9->hd=_tmpD99;}),_tmp7A9->tl=effect;_tmp7A9;});effect=_tmpD9A;});goto _LLD3;}}default: _LLE0: _tmp7AE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp79F)->f1;_LLE1:
# 4465
({struct Cyc_Core_Opt*_tmpD9C=({struct Cyc_Core_Opt*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));({void*_tmpD9B=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5->tag=2U,_tmp7A5->f1=0,_tmp7A5->f2=& Cyc_Tcutil_ak;_tmp7A5;});_tmp7A6->v=_tmpD9B;});_tmp7A6;});*_tmp7AE=_tmpD9C;});goto _LLE3;}_LLD3:;};}}
# 4472
{struct Cyc_List_List*ts=_tmp75B.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp7B1=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp7B2=_tmp7B1;void*_tmp7BB;int _tmp7BA;_LLE5: _tmp7BB=_tmp7B2.f1;_tmp7BA=_tmp7B2.f2;_LLE6:;
if(!_tmp7BA)continue;{
struct Cyc_Absyn_Kind*_tmp7B3=Cyc_Tcutil_typ_kind(_tmp7BB);struct Cyc_Absyn_Kind*_tmp7B4=_tmp7B3;switch(((struct Cyc_Absyn_Kind*)_tmp7B4)->kind){case Cyc_Absyn_RgnKind: _LLE8: _LLE9:
# 4477
({struct Cyc_List_List*_tmpD9E=({struct Cyc_List_List*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));({void*_tmpD9D=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5->tag=23U,_tmp7B5->f1=_tmp7BB;_tmp7B5;});_tmp7B6->hd=_tmpD9D;}),_tmp7B6->tl=effect;_tmp7B6;});effect=_tmpD9E;});goto _LLE7;case Cyc_Absyn_EffKind: _LLEA: _LLEB:
# 4479
({struct Cyc_List_List*_tmpD9F=({struct Cyc_List_List*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->hd=_tmp7BB,_tmp7B7->tl=effect;_tmp7B7;});effect=_tmpD9F;});goto _LLE7;case Cyc_Absyn_IntKind: _LLEC: _LLED:
 goto _LLE7;default: _LLEE: _LLEF:
# 4482
({struct Cyc_List_List*_tmpDA1=({struct Cyc_List_List*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));({void*_tmpDA0=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->tag=25U,_tmp7B8->f1=_tmp7BB;_tmp7B8;});_tmp7B9->hd=_tmpDA0;}),_tmp7B9->tl=effect;_tmp7B9;});effect=_tmpDA1;});goto _LLE7;}_LLE7:;};}}
# 4485
({void*_tmpDA2=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC->tag=24U,_tmp7BC->f1=effect;_tmp7BC;});*_tmp84B=_tmpDA2;});}
# 4491
if(*_tmp84C != 0){
struct Cyc_List_List*bs=*_tmp84C;for(0;bs != 0;bs=bs->tl){
void*_tmp7BD=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp7BE=_tmp7BD;struct Cyc_Core_Opt**_tmp7CE;struct Cyc_Absyn_Kind*_tmp7CD;struct Cyc_Core_Opt**_tmp7CC;struct Cyc_Core_Opt**_tmp7CB;struct Cyc_Core_Opt**_tmp7CA;struct Cyc_Core_Opt**_tmp7C9;struct Cyc_Core_Opt**_tmp7C8;struct Cyc_Core_Opt**_tmp7C7;struct Cyc_Core_Opt**_tmp7C6;struct Cyc_Core_Opt**_tmp7C5;struct Cyc_Core_Opt**_tmp7C4;switch(*((int*)_tmp7BE)){case 1U: _LLF1: _tmp7C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LLF2:
# 4495
({struct Cyc_String_pa_PrintArg_struct _tmp7C1=({struct Cyc_String_pa_PrintArg_struct _tmpA9C;_tmpA9C.tag=0U,_tmpA9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmpA9C;});void*_tmp7BF[1U];_tmp7BF[0]=& _tmp7C1;({unsigned int _tmpDA4=loc;struct _dyneither_ptr _tmpDA3=({const char*_tmp7C0="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp7C0,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpDA4,_tmpDA3,_tag_dyneither(_tmp7BF,sizeof(void*),1U));});});
# 4497
_tmp7C5=_tmp7C4;goto _LLF4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f2)->aliasqual == Cyc_Absyn_Top){_LLF3: _tmp7C5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LLF4:
 _tmp7C6=_tmp7C5;goto _LLF6;}else{goto _LL103;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF5: _tmp7C6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LLF6:
 _tmp7C7=_tmp7C6;goto _LLF8;case Cyc_Absyn_Aliasable: _LLF7: _tmp7C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LLF8:
 _tmp7C9=_tmp7C7;goto _LLFA;default: _LLFD: _tmp7C8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LLFE:
# 4504
 _tmp7CB=_tmp7C8;goto _LL100;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF9: _tmp7C9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LLFA:
# 4501
 _tmp7CA=_tmp7C9;goto _LLFC;case Cyc_Absyn_Aliasable: _LLFB: _tmp7CA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LLFC:
# 4503
({struct Cyc_Core_Opt*_tmpDA5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp7CA=_tmpDA5;});goto _LLF0;default: _LLFF: _tmp7CB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LL100:
# 4506
({struct Cyc_Core_Opt*_tmpDA6=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp7CB=_tmpDA6;});goto _LLF0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f2)->aliasqual == Cyc_Absyn_Top){_LL101: _tmp7CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_LL102:
# 4508
({struct Cyc_Core_Opt*_tmpDA7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp7CC=_tmpDA7;});goto _LLF0;}else{goto _LL103;}default: _LL103: _tmp7CE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f1;_tmp7CD=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BE)->f2;_LL104:
# 4510
({struct Cyc_Core_Opt*_tmpDA8=Cyc_Tcutil_kind_to_bound_opt(_tmp7CD);*_tmp7CE=_tmpDA8;});goto _LLF0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7BE)->f1)->kind == Cyc_Absyn_MemKind){_LL105: _LL106:
# 4512
({void*_tmp7C2=0U;({unsigned int _tmpDAA=loc;struct _dyneither_ptr _tmpDA9=({const char*_tmp7C3="functions can't abstract M types";_tag_dyneither(_tmp7C3,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpDAA,_tmpDA9,_tag_dyneither(_tmp7C2,sizeof(void*),0U));});});goto _LLF0;}else{_LL107: _LL108:
 goto _LLF0;}}_LLF0:;}}
# 4517
({struct Cyc_List_List*_tmpDAB=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp75B.kind_env,*_tmp84C);cvtenv.kind_env=_tmpDAB;});
({struct Cyc_List_List*_tmpDAC=Cyc_Tcutil_remove_bound_tvars_bool(_tmp75B.r,_tmp75B.free_vars,*_tmp84C);_tmp75B.free_vars=_tmpDAC;});
# 4520
{struct Cyc_List_List*tvs=_tmp75B.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp7CF=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp7D0=_tmp7CF;struct Cyc_Absyn_Tvar*_tmp7D2;int _tmp7D1;_LL10A: _tmp7D2=_tmp7D0.f1;_tmp7D1=_tmp7D0.f2;_LL10B:;
({struct Cyc_List_List*_tmpDAD=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7D2,_tmp7D1);cvtenv.free_vars=_tmpDAD;});}}
# 4525
{struct Cyc_List_List*evs=_tmp75B.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp7D3=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp7D4=_tmp7D3;void*_tmp7D6;int _tmp7D5;_LL10D: _tmp7D6=_tmp7D4.f1;_tmp7D5=_tmp7D4.f2;_LL10E:;
({struct Cyc_List_List*_tmpDAE=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp7D6,_tmp7D5);cvtenv.free_evars=_tmpDAE;});}}
# 4529
goto _LL0;};};};}case 10U: _LL23: _tmp84D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6CE)->f1;_LL24:
# 4532
 for(0;_tmp84D != 0;_tmp84D=_tmp84D->tl){
struct _tuple12*_tmp7DE=(struct _tuple12*)_tmp84D->hd;
({struct Cyc_Tcutil_CVTEnv _tmpDAF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp7DE).f2,1,0);cvtenv=_tmpDAF;});
({int _tmpDB0=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp7DE).f1).print_const,(*_tmp7DE).f2);
# 4535
((*_tmp7DE).f1).real_const=_tmpDB0;});}
# 4538
goto _LL0;case 12U: _LL25: _tmp84F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6CE)->f1;_tmp84E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6CE)->f2;_LL26: {
# 4542
struct Cyc_List_List*prev_fields=0;
for(0;_tmp84E != 0;_tmp84E=_tmp84E->tl){
struct Cyc_Absyn_Aggrfield*_tmp7DF=(struct Cyc_Absyn_Aggrfield*)_tmp84E->hd;struct Cyc_Absyn_Aggrfield*_tmp7E0=_tmp7DF;struct _dyneither_ptr*_tmp7F0;struct Cyc_Absyn_Tqual*_tmp7EF;void*_tmp7EE;struct Cyc_Absyn_Exp*_tmp7ED;struct Cyc_List_List*_tmp7EC;struct Cyc_Absyn_Exp*_tmp7EB;_LL110: _tmp7F0=_tmp7E0->name;_tmp7EF=(struct Cyc_Absyn_Tqual*)& _tmp7E0->tq;_tmp7EE=_tmp7E0->type;_tmp7ED=_tmp7E0->width;_tmp7EC=_tmp7E0->attributes;_tmp7EB=_tmp7E0->requires_clause;_LL111:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7F0))
({struct Cyc_String_pa_PrintArg_struct _tmp7E3=({struct Cyc_String_pa_PrintArg_struct _tmpA9E;_tmpA9E.tag=0U,_tmpA9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7F0);_tmpA9E;});void*_tmp7E1[1U];_tmp7E1[0]=& _tmp7E3;({unsigned int _tmpDB2=loc;struct _dyneither_ptr _tmpDB1=({const char*_tmp7E2="duplicate field %s";_tag_dyneither(_tmp7E2,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpDB2,_tmpDB1,_tag_dyneither(_tmp7E1,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpDB3=(struct _dyneither_ptr)*_tmp7F0;Cyc_strcmp(_tmpDB3,({const char*_tmp7E4="";_tag_dyneither(_tmp7E4,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpDB4=({struct Cyc_List_List*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5->hd=_tmp7F0,_tmp7E5->tl=prev_fields;_tmp7E5;});prev_fields=_tmpDB4;});
({struct Cyc_Tcutil_CVTEnv _tmpDB5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7EE,1,0);cvtenv=_tmpDB5;});
({int _tmpDB6=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7EF->print_const,_tmp7EE);_tmp7EF->real_const=_tmpDB6;});
# 4555
Cyc_Tcutil_check_bitfield(loc,te,_tmp7EE,_tmp7ED,_tmp7F0);
Cyc_Tcutil_check_field_atts(loc,_tmp7F0,_tmp7EC);
if(_tmp7EB != 0){
# 4559
if(_tmp84F != Cyc_Absyn_UnionA)
({void*_tmp7E6=0U;({unsigned int _tmpDB8=loc;struct _dyneither_ptr _tmpDB7=({const char*_tmp7E7="@requires clause is only allowed on union members";_tag_dyneither(_tmp7E7,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpDB8,_tmpDB7,_tag_dyneither(_tmp7E6,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpDB9=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDB9,0,_tmp7EB);});
if(!Cyc_Tcutil_is_integral(_tmp7EB))
({struct Cyc_String_pa_PrintArg_struct _tmp7EA=({struct Cyc_String_pa_PrintArg_struct _tmpA9F;_tmpA9F.tag=0U,({
struct _dyneither_ptr _tmpDBA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7EB->topt)));_tmpA9F.f1=_tmpDBA;});_tmpA9F;});void*_tmp7E8[1U];_tmp7E8[0]=& _tmp7EA;({unsigned int _tmpDBC=loc;struct _dyneither_ptr _tmpDBB=({const char*_tmp7E9="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp7E9,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpDBC,_tmpDBB,_tag_dyneither(_tmp7E8,sizeof(void*),1U));});});
({struct Cyc_Tcutil_CVTEnv _tmpDBD=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EB,te,cvtenv);cvtenv=_tmpDBD;});}}
# 4568
goto _LL0;}case 11U: _LL27: _tmp851=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6CE)->f1).aggr_info;_tmp850=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6CE)->f1).targs;_LL28:
# 4571
{union Cyc_Absyn_AggrInfoU _tmp7F1=*_tmp851;union Cyc_Absyn_AggrInfoU _tmp7F2=_tmp7F1;struct Cyc_Absyn_Aggrdecl*_tmp815;enum Cyc_Absyn_AggrKind _tmp814;struct _tuple2*_tmp813;struct Cyc_Core_Opt*_tmp812;if((_tmp7F2.UnknownAggr).tag == 1){_LL113: _tmp814=((_tmp7F2.UnknownAggr).val).f1;_tmp813=((_tmp7F2.UnknownAggr).val).f2;_tmp812=((_tmp7F2.UnknownAggr).val).f3;_LL114: {
# 4573
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp7F3;_push_handler(& _tmp7F3);{int _tmp7F5=0;if(setjmp(_tmp7F3.handler))_tmp7F5=1;if(!_tmp7F5){
({struct Cyc_Absyn_Aggrdecl**_tmpDBE=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp813);adp=_tmpDBE;});{
struct Cyc_Absyn_Aggrdecl*_tmp7F6=*adp;
if(_tmp7F6->kind != _tmp814){
if(_tmp7F6->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp7F9=({struct Cyc_String_pa_PrintArg_struct _tmpAA1;_tmpAA1.tag=0U,({struct _dyneither_ptr _tmpDBF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));_tmpAA1.f1=_tmpDBF;});_tmpAA1;});struct Cyc_String_pa_PrintArg_struct _tmp7FA=({struct Cyc_String_pa_PrintArg_struct _tmpAA0;_tmpAA0.tag=0U,({
struct _dyneither_ptr _tmpDC0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));_tmpAA0.f1=_tmpDC0;});_tmpAA0;});void*_tmp7F7[2U];_tmp7F7[0]=& _tmp7F9,_tmp7F7[1]=& _tmp7FA;({unsigned int _tmpDC2=loc;struct _dyneither_ptr _tmpDC1=({const char*_tmp7F8="expecting struct %s instead of union %s";_tag_dyneither(_tmp7F8,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpDC2,_tmpDC1,_tag_dyneither(_tmp7F7,sizeof(void*),2U));});});else{
# 4582
({struct Cyc_String_pa_PrintArg_struct _tmp7FD=({struct Cyc_String_pa_PrintArg_struct _tmpAA3;_tmpAA3.tag=0U,({struct _dyneither_ptr _tmpDC3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));_tmpAA3.f1=_tmpDC3;});_tmpAA3;});struct Cyc_String_pa_PrintArg_struct _tmp7FE=({struct Cyc_String_pa_PrintArg_struct _tmpAA2;_tmpAA2.tag=0U,({
struct _dyneither_ptr _tmpDC4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));_tmpAA2.f1=_tmpDC4;});_tmpAA2;});void*_tmp7FB[2U];_tmp7FB[0]=& _tmp7FD,_tmp7FB[1]=& _tmp7FE;({unsigned int _tmpDC6=loc;struct _dyneither_ptr _tmpDC5=({const char*_tmp7FC="expecting union %s instead of struct %s";_tag_dyneither(_tmp7FC,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpDC6,_tmpDC5,_tag_dyneither(_tmp7FB,sizeof(void*),2U));});});}}
# 4585
if((unsigned int)_tmp812  && (int)_tmp812->v){
if(!((unsigned int)_tmp7F6->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7F6->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp801=({struct Cyc_String_pa_PrintArg_struct _tmpAA4;_tmpAA4.tag=0U,({
struct _dyneither_ptr _tmpDC7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));_tmpAA4.f1=_tmpDC7;});_tmpAA4;});void*_tmp7FF[1U];_tmp7FF[0]=& _tmp801;({unsigned int _tmpDC9=loc;struct _dyneither_ptr _tmpDC8=({const char*_tmp800="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp800,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpDC9,_tmpDC8,_tag_dyneither(_tmp7FF,sizeof(void*),1U));});});}
# 4591
({union Cyc_Absyn_AggrInfoU _tmpDCA=Cyc_Absyn_KnownAggr(adp);*_tmp851=_tmpDCA;});};
# 4575
;_pop_handler();}else{void*_tmp7F4=(void*)_exn_thrown;void*_tmp802=_tmp7F4;void*_tmp808;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp802)->tag == Cyc_Dict_Absent){_LL118: _LL119: {
# 4595
struct Cyc_Absyn_Aggrdecl*_tmp803=({struct Cyc_Absyn_Aggrdecl*_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807->kind=_tmp814,_tmp807->sc=Cyc_Absyn_Extern,_tmp807->name=_tmp813,_tmp807->tvs=0,_tmp807->impl=0,_tmp807->attributes=0,_tmp807->expected_mem_kind=0;_tmp807;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp803);
({struct Cyc_Absyn_Aggrdecl**_tmpDCB=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp813);adp=_tmpDCB;});
({union Cyc_Absyn_AggrInfoU _tmpDCC=Cyc_Absyn_KnownAggr(adp);*_tmp851=_tmpDCC;});
# 4600
if(*_tmp850 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp806=({struct Cyc_String_pa_PrintArg_struct _tmpAA5;_tmpAA5.tag=0U,({
struct _dyneither_ptr _tmpDCD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp813));_tmpAA5.f1=_tmpDCD;});_tmpAA5;});void*_tmp804[1U];_tmp804[0]=& _tmp806;({unsigned int _tmpDCF=loc;struct _dyneither_ptr _tmpDCE=({const char*_tmp805="declare parameterized type %s before using";_tag_dyneither(_tmp805,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpDCF,_tmpDCE,_tag_dyneither(_tmp804,sizeof(void*),1U));});});
return cvtenv;}
# 4605
goto _LL117;}}else{_LL11A: _tmp808=_tmp802;_LL11B:(int)_rethrow(_tmp808);}_LL117:;}};}
# 4607
_tmp815=*adp;goto _LL116;}}else{_LL115: _tmp815=*(_tmp7F2.KnownAggr).val;_LL116: {
# 4609
struct Cyc_List_List*tvs=_tmp815->tvs;
struct Cyc_List_List*ts=*_tmp850;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp809=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp80A=(void*)ts->hd;
# 4617
{struct _tuple0 _tmp80B=({struct _tuple0 _tmpAA6;({void*_tmpDD0=Cyc_Absyn_compress_kb(_tmp809->kind);_tmpAA6.f1=_tmpDD0;}),_tmpAA6.f2=_tmp80A;_tmpAA6;});struct _tuple0 _tmp80C=_tmp80B;struct Cyc_Absyn_Tvar*_tmp80D;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp80C.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp80C.f2)->tag == 2U){_LL11D: _tmp80D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp80C.f2)->f1;_LL11E:
# 4619
({struct Cyc_List_List*_tmpDD1=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp80D);cvtenv.kind_env=_tmpDD1;});
({struct Cyc_List_List*_tmpDD2=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp80D,1);cvtenv.free_vars=_tmpDD2;});
continue;}else{goto _LL11F;}}else{_LL11F: _LL120:
 goto _LL11C;}_LL11C:;}{
# 4624
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDD3=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpDD3;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4628
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp810=({struct Cyc_String_pa_PrintArg_struct _tmpAA7;_tmpAA7.tag=0U,({struct _dyneither_ptr _tmpDD4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp815->name));_tmpAA7.f1=_tmpDD4;});_tmpAA7;});void*_tmp80E[1U];_tmp80E[0]=& _tmp810;({unsigned int _tmpDD6=loc;struct _dyneither_ptr _tmpDD5=({const char*_tmp80F="too many parameters for type %s";_tag_dyneither(_tmp80F,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpDD6,_tmpDD5,_tag_dyneither(_tmp80E,sizeof(void*),1U));});});
if(tvs != 0){
# 4632
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDD7=({struct Cyc_List_List*_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811->hd=e,_tmp811->tl=hidden_ts;_tmp811;});hidden_ts=_tmpDD7;});
({struct Cyc_Tcutil_CVTEnv _tmpDD8=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpDD8;});}
# 4639
({struct Cyc_List_List*_tmpDDA=({struct Cyc_List_List*_tmpDD9=*_tmp850;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDD9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp850=_tmpDDA;});}
# 4641
if((allow_abs_aggr  && _tmp815->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4646
_tmp815->expected_mem_kind=1;}}_LL112:;}
# 4649
goto _LL0;case 17U: _LL29: _tmp855=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CE)->f1;_tmp854=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CE)->f2;_tmp853=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CE)->f3;_tmp852=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CE)->f4;_LL2A: {
# 4652
struct Cyc_List_List*targs=*_tmp854;
# 4654
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp816;_push_handler(& _tmp816);{int _tmp818=0;if(setjmp(_tmp816.handler))_tmp818=1;if(!_tmp818){({struct Cyc_Absyn_Typedefdecl*_tmpDDB=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp855);td=_tmpDDB;});;_pop_handler();}else{void*_tmp817=(void*)_exn_thrown;void*_tmp819=_tmp817;void*_tmp81D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp819)->tag == Cyc_Dict_Absent){_LL122: _LL123:
# 4657
({struct Cyc_String_pa_PrintArg_struct _tmp81C=({struct Cyc_String_pa_PrintArg_struct _tmpAA8;_tmpAA8.tag=0U,({struct _dyneither_ptr _tmpDDC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp855));_tmpAA8.f1=_tmpDDC;});_tmpAA8;});void*_tmp81A[1U];_tmp81A[0]=& _tmp81C;({unsigned int _tmpDDE=loc;struct _dyneither_ptr _tmpDDD=({const char*_tmp81B="unbound typedef name %s";_tag_dyneither(_tmp81B,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpDDE,_tmpDDD,_tag_dyneither(_tmp81A,sizeof(void*),1U));});});
return cvtenv;}else{_LL124: _tmp81D=_tmp819;_LL125:(int)_rethrow(_tmp81D);}_LL121:;}};}
# 4660
*_tmp853=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4665
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4670
({struct Cyc_Tcutil_CVTEnv _tmpDDF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpDDF;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpDE1=({struct Cyc_List_List*_tmp81F=_cycalloc(sizeof(*_tmp81F));({struct _tuple16*_tmpDE0=({struct _tuple16*_tmp81E=_cycalloc(sizeof(*_tmp81E));_tmp81E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp81E->f2=(void*)ts->hd;_tmp81E;});_tmp81F->hd=_tmpDE0;}),_tmp81F->tl=inst;_tmp81F;});inst=_tmpDE1;});}
# 4674
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp822=({struct Cyc_String_pa_PrintArg_struct _tmpAA9;_tmpAA9.tag=0U,({struct _dyneither_ptr _tmpDE2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp855));_tmpAA9.f1=_tmpDE2;});_tmpAA9;});void*_tmp820[1U];_tmp820[0]=& _tmp822;({unsigned int _tmpDE4=loc;struct _dyneither_ptr _tmpDE3=({const char*_tmp821="too many parameters for typedef %s";_tag_dyneither(_tmp821,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpDE4,_tmpDE3,_tag_dyneither(_tmp820,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4679
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDE5=({struct Cyc_List_List*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->hd=e,_tmp823->tl=hidden_ts;_tmp823;});hidden_ts=_tmpDE5;});
({struct Cyc_Tcutil_CVTEnv _tmpDE6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpDE6;});
({struct Cyc_List_List*_tmpDE8=({struct Cyc_List_List*_tmp825=_cycalloc(sizeof(*_tmp825));({struct _tuple16*_tmpDE7=({struct _tuple16*_tmp824=_cycalloc(sizeof(*_tmp824));_tmp824->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp824->f2=e;_tmp824;});_tmp825->hd=_tmpDE7;}),_tmp825->tl=inst;_tmp825;});inst=_tmpDE8;});}
# 4687
({struct Cyc_List_List*_tmpDEA=({struct Cyc_List_List*_tmpDE9=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDE9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp854=_tmpDEA;});}
# 4689
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp852=new_typ;}
# 4695
goto _LL0;};}case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL0;case 15U: _LL31: _tmp856=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6CE)->f1;_LL32:
# 4700
({struct Cyc_Tcutil_CVTEnv _tmpDEB=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp856,1,allow_abs_aggr);cvtenv=_tmpDEB;});goto _LL0;case 16U: _LL33: _tmp858=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6CE)->f1;_tmp857=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6CE)->f2;_LL34:
# 4703
({struct Cyc_Tcutil_CVTEnv _tmpDEC=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp858,0,0);cvtenv=_tmpDEC;});
({struct Cyc_Tcutil_CVTEnv _tmpDED=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp857,1,0);cvtenv=_tmpDED;});
goto _LL0;case 23U: _LL35: _tmp859=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6CE)->f1;_LL36:
# 4707
({struct Cyc_Tcutil_CVTEnv _tmpDEE=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp859,1,1);cvtenv=_tmpDEE;});goto _LL0;case 25U: _LL37: _tmp85A=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6CE)->f1;_LL38:
# 4709
({struct Cyc_Tcutil_CVTEnv _tmpDEF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp85A,1,1);cvtenv=_tmpDEF;});goto _LL0;default: _LL39: _tmp85B=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6CE)->f1;_LL3A:
# 4711
 for(0;_tmp85B != 0;_tmp85B=_tmp85B->tl){
({struct Cyc_Tcutil_CVTEnv _tmpDF0=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp85B->hd,1,1);cvtenv=_tmpDF0;});}
goto _LL0;}_LL0:;}
# 4715
if(!({struct Cyc_Absyn_Kind*_tmpDF1=Cyc_Tcutil_typ_kind(t);Cyc_Tcutil_kind_leq(_tmpDF1,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp85E=({struct Cyc_String_pa_PrintArg_struct _tmpAAC;_tmpAAC.tag=0U,({
struct _dyneither_ptr _tmpDF2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAAC.f1=_tmpDF2;});_tmpAAC;});struct Cyc_String_pa_PrintArg_struct _tmp85F=({struct Cyc_String_pa_PrintArg_struct _tmpAAB;_tmpAAB.tag=0U,({struct _dyneither_ptr _tmpDF3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));_tmpAAB.f1=_tmpDF3;});_tmpAAB;});struct Cyc_String_pa_PrintArg_struct _tmp860=({struct Cyc_String_pa_PrintArg_struct _tmpAAA;_tmpAAA.tag=0U,({struct _dyneither_ptr _tmpDF4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmpAAA.f1=_tmpDF4;});_tmpAAA;});void*_tmp85C[3U];_tmp85C[0]=& _tmp85E,_tmp85C[1]=& _tmp85F,_tmp85C[2]=& _tmp860;({unsigned int _tmpDF6=loc;struct _dyneither_ptr _tmpDF5=({const char*_tmp85D="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp85D,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpDF6,_tmpDF5,_tag_dyneither(_tmp85C,sizeof(void*),3U));});});
# 4719
return cvtenv;}
# 4727
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4729
{void*_tmp861=e->r;void*_tmp862=_tmp861;struct Cyc_Absyn_Exp*_tmp875;struct Cyc_Absyn_Exp*_tmp874;void*_tmp873;void*_tmp872;void*_tmp871;void*_tmp870;struct Cyc_Absyn_Exp*_tmp86F;struct Cyc_Absyn_Exp*_tmp86E;struct Cyc_Absyn_Exp*_tmp86D;struct Cyc_Absyn_Exp*_tmp86C;struct Cyc_Absyn_Exp*_tmp86B;struct Cyc_Absyn_Exp*_tmp86A;struct Cyc_Absyn_Exp*_tmp869;struct Cyc_Absyn_Exp*_tmp868;struct Cyc_Absyn_Exp*_tmp867;struct Cyc_Absyn_Exp*_tmp866;struct Cyc_List_List*_tmp865;switch(*((int*)_tmp862)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp865=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp862)->f2;_LLC:
# 4736
 for(0;_tmp865 != 0;_tmp865=_tmp865->tl){
({struct Cyc_Tcutil_CVTEnv _tmpDF7=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp865->hd,te,cvtenv);cvtenv=_tmpDF7;});}
goto _LL0;case 6U: _LLD: _tmp868=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_tmp867=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp862)->f2;_tmp866=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp862)->f3;_LLE:
# 4740
({struct Cyc_Tcutil_CVTEnv _tmpDF8=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp868,te,cvtenv);cvtenv=_tmpDF8;});
({struct Cyc_Tcutil_CVTEnv _tmpDF9=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp867,te,cvtenv);cvtenv=_tmpDF9;});
({struct Cyc_Tcutil_CVTEnv _tmpDFA=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp866,te,cvtenv);cvtenv=_tmpDFA;});
goto _LL0;case 7U: _LLF: _tmp86A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_tmp869=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp862)->f2;_LL10:
 _tmp86C=_tmp86A;_tmp86B=_tmp869;goto _LL12;case 8U: _LL11: _tmp86C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_tmp86B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp862)->f2;_LL12:
 _tmp86E=_tmp86C;_tmp86D=_tmp86B;goto _LL14;case 9U: _LL13: _tmp86E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_tmp86D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp862)->f2;_LL14:
# 4747
({struct Cyc_Tcutil_CVTEnv _tmpDFB=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp86E,te,cvtenv);cvtenv=_tmpDFB;});
({struct Cyc_Tcutil_CVTEnv _tmpDFC=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp86D,te,cvtenv);cvtenv=_tmpDFC;});
goto _LL0;case 14U: _LL15: _tmp870=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_tmp86F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp862)->f2;_LL16:
# 4751
({struct Cyc_Tcutil_CVTEnv _tmpDFD=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp86F,te,cvtenv);cvtenv=_tmpDFD;});
({struct Cyc_Tcutil_CVTEnv _tmpDFE=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp870,1,0);cvtenv=_tmpDFE;});
goto _LL0;case 19U: _LL17: _tmp871=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_LL18:
 _tmp872=_tmp871;goto _LL1A;case 17U: _LL19: _tmp872=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_LL1A:
# 4756
({struct Cyc_Tcutil_CVTEnv _tmpDFF=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp872,1,0);cvtenv=_tmpDFF;});
goto _LL0;case 39U: _LL1B: _tmp873=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_LL1C:
# 4759
({struct Cyc_Tcutil_CVTEnv _tmpE00=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp873,1,0);cvtenv=_tmpE00;});
goto _LL0;case 18U: _LL1D: _tmp874=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_LL1E:
# 4762
({struct Cyc_Tcutil_CVTEnv _tmpE01=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp874,te,cvtenv);cvtenv=_tmpE01;});
goto _LL0;case 41U: _LL1F: _tmp875=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp862)->f1;_LL20:
# 4765
({struct Cyc_Tcutil_CVTEnv _tmpE02=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp875,te,cvtenv);cvtenv=_tmpE02;});
goto _LL0;default: _LL21: _LL22:
# 4768
({void*_tmp863=0U;({struct _dyneither_ptr _tmpE03=({const char*_tmp864="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp864,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE03,_tag_dyneither(_tmp863,sizeof(void*),0U));});});}_LL0:;}
# 4770
return cvtenv;}
# 4773
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4778
struct Cyc_List_List*_tmp876=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpE04=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpE04;});
# 4781
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp877=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp878=_tmp877;struct Cyc_Absyn_Tvar*_tmp879;_LL1: _tmp879=_tmp878.f1;_LL2:;
({struct Cyc_List_List*_tmpE05=Cyc_Tcutil_fast_add_free_tvar(_tmp876,_tmp879);cvt.kind_env=_tmpE05;});}}
# 4789
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp87A=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp87B=_tmp87A;void*_tmp884;_LL4: _tmp884=_tmp87B.f1;_LL5:;{
void*_tmp87C=Cyc_Tcutil_compress(_tmp884);void*_tmp87D=_tmp87C;struct Cyc_Core_Opt**_tmp883;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp87D)->tag == 1U){_LL7: _tmp883=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp87D)->f4;_LL8:
# 4793
 if(*_tmp883 == 0)
({struct Cyc_Core_Opt*_tmpE06=({struct Cyc_Core_Opt*_tmp87E=_cycalloc(sizeof(*_tmp87E));_tmp87E->v=_tmp876;_tmp87E;});*_tmp883=_tmpE06;});else{
# 4797
struct Cyc_List_List*_tmp87F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp883))->v;
struct Cyc_List_List*_tmp880=0;
for(0;_tmp87F != 0;_tmp87F=_tmp87F->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp876,(struct Cyc_Absyn_Tvar*)_tmp87F->hd))
({struct Cyc_List_List*_tmpE07=({struct Cyc_List_List*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881->hd=(struct Cyc_Absyn_Tvar*)_tmp87F->hd,_tmp881->tl=_tmp880;_tmp881;});_tmp880=_tmpE07;});}
({struct Cyc_Core_Opt*_tmpE08=({struct Cyc_Core_Opt*_tmp882=_cycalloc(sizeof(*_tmp882));_tmp882->v=_tmp880;_tmp882;});*_tmp883=_tmpE08;});}
# 4804
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4808
return cvt;}
# 4814
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp885=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp886=_tmp885;switch(((struct Cyc_Absyn_Kind*)_tmp886)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp886)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
# 4819
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp887=0U;({struct _dyneither_ptr _tmpE09=({const char*_tmp888="can't unify evar with unique region!";_tag_dyneither(_tmp888,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE09,_tag_dyneither(_tmp887,sizeof(void*),0U));});});
goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 goto _LL6;default: _LL5: _LL6:
# 4824
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp889=0U;({struct _dyneither_ptr _tmpE0A=({const char*_tmp88A="can't unify evar with heap!";_tag_dyneither(_tmp88A,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE0A,_tag_dyneither(_tmp889,sizeof(void*),0U));});});
goto _LL0;}case Cyc_Absyn_EffKind: _LL7: _LL8:
# 4827
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp88B=0U;({struct _dyneither_ptr _tmpE0B=({const char*_tmp88C="can't unify evar with {}!";_tag_dyneither(_tmp88C,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE0B,_tag_dyneither(_tmp88B,sizeof(void*),0U));});});
goto _LL0;default: _LL9: _LLA:
# 4830
({struct Cyc_String_pa_PrintArg_struct _tmp88F=({struct Cyc_String_pa_PrintArg_struct _tmpAAE;_tmpAAE.tag=0U,({
struct _dyneither_ptr _tmpE0C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmpAAE.f1=_tmpE0C;});_tmpAAE;});struct Cyc_String_pa_PrintArg_struct _tmp890=({struct Cyc_String_pa_PrintArg_struct _tmpAAD;_tmpAAD.tag=0U,({struct _dyneither_ptr _tmpE0D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmpAAD.f1=_tmpE0D;});_tmpAAD;});void*_tmp88D[2U];_tmp88D[0]=& _tmp88F,_tmp88D[1]=& _tmp890;({unsigned int _tmpE0F=loc;struct _dyneither_ptr _tmpE0E=({const char*_tmp88E="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp88E,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpE0F,_tmpE0E,_tag_dyneither(_tmp88D,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}}
# 4842
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp891=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp892=({unsigned int _tmpE13=loc;struct Cyc_Tcenv_Tenv*_tmpE12=te;struct Cyc_Tcutil_CVTEnv _tmpE11=({struct Cyc_Tcutil_CVTEnv _tmpAB2;_tmpAB2.r=Cyc_Core_heap_region,_tmpAB2.kind_env=_tmp891,_tmpAB2.free_vars=0,_tmpAB2.free_evars=0,_tmpAB2.generalize_evars=generalize_evars,_tmpAB2.fn_result=0;_tmpAB2;});struct Cyc_Absyn_Kind*_tmpE10=expected_kind;Cyc_Tcutil_check_valid_type(_tmpE13,_tmpE12,_tmpE11,_tmpE10,1,t);});
# 4850
struct Cyc_List_List*_tmp893=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp892.free_vars);
struct Cyc_List_List*_tmp894=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp892.free_evars);
# 4854
if(_tmp891 != 0){
struct Cyc_List_List*_tmp895=0;
{struct Cyc_List_List*_tmp896=_tmp893;for(0;(unsigned int)_tmp896;_tmp896=_tmp896->tl){
struct Cyc_Absyn_Tvar*_tmp897=(struct Cyc_Absyn_Tvar*)_tmp896->hd;
int found=0;
{struct Cyc_List_List*_tmp898=_tmp891;for(0;(unsigned int)_tmp898;_tmp898=_tmp898->tl){
if(Cyc_Absyn_tvar_cmp(_tmp897,(struct Cyc_Absyn_Tvar*)_tmp898->hd)== 0){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpE14=({struct Cyc_List_List*_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899->hd=(struct Cyc_Absyn_Tvar*)_tmp896->hd,_tmp899->tl=_tmp895;_tmp899;});_tmp895=_tmpE14;});}}
# 4864
({struct Cyc_List_List*_tmpE15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp895);_tmp893=_tmpE15;});}
# 4870
{struct Cyc_List_List*x=_tmp893;for(0;x != 0;x=x->tl){
void*_tmp89A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp89B=_tmp89A;enum Cyc_Absyn_AliasQual _tmp8A9;struct Cyc_Core_Opt**_tmp8A8;struct Cyc_Absyn_Kind*_tmp8A7;struct Cyc_Core_Opt**_tmp8A6;struct Cyc_Core_Opt**_tmp8A5;struct Cyc_Core_Opt**_tmp8A4;struct Cyc_Core_Opt**_tmp8A3;struct Cyc_Core_Opt**_tmp8A2;struct Cyc_Core_Opt**_tmp8A1;switch(*((int*)_tmp89B)){case 1U: _LL1: _tmp8A1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_LL2:
 _tmp8A2=_tmp8A1;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp8A2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_LL4:
 _tmp8A3=_tmp8A2;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp8A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_LL6:
 _tmp8A4=_tmp8A3;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp8A4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_LL8:
# 4876
({struct Cyc_Core_Opt*_tmpE16=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp8A4=_tmpE16;});goto _LL0;default: _LL9: _tmp8A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_LLA:
# 4878
({struct Cyc_Core_Opt*_tmpE17=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp8A5=_tmpE17;});goto _LL0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp8A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_LLC:
# 4880
({struct Cyc_Core_Opt*_tmpE18=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp8A6=_tmpE18;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp8A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_tmp8A7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f2;_LLE:
# 4882
({struct Cyc_Core_Opt*_tmpE19=Cyc_Tcutil_kind_to_bound_opt(_tmp8A7);*_tmp8A8=_tmpE19;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89B)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp8A9=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89B)->f1)->aliasqual;_LL10:
# 4884
({struct Cyc_String_pa_PrintArg_struct _tmp89E=({struct Cyc_String_pa_PrintArg_struct _tmpAB0;_tmpAB0.tag=0U,({
struct _dyneither_ptr _tmpE1A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmpAB0.f1=_tmpE1A;});_tmpAB0;});struct Cyc_String_pa_PrintArg_struct _tmp89F=({struct Cyc_String_pa_PrintArg_struct _tmpAAF;_tmpAAF.tag=0U,({struct _dyneither_ptr _tmpE1B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0->kind=Cyc_Absyn_MemKind,_tmp8A0->aliasqual=_tmp8A9;_tmp8A0;})));_tmpAAF.f1=_tmpE1B;});_tmpAAF;});void*_tmp89C[2U];_tmp89C[0]=& _tmp89E,_tmp89C[1]=& _tmp89F;({unsigned int _tmpE1D=loc;struct _dyneither_ptr _tmpE1C=({const char*_tmp89D="type variable %s cannot have kind %s";_tag_dyneither(_tmp89D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpE1D,_tmpE1C,_tag_dyneither(_tmp89C,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4891
if(_tmp893 != 0  || _tmp894 != 0){
{void*_tmp8AA=Cyc_Tcutil_compress(t);void*_tmp8AB=_tmp8AA;struct Cyc_List_List**_tmp8AC;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AB)->tag == 9U){_LL14: _tmp8AC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AB)->f1).tvars;_LL15:
# 4894
 if(*_tmp8AC == 0){
# 4896
({struct Cyc_List_List*_tmpE1E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp893);*_tmp8AC=_tmpE1E;});
_tmp893=0;}
# 4899
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4902
if(_tmp893 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8AF=({struct Cyc_String_pa_PrintArg_struct _tmpAB1;_tmpAB1.tag=0U,_tmpAB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp893->hd)->name);_tmpAB1;});void*_tmp8AD[1U];_tmp8AD[0]=& _tmp8AF;({unsigned int _tmpE20=loc;struct _dyneither_ptr _tmpE1F=({const char*_tmp8AE="unbound type variable %s";_tag_dyneither(_tmp8AE,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE20,_tmpE1F,_tag_dyneither(_tmp8AD,sizeof(void*),1U));});});
Cyc_Tcutil_check_free_evars(_tmp894,t,loc);}}
# 4912
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4915
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp8B0=Cyc_Tcutil_compress(t);void*_tmp8B1=_tmp8B0;struct Cyc_List_List*_tmp8BD;void*_tmp8BC;struct Cyc_Absyn_Tqual _tmp8BB;void*_tmp8BA;struct Cyc_List_List*_tmp8B9;struct Cyc_Absyn_Exp*_tmp8B8;struct Cyc_List_List*_tmp8B7;struct Cyc_Absyn_Exp*_tmp8B6;struct Cyc_List_List*_tmp8B5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->tag == 9U){_LL1: _tmp8BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).tvars;_tmp8BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).effect;_tmp8BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).ret_tqual;_tmp8BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).ret_typ;_tmp8B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).args;_tmp8B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).requires_clause;_tmp8B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).requires_relns;_tmp8B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).ensures_clause;_tmp8B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8B1)->f1).ensures_relns;_LL2:
# 4918
 fd->tvs=_tmp8BD;
fd->effect=_tmp8BC;
{struct Cyc_List_List*_tmp8B2=fd->args;for(0;_tmp8B2 != 0;(_tmp8B2=_tmp8B2->tl,_tmp8B9=_tmp8B9->tl)){
# 4922
(*((struct _tuple10*)_tmp8B2->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp8B9))->hd)).f2;
(*((struct _tuple10*)_tmp8B2->hd)).f3=(*((struct _tuple10*)_tmp8B9->hd)).f3;}}
# 4925
fd->ret_tqual=_tmp8BB;
fd->ret_type=_tmp8BA;
({int _tmpE21=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp8BA);(fd->ret_tqual).real_const=_tmpE21;});
fd->requires_clause=_tmp8B8;
fd->requires_relns=_tmp8B7;
fd->ensures_clause=_tmp8B6;
fd->ensures_relns=_tmp8B5;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp8B3=0U;({struct _dyneither_ptr _tmpE22=({const char*_tmp8B4="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp8B4,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE22,_tag_dyneither(_tmp8B3,sizeof(void*),0U));});});}_LL0:;}
# 4935
({struct Cyc_List_List*_tmpE24=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpE23=loc;Cyc_Tcutil_check_unique_vars(_tmpE24,_tmpE23,({const char*_tmp8BE="function declaration has repeated parameter";_tag_dyneither(_tmp8BE,sizeof(char),44U);}));});
# 4938
fd->cached_typ=t;}
# 4943
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4949
struct Cyc_Tcutil_CVTEnv _tmp8BF=({unsigned int _tmpE29=loc;struct Cyc_Tcenv_Tenv*_tmpE28=te;struct Cyc_Tcutil_CVTEnv _tmpE27=({struct Cyc_Tcutil_CVTEnv _tmpAB5;_tmpAB5.r=Cyc_Core_heap_region,_tmpAB5.kind_env=bound_tvars,_tmpAB5.free_vars=0,_tmpAB5.free_evars=0,_tmpAB5.generalize_evars=0,_tmpAB5.fn_result=0;_tmpAB5;});struct Cyc_Absyn_Kind*_tmpE26=expected_kind;int _tmpE25=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpE29,_tmpE28,_tmpE27,_tmpE26,_tmpE25,t);});
# 4953
struct Cyc_List_List*_tmp8C0=({struct _RegionHandle*_tmpE2B=Cyc_Core_heap_region;struct Cyc_List_List*_tmpE2A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp8BF.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpE2B,_tmpE2A,bound_tvars);});
# 4955
struct Cyc_List_List*_tmp8C1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp8BF.free_evars);
{struct Cyc_List_List*fs=_tmp8C0;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp8C2=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp8C5=({struct Cyc_String_pa_PrintArg_struct _tmpAB4;_tmpAB4.tag=0U,_tmpAB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8C2);_tmpAB4;});struct Cyc_String_pa_PrintArg_struct _tmp8C6=({struct Cyc_String_pa_PrintArg_struct _tmpAB3;_tmpAB3.tag=0U,({struct _dyneither_ptr _tmpE2C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAB3.f1=_tmpE2C;});_tmpAB3;});void*_tmp8C3[2U];_tmp8C3[0]=& _tmp8C5,_tmp8C3[1]=& _tmp8C6;({unsigned int _tmpE2E=loc;struct _dyneither_ptr _tmpE2D=({const char*_tmp8C4="unbound type variable %s in type %s";_tag_dyneither(_tmp8C4,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpE2E,_tmpE2D,_tag_dyneither(_tmp8C3,sizeof(void*),2U));});});}}
# 4960
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp8C1,t,loc);}
# 4964
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpE2F=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpE2F;});}
# 4969
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4975
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4978
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8C9=({struct Cyc_String_pa_PrintArg_struct _tmpAB7;_tmpAB7.tag=0U,_tmpAB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpAB7;});struct Cyc_String_pa_PrintArg_struct _tmp8CA=({struct Cyc_String_pa_PrintArg_struct _tmpAB6;_tmpAB6.tag=0U,({struct _dyneither_ptr _tmpE30=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmpAB6.f1=_tmpE30;});_tmpAB6;});void*_tmp8C7[2U];_tmp8C7[0]=& _tmp8C9,_tmp8C7[1]=& _tmp8CA;({unsigned int _tmpE32=loc;struct _dyneither_ptr _tmpE31=({const char*_tmp8C8="%s: %s";_tag_dyneither(_tmp8C8,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpE32,_tmpE31,_tag_dyneither(_tmp8C7,sizeof(void*),2U));});});}}}
# 4984
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4988
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4992
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpE34=tvs;unsigned int _tmpE33=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpE34,_tmpE33,Cyc_Absynpp_tvar2string,({const char*_tmp8CB="duplicate type variable";_tag_dyneither(_tmp8CB,sizeof(char),24U);}));});}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5006 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5011
struct _RegionHandle _tmp8CC=_new_region("temp");struct _RegionHandle*temp=& _tmp8CC;_push_region(temp);
# 5015
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpE35=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpE35,({const char*_tmp8CD="";_tag_dyneither(_tmp8CD,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpE37=({struct Cyc_List_List*_tmp8CF=_region_malloc(temp,sizeof(*_tmp8CF));({struct _tuple32*_tmpE36=({struct _tuple32*_tmp8CE=_region_malloc(temp,sizeof(*_tmp8CE));_tmp8CE->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp8CE->f2=0;_tmp8CE;});_tmp8CF->hd=_tmpE36;}),_tmp8CF->tl=fields;_tmp8CF;});fields=_tmpE37;});}}
# 5020
({struct Cyc_List_List*_tmpE38=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpE38;});{
# 5022
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp8F4="struct";_tag_dyneither(_tmp8F4,sizeof(char),7U);}):({const char*_tmp8F5="union";_tag_dyneither(_tmp8F5,sizeof(char),6U);});
# 5025
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp8D0=(struct _tuple33*)des->hd;struct _tuple33*_tmp8D1=_tmp8D0;struct Cyc_List_List*_tmp8EC;void*_tmp8EB;_LL1: _tmp8EC=_tmp8D1->f1;_tmp8EB=_tmp8D1->f2;_LL2:;
if(_tmp8EC == 0){
# 5030
struct Cyc_List_List*_tmp8D2=fields;
for(0;_tmp8D2 != 0;_tmp8D2=_tmp8D2->tl){
if(!(*((struct _tuple32*)_tmp8D2->hd)).f2){
(*((struct _tuple32*)_tmp8D2->hd)).f2=1;
({struct Cyc_List_List*_tmpE3A=({struct Cyc_List_List*_tmp8D4=_cycalloc(sizeof(*_tmp8D4));({void*_tmpE39=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp8D3=_cycalloc(sizeof(*_tmp8D3));_tmp8D3->tag=1U,_tmp8D3->f1=((*((struct _tuple32*)_tmp8D2->hd)).f1)->name;_tmp8D3;});_tmp8D4->hd=_tmpE39;}),_tmp8D4->tl=0;_tmp8D4;});(*((struct _tuple33*)des->hd)).f1=_tmpE3A;});
({struct Cyc_List_List*_tmpE3C=({struct Cyc_List_List*_tmp8D6=_region_malloc(rgn,sizeof(*_tmp8D6));({struct _tuple34*_tmpE3B=({struct _tuple34*_tmp8D5=_region_malloc(rgn,sizeof(*_tmp8D5));_tmp8D5->f1=(*((struct _tuple32*)_tmp8D2->hd)).f1,_tmp8D5->f2=_tmp8EB;_tmp8D5;});_tmp8D6->hd=_tmpE3B;}),_tmp8D6->tl=ans;_tmp8D6;});ans=_tmpE3C;});
break;}}
# 5038
if(_tmp8D2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8D9=({struct Cyc_String_pa_PrintArg_struct _tmpAB8;_tmpAB8.tag=0U,_tmpAB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpAB8;});void*_tmp8D7[1U];_tmp8D7[0]=& _tmp8D9;({unsigned int _tmpE3E=loc;struct _dyneither_ptr _tmpE3D=({const char*_tmp8D8="too many arguments to %s";_tag_dyneither(_tmp8D8,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE3E,_tmpE3D,_tag_dyneither(_tmp8D7,sizeof(void*),1U));});});}else{
if(_tmp8EC->tl != 0)
# 5042
({void*_tmp8DA=0U;({unsigned int _tmpE40=loc;struct _dyneither_ptr _tmpE3F=({const char*_tmp8DB="multiple designators are not yet supported";_tag_dyneither(_tmp8DB,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpE40,_tmpE3F,_tag_dyneither(_tmp8DA,sizeof(void*),0U));});});else{
# 5045
void*_tmp8DC=(void*)_tmp8EC->hd;void*_tmp8DD=_tmp8DC;struct _dyneither_ptr*_tmp8EA;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp8DD)->tag == 0U){_LL4: _LL5:
# 5047
({struct Cyc_String_pa_PrintArg_struct _tmp8E0=({struct Cyc_String_pa_PrintArg_struct _tmpAB9;_tmpAB9.tag=0U,_tmpAB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpAB9;});void*_tmp8DE[1U];_tmp8DE[0]=& _tmp8E0;({unsigned int _tmpE42=loc;struct _dyneither_ptr _tmpE41=({const char*_tmp8DF="array designator used in argument to %s";_tag_dyneither(_tmp8DF,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpE42,_tmpE41,_tag_dyneither(_tmp8DE,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp8EA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp8DD)->f1;_LL7: {
# 5050
struct Cyc_List_List*_tmp8E1=fields;
for(0;_tmp8E1 != 0;_tmp8E1=_tmp8E1->tl){
if(Cyc_strptrcmp(_tmp8EA,((*((struct _tuple32*)_tmp8E1->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp8E1->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp8E4=({struct Cyc_String_pa_PrintArg_struct _tmpABA;_tmpABA.tag=0U,_tmpABA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8EA);_tmpABA;});void*_tmp8E2[1U];_tmp8E2[0]=& _tmp8E4;({unsigned int _tmpE44=loc;struct _dyneither_ptr _tmpE43=({const char*_tmp8E3="member %s has already been used as an argument";_tag_dyneither(_tmp8E3,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpE44,_tmpE43,_tag_dyneither(_tmp8E2,sizeof(void*),1U));});});
(*((struct _tuple32*)_tmp8E1->hd)).f2=1;
({struct Cyc_List_List*_tmpE46=({struct Cyc_List_List*_tmp8E6=_region_malloc(rgn,sizeof(*_tmp8E6));({struct _tuple34*_tmpE45=({struct _tuple34*_tmp8E5=_region_malloc(rgn,sizeof(*_tmp8E5));_tmp8E5->f1=(*((struct _tuple32*)_tmp8E1->hd)).f1,_tmp8E5->f2=_tmp8EB;_tmp8E5;});_tmp8E6->hd=_tmpE45;}),_tmp8E6->tl=ans;_tmp8E6;});ans=_tmpE46;});
break;}}
# 5059
if(_tmp8E1 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8E9=({struct Cyc_String_pa_PrintArg_struct _tmpABB;_tmpABB.tag=0U,_tmpABB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8EA);_tmpABB;});void*_tmp8E7[1U];_tmp8E7[0]=& _tmp8E9;({unsigned int _tmpE48=loc;struct _dyneither_ptr _tmpE47=({const char*_tmp8E8="bad field designator %s";_tag_dyneither(_tmp8E8,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpE48,_tmpE47,_tag_dyneither(_tmp8E7,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 5064
if(aggr_kind == Cyc_Absyn_StructA)
# 5066
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp8ED=0U;({unsigned int _tmpE4A=loc;struct _dyneither_ptr _tmpE49=({const char*_tmp8EE="too few arguments to struct";_tag_dyneither(_tmp8EE,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpE4A,_tmpE49,_tag_dyneither(_tmp8ED,sizeof(void*),0U));});});
break;}}else{
# 5073
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp8EF=0U;({unsigned int _tmpE4C=loc;struct _dyneither_ptr _tmpE4B=({const char*_tmp8F0="only one member of a union is allowed";_tag_dyneither(_tmp8F0,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpE4C,_tmpE4B,_tag_dyneither(_tmp8EF,sizeof(void*),0U));});});
found=1;}}
# 5080
if(!found)({void*_tmp8F1=0U;({unsigned int _tmpE4E=loc;struct _dyneither_ptr _tmpE4D=({const char*_tmp8F2="missing member for union";_tag_dyneither(_tmp8F2,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE4E,_tmpE4D,_tag_dyneither(_tmp8F1,sizeof(void*),0U));});});}{
# 5083
struct Cyc_List_List*_tmp8F3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp8F3;};};}
# 5015
;_pop_region(temp);}
# 5086
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8F6=Cyc_Tcutil_compress(t);void*_tmp8F7=_tmp8F6;void*_tmp8FB;union Cyc_Absyn_Constraint*_tmp8FA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F7)->tag == 5U){_LL1: _tmp8FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F7)->f1).elt_typ;_tmp8FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F7)->f1).ptr_atts).bounds;_LL2: {
# 5089
void*_tmp8F8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8FA);void*_tmp8F9=_tmp8F8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8F9)->tag == 0U){_LL6: _LL7:
# 5091
*elt_typ_dest=_tmp8FB;
return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 5095
 return 0;}_LL0:;}
# 5099
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8FC=Cyc_Tcutil_compress(t);void*_tmp8FD=_tmp8FC;void*_tmp8FF;union Cyc_Absyn_Constraint*_tmp8FE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FD)->tag == 5U){_LL1: _tmp8FF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FD)->f1).elt_typ;_tmp8FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FD)->f1).ptr_atts).zero_term;_LL2:
# 5102
*elt_typ_dest=_tmp8FF;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8FE);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5110
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5112
void*_tmp900=Cyc_Tcutil_compress(t);void*_tmp901=_tmp900;void*_tmp90A;struct Cyc_Absyn_Tqual _tmp909;struct Cyc_Absyn_Exp*_tmp908;union Cyc_Absyn_Constraint*_tmp907;void*_tmp906;union Cyc_Absyn_Constraint*_tmp905;union Cyc_Absyn_Constraint*_tmp904;switch(*((int*)_tmp901)){case 5U: _LL1: _tmp906=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp901)->f1).elt_typ;_tmp905=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp901)->f1).ptr_atts).bounds;_tmp904=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp901)->f1).ptr_atts).zero_term;_LL2:
# 5114
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp904)){
*ptr_type=t;
*elt_type=_tmp906;
{void*_tmp902=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp905);void*_tmp903=_tmp902;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp903)->tag == 0U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 5121
return 1;}else{
return 0;}case 8U: _LL3: _tmp90A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp901)->f1).elt_type;_tmp909=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp901)->f1).tq;_tmp908=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp901)->f1).num_elts;_tmp907=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp901)->f1).zero_term;_LL4:
# 5124
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp907)){
*elt_type=_tmp90A;
*is_dyneither=0;
({void*_tmpE4F=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);*ptr_type=_tmpE4F;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5137
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5139
void*_tmp90B=e1->r;void*_tmp90C=_tmp90B;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_Absyn_Exp*_tmp91D;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91A;struct Cyc_Absyn_Exp*_tmp919;switch(*((int*)_tmp90C)){case 14U: _LL1: _LL2:
# 5141
({struct Cyc_String_pa_PrintArg_struct _tmp90F=({struct Cyc_String_pa_PrintArg_struct _tmpABC;_tmpABC.tag=0U,({struct _dyneither_ptr _tmpE50=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpABC.f1=_tmpE50;});_tmpABC;});void*_tmp90D[1U];_tmp90D[0]=& _tmp90F;({struct _dyneither_ptr _tmpE51=({const char*_tmp90E="we have a cast in a lhs:  %s";_tag_dyneither(_tmp90E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE51,_tag_dyneither(_tmp90D,sizeof(void*),1U));});});case 20U: _LL3: _tmp919=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp90C)->f1;_LL4:
 _tmp91A=_tmp919;goto _LL6;case 23U: _LL5: _tmp91A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp90C)->f1;_LL6:
# 5144
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp91A->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp91B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp90C)->f1;_LL8:
 _tmp91C=_tmp91B;goto _LLA;case 21U: _LL9: _tmp91C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp90C)->f1;_LLA:
# 5148
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp91C->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp912=({struct Cyc_String_pa_PrintArg_struct _tmpABD;_tmpABD.tag=0U,({
struct _dyneither_ptr _tmpE52=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpABD.f1=_tmpE52;});_tmpABD;});void*_tmp910[1U];_tmp910[0]=& _tmp912;({struct _dyneither_ptr _tmpE53=({const char*_tmp911="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp911,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE53,_tag_dyneither(_tmp910,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp91D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp90C)->f1;_LLC:
 _tmp91E=_tmp91D;goto _LLE;case 12U: _LLD: _tmp91E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp90C)->f1;_LLE:
# 5154
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp91E->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp915=({struct Cyc_String_pa_PrintArg_struct _tmpABE;_tmpABE.tag=0U,({
struct _dyneither_ptr _tmpE54=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpABE.f1=_tmpE54;});_tmpABE;});void*_tmp913[1U];_tmp913[0]=& _tmp915;({struct _dyneither_ptr _tmpE55=({const char*_tmp914="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp914,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE55,_tag_dyneither(_tmp913,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp918=({struct Cyc_String_pa_PrintArg_struct _tmpABF;_tmpABF.tag=0U,({struct _dyneither_ptr _tmpE56=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpABF.f1=_tmpE56;});_tmpABF;});void*_tmp916[1U];_tmp916[0]=& _tmp918;({struct _dyneither_ptr _tmpE57=({const char*_tmp917="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp917,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE57,_tag_dyneither(_tmp916,sizeof(void*),1U));});});}_LL0:;}
# 5163
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5174
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5177
void*_tmp91F=Cyc_Tcutil_compress(r);void*_tmp920=_tmp91F;struct Cyc_Absyn_Tvar*_tmp92C;enum Cyc_Absyn_KindQual _tmp92B;enum Cyc_Absyn_AliasQual _tmp92A;switch(*((int*)_tmp920)){case 22U: _LL1: _LL2:
 return !must_be_unique;case 21U: _LL3: _LL4:
 return 1;case 17U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp920)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp920)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp920)->f4 == 0){_LL5: _tmp92B=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp920)->f3)->kind)->v)->kind;_tmp92A=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp920)->f3)->kind)->v)->aliasqual;_LL6:
# 5181
 return _tmp92B == Cyc_Absyn_RgnKind  && (_tmp92A == Cyc_Absyn_Unique  || _tmp92A == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp92C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp920)->f1;_LL8: {
# 5185
struct Cyc_Absyn_Kind*_tmp921=Cyc_Tcutil_tvar_kind(_tmp92C,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp922=_tmp921;enum Cyc_Absyn_KindQual _tmp929;enum Cyc_Absyn_AliasQual _tmp928;_LLC: _tmp929=_tmp922->kind;_tmp928=_tmp922->aliasqual;_LLD:;
if(_tmp929 == Cyc_Absyn_RgnKind  && (_tmp928 == Cyc_Absyn_Unique  || _tmp928 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp923=Cyc_Absyn_compress_kb(_tmp92C->kind);void*_tmp924=_tmp923;struct Cyc_Core_Opt**_tmp927;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp924)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp924)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp924)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp927=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp924)->f1;_LL10:
# 5189
({struct Cyc_Core_Opt*_tmpE59=({struct Cyc_Core_Opt*_tmp926=_cycalloc(sizeof(*_tmp926));({void*_tmpE58=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp925=_cycalloc(sizeof(*_tmp925));_tmp925->tag=2U,_tmp925->f1=0,_tmp925->f2=& Cyc_Tcutil_rk;_tmp925;});_tmp926->v=_tmpE58;});_tmp926;});*_tmp927=_tmpE59;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5194
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5201
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp92D=Cyc_Tcutil_compress(t);void*_tmp92E=_tmp92D;struct Cyc_Absyn_Tvar*_tmp93C;void*_tmp93B;switch(*((int*)_tmp92E)){case 5U: _LL1: _tmp93B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp92E)->f1).ptr_atts).rgn;_LL2:
# 5204
 return Cyc_Tcutil_is_noalias_region(_tmp93B,must_be_unique);case 2U: _LL3: _tmp93C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp92E)->f1;_LL4: {
# 5206
struct Cyc_Absyn_Kind*_tmp92F=Cyc_Tcutil_tvar_kind(_tmp93C,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp930=_tmp92F;enum Cyc_Absyn_KindQual _tmp93A;enum Cyc_Absyn_AliasQual _tmp939;_LL8: _tmp93A=_tmp930->kind;_tmp939=_tmp930->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp931=_tmp93A;switch(_tmp931){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if(_tmp939 == Cyc_Absyn_Unique  || _tmp939 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp932=Cyc_Absyn_compress_kb(_tmp93C->kind);void*_tmp933=_tmp932;struct Cyc_Core_Opt**_tmp938;enum Cyc_Absyn_KindQual _tmp937;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp933)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp933)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp938=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp933)->f1;_tmp937=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp933)->f2)->kind;_LL15:
# 5212
({struct Cyc_Core_Opt*_tmpE5C=({struct Cyc_Core_Opt*_tmp936=_cycalloc(sizeof(*_tmp936));({void*_tmpE5B=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp935=_cycalloc(sizeof(*_tmp935));_tmp935->tag=2U,_tmp935->f1=0,({struct Cyc_Absyn_Kind*_tmpE5A=({struct Cyc_Absyn_Kind*_tmp934=_cycalloc(sizeof(*_tmp934));_tmp934->kind=_tmp937,_tmp934->aliasqual=Cyc_Absyn_Aliasable;_tmp934;});_tmp935->f2=_tmpE5A;});_tmp935;});_tmp936->v=_tmpE5B;});_tmp936;});*_tmp938=_tmpE5C;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5216
 return 1;}_LL13:;}
# 5219
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5222
 return 0;}_LL0:;}
# 5225
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp93D=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp93D,0))return 1;{
void*_tmp93E=_tmp93D;union Cyc_Absyn_DatatypeFieldInfoU _tmp952;struct Cyc_List_List*_tmp951;union Cyc_Absyn_DatatypeInfoU _tmp950;struct Cyc_List_List*_tmp94F;struct Cyc_List_List*_tmp94E;struct Cyc_Absyn_Aggrdecl**_tmp94D;struct Cyc_List_List*_tmp94C;struct Cyc_List_List*_tmp94B;switch(*((int*)_tmp93E)){case 10U: _LL1: _tmp94B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp93E)->f1;_LL2:
# 5230
 while(_tmp94B != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp94B->hd)).f2))return 1;
_tmp94B=_tmp94B->tl;}
# 5234
return 0;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp93E)->f1).aggr_info).KnownAggr).tag == 2){_LL3: _tmp94D=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp93E)->f1).aggr_info).KnownAggr).val;_tmp94C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp93E)->f1).targs;_LL4:
# 5236
 if((*_tmp94D)->impl == 0)return 0;else{
# 5238
struct Cyc_List_List*_tmp93F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp94D)->tvs,_tmp94C);
struct Cyc_List_List*_tmp940=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp94D)->impl))->fields;
void*t;
while(_tmp940 != 0){
({void*_tmpE5D=_tmp93F == 0?((struct Cyc_Absyn_Aggrfield*)_tmp940->hd)->type: Cyc_Tcutil_substitute(_tmp93F,((struct Cyc_Absyn_Aggrfield*)_tmp940->hd)->type);t=_tmpE5D;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp940=_tmp940->tl;}
# 5246
return 0;}}else{_LL7: _LL8:
# 5256
 return 0;}case 12U: _LL5: _tmp94E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp93E)->f2;_LL6:
# 5249
 while(_tmp94E != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp94E->hd)->type))return 1;
_tmp94E=_tmp94E->tl;}
# 5253
return 0;case 3U: _LL9: _tmp950=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp93E)->f1).datatype_info;_tmp94F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp93E)->f1).targs;_LLA: {
# 5258
union Cyc_Absyn_DatatypeInfoU _tmp941=_tmp950;struct Cyc_List_List*_tmp945;struct Cyc_Core_Opt*_tmp944;struct _tuple2*_tmp943;int _tmp942;if((_tmp941.UnknownDatatype).tag == 1){_LL10: _tmp943=((_tmp941.UnknownDatatype).val).name;_tmp942=((_tmp941.UnknownDatatype).val).is_extensible;_LL11:
# 5261
 return 0;}else{_LL12: _tmp945=(*(_tmp941.KnownDatatype).val)->tvs;_tmp944=(*(_tmp941.KnownDatatype).val)->fields;_LL13:
# 5264
 return 0;}_LLF:;}case 4U: _LLB: _tmp952=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp93E)->f1).field_info;_tmp951=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp93E)->f1).targs;_LLC: {
# 5267
union Cyc_Absyn_DatatypeFieldInfoU _tmp946=_tmp952;struct Cyc_Absyn_Datatypedecl*_tmp94A;struct Cyc_Absyn_Datatypefield*_tmp949;if((_tmp946.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5270
 return 0;}else{_LL17: _tmp94A=((_tmp946.KnownDatatypefield).val).f1;_tmp949=((_tmp946.KnownDatatypefield).val).f2;_LL18: {
# 5272
struct Cyc_List_List*_tmp947=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp94A->tvs,_tmp951);
struct Cyc_List_List*_tmp948=_tmp949->typs;
while(_tmp948 != 0){
({void*_tmpE5E=_tmp947 == 0?(*((struct _tuple12*)_tmp948->hd)).f2: Cyc_Tcutil_substitute(_tmp947,(*((struct _tuple12*)_tmp948->hd)).f2);_tmp93D=_tmpE5E;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp93D))return 1;
_tmp948=_tmp948->tl;}
# 5279
return 0;}}_LL14:;}default: _LLD: _LLE:
# 5281
 return 0;}_LL0:;};}
# 5288
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp953=e->r;void*_tmp954=_tmp953;struct Cyc_Absyn_Stmt*_tmp96A;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp963;struct _dyneither_ptr*_tmp962;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*_tmp960;switch(*((int*)_tmp954)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp954)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp960=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp954)->f1;_LL4:
 _tmp961=_tmp960;goto _LL6;case 20U: _LL5: _tmp961=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp954)->f1;_LL6:
# 5293
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp961->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp961);case 21U: _LL7: _tmp963=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp954)->f1;_tmp962=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp954)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp963);case 23U: _LL9: _tmp965=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp954)->f1;_tmp964=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp954)->f2;_LLA: {
# 5296
void*_tmp955=Cyc_Tcutil_compress((void*)_check_null(_tmp965->topt));void*_tmp956=_tmp955;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp956)->tag == 10U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp965);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp967=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp954)->f2;_tmp966=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp954)->f3;_LLC:
# 5301
 return Cyc_Tcutil_is_noalias_path(_tmp967) && Cyc_Tcutil_is_noalias_path(_tmp966);case 9U: _LLD: _tmp968=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp954)->f2;_LLE:
 _tmp969=_tmp968;goto _LL10;case 14U: _LLF: _tmp969=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp954)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp969);case 37U: _LL11: _tmp96A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp954)->f1;_LL12:
# 5305
 while(1){
void*_tmp957=_tmp96A->r;void*_tmp958=_tmp957;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Decl*_tmp95E;struct Cyc_Absyn_Stmt*_tmp95D;struct Cyc_Absyn_Stmt*_tmp95C;struct Cyc_Absyn_Stmt*_tmp95B;switch(*((int*)_tmp958)){case 2U: _LL1B: _tmp95C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp958)->f1;_tmp95B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp958)->f2;_LL1C:
 _tmp96A=_tmp95B;goto _LL1A;case 12U: _LL1D: _tmp95E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp958)->f1;_tmp95D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp958)->f2;_LL1E:
 _tmp96A=_tmp95D;goto _LL1A;case 1U: _LL1F: _tmp95F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp958)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp95F);default: _LL21: _LL22:
({void*_tmp959=0U;({struct _dyneither_ptr _tmpE5F=({const char*_tmp95A="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp95A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE5F,_tag_dyneither(_tmp959,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5313
 return 1;}_LL0:;}
# 5330 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5332
struct _tuple18 bogus_ans=({struct _tuple18 _tmpACC;_tmpACC.f1=0,_tmpACC.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpACC;});
void*_tmp96B=e->r;void*_tmp96C=_tmp96B;struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*_tmp9A2;struct _dyneither_ptr*_tmp9A1;int _tmp9A0;struct Cyc_Absyn_Exp*_tmp99F;struct _dyneither_ptr*_tmp99E;int _tmp99D;void*_tmp99C;switch(*((int*)_tmp96C)){case 1U: _LL1: _tmp99C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp96C)->f1;_LL2: {
# 5336
void*_tmp96D=_tmp99C;struct Cyc_Absyn_Vardecl*_tmp975;struct Cyc_Absyn_Vardecl*_tmp974;struct Cyc_Absyn_Vardecl*_tmp973;struct Cyc_Absyn_Vardecl*_tmp972;switch(*((int*)_tmp96D)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp972=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp96D)->f1;_LL13: {
# 5340
void*_tmp96E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp96F=_tmp96E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp96F)->tag == 8U){_LL1B: _LL1C:
# 5342
 return({struct _tuple18 _tmpAC0;_tmpAC0.f1=1,_tmpAC0.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAC0;});}else{_LL1D: _LL1E:
 return({struct _tuple18 _tmpAC1;_tmpAC1.f1=(_tmp972->tq).real_const,_tmpAC1.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAC1;});}_LL1A:;}case 4U: _LL14: _tmp973=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp96D)->f1;_LL15: {
# 5346
void*_tmp970=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp971=_tmp970;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp971)->tag == 8U){_LL20: _LL21:
 return({struct _tuple18 _tmpAC2;_tmpAC2.f1=1,_tmpAC2.f2=(void*)_check_null(_tmp973->rgn);_tmpAC2;});}else{_LL22: _LL23:
# 5349
 _tmp973->escapes=1;
return({struct _tuple18 _tmpAC3;_tmpAC3.f1=(_tmp973->tq).real_const,_tmpAC3.f2=(void*)_check_null(_tmp973->rgn);_tmpAC3;});}_LL1F:;}case 5U: _LL16: _tmp974=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp96D)->f1;_LL17:
# 5352
 _tmp975=_tmp974;goto _LL19;default: _LL18: _tmp975=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp96D)->f1;_LL19:
# 5354
 _tmp975->escapes=1;
return({struct _tuple18 _tmpAC4;_tmpAC4.f1=(_tmp975->tq).real_const,_tmpAC4.f2=(void*)_check_null(_tmp975->rgn);_tmpAC4;});}_LLD:;}case 21U: _LL3: _tmp99F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp96C)->f1;_tmp99E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp96C)->f2;_tmp99D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp96C)->f3;_LL4:
# 5359
 if(_tmp99D)return bogus_ans;{
# 5362
void*_tmp976=Cyc_Tcutil_compress((void*)_check_null(_tmp99F->topt));void*_tmp977=_tmp976;struct Cyc_Absyn_Aggrdecl*_tmp983;struct Cyc_List_List*_tmp982;switch(*((int*)_tmp977)){case 12U: _LL25: _tmp982=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp977)->f2;_LL26: {
# 5364
struct Cyc_Absyn_Aggrfield*_tmp978=Cyc_Absyn_lookup_field(_tmp982,_tmp99E);
if(_tmp978 != 0  && _tmp978->width == 0){
struct _tuple18 _tmp979=Cyc_Tcutil_addressof_props(te,_tmp99F);struct _tuple18 _tmp97A=_tmp979;int _tmp97C;void*_tmp97B;_LL2C: _tmp97C=_tmp97A.f1;_tmp97B=_tmp97A.f2;_LL2D:;
return({struct _tuple18 _tmpAC5;_tmpAC5.f1=(_tmp978->tq).real_const  || _tmp97C,_tmpAC5.f2=_tmp97B;_tmpAC5;});}
# 5369
return bogus_ans;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp977)->f1).aggr_info).KnownAggr).tag == 2){_LL27: _tmp983=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp977)->f1).aggr_info).KnownAggr).val;_LL28: {
# 5371
struct Cyc_Absyn_Aggrfield*_tmp97D=Cyc_Absyn_lookup_decl_field(_tmp983,_tmp99E);
if(_tmp97D != 0  && _tmp97D->width == 0){
struct _tuple18 _tmp97E=Cyc_Tcutil_addressof_props(te,_tmp99F);struct _tuple18 _tmp97F=_tmp97E;int _tmp981;void*_tmp980;_LL2F: _tmp981=_tmp97F.f1;_tmp980=_tmp97F.f2;_LL30:;
return({struct _tuple18 _tmpAC6;_tmpAC6.f1=(_tmp97D->tq).real_const  || _tmp981,_tmpAC6.f2=_tmp980;_tmpAC6;});}
# 5376
return bogus_ans;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp9A2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp96C)->f1;_tmp9A1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp96C)->f2;_tmp9A0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp96C)->f3;_LL6:
# 5381
 if(_tmp9A0)return bogus_ans;{
# 5384
void*_tmp984=Cyc_Tcutil_compress((void*)_check_null(_tmp9A2->topt));void*_tmp985=_tmp984;void*_tmp98B;void*_tmp98A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp985)->tag == 5U){_LL32: _tmp98B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp985)->f1).elt_typ;_tmp98A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp985)->f1).ptr_atts).rgn;_LL33: {
# 5386
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp986=Cyc_Tcutil_compress(_tmp98B);void*_tmp987=_tmp986;struct Cyc_Absyn_Aggrdecl*_tmp989;struct Cyc_List_List*_tmp988;switch(*((int*)_tmp987)){case 12U: _LL37: _tmp988=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp987)->f2;_LL38:
# 5389
({struct Cyc_Absyn_Aggrfield*_tmpE60=Cyc_Absyn_lookup_field(_tmp988,_tmp9A1);finfo=_tmpE60;});goto _LL36;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp987)->f1).aggr_info).KnownAggr).tag == 2){_LL39: _tmp989=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp987)->f1).aggr_info).KnownAggr).val;_LL3A:
# 5391
({struct Cyc_Absyn_Aggrfield*_tmpE61=Cyc_Absyn_lookup_decl_field(_tmp989,_tmp9A1);finfo=_tmpE61;});goto _LL36;}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5394
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpAC7;_tmpAC7.f1=(finfo->tq).real_const,_tmpAC7.f2=_tmp98A;_tmpAC7;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp9A3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp96C)->f1;_LL8: {
# 5401
void*_tmp98C=Cyc_Tcutil_compress((void*)_check_null(_tmp9A3->topt));void*_tmp98D=_tmp98C;struct Cyc_Absyn_Tqual _tmp98F;void*_tmp98E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98D)->tag == 5U){_LL3E: _tmp98F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98D)->f1).elt_tq;_tmp98E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98D)->f1).ptr_atts).rgn;_LL3F:
# 5403
 return({struct _tuple18 _tmpAC8;_tmpAC8.f1=_tmp98F.real_const,_tmpAC8.f2=_tmp98E;_tmpAC8;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp9A5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp96C)->f1;_tmp9A4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp96C)->f2;_LLA: {
# 5409
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp9A5->topt));
void*_tmp990=t;struct Cyc_Absyn_Tqual _tmp999;struct Cyc_Absyn_Tqual _tmp998;void*_tmp997;struct Cyc_List_List*_tmp996;switch(*((int*)_tmp990)){case 10U: _LL43: _tmp996=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp990)->f1;_LL44: {
# 5413
struct _tuple14 _tmp991=Cyc_Evexp_eval_const_uint_exp(_tmp9A4);struct _tuple14 _tmp992=_tmp991;unsigned int _tmp995;int _tmp994;_LL4C: _tmp995=_tmp992.f1;_tmp994=_tmp992.f2;_LL4D:;
if(!_tmp994)
return bogus_ans;{
struct _tuple12*_tmp993=Cyc_Absyn_lookup_tuple_field(_tmp996,(int)_tmp995);
if(_tmp993 != 0)
return({struct _tuple18 _tmpAC9;_tmpAC9.f1=((*_tmp993).f1).real_const,({void*_tmpE62=(Cyc_Tcutil_addressof_props(te,_tmp9A5)).f2;_tmpAC9.f2=_tmpE62;});_tmpAC9;});
return bogus_ans;};}case 5U: _LL45: _tmp998=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp990)->f1).elt_tq;_tmp997=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp990)->f1).ptr_atts).rgn;_LL46:
# 5421
 return({struct _tuple18 _tmpACA;_tmpACA.f1=_tmp998.real_const,_tmpACA.f2=_tmp997;_tmpACA;});case 8U: _LL47: _tmp999=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp990)->f1).tq;_LL48:
# 5427
 return({struct _tuple18 _tmpACB;_tmpACB.f1=_tmp999.real_const,({void*_tmpE63=(Cyc_Tcutil_addressof_props(te,_tmp9A5)).f2;_tmpACB.f2=_tmpE63;});_tmpACB;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5431
({void*_tmp99A=0U;({unsigned int _tmpE65=e->loc;struct _dyneither_ptr _tmpE64=({const char*_tmp99B="unary & applied to non-lvalue";_tag_dyneither(_tmp99B,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpE65,_tmpE64,_tag_dyneither(_tmp99A,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5438
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
({union Cyc_Absyn_Constraint*_tmpE66=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);b=_tmpE66;});{
void*_tmp9A6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmp9A7=_tmp9A6;struct Cyc_Absyn_Exp*_tmp9B0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9A7)->tag == 0U){_LL1: _LL2:
 return;}else{_LL3: _tmp9B0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9A7)->f1;_LL4: {
# 5443
struct _tuple14 _tmp9A8=Cyc_Evexp_eval_const_uint_exp(_tmp9B0);struct _tuple14 _tmp9A9=_tmp9A8;unsigned int _tmp9AF;int _tmp9AE;_LL6: _tmp9AF=_tmp9A9.f1;_tmp9AE=_tmp9A9.f2;_LL7:;
if(_tmp9AE  && _tmp9AF <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmp9AC=({struct Cyc_Int_pa_PrintArg_struct _tmpACE;_tmpACE.tag=1U,_tmpACE.f1=(unsigned long)((int)_tmp9AF);_tmpACE;});struct Cyc_Int_pa_PrintArg_struct _tmp9AD=({struct Cyc_Int_pa_PrintArg_struct _tmpACD;_tmpACD.tag=1U,_tmpACD.f1=(unsigned long)((int)i);_tmpACD;});void*_tmp9AA[2U];_tmp9AA[0]=& _tmp9AC,_tmp9AA[1]=& _tmp9AD;({unsigned int _tmpE68=loc;struct _dyneither_ptr _tmpE67=({const char*_tmp9AB="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp9AB,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE68,_tmpE67,_tag_dyneither(_tmp9AA,sizeof(void*),2U));});});
return;}}_LL0:;};}
# 5450
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0U,b);}
# 5454
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmp9B1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmp9B2=_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B7;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9B2)->tag == 1U){_LL1: _tmp9B7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9B2)->f1;_LL2: {
# 5457
struct _tuple14 _tmp9B3=Cyc_Evexp_eval_const_uint_exp(_tmp9B7);struct _tuple14 _tmp9B4=_tmp9B3;unsigned int _tmp9B6;int _tmp9B5;_LL6: _tmp9B6=_tmp9B4.f1;_tmp9B5=_tmp9B4.f2;_LL7:;
return _tmp9B5  && _tmp9B6 == 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5463
int Cyc_Tcutil_bits_only(void*t){
void*_tmp9B8=Cyc_Tcutil_compress(t);void*_tmp9B9=_tmp9B8;struct Cyc_List_List*_tmp9C3;struct Cyc_Absyn_Aggrdecl*_tmp9C2;struct Cyc_List_List*_tmp9C1;struct Cyc_List_List*_tmp9C0;void*_tmp9BF;union Cyc_Absyn_Constraint*_tmp9BE;switch(*((int*)_tmp9B9)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 return 0;case 8U: _LLB: _tmp9BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B9)->f1).elt_type;_tmp9BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B9)->f1).zero_term;_LLC:
# 5473
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9BE) && Cyc_Tcutil_bits_only(_tmp9BF);case 10U: _LLD: _tmp9C0=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9B9)->f1;_LLE:
# 5475
 for(0;_tmp9C0 != 0;_tmp9C0=_tmp9C0->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmp9C0->hd)).f2))return 0;}
return 1;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9B9)->f1).aggr_info).UnknownAggr).tag == 1){_LLF: _LL10:
 return 0;}else{_LL11: _tmp9C2=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9B9)->f1).aggr_info).KnownAggr).val;_tmp9C1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9B9)->f1).targs;_LL12:
# 5480
 if(_tmp9C2->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9C2->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp9BA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9BA;_push_region(rgn);
{struct Cyc_List_List*_tmp9BB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp9C2->tvs,_tmp9C1);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9C2->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp9BB,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp9BC=0;_npop_handler(0U);return _tmp9BC;}}}{
int _tmp9BD=1;_npop_handler(0U);return _tmp9BD;};}
# 5487
;_pop_region(rgn);};};}case 12U: _LL13: _tmp9C3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9B9)->f2;_LL14:
# 5492
 for(0;_tmp9C3 != 0;_tmp9C3=_tmp9C3->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmp9C3->hd)->type))return 0;}
return 1;default: _LL15: _LL16:
 return 0;}_LL0:;}
# 5500
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp9C4=e->r;void*_tmp9C5=_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C6;switch(*((int*)_tmp9C5)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp9C6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9C5)->f1;_LL4:
 _tmp9C7=_tmp9C6;goto _LL6;case 13U: _LL5: _tmp9C7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9C5)->f1;_LL6:
 e=_tmp9C7;continue;default: _LL7: _LL8:
# 5507
 return 0;}_LL0:;}}
# 5517
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp9C8=e->r;void*_tmp9C9=_tmp9C8;struct Cyc_List_List*_tmp9E8;struct Cyc_List_List*_tmp9E7;enum Cyc_Absyn_Primop _tmp9E6;struct Cyc_List_List*_tmp9E5;struct Cyc_List_List*_tmp9E4;struct Cyc_List_List*_tmp9E3;struct Cyc_List_List*_tmp9E2;struct Cyc_Absyn_Exp*_tmp9E1;struct Cyc_Absyn_Exp*_tmp9E0;struct Cyc_Absyn_Exp*_tmp9DF;struct Cyc_Absyn_Exp*_tmp9DE;void*_tmp9DD;struct Cyc_Absyn_Exp*_tmp9DC;void*_tmp9DB;struct Cyc_Absyn_Exp*_tmp9DA;struct Cyc_Absyn_Exp*_tmp9D9;struct Cyc_Absyn_Exp*_tmp9D8;struct Cyc_Absyn_Exp*_tmp9D7;struct Cyc_Absyn_Exp*_tmp9D6;struct Cyc_Absyn_Exp*_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D4;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D2;void*_tmp9D1;switch(*((int*)_tmp9C9)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp9D1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL10: {
# 5529
void*_tmp9CA=_tmp9D1;struct Cyc_Absyn_Vardecl*_tmp9D0;struct Cyc_Absyn_Vardecl*_tmp9CF;switch(*((int*)_tmp9CA)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp9CF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp9CA)->f1;_LL37: {
# 5532
void*_tmp9CB=Cyc_Tcutil_compress(_tmp9CF->type);void*_tmp9CC=_tmp9CB;switch(*((int*)_tmp9CC)){case 8U: _LL3F: _LL40:
 goto _LL42;case 9U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp9D0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp9CA)->f1;_LL39:
# 5539
 if(_tmp9D0->sc == Cyc_Absyn_Static){
void*_tmp9CD=Cyc_Tcutil_compress(_tmp9D0->type);void*_tmp9CE=_tmp9CD;switch(*((int*)_tmp9CE)){case 8U: _LL46: _LL47:
 goto _LL49;case 9U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 5546
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp9D4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_tmp9D3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9C9)->f2;_tmp9D2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9C9)->f3;_LL12:
# 5552
 return(Cyc_Tcutil_cnst_exp(0,_tmp9D4) && 
Cyc_Tcutil_cnst_exp(0,_tmp9D3)) && 
Cyc_Tcutil_cnst_exp(0,_tmp9D2);case 9U: _LL13: _tmp9D6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_tmp9D5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9C9)->f2;_LL14:
# 5556
 return Cyc_Tcutil_cnst_exp(0,_tmp9D6) && Cyc_Tcutil_cnst_exp(0,_tmp9D5);case 41U: _LL15: _tmp9D7=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL16:
 _tmp9D8=_tmp9D7;goto _LL18;case 12U: _LL17: _tmp9D8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL18:
 _tmp9D9=_tmp9D8;goto _LL1A;case 13U: _LL19: _tmp9D9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL1A:
# 5560
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9D9);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C9)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp9DB=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_tmp9DA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C9)->f2;_LL1C:
# 5562
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9DA);}else{_LL1D: _tmp9DD=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_tmp9DC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C9)->f2;_LL1E:
# 5565
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9DC);}case 15U: _LL1F: _tmp9DE=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL20:
# 5567
 return Cyc_Tcutil_cnst_exp(1,_tmp9DE);case 27U: _LL21: _tmp9E0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9C9)->f2;_tmp9DF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9C9)->f3;_LL22:
# 5569
 return Cyc_Tcutil_cnst_exp(0,_tmp9E0) && Cyc_Tcutil_cnst_exp(0,_tmp9DF);case 28U: _LL23: _tmp9E1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL24:
# 5571
 return Cyc_Tcutil_cnst_exp(0,_tmp9E1);case 26U: _LL25: _tmp9E2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL26:
 _tmp9E3=_tmp9E2;goto _LL28;case 30U: _LL27: _tmp9E3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9C9)->f2;_LL28:
 _tmp9E4=_tmp9E3;goto _LL2A;case 29U: _LL29: _tmp9E4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9C9)->f3;_LL2A:
# 5575
 for(0;_tmp9E4 != 0;_tmp9E4=_tmp9E4->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmp9E4->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp9E6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_tmp9E5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9C9)->f2;_LL2C:
# 5580
 _tmp9E7=_tmp9E5;goto _LL2E;case 24U: _LL2D: _tmp9E7=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL2E:
 _tmp9E8=_tmp9E7;goto _LL30;case 31U: _LL2F: _tmp9E8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9C9)->f1;_LL30:
# 5583
 for(0;_tmp9E8 != 0;_tmp9E8=_tmp9E8->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp9E8->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 5591
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5595
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5597
int Cyc_Tcutil_supports_default(void*t){
void*_tmp9E9=Cyc_Tcutil_compress(t);void*_tmp9EA=_tmp9E9;struct Cyc_List_List*_tmp9F4;union Cyc_Absyn_AggrInfoU _tmp9F3;struct Cyc_List_List*_tmp9F2;struct Cyc_List_List*_tmp9F1;void*_tmp9F0;union Cyc_Absyn_Constraint*_tmp9EF;union Cyc_Absyn_Constraint*_tmp9EE;switch(*((int*)_tmp9EA)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 5U: _LL7: _tmp9EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EA)->f1).ptr_atts).nullable;_tmp9EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EA)->f1).ptr_atts).bounds;_LL8: {
# 5604
void*_tmp9EB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9EE);void*_tmp9EC=_tmp9EB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9EC)->tag == 0U){_LL18: _LL19:
 return 1;}else{_LL1A: _LL1B:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp9EF);}_LL17:;}case 8U: _LL9: _tmp9F0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9EA)->f1).elt_type;_LLA:
# 5609
 return Cyc_Tcutil_supports_default(_tmp9F0);case 10U: _LLB: _tmp9F1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9EA)->f1;_LLC:
# 5611
 for(0;_tmp9F1 != 0;_tmp9F1=_tmp9F1->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp9F1->hd)).f2))return 0;}
return 1;case 11U: _LLD: _tmp9F3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9EA)->f1).aggr_info;_tmp9F2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9EA)->f1).targs;_LLE: {
# 5615
struct Cyc_Absyn_Aggrdecl*_tmp9ED=Cyc_Absyn_get_known_aggrdecl(_tmp9F3);
if(_tmp9ED->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9ED->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmp9ED->tvs,_tmp9F2,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9ED->impl))->fields);}case 12U: _LLF: _tmp9F4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9EA)->f2;_LL10:
 return Cyc_Tcutil_fields_support_default(0,0,_tmp9F4);case 14U: _LL11: _LL12:
# 5621
 goto _LL14;case 13U: _LL13: _LL14:
 return 1;default: _LL15: _LL16:
# 5624
 return 0;}_LL0:;}
# 5629
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp9F5=t;struct Cyc_Absyn_Typedefdecl*_tmp9FA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9F5)->tag == 17U){_LL1: _tmp9FA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9F5)->f3;_LL2:
# 5632
 if(_tmp9FA != 0){
struct Cyc_Absyn_Tqual _tmp9F6=_tmp9FA->tq;
if(((_tmp9F6.print_const  || _tmp9F6.q_volatile) || _tmp9F6.q_restrict) || _tmp9F6.real_const)
# 5637
({struct Cyc_String_pa_PrintArg_struct _tmp9F9=({struct Cyc_String_pa_PrintArg_struct _tmpACF;_tmpACF.tag=0U,({struct _dyneither_ptr _tmpE69=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpACF.f1=_tmpE69;});_tmpACF;});void*_tmp9F7[1U];_tmp9F7[0]=& _tmp9F9;({unsigned int _tmpE6B=loc;struct _dyneither_ptr _tmpE6A=({const char*_tmp9F8="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp9F8,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpE6B,_tmpE6A,_tag_dyneither(_tmp9F7,sizeof(void*),1U));});});}
# 5640
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5645
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5647
struct _RegionHandle _tmp9FB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9FB;_push_region(rgn);
{struct Cyc_List_List*_tmp9FC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmp9FD=1;_npop_handler(0U);return _tmp9FD;}
({void*_tmpE6C=Cyc_Tcutil_rsubstitute(rgn,_tmp9FC,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpE6C;});
if(!Cyc_Tcutil_supports_default(t)){int _tmp9FE=0;_npop_handler(0U);return _tmp9FE;}}{
# 5655
int _tmp9FF=1;_npop_handler(0U);return _tmp9FF;};}
# 5648
;_pop_region(rgn);}
# 5661
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpA00=Cyc_Tcutil_compress(t);void*_tmpA01=_tmpA00;union Cyc_Absyn_Constraint*_tmpA05;union Cyc_Absyn_Constraint*_tmpA04;switch(*((int*)_tmpA01)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;case 5U: _LL5: _tmpA05=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA01)->f1).ptr_atts).nullable;_tmpA04=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA01)->f1).ptr_atts).bounds;_LL6: {
# 5666
void*_tmpA02=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA04);void*_tmpA03=_tmpA02;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA03)->tag == 0U){_LLA: _LLB:
# 5670
 return 0;}else{_LLC: _LLD:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA05);}_LL9:;}default: _LL7: _LL8:
# 5673
 return 0;}_LL0:;}
# 5677
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpA06=Cyc_Tcutil_compress(t);void*_tmpA07=_tmpA06;struct Cyc_List_List*_tmpA0B;void*_tmpA0A;switch(*((int*)_tmpA07)){case 5U: _LL1: _tmpA0A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA07)->f1).elt_typ;_LL2:
 return Cyc_Tcutil_is_noreturn(_tmpA0A);case 9U: _LL3: _tmpA0B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA07)->f1).attributes;_LL4:
# 5681
 for(0;_tmpA0B != 0;_tmpA0B=_tmpA0B->tl){
void*_tmpA08=(void*)_tmpA0B->hd;void*_tmpA09=_tmpA08;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpA09)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 5686
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5689
return 0;}
# 5694
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpA0C=Cyc_Tcutil_compress(t);void*_tmpA0D=_tmpA0C;struct Cyc_List_List**_tmpA13;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0D)->tag == 9U){_LL1: _tmpA13=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0D)->f1).attributes;_LL2: {
# 5697
struct Cyc_List_List*_tmpA0E=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpA13,(void*)atts->hd))
({struct Cyc_List_List*_tmpE6D=({struct Cyc_List_List*_tmpA0F=_cycalloc(sizeof(*_tmpA0F));_tmpA0F->hd=(void*)atts->hd,_tmpA0F->tl=*_tmpA13;_tmpA0F;});*_tmpA13=_tmpE6D;});}else{
# 5704
({struct Cyc_List_List*_tmpE6E=({struct Cyc_List_List*_tmpA10=_cycalloc(sizeof(*_tmpA10));_tmpA10->hd=(void*)atts->hd,_tmpA10->tl=_tmpA0E;_tmpA10;});_tmpA0E=_tmpE6E;});}}
return _tmpA0E;}}else{_LL3: _LL4:
({void*_tmpA11=0U;({struct _dyneither_ptr _tmpE6F=({const char*_tmpA12="transfer_fn_type_atts";_tag_dyneither(_tmpA12,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE6F,_tag_dyneither(_tmpA11,sizeof(void*),0U));});});}_LL0:;}
# 5711
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpA14=Cyc_Tcutil_compress(t);void*_tmpA15=_tmpA14;struct Cyc_Absyn_Exp*_tmpA1A;struct Cyc_Absyn_PtrInfo*_tmpA19;switch(*((int*)_tmpA15)){case 5U: _LL1: _tmpA19=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA15)->f1;_LL2:
# 5715
{void*_tmpA16=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpA19->ptr_atts).bounds);void*_tmpA17=_tmpA16;struct Cyc_Absyn_Exp*_tmpA18;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA17)->tag == 1U){_LL8: _tmpA18=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA17)->f1;_LL9:
 bound=_tmpA18;goto _LL7;}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 5719
goto _LL0;case 8U: _LL3: _tmpA1A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA15)->f1).num_elts;_LL4:
# 5721
 bound=_tmpA1A;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5725
return bound;}
# 5730
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpA1B=b;struct Cyc_Absyn_Vardecl*_tmpA1F;struct Cyc_Absyn_Vardecl*_tmpA1E;struct Cyc_Absyn_Vardecl*_tmpA1D;struct Cyc_Absyn_Vardecl*_tmpA1C;switch(*((int*)_tmpA1B)){case 5U: _LL1: _tmpA1C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA1B)->f1;_LL2:
 _tmpA1D=_tmpA1C;goto _LL4;case 4U: _LL3: _tmpA1D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA1B)->f1;_LL4:
 _tmpA1E=_tmpA1D;goto _LL6;case 3U: _LL5: _tmpA1E=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA1B)->f1;_LL6:
 _tmpA1F=_tmpA1E;goto _LL8;case 1U: _LL7: _tmpA1F=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA1B)->f1;_LL8:
# 5736
 if(!_tmpA1F->escapes)return _tmpA1F;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5740
return 0;}
# 5744
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpA20=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpE70=({struct Cyc_List_List*_tmpA21=_cycalloc(sizeof(*_tmpA21));_tmpA21->hd=*((void**)_check_null((void**)x->hd)),_tmpA21->tl=_tmpA20;_tmpA21;});_tmpA20=_tmpE70;});}}
return _tmpA20;}
# 5751
int Cyc_Tcutil_is_array(void*t){
void*_tmpA22=Cyc_Tcutil_compress(t);void*_tmpA23=_tmpA22;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA23)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5758
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpA24=Cyc_Tcutil_compress(t);void*_tmpA25=_tmpA24;void*_tmpA32;struct Cyc_Absyn_Tqual _tmpA31;struct Cyc_Absyn_Exp*_tmpA30;union Cyc_Absyn_Constraint*_tmpA2F;unsigned int _tmpA2E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA25)->tag == 8U){_LL1: _tmpA32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA25)->f1).elt_type;_tmpA31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA25)->f1).tq;_tmpA30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA25)->f1).num_elts;_tmpA2F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA25)->f1).zero_term;_tmpA2E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA25)->f1).zt_loc;_LL2: {
# 5761
void*b;
if(_tmpA30 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5765
struct Cyc_Absyn_Exp*bnd=_tmpA30;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpA26=0U;({struct _dyneither_ptr _tmpE71=({const char*_tmpA27="cannot convert tag without type!";_tag_dyneither(_tmpA27,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE71,_tag_dyneither(_tmpA26,sizeof(void*),0U));});});{
void*_tmpA28=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpA29=_tmpA28;void*_tmpA2C;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA29)->tag == 19U){_LL6: _tmpA2C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA29)->f1;_LL7:
# 5771
({void*_tmpE74=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA2A=_cycalloc(sizeof(*_tmpA2A));_tmpA2A->tag=1U,({struct Cyc_Absyn_Exp*_tmpE73=({void*_tmpE72=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmpE72,Cyc_Absyn_valueof_exp(_tmpA2C,0U),0,Cyc_Absyn_No_coercion,0U);});_tmpA2A->f1=_tmpE73;});_tmpA2A;});b=_tmpE74;});
goto _LL5;}else{_LL8: _LL9:
# 5774
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpE75=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA2B=_cycalloc(sizeof(*_tmpA2B));_tmpA2B->tag=1U,_tmpA2B->f1=bnd;_tmpA2B;});b=_tmpE75;});else{
# 5777
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL5:;};}else{
# 5781
({void*_tmpE76=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA2D=_cycalloc(sizeof(*_tmpA2D));_tmpA2D->tag=1U,_tmpA2D->f1=bnd;_tmpA2D;});b=_tmpE76;});}}
# 5783
return Cyc_Absyn_atb_typ(_tmpA32,rgn,_tmpA31,b,_tmpA2F);}}else{_LL3: _LL4:
# 5786
 return t;}_LL0:;}
# 5791
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpA33=Cyc_Tcutil_compress(t);void*_tmpA34=_tmpA33;struct Cyc_List_List*_tmpA42;union Cyc_Absyn_AggrInfoU _tmpA41;struct Cyc_List_List*_tmpA40;struct Cyc_List_List*_tmpA3F;void*_tmpA3E;union Cyc_Absyn_Constraint*_tmpA3D;switch(*((int*)_tmpA34)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 return 0;case 5U: _LLB: _tmpA3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA34)->f1).ptr_atts).nullable;_LLC:
# 5799
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpA3D);case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmpA3E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA34)->f1).elt_type;_LL12:
 return Cyc_Tcutil_zeroable_type(_tmpA3E);case 9U: _LL13: _LL14:
 return 0;case 10U: _LL15: _tmpA3F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA34)->f1;_LL16:
# 5805
 for(0;(unsigned int)_tmpA3F;_tmpA3F=_tmpA3F->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpA3F->hd)).f2))return 0;}
return 1;case 11U: _LL17: _tmpA41=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA34)->f1).aggr_info;_tmpA40=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA34)->f1).targs;_LL18: {
# 5809
struct Cyc_Absyn_Aggrdecl*_tmpA35=Cyc_Absyn_get_known_aggrdecl(_tmpA41);
if(_tmpA35->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA35->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpA36=_new_region("r");struct _RegionHandle*r=& _tmpA36;_push_region(r);
{struct Cyc_List_List*_tmpA37=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpA35->tvs,_tmpA40);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA35->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpA37,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA38=0;_npop_handler(0U);return _tmpA38;}}}{
int _tmpA39=1;_npop_handler(0U);return _tmpA39;};}
# 5813
;_pop_region(r);};}case 13U: _LL19: _LL1A:
# 5817
 goto _LL1C;case 19U: _LL1B: _LL1C:
 return 1;case 12U: _LL1D: _tmpA42=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA34)->f2;_LL1E:
# 5820
 for(0;_tmpA42 != 0;_tmpA42=_tmpA42->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpA42->hd)->type))return 0;}
return 1;case 14U: _LL1F: _LL20:
 return 1;case 17U: _LL21: _LL22:
 goto _LL24;case 16U: _LL23: _LL24:
 goto _LL26;case 15U: _LL25: _LL26:
 goto _LL28;case 28U: _LL27: _LL28:
 return 0;case 26U: _LL29: _LL2A:
# 5829
 goto _LL2C;case 27U: _LL2B: _LL2C:
 goto _LL2E;case 18U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 21U: _LL31: _LL32:
 goto _LL34;case 22U: _LL33: _LL34:
 goto _LL36;case 23U: _LL35: _LL36:
 goto _LL38;case 24U: _LL37: _LL38:
 goto _LL3A;default: _LL39: _LL3A:
({struct Cyc_String_pa_PrintArg_struct _tmpA3C=({struct Cyc_String_pa_PrintArg_struct _tmpAD0;_tmpAD0.tag=0U,({struct _dyneither_ptr _tmpE77=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAD0.f1=_tmpE77;});_tmpAD0;});void*_tmpA3A[1U];_tmpA3A[0]=& _tmpA3C;({struct _dyneither_ptr _tmpE78=({const char*_tmpA3B="bad type `%s' in zeroable type";_tag_dyneither(_tmpA3B,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE78,_tag_dyneither(_tmpA3A,sizeof(void*),1U));});});}_LL0:;}
