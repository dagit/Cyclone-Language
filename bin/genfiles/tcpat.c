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
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base ? (_tag_arr_ans.base + (elt_sz) * (num_elts)) : 0; \
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
  unsigned delta = sz * numelts;
  /* Don't let someone decrease the size so much that it wraps around.
   * This is crucial to get NULL right. */
  if (x.last_plus_one - x.base >= delta)
    x.last_plus_one -= delta;
  else x.last_plus_one = x.base;
  return x; 
}

/* Allocation */
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);
/* bound the allocation size to be less than MAX_ALLOC_SIZE,
   which is defined in runtime_memory.c
*/
extern void* _bounded_GC_malloc(int,const char *file,int lineno);
extern void* _bounded_GC_malloc_atomic(int,const char *file,int lineno);
extern void* _bounded_GC_calloc(unsigned n, unsigned s,
                                const char *file,int lineno);
extern void* _bounded_GC_calloc_atomic(unsigned n, unsigned s,
                                       const char *file,
                                       int lineno);
/* FIX?  Not sure if we want to pass filename and lineno in here... */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)


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
# 159
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 180
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 185
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 213
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 215
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 303
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 325
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 899
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 907
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 912
void*Cyc_Absyn_compress_kb(void*);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 925
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 928
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 930
extern void*Cyc_Absyn_sint_type;
# 933
void*Cyc_Absyn_gen_float_type(unsigned int i);
# 935
extern void*Cyc_Absyn_unique_rgn_type;
# 937
extern void*Cyc_Absyn_empty_effect;
# 939
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 970
void*Cyc_Absyn_bounds_one();
# 1001
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
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
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 46
int Cyc_Tcutil_is_arithmetic_type(void*);
# 49
int Cyc_Tcutil_is_array_type(void*t);
# 104
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 107
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 112
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
# 114
void*Cyc_Tcutil_compress(void*t);
# 117
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 128
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 153
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 160
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 163
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 169
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 196
int Cyc_Tcutil_unify(void*,void*);
# 201
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 203
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 230
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 272 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 275
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 288
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 301
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 309
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 314
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 329
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 344
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 380
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 387
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 390
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 392
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**te);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
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
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Aggrdecl**_tmp2A;struct Cyc_List_List**_tmp29;struct Cyc_List_List*_tmp28;int _tmp27;struct Cyc_List_List*_tmp26;struct Cyc_List_List*_tmp25;int _tmp24;switch(*((int*)_tmp1)){case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp26=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp25=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp24=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2:
# 48
 if(topt == 0)
({void*_tmp2=0U;({unsigned int _tmp3D6=p->loc;struct _dyneither_ptr _tmp3D5=({const char*_tmp3="cannot determine pattern type";_tag_dyneither(_tmp3,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp3D6,_tmp3D5,_tag_dyneither(_tmp2,sizeof(void*),0U));});});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp4=t;union Cyc_Absyn_AggrInfo _tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->f1)->tag == 20U){_LL10: _tmpA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4)->f1)->f1;_LL11:
# 53
({void*_tmp3D8=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp3D7=({union Cyc_Absyn_AggrInfo*_tmp5=_cycalloc(sizeof(*_tmp5));*_tmp5=_tmpA;_tmp5;});_tmp6->f1=_tmp3D7;}),_tmp6->f2=_tmp26,_tmp6->f3=_tmp25,_tmp6->f4=_tmp24;_tmp6;});p->r=_tmp3D8;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}else{goto _LL12;}}else{_LL12: _LL13:
# 57
({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp384;_tmp384.tag=0U,({struct _dyneither_ptr _tmp3D9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp384.f1=_tmp3D9;});_tmp384;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({unsigned int _tmp3DB=p->loc;struct _dyneither_ptr _tmp3DA=({const char*_tmp8="pattern expects aggregate type instead of %s";_tag_dyneither(_tmp8,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp3DB,_tmp3DA,_tag_dyneither(_tmp7,sizeof(void*),1U));});});
goto _LLF;}_LLF:;}
# 60
return;};}else{if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).tag == 2){_LL3: _tmp2A=((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->KnownAggr).val;_tmp29=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp28=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp27=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {
# 63
struct Cyc_Absyn_Aggrdecl*_tmpB=*_tmp2A;
if(_tmpB->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0U,({
struct _dyneither_ptr _tmp3DC=(struct _dyneither_ptr)(_tmpB->kind == Cyc_Absyn_StructA?({const char*_tmpF="struct";_tag_dyneither(_tmpF,sizeof(char),7U);}):({const char*_tmp10="union";_tag_dyneither(_tmp10,sizeof(char),6U);}));_tmp385.f1=_tmp3DC;});_tmp385;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned int _tmp3DE=p->loc;struct _dyneither_ptr _tmp3DD=({const char*_tmpD="can't destructure an abstract %s";_tag_dyneither(_tmpD,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp3DE,_tmp3DD,_tag_dyneither(_tmpC,sizeof(void*),1U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 70
int more_exists=({int _tmp3DF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);_tmp3DF - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp29);});
if(more_exists < 0){
({void*_tmp11=0U;({unsigned int _tmp3E1=p->loc;struct _dyneither_ptr _tmp3E0=({const char*_tmp12="too many existentially bound type variables in pattern";_tag_dyneither(_tmp12,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp3E1,_tmp3E0,_tag_dyneither(_tmp11,sizeof(void*),0U));});});{
struct Cyc_List_List**_tmp13=_tmp29;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB->impl))->exist_vars);for(0;n != 0;-- n){
_tmp13=&((struct Cyc_List_List*)_check_null(*_tmp13))->tl;}}
*_tmp13=0;};}else{
if(more_exists > 0){
# 79
struct Cyc_List_List*_tmp14=0;
for(0;more_exists != 0;-- more_exists){
({struct Cyc_List_List*_tmp3E3=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Absyn_Tvar*_tmp3E2=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=1U,_tmp15->f1=0;_tmp15;}));_tmp16->hd=_tmp3E2;}),_tmp16->tl=_tmp14;_tmp16;});_tmp14=_tmp3E3;});}
({struct Cyc_List_List*_tmp3E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp29,_tmp14);*_tmp29=_tmp3E4;});}}
# 84
return;};}}else{_LLB: _LLC:
# 97
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp3E5=({const char*_tmp22="resolve_pat unknownAggr";_tag_dyneither(_tmp22,sizeof(char),24U);});_tmp23->f1=_tmp3E5;});_tmp23;}));}}case 17U: _LL5: _tmp2B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6:
# 86
 Cyc_Tcexp_tcExp(te,0,_tmp2B);
if(!Cyc_Tcutil_is_const_exp(_tmp2B)){
({void*_tmp17=0U;({unsigned int _tmp3E7=p->loc;struct _dyneither_ptr _tmp3E6=({const char*_tmp18="non-constant expression in case pattern";_tag_dyneither(_tmp18,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp3E7,_tmp3E6,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 91
struct _tuple14 _tmp19=Cyc_Evexp_eval_const_uint_exp(_tmp2B);struct _tuple14 _tmp1A=_tmp19;unsigned int _tmp1D;int _tmp1C;_LL15: _tmp1D=_tmp1A.f1;_tmp1C=_tmp1A.f2;_LL16:;
({void*_tmp3E8=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=10U,_tmp1B->f1=Cyc_Absyn_None,_tmp1B->f2=(int)_tmp1D;_tmp1B;});p->r=_tmp3E8;});
return;};case 15U: _LL7: _LL8:
# 95
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp3E9=({const char*_tmp1E="resolve_pat UnknownId_p";_tag_dyneither(_tmp1E,sizeof(char),24U);});_tmp1F->f1=_tmp3E9;});_tmp1F;}));case 16U: _LL9: _LLA:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp3EA=({const char*_tmp20="resolve_pat UnknownCall_p";_tag_dyneither(_tmp20,sizeof(char),26U);});_tmp21->f1=_tmp3EA;});_tmp21;}));default: _LLD: _LLE:
# 99
 return;}_LL0:;}
# 103
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 106
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->v=s;_tmp2C;}));}
# 111
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 115
if(topt != 0  && Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;
# 118
{void*_tmp2D=Cyc_Tcutil_compress(numt);void*_tmp2E=_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->f1)){case 15U: _LL1: _LL2:
 goto _LL4;case 16U: _LL3: _LL4:
 if(topt != 0)return*topt;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}
# 123
return numt;}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 126
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 128
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp3EB=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_tmp3EB;});}
# 132
({struct Cyc_List_List*_tmp3ED=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _tuple15*_tmp3EC=({struct _tuple15*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->f1=vd,_tmp2F->f2=e;_tmp2F;});_tmp30->hd=_tmp3EC;}),_tmp30->tl=*v_result_ptr;_tmp30;});*v_result_ptr=_tmp3ED;});}
# 134
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 136
struct Cyc_Tcpat_TcPatResult _tmp31=res1;struct _tuple1*_tmp39;struct Cyc_List_List*_tmp38;_LL1: _tmp39=_tmp31.tvars_and_bounds_opt;_tmp38=_tmp31.patvars;_LL2:;{
struct Cyc_Tcpat_TcPatResult _tmp32=res2;struct _tuple1*_tmp37;struct Cyc_List_List*_tmp36;_LL4: _tmp37=_tmp32.tvars_and_bounds_opt;_tmp36=_tmp32.patvars;_LL5:;
if(_tmp39 != 0  || _tmp37 != 0){
if(_tmp39 == 0)
({struct _tuple1*_tmp3EE=({struct _tuple1*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=0,_tmp33->f2=0;_tmp33;});_tmp39=_tmp3EE;});
if(_tmp37 == 0)
({struct _tuple1*_tmp3EF=({struct _tuple1*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->f1=0,_tmp34->f2=0;_tmp34;});_tmp37=_tmp3EF;});
return({struct Cyc_Tcpat_TcPatResult _tmp386;({struct _tuple1*_tmp3F3=({struct _tuple1*_tmp35=_cycalloc(sizeof(*_tmp35));({struct Cyc_List_List*_tmp3F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp39).f1,(*_tmp37).f1);_tmp35->f1=_tmp3F2;}),({
struct Cyc_List_List*_tmp3F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp39).f2,(*_tmp37).f2);_tmp35->f2=_tmp3F1;});_tmp35;});
# 143
_tmp386.tvars_and_bounds_opt=_tmp3F3;}),({
# 145
struct Cyc_List_List*_tmp3F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38,_tmp36);_tmp386.patvars=_tmp3F0;});_tmp386;});}
# 147
return({struct Cyc_Tcpat_TcPatResult _tmp387;_tmp387.tvars_and_bounds_opt=0,({struct Cyc_List_List*_tmp3F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38,_tmp36);_tmp387.patvars=_tmp3F4;});_tmp387;});};}
# 150
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
return({struct Cyc_Absyn_Pat*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->loc=loc,_tmp3A->topt=0,_tmp3A->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp3A;});}
# 155
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp3F6=t;Cyc_Tcutil_promote_array(_tmp3F6,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp3B=_cycalloc(sizeof(*_tmp3B));({struct Cyc_List_List*_tmp3F5=Cyc_Tcenv_lookup_type_vars(te);_tmp3B->v=_tmp3F5;});_tmp3B;})): rgn_opt,0);}): t;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};
# 162
static struct _tuple16*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple16*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->f1=tv,_tmp3C->f2=0;_tmp3C;});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 166
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 169
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp39A;_tmp39A.tvars_and_bounds_opt=0,_tmp39A.patvars=0;_tmp39A;});
# 174
{void*_tmp3D=p->r;void*_tmp3E=_tmp3D;struct Cyc_Absyn_Datatypedecl*_tmpF4;struct Cyc_Absyn_Datatypefield*_tmpF3;struct Cyc_List_List**_tmpF2;int _tmpF1;struct Cyc_Absyn_Aggrdecl*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_List_List**_tmpEE;int _tmpED;struct Cyc_List_List**_tmpEC;int _tmpEB;struct Cyc_Absyn_Pat*_tmpEA;void*_tmpE9;struct Cyc_Absyn_Enumdecl*_tmpE8;int _tmpE7;struct Cyc_Absyn_Tvar*_tmpE6;struct Cyc_Absyn_Vardecl*_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE4;struct Cyc_Absyn_Pat*_tmpE3;struct Cyc_Absyn_Tvar*_tmpE2;struct Cyc_Absyn_Vardecl*_tmpE1;struct Cyc_Absyn_Vardecl*_tmpE0;struct Cyc_Absyn_Pat*_tmpDF;switch(*((int*)_tmp3E)){case 0U: _LL1: _LL2:
# 177
 if(topt != 0)
t=*topt;else{
# 180
({void*_tmp3F8=({struct Cyc_List_List*_tmp3F7=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp3F7,topt);});t=_tmp3F8;});}
goto _LL0;case 1U: _LL3: _tmpE0=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpDF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL4: {
# 184
struct _tuple2*_tmp3F=_tmpE0->name;struct _tuple2*_tmp40=_tmp3F;struct _dyneither_ptr _tmp4D;_LL2E: _tmp4D=*_tmp40->f2;_LL2F:;
if(({struct _dyneither_ptr _tmp3FA=(struct _dyneither_ptr)_tmp4D;Cyc_strcmp(_tmp3FA,({const char*_tmp41="true";_tag_dyneither(_tmp41,sizeof(char),5U);}));})== 0  || ({struct _dyneither_ptr _tmp3F9=(struct _dyneither_ptr)_tmp4D;Cyc_strcmp(_tmp3F9,({const char*_tmp42="false";_tag_dyneither(_tmp42,sizeof(char),6U);}));})== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp45=({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0U,_tmp388.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4D);_tmp388;});void*_tmp43[1U];_tmp43[0]=& _tmp45;({unsigned int _tmp3FC=p->loc;struct _dyneither_ptr _tmp3FB=({const char*_tmp44="you probably do not want to use %s as a local variable";_tag_dyneither(_tmp44,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp3FC,_tmp3FB,_tag_dyneither(_tmp43,sizeof(void*),1U));});});
({struct Cyc_Tcpat_TcPatResult _tmp3FD=Cyc_Tcpat_tcPatRec(te,_tmpDF,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp3FD;});
t=(void*)_check_null(_tmpDF->topt);
# 191
{void*_tmp46=Cyc_Tcutil_compress(t);void*_tmp47=_tmp46;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->tag == 4U){_LL31: _LL32:
# 193
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp48=0U;({unsigned int _tmp3FF=p->loc;struct _dyneither_ptr _tmp3FE=({const char*_tmp49="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp49,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp3FF,_tmp3FE,_tag_dyneither(_tmp48,sizeof(void*),0U));});});
goto _LL30;}else{_LL33: _LL34:
# 197
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp4A=0U;({unsigned int _tmp401=p->loc;struct _dyneither_ptr _tmp400=({const char*_tmp4B="pattern would point to an abstract member";_tag_dyneither(_tmp4B,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp401,_tmp400,_tag_dyneither(_tmp4A,sizeof(void*),0U));});});
goto _LL30;}_LL30:;}
# 201
({struct Cyc_Absyn_Vardecl**_tmp404=({struct Cyc_Absyn_Vardecl**_tmp4C=_cycalloc(sizeof(*_tmp4C));*_tmp4C=_tmpE0;_tmp4C;});struct Cyc_Absyn_Exp*_tmp403=access_exp;struct Cyc_List_List**_tmp402=& res.patvars;Cyc_Tcpat_set_vd(_tmp404,_tmp403,_tmp402,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}case 2U: _LL5: _tmpE2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL6: {
# 204
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp406=p->loc;struct Cyc_Tcenv_Tenv*_tmp405=te;Cyc_Tcenv_add_type_vars(_tmp406,_tmp405,({struct Cyc_Absyn_Tvar*_tmp58[1U];_tmp58[0]=_tmpE2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp58,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp407=({struct _tuple1*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->f1=0,_tmp4E->f2=0;_tmp4E;});res.tvars_and_bounds_opt=_tmp407;});
({struct Cyc_List_List*_tmp40A=({
struct Cyc_List_List*_tmp409=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp409,({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _tuple16*_tmp408=({struct _tuple16*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=_tmpE2,_tmp4F->f2=1;_tmp4F;});_tmp50->hd=_tmp408;}),_tmp50->tl=0;_tmp50;}));});
# 207
(*res.tvars_and_bounds_opt).f1=_tmp40A;});
# 209
({unsigned int _tmp40D=p->loc;struct Cyc_Tcenv_Tenv*_tmp40C=te2;struct Cyc_List_List*_tmp40B=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp40D,_tmp40C,_tmp40B,& Cyc_Tcutil_tmk,1,0,_tmpE1->type);});
# 212
if(topt != 0)t=*topt;else{
({void*_tmp40F=({struct Cyc_List_List*_tmp40E=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp40E,topt);});t=_tmp40F;});}
{void*_tmp51=Cyc_Tcutil_compress(t);void*_tmp52=_tmp51;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->tag == 4U){_LL36: _LL37:
# 216
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp53=0U;({unsigned int _tmp411=p->loc;struct _dyneither_ptr _tmp410=({const char*_tmp54="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp54,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp411,_tmp410,_tag_dyneither(_tmp53,sizeof(void*),0U));});});
goto _LL35;}else{_LL38: _LL39:
# 220
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp55=0U;({unsigned int _tmp413=p->loc;struct _dyneither_ptr _tmp412=({const char*_tmp56="pattern would point to an abstract member";_tag_dyneither(_tmp56,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp413,_tmp412,_tag_dyneither(_tmp55,sizeof(void*),0U));});});
goto _LL35;}_LL35:;}
# 224
({struct Cyc_Absyn_Vardecl**_tmp416=({struct Cyc_Absyn_Vardecl**_tmp57=_cycalloc(sizeof(*_tmp57));*_tmp57=_tmpE1;_tmp57;});struct Cyc_Absyn_Exp*_tmp415=access_exp;struct Cyc_List_List**_tmp414=& res.patvars;Cyc_Tcpat_set_vd(_tmp416,_tmp415,_tmp414,_tmpE1->type);});
goto _LL0;}case 3U: _LL7: _tmpE4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL8:
# 228
({struct Cyc_Tcpat_TcPatResult _tmp417=Cyc_Tcpat_tcPatRec(te,_tmpE3,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp417;});
t=(void*)_check_null(_tmpE3->topt);
if(!allow_ref_pat  || rgn_pat == 0){
({void*_tmp59=0U;({unsigned int _tmp419=p->loc;struct _dyneither_ptr _tmp418=({const char*_tmp5A="* pattern would point into an unknown/unallowed region";_tag_dyneither(_tmp5A,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp419,_tmp418,_tag_dyneither(_tmp59,sizeof(void*),0U));});});
goto _LL0;}else{
# 235
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({void*_tmp5B=0U;({unsigned int _tmp41B=p->loc;struct _dyneither_ptr _tmp41A=({const char*_tmp5C="* pattern cannot take the address of an alias-free path";_tag_dyneither(_tmp5C,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp41B,_tmp41A,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});}{
# 238
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=3U,(_tmp5E->f1).elt_type=t,({struct Cyc_Absyn_Tqual _tmp41E=Cyc_Absyn_empty_tqual(0U);(_tmp5E->f1).elt_tq=_tmp41E;}),
((_tmp5E->f1).ptr_atts).rgn=rgn_pat,((_tmp5E->f1).ptr_atts).nullable=Cyc_Absyn_false_type,({
void*_tmp41D=Cyc_Tcutil_any_bounds(& te);((_tmp5E->f1).ptr_atts).bounds=_tmp41D;}),({void*_tmp41C=Cyc_Tcutil_any_bool(& te);((_tmp5E->f1).ptr_atts).zero_term=_tmp41C;}),((_tmp5E->f1).ptr_atts).ptrloc=0;_tmp5E;});
# 243
if((unsigned int)access_exp){
({struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_address_exp(access_exp,0U);new_access_exp=_tmp41F;});
new_access_exp->topt=t2;}
# 247
({struct Cyc_Absyn_Vardecl**_tmp422=({struct Cyc_Absyn_Vardecl**_tmp5D=_cycalloc(sizeof(*_tmp5D));*_tmp5D=_tmpE4;_tmp5D;});struct Cyc_Absyn_Exp*_tmp421=new_access_exp;struct Cyc_List_List**_tmp420=& res.patvars;Cyc_Tcpat_set_vd(_tmp422,_tmp421,_tmp420,t2);});
goto _LL0;};case 4U: _LL9: _tmpE6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpE5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LLA:
# 253
({struct Cyc_Absyn_Vardecl**_tmp425=({struct Cyc_Absyn_Vardecl**_tmp5F=_cycalloc(sizeof(*_tmp5F));*_tmp5F=_tmpE5;_tmp5F;});struct Cyc_Absyn_Exp*_tmp424=access_exp;struct Cyc_List_List**_tmp423=& res.patvars;Cyc_Tcpat_set_vd(_tmp425,_tmp424,_tmp423,_tmpE5->type);});
# 257
({unsigned int _tmp427=p->loc;struct Cyc_Tcenv_Tenv*_tmp426=te;Cyc_Tcenv_add_type_vars(_tmp427,_tmp426,({struct Cyc_Absyn_Tvar*_tmp60[1U];_tmp60[0]=_tmpE6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp428=({struct _tuple1*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->f1=0,_tmp61->f2=0;_tmp61;});res.tvars_and_bounds_opt=_tmp428;});
({struct Cyc_List_List*_tmp42A=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));
({struct _tuple16*_tmp429=({struct _tuple16*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=_tmpE6,_tmp62->f2=0;_tmp62;});_tmp63->hd=_tmp429;}),_tmp63->tl=(*res.tvars_and_bounds_opt).f1;_tmp63;});
# 260
(*res.tvars_and_bounds_opt).f1=_tmp42A;});
# 262
t=Cyc_Absyn_uint_type;
goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3E)->f1){case Cyc_Absyn_Unsigned: _LLB: _LLC:
# 265
({void*_tmp42B=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);t=_tmp42B;});goto _LL0;case Cyc_Absyn_None: _LLD: _LLE:
 goto _LL10;default: _LLF: _LL10:
({void*_tmp42C=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);t=_tmp42C;});goto _LL0;}case 11U: _LL11: _LL12:
({void*_tmp42D=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);t=_tmp42D;});goto _LL0;case 12U: _LL13: _tmpE7=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL14:
({void*_tmp42F=({void**_tmp42E=topt;Cyc_Tcpat_num_type(_tmp42E,Cyc_Absyn_gen_float_type((unsigned int)_tmpE7));});t=_tmp42F;});goto _LL0;case 13U: _LL15: _tmpE8=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL16:
# 271
({void*_tmp431=({void**_tmp430=topt;Cyc_Tcpat_num_type(_tmp430,Cyc_Absyn_enum_type(_tmpE8->name,_tmpE8));});t=_tmp431;});
goto _LL0;case 14U: _LL17: _tmpE9=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL18:
({void*_tmp432=Cyc_Tcpat_num_type(topt,_tmpE9);t=_tmp432;});goto _LL0;case 9U: _LL19: _LL1A:
# 275
 if(topt != 0){
void*_tmp64=Cyc_Tcutil_compress(*topt);void*_tmp65=_tmp64;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65)->tag == 3U){_LL3B: _LL3C:
 t=*topt;goto tcpat_end;}else{_LL3D: _LL3E:
 goto _LL3A;}_LL3A:;}{
# 280
struct Cyc_Core_Opt*_tmp66=Cyc_Tcenv_lookup_opt_type_vars(te);
({void*_tmp438=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=3U,({void*_tmp437=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp66);(_tmp67->f1).elt_type=_tmp437;}),({
struct Cyc_Absyn_Tqual _tmp436=Cyc_Absyn_empty_tqual(0U);(_tmp67->f1).elt_tq=_tmp436;}),
({void*_tmp435=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp66);((_tmp67->f1).ptr_atts).rgn=_tmp435;}),((_tmp67->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
# 285
void*_tmp434=Cyc_Tcutil_any_bounds(& te);((_tmp67->f1).ptr_atts).bounds=_tmp434;}),({void*_tmp433=Cyc_Tcutil_any_bool(& te);((_tmp67->f1).ptr_atts).zero_term=_tmp433;}),((_tmp67->f1).ptr_atts).ptrloc=0;_tmp67;});
# 281
t=_tmp438;});
# 286
goto _LL0;};case 6U: _LL1B: _tmpEA=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_LL1C: {
# 291
void*inner_typ=Cyc_Absyn_void_type;
void**_tmp68=0;
int elt_const=0;
if(topt != 0){
void*_tmp69=Cyc_Tcutil_compress(*topt);void*_tmp6A=_tmp69;void*_tmp6C;struct Cyc_Absyn_Tqual _tmp6B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->tag == 3U){_LL40: _tmp6C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->f1).elt_type;_tmp6B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A)->f1).elt_tq;_LL41:
# 297
 inner_typ=_tmp6C;
