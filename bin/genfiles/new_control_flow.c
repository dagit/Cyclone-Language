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
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 277 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 974 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1158
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1160
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
# 1174
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1176
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 87
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 92
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 167
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 237 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 269
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 330
int Cyc_Tcutil_is_noreturn(void*);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 63
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 67
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 69
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 99
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 102
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 107
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 111
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 120
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 153
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env();
# 155
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 161
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple15{void*f1;struct Cyc_List_List*f2;};
struct _tuple15 Cyc_CfFlowInfo_unname_rval(void*rv);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 171
int Cyc_CfFlowInfo_is_init_pointer(void*r);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 181
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 201
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple16 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple16 pr1,struct _tuple16 pr2);
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 51 "new_control_flow.cyc"
static int Cyc_NewControlFlow_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));
unsigned int minsz=sz1 < sz2?sz1: sz2;
# 59
while(i < minsz){
char c1=*((const char*)_check_dyneither_subscript(s1,sizeof(char),i));
char c2=*((const char*)_check_dyneither_subscript(s2,sizeof(char),i));
if(c1 == '\000'){
if(c2 == '\000')return 0;else{
return - 1;}}else{
if(c2 == '\000')return 1;else{
# 67
int diff=c1 - c2;
if(diff != 0)return diff;}}
# 70
++ i;}
# 72
if(sz1 == sz2)return 0;
if(minsz < sz2){
if(*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{
return - 1;}}else{
# 77
if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')return 0;else{
return 1;}}};}
# 83
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{int visited;};static char Cyc_NewControlFlow_CFAnnot[8U]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};struct Cyc_NewControlFlow_AnalEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*all_tables;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table*pat_pop_table;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;struct Cyc_List_List*return_relns;};struct _tuple17{void*f1;int f2;};
# 125 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*,struct Cyc_CfFlowInfo_Place*,void*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 135
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp0=s->annot;void*_tmp1=_tmp0;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp3;if(((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->tag == Cyc_NewControlFlow_CFAnnot){_LL1: _tmp3=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp1)->f1;_LL2:
 return _tmp3;}else{_LL3: _LL4:
({void*_tmp633=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_NewControlFlow_CFAnnot,(_tmp2->f1).visited=0;_tmp2;});s->annot=_tmp633;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp4=_cycalloc(sizeof(*_tmp4));({union Cyc_CfFlowInfo_FlowInfo _tmp634=Cyc_CfFlowInfo_BottomFL();*_tmp4=_tmp634;});_tmp4;});
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 149
return*sflow;}
# 152
static struct Cyc_List_List*Cyc_NewControlFlow_flowrelns(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp5=f;struct Cyc_List_List*_tmp6;if((_tmp5.BottomFL).tag == 1){_LL1: _LL2:
 return 0;}else{_LL3: _tmp6=((_tmp5.ReachableFL).val).f2;_LL4:
 return _tmp6;}_LL0:;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 159
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp7=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp8=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 163
({union Cyc_CfFlowInfo_FlowInfo _tmp635=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp635;});
# 169
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp594;_tmp594.f1=_tmp7,_tmp594.f2=_tmp8;_tmp594;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _tmp636=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp636;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 194
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 196
struct _tuple21*_tmp9=ckenv;struct Cyc_NewControlFlow_AnalEnv*_tmp13;unsigned int _tmp12;struct Cyc_Dict_Dict _tmp11;_LL1: _tmp13=_tmp9->f1;_tmp12=_tmp9->f2;_tmp11=_tmp9->f3;_LL2:;{
void*_tmpA=root;struct Cyc_Absyn_Vardecl*_tmp10;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpA)->tag == 0U){_LL4: _tmp10=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpA)->f1;_LL5:
# 199
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp11,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp10->type)){
retry: {void*_tmpB=rval;void*_tmpF;switch(*((int*)_tmpB)){case 8U: _LL9: _tmpF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB)->f2;_LLA:
 rval=_tmpF;goto retry;case 7U: _LLB: _LLC:
 goto _LLE;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpB)->f1 == Cyc_CfFlowInfo_NoneIL){_LLD: _LLE:
 goto _LL10;}else{goto _LL11;}case 0U: _LLF: _LL10:
 goto _LL8;default: _LL11: _LL12:
# 208
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp595;_tmp595.tag=0U,({struct _dyneither_ptr _tmp637=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp10->name));_tmp595.f1=_tmp637;});_tmp595;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned int _tmp639=_tmp12;struct _dyneither_ptr _tmp638=({const char*_tmpD="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpD,sizeof(char),64U);});Cyc_Warn_warn(_tmp639,_tmp638,_tag_dyneither(_tmpC,sizeof(void*),1U));});});
goto _LL8;}_LL8:;}}
# 212
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 220
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp14=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp15=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp16=Cyc_CfFlowInfo_join_flow(env->fenv,flow,*_tmp15);
# 226
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp17=({struct _tuple19 _tmp597;_tmp597.f1=flow,_tmp597.f2=_tmp16;_tmp597;});struct _tuple19 _tmp18=_tmp17;struct Cyc_Dict_Dict _tmp1B;struct Cyc_Dict_Dict _tmp1A;if(((_tmp18.f1).ReachableFL).tag == 2){if(((_tmp18.f2).ReachableFL).tag == 2){_LL1: _tmp1B=(((_tmp18.f1).ReachableFL).val).f1;_tmp1A=(((_tmp18.f2).ReachableFL).val).f1;_LL2: {
# 229
struct _tuple21 _tmp19=({struct _tuple21 _tmp596;_tmp596.f1=env,_tmp596.f2=s->loc,_tmp596.f3=_tmp1A;_tmp596;});
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp19,_tmp1B);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 235
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp16,*_tmp15)){
# 242
*_tmp15=_tmp16;
# 246
if(_tmp14->visited == env->iteration_num)
# 248
env->iterate_again=1;}}
# 253
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 257
union Cyc_CfFlowInfo_FlowInfo _tmp1C=inflow;struct Cyc_Dict_Dict _tmp22;struct Cyc_List_List*_tmp21;if((_tmp1C.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp22=((_tmp1C.ReachableFL).val).f1;_tmp21=((_tmp1C.ReachableFL).val).f2;_LL4:
# 260
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp1D=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=0U,_tmp20->f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp20;});
void*_tmp1E=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit)
({void*_tmp63A=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=8U,_tmp1F->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp1F->f2=_tmp1E;_tmp1F;});_tmp1E=_tmp63A;});
# 267
({struct Cyc_Dict_Dict _tmp63B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp22,(void*)_tmp1D,_tmp1E);_tmp22=_tmp63B;});}
# 269
return Cyc_CfFlowInfo_ReachableFL(_tmp22,_tmp21);}_LL0:;}
# 273
static int Cyc_NewControlFlow_relns_ok(struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 280
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp23=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)req->hd);
if(Cyc_Relations_consistent_relations(({struct Cyc_List_List*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->hd=_tmp23,_tmp24->tl=assume;_tmp24;})))
return 0;}
# 285
return 1;}
# 288
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp25=e->r;void*_tmp26=_tmp25;struct Cyc_Absyn_Exp*_tmp27;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->tag == 14U){_LL1: _tmp27=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26)->f2;_LL2:
 return _tmp27;}else{_LL3: _LL4:
 return e;}_LL0:;}
# 295
static void Cyc_NewControlFlow_check_union_requires(unsigned int loc,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 297
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_List_List*_tmp46;if((_tmp28.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp46=((_tmp28.ReachableFL).val).f2;_LL4:
# 300
{void*_tmp29=Cyc_Tcutil_compress(t);void*_tmp2A=_tmp29;struct Cyc_List_List*_tmp45;union Cyc_Absyn_AggrInfoU _tmp44;struct Cyc_List_List*_tmp43;switch(*((int*)_tmp2A)){case 11U: _LL6: _tmp44=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A)->f1).aggr_info;_tmp43=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A)->f1).targs;_LL7: {
# 302
struct Cyc_Absyn_Aggrdecl*_tmp2B=Cyc_Absyn_get_known_aggrdecl(_tmp44);
struct Cyc_Absyn_Aggrfield*_tmp2C=Cyc_Absyn_lookup_decl_field(_tmp2B,f);
struct Cyc_Absyn_Exp*_tmp2D=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp2C))->requires_clause;
if(_tmp2D != 0){
struct _RegionHandle _tmp2E=_new_region("temp");struct _RegionHandle*temp=& _tmp2E;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp2F=({struct _RegionHandle*_tmp63D=temp;struct Cyc_List_List*_tmp63C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp2B->tvs,_tmp43);Cyc_Tcutil_rsubsexp(_tmp63D,_tmp63C,_tmp2D);});
# 309
if(!({struct Cyc_List_List*_tmp63E=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp63E,Cyc_Relations_exp2relns(temp,_tmp2F));})){
({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp599;_tmp599.tag=0U,({struct _dyneither_ptr _tmp63F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp2F)));_tmp599.f1=_tmp63F;});_tmp599;});struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp598;_tmp598.tag=0U,_tmp598.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp598;});void*_tmp30[2U];_tmp30[0]=& _tmp32,_tmp30[1]=& _tmp33;({unsigned int _tmp641=loc;struct _dyneither_ptr _tmp640=({const char*_tmp31="unable to prove %s, required to access %s";_tag_dyneither(_tmp31,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp641,_tmp640,_tag_dyneither(_tmp30,sizeof(void*),2U));});});
({void*_tmp34=0U;({struct Cyc___cycFILE*_tmp643=Cyc_stderr;struct _dyneither_ptr _tmp642=({const char*_tmp35="  [recorded facts on path: ";_tag_dyneither(_tmp35,sizeof(char),28U);});Cyc_fprintf(_tmp643,_tmp642,_tag_dyneither(_tmp34,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp36=0U;({struct Cyc___cycFILE*_tmp645=Cyc_stderr;struct _dyneither_ptr _tmp644=({const char*_tmp37="]\n";_tag_dyneither(_tmp37,sizeof(char),3U);});Cyc_fprintf(_tmp645,_tmp644,_tag_dyneither(_tmp36,sizeof(void*),0U));});});}}
# 307
;_pop_region(temp);}
# 316
goto _LL5;}case 12U: _LL8: _tmp45=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A)->f2;_LL9: {
# 318
struct Cyc_Absyn_Aggrfield*_tmp38=Cyc_Absyn_lookup_field(_tmp45,f);
struct Cyc_Absyn_Exp*_tmp39=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp38))->requires_clause;
if(_tmp39 != 0){
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
if(!({struct Cyc_List_List*_tmp646=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp646,Cyc_Relations_exp2relns(temp,_tmp39));})){
({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp59B;_tmp59B.tag=0U,({struct _dyneither_ptr _tmp647=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp39)));_tmp59B.f1=_tmp647;});_tmp59B;});struct Cyc_String_pa_PrintArg_struct _tmp3E=({struct Cyc_String_pa_PrintArg_struct _tmp59A;_tmp59A.tag=0U,_tmp59A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp59A;});void*_tmp3B[2U];_tmp3B[0]=& _tmp3D,_tmp3B[1]=& _tmp3E;({unsigned int _tmp649=loc;struct _dyneither_ptr _tmp648=({const char*_tmp3C="unable to prove %s, required to access %s";_tag_dyneither(_tmp3C,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp649,_tmp648,_tag_dyneither(_tmp3B,sizeof(void*),2U));});});
({void*_tmp3F=0U;({struct Cyc___cycFILE*_tmp64B=Cyc_stderr;struct _dyneither_ptr _tmp64A=({const char*_tmp40="  [recorded facts on path: ";_tag_dyneither(_tmp40,sizeof(char),28U);});Cyc_fprintf(_tmp64B,_tmp64A,_tag_dyneither(_tmp3F,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp41=0U;({struct Cyc___cycFILE*_tmp64D=Cyc_stderr;struct _dyneither_ptr _tmp64C=({const char*_tmp42="]\n";_tag_dyneither(_tmp42,sizeof(char),3U);});Cyc_fprintf(_tmp64D,_tmp64C,_tag_dyneither(_tmp41,sizeof(void*),0U));});});}
# 322
;_pop_region(temp);}
# 329
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 332
goto _LL0;}_LL0:;}
# 336
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp47=inflow;struct Cyc_Dict_Dict _tmp4D;struct Cyc_List_List*_tmp4C;if((_tmp47.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp4D=((_tmp47.ReachableFL).val).f1;_tmp4C=((_tmp47.ReachableFL).val).f2;_LL4:
# 340
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4D,r)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp48=0U;({unsigned int _tmp64F=loc;struct _dyneither_ptr _tmp64E=({const char*_tmp49="expression may not be fully initialized";_tag_dyneither(_tmp49,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp64F,_tmp64E,_tag_dyneither(_tmp48,sizeof(void*),0U));});});{
struct Cyc_Dict_Dict _tmp4A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp4D,r);
if(_tmp4D.t == _tmp4A.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B=Cyc_CfFlowInfo_ReachableFL(_tmp4A,_tmp4C);
Cyc_NewControlFlow_update_tryflow(env,_tmp4B);
return _tmp4B;};};}_LL0:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 350
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp4E=({struct _tuple0 _tmp59C;({void*_tmp650=Cyc_Tcutil_compress(t);_tmp59C.f1=_tmp650;}),_tmp59C.f2=r;_tmp59C;});struct _tuple0 _tmp4F=_tmp4E;enum Cyc_Absyn_AggrKind _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;union Cyc_Absyn_AggrInfoU _tmp5D;struct Cyc_List_List*_tmp5C;struct _dyneither_ptr _tmp5B;struct Cyc_List_List*_tmp5A;struct _dyneither_ptr _tmp59;struct Cyc_Absyn_Datatypefield*_tmp58;struct _dyneither_ptr _tmp57;void*_tmp56;switch(*((int*)_tmp4F.f2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
 return;}else{switch(*((int*)_tmp4F.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F.f1)->f1).field_info).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 10U: goto _LL13;case 11U: goto _LL13;case 12U: goto _LL13;case 5U: goto _LL11;default: goto _LL13;}}case 0U: _LL3: _LL4:
 return;case 7U: _LL5: _LL6:
 return;case 8U: _LL7: _tmp56=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL8:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp56);return;default: switch(*((int*)_tmp4F.f1)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LL9: _tmp58=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp57=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLA:
# 357
 if(_tmp58->typs == 0)
return;
_tmp5A=_tmp58->typs;_tmp59=_tmp57;goto _LLC;}else{goto _LL13;}}else{goto _LL13;}case 10U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLB: _tmp5A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp59=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLC: {
# 361
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5A);
{int i=0;for(0;i < sz;(i ++,_tmp5A=_tmp5A->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp653=env;unsigned int _tmp652=loc;void*_tmp651=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5A))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp653,_tmp652,_tmp651,*((void**)_check_dyneither_subscript(_tmp59,sizeof(void*),i)));});}}
# 365
return;}}else{goto _LL13;}case 11U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLD: _tmp5D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F.f1)->f1).aggr_info;_tmp5C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F.f1)->f1).targs;_tmp5B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLE: {
# 368
struct Cyc_Absyn_Aggrdecl*_tmp50=Cyc_Absyn_get_known_aggrdecl(_tmp5D);
if(_tmp50->impl == 0)return;{
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50->impl))->fields;
struct _RegionHandle _tmp52=_new_region("temp");struct _RegionHandle*temp=& _tmp52;_push_region(temp);
{struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp50->tvs,_tmp5C);
{int i=0;for(0;i < _get_dyneither_size(_tmp5B,sizeof(void*));(i ++,_tmp51=_tmp51->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->type;
if(_tmp53 != 0)({void*_tmp654=Cyc_Tcutil_rsubstitute(temp,_tmp53,t);t=_tmp654;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp5B.curr)[i]);}}
# 378
_npop_handler(0U);return;}
# 372
;_pop_region(temp);};}}else{goto _LL13;}case 12U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLF: _tmp60=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp5F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL10:
# 381
{int i=0;for(0;i < _get_dyneither_size(_tmp5E,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((void**)_tmp5E.curr)[i]);}}
return;}else{goto _LL13;}case 5U: _LL11: _LL12:
# 385
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp54=0U;({unsigned int _tmp656=loc;struct _dyneither_ptr _tmp655=({const char*_tmp55="argument may still contain alias-free pointers";_tag_dyneither(_tmp55,sizeof(char),47U);});Cyc_Warn_warn(_tmp656,_tmp655,_tag_dyneither(_tmp54,sizeof(void*),0U));});});
return;default: _LL13: _LL14:
 return;}}_LL0:;}
# 392
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp61=inflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6D;if((_tmp61.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp6E=((_tmp61.ReachableFL).val).f1;_tmp6D=((_tmp61.ReachableFL).val).f2;_LL4:
# 396
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp62=0U;({struct _dyneither_ptr _tmp657=({const char*_tmp63="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp63,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp657,_tag_dyneither(_tmp62,sizeof(void*),0U));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 400
void*_tmp64=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp65=r;void*_tmp6A;struct Cyc_CfFlowInfo_Place*_tmp69;void*_tmp68;switch(*((int*)_tmp65)){case 8U: _LL6: _tmp68=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp65)->f2;_LL7:
 r=_tmp68;goto retry;case 4U: _LL8: _tmp69=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LL9:
({void*_tmp658=Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp69);_tmp6A=_tmp658;});goto _LLB;case 5U: _LLA: _tmp6A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LLB:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp64,_tmp6A);goto _LL5;default: _LLC: _LLD:
# 406
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp64))
({void*_tmp66=0U;({unsigned int _tmp65A=loc;struct _dyneither_ptr _tmp659=({const char*_tmp67="argument may contain live alias-free pointers";_tag_dyneither(_tmp67,sizeof(char),46U);});Cyc_Warn_warn(_tmp65A,_tmp659,_tag_dyneither(_tmp66,sizeof(void*),0U));});});
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);}_LL5:;}{
# 410
struct Cyc_Dict_Dict _tmp6B=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp6E,r);
if(_tmp6E.t == _tmp6B.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp6C=Cyc_CfFlowInfo_ReachableFL(_tmp6B,_tmp6D);
Cyc_NewControlFlow_update_tryflow(env,_tmp6C);
return _tmp6C;};};};}_LL0:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 420
static struct _tuple23 Cyc_NewControlFlow_anal_Rexps(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 425
struct Cyc_List_List*rvals=0;
if(es == 0)
return({struct _tuple23 _tmp59D;_tmp59D.f1=inflow,_tmp59D.f2=0;_tmp59D;});
# 430
do{
struct _tuple16 _tmp6F=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp70=_tmp6F;union Cyc_CfFlowInfo_FlowInfo _tmp73;void*_tmp72;_LL1: _tmp73=_tmp70.f1;_tmp72=_tmp70.f2;_LL2:;
inflow=_tmp73;
({struct Cyc_List_List*_tmp65B=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=_tmp72,_tmp71->tl=rvals;_tmp71;});rvals=_tmp65B;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 438
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp59E;_tmp59E.f1=inflow,({struct Cyc_List_List*_tmp65C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp59E.f2=_tmp65C;});_tmp59E;});}
# 444
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 446
struct _tuple23 _tmp74=
Cyc_NewControlFlow_anal_Rexps(env,inflow,es,0,0);
# 446
struct _tuple23 _tmp75=_tmp74;union Cyc_CfFlowInfo_FlowInfo _tmp7B;struct Cyc_List_List*_tmp7A;_LL1: _tmp7B=_tmp75.f1;_tmp7A=_tmp75.f2;_LL2:;
# 448
{union Cyc_CfFlowInfo_FlowInfo _tmp76=_tmp7B;struct Cyc_Dict_Dict _tmp79;if((_tmp76.ReachableFL).tag == 2){_LL4: _tmp79=((_tmp76.ReachableFL).val).f1;_LL5:
# 450
 for(0;_tmp7A != 0;(_tmp7A=_tmp7A->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp79,(void*)_tmp7A->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp77=0U;({unsigned int _tmp65E=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp65D=({const char*_tmp78="expression may not be initialized";_tag_dyneither(_tmp78,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp65E,_tmp65D,_tag_dyneither(_tmp77,sizeof(void*),0U));});});}
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 456
return({struct _tuple16 _tmp59F;_tmp59F.f1=_tmp7B,_tmp59F.f2=(env->fenv)->unknown_all;_tmp59F;});}
# 463
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 470
int needs_unconsume=0;
void*_tmp7C=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp7C,& needs_unconsume))
({void*_tmp7D=0U;({struct _dyneither_ptr _tmp65F=({const char*_tmp7E="consume_zero_rval";_tag_dyneither(_tmp7E,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp65F,_tag_dyneither(_tmp7D,sizeof(void*),0U));});});else{
# 475
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 479
return new_rval;}}}
# 492 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 497
union Cyc_CfFlowInfo_FlowInfo _tmp7F=outflow;struct Cyc_Dict_Dict _tmp86;struct Cyc_List_List*_tmp85;if((_tmp7F.BottomFL).tag == 1){_LL1: _LL2:
 return outflow;}else{_LL3: _tmp86=((_tmp7F.ReachableFL).val).f1;_tmp85=((_tmp7F.ReachableFL).val).f2;_LL4: {
# 500
union Cyc_CfFlowInfo_AbsLVal _tmp80=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp81=_tmp80;struct Cyc_CfFlowInfo_Place*_tmp84;if((_tmp81.UnknownL).tag == 2){_LL6: _LL7:
# 504
 return outflow;}else{_LL8: _tmp84=(_tmp81.PlaceL).val;_LL9: {
# 508
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
for(0;names != 0;names=names->tl){
({void*_tmp660=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=8U,_tmp82->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp82->f2=nzval;_tmp82;});nzval=_tmp660;});}
# 512
({void*_tmp661=Cyc_NewControlFlow_consume_zero_rval(env,_tmp86,_tmp84,e,nzval);nzval=_tmp661;});{
union Cyc_CfFlowInfo_FlowInfo _tmp83=({struct Cyc_Dict_Dict _tmp662=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp86,_tmp84,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp662,_tmp85);});
# 517
return _tmp83;};}}_LL5:;}}_LL0:;}
# 526
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 528
union Cyc_CfFlowInfo_FlowInfo _tmp87=outflow;struct Cyc_Dict_Dict _tmp8E;struct Cyc_List_List*_tmp8D;if((_tmp87.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple19 _tmp5A0;_tmp5A0.f1=outflow,_tmp5A0.f2=outflow;_tmp5A0;});}else{_LL3: _tmp8E=((_tmp87.ReachableFL).val).f1;_tmp8D=((_tmp87.ReachableFL).val).f2;_LL4: {
# 531
union Cyc_CfFlowInfo_AbsLVal _tmp88=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp89=_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8C;if((_tmp89.UnknownL).tag == 2){_LL6: _LL7:
 return({struct _tuple19 _tmp5A1;_tmp5A1.f1=outflow,_tmp5A1.f2=outflow;_tmp5A1;});}else{_LL8: _tmp8C=(_tmp89.PlaceL).val;_LL9: {
# 534
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp663=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=8U,_tmp8A->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8A->f2=nzval;_tmp8A;});nzval=_tmp663;});
({void*_tmp664=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=8U,_tmp8B->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8B->f2=zval;_tmp8B;});zval=_tmp664;});}
# 540
({void*_tmp665=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,nzval);nzval=_tmp665;});
({void*_tmp666=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,zval);zval=_tmp666;});
return({struct _tuple19 _tmp5A2;({
union Cyc_CfFlowInfo_FlowInfo _tmp66A=({struct Cyc_Dict_Dict _tmp669=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp669,_tmp8D);});_tmp5A2.f1=_tmp66A;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp668=({struct Cyc_Dict_Dict _tmp667=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,zval);Cyc_CfFlowInfo_ReachableFL(_tmp667,_tmp8D);});_tmp5A2.f2=_tmp668;});_tmp5A2;});}}_LL5:;}}_LL0:;}
# 549
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 552
static void Cyc_NewControlFlow_update_relns(struct Cyc_Absyn_Exp*e,int possibly_null,struct Cyc_List_List*relns){
void*_tmp8F=e->r;void*_tmp90=_tmp8F;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp90)->tag == 23U){_LL1: _LL2:
# 556
{void*_tmp91=e->annot;void*_tmp92=_tmp91;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp93;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp92)->tag == Cyc_CfFlowInfo_UnknownZ){_LL6: _tmp93=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp92)->f1;_LL7:
# 558
 if(possibly_null  && Cyc_Relations_same_relns(relns,_tmp93))return;
goto _LL5;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp92)->tag == Cyc_CfFlowInfo_NotZero){_LL8: _tmp94=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp92)->f1;_LL9:
# 561
 possibly_null=0;
if(Cyc_Relations_same_relns(relns,_tmp94))return;
goto _LL5;}else{_LLA: _LLB:
 goto _LL5;}}_LL5:;}
