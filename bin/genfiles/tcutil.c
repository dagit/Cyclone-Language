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
if(({struct _dyneither_ptr _tmpAB7=({const char*_tmp0="(qualifiers don't match)";_tag_dyneither(_tmp0,sizeof(char),25U);});Cyc_strcmp(_tmpAB7,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmpA29;_tmpA29.tag=0U,_tmpA29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmpA29;});void*_tmp1[1U];_tmp1[0]=& _tmp3;({struct Cyc___cycFILE*_tmpAB9=Cyc_stderr;struct _dyneither_ptr _tmpAB8=({const char*_tmp2="  %s\n";_tag_dyneither(_tmp2,sizeof(char),6U);});Cyc_fprintf(_tmpAB9,_tmpAB8,_tag_dyneither(_tmp1,sizeof(void*),1U));});});
return;}
# 68
if(({struct _dyneither_ptr _tmpABA=({const char*_tmp4="(function effects do not match)";_tag_dyneither(_tmp4,sizeof(char),32U);});Cyc_strcmp(_tmpABA,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp5=Cyc_Absynpp_tc_params_r;
_tmp5.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp5);}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmpA2A;_tmpA2A.tag=0U,_tmpA2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmpA2A;});void*_tmp6[1U];_tmp6[0]=& _tmp8;({struct Cyc___cycFILE*_tmpABC=Cyc_stderr;struct _dyneither_ptr _tmpABB=({const char*_tmp7="  %s";_tag_dyneither(_tmp7,sizeof(char),5U);});Cyc_fprintf(_tmpABC,_tmpABB,_tag_dyneither(_tmp6,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp9=0U;({struct Cyc___cycFILE*_tmpABE=Cyc_stderr;struct _dyneither_ptr _tmpABD=({const char*_tmpA="\n\t";_tag_dyneither(_tmpA,sizeof(char),3U);});Cyc_fprintf(_tmpABE,_tmpABD,_tag_dyneither(_tmp9,sizeof(void*),0U));});});
pos=8;}else{
# 82
({void*_tmpB=0U;({struct Cyc___cycFILE*_tmpAC0=Cyc_stderr;struct _dyneither_ptr _tmpABF=({const char*_tmpC=" ";_tag_dyneither(_tmpC,sizeof(char),2U);});Cyc_fprintf(_tmpAC0,_tmpABF,_tag_dyneither(_tmpB,sizeof(void*),0U));});});
++ pos;}
# 85
({void*_tmpD=0U;({struct Cyc___cycFILE*_tmpAC2=Cyc_stderr;struct _dyneither_ptr _tmpAC1=({const char*_tmpE="and ";_tag_dyneither(_tmpE,sizeof(char),5U);});Cyc_fprintf(_tmpAC2,_tmpAC1,_tag_dyneither(_tmpD,sizeof(void*),0U));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmpF=0U;({struct Cyc___cycFILE*_tmpAC4=Cyc_stderr;struct _dyneither_ptr _tmpAC3=({const char*_tmp10="\n\t";_tag_dyneither(_tmp10,sizeof(char),3U);});Cyc_fprintf(_tmpAC4,_tmpAC3,_tag_dyneither(_tmpF,sizeof(void*),0U));});});
pos=8;}
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpA2B;_tmpA2B.tag=0U,_tmpA2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmpA2B;});void*_tmp11[1U];_tmp11[0]=& _tmp13;({struct Cyc___cycFILE*_tmpAC6=Cyc_stderr;struct _dyneither_ptr _tmpAC5=({const char*_tmp12="%s ";_tag_dyneither(_tmp12,sizeof(char),4U);});Cyc_fprintf(_tmpAC6,_tmpAC5,_tag_dyneither(_tmp11,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp14=0U;({struct Cyc___cycFILE*_tmpAC8=Cyc_stderr;struct _dyneither_ptr _tmpAC7=({const char*_tmp15="\n\t";_tag_dyneither(_tmp15,sizeof(char),3U);});Cyc_fprintf(_tmpAC8,_tmpAC7,_tag_dyneither(_tmp14,sizeof(void*),0U));});});
pos=8;}
# 97
({void*_tmp16=0U;({struct Cyc___cycFILE*_tmpACA=Cyc_stderr;struct _dyneither_ptr _tmpAC9=({const char*_tmp17="are not compatible. ";_tag_dyneither(_tmp17,sizeof(char),21U);});Cyc_fprintf(_tmpACA,_tmpAC9,_tag_dyneither(_tmp16,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpACB=(char*)Cyc_Tcutil_failure_reason.curr;_tmpACB != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({int _tmpACC=pos;_tmpACC + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80)
({void*_tmp18=0U;({struct Cyc___cycFILE*_tmpACE=Cyc_stderr;struct _dyneither_ptr _tmpACD=({const char*_tmp19="\n\t";_tag_dyneither(_tmp19,sizeof(char),3U);});Cyc_fprintf(_tmpACE,_tmpACD,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmpA2C;_tmpA2C.tag=0U,_tmpA2C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmpA2C;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct Cyc___cycFILE*_tmpAD0=Cyc_stderr;struct _dyneither_ptr _tmpACF=({const char*_tmp1B="%s";_tag_dyneither(_tmp1B,sizeof(char),3U);});Cyc_fprintf(_tmpAD0,_tmpACF,_tag_dyneither(_tmp1A,sizeof(void*),1U));});});}
# 105
({void*_tmp1D=0U;({struct Cyc___cycFILE*_tmpAD2=Cyc_stderr;struct _dyneither_ptr _tmpAD1=({const char*_tmp1E="\n";_tag_dyneither(_tmp1E,sizeof(char),2U);});Cyc_fprintf(_tmpAD2,_tmpAD1,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 111
Cyc_Position_post_error(({unsigned int _tmpAD3=loc;Cyc_Position_mk_err(_tmpAD3,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 113
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 116
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
if(!Cyc_Position_error_p()){
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmpA2D;_tmpA2D.tag=0U,_tmpA2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpA2D;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({struct Cyc___cycFILE*_tmpAD5=Cyc_stderr;struct _dyneither_ptr _tmpAD4=({const char*_tmp20="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp20,sizeof(char),36U);});Cyc_fprintf(_tmpAD5,_tmpAD4,_tag_dyneither(_tmp1F,sizeof(void*),1U));});});
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
return({struct Cyc_Absyn_Tvar*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->name=tv->name,_tmp35->identity=- 1,({void*_tmpAD6=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp35->kind=_tmpAD6;});_tmp35;});}
# 191
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 193
struct _tuple10*_tmp36=arg;struct _dyneither_ptr*_tmp3A;struct Cyc_Absyn_Tqual _tmp39;void*_tmp38;_LL1: _tmp3A=_tmp36->f1;_tmp39=_tmp36->f2;_tmp38=_tmp36->f3;_LL2:;
return({struct _tuple10*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=_tmp3A,_tmp37->f2=_tmp39,({void*_tmpAD7=Cyc_Tcutil_copy_type(_tmp38);_tmp37->f3=_tmpAD7;});_tmp37;});}
# 196
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3B=arg;struct Cyc_Absyn_Tqual _tmp3E;void*_tmp3D;_LL1: _tmp3E=_tmp3B->f1;_tmp3D=_tmp3B->f2;_LL2:;
return({struct _tuple12*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=_tmp3E,({void*_tmpAD8=Cyc_Tcutil_copy_type(_tmp3D);_tmp3C->f2=_tmpAD8;});_tmp3C;});}
# 200
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 202
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=f->name,_tmp3F->tq=f->tq,({void*_tmpADA=Cyc_Tcutil_copy_type(f->type);_tmp3F->type=_tmpADA;}),_tmp3F->width=f->width,_tmp3F->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpAD9=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp3F->requires_clause=_tmpAD9;});_tmp3F;});}
# 206
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp40=x;void*_tmp43;void*_tmp42;_LL1: _tmp43=_tmp40->f1;_tmp42=_tmp40->f2;_LL2:;
return({struct _tuple0*_tmp41=_cycalloc(sizeof(*_tmp41));({void*_tmpADC=Cyc_Tcutil_copy_type(_tmp43);_tmp41->f1=_tmpADC;}),({void*_tmpADB=Cyc_Tcutil_copy_type(_tmp42);_tmp41->f2=_tmpADB;});_tmp41;});}
# 210
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->name=f->name,_tmp44->tag=f->tag,_tmp44->loc=f->loc;_tmp44;});}
# 213
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->tag=2U,({struct Cyc_Absyn_Tvar*_tmpADD=Cyc_Tcutil_copy_tvar(t);_tmp45->f1=_tmpADD;});_tmp45;});}
# 217
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp46=Cyc_Tcutil_compress(t);void*_tmp47=_tmp46;struct Cyc_Absyn_Datatypedecl*_tmpAB;struct Cyc_Absyn_Enumdecl*_tmpAA;struct Cyc_Absyn_Aggrdecl*_tmpA9;void*_tmpA8;struct Cyc_List_List*_tmpA7;void*_tmpA6;struct _tuple2*_tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_Absyn_Typedefdecl*_tmpA3;void*_tmpA2;void*_tmpA1;void*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;void*_tmp9D;struct Cyc_List_List*_tmp9C;struct _tuple2*_tmp9B;struct Cyc_Absyn_Enumdecl*_tmp9A;enum Cyc_Absyn_AggrKind _tmp99;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Aggrdecl**_tmp97;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp95;struct _tuple2*_tmp94;struct Cyc_Core_Opt*_tmp93;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp90;void*_tmp8F;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8D;struct Cyc_List_List*_tmp8C;int _tmp8B;struct Cyc_Absyn_VarargInfo*_tmp8A;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_List_List*_tmp84;void*_tmp83;struct Cyc_Absyn_Tqual _tmp82;struct Cyc_Absyn_Exp*_tmp81;union Cyc_Absyn_Constraint*_tmp80;unsigned int _tmp7F;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7D;void*_tmp7C;union Cyc_Absyn_Constraint*_tmp7B;union Cyc_Absyn_Constraint*_tmp7A;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp78;union Cyc_Absyn_DatatypeFieldInfoU _tmp77;struct Cyc_List_List*_tmp76;union Cyc_Absyn_DatatypeInfoU _tmp75;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Tvar*_tmp73;switch(*((int*)_tmp47)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return t;case 2U: _LL5: _tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp47)->f1;_LL6:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=2U,({struct Cyc_Absyn_Tvar*_tmpADE=Cyc_Tcutil_copy_tvar(_tmp73);_tmp48->f1=_tmpADE;});_tmp48;});case 3U: _LL7: _tmp75=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47)->f1).datatype_info;_tmp74=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL8:
# 223
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=3U,(_tmp49->f1).datatype_info=_tmp75,({struct Cyc_List_List*_tmpADF=Cyc_Tcutil_copy_types(_tmp74);(_tmp49->f1).targs=_tmpADF;});_tmp49;});case 4U: _LL9: _tmp77=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47)->f1).field_info;_tmp76=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47)->f1).targs;_LLA:
# 225
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=4U,(_tmp4A->f1).field_info=_tmp77,({struct Cyc_List_List*_tmpAE0=Cyc_Tcutil_copy_types(_tmp76);(_tmp4A->f1).targs=_tmpAE0;});_tmp4A;});case 5U: _LLB: _tmp7E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).elt_typ;_tmp7D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).elt_tq;_tmp7C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).rgn;_tmp7B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).nullable;_tmp7A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).bounds;_tmp79=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).zero_term;_tmp78=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47)->f1).ptr_atts).ptrloc;_LLC: {
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
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=8U,({void*_tmpAE2=Cyc_Tcutil_copy_type(_tmp83);(_tmp52->f1).elt_type=_tmpAE2;}),(_tmp52->f1).tq=_tmp82,(_tmp52->f1).num_elts=eopt2,({
union Cyc_Absyn_Constraint*_tmpAE1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp80);(_tmp52->f1).zero_term=_tmpAE1;}),(_tmp52->f1).zt_loc=_tmp7F;_tmp52;});}case 9U: _LL13: _tmp90=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).tvars;_tmp8F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).effect;_tmp8E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ret_tqual;_tmp8D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ret_typ;_tmp8C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).args;_tmp8B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).c_varargs;_tmp8A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).cyc_varargs;_tmp89=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).rgn_po;_tmp88=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).attributes;_tmp87=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).requires_clause;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).requires_relns;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ensures_clause;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47)->f1).ensures_relns;_LL14: {
# 242
struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp90);
void*effopt2=_tmp8F == 0?0: Cyc_Tcutil_copy_type(_tmp8F);
void*_tmp54=Cyc_Tcutil_copy_type(_tmp8D);
struct Cyc_List_List*_tmp55=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp8C);
int _tmp56=_tmp8B;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp8A != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp8A;
({struct Cyc_Absyn_VarargInfo*_tmpAE4=({struct Cyc_Absyn_VarargInfo*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->name=cv->name,_tmp57->tq=cv->tq,({void*_tmpAE3=Cyc_Tcutil_copy_type(cv->type);_tmp57->type=_tmpAE3;}),_tmp57->inject=cv->inject;_tmp57;});cyc_varargs2=_tmpAE4;});}{
# 253
struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp89);
struct Cyc_List_List*_tmp59=_tmp88;
struct Cyc_Absyn_Exp*_tmp5A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp87);
struct Cyc_List_List*_tmp5B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp86);
struct Cyc_Absyn_Exp*_tmp5C=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp85);
struct Cyc_List_List*_tmp5D=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp84);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=9U,(_tmp5E->f1).tvars=_tmp53,(_tmp5E->f1).effect=effopt2,(_tmp5E->f1).ret_tqual=_tmp8E,(_tmp5E->f1).ret_typ=_tmp54,(_tmp5E->f1).args=_tmp55,(_tmp5E->f1).c_varargs=_tmp56,(_tmp5E->f1).cyc_varargs=cyc_varargs2,(_tmp5E->f1).rgn_po=_tmp58,(_tmp5E->f1).attributes=_tmp59,(_tmp5E->f1).requires_clause=_tmp5A,(_tmp5E->f1).requires_relns=_tmp5B,(_tmp5E->f1).ensures_clause=_tmp5C,(_tmp5E->f1).ensures_relns=_tmp5D;_tmp5E;});};}case 10U: _LL15: _tmp91=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp47)->f1;_LL16:
# 262
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=10U,({struct Cyc_List_List*_tmpAE5=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp91);_tmp5F->f1=_tmpAE5;});_tmp5F;});case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).tag == 1){_LL17: _tmp95=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f1;_tmp94=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f2;_tmp93=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).UnknownAggr).val).f3;_tmp92=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL18:
# 264
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpAE7=Cyc_Absyn_UnknownAggr(_tmp95,_tmp94,_tmp93);(_tmp60->f1).aggr_info=_tmpAE7;}),({struct Cyc_List_List*_tmpAE6=Cyc_Tcutil_copy_types(_tmp92);(_tmp60->f1).targs=_tmpAE6;});_tmp60;});}else{_LL19: _tmp97=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).aggr_info).KnownAggr).val;_tmp96=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47)->f1).targs;_LL1A:
# 266
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpAE9=Cyc_Absyn_KnownAggr(_tmp97);(_tmp61->f1).aggr_info=_tmpAE9;}),({struct Cyc_List_List*_tmpAE8=Cyc_Tcutil_copy_types(_tmp96);(_tmp61->f1).targs=_tmpAE8;});_tmp61;});}case 12U: _LL1B: _tmp99=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47)->f1;_tmp98=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47)->f2;_LL1C:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=12U,_tmp62->f1=_tmp99,({struct Cyc_List_List*_tmpAEA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp98);_tmp62->f2=_tmpAEA;});_tmp62;});case 13U: _LL1D: _tmp9B=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47)->f1;_tmp9A=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47)->f2;_LL1E:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=13U,_tmp63->f1=_tmp9B,_tmp63->f2=_tmp9A;_tmp63;});case 14U: _LL1F: _tmp9C=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp47)->f1;_LL20:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=14U,({struct Cyc_List_List*_tmpAEB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9C);_tmp64->f1=_tmpAEB;});_tmp64;});case 19U: _LL21: _tmp9D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp47)->f1;_LL22:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=19U,({void*_tmpAEC=Cyc_Tcutil_copy_type(_tmp9D);_tmp65->f1=_tmpAEC;});_tmp65;});case 18U: _LL23: _tmp9E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp47)->f1;_LL24:
# 273
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=18U,_tmp66->f1=_tmp9E;_tmp66;});case 27U: _LL25: _tmp9F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp47)->f1;_LL26:
# 276
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=27U,_tmp67->f1=_tmp9F;_tmp67;});case 15U: _LL27: _tmpA0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp47)->f1;_LL28:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=15U,({void*_tmpAED=Cyc_Tcutil_copy_type(_tmpA0);_tmp68->f1=_tmpAED;});_tmp68;});case 16U: _LL29: _tmpA2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47)->f1;_tmpA1=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47)->f2;_LL2A:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=16U,({void*_tmpAEF=Cyc_Tcutil_copy_type(_tmpA2);_tmp69->f1=_tmpAEF;}),({void*_tmpAEE=Cyc_Tcutil_copy_type(_tmpA1);_tmp69->f2=_tmpAEE;});_tmp69;});case 17U: _LL2B: _tmpA5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f1;_tmpA4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f2;_tmpA3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47)->f3;_LL2C:
# 280
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=17U,_tmp6A->f1=_tmpA5,({struct Cyc_List_List*_tmpAF0=Cyc_Tcutil_copy_types(_tmpA4);_tmp6A->f2=_tmpAF0;}),_tmp6A->f3=_tmpA3,_tmp6A->f4=0;_tmp6A;});case 21U: _LL2D: _LL2E:
 goto _LL30;case 22U: _LL2F: _LL30:
 goto _LL32;case 28U: _LL31: _LL32:
 goto _LL34;case 20U: _LL33: _LL34:
 return t;case 23U: _LL35: _tmpA6=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp47)->f1;_LL36:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=23U,({void*_tmpAF1=Cyc_Tcutil_copy_type(_tmpA6);_tmp6B->f1=_tmpAF1;});_tmp6B;});case 24U: _LL37: _tmpA7=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp47)->f1;_LL38:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=24U,({struct Cyc_List_List*_tmpAF2=Cyc_Tcutil_copy_types(_tmpA7);_tmp6C->f1=_tmpAF2;});_tmp6C;});case 25U: _LL39: _tmpA8=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp47)->f1;_LL3A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=25U,({void*_tmpAF3=Cyc_Tcutil_copy_type(_tmpA8);_tmp6D->f1=_tmpAF3;});_tmp6D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)){case 0U: _LL3B: _tmpA9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL3C: {
# 290
struct Cyc_List_List*_tmp6E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpA9->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpAF4=Cyc_Absyn_UnknownAggr(_tmpA9->kind,_tmpA9->name,0);(_tmp6F->f1).aggr_info=_tmpAF4;}),(_tmp6F->f1).targs=_tmp6E;_tmp6F;});}case 1U: _LL3D: _tmpAA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL3E:
# 293
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=13U,_tmp70->f1=_tmpAA->name,_tmp70->f2=0;_tmp70;});default: _LL3F: _tmpAB=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47)->f1)->r)->f1;_LL40: {
# 295
struct Cyc_List_List*_tmp71=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAB->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=3U,({union Cyc_Absyn_DatatypeInfoU _tmpAF5=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmpA2E;_tmpA2E.name=_tmpAB->name,_tmpA2E.is_extensible=0;_tmpA2E;}));(_tmp72->f1).datatype_info=_tmpAF5;}),(_tmp72->f1).targs=_tmp71;_tmp72;});}}}_LL0:;}
# 310 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpAC=d;struct _dyneither_ptr*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC)->tag == 0U){_LL1: _tmpAE=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAC)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=0U,({struct Cyc_Absyn_Exp*_tmpAF6=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpAE);_tmpAD->f1=_tmpAF6;});_tmpAD;});}else{_LL3: _tmpAF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpAC)->f1;_LL4:
 return d;}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 316
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 318
return({struct _tuple19*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct Cyc_List_List*_tmpAF8=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB0->f1=_tmpAF8;}),({struct Cyc_Absyn_Exp*_tmpAF7=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB0->f2=_tmpAF7;});_tmpB0;});}
# 321
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB1=preserve_types;
{void*_tmpB2=e->r;void*_tmpB3=_tmpB2;int _tmp12F;struct _dyneither_ptr _tmp12E;void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct _dyneither_ptr*_tmp12B;struct Cyc_Core_Opt*_tmp12A;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;int _tmp126;struct Cyc_Absyn_Exp*_tmp125;void**_tmp124;struct Cyc_Absyn_Exp*_tmp123;int _tmp122;int _tmp121;void*_tmp120;struct Cyc_Absyn_Enumfield*_tmp11F;struct Cyc_Absyn_Enumdecl*_tmp11E;struct Cyc_Absyn_Enumfield*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Datatypedecl*_tmp11B;struct Cyc_Absyn_Datatypefield*_tmp11A;void*_tmp119;struct Cyc_List_List*_tmp118;struct _tuple2*_tmp117;struct Cyc_List_List*_tmp116;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Aggrdecl*_tmp114;struct Cyc_Absyn_Exp*_tmp113;void*_tmp112;int _tmp111;struct Cyc_Absyn_Vardecl*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;int _tmp10D;struct Cyc_List_List*_tmp10C;struct _dyneither_ptr*_tmp10B;struct Cyc_Absyn_Tqual _tmp10A;void*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct _dyneither_ptr*_tmp103;int _tmp102;int _tmp101;struct Cyc_Absyn_Exp*_tmp100;struct _dyneither_ptr*_tmpFF;int _tmpFE;int _tmpFD;struct Cyc_Absyn_Exp*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;void*_tmpF4;struct Cyc_Absyn_Exp*_tmpF3;int _tmpF2;enum Cyc_Absyn_Coercion _tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpED;int _tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargCallInfo*_tmpE9;int _tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;enum Cyc_Absyn_Incrementor _tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Core_Opt*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;enum Cyc_Absyn_Primop _tmpD9;struct Cyc_List_List*_tmpD8;struct _dyneither_ptr _tmpD7;void*_tmpD6;union Cyc_Absyn_Cnst _tmpD5;switch(*((int*)_tmpB3)){case 0U: _LL1: _tmpD5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL2:
({struct Cyc_Absyn_Exp*_tmpAF9=Cyc_Absyn_const_exp(_tmpD5,e->loc);new_e=_tmpAF9;});goto _LL0;case 1U: _LL3: _tmpD6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpAFA=Cyc_Absyn_varb_exp(_tmpD6,e->loc);new_e=_tmpAFA;});goto _LL0;case 2U: _LL5: _tmpD7=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Absyn_pragma_exp(_tmpD7,e->loc);new_e=_tmpAFB;});goto _LL0;case 3U: _LL7: _tmpD9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpD8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL8:
({struct Cyc_Absyn_Exp*_tmpAFE=({enum Cyc_Absyn_Primop _tmpAFD=_tmpD9;struct Cyc_List_List*_tmpAFC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpD8);Cyc_Absyn_primop_exp(_tmpAFD,_tmpAFC,e->loc);});new_e=_tmpAFE;});goto _LL0;case 4U: _LL9: _tmpDC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpDB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpDA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LLA:
# 330
({struct Cyc_Absyn_Exp*_tmpB02=({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDC);struct Cyc_Core_Opt*_tmpB00=(unsigned int)_tmpDB?({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->v=(void*)_tmpDB->v;_tmpB4;}): 0;struct Cyc_Absyn_Exp*_tmpAFF=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDA);Cyc_Absyn_assignop_exp(_tmpB01,_tmpB00,_tmpAFF,e->loc);});new_e=_tmpB02;});
goto _LL0;case 5U: _LLB: _tmpDE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpDD=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LLC:
({struct Cyc_Absyn_Exp*_tmpB05=({struct Cyc_Absyn_Exp*_tmpB04=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDE);enum Cyc_Absyn_Incrementor _tmpB03=_tmpDD;Cyc_Absyn_increment_exp(_tmpB04,_tmpB03,e->loc);});new_e=_tmpB05;});goto _LL0;case 6U: _LLD: _tmpE1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpDF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LLE:
# 334
({struct Cyc_Absyn_Exp*_tmpB09=({struct Cyc_Absyn_Exp*_tmpB08=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE1);struct Cyc_Absyn_Exp*_tmpB07=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE0);struct Cyc_Absyn_Exp*_tmpB06=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpDF);Cyc_Absyn_conditional_exp(_tmpB08,_tmpB07,_tmpB06,e->loc);});new_e=_tmpB09;});goto _LL0;case 7U: _LLF: _tmpE3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL10:
({struct Cyc_Absyn_Exp*_tmpB0C=({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE3);struct Cyc_Absyn_Exp*_tmpB0A=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE2);Cyc_Absyn_and_exp(_tmpB0B,_tmpB0A,e->loc);});new_e=_tmpB0C;});goto _LL0;case 8U: _LL11: _tmpE5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL12:
({struct Cyc_Absyn_Exp*_tmpB0F=({struct Cyc_Absyn_Exp*_tmpB0E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE5);struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE4);Cyc_Absyn_or_exp(_tmpB0E,_tmpB0D,e->loc);});new_e=_tmpB0F;});goto _LL0;case 9U: _LL13: _tmpE7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpE6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL14:
({struct Cyc_Absyn_Exp*_tmpB12=({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE7);struct Cyc_Absyn_Exp*_tmpB10=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpE6);Cyc_Absyn_seq_exp(_tmpB11,_tmpB10,e->loc);});new_e=_tmpB12;});goto _LL0;case 10U: _LL15: _tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpE9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpE8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL16:
# 339
{struct Cyc_Absyn_VarargCallInfo*_tmpB5=_tmpE9;int _tmpC1;struct Cyc_List_List*_tmpC0;struct Cyc_Absyn_VarargInfo*_tmpBF;if(_tmpB5 != 0){_LL54: _tmpC1=_tmpB5->num_varargs;_tmpC0=_tmpB5->injectors;_tmpBF=_tmpB5->vai;_LL55: {
# 341
struct Cyc_Absyn_VarargInfo*_tmpB6=_tmpBF;struct _dyneither_ptr*_tmpBD;struct Cyc_Absyn_Tqual _tmpBC;void*_tmpBB;int _tmpBA;_LL59: _tmpBD=_tmpB6->name;_tmpBC=_tmpB6->tq;_tmpBB=_tmpB6->type;_tmpBA=_tmpB6->inject;_LL5A:;
({struct Cyc_Absyn_Exp*_tmpB19=({void*_tmpB18=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->tag=10U,({
struct Cyc_Absyn_Exp*_tmpB17=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEB);_tmpB9->f1=_tmpB17;}),({struct Cyc_List_List*_tmpB16=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpEA);_tmpB9->f2=_tmpB16;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpB15=({struct Cyc_Absyn_VarargCallInfo*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->num_varargs=_tmpC1,_tmpB8->injectors=_tmpC0,({
struct Cyc_Absyn_VarargInfo*_tmpB14=({struct Cyc_Absyn_VarargInfo*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->name=_tmpBD,_tmpB7->tq=_tmpBC,({void*_tmpB13=Cyc_Tcutil_copy_type(_tmpBB);_tmpB7->type=_tmpB13;}),_tmpB7->inject=_tmpBA;_tmpB7;});_tmpB8->vai=_tmpB14;});_tmpB8;});
# 344
_tmpB9->f3=_tmpB15;}),_tmpB9->f4=_tmpE8;_tmpB9;});
# 342
Cyc_Absyn_new_exp(_tmpB18,e->loc);});new_e=_tmpB19;});
# 347
goto _LL53;}}else{_LL56: _LL57:
# 349
({struct Cyc_Absyn_Exp*_tmpB1D=({void*_tmpB1C=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=10U,({struct Cyc_Absyn_Exp*_tmpB1B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEB);_tmpBE->f1=_tmpB1B;}),({struct Cyc_List_List*_tmpB1A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmpEA);_tmpBE->f2=_tmpB1A;}),_tmpBE->f3=_tmpE9,_tmpBE->f4=_tmpE8;_tmpBE;});Cyc_Absyn_new_exp(_tmpB1C,e->loc);});new_e=_tmpB1D;});
goto _LL53;}_LL53:;}
# 352
goto _LL0;case 11U: _LL17: _tmpED=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL18:
# 354
({struct Cyc_Absyn_Exp*_tmpB20=_tmpEC?({struct Cyc_Absyn_Exp*_tmpB1F=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpED);Cyc_Absyn_rethrow_exp(_tmpB1F,e->loc);}):({struct Cyc_Absyn_Exp*_tmpB1E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpED);Cyc_Absyn_throw_exp(_tmpB1E,e->loc);});new_e=_tmpB20;});
goto _LL0;case 12U: _LL19: _tmpEE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL1A:
({struct Cyc_Absyn_Exp*_tmpB22=({struct Cyc_Absyn_Exp*_tmpB21=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpEE);Cyc_Absyn_noinstantiate_exp(_tmpB21,e->loc);});new_e=_tmpB22;});
goto _LL0;case 13U: _LL1B: _tmpF0=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpEF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL1C:
# 359
({struct Cyc_Absyn_Exp*_tmpB25=({struct Cyc_Absyn_Exp*_tmpB24=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF0);struct Cyc_List_List*_tmpB23=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmpEF);Cyc_Absyn_instantiate_exp(_tmpB24,_tmpB23,e->loc);});new_e=_tmpB25;});
goto _LL0;case 14U: _LL1D: _tmpF4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpF3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpF2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpF1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL1E:
# 362
({struct Cyc_Absyn_Exp*_tmpB2A=({void*_tmpB29=Cyc_Tcutil_copy_type(_tmpF4);struct Cyc_Absyn_Exp*_tmpB28=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF3);int _tmpB27=_tmpF2;enum Cyc_Absyn_Coercion _tmpB26=_tmpF1;Cyc_Absyn_cast_exp(_tmpB29,_tmpB28,_tmpB27,_tmpB26,e->loc);});new_e=_tmpB2A;});goto _LL0;case 15U: _LL1F: _tmpF5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL20:
({struct Cyc_Absyn_Exp*_tmpB2C=({struct Cyc_Absyn_Exp*_tmpB2B=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF5);Cyc_Absyn_address_exp(_tmpB2B,e->loc);});new_e=_tmpB2C;});goto _LL0;case 16U: _LL21: _tmpF7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpF6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL22: {
# 365
struct Cyc_Absyn_Exp*eo1=_tmpF7;if(_tmpF7 != 0)({struct Cyc_Absyn_Exp*_tmpB2D=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF7);eo1=_tmpB2D;});
({struct Cyc_Absyn_Exp*_tmpB30=({struct Cyc_Absyn_Exp*_tmpB2F=eo1;struct Cyc_Absyn_Exp*_tmpB2E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF6);Cyc_Absyn_New_exp(_tmpB2F,_tmpB2E,e->loc);});new_e=_tmpB30;});
goto _LL0;}case 17U: _LL23: _tmpF8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL24:
({struct Cyc_Absyn_Exp*_tmpB32=({void*_tmpB31=Cyc_Tcutil_copy_type(_tmpF8);Cyc_Absyn_sizeoftyp_exp(_tmpB31,e->loc);});new_e=_tmpB32;});
goto _LL0;case 18U: _LL25: _tmpF9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL26:
({struct Cyc_Absyn_Exp*_tmpB34=({struct Cyc_Absyn_Exp*_tmpB33=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpF9);Cyc_Absyn_sizeofexp_exp(_tmpB33,e->loc);});new_e=_tmpB34;});goto _LL0;case 19U: _LL27: _tmpFB=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpFA=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL28:
# 372
({struct Cyc_Absyn_Exp*_tmpB37=({void*_tmpB36=Cyc_Tcutil_copy_type(_tmpFB);struct Cyc_List_List*_tmpB35=_tmpFA;Cyc_Absyn_offsetof_exp(_tmpB36,_tmpB35,e->loc);});new_e=_tmpB37;});goto _LL0;case 20U: _LL29: _tmpFC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL2A:
({struct Cyc_Absyn_Exp*_tmpB39=({struct Cyc_Absyn_Exp*_tmpB38=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmpFC);Cyc_Absyn_deref_exp(_tmpB38,e->loc);});new_e=_tmpB39;});goto _LL0;case 21U: _LL2B: _tmp100=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmpFF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmpFE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmpFD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL2C:
# 375
({struct Cyc_Absyn_Exp*_tmpB3C=({void*_tmpB3B=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=21U,({struct Cyc_Absyn_Exp*_tmpB3A=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp100);_tmpC2->f1=_tmpB3A;}),_tmpC2->f2=_tmpFF,_tmpC2->f3=_tmpFE,_tmpC2->f4=_tmpFD;_tmpC2;});Cyc_Absyn_new_exp(_tmpB3B,e->loc);});new_e=_tmpB3C;});goto _LL0;case 22U: _LL2D: _tmp104=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp103=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp102=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp101=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL2E:
# 377
({struct Cyc_Absyn_Exp*_tmpB3F=({void*_tmpB3E=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=22U,({struct Cyc_Absyn_Exp*_tmpB3D=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp104);_tmpC3->f1=_tmpB3D;}),_tmpC3->f2=_tmp103,_tmpC3->f3=_tmp102,_tmpC3->f4=_tmp101;_tmpC3;});Cyc_Absyn_new_exp(_tmpB3E,e->loc);});new_e=_tmpB3F;});goto _LL0;case 23U: _LL2F: _tmp106=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp105=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL30:
({struct Cyc_Absyn_Exp*_tmpB42=({struct Cyc_Absyn_Exp*_tmpB41=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp106);struct Cyc_Absyn_Exp*_tmpB40=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp105);Cyc_Absyn_subscript_exp(_tmpB41,_tmpB40,e->loc);});new_e=_tmpB42;});
goto _LL0;case 24U: _LL31: _tmp107=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL32:
({struct Cyc_Absyn_Exp*_tmpB44=({struct Cyc_List_List*_tmpB43=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmp107);Cyc_Absyn_tuple_exp(_tmpB43,e->loc);});new_e=_tmpB44;});goto _LL0;case 25U: _LL33: _tmp10B=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f1;_tmp10A=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f2;_tmp109=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1)->f3;_tmp108=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL34: {
# 382
struct _dyneither_ptr*vopt1=_tmp10B;
if(_tmp10B != 0)vopt1=_tmp10B;
({struct Cyc_Absyn_Exp*_tmpB49=({void*_tmpB48=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->tag=25U,({struct _tuple10*_tmpB47=({struct _tuple10*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->f1=vopt1,_tmpC4->f2=_tmp10A,({void*_tmpB46=Cyc_Tcutil_copy_type(_tmp109);_tmpC4->f3=_tmpB46;});_tmpC4;});_tmpC5->f1=_tmpB47;}),({
struct Cyc_List_List*_tmpB45=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp108);_tmpC5->f2=_tmpB45;});_tmpC5;});
# 384
Cyc_Absyn_new_exp(_tmpB48,e->loc);});new_e=_tmpB49;});
# 386
goto _LL0;}case 26U: _LL35: _tmp10C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL36:
({struct Cyc_Absyn_Exp*_tmpB4C=({void*_tmpB4B=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=26U,({struct Cyc_List_List*_tmpB4A=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp10C);_tmpC6->f1=_tmpB4A;});_tmpC6;});Cyc_Absyn_new_exp(_tmpB4B,e->loc);});new_e=_tmpB4C;});
goto _LL0;case 27U: _LL37: _tmp110=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp10F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp10E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp10D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL38:
# 390
({struct Cyc_Absyn_Exp*_tmpB50=({void*_tmpB4F=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->tag=27U,_tmpC7->f1=_tmp110,({struct Cyc_Absyn_Exp*_tmpB4E=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp10F);_tmpC7->f2=_tmpB4E;}),({struct Cyc_Absyn_Exp*_tmpB4D=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp10E);_tmpC7->f3=_tmpB4D;}),_tmpC7->f4=_tmp10D;_tmpC7;});Cyc_Absyn_new_exp(_tmpB4F,e->loc);});new_e=_tmpB50;});
goto _LL0;case 28U: _LL39: _tmp113=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp112=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp111=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LL3A:
# 393
({struct Cyc_Absyn_Exp*_tmpB54=({void*_tmpB53=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=28U,({struct Cyc_Absyn_Exp*_tmpB52=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp113);_tmpC8->f1=_tmpB52;}),({void*_tmpB51=Cyc_Tcutil_copy_type(_tmp112);_tmpC8->f2=_tmpB51;}),_tmpC8->f3=_tmp111;_tmpC8;});Cyc_Absyn_new_exp(_tmpB53,_tmp113->loc);});new_e=_tmpB54;});
# 395
goto _LL0;case 29U: _LL3B: _tmp117=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp116=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp115=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_tmp114=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB3)->f4;_LL3C:
# 397
({struct Cyc_Absyn_Exp*_tmpB58=({void*_tmpB57=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=29U,_tmpC9->f1=_tmp117,({struct Cyc_List_List*_tmpB56=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp116);_tmpC9->f2=_tmpB56;}),({struct Cyc_List_List*_tmpB55=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp115);_tmpC9->f3=_tmpB55;}),_tmpC9->f4=_tmp114;_tmpC9;});Cyc_Absyn_new_exp(_tmpB57,e->loc);});new_e=_tmpB58;});
# 399
goto _LL0;case 30U: _LL3D: _tmp119=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp118=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL3E:
# 401
({struct Cyc_Absyn_Exp*_tmpB5C=({void*_tmpB5B=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->tag=30U,({void*_tmpB5A=Cyc_Tcutil_copy_type(_tmp119);_tmpCA->f1=_tmpB5A;}),({struct Cyc_List_List*_tmpB59=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp118);_tmpCA->f2=_tmpB59;});_tmpCA;});Cyc_Absyn_new_exp(_tmpB5B,e->loc);});new_e=_tmpB5C;});
goto _LL0;case 31U: _LL3F: _tmp11C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_tmp11A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB3)->f3;_LL40:
# 404
({struct Cyc_Absyn_Exp*_tmpB5F=({void*_tmpB5E=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=31U,({struct Cyc_List_List*_tmpB5D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB1,_tmp11C);_tmpCB->f1=_tmpB5D;}),_tmpCB->f2=_tmp11B,_tmpCB->f3=_tmp11A;_tmpCB;});Cyc_Absyn_new_exp(_tmpB5E,e->loc);});new_e=_tmpB5F;});
goto _LL0;case 32U: _LL41: _tmp11E=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL42:
 new_e=e;goto _LL0;case 33U: _LL43: _tmp120=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp11F=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL44:
# 408
({struct Cyc_Absyn_Exp*_tmpB62=({void*_tmpB61=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->tag=33U,({void*_tmpB60=Cyc_Tcutil_copy_type(_tmp120);_tmpCC->f1=_tmpB60;}),_tmpCC->f2=_tmp11F;_tmpCC;});Cyc_Absyn_new_exp(_tmpB61,e->loc);});new_e=_tmpB62;});
goto _LL0;case 34U: _LL45: _tmp126=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).is_calloc;_tmp125=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).rgn;_tmp124=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).elt_type;_tmp123=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).num_elts;_tmp122=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).fat_result;_tmp121=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB3)->f1).inline_call;_LL46: {
# 411
struct Cyc_Absyn_Exp*_tmpCD=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp125;if(_tmp125 != 0)({struct Cyc_Absyn_Exp*_tmpB63=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp125);r1=_tmpB63;});{
void**t1=_tmp124;if(_tmp124 != 0)({void**_tmpB65=({void**_tmpCE=_cycalloc(sizeof(*_tmpCE));({void*_tmpB64=Cyc_Tcutil_copy_type(*_tmp124);*_tmpCE=_tmpB64;});_tmpCE;});t1=_tmpB65;});
({void*_tmpB66=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=34U,(_tmpCF->f1).is_calloc=_tmp126,(_tmpCF->f1).rgn=r1,(_tmpCF->f1).elt_type=t1,(_tmpCF->f1).num_elts=_tmp123,(_tmpCF->f1).fat_result=_tmp122,(_tmpCF->f1).inline_call=_tmp121;_tmpCF;});_tmpCD->r=_tmpB66;});
new_e=_tmpCD;
goto _LL0;};}case 35U: _LL47: _tmp128=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp127=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL48:
({struct Cyc_Absyn_Exp*_tmpB69=({struct Cyc_Absyn_Exp*_tmpB68=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp128);struct Cyc_Absyn_Exp*_tmpB67=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp127);Cyc_Absyn_swap_exp(_tmpB68,_tmpB67,e->loc);});new_e=_tmpB69;});goto _LL0;case 36U: _LL49: _tmp12A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp129=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL4A: {
# 419
struct Cyc_Core_Opt*nopt1=_tmp12A;
if(_tmp12A != 0)({struct Cyc_Core_Opt*_tmpB6A=({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=(struct _tuple2*)_tmp12A->v;_tmpD0;});nopt1=_tmpB6A;});
({struct Cyc_Absyn_Exp*_tmpB6D=({void*_tmpB6C=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->tag=36U,_tmpD1->f1=nopt1,({struct Cyc_List_List*_tmpB6B=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB1,_tmp129);_tmpD1->f2=_tmpB6B;});_tmpD1;});Cyc_Absyn_new_exp(_tmpB6C,e->loc);});new_e=_tmpB6D;});
goto _LL0;}case 37U: _LL4B: _LL4C:
# 424
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpB6E=({const char*_tmpD2="deep_copy: statement expressions unsupported";_tag_dyneither(_tmpD2,sizeof(char),45U);});_tmpD3->f1=_tmpB6E;});_tmpD3;}));case 38U: _LL4D: _tmp12C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp12B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL4E:
({struct Cyc_Absyn_Exp*_tmpB71=({void*_tmpB70=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->tag=38U,({struct Cyc_Absyn_Exp*_tmpB6F=Cyc_Tcutil_deep_copy_exp(_tmpB1,_tmp12C);_tmpD4->f1=_tmpB6F;}),_tmpD4->f2=_tmp12B;_tmpD4;});Cyc_Absyn_new_exp(_tmpB70,e->loc);});new_e=_tmpB71;});
goto _LL0;case 39U: _LL4F: _tmp12D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_LL50:
({struct Cyc_Absyn_Exp*_tmpB73=({void*_tmpB72=Cyc_Tcutil_copy_type(_tmp12D);Cyc_Absyn_valueof_exp(_tmpB72,e->loc);});new_e=_tmpB73;});
goto _LL0;default: _LL51: _tmp12F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB3)->f1;_tmp12E=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB3)->f2;_LL52:
({struct Cyc_Absyn_Exp*_tmpB74=Cyc_Absyn_asm_exp(_tmp12F,_tmp12E,e->loc);new_e=_tmpB74;});goto _LL0;}_LL0:;}
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
struct _tuple20 _tmp132=({struct _tuple20 _tmpA2F;_tmpA2F.f1=_tmp139,_tmpA2F.f2=_tmp137;_tmpA2F;});struct _tuple20 _tmp133=_tmp132;switch(_tmp133.f1){case Cyc_Absyn_BoxKind: switch(_tmp133.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp133.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 465
if(_tmp138 != _tmp136){
struct _tuple21 _tmp134=({struct _tuple21 _tmpA30;_tmpA30.f1=_tmp138,_tmpA30.f2=_tmp136;_tmpA30;});struct _tuple21 _tmp135=_tmp134;switch(_tmp135.f1){case Cyc_Absyn_Aliasable: if(_tmp135.f2 == Cyc_Absyn_Top){_LL10: _LL11:
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
({void*_tmpB75=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=2U,_tmp13C->f1=0,_tmp13C->f2=def;_tmp13C;});tv->kind=_tmpB75;});
return def;}_LL0:;}
# 485
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp13F=({struct _tuple0 _tmpA31;({void*_tmpB77=Cyc_Absyn_compress_kb(kb1);_tmpA31.f1=_tmpB77;}),({void*_tmpB76=Cyc_Absyn_compress_kb(kb2);_tmpA31.f2=_tmpB76;});_tmpA31;});struct _tuple0 _tmp140=_tmp13F;struct Cyc_Core_Opt**_tmp155;void*_tmp154;void*_tmp153;struct Cyc_Core_Opt**_tmp152;struct Cyc_Core_Opt**_tmp151;struct Cyc_Absyn_Kind*_tmp150;struct Cyc_Core_Opt**_tmp14F;struct Cyc_Absyn_Kind*_tmp14E;struct Cyc_Core_Opt**_tmp14D;struct Cyc_Absyn_Kind*_tmp14C;struct Cyc_Absyn_Kind*_tmp14B;struct Cyc_Absyn_Kind*_tmp14A;struct Cyc_Core_Opt**_tmp149;struct Cyc_Absyn_Kind*_tmp148;struct Cyc_Absyn_Kind*_tmp147;struct Cyc_Absyn_Kind*_tmp146;switch(*((int*)_tmp140.f1)){case 0U: switch(*((int*)_tmp140.f2)){case 0U: _LL1: _tmp147=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp146=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_LL2:
 return _tmp147 == _tmp146;case 2U: _LL5: _tmp14A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp149=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_tmp148=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f2;_LL6:
# 494
 if(Cyc_Tcutil_kind_leq(_tmp14A,_tmp148)){
({struct Cyc_Core_Opt*_tmpB78=({struct Cyc_Core_Opt*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->v=kb1;_tmp142;});*_tmp149=_tmpB78;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp140.f2)){case 0U: _LL3: _tmp14D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp14C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f2;_tmp14B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_LL4:
# 489
 if(Cyc_Tcutil_kind_leq(_tmp14B,_tmp14C)){
({struct Cyc_Core_Opt*_tmpB79=({struct Cyc_Core_Opt*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->v=kb2;_tmp141;});*_tmp14D=_tmpB79;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp151=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp150=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f1)->f2;_tmp14F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_tmp14E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp140.f2)->f2;_LL8:
# 499
 if(Cyc_Tcutil_kind_leq(_tmp150,_tmp14E)){
({struct Cyc_Core_Opt*_tmpB7A=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->v=kb1;_tmp143;});*_tmp14F=_tmpB7A;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp14E,_tmp150)){
({struct Cyc_Core_Opt*_tmpB7B=({struct Cyc_Core_Opt*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->v=kb2;_tmp144;});*_tmp151=_tmpB7B;});
return 1;}else{
return 0;}}default: _LLB: _tmp153=_tmp140.f1;_tmp152=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp140.f2)->f1;_LLC:
# 508
({struct Cyc_Core_Opt*_tmpB7C=({struct Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->v=_tmp153;_tmp145;});*_tmp152=_tmpB7C;});
return 1;}default: _LL9: _tmp155=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp140.f1)->f1;_tmp154=_tmp140.f2;_LLA:
# 506
 _tmp153=_tmp154;_tmp152=_tmp155;goto _LLC;}_LL0:;}
# 513
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp156=Cyc_Tcutil_compress(t);void*_tmp157=_tmp156;struct Cyc_Absyn_Tvar*_tmp15C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp157)->tag == 2U){_LL1: _tmp15C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp157)->f1;_LL2: {
# 516
void*_tmp158=_tmp15C->kind;
_tmp15C->kind=kb;
return({struct _tuple16 _tmpA32;_tmpA32.f1=_tmp15C,_tmpA32.f2=_tmp158;_tmpA32;});}}else{_LL3: _LL4:
# 520
({struct Cyc_String_pa_PrintArg_struct _tmp15B=({struct Cyc_String_pa_PrintArg_struct _tmpA33;_tmpA33.tag=0U,({struct _dyneither_ptr _tmpB7D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA33.f1=_tmpB7D;});_tmpA33;});void*_tmp159[1U];_tmp159[0]=& _tmp15B;({struct _dyneither_ptr _tmpB7E=({const char*_tmp15A="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp15A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB7E,_tag_dyneither(_tmp159,sizeof(void*),1U));});});}_LL0:;}
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
({struct Cyc_List_List*_tmpB80=({struct Cyc_List_List*_tmp164=_region_malloc(temp,sizeof(*_tmp164));({struct _tuple16*_tmpB7F=({struct _tuple16*_tmp163=_region_malloc(temp,sizeof(*_tmp163));_tmp163->f1=_tmp15F,_tmp163->f2=_tmp160;_tmp163;});_tmp164->hd=_tmpB7F;}),_tmp164->tl=_tmp15E;_tmp164;});_tmp15E=_tmpB80;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 545
if(_tmp15E != 0){
({void*_tmpB83=({struct _RegionHandle*_tmpB82=temp;struct Cyc_List_List*_tmpB81=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15E);Cyc_Tcutil_rsubstitute(_tmpB82,_tmpB81,field_typ);});field_typ=_tmpB83;});
({struct Cyc_Absyn_Kind*_tmpB84=Cyc_Tcutil_typ_kind(field_typ);k=_tmpB84;});}}
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
({void*_tmp167=0U;({struct _dyneither_ptr _tmpB85=({const char*_tmp168="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp168,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB85,_tag_dyneither(_tmp167,sizeof(void*),0U));});});}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166)->f1).aggr_info).UnknownAggr).tag == 1){_LL21: _LL22:
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
({struct Cyc_String_pa_PrintArg_struct _tmp176=({struct Cyc_String_pa_PrintArg_struct _tmpA34;_tmpA34.tag=0U,({struct _dyneither_ptr _tmpB86=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA34.f1=_tmpB86;});_tmpA34;});void*_tmp174[1U];_tmp174[0]=& _tmp176;({struct _dyneither_ptr _tmpB87=({const char*_tmp175="typ_kind: typedef found: %s";_tag_dyneither(_tmp175,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB87,_tag_dyneither(_tmp174,sizeof(void*),1U));});});
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
({void*_tmpB88=Cyc_Tcutil_compress(t);t=_tmpB88;});{
void*_tmp18B=t;struct Cyc_List_List*_tmp1B1;void*_tmp1B0;void*_tmp1AF;void*_tmp1AE;void*_tmp1AD;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AB;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1A8;struct Cyc_List_List*_tmp1A7;struct Cyc_List_List*_tmp1A6;void*_tmp1A5;struct Cyc_Absyn_Tqual _tmp1A4;void*_tmp1A3;struct Cyc_List_List*_tmp1A2;int _tmp1A1;struct Cyc_Absyn_VarargInfo*_tmp1A0;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_List_List*_tmp19C;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_List_List*_tmp19A;void*_tmp199;struct Cyc_Absyn_PtrInfo _tmp198;void*_tmp197;struct Cyc_Core_Opt**_tmp196;struct Cyc_Absyn_Tvar*_tmp195;switch(*((int*)_tmp18B)){case 2U: _LL1: _tmp195=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp18B)->f1;_LL2:
# 668
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp195)){
({struct _dyneither_ptr _tmpB89=({const char*_tmp18C="(type variable would escape scope)";_tag_dyneither(_tmp18C,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpB89;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 672
goto _LL0;case 1U: _LL3: _tmp197=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18B)->f2;_tmp196=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18B)->f4;_LL4:
# 674
 if(t == evar){
({struct _dyneither_ptr _tmpB8A=({const char*_tmp18D="(occurs check)";_tag_dyneither(_tmp18D,sizeof(char),15U);});Cyc_Tcutil_failure_reason=_tmpB8A;});
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
({struct Cyc_List_List*_tmpB8B=({struct Cyc_List_List*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp18F->tl=_tmp18E;_tmp18F;});_tmp18E=_tmpB8B;});}}
# 694
({struct Cyc_Core_Opt*_tmpB8C=({struct Cyc_Core_Opt*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->v=_tmp18E;_tmp190;});*_tmp196=_tmpB8C;});}}}
# 697
goto _LL0;case 5U: _LL5: _tmp198=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18B)->f1;_LL6:
# 700
 Cyc_Tcutil_occurs(evar,r,env,_tmp198.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp198.ptr_atts).rgn);
goto _LL0;case 8U: _LL7: _tmp199=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18B)->f1).elt_type;_LL8:
# 704
 Cyc_Tcutil_occurs(evar,r,env,_tmp199);goto _LL0;case 9U: _LL9: _tmp1A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).tvars;_tmp1A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).effect;_tmp1A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ret_tqual;_tmp1A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ret_typ;_tmp1A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).args;_tmp1A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).c_varargs;_tmp1A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).cyc_varargs;_tmp19F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).rgn_po;_tmp19E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).attributes;_tmp19D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).requires_clause;_tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).requires_relns;_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ensures_clause;_tmp19A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18B)->f1).ensures_relns;_LLA:
# 707
({struct Cyc_List_List*_tmpB8D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1A6,env);env=_tmpB8D;});
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
({void*_tmp1B2=0U;({struct _dyneither_ptr _tmpB8E=({const char*_tmp1B3="tq1 real_const not set.";_tag_dyneither(_tmp1B3,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB8E,_tag_dyneither(_tmp1B2,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1B4=0U;({struct _dyneither_ptr _tmpB8F=({const char*_tmp1B5="tq2 real_const not set.";_tag_dyneither(_tmp1B5,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB8F,_tag_dyneither(_tmp1B4,sizeof(void*),0U));});});
# 765
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 768
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpB90=({const char*_tmp1B6="(qualifiers don't match)";_tag_dyneither(_tmp1B6,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB90;});
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
({union Cyc_Absyn_Constraint*_tmpB91=Cyc_Absyn_compress_conref(x);x=_tmpB91;});
({union Cyc_Absyn_Constraint*_tmpB92=Cyc_Absyn_compress_conref(y);y=_tmpB92;});
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1B7=x;void*_tmp1BE;switch((((union Cyc_Absyn_Constraint*)_tmp1B7)->Eq_constr).tag){case 3U: _LL1: _LL2:
# 794
({union Cyc_Absyn_Constraint _tmpB93=({union Cyc_Absyn_Constraint _tmpA35;(_tmpA35.Forward_constr).tag=2U,(_tmpA35.Forward_constr).val=y;_tmpA35;});*x=_tmpB93;});return;case 1U: _LL3: _tmp1BE=(void*)(_tmp1B7->Eq_constr).val;_LL4: {
# 796
union Cyc_Absyn_Constraint*_tmp1B8=y;void*_tmp1BB;switch((((union Cyc_Absyn_Constraint*)_tmp1B8)->Eq_constr).tag){case 3U: _LL8: _LL9:
*y=*x;return;case 1U: _LLA: _tmp1BB=(void*)(_tmp1B8->Eq_constr).val;_LLB:
# 799
 if(cmp(_tmp1BE,_tmp1BB)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 803
return;default: _LLC: _LLD:
({void*_tmp1B9=0U;({struct _dyneither_ptr _tmpB94=({const char*_tmp1BA="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1BA,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpB94,_tag_dyneither(_tmp1B9,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 806
({void*_tmp1BC=0U;({struct _dyneither_ptr _tmpB95=({const char*_tmp1BD="unify_conref: forward after compress";_tag_dyneither(_tmp1BD,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpB95,_tag_dyneither(_tmp1BC,sizeof(void*),0U));});});}_LL0:;};}
# 810
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1BF;_push_handler(& _tmp1BF);{int _tmp1C1=0;if(setjmp(_tmp1BF.handler))_tmp1C1=1;if(!_tmp1C1){
({int(*_tmpB98)(void*,void*)=cmp;union Cyc_Absyn_Constraint*_tmpB97=x;union Cyc_Absyn_Constraint*_tmpB96=y;Cyc_Tcutil_unify_it_conrefs(_tmpB98,_tmpB97,_tmpB96,_tag_dyneither(0,0,0));});{
int _tmp1C2=1;_npop_handler(0U);return _tmp1C2;};
# 812
;_pop_handler();}else{void*_tmp1C0=(void*)_exn_thrown;void*_tmp1C3=_tmp1C0;void*_tmp1C4;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1C3)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 814
 return 0;}else{_LL3: _tmp1C4=_tmp1C3;_LL4:(int)_rethrow(_tmp1C4);}_LL0:;}};}
# 817
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp1C5=({struct _tuple0 _tmpA36;_tmpA36.f1=b1,_tmpA36.f2=b2;_tmpA36;});struct _tuple0 _tmp1C6=_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C6.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C6.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1C6.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{_LL7: _tmp1C8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1C6.f1)->f1;_tmp1C7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp1C6.f2)->f1;_LL8:
 return Cyc_Evexp_const_exp_cmp(_tmp1C8,_tmp1C7);}}_LL0:;}
# 826
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1C9=({struct _tuple0 _tmpA37;_tmpA37.f1=e1->r,_tmpA37.f2=e2->r;_tmpA37;});struct _tuple0 _tmp1CA=_tmp1C9;void*_tmp1D4;void*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D1;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->tag == 14U){_LL1: _tmp1D1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->f2;_LL2:
# 831
 Cyc_Tcutil_unify_cmp_exp(_tmp1D1,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->tag == 14U){_LL3: _tmp1D2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1D2);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->tag == 39U){_LL5: _tmp1D3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f1)->f1;_LL6: {
# 834
void*_tmp1CB=Cyc_Tcutil_compress(_tmp1D3);void*_tmp1CC=_tmp1CB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CC)->tag == 1U){_LLC: _LLD:
({void*_tmpB99=_tmp1D3;Cyc_Tcutil_unify_it(_tmpB99,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->tag=18U,_tmp1CD->f1=e2;_tmp1CD;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->tag == 39U){_LL7: _tmp1D4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1CA.f2)->f1;_LL8: {
# 839
void*_tmp1CE=Cyc_Tcutil_compress(_tmp1D4);void*_tmp1CF=_tmp1CE;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->tag == 1U){_LL11: _LL12:
({void*_tmpB9A=_tmp1D4;Cyc_Tcutil_unify_it(_tmpB9A,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->tag=18U,_tmp1D0->f1=e1;_tmp1D0;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 843
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 847
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp1D5=({struct _tuple0 _tmpA38;_tmpA38.f1=b1,_tmpA38.f2=b2;_tmpA38;});struct _tuple0 _tmp1D6=_tmp1D5;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Exp*_tmp1DD;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D6.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1D6.f2)->tag == 0U){_LL1: _LL2:
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
struct _tuple0 _tmp1E0=({struct _tuple0 _tmpA39;_tmpA39.f1=att1,_tmpA39.f2=att2;_tmpA39;});struct _tuple0 _tmp1E1=_tmp1E0;enum Cyc_Absyn_Format_Type _tmp1F1;int _tmp1F0;int _tmp1EF;enum Cyc_Absyn_Format_Type _tmp1EE;int _tmp1ED;int _tmp1EC;struct _dyneither_ptr _tmp1EB;struct _dyneither_ptr _tmp1EA;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1E8;int _tmp1E7;int _tmp1E6;int _tmp1E5;int _tmp1E4;switch(*((int*)_tmp1E1.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E1.f2)->tag == 0U){_LL1: _tmp1E5=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E1.f1)->f1;_tmp1E4=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1E1.f2)->f1;_LL2:
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
 return({int _tmpB9B=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpB9B,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
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
({void*_tmpB9D=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->tag=18U,({struct Cyc_Absyn_Exp*_tmpB9C=Cyc_Absyn_uint_exp(0U,0U);_tmp1F4->f1=_tmpB9C;});_tmp1F4;});t=_tmpB9D;});goto _LL0;default: _LL9: _LLA:
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
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->tag=24U,({struct Cyc_List_List*_tmpBA0=({void*_tmp1FF[2U];({void*_tmpB9F=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->tag=23U,_tmp200->f1=_tmp20E;_tmp200;});_tmp1FF[0]=_tmpB9F;}),({void*_tmpB9E=Cyc_Tcutil_rgns_of(_tmp20F);_tmp1FF[1]=_tmpB9E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1FF,sizeof(void*),2U));});_tmp201->f1=_tmpBA0;});_tmp201;}));case 8U: _LL1D: _tmp210=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F7)->f1).elt_type;_LL1E:
# 978
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp210));case 10U: _LL1F: _tmp211=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F7)->f1;_LL20: {
# 980
struct Cyc_List_List*_tmp202=0;
for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
({struct Cyc_List_List*_tmpBA1=({struct Cyc_List_List*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->hd=(*((struct _tuple12*)_tmp211->hd)).f2,_tmp203->tl=_tmp202;_tmp203;});_tmp202=_tmpBA1;});}
_tmp212=_tmp202;goto _LL22;}case 4U: _LL21: _tmp212=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F7)->f1).targs;_LL22:
 _tmp213=_tmp212;goto _LL24;case 11U: _LL23: _tmp213=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F7)->f1).targs;_LL24:
# 986
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->tag=24U,({struct Cyc_List_List*_tmpBA2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp213);_tmp204->f1=_tmpBA2;});_tmp204;}));case 12U: _LL25: _tmp214=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F7)->f2;_LL26:
# 988
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->tag=24U,({struct Cyc_List_List*_tmpBA3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp214);_tmp205->f1=_tmpBA3;});_tmp205;}));case 19U: _LL27: _LL28:
 return Cyc_Absyn_empty_effect;case 9U: _LL29: _tmp21B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).tvars;_tmp21A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).effect;_tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).ret_tqual;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).ret_typ;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).args;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).cyc_varargs;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F7)->f1).rgn_po;_LL2A: {
# 998
void*_tmp206=({struct Cyc_List_List*_tmpBA4=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp21B);Cyc_Tcutil_substitute(_tmpBA4,(void*)_check_null(_tmp21A));});
return Cyc_Tcutil_normalize_effect(_tmp206);}case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 return Cyc_Absyn_empty_effect;case 23U: _LL31: _LL32:
 goto _LL34;case 24U: _LL33: _LL34:
 return t;case 25U: _LL35: _tmp21C=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1F7)->f1;_LL36:
 return Cyc_Tcutil_rgns_of(_tmp21C);case 17U: _LL37: _tmp21D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1F7)->f2;_LL38:
# 1007
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp207=_cycalloc(sizeof(*_tmp207));_tmp207->tag=24U,({struct Cyc_List_List*_tmpBA5=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp21D);_tmp207->f1=_tmpBA5;});_tmp207;}));default: _LL39: _LL3A:
({void*_tmp208=0U;({struct _dyneither_ptr _tmpBA6=({const char*_tmp209="typedecl in rgns_of";_tag_dyneither(_tmp209,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA6,_tag_dyneither(_tmp208,sizeof(void*),0U));});});}_LL0:;}
# 1015
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpBA7=Cyc_Tcutil_compress(e);e=_tmpBA7;});{
void*_tmp21E=e;void*_tmp22A;struct Cyc_List_List**_tmp229;switch(*((int*)_tmp21E)){case 24U: _LL1: _tmp229=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp21E)->f1;_LL2: {
# 1019
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp229;for(0;effs != 0;effs=effs->tl){
void*_tmp21F=(void*)effs->hd;
({void*_tmpBA8=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp21F));effs->hd=_tmpBA8;});{
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
({struct Cyc_List_List*_tmpBA9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp225,effects);effects=_tmpBA9;});
goto _LL12;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp223)->f1)){case 20U: _LL15: _LL16:
 goto _LL18;case 22U: _LL17: _LL18:
 goto _LL1A;case 21U: _LL19: _LL1A:
 goto _LL12;default: goto _LL1B;}default: _LL1B: _tmp226=_tmp223;_LL1C:
({struct Cyc_List_List*_tmpBAA=({struct Cyc_List_List*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->hd=_tmp226,_tmp224->tl=effects;_tmp224;});effects=_tmpBAA;});goto _LL12;}_LL12:;}}
# 1044
({struct Cyc_List_List*_tmpBAB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp229=_tmpBAB;});
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
struct Cyc_Absyn_Tqual _tmpBAC=Cyc_Absyn_empty_tqual(0U);(_tmp22C->f1).ret_tqual=_tmpBAC;}),(_tmp22C->f1).ret_typ=(void*)& Cyc_Absyn_VoidType_val,(_tmp22C->f1).args=0,(_tmp22C->f1).c_varargs=0,(_tmp22C->f1).cyc_varargs=0,(_tmp22C->f1).rgn_po=0,(_tmp22C->f1).attributes=0,(_tmp22C->f1).requires_clause=0,(_tmp22C->f1).requires_relns=0,(_tmp22C->f1).ensures_clause=0,(_tmp22C->f1).ensures_relns=0;_tmp22C;});
# 1067
return({void*_tmpBAF=(void*)_tmp22B;struct Cyc_Absyn_Tqual _tmpBAE=Cyc_Absyn_empty_tqual(0U);void*_tmpBAD=Cyc_Absyn_bounds_one;Cyc_Absyn_atb_typ(_tmpBAF,(void*)& Cyc_Absyn_HeapRgn_val,_tmpBAE,_tmpBAD,Cyc_Absyn_false_conref);});}
# 1074
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpBB0=Cyc_Tcutil_compress(r);r=_tmpBB0;});
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp22D=Cyc_Tcutil_compress(e);void*_tmp22E=_tmp22D;struct Cyc_Core_Opt*_tmp24E;void**_tmp24D;struct Cyc_Core_Opt*_tmp24C;void*_tmp24B;struct Cyc_List_List*_tmp24A;void*_tmp249;switch(*((int*)_tmp22E)){case 23U: _LL1: _tmp249=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp22E)->f1;_LL2:
# 1081
 if(constrain)return Cyc_Tcutil_unify(r,_tmp249);
({void*_tmpBB1=Cyc_Tcutil_compress(_tmp249);_tmp249=_tmpBB1;});
if(r == _tmp249)return 1;{
struct _tuple0 _tmp22F=({struct _tuple0 _tmpA3A;_tmpA3A.f1=r,_tmpA3A.f2=_tmp249;_tmpA3A;});struct _tuple0 _tmp230=_tmp22F;struct Cyc_Absyn_Tvar*_tmp232;struct Cyc_Absyn_Tvar*_tmp231;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f2)->tag == 2U){_LLC: _tmp232=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f1)->f1;_tmp231=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp230.f2)->f1;_LLD:
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
void*_tmp238=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->tag=24U,({struct Cyc_List_List*_tmpBB3=({void*_tmp239[2U];_tmp239[0]=_tmp237,({void*_tmpBB2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->tag=23U,_tmp23A->f1=r;_tmp23A;});_tmp239[1]=_tmpBB2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp239,sizeof(void*),2U));});_tmp23B->f1=_tmpBB3;});_tmp23B;}));
*_tmp23D=_tmp238;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{_LL13: _tmp240=_tmp234;_LL14:
# 1109
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp240);}_LL10:;}case 1U: _LL7: _tmp24E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f1;_tmp24D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f2;_tmp24C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f4;_LL8:
# 1112
 if(_tmp24E == 0  || ((struct Cyc_Absyn_Kind*)_tmp24E->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp241=0U;({struct _dyneither_ptr _tmpBB4=({const char*_tmp242="effect evar has wrong kind";_tag_dyneither(_tmp242,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBB4,_tag_dyneither(_tmp241,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1117
void*_tmp243=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp24C);
# 1120
Cyc_Tcutil_occurs(_tmp243,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24C))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp244=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248->tag=24U,({struct Cyc_List_List*_tmpBB7=({struct Cyc_List_List*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247->hd=_tmp243,({struct Cyc_List_List*_tmpBB6=({struct Cyc_List_List*_tmp246=_cycalloc(sizeof(*_tmp246));({void*_tmpBB5=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->tag=23U,_tmp245->f1=r;_tmp245;});_tmp246->hd=_tmpBB5;}),_tmp246->tl=0;_tmp246;});_tmp247->tl=_tmpBB6;});_tmp247;});_tmp248->f1=_tmpBB7;});_tmp248;});
*_tmp24D=(void*)_tmp244;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1131
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpBB8=Cyc_Tcutil_compress(t);t=_tmpBB8;});{
void*_tmp24F=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp250=_tmp24F;struct Cyc_Core_Opt*_tmp265;void**_tmp264;struct Cyc_Core_Opt*_tmp263;void*_tmp262;struct Cyc_List_List*_tmp261;switch(*((int*)_tmp250)){case 23U: _LL1: _LL2:
 return 0;case 24U: _LL3: _tmp261=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp250)->f1;_LL4:
# 1136
 for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp261->hd))
return 1;}
return 0;case 25U: _LL5: _tmp262=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp250)->f1;_LL6:
# 1141
({void*_tmpBB9=Cyc_Tcutil_compress(_tmp262);_tmp262=_tmpBB9;});
if(t == _tmp262)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp262);{
void*_tmp251=Cyc_Tcutil_rgns_of(t);void*_tmp252=_tmp251;void*_tmp258;void*_tmp257;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp252)->tag == 25U){_LLC: _tmp257=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp252)->f1;_LLD: {
# 1146
struct _tuple0 _tmp253=({struct _tuple0 _tmpA3B;({void*_tmpBBA=Cyc_Tcutil_compress(_tmp257);_tmpA3B.f1=_tmpBBA;}),_tmpA3B.f2=_tmp262;_tmpA3B;});struct _tuple0 _tmp254=_tmp253;struct Cyc_Absyn_Tvar*_tmp256;struct Cyc_Absyn_Tvar*_tmp255;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f2)->tag == 2U){_LL11: _tmp256=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f1)->f1;_tmp255=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp254.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp262);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp257 == _tmp262;}_LL10:;}}else{_LLE: _tmp258=_tmp252;_LLF:
# 1150
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp258);}_LLB:;};case 1U: _LL7: _tmp265=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp250)->f1;_tmp264=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp250)->f2;_tmp263=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp250)->f4;_LL8:
# 1153
 if(_tmp265 == 0  || ((struct Cyc_Absyn_Kind*)_tmp265->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp259=0U;({struct _dyneither_ptr _tmpBBB=({const char*_tmp25A="effect evar has wrong kind";_tag_dyneither(_tmp25A,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBBB,_tag_dyneither(_tmp259,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1158
void*_tmp25B=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp263);
# 1161
Cyc_Tcutil_occurs(_tmp25B,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp263))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25C=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->tag=24U,({struct Cyc_List_List*_tmpBBE=({struct Cyc_List_List*_tmp25F=_cycalloc(sizeof(*_tmp25F));_tmp25F->hd=_tmp25B,({struct Cyc_List_List*_tmpBBD=({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));({void*_tmpBBC=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->tag=25U,_tmp25D->f1=t;_tmp25D;});_tmp25E->hd=_tmpBBC;}),_tmp25E->tl=0;_tmp25E;});_tmp25F->tl=_tmpBBD;});_tmp25F;});_tmp260->f1=_tmpBBE;});_tmp260;});
*_tmp264=(void*)_tmp25C;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1172
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpBBF=Cyc_Tcutil_compress(e);e=_tmpBBF;});{
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
void*_tmp26C=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->tag=24U,({struct Cyc_List_List*_tmpBC1=({void*_tmp26D[2U];_tmp26D[0]=_tmp26B,({void*_tmpBC0=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->tag=2U,_tmp26E->f1=v;_tmp26E;});_tmp26D[1]=_tmpBC0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26D,sizeof(void*),2U));});_tmp26F->f1=_tmpBC1;});_tmp26F;}));
*_tmp271=_tmp26C;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{_LLE: _tmp274=_tmp268;_LLF:
# 1198
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp274);}_LLB:;}case 1U: _LL7: _tmp282=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp266)->f1;_tmp281=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp266)->f2;_tmp280=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp266)->f4;_LL8:
# 1201
 if(_tmp282 == 0  || ((struct Cyc_Absyn_Kind*)_tmp282->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp275=0U;({struct _dyneither_ptr _tmpBC2=({const char*_tmp276="effect evar has wrong kind";_tag_dyneither(_tmp276,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC2,_tag_dyneither(_tmp275,sizeof(void*),0U));});});{
# 1205
void*_tmp277=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp280);
# 1207
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp280))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp278=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->tag=24U,({struct Cyc_List_List*_tmpBC5=({struct Cyc_List_List*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->hd=_tmp277,({struct Cyc_List_List*_tmpBC4=({struct Cyc_List_List*_tmp27A=_cycalloc(sizeof(*_tmp27A));({void*_tmpBC3=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->tag=2U,_tmp279->f1=v;_tmp279;});_tmp27A->hd=_tmpBC3;}),_tmp27A->tl=0;_tmp27A;});_tmp27B->tl=_tmpBC4;});_tmp27B;});_tmp27C->f1=_tmpBC5;});_tmp27C;});
*_tmp281=(void*)_tmp278;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1217
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpBC6=Cyc_Tcutil_compress(e);e=_tmpBC6;});{
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
({struct Cyc_String_pa_PrintArg_struct _tmp292=({struct Cyc_String_pa_PrintArg_struct _tmpA3C;_tmpA3C.tag=0U,({struct _dyneither_ptr _tmpBC7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmpA3C.f1=_tmpBC7;});_tmpA3C;});void*_tmp290[1U];_tmp290[0]=& _tmp292;({struct _dyneither_ptr _tmpBC8=({const char*_tmp291="subset_effect: bad effect: %s";_tag_dyneither(_tmp291,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC8,_tag_dyneither(_tmp290,sizeof(void*),1U));});});}_LL0:;}
# 1301 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpBC9=Cyc_Tcutil_normalize_effect(e1);e1=_tmpBC9;});
({void*_tmpBCA=Cyc_Tcutil_normalize_effect(e2);e2=_tmpBCA;});
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
({struct _dyneither_ptr _tmpBCB=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpBCB;});
({void*_tmpBCC=Cyc_Tcutil_compress(t1);t1=_tmpBCC;});
({void*_tmpBCD=Cyc_Tcutil_compress(t2);t2=_tmpBCD;});
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
({struct _dyneither_ptr _tmpBCE=({const char*_tmp2A8="(type variable would escape scope)";_tag_dyneither(_tmp2A8,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpBCE;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1391
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2B3->v,_tmp2A5)){
*_tmp2AE=t1;return;}
# 1394
({struct _dyneither_ptr _tmpBCF=({const char*_tmp2A9="(kinds are incompatible)";_tag_dyneither(_tmp2A9,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBCF;});
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
({struct _dyneither_ptr _tmpBD0=({const char*_tmp2B0="(kinds are incompatible)";_tag_dyneither(_tmp2B0,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpBD0;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1415
 goto _LL0;}_LL0:;}
# 1420
{struct _tuple0 _tmp2B4=({struct _tuple0 _tmpA42;_tmpA42.f1=t2,_tmpA42.f2=t1;_tmpA42;});struct _tuple0 _tmp2B5=_tmp2B4;void*_tmp371;void*_tmp370;void*_tmp36F;void*_tmp36E;void*_tmp36D;void*_tmp36C;struct Cyc_List_List*_tmp36B;struct Cyc_Absyn_Typedefdecl*_tmp36A;struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Typedefdecl*_tmp368;enum Cyc_Absyn_AggrKind _tmp367;struct Cyc_List_List*_tmp366;enum Cyc_Absyn_AggrKind _tmp365;struct Cyc_List_List*_tmp364;struct Cyc_List_List*_tmp363;struct Cyc_List_List*_tmp362;struct Cyc_List_List*_tmp361;void*_tmp360;struct Cyc_Absyn_Tqual _tmp35F;void*_tmp35E;struct Cyc_List_List*_tmp35D;int _tmp35C;struct Cyc_Absyn_VarargInfo*_tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;struct Cyc_Absyn_Exp*_tmp358;struct Cyc_List_List*_tmp357;struct Cyc_Absyn_Exp*_tmp356;struct Cyc_List_List*_tmp355;struct Cyc_List_List*_tmp354;void*_tmp353;struct Cyc_Absyn_Tqual _tmp352;void*_tmp351;struct Cyc_List_List*_tmp350;int _tmp34F;struct Cyc_Absyn_VarargInfo*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_List_List*_tmp34C;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_List_List*_tmp34A;struct Cyc_Absyn_Exp*_tmp349;struct Cyc_List_List*_tmp348;void*_tmp347;struct Cyc_Absyn_Tqual _tmp346;struct Cyc_Absyn_Exp*_tmp345;union Cyc_Absyn_Constraint*_tmp344;void*_tmp343;struct Cyc_Absyn_Tqual _tmp342;struct Cyc_Absyn_Exp*_tmp341;union Cyc_Absyn_Constraint*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_Absyn_Exp*_tmp33E;void*_tmp33D;void*_tmp33C;int _tmp33B;int _tmp33A;enum Cyc_Absyn_Sign _tmp339;enum Cyc_Absyn_Size_of _tmp338;enum Cyc_Absyn_Sign _tmp337;enum Cyc_Absyn_Size_of _tmp336;void*_tmp335;struct Cyc_Absyn_Tqual _tmp334;void*_tmp333;union Cyc_Absyn_Constraint*_tmp332;union Cyc_Absyn_Constraint*_tmp331;union Cyc_Absyn_Constraint*_tmp330;void*_tmp32F;struct Cyc_Absyn_Tqual _tmp32E;void*_tmp32D;union Cyc_Absyn_Constraint*_tmp32C;union Cyc_Absyn_Constraint*_tmp32B;union Cyc_Absyn_Constraint*_tmp32A;struct Cyc_Absyn_Datatypedecl*_tmp329;struct Cyc_Absyn_Datatypefield*_tmp328;struct Cyc_List_List*_tmp327;struct Cyc_Absyn_Datatypedecl*_tmp326;struct Cyc_Absyn_Datatypefield*_tmp325;struct Cyc_List_List*_tmp324;struct Cyc_Absyn_Datatypedecl*_tmp323;struct Cyc_List_List*_tmp322;struct Cyc_Absyn_Datatypedecl*_tmp321;struct Cyc_List_List*_tmp320;struct Cyc_List_List*_tmp31F;struct Cyc_List_List*_tmp31E;struct _tuple2*_tmp31D;struct _tuple2*_tmp31C;union Cyc_Absyn_AggrInfoU _tmp31B;struct Cyc_List_List*_tmp31A;union Cyc_Absyn_AggrInfoU _tmp319;struct Cyc_List_List*_tmp318;struct Cyc_Absyn_Tvar*_tmp317;struct Cyc_Absyn_Tvar*_tmp316;switch(*((int*)_tmp2B5.f1)){case 1U: _LL12: _LL13:
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
({struct _dyneither_ptr _tmpBD1=({const char*_tmp2BA="(variable types are not the same)";_tag_dyneither(_tmp2BA,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpBD1;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 11U: switch(*((int*)_tmp2B5.f2)){case 11U: _LL18: _tmp31B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f1)->f1).aggr_info;_tmp31A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f1)->f1).targs;_tmp319=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f2)->f1).aggr_info;_tmp318=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B5.f2)->f1).targs;_LL19: {
# 1439
struct _tuple13 _tmp2BB=Cyc_Absyn_aggr_kinded_name(_tmp319);struct _tuple13 _tmp2BC=_tmp2BB;enum Cyc_Absyn_AggrKind _tmp2C4;struct _tuple2*_tmp2C3;_LL4F: _tmp2C4=_tmp2BC.f1;_tmp2C3=_tmp2BC.f2;_LL50:;{
struct _tuple13 _tmp2BD=Cyc_Absyn_aggr_kinded_name(_tmp31B);struct _tuple13 _tmp2BE=_tmp2BD;enum Cyc_Absyn_AggrKind _tmp2C2;struct _tuple2*_tmp2C1;_LL52: _tmp2C2=_tmp2BE.f1;_tmp2C1=_tmp2BE.f2;_LL53:;
if(_tmp2C4 != _tmp2C2){({struct _dyneither_ptr _tmpBD2=({const char*_tmp2BF="(struct and union type)";_tag_dyneither(_tmp2BF,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpBD2;});goto _LL11;}
if(Cyc_Absyn_qvar_cmp(_tmp2C3,_tmp2C1)!= 0){({struct _dyneither_ptr _tmpBD3=({const char*_tmp2C0="(different type name)";_tag_dyneither(_tmp2C0,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpBD3;});goto _LL11;}
Cyc_Tcutil_unify_list(_tmp318,_tmp31A);
return;};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 13U: switch(*((int*)_tmp2B5.f2)){case 13U: _LL1A: _tmp31D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp31C=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL1B:
# 1448
 if(Cyc_Absyn_qvar_cmp(_tmp31D,_tmp31C)== 0)return;
({struct _dyneither_ptr _tmpBD4=({const char*_tmp2C5="(different enum types)";_tag_dyneither(_tmp2C5,sizeof(char),23U);});Cyc_Tcutil_failure_reason=_tmpBD4;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 14U: switch(*((int*)_tmp2B5.f2)){case 14U: _LL1C: _tmp31F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp31E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_LL1D: {
# 1453
int bad=0;
for(0;_tmp31F != 0  && _tmp31E != 0;(_tmp31F=_tmp31F->tl,_tmp31E=_tmp31E->tl)){
struct Cyc_Absyn_Enumfield*_tmp2C6=(struct Cyc_Absyn_Enumfield*)_tmp31F->hd;
struct Cyc_Absyn_Enumfield*_tmp2C7=(struct Cyc_Absyn_Enumfield*)_tmp31E->hd;
if(Cyc_Absyn_qvar_cmp(_tmp2C6->name,_tmp2C7->name)!= 0){
({struct _dyneither_ptr _tmpBD5=({const char*_tmp2C8="(different names for enum fields)";_tag_dyneither(_tmp2C8,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpBD5;});
bad=1;
break;}
# 1462
if(_tmp2C6->tag == _tmp2C7->tag)continue;
if(_tmp2C6->tag == 0  || _tmp2C7->tag == 0){
({struct _dyneither_ptr _tmpBD6=({const char*_tmp2C9="(different tag values for enum fields)";_tag_dyneither(_tmp2C9,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBD6;});
bad=1;
break;}
# 1468
if(!({struct Cyc_Absyn_Exp*_tmpBD7=(struct Cyc_Absyn_Exp*)_check_null(_tmp2C6->tag);Cyc_Evexp_same_const_exp(_tmpBD7,(struct Cyc_Absyn_Exp*)_check_null(_tmp2C7->tag));})){
({struct _dyneither_ptr _tmpBD8=({const char*_tmp2CA="(different tag values for enum fields)";_tag_dyneither(_tmp2CA,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBD8;});
bad=1;
break;}}
# 1474
if(bad)goto _LL11;
if(_tmp31F == 0  && _tmp31E == 0)return;
({struct _dyneither_ptr _tmpBD9=({const char*_tmp2CB="(different number of fields for enums)";_tag_dyneither(_tmp2CB,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBD9;});
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
({struct _dyneither_ptr _tmpBDA=({const char*_tmp2CC="(different datatype field types)";_tag_dyneither(_tmp2CC,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpBDA;});
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp2B5.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 5U: switch(*((int*)_tmp2B5.f2)){case 5U: _LL22: _tmp335=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).elt_typ;_tmp334=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).elt_tq;_tmp333=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).rgn;_tmp332=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).nullable;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).bounds;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ptr_atts).zero_term;_tmp32F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).elt_typ;_tmp32E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).elt_tq;_tmp32D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).rgn;_tmp32C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).nullable;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).bounds;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ptr_atts).zero_term;_LL23:
# 1499
 Cyc_Tcutil_unify_it(_tmp32F,_tmp335);
Cyc_Tcutil_unify_it(_tmp333,_tmp32D);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpBDC=_tmp32A;union Cyc_Absyn_Constraint*_tmpBDB=_tmp330;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBDC,_tmpBDB,({const char*_tmp2CD="(not both zero terminated)";_tag_dyneither(_tmp2CD,sizeof(char),27U);}));});
Cyc_Tcutil_unify_tqual(_tmp32E,_tmp32F,_tmp334,_tmp335);
({union Cyc_Absyn_Constraint*_tmpBDE=_tmp32B;union Cyc_Absyn_Constraint*_tmpBDD=_tmp331;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmpBDE,_tmpBDD,({const char*_tmp2CE="(different pointer bounds)";_tag_dyneither(_tmp2CE,sizeof(char),27U);}));});
# 1507
{void*_tmp2CF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32B);void*_tmp2D0=_tmp2CF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D0)->tag == 0U){_LL55: _LL56:
 return;}else{_LL57: _LL58:
 goto _LL54;}_LL54:;}
# 1511
({union Cyc_Absyn_Constraint*_tmpBE0=_tmp32C;union Cyc_Absyn_Constraint*_tmpBDF=_tmp332;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBE0,_tmpBDF,({const char*_tmp2D1="(incompatible pointer types)";_tag_dyneither(_tmp2D1,sizeof(char),29U);}));});
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 6U: switch(*((int*)_tmp2B5.f2)){case 6U: _LL24: _tmp339=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp338=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f1)->f2;_tmp337=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_tmp336=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B5.f2)->f2;_LL25:
# 1516
 if(_tmp337 == _tmp339  && ((_tmp336 == _tmp338  || _tmp336 == Cyc_Absyn_Int_sz  && _tmp338 == Cyc_Absyn_Long_sz) || 
_tmp336 == Cyc_Absyn_Long_sz  && _tmp338 == Cyc_Absyn_Int_sz))return;
({struct _dyneither_ptr _tmpBE1=({const char*_tmp2D2="(different integral types)";_tag_dyneither(_tmp2D2,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpBE1;});
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
({struct _dyneither_ptr _tmpBE2=({const char*_tmp2D3="(cannot prove expressions are the same)";_tag_dyneither(_tmp2D3,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpBE2;});
goto _LL11;}
# 1535
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 8U: switch(*((int*)_tmp2B5.f2)){case 8U: _LL2C: _tmp347=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).elt_type;_tmp346=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).tq;_tmp345=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).num_elts;_tmp344=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f1)->f1).zero_term;_tmp343=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).elt_type;_tmp342=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).tq;_tmp341=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).num_elts;_tmp340=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2B5.f2)->f1).zero_term;_LL2D:
# 1539
 Cyc_Tcutil_unify_it(_tmp343,_tmp347);
Cyc_Tcutil_unify_tqual(_tmp342,_tmp343,_tmp346,_tmp347);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpBE4=_tmp344;union Cyc_Absyn_Constraint*_tmpBE3=_tmp340;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpBE4,_tmpBE3,({const char*_tmp2D4="(not both zero terminated)";_tag_dyneither(_tmp2D4,sizeof(char),27U);}));});
if(_tmp345 == _tmp341)return;
if(_tmp345 == 0  || _tmp341 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp345,_tmp341))
return;
({struct _dyneither_ptr _tmpBE5=({const char*_tmp2D5="(different array sizes)";_tag_dyneither(_tmp2D5,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpBE5;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 9U: switch(*((int*)_tmp2B5.f2)){case 9U: _LL2E: _tmp361=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).tvars;_tmp360=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).effect;_tmp35F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ret_tqual;_tmp35E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ret_typ;_tmp35D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).args;_tmp35C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).c_varargs;_tmp35B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).cyc_varargs;_tmp35A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).rgn_po;_tmp359=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).attributes;_tmp358=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).requires_clause;_tmp357=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).requires_relns;_tmp356=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ensures_clause;_tmp355=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f1)->f1).ensures_relns;_tmp354=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).tvars;_tmp353=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).effect;_tmp352=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ret_tqual;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ret_typ;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).args;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).c_varargs;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).cyc_varargs;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).rgn_po;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).attributes;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).requires_clause;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).requires_relns;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ensures_clause;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B5.f2)->f1).ensures_relns;_LL2F: {
# 1553
int done=0;
{struct _RegionHandle _tmp2D6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2D6;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp354 != 0){
if(_tmp361 == 0){
({struct _dyneither_ptr _tmpBE6=({const char*_tmp2D7="(second function type has too few type variables)";_tag_dyneither(_tmp2D7,sizeof(char),50U);});Cyc_Tcutil_failure_reason=_tmpBE6;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1561
void*_tmp2D8=((struct Cyc_Absyn_Tvar*)_tmp354->hd)->kind;
void*_tmp2D9=((struct Cyc_Absyn_Tvar*)_tmp361->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2D8,_tmp2D9)){
({struct _dyneither_ptr _tmpBEB=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DC=({struct Cyc_String_pa_PrintArg_struct _tmpA3F;_tmpA3F.tag=0U,({
struct _dyneither_ptr _tmpBE7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp354->hd));_tmpA3F.f1=_tmpBE7;});_tmpA3F;});struct Cyc_String_pa_PrintArg_struct _tmp2DD=({struct Cyc_String_pa_PrintArg_struct _tmpA3E;_tmpA3E.tag=0U,({
struct _dyneither_ptr _tmpBE8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp354->hd,& Cyc_Tcutil_bk)));_tmpA3E.f1=_tmpBE8;});_tmpA3E;});struct Cyc_String_pa_PrintArg_struct _tmp2DE=({struct Cyc_String_pa_PrintArg_struct _tmpA3D;_tmpA3D.tag=0U,({
struct _dyneither_ptr _tmpBE9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp361->hd,& Cyc_Tcutil_bk)));_tmpA3D.f1=_tmpBE9;});_tmpA3D;});void*_tmp2DA[3U];_tmp2DA[0]=& _tmp2DC,_tmp2DA[1]=& _tmp2DD,_tmp2DA[2]=& _tmp2DE;({struct _dyneither_ptr _tmpBEA=({const char*_tmp2DB="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2DB,sizeof(char),44U);});Cyc_aprintf(_tmpBEA,_tag_dyneither(_tmp2DA,sizeof(void*),3U));});});
# 1564
Cyc_Tcutil_failure_reason=_tmpBEB;});
# 1568
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1570
({struct Cyc_List_List*_tmpBEE=({struct Cyc_List_List*_tmp2E1=_region_malloc(rgn,sizeof(*_tmp2E1));({struct _tuple16*_tmpBED=({struct _tuple16*_tmp2E0=_region_malloc(rgn,sizeof(*_tmp2E0));_tmp2E0->f1=(struct Cyc_Absyn_Tvar*)_tmp361->hd,({void*_tmpBEC=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));_tmp2DF->tag=2U,_tmp2DF->f1=(struct Cyc_Absyn_Tvar*)_tmp354->hd;_tmp2DF;});_tmp2E0->f2=_tmpBEC;});_tmp2E0;});_tmp2E1->hd=_tmpBED;}),_tmp2E1->tl=inst;_tmp2E1;});inst=_tmpBEE;});
_tmp354=_tmp354->tl;
_tmp361=_tmp361->tl;};}
# 1574
if(_tmp361 != 0){
({struct _dyneither_ptr _tmpBEF=({const char*_tmp2E2="(second function type has too many type variables)";_tag_dyneither(_tmp2E2,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpBEF;});
_npop_handler(0U);goto _LL11;}
# 1578
if(inst != 0){
({void*_tmpBF2=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3->tag=9U,(_tmp2E3->f1).tvars=0,(_tmp2E3->f1).effect=_tmp353,(_tmp2E3->f1).ret_tqual=_tmp352,(_tmp2E3->f1).ret_typ=_tmp351,(_tmp2E3->f1).args=_tmp350,(_tmp2E3->f1).c_varargs=_tmp34F,(_tmp2E3->f1).cyc_varargs=_tmp34E,(_tmp2E3->f1).rgn_po=_tmp34D,(_tmp2E3->f1).attributes=_tmp34C,(_tmp2E3->f1).requires_clause=_tmp358,(_tmp2E3->f1).requires_relns=_tmp357,(_tmp2E3->f1).ensures_clause=_tmp356,(_tmp2E3->f1).ensures_relns=_tmp355;_tmp2E3;});Cyc_Tcutil_unify_it(_tmpBF2,({
# 1582
struct _RegionHandle*_tmpBF1=rgn;struct Cyc_List_List*_tmpBF0=inst;Cyc_Tcutil_rsubstitute(_tmpBF1,_tmpBF0,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E4=_cycalloc(sizeof(*_tmp2E4));
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
({struct _dyneither_ptr _tmpBF3=({const char*_tmp2ED="(function types have different number of arguments)";_tag_dyneither(_tmp2ED,sizeof(char),52U);});Cyc_Tcutil_failure_reason=_tmpBF3;});
goto _LL11;}
# 1606
if(_tmp34F != _tmp35C){
({struct _dyneither_ptr _tmpBF4=({const char*_tmp2EE="(only one function type takes C varargs)";_tag_dyneither(_tmp2EE,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpBF4;});
goto _LL11;}{
# 1611
int bad_cyc_vararg=0;
{struct _tuple22 _tmp2EF=({struct _tuple22 _tmpA40;_tmpA40.f1=_tmp34E,_tmpA40.f2=_tmp35B;_tmpA40;});struct _tuple22 _tmp2F0=_tmp2EF;struct _dyneither_ptr*_tmp2FA;struct Cyc_Absyn_Tqual _tmp2F9;void*_tmp2F8;int _tmp2F7;struct _dyneither_ptr*_tmp2F6;struct Cyc_Absyn_Tqual _tmp2F5;void*_tmp2F4;int _tmp2F3;if(_tmp2F0.f1 == 0){if(_tmp2F0.f2 == 0){_LL60: _LL61:
 goto _LL5F;}else{_LL62: _LL63:
 goto _LL65;}}else{if(_tmp2F0.f2 == 0){_LL64: _LL65:
# 1616
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpBF5=({const char*_tmp2F1="(only one function type takes varargs)";_tag_dyneither(_tmp2F1,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpBF5;});
goto _LL5F;}else{_LL66: _tmp2FA=(_tmp2F0.f1)->name;_tmp2F9=(_tmp2F0.f1)->tq;_tmp2F8=(_tmp2F0.f1)->type;_tmp2F7=(_tmp2F0.f1)->inject;_tmp2F6=(_tmp2F0.f2)->name;_tmp2F5=(_tmp2F0.f2)->tq;_tmp2F4=(_tmp2F0.f2)->type;_tmp2F3=(_tmp2F0.f2)->inject;_LL67:
# 1620
 Cyc_Tcutil_unify_it(_tmp2F8,_tmp2F4);
Cyc_Tcutil_unify_tqual(_tmp2F9,_tmp2F8,_tmp2F5,_tmp2F4);
if(_tmp2F7 != _tmp2F3){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpBF6=({const char*_tmp2F2="(only one function type injects varargs)";_tag_dyneither(_tmp2F2,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpBF6;});}
# 1626
goto _LL5F;}}_LL5F:;}
# 1628
if(bad_cyc_vararg)goto _LL11;{
# 1631
int bad_effect=0;
{struct _tuple0 _tmp2FB=({struct _tuple0 _tmpA41;_tmpA41.f1=_tmp353,_tmpA41.f2=_tmp360;_tmpA41;});struct _tuple0 _tmp2FC=_tmp2FB;if(_tmp2FC.f1 == 0){if(_tmp2FC.f2 == 0){_LL69: _LL6A:
 goto _LL68;}else{_LL6B: _LL6C:
 goto _LL6E;}}else{if(_tmp2FC.f2 == 0){_LL6D: _LL6E:
 bad_effect=1;goto _LL68;}else{_LL6F: _LL70:
({int _tmpBF8=!({void*_tmpBF7=(void*)_check_null(_tmp353);Cyc_Tcutil_unify_effect(_tmpBF7,(void*)_check_null(_tmp360));});bad_effect=_tmpBF8;});goto _LL68;}}_LL68:;}
# 1638
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpBF9=({const char*_tmp2FD="(function effects do not match)";_tag_dyneither(_tmp2FD,sizeof(char),32U);});Cyc_Tcutil_failure_reason=_tmpBF9;});
goto _LL11;}
# 1644
if(!Cyc_Tcutil_same_atts(_tmp359,_tmp34C)){
({struct _dyneither_ptr _tmpBFA=({const char*_tmp2FE="(function types have different attributes)";_tag_dyneither(_tmp2FE,sizeof(char),43U);});Cyc_Tcutil_failure_reason=_tmpBFA;});
goto _LL11;}
# 1648
if(!Cyc_Tcutil_same_rgn_po(_tmp35A,_tmp34D)){
({struct _dyneither_ptr _tmpBFB=({const char*_tmp2FF="(function types have different region lifetime orderings)";_tag_dyneither(_tmp2FF,sizeof(char),58U);});Cyc_Tcutil_failure_reason=_tmpBFB;});
goto _LL11;}
# 1652
if(!Cyc_Tcutil_check_logical_equivalence(_tmp357,_tmp34A)){
({struct _dyneither_ptr _tmpBFC=({const char*_tmp300="(@requires clauses not equivalent)";_tag_dyneither(_tmp300,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpBFC;});
goto _LL11;}
# 1656
if(!Cyc_Tcutil_check_logical_equivalence(_tmp355,_tmp348)){
({struct _dyneither_ptr _tmpBFD=({const char*_tmp301="(@ensures clauses not equivalent)";_tag_dyneither(_tmp301,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpBFD;});
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
({struct _dyneither_ptr _tmpBFE=({const char*_tmp30A="(tuple types have different numbers of components)";_tag_dyneither(_tmp30A,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpBFE;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 12U: switch(*((int*)_tmp2B5.f2)){case 12U: _LL32: _tmp367=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f1)->f1;_tmp366=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f1)->f2;_tmp365=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f2)->f1;_tmp364=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B5.f2)->f2;_LL33:
# 1676
 if(_tmp365 != _tmp367){({struct _dyneither_ptr _tmpBFF=({const char*_tmp30B="(struct and union type)";_tag_dyneither(_tmp30B,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpBFF;});goto _LL11;}
for(0;_tmp364 != 0  && _tmp366 != 0;(_tmp364=_tmp364->tl,_tmp366=_tmp366->tl)){
struct Cyc_Absyn_Aggrfield*_tmp30C=(struct Cyc_Absyn_Aggrfield*)_tmp364->hd;
struct Cyc_Absyn_Aggrfield*_tmp30D=(struct Cyc_Absyn_Aggrfield*)_tmp366->hd;
if(Cyc_strptrcmp(_tmp30C->name,_tmp30D->name)!= 0){
({struct _dyneither_ptr _tmpC00=({const char*_tmp30E="(different member names)";_tag_dyneither(_tmp30E,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC00;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1684
Cyc_Tcutil_unify_it(_tmp30C->type,_tmp30D->type);
Cyc_Tcutil_unify_tqual(_tmp30C->tq,_tmp30C->type,_tmp30D->tq,_tmp30D->type);
if(!Cyc_Tcutil_same_atts(_tmp30C->attributes,_tmp30D->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC01=({const char*_tmp30F="(different attributes on member)";_tag_dyneither(_tmp30F,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpC01;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1692
if((_tmp30C->width != 0  && _tmp30D->width == 0  || 
_tmp30D->width != 0  && _tmp30C->width == 0) || 
(_tmp30C->width != 0  && _tmp30D->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpC02=(struct Cyc_Absyn_Exp*)_check_null(_tmp30C->width);Cyc_Evexp_same_const_exp(_tmpC02,(struct Cyc_Absyn_Exp*)_check_null(_tmp30D->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC03=({const char*_tmp310="(different bitfield widths on member)";_tag_dyneither(_tmp310,sizeof(char),38U);});Cyc_Tcutil_failure_reason=_tmpC03;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1701
if((_tmp30C->requires_clause != 0  && _tmp30D->requires_clause == 0  || 
_tmp30C->requires_clause == 0  && _tmp30D->requires_clause != 0) || 
(_tmp30C->requires_clause == 0  && _tmp30D->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpC04=(struct Cyc_Absyn_Exp*)_check_null(_tmp30C->requires_clause);Cyc_Evexp_same_const_exp(_tmpC04,(struct Cyc_Absyn_Exp*)_check_null(_tmp30D->requires_clause));})){
# 1706
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC05=({const char*_tmp311="(different @requires clauses on member)";_tag_dyneither(_tmp311,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpC05;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1712
if(_tmp364 == 0  && _tmp366 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC06=({const char*_tmp312="(different number of members)";_tag_dyneither(_tmp312,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpC06;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 17U: switch(*((int*)_tmp2B5.f2)){case 17U: _LL34: _tmp36B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f1)->f2;_tmp36A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f1)->f3;_tmp369=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f2)->f2;_tmp368=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B5.f2)->f3;_LL35:
# 1718
 if(_tmp36A != _tmp368){
({struct _dyneither_ptr _tmpC07=({const char*_tmp313="(different abstract typedefs)";_tag_dyneither(_tmp313,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpC07;});
goto _LL11;}
# 1722
({struct _dyneither_ptr _tmpC08=({const char*_tmp314="(type parameters to typedef differ)";_tag_dyneither(_tmp314,sizeof(char),36U);});Cyc_Tcutil_failure_reason=_tmpC08;});
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
({struct _dyneither_ptr _tmpC09=({const char*_tmp315="(effects don't unify)";_tag_dyneither(_tmp315,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpC09;});
goto _LL11;default: _LL4C: _LL4D:
 goto _LL11;}}}}_LL11:;}
# 1746
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1749
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpC0B)(void*,void*)=cmp;void*_tmpC0A=(void*)_check_null(a1);_tmpC0B(_tmpC0A,(void*)_check_null(a2));});}
# 1756
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp372=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp373=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp372,_tmp373);}
# 1763
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
({union Cyc_Absyn_Constraint*_tmpC0C=Cyc_Absyn_compress_conref(x);x=_tmpC0C;});
({union Cyc_Absyn_Constraint*_tmpC0D=Cyc_Absyn_compress_conref(y);y=_tmpC0D;});
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp374=x;void*_tmp37B;switch((((union Cyc_Absyn_Constraint*)_tmp374)->Eq_constr).tag){case 3U: _LL1: _LL2:
 return - 1;case 1U: _LL3: _tmp37B=(void*)(_tmp374->Eq_constr).val;_LL4: {
# 1770
union Cyc_Absyn_Constraint*_tmp375=y;void*_tmp378;switch((((union Cyc_Absyn_Constraint*)_tmp375)->Eq_constr).tag){case 3U: _LL8: _LL9:
 return 1;case 1U: _LLA: _tmp378=(void*)(_tmp375->Eq_constr).val;_LLB:
 return cmp(_tmp37B,_tmp378);default: _LLC: _LLD:
({void*_tmp376=0U;({struct _dyneither_ptr _tmpC0E=({const char*_tmp377="unify_conref: forward after compress(2)";_tag_dyneither(_tmp377,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpC0E,_tag_dyneither(_tmp376,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 1775
({void*_tmp379=0U;({struct _dyneither_ptr _tmpC0F=({const char*_tmp37A="unify_conref: forward after compress";_tag_dyneither(_tmp37A,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpC0F,_tag_dyneither(_tmp379,sizeof(void*),0U));});});}_LL0:;};}
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
({int _tmpC10=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp386=_tmpC10;});
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
({void*_tmpC11=Cyc_Tcutil_compress(t1);t1=_tmpC11;});
({void*_tmpC12=Cyc_Tcutil_compress(t2);t2=_tmpC12;});
if(t1 == t2)return 0;{
int _tmp389=({int _tmpC13=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpC13,Cyc_Tcutil_type_case_number(t2));});
if(_tmp389 != 0)
return _tmp389;{
# 1853
struct _tuple0 _tmp38A=({struct _tuple0 _tmpA43;_tmpA43.f1=t2,_tmpA43.f2=t1;_tmpA43;});struct _tuple0 _tmp38B=_tmp38A;struct _dyneither_ptr _tmp3EC;struct _dyneither_ptr _tmp3EB;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E7;void*_tmp3E6;void*_tmp3E5;void*_tmp3E4;void*_tmp3E3;void*_tmp3E2;void*_tmp3E1;void*_tmp3E0;void*_tmp3DF;enum Cyc_Absyn_AggrKind _tmp3DE;struct Cyc_List_List*_tmp3DD;enum Cyc_Absyn_AggrKind _tmp3DC;struct Cyc_List_List*_tmp3DB;struct Cyc_List_List*_tmp3DA;struct Cyc_List_List*_tmp3D9;void*_tmp3D8;struct Cyc_Absyn_Tqual _tmp3D7;struct Cyc_Absyn_Exp*_tmp3D6;union Cyc_Absyn_Constraint*_tmp3D5;void*_tmp3D4;struct Cyc_Absyn_Tqual _tmp3D3;struct Cyc_Absyn_Exp*_tmp3D2;union Cyc_Absyn_Constraint*_tmp3D1;int _tmp3D0;int _tmp3CF;enum Cyc_Absyn_Sign _tmp3CE;enum Cyc_Absyn_Size_of _tmp3CD;enum Cyc_Absyn_Sign _tmp3CC;enum Cyc_Absyn_Size_of _tmp3CB;void*_tmp3CA;struct Cyc_Absyn_Tqual _tmp3C9;void*_tmp3C8;union Cyc_Absyn_Constraint*_tmp3C7;union Cyc_Absyn_Constraint*_tmp3C6;union Cyc_Absyn_Constraint*_tmp3C5;void*_tmp3C4;struct Cyc_Absyn_Tqual _tmp3C3;void*_tmp3C2;union Cyc_Absyn_Constraint*_tmp3C1;union Cyc_Absyn_Constraint*_tmp3C0;union Cyc_Absyn_Constraint*_tmp3BF;struct Cyc_Absyn_Datatypedecl*_tmp3BE;struct Cyc_Absyn_Datatypefield*_tmp3BD;struct Cyc_List_List*_tmp3BC;struct Cyc_Absyn_Datatypedecl*_tmp3BB;struct Cyc_Absyn_Datatypefield*_tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Datatypedecl*_tmp3B8;struct Cyc_List_List*_tmp3B7;struct Cyc_Absyn_Datatypedecl*_tmp3B6;struct Cyc_List_List*_tmp3B5;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*_tmp3B3;struct _tuple2*_tmp3B2;struct _tuple2*_tmp3B1;union Cyc_Absyn_AggrInfoU _tmp3B0;struct Cyc_List_List*_tmp3AF;union Cyc_Absyn_AggrInfoU _tmp3AE;struct Cyc_List_List*_tmp3AD;struct Cyc_Absyn_Tvar*_tmp3AC;struct Cyc_Absyn_Tvar*_tmp3AB;switch(*((int*)_tmp38B.f1)){case 1U: switch(*((int*)_tmp38B.f2)){case 1U: _LL1: _LL2:
# 1855
({void*_tmp38C=0U;({struct _dyneither_ptr _tmpC14=({const char*_tmp38D="typecmp: can only compare closed types";_tag_dyneither(_tmp38D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC14,_tag_dyneither(_tmp38C,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 2U: switch(*((int*)_tmp38B.f2)){case 2U: _LL3: _tmp3AC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3AB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL4:
# 1859
 return Cyc_Core_intcmp(_tmp3AB->identity,_tmp3AC->identity);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 11U: switch(*((int*)_tmp38B.f2)){case 11U: _LL5: _tmp3B0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f1)->f1).aggr_info;_tmp3AF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f1)->f1).targs;_tmp3AE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f2)->f1).aggr_info;_tmp3AD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38B.f2)->f1).targs;_LL6: {
# 1863
struct _tuple13 _tmp38E=Cyc_Absyn_aggr_kinded_name(_tmp3B0);struct _tuple13 _tmp38F=_tmp38E;struct _tuple2*_tmp394;_LL38: _tmp394=_tmp38F.f2;_LL39:;{
struct _tuple13 _tmp390=Cyc_Absyn_aggr_kinded_name(_tmp3AE);struct _tuple13 _tmp391=_tmp390;struct _tuple2*_tmp393;_LL3B: _tmp393=_tmp391.f2;_LL3C:;{
int _tmp392=Cyc_Absyn_qvar_cmp(_tmp394,_tmp393);
if(_tmp392 != 0)return _tmp392;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3AF,_tmp3AD);}};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 13U: switch(*((int*)_tmp38B.f2)){case 13U: _LL7: _tmp3B2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3B1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL8:
# 1870
 return Cyc_Absyn_qvar_cmp(_tmp3B2,_tmp3B1);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 14U: switch(*((int*)_tmp38B.f2)){case 14U: _LL9: _tmp3B4=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3B3=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LLA:
# 1873
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp3B4,_tmp3B3);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp38B.f2)){case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLB: _tmp3B8=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f1)->f1).datatype_info).KnownDatatype).val;_tmp3B7=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f1)->f1).targs;_tmp3B6=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f2)->f1).datatype_info).KnownDatatype).val;_tmp3B5=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp38B.f2)->f1).targs;_LLC:
# 1877
 if(_tmp3B6 == _tmp3B8)return 0;{
int _tmp395=Cyc_Absyn_qvar_cmp(_tmp3B6->name,_tmp3B8->name);
if(_tmp395 != 0)return _tmp395;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3B5,_tmp3B7);};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp38B.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp38B.f2)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LLD: _tmp3BE=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp3BD=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp3BC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f1)->f1).targs;_tmp3BB=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp3BA=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp3B9=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp38B.f2)->f1).targs;_LLE:
# 1884
 if(_tmp3BB == _tmp3BE)return 0;{
int _tmp396=Cyc_Absyn_qvar_cmp(_tmp3BE->name,_tmp3BB->name);
if(_tmp396 != 0)return _tmp396;{
int _tmp397=Cyc_Absyn_qvar_cmp(_tmp3BD->name,_tmp3BA->name);
if(_tmp397 != 0)return _tmp397;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp3B9,_tmp3BC);};};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp38B.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 5U: switch(*((int*)_tmp38B.f2)){case 5U: _LLF: _tmp3CA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).elt_typ;_tmp3C9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).elt_tq;_tmp3C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).rgn;_tmp3C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).nullable;_tmp3C6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).bounds;_tmp3C5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f1)->f1).ptr_atts).zero_term;_tmp3C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).elt_typ;_tmp3C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).elt_tq;_tmp3C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).rgn;_tmp3C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).nullable;_tmp3C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).bounds;_tmp3BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38B.f2)->f1).ptr_atts).zero_term;_LL10: {
# 1893
int _tmp398=Cyc_Tcutil_typecmp(_tmp3C4,_tmp3CA);
if(_tmp398 != 0)return _tmp398;{
int _tmp399=Cyc_Tcutil_typecmp(_tmp3C2,_tmp3C8);
if(_tmp399 != 0)return _tmp399;{
int _tmp39A=Cyc_Tcutil_tqual_cmp(_tmp3C3,_tmp3C9);
if(_tmp39A != 0)return _tmp39A;{
int _tmp39B=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp3C0,_tmp3C6);
if(_tmp39B != 0)return _tmp39B;{
int _tmp39C=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3BF,_tmp3C5);
if(_tmp39C != 0)return _tmp39C;
{void*_tmp39D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3C0);void*_tmp39E=_tmp39D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp39E)->tag == 0U){_LL3E: _LL3F:
 return 0;}else{_LL40: _LL41:
 goto _LL3D;}_LL3D:;}
# 1907
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3C1,_tmp3C7);};};};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 6U: switch(*((int*)_tmp38B.f2)){case 6U: _LL11: _tmp3CE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3CD=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f1)->f2;_tmp3CC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f2)->f1;_tmp3CB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp38B.f2)->f2;_LL12:
# 1910
 if(_tmp3CC != _tmp3CE)return Cyc_Core_intcmp((int)((unsigned int)_tmp3CC),(int)((unsigned int)_tmp3CE));
if(_tmp3CB != _tmp3CD)return Cyc_Core_intcmp((int)((unsigned int)_tmp3CB),(int)((unsigned int)_tmp3CD));
return 0;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 7U: switch(*((int*)_tmp38B.f2)){case 7U: _LL13: _tmp3D0=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3CF=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL14:
# 1915
 if(_tmp3D0 == _tmp3CF)return 0;else{
if(_tmp3CF == 0)return - 1;else{
if(_tmp3CF == 1  && _tmp3D0 == 0)return - 1;else{
return 1;}}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 8U: switch(*((int*)_tmp38B.f2)){case 8U: _LL15: _tmp3D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).elt_type;_tmp3D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).tq;_tmp3D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).num_elts;_tmp3D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f1)->f1).zero_term;_tmp3D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).elt_type;_tmp3D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).tq;_tmp3D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).num_elts;_tmp3D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp38B.f2)->f1).zero_term;_LL16: {
# 1922
int _tmp39F=Cyc_Tcutil_tqual_cmp(_tmp3D3,_tmp3D7);
if(_tmp39F != 0)return _tmp39F;{
int _tmp3A0=Cyc_Tcutil_typecmp(_tmp3D4,_tmp3D8);
if(_tmp3A0 != 0)return _tmp3A0;{
int _tmp3A1=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp3D5,_tmp3D1);
if(_tmp3A1 != 0)return _tmp3A1;
if(_tmp3D6 == _tmp3D2)return 0;
if(_tmp3D6 == 0  || _tmp3D2 == 0)
({void*_tmp3A2=0U;({struct _dyneither_ptr _tmpC15=({const char*_tmp3A3="missing expression in array index";_tag_dyneither(_tmp3A3,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC15,_tag_dyneither(_tmp3A2,sizeof(void*),0U));});});
# 1932
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3D6,_tmp3D2);};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 9U: switch(*((int*)_tmp38B.f2)){case 9U: _LL17: _LL18:
# 1935
({void*_tmp3A4=0U;({struct _dyneither_ptr _tmpC16=({const char*_tmp3A5="typecmp: function types not handled";_tag_dyneither(_tmp3A5,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC16,_tag_dyneither(_tmp3A4,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 10U: switch(*((int*)_tmp38B.f2)){case 10U: _LL19: _tmp3DA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3D9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL1A:
# 1938
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3D9,_tmp3DA);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 12U: switch(*((int*)_tmp38B.f2)){case 12U: _LL1B: _tmp3DE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3DD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f1)->f2;_tmp3DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f2)->f1;_tmp3DB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38B.f2)->f2;_LL1C:
# 1941
 if(_tmp3DC != _tmp3DE){
if(_tmp3DC == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3DB,_tmp3DD);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 15U: switch(*((int*)_tmp38B.f2)){case 15U: _LL1D: _tmp3E0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3DF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL1E:
# 1946
 return Cyc_Tcutil_typecmp(_tmp3E0,_tmp3DF);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 16U: switch(*((int*)_tmp38B.f2)){case 16U: _LL1F: _tmp3E4=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3E3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f1)->f2;_tmp3E2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f2)->f1;_tmp3E1=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp38B.f2)->f2;_LL20: {
# 1948
int _tmp3A6=Cyc_Tcutil_typecmp(_tmp3E4,_tmp3E2);
if(_tmp3A6 != 0)return _tmp3A6;else{
return Cyc_Tcutil_typecmp(_tmp3E3,_tmp3E1);}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 19U: switch(*((int*)_tmp38B.f2)){case 19U: _LL21: _tmp3E6=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3E5=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL22:
 return Cyc_Tcutil_typecmp(_tmp3E6,_tmp3E5);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 18U: switch(*((int*)_tmp38B.f2)){case 18U: _LL23: _tmp3E8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3E7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL24:
 _tmp3EA=_tmp3E8;_tmp3E9=_tmp3E7;goto _LL26;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 27U: switch(*((int*)_tmp38B.f2)){case 27U: _LL25: _tmp3EA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3E9=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL26:
# 1954
 return Cyc_Evexp_const_exp_cmp(_tmp3EA,_tmp3E9);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 28U: switch(*((int*)_tmp38B.f2)){case 28U: _LL27: _tmp3EC=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp38B.f1)->f1;_tmp3EB=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp38B.f2)->f1;_LL28:
# 1956
 return Cyc_strcmp((struct _dyneither_ptr)_tmp3EC,(struct _dyneither_ptr)_tmp3EB);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 24U: _LL29: _LL2A:
 goto _LL2C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp38B.f2)->tag == 24U){_LL2B: _LL2C:
 goto _LL2E;}else{switch(*((int*)_tmp38B.f1)){case 23U: _LL2D: _LL2E:
 goto _LL30;case 25U: _LL2F: _LL30:
 goto _LL32;default: switch(*((int*)_tmp38B.f2)){case 25U: _LL31: _LL32:
 goto _LL34;case 23U: _LL33: _LL34:
({void*_tmp3A7=0U;({struct _dyneither_ptr _tmpC17=({const char*_tmp3A8="typecmp: effects not handled";_tag_dyneither(_tmp3A8,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC17,_tag_dyneither(_tmp3A7,sizeof(void*),0U));});});default: _LL35: _LL36:
({void*_tmp3A9=0U;({struct _dyneither_ptr _tmpC18=({const char*_tmp3AA="Unmatched case in typecmp";_tag_dyneither(_tmp3AA,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC18,_tag_dyneither(_tmp3A9,sizeof(void*),0U));});});}}}}_LL0:;};};}
# 1967
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp3ED=Cyc_Tcutil_compress(t);void*_tmp3EE=_tmp3ED;switch(*((int*)_tmp3EE)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 1979
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
({void*_tmpC19=Cyc_Tcutil_compress(t1);t1=_tmpC19;});
({void*_tmpC1A=Cyc_Tcutil_compress(t2);t2=_tmpC1A;});{
struct _tuple0 _tmp3EF=({struct _tuple0 _tmpA44;_tmpA44.f1=t1,_tmpA44.f2=t2;_tmpA44;});struct _tuple0 _tmp3F0=_tmp3EF;int _tmp3F2;int _tmp3F1;switch(*((int*)_tmp3F0.f1)){case 7U: switch(*((int*)_tmp3F0.f2)){case 7U: _LL1: _tmp3F2=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp3F1=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL2:
# 1984
 return _tmp3F1 == 0  && _tmp3F2 != 0  || (_tmp3F1 == 1  && _tmp3F2 != 0) && _tmp3F2 != 1;case 6U: _LL3: _LL4:
 goto _LL6;case 19U: _LL5: _LL6:
 return 1;default: goto _LL21;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 return 0;}else{goto _LL9;}}else{_LL9: _LLA:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp3F0.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f2){case Cyc_Absyn_Int_sz: _LLB: _LLC:
# 1991
 goto _LLE;case Cyc_Absyn_Short_sz: _LL13: _LL14:
# 1996
 goto _LL16;case Cyc_Absyn_Char_sz: _LL19: _LL1A:
# 1999
 goto _LL1C;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f2)->f1 == 0){_LLF: _LL10:
# 1994
 goto _LL12;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp3F0.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f2){case Cyc_Absyn_Long_sz: _LLD: _LLE:
# 1992
 return 0;case Cyc_Absyn_Short_sz: _LL15: _LL16:
# 1997
 goto _LL18;case Cyc_Absyn_Char_sz: _LL1B: _LL1C:
# 2000
 goto _LL1E;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f2)->f1 == 0){_LL11: _LL12:
# 1995
 goto _LL14;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f2 == Cyc_Absyn_Char_sz){_LL1D: _LL1E:
# 2001
 goto _LL20;}else{goto _LL21;}}else{goto _LL21;}default: goto _LL21;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f2){case Cyc_Absyn_Short_sz: _LL17: _LL18:
# 1998
 goto _LL1A;case Cyc_Absyn_Char_sz: _LL1F: _LL20:
# 2002
 return 1;default: goto _LL21;}else{goto _LL21;}default: _LL21: _LL22:
# 2004
 return 0;}_LL0:;};}
# 2010
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2013
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
({struct Cyc_Core_Opt*_tmpC1B=({struct Cyc_Core_Opt*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->v=t1;_tmp3F3;});max_arith_type=_tmpC1B;});}}}
# 2022
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2026
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp3F6=({struct Cyc_String_pa_PrintArg_struct _tmpA46;_tmpA46.tag=0U,({
struct _dyneither_ptr _tmpC1C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA46.f1=_tmpC1C;});_tmpA46;});struct Cyc_String_pa_PrintArg_struct _tmp3F7=({struct Cyc_String_pa_PrintArg_struct _tmpA45;_tmpA45.tag=0U,({struct _dyneither_ptr _tmpC1D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmpA45.f1=_tmpC1D;});_tmpA45;});void*_tmp3F4[2U];_tmp3F4[0]=& _tmp3F6,_tmp3F4[1]=& _tmp3F7;({unsigned int _tmpC1F=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpC1E=({const char*_tmp3F5="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp3F5,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpC1F,_tmpC1E,_tag_dyneither(_tmp3F4,sizeof(void*),2U));});});
return 0;}}}
# 2032
return 1;}
# 2037
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp3F8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3F9=_tmp3F8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F9)->tag == 5U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2043
return 1;}
# 2046
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp3FA=Cyc_Tcutil_compress(t);void*_tmp3FB=_tmp3FA;switch(*((int*)_tmp3FB)){case 6U: _LL1: _LL2:
 goto _LL4;case 19U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2057
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2061
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp3FC=0U;({unsigned int _tmpC21=e->loc;struct _dyneither_ptr _tmpC20=({const char*_tmp3FD="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3FD,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpC21,_tmpC20,_tag_dyneither(_tmp3FC,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2067
return 0;}
# 2071
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2075
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp3FE=0U;({unsigned int _tmpC23=e->loc;struct _dyneither_ptr _tmpC22=({const char*_tmp3FF="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3FF,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpC23,_tmpC22,_tag_dyneither(_tmp3FE,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2081
return 0;}
# 2086
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2094
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2096
({void*_tmpC24=Cyc_Tcutil_compress(t1);t1=_tmpC24;});
({void*_tmpC25=Cyc_Tcutil_compress(t2);t2=_tmpC25;});{
struct _tuple0 _tmp400=({struct _tuple0 _tmpA4A;_tmpA4A.f1=t1,_tmpA4A.f2=t2;_tmpA4A;});struct _tuple0 _tmp401=_tmp400;void*_tmp417;struct Cyc_Absyn_Tqual _tmp416;struct Cyc_Absyn_Exp*_tmp415;union Cyc_Absyn_Constraint*_tmp414;void*_tmp413;struct Cyc_Absyn_Tqual _tmp412;struct Cyc_Absyn_Exp*_tmp411;union Cyc_Absyn_Constraint*_tmp410;struct Cyc_Absyn_PtrInfo _tmp40F;struct Cyc_Absyn_PtrInfo _tmp40E;switch(*((int*)_tmp401.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp401.f2)->tag == 5U){_LL1: _tmp40F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp401.f1)->f1;_tmp40E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp401.f2)->f1;_LL2: {
# 2100
int okay=1;
# 2102
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp40F.ptr_atts).nullable,(_tmp40E.ptr_atts).nullable))
# 2105
({int _tmpC26=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp40F.ptr_atts).nullable);okay=_tmpC26;});
# 2107
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp40F.ptr_atts).bounds,(_tmp40E.ptr_atts).bounds)){
# 2110
struct _tuple0 _tmp402=({struct _tuple0 _tmpA47;({void*_tmpC28=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp40F.ptr_atts).bounds);_tmpA47.f1=_tmpC28;}),({
void*_tmpC27=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp40E.ptr_atts).bounds);_tmpA47.f2=_tmpC27;});_tmpA47;});
# 2110
struct _tuple0 _tmp403=_tmp402;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp406;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp403.f1)->tag == 1U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp403.f2)->tag == 0U){_LLA: _LLB:
# 2112
 goto _LLD;}else{_LLE: _tmp407=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp403.f1)->f1;_tmp406=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp403.f2)->f1;_LLF:
# 2115
({int _tmpC29=okay  && Cyc_Evexp_lte_const_exp(_tmp406,_tmp407);okay=_tmpC29;});
# 2119
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp40E.ptr_atts).zero_term))
({void*_tmp404=0U;({unsigned int _tmpC2B=loc;struct _dyneither_ptr _tmpC2A=({const char*_tmp405="implicit cast to shorter array";_tag_dyneither(_tmp405,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpC2B,_tmpC2A,_tag_dyneither(_tmp404,sizeof(void*),0U));});});
goto _LL9;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp403.f2)->tag == 0U){_LLC: _LLD:
# 2113
 okay=1;goto _LL9;}else{_LL10: _tmp408=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp403.f2)->f1;_LL11:
# 2124
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp40F.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp40E.ptr_atts).bounds))
goto _LL9;
okay=0;
goto _LL9;}}_LL9:;}
# 2133
okay=okay  && (!(_tmp40F.elt_tq).real_const  || (_tmp40E.elt_tq).real_const);
# 2136
if(!Cyc_Tcutil_unify((_tmp40F.ptr_atts).rgn,(_tmp40E.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp40F.ptr_atts).rgn,(_tmp40E.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmpA49;_tmpA49.tag=0U,({
struct _dyneither_ptr _tmpC2C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp40F.ptr_atts).rgn));_tmpA49.f1=_tmpC2C;});_tmpA49;});struct Cyc_String_pa_PrintArg_struct _tmp40C=({struct Cyc_String_pa_PrintArg_struct _tmpA48;_tmpA48.tag=0U,({
struct _dyneither_ptr _tmpC2D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp40E.ptr_atts).rgn));_tmpA48.f1=_tmpC2D;});_tmpA48;});void*_tmp409[2U];_tmp409[0]=& _tmp40B,_tmp409[1]=& _tmp40C;({unsigned int _tmpC2F=loc;struct _dyneither_ptr _tmpC2E=({const char*_tmp40A="implicit cast from region %s to region %s";_tag_dyneither(_tmp40A,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpC2F,_tmpC2E,_tag_dyneither(_tmp409,sizeof(void*),2U));});});}else{
okay=0;}}
# 2146
({int _tmpC30=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp40F.ptr_atts).zero_term,(_tmp40E.ptr_atts).zero_term) || 
# 2149
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp40F.ptr_atts).zero_term) && (_tmp40E.elt_tq).real_const);
# 2146
okay=_tmpC30;});{
# 2157
int _tmp40D=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp40E.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2160
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp40E.ptr_atts).zero_term);
# 2164
({int _tmpC31=okay  && (Cyc_Tcutil_unify(_tmp40F.elt_typ,_tmp40E.elt_typ) || 
(_tmp40D  && ((_tmp40E.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp40E.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp40F.elt_typ,_tmp40E.elt_typ));
# 2164
okay=_tmpC31;});
# 2167
return okay;};}}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f2)->tag == 8U){_LL3: _tmp417=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f1)->f1).elt_type;_tmp416=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f1)->f1).tq;_tmp415=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f1)->f1).num_elts;_tmp414=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f1)->f1).zero_term;_tmp413=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f2)->f1).elt_type;_tmp412=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f2)->f1).tq;_tmp411=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f2)->f1).num_elts;_tmp410=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp401.f2)->f1).zero_term;_LL4: {
# 2171
int okay;
# 2174
({int _tmpC32=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp414,_tmp410) && (
(_tmp415 != 0  && _tmp411 != 0) && Cyc_Evexp_same_const_exp(_tmp415,_tmp411));
# 2174
okay=_tmpC32;});
# 2177
return(okay  && Cyc_Tcutil_unify(_tmp417,_tmp413)) && (!_tmp416.real_const  || _tmp412.real_const);}}else{goto _LL7;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp401.f2)->tag == 6U){_LL5: _LL6:
# 2179
 return 0;}else{goto _LL7;}default: _LL7: _LL8:
# 2181
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2185
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp418=Cyc_Tcutil_compress(t);void*_tmp419=_tmp418;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp419)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2192
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp41A=Cyc_Tcutil_compress(t);void*_tmp41B=_tmp41A;union Cyc_Absyn_Constraint*_tmp41C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->tag == 5U){_LL1: _tmp41C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->f1).ptr_atts).zero_term;_LL2:
# 2195
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp41C);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2200
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp41D=Cyc_Tcutil_compress(t);void*_tmp41E=_tmp41D;union Cyc_Absyn_Constraint*_tmp41F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->tag == 5U){_LL1: _tmp41F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41E)->f1).ptr_atts).nullable;_LL2:
# 2203
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp41F);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2209
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp420=Cyc_Tcutil_compress(t);void*_tmp421=_tmp420;void*_tmp424;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp421)->tag == 5U){_LL1: _tmp424=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp421)->f1).elt_typ;_LL2:
 return _tmp424;}else{_LL3: _LL4:
({void*_tmp422=0U;({struct _dyneither_ptr _tmpC33=({const char*_tmp423="pointer_elt_type";_tag_dyneither(_tmp423,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC33,_tag_dyneither(_tmp422,sizeof(void*),0U));});});}_LL0:;}
# 2215
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp425=Cyc_Tcutil_compress(t);void*_tmp426=_tmp425;struct Cyc_Absyn_PtrAtts*_tmp429;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp426)->tag == 5U){_LL1: _tmp429=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp426)->f1).ptr_atts;_LL2:
 return _tmp429->rgn;}else{_LL3: _LL4:
({void*_tmp427=0U;({struct _dyneither_ptr _tmpC34=({const char*_tmp428="pointer_elt_type";_tag_dyneither(_tmp428,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC34,_tag_dyneither(_tmp427,sizeof(void*),0U));});});}_LL0:;}
# 2222
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp42A=Cyc_Tcutil_compress(t);void*_tmp42B=_tmp42A;void*_tmp42C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->tag == 5U){_LL1: _tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).rgn;_LL2:
# 2225
*rgn=_tmp42C;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2232
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2236
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp42D=Cyc_Tcutil_compress(t);void*_tmp42E=_tmp42D;union Cyc_Absyn_Constraint*_tmp42F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42E)->tag == 5U){_LL1: _tmp42F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42E)->f1).ptr_atts).bounds;_LL2:
# 2239
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp42F)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2245
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp430=e->r;void*_tmp431=_tmp430;void*_tmp435;struct Cyc_Absyn_Exp*_tmp434;struct _dyneither_ptr _tmp433;switch(*((int*)_tmp431)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp431)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp431)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp431)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp431)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp431)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp433=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp431)->f1).Wchar_c).val;_LLC: {
# 2254
unsigned long _tmp432=Cyc_strlen((struct _dyneither_ptr)_tmp433);
int i=0;
if(_tmp432 >= 2  && *((const char*)_check_dyneither_subscript(_tmp433,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp433,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp433.curr)[1]== 'x'  && _tmp432 >= 3) && *((const char*)_check_dyneither_subscript(_tmp433,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp432;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp433,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2264
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2252
 return 1;case 14U: _LLD: _tmp435=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp431)->f1;_tmp434=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp431)->f2;_LLE:
# 2265
 return Cyc_Tcutil_is_zero(_tmp434) && Cyc_Tcutil_admits_zero(_tmp435);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2270
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2277
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2282
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2287
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2294
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2299
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2304
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp436=ka;enum Cyc_Absyn_KindQual _tmp43F;enum Cyc_Absyn_AliasQual _tmp43E;_LL1: _tmp43F=_tmp436->kind;_tmp43E=_tmp436->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp437=_tmp43E;switch(_tmp437){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2308
enum Cyc_Absyn_KindQual _tmp438=_tmp43F;switch(_tmp438){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& Cyc_Tcutil_eko;default: _LL15: _LL16:
 return& Cyc_Tcutil_iko;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2317
{enum Cyc_Absyn_KindQual _tmp439=_tmp43F;switch(_tmp439){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& Cyc_Tcutil_urko;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2324
goto _LL3;default: _LL8: _LL9:
# 2326
{enum Cyc_Absyn_KindQual _tmp43A=_tmp43F;switch(_tmp43A){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& Cyc_Tcutil_trko;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2333
goto _LL3;}_LL3:;}
# 2335
({struct Cyc_String_pa_PrintArg_struct _tmp43D=({struct Cyc_String_pa_PrintArg_struct _tmpA4B;_tmpA4B.tag=0U,({struct _dyneither_ptr _tmpC35=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmpA4B.f1=_tmpC35;});_tmpA4B;});void*_tmp43B[1U];_tmp43B[0]=& _tmp43D;({struct _dyneither_ptr _tmpC36=({const char*_tmp43C="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp43C,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC36,_tag_dyneither(_tmp43B,sizeof(void*),1U));});});}
# 2338
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0U,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0U,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0U,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0U,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0U,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0U,& Cyc_Tcutil_ik};
# 2346
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2353
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0U,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0U,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0U,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0U,& Cyc_Tcutil_trk};
# 2358
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2363
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0U,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0U,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0U,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0U,& Cyc_Tcutil_urk};
# 2368
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2373
struct Cyc_Absyn_Kind*_tmp440=ka;enum Cyc_Absyn_KindQual _tmp449;enum Cyc_Absyn_AliasQual _tmp448;_LL1: _tmp449=_tmp440->kind;_tmp448=_tmp440->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp441=_tmp448;switch(_tmp441){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2376
enum Cyc_Absyn_KindQual _tmp442=_tmp449;switch(_tmp442){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& ab;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& mb;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& bb;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& rb;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& eb;default: _LL15: _LL16:
 return& ib;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2385
{enum Cyc_Absyn_KindQual _tmp443=_tmp449;switch(_tmp443){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& uab;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& umb;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& urb;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2392
goto _LL3;default: _LL8: _LL9:
# 2394
{enum Cyc_Absyn_KindQual _tmp444=_tmp449;switch(_tmp444){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& tab;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& tmb;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& tbb;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& trb;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2401
goto _LL3;}_LL3:;}
# 2403
({struct Cyc_String_pa_PrintArg_struct _tmp447=({struct Cyc_String_pa_PrintArg_struct _tmpA4C;_tmpA4C.tag=0U,({struct _dyneither_ptr _tmpC37=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmpA4C.f1=_tmpC37;});_tmpA4C;});void*_tmp445[1U];_tmp445[0]=& _tmp447;({struct _dyneither_ptr _tmpC38=({const char*_tmp446="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp446,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC38,_tag_dyneither(_tmp445,sizeof(void*),1U));});});}
# 2406
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2409
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2411
return({struct Cyc_Core_Opt*_tmp44A=_cycalloc(sizeof(*_tmp44A));({void*_tmpC39=Cyc_Tcutil_kind_to_bound(k);_tmp44A->v=_tmpC39;});_tmp44A;});}
# 2416
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpC3A=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=0U,_tmp44B->f1=Cyc_Absyn_Null_c;_tmp44B;});e1->r=_tmpC3A;});{
struct Cyc_Core_Opt*_tmp44C=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44D=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->tag=5U,({void*_tmpC3F=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp44C);(_tmp452->f1).elt_typ=_tmpC3F;}),({
struct Cyc_Absyn_Tqual _tmpC3E=Cyc_Absyn_empty_tqual(0U);(_tmp452->f1).elt_tq=_tmpC3E;}),
({void*_tmpC3D=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp44C);((_tmp452->f1).ptr_atts).rgn=_tmpC3D;}),((_tmp452->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 2424
union Cyc_Absyn_Constraint*_tmpC3C=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp452->f1).ptr_atts).bounds=_tmpC3C;}),({
union Cyc_Absyn_Constraint*_tmpC3B=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp452->f1).ptr_atts).zero_term=_tmpC3B;}),((_tmp452->f1).ptr_atts).ptrloc=0;_tmp452;});
e1->topt=(void*)_tmp44D;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp450=({struct Cyc_String_pa_PrintArg_struct _tmpA4E;_tmpA4E.tag=0U,({
struct _dyneither_ptr _tmpC40=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA4E.f1=_tmpC40;});_tmpA4E;});struct Cyc_String_pa_PrintArg_struct _tmp451=({struct Cyc_String_pa_PrintArg_struct _tmpA4D;_tmpA4D.tag=0U,({struct _dyneither_ptr _tmpC41=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmpA4D.f1=_tmpC41;});_tmpA4D;});void*_tmp44E[2U];_tmp44E[0]=& _tmp450,_tmp44E[1]=& _tmp451;({struct _dyneither_ptr _tmpC42=({const char*_tmp44F="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp44F,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC42,_tag_dyneither(_tmp44E,sizeof(void*),2U));});});
return retv;};};}
# 2434
return 0;}
# 2437
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp453=c;switch(_tmp453){case Cyc_Absyn_Unknown_coercion: _LL1: _LL2:
 return({const char*_tmp454="unknown";_tag_dyneither(_tmp454,sizeof(char),8U);});case Cyc_Absyn_No_coercion: _LL3: _LL4:
 return({const char*_tmp455="no coercion";_tag_dyneither(_tmp455,sizeof(char),12U);});case Cyc_Absyn_Null_to_NonNull: _LL5: _LL6:
 return({const char*_tmp456="null check";_tag_dyneither(_tmp456,sizeof(char),11U);});default: _LL7: _LL8:
 return({const char*_tmp457="other coercion";_tag_dyneither(_tmp457,sizeof(char),15U);});}_LL0:;}
# 2446
int Cyc_Tcutil_warn_alias_coerce=0;
# 2452
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2456
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpC45=({struct _dyneither_ptr*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct _dyneither_ptr _tmpC44=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp46A=({struct Cyc_Int_pa_PrintArg_struct _tmpA50;_tmpA50.tag=1U,_tmpA50.f1=(unsigned long)counter ++;_tmpA50;});void*_tmp468[1U];_tmp468[0]=& _tmp46A;({struct _dyneither_ptr _tmpC43=({const char*_tmp469="__aliasvar%d";_tag_dyneither(_tmp469,sizeof(char),13U);});Cyc_aprintf(_tmpC43,_tag_dyneither(_tmp468,sizeof(void*),1U));});});*_tmp46B=_tmpC44;});_tmp46B;});_tmp46C->f2=_tmpC45;});_tmp46C;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_typ,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpC46=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=4U,_tmp467->f1=vd;_tmp467;});Cyc_Absyn_varb_exp(_tmpC46,e->loc);});
# 2465
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2467
{void*_tmp458=Cyc_Tcutil_compress(e_typ);void*_tmp459=_tmp458;void*_tmp466;struct Cyc_Absyn_Tqual _tmp465;void*_tmp464;union Cyc_Absyn_Constraint*_tmp463;union Cyc_Absyn_Constraint*_tmp462;union Cyc_Absyn_Constraint*_tmp461;struct Cyc_Absyn_PtrLoc*_tmp460;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->tag == 5U){_LL1: _tmp466=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->f1).elt_typ;_tmp465=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->f1).elt_tq;_tmp464=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->f1).ptr_atts).rgn;_tmp463=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->f1).ptr_atts).nullable;_tmp462=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->f1).ptr_atts).bounds;_tmp461=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->f1).ptr_atts).zero_term;_tmp460=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp459)->f1).ptr_atts).ptrloc;_LL2:
# 2469
{void*_tmp45A=Cyc_Tcutil_compress(_tmp464);void*_tmp45B=_tmp45A;void**_tmp45F;struct Cyc_Core_Opt*_tmp45E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp45B)->tag == 1U){_LL6: _tmp45F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp45B)->f2;_tmp45E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp45B)->f4;_LL7: {
# 2471
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45C=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->tag=2U,_tmp45D->f1=tv;_tmp45D;});
*_tmp45F=(void*)_tmp45C;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2476
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2480
e->topt=0;
vd->initializer=0;{
# 2484
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2486
return({struct _tuple15 _tmpA4F;_tmpA4F.f1=d,_tmpA4F.f2=ve;_tmpA4F;});};}
# 2491
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2494
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2499
void*_tmp46D=Cyc_Tcutil_compress(wants_typ);void*_tmp46E=_tmp46D;void*_tmp472;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->tag == 5U){_LL1: _tmp472=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E)->f1).ptr_atts).rgn;_LL2: {
# 2501
void*_tmp46F=Cyc_Tcutil_compress(_tmp472);void*_tmp470=_tmp46F;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp470)->tag == 20U){_LL6: _LL7:
 return 0;}else{_LL8: _LL9: {
# 2504
struct Cyc_Absyn_Kind*_tmp471=Cyc_Tcutil_typ_kind(_tmp472);
return _tmp471->kind == Cyc_Absyn_RgnKind  && _tmp471->aliasqual == Cyc_Absyn_Aliasable;}}_LL5:;}}else{_LL3: _LL4:
# 2507
 return 0;}_LL0:;}
# 2510
return 0;}
# 2514
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2519
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2521
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2523
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp475=({struct Cyc_String_pa_PrintArg_struct _tmpA52;_tmpA52.tag=0U,({
struct _dyneither_ptr _tmpC47=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA52.f1=_tmpC47;});_tmpA52;});struct Cyc_String_pa_PrintArg_struct _tmp476=({struct Cyc_String_pa_PrintArg_struct _tmpA51;_tmpA51.tag=0U,({struct _dyneither_ptr _tmpC48=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA51.f1=_tmpC48;});_tmpA51;});void*_tmp473[2U];_tmp473[0]=& _tmp475,_tmp473[1]=& _tmp476;({unsigned int _tmpC4A=e->loc;struct _dyneither_ptr _tmpC49=({const char*_tmp474="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp474,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpC4A,_tmpC49,_tag_dyneither(_tmp473,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2531
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp479=({struct Cyc_String_pa_PrintArg_struct _tmpA55;_tmpA55.tag=0U,({
struct _dyneither_ptr _tmpC4B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpA55.f1=_tmpC4B;});_tmpA55;});struct Cyc_String_pa_PrintArg_struct _tmp47A=({struct Cyc_String_pa_PrintArg_struct _tmpA54;_tmpA54.tag=0U,({struct _dyneither_ptr _tmpC4C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA54.f1=_tmpC4C;});_tmpA54;});struct Cyc_String_pa_PrintArg_struct _tmp47B=({struct Cyc_String_pa_PrintArg_struct _tmpA53;_tmpA53.tag=0U,({struct _dyneither_ptr _tmpC4D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA53.f1=_tmpC4D;});_tmpA53;});void*_tmp477[3U];_tmp477[0]=& _tmp479,_tmp477[1]=& _tmp47A,_tmp477[2]=& _tmp47B;({unsigned int _tmpC4F=e->loc;struct _dyneither_ptr _tmpC4E=({const char*_tmp478="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp478,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpC4F,_tmpC4E,_tag_dyneither(_tmp477,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2538
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({enum Cyc_Absyn_Coercion _tmpC50=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpC50;})!= Cyc_Absyn_Unknown_coercion){
# 2546
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp47E=({struct Cyc_String_pa_PrintArg_struct _tmpA57;_tmpA57.tag=0U,({
struct _dyneither_ptr _tmpC51=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpA57.f1=_tmpC51;});_tmpA57;});struct Cyc_String_pa_PrintArg_struct _tmp47F=({struct Cyc_String_pa_PrintArg_struct _tmpA56;_tmpA56.tag=0U,({struct _dyneither_ptr _tmpC52=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA56.f1=_tmpC52;});_tmpA56;});void*_tmp47C[2U];_tmp47C[0]=& _tmp47E,_tmp47C[1]=& _tmp47F;({unsigned int _tmpC54=e->loc;struct _dyneither_ptr _tmpC53=({const char*_tmp47D="implicit cast from %s to %s";_tag_dyneither(_tmp47D,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpC54,_tmpC53,_tag_dyneither(_tmp47C,sizeof(void*),2U));});});
return 1;}else{
# 2552
return 0;}}}}}
# 2559
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2562
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2566
int Cyc_Tcutil_coerceable(void*t){
void*_tmp480=Cyc_Tcutil_compress(t);void*_tmp481=_tmp480;switch(*((int*)_tmp481)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2584 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2588
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2591
struct _tuple23 _tmp482=*env;struct _tuple23 _tmp483=_tmp482;struct Cyc_List_List*_tmp48B;struct _RegionHandle*_tmp48A;struct Cyc_Tcenv_Tenv*_tmp489;int _tmp488;_LL1: _tmp48B=_tmp483.f1;_tmp48A=_tmp483.f2;_tmp489=_tmp483.f3;_tmp488=_tmp483.f4;_LL2:;{
# 2593
void*_tmp484=_tmp48B == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp48A,_tmp48B,x->type);
struct Cyc_List_List*_tmp485=Cyc_Tcutil_flatten_typ(_tmp48A,_tmp488,_tmp489,_tmp484);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp485)== 1)
return({struct Cyc_List_List*_tmp487=_region_malloc(_tmp48A,sizeof(*_tmp487));({struct _tuple12*_tmpC55=({struct _tuple12*_tmp486=_region_malloc(_tmp48A,sizeof(*_tmp486));_tmp486->f1=x->tq,_tmp486->f2=_tmp484;_tmp486;});_tmp487->hd=_tmpC55;}),_tmp487->tl=0;_tmp487;});else{
return _tmp485;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2599
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2601
struct _tuple24 _tmp48C=*env;struct _tuple24 _tmp48D=_tmp48C;struct _RegionHandle*_tmp497;struct Cyc_Tcenv_Tenv*_tmp496;int _tmp495;_LL1: _tmp497=_tmp48D.f1;_tmp496=_tmp48D.f2;_tmp495=_tmp48D.f3;_LL2:;{
struct _tuple12 _tmp48E=*x;struct _tuple12 _tmp48F=_tmp48E;struct Cyc_Absyn_Tqual _tmp494;void*_tmp493;_LL4: _tmp494=_tmp48F.f1;_tmp493=_tmp48F.f2;_LL5:;{
struct Cyc_List_List*_tmp490=Cyc_Tcutil_flatten_typ(_tmp497,_tmp495,_tmp496,_tmp493);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp490)== 1)
return({struct Cyc_List_List*_tmp492=_region_malloc(_tmp497,sizeof(*_tmp492));({struct _tuple12*_tmpC56=({struct _tuple12*_tmp491=_region_malloc(_tmp497,sizeof(*_tmp491));_tmp491->f1=_tmp494,_tmp491->f2=_tmp493;_tmp491;});_tmp492->hd=_tmpC56;}),_tmp492->tl=0;_tmp492;});else{
return _tmp490;}};};}
# 2608
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2612
if(flatten){
({void*_tmpC57=Cyc_Tcutil_compress(t1);t1=_tmpC57;});{
void*_tmp498=t1;struct Cyc_List_List*_tmp4B7;struct Cyc_Absyn_Aggrdecl*_tmp4B6;struct Cyc_List_List*_tmp4B5;struct Cyc_List_List*_tmp4B4;switch(*((int*)_tmp498)){case 0U: _LL1: _LL2:
 return 0;case 10U: _LL3: _tmp4B4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp498)->f1;_LL4: {
# 2617
struct _tuple24 _tmp499=({struct _tuple24 _tmpA58;_tmpA58.f1=r,_tmpA58.f2=te,_tmpA58.f3=flatten;_tmpA58;});
# 2619
struct Cyc_List_List*_tmp49A=_tmp4B4;struct _tuple12*_tmp4A1;struct Cyc_List_List*_tmp4A0;if(_tmp49A == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp4A1=(struct _tuple12*)_tmp49A->hd;_tmp4A0=_tmp49A->tl;_LLF: {
# 2622
struct Cyc_List_List*_tmp49B=Cyc_Tcutil_rcopy_tqt(& _tmp499,_tmp4A1);
_tmp499.f3=0;{
struct Cyc_List_List*_tmp49C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp499,_tmp4B4);
struct Cyc_List_List*_tmp49D=({struct Cyc_List_List*_tmp49F=_region_malloc(r,sizeof(*_tmp49F));_tmp49F->hd=_tmp49B,_tmp49F->tl=_tmp49C;_tmp49F;});
return({struct _RegionHandle*_tmpC58=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpC58,({struct Cyc_List_List*_tmp49E=_region_malloc(r,sizeof(*_tmp49E));_tmp49E->hd=_tmp49B,_tmp49E->tl=_tmp49C;_tmp49E;}));});};}}_LLB:;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp498)->f1).aggr_info).KnownAggr).tag == 2){_LL5: _tmp4B6=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp498)->f1).aggr_info).KnownAggr).val;_tmp4B5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp498)->f1).targs;_LL6:
# 2630
 if(((_tmp4B6->kind == Cyc_Absyn_UnionA  || _tmp4B6->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B6->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B6->impl))->rgn_po != 0)
# 2632
return({struct Cyc_List_List*_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3));({struct _tuple12*_tmpC5A=({struct _tuple12*_tmp4A2=_region_malloc(r,sizeof(*_tmp4A2));({struct Cyc_Absyn_Tqual _tmpC59=Cyc_Absyn_empty_tqual(0U);_tmp4A2->f1=_tmpC59;}),_tmp4A2->f2=t1;_tmp4A2;});_tmp4A3->hd=_tmpC5A;}),_tmp4A3->tl=0;_tmp4A3;});{
struct Cyc_List_List*_tmp4A4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4B6->tvs,_tmp4B5);
struct _tuple23 env=({struct _tuple23 _tmpA59;_tmpA59.f1=_tmp4A4,_tmpA59.f2=r,_tmpA59.f3=te,_tmpA59.f4=flatten;_tmpA59;});
struct Cyc_List_List*_tmp4A5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B6->impl))->fields;struct Cyc_List_List*_tmp4A6=_tmp4A5;struct Cyc_Absyn_Aggrfield*_tmp4AC;struct Cyc_List_List*_tmp4AB;if(_tmp4A6 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp4AC=(struct Cyc_Absyn_Aggrfield*)_tmp4A6->hd;_tmp4AB=_tmp4A6->tl;_LL14: {
# 2638
struct Cyc_List_List*_tmp4A7=Cyc_Tcutil_flatten_typ_f(& env,_tmp4AC);
env.f4=0;{
struct Cyc_List_List*_tmp4A8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp4AB);
struct Cyc_List_List*_tmp4A9=({struct Cyc_List_List*_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA));_tmp4AA->hd=_tmp4A7,_tmp4AA->tl=_tmp4A8;_tmp4AA;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp4A9);};}}_LL10:;};}else{goto _LL9;}case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp498)->f1 == Cyc_Absyn_StructA){_LL7: _tmp4B7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp498)->f2;_LL8: {
# 2645
struct _tuple23 env=({struct _tuple23 _tmpA5A;_tmpA5A.f1=0,_tmpA5A.f2=r,_tmpA5A.f3=te,_tmpA5A.f4=flatten;_tmpA5A;});
struct Cyc_List_List*_tmp4AD=_tmp4B7;struct Cyc_Absyn_Aggrfield*_tmp4B3;struct Cyc_List_List*_tmp4B2;if(_tmp4AD == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp4B3=(struct Cyc_Absyn_Aggrfield*)_tmp4AD->hd;_tmp4B2=_tmp4AD->tl;_LL19: {
# 2649
struct Cyc_List_List*_tmp4AE=Cyc_Tcutil_flatten_typ_f(& env,_tmp4B3);
env.f4=0;{
struct Cyc_List_List*_tmp4AF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp4B2);
struct Cyc_List_List*_tmp4B0=({struct Cyc_List_List*_tmp4B1=_region_malloc(r,sizeof(*_tmp4B1));_tmp4B1->hd=_tmp4AE,_tmp4B1->tl=_tmp4AF;_tmp4B1;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp4B0);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2655
 goto _LL0;}_LL0:;};}
# 2658
return({struct Cyc_List_List*_tmp4B9=_region_malloc(r,sizeof(*_tmp4B9));({struct _tuple12*_tmpC5C=({struct _tuple12*_tmp4B8=_region_malloc(r,sizeof(*_tmp4B8));({struct Cyc_Absyn_Tqual _tmpC5B=Cyc_Absyn_empty_tqual(0U);_tmp4B8->f1=_tmpC5B;}),_tmp4B8->f2=t1;_tmp4B8;});_tmp4B9->hd=_tmpC5C;}),_tmp4B9->tl=0;_tmp4B9;});}
# 2662
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp4BA=(void*)t->hd;void*_tmp4BB=_tmp4BA;switch(*((int*)_tmp4BB)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2668
 continue;default: _LL7: _LL8:
# 2670
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2673
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2676
return 1;}
# 2679
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp4BC=({struct _tuple0 _tmpA5B;({void*_tmpC5E=Cyc_Tcutil_compress(t1);_tmpA5B.f1=_tmpC5E;}),({void*_tmpC5D=Cyc_Tcutil_compress(t2);_tmpA5B.f2=_tmpC5D;});_tmpA5B;});struct _tuple0 _tmp4BD=_tmp4BC;enum Cyc_Absyn_Size_of _tmp4BF;enum Cyc_Absyn_Size_of _tmp4BE;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4BD.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4BD.f2)->tag == 6U){_LL1: _tmp4BF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4BD.f1)->f2;_tmp4BE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4BD.f2)->f2;_LL2:
# 2682
 return(_tmp4BF == _tmp4BE  || _tmp4BF == Cyc_Absyn_Int_sz  && _tmp4BE == Cyc_Absyn_Long_sz) || 
_tmp4BF == Cyc_Absyn_Long_sz  && _tmp4BE == Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2690
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2693
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
({void*_tmpC5F=Cyc_Tcutil_compress(t1);t1=_tmpC5F;});
({void*_tmpC60=Cyc_Tcutil_compress(t2);t2=_tmpC60;});{
struct _tuple0 _tmp4C0=({struct _tuple0 _tmpA5D;_tmpA5D.f1=t1,_tmpA5D.f2=t2;_tmpA5D;});struct _tuple0 _tmp4C1=_tmp4C0;struct Cyc_Absyn_FnInfo _tmp4EE;struct Cyc_Absyn_FnInfo _tmp4ED;struct Cyc_Absyn_Datatypedecl*_tmp4EC;struct Cyc_Absyn_Datatypefield*_tmp4EB;struct Cyc_List_List*_tmp4EA;struct Cyc_Absyn_Datatypedecl*_tmp4E9;struct Cyc_List_List*_tmp4E8;void*_tmp4E7;struct Cyc_Absyn_Tqual _tmp4E6;void*_tmp4E5;union Cyc_Absyn_Constraint*_tmp4E4;union Cyc_Absyn_Constraint*_tmp4E3;union Cyc_Absyn_Constraint*_tmp4E2;void*_tmp4E1;struct Cyc_Absyn_Tqual _tmp4E0;void*_tmp4DF;union Cyc_Absyn_Constraint*_tmp4DE;union Cyc_Absyn_Constraint*_tmp4DD;union Cyc_Absyn_Constraint*_tmp4DC;switch(*((int*)_tmp4C1.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f2)->tag == 5U){_LL1: _tmp4E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f1)->f1).elt_typ;_tmp4E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f1)->f1).elt_tq;_tmp4E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f1)->f1).ptr_atts).rgn;_tmp4E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f1)->f1).ptr_atts).nullable;_tmp4E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f1)->f1).ptr_atts).bounds;_tmp4E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f1)->f1).ptr_atts).zero_term;_tmp4E1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f2)->f1).elt_typ;_tmp4E0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f2)->f1).elt_tq;_tmp4DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f2)->f1).ptr_atts).rgn;_tmp4DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f2)->f1).ptr_atts).nullable;_tmp4DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f2)->f1).ptr_atts).bounds;_tmp4DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1.f2)->f1).ptr_atts).zero_term;_LL2:
# 2705
 if(_tmp4E6.real_const  && !_tmp4E0.real_const)
return 0;
# 2708
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4E4,_tmp4DE) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4E4)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DE))
return 0;
# 2712
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4E2,_tmp4DC) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4E2)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DC))
return 0;
# 2716
if((!Cyc_Tcutil_unify(_tmp4E5,_tmp4DF) && !Cyc_Tcenv_region_outlives(te,_tmp4E5,_tmp4DF)) && !
Cyc_Tcutil_subtype(te,assume,_tmp4E5,_tmp4DF))
return 0;
# 2720
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4E3,_tmp4DD)){
struct _tuple0 _tmp4C2=({struct _tuple0 _tmpA5C;({void*_tmpC62=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4E3);_tmpA5C.f1=_tmpC62;}),({void*_tmpC61=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DD);_tmpA5C.f2=_tmpC61;});_tmpA5C;});struct _tuple0 _tmp4C3=_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C4;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4C3.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4C3.f2)->tag == 0U){_LLA: _LLB:
 goto _LL9;}else{goto _LLE;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4C3.f2)->tag == 1U){_LLC: _tmp4C5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4C3.f1)->f1;_tmp4C4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4C3.f2)->f1;_LLD:
# 2725
 if(!Cyc_Evexp_lte_const_exp(_tmp4C4,_tmp4C5))
return 0;
goto _LL9;}else{_LLE: _LLF:
 return 0;}}_LL9:;}
# 2733
if(!_tmp4E0.real_const  && _tmp4E6.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp4E1)))
return 0;}{
# 2739
int _tmp4C6=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4DD,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp4DC);
# 2745
return(_tmp4C6  && ({struct Cyc_Tcenv_Tenv*_tmpC66=te;struct Cyc_List_List*_tmpC65=({struct Cyc_List_List*_tmp4C8=_cycalloc(sizeof(*_tmp4C8));({struct _tuple0*_tmpC63=({struct _tuple0*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7->f1=t1,_tmp4C7->f2=t2;_tmp4C7;});_tmp4C8->hd=_tmpC63;}),_tmp4C8->tl=assume;_tmp4C8;});void*_tmpC64=_tmp4E7;Cyc_Tcutil_ptrsubtype(_tmpC66,_tmpC65,_tmpC64,_tmp4E1);}) || Cyc_Tcutil_unify(_tmp4E7,_tmp4E1)) || Cyc_Tcutil_isomorphic(_tmp4E7,_tmp4E1);};}else{goto _LL7;}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C1.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C1.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C1.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3: _tmp4EC=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C1.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp4EB=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C1.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp4EA=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C1.f1)->f1).targs;_tmp4E9=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C1.f2)->f1).datatype_info).KnownDatatype).val;_tmp4E8=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C1.f2)->f1).targs;_LL4:
# 2751
 if(_tmp4EC != _tmp4E9  && Cyc_Absyn_qvar_cmp(_tmp4EC->name,_tmp4E9->name)!= 0)return 0;
# 2753
if(({int _tmpC67=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4EA);_tmpC67 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E8);}))return 0;
for(0;_tmp4EA != 0;(_tmp4EA=_tmp4EA->tl,_tmp4E8=_tmp4E8->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4EA->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp4E8))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 9U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1.f2)->tag == 9U){_LL5: _tmp4EE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1.f1)->f1;_tmp4ED=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C1.f2)->f1;_LL6:
# 2760
 if(_tmp4EE.tvars != 0  || _tmp4ED.tvars != 0){
struct Cyc_List_List*_tmp4C9=_tmp4EE.tvars;
struct Cyc_List_List*_tmp4CA=_tmp4ED.tvars;
if(({int _tmpC68=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4C9);_tmpC68 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4CA);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp4C9 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp4C9->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp4CA))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpC6B=({struct Cyc_List_List*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));({struct _tuple16*_tmpC6A=({struct _tuple16*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC->f1=(struct Cyc_Absyn_Tvar*)_tmp4CA->hd,({void*_tmpC69=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->tag=2U,_tmp4CB->f1=(struct Cyc_Absyn_Tvar*)_tmp4C9->hd;_tmp4CB;});_tmp4CC->f2=_tmpC69;});_tmp4CC;});_tmp4CD->hd=_tmpC6A;}),_tmp4CD->tl=inst;_tmp4CD;});inst=_tmpC6B;});
_tmp4C9=_tmp4C9->tl;
_tmp4CA=_tmp4CA->tl;}
# 2771
if(inst != 0){
_tmp4EE.tvars=0;
_tmp4ED.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpC6E=te;struct Cyc_List_List*_tmpC6D=assume;void*_tmpC6C=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE->tag=9U,_tmp4CE->f1=_tmp4EE;_tmp4CE;});Cyc_Tcutil_subtype(_tmpC6E,_tmpC6D,_tmpC6C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->tag=9U,_tmp4CF->f1=_tmp4ED;_tmp4CF;}));});}};}
# 2778
if(!Cyc_Tcutil_subtype(te,assume,_tmp4EE.ret_typ,_tmp4ED.ret_typ))return 0;{
struct Cyc_List_List*_tmp4D0=_tmp4EE.args;
struct Cyc_List_List*_tmp4D1=_tmp4ED.args;
# 2783
if(({int _tmpC6F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4D0);_tmpC6F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4D1);}))return 0;
# 2785
for(0;_tmp4D0 != 0;(_tmp4D0=_tmp4D0->tl,_tmp4D1=_tmp4D1->tl)){
struct _tuple10 _tmp4D2=*((struct _tuple10*)_tmp4D0->hd);struct _tuple10 _tmp4D3=_tmp4D2;struct Cyc_Absyn_Tqual _tmp4D9;void*_tmp4D8;_LL11: _tmp4D9=_tmp4D3.f2;_tmp4D8=_tmp4D3.f3;_LL12:;{
struct _tuple10 _tmp4D4=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp4D1))->hd);struct _tuple10 _tmp4D5=_tmp4D4;struct Cyc_Absyn_Tqual _tmp4D7;void*_tmp4D6;_LL14: _tmp4D7=_tmp4D5.f2;_tmp4D6=_tmp4D5.f3;_LL15:;
# 2789
if(_tmp4D7.real_const  && !_tmp4D9.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp4D6,_tmp4D8))
return 0;};}
# 2793
if(_tmp4EE.c_varargs != _tmp4ED.c_varargs)return 0;
if(_tmp4EE.cyc_varargs != 0  && _tmp4ED.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp4DA=*_tmp4EE.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp4DB=*_tmp4ED.cyc_varargs;
# 2798
if((_tmp4DB.tq).real_const  && !(_tmp4DA.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp4DB.type,_tmp4DA.type))
return 0;}else{
if(_tmp4EE.cyc_varargs != 0  || _tmp4ED.cyc_varargs != 0)return 0;}
# 2803
if(!({void*_tmpC70=(void*)_check_null(_tmp4EE.effect);Cyc_Tcutil_subset_effect(1,_tmpC70,(void*)_check_null(_tmp4ED.effect));}))return 0;
# 2805
if(!Cyc_Tcutil_sub_rgnpo(_tmp4EE.rgn_po,_tmp4ED.rgn_po))return 0;
# 2807
if(!Cyc_Tcutil_sub_attributes(_tmp4EE.attributes,_tmp4ED.attributes))return 0;
# 2809
if(!Cyc_Tcutil_check_logical_implication(_tmp4ED.requires_relns,_tmp4EE.requires_relns))
return 0;
# 2812
if(!Cyc_Tcutil_check_logical_implication(_tmp4EE.ensures_relns,_tmp4ED.ensures_relns))
return 0;
# 2815
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2827 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2829
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp4EF=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp4F0=_tmp4EF;struct Cyc_Absyn_Tqual _tmp4F6;void*_tmp4F5;_LL1: _tmp4F6=_tmp4F0->f1;_tmp4F5=_tmp4F0->f2;_LL2:;{
struct _tuple12*_tmp4F1=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp4F2=_tmp4F1;struct Cyc_Absyn_Tqual _tmp4F4;void*_tmp4F3;_LL4: _tmp4F4=_tmp4F2->f1;_tmp4F3=_tmp4F2->f2;_LL5:;
# 2836
if(_tmp4F6.real_const  && !_tmp4F4.real_const)return 0;
# 2838
if((_tmp4F4.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp4F3))) && 
Cyc_Tcutil_subtype(te,assume,_tmp4F5,_tmp4F3))
# 2841
continue;
# 2843
if(Cyc_Tcutil_unify(_tmp4F5,_tmp4F3))
# 2845
continue;
# 2847
if(Cyc_Tcutil_isomorphic(_tmp4F5,_tmp4F3))
# 2849
continue;
# 2852
return 0;};};}
# 2854
return 1;}
# 2857
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp4F7=Cyc_Tcutil_compress(t);void*_tmp4F8=_tmp4F7;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F8)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F8)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2866
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpC71=Cyc_Tcutil_compress(t1);t1=_tmpC71;});
({void*_tmpC72=Cyc_Tcutil_compress(t2);t2=_tmpC72;});
# 2872
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp4F9=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F9)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F9)->f2){case Cyc_Absyn_Int_sz: _LL1: _LL2:
# 2876
 goto _LL4;case Cyc_Absyn_Long_sz: _LL3: _LL4:
# 2878
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 2882
void*_tmp4FA=t1;void*_tmp521;struct Cyc_Absyn_Enumdecl*_tmp520;void*_tmp51F;struct Cyc_Absyn_Tqual _tmp51E;struct Cyc_Absyn_Exp*_tmp51D;union Cyc_Absyn_Constraint*_tmp51C;void*_tmp51B;struct Cyc_Absyn_Tqual _tmp51A;void*_tmp519;union Cyc_Absyn_Constraint*_tmp518;union Cyc_Absyn_Constraint*_tmp517;union Cyc_Absyn_Constraint*_tmp516;switch(*((int*)_tmp4FA)){case 5U: _LL8: _tmp51B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).elt_typ;_tmp51A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).elt_tq;_tmp519=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).ptr_atts).rgn;_tmp518=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).ptr_atts).nullable;_tmp517=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).ptr_atts).bounds;_tmp516=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FA)->f1).ptr_atts).zero_term;_LL9:
# 2891
{void*_tmp4FB=t2;void*_tmp50C;struct Cyc_Absyn_Tqual _tmp50B;void*_tmp50A;union Cyc_Absyn_Constraint*_tmp509;union Cyc_Absyn_Constraint*_tmp508;union Cyc_Absyn_Constraint*_tmp507;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB)->tag == 5U){_LL17: _tmp50C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB)->f1).elt_typ;_tmp50B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB)->f1).elt_tq;_tmp50A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB)->f1).ptr_atts).rgn;_tmp509=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB)->f1).ptr_atts).nullable;_tmp508=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB)->f1).ptr_atts).bounds;_tmp507=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FB)->f1).ptr_atts).zero_term;_LL18: {
# 2895
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp4FC=({struct Cyc_List_List*_tmp506=_cycalloc(sizeof(*_tmp506));({struct _tuple0*_tmpC73=({struct _tuple0*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->f1=t1,_tmp505->f2=t2;_tmp505;});_tmp506->hd=_tmpC73;}),_tmp506->tl=0;_tmp506;});
int _tmp4FD=_tmp50B.real_const  || !_tmp51A.real_const;
# 2909 "tcutil.cyc"
int _tmp4FE=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp508,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp507);
# 2913
int _tmp4FF=_tmp4FD  && (
((_tmp4FE  && Cyc_Tcutil_ptrsubtype(te,_tmp4FC,_tmp51B,_tmp50C) || 
Cyc_Tcutil_unify(_tmp51B,_tmp50C)) || Cyc_Tcutil_isomorphic(_tmp51B,_tmp50C)) || Cyc_Tcutil_unify(_tmp50C,(void*)& Cyc_Absyn_VoidType_val));
# 2917
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp516,_tmp507) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp507);
# 2921
int _tmp500=_tmp4FF?0:((Cyc_Tcutil_bits_only(_tmp51B) && Cyc_Tcutil_is_char_type(_tmp50C)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp507)) && (
_tmp50B.real_const  || !_tmp51A.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp517,_tmp508);
if(!bounds_ok  && !_tmp500){
struct _tuple0 _tmp501=({struct _tuple0 _tmpA5E;({void*_tmpC75=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp517);_tmpA5E.f1=_tmpC75;}),({void*_tmpC74=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp508);_tmpA5E.f2=_tmpC74;});_tmpA5E;});struct _tuple0 _tmp502=_tmp501;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*_tmp503;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp502.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp502.f2)->tag == 1U){_LL1C: _tmp504=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp502.f1)->f1;_tmp503=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp502.f2)->f1;_LL1D:
# 2931
 if(Cyc_Evexp_lte_const_exp(_tmp503,_tmp504))
bounds_ok=1;
goto _LL1B;}else{goto _LL1E;}}else{_LL1E: _LL1F:
# 2936
 bounds_ok=1;goto _LL1B;}_LL1B:;}{
# 2938
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp518);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp509);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2945
if(((bounds_ok  && zeroterm_ok) && (_tmp4FF  || _tmp500)) && (
Cyc_Tcutil_unify(_tmp519,_tmp50A) || Cyc_Tcenv_region_outlives(te,_tmp519,_tmp50A)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 2951
return Cyc_Absyn_Unknown_coercion;case 8U: _LLA: _tmp51F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).elt_type;_tmp51E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).tq;_tmp51D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).num_elts;_tmp51C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FA)->f1).zero_term;_LLB:
# 2953
{void*_tmp50D=t2;void*_tmp511;struct Cyc_Absyn_Tqual _tmp510;struct Cyc_Absyn_Exp*_tmp50F;union Cyc_Absyn_Constraint*_tmp50E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50D)->tag == 8U){_LL21: _tmp511=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50D)->f1).elt_type;_tmp510=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50D)->f1).tq;_tmp50F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50D)->f1).num_elts;_tmp50E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50D)->f1).zero_term;_LL22: {
# 2955
int okay;
({int _tmpC76=
(((_tmp51D != 0  && _tmp50F != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp51C,_tmp50E)) && 
Cyc_Evexp_lte_const_exp(_tmp50F,_tmp51D)) && 
Cyc_Evexp_lte_const_exp(_tmp51D,_tmp50F);
# 2956
okay=_tmpC76;});
# 2960
return
# 2962
(okay  && Cyc_Tcutil_unify(_tmp51F,_tmp511)) && (!_tmp51E.real_const  || _tmp510.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL23: _LL24:
# 2964
 return Cyc_Absyn_Unknown_coercion;}_LL20:;}
# 2966
return Cyc_Absyn_Unknown_coercion;case 13U: _LLC: _tmp520=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FA)->f2;_LLD:
# 2970
{void*_tmp512=t2;struct Cyc_Absyn_Enumdecl*_tmp513;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp512)->tag == 13U){_LL26: _tmp513=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp512)->f2;_LL27:
# 2972
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp520))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp513))->fields != 0) && ({
int _tmpC77=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp520->fields))->v);_tmpC77 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp513->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL25;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}
# 2978
goto _LLF;case 6U: _LLE: _LLF:
 goto _LL11;case 7U: _LL10: _LL11:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15U: _LL12: _tmp521=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FA)->f1;_LL13:
# 2983
{void*_tmp514=t2;void*_tmp515;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp514)->tag == 15U){_LL2B: _tmp515=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp514)->f1;_LL2C:
# 2985
 if(Cyc_Tcenv_region_outlives(te,_tmp521,_tmp515))return Cyc_Absyn_No_coercion;
goto _LL2A;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}
# 2989
return Cyc_Absyn_Unknown_coercion;default: _LL14: _LL15:
 return Cyc_Absyn_Unknown_coercion;}_LL7:;};}
# 2995
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp522=Cyc_Absyn_copy_exp(e);
({void*_tmpC78=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->tag=14U,_tmp523->f1=t,_tmp523->f2=_tmp522,_tmp523->f3=0,_tmp523->f4=c;_tmp523;});e->r=_tmpC78;});
e->topt=t;}}
# 3003
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp524=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp525=_tmp524;switch(*((int*)_tmp525)){case 6U: _LL1: _LL2:
 goto _LL4;case 13U: _LL3: _LL4:
 goto _LL6;case 14U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 return 1;case 1U: _LL9: _LLA:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3014
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp526=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp527=_tmp526;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp527)->tag == 7U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 3023
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp528=Cyc_Tcutil_compress(t);void*_tmp529=_tmp528;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp529)->tag == 9U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 3030
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp52A=({struct _tuple0 _tmpA5F;_tmpA5F.f1=t1,_tmpA5F.f2=t2;_tmpA5F;});struct _tuple0 _tmp52B=_tmp52A;int _tmp52D;int _tmp52C;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52B.f1)->tag == 7U){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52B.f2)->tag == 7U){_LL1: _tmp52D=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52B.f1)->f1;_tmp52C=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52B.f2)->f1;_LL2:
# 3033
 if(_tmp52D != 0  && _tmp52D != 1)return t1;else{
if(_tmp52C != 0  && _tmp52C != 1)return t2;else{
if(_tmp52D >= _tmp52C)return t1;else{
return t2;}}}}else{_LL3: _LL4:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52B.f2)->tag == 7U){_LL5: _LL6:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 goto _LLA;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2 == Cyc_Absyn_Long_sz)goto _LLF;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL11;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL13;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL15;else{goto _LL1B;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2){case Cyc_Absyn_Long_sz: goto _LLF;case Cyc_Absyn_Int_sz: goto _LL13;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2){case Cyc_Absyn_Long_sz: _LLF: _LL10:
# 3043
 goto _LL12;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 3046
 goto _LL16;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL11;case Cyc_Absyn_Int_sz: goto _LL15;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{goto _LL1B;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f1)->f2){case Cyc_Absyn_LongLong_sz: _LLB: _LLC:
# 3041
 goto _LLE;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 3048
 goto _LL1A;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL9: _LLA:
# 3040
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL11: _LL12:
# 3044
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL15: _LL16:
# 3047
 return Cyc_Absyn_uint_typ;default: goto _LL1B;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52B.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLD: _LLE:
# 3042
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 3049
 return Cyc_Absyn_slong_typ;default: goto _LL1B;}}}else{_LL1B: _LL1C:
 return Cyc_Absyn_sint_typ;}}}}_LL0:;}
# 3061 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpC79=Cyc_Absyn_compress_kb(c1);c1=_tmpC79;});
({void*_tmpC7A=Cyc_Absyn_compress_kb(c2);c2=_tmpC7A;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp52E=({struct _tuple0 _tmpA60;_tmpA60.f1=c1,_tmpA60.f2=c2;_tmpA60;});struct _tuple0 _tmp52F=_tmp52E;struct Cyc_Core_Opt**_tmp543;struct Cyc_Absyn_Kind*_tmp542;struct Cyc_Core_Opt**_tmp541;struct Cyc_Absyn_Kind*_tmp540;struct Cyc_Core_Opt**_tmp53F;struct Cyc_Absyn_Kind*_tmp53E;struct Cyc_Absyn_Kind*_tmp53D;struct Cyc_Core_Opt**_tmp53C;struct Cyc_Core_Opt**_tmp53B;struct Cyc_Absyn_Kind*_tmp53A;struct Cyc_Core_Opt**_tmp539;struct Cyc_Absyn_Kind*_tmp538;struct Cyc_Absyn_Kind*_tmp537;struct Cyc_Absyn_Kind*_tmp536;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp52F.f1)->tag == 0U)switch(*((int*)_tmp52F.f2)){case 0U: _LL1: _tmp537=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp52F.f1)->f1;_tmp536=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp52F.f2)->f1;_LL2:
 return _tmp537 == _tmp536;case 1U: goto _LL3;default: _LL9: _tmp53A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp52F.f1)->f1;_tmp539=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f2)->f1;_tmp538=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f2)->f2;_LLA:
# 3074
 if(Cyc_Tcutil_kind_leq(_tmp53A,_tmp538)){
({struct Cyc_Core_Opt*_tmpC7B=({struct Cyc_Core_Opt*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->v=c1;_tmp533;});*_tmp539=_tmpC7B;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp52F.f2)->tag == 1U){_LL3: _tmp53B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp52F.f2)->f1;_LL4:
# 3067
({struct Cyc_Core_Opt*_tmpC7C=({struct Cyc_Core_Opt*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->v=c1;_tmp530;});*_tmp53B=_tmpC7C;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp52F.f1)->tag == 1U){_LL5: _tmp53C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp52F.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpC7D=({struct Cyc_Core_Opt*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->v=c2;_tmp531;});*_tmp53C=_tmpC7D;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp52F.f2)->tag == 0U){_LL7: _tmp53F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f1)->f1;_tmp53E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f1)->f2;_tmp53D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp52F.f2)->f1;_LL8:
# 3070
 if(Cyc_Tcutil_kind_leq(_tmp53D,_tmp53E)){
({struct Cyc_Core_Opt*_tmpC7E=({struct Cyc_Core_Opt*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->v=c2;_tmp532;});*_tmp53F=_tmpC7E;});return 1;}else{
return 0;}}else{_LLB: _tmp543=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f1)->f1;_tmp542=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f1)->f2;_tmp541=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f2)->f1;_tmp540=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52F.f2)->f2;_LLC:
# 3078
 if(Cyc_Tcutil_kind_leq(_tmp542,_tmp540)){
({struct Cyc_Core_Opt*_tmpC7F=({struct Cyc_Core_Opt*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->v=c1;_tmp534;});*_tmp541=_tmpC7F;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp540,_tmp542)){
({struct Cyc_Core_Opt*_tmpC80=({struct Cyc_Core_Opt*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->v=c2;_tmp535;});*_tmp543=_tmpC80;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3087
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3092
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp549=({struct Cyc_Int_pa_PrintArg_struct _tmpA61;_tmpA61.tag=1U,_tmpA61.f1=(unsigned long)i;_tmpA61;});void*_tmp547[1U];_tmp547[0]=& _tmp549;({struct _dyneither_ptr _tmpC81=({const char*_tmp548="#%d";_tag_dyneither(_tmp548,sizeof(char),4U);});Cyc_aprintf(_tmpC81,_tag_dyneither(_tmp547,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp546=_cycalloc(sizeof(*_tmp546));({struct _dyneither_ptr*_tmpC82=({unsigned int _tmp545=1;struct _dyneither_ptr*_tmp544=_cycalloc(_check_times(_tmp545,sizeof(struct _dyneither_ptr)));_tmp544[0]=s;_tmp544;});_tmp546->name=_tmpC82;}),_tmp546->identity=- 1,_tmp546->kind=k;_tmp546;});}
# 3099
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp54A=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp54A,sizeof(char),0))== '#';}
# 3104
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp54D=({struct Cyc_String_pa_PrintArg_struct _tmpA62;_tmpA62.tag=0U,_tmpA62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmpA62;});void*_tmp54B[1U];_tmp54B[0]=& _tmp54D;({struct _dyneither_ptr _tmpC83=({const char*_tmp54C="%s";_tag_dyneither(_tmp54C,sizeof(char),3U);});Cyc_printf(_tmpC83,_tag_dyneither(_tmp54B,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp54E=({struct _dyneither_ptr _tmpC84=({const char*_tmp554="`";_tag_dyneither(_tmp554,sizeof(char),2U);});Cyc_strconcat(_tmpC84,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp54F=_dyneither_ptr_plus(_tmp54E,sizeof(char),1);char _tmp550=*((char*)_check_dyneither_subscript(_tmp54F,sizeof(char),0U));char _tmp551='t';if(_get_dyneither_size(_tmp54F,sizeof(char))== 1U  && (_tmp550 == '\000'  && _tmp551 != '\000'))_throw_arraybounds();*((char*)_tmp54F.curr)=_tmp551;});
({struct _dyneither_ptr*_tmpC85=({unsigned int _tmp553=1;struct _dyneither_ptr*_tmp552=_cycalloc(_check_times(_tmp553,sizeof(struct _dyneither_ptr)));_tmp552[0]=(struct _dyneither_ptr)_tmp54E;_tmp552;});t->name=_tmpC85;});};}
# 3113
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3115
return({struct _tuple10*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->f1=(*x).f1,_tmp555->f2=(*x).f2,_tmp555->f3=(*x).f3;_tmp555;});}
# 3118
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3125
struct Cyc_List_List*_tmp556=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpC86=({struct Cyc_List_List*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->hd=(void*)atts->hd,_tmp557->tl=_tmp556;_tmp557;});_tmp556=_tmpC86;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->tag=9U,(_tmp558->f1).tvars=fd->tvs,(_tmp558->f1).effect=fd->effect,(_tmp558->f1).ret_tqual=fd->ret_tqual,(_tmp558->f1).ret_typ=fd->ret_type,({
struct Cyc_List_List*_tmpC87=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);(_tmp558->f1).args=_tmpC87;}),(_tmp558->f1).c_varargs=fd->c_varargs,(_tmp558->f1).cyc_varargs=fd->cyc_varargs,(_tmp558->f1).rgn_po=fd->rgn_po,(_tmp558->f1).attributes=_tmp556,(_tmp558->f1).requires_clause=fd->requires_clause,(_tmp558->f1).requires_relns=fd->requires_relns,(_tmp558->f1).ensures_clause=fd->ensures_clause,(_tmp558->f1).ensures_relns=fd->ensures_relns;_tmp558;});}
# 3138
return(void*)_check_null(fd->cached_typ);}
# 3144
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3146
union Cyc_Relations_RelnOp _tmp559=*rop;union Cyc_Relations_RelnOp _tmp55A=_tmp559;struct Cyc_Absyn_Vardecl*_tmp56D;struct Cyc_Absyn_Vardecl*_tmp56C;switch((_tmp55A.RNumelts).tag){case 2U: _LL1: _tmp56C=(_tmp55A.RVar).val;_LL2: {
# 3148
struct _tuple2 _tmp55B=*_tmp56C->name;struct _tuple2 _tmp55C=_tmp55B;union Cyc_Absyn_Nmspace _tmp563;struct _dyneither_ptr*_tmp562;_LL8: _tmp563=_tmp55C.f1;_tmp562=_tmp55C.f2;_LL9:;
if(!((int)((_tmp563.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpC88=(struct _dyneither_ptr)*_tmp562;Cyc_strcmp(_tmpC88,({const char*_tmp55D="return_value";_tag_dyneither(_tmp55D,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpC89=Cyc_Relations_RReturn();*rop=_tmpC89;});
goto _LL0;}{
# 3154
unsigned int c=0U;
{struct Cyc_List_List*_tmp55E=args;for(0;_tmp55E != 0;(_tmp55E=_tmp55E->tl,c ++)){
struct _tuple10*_tmp55F=(struct _tuple10*)_tmp55E->hd;struct _tuple10*_tmp560=_tmp55F;struct _dyneither_ptr*_tmp561;_LLB: _tmp561=_tmp560->f1;_LLC:;
if(_tmp561 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp562,(struct _dyneither_ptr)*_tmp561)== 0){
({union Cyc_Relations_RelnOp _tmpC8A=Cyc_Relations_RParam(c);*rop=_tmpC8A;});
break;}}}}
# 3164
goto _LL0;};}case 3U: _LL3: _tmp56D=(_tmp55A.RNumelts).val;_LL4: {
# 3166
struct _tuple2 _tmp564=*_tmp56D->name;struct _tuple2 _tmp565=_tmp564;union Cyc_Absyn_Nmspace _tmp56B;struct _dyneither_ptr*_tmp56A;_LLE: _tmp56B=_tmp565.f1;_tmp56A=_tmp565.f2;_LLF:;
if(!((int)((_tmp56B.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp566=args;for(0;_tmp566 != 0;(_tmp566=_tmp566->tl,c ++)){
struct _tuple10*_tmp567=(struct _tuple10*)_tmp566->hd;struct _tuple10*_tmp568=_tmp567;struct _dyneither_ptr*_tmp569;_LL11: _tmp569=_tmp568->f1;_LL12:;
if(_tmp569 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp56A,(struct _dyneither_ptr)*_tmp569)== 0){
({union Cyc_Relations_RelnOp _tmpC8B=Cyc_Relations_RParamNumelts(c);*rop=_tmpC8B;});
break;}}}}
# 3178
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3183
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3185
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3189
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3191
if(e == 0)return 0;{
struct Cyc_List_List*_tmp56E=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp56E);
return _tmp56E;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3198
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp56F=pr;struct Cyc_Absyn_Tqual _tmp572;void*_tmp571;_LL1: _tmp572=_tmp56F->f1;_tmp571=_tmp56F->f2;_LL2:;
if(_tmp571 == t)return pr;else{
return({struct _tuple12*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->f1=_tmp572,_tmp570->f2=t;_tmp570;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3205
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3207
return({struct _tuple27*_tmp574=_region_malloc(rgn,sizeof(*_tmp574));({struct _tuple26*_tmpC8C=({struct _tuple26*_tmp573=_region_malloc(rgn,sizeof(*_tmp573));_tmp573->f1=(*y).f1,_tmp573->f2=(*y).f2;_tmp573;});_tmp574->f1=_tmpC8C;}),_tmp574->f2=(*y).f3;_tmp574;});}
# 3209
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3211
struct _tuple10 _tmp575=*orig_arg;struct _tuple10 _tmp576=_tmp575;struct _dyneither_ptr*_tmp57A;struct Cyc_Absyn_Tqual _tmp579;void*_tmp578;_LL1: _tmp57A=_tmp576.f1;_tmp579=_tmp576.f2;_tmp578=_tmp576.f3;_LL2:;
if(t == _tmp578)return orig_arg;
return({struct _tuple10*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->f1=_tmp57A,_tmp577->f2=_tmp579,_tmp577->f3=t;_tmp577;});}
# 3215
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3218
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3223
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3225
return({struct Cyc_Absyn_Exp*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->topt=old->topt,_tmp57B->r=r,_tmp57B->loc=old->loc,_tmp57B->annot=old->annot;_tmp57B;});}
# 3230
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp57C=e->r;void*_tmp57D=_tmp57C;void*_tmp5BA;void*_tmp5B9;struct Cyc_List_List*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5B7;void*_tmp5B6;void*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B4;int _tmp5B3;enum Cyc_Absyn_Coercion _tmp5B2;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5A9;enum Cyc_Absyn_Primop _tmp5A8;struct Cyc_List_List*_tmp5A7;switch(*((int*)_tmp57D)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp5A8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_tmp5A7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp57D)->f2;_LLC:
# 3239
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5A7)== 1){
struct Cyc_Absyn_Exp*_tmp57E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5A7))->hd;
struct Cyc_Absyn_Exp*_tmp57F=Cyc_Tcutil_rsubsexp(r,inst,_tmp57E);
if(_tmp57F == _tmp57E)return e;
return({struct Cyc_Absyn_Exp*_tmpC8E=e;Cyc_Tcutil_copye(_tmpC8E,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->tag=3U,_tmp581->f1=_tmp5A8,({struct Cyc_List_List*_tmpC8D=({struct Cyc_Absyn_Exp*_tmp580[1U];_tmp580[0]=_tmp57F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp580,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp581->f2=_tmpC8D;});_tmp581;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5A7)== 2){
struct Cyc_Absyn_Exp*_tmp582=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5A7))->hd;
struct Cyc_Absyn_Exp*_tmp583=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5A7->tl))->hd;
struct Cyc_Absyn_Exp*_tmp584=Cyc_Tcutil_rsubsexp(r,inst,_tmp582);
struct Cyc_Absyn_Exp*_tmp585=Cyc_Tcutil_rsubsexp(r,inst,_tmp583);
if(_tmp584 == _tmp582  && _tmp585 == _tmp583)return e;
return({struct Cyc_Absyn_Exp*_tmpC90=e;Cyc_Tcutil_copye(_tmpC90,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->tag=3U,_tmp587->f1=_tmp5A8,({struct Cyc_List_List*_tmpC8F=({struct Cyc_Absyn_Exp*_tmp586[2U];_tmp586[0]=_tmp584,_tmp586[1]=_tmp585;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp586,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp587->f2=_tmpC8F;});_tmp587;}));});}else{
return({void*_tmp588=0U;({struct _dyneither_ptr _tmpC91=({const char*_tmp589="primop does not have 1 or 2 args!";_tag_dyneither(_tmp589,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC91,_tag_dyneither(_tmp588,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp5AB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_tmp5AA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp57D)->f2;_tmp5A9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp57D)->f3;_LLE: {
# 3253
struct Cyc_Absyn_Exp*_tmp58A=Cyc_Tcutil_rsubsexp(r,inst,_tmp5AB);
struct Cyc_Absyn_Exp*_tmp58B=Cyc_Tcutil_rsubsexp(r,inst,_tmp5AA);
struct Cyc_Absyn_Exp*_tmp58C=Cyc_Tcutil_rsubsexp(r,inst,_tmp5A9);
if((_tmp58A == _tmp5AB  && _tmp58B == _tmp5AA) && _tmp58C == _tmp5A9)return e;
return({struct Cyc_Absyn_Exp*_tmpC92=e;Cyc_Tcutil_copye(_tmpC92,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->tag=6U,_tmp58D->f1=_tmp58A,_tmp58D->f2=_tmp58B,_tmp58D->f3=_tmp58C;_tmp58D;}));});}case 7U: _LLF: _tmp5AD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_tmp5AC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp57D)->f2;_LL10: {
# 3259
struct Cyc_Absyn_Exp*_tmp58E=Cyc_Tcutil_rsubsexp(r,inst,_tmp5AD);
struct Cyc_Absyn_Exp*_tmp58F=Cyc_Tcutil_rsubsexp(r,inst,_tmp5AC);
if(_tmp58E == _tmp5AD  && _tmp58F == _tmp5AC)return e;
return({struct Cyc_Absyn_Exp*_tmpC93=e;Cyc_Tcutil_copye(_tmpC93,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->tag=7U,_tmp590->f1=_tmp58E,_tmp590->f2=_tmp58F;_tmp590;}));});}case 8U: _LL11: _tmp5AF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_tmp5AE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp57D)->f2;_LL12: {
# 3264
struct Cyc_Absyn_Exp*_tmp591=Cyc_Tcutil_rsubsexp(r,inst,_tmp5AF);
struct Cyc_Absyn_Exp*_tmp592=Cyc_Tcutil_rsubsexp(r,inst,_tmp5AE);
if(_tmp591 == _tmp5AF  && _tmp592 == _tmp5AE)return e;
return({struct Cyc_Absyn_Exp*_tmpC94=e;Cyc_Tcutil_copye(_tmpC94,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->tag=8U,_tmp593->f1=_tmp591,_tmp593->f2=_tmp592;_tmp593;}));});}case 9U: _LL13: _tmp5B1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_tmp5B0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp57D)->f2;_LL14: {
# 3269
struct Cyc_Absyn_Exp*_tmp594=Cyc_Tcutil_rsubsexp(r,inst,_tmp5B1);
struct Cyc_Absyn_Exp*_tmp595=Cyc_Tcutil_rsubsexp(r,inst,_tmp5B0);
if(_tmp594 == _tmp5B1  && _tmp595 == _tmp5B0)return e;
return({struct Cyc_Absyn_Exp*_tmpC95=e;Cyc_Tcutil_copye(_tmpC95,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->tag=9U,_tmp596->f1=_tmp594,_tmp596->f2=_tmp595;_tmp596;}));});}case 14U: _LL15: _tmp5B5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_tmp5B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp57D)->f2;_tmp5B3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp57D)->f3;_tmp5B2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp57D)->f4;_LL16: {
# 3274
struct Cyc_Absyn_Exp*_tmp597=Cyc_Tcutil_rsubsexp(r,inst,_tmp5B4);
void*_tmp598=Cyc_Tcutil_rsubstitute(r,inst,_tmp5B5);
if(_tmp597 == _tmp5B4  && _tmp598 == _tmp5B5)return e;
return({struct Cyc_Absyn_Exp*_tmpC96=e;Cyc_Tcutil_copye(_tmpC96,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->tag=14U,_tmp599->f1=_tmp598,_tmp599->f2=_tmp597,_tmp599->f3=_tmp5B3,_tmp599->f4=_tmp5B2;_tmp599;}));});}case 17U: _LL17: _tmp5B6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_LL18: {
# 3279
void*_tmp59A=Cyc_Tcutil_rsubstitute(r,inst,_tmp5B6);
if(_tmp59A == _tmp5B6)return e;
return({struct Cyc_Absyn_Exp*_tmpC97=e;Cyc_Tcutil_copye(_tmpC97,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->tag=17U,_tmp59B->f1=_tmp59A;_tmp59B;}));});}case 18U: _LL19: _tmp5B7=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_LL1A: {
# 3283
struct Cyc_Absyn_Exp*_tmp59C=Cyc_Tcutil_rsubsexp(r,inst,_tmp5B7);
if(_tmp59C == _tmp5B7)return e;
return({struct Cyc_Absyn_Exp*_tmpC98=e;Cyc_Tcutil_copye(_tmpC98,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->tag=18U,_tmp59D->f1=_tmp59C;_tmp59D;}));});}case 19U: _LL1B: _tmp5B9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_tmp5B8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp57D)->f2;_LL1C: {
# 3287
void*_tmp59E=Cyc_Tcutil_rsubstitute(r,inst,_tmp5B9);
if(_tmp59E == _tmp5B9)return e;
return({struct Cyc_Absyn_Exp*_tmpC99=e;Cyc_Tcutil_copye(_tmpC99,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->tag=19U,_tmp59F->f1=_tmp59E,_tmp59F->f2=_tmp5B8;_tmp59F;}));});}case 39U: _LL1D: _tmp5BA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp57D)->f1;_LL1E: {
# 3291
void*_tmp5A0=Cyc_Tcutil_rsubstitute(r,inst,_tmp5BA);
if(_tmp5A0 == _tmp5BA)return e;{
# 3294
void*_tmp5A1=Cyc_Tcutil_compress(_tmp5A0);void*_tmp5A2=_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A4;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5A2)->tag == 18U){_LL22: _tmp5A4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5A2)->f1;_LL23:
 return _tmp5A4;}else{_LL24: _LL25:
# 3297
 return({struct Cyc_Absyn_Exp*_tmpC9A=e;Cyc_Tcutil_copye(_tmpC9A,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->tag=39U,_tmp5A3->f1=_tmp5A0;_tmp5A3;}));});}_LL21:;};}default: _LL1F: _LL20:
# 3300
 return({void*_tmp5A5=0U;({struct _dyneither_ptr _tmpC9B=({const char*_tmp5A6="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp5A6,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC9B,_tag_dyneither(_tmp5A5,sizeof(void*),0U));});});}_LL0:;}
# 3304
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3307
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3311
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3314
void*_tmp5BB=f->type;
struct Cyc_Absyn_Exp*_tmp5BC=f->requires_clause;
void*_tmp5BD=Cyc_Tcutil_rsubstitute(r,inst,_tmp5BB);
struct Cyc_Absyn_Exp*_tmp5BE=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp5BC);
if(_tmp5BB == _tmp5BD  && _tmp5BC == _tmp5BE)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->name=f->name,_tmp5BF->tq=f->tq,_tmp5BF->type=_tmp5BD,_tmp5BF->width=f->width,_tmp5BF->attributes=f->attributes,_tmp5BF->requires_clause=_tmp5BE;_tmp5BF;});}}
# 3324
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3327
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp5C0=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp5C1=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp5C2=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp5C0);
struct Cyc_List_List*_tmp5C3=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp5C1);
if(_tmp5C2 == _tmp5C0  && _tmp5C3 == _tmp5C1)return fs;
# 3334
return({struct Cyc_List_List*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->hd=_tmp5C2,_tmp5C4->tl=_tmp5C3;_tmp5C4;});};}
# 3337
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3340
struct _tuple1 _tmp5C5=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp5C6=_tmp5C5;struct Cyc_List_List*_tmp5CA;struct Cyc_List_List*_tmp5C9;_LL1: _tmp5CA=_tmp5C6.f1;_tmp5C9=_tmp5C6.f2;_LL2:;{
struct Cyc_List_List*_tmp5C7=Cyc_Tcutil_substs(rgn,inst,_tmp5CA);
struct Cyc_List_List*_tmp5C8=Cyc_Tcutil_substs(rgn,inst,_tmp5C9);
if(_tmp5C7 == _tmp5CA  && _tmp5C8 == _tmp5C9)
return rgn_po;else{
# 3346
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp5C7,_tmp5C8);}};}
# 3349
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3352
void*_tmp5CB=Cyc_Tcutil_compress(t);void*_tmp5CC=_tmp5CB;struct Cyc_List_List*_tmp644;void*_tmp643;void*_tmp642;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp640;void*_tmp63F;void*_tmp63E;void*_tmp63D;void*_tmp63C;void*_tmp63B;enum Cyc_Absyn_AggrKind _tmp63A;struct Cyc_List_List*_tmp639;struct Cyc_List_List*_tmp638;struct Cyc_List_List*_tmp637;void*_tmp636;struct Cyc_Absyn_Tqual _tmp635;void*_tmp634;struct Cyc_List_List*_tmp633;int _tmp632;struct Cyc_Absyn_VarargInfo*_tmp631;struct Cyc_List_List*_tmp630;struct Cyc_List_List*_tmp62F;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Exp*_tmp62D;void*_tmp62C;struct Cyc_Absyn_Tqual _tmp62B;void*_tmp62A;union Cyc_Absyn_Constraint*_tmp629;union Cyc_Absyn_Constraint*_tmp628;union Cyc_Absyn_Constraint*_tmp627;void*_tmp626;struct Cyc_Absyn_Tqual _tmp625;struct Cyc_Absyn_Exp*_tmp624;union Cyc_Absyn_Constraint*_tmp623;unsigned int _tmp622;struct _tuple2*_tmp621;struct Cyc_List_List*_tmp620;struct Cyc_Absyn_Typedefdecl*_tmp61F;void*_tmp61E;union Cyc_Absyn_DatatypeFieldInfoU _tmp61D;struct Cyc_List_List*_tmp61C;union Cyc_Absyn_DatatypeInfoU _tmp61B;struct Cyc_List_List*_tmp61A;union Cyc_Absyn_AggrInfoU _tmp619;struct Cyc_List_List*_tmp618;struct Cyc_Absyn_Tvar*_tmp617;switch(*((int*)_tmp5CC)){case 2U: _LL1: _tmp617=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5CC)->f1;_LL2: {
# 3355
struct _handler_cons _tmp5CD;_push_handler(& _tmp5CD);{int _tmp5CF=0;if(setjmp(_tmp5CD.handler))_tmp5CF=1;if(!_tmp5CF){{void*_tmp5D0=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp617);_npop_handler(0U);return _tmp5D0;};_pop_handler();}else{void*_tmp5CE=(void*)_exn_thrown;void*_tmp5D1=_tmp5CE;void*_tmp5D2;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp5D1)->tag == Cyc_Core_Not_found){_LL3C: _LL3D:
 return t;}else{_LL3E: _tmp5D2=_tmp5D1;_LL3F:(int)_rethrow(_tmp5D2);}_LL3B:;}};}case 11U: _LL3: _tmp619=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5CC)->f1).aggr_info;_tmp618=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5CC)->f1).targs;_LL4: {
# 3358
struct Cyc_List_List*_tmp5D3=Cyc_Tcutil_substs(rgn,inst,_tmp618);
return _tmp5D3 == _tmp618?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->tag=11U,(_tmp5D4->f1).aggr_info=_tmp619,(_tmp5D4->f1).targs=_tmp5D3;_tmp5D4;});}case 3U: _LL5: _tmp61B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5CC)->f1).datatype_info;_tmp61A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5CC)->f1).targs;_LL6: {
# 3361
struct Cyc_List_List*_tmp5D5=Cyc_Tcutil_substs(rgn,inst,_tmp61A);
return _tmp5D5 == _tmp61A?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));
_tmp5D6->tag=3U,(_tmp5D6->f1).datatype_info=_tmp61B,(_tmp5D6->f1).targs=_tmp5D5;_tmp5D6;});}case 4U: _LL7: _tmp61D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5CC)->f1).field_info;_tmp61C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp5CC)->f1).targs;_LL8: {
# 3365
struct Cyc_List_List*_tmp5D7=Cyc_Tcutil_substs(rgn,inst,_tmp61C);
return _tmp5D7 == _tmp61C?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));
_tmp5D8->tag=4U,(_tmp5D8->f1).field_info=_tmp61D,(_tmp5D8->f1).targs=_tmp5D7;_tmp5D8;});}case 17U: _LL9: _tmp621=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5CC)->f1;_tmp620=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5CC)->f2;_tmp61F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5CC)->f3;_tmp61E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5CC)->f4;_LLA: {
# 3369
struct Cyc_List_List*_tmp5D9=Cyc_Tcutil_substs(rgn,inst,_tmp620);
return _tmp5D9 == _tmp620?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->tag=17U,_tmp5DA->f1=_tmp621,_tmp5DA->f2=_tmp5D9,_tmp5DA->f3=_tmp61F,_tmp5DA->f4=_tmp61E;_tmp5DA;});}case 8U: _LLB: _tmp626=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5CC)->f1).elt_type;_tmp625=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5CC)->f1).tq;_tmp624=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5CC)->f1).num_elts;_tmp623=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5CC)->f1).zero_term;_tmp622=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5CC)->f1).zt_loc;_LLC: {
# 3372
void*_tmp5DB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp626);
struct Cyc_Absyn_Exp*_tmp5DC=_tmp624 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp624);
return _tmp5DB == _tmp626  && _tmp5DC == _tmp624?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));
_tmp5DD->tag=8U,(_tmp5DD->f1).elt_type=_tmp5DB,(_tmp5DD->f1).tq=_tmp625,(_tmp5DD->f1).num_elts=_tmp5DC,(_tmp5DD->f1).zero_term=_tmp623,(_tmp5DD->f1).zt_loc=_tmp622;_tmp5DD;});}case 5U: _LLD: _tmp62C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CC)->f1).elt_typ;_tmp62B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CC)->f1).elt_tq;_tmp62A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CC)->f1).ptr_atts).rgn;_tmp629=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CC)->f1).ptr_atts).nullable;_tmp628=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CC)->f1).ptr_atts).bounds;_tmp627=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CC)->f1).ptr_atts).zero_term;_LLE: {
# 3377
void*_tmp5DE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp62C);
void*_tmp5DF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp62A);
union Cyc_Absyn_Constraint*_tmp5E0=_tmp628;
{void*_tmp5E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp628);void*_tmp5E2=_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E5;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2)->tag == 1U){_LL41: _tmp5E5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2)->f1;_LL42: {
# 3382
struct Cyc_Absyn_Exp*_tmp5E3=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5E5);
if(_tmp5E3 != _tmp5E5)
({union Cyc_Absyn_Constraint*_tmpC9C=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->tag=1U,_tmp5E4->f1=_tmp5E3;_tmp5E4;}));_tmp5E0=_tmpC9C;});
goto _LL40;}}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3388
if((_tmp5DE == _tmp62C  && _tmp5DF == _tmp62A) && _tmp5E0 == _tmp628)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->tag=5U,(_tmp5E6->f1).elt_typ=_tmp5DE,(_tmp5E6->f1).elt_tq=_tmp62B,((_tmp5E6->f1).ptr_atts).rgn=_tmp5DF,((_tmp5E6->f1).ptr_atts).nullable=_tmp629,((_tmp5E6->f1).ptr_atts).bounds=_tmp5E0,((_tmp5E6->f1).ptr_atts).zero_term=_tmp627,((_tmp5E6->f1).ptr_atts).ptrloc=0;_tmp5E6;});}case 9U: _LLF: _tmp637=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).tvars;_tmp636=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).effect;_tmp635=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).ret_tqual;_tmp634=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).ret_typ;_tmp633=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).args;_tmp632=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).c_varargs;_tmp631=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).cyc_varargs;_tmp630=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).rgn_po;_tmp62F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).attributes;_tmp62E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).requires_clause;_tmp62D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5CC)->f1).ensures_clause;_LL10:
# 3394
{struct Cyc_List_List*_tmp5E7=_tmp637;for(0;_tmp5E7 != 0;_tmp5E7=_tmp5E7->tl){
({struct Cyc_List_List*_tmpC9F=({struct Cyc_List_List*_tmp5EA=_region_malloc(rgn,sizeof(*_tmp5EA));({struct _tuple16*_tmpC9E=({struct _tuple16*_tmp5E9=_region_malloc(rgn,sizeof(*_tmp5E9));_tmp5E9->f1=(struct Cyc_Absyn_Tvar*)_tmp5E7->hd,({void*_tmpC9D=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->tag=2U,_tmp5E8->f1=(struct Cyc_Absyn_Tvar*)_tmp5E7->hd;_tmp5E8;});_tmp5E9->f2=_tmpC9D;});_tmp5E9;});_tmp5EA->hd=_tmpC9E;}),_tmp5EA->tl=inst;_tmp5EA;});inst=_tmpC9F;});}}{
struct _tuple1 _tmp5EB=({struct _RegionHandle*_tmpCA1=rgn;struct _RegionHandle*_tmpCA0=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpCA1,_tmpCA0,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp633));});
# 3396
struct _tuple1 _tmp5EC=_tmp5EB;struct Cyc_List_List*_tmp5FB;struct Cyc_List_List*_tmp5FA;_LL46: _tmp5FB=_tmp5EC.f1;_tmp5FA=_tmp5EC.f2;_LL47:;{
# 3398
struct Cyc_List_List*_tmp5ED=_tmp633;
struct Cyc_List_List*_tmp5EE=Cyc_Tcutil_substs(rgn,inst,_tmp5FA);
if(_tmp5EE != _tmp5FA)
({struct Cyc_List_List*_tmpCA2=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp633,_tmp5EE);_tmp5ED=_tmpCA2;});{
void*eff2;
if(_tmp636 == 0)
eff2=0;else{
# 3406
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp636);
if(new_eff == _tmp636)
eff2=_tmp636;else{
# 3410
eff2=new_eff;}}{
# 3412
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp631 == 0)
cyc_varargs2=0;else{
# 3416
struct Cyc_Absyn_VarargInfo _tmp5EF=*_tmp631;struct Cyc_Absyn_VarargInfo _tmp5F0=_tmp5EF;struct _dyneither_ptr*_tmp5F6;struct Cyc_Absyn_Tqual _tmp5F5;void*_tmp5F4;int _tmp5F3;_LL49: _tmp5F6=_tmp5F0.name;_tmp5F5=_tmp5F0.tq;_tmp5F4=_tmp5F0.type;_tmp5F3=_tmp5F0.inject;_LL4A:;{
void*_tmp5F1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5F4);
if(_tmp5F1 == _tmp5F4)cyc_varargs2=_tmp631;else{
# 3420
({struct Cyc_Absyn_VarargInfo*_tmpCA3=({struct Cyc_Absyn_VarargInfo*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->name=_tmp5F6,_tmp5F2->tq=_tmp5F5,_tmp5F2->type=_tmp5F1,_tmp5F2->inject=_tmp5F3;_tmp5F2;});cyc_varargs2=_tmpCA3;});}};}{
# 3422
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp630);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp62E);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp62D);
struct Cyc_List_List*_tmp5F7=Cyc_Tcutil_extract_relns(_tmp5ED,req2);
struct Cyc_List_List*_tmp5F8=Cyc_Tcutil_extract_relns(_tmp5ED,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));
_tmp5F9->tag=9U,(_tmp5F9->f1).tvars=_tmp637,(_tmp5F9->f1).effect=eff2,(_tmp5F9->f1).ret_tqual=_tmp635,({void*_tmpCA4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp634);(_tmp5F9->f1).ret_typ=_tmpCA4;}),(_tmp5F9->f1).args=_tmp5ED,(_tmp5F9->f1).c_varargs=_tmp632,(_tmp5F9->f1).cyc_varargs=cyc_varargs2,(_tmp5F9->f1).rgn_po=rgn_po2,(_tmp5F9->f1).attributes=_tmp62F,(_tmp5F9->f1).requires_clause=req2,(_tmp5F9->f1).requires_relns=_tmp5F7,(_tmp5F9->f1).ensures_clause=ens2,(_tmp5F9->f1).ensures_relns=_tmp5F8;_tmp5F9;});};};};};};case 10U: _LL11: _tmp638=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5CC)->f1;_LL12: {
# 3432
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp5FC=_tmp638;for(0;_tmp5FC != 0;_tmp5FC=_tmp5FC->tl){
void*_tmp5FD=(*((struct _tuple12*)_tmp5FC->hd)).f2;
void*_tmp5FE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5FD);
if(_tmp5FD != _tmp5FE)
change=1;
# 3440
({struct Cyc_List_List*_tmpCA5=({struct Cyc_List_List*_tmp5FF=_region_malloc(rgn,sizeof(*_tmp5FF));_tmp5FF->hd=_tmp5FE,_tmp5FF->tl=ts2;_tmp5FF;});ts2=_tmpCA5;});}}
# 3442
if(!change)
return t;{
struct Cyc_List_List*_tmp600=({struct Cyc_List_List*_tmpCA6=_tmp638;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpCA6,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->tag=10U,_tmp601->f1=_tmp600;_tmp601;});};}case 12U: _LL13: _tmp63A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5CC)->f1;_tmp639=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5CC)->f2;_LL14: {
# 3447
struct Cyc_List_List*_tmp602=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp639);
if(_tmp639 == _tmp602)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->tag=12U,_tmp603->f1=_tmp63A,_tmp603->f2=_tmp602;_tmp603;});}case 1U: _LL15: _tmp63B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5CC)->f2;_LL16:
# 3451
 if(_tmp63B != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63B);else{
return t;}case 15U: _LL17: _tmp63C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5CC)->f1;_LL18: {
# 3454
void*_tmp604=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63C);
return _tmp604 == _tmp63C?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->tag=15U,_tmp605->f1=_tmp604;_tmp605;});}case 16U: _LL19: _tmp63E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5CC)->f1;_tmp63D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp5CC)->f2;_LL1A: {
# 3457
void*_tmp606=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63E);
void*_tmp607=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63D);
return _tmp606 == _tmp63E  && _tmp607 == _tmp63D?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->tag=16U,_tmp608->f1=_tmp606,_tmp608->f2=_tmp607;_tmp608;});}case 19U: _LL1B: _tmp63F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5CC)->f1;_LL1C: {
# 3461
void*_tmp609=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp63F);
return _tmp609 == _tmp63F?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->tag=19U,_tmp60A->f1=_tmp609;_tmp60A;});}case 18U: _LL1D: _tmp640=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5CC)->f1;_LL1E: {
# 3464
struct Cyc_Absyn_Exp*_tmp60B=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp640);
return _tmp60B == _tmp640?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=18U,_tmp60C->f1=_tmp60B;_tmp60C;});}case 27U: _LL1F: _tmp641=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5CC)->f1;_LL20: {
# 3467
struct Cyc_Absyn_Exp*_tmp60D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp641);
return _tmp60D == _tmp641?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->tag=27U,_tmp60E->f1=_tmp60D;_tmp60E;});}case 13U: _LL21: _LL22:
 goto _LL24;case 14U: _LL23: _LL24:
 goto _LL26;case 0U: _LL25: _LL26:
 goto _LL28;case 6U: _LL27: _LL28:
 goto _LL2A;case 7U: _LL29: _LL2A:
 goto _LL2C;case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 28U: _LL2F: _LL30:
 goto _LL32;case 20U: _LL31: _LL32:
 return t;case 25U: _LL33: _tmp642=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp5CC)->f1;_LL34: {
# 3479
void*_tmp60F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp642);
return _tmp60F == _tmp642?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=25U,_tmp610->f1=_tmp60F;_tmp610;});}case 23U: _LL35: _tmp643=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5CC)->f1;_LL36: {
# 3482
void*_tmp611=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp643);
return _tmp611 == _tmp643?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->tag=23U,_tmp612->f1=_tmp611;_tmp612;});}case 24U: _LL37: _tmp644=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5CC)->f1;_LL38: {
# 3485
struct Cyc_List_List*_tmp613=Cyc_Tcutil_substs(rgn,inst,_tmp644);
return _tmp613 == _tmp644?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->tag=24U,_tmp614->f1=_tmp613;_tmp614;});}default: _LL39: _LL3A:
({void*_tmp615=0U;({struct _dyneither_ptr _tmpCA7=({const char*_tmp616="found typedecltype in rsubs";_tag_dyneither(_tmp616,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCA7,_tag_dyneither(_tmp615,sizeof(void*),0U));});});}_LL0:;}
# 3491
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3494
if(ts == 0)
return 0;{
void*_tmp645=(void*)ts->hd;
struct Cyc_List_List*_tmp646=ts->tl;
void*_tmp647=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp645);
struct Cyc_List_List*_tmp648=Cyc_Tcutil_substs(rgn,inst,_tmp646);
if(_tmp645 == _tmp647  && _tmp646 == _tmp648)
return ts;
return({struct Cyc_List_List*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->hd=_tmp647,_tmp649->tl=_tmp648;_tmp649;});};}
# 3505
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3512
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp64A=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->f1=tv,({void*_tmpCA9=({struct Cyc_Core_Opt*_tmpCA8=_tmp64A;Cyc_Absyn_new_evar(_tmpCA8,({struct Cyc_Core_Opt*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->v=s;_tmp64B;}));});_tmp64C->f2=_tmpCA9;});_tmp64C;});}
# 3517
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3519
struct _tuple17*_tmp64D=env;struct Cyc_List_List*_tmp652;struct _RegionHandle*_tmp651;_LL1: _tmp652=_tmp64D->f1;_tmp651=_tmp64D->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp64E=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp650=_region_malloc(_tmp651,sizeof(*_tmp650));_tmp650->f1=tv,({void*_tmpCAB=({struct Cyc_Core_Opt*_tmpCAA=_tmp64E;Cyc_Absyn_new_evar(_tmpCAA,({struct Cyc_Core_Opt*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->v=_tmp652;_tmp64F;}));});_tmp650->f2=_tmpCAB;});_tmp650;});};}
# 3529
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3533
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp655=({struct Cyc_String_pa_PrintArg_struct _tmpA65;_tmpA65.tag=0U,_tmpA65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmpA65;});struct Cyc_String_pa_PrintArg_struct _tmp656=({struct Cyc_String_pa_PrintArg_struct _tmpA64;_tmpA64.tag=0U,({
struct _dyneither_ptr _tmpCAC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmpA64.f1=_tmpCAC;});_tmpA64;});struct Cyc_String_pa_PrintArg_struct _tmp657=({struct Cyc_String_pa_PrintArg_struct _tmpA63;_tmpA63.tag=0U,({struct _dyneither_ptr _tmpCAD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmpA63.f1=_tmpCAD;});_tmpA63;});void*_tmp653[3U];_tmp653[0]=& _tmp655,_tmp653[1]=& _tmp656,_tmp653[2]=& _tmp657;({unsigned int _tmpCAF=loc;struct _dyneither_ptr _tmpCAE=({const char*_tmp654="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp654,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpCAF,_tmpCAE,_tag_dyneither(_tmp653,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp658=0U;({struct _dyneither_ptr _tmpCB0=({const char*_tmp659="same type variable has different identity!";_tag_dyneither(_tmp659,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB0,_tag_dyneither(_tmp658,sizeof(void*),0U));});});}
return tvs;}}}
# 3547
({int _tmpCB1=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpCB1;});
return({struct Cyc_List_List*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->hd=tv,_tmp65A->tl=tvs;_tmp65A;});}
# 3553
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3555
if(tv->identity == - 1)
({void*_tmp65B=0U;({struct _dyneither_ptr _tmpCB2=({const char*_tmp65C="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp65C,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB2,_tag_dyneither(_tmp65B,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3559
struct Cyc_Absyn_Tvar*_tmp65D=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp65D->identity == - 1)
({void*_tmp65E=0U;({struct _dyneither_ptr _tmpCB3=({const char*_tmp65F="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp65F,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB3,_tag_dyneither(_tmp65E,sizeof(void*),0U));});});
if(_tmp65D->identity == tv->identity)
return tvs;}}
# 3566
return({struct Cyc_List_List*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->hd=tv,_tmp660->tl=tvs;_tmp660;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3572
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3577
if(tv->identity == - 1)
({void*_tmp661=0U;({struct _dyneither_ptr _tmpCB4=({const char*_tmp662="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp662,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB4,_tag_dyneither(_tmp661,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3581
struct _tuple28*_tmp663=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp664=_tmp663;struct Cyc_Absyn_Tvar*_tmp668;int*_tmp667;_LL1: _tmp668=_tmp664->f1;_tmp667=(int*)& _tmp664->f2;_LL2:;
if(_tmp668->identity == - 1)
({void*_tmp665=0U;({struct _dyneither_ptr _tmpCB5=({const char*_tmp666="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp666,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB5,_tag_dyneither(_tmp665,sizeof(void*),0U));});});
if(_tmp668->identity == tv->identity){
*_tmp667=*_tmp667  || b;
return tvs;}}}
# 3589
return({struct Cyc_List_List*_tmp66A=_region_malloc(r,sizeof(*_tmp66A));({struct _tuple28*_tmpCB6=({struct _tuple28*_tmp669=_region_malloc(r,sizeof(*_tmp669));_tmp669->f1=tv,_tmp669->f2=b;_tmp669;});_tmp66A->hd=_tmpCB6;}),_tmp66A->tl=tvs;_tmp66A;});}
# 3593
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3595
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp66D=({struct Cyc_String_pa_PrintArg_struct _tmpA66;_tmpA66.tag=0U,({struct _dyneither_ptr _tmpCB7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmpA66.f1=_tmpCB7;});_tmpA66;});void*_tmp66B[1U];_tmp66B[0]=& _tmp66D;({struct _dyneither_ptr _tmpCB8=({const char*_tmp66C="bound tvar id for %s is NULL";_tag_dyneither(_tmp66C,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB8,_tag_dyneither(_tmp66B,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->hd=tv,_tmp66E->tl=tvs;_tmp66E;});}struct _tuple29{void*f1;int f2;};
# 3604
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3607
void*_tmp66F=Cyc_Tcutil_compress(e);void*_tmp670=_tmp66F;int _tmp67A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp670)->tag == 1U){_LL1: _tmp67A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp670)->f3;_LL2:
# 3609
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp671=(struct _tuple29*)es2->hd;struct _tuple29*_tmp672=_tmp671;void*_tmp677;int*_tmp676;_LL6: _tmp677=_tmp672->f1;_tmp676=(int*)& _tmp672->f2;_LL7:;{
void*_tmp673=Cyc_Tcutil_compress(_tmp677);void*_tmp674=_tmp673;int _tmp675;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp674)->tag == 1U){_LL9: _tmp675=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp674)->f3;_LLA:
# 3613
 if(_tmp67A == _tmp675){
if(b != *_tmp676)*_tmp676=1;
return es;}
# 3617
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3621
return({struct Cyc_List_List*_tmp679=_region_malloc(r,sizeof(*_tmp679));({struct _tuple29*_tmpCB9=({struct _tuple29*_tmp678=_region_malloc(r,sizeof(*_tmp678));_tmp678->f1=e,_tmp678->f2=b;_tmp678;});_tmp679->hd=_tmpCB9;}),_tmp679->tl=es;_tmp679;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3626
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3629
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3638
if(!present)({struct Cyc_List_List*_tmpCBA=({struct Cyc_List_List*_tmp67B=_region_malloc(rgn,sizeof(*_tmp67B));_tmp67B->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp67B->tl=r;_tmp67B;});r=_tmpCBA;});}
# 3640
({struct Cyc_List_List*_tmpCBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpCBB;});
return r;}
# 3645
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3649
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp67C=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp67D=_tmp67C;struct Cyc_Absyn_Tvar*_tmp680;int _tmp67F;_LL1: _tmp680=_tmp67D.f1;_tmp67F=_tmp67D.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp680->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3659
if(!present)({struct Cyc_List_List*_tmpCBC=({struct Cyc_List_List*_tmp67E=_region_malloc(r,sizeof(*_tmp67E));_tmp67E->hd=(struct _tuple28*)tvs->hd,_tmp67E->tl=res;_tmp67E;});res=_tmpCBC;});};}
# 3661
({struct Cyc_List_List*_tmpCBD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpCBD;});
return res;}
# 3665
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3667
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp683=({struct Cyc_String_pa_PrintArg_struct _tmpA67;_tmpA67.tag=0U,_tmpA67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA67;});void*_tmp681[1U];_tmp681[0]=& _tmp683;({unsigned int _tmpCBF=loc;struct _dyneither_ptr _tmpCBE=({const char*_tmp682="bitfield %s does not have constant width";_tag_dyneither(_tmp682,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpCBF,_tmpCBE,_tag_dyneither(_tmp681,sizeof(void*),1U));});});else{
# 3672
struct _tuple14 _tmp684=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp685=_tmp684;unsigned int _tmp689;int _tmp688;_LL1: _tmp689=_tmp685.f1;_tmp688=_tmp685.f2;_LL2:;
if(!_tmp688)
({void*_tmp686=0U;({unsigned int _tmpCC1=loc;struct _dyneither_ptr _tmpCC0=({const char*_tmp687="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp687,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpCC1,_tmpCC0,_tag_dyneither(_tmp686,sizeof(void*),0U));});});
w=_tmp689;}{
# 3677
void*_tmp68A=Cyc_Tcutil_compress(field_typ);void*_tmp68B=_tmp68A;enum Cyc_Absyn_Size_of _tmp699;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68B)->tag == 6U){_LL4: _tmp699=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp68B)->f2;_LL5:
# 3680
{enum Cyc_Absyn_Size_of _tmp68C=_tmp699;switch(_tmp68C){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > 8)({void*_tmp68D=0U;({unsigned int _tmpCC3=loc;struct _dyneither_ptr _tmpCC2=({const char*_tmp68E="bitfield larger than type";_tag_dyneither(_tmp68E,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCC3,_tmpCC2,_tag_dyneither(_tmp68D,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > 16)({void*_tmp68F=0U;({unsigned int _tmpCC5=loc;struct _dyneither_ptr _tmpCC4=({const char*_tmp690="bitfield larger than type";_tag_dyneither(_tmp690,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCC5,_tmpCC4,_tag_dyneither(_tmp68F,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3685
 if(w > 32)({void*_tmp691=0U;({unsigned int _tmpCC7=loc;struct _dyneither_ptr _tmpCC6=({const char*_tmp692="bitfield larger than type";_tag_dyneither(_tmp692,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCC7,_tmpCC6,_tag_dyneither(_tmp691,sizeof(void*),0U));});});goto _LL8;default: _LL11: _LL12:
# 3687
 if(w > 64)({void*_tmp693=0U;({unsigned int _tmpCC9=loc;struct _dyneither_ptr _tmpCC8=({const char*_tmp694="bitfield larger than type";_tag_dyneither(_tmp694,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpCC9,_tmpCC8,_tag_dyneither(_tmp693,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3689
goto _LL3;}else{_LL6: _LL7:
# 3691
({struct Cyc_String_pa_PrintArg_struct _tmp697=({struct Cyc_String_pa_PrintArg_struct _tmpA69;_tmpA69.tag=0U,_tmpA69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA69;});struct Cyc_String_pa_PrintArg_struct _tmp698=({struct Cyc_String_pa_PrintArg_struct _tmpA68;_tmpA68.tag=0U,({
struct _dyneither_ptr _tmpCCA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ));_tmpA68.f1=_tmpCCA;});_tmpA68;});void*_tmp695[2U];_tmp695[0]=& _tmp697,_tmp695[1]=& _tmp698;({unsigned int _tmpCCC=loc;struct _dyneither_ptr _tmpCCB=({const char*_tmp696="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp696,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpCCC,_tmpCCB,_tag_dyneither(_tmp695,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3698
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp69A=(void*)atts->hd;void*_tmp69B=_tmp69A;switch(*((int*)_tmp69B)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp69E=({struct Cyc_String_pa_PrintArg_struct _tmpA6B;_tmpA6B.tag=0U,({
struct _dyneither_ptr _tmpCCD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmpA6B.f1=_tmpCCD;});_tmpA6B;});struct Cyc_String_pa_PrintArg_struct _tmp69F=({struct Cyc_String_pa_PrintArg_struct _tmpA6A;_tmpA6A.tag=0U,_tmpA6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmpA6A;});void*_tmp69C[2U];_tmp69C[0]=& _tmp69E,_tmp69C[1]=& _tmp69F;({unsigned int _tmpCCF=loc;struct _dyneither_ptr _tmpCCE=({const char*_tmp69D="bad attribute %s on member %s";_tag_dyneither(_tmp69D,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpCCF,_tmpCCE,_tag_dyneither(_tmp69C,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3726
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp6A0=t;struct Cyc_Absyn_Typedefdecl*_tmp6A4;void*_tmp6A3;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6A0)->tag == 17U){_LL1: _tmp6A4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6A0)->f3;_tmp6A3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6A0)->f4;_LL2:
# 3729
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp6A4))->tq).real_const  || (_tmp6A4->tq).print_const){
if(declared_const)({void*_tmp6A1=0U;({unsigned int _tmpCD1=loc;struct _dyneither_ptr _tmpCD0=({const char*_tmp6A2="extra const";_tag_dyneither(_tmp6A2,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpCD1,_tmpCD0,_tag_dyneither(_tmp6A1,sizeof(void*),0U));});});
return 1;}
# 3734
if((unsigned int)_tmp6A3)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp6A3);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3741
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp6A5=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp6A6=_tmp6A5;void*_tmp6AA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A6)->tag == 5U){_LL1: _tmp6AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A6)->f1).ptr_atts).rgn;_LL2:
# 3746
{void*_tmp6A7=Cyc_Tcutil_compress(_tmp6AA);void*_tmp6A8=_tmp6A7;struct Cyc_Absyn_Tvar*_tmp6A9;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A8)->tag == 2U){_LL6: _tmp6A9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A8)->f1;_LL7:
# 3748
 return Cyc_Absyn_tvar_cmp(tvar,_tmp6A9)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3751
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3757
return 1;}
return 0;}
# 3761
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3764
void*_tmp6AB=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp6AC=_tmp6AB;switch(*((int*)_tmp6AC)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6AC)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6AC)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp6AC)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp6AC)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3773
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3776
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3782
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3788
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3792
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3795
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp6AF=({struct Cyc_String_pa_PrintArg_struct _tmpA6D;_tmpA6D.tag=0U,_tmpA6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmpA6D;});struct Cyc_String_pa_PrintArg_struct _tmp6B0=({struct Cyc_String_pa_PrintArg_struct _tmpA6C;_tmpA6C.tag=0U,({
struct _dyneither_ptr _tmpCD2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmpA6C.f1=_tmpCD2;});_tmpA6C;});void*_tmp6AD[2U];_tmp6AD[0]=& _tmp6AF,_tmp6AD[1]=& _tmp6B0;({unsigned int _tmpCD4=loc;struct _dyneither_ptr _tmpCD3=({const char*_tmp6AE="%s clause has type %s instead of integral type";_tag_dyneither(_tmp6AE,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCD4,_tmpCD3,_tag_dyneither(_tmp6AD,sizeof(void*),2U));});});
({struct Cyc_Tcutil_CVTEnv _tmpCD5=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpCD5;});
({struct Cyc_List_List*_tmpCD6=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpCD6;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp6B3=({struct Cyc_String_pa_PrintArg_struct _tmpA6F;_tmpA6F.tag=0U,_tmpA6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmpA6F;});struct Cyc_String_pa_PrintArg_struct _tmp6B4=({struct Cyc_String_pa_PrintArg_struct _tmpA6E;_tmpA6E.tag=0U,({
struct _dyneither_ptr _tmpCD7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmpA6E.f1=_tmpCD7;});_tmpA6E;});void*_tmp6B1[2U];_tmp6B1[0]=& _tmp6B3,_tmp6B1[1]=& _tmp6B4;({unsigned int _tmpCD9=clause->loc;struct _dyneither_ptr _tmpCD8=({const char*_tmp6B2="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp6B2,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpCD9,_tmpCD8,_tag_dyneither(_tmp6B1,sizeof(void*),2U));});});}
# 3807
return({struct _tuple30 _tmpA70;_tmpA70.f1=cvtenv,_tmpA70.f2=relns;_tmpA70;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3838 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3846
{void*_tmp6B5=Cyc_Tcutil_compress(t);void*_tmp6B6=_tmp6B5;struct Cyc_List_List*_tmp843;void*_tmp842;void*_tmp841;void*_tmp840;void*_tmp83F;void*_tmp83E;struct _tuple2*_tmp83D;struct Cyc_List_List**_tmp83C;struct Cyc_Absyn_Typedefdecl**_tmp83B;void**_tmp83A;union Cyc_Absyn_AggrInfoU*_tmp839;struct Cyc_List_List**_tmp838;enum Cyc_Absyn_AggrKind _tmp837;struct Cyc_List_List*_tmp836;struct Cyc_List_List*_tmp835;struct Cyc_List_List**_tmp834;void**_tmp833;struct Cyc_Absyn_Tqual*_tmp832;void*_tmp831;struct Cyc_List_List*_tmp830;int _tmp82F;struct Cyc_Absyn_VarargInfo*_tmp82E;struct Cyc_List_List*_tmp82D;struct Cyc_List_List*_tmp82C;struct Cyc_Absyn_Exp*_tmp82B;struct Cyc_List_List**_tmp82A;struct Cyc_Absyn_Exp*_tmp829;struct Cyc_List_List**_tmp828;void*_tmp827;struct Cyc_Absyn_Tqual*_tmp826;struct Cyc_Absyn_Exp**_tmp825;union Cyc_Absyn_Constraint*_tmp824;unsigned int _tmp823;struct Cyc_Absyn_Exp*_tmp822;struct Cyc_Absyn_Exp*_tmp821;void*_tmp820;void*_tmp81F;struct Cyc_Absyn_Tqual*_tmp81E;void*_tmp81D;union Cyc_Absyn_Constraint*_tmp81C;union Cyc_Absyn_Constraint*_tmp81B;union Cyc_Absyn_Constraint*_tmp81A;union Cyc_Absyn_DatatypeFieldInfoU*_tmp819;struct Cyc_List_List*_tmp818;union Cyc_Absyn_DatatypeInfoU*_tmp817;struct Cyc_List_List**_tmp816;struct _tuple2*_tmp815;struct Cyc_Absyn_Enumdecl**_tmp814;struct Cyc_List_List*_tmp813;void*_tmp812;void***_tmp811;struct Cyc_Absyn_Tvar*_tmp810;struct Cyc_Core_Opt**_tmp80F;void**_tmp80E;switch(*((int*)_tmp6B6)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp80F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6B6)->f1;_tmp80E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6B6)->f2;_LL4:
# 3850
 if(*_tmp80F == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp80F))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp80F))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpCDA=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp80F=_tmpCDA;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3855
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp80E=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3858
*_tmp80E=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp6B7=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tag=2U,_tmp6B9->f1=0,_tmp6B9->f2=expected_kind;_tmp6B9;}));
({void*_tmpCDB=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8->tag=2U,_tmp6B8->f1=_tmp6B7;_tmp6B8;});*_tmp80E=_tmpCDB;});
_tmp810=_tmp6B7;goto _LL6;}else{
# 3864
({struct Cyc_List_List*_tmpCDC=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpCDC;});}}
# 3866
goto _LL0;case 2U: _LL5: _tmp810=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6B6)->f1;_LL6:
# 3868
{void*_tmp6BA=Cyc_Absyn_compress_kb(_tmp810->kind);void*_tmp6BB=_tmp6BA;struct Cyc_Core_Opt**_tmp6BE;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6BB)->tag == 1U){_LL3C: _tmp6BE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6BB)->f1;_LL3D:
# 3870
({struct Cyc_Core_Opt*_tmpCDE=({struct Cyc_Core_Opt*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));({void*_tmpCDD=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->tag=2U,_tmp6BC->f1=0,_tmp6BC->f2=expected_kind;_tmp6BC;});_tmp6BD->v=_tmpCDD;});_tmp6BD;});*_tmp6BE=_tmpCDE;});goto _LL3B;}else{_LL3E: _LL3F:
 goto _LL3B;}_LL3B:;}
# 3875
({struct Cyc_List_List*_tmpCDF=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp810);cvtenv.kind_env=_tmpCDF;});
# 3878
({struct Cyc_List_List*_tmpCE0=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp810,put_in_effect);cvtenv.free_vars=_tmpCE0;});
# 3880
{void*_tmp6BF=Cyc_Absyn_compress_kb(_tmp810->kind);void*_tmp6C0=_tmp6BF;struct Cyc_Core_Opt**_tmp6C4;struct Cyc_Absyn_Kind*_tmp6C3;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6C0)->tag == 2U){_LL41: _tmp6C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6C0)->f1;_tmp6C3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6C0)->f2;_LL42:
# 3882
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6C3))
({struct Cyc_Core_Opt*_tmpCE2=({struct Cyc_Core_Opt*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));({void*_tmpCE1=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->tag=2U,_tmp6C1->f1=0,_tmp6C1->f2=expected_kind;_tmp6C1;});_tmp6C2->v=_tmpCE1;});_tmp6C2;});*_tmp6C4=_tmpCE2;});
goto _LL40;}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3887
goto _LL0;case 26U: _LL7: _tmp812=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6B6)->f1)->r;_tmp811=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6B6)->f2;_LL8: {
# 3893
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6C5=_tmp812;struct Cyc_Absyn_Datatypedecl*_tmp6C8;struct Cyc_Absyn_Enumdecl*_tmp6C7;struct Cyc_Absyn_Aggrdecl*_tmp6C6;switch(*((int*)_tmp6C5)){case 0U: _LL46: _tmp6C6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6C5)->f1;_LL47:
# 3896
 if(te->in_extern_c_include)
_tmp6C6->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6C6);goto _LL45;case 1U: _LL48: _tmp6C7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6C5)->f1;_LL49:
# 3900
 if(te->in_extern_c_include)
_tmp6C7->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6C7);goto _LL45;default: _LL4A: _tmp6C8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6C5)->f1;_LL4B:
# 3904
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6C8);goto _LL45;}_LL45:;}
# 3906
({void**_tmpCE3=({void**_tmp6C9=_cycalloc(sizeof(*_tmp6C9));*_tmp6C9=new_t;_tmp6C9;});*_tmp811=_tmpCE3;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14U: _LL9: _tmp813=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6B6)->f1;_LLA: {
# 3912
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp813 != 0;_tmp813=_tmp813->tl){
struct Cyc_Absyn_Enumfield*_tmp6CA=(struct Cyc_Absyn_Enumfield*)_tmp813->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6CA->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6CD=({struct Cyc_String_pa_PrintArg_struct _tmpA71;_tmpA71.tag=0U,_tmpA71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6CA->name).f2);_tmpA71;});void*_tmp6CB[1U];_tmp6CB[0]=& _tmp6CD;({unsigned int _tmpCE5=_tmp6CA->loc;struct _dyneither_ptr _tmpCE4=({const char*_tmp6CC="duplicate enum field name %s";_tag_dyneither(_tmp6CC,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpCE5,_tmpCE4,_tag_dyneither(_tmp6CB,sizeof(void*),1U));});});else{
# 3919
({struct Cyc_List_List*_tmpCE6=({struct Cyc_List_List*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=(*_tmp6CA->name).f2,_tmp6CE->tl=prev_fields;_tmp6CE;});prev_fields=_tmpCE6;});}
# 3921
if(_tmp6CA->tag == 0)
({struct Cyc_Absyn_Exp*_tmpCE7=Cyc_Absyn_uint_exp(tag_count,_tmp6CA->loc);_tmp6CA->tag=_tmpCE7;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6CA->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6D1=({struct Cyc_String_pa_PrintArg_struct _tmpA72;_tmpA72.tag=0U,_tmpA72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6CA->name).f2);_tmpA72;});void*_tmp6CF[1U];_tmp6CF[0]=& _tmp6D1;({unsigned int _tmpCE9=loc;struct _dyneither_ptr _tmpCE8=({const char*_tmp6D0="enum field %s: expression is not constant";_tag_dyneither(_tmp6D0,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpCE9,_tmpCE8,_tag_dyneither(_tmp6CF,sizeof(void*),1U));});});}{
# 3926
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6CA->tag))).f1;
tag_count=t1 + 1;};}
# 3929
goto _LL0;}case 13U: _LLB: _tmp815=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6B6)->f1;_tmp814=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6B6)->f2;_LLC:
# 3931
 if(*_tmp814 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp814))->fields == 0){
struct _handler_cons _tmp6D2;_push_handler(& _tmp6D2);{int _tmp6D4=0;if(setjmp(_tmp6D2.handler))_tmp6D4=1;if(!_tmp6D4){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp815);
*_tmp814=*ed;}
# 3933
;_pop_handler();}else{void*_tmp6D3=(void*)_exn_thrown;void*_tmp6D5=_tmp6D3;void*_tmp6D8;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6D5)->tag == Cyc_Dict_Absent){_LL4D: _LL4E: {
# 3937
struct Cyc_Absyn_Enumdecl*_tmp6D6=({struct Cyc_Absyn_Enumdecl*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->sc=Cyc_Absyn_Extern,_tmp6D7->name=_tmp815,_tmp6D7->fields=0;_tmp6D7;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6D6);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp815);
*_tmp814=*ed;
goto _LL4C;};}}else{_LL4F: _tmp6D8=_tmp6D5;_LL50:(int)_rethrow(_tmp6D8);}_LL4C:;}};}{
# 3943
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp814);
goto _LL0;};case 3U: _LLD: _tmp817=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6B6)->f1).datatype_info;_tmp816=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6B6)->f1).targs;_LLE: {
# 3946
struct Cyc_List_List*_tmp6D9=*_tmp816;
{union Cyc_Absyn_DatatypeInfoU _tmp6DA=*_tmp817;union Cyc_Absyn_DatatypeInfoU _tmp6DB=_tmp6DA;struct Cyc_Absyn_Datatypedecl*_tmp6F2;struct _tuple2*_tmp6F1;int _tmp6F0;if((_tmp6DB.UnknownDatatype).tag == 1){_LL52: _tmp6F1=((_tmp6DB.UnknownDatatype).val).name;_tmp6F0=((_tmp6DB.UnknownDatatype).val).is_extensible;_LL53: {
# 3949
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp6DC;_push_handler(& _tmp6DC);{int _tmp6DE=0;if(setjmp(_tmp6DC.handler))_tmp6DE=1;if(!_tmp6DE){({struct Cyc_Absyn_Datatypedecl**_tmpCEA=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp6F1);tudp=_tmpCEA;});;_pop_handler();}else{void*_tmp6DD=(void*)_exn_thrown;void*_tmp6DF=_tmp6DD;void*_tmp6E5;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6DF)->tag == Cyc_Dict_Absent){_LL57: _LL58: {
# 3953
struct Cyc_Absyn_Datatypedecl*_tmp6E0=({struct Cyc_Absyn_Datatypedecl*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->sc=Cyc_Absyn_Extern,_tmp6E4->name=_tmp6F1,_tmp6E4->tvs=0,_tmp6E4->fields=0,_tmp6E4->is_extensible=_tmp6F0;_tmp6E4;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp6E0);
({struct Cyc_Absyn_Datatypedecl**_tmpCEB=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp6F1);tudp=_tmpCEB;});
# 3957
if(_tmp6D9 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp6E3=({struct Cyc_String_pa_PrintArg_struct _tmpA73;_tmpA73.tag=0U,({
struct _dyneither_ptr _tmpCEC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6F1));_tmpA73.f1=_tmpCEC;});_tmpA73;});void*_tmp6E1[1U];_tmp6E1[0]=& _tmp6E3;({unsigned int _tmpCEE=loc;struct _dyneither_ptr _tmpCED=({const char*_tmp6E2="declare parameterized datatype %s before using";_tag_dyneither(_tmp6E2,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCEE,_tmpCED,_tag_dyneither(_tmp6E1,sizeof(void*),1U));});});
return cvtenv;}
# 3962
goto _LL56;}}else{_LL59: _tmp6E5=_tmp6DF;_LL5A:(int)_rethrow(_tmp6E5);}_LL56:;}};}
# 3966
if(_tmp6F0  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp6E8=({struct Cyc_String_pa_PrintArg_struct _tmpA74;_tmpA74.tag=0U,({
struct _dyneither_ptr _tmpCEF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6F1));_tmpA74.f1=_tmpCEF;});_tmpA74;});void*_tmp6E6[1U];_tmp6E6[0]=& _tmp6E8;({unsigned int _tmpCF1=loc;struct _dyneither_ptr _tmpCF0=({const char*_tmp6E7="datatype %s was not declared @extensible";_tag_dyneither(_tmp6E7,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpCF1,_tmpCF0,_tag_dyneither(_tmp6E6,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfoU _tmpCF2=Cyc_Absyn_KnownDatatype(tudp);*_tmp817=_tmpCF2;});
_tmp6F2=*tudp;goto _LL55;}}else{_LL54: _tmp6F2=*(_tmp6DB.KnownDatatype).val;_LL55: {
# 3973
struct Cyc_List_List*tvs=_tmp6F2->tvs;
for(0;_tmp6D9 != 0  && tvs != 0;(_tmp6D9=_tmp6D9->tl,tvs=tvs->tl)){
void*t=(void*)_tmp6D9->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3979
{struct _tuple0 _tmp6E9=({struct _tuple0 _tmpA75;({void*_tmpCF3=Cyc_Absyn_compress_kb(tv->kind);_tmpA75.f1=_tmpCF3;}),_tmpA75.f2=t;_tmpA75;});struct _tuple0 _tmp6EA=_tmp6E9;struct Cyc_Absyn_Tvar*_tmp6EB;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6EA.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6EA.f2)->tag == 2U){_LL5C: _tmp6EB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6EA.f2)->f1;_LL5D:
# 3981
({struct Cyc_List_List*_tmpCF4=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6EB);cvtenv.kind_env=_tmpCF4;});
({struct Cyc_List_List*_tmpCF5=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp6EB,1);cvtenv.free_vars=_tmpCF5;});
continue;}else{goto _LL5E;}}else{_LL5E: _LL5F:
 goto _LL5B;}_LL5B:;}{
# 3986
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpCF6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpCF6;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3990
if(_tmp6D9 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6EE=({struct Cyc_String_pa_PrintArg_struct _tmpA76;_tmpA76.tag=0U,({
struct _dyneither_ptr _tmpCF7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6F2->name));_tmpA76.f1=_tmpCF7;});_tmpA76;});void*_tmp6EC[1U];_tmp6EC[0]=& _tmp6EE;({unsigned int _tmpCF9=loc;struct _dyneither_ptr _tmpCF8=({const char*_tmp6ED="too many type arguments for datatype %s";_tag_dyneither(_tmp6ED,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpCF9,_tmpCF8,_tag_dyneither(_tmp6EC,sizeof(void*),1U));});});
if(tvs != 0){
# 3995
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpCFA=({struct Cyc_List_List*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->hd=e,_tmp6EF->tl=hidden_ts;_tmp6EF;});hidden_ts=_tmpCFA;});
({struct Cyc_Tcutil_CVTEnv _tmpCFB=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpCFB;});}
# 4002
({struct Cyc_List_List*_tmpCFD=({struct Cyc_List_List*_tmpCFC=*_tmp816;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCFC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp816=_tmpCFD;});}
# 4004
goto _LL51;}}_LL51:;}
# 4006
goto _LL0;}case 4U: _LLF: _tmp819=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6B6)->f1).field_info;_tmp818=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6B6)->f1).targs;_LL10:
# 4009
{union Cyc_Absyn_DatatypeFieldInfoU _tmp6F3=*_tmp819;union Cyc_Absyn_DatatypeFieldInfoU _tmp6F4=_tmp6F3;struct Cyc_Absyn_Datatypedecl*_tmp707;struct Cyc_Absyn_Datatypefield*_tmp706;struct _tuple2*_tmp705;struct _tuple2*_tmp704;int _tmp703;if((_tmp6F4.UnknownDatatypefield).tag == 1){_LL61: _tmp705=((_tmp6F4.UnknownDatatypefield).val).datatype_name;_tmp704=((_tmp6F4.UnknownDatatypefield).val).field_name;_tmp703=((_tmp6F4.UnknownDatatypefield).val).is_extensible;_LL62: {
# 4012
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp705);
struct Cyc_Absyn_Datatypefield*tuf;
# 4017
{struct Cyc_List_List*_tmp6F5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp6F5=_tmp6F5->tl){
if(_tmp6F5 == 0)
({void*_tmp6F6=0U;({struct _dyneither_ptr _tmpCFE=({const char*_tmp6F7="Tcutil found a bad datatypefield";_tag_dyneither(_tmp6F7,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCFE,_tag_dyneither(_tmp6F6,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp6F5->hd)->name,_tmp704)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp6F5->hd;
break;}}}
# 4025
({union Cyc_Absyn_DatatypeFieldInfoU _tmpCFF=Cyc_Absyn_KnownDatatypefield(tud,tuf);*_tmp819=_tmpCFF;});
_tmp707=tud;_tmp706=tuf;goto _LL64;}}else{_LL63: _tmp707=((_tmp6F4.KnownDatatypefield).val).f1;_tmp706=((_tmp6F4.KnownDatatypefield).val).f2;_LL64: {
# 4029
struct Cyc_List_List*tvs=_tmp707->tvs;
for(0;_tmp818 != 0  && tvs != 0;(_tmp818=_tmp818->tl,tvs=tvs->tl)){
void*t=(void*)_tmp818->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4035
{struct _tuple0 _tmp6F8=({struct _tuple0 _tmpA77;({void*_tmpD00=Cyc_Absyn_compress_kb(tv->kind);_tmpA77.f1=_tmpD00;}),_tmpA77.f2=t;_tmpA77;});struct _tuple0 _tmp6F9=_tmp6F8;struct Cyc_Absyn_Tvar*_tmp6FA;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6F9.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6F9.f2)->tag == 2U){_LL66: _tmp6FA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6F9.f2)->f1;_LL67:
# 4037
({struct Cyc_List_List*_tmpD01=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6FA);cvtenv.kind_env=_tmpD01;});
({struct Cyc_List_List*_tmpD02=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp6FA,1);cvtenv.free_vars=_tmpD02;});
continue;}else{goto _LL68;}}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}{
# 4042
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpD03=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpD03;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4046
if(_tmp818 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6FD=({struct Cyc_String_pa_PrintArg_struct _tmpA79;_tmpA79.tag=0U,({
struct _dyneither_ptr _tmpD04=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp707->name));_tmpA79.f1=_tmpD04;});_tmpA79;});struct Cyc_String_pa_PrintArg_struct _tmp6FE=({struct Cyc_String_pa_PrintArg_struct _tmpA78;_tmpA78.tag=0U,({struct _dyneither_ptr _tmpD05=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp706->name));_tmpA78.f1=_tmpD05;});_tmpA78;});void*_tmp6FB[2U];_tmp6FB[0]=& _tmp6FD,_tmp6FB[1]=& _tmp6FE;({unsigned int _tmpD07=loc;struct _dyneither_ptr _tmpD06=({const char*_tmp6FC="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp6FC,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpD07,_tmpD06,_tag_dyneither(_tmp6FB,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp701=({struct Cyc_String_pa_PrintArg_struct _tmpA7B;_tmpA7B.tag=0U,({
struct _dyneither_ptr _tmpD08=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp707->name));_tmpA7B.f1=_tmpD08;});_tmpA7B;});struct Cyc_String_pa_PrintArg_struct _tmp702=({struct Cyc_String_pa_PrintArg_struct _tmpA7A;_tmpA7A.tag=0U,({struct _dyneither_ptr _tmpD09=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp706->name));_tmpA7A.f1=_tmpD09;});_tmpA7A;});void*_tmp6FF[2U];_tmp6FF[0]=& _tmp701,_tmp6FF[1]=& _tmp702;({unsigned int _tmpD0B=loc;struct _dyneither_ptr _tmpD0A=({const char*_tmp700="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp700,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpD0B,_tmpD0A,_tag_dyneither(_tmp6FF,sizeof(void*),2U));});});
goto _LL60;}}_LL60:;}
# 4054
goto _LL0;case 5U: _LL11: _tmp81F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B6)->f1).elt_typ;_tmp81E=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B6)->f1).elt_tq;_tmp81D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B6)->f1).ptr_atts).rgn;_tmp81C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B6)->f1).ptr_atts).nullable;_tmp81B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B6)->f1).ptr_atts).bounds;_tmp81A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B6)->f1).ptr_atts).zero_term;_LL12: {
# 4057
int is_zero_terminated;
# 4059
({struct Cyc_Tcutil_CVTEnv _tmpD0C=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp81F,1,1);cvtenv=_tmpD0C;});
({int _tmpD0D=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp81E->print_const,_tmp81F);_tmp81E->real_const=_tmpD0D;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp708=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp709=_tmp708;switch(_tmp709){case Cyc_Absyn_Aliasable: _LL6B: _LL6C:
 k=& Cyc_Tcutil_rk;goto _LL6A;case Cyc_Absyn_Unique: _LL6D: _LL6E:
 k=& Cyc_Tcutil_urk;goto _LL6A;default: _LL6F: _LL70:
 k=& Cyc_Tcutil_trk;goto _LL6A;}_LL6A:;}
# 4067
({struct Cyc_Tcutil_CVTEnv _tmpD0E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp81D,1,1);cvtenv=_tmpD0E;});
{union Cyc_Absyn_Constraint*_tmp70A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp81A);union Cyc_Absyn_Constraint*_tmp70B=_tmp70A;int _tmp711;switch((((union Cyc_Absyn_Constraint*)_tmp70B)->Eq_constr).tag){case 3U: _LL72: _LL73:
# 4072
{void*_tmp70C=Cyc_Tcutil_compress(_tmp81F);void*_tmp70D=_tmp70C;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70D)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp70D)->f2 == Cyc_Absyn_Char_sz){_LL79: _LL7A:
# 4074
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp81A,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL78;}else{goto _LL7B;}}else{_LL7B: _LL7C:
# 4078
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp81A,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL78;}_LL78:;}
# 4082
goto _LL71;case 1U: _LL74: _tmp711=(int)(_tmp70B->Eq_constr).val;if(_tmp711){_LL75:
# 4085
 if(!Cyc_Tcutil_admits_zero(_tmp81F))
({struct Cyc_String_pa_PrintArg_struct _tmp710=({struct Cyc_String_pa_PrintArg_struct _tmpA7C;_tmpA7C.tag=0U,({
struct _dyneither_ptr _tmpD0F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp81F));_tmpA7C.f1=_tmpD0F;});_tmpA7C;});void*_tmp70E[1U];_tmp70E[0]=& _tmp710;({unsigned int _tmpD11=loc;struct _dyneither_ptr _tmpD10=({const char*_tmp70F="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp70F,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpD11,_tmpD10,_tag_dyneither(_tmp70E,sizeof(void*),1U));});});
is_zero_terminated=1;
goto _LL71;}else{goto _LL76;}default: _LL76: _LL77:
# 4091
 is_zero_terminated=0;
goto _LL71;}_LL71:;}
# 4095
{void*_tmp712=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp81B);void*_tmp713=_tmp712;struct Cyc_Absyn_Exp*_tmp71C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp713)->tag == 0U){_LL7E: _LL7F:
 goto _LL7D;}else{_LL80: _tmp71C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp713)->f1;_LL81:
# 4098
({struct Cyc_Tcenv_Tenv*_tmpD12=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD12,0,_tmp71C);});
({struct Cyc_Tcutil_CVTEnv _tmpD13=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp71C,te,cvtenv);cvtenv=_tmpD13;});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp71C))
({void*_tmp714=0U;({unsigned int _tmpD15=loc;struct _dyneither_ptr _tmpD14=({const char*_tmp715="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp715,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpD15,_tmpD14,_tag_dyneither(_tmp714,sizeof(void*),0U));});});{
struct _tuple14 _tmp716=Cyc_Evexp_eval_const_uint_exp(_tmp71C);struct _tuple14 _tmp717=_tmp716;unsigned int _tmp71B;int _tmp71A;_LL83: _tmp71B=_tmp717.f1;_tmp71A=_tmp717.f2;_LL84:;
if(is_zero_terminated  && (!_tmp71A  || _tmp71B < 1))
({void*_tmp718=0U;({unsigned int _tmpD17=loc;struct _dyneither_ptr _tmpD16=({const char*_tmp719="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp719,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpD17,_tmpD16,_tag_dyneither(_tmp718,sizeof(void*),0U));});});
goto _LL7D;};}_LL7D:;}
# 4107
goto _LL0;};}case 19U: _LL13: _tmp820=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6B6)->f1;_LL14:
# 4109
({struct Cyc_Tcutil_CVTEnv _tmpD18=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp820,1,1);cvtenv=_tmpD18;});goto _LL0;case 28U: _LL15: _LL16:
 goto _LL0;case 18U: _LL17: _tmp821=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6B6)->f1;_LL18:
# 4115
({struct Cyc_Tcenv_Tenv*_tmpD19=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD19,0,_tmp821);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp821))
({void*_tmp71D=0U;({unsigned int _tmpD1B=loc;struct _dyneither_ptr _tmpD1A=({const char*_tmp71E="valueof_t requires an int expression";_tag_dyneither(_tmp71E,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD1B,_tmpD1A,_tag_dyneither(_tmp71D,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD1C=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp821,te,cvtenv);cvtenv=_tmpD1C;});
goto _LL0;case 27U: _LL19: _tmp822=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6B6)->f1;_LL1A:
# 4124
({struct Cyc_Tcenv_Tenv*_tmpD1D=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD1D,0,_tmp822);});
goto _LL0;case 6U: _LL1B: _LL1C:
 goto _LL1E;case 7U: _LL1D: _LL1E:
 goto _LL0;case 8U: _LL1F: _tmp827=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B6)->f1).elt_type;_tmp826=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B6)->f1).tq;_tmp825=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B6)->f1).num_elts;_tmp824=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B6)->f1).zero_term;_tmp823=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B6)->f1).zt_loc;_LL20: {
# 4131
struct Cyc_Absyn_Exp*_tmp71F=*_tmp825;
({struct Cyc_Tcutil_CVTEnv _tmpD1E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp827,1,allow_abs_aggr);cvtenv=_tmpD1E;});
({int _tmpD1F=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp826->print_const,_tmp827);_tmp826->real_const=_tmpD1F;});{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp720=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp824);union Cyc_Absyn_Constraint*_tmp721=_tmp720;int _tmp725;switch((((union Cyc_Absyn_Constraint*)_tmp721)->Eq_constr).tag){case 3U: _LL86: _LL87:
# 4138
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp824,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4153 "tcutil.cyc"
goto _LL85;case 1U: _LL88: _tmp725=(int)(_tmp721->Eq_constr).val;if(_tmp725){_LL89:
# 4156
 if(!Cyc_Tcutil_admits_zero(_tmp827))
({struct Cyc_String_pa_PrintArg_struct _tmp724=({struct Cyc_String_pa_PrintArg_struct _tmpA7D;_tmpA7D.tag=0U,({
struct _dyneither_ptr _tmpD20=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp827));_tmpA7D.f1=_tmpD20;});_tmpA7D;});void*_tmp722[1U];_tmp722[0]=& _tmp724;({unsigned int _tmpD22=loc;struct _dyneither_ptr _tmpD21=({const char*_tmp723="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp723,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD22,_tmpD21,_tag_dyneither(_tmp722,sizeof(void*),1U));});});
is_zero_terminated=1;
goto _LL85;}else{goto _LL8A;}default: _LL8A: _LL8B:
# 4162
 is_zero_terminated=0;
goto _LL85;}_LL85:;}
# 4167
if(_tmp71F == 0){
# 4169
if(is_zero_terminated)
# 4171
({struct Cyc_Absyn_Exp*_tmpD24=({struct Cyc_Absyn_Exp*_tmpD23=Cyc_Absyn_uint_exp(1U,0U);_tmp71F=_tmpD23;});*_tmp825=_tmpD24;});else{
# 4174
({void*_tmp726=0U;({unsigned int _tmpD26=loc;struct _dyneither_ptr _tmpD25=({const char*_tmp727="array bound defaults to 1 here";_tag_dyneither(_tmp727,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpD26,_tmpD25,_tag_dyneither(_tmp726,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpD28=({struct Cyc_Absyn_Exp*_tmpD27=Cyc_Absyn_uint_exp(1U,0U);_tmp71F=_tmpD27;});*_tmp825=_tmpD28;});}}
# 4178
({struct Cyc_Tcenv_Tenv*_tmpD29=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD29,0,_tmp71F);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp71F))
({void*_tmp728=0U;({unsigned int _tmpD2B=loc;struct _dyneither_ptr _tmpD2A=({const char*_tmp729="array bounds expression is not an unsigned int";_tag_dyneither(_tmp729,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD2B,_tmpD2A,_tag_dyneither(_tmp728,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD2C=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp71F,te,cvtenv);cvtenv=_tmpD2C;});{
# 4186
struct _tuple14 _tmp72A=Cyc_Evexp_eval_const_uint_exp(_tmp71F);struct _tuple14 _tmp72B=_tmp72A;unsigned int _tmp731;int _tmp730;_LL8D: _tmp731=_tmp72B.f1;_tmp730=_tmp72B.f2;_LL8E:;
# 4188
if((is_zero_terminated  && _tmp730) && _tmp731 < 1)
({void*_tmp72C=0U;({unsigned int _tmpD2E=loc;struct _dyneither_ptr _tmpD2D=({const char*_tmp72D="zero terminated array cannot have zero elements";_tag_dyneither(_tmp72D,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpD2E,_tmpD2D,_tag_dyneither(_tmp72C,sizeof(void*),0U));});});
# 4191
if((_tmp730  && _tmp731 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp72E=0U;({unsigned int _tmpD30=loc;struct _dyneither_ptr _tmpD2F=({const char*_tmp72F="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp72F,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpD30,_tmpD2F,_tag_dyneither(_tmp72E,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpD31=Cyc_Absyn_uint_exp(1U,0U);*_tmp825=_tmpD31;});}
# 4195
goto _LL0;};};}case 9U: _LL21: _tmp834=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).tvars;_tmp833=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).effect;_tmp832=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).ret_tqual;_tmp831=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).ret_typ;_tmp830=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).args;_tmp82F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).c_varargs;_tmp82E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).cyc_varargs;_tmp82D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).rgn_po;_tmp82C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).attributes;_tmp82B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).requires_clause;_tmp82A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).requires_relns;_tmp829=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).ensures_clause;_tmp828=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6B6)->f1).ensures_relns;_LL22: {
# 4202
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp82C != 0;_tmp82C=_tmp82C->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp82C->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp734=({struct Cyc_String_pa_PrintArg_struct _tmpA7E;_tmpA7E.tag=0U,({struct _dyneither_ptr _tmpD32=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp82C->hd));_tmpA7E.f1=_tmpD32;});_tmpA7E;});void*_tmp732[1U];_tmp732[0]=& _tmp734;({unsigned int _tmpD34=loc;struct _dyneither_ptr _tmpD33=({const char*_tmp733="bad function type attribute %s";_tag_dyneither(_tmp733,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpD34,_tmpD33,_tag_dyneither(_tmp732,sizeof(void*),1U));});});{
void*_tmp735=(void*)_tmp82C->hd;void*_tmp736=_tmp735;enum Cyc_Absyn_Format_Type _tmp73B;int _tmp73A;int _tmp739;switch(*((int*)_tmp736)){case 1U: _LL90: _LL91:
# 4215
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL8F;case 2U: _LL92: _LL93:
# 4217
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL8F;case 3U: _LL94: _LL95:
# 4219
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL8F;case 19U: _LL96: _tmp73B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp736)->f1;_tmp73A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp736)->f2;_tmp739=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp736)->f3;_LL97:
# 4221
 if(!seen_format){
seen_format=1;
ft=_tmp73B;
fmt_desc_arg=_tmp73A;
fmt_arg_start=_tmp739;}else{
# 4227
({void*_tmp737=0U;({unsigned int _tmpD36=loc;struct _dyneither_ptr _tmpD35=({const char*_tmp738="function can't have multiple format attributes";_tag_dyneither(_tmp738,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD36,_tmpD35,_tag_dyneither(_tmp737,sizeof(void*),0U));});});}
goto _LL8F;default: _LL98: _LL99:
 goto _LL8F;}_LL8F:;};}
# 4232
if(num_convs > 1)
({void*_tmp73C=0U;({unsigned int _tmpD38=loc;struct _dyneither_ptr _tmpD37=({const char*_tmp73D="function can't have multiple calling conventions";_tag_dyneither(_tmp73D,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpD38,_tmpD37,_tag_dyneither(_tmp73C,sizeof(void*),0U));});});
# 4237
Cyc_Tcutil_check_unique_tvars(loc,*_tmp834);
{struct Cyc_List_List*b=*_tmp834;for(0;b != 0;b=b->tl){
({int _tmpD39=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpD39;});
({struct Cyc_List_List*_tmpD3A=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpD3A;});{
void*_tmp73E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp73F=_tmp73E;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp73F)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp73F)->f1)->kind == Cyc_Absyn_MemKind){_LL9B: _LL9C:
# 4243
({struct Cyc_String_pa_PrintArg_struct _tmp742=({struct Cyc_String_pa_PrintArg_struct _tmpA7F;_tmpA7F.tag=0U,_tmpA7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmpA7F;});void*_tmp740[1U];_tmp740[0]=& _tmp742;({unsigned int _tmpD3C=loc;struct _dyneither_ptr _tmpD3B=({const char*_tmp741="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp741,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD3C,_tmpD3B,_tag_dyneither(_tmp740,sizeof(void*),1U));});});
goto _LL9A;}else{goto _LL9D;}}else{_LL9D: _LL9E:
 goto _LL9A;}_LL9A:;};}}{
# 4251
struct Cyc_Tcutil_CVTEnv _tmp743=({struct Cyc_Tcutil_CVTEnv _tmpA83;_tmpA83.r=Cyc_Core_heap_region,_tmpA83.kind_env=cvtenv.kind_env,_tmpA83.free_vars=0,_tmpA83.free_evars=0,_tmpA83.generalize_evars=cvtenv.generalize_evars,_tmpA83.fn_result=1;_tmpA83;});
# 4256
({struct Cyc_Tcutil_CVTEnv _tmpD3D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp743,& Cyc_Tcutil_tmk,_tmp831,1,1);_tmp743=_tmpD3D;});
({int _tmpD3E=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp832->print_const,_tmp831);_tmp832->real_const=_tmpD3E;});
_tmp743.fn_result=0;
{struct Cyc_List_List*a=_tmp830;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp744=(struct _tuple10*)a->hd;
void*_tmp745=(*_tmp744).f3;
({struct Cyc_Tcutil_CVTEnv _tmpD3F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp743,& Cyc_Tcutil_tmk,_tmp745,1,1);_tmp743=_tmpD3F;});{
int _tmp746=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp744).f2).print_const,_tmp745);
((*_tmp744).f2).real_const=_tmp746;
# 4267
if(Cyc_Tcutil_is_array(_tmp745)){
# 4269
void*_tmp747=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpD40=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp743,& Cyc_Tcutil_rk,_tmp747,1,1);_tmp743=_tmpD40;});
({void*_tmpD41=Cyc_Tcutil_promote_array(_tmp745,_tmp747,0);(*_tmp744).f3=_tmpD41;});}};}}
# 4276
if(_tmp82E != 0){
if(_tmp82F)({void*_tmp748=0U;({struct _dyneither_ptr _tmpD42=({const char*_tmp749="both c_vararg and cyc_vararg";_tag_dyneither(_tmp749,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD42,_tag_dyneither(_tmp748,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp74A=*_tmp82E;struct Cyc_Absyn_VarargInfo _tmp74B=_tmp74A;void*_tmp75E;int _tmp75D;_LLA0: _tmp75E=_tmp74B.type;_tmp75D=_tmp74B.inject;_LLA1:;
({struct Cyc_Tcutil_CVTEnv _tmpD43=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp743,& Cyc_Tcutil_tmk,_tmp75E,1,1);_tmp743=_tmpD43;});
({int _tmpD44=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp82E->tq).print_const,_tmp75E);(_tmp82E->tq).real_const=_tmpD44;});
# 4282
if(_tmp75D){
void*_tmp74C=Cyc_Tcutil_compress(_tmp75E);void*_tmp74D=_tmp74C;void*_tmp75C;union Cyc_Absyn_Constraint*_tmp75B;union Cyc_Absyn_Constraint*_tmp75A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74D)->tag == 5U){_LLA3: _tmp75C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74D)->f1).elt_typ;_tmp75B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74D)->f1).ptr_atts).bounds;_tmp75A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74D)->f1).ptr_atts).zero_term;_LLA4:
# 4285
{void*_tmp74E=Cyc_Tcutil_compress(_tmp75C);void*_tmp74F=_tmp74E;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp74F)->tag == 3U){_LLA8: _LLA9:
# 4287
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp75A))
({void*_tmp750=0U;({unsigned int _tmpD46=loc;struct _dyneither_ptr _tmpD45=({const char*_tmp751="can't inject into a zeroterm pointer";_tag_dyneither(_tmp751,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD46,_tmpD45,_tag_dyneither(_tmp750,sizeof(void*),0U));});});
{void*_tmp752=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp75B);void*_tmp753=_tmp752;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp753)->tag == 0U){_LLAD: _LLAE:
# 4291
({void*_tmp754=0U;({unsigned int _tmpD48=loc;struct _dyneither_ptr _tmpD47=({const char*_tmp755="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp755,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpD48,_tmpD47,_tag_dyneither(_tmp754,sizeof(void*),0U));});});
goto _LLAC;}else{_LLAF: _LLB0:
 goto _LLAC;}_LLAC:;}
# 4295
goto _LLA7;}else{_LLAA: _LLAB:
({void*_tmp756=0U;({unsigned int _tmpD4A=loc;struct _dyneither_ptr _tmpD49=({const char*_tmp757="can't inject a non-datatype type";_tag_dyneither(_tmp757,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpD4A,_tmpD49,_tag_dyneither(_tmp756,sizeof(void*),0U));});});goto _LLA7;}_LLA7:;}
# 4298
goto _LLA2;}else{_LLA5: _LLA6:
({void*_tmp758=0U;({unsigned int _tmpD4C=loc;struct _dyneither_ptr _tmpD4B=({const char*_tmp759="expecting a datatype pointer type";_tag_dyneither(_tmp759,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpD4C,_tmpD4B,_tag_dyneither(_tmp758,sizeof(void*),0U));});});goto _LLA2;}_LLA2:;}};}
# 4304
if(seen_format){
int _tmp75F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp830);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp75F) || fmt_arg_start < 0) || 
# 4308
(_tmp82E == 0  && !_tmp82F) && fmt_arg_start != 0) || 
(_tmp82E != 0  || _tmp82F) && fmt_arg_start != _tmp75F + 1)
# 4311
({void*_tmp760=0U;({unsigned int _tmpD4E=loc;struct _dyneither_ptr _tmpD4D=({const char*_tmp761="bad format descriptor";_tag_dyneither(_tmp761,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpD4E,_tmpD4D,_tag_dyneither(_tmp760,sizeof(void*),0U));});});else{
# 4314
struct _tuple10 _tmp762=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp830,fmt_desc_arg - 1);struct _tuple10 _tmp763=_tmp762;void*_tmp779;_LLB2: _tmp779=_tmp763.f3;_LLB3:;
# 4316
{void*_tmp764=Cyc_Tcutil_compress(_tmp779);void*_tmp765=_tmp764;void*_tmp772;union Cyc_Absyn_Constraint*_tmp771;union Cyc_Absyn_Constraint*_tmp770;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->tag == 5U){_LLB5: _tmp772=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->f1).elt_typ;_tmp771=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->f1).ptr_atts).bounds;_tmp770=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp765)->f1).ptr_atts).zero_term;_LLB6:
# 4319
{struct _tuple0 _tmp766=({struct _tuple0 _tmpA80;({void*_tmpD50=Cyc_Tcutil_compress(_tmp772);_tmpA80.f1=_tmpD50;}),({void*_tmpD4F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp771);_tmpA80.f2=_tmpD4F;});_tmpA80;});struct _tuple0 _tmp767=_tmp766;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp767.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp767.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp767.f2)->tag == 0U){_LLBA: _LLBB:
# 4321
 if(_tmp82F)
({void*_tmp768=0U;({unsigned int _tmpD52=loc;struct _dyneither_ptr _tmpD51=({const char*_tmp769="format descriptor is not a char * type";_tag_dyneither(_tmp769,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD52,_tmpD51,_tag_dyneither(_tmp768,sizeof(void*),0U));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 4325
 if(!_tmp82F)
({void*_tmp76A=0U;({unsigned int _tmpD54=loc;struct _dyneither_ptr _tmpD53=({const char*_tmp76B="format descriptor is not a char ? type";_tag_dyneither(_tmp76B,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD54,_tmpD53,_tag_dyneither(_tmp76A,sizeof(void*),0U));});});
goto _LLB9;}}else{goto _LLBE;}}else{_LLBE: _LLBF:
# 4329
({void*_tmp76C=0U;({unsigned int _tmpD56=loc;struct _dyneither_ptr _tmpD55=({const char*_tmp76D="format descriptor is not a string type";_tag_dyneither(_tmp76D,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD56,_tmpD55,_tag_dyneither(_tmp76C,sizeof(void*),0U));});});
goto _LLB9;}_LLB9:;}
# 4332
goto _LLB4;}else{_LLB7: _LLB8:
({void*_tmp76E=0U;({unsigned int _tmpD58=loc;struct _dyneither_ptr _tmpD57=({const char*_tmp76F="format descriptor is not a string type";_tag_dyneither(_tmp76F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpD58,_tmpD57,_tag_dyneither(_tmp76E,sizeof(void*),0U));});});goto _LLB4;}_LLB4:;}
# 4335
if(fmt_arg_start != 0  && !_tmp82F){
# 4339
int problem;
{struct _tuple31 _tmp773=({struct _tuple31 _tmpA81;_tmpA81.f1=ft,({void*_tmpD59=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp82E))->type));_tmpA81.f2=_tmpD59;});_tmpA81;});struct _tuple31 _tmp774=_tmp773;struct Cyc_Absyn_Datatypedecl*_tmp776;struct Cyc_Absyn_Datatypedecl*_tmp775;if(_tmp774.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp774.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp774.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC1: _tmp775=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp774.f2)->f1).datatype_info).KnownDatatype).val;_LLC2:
# 4342
({int _tmpD5A=Cyc_Absyn_qvar_cmp(_tmp775->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;problem=_tmpD5A;});goto _LLC0;}else{goto _LLC5;}}else{goto _LLC5;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp774.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp774.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC3: _tmp776=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp774.f2)->f1).datatype_info).KnownDatatype).val;_LLC4:
# 4344
({int _tmpD5B=Cyc_Absyn_qvar_cmp(_tmp776->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;problem=_tmpD5B;});goto _LLC0;}else{goto _LLC5;}}else{_LLC5: _LLC6:
# 4346
 problem=1;goto _LLC0;}}_LLC0:;}
# 4348
if(problem)
({void*_tmp777=0U;({unsigned int _tmpD5D=loc;struct _dyneither_ptr _tmpD5C=({const char*_tmp778="format attribute and vararg types don't match";_tag_dyneither(_tmp778,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpD5D,_tmpD5C,_tag_dyneither(_tmp777,sizeof(void*),0U));});});}}}
# 4356
{struct Cyc_List_List*rpo=_tmp82D;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp77A=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp77B=_tmp77A;void*_tmp77D;void*_tmp77C;_LLC8: _tmp77D=_tmp77B->f1;_tmp77C=_tmp77B->f2;_LLC9:;
({struct Cyc_Tcutil_CVTEnv _tmpD5E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp743,& Cyc_Tcutil_ek,_tmp77D,1,1);_tmp743=_tmpD5E;});
({struct Cyc_Tcutil_CVTEnv _tmpD5F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp743,& Cyc_Tcutil_trk,_tmp77C,1,1);_tmp743=_tmpD5F;});}}{
# 4364
struct Cyc_Tcenv_Fenv*_tmp77E=Cyc_Tcenv_bogus_fenv(_tmp831,_tmp830);
struct Cyc_Tcenv_Tenv*_tmp77F=({struct Cyc_Tcenv_Tenv*_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5->ns=te->ns,_tmp7C5->ae=te->ae,_tmp7C5->le=_tmp77E,_tmp7C5->allow_valueof=1,_tmp7C5->in_extern_c_include=te->in_extern_c_include;_tmp7C5;});
struct _tuple30 _tmp780=({unsigned int _tmpD63=loc;struct Cyc_Tcenv_Tenv*_tmpD62=_tmp77F;struct Cyc_Tcutil_CVTEnv _tmpD61=_tmp743;struct _dyneither_ptr _tmpD60=({const char*_tmp7C4="@requires";_tag_dyneither(_tmp7C4,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpD63,_tmpD62,_tmpD61,_tmpD60,_tmp82B);});struct _tuple30 _tmp781=_tmp780;struct Cyc_Tcutil_CVTEnv _tmp7C3;struct Cyc_List_List*_tmp7C2;_LLCB: _tmp7C3=_tmp781.f1;_tmp7C2=_tmp781.f2;_LLCC:;
_tmp743=_tmp7C3;
*_tmp82A=_tmp7C2;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp830,_tmp7C2);{
# 4377
struct _tuple30 _tmp782=({unsigned int _tmpD67=loc;struct Cyc_Tcenv_Tenv*_tmpD66=_tmp77F;struct Cyc_Tcutil_CVTEnv _tmpD65=_tmp743;struct _dyneither_ptr _tmpD64=({const char*_tmp7C1="@ensures";_tag_dyneither(_tmp7C1,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpD67,_tmpD66,_tmpD65,_tmpD64,_tmp829);});struct _tuple30 _tmp783=_tmp782;struct Cyc_Tcutil_CVTEnv _tmp7C0;struct Cyc_List_List*_tmp7BF;_LLCE: _tmp7C0=_tmp783.f1;_tmp7BF=_tmp783.f2;_LLCF:;
_tmp743=_tmp7C0;
*_tmp828=_tmp7BF;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp830,_tmp7BF);
# 4382
if(*_tmp833 != 0)
({struct Cyc_Tcutil_CVTEnv _tmpD68=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp743,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp833),1,1);_tmp743=_tmpD68;});else{
# 4385
struct Cyc_List_List*effect=0;
# 4390
{struct Cyc_List_List*tvs=_tmp743.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp784=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp785=_tmp784;struct Cyc_Absyn_Tvar*_tmp798;int _tmp797;_LLD1: _tmp798=_tmp785.f1;_tmp797=_tmp785.f2;_LLD2:;
if(!_tmp797)continue;{
void*_tmp786=Cyc_Absyn_compress_kb(_tmp798->kind);void*_tmp787=_tmp786;struct Cyc_Core_Opt**_tmp796;struct Cyc_Absyn_Kind*_tmp795;struct Cyc_Core_Opt**_tmp794;struct Cyc_Core_Opt**_tmp793;struct Cyc_Absyn_Kind*_tmp792;switch(*((int*)_tmp787)){case 2U: _LLD4: _tmp793=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp787)->f1;_tmp792=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp787)->f2;if(_tmp792->kind == Cyc_Absyn_RgnKind){_LLD5:
# 4396
 if(_tmp792->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpD69=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp793=_tmpD69;});_tmp795=_tmp792;goto _LLD7;}
# 4399
({struct Cyc_Core_Opt*_tmpD6A=Cyc_Tcutil_kind_to_bound_opt(_tmp792);*_tmp793=_tmpD6A;});_tmp795=_tmp792;goto _LLD7;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp787)->f2)->kind){case Cyc_Absyn_IntKind: _LLD8: _LLD9:
# 4402
 goto _LLDB;case Cyc_Absyn_EffKind: _LLDC: _tmp794=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp787)->f1;_LLDD:
# 4405
({struct Cyc_Core_Opt*_tmpD6B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp794=_tmpD6B;});goto _LLDF;default: goto _LLE2;}}case 0U: _LLD6: _tmp795=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp787)->f1;if(_tmp795->kind == Cyc_Absyn_RgnKind){_LLD7:
# 4401
({struct Cyc_List_List*_tmpD6E=({struct Cyc_List_List*_tmp78A=_cycalloc(sizeof(*_tmp78A));({void*_tmpD6D=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789->tag=23U,({void*_tmpD6C=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp788=_cycalloc(sizeof(*_tmp788));_tmp788->tag=2U,_tmp788->f1=_tmp798;_tmp788;});_tmp789->f1=_tmpD6C;});_tmp789;});_tmp78A->hd=_tmpD6D;}),_tmp78A->tl=effect;_tmp78A;});effect=_tmpD6E;});goto _LLD3;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp787)->f1)->kind){case Cyc_Absyn_IntKind: _LLDA: _LLDB:
# 4403
 goto _LLD3;case Cyc_Absyn_EffKind: _LLDE: _LLDF:
# 4407
({struct Cyc_List_List*_tmpD70=({struct Cyc_List_List*_tmp78C=_cycalloc(sizeof(*_tmp78C));({void*_tmpD6F=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp78B=_cycalloc(sizeof(*_tmp78B));_tmp78B->tag=2U,_tmp78B->f1=_tmp798;_tmp78B;});_tmp78C->hd=_tmpD6F;}),_tmp78C->tl=effect;_tmp78C;});effect=_tmpD70;});goto _LLD3;default: _LLE2: _LLE3:
# 4412
({struct Cyc_List_List*_tmpD73=({struct Cyc_List_List*_tmp791=_cycalloc(sizeof(*_tmp791));({void*_tmpD72=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp790=_cycalloc(sizeof(*_tmp790));_tmp790->tag=25U,({void*_tmpD71=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp78F=_cycalloc(sizeof(*_tmp78F));_tmp78F->tag=2U,_tmp78F->f1=_tmp798;_tmp78F;});_tmp790->f1=_tmpD71;});_tmp790;});_tmp791->hd=_tmpD72;}),_tmp791->tl=effect;_tmp791;});effect=_tmpD73;});goto _LLD3;}}default: _LLE0: _tmp796=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp787)->f1;_LLE1:
# 4409
({struct Cyc_Core_Opt*_tmpD75=({struct Cyc_Core_Opt*_tmp78E=_cycalloc(sizeof(*_tmp78E));({void*_tmpD74=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp78D=_cycalloc(sizeof(*_tmp78D));_tmp78D->tag=2U,_tmp78D->f1=0,_tmp78D->f2=& Cyc_Tcutil_ak;_tmp78D;});_tmp78E->v=_tmpD74;});_tmp78E;});*_tmp796=_tmpD75;});goto _LLE3;}_LLD3:;};}}
# 4416
{struct Cyc_List_List*ts=_tmp743.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp799=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp79A=_tmp799;void*_tmp7A3;int _tmp7A2;_LLE5: _tmp7A3=_tmp79A.f1;_tmp7A2=_tmp79A.f2;_LLE6:;
if(!_tmp7A2)continue;{
struct Cyc_Absyn_Kind*_tmp79B=Cyc_Tcutil_typ_kind(_tmp7A3);struct Cyc_Absyn_Kind*_tmp79C=_tmp79B;switch(((struct Cyc_Absyn_Kind*)_tmp79C)->kind){case Cyc_Absyn_RgnKind: _LLE8: _LLE9:
# 4421
({struct Cyc_List_List*_tmpD77=({struct Cyc_List_List*_tmp79E=_cycalloc(sizeof(*_tmp79E));({void*_tmpD76=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D->tag=23U,_tmp79D->f1=_tmp7A3;_tmp79D;});_tmp79E->hd=_tmpD76;}),_tmp79E->tl=effect;_tmp79E;});effect=_tmpD77;});goto _LLE7;case Cyc_Absyn_EffKind: _LLEA: _LLEB:
# 4423
({struct Cyc_List_List*_tmpD78=({struct Cyc_List_List*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F->hd=_tmp7A3,_tmp79F->tl=effect;_tmp79F;});effect=_tmpD78;});goto _LLE7;case Cyc_Absyn_IntKind: _LLEC: _LLED:
 goto _LLE7;default: _LLEE: _LLEF:
# 4426
({struct Cyc_List_List*_tmpD7A=({struct Cyc_List_List*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));({void*_tmpD79=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0->tag=25U,_tmp7A0->f1=_tmp7A3;_tmp7A0;});_tmp7A1->hd=_tmpD79;}),_tmp7A1->tl=effect;_tmp7A1;});effect=_tmpD7A;});goto _LLE7;}_LLE7:;};}}
# 4429
({void*_tmpD7B=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));_tmp7A4->tag=24U,_tmp7A4->f1=effect;_tmp7A4;});*_tmp833=_tmpD7B;});}
# 4435
if(*_tmp834 != 0){
struct Cyc_List_List*bs=*_tmp834;for(0;bs != 0;bs=bs->tl){
void*_tmp7A5=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp7A6=_tmp7A5;struct Cyc_Core_Opt**_tmp7B6;struct Cyc_Absyn_Kind*_tmp7B5;struct Cyc_Core_Opt**_tmp7B4;struct Cyc_Core_Opt**_tmp7B3;struct Cyc_Core_Opt**_tmp7B2;struct Cyc_Core_Opt**_tmp7B1;struct Cyc_Core_Opt**_tmp7B0;struct Cyc_Core_Opt**_tmp7AF;struct Cyc_Core_Opt**_tmp7AE;struct Cyc_Core_Opt**_tmp7AD;struct Cyc_Core_Opt**_tmp7AC;switch(*((int*)_tmp7A6)){case 1U: _LLF1: _tmp7AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LLF2:
# 4439
({struct Cyc_String_pa_PrintArg_struct _tmp7A9=({struct Cyc_String_pa_PrintArg_struct _tmpA82;_tmpA82.tag=0U,_tmpA82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmpA82;});void*_tmp7A7[1U];_tmp7A7[0]=& _tmp7A9;({unsigned int _tmpD7D=loc;struct _dyneither_ptr _tmpD7C=({const char*_tmp7A8="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp7A8,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpD7D,_tmpD7C,_tag_dyneither(_tmp7A7,sizeof(void*),1U));});});
# 4441
_tmp7AD=_tmp7AC;goto _LLF4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f2)->aliasqual == Cyc_Absyn_Top){_LLF3: _tmp7AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LLF4:
 _tmp7AE=_tmp7AD;goto _LLF6;}else{goto _LL103;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF5: _tmp7AE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LLF6:
 _tmp7AF=_tmp7AE;goto _LLF8;case Cyc_Absyn_Aliasable: _LLF7: _tmp7AF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LLF8:
 _tmp7B1=_tmp7AF;goto _LLFA;default: _LLFD: _tmp7B0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LLFE:
# 4448
 _tmp7B3=_tmp7B0;goto _LL100;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF9: _tmp7B1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LLFA:
# 4445
 _tmp7B2=_tmp7B1;goto _LLFC;case Cyc_Absyn_Aliasable: _LLFB: _tmp7B2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LLFC:
# 4447
({struct Cyc_Core_Opt*_tmpD7E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp7B2=_tmpD7E;});goto _LLF0;default: _LLFF: _tmp7B3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LL100:
# 4450
({struct Cyc_Core_Opt*_tmpD7F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp7B3=_tmpD7F;});goto _LLF0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f2)->aliasqual == Cyc_Absyn_Top){_LL101: _tmp7B4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_LL102:
# 4452
({struct Cyc_Core_Opt*_tmpD80=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp7B4=_tmpD80;});goto _LLF0;}else{goto _LL103;}default: _LL103: _tmp7B6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f1;_tmp7B5=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7A6)->f2;_LL104:
# 4454
({struct Cyc_Core_Opt*_tmpD81=Cyc_Tcutil_kind_to_bound_opt(_tmp7B5);*_tmp7B6=_tmpD81;});goto _LLF0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7A6)->f1)->kind == Cyc_Absyn_MemKind){_LL105: _LL106:
# 4456
({void*_tmp7AA=0U;({unsigned int _tmpD83=loc;struct _dyneither_ptr _tmpD82=({const char*_tmp7AB="functions can't abstract M types";_tag_dyneither(_tmp7AB,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpD83,_tmpD82,_tag_dyneither(_tmp7AA,sizeof(void*),0U));});});goto _LLF0;}else{_LL107: _LL108:
 goto _LLF0;}}_LLF0:;}}
# 4461
({struct Cyc_List_List*_tmpD84=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp743.kind_env,*_tmp834);cvtenv.kind_env=_tmpD84;});
({struct Cyc_List_List*_tmpD85=Cyc_Tcutil_remove_bound_tvars_bool(_tmp743.r,_tmp743.free_vars,*_tmp834);_tmp743.free_vars=_tmpD85;});
# 4464
{struct Cyc_List_List*tvs=_tmp743.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp7B7=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp7B8=_tmp7B7;struct Cyc_Absyn_Tvar*_tmp7BA;int _tmp7B9;_LL10A: _tmp7BA=_tmp7B8.f1;_tmp7B9=_tmp7B8.f2;_LL10B:;
({struct Cyc_List_List*_tmpD86=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7BA,_tmp7B9);cvtenv.free_vars=_tmpD86;});}}
# 4469
{struct Cyc_List_List*evs=_tmp743.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp7BB=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp7BC=_tmp7BB;void*_tmp7BE;int _tmp7BD;_LL10D: _tmp7BE=_tmp7BC.f1;_tmp7BD=_tmp7BC.f2;_LL10E:;
({struct Cyc_List_List*_tmpD87=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp7BE,_tmp7BD);cvtenv.free_evars=_tmpD87;});}}
# 4473
goto _LL0;};};};}case 10U: _LL23: _tmp835=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6B6)->f1;_LL24:
# 4476
 for(0;_tmp835 != 0;_tmp835=_tmp835->tl){
struct _tuple12*_tmp7C6=(struct _tuple12*)_tmp835->hd;
({struct Cyc_Tcutil_CVTEnv _tmpD88=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp7C6).f2,1,0);cvtenv=_tmpD88;});
({int _tmpD89=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp7C6).f1).print_const,(*_tmp7C6).f2);
# 4479
((*_tmp7C6).f1).real_const=_tmpD89;});}
# 4482
goto _LL0;case 12U: _LL25: _tmp837=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6B6)->f1;_tmp836=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6B6)->f2;_LL26: {
# 4486
struct Cyc_List_List*prev_fields=0;
for(0;_tmp836 != 0;_tmp836=_tmp836->tl){
struct Cyc_Absyn_Aggrfield*_tmp7C7=(struct Cyc_Absyn_Aggrfield*)_tmp836->hd;struct Cyc_Absyn_Aggrfield*_tmp7C8=_tmp7C7;struct _dyneither_ptr*_tmp7D8;struct Cyc_Absyn_Tqual*_tmp7D7;void*_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_List_List*_tmp7D4;struct Cyc_Absyn_Exp*_tmp7D3;_LL110: _tmp7D8=_tmp7C8->name;_tmp7D7=(struct Cyc_Absyn_Tqual*)& _tmp7C8->tq;_tmp7D6=_tmp7C8->type;_tmp7D5=_tmp7C8->width;_tmp7D4=_tmp7C8->attributes;_tmp7D3=_tmp7C8->requires_clause;_LL111:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7D8))
({struct Cyc_String_pa_PrintArg_struct _tmp7CB=({struct Cyc_String_pa_PrintArg_struct _tmpA84;_tmpA84.tag=0U,_tmpA84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7D8);_tmpA84;});void*_tmp7C9[1U];_tmp7C9[0]=& _tmp7CB;({unsigned int _tmpD8B=loc;struct _dyneither_ptr _tmpD8A=({const char*_tmp7CA="duplicate field %s";_tag_dyneither(_tmp7CA,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpD8B,_tmpD8A,_tag_dyneither(_tmp7C9,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpD8C=(struct _dyneither_ptr)*_tmp7D8;Cyc_strcmp(_tmpD8C,({const char*_tmp7CC="";_tag_dyneither(_tmp7CC,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpD8D=({struct Cyc_List_List*_tmp7CD=_cycalloc(sizeof(*_tmp7CD));_tmp7CD->hd=_tmp7D8,_tmp7CD->tl=prev_fields;_tmp7CD;});prev_fields=_tmpD8D;});
({struct Cyc_Tcutil_CVTEnv _tmpD8E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7D6,1,0);cvtenv=_tmpD8E;});
({int _tmpD8F=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7D7->print_const,_tmp7D6);_tmp7D7->real_const=_tmpD8F;});
# 4499
Cyc_Tcutil_check_bitfield(loc,te,_tmp7D6,_tmp7D5,_tmp7D8);
Cyc_Tcutil_check_field_atts(loc,_tmp7D8,_tmp7D4);
if(_tmp7D3 != 0){
# 4503
if(_tmp837 != Cyc_Absyn_UnionA)
({void*_tmp7CE=0U;({unsigned int _tmpD91=loc;struct _dyneither_ptr _tmpD90=({const char*_tmp7CF="@requires clause is only allowed on union members";_tag_dyneither(_tmp7CF,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpD91,_tmpD90,_tag_dyneither(_tmp7CE,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpD92=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD92,0,_tmp7D3);});
if(!Cyc_Tcutil_is_integral(_tmp7D3))
({struct Cyc_String_pa_PrintArg_struct _tmp7D2=({struct Cyc_String_pa_PrintArg_struct _tmpA85;_tmpA85.tag=0U,({
struct _dyneither_ptr _tmpD93=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7D3->topt)));_tmpA85.f1=_tmpD93;});_tmpA85;});void*_tmp7D0[1U];_tmp7D0[0]=& _tmp7D2;({unsigned int _tmpD95=loc;struct _dyneither_ptr _tmpD94=({const char*_tmp7D1="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp7D1,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpD95,_tmpD94,_tag_dyneither(_tmp7D0,sizeof(void*),1U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD96=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D3,te,cvtenv);cvtenv=_tmpD96;});}}
# 4512
goto _LL0;}case 11U: _LL27: _tmp839=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6B6)->f1).aggr_info;_tmp838=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6B6)->f1).targs;_LL28:
# 4515
{union Cyc_Absyn_AggrInfoU _tmp7D9=*_tmp839;union Cyc_Absyn_AggrInfoU _tmp7DA=_tmp7D9;struct Cyc_Absyn_Aggrdecl*_tmp7FD;enum Cyc_Absyn_AggrKind _tmp7FC;struct _tuple2*_tmp7FB;struct Cyc_Core_Opt*_tmp7FA;if((_tmp7DA.UnknownAggr).tag == 1){_LL113: _tmp7FC=((_tmp7DA.UnknownAggr).val).f1;_tmp7FB=((_tmp7DA.UnknownAggr).val).f2;_tmp7FA=((_tmp7DA.UnknownAggr).val).f3;_LL114: {
# 4517
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp7DB;_push_handler(& _tmp7DB);{int _tmp7DD=0;if(setjmp(_tmp7DB.handler))_tmp7DD=1;if(!_tmp7DD){
({struct Cyc_Absyn_Aggrdecl**_tmpD97=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp7FB);adp=_tmpD97;});{
struct Cyc_Absyn_Aggrdecl*_tmp7DE=*adp;
if(_tmp7DE->kind != _tmp7FC){
if(_tmp7DE->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp7E1=({struct Cyc_String_pa_PrintArg_struct _tmpA87;_tmpA87.tag=0U,({struct _dyneither_ptr _tmpD98=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));_tmpA87.f1=_tmpD98;});_tmpA87;});struct Cyc_String_pa_PrintArg_struct _tmp7E2=({struct Cyc_String_pa_PrintArg_struct _tmpA86;_tmpA86.tag=0U,({
struct _dyneither_ptr _tmpD99=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));_tmpA86.f1=_tmpD99;});_tmpA86;});void*_tmp7DF[2U];_tmp7DF[0]=& _tmp7E1,_tmp7DF[1]=& _tmp7E2;({unsigned int _tmpD9B=loc;struct _dyneither_ptr _tmpD9A=({const char*_tmp7E0="expecting struct %s instead of union %s";_tag_dyneither(_tmp7E0,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpD9B,_tmpD9A,_tag_dyneither(_tmp7DF,sizeof(void*),2U));});});else{
# 4526
({struct Cyc_String_pa_PrintArg_struct _tmp7E5=({struct Cyc_String_pa_PrintArg_struct _tmpA89;_tmpA89.tag=0U,({struct _dyneither_ptr _tmpD9C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));_tmpA89.f1=_tmpD9C;});_tmpA89;});struct Cyc_String_pa_PrintArg_struct _tmp7E6=({struct Cyc_String_pa_PrintArg_struct _tmpA88;_tmpA88.tag=0U,({
struct _dyneither_ptr _tmpD9D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));_tmpA88.f1=_tmpD9D;});_tmpA88;});void*_tmp7E3[2U];_tmp7E3[0]=& _tmp7E5,_tmp7E3[1]=& _tmp7E6;({unsigned int _tmpD9F=loc;struct _dyneither_ptr _tmpD9E=({const char*_tmp7E4="expecting union %s instead of struct %s";_tag_dyneither(_tmp7E4,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpD9F,_tmpD9E,_tag_dyneither(_tmp7E3,sizeof(void*),2U));});});}}
# 4529
if((unsigned int)_tmp7FA  && (int)_tmp7FA->v){
if(!((unsigned int)_tmp7DE->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7DE->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp7E9=({struct Cyc_String_pa_PrintArg_struct _tmpA8A;_tmpA8A.tag=0U,({
struct _dyneither_ptr _tmpDA0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));_tmpA8A.f1=_tmpDA0;});_tmpA8A;});void*_tmp7E7[1U];_tmp7E7[0]=& _tmp7E9;({unsigned int _tmpDA2=loc;struct _dyneither_ptr _tmpDA1=({const char*_tmp7E8="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp7E8,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpDA2,_tmpDA1,_tag_dyneither(_tmp7E7,sizeof(void*),1U));});});}
# 4535
({union Cyc_Absyn_AggrInfoU _tmpDA3=Cyc_Absyn_KnownAggr(adp);*_tmp839=_tmpDA3;});};
# 4519
;_pop_handler();}else{void*_tmp7DC=(void*)_exn_thrown;void*_tmp7EA=_tmp7DC;void*_tmp7F0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7EA)->tag == Cyc_Dict_Absent){_LL118: _LL119: {
# 4539
struct Cyc_Absyn_Aggrdecl*_tmp7EB=({struct Cyc_Absyn_Aggrdecl*_tmp7EF=_cycalloc(sizeof(*_tmp7EF));_tmp7EF->kind=_tmp7FC,_tmp7EF->sc=Cyc_Absyn_Extern,_tmp7EF->name=_tmp7FB,_tmp7EF->tvs=0,_tmp7EF->impl=0,_tmp7EF->attributes=0,_tmp7EF->expected_mem_kind=0;_tmp7EF;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp7EB);
({struct Cyc_Absyn_Aggrdecl**_tmpDA4=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp7FB);adp=_tmpDA4;});
({union Cyc_Absyn_AggrInfoU _tmpDA5=Cyc_Absyn_KnownAggr(adp);*_tmp839=_tmpDA5;});
# 4544
if(*_tmp838 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7EE=({struct Cyc_String_pa_PrintArg_struct _tmpA8B;_tmpA8B.tag=0U,({
struct _dyneither_ptr _tmpDA6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FB));_tmpA8B.f1=_tmpDA6;});_tmpA8B;});void*_tmp7EC[1U];_tmp7EC[0]=& _tmp7EE;({unsigned int _tmpDA8=loc;struct _dyneither_ptr _tmpDA7=({const char*_tmp7ED="declare parameterized type %s before using";_tag_dyneither(_tmp7ED,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpDA8,_tmpDA7,_tag_dyneither(_tmp7EC,sizeof(void*),1U));});});
return cvtenv;}
# 4549
goto _LL117;}}else{_LL11A: _tmp7F0=_tmp7EA;_LL11B:(int)_rethrow(_tmp7F0);}_LL117:;}};}
# 4551
_tmp7FD=*adp;goto _LL116;}}else{_LL115: _tmp7FD=*(_tmp7DA.KnownAggr).val;_LL116: {
# 4553
struct Cyc_List_List*tvs=_tmp7FD->tvs;
struct Cyc_List_List*ts=*_tmp838;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp7F1=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp7F2=(void*)ts->hd;
# 4561
{struct _tuple0 _tmp7F3=({struct _tuple0 _tmpA8C;({void*_tmpDA9=Cyc_Absyn_compress_kb(_tmp7F1->kind);_tmpA8C.f1=_tmpDA9;}),_tmpA8C.f2=_tmp7F2;_tmpA8C;});struct _tuple0 _tmp7F4=_tmp7F3;struct Cyc_Absyn_Tvar*_tmp7F5;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7F4.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F4.f2)->tag == 2U){_LL11D: _tmp7F5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7F4.f2)->f1;_LL11E:
# 4563
({struct Cyc_List_List*_tmpDAA=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7F5);cvtenv.kind_env=_tmpDAA;});
({struct Cyc_List_List*_tmpDAB=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7F5,1);cvtenv.free_vars=_tmpDAB;});
continue;}else{goto _LL11F;}}else{_LL11F: _LL120:
 goto _LL11C;}_LL11C:;}{
# 4568
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDAC=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpDAC;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4572
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7F8=({struct Cyc_String_pa_PrintArg_struct _tmpA8D;_tmpA8D.tag=0U,({struct _dyneither_ptr _tmpDAD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7FD->name));_tmpA8D.f1=_tmpDAD;});_tmpA8D;});void*_tmp7F6[1U];_tmp7F6[0]=& _tmp7F8;({unsigned int _tmpDAF=loc;struct _dyneither_ptr _tmpDAE=({const char*_tmp7F7="too many parameters for type %s";_tag_dyneither(_tmp7F7,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpDAF,_tmpDAE,_tag_dyneither(_tmp7F6,sizeof(void*),1U));});});
if(tvs != 0){
# 4576
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDB0=({struct Cyc_List_List*_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9->hd=e,_tmp7F9->tl=hidden_ts;_tmp7F9;});hidden_ts=_tmpDB0;});
({struct Cyc_Tcutil_CVTEnv _tmpDB1=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpDB1;});}
# 4583
({struct Cyc_List_List*_tmpDB3=({struct Cyc_List_List*_tmpDB2=*_tmp838;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDB2,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp838=_tmpDB3;});}
# 4585
if((allow_abs_aggr  && _tmp7FD->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4590
_tmp7FD->expected_mem_kind=1;}}_LL112:;}
# 4593
goto _LL0;case 17U: _LL29: _tmp83D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B6)->f1;_tmp83C=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B6)->f2;_tmp83B=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B6)->f3;_tmp83A=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6B6)->f4;_LL2A: {
# 4596
struct Cyc_List_List*targs=*_tmp83C;
# 4598
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp7FE;_push_handler(& _tmp7FE);{int _tmp800=0;if(setjmp(_tmp7FE.handler))_tmp800=1;if(!_tmp800){({struct Cyc_Absyn_Typedefdecl*_tmpDB4=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp83D);td=_tmpDB4;});;_pop_handler();}else{void*_tmp7FF=(void*)_exn_thrown;void*_tmp801=_tmp7FF;void*_tmp805;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp801)->tag == Cyc_Dict_Absent){_LL122: _LL123:
# 4601
({struct Cyc_String_pa_PrintArg_struct _tmp804=({struct Cyc_String_pa_PrintArg_struct _tmpA8E;_tmpA8E.tag=0U,({struct _dyneither_ptr _tmpDB5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp83D));_tmpA8E.f1=_tmpDB5;});_tmpA8E;});void*_tmp802[1U];_tmp802[0]=& _tmp804;({unsigned int _tmpDB7=loc;struct _dyneither_ptr _tmpDB6=({const char*_tmp803="unbound typedef name %s";_tag_dyneither(_tmp803,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpDB7,_tmpDB6,_tag_dyneither(_tmp802,sizeof(void*),1U));});});
return cvtenv;}else{_LL124: _tmp805=_tmp801;_LL125:(int)_rethrow(_tmp805);}_LL121:;}};}
# 4604
*_tmp83B=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4609
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4614
({struct Cyc_Tcutil_CVTEnv _tmpDB8=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpDB8;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpDBA=({struct Cyc_List_List*_tmp807=_cycalloc(sizeof(*_tmp807));({struct _tuple16*_tmpDB9=({struct _tuple16*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp806->f2=(void*)ts->hd;_tmp806;});_tmp807->hd=_tmpDB9;}),_tmp807->tl=inst;_tmp807;});inst=_tmpDBA;});}
# 4618
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp80A=({struct Cyc_String_pa_PrintArg_struct _tmpA8F;_tmpA8F.tag=0U,({struct _dyneither_ptr _tmpDBB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp83D));_tmpA8F.f1=_tmpDBB;});_tmpA8F;});void*_tmp808[1U];_tmp808[0]=& _tmp80A;({unsigned int _tmpDBD=loc;struct _dyneither_ptr _tmpDBC=({const char*_tmp809="too many parameters for typedef %s";_tag_dyneither(_tmp809,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpDBD,_tmpDBC,_tag_dyneither(_tmp808,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4623
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDBE=({struct Cyc_List_List*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B->hd=e,_tmp80B->tl=hidden_ts;_tmp80B;});hidden_ts=_tmpDBE;});
({struct Cyc_Tcutil_CVTEnv _tmpDBF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpDBF;});
({struct Cyc_List_List*_tmpDC1=({struct Cyc_List_List*_tmp80D=_cycalloc(sizeof(*_tmp80D));({struct _tuple16*_tmpDC0=({struct _tuple16*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp80C->f2=e;_tmp80C;});_tmp80D->hd=_tmpDC0;}),_tmp80D->tl=inst;_tmp80D;});inst=_tmpDC1;});}
# 4631
({struct Cyc_List_List*_tmpDC3=({struct Cyc_List_List*_tmpDC2=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDC2,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp83C=_tmpDC3;});}
# 4633
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp83A=new_typ;}
# 4639
goto _LL0;};}case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL0;case 15U: _LL31: _tmp83E=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6B6)->f1;_LL32:
# 4644
({struct Cyc_Tcutil_CVTEnv _tmpDC4=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp83E,1,allow_abs_aggr);cvtenv=_tmpDC4;});goto _LL0;case 16U: _LL33: _tmp840=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6B6)->f1;_tmp83F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6B6)->f2;_LL34:
# 4647
({struct Cyc_Tcutil_CVTEnv _tmpDC5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp840,0,0);cvtenv=_tmpDC5;});
({struct Cyc_Tcutil_CVTEnv _tmpDC6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp83F,1,0);cvtenv=_tmpDC6;});
goto _LL0;case 23U: _LL35: _tmp841=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6B6)->f1;_LL36:
# 4651
({struct Cyc_Tcutil_CVTEnv _tmpDC7=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp841,1,1);cvtenv=_tmpDC7;});goto _LL0;case 25U: _LL37: _tmp842=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6B6)->f1;_LL38:
# 4653
({struct Cyc_Tcutil_CVTEnv _tmpDC8=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp842,1,1);cvtenv=_tmpDC8;});goto _LL0;default: _LL39: _tmp843=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6B6)->f1;_LL3A:
# 4655
 for(0;_tmp843 != 0;_tmp843=_tmp843->tl){
({struct Cyc_Tcutil_CVTEnv _tmpDC9=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp843->hd,1,1);cvtenv=_tmpDC9;});}
goto _LL0;}_LL0:;}
# 4659
if(!({struct Cyc_Absyn_Kind*_tmpDCA=Cyc_Tcutil_typ_kind(t);Cyc_Tcutil_kind_leq(_tmpDCA,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp846=({struct Cyc_String_pa_PrintArg_struct _tmpA92;_tmpA92.tag=0U,({
struct _dyneither_ptr _tmpDCB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA92.f1=_tmpDCB;});_tmpA92;});struct Cyc_String_pa_PrintArg_struct _tmp847=({struct Cyc_String_pa_PrintArg_struct _tmpA91;_tmpA91.tag=0U,({struct _dyneither_ptr _tmpDCC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));_tmpA91.f1=_tmpDCC;});_tmpA91;});struct Cyc_String_pa_PrintArg_struct _tmp848=({struct Cyc_String_pa_PrintArg_struct _tmpA90;_tmpA90.tag=0U,({struct _dyneither_ptr _tmpDCD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmpA90.f1=_tmpDCD;});_tmpA90;});void*_tmp844[3U];_tmp844[0]=& _tmp846,_tmp844[1]=& _tmp847,_tmp844[2]=& _tmp848;({unsigned int _tmpDCF=loc;struct _dyneither_ptr _tmpDCE=({const char*_tmp845="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp845,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpDCF,_tmpDCE,_tag_dyneither(_tmp844,sizeof(void*),3U));});});
# 4663
return cvtenv;}
# 4671
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4673
{void*_tmp849=e->r;void*_tmp84A=_tmp849;struct Cyc_Absyn_Exp*_tmp85C;void*_tmp85B;void*_tmp85A;void*_tmp859;void*_tmp858;struct Cyc_Absyn_Exp*_tmp857;struct Cyc_Absyn_Exp*_tmp856;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_Absyn_Exp*_tmp854;struct Cyc_Absyn_Exp*_tmp853;struct Cyc_Absyn_Exp*_tmp852;struct Cyc_Absyn_Exp*_tmp851;struct Cyc_Absyn_Exp*_tmp850;struct Cyc_Absyn_Exp*_tmp84F;struct Cyc_Absyn_Exp*_tmp84E;struct Cyc_List_List*_tmp84D;switch(*((int*)_tmp84A)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp84D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp84A)->f2;_LLC:
# 4680
 for(0;_tmp84D != 0;_tmp84D=_tmp84D->tl){
({struct Cyc_Tcutil_CVTEnv _tmpDD0=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp84D->hd,te,cvtenv);cvtenv=_tmpDD0;});}
goto _LL0;case 6U: _LLD: _tmp850=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_tmp84F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp84A)->f2;_tmp84E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp84A)->f3;_LLE:
# 4684
({struct Cyc_Tcutil_CVTEnv _tmpDD1=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp850,te,cvtenv);cvtenv=_tmpDD1;});
({struct Cyc_Tcutil_CVTEnv _tmpDD2=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp84F,te,cvtenv);cvtenv=_tmpDD2;});
({struct Cyc_Tcutil_CVTEnv _tmpDD3=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp84E,te,cvtenv);cvtenv=_tmpDD3;});
goto _LL0;case 7U: _LLF: _tmp852=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_tmp851=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp84A)->f2;_LL10:
 _tmp854=_tmp852;_tmp853=_tmp851;goto _LL12;case 8U: _LL11: _tmp854=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_tmp853=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp84A)->f2;_LL12:
 _tmp856=_tmp854;_tmp855=_tmp853;goto _LL14;case 9U: _LL13: _tmp856=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_tmp855=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp84A)->f2;_LL14:
# 4691
({struct Cyc_Tcutil_CVTEnv _tmpDD4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp856,te,cvtenv);cvtenv=_tmpDD4;});
({struct Cyc_Tcutil_CVTEnv _tmpDD5=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp855,te,cvtenv);cvtenv=_tmpDD5;});
goto _LL0;case 14U: _LL15: _tmp858=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_tmp857=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp84A)->f2;_LL16:
# 4695
({struct Cyc_Tcutil_CVTEnv _tmpDD6=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp857,te,cvtenv);cvtenv=_tmpDD6;});
({struct Cyc_Tcutil_CVTEnv _tmpDD7=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp858,1,0);cvtenv=_tmpDD7;});
goto _LL0;case 19U: _LL17: _tmp859=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_LL18:
 _tmp85A=_tmp859;goto _LL1A;case 17U: _LL19: _tmp85A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_LL1A:
# 4700
({struct Cyc_Tcutil_CVTEnv _tmpDD8=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp85A,1,0);cvtenv=_tmpDD8;});
goto _LL0;case 39U: _LL1B: _tmp85B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_LL1C:
# 4703
({struct Cyc_Tcutil_CVTEnv _tmpDD9=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp85B,1,0);cvtenv=_tmpDD9;});
goto _LL0;case 18U: _LL1D: _tmp85C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp84A)->f1;_LL1E:
# 4706
({struct Cyc_Tcutil_CVTEnv _tmpDDA=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp85C,te,cvtenv);cvtenv=_tmpDDA;});
goto _LL0;default: _LL1F: _LL20:
# 4709
({void*_tmp84B=0U;({struct _dyneither_ptr _tmpDDB=({const char*_tmp84C="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp84C,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDDB,_tag_dyneither(_tmp84B,sizeof(void*),0U));});});}_LL0:;}
# 4711
return cvtenv;}
# 4714
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4719
struct Cyc_List_List*_tmp85D=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpDDC=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpDDC;});
# 4722
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp85E=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp85F=_tmp85E;struct Cyc_Absyn_Tvar*_tmp860;_LL1: _tmp860=_tmp85F.f1;_LL2:;
({struct Cyc_List_List*_tmpDDD=Cyc_Tcutil_fast_add_free_tvar(_tmp85D,_tmp860);cvt.kind_env=_tmpDDD;});}}
# 4730
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp861=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp862=_tmp861;void*_tmp86B;_LL4: _tmp86B=_tmp862.f1;_LL5:;{
void*_tmp863=Cyc_Tcutil_compress(_tmp86B);void*_tmp864=_tmp863;struct Cyc_Core_Opt**_tmp86A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp864)->tag == 1U){_LL7: _tmp86A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp864)->f4;_LL8:
# 4734
 if(*_tmp86A == 0)
({struct Cyc_Core_Opt*_tmpDDE=({struct Cyc_Core_Opt*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865->v=_tmp85D;_tmp865;});*_tmp86A=_tmpDDE;});else{
# 4738
struct Cyc_List_List*_tmp866=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp86A))->v;
struct Cyc_List_List*_tmp867=0;
for(0;_tmp866 != 0;_tmp866=_tmp866->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp85D,(struct Cyc_Absyn_Tvar*)_tmp866->hd))
({struct Cyc_List_List*_tmpDDF=({struct Cyc_List_List*_tmp868=_cycalloc(sizeof(*_tmp868));_tmp868->hd=(struct Cyc_Absyn_Tvar*)_tmp866->hd,_tmp868->tl=_tmp867;_tmp868;});_tmp867=_tmpDDF;});}
({struct Cyc_Core_Opt*_tmpDE0=({struct Cyc_Core_Opt*_tmp869=_cycalloc(sizeof(*_tmp869));_tmp869->v=_tmp867;_tmp869;});*_tmp86A=_tmpDE0;});}
# 4745
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4749
return cvt;}
# 4755
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp86C=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp86D=_tmp86C;switch(((struct Cyc_Absyn_Kind*)_tmp86D)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp86D)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
# 4760
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp86E=0U;({struct _dyneither_ptr _tmpDE1=({const char*_tmp86F="can't unify evar with unique region!";_tag_dyneither(_tmp86F,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDE1,_tag_dyneither(_tmp86E,sizeof(void*),0U));});});
goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 goto _LL6;default: _LL5: _LL6:
# 4765
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp870=0U;({struct _dyneither_ptr _tmpDE2=({const char*_tmp871="can't unify evar with heap!";_tag_dyneither(_tmp871,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDE2,_tag_dyneither(_tmp870,sizeof(void*),0U));});});
goto _LL0;}case Cyc_Absyn_EffKind: _LL7: _LL8:
# 4768
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp872=0U;({struct _dyneither_ptr _tmpDE3=({const char*_tmp873="can't unify evar with {}!";_tag_dyneither(_tmp873,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDE3,_tag_dyneither(_tmp872,sizeof(void*),0U));});});
goto _LL0;default: _LL9: _LLA:
# 4771
({struct Cyc_String_pa_PrintArg_struct _tmp876=({struct Cyc_String_pa_PrintArg_struct _tmpA94;_tmpA94.tag=0U,({
struct _dyneither_ptr _tmpDE4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmpA94.f1=_tmpDE4;});_tmpA94;});struct Cyc_String_pa_PrintArg_struct _tmp877=({struct Cyc_String_pa_PrintArg_struct _tmpA93;_tmpA93.tag=0U,({struct _dyneither_ptr _tmpDE5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmpA93.f1=_tmpDE5;});_tmpA93;});void*_tmp874[2U];_tmp874[0]=& _tmp876,_tmp874[1]=& _tmp877;({unsigned int _tmpDE7=loc;struct _dyneither_ptr _tmpDE6=({const char*_tmp875="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp875,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpDE7,_tmpDE6,_tag_dyneither(_tmp874,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}}
# 4783
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp878=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp879=({unsigned int _tmpDEB=loc;struct Cyc_Tcenv_Tenv*_tmpDEA=te;struct Cyc_Tcutil_CVTEnv _tmpDE9=({struct Cyc_Tcutil_CVTEnv _tmpA98;_tmpA98.r=Cyc_Core_heap_region,_tmpA98.kind_env=_tmp878,_tmpA98.free_vars=0,_tmpA98.free_evars=0,_tmpA98.generalize_evars=generalize_evars,_tmpA98.fn_result=0;_tmpA98;});struct Cyc_Absyn_Kind*_tmpDE8=expected_kind;Cyc_Tcutil_check_valid_type(_tmpDEB,_tmpDEA,_tmpDE9,_tmpDE8,1,t);});
# 4791
struct Cyc_List_List*_tmp87A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp879.free_vars);
struct Cyc_List_List*_tmp87B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp879.free_evars);
# 4795
if(_tmp878 != 0){
struct Cyc_List_List*_tmp87C=0;
{struct Cyc_List_List*_tmp87D=_tmp87A;for(0;(unsigned int)_tmp87D;_tmp87D=_tmp87D->tl){
struct Cyc_Absyn_Tvar*_tmp87E=(struct Cyc_Absyn_Tvar*)_tmp87D->hd;
int found=0;
{struct Cyc_List_List*_tmp87F=_tmp878;for(0;(unsigned int)_tmp87F;_tmp87F=_tmp87F->tl){
if(Cyc_Absyn_tvar_cmp(_tmp87E,(struct Cyc_Absyn_Tvar*)_tmp87F->hd)== 0){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpDEC=({struct Cyc_List_List*_tmp880=_cycalloc(sizeof(*_tmp880));_tmp880->hd=(struct Cyc_Absyn_Tvar*)_tmp87D->hd,_tmp880->tl=_tmp87C;_tmp880;});_tmp87C=_tmpDEC;});}}
# 4805
({struct Cyc_List_List*_tmpDED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp87C);_tmp87A=_tmpDED;});}
# 4811
{struct Cyc_List_List*x=_tmp87A;for(0;x != 0;x=x->tl){
void*_tmp881=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp882=_tmp881;enum Cyc_Absyn_AliasQual _tmp890;struct Cyc_Core_Opt**_tmp88F;struct Cyc_Absyn_Kind*_tmp88E;struct Cyc_Core_Opt**_tmp88D;struct Cyc_Core_Opt**_tmp88C;struct Cyc_Core_Opt**_tmp88B;struct Cyc_Core_Opt**_tmp88A;struct Cyc_Core_Opt**_tmp889;struct Cyc_Core_Opt**_tmp888;switch(*((int*)_tmp882)){case 1U: _LL1: _tmp888=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp882)->f1;_LL2:
 _tmp889=_tmp888;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp889=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f1;_LL4:
 _tmp88A=_tmp889;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp88A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f1;_LL6:
 _tmp88B=_tmp88A;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp88B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f1;_LL8:
# 4817
({struct Cyc_Core_Opt*_tmpDEE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp88B=_tmpDEE;});goto _LL0;default: _LL9: _tmp88C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f1;_LLA:
# 4819
({struct Cyc_Core_Opt*_tmpDEF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp88C=_tmpDEF;});goto _LL0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp88D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f1;_LLC:
# 4821
({struct Cyc_Core_Opt*_tmpDF0=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp88D=_tmpDF0;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp88F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f1;_tmp88E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp882)->f2;_LLE:
# 4823
({struct Cyc_Core_Opt*_tmpDF1=Cyc_Tcutil_kind_to_bound_opt(_tmp88E);*_tmp88F=_tmpDF1;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp882)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp890=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp882)->f1)->aliasqual;_LL10:
# 4825
({struct Cyc_String_pa_PrintArg_struct _tmp885=({struct Cyc_String_pa_PrintArg_struct _tmpA96;_tmpA96.tag=0U,({
struct _dyneither_ptr _tmpDF2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmpA96.f1=_tmpDF2;});_tmpA96;});struct Cyc_String_pa_PrintArg_struct _tmp886=({struct Cyc_String_pa_PrintArg_struct _tmpA95;_tmpA95.tag=0U,({struct _dyneither_ptr _tmpDF3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp887=_cycalloc(sizeof(*_tmp887));_tmp887->kind=Cyc_Absyn_MemKind,_tmp887->aliasqual=_tmp890;_tmp887;})));_tmpA95.f1=_tmpDF3;});_tmpA95;});void*_tmp883[2U];_tmp883[0]=& _tmp885,_tmp883[1]=& _tmp886;({unsigned int _tmpDF5=loc;struct _dyneither_ptr _tmpDF4=({const char*_tmp884="type variable %s cannot have kind %s";_tag_dyneither(_tmp884,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpDF5,_tmpDF4,_tag_dyneither(_tmp883,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4832
if(_tmp87A != 0  || _tmp87B != 0){
{void*_tmp891=Cyc_Tcutil_compress(t);void*_tmp892=_tmp891;struct Cyc_List_List**_tmp893;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp892)->tag == 9U){_LL14: _tmp893=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp892)->f1).tvars;_LL15:
# 4835
 if(*_tmp893 == 0){
# 4837
({struct Cyc_List_List*_tmpDF6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp87A);*_tmp893=_tmpDF6;});
_tmp87A=0;}
# 4840
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4843
if(_tmp87A != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp896=({struct Cyc_String_pa_PrintArg_struct _tmpA97;_tmpA97.tag=0U,_tmpA97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp87A->hd)->name);_tmpA97;});void*_tmp894[1U];_tmp894[0]=& _tmp896;({unsigned int _tmpDF8=loc;struct _dyneither_ptr _tmpDF7=({const char*_tmp895="unbound type variable %s";_tag_dyneither(_tmp895,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpDF8,_tmpDF7,_tag_dyneither(_tmp894,sizeof(void*),1U));});});
Cyc_Tcutil_check_free_evars(_tmp87B,t,loc);}}
# 4853
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4856
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp897=Cyc_Tcutil_compress(t);void*_tmp898=_tmp897;struct Cyc_List_List*_tmp8A4;void*_tmp8A3;struct Cyc_Absyn_Tqual _tmp8A2;void*_tmp8A1;struct Cyc_List_List*_tmp8A0;struct Cyc_Absyn_Exp*_tmp89F;struct Cyc_List_List*_tmp89E;struct Cyc_Absyn_Exp*_tmp89D;struct Cyc_List_List*_tmp89C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->tag == 9U){_LL1: _tmp8A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).tvars;_tmp8A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).effect;_tmp8A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).ret_tqual;_tmp8A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).ret_typ;_tmp8A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).args;_tmp89F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).requires_clause;_tmp89E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).requires_relns;_tmp89D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).ensures_clause;_tmp89C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp898)->f1).ensures_relns;_LL2:
# 4859
 fd->tvs=_tmp8A4;
fd->effect=_tmp8A3;
{struct Cyc_List_List*_tmp899=fd->args;for(0;_tmp899 != 0;(_tmp899=_tmp899->tl,_tmp8A0=_tmp8A0->tl)){
# 4863
(*((struct _tuple10*)_tmp899->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp8A0))->hd)).f2;
(*((struct _tuple10*)_tmp899->hd)).f3=(*((struct _tuple10*)_tmp8A0->hd)).f3;}}
# 4866
fd->ret_tqual=_tmp8A2;
fd->ret_type=_tmp8A1;
({int _tmpDF9=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp8A1);(fd->ret_tqual).real_const=_tmpDF9;});
fd->requires_clause=_tmp89F;
fd->requires_relns=_tmp89E;
fd->ensures_clause=_tmp89D;
fd->ensures_relns=_tmp89C;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp89A=0U;({struct _dyneither_ptr _tmpDFA=({const char*_tmp89B="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp89B,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDFA,_tag_dyneither(_tmp89A,sizeof(void*),0U));});});}_LL0:;}
# 4876
({struct Cyc_List_List*_tmpDFC=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpDFB=loc;Cyc_Tcutil_check_unique_vars(_tmpDFC,_tmpDFB,({const char*_tmp8A5="function declaration has repeated parameter";_tag_dyneither(_tmp8A5,sizeof(char),44U);}));});
# 4879
fd->cached_typ=t;}
# 4884
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4890
struct Cyc_Tcutil_CVTEnv _tmp8A6=({unsigned int _tmpE01=loc;struct Cyc_Tcenv_Tenv*_tmpE00=te;struct Cyc_Tcutil_CVTEnv _tmpDFF=({struct Cyc_Tcutil_CVTEnv _tmpA9B;_tmpA9B.r=Cyc_Core_heap_region,_tmpA9B.kind_env=bound_tvars,_tmpA9B.free_vars=0,_tmpA9B.free_evars=0,_tmpA9B.generalize_evars=0,_tmpA9B.fn_result=0;_tmpA9B;});struct Cyc_Absyn_Kind*_tmpDFE=expected_kind;int _tmpDFD=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpE01,_tmpE00,_tmpDFF,_tmpDFE,_tmpDFD,t);});
# 4894
struct Cyc_List_List*_tmp8A7=({struct _RegionHandle*_tmpE03=Cyc_Core_heap_region;struct Cyc_List_List*_tmpE02=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp8A6.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpE03,_tmpE02,bound_tvars);});
# 4896
struct Cyc_List_List*_tmp8A8=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp8A6.free_evars);
{struct Cyc_List_List*fs=_tmp8A7;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp8A9=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp8AC=({struct Cyc_String_pa_PrintArg_struct _tmpA9A;_tmpA9A.tag=0U,_tmpA9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8A9);_tmpA9A;});struct Cyc_String_pa_PrintArg_struct _tmp8AD=({struct Cyc_String_pa_PrintArg_struct _tmpA99;_tmpA99.tag=0U,({struct _dyneither_ptr _tmpE04=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA99.f1=_tmpE04;});_tmpA99;});void*_tmp8AA[2U];_tmp8AA[0]=& _tmp8AC,_tmp8AA[1]=& _tmp8AD;({unsigned int _tmpE06=loc;struct _dyneither_ptr _tmpE05=({const char*_tmp8AB="unbound type variable %s in type %s";_tag_dyneither(_tmp8AB,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpE06,_tmpE05,_tag_dyneither(_tmp8AA,sizeof(void*),2U));});});}}
# 4901
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp8A8,t,loc);}
# 4905
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpE07=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpE07;});}
# 4910
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4916
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4919
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8B0=({struct Cyc_String_pa_PrintArg_struct _tmpA9D;_tmpA9D.tag=0U,_tmpA9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpA9D;});struct Cyc_String_pa_PrintArg_struct _tmp8B1=({struct Cyc_String_pa_PrintArg_struct _tmpA9C;_tmpA9C.tag=0U,({struct _dyneither_ptr _tmpE08=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmpA9C.f1=_tmpE08;});_tmpA9C;});void*_tmp8AE[2U];_tmp8AE[0]=& _tmp8B0,_tmp8AE[1]=& _tmp8B1;({unsigned int _tmpE0A=loc;struct _dyneither_ptr _tmpE09=({const char*_tmp8AF="%s: %s";_tag_dyneither(_tmp8AF,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpE0A,_tmpE09,_tag_dyneither(_tmp8AE,sizeof(void*),2U));});});}}}
# 4925
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4929
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4933
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpE0C=tvs;unsigned int _tmpE0B=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpE0C,_tmpE0B,Cyc_Absynpp_tvar2string,({const char*_tmp8B2="duplicate type variable";_tag_dyneither(_tmp8B2,sizeof(char),24U);}));});}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4947 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4952
struct _RegionHandle _tmp8B3=_new_region("temp");struct _RegionHandle*temp=& _tmp8B3;_push_region(temp);
# 4956
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpE0D=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpE0D,({const char*_tmp8B4="";_tag_dyneither(_tmp8B4,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpE0F=({struct Cyc_List_List*_tmp8B6=_region_malloc(temp,sizeof(*_tmp8B6));({struct _tuple32*_tmpE0E=({struct _tuple32*_tmp8B5=_region_malloc(temp,sizeof(*_tmp8B5));_tmp8B5->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp8B5->f2=0;_tmp8B5;});_tmp8B6->hd=_tmpE0E;}),_tmp8B6->tl=fields;_tmp8B6;});fields=_tmpE0F;});}}
# 4961
({struct Cyc_List_List*_tmpE10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpE10;});{
# 4963
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp8DB="struct";_tag_dyneither(_tmp8DB,sizeof(char),7U);}):({const char*_tmp8DC="union";_tag_dyneither(_tmp8DC,sizeof(char),6U);});
# 4966
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp8B7=(struct _tuple33*)des->hd;struct _tuple33*_tmp8B8=_tmp8B7;struct Cyc_List_List*_tmp8D3;void*_tmp8D2;_LL1: _tmp8D3=_tmp8B8->f1;_tmp8D2=_tmp8B8->f2;_LL2:;
if(_tmp8D3 == 0){
# 4971
struct Cyc_List_List*_tmp8B9=fields;
for(0;_tmp8B9 != 0;_tmp8B9=_tmp8B9->tl){
if(!(*((struct _tuple32*)_tmp8B9->hd)).f2){
(*((struct _tuple32*)_tmp8B9->hd)).f2=1;
({struct Cyc_List_List*_tmpE12=({struct Cyc_List_List*_tmp8BB=_cycalloc(sizeof(*_tmp8BB));({void*_tmpE11=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp8BA=_cycalloc(sizeof(*_tmp8BA));_tmp8BA->tag=1U,_tmp8BA->f1=((*((struct _tuple32*)_tmp8B9->hd)).f1)->name;_tmp8BA;});_tmp8BB->hd=_tmpE11;}),_tmp8BB->tl=0;_tmp8BB;});(*((struct _tuple33*)des->hd)).f1=_tmpE12;});
({struct Cyc_List_List*_tmpE14=({struct Cyc_List_List*_tmp8BD=_region_malloc(rgn,sizeof(*_tmp8BD));({struct _tuple34*_tmpE13=({struct _tuple34*_tmp8BC=_region_malloc(rgn,sizeof(*_tmp8BC));_tmp8BC->f1=(*((struct _tuple32*)_tmp8B9->hd)).f1,_tmp8BC->f2=_tmp8D2;_tmp8BC;});_tmp8BD->hd=_tmpE13;}),_tmp8BD->tl=ans;_tmp8BD;});ans=_tmpE14;});
break;}}
# 4979
if(_tmp8B9 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8C0=({struct Cyc_String_pa_PrintArg_struct _tmpA9E;_tmpA9E.tag=0U,_tmpA9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpA9E;});void*_tmp8BE[1U];_tmp8BE[0]=& _tmp8C0;({unsigned int _tmpE16=loc;struct _dyneither_ptr _tmpE15=({const char*_tmp8BF="too many arguments to %s";_tag_dyneither(_tmp8BF,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE16,_tmpE15,_tag_dyneither(_tmp8BE,sizeof(void*),1U));});});}else{
if(_tmp8D3->tl != 0)
# 4983
({void*_tmp8C1=0U;({unsigned int _tmpE18=loc;struct _dyneither_ptr _tmpE17=({const char*_tmp8C2="multiple designators are not yet supported";_tag_dyneither(_tmp8C2,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpE18,_tmpE17,_tag_dyneither(_tmp8C1,sizeof(void*),0U));});});else{
# 4986
void*_tmp8C3=(void*)_tmp8D3->hd;void*_tmp8C4=_tmp8C3;struct _dyneither_ptr*_tmp8D1;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp8C4)->tag == 0U){_LL4: _LL5:
# 4988
({struct Cyc_String_pa_PrintArg_struct _tmp8C7=({struct Cyc_String_pa_PrintArg_struct _tmpA9F;_tmpA9F.tag=0U,_tmpA9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpA9F;});void*_tmp8C5[1U];_tmp8C5[0]=& _tmp8C7;({unsigned int _tmpE1A=loc;struct _dyneither_ptr _tmpE19=({const char*_tmp8C6="array designator used in argument to %s";_tag_dyneither(_tmp8C6,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpE1A,_tmpE19,_tag_dyneither(_tmp8C5,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp8D1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp8C4)->f1;_LL7: {
# 4991
struct Cyc_List_List*_tmp8C8=fields;
for(0;_tmp8C8 != 0;_tmp8C8=_tmp8C8->tl){
if(Cyc_strptrcmp(_tmp8D1,((*((struct _tuple32*)_tmp8C8->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp8C8->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp8CB=({struct Cyc_String_pa_PrintArg_struct _tmpAA0;_tmpAA0.tag=0U,_tmpAA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8D1);_tmpAA0;});void*_tmp8C9[1U];_tmp8C9[0]=& _tmp8CB;({unsigned int _tmpE1C=loc;struct _dyneither_ptr _tmpE1B=({const char*_tmp8CA="member %s has already been used as an argument";_tag_dyneither(_tmp8CA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpE1C,_tmpE1B,_tag_dyneither(_tmp8C9,sizeof(void*),1U));});});
(*((struct _tuple32*)_tmp8C8->hd)).f2=1;
({struct Cyc_List_List*_tmpE1E=({struct Cyc_List_List*_tmp8CD=_region_malloc(rgn,sizeof(*_tmp8CD));({struct _tuple34*_tmpE1D=({struct _tuple34*_tmp8CC=_region_malloc(rgn,sizeof(*_tmp8CC));_tmp8CC->f1=(*((struct _tuple32*)_tmp8C8->hd)).f1,_tmp8CC->f2=_tmp8D2;_tmp8CC;});_tmp8CD->hd=_tmpE1D;}),_tmp8CD->tl=ans;_tmp8CD;});ans=_tmpE1E;});
break;}}
# 5000
if(_tmp8C8 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8D0=({struct Cyc_String_pa_PrintArg_struct _tmpAA1;_tmpAA1.tag=0U,_tmpAA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8D1);_tmpAA1;});void*_tmp8CE[1U];_tmp8CE[0]=& _tmp8D0;({unsigned int _tmpE20=loc;struct _dyneither_ptr _tmpE1F=({const char*_tmp8CF="bad field designator %s";_tag_dyneither(_tmp8CF,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpE20,_tmpE1F,_tag_dyneither(_tmp8CE,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 5005
if(aggr_kind == Cyc_Absyn_StructA)
# 5007
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp8D4=0U;({unsigned int _tmpE22=loc;struct _dyneither_ptr _tmpE21=({const char*_tmp8D5="too few arguments to struct";_tag_dyneither(_tmp8D5,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpE22,_tmpE21,_tag_dyneither(_tmp8D4,sizeof(void*),0U));});});
break;}}else{
# 5014
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp8D6=0U;({unsigned int _tmpE24=loc;struct _dyneither_ptr _tmpE23=({const char*_tmp8D7="only one member of a union is allowed";_tag_dyneither(_tmp8D7,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpE24,_tmpE23,_tag_dyneither(_tmp8D6,sizeof(void*),0U));});});
found=1;}}
# 5021
if(!found)({void*_tmp8D8=0U;({unsigned int _tmpE26=loc;struct _dyneither_ptr _tmpE25=({const char*_tmp8D9="missing member for union";_tag_dyneither(_tmp8D9,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpE26,_tmpE25,_tag_dyneither(_tmp8D8,sizeof(void*),0U));});});}{
# 5024
struct Cyc_List_List*_tmp8DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp8DA;};};}
# 4956
;_pop_region(temp);}
# 5027
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8DD=Cyc_Tcutil_compress(t);void*_tmp8DE=_tmp8DD;void*_tmp8E2;union Cyc_Absyn_Constraint*_tmp8E1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DE)->tag == 5U){_LL1: _tmp8E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DE)->f1).elt_typ;_tmp8E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8DE)->f1).ptr_atts).bounds;_LL2: {
# 5030
void*_tmp8DF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8E1);void*_tmp8E0=_tmp8DF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8E0)->tag == 0U){_LL6: _LL7:
# 5032
*elt_typ_dest=_tmp8E2;
return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 5036
 return 0;}_LL0:;}
# 5040
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp8E3=Cyc_Tcutil_compress(t);void*_tmp8E4=_tmp8E3;void*_tmp8E6;union Cyc_Absyn_Constraint*_tmp8E5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E4)->tag == 5U){_LL1: _tmp8E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E4)->f1).elt_typ;_tmp8E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E4)->f1).ptr_atts).zero_term;_LL2:
# 5043
*elt_typ_dest=_tmp8E6;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E5);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5051
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5053
void*_tmp8E7=Cyc_Tcutil_compress(t);void*_tmp8E8=_tmp8E7;void*_tmp8F1;struct Cyc_Absyn_Tqual _tmp8F0;struct Cyc_Absyn_Exp*_tmp8EF;union Cyc_Absyn_Constraint*_tmp8EE;void*_tmp8ED;union Cyc_Absyn_Constraint*_tmp8EC;union Cyc_Absyn_Constraint*_tmp8EB;switch(*((int*)_tmp8E8)){case 5U: _LL1: _tmp8ED=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E8)->f1).elt_typ;_tmp8EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E8)->f1).ptr_atts).bounds;_tmp8EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E8)->f1).ptr_atts).zero_term;_LL2:
# 5055
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8EB)){
*ptr_type=t;
*elt_type=_tmp8ED;
{void*_tmp8E9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp8EC);void*_tmp8EA=_tmp8E9;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8EA)->tag == 0U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 5062
return 1;}else{
return 0;}case 8U: _LL3: _tmp8F1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E8)->f1).elt_type;_tmp8F0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E8)->f1).tq;_tmp8EF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E8)->f1).num_elts;_tmp8EE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E8)->f1).zero_term;_LL4:
# 5065
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8EE)){
*elt_type=_tmp8F1;
*is_dyneither=0;
({void*_tmpE27=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);*ptr_type=_tmpE27;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5078
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5080
void*_tmp8F2=e1->r;void*_tmp8F3=_tmp8F2;struct Cyc_Absyn_Exp*_tmp905;struct Cyc_Absyn_Exp*_tmp904;struct Cyc_Absyn_Exp*_tmp903;struct Cyc_Absyn_Exp*_tmp902;struct Cyc_Absyn_Exp*_tmp901;struct Cyc_Absyn_Exp*_tmp900;switch(*((int*)_tmp8F3)){case 14U: _LL1: _LL2:
# 5082
({struct Cyc_String_pa_PrintArg_struct _tmp8F6=({struct Cyc_String_pa_PrintArg_struct _tmpAA2;_tmpAA2.tag=0U,({struct _dyneither_ptr _tmpE28=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAA2.f1=_tmpE28;});_tmpAA2;});void*_tmp8F4[1U];_tmp8F4[0]=& _tmp8F6;({struct _dyneither_ptr _tmpE29=({const char*_tmp8F5="we have a cast in a lhs:  %s";_tag_dyneither(_tmp8F5,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE29,_tag_dyneither(_tmp8F4,sizeof(void*),1U));});});case 20U: _LL3: _tmp900=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8F3)->f1;_LL4:
 _tmp901=_tmp900;goto _LL6;case 23U: _LL5: _tmp901=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8F3)->f1;_LL6:
# 5085
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp901->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp902=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8F3)->f1;_LL8:
 _tmp903=_tmp902;goto _LLA;case 21U: _LL9: _tmp903=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8F3)->f1;_LLA:
# 5089
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp903->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp8F9=({struct Cyc_String_pa_PrintArg_struct _tmpAA3;_tmpAA3.tag=0U,({
struct _dyneither_ptr _tmpE2A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAA3.f1=_tmpE2A;});_tmpAA3;});void*_tmp8F7[1U];_tmp8F7[0]=& _tmp8F9;({struct _dyneither_ptr _tmpE2B=({const char*_tmp8F8="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp8F8,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE2B,_tag_dyneither(_tmp8F7,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp904=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8F3)->f1;_LLC:
 _tmp905=_tmp904;goto _LLE;case 12U: _LLD: _tmp905=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8F3)->f1;_LLE:
# 5095
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp905->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp8FC=({struct Cyc_String_pa_PrintArg_struct _tmpAA4;_tmpAA4.tag=0U,({
struct _dyneither_ptr _tmpE2C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAA4.f1=_tmpE2C;});_tmpAA4;});void*_tmp8FA[1U];_tmp8FA[0]=& _tmp8FC;({struct _dyneither_ptr _tmpE2D=({const char*_tmp8FB="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp8FB,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE2D,_tag_dyneither(_tmp8FA,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp8FF=({struct Cyc_String_pa_PrintArg_struct _tmpAA5;_tmpAA5.tag=0U,({struct _dyneither_ptr _tmpE2E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpAA5.f1=_tmpE2E;});_tmpAA5;});void*_tmp8FD[1U];_tmp8FD[0]=& _tmp8FF;({struct _dyneither_ptr _tmpE2F=({const char*_tmp8FE="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp8FE,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE2F,_tag_dyneither(_tmp8FD,sizeof(void*),1U));});});}_LL0:;}
# 5104
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5115
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5118
void*_tmp906=Cyc_Tcutil_compress(r);void*_tmp907=_tmp906;struct Cyc_Absyn_Tvar*_tmp913;enum Cyc_Absyn_KindQual _tmp912;enum Cyc_Absyn_AliasQual _tmp911;switch(*((int*)_tmp907)){case 22U: _LL1: _LL2:
 return !must_be_unique;case 21U: _LL3: _LL4:
 return 1;case 17U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp907)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp907)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp907)->f4 == 0){_LL5: _tmp912=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp907)->f3)->kind)->v)->kind;_tmp911=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp907)->f3)->kind)->v)->aliasqual;_LL6:
# 5122
 return _tmp912 == Cyc_Absyn_RgnKind  && (_tmp911 == Cyc_Absyn_Unique  || _tmp911 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp913=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp907)->f1;_LL8: {
# 5126
struct Cyc_Absyn_Kind*_tmp908=Cyc_Tcutil_tvar_kind(_tmp913,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp909=_tmp908;enum Cyc_Absyn_KindQual _tmp910;enum Cyc_Absyn_AliasQual _tmp90F;_LLC: _tmp910=_tmp909->kind;_tmp90F=_tmp909->aliasqual;_LLD:;
if(_tmp910 == Cyc_Absyn_RgnKind  && (_tmp90F == Cyc_Absyn_Unique  || _tmp90F == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp90A=Cyc_Absyn_compress_kb(_tmp913->kind);void*_tmp90B=_tmp90A;struct Cyc_Core_Opt**_tmp90E;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90B)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90B)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90B)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp90E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90B)->f1;_LL10:
# 5130
({struct Cyc_Core_Opt*_tmpE31=({struct Cyc_Core_Opt*_tmp90D=_cycalloc(sizeof(*_tmp90D));({void*_tmpE30=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp90C=_cycalloc(sizeof(*_tmp90C));_tmp90C->tag=2U,_tmp90C->f1=0,_tmp90C->f2=& Cyc_Tcutil_rk;_tmp90C;});_tmp90D->v=_tmpE30;});_tmp90D;});*_tmp90E=_tmpE31;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5135
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5142
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp914=Cyc_Tcutil_compress(t);void*_tmp915=_tmp914;struct Cyc_Absyn_Tvar*_tmp923;void*_tmp922;switch(*((int*)_tmp915)){case 5U: _LL1: _tmp922=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp915)->f1).ptr_atts).rgn;_LL2:
# 5145
 return Cyc_Tcutil_is_noalias_region(_tmp922,must_be_unique);case 2U: _LL3: _tmp923=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp915)->f1;_LL4: {
# 5147
struct Cyc_Absyn_Kind*_tmp916=Cyc_Tcutil_tvar_kind(_tmp923,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp917=_tmp916;enum Cyc_Absyn_KindQual _tmp921;enum Cyc_Absyn_AliasQual _tmp920;_LL8: _tmp921=_tmp917->kind;_tmp920=_tmp917->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp918=_tmp921;switch(_tmp918){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if(_tmp920 == Cyc_Absyn_Unique  || _tmp920 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp919=Cyc_Absyn_compress_kb(_tmp923->kind);void*_tmp91A=_tmp919;struct Cyc_Core_Opt**_tmp91F;enum Cyc_Absyn_KindQual _tmp91E;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91A)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91A)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp91F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91A)->f1;_tmp91E=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp91A)->f2)->kind;_LL15:
# 5153
({struct Cyc_Core_Opt*_tmpE34=({struct Cyc_Core_Opt*_tmp91D=_cycalloc(sizeof(*_tmp91D));({void*_tmpE33=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp91C=_cycalloc(sizeof(*_tmp91C));_tmp91C->tag=2U,_tmp91C->f1=0,({struct Cyc_Absyn_Kind*_tmpE32=({struct Cyc_Absyn_Kind*_tmp91B=_cycalloc(sizeof(*_tmp91B));_tmp91B->kind=_tmp91E,_tmp91B->aliasqual=Cyc_Absyn_Aliasable;_tmp91B;});_tmp91C->f2=_tmpE32;});_tmp91C;});_tmp91D->v=_tmpE33;});_tmp91D;});*_tmp91F=_tmpE34;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5157
 return 1;}_LL13:;}
# 5160
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5163
 return 0;}_LL0:;}
# 5166
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp924=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp924,0))return 1;{
void*_tmp925=_tmp924;union Cyc_Absyn_DatatypeFieldInfoU _tmp939;struct Cyc_List_List*_tmp938;union Cyc_Absyn_DatatypeInfoU _tmp937;struct Cyc_List_List*_tmp936;struct Cyc_List_List*_tmp935;struct Cyc_Absyn_Aggrdecl**_tmp934;struct Cyc_List_List*_tmp933;struct Cyc_List_List*_tmp932;switch(*((int*)_tmp925)){case 10U: _LL1: _tmp932=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp925)->f1;_LL2:
# 5171
 while(_tmp932 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp932->hd)).f2))return 1;
_tmp932=_tmp932->tl;}
# 5175
return 0;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp925)->f1).aggr_info).KnownAggr).tag == 2){_LL3: _tmp934=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp925)->f1).aggr_info).KnownAggr).val;_tmp933=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp925)->f1).targs;_LL4:
# 5177
 if((*_tmp934)->impl == 0)return 0;else{
# 5179
struct Cyc_List_List*_tmp926=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp934)->tvs,_tmp933);
struct Cyc_List_List*_tmp927=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp934)->impl))->fields;
void*t;
while(_tmp927 != 0){
({void*_tmpE35=_tmp926 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp927->hd)->type: Cyc_Tcutil_substitute(_tmp926,((struct Cyc_Absyn_Aggrfield*)_tmp927->hd)->type);t=_tmpE35;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp927=_tmp927->tl;}
# 5187
return 0;}}else{_LL7: _LL8:
# 5197
 return 0;}case 12U: _LL5: _tmp935=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp925)->f2;_LL6:
# 5190
 while(_tmp935 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp935->hd)->type))return 1;
_tmp935=_tmp935->tl;}
# 5194
return 0;case 3U: _LL9: _tmp937=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp925)->f1).datatype_info;_tmp936=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp925)->f1).targs;_LLA: {
# 5199
union Cyc_Absyn_DatatypeInfoU _tmp928=_tmp937;struct Cyc_List_List*_tmp92C;struct Cyc_Core_Opt*_tmp92B;struct _tuple2*_tmp92A;int _tmp929;if((_tmp928.UnknownDatatype).tag == 1){_LL10: _tmp92A=((_tmp928.UnknownDatatype).val).name;_tmp929=((_tmp928.UnknownDatatype).val).is_extensible;_LL11:
# 5202
 return 0;}else{_LL12: _tmp92C=(*(_tmp928.KnownDatatype).val)->tvs;_tmp92B=(*(_tmp928.KnownDatatype).val)->fields;_LL13:
# 5205
 return 0;}_LLF:;}case 4U: _LLB: _tmp939=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp925)->f1).field_info;_tmp938=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp925)->f1).targs;_LLC: {
# 5208
union Cyc_Absyn_DatatypeFieldInfoU _tmp92D=_tmp939;struct Cyc_Absyn_Datatypedecl*_tmp931;struct Cyc_Absyn_Datatypefield*_tmp930;if((_tmp92D.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5211
 return 0;}else{_LL17: _tmp931=((_tmp92D.KnownDatatypefield).val).f1;_tmp930=((_tmp92D.KnownDatatypefield).val).f2;_LL18: {
# 5213
struct Cyc_List_List*_tmp92E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp931->tvs,_tmp938);
struct Cyc_List_List*_tmp92F=_tmp930->typs;
while(_tmp92F != 0){
({void*_tmpE36=_tmp92E == 0?(*((struct _tuple12*)_tmp92F->hd)).f2: Cyc_Tcutil_substitute(_tmp92E,(*((struct _tuple12*)_tmp92F->hd)).f2);_tmp924=_tmpE36;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp924))return 1;
_tmp92F=_tmp92F->tl;}
# 5220
return 0;}}_LL14:;}default: _LLD: _LLE:
# 5222
 return 0;}_LL0:;};}
# 5229
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp93A=e->r;void*_tmp93B=_tmp93A;struct Cyc_Absyn_Stmt*_tmp951;struct Cyc_Absyn_Exp*_tmp950;struct Cyc_Absyn_Exp*_tmp94F;struct Cyc_Absyn_Exp*_tmp94E;struct Cyc_Absyn_Exp*_tmp94D;struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*_tmp94B;struct Cyc_Absyn_Exp*_tmp94A;struct _dyneither_ptr*_tmp949;struct Cyc_Absyn_Exp*_tmp948;struct Cyc_Absyn_Exp*_tmp947;switch(*((int*)_tmp93B)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp93B)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp947=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp93B)->f1;_LL4:
 _tmp948=_tmp947;goto _LL6;case 20U: _LL5: _tmp948=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp93B)->f1;_LL6:
# 5234
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp948->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp948);case 21U: _LL7: _tmp94A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp93B)->f1;_tmp949=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp93B)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp94A);case 23U: _LL9: _tmp94C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp93B)->f1;_tmp94B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp93B)->f2;_LLA: {
# 5237
void*_tmp93C=Cyc_Tcutil_compress((void*)_check_null(_tmp94C->topt));void*_tmp93D=_tmp93C;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp93D)->tag == 10U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp94C);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp94E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp93B)->f2;_tmp94D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp93B)->f3;_LLC:
# 5242
 return Cyc_Tcutil_is_noalias_path(_tmp94E) && Cyc_Tcutil_is_noalias_path(_tmp94D);case 9U: _LLD: _tmp94F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp93B)->f2;_LLE:
 _tmp950=_tmp94F;goto _LL10;case 14U: _LLF: _tmp950=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93B)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp950);case 37U: _LL11: _tmp951=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp93B)->f1;_LL12:
# 5246
 while(1){
void*_tmp93E=_tmp951->r;void*_tmp93F=_tmp93E;struct Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Decl*_tmp945;struct Cyc_Absyn_Stmt*_tmp944;struct Cyc_Absyn_Stmt*_tmp943;struct Cyc_Absyn_Stmt*_tmp942;switch(*((int*)_tmp93F)){case 2U: _LL1B: _tmp943=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp93F)->f1;_tmp942=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp93F)->f2;_LL1C:
 _tmp951=_tmp942;goto _LL1A;case 12U: _LL1D: _tmp945=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp93F)->f1;_tmp944=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp93F)->f2;_LL1E:
 _tmp951=_tmp944;goto _LL1A;case 1U: _LL1F: _tmp946=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp93F)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp946);default: _LL21: _LL22:
({void*_tmp940=0U;({struct _dyneither_ptr _tmpE37=({const char*_tmp941="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp941,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE37,_tag_dyneither(_tmp940,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5254
 return 1;}_LL0:;}
# 5271 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5273
struct _tuple18 bogus_ans=({struct _tuple18 _tmpAB2;_tmpAB2.f1=0,_tmpAB2.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAB2;});
void*_tmp952=e->r;void*_tmp953=_tmp952;struct Cyc_Absyn_Exp*_tmp98C;struct Cyc_Absyn_Exp*_tmp98B;struct Cyc_Absyn_Exp*_tmp98A;struct Cyc_Absyn_Exp*_tmp989;struct _dyneither_ptr*_tmp988;int _tmp987;struct Cyc_Absyn_Exp*_tmp986;struct _dyneither_ptr*_tmp985;int _tmp984;void*_tmp983;switch(*((int*)_tmp953)){case 1U: _LL1: _tmp983=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp953)->f1;_LL2: {
# 5277
void*_tmp954=_tmp983;struct Cyc_Absyn_Vardecl*_tmp95C;struct Cyc_Absyn_Vardecl*_tmp95B;struct Cyc_Absyn_Vardecl*_tmp95A;struct Cyc_Absyn_Vardecl*_tmp959;switch(*((int*)_tmp954)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp959=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp954)->f1;_LL13: {
# 5281
void*_tmp955=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp956=_tmp955;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp956)->tag == 8U){_LL1B: _LL1C:
# 5283
 return({struct _tuple18 _tmpAA6;_tmpAA6.f1=1,_tmpAA6.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAA6;});}else{_LL1D: _LL1E:
 return({struct _tuple18 _tmpAA7;_tmpAA7.f1=(_tmp959->tq).real_const,_tmpAA7.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAA7;});}_LL1A:;}case 4U: _LL14: _tmp95A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp954)->f1;_LL15: {
# 5287
void*_tmp957=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp958=_tmp957;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp958)->tag == 8U){_LL20: _LL21:
 return({struct _tuple18 _tmpAA8;_tmpAA8.f1=1,_tmpAA8.f2=(void*)_check_null(_tmp95A->rgn);_tmpAA8;});}else{_LL22: _LL23:
# 5290
 _tmp95A->escapes=1;
return({struct _tuple18 _tmpAA9;_tmpAA9.f1=(_tmp95A->tq).real_const,_tmpAA9.f2=(void*)_check_null(_tmp95A->rgn);_tmpAA9;});}_LL1F:;}case 5U: _LL16: _tmp95B=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp954)->f1;_LL17:
# 5293
 _tmp95C=_tmp95B;goto _LL19;default: _LL18: _tmp95C=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp954)->f1;_LL19:
# 5295
 _tmp95C->escapes=1;
return({struct _tuple18 _tmpAAA;_tmpAAA.f1=(_tmp95C->tq).real_const,_tmpAAA.f2=(void*)_check_null(_tmp95C->rgn);_tmpAAA;});}_LLD:;}case 21U: _LL3: _tmp986=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp953)->f1;_tmp985=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp953)->f2;_tmp984=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp953)->f3;_LL4:
# 5300
 if(_tmp984)return bogus_ans;{
# 5303
void*_tmp95D=Cyc_Tcutil_compress((void*)_check_null(_tmp986->topt));void*_tmp95E=_tmp95D;struct Cyc_Absyn_Aggrdecl*_tmp96A;struct Cyc_List_List*_tmp969;switch(*((int*)_tmp95E)){case 12U: _LL25: _tmp969=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp95E)->f2;_LL26: {
# 5305
struct Cyc_Absyn_Aggrfield*_tmp95F=Cyc_Absyn_lookup_field(_tmp969,_tmp985);
if(_tmp95F != 0  && _tmp95F->width == 0){
struct _tuple18 _tmp960=Cyc_Tcutil_addressof_props(te,_tmp986);struct _tuple18 _tmp961=_tmp960;int _tmp963;void*_tmp962;_LL2C: _tmp963=_tmp961.f1;_tmp962=_tmp961.f2;_LL2D:;
return({struct _tuple18 _tmpAAB;_tmpAAB.f1=(_tmp95F->tq).real_const  || _tmp963,_tmpAAB.f2=_tmp962;_tmpAAB;});}
# 5310
return bogus_ans;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp95E)->f1).aggr_info).KnownAggr).tag == 2){_LL27: _tmp96A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp95E)->f1).aggr_info).KnownAggr).val;_LL28: {
# 5312
struct Cyc_Absyn_Aggrfield*_tmp964=Cyc_Absyn_lookup_decl_field(_tmp96A,_tmp985);
if(_tmp964 != 0  && _tmp964->width == 0){
struct _tuple18 _tmp965=Cyc_Tcutil_addressof_props(te,_tmp986);struct _tuple18 _tmp966=_tmp965;int _tmp968;void*_tmp967;_LL2F: _tmp968=_tmp966.f1;_tmp967=_tmp966.f2;_LL30:;
return({struct _tuple18 _tmpAAC;_tmpAAC.f1=(_tmp964->tq).real_const  || _tmp968,_tmpAAC.f2=_tmp967;_tmpAAC;});}
# 5317
return bogus_ans;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp989=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp953)->f1;_tmp988=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp953)->f2;_tmp987=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp953)->f3;_LL6:
# 5322
 if(_tmp987)return bogus_ans;{
# 5325
void*_tmp96B=Cyc_Tcutil_compress((void*)_check_null(_tmp989->topt));void*_tmp96C=_tmp96B;void*_tmp972;void*_tmp971;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96C)->tag == 5U){_LL32: _tmp972=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96C)->f1).elt_typ;_tmp971=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96C)->f1).ptr_atts).rgn;_LL33: {
# 5327
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp96D=Cyc_Tcutil_compress(_tmp972);void*_tmp96E=_tmp96D;struct Cyc_Absyn_Aggrdecl*_tmp970;struct Cyc_List_List*_tmp96F;switch(*((int*)_tmp96E)){case 12U: _LL37: _tmp96F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp96E)->f2;_LL38:
# 5330
({struct Cyc_Absyn_Aggrfield*_tmpE38=Cyc_Absyn_lookup_field(_tmp96F,_tmp988);finfo=_tmpE38;});goto _LL36;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp96E)->f1).aggr_info).KnownAggr).tag == 2){_LL39: _tmp970=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp96E)->f1).aggr_info).KnownAggr).val;_LL3A:
# 5332
({struct Cyc_Absyn_Aggrfield*_tmpE39=Cyc_Absyn_lookup_decl_field(_tmp970,_tmp988);finfo=_tmpE39;});goto _LL36;}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5335
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpAAD;_tmpAAD.f1=(finfo->tq).real_const,_tmpAAD.f2=_tmp971;_tmpAAD;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp98A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp953)->f1;_LL8: {
# 5342
void*_tmp973=Cyc_Tcutil_compress((void*)_check_null(_tmp98A->topt));void*_tmp974=_tmp973;struct Cyc_Absyn_Tqual _tmp976;void*_tmp975;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp974)->tag == 5U){_LL3E: _tmp976=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp974)->f1).elt_tq;_tmp975=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp974)->f1).ptr_atts).rgn;_LL3F:
# 5344
 return({struct _tuple18 _tmpAAE;_tmpAAE.f1=_tmp976.real_const,_tmpAAE.f2=_tmp975;_tmpAAE;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp98C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp953)->f1;_tmp98B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp953)->f2;_LLA: {
# 5350
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp98C->topt));
void*_tmp977=t;struct Cyc_Absyn_Tqual _tmp980;struct Cyc_Absyn_Tqual _tmp97F;void*_tmp97E;struct Cyc_List_List*_tmp97D;switch(*((int*)_tmp977)){case 10U: _LL43: _tmp97D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp977)->f1;_LL44: {
# 5354
struct _tuple14 _tmp978=Cyc_Evexp_eval_const_uint_exp(_tmp98B);struct _tuple14 _tmp979=_tmp978;unsigned int _tmp97C;int _tmp97B;_LL4C: _tmp97C=_tmp979.f1;_tmp97B=_tmp979.f2;_LL4D:;
if(!_tmp97B)
return bogus_ans;{
struct _tuple12*_tmp97A=Cyc_Absyn_lookup_tuple_field(_tmp97D,(int)_tmp97C);
if(_tmp97A != 0)
return({struct _tuple18 _tmpAAF;_tmpAAF.f1=((*_tmp97A).f1).real_const,({void*_tmpE3A=(Cyc_Tcutil_addressof_props(te,_tmp98C)).f2;_tmpAAF.f2=_tmpE3A;});_tmpAAF;});
return bogus_ans;};}case 5U: _LL45: _tmp97F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp977)->f1).elt_tq;_tmp97E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp977)->f1).ptr_atts).rgn;_LL46:
# 5362
 return({struct _tuple18 _tmpAB0;_tmpAB0.f1=_tmp97F.real_const,_tmpAB0.f2=_tmp97E;_tmpAB0;});case 8U: _LL47: _tmp980=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp977)->f1).tq;_LL48:
# 5368
 return({struct _tuple18 _tmpAB1;_tmpAB1.f1=_tmp980.real_const,({void*_tmpE3B=(Cyc_Tcutil_addressof_props(te,_tmp98C)).f2;_tmpAB1.f2=_tmpE3B;});_tmpAB1;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5372
({void*_tmp981=0U;({unsigned int _tmpE3D=e->loc;struct _dyneither_ptr _tmpE3C=({const char*_tmp982="unary & applied to non-lvalue";_tag_dyneither(_tmp982,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpE3D,_tmpE3C,_tag_dyneither(_tmp981,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5379
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
({union Cyc_Absyn_Constraint*_tmpE3E=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);b=_tmpE3E;});{
void*_tmp98D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmp98E=_tmp98D;struct Cyc_Absyn_Exp*_tmp997;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp98E)->tag == 0U){_LL1: _LL2:
 return;}else{_LL3: _tmp997=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp98E)->f1;_LL4: {
# 5384
struct _tuple14 _tmp98F=Cyc_Evexp_eval_const_uint_exp(_tmp997);struct _tuple14 _tmp990=_tmp98F;unsigned int _tmp996;int _tmp995;_LL6: _tmp996=_tmp990.f1;_tmp995=_tmp990.f2;_LL7:;
if(_tmp995  && _tmp996 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmp993=({struct Cyc_Int_pa_PrintArg_struct _tmpAB4;_tmpAB4.tag=1U,_tmpAB4.f1=(unsigned long)((int)_tmp996);_tmpAB4;});struct Cyc_Int_pa_PrintArg_struct _tmp994=({struct Cyc_Int_pa_PrintArg_struct _tmpAB3;_tmpAB3.tag=1U,_tmpAB3.f1=(unsigned long)((int)i);_tmpAB3;});void*_tmp991[2U];_tmp991[0]=& _tmp993,_tmp991[1]=& _tmp994;({unsigned int _tmpE40=loc;struct _dyneither_ptr _tmpE3F=({const char*_tmp992="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp992,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE40,_tmpE3F,_tag_dyneither(_tmp991,sizeof(void*),2U));});});
return;}}_LL0:;};}
# 5391
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0U,b);}
# 5395
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmp998=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmp999=_tmp998;struct Cyc_Absyn_Exp*_tmp99E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp999)->tag == 1U){_LL1: _tmp99E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp999)->f1;_LL2: {
# 5398
struct _tuple14 _tmp99A=Cyc_Evexp_eval_const_uint_exp(_tmp99E);struct _tuple14 _tmp99B=_tmp99A;unsigned int _tmp99D;int _tmp99C;_LL6: _tmp99D=_tmp99B.f1;_tmp99C=_tmp99B.f2;_LL7:;
return _tmp99C  && _tmp99D == 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5404
int Cyc_Tcutil_bits_only(void*t){
void*_tmp99F=Cyc_Tcutil_compress(t);void*_tmp9A0=_tmp99F;struct Cyc_List_List*_tmp9AA;struct Cyc_Absyn_Aggrdecl*_tmp9A9;struct Cyc_List_List*_tmp9A8;struct Cyc_List_List*_tmp9A7;void*_tmp9A6;union Cyc_Absyn_Constraint*_tmp9A5;switch(*((int*)_tmp9A0)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 return 0;case 8U: _LLB: _tmp9A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A0)->f1).elt_type;_tmp9A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A0)->f1).zero_term;_LLC:
# 5414
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9A5) && Cyc_Tcutil_bits_only(_tmp9A6);case 10U: _LLD: _tmp9A7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9A0)->f1;_LLE:
# 5416
 for(0;_tmp9A7 != 0;_tmp9A7=_tmp9A7->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmp9A7->hd)).f2))return 0;}
return 1;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9A0)->f1).aggr_info).UnknownAggr).tag == 1){_LLF: _LL10:
 return 0;}else{_LL11: _tmp9A9=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9A0)->f1).aggr_info).KnownAggr).val;_tmp9A8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9A0)->f1).targs;_LL12:
# 5421
 if(_tmp9A9->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9A9->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp9A1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9A1;_push_region(rgn);
{struct Cyc_List_List*_tmp9A2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp9A9->tvs,_tmp9A8);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9A9->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp9A2,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp9A3=0;_npop_handler(0U);return _tmp9A3;}}}{
int _tmp9A4=1;_npop_handler(0U);return _tmp9A4;};}
# 5428
;_pop_region(rgn);};};}case 12U: _LL13: _tmp9AA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9A0)->f2;_LL14:
# 5433
 for(0;_tmp9AA != 0;_tmp9AA=_tmp9AA->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmp9AA->hd)->type))return 0;}
return 1;default: _LL15: _LL16:
 return 0;}_LL0:;}
# 5441
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp9AB=e->r;void*_tmp9AC=_tmp9AB;struct Cyc_Absyn_Exp*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9AD;switch(*((int*)_tmp9AC)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp9AD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9AC)->f1;_LL4:
 _tmp9AE=_tmp9AD;goto _LL6;case 13U: _LL5: _tmp9AE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9AC)->f1;_LL6:
 e=_tmp9AE;continue;default: _LL7: _LL8:
# 5448
 return 0;}_LL0:;}}
# 5458
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp9AF=e->r;void*_tmp9B0=_tmp9AF;struct Cyc_List_List*_tmp9CE;struct Cyc_List_List*_tmp9CD;enum Cyc_Absyn_Primop _tmp9CC;struct Cyc_List_List*_tmp9CB;struct Cyc_List_List*_tmp9CA;struct Cyc_List_List*_tmp9C9;struct Cyc_List_List*_tmp9C8;struct Cyc_Absyn_Exp*_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C6;struct Cyc_Absyn_Exp*_tmp9C5;struct Cyc_Absyn_Exp*_tmp9C4;void*_tmp9C3;struct Cyc_Absyn_Exp*_tmp9C2;void*_tmp9C1;struct Cyc_Absyn_Exp*_tmp9C0;struct Cyc_Absyn_Exp*_tmp9BF;struct Cyc_Absyn_Exp*_tmp9BE;struct Cyc_Absyn_Exp*_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BC;struct Cyc_Absyn_Exp*_tmp9BB;struct Cyc_Absyn_Exp*_tmp9BA;struct Cyc_Absyn_Exp*_tmp9B9;void*_tmp9B8;switch(*((int*)_tmp9B0)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp9B8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL10: {
# 5470
void*_tmp9B1=_tmp9B8;struct Cyc_Absyn_Vardecl*_tmp9B7;struct Cyc_Absyn_Vardecl*_tmp9B6;switch(*((int*)_tmp9B1)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmp9B6=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp9B1)->f1;_LL35: {
# 5473
void*_tmp9B2=Cyc_Tcutil_compress(_tmp9B6->type);void*_tmp9B3=_tmp9B2;switch(*((int*)_tmp9B3)){case 8U: _LL3D: _LL3E:
 goto _LL40;case 9U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmp9B7=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp9B1)->f1;_LL37:
# 5480
 if(_tmp9B7->sc == Cyc_Absyn_Static){
void*_tmp9B4=Cyc_Tcutil_compress(_tmp9B7->type);void*_tmp9B5=_tmp9B4;switch(*((int*)_tmp9B5)){case 8U: _LL44: _LL45:
 goto _LL47;case 9U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}else{
# 5487
return var_okay;}case 0U: _LL38: _LL39:
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmp9BB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_tmp9BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B0)->f2;_tmp9B9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9B0)->f3;_LL12:
# 5493
 return(Cyc_Tcutil_cnst_exp(0,_tmp9BB) && 
Cyc_Tcutil_cnst_exp(0,_tmp9BA)) && 
Cyc_Tcutil_cnst_exp(0,_tmp9B9);case 9U: _LL13: _tmp9BD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_tmp9BC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9B0)->f2;_LL14:
# 5497
 return Cyc_Tcutil_cnst_exp(0,_tmp9BD) && Cyc_Tcutil_cnst_exp(0,_tmp9BC);case 12U: _LL15: _tmp9BE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL16:
 _tmp9BF=_tmp9BE;goto _LL18;case 13U: _LL17: _tmp9BF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL18:
# 5500
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9BF);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B0)->f4 == Cyc_Absyn_No_coercion){_LL19: _tmp9C1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_tmp9C0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B0)->f2;_LL1A:
# 5502
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9C0);}else{_LL1B: _tmp9C3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_tmp9C2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9B0)->f2;_LL1C:
# 5505
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp9C2);}case 15U: _LL1D: _tmp9C4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL1E:
# 5507
 return Cyc_Tcutil_cnst_exp(1,_tmp9C4);case 27U: _LL1F: _tmp9C6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B0)->f2;_tmp9C5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9B0)->f3;_LL20:
# 5509
 return Cyc_Tcutil_cnst_exp(0,_tmp9C6) && Cyc_Tcutil_cnst_exp(0,_tmp9C5);case 28U: _LL21: _tmp9C7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL22:
# 5511
 return Cyc_Tcutil_cnst_exp(0,_tmp9C7);case 26U: _LL23: _tmp9C8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL24:
 _tmp9C9=_tmp9C8;goto _LL26;case 30U: _LL25: _tmp9C9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9B0)->f2;_LL26:
 _tmp9CA=_tmp9C9;goto _LL28;case 29U: _LL27: _tmp9CA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9B0)->f3;_LL28:
# 5515
 for(0;_tmp9CA != 0;_tmp9CA=_tmp9CA->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmp9CA->hd)).f2))
return 0;}
return 1;case 3U: _LL29: _tmp9CC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_tmp9CB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9B0)->f2;_LL2A:
# 5520
 _tmp9CD=_tmp9CB;goto _LL2C;case 24U: _LL2B: _tmp9CD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL2C:
 _tmp9CE=_tmp9CD;goto _LL2E;case 31U: _LL2D: _tmp9CE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9B0)->f1;_LL2E:
# 5523
 for(0;_tmp9CE != 0;_tmp9CE=_tmp9CE->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp9CE->hd))
return 0;}
return 1;default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 5531
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5535
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5537
int Cyc_Tcutil_supports_default(void*t){
void*_tmp9CF=Cyc_Tcutil_compress(t);void*_tmp9D0=_tmp9CF;struct Cyc_List_List*_tmp9DA;union Cyc_Absyn_AggrInfoU _tmp9D9;struct Cyc_List_List*_tmp9D8;struct Cyc_List_List*_tmp9D7;void*_tmp9D6;union Cyc_Absyn_Constraint*_tmp9D5;union Cyc_Absyn_Constraint*_tmp9D4;switch(*((int*)_tmp9D0)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 5U: _LL7: _tmp9D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9D0)->f1).ptr_atts).nullable;_tmp9D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9D0)->f1).ptr_atts).bounds;_LL8: {
# 5544
void*_tmp9D1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9D4);void*_tmp9D2=_tmp9D1;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9D2)->tag == 0U){_LL18: _LL19:
 return 1;}else{_LL1A: _LL1B:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp9D5);}_LL17:;}case 8U: _LL9: _tmp9D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9D0)->f1).elt_type;_LLA:
# 5549
 return Cyc_Tcutil_supports_default(_tmp9D6);case 10U: _LLB: _tmp9D7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9D0)->f1;_LLC:
# 5551
 for(0;_tmp9D7 != 0;_tmp9D7=_tmp9D7->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp9D7->hd)).f2))return 0;}
return 1;case 11U: _LLD: _tmp9D9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9D0)->f1).aggr_info;_tmp9D8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9D0)->f1).targs;_LLE: {
# 5555
struct Cyc_Absyn_Aggrdecl*_tmp9D3=Cyc_Absyn_get_known_aggrdecl(_tmp9D9);
if(_tmp9D3->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9D3->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmp9D3->tvs,_tmp9D8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp9D3->impl))->fields);}case 12U: _LLF: _tmp9DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9D0)->f2;_LL10:
 return Cyc_Tcutil_fields_support_default(0,0,_tmp9DA);case 14U: _LL11: _LL12:
# 5561
 goto _LL14;case 13U: _LL13: _LL14:
 return 1;default: _LL15: _LL16:
# 5564
 return 0;}_LL0:;}
# 5569
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp9DB=t;struct Cyc_Absyn_Typedefdecl*_tmp9E0;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9DB)->tag == 17U){_LL1: _tmp9E0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9DB)->f3;_LL2:
# 5572
 if(_tmp9E0 != 0){
struct Cyc_Absyn_Tqual _tmp9DC=_tmp9E0->tq;
if(((_tmp9DC.print_const  || _tmp9DC.q_volatile) || _tmp9DC.q_restrict) || _tmp9DC.real_const)
# 5577
({struct Cyc_String_pa_PrintArg_struct _tmp9DF=({struct Cyc_String_pa_PrintArg_struct _tmpAB5;_tmpAB5.tag=0U,({struct _dyneither_ptr _tmpE41=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAB5.f1=_tmpE41;});_tmpAB5;});void*_tmp9DD[1U];_tmp9DD[0]=& _tmp9DF;({unsigned int _tmpE43=loc;struct _dyneither_ptr _tmpE42=({const char*_tmp9DE="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp9DE,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpE43,_tmpE42,_tag_dyneither(_tmp9DD,sizeof(void*),1U));});});}
# 5580
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5585
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5587
struct _RegionHandle _tmp9E1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp9E1;_push_region(rgn);
{struct Cyc_List_List*_tmp9E2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmp9E3=1;_npop_handler(0U);return _tmp9E3;}
({void*_tmpE44=Cyc_Tcutil_rsubstitute(rgn,_tmp9E2,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpE44;});
if(!Cyc_Tcutil_supports_default(t)){int _tmp9E4=0;_npop_handler(0U);return _tmp9E4;}}{
# 5595
int _tmp9E5=1;_npop_handler(0U);return _tmp9E5;};}
# 5588
;_pop_region(rgn);}
# 5601
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp9E6=Cyc_Tcutil_compress(t);void*_tmp9E7=_tmp9E6;union Cyc_Absyn_Constraint*_tmp9EB;union Cyc_Absyn_Constraint*_tmp9EA;switch(*((int*)_tmp9E7)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;case 5U: _LL5: _tmp9EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E7)->f1).ptr_atts).nullable;_tmp9EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E7)->f1).ptr_atts).bounds;_LL6: {
# 5606
void*_tmp9E8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9EA);void*_tmp9E9=_tmp9E8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E9)->tag == 0U){_LLA: _LLB:
# 5610
 return 0;}else{_LLC: _LLD:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9EB);}_LL9:;}default: _LL7: _LL8:
# 5613
 return 0;}_LL0:;}
# 5617
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmp9EC=Cyc_Tcutil_compress(t);void*_tmp9ED=_tmp9EC;struct Cyc_List_List*_tmp9F1;void*_tmp9F0;switch(*((int*)_tmp9ED)){case 5U: _LL1: _tmp9F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9ED)->f1).elt_typ;_LL2:
 return Cyc_Tcutil_is_noreturn(_tmp9F0);case 9U: _LL3: _tmp9F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9ED)->f1).attributes;_LL4:
# 5621
 for(0;_tmp9F1 != 0;_tmp9F1=_tmp9F1->tl){
void*_tmp9EE=(void*)_tmp9F1->hd;void*_tmp9EF=_tmp9EE;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp9EF)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 5626
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5629
return 0;}
# 5634
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp9F2=Cyc_Tcutil_compress(t);void*_tmp9F3=_tmp9F2;struct Cyc_List_List**_tmp9F9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9F3)->tag == 9U){_LL1: _tmp9F9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9F3)->f1).attributes;_LL2: {
# 5637
struct Cyc_List_List*_tmp9F4=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmp9F9,(void*)atts->hd))
({struct Cyc_List_List*_tmpE45=({struct Cyc_List_List*_tmp9F5=_cycalloc(sizeof(*_tmp9F5));_tmp9F5->hd=(void*)atts->hd,_tmp9F5->tl=*_tmp9F9;_tmp9F5;});*_tmp9F9=_tmpE45;});}else{
# 5644
({struct Cyc_List_List*_tmpE46=({struct Cyc_List_List*_tmp9F6=_cycalloc(sizeof(*_tmp9F6));_tmp9F6->hd=(void*)atts->hd,_tmp9F6->tl=_tmp9F4;_tmp9F6;});_tmp9F4=_tmpE46;});}}
return _tmp9F4;}}else{_LL3: _LL4:
({void*_tmp9F7=0U;({struct _dyneither_ptr _tmpE47=({const char*_tmp9F8="transfer_fn_type_atts";_tag_dyneither(_tmp9F8,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE47,_tag_dyneither(_tmp9F7,sizeof(void*),0U));});});}_LL0:;}
# 5651
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmp9FA=Cyc_Tcutil_compress(t);void*_tmp9FB=_tmp9FA;struct Cyc_Absyn_Exp*_tmpA00;struct Cyc_Absyn_PtrInfo*_tmp9FF;switch(*((int*)_tmp9FB)){case 5U: _LL1: _tmp9FF=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9FB)->f1;_LL2:
# 5655
{void*_tmp9FC=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmp9FF->ptr_atts).bounds);void*_tmp9FD=_tmp9FC;struct Cyc_Absyn_Exp*_tmp9FE;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9FD)->tag == 1U){_LL8: _tmp9FE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9FD)->f1;_LL9:
 bound=_tmp9FE;goto _LL7;}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 5659
goto _LL0;case 8U: _LL3: _tmpA00=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9FB)->f1).num_elts;_LL4:
# 5661
 bound=_tmpA00;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5665
return bound;}
# 5670
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpA01=b;struct Cyc_Absyn_Vardecl*_tmpA05;struct Cyc_Absyn_Vardecl*_tmpA04;struct Cyc_Absyn_Vardecl*_tmpA03;struct Cyc_Absyn_Vardecl*_tmpA02;switch(*((int*)_tmpA01)){case 5U: _LL1: _tmpA02=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA01)->f1;_LL2:
 _tmpA03=_tmpA02;goto _LL4;case 4U: _LL3: _tmpA03=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA01)->f1;_LL4:
 _tmpA04=_tmpA03;goto _LL6;case 3U: _LL5: _tmpA04=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA01)->f1;_LL6:
 _tmpA05=_tmpA04;goto _LL8;case 1U: _LL7: _tmpA05=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA01)->f1;_LL8:
# 5676
 if(!_tmpA05->escapes)return _tmpA05;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5680
return 0;}
# 5684
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpA06=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpE48=({struct Cyc_List_List*_tmpA07=_cycalloc(sizeof(*_tmpA07));_tmpA07->hd=*((void**)_check_null((void**)x->hd)),_tmpA07->tl=_tmpA06;_tmpA07;});_tmpA06=_tmpE48;});}}
return _tmpA06;}
# 5691
int Cyc_Tcutil_is_array(void*t){
void*_tmpA08=Cyc_Tcutil_compress(t);void*_tmpA09=_tmpA08;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA09)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5698
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpA0A=Cyc_Tcutil_compress(t);void*_tmpA0B=_tmpA0A;void*_tmpA18;struct Cyc_Absyn_Tqual _tmpA17;struct Cyc_Absyn_Exp*_tmpA16;union Cyc_Absyn_Constraint*_tmpA15;unsigned int _tmpA14;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0B)->tag == 8U){_LL1: _tmpA18=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0B)->f1).elt_type;_tmpA17=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0B)->f1).tq;_tmpA16=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0B)->f1).num_elts;_tmpA15=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0B)->f1).zero_term;_tmpA14=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA0B)->f1).zt_loc;_LL2: {
# 5701
void*b;
if(_tmpA16 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5705
struct Cyc_Absyn_Exp*bnd=_tmpA16;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpA0C=0U;({struct _dyneither_ptr _tmpE49=({const char*_tmpA0D="cannot convert tag without type!";_tag_dyneither(_tmpA0D,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE49,_tag_dyneither(_tmpA0C,sizeof(void*),0U));});});{
void*_tmpA0E=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpA0F=_tmpA0E;void*_tmpA12;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA0F)->tag == 19U){_LL6: _tmpA12=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpA0F)->f1;_LL7:
# 5711
({void*_tmpE4C=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA10=_cycalloc(sizeof(*_tmpA10));_tmpA10->tag=1U,({struct Cyc_Absyn_Exp*_tmpE4B=({void*_tmpE4A=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmpE4A,Cyc_Absyn_valueof_exp(_tmpA12,0U),0,Cyc_Absyn_No_coercion,0U);});_tmpA10->f1=_tmpE4B;});_tmpA10;});b=_tmpE4C;});
goto _LL5;}else{_LL8: _LL9:
# 5714
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpE4D=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA11=_cycalloc(sizeof(*_tmpA11));_tmpA11->tag=1U,_tmpA11->f1=bnd;_tmpA11;});b=_tmpE4D;});else{
# 5717
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL5:;};}else{
# 5721
({void*_tmpE4E=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA13=_cycalloc(sizeof(*_tmpA13));_tmpA13->tag=1U,_tmpA13->f1=bnd;_tmpA13;});b=_tmpE4E;});}}
# 5723
return Cyc_Absyn_atb_typ(_tmpA18,rgn,_tmpA17,b,_tmpA15);}}else{_LL3: _LL4:
# 5726
 return t;}_LL0:;}
# 5731
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpA19=Cyc_Tcutil_compress(t);void*_tmpA1A=_tmpA19;struct Cyc_List_List*_tmpA28;union Cyc_Absyn_AggrInfoU _tmpA27;struct Cyc_List_List*_tmpA26;struct Cyc_List_List*_tmpA25;void*_tmpA24;union Cyc_Absyn_Constraint*_tmpA23;switch(*((int*)_tmpA1A)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 return 0;case 5U: _LLB: _tmpA23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1A)->f1).ptr_atts).nullable;_LLC:
# 5739
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpA23);case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmpA24=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1A)->f1).elt_type;_LL12:
 return Cyc_Tcutil_zeroable_type(_tmpA24);case 9U: _LL13: _LL14:
 return 0;case 10U: _LL15: _tmpA25=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA1A)->f1;_LL16:
# 5745
 for(0;(unsigned int)_tmpA25;_tmpA25=_tmpA25->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpA25->hd)).f2))return 0;}
return 1;case 11U: _LL17: _tmpA27=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA1A)->f1).aggr_info;_tmpA26=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA1A)->f1).targs;_LL18: {
# 5749
struct Cyc_Absyn_Aggrdecl*_tmpA1B=Cyc_Absyn_get_known_aggrdecl(_tmpA27);
if(_tmpA1B->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA1B->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpA1C=_new_region("r");struct _RegionHandle*r=& _tmpA1C;_push_region(r);
{struct Cyc_List_List*_tmpA1D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpA1B->tvs,_tmpA26);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA1B->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpA1D,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA1E=0;_npop_handler(0U);return _tmpA1E;}}}{
int _tmpA1F=1;_npop_handler(0U);return _tmpA1F;};}
# 5753
;_pop_region(r);};}case 13U: _LL19: _LL1A:
# 5757
 goto _LL1C;case 19U: _LL1B: _LL1C:
 return 1;case 12U: _LL1D: _tmpA28=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA1A)->f2;_LL1E:
# 5760
 for(0;_tmpA28 != 0;_tmpA28=_tmpA28->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpA28->hd)->type))return 0;}
return 1;case 14U: _LL1F: _LL20:
 return 1;case 17U: _LL21: _LL22:
 goto _LL24;case 16U: _LL23: _LL24:
 goto _LL26;case 15U: _LL25: _LL26:
 goto _LL28;case 28U: _LL27: _LL28:
 return 0;case 26U: _LL29: _LL2A:
# 5769
 goto _LL2C;case 27U: _LL2B: _LL2C:
 goto _LL2E;case 18U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 21U: _LL31: _LL32:
 goto _LL34;case 22U: _LL33: _LL34:
 goto _LL36;case 23U: _LL35: _LL36:
 goto _LL38;case 24U: _LL37: _LL38:
 goto _LL3A;default: _LL39: _LL3A:
({struct Cyc_String_pa_PrintArg_struct _tmpA22=({struct Cyc_String_pa_PrintArg_struct _tmpAB6;_tmpAB6.tag=0U,({struct _dyneither_ptr _tmpE4F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAB6.f1=_tmpE4F;});_tmpAB6;});void*_tmpA20[1U];_tmpA20[0]=& _tmpA22;({struct _dyneither_ptr _tmpE50=({const char*_tmpA21="bad type `%s' in zeroable type";_tag_dyneither(_tmpA21,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE50,_tag_dyneither(_tmpA20,sizeof(void*),1U));});});}_LL0:;}
