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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(void*ptr);
# 190
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey();
# 245
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 67
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Position_Error;
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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 916
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_uint_typ;
# 956
extern void*Cyc_Absyn_sint_typ;
# 964
void*Cyc_Absyn_exn_typ();
# 973
extern void*Cyc_Absyn_bounds_one;
# 981
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 992
void*Cyc_Absyn_void_star_typ();
# 994
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1095
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1152
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1165
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
# 1178
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1183
extern int Cyc_Absyn_no_regions;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 65
int Cyc_Tcutil_is_arithmetic_type(void*);
# 86
int Cyc_Tcutil_is_pointer_type(void*t);
# 88
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
# 90
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 147
int Cyc_Tcutil_unify(void*,void*);
# 150
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 241 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 319
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 358
int Cyc_Tcutil_is_array(void*t);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 34 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 61 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 76 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 78
({struct _dyneither_ptr _tmp7AC=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7AB=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp7AB,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7AC,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp7AE=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7AD=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp7AD,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7AE,ap);});}
# 86
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0U;
unsigned int Cyc_Toc_bounds_checks_eliminated=0U;
# 91
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 94
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 104
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 106
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple15{struct _tuple1*f1;struct _tuple1*f2;};
# 132
static int Cyc_Toc_qvar_tag_cmp(struct _tuple15*x,struct _tuple15*y){
struct _tuple15 _tmp2=*x;struct _tuple15 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple15 _tmp4=*y;struct _tuple15 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 141
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp7BA=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp7BA;}),({
struct Cyc_List_List**_tmp7B9=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp7B9;}),({
struct Cyc_Dict_Dict*_tmp7B8=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7B7=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp7B7;});_tmpC;});_tmp11->aggrs_so_far=_tmp7B8;}),({
struct Cyc_List_List**_tmp7B6=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp7B6;}),({
struct Cyc_Set_Set**_tmp7B5=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp7B4=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp7B4;});_tmpE;});_tmp11->datatypes_so_far=_tmp7B5;}),({
struct Cyc_Dict_Dict*_tmp7B3=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp7B2=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp7B2;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp7B3;}),({
struct Cyc_Dict_Dict*_tmp7B1=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp7B0=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp7B0;});_tmp10;});_tmp11->qvar_tags=_tmp7B1;}),({
struct Cyc_Xarray_Xarray*_tmp7AF=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp7AF;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp7BB=({struct Cyc_Toc_TocStateWrap _tmp776;_tmp776.dyn=_tmp1C,_tmp776.state=_tmp1B;_tmp776;});*ts=_tmp7BB;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp777;_tmp777.f1=_tmp1B,_tmp777.f2=arg;_tmp777;});
({void*_tmp7BC=f(h,& _tmp1A);res=_tmp7BC;});}
# 177
;}
# 179
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple17{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple18{struct Cyc_Toc_TocState*f1;struct _tuple17*f2;};struct _tuple19{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 183
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple18*env){
# 186
struct _tuple18 _tmp1D=*env;struct _tuple18 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp24;struct _tuple1*_tmp23;void*(*_tmp22)(struct _tuple1*);_LL1: _tmp24=_tmp1E.f1;_tmp23=(_tmp1E.f2)->f1;_tmp22=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple19**v=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp24->aggrs_so_far,_tmp23);
if(v == 0)
return _tmp22(_tmp23);else{
# 191
struct _tuple19*_tmp1F=*v;struct _tuple19*_tmp20=_tmp1F;void*_tmp21;_LL4: _tmp21=_tmp20->f2;_LL5:;
return _tmp21;}};}
# 196
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple17 env=({struct _tuple17 _tmp778;_tmp778.f1=q,_tmp778.f2=type_maker;_tmp778;});
return((void*(*)(struct _tuple17*arg,void*(*f)(struct _RegionHandle*,struct _tuple18*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp25[5U]="curr";
# 209 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp25,_tmp25,_tmp25 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp26[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp26,_tmp26,_tmp26 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp27[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp27,_tmp27,_tmp27 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp28[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp28,_tmp28,_tmp28 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp29[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp29,_tmp29,_tmp29 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2A[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2B[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2B,_tmp2B,_tmp2B + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2C[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2C,_tmp2C,_tmp2C + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2D[7U]="_throw";
# 230 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp2D,_tmp2D,_tmp2D + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp2E[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp2E,_tmp2E,_tmp2E + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp2F[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp2F,_tmp2F,_tmp2F + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp30[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp30,_tmp30,_tmp30 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp31[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp31,_tmp31,_tmp31 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp32[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp32,_tmp32,_tmp32 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp33[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp33,_tmp33,_tmp33 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp34[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp34,_tmp34,_tmp34 + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp35[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp35,_tmp35,_tmp35 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp36,_tmp36,_tmp36 + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp37[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp37,_tmp37,_tmp37 + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp38[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp38,_tmp38,_tmp38 + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp39[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39 + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3A[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3A,_tmp3A,_tmp3A + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3B[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3B,_tmp3B,_tmp3B + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3C[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3C,_tmp3C,_tmp3C + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp3D[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp3D,_tmp3D,_tmp3D + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp3E[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp3E,_tmp3E,_tmp3E + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp3F[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp3F,_tmp3F,_tmp3F + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp40[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp40,_tmp40,_tmp40 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp41[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp41,_tmp41,_tmp41 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp42[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp42,_tmp42,_tmp42 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp43[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp43,_tmp43,_tmp43 + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp44[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp44,_tmp44,_tmp44 + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp45[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp45,_tmp45,_tmp45 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp46[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp46,_tmp46,_tmp46 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp47[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp47,_tmp47,_tmp47 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp48[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp48,_tmp48,_tmp48 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp49[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp49,_tmp49,_tmp49 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp4A[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp4A,_tmp4A,_tmp4A + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp4B[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp4B,_tmp4B,_tmp4B + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp4C[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp4C,_tmp4C,_tmp4C + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp4D[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp4D,_tmp4D,_tmp4D + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp4E[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp4E,_tmp4E,_tmp4E + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp4F[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp4F,_tmp4F,_tmp4F + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp50[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp50,_tmp50,_tmp50 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp51[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp51,_tmp51,_tmp51 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp52[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp52,_tmp52,_tmp52 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp53[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp53,_tmp53,_tmp53 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp54[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp54,_tmp54,_tmp54 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp55[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp55,_tmp55,_tmp55 + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp56[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp56,_tmp56,_tmp56 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp57[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp57,_tmp57,_tmp57 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp58[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp58,_tmp58,_tmp58 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp59[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp59,_tmp59,_tmp59 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp5A[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp5A,_tmp5A,_tmp5A + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp5B[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp5B,_tmp5B,_tmp5B + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp5C[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp5C,_tmp5C,_tmp5C + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp5D[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp5D,_tmp5D,_tmp5D + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp5E[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp5E,_tmp5E,_tmp5E + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp5F[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp5F,_tmp5F,_tmp5F + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp60[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp60,_tmp60,_tmp60 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp61[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp61,_tmp61,_tmp61 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp62[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp62,_tmp62,_tmp62 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp63[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp63,_tmp63,_tmp63 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp64[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp64,_tmp64,_tmp64 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp65[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp65,_tmp65,_tmp65 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp66[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp66,_tmp66,_tmp66 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp67[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp67,_tmp67,_tmp67 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp68[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp68,_tmp68,_tmp68 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp69[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp69,_tmp69,_tmp69 + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp6A[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp6A,_tmp6A,_tmp6A + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp6B[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp6B,_tmp6B,_tmp6B + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp6C[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp6C,_tmp6C,_tmp6C + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 298
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11U,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 301
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 303
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 305
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 309
static struct _tuple20*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_List_List*_tmp7BE=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));({void*_tmp7BD=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=1U,_tmp6D->f1=name;_tmp6D;});_tmp6E->hd=_tmp7BD;}),_tmp6E->tl=0;_tmp6E;});_tmp6F->f1=_tmp7BE;}),_tmp6F->f2=e;_tmp6F;});}
# 315
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp70=e->r;void*_tmp71=_tmp70;struct Cyc_Absyn_Exp*_tmp72;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp72=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp72);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 321
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=14U,_tmp73->f1=t,_tmp73->f2=e,_tmp73->f3=0,_tmp73->f4=Cyc_Absyn_No_coercion;_tmp73;});}
# 324
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=20U,_tmp74->f1=e;_tmp74;});}
# 327
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=23U,_tmp75->f1=e1,_tmp75->f2=e2;_tmp75;});}
# 330
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=37U,_tmp76->f1=s;_tmp76;});}
# 333
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=17U,_tmp77->f1=t;_tmp77;});}
# 336
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=10U,_tmp78->f1=e,_tmp78->f2=es,_tmp78->f3=0,_tmp78->f4=1;_tmp78;});}
# 339
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=1U,_tmp79->f1=e;_tmp79;});}
# 342
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=2U,_tmp7A->f1=s1,_tmp7A->f2=s2;_tmp7A;});}
# 345
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=6U,_tmp7B->f1=e1,_tmp7B->f2=e2,_tmp7B->f3=e3;_tmp7B;});}
# 348
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=21U,_tmp7C->f1=e,_tmp7C->f2=n,_tmp7C->f3=0,_tmp7C->f4=0;_tmp7C;});}
# 351
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=22U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 354
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 357
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=36U,_tmp7E->f1=tdopt,_tmp7E->f2=ds;_tmp7E;});}
# 359
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=8U,_tmp7F->f1=v;_tmp7F;});}
# 362
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 366
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp80=e->r;void*_tmp81=_tmp80;struct Cyc_Absyn_Exp*_tmp82;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->tag == 20U){_LL1: _tmp82=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp82,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 385
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 394
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 403
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 412
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 423
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp83=Cyc_Tcutil_compress(t);void*_tmp84=_tmp83;enum Cyc_Absyn_Size_of _tmp8B;switch(*((int*)_tmp84)){case 6U: _LL1: _tmp8B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp84)->f2;_LL2:
# 427
{enum Cyc_Absyn_Size_of _tmp85=_tmp8B;switch(_tmp85){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp86=0U;({struct _dyneither_ptr _tmp7BF=({const char*_tmp87="impossible IntType (not char, short or int)";_tag_dyneither(_tmp87,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7BF,_tag_dyneither(_tmp86,sizeof(void*),0U));});});}_LLD:;}
# 433
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp84)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp779;_tmp779.tag=0U,({struct _dyneither_ptr _tmp7C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp779.f1=_tmp7C0;});_tmp779;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({struct _dyneither_ptr _tmp7C1=({const char*_tmp89="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp89,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C1,_tag_dyneither(_tmp88,sizeof(void*),1U));});});}_LL0:;}
# 440
return function;}
# 442
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8D=_tmp8C;void*_tmp90;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 5U){_LL1: _tmp90=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp90);}else{_LL3: _LL4:
({void*_tmp8E=0U;({struct _dyneither_ptr _tmp7C2=({const char*_tmp8F="impossible type (not pointer)";_tag_dyneither(_tmp8F,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7C2,_tag_dyneither(_tmp8E,sizeof(void*),0U));});});}_LL0:;}
# 451
static int Cyc_Toc_is_array_type(void*t){
void*_tmp91=Cyc_Tcutil_compress(t);void*_tmp92=_tmp91;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 459
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp93=e->r;void*_tmp94=_tmp93;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;long long _tmp9A;int _tmp99;short _tmp98;struct _dyneither_ptr _tmp97;char _tmp96;switch(*((int*)_tmp94)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Null_c).tag){case 2U: _LL1: _tmp96=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Char_c).val).f2;_LL2:
 return _tmp96 == '\000';case 3U: _LL3: _tmp97=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Wchar_c).val;_LL4: {
# 463
unsigned long _tmp95=Cyc_strlen((struct _dyneither_ptr)_tmp97);
int i=0;
if(_tmp95 >= 2  && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp97.curr)[1]== 'x'  && _tmp95 >= 3) && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp95;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 473
return 0;}}case 4U: _LL5: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Short_c).val).f2;_LL6:
 return _tmp98 == 0;case 5U: _LL7: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Int_c).val).f2;_LL8:
 return _tmp99 == 0;case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9A == 0;case 1U: _LLD: _LLE:
# 478
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 477
 goto _LLE;case 14U: _LLF: _tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL10:
# 479
 return Cyc_Toc_is_zero(_tmp9B);case 24U: _LL11: _tmp9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9C);case 26U: _LL13: _tmp9D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL14:
 _tmp9E=_tmp9D;goto _LL16;case 29U: _LL15: _tmp9E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp94)->f3;_LL16:
 _tmp9F=_tmp9E;goto _LL18;case 25U: _LL17: _tmp9F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL18:
 _tmpA0=_tmp9F;goto _LL1A;case 36U: _LL19: _tmpA0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL1A:
# 485
 for(0;_tmpA0 != 0;_tmpA0=_tmpA0->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmpA0->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 494
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA1=x;union Cyc_Absyn_Nmspace _tmpB3;struct _dyneither_ptr*_tmpB2;_LL1: _tmpB3=_tmpA1->f1;_tmpB2=_tmpA1->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA2=_tmpB3;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;switch((_tmpA2.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAF=0;goto _LL7;case 1U: _LL6: _tmpAF=(_tmpA2.Rel_n).val;_LL7:
 _tmpB0=_tmpAF;goto _LL9;case 2U: _LL8: _tmpB0=(_tmpA2.Abs_n).val;_LL9:
 _tmpB1=_tmpB0;goto _LLB;default: _LLA: _tmpB1=(_tmpA2.C_n).val;_LLB:
# 503
 if(_tmpB1 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp77B;_tmp77B.tag=0U,_tmp77B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp77B;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp77A;_tmp77A.tag=0U,_tmp77A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp77A;});void*_tmpA3[2U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6;({struct _dyneither_ptr _tmp7C3=({const char*_tmpA4="%s_%s_struct";_tag_dyneither(_tmpA4,sizeof(char),13U);});Cyc_aprintf(_tmp7C3,_tag_dyneither(_tmpA3,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA7=_new_region("r");struct _RegionHandle*r=& _tmpA7;_push_region(r);
{struct _dyneither_ptr _tmpAE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp77E;_tmp77E.tag=0U,_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp77E;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp77D;_tmp77D.tag=0U,({struct _dyneither_ptr _tmp7C6=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7C5=r;struct Cyc_List_List*_tmp7C4=_tmpB1;Cyc_rstr_sepstr(_tmp7C5,_tmp7C4,({const char*_tmpAD="_";_tag_dyneither(_tmpAD,sizeof(char),2U);}));}));_tmp77D.f1=_tmp7C6;});_tmp77D;});struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U,_tmp77C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp77C;});void*_tmpA8[3U];_tmpA8[0]=& _tmpAA,_tmpA8[1]=& _tmpAB,_tmpA8[2]=& _tmpAC;({struct _dyneither_ptr _tmp7C7=({const char*_tmpA9="%s_%s_%s_struct";_tag_dyneither(_tmpA9,sizeof(char),16U);});Cyc_aprintf(_tmp7C7,_tag_dyneither(_tmpA8,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAE;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 516
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 519
struct _tuple21 _tmpB4=*env;struct _tuple21 _tmpB5=_tmpB4;struct Cyc_Dict_Dict*_tmpC8;struct _tuple15*_tmpC7;_LL1: _tmpC8=(_tmpB5.f1)->qvar_tags;_tmpC7=_tmpB5.f2;_LL2:;{
struct _tuple15 _tmpB6=*_tmpC7;struct _tuple15 _tmpB7=_tmpB6;struct _tuple1*_tmpC6;struct _tuple1*_tmpC5;_LL4: _tmpC6=_tmpB7.f1;_tmpC5=_tmpB7.f2;_LL5:;{
struct _handler_cons _tmpB8;_push_handler(& _tmpB8);{int _tmpBA=0;if(setjmp(_tmpB8.handler))_tmpBA=1;if(!_tmpBA){{struct _tuple1*_tmpBB=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpC8,Cyc_Toc_qvar_tag_cmp,_tmpC7);_npop_handler(0U);return _tmpBB;};_pop_handler();}else{void*_tmpB9=(void*)_exn_thrown;void*_tmpBC=_tmpB9;void*_tmpC4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBC)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 523
struct _tuple15*_tmpBD=({struct _tuple15*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=_tmpC6,_tmpC3->f2=_tmpC5;_tmpC3;});
struct _tuple1*_tmpBE=_tmpC6;union Cyc_Absyn_Nmspace _tmpC2;struct _dyneither_ptr*_tmpC1;_LLC: _tmpC2=_tmpBE->f1;_tmpC1=_tmpBE->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC1,_tmpC5);
struct _tuple1*res=({struct _tuple1*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=_tmpC2,({struct _dyneither_ptr*_tmp7C8=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));*_tmpBF=newvar;_tmpBF;});_tmpC0->f2=_tmp7C8;});_tmpC0;});
({struct Cyc_Dict_Dict _tmp7C9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC8,_tmpBD,res);*_tmpC8=_tmp7C9;});
return res;};}}else{_LL9: _tmpC4=_tmpBC;_LLA:(int)_rethrow(_tmpC4);}_LL6:;}};};};}
# 532
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp77F;_tmp77F.f1=fieldname,_tmp77F.f2=dtname;_tmp77F;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 539
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->kind=Cyc_Absyn_StructA,_tmpCB->sc=Cyc_Absyn_Public,_tmpCB->tvs=0,_tmpCB->attributes=0,_tmpCB->expected_mem_kind=0,({
# 542
struct _tuple1*_tmp7CC=({struct _tuple1*_tmpC9=_cycalloc(sizeof(*_tmpC9));({union Cyc_Absyn_Nmspace _tmp7CB=Cyc_Absyn_Rel_n(0);_tmpC9->f1=_tmp7CB;}),_tmpC9->f2=name;_tmpC9;});_tmpCB->name=_tmp7CC;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7CA=({struct Cyc_Absyn_AggrdeclImpl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->exist_vars=0,_tmpCA->rgn_po=0,_tmpCA->tagged=0,_tmpCA->fields=fs;_tmpCA;});_tmpCB->impl=_tmp7CA;});_tmpCB;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 551
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 554
struct _tuple22 _tmpCC=*env;struct _tuple22 _tmpCD=_tmpCC;struct Cyc_List_List**_tmpE4;struct Cyc_List_List*_tmpE3;_LL1: _tmpE4=(_tmpCD.f1)->tuple_types;_tmpE3=_tmpCD.f2;_LL2:;
# 556
{struct Cyc_List_List*_tmpCE=*_tmpE4;for(0;_tmpCE != 0;_tmpCE=_tmpCE->tl){
struct _tuple23*_tmpCF=(struct _tuple23*)_tmpCE->hd;struct _tuple23*_tmpD0=_tmpCF;void*_tmpD3;struct Cyc_List_List*_tmpD2;_LL4: _tmpD3=_tmpD0->f1;_tmpD2=_tmpD0->f2;_LL5:;{
struct Cyc_List_List*_tmpD1=_tmpE3;
for(0;_tmpD1 != 0  && _tmpD2 != 0;(_tmpD1=_tmpD1->tl,_tmpD2=_tmpD2->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD1->hd)).f2,(void*)_tmpD2->hd))
break;}
if(_tmpD1 == 0  && _tmpD2 == 0)
return _tmpD3;};}}{
# 567
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct _dyneither_ptr _tmp7CE=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp780;_tmp780.tag=1U,_tmp780.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp780;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _dyneither_ptr _tmp7CD=({const char*_tmpE0="_tuple%d";_tag_dyneither(_tmpE0,sizeof(char),9U);});Cyc_aprintf(_tmp7CD,_tag_dyneither(_tmpDF,sizeof(void*),1U));});});*_tmpE2=_tmp7CE;});_tmpE2;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE3);
struct Cyc_List_List*_tmpD4=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7D1=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct Cyc_Absyn_Aggrfield*_tmp7D0=({struct Cyc_Absyn_Aggrfield*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct _dyneither_ptr*_tmp7CF=Cyc_Absyn_fieldname(i);_tmpD5->name=_tmp7CF;}),_tmpD5->tq=Cyc_Toc_mt_tq,_tmpD5->type=(void*)ts2->hd,_tmpD5->width=0,_tmpD5->attributes=0,_tmpD5->requires_clause=0;_tmpD5;});_tmpD6->hd=_tmp7D0;}),_tmpD6->tl=_tmpD4;_tmpD6;});_tmpD4=_tmp7D1;});}}
({struct Cyc_List_List*_tmp7D2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);_tmpD4=_tmp7D2;});{
struct Cyc_Absyn_Aggrdecl*_tmpD7=Cyc_Toc_make_c_struct_defn(xname,_tmpD4);
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD8=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7D3=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDD=_cycalloc(sizeof(*_tmpDD));*_tmpDD=_tmpD7;_tmpDD;}));(_tmpDE->f1).aggr_info=_tmp7D3;}),(_tmpDE->f1).targs=0;_tmpDE;});
({struct Cyc_List_List*_tmp7D5=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_Decl*_tmp7D4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=5U,_tmpD9->f1=_tmpD7;_tmpD9;}),0U);_tmpDA->hd=_tmp7D4;}),_tmpDA->tl=Cyc_Toc_result_decls;_tmpDA;});Cyc_Toc_result_decls=_tmp7D5;});
({struct Cyc_List_List*_tmp7D7=({struct Cyc_List_List*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));({struct _tuple23*_tmp7D6=({struct _tuple23*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));_tmpDB->f1=(void*)_tmpD8,_tmpDB->f2=ts;_tmpDB;});_tmpDC->hd=_tmp7D6;}),_tmpDC->tl=*_tmpE4;_tmpDC;});*_tmpE4=_tmp7D7;});
return(void*)_tmpD8;};};}
# 580
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 585
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 588
struct _tuple25*_tmpE5=env;struct Cyc_List_List**_tmpFA;enum Cyc_Absyn_AggrKind _tmpF9;struct Cyc_List_List*_tmpF8;_LL1: _tmpFA=(_tmpE5->f1)->anon_aggr_types;_tmpF9=(_tmpE5->f2)->f1;_tmpF8=(_tmpE5->f2)->f2;_LL2:;
# 590
{struct Cyc_List_List*_tmpE6=*_tmpFA;for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
struct _tuple26*_tmpE7=(struct _tuple26*)_tmpE6->hd;struct _tuple26*_tmpE8=_tmpE7;void*_tmpEB;enum Cyc_Absyn_AggrKind _tmpEA;struct Cyc_List_List*_tmpE9;_LL4: _tmpEB=_tmpE8->f1;_tmpEA=_tmpE8->f2;_tmpE9=_tmpE8->f3;_LL5:;
if(_tmpF9 != _tmpEA)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE9,_tmpF8))
return _tmpEB;}}{
# 599
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _dyneither_ptr _tmp7D9=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp781;_tmp781.tag=1U,_tmp781.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp781;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp7D8=({const char*_tmpF5="_tuple%d";_tag_dyneither(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp7D8,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp7D9;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpEC=Cyc_Toc_make_c_struct_defn(xname,_tmpF8);
_tmpEC->kind=_tmpF9;{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpED=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7DA=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF2=_cycalloc(sizeof(*_tmpF2));*_tmpF2=_tmpEC;_tmpF2;}));(_tmpF3->f1).aggr_info=_tmp7DA;}),(_tmpF3->f1).targs=0;_tmpF3;});
({struct Cyc_List_List*_tmp7DC=({struct Cyc_List_List*_tmpEF=_cycalloc(sizeof(*_tmpEF));({struct Cyc_Absyn_Decl*_tmp7DB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=5U,_tmpEE->f1=_tmpEC;_tmpEE;}),0U);_tmpEF->hd=_tmp7DB;}),_tmpEF->tl=Cyc_Toc_result_decls;_tmpEF;});Cyc_Toc_result_decls=_tmp7DC;});
({struct Cyc_List_List*_tmp7DE=({struct Cyc_List_List*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));({struct _tuple26*_tmp7DD=({struct _tuple26*_tmpF0=_region_malloc(d,sizeof(*_tmpF0));_tmpF0->f1=(void*)_tmpED,_tmpF0->f2=_tmpF9,_tmpF0->f3=_tmpF8;_tmpF0;});_tmpF1->hd=_tmp7DD;}),_tmpF1->tl=*_tmpFA;_tmpF1;});*_tmpFA=_tmp7DE;});
return(void*)_tmpED;};};}
# 607
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=ak,_tmpFB->f2=fs;_tmpFB;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 616
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 625
struct _tuple28 _tmpFC=*env;struct _tuple28 _tmpFD=_tmpFC;struct Cyc_List_List**_tmp120;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11C;_LL1: _tmp120=(_tmpFD.f1)->abs_struct_types;_tmp11F=(_tmpFD.f2)->f1;_tmp11E=(_tmpFD.f2)->f2;_tmp11D=(_tmpFD.f2)->f3;_tmp11C=(_tmpFD.f2)->f4;_LL2:;
# 629
{struct Cyc_List_List*_tmpFE=*_tmp120;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple29*_tmpFF=(struct _tuple29*)_tmpFE->hd;struct _tuple29*_tmp100=_tmpFF;struct _tuple1*_tmp108;struct Cyc_List_List*_tmp107;void*_tmp106;_LL4: _tmp108=_tmp100->f1;_tmp107=_tmp100->f2;_tmp106=_tmp100->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp108,_tmp11F)== 0  && ({
int _tmp7DF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11D);_tmp7DF == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp107);})){
int okay=1;
{struct Cyc_List_List*_tmp101=_tmp11D;for(0;_tmp101 != 0;(_tmp101=_tmp101->tl,_tmp107=_tmp107->tl)){
void*_tmp102=(void*)_tmp101->hd;
void*_tmp103=(void*)((struct Cyc_List_List*)_check_null(_tmp107))->hd;
{struct Cyc_Absyn_Kind*_tmp104=Cyc_Tcutil_typ_kind(_tmp102);struct Cyc_Absyn_Kind*_tmp105=_tmp104;switch(((struct Cyc_Absyn_Kind*)_tmp105)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 640
 continue;default: _LLB: _LLC:
# 643
 if(Cyc_Tcutil_unify(_tmp102,_tmp103) || ({void*_tmp7E0=Cyc_Toc_typ_to_c(_tmp102);Cyc_Tcutil_unify(_tmp7E0,Cyc_Toc_typ_to_c(_tmp103));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 648
break;}}
# 650
if(okay)
return _tmp106;}}}{
# 657
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct _dyneither_ptr _tmp7E2=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp782;_tmp782.tag=1U,_tmp782.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp782;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct _dyneither_ptr _tmp7E1=({const char*_tmp119="_tuple%d";_tag_dyneither(_tmp119,sizeof(char),9U);});Cyc_aprintf(_tmp7E1,_tag_dyneither(_tmp118,sizeof(void*),1U));});});*_tmp11B=_tmp7E2;});_tmp11B;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp109=0;
# 661
({struct Cyc_List_List*_tmp7E4=({struct Cyc_List_List*_tmp10B=_region_malloc(d,sizeof(*_tmp10B));({struct _tuple29*_tmp7E3=({struct _tuple29*_tmp10A=_region_malloc(d,sizeof(*_tmp10A));_tmp10A->f1=_tmp11F,_tmp10A->f2=_tmp11D,_tmp10A->f3=x;_tmp10A;});_tmp10B->hd=_tmp7E3;}),_tmp10B->tl=*_tmp120;_tmp10B;});*_tmp120=_tmp7E4;});{
# 664
struct _RegionHandle _tmp10C=_new_region("r");struct _RegionHandle*r=& _tmp10C;_push_region(r);
{struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11E,_tmp11D);
for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
struct Cyc_Absyn_Aggrfield*_tmp10E=(struct Cyc_Absyn_Aggrfield*)_tmp11C->hd;
void*t=_tmp10E->type;
struct Cyc_List_List*atts=_tmp10E->attributes;
# 674
if((_tmp11C->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))) && !
Cyc_Toc_is_array_type(t))
({struct Cyc_List_List*_tmp7E6=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({void*_tmp7E5=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=0;_tmp10F;});_tmp110->hd=_tmp7E5;}),_tmp110->tl=atts;_tmp110;});atts=_tmp7E6;});
({void*_tmp7E7=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10D,t));t=_tmp7E7;});
# 681
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp7EC=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=8U,({void*_tmp7EB=Cyc_Absyn_void_star_typ();(_tmp111->f1).elt_type=_tmp7EB;}),({struct Cyc_Absyn_Tqual _tmp7EA=Cyc_Absyn_empty_tqual(0U);(_tmp111->f1).tq=_tmp7EA;}),({
struct Cyc_Absyn_Exp*_tmp7E9=Cyc_Absyn_uint_exp(0U,0U);(_tmp111->f1).num_elts=_tmp7E9;}),({union Cyc_Absyn_Constraint*_tmp7E8=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp111->f1).zero_term=_tmp7E8;}),(_tmp111->f1).zt_loc=0U;_tmp111;});
# 682
t=_tmp7EC;});
# 685
({struct Cyc_List_List*_tmp7EE=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Aggrfield*_tmp7ED=({struct Cyc_Absyn_Aggrfield*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp10E->name,_tmp112->tq=Cyc_Toc_mt_tq,_tmp112->type=t,_tmp112->width=_tmp10E->width,_tmp112->attributes=atts,_tmp112->requires_clause=0;_tmp112;});_tmp113->hd=_tmp7ED;}),_tmp113->tl=_tmp109;_tmp113;});_tmp109=_tmp7EE;});}
# 687
({struct Cyc_List_List*_tmp7EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp109);_tmp109=_tmp7EF;});{
struct Cyc_Absyn_Aggrdecl*_tmp114=Cyc_Toc_make_c_struct_defn(xname,_tmp109);
({struct Cyc_List_List*_tmp7F1=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_Decl*_tmp7F0=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=5U,_tmp115->f1=_tmp114;_tmp115;}),0U);_tmp116->hd=_tmp7F0;}),_tmp116->tl=Cyc_Toc_result_decls;_tmp116;});Cyc_Toc_result_decls=_tmp7F1;});{
void*_tmp117=x;_npop_handler(0U);return _tmp117;};};}
# 665
;_pop_region(r);};};}
# 693
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 697
struct _tuple27 env=({struct _tuple27 _tmp783;_tmp783.f1=struct_name,_tmp783.f2=type_vars,_tmp783.f3=type_args,_tmp783.f4=fields;_tmp783;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 705
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp7F4=({struct _dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _dyneither_ptr _tmp7F3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp123=({struct Cyc_Int_pa_PrintArg_struct _tmp784;_tmp784.tag=1U,_tmp784.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp784;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _dyneither_ptr _tmp7F2=({const char*_tmp122="_tmp%X";_tag_dyneither(_tmp122,sizeof(char),7U);});Cyc_aprintf(_tmp7F2,_tag_dyneither(_tmp121,sizeof(void*),1U));});});*_tmp124=_tmp7F3;});_tmp124;});_tmp125->f2=_tmp7F4;});_tmp125;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 711
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp126=*env;struct _tuple30 _tmp127=_tmp126;struct Cyc_Xarray_Xarray*_tmp12F;_LL1: _tmp12F=(_tmp127.f1)->temp_labels;_LL2:;{
int _tmp128=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7F5=_tmp128;_tmp7F5 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12F);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12F,_tmp128);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct _dyneither_ptr _tmp7F7=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12D=({struct Cyc_Int_pa_PrintArg_struct _tmp785;_tmp785.tag=1U,_tmp785.f1=(unsigned int)_tmp128;_tmp785;});void*_tmp12B[1U];_tmp12B[0]=& _tmp12D;({struct _dyneither_ptr _tmp7F6=({const char*_tmp12C="_LL%X";_tag_dyneither(_tmp12C,sizeof(char),6U);});Cyc_aprintf(_tmp7F6,_tag_dyneither(_tmp12B,sizeof(void*),1U));});});*_tmp12E=_tmp7F7;});_tmp12E;});
if(({int _tmp7F8=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12F,res);_tmp7F8 != _tmp128;}))
({void*_tmp129=0U;({struct _dyneither_ptr _tmp7F9=({const char*_tmp12A="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp12A,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F9,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
return res;};};}
# 722
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 728
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp130))->hd)->name)!= 0){
++ ans;
_tmp130=_tmp130->tl;}
# 735
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 741
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp131=*a;struct _tuple9 _tmp132=_tmp131;struct _dyneither_ptr*_tmp136;struct Cyc_Absyn_Tqual _tmp135;void*_tmp134;_LL1: _tmp136=_tmp132.f1;_tmp135=_tmp132.f2;_tmp134=_tmp132.f3;_LL2:;
return({struct _tuple9*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=_tmp136,_tmp133->f2=_tmp135,({void*_tmp7FA=Cyc_Toc_typ_to_c(_tmp134);_tmp133->f3=_tmp7FA;});_tmp133;});}
# 764 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;void*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Tqual _tmp13C;struct Cyc_Absyn_Exp*_tmp13B;union Cyc_Absyn_Constraint*_tmp13A;unsigned int _tmp139;switch(*((int*)_tmp138)){case 8U: _LL1: _tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).elt_type;_tmp13C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).tq;_tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).num_elts;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zero_term;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zt_loc;_LL2:
# 767
 return({void*_tmp7FB=Cyc_Toc_typ_to_c_array(_tmp13D);Cyc_Absyn_cstar_typ(_tmp7FB,_tmp13C);});case 1U: _LL3: _tmp13E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp138)->f2;if(_tmp13E != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13E);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 773
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 775
struct Cyc_Absyn_Aggrfield*_tmp13F=({struct Cyc_Absyn_Aggrfield*_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=*f;_tmp140;});
_tmp13F->type=new_type;
_tmp13F->requires_clause=0;
_tmp13F->tq=Cyc_Toc_mt_tq;
return _tmp13F;}
# 782
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 784
return;}
# 787
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp7FD=({void**_tmp141=_cycalloc(sizeof(*_tmp141));({void*_tmp7FC=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);*_tmp141=_tmp7FC;});_tmp141;});cs=_tmp7FD;});
return*cs;}
# 793
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp800=({void**_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp7FF=({void*_tmp7FE=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7FE,Cyc_Toc_mt_tq);});*_tmp142=_tmp7FF;});_tmp142;});r=_tmp800;});
return*r;}
# 799
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp803=({void**_tmp143=_cycalloc(sizeof(*_tmp143));({void*_tmp802=({void*_tmp801=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp801,Cyc_Toc_mt_tq);});*_tmp143=_tmp802;});_tmp143;});r=_tmp803;});
return*r;}
# 805
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp144=Cyc_Tcutil_compress(t);void*_tmp145=_tmp144;struct Cyc_Absyn_Tvar*_tmp146;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->tag == 2U){_LL1: _tmp146=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 811
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp147=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp148=_tmp147;if(((struct Cyc_Absyn_Kind*)_tmp148)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 818
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp149=t;struct Cyc_Absyn_Datatypedecl*_tmp192;void**_tmp191;struct Cyc_Absyn_Enumdecl*_tmp190;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Typedefdecl*_tmp18A;void*_tmp189;struct Cyc_List_List*_tmp188;struct _tuple1*_tmp187;union Cyc_Absyn_AggrInfoU _tmp186;struct Cyc_List_List*_tmp185;enum Cyc_Absyn_AggrKind _tmp184;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Tqual _tmp181;void*_tmp180;struct Cyc_List_List*_tmp17F;int _tmp17E;struct Cyc_Absyn_VarargInfo*_tmp17D;struct Cyc_List_List*_tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp179;unsigned int _tmp178;void*_tmp177;struct Cyc_Absyn_Tqual _tmp176;union Cyc_Absyn_Constraint*_tmp175;struct Cyc_Absyn_Datatypedecl*_tmp174;struct Cyc_Absyn_Datatypefield*_tmp173;struct Cyc_Absyn_Tvar*_tmp172;void**_tmp171;switch(*((int*)_tmp149)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmp171=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp149)->f2;_LL4:
# 822
 if(*_tmp171 == 0){
*_tmp171=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 826
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp171));case 2U: _LL5: _tmp172=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL6:
# 828
 if((Cyc_Tcutil_tvar_kind(_tmp172,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 830
return(void*)& Cyc_Absyn_VoidType_val;else{
# 832
return Cyc_Absyn_void_star_typ();}case 3U: _LL7: _LL8:
# 834
 return(void*)& Cyc_Absyn_VoidType_val;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).tag == 2){_LL9: _tmp174=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f1;_tmp173=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f2;_LLA:
# 836
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp173->name,_tmp174->name));}else{_LLB: _LLC:
({void*_tmp14A=0U;({struct _dyneither_ptr _tmp804=({const char*_tmp14B="unresolved DatatypeFieldType";_tag_dyneither(_tmp14B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp804,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});}case 5U: _LLD: _tmp177=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_typ;_tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_tq;_tmp175=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).bounds;_LLE:
# 841
({void*_tmp805=Cyc_Toc_typ_to_c(_tmp177);_tmp177=_tmp805;});{
void*_tmp14C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp175);void*_tmp14D=_tmp14C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp14D)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp177,(void*)& Cyc_Absyn_HeapRgn_val,_tmp176,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 846
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp17B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).tq;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).num_elts;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).zt_loc;_LL14:
# 849
 return({void*_tmp809=Cyc_Toc_typ_to_c(_tmp17B);struct Cyc_Absyn_Tqual _tmp808=_tmp17A;struct Cyc_Absyn_Exp*_tmp807=_tmp179;union Cyc_Absyn_Constraint*_tmp806=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp809,_tmp808,_tmp807,_tmp806,_tmp178);});case 9U: _LL15: _tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_tqual;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_typ;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).args;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).c_varargs;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).cyc_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).attributes;_LL16: {
# 855
struct Cyc_List_List*_tmp14E=0;
for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
void*_tmp14F=(void*)_tmp17C->hd;void*_tmp150=_tmp14F;switch(*((int*)_tmp150)){case 4U: _LL47: _LL48:
 goto _LL4A;case 5U: _LL49: _LL4A:
 goto _LL4C;case 19U: _LL4B: _LL4C:
 continue;case 22U: _LL4D: _LL4E:
 continue;case 21U: _LL4F: _LL50:
 continue;case 20U: _LL51: _LL52:
 continue;default: _LL53: _LL54:
({struct Cyc_List_List*_tmp80A=({struct Cyc_List_List*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->hd=(void*)_tmp17C->hd,_tmp151->tl=_tmp14E;_tmp151;});_tmp14E=_tmp80A;});goto _LL46;}_LL46:;}{
# 866
struct Cyc_List_List*_tmp152=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17F);
if(_tmp17D != 0){
# 869
void*_tmp153=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp17D->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp154=({struct _tuple9*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->f1=_tmp17D->name,_tmp156->f2=_tmp17D->tq,_tmp156->f3=_tmp153;_tmp156;});
({struct Cyc_List_List*_tmp80C=({struct Cyc_List_List*_tmp80B=_tmp152;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp80B,({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=_tmp154,_tmp155->tl=0;_tmp155;}));});_tmp152=_tmp80C;});}
# 873
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->tag=9U,(_tmp157->f1).tvars=0,(_tmp157->f1).effect=0,(_tmp157->f1).ret_tqual=_tmp181,({void*_tmp80D=Cyc_Toc_typ_to_c(_tmp180);(_tmp157->f1).ret_typ=_tmp80D;}),(_tmp157->f1).args=_tmp152,(_tmp157->f1).c_varargs=_tmp17E,(_tmp157->f1).cyc_varargs=0,(_tmp157->f1).rgn_po=0,(_tmp157->f1).attributes=_tmp14E,(_tmp157->f1).requires_clause=0,(_tmp157->f1).requires_relns=0,(_tmp157->f1).ensures_clause=0,(_tmp157->f1).ensures_relns=0;_tmp157;});};}case 10U: _LL17: _tmp182=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp149)->f1;_LL18: {
# 878
struct Cyc_List_List*_tmp158=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp810=({struct Cyc_List_List*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct _tuple12*_tmp80F=({struct _tuple12*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->f1=(*((struct _tuple12*)_tmp182->hd)).f1,({void*_tmp80E=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp182->hd)).f2);_tmp159->f2=_tmp80E;});_tmp159;});_tmp15A->hd=_tmp80F;}),_tmp15A->tl=_tmp158;_tmp15A;});_tmp158=_tmp810;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp158));}case 12U: _LL19: _tmp184=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f1;_tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f2;_LL1A: {
# 887
struct Cyc_List_List*_tmp15B=0;
for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
({struct Cyc_List_List*_tmp813=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));({struct Cyc_Absyn_Aggrfield*_tmp812=({struct Cyc_Absyn_Aggrfield*_tmp811=(struct Cyc_Absyn_Aggrfield*)_tmp183->hd;Cyc_Toc_aggrfield_to_c(_tmp811,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp183->hd)->type));});_tmp15C->hd=_tmp812;}),_tmp15C->tl=_tmp15B;_tmp15C;});_tmp15B=_tmp813;});}
return({enum Cyc_Absyn_AggrKind _tmp814=_tmp184;Cyc_Toc_add_anon_aggr_type(_tmp814,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15B));});}case 11U: _LL1B: _tmp186=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).aggr_info;_tmp185=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).targs;_LL1C:
# 893
{union Cyc_Absyn_AggrInfoU _tmp15D=_tmp186;if((_tmp15D.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 897
struct Cyc_Absyn_Aggrdecl*_tmp15E=Cyc_Absyn_get_known_aggrdecl(_tmp186);
if(_tmp15E->expected_mem_kind){
# 900
if(_tmp15E->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0U,({
struct _dyneither_ptr _tmp815=(struct _dyneither_ptr)(_tmp15E->kind == Cyc_Absyn_UnionA?({const char*_tmp163="union";_tag_dyneither(_tmp163,sizeof(char),6U);}):({const char*_tmp164="struct";_tag_dyneither(_tmp164,sizeof(char),7U);}));_tmp787.f1=_tmp815;});_tmp787;});struct Cyc_String_pa_PrintArg_struct _tmp162=({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0U,({
struct _dyneither_ptr _tmp816=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp786.f1=_tmp816;});_tmp786;});void*_tmp15F[2U];_tmp15F[0]=& _tmp161,_tmp15F[1]=& _tmp162;({struct _dyneither_ptr _tmp817=({const char*_tmp160="%s %s was never defined.";_tag_dyneither(_tmp160,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp817,_tag_dyneither(_tmp15F,sizeof(void*),2U));});});}
# 908
if(_tmp15E->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp165=_tmp15E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields;
if(_tmp165 == 0)return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp165))->tl != 0;_tmp165=_tmp165->tl){;}{
void*_tmp166=((struct Cyc_Absyn_Aggrfield*)_tmp165->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp166))){
if(_tmp15E->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp169=({struct Cyc_String_pa_PrintArg_struct _tmp788;_tmp788.tag=0U,({struct _dyneither_ptr _tmp818=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp788.f1=_tmp818;});_tmp788;});void*_tmp167[1U];_tmp167[0]=& _tmp169;({struct _dyneither_ptr _tmp819=({const char*_tmp168="struct %s ended up being abstract.";_tag_dyneither(_tmp168,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp819,_tag_dyneither(_tmp167,sizeof(void*),1U));});});{
# 921
struct _RegionHandle _tmp16A=_new_region("r");struct _RegionHandle*r=& _tmp16A;_push_region(r);
{struct Cyc_List_List*_tmp16B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15E->tvs,_tmp185);
void*_tmp16C=Cyc_Tcutil_rsubstitute(r,_tmp16B,_tmp166);
if(Cyc_Toc_is_abstract_type(_tmp16C)){void*_tmp16D=Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp16D;}{
void*_tmp16E=Cyc_Toc_add_struct_type(_tmp15E->name,_tmp15E->tvs,_tmp185,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields);_npop_handler(0U);return _tmp16E;};}
# 922
;_pop_region(r);};}
# 927
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);};};};case 13U: _LL1D: _tmp187=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp149)->f1;_LL1E:
 return t;case 14U: _LL1F: _tmp188=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp149)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp188);return t;case 17U: _LL21: _tmp18C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f1;_tmp18B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f2;_tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f3;_tmp189=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f4;_LL22:
# 933
 if(_tmp189 == 0){
if(_tmp18B != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=17U,_tmp16F->f1=_tmp18C,_tmp16F->f2=0,_tmp16F->f3=_tmp18A,_tmp16F->f4=0;_tmp16F;});else{
return t;}}else{
# 938
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=17U,_tmp170->f1=_tmp18C,_tmp170->f2=0,_tmp170->f3=_tmp18A,({void*_tmp81A=Cyc_Toc_typ_to_c(_tmp189);_tmp170->f4=_tmp81A;});_tmp170;});}case 19U: _LL23: _LL24:
 return Cyc_Absyn_uint_typ;case 15U: _LL25: _LL26:
 return Cyc_Toc_rgn_typ();case 16U: _LL27: _LL28:
 return Cyc_Toc_dyn_rgn_typ();case 20U: _LL29: _LL2A:
# 944
 goto _LL2C;case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 23U: _LL2F: _LL30:
 goto _LL32;case 24U: _LL31: _LL32:
 goto _LL34;case 25U: _LL33: _LL34:
 return Cyc_Absyn_void_star_typ();case 18U: _LL35: _tmp18D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp149)->f1;_LL36:
# 954
 return t;case 27U: _LL37: _tmp18E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp149)->f1;_LL38:
# 956
 return t;case 28U: _LL39: _LL3A:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)){case 0U: _LL3B: _tmp18F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3C:
# 959
 Cyc_Toc_aggrdecl_to_c(_tmp18F,1);
if(_tmp18F->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_strctq);}case 1U: _LL3D: _tmp190=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3E:
# 964
 Cyc_Toc_enumdecl_to_c(_tmp190);
return t;default: _LL3F: _tmp192=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_tmp191=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f2;_LL40:
# 967
 Cyc_Toc_datatypedecl_to_c(_tmp192);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp191)));}}_LL0:;}
# 972
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp193=t;void*_tmp195;struct Cyc_Absyn_Tqual _tmp194;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->tag == 8U){_LL1: _tmp195=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).tq;_LL2:
# 975
 return({void*_tmp81B=Cyc_Absyn_star_typ(_tmp195,(void*)& Cyc_Absyn_HeapRgn_val,_tmp194,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp81B,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 982
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp196=Cyc_Tcutil_compress(t);void*_tmp197=_tmp196;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Datatypedecl*_tmp1A2;struct Cyc_Absyn_Datatypefield*_tmp1A1;struct Cyc_List_List*_tmp1A0;union Cyc_Absyn_AggrInfoU _tmp19F;void*_tmp19E;switch(*((int*)_tmp197)){case 0U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 0;case 6U: _LL5: _LL6:
 goto _LL8;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 9U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmp19E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).elt_type;_LL12:
 return Cyc_Toc_atomic_typ(_tmp19E);case 11U: _LL13: _tmp19F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp197)->f1).aggr_info;_LL14:
# 999
{union Cyc_Absyn_AggrInfoU _tmp198=_tmp19F;if((_tmp198.UnknownAggr).tag == 1){_LL26: _LL27:
 return 0;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 1003
struct Cyc_Absyn_Aggrdecl*_tmp199=Cyc_Absyn_get_known_aggrdecl(_tmp19F);
if(_tmp199->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp199->impl))->fields;for(0;_tmp19A != 0;_tmp19A=_tmp19A->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp19A->hd)->type))return 0;}}
return 1;};case 12U: _LL15: _tmp1A0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp197)->f2;_LL16:
# 1010
 for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1A0->hd)->type))return 0;}
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).tag == 2){_LL17: _tmp1A2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f1;_tmp1A1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f2;_LL18:
# 1014
 _tmp1A3=_tmp1A1->typs;goto _LL1A;}else{goto _LL23;}case 10U: _LL19: _tmp1A3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp197)->f1;_LL1A:
# 1016
 for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple12*)_tmp1A3->hd)).f2))return 0;}
return 1;case 3U: _LL1B: _LL1C:
# 1021
 goto _LL1E;case 5U: _LL1D: _LL1E:
 goto _LL20;case 16U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return 0;default: _LL23: _LL24:
({struct Cyc_String_pa_PrintArg_struct _tmp19D=({struct Cyc_String_pa_PrintArg_struct _tmp789;_tmp789.tag=0U,({struct _dyneither_ptr _tmp81C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp789.f1=_tmp81C;});_tmp789;});void*_tmp19B[1U];_tmp19B[0]=& _tmp19D;({struct _dyneither_ptr _tmp81D=({const char*_tmp19C="atomic_typ:  bad type %s";_tag_dyneither(_tmp19C,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81D,_tag_dyneither(_tmp19B,sizeof(void*),1U));});});}_LL0:;}
# 1029
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A4=Cyc_Tcutil_compress(t);void*_tmp1A5=_tmp1A4;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->tag == 5U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_typ;_LL2: {
# 1032
void*_tmp1A6=Cyc_Tcutil_compress(_tmp1A8);void*_tmp1A7=_tmp1A6;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A7)->tag == 0U){_LL6: _LL7:
 return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 1036
 return 0;}_LL0:;}