# 566
if(possibly_null)
({void*_tmp66B=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=Cyc_CfFlowInfo_UnknownZ,_tmp95->f1=relns;_tmp95;});e->annot=_tmp66B;});else{
# 570
({void*_tmp66C=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=Cyc_CfFlowInfo_NotZero,_tmp96->f1=relns;_tmp96;});e->annot=_tmp66C;});}
# 572
goto _LL0;}else{_LL3: _LL4:
# 576
 if(possibly_null){
void*_tmp97=e->annot;void*_tmp98=_tmp97;struct Cyc_List_List*_tmp99;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp98)->tag == Cyc_CfFlowInfo_UnknownZ){_LLD: _tmp99=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp98)->f1;_LLE:
# 579
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 584
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL0;}_LL0:;}
# 594
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 597
void*_tmp9A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp9B=_tmp9A;void*_tmpBF;union Cyc_Absyn_Constraint*_tmpBE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->tag == 5U){_LL1: _tmpBF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->f1).elt_typ;_tmpBE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->f1).ptr_atts).bounds;_LL2: {
# 599
union Cyc_CfFlowInfo_FlowInfo _tmp9C=f;struct Cyc_Dict_Dict _tmpBB;struct Cyc_List_List*_tmpBA;if((_tmp9C.BottomFL).tag == 1){_LL6: _LL7:
# 601
 return({struct _tuple16 _tmp5A3;_tmp5A3.f1=f,({void*_tmp66D=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A3.f2=_tmp66D;});_tmp5A3;});}else{_LL8: _tmpBB=((_tmp9C.ReachableFL).val).f1;_tmpBA=((_tmp9C.ReachableFL).val).f2;_LL9:
# 604
 if(Cyc_Tcutil_is_bound_one(_tmpBE)){
struct _tuple15 _tmp9D=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp9E=_tmp9D;void*_tmpAB;struct Cyc_List_List*_tmpAA;_LLB: _tmpAB=_tmp9E.f1;_tmpAA=_tmp9E.f2;_LLC:;{
void*_tmp9F=_tmpAB;enum Cyc_CfFlowInfo_InitLevel _tmpA9;void*_tmpA8;struct Cyc_CfFlowInfo_Place*_tmpA7;struct Cyc_Absyn_Vardecl*_tmpA6;void*_tmpA5;switch(*((int*)_tmp9F)){case 8U: _LLE: _tmpA6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_tmpA5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f2;_LLF:
# 608
({void*_tmpA0=0U;({struct _dyneither_ptr _tmp66E=({const char*_tmpA1="named location in anal_derefR";_tag_dyneither(_tmpA1,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp66E,_tag_dyneither(_tmpA0,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 610
 Cyc_NewControlFlow_update_relns(e,0,_tmpBA);
goto _LLD;case 4U: _LL12: _tmpA7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL13:
# 613
 Cyc_NewControlFlow_update_relns(e,0,_tmpBA);
return({struct _tuple16 _tmp5A4;_tmp5A4.f1=f,({void*_tmp66F=Cyc_CfFlowInfo_lookup_place(_tmpBB,_tmpA7);_tmp5A4.f2=_tmp66F;});_tmp5A4;});case 5U: _LL14: _tmpA8=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL15:
# 616
 Cyc_NewControlFlow_update_relns(e,1,_tmpBA);
return({struct _tuple16 _tmp5A5;_tmp5A5.f1=f,_tmp5A5.f2=_tmpA8;_tmp5A5;});case 0U: _LL16: _LL17:
# 619
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmp5A6;({union Cyc_CfFlowInfo_FlowInfo _tmp671=Cyc_CfFlowInfo_BottomFL();_tmp5A6.f1=_tmp671;}),({void*_tmp670=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A6.f2=_tmp670;});_tmp5A6;});case 2U: _LL18: _tmpA9=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL19:
# 623
({union Cyc_CfFlowInfo_FlowInfo _tmp672=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA9,_tmpAA);f=_tmp672;});
goto _LL1B;default: _LL1A: _LL1B:
# 626
{void*_tmpA2=e->r;void*_tmpA3=_tmpA2;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA3)->tag == 23U){_LL1D: _LL1E:
# 628
({void*_tmp673=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=Cyc_CfFlowInfo_UnknownZ,_tmpA4->f1=_tmpBA;_tmpA4;});e->annot=_tmp673;});
goto _LL1C;}else{_LL1F: _LL20:
# 631
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL1C;}_LL1C:;}
# 634
goto _LLD;}_LLD:;};}else{
# 637
void*_tmpAC=e->annot;void*_tmpAD=_tmpAC;struct Cyc_List_List*_tmpAF;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL22: _tmpAF=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpAD)->f1;_LL23:
# 639
 if(!Cyc_Relations_same_relns(_tmpBA,_tmpAF))goto _LL25;
goto _LL21;}else{_LL24: _LL25:
# 642
({void*_tmp674=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=Cyc_CfFlowInfo_UnknownZ,_tmpAE->f1=_tmpBA;_tmpAE;});e->annot=_tmp674;});
goto _LL21;}_LL21:;}{
# 646
enum Cyc_CfFlowInfo_InitLevel _tmpB0=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpBB,r);enum Cyc_CfFlowInfo_InitLevel _tmpB1=_tmpB0;if(_tmpB1 == Cyc_CfFlowInfo_NoneIL){_LL27: _LL28: {
# 648
struct _tuple15 _tmpB2=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpB3=_tmpB2;void*_tmpB9;_LL2C: _tmpB9=_tmpB3.f1;_LL2D:;
{void*_tmpB4=_tmpB9;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB4)->tag == 7U){_LL2F: _LL30:
# 651
({void*_tmpB5=0U;({unsigned int _tmp676=e->loc;struct _dyneither_ptr _tmp675=({const char*_tmpB6="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpB6,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp676,_tmp675,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});
goto _LL2E;}else{_LL31: _LL32:
# 654
({void*_tmpB7=0U;({unsigned int _tmp678=e->loc;struct _dyneither_ptr _tmp677=({const char*_tmpB8="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpB8,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp678,_tmp677,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});}_LL2E:;}
# 656
goto _LL2A;}}else{_LL29: _LL2A:
# 658
 return({struct _tuple16 _tmp5A7;_tmp5A7.f1=f,({void*_tmp679=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A7.f2=_tmp679;});_tmp5A7;});}_LL26:;};}_LL5:;}}else{_LL3: _LL4:
# 661
({void*_tmpBC=0U;({struct _dyneither_ptr _tmp67A=({const char*_tmpBD="right deref of non-pointer-type";_tag_dyneither(_tmpBD,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp67A,_tag_dyneither(_tmpBC,sizeof(void*),0U));});});}_LL0:;}
# 668
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 670
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 673
{void*_tmpC0=e1->r;void*_tmpC1=_tmpC0;void*_tmpC3;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC1)->tag == 1U){_LL1: _tmpC3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL2: {
# 675
struct Cyc_Absyn_Vardecl*_tmpC2=Cyc_Tcutil_nonesc_vardecl(_tmpC3);
if(_tmpC2 != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmpC2);
if(e2_valid_op)
({struct Cyc_List_List*_tmp67B=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp67B;});}
# 681
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 685
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp67C=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp67C;});}
# 691
return relns;};}
# 699
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 704
struct _tuple18 _tmpC4=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpC5=_tmpC4;union Cyc_CfFlowInfo_AbsLVal _tmpD4;_LL1: _tmpD4=_tmpC5.f2;_LL2:;
{struct _tuple18 _tmpC6=({struct _tuple18 _tmp5A9;_tmp5A9.f1=inflow,_tmp5A9.f2=_tmpD4;_tmp5A9;});struct _tuple18 _tmpC7=_tmpC6;struct Cyc_Dict_Dict _tmpD3;struct Cyc_List_List*_tmpD2;struct Cyc_CfFlowInfo_Place*_tmpD1;if(((_tmpC7.f1).ReachableFL).tag == 2){if(((_tmpC7.f2).PlaceL).tag == 1){_LL4: _tmpD3=(((_tmpC7.f1).ReachableFL).val).f1;_tmpD2=(((_tmpC7.f1).ReachableFL).val).f2;_tmpD1=((_tmpC7.f2).PlaceL).val;_LL5: {
# 707
void*_tmpC8=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 709
struct _tuple15 _tmpC9=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple15 _tmpCA=_tmpC9;void*_tmpCD;struct Cyc_List_List*_tmpCC;_LLB: _tmpCD=_tmpCA.f1;_tmpCC=_tmpCA.f2;_LLC:;
for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
({void*_tmp67D=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=8U,_tmpCB->f1=(struct Cyc_Absyn_Vardecl*)_tmpCC->hd,_tmpCB->f2=_tmpC8;_tmpCB;});_tmpC8=_tmp67D;});}
# 714
({struct Cyc_Dict_Dict _tmp67E=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpD3,_tmpD1,_tmpC8);_tmpD3=_tmp67E;});
({union Cyc_CfFlowInfo_FlowInfo _tmp67F=Cyc_CfFlowInfo_ReachableFL(_tmpD3,_tmpD2);inflow=_tmp67F;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 720
goto _LL3;}}else{_LL8: _LL9:
# 723
({struct Cyc_String_pa_PrintArg_struct _tmpD0=({struct Cyc_String_pa_PrintArg_struct _tmp5A8;_tmp5A8.tag=0U,({struct _dyneither_ptr _tmp680=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(exp));_tmp5A8.f1=_tmp680;});_tmp5A8;});void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({struct _dyneither_ptr _tmp681=({const char*_tmpCF="noconsume parameter %s must be l-values";_tag_dyneither(_tmpCF,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp681,_tag_dyneither(_tmpCE,sizeof(void*),1U));});});
goto _LL3;}}else{_LL6: _LL7:
# 721
 goto _LL3;}_LL3:;}
# 726
return inflow;}
# 731
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 738
union Cyc_CfFlowInfo_FlowInfo _tmpD5=outflow;struct Cyc_Dict_Dict _tmpE0;struct Cyc_List_List*_tmpDF;if((_tmpD5.BottomFL).tag == 1){_LL1: _LL2:
# 740
 return({struct _tuple16 _tmp5AA;({union Cyc_CfFlowInfo_FlowInfo _tmp682=Cyc_CfFlowInfo_BottomFL();_tmp5AA.f1=_tmp682;}),_tmp5AA.f2=rval;_tmp5AA;});}else{_LL3: _tmpE0=((_tmpD5.ReachableFL).val).f1;_tmpDF=((_tmpD5.ReachableFL).val).f2;_LL4: {
# 742
union Cyc_CfFlowInfo_AbsLVal _tmpD6=lval;struct Cyc_CfFlowInfo_Place*_tmpDE;if((_tmpD6.PlaceL).tag == 1){_LL6: _tmpDE=(_tmpD6.PlaceL).val;_LL7: {
# 750
struct Cyc_Dict_Dict _tmpD7=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpE0,_tmpDE,rval);
({struct Cyc_List_List*_tmp683=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,_tmpDF,lexp,rexp);_tmpDF=_tmp683;});
({union Cyc_CfFlowInfo_FlowInfo _tmp684=Cyc_CfFlowInfo_ReachableFL(_tmpD7,_tmpDF);outflow=_tmp684;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 756
struct _tuple15 _tmpD8=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(_tmpE0,_tmpDE));struct _tuple15 _tmpD9=_tmpD8;void*_tmpDD;_LLB: _tmpDD=_tmpD9.f1;_LLC:;{
void*_tmpDA=_tmpDD;switch(*((int*)_tmpDA)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpDA)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE: _LLF:
 goto _LL11;}else{goto _LL14;}case 7U: _LL10: _LL11:
 goto _LL13;case 0U: _LL12: _LL13:
 goto _LLD;default: _LL14: _LL15:
# 762
({void*_tmpDB=0U;({unsigned int _tmp686=lexp->loc;struct _dyneither_ptr _tmp685=({const char*_tmpDC="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpDC,sizeof(char),47U);});Cyc_Warn_warn(_tmp686,_tmp685,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
goto _LLD;}_LLD:;};}
# 767
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmp5AB;_tmp5AB.f1=outflow,_tmp5AB.f2=rval;_tmp5AB;});}}else{_LL8: _LL9:
# 773
 return({struct _tuple16 _tmp5AC;({union Cyc_CfFlowInfo_FlowInfo _tmp687=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp5AC.f1=_tmp687;}),_tmp5AC.f2=rval;_tmp5AC;});}_LL5:;}}_LL0:;}
# 780
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 786
union Cyc_CfFlowInfo_FlowInfo _tmpE1=f;struct Cyc_Dict_Dict _tmpE6;struct Cyc_List_List*_tmpE5;if((_tmpE1.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmpE6=((_tmpE1.ReachableFL).val).f1;_tmpE5=((_tmpE1.ReachableFL).val).f2;_LL4:
# 791
({struct Cyc_Dict_Dict _tmp68D=({struct Cyc_CfFlowInfo_FlowEnv*_tmp68C=fenv;unsigned int _tmp68B=loc;struct Cyc_Dict_Dict _tmp68A=_tmpE6;struct Cyc_CfFlowInfo_Place*_tmp689=({struct Cyc_CfFlowInfo_Place*_tmpE3=_cycalloc(sizeof(*_tmpE3));
({void*_tmp688=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->tag=0U,_tmpE2->f1=vd;_tmpE2;});_tmpE3->root=_tmp688;}),_tmpE3->path=0;_tmpE3;});
# 791
Cyc_CfFlowInfo_assign_place(_tmp68C,_tmp68B,_tmp68A,_tmp689,rval);});_tmpE6=_tmp68D;});
# 794
({struct Cyc_List_List*_tmp68E=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,_tmpE5,vd,rexp);_tmpE5=_tmp68E;});{
union Cyc_CfFlowInfo_FlowInfo _tmpE4=Cyc_CfFlowInfo_ReachableFL(_tmpE6,_tmpE5);
Cyc_NewControlFlow_update_tryflow(env,_tmpE4);
# 799
return _tmpE4;};}_LL0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 803
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 810
if(vds == 0)return inflow;{
# 813
struct Cyc_List_List*_tmpE7=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp68F=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),_tmpE8->tl=es;_tmpE8;});es=_tmp68F;});}}
# 819
({union Cyc_CfFlowInfo_FlowInfo _tmp690=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmpE7,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp690;});
# 821
({union Cyc_CfFlowInfo_FlowInfo _tmp691=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp691;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 825
struct _tuple16 _tmpE9=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmpEA=_tmpE9;union Cyc_CfFlowInfo_FlowInfo _tmpEC;void*_tmpEB;_LL1: _tmpEC=_tmpEA.f1;_tmpEB=_tmpEA.f2;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp692=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmpEC,_tmpEB);inflow=_tmp692;});}}{
# 833
struct Cyc_List_List*_tmpED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _tuple24*_tmpEE=(struct _tuple24*)_tmpED->hd;struct _tuple24*_tmpEF=_tmpEE;struct Cyc_Absyn_Vardecl**_tmp107;struct Cyc_Absyn_Exp*_tmp106;_LL4: _tmp107=_tmpEF->f1;_tmp106=_tmpEF->f2;_LL5:;
if(_tmp107 != 0  && _tmp106 != 0){
if(_tmp106->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp5AD;_tmp5AD.tag=0U,({
struct _dyneither_ptr _tmp693=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp106));_tmp5AD.f1=_tmp693;});_tmp5AD;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF2;({struct _dyneither_ptr _tmp694=({const char*_tmpF1="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmpF1,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp694,_tag_dyneither(_tmpF0,sizeof(void*),1U));});});{
# 847
struct Cyc_List_List l=({struct Cyc_List_List _tmp5AF;_tmp5AF.hd=_tmp106,_tmp5AF.tl=0;_tmp5AF;});
union Cyc_CfFlowInfo_FlowInfo _tmpF3=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmpF4=Cyc_NewControlFlow_anal_Lexp(env,_tmpF3,0,0,_tmp106);struct _tuple18 _tmpF5=_tmpF4;union Cyc_CfFlowInfo_AbsLVal _tmp105;_LL7: _tmp105=_tmpF5.f2;_LL8:;{
struct _tuple16 _tmpF6=Cyc_NewControlFlow_anal_Rexp(env,1,_tmpF3,_tmp106);struct _tuple16 _tmpF7=_tmpF6;union Cyc_CfFlowInfo_FlowInfo _tmp104;void*_tmp103;_LLA: _tmp104=_tmpF7.f1;_tmp103=_tmpF7.f2;_LLB:;{
union Cyc_CfFlowInfo_FlowInfo _tmpF8=_tmp104;struct Cyc_Dict_Dict _tmp102;struct Cyc_List_List*_tmp101;if((_tmpF8.ReachableFL).tag == 2){_LLD: _tmp102=((_tmpF8.ReachableFL).val).f1;_tmp101=((_tmpF8.ReachableFL).val).f2;_LLE:
# 853
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmpF9=_tmp105;struct Cyc_CfFlowInfo_Place*_tmp100;if((_tmpF9.PlaceL).tag == 1){_LL12: _tmp100=(_tmpF9.PlaceL).val;_LL13:
# 856
({void*_tmp695=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=8U,_tmpFA->f1=*_tmp107,_tmpFA->f2=_tmp103;_tmpFA;});_tmp103=_tmp695;});{
# 859
void*_tmpFB=Cyc_CfFlowInfo_lookup_place(_tmp102,_tmp100);
({void*_tmp696=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=8U,_tmpFC->f1=*_tmp107,_tmpFC->f2=_tmpFB;_tmpFC;});_tmpFB=_tmp696;});
({struct Cyc_Dict_Dict _tmp697=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp102,_tmp100,_tmpFB);_tmp102=_tmp697;});
({union Cyc_CfFlowInfo_FlowInfo _tmp698=Cyc_CfFlowInfo_ReachableFL(_tmp102,_tmp101);_tmp104=_tmp698;});
goto _LL11;};}else{_LL14: _LL15:
# 866
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp106->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp107)->type))
({struct Cyc_String_pa_PrintArg_struct _tmpFF=({struct Cyc_String_pa_PrintArg_struct _tmp5AE;_tmp5AE.tag=0U,({
struct _dyneither_ptr _tmp699=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp106));_tmp5AE.f1=_tmp699;});_tmp5AE;});void*_tmpFD[1U];_tmpFD[0]=& _tmpFF;({unsigned int _tmp69B=exp_loc;struct _dyneither_ptr _tmp69A=({const char*_tmpFE="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmpFE,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp69B,_tmp69A,_tag_dyneither(_tmpFD,sizeof(void*),1U));});});}_LL11:;}
# 876
({union Cyc_CfFlowInfo_FlowInfo _tmp69C=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp104,*_tmp107,_tmp106,_tmp103,pat_loc);inflow=_tmp69C;});
goto _LLC;}else{_LLF: _LL10:
# 879
 goto _LLC;}_LLC:;};};};}}
# 884
return inflow;};};}
# 887
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 889
void*_tmp108=e->r;void*_tmp109=_tmp108;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;switch(*((int*)_tmp109)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp109)->f1)){case 4U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: goto _LL11;}case 20U: _LL7: _tmp10C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp109)->f1;_LL8:
 _tmp10D=_tmp10C;goto _LLA;case 21U: _LL9: _tmp10D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp109)->f1;_LLA:
 _tmp10E=_tmp10D;goto _LLC;case 22U: _LLB: _tmp10E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp109)->f1;_LLC:
# 896
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp10E,cast_ok);case 23U: _LLD: _tmp10F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp109)->f1;_LLE: {
# 898
void*_tmp10A=Cyc_Tcutil_compress((void*)_check_null(_tmp10F->topt));void*_tmp10B=_tmp10A;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp10B)->tag == 10U){_LL14: _LL15:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp10F,cast_ok);}else{_LL16: _LL17:
 return 0;}_LL13:;}case 14U: _LLF: _tmp110=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp109)->f2;_LL10:
# 903
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp110,cast_ok);else{
return 0;}default: _LL11: _LL12:
 return 0;}_LL0:;}
# 909
static int Cyc_NewControlFlow_subst_param(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp*rop){
union Cyc_Relations_RelnOp _tmp111=*rop;union Cyc_Relations_RelnOp _tmp112=_tmp111;unsigned int _tmp116;unsigned int _tmp115;switch((_tmp112.RParamNumelts).tag){case 5U: _LL1: _tmp115=(_tmp112.RParam).val;_LL2: {
# 912
struct Cyc_Absyn_Exp*_tmp113=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp115);
return Cyc_Relations_exp2relnop(_tmp113,rop);}case 6U: _LL3: _tmp116=(_tmp112.RParamNumelts).val;_LL4: {
# 915
struct Cyc_Absyn_Exp*_tmp114=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp116);
return({struct Cyc_Absyn_Exp*_tmp69D=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp114,0U);Cyc_Relations_exp2relnop(_tmp69D,rop);});}default: _LL5: _LL6:
 return 1;}_LL0:;}
# 921
static struct _dyneither_ptr Cyc_NewControlFlow_subst_param_string(struct Cyc_List_List*exps,union Cyc_Relations_RelnOp rop){
union Cyc_Relations_RelnOp _tmp117=rop;unsigned int _tmp119;unsigned int _tmp118;switch((_tmp117.RParamNumelts).tag){case 5U: _LL1: _tmp118=(_tmp117.RParam).val;_LL2:
# 924
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp118));case 6U: _LL3: _tmp119=(_tmp117.RParamNumelts).val;_LL4:
# 926
 return Cyc_Absynpp_exp2string(((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(exps,(int)_tmp119));default: _LL5: _LL6:
 return Cyc_Relations_rop2string(rop);}_LL0:;}
# 931
static void Cyc_NewControlFlow_check_fn_requires(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*exps,struct Cyc_List_List*req,unsigned int loc){
# 934
union Cyc_CfFlowInfo_FlowInfo _tmp11A=inflow;struct Cyc_Dict_Dict _tmp124;struct Cyc_List_List*_tmp123;if((_tmp11A.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp124=((_tmp11A.ReachableFL).val).f1;_tmp123=((_tmp11A.ReachableFL).val).f2;_LL4:
# 937
 for(0;req != 0;req=req->tl){
struct Cyc_Relations_Reln*_tmp11B=(struct Cyc_Relations_Reln*)req->hd;
union Cyc_Relations_RelnOp rop1=_tmp11B->rop1;
union Cyc_Relations_RelnOp rop2=_tmp11B->rop2;
enum Cyc_Relations_Relation _tmp11C=Cyc_Relations_flip_relation(_tmp11B->relation);
if((!Cyc_NewControlFlow_subst_param(exps,& rop1) || !Cyc_NewControlFlow_subst_param(exps,& rop2)) || 
Cyc_Relations_consistent_relations(Cyc_Relations_add_relation(Cyc_Core_heap_region,rop2,_tmp11C,rop1,_tmp123))){
struct _dyneither_ptr s1=Cyc_NewControlFlow_subst_param_string(exps,rop1);
struct _dyneither_ptr s2=Cyc_Relations_relation2string(_tmp11B->relation);
struct _dyneither_ptr s3=Cyc_NewControlFlow_subst_param_string(exps,rop2);
({struct Cyc_String_pa_PrintArg_struct _tmp11F=({struct Cyc_String_pa_PrintArg_struct _tmp5B3;_tmp5B3.tag=0U,_tmp5B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp5B3;});struct Cyc_String_pa_PrintArg_struct _tmp120=({struct Cyc_String_pa_PrintArg_struct _tmp5B2;_tmp5B2.tag=0U,_tmp5B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp5B2;});struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp5B1;_tmp5B1.tag=0U,_tmp5B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp5B1;});struct Cyc_String_pa_PrintArg_struct _tmp122=({struct Cyc_String_pa_PrintArg_struct _tmp5B0;_tmp5B0.tag=0U,({
struct _dyneither_ptr _tmp69E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_relns2string(_tmp123));_tmp5B0.f1=_tmp69E;});_tmp5B0;});void*_tmp11D[4U];_tmp11D[0]=& _tmp11F,_tmp11D[1]=& _tmp120,_tmp11D[2]=& _tmp121,_tmp11D[3]=& _tmp122;({unsigned int _tmp6A0=loc;struct _dyneither_ptr _tmp69F=({const char*_tmp11E="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp11E,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp6A0,_tmp69F,_tag_dyneither(_tmp11D,sizeof(void*),4U));});});
break;}}
# 952
goto _LL0;}_LL0:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 957
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 961
struct Cyc_CfFlowInfo_FlowEnv*_tmp125=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 974
{union Cyc_CfFlowInfo_FlowInfo _tmp126=inflow;struct Cyc_Dict_Dict _tmp128;struct Cyc_List_List*_tmp127;if((_tmp126.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple16 _tmp5B4;({union Cyc_CfFlowInfo_FlowInfo _tmp6A1=Cyc_CfFlowInfo_BottomFL();_tmp5B4.f1=_tmp6A1;}),_tmp5B4.f2=_tmp125->unknown_all;_tmp5B4;});}else{_LL3: _tmp128=((_tmp126.ReachableFL).val).f1;_tmp127=((_tmp126.ReachableFL).val).f2;_LL4:
 d=_tmp128;relns=_tmp127;}_LL0:;}
# 989 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 1010 "new_control_flow.cyc"
struct _tuple18 _tmp129=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp12A=_tmp129;union Cyc_CfFlowInfo_FlowInfo _tmp136;union Cyc_CfFlowInfo_AbsLVal _tmp135;_LL6: _tmp136=_tmp12A.f1;_tmp135=_tmp12A.f2;_LL7:;{
struct _tuple18 _tmp12B=({struct _tuple18 _tmp5B8;_tmp5B8.f1=_tmp136,_tmp5B8.f2=_tmp135;_tmp5B8;});struct _tuple18 _tmp12C=_tmp12B;struct Cyc_Dict_Dict _tmp134;struct Cyc_List_List*_tmp133;struct Cyc_CfFlowInfo_Place*_tmp132;if(((_tmp12C.f1).ReachableFL).tag == 2){if(((_tmp12C.f2).PlaceL).tag == 1){_LL9: _tmp134=(((_tmp12C.f1).ReachableFL).val).f1;_tmp133=(((_tmp12C.f1).ReachableFL).val).f2;_tmp132=((_tmp12C.f2).PlaceL).val;_LLA: {
# 1013
void*_tmp12D=Cyc_CfFlowInfo_lookup_place(_tmp134,_tmp132);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp12D,& needs_unconsume)){
({void*_tmp12E=0U;({unsigned int _tmp6A3=e->loc;struct _dyneither_ptr _tmp6A2=({const char*_tmp12F="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp12F,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6A3,_tmp6A2,_tag_dyneither(_tmp12E,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5B5;({union Cyc_CfFlowInfo_FlowInfo _tmp6A4=Cyc_CfFlowInfo_BottomFL();_tmp5B5.f1=_tmp6A4;}),_tmp5B5.f2=_tmp125->unknown_all;_tmp5B5;});}else{
# 1019
if(needs_unconsume)
# 1021
return({struct _tuple16 _tmp5B6;_tmp5B6.f1=_tmp136,({void*_tmp6A5=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp125,_tmp12D);_tmp5B6.f2=_tmp6A5;});_tmp5B6;});else{
# 1024
void*_tmp130=Cyc_CfFlowInfo_make_unique_consumed(_tmp125,(void*)_check_null(e->topt),e,env->iteration_num,_tmp12D);
struct Cyc_Dict_Dict _tmp131=Cyc_CfFlowInfo_assign_place(_tmp125,e->loc,_tmp134,_tmp132,_tmp130);
# 1036
return({struct _tuple16 _tmp5B7;({union Cyc_CfFlowInfo_FlowInfo _tmp6A6=Cyc_CfFlowInfo_ReachableFL(_tmp131,_tmp133);_tmp5B7.f1=_tmp6A6;}),_tmp5B7.f2=_tmp12D;_tmp5B7;});}}}}else{goto _LLB;}}else{_LLB: _LLC:
# 1040
 goto _LL8;}_LL8:;};}{
# 1043
void*_tmp137=e->r;void*_tmp138=_tmp137;struct Cyc_Absyn_Stmt*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2EF;void*_tmp2EE;int _tmp2ED;struct Cyc_Absyn_Vardecl*_tmp2EC;struct Cyc_Absyn_Exp*_tmp2EB;struct Cyc_Absyn_Exp*_tmp2EA;int _tmp2E9;struct Cyc_List_List*_tmp2E8;struct Cyc_List_List*_tmp2E7;enum Cyc_Absyn_AggrKind _tmp2E6;struct Cyc_List_List*_tmp2E5;struct Cyc_List_List*_tmp2E4;struct Cyc_List_List*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;struct Cyc_Absyn_Exp*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2D9;struct _dyneither_ptr*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D7;struct _dyneither_ptr*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct _dyneither_ptr*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;int _tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;void**_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;int _tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_List_List*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;enum Cyc_Absyn_Primop _tmp2BE;struct Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Datatypedecl*_tmp2BB;struct Cyc_Absyn_Vardecl*_tmp2BA;struct Cyc_Absyn_Vardecl*_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2B8;struct _dyneither_ptr _tmp2B7;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;switch(*((int*)_tmp138)){case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp138)->f4 == Cyc_Absyn_Null_to_NonNull){_LLE: _tmp2B2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LLF: {
# 1047
struct _tuple16 _tmp139=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2B2);struct _tuple16 _tmp13A=_tmp139;union Cyc_CfFlowInfo_FlowInfo _tmp140;void*_tmp13F;_LL7F: _tmp140=_tmp13A.f1;_tmp13F=_tmp13A.f2;_LL80:;{
struct _tuple16 _tmp13B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp140,_tmp2B2,_tmp13F);struct _tuple16 _tmp13C=_tmp13B;union Cyc_CfFlowInfo_FlowInfo _tmp13E;void*_tmp13D;_LL82: _tmp13E=_tmp13C.f1;_tmp13D=_tmp13C.f2;_LL83:;
return({struct _tuple16 _tmp5B9;_tmp5B9.f1=_tmp13E,_tmp5B9.f2=_tmp13F;_tmp5B9;});};}}else{_LL10: _tmp2B3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL11:
# 1053
 _tmp2B4=_tmp2B3;goto _LL13;}case 12U: _LL12: _tmp2B4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL13:
 _tmp2B5=_tmp2B4;goto _LL15;case 41U: _LL14: _tmp2B5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL15:
 _tmp2B6=_tmp2B5;goto _LL17;case 13U: _LL16: _tmp2B6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL17:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2B6);case 2U: _LL18: _tmp2B7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL19:
# 1059
 if(!({struct _dyneither_ptr _tmp6A7=(struct _dyneither_ptr)_tmp2B7;Cyc_NewControlFlow_strcmp(_tmp6A7,({const char*_tmp141="print_flow";_tag_dyneither(_tmp141,sizeof(char),11U);}));})){
struct _dyneither_ptr _tmp142=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp5BA;_tmp5BA.tag=0U,_tmp5BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp142);_tmp5BA;});void*_tmp143[1U];_tmp143[0]=& _tmp145;({struct Cyc___cycFILE*_tmp6A9=Cyc_stderr;struct _dyneither_ptr _tmp6A8=({const char*_tmp144="%s: current flow is\n";_tag_dyneither(_tmp144,sizeof(char),21U);});Cyc_fprintf(_tmp6A9,_tmp6A8,_tag_dyneither(_tmp143,sizeof(void*),1U));});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp146=0U;({struct Cyc___cycFILE*_tmp6AB=Cyc_stderr;struct _dyneither_ptr _tmp6AA=({const char*_tmp147="\n";_tag_dyneither(_tmp147,sizeof(char),2U);});Cyc_fprintf(_tmp6AB,_tmp6AA,_tag_dyneither(_tmp146,sizeof(void*),0U));});});}else{
# 1065
if(!({struct _dyneither_ptr _tmp6AC=(struct _dyneither_ptr)_tmp2B7;Cyc_NewControlFlow_strcmp(_tmp6AC,({const char*_tmp148="debug_on";_tag_dyneither(_tmp148,sizeof(char),9U);}));}));else{
# 1070
if(!({struct _dyneither_ptr _tmp6AD=(struct _dyneither_ptr)_tmp2B7;Cyc_NewControlFlow_strcmp(_tmp6AD,({const char*_tmp149="debug_off";_tag_dyneither(_tmp149,sizeof(char),10U);}));}));else{
# 1076
({struct Cyc_String_pa_PrintArg_struct _tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp5BB;_tmp5BB.tag=0U,_tmp5BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2B7);_tmp5BB;});void*_tmp14A[1U];_tmp14A[0]=& _tmp14C;({unsigned int _tmp6AF=e->loc;struct _dyneither_ptr _tmp6AE=({const char*_tmp14B="unknown pragma %s";_tag_dyneither(_tmp14B,sizeof(char),18U);});Cyc_Warn_warn(_tmp6AF,_tmp6AE,_tag_dyneither(_tmp14A,sizeof(void*),1U));});});}}}
return({struct _tuple16 _tmp5BC;_tmp5BC.f1=inflow,_tmp5BC.f2=_tmp125->zero;_tmp5BC;});case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp138)->f1).Wstring_c).tag){case 1U: _LL1A: _LL1B:
# 1079
 goto _LL1D;case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp138)->f1).Int_c).val).f2 == 0){_LL1C: _LL1D:
 return({struct _tuple16 _tmp5BD;_tmp5BD.f1=inflow,_tmp5BD.f2=_tmp125->zero;_tmp5BD;});}else{_LL1E: _LL1F:
 goto _LL21;}case 8U: _LL20: _LL21:
 goto _LL23;case 9U: _LL22: _LL23:
 goto _LL25;default: _LL28: _LL29:
# 1087
 goto _LL2B;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)){case 2U: _LL24: _LL25:
# 1084
 return({struct _tuple16 _tmp5BE;_tmp5BE.f1=inflow,_tmp5BE.f2=_tmp125->notzeroall;_tmp5BE;});case 1U: _LL34: _LL35:
# 1095
 return({struct _tuple16 _tmp5BF;_tmp5BF.f1=inflow,({void*_tmp6B0=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5BF.f2=_tmp6B0;});_tmp5BF;});case 3U: _LL36: _tmp2B8=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)->f1;_LL37:
# 1098
 _tmp2B9=_tmp2B8;goto _LL39;case 4U: _LL38: _tmp2B9=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)->f1;_LL39:
 _tmp2BA=_tmp2B9;goto _LL3B;case 5U: _LL3A: _tmp2BA=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)->f1;_LL3B: {
# 1102
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5C1;_tmp5C1.tag=0U,_tmp5C1.f1=_tmp2BA;_tmp5C1;});
return({struct _tuple16 _tmp5C0;_tmp5C0.f1=inflow,({void*_tmp6B2=({struct Cyc_Dict_Dict _tmp6B1=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp6B1,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));*_tmp14D=vdroot;_tmp14D;}));});_tmp5C0.f2=_tmp6B2;});_tmp5C0;});}default: _LL74: _LL75:
# 1682
 goto _LL77;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp138)->f1 == 0){_LL26: _LL27:
# 1086
 goto _LL29;}else{_LL62: _tmp2BC=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2BB=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL63:
# 1545
 _tmp2E3=_tmp2BC;goto _LL65;}case 18U: _LL2A: _LL2B:
# 1088
 goto _LL2D;case 17U: _LL2C: _LL2D:
 goto _LL2F;case 19U: _LL2E: _LL2F:
 goto _LL31;case 33U: _LL30: _LL31:
 goto _LL33;case 32U: _LL32: _LL33:
 return({struct _tuple16 _tmp5C2;_tmp5C2.f1=inflow,_tmp5C2.f2=_tmp125->unknown_all;_tmp5C2;});case 3U: _LL3C: _tmp2BE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2BD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL3D: {
# 1109
struct _tuple16 _tmp14E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp2BD);struct _tuple16 _tmp14F=_tmp14E;union Cyc_CfFlowInfo_FlowInfo _tmp151;void*_tmp150;_LL85: _tmp151=_tmp14F.f1;_tmp150=_tmp14F.f2;_LL86:;
return({struct _tuple16 _tmp5C3;_tmp5C3.f1=_tmp151,_tmp5C3.f2=_tmp150;_tmp5C3;});}case 5U: _LL3E: _tmp2BF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL3F: {
# 1113
struct Cyc_List_List _tmp152=({struct Cyc_List_List _tmp5C7;_tmp5C7.hd=_tmp2BF,_tmp5C7.tl=0;_tmp5C7;});
struct _tuple16 _tmp153=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp152);struct _tuple16 _tmp154=_tmp153;union Cyc_CfFlowInfo_FlowInfo _tmp161;_LL88: _tmp161=_tmp154.f1;_LL89:;{
struct _tuple18 _tmp155=Cyc_NewControlFlow_anal_Lexp(env,_tmp161,0,0,_tmp2BF);struct _tuple18 _tmp156=_tmp155;union Cyc_CfFlowInfo_AbsLVal _tmp160;_LL8B: _tmp160=_tmp156.f2;_LL8C:;{
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(_tmp2BF->topt));
if(iszt){
# 1120
struct _tuple16 _tmp157=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2BF);struct _tuple16 _tmp158=_tmp157;union Cyc_CfFlowInfo_FlowInfo _tmp15A;void*_tmp159;_LL8E: _tmp15A=_tmp158.f1;_tmp159=_tmp158.f2;_LL8F:;
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp15A,_tmp2BF,_tmp159);}
# 1123
{struct _tuple25 _tmp15B=({struct _tuple25 _tmp5C4;_tmp5C4.f1=_tmp160,_tmp5C4.f2=_tmp161;_tmp5C4;});struct _tuple25 _tmp15C=_tmp15B;struct Cyc_CfFlowInfo_Place*_tmp15F;struct Cyc_Dict_Dict _tmp15E;struct Cyc_List_List*_tmp15D;if(((_tmp15C.f1).PlaceL).tag == 1){if(((_tmp15C.f2).ReachableFL).tag == 2){_LL91: _tmp15F=((_tmp15C.f1).PlaceL).val;_tmp15E=(((_tmp15C.f2).ReachableFL).val).f1;_tmp15D=(((_tmp15C.f2).ReachableFL).val).f2;_LL92:
# 1125
({struct Cyc_List_List*_tmp6B3=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp15D,_tmp2BF);_tmp15D=_tmp6B3;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6B5=({struct Cyc_Dict_Dict _tmp6B4=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2BF->loc,_tmp15E,_tmp15F,_tmp125->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp6B4,_tmp15D);});_tmp161=_tmp6B5;});
# 1130
goto _LL90;}else{goto _LL93;}}else{_LL93: _LL94:
 goto _LL90;}_LL90:;}
# 1134
if(iszt)
return({struct _tuple16 _tmp5C5;_tmp5C5.f1=_tmp161,_tmp5C5.f2=_tmp125->notzeroall;_tmp5C5;});else{
return({struct _tuple16 _tmp5C6;_tmp5C6.f1=_tmp161,_tmp5C6.f2=_tmp125->unknown_all;_tmp5C6;});}};};}case 4U: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f2 != 0){_LL40: _tmp2C1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL41:
# 1139
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp162=0U;({unsigned int _tmp6B7=e->loc;struct _dyneither_ptr _tmp6B6=({const char*_tmp163="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp163,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6B7,_tmp6B6,_tag_dyneither(_tmp162,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5C8;({union Cyc_CfFlowInfo_FlowInfo _tmp6B8=Cyc_CfFlowInfo_BottomFL();_tmp5C8.f1=_tmp6B8;}),_tmp5C8.f2=_tmp125->unknown_all;_tmp5C8;});}{
# 1143
struct _tuple16 _tmp164=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C0);struct _tuple16 _tmp165=_tmp164;union Cyc_CfFlowInfo_FlowInfo _tmp170;void*_tmp16F;_LL96: _tmp170=_tmp165.f1;_tmp16F=_tmp165.f2;_LL97:;{
struct _tuple18 _tmp166=Cyc_NewControlFlow_anal_Lexp(env,_tmp170,0,0,_tmp2C1);struct _tuple18 _tmp167=_tmp166;union Cyc_CfFlowInfo_FlowInfo _tmp16E;union Cyc_CfFlowInfo_AbsLVal _tmp16D;_LL99: _tmp16E=_tmp167.f1;_tmp16D=_tmp167.f2;_LL9A:;
{union Cyc_CfFlowInfo_FlowInfo _tmp168=_tmp16E;struct Cyc_Dict_Dict _tmp16C;struct Cyc_List_List*_tmp16B;if((_tmp168.ReachableFL).tag == 2){_LL9C: _tmp16C=((_tmp168.ReachableFL).val).f1;_tmp16B=((_tmp168.ReachableFL).val).f2;_LL9D:
# 1147
{union Cyc_CfFlowInfo_AbsLVal _tmp169=_tmp16D;struct Cyc_CfFlowInfo_Place*_tmp16A;if((_tmp169.PlaceL).tag == 1){_LLA1: _tmp16A=(_tmp169.PlaceL).val;_LLA2:
# 1151
({struct Cyc_List_List*_tmp6B9=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp16B,_tmp2C1);_tmp16B=_tmp6B9;});
({struct Cyc_Dict_Dict _tmp6BA=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2C1->loc,_tmp16C,_tmp16A,_tmp125->unknown_all);_tmp16C=_tmp6BA;});
# 1154
({union Cyc_CfFlowInfo_FlowInfo _tmp6BB=Cyc_CfFlowInfo_ReachableFL(_tmp16C,_tmp16B);_tmp16E=_tmp6BB;});
# 1158
goto _LLA0;}else{_LLA3: _LLA4:
# 1161
 goto _LLA0;}_LLA0:;}
# 1163
goto _LL9B;}else{_LL9E: _LL9F:
 goto _LL9B;}_LL9B:;}
# 1166
return({struct _tuple16 _tmp5C9;_tmp5C9.f1=_tmp16E,_tmp5C9.f2=_tmp125->unknown_all;_tmp5C9;});};};}else{_LL42: _tmp2C3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL43:
# 1169
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp171=0U;({unsigned int _tmp6BD=e->loc;struct _dyneither_ptr _tmp6BC=({const char*_tmp172="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp172,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6BD,_tmp6BC,_tag_dyneither(_tmp171,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5CA;({union Cyc_CfFlowInfo_FlowInfo _tmp6BE=Cyc_CfFlowInfo_BottomFL();_tmp5CA.f1=_tmp6BE;}),_tmp5CA.f2=_tmp125->unknown_all;_tmp5CA;});}
# 1175
({union Cyc_CfFlowInfo_FlowInfo _tmp6C1=({struct Cyc_NewControlFlow_AnalEnv*_tmp6C0=env;union Cyc_CfFlowInfo_FlowInfo _tmp6BF=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6C0,_tmp6BF,({struct Cyc_Absyn_Exp*_tmp173[2U];_tmp173[0]=_tmp2C3,_tmp173[1]=_tmp2C2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp173,sizeof(struct Cyc_Absyn_Exp*),2U));}));});inflow=_tmp6C1;});{
# 1177
struct _tuple16 _tmp174=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C2);struct _tuple16 _tmp175=_tmp174;union Cyc_CfFlowInfo_FlowInfo _tmp17F;void*_tmp17E;_LLA6: _tmp17F=_tmp175.f1;_tmp17E=_tmp175.f2;_LLA7:;{
struct _tuple18 _tmp176=Cyc_NewControlFlow_anal_Lexp(env,_tmp17F,0,0,_tmp2C3);struct _tuple18 _tmp177=_tmp176;union Cyc_CfFlowInfo_FlowInfo _tmp17D;union Cyc_CfFlowInfo_AbsLVal _tmp17C;_LLA9: _tmp17D=_tmp177.f1;_tmp17C=_tmp177.f2;_LLAA:;{
struct _tuple16 _tmp178=Cyc_NewControlFlow_do_assign(_tmp125,env,_tmp17D,_tmp2C3,_tmp17C,_tmp2C2,_tmp17E,e->loc);struct _tuple16 _tmp179=_tmp178;union Cyc_CfFlowInfo_FlowInfo _tmp17B;void*_tmp17A;_LLAC: _tmp17B=_tmp179.f1;_tmp17A=_tmp179.f2;_LLAD:;
# 1183
return({struct _tuple16 _tmp5CB;_tmp5CB.f1=_tmp17B,_tmp5CB.f2=_tmp17A;_tmp5CB;});};};};}case 9U: _LL44: _tmp2C5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL45: {
# 1186
struct _tuple16 _tmp180=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2C5);struct _tuple16 _tmp181=_tmp180;union Cyc_CfFlowInfo_FlowInfo _tmp183;void*_tmp182;_LLAF: _tmp183=_tmp181.f1;_tmp182=_tmp181.f2;_LLB0:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp183,_tmp2C4);}case 11U: _LL46: _tmp2C6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL47: {
# 1190
struct _tuple16 _tmp184=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C6);struct _tuple16 _tmp185=_tmp184;union Cyc_CfFlowInfo_FlowInfo _tmp187;void*_tmp186;_LLB2: _tmp187=_tmp185.f1;_tmp186=_tmp185.f2;_LLB3:;
Cyc_NewControlFlow_use_Rval(env,_tmp2C6->loc,_tmp187,_tmp186);
return({struct _tuple16 _tmp5CC;({union Cyc_CfFlowInfo_FlowInfo _tmp6C3=Cyc_CfFlowInfo_BottomFL();_tmp5CC.f1=_tmp6C3;}),({void*_tmp6C2=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5CC.f2=_tmp6C2;});_tmp5CC;});}case 10U: _LL48: _tmp2C8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL49: {
# 1195
struct Cyc_List_List*_tmp188=_tmp2C7;
struct _tuple23 _tmp189=({
struct Cyc_NewControlFlow_AnalEnv*_tmp6C6=env;union Cyc_CfFlowInfo_FlowInfo _tmp6C5=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6C6,_tmp6C5,({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->hd=_tmp2C8,({struct Cyc_List_List*_tmp6C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp2C7);_tmp1B7->tl=_tmp6C4;});_tmp1B7;}),0,1);});
# 1196
struct _tuple23 _tmp18A=_tmp189;union Cyc_CfFlowInfo_FlowInfo _tmp1B6;struct Cyc_List_List*_tmp1B5;_LLB5: _tmp1B6=_tmp18A.f1;_tmp1B5=_tmp18A.f2;_LLB6:;{
# 1199
union Cyc_CfFlowInfo_FlowInfo _tmp18B=Cyc_NewControlFlow_use_Rval(env,_tmp2C8->loc,_tmp1B6,(void*)((struct Cyc_List_List*)_check_null(_tmp1B5))->hd);
_tmp1B5=_tmp1B5->tl;{
# 1202
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp18C=Cyc_Tcutil_compress((void*)_check_null(_tmp2C8->topt));void*_tmp18D=_tmp18C;void*_tmp19F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18D)->tag == 5U){_LLB8: _tmp19F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18D)->f1).elt_typ;_LLB9:
# 1209
{void*_tmp18E=Cyc_Tcutil_compress(_tmp19F);void*_tmp18F=_tmp18E;struct Cyc_List_List*_tmp19C;struct Cyc_List_List*_tmp19B;struct Cyc_List_List*_tmp19A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->tag == 9U){_LLBD: _tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->f1).attributes;_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->f1).requires_relns;_tmp19A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->f1).ensures_relns;_LLBE:
# 1211
 requires=_tmp19B;
ensures=_tmp19A;
for(0;_tmp19C != 0;_tmp19C=_tmp19C->tl){
# 1215
void*_tmp190=(void*)_tmp19C->hd;void*_tmp191=_tmp190;int _tmp197;int _tmp196;int _tmp195;switch(*((int*)_tmp191)){case 20U: _LLC2: _tmp195=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp191)->f1;_LLC3:
# 1217
({struct Cyc_List_List*_tmp6C7=({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=(void*)_tmp195,_tmp192->tl=init_params;_tmp192;});init_params=_tmp6C7;});goto _LLC1;case 21U: _LLC4: _tmp196=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp191)->f1;_LLC5:
# 1219
({struct Cyc_List_List*_tmp6C8=({struct Cyc_List_List*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=(void*)_tmp196,_tmp193->tl=nolive_unique_params;_tmp193;});nolive_unique_params=_tmp6C8;});
goto _LLC1;case 22U: _LLC6: _tmp197=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp191)->f1;_LLC7:
# 1223
({struct Cyc_List_List*_tmp6C9=({struct Cyc_List_List*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->hd=(void*)_tmp197,_tmp194->tl=noconsume_params;_tmp194;});noconsume_params=_tmp6C9;});
goto _LLC1;default: _LLC8: _LLC9:
 goto _LLC1;}_LLC1:;}
# 1227
goto _LLBC;}else{_LLBF: _LLC0:
({void*_tmp198=0U;({struct _dyneither_ptr _tmp6CA=({const char*_tmp199="anal_Rexp: bad function type";_tag_dyneither(_tmp199,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6CA,_tag_dyneither(_tmp198,sizeof(void*),0U));});});}_LLBC:;}
# 1230
goto _LLB7;}else{_LLBA: _LLBB:
({void*_tmp19D=0U;({struct _dyneither_ptr _tmp6CB=({const char*_tmp19E="anal_Rexp: bad function type";_tag_dyneither(_tmp19E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6CB,_tag_dyneither(_tmp19D,sizeof(void*),0U));});});}_LLB7:;}
# 1233
{int i=1;for(0;_tmp1B5 != 0;(_tmp1B5=_tmp1B5->tl,_tmp2C7=((struct Cyc_List_List*)_check_null(_tmp2C7))->tl,++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1A0=_tmp1B6;struct Cyc_Dict_Dict _tmp1B4;if((_tmp1A0.BottomFL).tag == 1){_LLCB: _LLCC:
 goto _LLCA;}else{_LLCD: _tmp1B4=((_tmp1A0.ReachableFL).val).f1;_LLCE: {
# 1238
struct _tuple15 _tmp1A1=Cyc_CfFlowInfo_unname_rval((void*)_tmp1B5->hd);struct _tuple15 _tmp1A2=_tmp1A1;void*_tmp1B3;_LLD0: _tmp1B3=_tmp1A2.f1;_LLD1:;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1B4,(void*)_tmp1B5->hd)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1B5->hd))
({void*_tmp1A3=0U;({unsigned int _tmp6CD=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C7))->hd)->loc;struct _dyneither_ptr _tmp6CC=({const char*_tmp1A4="expression may not be initialized";_tag_dyneither(_tmp1A4,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6CD,_tmp6CC,_tag_dyneither(_tmp1A3,sizeof(void*),0U));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1A5=_tmp18B;struct Cyc_Dict_Dict _tmp1B2;struct Cyc_List_List*_tmp1B1;if((_tmp1A5.BottomFL).tag == 1){_LLD3: _LLD4:
 goto _LLD2;}else{_LLD5: _tmp1B2=((_tmp1A5.ReachableFL).val).f1;_tmp1B1=((_tmp1A5.ReachableFL).val).f2;_LLD6: {
# 1246
struct Cyc_Dict_Dict _tmp1A6=Cyc_CfFlowInfo_escape_deref(_tmp125,_tmp1B2,(void*)_tmp1B5->hd);
{void*_tmp1A7=(void*)_tmp1B5->hd;void*_tmp1A8=_tmp1A7;struct Cyc_CfFlowInfo_Place*_tmp1B0;switch(*((int*)_tmp1A8)){case 4U: _LLD8: _tmp1B0=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8)->f1;_LLD9:
# 1249
{void*_tmp1A9=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C7))->hd)->topt));void*_tmp1AA=_tmp1A9;void*_tmp1AD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->tag == 5U){_LLDF: _tmp1AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->f1).elt_typ;_LLE0:
# 1251
({struct Cyc_Dict_Dict _tmp6D2=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6D1=_tmp125;unsigned int _tmp6D0=((struct Cyc_Absyn_Exp*)_tmp2C7->hd)->loc;struct Cyc_Dict_Dict _tmp6CF=_tmp1A6;struct Cyc_CfFlowInfo_Place*_tmp6CE=_tmp1B0;Cyc_CfFlowInfo_assign_place(_tmp6D1,_tmp6D0,_tmp6CF,_tmp6CE,
Cyc_CfFlowInfo_typ_to_absrval(_tmp125,_tmp1AD,0,_tmp125->esc_all));});
# 1251
_tmp1A6=_tmp6D2;});
# 1255
goto _LLDE;}else{_LLE1: _LLE2:
({void*_tmp1AB=0U;({struct _dyneither_ptr _tmp6D3=({const char*_tmp1AC="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1AC,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6D3,_tag_dyneither(_tmp1AB,sizeof(void*),0U));});});}_LLDE:;}
# 1258
goto _LLD7;case 5U: _LLDA: _LLDB:
# 1260
({void*_tmp1AE=0U;({struct _dyneither_ptr _tmp6D4=({const char*_tmp1AF="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1AF,sizeof(char),68U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6D4,_tag_dyneither(_tmp1AE,sizeof(void*),0U));});});default: _LLDC: _LLDD:
# 1262
 goto _LLD7;}_LLD7:;}
# 1264
({union Cyc_CfFlowInfo_FlowInfo _tmp6D5=Cyc_CfFlowInfo_ReachableFL(_tmp1A6,_tmp1B1);_tmp18B=_tmp6D5;});
goto _LLD2;}}_LLD2:;}
# 1267
goto _LLCA;}}_LLCA:;}else{
# 1270
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1275
({union Cyc_CfFlowInfo_FlowInfo _tmp6DA=({struct Cyc_NewControlFlow_AnalEnv*_tmp6D9=env;unsigned int _tmp6D8=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C7))->hd)->loc;void*_tmp6D7=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2C7->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp6D6=_tmp18B;Cyc_NewControlFlow_use_nounique_Rval(_tmp6D9,_tmp6D8,_tmp6D7,_tmp6D6,(void*)_tmp1B5->hd);});_tmp18B=_tmp6DA;});else{
# 1285
({union Cyc_CfFlowInfo_FlowInfo _tmp6DB=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2C7))->hd)->loc,_tmp18B,(void*)_tmp1B5->hd);_tmp18B=_tmp6DB;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2C7->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp6DC=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp18B,(struct Cyc_Absyn_Exp*)_tmp2C7->hd,((struct Cyc_Absyn_Exp*)_tmp2C7->hd)->loc,(void*)_tmp1B5->hd);_tmp18B=_tmp6DC;});}}}}
# 1292
Cyc_NewControlFlow_check_fn_requires(env,_tmp18B,_tmp188,requires,e->loc);
# 1295
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp2C8->topt)))
return({struct _tuple16 _tmp5CD;({union Cyc_CfFlowInfo_FlowInfo _tmp6DE=Cyc_CfFlowInfo_BottomFL();_tmp5CD.f1=_tmp6DE;}),({void*_tmp6DD=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5CD.f2=_tmp6DD;});_tmp5CD;});else{
# 1298
return({struct _tuple16 _tmp5CE;_tmp5CE.f1=_tmp18B,({void*_tmp6DF=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5CE.f2=_tmp6DF;});_tmp5CE;});}};};}case 34U: _LL4A: _tmp2CD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).is_calloc;_tmp2CC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).rgn;_tmp2CB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).elt_type;_tmp2CA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).num_elts;_tmp2C9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).fat_result;_LL4B: {
# 1301
void*place_val;
if(_tmp2C9)place_val=_tmp125->notzeroall;else{
if(_tmp2CD)({void*_tmp6E0=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,*((void**)_check_null(_tmp2CB)),0,_tmp125->zero);place_val=_tmp6E0;});else{
({void*_tmp6E1=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,*((void**)_check_null(_tmp2CB)),0,_tmp125->unknown_none);place_val=_tmp6E1;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp2CC != 0){
struct _tuple23 _tmp1B8=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E3=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E2=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6E3,_tmp6E2,({struct Cyc_Absyn_Exp*_tmp1BC[2U];_tmp1BC[0]=_tmp2CC,_tmp1BC[1]=_tmp2CA;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1BC,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1B9=_tmp1B8;union Cyc_CfFlowInfo_FlowInfo _tmp1BB;struct Cyc_List_List*_tmp1BA;_LLE4: _tmp1BB=_tmp1B9.f1;_tmp1BA=_tmp1B9.f2;_LLE5:;
outflow=_tmp1BB;}else{
# 1310
struct _tuple16 _tmp1BD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2CA);struct _tuple16 _tmp1BE=_tmp1BD;union Cyc_CfFlowInfo_FlowInfo _tmp1BF;_LLE7: _tmp1BF=_tmp1BE.f1;_LLE8:;
outflow=_tmp1BF;}
# 1315
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp5CF;_tmp5CF.f1=outflow,({void*_tmp6E4=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->tag=5U,_tmp1C0->f1=place_val;_tmp1C0;});_tmp5CF.f2=_tmp6E4;});_tmp5CF;});else{
# 1319
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->tag=1U,_tmp1C6->f1=_tmp2CA,_tmp1C6->f2=(void*)_check_null(e->topt);_tmp1C6;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->root=root,_tmp1C5->path=0;_tmp1C5;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->tag=4U,_tmp1C4->f1=place;_tmp1C4;});
union Cyc_CfFlowInfo_FlowInfo _tmp1C1=outflow;struct Cyc_Dict_Dict _tmp1C3;struct Cyc_List_List*_tmp1C2;if((_tmp1C1.BottomFL).tag == 1){_LLEA: _LLEB:
 return({struct _tuple16 _tmp5D0;_tmp5D0.f1=outflow,_tmp5D0.f2=rval;_tmp5D0;});}else{_LLEC: _tmp1C3=((_tmp1C1.ReachableFL).val).f1;_tmp1C2=((_tmp1C1.ReachableFL).val).f2;_LLED:
# 1325
 return({struct _tuple16 _tmp5D1;({union Cyc_CfFlowInfo_FlowInfo _tmp6E6=({struct Cyc_Dict_Dict _tmp6E5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1C3,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6E5,_tmp1C2);});_tmp5D1.f1=_tmp6E6;}),_tmp5D1.f2=rval;_tmp5D1;});}_LLE9:;}};}case 35U: _LL4C: _tmp2CF=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2CE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL4D: {
# 1330
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1336
struct _tuple23 _tmp1C7=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E8=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E7=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6E8,_tmp6E7,({struct Cyc_Absyn_Exp*_tmp1DC[2U];_tmp1DC[0]=_tmp2CF,_tmp1DC[1]=_tmp2CE;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1DC,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,0);});struct _tuple23 _tmp1C8=_tmp1C7;union Cyc_CfFlowInfo_FlowInfo _tmp1DB;struct Cyc_List_List*_tmp1DA;_LLEF: _tmp1DB=_tmp1C8.f1;_tmp1DA=_tmp1C8.f2;_LLF0:;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1DA))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1DA->tl))->hd;
outflow=_tmp1DB;{
# 1341
struct _tuple18 _tmp1C9=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2CF);struct _tuple18 _tmp1CA=_tmp1C9;union Cyc_CfFlowInfo_AbsLVal _tmp1D9;_LLF2: _tmp1D9=_tmp1CA.f2;_LLF3:;{
struct _tuple18 _tmp1CB=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2CE);struct _tuple18 _tmp1CC=_tmp1CB;union Cyc_CfFlowInfo_AbsLVal _tmp1D8;_LLF5: _tmp1D8=_tmp1CC.f2;_LLF6:;
{union Cyc_CfFlowInfo_FlowInfo _tmp1CD=outflow;struct Cyc_Dict_Dict _tmp1D7;struct Cyc_List_List*_tmp1D6;if((_tmp1CD.ReachableFL).tag == 2){_LLF8: _tmp1D7=((_tmp1CD.ReachableFL).val).f1;_tmp1D6=((_tmp1CD.ReachableFL).val).f2;_LLF9:
# 1345
{union Cyc_CfFlowInfo_AbsLVal _tmp1CE=_tmp1D9;struct Cyc_CfFlowInfo_Place*_tmp1D1;if((_tmp1CE.PlaceL).tag == 1){_LLFD: _tmp1D1=(_tmp1CE.PlaceL).val;_LLFE:
# 1347
({struct Cyc_Dict_Dict _tmp6E9=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2CF->loc,_tmp1D7,_tmp1D1,right_rval);_tmp1D7=_tmp6E9;});
goto _LLFC;}else{_LLFF: _LL100:
# 1353
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp1D7,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp1CF=0U;({unsigned int _tmp6EB=_tmp2CE->loc;struct _dyneither_ptr _tmp6EA=({const char*_tmp1D0="expression may not be fully initialized";_tag_dyneither(_tmp1D0,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6EB,_tmp6EA,_tag_dyneither(_tmp1CF,sizeof(void*),0U));});});
goto _LLFC;}_LLFC:;}
# 1357
{union Cyc_CfFlowInfo_AbsLVal _tmp1D2=_tmp1D8;struct Cyc_CfFlowInfo_Place*_tmp1D5;if((_tmp1D2.PlaceL).tag == 1){_LL102: _tmp1D5=(_tmp1D2.PlaceL).val;_LL103:
# 1359
({struct Cyc_Dict_Dict _tmp6EC=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2CE->loc,_tmp1D7,_tmp1D5,left_rval);_tmp1D7=_tmp6EC;});
goto _LL101;}else{_LL104: _LL105:
# 1362
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp1D7,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp1D3=0U;({unsigned int _tmp6EE=_tmp2CF->loc;struct _dyneither_ptr _tmp6ED=({const char*_tmp1D4="expression may not be fully initialized";_tag_dyneither(_tmp1D4,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6EE,_tmp6ED,_tag_dyneither(_tmp1D3,sizeof(void*),0U));});});
goto _LL101;}_LL101:;}
# 1367
({struct Cyc_List_List*_tmp6EF=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1D6,_tmp2CF);_tmp1D6=_tmp6EF;});
({struct Cyc_List_List*_tmp6F0=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1D6,_tmp2CE);_tmp1D6=_tmp6F0;});
# 1370
({union Cyc_CfFlowInfo_FlowInfo _tmp6F1=Cyc_CfFlowInfo_ReachableFL(_tmp1D7,_tmp1D6);outflow=_tmp6F1;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LLF7;}else{_LLFA: _LLFB:
 goto _LLF7;}_LLF7:;}