_tmp68=& inner_typ;
elt_const=_tmp6B.real_const;
goto _LL3F;}else{_LL42: _LL43:
 goto _LL3F;}_LL3F:;}{
# 306
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)({struct Cyc_Absyn_Exp*_tmp439=Cyc_Absyn_deref_exp(access_exp,0U);new_access_exp=_tmp439;});
({struct Cyc_Tcpat_TcPatResult _tmp43B=({struct Cyc_Tcpat_TcPatResult _tmp43A=res;Cyc_Tcpat_combine_results(_tmp43A,Cyc_Tcpat_tcPatRec(te,_tmpEA,_tmp68,ptr_rgn,1,new_access_exp));});res=_tmp43B;});
# 315
{void*_tmp6D=Cyc_Tcutil_compress((void*)_check_null(_tmpEA->topt));void*_tmp6E=_tmp6D;struct Cyc_Absyn_Datatypedecl*_tmp76;struct Cyc_Absyn_Datatypefield*_tmp75;struct Cyc_List_List*_tmp74;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->f1).KnownDatatypefield).tag == 2){_LL45: _tmp76=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->f1).KnownDatatypefield).val).f1;_tmp75=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1)->f1).KnownDatatypefield).val).f2;_tmp74=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f2;_LL46:
# 319
{void*_tmp6F=Cyc_Tcutil_compress(inner_typ);void*_tmp70=_tmp6F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70)->f1)->tag == 19U){_LL4A: _LL4B:
# 321
 goto DONT_PROMOTE;}else{goto _LL4C;}}else{_LL4C: _LL4D:
 goto _LL49;}_LL49:;}{
# 325
void*new_type=({union Cyc_Absyn_DatatypeInfo _tmp43C=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp72=_cycalloc(sizeof(*_tmp72));*_tmp72=_tmp76;_tmp72;}));Cyc_Absyn_datatype_type(_tmp43C,_tmp74);});
# 327
_tmpEA->topt=new_type;
({void*_tmp441=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=3U,(_tmp71->f1).elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _tmp440=Cyc_Absyn_const_tqual(0U);(_tmp71->f1).elt_tq=_tmp440;}):({
struct Cyc_Absyn_Tqual _tmp43F=Cyc_Absyn_empty_tqual(0U);(_tmp71->f1).elt_tq=_tmp43F;}),
((_tmp71->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp43E=Cyc_Tcutil_any_bool(& te);((_tmp71->f1).ptr_atts).nullable=_tmp43E;}),({
void*_tmp43D=Cyc_Absyn_bounds_one();((_tmp71->f1).ptr_atts).bounds=_tmp43D;}),((_tmp71->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp71->f1).ptr_atts).ptrloc=0;_tmp71;});
# 328
t=_tmp441;});
# 334
goto _LL44;};}else{goto _LL47;}}else{goto _LL47;}}else{_LL47: _LL48:
# 336
 DONT_PROMOTE:
({void*_tmp447=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=3U,(_tmp73->f1).elt_type=(void*)_check_null(_tmpEA->topt),
elt_const?({struct Cyc_Absyn_Tqual _tmp446=Cyc_Absyn_const_tqual(0U);(_tmp73->f1).elt_tq=_tmp446;}):({
struct Cyc_Absyn_Tqual _tmp445=Cyc_Absyn_empty_tqual(0U);(_tmp73->f1).elt_tq=_tmp445;}),
((_tmp73->f1).ptr_atts).rgn=ptr_rgn,({void*_tmp444=Cyc_Tcutil_any_bool(& te);((_tmp73->f1).ptr_atts).nullable=_tmp444;}),({
void*_tmp443=Cyc_Tcutil_any_bounds(& te);((_tmp73->f1).ptr_atts).bounds=_tmp443;}),({void*_tmp442=Cyc_Tcutil_any_bool(& te);((_tmp73->f1).ptr_atts).zero_term=_tmp442;}),((_tmp73->f1).ptr_atts).ptrloc=0;_tmp73;});
# 337
t=_tmp447;});}_LL44:;}
# 343
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpEA->topt;
goto _LL0;};}case 5U: _LL1D: _tmpEC=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpEB=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_LL1E: {
# 347
struct Cyc_List_List*_tmp77=*_tmpEC;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp78=Cyc_Tcutil_compress(*topt);void*_tmp79=_tmp78;struct Cyc_List_List*_tmp7F;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp79)->tag == 6U){_LL4F: _tmp7F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp79)->f1;_LL50:
# 353
 topt_ts=_tmp7F;