# 1040
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1044
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1049
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfoU _tmp1B5;switch(*((int*)_tmp1AA)){case 11U: _LL1: _tmp1B5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AA)->f1).aggr_info;_LL2: {
# 1052
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp81E=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81E,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1057
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp78A;_tmp78A.tag=0U,_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp78A;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp81F=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp81F,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp78B;_tmp78B.tag=0U,({struct _dyneither_ptr _tmp820=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp78B.f1=_tmp820;});_tmp78B;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp821=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp821,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1068
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1071
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1074
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 5U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_typ;_LL9:
# 1076
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp78C;_tmp78C.tag=0U,({struct _dyneither_ptr _tmp822=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp78C.f1=_tmp822;});_tmp78C;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp823=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp823,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1079
 return 0;}_LL0:;}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp824=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp824,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp825=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp825,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1098
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp826=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp826,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp827=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp827,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp828=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp828,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1110
return({struct Cyc_Absyn_Exp*_tmp829=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp829,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp82A=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp82A,({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp82B=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp82B,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1120
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp82C=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp82C,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1124
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp82D=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp82D,({void*_tmp1CC=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1129
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1137
int is_string=0;
{void*_tmp1CD=e->r;void*_tmp1CE=_tmp1CD;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1143
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp82F=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp82E=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=0U,_tmp1CF->f1=vd;_tmp1CF;}),0U);_tmp1D0->hd=_tmp82E;}),_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});Cyc_Toc_result_decls=_tmp82F;});
({struct Cyc_Absyn_Exp*_tmp830=Cyc_Absyn_var_exp(x,0U);xexp=_tmp830;});
({struct Cyc_Absyn_Exp*_tmp831=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp831;});}else{
# 1153
({struct Cyc_Absyn_Exp*_tmp833=({void*_tmp832=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp832,e);});xexp=_tmp833;});
# 1155
({struct Cyc_Absyn_Exp*_tmp835=({void*_tmp834=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp834,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp835;});}
# 1157
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D1[3U];({struct _tuple20*_tmp838=({struct _tuple20*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D1[0]=_tmp838;}),({
struct _tuple20*_tmp837=({struct _tuple20*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D1[1]=_tmp837;}),({
struct _tuple20*_tmp836=({struct _tuple20*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xplussz;_tmp1D4;});_tmp1D1[2]=_tmp836;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D1,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1194 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1198
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D7=nv;int*_tmp1D8;_LL1: _tmp1D8=_tmp1D7->in_lhs;_LL2:;
return*_tmp1D8;}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=0,_tmp1DB->fallthru_info=0,_tmp1DB->toplevel=1,({
# 1209
int*_tmp83A=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});_tmp1DB->in_lhs=_tmp83A;}),({
int*_tmp839=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DB->in_sizeof=_tmp839;}),_tmp1DB->rgn=r;_tmp1DB;});}
# 1214
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DC=e;struct _dyneither_ptr**_tmp1E3;struct _dyneither_ptr**_tmp1E2;struct Cyc_Toc_FallthruInfo*_tmp1E1;int _tmp1E0;int*_tmp1DF;int*_tmp1DE;_LL1: _tmp1E3=_tmp1DC->break_lab;_tmp1E2=_tmp1DC->continue_lab;_tmp1E1=_tmp1DC->fallthru_info;_tmp1E0=_tmp1DC->toplevel;_tmp1DF=_tmp1DC->in_lhs;_tmp1DE=_tmp1DC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=_tmp1E3,_tmp1DD->continue_lab=_tmp1E2,_tmp1DD->fallthru_info=_tmp1E1,_tmp1DD->toplevel=_tmp1E0,_tmp1DD->in_lhs=_tmp1DF,_tmp1DD->in_sizeof=_tmp1DE,_tmp1DD->rgn=r;_tmp1DD;});}
# 1219
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E4=e;struct _dyneither_ptr**_tmp1EB;struct _dyneither_ptr**_tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;int _tmp1E8;int*_tmp1E7;int*_tmp1E6;_LL1: _tmp1EB=_tmp1E4->break_lab;_tmp1EA=_tmp1E4->continue_lab;_tmp1E9=_tmp1E4->fallthru_info;_tmp1E8=_tmp1E4->toplevel;_tmp1E7=_tmp1E4->in_lhs;_tmp1E6=_tmp1E4->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->break_lab=_tmp1EB,_tmp1E5->continue_lab=_tmp1EA,_tmp1E5->fallthru_info=_tmp1E9,_tmp1E5->toplevel=0,_tmp1E5->in_lhs=_tmp1E7,_tmp1E5->in_sizeof=_tmp1E6,_tmp1E5->rgn=r;_tmp1E5;});}
# 1223
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;struct _dyneither_ptr**_tmp1F3;struct _dyneither_ptr**_tmp1F2;struct Cyc_Toc_FallthruInfo*_tmp1F1;int _tmp1F0;int*_tmp1EF;int*_tmp1EE;_LL1: _tmp1F3=_tmp1EC->break_lab;_tmp1F2=_tmp1EC->continue_lab;_tmp1F1=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1EF=_tmp1EC->in_lhs;_tmp1EE=_tmp1EC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=_tmp1F3,_tmp1ED->continue_lab=_tmp1F2,_tmp1ED->fallthru_info=_tmp1F1,_tmp1ED->toplevel=1,_tmp1ED->in_lhs=_tmp1EF,_tmp1ED->in_sizeof=_tmp1EE,_tmp1ED->rgn=r;_tmp1ED;});}
# 1227
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F4=e;int*_tmp1F5;_LL1: _tmp1F5=_tmp1F4->in_lhs;_LL2:;
*_tmp1F5=b;}
# 1231
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F6=e;int*_tmp1F8;_LL1: _tmp1F8=_tmp1F6->in_sizeof;_LL2:;{
int _tmp1F7=*_tmp1F8;
*_tmp1F8=b;
return _tmp1F7;};}
# 1237
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F9=e;int*_tmp1FA;_LL1: _tmp1FA=_tmp1F9->in_sizeof;_LL2:;
return*_tmp1FA;}
# 1244
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FB=e;struct _dyneither_ptr**_tmp202;struct _dyneither_ptr**_tmp201;struct Cyc_Toc_FallthruInfo*_tmp200;int _tmp1FF;int*_tmp1FE;int*_tmp1FD;_LL1: _tmp202=_tmp1FB->break_lab;_tmp201=_tmp1FB->continue_lab;_tmp200=_tmp1FB->fallthru_info;_tmp1FF=_tmp1FB->toplevel;_tmp1FE=_tmp1FB->in_lhs;_tmp1FD=_tmp1FB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));_tmp1FC->break_lab=0,_tmp1FC->continue_lab=0,_tmp1FC->fallthru_info=_tmp200,_tmp1FC->toplevel=_tmp1FF,_tmp1FC->in_lhs=_tmp1FE,_tmp1FC->in_sizeof=_tmp1FD,_tmp1FC->rgn=r;_tmp1FC;});}
# 1250
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1254
struct Cyc_Toc_Env*_tmp203=e;struct _dyneither_ptr**_tmp20C;struct _dyneither_ptr**_tmp20B;struct Cyc_Toc_FallthruInfo*_tmp20A;int _tmp209;int*_tmp208;int*_tmp207;_LL1: _tmp20C=_tmp203->break_lab;_tmp20B=_tmp203->continue_lab;_tmp20A=_tmp203->fallthru_info;_tmp209=_tmp203->toplevel;_tmp208=_tmp203->in_lhs;_tmp207=_tmp203->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp206=_region_malloc(r,sizeof(*_tmp206));
_tmp206->label=fallthru_l,_tmp206->binders=fallthru_binders;_tmp206;});
return({struct Cyc_Toc_Env*_tmp205=_region_malloc(r,sizeof(*_tmp205));({struct _dyneither_ptr**_tmp83B=({struct _dyneither_ptr**_tmp204=_region_malloc(r,sizeof(*_tmp204));*_tmp204=break_l;_tmp204;});_tmp205->break_lab=_tmp83B;}),_tmp205->continue_lab=_tmp20B,_tmp205->fallthru_info=fi,_tmp205->toplevel=_tmp209,_tmp205->in_lhs=_tmp208,_tmp205->in_sizeof=_tmp207,_tmp205->rgn=r;_tmp205;});};}
# 1260
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1262
struct Cyc_Toc_Env*_tmp20D=e;struct _dyneither_ptr**_tmp215;struct _dyneither_ptr**_tmp214;struct Cyc_Toc_FallthruInfo*_tmp213;int _tmp212;int*_tmp211;int*_tmp210;_LL1: _tmp215=_tmp20D->break_lab;_tmp214=_tmp20D->continue_lab;_tmp213=_tmp20D->fallthru_info;_tmp212=_tmp20D->toplevel;_tmp211=_tmp20D->in_lhs;_tmp210=_tmp20D->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));({struct _dyneither_ptr**_tmp83C=({struct _dyneither_ptr**_tmp20E=_region_malloc(r,sizeof(*_tmp20E));*_tmp20E=break_l;_tmp20E;});_tmp20F->break_lab=_tmp83C;}),_tmp20F->continue_lab=_tmp214,_tmp20F->fallthru_info=0,_tmp20F->toplevel=_tmp212,_tmp20F->in_lhs=_tmp211,_tmp20F->in_sizeof=_tmp210,_tmp20F->rgn=r;_tmp20F;});}
# 1269
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1272
struct Cyc_Toc_Env*_tmp216=e;struct _dyneither_ptr**_tmp21E;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21C;int _tmp21B;int*_tmp21A;int*_tmp219;_LL1: _tmp21E=_tmp216->break_lab;_tmp21D=_tmp216->continue_lab;_tmp21C=_tmp216->fallthru_info;_tmp21B=_tmp216->toplevel;_tmp21A=_tmp216->in_lhs;_tmp219=_tmp216->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->break_lab=0,_tmp218->continue_lab=_tmp21D,({struct Cyc_Toc_FallthruInfo*_tmp83D=({struct Cyc_Toc_FallthruInfo*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->label=next_l,_tmp217->binders=0;_tmp217;});_tmp218->fallthru_info=_tmp83D;}),_tmp218->toplevel=_tmp21B,_tmp218->in_lhs=_tmp21A,_tmp218->in_sizeof=_tmp219,_tmp218->rgn=r;_tmp218;});}
# 1285 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1288
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1290
void*_tmp21F=e->annot;void*_tmp220=_tmp21F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1293
void*_tmp221=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp222=_tmp221;union Cyc_Absyn_Constraint*_tmp225;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->tag == 5U){_LLC: _tmp225=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->f1).ptr_atts).nullable;_LLD:
# 1296
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp225);}else{_LLE: _LLF:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp83E=({const char*_tmp224="Toc::is_nullable";_tag_dyneither(_tmp224,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83E,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1301
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1303
({void*_tmp226=0U;({unsigned int _tmp840=e->loc;struct _dyneither_ptr _tmp83F=({const char*_tmp227="NULL pointer check will definitely fail";_tag_dyneither(_tmp227,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp840,_tmp83F,_tag_dyneither(_tmp226,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1309
 return 1;}else{_LL9: _LLA:
({void*_tmp228=0U;({struct _dyneither_ptr _tmp841=({const char*_tmp229="need_null_check";_tag_dyneither(_tmp229,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp841,_tag_dyneither(_tmp228,sizeof(void*),0U));});});}}}}_LL0:;}
# 1314
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp22A=e->annot;void*_tmp22B=_tmp22A;struct Cyc_List_List*_tmp231;struct Cyc_List_List*_tmp230;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp230=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL2:
 return _tmp230;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp231=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL4:
 return _tmp231;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1319
({void*_tmp22C=0U;({unsigned int _tmp843=e->loc;struct _dyneither_ptr _tmp842=({const char*_tmp22D="dereference of NULL pointer";_tag_dyneither(_tmp22D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp843,_tmp842,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp22E=0U;({struct _dyneither_ptr _tmp844=({const char*_tmp22F="get_relns";_tag_dyneither(_tmp22F,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp844,_tag_dyneither(_tmp22E,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp235[8U]="*bogus*";
# 1329
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1339 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1342
struct Cyc_Absyn_Vardecl*x;
{void*_tmp232=a->r;void*_tmp233=_tmp232;void*_tmp236;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->tag == 1U){_LL1: _tmp236=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->f1;_LL2: {
# 1345
struct Cyc_Absyn_Vardecl*_tmp234=Cyc_Tcutil_nonesc_vardecl(_tmp236);
if(_tmp234 == 0)goto _LL4;
x=_tmp234;
goto _LL0;}}else{_LL3: _LL4: {
# 1351
static struct _dyneither_ptr bogus_string={_tmp235,_tmp235,_tmp235 + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1354
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1360
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1366
void*_tmp237=a_typ;
void*_tmp238=x->type;
x->type=_tmp237;{
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1375
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp237);
if(bound != 0){
# 1378
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp845=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp845;});}
# 1385
if(valid_rop_i){
struct Cyc_List_List*_tmp239=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1393
int _tmp23A=Cyc_Relations_consistent_relations(_tmp239);
x->type=_tmp238;
if(!_tmp23A)return 1;}
# 1398
inner_loop: {
void*_tmp23B=i->r;void*_tmp23C=_tmp23B;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_Absyn_Exp*_tmp241;void*_tmp240;struct Cyc_Absyn_Exp*_tmp23F;switch(*((int*)_tmp23C)){case 14U: _LL6: _tmp240=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f1;_tmp23F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23C)->f2;_LL7:
 i=_tmp23F;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl != 0){_LL8: _tmp242=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->hd;_tmp241=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23C)->f2)->tl)->hd;_LL9: {
# 1404
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp241,& rop_z)){
# 1407
struct Cyc_List_List*_tmp23D=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1409
int _tmp23E=Cyc_Relations_consistent_relations(_tmp23D);
x->type=_tmp238;
return !_tmp23E;}
# 1413
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1416
x->type=_tmp238;
return 0;};};}
# 1420
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp243=0U;({struct _dyneither_ptr _tmp846=({const char*_tmp244="Missing type in primop ";_tag_dyneither(_tmp244,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp846,_tag_dyneither(_tmp243,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1424
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->f1=Cyc_Toc_mt_tq,({void*_tmp847=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp245->f2=_tmp847;});_tmp245;});}
# 1429
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp246=e->r;void*_tmp247=_tmp246;struct Cyc_Absyn_Exp*_tmp24F;int _tmp24E;struct Cyc_Absyn_Exp*_tmp24D;int _tmp24C;struct Cyc_List_List*_tmp24B;switch(*((int*)_tmp247)){case 26U: _LL1: _tmp24B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_LL2:
# 1432
{struct Cyc_List_List*_tmp248=_tmp24B;for(0;_tmp248 != 0;_tmp248=_tmp248->tl){
if((*((struct _tuple20*)_tmp248->hd)).f1 != 0)
({void*_tmp249=0U;({struct _dyneither_ptr _tmp848=({const char*_tmp24A="array designators for abstract-field initialization";_tag_dyneither(_tmp24A,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp848,_tag_dyneither(_tmp249,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp849=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp24B),0U);_tmp24D=_tmp849;});_tmp24C=0;goto _LL4;case 27U: _LL3: _tmp24D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f2;_tmp24C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247)->f4;_LL4:
 _tmp24F=_tmp24D;_tmp24E=_tmp24C;goto _LL6;case 28U: _LL5: _tmp24F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f1;_tmp24E=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247)->f3;_LL6:
# 1438
({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Absyn_copy_exp(_tmp24F);_tmp24F=_tmp84A;});
return _tmp24E?({struct Cyc_Absyn_Exp*_tmp84B=_tmp24F;Cyc_Absyn_add_exp(_tmp84B,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp24F;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1447
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1455
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp250=e->r;void*_tmp251=_tmp250;struct Cyc_List_List*_tmp252;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->tag == 29U){_LL1: _tmp252=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp251)->f3;_LL2:
 dles=_tmp252;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1461
{void*_tmp253=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp254=_tmp253;struct Cyc_Absyn_Aggrdecl*_tmp255;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp255=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp254)->f1).aggr_info).KnownAggr).val;_LL7:
# 1463
 if(_tmp255->impl == 0)
return 0;
if(_tmp255->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp255->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{_LL8: _LL9:
# 1471
 return 0;}_LL5:;}
# 1473
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp256=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp257=(struct _tuple20*)dles->hd;struct _tuple20*_tmp258=_tmp257;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_Exp*_tmp260;_LLB: _tmp261=_tmp258->f1;_tmp260=_tmp258->f2;_LLC:;{
struct _dyneither_ptr*_tmp259=Cyc_Absyn_designatorlist_to_fieldname(_tmp261);
if(!Cyc_strptrcmp(_tmp259,_tmp256->name)){
struct Cyc_Absyn_Exp*_tmp25A=Cyc_Toc_get_varsizeexp(_tmp260);
if(_tmp25A != 0)
return _tmp25A;{
void*_tmp25B=Cyc_Tcutil_compress(_tmp256->type);void*_tmp25C=_tmp25B;void*_tmp25F;struct Cyc_Absyn_Exp*_tmp25E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->tag == 8U){_LLE: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).elt_type;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25C)->f1).num_elts;_LLF:
# 1488
 if(_tmp25E == 0  || !Cyc_Toc_is_zero(_tmp25E))
return 0;
# 1494
return({struct Cyc_Absyn_Exp*_tmp84E=({struct Cyc_Absyn_Exp*_tmp84D=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp84D,({struct Cyc_Absyn_Exp*_tmp25D[2U];_tmp25D[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp260)),({
# 1496
struct Cyc_Absyn_Exp*_tmp84C=Cyc_Absyn_sizeoftyp_exp(_tmp25F,0U);_tmp25D[1]=_tmp84C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1494
Cyc_Absyn_add_exp(_tmp84E,
# 1497
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1502
({void*_tmp262=0U;({struct _dyneither_ptr _tmp84F=({const char*_tmp263="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp263,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84F,_tag_dyneither(_tmp262,sizeof(void*),0U));});});};}
# 1505
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp264=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp264 != 0;_tmp264=_tmp264->tl){
struct Cyc_Absyn_Aggrfield*_tmp265=(struct Cyc_Absyn_Aggrfield*)_tmp264->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp265->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1512
({void*_tmp266=0U;({struct _dyneither_ptr _tmp851=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp78D;_tmp78D.tag=0U,_tmp78D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp78D;});void*_tmp267[1U];_tmp267[0]=& _tmp269;({struct _dyneither_ptr _tmp850=({const char*_tmp268="get_member_offset %s failed";_tag_dyneither(_tmp268,sizeof(char),28U);});Cyc_aprintf(_tmp850,_tag_dyneither(_tmp267,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp851,_tag_dyneither(_tmp266,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1516
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1519
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1522
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->tag=5U,_tmp26A->f1=e1,_tmp26A->f2=incr;_tmp26A;}),0U);}
# 1526
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1535
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1537
void*_tmp26B=e1->r;void*_tmp26C=_tmp26B;struct Cyc_Absyn_Exp*_tmp274;struct _dyneither_ptr*_tmp273;int _tmp272;int _tmp271;void*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Stmt*_tmp26E;switch(*((int*)_tmp26C)){case 37U: _LL1: _tmp26E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26E,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp270=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp26F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp26F,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp274=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f1;_tmp273=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f2;_tmp272=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f3;_tmp271=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26C)->f4;_LL6:
# 1542
 e1->r=_tmp274->r;
({struct Cyc_Absyn_Exp*_tmp854=e1;struct Cyc_List_List*_tmp853=({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->hd=_tmp273,_tmp26D->tl=fs;_tmp26D;});struct Cyc_Absyn_Exp*(*_tmp852)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp854,_tmp853,_tmp852,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1550
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1552
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp855=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp855;});}
({void*_tmp856=(f(e1_copy,f_env))->r;e1->r=_tmp856;});
goto _LL0;}}_LL0:;}
# 1558
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1560
void*_tmp275=s->r;void*_tmp276=_tmp275;struct Cyc_Absyn_Stmt*_tmp27D;struct Cyc_Absyn_Decl*_tmp27C;struct Cyc_Absyn_Stmt*_tmp27B;struct Cyc_Absyn_Exp*_tmp27A;switch(*((int*)_tmp276)){case 1U: _LL1: _tmp27A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp27A,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp27C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f1;_tmp27B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL4:
# 1563
 Cyc_Toc_lvalue_assign_stmt(_tmp27B,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp276)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27D,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp279=({struct Cyc_String_pa_PrintArg_struct _tmp78E;_tmp78E.tag=0U,({struct _dyneither_ptr _tmp857=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp78E.f1=_tmp857;});_tmp78E;});void*_tmp277[1U];_tmp277[0]=& _tmp279;({struct _dyneither_ptr _tmp858=({const char*_tmp278="lvalue_assign_stmt: %s";_tag_dyneither(_tmp278,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp858,_tag_dyneither(_tmp277,sizeof(void*),1U));});});}_LL0:;}
# 1569
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1573
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27E=e->r;void*_tmp27F=_tmp27E;struct Cyc_Absyn_Stmt*_tmp286;struct Cyc_Absyn_Exp*_tmp285;void**_tmp284;struct Cyc_Absyn_Exp**_tmp283;switch(*((int*)_tmp27F)){case 14U: _LL1: _tmp284=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_tmp283=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27F)->f2;_LL2:
# 1576
({struct Cyc_Absyn_Exp*_tmp859=Cyc_Toc_push_address_exp(*_tmp283);*_tmp283=_tmp859;});
({void*_tmp85A=Cyc_Absyn_cstar_typ(*_tmp284,Cyc_Toc_mt_tq);*_tmp284=_tmp85A;});
return e;case 20U: _LL3: _tmp285=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL4:
# 1580
 return _tmp285;case 37U: _LL5: _tmp286=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp27F)->f1;_LL6:
# 1584
 Cyc_Toc_push_address_stmt(_tmp286);
return e;default: _LL7: _LL8:
# 1587
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp78F;_tmp78F.tag=0U,({struct _dyneither_ptr _tmp85B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp78F.f1=_tmp85B;});_tmp78F;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmp85C=({const char*_tmp281="can't take & of exp %s";_tag_dyneither(_tmp281,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85C,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}_LL0:;}
# 1592
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp287=s->r;void*_tmp288=_tmp287;struct Cyc_Absyn_Exp**_tmp28E;struct Cyc_Absyn_Stmt*_tmp28D;struct Cyc_Absyn_Stmt*_tmp28C;switch(*((int*)_tmp288)){case 2U: _LL1: _tmp28C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL2:
 _tmp28D=_tmp28C;goto _LL4;case 12U: _LL3: _tmp28D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28D);goto _LL0;case 1U: _LL5: _tmp28E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp85D=Cyc_Toc_push_address_exp(*_tmp28E);*_tmp28E=_tmp85D;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp28B=({struct Cyc_String_pa_PrintArg_struct _tmp790;_tmp790.tag=0U,({struct _dyneither_ptr _tmp85E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp790.f1=_tmp85E;});_tmp790;});void*_tmp289[1U];_tmp289[0]=& _tmp28B;({struct _dyneither_ptr _tmp85F=({const char*_tmp28A="can't take & of stmt %s";_tag_dyneither(_tmp28A,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85F,_tag_dyneither(_tmp289,sizeof(void*),1U));});});}_LL0:;}
# 1601
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp28F=Cyc_Tcutil_compress(t);void*_tmp290=_tmp28F;struct Cyc_Absyn_PtrInfo _tmp293;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->tag == 5U){_LL1: _tmp293=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp290)->f1;_LL2:
 return _tmp293;}else{_LL3: _LL4:
({void*_tmp291=0U;({struct _dyneither_ptr _tmp860=({const char*_tmp292="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp292,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp860,_tag_dyneither(_tmp291,sizeof(void*),0U));});});}_LL0:;}
# 1611
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp294=Cyc_Tcutil_compress(t);void*_tmp295=_tmp294;int _tmp2A1;enum Cyc_Absyn_Sign _tmp2A0;enum Cyc_Absyn_Sign _tmp29F;enum Cyc_Absyn_Sign _tmp29E;enum Cyc_Absyn_Sign _tmp29D;enum Cyc_Absyn_Sign _tmp29C;switch(*((int*)_tmp295)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp861=Cyc_Absyn_null_exp(0U);res=_tmp861;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp29C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp29C,'\000'),0U);res=_tmp862;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp29D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp863=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29D,0),0U);res=_tmp863;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp29E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LLC:
# 1619
 _tmp29F=_tmp29E;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp29F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LLE:
# 1621
({struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp29F,0),0U);res=_tmp864;});goto _LL0;default: _LLF: _tmp2A0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp295)->f1;_LL10:
# 1623
({struct Cyc_Absyn_Exp*_tmp865=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp2A0,(long long)0),0U);res=_tmp865;});goto _LL0;}case 13U: _LL7: _LL8:
# 1617
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp29E=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp295)->f1){case 0U: _LL11: _LL12:
# 1624
({struct Cyc_Absyn_Exp*_tmp866=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp296="0.0F";_tag_dyneither(_tmp296,sizeof(char),5U);}),0),0U);res=_tmp866;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmp867=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp297="0.0";_tag_dyneither(_tmp297,sizeof(char),4U);}),1),0U);res=_tmp867;});goto _LL0;default: _LL15: _tmp2A1=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp295)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmp869=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp868=({const char*_tmp298="0.0L";_tag_dyneither(_tmp298,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp868,_tmp2A1);}),0U);res=_tmp869;});goto _LL0;}default: _LL17: _LL18:
# 1628
({struct Cyc_String_pa_PrintArg_struct _tmp29B=({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0U,({struct _dyneither_ptr _tmp86A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp791.f1=_tmp86A;});_tmp791;});void*_tmp299[1U];_tmp299[0]=& _tmp29B;({struct _dyneither_ptr _tmp86B=({const char*_tmp29A="found non-zero type %s in generate_zero";_tag_dyneither(_tmp29A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp86B,_tag_dyneither(_tmp299,sizeof(void*),1U));});});}_LL0:;}
# 1630
res->topt=t;
return res;}
# 1637
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1692 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp2A2=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A3=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A4=Cyc_Absyn_cstar_typ(_tmp2A2,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A5=({struct Cyc_Core_Opt*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->v=_tmp2A4;_tmp2D6;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A6=e1->r;void*_tmp2A7=_tmp2A6;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;switch(*((int*)_tmp2A7)){case 20U: _LL1: _tmp2AB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A7)->f1;_LL2:
# 1700
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp86C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AB,0,Cyc_Absyn_Other_coercion,0U);_tmp2AB=_tmp86C;});
_tmp2AB->topt=fat_ptr_type;}
# 1704
Cyc_Toc_exp_to_c(nv,_tmp2AB);xinit=_tmp2AB;goto _LL0;case 23U: _LL3: _tmp2AD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A7)->f1;_tmp2AC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A7)->f2;_LL4:
# 1706
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AD,0,Cyc_Absyn_Other_coercion,0U);_tmp2AD=_tmp86D;});
_tmp2AD->topt=fat_ptr_type;}
# 1710
Cyc_Toc_exp_to_c(nv,_tmp2AD);Cyc_Toc_exp_to_c(nv,_tmp2AC);
({struct Cyc_Absyn_Exp*_tmp870=({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp86F,({struct Cyc_Absyn_Exp*_tmp2A8[3U];_tmp2A8[0]=_tmp2AD,({
struct Cyc_Absyn_Exp*_tmp86E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A8[1]=_tmp86E;}),_tmp2A8[2]=_tmp2AC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A8,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1711
xinit=_tmp870;});
# 1713
goto _LL0;default: _LL5: _LL6:
({void*_tmp2A9=0U;({struct _dyneither_ptr _tmp871=({const char*_tmp2AA="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2AA,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp871,_tag_dyneither(_tmp2A9,sizeof(void*),0U));});});}_LL0:;}{
# 1716
struct _tuple1*_tmp2AE=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2AF=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2AF;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2B0=({struct Cyc_Absyn_Vardecl*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->sc=Cyc_Absyn_Public,_tmp2D5->name=_tmp2AE,_tmp2D5->varloc=0U,_tmp2D5->tq=Cyc_Toc_mt_tq,_tmp2D5->type=_tmp2A3,_tmp2D5->initializer=xinit,_tmp2D5->rgn=0,_tmp2D5->attributes=0,_tmp2D5->escapes=0;_tmp2D5;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B1=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->tag=4U,_tmp2D4->f1=_tmp2B0;_tmp2D4;});
struct Cyc_Absyn_Exp*_tmp2B2=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);
_tmp2B2->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_deref_exp(_tmp2B2,0U);
_tmp2B3->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp2B3);{
struct _tuple1*_tmp2B4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B5=({struct Cyc_Absyn_Vardecl*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->sc=Cyc_Absyn_Public,_tmp2D3->name=_tmp2B4,_tmp2D3->varloc=0U,_tmp2D3->tq=Cyc_Toc_mt_tq,_tmp2D3->type=_tmp2A2,_tmp2D3->initializer=_tmp2B3,_tmp2D3->rgn=0,_tmp2D3->attributes=0,_tmp2D3->escapes=0;_tmp2D3;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->tag=4U,_tmp2D2->f1=_tmp2B5;_tmp2D2;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B7=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);
_tmp2B7->topt=_tmp2B3->topt;
({struct Cyc_Absyn_Exp*_tmp872=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B7,e2,0U);z_init=_tmp872;});
z_init->topt=_tmp2B7->topt;}
# 1735
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B9=({struct Cyc_Absyn_Vardecl*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->sc=Cyc_Absyn_Public,_tmp2D1->name=_tmp2B8,_tmp2D1->varloc=0U,_tmp2D1->tq=Cyc_Toc_mt_tq,_tmp2D1->type=_tmp2A2,_tmp2D1->initializer=z_init,_tmp2D1->rgn=0,_tmp2D1->attributes=0,_tmp2D1->escapes=0;_tmp2D1;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BA=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->tag=4U,_tmp2D0->f1=_tmp2B9;_tmp2D0;});
# 1741
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_varb_exp((void*)_tmp2B6,0U);_tmp2BB->topt=_tmp2B3->topt;{
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2BB,_tmp2BC,0U);
_tmp2BD->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BD);{
# 1747
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_varb_exp((void*)_tmp2BA,0U);_tmp2BE->topt=_tmp2B3->topt;{
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2BE,_tmp2BF,0U);
_tmp2C0->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2C0);{
# 1753
struct Cyc_List_List*_tmp2C1=({struct Cyc_Absyn_Exp*_tmp2CF[2U];({struct Cyc_Absyn_Exp*_tmp874=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);_tmp2CF[0]=_tmp874;}),({
struct Cyc_Absyn_Exp*_tmp873=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2CF[1]=_tmp873;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2CF,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2C2=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp876=({struct Cyc_Absyn_Exp*_tmp875=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2C1,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp875,_tmp2C2,0U);});xsize=_tmp876;});{
# 1760
struct Cyc_Absyn_Exp*_tmp2C3=({struct Cyc_Absyn_Exp*_tmp877=xsize;Cyc_Absyn_and_exp(_tmp877,Cyc_Absyn_and_exp(_tmp2BD,_tmp2C0,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C4=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C5=({struct Cyc_Absyn_Exp*_tmp878=Cyc_Absyn_varb_exp((void*)_tmp2B1,0U);Cyc_Toc_member_exp(_tmp878,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp879=Cyc_Toc_cast_it(_tmp2A4,_tmp2C5);_tmp2C5=_tmp879;});{
struct Cyc_Absyn_Exp*_tmp2C6=Cyc_Absyn_deref_exp(_tmp2C5,0U);
struct Cyc_Absyn_Exp*_tmp2C7=({struct Cyc_Absyn_Exp*_tmp87A=_tmp2C6;Cyc_Absyn_assign_exp(_tmp87A,Cyc_Absyn_var_exp(_tmp2B8,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C8=Cyc_Absyn_exp_stmt(_tmp2C7,0U);
({struct Cyc_Absyn_Stmt*_tmp87E=({struct Cyc_Absyn_Stmt*_tmp87D=({struct Cyc_Absyn_Exp*_tmp87C=_tmp2C3;struct Cyc_Absyn_Stmt*_tmp87B=_tmp2C4;Cyc_Absyn_ifthenelse_stmt(_tmp87C,_tmp87B,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp87D,_tmp2C8,0U);});_tmp2C8=_tmp87E;});
({struct Cyc_Absyn_Stmt*_tmp881=({struct Cyc_Absyn_Decl*_tmp880=({struct Cyc_Absyn_Decl*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));({void*_tmp87F=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9->tag=0U,_tmp2C9->f1=_tmp2B9;_tmp2C9;});_tmp2CA->r=_tmp87F;}),_tmp2CA->loc=0U;_tmp2CA;});Cyc_Absyn_decl_stmt(_tmp880,_tmp2C8,0U);});_tmp2C8=_tmp881;});
({struct Cyc_Absyn_Stmt*_tmp884=({struct Cyc_Absyn_Decl*_tmp883=({struct Cyc_Absyn_Decl*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));({void*_tmp882=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));_tmp2CB->tag=0U,_tmp2CB->f1=_tmp2B5;_tmp2CB;});_tmp2CC->r=_tmp882;}),_tmp2CC->loc=0U;_tmp2CC;});Cyc_Absyn_decl_stmt(_tmp883,_tmp2C8,0U);});_tmp2C8=_tmp884;});
({struct Cyc_Absyn_Stmt*_tmp887=({struct Cyc_Absyn_Decl*_tmp886=({struct Cyc_Absyn_Decl*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));({void*_tmp885=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->tag=0U,_tmp2CD->f1=_tmp2B0;_tmp2CD;});_tmp2CE->r=_tmp885;}),_tmp2CE->loc=0U;_tmp2CE;});Cyc_Absyn_decl_stmt(_tmp886,_tmp2C8,0U);});_tmp2C8=_tmp887;});
({void*_tmp888=Cyc_Toc_stmt_exp_r(_tmp2C8);e->r=_tmp888;});};};};};};};};};};}
# 1718
;_pop_region(rgn2);};}
# 1786 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1790
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D7=Cyc_Tcutil_compress(aggrtype);void*_tmp2D8=_tmp2D7;union Cyc_Absyn_AggrInfoU _tmp2DD;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D8)->tag == 11U){_LL1: _tmp2DD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D8)->f1).aggr_info;_LL2:
# 1793
({struct Cyc_Absyn_Aggrdecl*_tmp889=Cyc_Absyn_get_known_aggrdecl(_tmp2DD);ad=_tmp889;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp2D9=0U;({struct _dyneither_ptr _tmp88C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DC=({struct Cyc_String_pa_PrintArg_struct _tmp792;_tmp792.tag=0U,({
struct _dyneither_ptr _tmp88A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp792.f1=_tmp88A;});_tmp792;});void*_tmp2DA[1U];_tmp2DA[0]=& _tmp2DC;({struct _dyneither_ptr _tmp88B=({const char*_tmp2DB="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2DB,sizeof(char),51U);});Cyc_aprintf(_tmp88B,_tag_dyneither(_tmp2DA,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp88C,_tag_dyneither(_tmp2D9,sizeof(void*),0U));});});}_LL0:;}{
# 1797
struct _tuple1*_tmp2DE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2DF=Cyc_Absyn_var_exp(_tmp2DE,0U);
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2E1=Cyc_Absyn_aggrarrow_exp(_tmp2DF,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2E2=Cyc_Absyn_neq_exp(_tmp2E1,_tmp2E0,0U);
struct Cyc_Absyn_Exp*_tmp2E3=Cyc_Absyn_aggrarrow_exp(_tmp2DF,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E4=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E3,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E5=({struct Cyc_Absyn_Exp*_tmp88E=_tmp2E2;struct Cyc_Absyn_Stmt*_tmp88D=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp88E,_tmp88D,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E6=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E7=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E8=({struct _tuple1*_tmp891=_tmp2DE;void*_tmp890=_tmp2E6;struct Cyc_Absyn_Exp*_tmp88F=_tmp2E7;Cyc_Absyn_declare_stmt(_tmp891,_tmp890,_tmp88F,Cyc_Absyn_seq_stmt(_tmp2E5,_tmp2E4,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2E8);}else{
# 1811
struct Cyc_Absyn_Exp*_tmp2E9=({struct Cyc_Absyn_Exp*_tmp892=aggrproj(_tmp2DF,f,0U);Cyc_Toc_member_exp(_tmp892,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2EA=Cyc_Absyn_neq_exp(_tmp2E9,_tmp2E0,0U);
struct Cyc_Absyn_Exp*_tmp2EB=({struct Cyc_Absyn_Exp*_tmp893=aggrproj(_tmp2DF,f,0U);Cyc_Toc_member_exp(_tmp893,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2EC=Cyc_Absyn_exp_stmt(_tmp2EB,0U);
struct Cyc_Absyn_Stmt*_tmp2ED=({struct Cyc_Absyn_Exp*_tmp895=_tmp2EA;struct Cyc_Absyn_Stmt*_tmp894=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp895,_tmp894,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2EE=({struct _tuple1*_tmp898=_tmp2DE;void*_tmp897=e1_c_type;struct Cyc_Absyn_Exp*_tmp896=e1;Cyc_Absyn_declare_stmt(_tmp898,_tmp897,_tmp896,Cyc_Absyn_seq_stmt(_tmp2ED,_tmp2EC,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EE);}};}
# 1821
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1824
void*_tmp2EF=Cyc_Tcutil_compress(t);void*_tmp2F0=_tmp2EF;union Cyc_Absyn_AggrInfoU _tmp2F7;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F0)->tag == 11U){_LL1: _tmp2F7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F0)->f1).aggr_info;_LL2: {
# 1826
struct Cyc_Absyn_Aggrdecl*_tmp2F1=Cyc_Absyn_get_known_aggrdecl(_tmp2F7);
({int _tmp899=Cyc_Toc_get_member_offset(_tmp2F1,f);*f_tag=_tmp899;});{
# 1829
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F5=({struct Cyc_String_pa_PrintArg_struct _tmp794;_tmp794.tag=0U,_tmp794.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2F1->name).f2);_tmp794;});struct Cyc_String_pa_PrintArg_struct _tmp2F6=({struct Cyc_String_pa_PrintArg_struct _tmp793;_tmp793.tag=0U,_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp793;});void*_tmp2F3[2U];_tmp2F3[0]=& _tmp2F5,_tmp2F3[1]=& _tmp2F6;({struct _dyneither_ptr _tmp89A=({const char*_tmp2F4="_union_%s_%s";_tag_dyneither(_tmp2F4,sizeof(char),13U);});Cyc_aprintf(_tmp89A,_tag_dyneither(_tmp2F3,sizeof(void*),2U));});});
({void*_tmp89B=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));*_tmp2F2=str;_tmp2F2;}));*tagged_member_type=_tmp89B;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2F1->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1840
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1843
void*_tmp2F8=e->r;void*_tmp2F9=_tmp2F8;struct Cyc_Absyn_Exp*_tmp305;struct _dyneither_ptr*_tmp304;int*_tmp303;struct Cyc_Absyn_Exp*_tmp302;struct _dyneither_ptr*_tmp301;int*_tmp300;struct Cyc_Absyn_Exp*_tmp2FF;switch(*((int*)_tmp2F9)){case 14U: _LL1: _tmp2FF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_LL2:
({void*_tmp2FA=0U;({struct _dyneither_ptr _tmp89C=({const char*_tmp2FB="cast on lhs!";_tag_dyneither(_tmp2FB,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp89C,_tag_dyneither(_tmp2FA,sizeof(void*),0U));});});case 21U: _LL3: _tmp302=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f1;_tmp301=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_tmp300=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9)->f4;_LL4:
# 1846
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp302->topt),_tmp301,f_tag,tagged_member_type,clear_read,_tmp300);case 22U: _LL5: _tmp305=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f1;_tmp304=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f2;_tmp303=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9)->f4;_LL6: {
# 1849
void*_tmp2FC=Cyc_Tcutil_compress((void*)_check_null(_tmp305->topt));void*_tmp2FD=_tmp2FC;void*_tmp2FE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->tag == 5U){_LLA: _tmp2FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_typ;_LLB:
# 1851
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2FE,_tmp304,f_tag,tagged_member_type,clear_read,_tmp303);}else{_LLC: _LLD:
# 1853
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1855
 return 0;}_LL0:;}
# 1868 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1872
struct _tuple1*_tmp306=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp306,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp89D=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp89D;});else{
# 1882
struct Cyc_Absyn_Exp*_tmp307=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp8A0=({struct Cyc_Absyn_Exp*_tmp89F=_tmp307;struct Cyc_Absyn_Stmt*_tmp89E=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp89F,_tmp89E,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp8A0;});}{
# 1885
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp8A3=_tmp306;void*_tmp8A2=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp8A1=
Cyc_Toc_push_address_exp(e1);
# 1885
Cyc_Absyn_declare_stmt(_tmp8A3,_tmp8A2,_tmp8A1,
# 1887
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1891
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp308=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp30B=({struct Cyc_String_pa_PrintArg_struct _tmp795;_tmp795.tag=0U,({struct _dyneither_ptr _tmp8A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp795.f1=_tmp8A4;});_tmp795;});void*_tmp309[1U];_tmp309[0]=& _tmp30B;({struct _dyneither_ptr _tmp8A5=({const char*_tmp30A="exp_to_c: no type for %s";_tag_dyneither(_tmp30A,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8A5,_tag_dyneither(_tmp309,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp30C=_tmp308;struct Cyc_Absyn_Stmt*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;struct _dyneither_ptr*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp528;int _tmp527;struct Cyc_Absyn_Exp*_tmp526;void**_tmp525;struct Cyc_Absyn_Exp*_tmp524;int _tmp523;int _tmp522;struct Cyc_List_List*_tmp521;struct Cyc_Absyn_Datatypedecl*_tmp520;struct Cyc_Absyn_Datatypefield*_tmp51F;struct _tuple1**_tmp51E;struct Cyc_List_List*_tmp51D;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_Aggrdecl*_tmp51B;void*_tmp51A;struct Cyc_List_List*_tmp519;struct Cyc_Absyn_Exp*_tmp518;void*_tmp517;int _tmp516;struct Cyc_Absyn_Vardecl*_tmp515;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp513;int _tmp512;struct Cyc_List_List*_tmp511;struct Cyc_List_List*_tmp510;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;struct _dyneither_ptr*_tmp50C;int _tmp50B;int _tmp50A;struct Cyc_Absyn_Exp*_tmp509;struct _dyneither_ptr*_tmp508;int _tmp507;int _tmp506;struct Cyc_Absyn_Exp*_tmp505;void*_tmp504;struct Cyc_List_List*_tmp503;void*_tmp502;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp4FE;void**_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FC;int _tmp4FB;enum Cyc_Absyn_Coercion _tmp4FA;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;int _tmp4F5;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_List_List*_tmp4F3;int _tmp4F2;struct Cyc_List_List*_tmp4F1;struct Cyc_Absyn_VarargInfo*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_List_List*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Core_Opt*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E1;enum Cyc_Absyn_Incrementor _tmp4E0;enum Cyc_Absyn_Primop _tmp4DF;struct Cyc_List_List*_tmp4DE;switch(*((int*)_tmp30C)){case 2U: _LL1: _LL2:
# 1898
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1905
struct Cyc_Absyn_Exp*_tmp30D=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp8A6=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30D,_tmp30D))->r;e->r=_tmp8A6;});else{
# 1910
({void*_tmp8A8=({struct Cyc_Absyn_Exp*_tmp8A7=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8A7,({struct Cyc_Absyn_Exp*_tmp30E[3U];_tmp30E[0]=_tmp30D,_tmp30E[1]=_tmp30D,_tmp30E[2]=_tmp30D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp30E,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp8A8;});}}else{
# 1912
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1914
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 3U: _LL9: _tmp4DF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4DE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LLA: {
# 1919
struct Cyc_List_List*_tmp30F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4DE);
# 1921
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4DE);
{enum Cyc_Absyn_Primop _tmp310=_tmp4DF;switch(_tmp310){case Cyc_Absyn_Numelts: _LL58: _LL59: {
# 1924
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd;
{void*_tmp311=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp312=_tmp311;void*_tmp324;union Cyc_Absyn_Constraint*_tmp323;union Cyc_Absyn_Constraint*_tmp322;union Cyc_Absyn_Constraint*_tmp321;struct Cyc_Absyn_Exp*_tmp320;switch(*((int*)_tmp312)){case 8U: _LL6D: _tmp320=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312)->f1).num_elts;_LL6E:
# 1928
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp320)))
({void*_tmp313=0U;({unsigned int _tmp8AA=e->loc;struct _dyneither_ptr _tmp8A9=({const char*_tmp314="can't calculate numelts";_tag_dyneither(_tmp314,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8AA,_tmp8A9,_tag_dyneither(_tmp313,sizeof(void*),0U));});});
e->r=_tmp320->r;goto _LL6C;case 5U: _LL6F: _tmp324=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).elt_typ;_tmp323=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).nullable;_tmp322=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).bounds;_tmp321=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).zero_term;_LL70:
# 1932
{void*_tmp315=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp322);void*_tmp316=_tmp315;struct Cyc_Absyn_Exp*_tmp31B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp316)->tag == 0U){_LL74: _LL75:
# 1934
({void*_tmp8AD=({struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp8AC,({struct Cyc_Absyn_Exp*_tmp317[2U];_tmp317[0]=(struct Cyc_Absyn_Exp*)_tmp4DE->hd,({
# 1936
struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp324),0U);_tmp317[1]=_tmp8AB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp317,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1934
e->r=_tmp8AD;});
# 1937
goto _LL73;}else{_LL76: _tmp31B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp316)->f1;_LL77:
# 1940
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp321)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4DE->hd);
# 1944
({void*_tmp8AF=({struct Cyc_Absyn_Exp*_tmp8AE=function_e;Cyc_Toc_fncall_exp_r(_tmp8AE,({struct Cyc_Absyn_Exp*_tmp318[2U];_tmp318[0]=(struct Cyc_Absyn_Exp*)_tmp4DE->hd,_tmp318[1]=_tmp31B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp318,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8AF;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp323)){
if(!Cyc_Evexp_c_can_eval(_tmp31B))
({void*_tmp319=0U;({unsigned int _tmp8B1=e->loc;struct _dyneither_ptr _tmp8B0=({const char*_tmp31A="can't calculate numelts";_tag_dyneither(_tmp31A,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8B1,_tmp8B0,_tag_dyneither(_tmp319,sizeof(void*),0U));});});
# 1949
({void*_tmp8B4=({struct Cyc_Absyn_Exp*_tmp8B3=arg;struct Cyc_Absyn_Exp*_tmp8B2=_tmp31B;Cyc_Toc_conditional_exp_r(_tmp8B3,_tmp8B2,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8B4;});}else{
# 1951
e->r=_tmp31B->r;goto _LL73;}}
# 1953
goto _LL73;}_LL73:;}
# 1955
goto _LL6C;default: _LL71: _LL72:
# 1957
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0U,({
struct _dyneither_ptr _tmp8B5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp797.f1=_tmp8B5;});_tmp797;});struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp796;_tmp796.tag=0U,({struct _dyneither_ptr _tmp8B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp796.f1=_tmp8B6;});_tmp796;});void*_tmp31C[2U];_tmp31C[0]=& _tmp31E,_tmp31C[1]=& _tmp31F;({struct _dyneither_ptr _tmp8B7=({const char*_tmp31D="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp31D,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8B7,_tag_dyneither(_tmp31C,sizeof(void*),2U));});});}_LL6C:;}
# 1960
goto _LL57;}case Cyc_Absyn_Plus: _LL5A: _LL5B:
# 1965
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp325=0U;({struct _dyneither_ptr _tmp8B8=({const char*_tmp326="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp326,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8B8,_tag_dyneither(_tmp325,sizeof(void*),0U));});});
{void*_tmp327=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd);void*_tmp328=_tmp327;void*_tmp330;union Cyc_Absyn_Constraint*_tmp32F;union Cyc_Absyn_Constraint*_tmp32E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->tag == 5U){_LL79: _tmp330=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).elt_typ;_tmp32F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).ptr_atts).bounds;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1).ptr_atts).zero_term;_LL7A:
# 1969
{void*_tmp329=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32F);void*_tmp32A=_tmp329;struct Cyc_Absyn_Exp*_tmp32D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp32A)->tag == 0U){_LL7E: _LL7F: {
# 1971
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;
({void*_tmp8BB=({struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8BA,({struct Cyc_Absyn_Exp*_tmp32B[3U];_tmp32B[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8B9=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp330),0U);_tmp32B[1]=_tmp8B9;}),_tmp32B[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1973
e->r=_tmp8BB;});
# 1975
goto _LL7D;}}else{_LL80: _tmp32D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp32A)->f1;_LL81:
# 1977
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp32E)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;
({void*_tmp8BD=({struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp8BC,({struct Cyc_Absyn_Exp*_tmp32C[3U];_tmp32C[0]=e1,_tmp32C[1]=_tmp32D,_tmp32C[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp8BD;});}
# 1982
goto _LL7D;}_LL7D:;}
# 1984
goto _LL78;}else{_LL7B: _LL7C:
 goto _LL78;}_LL78:;}
# 1987
goto _LL57;case Cyc_Absyn_Minus: _LL5C: _LL5D: {
# 1992
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp30F->tl))->hd)){
({void*_tmp8BF=({struct Cyc_Absyn_Exp*_tmp8BE=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8BE,Cyc_Toc_curr_sp);});e1->r=_tmp8BF;});
({void*_tmp8C1=({struct Cyc_Absyn_Exp*_tmp8C0=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8C0,Cyc_Toc_curr_sp);});e2->r=_tmp8C1;});
({void*_tmp8C3=({void*_tmp8C2=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmp8C2;});e1->topt=_tmp8C3;});
({void*_tmp8C5=({struct Cyc_Absyn_Exp*_tmp8C4=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8C4,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 2000
e->r=_tmp8C5;});}else{
# 2003
({void*_tmp8C9=({struct Cyc_Absyn_Exp*_tmp8C8=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8C8,({struct Cyc_Absyn_Exp*_tmp331[3U];_tmp331[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8C7=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp331[1]=_tmp8C7;}),({
struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp331[2]=_tmp8C6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp331,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2003
e->r=_tmp8C9;});}}
# 2008
goto _LL57;}case Cyc_Absyn_Eq: _LL5E: _LL5F:
 goto _LL61;case Cyc_Absyn_Neq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Gt: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gte: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lte: _LL68: _LL69: {
# 2016
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DE->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp30F))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp30F->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp332=({void*_tmp8CA=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8CA,Cyc_Toc_mt_tq);});
({void*_tmp8CD=({void*_tmp8CC=_tmp332;Cyc_Toc_cast_it_r(_tmp8CC,({struct Cyc_Absyn_Exp*_tmp8CB=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8CB,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8CD;});
e1->topt=_tmp332;}
# 2026
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp333=({void*_tmp8CE=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8CE,Cyc_Toc_mt_tq);});
({void*_tmp8D1=({void*_tmp8D0=_tmp333;Cyc_Toc_cast_it_r(_tmp8D0,({struct Cyc_Absyn_Exp*_tmp8CF=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8CF,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8D1;});
e2->topt=_tmp333;}
# 2031
goto _LL57;}default: _LL6A: _LL6B:
 goto _LL57;}_LL57:;}
# 2034
goto _LL0;}case 5U: _LLB: _tmp4E1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E0=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LLC: {
# 2036
void*e2_cyc_typ=(void*)_check_null(_tmp4E1->topt);
# 2045 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp34D="increment";_tag_dyneither(_tmp34D,sizeof(char),10U);});
if(_tmp4E0 == Cyc_Absyn_PreDec  || _tmp4E0 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8D2=({const char*_tmp334="decrement";_tag_dyneither(_tmp334,sizeof(char),10U);});incr_str=_tmp8D2;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E1,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp798;_tmp798.tag=0U,_tmp798.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp798;});void*_tmp335[1U];_tmp335[0]=& _tmp337;({unsigned int _tmp8D4=e->loc;struct _dyneither_ptr _tmp8D3=({const char*_tmp336="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp336,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp8D4,_tmp8D3,_tag_dyneither(_tmp335,sizeof(void*),1U));});});
({void*_tmp338=0U;({struct _dyneither_ptr _tmp8D5=({const char*_tmp339="in-place inc/dec on zero-term";_tag_dyneither(_tmp339,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D5,_tag_dyneither(_tmp338,sizeof(void*),0U));});});}{
# 2054
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4E1,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp33A=Cyc_Absyn_signed_int_exp(1,0U);
_tmp33A->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp33B=_tmp4E0;switch(_tmp33B){case Cyc_Absyn_PreInc: _LL83: _LL84:
# 2061
({void*_tmp8D7=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->tag=4U,_tmp33D->f1=_tmp4E1,({struct Cyc_Core_Opt*_tmp8D6=({struct Cyc_Core_Opt*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->v=(void*)Cyc_Absyn_Plus;_tmp33C;});_tmp33D->f2=_tmp8D6;}),_tmp33D->f3=_tmp33A;_tmp33D;});e->r=_tmp8D7;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL85: _LL86:
# 2065
({void*_tmp8D9=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33F=_cycalloc(sizeof(*_tmp33F));_tmp33F->tag=4U,_tmp33F->f1=_tmp4E1,({struct Cyc_Core_Opt*_tmp8D8=({struct Cyc_Core_Opt*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->v=(void*)Cyc_Absyn_Minus;_tmp33E;});_tmp33F->f2=_tmp8D8;}),_tmp33F->f3=_tmp33A;_tmp33F;});e->r=_tmp8D9;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL87: _LL88:
# 2069
({struct Cyc_String_pa_PrintArg_struct _tmp342=({struct Cyc_String_pa_PrintArg_struct _tmp799;_tmp799.tag=0U,_tmp799.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp799;});void*_tmp340[1U];_tmp340[0]=& _tmp342;({unsigned int _tmp8DB=e->loc;struct _dyneither_ptr _tmp8DA=({const char*_tmp341="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp341,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp8DB,_tmp8DA,_tag_dyneither(_tmp340,sizeof(void*),1U));});});
# 2071
({void*_tmp343=0U;({struct _dyneither_ptr _tmp8DC=({const char*_tmp344="in-place inc/dec on @tagged union";_tag_dyneither(_tmp344,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8DC,_tag_dyneither(_tmp343,sizeof(void*),0U));});});}_LL82:;};}{
# 2074
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp4E1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E1);
Cyc_Toc_set_lhs(nv,0);{
# 2081
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4E0 == Cyc_Absyn_PostInc  || _tmp4E0 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2087
if(_tmp4E0 == Cyc_Absyn_PreDec  || _tmp4E0 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8E1=({struct Cyc_Absyn_Exp*_tmp8E0=fn_e;Cyc_Toc_fncall_exp_r(_tmp8E0,({struct Cyc_Absyn_Exp*_tmp345[3U];({struct Cyc_Absyn_Exp*_tmp8DF=Cyc_Toc_push_address_exp(_tmp4E1);_tmp345[0]=_tmp8DF;}),({
struct Cyc_Absyn_Exp*_tmp8DE=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp345[1]=_tmp8DE;}),({
struct Cyc_Absyn_Exp*_tmp8DD=Cyc_Absyn_signed_int_exp(change,0U);_tmp345[2]=_tmp8DD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp345,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2089
e->r=_tmp8E1;});}else{
# 2092
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2098
if(_tmp4E0 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp346=Cyc_Toc_temp_var();
void*_tmp347=({void*_tmp8E2=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_typ(_tmp8E2,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp348=Cyc_Toc_push_address_exp(_tmp4E1);
struct Cyc_Absyn_Exp*_tmp349=({struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8E3,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmp8E6=({struct Cyc_Absyn_Exp*_tmp8E5=({struct Cyc_Absyn_Exp*_tmp8E4=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U);Cyc_Absyn_neq_exp(_tmp8E4,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2105
Cyc_Absyn_and_exp(_tmp8E5,_tmp349,0U);});_tmp349=_tmp8E6;});{
# 2107
struct Cyc_Absyn_Stmt*_tmp34A=({struct Cyc_Absyn_Exp*_tmp8E8=_tmp349;struct Cyc_Absyn_Stmt*_tmp8E7=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U),0U),0U);
# 2107
Cyc_Absyn_ifthenelse_stmt(_tmp8E8,_tmp8E7,
# 2109
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8EA=({struct Cyc_Absyn_Stmt*_tmp8E9=_tmp34A;Cyc_Absyn_seq_stmt(_tmp8E9,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp346,0U),0U),0U),0U);});_tmp34A=_tmp8EA;});
({void*_tmp8EB=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp346,_tmp347,_tmp348,_tmp34A,0U),0U))->r;e->r=_tmp8EB;});};}else{
# 2114
struct Cyc_Toc_functionSet*_tmp34B=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp34B,_tmp4E1);
({void*_tmp8EF=({struct Cyc_Absyn_Exp*_tmp8EE=fn_e;Cyc_Toc_fncall_exp_r(_tmp8EE,({struct Cyc_Absyn_Exp*_tmp34C[2U];({struct Cyc_Absyn_Exp*_tmp8ED=Cyc_Toc_push_address_exp(_tmp4E1);_tmp34C[0]=_tmp8ED;}),({
struct Cyc_Absyn_Exp*_tmp8EC=Cyc_Absyn_signed_int_exp(1,0U);_tmp34C[1]=_tmp8EC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34C,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2116
e->r=_tmp8EF;});}}else{
# 2119
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4E1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4E1,0,Cyc_Toc_incr_lvalue,_tmp4E0);
e->r=_tmp4E1->r;}}}
# 2123
goto _LL0;};};};}case 4U: _LLD: _tmp4E4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4E2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LLE: {
# 2142 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4E4->topt);
void*e2_old_typ=(void*)_check_null(_tmp4E2->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4E4,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E2);
({void*_tmp8F0=Cyc_Toc_tagged_union_assignop(_tmp4E4,e1_old_typ,_tmp4E3,_tmp4E2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8F0;});
# 2153
return;}{
# 2155
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E4,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4E4,_tmp4E3,_tmp4E2,ptr_type,is_dyneither,elt_type);
# 2161
return;}{
# 2165
int e1_poly=Cyc_Toc_is_poly_project(_tmp4E4);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E2);{
# 2171
int done=0;
if(_tmp4E3 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp34E=(enum Cyc_Absyn_Primop)_tmp4E3->v;enum Cyc_Absyn_Primop _tmp34F=_tmp34E;switch(_tmp34F){case Cyc_Absyn_Plus: _LL8A: _LL8B:
 change=_tmp4E2;goto _LL89;case Cyc_Absyn_Minus: _LL8C: _LL8D:
# 2179
({struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4E2,0U);change=_tmp8F1;});goto _LL89;default: _LL8E: _LL8F:
({void*_tmp350=0U;({struct _dyneither_ptr _tmp8F2=({const char*_tmp351="bad t ? pointer arithmetic";_tag_dyneither(_tmp351,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F2,_tag_dyneither(_tmp350,sizeof(void*),0U));});});}_LL89:;}
# 2182
done=1;{
# 2184
struct Cyc_Absyn_Exp*_tmp352=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8F6=({struct Cyc_Absyn_Exp*_tmp8F5=_tmp352;Cyc_Toc_fncall_exp_r(_tmp8F5,({struct Cyc_Absyn_Exp*_tmp353[3U];({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Toc_push_address_exp(_tmp4E4);_tmp353[0]=_tmp8F4;}),({
struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp353[1]=_tmp8F3;}),_tmp353[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp353,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2185
e->r=_tmp8F6;});};}else{
# 2188
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2191
enum Cyc_Absyn_Primop _tmp354=(enum Cyc_Absyn_Primop)_tmp4E3->v;enum Cyc_Absyn_Primop _tmp355=_tmp354;if(_tmp355 == Cyc_Absyn_Plus){_LL91: _LL92:
# 2193
 done=1;
({void*_tmp8F8=({struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4E4);Cyc_Toc_fncall_exp_r(_tmp8F7,({struct Cyc_Absyn_Exp*_tmp356[2U];_tmp356[0]=_tmp4E4,_tmp356[1]=_tmp4E2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp356,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8F8;});
goto _LL90;}else{_LL93: _LL94:
({void*_tmp357=0U;({struct _dyneither_ptr _tmp8F9=({const char*_tmp358="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp358,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F9,_tag_dyneither(_tmp357,sizeof(void*),0U));});});}_LL90:;}}}
# 2200
if(!done){
# 2202
if(e1_poly)
({void*_tmp8FB=({void*_tmp8FA=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmp8FA,Cyc_Absyn_new_exp(_tmp4E2->r,0U));});_tmp4E2->r=_tmp8FB;});
# 2208
if(!Cyc_Absyn_is_lvalue(_tmp4E4)){
({struct Cyc_Absyn_Exp*_tmp8FC=_tmp4E4;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8FC,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp35A=1;struct _tuple31*_tmp359=_cycalloc(_check_times(_tmp35A,sizeof(struct _tuple31)));(_tmp359[0]).f1=_tmp4E3,(_tmp359[0]).f2=_tmp4E2;_tmp359;}));});
e->r=_tmp4E4->r;}}
# 2213
goto _LL0;};};};}case 6U: _LLF: _tmp4E7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4E5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL10:
# 2215
 Cyc_Toc_exp_to_c(nv,_tmp4E7);
Cyc_Toc_exp_to_c(nv,_tmp4E6);
Cyc_Toc_exp_to_c(nv,_tmp4E5);
goto _LL0;case 7U: _LL11: _tmp4E9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4E8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL12:
 _tmp4EB=_tmp4E9;_tmp4EA=_tmp4E8;goto _LL14;case 8U: _LL13: _tmp4EB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4EA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL14:
 _tmp4ED=_tmp4EB;_tmp4EC=_tmp4EA;goto _LL16;case 9U: _LL15: _tmp4ED=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4EC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL16:
# 2222
 Cyc_Toc_exp_to_c(nv,_tmp4ED);
Cyc_Toc_exp_to_c(nv,_tmp4EC);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3 == 0){_LL17: _tmp4EF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4EE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL18:
# 2226
 Cyc_Toc_exp_to_c(nv,_tmp4EF);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4EE);
goto _LL0;}else{_LL19: _tmp4F4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4F2=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->num_varargs;_tmp4F1=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->injectors;_tmp4F0=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30C)->f3)->vai;_LL1A: {
# 2238 "toc.cyc"
struct _RegionHandle _tmp35B=_new_region("r");struct _RegionHandle*r=& _tmp35B;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4F2,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4F0->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2246
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F3);
int num_normargs=num_args - _tmp4F2;
# 2250
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4F3=_tmp4F3->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F3))->hd);
({struct Cyc_List_List*_tmp8FD=({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C->hd=(struct Cyc_Absyn_Exp*)_tmp4F3->hd,_tmp35C->tl=new_args;_tmp35C;});new_args=_tmp8FD;});}}
# 2255
({struct Cyc_List_List*_tmp901=({struct Cyc_List_List*_tmp35E=_cycalloc(sizeof(*_tmp35E));({struct Cyc_Absyn_Exp*_tmp900=({struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8FF,({struct Cyc_Absyn_Exp*_tmp35D[3U];_tmp35D[0]=argvexp,({
# 2257
struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp35D[1]=_tmp8FE;}),_tmp35D[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35D,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2255
_tmp35E->hd=_tmp900;}),_tmp35E->tl=new_args;_tmp35E;});new_args=_tmp901;});
# 2260
({struct Cyc_List_List*_tmp902=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp902;});
# 2262
Cyc_Toc_exp_to_c(nv,_tmp4F4);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4F4,new_args,0U),0U);
# 2266
if(_tmp4F0->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp35F=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4F0->type));void*_tmp360=_tmp35F;struct Cyc_Absyn_Datatypedecl*_tmp363;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp360)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp360)->f1).datatype_info).KnownDatatype).tag == 2){_LL96: _tmp363=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp360)->f1).datatype_info).KnownDatatype).val;_LL97:
 tud=_tmp363;goto _LL95;}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp361=0U;({struct _dyneither_ptr _tmp903=({const char*_tmp362="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp362,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp903,_tag_dyneither(_tmp361,sizeof(void*),0U));});});}_LL95:;}{
# 2272
struct _dyneither_ptr vs=({unsigned int _tmp374=(unsigned int)_tmp4F2;struct _tuple1**_tmp373=({struct _RegionHandle*_tmp904=r;_region_malloc(_tmp904,_check_times(_tmp374,sizeof(struct _tuple1*)));});({{unsigned int _tmp79A=(unsigned int)_tmp4F2;unsigned int i;for(i=0;i < _tmp79A;++ i){({struct _tuple1*_tmp905=Cyc_Toc_temp_var();_tmp373[i]=_tmp905;});}}0;});_tag_dyneither(_tmp373,sizeof(struct _tuple1*),_tmp374);});
# 2274
if(_tmp4F2 != 0){
# 2278
struct Cyc_List_List*_tmp364=0;
{int i=_tmp4F2 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp907=({struct Cyc_List_List*_tmp365=_cycalloc(sizeof(*_tmp365));({struct Cyc_Absyn_Exp*_tmp906=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp365->hd=_tmp906;}),_tmp365->tl=_tmp364;_tmp365;});_tmp364=_tmp907;});}}
({struct Cyc_Absyn_Stmt*_tmp90B=({struct _tuple1*_tmp90A=argv;void*_tmp909=arr_type;struct Cyc_Absyn_Exp*_tmp908=Cyc_Absyn_array_exp(_tmp364,0U);Cyc_Absyn_declare_stmt(_tmp90A,_tmp909,_tmp908,s,0U);});s=_tmp90B;});
# 2283
({struct Cyc_List_List*_tmp90C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F3);_tmp4F3=_tmp90C;});
({struct Cyc_List_List*_tmp90D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4F1);_tmp4F1=_tmp90D;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F3)- 1;
for(0;_tmp4F3 != 0;(_tmp4F3=_tmp4F3->tl,_tmp4F1=_tmp4F1->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F3->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp366=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4F1))->hd;struct Cyc_Absyn_Datatypefield*_tmp367=_tmp366;struct _tuple1*_tmp36D;struct Cyc_List_List*_tmp36C;_LL9B: _tmp36D=_tmp367->name;_tmp36C=_tmp367->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp36C))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp90E=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp90E;});{
# 2297
struct _tuple1*_tmp368=Cyc_Toc_collapse_qvars(_tmp36D,tud->name);
struct Cyc_List_List*_tmp369=({struct _tuple20*_tmp36B[2U];({struct _tuple20*_tmp912=({struct _dyneither_ptr*_tmp911=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp911,Cyc_Toc_datatype_tag(tud,_tmp36D));});_tmp36B[0]=_tmp912;}),({
struct _tuple20*_tmp910=({struct _dyneither_ptr*_tmp90F=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp90F,arg);});_tmp36B[1]=_tmp910;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36B,sizeof(struct _tuple20*),2U));});
({struct Cyc_Absyn_Stmt*_tmp916=({struct _tuple1*_tmp915=var;void*_tmp914=Cyc_Absyn_strctq(_tmp368);struct Cyc_Absyn_Exp*_tmp913=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->tag=29U,_tmp36A->f1=_tmp368,_tmp36A->f2=0,_tmp36A->f3=_tmp369,_tmp36A->f4=0;_tmp36A;}),0U);
# 2300
Cyc_Absyn_declare_stmt(_tmp915,_tmp914,_tmp913,s,0U);});s=_tmp916;});};};}};}else{
# 2308
struct Cyc_List_List*_tmp36E=({struct _tuple32*_tmp36F[3U];({struct _tuple32*_tmp91F=({struct _tuple32*_tmp370=_cycalloc(sizeof(*_tmp370));({struct _dyneither_ptr _tmp91E=_tag_dyneither(0,0,0);_tmp370->f1=_tmp91E;}),({struct Cyc_Absyn_Exp*_tmp91D=Cyc_Absyn_uint_exp(0U,0U);_tmp370->f2=_tmp91D;});_tmp370;});_tmp36F[0]=_tmp91F;}),({
struct _tuple32*_tmp91C=({struct _tuple32*_tmp371=_cycalloc(sizeof(*_tmp371));({struct _dyneither_ptr _tmp91B=_tag_dyneither(0,0,0);_tmp371->f1=_tmp91B;}),({struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_uint_exp(0U,0U);_tmp371->f2=_tmp91A;});_tmp371;});_tmp36F[1]=_tmp91C;}),({
struct _tuple32*_tmp919=({struct _tuple32*_tmp372=_cycalloc(sizeof(*_tmp372));({struct _dyneither_ptr _tmp918=_tag_dyneither(0,0,0);_tmp372->f1=_tmp918;}),({struct Cyc_Absyn_Exp*_tmp917=Cyc_Absyn_uint_exp(0U,0U);_tmp372->f2=_tmp917;});_tmp372;});_tmp36F[2]=_tmp919;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36F,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp923=({struct _tuple1*_tmp922=argv;void*_tmp921=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp920=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp922,_tmp921,_tmp920,s,0U);});s=_tmp923;});}};}else{
# 2316
if(_tmp4F2 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4F3 != 0;_tmp4F3=_tmp4F3->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F3->hd);
({struct Cyc_List_List*_tmp924=({struct Cyc_List_List*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->hd=(struct Cyc_Absyn_Exp*)_tmp4F3->hd,_tmp375->tl=array_args;_tmp375;});array_args=_tmp924;});}{
# 2322
struct Cyc_Absyn_Exp*_tmp376=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp925=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp376,s,0U);s=_tmp925;});};}else{
# 2325
({struct Cyc_Absyn_Stmt*_tmp929=({struct _tuple1*_tmp928=argv;void*_tmp927=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp926=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp928,_tmp927,_tmp926,s,0U);});s=_tmp929;});}}
# 2328
({void*_tmp92A=Cyc_Toc_stmt_exp_r(s);e->r=_tmp92A;});};}
# 2330
_npop_handler(0U);goto _LL0;
# 2238
;_pop_region(r);}}case 11U: _LL1B: _tmp4F6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL1C:
# 2333
 Cyc_Toc_exp_to_c(nv,_tmp4F6);{
struct Cyc_Absyn_Exp*_tmp377=_tmp4F5?Cyc_Toc_newrethrow_exp(_tmp4F6): Cyc_Toc_newthrow_exp(_tmp4F6);
({void*_tmp92C=({void*_tmp92B=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp92B,_tmp377,0U);})->r;e->r=_tmp92C;});
goto _LL0;};case 12U: _LL1D: _tmp4F7=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL1E:
 Cyc_Toc_exp_to_c(nv,_tmp4F7);goto _LL0;case 13U: _LL1F: _tmp4F9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4F8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL20:
# 2339
 Cyc_Toc_exp_to_c(nv,_tmp4F9);
# 2348 "toc.cyc"
for(0;_tmp4F8 != 0;_tmp4F8=_tmp4F8->tl){
enum Cyc_Absyn_KindQual _tmp378=(Cyc_Tcutil_typ_kind((void*)_tmp4F8->hd))->kind;
if(_tmp378 != Cyc_Absyn_EffKind  && _tmp378 != Cyc_Absyn_RgnKind){
{void*_tmp379=Cyc_Tcutil_compress((void*)_tmp4F8->hd);void*_tmp37A=_tmp379;switch(*((int*)_tmp37A)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 3U: _LLA0: _LLA1:
 continue;default: _LLA2: _LLA3:
# 2355
({void*_tmp92E=({void*_tmp92D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp92D,_tmp4F9,0U);})->r;e->r=_tmp92E;});
goto _LL9D;}_LL9D:;}
# 2358
break;}}
# 2361
goto _LL0;case 14U: _LL21: _tmp4FD=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4FC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp4FB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp4FA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL22: {
# 2363
void*old_t2=(void*)_check_null(_tmp4FC->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4FD;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4FD=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4FC);
# 2370
{struct _tuple33 _tmp37B=({struct _tuple33 _tmp79E;({void*_tmp930=Cyc_Tcutil_compress(old_t2);_tmp79E.f1=_tmp930;}),({void*_tmp92F=Cyc_Tcutil_compress(new_typ);_tmp79E.f2=_tmp92F;});_tmp79E;});struct _tuple33 _tmp37C=_tmp37B;struct Cyc_Absyn_PtrInfo _tmp3BA;struct Cyc_Absyn_PtrInfo _tmp3B9;struct Cyc_Absyn_PtrInfo _tmp3B8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f1)->tag == 5U)switch(*((int*)_tmp37C.f2)){case 5U: _LLA5: _tmp3B9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f1)->f1;_tmp3B8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f2)->f1;_LLA6: {
# 2372
int _tmp37D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B9.ptr_atts).nullable);
int _tmp37E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B8.ptr_atts).nullable);
void*_tmp37F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B9.ptr_atts).bounds);
void*_tmp380=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B8.ptr_atts).bounds);
int _tmp381=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B9.ptr_atts).zero_term);
int _tmp382=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B8.ptr_atts).zero_term);
{struct _tuple33 _tmp383=({struct _tuple33 _tmp79D;_tmp79D.f1=_tmp37F,_tmp79D.f2=_tmp380;_tmp79D;});struct _tuple33 _tmp384=_tmp383;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B2;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->tag == 1U){_LLAC: _tmp3B3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f1)->f1;_tmp3B2=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->f1;_LLAD:
# 2380
 if((!Cyc_Evexp_c_can_eval(_tmp3B3) || !Cyc_Evexp_c_can_eval(_tmp3B2)) && !
Cyc_Evexp_same_const_exp(_tmp3B3,_tmp3B2))
({void*_tmp385=0U;({unsigned int _tmp932=e->loc;struct _dyneither_ptr _tmp931=({const char*_tmp386="can't validate cast due to potential size differences";_tag_dyneither(_tmp386,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp932,_tmp931,_tag_dyneither(_tmp385,sizeof(void*),0U));});});
if(_tmp37D  && !_tmp37E){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp387=0U;({struct _dyneither_ptr _tmp933=({const char*_tmp388="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp388,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp933,_tag_dyneither(_tmp387,sizeof(void*),0U));});});
# 2389
if(_tmp4FA != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp38B=({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0U,({struct _dyneither_ptr _tmp934=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp79B.f1=_tmp934;});_tmp79B;});void*_tmp389[1U];_tmp389[0]=& _tmp38B;({struct _dyneither_ptr _tmp935=({const char*_tmp38A="null-check conversion mis-classified: %s";_tag_dyneither(_tmp38A,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp935,_tag_dyneither(_tmp389,sizeof(void*),1U));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp4FC);
if(do_null_check){
if(!_tmp4FB)
({void*_tmp38C=0U;({unsigned int _tmp937=e->loc;struct _dyneither_ptr _tmp936=({const char*_tmp38D="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp38D,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp937,_tmp936,_tag_dyneither(_tmp38C,sizeof(void*),0U));});});
# 2396
({void*_tmp93A=({void*_tmp939=*_tmp4FD;Cyc_Toc_cast_it_r(_tmp939,({struct Cyc_Absyn_Exp*_tmp938=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp938,({struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));
_tmp38E->hd=_tmp4FC,_tmp38E->tl=0;_tmp38E;}),0U);}));});
# 2396
e->r=_tmp93A;});}else{
# 2400
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FC->r;}};}else{
# 2405
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FC->r;}
# 2412
goto _LLAB;}else{_LLAE: _tmp3B4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f1)->f1;_LLAF:
# 2414
 if(!Cyc_Evexp_c_can_eval(_tmp3B4))
({void*_tmp38F=0U;({unsigned int _tmp93C=e->loc;struct _dyneither_ptr _tmp93B=({const char*_tmp390="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp390,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp93C,_tmp93B,_tag_dyneither(_tmp38F,sizeof(void*),0U));});});{
struct _tuple13 _tmp391=Cyc_Evexp_eval_const_uint_exp(_tmp3B4);struct _tuple13 _tmp392=_tmp391;unsigned int _tmp3A4;int _tmp3A3;_LLB5: _tmp3A4=_tmp392.f1;_tmp3A3=_tmp392.f2;_LLB6:;
# 2418
if(_tmp4FA == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp79C;_tmp79C.tag=0U,({struct _dyneither_ptr _tmp93D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp79C.f1=_tmp93D;});_tmp79C;});void*_tmp393[1U];_tmp393[0]=& _tmp395;({struct _dyneither_ptr _tmp93E=({const char*_tmp394="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp394,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp93E,_tag_dyneither(_tmp393,sizeof(void*),1U));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2422
if((_tmp381  && !(_tmp3B8.elt_tq).real_const) && !_tmp382)
# 2425
({struct Cyc_Absyn_Exp*_tmp940=({struct Cyc_Absyn_Exp*_tmp93F=_tmp3B4;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp93F,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3B4=_tmp940;});
# 2427
({void*_tmp941=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3B4,_tmp4FC))->r;e->r=_tmp941;});}else{
# 2429
struct Cyc_Absyn_Exp*_tmp396=Cyc_Toc__tag_dyneither_e;
# 2431
if(_tmp381){
# 2436
struct _tuple1*_tmp397=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp398=Cyc_Absyn_var_exp(_tmp397,0U);
struct Cyc_Absyn_Exp*arg3;
# 2441
{void*_tmp399=_tmp4FC->r;void*_tmp39A=_tmp399;struct Cyc_Absyn_Vardecl*_tmp39D;struct Cyc_Absyn_Vardecl*_tmp39C;switch(*((int*)_tmp39A)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp39A)->f1).Wstring_c).tag){case 8U: _LLB8: _LLB9:
# 2443
 arg3=_tmp3B4;goto _LLB7;case 9U: _LLBA: _LLBB:
# 2445
 arg3=_tmp3B4;goto _LLB7;default: goto _LLC0;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39A)->f1)){case 1U: _LLBC: _tmp39C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39A)->f1)->f1;_LLBD:
 _tmp39D=_tmp39C;goto _LLBF;case 4U: _LLBE: _tmp39D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp39A)->f1)->f1;_LLBF:
# 2448
 if(!Cyc_Tcutil_is_array(_tmp39D->type))goto _LLC1;
arg3=_tmp3B4;
goto _LLB7;default: goto _LLC0;}default: _LLC0: _LLC1:
# 2454
 if(_tmp3A3  && _tmp3A4 != 1)
arg3=_tmp3B4;else{
# 2457
({struct Cyc_Absyn_Exp*_tmp945=({struct Cyc_Absyn_Exp*_tmp944=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4FC);Cyc_Absyn_fncall_exp(_tmp944,({struct Cyc_Absyn_Exp*_tmp39B[2U];({
# 2459
struct Cyc_Absyn_Exp*_tmp943=({void*_tmp942=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp942,_tmp398);});_tmp39B[0]=_tmp943;}),_tmp39B[1]=_tmp3B4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39B,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2457
arg3=_tmp945;});}
# 2460
goto _LLB7;}_LLB7:;}
# 2462
if(!_tmp382  && !(_tmp3B8.elt_tq).real_const)
# 2465
({struct Cyc_Absyn_Exp*_tmp947=({struct Cyc_Absyn_Exp*_tmp946=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp946,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp947;});{
# 2467
struct Cyc_Absyn_Exp*_tmp39E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B8.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp39F=({struct Cyc_Absyn_Exp*_tmp948=_tmp396;Cyc_Absyn_fncall_exp(_tmp948,({struct Cyc_Absyn_Exp*_tmp3A1[3U];_tmp3A1[0]=_tmp398,_tmp3A1[1]=_tmp39E,_tmp3A1[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A1,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp3A0=Cyc_Absyn_exp_stmt(_tmp39F,0U);
({struct Cyc_Absyn_Stmt*_tmp94C=({struct _tuple1*_tmp94B=_tmp397;void*_tmp94A=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp949=_tmp4FC;Cyc_Absyn_declare_stmt(_tmp94B,_tmp94A,_tmp949,_tmp3A0,0U);});_tmp3A0=_tmp94C;});
({void*_tmp94D=Cyc_Toc_stmt_exp_r(_tmp3A0);e->r=_tmp94D;});};}else{
# 2474
({void*_tmp950=({struct Cyc_Absyn_Exp*_tmp94F=_tmp396;Cyc_Toc_fncall_exp_r(_tmp94F,({struct Cyc_Absyn_Exp*_tmp3A2[3U];_tmp3A2[0]=_tmp4FC,({
# 2476
struct Cyc_Absyn_Exp*_tmp94E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B8.elt_typ),0U);_tmp3A2[1]=_tmp94E;}),_tmp3A2[2]=_tmp3B4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A2,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2474
e->r=_tmp950;});}}
# 2480
goto _LLAB;};}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->tag == 1U){_LLB0: _tmp3B5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp384.f2)->f1;_LLB1:
# 2482
 if(!Cyc_Evexp_c_can_eval(_tmp3B5))
({void*_tmp3A5=0U;({unsigned int _tmp952=e->loc;struct _dyneither_ptr _tmp951=({const char*_tmp3A6="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp3A6,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp952,_tmp951,_tag_dyneither(_tmp3A5,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3A7=0U;({struct _dyneither_ptr _tmp953=({const char*_tmp3A8="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp3A8,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp953,_tag_dyneither(_tmp3A7,sizeof(void*),0U));});});{
# 2494 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3A9=_tmp3B5;
if(_tmp381  && !_tmp382)
({struct Cyc_Absyn_Exp*_tmp955=({struct Cyc_Absyn_Exp*_tmp954=_tmp3B5;Cyc_Absyn_add_exp(_tmp954,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3A9=_tmp955;});{
# 2501
struct Cyc_Absyn_Exp*_tmp3AA=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp3AB=({struct Cyc_Absyn_Exp*_tmp957=_tmp3AA;Cyc_Absyn_fncall_exp(_tmp957,({struct Cyc_Absyn_Exp*_tmp3AD[3U];_tmp3AD[0]=_tmp4FC,({
# 2504
struct Cyc_Absyn_Exp*_tmp956=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B9.elt_typ),0U);_tmp3AD[1]=_tmp956;}),_tmp3AD[2]=_tmp3A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3AD,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2506
if(!_tmp37E)
({void*_tmp95A=({struct Cyc_Absyn_Exp*_tmp959=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp959,({struct Cyc_List_List*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));
({struct Cyc_Absyn_Exp*_tmp958=Cyc_Absyn_copy_exp(_tmp3AB);_tmp3AC->hd=_tmp958;}),_tmp3AC->tl=0;_tmp3AC;}));});
# 2507
_tmp3AB->r=_tmp95A;});
# 2509
({void*_tmp95B=Cyc_Toc_cast_it_r(*_tmp4FD,_tmp3AB);e->r=_tmp95B;});
goto _LLAB;};};}else{_LLB2: _LLB3:
# 2514
 DynCast:
 if((_tmp381  && !_tmp382) && !(_tmp3B8.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3AE=0U;({struct _dyneither_ptr _tmp95C=({const char*_tmp3AF="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp3AF,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp95C,_tag_dyneither(_tmp3AE,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp3B0=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmp960=({struct Cyc_Absyn_Exp*_tmp95F=_tmp3B0;Cyc_Toc_fncall_exp_r(_tmp95F,({struct Cyc_Absyn_Exp*_tmp3B1[3U];_tmp3B1[0]=_tmp4FC,({
struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B9.elt_typ),0U);_tmp3B1[1]=_tmp95E;}),({
struct Cyc_Absyn_Exp*_tmp95D=Cyc_Absyn_uint_exp(1U,0U);_tmp3B1[2]=_tmp95D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B1,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2519
e->r=_tmp960;});};}
# 2523
goto _LLAB;}}_LLAB:;}
# 2525
goto _LLA4;}case 6U: _LLA7: _tmp3BA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C.f1)->f1;_LLA8:
# 2527
{void*_tmp3B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3BA.ptr_atts).bounds);void*_tmp3B7=_tmp3B6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B7)->tag == 0U){_LLC3: _LLC4:
# 2529
({void*_tmp962=({struct Cyc_Absyn_Exp*_tmp961=Cyc_Absyn_new_exp(_tmp4FC->r,_tmp4FC->loc);Cyc_Toc_aggrmember_exp_r(_tmp961,Cyc_Toc_curr_sp);});_tmp4FC->r=_tmp962;});
_tmp4FC->topt=new_typ_c;
goto _LLC2;}else{_LLC5: _LLC6:
 goto _LLC2;}_LLC2:;}
# 2534
goto _LLA4;default: goto _LLA9;}else{_LLA9: _LLAA:
# 2536
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4FC->r;
goto _LLA4;}_LLA4:;}
# 2540
goto _LL0;}case 15U: _LL23: _tmp4FE=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL24:
# 2543
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4FE);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4FE)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4FE,0,Cyc_Toc_address_lvalue,1);
# 2549
({void*_tmp964=({void*_tmp963=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp963,_tmp4FE);});e->r=_tmp964;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp4FE->topt))))
# 2553
({void*_tmp966=({void*_tmp965=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp965,_tmp4FE);});e->r=_tmp966;});}
# 2555
goto _LL0;case 16U: _LL25: _tmp500=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp4FF=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL26:
# 2562
 Cyc_Toc_exp_to_c(nv,_tmp4FF);{
# 2564
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4FF->topt));
struct _tuple1*_tmp3BB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3BC=Cyc_Absyn_var_exp(_tmp3BB,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3BD=Cyc_Tcutil_compress(elt_typ);void*_tmp3BE=_tmp3BD;void*_tmp3BF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->tag == 8U){_LLC8: _tmp3BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->f1).elt_type;_LLC9:
# 2570
({void*_tmp967=Cyc_Toc_typ_to_c(_tmp3BF);elt_typ=_tmp967;});
({struct Cyc_Absyn_Exp*_tmp968=Cyc_Absyn_copy_exp(_tmp3BC);lhs=_tmp968;});
goto _LLC7;}else{_LLCA: _LLCB:
# 2574
({struct Cyc_Absyn_Exp*_tmp969=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp3BC),0U);lhs=_tmp969;});
goto _LLC7;}_LLC7:;}{
# 2577
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp4FF);
struct _tuple1*_tmp3C0=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3C1=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3C0),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp96C=({struct Cyc_Absyn_Exp*_tmp96B=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp96B,({struct Cyc_Absyn_Exp*_tmp3C2[2U];_tmp3C2[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3C1),({
struct Cyc_Absyn_Exp*_tmp96A=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3C2[1]=_tmp96A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C2,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2582
mexp=_tmp96C;});else{
# 2585
({struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp3BC,0U),0U);mexp=_tmp96D;});}{
# 2588
struct Cyc_Absyn_Exp*_tmp3C3=Cyc_Toc_get_varsizeexp(_tmp4FF);
if(_tmp3C3 != 0)
({struct Cyc_Absyn_Exp*_tmp96E=Cyc_Absyn_add_exp(mexp,_tmp3C3,0U);mexp=_tmp96E;});
# 2592
if(_tmp500 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp96F=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp96F;});else{
# 2595
struct Cyc_Absyn_Exp*r=_tmp500;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp970=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp970;});}{
# 2599
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp975=({struct Cyc_Absyn_Exp*_tmp974=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp974,({struct Cyc_Absyn_Exp*_tmp3C4[3U];({
struct Cyc_Absyn_Exp*_tmp973=Cyc_Absyn_copy_exp(_tmp3BC);_tmp3C4[0]=_tmp973;}),({
struct Cyc_Absyn_Exp*_tmp972=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3C4[1]=_tmp972;}),
(unsigned int)_tmp3C1?_tmp3C4[2]=_tmp3C1:({struct Cyc_Absyn_Exp*_tmp971=Cyc_Absyn_uint_exp(1U,0U);_tmp3C4[2]=_tmp971;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3C4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2601
answer=_tmp975;});else{
# 2606
({struct Cyc_Absyn_Exp*_tmp976=Cyc_Absyn_copy_exp(_tmp3BC);answer=_tmp976;});}
({void*_tmp977=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->tag=Cyc_Toc_Dest,_tmp3C5->f1=_tmp3BC;_tmp3C5;});e->annot=_tmp977;});{
struct Cyc_Absyn_Stmt*_tmp3C6=({struct _tuple1*_tmp97B=_tmp3BB;void*_tmp97A=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp979=mexp;Cyc_Absyn_declare_stmt(_tmp97B,_tmp97A,_tmp979,({
struct Cyc_Absyn_Stmt*_tmp978=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp978,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2612
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp97C=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3C0),Cyc_Absyn_uint_typ,array_len,_tmp3C6,0U);_tmp3C6=_tmp97C;});
({void*_tmp97D=Cyc_Toc_stmt_exp_r(_tmp3C6);e->r=_tmp97D;});
if(_tmp3C3 != 0)
({void*_tmp97F=({void*_tmp97E=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp97E,Cyc_Absyn_copy_exp(e));});e->r=_tmp97F;});
goto _LL0;};};};};};case 18U: _LL27: _tmp501=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL28: {
# 2621
int _tmp3C7=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp501);
Cyc_Toc_set_in_sizeof(nv,_tmp3C7);
goto _LL0;}case 17U: _LL29: _tmp502=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL2A:
({void*_tmp981=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=17U,({void*_tmp980=Cyc_Toc_typ_to_c(_tmp502);_tmp3C8->f1=_tmp980;});_tmp3C8;});e->r=_tmp981;});goto _LL0;case 19U: _LL2B: _tmp504=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp503=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL2C: {
# 2627
void*_tmp3C9=_tmp504;
struct Cyc_List_List*_tmp3CA=_tmp503;
for(0;_tmp3CA != 0;_tmp3CA=_tmp3CA->tl){
void*_tmp3CB=(void*)_tmp3CA->hd;void*_tmp3CC=_tmp3CB;unsigned int _tmp3DE;struct _dyneither_ptr*_tmp3DD;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CC)->tag == 0U){_LLCD: _tmp3DD=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3CC)->f1;_LLCE:
 goto _LLCC;}else{_LLCF: _tmp3DE=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3CC)->f1;_LLD0:
# 2633
{void*_tmp3CD=Cyc_Tcutil_compress(_tmp3C9);void*_tmp3CE=_tmp3CD;struct Cyc_Absyn_Datatypefield*_tmp3DC;struct Cyc_List_List*_tmp3DB;struct Cyc_List_List*_tmp3DA;union Cyc_Absyn_AggrInfoU _tmp3D9;switch(*((int*)_tmp3CE)){case 11U: _LLD2: _tmp3D9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CE)->f1).aggr_info;_LLD3: {
# 2635
struct Cyc_Absyn_Aggrdecl*_tmp3CF=Cyc_Absyn_get_known_aggrdecl(_tmp3D9);
if(_tmp3CF->impl == 0)
({void*_tmp3D0=0U;({struct _dyneither_ptr _tmp982=({const char*_tmp3D1="struct fields must be known";_tag_dyneither(_tmp3D1,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp982,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});
_tmp3DA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3CF->impl))->fields;goto _LLD5;}case 12U: _LLD4: _tmp3DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3CE)->f2;_LLD5: {
# 2640
struct Cyc_Absyn_Aggrfield*_tmp3D2=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DA,(int)_tmp3DE);
({void*_tmp983=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->tag=0U,_tmp3D3->f1=_tmp3D2->name;_tmp3D3;}));_tmp3CA->hd=_tmp983;});
_tmp3C9=_tmp3D2->type;
goto _LLD1;}case 10U: _LLD6: _tmp3DB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3CE)->f1;_LLD7:
# 2645
({void*_tmp985=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->tag=0U,({struct _dyneither_ptr*_tmp984=Cyc_Absyn_fieldname((int)(_tmp3DE + 1));_tmp3D4->f1=_tmp984;});_tmp3D4;}));_tmp3CA->hd=_tmp985;});
({void*_tmp986=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DB,(int)_tmp3DE)).f2;_tmp3C9=_tmp986;});
goto _LLD1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CE)->f1).field_info).KnownDatatypefield).tag == 2){_LLD8: _tmp3DC=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CE)->f1).field_info).KnownDatatypefield).val).f2;_LLD9:
# 2649
 if(_tmp3DE == 0)