# 1377
return({struct _tuple16 _tmp5D2;_tmp5D2.f1=outflow,_tmp5D2.f2=_tmp125->unknown_all;_tmp5D2;});};};}case 16U: _LL4E: _tmp2D1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2D0=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL4F: {
# 1380
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp2D1 != 0){
struct _tuple23 _tmp1DD=({struct Cyc_NewControlFlow_AnalEnv*_tmp6F3=env;union Cyc_CfFlowInfo_FlowInfo _tmp6F2=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6F3,_tmp6F2,({struct Cyc_Absyn_Exp*_tmp1E1[2U];_tmp1E1[0]=_tmp2D1,_tmp1E1[1]=_tmp2D0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1E1,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1DE=_tmp1DD;union Cyc_CfFlowInfo_FlowInfo _tmp1E0;struct Cyc_List_List*_tmp1DF;_LL107: _tmp1E0=_tmp1DE.f1;_tmp1DF=_tmp1DE.f2;_LL108:;
outflow=_tmp1E0;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp1DF))->tl))->hd;}else{
# 1387
struct _tuple16 _tmp1E2=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2D0);struct _tuple16 _tmp1E3=_tmp1E2;union Cyc_CfFlowInfo_FlowInfo _tmp1E5;void*_tmp1E4;_LL10A: _tmp1E5=_tmp1E3.f1;_tmp1E4=_tmp1E3.f2;_LL10B:;
outflow=_tmp1E5;
place_val=_tmp1E4;}
# 1392
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp5D3;_tmp5D3.f1=outflow,({void*_tmp6F4=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->tag=5U,_tmp1E6->f1=place_val;_tmp1E6;});_tmp5D3.f2=_tmp6F4;});_tmp5D3;});else{
# 1396
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->tag=1U,_tmp1EC->f1=_tmp2D0,_tmp1EC->f2=(void*)_check_null(e->topt);_tmp1EC;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->root=root,_tmp1EB->path=0;_tmp1EB;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->tag=4U,_tmp1EA->f1=place;_tmp1EA;});
union Cyc_CfFlowInfo_FlowInfo _tmp1E7=outflow;struct Cyc_Dict_Dict _tmp1E9;struct Cyc_List_List*_tmp1E8;if((_tmp1E7.BottomFL).tag == 1){_LL10D: _LL10E:
 return({struct _tuple16 _tmp5D4;_tmp5D4.f1=outflow,_tmp5D4.f2=rval;_tmp5D4;});}else{_LL10F: _tmp1E9=((_tmp1E7.ReachableFL).val).f1;_tmp1E8=((_tmp1E7.ReachableFL).val).f2;_LL110:
# 1402
 return({struct _tuple16 _tmp5D5;({union Cyc_CfFlowInfo_FlowInfo _tmp6F6=({struct Cyc_Dict_Dict _tmp6F5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1E9,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6F5,_tmp1E8);});_tmp5D5.f1=_tmp6F6;}),_tmp5D5.f2=rval;_tmp5D5;});}_LL10C:;}}case 15U: _LL50: _tmp2D2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL51: {
# 1407
struct _tuple18 _tmp1ED=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp2D2);struct _tuple18 _tmp1EE=_tmp1ED;union Cyc_CfFlowInfo_FlowInfo _tmp1F3;union Cyc_CfFlowInfo_AbsLVal _tmp1F2;_LL112: _tmp1F3=_tmp1EE.f1;_tmp1F2=_tmp1EE.f2;_LL113:;{
union Cyc_CfFlowInfo_AbsLVal _tmp1EF=_tmp1F2;struct Cyc_CfFlowInfo_Place*_tmp1F1;if((_tmp1EF.UnknownL).tag == 2){_LL115: _LL116:
 return({struct _tuple16 _tmp5D6;_tmp5D6.f1=_tmp1F3,_tmp5D6.f2=_tmp125->notzeroall;_tmp5D6;});}else{_LL117: _tmp1F1=(_tmp1EF.PlaceL).val;_LL118:
 return({struct _tuple16 _tmp5D7;_tmp5D7.f1=_tmp1F3,({void*_tmp6F7=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->tag=4U,_tmp1F0->f1=_tmp1F1;_tmp1F0;});_tmp5D7.f2=_tmp6F7;});_tmp5D7;});}_LL114:;};}case 20U: _LL52: _tmp2D3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL53: {
# 1414
struct _tuple16 _tmp1F4=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D3);struct _tuple16 _tmp1F5=_tmp1F4;union Cyc_CfFlowInfo_FlowInfo _tmp1F7;void*_tmp1F6;_LL11A: _tmp1F7=_tmp1F5.f1;_tmp1F6=_tmp1F5.f2;_LL11B:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1F7,_tmp2D3,_tmp1F6);}case 21U: _LL54: _tmp2D5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2D4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL55: {
# 1418
struct _tuple16 _tmp1F8=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D5);struct _tuple16 _tmp1F9=_tmp1F8;union Cyc_CfFlowInfo_FlowInfo _tmp209;void*_tmp208;_LL11D: _tmp209=_tmp1F9.f1;_tmp208=_tmp1F9.f2;_LL11E:;{
void*_tmp1FA=(void*)_check_null(_tmp2D5->topt);
# 1421
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp1FA))
return({struct _tuple16 _tmp5D8;_tmp5D8.f1=_tmp209,({void*_tmp6F8=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5D8.f2=_tmp6F8;});_tmp5D8;});
if(Cyc_Absyn_is_require_union_type(_tmp1FA))
Cyc_NewControlFlow_check_union_requires(_tmp2D5->loc,_tmp1FA,_tmp2D4,_tmp209);{
struct _tuple15 _tmp1FB=Cyc_CfFlowInfo_unname_rval(_tmp208);struct _tuple15 _tmp1FC=_tmp1FB;void*_tmp207;_LL120: _tmp207=_tmp1FC.f1;_LL121:;{
void*_tmp1FD=_tmp207;int _tmp206;int _tmp205;struct _dyneither_ptr _tmp204;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FD)->tag == 6U){_LL123: _tmp206=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FD)->f1).is_union;_tmp205=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FD)->f1).fieldnum;_tmp204=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1FD)->f2;_LL124: {
# 1428
int _tmp1FE=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2D5->topt),_tmp2D4);
if((_tmp206  && _tmp205 != - 1) && _tmp205 != _tmp1FE)
return({struct _tuple16 _tmp5D9;_tmp5D9.f1=_tmp209,({void*_tmp6F9=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),1,_tmp125->unknown_none);_tmp5D9.f2=_tmp6F9;});_tmp5D9;});
return({struct _tuple16 _tmp5DA;_tmp5DA.f1=_tmp209,_tmp5DA.f2=*((void**)_check_dyneither_subscript(_tmp204,sizeof(void*),_tmp1FE));_tmp5DA;});}}else{_LL125: _LL126:
# 1433
({void*_tmp1FF=0U;({struct Cyc___cycFILE*_tmp6FB=Cyc_stderr;struct _dyneither_ptr _tmp6FA=({const char*_tmp200="the bad rexp is :";_tag_dyneither(_tmp200,sizeof(char),18U);});Cyc_fprintf(_tmp6FB,_tmp6FA,_tag_dyneither(_tmp1FF,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(_tmp207);
({struct Cyc_String_pa_PrintArg_struct _tmp203=({struct Cyc_String_pa_PrintArg_struct _tmp5DB;_tmp5DB.tag=0U,({struct _dyneither_ptr _tmp6FC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5DB.f1=_tmp6FC;});_tmp5DB;});void*_tmp201[1U];_tmp201[0]=& _tmp203;({struct _dyneither_ptr _tmp6FD=({const char*_tmp202="\nanal_Rexp: AggrMember: %s";_tag_dyneither(_tmp202,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6FD,_tag_dyneither(_tmp201,sizeof(void*),1U));});});}_LL122:;};};};}case 38U: _LL56: _tmp2D7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2D6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL57: {
# 1441
struct _tuple16 _tmp20A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D7);struct _tuple16 _tmp20B=_tmp20A;union Cyc_CfFlowInfo_FlowInfo _tmp218;void*_tmp217;_LL128: _tmp218=_tmp20B.f1;_tmp217=_tmp20B.f2;_LL129:;
# 1443
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp2D7->topt)))
return({struct _tuple16 _tmp5DC;_tmp5DC.f1=_tmp218,_tmp5DC.f2=_tmp125->unknown_all;_tmp5DC;});{
struct _tuple15 _tmp20C=Cyc_CfFlowInfo_unname_rval(_tmp217);struct _tuple15 _tmp20D=_tmp20C;void*_tmp216;_LL12B: _tmp216=_tmp20D.f1;_LL12C:;{
void*_tmp20E=_tmp216;int _tmp215;int _tmp214;struct _dyneither_ptr _tmp213;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20E)->tag == 6U){_LL12E: _tmp215=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20E)->f1).is_union;_tmp214=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20E)->f1).fieldnum;_tmp213=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp20E)->f2;_LL12F: {
# 1448
int _tmp20F=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2D7->topt),_tmp2D6);
if(_tmp215  && _tmp214 != - 1){
if(_tmp214 != _tmp20F)
return({struct _tuple16 _tmp5DD;_tmp5DD.f1=_tmp218,_tmp5DD.f2=_tmp125->zero;_tmp5DD;});else{
# 1453
return({struct _tuple16 _tmp5DE;_tmp5DE.f1=_tmp218,_tmp5DE.f2=_tmp125->notzeroall;_tmp5DE;});}}else{
# 1455
return({struct _tuple16 _tmp5DF;_tmp5DF.f1=_tmp218,_tmp5DF.f2=_tmp125->unknown_all;_tmp5DF;});}}}else{_LL130: _LL131:
# 1457
({struct Cyc_String_pa_PrintArg_struct _tmp212=({struct Cyc_String_pa_PrintArg_struct _tmp5E0;_tmp5E0.tag=0U,({struct _dyneither_ptr _tmp6FE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5E0.f1=_tmp6FE;});_tmp5E0;});void*_tmp210[1U];_tmp210[0]=& _tmp212;({struct _dyneither_ptr _tmp6FF=({const char*_tmp211="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp211,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6FF,_tag_dyneither(_tmp210,sizeof(void*),1U));});});}_LL12D:;};};}case 22U: _LL58: _tmp2D9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2D8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL59: {
# 1461
struct _tuple16 _tmp219=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D9);struct _tuple16 _tmp21A=_tmp219;union Cyc_CfFlowInfo_FlowInfo _tmp22F;void*_tmp22E;_LL133: _tmp22F=_tmp21A.f1;_tmp22E=_tmp21A.f2;_LL134:;{
# 1464
struct _tuple16 _tmp21B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp22F,_tmp2D9,_tmp22E);struct _tuple16 _tmp21C=_tmp21B;union Cyc_CfFlowInfo_FlowInfo _tmp22D;void*_tmp22C;_LL136: _tmp22D=_tmp21C.f1;_tmp22C=_tmp21C.f2;_LL137:;{
# 1467
void*_tmp21D=Cyc_Tcutil_compress((void*)_check_null(_tmp2D9->topt));void*_tmp21E=_tmp21D;void*_tmp22B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21E)->tag == 5U){_LL139: _tmp22B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21E)->f1).elt_typ;_LL13A:
# 1469
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp22B))
# 1471
return({struct _tuple16 _tmp5E1;_tmp5E1.f1=_tmp22D,({void*_tmp700=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5E1.f2=_tmp700;});_tmp5E1;});
# 1473
if(Cyc_Absyn_is_require_union_type(_tmp22B))
Cyc_NewControlFlow_check_union_requires(_tmp2D9->loc,_tmp22B,_tmp2D8,_tmp22F);{
# 1476
struct _tuple15 _tmp21F=Cyc_CfFlowInfo_unname_rval(_tmp22C);struct _tuple15 _tmp220=_tmp21F;void*_tmp228;_LL13E: _tmp228=_tmp220.f1;_LL13F:;{
void*_tmp221=_tmp228;int _tmp227;int _tmp226;struct _dyneither_ptr _tmp225;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp221)->tag == 6U){_LL141: _tmp227=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp221)->f1).is_union;_tmp226=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp221)->f1).fieldnum;_tmp225=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp221)->f2;_LL142: {
# 1479
int _tmp222=Cyc_CfFlowInfo_get_field_index(_tmp22B,_tmp2D8);
if((_tmp227  && _tmp226 != - 1) && _tmp226 != _tmp222)
return({struct _tuple16 _tmp5E2;_tmp5E2.f1=_tmp22D,({void*_tmp701=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),1,_tmp125->unknown_none);_tmp5E2.f2=_tmp701;});_tmp5E2;});
return({struct _tuple16 _tmp5E3;_tmp5E3.f1=_tmp22D,_tmp5E3.f2=*((void**)_check_dyneither_subscript(_tmp225,sizeof(void*),_tmp222));_tmp5E3;});}}else{_LL143: _LL144:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp702=({const char*_tmp224="anal_Rexp: AggrArrow";_tag_dyneither(_tmp224,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp702,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}_LL140:;};};}else{_LL13B: _LL13C:
# 1485
({void*_tmp229=0U;({struct _dyneither_ptr _tmp703=({const char*_tmp22A="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp22A,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp703,_tag_dyneither(_tmp229,sizeof(void*),0U));});});}_LL138:;};};}case 6U: _LL5A: _tmp2DC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2DB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_tmp2DA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL5B: {
# 1489
struct _tuple19 _tmp230=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DC);struct _tuple19 _tmp231=_tmp230;union Cyc_CfFlowInfo_FlowInfo _tmp235;union Cyc_CfFlowInfo_FlowInfo _tmp234;_LL146: _tmp235=_tmp231.f1;_tmp234=_tmp231.f2;_LL147:;{
struct _tuple16 _tmp232=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp235,_tmp2DB);
struct _tuple16 _tmp233=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp234,_tmp2DA);
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp125,_tmp232,_tmp233);};}case 7U: _LL5C: _tmp2DE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2DD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL5D: {
# 1495
struct _tuple19 _tmp236=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2DE);struct _tuple19 _tmp237=_tmp236;union Cyc_CfFlowInfo_FlowInfo _tmp23F;union Cyc_CfFlowInfo_FlowInfo _tmp23E;_LL149: _tmp23F=_tmp237.f1;_tmp23E=_tmp237.f2;_LL14A:;{
struct _tuple16 _tmp238=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp23F,_tmp2DD);struct _tuple16 _tmp239=_tmp238;union Cyc_CfFlowInfo_FlowInfo _tmp23D;void*_tmp23C;_LL14C: _tmp23D=_tmp239.f1;_tmp23C=_tmp239.f2;_LL14D:;{
struct _tuple16 _tmp23A=({struct _tuple16 _tmp5E5;_tmp5E5.f1=_tmp23D,_tmp5E5.f2=_tmp23C;_tmp5E5;});
struct _tuple16 _tmp23B=({struct _tuple16 _tmp5E4;_tmp5E4.f1=_tmp23E,_tmp5E4.f2=_tmp125->zero;_tmp5E4;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp125,_tmp23A,_tmp23B);};};}case 8U: _LL5E: _tmp2E0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2DF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL5F: {
# 1502
struct _tuple19 _tmp240=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2E0);struct _tuple19 _tmp241=_tmp240;union Cyc_CfFlowInfo_FlowInfo _tmp249;union Cyc_CfFlowInfo_FlowInfo _tmp248;_LL14F: _tmp249=_tmp241.f1;_tmp248=_tmp241.f2;_LL150:;{
struct _tuple16 _tmp242=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp248,_tmp2DF);struct _tuple16 _tmp243=_tmp242;union Cyc_CfFlowInfo_FlowInfo _tmp247;void*_tmp246;_LL152: _tmp247=_tmp243.f1;_tmp246=_tmp243.f2;_LL153:;{
struct _tuple16 _tmp244=({struct _tuple16 _tmp5E7;_tmp5E7.f1=_tmp247,_tmp5E7.f2=_tmp246;_tmp5E7;});
struct _tuple16 _tmp245=({struct _tuple16 _tmp5E6;_tmp5E6.f1=_tmp249,_tmp5E6.f2=_tmp125->notzeroall;_tmp5E6;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp125,_tmp244,_tmp245);};};}case 23U: _LL60: _tmp2E2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2E1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL61: {
# 1509
struct _tuple23 _tmp24A=({struct Cyc_NewControlFlow_AnalEnv*_tmp705=env;union Cyc_CfFlowInfo_FlowInfo _tmp704=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp705,_tmp704,({struct Cyc_Absyn_Exp*_tmp266[2U];_tmp266[0]=_tmp2E2,_tmp266[1]=_tmp2E1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp266,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp24B=_tmp24A;union Cyc_CfFlowInfo_FlowInfo _tmp265;struct Cyc_List_List*_tmp264;_LL155: _tmp265=_tmp24B.f1;_tmp264=_tmp24B.f2;_LL156:;{
# 1513
union Cyc_CfFlowInfo_FlowInfo _tmp24C=_tmp265;
{union Cyc_CfFlowInfo_FlowInfo _tmp24D=_tmp265;struct Cyc_Dict_Dict _tmp252;struct Cyc_List_List*_tmp251;if((_tmp24D.ReachableFL).tag == 2){_LL158: _tmp252=((_tmp24D.ReachableFL).val).f1;_tmp251=((_tmp24D.ReachableFL).val).f2;_LL159:
# 1518
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp252,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp264))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp24E=0U;({unsigned int _tmp707=_tmp2E1->loc;struct _dyneither_ptr _tmp706=({const char*_tmp24F="expression may not be initialized";_tag_dyneither(_tmp24F,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp707,_tmp706,_tag_dyneither(_tmp24E,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp250=Cyc_NewControlFlow_add_subscript_reln(_tmp251,_tmp2E2,_tmp2E1);
if(_tmp251 != _tmp250)
({union Cyc_CfFlowInfo_FlowInfo _tmp708=Cyc_CfFlowInfo_ReachableFL(_tmp252,_tmp250);_tmp24C=_tmp708;});
goto _LL157;};}else{_LL15A: _LL15B:
 goto _LL157;}_LL157:;}{
# 1526
void*_tmp253=Cyc_Tcutil_compress((void*)_check_null(_tmp2E2->topt));void*_tmp254=_tmp253;struct Cyc_List_List*_tmp263;switch(*((int*)_tmp254)){case 10U: _LL15D: _tmp263=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp254)->f1;_LL15E: {
# 1528
struct _tuple15 _tmp255=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(_tmp264))->hd);struct _tuple15 _tmp256=_tmp255;void*_tmp25B;_LL164: _tmp25B=_tmp256.f1;_LL165:;{
void*_tmp257=_tmp25B;struct _dyneither_ptr _tmp25A;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp257)->tag == 6U){_LL167: _tmp25A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp257)->f2;_LL168: {
# 1531
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp2E1)).f1;
return({struct _tuple16 _tmp5E8;_tmp5E8.f1=_tmp24C,_tmp5E8.f2=*((void**)_check_dyneither_subscript(_tmp25A,sizeof(void*),(int)i));_tmp5E8;});}}else{_LL169: _LL16A:
({void*_tmp258=0U;({struct _dyneither_ptr _tmp709=({const char*_tmp259="anal_Rexp: Subscript";_tag_dyneither(_tmp259,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp709,_tag_dyneither(_tmp258,sizeof(void*),0U));});});}_LL166:;};}case 5U: _LL15F: _LL160: {
# 1536
struct _tuple16 _tmp25C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp265,_tmp2E2,(void*)((struct Cyc_List_List*)_check_null(_tmp264))->hd);struct _tuple16 _tmp25D=_tmp25C;union Cyc_CfFlowInfo_FlowInfo _tmp260;void*_tmp25F;_LL16C: _tmp260=_tmp25D.f1;_tmp25F=_tmp25D.f2;_LL16D:;{
union Cyc_CfFlowInfo_FlowInfo _tmp25E=_tmp260;if((_tmp25E.BottomFL).tag == 1){_LL16F: _LL170:
 return({struct _tuple16 _tmp5E9;_tmp5E9.f1=_tmp260,_tmp5E9.f2=_tmp25F;_tmp5E9;});}else{_LL171: _LL172:
 return({struct _tuple16 _tmp5EA;_tmp5EA.f1=_tmp24C,_tmp5EA.f2=_tmp25F;_tmp5EA;});}_LL16E:;};}default: _LL161: _LL162:
# 1541
({void*_tmp261=0U;({struct _dyneither_ptr _tmp70A=({const char*_tmp262="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp262,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp70A,_tag_dyneither(_tmp261,sizeof(void*),0U));});});}_LL15C:;};};}case 24U: _LL64: _tmp2E3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL65: {
# 1547
struct _tuple23 _tmp267=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp2E3,1,1);struct _tuple23 _tmp268=_tmp267;union Cyc_CfFlowInfo_FlowInfo _tmp26E;struct Cyc_List_List*_tmp26D;_LL174: _tmp26E=_tmp268.f1;_tmp26D=_tmp268.f2;_LL175:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp26C=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E3);void**_tmp26B=_cycalloc(_check_times(_tmp26C,sizeof(void*)));({{unsigned int _tmp5EC=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E3);unsigned int i;for(i=0;i < _tmp5EC;++ i){({
void*_tmp70B=({void*_tmp26A=(void*)((struct Cyc_List_List*)_check_null(_tmp26D))->hd;
_tmp26D=_tmp26D->tl;
_tmp26A;});
# 1550
_tmp26B[i]=_tmp70B;});}}0;});_tag_dyneither(_tmp26B,sizeof(void*),_tmp26C);});
# 1554
return({struct _tuple16 _tmp5EB;_tmp5EB.f1=_tmp26E,({void*_tmp70C=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->tag=6U,(_tmp269->f1).is_union=0,(_tmp269->f1).fieldnum=- 1,_tmp269->f2=aggrdict;_tmp269;});_tmp5EB.f2=_tmp70C;});_tmp5EB;});};}case 30U: _LL66: _tmp2E4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL67: {
# 1557
struct Cyc_List_List*fs;
{void*_tmp26F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp270=_tmp26F;struct Cyc_List_List*_tmp273;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp270)->tag == 12U){_LL177: _tmp273=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp270)->f2;_LL178:
 fs=_tmp273;goto _LL176;}else{_LL179: _LL17A:
({void*_tmp271=0U;({struct _dyneither_ptr _tmp70D=({const char*_tmp272="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp272,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp70D,_tag_dyneither(_tmp271,sizeof(void*),0U));});});}_LL176:;}
# 1562
_tmp2E7=_tmp2E4;_tmp2E6=Cyc_Absyn_StructA;_tmp2E5=fs;goto _LL69;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4)->impl != 0){_LL68: _tmp2E7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_tmp2E6=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4)->kind;_tmp2E5=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4)->impl)->fields;_LL69: {
# 1564
void*exp_type=(void*)_check_null(e->topt);
struct _tuple23 _tmp274=({struct Cyc_NewControlFlow_AnalEnv*_tmp70F=env;union Cyc_CfFlowInfo_FlowInfo _tmp70E=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp70F,_tmp70E,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp2E7),1,1);});struct _tuple23 _tmp275=_tmp274;union Cyc_CfFlowInfo_FlowInfo _tmp27F;struct Cyc_List_List*_tmp27E;_LL17C: _tmp27F=_tmp275.f1;_tmp27E=_tmp275.f2;_LL17D:;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp125,_tmp2E5,_tmp2E6 == Cyc_Absyn_UnionA,_tmp125->unknown_all);
# 1569
int field_no=-1;
{int i=0;for(0;_tmp27E != 0;(_tmp27E=_tmp27E->tl,_tmp2E7=_tmp2E7->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp2E7))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp276=(void*)ds->hd;void*_tmp277=_tmp276;struct _dyneither_ptr*_tmp27B;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp277)->tag == 0U){_LL17F: _LL180:
({void*_tmp278=0U;({struct _dyneither_ptr _tmp710=({const char*_tmp279="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp279,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp710,_tag_dyneither(_tmp278,sizeof(void*),0U));});});}else{_LL181: _tmp27B=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp277)->f1;_LL182:
# 1576
({int _tmp711=Cyc_CfFlowInfo_get_field_index_fs(_tmp2E5,_tmp27B);field_no=_tmp711;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp27E->hd;
# 1579
if(_tmp2E6 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp27A=(*((struct _tuple26*)_tmp2E7->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp712=Cyc_NewControlFlow_use_Rval(env,_tmp27A->loc,_tmp27F,(void*)_tmp27E->hd);_tmp27F=_tmp712;});
# 1583
Cyc_NewControlFlow_check_union_requires(_tmp27A->loc,exp_type,_tmp27B,_tmp27F);}}_LL17E:;}}}{
# 1586
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27C=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D->tag=6U,(_tmp27D->f1).is_union=_tmp2E6 == Cyc_Absyn_UnionA,(_tmp27D->f1).fieldnum=field_no,_tmp27D->f2=aggrdict;_tmp27D;});
return({struct _tuple16 _tmp5ED;_tmp5ED.f1=_tmp27F,_tmp5ED.f2=(void*)_tmp27C;_tmp5ED;});};};}}else{goto _LL6A;}}else{_LL6A: _LL6B:
# 1590
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp5EE;_tmp5EE.tag=0U,({struct _dyneither_ptr _tmp713=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5EE.f1=_tmp713;});_tmp5EE;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmp714=({const char*_tmp281="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp281,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp714,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}case 26U: _LL6C: _tmp2E8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL6D: {
# 1592
struct Cyc_List_List*_tmp283=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp2E8);
struct _tuple23 _tmp284=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp283,1,1);struct _tuple23 _tmp285=_tmp284;union Cyc_CfFlowInfo_FlowInfo _tmp287;struct Cyc_List_List*_tmp286;_LL184: _tmp287=_tmp285.f1;_tmp286=_tmp285.f2;_LL185:;
for(0;_tmp286 != 0;(_tmp286=_tmp286->tl,_tmp283=_tmp283->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp715=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp283))->hd)->loc,_tmp287,(void*)_tmp286->hd);_tmp287=_tmp715;});}
return({struct _tuple16 _tmp5EF;_tmp5EF.f1=_tmp287,({void*_tmp716=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5EF.f2=_tmp716;});_tmp5EF;});}case 27U: _LL6E: _tmp2EC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2EB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_tmp2EA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_tmp2E9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f4;_LL6F: {
# 1600
struct _tuple16 _tmp288=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2EB);struct _tuple16 _tmp289=_tmp288;union Cyc_CfFlowInfo_FlowInfo _tmp2A4;void*_tmp2A3;_LL187: _tmp2A4=_tmp289.f1;_tmp2A3=_tmp289.f2;_LL188:;{
union Cyc_CfFlowInfo_FlowInfo _tmp28A=_tmp2A4;struct Cyc_Dict_Dict _tmp2A2;struct Cyc_List_List*_tmp2A1;if((_tmp28A.BottomFL).tag == 1){_LL18A: _LL18B:
 return({struct _tuple16 _tmp5F0;_tmp5F0.f1=_tmp2A4,_tmp5F0.f2=_tmp125->unknown_all;_tmp5F0;});}else{_LL18C: _tmp2A2=((_tmp28A.ReachableFL).val).f1;_tmp2A1=((_tmp28A.ReachableFL).val).f2;_LL18D:
# 1604
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2A2,_tmp2A3)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp28B=0U;({unsigned int _tmp718=_tmp2EB->loc;struct _dyneither_ptr _tmp717=({const char*_tmp28C="expression may not be initialized";_tag_dyneither(_tmp28C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp718,_tmp717,_tag_dyneither(_tmp28B,sizeof(void*),0U));});});{
# 1608
struct Cyc_List_List*new_relns=_tmp2A1;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp2EC);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp2EB,& n2))
({struct Cyc_List_List*_tmp719=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,_tmp2A1);new_relns=_tmp719;});
if(_tmp2A1 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp71A=Cyc_CfFlowInfo_ReachableFL(_tmp2A2,new_relns);_tmp2A4=_tmp71A;});{
# 1617
void*_tmp28D=_tmp2A3;switch(*((int*)_tmp28D)){case 0U: _LL18F: _LL190:
 return({struct _tuple16 _tmp5F1;_tmp5F1.f1=_tmp2A4,_tmp5F1.f2=_tmp125->unknown_all;_tmp5F1;});case 1U: _LL191: _LL192:
 goto _LL194;case 4U: _LL193: _LL194: {
# 1621
struct Cyc_List_List _tmp28E=({struct Cyc_List_List _tmp5F4;_tmp5F4.hd=_tmp2EC,_tmp5F4.tl=0;_tmp5F4;});
({union Cyc_CfFlowInfo_FlowInfo _tmp71B=Cyc_NewControlFlow_add_vars(_tmp125,_tmp2A4,& _tmp28E,_tmp125->unknown_all,e->loc,0);_tmp2A4=_tmp71B;});{
# 1624
struct _tuple16 _tmp28F=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2A4,_tmp2EA);struct _tuple16 _tmp290=_tmp28F;union Cyc_CfFlowInfo_FlowInfo _tmp296;void*_tmp295;_LL198: _tmp296=_tmp290.f1;_tmp295=_tmp290.f2;_LL199:;
{union Cyc_CfFlowInfo_FlowInfo _tmp291=_tmp296;struct Cyc_Dict_Dict _tmp294;if((_tmp291.BottomFL).tag == 1){_LL19B: _LL19C:
 return({struct _tuple16 _tmp5F2;_tmp5F2.f1=_tmp296,_tmp5F2.f2=_tmp125->unknown_all;_tmp5F2;});}else{_LL19D: _tmp294=((_tmp291.ReachableFL).val).f1;_LL19E:
# 1628
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp294,_tmp295)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp292=0U;({unsigned int _tmp71D=_tmp2EB->loc;struct _dyneither_ptr _tmp71C=({const char*_tmp293="expression may not be initialized";_tag_dyneither(_tmp293,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp71D,_tmp71C,_tag_dyneither(_tmp292,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5F3;({union Cyc_CfFlowInfo_FlowInfo _tmp71E=Cyc_CfFlowInfo_BottomFL();_tmp5F3.f1=_tmp71E;}),_tmp5F3.f2=_tmp125->unknown_all;_tmp5F3;});}}_LL19A:;}
# 1633
_tmp2A4=_tmp296;
goto _LL196;};}default: _LL195: _LL196:
# 1636
 while(1){
struct Cyc_List_List _tmp297=({struct Cyc_List_List _tmp5F6;_tmp5F6.hd=_tmp2EC,_tmp5F6.tl=0;_tmp5F6;});
({union Cyc_CfFlowInfo_FlowInfo _tmp71F=Cyc_NewControlFlow_add_vars(_tmp125,_tmp2A4,& _tmp297,_tmp125->unknown_all,e->loc,0);_tmp2A4=_tmp71F;});{
struct _tuple16 _tmp298=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2A4,_tmp2EA);struct _tuple16 _tmp299=_tmp298;union Cyc_CfFlowInfo_FlowInfo _tmp2A0;void*_tmp29F;_LL1A0: _tmp2A0=_tmp299.f1;_tmp29F=_tmp299.f2;_LL1A1:;
{union Cyc_CfFlowInfo_FlowInfo _tmp29A=_tmp2A0;struct Cyc_Dict_Dict _tmp29D;if((_tmp29A.BottomFL).tag == 1){_LL1A3: _LL1A4:
 goto _LL1A2;}else{_LL1A5: _tmp29D=((_tmp29A.ReachableFL).val).f1;_LL1A6:
# 1643
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp29D,_tmp29F)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp29B=0U;({unsigned int _tmp721=_tmp2EB->loc;struct _dyneither_ptr _tmp720=({const char*_tmp29C="expression may not be initialized";_tag_dyneither(_tmp29C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp721,_tmp720,_tag_dyneither(_tmp29B,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5F5;({union Cyc_CfFlowInfo_FlowInfo _tmp722=Cyc_CfFlowInfo_BottomFL();_tmp5F5.f1=_tmp722;}),_tmp5F5.f2=_tmp125->unknown_all;_tmp5F5;});}}_LL1A2:;}{
# 1648
union Cyc_CfFlowInfo_FlowInfo _tmp29E=Cyc_CfFlowInfo_join_flow(_tmp125,_tmp2A4,_tmp2A0);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp29E,_tmp2A4))
break;
_tmp2A4=_tmp29E;};};}
# 1653
return({struct _tuple16 _tmp5F7;_tmp5F7.f1=_tmp2A4,_tmp5F7.f2=_tmp125->unknown_all;_tmp5F7;});}_LL18E:;};};}_LL189:;};}case 28U: _LL70: _tmp2EF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2EE=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_tmp2ED=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL71: {
# 1659
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->tag=1U,_tmp2AD->f1=_tmp2EF,_tmp2AD->f2=(void*)_check_null(e->topt);_tmp2AD;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->root=root,_tmp2AC->path=0;_tmp2AC;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->tag=4U,_tmp2AB->f1=place;_tmp2AB;});
void*place_val;
# 1667
({void*_tmp723=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,_tmp2EE,0,_tmp125->unknown_none);place_val=_tmp723;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp2A5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2EF);struct _tuple16 _tmp2A6=_tmp2A5;union Cyc_CfFlowInfo_FlowInfo _tmp2AA;_LL1A8: _tmp2AA=_tmp2A6.f1;_LL1A9:;
outflow=_tmp2AA;{
union Cyc_CfFlowInfo_FlowInfo _tmp2A7=outflow;struct Cyc_Dict_Dict _tmp2A9;struct Cyc_List_List*_tmp2A8;if((_tmp2A7.BottomFL).tag == 1){_LL1AB: _LL1AC:
 return({struct _tuple16 _tmp5F8;_tmp5F8.f1=outflow,_tmp5F8.f2=rval;_tmp5F8;});}else{_LL1AD: _tmp2A9=((_tmp2A7.ReachableFL).val).f1;_tmp2A8=((_tmp2A7.ReachableFL).val).f2;_LL1AE:
# 1674
 return({struct _tuple16 _tmp5F9;({union Cyc_CfFlowInfo_FlowInfo _tmp725=({struct Cyc_Dict_Dict _tmp724=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A9,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp724,_tmp2A8);});_tmp5F9.f1=_tmp725;}),_tmp5F9.f2=rval;_tmp5F9;});}_LL1AA:;};};}case 37U: _LL72: _tmp2F0=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL73: {
# 1678
struct _tuple17 _tmp2AE=({struct _tuple17 _tmp5FB;_tmp5FB.f1=(env->fenv)->unknown_all,_tmp5FB.f2=copy_ctxt;_tmp5FB;});
union Cyc_CfFlowInfo_FlowInfo _tmp2AF=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp2F0,& _tmp2AE);
return({struct _tuple16 _tmp5FA;_tmp5FA.f1=_tmp2AF,_tmp5FA.f2=_tmp2AE.f1;_tmp5FA;});}case 36U: _LL76: _LL77:
# 1683
 goto _LL79;case 25U: _LL78: _LL79:
 goto _LL7B;case 39U: _LL7A: _LL7B:
 goto _LL7D;default: _LL7C: _LL7D:
({void*_tmp2B0=0U;({struct _dyneither_ptr _tmp726=({const char*_tmp2B1="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp2B1,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp726,_tag_dyneither(_tmp2B0,sizeof(void*),0U));});});}_LLD:;};}
# 1698 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1707
struct Cyc_CfFlowInfo_FlowEnv*_tmp2F1=env->fenv;
void*_tmp2F2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2F3=_tmp2F2;union Cyc_Absyn_Constraint*_tmp317;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F3)->tag == 5U){_LL1: _tmp317=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F3)->f1).ptr_atts).bounds;_LL2: {
# 1710
union Cyc_CfFlowInfo_FlowInfo _tmp2F4=f;struct Cyc_Dict_Dict _tmp314;struct Cyc_List_List*_tmp313;if((_tmp2F4.BottomFL).tag == 1){_LL6: _LL7:
 return({struct _tuple18 _tmp5FC;_tmp5FC.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp727=Cyc_CfFlowInfo_UnknownL();_tmp5FC.f2=_tmp727;});_tmp5FC;});}else{_LL8: _tmp314=((_tmp2F4.ReachableFL).val).f1;_tmp313=((_tmp2F4.ReachableFL).val).f2;_LL9: {
# 1714
struct _tuple15 _tmp2F5=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp2F6=_tmp2F5;void*_tmp312;struct Cyc_List_List*_tmp311;_LLB: _tmp312=_tmp2F6.f1;_tmp311=_tmp2F6.f2;_LLC:;
retry: {void*_tmp2F7=_tmp312;void*_tmp308;enum Cyc_CfFlowInfo_InitLevel _tmp307;void*_tmp306;struct Cyc_List_List*_tmp305;void*_tmp304;switch(*((int*)_tmp2F7)){case 8U: _LLE: _LLF:
# 1717
({void*_tmp2F8=0U;({struct _dyneither_ptr _tmp728=({const char*_tmp2F9="named location in anal_derefL";_tag_dyneither(_tmp2F9,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp728,_tag_dyneither(_tmp2F8,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 1719
({void*_tmp729=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));_tmp2FA->tag=Cyc_CfFlowInfo_NotZero,_tmp2FA->f1=_tmp313;_tmp2FA;});e->annot=_tmp729;});goto _LLD;case 5U: _LL12: _tmp304=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp2F7)->f1;_LL13:
# 1722
 if(expanded_place_possibly_null)
# 1725
({void*_tmp72A=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2FB->f1=_tmp313;_tmp2FB;});e->annot=_tmp72A;});else{
# 1728
void*_tmp2FC=e->annot;void*_tmp2FD=_tmp2FC;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2FD)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1F: _LL20:
# 1732
({void*_tmp72B=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE->tag=Cyc_CfFlowInfo_UnknownZ,_tmp2FE->f1=_tmp313;_tmp2FE;});e->annot=_tmp72B;});
goto _LL1E;}else{_LL21: _LL22:
# 1735
({void*_tmp72C=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));_tmp2FF->tag=Cyc_CfFlowInfo_NotZero,_tmp2FF->f1=_tmp313;_tmp2FF;});e->annot=_tmp72C;});
goto _LL1E;}_LL1E:;}
# 1740
if(Cyc_Tcutil_is_bound_one(_tmp317))return({struct _tuple18 _tmp5FD;_tmp5FD.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp72D=Cyc_CfFlowInfo_UnknownL();_tmp5FD.f2=_tmp72D;});_tmp5FD;});
goto _LLD;case 4U: _LL14: _tmp306=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F7)->f1)->root;_tmp305=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2F7)->f1)->path;_LL15:
# 1744
({void*_tmp72E=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->tag=Cyc_CfFlowInfo_NotZero,_tmp300->f1=_tmp313;_tmp300;});e->annot=_tmp72E;});
if(Cyc_Tcutil_is_bound_one(_tmp317))
return({struct _tuple18 _tmp5FE;_tmp5FE.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp730=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->root=_tmp306,({struct Cyc_List_List*_tmp72F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp305,path);_tmp301->path=_tmp72F;});_tmp301;}));_tmp5FE.f2=_tmp730;});_tmp5FE;});
goto _LLD;case 0U: _LL16: _LL17:
# 1749
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp5FF;({union Cyc_CfFlowInfo_FlowInfo _tmp732=Cyc_CfFlowInfo_BottomFL();_tmp5FF.f1=_tmp732;}),({union Cyc_CfFlowInfo_AbsLVal _tmp731=Cyc_CfFlowInfo_UnknownL();_tmp5FF.f2=_tmp731;});_tmp5FF;});case 2U: _LL18: _tmp307=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2F7)->f1;_LL19:
# 1754
 if(Cyc_Tcutil_is_bound_one(_tmp317))
({union Cyc_CfFlowInfo_FlowInfo _tmp733=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp307,_tmp311);f=_tmp733;});
({void*_tmp734=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp302=_cycalloc(sizeof(*_tmp302));_tmp302->tag=Cyc_CfFlowInfo_UnknownZ,_tmp302->f1=_tmp313;_tmp302;});e->annot=_tmp734;});
goto _LLD;case 7U: _LL1A: _tmp308=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2F7)->f3;_LL1B:
# 1759
 if(passthrough_consumes){
_tmp312=_tmp308;goto retry;}
# 1762
goto _LL1D;default: _LL1C: _LL1D:
# 1764
({void*_tmp735=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp303=_cycalloc(sizeof(*_tmp303));_tmp303->tag=Cyc_CfFlowInfo_UnknownZ,_tmp303->f1=_tmp313;_tmp303;});e->annot=_tmp735;});
goto _LLD;}_LLD:;}
# 1767
if(Cyc_CfFlowInfo_initlevel(_tmp2F1,_tmp314,_tmp312)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp309=Cyc_CfFlowInfo_unname_rval(_tmp312);struct _tuple15 _tmp30A=_tmp309;void*_tmp310;_LL24: _tmp310=_tmp30A.f1;_LL25:;{
void*_tmp30B=_tmp310;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp30B)->tag == 7U){_LL27: _LL28:
# 1771
({void*_tmp30C=0U;({unsigned int _tmp737=e->loc;struct _dyneither_ptr _tmp736=({const char*_tmp30D="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp30D,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp737,_tmp736,_tag_dyneither(_tmp30C,sizeof(void*),0U));});});
goto _LL26;}else{_LL29: _LL2A:
# 1774
({void*_tmp30E=0U;({unsigned int _tmp739=e->loc;struct _dyneither_ptr _tmp738=({const char*_tmp30F="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp30F,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp739,_tmp738,_tag_dyneither(_tmp30E,sizeof(void*),0U));});});
goto _LL26;}_LL26:;};}
# 1778
return({struct _tuple18 _tmp600;_tmp600.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp73A=Cyc_CfFlowInfo_UnknownL();_tmp600.f2=_tmp73A;});_tmp600;});}}_LL5:;}}else{_LL3: _LL4:
# 1780
({void*_tmp315=0U;({struct _dyneither_ptr _tmp73B=({const char*_tmp316="left deref of non-pointer-type";_tag_dyneither(_tmp316,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp73B,_tag_dyneither(_tmp315,sizeof(void*),0U));});});}_LL0:;}
# 1786
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp318=p;void*_tmp31D;struct Cyc_List_List*_tmp31C;_LL1: _tmp31D=_tmp318->root;_tmp31C=_tmp318->path;_LL2:;
({struct Cyc_List_List*_tmp73D=({struct Cyc_List_List*_tmp31A=_cycalloc(sizeof(*_tmp31A));({void*_tmp73C=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp319=_cycalloc(sizeof(*_tmp319));_tmp319->tag=1U;_tmp319;});_tmp31A->hd=_tmp73C;}),_tmp31A->tl=path;_tmp31A;});path=_tmp73D;});
return({struct Cyc_CfFlowInfo_Place*_tmp31B=_cycalloc(sizeof(*_tmp31B));_tmp31B->root=_tmp31D,({struct Cyc_List_List*_tmp73E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp31C,path);_tmp31B->path=_tmp73E;});_tmp31B;});}
# 1796
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1800
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp31E=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp31F=inflow;struct Cyc_Dict_Dict _tmp321;struct Cyc_List_List*_tmp320;if((_tmp31F.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple18 _tmp601;({union Cyc_CfFlowInfo_FlowInfo _tmp740=Cyc_CfFlowInfo_BottomFL();_tmp601.f1=_tmp740;}),({union Cyc_CfFlowInfo_AbsLVal _tmp73F=Cyc_CfFlowInfo_UnknownL();_tmp601.f2=_tmp73F;});_tmp601;});}else{_LL3: _tmp321=((_tmp31F.ReachableFL).val).f1;_tmp320=((_tmp31F.ReachableFL).val).f2;_LL4:
# 1805
 d=_tmp321;}_LL0:;}{
# 1809
void*_tmp322=e->r;void*_tmp323=_tmp322;struct Cyc_Absyn_Exp*_tmp37A;struct _dyneither_ptr*_tmp379;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*_tmp376;struct Cyc_Absyn_Exp*_tmp375;struct _dyneither_ptr*_tmp374;struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Vardecl*_tmp372;struct Cyc_Absyn_Vardecl*_tmp371;struct Cyc_Absyn_Vardecl*_tmp370;struct Cyc_Absyn_Exp*_tmp36F;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Absyn_Exp*_tmp36D;switch(*((int*)_tmp323)){case 14U: _LL6: _tmp36D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp323)->f2;_LL7:
 _tmp36E=_tmp36D;goto _LL9;case 12U: _LL8: _tmp36E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp323)->f1;_LL9:
 _tmp36F=_tmp36E;goto _LLB;case 13U: _LLA: _tmp36F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp323)->f1;_LLB:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp36F,path);case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp323)->f1)){case 3U: _LLC: _tmp370=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp323)->f1)->f1;_LLD:
# 1814
 _tmp371=_tmp370;goto _LLF;case 4U: _LLE: _tmp371=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp323)->f1)->f1;_LLF:
 _tmp372=_tmp371;goto _LL11;case 5U: _LL10: _tmp372=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp323)->f1)->f1;_LL11:
# 1817
 return({struct _tuple18 _tmp602;_tmp602.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp742=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp325=_cycalloc(sizeof(*_tmp325));({void*_tmp741=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->tag=0U,_tmp324->f1=_tmp372;_tmp324;});_tmp325->root=_tmp741;}),_tmp325->path=path;_tmp325;}));_tmp602.f2=_tmp742;});_tmp602;});case 1U: _LL12: _tmp373=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp323)->f1)->f1;_LL13:
# 1819
 return({struct _tuple18 _tmp603;_tmp603.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp743=Cyc_CfFlowInfo_UnknownL();_tmp603.f2=_tmp743;});_tmp603;});default: goto _LL1C;}case 22U: _LL14: _tmp375=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp323)->f1;_tmp374=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp323)->f2;_LL15:
# 1822
{void*_tmp326=Cyc_Tcutil_compress((void*)_check_null(_tmp375->topt));void*_tmp327=_tmp326;void*_tmp32C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327)->tag == 5U){_LL1F: _tmp32C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327)->f1).elt_typ;_LL20:
# 1824
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp32C)){
Cyc_NewControlFlow_check_union_requires(_tmp375->loc,_tmp32C,_tmp374,inflow);
({struct Cyc_List_List*_tmp746=({struct Cyc_List_List*_tmp329=_cycalloc(sizeof(*_tmp329));({void*_tmp745=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->tag=0U,({int _tmp744=Cyc_CfFlowInfo_get_field_index(_tmp32C,_tmp374);_tmp328->f1=_tmp744;});_tmp328;});_tmp329->hd=_tmp745;}),_tmp329->tl=path;_tmp329;});path=_tmp746;});}
# 1828
goto _LL1E;}else{_LL21: _LL22:
({void*_tmp32A=0U;({struct _dyneither_ptr _tmp747=({const char*_tmp32B="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp32B,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp747,_tag_dyneither(_tmp32A,sizeof(void*),0U));});});}_LL1E:;}
# 1831
_tmp376=_tmp375;goto _LL17;case 20U: _LL16: _tmp376=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp323)->f1;_LL17:
# 1835
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp376->topt),1)){
# 1837
struct _tuple18 _tmp32D=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp376);struct _tuple18 _tmp32E=_tmp32D;union Cyc_CfFlowInfo_FlowInfo _tmp34D;union Cyc_CfFlowInfo_AbsLVal _tmp34C;_LL24: _tmp34D=_tmp32E.f1;_tmp34C=_tmp32E.f2;_LL25:;{
struct _tuple18 _tmp32F=({struct _tuple18 _tmp607;_tmp607.f1=_tmp34D,_tmp607.f2=_tmp34C;_tmp607;});struct _tuple18 _tmp330=_tmp32F;struct Cyc_Dict_Dict _tmp34B;struct Cyc_List_List*_tmp34A;struct Cyc_CfFlowInfo_Place*_tmp349;if(((_tmp330.f1).ReachableFL).tag == 2){if(((_tmp330.f2).PlaceL).tag == 1){_LL27: _tmp34B=(((_tmp330.f1).ReachableFL).val).f1;_tmp34A=(((_tmp330.f1).ReachableFL).val).f2;_tmp349=((_tmp330.f2).PlaceL).val;_LL28: {
# 1840
void*_tmp331=Cyc_CfFlowInfo_lookup_place(_tmp34B,_tmp349);
struct _tuple15 _tmp332=Cyc_CfFlowInfo_unname_rval(_tmp331);struct _tuple15 _tmp333=_tmp332;void*_tmp348;struct Cyc_List_List*_tmp347;_LL2C: _tmp348=_tmp333.f1;_tmp347=_tmp333.f2;_LL2D:;
if(expand_unique){
int _tmp334=1;
void*_tmp335=_tmp348;struct Cyc_Absyn_Vardecl*_tmp342;void*_tmp341;switch(*((int*)_tmp335)){case 8U: _LL2F: _tmp342=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp335)->f1;_tmp341=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp335)->f2;_LL30:
# 1846
({void*_tmp336=0U;({struct _dyneither_ptr _tmp748=({const char*_tmp337="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp337,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp748,_tag_dyneither(_tmp336,sizeof(void*),0U));});});case 7U: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp335)->f3)){case 4U: _LL31: _LL32:
 goto _LL34;case 5U: _LL35: _LL36:
# 1850
 goto _LL38;default: goto _LL3B;}case 4U: _LL33: _LL34:
# 1849
 return Cyc_NewControlFlow_anal_derefL(env,_tmp34D,_tmp376,_tmp34D,_tmp331,passthrough_consumes,0,path);case 5U: _LL37: _LL38: {
# 1854
struct _tuple18 _tmp338=Cyc_NewControlFlow_anal_derefL(env,_tmp34D,_tmp376,_tmp34D,_tmp331,passthrough_consumes,0,path);struct _tuple18 _tmp339=_tmp338;union Cyc_CfFlowInfo_FlowInfo _tmp33A;_LL3E: _tmp33A=_tmp339.f1;_LL3F:;
return({struct _tuple18 _tmp604;_tmp604.f1=_tmp33A,({union Cyc_CfFlowInfo_AbsLVal _tmp749=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp349,path));_tmp604.f2=_tmp749;});_tmp604;});}case 1U: _LL39: _LL3A:
# 1857
 _tmp334=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1864
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp31E,_tmp34B,_tmp348);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp31E->unknown_all: _tmp31E->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->tag=5U,({void*_tmp74C=({struct Cyc_CfFlowInfo_FlowEnv*_tmp74B=_tmp31E;void*_tmp74A=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp376->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp74B,_tmp74A,0,leaf);});_tmp340->f1=_tmp74C;});_tmp340;});
for(0;_tmp347 != 0;_tmp347=_tmp347->tl){
({void*_tmp74D=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->tag=8U,_tmp33B->f1=(struct Cyc_Absyn_Vardecl*)_tmp347->hd,_tmp33B->f2=res;_tmp33B;});res=_tmp74D;});}
({struct Cyc_Dict_Dict _tmp74E=Cyc_CfFlowInfo_assign_place(_tmp31E,e->loc,_tmp34B,_tmp349,res);_tmp34B=_tmp74E;});{
union Cyc_CfFlowInfo_FlowInfo _tmp33C=Cyc_CfFlowInfo_ReachableFL(_tmp34B,_tmp34A);
# 1877
struct _tuple18 _tmp33D=Cyc_NewControlFlow_anal_derefL(env,_tmp33C,_tmp376,_tmp33C,res,passthrough_consumes,_tmp334,path);struct _tuple18 _tmp33E=_tmp33D;union Cyc_CfFlowInfo_FlowInfo _tmp33F;_LL41: _tmp33F=_tmp33E.f1;_LL42:;
# 1879
return({struct _tuple18 _tmp605;_tmp605.f1=_tmp33F,({union Cyc_CfFlowInfo_AbsLVal _tmp74F=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp349,path));_tmp605.f2=_tmp74F;});_tmp605;});};}}_LL2E:;}else{
# 1883
void*_tmp343=_tmp348;switch(*((int*)_tmp343)){case 7U: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp343)->f3)->tag == 5U){_LL44: _LL45:
 goto _LL47;}else{goto _LL48;}case 5U: _LL46: _LL47: {
# 1888
struct _tuple18 _tmp344=Cyc_NewControlFlow_anal_derefL(env,_tmp34D,_tmp376,_tmp34D,_tmp331,passthrough_consumes,0,path);struct _tuple18 _tmp345=_tmp344;union Cyc_CfFlowInfo_FlowInfo _tmp346;_LL4B: _tmp346=_tmp345.f1;_LL4C:;
return({struct _tuple18 _tmp606;_tmp606.f1=_tmp346,({union Cyc_CfFlowInfo_AbsLVal _tmp750=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp349,path));_tmp606.f2=_tmp750;});_tmp606;});}default: _LL48: _LL49:
# 1892
 goto _LL43;}_LL43:;}