if(_tmpEB){
# 356
int _tmp7A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp77);
int _tmp7B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7F);
if(_tmp7A < _tmp7B){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp7B - _tmp7A;++ i){
({struct Cyc_List_List*_tmp449=({struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*_tmp7C));({struct Cyc_Absyn_Pat*_tmp448=Cyc_Tcpat_wild_pat(p->loc);_tmp7C->hd=_tmp448;}),_tmp7C->tl=wild_ps;_tmp7C;});wild_ps=_tmp449;});}}
({struct Cyc_List_List*_tmp44A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp77,wild_ps);*_tmpEC=_tmp44A;});
_tmp77=*_tmpEC;}else{
if(({int _tmp44B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp77);_tmp44B == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7F);}))
({void*_tmp7D=0U;({unsigned int _tmp44D=p->loc;struct _dyneither_ptr _tmp44C=({const char*_tmp7E="unnecessary ... in tuple pattern";_tag_dyneither(_tmp7E,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp44D,_tmp44C,_tag_dyneither(_tmp7D,sizeof(void*),0U));});});}}
# 368
goto _LL4E;}else{_LL51: _LL52:
# 370
 goto _LL4E;}_LL4E:;}else{
# 372
if(_tmpEB)
({void*_tmp80=0U;({unsigned int _tmp44F=p->loc;struct _dyneither_ptr _tmp44E=({const char*_tmp81="cannot determine missing fields for ... in tuple pattern";_tag_dyneither(_tmp81,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp44F,_tmp44E,_tag_dyneither(_tmp80,sizeof(void*),0U));});});}
{int i=0;for(0;_tmp77 != 0;(_tmp77=_tmp77->tl,i ++)){
void**_tmp82=0;
if(topt_ts != 0){
_tmp82=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 380
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp451=({struct Cyc_Absyn_Exp*_tmp450=access_exp;Cyc_Absyn_subscript_exp(_tmp450,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp389;(_tmp389.Int_c).tag=5U,((_tmp389.Int_c).val).f1=Cyc_Absyn_Unsigned,((_tmp389.Int_c).val).f2=i;_tmp389;}),0U),0U);});
# 382
new_access_exp=_tmp451;});
# 385
({struct Cyc_Tcpat_TcPatResult _tmp453=({struct Cyc_Tcpat_TcPatResult _tmp452=res;Cyc_Tcpat_combine_results(_tmp452,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp77->hd,_tmp82,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp453;});
# 388
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp77->hd)->topt;
({struct Cyc_List_List*_tmp456=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));({struct _tuple17*_tmp455=({struct _tuple17*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_Tqual _tmp454=Cyc_Absyn_empty_tqual(0U);_tmp83->f1=_tmp454;}),_tmp83->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp77->hd)->topt);_tmp83;});_tmp84->hd=_tmp455;}),_tmp84->tl=pat_ts;_tmp84;});pat_ts=_tmp456;});};}}
# 391
({void*_tmp458=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=6U,({struct Cyc_List_List*_tmp457=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp85->f1=_tmp457;});_tmp85;});t=_tmp458;});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1)->KnownAggr).tag == 2){_LL1F: _tmpF0=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f1)->KnownAggr).val;_tmpEF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_tmpEE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f3;_tmpED=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3E)->f4;_LL20: {
# 395
struct Cyc_List_List*_tmp86=*_tmpEE;
struct _dyneither_ptr aggr_str=_tmpF0->kind == Cyc_Absyn_StructA?({const char*_tmpC5="struct";_tag_dyneither(_tmpC5,sizeof(char),7U);}):({const char*_tmpC6="union";_tag_dyneither(_tmpC6,sizeof(char),6U);});
if(_tmpF0->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp38A;_tmp38A.tag=0U,_tmp38A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp38A;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({unsigned int _tmp45A=p->loc;struct _dyneither_ptr _tmp459=({const char*_tmp88="can't destructure an abstract %s";_tag_dyneither(_tmp88,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp45A,_tmp459,_tag_dyneither(_tmp87,sizeof(void*),1U));});});
({void*_tmp45B=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp45B;});
goto _LL0;}
# 404
if(_tmpF0->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->tagged)
allow_ref_pat=0;
if(_tmpEF != 0){
# 410
if(topt == 0  || Cyc_Tcutil_type_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}
# 413
{struct _RegionHandle _tmp8A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp8A;_push_region(rgn);
# 415
{struct Cyc_List_List*_tmp8B=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp8C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpEF;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp8C))->hd;
_tmp8C=_tmp8C->tl;{
void*_tmp8D=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp8E=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp8F=_tmp8E;struct Cyc_Absyn_Kind*_tmp93;struct Cyc_Absyn_Kind*_tmp92;switch(*((int*)_tmp8F)){case 2U: _LL54: _tmp92=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F)->f2;_LL55:
 _tmp93=_tmp92;goto _LL57;case 0U: _LL56: _tmp93=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F)->f1;_LL57:
 k2=_tmp93;goto _LL53;default: _LL58: _LL59:
({void*_tmp90=0U;({struct _dyneither_ptr _tmp45C=({const char*_tmp91="unconstrained existential type variable in aggregate";_tag_dyneither(_tmp91,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp45C,_tag_dyneither(_tmp90,sizeof(void*),0U));});});}_LL53:;}
# 431
{void*_tmp94=_tmp8D;struct Cyc_Core_Opt**_tmp99;struct Cyc_Core_Opt**_tmp98;struct Cyc_Absyn_Kind*_tmp97;struct Cyc_Absyn_Kind*_tmp96;switch(*((int*)_tmp94)){case 0U: _LL5B: _tmp96=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL5C:
# 434
 if(!Cyc_Tcutil_kind_leq(k2,_tmp96))
error=1;
goto _LL5A;case 2U: _LL5D: _tmp98=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f1;_tmp97=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL5E:
 _tmp99=_tmp98;goto _LL60;default: _LL5F: _tmp99=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL60:
({struct Cyc_Core_Opt*_tmp45D=({struct Cyc_Core_Opt*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->v=_tmp8E;_tmp95;});*_tmp99=_tmp45D;});goto _LL5A;}_LL5A:;}
# 440
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmp9C=({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0U,_tmp38D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmp38D;});struct Cyc_String_pa_PrintArg_struct _tmp9D=({struct Cyc_String_pa_PrintArg_struct _tmp38C;_tmp38C.tag=0U,({
# 444
struct _dyneither_ptr _tmp45E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp8D));_tmp38C.f1=_tmp45E;});_tmp38C;});struct Cyc_String_pa_PrintArg_struct _tmp9E=({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U,({struct _dyneither_ptr _tmp45F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(k2));_tmp38B.f1=_tmp45F;});_tmp38B;});void*_tmp9A[3U];_tmp9A[0]=& _tmp9C,_tmp9A[1]=& _tmp9D,_tmp9A[2]=& _tmp9E;({unsigned int _tmp461=p->loc;struct _dyneither_ptr _tmp460=({const char*_tmp9B="type variable %s has kind %s but must have at least kind %s";_tag_dyneither(_tmp9B,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp461,_tmp460,_tag_dyneither(_tmp9A,sizeof(void*),3U));});});{
# 446
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=2U,_tmpA4->f1=tv;_tmpA4;});
({struct Cyc_List_List*_tmp462=({struct Cyc_List_List*_tmp9F=_region_malloc(rgn,sizeof(*_tmp9F));_tmp9F->hd=vartype,_tmp9F->tl=_tmp8B;_tmp9F;});_tmp8B=_tmp462;});
# 450
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable)
({struct Cyc_List_List*_tmp464=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*_tmpA1));
({struct _tuple0*_tmp463=({struct _tuple0*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->f1=Cyc_Absyn_empty_effect,_tmpA0->f2=vartype;_tmpA0;});_tmpA1->hd=_tmp463;}),_tmpA1->tl=outlives_constraints;_tmpA1;});
# 452
outlives_constraints=_tmp464;});else{
# 455
({void*_tmpA2=0U;({struct _dyneither_ptr _tmp465=({const char*_tmpA3="opened existential had unique or top region kind";_tag_dyneither(_tmpA3,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp465,_tag_dyneither(_tmpA2,sizeof(void*),0U));});});}}};};}}
# 459
({struct Cyc_List_List*_tmp466=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8B);_tmp8B=_tmp466;});{
# 461
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmpEF);
# 463
struct Cyc_List_List*_tmpA5=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmpA6=({struct _tuple12 _tmp392;_tmp392.f1=_tmpA5,_tmp392.f2=rgn;_tmp392;});
struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpA6,_tmpF0->tvs);
struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->exist_vars,_tmp8B);
struct Cyc_List_List*_tmpA9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpA7);
struct Cyc_List_List*_tmpAA=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpA8);
struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpA7,_tmpA8);
# 471
if(_tmpEF != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp467=({struct _tuple1*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->f1=0,_tmpAC->f2=0;_tmpAC;});res.tvars_and_bounds_opt=_tmp467;});
({struct Cyc_List_List*_tmp469=({
struct Cyc_List_List*_tmp468=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp468,((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmpEF));});
# 474
(*res.tvars_and_bounds_opt).f1=_tmp469;});
# 476
({struct Cyc_List_List*_tmp46A=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 476
(*res.tvars_and_bounds_opt).f2=_tmp46A;});{
# 478
struct Cyc_List_List*_tmpAD=0;
{struct Cyc_List_List*_tmpAE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->rgn_po;for(0;_tmpAE != 0;_tmpAE=_tmpAE->tl){
({struct Cyc_List_List*_tmp46E=({struct Cyc_List_List*_tmpB0=_cycalloc(sizeof(*_tmpB0));({struct _tuple0*_tmp46D=({struct _tuple0*_tmpAF=_cycalloc(sizeof(*_tmpAF));({void*_tmp46C=Cyc_Tcutil_rsubstitute(rgn,_tmpAB,(*((struct _tuple0*)_tmpAE->hd)).f1);_tmpAF->f1=_tmp46C;}),({
void*_tmp46B=Cyc_Tcutil_rsubstitute(rgn,_tmpAB,(*((struct _tuple0*)_tmpAE->hd)).f2);_tmpAF->f2=_tmp46B;});_tmpAF;});
# 480
_tmpB0->hd=_tmp46D;}),_tmpB0->tl=_tmpAD;_tmpB0;});_tmpAD=_tmp46E;});}}
# 483
({struct Cyc_List_List*_tmp46F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpAD);_tmpAD=_tmp46F;});
({struct Cyc_List_List*_tmp470=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpAD);
# 484
(*res.tvars_and_bounds_opt).f2=_tmp470;});};}
# 489
({void*_tmp472=({union Cyc_Absyn_AggrInfo _tmp471=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpB1=_cycalloc(sizeof(*_tmpB1));*_tmpB1=_tmpF0;_tmpB1;}));Cyc_Absyn_aggr_type(_tmp471,_tmpA9);});t=_tmp472;});
if(_tmpED  && _tmpF0->kind == Cyc_Absyn_UnionA)
({void*_tmpB2=0U;({unsigned int _tmp474=p->loc;struct _dyneither_ptr _tmp473=({const char*_tmpB3="`...' pattern not allowed in union pattern";_tag_dyneither(_tmpB3,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp474,_tmp473,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});else{
if(_tmpED){
# 494
int _tmpB4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp86);
int _tmpB5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->fields);
if(_tmpB4 < _tmpB5){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmpB5 - _tmpB4;++ i){
({struct Cyc_List_List*_tmp477=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));({struct _tuple18*_tmp476=({struct _tuple18*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->f1=0,({struct Cyc_Absyn_Pat*_tmp475=Cyc_Tcpat_wild_pat(p->loc);_tmpB6->f2=_tmp475;});_tmpB6;});_tmpB7->hd=_tmp476;}),_tmpB7->tl=wild_dps;_tmpB7;});wild_dps=_tmp477;});}}
({struct Cyc_List_List*_tmp478=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp86,wild_dps);*_tmpEE=_tmp478;});
_tmp86=*_tmpEE;}else{
if(_tmpB4 == _tmpB5)
({void*_tmpB8=0U;({unsigned int _tmp47A=p->loc;struct _dyneither_ptr _tmp479=({const char*_tmpB9="unnecessary ... in struct pattern";_tag_dyneither(_tmpB9,sizeof(char),34U);});Cyc_Tcutil_warn(_tmp47A,_tmp479,_tag_dyneither(_tmpB8,sizeof(void*),0U));});});}}}{
# 505
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp86,_tmpF0->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF0->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmpBA=(struct _tuple19*)fields->hd;struct _tuple19*_tmpBB=_tmpBA;struct Cyc_Absyn_Aggrfield*_tmpC4;struct Cyc_Absyn_Pat*_tmpC3;_LL62: _tmpC4=_tmpBB->f1;_tmpC3=_tmpBB->f2;_LL63:;{
void*_tmpBC=Cyc_Tcutil_rsubstitute(rgn,_tmpAB,_tmpC4->type);
# 511
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp47B=Cyc_Absyn_aggrmember_exp(access_exp,_tmpC4->name,0U);new_access_exp=_tmp47B;});
({struct Cyc_Tcpat_TcPatResult _tmp47D=({struct Cyc_Tcpat_TcPatResult _tmp47C=res;Cyc_Tcpat_combine_results(_tmp47C,Cyc_Tcpat_tcPatRec(te2,_tmpC3,& _tmpBC,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp47D;});
# 519
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpC3->topt),_tmpBC))
({struct Cyc_String_pa_PrintArg_struct _tmpBF=({struct Cyc_String_pa_PrintArg_struct _tmp391;_tmp391.tag=0U,_tmp391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC4->name);_tmp391;});struct Cyc_String_pa_PrintArg_struct _tmpC0=({struct Cyc_String_pa_PrintArg_struct _tmp390;_tmp390.tag=0U,_tmp390.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp390;});struct Cyc_String_pa_PrintArg_struct _tmpC1=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,({
struct _dyneither_ptr _tmp47E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpBC));_tmp38F.f1=_tmp47E;});_tmp38F;});struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,({
struct _dyneither_ptr _tmp47F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpC3->topt)));_tmp38E.f1=_tmp47F;});_tmp38E;});void*_tmpBD[4U];_tmpBD[0]=& _tmpBF,_tmpBD[1]=& _tmpC0,_tmpBD[2]=& _tmpC1,_tmpBD[3]=& _tmpC2;({unsigned int _tmp481=p->loc;struct _dyneither_ptr _tmp480=({const char*_tmpBE="field %s of %s pattern expects type %s != %s";_tag_dyneither(_tmpBE,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp481,_tmp480,_tag_dyneither(_tmpBD,sizeof(void*),4U));});});
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpC3->topt;};}};};}
# 415
;_pop_region(rgn);}
# 526
goto _LL0;}}else{_LL25: _LL26:
# 579
 goto _LL28;}}else{_LL23: _LL24:
# 578
 goto _LL26;}case 8U: _LL21: _tmpF4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f1;_tmpF3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f2;_tmpF2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f3;_tmpF1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3E)->f4;_LL22: {
# 529
struct Cyc_List_List*_tmpC7=*_tmpF2;
struct Cyc_List_List*tqts=_tmpF3->typs;
# 532
struct Cyc_List_List*_tmpC8=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmpC9=({struct _tuple12 _tmp399;_tmp399.f1=_tmpC8,_tmp399.f2=Cyc_Core_heap_region;_tmp399;});
struct Cyc_List_List*_tmpCA=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmpC9,_tmpF4->tvs);
struct Cyc_List_List*_tmpCB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpCA);
({void*_tmp483=({union Cyc_Absyn_DatatypeFieldInfo _tmp482=Cyc_Absyn_KnownDatatypefield(_tmpF4,_tmpF3);Cyc_Absyn_datatype_field_type(_tmp482,_tmpCB);});t=_tmp483;});
if(_tmpF1){
# 539
int _tmpCC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC7);
int _tmpCD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmpCC < _tmpCD){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpCD - _tmpCC;++ i){
({struct Cyc_List_List*_tmp485=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));({struct Cyc_Absyn_Pat*_tmp484=Cyc_Tcpat_wild_pat(p->loc);_tmpCE->hd=_tmp484;}),_tmpCE->tl=wild_ps;_tmpCE;});wild_ps=_tmp485;});}}
({struct Cyc_List_List*_tmp486=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC7,wild_ps);*_tmpF2=_tmp486;});
_tmpC7=*_tmpF2;}else{
if(_tmpCC == _tmpCD)
({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp393;_tmp393.tag=0U,({
struct _dyneither_ptr _tmp487=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF4->name));_tmp393.f1=_tmp487;});_tmp393;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned int _tmp489=p->loc;struct _dyneither_ptr _tmp488=({const char*_tmpD0="unnecessary ... in datatype field %s";_tag_dyneither(_tmpD0,sizeof(char),37U);});Cyc_Tcutil_warn(_tmp489,_tmp488,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});}}
# 551
for(0;_tmpC7 != 0  && tqts != 0;(_tmpC7=_tmpC7->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpD2=(struct Cyc_Absyn_Pat*)_tmpC7->hd;
# 555
void*_tmpD3=Cyc_Tcutil_substitute(_tmpCA,(*((struct _tuple17*)tqts->hd)).f2);
# 558
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
({struct Cyc_Tcpat_TcPatResult _tmp48B=({struct Cyc_Tcpat_TcPatResult _tmp48A=res;Cyc_Tcpat_combine_results(_tmp48A,Cyc_Tcpat_tcPatRec(te,_tmpD2,& _tmpD3,rgn_pat,allow_ref_pat,0));});res=_tmp48B;});
# 565
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpD2->topt),_tmpD3))
({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp396;_tmp396.tag=0U,({
struct _dyneither_ptr _tmp48C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF3->name));_tmp396.f1=_tmp48C;});_tmp396;});struct Cyc_String_pa_PrintArg_struct _tmpD7=({struct Cyc_String_pa_PrintArg_struct _tmp395;_tmp395.tag=0U,({struct _dyneither_ptr _tmp48D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD3));_tmp395.f1=_tmp48D;});_tmp395;});struct Cyc_String_pa_PrintArg_struct _tmpD8=({struct Cyc_String_pa_PrintArg_struct _tmp394;_tmp394.tag=0U,({
struct _dyneither_ptr _tmp48E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpD2->topt)));_tmp394.f1=_tmp48E;});_tmp394;});void*_tmpD4[3U];_tmpD4[0]=& _tmpD6,_tmpD4[1]=& _tmpD7,_tmpD4[2]=& _tmpD8;({unsigned int _tmp490=_tmpD2->loc;struct _dyneither_ptr _tmp48F=({const char*_tmpD5="%s expects argument type %s, not %s";_tag_dyneither(_tmpD5,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp490,_tmp48F,_tag_dyneither(_tmpD4,sizeof(void*),3U));});});}
# 570
if(_tmpC7 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp397;_tmp397.tag=0U,({
struct _dyneither_ptr _tmp491=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF3->name));_tmp397.f1=_tmp491;});_tmp397;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({unsigned int _tmp493=p->loc;struct _dyneither_ptr _tmp492=({const char*_tmpDA="too many arguments for datatype constructor %s";_tag_dyneither(_tmpDA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp493,_tmp492,_tag_dyneither(_tmpD9,sizeof(void*),1U));});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0U,({
struct _dyneither_ptr _tmp494=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF3->name));_tmp398.f1=_tmp494;});_tmp398;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({unsigned int _tmp496=p->loc;struct _dyneither_ptr _tmp495=({const char*_tmpDD="too few arguments for datatype constructor %s";_tag_dyneither(_tmpDD,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp496,_tmp495,_tag_dyneither(_tmpDC,sizeof(void*),1U));});});
goto _LL0;}case 15U: _LL27: _LL28:
# 580
 goto _LL2A;case 17U: _LL29: _LL2A:
 goto _LL2C;default: _LL2B: _LL2C:
# 583
({void*_tmp497=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp497;});goto _LL0;}_LL0:;}
# 585
tcpat_end:
 p->topt=t;
return res;};}
# 590
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 592
struct Cyc_Tcpat_TcPatResult _tmpF5=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 594
struct _tuple1 _tmpF6=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpF5.patvars);struct _tuple1 _tmpF7=_tmpF6;struct Cyc_List_List*_tmpFF;_LL1: _tmpFF=_tmpF7.f1;_LL2:;{
struct Cyc_List_List*_tmpF8=0;
{struct Cyc_List_List*x=_tmpFF;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)({struct Cyc_List_List*_tmp498=({struct Cyc_List_List*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),_tmpF9->tl=_tmpF8;_tmpF9;});_tmpF8=_tmp498;});}}
({struct Cyc_List_List*_tmp49A=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_name,_tmpF8);unsigned int _tmp499=p->loc;Cyc_Tcutil_check_unique_vars(_tmp49A,_tmp499,({const char*_tmpFA="pattern contains a repeated variable";_tag_dyneither(_tmpFA,sizeof(char),37U);}));});
# 603
{struct Cyc_List_List*x=_tmpF5.patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmpFB=(struct _tuple15*)x->hd;struct _tuple15*_tmpFC=_tmpFB;struct Cyc_Absyn_Vardecl**_tmpFE;struct Cyc_Absyn_Exp**_tmpFD;_LL4: _tmpFE=_tmpFC->f1;_tmpFD=(struct Cyc_Absyn_Exp**)& _tmpFC->f2;_LL5:;
if(*_tmpFD != 0  && *_tmpFD != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp49B=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmpFD));*_tmpFD=_tmp49B;});}}
# 608
return _tmpF5;};}
# 614
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 617
struct _tuple0 _tmp100=({struct _tuple0 _tmp39B;({void*_tmp49D=Cyc_Tcutil_compress(old_type);_tmp39B.f1=_tmp49D;}),({void*_tmp49C=Cyc_Tcutil_compress(new_type);_tmp39B.f2=_tmp49C;});_tmp39B;});struct _tuple0 _tmp101=_tmp100;struct Cyc_Absyn_PtrInfo _tmp105;struct Cyc_Absyn_PtrInfo _tmp104;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f2)->tag == 3U){_LL1: _tmp105=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f1)->f1;_tmp104=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp101.f2)->f1;_LL2: {
# 619
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp102=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=3U,(_tmp103->f1).elt_type=_tmp105.elt_type,(_tmp103->f1).elt_tq=_tmp104.elt_tq,
((_tmp103->f1).ptr_atts).rgn=(_tmp105.ptr_atts).rgn,((_tmp103->f1).ptr_atts).nullable=(_tmp104.ptr_atts).nullable,((_tmp103->f1).ptr_atts).bounds=(_tmp104.ptr_atts).bounds,((_tmp103->f1).ptr_atts).zero_term=(_tmp104.ptr_atts).zero_term,((_tmp103->f1).ptr_atts).ptrloc=(_tmp105.ptr_atts).ptrloc;_tmp103;});
# 625
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp102,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp102);}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 634
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 637
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));({struct _tuple0*_tmp49F=({struct _tuple0*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->f1=Cyc_Absyn_unique_rgn_type,({void*_tmp49E=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->tag=2U,_tmp110->f1=tv;_tmp110;});_tmp111->f2=_tmp49E;});_tmp111;});_tmp112->hd=_tmp49F;}),_tmp112->tl=0;_tmp112;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 655 "tcpat.cyc"
struct _tuple0 _tmp106=({struct _tuple0 _tmp39C;({void*_tmp4A1=Cyc_Tcutil_compress(old_type);_tmp39C.f1=_tmp4A1;}),({void*_tmp4A0=Cyc_Tcutil_compress(new_type);_tmp39C.f2=_tmp4A0;});_tmp39C;});struct _tuple0 _tmp107=_tmp106;struct Cyc_Absyn_PtrInfo _tmp10B;struct Cyc_Absyn_PtrInfo _tmp10A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f1)->tag == 3U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f2)->tag == 3U){_LL1: _tmp10B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f1)->f1;_tmp10A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp107.f2)->f1;_LL2:
 goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp108=0U;({unsigned int _tmp4A3=loc;struct _dyneither_ptr _tmp4A2=({const char*_tmp109="alias requires pointer type";_tag_dyneither(_tmp109,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp4A3,_tmp4A2,_tag_dyneither(_tmp108,sizeof(void*),0U));});});goto _LL0;}_LL0:;}else{
# 660
({struct Cyc_String_pa_PrintArg_struct _tmp10E=({struct Cyc_String_pa_PrintArg_struct _tmp39E;_tmp39E.tag=0U,({
struct _dyneither_ptr _tmp4A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type));_tmp39E.f1=_tmp4A4;});_tmp39E;});struct Cyc_String_pa_PrintArg_struct _tmp10F=({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0U,({struct _dyneither_ptr _tmp4A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(new_type));_tmp39D.f1=_tmp4A5;});_tmp39D;});void*_tmp10C[2U];_tmp10C[0]=& _tmp10E,_tmp10C[1]=& _tmp10F;({unsigned int _tmp4A7=loc;struct _dyneither_ptr _tmp4A6=({const char*_tmp10D="cannot alias value of type %s to type %s";_tag_dyneither(_tmp10D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp4A7,_tmp4A6,_tag_dyneither(_tmp10C,sizeof(void*),2U));});});}}
# 667
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 671
void*_tmp113=p->r;void*_tmp114=_tmp113;struct Cyc_Absyn_Tvar*_tmp13C;struct Cyc_Absyn_Vardecl*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13A;struct Cyc_Absyn_Pat*_tmp139;struct Cyc_Absyn_Vardecl*_tmp138;struct Cyc_Absyn_Pat*_tmp137;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp135;union Cyc_Absyn_AggrInfo*_tmp134;struct Cyc_List_List*_tmp133;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_Pat*_tmp131;switch(*((int*)_tmp114)){case 6U: _LL1: _tmp131=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_LL2: {
# 673
void*_tmp115=(void*)_check_null(p->topt);void*_tmp116=_tmp115;void*_tmp119;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp116)->tag == 3U){_LL12: _tmp119=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp116)->f1).ptr_atts).rgn;_LL13:
# 675
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp119);
({struct Cyc_Tcenv_Tenv*_tmp4AA=te;struct Cyc_Absyn_Pat*_tmp4A9=_tmp131;int _tmp4A8=Cyc_Tcutil_is_noalias_region(_tmp119,0);Cyc_Tcpat_check_pat_regions_rec(_tmp4AA,_tmp4A9,_tmp4A8,patvars);});
return;}else{_LL14: _LL15:
({void*_tmp117=0U;({struct _dyneither_ptr _tmp4AB=({const char*_tmp118="check_pat_regions: bad pointer type";_tag_dyneither(_tmp118,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4AB,_tag_dyneither(_tmp117,sizeof(void*),0U));});});}_LL11:;}case 7U: _LL3: _tmp134=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp133=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_tmp132=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp114)->f3;_LL4:
# 681
 for(0;_tmp132 != 0;_tmp132=_tmp132->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple18*)_tmp132->hd)).f2,did_noalias_deref,patvars);}
return;case 8U: _LL5: _tmp135=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp114)->f3;_LL6:
# 685
 did_noalias_deref=0;_tmp136=_tmp135;goto _LL8;case 5U: _LL7: _tmp136=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_LL8:
# 687
 for(0;_tmp136 != 0;_tmp136=_tmp136->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp136->hd,did_noalias_deref,patvars);}
