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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 918
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 933
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 944
void*Cyc_Absyn_compress_kb(void*);
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 954
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 956
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 960
extern void*Cyc_Absyn_empty_effect;
# 966
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 973
extern void*Cyc_Absyn_bounds_one;
# 978
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1050
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
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1159
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1161
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1163
int Cyc_Absyn_fntype_att(void*);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1167
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
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
if(({struct _dyneither_ptr _tmpACB=({const char*_tmp0="(qualifiers don't match)";_tag_dyneither(_tmp0,sizeof(char),25U);});Cyc_strcmp(_tmpACB,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmpA3D;_tmpA3D.tag=0U,_tmpA3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmpA3D;});void*_tmp1[1U];_tmp1[0]=& _tmp3;({struct Cyc___cycFILE*_tmpACD=Cyc_stderr;struct _dyneither_ptr _tmpACC=({const char*_tmp2="  %s\n";_tag_dyneither(_tmp2,sizeof(char),6U);});Cyc_fprintf(_tmpACD,_tmpACC,_tag_dyneither(_tmp1,sizeof(void*),1U));});});
return;}
# 68
if(({struct _dyneither_ptr _tmpACE=({const char*_tmp4="(function effects do not match)";_tag_dyneither(_tmp4,sizeof(char),32U);});Cyc_strcmp(_tmpACE,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp5=Cyc_Absynpp_tc_params_r;
_tmp5.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp5);}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmpA3E;_tmpA3E.tag=0U,_tmpA3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmpA3E;});void*_tmp6[1U];_tmp6[0]=& _tmp8;({struct Cyc___cycFILE*_tmpAD0=Cyc_stderr;struct _dyneither_ptr _tmpACF=({const char*_tmp7="  %s";_tag_dyneither(_tmp7,sizeof(char),5U);});Cyc_fprintf(_tmpAD0,_tmpACF,_tag_dyneither(_tmp6,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp9=0U;({struct Cyc___cycFILE*_tmpAD2=Cyc_stderr;struct _dyneither_ptr _tmpAD1=({const char*_tmpA="\n\t";_tag_dyneither(_tmpA,sizeof(char),3U);});Cyc_fprintf(_tmpAD2,_tmpAD1,_tag_dyneither(_tmp9,sizeof(void*),0U));});});
pos=8;}else{
# 82
({void*_tmpB=0U;({struct Cyc___cycFILE*_tmpAD4=Cyc_stderr;struct _dyneither_ptr _tmpAD3=({const char*_tmpC=" ";_tag_dyneither(_tmpC,sizeof(char),2U);});Cyc_fprintf(_tmpAD4,_tmpAD3,_tag_dyneither(_tmpB,sizeof(void*),0U));});});
++ pos;}
# 85
({void*_tmpD=0U;({struct Cyc___cycFILE*_tmpAD6=Cyc_stderr;struct _dyneither_ptr _tmpAD5=({const char*_tmpE="and ";_tag_dyneither(_tmpE,sizeof(char),5U);});Cyc_fprintf(_tmpAD6,_tmpAD5,_tag_dyneither(_tmpD,sizeof(void*),0U));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmpF=0U;({struct Cyc___cycFILE*_tmpAD8=Cyc_stderr;struct _dyneither_ptr _tmpAD7=({const char*_tmp10="\n\t";_tag_dyneither(_tmp10,sizeof(char),3U);});Cyc_fprintf(_tmpAD8,_tmpAD7,_tag_dyneither(_tmpF,sizeof(void*),0U));});});
pos=8;}
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpA3F;_tmpA3F.tag=0U,_tmpA3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmpA3F;});void*_tmp11[1U];_tmp11[0]=& _tmp13;({struct Cyc___cycFILE*_tmpADA=Cyc_stderr;struct _dyneither_ptr _tmpAD9=({const char*_tmp12="%s ";_tag_dyneither(_tmp12,sizeof(char),4U);});Cyc_fprintf(_tmpADA,_tmpAD9,_tag_dyneither(_tmp11,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp14=0U;({struct Cyc___cycFILE*_tmpADC=Cyc_stderr;struct _dyneither_ptr _tmpADB=({const char*_tmp15="\n\t";_tag_dyneither(_tmp15,sizeof(char),3U);});Cyc_fprintf(_tmpADC,_tmpADB,_tag_dyneither(_tmp14,sizeof(void*),0U));});});
pos=8;}
# 97
({void*_tmp16=0U;({struct Cyc___cycFILE*_tmpADE=Cyc_stderr;struct _dyneither_ptr _tmpADD=({const char*_tmp17="are not compatible. ";_tag_dyneither(_tmp17,sizeof(char),21U);});Cyc_fprintf(_tmpADE,_tmpADD,_tag_dyneither(_tmp16,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpADF=(char*)Cyc_Tcutil_failure_reason.curr;_tmpADF != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({int _tmpAE0=pos;_tmpAE0 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80)
({void*_tmp18=0U;({struct Cyc___cycFILE*_tmpAE2=Cyc_stderr;struct _dyneither_ptr _tmpAE1=({const char*_tmp19="\n\t";_tag_dyneither(_tmp19,sizeof(char),3U);});Cyc_fprintf(_tmpAE2,_tmpAE1,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmpA40;_tmpA40.tag=0U,_tmpA40.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmpA40;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct Cyc___cycFILE*_tmpAE4=Cyc_stderr;struct _dyneither_ptr _tmpAE3=({const char*_tmp1B="%s";_tag_dyneither(_tmp1B,sizeof(char),3U);});Cyc_fprintf(_tmpAE4,_tmpAE3,_tag_dyneither(_tmp1A,sizeof(void*),1U));});});}
# 105
({void*_tmp1D=0U;({struct Cyc___cycFILE*_tmpAE6=Cyc_stderr;struct _dyneither_ptr _tmpAE5=({const char*_tmp1E="\n";_tag_dyneither(_tmp1E,sizeof(char),2U);});Cyc_fprintf(_tmpAE6,_tmpAE5,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 111
Cyc_Position_post_error(({unsigned int _tmpAE7=loc;Cyc_Position_mk_err(_tmpAE7,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 113
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 116
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
if(!Cyc_Position_error_p()){
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmpA41;_tmpA41.tag=0U,_tmpA41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpA41;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({struct Cyc___cycFILE*_tmpAE9=Cyc_stderr;struct _dyneither_ptr _tmpAE8=({const char*_tmp20="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp20,sizeof(char),36U);});Cyc_fprintf(_tmpAE9,_tmpAE8,_tag_dyneither(_tmp1F,sizeof(void*),1U));});});
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
return({struct Cyc_Absyn_Tvar*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->name=tv->name,_tmp35->identity=- 1,({void*_tmpAEA=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp35->kind=_tmpAEA;});_tmp35;});}
# 191
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 193
struct _tuple10*_tmp36=arg;struct _dyneither_ptr*_tmp3A;struct Cyc_Absyn_Tqual _tmp39;void*_tmp38;_LL1: _tmp3A=_tmp36->f1;_tmp39=_tmp36->f2;_tmp38=_tmp36->f3;_LL2:;
return({struct _tuple10*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=_tmp3A,_tmp37->f2=_tmp39,({void*_tmpAEB=Cyc_Tcutil_copy_type(_tmp38);_tmp37->f3=_tmpAEB;});_tmp37;});}
# 196
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3B=arg;struct Cyc_Absyn_Tqual _tmp3E;void*_tmp3D;_LL1: _tmp3E=_tmp3B->f1;_tmp3D=_tmp3B->f2;_LL2:;
return({struct _tuple12*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=_tmp3E,({void*_tmpAEC=Cyc_Tcutil_copy_type(_tmp3D);_tmp3C->f2=_tmpAEC;});_tmp3C;});}
# 200
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 202
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=f->name,_tmp3F->tq=f->tq,({void*_tmpAEE=Cyc_Tcutil_copy_type(f->type);_tmp3F->type=_tmpAEE;}),_tmp3F->width=f->width,_tmp3F->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpAED=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp3F->requires_clause=_tmpAED;});_tmp3F;});}
# 206
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp40=x;void*_tmp43;void*_tmp42;_LL1: _tmp43=_tmp40->f1;_tmp42=_tmp40->f2;_LL2:;
return({struct _tuple0*_tmp41=_cycalloc(sizeof(*_tmp41));({void*_tmpAF0=Cyc_Tcutil_copy_type(_tmp43);_tmp41->f1=_tmpAF0;}),({void*_tmpAEF=Cyc_Tcutil_copy_type(_tmp42);_tmp41->f2=_tmpAEF;});_tmp41;});}
# 210
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->name=f->name,_tmp44->tag=f->tag,_tmp44->loc=f->loc;_tmp44;});}
# 213
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->tag=2U,({struct Cyc_Absyn_Tvar*_tmpAF1=Cyc_Tcutil_copy_tvar(t);_tmp45->f1=_tmpAF1;});_tmp45;});}
# 217
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp46=Cyc_Tcutil_compress(t);void*_tmp47=_tmp46;struct Cyc_Absyn_Datatypedecl*_tmpAB;struct Cyc_Absyn_Enumdecl*_tmpAA;struct Cyc_Absyn_Aggrdecl*_tmpA9;void*_tmpA8;struct Cyc_List_List*_tmpA7;void*_tmpA6;struct _tuple2*_tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_Absyn_Typedefdecl*_tmpA3;void*_tmpA2;void*_tmpA1;void*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;void*_tmp9D;struct Cyc_List_List*_tmp9C;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9A;enum Cyc_Absyn_AggrKind _tmp99;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Aggrdecl**_tmp97;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp95;struct _tuple2*_tmp94;struct Cyc_Core_Opt*_tmp93;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp90;void*_tmp8F;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8D;struct Cyc_List_List*_tmp8C;int _tmp8B;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_List_List*_tmp84;void*_tmp83;struct Cyc_Absyn_Tqual _tmp82;struct Cyc_Absyn_Exp*_tmp81;union Cyc_Absyn_Constraint*_tmp80;unsigned int _tmp7F;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7D;void*_tmp7C;union Cyc_Absyn_Constraint*_tmp7B;union Cyc_Absyn_Constraint*_tmp7A;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp78;union Cyc_Absyn_DatatypeFieldInfoU _tmp77;struct Cyc_List_List*_tmp76;union Cyc_Absyn_DatatypeInfoU _tmp75;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Tvar*_tmp73;switch(*((int*)_tmp47)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return t;case 2U: _LL5: _tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp47)->f1;_LL6:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=2U,({struct Cyc_Absyn_Tvar*_tmpAF2=Cyc_Tcutil_copy_tvar(_tmp73);_tmp48->f1=_tmpAF2;});_tmp48;});case 3U: _LL7: _tmp75=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47)->f1).datatype_info;_tmp74=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL8:
# 223
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=3U,(_tmp49->f1).datatype_info=_tmp75,({struct Cyc_List_List*_tmpAF3=Cyc_Tcutil_copy_types(_tmp74);(_tmp49->f1).targs=_tmpAF3;});_tmp49;});case 4U: _LL9: _tmp77=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47)->f1).field_info;_tmp76=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47)->f1).targs;_LLA:
# 225
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=4U,(_tmp4A->f1).field_info=_tmp77,({struct Cyc_List_List*_tmpAF4=Cyc_Tcutil_copy_types(_tmp76);(_tmp4A->f1).targs=_tmpAF4;});_tmp4A;});case 5U: _LLB: _tmp7E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).elt_typ;_tmp7D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).elt_tq;_tmp7C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).rgn;_tmp7B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).nullable;_tmp7A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).bounds;_tmp79=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).zero_term;_tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).ptrloc;_LLC: {
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
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=8U,({void*_tmpAF6=Cyc_Tcutil_copy_type(_tmp83);(_tmp52->f1).elt_type=_tmpAF6;}),(_tmp52->f1).tq=_tmp82,(_tmp52->f1).num_elts=eopt2,({
union Cyc_Absyn_Constraint*_tmpAF5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp80);(_tmp52->f1).zero_term=_tmpAF5;}),(_tmp52->f1).zt_loc=_tmp7F;_tmp52;});}case 9U: _LL13: _tmp90=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).tvars;_tmp8F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).effect;_tmp8E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ret_tqual;_tmp8D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ret_typ;_tmp8C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).args;_tmp8B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).c_varargs;_tmp8A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).cyc_varargs;_tmp89=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).rgn_po;_tmp88=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).attributes;_tmp87=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).requires_clause;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).requires_relns;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ensures_clause;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ensures_relns;_LL14: {
# 242
struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp90);
void*effopt2=_tmp8F == 0?0: Cyc_Tcutil_copy_type(_tmp8F);
void*_tmp54=Cyc_Tcutil_copy_type(_tmp8D);
struct Cyc_List_List*_tmp55=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp8C);
int _tmp56=_tmp8B;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
({struct Cyc_Absyn_VarargInfo*_tmpAF8=({struct Cyc_Absyn_VarargInfo*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->name=cv->name,_tmp57->tq=cv->tq,({void*_tmpAF7=Cyc_Tcutil_copy_type(cv->type);_tmp57->type=_tmpAF7;}),_tmp57->inject=cv->inject;_tmp57;});cyc_varargs2=_tmpAF8;});}{
# 253
struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp89);
struct Cyc_List_List*_tmp59=_tmp88;
struct Cyc_Absyn_Exp*_tmp5A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp87);
struct Cyc_List_List*_tmp5B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp86);
struct Cyc_Absyn_Exp*_tmp5C=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp85);
struct Cyc_List_List*_tmp5D=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp84);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=9U,(_tmp5E->f1).tvars=_tmp53,(_tmp5E->f1).effect=effopt2,(_tmp5E->f1).ret_tqual=_tmp8E,(_tmp5E->f1).ret_typ=_tmp54,(_tmp5E->f1).args=_tmp55,(_tmp5E->f1).c_varargs=_tmp56,(_tmp5E->f1).cyc_varargs=cyc_varargs2,(_tmp5E->f1).rgn_po=_tmp58,(_tmp5E->f1).attributes=_tmp59,(_tmp5E->f1).requires_clause=_tmp5A,(_tmp5E->f1).requires_relns=_tmp5B,(_tmp5E->f1).ensures_clause=_tmp5C,(_tmp5E->f1).ensures_relns=_tmp5D;_tmp5E;});};}case 10U: _LL15: _tmp91=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp47)->f1;_LL16:
# 262
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=10U,({struct Cyc_List_List*_tmpAF9=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp91);_tmp5F->f1=_tmpAF9;});_tmp5F;});case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).tag == 1){_LL17: _tmp95=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f1;_tmp94=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f2;_tmp93=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f3;_tmp92=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL18:
# 264
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpAFB=Cyc_Absyn_UnknownAggr(_tmp95,_tmp94,_tmp93);(_tmp60->f1).aggr_info=_tmpAFB;}),({struct Cyc_List_List*_tmpAFA=Cyc_Tcutil_copy_types(_tmp92);(_tmp60->f1).targs=_tmpAFA;});_tmp60;});}else{_LL19: _tmp97=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).KnownAggr).val;_tmp96=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL1A:
# 266
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpAFD=Cyc_Absyn_KnownAggr(_tmp97);(_tmp61->f1).aggr_info=_tmpAFD;}),({struct Cyc_List_List*_tmpAFC=Cyc_Tcutil_copy_types(_tmp96);(_tmp61->f1).targs=_tmpAFC;});_tmp61;});}case 12U: _LL1B: _tmp99=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47)->f1;_tmp98=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47)->f2;_LL1C:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=12U,_tmp62->f1=_tmp99,({struct Cyc_List_List*_tmpAFE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp98);_tmp62->f2=_tmpAFE;});_tmp62;});case 13U: _LL1D: _tmp9B=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47)->f1;_tmp9A=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47)->f2;_LL1E:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=13U,_tmp63->f1=_tmp9B,_tmp63->f2=_tmp9A;_tmp63;});case 14U: _LL1F: _tmp9C=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp47)->f1;_LL20:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=14U,({struct Cyc_List_List*_tmpAFF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9C);_tmp64->f1=_tmpAFF;});_tmp64;});case 19U: _LL21: _tmp9D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp47)->f1;_LL22:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=19U,({void*_tmpB00=Cyc_Tcutil_copy_type(_tmp9D);_tmp65->f1=_tmpB00;});_tmp65;});case 18U: _LL23: _tmp9E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp47)->f1;_LL24:
# 273
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=18U,_tmp66->f1=_tmp9E;_tmp66;});case 27U: _LL25: _tmp9F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp47)->f1;_LL26:
# 276
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=27U,_tmp67->f1=_tmp9F;_tmp67;});case 15U: _LL27: _tmpA0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp47)->f1;_LL28:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=15U,({void*_tmpB01=Cyc_Tcutil_copy_type(_tmpA0);_tmp68->f1=_tmpB01;});_tmp68;});case 16U: _LL29: _tmpA2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47)->f1;_tmpA1=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47)->f2;_LL2A:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=16U,({void*_tmpB03=Cyc_Tcutil_copy_type(_tmpA2);_tmp69->f1=_tmpB03;}),({void*_tmpB02=Cyc_Tcutil_copy_type(_tmpA1);_tmp69->f2=_tmpB02;});_tmp69;});case 17U: _LL2B: _tmpA5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f1;_tmpA4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f2;_tmpA3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f3;_LL2C:
# 280
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=17U,_tmp6A->f1=_tmpA5,({struct Cyc_List_List*_tmpB04=Cyc_Tcutil_copy_types(_tmpA4);_tmp6A->f2=_tmpB04;}),_tmp6A->f3=_tmpA3,_tmp6A->f4=0;_tmp6A;});case 21U: _LL2D: _LL2E:
 goto _LL30;case 22U: _LL2F: _LL30:
 goto _LL32;case 28U: _LL31: _LL32:
 goto _LL34;case 20U: _LL33: _LL34:
 return t;case 23U: _LL35: _tmpA6=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp47)->f1;_LL36:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=23U,({void*_tmpB05=Cyc_Tcutil_copy_type(_tmpA6);_tmp6B->f1=_tmpB05;});_tmp6B;});case 24U: _LL37: _tmpA7=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp47)->f1;_LL38:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=24U,({struct Cyc_List_List*_tmpB06=Cyc_Tcutil_copy_types(_tmpA7);_tmp6C->f1=_tmpB06;});_tmp6C;});case 25U: _LL39: _tmpA8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp47)->f1;_LL3A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=25U,({void*_tmpB07=Cyc_Tcutil_copy_type(_tmpA8);_tmp6D->f1=_tmpB07;});_tmp6D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)){case 0U: _LL3B: _tmpA9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL3C: {
# 290
struct Cyc_List_List*_tmp6E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpA9->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpB08=Cyc_Absyn_UnknownAggr(_tmpA9->kind,_tmpA9->name,0);(_tmp6F->f1).aggr_info=_tmpB08;}),(_tmp6F->f1).targs=_tmp6E;_tmp6F;});}case 1U: _LL3D: _tmpAA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL3E:
# 293
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=13U,_tmp70->f1=_tmpAA->name,_tmp70->f2=0;_tmp70;});default: _LL3F: _tmpAB=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL40: {
# 295
struct Cyc_List_List*_tmp71=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAB->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=3U,({union Cyc_Absyn_DatatypeInfoU _tmpB09=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmpA42;_tmpA42.name=_tmpAB->name,_tmpA42.is_extensible=0;_tmpA42;}));(_tmp72->f1).datatype_info=_tmpB09;}),(_tmp72->f1).targs=_tmp71;_tmp72;});}}}_LL0:;}
# 310 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpAC=d;struct _dyneither_ptr*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC)->tag == 0U){_LL1: _tmpAE=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=0U,({struct Cyc_Absyn_Exp*_tmpB0A=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpAE);_tmpAD->f1=_tmpB0A;});_tmpAD;});}else{_LL3: _tmpAF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpAC)->f1;_LL4:
 return d;}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 316
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 318
return({struct _tuple19*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct Cyc_List_List*_tmpB0C=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB0->f1=_tmpB0C;}),({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB0->f2=_tmpB0B;});_tmpB0;});}
# 321
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB1=preserve_types;
{void*_tmpB2=e->r;void*_tmpB3=_tmpB2;int _tmp12F;struct _dyneither_ptr _tmp12E;void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct _dyneither_ptr*_tmp12B;struct Cyc_Core_Opt*_tmp12A;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;int _tmp126;struct Cyc_Absyn_Exp*_tmp125;void**_tmp124;struct Cyc_Absyn_Exp*_tmp123;int _tmp122;int _tmp121;void*_tmp120;struct Cyc_Absyn_Enumfield*_tmp11F;struct Cyc_Absyn_Enumdecl*_tmp11E;struct Cyc_Absyn_Enumfield*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Datatypedecl*_tmp11B;struct Cyc_Absyn_Datatypefield*_tmp11A;void*_tmp119;struct Cyc_List_List*_tmp118;struct _tuple2*_tmp117;struct Cyc_List_List*_tmp116;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Aggrdecl*_tmp114;struct Cyc_Absyn_Exp*_tmp113;void*_tmp112;int _tmp111;struct Cyc_Absyn_Vardecl*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;int _tmp10D;struct Cyc_List_List*_tmp10C;struct _dyneither_ptr*_tmp10B;struct Cyc_Absyn_Tqual _tmp10A;void*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct _dyneither_ptr*_tmp103;int _tmp102;int _tmp101;struct Cyc_Absyn_Exp*_tmp100;struct _dyneither_ptr*_tmpFF;int _tmpFE;int _tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;void*_tmpF4;struct Cyc_Absyn_Exp*_tmpF3;int _tmpF2;enum Cyc_Absyn_Coercion _tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpED;int _tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargCallInfo*_tmpE9;int _tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;enum Cyc_Absyn_Incrementor _tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Core_Opt*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;enum Cyc_Absyn_Primop _tmpD9;struct Cyc_List_List*_tmpD8;struct _dyneither_ptr _tmpD7;void*_tmpD6;union Cyc_Absyn_Cnst _tmpD5;switch(*((int*)_tmpB3)){case 0U: _LL1: _tmpD5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL2:
({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Absyn_const_exp(_tmpD5,e->loc);new_e=_tmpB0D;});goto _LL0;case 1U: _LL3: _tmpD6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpB0E=Cyc_Absyn_varb_exp(_tmpD6,e->loc);new_e=_tmpB0E;});goto _LL0;case 2U: _LL5: _tmpD7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Absyn_pragma_exp(_tmpD7,e->loc);new_e=_tmpB0F;});goto _LL0;case 3U: _LL7: _tmpD9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpD8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL8:
({struct Cyc_Absyn_Exp*_tmpB12=({enum Cyc_Absyn_Primop _tmpB11=_tmpD9;struct Cyc_List_List*_tmpB10=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpD8);Cyc_Absyn_primop_exp(_tmpB11,_tmpB10,e->loc);});new_e=_tmpB12;});goto _LL0;case 4U: _LL9: _tmpDC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpDB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpDA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LLA:
# 330
({struct Cyc_Absyn_Exp*_tmpB16=({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDC);struct Cyc_Core_Opt*_tmpB14=(unsigned int)_tmpDB?({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->v=(void*)_tmpDB->v;_tmpB4;}): 0;struct Cyc_Absyn_Exp*_tmpB13=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDA);Cyc_Absyn_assignop_exp(_tmpB15,_tmpB14,_tmpB13,e->loc);});new_e=_tmpB16;});
goto _LL0;case 5U: _LLB: _tmpDE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpDD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LLC:
({struct Cyc_Absyn_Exp*_tmpB19=({struct Cyc_Absyn_Exp*_tmpB18=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDE);enum Cyc_Absyn_Incrementor _tmpB17=_tmpDD;Cyc_Absyn_increment_exp(_tmpB18,_tmpB17,e->loc);});new_e=_tmpB19;});goto _LL0;case 6U: _LLD: _tmpE1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpDF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LLE:
# 334
({struct Cyc_Absyn_Exp*_tmpB1D=({struct Cyc_Absyn_Exp*_tmpB1C=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE1);struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE0);struct Cyc_Absyn_Exp*_tmpB1A=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDF);Cyc_Absyn_conditional_exp(_tmpB1C,_tmpB1B,_tmpB1A,e->loc);});new_e=_tmpB1D;});goto _LL0;case 7U: _LLF: _tmpE3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL10:
({struct Cyc_Absyn_Exp*_tmpB20=({struct Cyc_Absyn_Exp*_tmpB1F=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE3);struct Cyc_Absyn_Exp*_tmpB1E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE2);Cyc_Absyn_and_exp(_tmpB1F,_tmpB1E,e->loc);});new_e=_tmpB20;});goto _LL0;case 8U: _LL11: _tmpE5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL12:
({struct Cyc_Absyn_Exp*_tmpB23=({struct Cyc_Absyn_Exp*_tmpB22=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE5);struct Cyc_Absyn_Exp*_tmpB21=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE4);Cyc_Absyn_or_exp(_tmpB22,_tmpB21,e->loc);});new_e=_tmpB23;});goto _LL0;case 9U: _LL13: _tmpE7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL14:
({struct Cyc_Absyn_Exp*_tmpB26=({struct Cyc_Absyn_Exp*_tmpB25=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE7);struct Cyc_Absyn_Exp*_tmpB24=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE6);Cyc_Absyn_seq_exp(_tmpB25,_tmpB24,e->loc);});new_e=_tmpB26;});goto _LL0;case 10U: _LL15: _tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpE9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpE8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL16:
# 339
{struct Cyc_Absyn_VarargCallInfo*_tmpB5=_tmpE9;int _tmpC1;struct Cyc_List_List*_tmpC0;struct Cyc_Absyn_VarargInfo*_tmpBF;if(_tmpB5 != 0){_LL54: _tmpC1=_tmpB5->num_varargs;_tmpC0=_tmpB5->injectors;_tmpBF=_tmpB5->vai;_LL55: {
# 341
struct Cyc_Absyn_VarargInfo*_tmpB6=_tmpBF;struct _dyneither_ptr*_tmpBD;struct Cyc_Absyn_Tqual _tmpBC;void*_tmpBB;int _tmpBA;_LL59: _tmpBD=_tmpB6->name;_tmpBC=_tmpB6->tq;_tmpBB=_tmpB6->type;_tmpBA=_tmpB6->inject;_LL5A:;
({struct Cyc_Absyn_Exp*_tmpB2D=({void*_tmpB2C=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->tag=10U,({
struct Cyc_Absyn_Exp*_tmpB2B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEB);_tmpB9->f1=_tmpB2B;}),({struct Cyc_List_List*_tmpB2A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpEA);_tmpB9->f2=_tmpB2A;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpB29=({struct Cyc_Absyn_VarargCallInfo*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->num_varargs=_tmpC1,_tmpB8->injectors=_tmpC0,({
struct Cyc_Absyn_VarargInfo*_tmpB28=({struct Cyc_Absyn_VarargInfo*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->name=_tmpBD,_tmpB7->tq=_tmpBC,({void*_tmpB27=Cyc_Tcutil_copy_type(_tmpBB);_tmpB7->type=_tmpB27;}),_tmpB7->inject=_tmpBA;_tmpB7;});_tmpB8->vai=_tmpB28;});_tmpB8;});
# 344
_tmpB9->f3=_tmpB29;}),_tmpB9->f4=_tmpE8;_tmpB9;});
# 342
Cyc_Absyn_new_exp(_tmpB2C,e->loc);});new_e=_tmpB2D;});
# 347
goto _LL53;}}else{_LL56: _LL57:
# 349
({struct Cyc_Absyn_Exp*_tmpB31=({void*_tmpB30=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=10U,({struct Cyc_Absyn_Exp*_tmpB2F=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEB);_tmpBE->f1=_tmpB2F;}),({struct Cyc_List_List*_tmpB2E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpEA);_tmpBE->f2=_tmpB2E;}),_tmpBE->f3=_tmpE9,_tmpBE->f4=_tmpE8;_tmpBE;});Cyc_Absyn_new_exp(_tmpB30,e->loc);});new_e=_tmpB31;});
goto _LL53;}_LL53:;}
# 352
goto _LL0;case 11U: _LL17: _tmpED=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL18:
# 354
({struct Cyc_Absyn_Exp*_tmpB34=_tmpEC?({struct Cyc_Absyn_Exp*_tmpB33=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpED);Cyc_Absyn_rethrow_exp(_tmpB33,e->loc);}):({struct Cyc_Absyn_Exp*_tmpB32=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpED);Cyc_Absyn_throw_exp(_tmpB32,e->loc);});new_e=_tmpB34;});
goto _LL0;case 12U: _LL19: _tmpEE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL1A:
({struct Cyc_Absyn_Exp*_tmpB36=({struct Cyc_Absyn_Exp*_tmpB35=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEE);Cyc_Absyn_noinstantiate_exp(_tmpB35,e->loc);});new_e=_tmpB36;});
goto _LL0;case 13U: _LL1B: _tmpF0=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL1C:
# 359
({struct Cyc_Absyn_Exp*_tmpB39=({struct Cyc_Absyn_Exp*_tmpB38=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF0);struct Cyc_List_List*_tmpB37=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmpEF);Cyc_Absyn_instantiate_exp(_tmpB38,_tmpB37,e->loc);});new_e=_tmpB39;});
goto _LL0;case 14U: _LL1D: _tmpF4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpF3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpF2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpF1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL1E:
# 362
({struct Cyc_Absyn_Exp*_tmpB3E=({void*_tmpB3D=Cyc_Tcutil_copy_type(_tmpF4);struct Cyc_Absyn_Exp*_tmpB3C=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF3);int _tmpB3B=_tmpF2;enum Cyc_Absyn_Coercion _tmpB3A=_tmpF1;Cyc_Absyn_cast_exp(_tmpB3D,_tmpB3C,_tmpB3B,_tmpB3A,e->loc);});new_e=_tmpB3E;});goto _LL0;case 15U: _LL1F: _tmpF5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL20:
({struct Cyc_Absyn_Exp*_tmpB40=({struct Cyc_Absyn_Exp*_tmpB3F=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF5);Cyc_Absyn_address_exp(_tmpB3F,e->loc);});new_e=_tmpB40;});goto _LL0;case 16U: _LL21: _tmpF7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpF6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL22: {
# 365
struct Cyc_Absyn_Exp*eo1=_tmpF7;if(_tmpF7 != 0)({struct Cyc_Absyn_Exp*_tmpB41=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF7);eo1=_tmpB41;});
({struct Cyc_Absyn_Exp*_tmpB44=({struct Cyc_Absyn_Exp*_tmpB43=eo1;struct Cyc_Absyn_Exp*_tmpB42=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF6);Cyc_Absyn_New_exp(_tmpB43,_tmpB42,e->loc);});new_e=_tmpB44;});
goto _LL0;}case 17U: _LL23: _tmpF8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL24:
({struct Cyc_Absyn_Exp*_tmpB46=({void*_tmpB45=Cyc_Tcutil_copy_type(_tmpF8);Cyc_Absyn_sizeoftyp_exp(_tmpB45,e->loc);});new_e=_tmpB46;});
goto _LL0;case 18U: _LL25: _tmpF9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL26:
({struct Cyc_Absyn_Exp*_tmpB48=({struct Cyc_Absyn_Exp*_tmpB47=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF9);Cyc_Absyn_sizeofexp_exp(_tmpB47,e->loc);});new_e=_tmpB48;});goto _LL0;case 19U: _LL27: _tmpFB=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpFA=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL28:
# 372
({struct Cyc_Absyn_Exp*_tmpB4B=({void*_tmpB4A=Cyc_Tcutil_copy_type(_tmpFB);struct Cyc_List_List*_tmpB49=_tmpFA;Cyc_Absyn_offsetof_exp(_tmpB4A,_tmpB49,e->loc);});new_e=_tmpB4B;});goto _LL0;case 20U: _LL29: _tmpFC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL2A:
({struct Cyc_Absyn_Exp*_tmpB4D=({struct Cyc_Absyn_Exp*_tmpB4C=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpFC);Cyc_Absyn_deref_exp(_tmpB4C,e->loc);});new_e=_tmpB4D;});goto _LL0;case 21U: _LL2B: _tmp100=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpFF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpFE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpFD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL2C:
# 375
({struct Cyc_Absyn_Exp*_tmpB50=({void*_tmpB4F=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=21U,({struct Cyc_Absyn_Exp*_tmpB4E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp100);_tmpC2->f1=_tmpB4E;}),_tmpC2->f2=_tmpFF,_tmpC2->f3=_tmpFE,_tmpC2->f4=_tmpFD;_tmpC2;});Cyc_Absyn_new_exp(_tmpB4F,e->loc);});new_e=_tmpB50;});goto _LL0;case 22U: _LL2D: _tmp104=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp103=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp102=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp101=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL2E:
# 377
({struct Cyc_Absyn_Exp*_tmpB53=({void*_tmpB52=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=22U,({struct Cyc_Absyn_Exp*_tmpB51=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp104);_tmpC3->f1=_tmpB51;}),_tmpC3->f2=_tmp103,_tmpC3->f3=_tmp102,_tmpC3->f4=_tmp101;_tmpC3;});Cyc_Absyn_new_exp(_tmpB52,e->loc);});new_e=_tmpB53;});goto _LL0;case 23U: _LL2F: _tmp106=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp105=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL30:
({struct Cyc_Absyn_Exp*_tmpB56=({struct Cyc_Absyn_Exp*_tmpB55=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp106);struct Cyc_Absyn_Exp*_tmpB54=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp105);Cyc_Absyn_subscript_exp(_tmpB55,_tmpB54,e->loc);});new_e=_tmpB56;});
goto _LL0;case 24U: _LL31: _tmp107=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL32:
({struct Cyc_Absyn_Exp*_tmpB58=({struct Cyc_List_List*_tmpB57=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmp107);Cyc_Absyn_tuple_exp(_tmpB57,e->loc);});new_e=_tmpB58;});goto _LL0;case 25U: _LL33: _tmp10B=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f1;_tmp10A=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f2;_tmp109=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f3;_tmp108=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL34: {
# 382
struct _dyneither_ptr*vopt1=_tmp10B;
if(_tmp10B != 0)vopt1=_tmp10B;
({struct Cyc_Absyn_Exp*_tmpB5D=({void*_tmpB5C=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->tag=25U,({struct _tuple10*_tmpB5B=({struct _tuple10*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->f1=vopt1,_tmpC4->f2=_tmp10A,({void*_tmpB5A=Cyc_Tcutil_copy_type(_tmp109);_tmpC4->f3=_tmpB5A;});_tmpC4;});_tmpC5->f1=_tmpB5B;}),({
struct Cyc_List_List*_tmpB59=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp108);_tmpC5->f2=_tmpB59;});_tmpC5;});
# 384
Cyc_Absyn_new_exp(_tmpB5C,e->loc);});new_e=_tmpB5D;});
# 386
goto _LL0;}case 26U: _LL35: _tmp10C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL36:
({struct Cyc_Absyn_Exp*_tmpB60=({void*_tmpB5F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=26U,({struct Cyc_List_List*_tmpB5E=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp10C);_tmpC6->f1=_tmpB5E;});_tmpC6;});Cyc_Absyn_new_exp(_tmpB5F,e->loc);});new_e=_tmpB60;});
goto _LL0;case 27U: _LL37: _tmp110=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp10F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp10E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp10D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL38:
# 390
({struct Cyc_Absyn_Exp*_tmpB64=({void*_tmpB63=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->tag=27U,_tmpC7->f1=_tmp110,({struct Cyc_Absyn_Exp*_tmpB62=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp10F);_tmpC7->f2=_tmpB62;}),({struct Cyc_Absyn_Exp*_tmpB61=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp10E);_tmpC7->f3=_tmpB61;}),_tmpC7->f4=_tmp10D;_tmpC7;});Cyc_Absyn_new_exp(_tmpB63,e->loc);});new_e=_tmpB64;});
goto _LL0;case 28U: _LL39: _tmp113=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp112=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp111=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LL3A:
# 393
({struct Cyc_Absyn_Exp*_tmpB68=({void*_tmpB67=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=28U,({struct Cyc_Absyn_Exp*_tmpB66=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp113);_tmpC8->f1=_tmpB66;}),({void*_tmpB65=Cyc_Tcutil_copy_type(_tmp112);_tmpC8->f2=_tmpB65;}),_tmpC8->f3=_tmp111;_tmpC8;});Cyc_Absyn_new_exp(_tmpB67,_tmp113->loc);});new_e=_tmpB68;});
# 395
goto _LL0;case 29U: _LL3B: _tmp117=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp116=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp115=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp114=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL3C:
# 397
({struct Cyc_Absyn_Exp*_tmpB6C=({void*_tmpB6B=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=29U,_tmpC9->f1=_tmp117,({struct Cyc_List_List*_tmpB6A=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp116);_tmpC9->f2=_tmpB6A;}),({struct Cyc_List_List*_tmpB69=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp115);_tmpC9->f3=_tmpB69;}),_tmpC9->f4=_tmp114;_tmpC9;});Cyc_Absyn_new_exp(_tmpB6B,e->loc);});new_e=_tmpB6C;});
# 399
goto _LL0;case 30U: _LL3D: _tmp119=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp118=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL3E:
# 401
({struct Cyc_Absyn_Exp*_tmpB70=({void*_tmpB6F=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->tag=30U,({void*_tmpB6E=Cyc_Tcutil_copy_type(_tmp119);_tmpCA->f1=_tmpB6E;}),({struct Cyc_List_List*_tmpB6D=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp118);_tmpCA->f2=_tmpB6D;});_tmpCA;});Cyc_Absyn_new_exp(_tmpB6F,e->loc);});new_e=_tmpB70;});
goto _LL0;case 31U: _LL3F: _tmp11C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp11A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LL40:
# 404
({struct Cyc_Absyn_Exp*_tmpB73=({void*_tmpB72=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=31U,({struct Cyc_List_List*_tmpB71=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmp11C);_tmpCB->f1=_tmpB71;}),_tmpCB->f2=_tmp11B,_tmpCB->f3=_tmp11A;_tmpCB;});Cyc_Absyn_new_exp(_tmpB72,e->loc);});new_e=_tmpB73;});
goto _LL0;case 32U: _LL41: _tmp11E=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL42:
 new_e=e;goto _LL0;case 33U: _LL43: _tmp120=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11F=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL44:
# 408
({struct Cyc_Absyn_Exp*_tmpB76=({void*_tmpB75=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->tag=33U,({void*_tmpB74=Cyc_Tcutil_copy_type(_tmp120);_tmpCC->f1=_tmpB74;}),_tmpCC->f2=_tmp11F;_tmpCC;});Cyc_Absyn_new_exp(_tmpB75,e->loc);});new_e=_tmpB76;});
goto _LL0;case 34U: _LL45: _tmp126=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).is_calloc;_tmp125=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).rgn;_tmp124=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).elt_type;_tmp123=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).num_elts;_tmp122=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).fat_result;_tmp121=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).inline_call;_LL46: {
# 411
struct Cyc_Absyn_Exp*_tmpCD=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp125;if(_tmp125 != 0)({struct Cyc_Absyn_Exp*_tmpB77=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp125);r1=_tmpB77;});{
void**t1=_tmp124;if(_tmp124 != 0)({void**_tmpB79=({void**_tmpCE=_cycalloc(sizeof(*_tmpCE));({void*_tmpB78=Cyc_Tcutil_copy_type(*_tmp124);*_tmpCE=_tmpB78;});_tmpCE;});t1=_tmpB79;});
({void*_tmpB7A=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=34U,(_tmpCF->f1).is_calloc=_tmp126,(_tmpCF->f1).rgn=r1,(_tmpCF->f1).elt_type=t1,(_tmpCF->f1).num_elts=_tmp123,(_tmpCF->f1).fat_result=_tmp122,(_tmpCF->f1).inline_call=_tmp121;_tmpCF;});_tmpCD->r=_tmpB7A;});
new_e=_tmpCD;
goto _LL0;};}case 35U: _LL47: _tmp128=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp127=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL48:
({struct Cyc_Absyn_Exp*_tmpB7D=({struct Cyc_Absyn_Exp*_tmpB7C=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp128);struct Cyc_Absyn_Exp*_tmpB7B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp127);Cyc_Absyn_swap_exp(_tmpB7C,_tmpB7B,e->loc);});new_e=_tmpB7D;});goto _LL0;case 36U: _LL49: _tmp12A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp129=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL4A: {
# 419
struct Cyc_Core_Opt*nopt1=_tmp12A;
if(_tmp12A != 0)({struct Cyc_Core_Opt*_tmpB7E=({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=(struct _tuple2*)_tmp12A->v;_tmpD0;});nopt1=_tmpB7E;});
({struct Cyc_Absyn_Exp*_tmpB81=({void*_tmpB80=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=36U,_tmpD1->f1=nopt1,({struct Cyc_List_List*_tmpB7F=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp129);_tmpD1->f2=_tmpB7F;});_tmpD1;});Cyc_Absyn_new_exp(_tmpB80,e->loc);});new_e=_tmpB81;});
goto _LL0;}case 37U: _LL4B: _LL4C:
# 424
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpB82=({const char*_tmpD2="deep_copy: statement expressions unsupported";_tag_dyneither(_tmpD2,sizeof(char),45U);});_tmpD3->f1=_tmpB82;});_tmpD3;}));case 38U: _LL4D: _tmp12C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp12B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL4E:
({struct Cyc_Absyn_Exp*_tmpB85=({void*_tmpB84=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->tag=38U,({struct Cyc_Absyn_Exp*_tmpB83=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp12C);_tmpD4->f1=_tmpB83;}),_tmpD4->f2=_tmp12B;_tmpD4;});Cyc_Absyn_new_exp(_tmpB84,e->loc);});new_e=_tmpB85;});
goto _LL0;case 39U: _LL4F: _tmp12D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL50:
({struct Cyc_Absyn_Exp*_tmpB87=({void*_tmpB86=Cyc_Tcutil_copy_type(_tmp12D);Cyc_Absyn_valueof_exp(_tmpB86,e->loc);});new_e=_tmpB87;});
goto _LL0;default: _LL51: _tmp12F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp12E=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL52:
({struct Cyc_Absyn_Exp*_tmpB88=Cyc_Absyn_asm_exp(_tmp12F,_tmp12E,e->loc);new_e=_tmpB88;});goto _LL0;}_LL0:;}
# 432
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 436
return new_e;}
# 439
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 452 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp130=ka1;enum Cyc_Absyn_KindQual _tmp139;enum Cyc_Absyn_AliasQual _tmp138;_LL1: _tmp139=_tmp130->kind;_tmp138=_tmp130->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp131=ka2;enum Cyc_Absyn_KindQual _tmp137;enum Cyc_Absyn_AliasQual _tmp136;_LL4: _tmp137=_tmp131->kind;_tmp136=_tmp131->aliasqual;_LL5:;
# 456
if(_tmp139 != _tmp137){
struct _tuple20 _tmp132=({struct _tuple20 _tmpA43;_tmpA43.f1=_tmp139,_tmpA43.f2=_tmp137;_tmpA43;});struct _tuple20 _tmp133=_tmp132;switch(_tmp133.f1){case Cyc_Absyn_BoxKind: switch(_tmp133.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp133.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 465
if(_tmp138 != _tmp136){
struct _tuple21 _tmp134=({struct _tuple21 _tmpA44;_tmpA44.f1=_tmp138,_tmpA44.f2=_tmp136;_tmpA44;});struct _tuple21 _tmp135=_tmp134;switch(_tmp135.f1){case Cyc_Absyn_Aliasable: if(_tmp135.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp135.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 472
return 1;};}
# 475
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp13A=Cyc_Absyn_compress_kb(tv->kind);void*_tmp13B=_tmp13A;struct Cyc_Absyn_Kind*_tmp13E;struct Cyc_Absyn_Kind*_tmp13D;switch(*((int*)_tmp13B)){case 0U: _LL1: _tmp13D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp13B)->f1;_LL2:
 return _tmp13D;case 2U: _LL3: _tmp13E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp13B)->f2;_LL4:
 return _tmp13E;default: _LL5: _LL6:
# 480
({void*_tmpB89=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=2U,_tmp13C->f1=0,_tmp13C->f2=def;_tmp13C;});tv->kind=_tmpB89;});
return def;}_LL0:;}
# 485
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp13F=({struct _tuple0 _tmpA45;({void*_tmpB8B=Cyc_Absyn_compress_kb(kb1);_tmpA45.f1=_tmpB8B;}),({void*_tmpB8A=Cyc_Absyn_compress_kb(kb2);_tmpA45.f2=_tmpB8A;});_tmpA45;});struct _tuple0 _tmp140=_tmp13F;struct Cyc_Core_Opt**_tmp155;void*_tmp154;void*_tmp153;struct Cyc_Core_Opt**_tmp152;struct Cyc_Core_Opt**_tmp151;struct Cyc_Absyn_Kind*_tmp150;struct Cyc_Core_Opt**_tmp14F;struct Cyc_Absyn_Kind*_tmp14E;struct Cyc_Core_Opt**_tmp14D;struct Cyc_Absyn_Kind*_tmp14C;struct Cyc_Absyn_Kind*_tmp14B;struct Cyc_Absyn_Kind*_tmp14A;struct Cyc_Core_Opt**_tmp149;struct Cyc_Absyn_Kind*_tmp148;struct Cyc_Absyn_Kind*_tmp147;struct Cyc_Absyn_Kind*_tmp146;switch(*((int*)_tmp140.f1)){case 0U: switch(*((int*)_tmp140.f2)){case 0U: _LL1: _tmp147=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp146=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_LL2:
 return _tmp147 == _tmp146;case 2U: _LL5: _tmp14A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp149=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_tmp148=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f2;_LL6:
# 494
 if(Cyc_Tcutil_kind_leq(_tmp14A,_tmp148)){
({struct Cyc_Core_Opt*_tmpB8C=({struct Cyc_Core_Opt*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->v=kb1;_tmp142;});*_tmp149=_tmpB8C;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp140.f2)){case 0U: _LL3: _tmp14D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp14C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f2;_tmp14B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_LL4:
# 489
 if(Cyc_Tcutil_kind_leq(_tmp14B,_tmp14C)){
({struct Cyc_Core_Opt*_tmpB8D=({struct Cyc_Core_Opt*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->v=kb2;_tmp141;});*_tmp14D=_tmpB8D;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp151=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp150=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f2;_tmp14F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_tmp14E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f2;_LL8:
# 499
 if(Cyc_Tcutil_kind_leq(_tmp150,_tmp14E)){
({struct Cyc_Core_Opt*_tmpB8E=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->v=kb1;_tmp143;});*_tmp14F=_tmpB8E;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp14E,_tmp150)){
({struct Cyc_Core_Opt*_tmpB8F=({struct Cyc_Core_Opt*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->v=kb2;_tmp144;});*_tmp151=_tmpB8F;});
return 1;}else{
return 0;}}default: _LLB: _tmp153=_tmp140.f1;_tmp152=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_LLC:
# 508
({struct Cyc_Core_Opt*_tmpB90=({struct Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->v=_tmp153;_tmp145;});*_tmp152=_tmpB90;});
return 1;}default: _LL9: _tmp155=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp154=_tmp140.f2;_LLA:
# 506
 _tmp153=_tmp154;_tmp152=_tmp155;goto _LLC;}_LL0:;}
# 513
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp156=Cyc_Tcutil_compress(t);void*_tmp157=_tmp156;struct Cyc_Absyn_Tvar*_tmp15C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp157)->tag == 2U){_LL1: _tmp15C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp157)->f1;_LL2: {
# 516
void*_tmp158=_tmp15C->kind;
_tmp15C->kind=kb;
return({struct _tuple16 _tmpA46;_tmpA46.f1=_tmp15C,_tmpA46.f2=_tmp158;_tmpA46;});}}else{_LL3: _LL4:
# 520
({struct Cyc_String_pa_PrintArg_struct _tmp15B=({struct Cyc_String_pa_PrintArg_struct _tmpA47;_tmpA47.tag=0U,({struct _dyneither_ptr _tmpB91=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA47.f1=_tmpB91;});_tmpA47;});void*_tmp159[1U];_tmp159[0]=& _tmp15B;({struct _dyneither_ptr _tmpB92=({const char*_tmp15A="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp15A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB92,_tag_dyneither(_tmp159,sizeof(void*),1U));});});}_LL0:;}
# 526
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 528
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 532
struct _RegionHandle _tmp15D=_new_region("temp");struct _RegionHandle*temp=& _tmp15D;_push_region(temp);
{struct Cyc_List_List*_tmp15E=0;
# 535
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp15F=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp160=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp161=Cyc_Tcutil_tvar_kind(_tmp15F,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp162=_tmp161;switch(((struct Cyc_Absyn_Kind*)_tmp162)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 541
({struct Cyc_List_List*_tmpB94=({struct Cyc_List_List*_tmp164=_region_malloc(temp,sizeof(*_tmp164));({struct _tuple16*_tmpB93=({struct _tuple16*_tmp163=_region_malloc(temp,sizeof(*_tmp163));_tmp163->f1=_tmp15F,_tmp163->f2=_tmp160;_tmp163;});_tmp164->hd=_tmpB93;}),_tmp164->tl=_tmp15E;_tmp164;});_tmp15E=_tmpB94;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 545
if(_tmp15E != 0){
({void*_tmpB97=({struct _RegionHandle*_tmpB96=temp;struct Cyc_List_List*_tmpB95=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15E);Cyc_Tcutil_rsubstitute(_tmpB96,_tmpB95,field_typ);});field_typ=_tmpB97;});
({struct Cyc_Absyn_Kind*_tmpB98=Cyc_Tcutil_typ_kind(field_typ);k=_tmpB98;});}}
# 533
;_pop_region(temp);}
# 550
return k;}
# 557
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp165=Cyc_Tcutil_compress(t);void*_tmp166=_tmp165;struct Cyc_Absyn_Typedefdecl*_tmp184;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Kind*_tmp182;struct Cyc_Absyn_PtrInfo _tmp181;enum Cyc_Absyn_AggrKind _tmp180;struct Cyc_List_List*_tmp17F;struct Cyc_Absyn_AggrdeclImpl*_tmp17E;int _tmp17D;struct Cyc_List_List*_tmp17C;struct Cyc_Absyn_Datatypedecl*_tmp17B;struct Cyc_Absyn_Datatypefield*_tmp17A;enum Cyc_Absyn_Size_of _tmp179;struct Cyc_Absyn_Tvar*_tmp178;struct Cyc_Core_Opt*_tmp177;switch(*((int*)_tmp166)){case 1U: _LL1: _tmp177=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp166)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp177))->v;case 2U: _LL3: _tmp178=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp166)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp178,& Cyc_Tcutil_bk);case 0U: _LL5: _LL6:
 return& Cyc_Tcutil_mk;case 6U: _LL7: _tmp179=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp166)->f2;_LL8:
# 563
 return(_tmp179 == Cyc_Absyn_Int_sz  || _tmp179 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7U: _LL9: _LLA:
# 565
 return& Cyc_Tcutil_mk;case 9U: _LLB: _LLC:
 return& Cyc_Tcutil_ak;case 16U: _LLD: _LLE:
 goto _LL10;case 13U: _LLF: _LL10:
 goto _LL12;case 14U: _LL11: _LL12:
 goto _LL14;case 15U: _LL13: _LL14:
 return& Cyc_Tcutil_bk;case 21U: _LL15: _LL16:
 return& Cyc_Tcutil_urk;case 20U: _LL17: _LL18:
 return& Cyc_Tcutil_rk;case 22U: _LL19: _LL1A:
 return& Cyc_Tcutil_trk;case 3U: _LL1B: _LL1C:
# 576
 return& Cyc_Tcutil_ak;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp166)->f1).field_info).KnownDatatypefield).tag == 2){_LL1D: _tmp17B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp166)->f1).field_info).KnownDatatypefield).val).f1;_tmp17A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp166)->f1).field_info).KnownDatatypefield).val).f2;_LL1E:
# 578
 return& Cyc_Tcutil_mk;}else{_LL1F: _LL20:
# 580
({void*_tmp167=0U;({struct _dyneither_ptr _tmpB99=({const char*_tmp168="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp168,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB99,_tag_dyneither(_tmp167,sizeof(void*),0U));});});}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166)->f1).aggr_info).UnknownAggr).tag == 1){_LL21: _LL22:
# 583
 return& Cyc_Tcutil_ak;}else{_LL23: _tmp180=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166)->f1).aggr_info).KnownAggr).val)->kind;_tmp17F=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166)->f1).aggr_info).KnownAggr).val)->tvs;_tmp17E=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166)->f1).aggr_info).KnownAggr).val)->impl;_tmp17D=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp17C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166)->f1).targs;_LL24:
# 585
 if(_tmp17E == 0){
if(_tmp17D)
return& Cyc_Tcutil_mk;else{
# 589
return& Cyc_Tcutil_ak;}}{
# 591
struct Cyc_List_List*_tmp169=_tmp17E->fields;
if(_tmp169 == 0)return& Cyc_Tcutil_mk;
# 594
if(_tmp180 == Cyc_Absyn_StructA){
for(0;((struct Cyc_List_List*)_check_null(_tmp169))->tl != 0;_tmp169=_tmp169->tl){;}{
void*_tmp16A=((struct Cyc_Absyn_Aggrfield*)_tmp169->hd)->type;
struct Cyc_Absyn_Kind*_tmp16B=Cyc_Tcutil_field_kind(_tmp16A,_tmp17C,_tmp17F);
if(_tmp16B == & Cyc_Tcutil_ak  || _tmp16B == & Cyc_Tcutil_tak)return _tmp16B;};}else{
# 602
for(0;_tmp169 != 0;_tmp169=_tmp169->tl){
void*_tmp16C=((struct Cyc_Absyn_Aggrfield*)_tmp169->hd)->type;
struct Cyc_Absyn_Kind*_tmp16D=Cyc_Tcutil_field_kind(_tmp16C,_tmp17C,_tmp17F);
if(_tmp16D == & Cyc_Tcutil_ak  || _tmp16D == & Cyc_Tcutil_tak)return _tmp16D;}}
# 608
return& Cyc_Tcutil_mk;};}case 12U: _LL25: _LL26:
 return& Cyc_Tcutil_mk;case 5U: _LL27: _tmp181=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp166)->f1;_LL28: {
# 611
void*_tmp16E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp181.ptr_atts).bounds);void*_tmp16F=_tmp16E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp16F)->tag == 0U){_LL44: _LL45: {
# 613
enum Cyc_Absyn_AliasQual _tmp170=(Cyc_Tcutil_typ_kind((_tmp181.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp171=_tmp170;switch(_tmp171){case Cyc_Absyn_Aliasable: _LL49: _LL4A:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL4B: _LL4C:
 return& Cyc_Tcutil_umk;default: _LL4D: _LL4E:
 return& Cyc_Tcutil_tmk;}_LL48:;}}else{_LL46: _LL47: {
# 619
enum Cyc_Absyn_AliasQual _tmp172=(Cyc_Tcutil_typ_kind((_tmp181.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp173=_tmp172;switch(_tmp173){case Cyc_Absyn_Aliasable: _LL50: _LL51:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL52: _LL53:
 return& Cyc_Tcutil_ubk;default: _LL54: _LL55:
 return& Cyc_Tcutil_tbk;}_LL4F:;}}_LL43:;}case 18U: _LL29: _LL2A:
# 625
 return& Cyc_Tcutil_ik;case 28U: _LL2B: _tmp182=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp166)->f2;_LL2C:
 return _tmp182;case 27U: _LL2D: _LL2E:
# 630
 return& Cyc_Tcutil_ak;case 19U: _LL2F: _LL30:
 return& Cyc_Tcutil_bk;case 8U: _LL31: _tmp183=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp166)->f1).num_elts;_LL32:
# 633
 if(_tmp183 == 0  || Cyc_Tcutil_is_const_exp(_tmp183))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10U: _LL33: _LL34:
 return& Cyc_Tcutil_mk;case 17U: _LL35: _tmp184=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp166)->f3;_LL36:
# 637
 if(_tmp184 == 0  || _tmp184->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp176=({struct Cyc_String_pa_PrintArg_struct _tmpA48;_tmpA48.tag=0U,({struct _dyneither_ptr _tmpB9A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA48.f1=_tmpB9A;});_tmpA48;});void*_tmp174[1U];_tmp174[0]=& _tmp176;({struct _dyneither_ptr _tmpB9B=({const char*_tmp175="typ_kind: typedef found: %s";_tag_dyneither(_tmp175,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB9B,_tag_dyneither(_tmp174,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp184->kind))->v;case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166)->f1)->r)){case 0U: _LL37: _LL38:
 return& Cyc_Tcutil_ak;case 1U: _LL39: _LL3A:
 return& Cyc_Tcutil_bk;default: _LL3B: _LL3C:
 return& Cyc_Tcutil_ak;}case 23U: _LL3D: _LL3E:
 goto _LL40;case 24U: _LL3F: _LL40:
 goto _LL42;default: _LL41: _LL42:
 return& Cyc_Tcutil_ek;}_LL0:;}
# 649
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 654
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp185;_push_handler(& _tmp185);{int _tmp187=0;if(setjmp(_tmp185.handler))_tmp187=1;if(!_tmp187){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp188=1;_npop_handler(0U);return _tmp188;};
# 656
;_pop_handler();}else{void*_tmp186=(void*)_exn_thrown;void*_tmp189=_tmp186;void*_tmp18A;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp189)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 658
 return 0;}else{_LL3: _tmp18A=_tmp189;_LL4:(int)_rethrow(_tmp18A);}_LL0:;}};}
# 663
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
({void*_tmpB9C=Cyc_Tcutil_compress(t);t=_tmpB9C;});{
void*_tmp18B=t;struct Cyc_List_List*_tmp1B1;void*_tmp1B0;void*_tmp1AF;void*_tmp1AE;void*_tmp1AD;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AB;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*_tmp1A7;struct Cyc_List_List*_tmp1A6;void*_tmp1A5;struct Cyc_Absyn_Tqual _tmp1A4;void*_tmp1A3;struct Cyc_List_List*_tmp1A2;int _tmp1A1;struct Cyc_Absyn_VarargInfo*_tmp1A0;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_List_List*_tmp19C;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_List_List*_tmp19A;void*_tmp199;struct Cyc_Absyn_PtrInfo _tmp198;void*_tmp197;struct Cyc_Core_Opt**_tmp196;struct Cyc_Absyn_Tvar*_tmp195;switch(*((int*)_tmp18B)){case 2U: _LL1: _tmp195=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp18B)->f1;_LL2:
# 668
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp195)){
({struct _dyneither_ptr _tmpB9D=({const char*_tmp18C="(type variable would escape scope)";_tag_dyneither(_tmp18C,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpB9D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 672
goto _LL0;case 1U: _LL3: _tmp197=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18B)->f2;_tmp196=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18B)->f4;_LL4:
# 674
 if(t == evar){
({struct _dyneither_ptr _tmpB9E=({const char*_tmp18D="(occurs check)";_tag_dyneither(_tmp18D,sizeof(char),15U);});Cyc_Tcutil_failure_reason=_tmpB9E;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 678
if(_tmp197 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp197);else{
# 681
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp196))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 688
if(problem){
struct Cyc_List_List*_tmp18E=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp196))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
({struct Cyc_List_List*_tmpB9F=({struct Cyc_List_List*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp18F->tl=_tmp18E;_tmp18F;});_tmp18E=_tmpB9F;});}}
# 694
({struct Cyc_Core_Opt*_tmpBA0=({struct Cyc_Core_Opt*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->v=_tmp18E;_tmp190;});*_tmp196=_tmpBA0;});}}}
# 697
goto _LL0;case 5U: _LL5: _tmp198=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18B)->f1;_LL6:
# 700
 Cyc_Tcutil_occurs(evar,r,env,_tmp198.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp198.ptr_atts).rgn);
goto _LL0;case 8U: _LL7: _tmp199=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18B)->f1).elt_type;_LL8:
# 704
 Cyc_Tcutil_occurs(evar,r,env,_tmp199);goto _LL0;case 9U: _LL9: _tmp1A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).tvars;_tmp1A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).effect;_tmp1A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ret_tqual;_tmp1A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ret_typ;_tmp1A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).args;_tmp1A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).c_varargs;_tmp1A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).cyc_varargs;_tmp19F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).rgn_po;_tmp19E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).attributes;_tmp19D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).requires_clause;_tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).requires_relns;_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ensures_clause;_tmp19A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ensures_relns;_LLA:
# 707
({struct Cyc_List_List*_tmpBA1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1A6,env);env=_tmpBA1;});
if(_tmp1A5 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1A5);
Cyc_Tcutil_occurs(evar,r,env,_tmp1A3);
for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1A2->hd)).f3);}
if(_tmp1A0 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1A0->type);
for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){
struct _tuple0*_tmp191=(struct _tuple0*)_tmp19F->hd;struct _tuple0*_tmp192=_tmp191;void*_tmp194;void*_tmp193;_LL24: _tmp194=_tmp192->f1;_tmp193=_tmp192->f2;_LL25:;
Cyc_Tcutil_occurs(evar,r,env,_tmp194);
Cyc_Tcutil_occurs(evar,r,env,_tmp193);}
# 719
goto _LL0;case 10U: _LLB: _tmp1A7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp18B)->f1;_LLC:
# 721
 for(0;_tmp1A7 != 0;_tmp1A7=_tmp1A7->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1A7->hd)).f2);}
goto _LL0;case 3U: _LLD: _tmp1A8=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp18B)->f1).targs;_LLE:
# 725
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1A8);goto _LL0;case 17U: _LLF: _tmp1A9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp18B)->f2;_LL10:
 _tmp1AA=_tmp1A9;goto _LL12;case 4U: _LL11: _tmp1AA=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp18B)->f1).targs;_LL12:
 _tmp1AB=_tmp1AA;goto _LL14;case 11U: _LL13: _tmp1AB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp18B)->f1).targs;_LL14:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1AB);goto _LL0;case 12U: _LL15: _tmp1AC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp18B)->f2;_LL16:
# 731
 for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1AC->hd)->type);}
goto _LL0;case 19U: _LL17: _tmp1AD=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp18B)->f1;_LL18:
 _tmp1AE=_tmp1AD;goto _LL1A;case 23U: _LL19: _tmp1AE=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp18B)->f1;_LL1A:
 _tmp1AF=_tmp1AE;goto _LL1C;case 15U: _LL1B: _tmp1AF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp18B)->f1;_LL1C:
 _tmp1B0=_tmp1AF;goto _LL1E;case 25U: _LL1D: _tmp1B0=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp18B)->f1;_LL1E:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1B0);goto _LL0;case 24U: _LL1F: _tmp1B1=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp18B)->f1;_LL20:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1B1);goto _LL0;default: _LL21: _LL22:
# 741
 goto _LL0;}_LL0:;};}
# 744
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 746
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 751
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 759
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1B2=0U;({struct _dyneither_ptr _tmpBA2=({const char*_tmp1B3="tq1 real_const not set.";_tag_dyneither(_tmp1B3,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA2,_tag_dyneither(_tmp1B2,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1B4=0U;({struct _dyneither_ptr _tmpBA3=({const char*_tmp1B5="tq2 real_const not set.";_tag_dyneither(_tmp1B5,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA3,_tag_dyneither(_tmp1B4,sizeof(void*),0U));});});
# 765
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 768
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpBA4=({const char*_tmp1B6="(qualifiers don't match)";_tag_dyneither(_tmp1B6,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBA4;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 776
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 780
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 787
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 789
({union Cyc_Absyn_Constraint*_tmpBA5=Cyc_Absyn_compress_conref(x);x=_tmpBA5;});
({union Cyc_Absyn_Constraint*_tmpBA6=Cyc_Absyn_compress_conref(y);y=_tmpBA6;});
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1B7=x;void*_tmp1BE;switch((((union Cyc_Absyn_Constraint*)_tmp1B7)->Eq_constr).tag){case 3U: _LL1: _LL2:
# 794
({union Cyc_Absyn_Constraint _tmpBA7=({union Cyc_Absyn_Constraint _tmpA49;(_tmpA49.Forward_constr).tag=2U,(_tmpA49.Forward_constr).val=y;_tmpA49;});*x=_tmpBA7;});return;case 1U: _LL3: _tmp1BE=(void*)(_tmp1B7->Eq_constr).val;_LL4: {
# 796
union Cyc_Absyn_Constraint*_tmp1B8=y;void*_tmp1BB;switch((((union Cyc_Absyn_Constraint*)_tmp1B8)->Eq_constr).tag){case 3U: _LL8: _LL9:
*y=*x;return;case 1U: _LLA: _tmp1BB=(void*)(_tmp1B8->Eq_constr).val;_LLB:
# 799
 if(cmp(_tmp1BE,_tmp1BB)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 803
return;default: _LLC: _LLD:
({void*_tmp1B9=0U;({struct _dyneither_ptr _tmpBA8=({const char*_tmp1BA="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1BA,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpBA8,_tag_dyneither(_tmp1B9,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 806
({void*_tmp1BC=0U;({struct _dyneither_ptr _tmpBA9=({const char*_tmp1BD="unify_conref: forward after compress";_tag_dyneither(_tmp1BD,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpBA9,_tag_dyneither(_tmp1BC,sizeof(void*),0U));});});}_LL0:;};}
# 810
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1BF;_push_handler(& _tmp1BF);{int _tmp1C1=0;if(setjmp(_tmp1BF.handler))_tmp1C1=1;if(!_tmp1C1){
({int(*_tmpBAC)(void*,void*)=cmp;union Cyc_Absyn_Constraint*_tmpBAB=x;union Cyc_Absyn_Constraint*_tmpBAA=y;Cyc_Tcutil_unify_it_conrefs(_tmpBAC,_tmpBAB,_tmpBAA,_tag_dyneither(0,0,0));});{
int _tmp1C2=1;_npop_handler(0U);return _tmp1C2;};
# 812
;_pop_handler();}else{void*_tmp1C0=(void*)_exn_thrown;void*_tmp1C3=_tmp1C0;void*_tmp1C4;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1C3)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 814
 return 0;}else{_LL3: _tmp1C4=_tmp1C3;_LL4:(int)_rethrow(_tmp1C4);}_LL0:;}};}
# 817
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp1C5=({struct _tuple0 _tmpA4A;_tmpA4A.f1=b1,_tmpA4A.f2=b2;_tmpA4A;});struct _tuple0 _tmp1C6=_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C6.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C6.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C6.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{_LL7: _tmp1C8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1C6.f1)->f1;_tmp1C7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1C6.f2)->f1;_LL8:
 return Cyc_Evexp_const_exp_cmp(_tmp1C8,_tmp1C7);}}_LL0:;}
# 826
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1C9=({struct _tuple0 _tmpA4B;_tmpA4B.f1=e1->r,_tmpA4B.f2=e2->r;_tmpA4B;});struct _tuple0 _tmp1CA=_tmp1C9;void*_tmp1D4;void*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D1;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->tag == 14U){_LL1: _tmp1D1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->f2;_LL2:
# 831
 Cyc_Tcutil_unify_cmp_exp(_tmp1D1,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->tag == 14U){_LL3: _tmp1D2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1D2);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->tag == 39U){_LL5: _tmp1D3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->f1;_LL6: {
# 834
void*_tmp1CB=Cyc_Tcutil_compress(_tmp1D3);void*_tmp1CC=_tmp1CB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CC)->tag == 1U){_LLC: _LLD:
({void*_tmpBAD=_tmp1D3;Cyc_Tcutil_unify_it(_tmpBAD,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->tag=18U,_tmp1CD->f1=e2;_tmp1CD;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->tag == 39U){_LL7: _tmp1D4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->f1;_LL8: {
# 839
void*_tmp1CE=Cyc_Tcutil_compress(_tmp1D4);void*_tmp1CF=_tmp1CE;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->tag == 1U){_LL11: _LL12:
({void*_tmpBAE=_tmp1D4;Cyc_Tcutil_unify_it(_tmpBAE,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->tag=18U,_tmp1D0->f1=e1;_tmp1D0;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 843
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 847
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp1D5=({struct _tuple0 _tmpA4C;_tmpA4C.f1=b1,_tmpA4C.f2=b2;_tmpA4C;});struct _tuple0 _tmp1D6=_tmp1D5;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Exp*_tmp1DD;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D6.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D6.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D6.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{_LL7: _tmp1DE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1D6.f1)->f1;_tmp1DD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1D6.f2)->f1;_LL8: {
# 853
int r=Cyc_Evexp_const_exp_cmp(_tmp1DE,_tmp1DD);
if(r == 0)return 0;{
struct _handler_cons _tmp1D7;_push_handler(& _tmp1D7);{int _tmp1D9=0;if(setjmp(_tmp1D7.handler))_tmp1D9=1;if(!_tmp1D9){
Cyc_Tcutil_unify_cmp_exp(_tmp1DE,_tmp1DD);{
int _tmp1DA=0;_npop_handler(0U);return _tmp1DA;};
# 856
;_pop_handler();}else{void*_tmp1D8=(void*)_exn_thrown;void*_tmp1DB=_tmp1D8;void*_tmp1DC;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1DB)->tag == Cyc_Tcutil_Unify){_LLA: _LLB:
# 859
 return r;}else{_LLC: _tmp1DC=_tmp1DB;_LLD:(int)_rethrow(_tmp1DC);}_LL9:;}};};}}}_LL0:;}
# 864
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp1DF=att;switch(*((int*)_tmp1DF)){case 0U: _LL1: _LL2:
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
# 891
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp1E0=({struct _tuple0 _tmpA4D;_tmpA4D.f1=att1,_tmpA4D.f2=att2;_tmpA4D;});struct _tuple0 _tmp1E1=_tmp1E0;enum Cyc_Absyn_Format_Type _tmp1F1;int _tmp1F0;int _tmp1EF;enum Cyc_Absyn_Format_Type _tmp1EE;int _tmp1ED;int _tmp1EC;struct _dyneither_ptr _tmp1EB;struct _dyneither_ptr _tmp1EA;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1E8;int _tmp1E7;int _tmp1E6;int _tmp1E5;int _tmp1E4;switch(*((int*)_tmp1E1.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E1.f2)->tag == 0U){_LL1: _tmp1E5=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f1;_tmp1E4=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f1;_LL2:
 _tmp1E7=_tmp1E5;_tmp1E6=_tmp1E4;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1E1.f2)->tag == 20U){_LL3: _tmp1E7=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f1;_tmp1E6=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f1;_LL4:
# 895
 return Cyc_Core_intcmp(_tmp1E7,_tmp1E6);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1E1.f2)->tag == 6U){_LL5: _tmp1E9=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f1;_tmp1E8=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f1;_LL6:
# 897
 if(_tmp1E9 == _tmp1E8)return 0;
if(_tmp1E9 == 0)return - 1;
if(_tmp1E8 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp1E9,_tmp1E8);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1E1.f2)->tag == 8U){_LL7: _tmp1EB=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f1;_tmp1EA=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1EB,(struct _dyneither_ptr)_tmp1EA);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E1.f2)->tag == 19U){_LL9: _tmp1F1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f1;_tmp1F0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f2;_tmp1EF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f3;_tmp1EE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f1;_tmp1ED=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f2;_tmp1EC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f3;_LLA: {
# 903
int _tmp1E2=Cyc_Core_intcmp((int)((unsigned int)_tmp1F1),(int)((unsigned int)_tmp1EE));
if(_tmp1E2 != 0)return _tmp1E2;{
int _tmp1E3=Cyc_Core_intcmp(_tmp1F0,_tmp1ED);
if(_tmp1E3 != 0)return _tmp1E3;
return Cyc_Core_intcmp(_tmp1EF,_tmp1EC);};}}else{goto _LLB;}default: _LLB: _LLC:
# 909
 return({int _tmpBAF=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpBAF,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
# 913
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 917
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 926
static void*Cyc_Tcutil_rgns_of(void*t);
# 928
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 932
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp1F2=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1F3=_tmp1F2;switch(((struct Cyc_Absyn_Kind*)_tmp1F3)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp1F3)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL0;default: goto _LL9;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
({void*_tmpBB1=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->tag=18U,({struct Cyc_Absyn_Exp*_tmpBB0=Cyc_Absyn_uint_exp(0U,0U);_tmp1F4->f1=_tmpBB0;});_tmp1F4;});t=_tmpBB1;});goto _LL0;default: _LL9: _LLA:
 t=Cyc_Absyn_sint_typ;goto _LL0;}_LL0:;}
# 941
return({struct _tuple16*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->f1=tv,_tmp1F5->f2=t;_tmp1F5;});}
# 948
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1F6=Cyc_Tcutil_compress(t);void*_tmp1F7=_tmp1F6;struct Cyc_List_List*_tmp21D;void*_tmp21C;struct Cyc_List_List*_tmp21B;void*_tmp21A;struct Cyc_Absyn_Tqual _tmp219;void*_tmp218;struct Cyc_List_List*_tmp217;struct Cyc_Absyn_VarargInfo*_tmp216;struct Cyc_List_List*_tmp215;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp211;void*_tmp210;void*_tmp20F;void*_tmp20E;struct Cyc_List_List*_tmp20D;void*_tmp20C;void*_tmp20B;void*_tmp20A;switch(*((int*)_tmp1F7)){case 0U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 goto _LLA;case 18U: _LL9: _LLA:
 goto _LLC;case 27U: _LLB: _LLC:
 goto _LLE;case 28U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 return Cyc_Absyn_empty_effect;case 1U: _LL11: _LL12:
 goto _LL14;case 2U: _LL13: _LL14: {
# 960
struct Cyc_Absyn_Kind*_tmp1F8=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1F9=_tmp1F8;switch(((struct Cyc_Absyn_Kind*)_tmp1F9)->kind){case Cyc_Absyn_RgnKind: _LL3C: _LL3D:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->tag=23U,_tmp1FA->f1=t;_tmp1FA;});case Cyc_Absyn_EffKind: _LL3E: _LL3F:
 return t;case Cyc_Absyn_IntKind: _LL40: _LL41:
 return Cyc_Absyn_empty_effect;default: _LL42: _LL43:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB->tag=25U,_tmp1FB->f1=t;_tmp1FB;});}_LL3B:;}case 15U: _LL15: _tmp20A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F7)->f1;_LL16:
# 966
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->tag=23U,_tmp1FC->f1=_tmp20A;_tmp1FC;});case 16U: _LL17: _tmp20C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F7)->f1;_tmp20B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F7)->f2;_LL18:
# 970
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));_tmp1FD->tag=23U,_tmp1FD->f1=_tmp20B;_tmp1FD;});case 3U: _LL19: _tmp20D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F7)->f1).targs;_LL1A: {
# 972
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp20D);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->tag=24U,_tmp1FE->f1=ts;_tmp1FE;}));}case 5U: _LL1B: _tmp20F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F7)->f1).elt_typ;_tmp20E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F7)->f1).ptr_atts).rgn;_LL1C:
# 976
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->tag=24U,({struct Cyc_List_List*_tmpBB4=({void*_tmp1FF[2U];({void*_tmpBB3=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->tag=23U,_tmp200->f1=_tmp20E;_tmp200;});_tmp1FF[0]=_tmpBB3;}),({void*_tmpBB2=Cyc_Tcutil_rgns_of(_tmp20F);_tmp1FF[1]=_tmpBB2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1FF,sizeof(void*),2U));});_tmp201->f1=_tmpBB4;});_tmp201;}));case 8U: _LL1D: _tmp210=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F7)->f1).elt_type;_LL1E:
# 978
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp210));case 10U: _LL1F: _tmp211=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F7)->f1;_LL20: {
# 980
struct Cyc_List_List*_tmp202=0;
for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
({struct Cyc_List_List*_tmpBB5=({struct Cyc_List_List*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->hd=(*((struct _tuple12*)_tmp211->hd)).f2,_tmp203->tl=_tmp202;_tmp203;});_tmp202=_tmpBB5;});}
_tmp212=_tmp202;goto _LL22;}case 4U: _LL21: _tmp212=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F7)->f1).targs;_LL22:
 _tmp213=_tmp212;goto _LL24;case 11U: _LL23: _tmp213=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F7)->f1).targs;_LL24:
# 986
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->tag=24U,({struct Cyc_List_List*_tmpBB6=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp213);_tmp204->f1=_tmpBB6;});_tmp204;}));case 12U: _LL25: _tmp214=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F7)->f2;_LL26:
# 988
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->tag=24U,({struct Cyc_List_List*_tmpBB7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp214);_tmp205->f1=_tmpBB7;});_tmp205;}));case 19U: _LL27: _LL28:
 return Cyc_Absyn_empty_effect;case 9U: _LL29: _tmp21B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).tvars;_tmp21A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).effect;_tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).ret_tqual;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).ret_typ;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).args;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).cyc_varargs;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).rgn_po;_LL2A: {
# 998
void*_tmp206=({struct Cyc_List_List*_tmpBB8=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp21B);Cyc_Tcutil_substitute(_tmpBB8,(void*)_check_null(_tmp21A));});
return Cyc_Tcutil_normalize_effect(_tmp206);}case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 return Cyc_Absyn_empty_effect;case 23U: _LL31: _LL32:
 goto _LL34;case 24U: _LL33: _LL34:
 return t;case 25U: _LL35: _tmp21C=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1F7)->f1;_LL36:
 return Cyc_Tcutil_rgns_of(_tmp21C);case 17U: _LL37: _tmp21D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F7)->f2;_LL38:
# 1007
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->tag=24U,({struct Cyc_List_List*_tmpBB9=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp21D);_tmp207->f1=_tmpBB9;});_tmp207;}));default: _LL39: _LL3A:
({void*_tmp208=0U;({struct _dyneither_ptr _tmpBBA=({const char*_tmp209="typedecl in rgns_of";_tag_dyneither(_tmp209,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBBA,_tag_dyneither(_tmp208,sizeof(void*),0U));});});}_LL0:;}
# 1015
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpBBB=Cyc_Tcutil_compress(e);e=_tmpBBB;});{
void*_tmp21E=e;void*_tmp22A;struct Cyc_List_List**_tmp229;switch(*((int*)_tmp21E)){case 24U: _LL1: _tmp229=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp21E)->f1;_LL2: {
# 1019
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp229;for(0;effs != 0;effs=effs->tl){
void*_tmp21F=(void*)effs->hd;
({void*_tmpBBC=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp21F));effs->hd=_tmpBBC;});{
void*_tmp220=(void*)effs->hd;void*_tmp221=_tmp220;switch(*((int*)_tmp221)){case 24U: _LL8: _LL9:
 goto _LLB;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp221)->f1)){case 20U: _LLA: _LLB:
 goto _LLD;case 22U: _LLC: _LLD:
 goto _LLF;case 21U: _LLE: _LLF:
 redo_join=1;goto _LL7;default: goto _LL10;}default: _LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1031
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp229;for(0;effs != 0;effs=effs->tl){
void*_tmp222=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp223=_tmp222;void*_tmp226;struct Cyc_List_List*_tmp225;switch(*((int*)_tmp223)){case 24U: _LL13: _tmp225=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp223)->f1;_LL14:
# 1036
({struct Cyc_List_List*_tmpBBD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp225,effects);effects=_tmpBBD;});
goto _LL12;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp223)->f1)){case 20U: _LL15: _LL16:
 goto _LL18;case 22U: _LL17: _LL18:
 goto _LL1A;case 21U: _LL19: _LL1A:
 goto _LL12;default: goto _LL1B;}default: _LL1B: _tmp226=_tmp223;_LL1C:
({struct Cyc_List_List*_tmpBBE=({struct Cyc_List_List*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->hd=_tmp226,_tmp224->tl=effects;_tmp224;});effects=_tmpBBE;});goto _LL12;}_LL12:;}}
# 1044
({struct Cyc_List_List*_tmpBBF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp229=_tmpBBF;});
return e;};}case 25U: _LL3: _tmp22A=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp21E)->f1;_LL4: {
# 1047
void*_tmp227=Cyc_Tcutil_compress(_tmp22A);void*_tmp228=_tmp227;switch(*((int*)_tmp228)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp22A);}_LL1D:;}default: _LL5: _LL6:
# 1052
 return e;}_LL0:;};}
# 1057
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22B=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->tag=9U,(_tmp22C->f1).tvars=0,(_tmp22C->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmpBC0=Cyc_Absyn_empty_tqual(0U);(_tmp22C->f1).ret_tqual=_tmpBC0;}),(_tmp22C->f1).ret_typ=(void*)& Cyc_Absyn_VoidType_val,(_tmp22C->f1).args=0,(_tmp22C->f1).c_varargs=0,(_tmp22C->f1).cyc_varargs=0,(_tmp22C->f1).rgn_po=0,(_tmp22C->f1).attributes=0,(_tmp22C->f1).requires_clause=0,(_tmp22C->f1).requires_relns=0,(_tmp22C->f1).ensures_clause=0,(_tmp22C->f1).ensures_relns=0;_tmp22C;});
# 1067
return({void*_tmpBC3=(void*)_tmp22B;struct Cyc_Absyn_Tqual _tmpBC2=Cyc_Absyn_empty_tqual(0U);void*_tmpBC1=Cyc_Absyn_bounds_one;Cyc_Absyn_atb_typ(_tmpBC3,(void*)& Cyc_Absyn_HeapRgn_val,_tmpBC2,_tmpBC1,Cyc_Absyn_false_conref);});}
# 1074
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpBC4=Cyc_Tcutil_compress(r);r=_tmpBC4;});
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp22D=Cyc_Tcutil_compress(e);void*_tmp22E=_tmp22D;struct Cyc_Core_Opt*_tmp24E;void**_tmp24D;struct Cyc_Core_Opt*_tmp24C;void*_tmp24B;struct Cyc_List_List*_tmp24A;void*_tmp249;switch(*((int*)_tmp22E)){case 23U: _LL1: _tmp249=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp22E)->f1;_LL2:
# 1081
 if(constrain)return Cyc_Tcutil_unify(r,_tmp249);
({void*_tmpBC5=Cyc_Tcutil_compress(_tmp249);_tmp249=_tmpBC5;});
if(r == _tmp249)return 1;{
struct _tuple0 _tmp22F=({struct _tuple0 _tmpA4E;_tmpA4E.f1=r,_tmpA4E.f2=_tmp249;_tmpA4E;});struct _tuple0 _tmp230=_tmp22F;struct Cyc_Absyn_Tvar*_tmp232;struct Cyc_Absyn_Tvar*_tmp231;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f2)->tag == 2U){_LLC: _tmp232=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f1)->f1;_tmp231=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp232,_tmp231)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};case 24U: _LL3: _tmp24A=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp22E)->f1;_LL4:
# 1089
 for(0;_tmp24A != 0;_tmp24A=_tmp24A->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp24A->hd))return 1;}
return 0;case 25U: _LL5: _tmp24B=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp22E)->f1;_LL6: {
# 1093
void*_tmp233=Cyc_Tcutil_rgns_of(_tmp24B);void*_tmp234=_tmp233;void*_tmp240;void*_tmp23F;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp234)->tag == 25U){_LL11: _tmp23F=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp234)->f1;_LL12:
# 1095
 if(!constrain)return 0;{
void*_tmp235=Cyc_Tcutil_compress(_tmp23F);void*_tmp236=_tmp235;struct Cyc_Core_Opt*_tmp23E;void**_tmp23D;struct Cyc_Core_Opt*_tmp23C;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp236)->tag == 1U){_LL16: _tmp23E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp236)->f1;_tmp23D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp236)->f2;_tmp23C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp236)->f4;_LL17: {
# 1100
void*_tmp237=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp23C);
# 1103
Cyc_Tcutil_occurs(_tmp237,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23C))->v,r);{
void*_tmp238=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->tag=24U,({struct Cyc_List_List*_tmpBC7=({void*_tmp239[2U];_tmp239[0]=_tmp237,({void*_tmpBC6=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->tag=23U,_tmp23A->f1=r;_tmp23A;});_tmp239[1]=_tmpBC6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp239,sizeof(void*),2U));});_tmp23B->f1=_tmpBC7;});_tmp23B;}));
*_tmp23D=_tmp238;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{_LL13: _tmp240=_tmp234;_LL14:
# 1109
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp240);}_LL10:;}case 1U: _LL7: _tmp24E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f1;_tmp24D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f2;_tmp24C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f4;_LL8:
# 1112
 if(_tmp24E == 0  || ((struct Cyc_Absyn_Kind*)_tmp24E->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp241=0U;({struct _dyneither_ptr _tmpBC8=({const char*_tmp242="effect evar has wrong kind";_tag_dyneither(_tmp242,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC8,_tag_dyneither(_tmp241,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1117
void*_tmp243=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp24C);
# 1120
Cyc_Tcutil_occurs(_tmp243,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24C))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp244=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->tag=24U,({struct Cyc_List_List*_tmpBCB=({struct Cyc_List_List*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->hd=_tmp243,({struct Cyc_List_List*_tmpBCA=({struct Cyc_List_List*_tmp246=_cycalloc(sizeof(*_tmp246));({void*_tmpBC9=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->tag=23U,_tmp245->f1=r;_tmp245;});_tmp246->hd=_tmpBC9;}),_tmp246->tl=0;_tmp246;});_tmp247->tl=_tmpBCA;});_tmp247;});_tmp248->f1=_tmpBCB;});_tmp248;});
*_tmp24D=(void*)_tmp244;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1131
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpBCC=Cyc_Tcutil_compress(t);t=_tmpBCC;});{
void*_tmp24F=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp250=_tmp24F;struct Cyc_Core_Opt*_tmp265;void**_tmp264;struct Cyc_Core_Opt*_tmp263;void*_tmp262;struct Cyc_List_List*_tmp261;switch(*((int*)_tmp250)){case 23U: _LL1: _LL2:
 return 0;case 24U: _LL3: _tmp261=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp250)->f1;_LL4:
# 1136
 for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp261->hd))
return 1;}
return 0;case 25U: _LL5: _tmp262=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp250)->f1;_LL6:
# 1141
({void*_tmpBCD=Cyc_Tcutil_compress(_tmp262);_tmp262=_tmpBCD;});
if(t == _tmp262)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp262);{
void*_tmp251=Cyc_Tcutil_rgns_of(t);void*_tmp252=_tmp251;void*_tmp258;void*_tmp257;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp252)->tag == 25U){_LLC: _tmp257=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp252)->f1;_LLD: {
# 1146
struct _tuple0 _tmp253=({struct _tuple0 _tmpA4F;({void*_tmpBCE=Cyc_Tcutil_compress(_tmp257);_tmpA4F.f1=_tmpBCE;}),_tmpA4F.f2=_tmp262;_tmpA4F;});struct _tuple0 _tmp254=_tmp253;struct Cyc_Absyn_Tvar*_tmp256;struct Cyc_Absyn_Tvar*_tmp255;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f2)->tag == 2U){_LL11: _tmp256=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f1)->f1;_tmp255=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp262);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp257 == _tmp262;}_LL10:;}}else{_LLE: _tmp258=_tmp252;_LLF:
# 1150
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp258);}_LLB:;};case 1U: _LL7: _tmp265=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp250)->f1;_tmp264=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp250)->f2;_tmp263=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp250)->f4;_LL8:
# 1153
 if(_tmp265 == 0  || ((struct Cyc_Absyn_Kind*)_tmp265->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp259=0U;({struct _dyneither_ptr _tmpBCF=({const char*_tmp25A="effect evar has wrong kind";_tag_dyneither(_tmp25A,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBCF,_tag_dyneither(_tmp259,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1158
void*_tmp25B=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp263);
# 1161
Cyc_Tcutil_occurs(_tmp25B,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp263))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25C=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->tag=24U,({struct Cyc_List_List*_tmpBD2=({struct Cyc_List_List*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->hd=_tmp25B,({struct Cyc_List_List*_tmpBD1=({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));({void*_tmpBD0=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->tag=25U,_tmp25D->f1=t;_tmp25D;});_tmp25E->hd=_tmpBD0;}),_tmp25E->tl=0;_tmp25E;});_tmp25F->tl=_tmpBD1;});_tmp25F;});_tmp260->f1=_tmpBD2;});_tmp260;});
*_tmp264=(void*)_tmp25C;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1172
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpBD3=Cyc_Tcutil_compress(e);e=_tmpBD3;});{
void*_tmp266=e;struct Cyc_Core_Opt*_tmp282;void**_tmp281;struct Cyc_Core_Opt*_tmp280;void*_tmp27F;struct Cyc_List_List*_tmp27E;struct Cyc_Absyn_Tvar*_tmp27D;switch(*((int*)_tmp266)){case 2U: _LL1: _tmp27D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp266)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp27D)== 0;case 24U: _LL3: _tmp27E=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp266)->f1;_LL4:
# 1177
 for(0;_tmp27E != 0;_tmp27E=_tmp27E->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp27E->hd))
return 1;}
return 0;case 25U: _LL5: _tmp27F=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp266)->f1;_LL6: {
# 1182
void*_tmp267=Cyc_Tcutil_rgns_of(_tmp27F);void*_tmp268=_tmp267;void*_tmp274;void*_tmp273;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp268)->tag == 25U){_LLC: _tmp273=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp268)->f1;_LLD:
# 1184
 if(!may_constrain_evars)return 0;{
void*_tmp269=Cyc_Tcutil_compress(_tmp273);void*_tmp26A=_tmp269;struct Cyc_Core_Opt*_tmp272;void**_tmp271;struct Cyc_Core_Opt*_tmp270;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26A)->tag == 1U){_LL11: _tmp272=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26A)->f1;_tmp271=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26A)->f2;_tmp270=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp26A)->f4;_LL12: {
# 1190
void*_tmp26B=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp270);
# 1192
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp270))->v,v))return 0;{
void*_tmp26C=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->tag=24U,({struct Cyc_List_List*_tmpBD5=({void*_tmp26D[2U];_tmp26D[0]=_tmp26B,({void*_tmpBD4=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->tag=2U,_tmp26E->f1=v;_tmp26E;});_tmp26D[1]=_tmpBD4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26D,sizeof(void*),2U));});_tmp26F->f1=_tmpBD5;});_tmp26F;}));
*_tmp271=_tmp26C;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{_LLE: _tmp274=_tmp268;_LLF:
# 1198
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp274);}_LLB:;}case 1U: _LL7: _tmp282=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp266)->f1;_tmp281=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp266)->f2;_tmp280=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp266)->f4;_LL8:
# 1201
 if(_tmp282 == 0  || ((struct Cyc_Absyn_Kind*)_tmp282->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp275=0U;({struct _dyneither_ptr _tmpBD6=({const char*_tmp276="effect evar has wrong kind";_tag_dyneither(_tmp276,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBD6,_tag_dyneither(_tmp275,sizeof(void*),0U));});});{
# 1205
void*_tmp277=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp280);
# 1207
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp280))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp278=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->tag=24U,({struct Cyc_List_List*_tmpBD9=({struct Cyc_List_List*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->hd=_tmp277,({struct Cyc_List_List*_tmpBD8=({struct Cyc_List_List*_tmp27A=_cycalloc(sizeof(*_tmp27A));({void*_tmpBD7=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->tag=2U,_tmp279->f1=v;_tmp279;});_tmp27A->hd=_tmpBD7;}),_tmp27A->tl=0;_tmp27A;});_tmp27B->tl=_tmpBD8;});_tmp27B;});_tmp27C->f1=_tmpBD9;});_tmp27C;});
*_tmp281=(void*)_tmp278;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1217
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpBDA=Cyc_Tcutil_compress(e);e=_tmpBDA;});{
void*_tmp283=e;void*_tmp289;struct Cyc_List_List*_tmp288;switch(*((int*)_tmp283)){case 24U: _LL1: _tmp288=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp283)->f1;_LL2:
# 1221
 for(0;_tmp288 != 0;_tmp288=_tmp288->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp288->hd))
return 1;}
return 0;case 25U: _LL3: _tmp289=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp283)->f1;_LL4: {
# 1226
void*_tmp284=Cyc_Tcutil_rgns_of(_tmp289);void*_tmp285=_tmp284;void*_tmp287;void*_tmp286;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp285)->tag == 25U){_LLA: _tmp286=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp285)->f1;_LLB:
 return 0;}else{_LLC: _tmp287=_tmp285;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp287);}_LL9:;}case 1U: _LL5: _LL6:
# 1230
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1244 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1249
void*_tmp28A=Cyc_Tcutil_compress(e1);void*_tmp28B=_tmp28A;void**_tmp298;struct Cyc_Core_Opt*_tmp297;void*_tmp296;struct Cyc_Absyn_Tvar*_tmp295;void*_tmp294;struct Cyc_List_List*_tmp293;switch(*((int*)_tmp28B)){case 24U: _LL1: _tmp293=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp28B)->f1;_LL2:
# 1251
 for(0;_tmp293 != 0;_tmp293=_tmp293->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp293->hd,e2))
return 0;}
return 1;case 23U: _LL3: _tmp294=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp28B)->f1;_LL4:
# 1262
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp294,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp294,(void*)& Cyc_Absyn_HeapRgn_val);case 2U: _LL5: _tmp295=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp28B)->f1;_LL6:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp295,e2);case 25U: _LL7: _tmp296=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28B)->f1;_LL8: {
# 1266
void*_tmp28C=Cyc_Tcutil_rgns_of(_tmp296);void*_tmp28D=_tmp28C;void*_tmp28F;void*_tmp28E;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28D)->tag == 25U){_LLE: _tmp28E=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28D)->f1;_LLF:
# 1271
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp28E,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp28E,Cyc_Absyn_sint_typ);}else{_LL10: _tmp28F=_tmp28D;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp28F,e2);}_LLD:;}case 1U: _LL9: _tmp298=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28B)->f2;_tmp297=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28B)->f4;_LLA:
# 1276
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1280
*_tmp298=Cyc_Absyn_empty_effect;
# 1283
return 1;}else{
# 1285
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp292=({struct Cyc_String_pa_PrintArg_struct _tmpA50;_tmpA50.tag=0U,({struct _dyneither_ptr _tmpBDB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmpA50.f1=_tmpBDB;});_tmpA50;});void*_tmp290[1U];_tmp290[0]=& _tmp292;({struct _dyneither_ptr _tmpBDC=({const char*_tmp291="subset_effect: bad effect: %s";_tag_dyneither(_tmp291,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBDC,_tag_dyneither(_tmp290,sizeof(void*),1U));});});}_LL0:;}
# 1301 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpBDD=Cyc_Tcutil_normalize_effect(e1);e1=_tmpBDD;});
({void*_tmpBDE=Cyc_Tcutil_normalize_effect(e2);e2=_tmpBDE;});
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1317
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1319
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp299=(struct _tuple0*)r1->hd;struct _tuple0*_tmp29A=_tmp299;void*_tmp2A0;void*_tmp29F;_LL1: _tmp2A0=_tmp29A->f1;_tmp29F=_tmp29A->f2;_LL2:;{
int found=_tmp2A0 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp29B=(struct _tuple0*)r2->hd;struct _tuple0*_tmp29C=_tmp29B;void*_tmp29E;void*_tmp29D;_LL4: _tmp29E=_tmp29C->f1;_tmp29D=_tmp29C->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp2A0,_tmp29E) && Cyc_Tcutil_unify(_tmp29F,_tmp29D)){
found=1;
break;}}}
# 1329
if(!found)return 0;};}}
# 1331
return 1;}
# 1338
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp2A1=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp2A2=({struct Cyc_List_List*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->hd=_tmp2A1,_tmp2A3->tl=r1;_tmp2A3;});
if(Cyc_Relations_consistent_relations(_tmp2A2))return 0;}
# 1344
return 1;}
# 1349
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1355
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1357
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1361
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpBDF=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpBDF;});
({void*_tmpBE0=Cyc_Tcutil_compress(t1);t1=_tmpBE0;});
({void*_tmpBE1=Cyc_Tcutil_compress(t2);t2=_tmpBE1;});
if(t1 == t2)return;
{void*_tmp2A4=t1;struct Cyc_Core_Opt*_tmp2B3;void**_tmp2B2;struct Cyc_Core_Opt*_tmp2B1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->tag == 1U){_LL1: _tmp2B3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->f1;_tmp2B2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->f2;_tmp2B1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A4)->f4;_LL2:
# 1372
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B1))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2A5=Cyc_Tcutil_typ_kind(t2);
# 1377
if(Cyc_Tcutil_kind_leq(_tmp2A5,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2B3))->v)){
*_tmp2B2=t2;
return;}else{
# 1381
{void*_tmp2A6=t2;struct Cyc_Absyn_PtrInfo _tmp2AF;void**_tmp2AE;struct Cyc_Core_Opt*_tmp2AD;switch(*((int*)_tmp2A6)){case 1U: _LL6: _tmp2AE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f2;_tmp2AD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f4;_LL7: {
# 1384
struct Cyc_List_List*_tmp2A7=(struct Cyc_List_List*)_tmp2B1->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2AD))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2A7,(struct Cyc_Absyn_Tvar*)s2->hd)){
({struct _dyneither_ptr _tmpBE2=({const char*_tmp2A8="(type variable would escape scope)";_tag_dyneither(_tmp2A8,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpBE2;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1391
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2B3->v,_tmp2A5)){
*_tmp2AE=t1;return;}
# 1394
({struct _dyneither_ptr _tmpBE3=({const char*_tmp2A9="(kinds are incompatible)";_tag_dyneither(_tmp2A9,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBE3;});
goto _LL5;}case 5U: _LL8: _tmp2AF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A6)->f1;if(((struct Cyc_Absyn_Kind*)_tmp2B3->v)->kind == Cyc_Absyn_BoxKind){_LL9: {
# 1400
union Cyc_Absyn_Constraint*_tmp2AA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp2AF.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp2AB=_tmp2AA;if((((union Cyc_Absyn_Constraint*)_tmp2AB)->No_constr).tag == 3){_LLD: _LLE:
# 1404
({struct _union_Constraint_Eq_constr*_tmp2AC=& _tmp2AA->Eq_constr;_tmp2AC->tag=1;_tmp2AC->val=Cyc_Absyn_bounds_one;});
*_tmp2B2=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1409
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1412
({struct _dyneither_ptr _tmpBE4=({const char*_tmp2B0="(kinds are incompatible)";_tag_dyneither(_tmp2B0,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBE4;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1415
 goto _LL0;}_LL0:;}
# 1420
{struct _tuple0 _tmp2B4=({struct _tuple0 _tmpA56;_tmpA56.f1=t2,_tmpA56.f2=t1;_tmpA56;});struct _tuple0 _tmp2B5=_tmp2B4;void*_tmp371;void*_tmp370;void*_tmp36F;void*_tmp36E;void*_tmp36D;void*_tmp36C;struct Cyc_List_List*_tmp36B;struct Cyc_Absyn_Typedefdecl*_tmp36A;struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Typedefdecl*_tmp368;enum Cyc_Absyn_AggrKind _tmp367;struct Cyc_List_List*_tmp366;enum Cyc_Absyn_AggrKind _tmp365;struct Cyc_List_List*_tmp364;struct Cyc_List_List*_tmp363;struct Cyc_List_List*_tmp362;struct Cyc_List_List*_tmp361;void*_tmp360;struct Cyc_Absyn_Tqual _tmp35F;void*_tmp35E;struct Cyc_List_List*_tmp35D;int _tmp35C;struct Cyc_Absyn_VarargInfo*_tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_List_List*_tmp357;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_List_List*_tmp355;struct Cyc_List_List*_tmp354;void*_tmp353;struct Cyc_Absyn_Tqual _tmp352;void*_tmp351;struct Cyc_List_List*_tmp350;int _tmp34F;struct Cyc_Absyn_VarargInfo*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_List_List*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_List_List*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_List_List*_tmp348;void*_tmp347;struct Cyc_Absyn_Tqual _tmp346;struct Cyc_Absyn_Exp*_tmp345;union Cyc_Absyn_Constraint*_tmp344;void*_tmp343;struct Cyc_Absyn_Tqual _tmp342;struct Cyc_Absyn_Exp*_tmp341;union Cyc_Absyn_Constraint*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;void*_tmp33D;void*_tmp33C;int _tmp33B;int _tmp33A;enum Cyc_Absyn_Sign _tmp339;enum Cyc_Absyn_Size_of _tmp338;enum Cyc_Absyn_Sign _tmp337;enum Cyc_Absyn_Size_of _tmp336;void*_tmp335;struct Cyc_Absyn_Tqual _tmp334;void*_tmp333;union Cyc_Absyn_Constraint*_tmp332;union Cyc_Absyn_Constraint*_tmp331;union Cyc_Absyn_Constraint*_tmp330;void*_tmp32F;struct Cyc_Absyn_Tqual _tmp32E;void*_tmp32D;union Cyc_Absyn_Constraint*_tmp32C;union Cyc_Absyn_Constraint*_tmp32B;union Cyc_Absyn_Constraint*_tmp32A;struct Cyc_Absyn_Datatypedecl*_tmp329;struct Cyc_Absyn_Datatypefield*_tmp328;struct Cyc_List_List*_tmp327;struct Cyc_Absyn_Datatypedecl*_tmp326;struct Cyc_Absyn_Datatypefield*_tmp325;struct Cyc_List_List*_tmp324;struct Cyc_Absyn_Datatypedecl*_tmp323;struct Cyc_List_List*_tmp322;struct Cyc_Absyn_Datatypedecl*_tmp321;struct Cyc_List_List*_tmp320;struct Cyc_List_List*_tmp31F;struct Cyc_List_List*_tmp31E;struct _tuple2*_tmp31D;struct _tuple2*_tmp31C;union Cyc_Absyn_AggrInfoU _tmp31B;struct Cyc_List_List*_tmp31A;union Cyc_Absyn_AggrInfoU _tmp319;struct Cyc_List_List*_tmp318;struct Cyc_Absyn_Tvar*_tmp317;struct Cyc_Absyn_Tvar*_tmp316;switch(*((int*)_tmp2B5.f1)){case 1U: _LL12: _LL13:
# 1423
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: switch(*((int*)_tmp2B5.f2)){case 0U: _LL14: _LL15:
# 1426
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 2U: switch(*((int*)_tmp2B5.f2)){case 2U: _LL16: _tmp317=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp316=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL17: {
# 1429
struct _dyneither_ptr*_tmp2B6=_tmp317->name;
struct _dyneither_ptr*_tmp2B7=_tmp316->name;
# 1432
int _tmp2B8=_tmp317->identity;
int _tmp2B9=_tmp316->identity;
if(_tmp2B9 == _tmp2B8)return;
({struct _dyneither_ptr _tmpBE5=({const char*_tmp2BA="(variable types are not the same)";_tag_dyneither(_tmp2BA,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpBE5;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 11U: switch(*((int*)_tmp2B5.f2)){case 11U: _LL18: _tmp31B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f1)->f1).aggr_info;_tmp31A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f1)->f1).targs;_tmp319=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f2)->f1).aggr_info;_tmp318=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f2)->f1).targs;_LL19: {
# 1439
struct _tuple13 _tmp2BB=Cyc_Absyn_aggr_kinded_name(_tmp319);struct _tuple13 _tmp2BC=_tmp2BB;enum Cyc_Absyn_AggrKind _tmp2C4;struct _tuple2*_tmp2C3;_LL4F: _tmp2C4=_tmp2BC.f1;_tmp2C3=_tmp2BC.f2;_LL50:;{
struct _tuple13 _tmp2BD=Cyc_Absyn_aggr_kinded_name(_tmp31B);struct _tuple13 _tmp2BE=_tmp2BD;enum Cyc_Absyn_AggrKind _tmp2C2;struct _tuple2*_tmp2C1;_LL52: _tmp2C2=_tmp2BE.f1;_tmp2C1=_tmp2BE.f2;_LL53:;
if(_tmp2C4 != _tmp2C2){({struct _dyneither_ptr _tmpBE6=({const char*_tmp2BF="(struct and union type)";_tag_dyneither(_tmp2BF,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpBE6;});goto _LL11;}
if(Cyc_Absyn_qvar_cmp(_tmp2C3,_tmp2C1)!= 0){({struct _dyneither_ptr _tmpBE7=({const char*_tmp2C0="(different type name)";_tag_dyneither(_tmp2C0,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpBE7;});goto _LL11;}
Cyc_Tcutil_unify_list(_tmp318,_tmp31A);
return;};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 13U: switch(*((int*)_tmp2B5.f2)){case 13U: _LL1A: _tmp31D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp31C=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL1B:
# 1448
 if(Cyc_Absyn_qvar_cmp(_tmp31D,_tmp31C)== 0)return;
({struct _dyneither_ptr _tmpBE8=({const char*_tmp2C5="(different enum types)";_tag_dyneither(_tmp2C5,sizeof(char),23U);});Cyc_Tcutil_failure_reason=_tmpBE8;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 14U: switch(*((int*)_tmp2B5.f2)){case 14U: _LL1C: _tmp31F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp31E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL1D: {
# 1453
int bad=0;
for(0;_tmp31F != 0  && _tmp31E != 0;(_tmp31F=_tmp31F->tl,_tmp31E=_tmp31E->tl)){
struct Cyc_Absyn_Enumfield*_tmp2C6=(struct Cyc_Absyn_Enumfield*)_tmp31F->hd;
struct Cyc_Absyn_Enumfield*_tmp2C7=(struct Cyc_Absyn_Enumfield*)_tmp31E->hd;
if(Cyc_Absyn_qvar_cmp(_tmp2C6->name,_tmp2C7->name)!= 0){
({struct _dyneither_ptr _tmpBE9=({const char*_tmp2C8="(different names for enum fields)";_tag_dyneither(_tmp2C8,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpBE9;});
bad=1;
break;}
# 1462
if(_tmp2C6->tag == _tmp2C7->tag)continue;
if(_tmp2C6->tag == 0  || _tmp2C7->tag == 0){
({struct _dyneither_ptr _tmpBEA=({const char*_tmp2C9="(different tag values for enum fields)";_tag_dyneither(_tmp2C9,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBEA;});
bad=1;
break;}
# 1468
if(!({struct Cyc_Absyn_Exp*_tmpBEB=(struct Cyc_Absyn_Exp*)_check_null(_tmp2C6->tag);Cyc_Evexp_same_const_exp(_tmpBEB,(struct Cyc_Absyn_Exp*)_check_null(_tmp2C7->tag));})){
({struct _dyneither_ptr _tmpBEC=({const char*_tmp2CA="(different tag values for enum fields)";_tag_dyneither(_tmp2CA,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBEC;});
bad=1;
break;}}
# 1474
if(bad)goto _LL11;
if(_tmp31F == 0  && _tmp31E == 0)return;
({struct _dyneither_ptr _tmpBED=({const char*_tmp2CB="(different number of fields for enums)";_tag_dyneither(_tmp2CB,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBED;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B5.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp2B5.f2)){case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B5.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL1E: _tmp323=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B5.f1)->f1).datatype_info).KnownDatatype).val;_tmp322=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B5.f1)->f1).targs;_tmp321=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B5.f2)->f1).datatype_info).KnownDatatype).val;_tmp320=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2B5.f2)->f1).targs;_LL1F:
# 1481
 if(_tmp323 == _tmp321  || Cyc_Absyn_qvar_cmp(_tmp323->name,_tmp321->name)== 0){
Cyc_Tcutil_unify_list(_tmp320,_tmp322);
return;}
# 1485
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp2B5.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp2B5.f2)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL20: _tmp329=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp328=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp327=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f1)->f1).targs;_tmp326=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp325=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp324=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2B5.f2)->f1).targs;_LL21:
# 1489
 if((_tmp329 == _tmp326  || Cyc_Absyn_qvar_cmp(_tmp329->name,_tmp326->name)== 0) && (
_tmp328 == _tmp325  || Cyc_Absyn_qvar_cmp(_tmp328->name,_tmp325->name)== 0)){
Cyc_Tcutil_unify_list(_tmp324,_tmp327);
return;}
# 1494
({struct _dyneither_ptr _tmpBEE=({const char*_tmp2CC="(different datatype field types)";_tag_dyneither(_tmp2CC,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpBEE;});
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp2B5.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 5U: switch(*((int*)_tmp2B5.f2)){case 5U: _LL22: _tmp335=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).elt_typ;_tmp334=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).elt_tq;_tmp333=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).rgn;_tmp332=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).nullable;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).bounds;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).zero_term;_tmp32F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).elt_typ;_tmp32E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).elt_tq;_tmp32D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).rgn;_tmp32C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).nullable;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).bounds;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).zero_term;_LL23:
# 1499
 Cyc_Tcutil_unify_it(_tmp32F,_tmp335);
Cyc_Tcutil_unify_it(_tmp333,_tmp32D);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpBF0=_tmp32A;union Cyc_Absyn_Constraint*_tmpBEF=_tmp330;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBF0,_tmpBEF,({const char*_tmp2CD="(not both zero terminated)";_tag_dyneither(_tmp2CD,sizeof(char),27U);}));});
Cyc_Tcutil_unify_tqual(_tmp32E,_tmp32F,_tmp334,_tmp335);
({union Cyc_Absyn_Constraint*_tmpBF2=_tmp32B;union Cyc_Absyn_Constraint*_tmpBF1=_tmp331;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmpBF2,_tmpBF1,({const char*_tmp2CE="(different pointer bounds)";_tag_dyneither(_tmp2CE,sizeof(char),27U);}));});
# 1507
{void*_tmp2CF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32B);void*_tmp2D0=_tmp2CF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D0)->tag == 0U){_LL55: _LL56:
 return;}else{_LL57: _LL58:
 goto _LL54;}_LL54:;}
# 1511
({union Cyc_Absyn_Constraint*_tmpBF4=_tmp32C;union Cyc_Absyn_Constraint*_tmpBF3=_tmp332;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBF4,_tmpBF3,({const char*_tmp2D1="(incompatible pointer types)";_tag_dyneither(_tmp2D1,sizeof(char),29U);}));});
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 6U: switch(*((int*)_tmp2B5.f2)){case 6U: _LL24: _tmp339=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp338=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f1)->f2;_tmp337=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_tmp336=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f2)->f2;_LL25:
# 1516
 if(_tmp337 == _tmp339  && ((_tmp336 == _tmp338  || _tmp336 == Cyc_Absyn_Int_sz  && _tmp338 == Cyc_Absyn_Long_sz) || 
_tmp336 == Cyc_Absyn_Long_sz  && _tmp338 == Cyc_Absyn_Int_sz))return;
({struct _dyneither_ptr _tmpBF5=({const char*_tmp2D2="(different integral types)";_tag_dyneither(_tmp2D2,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpBF5;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 7U: switch(*((int*)_tmp2B5.f2)){case 7U: _LL26: _tmp33B=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp33A=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL27:
# 1522
 if(_tmp33A == 0  && _tmp33B == 0)return;else{
if(_tmp33A == 1  && _tmp33B == 1)return;else{
# 1525
if(((_tmp33A != 0  && _tmp33A != 1) && _tmp33B != 0) && _tmp33B != 1)return;}}
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 19U: switch(*((int*)_tmp2B5.f2)){case 19U: _LL28: _tmp33D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp33C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL29:
# 1528
 Cyc_Tcutil_unify_it(_tmp33D,_tmp33C);return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 18U: switch(*((int*)_tmp2B5.f2)){case 18U: _LL2A: _tmp33F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp33E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL2B:
# 1531
 if(!Cyc_Evexp_same_const_exp(_tmp33F,_tmp33E)){
({struct _dyneither_ptr _tmpBF6=({const char*_tmp2D3="(cannot prove expressions are the same)";_tag_dyneither(_tmp2D3,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpBF6;});
goto _LL11;}
# 1535
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 8U: switch(*((int*)_tmp2B5.f2)){case 8U: _LL2C: _tmp347=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).elt_type;_tmp346=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).tq;_tmp345=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).num_elts;_tmp344=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).zero_term;_tmp343=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).elt_type;_tmp342=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).tq;_tmp341=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).num_elts;_tmp340=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).zero_term;_LL2D:
# 1539
 Cyc_Tcutil_unify_it(_tmp343,_tmp347);
Cyc_Tcutil_unify_tqual(_tmp342,_tmp343,_tmp346,_tmp347);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpBF8=_tmp344;union Cyc_Absyn_Constraint*_tmpBF7=_tmp340;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBF8,_tmpBF7,({const char*_tmp2D4="(not both zero terminated)";_tag_dyneither(_tmp2D4,sizeof(char),27U);}));});
if(_tmp345 == _tmp341)return;
if(_tmp345 == 0  || _tmp341 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp345,_tmp341))
return;
({struct _dyneither_ptr _tmpBF9=({const char*_tmp2D5="(different array sizes)";_tag_dyneither(_tmp2D5,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpBF9;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 9U: switch(*((int*)_tmp2B5.f2)){case 9U: _LL2E: _tmp361=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).tvars;_tmp360=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).effect;_tmp35F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ret_tqual;_tmp35E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ret_typ;_tmp35D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).args;_tmp35C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).c_varargs;_tmp35B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).cyc_varargs;_tmp35A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).rgn_po;_tmp359=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).attributes;_tmp358=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).requires_clause;_tmp357=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).requires_relns;_tmp356=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ensures_clause;_tmp355=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ensures_relns;_tmp354=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).tvars;_tmp353=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).effect;_tmp352=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ret_tqual;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ret_typ;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).args;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).c_varargs;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).cyc_varargs;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).rgn_po;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).attributes;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).requires_clause;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).requires_relns;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ensures_clause;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ensures_relns;_LL2F: {
# 1553
int done=0;
{struct _RegionHandle _tmp2D6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2D6;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp354 != 0){
if(_tmp361 == 0){
({struct _dyneither_ptr _tmpBFA=({const char*_tmp2D7="(second function type has too few type variables)";_tag_dyneither(_tmp2D7,sizeof(char),50U);});Cyc_Tcutil_failure_reason=_tmpBFA;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1561
void*_tmp2D8=((struct Cyc_Absyn_Tvar*)_tmp354->hd)->kind;
void*_tmp2D9=((struct Cyc_Absyn_Tvar*)_tmp361->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2D8,_tmp2D9)){
({struct _dyneither_ptr _tmpBFF=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DC=({struct Cyc_String_pa_PrintArg_struct _tmpA53;_tmpA53.tag=0U,({
struct _dyneither_ptr _tmpBFB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp354->hd));_tmpA53.f1=_tmpBFB;});_tmpA53;});struct Cyc_String_pa_PrintArg_struct _tmp2DD=({struct Cyc_String_pa_PrintArg_struct _tmpA52;_tmpA52.tag=0U,({
struct _dyneither_ptr _tmpBFC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp354->hd,& Cyc_Tcutil_bk)));_tmpA52.f1=_tmpBFC;});_tmpA52;});struct Cyc_String_pa_PrintArg_struct _tmp2DE=({struct Cyc_String_pa_PrintArg_struct _tmpA51;_tmpA51.tag=0U,({
struct _dyneither_ptr _tmpBFD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp361->hd,& Cyc_Tcutil_bk)));_tmpA51.f1=_tmpBFD;});_tmpA51;});void*_tmp2DA[3U];_tmp2DA[0]=& _tmp2DC,_tmp2DA[1]=& _tmp2DD,_tmp2DA[2]=& _tmp2DE;({struct _dyneither_ptr _tmpBFE=({const char*_tmp2DB="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2DB,sizeof(char),44U);});Cyc_aprintf(_tmpBFE,_tag_dyneither(_tmp2DA,sizeof(void*),3U));});});
# 1564
Cyc_Tcutil_failure_reason=_tmpBFF;});
# 1568
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1570
({struct Cyc_List_List*_tmpC02=({struct Cyc_List_List*_tmp2E1=_region_malloc(rgn,sizeof(*_tmp2E1));({struct _tuple16*_tmpC01=({struct _tuple16*_tmp2E0=_region_malloc(rgn,sizeof(*_tmp2E0));_tmp2E0->f1=(struct Cyc_Absyn_Tvar*)_tmp361->hd,({void*_tmpC00=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));_tmp2DF->tag=2U,_tmp2DF->f1=(struct Cyc_Absyn_Tvar*)_tmp354->hd;_tmp2DF;});_tmp2E0->f2=_tmpC00;});_tmp2E0;});_tmp2E1->hd=_tmpC01;}),_tmp2E1->tl=inst;_tmp2E1;});inst=_tmpC02;});
_tmp354=_tmp354->tl;
_tmp361=_tmp361->tl;};}
# 1574
if(_tmp361 != 0){
({struct _dyneither_ptr _tmpC03=({const char*_tmp2E2="(second function type has too many type variables)";_tag_dyneither(_tmp2E2,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpC03;});
_npop_handler(0U);goto _LL11;}
# 1578
if(inst != 0){
({void*_tmpC06=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3->tag=9U,(_tmp2E3->f1).tvars=0,(_tmp2E3->f1).effect=_tmp353,(_tmp2E3->f1).ret_tqual=_tmp352,(_tmp2E3->f1).ret_typ=_tmp351,(_tmp2E3->f1).args=_tmp350,(_tmp2E3->f1).c_varargs=_tmp34F,(_tmp2E3->f1).cyc_varargs=_tmp34E,(_tmp2E3->f1).rgn_po=_tmp34D,(_tmp2E3->f1).attributes=_tmp34C,(_tmp2E3->f1).requires_clause=_tmp358,(_tmp2E3->f1).requires_relns=_tmp357,(_tmp2E3->f1).ensures_clause=_tmp356,(_tmp2E3->f1).ensures_relns=_tmp355;_tmp2E3;});Cyc_Tcutil_unify_it(_tmpC06,({
# 1582
struct _RegionHandle*_tmpC05=rgn;struct Cyc_List_List*_tmpC04=inst;Cyc_Tcutil_rsubstitute(_tmpC05,_tmpC04,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E4=_cycalloc(sizeof(*_tmp2E4));
_tmp2E4->tag=9U,(_tmp2E4->f1).tvars=0,(_tmp2E4->f1).effect=_tmp360,(_tmp2E4->f1).ret_tqual=_tmp35F,(_tmp2E4->f1).ret_typ=_tmp35E,(_tmp2E4->f1).args=_tmp35D,(_tmp2E4->f1).c_varargs=_tmp35C,(_tmp2E4->f1).cyc_varargs=_tmp35B,(_tmp2E4->f1).rgn_po=_tmp35A,(_tmp2E4->f1).attributes=_tmp359,(_tmp2E4->f1).requires_clause=_tmp34B,(_tmp2E4->f1).requires_relns=_tmp34A,(_tmp2E4->f1).ensures_clause=_tmp349,(_tmp2E4->f1).ensures_relns=_tmp348;_tmp2E4;}));}));});
# 1587
done=1;}}
# 1555
;_pop_region(rgn);}
# 1590
if(done)
return;
Cyc_Tcutil_unify_it(_tmp351,_tmp35E);
Cyc_Tcutil_unify_tqual(_tmp352,_tmp351,_tmp35F,_tmp35E);
for(0;_tmp350 != 0  && _tmp35D != 0;(_tmp350=_tmp350->tl,_tmp35D=_tmp35D->tl)){
struct _tuple10 _tmp2E5=*((struct _tuple10*)_tmp350->hd);struct _tuple10 _tmp2E6=_tmp2E5;struct Cyc_Absyn_Tqual _tmp2EC;void*_tmp2EB;_LL5A: _tmp2EC=_tmp2E6.f2;_tmp2EB=_tmp2E6.f3;_LL5B:;{
struct _tuple10 _tmp2E7=*((struct _tuple10*)_tmp35D->hd);struct _tuple10 _tmp2E8=_tmp2E7;struct Cyc_Absyn_Tqual _tmp2EA;void*_tmp2E9;_LL5D: _tmp2EA=_tmp2E8.f2;_tmp2E9=_tmp2E8.f3;_LL5E:;
Cyc_Tcutil_unify_it(_tmp2EB,_tmp2E9);
Cyc_Tcutil_unify_tqual(_tmp2EC,_tmp2EB,_tmp2EA,_tmp2E9);};}
# 1600
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp350 != 0  || _tmp35D != 0){
({struct _dyneither_ptr _tmpC07=({const char*_tmp2ED="(function types have different number of arguments)";_tag_dyneither(_tmp2ED,sizeof(char),52U);});Cyc_Tcutil_failure_reason=_tmpC07;});
goto _LL11;}
# 1606
if(_tmp34F != _tmp35C){
({struct _dyneither_ptr _tmpC08=({const char*_tmp2EE="(only one function type takes C varargs)";_tag_dyneither(_tmp2EE,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC08;});
goto _LL11;}{
# 1611
int bad_cyc_vararg=0;
{struct _tuple22 _tmp2EF=({struct _tuple22 _tmpA54;_tmpA54.f1=_tmp34E,_tmpA54.f2=_tmp35B;_tmpA54;});struct _tuple22 _tmp2F0=_tmp2EF;struct _dyneither_ptr*_tmp2FA;struct Cyc_Absyn_Tqual _tmp2F9;void*_tmp2F8;int _tmp2F7;struct _dyneither_ptr*_tmp2F6;struct Cyc_Absyn_Tqual _tmp2F5;void*_tmp2F4;int _tmp2F3;if(_tmp2F0.f1 == 0){if(_tmp2F0.f2 == 0){_LL60: _LL61:
 goto _LL5F;}else{_LL62: _LL63:
 goto _LL65;}}else{if(_tmp2F0.f2 == 0){_LL64: _LL65:
# 1616
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC09=({const char*_tmp2F1="(only one function type takes varargs)";_tag_dyneither(_tmp2F1,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC09;});
goto _LL5F;}else{_LL66: _tmp2FA=(_tmp2F0.f1)->name;_tmp2F9=(_tmp2F0.f1)->tq;_tmp2F8=(_tmp2F0.f1)->type;_tmp2F7=(_tmp2F0.f1)->inject;_tmp2F6=(_tmp2F0.f2)->name;_tmp2F5=(_tmp2F0.f2)->tq;_tmp2F4=(_tmp2F0.f2)->type;_tmp2F3=(_tmp2F0.f2)->inject;_LL67:
# 1620
 Cyc_Tcutil_unify_it(_tmp2F8,_tmp2F4);
Cyc_Tcutil_unify_tqual(_tmp2F9,_tmp2F8,_tmp2F5,_tmp2F4);
if(_tmp2F7 != _tmp2F3){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC0A=({const char*_tmp2F2="(only one function type injects varargs)";_tag_dyneither(_tmp2F2,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC0A;});}
# 1626
goto _LL5F;}}_LL5F:;}
# 1628
if(bad_cyc_vararg)goto _LL11;{
# 1631
int bad_effect=0;
{struct _tuple0 _tmp2FB=({struct _tuple0 _tmpA55;_tmpA55.f1=_tmp353,_tmpA55.f2=_tmp360;_tmpA55;});struct _tuple0 _tmp2FC=_tmp2FB;if(_tmp2FC.f1 == 0){if(_tmp2FC.f2 == 0){_LL69: _LL6A:
 goto _LL68;}else{_LL6B: _LL6C:
 goto _LL6E;}}else{if(_tmp2FC.f2 == 0){_LL6D: _LL6E:
 bad_effect=1;goto _LL68;}else{_LL6F: _LL70:
({int _tmpC0C=!({void*_tmpC0B=(void*)_check_null(_tmp353);Cyc_Tcutil_unify_effect(_tmpC0B,(void*)_check_null(_tmp360));});bad_effect=_tmpC0C;});goto _LL68;}}_LL68:;}
# 1638
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpC0D=({const char*_tmp2FD="(function effects do not match)";_tag_dyneither(_tmp2FD,sizeof(char),32U);});Cyc_Tcutil_failure_reason=_tmpC0D;});
goto _LL11;}
# 1644
if(!Cyc_Tcutil_same_atts(_tmp359,_tmp34C)){
({struct _dyneither_ptr _tmpC0E=({const char*_tmp2FE="(function types have different attributes)";_tag_dyneither(_tmp2FE,sizeof(char),43U);});Cyc_Tcutil_failure_reason=_tmpC0E;});
goto _LL11;}
# 1648
if(!Cyc_Tcutil_same_rgn_po(_tmp35A,_tmp34D)){
({struct _dyneither_ptr _tmpC0F=({const char*_tmp2FF="(function types have different region lifetime orderings)";_tag_dyneither(_tmp2FF,sizeof(char),58U);});Cyc_Tcutil_failure_reason=_tmpC0F;});
goto _LL11;}
# 1652
if(!Cyc_Tcutil_check_logical_equivalence(_tmp357,_tmp34A)){
({struct _dyneither_ptr _tmpC10=({const char*_tmp300="(@requires clauses not equivalent)";_tag_dyneither(_tmp300,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC10;});
goto _LL11;}
# 1656
if(!Cyc_Tcutil_check_logical_equivalence(_tmp355,_tmp348)){
({struct _dyneither_ptr _tmpC11=({const char*_tmp301="(@ensures clauses not equivalent)";_tag_dyneither(_tmp301,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC11;});
goto _LL11;}
# 1660
return;};};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 10U: switch(*((int*)_tmp2B5.f2)){case 10U: _LL30: _tmp363=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp362=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL31:
# 1663
 for(0;_tmp362 != 0  && _tmp363 != 0;(_tmp362=_tmp362->tl,_tmp363=_tmp363->tl)){
struct _tuple12 _tmp302=*((struct _tuple12*)_tmp362->hd);struct _tuple12 _tmp303=_tmp302;struct Cyc_Absyn_Tqual _tmp309;void*_tmp308;_LL72: _tmp309=_tmp303.f1;_tmp308=_tmp303.f2;_LL73:;{
struct _tuple12 _tmp304=*((struct _tuple12*)_tmp363->hd);struct _tuple12 _tmp305=_tmp304;struct Cyc_Absyn_Tqual _tmp307;void*_tmp306;_LL75: _tmp307=_tmp305.f1;_tmp306=_tmp305.f2;_LL76:;
Cyc_Tcutil_unify_it(_tmp308,_tmp306);
Cyc_Tcutil_unify_tqual(_tmp309,_tmp308,_tmp307,_tmp306);};}
# 1669
if(_tmp362 == 0  && _tmp363 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC12=({const char*_tmp30A="(tuple types have different numbers of components)";_tag_dyneither(_tmp30A,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpC12;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 12U: switch(*((int*)_tmp2B5.f2)){case 12U: _LL32: _tmp367=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp366=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f1)->f2;_tmp365=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_tmp364=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f2)->f2;_LL33:
# 1676
 if(_tmp365 != _tmp367){({struct _dyneither_ptr _tmpC13=({const char*_tmp30B="(struct and union type)";_tag_dyneither(_tmp30B,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC13;});goto _LL11;}
for(0;_tmp364 != 0  && _tmp366 != 0;(_tmp364=_tmp364->tl,_tmp366=_tmp366->tl)){
struct Cyc_Absyn_Aggrfield*_tmp30C=(struct Cyc_Absyn_Aggrfield*)_tmp364->hd;
struct Cyc_Absyn_Aggrfield*_tmp30D=(struct Cyc_Absyn_Aggrfield*)_tmp366->hd;
if(Cyc_strptrcmp(_tmp30C->name,_tmp30D->name)!= 0){
({struct _dyneither_ptr _tmpC14=({const char*_tmp30E="(different member names)";_tag_dyneither(_tmp30E,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC14;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1684
Cyc_Tcutil_unify_it(_tmp30C->type,_tmp30D->type);
Cyc_Tcutil_unify_tqual(_tmp30C->tq,_tmp30C->type,_tmp30D->tq,_tmp30D->type);
if(!Cyc_Tcutil_same_atts(_tmp30C->attributes,_tmp30D->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC15=({const char*_tmp30F="(different attributes on member)";_tag_dyneither(_tmp30F,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpC15;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1692
if((_tmp30C->width != 0  && _tmp30D->width == 0  || 
_tmp30D->width != 0  && _tmp30C->width == 0) || 
(_tmp30C->width != 0  && _tmp30D->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpC16=(struct Cyc_Absyn_Exp*)_check_null(_tmp30C->width);Cyc_Evexp_same_const_exp(_tmpC16,(struct Cyc_Absyn_Exp*)_check_null(_tmp30D->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC17=({const char*_tmp310="(different bitfield widths on member)";_tag_dyneither(_tmp310,sizeof(char),38U);});Cyc_Tcutil_failure_reason=_tmpC17;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1701
if((_tmp30C->requires_clause != 0  && _tmp30D->requires_clause == 0  || 
_tmp30C->requires_clause == 0  && _tmp30D->requires_clause != 0) || 
(_tmp30C->requires_clause == 0  && _tmp30D->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpC18=(struct Cyc_Absyn_Exp*)_check_null(_tmp30C->requires_clause);Cyc_Evexp_same_const_exp(_tmpC18,(struct Cyc_Absyn_Exp*)_check_null(_tmp30D->requires_clause));})){
# 1706
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC19=({const char*_tmp311="(different @requires clauses on member)";_tag_dyneither(_tmp311,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpC19;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1712
if(_tmp364 == 0  && _tmp366 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC1A=({const char*_tmp312="(different number of members)";_tag_dyneither(_tmp312,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpC1A;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 17U: switch(*((int*)_tmp2B5.f2)){case 17U: _LL34: _tmp36B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f1)->f2;_tmp36A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f1)->f3;_tmp369=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f2)->f2;_tmp368=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f2)->f3;_LL35:
# 1718
 if(_tmp36A != _tmp368){
({struct _dyneither_ptr _tmpC1B=({const char*_tmp313="(different abstract typedefs)";_tag_dyneither(_tmp313,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpC1B;});
goto _LL11;}
# 1722
({struct _dyneither_ptr _tmpC1C=({const char*_tmp314="(type parameters to typedef differ)";_tag_dyneither(_tmp314,sizeof(char),36U);});Cyc_Tcutil_failure_reason=_tmpC1C;});
Cyc_Tcutil_unify_list(_tmp36B,_tmp369);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 20U: switch(*((int*)_tmp2B5.f2)){case 20U: _LL36: _LL37:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 21U: switch(*((int*)_tmp2B5.f2)){case 21U: _LL38: _LL39:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 22U: switch(*((int*)_tmp2B5.f2)){case 22U: _LL3A: _LL3B:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 15U: switch(*((int*)_tmp2B5.f2)){case 15U: _LL3C: _tmp36D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp36C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL3D:
# 1729
 Cyc_Tcutil_unify_it(_tmp36D,_tmp36C);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 16U: switch(*((int*)_tmp2B5.f2)){case 16U: _LL3E: _tmp371=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp370=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B5.f1)->f2;_tmp36F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_tmp36E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2B5.f2)->f2;_LL3F:
# 1732
 Cyc_Tcutil_unify_it(_tmp371,_tmp36F);
Cyc_Tcutil_unify_it(_tmp370,_tmp36E);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 24U: _LL40: _LL41:
 goto _LL43;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2B5.f2)->tag == 24U){_LL42: _LL43:
 goto _LL45;}else{switch(*((int*)_tmp2B5.f1)){case 23U: _LL44: _LL45:
 goto _LL47;case 25U: _LL46: _LL47:
 goto _LL49;default: switch(*((int*)_tmp2B5.f2)){case 25U: _LL48: _LL49:
 goto _LL4B;case 23U: _LL4A: _LL4B:
# 1741
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
({struct _dyneither_ptr _tmpC1D=({const char*_tmp315="(effects don't unify)";_tag_dyneither(_tmp315,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpC1D;});
goto _LL11;default: _LL4C: _LL4D:
 goto _LL11;}}}}_LL11:;}
# 1746
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1749
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpC1F)(void*,void*)=cmp;void*_tmpC1E=(void*)_check_null(a1);_tmpC1F(_tmpC1E,(void*)_check_null(a2));});}
# 1756
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp372=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp373=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp372,_tmp373);}
# 1763
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
({union Cyc_Absyn_Constraint*_tmpC20=Cyc_Absyn_compress_conref(x);x=_tmpC20;});
({union Cyc_Absyn_Constraint*_tmpC21=Cyc_Absyn_compress_conref(y);y=_tmpC21;});
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp374=x;void*_tmp37B;switch((((union Cyc_Absyn_Constraint*)_tmp374)->Eq_constr).tag){case 3U: _LL1: _LL2:
 return - 1;case 1U: _LL3: _tmp37B=(void*)(_tmp374->Eq_constr).val;_LL4: {
# 1770
union Cyc_Absyn_Constraint*_tmp375=y;void*_tmp378;switch((((union Cyc_Absyn_Constraint*)_tmp375)->Eq_constr).tag){case 3U: _LL8: _LL9:
 return 1;case 1U: _LLA: _tmp378=(void*)(_tmp375->Eq_constr).val;_LLB:
 return cmp(_tmp37B,_tmp378);default: _LLC: _LLD:
({void*_tmp376=0U;({struct _dyneither_ptr _tmpC22=({const char*_tmp377="unify_conref: forward after compress(2)";_tag_dyneither(_tmp377,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpC22,_tag_dyneither(_tmp376,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 1775
({void*_tmp379=0U;({struct _dyneither_ptr _tmpC23=({const char*_tmp37A="unify_conref: forward after compress";_tag_dyneither(_tmp37A,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpC23,_tag_dyneither(_tmp379,sizeof(void*),0U));});});}_LL0:;};}
# 1779
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp37C=tqt1;struct Cyc_Absyn_Tqual _tmp382;void*_tmp381;_LL1: _tmp382=_tmp37C->f1;_tmp381=_tmp37C->f2;_LL2:;{
struct _tuple12*_tmp37D=tqt2;struct Cyc_Absyn_Tqual _tmp380;void*_tmp37F;_LL4: _tmp380=_tmp37D->f1;_tmp37F=_tmp37D->f2;_LL5:;{
int _tmp37E=Cyc_Tcutil_tqual_cmp(_tmp382,_tmp380);
if(_tmp37E != 0)return _tmp37E;
return Cyc_Tcutil_typecmp(_tmp381,_tmp37F);};};}
# 1787
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp383=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp383 != 0)return _tmp383;{
int _tmp384=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp384 != 0)return _tmp384;{
int _tmp385=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp385 != 0)return _tmp385;{
int _tmp386=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp386 != 0)return _tmp386;
({int _tmpC24=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp386=_tmpC24;});
if(_tmp386 != 0)return _tmp386;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1801
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp387=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp387 != 0)return _tmp387;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1807
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp388=t;switch(*((int*)_tmp388)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp388)->f1 == 0){_LLF: _LL10:
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
# 1844
int Cyc_Tcutil_typecmp(void*t1,void*t2){
({void*_tmpC25=Cyc_Tcutil_compress(t1);t1=_tmpC25;});
({void*_tmpC26=Cyc_Tcutil_compress(t2);t2=_tmpC26;});
if(t1 == t2)return 0;{
int _tmp389=({int _tmpC27=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpC27,Cyc_Tcutil_type_case_number(t2));});
if(_tmp389 != 0)
return _tmp389;{
# 1853
struct _tuple0 _tmp38A=({struct _tuple0 _tmpA57;_tmpA57.f1=t2,_tmpA57.f2=t1;_tmpA57;});struct _tuple0 _tmp38B=_tmp38A;struct _dyneither_ptr _tmp400;struct _dyneither_ptr _tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;void*_tmp3FA;void*_tmp3F9;void*_tmp3F8;void*_tmp3F7;void*_tmp3F6;void*_tmp3F5;void*_tmp3F4;void*_tmp3F3;enum Cyc_Absyn_AggrKind _tmp3F2;struct Cyc_List_List*_tmp3F1;enum Cyc_Absyn_AggrKind _tmp3F0;struct Cyc_List_List*_tmp3EF;struct Cyc_List_List*_tmp3EE;struct Cyc_List_List*_tmp3ED;struct Cyc_Absyn_FnInfo _tmp3EC;struct Cyc_Absyn_FnInfo _tmp3EB;void*_tmp3EA;struct Cyc_Absyn_Tqual _tmp3E9;struct Cyc_Absyn_Exp*_tmp3E8;union Cyc_Absyn_Constraint*_tmp3E7;void*_tmp3E6;struct Cyc_Absyn_Tqual _tmp3E5;struct Cyc_Absyn_Exp*_tmp3E4;union Cyc_Absyn_Constraint*_tmp3E3;int _tmp3E2;int _tmp3E1;enum Cyc_Absyn_Sign _tmp3E0;enum Cyc_Absyn_Size_of _tmp3DF;enum Cyc_Absyn_Sign _tmp3DE;enum Cyc_Absyn_Size_of _tmp3DD;void*_tmp3DC;struct Cyc_Absyn_Tqual _tmp3DB;void*_tmp3DA;union Cyc_Absyn_Constraint*_tmp3D9;union Cyc_Absyn_Constraint*_tmp3D8;union Cyc_Absyn_Constraint*_tmp3D7;void*_tmp3D6;struct Cyc_Absyn_Tqual _tmp3D5;void*_tmp3D4;union Cyc_Absyn_Constraint*_tmp3D3;union Cyc_Absyn_Constraint*_tmp3D2;union Cyc_Absyn_Constraint*_tmp3D1;struct Cyc_Absyn_Datatypedecl*_tmp3D0;struct Cyc_Absyn_Datatypefield*_tmp3CF;struct Cyc_List_List*_tmp3CE;struct Cyc_Absyn_Datatypedecl*_tmp3CD;struct Cyc_Absyn_Datatypefield*_tmp3CC;struct Cyc_List_List*_tmp3CB;struct Cyc_Absyn_Datatypedecl*_tmp3CA;struct Cyc_List_List*_tmp3C9;struct Cyc_Absyn_Datatypedecl*_tmp3C8;struct Cyc_List_List*_tmp3C7;struct Cyc_List_List*_tmp3C6;struct Cyc_List_List*_tmp3C5;struct _tuple2*_tmp3C4;struct _tuple2*_tmp3C3;union Cyc_Absyn_AggrInfoU _tmp3C2;struct Cyc_List_List*_tmp3C1;union Cyc_Absyn_AggrInfoU _tmp3C0;struct Cyc_List_List*_tmp3BF;struct Cyc_Absyn_Tvar*_tmp3BE;struct Cyc_Absyn_Tvar*_tmp3BD;switch(*((int*)_tmp38B.f1)){case 1U: switch(*((int*)_tmp38B.f2)){case 1U: _LL1: _LL2:
# 1855
({void*_tmp38C=0U;({struct _dyneither_ptr _tmpC28=({const char*_tmp38D="typecmp: can only compare closed types";_tag_dyneither(_tmp38D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC28,_tag_dyneither(_tmp38C,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 2U: switch(*((int*)_tmp38B.f2)){case 2U: _LL3: _tmp3BE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3BD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL4:
# 1859
 return Cyc_Core_intcmp(_tmp3BD->identity,_tmp3BE->identity);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 11U: switch(*((int*)_tmp38B.f2)){case 11U: _LL5: _tmp3C2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f1)->f1).aggr_info;_tmp3C1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f1)->f1).targs;_tmp3C0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f2)->f1).aggr_info;_tmp3BF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f2)->f1).targs;_LL6: {
# 1863
struct _tuple13 _tmp38E=Cyc_Absyn_aggr_kinded_name(_tmp3C2);struct _tuple13 _tmp38F=_tmp38E;struct _tuple2*_tmp394;_LL38: _tmp394=_tmp38F.f2;_LL39:;{
struct _tuple13 _tmp390=Cyc_Absyn_aggr_kinded_name(_tmp3C0);struct _tuple13 _tmp391=_tmp390;struct _tuple2*_tmp393;_LL3B: _tmp393=_tmp391.f2;_LL3C:;{
int _tmp392=Cyc_Absyn_qvar_cmp(_tmp394,_tmp393);
if(_tmp392 != 0)return _tmp392;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3C1,_tmp3BF);}};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 13U: switch(*((int*)_tmp38B.f2)){case 13U: _LL7: _tmp3C4=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3C3=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL8:
# 1870
 return Cyc_Absyn_qvar_cmp(_tmp3C4,_tmp3C3);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 14U: switch(*((int*)_tmp38B.f2)){case 14U: _LL9: _tmp3C6=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3C5=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LLA:
# 1873
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3C6,_tmp3C5);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp38B.f2)){case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLB: _tmp3CA=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f1)->f1).datatype_info).KnownDatatype).val;_tmp3C9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f1)->f1).targs;_tmp3C8=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f2)->f1).datatype_info).KnownDatatype).val;_tmp3C7=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f2)->f1).targs;_LLC:
# 1877
 if(_tmp3C8 == _tmp3CA)return 0;{
int _tmp395=Cyc_Absyn_qvar_cmp(_tmp3C8->name,_tmp3CA->name);
if(_tmp395 != 0)return _tmp395;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3C7,_tmp3C9);};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp38B.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp38B.f2)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LLD: _tmp3D0=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp3CF=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp3CE=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).targs;_tmp3CD=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp3CC=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp3CB=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).targs;_LLE:
# 1884
 if(_tmp3CD == _tmp3D0)return 0;{
int _tmp396=Cyc_Absyn_qvar_cmp(_tmp3D0->name,_tmp3CD->name);
if(_tmp396 != 0)return _tmp396;{
int _tmp397=Cyc_Absyn_qvar_cmp(_tmp3CF->name,_tmp3CC->name);
if(_tmp397 != 0)return _tmp397;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3CB,_tmp3CE);};};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp38B.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 5U: switch(*((int*)_tmp38B.f2)){case 5U: _LLF: _tmp3DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).elt_typ;_tmp3DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).elt_tq;_tmp3DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).rgn;_tmp3D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).nullable;_tmp3D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).bounds;_tmp3D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).zero_term;_tmp3D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).elt_typ;_tmp3D5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).elt_tq;_tmp3D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).rgn;_tmp3D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).nullable;_tmp3D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).bounds;_tmp3D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).zero_term;_LL10: {
# 1893
int _tmp398=Cyc_Tcutil_typecmp(_tmp3D6,_tmp3DC);
if(_tmp398 != 0)return _tmp398;{
int _tmp399=Cyc_Tcutil_typecmp(_tmp3D4,_tmp3DA);
if(_tmp399 != 0)return _tmp399;{
int _tmp39A=Cyc_Tcutil_tqual_cmp(_tmp3D5,_tmp3DB);
if(_tmp39A != 0)return _tmp39A;{
int _tmp39B=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp3D2,_tmp3D8);
if(_tmp39B != 0)return _tmp39B;{
int _tmp39C=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3D1,_tmp3D7);
if(_tmp39C != 0)return _tmp39C;
{void*_tmp39D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3D2);void*_tmp39E=_tmp39D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp39E)->tag == 0U){_LL3E: _LL3F:
 return 0;}else{_LL40: _LL41:
 goto _LL3D;}_LL3D:;}
# 1907
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3D3,_tmp3D9);};};};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 6U: switch(*((int*)_tmp38B.f2)){case 6U: _LL11: _tmp3E0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3DF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f1)->f2;_tmp3DE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f2)->f1;_tmp3DD=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f2)->f2;_LL12:
# 1910
 if(_tmp3DE != _tmp3E0)return Cyc_Core_intcmp((int)((unsigned int)_tmp3DE),(int)((unsigned int)_tmp3E0));
if(_tmp3DD != _tmp3DF)return Cyc_Core_intcmp((int)((unsigned int)_tmp3DD),(int)((unsigned int)_tmp3DF));
return 0;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 7U: switch(*((int*)_tmp38B.f2)){case 7U: _LL13: _tmp3E2=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3E1=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL14:
# 1915
 if(_tmp3E2 == _tmp3E1)return 0;else{
if(_tmp3E1 == 0)return - 1;else{
if(_tmp3E1 == 1  && _tmp3E2 == 0)return - 1;else{
return 1;}}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 8U: switch(*((int*)_tmp38B.f2)){case 8U: _LL15: _tmp3EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).elt_type;_tmp3E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).tq;_tmp3E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).num_elts;_tmp3E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).zero_term;_tmp3E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).elt_type;_tmp3E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).tq;_tmp3E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).num_elts;_tmp3E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).zero_term;_LL16: {
# 1922
int _tmp39F=Cyc_Tcutil_tqual_cmp(_tmp3E5,_tmp3E9);
if(_tmp39F != 0)return _tmp39F;{
int _tmp3A0=Cyc_Tcutil_typecmp(_tmp3E6,_tmp3EA);
if(_tmp3A0 != 0)return _tmp3A0;{
int _tmp3A1=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3E7,_tmp3E3);
if(_tmp3A1 != 0)return _tmp3A1;
if(_tmp3E8 == _tmp3E4)return 0;
if(_tmp3E8 == 0  || _tmp3E4 == 0)
({void*_tmp3A2=0U;({struct _dyneither_ptr _tmpC29=({const char*_tmp3A3="missing expression in array index";_tag_dyneither(_tmp3A3,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC29,_tag_dyneither(_tmp3A2,sizeof(void*),0U));});});
# 1932
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3E8,_tmp3E4);};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 9U: switch(*((int*)_tmp38B.f2)){case 9U: _LL17: _tmp3EC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3EB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL18:
# 1935
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp3EC.ret_typ,_tmp3EB.ret_typ);
if(r != 0)return r;
({int _tmpC2A=Cyc_Tcutil_tqual_cmp(_tmp3EC.ret_tqual,_tmp3EB.ret_tqual);r=_tmpC2A;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp3A4=_tmp3EC.args;
struct Cyc_List_List*_tmp3A5=_tmp3EB.args;
for(0;_tmp3A4 != 0  && _tmp3A5 != 0;(_tmp3A4=_tmp3A4->tl,_tmp3A5=_tmp3A5->tl)){
struct _tuple10 _tmp3A6=*((struct _tuple10*)_tmp3A4->hd);struct _tuple10 _tmp3A7=_tmp3A6;struct Cyc_Absyn_Tqual _tmp3AD;void*_tmp3AC;_LL43: _tmp3AD=_tmp3A7.f2;_tmp3AC=_tmp3A7.f3;_LL44:;{
struct _tuple10 _tmp3A8=*((struct _tuple10*)_tmp3A5->hd);struct _tuple10 _tmp3A9=_tmp3A8;struct Cyc_Absyn_Tqual _tmp3AB;void*_tmp3AA;_LL46: _tmp3AB=_tmp3A9.f2;_tmp3AA=_tmp3A9.f3;_LL47:;
({int _tmpC2B=Cyc_Tcutil_tqual_cmp(_tmp3AD,_tmp3AB);r=_tmpC2B;});
if(r != 0)return r;
({int _tmpC2C=Cyc_Tcutil_typecmp(_tmp3AC,_tmp3AA);r=_tmpC2C;});
if(r != 0)return r;};}
# 1950
if(_tmp3A4 != 0)return 1;
if(_tmp3A5 != 0)return - 1;
if(_tmp3EC.c_varargs  && !_tmp3EB.c_varargs)return 1;
if(!_tmp3EC.c_varargs  && _tmp3EB.c_varargs)return - 1;
if(_tmp3EC.cyc_varargs != 0 & _tmp3EB.cyc_varargs == 0)return 1;
if(_tmp3EC.cyc_varargs == 0 & _tmp3EB.cyc_varargs != 0)return - 1;
if(_tmp3EC.cyc_varargs != 0 & _tmp3EB.cyc_varargs != 0){
({int _tmpC2E=({struct Cyc_Absyn_Tqual _tmpC2D=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3EC.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmpC2D,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3EB.cyc_varargs))->tq);});r=_tmpC2E;});
if(r != 0)return r;
({int _tmpC2F=Cyc_Tcutil_typecmp((_tmp3EC.cyc_varargs)->type,(_tmp3EB.cyc_varargs)->type);r=_tmpC2F;});
if(r != 0)return r;
if((_tmp3EC.cyc_varargs)->inject  && !(_tmp3EB.cyc_varargs)->inject)return 1;
if(!(_tmp3EC.cyc_varargs)->inject  && (_tmp3EB.cyc_varargs)->inject)return - 1;}
# 1964
({int _tmpC30=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp3EC.effect,_tmp3EB.effect);r=_tmpC30;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp3AE=_tmp3EC.rgn_po;
struct Cyc_List_List*_tmp3AF=_tmp3EB.rgn_po;
for(0;_tmp3AE != 0  && _tmp3AF != 0;(_tmp3AE=_tmp3AE->tl,_tmp3AF=_tmp3AF->tl)){
struct _tuple0 _tmp3B0=*((struct _tuple0*)_tmp3AE->hd);struct _tuple0 _tmp3B1=_tmp3B0;void*_tmp3B7;void*_tmp3B6;_LL49: _tmp3B7=_tmp3B1.f1;_tmp3B6=_tmp3B1.f2;_LL4A:;{
struct _tuple0 _tmp3B2=*((struct _tuple0*)_tmp3AF->hd);struct _tuple0 _tmp3B3=_tmp3B2;void*_tmp3B5;void*_tmp3B4;_LL4C: _tmp3B5=_tmp3B3.f1;_tmp3B4=_tmp3B3.f2;_LL4D:;
({int _tmpC31=Cyc_Tcutil_typecmp(_tmp3B7,_tmp3B5);r=_tmpC31;});if(r != 0)return r;
({int _tmpC32=Cyc_Tcutil_typecmp(_tmp3B6,_tmp3B4);r=_tmpC32;});if(r != 0)return r;};}
# 1974
if(_tmp3AE != 0)return 1;
if(_tmp3AF != 0)return - 1;
({int _tmpC33=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3EC.requires_clause,_tmp3EB.requires_clause);r=_tmpC33;});
if(r != 0)return r;
({int _tmpC34=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3EC.ensures_clause,_tmp3EB.ensures_clause);r=_tmpC34;});
if(r != 0)return r;
# 1982
({void*_tmp3B8=0U;({struct _dyneither_ptr _tmpC35=({const char*_tmp3B9="typecmp: function type comparison should never get here!";_tag_dyneither(_tmp3B9,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC35,_tag_dyneither(_tmp3B8,sizeof(void*),0U));});});};};};case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 10U: switch(*((int*)_tmp38B.f2)){case 10U: _LL19: _tmp3EE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3ED=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL1A:
# 1985
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3ED,_tmp3EE);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 12U: switch(*((int*)_tmp38B.f2)){case 12U: _LL1B: _tmp3F2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3F1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f1)->f2;_tmp3F0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f2)->f1;_tmp3EF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f2)->f2;_LL1C:
# 1988
 if(_tmp3F0 != _tmp3F2){
if(_tmp3F0 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3EF,_tmp3F1);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 15U: switch(*((int*)_tmp38B.f2)){case 15U: _LL1D: _tmp3F4=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3F3=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL1E:
# 1993
 return Cyc_Tcutil_typecmp(_tmp3F4,_tmp3F3);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 16U: switch(*((int*)_tmp38B.f2)){case 16U: _LL1F: _tmp3F8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3F7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f1)->f2;_tmp3F6=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f2)->f1;_tmp3F5=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f2)->f2;_LL20: {
# 1995
int _tmp3BA=Cyc_Tcutil_typecmp(_tmp3F8,_tmp3F6);
if(_tmp3BA != 0)return _tmp3BA;else{
return Cyc_Tcutil_typecmp(_tmp3F7,_tmp3F5);}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 19U: switch(*((int*)_tmp38B.f2)){case 19U: _LL21: _tmp3FA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3F9=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL22:
 return Cyc_Tcutil_typecmp(_tmp3FA,_tmp3F9);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 18U: switch(*((int*)_tmp38B.f2)){case 18U: _LL23: _tmp3FC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3FB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL24:
 _tmp3FE=_tmp3FC;_tmp3FD=_tmp3FB;goto _LL26;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 27U: switch(*((int*)_tmp38B.f2)){case 27U: _LL25: _tmp3FE=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3FD=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL26:
# 2001
 return Cyc_Evexp_const_exp_cmp(_tmp3FE,_tmp3FD);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 28U: switch(*((int*)_tmp38B.f2)){case 28U: _LL27: _tmp400=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3FF=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL28:
# 2003
 return Cyc_strcmp((struct _dyneither_ptr)_tmp400,(struct _dyneither_ptr)_tmp3FF);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 24U: _LL29: _LL2A:
 goto _LL2C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp38B.f2)->tag == 24U){_LL2B: _LL2C:
 goto _LL2E;}else{switch(*((int*)_tmp38B.f1)){case 23U: _LL2D: _LL2E:
 goto _LL30;case 25U: _LL2F: _LL30:
 goto _LL32;default: switch(*((int*)_tmp38B.f2)){case 25U: _LL31: _LL32:
 goto _LL34;case 23U: _LL33: _LL34:
# 2011
 if(Cyc_Tcutil_unify(t1,t2))return 0;
if((int)t1 < (int)t2)return - 1;
return 1;default: _LL35: _LL36:
({void*_tmp3BB=0U;({struct _dyneither_ptr _tmpC36=({const char*_tmp3BC="Unmatched case in typecmp";_tag_dyneither(_tmp3BC,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC36,_tag_dyneither(_tmp3BB,sizeof(void*),0U));});});}}}}_LL0:;};};}
# 2018
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp401=Cyc_Tcutil_compress(t);void*_tmp402=_tmp401;switch(*((int*)_tmp402)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2030
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
({void*_tmpC37=Cyc_Tcutil_compress(t1);t1=_tmpC37;});
({void*_tmpC38=Cyc_Tcutil_compress(t2);t2=_tmpC38;});{
struct _tuple0 _tmp403=({struct _tuple0 _tmpA58;_tmpA58.f1=t1,_tmpA58.f2=t2;_tmpA58;});struct _tuple0 _tmp404=_tmp403;int _tmp406;int _tmp405;switch(*((int*)_tmp404.f1)){case 7U: switch(*((int*)_tmp404.f2)){case 7U: _LL1: _tmp406=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f1)->f1;_tmp405=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f2)->f1;_LL2:
# 2035
 return _tmp405 == 0  && _tmp406 != 0  || (_tmp405 == 1  && _tmp406 != 0) && _tmp406 != 1;case 6U: _LL3: _LL4:
 goto _LL6;case 19U: _LL5: _LL6:
 return 1;default: goto _LL21;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 return 0;}else{goto _LL9;}}else{_LL9: _LLA:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp404.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f2){case Cyc_Absyn_Int_sz: _LLB: _LLC:
# 2042
 goto _LLE;case Cyc_Absyn_Short_sz: _LL13: _LL14:
# 2047
 goto _LL16;case Cyc_Absyn_Char_sz: _LL19: _LL1A:
# 2050
 goto _LL1C;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f2)->f1 == 0){_LLF: _LL10:
# 2045
 goto _LL12;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp404.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f2){case Cyc_Absyn_Long_sz: _LLD: _LLE:
# 2043
 return 0;case Cyc_Absyn_Short_sz: _LL15: _LL16:
# 2048
 goto _LL18;case Cyc_Absyn_Char_sz: _LL1B: _LL1C:
# 2051
 goto _LL1E;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp404.f2)->f1 == 0){_LL11: _LL12:
# 2046
 goto _LL14;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f2 == Cyc_Absyn_Char_sz){_LL1D: _LL1E:
# 2052
 goto _LL20;}else{goto _LL21;}}else{goto _LL21;}default: goto _LL21;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp404.f2)->f2){case Cyc_Absyn_Short_sz: _LL17: _LL18:
# 2049
 goto _LL1A;case Cyc_Absyn_Char_sz: _LL1F: _LL20:
# 2053
 return 1;default: goto _LL21;}else{goto _LL21;}default: _LL21: _LL22:
# 2055
 return 0;}_LL0:;};}
# 2061
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2064
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
({struct Cyc_Core_Opt*_tmpC39=({struct Cyc_Core_Opt*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->v=t1;_tmp407;});max_arith_type=_tmpC39;});}}}
# 2073
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2077
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp40A=({struct Cyc_String_pa_PrintArg_struct _tmpA5A;_tmpA5A.tag=0U,({
struct _dyneither_ptr _tmpC3A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA5A.f1=_tmpC3A;});_tmpA5A;});struct Cyc_String_pa_PrintArg_struct _tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmpA59;_tmpA59.tag=0U,({struct _dyneither_ptr _tmpC3B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmpA59.f1=_tmpC3B;});_tmpA59;});void*_tmp408[2U];_tmp408[0]=& _tmp40A,_tmp408[1]=& _tmp40B;({unsigned int _tmpC3D=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpC3C=({const char*_tmp409="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp409,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpC3D,_tmpC3C,_tag_dyneither(_tmp408,sizeof(void*),2U));});});
return 0;}}}
# 2083
return 1;}
# 2088
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp40C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp40D=_tmp40C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40D)->tag == 5U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2094
return 1;}
# 2097
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp40E=Cyc_Tcutil_compress(t);void*_tmp40F=_tmp40E;switch(*((int*)_tmp40F)){case 6U: _LL1: _LL2:
 goto _LL4;case 19U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2108
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2112
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp410=0U;({unsigned int _tmpC3F=e->loc;struct _dyneither_ptr _tmpC3E=({const char*_tmp411="integral size mismatch; conversion supplied";_tag_dyneither(_tmp411,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpC3F,_tmpC3E,_tag_dyneither(_tmp410,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2118
return 0;}
# 2122
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2126
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp412=0U;({unsigned int _tmpC41=e->loc;struct _dyneither_ptr _tmpC40=({const char*_tmp413="integral size mismatch; conversion supplied";_tag_dyneither(_tmp413,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpC41,_tmpC40,_tag_dyneither(_tmp412,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2132
return 0;}
# 2137
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2145
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2147
({void*_tmpC42=Cyc_Tcutil_compress(t1);t1=_tmpC42;});
({void*_tmpC43=Cyc_Tcutil_compress(t2);t2=_tmpC43;});{
struct _tuple0 _tmp414=({struct _tuple0 _tmpA5E;_tmpA5E.f1=t1,_tmpA5E.f2=t2;_tmpA5E;});struct _tuple0 _tmp415=_tmp414;void*_tmp42B;struct Cyc_Absyn_Tqual _tmp42A;struct Cyc_Absyn_Exp*_tmp429;union Cyc_Absyn_Constraint*_tmp428;void*_tmp427;struct Cyc_Absyn_Tqual _tmp426;struct Cyc_Absyn_Exp*_tmp425;union Cyc_Absyn_Constraint*_tmp424;struct Cyc_Absyn_PtrInfo _tmp423;struct Cyc_Absyn_PtrInfo _tmp422;switch(*((int*)_tmp415.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415.f2)->tag == 5U){_LL1: _tmp423=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415.f1)->f1;_tmp422=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp415.f2)->f1;_LL2: {
# 2151
int okay=1;
# 2153
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp423.ptr_atts).nullable,(_tmp422.ptr_atts).nullable))
# 2156
({int _tmpC44=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp423.ptr_atts).nullable);okay=_tmpC44;});
# 2158
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp423.ptr_atts).bounds,(_tmp422.ptr_atts).bounds)){
# 2161
struct _tuple0 _tmp416=({struct _tuple0 _tmpA5B;({void*_tmpC46=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp423.ptr_atts).bounds);_tmpA5B.f1=_tmpC46;}),({
void*_tmpC45=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp422.ptr_atts).bounds);_tmpA5B.f2=_tmpC45;});_tmpA5B;});
# 2161
struct _tuple0 _tmp417=_tmp416;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_Absyn_Exp*_tmp41A;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp417.f1)->tag == 1U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp417.f2)->tag == 0U){_LLA: _LLB:
# 2163
 goto _LLD;}else{_LLE: _tmp41B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp417.f1)->f1;_tmp41A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp417.f2)->f1;_LLF:
# 2166
({int _tmpC47=okay  && Cyc_Evexp_lte_const_exp(_tmp41A,_tmp41B);okay=_tmpC47;});
# 2170
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp422.ptr_atts).zero_term))
({void*_tmp418=0U;({unsigned int _tmpC49=loc;struct _dyneither_ptr _tmpC48=({const char*_tmp419="implicit cast to shorter array";_tag_dyneither(_tmp419,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpC49,_tmpC48,_tag_dyneither(_tmp418,sizeof(void*),0U));});});
goto _LL9;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp417.f2)->tag == 0U){_LLC: _LLD:
# 2164
 okay=1;goto _LL9;}else{_LL10: _tmp41C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp417.f2)->f1;_LL11:
# 2175
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp423.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp422.ptr_atts).bounds))
goto _LL9;
okay=0;
goto _LL9;}}_LL9:;}
# 2184
okay=okay  && (!(_tmp423.elt_tq).real_const  || (_tmp422.elt_tq).real_const);
# 2187
if(!Cyc_Tcutil_unify((_tmp423.ptr_atts).rgn,(_tmp422.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp423.ptr_atts).rgn,(_tmp422.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp41F=({struct Cyc_String_pa_PrintArg_struct _tmpA5D;_tmpA5D.tag=0U,({
struct _dyneither_ptr _tmpC4A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp423.ptr_atts).rgn));_tmpA5D.f1=_tmpC4A;});_tmpA5D;});struct Cyc_String_pa_PrintArg_struct _tmp420=({struct Cyc_String_pa_PrintArg_struct _tmpA5C;_tmpA5C.tag=0U,({
struct _dyneither_ptr _tmpC4B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp422.ptr_atts).rgn));_tmpA5C.f1=_tmpC4B;});_tmpA5C;});void*_tmp41D[2U];_tmp41D[0]=& _tmp41F,_tmp41D[1]=& _tmp420;({unsigned int _tmpC4D=loc;struct _dyneither_ptr _tmpC4C=({const char*_tmp41E="implicit cast from region %s to region %s";_tag_dyneither(_tmp41E,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpC4D,_tmpC4C,_tag_dyneither(_tmp41D,sizeof(void*),2U));});});}else{
okay=0;}}
# 2197
({int _tmpC4E=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp423.ptr_atts).zero_term,(_tmp422.ptr_atts).zero_term) || 
# 2200
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp423.ptr_atts).zero_term) && (_tmp422.elt_tq).real_const);
# 2197
okay=_tmpC4E;});{
# 2208
int _tmp421=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp422.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2211
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp422.ptr_atts).zero_term);
# 2215
({int _tmpC4F=okay  && (Cyc_Tcutil_unify(_tmp423.elt_typ,_tmp422.elt_typ) || 
(_tmp421  && ((_tmp422.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp422.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp423.elt_typ,_tmp422.elt_typ));
# 2215
okay=_tmpC4F;});
# 2218
return okay;};}}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f2)->tag == 8U){_LL3: _tmp42B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f1)->f1).elt_type;_tmp42A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f1)->f1).tq;_tmp429=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f1)->f1).num_elts;_tmp428=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f1)->f1).zero_term;_tmp427=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f2)->f1).elt_type;_tmp426=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f2)->f1).tq;_tmp425=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f2)->f1).num_elts;_tmp424=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp415.f2)->f1).zero_term;_LL4: {
# 2222
int okay;
# 2225
({int _tmpC50=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp428,_tmp424) && (
(_tmp429 != 0  && _tmp425 != 0) && Cyc_Evexp_same_const_exp(_tmp429,_tmp425));
# 2225
okay=_tmpC50;});
# 2228
return(okay  && Cyc_Tcutil_unify(_tmp42B,_tmp427)) && (!_tmp42A.real_const  || _tmp426.real_const);}}else{goto _LL7;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp415.f2)->tag == 6U){_LL5: _LL6:
# 2230
 return 0;}else{goto _LL7;}default: _LL7: _LL8:
# 2232
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2236
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp42C=Cyc_Tcutil_compress(t);void*_tmp42D=_tmp42C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2243
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp42E=Cyc_Tcutil_compress(t);void*_tmp42F=_tmp42E;union Cyc_Absyn_Constraint*_tmp430;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->tag == 5U){_LL1: _tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->f1).ptr_atts).zero_term;_LL2:
# 2246
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp430);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2251
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp431=Cyc_Tcutil_compress(t);void*_tmp432=_tmp431;union Cyc_Absyn_Constraint*_tmp433;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp432)->tag == 5U){_LL1: _tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp432)->f1).ptr_atts).nullable;_LL2:
# 2254
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp433);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2260
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp434=Cyc_Tcutil_compress(t);void*_tmp435=_tmp434;void*_tmp438;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->tag == 5U){_LL1: _tmp438=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->f1).elt_typ;_LL2:
 return _tmp438;}else{_LL3: _LL4:
({void*_tmp436=0U;({struct _dyneither_ptr _tmpC51=({const char*_tmp437="pointer_elt_type";_tag_dyneither(_tmp437,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC51,_tag_dyneither(_tmp436,sizeof(void*),0U));});});}_LL0:;}
# 2266
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp439=Cyc_Tcutil_compress(t);void*_tmp43A=_tmp439;struct Cyc_Absyn_PtrAtts*_tmp43D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43A)->tag == 5U){_LL1: _tmp43D=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43A)->f1).ptr_atts;_LL2:
 return _tmp43D->rgn;}else{_LL3: _LL4:
({void*_tmp43B=0U;({struct _dyneither_ptr _tmpC52=({const char*_tmp43C="pointer_elt_type";_tag_dyneither(_tmp43C,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC52,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});}_LL0:;}
# 2273
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp43E=Cyc_Tcutil_compress(t);void*_tmp43F=_tmp43E;void*_tmp440;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->tag == 5U){_LL1: _tmp440=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).rgn;_LL2:
# 2276
*rgn=_tmp440;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2283
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2287
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp441=Cyc_Tcutil_compress(t);void*_tmp442=_tmp441;union Cyc_Absyn_Constraint*_tmp443;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp442)->tag == 5U){_LL1: _tmp443=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp442)->f1).ptr_atts).bounds;_LL2:
# 2290
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp443)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2296
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp444=e->r;void*_tmp445=_tmp444;void*_tmp449;struct Cyc_Absyn_Exp*_tmp448;struct _dyneither_ptr _tmp447;switch(*((int*)_tmp445)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp445)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp445)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp445)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp445)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp445)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp447=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp445)->f1).Wchar_c).val;_LLC: {
# 2305
unsigned long _tmp446=Cyc_strlen((struct _dyneither_ptr)_tmp447);
int i=0;
if(_tmp446 >= 2  && *((const char*)_check_dyneither_subscript(_tmp447,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp447,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp447.curr)[1]== 'x'  && _tmp446 >= 3) && *((const char*)_check_dyneither_subscript(_tmp447,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp446;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp447,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2315
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2303
 return 1;case 14U: _LLD: _tmp449=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp445)->f1;_tmp448=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp445)->f2;_LLE:
# 2316
 return Cyc_Tcutil_is_zero(_tmp448) && Cyc_Tcutil_admits_zero(_tmp449);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2321
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2328
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2333
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2338
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2345
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2350
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2355
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp44A=ka;enum Cyc_Absyn_KindQual _tmp453;enum Cyc_Absyn_AliasQual _tmp452;_LL1: _tmp453=_tmp44A->kind;_tmp452=_tmp44A->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp44B=_tmp452;switch(_tmp44B){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2359
enum Cyc_Absyn_KindQual _tmp44C=_tmp453;switch(_tmp44C){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& Cyc_Tcutil_eko;default: _LL15: _LL16:
 return& Cyc_Tcutil_iko;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2368
{enum Cyc_Absyn_KindQual _tmp44D=_tmp453;switch(_tmp44D){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& Cyc_Tcutil_urko;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2375
goto _LL3;default: _LL8: _LL9:
# 2377
{enum Cyc_Absyn_KindQual _tmp44E=_tmp453;switch(_tmp44E){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& Cyc_Tcutil_trko;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2384
goto _LL3;}_LL3:;}
# 2386
({struct Cyc_String_pa_PrintArg_struct _tmp451=({struct Cyc_String_pa_PrintArg_struct _tmpA5F;_tmpA5F.tag=0U,({struct _dyneither_ptr _tmpC53=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmpA5F.f1=_tmpC53;});_tmpA5F;});void*_tmp44F[1U];_tmp44F[0]=& _tmp451;({struct _dyneither_ptr _tmpC54=({const char*_tmp450="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp450,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC54,_tag_dyneither(_tmp44F,sizeof(void*),1U));});});}
# 2389
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0U,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0U,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0U,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0U,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0U,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0U,& Cyc_Tcutil_ik};
# 2397
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2404
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0U,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0U,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0U,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0U,& Cyc_Tcutil_trk};
# 2409
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2414
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0U,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0U,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0U,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0U,& Cyc_Tcutil_urk};
# 2419
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2424
struct Cyc_Absyn_Kind*_tmp454=ka;enum Cyc_Absyn_KindQual _tmp45D;enum Cyc_Absyn_AliasQual _tmp45C;_LL1: _tmp45D=_tmp454->kind;_tmp45C=_tmp454->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp455=_tmp45C;switch(_tmp455){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2427
enum Cyc_Absyn_KindQual _tmp456=_tmp45D;switch(_tmp456){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& ab;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& mb;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& bb;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& rb;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& eb;default: _LL15: _LL16:
 return& ib;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2436
{enum Cyc_Absyn_KindQual _tmp457=_tmp45D;switch(_tmp457){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& uab;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& umb;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& urb;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2443
goto _LL3;default: _LL8: _LL9:
# 2445
{enum Cyc_Absyn_KindQual _tmp458=_tmp45D;switch(_tmp458){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& tab;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& tmb;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& tbb;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& trb;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2452
goto _LL3;}_LL3:;}
# 2454
({struct Cyc_String_pa_PrintArg_struct _tmp45B=({struct Cyc_String_pa_PrintArg_struct _tmpA60;_tmpA60.tag=0U,({struct _dyneither_ptr _tmpC55=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmpA60.f1=_tmpC55;});_tmpA60;});void*_tmp459[1U];_tmp459[0]=& _tmp45B;({struct _dyneither_ptr _tmpC56=({const char*_tmp45A="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp45A,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC56,_tag_dyneither(_tmp459,sizeof(void*),1U));});});}
# 2457
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2460
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2462
return({struct Cyc_Core_Opt*_tmp45E=_cycalloc(sizeof(*_tmp45E));({void*_tmpC57=Cyc_Tcutil_kind_to_bound(k);_tmp45E->v=_tmpC57;});_tmp45E;});}
# 2467
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpC58=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->tag=0U,_tmp45F->f1=Cyc_Absyn_Null_c;_tmp45F;});e1->r=_tmpC58;});{
struct Cyc_Core_Opt*_tmp460=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp461=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->tag=5U,({void*_tmpC5D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp460);(_tmp466->f1).elt_typ=_tmpC5D;}),({
struct Cyc_Absyn_Tqual _tmpC5C=Cyc_Absyn_empty_tqual(0U);(_tmp466->f1).elt_tq=_tmpC5C;}),
({void*_tmpC5B=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp460);((_tmp466->f1).ptr_atts).rgn=_tmpC5B;}),((_tmp466->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 2475
union Cyc_Absyn_Constraint*_tmpC5A=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp466->f1).ptr_atts).bounds=_tmpC5A;}),({
union Cyc_Absyn_Constraint*_tmpC59=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp466->f1).ptr_atts).zero_term=_tmpC59;}),((_tmp466->f1).ptr_atts).ptrloc=0;_tmp466;});
e1->topt=(void*)_tmp461;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp464=({struct Cyc_String_pa_PrintArg_struct _tmpA62;_tmpA62.tag=0U,({
struct _dyneither_ptr _tmpC5E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA62.f1=_tmpC5E;});_tmpA62;});struct Cyc_String_pa_PrintArg_struct _tmp465=({struct Cyc_String_pa_PrintArg_struct _tmpA61;_tmpA61.tag=0U,({struct _dyneither_ptr _tmpC5F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmpA61.f1=_tmpC5F;});_tmpA61;});void*_tmp462[2U];_tmp462[0]=& _tmp464,_tmp462[1]=& _tmp465;({struct _dyneither_ptr _tmpC60=({const char*_tmp463="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp463,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC60,_tag_dyneither(_tmp462,sizeof(void*),2U));});});
return retv;};};}
# 2485
return 0;}
# 2488
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp467=c;switch(_tmp467){case Cyc_Absyn_Unknown_coercion: _LL1: _LL2:
 return({const char*_tmp468="unknown";_tag_dyneither(_tmp468,sizeof(char),8U);});case Cyc_Absyn_No_coercion: _LL3: _LL4:
 return({const char*_tmp469="no coercion";_tag_dyneither(_tmp469,sizeof(char),12U);});case Cyc_Absyn_Null_to_NonNull: _LL5: _LL6:
 return({const char*_tmp46A="null check";_tag_dyneither(_tmp46A,sizeof(char),11U);});default: _LL7: _LL8:
 return({const char*_tmp46B="other coercion";_tag_dyneither(_tmp46B,sizeof(char),15U);});}_LL0:;}
# 2497
int Cyc_Tcutil_warn_alias_coerce=0;
# 2503
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2507
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpC63=({struct _dyneither_ptr*_tmp47F=_cycalloc(sizeof(*_tmp47F));({struct _dyneither_ptr _tmpC62=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp47E=({struct Cyc_Int_pa_PrintArg_struct _tmpA64;_tmpA64.tag=1U,_tmpA64.f1=(unsigned long)counter ++;_tmpA64;});void*_tmp47C[1U];_tmp47C[0]=& _tmp47E;({struct _dyneither_ptr _tmpC61=({const char*_tmp47D="__aliasvar%d";_tag_dyneither(_tmp47D,sizeof(char),13U);});Cyc_aprintf(_tmpC61,_tag_dyneither(_tmp47C,sizeof(void*),1U));});});*_tmp47F=_tmpC62;});_tmp47F;});_tmp480->f2=_tmpC63;});_tmp480;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_typ,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpC64=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->tag=4U,_tmp47B->f1=vd;_tmp47B;});Cyc_Absyn_varb_exp(_tmpC64,e->loc);});
# 2516
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2518
{void*_tmp46C=Cyc_Tcutil_compress(e_typ);void*_tmp46D=_tmp46C;void*_tmp47A;struct Cyc_Absyn_Tqual _tmp479;void*_tmp478;union Cyc_Absyn_Constraint*_tmp477;union Cyc_Absyn_Constraint*_tmp476;union Cyc_Absyn_Constraint*_tmp475;struct Cyc_Absyn_PtrLoc*_tmp474;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->tag == 5U){_LL1: _tmp47A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->f1).elt_typ;_tmp479=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->f1).elt_tq;_tmp478=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->f1).ptr_atts).rgn;_tmp477=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->f1).ptr_atts).nullable;_tmp476=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->f1).ptr_atts).bounds;_tmp475=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->f1).ptr_atts).zero_term;_tmp474=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46D)->f1).ptr_atts).ptrloc;_LL2:
# 2520
{void*_tmp46E=Cyc_Tcutil_compress(_tmp478);void*_tmp46F=_tmp46E;void**_tmp473;struct Cyc_Core_Opt*_tmp472;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp46F)->tag == 1U){_LL6: _tmp473=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp46F)->f2;_tmp472=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp46F)->f4;_LL7: {
# 2522
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp470=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=2U,_tmp471->f1=tv;_tmp471;});
*_tmp473=(void*)_tmp470;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2527
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2531
e->topt=0;
vd->initializer=0;{
# 2535
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2537
return({struct _tuple15 _tmpA63;_tmpA63.f1=d,_tmpA63.f2=ve;_tmpA63;});};}
# 2542
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2545
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2550
void*_tmp481=Cyc_Tcutil_compress(wants_typ);void*_tmp482=_tmp481;void*_tmp486;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp482)->tag == 5U){_LL1: _tmp486=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp482)->f1).ptr_atts).rgn;_LL2: {
# 2552
void*_tmp483=Cyc_Tcutil_compress(_tmp486);void*_tmp484=_tmp483;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp484)->tag == 20U){_LL6: _LL7:
 return 0;}else{_LL8: _LL9: {
# 2555
struct Cyc_Absyn_Kind*_tmp485=Cyc_Tcutil_typ_kind(_tmp486);
return _tmp485->kind == Cyc_Absyn_RgnKind  && _tmp485->aliasqual == Cyc_Absyn_Aliasable;}}_LL5:;}}else{_LL3: _LL4:
# 2558
 return 0;}_LL0:;}
# 2561
return 0;}
# 2565
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2570
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2572
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2574
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp489=({struct Cyc_String_pa_PrintArg_struct _tmpA66;_tmpA66.tag=0U,({
struct _dyneither_ptr _tmpC65=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA66.f1=_tmpC65;});_tmpA66;});struct Cyc_String_pa_PrintArg_struct _tmp48A=({struct Cyc_String_pa_PrintArg_struct _tmpA65;_tmpA65.tag=0U,({struct _dyneither_ptr _tmpC66=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA65.f1=_tmpC66;});_tmpA65;});void*_tmp487[2U];_tmp487[0]=& _tmp489,_tmp487[1]=& _tmp48A;({unsigned int _tmpC68=e->loc;struct _dyneither_ptr _tmpC67=({const char*_tmp488="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp488,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpC68,_tmpC67,_tag_dyneither(_tmp487,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2582
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp48D=({struct Cyc_String_pa_PrintArg_struct _tmpA69;_tmpA69.tag=0U,({
struct _dyneither_ptr _tmpC69=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpA69.f1=_tmpC69;});_tmpA69;});struct Cyc_String_pa_PrintArg_struct _tmp48E=({struct Cyc_String_pa_PrintArg_struct _tmpA68;_tmpA68.tag=0U,({struct _dyneither_ptr _tmpC6A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA68.f1=_tmpC6A;});_tmpA68;});struct Cyc_String_pa_PrintArg_struct _tmp48F=({struct Cyc_String_pa_PrintArg_struct _tmpA67;_tmpA67.tag=0U,({struct _dyneither_ptr _tmpC6B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA67.f1=_tmpC6B;});_tmpA67;});void*_tmp48B[3U];_tmp48B[0]=& _tmp48D,_tmp48B[1]=& _tmp48E,_tmp48B[2]=& _tmp48F;({unsigned int _tmpC6D=e->loc;struct _dyneither_ptr _tmpC6C=({const char*_tmp48C="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp48C,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpC6D,_tmpC6C,_tag_dyneither(_tmp48B,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2589
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({enum Cyc_Absyn_Coercion _tmpC6E=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpC6E;})!= Cyc_Absyn_Unknown_coercion){
# 2597
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp492=({struct Cyc_String_pa_PrintArg_struct _tmpA6B;_tmpA6B.tag=0U,({
struct _dyneither_ptr _tmpC6F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA6B.f1=_tmpC6F;});_tmpA6B;});struct Cyc_String_pa_PrintArg_struct _tmp493=({struct Cyc_String_pa_PrintArg_struct _tmpA6A;_tmpA6A.tag=0U,({struct _dyneither_ptr _tmpC70=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA6A.f1=_tmpC70;});_tmpA6A;});void*_tmp490[2U];_tmp490[0]=& _tmp492,_tmp490[1]=& _tmp493;({unsigned int _tmpC72=e->loc;struct _dyneither_ptr _tmpC71=({const char*_tmp491="implicit cast from %s to %s";_tag_dyneither(_tmp491,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpC72,_tmpC71,_tag_dyneither(_tmp490,sizeof(void*),2U));});});
return 1;}else{
# 2603
return 0;}}}}}
# 2610
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2613
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2617
int Cyc_Tcutil_coerceable(void*t){
void*_tmp494=Cyc_Tcutil_compress(t);void*_tmp495=_tmp494;switch(*((int*)_tmp495)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2635 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2639
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2642
struct _tuple23 _tmp496=*env;struct _tuple23 _tmp497=_tmp496;struct Cyc_List_List*_tmp49F;struct _RegionHandle*_tmp49E;struct Cyc_Tcenv_Tenv*_tmp49D;int _tmp49C;_LL1: _tmp49F=_tmp497.f1;_tmp49E=_tmp497.f2;_tmp49D=_tmp497.f3;_tmp49C=_tmp497.f4;_LL2:;{
# 2644
void*_tmp498=_tmp49F == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp49E,_tmp49F,x->type);
struct Cyc_List_List*_tmp499=Cyc_Tcutil_flatten_typ(_tmp49E,_tmp49C,_tmp49D,_tmp498);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp499)== 1)
return({struct Cyc_List_List*_tmp49B=_region_malloc(_tmp49E,sizeof(*_tmp49B));({struct _tuple12*_tmpC73=({struct _tuple12*_tmp49A=_region_malloc(_tmp49E,sizeof(*_tmp49A));_tmp49A->f1=x->tq,_tmp49A->f2=_tmp498;_tmp49A;});_tmp49B->hd=_tmpC73;}),_tmp49B->tl=0;_tmp49B;});else{
return _tmp499;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2650
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2652
struct _tuple24 _tmp4A0=*env;struct _tuple24 _tmp4A1=_tmp4A0;struct _RegionHandle*_tmp4AB;struct Cyc_Tcenv_Tenv*_tmp4AA;int _tmp4A9;_LL1: _tmp4AB=_tmp4A1.f1;_tmp4AA=_tmp4A1.f2;_tmp4A9=_tmp4A1.f3;_LL2:;{
struct _tuple12 _tmp4A2=*x;struct _tuple12 _tmp4A3=_tmp4A2;struct Cyc_Absyn_Tqual _tmp4A8;void*_tmp4A7;_LL4: _tmp4A8=_tmp4A3.f1;_tmp4A7=_tmp4A3.f2;_LL5:;{
struct Cyc_List_List*_tmp4A4=Cyc_Tcutil_flatten_typ(_tmp4AB,_tmp4A9,_tmp4AA,_tmp4A7);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4A4)== 1)
return({struct Cyc_List_List*_tmp4A6=_region_malloc(_tmp4AB,sizeof(*_tmp4A6));({struct _tuple12*_tmpC74=({struct _tuple12*_tmp4A5=_region_malloc(_tmp4AB,sizeof(*_tmp4A5));_tmp4A5->f1=_tmp4A8,_tmp4A5->f2=_tmp4A7;_tmp4A5;});_tmp4A6->hd=_tmpC74;}),_tmp4A6->tl=0;_tmp4A6;});else{
return _tmp4A4;}};};}
# 2659
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2663
if(flatten){
({void*_tmpC75=Cyc_Tcutil_compress(t1);t1=_tmpC75;});{
void*_tmp4AC=t1;struct Cyc_List_List*_tmp4CB;struct Cyc_Absyn_Aggrdecl*_tmp4CA;struct Cyc_List_List*_tmp4C9;struct Cyc_List_List*_tmp4C8;switch(*((int*)_tmp4AC)){case 0U: _LL1: _LL2:
 return 0;case 10U: _LL3: _tmp4C8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4AC)->f1;_LL4: {
# 2668
struct _tuple24 _tmp4AD=({struct _tuple24 _tmpA6C;_tmpA6C.f1=r,_tmpA6C.f2=te,_tmpA6C.f3=flatten;_tmpA6C;});
# 2670
struct Cyc_List_List*_tmp4AE=_tmp4C8;struct _tuple12*_tmp4B5;struct Cyc_List_List*_tmp4B4;if(_tmp4AE == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp4B5=(struct _tuple12*)_tmp4AE->hd;_tmp4B4=_tmp4AE->tl;_LLF: {
# 2673
struct Cyc_List_List*_tmp4AF=Cyc_Tcutil_rcopy_tqt(& _tmp4AD,_tmp4B5);
_tmp4AD.f3=0;{
struct Cyc_List_List*_tmp4B0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp4AD,_tmp4C8);
struct Cyc_List_List*_tmp4B1=({struct Cyc_List_List*_tmp4B3=_region_malloc(r,sizeof(*_tmp4B3));_tmp4B3->hd=_tmp4AF,_tmp4B3->tl=_tmp4B0;_tmp4B3;});
return({struct _RegionHandle*_tmpC76=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpC76,({struct Cyc_List_List*_tmp4B2=_region_malloc(r,sizeof(*_tmp4B2));_tmp4B2->hd=_tmp4AF,_tmp4B2->tl=_tmp4B0;_tmp4B2;}));});};}}_LLB:;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AC)->f1).aggr_info).KnownAggr).tag == 2){_LL5: _tmp4CA=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AC)->f1).aggr_info).KnownAggr).val;_tmp4C9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AC)->f1).targs;_LL6:
# 2681
 if(((_tmp4CA->kind == Cyc_Absyn_UnionA  || _tmp4CA->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CA->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CA->impl))->rgn_po != 0)
# 2683
return({struct Cyc_List_List*_tmp4B7=_region_malloc(r,sizeof(*_tmp4B7));({struct _tuple12*_tmpC78=({struct _tuple12*_tmp4B6=_region_malloc(r,sizeof(*_tmp4B6));({struct Cyc_Absyn_Tqual _tmpC77=Cyc_Absyn_empty_tqual(0U);_tmp4B6->f1=_tmpC77;}),_tmp4B6->f2=t1;_tmp4B6;});_tmp4B7->hd=_tmpC78;}),_tmp4B7->tl=0;_tmp4B7;});{
struct Cyc_List_List*_tmp4B8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4CA->tvs,_tmp4C9);
struct _tuple23 env=({struct _tuple23 _tmpA6D;_tmpA6D.f1=_tmp4B8,_tmpA6D.f2=r,_tmpA6D.f3=te,_tmpA6D.f4=flatten;_tmpA6D;});
struct Cyc_List_List*_tmp4B9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CA->impl))->fields;struct Cyc_List_List*_tmp4BA=_tmp4B9;struct Cyc_Absyn_Aggrfield*_tmp4C0;struct Cyc_List_List*_tmp4BF;if(_tmp4BA == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp4C0=(struct Cyc_Absyn_Aggrfield*)_tmp4BA->hd;_tmp4BF=_tmp4BA->tl;_LL14: {
# 2689
struct Cyc_List_List*_tmp4BB=Cyc_Tcutil_flatten_typ_f(& env,_tmp4C0);
env.f4=0;{
struct Cyc_List_List*_tmp4BC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp4BF);
struct Cyc_List_List*_tmp4BD=({struct Cyc_List_List*_tmp4BE=_region_malloc(r,sizeof(*_tmp4BE));_tmp4BE->hd=_tmp4BB,_tmp4BE->tl=_tmp4BC;_tmp4BE;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp4BD);};}}_LL10:;};}else{goto _LL9;}case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4AC)->f1 == Cyc_Absyn_StructA){_LL7: _tmp4CB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4AC)->f2;_LL8: {
# 2696
struct _tuple23 env=({struct _tuple23 _tmpA6E;_tmpA6E.f1=0,_tmpA6E.f2=r,_tmpA6E.f3=te,_tmpA6E.f4=flatten;_tmpA6E;});
struct Cyc_List_List*_tmp4C1=_tmp4CB;struct Cyc_Absyn_Aggrfield*_tmp4C7;struct Cyc_List_List*_tmp4C6;if(_tmp4C1 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp4C7=(struct Cyc_Absyn_Aggrfield*)_tmp4C1->hd;_tmp4C6=_tmp4C1->tl;_LL19: {
# 2700
struct Cyc_List_List*_tmp4C2=Cyc_Tcutil_flatten_typ_f(& env,_tmp4C7);
env.f4=0;{
struct Cyc_List_List*_tmp4C3=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp4C6);
struct Cyc_List_List*_tmp4C4=({struct Cyc_List_List*_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5));_tmp4C5->hd=_tmp4C2,_tmp4C5->tl=_tmp4C3;_tmp4C5;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp4C4);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2706
 goto _LL0;}_LL0:;};}
# 2709
return({struct Cyc_List_List*_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD));({struct _tuple12*_tmpC7A=({struct _tuple12*_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC));({struct Cyc_Absyn_Tqual _tmpC79=Cyc_Absyn_empty_tqual(0U);_tmp4CC->f1=_tmpC79;}),_tmp4CC->f2=t1;_tmp4CC;});_tmp4CD->hd=_tmpC7A;}),_tmp4CD->tl=0;_tmp4CD;});}
# 2713
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp4CE=(void*)t->hd;void*_tmp4CF=_tmp4CE;switch(*((int*)_tmp4CF)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2719
 continue;default: _LL7: _LL8:
# 2721
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2724
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2727
return 1;}
# 2730
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp4D0=({struct _tuple0 _tmpA6F;({void*_tmpC7C=Cyc_Tcutil_compress(t1);_tmpA6F.f1=_tmpC7C;}),({void*_tmpC7B=Cyc_Tcutil_compress(t2);_tmpA6F.f2=_tmpC7B;});_tmpA6F;});struct _tuple0 _tmp4D1=_tmp4D0;enum Cyc_Absyn_Size_of _tmp4D3;enum Cyc_Absyn_Size_of _tmp4D2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D1.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D1.f2)->tag == 6U){_LL1: _tmp4D3=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D1.f1)->f2;_tmp4D2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4D1.f2)->f2;_LL2:
# 2733
 return(_tmp4D3 == _tmp4D2  || _tmp4D3 == Cyc_Absyn_Int_sz  && _tmp4D2 == Cyc_Absyn_Long_sz) || 
_tmp4D3 == Cyc_Absyn_Long_sz  && _tmp4D2 == Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2741
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2744
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
({void*_tmpC7D=Cyc_Tcutil_compress(t1);t1=_tmpC7D;});
({void*_tmpC7E=Cyc_Tcutil_compress(t2);t2=_tmpC7E;});{
struct _tuple0 _tmp4D4=({struct _tuple0 _tmpA71;_tmpA71.f1=t1,_tmpA71.f2=t2;_tmpA71;});struct _tuple0 _tmp4D5=_tmp4D4;struct Cyc_Absyn_FnInfo _tmp502;struct Cyc_Absyn_FnInfo _tmp501;struct Cyc_Absyn_Datatypedecl*_tmp500;struct Cyc_Absyn_Datatypefield*_tmp4FF;struct Cyc_List_List*_tmp4FE;struct Cyc_Absyn_Datatypedecl*_tmp4FD;struct Cyc_List_List*_tmp4FC;void*_tmp4FB;struct Cyc_Absyn_Tqual _tmp4FA;void*_tmp4F9;union Cyc_Absyn_Constraint*_tmp4F8;union Cyc_Absyn_Constraint*_tmp4F7;union Cyc_Absyn_Constraint*_tmp4F6;void*_tmp4F5;struct Cyc_Absyn_Tqual _tmp4F4;void*_tmp4F3;union Cyc_Absyn_Constraint*_tmp4F2;union Cyc_Absyn_Constraint*_tmp4F1;union Cyc_Absyn_Constraint*_tmp4F0;switch(*((int*)_tmp4D5.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f2)->tag == 5U){_LL1: _tmp4FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f1)->f1).elt_typ;_tmp4FA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f1)->f1).elt_tq;_tmp4F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f1)->f1).ptr_atts).rgn;_tmp4F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f1)->f1).ptr_atts).nullable;_tmp4F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f1)->f1).ptr_atts).bounds;_tmp4F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f1)->f1).ptr_atts).zero_term;_tmp4F5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f2)->f1).elt_typ;_tmp4F4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f2)->f1).elt_tq;_tmp4F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f2)->f1).ptr_atts).rgn;_tmp4F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f2)->f1).ptr_atts).nullable;_tmp4F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f2)->f1).ptr_atts).bounds;_tmp4F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5.f2)->f1).ptr_atts).zero_term;_LL2:
# 2756
 if(_tmp4FA.real_const  && !_tmp4F4.real_const)
return 0;
# 2759
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F8,_tmp4F2) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F8)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F2))
return 0;
# 2763
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4F6,_tmp4F0) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F6)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4F0))
return 0;
# 2767
if((!Cyc_Tcutil_unify(_tmp4F9,_tmp4F3) && !Cyc_Tcenv_region_outlives(te,_tmp4F9,_tmp4F3)) && !
Cyc_Tcutil_subtype(te,assume,_tmp4F9,_tmp4F3))
return 0;
# 2771
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4F7,_tmp4F1)){
struct _tuple0 _tmp4D6=({struct _tuple0 _tmpA70;({void*_tmpC80=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4F7);_tmpA70.f1=_tmpC80;}),({void*_tmpC7F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4F1);_tmpA70.f2=_tmpC7F;});_tmpA70;});struct _tuple0 _tmp4D7=_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D7.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4D7.f2)->tag == 0U){_LLA: _LLB:
 goto _LL9;}else{goto _LLE;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D7.f2)->tag == 1U){_LLC: _tmp4D9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D7.f1)->f1;_tmp4D8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4D7.f2)->f1;_LLD:
# 2776
 if(!Cyc_Evexp_lte_const_exp(_tmp4D8,_tmp4D9))
return 0;
goto _LL9;}else{_LLE: _LLF:
 return 0;}}_LL9:;}
# 2784
if(!_tmp4F4.real_const  && _tmp4FA.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp4F5)))
return 0;}{
# 2790
int _tmp4DA=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4F1,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp4F0);
# 2796
return(_tmp4DA  && ({struct Cyc_Tcenv_Tenv*_tmpC84=te;struct Cyc_List_List*_tmpC83=({struct Cyc_List_List*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));({struct _tuple0*_tmpC81=({struct _tuple0*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->f1=t1,_tmp4DB->f2=t2;_tmp4DB;});_tmp4DC->hd=_tmpC81;}),_tmp4DC->tl=assume;_tmp4DC;});void*_tmpC82=_tmp4FB;Cyc_Tcutil_ptrsubtype(_tmpC84,_tmpC83,_tmpC82,_tmp4F5);}) || Cyc_Tcutil_unify(_tmp4FB,_tmp4F5)) || Cyc_Tcutil_isomorphic(_tmp4FB,_tmp4F5);};}else{goto _LL7;}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D5.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D5.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D5.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3: _tmp500=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D5.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp4FF=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D5.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp4FE=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4D5.f1)->f1).targs;_tmp4FD=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D5.f2)->f1).datatype_info).KnownDatatype).val;_tmp4FC=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4D5.f2)->f1).targs;_LL4:
# 2802
 if(_tmp500 != _tmp4FD  && Cyc_Absyn_qvar_cmp(_tmp500->name,_tmp4FD->name)!= 0)return 0;
# 2804
if(({int _tmpC85=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4FE);_tmpC85 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4FC);}))return 0;
for(0;_tmp4FE != 0;(_tmp4FE=_tmp4FE->tl,_tmp4FC=_tmp4FC->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4FE->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp4FC))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 9U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D5.f2)->tag == 9U){_LL5: _tmp502=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D5.f1)->f1;_tmp501=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D5.f2)->f1;_LL6:
# 2811
 if(_tmp502.tvars != 0  || _tmp501.tvars != 0){
struct Cyc_List_List*_tmp4DD=_tmp502.tvars;
struct Cyc_List_List*_tmp4DE=_tmp501.tvars;
if(({int _tmpC86=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4DD);_tmpC86 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4DE);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp4DD != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp4DD->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpC89=({struct Cyc_List_List*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));({struct _tuple16*_tmpC88=({struct _tuple16*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->f1=(struct Cyc_Absyn_Tvar*)_tmp4DE->hd,({void*_tmpC87=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->tag=2U,_tmp4DF->f1=(struct Cyc_Absyn_Tvar*)_tmp4DD->hd;_tmp4DF;});_tmp4E0->f2=_tmpC87;});_tmp4E0;});_tmp4E1->hd=_tmpC88;}),_tmp4E1->tl=inst;_tmp4E1;});inst=_tmpC89;});
_tmp4DD=_tmp4DD->tl;
_tmp4DE=_tmp4DE->tl;}
# 2822
if(inst != 0){
_tmp502.tvars=0;
_tmp501.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpC8C=te;struct Cyc_List_List*_tmpC8B=assume;void*_tmpC8A=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->tag=9U,_tmp4E2->f1=_tmp502;_tmp4E2;});Cyc_Tcutil_subtype(_tmpC8C,_tmpC8B,_tmpC8A,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->tag=9U,_tmp4E3->f1=_tmp501;_tmp4E3;}));});}};}
# 2829
if(!Cyc_Tcutil_subtype(te,assume,_tmp502.ret_typ,_tmp501.ret_typ))return 0;{
struct Cyc_List_List*_tmp4E4=_tmp502.args;
struct Cyc_List_List*_tmp4E5=_tmp501.args;
# 2834
if(({int _tmpC8D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E4);_tmpC8D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E5);}))return 0;
# 2836
for(0;_tmp4E4 != 0;(_tmp4E4=_tmp4E4->tl,_tmp4E5=_tmp4E5->tl)){
struct _tuple10 _tmp4E6=*((struct _tuple10*)_tmp4E4->hd);struct _tuple10 _tmp4E7=_tmp4E6;struct Cyc_Absyn_Tqual _tmp4ED;void*_tmp4EC;_LL11: _tmp4ED=_tmp4E7.f2;_tmp4EC=_tmp4E7.f3;_LL12:;{
struct _tuple10 _tmp4E8=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp4E5))->hd);struct _tuple10 _tmp4E9=_tmp4E8;struct Cyc_Absyn_Tqual _tmp4EB;void*_tmp4EA;_LL14: _tmp4EB=_tmp4E9.f2;_tmp4EA=_tmp4E9.f3;_LL15:;
# 2840
if(_tmp4EB.real_const  && !_tmp4ED.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp4EA,_tmp4EC))
return 0;};}
# 2844
if(_tmp502.c_varargs != _tmp501.c_varargs)return 0;
if(_tmp502.cyc_varargs != 0  && _tmp501.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp4EE=*_tmp502.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp4EF=*_tmp501.cyc_varargs;
# 2849
if((_tmp4EF.tq).real_const  && !(_tmp4EE.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp4EF.type,_tmp4EE.type))
return 0;}else{
if(_tmp502.cyc_varargs != 0  || _tmp501.cyc_varargs != 0)return 0;}
# 2854
if(!({void*_tmpC8E=(void*)_check_null(_tmp502.effect);Cyc_Tcutil_subset_effect(1,_tmpC8E,(void*)_check_null(_tmp501.effect));}))return 0;
# 2856
if(!Cyc_Tcutil_sub_rgnpo(_tmp502.rgn_po,_tmp501.rgn_po))return 0;
# 2858
if(!Cyc_Tcutil_sub_attributes(_tmp502.attributes,_tmp501.attributes))return 0;
# 2860
if(!Cyc_Tcutil_check_logical_implication(_tmp501.requires_relns,_tmp502.requires_relns))
return 0;
# 2863
if(!Cyc_Tcutil_check_logical_implication(_tmp502.ensures_relns,_tmp501.ensures_relns))
return 0;
# 2866
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2878 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2880
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp503=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp504=_tmp503;struct Cyc_Absyn_Tqual _tmp50A;void*_tmp509;_LL1: _tmp50A=_tmp504->f1;_tmp509=_tmp504->f2;_LL2:;{
struct _tuple12*_tmp505=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp506=_tmp505;struct Cyc_Absyn_Tqual _tmp508;void*_tmp507;_LL4: _tmp508=_tmp506->f1;_tmp507=_tmp506->f2;_LL5:;
# 2887
if(_tmp50A.real_const  && !_tmp508.real_const)return 0;
# 2889
if((_tmp508.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp507))) && 
Cyc_Tcutil_subtype(te,assume,_tmp509,_tmp507))
# 2892
continue;
# 2894
if(Cyc_Tcutil_unify(_tmp509,_tmp507))
# 2896
continue;
# 2898
if(Cyc_Tcutil_isomorphic(_tmp509,_tmp507))
# 2900
continue;
# 2903
return 0;};};}
# 2905
return 1;}
# 2908
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp50B=Cyc_Tcutil_compress(t);void*_tmp50C=_tmp50B;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50C)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50C)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2917
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpC8F=Cyc_Tcutil_compress(t1);t1=_tmpC8F;});
({void*_tmpC90=Cyc_Tcutil_compress(t2);t2=_tmpC90;});
# 2923
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp50D=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50D)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp50D)->f2){case Cyc_Absyn_Int_sz: _LL1: _LL2:
# 2927
 goto _LL4;case Cyc_Absyn_Long_sz: _LL3: _LL4:
# 2929
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 2933
void*_tmp50E=t1;void*_tmp535;struct Cyc_Absyn_Enumdecl*_tmp534;void*_tmp533;struct Cyc_Absyn_Tqual _tmp532;struct Cyc_Absyn_Exp*_tmp531;union Cyc_Absyn_Constraint*_tmp530;void*_tmp52F;struct Cyc_Absyn_Tqual _tmp52E;void*_tmp52D;union Cyc_Absyn_Constraint*_tmp52C;union Cyc_Absyn_Constraint*_tmp52B;union Cyc_Absyn_Constraint*_tmp52A;switch(*((int*)_tmp50E)){case 5U: _LL8: _tmp52F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50E)->f1).elt_typ;_tmp52E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50E)->f1).elt_tq;_tmp52D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50E)->f1).ptr_atts).rgn;_tmp52C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50E)->f1).ptr_atts).nullable;_tmp52B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50E)->f1).ptr_atts).bounds;_tmp52A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50E)->f1).ptr_atts).zero_term;_LL9:
# 2942
{void*_tmp50F=t2;void*_tmp520;struct Cyc_Absyn_Tqual _tmp51F;void*_tmp51E;union Cyc_Absyn_Constraint*_tmp51D;union Cyc_Absyn_Constraint*_tmp51C;union Cyc_Absyn_Constraint*_tmp51B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->tag == 5U){_LL17: _tmp520=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).elt_typ;_tmp51F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).elt_tq;_tmp51E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).rgn;_tmp51D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).nullable;_tmp51C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).bounds;_tmp51B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50F)->f1).ptr_atts).zero_term;_LL18: {
# 2946
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp510=({struct Cyc_List_List*_tmp51A=_cycalloc(sizeof(*_tmp51A));({struct _tuple0*_tmpC91=({struct _tuple0*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->f1=t1,_tmp519->f2=t2;_tmp519;});_tmp51A->hd=_tmpC91;}),_tmp51A->tl=0;_tmp51A;});
int _tmp511=_tmp51F.real_const  || !_tmp52E.real_const;
# 2960 "tcutil.cyc"
int _tmp512=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp51C,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp51B);
# 2964
int _tmp513=_tmp511  && (
((_tmp512  && Cyc_Tcutil_ptrsubtype(te,_tmp510,_tmp52F,_tmp520) || 
Cyc_Tcutil_unify(_tmp52F,_tmp520)) || Cyc_Tcutil_isomorphic(_tmp52F,_tmp520)) || Cyc_Tcutil_unify(_tmp520,(void*)& Cyc_Absyn_VoidType_val));
# 2968
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp52A,_tmp51B) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp51B);
# 2972
int _tmp514=_tmp513?0:((Cyc_Tcutil_bits_only(_tmp52F) && Cyc_Tcutil_is_char_type(_tmp520)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp51B)) && (
_tmp51F.real_const  || !_tmp52E.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp52B,_tmp51C);
if(!bounds_ok  && !_tmp514){
struct _tuple0 _tmp515=({struct _tuple0 _tmpA72;({void*_tmpC93=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp52B);_tmpA72.f1=_tmpC93;}),({void*_tmpC92=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp51C);_tmpA72.f2=_tmpC92;});_tmpA72;});struct _tuple0 _tmp516=_tmp515;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp517;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp516.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp516.f2)->tag == 1U){_LL1C: _tmp518=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp516.f1)->f1;_tmp517=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp516.f2)->f1;_LL1D:
# 2982
 if(Cyc_Evexp_lte_const_exp(_tmp517,_tmp518))
bounds_ok=1;
goto _LL1B;}else{goto _LL1E;}}else{_LL1E: _LL1F:
# 2987
 bounds_ok=1;goto _LL1B;}_LL1B:;}{
# 2989
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52C);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp51D);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2996
if(((bounds_ok  && zeroterm_ok) && (_tmp513  || _tmp514)) && (
Cyc_Tcutil_unify(_tmp52D,_tmp51E) || Cyc_Tcenv_region_outlives(te,_tmp52D,_tmp51E)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 3002
return Cyc_Absyn_Unknown_coercion;case 8U: _LLA: _tmp533=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50E)->f1).elt_type;_tmp532=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50E)->f1).tq;_tmp531=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50E)->f1).num_elts;_tmp530=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50E)->f1).zero_term;_LLB:
# 3004
{void*_tmp521=t2;void*_tmp525;struct Cyc_Absyn_Tqual _tmp524;struct Cyc_Absyn_Exp*_tmp523;union Cyc_Absyn_Constraint*_tmp522;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp521)->tag == 8U){_LL21: _tmp525=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp521)->f1).elt_type;_tmp524=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp521)->f1).tq;_tmp523=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp521)->f1).num_elts;_tmp522=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp521)->f1).zero_term;_LL22: {
# 3006
int okay;
({int _tmpC94=
(((_tmp531 != 0  && _tmp523 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp530,_tmp522)) && 
Cyc_Evexp_lte_const_exp(_tmp523,_tmp531)) && 
Cyc_Evexp_lte_const_exp(_tmp531,_tmp523);
# 3007
okay=_tmpC94;});
# 3011
return
# 3013
(okay  && Cyc_Tcutil_unify(_tmp533,_tmp525)) && (!_tmp532.real_const  || _tmp524.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL23: _LL24:
# 3015
 return Cyc_Absyn_Unknown_coercion;}_LL20:;}
# 3017
return Cyc_Absyn_Unknown_coercion;case 13U: _LLC: _tmp534=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp50E)->f2;_LLD:
# 3021
{void*_tmp526=t2;struct Cyc_Absyn_Enumdecl*_tmp527;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp526)->tag == 13U){_LL26: _tmp527=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp526)->f2;_LL27:
# 3023
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp534))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp527))->fields != 0) && ({
int _tmpC95=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp534->fields))->v);_tmpC95 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp527->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL25;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}
# 3029
goto _LLF;case 6U: _LLE: _LLF:
 goto _LL11;case 7U: _LL10: _LL11:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15U: _LL12: _tmp535=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp50E)->f1;_LL13:
# 3034
{void*_tmp528=t2;void*_tmp529;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp528)->tag == 15U){_LL2B: _tmp529=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp528)->f1;_LL2C:
# 3036
 if(Cyc_Tcenv_region_outlives(te,_tmp535,_tmp529))return Cyc_Absyn_No_coercion;
goto _LL2A;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}
# 3040
return Cyc_Absyn_Unknown_coercion;default: _LL14: _LL15:
 return Cyc_Absyn_Unknown_coercion;}_LL7:;};}
# 3046
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp536=Cyc_Absyn_copy_exp(e);
({void*_tmpC96=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->tag=14U,_tmp537->f1=t,_tmp537->f2=_tmp536,_tmp537->f3=0,_tmp537->f4=c;_tmp537;});e->r=_tmpC96;});
e->topt=t;}}
# 3054
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp538=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp539=_tmp538;switch(*((int*)_tmp539)){case 6U: _LL1: _LL2:
 goto _LL4;case 13U: _LL3: _LL4:
 goto _LL6;case 14U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 return 1;case 1U: _LL9: _LLA:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3065
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp53A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp53B=_tmp53A;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp53B)->tag == 7U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 3074
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp53C=Cyc_Tcutil_compress(t);void*_tmp53D=_tmp53C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53D)->tag == 9U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 3081
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp53E=({struct _tuple0 _tmpA73;_tmpA73.f1=t1,_tmpA73.f2=t2;_tmpA73;});struct _tuple0 _tmp53F=_tmp53E;int _tmp541;int _tmp540;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp53F.f1)->tag == 7U){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp53F.f2)->tag == 7U){_LL1: _tmp541=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp53F.f1)->f1;_tmp540=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp53F.f2)->f1;_LL2:
# 3084
 if(_tmp541 != 0  && _tmp541 != 1)return t1;else{
if(_tmp540 != 0  && _tmp540 != 1)return t2;else{
if(_tmp541 >= _tmp540)return t1;else{
return t2;}}}}else{_LL3: _LL4:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp53F.f2)->tag == 7U){_LL5: _LL6:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 goto _LLA;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2 == Cyc_Absyn_Long_sz)goto _LLF;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL11;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL13;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL15;else{goto _LL1B;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2){case Cyc_Absyn_Long_sz: goto _LLF;case Cyc_Absyn_Int_sz: goto _LL13;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2){case Cyc_Absyn_Long_sz: _LLF: _LL10:
# 3094
 goto _LL12;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 3097
 goto _LL16;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL11;case Cyc_Absyn_Int_sz: goto _LL15;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{goto _LL1B;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2){case Cyc_Absyn_LongLong_sz: _LLB: _LLC:
# 3092
 goto _LLE;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 3099
 goto _LL1A;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL9: _LLA:
# 3091
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL11: _LL12:
# 3095
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL15: _LL16:
# 3098
 return Cyc_Absyn_uint_typ;default: goto _LL1B;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLD: _LLE:
# 3093
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 3100
 return Cyc_Absyn_slong_typ;default: goto _LL1B;}}}else{_LL1B: _LL1C:
 return Cyc_Absyn_sint_typ;}}}}_LL0:;}
# 3112 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpC97=Cyc_Absyn_compress_kb(c1);c1=_tmpC97;});
({void*_tmpC98=Cyc_Absyn_compress_kb(c2);c2=_tmpC98;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp542=({struct _tuple0 _tmpA74;_tmpA74.f1=c1,_tmpA74.f2=c2;_tmpA74;});struct _tuple0 _tmp543=_tmp542;struct Cyc_Core_Opt**_tmp557;struct Cyc_Absyn_Kind*_tmp556;struct Cyc_Core_Opt**_tmp555;struct Cyc_Absyn_Kind*_tmp554;struct Cyc_Core_Opt**_tmp553;struct Cyc_Absyn_Kind*_tmp552;struct Cyc_Absyn_Kind*_tmp551;struct Cyc_Core_Opt**_tmp550;struct Cyc_Core_Opt**_tmp54F;struct Cyc_Absyn_Kind*_tmp54E;struct Cyc_Core_Opt**_tmp54D;struct Cyc_Absyn_Kind*_tmp54C;struct Cyc_Absyn_Kind*_tmp54B;struct Cyc_Absyn_Kind*_tmp54A;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp543.f1)->tag == 0U)switch(*((int*)_tmp543.f2)){case 0U: _LL1: _tmp54B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp543.f1)->f1;_tmp54A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp543.f2)->f1;_LL2:
 return _tmp54B == _tmp54A;case 1U: goto _LL3;default: _LL9: _tmp54E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp543.f1)->f1;_tmp54D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f2)->f1;_tmp54C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f2)->f2;_LLA:
# 3125
 if(Cyc_Tcutil_kind_leq(_tmp54E,_tmp54C)){
({struct Cyc_Core_Opt*_tmpC99=({struct Cyc_Core_Opt*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->v=c1;_tmp547;});*_tmp54D=_tmpC99;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp543.f2)->tag == 1U){_LL3: _tmp54F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp543.f2)->f1;_LL4:
# 3118
({struct Cyc_Core_Opt*_tmpC9A=({struct Cyc_Core_Opt*_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544->v=c1;_tmp544;});*_tmp54F=_tmpC9A;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp543.f1)->tag == 1U){_LL5: _tmp550=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp543.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpC9B=({struct Cyc_Core_Opt*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->v=c2;_tmp545;});*_tmp550=_tmpC9B;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp543.f2)->tag == 0U){_LL7: _tmp553=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f1)->f1;_tmp552=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f1)->f2;_tmp551=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp543.f2)->f1;_LL8:
# 3121
 if(Cyc_Tcutil_kind_leq(_tmp551,_tmp552)){
({struct Cyc_Core_Opt*_tmpC9C=({struct Cyc_Core_Opt*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->v=c2;_tmp546;});*_tmp553=_tmpC9C;});return 1;}else{
return 0;}}else{_LLB: _tmp557=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f1)->f1;_tmp556=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f1)->f2;_tmp555=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f2)->f1;_tmp554=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543.f2)->f2;_LLC:
# 3129
 if(Cyc_Tcutil_kind_leq(_tmp556,_tmp554)){
({struct Cyc_Core_Opt*_tmpC9D=({struct Cyc_Core_Opt*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548->v=c1;_tmp548;});*_tmp555=_tmpC9D;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp554,_tmp556)){
({struct Cyc_Core_Opt*_tmpC9E=({struct Cyc_Core_Opt*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->v=c2;_tmp549;});*_tmp557=_tmpC9E;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3138
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3143
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp55D=({struct Cyc_Int_pa_PrintArg_struct _tmpA75;_tmpA75.tag=1U,_tmpA75.f1=(unsigned long)i;_tmpA75;});void*_tmp55B[1U];_tmp55B[0]=& _tmp55D;({struct _dyneither_ptr _tmpC9F=({const char*_tmp55C="#%d";_tag_dyneither(_tmp55C,sizeof(char),4U);});Cyc_aprintf(_tmpC9F,_tag_dyneither(_tmp55B,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp55A=_cycalloc(sizeof(*_tmp55A));({struct _dyneither_ptr*_tmpCA0=({unsigned int _tmp559=1;struct _dyneither_ptr*_tmp558=_cycalloc(_check_times(_tmp559,sizeof(struct _dyneither_ptr)));_tmp558[0]=s;_tmp558;});_tmp55A->name=_tmpCA0;}),_tmp55A->identity=- 1,_tmp55A->kind=k;_tmp55A;});}
# 3150
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp55E=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp55E,sizeof(char),0))== '#';}
# 3155
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp561=({struct Cyc_String_pa_PrintArg_struct _tmpA76;_tmpA76.tag=0U,_tmpA76.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmpA76;});void*_tmp55F[1U];_tmp55F[0]=& _tmp561;({struct _dyneither_ptr _tmpCA1=({const char*_tmp560="%s";_tag_dyneither(_tmp560,sizeof(char),3U);});Cyc_printf(_tmpCA1,_tag_dyneither(_tmp55F,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp562=({struct _dyneither_ptr _tmpCA2=({const char*_tmp568="`";_tag_dyneither(_tmp568,sizeof(char),2U);});Cyc_strconcat(_tmpCA2,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp563=_dyneither_ptr_plus(_tmp562,sizeof(char),1);char _tmp564=*((char*)_check_dyneither_subscript(_tmp563,sizeof(char),0U));char _tmp565='t';if(_get_dyneither_size(_tmp563,sizeof(char))== 1U  && (_tmp564 == '\000'  && _tmp565 != '\000'))_throw_arraybounds();*((char*)_tmp563.curr)=_tmp565;});
({struct _dyneither_ptr*_tmpCA3=({unsigned int _tmp567=1;struct _dyneither_ptr*_tmp566=_cycalloc(_check_times(_tmp567,sizeof(struct _dyneither_ptr)));_tmp566[0]=(struct _dyneither_ptr)_tmp562;_tmp566;});t->name=_tmpCA3;});};}
# 3164
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3166
return({struct _tuple10*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->f1=(*x).f1,_tmp569->f2=(*x).f2,_tmp569->f3=(*x).f3;_tmp569;});}
# 3169
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3176
struct Cyc_List_List*_tmp56A=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpCA4=({struct Cyc_List_List*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->hd=(void*)atts->hd,_tmp56B->tl=_tmp56A;_tmp56B;});_tmp56A=_tmpCA4;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->tag=9U,(_tmp56C->f1).tvars=fd->tvs,(_tmp56C->f1).effect=fd->effect,(_tmp56C->f1).ret_tqual=fd->ret_tqual,(_tmp56C->f1).ret_typ=fd->ret_type,({
struct Cyc_List_List*_tmpCA5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);(_tmp56C->f1).args=_tmpCA5;}),(_tmp56C->f1).c_varargs=fd->c_varargs,(_tmp56C->f1).cyc_varargs=fd->cyc_varargs,(_tmp56C->f1).rgn_po=fd->rgn_po,(_tmp56C->f1).attributes=_tmp56A,(_tmp56C->f1).requires_clause=fd->requires_clause,(_tmp56C->f1).requires_relns=fd->requires_relns,(_tmp56C->f1).ensures_clause=fd->ensures_clause,(_tmp56C->f1).ensures_relns=fd->ensures_relns;_tmp56C;});}
# 3189
return(void*)_check_null(fd->cached_typ);}
# 3195
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3197
union Cyc_Relations_RelnOp _tmp56D=*rop;union Cyc_Relations_RelnOp _tmp56E=_tmp56D;struct Cyc_Absyn_Vardecl*_tmp581;struct Cyc_Absyn_Vardecl*_tmp580;switch((_tmp56E.RNumelts).tag){case 2U: _LL1: _tmp580=(_tmp56E.RVar).val;_LL2: {
# 3199
struct _tuple2 _tmp56F=*_tmp580->name;struct _tuple2 _tmp570=_tmp56F;union Cyc_Absyn_Nmspace _tmp577;struct _dyneither_ptr*_tmp576;_LL8: _tmp577=_tmp570.f1;_tmp576=_tmp570.f2;_LL9:;
if(!((int)((_tmp577.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpCA6=(struct _dyneither_ptr)*_tmp576;Cyc_strcmp(_tmpCA6,({const char*_tmp571="return_value";_tag_dyneither(_tmp571,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpCA7=Cyc_Relations_RReturn();*rop=_tmpCA7;});
goto _LL0;}{
# 3205
unsigned int c=0U;
{struct Cyc_List_List*_tmp572=args;for(0;_tmp572 != 0;(_tmp572=_tmp572->tl,c ++)){
struct _tuple10*_tmp573=(struct _tuple10*)_tmp572->hd;struct _tuple10*_tmp574=_tmp573;struct _dyneither_ptr*_tmp575;_LLB: _tmp575=_tmp574->f1;_LLC:;
if(_tmp575 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp576,(struct _dyneither_ptr)*_tmp575)== 0){
({union Cyc_Relations_RelnOp _tmpCA8=Cyc_Relations_RParam(c);*rop=_tmpCA8;});
break;}}}}
# 3215
goto _LL0;};}case 3U: _LL3: _tmp581=(_tmp56E.RNumelts).val;_LL4: {
# 3217
struct _tuple2 _tmp578=*_tmp581->name;struct _tuple2 _tmp579=_tmp578;union Cyc_Absyn_Nmspace _tmp57F;struct _dyneither_ptr*_tmp57E;_LLE: _tmp57F=_tmp579.f1;_tmp57E=_tmp579.f2;_LLF:;
if(!((int)((_tmp57F.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp57A=args;for(0;_tmp57A != 0;(_tmp57A=_tmp57A->tl,c ++)){
struct _tuple10*_tmp57B=(struct _tuple10*)_tmp57A->hd;struct _tuple10*_tmp57C=_tmp57B;struct _dyneither_ptr*_tmp57D;_LL11: _tmp57D=_tmp57C->f1;_LL12:;
if(_tmp57D != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp57E,(struct _dyneither_ptr)*_tmp57D)== 0){
({union Cyc_Relations_RelnOp _tmpCA9=Cyc_Relations_RParamNumelts(c);*rop=_tmpCA9;});
break;}}}}
# 3229
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3234
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3236
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3240
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3242
if(e == 0)return 0;{
struct Cyc_List_List*_tmp582=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp582);
return _tmp582;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3249
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp583=pr;struct Cyc_Absyn_Tqual _tmp586;void*_tmp585;_LL1: _tmp586=_tmp583->f1;_tmp585=_tmp583->f2;_LL2:;
if(_tmp585 == t)return pr;else{
return({struct _tuple12*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->f1=_tmp586,_tmp584->f2=t;_tmp584;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3256
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3258
return({struct _tuple27*_tmp588=_region_malloc(rgn,sizeof(*_tmp588));({struct _tuple26*_tmpCAA=({struct _tuple26*_tmp587=_region_malloc(rgn,sizeof(*_tmp587));_tmp587->f1=(*y).f1,_tmp587->f2=(*y).f2;_tmp587;});_tmp588->f1=_tmpCAA;}),_tmp588->f2=(*y).f3;_tmp588;});}
# 3260
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3262
struct _tuple10 _tmp589=*orig_arg;struct _tuple10 _tmp58A=_tmp589;struct _dyneither_ptr*_tmp58E;struct Cyc_Absyn_Tqual _tmp58D;void*_tmp58C;_LL1: _tmp58E=_tmp58A.f1;_tmp58D=_tmp58A.f2;_tmp58C=_tmp58A.f3;_LL2:;
if(t == _tmp58C)return orig_arg;
return({struct _tuple10*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->f1=_tmp58E,_tmp58B->f2=_tmp58D,_tmp58B->f3=t;_tmp58B;});}
# 3266
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3269
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3274
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3276
return({struct Cyc_Absyn_Exp*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->topt=old->topt,_tmp58F->r=r,_tmp58F->loc=old->loc,_tmp58F->annot=old->annot;_tmp58F;});}
# 3281
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp590=e->r;void*_tmp591=_tmp590;void*_tmp5CE;void*_tmp5CD;struct Cyc_List_List*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CB;void*_tmp5CA;void*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5C8;int _tmp5C7;enum Cyc_Absyn_Coercion _tmp5C6;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C4;struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C1;struct Cyc_Absyn_Exp*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5BD;enum Cyc_Absyn_Primop _tmp5BC;struct Cyc_List_List*_tmp5BB;switch(*((int*)_tmp591)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp5BC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_tmp5BB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp591)->f2;_LLC:
# 3290
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5BB)== 1){
struct Cyc_Absyn_Exp*_tmp592=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BB))->hd;
struct Cyc_Absyn_Exp*_tmp593=Cyc_Tcutil_rsubsexp(r,inst,_tmp592);
if(_tmp593 == _tmp592)return e;
return({struct Cyc_Absyn_Exp*_tmpCAC=e;Cyc_Tcutil_copye(_tmpCAC,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->tag=3U,_tmp595->f1=_tmp5BC,({struct Cyc_List_List*_tmpCAB=({struct Cyc_Absyn_Exp*_tmp594[1U];_tmp594[0]=_tmp593;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp594,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp595->f2=_tmpCAB;});_tmp595;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5BB)== 2){
struct Cyc_Absyn_Exp*_tmp596=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BB))->hd;
struct Cyc_Absyn_Exp*_tmp597=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BB->tl))->hd;
struct Cyc_Absyn_Exp*_tmp598=Cyc_Tcutil_rsubsexp(r,inst,_tmp596);
struct Cyc_Absyn_Exp*_tmp599=Cyc_Tcutil_rsubsexp(r,inst,_tmp597);
if(_tmp598 == _tmp596  && _tmp599 == _tmp597)return e;
return({struct Cyc_Absyn_Exp*_tmpCAE=e;Cyc_Tcutil_copye(_tmpCAE,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->tag=3U,_tmp59B->f1=_tmp5BC,({struct Cyc_List_List*_tmpCAD=({struct Cyc_Absyn_Exp*_tmp59A[2U];_tmp59A[0]=_tmp598,_tmp59A[1]=_tmp599;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59A,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp59B->f2=_tmpCAD;});_tmp59B;}));});}else{
return({void*_tmp59C=0U;({struct _dyneither_ptr _tmpCAF=({const char*_tmp59D="primop does not have 1 or 2 args!";_tag_dyneither(_tmp59D,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCAF,_tag_dyneither(_tmp59C,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp5BF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_tmp5BE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp591)->f2;_tmp5BD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp591)->f3;_LLE: {
# 3304
struct Cyc_Absyn_Exp*_tmp59E=Cyc_Tcutil_rsubsexp(r,inst,_tmp5BF);
struct Cyc_Absyn_Exp*_tmp59F=Cyc_Tcutil_rsubsexp(r,inst,_tmp5BE);
struct Cyc_Absyn_Exp*_tmp5A0=Cyc_Tcutil_rsubsexp(r,inst,_tmp5BD);
if((_tmp59E == _tmp5BF  && _tmp59F == _tmp5BE) && _tmp5A0 == _tmp5BD)return e;
return({struct Cyc_Absyn_Exp*_tmpCB0=e;Cyc_Tcutil_copye(_tmpCB0,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->tag=6U,_tmp5A1->f1=_tmp59E,_tmp5A1->f2=_tmp59F,_tmp5A1->f3=_tmp5A0;_tmp5A1;}));});}case 7U: _LLF: _tmp5C1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_tmp5C0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp591)->f2;_LL10: {
# 3310
struct Cyc_Absyn_Exp*_tmp5A2=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C1);
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C0);
if(_tmp5A2 == _tmp5C1  && _tmp5A3 == _tmp5C0)return e;
return({struct Cyc_Absyn_Exp*_tmpCB1=e;Cyc_Tcutil_copye(_tmpCB1,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->tag=7U,_tmp5A4->f1=_tmp5A2,_tmp5A4->f2=_tmp5A3;_tmp5A4;}));});}case 8U: _LL11: _tmp5C3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_tmp5C2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp591)->f2;_LL12: {
# 3315
struct Cyc_Absyn_Exp*_tmp5A5=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C3);
struct Cyc_Absyn_Exp*_tmp5A6=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C2);
if(_tmp5A5 == _tmp5C3  && _tmp5A6 == _tmp5C2)return e;
return({struct Cyc_Absyn_Exp*_tmpCB2=e;Cyc_Tcutil_copye(_tmpCB2,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->tag=8U,_tmp5A7->f1=_tmp5A5,_tmp5A7->f2=_tmp5A6;_tmp5A7;}));});}case 9U: _LL13: _tmp5C5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_tmp5C4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp591)->f2;_LL14: {
# 3320
struct Cyc_Absyn_Exp*_tmp5A8=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C5);
struct Cyc_Absyn_Exp*_tmp5A9=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C4);
if(_tmp5A8 == _tmp5C5  && _tmp5A9 == _tmp5C4)return e;
return({struct Cyc_Absyn_Exp*_tmpCB3=e;Cyc_Tcutil_copye(_tmpCB3,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->tag=9U,_tmp5AA->f1=_tmp5A8,_tmp5AA->f2=_tmp5A9;_tmp5AA;}));});}case 14U: _LL15: _tmp5C9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_tmp5C8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp591)->f2;_tmp5C7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp591)->f3;_tmp5C6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp591)->f4;_LL16: {
# 3325
struct Cyc_Absyn_Exp*_tmp5AB=Cyc_Tcutil_rsubsexp(r,inst,_tmp5C8);
void*_tmp5AC=Cyc_Tcutil_rsubstitute(r,inst,_tmp5C9);
if(_tmp5AB == _tmp5C8  && _tmp5AC == _tmp5C9)return e;
return({struct Cyc_Absyn_Exp*_tmpCB4=e;Cyc_Tcutil_copye(_tmpCB4,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));_tmp5AD->tag=14U,_tmp5AD->f1=_tmp5AC,_tmp5AD->f2=_tmp5AB,_tmp5AD->f3=_tmp5C7,_tmp5AD->f4=_tmp5C6;_tmp5AD;}));});}case 17U: _LL17: _tmp5CA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_LL18: {
# 3330
void*_tmp5AE=Cyc_Tcutil_rsubstitute(r,inst,_tmp5CA);
if(_tmp5AE == _tmp5CA)return e;
return({struct Cyc_Absyn_Exp*_tmpCB5=e;Cyc_Tcutil_copye(_tmpCB5,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->tag=17U,_tmp5AF->f1=_tmp5AE;_tmp5AF;}));});}case 18U: _LL19: _tmp5CB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_LL1A: {
# 3334
struct Cyc_Absyn_Exp*_tmp5B0=Cyc_Tcutil_rsubsexp(r,inst,_tmp5CB);
if(_tmp5B0 == _tmp5CB)return e;
return({struct Cyc_Absyn_Exp*_tmpCB6=e;Cyc_Tcutil_copye(_tmpCB6,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->tag=18U,_tmp5B1->f1=_tmp5B0;_tmp5B1;}));});}case 19U: _LL1B: _tmp5CD=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_tmp5CC=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp591)->f2;_LL1C: {
# 3338
void*_tmp5B2=Cyc_Tcutil_rsubstitute(r,inst,_tmp5CD);
if(_tmp5B2 == _tmp5CD)return e;
return({struct Cyc_Absyn_Exp*_tmpCB7=e;Cyc_Tcutil_copye(_tmpCB7,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->tag=19U,_tmp5B3->f1=_tmp5B2,_tmp5B3->f2=_tmp5CC;_tmp5B3;}));});}case 39U: _LL1D: _tmp5CE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp591)->f1;_LL1E: {
# 3342
void*_tmp5B4=Cyc_Tcutil_rsubstitute(r,inst,_tmp5CE);
if(_tmp5B4 == _tmp5CE)return e;{
# 3345
void*_tmp5B5=Cyc_Tcutil_compress(_tmp5B4);void*_tmp5B6=_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B8;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5B6)->tag == 18U){_LL22: _tmp5B8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5B6)->f1;_LL23:
 return _tmp5B8;}else{_LL24: _LL25:
# 3348
 return({struct Cyc_Absyn_Exp*_tmpCB8=e;Cyc_Tcutil_copye(_tmpCB8,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->tag=39U,_tmp5B7->f1=_tmp5B4;_tmp5B7;}));});}_LL21:;};}default: _LL1F: _LL20:
# 3351
 return({void*_tmp5B9=0U;({struct _dyneither_ptr _tmpCB9=({const char*_tmp5BA="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp5BA,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB9,_tag_dyneither(_tmp5B9,sizeof(void*),0U));});});}_LL0:;}
# 3355
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3358
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3362
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3365
void*_tmp5CF=f->type;
struct Cyc_Absyn_Exp*_tmp5D0=f->requires_clause;
void*_tmp5D1=Cyc_Tcutil_rsubstitute(r,inst,_tmp5CF);
struct Cyc_Absyn_Exp*_tmp5D2=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp5D0);
if(_tmp5CF == _tmp5D1  && _tmp5D0 == _tmp5D2)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->name=f->name,_tmp5D3->tq=f->tq,_tmp5D3->type=_tmp5D1,_tmp5D3->width=f->width,_tmp5D3->attributes=f->attributes,_tmp5D3->requires_clause=_tmp5D2;_tmp5D3;});}}
# 3375
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3378
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp5D4=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp5D5=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp5D6=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp5D4);
struct Cyc_List_List*_tmp5D7=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp5D5);
if(_tmp5D6 == _tmp5D4  && _tmp5D7 == _tmp5D5)return fs;
# 3385
return({struct Cyc_List_List*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->hd=_tmp5D6,_tmp5D8->tl=_tmp5D7;_tmp5D8;});};}
# 3388
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3391
struct _tuple1 _tmp5D9=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp5DA=_tmp5D9;struct Cyc_List_List*_tmp5DE;struct Cyc_List_List*_tmp5DD;_LL1: _tmp5DE=_tmp5DA.f1;_tmp5DD=_tmp5DA.f2;_LL2:;{
struct Cyc_List_List*_tmp5DB=Cyc_Tcutil_substs(rgn,inst,_tmp5DE);
struct Cyc_List_List*_tmp5DC=Cyc_Tcutil_substs(rgn,inst,_tmp5DD);
if(_tmp5DB == _tmp5DE  && _tmp5DC == _tmp5DD)
return rgn_po;else{
# 3397
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp5DB,_tmp5DC);}};}
# 3400
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3403
void*_tmp5DF=Cyc_Tcutil_compress(t);void*_tmp5E0=_tmp5DF;struct Cyc_List_List*_tmp658;void*_tmp657;void*_tmp656;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Exp*_tmp654;void*_tmp653;void*_tmp652;void*_tmp651;void*_tmp650;void*_tmp64F;enum Cyc_Absyn_AggrKind _tmp64E;struct Cyc_List_List*_tmp64D;struct Cyc_List_List*_tmp64C;struct Cyc_List_List*_tmp64B;void*_tmp64A;struct Cyc_Absyn_Tqual _tmp649;void*_tmp648;struct Cyc_List_List*_tmp647;int _tmp646;struct Cyc_Absyn_VarargInfo*_tmp645;struct Cyc_List_List*_tmp644;struct Cyc_List_List*_tmp643;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Absyn_Exp*_tmp641;void*_tmp640;struct Cyc_Absyn_Tqual _tmp63F;void*_tmp63E;union Cyc_Absyn_Constraint*_tmp63D;union Cyc_Absyn_Constraint*_tmp63C;union Cyc_Absyn_Constraint*_tmp63B;void*_tmp63A;struct Cyc_Absyn_Tqual _tmp639;struct Cyc_Absyn_Exp*_tmp638;union Cyc_Absyn_Constraint*_tmp637;unsigned int _tmp636;struct _tuple2*_tmp635;struct Cyc_List_List*_tmp634;struct Cyc_Absyn_Typedefdecl*_tmp633;void*_tmp632;union Cyc_Absyn_DatatypeFieldInfoU _tmp631;struct Cyc_List_List*_tmp630;union Cyc_Absyn_DatatypeInfoU _tmp62F;struct Cyc_List_List*_tmp62E;union Cyc_Absyn_AggrInfoU _tmp62D;struct Cyc_List_List*_tmp62C;struct Cyc_Absyn_Tvar*_tmp62B;switch(*((int*)_tmp5E0)){case 2U: _LL1: _tmp62B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E0)->f1;_LL2: {
# 3406
struct _handler_cons _tmp5E1;_push_handler(& _tmp5E1);{int _tmp5E3=0;if(setjmp(_tmp5E1.handler))_tmp5E3=1;if(!_tmp5E3){{void*_tmp5E4=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp62B);_npop_handler(0U);return _tmp5E4;};_pop_handler();}else{void*_tmp5E2=(void*)_exn_thrown;void*_tmp5E5=_tmp5E2;void*_tmp5E6;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp5E5)->tag == Cyc_Core_Not_found){_LL3C: _LL3D:
 return t;}else{_LL3E: _tmp5E6=_tmp5E5;_LL3F:(int)_rethrow(_tmp5E6);}_LL3B:;}};}case 11U: _LL3: _tmp62D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E0)->f1).aggr_info;_tmp62C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5E0)->f1).targs;_LL4: {
# 3409
struct Cyc_List_List*_tmp5E7=Cyc_Tcutil_substs(rgn,inst,_tmp62C);
return _tmp5E7 == _tmp62C?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->tag=11U,(_tmp5E8->f1).aggr_info=_tmp62D,(_tmp5E8->f1).targs=_tmp5E7;_tmp5E8;});}case 3U: _LL5: _tmp62F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E0)->f1).datatype_info;_tmp62E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E0)->f1).targs;_LL6: {
# 3412
struct Cyc_List_List*_tmp5E9=Cyc_Tcutil_substs(rgn,inst,_tmp62E);
return _tmp5E9 == _tmp62E?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));
_tmp5EA->tag=3U,(_tmp5EA->f1).datatype_info=_tmp62F,(_tmp5EA->f1).targs=_tmp5E9;_tmp5EA;});}case 4U: _LL7: _tmp631=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E0)->f1).field_info;_tmp630=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5E0)->f1).targs;_LL8: {
# 3416
struct Cyc_List_List*_tmp5EB=Cyc_Tcutil_substs(rgn,inst,_tmp630);
return _tmp5EB == _tmp630?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));
_tmp5EC->tag=4U,(_tmp5EC->f1).field_info=_tmp631,(_tmp5EC->f1).targs=_tmp5EB;_tmp5EC;});}case 17U: _LL9: _tmp635=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E0)->f1;_tmp634=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E0)->f2;_tmp633=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E0)->f3;_tmp632=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E0)->f4;_LLA: {
# 3420
struct Cyc_List_List*_tmp5ED=Cyc_Tcutil_substs(rgn,inst,_tmp634);
return _tmp5ED == _tmp634?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->tag=17U,_tmp5EE->f1=_tmp635,_tmp5EE->f2=_tmp5ED,_tmp5EE->f3=_tmp633,_tmp5EE->f4=_tmp632;_tmp5EE;});}case 8U: _LLB: _tmp63A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).elt_type;_tmp639=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).tq;_tmp638=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).num_elts;_tmp637=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).zero_term;_tmp636=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E0)->f1).zt_loc;_LLC: {
# 3423
void*_tmp5EF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63A);
struct Cyc_Absyn_Exp*_tmp5F0=_tmp638 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp638);
return _tmp5EF == _tmp63A  && _tmp5F0 == _tmp638?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));
_tmp5F1->tag=8U,(_tmp5F1->f1).elt_type=_tmp5EF,(_tmp5F1->f1).tq=_tmp639,(_tmp5F1->f1).num_elts=_tmp5F0,(_tmp5F1->f1).zero_term=_tmp637,(_tmp5F1->f1).zt_loc=_tmp636;_tmp5F1;});}case 5U: _LLD: _tmp640=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E0)->f1).elt_typ;_tmp63F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E0)->f1).elt_tq;_tmp63E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E0)->f1).ptr_atts).rgn;_tmp63D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E0)->f1).ptr_atts).nullable;_tmp63C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E0)->f1).ptr_atts).bounds;_tmp63B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E0)->f1).ptr_atts).zero_term;_LLE: {
# 3428
void*_tmp5F2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp640);
void*_tmp5F3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63E);
union Cyc_Absyn_Constraint*_tmp5F4=_tmp63C;
{void*_tmp5F5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp63C);void*_tmp5F6=_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F9;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F6)->tag == 1U){_LL41: _tmp5F9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F6)->f1;_LL42: {
# 3433
struct Cyc_Absyn_Exp*_tmp5F7=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5F9);
if(_tmp5F7 != _tmp5F9)
({union Cyc_Absyn_Constraint*_tmpCBA=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->tag=1U,_tmp5F8->f1=_tmp5F7;_tmp5F8;}));_tmp5F4=_tmpCBA;});
goto _LL40;}}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3439
if((_tmp5F2 == _tmp640  && _tmp5F3 == _tmp63E) && _tmp5F4 == _tmp63C)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->tag=5U,(_tmp5FA->f1).elt_typ=_tmp5F2,(_tmp5FA->f1).elt_tq=_tmp63F,((_tmp5FA->f1).ptr_atts).rgn=_tmp5F3,((_tmp5FA->f1).ptr_atts).nullable=_tmp63D,((_tmp5FA->f1).ptr_atts).bounds=_tmp5F4,((_tmp5FA->f1).ptr_atts).zero_term=_tmp63B,((_tmp5FA->f1).ptr_atts).ptrloc=0;_tmp5FA;});}case 9U: _LLF: _tmp64B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).tvars;_tmp64A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).effect;_tmp649=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).ret_tqual;_tmp648=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).ret_typ;_tmp647=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).args;_tmp646=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).c_varargs;_tmp645=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).cyc_varargs;_tmp644=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).rgn_po;_tmp643=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).attributes;_tmp642=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).requires_clause;_tmp641=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E0)->f1).ensures_clause;_LL10:
# 3445
{struct Cyc_List_List*_tmp5FB=_tmp64B;for(0;_tmp5FB != 0;_tmp5FB=_tmp5FB->tl){
({struct Cyc_List_List*_tmpCBD=({struct Cyc_List_List*_tmp5FE=_region_malloc(rgn,sizeof(*_tmp5FE));({struct _tuple16*_tmpCBC=({struct _tuple16*_tmp5FD=_region_malloc(rgn,sizeof(*_tmp5FD));_tmp5FD->f1=(struct Cyc_Absyn_Tvar*)_tmp5FB->hd,({void*_tmpCBB=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->tag=2U,_tmp5FC->f1=(struct Cyc_Absyn_Tvar*)_tmp5FB->hd;_tmp5FC;});_tmp5FD->f2=_tmpCBB;});_tmp5FD;});_tmp5FE->hd=_tmpCBC;}),_tmp5FE->tl=inst;_tmp5FE;});inst=_tmpCBD;});}}{
struct _tuple1 _tmp5FF=({struct _RegionHandle*_tmpCBF=rgn;struct _RegionHandle*_tmpCBE=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpCBF,_tmpCBE,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp647));});
# 3447
struct _tuple1 _tmp600=_tmp5FF;struct Cyc_List_List*_tmp60F;struct Cyc_List_List*_tmp60E;_LL46: _tmp60F=_tmp600.f1;_tmp60E=_tmp600.f2;_LL47:;{
# 3449
struct Cyc_List_List*_tmp601=_tmp647;
struct Cyc_List_List*_tmp602=Cyc_Tcutil_substs(rgn,inst,_tmp60E);
if(_tmp602 != _tmp60E)
({struct Cyc_List_List*_tmpCC0=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp647,_tmp602);_tmp601=_tmpCC0;});{
void*eff2;
if(_tmp64A == 0)
eff2=0;else{
# 3457
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64A);
if(new_eff == _tmp64A)
eff2=_tmp64A;else{
# 3461
eff2=new_eff;}}{
# 3463
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp645 == 0)
cyc_varargs2=0;else{
# 3467
struct Cyc_Absyn_VarargInfo _tmp603=*_tmp645;struct Cyc_Absyn_VarargInfo _tmp604=_tmp603;struct _dyneither_ptr*_tmp60A;struct Cyc_Absyn_Tqual _tmp609;void*_tmp608;int _tmp607;_LL49: _tmp60A=_tmp604.name;_tmp609=_tmp604.tq;_tmp608=_tmp604.type;_tmp607=_tmp604.inject;_LL4A:;{
void*_tmp605=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp608);
if(_tmp605 == _tmp608)cyc_varargs2=_tmp645;else{
# 3471
({struct Cyc_Absyn_VarargInfo*_tmpCC1=({struct Cyc_Absyn_VarargInfo*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->name=_tmp60A,_tmp606->tq=_tmp609,_tmp606->type=_tmp605,_tmp606->inject=_tmp607;_tmp606;});cyc_varargs2=_tmpCC1;});}};}{
# 3473
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp644);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp642);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp641);
struct Cyc_List_List*_tmp60B=Cyc_Tcutil_extract_relns(_tmp601,req2);
struct Cyc_List_List*_tmp60C=Cyc_Tcutil_extract_relns(_tmp601,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));
_tmp60D->tag=9U,(_tmp60D->f1).tvars=_tmp64B,(_tmp60D->f1).effect=eff2,(_tmp60D->f1).ret_tqual=_tmp649,({void*_tmpCC2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp648);(_tmp60D->f1).ret_typ=_tmpCC2;}),(_tmp60D->f1).args=_tmp601,(_tmp60D->f1).c_varargs=_tmp646,(_tmp60D->f1).cyc_varargs=cyc_varargs2,(_tmp60D->f1).rgn_po=rgn_po2,(_tmp60D->f1).attributes=_tmp643,(_tmp60D->f1).requires_clause=req2,(_tmp60D->f1).requires_relns=_tmp60B,(_tmp60D->f1).ensures_clause=ens2,(_tmp60D->f1).ensures_relns=_tmp60C;_tmp60D;});};};};};};case 10U: _LL11: _tmp64C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E0)->f1;_LL12: {
# 3483
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp610=_tmp64C;for(0;_tmp610 != 0;_tmp610=_tmp610->tl){
void*_tmp611=(*((struct _tuple12*)_tmp610->hd)).f2;
void*_tmp612=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp611);
if(_tmp611 != _tmp612)
change=1;
# 3491
({struct Cyc_List_List*_tmpCC3=({struct Cyc_List_List*_tmp613=_region_malloc(rgn,sizeof(*_tmp613));_tmp613->hd=_tmp612,_tmp613->tl=ts2;_tmp613;});ts2=_tmpCC3;});}}
# 3493
if(!change)
return t;{
struct Cyc_List_List*_tmp614=({struct Cyc_List_List*_tmpCC4=_tmp64C;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpCC4,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=10U,_tmp615->f1=_tmp614;_tmp615;});};}case 12U: _LL13: _tmp64E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E0)->f1;_tmp64D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E0)->f2;_LL14: {
# 3498
struct Cyc_List_List*_tmp616=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp64D);
if(_tmp64D == _tmp616)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->tag=12U,_tmp617->f1=_tmp64E,_tmp617->f2=_tmp616;_tmp617;});}case 1U: _LL15: _tmp64F=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E0)->f2;_LL16:
# 3502
 if(_tmp64F != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp64F);else{
return t;}case 15U: _LL17: _tmp650=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5E0)->f1;_LL18: {
# 3505
void*_tmp618=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp650);
return _tmp618 == _tmp650?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=15U,_tmp619->f1=_tmp618;_tmp619;});}case 16U: _LL19: _tmp652=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E0)->f1;_tmp651=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5E0)->f2;_LL1A: {
# 3508
void*_tmp61A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp652);
void*_tmp61B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp651);
return _tmp61A == _tmp652  && _tmp61B == _tmp651?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tag=16U,_tmp61C->f1=_tmp61A,_tmp61C->f2=_tmp61B;_tmp61C;});}case 19U: _LL1B: _tmp653=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E0)->f1;_LL1C: {
# 3512
void*_tmp61D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp653);
return _tmp61D == _tmp653?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->tag=19U,_tmp61E->f1=_tmp61D;_tmp61E;});}case 18U: _LL1D: _tmp654=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E0)->f1;_LL1E: {
# 3515
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp654);
return _tmp61F == _tmp654?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tag=18U,_tmp620->f1=_tmp61F;_tmp620;});}case 27U: _LL1F: _tmp655=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E0)->f1;_LL20: {
# 3518
struct Cyc_Absyn_Exp*_tmp621=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp655);
return _tmp621 == _tmp655?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->tag=27U,_tmp622->f1=_tmp621;_tmp622;});}case 13U: _LL21: _LL22:
 goto _LL24;case 14U: _LL23: _LL24:
 goto _LL26;case 0U: _LL25: _LL26:
 goto _LL28;case 6U: _LL27: _LL28:
 goto _LL2A;case 7U: _LL29: _LL2A:
 goto _LL2C;case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 28U: _LL2F: _LL30:
 goto _LL32;case 20U: _LL31: _LL32:
 return t;case 25U: _LL33: _tmp656=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5E0)->f1;_LL34: {
# 3530
void*_tmp623=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp656);
return _tmp623 == _tmp656?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=25U,_tmp624->f1=_tmp623;_tmp624;});}case 23U: _LL35: _tmp657=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E0)->f1;_LL36: {
# 3533
void*_tmp625=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp657);
return _tmp625 == _tmp657?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tag=23U,_tmp626->f1=_tmp625;_tmp626;});}case 24U: _LL37: _tmp658=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E0)->f1;_LL38: {
# 3536
struct Cyc_List_List*_tmp627=Cyc_Tcutil_substs(rgn,inst,_tmp658);
return _tmp627 == _tmp658?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=24U,_tmp628->f1=_tmp627;_tmp628;});}default: _LL39: _LL3A:
({void*_tmp629=0U;({struct _dyneither_ptr _tmpCC5=({const char*_tmp62A="found typedecltype in rsubs";_tag_dyneither(_tmp62A,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCC5,_tag_dyneither(_tmp629,sizeof(void*),0U));});});}_LL0:;}
# 3542
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3545
if(ts == 0)
return 0;{
void*_tmp659=(void*)ts->hd;
struct Cyc_List_List*_tmp65A=ts->tl;
void*_tmp65B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp659);
struct Cyc_List_List*_tmp65C=Cyc_Tcutil_substs(rgn,inst,_tmp65A);
if(_tmp659 == _tmp65B  && _tmp65A == _tmp65C)
return ts;
return({struct Cyc_List_List*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->hd=_tmp65B,_tmp65D->tl=_tmp65C;_tmp65D;});};}
# 3556
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3563
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp65E=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->f1=tv,({void*_tmpCC7=({struct Cyc_Core_Opt*_tmpCC6=_tmp65E;Cyc_Absyn_new_evar(_tmpCC6,({struct Cyc_Core_Opt*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->v=s;_tmp65F;}));});_tmp660->f2=_tmpCC7;});_tmp660;});}
# 3568
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3570
struct _tuple17*_tmp661=env;struct Cyc_List_List*_tmp666;struct _RegionHandle*_tmp665;_LL1: _tmp666=_tmp661->f1;_tmp665=_tmp661->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp662=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp664=_region_malloc(_tmp665,sizeof(*_tmp664));_tmp664->f1=tv,({void*_tmpCC9=({struct Cyc_Core_Opt*_tmpCC8=_tmp662;Cyc_Absyn_new_evar(_tmpCC8,({struct Cyc_Core_Opt*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->v=_tmp666;_tmp663;}));});_tmp664->f2=_tmpCC9;});_tmp664;});};}
# 3580
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3584
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp669=({struct Cyc_String_pa_PrintArg_struct _tmpA79;_tmpA79.tag=0U,_tmpA79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmpA79;});struct Cyc_String_pa_PrintArg_struct _tmp66A=({struct Cyc_String_pa_PrintArg_struct _tmpA78;_tmpA78.tag=0U,({
struct _dyneither_ptr _tmpCCA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmpA78.f1=_tmpCCA;});_tmpA78;});struct Cyc_String_pa_PrintArg_struct _tmp66B=({struct Cyc_String_pa_PrintArg_struct _tmpA77;_tmpA77.tag=0U,({struct _dyneither_ptr _tmpCCB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmpA77.f1=_tmpCCB;});_tmpA77;});void*_tmp667[3U];_tmp667[0]=& _tmp669,_tmp667[1]=& _tmp66A,_tmp667[2]=& _tmp66B;({unsigned int _tmpCCD=loc;struct _dyneither_ptr _tmpCCC=({const char*_tmp668="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp668,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpCCD,_tmpCCC,_tag_dyneither(_tmp667,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp66C=0U;({struct _dyneither_ptr _tmpCCE=({const char*_tmp66D="same type variable has different identity!";_tag_dyneither(_tmp66D,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCCE,_tag_dyneither(_tmp66C,sizeof(void*),0U));});});}
return tvs;}}}
# 3598
({int _tmpCCF=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpCCF;});
return({struct Cyc_List_List*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->hd=tv,_tmp66E->tl=tvs;_tmp66E;});}
# 3604
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3606
if(tv->identity == - 1)
({void*_tmp66F=0U;({struct _dyneither_ptr _tmpCD0=({const char*_tmp670="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp670,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD0,_tag_dyneither(_tmp66F,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3610
struct Cyc_Absyn_Tvar*_tmp671=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp671->identity == - 1)
({void*_tmp672=0U;({struct _dyneither_ptr _tmpCD1=({const char*_tmp673="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp673,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD1,_tag_dyneither(_tmp672,sizeof(void*),0U));});});
if(_tmp671->identity == tv->identity)
return tvs;}}
# 3617
return({struct Cyc_List_List*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->hd=tv,_tmp674->tl=tvs;_tmp674;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3623
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3628
if(tv->identity == - 1)
({void*_tmp675=0U;({struct _dyneither_ptr _tmpCD2=({const char*_tmp676="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp676,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD2,_tag_dyneither(_tmp675,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3632
struct _tuple28*_tmp677=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp678=_tmp677;struct Cyc_Absyn_Tvar*_tmp67C;int*_tmp67B;_LL1: _tmp67C=_tmp678->f1;_tmp67B=(int*)& _tmp678->f2;_LL2:;
if(_tmp67C->identity == - 1)
({void*_tmp679=0U;({struct _dyneither_ptr _tmpCD3=({const char*_tmp67A="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp67A,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD3,_tag_dyneither(_tmp679,sizeof(void*),0U));});});
if(_tmp67C->identity == tv->identity){
*_tmp67B=*_tmp67B  || b;
return tvs;}}}
# 3640
return({struct Cyc_List_List*_tmp67E=_region_malloc(r,sizeof(*_tmp67E));({struct _tuple28*_tmpCD4=({struct _tuple28*_tmp67D=_region_malloc(r,sizeof(*_tmp67D));_tmp67D->f1=tv,_tmp67D->f2=b;_tmp67D;});_tmp67E->hd=_tmpCD4;}),_tmp67E->tl=tvs;_tmp67E;});}
# 3644
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3646
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp681=({struct Cyc_String_pa_PrintArg_struct _tmpA7A;_tmpA7A.tag=0U,({struct _dyneither_ptr _tmpCD5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmpA7A.f1=_tmpCD5;});_tmpA7A;});void*_tmp67F[1U];_tmp67F[0]=& _tmp681;({struct _dyneither_ptr _tmpCD6=({const char*_tmp680="bound tvar id for %s is NULL";_tag_dyneither(_tmp680,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD6,_tag_dyneither(_tmp67F,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->hd=tv,_tmp682->tl=tvs;_tmp682;});}struct _tuple29{void*f1;int f2;};
# 3655
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3658
void*_tmp683=Cyc_Tcutil_compress(e);void*_tmp684=_tmp683;int _tmp68E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp684)->tag == 1U){_LL1: _tmp68E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp684)->f3;_LL2:
# 3660
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp685=(struct _tuple29*)es2->hd;struct _tuple29*_tmp686=_tmp685;void*_tmp68B;int*_tmp68A;_LL6: _tmp68B=_tmp686->f1;_tmp68A=(int*)& _tmp686->f2;_LL7:;{
void*_tmp687=Cyc_Tcutil_compress(_tmp68B);void*_tmp688=_tmp687;int _tmp689;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp688)->tag == 1U){_LL9: _tmp689=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp688)->f3;_LLA:
# 3664
 if(_tmp68E == _tmp689){
if(b != *_tmp68A)*_tmp68A=1;
return es;}
# 3668
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3672
return({struct Cyc_List_List*_tmp68D=_region_malloc(r,sizeof(*_tmp68D));({struct _tuple29*_tmpCD7=({struct _tuple29*_tmp68C=_region_malloc(r,sizeof(*_tmp68C));_tmp68C->f1=e,_tmp68C->f2=b;_tmp68C;});_tmp68D->hd=_tmpCD7;}),_tmp68D->tl=es;_tmp68D;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3677
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3680
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3689
if(!present)({struct Cyc_List_List*_tmpCD8=({struct Cyc_List_List*_tmp68F=_region_malloc(rgn,sizeof(*_tmp68F));_tmp68F->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp68F->tl=r;_tmp68F;});r=_tmpCD8;});}
# 3691
({struct Cyc_List_List*_tmpCD9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpCD9;});
return r;}
# 3696
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3700
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp690=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp691=_tmp690;struct Cyc_Absyn_Tvar*_tmp694;int _tmp693;_LL1: _tmp694=_tmp691.f1;_tmp693=_tmp691.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp694->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3710
if(!present)({struct Cyc_List_List*_tmpCDA=({struct Cyc_List_List*_tmp692=_region_malloc(r,sizeof(*_tmp692));_tmp692->hd=(struct _tuple28*)tvs->hd,_tmp692->tl=res;_tmp692;});res=_tmpCDA;});};}
# 3712
({struct Cyc_List_List*_tmpCDB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpCDB;});
return res;}
# 3716
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3718
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp697=({struct Cyc_String_pa_PrintArg_struct _tmpA7B;_tmpA7B.tag=0U,_tmpA7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA7B;});void*_tmp695[1U];_tmp695[0]=& _tmp697;({unsigned int _tmpCDD=loc;struct _dyneither_ptr _tmpCDC=({const char*_tmp696="bitfield %s does not have constant width";_tag_dyneither(_tmp696,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpCDD,_tmpCDC,_tag_dyneither(_tmp695,sizeof(void*),1U));});});else{
# 3723
struct _tuple14 _tmp698=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp699=_tmp698;unsigned int _tmp69D;int _tmp69C;_LL1: _tmp69D=_tmp699.f1;_tmp69C=_tmp699.f2;_LL2:;
if(!_tmp69C)
({void*_tmp69A=0U;({unsigned int _tmpCDF=loc;struct _dyneither_ptr _tmpCDE=({const char*_tmp69B="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp69B,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpCDF,_tmpCDE,_tag_dyneither(_tmp69A,sizeof(void*),0U));});});
w=_tmp69D;}{
# 3728
void*_tmp69E=Cyc_Tcutil_compress(field_typ);void*_tmp69F=_tmp69E;enum Cyc_Absyn_Size_of _tmp6AD;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp69F)->tag == 6U){_LL4: _tmp6AD=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp69F)->f2;_LL5:
# 3731
{enum Cyc_Absyn_Size_of _tmp6A0=_tmp6AD;switch(_tmp6A0){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > 8)({void*_tmp6A1=0U;({unsigned int _tmpCE1=loc;struct _dyneither_ptr _tmpCE0=({const char*_tmp6A2="bitfield larger than type";_tag_dyneither(_tmp6A2,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCE1,_tmpCE0,_tag_dyneither(_tmp6A1,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > 16)({void*_tmp6A3=0U;({unsigned int _tmpCE3=loc;struct _dyneither_ptr _tmpCE2=({const char*_tmp6A4="bitfield larger than type";_tag_dyneither(_tmp6A4,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCE3,_tmpCE2,_tag_dyneither(_tmp6A3,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3736
 if(w > 32)({void*_tmp6A5=0U;({unsigned int _tmpCE5=loc;struct _dyneither_ptr _tmpCE4=({const char*_tmp6A6="bitfield larger than type";_tag_dyneither(_tmp6A6,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCE5,_tmpCE4,_tag_dyneither(_tmp6A5,sizeof(void*),0U));});});goto _LL8;default: _LL11: _LL12:
# 3738
 if(w > 64)({void*_tmp6A7=0U;({unsigned int _tmpCE7=loc;struct _dyneither_ptr _tmpCE6=({const char*_tmp6A8="bitfield larger than type";_tag_dyneither(_tmp6A8,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCE7,_tmpCE6,_tag_dyneither(_tmp6A7,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3740
goto _LL3;}else{_LL6: _LL7:
# 3742
({struct Cyc_String_pa_PrintArg_struct _tmp6AB=({struct Cyc_String_pa_PrintArg_struct _tmpA7D;_tmpA7D.tag=0U,_tmpA7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA7D;});struct Cyc_String_pa_PrintArg_struct _tmp6AC=({struct Cyc_String_pa_PrintArg_struct _tmpA7C;_tmpA7C.tag=0U,({
struct _dyneither_ptr _tmpCE8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ));_tmpA7C.f1=_tmpCE8;});_tmpA7C;});void*_tmp6A9[2U];_tmp6A9[0]=& _tmp6AB,_tmp6A9[1]=& _tmp6AC;({unsigned int _tmpCEA=loc;struct _dyneither_ptr _tmpCE9=({const char*_tmp6AA="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp6AA,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpCEA,_tmpCE9,_tag_dyneither(_tmp6A9,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3749
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp6AE=(void*)atts->hd;void*_tmp6AF=_tmp6AE;switch(*((int*)_tmp6AF)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp6B2=({struct Cyc_String_pa_PrintArg_struct _tmpA7F;_tmpA7F.tag=0U,({
struct _dyneither_ptr _tmpCEB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmpA7F.f1=_tmpCEB;});_tmpA7F;});struct Cyc_String_pa_PrintArg_struct _tmp6B3=({struct Cyc_String_pa_PrintArg_struct _tmpA7E;_tmpA7E.tag=0U,_tmpA7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA7E;});void*_tmp6B0[2U];_tmp6B0[0]=& _tmp6B2,_tmp6B0[1]=& _tmp6B3;({unsigned int _tmpCED=loc;struct _dyneither_ptr _tmpCEC=({const char*_tmp6B1="bad attribute %s on member %s";_tag_dyneither(_tmp6B1,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpCED,_tmpCEC,_tag_dyneither(_tmp6B0,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3777
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp6B4=t;struct Cyc_Absyn_Typedefdecl*_tmp6B8;void*_tmp6B7;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B4)->tag == 17U){_LL1: _tmp6B8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B4)->f3;_tmp6B7=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B4)->f4;_LL2:
# 3780
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp6B8))->tq).real_const  || (_tmp6B8->tq).print_const){
if(declared_const)({void*_tmp6B5=0U;({unsigned int _tmpCEF=loc;struct _dyneither_ptr _tmpCEE=({const char*_tmp6B6="extra const";_tag_dyneither(_tmp6B6,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpCEF,_tmpCEE,_tag_dyneither(_tmp6B5,sizeof(void*),0U));});});
return 1;}
# 3785
if((unsigned int)_tmp6B7)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp6B7);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3792
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp6B9=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp6BA=_tmp6B9;void*_tmp6BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BA)->tag == 5U){_LL1: _tmp6BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BA)->f1).ptr_atts).rgn;_LL2:
# 3797
{void*_tmp6BB=Cyc_Tcutil_compress(_tmp6BE);void*_tmp6BC=_tmp6BB;struct Cyc_Absyn_Tvar*_tmp6BD;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6BC)->tag == 2U){_LL6: _tmp6BD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6BC)->f1;_LL7:
# 3799
 return Cyc_Absyn_tvar_cmp(tvar,_tmp6BD)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3802
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3808
return 1;}
return 0;}
# 3812
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3815
void*_tmp6BF=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp6C0=_tmp6BF;switch(*((int*)_tmp6C0)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6C0)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6C0)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp6C0)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp6C0)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3824
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3827
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3833
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3839
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3843
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3846
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp6C3=({struct Cyc_String_pa_PrintArg_struct _tmpA81;_tmpA81.tag=0U,_tmpA81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmpA81;});struct Cyc_String_pa_PrintArg_struct _tmp6C4=({struct Cyc_String_pa_PrintArg_struct _tmpA80;_tmpA80.tag=0U,({
struct _dyneither_ptr _tmpCF0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmpA80.f1=_tmpCF0;});_tmpA80;});void*_tmp6C1[2U];_tmp6C1[0]=& _tmp6C3,_tmp6C1[1]=& _tmp6C4;({unsigned int _tmpCF2=loc;struct _dyneither_ptr _tmpCF1=({const char*_tmp6C2="%s clause has type %s instead of integral type";_tag_dyneither(_tmp6C2,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCF2,_tmpCF1,_tag_dyneither(_tmp6C1,sizeof(void*),2U));});});
({struct Cyc_Tcutil_CVTEnv _tmpCF3=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpCF3;});
({struct Cyc_List_List*_tmpCF4=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpCF4;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp6C7=({struct Cyc_String_pa_PrintArg_struct _tmpA83;_tmpA83.tag=0U,_tmpA83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmpA83;});struct Cyc_String_pa_PrintArg_struct _tmp6C8=({struct Cyc_String_pa_PrintArg_struct _tmpA82;_tmpA82.tag=0U,({
struct _dyneither_ptr _tmpCF5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmpA82.f1=_tmpCF5;});_tmpA82;});void*_tmp6C5[2U];_tmp6C5[0]=& _tmp6C7,_tmp6C5[1]=& _tmp6C8;({unsigned int _tmpCF7=clause->loc;struct _dyneither_ptr _tmpCF6=({const char*_tmp6C6="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp6C6,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpCF7,_tmpCF6,_tag_dyneither(_tmp6C5,sizeof(void*),2U));});});}
# 3858
return({struct _tuple30 _tmpA84;_tmpA84.f1=cvtenv,_tmpA84.f2=relns;_tmpA84;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3889 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3897
{void*_tmp6C9=Cyc_Tcutil_compress(t);void*_tmp6CA=_tmp6C9;struct Cyc_List_List*_tmp857;void*_tmp856;void*_tmp855;void*_tmp854;void*_tmp853;void*_tmp852;struct _tuple2*_tmp851;struct Cyc_List_List**_tmp850;struct Cyc_Absyn_Typedefdecl**_tmp84F;void**_tmp84E;union Cyc_Absyn_AggrInfoU*_tmp84D;struct Cyc_List_List**_tmp84C;enum Cyc_Absyn_AggrKind _tmp84B;struct Cyc_List_List*_tmp84A;struct Cyc_List_List*_tmp849;struct Cyc_List_List**_tmp848;void**_tmp847;struct Cyc_Absyn_Tqual*_tmp846;void*_tmp845;struct Cyc_List_List*_tmp844;int _tmp843;struct Cyc_Absyn_VarargInfo*_tmp842;struct Cyc_List_List*_tmp841;struct Cyc_List_List*_tmp840;struct Cyc_Absyn_Exp*_tmp83F;struct Cyc_List_List**_tmp83E;struct Cyc_Absyn_Exp*_tmp83D;struct Cyc_List_List**_tmp83C;void*_tmp83B;struct Cyc_Absyn_Tqual*_tmp83A;struct Cyc_Absyn_Exp**_tmp839;union Cyc_Absyn_Constraint*_tmp838;unsigned int _tmp837;struct Cyc_Absyn_Exp*_tmp836;struct Cyc_Absyn_Exp*_tmp835;void*_tmp834;void*_tmp833;struct Cyc_Absyn_Tqual*_tmp832;void*_tmp831;union Cyc_Absyn_Constraint*_tmp830;union Cyc_Absyn_Constraint*_tmp82F;union Cyc_Absyn_Constraint*_tmp82E;union Cyc_Absyn_DatatypeFieldInfoU*_tmp82D;struct Cyc_List_List*_tmp82C;union Cyc_Absyn_DatatypeInfoU*_tmp82B;struct Cyc_List_List**_tmp82A;struct _tuple2*_tmp829;struct Cyc_Absyn_Enumdecl**_tmp828;struct Cyc_List_List*_tmp827;void*_tmp826;void***_tmp825;struct Cyc_Absyn_Tvar*_tmp824;struct Cyc_Core_Opt**_tmp823;void**_tmp822;switch(*((int*)_tmp6CA)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp823=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6CA)->f1;_tmp822=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6CA)->f2;_LL4:
# 3901
 if(*_tmp823 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp823))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp823))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpCF8=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp823=_tmpCF8;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3906
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp822=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3909
*_tmp822=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp6CB=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->tag=2U,_tmp6CD->f1=0,_tmp6CD->f2=expected_kind;_tmp6CD;}));
({void*_tmpCF9=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC->tag=2U,_tmp6CC->f1=_tmp6CB;_tmp6CC;});*_tmp822=_tmpCF9;});
_tmp824=_tmp6CB;goto _LL6;}else{
# 3915
({struct Cyc_List_List*_tmpCFA=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpCFA;});}}
# 3917
goto _LL0;case 2U: _LL5: _tmp824=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6CA)->f1;_LL6:
# 3919
{void*_tmp6CE=Cyc_Absyn_compress_kb(_tmp824->kind);void*_tmp6CF=_tmp6CE;struct Cyc_Core_Opt**_tmp6D2;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6CF)->tag == 1U){_LL3C: _tmp6D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6CF)->f1;_LL3D:
# 3921
({struct Cyc_Core_Opt*_tmpCFC=({struct Cyc_Core_Opt*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));({void*_tmpCFB=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->tag=2U,_tmp6D0->f1=0,_tmp6D0->f2=expected_kind;_tmp6D0;});_tmp6D1->v=_tmpCFB;});_tmp6D1;});*_tmp6D2=_tmpCFC;});goto _LL3B;}else{_LL3E: _LL3F:
 goto _LL3B;}_LL3B:;}
# 3926
({struct Cyc_List_List*_tmpCFD=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp824);cvtenv.kind_env=_tmpCFD;});
# 3929
({struct Cyc_List_List*_tmpCFE=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp824,put_in_effect);cvtenv.free_vars=_tmpCFE;});
# 3931
{void*_tmp6D3=Cyc_Absyn_compress_kb(_tmp824->kind);void*_tmp6D4=_tmp6D3;struct Cyc_Core_Opt**_tmp6D8;struct Cyc_Absyn_Kind*_tmp6D7;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6D4)->tag == 2U){_LL41: _tmp6D8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6D4)->f1;_tmp6D7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6D4)->f2;_LL42:
# 3933
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6D7))
({struct Cyc_Core_Opt*_tmpD00=({struct Cyc_Core_Opt*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));({void*_tmpCFF=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5->tag=2U,_tmp6D5->f1=0,_tmp6D5->f2=expected_kind;_tmp6D5;});_tmp6D6->v=_tmpCFF;});_tmp6D6;});*_tmp6D8=_tmpD00;});
goto _LL40;}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3938
goto _LL0;case 26U: _LL7: _tmp826=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CA)->f1)->r;_tmp825=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CA)->f2;_LL8: {
# 3944
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6D9=_tmp826;struct Cyc_Absyn_Datatypedecl*_tmp6DC;struct Cyc_Absyn_Enumdecl*_tmp6DB;struct Cyc_Absyn_Aggrdecl*_tmp6DA;switch(*((int*)_tmp6D9)){case 0U: _LL46: _tmp6DA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6D9)->f1;_LL47:
# 3947
 if(te->in_extern_c_include)
_tmp6DA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6DA);goto _LL45;case 1U: _LL48: _tmp6DB=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6D9)->f1;_LL49:
# 3951
 if(te->in_extern_c_include)
_tmp6DB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6DB);goto _LL45;default: _LL4A: _tmp6DC=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6D9)->f1;_LL4B:
# 3955
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6DC);goto _LL45;}_LL45:;}
# 3957
({void**_tmpD01=({void**_tmp6DD=_cycalloc(sizeof(*_tmp6DD));*_tmp6DD=new_t;_tmp6DD;});*_tmp825=_tmpD01;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14U: _LL9: _tmp827=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6CA)->f1;_LLA: {
# 3963
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp827 != 0;_tmp827=_tmp827->tl){
struct Cyc_Absyn_Enumfield*_tmp6DE=(struct Cyc_Absyn_Enumfield*)_tmp827->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6DE->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6E1=({struct Cyc_String_pa_PrintArg_struct _tmpA85;_tmpA85.tag=0U,_tmpA85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6DE->name).f2);_tmpA85;});void*_tmp6DF[1U];_tmp6DF[0]=& _tmp6E1;({unsigned int _tmpD03=_tmp6DE->loc;struct _dyneither_ptr _tmpD02=({const char*_tmp6E0="duplicate enum field name %s";_tag_dyneither(_tmp6E0,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpD03,_tmpD02,_tag_dyneither(_tmp6DF,sizeof(void*),1U));});});else{
# 3970
({struct Cyc_List_List*_tmpD04=({struct Cyc_List_List*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->hd=(*_tmp6DE->name).f2,_tmp6E2->tl=prev_fields;_tmp6E2;});prev_fields=_tmpD04;});}
# 3972
if(_tmp6DE->tag == 0)
({struct Cyc_Absyn_Exp*_tmpD05=Cyc_Absyn_uint_exp(tag_count,_tmp6DE->loc);_tmp6DE->tag=_tmpD05;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6DE->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6E5=({struct Cyc_String_pa_PrintArg_struct _tmpA86;_tmpA86.tag=0U,_tmpA86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6DE->name).f2);_tmpA86;});void*_tmp6E3[1U];_tmp6E3[0]=& _tmp6E5;({unsigned int _tmpD07=loc;struct _dyneither_ptr _tmpD06=({const char*_tmp6E4="enum field %s: expression is not constant";_tag_dyneither(_tmp6E4,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpD07,_tmpD06,_tag_dyneither(_tmp6E3,sizeof(void*),1U));});});}{
# 3977
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6DE->tag))).f1;
tag_count=t1 + 1;};}
# 3980
goto _LL0;}case 13U: _LLB: _tmp829=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6CA)->f1;_tmp828=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6CA)->f2;_LLC:
# 3982
 if(*_tmp828 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp828))->fields == 0){
struct _handler_cons _tmp6E6;_push_handler(& _tmp6E6);{int _tmp6E8=0;if(setjmp(_tmp6E6.handler))_tmp6E8=1;if(!_tmp6E8){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp829);
*_tmp828=*ed;}
# 3984
;_pop_handler();}else{void*_tmp6E7=(void*)_exn_thrown;void*_tmp6E9=_tmp6E7;void*_tmp6EC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6E9)->tag == Cyc_Dict_Absent){_LL4D: _LL4E: {
# 3988
struct Cyc_Absyn_Enumdecl*_tmp6EA=({struct Cyc_Absyn_Enumdecl*_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB->sc=Cyc_Absyn_Extern,_tmp6EB->name=_tmp829,_tmp6EB->fields=0;_tmp6EB;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6EA);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp829);
*_tmp828=*ed;
goto _LL4C;};}}else{_LL4F: _tmp6EC=_tmp6E9;_LL50:(int)_rethrow(_tmp6EC);}_LL4C:;}};}{
# 3994
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp828);
goto _LL0;};case 3U: _LLD: _tmp82B=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6CA)->f1).datatype_info;_tmp82A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6CA)->f1).targs;_LLE: {
# 3997
struct Cyc_List_List*_tmp6ED=*_tmp82A;
{union Cyc_Absyn_DatatypeInfoU _tmp6EE=*_tmp82B;union Cyc_Absyn_DatatypeInfoU _tmp6EF=_tmp6EE;struct Cyc_Absyn_Datatypedecl*_tmp706;struct _tuple2*_tmp705;int _tmp704;if((_tmp6EF.UnknownDatatype).tag == 1){_LL52: _tmp705=((_tmp6EF.UnknownDatatype).val).name;_tmp704=((_tmp6EF.UnknownDatatype).val).is_extensible;_LL53: {
# 4000
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp6F0;_push_handler(& _tmp6F0);{int _tmp6F2=0;if(setjmp(_tmp6F0.handler))_tmp6F2=1;if(!_tmp6F2){({struct Cyc_Absyn_Datatypedecl**_tmpD08=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp705);tudp=_tmpD08;});;_pop_handler();}else{void*_tmp6F1=(void*)_exn_thrown;void*_tmp6F3=_tmp6F1;void*_tmp6F9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6F3)->tag == Cyc_Dict_Absent){_LL57: _LL58: {
# 4004
struct Cyc_Absyn_Datatypedecl*_tmp6F4=({struct Cyc_Absyn_Datatypedecl*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));_tmp6F8->sc=Cyc_Absyn_Extern,_tmp6F8->name=_tmp705,_tmp6F8->tvs=0,_tmp6F8->fields=0,_tmp6F8->is_extensible=_tmp704;_tmp6F8;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp6F4);
({struct Cyc_Absyn_Datatypedecl**_tmpD09=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp705);tudp=_tmpD09;});
# 4008
if(_tmp6ED != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp6F7=({struct Cyc_String_pa_PrintArg_struct _tmpA87;_tmpA87.tag=0U,({
struct _dyneither_ptr _tmpD0A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp705));_tmpA87.f1=_tmpD0A;});_tmpA87;});void*_tmp6F5[1U];_tmp6F5[0]=& _tmp6F7;({unsigned int _tmpD0C=loc;struct _dyneither_ptr _tmpD0B=({const char*_tmp6F6="declare parameterized datatype %s before using";_tag_dyneither(_tmp6F6,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD0C,_tmpD0B,_tag_dyneither(_tmp6F5,sizeof(void*),1U));});});
return cvtenv;}
# 4013
goto _LL56;}}else{_LL59: _tmp6F9=_tmp6F3;_LL5A:(int)_rethrow(_tmp6F9);}_LL56:;}};}
# 4017
if(_tmp704  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp6FC=({struct Cyc_String_pa_PrintArg_struct _tmpA88;_tmpA88.tag=0U,({
struct _dyneither_ptr _tmpD0D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp705));_tmpA88.f1=_tmpD0D;});_tmpA88;});void*_tmp6FA[1U];_tmp6FA[0]=& _tmp6FC;({unsigned int _tmpD0F=loc;struct _dyneither_ptr _tmpD0E=({const char*_tmp6FB="datatype %s was not declared @extensible";_tag_dyneither(_tmp6FB,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpD0F,_tmpD0E,_tag_dyneither(_tmp6FA,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfoU _tmpD10=Cyc_Absyn_KnownDatatype(tudp);*_tmp82B=_tmpD10;});
_tmp706=*tudp;goto _LL55;}}else{_LL54: _tmp706=*(_tmp6EF.KnownDatatype).val;_LL55: {
# 4024
struct Cyc_List_List*tvs=_tmp706->tvs;
for(0;_tmp6ED != 0  && tvs != 0;(_tmp6ED=_tmp6ED->tl,tvs=tvs->tl)){
void*t=(void*)_tmp6ED->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4030
{struct _tuple0 _tmp6FD=({struct _tuple0 _tmpA89;({void*_tmpD11=Cyc_Absyn_compress_kb(tv->kind);_tmpA89.f1=_tmpD11;}),_tmpA89.f2=t;_tmpA89;});struct _tuple0 _tmp6FE=_tmp6FD;struct Cyc_Absyn_Tvar*_tmp6FF;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6FE.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6FE.f2)->tag == 2U){_LL5C: _tmp6FF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6FE.f2)->f1;_LL5D:
# 4032
({struct Cyc_List_List*_tmpD12=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6FF);cvtenv.kind_env=_tmpD12;});
({struct Cyc_List_List*_tmpD13=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp6FF,1);cvtenv.free_vars=_tmpD13;});
continue;}else{goto _LL5E;}}else{_LL5E: _LL5F:
 goto _LL5B;}_LL5B:;}{
# 4037
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpD14=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpD14;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4041
if(_tmp6ED != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp702=({struct Cyc_String_pa_PrintArg_struct _tmpA8A;_tmpA8A.tag=0U,({
struct _dyneither_ptr _tmpD15=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp706->name));_tmpA8A.f1=_tmpD15;});_tmpA8A;});void*_tmp700[1U];_tmp700[0]=& _tmp702;({unsigned int _tmpD17=loc;struct _dyneither_ptr _tmpD16=({const char*_tmp701="too many type arguments for datatype %s";_tag_dyneither(_tmp701,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpD17,_tmpD16,_tag_dyneither(_tmp700,sizeof(void*),1U));});});
if(tvs != 0){
# 4046
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpD18=({struct Cyc_List_List*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703->hd=e,_tmp703->tl=hidden_ts;_tmp703;});hidden_ts=_tmpD18;});
({struct Cyc_Tcutil_CVTEnv _tmpD19=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpD19;});}
# 4053
({struct Cyc_List_List*_tmpD1B=({struct Cyc_List_List*_tmpD1A=*_tmp82A;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD1A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp82A=_tmpD1B;});}
# 4055
goto _LL51;}}_LL51:;}
# 4057
goto _LL0;}case 4U: _LLF: _tmp82D=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6CA)->f1).field_info;_tmp82C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6CA)->f1).targs;_LL10:
# 4060
{union Cyc_Absyn_DatatypeFieldInfoU _tmp707=*_tmp82D;union Cyc_Absyn_DatatypeFieldInfoU _tmp708=_tmp707;struct Cyc_Absyn_Datatypedecl*_tmp71B;struct Cyc_Absyn_Datatypefield*_tmp71A;struct _tuple2*_tmp719;struct _tuple2*_tmp718;int _tmp717;if((_tmp708.UnknownDatatypefield).tag == 1){_LL61: _tmp719=((_tmp708.UnknownDatatypefield).val).datatype_name;_tmp718=((_tmp708.UnknownDatatypefield).val).field_name;_tmp717=((_tmp708.UnknownDatatypefield).val).is_extensible;_LL62: {
# 4063
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp719);
struct Cyc_Absyn_Datatypefield*tuf;
# 4068
{struct Cyc_List_List*_tmp709=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp709=_tmp709->tl){
if(_tmp709 == 0)
({void*_tmp70A=0U;({struct _dyneither_ptr _tmpD1C=({const char*_tmp70B="Tcutil found a bad datatypefield";_tag_dyneither(_tmp70B,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD1C,_tag_dyneither(_tmp70A,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp709->hd)->name,_tmp718)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp709->hd;
break;}}}
# 4076
({union Cyc_Absyn_DatatypeFieldInfoU _tmpD1D=Cyc_Absyn_KnownDatatypefield(tud,tuf);*_tmp82D=_tmpD1D;});
_tmp71B=tud;_tmp71A=tuf;goto _LL64;}}else{_LL63: _tmp71B=((_tmp708.KnownDatatypefield).val).f1;_tmp71A=((_tmp708.KnownDatatypefield).val).f2;_LL64: {
# 4080
struct Cyc_List_List*tvs=_tmp71B->tvs;
for(0;_tmp82C != 0  && tvs != 0;(_tmp82C=_tmp82C->tl,tvs=tvs->tl)){
void*t=(void*)_tmp82C->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4086
{struct _tuple0 _tmp70C=({struct _tuple0 _tmpA8B;({void*_tmpD1E=Cyc_Absyn_compress_kb(tv->kind);_tmpA8B.f1=_tmpD1E;}),_tmpA8B.f2=t;_tmpA8B;});struct _tuple0 _tmp70D=_tmp70C;struct Cyc_Absyn_Tvar*_tmp70E;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp70D.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70D.f2)->tag == 2U){_LL66: _tmp70E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70D.f2)->f1;_LL67:
# 4088
({struct Cyc_List_List*_tmpD1F=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp70E);cvtenv.kind_env=_tmpD1F;});
({struct Cyc_List_List*_tmpD20=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp70E,1);cvtenv.free_vars=_tmpD20;});
continue;}else{goto _LL68;}}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}{
# 4093
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpD21=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpD21;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4097
if(_tmp82C != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp711=({struct Cyc_String_pa_PrintArg_struct _tmpA8D;_tmpA8D.tag=0U,({
struct _dyneither_ptr _tmpD22=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71B->name));_tmpA8D.f1=_tmpD22;});_tmpA8D;});struct Cyc_String_pa_PrintArg_struct _tmp712=({struct Cyc_String_pa_PrintArg_struct _tmpA8C;_tmpA8C.tag=0U,({struct _dyneither_ptr _tmpD23=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71A->name));_tmpA8C.f1=_tmpD23;});_tmpA8C;});void*_tmp70F[2U];_tmp70F[0]=& _tmp711,_tmp70F[1]=& _tmp712;({unsigned int _tmpD25=loc;struct _dyneither_ptr _tmpD24=({const char*_tmp710="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp710,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpD25,_tmpD24,_tag_dyneither(_tmp70F,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp715=({struct Cyc_String_pa_PrintArg_struct _tmpA8F;_tmpA8F.tag=0U,({
struct _dyneither_ptr _tmpD26=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71B->name));_tmpA8F.f1=_tmpD26;});_tmpA8F;});struct Cyc_String_pa_PrintArg_struct _tmp716=({struct Cyc_String_pa_PrintArg_struct _tmpA8E;_tmpA8E.tag=0U,({struct _dyneither_ptr _tmpD27=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71A->name));_tmpA8E.f1=_tmpD27;});_tmpA8E;});void*_tmp713[2U];_tmp713[0]=& _tmp715,_tmp713[1]=& _tmp716;({unsigned int _tmpD29=loc;struct _dyneither_ptr _tmpD28=({const char*_tmp714="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp714,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpD29,_tmpD28,_tag_dyneither(_tmp713,sizeof(void*),2U));});});
goto _LL60;}}_LL60:;}
# 4105
goto _LL0;case 5U: _LL11: _tmp833=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CA)->f1).elt_typ;_tmp832=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CA)->f1).elt_tq;_tmp831=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CA)->f1).ptr_atts).rgn;_tmp830=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CA)->f1).ptr_atts).nullable;_tmp82F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CA)->f1).ptr_atts).bounds;_tmp82E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CA)->f1).ptr_atts).zero_term;_LL12: {
# 4108
int is_zero_terminated;
# 4110
({struct Cyc_Tcutil_CVTEnv _tmpD2A=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp833,1,1);cvtenv=_tmpD2A;});
({int _tmpD2B=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp832->print_const,_tmp833);_tmp832->real_const=_tmpD2B;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp71C=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp71D=_tmp71C;switch(_tmp71D){case Cyc_Absyn_Aliasable: _LL6B: _LL6C:
 k=& Cyc_Tcutil_rk;goto _LL6A;case Cyc_Absyn_Unique: _LL6D: _LL6E:
 k=& Cyc_Tcutil_urk;goto _LL6A;default: _LL6F: _LL70:
 k=& Cyc_Tcutil_trk;goto _LL6A;}_LL6A:;}
# 4118
({struct Cyc_Tcutil_CVTEnv _tmpD2C=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp831,1,1);cvtenv=_tmpD2C;});
{union Cyc_Absyn_Constraint*_tmp71E=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp82E);union Cyc_Absyn_Constraint*_tmp71F=_tmp71E;int _tmp725;switch((((union Cyc_Absyn_Constraint*)_tmp71F)->Eq_constr).tag){case 3U: _LL72: _LL73:
# 4123
{void*_tmp720=Cyc_Tcutil_compress(_tmp833);void*_tmp721=_tmp720;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp721)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp721)->f2 == Cyc_Absyn_Char_sz){_LL79: _LL7A:
# 4125
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp82E,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL78;}else{goto _LL7B;}}else{_LL7B: _LL7C:
# 4129
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp82E,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL78;}_LL78:;}
# 4133
goto _LL71;case 1U: _LL74: _tmp725=(int)(_tmp71F->Eq_constr).val;if(_tmp725){_LL75:
# 4136
 if(!Cyc_Tcutil_admits_zero(_tmp833))
({struct Cyc_String_pa_PrintArg_struct _tmp724=({struct Cyc_String_pa_PrintArg_struct _tmpA90;_tmpA90.tag=0U,({
struct _dyneither_ptr _tmpD2D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp833));_tmpA90.f1=_tmpD2D;});_tmpA90;});void*_tmp722[1U];_tmp722[0]=& _tmp724;({unsigned int _tmpD2F=loc;struct _dyneither_ptr _tmpD2E=({const char*_tmp723="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp723,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpD2F,_tmpD2E,_tag_dyneither(_tmp722,sizeof(void*),1U));});});
is_zero_terminated=1;
goto _LL71;}else{goto _LL76;}default: _LL76: _LL77:
# 4142
 is_zero_terminated=0;
goto _LL71;}_LL71:;}
# 4146
{void*_tmp726=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp82F);void*_tmp727=_tmp726;struct Cyc_Absyn_Exp*_tmp730;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp727)->tag == 0U){_LL7E: _LL7F:
 goto _LL7D;}else{_LL80: _tmp730=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp727)->f1;_LL81:
# 4149
({struct Cyc_Tcenv_Tenv*_tmpD30=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD30,0,_tmp730);});
({struct Cyc_Tcutil_CVTEnv _tmpD31=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp730,te,cvtenv);cvtenv=_tmpD31;});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp730))
({void*_tmp728=0U;({unsigned int _tmpD33=loc;struct _dyneither_ptr _tmpD32=({const char*_tmp729="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp729,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpD33,_tmpD32,_tag_dyneither(_tmp728,sizeof(void*),0U));});});{
struct _tuple14 _tmp72A=Cyc_Evexp_eval_const_uint_exp(_tmp730);struct _tuple14 _tmp72B=_tmp72A;unsigned int _tmp72F;int _tmp72E;_LL83: _tmp72F=_tmp72B.f1;_tmp72E=_tmp72B.f2;_LL84:;
if(is_zero_terminated  && (!_tmp72E  || _tmp72F < 1))
({void*_tmp72C=0U;({unsigned int _tmpD35=loc;struct _dyneither_ptr _tmpD34=({const char*_tmp72D="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp72D,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpD35,_tmpD34,_tag_dyneither(_tmp72C,sizeof(void*),0U));});});
goto _LL7D;};}_LL7D:;}
# 4158
goto _LL0;};}case 19U: _LL13: _tmp834=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6CA)->f1;_LL14:
# 4160
({struct Cyc_Tcutil_CVTEnv _tmpD36=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp834,1,1);cvtenv=_tmpD36;});goto _LL0;case 28U: _LL15: _LL16:
 goto _LL0;case 18U: _LL17: _tmp835=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6CA)->f1;_LL18:
# 4166
({struct Cyc_Tcenv_Tenv*_tmpD37=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD37,0,_tmp835);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp835))
({void*_tmp731=0U;({unsigned int _tmpD39=loc;struct _dyneither_ptr _tmpD38=({const char*_tmp732="valueof_t requires an int expression";_tag_dyneither(_tmp732,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD39,_tmpD38,_tag_dyneither(_tmp731,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD3A=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp835,te,cvtenv);cvtenv=_tmpD3A;});
goto _LL0;case 27U: _LL19: _tmp836=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6CA)->f1;_LL1A:
# 4175
({struct Cyc_Tcenv_Tenv*_tmpD3B=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD3B,0,_tmp836);});
goto _LL0;case 6U: _LL1B: _LL1C:
 goto _LL1E;case 7U: _LL1D: _LL1E:
 goto _LL0;case 8U: _LL1F: _tmp83B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CA)->f1).elt_type;_tmp83A=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CA)->f1).tq;_tmp839=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CA)->f1).num_elts;_tmp838=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CA)->f1).zero_term;_tmp837=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CA)->f1).zt_loc;_LL20: {
# 4182
struct Cyc_Absyn_Exp*_tmp733=*_tmp839;
({struct Cyc_Tcutil_CVTEnv _tmpD3C=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp83B,1,allow_abs_aggr);cvtenv=_tmpD3C;});
({int _tmpD3D=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp83A->print_const,_tmp83B);_tmp83A->real_const=_tmpD3D;});{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp734=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp838);union Cyc_Absyn_Constraint*_tmp735=_tmp734;int _tmp739;switch((((union Cyc_Absyn_Constraint*)_tmp735)->Eq_constr).tag){case 3U: _LL86: _LL87:
# 4189
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp838,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4204 "tcutil.cyc"
goto _LL85;case 1U: _LL88: _tmp739=(int)(_tmp735->Eq_constr).val;if(_tmp739){_LL89:
# 4207
 if(!Cyc_Tcutil_admits_zero(_tmp83B))
({struct Cyc_String_pa_PrintArg_struct _tmp738=({struct Cyc_String_pa_PrintArg_struct _tmpA91;_tmpA91.tag=0U,({
struct _dyneither_ptr _tmpD3E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp83B));_tmpA91.f1=_tmpD3E;});_tmpA91;});void*_tmp736[1U];_tmp736[0]=& _tmp738;({unsigned int _tmpD40=loc;struct _dyneither_ptr _tmpD3F=({const char*_tmp737="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp737,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD40,_tmpD3F,_tag_dyneither(_tmp736,sizeof(void*),1U));});});
is_zero_terminated=1;
goto _LL85;}else{goto _LL8A;}default: _LL8A: _LL8B:
# 4213
 is_zero_terminated=0;
goto _LL85;}_LL85:;}
# 4218
if(_tmp733 == 0){
# 4220
if(is_zero_terminated)
# 4222
({struct Cyc_Absyn_Exp*_tmpD42=({struct Cyc_Absyn_Exp*_tmpD41=Cyc_Absyn_uint_exp(1U,0U);_tmp733=_tmpD41;});*_tmp839=_tmpD42;});else{
# 4225
({void*_tmp73A=0U;({unsigned int _tmpD44=loc;struct _dyneither_ptr _tmpD43=({const char*_tmp73B="array bound defaults to 1 here";_tag_dyneither(_tmp73B,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpD44,_tmpD43,_tag_dyneither(_tmp73A,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpD46=({struct Cyc_Absyn_Exp*_tmpD45=Cyc_Absyn_uint_exp(1U,0U);_tmp733=_tmpD45;});*_tmp839=_tmpD46;});}}
# 4229
({struct Cyc_Tcenv_Tenv*_tmpD47=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD47,0,_tmp733);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp733))
({void*_tmp73C=0U;({unsigned int _tmpD49=loc;struct _dyneither_ptr _tmpD48=({const char*_tmp73D="array bounds expression is not an unsigned int";_tag_dyneither(_tmp73D,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD49,_tmpD48,_tag_dyneither(_tmp73C,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD4A=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp733,te,cvtenv);cvtenv=_tmpD4A;});{
# 4237
struct _tuple14 _tmp73E=Cyc_Evexp_eval_const_uint_exp(_tmp733);struct _tuple14 _tmp73F=_tmp73E;unsigned int _tmp745;int _tmp744;_LL8D: _tmp745=_tmp73F.f1;_tmp744=_tmp73F.f2;_LL8E:;
# 4239
if((is_zero_terminated  && _tmp744) && _tmp745 < 1)
({void*_tmp740=0U;({unsigned int _tmpD4C=loc;struct _dyneither_ptr _tmpD4B=({const char*_tmp741="zero terminated array cannot have zero elements";_tag_dyneither(_tmp741,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpD4C,_tmpD4B,_tag_dyneither(_tmp740,sizeof(void*),0U));});});
# 4242
if((_tmp744  && _tmp745 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp742=0U;({unsigned int _tmpD4E=loc;struct _dyneither_ptr _tmpD4D=({const char*_tmp743="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp743,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpD4E,_tmpD4D,_tag_dyneither(_tmp742,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpD4F=Cyc_Absyn_uint_exp(1U,0U);*_tmp839=_tmpD4F;});}
# 4246
goto _LL0;};};}case 9U: _LL21: _tmp848=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).tvars;_tmp847=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).effect;_tmp846=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).ret_tqual;_tmp845=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).ret_typ;_tmp844=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).args;_tmp843=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).c_varargs;_tmp842=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).cyc_varargs;_tmp841=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).rgn_po;_tmp840=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).attributes;_tmp83F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).requires_clause;_tmp83E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).requires_relns;_tmp83D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).ensures_clause;_tmp83C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6CA)->f1).ensures_relns;_LL22: {
# 4253
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp840 != 0;_tmp840=_tmp840->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp840->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp748=({struct Cyc_String_pa_PrintArg_struct _tmpA92;_tmpA92.tag=0U,({struct _dyneither_ptr _tmpD50=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp840->hd));_tmpA92.f1=_tmpD50;});_tmpA92;});void*_tmp746[1U];_tmp746[0]=& _tmp748;({unsigned int _tmpD52=loc;struct _dyneither_ptr _tmpD51=({const char*_tmp747="bad function type attribute %s";_tag_dyneither(_tmp747,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpD52,_tmpD51,_tag_dyneither(_tmp746,sizeof(void*),1U));});});{
void*_tmp749=(void*)_tmp840->hd;void*_tmp74A=_tmp749;enum Cyc_Absyn_Format_Type _tmp74F;int _tmp74E;int _tmp74D;switch(*((int*)_tmp74A)){case 1U: _LL90: _LL91:
# 4266
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL8F;case 2U: _LL92: _LL93:
# 4268
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL8F;case 3U: _LL94: _LL95:
# 4270
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL8F;case 19U: _LL96: _tmp74F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp74A)->f1;_tmp74E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp74A)->f2;_tmp74D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp74A)->f3;_LL97:
# 4272
 if(!seen_format){
seen_format=1;
ft=_tmp74F;
fmt_desc_arg=_tmp74E;
fmt_arg_start=_tmp74D;}else{
# 4278
({void*_tmp74B=0U;({unsigned int _tmpD54=loc;struct _dyneither_ptr _tmpD53=({const char*_tmp74C="function can't have multiple format attributes";_tag_dyneither(_tmp74C,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD54,_tmpD53,_tag_dyneither(_tmp74B,sizeof(void*),0U));});});}
goto _LL8F;default: _LL98: _LL99:
 goto _LL8F;}_LL8F:;};}
# 4283
if(num_convs > 1)
({void*_tmp750=0U;({unsigned int _tmpD56=loc;struct _dyneither_ptr _tmpD55=({const char*_tmp751="function can't have multiple calling conventions";_tag_dyneither(_tmp751,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpD56,_tmpD55,_tag_dyneither(_tmp750,sizeof(void*),0U));});});
# 4288
Cyc_Tcutil_check_unique_tvars(loc,*_tmp848);
{struct Cyc_List_List*b=*_tmp848;for(0;b != 0;b=b->tl){
({int _tmpD57=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpD57;});
({struct Cyc_List_List*_tmpD58=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpD58;});{
void*_tmp752=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp753=_tmp752;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp753)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp753)->f1)->kind == Cyc_Absyn_MemKind){_LL9B: _LL9C:
# 4294
({struct Cyc_String_pa_PrintArg_struct _tmp756=({struct Cyc_String_pa_PrintArg_struct _tmpA93;_tmpA93.tag=0U,_tmpA93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmpA93;});void*_tmp754[1U];_tmp754[0]=& _tmp756;({unsigned int _tmpD5A=loc;struct _dyneither_ptr _tmpD59=({const char*_tmp755="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp755,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD5A,_tmpD59,_tag_dyneither(_tmp754,sizeof(void*),1U));});});
goto _LL9A;}else{goto _LL9D;}}else{_LL9D: _LL9E:
 goto _LL9A;}_LL9A:;};}}{
# 4302
struct Cyc_Tcutil_CVTEnv _tmp757=({struct Cyc_Tcutil_CVTEnv _tmpA97;_tmpA97.r=Cyc_Core_heap_region,_tmpA97.kind_env=cvtenv.kind_env,_tmpA97.free_vars=0,_tmpA97.free_evars=0,_tmpA97.generalize_evars=cvtenv.generalize_evars,_tmpA97.fn_result=1;_tmpA97;});
# 4307
({struct Cyc_Tcutil_CVTEnv _tmpD5B=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp757,& Cyc_Tcutil_tmk,_tmp845,1,1);_tmp757=_tmpD5B;});
({int _tmpD5C=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp846->print_const,_tmp845);_tmp846->real_const=_tmpD5C;});
_tmp757.fn_result=0;
{struct Cyc_List_List*a=_tmp844;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp758=(struct _tuple10*)a->hd;
void*_tmp759=(*_tmp758).f3;
({struct Cyc_Tcutil_CVTEnv _tmpD5D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp757,& Cyc_Tcutil_tmk,_tmp759,1,1);_tmp757=_tmpD5D;});{
int _tmp75A=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp758).f2).print_const,_tmp759);
((*_tmp758).f2).real_const=_tmp75A;
# 4318
if(Cyc_Tcutil_is_array(_tmp759)){
# 4320
void*_tmp75B=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpD5E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp757,& Cyc_Tcutil_rk,_tmp75B,1,1);_tmp757=_tmpD5E;});
({void*_tmpD5F=Cyc_Tcutil_promote_array(_tmp759,_tmp75B,0);(*_tmp758).f3=_tmpD5F;});}};}}
# 4327
if(_tmp842 != 0){
if(_tmp843)({void*_tmp75C=0U;({struct _dyneither_ptr _tmpD60=({const char*_tmp75D="both c_vararg and cyc_vararg";_tag_dyneither(_tmp75D,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD60,_tag_dyneither(_tmp75C,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp75E=*_tmp842;struct Cyc_Absyn_VarargInfo _tmp75F=_tmp75E;void*_tmp772;int _tmp771;_LLA0: _tmp772=_tmp75F.type;_tmp771=_tmp75F.inject;_LLA1:;
({struct Cyc_Tcutil_CVTEnv _tmpD61=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp757,& Cyc_Tcutil_tmk,_tmp772,1,1);_tmp757=_tmpD61;});
({int _tmpD62=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp842->tq).print_const,_tmp772);(_tmp842->tq).real_const=_tmpD62;});
# 4333
if(_tmp771){
void*_tmp760=Cyc_Tcutil_compress(_tmp772);void*_tmp761=_tmp760;void*_tmp770;union Cyc_Absyn_Constraint*_tmp76F;union Cyc_Absyn_Constraint*_tmp76E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->tag == 5U){_LLA3: _tmp770=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).elt_typ;_tmp76F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).ptr_atts).bounds;_tmp76E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).ptr_atts).zero_term;_LLA4:
# 4336
{void*_tmp762=Cyc_Tcutil_compress(_tmp770);void*_tmp763=_tmp762;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp763)->tag == 3U){_LLA8: _LLA9:
# 4338
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp76E))
({void*_tmp764=0U;({unsigned int _tmpD64=loc;struct _dyneither_ptr _tmpD63=({const char*_tmp765="can't inject into a zeroterm pointer";_tag_dyneither(_tmp765,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD64,_tmpD63,_tag_dyneither(_tmp764,sizeof(void*),0U));});});
{void*_tmp766=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp76F);void*_tmp767=_tmp766;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp767)->tag == 0U){_LLAD: _LLAE:
# 4342
({void*_tmp768=0U;({unsigned int _tmpD66=loc;struct _dyneither_ptr _tmpD65=({const char*_tmp769="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp769,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpD66,_tmpD65,_tag_dyneither(_tmp768,sizeof(void*),0U));});});
goto _LLAC;}else{_LLAF: _LLB0:
 goto _LLAC;}_LLAC:;}
# 4346
goto _LLA7;}else{_LLAA: _LLAB:
({void*_tmp76A=0U;({unsigned int _tmpD68=loc;struct _dyneither_ptr _tmpD67=({const char*_tmp76B="can't inject a non-datatype type";_tag_dyneither(_tmp76B,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpD68,_tmpD67,_tag_dyneither(_tmp76A,sizeof(void*),0U));});});goto _LLA7;}_LLA7:;}
# 4349
goto _LLA2;}else{_LLA5: _LLA6:
({void*_tmp76C=0U;({unsigned int _tmpD6A=loc;struct _dyneither_ptr _tmpD69=({const char*_tmp76D="expecting a datatype pointer type";_tag_dyneither(_tmp76D,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpD6A,_tmpD69,_tag_dyneither(_tmp76C,sizeof(void*),0U));});});goto _LLA2;}_LLA2:;}};}
# 4355
if(seen_format){
int _tmp773=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp844);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp773) || fmt_arg_start < 0) || 
# 4359
(_tmp842 == 0  && !_tmp843) && fmt_arg_start != 0) || 
(_tmp842 != 0  || _tmp843) && fmt_arg_start != _tmp773 + 1)
# 4362
({void*_tmp774=0U;({unsigned int _tmpD6C=loc;struct _dyneither_ptr _tmpD6B=({const char*_tmp775="bad format descriptor";_tag_dyneither(_tmp775,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpD6C,_tmpD6B,_tag_dyneither(_tmp774,sizeof(void*),0U));});});else{
# 4365
struct _tuple10 _tmp776=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp844,fmt_desc_arg - 1);struct _tuple10 _tmp777=_tmp776;void*_tmp78D;_LLB2: _tmp78D=_tmp777.f3;_LLB3:;
# 4367
{void*_tmp778=Cyc_Tcutil_compress(_tmp78D);void*_tmp779=_tmp778;void*_tmp786;union Cyc_Absyn_Constraint*_tmp785;union Cyc_Absyn_Constraint*_tmp784;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp779)->tag == 5U){_LLB5: _tmp786=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp779)->f1).elt_typ;_tmp785=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp779)->f1).ptr_atts).bounds;_tmp784=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp779)->f1).ptr_atts).zero_term;_LLB6:
# 4370
{struct _tuple0 _tmp77A=({struct _tuple0 _tmpA94;({void*_tmpD6E=Cyc_Tcutil_compress(_tmp786);_tmpA94.f1=_tmpD6E;}),({void*_tmpD6D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp785);_tmpA94.f2=_tmpD6D;});_tmpA94;});struct _tuple0 _tmp77B=_tmp77A;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77B.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77B.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp77B.f2)->tag == 0U){_LLBA: _LLBB:
# 4372
 if(_tmp843)
({void*_tmp77C=0U;({unsigned int _tmpD70=loc;struct _dyneither_ptr _tmpD6F=({const char*_tmp77D="format descriptor is not a char * type";_tag_dyneither(_tmp77D,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD70,_tmpD6F,_tag_dyneither(_tmp77C,sizeof(void*),0U));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 4376
 if(!_tmp843)
({void*_tmp77E=0U;({unsigned int _tmpD72=loc;struct _dyneither_ptr _tmpD71=({const char*_tmp77F="format descriptor is not a char ? type";_tag_dyneither(_tmp77F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD72,_tmpD71,_tag_dyneither(_tmp77E,sizeof(void*),0U));});});
goto _LLB9;}}else{goto _LLBE;}}else{_LLBE: _LLBF:
# 4380
({void*_tmp780=0U;({unsigned int _tmpD74=loc;struct _dyneither_ptr _tmpD73=({const char*_tmp781="format descriptor is not a string type";_tag_dyneither(_tmp781,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD74,_tmpD73,_tag_dyneither(_tmp780,sizeof(void*),0U));});});
goto _LLB9;}_LLB9:;}
# 4383
goto _LLB4;}else{_LLB7: _LLB8:
({void*_tmp782=0U;({unsigned int _tmpD76=loc;struct _dyneither_ptr _tmpD75=({const char*_tmp783="format descriptor is not a string type";_tag_dyneither(_tmp783,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD76,_tmpD75,_tag_dyneither(_tmp782,sizeof(void*),0U));});});goto _LLB4;}_LLB4:;}
# 4386
if(fmt_arg_start != 0  && !_tmp843){
# 4390
int problem;
{struct _tuple31 _tmp787=({struct _tuple31 _tmpA95;_tmpA95.f1=ft,({void*_tmpD77=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp842))->type));_tmpA95.f2=_tmpD77;});_tmpA95;});struct _tuple31 _tmp788=_tmp787;struct Cyc_Absyn_Datatypedecl*_tmp78A;struct Cyc_Absyn_Datatypedecl*_tmp789;if(_tmp788.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp788.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp788.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC1: _tmp789=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp788.f2)->f1).datatype_info).KnownDatatype).val;_LLC2:
# 4393
({int _tmpD78=Cyc_Absyn_qvar_cmp(_tmp789->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;problem=_tmpD78;});goto _LLC0;}else{goto _LLC5;}}else{goto _LLC5;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp788.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp788.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC3: _tmp78A=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp788.f2)->f1).datatype_info).KnownDatatype).val;_LLC4:
# 4395
({int _tmpD79=Cyc_Absyn_qvar_cmp(_tmp78A->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;problem=_tmpD79;});goto _LLC0;}else{goto _LLC5;}}else{_LLC5: _LLC6:
# 4397
 problem=1;goto _LLC0;}}_LLC0:;}
# 4399
if(problem)
({void*_tmp78B=0U;({unsigned int _tmpD7B=loc;struct _dyneither_ptr _tmpD7A=({const char*_tmp78C="format attribute and vararg types don't match";_tag_dyneither(_tmp78C,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpD7B,_tmpD7A,_tag_dyneither(_tmp78B,sizeof(void*),0U));});});}}}
# 4407
{struct Cyc_List_List*rpo=_tmp841;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp78E=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp78F=_tmp78E;void*_tmp791;void*_tmp790;_LLC8: _tmp791=_tmp78F->f1;_tmp790=_tmp78F->f2;_LLC9:;
({struct Cyc_Tcutil_CVTEnv _tmpD7C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp757,& Cyc_Tcutil_ek,_tmp791,1,1);_tmp757=_tmpD7C;});
({struct Cyc_Tcutil_CVTEnv _tmpD7D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp757,& Cyc_Tcutil_trk,_tmp790,1,1);_tmp757=_tmpD7D;});}}{
# 4415
struct Cyc_Tcenv_Fenv*_tmp792=Cyc_Tcenv_bogus_fenv(_tmp845,_tmp844);
struct Cyc_Tcenv_Tenv*_tmp793=({struct Cyc_Tcenv_Tenv*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));_tmp7D9->ns=te->ns,_tmp7D9->ae=te->ae,_tmp7D9->le=_tmp792,_tmp7D9->allow_valueof=1,_tmp7D9->in_extern_c_include=te->in_extern_c_include;_tmp7D9;});
struct _tuple30 _tmp794=({unsigned int _tmpD81=loc;struct Cyc_Tcenv_Tenv*_tmpD80=_tmp793;struct Cyc_Tcutil_CVTEnv _tmpD7F=_tmp757;struct _dyneither_ptr _tmpD7E=({const char*_tmp7D8="@requires";_tag_dyneither(_tmp7D8,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpD81,_tmpD80,_tmpD7F,_tmpD7E,_tmp83F);});struct _tuple30 _tmp795=_tmp794;struct Cyc_Tcutil_CVTEnv _tmp7D7;struct Cyc_List_List*_tmp7D6;_LLCB: _tmp7D7=_tmp795.f1;_tmp7D6=_tmp795.f2;_LLCC:;
_tmp757=_tmp7D7;
*_tmp83E=_tmp7D6;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp844,_tmp7D6);{
# 4428
struct _tuple30 _tmp796=({unsigned int _tmpD85=loc;struct Cyc_Tcenv_Tenv*_tmpD84=_tmp793;struct Cyc_Tcutil_CVTEnv _tmpD83=_tmp757;struct _dyneither_ptr _tmpD82=({const char*_tmp7D5="@ensures";_tag_dyneither(_tmp7D5,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpD85,_tmpD84,_tmpD83,_tmpD82,_tmp83D);});struct _tuple30 _tmp797=_tmp796;struct Cyc_Tcutil_CVTEnv _tmp7D4;struct Cyc_List_List*_tmp7D3;_LLCE: _tmp7D4=_tmp797.f1;_tmp7D3=_tmp797.f2;_LLCF:;
_tmp757=_tmp7D4;
*_tmp83C=_tmp7D3;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp844,_tmp7D3);
# 4433
if(*_tmp847 != 0)
({struct Cyc_Tcutil_CVTEnv _tmpD86=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp757,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp847),1,1);_tmp757=_tmpD86;});else{
# 4436
struct Cyc_List_List*effect=0;
# 4441
{struct Cyc_List_List*tvs=_tmp757.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp798=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp799=_tmp798;struct Cyc_Absyn_Tvar*_tmp7AC;int _tmp7AB;_LLD1: _tmp7AC=_tmp799.f1;_tmp7AB=_tmp799.f2;_LLD2:;
if(!_tmp7AB)continue;{
void*_tmp79A=Cyc_Absyn_compress_kb(_tmp7AC->kind);void*_tmp79B=_tmp79A;struct Cyc_Core_Opt**_tmp7AA;struct Cyc_Absyn_Kind*_tmp7A9;struct Cyc_Core_Opt**_tmp7A8;struct Cyc_Core_Opt**_tmp7A7;struct Cyc_Absyn_Kind*_tmp7A6;switch(*((int*)_tmp79B)){case 2U: _LLD4: _tmp7A7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79B)->f1;_tmp7A6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79B)->f2;if(_tmp7A6->kind == Cyc_Absyn_RgnKind){_LLD5:
# 4447
 if(_tmp7A6->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpD87=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp7A7=_tmpD87;});_tmp7A9=_tmp7A6;goto _LLD7;}
# 4450
({struct Cyc_Core_Opt*_tmpD88=Cyc_Tcutil_kind_to_bound_opt(_tmp7A6);*_tmp7A7=_tmpD88;});_tmp7A9=_tmp7A6;goto _LLD7;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79B)->f2)->kind){case Cyc_Absyn_IntKind: _LLD8: _LLD9:
# 4453
 goto _LLDB;case Cyc_Absyn_EffKind: _LLDC: _tmp7A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp79B)->f1;_LLDD:
# 4456
({struct Cyc_Core_Opt*_tmpD89=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp7A8=_tmpD89;});goto _LLDF;default: goto _LLE2;}}case 0U: _LLD6: _tmp7A9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp79B)->f1;if(_tmp7A9->kind == Cyc_Absyn_RgnKind){_LLD7:
# 4452
({struct Cyc_List_List*_tmpD8C=({struct Cyc_List_List*_tmp79E=_cycalloc(sizeof(*_tmp79E));({void*_tmpD8B=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D->tag=23U,({void*_tmpD8A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C->tag=2U,_tmp79C->f1=_tmp7AC;_tmp79C;});_tmp79D->f1=_tmpD8A;});_tmp79D;});_tmp79E->hd=_tmpD8B;}),_tmp79E->tl=effect;_tmp79E;});effect=_tmpD8C;});goto _LLD3;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp79B)->f1)->kind){case Cyc_Absyn_IntKind: _LLDA: _LLDB:
# 4454
 goto _LLD3;case Cyc_Absyn_EffKind: _LLDE: _LLDF:
# 4458
({struct Cyc_List_List*_tmpD8E=({struct Cyc_List_List*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));({void*_tmpD8D=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F->tag=2U,_tmp79F->f1=_tmp7AC;_tmp79F;});_tmp7A0->hd=_tmpD8D;}),_tmp7A0->tl=effect;_tmp7A0;});effect=_tmpD8E;});goto _LLD3;default: _LLE2: _LLE3:
# 4463
({struct Cyc_List_List*_tmpD91=({struct Cyc_List_List*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));({void*_tmpD90=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));_tmp7A4->tag=25U,({void*_tmpD8F=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));_tmp7A3->tag=2U,_tmp7A3->f1=_tmp7AC;_tmp7A3;});_tmp7A4->f1=_tmpD8F;});_tmp7A4;});_tmp7A5->hd=_tmpD90;}),_tmp7A5->tl=effect;_tmp7A5;});effect=_tmpD91;});goto _LLD3;}}default: _LLE0: _tmp7AA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp79B)->f1;_LLE1:
# 4460
({struct Cyc_Core_Opt*_tmpD93=({struct Cyc_Core_Opt*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));({void*_tmpD92=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));_tmp7A1->tag=2U,_tmp7A1->f1=0,_tmp7A1->f2=& Cyc_Tcutil_ak;_tmp7A1;});_tmp7A2->v=_tmpD92;});_tmp7A2;});*_tmp7AA=_tmpD93;});goto _LLE3;}_LLD3:;};}}
# 4467
{struct Cyc_List_List*ts=_tmp757.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp7AD=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp7AE=_tmp7AD;void*_tmp7B7;int _tmp7B6;_LLE5: _tmp7B7=_tmp7AE.f1;_tmp7B6=_tmp7AE.f2;_LLE6:;
if(!_tmp7B6)continue;{
struct Cyc_Absyn_Kind*_tmp7AF=Cyc_Tcutil_typ_kind(_tmp7B7);struct Cyc_Absyn_Kind*_tmp7B0=_tmp7AF;switch(((struct Cyc_Absyn_Kind*)_tmp7B0)->kind){case Cyc_Absyn_RgnKind: _LLE8: _LLE9:
# 4472
({struct Cyc_List_List*_tmpD95=({struct Cyc_List_List*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));({void*_tmpD94=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1->tag=23U,_tmp7B1->f1=_tmp7B7;_tmp7B1;});_tmp7B2->hd=_tmpD94;}),_tmp7B2->tl=effect;_tmp7B2;});effect=_tmpD95;});goto _LLE7;case Cyc_Absyn_EffKind: _LLEA: _LLEB:
# 4474
({struct Cyc_List_List*_tmpD96=({struct Cyc_List_List*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3->hd=_tmp7B7,_tmp7B3->tl=effect;_tmp7B3;});effect=_tmpD96;});goto _LLE7;case Cyc_Absyn_IntKind: _LLEC: _LLED:
 goto _LLE7;default: _LLEE: _LLEF:
# 4477
({struct Cyc_List_List*_tmpD98=({struct Cyc_List_List*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));({void*_tmpD97=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4->tag=25U,_tmp7B4->f1=_tmp7B7;_tmp7B4;});_tmp7B5->hd=_tmpD97;}),_tmp7B5->tl=effect;_tmp7B5;});effect=_tmpD98;});goto _LLE7;}_LLE7:;};}}
# 4480
({void*_tmpD99=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->tag=24U,_tmp7B8->f1=effect;_tmp7B8;});*_tmp847=_tmpD99;});}
# 4486
if(*_tmp848 != 0){
struct Cyc_List_List*bs=*_tmp848;for(0;bs != 0;bs=bs->tl){
void*_tmp7B9=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp7BA=_tmp7B9;struct Cyc_Core_Opt**_tmp7CA;struct Cyc_Absyn_Kind*_tmp7C9;struct Cyc_Core_Opt**_tmp7C8;struct Cyc_Core_Opt**_tmp7C7;struct Cyc_Core_Opt**_tmp7C6;struct Cyc_Core_Opt**_tmp7C5;struct Cyc_Core_Opt**_tmp7C4;struct Cyc_Core_Opt**_tmp7C3;struct Cyc_Core_Opt**_tmp7C2;struct Cyc_Core_Opt**_tmp7C1;struct Cyc_Core_Opt**_tmp7C0;switch(*((int*)_tmp7BA)){case 1U: _LLF1: _tmp7C0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LLF2:
# 4490
({struct Cyc_String_pa_PrintArg_struct _tmp7BD=({struct Cyc_String_pa_PrintArg_struct _tmpA96;_tmpA96.tag=0U,_tmpA96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmpA96;});void*_tmp7BB[1U];_tmp7BB[0]=& _tmp7BD;({unsigned int _tmpD9B=loc;struct _dyneither_ptr _tmpD9A=({const char*_tmp7BC="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp7BC,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpD9B,_tmpD9A,_tag_dyneither(_tmp7BB,sizeof(void*),1U));});});
# 4492
_tmp7C1=_tmp7C0;goto _LLF4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f2)->aliasqual == Cyc_Absyn_Top){_LLF3: _tmp7C1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LLF4:
 _tmp7C2=_tmp7C1;goto _LLF6;}else{goto _LL103;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF5: _tmp7C2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LLF6:
 _tmp7C3=_tmp7C2;goto _LLF8;case Cyc_Absyn_Aliasable: _LLF7: _tmp7C3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LLF8:
 _tmp7C5=_tmp7C3;goto _LLFA;default: _LLFD: _tmp7C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LLFE:
# 4499
 _tmp7C7=_tmp7C4;goto _LL100;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF9: _tmp7C5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LLFA:
# 4496
 _tmp7C6=_tmp7C5;goto _LLFC;case Cyc_Absyn_Aliasable: _LLFB: _tmp7C6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LLFC:
# 4498
({struct Cyc_Core_Opt*_tmpD9C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp7C6=_tmpD9C;});goto _LLF0;default: _LLFF: _tmp7C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LL100:
# 4501
({struct Cyc_Core_Opt*_tmpD9D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp7C7=_tmpD9D;});goto _LLF0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f2)->aliasqual == Cyc_Absyn_Top){_LL101: _tmp7C8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_LL102:
# 4503
({struct Cyc_Core_Opt*_tmpD9E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp7C8=_tmpD9E;});goto _LLF0;}else{goto _LL103;}default: _LL103: _tmp7CA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f1;_tmp7C9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BA)->f2;_LL104:
# 4505
({struct Cyc_Core_Opt*_tmpD9F=Cyc_Tcutil_kind_to_bound_opt(_tmp7C9);*_tmp7CA=_tmpD9F;});goto _LLF0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7BA)->f1)->kind == Cyc_Absyn_MemKind){_LL105: _LL106:
# 4507
({void*_tmp7BE=0U;({unsigned int _tmpDA1=loc;struct _dyneither_ptr _tmpDA0=({const char*_tmp7BF="functions can't abstract M types";_tag_dyneither(_tmp7BF,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpDA1,_tmpDA0,_tag_dyneither(_tmp7BE,sizeof(void*),0U));});});goto _LLF0;}else{_LL107: _LL108:
 goto _LLF0;}}_LLF0:;}}
# 4512
({struct Cyc_List_List*_tmpDA2=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp757.kind_env,*_tmp848);cvtenv.kind_env=_tmpDA2;});
({struct Cyc_List_List*_tmpDA3=Cyc_Tcutil_remove_bound_tvars_bool(_tmp757.r,_tmp757.free_vars,*_tmp848);_tmp757.free_vars=_tmpDA3;});
# 4515
{struct Cyc_List_List*tvs=_tmp757.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp7CB=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp7CC=_tmp7CB;struct Cyc_Absyn_Tvar*_tmp7CE;int _tmp7CD;_LL10A: _tmp7CE=_tmp7CC.f1;_tmp7CD=_tmp7CC.f2;_LL10B:;
({struct Cyc_List_List*_tmpDA4=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7CE,_tmp7CD);cvtenv.free_vars=_tmpDA4;});}}
# 4520
{struct Cyc_List_List*evs=_tmp757.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp7CF=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp7D0=_tmp7CF;void*_tmp7D2;int _tmp7D1;_LL10D: _tmp7D2=_tmp7D0.f1;_tmp7D1=_tmp7D0.f2;_LL10E:;
({struct Cyc_List_List*_tmpDA5=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp7D2,_tmp7D1);cvtenv.free_evars=_tmpDA5;});}}
# 4524
goto _LL0;};};};}case 10U: _LL23: _tmp849=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6CA)->f1;_LL24:
# 4527
 for(0;_tmp849 != 0;_tmp849=_tmp849->tl){
struct _tuple12*_tmp7DA=(struct _tuple12*)_tmp849->hd;
({struct Cyc_Tcutil_CVTEnv _tmpDA6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp7DA).f2,1,0);cvtenv=_tmpDA6;});
({int _tmpDA7=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp7DA).f1).print_const,(*_tmp7DA).f2);
# 4530
((*_tmp7DA).f1).real_const=_tmpDA7;});}
# 4533
goto _LL0;case 12U: _LL25: _tmp84B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6CA)->f1;_tmp84A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6CA)->f2;_LL26: {
# 4537
struct Cyc_List_List*prev_fields=0;
for(0;_tmp84A != 0;_tmp84A=_tmp84A->tl){
struct Cyc_Absyn_Aggrfield*_tmp7DB=(struct Cyc_Absyn_Aggrfield*)_tmp84A->hd;struct Cyc_Absyn_Aggrfield*_tmp7DC=_tmp7DB;struct _dyneither_ptr*_tmp7EC;struct Cyc_Absyn_Tqual*_tmp7EB;void*_tmp7EA;struct Cyc_Absyn_Exp*_tmp7E9;struct Cyc_List_List*_tmp7E8;struct Cyc_Absyn_Exp*_tmp7E7;_LL110: _tmp7EC=_tmp7DC->name;_tmp7EB=(struct Cyc_Absyn_Tqual*)& _tmp7DC->tq;_tmp7EA=_tmp7DC->type;_tmp7E9=_tmp7DC->width;_tmp7E8=_tmp7DC->attributes;_tmp7E7=_tmp7DC->requires_clause;_LL111:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7EC))
({struct Cyc_String_pa_PrintArg_struct _tmp7DF=({struct Cyc_String_pa_PrintArg_struct _tmpA98;_tmpA98.tag=0U,_tmpA98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7EC);_tmpA98;});void*_tmp7DD[1U];_tmp7DD[0]=& _tmp7DF;({unsigned int _tmpDA9=loc;struct _dyneither_ptr _tmpDA8=({const char*_tmp7DE="duplicate field %s";_tag_dyneither(_tmp7DE,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpDA9,_tmpDA8,_tag_dyneither(_tmp7DD,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpDAA=(struct _dyneither_ptr)*_tmp7EC;Cyc_strcmp(_tmpDAA,({const char*_tmp7E0="";_tag_dyneither(_tmp7E0,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpDAB=({struct Cyc_List_List*_tmp7E1=_cycalloc(sizeof(*_tmp7E1));_tmp7E1->hd=_tmp7EC,_tmp7E1->tl=prev_fields;_tmp7E1;});prev_fields=_tmpDAB;});
({struct Cyc_Tcutil_CVTEnv _tmpDAC=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7EA,1,0);cvtenv=_tmpDAC;});
({int _tmpDAD=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7EB->print_const,_tmp7EA);_tmp7EB->real_const=_tmpDAD;});
# 4550
Cyc_Tcutil_check_bitfield(loc,te,_tmp7EA,_tmp7E9,_tmp7EC);
Cyc_Tcutil_check_field_atts(loc,_tmp7EC,_tmp7E8);
if(_tmp7E7 != 0){
# 4554
if(_tmp84B != Cyc_Absyn_UnionA)
({void*_tmp7E2=0U;({unsigned int _tmpDAF=loc;struct _dyneither_ptr _tmpDAE=({const char*_tmp7E3="@requires clause is only allowed on union members";_tag_dyneither(_tmp7E3,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpDAF,_tmpDAE,_tag_dyneither(_tmp7E2,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpDB0=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDB0,0,_tmp7E7);});
if(!Cyc_Tcutil_is_integral(_tmp7E7))
({struct Cyc_String_pa_PrintArg_struct _tmp7E6=({struct Cyc_String_pa_PrintArg_struct _tmpA99;_tmpA99.tag=0U,({
struct _dyneither_ptr _tmpDB1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7E7->topt)));_tmpA99.f1=_tmpDB1;});_tmpA99;});void*_tmp7E4[1U];_tmp7E4[0]=& _tmp7E6;({unsigned int _tmpDB3=loc;struct _dyneither_ptr _tmpDB2=({const char*_tmp7E5="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp7E5,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpDB3,_tmpDB2,_tag_dyneither(_tmp7E4,sizeof(void*),1U));});});
({struct Cyc_Tcutil_CVTEnv _tmpDB4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7E7,te,cvtenv);cvtenv=_tmpDB4;});}}
# 4563
goto _LL0;}case 11U: _LL27: _tmp84D=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6CA)->f1).aggr_info;_tmp84C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6CA)->f1).targs;_LL28:
# 4566
{union Cyc_Absyn_AggrInfoU _tmp7ED=*_tmp84D;union Cyc_Absyn_AggrInfoU _tmp7EE=_tmp7ED;struct Cyc_Absyn_Aggrdecl*_tmp811;enum Cyc_Absyn_AggrKind _tmp810;struct _tuple2*_tmp80F;struct Cyc_Core_Opt*_tmp80E;if((_tmp7EE.UnknownAggr).tag == 1){_LL113: _tmp810=((_tmp7EE.UnknownAggr).val).f1;_tmp80F=((_tmp7EE.UnknownAggr).val).f2;_tmp80E=((_tmp7EE.UnknownAggr).val).f3;_LL114: {
# 4568
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp7EF;_push_handler(& _tmp7EF);{int _tmp7F1=0;if(setjmp(_tmp7EF.handler))_tmp7F1=1;if(!_tmp7F1){
({struct Cyc_Absyn_Aggrdecl**_tmpDB5=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp80F);adp=_tmpDB5;});{
struct Cyc_Absyn_Aggrdecl*_tmp7F2=*adp;
if(_tmp7F2->kind != _tmp810){
if(_tmp7F2->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp7F5=({struct Cyc_String_pa_PrintArg_struct _tmpA9B;_tmpA9B.tag=0U,({struct _dyneither_ptr _tmpDB6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F));_tmpA9B.f1=_tmpDB6;});_tmpA9B;});struct Cyc_String_pa_PrintArg_struct _tmp7F6=({struct Cyc_String_pa_PrintArg_struct _tmpA9A;_tmpA9A.tag=0U,({
struct _dyneither_ptr _tmpDB7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F));_tmpA9A.f1=_tmpDB7;});_tmpA9A;});void*_tmp7F3[2U];_tmp7F3[0]=& _tmp7F5,_tmp7F3[1]=& _tmp7F6;({unsigned int _tmpDB9=loc;struct _dyneither_ptr _tmpDB8=({const char*_tmp7F4="expecting struct %s instead of union %s";_tag_dyneither(_tmp7F4,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpDB9,_tmpDB8,_tag_dyneither(_tmp7F3,sizeof(void*),2U));});});else{
# 4577
({struct Cyc_String_pa_PrintArg_struct _tmp7F9=({struct Cyc_String_pa_PrintArg_struct _tmpA9D;_tmpA9D.tag=0U,({struct _dyneither_ptr _tmpDBA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F));_tmpA9D.f1=_tmpDBA;});_tmpA9D;});struct Cyc_String_pa_PrintArg_struct _tmp7FA=({struct Cyc_String_pa_PrintArg_struct _tmpA9C;_tmpA9C.tag=0U,({
struct _dyneither_ptr _tmpDBB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F));_tmpA9C.f1=_tmpDBB;});_tmpA9C;});void*_tmp7F7[2U];_tmp7F7[0]=& _tmp7F9,_tmp7F7[1]=& _tmp7FA;({unsigned int _tmpDBD=loc;struct _dyneither_ptr _tmpDBC=({const char*_tmp7F8="expecting union %s instead of struct %s";_tag_dyneither(_tmp7F8,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpDBD,_tmpDBC,_tag_dyneither(_tmp7F7,sizeof(void*),2U));});});}}
# 4580
if((unsigned int)_tmp80E  && (int)_tmp80E->v){
if(!((unsigned int)_tmp7F2->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7F2->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp7FD=({struct Cyc_String_pa_PrintArg_struct _tmpA9E;_tmpA9E.tag=0U,({
struct _dyneither_ptr _tmpDBE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F));_tmpA9E.f1=_tmpDBE;});_tmpA9E;});void*_tmp7FB[1U];_tmp7FB[0]=& _tmp7FD;({unsigned int _tmpDC0=loc;struct _dyneither_ptr _tmpDBF=({const char*_tmp7FC="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp7FC,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpDC0,_tmpDBF,_tag_dyneither(_tmp7FB,sizeof(void*),1U));});});}
# 4586
({union Cyc_Absyn_AggrInfoU _tmpDC1=Cyc_Absyn_KnownAggr(adp);*_tmp84D=_tmpDC1;});};
# 4570
;_pop_handler();}else{void*_tmp7F0=(void*)_exn_thrown;void*_tmp7FE=_tmp7F0;void*_tmp804;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7FE)->tag == Cyc_Dict_Absent){_LL118: _LL119: {
# 4590
struct Cyc_Absyn_Aggrdecl*_tmp7FF=({struct Cyc_Absyn_Aggrdecl*_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803->kind=_tmp810,_tmp803->sc=Cyc_Absyn_Extern,_tmp803->name=_tmp80F,_tmp803->tvs=0,_tmp803->impl=0,_tmp803->attributes=0,_tmp803->expected_mem_kind=0;_tmp803;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp7FF);
({struct Cyc_Absyn_Aggrdecl**_tmpDC2=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp80F);adp=_tmpDC2;});
({union Cyc_Absyn_AggrInfoU _tmpDC3=Cyc_Absyn_KnownAggr(adp);*_tmp84D=_tmpDC3;});
# 4595
if(*_tmp84C != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp802=({struct Cyc_String_pa_PrintArg_struct _tmpA9F;_tmpA9F.tag=0U,({
struct _dyneither_ptr _tmpDC4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp80F));_tmpA9F.f1=_tmpDC4;});_tmpA9F;});void*_tmp800[1U];_tmp800[0]=& _tmp802;({unsigned int _tmpDC6=loc;struct _dyneither_ptr _tmpDC5=({const char*_tmp801="declare parameterized type %s before using";_tag_dyneither(_tmp801,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpDC6,_tmpDC5,_tag_dyneither(_tmp800,sizeof(void*),1U));});});
return cvtenv;}
# 4600
goto _LL117;}}else{_LL11A: _tmp804=_tmp7FE;_LL11B:(int)_rethrow(_tmp804);}_LL117:;}};}
# 4602
_tmp811=*adp;goto _LL116;}}else{_LL115: _tmp811=*(_tmp7EE.KnownAggr).val;_LL116: {
# 4604
struct Cyc_List_List*tvs=_tmp811->tvs;
struct Cyc_List_List*ts=*_tmp84C;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp805=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp806=(void*)ts->hd;
# 4612
{struct _tuple0 _tmp807=({struct _tuple0 _tmpAA0;({void*_tmpDC7=Cyc_Absyn_compress_kb(_tmp805->kind);_tmpAA0.f1=_tmpDC7;}),_tmpAA0.f2=_tmp806;_tmpAA0;});struct _tuple0 _tmp808=_tmp807;struct Cyc_Absyn_Tvar*_tmp809;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp808.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp808.f2)->tag == 2U){_LL11D: _tmp809=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp808.f2)->f1;_LL11E:
# 4614
({struct Cyc_List_List*_tmpDC8=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp809);cvtenv.kind_env=_tmpDC8;});
({struct Cyc_List_List*_tmpDC9=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp809,1);cvtenv.free_vars=_tmpDC9;});
continue;}else{goto _LL11F;}}else{_LL11F: _LL120:
 goto _LL11C;}_LL11C:;}{
# 4619
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDCA=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpDCA;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4623
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp80C=({struct Cyc_String_pa_PrintArg_struct _tmpAA1;_tmpAA1.tag=0U,({struct _dyneither_ptr _tmpDCB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp811->name));_tmpAA1.f1=_tmpDCB;});_tmpAA1;});void*_tmp80A[1U];_tmp80A[0]=& _tmp80C;({unsigned int _tmpDCD=loc;struct _dyneither_ptr _tmpDCC=({const char*_tmp80B="too many parameters for type %s";_tag_dyneither(_tmp80B,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpDCD,_tmpDCC,_tag_dyneither(_tmp80A,sizeof(void*),1U));});});
if(tvs != 0){
# 4627
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDCE=({struct Cyc_List_List*_tmp80D=_cycalloc(sizeof(*_tmp80D));_tmp80D->hd=e,_tmp80D->tl=hidden_ts;_tmp80D;});hidden_ts=_tmpDCE;});
({struct Cyc_Tcutil_CVTEnv _tmpDCF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpDCF;});}
# 4634
({struct Cyc_List_List*_tmpDD1=({struct Cyc_List_List*_tmpDD0=*_tmp84C;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDD0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp84C=_tmpDD1;});}
# 4636
if((allow_abs_aggr  && _tmp811->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4641
_tmp811->expected_mem_kind=1;}}_LL112:;}
# 4644
goto _LL0;case 17U: _LL29: _tmp851=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CA)->f1;_tmp850=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CA)->f2;_tmp84F=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CA)->f3;_tmp84E=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6CA)->f4;_LL2A: {
# 4647
struct Cyc_List_List*targs=*_tmp850;
# 4649
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp812;_push_handler(& _tmp812);{int _tmp814=0;if(setjmp(_tmp812.handler))_tmp814=1;if(!_tmp814){({struct Cyc_Absyn_Typedefdecl*_tmpDD2=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp851);td=_tmpDD2;});;_pop_handler();}else{void*_tmp813=(void*)_exn_thrown;void*_tmp815=_tmp813;void*_tmp819;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp815)->tag == Cyc_Dict_Absent){_LL122: _LL123:
# 4652
({struct Cyc_String_pa_PrintArg_struct _tmp818=({struct Cyc_String_pa_PrintArg_struct _tmpAA2;_tmpAA2.tag=0U,({struct _dyneither_ptr _tmpDD3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp851));_tmpAA2.f1=_tmpDD3;});_tmpAA2;});void*_tmp816[1U];_tmp816[0]=& _tmp818;({unsigned int _tmpDD5=loc;struct _dyneither_ptr _tmpDD4=({const char*_tmp817="unbound typedef name %s";_tag_dyneither(_tmp817,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpDD5,_tmpDD4,_tag_dyneither(_tmp816,sizeof(void*),1U));});});
return cvtenv;}else{_LL124: _tmp819=_tmp815;_LL125:(int)_rethrow(_tmp819);}_LL121:;}};}
# 4655
*_tmp84F=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4660
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4665
({struct Cyc_Tcutil_CVTEnv _tmpDD6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpDD6;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpDD8=({struct Cyc_List_List*_tmp81B=_cycalloc(sizeof(*_tmp81B));({struct _tuple16*_tmpDD7=({struct _tuple16*_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp81A->f2=(void*)ts->hd;_tmp81A;});_tmp81B->hd=_tmpDD7;}),_tmp81B->tl=inst;_tmp81B;});inst=_tmpDD8;});}
# 4669
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp81E=({struct Cyc_String_pa_PrintArg_struct _tmpAA3;_tmpAA3.tag=0U,({struct _dyneither_ptr _tmpDD9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp851));_tmpAA3.f1=_tmpDD9;});_tmpAA3;});void*_tmp81C[1U];_tmp81C[0]=& _tmp81E;({unsigned int _tmpDDB=loc;struct _dyneither_ptr _tmpDDA=({const char*_tmp81D="too many parameters for typedef %s";_tag_dyneither(_tmp81D,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpDDB,_tmpDDA,_tag_dyneither(_tmp81C,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4674
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDDC=({struct Cyc_List_List*_tmp81F=_cycalloc(sizeof(*_tmp81F));_tmp81F->hd=e,_tmp81F->tl=hidden_ts;_tmp81F;});hidden_ts=_tmpDDC;});
({struct Cyc_Tcutil_CVTEnv _tmpDDD=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpDDD;});
({struct Cyc_List_List*_tmpDDF=({struct Cyc_List_List*_tmp821=_cycalloc(sizeof(*_tmp821));({struct _tuple16*_tmpDDE=({struct _tuple16*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp820->f2=e;_tmp820;});_tmp821->hd=_tmpDDE;}),_tmp821->tl=inst;_tmp821;});inst=_tmpDDF;});}
# 4682
({struct Cyc_List_List*_tmpDE1=({struct Cyc_List_List*_tmpDE0=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDE0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp850=_tmpDE1;});}
# 4684
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp84E=new_typ;}
# 4690
goto _LL0;};}case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL0;case 15U: _LL31: _tmp852=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6CA)->f1;_LL32:
# 4695
({struct Cyc_Tcutil_CVTEnv _tmpDE2=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp852,1,allow_abs_aggr);cvtenv=_tmpDE2;});goto _LL0;case 16U: _LL33: _tmp854=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6CA)->f1;_tmp853=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6CA)->f2;_LL34:
# 4698
({struct Cyc_Tcutil_CVTEnv _tmpDE3=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp854,0,0);cvtenv=_tmpDE3;});
({struct Cyc_Tcutil_CVTEnv _tmpDE4=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp853,1,0);cvtenv=_tmpDE4;});
goto _LL0;case 23U: _LL35: _tmp855=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6CA)->f1;_LL36:
# 4702
({struct Cyc_Tcutil_CVTEnv _tmpDE5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp855,1,1);cvtenv=_tmpDE5;});goto _LL0;case 25U: _LL37: _tmp856=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6CA)->f1;_LL38:
# 4704
({struct Cyc_Tcutil_CVTEnv _tmpDE6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp856,1,1);cvtenv=_tmpDE6;});goto _LL0;default: _LL39: _tmp857=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6CA)->f1;_LL3A:
# 4706
 for(0;_tmp857 != 0;_tmp857=_tmp857->tl){
({struct Cyc_Tcutil_CVTEnv _tmpDE7=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp857->hd,1,1);cvtenv=_tmpDE7;});}
goto _LL0;}_LL0:;}
# 4710
if(!({struct Cyc_Absyn_Kind*_tmpDE8=Cyc_Tcutil_typ_kind(t);Cyc_Tcutil_kind_leq(_tmpDE8,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp85A=({struct Cyc_String_pa_PrintArg_struct _tmpAA6;_tmpAA6.tag=0U,({
struct _dyneither_ptr _tmpDE9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAA6.f1=_tmpDE9;});_tmpAA6;});struct Cyc_String_pa_PrintArg_struct _tmp85B=({struct Cyc_String_pa_PrintArg_struct _tmpAA5;_tmpAA5.tag=0U,({struct _dyneither_ptr _tmpDEA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));_tmpAA5.f1=_tmpDEA;});_tmpAA5;});struct Cyc_String_pa_PrintArg_struct _tmp85C=({struct Cyc_String_pa_PrintArg_struct _tmpAA4;_tmpAA4.tag=0U,({struct _dyneither_ptr _tmpDEB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmpAA4.f1=_tmpDEB;});_tmpAA4;});void*_tmp858[3U];_tmp858[0]=& _tmp85A,_tmp858[1]=& _tmp85B,_tmp858[2]=& _tmp85C;({unsigned int _tmpDED=loc;struct _dyneither_ptr _tmpDEC=({const char*_tmp859="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp859,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpDED,_tmpDEC,_tag_dyneither(_tmp858,sizeof(void*),3U));});});
# 4714
return cvtenv;}
# 4722
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4724
{void*_tmp85D=e->r;void*_tmp85E=_tmp85D;struct Cyc_Absyn_Exp*_tmp870;void*_tmp86F;void*_tmp86E;void*_tmp86D;void*_tmp86C;struct Cyc_Absyn_Exp*_tmp86B;struct Cyc_Absyn_Exp*_tmp86A;struct Cyc_Absyn_Exp*_tmp869;struct Cyc_Absyn_Exp*_tmp868;struct Cyc_Absyn_Exp*_tmp867;struct Cyc_Absyn_Exp*_tmp866;struct Cyc_Absyn_Exp*_tmp865;struct Cyc_Absyn_Exp*_tmp864;struct Cyc_Absyn_Exp*_tmp863;struct Cyc_Absyn_Exp*_tmp862;struct Cyc_List_List*_tmp861;switch(*((int*)_tmp85E)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp861=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp85E)->f2;_LLC:
# 4731
 for(0;_tmp861 != 0;_tmp861=_tmp861->tl){
({struct Cyc_Tcutil_CVTEnv _tmpDEE=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp861->hd,te,cvtenv);cvtenv=_tmpDEE;});}
goto _LL0;case 6U: _LLD: _tmp864=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_tmp863=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp85E)->f2;_tmp862=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp85E)->f3;_LLE:
# 4735
({struct Cyc_Tcutil_CVTEnv _tmpDEF=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp864,te,cvtenv);cvtenv=_tmpDEF;});
({struct Cyc_Tcutil_CVTEnv _tmpDF0=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp863,te,cvtenv);cvtenv=_tmpDF0;});
({struct Cyc_Tcutil_CVTEnv _tmpDF1=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp862,te,cvtenv);cvtenv=_tmpDF1;});
goto _LL0;case 7U: _LLF: _tmp866=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_tmp865=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp85E)->f2;_LL10:
 _tmp868=_tmp866;_tmp867=_tmp865;goto _LL12;case 8U: _LL11: _tmp868=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_tmp867=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp85E)->f2;_LL12:
 _tmp86A=_tmp868;_tmp869=_tmp867;goto _LL14;case 9U: _LL13: _tmp86A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_tmp869=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp85E)->f2;_LL14:
# 4742
({struct Cyc_Tcutil_CVTEnv _tmpDF2=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp86A,te,cvtenv);cvtenv=_tmpDF2;});
({struct Cyc_Tcutil_CVTEnv _tmpDF3=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp869,te,cvtenv);cvtenv=_tmpDF3;});
goto _LL0;case 14U: _LL15: _tmp86C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_tmp86B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp85E)->f2;_LL16:
# 4746
({struct Cyc_Tcutil_CVTEnv _tmpDF4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp86B,te,cvtenv);cvtenv=_tmpDF4;});
({struct Cyc_Tcutil_CVTEnv _tmpDF5=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp86C,1,0);cvtenv=_tmpDF5;});
goto _LL0;case 19U: _LL17: _tmp86D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_LL18:
 _tmp86E=_tmp86D;goto _LL1A;case 17U: _LL19: _tmp86E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_LL1A:
# 4751
({struct Cyc_Tcutil_CVTEnv _tmpDF6=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp86E,1,0);cvtenv=_tmpDF6;});
goto _LL0;case 39U: _LL1B: _tmp86F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_LL1C:
# 4754
({struct Cyc_Tcutil_CVTEnv _tmpDF7=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp86F,1,0);cvtenv=_tmpDF7;});
goto _LL0;case 18U: _LL1D: _tmp870=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp85E)->f1;_LL1E:
# 4757
({struct Cyc_Tcutil_CVTEnv _tmpDF8=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp870,te,cvtenv);cvtenv=_tmpDF8;});
goto _LL0;default: _LL1F: _LL20:
# 4760
({void*_tmp85F=0U;({struct _dyneither_ptr _tmpDF9=({const char*_tmp860="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp860,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDF9,_tag_dyneither(_tmp85F,sizeof(void*),0U));});});}_LL0:;}
# 4762
return cvtenv;}
# 4765
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4770
struct Cyc_List_List*_tmp871=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpDFA=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpDFA;});
# 4773
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp872=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp873=_tmp872;struct Cyc_Absyn_Tvar*_tmp874;_LL1: _tmp874=_tmp873.f1;_LL2:;
({struct Cyc_List_List*_tmpDFB=Cyc_Tcutil_fast_add_free_tvar(_tmp871,_tmp874);cvt.kind_env=_tmpDFB;});}}
# 4781
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp875=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp876=_tmp875;void*_tmp87F;_LL4: _tmp87F=_tmp876.f1;_LL5:;{
void*_tmp877=Cyc_Tcutil_compress(_tmp87F);void*_tmp878=_tmp877;struct Cyc_Core_Opt**_tmp87E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp878)->tag == 1U){_LL7: _tmp87E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp878)->f4;_LL8:
# 4785
 if(*_tmp87E == 0)
({struct Cyc_Core_Opt*_tmpDFC=({struct Cyc_Core_Opt*_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879->v=_tmp871;_tmp879;});*_tmp87E=_tmpDFC;});else{
# 4789
struct Cyc_List_List*_tmp87A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp87E))->v;
struct Cyc_List_List*_tmp87B=0;
for(0;_tmp87A != 0;_tmp87A=_tmp87A->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp871,(struct Cyc_Absyn_Tvar*)_tmp87A->hd))
({struct Cyc_List_List*_tmpDFD=({struct Cyc_List_List*_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C->hd=(struct Cyc_Absyn_Tvar*)_tmp87A->hd,_tmp87C->tl=_tmp87B;_tmp87C;});_tmp87B=_tmpDFD;});}
({struct Cyc_Core_Opt*_tmpDFE=({struct Cyc_Core_Opt*_tmp87D=_cycalloc(sizeof(*_tmp87D));_tmp87D->v=_tmp87B;_tmp87D;});*_tmp87E=_tmpDFE;});}
# 4796
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4800
return cvt;}
# 4806
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp880=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp881=_tmp880;switch(((struct Cyc_Absyn_Kind*)_tmp881)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp881)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
# 4811
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp882=0U;({struct _dyneither_ptr _tmpDFF=({const char*_tmp883="can't unify evar with unique region!";_tag_dyneither(_tmp883,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDFF,_tag_dyneither(_tmp882,sizeof(void*),0U));});});
goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 goto _LL6;default: _LL5: _LL6:
# 4816
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp884=0U;({struct _dyneither_ptr _tmpE00=({const char*_tmp885="can't unify evar with heap!";_tag_dyneither(_tmp885,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE00,_tag_dyneither(_tmp884,sizeof(void*),0U));});});
goto _LL0;}case Cyc_Absyn_EffKind: _LL7: _LL8:
# 4819
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp886=0U;({struct _dyneither_ptr _tmpE01=({const char*_tmp887="can't unify evar with {}!";_tag_dyneither(_tmp887,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE01,_tag_dyneither(_tmp886,sizeof(void*),0U));});});
goto _LL0;default: _LL9: _LLA:
# 4822
({struct Cyc_String_pa_PrintArg_struct _tmp88A=({struct Cyc_String_pa_PrintArg_struct _tmpAA8;_tmpAA8.tag=0U,({
struct _dyneither_ptr _tmpE02=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmpAA8.f1=_tmpE02;});_tmpAA8;});struct Cyc_String_pa_PrintArg_struct _tmp88B=({struct Cyc_String_pa_PrintArg_struct _tmpAA7;_tmpAA7.tag=0U,({struct _dyneither_ptr _tmpE03=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmpAA7.f1=_tmpE03;});_tmpAA7;});void*_tmp888[2U];_tmp888[0]=& _tmp88A,_tmp888[1]=& _tmp88B;({unsigned int _tmpE05=loc;struct _dyneither_ptr _tmpE04=({const char*_tmp889="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp889,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpE05,_tmpE04,_tag_dyneither(_tmp888,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}}
# 4834
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp88C=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp88D=({unsigned int _tmpE09=loc;struct Cyc_Tcenv_Tenv*_tmpE08=te;struct Cyc_Tcutil_CVTEnv _tmpE07=({struct Cyc_Tcutil_CVTEnv _tmpAAC;_tmpAAC.r=Cyc_Core_heap_region,_tmpAAC.kind_env=_tmp88C,_tmpAAC.free_vars=0,_tmpAAC.free_evars=0,_tmpAAC.generalize_evars=generalize_evars,_tmpAAC.fn_result=0;_tmpAAC;});struct Cyc_Absyn_Kind*_tmpE06=expected_kind;Cyc_Tcutil_check_valid_type(_tmpE09,_tmpE08,_tmpE07,_tmpE06,1,t);});
# 4842
struct Cyc_List_List*_tmp88E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp88D.free_vars);
struct Cyc_List_List*_tmp88F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp88D.free_evars);
# 4846
if(_tmp88C != 0){
struct Cyc_List_List*_tmp890=0;
{struct Cyc_List_List*_tmp891=_tmp88E;for(0;(unsigned int)_tmp891;_tmp891=_tmp891->tl){
struct Cyc_Absyn_Tvar*_tmp892=(struct Cyc_Absyn_Tvar*)_tmp891->hd;
int found=0;
{struct Cyc_List_List*_tmp893=_tmp88C;for(0;(unsigned int)_tmp893;_tmp893=_tmp893->tl){
if(Cyc_Absyn_tvar_cmp(_tmp892,(struct Cyc_Absyn_Tvar*)_tmp893->hd)== 0){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpE0A=({struct Cyc_List_List*_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894->hd=(struct Cyc_Absyn_Tvar*)_tmp891->hd,_tmp894->tl=_tmp890;_tmp894;});_tmp890=_tmpE0A;});}}
# 4856
({struct Cyc_List_List*_tmpE0B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp890);_tmp88E=_tmpE0B;});}
# 4862
{struct Cyc_List_List*x=_tmp88E;for(0;x != 0;x=x->tl){
void*_tmp895=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp896=_tmp895;enum Cyc_Absyn_AliasQual _tmp8A4;struct Cyc_Core_Opt**_tmp8A3;struct Cyc_Absyn_Kind*_tmp8A2;struct Cyc_Core_Opt**_tmp8A1;struct Cyc_Core_Opt**_tmp8A0;struct Cyc_Core_Opt**_tmp89F;struct Cyc_Core_Opt**_tmp89E;struct Cyc_Core_Opt**_tmp89D;struct Cyc_Core_Opt**_tmp89C;switch(*((int*)_tmp896)){case 1U: _LL1: _tmp89C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp896)->f1;_LL2:
 _tmp89D=_tmp89C;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp89D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f1;_LL4:
 _tmp89E=_tmp89D;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp89E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f1;_LL6:
 _tmp89F=_tmp89E;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp89F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f1;_LL8:
# 4868
({struct Cyc_Core_Opt*_tmpE0C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp89F=_tmpE0C;});goto _LL0;default: _LL9: _tmp8A0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f1;_LLA:
# 4870
({struct Cyc_Core_Opt*_tmpE0D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp8A0=_tmpE0D;});goto _LL0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp8A1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f1;_LLC:
# 4872
({struct Cyc_Core_Opt*_tmpE0E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp8A1=_tmpE0E;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp8A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f1;_tmp8A2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp896)->f2;_LLE:
# 4874
({struct Cyc_Core_Opt*_tmpE0F=Cyc_Tcutil_kind_to_bound_opt(_tmp8A2);*_tmp8A3=_tmpE0F;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp896)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp8A4=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp896)->f1)->aliasqual;_LL10:
# 4876
({struct Cyc_String_pa_PrintArg_struct _tmp899=({struct Cyc_String_pa_PrintArg_struct _tmpAAA;_tmpAAA.tag=0U,({
struct _dyneither_ptr _tmpE10=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmpAAA.f1=_tmpE10;});_tmpAAA;});struct Cyc_String_pa_PrintArg_struct _tmp89A=({struct Cyc_String_pa_PrintArg_struct _tmpAA9;_tmpAA9.tag=0U,({struct _dyneither_ptr _tmpE11=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B->kind=Cyc_Absyn_MemKind,_tmp89B->aliasqual=_tmp8A4;_tmp89B;})));_tmpAA9.f1=_tmpE11;});_tmpAA9;});void*_tmp897[2U];_tmp897[0]=& _tmp899,_tmp897[1]=& _tmp89A;({unsigned int _tmpE13=loc;struct _dyneither_ptr _tmpE12=({const char*_tmp898="type variable %s cannot have kind %s";_tag_dyneither(_tmp898,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpE13,_tmpE12,_tag_dyneither(_tmp897,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4883
if(_tmp88E != 0  || _tmp88F != 0){
{void*_tmp8A5=Cyc_Tcutil_compress(t);void*_tmp8A6=_tmp8A5;struct Cyc_List_List**_tmp8A7;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8A6)->tag == 9U){_LL14: _tmp8A7=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8A6)->f1).tvars;_LL15:
# 4886
 if(*_tmp8A7 == 0){
# 4888
({struct Cyc_List_List*_tmpE14=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp88E);*_tmp8A7=_tmpE14;});
_tmp88E=0;}
# 4891
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4894
if(_tmp88E != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8AA=({struct Cyc_String_pa_PrintArg_struct _tmpAAB;_tmpAAB.tag=0U,_tmpAAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp88E->hd)->name);_tmpAAB;});void*_tmp8A8[1U];_tmp8A8[0]=& _tmp8AA;({unsigned int _tmpE16=loc;struct _dyneither_ptr _tmpE15=({const char*_tmp8A9="unbound type variable %s";_tag_dyneither(_tmp8A9,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE16,_tmpE15,_tag_dyneither(_tmp8A8,sizeof(void*),1U));});});
Cyc_Tcutil_check_free_evars(_tmp88F,t,loc);}}
# 4904
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4907
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp8AB=Cyc_Tcutil_compress(t);void*_tmp8AC=_tmp8AB;struct Cyc_List_List*_tmp8B8;void*_tmp8B7;struct Cyc_Absyn_Tqual _tmp8B6;void*_tmp8B5;struct Cyc_List_List*_tmp8B4;struct Cyc_Absyn_Exp*_tmp8B3;struct Cyc_List_List*_tmp8B2;struct Cyc_Absyn_Exp*_tmp8B1;struct Cyc_List_List*_tmp8B0;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->tag == 9U){_LL1: _tmp8B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).tvars;_tmp8B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).effect;_tmp8B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).ret_tqual;_tmp8B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).ret_typ;_tmp8B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).args;_tmp8B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).requires_clause;_tmp8B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).requires_relns;_tmp8B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).ensures_clause;_tmp8B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8AC)->f1).ensures_relns;_LL2:
# 4910
 fd->tvs=_tmp8B8;
fd->effect=_tmp8B7;
{struct Cyc_List_List*_tmp8AD=fd->args;for(0;_tmp8AD != 0;(_tmp8AD=_tmp8AD->tl,_tmp8B4=_tmp8B4->tl)){
# 4914
(*((struct _tuple10*)_tmp8AD->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp8B4))->hd)).f2;
(*((struct _tuple10*)_tmp8AD->hd)).f3=(*((struct _tuple10*)_tmp8B4->hd)).f3;}}
# 4917
fd->ret_tqual=_tmp8B6;
fd->ret_type=_tmp8B5;
({int _tmpE17=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp8B5);(fd->ret_tqual).real_const=_tmpE17;});
fd->requires_clause=_tmp8B3;
fd->requires_relns=_tmp8B2;
fd->ensures_clause=_tmp8B1;
fd->ensures_relns=_tmp8B0;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp8AE=0U;({struct _dyneither_ptr _tmpE18=({const char*_tmp8AF="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp8AF,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE18,_tag_dyneither(_tmp8AE,sizeof(void*),0U));});});}_LL0:;}
# 4927
({struct Cyc_List_List*_tmpE1A=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpE19=loc;Cyc_Tcutil_check_unique_vars(_tmpE1A,_tmpE19,({const char*_tmp8B9="function declaration has repeated parameter";_tag_dyneither(_tmp8B9,sizeof(char),44U);}));});
# 4930
fd->cached_typ=t;}
# 4935
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4941
struct Cyc_Tcutil_CVTEnv _tmp8BA=({unsigned int _tmpE1F=loc;struct Cyc_Tcenv_Tenv*_tmpE1E=te;struct Cyc_Tcutil_CVTEnv _tmpE1D=({struct Cyc_Tcutil_CVTEnv _tmpAAF;_tmpAAF.r=Cyc_Core_heap_region,_tmpAAF.kind_env=bound_tvars,_tmpAAF.free_vars=0,_tmpAAF.free_evars=0,_tmpAAF.generalize_evars=0,_tmpAAF.fn_result=0;_tmpAAF;});struct Cyc_Absyn_Kind*_tmpE1C=expected_kind;int _tmpE1B=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpE1F,_tmpE1E,_tmpE1D,_tmpE1C,_tmpE1B,t);});
# 4945
struct Cyc_List_List*_tmp8BB=({struct _RegionHandle*_tmpE21=Cyc_Core_heap_region;struct Cyc_List_List*_tmpE20=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp8BA.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpE21,_tmpE20,bound_tvars);});
# 4947
struct Cyc_List_List*_tmp8BC=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp8BA.free_evars);
{struct Cyc_List_List*fs=_tmp8BB;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp8BD=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp8C0=({struct Cyc_String_pa_PrintArg_struct _tmpAAE;_tmpAAE.tag=0U,_tmpAAE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8BD);_tmpAAE;});struct Cyc_String_pa_PrintArg_struct _tmp8C1=({struct Cyc_String_pa_PrintArg_struct _tmpAAD;_tmpAAD.tag=0U,({struct _dyneither_ptr _tmpE22=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAAD.f1=_tmpE22;});_tmpAAD;});void*_tmp8BE[2U];_tmp8BE[0]=& _tmp8C0,_tmp8BE[1]=& _tmp8C1;({unsigned int _tmpE24=loc;struct _dyneither_ptr _tmpE23=({const char*_tmp8BF="unbound type variable %s in type %s";_tag_dyneither(_tmp8BF,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpE24,_tmpE23,_tag_dyneither(_tmp8BE,sizeof(void*),2U));});});}}
# 4952
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp8BC,t,loc);}
# 4956
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpE25=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpE25;});}
# 4961
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4967
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4970
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8C4=({struct Cyc_String_pa_PrintArg_struct _tmpAB1;_tmpAB1.tag=0U,_tmpAB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpAB1;});struct Cyc_String_pa_PrintArg_struct _tmp8C5=({struct Cyc_String_pa_PrintArg_struct _tmpAB0;_tmpAB0.tag=0U,({struct _dyneither_ptr _tmpE26=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmpAB0.f1=_tmpE26;});_tmpAB0;});void*_tmp8C2[2U];_tmp8C2[0]=& _tmp8C4,_tmp8C2[1]=& _tmp8C5;({unsigned int _tmpE28=loc;struct _dyneither_ptr _tmpE27=({const char*_tmp8C3="%s: %s";_tag_dyneither(_tmp8C3,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpE28,_tmpE27,_tag_dyneither(_tmp8C2,sizeof(void*),2U));});});}}}
# 4976
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4980
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4984
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpE2A=tvs;unsigned int _tmpE29=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpE2A,_tmpE29,Cyc_Absynpp_tvar2string,({const char*_tmp8C6="duplicate type variable";_tag_dyneither(_tmp8C6,sizeof(char),24U);}));});}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4998 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5003
struct _RegionHandle _tmp8C7=_new_region("temp");struct _RegionHandle*temp=& _tmp8C7;_push_region(temp);
# 5007
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpE2B=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpE2B,({const char*_tmp8C8="";_tag_dyneither(_tmp8C8,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpE2D=({struct Cyc_List_List*_tmp8CA=_region_malloc(temp,sizeof(*_tmp8CA));({struct _tuple32*_tmpE2C=({struct _tuple32*_tmp8C9=_region_malloc(temp,sizeof(*_tmp8C9));_tmp8C9->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp8C9->f2=0;_tmp8C9;});_tmp8CA->hd=_tmpE2C;}),_tmp8CA->tl=fields;_tmp8CA;});fields=_tmpE2D;});}}
# 5012
({struct Cyc_List_List*_tmpE2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpE2E;});{
# 5014
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp8EF="struct";_tag_dyneither(_tmp8EF,sizeof(char),7U);}):({const char*_tmp8F0="union";_tag_dyneither(_tmp8F0,sizeof(char),6U);});
# 5017
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp8CB=(struct _tuple33*)des->hd;struct _tuple33*_tmp8CC=_tmp8CB;struct Cyc_List_List*_tmp8E7;void*_tmp8E6;_LL1: _tmp8E7=_tmp8CC->f1;_tmp8E6=_tmp8CC->f2;_LL2:;
if(_tmp8E7 == 0){
# 5022
struct Cyc_List_List*_tmp8CD=fields;
for(0;_tmp8CD != 0;_tmp8CD=_tmp8CD->tl){
if(!(*((struct _tuple32*)_tmp8CD->hd)).f2){
(*((struct _tuple32*)_tmp8CD->hd)).f2=1;
({struct Cyc_List_List*_tmpE30=({struct Cyc_List_List*_tmp8CF=_cycalloc(sizeof(*_tmp8CF));({void*_tmpE2F=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp8CE=_cycalloc(sizeof(*_tmp8CE));_tmp8CE->tag=1U,_tmp8CE->f1=((*((struct _tuple32*)_tmp8CD->hd)).f1)->name;_tmp8CE;});_tmp8CF->hd=_tmpE2F;}),_tmp8CF->tl=0;_tmp8CF;});(*((struct _tuple33*)des->hd)).f1=_tmpE30;});
({struct Cyc_List_List*_tmpE32=({struct Cyc_List_List*_tmp8D1=_region_malloc(rgn,sizeof(*_tmp8D1));({struct _tuple34*_tmpE31=({struct _tuple34*_tmp8D0=_region_malloc(rgn,sizeof(*_tmp8D0));_tmp8D0->f1=(*((struct _tuple32*)_tmp8CD->hd)).f1,_tmp8D0->f2=_tmp8E6;_tmp8D0;});_tmp8D1->hd=_tmpE31;}),_tmp8D1->tl=ans;_tmp8D1;});ans=_tmpE32;});
break;}}
# 5030
if(_tmp8CD == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8D4=({struct Cyc_String_pa_PrintArg_struct _tmpAB2;_tmpAB2.tag=0U,_tmpAB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpAB2;});void*_tmp8D2[1U];_tmp8D2[0]=& _tmp8D4;({unsigned int _tmpE34=loc;struct _dyneither_ptr _tmpE33=({const char*_tmp8D3="too many arguments to %s";_tag_dyneither(_tmp8D3,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE34,_tmpE33,_tag_dyneither(_tmp8D2,sizeof(void*),1U));});});}else{
if(_tmp8E7->tl != 0)
# 5034
({void*_tmp8D5=0U;({unsigned int _tmpE36=loc;struct _dyneither_ptr _tmpE35=({const char*_tmp8D6="multiple designators are not yet supported";_tag_dyneither(_tmp8D6,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpE36,_tmpE35,_tag_dyneither(_tmp8D5,sizeof(void*),0U));});});else{
# 5037
void*_tmp8D7=(void*)_tmp8E7->hd;void*_tmp8D8=_tmp8D7;struct _dyneither_ptr*_tmp8E5;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp8D8)->tag == 0U){_LL4: _LL5:
# 5039
({struct Cyc_String_pa_PrintArg_struct _tmp8DB=({struct Cyc_String_pa_PrintArg_struct _tmpAB3;_tmpAB3.tag=0U,_tmpAB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpAB3;});void*_tmp8D9[1U];_tmp8D9[0]=& _tmp8DB;({unsigned int _tmpE38=loc;struct _dyneither_ptr _tmpE37=({const char*_tmp8DA="array designator used in argument to %s";_tag_dyneither(_tmp8DA,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpE38,_tmpE37,_tag_dyneither(_tmp8D9,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp8E5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp8D8)->f1;_LL7: {
# 5042
struct Cyc_List_List*_tmp8DC=fields;
for(0;_tmp8DC != 0;_tmp8DC=_tmp8DC->tl){
if(Cyc_strptrcmp(_tmp8E5,((*((struct _tuple32*)_tmp8DC->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp8DC->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp8DF=({struct Cyc_String_pa_PrintArg_struct _tmpAB4;_tmpAB4.tag=0U,_tmpAB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8E5);_tmpAB4;});void*_tmp8DD[1U];_tmp8DD[0]=& _tmp8DF;({unsigned int _tmpE3A=loc;struct _dyneither_ptr _tmpE39=({const char*_tmp8DE="member %s has already been used as an argument";_tag_dyneither(_tmp8DE,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpE3A,_tmpE39,_tag_dyneither(_tmp8DD,sizeof(void*),1U));});});
(*((struct _tuple32*)_tmp8DC->hd)).f2=1;
({struct Cyc_List_List*_tmpE3C=({struct Cyc_List_List*_tmp8E1=_region_malloc(rgn,sizeof(*_tmp8E1));({struct _tuple34*_tmpE3B=({struct _tuple34*_tmp8E0=_region_malloc(rgn,sizeof(*_tmp8E0));_tmp8E0->f1=(*((struct _tuple32*)_tmp8DC->hd)).f1,_tmp8E0->f2=_tmp8E6;_tmp8E0;});_tmp8E1->hd=_tmpE3B;}),_tmp8E1->tl=ans;_tmp8E1;});ans=_tmpE3C;});
break;}}
# 5051
if(_tmp8DC == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8E4=({struct Cyc_String_pa_PrintArg_struct _tmpAB5;_tmpAB5.tag=0U,_tmpAB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8E5);_tmpAB5;});void*_tmp8E2[1U];_tmp8E2[0]=& _tmp8E4;({unsigned int _tmpE3E=loc;struct _dyneither_ptr _tmpE3D=({const char*_tmp8E3="bad field designator %s";_tag_dyneither(_tmp8E3,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpE3E,_tmpE3D,_tag_dyneither(_tmp8E2,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 5056
if(aggr_kind == Cyc_Absyn_StructA)
# 5058
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp8E8=0U;({unsigned int _tmpE40=loc;struct _dyneither_ptr _tmpE3F=({const char*_tmp8E9="too few arguments to struct";_tag_dyneither(_tmp8E9,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpE40,_tmpE3F,_tag_dyneither(_tmp8E8,sizeof(void*),0U));});});
break;}}else{
# 5065
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp8EA=0U;({unsigned int _tmpE42=loc;struct _dyneither_ptr _tmpE41=({const char*_tmp8EB="only one member of a union is allowed";_tag_dyneither(_tmp8EB,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpE42,_tmpE41,_tag_dyneither(_tmp8EA,sizeof(void*),0U));});});
found=1;}}
# 5072
if(!found)({void*_tmp8EC=0U;({unsigned int _tmpE44=loc;struct _dyneither_ptr _tmpE43=({const char*_tmp8ED="missing member for union";_tag_dyneither(_tmp8ED,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE44,_tmpE43,_tag_dyneither(_tmp8EC,sizeof(void*),0U));});});}{
# 5075
struct Cyc_List_List*_tmp8EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp8EE;};};}
# 5007
;_pop_region(temp);}
# 5078
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8F1=Cyc_Tcutil_compress(t);void*_tmp8F2=_tmp8F1;void*_tmp8F6;union Cyc_Absyn_Constraint*_tmp8F5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F2)->tag == 5U){_LL1: _tmp8F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F2)->f1).elt_typ;_tmp8F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F2)->f1).ptr_atts).bounds;_LL2: {
# 5081
void*_tmp8F3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8F5);void*_tmp8F4=_tmp8F3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8F4)->tag == 0U){_LL6: _LL7:
# 5083
*elt_typ_dest=_tmp8F6;
return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 5087
 return 0;}_LL0:;}
# 5091
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8F7=Cyc_Tcutil_compress(t);void*_tmp8F8=_tmp8F7;void*_tmp8FA;union Cyc_Absyn_Constraint*_tmp8F9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F8)->tag == 5U){_LL1: _tmp8FA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F8)->f1).elt_typ;_tmp8F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F8)->f1).ptr_atts).zero_term;_LL2:
# 5094
*elt_typ_dest=_tmp8FA;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8F9);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5102
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5104
void*_tmp8FB=Cyc_Tcutil_compress(t);void*_tmp8FC=_tmp8FB;void*_tmp905;struct Cyc_Absyn_Tqual _tmp904;struct Cyc_Absyn_Exp*_tmp903;union Cyc_Absyn_Constraint*_tmp902;void*_tmp901;union Cyc_Absyn_Constraint*_tmp900;union Cyc_Absyn_Constraint*_tmp8FF;switch(*((int*)_tmp8FC)){case 5U: _LL1: _tmp901=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FC)->f1).elt_typ;_tmp900=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FC)->f1).ptr_atts).bounds;_tmp8FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FC)->f1).ptr_atts).zero_term;_LL2:
# 5106
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8FF)){
*ptr_type=t;
*elt_type=_tmp901;
{void*_tmp8FD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp900);void*_tmp8FE=_tmp8FD;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8FE)->tag == 0U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 5113
return 1;}else{
return 0;}case 8U: _LL3: _tmp905=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8FC)->f1).elt_type;_tmp904=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8FC)->f1).tq;_tmp903=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8FC)->f1).num_elts;_tmp902=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8FC)->f1).zero_term;_LL4:
# 5116
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp902)){
*elt_type=_tmp905;
*is_dyneither=0;
({void*_tmpE45=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);*ptr_type=_tmpE45;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5129
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5131
void*_tmp906=e1->r;void*_tmp907=_tmp906;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_Absyn_Exp*_tmp918;struct Cyc_Absyn_Exp*_tmp917;struct Cyc_Absyn_Exp*_tmp916;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Exp*_tmp914;switch(*((int*)_tmp907)){case 14U: _LL1: _LL2:
# 5133
({struct Cyc_String_pa_PrintArg_struct _tmp90A=({struct Cyc_String_pa_PrintArg_struct _tmpAB6;_tmpAB6.tag=0U,({struct _dyneither_ptr _tmpE46=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAB6.f1=_tmpE46;});_tmpAB6;});void*_tmp908[1U];_tmp908[0]=& _tmp90A;({struct _dyneither_ptr _tmpE47=({const char*_tmp909="we have a cast in a lhs:  %s";_tag_dyneither(_tmp909,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE47,_tag_dyneither(_tmp908,sizeof(void*),1U));});});case 20U: _LL3: _tmp914=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LL4:
 _tmp915=_tmp914;goto _LL6;case 23U: _LL5: _tmp915=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LL6:
# 5136
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp915->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp916=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LL8:
 _tmp917=_tmp916;goto _LLA;case 21U: _LL9: _tmp917=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LLA:
# 5140
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp917->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp90D=({struct Cyc_String_pa_PrintArg_struct _tmpAB7;_tmpAB7.tag=0U,({
struct _dyneither_ptr _tmpE48=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAB7.f1=_tmpE48;});_tmpAB7;});void*_tmp90B[1U];_tmp90B[0]=& _tmp90D;({struct _dyneither_ptr _tmpE49=({const char*_tmp90C="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp90C,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE49,_tag_dyneither(_tmp90B,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp918=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LLC:
 _tmp919=_tmp918;goto _LLE;case 12U: _LLD: _tmp919=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LLE:
# 5146
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp919->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp910=({struct Cyc_String_pa_PrintArg_struct _tmpAB8;_tmpAB8.tag=0U,({
struct _dyneither_ptr _tmpE4A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAB8.f1=_tmpE4A;});_tmpAB8;});void*_tmp90E[1U];_tmp90E[0]=& _tmp910;({struct _dyneither_ptr _tmpE4B=({const char*_tmp90F="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp90F,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE4B,_tag_dyneither(_tmp90E,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp913=({struct Cyc_String_pa_PrintArg_struct _tmpAB9;_tmpAB9.tag=0U,({struct _dyneither_ptr _tmpE4C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAB9.f1=_tmpE4C;});_tmpAB9;});void*_tmp911[1U];_tmp911[0]=& _tmp913;({struct _dyneither_ptr _tmpE4D=({const char*_tmp912="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp912,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE4D,_tag_dyneither(_tmp911,sizeof(void*),1U));});});}_LL0:;}
# 5155
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5166
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5169
void*_tmp91A=Cyc_Tcutil_compress(r);void*_tmp91B=_tmp91A;struct Cyc_Absyn_Tvar*_tmp927;enum Cyc_Absyn_KindQual _tmp926;enum Cyc_Absyn_AliasQual _tmp925;switch(*((int*)_tmp91B)){case 22U: _LL1: _LL2:
 return !must_be_unique;case 21U: _LL3: _LL4:
 return 1;case 17U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp91B)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp91B)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp91B)->f4 == 0){_LL5: _tmp926=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp91B)->f3)->kind)->v)->kind;_tmp925=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp91B)->f3)->kind)->v)->aliasqual;_LL6:
# 5173
 return _tmp926 == Cyc_Absyn_RgnKind  && (_tmp925 == Cyc_Absyn_Unique  || _tmp925 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp927=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp91B)->f1;_LL8: {
# 5177
struct Cyc_Absyn_Kind*_tmp91C=Cyc_Tcutil_tvar_kind(_tmp927,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp91D=_tmp91C;enum Cyc_Absyn_KindQual _tmp924;enum Cyc_Absyn_AliasQual _tmp923;_LLC: _tmp924=_tmp91D->kind;_tmp923=_tmp91D->aliasqual;_LLD:;
if(_tmp924 == Cyc_Absyn_RgnKind  && (_tmp923 == Cyc_Absyn_Unique  || _tmp923 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp91E=Cyc_Absyn_compress_kb(_tmp927->kind);void*_tmp91F=_tmp91E;struct Cyc_Core_Opt**_tmp922;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91F)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91F)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91F)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp922=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91F)->f1;_LL10:
# 5181
({struct Cyc_Core_Opt*_tmpE4F=({struct Cyc_Core_Opt*_tmp921=_cycalloc(sizeof(*_tmp921));({void*_tmpE4E=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp920=_cycalloc(sizeof(*_tmp920));_tmp920->tag=2U,_tmp920->f1=0,_tmp920->f2=& Cyc_Tcutil_rk;_tmp920;});_tmp921->v=_tmpE4E;});_tmp921;});*_tmp922=_tmpE4F;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5186
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5193
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp928=Cyc_Tcutil_compress(t);void*_tmp929=_tmp928;struct Cyc_Absyn_Tvar*_tmp937;void*_tmp936;switch(*((int*)_tmp929)){case 5U: _LL1: _tmp936=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp929)->f1).ptr_atts).rgn;_LL2:
# 5196
 return Cyc_Tcutil_is_noalias_region(_tmp936,must_be_unique);case 2U: _LL3: _tmp937=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp929)->f1;_LL4: {
# 5198
struct Cyc_Absyn_Kind*_tmp92A=Cyc_Tcutil_tvar_kind(_tmp937,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp92B=_tmp92A;enum Cyc_Absyn_KindQual _tmp935;enum Cyc_Absyn_AliasQual _tmp934;_LL8: _tmp935=_tmp92B->kind;_tmp934=_tmp92B->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp92C=_tmp935;switch(_tmp92C){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if(_tmp934 == Cyc_Absyn_Unique  || _tmp934 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp92D=Cyc_Absyn_compress_kb(_tmp937->kind);void*_tmp92E=_tmp92D;struct Cyc_Core_Opt**_tmp933;enum Cyc_Absyn_KindQual _tmp932;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92E)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92E)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp933=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92E)->f1;_tmp932=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp92E)->f2)->kind;_LL15:
# 5204
({struct Cyc_Core_Opt*_tmpE52=({struct Cyc_Core_Opt*_tmp931=_cycalloc(sizeof(*_tmp931));({void*_tmpE51=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp930=_cycalloc(sizeof(*_tmp930));_tmp930->tag=2U,_tmp930->f1=0,({struct Cyc_Absyn_Kind*_tmpE50=({struct Cyc_Absyn_Kind*_tmp92F=_cycalloc(sizeof(*_tmp92F));_tmp92F->kind=_tmp932,_tmp92F->aliasqual=Cyc_Absyn_Aliasable;_tmp92F;});_tmp930->f2=_tmpE50;});_tmp930;});_tmp931->v=_tmpE51;});_tmp931;});*_tmp933=_tmpE52;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5208
 return 1;}_LL13:;}
# 5211
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5214
 return 0;}_LL0:;}
# 5217
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp938=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp938,0))return 1;{
void*_tmp939=_tmp938;union Cyc_Absyn_DatatypeFieldInfoU _tmp94D;struct Cyc_List_List*_tmp94C;union Cyc_Absyn_DatatypeInfoU _tmp94B;struct Cyc_List_List*_tmp94A;struct Cyc_List_List*_tmp949;struct Cyc_Absyn_Aggrdecl**_tmp948;struct Cyc_List_List*_tmp947;struct Cyc_List_List*_tmp946;switch(*((int*)_tmp939)){case 10U: _LL1: _tmp946=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp939)->f1;_LL2:
# 5222
 while(_tmp946 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp946->hd)).f2))return 1;
_tmp946=_tmp946->tl;}
# 5226
return 0;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp939)->f1).aggr_info).KnownAggr).tag == 2){_LL3: _tmp948=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp939)->f1).aggr_info).KnownAggr).val;_tmp947=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp939)->f1).targs;_LL4:
# 5228
 if((*_tmp948)->impl == 0)return 0;else{
# 5230
struct Cyc_List_List*_tmp93A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp948)->tvs,_tmp947);
struct Cyc_List_List*_tmp93B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp948)->impl))->fields;
void*t;
while(_tmp93B != 0){
({void*_tmpE53=_tmp93A == 0?((struct Cyc_Absyn_Aggrfield*)_tmp93B->hd)->type: Cyc_Tcutil_substitute(_tmp93A,((struct Cyc_Absyn_Aggrfield*)_tmp93B->hd)->type);t=_tmpE53;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp93B=_tmp93B->tl;}
# 5238
return 0;}}else{_LL7: _LL8:
# 5248
 return 0;}case 12U: _LL5: _tmp949=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp939)->f2;_LL6:
# 5241
 while(_tmp949 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp949->hd)->type))return 1;
_tmp949=_tmp949->tl;}
# 5245
return 0;case 3U: _LL9: _tmp94B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp939)->f1).datatype_info;_tmp94A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp939)->f1).targs;_LLA: {
# 5250
union Cyc_Absyn_DatatypeInfoU _tmp93C=_tmp94B;struct Cyc_List_List*_tmp940;struct Cyc_Core_Opt*_tmp93F;struct _tuple2*_tmp93E;int _tmp93D;if((_tmp93C.UnknownDatatype).tag == 1){_LL10: _tmp93E=((_tmp93C.UnknownDatatype).val).name;_tmp93D=((_tmp93C.UnknownDatatype).val).is_extensible;_LL11:
# 5253
 return 0;}else{_LL12: _tmp940=(*(_tmp93C.KnownDatatype).val)->tvs;_tmp93F=(*(_tmp93C.KnownDatatype).val)->fields;_LL13:
# 5256
 return 0;}_LLF:;}case 4U: _LLB: _tmp94D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp939)->f1).field_info;_tmp94C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp939)->f1).targs;_LLC: {
# 5259
union Cyc_Absyn_DatatypeFieldInfoU _tmp941=_tmp94D;struct Cyc_Absyn_Datatypedecl*_tmp945;struct Cyc_Absyn_Datatypefield*_tmp944;if((_tmp941.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5262
 return 0;}else{_LL17: _tmp945=((_tmp941.KnownDatatypefield).val).f1;_tmp944=((_tmp941.KnownDatatypefield).val).f2;_LL18: {
# 5264
struct Cyc_List_List*_tmp942=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp945->tvs,_tmp94C);
struct Cyc_List_List*_tmp943=_tmp944->typs;
while(_tmp943 != 0){
({void*_tmpE54=_tmp942 == 0?(*((struct _tuple12*)_tmp943->hd)).f2: Cyc_Tcutil_substitute(_tmp942,(*((struct _tuple12*)_tmp943->hd)).f2);_tmp938=_tmpE54;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp938))return 1;
_tmp943=_tmp943->tl;}
# 5271
return 0;}}_LL14:;}default: _LLD: _LLE:
# 5273
 return 0;}_LL0:;};}
# 5280
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp94E=e->r;void*_tmp94F=_tmp94E;struct Cyc_Absyn_Stmt*_tmp965;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp963;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*_tmp95E;struct _dyneither_ptr*_tmp95D;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*_tmp95B;switch(*((int*)_tmp94F)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp94F)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp95B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp94F)->f1;_LL4:
 _tmp95C=_tmp95B;goto _LL6;case 20U: _LL5: _tmp95C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp94F)->f1;_LL6:
# 5285
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp95C->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp95C);case 21U: _LL7: _tmp95E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp94F)->f1;_tmp95D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp94F)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp95E);case 23U: _LL9: _tmp960=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp94F)->f1;_tmp95F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp94F)->f2;_LLA: {
# 5288
void*_tmp950=Cyc_Tcutil_compress((void*)_check_null(_tmp960->topt));void*_tmp951=_tmp950;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp951)->tag == 10U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp960);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp962=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp94F)->f2;_tmp961=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp94F)->f3;_LLC:
# 5293
 return Cyc_Tcutil_is_noalias_path(_tmp962) && Cyc_Tcutil_is_noalias_path(_tmp961);case 9U: _LLD: _tmp963=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp94F)->f2;_LLE:
 _tmp964=_tmp963;goto _LL10;case 14U: _LLF: _tmp964=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp94F)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp964);case 37U: _LL11: _tmp965=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp94F)->f1;_LL12:
# 5297
 while(1){
void*_tmp952=_tmp965->r;void*_tmp953=_tmp952;struct Cyc_Absyn_Exp*_tmp95A;struct Cyc_Absyn_Decl*_tmp959;struct Cyc_Absyn_Stmt*_tmp958;struct Cyc_Absyn_Stmt*_tmp957;struct Cyc_Absyn_Stmt*_tmp956;switch(*((int*)_tmp953)){case 2U: _LL1B: _tmp957=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp953)->f1;_tmp956=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp953)->f2;_LL1C:
 _tmp965=_tmp956;goto _LL1A;case 12U: _LL1D: _tmp959=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp953)->f1;_tmp958=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp953)->f2;_LL1E:
 _tmp965=_tmp958;goto _LL1A;case 1U: _LL1F: _tmp95A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp953)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp95A);default: _LL21: _LL22:
({void*_tmp954=0U;({struct _dyneither_ptr _tmpE55=({const char*_tmp955="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp955,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE55,_tag_dyneither(_tmp954,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5305
 return 1;}_LL0:;}
# 5322 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5324
struct _tuple18 bogus_ans=({struct _tuple18 _tmpAC6;_tmpAC6.f1=0,_tmpAC6.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAC6;});
void*_tmp966=e->r;void*_tmp967=_tmp966;struct Cyc_Absyn_Exp*_tmp9A0;struct Cyc_Absyn_Exp*_tmp99F;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_Absyn_Exp*_tmp99D;struct _dyneither_ptr*_tmp99C;int _tmp99B;struct Cyc_Absyn_Exp*_tmp99A;struct _dyneither_ptr*_tmp999;int _tmp998;void*_tmp997;switch(*((int*)_tmp967)){case 1U: _LL1: _tmp997=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp967)->f1;_LL2: {
# 5328
void*_tmp968=_tmp997;struct Cyc_Absyn_Vardecl*_tmp970;struct Cyc_Absyn_Vardecl*_tmp96F;struct Cyc_Absyn_Vardecl*_tmp96E;struct Cyc_Absyn_Vardecl*_tmp96D;switch(*((int*)_tmp968)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp96D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp968)->f1;_LL13: {
# 5332
void*_tmp969=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp96A=_tmp969;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp96A)->tag == 8U){_LL1B: _LL1C:
# 5334
 return({struct _tuple18 _tmpABA;_tmpABA.f1=1,_tmpABA.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpABA;});}else{_LL1D: _LL1E:
 return({struct _tuple18 _tmpABB;_tmpABB.f1=(_tmp96D->tq).real_const,_tmpABB.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpABB;});}_LL1A:;}case 4U: _LL14: _tmp96E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp968)->f1;_LL15: {
# 5338
void*_tmp96B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp96C=_tmp96B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp96C)->tag == 8U){_LL20: _LL21:
 return({struct _tuple18 _tmpABC;_tmpABC.f1=1,_tmpABC.f2=(void*)_check_null(_tmp96E->rgn);_tmpABC;});}else{_LL22: _LL23:
# 5341
 _tmp96E->escapes=1;
return({struct _tuple18 _tmpABD;_tmpABD.f1=(_tmp96E->tq).real_const,_tmpABD.f2=(void*)_check_null(_tmp96E->rgn);_tmpABD;});}_LL1F:;}case 5U: _LL16: _tmp96F=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp968)->f1;_LL17:
# 5344
 _tmp970=_tmp96F;goto _LL19;default: _LL18: _tmp970=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp968)->f1;_LL19:
# 5346
 _tmp970->escapes=1;
return({struct _tuple18 _tmpABE;_tmpABE.f1=(_tmp970->tq).real_const,_tmpABE.f2=(void*)_check_null(_tmp970->rgn);_tmpABE;});}_LLD:;}case 21U: _LL3: _tmp99A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp967)->f1;_tmp999=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp967)->f2;_tmp998=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp967)->f3;_LL4:
# 5351
 if(_tmp998)return bogus_ans;{
# 5354
void*_tmp971=Cyc_Tcutil_compress((void*)_check_null(_tmp99A->topt));void*_tmp972=_tmp971;struct Cyc_Absyn_Aggrdecl*_tmp97E;struct Cyc_List_List*_tmp97D;switch(*((int*)_tmp972)){case 12U: _LL25: _tmp97D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp972)->f2;_LL26: {
# 5356
struct Cyc_Absyn_Aggrfield*_tmp973=Cyc_Absyn_lookup_field(_tmp97D,_tmp999);
if(_tmp973 != 0  && _tmp973->width == 0){
struct _tuple18 _tmp974=Cyc_Tcutil_addressof_props(te,_tmp99A);struct _tuple18 _tmp975=_tmp974;int _tmp977;void*_tmp976;_LL2C: _tmp977=_tmp975.f1;_tmp976=_tmp975.f2;_LL2D:;
return({struct _tuple18 _tmpABF;_tmpABF.f1=(_tmp973->tq).real_const  || _tmp977,_tmpABF.f2=_tmp976;_tmpABF;});}
# 5361
return bogus_ans;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp972)->f1).aggr_info).KnownAggr).tag == 2){_LL27: _tmp97E=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp972)->f1).aggr_info).KnownAggr).val;_LL28: {
# 5363
struct Cyc_Absyn_Aggrfield*_tmp978=Cyc_Absyn_lookup_decl_field(_tmp97E,_tmp999);
if(_tmp978 != 0  && _tmp978->width == 0){
struct _tuple18 _tmp979=Cyc_Tcutil_addressof_props(te,_tmp99A);struct _tuple18 _tmp97A=_tmp979;int _tmp97C;void*_tmp97B;_LL2F: _tmp97C=_tmp97A.f1;_tmp97B=_tmp97A.f2;_LL30:;
return({struct _tuple18 _tmpAC0;_tmpAC0.f1=(_tmp978->tq).real_const  || _tmp97C,_tmpAC0.f2=_tmp97B;_tmpAC0;});}
# 5368
return bogus_ans;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp99D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp967)->f1;_tmp99C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp967)->f2;_tmp99B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp967)->f3;_LL6:
# 5373
 if(_tmp99B)return bogus_ans;{
# 5376
void*_tmp97F=Cyc_Tcutil_compress((void*)_check_null(_tmp99D->topt));void*_tmp980=_tmp97F;void*_tmp986;void*_tmp985;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp980)->tag == 5U){_LL32: _tmp986=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp980)->f1).elt_typ;_tmp985=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp980)->f1).ptr_atts).rgn;_LL33: {
# 5378
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp981=Cyc_Tcutil_compress(_tmp986);void*_tmp982=_tmp981;struct Cyc_Absyn_Aggrdecl*_tmp984;struct Cyc_List_List*_tmp983;switch(*((int*)_tmp982)){case 12U: _LL37: _tmp983=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp982)->f2;_LL38:
# 5381
({struct Cyc_Absyn_Aggrfield*_tmpE56=Cyc_Absyn_lookup_field(_tmp983,_tmp99C);finfo=_tmpE56;});goto _LL36;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp982)->f1).aggr_info).KnownAggr).tag == 2){_LL39: _tmp984=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp982)->f1).aggr_info).KnownAggr).val;_LL3A:
# 5383
({struct Cyc_Absyn_Aggrfield*_tmpE57=Cyc_Absyn_lookup_decl_field(_tmp984,_tmp99C);finfo=_tmpE57;});goto _LL36;}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5386
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpAC1;_tmpAC1.f1=(finfo->tq).real_const,_tmpAC1.f2=_tmp985;_tmpAC1;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp99E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp967)->f1;_LL8: {
# 5393
void*_tmp987=Cyc_Tcutil_compress((void*)_check_null(_tmp99E->topt));void*_tmp988=_tmp987;struct Cyc_Absyn_Tqual _tmp98A;void*_tmp989;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp988)->tag == 5U){_LL3E: _tmp98A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp988)->f1).elt_tq;_tmp989=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp988)->f1).ptr_atts).rgn;_LL3F:
# 5395
 return({struct _tuple18 _tmpAC2;_tmpAC2.f1=_tmp98A.real_const,_tmpAC2.f2=_tmp989;_tmpAC2;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp9A0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp967)->f1;_tmp99F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp967)->f2;_LLA: {
# 5401
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp9A0->topt));
void*_tmp98B=t;struct Cyc_Absyn_Tqual _tmp994;struct Cyc_Absyn_Tqual _tmp993;void*_tmp992;struct Cyc_List_List*_tmp991;switch(*((int*)_tmp98B)){case 10U: _LL43: _tmp991=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp98B)->f1;_LL44: {
# 5405
struct _tuple14 _tmp98C=Cyc_Evexp_eval_const_uint_exp(_tmp99F);struct _tuple14 _tmp98D=_tmp98C;unsigned int _tmp990;int _tmp98F;_LL4C: _tmp990=_tmp98D.f1;_tmp98F=_tmp98D.f2;_LL4D:;
if(!_tmp98F)
return bogus_ans;{
struct _tuple12*_tmp98E=Cyc_Absyn_lookup_tuple_field(_tmp991,(int)_tmp990);
if(_tmp98E != 0)
return({struct _tuple18 _tmpAC3;_tmpAC3.f1=((*_tmp98E).f1).real_const,({void*_tmpE58=(Cyc_Tcutil_addressof_props(te,_tmp9A0)).f2;_tmpAC3.f2=_tmpE58;});_tmpAC3;});
return bogus_ans;};}case 5U: _LL45: _tmp993=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98B)->f1).elt_tq;_tmp992=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98B)->f1).ptr_atts).rgn;_LL46:
# 5413
 return({struct _tuple18 _tmpAC4;_tmpAC4.f1=_tmp993.real_const,_tmpAC4.f2=_tmp992;_tmpAC4;});case 8U: _LL47: _tmp994=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp98B)->f1).tq;_LL48:
# 5419
 return({struct _tuple18 _tmpAC5;_tmpAC5.f1=_tmp994.real_const,({void*_tmpE59=(Cyc_Tcutil_addressof_props(te,_tmp9A0)).f2;_tmpAC5.f2=_tmpE59;});_tmpAC5;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5423
({void*_tmp995=0U;({unsigned int _tmpE5B=e->loc;struct _dyneither_ptr _tmpE5A=({const char*_tmp996="unary & applied to non-lvalue";_tag_dyneither(_tmp996,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpE5B,_tmpE5A,_tag_dyneither(_tmp995,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5430
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
({union Cyc_Absyn_Constraint*_tmpE5C=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);b=_tmpE5C;});{
void*_tmp9A1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmp9A2=_tmp9A1;struct Cyc_Absyn_Exp*_tmp9AB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9A2)->tag == 0U){_LL1: _LL2:
 return;}else{_LL3: _tmp9AB=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9A2)->f1;_LL4: {
# 5435
struct _tuple14 _tmp9A3=Cyc_Evexp_eval_const_uint_exp(_tmp9AB);struct _tuple14 _tmp9A4=_tmp9A3;unsigned int _tmp9AA;int _tmp9A9;_LL6: _tmp9AA=_tmp9A4.f1;_tmp9A9=_tmp9A4.f2;_LL7:;
if(_tmp9A9  && _tmp9AA <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmp9A7=({struct Cyc_Int_pa_PrintArg_struct _tmpAC8;_tmpAC8.tag=1U,_tmpAC8.f1=(unsigned long)((int)_tmp9AA);_tmpAC8;});struct Cyc_Int_pa_PrintArg_struct _tmp9A8=({struct Cyc_Int_pa_PrintArg_struct _tmpAC7;_tmpAC7.tag=1U,_tmpAC7.f1=(unsigned long)((int)i);_tmpAC7;});void*_tmp9A5[2U];_tmp9A5[0]=& _tmp9A7,_tmp9A5[1]=& _tmp9A8;({unsigned int _tmpE5E=loc;struct _dyneither_ptr _tmpE5D=({const char*_tmp9A6="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp9A6,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE5E,_tmpE5D,_tag_dyneither(_tmp9A5,sizeof(void*),2U));});});
return;}}_LL0:;};}
# 5442
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0U,b);}
# 5446
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmp9AC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmp9AD=_tmp9AC;struct Cyc_Absyn_Exp*_tmp9B2;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9AD)->tag == 1U){_LL1: _tmp9B2=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9AD)->f1;_LL2: {
# 5449
struct _tuple14 _tmp9AE=Cyc_Evexp_eval_const_uint_exp(_tmp9B2);struct _tuple14 _tmp9AF=_tmp9AE;unsigned int _tmp9B1;int _tmp9B0;_LL6: _tmp9B1=_tmp9AF.f1;_tmp9B0=_tmp9AF.f2;_LL7:;
return _tmp9B0  && _tmp9B1 == 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5455
int Cyc_Tcutil_bits_only(void*t){
void*_tmp9B3=Cyc_Tcutil_compress(t);void*_tmp9B4=_tmp9B3;struct Cyc_List_List*_tmp9BE;struct Cyc_Absyn_Aggrdecl*_tmp9BD;struct Cyc_List_List*_tmp9BC;struct Cyc_List_List*_tmp9BB;void*_tmp9BA;union Cyc_Absyn_Constraint*_tmp9B9;switch(*((int*)_tmp9B4)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 return 0;case 8U: _LLB: _tmp9BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B4)->f1).elt_type;_tmp9B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B4)->f1).zero_term;_LLC:
# 5465
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9B9) && Cyc_Tcutil_bits_only(_tmp9BA);case 10U: _LLD: _tmp9BB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9B4)->f1;_LLE:
# 5467
 for(0;_tmp9BB != 0;_tmp9BB=_tmp9BB->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmp9BB->hd)).f2))return 0;}
return 1;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9B4)->f1).aggr_info).UnknownAggr).tag == 1){_LLF: _LL10:
 return 0;}else{_LL11: _tmp9BD=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9B4)->f1).aggr_info).KnownAggr).val;_tmp9BC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9B4)->f1).targs;_LL12:
# 5472
 if(_tmp9BD->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9BD->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp9B5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9B5;_push_region(rgn);
{struct Cyc_List_List*_tmp9B6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp9BD->tvs,_tmp9BC);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9BD->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp9B6,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp9B7=0;_npop_handler(0U);return _tmp9B7;}}}{
int _tmp9B8=1;_npop_handler(0U);return _tmp9B8;};}
# 5479
;_pop_region(rgn);};};}case 12U: _LL13: _tmp9BE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9B4)->f2;_LL14:
# 5484
 for(0;_tmp9BE != 0;_tmp9BE=_tmp9BE->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmp9BE->hd)->type))return 0;}
return 1;default: _LL15: _LL16:
 return 0;}_LL0:;}
# 5492
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp9BF=e->r;void*_tmp9C0=_tmp9BF;struct Cyc_Absyn_Exp*_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C1;switch(*((int*)_tmp9C0)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp9C1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9C0)->f1;_LL4:
 _tmp9C2=_tmp9C1;goto _LL6;case 13U: _LL5: _tmp9C2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9C0)->f1;_LL6:
 e=_tmp9C2;continue;default: _LL7: _LL8:
# 5499
 return 0;}_LL0:;}}
# 5509
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp9C3=e->r;void*_tmp9C4=_tmp9C3;struct Cyc_List_List*_tmp9E2;struct Cyc_List_List*_tmp9E1;enum Cyc_Absyn_Primop _tmp9E0;struct Cyc_List_List*_tmp9DF;struct Cyc_List_List*_tmp9DE;struct Cyc_List_List*_tmp9DD;struct Cyc_List_List*_tmp9DC;struct Cyc_Absyn_Exp*_tmp9DB;struct Cyc_Absyn_Exp*_tmp9DA;struct Cyc_Absyn_Exp*_tmp9D9;struct Cyc_Absyn_Exp*_tmp9D8;void*_tmp9D7;struct Cyc_Absyn_Exp*_tmp9D6;void*_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D4;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D2;struct Cyc_Absyn_Exp*_tmp9D1;struct Cyc_Absyn_Exp*_tmp9D0;struct Cyc_Absyn_Exp*_tmp9CF;struct Cyc_Absyn_Exp*_tmp9CE;struct Cyc_Absyn_Exp*_tmp9CD;void*_tmp9CC;switch(*((int*)_tmp9C4)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp9CC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL10: {
# 5521
void*_tmp9C5=_tmp9CC;struct Cyc_Absyn_Vardecl*_tmp9CB;struct Cyc_Absyn_Vardecl*_tmp9CA;switch(*((int*)_tmp9C5)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmp9CA=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp9C5)->f1;_LL35: {
# 5524
void*_tmp9C6=Cyc_Tcutil_compress(_tmp9CA->type);void*_tmp9C7=_tmp9C6;switch(*((int*)_tmp9C7)){case 8U: _LL3D: _LL3E:
 goto _LL40;case 9U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmp9CB=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp9C5)->f1;_LL37:
# 5531
 if(_tmp9CB->sc == Cyc_Absyn_Static){
void*_tmp9C8=Cyc_Tcutil_compress(_tmp9CB->type);void*_tmp9C9=_tmp9C8;switch(*((int*)_tmp9C9)){case 8U: _LL44: _LL45:
 goto _LL47;case 9U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}else{
# 5538
return var_okay;}case 0U: _LL38: _LL39:
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmp9CF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_tmp9CE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9C4)->f2;_tmp9CD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9C4)->f3;_LL12:
# 5544
 return(Cyc_Tcutil_cnst_exp(0,_tmp9CF) && 
Cyc_Tcutil_cnst_exp(0,_tmp9CE)) && 
Cyc_Tcutil_cnst_exp(0,_tmp9CD);case 9U: _LL13: _tmp9D1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_tmp9D0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9C4)->f2;_LL14:
# 5548
 return Cyc_Tcutil_cnst_exp(0,_tmp9D1) && Cyc_Tcutil_cnst_exp(0,_tmp9D0);case 12U: _LL15: _tmp9D2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL16:
 _tmp9D3=_tmp9D2;goto _LL18;case 13U: _LL17: _tmp9D3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL18:
# 5551
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9D3);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C4)->f4 == Cyc_Absyn_No_coercion){_LL19: _tmp9D5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_tmp9D4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C4)->f2;_LL1A:
# 5553
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9D4);}else{_LL1B: _tmp9D7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_tmp9D6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9C4)->f2;_LL1C:
# 5556
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9D6);}case 15U: _LL1D: _tmp9D8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL1E:
# 5558
 return Cyc_Tcutil_cnst_exp(1,_tmp9D8);case 27U: _LL1F: _tmp9DA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9C4)->f2;_tmp9D9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9C4)->f3;_LL20:
# 5560
 return Cyc_Tcutil_cnst_exp(0,_tmp9DA) && Cyc_Tcutil_cnst_exp(0,_tmp9D9);case 28U: _LL21: _tmp9DB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL22:
# 5562
 return Cyc_Tcutil_cnst_exp(0,_tmp9DB);case 26U: _LL23: _tmp9DC=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL24:
 _tmp9DD=_tmp9DC;goto _LL26;case 30U: _LL25: _tmp9DD=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9C4)->f2;_LL26:
 _tmp9DE=_tmp9DD;goto _LL28;case 29U: _LL27: _tmp9DE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9C4)->f3;_LL28:
# 5566
 for(0;_tmp9DE != 0;_tmp9DE=_tmp9DE->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmp9DE->hd)).f2))
return 0;}
return 1;case 3U: _LL29: _tmp9E0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_tmp9DF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9C4)->f2;_LL2A:
# 5571
 _tmp9E1=_tmp9DF;goto _LL2C;case 24U: _LL2B: _tmp9E1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL2C:
 _tmp9E2=_tmp9E1;goto _LL2E;case 31U: _LL2D: _tmp9E2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL2E:
# 5574
 for(0;_tmp9E2 != 0;_tmp9E2=_tmp9E2->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp9E2->hd))
return 0;}
return 1;default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 5582
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5586
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5588
int Cyc_Tcutil_supports_default(void*t){
void*_tmp9E3=Cyc_Tcutil_compress(t);void*_tmp9E4=_tmp9E3;struct Cyc_List_List*_tmp9EE;union Cyc_Absyn_AggrInfoU _tmp9ED;struct Cyc_List_List*_tmp9EC;struct Cyc_List_List*_tmp9EB;void*_tmp9EA;union Cyc_Absyn_Constraint*_tmp9E9;union Cyc_Absyn_Constraint*_tmp9E8;switch(*((int*)_tmp9E4)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 5U: _LL7: _tmp9E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E4)->f1).ptr_atts).nullable;_tmp9E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E4)->f1).ptr_atts).bounds;_LL8: {
# 5595
void*_tmp9E5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9E8);void*_tmp9E6=_tmp9E5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E6)->tag == 0U){_LL18: _LL19:
 return 1;}else{_LL1A: _LL1B:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp9E9);}_LL17:;}case 8U: _LL9: _tmp9EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9E4)->f1).elt_type;_LLA:
# 5600
 return Cyc_Tcutil_supports_default(_tmp9EA);case 10U: _LLB: _tmp9EB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9E4)->f1;_LLC:
# 5602
 for(0;_tmp9EB != 0;_tmp9EB=_tmp9EB->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp9EB->hd)).f2))return 0;}
return 1;case 11U: _LLD: _tmp9ED=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E4)->f1).aggr_info;_tmp9EC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E4)->f1).targs;_LLE: {
# 5606
struct Cyc_Absyn_Aggrdecl*_tmp9E7=Cyc_Absyn_get_known_aggrdecl(_tmp9ED);
if(_tmp9E7->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9E7->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmp9E7->tvs,_tmp9EC,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9E7->impl))->fields);}case 12U: _LLF: _tmp9EE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9E4)->f2;_LL10:
 return Cyc_Tcutil_fields_support_default(0,0,_tmp9EE);case 14U: _LL11: _LL12:
# 5612
 goto _LL14;case 13U: _LL13: _LL14:
 return 1;default: _LL15: _LL16:
# 5615
 return 0;}_LL0:;}
# 5620
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp9EF=t;struct Cyc_Absyn_Typedefdecl*_tmp9F4;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9EF)->tag == 17U){_LL1: _tmp9F4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9EF)->f3;_LL2:
# 5623
 if(_tmp9F4 != 0){
struct Cyc_Absyn_Tqual _tmp9F0=_tmp9F4->tq;
if(((_tmp9F0.print_const  || _tmp9F0.q_volatile) || _tmp9F0.q_restrict) || _tmp9F0.real_const)
# 5628
({struct Cyc_String_pa_PrintArg_struct _tmp9F3=({struct Cyc_String_pa_PrintArg_struct _tmpAC9;_tmpAC9.tag=0U,({struct _dyneither_ptr _tmpE5F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAC9.f1=_tmpE5F;});_tmpAC9;});void*_tmp9F1[1U];_tmp9F1[0]=& _tmp9F3;({unsigned int _tmpE61=loc;struct _dyneither_ptr _tmpE60=({const char*_tmp9F2="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp9F2,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpE61,_tmpE60,_tag_dyneither(_tmp9F1,sizeof(void*),1U));});});}
# 5631
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5636
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5638
struct _RegionHandle _tmp9F5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9F5;_push_region(rgn);
{struct Cyc_List_List*_tmp9F6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmp9F7=1;_npop_handler(0U);return _tmp9F7;}
({void*_tmpE62=Cyc_Tcutil_rsubstitute(rgn,_tmp9F6,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpE62;});
if(!Cyc_Tcutil_supports_default(t)){int _tmp9F8=0;_npop_handler(0U);return _tmp9F8;}}{
# 5646
int _tmp9F9=1;_npop_handler(0U);return _tmp9F9;};}
# 5639
;_pop_region(rgn);}
# 5652
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp9FA=Cyc_Tcutil_compress(t);void*_tmp9FB=_tmp9FA;union Cyc_Absyn_Constraint*_tmp9FF;union Cyc_Absyn_Constraint*_tmp9FE;switch(*((int*)_tmp9FB)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;case 5U: _LL5: _tmp9FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9FB)->f1).ptr_atts).nullable;_tmp9FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9FB)->f1).ptr_atts).bounds;_LL6: {
# 5657
void*_tmp9FC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9FE);void*_tmp9FD=_tmp9FC;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9FD)->tag == 0U){_LLA: _LLB:
# 5661
 return 0;}else{_LLC: _LLD:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9FF);}_LL9:;}default: _LL7: _LL8:
# 5664
 return 0;}_LL0:;}
# 5668
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpA00=Cyc_Tcutil_compress(t);void*_tmpA01=_tmpA00;struct Cyc_List_List*_tmpA05;void*_tmpA04;switch(*((int*)_tmpA01)){case 5U: _LL1: _tmpA04=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA01)->f1).elt_typ;_LL2:
 return Cyc_Tcutil_is_noreturn(_tmpA04);case 9U: _LL3: _tmpA05=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA01)->f1).attributes;_LL4:
# 5672
 for(0;_tmpA05 != 0;_tmpA05=_tmpA05->tl){
void*_tmpA02=(void*)_tmpA05->hd;void*_tmpA03=_tmpA02;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpA03)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 5677
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5680
return 0;}
# 5685
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpA06=Cyc_Tcutil_compress(t);void*_tmpA07=_tmpA06;struct Cyc_List_List**_tmpA0D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA07)->tag == 9U){_LL1: _tmpA0D=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA07)->f1).attributes;_LL2: {
# 5688
struct Cyc_List_List*_tmpA08=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpA0D,(void*)atts->hd))
({struct Cyc_List_List*_tmpE63=({struct Cyc_List_List*_tmpA09=_cycalloc(sizeof(*_tmpA09));_tmpA09->hd=(void*)atts->hd,_tmpA09->tl=*_tmpA0D;_tmpA09;});*_tmpA0D=_tmpE63;});}else{
# 5695
({struct Cyc_List_List*_tmpE64=({struct Cyc_List_List*_tmpA0A=_cycalloc(sizeof(*_tmpA0A));_tmpA0A->hd=(void*)atts->hd,_tmpA0A->tl=_tmpA08;_tmpA0A;});_tmpA08=_tmpE64;});}}
return _tmpA08;}}else{_LL3: _LL4:
({void*_tmpA0B=0U;({struct _dyneither_ptr _tmpE65=({const char*_tmpA0C="transfer_fn_type_atts";_tag_dyneither(_tmpA0C,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE65,_tag_dyneither(_tmpA0B,sizeof(void*),0U));});});}_LL0:;}
# 5702
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpA0E=Cyc_Tcutil_compress(t);void*_tmpA0F=_tmpA0E;struct Cyc_Absyn_Exp*_tmpA14;struct Cyc_Absyn_PtrInfo*_tmpA13;switch(*((int*)_tmpA0F)){case 5U: _LL1: _tmpA13=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0F)->f1;_LL2:
# 5706
{void*_tmpA10=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpA13->ptr_atts).bounds);void*_tmpA11=_tmpA10;struct Cyc_Absyn_Exp*_tmpA12;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA11)->tag == 1U){_LL8: _tmpA12=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA11)->f1;_LL9:
 bound=_tmpA12;goto _LL7;}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 5710
goto _LL0;case 8U: _LL3: _tmpA14=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0F)->f1).num_elts;_LL4:
# 5712
 bound=_tmpA14;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5716
return bound;}
# 5721
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpA15=b;struct Cyc_Absyn_Vardecl*_tmpA19;struct Cyc_Absyn_Vardecl*_tmpA18;struct Cyc_Absyn_Vardecl*_tmpA17;struct Cyc_Absyn_Vardecl*_tmpA16;switch(*((int*)_tmpA15)){case 5U: _LL1: _tmpA16=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA15)->f1;_LL2:
 _tmpA17=_tmpA16;goto _LL4;case 4U: _LL3: _tmpA17=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA15)->f1;_LL4:
 _tmpA18=_tmpA17;goto _LL6;case 3U: _LL5: _tmpA18=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA15)->f1;_LL6:
 _tmpA19=_tmpA18;goto _LL8;case 1U: _LL7: _tmpA19=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA15)->f1;_LL8:
# 5727
 if(!_tmpA19->escapes)return _tmpA19;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5731
return 0;}
# 5735
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpA1A=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpE66=({struct Cyc_List_List*_tmpA1B=_cycalloc(sizeof(*_tmpA1B));_tmpA1B->hd=*((void**)_check_null((void**)x->hd)),_tmpA1B->tl=_tmpA1A;_tmpA1B;});_tmpA1A=_tmpE66;});}}
return _tmpA1A;}
# 5742
int Cyc_Tcutil_is_array(void*t){
void*_tmpA1C=Cyc_Tcutil_compress(t);void*_tmpA1D=_tmpA1C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1D)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5749
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpA1E=Cyc_Tcutil_compress(t);void*_tmpA1F=_tmpA1E;void*_tmpA2C;struct Cyc_Absyn_Tqual _tmpA2B;struct Cyc_Absyn_Exp*_tmpA2A;union Cyc_Absyn_Constraint*_tmpA29;unsigned int _tmpA28;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1F)->tag == 8U){_LL1: _tmpA2C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1F)->f1).elt_type;_tmpA2B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1F)->f1).tq;_tmpA2A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1F)->f1).num_elts;_tmpA29=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1F)->f1).zero_term;_tmpA28=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1F)->f1).zt_loc;_LL2: {
# 5752
void*b;
if(_tmpA2A == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5756
struct Cyc_Absyn_Exp*bnd=_tmpA2A;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpA20=0U;({struct _dyneither_ptr _tmpE67=({const char*_tmpA21="cannot convert tag without type!";_tag_dyneither(_tmpA21,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE67,_tag_dyneither(_tmpA20,sizeof(void*),0U));});});{
void*_tmpA22=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpA23=_tmpA22;void*_tmpA26;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA23)->tag == 19U){_LL6: _tmpA26=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA23)->f1;_LL7:
# 5762
({void*_tmpE6A=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA24=_cycalloc(sizeof(*_tmpA24));_tmpA24->tag=1U,({struct Cyc_Absyn_Exp*_tmpE69=({void*_tmpE68=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmpE68,Cyc_Absyn_valueof_exp(_tmpA26,0U),0,Cyc_Absyn_No_coercion,0U);});_tmpA24->f1=_tmpE69;});_tmpA24;});b=_tmpE6A;});
goto _LL5;}else{_LL8: _LL9:
# 5765
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpE6B=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA25=_cycalloc(sizeof(*_tmpA25));_tmpA25->tag=1U,_tmpA25->f1=bnd;_tmpA25;});b=_tmpE6B;});else{
# 5768
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL5:;};}else{
# 5772
({void*_tmpE6C=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA27=_cycalloc(sizeof(*_tmpA27));_tmpA27->tag=1U,_tmpA27->f1=bnd;_tmpA27;});b=_tmpE6C;});}}
# 5774
return Cyc_Absyn_atb_typ(_tmpA2C,rgn,_tmpA2B,b,_tmpA29);}}else{_LL3: _LL4:
# 5777
 return t;}_LL0:;}
# 5782
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpA2D=Cyc_Tcutil_compress(t);void*_tmpA2E=_tmpA2D;struct Cyc_List_List*_tmpA3C;union Cyc_Absyn_AggrInfoU _tmpA3B;struct Cyc_List_List*_tmpA3A;struct Cyc_List_List*_tmpA39;void*_tmpA38;union Cyc_Absyn_Constraint*_tmpA37;switch(*((int*)_tmpA2E)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 return 0;case 5U: _LLB: _tmpA37=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA2E)->f1).ptr_atts).nullable;_LLC:
# 5790
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpA37);case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmpA38=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA2E)->f1).elt_type;_LL12:
 return Cyc_Tcutil_zeroable_type(_tmpA38);case 9U: _LL13: _LL14:
 return 0;case 10U: _LL15: _tmpA39=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA2E)->f1;_LL16:
# 5796
 for(0;(unsigned int)_tmpA39;_tmpA39=_tmpA39->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpA39->hd)).f2))return 0;}
return 1;case 11U: _LL17: _tmpA3B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA2E)->f1).aggr_info;_tmpA3A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA2E)->f1).targs;_LL18: {
# 5800
struct Cyc_Absyn_Aggrdecl*_tmpA2F=Cyc_Absyn_get_known_aggrdecl(_tmpA3B);
if(_tmpA2F->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA2F->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpA30=_new_region("r");struct _RegionHandle*r=& _tmpA30;_push_region(r);
{struct Cyc_List_List*_tmpA31=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpA2F->tvs,_tmpA3A);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA2F->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpA31,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA32=0;_npop_handler(0U);return _tmpA32;}}}{
int _tmpA33=1;_npop_handler(0U);return _tmpA33;};}
# 5804
;_pop_region(r);};}case 13U: _LL19: _LL1A:
# 5808
 goto _LL1C;case 19U: _LL1B: _LL1C:
 return 1;case 12U: _LL1D: _tmpA3C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA2E)->f2;_LL1E:
# 5811
 for(0;_tmpA3C != 0;_tmpA3C=_tmpA3C->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpA3C->hd)->type))return 0;}
return 1;case 14U: _LL1F: _LL20:
 return 1;case 17U: _LL21: _LL22:
 goto _LL24;case 16U: _LL23: _LL24:
 goto _LL26;case 15U: _LL25: _LL26:
 goto _LL28;case 28U: _LL27: _LL28:
 return 0;case 26U: _LL29: _LL2A:
# 5820
 goto _LL2C;case 27U: _LL2B: _LL2C:
 goto _LL2E;case 18U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 21U: _LL31: _LL32:
 goto _LL34;case 22U: _LL33: _LL34:
 goto _LL36;case 23U: _LL35: _LL36:
 goto _LL38;case 24U: _LL37: _LL38:
 goto _LL3A;default: _LL39: _LL3A:
({struct Cyc_String_pa_PrintArg_struct _tmpA36=({struct Cyc_String_pa_PrintArg_struct _tmpACA;_tmpACA.tag=0U,({struct _dyneither_ptr _tmpE6D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpACA.f1=_tmpE6D;});_tmpACA;});void*_tmpA34[1U];_tmpA34[0]=& _tmpA36;({struct _dyneither_ptr _tmpE6E=({const char*_tmpA35="bad type `%s' in zeroable type";_tag_dyneither(_tmpA35,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE6E,_tag_dyneither(_tmpA34,sizeof(void*),1U));});});}_LL0:;}