# 1895
goto _LL26;}}else{goto _LL29;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;};}{
# 1900
struct _tuple16 _tmp34E=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp376);struct _tuple16 _tmp34F=_tmp34E;union Cyc_CfFlowInfo_FlowInfo _tmp351;void*_tmp350;_LL4E: _tmp351=_tmp34F.f1;_tmp350=_tmp34F.f2;_LL4F:;
# 1902
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp376,_tmp351,_tmp350,passthrough_consumes,0,path);};case 23U: _LL18: _tmp378=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp323)->f1;_tmp377=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp323)->f2;_LL19: {
# 1905
void*_tmp352=Cyc_Tcutil_compress((void*)_check_null(_tmp378->topt));void*_tmp353=_tmp352;switch(*((int*)_tmp353)){case 10U: _LL51: _LL52: {
# 1907
unsigned int _tmp354=(Cyc_Evexp_eval_const_uint_exp(_tmp377)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp756=env;union Cyc_CfFlowInfo_FlowInfo _tmp755=inflow;int _tmp754=expand_unique;int _tmp753=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp752=_tmp378;Cyc_NewControlFlow_anal_Lexp_rec(_tmp756,_tmp755,_tmp754,_tmp753,_tmp752,({struct Cyc_List_List*_tmp356=_cycalloc(sizeof(*_tmp356));
({void*_tmp751=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355->tag=0U,_tmp355->f1=(int)_tmp354;_tmp355;});_tmp356->hd=_tmp751;}),_tmp356->tl=path;_tmp356;}));});}case 5U: _LL53: _LL54: {
# 1911
struct _tuple23 _tmp357=({struct Cyc_NewControlFlow_AnalEnv*_tmp758=env;union Cyc_CfFlowInfo_FlowInfo _tmp757=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp758,_tmp757,({struct Cyc_Absyn_Exp*_tmp367[2U];_tmp367[0]=_tmp378,_tmp367[1]=_tmp377;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp367,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp358=_tmp357;union Cyc_CfFlowInfo_FlowInfo _tmp366;struct Cyc_List_List*_tmp365;_LL58: _tmp366=_tmp358.f1;_tmp365=_tmp358.f2;_LL59:;{
union Cyc_CfFlowInfo_FlowInfo _tmp359=_tmp366;
{union Cyc_CfFlowInfo_FlowInfo _tmp35A=_tmp366;struct Cyc_Dict_Dict _tmp35F;struct Cyc_List_List*_tmp35E;if((_tmp35A.ReachableFL).tag == 2){_LL5B: _tmp35F=((_tmp35A.ReachableFL).val).f1;_tmp35E=((_tmp35A.ReachableFL).val).f2;_LL5C:
# 1915
 if(Cyc_CfFlowInfo_initlevel(_tmp31E,_tmp35F,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp365))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp35B=0U;({unsigned int _tmp75A=_tmp377->loc;struct _dyneither_ptr _tmp759=({const char*_tmp35C="expression may not be initialized";_tag_dyneither(_tmp35C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp75A,_tmp759,_tag_dyneither(_tmp35B,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp35D=Cyc_NewControlFlow_add_subscript_reln(_tmp35E,_tmp378,_tmp377);
if(_tmp35E != _tmp35D)
({union Cyc_CfFlowInfo_FlowInfo _tmp75B=Cyc_CfFlowInfo_ReachableFL(_tmp35F,_tmp35D);_tmp359=_tmp75B;});
goto _LL5A;};}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}{
# 1924
struct _tuple18 _tmp360=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp378,_tmp366,(void*)((struct Cyc_List_List*)_check_null(_tmp365))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp361=_tmp360;union Cyc_CfFlowInfo_FlowInfo _tmp364;union Cyc_CfFlowInfo_AbsLVal _tmp363;_LL60: _tmp364=_tmp361.f1;_tmp363=_tmp361.f2;_LL61:;{
union Cyc_CfFlowInfo_FlowInfo _tmp362=_tmp364;if((_tmp362.BottomFL).tag == 1){_LL63: _LL64:
 return({struct _tuple18 _tmp608;_tmp608.f1=_tmp364,_tmp608.f2=_tmp363;_tmp608;});}else{_LL65: _LL66:
 return({struct _tuple18 _tmp609;_tmp609.f1=_tmp359,_tmp609.f2=_tmp363;_tmp609;});}_LL62:;};};};}default: _LL55: _LL56:
# 1929
({void*_tmp368=0U;({struct _dyneither_ptr _tmp75C=({const char*_tmp369="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp369,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp75C,_tag_dyneither(_tmp368,sizeof(void*),0U));});});}_LL50:;}case 21U: _LL1A: _tmp37A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp323)->f1;_tmp379=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp323)->f2;_LL1B: {
# 1933
void*_tmp36A=(void*)_check_null(_tmp37A->topt);
if(Cyc_Absyn_is_require_union_type(_tmp36A))
Cyc_NewControlFlow_check_union_requires(_tmp37A->loc,_tmp36A,_tmp379,inflow);
# 1937
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp36A))
return({struct _tuple18 _tmp60A;_tmp60A.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp75D=Cyc_CfFlowInfo_UnknownL();_tmp60A.f2=_tmp75D;});_tmp60A;});
# 1940
return({struct Cyc_NewControlFlow_AnalEnv*_tmp764=env;union Cyc_CfFlowInfo_FlowInfo _tmp763=inflow;int _tmp762=expand_unique;int _tmp761=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp760=_tmp37A;Cyc_NewControlFlow_anal_Lexp_rec(_tmp764,_tmp763,_tmp762,_tmp761,_tmp760,({struct Cyc_List_List*_tmp36C=_cycalloc(sizeof(*_tmp36C));
({void*_tmp75F=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->tag=0U,({int _tmp75E=Cyc_CfFlowInfo_get_field_index(_tmp36A,_tmp379);_tmp36B->f1=_tmp75E;});_tmp36B;});_tmp36C->hd=_tmp75F;}),_tmp36C->tl=path;_tmp36C;}));});}default: _LL1C: _LL1D:
# 1944
 return({struct _tuple18 _tmp60B;({union Cyc_CfFlowInfo_FlowInfo _tmp766=Cyc_CfFlowInfo_BottomFL();_tmp60B.f1=_tmp766;}),({union Cyc_CfFlowInfo_AbsLVal _tmp765=Cyc_CfFlowInfo_UnknownL();_tmp60B.f2=_tmp765;});_tmp60B;});}_LL5:;};}
# 1948
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1952
struct _tuple18 _tmp37B=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp37C=_tmp37B;union Cyc_CfFlowInfo_FlowInfo _tmp37E;union Cyc_CfFlowInfo_AbsLVal _tmp37D;_LL1: _tmp37E=_tmp37C.f1;_tmp37D=_tmp37C.f2;_LL2:;
return({struct _tuple18 _tmp60C;_tmp60C.f1=_tmp37E,_tmp60C.f2=_tmp37D;_tmp60C;});}
# 1959
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1962
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp37F=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp380=_tmp37F;union Cyc_CfFlowInfo_FlowInfo _tmp381;_LL1: _tmp381=_tmp380.f1;_LL2:;
inflow=_tmp381;}}}
# 1970
return inflow;}
# 1976
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1978
struct Cyc_CfFlowInfo_FlowEnv*_tmp382=env->fenv;
# 1981
struct _tuple16 _tmp383=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);struct _tuple16 _tmp384=_tmp383;union Cyc_CfFlowInfo_FlowInfo _tmp3C1;void*_tmp3C0;_LL1: _tmp3C1=_tmp384.f1;_tmp3C0=_tmp384.f2;_LL2:;{
struct _tuple16 _tmp385=Cyc_NewControlFlow_anal_Rexp(env,0,_tmp3C1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);struct _tuple16 _tmp386=_tmp385;union Cyc_CfFlowInfo_FlowInfo _tmp3BF;void*_tmp3BE;_LL4: _tmp3BF=_tmp386.f1;_tmp3BE=_tmp386.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp387=_tmp3BF;
# 1986
union Cyc_CfFlowInfo_FlowInfo _tmp388=_tmp387;struct Cyc_Dict_Dict _tmp3BD;struct Cyc_List_List*_tmp3BC;if((_tmp388.BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple19 _tmp60D;_tmp60D.f1=_tmp387,_tmp60D.f2=_tmp387;_tmp60D;});}else{_LL9: _tmp3BD=((_tmp388.ReachableFL).val).f1;_tmp3BC=((_tmp388.ReachableFL).val).f2;_LLA: {
# 1989
struct Cyc_Absyn_Exp*_tmp389=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*_tmp38A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BD,_tmp3C0)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp3C0))
({void*_tmp38B=0U;({unsigned int _tmp768=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp767=({const char*_tmp38C="expression may not be initialized";_tag_dyneither(_tmp38C,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp768,_tmp767,_tag_dyneither(_tmp38B,sizeof(void*),0U));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BD,_tmp3BE)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp3C0))
({void*_tmp38D=0U;({unsigned int _tmp76A=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp769=({const char*_tmp38E="expression may not be initialized";_tag_dyneither(_tmp38E,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp76A,_tmp769,_tag_dyneither(_tmp38D,sizeof(void*),0U));});});{
# 1996
union Cyc_CfFlowInfo_FlowInfo _tmp38F=_tmp387;
union Cyc_CfFlowInfo_FlowInfo _tmp390=_tmp387;
# 2001
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp391=Cyc_CfFlowInfo_unname_rval(_tmp3C0);struct _tuple15 _tmp392=_tmp391;void*_tmp3AA;struct Cyc_List_List*_tmp3A9;_LLC: _tmp3AA=_tmp392.f1;_tmp3A9=_tmp392.f2;_LLD:;{
struct _tuple15 _tmp393=Cyc_CfFlowInfo_unname_rval(_tmp3BE);struct _tuple15 _tmp394=_tmp393;void*_tmp3A8;struct Cyc_List_List*_tmp3A7;_LLF: _tmp3A8=_tmp394.f1;_tmp3A7=_tmp394.f2;_LL10:;{
struct _tuple0 _tmp395=({struct _tuple0 _tmp60E;_tmp60E.f1=_tmp3AA,_tmp60E.f2=_tmp3A8;_tmp60E;});struct _tuple0 _tmp396=_tmp395;enum Cyc_CfFlowInfo_InitLevel _tmp3A6;enum Cyc_CfFlowInfo_InitLevel _tmp3A5;switch(*((int*)_tmp396.f1)){case 2U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp396.f2)->tag == 0U){_LL12: _tmp3A5=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp396.f1)->f1;_LL13: {
# 2006
struct _tuple19 _tmp397=Cyc_NewControlFlow_splitzero(env,inflow,_tmp387,_tmp389,_tmp3A5,_tmp3A9);struct _tuple19 _tmp398=_tmp397;union Cyc_CfFlowInfo_FlowInfo _tmp39D;union Cyc_CfFlowInfo_FlowInfo _tmp39C;_LL23: _tmp39D=_tmp398.f1;_tmp39C=_tmp398.f2;_LL24:;
{enum Cyc_Absyn_Primop _tmp399=p;switch(_tmp399){case Cyc_Absyn_Eq: _LL26: _LL27:
 _tmp38F=_tmp39C;_tmp390=_tmp39D;goto _LL25;case Cyc_Absyn_Neq: _LL28: _LL29:
 _tmp38F=_tmp39D;_tmp390=_tmp39C;goto _LL25;default: _LL2A: _LL2B:
({void*_tmp39A=0U;({struct _dyneither_ptr _tmp76B=({const char*_tmp39B="anal_test, zero-split";_tag_dyneither(_tmp39B,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp76B,_tag_dyneither(_tmp39A,sizeof(void*),0U));});});}_LL25:;}
# 2012
goto _LL11;}}else{goto _LL20;}case 0U: switch(*((int*)_tmp396.f2)){case 2U: _LL14: _tmp3A6=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp396.f2)->f1;_LL15: {
# 2014
struct _tuple19 _tmp39E=Cyc_NewControlFlow_splitzero(env,_tmp3BF,_tmp387,_tmp38A,_tmp3A6,_tmp3A7);struct _tuple19 _tmp39F=_tmp39E;union Cyc_CfFlowInfo_FlowInfo _tmp3A4;union Cyc_CfFlowInfo_FlowInfo _tmp3A3;_LL2D: _tmp3A4=_tmp39F.f1;_tmp3A3=_tmp39F.f2;_LL2E:;
{enum Cyc_Absyn_Primop _tmp3A0=p;switch(_tmp3A0){case Cyc_Absyn_Eq: _LL30: _LL31:
 _tmp38F=_tmp3A3;_tmp390=_tmp3A4;goto _LL2F;case Cyc_Absyn_Neq: _LL32: _LL33:
 _tmp38F=_tmp3A4;_tmp390=_tmp3A3;goto _LL2F;default: _LL34: _LL35:
({void*_tmp3A1=0U;({struct _dyneither_ptr _tmp76C=({const char*_tmp3A2="anal_test, zero-split";_tag_dyneither(_tmp3A2,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp76C,_tag_dyneither(_tmp3A1,sizeof(void*),0U));});});}_LL2F:;}
# 2020
goto _LL11;}case 0U: _LL16: _LL17:
# 2022
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp76D=Cyc_CfFlowInfo_BottomFL();_tmp390=_tmp76D;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp76E=Cyc_CfFlowInfo_BottomFL();_tmp38F=_tmp76E;});}
goto _LL11;case 1U: _LL18: _LL19:
 goto _LL1B;case 4U: _LL1A: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp396.f2)->tag == 0U){_LL1C: _LL1D:
 goto _LL1F;}else{goto _LL20;}case 4U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp396.f2)->tag == 0U){_LL1E: _LL1F:
# 2029
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp76F=Cyc_CfFlowInfo_BottomFL();_tmp390=_tmp76F;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp770=Cyc_CfFlowInfo_BottomFL();_tmp38F=_tmp770;});}
goto _LL11;}else{goto _LL20;}default: _LL20: _LL21:
 goto _LL11;}_LL11:;};};}
# 2040
{struct _tuple0 _tmp3AB=({struct _tuple0 _tmp610;({void*_tmp772=Cyc_Tcutil_compress((void*)_check_null(_tmp389->topt));_tmp610.f1=_tmp772;}),({
void*_tmp771=Cyc_Tcutil_compress((void*)_check_null(_tmp38A->topt));_tmp610.f2=_tmp771;});_tmp610;});
# 2040
struct _tuple0 _tmp3AC=_tmp3AB;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AC.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AC.f1)->f1 == Cyc_Absyn_Unsigned){_LL37: _LL38:
# 2042
 goto _LL3A;}else{switch(*((int*)_tmp3AC.f2)){case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AC.f2)->f1 == Cyc_Absyn_Unsigned)goto _LL39;else{goto _LL3F;}case 19U: goto _LL3D;default: goto _LL3F;}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AC.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3AC.f2)->f1 == Cyc_Absyn_Unsigned){_LL39: _LL3A:
 goto _LL3C;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3AC.f1)->tag == 19U)goto _LL3B;else{goto _LL3F;}}}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3AC.f1)->tag == 19U){_LL3B: _LL3C:
 goto _LL3E;}else{if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3AC.f2)->tag == 19U){_LL3D: _LL3E:
 goto _LL36;}else{_LL3F: _LL40:
 return({struct _tuple19 _tmp60F;_tmp60F.f1=_tmp38F,_tmp60F.f2=_tmp390;_tmp60F;});}}}}_LL36:;}
# 2050
{enum Cyc_Absyn_Primop _tmp3AD=p;switch(_tmp3AD){case Cyc_Absyn_Eq: _LL42: _LL43:
 goto _LL45;case Cyc_Absyn_Neq: _LL44: _LL45: goto _LL47;case Cyc_Absyn_Gt: _LL46: _LL47: goto _LL49;case Cyc_Absyn_Gte: _LL48: _LL49: goto _LL4B;case Cyc_Absyn_Lt: _LL4A: _LL4B: goto _LL4D;case Cyc_Absyn_Lte: _LL4C: _LL4D: goto _LL41;default: _LL4E: _LL4F:
 return({struct _tuple19 _tmp611;_tmp611.f1=_tmp38F,_tmp611.f2=_tmp390;_tmp611;});}_LL41:;}{
# 2055
struct _RegionHandle*_tmp3AE=Cyc_Core_heap_region;
struct _tuple13 _tmp3AF=Cyc_Relations_primop2relation(_tmp389,p,_tmp38A);struct _tuple13 _tmp3B0=_tmp3AF;struct Cyc_Absyn_Exp*_tmp3BB;enum Cyc_Relations_Relation _tmp3BA;struct Cyc_Absyn_Exp*_tmp3B9;_LL51: _tmp3BB=_tmp3B0.f1;_tmp3BA=_tmp3B0.f2;_tmp3B9=_tmp3B0.f3;_LL52:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2060
if(Cyc_Relations_exp2relnop(_tmp3BB,& n1) && Cyc_Relations_exp2relnop(_tmp3B9,& n2)){
# 2062
struct Cyc_List_List*_tmp3B1=Cyc_Relations_add_relation(_tmp3AE,n1,_tmp3BA,n2,_tmp3BC);
# 2066
struct Cyc_List_List*_tmp3B2=({struct _RegionHandle*_tmp776=_tmp3AE;union Cyc_Relations_RelnOp _tmp775=n2;enum Cyc_Relations_Relation _tmp774=Cyc_Relations_flip_relation(_tmp3BA);union Cyc_Relations_RelnOp _tmp773=n1;Cyc_Relations_add_relation(_tmp776,_tmp775,_tmp774,_tmp773,_tmp3BC);});
struct _tuple19 _tmp3B3=({struct _tuple19 _tmp616;_tmp616.f1=_tmp38F,_tmp616.f2=_tmp390;_tmp616;});struct _tuple19 _tmp3B4=_tmp3B3;struct Cyc_Dict_Dict _tmp3B8;struct Cyc_Dict_Dict _tmp3B7;struct Cyc_Dict_Dict _tmp3B6;struct Cyc_Dict_Dict _tmp3B5;if(((_tmp3B4.f1).ReachableFL).tag == 2){if(((_tmp3B4.f2).ReachableFL).tag == 2){_LL54: _tmp3B6=(((_tmp3B4.f1).ReachableFL).val).f1;_tmp3B5=(((_tmp3B4.f2).ReachableFL).val).f1;_LL55:
# 2069
 return({struct _tuple19 _tmp612;({union Cyc_CfFlowInfo_FlowInfo _tmp778=Cyc_CfFlowInfo_ReachableFL(_tmp3B6,_tmp3B1);_tmp612.f1=_tmp778;}),({union Cyc_CfFlowInfo_FlowInfo _tmp777=Cyc_CfFlowInfo_ReachableFL(_tmp3B5,_tmp3B2);_tmp612.f2=_tmp777;});_tmp612;});}else{_LL58: _tmp3B7=(((_tmp3B4.f1).ReachableFL).val).f1;_LL59:
# 2073
 return({struct _tuple19 _tmp613;({union Cyc_CfFlowInfo_FlowInfo _tmp779=Cyc_CfFlowInfo_ReachableFL(_tmp3B7,_tmp3B1);_tmp613.f1=_tmp779;}),_tmp613.f2=_tmp390;_tmp613;});}}else{if(((_tmp3B4.f2).ReachableFL).tag == 2){_LL56: _tmp3B8=(((_tmp3B4.f2).ReachableFL).val).f1;_LL57:
# 2071
 return({struct _tuple19 _tmp614;_tmp614.f1=_tmp38F,({union Cyc_CfFlowInfo_FlowInfo _tmp77A=Cyc_CfFlowInfo_ReachableFL(_tmp3B8,_tmp3B2);_tmp614.f2=_tmp77A;});_tmp614;});}else{_LL5A: _LL5B:
# 2075
 return({struct _tuple19 _tmp615;_tmp615.f1=_tmp38F,_tmp615.f2=_tmp390;_tmp615;});}}_LL53:;}else{
# 2078
return({struct _tuple19 _tmp617;_tmp617.f1=_tmp38F,_tmp617.f2=_tmp390;_tmp617;});}};};};}}_LL6:;};};}
# 2084
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C2=env->fenv;
void*_tmp3C3=e->r;void*_tmp3C4=_tmp3C3;enum Cyc_Absyn_Primop _tmp408;struct Cyc_List_List*_tmp407;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp405;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;switch(*((int*)_tmp3C4)){case 6U: _LL1: _tmp3FF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C4)->f1;_tmp3FE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2;_tmp3FD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3C4)->f3;_LL2: {
# 2088
struct _tuple19 _tmp3C5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3FF);struct _tuple19 _tmp3C6=_tmp3C5;union Cyc_CfFlowInfo_FlowInfo _tmp3D0;union Cyc_CfFlowInfo_FlowInfo _tmp3CF;_LL10: _tmp3D0=_tmp3C6.f1;_tmp3CF=_tmp3C6.f2;_LL11:;{
struct _tuple19 _tmp3C7=Cyc_NewControlFlow_anal_test(env,_tmp3D0,_tmp3FE);struct _tuple19 _tmp3C8=_tmp3C7;union Cyc_CfFlowInfo_FlowInfo _tmp3CE;union Cyc_CfFlowInfo_FlowInfo _tmp3CD;_LL13: _tmp3CE=_tmp3C8.f1;_tmp3CD=_tmp3C8.f2;_LL14:;{
struct _tuple19 _tmp3C9=Cyc_NewControlFlow_anal_test(env,_tmp3CF,_tmp3FD);struct _tuple19 _tmp3CA=_tmp3C9;union Cyc_CfFlowInfo_FlowInfo _tmp3CC;union Cyc_CfFlowInfo_FlowInfo _tmp3CB;_LL16: _tmp3CC=_tmp3CA.f1;_tmp3CB=_tmp3CA.f2;_LL17:;
return({struct _tuple19 _tmp618;({union Cyc_CfFlowInfo_FlowInfo _tmp77C=Cyc_CfFlowInfo_join_flow(_tmp3C2,_tmp3CE,_tmp3CC);_tmp618.f1=_tmp77C;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp77B=Cyc_CfFlowInfo_join_flow(_tmp3C2,_tmp3CD,_tmp3CB);_tmp618.f2=_tmp77B;});_tmp618;});};};}case 7U: _LL3: _tmp401=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C4)->f1;_tmp400=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2;_LL4: {
# 2094
struct _tuple19 _tmp3D1=Cyc_NewControlFlow_anal_test(env,inflow,_tmp401);struct _tuple19 _tmp3D2=_tmp3D1;union Cyc_CfFlowInfo_FlowInfo _tmp3D8;union Cyc_CfFlowInfo_FlowInfo _tmp3D7;_LL19: _tmp3D8=_tmp3D2.f1;_tmp3D7=_tmp3D2.f2;_LL1A:;{
struct _tuple19 _tmp3D3=Cyc_NewControlFlow_anal_test(env,_tmp3D8,_tmp400);struct _tuple19 _tmp3D4=_tmp3D3;union Cyc_CfFlowInfo_FlowInfo _tmp3D6;union Cyc_CfFlowInfo_FlowInfo _tmp3D5;_LL1C: _tmp3D6=_tmp3D4.f1;_tmp3D5=_tmp3D4.f2;_LL1D:;
return({struct _tuple19 _tmp619;_tmp619.f1=_tmp3D6,({union Cyc_CfFlowInfo_FlowInfo _tmp77D=Cyc_CfFlowInfo_join_flow(_tmp3C2,_tmp3D7,_tmp3D5);_tmp619.f2=_tmp77D;});_tmp619;});};}case 8U: _LL5: _tmp403=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C4)->f1;_tmp402=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2;_LL6: {
# 2098
struct _tuple19 _tmp3D9=Cyc_NewControlFlow_anal_test(env,inflow,_tmp403);struct _tuple19 _tmp3DA=_tmp3D9;union Cyc_CfFlowInfo_FlowInfo _tmp3E0;union Cyc_CfFlowInfo_FlowInfo _tmp3DF;_LL1F: _tmp3E0=_tmp3DA.f1;_tmp3DF=_tmp3DA.f2;_LL20:;{
struct _tuple19 _tmp3DB=Cyc_NewControlFlow_anal_test(env,_tmp3DF,_tmp402);struct _tuple19 _tmp3DC=_tmp3DB;union Cyc_CfFlowInfo_FlowInfo _tmp3DE;union Cyc_CfFlowInfo_FlowInfo _tmp3DD;_LL22: _tmp3DE=_tmp3DC.f1;_tmp3DD=_tmp3DC.f2;_LL23:;
return({struct _tuple19 _tmp61A;({union Cyc_CfFlowInfo_FlowInfo _tmp77E=Cyc_CfFlowInfo_join_flow(_tmp3C2,_tmp3E0,_tmp3DE);_tmp61A.f1=_tmp77E;}),_tmp61A.f2=_tmp3DD;_tmp61A;});};}case 9U: _LL7: _tmp405=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C4)->f1;_tmp404=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2;_LL8: {
# 2102
struct _tuple16 _tmp3E1=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp405);struct _tuple16 _tmp3E2=_tmp3E1;union Cyc_CfFlowInfo_FlowInfo _tmp3E4;void*_tmp3E3;_LL25: _tmp3E4=_tmp3E2.f1;_tmp3E3=_tmp3E2.f2;_LL26:;
return Cyc_NewControlFlow_anal_test(env,_tmp3E4,_tmp404);}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C4)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2)->tl == 0){_LL9: _tmp406=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2)->hd;_LLA: {
# 2105
struct _tuple19 _tmp3E5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp406);struct _tuple19 _tmp3E6=_tmp3E5;union Cyc_CfFlowInfo_FlowInfo _tmp3E8;union Cyc_CfFlowInfo_FlowInfo _tmp3E7;_LL28: _tmp3E8=_tmp3E6.f1;_tmp3E7=_tmp3E6.f2;_LL29:;
return({struct _tuple19 _tmp61B;_tmp61B.f1=_tmp3E7,_tmp61B.f2=_tmp3E8;_tmp61B;});}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp408=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C4)->f1;_tmp407=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C4)->f2;_LLC:
# 2108
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp408,_tmp407);}default: _LLD: _LLE: {
# 2112
struct _tuple16 _tmp3E9=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp3EA=_tmp3E9;union Cyc_CfFlowInfo_FlowInfo _tmp3FC;void*_tmp3FB;_LL2B: _tmp3FC=_tmp3EA.f1;_tmp3FB=_tmp3EA.f2;_LL2C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3EB=_tmp3FC;struct Cyc_Dict_Dict _tmp3FA;if((_tmp3EB.BottomFL).tag == 1){_LL2E: _LL2F:
 return({struct _tuple19 _tmp61C;_tmp61C.f1=_tmp3FC,_tmp61C.f2=_tmp3FC;_tmp61C;});}else{_LL30: _tmp3FA=((_tmp3EB.ReachableFL).val).f1;_LL31: {
# 2116
struct _tuple15 _tmp3EC=Cyc_CfFlowInfo_unname_rval(_tmp3FB);struct _tuple15 _tmp3ED=_tmp3EC;void*_tmp3F9;struct Cyc_List_List*_tmp3F8;_LL33: _tmp3F9=_tmp3ED.f1;_tmp3F8=_tmp3ED.f2;_LL34:;{
void*_tmp3EE=_tmp3F9;enum Cyc_CfFlowInfo_InitLevel _tmp3F7;struct Cyc_Absyn_Vardecl*_tmp3F6;void*_tmp3F5;switch(*((int*)_tmp3EE)){case 8U: _LL36: _tmp3F6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EE)->f1;_tmp3F5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3EE)->f2;_LL37:
# 2119
({void*_tmp3EF=0U;({struct _dyneither_ptr _tmp77F=({const char*_tmp3F0="anal_test: bad namedlocation";_tag_dyneither(_tmp3F0,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp77F,_tag_dyneither(_tmp3EF,sizeof(void*),0U));});});case 0U: _LL38: _LL39:
 return({struct _tuple19 _tmp61D;({union Cyc_CfFlowInfo_FlowInfo _tmp780=Cyc_CfFlowInfo_BottomFL();_tmp61D.f1=_tmp780;}),_tmp61D.f2=_tmp3FC;_tmp61D;});case 1U: _LL3A: _LL3B:
 goto _LL3D;case 5U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F:
 return({struct _tuple19 _tmp61E;_tmp61E.f1=_tmp3FC,({union Cyc_CfFlowInfo_FlowInfo _tmp781=Cyc_CfFlowInfo_BottomFL();_tmp61E.f2=_tmp781;});_tmp61E;});case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3EE)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40: _LL41:
 goto _LL43;}else{_LL46: _tmp3F7=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3EE)->f1;_LL47:
# 2129
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp3FC,e,_tmp3F7,_tmp3F8);}case 3U: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp3EE)->f1 == Cyc_CfFlowInfo_NoneIL){_LL42: _LL43:
# 2125
 goto _LL45;}else{_LL48: _LL49:
# 2130
 return({struct _tuple19 _tmp61F;_tmp61F.f1=_tmp3FC,_tmp61F.f2=_tmp3FC;_tmp61F;});}case 7U: _LL44: _LL45:
# 2127
({void*_tmp3F1=0U;({unsigned int _tmp783=e->loc;struct _dyneither_ptr _tmp782=({const char*_tmp3F2="expression may not be initialized";_tag_dyneither(_tmp3F2,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp783,_tmp782,_tag_dyneither(_tmp3F1,sizeof(void*),0U));});});
return({struct _tuple19 _tmp620;({union Cyc_CfFlowInfo_FlowInfo _tmp785=Cyc_CfFlowInfo_BottomFL();_tmp620.f1=_tmp785;}),({union Cyc_CfFlowInfo_FlowInfo _tmp784=Cyc_CfFlowInfo_BottomFL();_tmp620.f2=_tmp784;});_tmp620;});default: _LL4A: _LL4B:
# 2131
({void*_tmp3F3=0U;({struct _dyneither_ptr _tmp786=({const char*_tmp3F4="anal_test";_tag_dyneither(_tmp3F4,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp786,_tag_dyneither(_tmp3F3,sizeof(void*),0U));});});}_LL35:;};}}_LL2D:;};}}_LL0:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2137
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2139
struct _tuple27*_tmp409=ckenv;unsigned int _tmp415;struct Cyc_NewControlFlow_AnalEnv*_tmp414;struct Cyc_Dict_Dict _tmp413;_LL1: _tmp415=_tmp409->f1;_tmp414=_tmp409->f2;_tmp413=_tmp409->f3;_LL2:;{
void*_tmp40A=root;struct Cyc_Absyn_Vardecl*_tmp412;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp40A)->tag == 0U){_LL4: _tmp412=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp40A)->f1;_LL5:
# 2142
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp412->type)){
struct _tuple15 _tmp40B=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple15 _tmp40C=_tmp40B;void*_tmp411;_LL9: _tmp411=_tmp40C.f1;_LLA:;{
void*_tmp40D=_tmp411;switch(*((int*)_tmp40D)){case 7U: _LLC: _LLD:
 goto _LLF;case 0U: _LLE: _LLF:
 goto _LL11;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp40D)->f1 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12: _LL13:
# 2149
({struct Cyc_String_pa_PrintArg_struct _tmp410=({struct Cyc_String_pa_PrintArg_struct _tmp621;_tmp621.tag=0U,({struct _dyneither_ptr _tmp787=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp412->name));_tmp621.f1=_tmp787;});_tmp621;});void*_tmp40E[1U];_tmp40E[0]=& _tmp410;({unsigned int _tmp789=_tmp415;struct _dyneither_ptr _tmp788=({const char*_tmp40F="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp40F,sizeof(char),57U);});Cyc_Warn_warn(_tmp789,_tmp788,_tag_dyneither(_tmp40E,sizeof(void*),1U));});});
goto _LLB;}_LLB:;};}
# 2153
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 2158
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp416=flow;struct Cyc_Dict_Dict _tmp41B;if((_tmp416.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp41B=((_tmp416.ReachableFL).val).f1;_LL4:
# 2162
{struct Cyc_List_List*_tmp417=env->param_roots;for(0;_tmp417 != 0;_tmp417=_tmp417->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp78B=env->fenv;struct Cyc_Dict_Dict _tmp78A=_tmp41B;Cyc_CfFlowInfo_initlevel(_tmp78B,_tmp78A,Cyc_CfFlowInfo_lookup_place(_tmp41B,(struct Cyc_CfFlowInfo_Place*)_tmp417->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp418=0U;({unsigned int _tmp78D=loc;struct _dyneither_ptr _tmp78C=({const char*_tmp419="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp419,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp78D,_tmp78C,_tag_dyneither(_tmp418,sizeof(void*),0U));});});}}
# 2166
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp41A=({struct _tuple27 _tmp622;_tmp622.f1=loc,_tmp622.f2=env,_tmp622.f3=_tmp41B;_tmp622;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp41A,_tmp41B);}
# 2170
return;}_LL0:;}
# 2179
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2181
struct Cyc_List_List*_tmp41C=0;
struct Cyc_List_List*_tmp41D=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp41E=(struct _tuple24*)x->hd;struct _tuple24*_tmp41F=_tmp41E;struct Cyc_Absyn_Vardecl**_tmp427;struct Cyc_Absyn_Exp*_tmp426;_LL1: _tmp427=_tmp41F->f1;_tmp426=_tmp41F->f2;_LL2:;
if((_tmp427 != 0  && _tmp426 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp426->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp420=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->tag=0U,_tmp425->f1=*_tmp427;_tmp425;});
struct Cyc_CfFlowInfo_Place*_tmp421=({struct Cyc_CfFlowInfo_Place*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->root=(void*)_tmp420,_tmp424->path=0;_tmp424;});
({struct Cyc_List_List*_tmp78E=({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->hd=_tmp421,_tmp422->tl=_tmp41C;_tmp422;});_tmp41C=_tmp78E;});
({struct Cyc_List_List*_tmp78F=({struct Cyc_List_List*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->hd=_tmp426,_tmp423->tl=_tmp41D;_tmp423;});_tmp41D=_tmp78F;});}}}
# 2193
return({struct _tuple1 _tmp623;_tmp623.f1=_tmp41C,_tmp623.f2=_tmp41D;_tmp623;});}struct _tuple28{int f1;void*f2;};
# 2199
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2206
union Cyc_CfFlowInfo_FlowInfo _tmp428=flow;struct Cyc_Dict_Dict _tmp43C;if((_tmp428.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp429=0U;({struct _dyneither_ptr _tmp790=({const char*_tmp42A="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp42A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp790,_tag_dyneither(_tmp429,sizeof(void*),0U));});});}else{_LL3: _tmp43C=((_tmp428.ReachableFL).val).f1;_LL4: {
# 2214
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_exp_unsigned_one;
int _tmp42C=0;
int _tmp42D=1;
void*_tmp42E=Cyc_CfFlowInfo_lookup_place(_tmp43C,place);
void*_tmp42F=_tmp42E;
# 2226
int varok=0;
{void*_tmp430=_tmp42E;struct Cyc_Absyn_Vardecl*_tmp43B;void*_tmp43A;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp430)->tag == 8U){_LL6: _tmp43B=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp430)->f1;_tmp43A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp430)->f2;_LL7:
# 2229
 if(vd == _tmp43B){
_tmp42F=_tmp43A;
# 2232
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2234
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp42B,_tmp42D,_tmp42F,& _tmp42C)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp433=({struct Cyc_String_pa_PrintArg_struct _tmp624;_tmp624.tag=0U,({
# 2238
struct _dyneither_ptr _tmp791=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp624.f1=_tmp791;});_tmp624;});void*_tmp431[1U];_tmp431[0]=& _tmp433;({unsigned int _tmp793=loc;struct _dyneither_ptr _tmp792=({const char*_tmp432="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp432,sizeof(char),58U);});Cyc_CfFlowInfo_aerr(_tmp793,_tmp792,_tag_dyneither(_tmp431,sizeof(void*),1U));});});}else{
# 2241
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp43C,_tmp42F)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp436=({struct Cyc_String_pa_PrintArg_struct _tmp625;_tmp625.tag=0U,({
# 2244
struct _dyneither_ptr _tmp794=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp625.f1=_tmp794;});_tmp625;});void*_tmp434[1U];_tmp434[0]=& _tmp436;({unsigned int _tmp796=loc;struct _dyneither_ptr _tmp795=({const char*_tmp435="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp435,sizeof(char),84U);});Cyc_CfFlowInfo_aerr(_tmp796,_tmp795,_tag_dyneither(_tmp434,sizeof(void*),1U));});});else{
# 2246
varok=1;}}}else{
# 2249
varok=1;}}else{
# 2252
goto _LL9;}
goto _LL5;}else{_LL8: _LL9:
# 2256
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp439=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({
# 2261
struct _dyneither_ptr _tmp797=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp626.f1=_tmp797;});_tmp626;});void*_tmp437[1U];_tmp437[0]=& _tmp439;({unsigned int _tmp799=loc;struct _dyneither_ptr _tmp798=({const char*_tmp438="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp438,sizeof(char),85U);});Cyc_CfFlowInfo_aerr(_tmp799,_tmp798,_tag_dyneither(_tmp437,sizeof(void*),1U));});});}
goto _LL5;}_LL5:;}
# 2268
return({struct _tuple28 _tmp627;_tmp627.f1=varok,_tmp627.f2=_tmp42F;_tmp627;});}}_LL0:;}
# 2274
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp43D=p;void*_tmp445;struct Cyc_List_List*_tmp444;_LL1: _tmp445=_tmp43D->root;_tmp444=_tmp43D->path;_LL2:;
if(_tmp444 != 0)
({void*_tmp43E=0U;({struct _dyneither_ptr _tmp79A=({const char*_tmp43F="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp43F,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp79A,_tag_dyneither(_tmp43E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp440=_tmp445;struct Cyc_Absyn_Vardecl*_tmp443;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp440)->tag == 0U){_LL4: _tmp443=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp440)->f1;_LL5:
 return _tmp443;}else{_LL6: _LL7:
({void*_tmp441=0U;({struct _dyneither_ptr _tmp79B=({const char*_tmp442="unconsume_params: root is not a varroot";_tag_dyneither(_tmp442,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp79B,_tag_dyneither(_tmp441,sizeof(void*),0U));});});}_LL3:;};}
# 2293 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2301
{union Cyc_CfFlowInfo_FlowInfo _tmp446=flow;struct Cyc_Dict_Dict _tmp45D;if((_tmp446.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _tmp45D=((_tmp446.ReachableFL).val).f1;_LL4: {
# 2308
struct _tuple18 _tmp447=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp448=_tmp447;union Cyc_CfFlowInfo_FlowInfo _tmp45C;union Cyc_CfFlowInfo_AbsLVal _tmp45B;_LL6: _tmp45C=_tmp448.f1;_tmp45B=_tmp448.f2;_LL7:;
# 2311
{union Cyc_CfFlowInfo_AbsLVal _tmp449=_tmp45B;struct Cyc_CfFlowInfo_Place*_tmp45A;if((_tmp449.PlaceL).tag == 1){_LL9: _tmp45A=(_tmp449.PlaceL).val;_LLA: {
# 2315
void*_tmp44A=Cyc_CfFlowInfo_lookup_place(_tmp45D,_tmp45A);
{void*_tmp44B=_tmp44A;struct Cyc_Absyn_Vardecl*_tmp459;void*_tmp458;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp44B)->tag == 8U){_LLE: _tmp459=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp44B)->f1;_tmp458=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp44B)->f2;_LLF: {
# 2321
void*new_rval;
if(_tmp459 == vd){
# 2325
if(varok){
# 2327
({void*_tmp79C=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp458);_tmp44A=_tmp79C;});
# 2332
if(ropt != 0){
# 2338
struct _tuple16 _tmp44C=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp79E=env->fenv;struct _tuple16 _tmp79D=({struct _tuple16 _tmp628;_tmp628.f1=_tmp45C,_tmp628.f2=_tmp44A;_tmp628;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp79E,_tmp79D,({struct _tuple16 _tmp629;_tmp629.f1=_tmp45C,_tmp629.f2=ropt;_tmp629;}));});
# 2338
struct _tuple16 _tmp44D=_tmp44C;union Cyc_CfFlowInfo_FlowInfo _tmp44F;void*_tmp44E;_LL13: _tmp44F=_tmp44D.f1;_tmp44E=_tmp44D.f2;_LL14:;
# 2342
_tmp45C=_tmp44F;new_rval=_tmp44E;}else{
# 2347
new_rval=_tmp44A;}}else{
# 2350
new_rval=_tmp458;}
# 2352
{union Cyc_CfFlowInfo_FlowInfo _tmp450=_tmp45C;struct Cyc_Dict_Dict _tmp454;struct Cyc_List_List*_tmp453;if((_tmp450.ReachableFL).tag == 2){_LL16: _tmp454=((_tmp450.ReachableFL).val).f1;_tmp453=((_tmp450.ReachableFL).val).f2;_LL17:
# 2354
({union Cyc_CfFlowInfo_FlowInfo _tmp7A0=({struct Cyc_Dict_Dict _tmp79F=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp454,_tmp45A,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp79F,_tmp453);});flow=_tmp7A0;});
# 2358
goto _LL15;}else{_LL18: _LL19:
# 2360
({void*_tmp451=0U;({struct _dyneither_ptr _tmp7A1=({const char*_tmp452="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp452,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7A1,_tag_dyneither(_tmp451,sizeof(void*),0U));});});}_LL15:;}
# 2362
goto _LLD;}else{
# 2364
goto _LL11;}
goto _LLD;}}else{_LL10: _LL11:
# 2371
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp457=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,({
struct _dyneither_ptr _tmp7A2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(unconsume_exp));_tmp62A.f1=_tmp7A2;});_tmp62A;});void*_tmp455[1U];_tmp455[0]=& _tmp457;({unsigned int _tmp7A4=loc;struct _dyneither_ptr _tmp7A3=({const char*_tmp456="aliased expression %s was overwritten";_tag_dyneither(_tmp456,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7A4,_tmp7A3,_tag_dyneither(_tmp455,sizeof(void*),1U));});});
# 2388 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2390
goto _LL8;}}else{_LLB: _LLC:
# 2396
 goto _LL8;}_LL8:;}
# 2398
goto _LL0;}}_LL0:;}
# 2400
return flow;}
# 2413 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2419
{union Cyc_CfFlowInfo_FlowInfo _tmp45E=flow;if((_tmp45E.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2423
int _tmp45F=unconsume_exps != 0;
struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);
struct Cyc_List_List*_tmp461=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);
{struct Cyc_List_List*_tmp462=_tmp460;for(0;_tmp462 != 0;(
_tmp462=_tmp462->tl,
_tmp461 != 0?_tmp461=_tmp461->tl: 0)){
# 2432
struct Cyc_Absyn_Vardecl*_tmp463=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp462->hd);
struct _tuple28 _tmp464=
is_region_open?({struct _tuple28 _tmp62B;_tmp62B.f1=1,_tmp62B.f2=0;_tmp62B;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp462->hd,_tmp45F,_tmp463,flow,loc);
# 2433
struct _tuple28 _tmp465=_tmp464;int _tmp467;void*_tmp466;_LL6: _tmp467=_tmp465.f1;_tmp466=_tmp465.f2;_LL7:;
# 2439
if(_tmp45F)
({union Cyc_CfFlowInfo_FlowInfo _tmp7A5=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp461))->hd,_tmp463,_tmp467,_tmp466,flow,loc);flow=_tmp7A5;});}}
# 2442
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2446
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2448
struct Cyc_CfFlowInfo_FlowEnv*_tmp468=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp469=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp46A=_tmp469;struct Cyc_Core_Opt*_tmp47E;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_Absyn_Stmt*_tmp47C;unsigned int _tmp47B;_LL1: _tmp47E=_tmp46A->pat_vars;_tmp47D=_tmp46A->where_clause;_tmp47C=_tmp46A->body;_tmp47B=_tmp46A->loc;_LL2:;{
struct _tuple1 _tmp46B=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47E))->v);struct _tuple1 _tmp46C=_tmp46B;struct Cyc_List_List*_tmp47A;struct Cyc_List_List*_tmp479;_LL4: _tmp47A=_tmp46C.f1;_tmp479=_tmp46C.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp47E->v,_tmp47A != 0,_tmp47B,exp_loc);
# 2456
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp46D=env->unique_pat_vars;
# 2459
if(Cyc_Tcpat_has_vars(_tmp47E))
({struct Cyc_List_List*_tmp7A7=({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*_tmp46F));
({struct _tuple29*_tmp7A6=({struct _tuple29*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->f1=0,_tmp46E->f2=_tmp47C,_tmp46E->f3=_tmp47A,_tmp46E->f4=_tmp479;_tmp46E;});_tmp46F->hd=_tmp7A6;}),_tmp46F->tl=_tmp46D;_tmp46F;});
# 2460
env->unique_pat_vars=_tmp7A7;});
# 2463
if(_tmp47D != 0){
struct Cyc_Absyn_Exp*wexp=_tmp47D;
struct _tuple19 _tmp470=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp471=_tmp470;union Cyc_CfFlowInfo_FlowInfo _tmp473;union Cyc_CfFlowInfo_FlowInfo _tmp472;_LL7: _tmp473=_tmp471.f1;_tmp472=_tmp471.f2;_LL8:;
inflow=_tmp472;
({union Cyc_CfFlowInfo_FlowInfo _tmp7A8=Cyc_NewControlFlow_anal_stmt(env,_tmp473,_tmp47C,0);clause_outflow=_tmp7A8;});}else{
# 2469
({union Cyc_CfFlowInfo_FlowInfo _tmp7A9=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp47C,0);clause_outflow=_tmp7A9;});}
# 2471
env->unique_pat_vars=_tmp46D;{
union Cyc_CfFlowInfo_FlowInfo _tmp474=clause_outflow;if((_tmp474.BottomFL).tag == 1){_LLA: _LLB:
 goto _LL9;}else{_LLC: _LLD:
# 2476
({union Cyc_CfFlowInfo_FlowInfo _tmp7AA=Cyc_NewControlFlow_unconsume_params(env,_tmp47A,_tmp479,0,clause_outflow,_tmp47B);clause_outflow=_tmp7AA;});
# 2478
if(scs->tl == 0)
return clause_outflow;else{
# 2483
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp475=0U;({unsigned int _tmp7AC=_tmp47C->loc;struct _dyneither_ptr _tmp7AB=({const char*_tmp476="switch clause may implicitly fallthru";_tag_dyneither(_tmp476,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7AC,_tmp7AB,_tag_dyneither(_tmp475,sizeof(void*),0U));});});else{
# 2486
({void*_tmp477=0U;({unsigned int _tmp7AE=_tmp47C->loc;struct _dyneither_ptr _tmp7AD=({const char*_tmp478="switch clause may implicitly fallthru";_tag_dyneither(_tmp478,sizeof(char),38U);});Cyc_Warn_warn(_tmp7AE,_tmp7AD,_tag_dyneither(_tmp477,sizeof(void*),0U));});});}
# 2488
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2490
goto _LL9;}_LL9:;};};};}
# 2493
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2498
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2500
struct _tuple30*_tmp47F=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp48A;struct Cyc_Dict_Dict _tmp489;unsigned int _tmp488;_LL1: _tmp48A=_tmp47F->f1;_tmp489=_tmp47F->f2;_tmp488=_tmp47F->f3;_LL2:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2503
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp62D;_tmp62D.tag=0U,_tmp62D.f1=vd;_tmp62D;});
struct _tuple15 _tmp480=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp7AF=_tmp489;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp7AF,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp487=_cycalloc(sizeof(*_tmp487));*_tmp487=vdroot;_tmp487;}));}));struct _tuple15 _tmp481=_tmp480;void*_tmp486;_LL4: _tmp486=_tmp481.f1;_LL5:;{
void*_tmp482=_tmp486;switch(*((int*)_tmp482)){case 7U: _LL7: _LL8:
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp482)->f1 == Cyc_CfFlowInfo_NoneIL){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
# 2510
({struct Cyc_String_pa_PrintArg_struct _tmp485=({struct Cyc_String_pa_PrintArg_struct _tmp62C;_tmp62C.tag=0U,({struct _dyneither_ptr _tmp7B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp62C.f1=_tmp7B0;});_tmp62C;});void*_tmp483[1U];_tmp483[0]=& _tmp485;({unsigned int _tmp7B2=_tmp488;struct _dyneither_ptr _tmp7B1=({const char*_tmp484="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp484,sizeof(char),68U);});Cyc_Warn_warn(_tmp7B2,_tmp7B1,_tag_dyneither(_tmp483,sizeof(void*),1U));});});
# 2512
goto _LL6;}_LL6:;};}}
# 2517
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp48B=inflow;struct Cyc_Dict_Dict _tmp497;if((_tmp48B.ReachableFL).tag == 2){_LL1: _tmp497=((_tmp48B.ReachableFL).val).f1;_LL2: {
# 2520
struct _tuple30 _tmp48C=({struct _tuple30 _tmp62E;_tmp62E.f1=env,_tmp62E.f2=_tmp497,_tmp62E.f3=decl->loc;_tmp62E;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp48D=env->fenv;
{void*_tmp48E=decl->r;void*_tmp48F=_tmp48E;struct Cyc_List_List*_tmp496;struct Cyc_List_List*_tmp495;struct Cyc_Absyn_Vardecl*_tmp494;switch(*((int*)_tmp48F)){case 0U: _LL6: _tmp494=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp48F)->f1;_LL7:
# 2524
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp48C,_tmp494);
goto _LL5;case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48F)->f2 != 0){_LL8: _tmp495=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp48F)->f2)->v;_LL9: {
# 2527
struct _tuple1 _tmp490=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp495);struct _tuple1 _tmp491=_tmp490;struct Cyc_List_List*_tmp493;_LLF: _tmp493=_tmp491.f1;_LL10:;{
struct Cyc_List_List*_tmp492=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp493);
_tmp496=_tmp492;goto _LLB;};}}else{goto _LLC;}case 3U: _LLA: _tmp496=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp48F)->f1;_LLB:
# 2531
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp48C,_tmp496);
goto _LL5;default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 2535
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2538
return;}
# 2544
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2548
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp498=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp499=_tmp498;int _tmp49D;struct Cyc_Absyn_Stmt*_tmp49C;struct Cyc_List_List*_tmp49B;struct Cyc_List_List*_tmp49A;_LL1: _tmp49D=_tmp499->f1;_tmp49C=_tmp499->f2;_tmp49B=_tmp499->f3;_tmp49A=_tmp499->f4;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7B3=Cyc_NewControlFlow_unconsume_params(env,_tmp49B,_tmp49A,_tmp49D,inflow,dest->loc);inflow=_tmp7B3;});}}
# 2553
return inflow;}
# 2559
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2561
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp49E=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp49F=_tmp49E;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp53B;union Cyc_CfFlowInfo_FlowInfo*_tmp53A;_LL1: _tmp53B=_tmp49F.f1;_tmp53A=_tmp49F.f2;_LL2:;
inflow=*_tmp53A;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp4A0=env->fenv;
# 2568
void*_tmp4A1=s->r;void*_tmp4A2=_tmp4A1;struct Cyc_Absyn_Stmt*_tmp539;struct Cyc_Absyn_Decl*_tmp538;struct Cyc_Absyn_Stmt*_tmp537;struct Cyc_Absyn_Vardecl*_tmp536;struct Cyc_Absyn_Exp*_tmp535;unsigned int _tmp534;struct Cyc_Absyn_Stmt*_tmp533;struct Cyc_List_List*_tmp532;struct Cyc_Absyn_Exp*_tmp531;unsigned int _tmp530;struct Cyc_Absyn_Stmt*_tmp52F;struct Cyc_Absyn_Stmt*_tmp52E;struct Cyc_List_List*_tmp52D;void*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_List_List*_tmp52A;void*_tmp529;struct Cyc_List_List*_tmp528;struct Cyc_Absyn_Switch_clause*_tmp527;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Stmt*_tmp524;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Stmt*_tmp522;struct Cyc_Absyn_Stmt*_tmp521;struct Cyc_Absyn_Stmt*_tmp520;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Stmt*_tmp51E;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Stmt*_tmp51C;struct Cyc_Absyn_Stmt*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Stmt*_tmp519;struct Cyc_Absyn_Stmt*_tmp518;struct Cyc_Absyn_Stmt*_tmp517;struct Cyc_Absyn_Stmt*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp514;switch(*((int*)_tmp4A2)){case 0U: _LL4: _LL5:
 return inflow;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1 == 0){_LL6: _LL7:
# 2572
 if(env->noreturn)
({void*_tmp4A3=0U;({unsigned int _tmp7B5=s->loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp4A4="`noreturn' function might return";_tag_dyneither(_tmp4A4,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp4A3,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL8: _tmp514=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_LL9:
# 2578
 if(env->noreturn)
({void*_tmp4A5=0U;({unsigned int _tmp7B7=s->loc;struct _dyneither_ptr _tmp7B6=({const char*_tmp4A6="`noreturn' function might return";_tag_dyneither(_tmp4A6,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7B7,_tmp7B6,_tag_dyneither(_tmp4A5,sizeof(void*),0U));});});{
struct _tuple16 _tmp4A7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp514));struct _tuple16 _tmp4A8=_tmp4A7;union Cyc_CfFlowInfo_FlowInfo _tmp4AA;void*_tmp4A9;_LL2D: _tmp4AA=_tmp4A8.f1;_tmp4A9=_tmp4A8.f2;_LL2E:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7B8=Cyc_NewControlFlow_use_Rval(env,_tmp514->loc,_tmp4AA,_tmp4A9);_tmp4AA=_tmp7B8;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp4AA);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp4AA,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1U: _LLA: _tmp515=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_LLB: {
# 2587
struct _tuple17*_tmp4AB=rval_opt;void**_tmp4B1;int _tmp4B0;if(_tmp4AB != 0){_LL30: _tmp4B1=(void**)& _tmp4AB->f1;_tmp4B0=_tmp4AB->f2;_LL31: {
# 2589
struct _tuple16 _tmp4AC=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B0,inflow,_tmp515);struct _tuple16 _tmp4AD=_tmp4AC;union Cyc_CfFlowInfo_FlowInfo _tmp4AF;void*_tmp4AE;_LL35: _tmp4AF=_tmp4AD.f1;_tmp4AE=_tmp4AD.f2;_LL36:;
*_tmp4B1=_tmp4AE;
return _tmp4AF;}}else{_LL32: _LL33:
# 2593
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp515)).f1;}_LL2F:;}case 2U: _LLC: _tmp517=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp516=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LLD:
# 2597
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp7BB=env;union Cyc_CfFlowInfo_FlowInfo _tmp7BA=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp517,0);struct Cyc_Absyn_Stmt*_tmp7B9=_tmp516;Cyc_NewControlFlow_anal_stmt(_tmp7BB,_tmp7BA,_tmp7B9,rval_opt);});case 4U: _LLE: _tmp51A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp519=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_tmp518=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3;_LLF: {
# 2600
struct _tuple19 _tmp4B2=Cyc_NewControlFlow_anal_test(env,inflow,_tmp51A);struct _tuple19 _tmp4B3=_tmp4B2;union Cyc_CfFlowInfo_FlowInfo _tmp4B7;union Cyc_CfFlowInfo_FlowInfo _tmp4B6;_LL38: _tmp4B7=_tmp4B3.f1;_tmp4B6=_tmp4B3.f2;_LL39:;{
# 2607
union Cyc_CfFlowInfo_FlowInfo _tmp4B4=Cyc_NewControlFlow_anal_stmt(env,_tmp4B6,_tmp518,0);
union Cyc_CfFlowInfo_FlowInfo _tmp4B5=Cyc_NewControlFlow_anal_stmt(env,_tmp4B7,_tmp519,0);
return Cyc_CfFlowInfo_join_flow(_tmp4A0,_tmp4B5,_tmp4B4);};}case 5U: _LL10: _tmp51D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1).f1;_tmp51C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1).f2;_tmp51B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL11: {
# 2615
struct _tuple20 _tmp4B8=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp51C);struct _tuple20 _tmp4B9=_tmp4B8;union Cyc_CfFlowInfo_FlowInfo*_tmp4C0;_LL3B: _tmp4C0=_tmp4B9.f2;_LL3C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4BA=*_tmp4C0;
struct _tuple19 _tmp4BB=Cyc_NewControlFlow_anal_test(env,_tmp4BA,_tmp51D);struct _tuple19 _tmp4BC=_tmp4BB;union Cyc_CfFlowInfo_FlowInfo _tmp4BF;union Cyc_CfFlowInfo_FlowInfo _tmp4BE;_LL3E: _tmp4BF=_tmp4BC.f1;_tmp4BE=_tmp4BC.f2;_LL3F:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4BD=Cyc_NewControlFlow_anal_stmt(env,_tmp4BF,_tmp51B,0);
Cyc_NewControlFlow_update_flow(env,_tmp51C,_tmp4BD);
return _tmp4BE;};};}case 14U: _LL12: _tmp520=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp51F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f1;_tmp51E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f2;_LL13: {
# 2625
union Cyc_CfFlowInfo_FlowInfo _tmp4C1=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp520,0);
struct _tuple20 _tmp4C2=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4C1,_tmp51E);struct _tuple20 _tmp4C3=_tmp4C2;union Cyc_CfFlowInfo_FlowInfo*_tmp4C9;_LL41: _tmp4C9=_tmp4C3.f2;_LL42:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4C4=*_tmp4C9;
struct _tuple19 _tmp4C5=Cyc_NewControlFlow_anal_test(env,_tmp4C4,_tmp51F);struct _tuple19 _tmp4C6=_tmp4C5;union Cyc_CfFlowInfo_FlowInfo _tmp4C8;union Cyc_CfFlowInfo_FlowInfo _tmp4C7;_LL44: _tmp4C8=_tmp4C6.f1;_tmp4C7=_tmp4C6.f2;_LL45:;
Cyc_NewControlFlow_update_flow(env,_tmp520,_tmp4C8);
return _tmp4C7;};}case 9U: _LL14: _tmp526=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp525=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f1;_tmp524=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2).f2;_tmp523=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3).f1;_tmp522=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3).f2;_tmp521=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f4;_LL15: {
# 2634
union Cyc_CfFlowInfo_FlowInfo _tmp4CA=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp526)).f1;
struct _tuple20 _tmp4CB=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4CA,_tmp524);struct _tuple20 _tmp4CC=_tmp4CB;union Cyc_CfFlowInfo_FlowInfo*_tmp4D8;_LL47: _tmp4D8=_tmp4CC.f2;_LL48:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4CD=*_tmp4D8;
struct _tuple19 _tmp4CE=Cyc_NewControlFlow_anal_test(env,_tmp4CD,_tmp525);struct _tuple19 _tmp4CF=_tmp4CE;union Cyc_CfFlowInfo_FlowInfo _tmp4D7;union Cyc_CfFlowInfo_FlowInfo _tmp4D6;_LL4A: _tmp4D7=_tmp4CF.f1;_tmp4D6=_tmp4CF.f2;_LL4B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D0=Cyc_NewControlFlow_anal_stmt(env,_tmp4D7,_tmp521,0);
struct _tuple20 _tmp4D1=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4D0,_tmp522);struct _tuple20 _tmp4D2=_tmp4D1;union Cyc_CfFlowInfo_FlowInfo*_tmp4D5;_LL4D: _tmp4D5=_tmp4D2.f2;_LL4E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D3=*_tmp4D5;
union Cyc_CfFlowInfo_FlowInfo _tmp4D4=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp4D3,_tmp523)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp524,_tmp4D4);
return _tmp4D6;};};};}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2 != 0){_LL16: _tmp528=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp527=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL17: {
# 2646
struct _tuple23 _tmp4D9=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp528,1,1);struct _tuple23 _tmp4DA=_tmp4D9;union Cyc_CfFlowInfo_FlowInfo _tmp4E1;struct Cyc_List_List*_tmp4E0;_LL50: _tmp4E1=_tmp4DA.f1;_tmp4E0=_tmp4DA.f2;_LL51:;
# 2648
({union Cyc_CfFlowInfo_FlowInfo _tmp7BC=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp527->body);inflow=_tmp7BC;});{
# 2650
struct Cyc_List_List*_tmp4DB=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp527->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp7BD=Cyc_NewControlFlow_add_vars(_tmp4A0,_tmp4E1,_tmp4DB,_tmp4A0->unknown_all,s->loc,0);_tmp4E1=_tmp7BD;});
# 2653
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp527->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4DC=(struct _tuple24*)x->hd;struct _tuple24*_tmp4DD=_tmp4DC;struct Cyc_Absyn_Vardecl**_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;_LL53: _tmp4DF=_tmp4DD->f1;_tmp4DE=_tmp4DD->f2;_LL54:;
if(_tmp4DF != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp7C4=({struct Cyc_CfFlowInfo_FlowEnv*_tmp7C3=_tmp4A0;struct Cyc_NewControlFlow_AnalEnv*_tmp7C2=env;union Cyc_CfFlowInfo_FlowInfo _tmp7C1=_tmp4E1;struct Cyc_Absyn_Vardecl*_tmp7C0=*_tmp4DF;struct Cyc_Absyn_Exp*_tmp7BF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp528))->hd;void*_tmp7BE=(void*)((struct Cyc_List_List*)_check_null(_tmp4E0))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp7C3,_tmp7C2,_tmp7C1,_tmp7C0,_tmp7BF,_tmp7BE,s->loc);});_tmp4E1=_tmp7C4;});
_tmp4E0=_tmp4E0->tl;
_tmp528=_tmp528->tl;}}}
# 2661
Cyc_NewControlFlow_update_flow(env,_tmp527->body,_tmp4E1);
return Cyc_CfFlowInfo_BottomFL();};}}else{_LL2A: _LL2B:
# 2828
({void*_tmp512=0U;({struct _dyneither_ptr _tmp7C5=({const char*_tmp513="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp513,sizeof(char),54U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7C5,_tag_dyneither(_tmp512,sizeof(void*),0U));});});}case 6U: _LL18: _LL19:
# 2667
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp4E2=inflow;if((_tmp4E2.ReachableFL).tag == 2){_LL56: _LL57:
# 2670
{void*_tmp4E3=Cyc_Tcutil_compress(env->return_type);void*_tmp4E4=_tmp4E3;switch(*((int*)_tmp4E4)){case 0U: _LL5B: _LL5C:
 goto _LL5A;case 7U: _LL5D: _LL5E:
 goto _LL60;case 6U: _LL5F: _LL60:
# 2674
({void*_tmp4E5=0U;({unsigned int _tmp7C7=s->loc;struct _dyneither_ptr _tmp7C6=({const char*_tmp4E6="break may cause function not to return a value";_tag_dyneither(_tmp4E6,sizeof(char),47U);});Cyc_Warn_warn(_tmp7C7,_tmp7C6,_tag_dyneither(_tmp4E5,sizeof(void*),0U));});});
goto _LL5A;default: _LL61: _LL62:
# 2677
({void*_tmp4E7=0U;({unsigned int _tmp7C9=s->loc;struct _dyneither_ptr _tmp7C8=({const char*_tmp4E8="break may cause function not to return a value";_tag_dyneither(_tmp4E8,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp7C9,_tmp7C8,_tag_dyneither(_tmp4E7,sizeof(void*),0U));});});
goto _LL5A;}_LL5A:;}
# 2680
goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 2683
if(env->noreturn)
({void*_tmp4E9=0U;({unsigned int _tmp7CB=s->loc;struct _dyneither_ptr _tmp7CA=({const char*_tmp4EA="`noreturn' function might return";_tag_dyneither(_tmp4EA,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7CB,_tmp7CA,_tag_dyneither(_tmp4E9,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2689
goto _LL1B;case 7U: _LL1A: _LL1B:
 goto _LL1D;case 8U: _LL1C: _LL1D: {
# 2693
struct Cyc_Absyn_Stmt*_tmp4EB=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp4EB == 0)
({void*_tmp4EC=0U;({unsigned int _tmp7CD=s->loc;struct _dyneither_ptr _tmp7CC=({const char*_tmp4ED="jump has no target (should be impossible)";_tag_dyneither(_tmp4ED,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp7CD,_tmp7CC,_tag_dyneither(_tmp4EC,sizeof(void*),0U));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp7CE=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp4EB));inflow=_tmp7CE;});
# 2698
Cyc_NewControlFlow_update_flow(env,_tmp4EB,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10U: _LL1E: _tmp52B=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp52A=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_tmp529=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3;_LL1F:
# 2704
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp52A,_tmp52B->loc);case 15U: _LL20: _tmp52E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp52D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_tmp52C=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f3;_LL21: {
# 2709
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp52E,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2717
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2720
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp52D,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp4EE=scs_outflow;if((_tmp4EE.BottomFL).tag == 1){_LL64: _LL65:
 goto _LL63;}else{_LL66: _LL67:
({void*_tmp4EF=0U;({unsigned int _tmp7D0=s->loc;struct _dyneither_ptr _tmp7CF=({const char*_tmp4F0="last catch clause may implicitly fallthru";_tag_dyneither(_tmp4F0,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp7D0,_tmp7CF,_tag_dyneither(_tmp4EF,sizeof(void*),0U));});});}_LL63:;}
# 2726
outflow=s1_outflow;
# 2728
return outflow;};};}case 12U: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)){case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f2 != 0){_LL22: _tmp532=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f2)->v;_tmp531=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f3;_tmp530=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->loc;_tmp52F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL23: {
# 2738
struct _tuple1 _tmp4F1=Cyc_NewControlFlow_get_unconsume_pat_vars(_tmp532);struct _tuple1 _tmp4F2=_tmp4F1;struct Cyc_List_List*_tmp4F7;struct Cyc_List_List*_tmp4F6;_LL69: _tmp4F7=_tmp4F2.f1;_tmp4F6=_tmp4F2.f2;_LL6A:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7D1=Cyc_NewControlFlow_initialize_pat_vars(_tmp4A0,env,inflow,_tmp532,_tmp4F7 != 0,_tmp530,_tmp531->loc);inflow=_tmp7D1;});{
struct Cyc_List_List*_tmp4F3=env->unique_pat_vars;
# 2742
({struct Cyc_List_List*_tmp7D3=({struct Cyc_List_List*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));
({struct _tuple29*_tmp7D2=({struct _tuple29*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->f1=0,_tmp4F4->f2=s,_tmp4F4->f3=_tmp4F7,_tmp4F4->f4=_tmp4F6;_tmp4F4;});_tmp4F5->hd=_tmp7D2;}),_tmp4F5->tl=_tmp4F3;_tmp4F5;});
# 2742
env->unique_pat_vars=_tmp7D3;});
# 2747
({union Cyc_CfFlowInfo_FlowInfo _tmp7D4=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp52F,rval_opt);inflow=_tmp7D4;});
env->unique_pat_vars=_tmp4F3;
# 2751
({union Cyc_CfFlowInfo_FlowInfo _tmp7D5=Cyc_NewControlFlow_unconsume_params(env,_tmp4F7,_tmp4F6,0,inflow,_tmp530);inflow=_tmp7D5;});
# 2755
return inflow;};}}else{goto _LL26;}case 4U: _LL24: _tmp536=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f2;_tmp535=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->r)->f3;_tmp534=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1)->loc;_tmp533=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;if(_tmp535 != 0){_LL25: {
# 2767
struct Cyc_List_List l=({struct Cyc_List_List _tmp630;_tmp630.hd=_tmp535,_tmp630.tl=0;_tmp630;});
union Cyc_CfFlowInfo_FlowInfo _tmp4F8=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp4F9=Cyc_NewControlFlow_anal_Lexp(env,_tmp4F8,0,0,_tmp535);struct _tuple18 _tmp4FA=_tmp4F9;union Cyc_CfFlowInfo_AbsLVal _tmp511;_LL6C: _tmp511=_tmp4FA.f2;_LL6D:;{
struct _tuple16 _tmp4FB=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp4F8,_tmp535);struct _tuple16 _tmp4FC=_tmp4FB;union Cyc_CfFlowInfo_FlowInfo _tmp510;_LL6F: _tmp510=_tmp4FC.f1;_LL70:;{
struct Cyc_List_List*_tmp4FD=0;
struct Cyc_List_List*_tmp4FE=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp4FF=_tmp510;struct Cyc_Dict_Dict _tmp50B;struct Cyc_List_List*_tmp50A;if((_tmp4FF.ReachableFL).tag == 2){_LL72: _tmp50B=((_tmp4FF.ReachableFL).val).f1;_tmp50A=((_tmp4FF.ReachableFL).val).f2;_LL73:
# 2775
{union Cyc_CfFlowInfo_AbsLVal _tmp500=_tmp511;struct Cyc_CfFlowInfo_Place*_tmp509;if((_tmp500.PlaceL).tag == 1){_LL77: _tmp509=(_tmp500.PlaceL).val;_LL78: {
# 2779
void*_tmp501=Cyc_CfFlowInfo_lookup_place(_tmp50B,_tmp509);
({void*_tmp7D6=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->tag=8U,_tmp502->f1=_tmp536,_tmp502->f2=_tmp501;_tmp502;});_tmp501=_tmp7D6;});
({struct Cyc_Dict_Dict _tmp7D7=Cyc_CfFlowInfo_assign_place(_tmp4A0,_tmp535->loc,_tmp50B,_tmp509,_tmp501);_tmp50B=_tmp7D7;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7D8=Cyc_CfFlowInfo_ReachableFL(_tmp50B,_tmp50A);_tmp510=_tmp7D8;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp503=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508->tag=0U,_tmp508->f1=_tmp536;_tmp508;});
struct Cyc_CfFlowInfo_Place*_tmp504=({struct Cyc_CfFlowInfo_Place*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->root=(void*)_tmp503,_tmp507->path=0;_tmp507;});
({struct Cyc_List_List*_tmp7D9=({struct Cyc_List_List*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->hd=_tmp504,_tmp505->tl=_tmp4FD;_tmp505;});_tmp4FD=_tmp7D9;});
({struct Cyc_List_List*_tmp7DA=({struct Cyc_List_List*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->hd=_tmp535,_tmp506->tl=_tmp4FE;_tmp506;});_tmp4FE=_tmp7DA;});
goto _LL76;};}}else{_LL79: _LL7A:
# 2794
 goto _LL76;}_LL76:;}
# 2796
goto _LL71;}else{_LL74: _LL75:
# 2798
 goto _LL71;}_LL71:;}{
# 2801
struct Cyc_List_List _tmp50C=({struct Cyc_List_List _tmp62F;_tmp62F.hd=_tmp536,_tmp62F.tl=0;_tmp62F;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7DB=Cyc_NewControlFlow_add_vars(_tmp4A0,_tmp510,& _tmp50C,_tmp4A0->unknown_all,_tmp534,0);_tmp510=_tmp7DB;});{
# 2805
struct Cyc_List_List*_tmp50D=env->unique_pat_vars;
({struct Cyc_List_List*_tmp7DD=({struct Cyc_List_List*_tmp50F=_cycalloc(sizeof(*_tmp50F));({struct _tuple29*_tmp7DC=({struct _tuple29*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=1,_tmp50E->f2=s,_tmp50E->f3=_tmp4FD,_tmp50E->f4=_tmp4FE;_tmp50E;});_tmp50F->hd=_tmp7DC;}),_tmp50F->tl=_tmp50D;_tmp50F;});env->unique_pat_vars=_tmp7DD;});
# 2810
({union Cyc_CfFlowInfo_FlowInfo _tmp7DE=Cyc_NewControlFlow_anal_stmt(env,_tmp510,_tmp533,rval_opt);_tmp510=_tmp7DE;});
env->unique_pat_vars=_tmp50D;
# 2814
({union Cyc_CfFlowInfo_FlowInfo _tmp7DF=Cyc_NewControlFlow_unconsume_params(env,_tmp4FD,_tmp4FE,1,_tmp510,_tmp534);_tmp510=_tmp7DF;});
# 2818
return _tmp510;};};};};}}else{goto _LL26;}default: _LL26: _tmp538=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f1;_tmp537=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL27:
# 2821
({union Cyc_CfFlowInfo_FlowInfo _tmp7E3=({struct Cyc_NewControlFlow_AnalEnv*_tmp7E2=env;union Cyc_CfFlowInfo_FlowInfo _tmp7E1=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp538);struct Cyc_Absyn_Stmt*_tmp7E0=_tmp537;Cyc_NewControlFlow_anal_stmt(_tmp7E2,_tmp7E1,_tmp7E0,rval_opt);});outflow=_tmp7E3;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp538);
return outflow;}default: _LL28: _tmp539=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp4A2)->f2;_LL29:
# 2826
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp539,rval_opt);}_LL3:;};}
# 2832
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2836
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp53C=env->fenv;
void*_tmp53D=decl->r;void*_tmp53E=_tmp53D;struct Cyc_Absyn_Tvar*_tmp551;struct Cyc_Absyn_Vardecl*_tmp550;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Fndecl*_tmp54E;struct Cyc_List_List*_tmp54D;struct Cyc_Absyn_Vardecl*_tmp54C;switch(*((int*)_tmp53E)){case 0U: _LL1: _tmp54C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp53E)->f1;_LL2: {
# 2846
struct Cyc_List_List _tmp53F=({struct Cyc_List_List _tmp631;_tmp631.hd=_tmp54C,_tmp631.tl=0;_tmp631;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7E4=Cyc_NewControlFlow_add_vars(_tmp53C,inflow,& _tmp53F,_tmp53C->unknown_none,decl->loc,0);inflow=_tmp7E4;});{
struct Cyc_Absyn_Exp*_tmp540=_tmp54C->initializer;
if(_tmp540 == 0)
return inflow;{
struct _tuple16 _tmp541=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp540);struct _tuple16 _tmp542=_tmp541;union Cyc_CfFlowInfo_FlowInfo _tmp544;void*_tmp543;_LLC: _tmp544=_tmp542.f1;_tmp543=_tmp542.f2;_LLD:;
return Cyc_NewControlFlow_do_initialize_var(_tmp53C,env,_tmp544,_tmp54C,_tmp540,_tmp543,decl->loc);};};}case 3U: _LL3: _tmp54D=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp53E)->f1;_LL4:
# 2855
 return Cyc_NewControlFlow_add_vars(_tmp53C,inflow,_tmp54D,_tmp53C->unknown_none,decl->loc,0);case 1U: _LL5: _tmp54E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp53E)->f1;_LL6:
# 2858
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp53C,inflow,_tmp54E);{
void*t=(void*)_check_null(_tmp54E->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp545=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp54E->fn_vardecl);
# 2864
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp7E8=_tmp53C;union Cyc_CfFlowInfo_FlowInfo _tmp7E7=inflow;struct Cyc_List_List*_tmp7E6=({struct Cyc_List_List*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->hd=_tmp545,_tmp546->tl=0;_tmp546;});void*_tmp7E5=_tmp53C->unknown_all;Cyc_NewControlFlow_add_vars(_tmp7E8,_tmp7E7,_tmp7E6,_tmp7E5,decl->loc,0);});};case 4U: _LL7: _tmp551=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53E)->f1;_tmp550=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53E)->f2;_tmp54F=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp53E)->f3;_LL8:
# 2867
 if(_tmp54F != 0)
({void*_tmp547=0U;({struct _dyneither_ptr _tmp7E9=({const char*_tmp548="found open expression in declaration!";_tag_dyneither(_tmp548,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7E9,_tag_dyneither(_tmp547,sizeof(void*),0U));});});{
struct Cyc_List_List _tmp549=({struct Cyc_List_List _tmp632;_tmp632.hd=_tmp550,_tmp632.tl=0;_tmp632;});
return Cyc_NewControlFlow_add_vars(_tmp53C,inflow,& _tmp549,_tmp53C->unknown_all,decl->loc,0);};default: _LL9: _LLA:
# 2873
({void*_tmp54A=0U;({struct _dyneither_ptr _tmp7EA=({const char*_tmp54B="anal_decl: unexpected decl variant";_tag_dyneither(_tmp54B,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7EA,_tag_dyneither(_tmp54A,sizeof(void*),0U));});});}_LL0:;}
# 2881
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp552;_push_handler(& _tmp552);{int _tmp554=0;if(setjmp(_tmp552.handler))_tmp554=1;if(!_tmp554){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp7ED=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp7EC=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp7EB=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp7ED,_tmp7EC,_tmp7EB,fd);});}
# 2883
;_pop_handler();}else{void*_tmp553=(void*)_exn_thrown;void*_tmp555=_tmp553;void*_tmp557;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp555)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 2889
 if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->tag=Cyc_Dict_Absent;_tmp556;}));}}else{_LL3: _tmp557=_tmp555;_LL4:(int)_rethrow(_tmp557);}_LL0:;}};}
# 2895
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2900
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp558=r;unsigned int _tmp55A;if((_tmp558.RParam).tag == 5){_LL1: _tmp55A=(_tmp558.RParam).val;_LL2: {
# 2903
struct Cyc_Absyn_Vardecl*_tmp559=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp55A);
if(!_tmp559->escapes)
return Cyc_Relations_RVar(_tmp559);
return r;}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 2911
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2915
unsigned int _tmp55B=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp7EE=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp55B,1);inflow=_tmp7EE;});{
# 2920
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp55C=({union Cyc_CfFlowInfo_FlowInfo _tmp58D=inflow;if((_tmp58D.ReachableFL).tag != 2)_throw_match();(_tmp58D.ReachableFL).val;});struct _tuple14 _tmp55D=_tmp55C;struct Cyc_Dict_Dict _tmp58C;struct Cyc_List_List*_tmp58B;_LL1: _tmp58C=_tmp55D.f1;_tmp58B=_tmp55D.f2;_LL2:;{
# 2925
struct Cyc_List_List*_tmp55E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp55F=fd->requires_relns;for(0;_tmp55F != 0;_tmp55F=_tmp55F->tl){
struct Cyc_Relations_Reln*_tmp560=(struct Cyc_Relations_Reln*)_tmp55F->hd;
({struct Cyc_List_List*_tmp7F3=({struct _RegionHandle*_tmp7F2=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp7F1=Cyc_NewControlFlow_translate_rop(_tmp55E,_tmp560->rop1);enum Cyc_Relations_Relation _tmp7F0=_tmp560->relation;union Cyc_Relations_RelnOp _tmp7EF=
Cyc_NewControlFlow_translate_rop(_tmp55E,_tmp560->rop2);
# 2928
Cyc_Relations_add_relation(_tmp7F2,_tmp7F1,_tmp7F0,_tmp7EF,_tmp58B);});_tmp58B=_tmp7F3;});}}{
# 2934
struct Cyc_List_List*atts;
{void*_tmp561=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp562=_tmp561;struct Cyc_List_List*_tmp565;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp562)->tag == 9U){_LL4: _tmp565=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp562)->f1).attributes;_LL5:
 atts=_tmp565;goto _LL3;}else{_LL6: _LL7:
({void*_tmp563=0U;({struct _dyneither_ptr _tmp7F4=({const char*_tmp564="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp564,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F4,_tag_dyneither(_tmp563,sizeof(void*),0U));});});}_LL3:;}
# 2941
for(0;atts != 0;atts=atts->tl){
void*_tmp566=(void*)atts->hd;void*_tmp567=_tmp566;int _tmp580;int _tmp57F;int _tmp57E;switch(*((int*)_tmp567)){case 21U: _LL9: _tmp57E=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp567)->f1;_LLA: {
# 2944
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp568=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp57E - 1);
void*t=Cyc_Tcutil_compress(_tmp568->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp569=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp7F7=fenv;void*_tmp7F6=elttype;struct Cyc_Absyn_Exp*_tmp7F5=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp7F7,_tmp7F6,_tmp7F5,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp56A=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=2U,_tmp56F->f1=_tmp57E,_tmp56F->f2=t;_tmp56F;});
struct Cyc_CfFlowInfo_Place*_tmp56B=({struct Cyc_CfFlowInfo_Place*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->root=(void*)_tmp56A,_tmp56E->path=0;_tmp56E;});
({struct Cyc_Dict_Dict _tmp7F8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp58C,(void*)_tmp56A,_tmp569);_tmp58C=_tmp7F8;});
({struct Cyc_Dict_Dict _tmp7FB=({struct Cyc_Dict_Dict _tmp7FA=_tmp58C;void*_tmp7F9=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->tag=0U,_tmp56C->f1=_tmp568;_tmp56C;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7FA,_tmp7F9,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->tag=4U,_tmp56D->f1=_tmp56B;_tmp56D;}));});_tmp58C=_tmp7FB;});
goto _LL8;}case 20U: _LLB: _tmp57F=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp567)->f1;_LLC: {
# 2957
struct Cyc_Absyn_Vardecl*_tmp570=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp57F - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp570->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp571=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->tag=2U,_tmp577->f1=_tmp57F,_tmp577->f2=elttype;_tmp577;});
struct Cyc_CfFlowInfo_Place*_tmp572=({struct Cyc_CfFlowInfo_Place*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->root=(void*)_tmp571,_tmp576->path=0;_tmp576;});
({struct Cyc_Dict_Dict _tmp7FE=({struct Cyc_Dict_Dict _tmp7FD=_tmp58C;void*_tmp7FC=(void*)_tmp571;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp7FD,_tmp7FC,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp58C=_tmp7FE;});
({struct Cyc_Dict_Dict _tmp801=({struct Cyc_Dict_Dict _tmp800=_tmp58C;void*_tmp7FF=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=0U,_tmp573->f1=_tmp570;_tmp573;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp800,_tmp7FF,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->tag=4U,_tmp574->f1=_tmp572;_tmp574;}));});_tmp58C=_tmp801;});
({struct Cyc_List_List*_tmp802=({struct Cyc_List_List*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->hd=_tmp572,_tmp575->tl=param_roots;_tmp575;});param_roots=_tmp802;});
goto _LL8;}case 22U: _LLD: _tmp580=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp567)->f1;_LLE: {
# 2966
struct Cyc_Absyn_Vardecl*_tmp578=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp580 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp578->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp579=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=0U,_tmp57D->f1=_tmp578;_tmp57D;});
struct Cyc_CfFlowInfo_Place*_tmp57A=({struct Cyc_CfFlowInfo_Place*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->root=(void*)_tmp579,_tmp57C->path=0;_tmp57C;});
({struct Cyc_List_List*_tmp803=({struct Cyc_List_List*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->hd=_tmp57A,_tmp57B->tl=noconsume_roots;_tmp57B;});noconsume_roots=_tmp803;});}
# 2972
goto _LL8;}default: _LLF: _LL10:
 goto _LL8;}_LL8:;}
# 2975
({union Cyc_CfFlowInfo_FlowInfo _tmp804=Cyc_CfFlowInfo_ReachableFL(_tmp58C,_tmp58B);inflow=_tmp804;});{
# 2977
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp58A=_cycalloc(sizeof(*_tmp58A));
_tmp58A->all_tables=tables,({
struct Cyc_Hashtable_Table*_tmp806=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp58A->succ_table=_tmp806;}),({
struct Cyc_Hashtable_Table*_tmp805=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp58A->pat_pop_table=_tmp805;}),_tmp58A->fenv=fenv,_tmp58A->iterate_again=1,_tmp58A->iteration_num=0,_tmp58A->in_try=0,_tmp58A->tryflow=inflow,_tmp58A->noreturn=noreturn,_tmp58A->return_type=fd->ret_type,_tmp58A->unique_pat_vars=0,_tmp58A->param_roots=param_roots,_tmp58A->noconsume_params=noconsume_roots,_tmp58A->flow_table=flow_table,_tmp58A->return_relns=fd->ensures_relns;_tmp58A;});
# 2987
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2993
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp807=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp807;});}{
# 2996
union Cyc_CfFlowInfo_FlowInfo _tmp581=outflow;if((_tmp581.BottomFL).tag == 1){_LL12: _LL13:
 goto _LL11;}else{_LL14: _LL15:
# 2999
 Cyc_NewControlFlow_check_init_params(_tmp55B,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp55B);
# 3003
if(noreturn)
({void*_tmp582=0U;({unsigned int _tmp809=_tmp55B;struct _dyneither_ptr _tmp808=({const char*_tmp583="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp583,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp809,_tmp808,_tag_dyneither(_tmp582,sizeof(void*),0U));});});else{
# 3006
void*_tmp584=Cyc_Tcutil_compress(fd->ret_type);void*_tmp585=_tmp584;switch(*((int*)_tmp585)){case 0U: _LL17: _LL18:
 goto _LL16;case 7U: _LL19: _LL1A:
 goto _LL1C;case 6U: _LL1B: _LL1C:
# 3010
({void*_tmp586=0U;({unsigned int _tmp80B=_tmp55B;struct _dyneither_ptr _tmp80A=({const char*_tmp587="function may not return a value";_tag_dyneither(_tmp587,sizeof(char),32U);});Cyc_Warn_warn(_tmp80B,_tmp80A,_tag_dyneither(_tmp586,sizeof(void*),0U));});});goto _LL16;default: _LL1D: _LL1E:
# 3012
({void*_tmp588=0U;({unsigned int _tmp80D=_tmp55B;struct _dyneither_ptr _tmp80C=({const char*_tmp589="function may not return a value";_tag_dyneither(_tmp589,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp80D,_tmp80C,_tag_dyneither(_tmp588,sizeof(void*),0U));});});goto _LL16;}_LL16:;}
# 3014
goto _LL11;}_LL11:;};};};};};}
# 3018
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp58E=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp58F=_tmp58E;struct Cyc_List_List*_tmp593;struct Cyc_List_List*_tmp592;struct Cyc_List_List*_tmp591;struct Cyc_Absyn_Fndecl*_tmp590;switch(*((int*)_tmp58F)){case 1U: _LL1: _tmp590=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp58F)->f1;_LL2:
 Cyc_NewControlFlow_check_fun(tables,_tmp590);goto _LL0;case 11U: _LL3: _tmp591=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp58F)->f1;_LL4:
 _tmp592=_tmp591;goto _LL6;case 10U: _LL5: _tmp592=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp58F)->f2;_LL6:
 _tmp593=_tmp592;goto _LL8;case 9U: _LL7: _tmp593=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp58F)->f2;_LL8:
 Cyc_NewControlFlow_cf_check(tables,_tmp593);goto _LL0;case 12U: _LL9: _LLA:
 goto _LL0;default: _LLB: _LLC:
 goto _LL0;}_LL0:;};}}