return;case 3U: _LL9: _tmp138=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp137=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_LLA:
# 691
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp11A=(struct _tuple15*)x->hd;struct _tuple15*_tmp11B=_tmp11A;struct Cyc_Absyn_Vardecl**_tmp124;struct Cyc_Absyn_Exp*_tmp123;_LL17: _tmp124=_tmp11B->f1;_tmp123=_tmp11B->f2;_LL18:;
# 697
if((_tmp124 != 0  && *_tmp124 == _tmp138) && _tmp123 != 0){
{void*_tmp11C=_tmp123->r;void*_tmp11D=_tmp11C;struct Cyc_Absyn_Exp*_tmp122;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp11D)->tag == 15U){_LL1A: _tmp122=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp11D)->f1;_LL1B:
# 700
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp122->topt)))
({void*_tmp11E=0U;({unsigned int _tmp4AD=p->loc;struct _dyneither_ptr _tmp4AC=({const char*_tmp11F="reference pattern not allowed on alias-free pointers";_tag_dyneither(_tmp11F,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp4AD,_tmp4AC,_tag_dyneither(_tmp11E,sizeof(void*),0U));});});
goto _LL19;}else{_LL1C: _LL1D:
# 704
({void*_tmp120=0U;({struct _dyneither_ptr _tmp4AE=({const char*_tmp121="check_pat_regions: bad reference access expression";_tag_dyneither(_tmp121,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4AE,_tag_dyneither(_tmp120,sizeof(void*),0U));});});}_LL19:;}
# 706
break;}}}
# 709
Cyc_Tcpat_check_pat_regions_rec(te,_tmp137,did_noalias_deref,patvars);
return;case 1U: _LLB: _tmp13A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp139=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_LLC:
# 712
{void*_tmp125=p->topt;void*_tmp126=_tmp125;if(_tmp126 != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp126)->tag == 4U){_LL1F: _LL20:
# 714
 if(did_noalias_deref){
({void*_tmp127=0U;({unsigned int _tmp4B0=p->loc;struct _dyneither_ptr _tmp4AF=({const char*_tmp128="pattern to array would create alias of no-alias pointer";_tag_dyneither(_tmp128,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp4B0,_tmp4AF,_tag_dyneither(_tmp127,sizeof(void*),0U));});});
return;}
# 718
goto _LL1E;}else{goto _LL21;}}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 721
Cyc_Tcpat_check_pat_regions_rec(te,_tmp139,did_noalias_deref,patvars);
return;case 2U: _LLD: _tmp13C=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp114)->f1;_tmp13B=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp114)->f2;_LLE:
# 724
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp129=(struct _tuple15*)x->hd;struct _tuple15*_tmp12A=_tmp129;struct Cyc_Absyn_Vardecl**_tmp130;struct Cyc_Absyn_Exp*_tmp12F;_LL24: _tmp130=_tmp12A->f1;_tmp12F=_tmp12A->f2;_LL25:;
# 728
if(_tmp130 != 0  && *_tmp130 == _tmp13B){
if(_tmp12F == 0)
({void*_tmp12B=0U;({unsigned int _tmp4B2=p->loc;struct _dyneither_ptr _tmp4B1=({const char*_tmp12C="cannot alias pattern expression in datatype";_tag_dyneither(_tmp12C,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp4B2,_tmp4B1,_tag_dyneither(_tmp12B,sizeof(void*),0U));});});else{
# 732
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp4B4=p->loc;struct Cyc_Tcenv_Tenv*_tmp4B3=te;Cyc_Tcenv_add_type_vars(_tmp4B4,_tmp4B3,({struct Cyc_Absyn_Tvar*_tmp12E[1U];_tmp12E[0]=_tmp13C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp12E,sizeof(struct Cyc_Absyn_Tvar*),1U));}));});
({struct Cyc_Tcenv_Tenv*_tmp4B6=({struct Cyc_Tcenv_Tenv*_tmp4B5=te2;Cyc_Tcenv_add_region(_tmp4B5,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=2U,_tmp12D->f1=_tmp13C;_tmp12D;}),0);});te2=_tmp4B6;});
# 735
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp12F->topt),_tmp13C,_tmp13B->type,_tmp12F);}
# 738
break;}}}
# 741
goto _LL0;default: _LLF: _LL10:
 return;}_LL0:;}
# 757 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 759
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp13D=(struct _tuple15*)x->hd;struct _tuple15*_tmp13E=_tmp13D;struct Cyc_Absyn_Vardecl**_tmp148;struct Cyc_Absyn_Exp*_tmp147;_LL1: _tmp148=_tmp13E->f1;_tmp147=_tmp13E->f2;_LL2:;
if(_tmp147 != 0){
struct Cyc_Absyn_Exp*_tmp13F=_tmp147;
# 766
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp13F->topt)) && !Cyc_Tcutil_is_noalias_path(_tmp13F))
# 768
({struct Cyc_String_pa_PrintArg_struct _tmp142=({struct Cyc_String_pa_PrintArg_struct _tmp39F;_tmp39F.tag=0U,({
# 770
struct _dyneither_ptr _tmp4B9=(struct _dyneither_ptr)(_tmp148 != 0?(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp3A0;_tmp3A0.tag=0U,({
# 772
struct _dyneither_ptr _tmp4B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*_tmp148)->name));_tmp3A0.f1=_tmp4B7;});_tmp3A0;});void*_tmp143[1U];_tmp143[0]=& _tmp145;({struct _dyneither_ptr _tmp4B8=({const char*_tmp144="for variable %s";_tag_dyneither(_tmp144,sizeof(char),16U);});Cyc_aprintf(_tmp4B8,_tag_dyneither(_tmp143,sizeof(void*),1U));});}):({const char*_tmp146="";_tag_dyneither(_tmp146,sizeof(char),1U);}));
# 770
_tmp39F.f1=_tmp4B9;});_tmp39F;});void*_tmp140[1U];_tmp140[0]=& _tmp142;({unsigned int _tmp4BB=p->loc;struct _dyneither_ptr _tmp4BA=({const char*_tmp141="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_dyneither(_tmp141,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp4BB,_tmp4BA,_tag_dyneither(_tmp140,sizeof(void*),1U));});});}}};}
# 826 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1U};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2U};
# 836
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0U};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1U};union Cyc_Tcpat_PatOrWhere;struct Cyc_Tcpat_PathNode;struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 850
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp3A1;(_tmp3A1.Name_v).tag=1U,(_tmp3A1.Name_v).val=s;_tmp3A1;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp3A2;(_tmp3A2.Int_v).tag=2U,(_tmp3A2.Int_v).val=i;_tmp3A2;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 867
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp149=c1->name;union Cyc_Tcpat_Name_value _tmp14A=_tmp149;int _tmp152;struct _dyneither_ptr _tmp151;if((_tmp14A.Name_v).tag == 1){_LL1: _tmp151=(_tmp14A.Name_v).val;_LL2: {
# 870
union Cyc_Tcpat_Name_value _tmp14B=c2->name;union Cyc_Tcpat_Name_value _tmp14C=_tmp14B;struct _dyneither_ptr _tmp14D;if((_tmp14C.Name_v).tag == 1){_LL6: _tmp14D=(_tmp14C.Name_v).val;_LL7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp151,(struct _dyneither_ptr)_tmp14D);}else{_LL8: _LL9:
 return - 1;}_LL5:;}}else{_LL3: _tmp152=(_tmp14A.Int_v).val;_LL4: {
# 875
union Cyc_Tcpat_Name_value _tmp14E=c2->name;union Cyc_Tcpat_Name_value _tmp14F=_tmp14E;int _tmp150;if((_tmp14F.Name_v).tag == 1){_LLB: _LLC:
 return 1;}else{_LLD: _tmp150=(_tmp14F.Int_v).val;_LLE:
 return _tmp152 - _tmp150;}_LLA:;}}_LL0:;}
# 883
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 887
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 891
static unsigned int Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned int ans=0U;
struct Cyc_List_List*_tmp153=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp153))->hd)->name)!= 0){
++ ans;
_tmp153=_tmp153->tl;}
# 898
return ans;}
# 901
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp154=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp154 != 0;_tmp154=_tmp154->tl){
struct Cyc_Absyn_Aggrfield*_tmp155=(struct Cyc_Absyn_Aggrfield*)_tmp154->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp155->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 908
({void*_tmp156=0U;({struct _dyneither_ptr _tmp4BD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp3A3;_tmp3A3.tag=0U,_tmp3A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp3A3;});void*_tmp157[1U];_tmp157[0]=& _tmp159;({struct _dyneither_ptr _tmp4BC=({const char*_tmp158="get_member_offset %s failed";_tag_dyneither(_tmp158,sizeof(char),28U);});Cyc_aprintf(_tmp4BC,_tag_dyneither(_tmp157,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4BD,_tag_dyneither(_tmp156,sizeof(void*),0U));});});}
# 911
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp15A=pw;struct Cyc_Absyn_Pat*_tmp189;struct Cyc_Absyn_Exp*_tmp188;if((_tmp15A.where_clause).tag == 2){_LL1: _tmp188=(_tmp15A.where_clause).val;_LL2:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->tag=0U,_tmp15B->f1=_tmp188;_tmp15B;});}else{_LL3: _tmp189=(_tmp15A.pattern).val;_LL4: {
# 915
void*_tmp15C=_tmp189->r;void*_tmp15D=_tmp15C;union Cyc_Absyn_AggrInfo _tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Datatypedecl*_tmp185;struct Cyc_Absyn_Datatypefield*_tmp184;void*_tmp183;struct Cyc_Absyn_Enumfield*_tmp182;struct Cyc_Absyn_Enumdecl*_tmp181;struct Cyc_Absyn_Enumfield*_tmp180;struct _dyneither_ptr _tmp17F;int _tmp17E;char _tmp17D;enum Cyc_Absyn_Sign _tmp17C;int _tmp17B;struct Cyc_Absyn_Pat*_tmp17A;struct Cyc_Absyn_Pat*_tmp179;switch(*((int*)_tmp15D)){case 1U: _LL6: _tmp179=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL7:
 _tmp17A=_tmp179;goto _LL9;case 3U: _LL8: _tmp17A=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL9:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp3A4;(_tmp3A4.pattern).tag=1U,(_tmp3A4.pattern).val=_tmp17A;_tmp3A4;}));case 9U: _LLA: _LLB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10U: _LLC: _tmp17C=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp17B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LLD:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp15E=_cycalloc(sizeof(*_tmp15E));_tmp15E->tag=6U,_tmp15E->f1=(unsigned int)_tmp17B;_tmp15E;});case 11U: _LLE: _tmp17D=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_LLF:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp15F=_cycalloc(sizeof(*_tmp15F));_tmp15F->tag=6U,_tmp15F->f1=(unsigned int)_tmp17D;_tmp15F;});case 12U: _LL10: _tmp17F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp17E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL11:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160->tag=5U,_tmp160->f1=_tmp17F,_tmp160->f2=_tmp17E;_tmp160;});case 13U: _LL12: _tmp181=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp180=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL13:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->tag=3U,_tmp161->f1=_tmp181,_tmp161->f2=_tmp180;_tmp161;});case 14U: _LL14: _tmp183=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp182=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL15:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->tag=4U,_tmp162->f1=_tmp183,_tmp162->f2=_tmp182;_tmp162;});case 6U: _LL16: _LL17:
# 925
{void*_tmp163=Cyc_Tcutil_compress((void*)_check_null(_tmp189->topt));void*_tmp164=_tmp163;void*_tmp165;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp164)->tag == 3U){_LL1F: _tmp165=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp164)->f1).ptr_atts).nullable;_LL20:
# 927
 if(Cyc_Tcutil_force_type2bool(0,_tmp165))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 932
({void*_tmp166=0U;({struct _dyneither_ptr _tmp4BE=({const char*_tmp167="non-null pointer type or non-pointer type in pointer pattern";_tag_dyneither(_tmp167,sizeof(char),61U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4BE,_tag_dyneither(_tmp166,sizeof(void*),0U));});});case 8U: _LL18: _tmp185=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp184=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp15D)->f2;_LL19:
# 934
 if(_tmp185->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->tag=9U,_tmp168->f1=_tmp185,_tmp168->f2=_tmp184;_tmp168;});else{
# 937
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->tag=7U,({int _tmp4BF=(int)Cyc_Tcpat_datatype_tag_number(_tmp185,_tmp184->name);_tmp169->f1=_tmp4BF;}),_tmp169->f2=_tmp185,_tmp169->f3=_tmp184;_tmp169;});}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15D)->f1 != 0){_LL1A: _tmp187=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15D)->f1;_tmp186=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp15D)->f3;_LL1B: {
# 939
struct Cyc_Absyn_Aggrdecl*_tmp16A=Cyc_Absyn_get_known_aggrdecl(_tmp187);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp16A->impl))->tagged){
struct _tuple18*_tmp16B=(struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp186))->hd;struct _tuple18*_tmp16C=_tmp16B;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Pat*_tmp173;_LL24: _tmp174=_tmp16C->f1;_tmp173=_tmp16C->f2;_LL25:;{
struct _dyneither_ptr*f;
{void*_tmp16D=(void*)((struct Cyc_List_List*)_check_null(_tmp174))->hd;void*_tmp16E=_tmp16D;struct _dyneither_ptr*_tmp171;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp16E)->tag == 1U){_LL27: _tmp171=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp16E)->f1;_LL28:
 f=_tmp171;goto _LL26;}else{_LL29: _LL2A:
({void*_tmp16F=0U;({struct _dyneither_ptr _tmp4C0=({const char*_tmp170="no field name in tagged union pattern";_tag_dyneither(_tmp170,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C0,_tag_dyneither(_tmp16F,sizeof(void*),0U));});});}_LL26:;}
# 947
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->tag=8U,_tmp172->f1=f,({int _tmp4C1=Cyc_Tcpat_get_member_offset(_tmp16A,f);_tmp172->f2=_tmp4C1;});_tmp172;});};}else{
# 949
({void*_tmp175=0U;({struct _dyneither_ptr _tmp4C2=({const char*_tmp176="non-tagged aggregate in pattern test";_tag_dyneither(_tmp176,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C2,_tag_dyneither(_tmp175,sizeof(void*),0U));});});}}}else{goto _LL1C;}default: _LL1C: _LL1D:
({void*_tmp177=0U;({struct _dyneither_ptr _tmp4C3=({const char*_tmp178="non-test pattern in pattern test";_tag_dyneither(_tmp178,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C3,_tag_dyneither(_tmp177,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}
# 955
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp3A5;(_tmp3A5.pattern).tag=1U,(_tmp3A5.pattern).val=p;_tmp3A5;});}
# 959
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18B=_cycalloc(sizeof(*_tmp18B));({union Cyc_Tcpat_Name_value _tmp4C5=Cyc_Tcpat_Name_v(({const char*_tmp18A="NULL";_tag_dyneither(_tmp18A,sizeof(char),5U);}));_tmp18B->name=_tmp4C5;}),_tmp18B->arity=0,_tmp18B->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4C4=Cyc_Tcpat_pw(p);_tmp18B->orig_pat=_tmp4C4;});_tmp18B;});}
# 962
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18D=_cycalloc(sizeof(*_tmp18D));({union Cyc_Tcpat_Name_value _tmp4C7=Cyc_Tcpat_Name_v(({const char*_tmp18C="&";_tag_dyneither(_tmp18C,sizeof(char),2U);}));_tmp18D->name=_tmp4C7;}),_tmp18D->arity=1,_tmp18D->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _tmp4C6=Cyc_Tcpat_pw(p);_tmp18D->orig_pat=_tmp4C6;});_tmp18D;});}
# 965
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp18F=_cycalloc(sizeof(*_tmp18F));({union Cyc_Tcpat_Name_value _tmp4C9=Cyc_Tcpat_Name_v(({const char*_tmp18E="&";_tag_dyneither(_tmp18E,sizeof(char),2U);}));_tmp18F->name=_tmp4C9;}),_tmp18F->arity=1,_tmp18F->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _tmp4C8=Cyc_Tcpat_pw(p);_tmp18F->orig_pat=_tmp4C8;});_tmp18F;});}
# 968
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp190=_cycalloc(sizeof(*_tmp190));({union Cyc_Tcpat_Name_value _tmp4CA=Cyc_Tcpat_Int_v(i);_tmp190->name=_tmp4CA;}),_tmp190->arity=0,_tmp190->span=0,_tmp190->orig_pat=p;_tmp190;});}
# 971
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp191=_cycalloc(sizeof(*_tmp191));({union Cyc_Tcpat_Name_value _tmp4CC=Cyc_Tcpat_Name_v(f);_tmp191->name=_tmp4CC;}),_tmp191->arity=0,_tmp191->span=0,({union Cyc_Tcpat_PatOrWhere _tmp4CB=Cyc_Tcpat_pw(p);_tmp191->orig_pat=_tmp4CB;});_tmp191;});}
# 974
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp192=_cycalloc(sizeof(*_tmp192));({union Cyc_Tcpat_Name_value _tmp4CE=Cyc_Tcpat_Int_v((int)c);_tmp192->name=_tmp4CE;}),_tmp192->arity=0,_tmp192->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _tmp4CD=Cyc_Tcpat_pw(p);_tmp192->orig_pat=_tmp4CD;});_tmp192;});}
# 977
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp194=_cycalloc(sizeof(*_tmp194));({union Cyc_Tcpat_Name_value _tmp4CF=Cyc_Tcpat_Name_v(({const char*_tmp193="$";_tag_dyneither(_tmp193,sizeof(char),2U);}));_tmp194->name=_tmp4CF;}),_tmp194->arity=i,_tmp194->span=& Cyc_Tcpat_one_opt,_tmp194->orig_pat=p;_tmp194;});}
# 982
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D0=Cyc_Tcpat_null_con(p);_tmp195->f1=_tmp4D0;}),_tmp195->f2=0;_tmp195;});}
# 985
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D1=Cyc_Tcpat_int_con(i,p);_tmp196->f1=_tmp4D1;}),_tmp196->f2=0;_tmp196;});}
# 988
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp197=_cycalloc(sizeof(*_tmp197));_tmp197->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D2=Cyc_Tcpat_char_con(c,p);_tmp197->f1=_tmp4D2;}),_tmp197->f2=0;_tmp197;});}
# 991
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D3=Cyc_Tcpat_float_con(f,p);_tmp198->f1=_tmp4D3;}),_tmp198->f2=0;_tmp198;});}
# 994
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D5=Cyc_Tcpat_null_ptr_con(p0);_tmp19A->f1=_tmp4D5;}),({struct Cyc_List_List*_tmp4D4=({struct Cyc_List_List*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->hd=p,_tmp199->tl=0;_tmp199;});_tmp19A->f2=_tmp4D4;});_tmp19A;});}
# 997
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D7=Cyc_Tcpat_ptr_con(p0);_tmp19C->f1=_tmp4D7;}),({struct Cyc_List_List*_tmp4D6=({struct Cyc_List_List*_tmp19B=_cycalloc(sizeof(*_tmp19B));_tmp19B->hd=p,_tmp19B->tl=0;_tmp19B;});_tmp19C->f2=_tmp4D6;});_tmp19C;});}
# 1000
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D->tag=1U,({struct Cyc_Tcpat_Con_s*_tmp4D9=({int _tmp4D8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp4D8,p);});_tmp19D->f1=_tmp4D9;}),_tmp19D->f2=ss;_tmp19D;});}
# 1003
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1005
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp19F=_cycalloc(sizeof(*_tmp19F));({union Cyc_Tcpat_Name_value _tmp4DC=Cyc_Tcpat_Name_v(con_name);_tmp19F->name=_tmp4DC;}),({int _tmp4DB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp19F->arity=_tmp4DB;}),_tmp19F->span=span,({union Cyc_Tcpat_PatOrWhere _tmp4DA=Cyc_Tcpat_pw(p);_tmp19F->orig_pat=_tmp4DA;});_tmp19F;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=1U,_tmp19E->f1=c,_tmp19E->f2=ps;_tmp19E;});}
# 1010
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1A0=p->r;void*_tmp1A1=_tmp1A0;void*_tmp1DE;struct Cyc_Absyn_Enumfield*_tmp1DD;struct Cyc_Absyn_Enumdecl*_tmp1DC;struct Cyc_Absyn_Enumfield*_tmp1DB;struct Cyc_Absyn_Aggrdecl*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1D8;struct Cyc_Absyn_Datatypedecl*_tmp1D7;struct Cyc_Absyn_Datatypefield*_tmp1D6;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_Pat*_tmp1D4;struct Cyc_Absyn_Pat*_tmp1D3;struct Cyc_Absyn_Pat*_tmp1D2;struct _dyneither_ptr _tmp1D1;char _tmp1D0;enum Cyc_Absyn_Sign _tmp1CF;int _tmp1CE;switch(*((int*)_tmp1A1)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
({void*_tmp4DD=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=0U;_tmp1A2;});s=_tmp4DD;});goto _LL0;case 9U: _LL7: _LL8:
({void*_tmp4DE=Cyc_Tcpat_null_pat(p);s=_tmp4DE;});goto _LL0;case 10U: _LL9: _tmp1CF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1CE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LLA:
({void*_tmp4E0=({int _tmp4DF=_tmp1CE;Cyc_Tcpat_int_pat(_tmp4DF,Cyc_Tcpat_pw(p));});s=_tmp4E0;});goto _LL0;case 11U: _LLB: _tmp1D0=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LLC:
({void*_tmp4E1=Cyc_Tcpat_char_pat(_tmp1D0,p);s=_tmp4E1;});goto _LL0;case 12U: _LLD: _tmp1D1=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LLE:
({void*_tmp4E2=Cyc_Tcpat_float_pat(_tmp1D1,p);s=_tmp4E2;});goto _LL0;case 1U: _LLF: _tmp1D2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL10:
({void*_tmp4E3=Cyc_Tcpat_compile_pat(_tmp1D2);s=_tmp4E3;});goto _LL0;case 3U: _LL11: _tmp1D3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL12:
({void*_tmp4E4=Cyc_Tcpat_compile_pat(_tmp1D3);s=_tmp4E4;});goto _LL0;case 6U: _LL13: _tmp1D4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LL14:
# 1023
{void*_tmp1A3=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1A4=_tmp1A3;void*_tmp1A7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->tag == 3U){_LL28: _tmp1A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1).ptr_atts).nullable;_LL29: {
# 1025
int is_nullable=Cyc_Tcutil_force_type2bool(0,_tmp1A7);
void*ss=Cyc_Tcpat_compile_pat(_tmp1D4);
if(is_nullable)
({void*_tmp4E5=Cyc_Tcpat_null_ptr_pat(ss,p);s=_tmp4E5;});else{
# 1030
({void*_tmp4E6=Cyc_Tcpat_ptr_pat(ss,p);s=_tmp4E6;});}
goto _LL27;}}else{_LL2A: _LL2B:
({void*_tmp1A5=0U;({struct _dyneither_ptr _tmp4E7=({const char*_tmp1A6="expecting pointertype for pattern!";_tag_dyneither(_tmp1A6,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4E7,_tag_dyneither(_tmp1A5,sizeof(void*),0U));});});}_LL27:;}
# 1034
goto _LL0;case 8U: _LL15: _tmp1D7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1D6=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_tmp1D5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1A1)->f3;_LL16: {
# 1036
int*span;
{void*_tmp1A8=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1A9=_tmp1A8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f1)){case 18U: _LL2D: _LL2E:
# 1039
 if(_tmp1D7->is_extensible)
span=0;else{
# 1042
({int*_tmp4E9=({int*_tmp1AA=_cycalloc_atomic(sizeof(*_tmp1AA));({int _tmp4E8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D7->fields))->v);*_tmp1AA=_tmp4E8;});_tmp1AA;});span=_tmp4E9;});}
goto _LL2C;case 19U: _LL2F: _LL30:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31: _LL32:
({int*_tmp4EB=({void*_tmp1AB=0U;({struct _dyneither_ptr _tmp4EA=({const char*_tmp1AC="void datatype pattern has bad type";_tag_dyneither(_tmp1AC,sizeof(char),35U);});((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EA,_tag_dyneither(_tmp1AB,sizeof(void*),0U));});});span=_tmp4EB;});goto _LL2C;}_LL2C:;}
# 1047
({void*_tmp4EF=({struct _dyneither_ptr _tmp4EE=*(*_tmp1D6->name).f2;int*_tmp4ED=span;struct Cyc_List_List*_tmp4EC=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1D5);Cyc_Tcpat_con_pat(_tmp4EE,_tmp4ED,_tmp4EC,p);});s=_tmp4EF;});
goto _LL0;}case 5U: _LL17: _tmp1D8=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_LL18:
# 1051
({void*_tmp4F1=({struct Cyc_List_List*_tmp4F0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp1D8);Cyc_Tcpat_tuple_pat(_tmp4F0,Cyc_Tcpat_pw(p));});s=_tmp4F1;});goto _LL0;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1)->KnownAggr).tag == 2){_LL19: _tmp1DA=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1)->KnownAggr).val;_tmp1D9=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1A1)->f3;_LL1A:
# 1056
 if(_tmp1DA->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DA->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1060
int found=({struct _dyneither_ptr _tmp4F2=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp4F2,({const char*_tmp1B8="";_tag_dyneither(_tmp1B8,sizeof(char),1U);}));})== 0;
{struct Cyc_List_List*dlps0=_tmp1D9;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple18*_tmp1AD=(struct _tuple18*)dlps0->hd;struct _tuple18*_tmp1AE=_tmp1AD;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_Pat*_tmp1B4;_LL34: _tmp1B5=_tmp1AE->f1;_tmp1B4=_tmp1AE->f2;_LL35:;{
struct Cyc_List_List*_tmp1AF=_tmp1B5;struct _dyneither_ptr*_tmp1B3;if(_tmp1AF != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1AF)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp1AF)->tl == 0){_LL37: _tmp1B3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1AF->hd)->f1;_LL38:
# 1065
 if(Cyc_strptrcmp(_tmp1B3,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
({struct Cyc_List_List*_tmp4F4=({struct Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));({void*_tmp4F3=Cyc_Tcpat_compile_pat(_tmp1B4);_tmp1B0->hd=_tmp4F3;}),_tmp1B0->tl=ps;_tmp1B0;});ps=_tmp4F4;});
found=1;}
# 1069
goto _LL36;}else{goto _LL39;}}else{goto _LL39;}}else{_LL39: _LL3A:
({void*_tmp1B1=0U;({struct _dyneither_ptr _tmp4F5=({const char*_tmp1B2="bad designator(s)";_tag_dyneither(_tmp1B2,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F5,_tag_dyneither(_tmp1B1,sizeof(void*),0U));});});}_LL36:;};}}
# 1073
if(!found)
({void*_tmp1B6=0U;({struct _dyneither_ptr _tmp4F6=({const char*_tmp1B7="bad designator";_tag_dyneither(_tmp1B7,sizeof(char),15U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F6,_tag_dyneither(_tmp1B6,sizeof(void*),0U));});});}}
# 1076
({void*_tmp4F8=({struct Cyc_List_List*_tmp4F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp4F7,Cyc_Tcpat_pw(p));});s=_tmp4F8;});}else{
# 1079
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DA->impl))->tagged)
({void*_tmp1B9=0U;({unsigned int _tmp4FA=p->loc;struct _dyneither_ptr _tmp4F9=({const char*_tmp1BA="patterns on untagged unions not yet supported.";_tag_dyneither(_tmp1BA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp4FA,_tmp4F9,_tag_dyneither(_tmp1B9,sizeof(void*),0U));});});{
int*span=({int*_tmp1C1=_cycalloc_atomic(sizeof(*_tmp1C1));({int _tmp4FB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DA->impl))->fields);*_tmp1C1=_tmp4FB;});_tmp1C1;});
struct Cyc_List_List*_tmp1BB=_tmp1D9;struct _dyneither_ptr*_tmp1C0;struct Cyc_Absyn_Pat*_tmp1BF;if(_tmp1BB != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp1BB)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1BB)->hd)->f1)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp1BB)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp1BB)->tl == 0){_LL3C: _tmp1C0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp1BB->hd)->f1)->hd)->f1;_tmp1BF=((struct _tuple18*)_tmp1BB->hd)->f2;_LL3D:
# 1084
({void*_tmp500=({struct _dyneither_ptr _tmp4FF=*_tmp1C0;int*_tmp4FE=span;struct Cyc_List_List*_tmp4FD=({struct Cyc_List_List*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));({void*_tmp4FC=Cyc_Tcpat_compile_pat(_tmp1BF);_tmp1BC->hd=_tmp4FC;}),_tmp1BC->tl=0;_tmp1BC;});Cyc_Tcpat_con_pat(_tmp4FF,_tmp4FE,_tmp4FD,p);});s=_tmp500;});
goto _LL3B;}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E: _LL3F:
({void*_tmp1BD=0U;({struct _dyneither_ptr _tmp501=({const char*_tmp1BE="bad union pattern";_tag_dyneither(_tmp1BE,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp501,_tag_dyneither(_tmp1BD,sizeof(void*),0U));});});}_LL3B:;};}
# 1089
goto _LL0;}else{goto _LL23;}}else{_LL23: _LL24:
# 1131
 goto _LL26;}case 13U: _LL1B: _tmp1DC=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1DB=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL1C:
# 1094
{void*_tmp1C2=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1C3=_tmp1C2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f1)->tag == 1U){_LL41: _LL42:
# 1098
({void*_tmp502=Cyc_Tcpat_con_pat(*(*_tmp1DB->name).f2,0,0,p);s=_tmp502;});
goto _LL40;}else{goto _LL43;}}else{_LL43: _LL44: {
# 1101
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1DC->fields))->v);
({void*_tmp505=({struct _dyneither_ptr _tmp504=*(*_tmp1DB->name).f2;int*_tmp503=({int*_tmp1C4=_cycalloc_atomic(sizeof(*_tmp1C4));*_tmp1C4=span;_tmp1C4;});Cyc_Tcpat_con_pat(_tmp504,_tmp503,0,p);});s=_tmp505;});
goto _LL40;}}_LL40:;}
# 1105
goto _LL0;case 14U: _LL1D: _tmp1DE=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f1;_tmp1DD=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1A1)->f2;_LL1E: {
# 1110
struct Cyc_List_List*fields;
{void*_tmp1C5=Cyc_Tcutil_compress(_tmp1DE);void*_tmp1C6=_tmp1C5;struct Cyc_List_List*_tmp1C9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C6)->tag == 0U){if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C6)->f1)->tag == 16U){_LL46: _tmp1C9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C6)->f1)->f1;_LL47:
 fields=_tmp1C9;goto _LL45;}else{goto _LL48;}}else{_LL48: _LL49:
({void*_tmp1C7=0U;({struct _dyneither_ptr _tmp506=({const char*_tmp1C8="bad type in AnonEnum_p";_tag_dyneither(_tmp1C8,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp506,_tag_dyneither(_tmp1C7,sizeof(void*),0U));});});}_LL45:;}
# 1117
{void*_tmp1CA=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1CB=_tmp1CA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f1)->tag == 1U){_LL4B: _LL4C:
# 1121
({void*_tmp507=Cyc_Tcpat_con_pat(*(*_tmp1DD->name).f2,0,0,p);s=_tmp507;});
goto _LL4A;}else{goto _LL4D;}}else{_LL4D: _LL4E: {
# 1124
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
({void*_tmp50A=({struct _dyneither_ptr _tmp509=*(*_tmp1DD->name).f2;int*_tmp508=({int*_tmp1CC=_cycalloc_atomic(sizeof(*_tmp1CC));*_tmp1CC=span;_tmp1CC;});Cyc_Tcpat_con_pat(_tmp509,_tmp508,0,p);});s=_tmp50A;});
goto _LL4A;}}_LL4A:;}
# 1128
goto _LL0;}case 15U: _LL1F: _LL20:
 goto _LL22;case 16U: _LL21: _LL22:
 goto _LL24;default: _LL25: _LL26:
# 1132
({void*_tmp50B=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->tag=0U;_tmp1CD;});s=_tmp50B;});}_LL0:;}
# 1134
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1163
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp1DF=({struct _tuple0 _tmp3A6;_tmp3A6.f1=a1,_tmp3A6.f2=a2;_tmp3A6;});struct _tuple0 _tmp1E0=_tmp1DF;int _tmp1EA;struct _dyneither_ptr*_tmp1E9;int _tmp1E8;struct _dyneither_ptr*_tmp1E7;struct Cyc_Absyn_Datatypefield*_tmp1E6;unsigned int _tmp1E5;struct Cyc_Absyn_Datatypefield*_tmp1E4;unsigned int _tmp1E3;unsigned int _tmp1E2;unsigned int _tmp1E1;switch(*((int*)_tmp1E0.f1)){case 0U: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp1E0.f2)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LLB;}case 1U: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp1E0.f2)->tag == 1U){_LL3: _LL4:
 return 1;}else{goto _LLB;}case 2U: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1E0.f2)->tag == 2U){_LL5: _tmp1E2=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1E0.f1)->f1;_tmp1E1=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp1E0.f2)->f1;_LL6:
 return _tmp1E2 == _tmp1E1;}else{goto _LLB;}case 3U: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1E0.f2)->tag == 3U){_LL7: _tmp1E6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1E0.f1)->f2;_tmp1E5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1E0.f1)->f3;_tmp1E4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1E0.f2)->f2;_tmp1E3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp1E0.f2)->f3;_LL8:
# 1169
 return _tmp1E6 == _tmp1E4  && _tmp1E5 == _tmp1E3;}else{goto _LLB;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1E0.f2)->tag == 4U){_LL9: _tmp1EA=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1E0.f1)->f1;_tmp1E9=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1E0.f1)->f2;_tmp1E8=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1E0.f2)->f1;_tmp1E7=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp1E0.f2)->f2;_LLA:
# 1171
 return _tmp1EA == _tmp1E8  && Cyc_strptrcmp(_tmp1E9,_tmp1E7)== 0;}else{_LLB: _LLC:
 return 0;}}_LL0:;}
# 1176
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0  && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1182
if(p1 != p2)return 0;
return 1;}
# 1186
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp1EB=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp1EC=d2;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F1;void*_tmp1F0;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EC)->tag == 2U){_LL1: _tmp1F2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EC)->f1;_tmp1F1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EC)->f2;_tmp1F0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp1EC)->f3;_LL2:
# 1190
 if(Cyc_Tcpat_same_path(access,_tmp1F2) && (int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->tag=2U,_tmp1EF->f1=_tmp1F2,({
struct Cyc_List_List*_tmp50D=({struct Cyc_List_List*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));({struct _tuple0*_tmp50C=({struct _tuple0*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->f1=_tmp1EB,_tmp1ED->f2=d1;_tmp1ED;});_tmp1EE->hd=_tmp50C;}),_tmp1EE->tl=_tmp1F1;_tmp1EE;});_tmp1EF->f2=_tmp50D;}),_tmp1EF->f3=_tmp1F0;_tmp1EF;});else{
# 1194
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1197
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->tag=2U,_tmp1F5->f1=access,({struct Cyc_List_List*_tmp50F=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));({struct _tuple0*_tmp50E=({struct _tuple0*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->f1=_tmp1EB,_tmp1F3->f2=d1;_tmp1F3;});_tmp1F4->hd=_tmp50E;}),_tmp1F4->tl=0;_tmp1F4;});_tmp1F5->f2=_tmp50F;}),_tmp1F5->f3=d2;_tmp1F5;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1208
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0U,Cyc_Tcpat_No  = 1U,Cyc_Tcpat_Maybe  = 2U};
# 1211
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp1F6=0U;({struct Cyc___cycFILE*_tmp511=Cyc_stderr;struct _dyneither_ptr _tmp510=({const char*_tmp1F7=" ";_tag_dyneither(_tmp1F7,sizeof(char),2U);});Cyc_fprintf(_tmp511,_tmp510,_tag_dyneither(_tmp1F6,sizeof(void*),0U));});});}}
# 1217
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp1F8=c->name;
union Cyc_Tcpat_Name_value _tmp1F9=_tmp1F8;int _tmp201;struct _dyneither_ptr _tmp200;if((_tmp1F9.Name_v).tag == 1){_LL1: _tmp200=(_tmp1F9.Name_v).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp1FC=({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0U,_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp200);_tmp3A7;});void*_tmp1FA[1U];_tmp1FA[0]=& _tmp1FC;({struct Cyc___cycFILE*_tmp513=Cyc_stderr;struct _dyneither_ptr _tmp512=({const char*_tmp1FB="%s";_tag_dyneither(_tmp1FB,sizeof(char),3U);});Cyc_fprintf(_tmp513,_tmp512,_tag_dyneither(_tmp1FA,sizeof(void*),1U));});});goto _LL0;}else{_LL3: _tmp201=(_tmp1F9.Int_v).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp1FF=({struct Cyc_Int_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=1U,_tmp3A8.f1=(unsigned long)_tmp201;_tmp3A8;});void*_tmp1FD[1U];_tmp1FD[0]=& _tmp1FF;({struct Cyc___cycFILE*_tmp515=Cyc_stderr;struct _dyneither_ptr _tmp514=({const char*_tmp1FE="%d";_tag_dyneither(_tmp1FE,sizeof(char),3U);});Cyc_fprintf(_tmp515,_tmp514,_tag_dyneither(_tmp1FD,sizeof(void*),1U));});});goto _LL0;}_LL0:;}
# 1225
static void Cyc_Tcpat_print_access(void*a){
void*_tmp202=a;int _tmp218;struct _dyneither_ptr*_tmp217;struct Cyc_Absyn_Datatypefield*_tmp216;unsigned int _tmp215;unsigned int _tmp214;switch(*((int*)_tmp202)){case 0U: _LL1: _LL2:
({void*_tmp203=0U;({struct Cyc___cycFILE*_tmp517=Cyc_stderr;struct _dyneither_ptr _tmp516=({const char*_tmp204="DUMMY";_tag_dyneither(_tmp204,sizeof(char),6U);});Cyc_fprintf(_tmp517,_tmp516,_tag_dyneither(_tmp203,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp205=0U;({struct Cyc___cycFILE*_tmp519=Cyc_stderr;struct _dyneither_ptr _tmp518=({const char*_tmp206="*";_tag_dyneither(_tmp206,sizeof(char),2U);});Cyc_fprintf(_tmp519,_tmp518,_tag_dyneither(_tmp205,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp214=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp202)->f1;_LL6:
({struct Cyc_Int_pa_PrintArg_struct _tmp209=({struct Cyc_Int_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=1U,_tmp3A9.f1=(unsigned long)((int)_tmp214);_tmp3A9;});void*_tmp207[1U];_tmp207[0]=& _tmp209;({struct Cyc___cycFILE*_tmp51B=Cyc_stderr;struct _dyneither_ptr _tmp51A=({const char*_tmp208="[%d]";_tag_dyneither(_tmp208,sizeof(char),5U);});Cyc_fprintf(_tmp51B,_tmp51A,_tag_dyneither(_tmp207,sizeof(void*),1U));});});goto _LL0;case 3U: _LL7: _tmp216=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp202)->f2;_tmp215=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp202)->f3;_LL8:
# 1231
({struct Cyc_String_pa_PrintArg_struct _tmp20C=({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0U,({struct _dyneither_ptr _tmp51C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp216->name));_tmp3AB.f1=_tmp51C;});_tmp3AB;});struct Cyc_Int_pa_PrintArg_struct _tmp20D=({struct Cyc_Int_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=1U,_tmp3AA.f1=(unsigned long)((int)_tmp215);_tmp3AA;});void*_tmp20A[2U];_tmp20A[0]=& _tmp20C,_tmp20A[1]=& _tmp20D;({struct Cyc___cycFILE*_tmp51E=Cyc_stderr;struct _dyneither_ptr _tmp51D=({const char*_tmp20B="%s[%d]";_tag_dyneither(_tmp20B,sizeof(char),7U);});Cyc_fprintf(_tmp51E,_tmp51D,_tag_dyneither(_tmp20A,sizeof(void*),2U));});});goto _LL0;default: _LL9: _tmp218=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp202)->f1;_tmp217=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp202)->f2;_LLA:
# 1233
 if(_tmp218)
({struct Cyc_String_pa_PrintArg_struct _tmp210=({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0U,_tmp3AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp217);_tmp3AC;});void*_tmp20E[1U];_tmp20E[0]=& _tmp210;({struct Cyc___cycFILE*_tmp520=Cyc_stderr;struct _dyneither_ptr _tmp51F=({const char*_tmp20F=".tagunion.%s";_tag_dyneither(_tmp20F,sizeof(char),13U);});Cyc_fprintf(_tmp520,_tmp51F,_tag_dyneither(_tmp20E,sizeof(void*),1U));});});else{
# 1236
({struct Cyc_String_pa_PrintArg_struct _tmp213=({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0U,_tmp3AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp217);_tmp3AD;});void*_tmp211[1U];_tmp211[0]=& _tmp213;({struct Cyc___cycFILE*_tmp522=Cyc_stderr;struct _dyneither_ptr _tmp521=({const char*_tmp212=".%s";_tag_dyneither(_tmp212,sizeof(char),4U);});Cyc_fprintf(_tmp522,_tmp521,_tag_dyneither(_tmp211,sizeof(void*),1U));});});}
goto _LL0;}_LL0:;}
# 1241
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp219=p;struct Cyc_Absyn_Datatypefield*_tmp23E;struct _dyneither_ptr*_tmp23D;int _tmp23C;int _tmp23B;unsigned int _tmp23A;struct _dyneither_ptr _tmp239;struct Cyc_Absyn_Enumfield*_tmp238;struct Cyc_Absyn_Enumfield*_tmp237;struct Cyc_Absyn_Exp*_tmp236;switch(*((int*)_tmp219)){case 0U: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp219)->f1 == 0){_LL1: _LL2:
({void*_tmp21A=0U;({struct Cyc___cycFILE*_tmp524=Cyc_stderr;struct _dyneither_ptr _tmp523=({const char*_tmp21B="where(NULL)";_tag_dyneither(_tmp21B,sizeof(char),12U);});Cyc_fprintf(_tmp524,_tmp523,_tag_dyneither(_tmp21A,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp236=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp219)->f1;_LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp21E=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0U,({struct _dyneither_ptr _tmp525=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp236)));_tmp3AE.f1=_tmp525;});_tmp3AE;});void*_tmp21C[1U];_tmp21C[0]=& _tmp21E;({struct Cyc___cycFILE*_tmp527=Cyc_stderr;struct _dyneither_ptr _tmp526=({const char*_tmp21D="where(%s)";_tag_dyneither(_tmp21D,sizeof(char),10U);});Cyc_fprintf(_tmp527,_tmp526,_tag_dyneither(_tmp21C,sizeof(void*),1U));});});goto _LL0;}case 1U: _LL5: _LL6:
({void*_tmp21F=0U;({struct Cyc___cycFILE*_tmp529=Cyc_stderr;struct _dyneither_ptr _tmp528=({const char*_tmp220="NULL";_tag_dyneither(_tmp220,sizeof(char),5U);});Cyc_fprintf(_tmp529,_tmp528,_tag_dyneither(_tmp21F,sizeof(void*),0U));});});goto _LL0;case 2U: _LL7: _LL8:
({void*_tmp221=0U;({struct Cyc___cycFILE*_tmp52B=Cyc_stderr;struct _dyneither_ptr _tmp52A=({const char*_tmp222="NOT-NULL:";_tag_dyneither(_tmp222,sizeof(char),10U);});Cyc_fprintf(_tmp52B,_tmp52A,_tag_dyneither(_tmp221,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp237=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp219)->f2;_LLA:
 _tmp238=_tmp237;goto _LLC;case 3U: _LLB: _tmp238=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp219)->f2;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp225=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0U,({struct _dyneither_ptr _tmp52C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp238->name));_tmp3AF.f1=_tmp52C;});_tmp3AF;});void*_tmp223[1U];_tmp223[0]=& _tmp225;({struct Cyc___cycFILE*_tmp52E=Cyc_stderr;struct _dyneither_ptr _tmp52D=({const char*_tmp224="%s";_tag_dyneither(_tmp224,sizeof(char),3U);});Cyc_fprintf(_tmp52E,_tmp52D,_tag_dyneither(_tmp223,sizeof(void*),1U));});});goto _LL0;case 5U: _LLD: _tmp239=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp219)->f1;_LLE:
({struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0U,_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp239);_tmp3B0;});void*_tmp226[1U];_tmp226[0]=& _tmp228;({struct Cyc___cycFILE*_tmp530=Cyc_stderr;struct _dyneither_ptr _tmp52F=({const char*_tmp227="%s";_tag_dyneither(_tmp227,sizeof(char),3U);});Cyc_fprintf(_tmp530,_tmp52F,_tag_dyneither(_tmp226,sizeof(void*),1U));});});goto _LL0;case 6U: _LLF: _tmp23A=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp219)->f1;_LL10:
({struct Cyc_Int_pa_PrintArg_struct _tmp22B=({struct Cyc_Int_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=1U,_tmp3B1.f1=(unsigned long)((int)_tmp23A);_tmp3B1;});void*_tmp229[1U];_tmp229[0]=& _tmp22B;({struct Cyc___cycFILE*_tmp532=Cyc_stderr;struct _dyneither_ptr _tmp531=({const char*_tmp22A="%d";_tag_dyneither(_tmp22A,sizeof(char),3U);});Cyc_fprintf(_tmp532,_tmp531,_tag_dyneither(_tmp229,sizeof(void*),1U));});});goto _LL0;case 7U: _LL11: _tmp23B=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp219)->f1;_LL12:
({struct Cyc_Int_pa_PrintArg_struct _tmp22E=({struct Cyc_Int_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=1U,_tmp3B2.f1=(unsigned long)_tmp23B;_tmp3B2;});void*_tmp22C[1U];_tmp22C[0]=& _tmp22E;({struct Cyc___cycFILE*_tmp534=Cyc_stderr;struct _dyneither_ptr _tmp533=({const char*_tmp22D="datatypetag(%d)";_tag_dyneither(_tmp22D,sizeof(char),16U);});Cyc_fprintf(_tmp534,_tmp533,_tag_dyneither(_tmp22C,sizeof(void*),1U));});});goto _LL0;case 8U: _LL13: _tmp23D=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp219)->f1;_tmp23C=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp219)->f2;_LL14:
({struct Cyc_String_pa_PrintArg_struct _tmp231=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0U,_tmp3B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp23D);_tmp3B4;});struct Cyc_Int_pa_PrintArg_struct _tmp232=({struct Cyc_Int_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=1U,_tmp3B3.f1=(unsigned long)_tmp23C;_tmp3B3;});void*_tmp22F[2U];_tmp22F[0]=& _tmp231,_tmp22F[1]=& _tmp232;({struct Cyc___cycFILE*_tmp536=Cyc_stderr;struct _dyneither_ptr _tmp535=({const char*_tmp230="uniontag[%s](%d)";_tag_dyneither(_tmp230,sizeof(char),17U);});Cyc_fprintf(_tmp536,_tmp535,_tag_dyneither(_tmp22F,sizeof(void*),2U));});});goto _LL0;default: _LL15: _tmp23E=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp219)->f2;_LL16:
# 1254
({struct Cyc_String_pa_PrintArg_struct _tmp235=({struct Cyc_String_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=0U,({struct _dyneither_ptr _tmp537=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp23E->name));_tmp3B5.f1=_tmp537;});_tmp3B5;});void*_tmp233[1U];_tmp233[0]=& _tmp235;({struct Cyc___cycFILE*_tmp539=Cyc_stderr;struct _dyneither_ptr _tmp538=({const char*_tmp234="datatypefield(%s)";_tag_dyneither(_tmp234,sizeof(char),18U);});Cyc_fprintf(_tmp539,_tmp538,_tag_dyneither(_tmp233,sizeof(void*),1U));});});}_LL0:;}
# 1258
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp241=({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0U,({struct _dyneither_ptr _tmp53A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp3B6.f1=_tmp53A;});_tmp3B6;});void*_tmp23F[1U];_tmp23F[0]=& _tmp241;({struct Cyc___cycFILE*_tmp53C=Cyc_stderr;struct _dyneither_ptr _tmp53B=({const char*_tmp240="%s";_tag_dyneither(_tmp240,sizeof(char),3U);});Cyc_fprintf(_tmp53C,_tmp53B,_tag_dyneither(_tmp23F,sizeof(void*),1U));});});}
# 1262
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp242=d;struct Cyc_List_List*_tmp25E;struct Cyc_List_List*_tmp25D;void*_tmp25C;struct Cyc_Tcpat_Rhs*_tmp25B;switch(*((int*)_tmp242)){case 1U: _LL1: _tmp25B=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp242)->f1;_LL2:
# 1265
 Cyc_Tcpat_print_tab(tab);
({void*_tmp243=0U;({struct Cyc___cycFILE*_tmp53E=Cyc_stderr;struct _dyneither_ptr _tmp53D=({const char*_tmp244="Success(";_tag_dyneither(_tmp244,sizeof(char),9U);});Cyc_fprintf(_tmp53E,_tmp53D,_tag_dyneither(_tmp243,sizeof(void*),0U));});});Cyc_Tcpat_print_rhs(_tmp25B);({void*_tmp245=0U;({struct Cyc___cycFILE*_tmp540=Cyc_stderr;struct _dyneither_ptr _tmp53F=({const char*_tmp246=")\n";_tag_dyneither(_tmp246,sizeof(char),3U);});Cyc_fprintf(_tmp540,_tmp53F,_tag_dyneither(_tmp245,sizeof(void*),0U));});});
goto _LL0;case 0U: _LL3: _LL4:
({void*_tmp247=0U;({struct Cyc___cycFILE*_tmp542=Cyc_stderr;struct _dyneither_ptr _tmp541=({const char*_tmp248="Failure\n";_tag_dyneither(_tmp248,sizeof(char),9U);});Cyc_fprintf(_tmp542,_tmp541,_tag_dyneither(_tmp247,sizeof(void*),0U));});});goto _LL0;default: _LL5: _tmp25E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp242)->f1;_tmp25D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp242)->f2;_tmp25C=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp242)->f3;_LL6:
# 1270
 Cyc_Tcpat_print_tab(tab);
({void*_tmp249=0U;({struct Cyc___cycFILE*_tmp544=Cyc_stderr;struct _dyneither_ptr _tmp543=({const char*_tmp24A="Switch[";_tag_dyneither(_tmp24A,sizeof(char),8U);});Cyc_fprintf(_tmp544,_tmp543,_tag_dyneither(_tmp249,sizeof(void*),0U));});});
for(0;_tmp25E != 0;_tmp25E=_tmp25E->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp25E->hd)->access);
if(_tmp25E->tl != 0)({void*_tmp24B=0U;({struct Cyc___cycFILE*_tmp546=Cyc_stderr;struct _dyneither_ptr _tmp545=({const char*_tmp24C=",";_tag_dyneither(_tmp24C,sizeof(char),2U);});Cyc_fprintf(_tmp546,_tmp545,_tag_dyneither(_tmp24B,sizeof(void*),0U));});});}
# 1276
({void*_tmp24D=0U;({struct Cyc___cycFILE*_tmp548=Cyc_stderr;struct _dyneither_ptr _tmp547=({const char*_tmp24E="] {\n";_tag_dyneither(_tmp24E,sizeof(char),5U);});Cyc_fprintf(_tmp548,_tmp547,_tag_dyneither(_tmp24D,sizeof(void*),0U));});});
for(0;_tmp25D != 0;_tmp25D=_tmp25D->tl){
struct _tuple0 _tmp24F=*((struct _tuple0*)_tmp25D->hd);struct _tuple0 _tmp250=_tmp24F;void*_tmp256;void*_tmp255;_LL8: _tmp256=_tmp250.f1;_tmp255=_tmp250.f2;_LL9:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp251=0U;({struct Cyc___cycFILE*_tmp54A=Cyc_stderr;struct _dyneither_ptr _tmp549=({const char*_tmp252="case ";_tag_dyneither(_tmp252,sizeof(char),6U);});Cyc_fprintf(_tmp54A,_tmp549,_tag_dyneither(_tmp251,sizeof(void*),0U));});});
Cyc_Tcpat_print_pat_test(_tmp256);
({void*_tmp253=0U;({struct Cyc___cycFILE*_tmp54C=Cyc_stderr;struct _dyneither_ptr _tmp54B=({const char*_tmp254=":\n";_tag_dyneither(_tmp254,sizeof(char),3U);});Cyc_fprintf(_tmp54C,_tmp54B,_tag_dyneither(_tmp253,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp255,tab + 7);}
# 1285
Cyc_Tcpat_print_tab(tab);
({void*_tmp257=0U;({struct Cyc___cycFILE*_tmp54E=Cyc_stderr;struct _dyneither_ptr _tmp54D=({const char*_tmp258="default:\n";_tag_dyneither(_tmp258,sizeof(char),10U);});Cyc_fprintf(_tmp54E,_tmp54D,_tag_dyneither(_tmp257,sizeof(void*),0U));});});
Cyc_Tcpat_print_dec_tree(_tmp25C,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp259=0U;({struct Cyc___cycFILE*_tmp550=Cyc_stderr;struct _dyneither_ptr _tmp54F=({const char*_tmp25A="}\n";_tag_dyneither(_tmp25A,sizeof(char),3U);});Cyc_fprintf(_tmp550,_tmp54F,_tag_dyneither(_tmp259,sizeof(void*),0U));});});}_LL0:;}
# 1293
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1301
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp25F=td;struct Cyc_Set_Set*_tmp263;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp25F)->tag == 1U){_LL1: _tmp263=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp25F)->f1;_LL2:
# 1311
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260->tag=1U,({struct Cyc_Set_Set*_tmp551=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp263,c);_tmp260->f1=_tmp551;});_tmp260;});}else{_LL3: _LL4:
({void*_tmp261=0U;({struct _dyneither_ptr _tmp552=({const char*_tmp262="add_neg called when td is Positive";_tag_dyneither(_tmp262,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp552,_tag_dyneither(_tmp261,sizeof(void*),0U));});});}_LL0:;}
# 1318
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp264=td;struct Cyc_Set_Set*_tmp266;struct Cyc_Tcpat_Con_s*_tmp265;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp264)->tag == 0U){_LL1: _tmp265=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp264)->f1;_LL2:
# 1322
 if(Cyc_Tcpat_compare_con(c,_tmp265)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL3: _tmp266=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp264)->f1;_LL4:
# 1326
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp266,c))return Cyc_Tcpat_No;else{
# 1329
if(c->span != 0  && ({int _tmp553=*((int*)_check_null(c->span));_tmp553 == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp266)+ 1;}))
return Cyc_Tcpat_Yes;else{
# 1332
return Cyc_Tcpat_Maybe;}}}_LL0:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1340
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp267=ctxt;struct Cyc_Tcpat_Con_s*_tmp26D;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26B;if(_tmp267 == 0){_LL1: _LL2:
 return 0;}else{_LL3: _tmp26D=((struct _tuple21*)_tmp267->hd)->f1;_tmp26C=((struct _tuple21*)_tmp267->hd)->f2;_tmp26B=_tmp267->tl;_LL4:
# 1344
 return({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));
({struct _tuple21*_tmp555=({struct _tuple21*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->f1=_tmp26D,({struct Cyc_List_List*_tmp554=({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->hd=dsc,_tmp268->tl=_tmp26C;_tmp268;});_tmp269->f2=_tmp554;});_tmp269;});_tmp26A->hd=_tmp555;}),_tmp26A->tl=_tmp26B;_tmp26A;});}_LL0:;}
# 1352
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp26E=ctxt;struct Cyc_Tcpat_Con_s*_tmp274;struct Cyc_List_List*_tmp273;struct Cyc_List_List*_tmp272;if(_tmp26E == 0){_LL1: _LL2:
({void*_tmp26F=0U;({struct _dyneither_ptr _tmp556=({const char*_tmp270="norm_context: empty context";_tag_dyneither(_tmp270,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp556,_tag_dyneither(_tmp26F,sizeof(void*),0U));});});}else{_LL3: _tmp274=((struct _tuple21*)_tmp26E->hd)->f1;_tmp273=((struct _tuple21*)_tmp26E->hd)->f2;_tmp272=_tmp26E->tl;_LL4:
# 1356
 return({struct Cyc_List_List*_tmp558=_tmp272;Cyc_Tcpat_augment(_tmp558,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271->tag=0U,_tmp271->f1=_tmp274,({struct Cyc_List_List*_tmp557=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp273);_tmp271->f2=_tmp557;});_tmp271;}));});}_LL0:;}
# 1365
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1367
struct _tuple1 _tmp275=({struct _tuple1 _tmp3B7;_tmp3B7.f1=ctxt,_tmp3B7.f2=work;_tmp3B7;});struct _tuple1 _tmp276=_tmp275;struct Cyc_Tcpat_Con_s*_tmp280;struct Cyc_List_List*_tmp27F;struct Cyc_List_List*_tmp27E;struct Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp27C;if(_tmp276.f1 == 0){if(_tmp276.f2 == 0){_LL1: _LL2:
 return dsc;}else{_LL3: _LL4:
 goto _LL6;}}else{if(_tmp276.f2 == 0){_LL5: _LL6:
({void*_tmp277=0U;({struct _dyneither_ptr _tmp559=({const char*_tmp278="build_desc: ctxt and work don't match";_tag_dyneither(_tmp278,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp559,_tag_dyneither(_tmp277,sizeof(void*),0U));});});}else{_LL7: _tmp280=((struct _tuple21*)(_tmp276.f1)->hd)->f1;_tmp27F=((struct _tuple21*)(_tmp276.f1)->hd)->f2;_tmp27E=(_tmp276.f1)->tl;_tmp27D=((struct _tuple20*)(_tmp276.f2)->hd)->f3;_tmp27C=(_tmp276.f2)->tl;_LL8: {
# 1372
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp279=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B->tag=0U,_tmp27B->f1=_tmp280,({struct Cyc_List_List*_tmp55B=({struct Cyc_List_List*_tmp55A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp27F);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp55A,({struct Cyc_List_List*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A->hd=dsc,_tmp27A->tl=_tmp27D;_tmp27A;}));});_tmp27B->f2=_tmp55B;});_tmp27B;});
return Cyc_Tcpat_build_desc(_tmp27E,(void*)_tmp279,_tmp27C);}}}_LL0:;}
# 1377
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1384
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp281=allmrules;void*_tmp285;struct Cyc_Tcpat_Rhs*_tmp284;struct Cyc_List_List*_tmp283;if(_tmp281 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->tag=0U,_tmp282->f1=dsc;_tmp282;});}else{_LL3: _tmp285=((struct _tuple22*)_tmp281->hd)->f1;_tmp284=((struct _tuple22*)_tmp281->hd)->f2;_tmp283=_tmp281->tl;_LL4:
# 1388
 return Cyc_Tcpat_match(_tmp285,0,dsc,0,0,_tmp284,_tmp283);}_LL0:;}