({void*_tmp987=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->tag=0U,_tmp3D5->f1=Cyc_Toc_tag_sp;_tmp3D5;}));_tmp3CA->hd=_tmp987;});else{
# 2652
({void*_tmp988=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3DC->typs,(int)(_tmp3DE - 1))).f2;_tmp3C9=_tmp988;});
({void*_tmp98A=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->tag=0U,({struct _dyneither_ptr*_tmp989=Cyc_Absyn_fieldname((int)_tmp3DE);_tmp3D6->f1=_tmp989;});_tmp3D6;}));_tmp3CA->hd=_tmp98A;});}
# 2655
goto _LLD1;}else{goto _LLDA;}default: _LLDA: _LLDB:
({void*_tmp3D7=0U;({struct _dyneither_ptr _tmp98B=({const char*_tmp3D8="impossible type for offsetof tuple index";_tag_dyneither(_tmp3D8,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98B,_tag_dyneither(_tmp3D7,sizeof(void*),0U));});});
goto _LLD1;}_LLD1:;}
# 2659
goto _LLCC;}_LLCC:;}
# 2662
({void*_tmp98D=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->tag=19U,({void*_tmp98C=Cyc_Toc_typ_to_c(_tmp504);_tmp3DF->f1=_tmp98C;}),_tmp3DF->f2=_tmp503;_tmp3DF;});e->r=_tmp98D;});
goto _LL0;}case 20U: _LL2D: _tmp505=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL2E: {
# 2665
int _tmp3E0=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3E1=Cyc_Tcutil_compress((void*)_check_null(_tmp505->topt));
{void*_tmp3E2=_tmp3E1;void*_tmp3F7;struct Cyc_Absyn_Tqual _tmp3F6;void*_tmp3F5;union Cyc_Absyn_Constraint*_tmp3F4;union Cyc_Absyn_Constraint*_tmp3F3;union Cyc_Absyn_Constraint*_tmp3F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->tag == 5U){_LLDD: _tmp3F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).elt_typ;_tmp3F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).elt_tq;_tmp3F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).rgn;_tmp3F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).nullable;_tmp3F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).bounds;_tmp3F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2)->f1).ptr_atts).zero_term;_LLDE:
# 2670
{void*_tmp3E3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3F3);void*_tmp3E4=_tmp3E3;struct Cyc_Absyn_Exp*_tmp3EF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3E4)->tag == 1U){_LLE2: _tmp3EF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3E4)->f1;_LLE3: {
# 2672
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp505);
Cyc_Toc_exp_to_c(nv,_tmp505);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3E5=0U;({unsigned int _tmp98F=e->loc;struct _dyneither_ptr _tmp98E=({const char*_tmp3E6="inserted null check due to dereference";_tag_dyneither(_tmp3E6,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp98F,_tmp98E,_tag_dyneither(_tmp3E5,sizeof(void*),0U));});});
# 2678
({void*_tmp993=({void*_tmp992=Cyc_Toc_typ_to_c(_tmp3E1);Cyc_Toc_cast_it_r(_tmp992,({
struct Cyc_Absyn_Exp*_tmp991=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp991,({struct Cyc_List_List*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));
({struct Cyc_Absyn_Exp*_tmp990=Cyc_Absyn_copy_exp(_tmp505);_tmp3E7->hd=_tmp990;}),_tmp3E7->tl=0;_tmp3E7;}),0U);}));});
# 2678
_tmp505->r=_tmp993;});}
# 2686
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F2)){
struct _tuple13 _tmp3E8=Cyc_Evexp_eval_const_uint_exp(_tmp3EF);struct _tuple13 _tmp3E9=_tmp3E8;unsigned int _tmp3ED;int _tmp3EC;_LLE7: _tmp3ED=_tmp3E9.f1;_tmp3EC=_tmp3E9.f2;_LLE8:;
# 2692
if(!_tmp3EC  || _tmp3ED <= 0)
({void*_tmp3EA=0U;({unsigned int _tmp995=e->loc;struct _dyneither_ptr _tmp994=({const char*_tmp3EB="cannot determine dereference is in bounds";_tag_dyneither(_tmp3EB,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp995,_tmp994,_tag_dyneither(_tmp3EA,sizeof(void*),0U));});});}
# 2695
goto _LLE1;}}else{_LLE4: _LLE5: {
# 2698
struct Cyc_Absyn_Exp*_tmp3EE=Cyc_Absyn_uint_exp(0U,0U);
_tmp3EE->topt=Cyc_Absyn_uint_typ;
({void*_tmp996=Cyc_Toc_subscript_exp_r(_tmp505,_tmp3EE);e->r=_tmp996;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLE1;}}_LLE1:;}
# 2704
goto _LLDC;}else{_LLDF: _LLE0:
({void*_tmp3F0=0U;({struct _dyneither_ptr _tmp997=({const char*_tmp3F1="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3F1,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp997,_tag_dyneither(_tmp3F0,sizeof(void*),0U));});});}_LLDC:;}
# 2707
Cyc_Toc_set_lhs(nv,_tmp3E0);
goto _LL0;};}case 21U: _LL2F: _tmp509=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp508=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp507=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp506=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL30: {
# 2710
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp509->topt);
Cyc_Toc_exp_to_c(nv,_tmp509);
if(_tmp507  && _tmp506)
({void*_tmp99C=({struct Cyc_Absyn_Exp*_tmp99B=_tmp509;void*_tmp99A=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp999=e1_cyc_type;struct _dyneither_ptr*_tmp998=_tmp508;Cyc_Toc_check_tagged_union(_tmp99B,_tmp99A,_tmp999,_tmp998,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2714
e->r=_tmp99C;});
# 2718
if(is_poly)
({void*_tmp99E=({void*_tmp99D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp99D,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp99E;});
goto _LL0;}case 22U: _LL31: _tmp50D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp50C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp50B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp50A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL32: {
# 2722
int _tmp3F8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp50D->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp50D);
Cyc_Toc_exp_to_c(nv,_tmp50D);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3F9=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3FA=_tmp3F9;void*_tmp410;struct Cyc_Absyn_Tqual _tmp40F;void*_tmp40E;union Cyc_Absyn_Constraint*_tmp40D;union Cyc_Absyn_Constraint*_tmp40C;union Cyc_Absyn_Constraint*_tmp40B;_LLEA: _tmp410=_tmp3FA.elt_typ;_tmp40F=_tmp3FA.elt_tq;_tmp40E=(_tmp3FA.ptr_atts).rgn;_tmp40D=(_tmp3FA.ptr_atts).nullable;_tmp40C=(_tmp3FA.ptr_atts).bounds;_tmp40B=(_tmp3FA.ptr_atts).zero_term;_LLEB:;
{void*_tmp3FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp40C);void*_tmp3FC=_tmp3FB;struct Cyc_Absyn_Exp*_tmp40A;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3FC)->tag == 1U){_LLED: _tmp40A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3FC)->f1;_LLEE: {
# 2733
struct _tuple13 _tmp3FD=Cyc_Evexp_eval_const_uint_exp(_tmp40A);struct _tuple13 _tmp3FE=_tmp3FD;unsigned int _tmp408;int _tmp407;_LLF2: _tmp408=_tmp3FE.f1;_tmp407=_tmp3FE.f2;_LLF3:;
if(_tmp407){
if(_tmp408 < 1)
({void*_tmp3FF=0U;({struct _dyneither_ptr _tmp99F=({const char*_tmp400="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp400,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp99F,_tag_dyneither(_tmp3FF,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp401=0U;({unsigned int _tmp9A1=e->loc;struct _dyneither_ptr _tmp9A0=({const char*_tmp402="inserted null check due to dereference";_tag_dyneither(_tmp402,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9A1,_tmp9A0,_tag_dyneither(_tmp401,sizeof(void*),0U));});});
# 2741
({void*_tmp9A5=({void*_tmp9A4=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9A4,({
struct Cyc_Absyn_Exp*_tmp9A3=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9A3,({struct Cyc_Absyn_Exp*_tmp403[1U];({struct Cyc_Absyn_Exp*_tmp9A2=
Cyc_Absyn_new_exp(_tmp50D->r,0U);_tmp403[0]=_tmp9A2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp403,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2741
_tmp50D->r=_tmp9A5;});}}else{
# 2746
if(!Cyc_Evexp_c_can_eval(_tmp40A))
({void*_tmp404=0U;({unsigned int _tmp9A7=e->loc;struct _dyneither_ptr _tmp9A6=({const char*_tmp405="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp405,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp9A7,_tmp9A6,_tag_dyneither(_tmp404,sizeof(void*),0U));});});
# 2750
({void*_tmp9AD=({void*_tmp9AC=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9AC,({
struct Cyc_Absyn_Exp*_tmp9AB=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9AB,({struct Cyc_Absyn_Exp*_tmp406[4U];({
struct Cyc_Absyn_Exp*_tmp9AA=Cyc_Absyn_new_exp(_tmp50D->r,0U);_tmp406[0]=_tmp9AA;}),_tmp406[1]=_tmp40A,({
struct Cyc_Absyn_Exp*_tmp9A9=Cyc_Absyn_sizeoftyp_exp(_tmp410,0U);_tmp406[2]=_tmp9A9;}),({
struct Cyc_Absyn_Exp*_tmp9A8=Cyc_Absyn_uint_exp(0U,0U);_tmp406[3]=_tmp9A8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp406,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2750
_tmp50D->r=_tmp9AD;});}
# 2756
goto _LLEC;}}else{_LLEF: _LLF0: {
# 2759
void*ta1=Cyc_Toc_typ_to_c(_tmp410);
({void*_tmp9B3=({void*_tmp9B2=Cyc_Absyn_cstar_typ(ta1,_tmp40F);Cyc_Toc_cast_it_r(_tmp9B2,({
struct Cyc_Absyn_Exp*_tmp9B1=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9B1,({struct Cyc_Absyn_Exp*_tmp409[3U];({
struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Absyn_new_exp(_tmp50D->r,0U);_tmp409[0]=_tmp9B0;}),({
struct Cyc_Absyn_Exp*_tmp9AF=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp409[1]=_tmp9AF;}),({
struct Cyc_Absyn_Exp*_tmp9AE=Cyc_Absyn_uint_exp(0U,0U);_tmp409[2]=_tmp9AE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp409,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2760
_tmp50D->r=_tmp9B3;});
# 2767
goto _LLEC;}}_LLEC:;}
# 2769
if(_tmp50B  && _tmp50A)
({void*_tmp9B8=({struct Cyc_Absyn_Exp*_tmp9B7=_tmp50D;void*_tmp9B6=Cyc_Toc_typ_to_c(e1typ);void*_tmp9B5=_tmp410;struct _dyneither_ptr*_tmp9B4=_tmp50C;Cyc_Toc_check_tagged_union(_tmp9B7,_tmp9B6,_tmp9B5,_tmp9B4,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9B8;});
# 2772
if(is_poly  && _tmp50A)
({void*_tmp9BA=({void*_tmp9B9=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9B9,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9BA;});
Cyc_Toc_set_lhs(nv,_tmp3F8);
goto _LL0;};};}case 23U: _LL33: _tmp50F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp50E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL34: {
# 2777
int _tmp411=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp412=Cyc_Tcutil_compress((void*)_check_null(_tmp50F->topt));
# 2781
{void*_tmp413=_tmp412;void*_tmp441;struct Cyc_Absyn_Tqual _tmp440;void*_tmp43F;union Cyc_Absyn_Constraint*_tmp43E;union Cyc_Absyn_Constraint*_tmp43D;union Cyc_Absyn_Constraint*_tmp43C;struct Cyc_List_List*_tmp43B;switch(*((int*)_tmp413)){case 10U: _LLF5: _tmp43B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp413)->f1;_LLF6:
# 2784
 Cyc_Toc_exp_to_c(nv,_tmp50F);
Cyc_Toc_exp_to_c(nv,_tmp50E);{
struct _tuple13 _tmp414=Cyc_Evexp_eval_const_uint_exp(_tmp50E);struct _tuple13 _tmp415=_tmp414;unsigned int _tmp419;int _tmp418;_LLFC: _tmp419=_tmp415.f1;_tmp418=_tmp415.f2;_LLFD:;
if(!_tmp418)
({void*_tmp416=0U;({struct _dyneither_ptr _tmp9BB=({const char*_tmp417="unknown tuple subscript in translation to C";_tag_dyneither(_tmp417,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9BB,_tag_dyneither(_tmp416,sizeof(void*),0U));});});
({void*_tmp9BD=({struct Cyc_Absyn_Exp*_tmp9BC=_tmp50F;Cyc_Toc_aggrmember_exp_r(_tmp9BC,Cyc_Absyn_fieldname((int)(_tmp419 + 1)));});e->r=_tmp9BD;});
goto _LLF4;};case 5U: _LLF7: _tmp441=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).elt_typ;_tmp440=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).elt_tq;_tmp43F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).rgn;_tmp43E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).nullable;_tmp43D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).bounds;_tmp43C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp413)->f1).ptr_atts).zero_term;_LLF8: {
# 2792
struct Cyc_List_List*_tmp41A=Cyc_Toc_get_relns(_tmp50F);
int _tmp41B=Cyc_Toc_need_null_check(_tmp50F);
int _tmp41C=Cyc_Toc_in_sizeof(nv);
# 2800
int in_bnds=_tmp41C;
{void*_tmp41D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp43D);void*_tmp41E=_tmp41D;_LLFF: _LL100:
# 2803
({int _tmp9BE=in_bnds  || Cyc_Toc_check_bounds(_tmp412,_tmp41A,_tmp50F,_tmp50E);in_bnds=_tmp9BE;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp421=({struct Cyc_String_pa_PrintArg_struct _tmp79F;_tmp79F.tag=0U,({struct _dyneither_ptr _tmp9BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp79F.f1=_tmp9BF;});_tmp79F;});void*_tmp41F[1U];_tmp41F[0]=& _tmp421;({unsigned int _tmp9C1=e->loc;struct _dyneither_ptr _tmp9C0=({const char*_tmp420="bounds check necessary for %s";_tag_dyneither(_tmp420,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp9C1,_tmp9C0,_tag_dyneither(_tmp41F,sizeof(void*),1U));});});_LLFE:;}
# 2810
Cyc_Toc_exp_to_c(nv,_tmp50F);
Cyc_Toc_exp_to_c(nv,_tmp50E);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp422=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp43D);void*_tmp423=_tmp422;struct Cyc_Absyn_Exp*_tmp438;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp423)->tag == 1U){_LL102: _tmp438=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp423)->f1;_LL103: {
# 2815
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp43E) && _tmp41B;
void*ta1=Cyc_Toc_typ_to_c(_tmp441);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp440);
struct _tuple13 _tmp424=Cyc_Evexp_eval_const_uint_exp(_tmp438);struct _tuple13 _tmp425=_tmp424;unsigned int _tmp435;int _tmp434;_LL107: _tmp435=_tmp425.f1;_tmp434=_tmp425.f2;_LL108:;
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp426=0U;({unsigned int _tmp9C3=e->loc;struct _dyneither_ptr _tmp9C2=({const char*_tmp427="inserted null check due to dereference";_tag_dyneither(_tmp427,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9C3,_tmp9C2,_tag_dyneither(_tmp426,sizeof(void*),0U));});});
({void*_tmp9C7=({void*_tmp9C6=ta2;Cyc_Toc_cast_it_r(_tmp9C6,({struct Cyc_Absyn_Exp*_tmp9C5=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9C5,({struct Cyc_Absyn_Exp*_tmp428[1U];({struct Cyc_Absyn_Exp*_tmp9C4=
Cyc_Absyn_copy_exp(_tmp50F);_tmp428[0]=_tmp9C4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp428,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2825
_tmp50F->r=_tmp9C7;});}else{
# 2829
if((_tmp434  && _tmp435 == 1) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp43C)){
# 2831
if(!Cyc_Evexp_c_can_eval(_tmp438))
({void*_tmp429=0U;({unsigned int _tmp9C9=e->loc;struct _dyneither_ptr _tmp9C8=({const char*_tmp42A="cannot determine subscript is in bounds";_tag_dyneither(_tmp42A,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9C9,_tmp9C8,_tag_dyneither(_tmp429,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp50F);
({void*_tmp9CC=Cyc_Toc_deref_exp_r(({void*_tmp9CB=ta2;Cyc_Toc_cast_it(_tmp9CB,({
struct Cyc_Absyn_Exp*_tmp9CA=function_e;Cyc_Absyn_fncall_exp(_tmp9CA,({struct Cyc_Absyn_Exp*_tmp42B[3U];_tmp42B[0]=_tmp50F,_tmp42B[1]=_tmp438,_tmp42B[2]=_tmp50E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2834
e->r=_tmp9CC;});};}else{
# 2837
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp438))
({void*_tmp42C=0U;({unsigned int _tmp9CE=e->loc;struct _dyneither_ptr _tmp9CD=({const char*_tmp42D="cannot determine subscript is in bounds";_tag_dyneither(_tmp42D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9CE,_tmp9CD,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp42E=0U;({unsigned int _tmp9D0=e->loc;struct _dyneither_ptr _tmp9CF=({const char*_tmp42F="inserted null check due to dereference";_tag_dyneither(_tmp42F,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9D0,_tmp9CF,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});
# 2843
({void*_tmp9D4=Cyc_Toc_deref_exp_r(({void*_tmp9D3=ta2;Cyc_Toc_cast_it(_tmp9D3,({
struct Cyc_Absyn_Exp*_tmp9D2=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9D2,({struct Cyc_Absyn_Exp*_tmp430[4U];_tmp430[0]=_tmp50F,_tmp430[1]=_tmp438,({
# 2846
struct Cyc_Absyn_Exp*_tmp9D1=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp430[2]=_tmp9D1;}),_tmp430[3]=_tmp50E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp430,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2843
e->r=_tmp9D4;});}else{
# 2849
if(!Cyc_Evexp_c_can_eval(_tmp438))
({void*_tmp431=0U;({unsigned int _tmp9D6=e->loc;struct _dyneither_ptr _tmp9D5=({const char*_tmp432="cannot determine subscript is in bounds";_tag_dyneither(_tmp432,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9D6,_tmp9D5,_tag_dyneither(_tmp431,sizeof(void*),0U));});});
# 2852
({void*_tmp9D9=({struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9D8,({struct Cyc_Absyn_Exp*_tmp433[2U];_tmp433[0]=_tmp438,({
struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Absyn_copy_exp(_tmp50E);_tmp433[1]=_tmp9D7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp433,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2852
_tmp50E->r=_tmp9D9;});}}}}
# 2855
goto _LL101;}}else{_LL104: _LL105: {
# 2857
void*ta1=Cyc_Toc_typ_to_c(_tmp441);
if(in_bnds){
# 2861
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9DC=({struct Cyc_Absyn_Exp*_tmp9DB=({void*_tmp9DA=Cyc_Absyn_cstar_typ(ta1,_tmp440);Cyc_Toc_cast_it(_tmp9DA,
Cyc_Toc_member_exp(_tmp50F,Cyc_Toc_curr_sp,0U));});
# 2862
Cyc_Toc_subscript_exp_r(_tmp9DB,_tmp50E);});e->r=_tmp9DC;});}else{
# 2866
struct Cyc_Absyn_Exp*_tmp436=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmp9E0=Cyc_Toc_deref_exp_r(({void*_tmp9DF=Cyc_Absyn_cstar_typ(ta1,_tmp440);Cyc_Toc_cast_it(_tmp9DF,({
struct Cyc_Absyn_Exp*_tmp9DE=_tmp436;Cyc_Absyn_fncall_exp(_tmp9DE,({struct Cyc_Absyn_Exp*_tmp437[3U];_tmp437[0]=_tmp50F,({
struct Cyc_Absyn_Exp*_tmp9DD=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp437[1]=_tmp9DD;}),_tmp437[2]=_tmp50E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp437,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2867
e->r=_tmp9E0;});}
# 2872
goto _LL101;}}_LL101:;}
# 2874
goto _LLF4;}default: _LLF9: _LLFA:
({void*_tmp439=0U;({struct _dyneither_ptr _tmp9E1=({const char*_tmp43A="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp43A,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E1,_tag_dyneither(_tmp439,sizeof(void*),0U));});});}_LLF4:;}
# 2877
Cyc_Toc_set_lhs(nv,_tmp411);
goto _LL0;};}case 24U: _LL35: _tmp510=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL36:
# 2880
 if(!Cyc_Toc_is_toplevel(nv)){
# 2882
void*_tmp442=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp510));
{void*_tmp443=_tmp442;union Cyc_Absyn_AggrInfoU _tmp448;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp443)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp443)->f1).targs == 0){_LL10A: _tmp448=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp443)->f1).aggr_info;_LL10B: {
# 2885
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp448);
{int i=1;for(0;_tmp510 != 0;(_tmp510=_tmp510->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp510->hd);
({struct Cyc_List_List*_tmp9E4=({struct Cyc_List_List*_tmp444=_cycalloc(sizeof(*_tmp444));({struct _tuple20*_tmp9E3=({struct _dyneither_ptr*_tmp9E2=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9E2,(struct Cyc_Absyn_Exp*)_tmp510->hd);});_tmp444->hd=_tmp9E3;}),_tmp444->tl=dles;_tmp444;});dles=_tmp9E4;});}}
# 2891
({void*_tmp9E6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=29U,_tmp445->f1=sd->name,_tmp445->f2=0,({struct Cyc_List_List*_tmp9E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp445->f3=_tmp9E5;}),_tmp445->f4=sd;_tmp445;});e->r=_tmp9E6;});
e->topt=_tmp442;
goto _LL109;}}else{goto _LL10C;}}else{_LL10C: _LL10D:
({void*_tmp446=0U;({struct _dyneither_ptr _tmp9E7=({const char*_tmp447="tuple type not an aggregate";_tag_dyneither(_tmp447,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E7,_tag_dyneither(_tmp446,sizeof(void*),0U));});});}_LL109:;}
# 2896
goto _LL0;}else{
# 2900
struct Cyc_List_List*dles=0;
for(0;_tmp510 != 0;_tmp510=_tmp510->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp510->hd);
({struct Cyc_List_List*_tmp9E9=({struct Cyc_List_List*_tmp44A=_cycalloc(sizeof(*_tmp44A));({struct _tuple20*_tmp9E8=({struct _tuple20*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->f1=0,_tmp449->f2=(struct Cyc_Absyn_Exp*)_tmp510->hd;_tmp449;});_tmp44A->hd=_tmp9E8;}),_tmp44A->tl=dles;_tmp44A;});dles=_tmp9E9;});}
# 2905
({void*_tmp9EA=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9EA;});}
# 2907
goto _LL0;case 26U: _LL37: _tmp511=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL38:
# 2913
{struct Cyc_List_List*_tmp44B=_tmp511;for(0;_tmp44B != 0;_tmp44B=_tmp44B->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp44B->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9EB=Cyc_Toc_unresolvedmem_exp_r(0,_tmp511);e->r=_tmp9EB;});
goto _LL0;case 27U: _LL39: _tmp515=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp514=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp513=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp512=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL3A:
# 2921
 Cyc_Toc_exp_to_c(nv,_tmp514);
Cyc_Toc_exp_to_c(nv,_tmp513);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp44C=Cyc_Evexp_eval_const_uint_exp(_tmp514);struct _tuple13 _tmp44D=_tmp44C;unsigned int _tmp457;int _tmp456;_LL10F: _tmp457=_tmp44D.f1;_tmp456=_tmp44D.f2;_LL110:;{
void*_tmp44E=Cyc_Toc_typ_to_c((void*)_check_null(_tmp513->topt));
struct Cyc_List_List*es=0;
# 2928
if(!Cyc_Toc_is_zero(_tmp513)){
if(!_tmp456)
({void*_tmp44F=0U;({unsigned int _tmp9ED=_tmp514->loc;struct _dyneither_ptr _tmp9EC=({const char*_tmp450="cannot determine value of constant";_tag_dyneither(_tmp450,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9ED,_tmp9EC,_tag_dyneither(_tmp44F,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp457;++ i){
({struct Cyc_List_List*_tmp9EF=({struct Cyc_List_List*_tmp452=_cycalloc(sizeof(*_tmp452));({struct _tuple20*_tmp9EE=({struct _tuple20*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->f1=0,_tmp451->f2=_tmp513;_tmp451;});_tmp452->hd=_tmp9EE;}),_tmp452->tl=es;_tmp452;});es=_tmp9EF;});}}
# 2934
if(_tmp512){
struct Cyc_Absyn_Exp*_tmp453=({void*_tmp9F0=_tmp44E;Cyc_Toc_cast_it(_tmp9F0,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp9F3=({struct Cyc_List_List*_tmp9F2=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F2,({struct Cyc_List_List*_tmp455=_cycalloc(sizeof(*_tmp455));({struct _tuple20*_tmp9F1=({struct _tuple20*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->f1=0,_tmp454->f2=_tmp453;_tmp454;});_tmp455->hd=_tmp9F1;}),_tmp455->tl=0;_tmp455;}));});es=_tmp9F3;});}}
# 2939
({void*_tmp9F4=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9F4;});};}
# 2941
goto _LL0;case 28U: _LL3B: _tmp518=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp517=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp516=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL3C:
# 2948
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9F5=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9F5;});else{
# 2951
Cyc_Toc_exp_to_c(nv,_tmp518);}
goto _LL0;case 30U: _LL3D: _tmp51A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp519=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL3E:
# 2955
{struct Cyc_List_List*_tmp458=_tmp519;for(0;_tmp458 != 0;_tmp458=_tmp458->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp458->hd)).f2);}}{
void*_tmp459=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp459;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp45A=Cyc_Tcutil_compress(_tmp459);void*_tmp45B=_tmp45A;union Cyc_Absyn_AggrInfoU _tmp45F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45B)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45B)->f1).targs == 0){_LL112: _tmp45F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45B)->f1).aggr_info;_LL113: {
# 2962
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp45F);
({void*_tmp9F6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=29U,_tmp45C->f1=sd->name,_tmp45C->f2=0,_tmp45C->f3=_tmp519,_tmp45C->f4=sd;_tmp45C;});e->r=_tmp9F6;});
e->topt=_tmp459;
goto _LL111;}}else{goto _LL114;}}else{_LL114: _LL115:
({void*_tmp45D=0U;({struct _dyneither_ptr _tmp9F7=({const char*_tmp45E="anonStruct type not an aggregate";_tag_dyneither(_tmp45E,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F7,_tag_dyneither(_tmp45D,sizeof(void*),0U));});});}_LL111:;}else{
# 2969
({void*_tmp9F8=Cyc_Toc_unresolvedmem_exp_r(0,_tmp519);e->r=_tmp9F8;});}
goto _LL0;};case 29U: _LL3F: _tmp51E=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp51D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp51C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_tmp51B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30C)->f4;_LL40:
# 2975
 if(_tmp51B == 0  || _tmp51B->impl == 0)
({void*_tmp460=0U;({struct _dyneither_ptr _tmp9F9=({const char*_tmp461="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp461,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9F9,_tag_dyneither(_tmp460,sizeof(void*),0U));});});{
void*_tmp462=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp463=Cyc_Tcutil_compress(_tmp462);void*_tmp464=_tmp463;union Cyc_Absyn_AggrInfoU _tmp467;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp464)->tag == 11U){_LL117: _tmp467=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp464)->f1).aggr_info;_LL118:
({struct _tuple1*_tmp9FA=(Cyc_Absyn_aggr_kinded_name(_tmp467)).f2;*_tmp51E=_tmp9FA;});goto _LL116;}else{_LL119: _LL11A:
({void*_tmp465=0U;({struct _dyneither_ptr _tmp9FB=({const char*_tmp466="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp466,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9FB,_tag_dyneither(_tmp465,sizeof(void*),0U));});});}_LL116:;}{
# 2986
struct Cyc_List_List*_tmp468=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->fields;
if(_tmp468 == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp468))->tl != 0;_tmp468=_tmp468->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp469=(struct Cyc_Absyn_Aggrfield*)_tmp468->hd;
struct Cyc_List_List*_tmp46A=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp51C,_tmp51B->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->fields);
# 2993
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp46B=Cyc_Tcutil_compress(old_typ);void*_tmp46C=_tmp46B;struct Cyc_List_List*_tmp499;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46C)->tag == 11U){_LL11C: _tmp499=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46C)->f1).targs;_LL11D:
# 2997
{struct Cyc_List_List*_tmp46D=_tmp46A;for(0;_tmp46D != 0;_tmp46D=_tmp46D->tl){
struct _tuple34*_tmp46E=(struct _tuple34*)_tmp46D->hd;struct _tuple34*_tmp46F=_tmp46E;struct Cyc_Absyn_Aggrfield*_tmp496;struct Cyc_Absyn_Exp*_tmp495;_LL121: _tmp496=_tmp46F->f1;_tmp495=_tmp46F->f2;_LL122:;{
void*_tmp470=_tmp495->topt;
Cyc_Toc_exp_to_c(nv,_tmp495);
# 3002
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp496->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp495->topt)))
({void*_tmp9FD=({void*_tmp9FC=Cyc_Toc_typ_to_c(_tmp496->type);Cyc_Toc_cast_it(_tmp9FC,
Cyc_Absyn_copy_exp(_tmp495));})->r;
# 3004
_tmp495->r=_tmp9FD;});
# 3007
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->tagged){
struct _dyneither_ptr*_tmp471=_tmp496->name;
struct Cyc_Absyn_Exp*_tmp472=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp51B,_tmp471),0U);
struct _tuple20*_tmp473=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp472);
struct _tuple20*_tmp474=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp495);
struct _tuple1*s=({struct _tuple1*_tmp47F=_cycalloc(sizeof(*_tmp47F));({union Cyc_Absyn_Nmspace _tmpA01=Cyc_Absyn_Abs_n(0,1);_tmp47F->f1=_tmpA01;}),({
struct _dyneither_ptr*_tmpA00=({struct _dyneither_ptr*_tmp47E=_cycalloc(sizeof(*_tmp47E));({struct _dyneither_ptr _tmp9FF=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp47C=({struct Cyc_String_pa_PrintArg_struct _tmp7A1;_tmp7A1.tag=0U,_tmp7A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp51B->name).f2);_tmp7A1;});struct Cyc_String_pa_PrintArg_struct _tmp47D=({struct Cyc_String_pa_PrintArg_struct _tmp7A0;_tmp7A0.tag=0U,_tmp7A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp471);_tmp7A0;});void*_tmp47A[2U];_tmp47A[0]=& _tmp47C,_tmp47A[1]=& _tmp47D;({struct _dyneither_ptr _tmp9FE=({const char*_tmp47B="_union_%s_%s";_tag_dyneither(_tmp47B,sizeof(char),13U);});Cyc_aprintf(_tmp9FE,_tag_dyneither(_tmp47A,sizeof(void*),2U));});});*_tmp47E=_tmp9FF;});_tmp47E;});_tmp47F->f2=_tmpA00;});_tmp47F;});
# 3015
struct _tuple20*_tmp475=({
struct _dyneither_ptr*_tmpA03=_tmp471;Cyc_Toc_make_field(_tmpA03,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->tag=29U,_tmp479->f1=s,_tmp479->f2=0,({struct Cyc_List_List*_tmpA02=({struct _tuple20*_tmp478[2U];_tmp478[0]=_tmp473,_tmp478[1]=_tmp474;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp478,sizeof(struct _tuple20*),2U));});_tmp479->f3=_tmpA02;}),_tmp479->f4=0;_tmp479;}),0U));});
# 3018
({void*_tmpA05=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=29U,_tmp477->f1=*_tmp51E,_tmp477->f2=0,({struct Cyc_List_List*_tmpA04=({struct _tuple20*_tmp476[1U];_tmp476[0]=_tmp475;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp476,sizeof(struct _tuple20*),1U));});_tmp477->f3=_tmpA04;}),_tmp477->f4=_tmp51B;_tmp477;});e->r=_tmpA05;});}
# 3024
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp469 == _tmp496){
struct Cyc_List_List*_tmp480=({struct Cyc_List_List*_tmpA06=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->exist_vars,_tmp51D);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA06,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp51B->tvs,_tmp499));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp481=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->fields;for(0;_tmp481 != 0;_tmp481=_tmp481->tl){
struct Cyc_Absyn_Aggrfield*_tmp482=(struct Cyc_Absyn_Aggrfield*)_tmp481->hd;
void*_tmp483=Cyc_Tcutil_substitute(_tmp480,_tmp482->type);
struct Cyc_Absyn_Aggrfield*_tmp484=({struct Cyc_Absyn_Aggrfield*_tmpA07=_tmp482;Cyc_Toc_aggrfield_to_c(_tmpA07,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp480,_tmp483)));});
({struct Cyc_List_List*_tmpA08=({struct Cyc_List_List*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->hd=_tmp484,_tmp485->tl=new_fields;_tmp485;});new_fields=_tmpA08;});
# 3039
if(_tmp481->tl == 0){
{void*_tmp486=Cyc_Tcutil_compress(_tmp484->type);void*_tmp487=_tmp486;struct Cyc_Absyn_ArrayInfo _tmp48C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp487)->tag == 8U){_LL124: _tmp48C=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp487)->f1;_LL125:
# 3042
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp48C.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp488=_tmp48C;
({struct Cyc_Absyn_Exp*_tmpA09=Cyc_Absyn_uint_exp(0U,0U);_tmp488.num_elts=_tmpA09;});
({void*_tmpA0A=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->tag=8U,_tmp489->f1=_tmp488;_tmp489;});_tmp484->type=_tmpA0A;});}
# 3047
goto _LL123;}else{_LL126: _LL127:
# 3051
 if(_tmp495->topt == 0)
goto _LL123;
{void*_tmp48A=Cyc_Tcutil_compress((void*)_check_null(_tmp495->topt));void*_tmp48B=_tmp48A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp48B)->tag == 11U){_LL129: _LL12A:
# 3055
 _tmp484->type=(void*)_check_null(_tmp495->topt);goto _LL128;}else{_LL12B: _LL12C:
 goto _LL128;}_LL128:;}
# 3058
goto _LL123;}_LL123:;}
# 3062
if(!Cyc_Toc_is_array_type(_tmp482->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp482->type)))
({struct Cyc_List_List*_tmpA0C=({struct Cyc_List_List*_tmp48E=_cycalloc(sizeof(*_tmp48E));({void*_tmpA0B=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->tag=6U,_tmp48D->f1=0;_tmp48D;});_tmp48E->hd=_tmpA0B;}),_tmp48E->tl=_tmp484->attributes;_tmp48E;});_tmp484->attributes=_tmpA0C;});}}}
# 3068
({struct Cyc_Absyn_Aggrdecl*_tmpA10=({struct _dyneither_ptr*_tmpA0F=({struct _dyneither_ptr*_tmp492=_cycalloc(sizeof(*_tmp492));({struct _dyneither_ptr _tmpA0E=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp491=({struct Cyc_Int_pa_PrintArg_struct _tmp7A2;_tmp7A2.tag=1U,_tmp7A2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp7A2;});void*_tmp48F[1U];_tmp48F[0]=& _tmp491;({struct _dyneither_ptr _tmpA0D=({const char*_tmp490="_genStruct%d";_tag_dyneither(_tmp490,sizeof(char),13U);});Cyc_aprintf(_tmpA0D,_tag_dyneither(_tmp48F,sizeof(void*),1U));});});*_tmp492=_tmpA0E;});_tmp492;});Cyc_Toc_make_c_struct_defn(_tmpA0F,
# 3070
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 3068
_tmp51B=_tmpA10;});
# 3071
*_tmp51E=_tmp51B->name;
Cyc_Toc_aggrdecl_to_c(_tmp51B,1);
# 3074
({void*_tmpA12=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpA11=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp493=_cycalloc(sizeof(*_tmp493));*_tmp493=_tmp51B;_tmp493;}));(_tmp494->f1).aggr_info=_tmpA11;}),(_tmp494->f1).targs=0;_tmp494;});e->topt=_tmpA12;});}};}}
# 3077
goto _LL11B;}else{_LL11E: _LL11F:
({void*_tmp497=0U;({struct _dyneither_ptr _tmpA13=({const char*_tmp498="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp498,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA13,_tag_dyneither(_tmp497,sizeof(void*),0U));});});}_LL11B:;}else{
# 3086
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->tagged){
# 3088
struct _tuple34*_tmp49A=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp46A))->hd;struct _tuple34*_tmp49B=_tmp49A;struct Cyc_Absyn_Aggrfield*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;_LL12E: _tmp4A4=_tmp49B->f1;_tmp4A3=_tmp49B->f2;_LL12F:;{
void*_tmp49C=(void*)_check_null(_tmp4A3->topt);
void*_tmp49D=_tmp4A4->type;
Cyc_Toc_exp_to_c(nv,_tmp4A3);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49D) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp49C))
({void*_tmpA15=({
void*_tmpA14=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpA14,Cyc_Absyn_new_exp(_tmp4A3->r,0U));});
# 3094
_tmp4A3->r=_tmpA15;});{
# 3097
int i=Cyc_Toc_get_member_offset(_tmp51B,_tmp4A4->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp49E=({struct _tuple20*_tmp4A0[2U];({struct _tuple20*_tmpA17=({struct _tuple20*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->f1=0,_tmp4A1->f2=field_tag_exp;_tmp4A1;});_tmp4A0[0]=_tmpA17;}),({struct _tuple20*_tmpA16=({struct _tuple20*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2->f1=0,_tmp4A2->f2=_tmp4A3;_tmp4A2;});_tmp4A0[1]=_tmpA16;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A0,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp49E,0U);
({void*_tmpA19=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp49F[1U];({struct _tuple20*_tmpA18=Cyc_Toc_make_field(_tmp4A4->name,umem);_tmp49F[0]=_tmpA18;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp49F,sizeof(struct _tuple20*),1U));}));e->r=_tmpA19;});};};}else{
# 3104
struct Cyc_List_List*_tmp4A5=0;
struct Cyc_List_List*_tmp4A6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->fields;
for(0;_tmp4A6 != 0;_tmp4A6=_tmp4A6->tl){
struct Cyc_List_List*_tmp4A7=_tmp46A;for(0;_tmp4A7 != 0;_tmp4A7=_tmp4A7->tl){
if((*((struct _tuple34*)_tmp4A7->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp4A6->hd){
struct _tuple34*_tmp4A8=(struct _tuple34*)_tmp4A7->hd;struct _tuple34*_tmp4A9=_tmp4A8;struct Cyc_Absyn_Aggrfield*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;_LL131: _tmp4AF=_tmp4A9->f1;_tmp4AE=_tmp4A9->f2;_LL132:;{
void*_tmp4AA=Cyc_Toc_typ_to_c(_tmp4AF->type);
void*_tmp4AB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4AE->topt));
Cyc_Toc_exp_to_c(nv,_tmp4AE);
# 3114
if(!Cyc_Tcutil_unify(_tmp4AA,_tmp4AB)){
# 3116
if(!Cyc_Tcutil_is_arithmetic_type(_tmp4AA) || !
Cyc_Tcutil_is_arithmetic_type(_tmp4AB))
({struct Cyc_Absyn_Exp*_tmpA1B=({void*_tmpA1A=_tmp4AA;Cyc_Toc_cast_it(_tmpA1A,Cyc_Absyn_copy_exp(_tmp4AE));});_tmp4AE=_tmpA1B;});}
({struct Cyc_List_List*_tmpA1D=({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct _tuple20*_tmpA1C=({struct _tuple20*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->f1=0,_tmp4AC->f2=_tmp4AE;_tmp4AC;});_tmp4AD->hd=_tmpA1C;}),_tmp4AD->tl=_tmp4A5;_tmp4AD;});_tmp4A5=_tmpA1D;});
break;};}}}
# 3123
({void*_tmpA1E=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4A5));e->r=_tmpA1E;});}}
# 3126
goto _LL0;};};};case 31U: _LL41: _tmp521=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp520=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_tmp51F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30C)->f3;_LL42: {
# 3130
struct Cyc_List_List*_tmp4B0=_tmp51F->typs;
{struct Cyc_List_List*_tmp4B1=_tmp521;for(0;_tmp4B1 != 0;(_tmp4B1=_tmp4B1->tl,_tmp4B0=_tmp4B0->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4B1->hd;
void*_tmp4B2=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4B0))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp4B2))
({void*_tmpA1F=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA1F;});}}{
# 3141
struct Cyc_Absyn_Exp*_tmp4B3=_tmp520->is_extensible?Cyc_Absyn_var_exp(_tmp51F->name,0U):
 Cyc_Toc_datatype_tag(_tmp520,_tmp51F->name);
# 3144
if(!Cyc_Toc_is_toplevel(nv)){
# 3146
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp521 != 0;(_tmp521=_tmp521->tl,++ i)){
({struct Cyc_List_List*_tmpA22=({struct Cyc_List_List*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));({struct _tuple20*_tmpA21=({struct _dyneither_ptr*_tmpA20=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA20,(struct Cyc_Absyn_Exp*)_tmp521->hd);});_tmp4B4->hd=_tmpA21;}),_tmp4B4->tl=dles;_tmp4B4;});dles=_tmpA22;});}}{
# 3152
struct _tuple20*_tmp4B5=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp4B3);
({void*_tmpA26=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->tag=29U,({struct _tuple1*_tmpA25=Cyc_Toc_collapse_qvars(_tmp51F->name,_tmp520->name);_tmp4B7->f1=_tmpA25;}),_tmp4B7->f2=0,({
struct Cyc_List_List*_tmpA24=({struct Cyc_List_List*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->hd=_tmp4B5,({struct Cyc_List_List*_tmpA23=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp4B6->tl=_tmpA23;});_tmp4B6;});_tmp4B7->f3=_tmpA24;}),_tmp4B7->f4=0;_tmp4B7;});
# 3153
e->r=_tmpA26;});};}else{
# 3158
struct Cyc_List_List*_tmp4B8=0;
for(0;_tmp521 != 0;_tmp521=_tmp521->tl){
({struct Cyc_List_List*_tmpA28=({struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));({struct _tuple20*_tmpA27=({struct _tuple20*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->f1=0,_tmp4B9->f2=(struct Cyc_Absyn_Exp*)_tmp521->hd;_tmp4B9;});_tmp4BA->hd=_tmpA27;}),_tmp4BA->tl=_tmp4B8;_tmp4BA;});_tmp4B8=_tmpA28;});}
({void*_tmpA2B=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));({struct _tuple20*_tmpA2A=({struct _tuple20*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->f1=0,_tmp4BB->f2=_tmp4B3;_tmp4BB;});_tmp4BC->hd=_tmpA2A;}),({
struct Cyc_List_List*_tmpA29=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4B8);_tmp4BC->tl=_tmpA29;});_tmp4BC;}));
# 3161
e->r=_tmpA2B;});}
# 3164
goto _LL0;};}case 32U: _LL43: _LL44:
# 3166
 goto _LL46;case 33U: _LL45: _LL46:
 goto _LL0;case 34U: _LL47: _tmp527=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).is_calloc;_tmp526=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).rgn;_tmp525=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).elt_type;_tmp524=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).num_elts;_tmp523=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).fat_result;_tmp522=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30C)->f1).inline_call;_LL48: {
# 3170
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp525)));
Cyc_Toc_exp_to_c(nv,_tmp524);
# 3174
if(_tmp523){
struct _tuple1*_tmp4BD=Cyc_Toc_temp_var();
struct _tuple1*_tmp4BE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp527){
xexp=_tmp524;
if(_tmp526 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp526;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA2E=({struct Cyc_Absyn_Exp*_tmpA2D=rgn;struct Cyc_Absyn_Exp*_tmpA2C=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA2D,_tmpA2C,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA2E;});}else{
# 3185
({struct Cyc_Absyn_Exp*_tmpA31=({void*_tmpA30=*_tmp525;struct Cyc_Absyn_Exp*_tmpA2F=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA30,_tmpA2F,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA31;});}
# 3187
({struct Cyc_Absyn_Exp*_tmpA36=({struct Cyc_Absyn_Exp*_tmpA35=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA35,({struct Cyc_Absyn_Exp*_tmp4BF[3U];({
struct Cyc_Absyn_Exp*_tmpA34=Cyc_Absyn_var_exp(_tmp4BE,0U);_tmp4BF[0]=_tmpA34;}),({struct Cyc_Absyn_Exp*_tmpA33=Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp4BF[1]=_tmpA33;}),({
struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_var_exp(_tmp4BD,0U);_tmp4BF[2]=_tmpA32;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4BF,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3187
rexp=_tmpA36;});}else{
# 3191
if(_tmp526 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp526;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp522)
({struct Cyc_Absyn_Exp*_tmpA38=({struct Cyc_Absyn_Exp*_tmpA37=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA37,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA38;});else{
# 3197
({struct Cyc_Absyn_Exp*_tmpA3A=({struct Cyc_Absyn_Exp*_tmpA39=rgn;Cyc_Toc_rmalloc_exp(_tmpA39,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA3A;});}}else{
# 3199
({struct Cyc_Absyn_Exp*_tmpA3C=({void*_tmpA3B=*_tmp525;Cyc_Toc_malloc_exp(_tmpA3B,Cyc_Absyn_var_exp(_tmp4BD,0U));});pexp=_tmpA3C;});}
# 3201
({struct Cyc_Absyn_Exp*_tmpA41=({struct Cyc_Absyn_Exp*_tmpA40=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA40,({struct Cyc_Absyn_Exp*_tmp4C0[3U];({struct Cyc_Absyn_Exp*_tmpA3F=Cyc_Absyn_var_exp(_tmp4BE,0U);_tmp4C0[0]=_tmpA3F;}),({struct Cyc_Absyn_Exp*_tmpA3E=Cyc_Absyn_uint_exp(1U,0U);_tmp4C0[1]=_tmpA3E;}),({
struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Absyn_var_exp(_tmp4BD,0U);_tmp4C0[2]=_tmpA3D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4C0,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3201
rexp=_tmpA41;});}{
# 3204
struct Cyc_Absyn_Stmt*_tmp4C1=({struct _tuple1*_tmpA47=_tmp4BD;void*_tmpA46=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA45=_tmp524;Cyc_Absyn_declare_stmt(_tmpA47,_tmpA46,_tmpA45,({
struct _tuple1*_tmpA44=_tmp4BE;void*_tmpA43=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA42=pexp;Cyc_Absyn_declare_stmt(_tmpA44,_tmpA43,_tmpA42,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA48=Cyc_Toc_stmt_exp_r(_tmp4C1);e->r=_tmpA48;});};}else{
# 3209
if(_tmp527){
if(_tmp526 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp526;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA4B=({struct Cyc_Absyn_Exp*_tmpA4A=rgn;struct Cyc_Absyn_Exp*_tmpA49=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA4A,_tmpA49,_tmp524);})->r;e->r=_tmpA4B;});}else{
# 3215
({void*_tmpA4E=({void*_tmpA4D=*_tmp525;struct Cyc_Absyn_Exp*_tmpA4C=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA4D,_tmpA4C,_tmp524);})->r;e->r=_tmpA4E;});}}else{
# 3218
if(_tmp526 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp526;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp522)
({void*_tmpA4F=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp524))->r;e->r=_tmpA4F;});else{
# 3224
({void*_tmpA50=(Cyc_Toc_rmalloc_exp(rgn,_tmp524))->r;e->r=_tmpA50;});}}else{
# 3226
({void*_tmpA51=(Cyc_Toc_malloc_exp(*_tmp525,_tmp524))->r;e->r=_tmpA51;});}}}
# 3230
goto _LL0;}case 35U: _LL49: _tmp529=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp528=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL4A: {
# 3239
void*e1_old_typ=(void*)_check_null(_tmp529->topt);
void*e2_old_typ=(void*)_check_null(_tmp528->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4C2=0U;({struct _dyneither_ptr _tmpA52=({const char*_tmp4C3="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4C3,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA52,_tag_dyneither(_tmp4C2,sizeof(void*),0U));});});{
# 3247
unsigned int _tmp4C4=e->loc;
struct _tuple1*_tmp4C5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Absyn_var_exp(_tmp4C5,_tmp4C4);_tmp4C6->topt=e1_old_typ;{
struct _tuple1*_tmp4C7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Absyn_var_exp(_tmp4C7,_tmp4C4);_tmp4C8->topt=e2_old_typ;{
# 3253
struct Cyc_Absyn_Exp*_tmp4C9=({struct Cyc_Absyn_Exp*_tmpA54=Cyc_Tcutil_deep_copy_exp(1,_tmp529);struct Cyc_Absyn_Exp*_tmpA53=_tmp4C8;Cyc_Absyn_assign_exp(_tmpA54,_tmpA53,_tmp4C4);});_tmp4C9->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4CA=Cyc_Absyn_exp_stmt(_tmp4C9,_tmp4C4);
struct Cyc_Absyn_Exp*_tmp4CB=({struct Cyc_Absyn_Exp*_tmpA56=Cyc_Tcutil_deep_copy_exp(1,_tmp528);struct Cyc_Absyn_Exp*_tmpA55=_tmp4C6;Cyc_Absyn_assign_exp(_tmpA56,_tmpA55,_tmp4C4);});_tmp4CB->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4CC=Cyc_Absyn_exp_stmt(_tmp4CB,_tmp4C4);
# 3258
struct Cyc_Absyn_Stmt*_tmp4CD=({struct _tuple1*_tmpA5E=_tmp4C5;void*_tmpA5D=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA5C=_tmp529;struct Cyc_Absyn_Stmt*_tmpA5B=({
struct _tuple1*_tmpA5A=_tmp4C7;void*_tmpA59=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA58=_tmp528;struct Cyc_Absyn_Stmt*_tmpA57=
Cyc_Absyn_seq_stmt(_tmp4CA,_tmp4CC,_tmp4C4);
# 3259
Cyc_Absyn_declare_stmt(_tmpA5A,_tmpA59,_tmpA58,_tmpA57,_tmp4C4);});
# 3258
Cyc_Absyn_declare_stmt(_tmpA5E,_tmpA5D,_tmpA5C,_tmpA5B,_tmp4C4);});
# 3261
Cyc_Toc_stmt_to_c(nv,_tmp4CD);
({void*_tmpA5F=Cyc_Toc_stmt_exp_r(_tmp4CD);e->r=_tmpA5F;});
goto _LL0;};};};};};}case 38U: _LL4B: _tmp52B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_tmp52A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30C)->f2;_LL4C: {
# 3266
void*_tmp4CE=Cyc_Tcutil_compress((void*)_check_null(_tmp52B->topt));
Cyc_Toc_exp_to_c(nv,_tmp52B);
{void*_tmp4CF=_tmp4CE;struct Cyc_Absyn_Aggrdecl*_tmp4D5;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF)->f1).aggr_info).KnownAggr).tag == 2){_LL134: _tmp4D5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF)->f1).aggr_info).KnownAggr).val;_LL135: {
# 3270
struct Cyc_Absyn_Exp*_tmp4D0=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4D5,_tmp52A),0U);
struct Cyc_Absyn_Exp*_tmp4D1=Cyc_Toc_member_exp(_tmp52B,_tmp52A,0U);
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_Toc_member_exp(_tmp4D1,Cyc_Toc_tag_sp,0U);
({void*_tmpA60=(Cyc_Absyn_eq_exp(_tmp4D2,_tmp4D0,0U))->r;e->r=_tmpA60;});
goto _LL133;}}else{goto _LL136;}}else{_LL136: _LL137:
({void*_tmp4D3=0U;({struct _dyneither_ptr _tmpA61=({const char*_tmp4D4="non-aggregate type in tagcheck";_tag_dyneither(_tmp4D4,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA61,_tag_dyneither(_tmp4D3,sizeof(void*),0U));});});}_LL133:;}
# 3277
goto _LL0;}case 37U: _LL4D: _tmp52C=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp30C)->f1;_LL4E:
 Cyc_Toc_stmt_to_c(nv,_tmp52C);goto _LL0;case 36U: _LL4F: _LL50:
({void*_tmp4D6=0U;({struct _dyneither_ptr _tmpA62=({const char*_tmp4D7="UnresolvedMem";_tag_dyneither(_tmp4D7,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA62,_tag_dyneither(_tmp4D6,sizeof(void*),0U));});});case 25U: _LL51: _LL52:
({void*_tmp4D8=0U;({struct _dyneither_ptr _tmpA63=({const char*_tmp4D9="compoundlit";_tag_dyneither(_tmp4D9,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA63,_tag_dyneither(_tmp4D8,sizeof(void*),0U));});});case 39U: _LL53: _LL54:
({void*_tmp4DA=0U;({struct _dyneither_ptr _tmpA64=({const char*_tmp4DB="valueof(-)";_tag_dyneither(_tmp4DB,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA64,_tag_dyneither(_tmp4DA,sizeof(void*),0U));});});default: _LL55: _LL56:
({void*_tmp4DC=0U;({struct _dyneither_ptr _tmpA65=({const char*_tmp4DD="__asm__";_tag_dyneither(_tmp4DD,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA65,_tag_dyneither(_tmp4DC,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3310 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3312
return({struct _tuple35*_tmp52D=_region_malloc(r,sizeof(*_tmp52D));_tmp52D->f1=0,({struct _dyneither_ptr*_tmpA67=Cyc_Toc_fresh_label();_tmp52D->f2=_tmpA67;}),({struct _dyneither_ptr*_tmpA66=Cyc_Toc_fresh_label();_tmp52D->f3=_tmpA66;}),_tmp52D->f4=sc;_tmp52D;});}
# 3317
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp52E=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3323
if((int)(((_tmp52E->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp531=_tmp52E->orig_pat;if((_tmp531.pattern).tag != 1)_throw_match();(_tmp531.pattern).val;})->topt);
void*_tmp52F=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp530=_tmp52F;switch(*((int*)_tmp530)){case 0U: _LL1: _LL2:
# 3328
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA69=({void*_tmpA68=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA68,v);});v=_tmpA69;});goto _LL0;}_LL0:;}{
# 3334
void*_tmp532=_tmp52E->access;void*_tmp533=_tmp532;struct Cyc_Absyn_Datatypedecl*_tmp53E;struct Cyc_Absyn_Datatypefield*_tmp53D;unsigned int _tmp53C;int _tmp53B;struct _dyneither_ptr*_tmp53A;unsigned int _tmp539;switch(*((int*)_tmp533)){case 0U: _LLA: _LLB:
# 3339
 goto _LL9;case 1U: _LLC: _LLD:
# 3344
 if(ps->tl != 0){
void*_tmp534=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp535=_tmp534;struct Cyc_Absyn_Datatypedecl*_tmp538;struct Cyc_Absyn_Datatypefield*_tmp537;unsigned int _tmp536;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp535)->tag == 3U){_LL15: _tmp538=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp535)->f1;_tmp537=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp535)->f2;_tmp536=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp535)->f3;_LL16:
# 3347
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA6C=({void*_tmpA6B=({void*_tmpA6A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp537->name,_tmp538->name));Cyc_Absyn_cstar_typ(_tmpA6A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA6B,v);});v=_tmpA6C;});
({struct Cyc_Absyn_Exp*_tmpA6E=({struct Cyc_Absyn_Exp*_tmpA6D=v;Cyc_Absyn_aggrarrow_exp(_tmpA6D,Cyc_Absyn_fieldname((int)(_tmp536 + 1)),0U);});v=_tmpA6E;});
continue;}else{_LL17: _LL18:
# 3352
 goto _LL14;}_LL14:;}
# 3355
({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Absyn_deref_exp(v,0U);v=_tmpA6F;});
goto _LL9;case 2U: _LLE: _tmp539=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp533)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA71=({struct Cyc_Absyn_Exp*_tmpA70=v;Cyc_Toc_member_exp(_tmpA70,Cyc_Absyn_fieldname((int)(_tmp539 + 1)),0U);});v=_tmpA71;});goto _LL9;case 4U: _LL10: _tmp53B=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp533)->f1;_tmp53A=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp533)->f2;_LL11:
# 3359
({struct Cyc_Absyn_Exp*_tmpA72=Cyc_Toc_member_exp(v,_tmp53A,0U);v=_tmpA72;});
if(_tmp53B)
({struct Cyc_Absyn_Exp*_tmpA73=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA73;});
goto _LL9;default: _LL12: _tmp53E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp533)->f1;_tmp53D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp533)->f2;_tmp53C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp533)->f3;_LL13:
# 3364
({struct Cyc_Absyn_Exp*_tmpA75=({struct Cyc_Absyn_Exp*_tmpA74=v;Cyc_Toc_member_exp(_tmpA74,Cyc_Absyn_fieldname((int)(_tmp53C + 1)),0U);});v=_tmpA75;});
goto _LL9;}_LL9:;};}
# 3368
return v;}
# 3373
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp53F=t;struct Cyc_Absyn_Datatypedecl*_tmp558;struct Cyc_Absyn_Datatypefield*_tmp557;struct _dyneither_ptr*_tmp556;int _tmp555;int _tmp554;struct Cyc_Absyn_Datatypedecl*_tmp553;struct Cyc_Absyn_Datatypefield*_tmp552;unsigned int _tmp551;struct _dyneither_ptr _tmp550;int _tmp54F;void*_tmp54E;struct Cyc_Absyn_Enumfield*_tmp54D;struct Cyc_Absyn_Enumdecl*_tmp54C;struct Cyc_Absyn_Enumfield*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;switch(*((int*)_tmp53F)){case 0U: _LL1: _tmp54A=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp53F)->f1;_LL2:
# 3377
 if(_tmp54A == 0)return v;else{return _tmp54A;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA76=v;Cyc_Absyn_eq_exp(_tmpA76,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA77=v;Cyc_Absyn_neq_exp(_tmpA77,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp54C=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp53F)->f1;_tmp54B=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp53F)->f2;_LL8:
# 3381
 return({struct Cyc_Absyn_Exp*_tmpA78=v;Cyc_Absyn_eq_exp(_tmpA78,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->tag=32U,_tmp540->f1=_tmp54C,_tmp540->f2=_tmp54B;_tmp540;}),0U),0U);});case 4U: _LL9: _tmp54E=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp53F)->f1;_tmp54D=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp53F)->f2;_LLA:
# 3383
 return({struct Cyc_Absyn_Exp*_tmpA79=v;Cyc_Absyn_eq_exp(_tmpA79,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->tag=33U,_tmp541->f1=_tmp54E,_tmp541->f2=_tmp54D;_tmp541;}),0U),0U);});case 5U: _LLB: _tmp550=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp53F)->f1;_tmp54F=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp53F)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA7A=v;Cyc_Absyn_eq_exp(_tmpA7A,Cyc_Absyn_float_exp(_tmp550,_tmp54F,0U),0U);});case 6U: _LLD: _tmp551=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp53F)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA7B=v;Cyc_Absyn_eq_exp(_tmpA7B,Cyc_Absyn_signed_int_exp((int)_tmp551,0U),0U);});case 7U: _LLF: _tmp554=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp53F)->f1;_tmp553=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp53F)->f2;_tmp552=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp53F)->f3;_LL10:
# 3389
 LOOP1: {
void*_tmp542=v->r;void*_tmp543=_tmp542;struct Cyc_Absyn_Exp*_tmp545;struct Cyc_Absyn_Exp*_tmp544;switch(*((int*)_tmp543)){case 14U: _LL16: _tmp544=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL17:
 v=_tmp544;goto LOOP1;case 20U: _LL18: _tmp545=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL19:
 v=_tmp545;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3396
({struct Cyc_Absyn_Exp*_tmpA7E=({void*_tmpA7D=({void*_tmpA7C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp552->name,_tmp553->name));Cyc_Absyn_cstar_typ(_tmpA7C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA7D,v);});v=_tmpA7E;});
return({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA7F,Cyc_Absyn_uint_exp((unsigned int)_tmp554,0U),0U);});case 8U: _LL11: _tmp556=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp53F)->f1;_tmp555=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp53F)->f2;_LL12:
# 3401
({struct Cyc_Absyn_Exp*_tmpA81=({struct Cyc_Absyn_Exp*_tmpA80=Cyc_Toc_member_exp(v,_tmp556,0U);Cyc_Toc_member_exp(_tmpA80,Cyc_Toc_tag_sp,0U);});v=_tmpA81;});
return({struct Cyc_Absyn_Exp*_tmpA82=v;Cyc_Absyn_eq_exp(_tmpA82,Cyc_Absyn_signed_int_exp(_tmp555,0U),0U);});default: _LL13: _tmp558=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp53F)->f1;_tmp557=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp53F)->f2;_LL14:
# 3405
 LOOP2: {
void*_tmp546=v->r;void*_tmp547=_tmp546;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp548;switch(*((int*)_tmp547)){case 14U: _LL1D: _tmp548=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp547)->f2;_LL1E:
 v=_tmp548;goto LOOP2;case 20U: _LL1F: _tmp549=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp547)->f1;_LL20:
 v=_tmp549;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3412
({struct Cyc_Absyn_Exp*_tmpA85=({void*_tmpA84=({void*_tmpA83=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp557->name,_tmp558->name));Cyc_Absyn_cstar_typ(_tmpA83,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA84,v);});v=_tmpA85;});
return({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA86,Cyc_Absyn_var_exp(_tmp557->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3426
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3434
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3436
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp559=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp55A=_tmp559;void*_tmp560;_LL1: _tmp560=_tmp55A.f1;_LL2:;{
void*_tmp55B=_tmp560;struct Cyc_Absyn_Exp*_tmp55F;struct _dyneither_ptr*_tmp55E;switch(*((int*)_tmp55B)){case 3U: _LL4: _LL5:
# 3442
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3445
 continue;case 8U: _LLA: _tmp55E=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp55B)->f1;_LLB:
# 3447
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA87=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->tag=3U,_tmp55C->f1=_tmp55E;_tmp55C;});k=_tmpA87;});
continue;case 7U: _LLC: _LLD:
# 3451
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp55F=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp55B)->f1;if(_tmp55F != 0){_LLF:
# 3455
({void*_tmpA88=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=2U,_tmp55D->f1=_tmp55F;_tmp55D;});k=_tmpA88;});
return({struct _tuple36 _tmp7A3;_tmp7A3.f1=0,_tmp7A3.f2=k;_tmp7A3;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp7A4;_tmp7A4.f1=0,_tmp7A4.f2=k;_tmp7A4;});}_LL3:;};}
# 3464
return({struct _tuple36 _tmp7A5;_tmp7A5.f1=c,_tmp7A5.f2=k;_tmp7A5;});}
# 3469
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp561=p;int _tmp56C;int _tmp56B;unsigned int _tmp56A;void*_tmp569;struct Cyc_Absyn_Enumfield*_tmp568;struct Cyc_Absyn_Enumdecl*_tmp567;struct Cyc_Absyn_Enumfield*_tmp566;switch(*((int*)_tmp561)){case 3U: _LL1: _tmp567=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp561)->f1;_tmp566=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp561)->f2;_LL2:
# 3473
({struct Cyc_Absyn_Exp*_tmpA89=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=32U,_tmp562->f1=_tmp567,_tmp562->f2=_tmp566;_tmp562;}),0U);e=_tmpA89;});goto _LL0;case 4U: _LL3: _tmp569=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp561)->f1;_tmp568=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp561)->f2;_LL4:
# 3475
({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=33U,_tmp563->f1=_tmp569,_tmp563->f2=_tmp568;_tmp563;}),0U);e=_tmpA8A;});goto _LL0;case 6U: _LL5: _tmp56A=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp561)->f1;_LL6:
 _tmp56B=(int)_tmp56A;goto _LL8;case 7U: _LL7: _tmp56B=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp561)->f1;_LL8:
 _tmp56C=_tmp56B;goto _LLA;case 8U: _LL9: _tmp56C=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp561)->f2;_LLA:
# 3479
({struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Absyn_uint_exp((unsigned int)_tmp56C,0U);e=_tmpA8B;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp564=0U;({struct _dyneither_ptr _tmpA8C=({const char*_tmp565="compile_pat_test_as_case!";_tag_dyneither(_tmp565,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA8C,_tag_dyneither(_tmp564,sizeof(void*),0U));});});}_LL0:;}
# 3483
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->tag=17U,_tmp56D->f1=e;_tmp56D;}),0U);}
# 3487
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3496
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3499
void*t=(void*)_check_null(p->topt);
void*_tmp56E=p->r;void*_tmp56F=_tmp56E;union Cyc_Absyn_AggrInfoU _tmp5A0;struct Cyc_List_List*_tmp59F;struct Cyc_List_List*_tmp59E;struct Cyc_List_List*_tmp59D;struct Cyc_Absyn_Pat*_tmp59C;struct Cyc_Absyn_Datatypedecl*_tmp59B;struct Cyc_Absyn_Datatypefield*_tmp59A;struct Cyc_List_List*_tmp599;struct Cyc_Absyn_Vardecl*_tmp598;struct Cyc_Absyn_Pat*_tmp597;struct Cyc_Absyn_Vardecl*_tmp596;struct Cyc_Absyn_Vardecl*_tmp595;struct Cyc_Absyn_Pat*_tmp594;struct Cyc_Absyn_Vardecl*_tmp593;switch(*((int*)_tmp56F)){case 2U: _LL1: _tmp593=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp56F)->f2;_LL2: {
# 3502
struct Cyc_Absyn_Pat*_tmp570=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp570->topt=p->topt;
_tmp595=_tmp593;_tmp594=_tmp570;goto _LL4;}case 1U: _LL3: _tmp595=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp56F)->f1;_tmp594=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp56F)->f2;_LL4:
# 3507
({struct Cyc_List_List*_tmpA8D=({struct Cyc_List_List*_tmp571=_region_malloc(rgn,sizeof(*_tmp571));_tmp571->hd=_tmp595,_tmp571->tl=*decls;_tmp571;});*decls=_tmpA8D;});{
struct Cyc_Absyn_Stmt*_tmp572=({struct Cyc_Absyn_Exp*_tmpA8E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=4U,_tmp573->f1=_tmp595;_tmp573;}),0U);Cyc_Absyn_assign_stmt(_tmpA8E,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA8F=_tmp572;Cyc_Toc_seq_stmt_opt(_tmpA8F,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp594));});};case 4U: _LL5: _tmp596=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp56F)->f2;_LL6:
# 3512
({struct Cyc_List_List*_tmpA90=({struct Cyc_List_List*_tmp574=_region_malloc(rgn,sizeof(*_tmp574));_tmp574->hd=_tmp596,_tmp574->tl=*decls;_tmp574;});*decls=_tmpA90;});
return({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->tag=4U,_tmp575->f1=_tmp596;_tmp575;}),0U);Cyc_Absyn_assign_stmt(_tmpA91,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp598=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp56F)->f1;_tmp597=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp56F)->f2;_LLA:
# 3517
({struct Cyc_List_List*_tmpA92=({struct Cyc_List_List*_tmp576=_region_malloc(rgn,sizeof(*_tmp576));_tmp576->hd=_tmp598,_tmp576->tl=*decls;_tmp576;});*decls=_tmpA92;});
({void*_tmpA93=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp598->type=_tmpA93;});{
# 3520
struct Cyc_Absyn_Stmt*_tmp577=({struct Cyc_Absyn_Exp*_tmpA96=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->tag=4U,_tmp578->f1=_tmp598;_tmp578;}),0U);Cyc_Absyn_assign_stmt(_tmpA96,({
void*_tmpA95=({void*_tmpA94=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpA94,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA95,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA97=_tmp577;Cyc_Toc_seq_stmt_opt(_tmpA97,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp597));});};case 9U: _LLB: _LLC:
# 3525
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56F)->f1)->r)->tag == 8U){_LL17: _tmp59B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56F)->f1)->r)->f1;_tmp59A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56F)->f1)->r)->f2;_tmp599=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56F)->f1)->r)->f3;_LL18:
# 3534
 if(_tmp599 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp59A->name,_tmp59B->name);
void*_tmp579=({void*_tmpA98=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpA98,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA99=Cyc_Toc_cast_it(_tmp579,path);path=_tmpA99;});{
int cnt=1;
struct Cyc_List_List*_tmp57A=_tmp59A->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp599 != 0;(_tmp599=_tmp599->tl,_tmp57A=((struct Cyc_List_List*)_check_null(_tmp57A))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp57B=(struct Cyc_Absyn_Pat*)_tmp599->hd;
if(_tmp57B->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp57C=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp57A))->hd)).f2;
void*_tmp57D=(void*)_check_null(_tmp57B->topt);
void*_tmp57E=Cyc_Toc_typ_to_c_array(_tmp57D);
struct Cyc_Absyn_Exp*_tmp57F=({struct Cyc_Absyn_Exp*_tmpA9A=path;Cyc_Absyn_aggrarrow_exp(_tmpA9A,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp57C)))
({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Toc_cast_it(_tmp57E,_tmp57F);_tmp57F=_tmpA9B;});
({struct Cyc_Absyn_Stmt*_tmpA9D=({struct Cyc_Absyn_Stmt*_tmpA9C=s;Cyc_Toc_seq_stmt_opt(_tmpA9C,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp57F,_tmp57B));});s=_tmpA9D;});};}
# 3552
return s;};};}else{_LL21: _tmp59C=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp56F)->f1;_LL22:
# 3597
 return({struct _RegionHandle*_tmpAA1=rgn;struct Cyc_Toc_Env**_tmpAA0=nv;struct Cyc_List_List**_tmpA9F=decls;struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpAA1,_tmpAA0,_tmpA9F,_tmpA9E,_tmp59C);});}case 8U: _LL19: _tmp59D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp56F)->f3;_LL1A:
# 3554
 _tmp59E=_tmp59D;goto _LL1C;case 5U: _LL1B: _tmp59E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp56F)->f1;_LL1C: {
# 3556
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp59E != 0;(_tmp59E=_tmp59E->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp580=(struct Cyc_Absyn_Pat*)_tmp59E->hd;
if(_tmp580->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp581=(void*)_check_null(_tmp580->topt);
struct _dyneither_ptr*_tmp582=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpAA7=({struct Cyc_Absyn_Stmt*_tmpAA6=s;Cyc_Toc_seq_stmt_opt(_tmpAA6,({struct _RegionHandle*_tmpAA5=rgn;struct Cyc_Toc_Env**_tmpAA4=nv;struct Cyc_List_List**_tmpAA3=decls;struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Toc_member_exp(path,_tmp582,0U);Cyc_Toc_extract_pattern_vars(_tmpAA5,_tmpAA4,_tmpAA3,_tmpAA2,_tmp580);}));});s=_tmpAA7;});};}
# 3566
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp56F)->f1 == 0){_LL1D: _LL1E:
({void*_tmp583=0U;({struct _dyneither_ptr _tmpAA8=({const char*_tmp584="unresolved aggregate pattern!";_tag_dyneither(_tmp584,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA8,_tag_dyneither(_tmp583,sizeof(void*),0U));});});}else{_LL1F: _tmp5A0=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp56F)->f1)->aggr_info;_tmp59F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp56F)->f3;_LL20: {
# 3570
struct Cyc_Absyn_Aggrdecl*_tmp585=Cyc_Absyn_get_known_aggrdecl(_tmp5A0);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp59F != 0;_tmp59F=_tmp59F->tl){
struct _tuple37*_tmp586=(struct _tuple37*)_tmp59F->hd;
struct Cyc_Absyn_Pat*_tmp587=(*_tmp586).f2;
if(_tmp587->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp588=Cyc_Absyn_designatorlist_to_fieldname((*_tmp586).f1);
void*_tmp589=(void*)_check_null(_tmp587->topt);
void*_tmp58A=Cyc_Toc_typ_to_c_array(_tmp589);
struct Cyc_Absyn_Exp*_tmp58B=Cyc_Toc_member_exp(path,_tmp588,0U);
# 3582
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp585->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Toc_member_exp(_tmp58B,Cyc_Toc_val_sp,0U);_tmp58B=_tmpAA9;});{
void*_tmp58C=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp585->impl))->fields,_tmp588)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp58C))
({struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Toc_cast_it(_tmp58A,_tmp58B);_tmp58B=_tmpAAA;});else{
if(!Cyc_Toc_is_array_type(_tmp58C) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp58C)))
# 3589
({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_Absyn_deref_exp(({void*_tmpAAB=Cyc_Absyn_cstar_typ(_tmp58A,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAAB,
Cyc_Absyn_address_exp(_tmp58B,0U));}),0U);
# 3589
_tmp58B=_tmpAAC;});}
# 3592
({struct Cyc_Absyn_Stmt*_tmpAAE=({struct Cyc_Absyn_Stmt*_tmpAAD=s;Cyc_Toc_seq_stmt_opt(_tmpAAD,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp58B,_tmp587));});s=_tmpAAE;});};};}
# 3594
return s;}}case 15U: _LL23: _LL24:
# 3599
({void*_tmp58D=0U;({struct _dyneither_ptr _tmpAAF=({const char*_tmp58E="unknownid pat";_tag_dyneither(_tmp58E,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAF,_tag_dyneither(_tmp58D,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp58F=0U;({struct _dyneither_ptr _tmpAB0=({const char*_tmp590="unknowncall pat";_tag_dyneither(_tmp590,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB0,_tag_dyneither(_tmp58F,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp591=0U;({struct _dyneither_ptr _tmpAB1=({const char*_tmp592="exp pat";_tag_dyneither(_tmp592,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB1,_tag_dyneither(_tmp591,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3610
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3618
void*_tmp5A1=dopt;struct Cyc_List_List*_tmp5DF;struct Cyc_List_List*_tmp5DE;void*_tmp5DD;struct Cyc_Tcpat_Rhs*_tmp5DC;if(_tmp5A1 == 0){_LL1: _LL2:
# 3620
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp5A1)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5DC=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp5A1)->f1;_LL6:
# 3625
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5A2=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5A3=_tmp5A2;int*_tmp5AA;struct _dyneither_ptr*_tmp5A9;struct _dyneither_ptr*_tmp5A8;struct Cyc_Absyn_Switch_clause*_tmp5A7;_LLA: _tmp5AA=(int*)& _tmp5A3->f1;_tmp5A9=_tmp5A3->f2;_tmp5A8=_tmp5A3->f3;_tmp5A7=_tmp5A3->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp5A4=_tmp5A7->body;
if(_tmp5A4 == _tmp5DC->rhs){
# 3630
if(*_tmp5AA)
return Cyc_Absyn_goto_stmt(_tmp5A9,0U);
*_tmp5AA=1;{
# 3634
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAB4=rgn;struct Cyc_List_List**_tmpAB3=decls;struct Cyc_Absyn_Exp*_tmpAB2=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAB4,& nv,_tmpAB3,_tmpAB2,_tmp5A7->pattern);});
# 3637
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp5A8,_tmp5A7->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAB5=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpAB5;});
({struct Cyc_Absyn_Stmt*_tmpAB6=Cyc_Absyn_label_stmt(_tmp5A9,res,0U);res=_tmpAB6;});
({struct Cyc_List_List*_tmpAB8=({struct Cyc_List_List*_tmp5A6=_region_malloc(rgn,sizeof(*_tmp5A6));({struct _tuple38*_tmpAB7=({struct _tuple38*_tmp5A5=_region_malloc(rgn,sizeof(*_tmp5A5));_tmp5A5->f1=nv,_tmp5A5->f2=_tmp5A8,_tmp5A5->f3=_tmp5A4;_tmp5A5;});_tmp5A6->hd=_tmpAB7;}),_tmp5A6->tl=*bodies;_tmp5A6;});*bodies=_tmpAB8;});
return res;};}};}
# 3645
({void*_tmp5AB=0U;({struct _dyneither_ptr _tmpAB9=({const char*_tmp5AC="couldn't find rhs!";_tag_dyneither(_tmp5AC,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAB9,_tag_dyneither(_tmp5AB,sizeof(void*),0U));});});default: _LL7: _tmp5DF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A1)->f1;_tmp5DE=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A1)->f2;_tmp5DD=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp5A1)->f3;_LL8: {
# 3648
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5DD,lscs,v);
# 3650
struct Cyc_Absyn_Exp*_tmp5AD=({struct Cyc_List_List*_tmpABA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5DF);Cyc_Toc_compile_path(_tmpABA,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp5AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5DE);
# 3653
struct _tuple36 _tmp5AF=Cyc_Toc_admits_switch(_tmp5AE);struct _tuple36 _tmp5B0=_tmp5AF;int _tmp5DB;void*_tmp5DA;_LLD: _tmp5DB=_tmp5B0.f1;_tmp5DA=_tmp5B0.f2;_LLE:;
if(_tmp5DB > 1){
# 3657
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));
({struct Cyc_Absyn_Switch_clause*_tmpABC=({struct Cyc_Absyn_Switch_clause*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));({struct Cyc_Absyn_Pat*_tmpABB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5C0->pattern=_tmpABB;}),_tmp5C0->pat_vars=0,_tmp5C0->where_clause=0,_tmp5C0->body=res,_tmp5C0->loc=0U;_tmp5C0;});_tmp5C1->hd=_tmpABC;}),_tmp5C1->tl=0;_tmp5C1;});
# 3660
for(0;_tmp5AE != 0;_tmp5AE=_tmp5AE->tl){
struct _tuple33 _tmp5B1=*((struct _tuple33*)_tmp5AE->hd);struct _tuple33 _tmp5B2=_tmp5B1;void*_tmp5B8;void*_tmp5B7;_LL10: _tmp5B8=_tmp5B2.f1;_tmp5B7=_tmp5B2.f2;_LL11:;{
# 3663
struct Cyc_Absyn_Pat*_tmp5B3=Cyc_Toc_compile_pat_test_as_case(_tmp5B8);
# 3665
struct Cyc_Absyn_Stmt*_tmp5B4=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5B7,lscs,v);
# 3667
({struct Cyc_List_List*_tmpABE=({struct Cyc_List_List*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));({struct Cyc_Absyn_Switch_clause*_tmpABD=({struct Cyc_Absyn_Switch_clause*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->pattern=_tmp5B3,_tmp5B5->pat_vars=0,_tmp5B5->where_clause=0,_tmp5B5->body=_tmp5B4,_tmp5B5->loc=0U;_tmp5B5;});_tmp5B6->hd=_tmpABD;}),_tmp5B6->tl=new_lscs;_tmp5B6;});new_lscs=_tmpABE;});};}
# 3669
{void*_tmp5B9=_tmp5DA;struct _dyneither_ptr*_tmp5BE;switch(*((int*)_tmp5B9)){case 1U: _LL13: _LL14:
# 3672
 LOOP1: {
void*_tmp5BA=_tmp5AD->r;void*_tmp5BB=_tmp5BA;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BC;switch(*((int*)_tmp5BB)){case 14U: _LL1C: _tmp5BC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5BB)->f2;_LL1D:
 _tmp5AD=_tmp5BC;goto LOOP1;case 20U: _LL1E: _tmp5BD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5BB)->f1;_LL1F:
 _tmp5AD=_tmp5BD;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3678
({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Absyn_deref_exp(({void*_tmpABF=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpABF,_tmp5AD);}),0U);_tmp5AD=_tmpAC0;});goto _LL12;case 3U: _LL15: _tmp5BE=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5B9)->f1;_LL16:
# 3681
({struct Cyc_Absyn_Exp*_tmpAC2=({struct Cyc_Absyn_Exp*_tmpAC1=Cyc_Toc_member_exp(_tmp5AD,_tmp5BE,0U);Cyc_Toc_member_exp(_tmpAC1,Cyc_Toc_tag_sp,0U);});_tmp5AD=_tmpAC2;});goto _LL12;case 2U: _LL17: _LL18:
# 3683
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3686
({struct Cyc_Absyn_Stmt*_tmpAC3=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->tag=10U,_tmp5BF->f1=_tmp5AD,_tmp5BF->f2=new_lscs,_tmp5BF->f3=0;_tmp5BF;}),0U);res=_tmpAC3;});}else{
# 3690
void*_tmp5C2=_tmp5DA;struct Cyc_Absyn_Exp*_tmp5D9;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C2)->tag == 2U){_LL23: _tmp5D9=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5C2)->f1;_LL24: {
# 3692
struct Cyc_List_List*_tmp5C3=_tmp5AE;void*_tmp5D2;struct Cyc_Tcpat_Rhs*_tmp5D1;if(_tmp5C3 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5C3)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5C3)->tl == 0){_LL28: _tmp5D2=((struct _tuple33*)_tmp5C3->hd)->f1;_tmp5D1=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5C3->hd)->f2)->f1;_LL29:
# 3698
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5C4=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5C5=_tmp5C4;int*_tmp5CC;struct _dyneither_ptr*_tmp5CB;struct _dyneither_ptr*_tmp5CA;struct Cyc_Absyn_Switch_clause*_tmp5C9;_LL2D: _tmp5CC=(int*)& _tmp5C5->f1;_tmp5CB=_tmp5C5->f2;_tmp5CA=_tmp5C5->f3;_tmp5C9=_tmp5C5->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5C6=_tmp5C9->body;
if(_tmp5C6 == _tmp5D1->rhs){
# 3703
if(*_tmp5CC)
return Cyc_Absyn_goto_stmt(_tmp5CB,0U);
*_tmp5CC=1;{
# 3707
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAC6=rgn;struct Cyc_List_List**_tmpAC5=decls;struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAC6,& nv,_tmpAC5,_tmpAC4,_tmp5C9->pattern);});
# 3710
Cyc_Toc_exp_to_c(nv,_tmp5D9);{
# 3713
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5CA,_tmp5C9->body,0U);
({struct Cyc_Absyn_Stmt*_tmpAC7=Cyc_Absyn_ifthenelse_stmt(_tmp5D9,r,res,0U);r=_tmpAC7;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAC8=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpAC8;});
({struct Cyc_Absyn_Stmt*_tmpAC9=Cyc_Absyn_label_stmt(_tmp5CB,r,0U);r=_tmpAC9;});
({struct Cyc_List_List*_tmpACB=({struct Cyc_List_List*_tmp5C8=_region_malloc(rgn,sizeof(*_tmp5C8));({struct _tuple38*_tmpACA=({struct _tuple38*_tmp5C7=_region_malloc(rgn,sizeof(*_tmp5C7));_tmp5C7->f1=nv,_tmp5C7->f2=_tmp5CA,_tmp5C7->f3=_tmp5C6;_tmp5C7;});_tmp5C8->hd=_tmpACA;}),_tmp5C8->tl=*bodies;_tmp5C8;});*bodies=_tmpACB;});
return r;};};}};}
# 3722
({void*_tmp5CD=0U;({struct _dyneither_ptr _tmpACC=({const char*_tmp5CE="couldn't find rhs!";_tag_dyneither(_tmp5CE,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACC,_tag_dyneither(_tmp5CD,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5CF=0U;({struct _dyneither_ptr _tmpACD=({const char*_tmp5D0="bad where clause in match compiler";_tag_dyneither(_tmp5D0,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACD,_tag_dyneither(_tmp5CF,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3727
 for(0;_tmp5AE != 0;_tmp5AE=_tmp5AE->tl){
struct _tuple33 _tmp5D3=*((struct _tuple33*)_tmp5AE->hd);struct _tuple33 _tmp5D4=_tmp5D3;void*_tmp5D8;void*_tmp5D7;_LL30: _tmp5D8=_tmp5D4.f1;_tmp5D7=_tmp5D4.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5D5=Cyc_Toc_compile_pat_test(_tmp5AD,_tmp5D8);
struct Cyc_Absyn_Stmt*_tmp5D6=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5D7,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpACE=Cyc_Absyn_ifthenelse_stmt(_tmp5D5,_tmp5D6,res,0U);res=_tmpACE;});};}}_LL22:;}
# 3735
return res;}}}_LL0:;}
# 3745
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3747
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp5E0=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp5E1=_tmp5E0;struct Cyc_Toc_Env**_tmp5E3;struct Cyc_Absyn_Stmt*_tmp5E2;_LL1: _tmp5E3=(struct Cyc_Toc_Env**)& _tmp5E1->f1;_tmp5E2=_tmp5E1->f3;_LL2:;
if(_tmp5E2 == s)return _tmp5E3;}
# 3753
return 0;}
# 3757
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3760
void*_tmp5E4=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3763
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5E5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5E5;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5E6=Cyc_Toc_share_env(rgn,nv);
# 3770
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3775
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5E6,& mydecls,& mybodies,dopt,lscs,v);
# 3783
{struct Cyc_List_List*_tmp5E7=lscs;for(0;_tmp5E7 != 0;_tmp5E7=_tmp5E7->tl){
struct _tuple35*_tmp5E8=(struct _tuple35*)_tmp5E7->hd;struct _tuple35*_tmp5E9=_tmp5E8;struct _dyneither_ptr*_tmp5F1;struct Cyc_Absyn_Switch_clause*_tmp5F0;_LL1: _tmp5F1=_tmp5E9->f3;_tmp5F0=_tmp5E9->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5F0->body;
# 3787
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3790
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5EA=*envp;
# 3793
if(_tmp5E7->tl != 0){
struct _tuple35*_tmp5EB=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp5E7->tl))->hd;struct _tuple35*_tmp5EC=_tmp5EB;struct _dyneither_ptr*_tmp5EF;struct Cyc_Absyn_Switch_clause*_tmp5EE;_LL4: _tmp5EF=_tmp5EC->f3;_tmp5EE=_tmp5EC->f4;_LL5:;{
# 3797
struct Cyc_Toc_Env**_tmp5ED=Cyc_Toc_find_case_env(mybodies,_tmp5EE->body);
# 3802
if(_tmp5ED == 0)
({struct Cyc_Toc_Env*_tmpACF=Cyc_Toc_last_switchclause_env(rgn,_tmp5EA,end_l);Cyc_Toc_stmt_to_c(_tmpACF,s);});else{
# 3806
struct Cyc_List_List*vs=0;
if(_tmp5EE->pat_vars != 0){
({struct Cyc_List_List*_tmpAD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5EE->pat_vars))->v)).f1);vs=_tmpAD0;});
({struct Cyc_List_List*_tmpAD1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpAD1;});}
# 3811
({struct Cyc_Toc_Env*_tmpAD2=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5EA,end_l,_tmp5EF,vs);Cyc_Toc_stmt_to_c(_tmpAD2,s);});}};}else{
# 3815
({struct Cyc_Toc_Env*_tmpAD3=Cyc_Toc_last_switchclause_env(rgn,_tmp5EA,end_l);Cyc_Toc_stmt_to_c(_tmpAD3,s);});}};};}}{
# 3819
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpAD5=test_tree;Cyc_Absyn_seq_stmt(_tmpAD5,({struct _dyneither_ptr*_tmpAD4=end_l;Cyc_Absyn_label_stmt(_tmpAD4,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3821
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5F2=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpAD6=Cyc_Toc_temp_var();_tmp5F2->name=_tmpAD6;});
({void*_tmpAD7=Cyc_Toc_typ_to_c_array(_tmp5F2->type);_tmp5F2->type=_tmpAD7;});
({struct Cyc_Absyn_Stmt*_tmpAD9=({struct Cyc_Absyn_Decl*_tmpAD8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->tag=0U,_tmp5F3->f1=_tmp5F2;_tmp5F3;}),0U);Cyc_Absyn_decl_stmt(_tmpAD8,res,0U);});res=_tmpAD9;});}
# 3828
({void*_tmpADD=({struct _tuple1*_tmpADC=v;void*_tmpADB=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpADA=e;Cyc_Absyn_declare_stmt(_tmpADC,_tmpADB,_tmpADA,res,0U);})->r;whole_s->r=_tmpADD;});
_npop_handler(0U);return;};}
# 3767
;_pop_region(rgn);};}
# 3834
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3836
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3841
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3843
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpADF=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpADF,({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));
({struct Cyc_Absyn_Exp*_tmpADE=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp5F4->hd=_tmpADE;}),_tmp5F4->tl=0;_tmp5F4;}),0U);}),0U);}
# 3847
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpAE1=({struct Cyc_Absyn_Stmt*_tmpAE0=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpAE0,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpAE1;});}
# 3852
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3854
while(1){
void*_tmp5F5=s->r;void*_tmp5F6=_tmp5F5;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_List_List*_tmp64C;void*_tmp64B;struct Cyc_Absyn_Stmt*_tmp64A;struct Cyc_Absyn_Exp*_tmp649;struct _dyneither_ptr*_tmp648;struct Cyc_Absyn_Stmt*_tmp647;struct Cyc_Absyn_Decl*_tmp646;struct Cyc_Absyn_Stmt*_tmp645;struct Cyc_List_List*_tmp644;struct Cyc_Absyn_Switch_clause**_tmp643;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_List_List*_tmp641;void*_tmp640;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Stmt*_tmp63C;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Stmt*_tmp638;struct Cyc_Absyn_Stmt*_tmp637;struct Cyc_Absyn_Exp*_tmp636;struct Cyc_Absyn_Stmt*_tmp635;struct Cyc_Absyn_Stmt*_tmp634;struct Cyc_Absyn_Exp*_tmp633;switch(*((int*)_tmp5F6)){case 0U: _LL1: _LL2:
# 3857
 return;case 1U: _LL3: _tmp633=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_LL4:
# 3859
 Cyc_Toc_exp_to_c(nv,_tmp633);
return;case 2U: _LL5: _tmp635=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp634=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_LL6:
# 3862
 Cyc_Toc_stmt_to_c(nv,_tmp635);
s=_tmp634;
continue;case 3U: _LL7: _tmp636=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_LL8: {
# 3866
void*topt=0;
if(_tmp636 != 0){
({void*_tmpAE2=Cyc_Toc_typ_to_c((void*)_check_null(_tmp636->topt));topt=_tmpAE2;});
Cyc_Toc_exp_to_c(nv,_tmp636);}{
# 3872
int _tmp5F7=Cyc_Toc_get_npop(s);
if(_tmp5F7 > 0){
if(topt != 0){
struct _tuple1*_tmp5F8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5F9=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5F8,0U),0U);
({void*_tmpAE7=({struct _tuple1*_tmpAE6=_tmp5F8;void*_tmpAE5=topt;struct Cyc_Absyn_Exp*_tmpAE4=_tmp636;Cyc_Absyn_declare_stmt(_tmpAE6,_tmpAE5,_tmpAE4,({
struct Cyc_Absyn_Stmt*_tmpAE3=Cyc_Toc_make_npop_handler(_tmp5F7);Cyc_Absyn_seq_stmt(_tmpAE3,_tmp5F9,0U);}),0U);})->r;
# 3877
s->r=_tmpAE7;});}else{
# 3881
Cyc_Toc_do_npop_before(_tmp5F7,s);}}
# 3883
return;};}case 4U: _LL9: _tmp639=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp638=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_tmp637=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f3;_LLA:
# 3885
 Cyc_Toc_exp_to_c(nv,_tmp639);