# 1393
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp55D=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->tag=1U,({struct Cyc_Set_Set*_tmp55C=Cyc_Tcpat_empty_con_set();_tmp286->f1=_tmp55C;});_tmp286;});Cyc_Tcpat_or_match(_tmp55D,allmrules);});}
# 1400
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1403
struct Cyc_List_List*_tmp287=work;struct Cyc_List_List*_tmp29E;struct Cyc_List_List*_tmp29D;struct Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29A;if(_tmp287 == 0){_LL1: _LL2:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->tag=1U,_tmp288->f1=right_hand_side;_tmp288;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp287)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp287)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp287)->hd)->f3 == 0){_LL3: _tmp29A=_tmp287->tl;_LL4:
# 1406
 return({struct Cyc_List_List*_tmp560=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp55F=_tmp29A;struct Cyc_Tcpat_Rhs*_tmp55E=right_hand_side;Cyc_Tcpat_and_match(_tmp560,_tmp55F,_tmp55E,rules);});}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _tmp29E=((struct _tuple20*)_tmp287->hd)->f1;_tmp29D=((struct _tuple20*)_tmp287->hd)->f2;_tmp29C=((struct _tuple20*)_tmp287->hd)->f3;_tmp29B=_tmp287->tl;_LL6:
# 1408
 if((_tmp29E == 0  || _tmp29D == 0) || _tmp29C == 0)
({void*_tmp289=0U;({struct _dyneither_ptr _tmp561=({const char*_tmp28A="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp28A,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp561,_tag_dyneither(_tmp289,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp28B=_tmp29E;struct Cyc_List_List*_tmp28C=_tmp28B;void*_tmp299;struct Cyc_List_List*_tmp298;_LL8: _tmp299=(void*)_tmp28C->hd;_tmp298=_tmp28C->tl;_LL9:;{
struct Cyc_List_List*_tmp28D=_tmp29D;struct Cyc_List_List*_tmp28E=_tmp28D;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp296;_LLB: _tmp297=(struct Cyc_List_List*)_tmp28E->hd;_tmp296=_tmp28E->tl;_LLC:;{
struct Cyc_List_List*_tmp28F=_tmp29C;struct Cyc_List_List*_tmp290=_tmp28F;void*_tmp295;struct Cyc_List_List*_tmp294;_LLE: _tmp295=(void*)_tmp290->hd;_tmp294=_tmp290->tl;_LLF:;{
struct _tuple20*_tmp291=({struct _tuple20*_tmp293=_cycalloc(sizeof(*_tmp293));_tmp293->f1=_tmp298,_tmp293->f2=_tmp296,_tmp293->f3=_tmp294;_tmp293;});
return({void*_tmp567=_tmp299;struct Cyc_List_List*_tmp566=_tmp297;void*_tmp565=_tmp295;struct Cyc_List_List*_tmp564=ctx;struct Cyc_List_List*_tmp563=({struct Cyc_List_List*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->hd=_tmp291,_tmp292->tl=_tmp29B;_tmp292;});struct Cyc_Tcpat_Rhs*_tmp562=right_hand_side;Cyc_Tcpat_match(_tmp567,_tmp566,_tmp565,_tmp564,_tmp563,_tmp562,rules);});};};};};}}_LL0:;}
# 1419
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp29F=dsc;struct Cyc_List_List*_tmp2A4;struct Cyc_Set_Set*_tmp2A3;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp29F)->tag == 1U){_LL1: _tmp2A3=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp29F)->f1;_LL2: {
# 1425
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->tag=1U,({struct Cyc_Set_Set*_tmp568=Cyc_Tcpat_empty_con_set();_tmp2A2->f1=_tmp568;});_tmp2A2;});
struct Cyc_List_List*_tmp2A0=0;
{int i=0;for(0;i < pcon->arity;++ i){
({struct Cyc_List_List*_tmp569=({struct Cyc_List_List*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->hd=any,_tmp2A1->tl=_tmp2A0;_tmp2A1;});_tmp2A0=_tmp569;});}}
return _tmp2A0;}}else{_LL3: _tmp2A4=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp29F)->f2;_LL4:
 return _tmp2A4;}_LL0:;}
# 1434
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2A5=pw;struct Cyc_Absyn_Pat*_tmp2C2;if((_tmp2A5.where_clause).tag == 2){_LL1: _LL2:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL3: _tmp2C2=(_tmp2A5.pattern).val;_LL4: {
# 1438
void*_tmp2A6=_tmp2C2->r;void*_tmp2A7=_tmp2A6;union Cyc_Absyn_AggrInfo _tmp2C1;struct Cyc_List_List*_tmp2C0;struct Cyc_Absyn_Datatypedecl*_tmp2BF;struct Cyc_Absyn_Datatypefield*_tmp2BE;switch(*((int*)_tmp2A7)){case 6U: _LL6: _LL7:
# 1440
 if(i != 0)
({void*_tmp2A8=0U;({struct _dyneither_ptr _tmp56B=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2AB=({struct Cyc_Int_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=1U,_tmp3B8.f1=(unsigned long)i;_tmp3B8;});void*_tmp2A9[1U];_tmp2A9[0]=& _tmp2AB;({struct _dyneither_ptr _tmp56A=({const char*_tmp2AA="get_access on pointer pattern with offset %d\n";_tag_dyneither(_tmp2AA,sizeof(char),46U);});Cyc_aprintf(_tmp56A,_tag_dyneither(_tmp2A9,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp56B,_tag_dyneither(_tmp2A8,sizeof(void*),0U));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5U: _LL8: _LL9:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));_tmp2AC->tag=2U,_tmp2AC->f1=(unsigned int)i;_tmp2AC;});case 8U: _LLA: _tmp2BF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2A7)->f1;_tmp2BE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2A7)->f2;_LLB:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp2AD=_cycalloc(sizeof(*_tmp2AD));_tmp2AD->tag=3U,_tmp2AD->f1=_tmp2BF,_tmp2AD->f2=_tmp2BE,_tmp2AD->f3=(unsigned int)i;_tmp2AD;});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A7)->f1 != 0){_LLC: _tmp2C1=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A7)->f1;_tmp2C0=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2A7)->f3;_LLD: {
# 1446
struct Cyc_Absyn_Aggrdecl*_tmp2AE=Cyc_Absyn_get_known_aggrdecl(_tmp2C1);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AE->impl))->tagged){
struct Cyc_List_List*_tmp2AF=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp2C0))->hd)).f1;struct Cyc_List_List*_tmp2B0=_tmp2AF;struct _dyneither_ptr*_tmp2B6;if(_tmp2B0 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp2B0)->hd)->tag == 1U){if(((struct Cyc_List_List*)_tmp2B0)->tl == 0){_LL11: _tmp2B6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2B0->hd)->f1;_LL12:
# 1450
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->tag=4U,_tmp2B1->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AE->impl))->tagged,_tmp2B1->f2=_tmp2B6;_tmp2B1;});}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _LL14:
({void*_tmp2B2=0U;({struct _dyneither_ptr _tmp56E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2B5=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0U,({struct _dyneither_ptr _tmp56C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp2C2));_tmp3B9.f1=_tmp56C;});_tmp3B9;});void*_tmp2B3[1U];_tmp2B3[0]=& _tmp2B5;({struct _dyneither_ptr _tmp56D=({const char*_tmp2B4="get_access on bad aggr pattern: %s";_tag_dyneither(_tmp2B4,sizeof(char),35U);});Cyc_aprintf(_tmp56D,_tag_dyneither(_tmp2B3,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp56E,_tag_dyneither(_tmp2B2,sizeof(void*),0U));});});}_LL10:;}{
# 1454
struct Cyc_List_List*_tmp2B7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AE->impl))->fields;
int _tmp2B8=i;
for(0;i != 0;-- i){_tmp2B7=((struct Cyc_List_List*)_check_null(_tmp2B7))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));_tmp2B9->tag=4U,_tmp2B9->f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AE->impl))->tagged,_tmp2B9->f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2B7))->hd)->name;_tmp2B9;});};}}else{goto _LLE;}default: _LLE: _LLF:
({void*_tmp2BA=0U;({struct _dyneither_ptr _tmp571=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0U,({struct _dyneither_ptr _tmp56F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp2C2));_tmp3BA.f1=_tmp56F;});_tmp3BA;});void*_tmp2BB[1U];_tmp2BB[0]=& _tmp2BD;({struct _dyneither_ptr _tmp570=({const char*_tmp2BC="get_access on bad pattern: %s";_tag_dyneither(_tmp2BC,sizeof(char),30U);});Cyc_aprintf(_tmp570,_tag_dyneither(_tmp2BB,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp571,_tag_dyneither(_tmp2BA,sizeof(void*),0U));});});}_LL5:;}}_LL0:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1464
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp2C3=env;struct Cyc_List_List*_tmp2C7;struct Cyc_Tcpat_Con_s*_tmp2C6;_LL1: _tmp2C7=_tmp2C3->f1;_tmp2C6=_tmp2C3->f2;_LL2:;{
void*acc=Cyc_Tcpat_get_access(_tmp2C6->orig_pat,i);
return({struct Cyc_List_List*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));({struct Cyc_Tcpat_PathNode*_tmp572=({struct Cyc_Tcpat_PathNode*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4->orig_pat=_tmp2C6->orig_pat,_tmp2C4->access=acc;_tmp2C4;});_tmp2C5->hd=_tmp572;}),_tmp2C5->tl=_tmp2C7;_tmp2C5;});};}
# 1469
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp2C8=({struct _tuple23 _tmp3BB;_tmp3BB.f1=obj,_tmp3BB.f2=pcon;_tmp3BB;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp2C8);}
# 1477
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1481
void*_tmp2C9=p;struct Cyc_Tcpat_Con_s*_tmp2D7;struct Cyc_List_List*_tmp2D6;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2C9)->tag == 0U){_LL1: _LL2:
# 1483
 return({struct Cyc_List_List*_tmp575=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp574=work;struct Cyc_Tcpat_Rhs*_tmp573=right_hand_side;Cyc_Tcpat_and_match(_tmp575,_tmp574,_tmp573,rules);});}else{_LL3: _tmp2D7=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2C9)->f1;_tmp2D6=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2C9)->f2;_LL4: {
# 1485
enum Cyc_Tcpat_Answer _tmp2CA=Cyc_Tcpat_static_match(_tmp2D7,dsc);enum Cyc_Tcpat_Answer _tmp2CB=_tmp2CA;switch(_tmp2CB){case Cyc_Tcpat_Yes: _LL6: _LL7: {
# 1487
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));({struct _tuple21*_tmp576=({struct _tuple21*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->f1=_tmp2D7,_tmp2CE->f2=0;_tmp2CE;});_tmp2CF->hd=_tmp576;}),_tmp2CF->tl=ctx;_tmp2CF;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->f1=_tmp2D6,({struct Cyc_List_List*_tmp578=Cyc_Tcpat_getoargs(_tmp2D7,obj);_tmp2CD->f2=_tmp578;}),({
struct Cyc_List_List*_tmp577=Cyc_Tcpat_getdargs(_tmp2D7,dsc);_tmp2CD->f3=_tmp577;});_tmp2CD;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->hd=work_frame,_tmp2CC->tl=work;_tmp2CC;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL8: _LL9:
# 1493
 return({void*_tmp579=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp579,rules);});default: _LLA: _LLB: {
# 1495
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));({struct _tuple21*_tmp57A=({struct _tuple21*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->f1=_tmp2D7,_tmp2D4->f2=0;_tmp2D4;});_tmp2D5->hd=_tmp57A;}),_tmp2D5->tl=ctx;_tmp2D5;});
struct _tuple20*work_frame=({struct _tuple20*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->f1=_tmp2D6,({struct Cyc_List_List*_tmp57C=Cyc_Tcpat_getoargs(_tmp2D7,obj);_tmp2D3->f2=_tmp57C;}),({
struct Cyc_List_List*_tmp57B=Cyc_Tcpat_getdargs(_tmp2D7,dsc);_tmp2D3->f3=_tmp57B;});_tmp2D3;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->hd=work_frame,_tmp2D2->tl=work;_tmp2D2;});
void*_tmp2D0=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp2D1=({void*_tmp57F=({struct Cyc_List_List*_tmp57E=ctx;void*_tmp57D=Cyc_Tcpat_add_neg(dsc,_tmp2D7);Cyc_Tcpat_build_desc(_tmp57E,_tmp57D,work);});Cyc_Tcpat_or_match(_tmp57F,rules);});
# 1502
return Cyc_Tcpat_ifeq(obj,_tmp2D7,_tmp2D0,_tmp2D1);}}_LL5:;}}_LL0:;}
# 1512
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1517
void*_tmp2D8=d;struct Cyc_List_List*_tmp2DF;void*_tmp2DE;struct Cyc_Tcpat_Rhs*_tmp2DD;void*_tmp2DC;switch(*((int*)_tmp2D8)){case 0U: _LL1: _tmp2DC=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2D8)->f1;_LL2:
# 1519
 if(!(*exhaust_done)){not_exhaust(env1,_tmp2DC);*exhaust_done=1;}
goto _LL0;case 1U: _LL3: _tmp2DD=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2D8)->f1;_LL4:
 rhs_appears(env2,_tmp2DD);goto _LL0;default: _LL5: _tmp2DF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D8)->f2;_tmp2DE=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp2D8)->f3;_LL6:
# 1523
 for(0;_tmp2DF != 0;_tmp2DF=_tmp2DF->tl){
struct _tuple0 _tmp2D9=*((struct _tuple0*)_tmp2DF->hd);struct _tuple0 _tmp2DA=_tmp2D9;void*_tmp2DB;_LL8: _tmp2DB=_tmp2DA.f2;_LL9:;
Cyc_Tcpat_check_exhaust_overlap(_tmp2DB,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1528
Cyc_Tcpat_check_exhaust_overlap(_tmp2DE,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1530
goto _LL0;}_LL0:;}
# 1538
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1540
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5->used=0,_tmp2E5->pat_loc=(swc->pattern)->loc,_tmp2E5->rhs=swc->body;_tmp2E5;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp2E0=({union Cyc_Tcpat_PatOrWhere _tmp3BC;(_tmp3BC.where_clause).tag=2U,(_tmp3BC.where_clause).val=swc->where_clause;_tmp3BC;});
({void*_tmp582=({struct Cyc_List_List*_tmp581=({void*_tmp2E1[2U];_tmp2E1[0]=sp0,({void*_tmp580=Cyc_Tcpat_int_pat(*ctr,_tmp2E0);_tmp2E1[1]=_tmp580;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2E1,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp581,_tmp2E0);});sp=_tmp582;});
*ctr=*ctr + 1;}else{
# 1548
({void*_tmp585=({struct Cyc_List_List*_tmp584=({void*_tmp2E2[2U];_tmp2E2[0]=sp0,({void*_tmp583=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2E3=_cycalloc(sizeof(*_tmp2E3));_tmp2E3->tag=0U;_tmp2E3;});_tmp2E2[1]=_tmp583;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2E2,sizeof(void*),2U));});Cyc_Tcpat_tuple_pat(_tmp584,({union Cyc_Tcpat_PatOrWhere _tmp3BD;(_tmp3BD.where_clause).tag=2U,(_tmp3BD.where_clause).val=0;_tmp3BD;}));});sp=_tmp585;});}
return({struct _tuple22*_tmp2E4=_cycalloc(sizeof(*_tmp2E4));_tmp2E4->f1=sp,_tmp2E4->f2=rhs;_tmp2E4;});}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1554
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1556
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp2E6=d;struct Cyc_Set_Set*_tmp347;struct Cyc_Tcpat_Con_s*_tmp346;struct Cyc_List_List*_tmp345;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E6)->tag == 0U){_LL1: _tmp346=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E6)->f1;_tmp345=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E6)->f2;_LL2: {
# 1560
union Cyc_Tcpat_Name_value _tmp2E7=_tmp346->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp2E8=_tmp346->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp2E9=_tmp2E8;struct Cyc_Absyn_Pat*_tmp2EA;if((_tmp2E9.where_clause).tag == 2){_LL6: _LL7:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp345))->hd);}else{_LL8: _tmp2EA=(_tmp2E9.pattern).val;_LL9:
 p=_tmp2EA;goto _LL5;}_LL5:;}{
# 1566
void*_tmp2EB=p->r;void*_tmp2EC=_tmp2EB;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Enumfield*_tmp320;struct Cyc_Absyn_Enumfield*_tmp31F;struct _dyneither_ptr _tmp31E;int _tmp31D;char _tmp31C;int _tmp31B;struct Cyc_Absyn_Datatypefield*_tmp31A;struct Cyc_Absyn_Aggrdecl*_tmp319;struct Cyc_List_List*_tmp318;struct Cyc_Absyn_Tvar*_tmp317;struct Cyc_Absyn_Vardecl*_tmp316;struct Cyc_Absyn_Vardecl*_tmp315;struct Cyc_Absyn_Vardecl*_tmp314;switch(*((int*)_tmp2EC)){case 0U: _LLB: _LLC:
 return({const char*_tmp2ED="_";_tag_dyneither(_tmp2ED,sizeof(char),2U);});case 1U: _LLD: _tmp314=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1;_LLE:
 return Cyc_Absynpp_qvar2string(_tmp314->name);case 3U: _LLF: _tmp315=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1;_LL10:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F0=({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0U,({struct _dyneither_ptr _tmp586=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp315->name));_tmp3BE.f1=_tmp586;});_tmp3BE;});void*_tmp2EE[1U];_tmp2EE[0]=& _tmp2F0;({struct _dyneither_ptr _tmp587=({const char*_tmp2EF="*%s";_tag_dyneither(_tmp2EF,sizeof(char),4U);});Cyc_aprintf(_tmp587,_tag_dyneither(_tmp2EE,sizeof(void*),1U));});});case 4U: _LL11: _tmp317=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1;_tmp316=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2EC)->f2;_LL12:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F3=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0U,({struct _dyneither_ptr _tmp588=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp316->name));_tmp3C0.f1=_tmp588;});_tmp3C0;});struct Cyc_String_pa_PrintArg_struct _tmp2F4=({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0U,_tmp3BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp317->name);_tmp3BF;});void*_tmp2F1[2U];_tmp2F1[0]=& _tmp2F3,_tmp2F1[1]=& _tmp2F4;({struct _dyneither_ptr _tmp589=({const char*_tmp2F2="%s<`%s>";_tag_dyneither(_tmp2F2,sizeof(char),8U);});Cyc_aprintf(_tmp589,_tag_dyneither(_tmp2F1,sizeof(void*),2U));});});case 5U: _LL13: _LL14:
# 1572
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F7=({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0U,({struct _dyneither_ptr _tmp58A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp345));_tmp3C1.f1=_tmp58A;});_tmp3C1;});void*_tmp2F5[1U];_tmp2F5[0]=& _tmp2F7;({struct _dyneither_ptr _tmp58B=({const char*_tmp2F6="$(%s)";_tag_dyneither(_tmp2F6,sizeof(char),6U);});Cyc_aprintf(_tmp58B,_tag_dyneither(_tmp2F5,sizeof(void*),1U));});});case 6U: _LL15: _LL16:
# 1574
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FA=({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0U,({struct _dyneither_ptr _tmp58C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp345));_tmp3C2.f1=_tmp58C;});_tmp3C2;});void*_tmp2F8[1U];_tmp2F8[0]=& _tmp2FA;({struct _dyneither_ptr _tmp58D=({const char*_tmp2F9="&%s";_tag_dyneither(_tmp2F9,sizeof(char),4U);});Cyc_aprintf(_tmp58D,_tag_dyneither(_tmp2F8,sizeof(void*),1U));});});case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1)->KnownAggr).tag == 2){_LL17: _tmp319=*((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1)->KnownAggr).val;_tmp318=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2EC)->f3;_LL18:
# 1576
 if(_tmp319->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2FB=_tmp318;struct _dyneither_ptr*_tmp301;if(_tmp2FB != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp2FB)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp2FB)->hd)->f1)->hd)->tag == 1U){_LL2C: _tmp301=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp2FB->hd)->f1)->hd)->f1;_LL2D:
# 1579
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp3C5;_tmp3C5.tag=0U,({struct _dyneither_ptr _tmp58E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp319->name));_tmp3C5.f1=_tmp58E;});_tmp3C5;});struct Cyc_String_pa_PrintArg_struct _tmp2FF=({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0U,_tmp3C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp301);_tmp3C4;});struct Cyc_String_pa_PrintArg_struct _tmp300=({struct Cyc_String_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=0U,({
struct _dyneither_ptr _tmp58F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp345));_tmp3C3.f1=_tmp58F;});_tmp3C3;});void*_tmp2FC[3U];_tmp2FC[0]=& _tmp2FE,_tmp2FC[1]=& _tmp2FF,_tmp2FC[2]=& _tmp300;({struct _dyneither_ptr _tmp590=({const char*_tmp2FD="%s{.%s = %s}";_tag_dyneither(_tmp2FD,sizeof(char),13U);});Cyc_aprintf(_tmp590,_tag_dyneither(_tmp2FC,sizeof(void*),3U));});});}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E: _LL2F:
 goto _LL2B;}_LL2B:;}
# 1584
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp304=({struct Cyc_String_pa_PrintArg_struct _tmp3C7;_tmp3C7.tag=0U,({struct _dyneither_ptr _tmp591=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp319->name));_tmp3C7.f1=_tmp591;});_tmp3C7;});struct Cyc_String_pa_PrintArg_struct _tmp305=({struct Cyc_String_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=0U,({struct _dyneither_ptr _tmp592=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp345));_tmp3C6.f1=_tmp592;});_tmp3C6;});void*_tmp302[2U];_tmp302[0]=& _tmp304,_tmp302[1]=& _tmp305;({struct _dyneither_ptr _tmp593=({const char*_tmp303="%s{%s}";_tag_dyneither(_tmp303,sizeof(char),7U);});Cyc_aprintf(_tmp593,_tag_dyneither(_tmp302,sizeof(void*),2U));});});}else{goto _LL29;}}else{goto _LL29;}case 8U: _LL19: _tmp31A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2EC)->f2;_LL1A:
# 1586
 if(_tmp345 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=0U,({struct _dyneither_ptr _tmp594=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31A->name));_tmp3C8.f1=_tmp594;});_tmp3C8;});void*_tmp306[1U];_tmp306[0]=& _tmp308;({struct _dyneither_ptr _tmp595=({const char*_tmp307="%s";_tag_dyneither(_tmp307,sizeof(char),3U);});Cyc_aprintf(_tmp595,_tag_dyneither(_tmp306,sizeof(void*),1U));});});else{
# 1589
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30B=({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0U,({struct _dyneither_ptr _tmp596=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31A->name));_tmp3CA.f1=_tmp596;});_tmp3CA;});struct Cyc_String_pa_PrintArg_struct _tmp30C=({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0U,({struct _dyneither_ptr _tmp597=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp345));_tmp3C9.f1=_tmp597;});_tmp3C9;});void*_tmp309[2U];_tmp309[0]=& _tmp30B,_tmp309[1]=& _tmp30C;({struct _dyneither_ptr _tmp598=({const char*_tmp30A="%s(%s)";_tag_dyneither(_tmp30A,sizeof(char),7U);});Cyc_aprintf(_tmp598,_tag_dyneither(_tmp309,sizeof(void*),2U));});});}case 9U: _LL1B: _LL1C:
 return({const char*_tmp30D="NULL";_tag_dyneither(_tmp30D,sizeof(char),5U);});case 10U: _LL1D: _tmp31B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2EC)->f2;_LL1E:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp310=({struct Cyc_Int_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=1U,_tmp3CB.f1=(unsigned long)_tmp31B;_tmp3CB;});void*_tmp30E[1U];_tmp30E[0]=& _tmp310;({struct _dyneither_ptr _tmp599=({const char*_tmp30F="%d";_tag_dyneither(_tmp30F,sizeof(char),3U);});Cyc_aprintf(_tmp599,_tag_dyneither(_tmp30E,sizeof(void*),1U));});});case 11U: _LL1F: _tmp31C=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1;_LL20:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp313=({struct Cyc_Int_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=1U,_tmp3CC.f1=(unsigned long)((int)_tmp31C);_tmp3CC;});void*_tmp311[1U];_tmp311[0]=& _tmp313;({struct _dyneither_ptr _tmp59A=({const char*_tmp312="%d";_tag_dyneither(_tmp312,sizeof(char),3U);});Cyc_aprintf(_tmp59A,_tag_dyneither(_tmp311,sizeof(void*),1U));});});case 12U: _LL21: _tmp31E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1;_tmp31D=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2EC)->f2;_LL22:
 return _tmp31E;case 13U: _LL23: _tmp31F=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2EC)->f2;_LL24:
 _tmp320=_tmp31F;goto _LL26;case 14U: _LL25: _tmp320=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2EC)->f2;_LL26:
 return Cyc_Absynpp_qvar2string(_tmp320->name);case 17U: _LL27: _tmp321=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2EC)->f1;_LL28:
 return Cyc_Absynpp_exp2string(_tmp321);default: _LL29: _LL2A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LLA:;};}}else{_LL3: _tmp347=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2E6)->f1;_LL4:
# 1600
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp347))return({const char*_tmp322="_";_tag_dyneither(_tmp322,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*_tmp323=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp347);
# 1604
if((int)(((_tmp323->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp324=({union Cyc_Tcpat_PatOrWhere _tmp344=_tmp323->orig_pat;if((_tmp344.pattern).tag != 1)_throw_match();(_tmp344.pattern).val;});
void*_tmp325=Cyc_Tcutil_compress((void*)_check_null(_tmp324->topt));void*_tmp326=_tmp325;struct Cyc_Absyn_PtrAtts _tmp343;struct Cyc_Absyn_Aggrdecl*_tmp342;struct Cyc_Absyn_Datatypedecl*_tmp341;switch(*((int*)_tmp326)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)->f2 == Cyc_Absyn_Char_sz){_LL31: _LL32:
# 1609
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp327=Cyc_Tcpat_char_con((char)i,_tmp324);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp347,_tmp327))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp32A=({struct Cyc_Int_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=1U,_tmp3CD.f1=(unsigned long)i;_tmp3CD;});void*_tmp328[1U];_tmp328[0]=& _tmp32A;({struct _dyneither_ptr _tmp59B=({const char*_tmp329="%d";_tag_dyneither(_tmp329,sizeof(char),3U);});Cyc_aprintf(_tmp59B,_tag_dyneither(_tmp328,sizeof(void*),1U));});});}}
# 1614
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL33: _LL34:
# 1617
{unsigned int i=0U;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp32B=Cyc_Tcpat_int_con((int)i,_tmp323->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp347,_tmp32B))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp32E=({struct Cyc_Int_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=1U,_tmp3CE.f1=(unsigned long)((int)i);_tmp3CE;});void*_tmp32C[1U];_tmp32C[0]=& _tmp32E;({struct _dyneither_ptr _tmp59C=({const char*_tmp32D="%d";_tag_dyneither(_tmp32D,sizeof(char),3U);});Cyc_aprintf(_tmp59C,_tag_dyneither(_tmp32C,sizeof(void*),1U));});});}}
# 1622
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)->f1).KnownDatatype).tag == 2){_LL37: _tmp341=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)->f1).KnownDatatype).val;_LL38:
# 1632
 if(_tmp341->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp332=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp341->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp332);
for(0;(unsigned int)_tmp332;_tmp332=_tmp332->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp332->hd)->name).f2;
struct Cyc_List_List*_tmp333=((struct Cyc_Absyn_Datatypefield*)_tmp332->hd)->typs;
if(!({struct Cyc_Set_Set*_tmp59E=_tmp347;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp59E,({struct Cyc_Tcpat_Con_s*_tmp334=_cycalloc(sizeof(*_tmp334));({union Cyc_Tcpat_Name_value _tmp59D=Cyc_Tcpat_Name_v(n);_tmp334->name=_tmp59D;}),_tmp334->arity=0,_tmp334->span=0,_tmp334->orig_pat=_tmp323->orig_pat;_tmp334;}));})){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp333)== 0)
return n;else{
# 1642
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0U,_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);_tmp3CF;});void*_tmp335[1U];_tmp335[0]=& _tmp337;({struct _dyneither_ptr _tmp59F=({const char*_tmp336="%s(...)";_tag_dyneither(_tmp336,sizeof(char),8U);});Cyc_aprintf(_tmp59F,_tag_dyneither(_tmp335,sizeof(void*),1U));});});}}}
# 1645
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL3B;}case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp342=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)->f1).KnownAggr).val;if(_tmp342->kind == Cyc_Absyn_UnionA){_LL3A: {
# 1647
struct Cyc_List_List*_tmp338=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp342->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp338);
struct _tuple2*_tmp339=_tmp342->name;struct _tuple2*_tmp33A=_tmp339;struct _dyneither_ptr _tmp340;_LL3E: _tmp340=*_tmp33A->f2;_LL3F:;
for(0;(unsigned int)_tmp338;_tmp338=_tmp338->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp338->hd)->name;
if(!({struct Cyc_Set_Set*_tmp5A1=_tmp347;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5A1,({struct Cyc_Tcpat_Con_s*_tmp33B=_cycalloc(sizeof(*_tmp33B));({union Cyc_Tcpat_Name_value _tmp5A0=Cyc_Tcpat_Name_v(n);_tmp33B->name=_tmp5A0;}),_tmp33B->arity=0,_tmp33B->span=0,_tmp33B->orig_pat=_tmp323->orig_pat;_tmp33B;}));}))
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33E=({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0U,_tmp3D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp340);_tmp3D1;});struct Cyc_String_pa_PrintArg_struct _tmp33F=({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0U,_tmp3D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);_tmp3D0;});void*_tmp33C[2U];_tmp33C[0]=& _tmp33E,_tmp33C[1]=& _tmp33F;({struct _dyneither_ptr _tmp5A2=({const char*_tmp33D="%s{.%s = _}";_tag_dyneither(_tmp33D,sizeof(char),12U);});Cyc_aprintf(_tmp5A2,_tag_dyneither(_tmp33C,sizeof(void*),2U));});});}
# 1655
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3U: _LL35: _tmp343=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1).ptr_atts;_LL36: {
# 1624
void*_tmp32F=_tmp343.nullable;
int is_nullable=Cyc_Tcutil_force_type2bool(0,_tmp32F);
if(is_nullable){
if(!({struct Cyc_Set_Set*_tmp5A3=_tmp347;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5A3,Cyc_Tcpat_null_con(_tmp324));}))
return({const char*_tmp330="NULL";_tag_dyneither(_tmp330,sizeof(char),5U);});}
# 1630
return({const char*_tmp331="&_";_tag_dyneither(_tmp331,sizeof(char),3U);});}default: _LL3B: _LL3C:
# 1656
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL30:;};};}_LL0:;}
# 1660
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dyneither_ptr*_tmp348=_cycalloc(sizeof(*_tmp348));({struct _dyneither_ptr _tmp5A4=Cyc_Tcpat_desc2string(d);*_tmp348=_tmp5A4;});_tmp348;});}
# 1663
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp349=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=({struct _dyneither_ptr*_tmp34D=_cycalloc(sizeof(*_tmp34D));({struct _dyneither_ptr _tmp5A5=({const char*_tmp34C=",";_tag_dyneither(_tmp34C,sizeof(char),2U);});*_tmp34D=_tmp5A5;});_tmp34D;});
{struct Cyc_List_List*_tmp34A=_tmp349;for(0;_tmp34A != 0;_tmp34A=((struct Cyc_List_List*)_check_null(_tmp34A))->tl){
if(_tmp34A->tl != 0){
({struct Cyc_List_List*_tmp5A6=({struct Cyc_List_List*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->hd=comma,_tmp34B->tl=_tmp34A->tl;_tmp34B;});_tmp34A->tl=_tmp5A6;});
_tmp34A=_tmp34A->tl;}}}
# 1672
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp349);}
# 1675
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp34E;_push_handler(& _tmp34E);{int _tmp350=0;if(setjmp(_tmp34E.handler))_tmp350=1;if(!_tmp350){
{struct _dyneither_ptr _tmp351=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp354=({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0U,_tmp3D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp351);_tmp3D2;});void*_tmp352[1U];_tmp352[0]=& _tmp354;({unsigned int _tmp5A8=loc;struct _dyneither_ptr _tmp5A7=({const char*_tmp353="patterns may not be exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp353,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp5A8,_tmp5A7,_tag_dyneither(_tmp352,sizeof(void*),1U));});});}
# 1677
;_pop_handler();}else{void*_tmp34F=(void*)_exn_thrown;void*_tmp355=_tmp34F;void*_tmp358;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp355)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1681
({void*_tmp356=0U;({unsigned int _tmp5AA=loc;struct _dyneither_ptr _tmp5A9=({const char*_tmp357="patterns may not be exhaustive.";_tag_dyneither(_tmp357,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp5AA,_tmp5A9,_tag_dyneither(_tmp356,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp358=_tmp355;_LL4:(int)_rethrow(_tmp358);}_LL0:;}};}
# 1685
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1689
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1695
int _tmp359=0;
int*_tmp35A=& _tmp359;
struct Cyc_List_List*_tmp35B=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp35A,swcs);
void*_tmp35C=Cyc_Tcpat_match_compile(_tmp35B);
*dopt=_tmp35C;{
# 1701
int _tmp35D=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp35C,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp35D);
# 1704
for(0;_tmp35B != 0;_tmp35B=_tmp35B->tl){
struct _tuple22*_tmp35E=(struct _tuple22*)_tmp35B->hd;struct _tuple22*_tmp35F=_tmp35E;int _tmp363;unsigned int _tmp362;_LL1: _tmp363=(_tmp35F->f2)->used;_tmp362=(_tmp35F->f2)->pat_loc;_LL2:;
if(!_tmp363){
({void*_tmp360=0U;({unsigned int _tmp5AC=_tmp362;struct _dyneither_ptr _tmp5AB=({const char*_tmp361="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp361,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp5AC,_tmp5AB,_tag_dyneither(_tmp360,sizeof(void*),0U));});});
# 1709
break;}}};}
# 1718
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp364;_push_handler(& _tmp364);{int _tmp366=0;if(setjmp(_tmp364.handler))_tmp366=1;if(!_tmp366){
{struct _dyneither_ptr _tmp367=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp36A=({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0U,_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp367);_tmp3D3;});void*_tmp368[1U];_tmp368[0]=& _tmp36A;({unsigned int _tmp5AE=(*pr).f1;struct _dyneither_ptr _tmp5AD=({const char*_tmp369="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp369,sizeof(char),45U);});Cyc_Tcutil_warn(_tmp5AE,_tmp5AD,_tag_dyneither(_tmp368,sizeof(void*),1U));});});}
# 1721
;_pop_handler();}else{void*_tmp365=(void*)_exn_thrown;void*_tmp36B=_tmp365;void*_tmp36E;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp36B)->tag == Cyc_Tcpat_Desc2string){_LL1: _LL2:
# 1725
({void*_tmp36C=0U;({unsigned int _tmp5B0=(*pr).f1;struct _dyneither_ptr _tmp5AF=({const char*_tmp36D="pattern not exhaustive.";_tag_dyneither(_tmp36D,sizeof(char),24U);});Cyc_Tcutil_warn(_tmp5B0,_tmp5AF,_tag_dyneither(_tmp36C,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp36E=_tmp36B;_LL4:(int)_rethrow(_tmp36E);}_LL0:;}};};}
# 1729
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1733
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->used=0,_tmp375->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_tmp5B1=Cyc_Absyn_skip_stmt(0U);_tmp375->rhs=_tmp5B1;});_tmp375;});
struct Cyc_List_List*_tmp36F=({struct Cyc_List_List*_tmp374=_cycalloc(sizeof(*_tmp374));({struct _tuple22*_tmp5B3=({struct _tuple22*_tmp373=_cycalloc(sizeof(*_tmp373));({void*_tmp5B2=Cyc_Tcpat_compile_pat(p);_tmp373->f1=_tmp5B2;}),_tmp373->f2=rhs;_tmp373;});_tmp374->hd=_tmp5B3;}),_tmp374->tl=0;_tmp374;});
void*_tmp370=Cyc_Tcpat_match_compile(_tmp36F);
struct _tuple14 _tmp371=({struct _tuple14 _tmp3D4;_tmp3D4.f1=loc,_tmp3D4.f2=1;_tmp3D4;});
int _tmp372=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp370,Cyc_Tcpat_not_exhaust_warn,& _tmp371,Cyc_Tcpat_dummy_fn,0,& _tmp372);
# 1741
*dopt=_tmp370;
return _tmp371.f2;}
# 1749
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1753
if(swc->where_clause != 0)
({void*_tmp376=0U;({unsigned int _tmp5B5=((struct Cyc_Absyn_Exp*)_check_null(swc->where_clause))->loc;struct _dyneither_ptr _tmp5B4=({const char*_tmp377="&&-clauses not supported in exception handlers.";_tag_dyneither(_tmp377,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp5B5,_tmp5B4,_tag_dyneither(_tmp376,sizeof(void*),0U));});});{
# 1756
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->used=0,_tmp379->pat_loc=(swc->pattern)->loc,_tmp379->rhs=swc->body;_tmp379;});
return({struct _tuple22*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->f1=sp0,_tmp378->f2=rhs;_tmp378;});};}
# 1759
static void Cyc_Tcpat_not_exhaust_err2(unsigned int loc,void*d){;}
# 1762
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1766
struct Cyc_List_List*_tmp37A=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp37B=Cyc_Tcpat_match_compile(_tmp37A);
*dopt=_tmp37B;{
int _tmp37C=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp37B,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp37C);
# 1772
for(0;_tmp37A != 0;_tmp37A=_tmp37A->tl){
# 1774
if(_tmp37A->tl == 0)break;{
struct _tuple22*_tmp37D=(struct _tuple22*)_tmp37A->hd;struct _tuple22*_tmp37E=_tmp37D;int _tmp382;unsigned int _tmp381;_LL1: _tmp382=(_tmp37E->f2)->used;_tmp381=(_tmp37E->f2)->pat_loc;_LL2:;
if(!_tmp382){
({void*_tmp37F=0U;({unsigned int _tmp5B7=_tmp381;struct _dyneither_ptr _tmp5B6=({const char*_tmp380="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp380,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp5B7,_tmp5B6,_tag_dyneither(_tmp37F,sizeof(void*),0U));});});
break;}};}};}
# 1783
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp383=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp383 != 0;_tmp383=_tmp383->tl){
if((*((struct _tuple15*)_tmp383->hd)).f1 != 0)
return 1;}}
return 0;}