Cyc_Toc_stmt_to_c(nv,_tmp638);
s=_tmp637;
continue;case 5U: _LLB: _tmp63B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1).f1;_tmp63A=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_LLC:
# 3890
 Cyc_Toc_exp_to_c(nv,_tmp63B);{
struct _RegionHandle _tmp5FA=_new_region("temp");struct _RegionHandle*temp=& _tmp5FA;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAE8=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAE8,_tmp63A);});
# 3894
_npop_handler(0U);return;
# 3891
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3896
struct Cyc_Toc_Env*_tmp5FB=nv;struct _dyneither_ptr**_tmp5FC;_LL22: _tmp5FC=_tmp5FB->break_lab;_LL23:;
if(_tmp5FC != 0)
({void*_tmpAE9=Cyc_Toc_goto_stmt_r(*_tmp5FC);s->r=_tmpAE9;});
# 3900
({int _tmpAEA=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEA,s);});
return;}case 7U: _LLF: _LL10: {
# 3903
struct Cyc_Toc_Env*_tmp5FD=nv;struct _dyneither_ptr**_tmp5FE;_LL25: _tmp5FE=_tmp5FD->continue_lab;_LL26:;
if(_tmp5FE != 0)
({void*_tmpAEB=Cyc_Toc_goto_stmt_r(*_tmp5FE);s->r=_tmpAEB;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3909
({int _tmpAEC=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEC,s);});
return;case 9U: _LL13: _tmp63F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp63E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2).f1;_tmp63D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f3).f1;_tmp63C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f4;_LL14:
# 3913
 Cyc_Toc_exp_to_c(nv,_tmp63F);Cyc_Toc_exp_to_c(nv,_tmp63E);Cyc_Toc_exp_to_c(nv,_tmp63D);{
struct _RegionHandle _tmp5FF=_new_region("temp");struct _RegionHandle*temp=& _tmp5FF;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAED=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAED,_tmp63C);});
# 3917
_npop_handler(0U);return;
# 3914
;_pop_region(temp);};case 10U: _LL15: _tmp642=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp641=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_tmp640=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f3;_LL16:
# 3919
 Cyc_Toc_xlate_switch(nv,s,_tmp642,_tmp641,_tmp640);
return;case 11U: _LL17: _tmp644=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp643=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_LL18: {
# 3922
struct Cyc_Toc_Env*_tmp600=nv;struct Cyc_Toc_FallthruInfo*_tmp60A;_LL28: _tmp60A=_tmp600->fallthru_info;_LL29:;
if(_tmp60A == 0)
({void*_tmp601=0U;({struct _dyneither_ptr _tmpAEE=({const char*_tmp602="fallthru in unexpected place";_tag_dyneither(_tmp602,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAEE,_tag_dyneither(_tmp601,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp603=*_tmp60A;struct Cyc_Toc_FallthruInfo _tmp604=_tmp603;struct _dyneither_ptr*_tmp609;struct Cyc_List_List*_tmp608;_LL2B: _tmp609=_tmp604.label;_tmp608=_tmp604.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp609,0U);
# 3928
({int _tmpAEF=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAEF,s2);});{
struct Cyc_List_List*_tmp605=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp608);
struct Cyc_List_List*_tmp606=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp644);
for(0;_tmp605 != 0;(_tmp605=_tmp605->tl,_tmp606=_tmp606->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp606))->hd);
({struct Cyc_Absyn_Stmt*_tmpAF2=({struct Cyc_Absyn_Stmt*_tmpAF1=({struct Cyc_Absyn_Exp*_tmpAF0=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->tag=5U,_tmp607->f1=(struct Cyc_Absyn_Vardecl*)_tmp605->hd;_tmp607;}),0U);Cyc_Absyn_assign_stmt(_tmpAF0,(struct Cyc_Absyn_Exp*)_tmp606->hd,0U);});Cyc_Absyn_seq_stmt(_tmpAF1,s2,0U);});s2=_tmpAF2;});}
# 3936
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp646=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp645=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_LL1A:
# 3942
{void*_tmp60B=_tmp646->r;void*_tmp60C=_tmp60B;struct Cyc_Absyn_Tvar*_tmp627;struct Cyc_Absyn_Vardecl*_tmp626;struct Cyc_Absyn_Exp*_tmp625;struct Cyc_Absyn_Fndecl*_tmp624;struct Cyc_List_List*_tmp623;struct Cyc_Absyn_Pat*_tmp622;struct Cyc_Absyn_Exp*_tmp621;void*_tmp620;struct Cyc_Absyn_Vardecl*_tmp61F;switch(*((int*)_tmp60C)){case 0U: _LL2E: _tmp61F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp60C)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp61F,_tmp645);goto _LL2D;case 2U: _LL30: _tmp622=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60C)->f1;_tmp621=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60C)->f3;_tmp620=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp60C)->f4;_LL31:
# 3950
{void*_tmp60D=_tmp622->r;void*_tmp60E=_tmp60D;struct Cyc_Absyn_Vardecl*_tmp610;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60E)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60E)->f2)->r)->tag == 0U){_LL3B: _tmp610=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp60E)->f1;_LL3C:
# 3952
({struct _tuple1*_tmpAF3=Cyc_Toc_temp_var();_tmp610->name=_tmpAF3;});
_tmp610->initializer=_tmp621;
({void*_tmpAF4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->tag=0U,_tmp60F->f1=_tmp610;_tmp60F;});_tmp646->r=_tmpAF4;});
Cyc_Toc_local_decl_to_c(nv,_tmp610,_tmp645);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3961
({void*_tmpAF5=(Cyc_Toc_letdecl_to_c(nv,_tmp622,_tmp620,(void*)_check_null(_tmp621->topt),_tmp621,_tmp645))->r;s->r=_tmpAF5;});
goto _LL3A;}_LL3A:;}
# 3964
goto _LL2D;case 3U: _LL32: _tmp623=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp60C)->f1;_LL33: {
# 3968
struct Cyc_List_List*_tmp611=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp623);
if(_tmp611 == 0)
({void*_tmp612=0U;({struct _dyneither_ptr _tmpAF6=({const char*_tmp613="empty Letv_d";_tag_dyneither(_tmp613,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF6,_tag_dyneither(_tmp612,sizeof(void*),0U));});});
({void*_tmpAF7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->tag=0U,_tmp614->f1=(struct Cyc_Absyn_Vardecl*)_tmp611->hd;_tmp614;});_tmp646->r=_tmpAF7;});
_tmp611=_tmp611->tl;
for(0;_tmp611 != 0;_tmp611=_tmp611->tl){
struct Cyc_Absyn_Decl*_tmp615=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=0U,_tmp616->f1=(struct Cyc_Absyn_Vardecl*)_tmp611->hd;_tmp616;}),0U);
({void*_tmpAF9=({struct Cyc_Absyn_Decl*_tmpAF8=_tmp615;Cyc_Absyn_decl_stmt(_tmpAF8,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpAF9;});}
# 3977
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp624=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp60C)->f1;_LL35:
# 3980
 Cyc_Toc_fndecl_to_c(nv,_tmp624,0);
Cyc_Toc_stmt_to_c(nv,_tmp645);
goto _LL2D;case 4U: _LL36: _tmp627=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60C)->f1;_tmp626=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60C)->f2;_tmp625=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp60C)->f3;_LL37: {
# 3984
struct Cyc_Absyn_Stmt*_tmp617=_tmp645;
# 3986
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp626->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3993
Cyc_Toc_stmt_to_c(nv,_tmp617);
if(Cyc_Absyn_no_regions)
({void*_tmpAFD=({struct _tuple1*_tmpAFC=x_var;void*_tmpAFB=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAFA=
Cyc_Absyn_uint_exp(0U,0U);
# 3995
Cyc_Absyn_declare_stmt(_tmpAFC,_tmpAFB,_tmpAFA,_tmp617,0U);})->r;s->r=_tmpAFD;});else{
# 3997
if((unsigned int)_tmp625){
# 3999
Cyc_Toc_exp_to_c(nv,_tmp625);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpAFF=_tmp625;Cyc_Absyn_aggrarrow_exp(_tmpAFF,({struct _dyneither_ptr*_tmp619=_cycalloc(sizeof(*_tmp619));({struct _dyneither_ptr _tmpAFE=({const char*_tmp618="h";_tag_dyneither(_tmp618,sizeof(char),2U);});*_tmp619=_tmpAFE;});_tmp619;}),0U);}),0U);
({void*_tmpB00=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp617,0U))->r;s->r=_tmpB00;});};}else{
# 4009
({void*_tmpB0D=({
struct _tuple1*_tmpB0C=rh_var;void*_tmpB0B=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpB0A=({
struct Cyc_Absyn_Exp*_tmpB02=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpB02,({struct Cyc_List_List*_tmp61A=_cycalloc(sizeof(*_tmp61A));
({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp61A->hd=_tmpB01;}),_tmp61A->tl=0;_tmp61A;}),0U);});
# 4010
Cyc_Absyn_declare_stmt(_tmpB0C,_tmpB0B,_tmpB0A,({
# 4014
struct _tuple1*_tmpB09=x_var;void*_tmpB08=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpB07=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpB09,_tmpB08,_tmpB07,({
struct Cyc_Absyn_Stmt*_tmpB06=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpB03,({struct Cyc_Absyn_Exp*_tmp61B[1U];_tmp61B[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61B,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpB06,({
struct Cyc_Absyn_Stmt*_tmpB05=_tmp617;Cyc_Absyn_seq_stmt(_tmpB05,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB04=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpB04,({struct Cyc_Absyn_Exp*_tmp61C[1U];_tmp61C[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61C,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 4009
s->r=_tmpB0D;});}}
# 4020
return;}default: _LL38: _LL39:
({void*_tmp61D=0U;({struct _dyneither_ptr _tmpB0E=({const char*_tmp61E="bad nested declaration within function";_tag_dyneither(_tmp61E,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB0E,_tag_dyneither(_tmp61D,sizeof(void*),0U));});});}_LL2D:;}
# 4023
return;case 13U: _LL1B: _tmp648=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp647=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_LL1C:
# 4025
 s=_tmp647;continue;case 14U: _LL1D: _tmp64A=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp649=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2).f1;_LL1E: {
# 4027
struct _RegionHandle _tmp628=_new_region("temp");struct _RegionHandle*temp=& _tmp628;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB0F=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB0F,_tmp64A);});
Cyc_Toc_exp_to_c(nv,_tmp649);
# 4031
_npop_handler(0U);return;
# 4027
;_pop_region(temp);}default: _LL1F: _tmp64D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f1;_tmp64C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f2;_tmp64B=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5F6)->f3;_LL20: {
# 4045 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4055
e_exp->topt=e_typ;{
struct _RegionHandle _tmp629=_new_region("temp");struct _RegionHandle*temp=& _tmp629;_push_region(temp);
# 4058
Cyc_Toc_stmt_to_c(nv,_tmp64D);{
struct Cyc_Absyn_Stmt*_tmp62A=({struct Cyc_Absyn_Stmt*_tmpB10=_tmp64D;Cyc_Absyn_seq_stmt(_tmpB10,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 4063
struct Cyc_Absyn_Stmt*_tmp62B=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=10U,_tmp632->f1=e_exp,_tmp632->f2=_tmp64C,_tmp632->f3=_tmp64B;_tmp632;}),0U);
# 4065
Cyc_Toc_stmt_to_c(nv,_tmp62B);{
# 4068
struct Cyc_Absyn_Exp*_tmp62C=({
struct Cyc_Absyn_Exp*_tmpB12=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpB12,({struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));
({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp631->hd=_tmpB11;}),_tmp631->tl=0;_tmp631;}),0U);});
# 4072
struct Cyc_Absyn_Stmt*_tmp62D=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB14=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpB14,({struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));
({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Absyn_address_exp(h_exp,0U);_tmp630->hd=_tmpB13;}),_tmp630->tl=0;_tmp630;}),0U);}),0U);
# 4076
struct Cyc_Absyn_Exp*_tmp62E=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp62F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB23=({
struct _tuple1*_tmpB22=h_var;void*_tmpB21=h_typ;Cyc_Absyn_declare_stmt(_tmpB22,_tmpB21,0,({
struct Cyc_Absyn_Stmt*_tmpB20=_tmp62D;Cyc_Absyn_seq_stmt(_tmpB20,({
struct _tuple1*_tmpB1F=was_thrown_var;void*_tmpB1E=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB1D=_tmp62E;Cyc_Absyn_declare_stmt(_tmpB1F,_tmpB1E,_tmpB1D,({
struct Cyc_Absyn_Stmt*_tmpB1C=({struct Cyc_Absyn_Exp*_tmpB16=_tmp62C;struct Cyc_Absyn_Stmt*_tmpB15=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp62F,0U);
# 4082
Cyc_Absyn_ifthenelse_stmt(_tmpB16,_tmpB15,
# 4084
Cyc_Toc_skip_stmt_dl(),0U);});
# 4082
Cyc_Absyn_seq_stmt(_tmpB1C,({
# 4085
struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB1A=_tmp62A;Cyc_Absyn_ifthenelse_stmt(_tmpB1B,_tmpB1A,({
# 4087
struct _tuple1*_tmpB19=e_var;void*_tmpB18=e_typ;struct Cyc_Absyn_Exp*_tmpB17=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4087
Cyc_Absyn_declare_stmt(_tmpB19,_tmpB18,_tmpB17,_tmp62B,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4078
s->r=_tmpB23;});};};
# 4091
_npop_handler(0U);return;
# 4056
;_pop_region(temp);};}}_LL0:;}}
# 4100
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB24=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB24;});{
struct _RegionHandle _tmp64E=_new_region("frgn");struct _RegionHandle*frgn=& _tmp64E;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp64F=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp650=f->args;for(0;_tmp650 != 0;_tmp650=_tmp650->tl){
struct _tuple1*_tmp651=({struct _tuple1*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->f1=Cyc_Absyn_Loc_n,_tmp652->f2=(*((struct _tuple9*)_tmp650->hd)).f1;_tmp652;});
({void*_tmpB25=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp650->hd)).f3);(*((struct _tuple9*)_tmp650->hd)).f3=_tmpB25;});}}
# 4116
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4120
({struct Cyc_Hashtable_Table**_tmpB27=({struct Cyc_Hashtable_Table**_tmp653=_cycalloc(sizeof(*_tmp653));({struct Cyc_Hashtable_Table*_tmpB26=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp653=_tmpB26;});_tmp653;});Cyc_Toc_fn_pop_table=_tmpB27;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp654=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp655=_tmp654;struct _dyneither_ptr*_tmp65E;struct Cyc_Absyn_Tqual _tmp65D;void*_tmp65C;int _tmp65B;_LL1: _tmp65E=_tmp655.name;_tmp65D=_tmp655.tq;_tmp65C=_tmp655.type;_tmp65B=_tmp655.inject;_LL2:;{
void*_tmp656=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp65C,(void*)& Cyc_Absyn_HeapRgn_val,_tmp65D,Cyc_Absyn_false_conref));
struct _tuple1*_tmp657=({struct _tuple1*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->f1=Cyc_Absyn_Loc_n,_tmp65A->f2=(struct _dyneither_ptr*)_check_null(_tmp65E);_tmp65A;});
({struct Cyc_List_List*_tmpB2A=({struct Cyc_List_List*_tmpB29=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB29,({struct Cyc_List_List*_tmp659=_cycalloc(sizeof(*_tmp659));({struct _tuple9*_tmpB28=({struct _tuple9*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->f1=_tmp65E,_tmp658->f2=_tmp65D,_tmp658->f3=_tmp656;_tmp658;});_tmp659->hd=_tmpB28;}),_tmp659->tl=0;_tmp659;}));});f->args=_tmpB2A;});
f->cyc_varargs=0;};}
# 4129
{struct Cyc_List_List*_tmp65F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp65F != 0;_tmp65F=_tmp65F->tl){
({void*_tmpB2B=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp65F->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp65F->hd)->type=_tmpB2B;});}}
# 4132
({struct Cyc_Toc_Env*_tmpB2C=Cyc_Toc_clear_toplevel(frgn,_tmp64F);Cyc_Toc_stmt_to_c(_tmpB2C,f->body);});}
# 4109
;_pop_region(frgn);};}
# 4136
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp660=s;switch(_tmp660){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple40{struct Cyc_Toc_TocState*f1;struct _tuple39*f2;};
# 4153 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4156
struct _tuple40 _tmp661=*env;struct _tuple40 _tmp662=_tmp661;struct Cyc_Toc_TocState*_tmp697;struct Cyc_Absyn_Aggrdecl*_tmp696;int _tmp695;_LL1: _tmp697=_tmp662.f1;_tmp696=(_tmp662.f2)->f1;_tmp695=(_tmp662.f2)->f2;_LL2:;{
struct _tuple1*_tmp663=_tmp696->name;
struct Cyc_Toc_TocState _tmp664=*_tmp697;struct Cyc_Toc_TocState _tmp665=_tmp664;struct Cyc_Dict_Dict*_tmp694;_LL4: _tmp694=_tmp665.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp666=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp694,_tmp663);
if(_tmp666 == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp696->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB2E=({struct _tuple19*_tmp667=_region_malloc(d,sizeof(*_tmp667));_tmp667->f1=_tmp696,({void*_tmpB2D=Cyc_Absyn_strctq(_tmp663);_tmp667->f2=_tmpB2D;});_tmp667;});v=_tmpB2E;});else{
# 4167
({struct _tuple19*_tmpB30=({struct _tuple19*_tmp668=_region_malloc(d,sizeof(*_tmp668));_tmp668->f1=_tmp696,({void*_tmpB2F=Cyc_Absyn_unionq_typ(_tmp663);_tmp668->f2=_tmpB2F;});_tmp668;});v=_tmpB30;});}
({struct Cyc_Dict_Dict _tmpB31=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp694,_tmp663,v);*_tmp694=_tmpB31;});};}else{
# 4170
struct _tuple19*_tmp669=*_tmp666;struct _tuple19*_tmp66A=_tmp669;struct Cyc_Absyn_Aggrdecl*_tmp66D;void*_tmp66C;_LL7: _tmp66D=_tmp66A->f1;_tmp66C=_tmp66A->f2;_LL8:;
if(_tmp66D->impl == 0){
({struct Cyc_Dict_Dict _tmpB34=({struct Cyc_Dict_Dict _tmpB33=*_tmp694;struct _tuple1*_tmpB32=_tmp663;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB33,_tmpB32,({struct _tuple19*_tmp66B=_region_malloc(d,sizeof(*_tmp66B));_tmp66B->f1=_tmp696,_tmp66B->f2=_tmp66C;_tmp66B;}));});*_tmp694=_tmpB34;});
seen_defn_before=0;}else{
# 4175
seen_defn_before=1;}}{
# 4177
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->kind=_tmp696->kind,_tmp693->sc=Cyc_Absyn_Public,_tmp693->name=_tmp696->name,_tmp693->tvs=0,_tmp693->impl=0,_tmp693->expected_mem_kind=0,_tmp693->attributes=_tmp696->attributes;_tmp693;});
# 4184
if(_tmp696->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB35=({struct Cyc_Absyn_AggrdeclImpl*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->exist_vars=0,_tmp66E->rgn_po=0,_tmp66E->fields=0,_tmp66E->tagged=0;_tmp66E;});new_ad->impl=_tmpB35;});{
# 4189
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp66F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp696->impl))->fields;for(0;_tmp66F != 0;_tmp66F=_tmp66F->tl){
# 4193
struct Cyc_Absyn_Aggrfield*_tmp670=(struct Cyc_Absyn_Aggrfield*)_tmp66F->hd;
void*_tmp671=_tmp670->type;
struct Cyc_List_List*_tmp672=_tmp670->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp671)) && (
_tmp696->kind == Cyc_Absyn_StructA  && _tmp66F->tl == 0  || _tmp696->kind == Cyc_Absyn_UnionA)){
# 4207 "toc.cyc"
void*_tmp673=Cyc_Tcutil_compress(_tmp671);void*_tmp674=_tmp673;void*_tmp67C;struct Cyc_Absyn_Tqual _tmp67B;union Cyc_Absyn_Constraint*_tmp67A;unsigned int _tmp679;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp674)->tag == 8U){_LLA: _tmp67C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp674)->f1).elt_type;_tmp67B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp674)->f1).tq;_tmp67A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp674)->f1).zero_term;_tmp679=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp674)->f1).zt_loc;_LLB:
# 4210
({void*_tmpB37=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->tag=8U,(_tmp675->f1).elt_type=_tmp67C,(_tmp675->f1).tq=_tmp67B,({struct Cyc_Absyn_Exp*_tmpB36=Cyc_Absyn_uint_exp(0U,0U);(_tmp675->f1).num_elts=_tmpB36;}),(_tmp675->f1).zero_term=_tmp67A,(_tmp675->f1).zt_loc=_tmp679;_tmp675;});_tmp671=_tmpB37;});
goto _LL9;}else{_LLC: _LLD:
# 4213
({struct Cyc_List_List*_tmpB39=({struct Cyc_List_List*_tmp677=_cycalloc(sizeof(*_tmp677));({void*_tmpB38=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->tag=6U,_tmp676->f1=0;_tmp676;});_tmp677->hd=_tmpB38;}),_tmp677->tl=_tmp672;_tmp677;});_tmp672=_tmpB39;});
({void*_tmpB3E=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->tag=8U,({void*_tmpB3D=Cyc_Absyn_void_star_typ();(_tmp678->f1).elt_type=_tmpB3D;}),({
struct Cyc_Absyn_Tqual _tmpB3C=Cyc_Absyn_empty_tqual(0U);(_tmp678->f1).tq=_tmpB3C;}),({
struct Cyc_Absyn_Exp*_tmpB3B=Cyc_Absyn_uint_exp(0U,0U);(_tmp678->f1).num_elts=_tmpB3B;}),({
union Cyc_Absyn_Constraint*_tmpB3A=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp678->f1).zero_term=_tmpB3A;}),(_tmp678->f1).zt_loc=0U;_tmp678;});
# 4214
_tmp671=_tmpB3E;});}_LL9:;}{
# 4220
struct Cyc_Absyn_Aggrfield*_tmp67D=({struct Cyc_Absyn_Aggrfield*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->name=_tmp670->name,_tmp68F->tq=Cyc_Toc_mt_tq,({
# 4222
void*_tmpB3F=Cyc_Toc_typ_to_c(_tmp671);_tmp68F->type=_tmpB3F;}),_tmp68F->width=_tmp670->width,_tmp68F->attributes=_tmp672,_tmp68F->requires_clause=0;_tmp68F;});
# 4230
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp696->impl))->tagged){
void*_tmp67E=_tmp67D->type;
struct _dyneither_ptr*_tmp67F=_tmp67D->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp68C=({struct Cyc_String_pa_PrintArg_struct _tmp7A7;_tmp7A7.tag=0U,_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp696->name).f2);_tmp7A7;});struct Cyc_String_pa_PrintArg_struct _tmp68D=({struct Cyc_String_pa_PrintArg_struct _tmp7A6;_tmp7A6.tag=0U,_tmp7A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp67F);_tmp7A6;});void*_tmp68A[2U];_tmp68A[0]=& _tmp68C,_tmp68A[1]=& _tmp68D;({struct _dyneither_ptr _tmpB40=({const char*_tmp68B="_union_%s_%s";_tag_dyneither(_tmp68B,sizeof(char),13U);});Cyc_aprintf(_tmpB40,_tag_dyneither(_tmp68A,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp689=_cycalloc(sizeof(*_tmp689));*_tmp689=s;_tmp689;});
struct Cyc_Absyn_Aggrfield*_tmp680=({struct Cyc_Absyn_Aggrfield*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->name=Cyc_Toc_val_sp,_tmp688->tq=Cyc_Toc_mt_tq,_tmp688->type=_tmp67E,_tmp688->width=0,_tmp688->attributes=0,_tmp688->requires_clause=0;_tmp688;});
struct Cyc_Absyn_Aggrfield*_tmp681=({struct Cyc_Absyn_Aggrfield*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687->name=Cyc_Toc_tag_sp,_tmp687->tq=Cyc_Toc_mt_tq,_tmp687->type=Cyc_Absyn_sint_typ,_tmp687->width=0,_tmp687->attributes=0,_tmp687->requires_clause=0;_tmp687;});
struct Cyc_List_List*_tmp682=({struct Cyc_Absyn_Aggrfield*_tmp686[2U];_tmp686[0]=_tmp681,_tmp686[1]=_tmp680;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp686,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp683=Cyc_Toc_make_c_struct_defn(str,_tmp682);
({struct Cyc_List_List*_tmpB42=({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct Cyc_Absyn_Decl*_tmpB41=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->tag=5U,_tmp684->f1=_tmp683;_tmp684;}),0U);_tmp685->hd=_tmpB41;}),_tmp685->tl=Cyc_Toc_result_decls;_tmp685;});Cyc_Toc_result_decls=_tmpB42;});
({void*_tmpB43=Cyc_Absyn_strct(str);_tmp67D->type=_tmpB43;});}
# 4242
({struct Cyc_List_List*_tmpB44=({struct Cyc_List_List*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->hd=_tmp67D,_tmp68E->tl=new_fields;_tmp68E;});new_fields=_tmpB44;});};}}
# 4244
({struct Cyc_List_List*_tmpB45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB45;});};}
# 4246
if(_tmp695)
({struct Cyc_List_List*_tmpB48=({struct Cyc_List_List*_tmp692=_cycalloc(sizeof(*_tmp692));({struct Cyc_Absyn_Decl*_tmpB47=({struct Cyc_Absyn_Decl*_tmp691=_cycalloc(sizeof(*_tmp691));({void*_tmpB46=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->tag=5U,_tmp690->f1=new_ad;_tmp690;});_tmp691->r=_tmpB46;}),_tmp691->loc=0U;_tmp691;});_tmp692->hd=_tmpB47;}),_tmp692->tl=Cyc_Toc_result_decls;_tmp692;});Cyc_Toc_result_decls=_tmpB48;});
return 0;};};};}
# 4251
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple39 p=({struct _tuple39 _tmp7A8;_tmp7A8.f1=ad,_tmp7A8.f2=add_to_decls;_tmp7A8;});
((int(*)(struct _tuple39*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4280 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4283
struct _tuple41 _tmp698=*env;struct _tuple41 _tmp699=_tmp698;struct Cyc_Set_Set**_tmp6AA;struct Cyc_Absyn_Datatypedecl*_tmp6A9;_LL1: _tmp6AA=(_tmp699.f1)->datatypes_so_far;_tmp6A9=_tmp699.f2;_LL2:;{
struct _tuple1*_tmp69A=_tmp6A9->name;
if(_tmp6A9->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp6AA,_tmp69A))
return 0;
({struct Cyc_Set_Set*_tmpB49=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp6AA,_tmp69A);*_tmp6AA=_tmpB49;});
{struct Cyc_List_List*_tmp69B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6A9->fields))->v;for(0;_tmp69B != 0;_tmp69B=_tmp69B->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp69B->hd;
# 4291
struct Cyc_List_List*_tmp69C=0;
int i=1;
{struct Cyc_List_List*_tmp69D=f->typs;for(0;_tmp69D != 0;(_tmp69D=_tmp69D->tl,i ++)){
struct _dyneither_ptr*_tmp69E=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp69F=({struct Cyc_Absyn_Aggrfield*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->name=_tmp69E,_tmp6A1->tq=(*((struct _tuple12*)_tmp69D->hd)).f1,({
void*_tmpB4A=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp69D->hd)).f2);_tmp6A1->type=_tmpB4A;}),_tmp6A1->width=0,_tmp6A1->attributes=0,_tmp6A1->requires_clause=0;_tmp6A1;});
({struct Cyc_List_List*_tmpB4B=({struct Cyc_List_List*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->hd=_tmp69F,_tmp6A0->tl=_tmp69C;_tmp6A0;});_tmp69C=_tmpB4B;});}}
# 4299
({struct Cyc_List_List*_tmpB4E=({struct Cyc_List_List*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));({struct Cyc_Absyn_Aggrfield*_tmpB4D=({struct Cyc_Absyn_Aggrfield*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2->name=Cyc_Toc_tag_sp,_tmp6A2->tq=Cyc_Toc_mt_tq,_tmp6A2->type=Cyc_Absyn_sint_typ,_tmp6A2->width=0,_tmp6A2->attributes=0,_tmp6A2->requires_clause=0;_tmp6A2;});_tmp6A3->hd=_tmpB4D;}),({
struct Cyc_List_List*_tmpB4C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp69C);_tmp6A3->tl=_tmpB4C;});_tmp6A3;});
# 4299
_tmp69C=_tmpB4E;});{
# 4301
struct Cyc_Absyn_Aggrdecl*_tmp6A4=({struct _dyneither_ptr*_tmpB50=({struct _dyneither_ptr*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));({struct _dyneither_ptr _tmpB4F=({const char*_tmp6A7="";_tag_dyneither(_tmp6A7,sizeof(char),1U);});*_tmp6A8=_tmpB4F;});_tmp6A8;});Cyc_Toc_make_c_struct_defn(_tmpB50,_tmp69C);});
({struct _tuple1*_tmpB51=Cyc_Toc_collapse_qvars(f->name,_tmp6A9->name);_tmp6A4->name=_tmpB51;});
({struct Cyc_List_List*_tmpB53=({struct Cyc_List_List*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));({struct Cyc_Absyn_Decl*_tmpB52=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->tag=5U,_tmp6A5->f1=_tmp6A4;_tmp6A5;}),0U);_tmp6A6->hd=_tmpB52;}),_tmp6A6->tl=Cyc_Toc_result_decls;_tmp6A6;});Cyc_Toc_result_decls=_tmpB53;});};}}
# 4305
return 0;};}
# 4308
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4327 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4330
struct _tuple41 _tmp6AB=*env;struct _tuple41 _tmp6AC=_tmp6AB;struct Cyc_Toc_TocState*_tmp6CE;struct Cyc_Absyn_Datatypedecl*_tmp6CD;_LL1: _tmp6CE=_tmp6AC.f1;_tmp6CD=_tmp6AC.f2;_LL2:;
if(_tmp6CD->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6AD=*_tmp6CE;struct Cyc_Toc_TocState _tmp6AE=_tmp6AD;struct Cyc_Dict_Dict*_tmp6CC;_LL4: _tmp6CC=_tmp6AE.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp6AF=_tmp6CD->name;
{struct Cyc_List_List*_tmp6B0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6CD->fields))->v;for(0;_tmp6B0 != 0;_tmp6B0=_tmp6B0->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6B0->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6B1=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp6B2=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp6B1,Cyc_Absyn_false_conref,0U);
# 4341
int*_tmp6B3=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6CC,f->name);int*_tmp6B4=_tmp6B3;if(_tmp6B4 == 0){_LL7: _LL8: {
# 4343
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB54=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB54;});{
# 4347
struct Cyc_Absyn_Vardecl*_tmp6B5=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B2,initopt);
_tmp6B5->sc=f->sc;
({struct Cyc_List_List*_tmpB56=({struct Cyc_List_List*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));({struct Cyc_Absyn_Decl*_tmpB55=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->tag=0U,_tmp6B6->f1=_tmp6B5;_tmp6B6;}),0U);_tmp6B7->hd=_tmpB55;}),_tmp6B7->tl=Cyc_Toc_result_decls;_tmp6B7;});Cyc_Toc_result_decls=_tmpB56;});
({struct Cyc_Dict_Dict _tmpB57=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6CC,f->name,f->sc != Cyc_Absyn_Extern);
# 4350
*_tmp6CC=_tmpB57;});{
# 4352
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6B8=f->typs;for(0;_tmp6B8 != 0;(_tmp6B8=_tmp6B8->tl,i ++)){
struct _dyneither_ptr*_tmp6B9=({struct _dyneither_ptr*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));({struct _dyneither_ptr _tmpB59=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6BF=({struct Cyc_Int_pa_PrintArg_struct _tmp7A9;_tmp7A9.tag=1U,_tmp7A9.f1=(unsigned long)i;_tmp7A9;});void*_tmp6BD[1U];_tmp6BD[0]=& _tmp6BF;({struct _dyneither_ptr _tmpB58=({const char*_tmp6BE="f%d";_tag_dyneither(_tmp6BE,sizeof(char),4U);});Cyc_aprintf(_tmpB58,_tag_dyneither(_tmp6BD,sizeof(void*),1U));});});*_tmp6C0=_tmpB59;});_tmp6C0;});
struct Cyc_Absyn_Aggrfield*_tmp6BA=({struct Cyc_Absyn_Aggrfield*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->name=_tmp6B9,_tmp6BC->tq=(*((struct _tuple12*)_tmp6B8->hd)).f1,({
void*_tmpB5A=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6B8->hd)).f2);_tmp6BC->type=_tmpB5A;}),_tmp6BC->width=0,_tmp6BC->attributes=0,_tmp6BC->requires_clause=0;_tmp6BC;});
({struct Cyc_List_List*_tmpB5B=({struct Cyc_List_List*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->hd=_tmp6BA,_tmp6BB->tl=fields;_tmp6BB;});fields=_tmpB5B;});}}
# 4360
({struct Cyc_List_List*_tmpB5F=({struct Cyc_List_List*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));({struct Cyc_Absyn_Aggrfield*_tmpB5E=({struct Cyc_Absyn_Aggrfield*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->name=Cyc_Toc_tag_sp,_tmp6C1->tq=Cyc_Toc_mt_tq,({
void*_tmpB5D=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp6C1->type=_tmpB5D;}),_tmp6C1->width=0,_tmp6C1->attributes=0,_tmp6C1->requires_clause=0;_tmp6C1;});
# 4360
_tmp6C2->hd=_tmpB5E;}),({
# 4362
struct Cyc_List_List*_tmpB5C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6C2->tl=_tmpB5C;});_tmp6C2;});
# 4360
fields=_tmpB5F;});{
# 4363
struct Cyc_Absyn_Aggrdecl*_tmp6C3=({struct _dyneither_ptr*_tmpB61=({struct _dyneither_ptr*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));({struct _dyneither_ptr _tmpB60=({const char*_tmp6C6="";_tag_dyneither(_tmp6C6,sizeof(char),1U);});*_tmp6C7=_tmpB60;});_tmp6C7;});Cyc_Toc_make_c_struct_defn(_tmpB61,fields);});
({struct _tuple1*_tmpB62=Cyc_Toc_collapse_qvars(f->name,_tmp6CD->name);_tmp6C3->name=_tmpB62;});
({struct Cyc_List_List*_tmpB64=({struct Cyc_List_List*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));({struct Cyc_Absyn_Decl*_tmpB63=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6C4=_cycalloc(sizeof(*_tmp6C4));_tmp6C4->tag=5U,_tmp6C4->f1=_tmp6C3;_tmp6C4;}),0U);_tmp6C5->hd=_tmpB63;}),_tmp6C5->tl=Cyc_Toc_result_decls;_tmp6C5;});Cyc_Toc_result_decls=_tmpB64;});
goto _LL6;};};};}}else{if(*((int*)_tmp6B4)== 0){_LL9: _LLA:
# 4368
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6C8=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6C9=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6B2,_tmp6C8);
_tmp6C9->sc=f->sc;
({struct Cyc_List_List*_tmpB66=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));({struct Cyc_Absyn_Decl*_tmpB65=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA->tag=0U,_tmp6CA->f1=_tmp6C9;_tmp6CA;}),0U);_tmp6CB->hd=_tmpB65;}),_tmp6CB->tl=Cyc_Toc_result_decls;_tmp6CB;});Cyc_Toc_result_decls=_tmpB66;});
({struct Cyc_Dict_Dict _tmpB67=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6CC,f->name,1);*_tmp6CC=_tmpB67;});}
# 4375
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4379
return 0;};};}
# 4382
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4386
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4392
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB68=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB68;});
# 4396
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4405
struct _RegionHandle _tmp6CF=_new_region("temp");struct _RegionHandle*temp=& _tmp6CF;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6D0=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6D0,init);}
# 4406
;_pop_region(temp);}else{
# 4410
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4413
void*_tmp6D1=Cyc_Tcutil_compress(old_typ);void*_tmp6D2=_tmp6D1;void*_tmp6D9;struct Cyc_Absyn_Exp*_tmp6D8;union Cyc_Absyn_Constraint*_tmp6D7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D2)->tag == 8U){_LL1: _tmp6D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D2)->f1).elt_type;_tmp6D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D2)->f1).num_elts;_tmp6D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D2)->f1).zero_term;_LL2:
# 4415
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D7)){
if(_tmp6D8 == 0)
({void*_tmp6D3=0U;({struct _dyneither_ptr _tmpB69=({const char*_tmp6D4="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6D4,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB69,_tag_dyneither(_tmp6D3,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6D8;
struct Cyc_Absyn_Exp*_tmp6D5=({struct Cyc_Absyn_Exp*_tmpB6B=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB6B,({
struct Cyc_Absyn_Exp*_tmpB6A=num_elts;Cyc_Absyn_add_exp(_tmpB6A,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4422
struct Cyc_Absyn_Exp*_tmp6D6=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB6D=({struct Cyc_Absyn_Stmt*_tmpB6C=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6D5,_tmp6D6,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB6C,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4423
s->r=_tmpB6D;});};}
# 4426
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4434
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6DA=d;struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*_tmp6E0;void**_tmp6DF;struct Cyc_Tcpat_Rhs*_tmp6DE;switch(*((int*)_tmp6DA)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6DE=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6DA)->f1;_LL4:
 _tmp6DE->rhs=success;return d;default: _LL5: _tmp6E1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DA)->f1;_tmp6E0=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DA)->f2;_tmp6DF=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6DA)->f3;_LL6:
# 4439
({void*_tmpB6E=Cyc_Toc_rewrite_decision(*_tmp6DF,success);*_tmp6DF=_tmpB6E;});
for(0;_tmp6E0 != 0;_tmp6E0=_tmp6E0->tl){
struct _tuple33*_tmp6DB=(struct _tuple33*)_tmp6E0->hd;struct _tuple33*_tmp6DC=_tmp6DB;void**_tmp6DD;_LL8: _tmp6DD=(void**)& _tmp6DC->f2;_LL9:;
({void*_tmpB6F=Cyc_Toc_rewrite_decision(*_tmp6DD,success);*_tmp6DD=_tmpB6F;});}
# 4444
return d;}_LL0:;}
# 4455 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4457
struct _RegionHandle _tmp6E2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6E2;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6E3=Cyc_Toc_share_env(rgn,nv);
void*_tmp6E4=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6E3,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4467
struct Cyc_Absyn_Stmt*_tmp6E5=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4471
({void*_tmpB70=Cyc_Toc_rewrite_decision(dopt,_tmp6E5);dopt=_tmpB70;});{
# 4473
struct Cyc_Absyn_Switch_clause*_tmp6E6=({struct Cyc_Absyn_Switch_clause*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->pattern=p,_tmp6F2->pat_vars=0,_tmp6F2->where_clause=0,_tmp6F2->body=_tmp6E5,_tmp6F2->loc=0U;_tmp6F2;});
struct Cyc_List_List*_tmp6E7=({struct _RegionHandle*_tmpB73=rgn;struct _RegionHandle*_tmpB72=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB73,Cyc_Toc_gen_labels,_tmpB72,({struct Cyc_Absyn_Switch_clause*_tmp6F1[1U];_tmp6F1[0]=_tmp6E6;({struct _RegionHandle*_tmpB71=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB71,_tag_dyneither(_tmp6F1,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4476
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4480
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6E3,& mydecls,& mybodies,dopt,_tmp6E7,v);
# 4483
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp6E8=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp6E9=_tmp6E8;struct Cyc_Toc_Env*_tmp6EB;struct Cyc_Absyn_Stmt*_tmp6EA;_LL1: _tmp6EB=_tmp6E9->f1;_tmp6EA=_tmp6E9->f3;_LL2:;
if(_tmp6EA == _tmp6E5){senv=_tmp6EB;goto FOUND_ENV;}}
# 4488
({void*_tmp6EC=0U;({struct _dyneither_ptr _tmpB74=({const char*_tmp6ED="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6ED,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB74,_tag_dyneither(_tmp6EC,sizeof(void*),0U));});});
FOUND_ENV:
# 4492
 Cyc_Toc_stmt_to_c(senv,s);{
# 4494
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4496
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6EE=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB75=Cyc_Toc_temp_var();_tmp6EE->name=_tmpB75;});
({void*_tmpB76=Cyc_Toc_typ_to_c_array(_tmp6EE->type);_tmp6EE->type=_tmpB76;});
({struct Cyc_Absyn_Stmt*_tmpB78=({struct Cyc_Absyn_Decl*_tmpB77=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->tag=0U,_tmp6EF->f1=_tmp6EE;_tmp6EF;}),0U);Cyc_Absyn_decl_stmt(_tmpB77,res,0U);});res=_tmpB78;});}
# 4503
({struct Cyc_Absyn_Stmt*_tmpB7C=({struct _tuple1*_tmpB7B=v;void*_tmpB7A=Cyc_Toc_typ_to_c(_tmp6E4);struct Cyc_Absyn_Exp*_tmpB79=e;Cyc_Absyn_declare_stmt(_tmpB7B,_tmpB7A,_tmpB79,res,0U);});res=_tmpB7C;});{
struct Cyc_Absyn_Stmt*_tmp6F0=res;_npop_handler(0U);return _tmp6F0;};};};};}
# 4458
;_pop_region(rgn);}
# 4511
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6F3=e->r;void*_tmp6F4=_tmp6F3;struct Cyc_Absyn_MallocInfo*_tmp71E;struct Cyc_Absyn_Stmt*_tmp71D;void**_tmp71C;void**_tmp71B;struct Cyc_List_List*_tmp71A;struct Cyc_List_List*_tmp719;void**_tmp718;struct Cyc_List_List*_tmp717;void**_tmp716;struct Cyc_Absyn_Exp*_tmp715;struct Cyc_Absyn_Exp*_tmp714;struct Cyc_List_List*_tmp713;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp711;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_Absyn_Exp*_tmp70F;struct Cyc_Absyn_Exp*_tmp70E;struct Cyc_Absyn_Exp*_tmp70D;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_Absyn_Exp*_tmp70B;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_Absyn_Exp*_tmp708;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_List_List*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp701;struct Cyc_Absyn_Exp*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FC;struct Cyc_Absyn_Exp*_tmp6FB;switch(*((int*)_tmp6F4)){case 20U: _LL1: _tmp6FB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL2:
 _tmp6FC=_tmp6FB;goto _LL4;case 21U: _LL3: _tmp6FC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL4:
 _tmp6FD=_tmp6FC;goto _LL6;case 22U: _LL5: _tmp6FD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL6:
 _tmp6FE=_tmp6FD;goto _LL8;case 15U: _LL7: _tmp6FE=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL8:
 _tmp6FF=_tmp6FE;goto _LLA;case 11U: _LL9: _tmp6FF=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LLA:
 _tmp700=_tmp6FF;goto _LLC;case 12U: _LLB: _tmp700=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LLC:
 _tmp701=_tmp700;goto _LLE;case 18U: _LLD: _tmp701=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LLE:
 _tmp702=_tmp701;goto _LL10;case 5U: _LLF: _tmp702=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL10:
 Cyc_Toc_exptypes_to_c(_tmp702);goto _LL0;case 3U: _LL11: _tmp703=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL12:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp703);goto _LL0;case 7U: _LL13: _tmp705=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp704=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL14:
 _tmp707=_tmp705;_tmp706=_tmp704;goto _LL16;case 8U: _LL15: _tmp707=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp706=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL16:
 _tmp709=_tmp707;_tmp708=_tmp706;goto _LL18;case 9U: _LL17: _tmp709=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp708=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL18:
 _tmp70B=_tmp709;_tmp70A=_tmp708;goto _LL1A;case 23U: _LL19: _tmp70B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp70A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL1A:
 _tmp70D=_tmp70B;_tmp70C=_tmp70A;goto _LL1C;case 35U: _LL1B: _tmp70D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp70C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL1C:
 _tmp70F=_tmp70D;_tmp70E=_tmp70C;goto _LL1E;case 4U: _LL1D: _tmp70F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp70E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f3;_LL1E:
 Cyc_Toc_exptypes_to_c(_tmp70F);Cyc_Toc_exptypes_to_c(_tmp70E);goto _LL0;case 6U: _LL1F: _tmp712=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp711=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_tmp710=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6F4)->f3;_LL20:
# 4529
 Cyc_Toc_exptypes_to_c(_tmp712);Cyc_Toc_exptypes_to_c(_tmp711);Cyc_Toc_exptypes_to_c(_tmp710);goto _LL0;case 10U: _LL21: _tmp714=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp713=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL22:
# 4531
 Cyc_Toc_exptypes_to_c(_tmp714);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp713);goto _LL0;case 14U: _LL23: _tmp716=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_tmp715=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL24:
({void*_tmpB7D=Cyc_Toc_typ_to_c(*_tmp716);*_tmp716=_tmpB7D;});Cyc_Toc_exptypes_to_c(_tmp715);goto _LL0;case 25U: _LL25: _tmp718=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1)->f3;_tmp717=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL26:
# 4534
({void*_tmpB7E=Cyc_Toc_typ_to_c(*_tmp718);*_tmp718=_tmpB7E;});
_tmp719=_tmp717;goto _LL28;case 36U: _LL27: _tmp719=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6F4)->f2;_LL28:
 _tmp71A=_tmp719;goto _LL2A;case 26U: _LL29: _tmp71A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL2A:
# 4538
 for(0;_tmp71A != 0;_tmp71A=_tmp71A->tl){
struct _tuple20 _tmp6F5=*((struct _tuple20*)_tmp71A->hd);struct _tuple20 _tmp6F6=_tmp6F5;struct Cyc_Absyn_Exp*_tmp6F7;_LL54: _tmp6F7=_tmp6F6.f2;_LL55:;
Cyc_Toc_exptypes_to_c(_tmp6F7);}
# 4542
goto _LL0;case 19U: _LL2B: _tmp71B=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL2C:
 _tmp71C=_tmp71B;goto _LL2E;case 17U: _LL2D: _tmp71C=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL2E:
({void*_tmpB7F=Cyc_Toc_typ_to_c(*_tmp71C);*_tmp71C=_tmpB7F;});goto _LL0;case 37U: _LL2F: _tmp71D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL30:
 Cyc_Toc_stmttypes_to_c(_tmp71D);goto _LL0;case 34U: _LL31: _tmp71E=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6F4)->f1;_LL32:
# 4547
 if(_tmp71E->elt_type != 0)
({void**_tmpB81=({void**_tmp6F8=_cycalloc(sizeof(*_tmp6F8));({void*_tmpB80=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp71E->elt_type)));*_tmp6F8=_tmpB80;});_tmp6F8;});_tmp71E->elt_type=_tmpB81;});
Cyc_Toc_exptypes_to_c(_tmp71E->num_elts);
goto _LL0;case 0U: _LL33: _LL34:
 goto _LL36;case 1U: _LL35: _LL36:
 goto _LL38;case 32U: _LL37: _LL38:
 goto _LL3A;case 40U: _LL39: _LL3A:
 goto _LL3C;case 33U: _LL3B: _LL3C:
 goto _LL0;case 2U: _LL3D: _LL3E:
# 4557
 goto _LL40;case 30U: _LL3F: _LL40:
 goto _LL42;case 31U: _LL41: _LL42:
 goto _LL44;case 29U: _LL43: _LL44:
 goto _LL46;case 27U: _LL45: _LL46:
 goto _LL48;case 28U: _LL47: _LL48:
 goto _LL4A;case 24U: _LL49: _LL4A:
 goto _LL4C;case 13U: _LL4B: _LL4C:
 goto _LL4E;case 16U: _LL4D: _LL4E:
 goto _LL50;case 39U: _LL4F: _LL50:
 goto _LL52;default: _LL51: _LL52:
({void*_tmp6F9=0U;({unsigned int _tmpB83=e->loc;struct _dyneither_ptr _tmpB82=({const char*_tmp6FA="Cyclone expression within C code";_tag_dyneither(_tmp6FA,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpB83,_tmpB82,_tag_dyneither(_tmp6F9,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4571
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp71F=d->r;void*_tmp720=_tmp71F;struct Cyc_Absyn_Typedefdecl*_tmp72A;struct Cyc_Absyn_Enumdecl*_tmp729;struct Cyc_Absyn_Aggrdecl*_tmp728;struct Cyc_Absyn_Fndecl*_tmp727;struct Cyc_Absyn_Vardecl*_tmp726;switch(*((int*)_tmp720)){case 0U: _LL1: _tmp726=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp720)->f1;_LL2:
# 4574
({void*_tmpB84=Cyc_Toc_typ_to_c(_tmp726->type);_tmp726->type=_tmpB84;});
if(_tmp726->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp726->initializer));
goto _LL0;case 1U: _LL3: _tmp727=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp720)->f1;_LL4:
# 4578
({void*_tmpB85=Cyc_Toc_typ_to_c(_tmp727->ret_type);_tmp727->ret_type=_tmpB85;});
{struct Cyc_List_List*_tmp721=_tmp727->args;for(0;_tmp721 != 0;_tmp721=_tmp721->tl){
({void*_tmpB86=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp721->hd)).f3);(*((struct _tuple9*)_tmp721->hd)).f3=_tmpB86;});}}
# 4582
goto _LL0;case 5U: _LL5: _tmp728=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp720)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp728,1);goto _LL0;case 7U: _LL7: _tmp729=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp720)->f1;_LL8:
# 4585
 if(_tmp729->fields != 0){
struct Cyc_List_List*_tmp722=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp729->fields))->v;for(0;_tmp722 != 0;_tmp722=_tmp722->tl){
struct Cyc_Absyn_Enumfield*_tmp723=(struct Cyc_Absyn_Enumfield*)_tmp722->hd;
if(_tmp723->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp723->tag));}}
# 4590
goto _LL0;case 8U: _LL9: _tmp72A=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp720)->f1;_LLA:
({void*_tmpB87=Cyc_Toc_typ_to_c((void*)_check_null(_tmp72A->defn));_tmp72A->defn=_tmpB87;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4600
({void*_tmp724=0U;({unsigned int _tmpB89=d->loc;struct _dyneither_ptr _tmpB88=({const char*_tmp725="Cyclone declaration within C code";_tag_dyneither(_tmp725,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpB89,_tmpB88,_tag_dyneither(_tmp724,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 4604
 goto _LL0;}_LL0:;}
# 4608
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp72B=s->r;void*_tmp72C=_tmp72B;struct Cyc_Absyn_Stmt*_tmp746;struct Cyc_Absyn_Stmt*_tmp745;struct Cyc_Absyn_Exp*_tmp744;struct Cyc_Absyn_Decl*_tmp743;struct Cyc_Absyn_Stmt*_tmp742;struct Cyc_Absyn_Exp*_tmp741;struct Cyc_List_List*_tmp740;void*_tmp73F;struct Cyc_Absyn_Exp*_tmp73E;struct Cyc_Absyn_Exp*_tmp73D;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Stmt*_tmp73B;struct Cyc_Absyn_Exp*_tmp73A;struct Cyc_Absyn_Stmt*_tmp739;struct Cyc_Absyn_Exp*_tmp738;struct Cyc_Absyn_Stmt*_tmp737;struct Cyc_Absyn_Stmt*_tmp736;struct Cyc_Absyn_Exp*_tmp735;struct Cyc_Absyn_Stmt*_tmp734;struct Cyc_Absyn_Stmt*_tmp733;struct Cyc_Absyn_Exp*_tmp732;switch(*((int*)_tmp72C)){case 1U: _LL1: _tmp732=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp732);goto _LL0;case 2U: _LL3: _tmp734=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_tmp733=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp734);Cyc_Toc_stmttypes_to_c(_tmp733);goto _LL0;case 3U: _LL5: _tmp735=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_LL6:
 if(_tmp735 != 0)Cyc_Toc_exptypes_to_c(_tmp735);goto _LL0;case 4U: _LL7: _tmp738=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_tmp737=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2;_tmp736=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp72C)->f3;_LL8:
# 4614
 Cyc_Toc_exptypes_to_c(_tmp738);Cyc_Toc_stmttypes_to_c(_tmp737);Cyc_Toc_stmttypes_to_c(_tmp736);goto _LL0;case 5U: _LL9: _tmp73A=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1).f1;_tmp739=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2;_LLA:
# 4616
 Cyc_Toc_exptypes_to_c(_tmp73A);Cyc_Toc_stmttypes_to_c(_tmp739);goto _LL0;case 9U: _LLB: _tmp73E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_tmp73D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2).f1;_tmp73C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72C)->f3).f1;_tmp73B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp72C)->f4;_LLC:
# 4618
 Cyc_Toc_exptypes_to_c(_tmp73E);Cyc_Toc_exptypes_to_c(_tmp73D);Cyc_Toc_exptypes_to_c(_tmp73C);
Cyc_Toc_stmttypes_to_c(_tmp73B);goto _LL0;case 10U: _LLD: _tmp741=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_tmp740=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2;_tmp73F=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp72C)->f3;_LLE:
# 4621
 Cyc_Toc_exptypes_to_c(_tmp741);
for(0;_tmp740 != 0;_tmp740=_tmp740->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp740->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp743=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_tmp742=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp743);Cyc_Toc_stmttypes_to_c(_tmp742);goto _LL0;case 14U: _LL11: _tmp745=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72C)->f1;_tmp744=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp745);Cyc_Toc_exptypes_to_c(_tmp744);goto _LL0;case 13U: _LL13: _tmp746=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp72C)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp746);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4631
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4634
({void*_tmpB8A=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D->tag=0U;_tmp72D;});s->r=_tmpB8A;});
goto _LL0;default: _LL1F: _LL20:
# 4637
({void*_tmp72E=0U;({unsigned int _tmpB8E=s->loc;struct _dyneither_ptr _tmpB8D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp731=({struct Cyc_String_pa_PrintArg_struct _tmp7AA;_tmp7AA.tag=0U,({struct _dyneither_ptr _tmpB8B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp7AA.f1=_tmpB8B;});_tmp7AA;});void*_tmp72F[1U];_tmp72F[0]=& _tmp731;({struct _dyneither_ptr _tmpB8C=({const char*_tmp730="Cyclone statement in C code: %s";_tag_dyneither(_tmp730,sizeof(char),32U);});Cyc_aprintf(_tmpB8C,_tag_dyneither(_tmp72F,sizeof(void*),1U));});});Cyc_Tcutil_terr(_tmpB8E,_tmpB8D,_tag_dyneither(_tmp72E,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 4645
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp747=0U;({struct _dyneither_ptr _tmpB8F=({const char*_tmp748="decls_to_c: not at toplevel!";_tag_dyneither(_tmp748,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB8F,_tag_dyneither(_tmp747,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp749=d->r;void*_tmp74A=_tmp749;struct Cyc_List_List*_tmp762;struct Cyc_List_List*_tmp761;struct Cyc_List_List*_tmp760;struct Cyc_List_List*_tmp75F;struct Cyc_Absyn_Typedefdecl*_tmp75E;struct Cyc_Absyn_Enumdecl*_tmp75D;struct Cyc_Absyn_Datatypedecl*_tmp75C;struct Cyc_Absyn_Aggrdecl*_tmp75B;struct Cyc_Absyn_Fndecl*_tmp75A;struct Cyc_Absyn_Vardecl*_tmp759;switch(*((int*)_tmp74A)){case 0U: _LL1: _tmp759=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LL2: {
# 4653
struct _tuple1*_tmp74B=_tmp759->name;
# 4655
if(_tmp759->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB91=({struct _tuple1*_tmp74C=_cycalloc(sizeof(*_tmp74C));({union Cyc_Absyn_Nmspace _tmpB90=Cyc_Absyn_Abs_n(0,1);_tmp74C->f1=_tmpB90;}),_tmp74C->f2=(*_tmp74B).f2;_tmp74C;});_tmp74B=_tmpB91;});
if(_tmp759->initializer != 0){
if(_tmp759->sc == Cyc_Absyn_ExternC)_tmp759->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp759->initializer));else{
# 4662
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp759->initializer));}}
# 4664
_tmp759->name=_tmp74B;
({enum Cyc_Absyn_Scope _tmpB92=Cyc_Toc_scope_to_c(_tmp759->sc);_tmp759->sc=_tmpB92;});
({void*_tmpB93=Cyc_Toc_typ_to_c(_tmp759->type);_tmp759->type=_tmpB93;});
({struct Cyc_List_List*_tmpB94=({struct Cyc_List_List*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->hd=d,_tmp74D->tl=Cyc_Toc_result_decls;_tmp74D;});Cyc_Toc_result_decls=_tmpB94;});
goto _LL0;}case 1U: _LL3: _tmp75A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LL4: {
# 4670
struct _tuple1*_tmp74E=_tmp75A->name;
# 4672
if(_tmp75A->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB96=({struct _tuple1*_tmp74F=_cycalloc(sizeof(*_tmp74F));({union Cyc_Absyn_Nmspace _tmpB95=Cyc_Absyn_Abs_n(0,1);_tmp74F->f1=_tmpB95;}),_tmp74F->f2=(*_tmp74E).f2;_tmp74F;});_tmp74E=_tmpB96;});
_tmp75A->sc=Cyc_Absyn_Public;}
# 4676
_tmp75A->name=_tmp74E;
Cyc_Toc_fndecl_to_c(nv,_tmp75A,cinclude);
({struct Cyc_List_List*_tmpB97=({struct Cyc_List_List*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750->hd=d,_tmp750->tl=Cyc_Toc_result_decls;_tmp750;});Cyc_Toc_result_decls=_tmpB97;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp751=0U;({struct _dyneither_ptr _tmpB98=({const char*_tmp752="letdecl at toplevel";_tag_dyneither(_tmp752,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB98,_tag_dyneither(_tmp751,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp753=0U;({struct _dyneither_ptr _tmpB99=({const char*_tmp754="region decl at toplevel";_tag_dyneither(_tmp754,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB99,_tag_dyneither(_tmp753,sizeof(void*),0U));});});case 5U: _LLB: _tmp75B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LLC:
# 4684
 Cyc_Toc_aggrdecl_to_c(_tmp75B,1);
goto _LL0;case 6U: _LLD: _tmp75C=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LLE:
# 4687
 if(_tmp75C->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp75C);else{
# 4690
Cyc_Toc_datatypedecl_to_c(_tmp75C);}
goto _LL0;case 7U: _LLF: _tmp75D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LL10:
# 4693
 Cyc_Toc_enumdecl_to_c(_tmp75D);
({struct Cyc_List_List*_tmpB9A=({struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->hd=d,_tmp755->tl=Cyc_Toc_result_decls;_tmp755;});Cyc_Toc_result_decls=_tmpB9A;});
goto _LL0;case 8U: _LL11: _tmp75E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LL12:
# 4697
 _tmp75E->tvs=0;
if(_tmp75E->defn != 0)
({void*_tmpB9B=Cyc_Toc_typ_to_c((void*)_check_null(_tmp75E->defn));_tmp75E->defn=_tmpB9B;});else{
# 4701
enum Cyc_Absyn_KindQual _tmp756=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp75E->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp757=_tmp756;if(_tmp757 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB9C=Cyc_Absyn_void_star_typ();_tmp75E->defn=_tmpB9C;});goto _LL1F;}else{_LL22: _LL23:
 _tmp75E->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 4709
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB9D=({struct Cyc_List_List*_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758->hd=d,_tmp758->tl=Cyc_Toc_result_decls;_tmp758;});Cyc_Toc_result_decls=_tmpB9D;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp75F=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp74A)->f2;_LL18:
 _tmp760=_tmp75F;goto _LL1A;case 10U: _LL19: _tmp760=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp74A)->f2;_LL1A:
 _tmp761=_tmp760;goto _LL1C;case 11U: _LL1B: _tmp761=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpB9E=Cyc_Toc_decls_to_c(r,nv,_tmp761,top,cinclude);nv=_tmpB9E;});goto _LL0;default: _LL1D: _tmp762=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp74A)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpB9F=Cyc_Toc_decls_to_c(r,nv,_tmp762,top,1);nv=_tmpB9F;});goto _LL0;}_LL0:;};}
# 4720
return nv;}
# 4724
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp763=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp764=_tmp763;struct Cyc_Core_DynamicRegion*_tmp769;_LL1: _tmp769=_tmp764.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp769->h;
({struct Cyc_Toc_TocState*_tmpBA0=Cyc_Toc_empty_toc_state(h);ts=_tmpBA0;});;}
({struct Cyc_Toc_TocStateWrap*_tmpBA1=({struct Cyc_Toc_TocStateWrap*_tmp766=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp766));_tmp766->dyn=_tmp769,_tmp766->state=ts;_tmp766;});Cyc_Toc_toc_state=_tmpBA1;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpBA2=_tag_dyneither(({unsigned int _tmp768=35;struct _dyneither_ptr**_tmp767=_cycalloc(_check_times(_tmp768,sizeof(struct _dyneither_ptr*)));_tmp767[0]=& Cyc_Toc__throw_str,_tmp767[1]=& Cyc_Toc_setjmp_str,_tmp767[2]=& Cyc_Toc__push_handler_str,_tmp767[3]=& Cyc_Toc__pop_handler_str,_tmp767[4]=& Cyc_Toc__exn_thrown_str,_tmp767[5]=& Cyc_Toc__npop_handler_str,_tmp767[6]=& Cyc_Toc__check_null_str,_tmp767[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp767[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp767[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp767[10]=& Cyc_Toc__dyneither_ptr_str,_tmp767[11]=& Cyc_Toc__tag_dyneither_str,_tmp767[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp767[13]=& Cyc_Toc__get_dyneither_size_str,_tmp767[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp767[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp767[16]=& Cyc_Toc__zero_arr_plus_str,_tmp767[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp767[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp767[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp767[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp767[21]=& Cyc_Toc__cycalloc_str,_tmp767[22]=& Cyc_Toc__cyccalloc_str,_tmp767[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp767[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp767[25]=& Cyc_Toc__region_malloc_str,_tmp767[26]=& Cyc_Toc__region_calloc_str,_tmp767[27]=& Cyc_Toc__check_times_str,_tmp767[28]=& Cyc_Toc__new_region_str,_tmp767[29]=& Cyc_Toc__push_region_str,_tmp767[30]=& Cyc_Toc__pop_region_str,_tmp767[31]=& Cyc_Toc__throw_arraybounds_str,_tmp767[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp767[33]=& Cyc_Toc__throw_match_str,_tmp767[34]=& Cyc_Toc__fast_region_malloc_str;_tmp767;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpBA2;});};}
# 4775
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp76A=ts;struct Cyc_Toc_TocStateWrap*_tmp76B=Cyc_Toc_toc_state;ts=_tmp76B;Cyc_Toc_toc_state=_tmp76A;});{
struct Cyc_Toc_TocStateWrap _tmp76C=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp76D=_tmp76C;struct Cyc_Core_DynamicRegion*_tmp773;struct Cyc_Toc_TocState*_tmp772;_LL1: _tmp773=_tmp76D.dyn;_tmp772=_tmp76D.state;_LL2:;
# 4780
{struct _RegionHandle*h=& _tmp773->h;
{struct Cyc_Toc_TocState _tmp76F=*_tmp772;struct Cyc_Toc_TocState _tmp770=_tmp76F;struct Cyc_Xarray_Xarray*_tmp771;_LL4: _tmp771=_tmp770.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp771);}
# 4781
;}
# 4784
Cyc_Core_free_rckey(_tmp773);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4787
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4793
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4795
({struct Cyc_Hashtable_Table**_tmpBA3=({struct Cyc_Hashtable_Table**_tmp774=_cycalloc(sizeof(*_tmp774));*_tmp774=pop_tables;_tmp774;});Cyc_Toc_gpop_tables=_tmpBA3;});
Cyc_Toc_init();
{struct _RegionHandle _tmp775=_new_region("start");struct _RegionHandle*start=& _tmp775;_push_region(start);
({struct _RegionHandle*_tmpBA5=start;struct Cyc_Toc_Env*_tmpBA4=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpBA5,_tmpBA4,ds,1,0);});;_pop_region(start);}
# 4804
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
