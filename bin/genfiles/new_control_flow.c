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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 941 "absyn.h"
extern void*Cyc_Absyn_void_type;
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1158
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1160
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
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
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
int Cyc_Tcutil_is_void_type(void*);
# 43
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 54
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 56
int Cyc_Tcutil_is_bound_one(void*b);
# 63
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 68
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 79
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 114
void*Cyc_Tcutil_compress(void*t);
# 203
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 216
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 225
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 296 "tcutil.h"
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 304
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 314
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 373
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 376
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
({void*_tmp635=(void*)({struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_NewControlFlow_CFAnnot,(_tmp2->f1).visited=0;_tmp2;});s->annot=_tmp635;});return Cyc_NewControlFlow_get_stmt_annot(s);}_LL0:;}
# 142
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*_tmp4=_cycalloc(sizeof(*_tmp4));({union Cyc_CfFlowInfo_FlowInfo _tmp636=Cyc_CfFlowInfo_BottomFL();*_tmp4=_tmp636;});_tmp4;});
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
({union Cyc_CfFlowInfo_FlowInfo _tmp637=Cyc_CfFlowInfo_join_flow(env->fenv,inflow,*_tmp8);*_tmp8=_tmp637;});
# 169
_tmp7->visited=env->iteration_num;
return({struct _tuple20 _tmp596;_tmp596.f1=_tmp7,_tmp596.f2=_tmp8;_tmp596;});}
# 179
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 187
({union Cyc_CfFlowInfo_FlowInfo _tmp638=Cyc_CfFlowInfo_join_flow(env->fenv,new_flow,env->tryflow);env->tryflow=_tmp638;});}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
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
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp597;_tmp597.tag=0U,({struct _dyneither_ptr _tmp639=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp10->name));_tmp597.f1=_tmp639;});_tmp597;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({unsigned int _tmp63B=_tmp12;struct _dyneither_ptr _tmp63A=({const char*_tmpD="alias-free pointer(s) reachable from %s may become unreachable.";_tag_dyneither(_tmpD,sizeof(char),64U);});Cyc_Warn_warn(_tmp63B,_tmp63A,_tag_dyneither(_tmpC,sizeof(void*),1U));});});
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
struct _tuple19 _tmp17=({struct _tuple19 _tmp599;_tmp599.f1=flow,_tmp599.f2=_tmp16;_tmp599;});struct _tuple19 _tmp18=_tmp17;struct Cyc_Dict_Dict _tmp1B;struct Cyc_Dict_Dict _tmp1A;if(((_tmp18.f1).ReachableFL).tag == 2){if(((_tmp18.f2).ReachableFL).tag == 2){_LL1: _tmp1B=(((_tmp18.f1).ReachableFL).val).f1;_tmp1A=(((_tmp18.f2).ReachableFL).val).f1;_LL2: {
# 229
struct _tuple21 _tmp19=({struct _tuple21 _tmp598;_tmp598.f1=env,_tmp598.f2=s->loc,_tmp598.f3=_tmp1A;_tmp598;});
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
({void*_tmp63C=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=8U,_tmp1F->f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp1F->f2=_tmp1E;_tmp1F;});_tmp1E=_tmp63C;});
# 267
({struct Cyc_Dict_Dict _tmp63D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp22,(void*)_tmp1D,_tmp1E);_tmp22=_tmp63D;});}
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
{void*_tmp29=Cyc_Tcutil_compress(t);void*_tmp2A=_tmp29;struct Cyc_List_List*_tmp45;union Cyc_Absyn_AggrInfo _tmp44;struct Cyc_List_List*_tmp43;switch(*((int*)_tmp2A)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f1)->tag == 20U){_LL6: _tmp44=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f1)->f1;_tmp43=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f2;_LL7: {
# 302
struct Cyc_Absyn_Aggrdecl*_tmp2B=Cyc_Absyn_get_known_aggrdecl(_tmp44);
struct Cyc_Absyn_Aggrfield*_tmp2C=Cyc_Absyn_lookup_decl_field(_tmp2B,f);
struct Cyc_Absyn_Exp*_tmp2D=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp2C))->requires_clause;
if(_tmp2D != 0){
struct _RegionHandle _tmp2E=_new_region("temp");struct _RegionHandle*temp=& _tmp2E;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp2F=({struct _RegionHandle*_tmp63F=temp;struct Cyc_List_List*_tmp63E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp2B->tvs,_tmp43);Cyc_Tcutil_rsubsexp(_tmp63F,_tmp63E,_tmp2D);});
# 309
if(!({struct Cyc_List_List*_tmp640=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp640,Cyc_Relations_exp2relns(temp,_tmp2F));})){
({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp59B;_tmp59B.tag=0U,({struct _dyneither_ptr _tmp641=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp2F)));_tmp59B.f1=_tmp641;});_tmp59B;});struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp59A;_tmp59A.tag=0U,_tmp59A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp59A;});void*_tmp30[2U];_tmp30[0]=& _tmp32,_tmp30[1]=& _tmp33;({unsigned int _tmp643=loc;struct _dyneither_ptr _tmp642=({const char*_tmp31="unable to prove %s, required to access %s";_tag_dyneither(_tmp31,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp643,_tmp642,_tag_dyneither(_tmp30,sizeof(void*),2U));});});
({void*_tmp34=0U;({struct Cyc___cycFILE*_tmp645=Cyc_stderr;struct _dyneither_ptr _tmp644=({const char*_tmp35="  [recorded facts on path: ";_tag_dyneither(_tmp35,sizeof(char),28U);});Cyc_fprintf(_tmp645,_tmp644,_tag_dyneither(_tmp34,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp36=0U;({struct Cyc___cycFILE*_tmp647=Cyc_stderr;struct _dyneither_ptr _tmp646=({const char*_tmp37="]\n";_tag_dyneither(_tmp37,sizeof(char),3U);});Cyc_fprintf(_tmp647,_tmp646,_tag_dyneither(_tmp36,sizeof(void*),0U));});});}}
# 307
;_pop_region(temp);}
# 316
goto _LL5;}}else{goto _LLA;}case 7U: _LL8: _tmp45=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A)->f2;_LL9: {
# 318
struct Cyc_Absyn_Aggrfield*_tmp38=Cyc_Absyn_lookup_field(_tmp45,f);
struct Cyc_Absyn_Exp*_tmp39=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp38))->requires_clause;
if(_tmp39 != 0){
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
if(!({struct Cyc_List_List*_tmp648=_tmp46;Cyc_NewControlFlow_relns_ok(_tmp648,Cyc_Relations_exp2relns(temp,_tmp39));})){
({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp59D;_tmp59D.tag=0U,({struct _dyneither_ptr _tmp649=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp39)));_tmp59D.f1=_tmp649;});_tmp59D;});struct Cyc_String_pa_PrintArg_struct _tmp3E=({struct Cyc_String_pa_PrintArg_struct _tmp59C;_tmp59C.tag=0U,_tmp59C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp59C;});void*_tmp3B[2U];_tmp3B[0]=& _tmp3D,_tmp3B[1]=& _tmp3E;({unsigned int _tmp64B=loc;struct _dyneither_ptr _tmp64A=({const char*_tmp3C="unable to prove %s, required to access %s";_tag_dyneither(_tmp3C,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp64B,_tmp64A,_tag_dyneither(_tmp3B,sizeof(void*),2U));});});
({void*_tmp3F=0U;({struct Cyc___cycFILE*_tmp64D=Cyc_stderr;struct _dyneither_ptr _tmp64C=({const char*_tmp40="  [recorded facts on path: ";_tag_dyneither(_tmp40,sizeof(char),28U);});Cyc_fprintf(_tmp64D,_tmp64C,_tag_dyneither(_tmp3F,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp46);
({void*_tmp41=0U;({struct Cyc___cycFILE*_tmp64F=Cyc_stderr;struct _dyneither_ptr _tmp64E=({const char*_tmp42="]\n";_tag_dyneither(_tmp42,sizeof(char),3U);});Cyc_fprintf(_tmp64F,_tmp64E,_tag_dyneither(_tmp41,sizeof(void*),0U));});});}
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
({void*_tmp48=0U;({unsigned int _tmp651=loc;struct _dyneither_ptr _tmp650=({const char*_tmp49="expression may not be fully initialized";_tag_dyneither(_tmp49,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp651,_tmp650,_tag_dyneither(_tmp48,sizeof(void*),0U));});});{
struct Cyc_Dict_Dict _tmp4A=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp4D,r);
if(_tmp4D.t == _tmp4A.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp4B=Cyc_CfFlowInfo_ReachableFL(_tmp4A,_tmp4C);
Cyc_NewControlFlow_update_tryflow(env,_tmp4B);
return _tmp4B;};};}_LL0:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 350
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp4E=({struct _tuple0 _tmp59E;({void*_tmp652=Cyc_Tcutil_compress(t);_tmp59E.f1=_tmp652;}),_tmp59E.f2=r;_tmp59E;});struct _tuple0 _tmp4F=_tmp4E;enum Cyc_Absyn_AggrKind _tmp60;struct Cyc_List_List*_tmp5F;struct _dyneither_ptr _tmp5E;struct Cyc_List_List*_tmp5D;struct _dyneither_ptr _tmp5C;union Cyc_Absyn_AggrInfo _tmp5B;struct Cyc_List_List*_tmp5A;struct _dyneither_ptr _tmp59;struct Cyc_Absyn_Datatypefield*_tmp58;struct _dyneither_ptr _tmp57;void*_tmp56;switch(*((int*)_tmp4F.f2)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
 return;}else{switch(*((int*)_tmp4F.f1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).tag == 2)goto _LL13;else{goto _LL13;}case 20U: goto _LL13;default: goto _LL13;}case 6U: goto _LL13;case 7U: goto _LL13;case 3U: goto _LL11;default: goto _LL13;}}case 0U: _LL3: _LL4:
 return;case 7U: _LL5: _LL6:
 return;case 8U: _LL7: _tmp56=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL8:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp56);return;default: switch(*((int*)_tmp4F.f1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LL9: _tmp58=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp57=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLA:
# 357
 if(_tmp58->typs == 0)
return;
_tmp5D=_tmp58->typs;_tmp5C=_tmp57;goto _LLC;}else{goto _LL13;}}else{goto _LL13;}case 20U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLD: _tmp5B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f1)->f1;_tmp5A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp59=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLE: {
# 368
struct Cyc_Absyn_Aggrdecl*_tmp50=Cyc_Absyn_get_known_aggrdecl(_tmp5B);
if(_tmp50->impl == 0)return;{
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50->impl))->fields;
struct _RegionHandle _tmp52=_new_region("temp");struct _RegionHandle*temp=& _tmp52;_push_region(temp);
{struct Cyc_List_List*_tmp53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp50->tvs,_tmp5A);
{int i=0;for(0;i < _get_dyneither_size(_tmp59,sizeof(void*));(i ++,_tmp51=_tmp51->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->type;
if(_tmp53 != 0)({void*_tmp653=Cyc_Tcutil_rsubstitute(temp,_tmp53,t);t=_tmp653;});
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp59.curr)[i]);}}
# 378
_npop_handler(0U);return;}
# 372
;_pop_region(temp);};}}else{goto _LL13;}default: goto _LL13;}case 6U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLB: _tmp5D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp5C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LLC: {
# 361
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5D);
{int i=0;for(0;i < sz;(i ++,_tmp5D=_tmp5D->tl)){
({struct Cyc_NewControlFlow_AnalEnv*_tmp656=env;unsigned int _tmp655=loc;void*_tmp654=(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp5D))->hd)).f2;Cyc_NewControlFlow_check_nounique(_tmp656,_tmp655,_tmp654,*((void**)_check_dyneither_subscript(_tmp5C,sizeof(void*),i)));});}}
# 365
return;}}else{goto _LL13;}case 7U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->tag == 6U){_LLF: _tmp60=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f1;_tmp5F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F.f1)->f2;_tmp5E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp4F.f2)->f2;_LL10:
# 381
{int i=0;for(0;i < _get_dyneither_size(_tmp5E,sizeof(void*));(i ++,_tmp5F=_tmp5F->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp5F))->hd)->type,((void**)_tmp5E.curr)[i]);}}
return;}else{goto _LL13;}case 3U: _LL11: _LL12:
# 385
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp54=0U;({unsigned int _tmp658=loc;struct _dyneither_ptr _tmp657=({const char*_tmp55="argument may still contain alias-free pointers";_tag_dyneither(_tmp55,sizeof(char),47U);});Cyc_Warn_warn(_tmp658,_tmp657,_tag_dyneither(_tmp54,sizeof(void*),0U));});});
return;default: _LL13: _LL14:
 return;}}_LL0:;}
# 392
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp61=inflow;struct Cyc_Dict_Dict _tmp6E;struct Cyc_List_List*_tmp6D;if((_tmp61.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmp6E=((_tmp61.ReachableFL).val).f1;_tmp6D=((_tmp61.ReachableFL).val).f2;_LL4:
# 396
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
({void*_tmp62=0U;({struct _dyneither_ptr _tmp659=({const char*_tmp63="noliveunique attribute requires alias-free pointer";_tag_dyneither(_tmp63,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp659,_tag_dyneither(_tmp62,sizeof(void*),0U));});});
return Cyc_CfFlowInfo_BottomFL();}{
# 400
void*_tmp64=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp65=r;void*_tmp6A;struct Cyc_CfFlowInfo_Place*_tmp69;void*_tmp68;switch(*((int*)_tmp65)){case 8U: _LL6: _tmp68=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp65)->f2;_LL7:
 r=_tmp68;goto retry;case 4U: _LL8: _tmp69=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LL9:
({void*_tmp65A=Cyc_CfFlowInfo_lookup_place(_tmp6E,_tmp69);_tmp6A=_tmp65A;});goto _LLB;case 5U: _LLA: _tmp6A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp65)->f1;_LLB:
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp64,_tmp6A);goto _LL5;default: _LLC: _LLD:
# 406
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp64))
({void*_tmp66=0U;({unsigned int _tmp65C=loc;struct _dyneither_ptr _tmp65B=({const char*_tmp67="argument may contain live alias-free pointers";_tag_dyneither(_tmp67,sizeof(char),46U);});Cyc_Warn_warn(_tmp65C,_tmp65B,_tag_dyneither(_tmp66,sizeof(void*),0U));});});
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
return({struct _tuple23 _tmp59F;_tmp59F.f1=inflow,_tmp59F.f2=0;_tmp59F;});
# 430
do{
struct _tuple16 _tmp6F=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);struct _tuple16 _tmp70=_tmp6F;union Cyc_CfFlowInfo_FlowInfo _tmp73;void*_tmp72;_LL1: _tmp73=_tmp70.f1;_tmp72=_tmp70.f2;_LL2:;
inflow=_tmp73;
({struct Cyc_List_List*_tmp65D=({struct Cyc_List_List*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->hd=_tmp72,_tmp71->tl=rvals;_tmp71;});rvals=_tmp65D;});
es=es->tl;
first_is_copy=others_are_copy;}while(es != 0);
# 438
Cyc_NewControlFlow_update_tryflow(env,inflow);
return({struct _tuple23 _tmp5A0;_tmp5A0.f1=inflow,({struct Cyc_List_List*_tmp65E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals);_tmp5A0.f2=_tmp65E;});_tmp5A0;});}
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
({void*_tmp77=0U;({unsigned int _tmp660=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp65F=({const char*_tmp78="expression may not be initialized";_tag_dyneither(_tmp78,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp660,_tmp65F,_tag_dyneither(_tmp77,sizeof(void*),0U));});});}
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 456
return({struct _tuple16 _tmp5A1;_tmp5A1.f1=_tmp7B,_tmp5A1.f2=(env->fenv)->unknown_all;_tmp5A1;});}
# 463
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 470
int needs_unconsume=0;
void*_tmp7C=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp7C,& needs_unconsume))
({void*_tmp7D=0U;({struct _dyneither_ptr _tmp661=({const char*_tmp7E="consume_zero_rval";_tag_dyneither(_tmp7E,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp661,_tag_dyneither(_tmp7D,sizeof(void*),0U));});});else{
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
({void*_tmp662=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=8U,_tmp82->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp82->f2=nzval;_tmp82;});nzval=_tmp662;});}
# 512
({void*_tmp663=Cyc_NewControlFlow_consume_zero_rval(env,_tmp86,_tmp84,e,nzval);nzval=_tmp663;});{
union Cyc_CfFlowInfo_FlowInfo _tmp83=({struct Cyc_Dict_Dict _tmp664=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp86,_tmp84,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp664,_tmp85);});
# 517
return _tmp83;};}}_LL5:;}}_LL0:;}
# 526
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 528
union Cyc_CfFlowInfo_FlowInfo _tmp87=outflow;struct Cyc_Dict_Dict _tmp8E;struct Cyc_List_List*_tmp8D;if((_tmp87.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple19 _tmp5A2;_tmp5A2.f1=outflow,_tmp5A2.f2=outflow;_tmp5A2;});}else{_LL3: _tmp8E=((_tmp87.ReachableFL).val).f1;_tmp8D=((_tmp87.ReachableFL).val).f2;_LL4: {
# 531
union Cyc_CfFlowInfo_AbsLVal _tmp88=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmp89=_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8C;if((_tmp89.UnknownL).tag == 2){_LL6: _LL7:
 return({struct _tuple19 _tmp5A3;_tmp5A3.f1=outflow,_tmp5A3.f2=outflow;_tmp5A3;});}else{_LL8: _tmp8C=(_tmp89.PlaceL).val;_LL9: {
# 534
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->unknown_none;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
({void*_tmp665=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=8U,_tmp8A->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8A->f2=nzval;_tmp8A;});nzval=_tmp665;});
({void*_tmp666=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=8U,_tmp8B->f1=(struct Cyc_Absyn_Vardecl*)names->hd,_tmp8B->f2=zval;_tmp8B;});zval=_tmp666;});}
# 540
({void*_tmp667=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,nzval);nzval=_tmp667;});
({void*_tmp668=Cyc_NewControlFlow_consume_zero_rval(env,_tmp8E,_tmp8C,e,zval);zval=_tmp668;});
return({struct _tuple19 _tmp5A4;({
union Cyc_CfFlowInfo_FlowInfo _tmp66C=({struct Cyc_Dict_Dict _tmp66B=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,nzval);Cyc_CfFlowInfo_ReachableFL(_tmp66B,_tmp8D);});_tmp5A4.f1=_tmp66C;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp66A=({struct Cyc_Dict_Dict _tmp669=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp8E,_tmp8C,zval);Cyc_CfFlowInfo_ReachableFL(_tmp669,_tmp8D);});_tmp5A4.f2=_tmp66A;});_tmp5A4;});}}_LL5:;}}_LL0:;}
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
({void*_tmp66D=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=Cyc_CfFlowInfo_UnknownZ,_tmp95->f1=relns;_tmp95;});e->annot=_tmp66D;});else{
# 570
({void*_tmp66E=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=Cyc_CfFlowInfo_NotZero,_tmp96->f1=relns;_tmp96;});e->annot=_tmp66E;});}
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
void*_tmp9A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp9B=_tmp9A;void*_tmpBF;void*_tmpBE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->tag == 3U){_LL1: _tmpBF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->f1).elt_type;_tmpBE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B)->f1).ptr_atts).bounds;_LL2: {
# 599
union Cyc_CfFlowInfo_FlowInfo _tmp9C=f;struct Cyc_Dict_Dict _tmpBB;struct Cyc_List_List*_tmpBA;if((_tmp9C.BottomFL).tag == 1){_LL6: _LL7:
# 601
 return({struct _tuple16 _tmp5A5;_tmp5A5.f1=f,({void*_tmp66F=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A5.f2=_tmp66F;});_tmp5A5;});}else{_LL8: _tmpBB=((_tmp9C.ReachableFL).val).f1;_tmpBA=((_tmp9C.ReachableFL).val).f2;_LL9:
# 604
 if(Cyc_Tcutil_is_bound_one(_tmpBE)){
struct _tuple15 _tmp9D=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp9E=_tmp9D;void*_tmpAB;struct Cyc_List_List*_tmpAA;_LLB: _tmpAB=_tmp9E.f1;_tmpAA=_tmp9E.f2;_LLC:;{
void*_tmp9F=_tmpAB;enum Cyc_CfFlowInfo_InitLevel _tmpA9;void*_tmpA8;struct Cyc_CfFlowInfo_Place*_tmpA7;struct Cyc_Absyn_Vardecl*_tmpA6;void*_tmpA5;switch(*((int*)_tmp9F)){case 8U: _LLE: _tmpA6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_tmpA5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f2;_LLF:
# 608
({void*_tmpA0=0U;({struct _dyneither_ptr _tmp670=({const char*_tmpA1="named location in anal_derefR";_tag_dyneither(_tmpA1,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp670,_tag_dyneither(_tmpA0,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 610
 Cyc_NewControlFlow_update_relns(e,0,_tmpBA);
goto _LLD;case 4U: _LL12: _tmpA7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL13:
# 613
 Cyc_NewControlFlow_update_relns(e,0,_tmpBA);
return({struct _tuple16 _tmp5A6;_tmp5A6.f1=f,({void*_tmp671=Cyc_CfFlowInfo_lookup_place(_tmpBB,_tmpA7);_tmp5A6.f2=_tmp671;});_tmp5A6;});case 5U: _LL14: _tmpA8=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL15:
# 616
 Cyc_NewControlFlow_update_relns(e,1,_tmpBA);
return({struct _tuple16 _tmp5A7;_tmp5A7.f1=f,_tmp5A7.f2=_tmpA8;_tmp5A7;});case 0U: _LL16: _LL17:
# 619
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple16 _tmp5A8;({union Cyc_CfFlowInfo_FlowInfo _tmp673=Cyc_CfFlowInfo_BottomFL();_tmp5A8.f1=_tmp673;}),({void*_tmp672=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A8.f2=_tmp672;});_tmp5A8;});case 2U: _LL18: _tmpA9=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp9F)->f1;_LL19:
# 623
({union Cyc_CfFlowInfo_FlowInfo _tmp674=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA9,_tmpAA);f=_tmp674;});
goto _LL1B;default: _LL1A: _LL1B:
# 626
{void*_tmpA2=e->r;void*_tmpA3=_tmpA2;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA3)->tag == 23U){_LL1D: _LL1E:
# 628
({void*_tmp675=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->tag=Cyc_CfFlowInfo_UnknownZ,_tmpA4->f1=_tmpBA;_tmpA4;});e->annot=_tmp675;});
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
({void*_tmp676=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=Cyc_CfFlowInfo_UnknownZ,_tmpAE->f1=_tmpBA;_tmpAE;});e->annot=_tmp676;});
goto _LL21;}_LL21:;}{
# 646
enum Cyc_CfFlowInfo_InitLevel _tmpB0=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpBB,r);enum Cyc_CfFlowInfo_InitLevel _tmpB1=_tmpB0;if(_tmpB1 == Cyc_CfFlowInfo_NoneIL){_LL27: _LL28: {
# 648
struct _tuple15 _tmpB2=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmpB3=_tmpB2;void*_tmpB9;_LL2C: _tmpB9=_tmpB3.f1;_LL2D:;
{void*_tmpB4=_tmpB9;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpB4)->tag == 7U){_LL2F: _LL30:
# 651
({void*_tmpB5=0U;({unsigned int _tmp678=e->loc;struct _dyneither_ptr _tmp677=({const char*_tmpB6="attempt to dereference a consumed alias-free pointer";_tag_dyneither(_tmpB6,sizeof(char),53U);});Cyc_CfFlowInfo_aerr(_tmp678,_tmp677,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});
goto _LL2E;}else{_LL31: _LL32:
# 654
({void*_tmpB7=0U;({unsigned int _tmp67A=e->loc;struct _dyneither_ptr _tmp679=({const char*_tmpB8="dereference of possibly uninitialized pointer";_tag_dyneither(_tmpB8,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp67A,_tmp679,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});}_LL2E:;}
# 656
goto _LL2A;}}else{_LL29: _LL2A:
# 658
 return({struct _tuple16 _tmp5A9;_tmp5A9.f1=f,({void*_tmp67B=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpBF,0,(env->fenv)->unknown_all);_tmp5A9.f2=_tmp67B;});_tmp5A9;});}_LL26:;};}_LL5:;}}else{_LL3: _LL4:
# 661
({void*_tmpBC=0U;({struct _dyneither_ptr _tmp67C=({const char*_tmpBD="right deref of non-pointer-type";_tag_dyneither(_tmpBD,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp67C,_tag_dyneither(_tmpBC,sizeof(void*),0U));});});}_LL0:;}
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
({struct Cyc_List_List*_tmp67D=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,n1,relns);relns=_tmp67D;});}
# 681
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 685
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rbound))
({struct Cyc_List_List*_tmp67E=Cyc_Relations_add_relation(Cyc_Core_heap_region,n2,Cyc_Relations_Rlt,rbound,relns);relns=_tmp67E;});}
# 691
return relns;};}
# 699
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 704
struct _tuple18 _tmpC4=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);struct _tuple18 _tmpC5=_tmpC4;union Cyc_CfFlowInfo_AbsLVal _tmpD4;_LL1: _tmpD4=_tmpC5.f2;_LL2:;
{struct _tuple18 _tmpC6=({struct _tuple18 _tmp5AB;_tmp5AB.f1=inflow,_tmp5AB.f2=_tmpD4;_tmp5AB;});struct _tuple18 _tmpC7=_tmpC6;struct Cyc_Dict_Dict _tmpD3;struct Cyc_List_List*_tmpD2;struct Cyc_CfFlowInfo_Place*_tmpD1;if(((_tmpC7.f1).ReachableFL).tag == 2){if(((_tmpC7.f2).PlaceL).tag == 1){_LL4: _tmpD3=(((_tmpC7.f1).ReachableFL).val).f1;_tmpD2=(((_tmpC7.f1).ReachableFL).val).f2;_tmpD1=((_tmpC7.f2).PlaceL).val;_LL5: {
# 707
void*_tmpC8=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 709
struct _tuple15 _tmpC9=Cyc_CfFlowInfo_unname_rval(old_rval);struct _tuple15 _tmpCA=_tmpC9;void*_tmpCD;struct Cyc_List_List*_tmpCC;_LLB: _tmpCD=_tmpCA.f1;_tmpCC=_tmpCA.f2;_LLC:;
for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
({void*_tmp67F=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=8U,_tmpCB->f1=(struct Cyc_Absyn_Vardecl*)_tmpCC->hd,_tmpCB->f2=_tmpC8;_tmpCB;});_tmpC8=_tmp67F;});}
# 714
({struct Cyc_Dict_Dict _tmp680=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmpD3,_tmpD1,_tmpC8);_tmpD3=_tmp680;});
({union Cyc_CfFlowInfo_FlowInfo _tmp681=Cyc_CfFlowInfo_ReachableFL(_tmpD3,_tmpD2);inflow=_tmp681;});
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 720
goto _LL3;}}else{_LL8: _LL9:
# 723
({struct Cyc_String_pa_PrintArg_struct _tmpD0=({struct Cyc_String_pa_PrintArg_struct _tmp5AA;_tmp5AA.tag=0U,({struct _dyneither_ptr _tmp682=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(exp));_tmp5AA.f1=_tmp682;});_tmp5AA;});void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({struct _dyneither_ptr _tmp683=({const char*_tmpCF="noconsume parameter %s must be l-values";_tag_dyneither(_tmpCF,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp683,_tag_dyneither(_tmpCE,sizeof(void*),1U));});});
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
 return({struct _tuple16 _tmp5AC;({union Cyc_CfFlowInfo_FlowInfo _tmp684=Cyc_CfFlowInfo_BottomFL();_tmp5AC.f1=_tmp684;}),_tmp5AC.f2=rval;_tmp5AC;});}else{_LL3: _tmpE0=((_tmpD5.ReachableFL).val).f1;_tmpDF=((_tmpD5.ReachableFL).val).f2;_LL4: {
# 742
union Cyc_CfFlowInfo_AbsLVal _tmpD6=lval;struct Cyc_CfFlowInfo_Place*_tmpDE;if((_tmpD6.PlaceL).tag == 1){_LL6: _tmpDE=(_tmpD6.PlaceL).val;_LL7: {
# 750
struct Cyc_Dict_Dict _tmpD7=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmpE0,_tmpDE,rval);
({struct Cyc_List_List*_tmp685=Cyc_Relations_reln_assign_exp(Cyc_Core_heap_region,_tmpDF,lexp,rexp);_tmpDF=_tmp685;});
({union Cyc_CfFlowInfo_FlowInfo _tmp686=Cyc_CfFlowInfo_ReachableFL(_tmpD7,_tmpDF);outflow=_tmp686;});
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(lexp->topt))){
# 756
struct _tuple15 _tmpD8=Cyc_CfFlowInfo_unname_rval(Cyc_CfFlowInfo_lookup_place(_tmpE0,_tmpDE));struct _tuple15 _tmpD9=_tmpD8;void*_tmpDD;_LLB: _tmpDD=_tmpD9.f1;_LLC:;{
void*_tmpDA=_tmpDD;switch(*((int*)_tmpDA)){case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpDA)->f1 == Cyc_CfFlowInfo_NoneIL){_LLE: _LLF:
 goto _LL11;}else{goto _LL14;}case 7U: _LL10: _LL11:
 goto _LL13;case 0U: _LL12: _LL13:
 goto _LLD;default: _LL14: _LL15:
# 762
({void*_tmpDB=0U;({unsigned int _tmp688=lexp->loc;struct _dyneither_ptr _tmp687=({const char*_tmpDC="assignment may overwrite alias-free pointer(s)";_tag_dyneither(_tmpDC,sizeof(char),47U);});Cyc_Warn_warn(_tmp688,_tmp687,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
goto _LLD;}_LLD:;};}
# 767
Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple16 _tmp5AD;_tmp5AD.f1=outflow,_tmp5AD.f2=rval;_tmp5AD;});}}else{_LL8: _LL9:
# 773
 return({struct _tuple16 _tmp5AE;({union Cyc_CfFlowInfo_FlowInfo _tmp689=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);_tmp5AE.f1=_tmp689;}),_tmp5AE.f2=rval;_tmp5AE;});}_LL5:;}}_LL0:;}
# 780
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 786
union Cyc_CfFlowInfo_FlowInfo _tmpE1=f;struct Cyc_Dict_Dict _tmpE6;struct Cyc_List_List*_tmpE5;if((_tmpE1.BottomFL).tag == 1){_LL1: _LL2:
 return Cyc_CfFlowInfo_BottomFL();}else{_LL3: _tmpE6=((_tmpE1.ReachableFL).val).f1;_tmpE5=((_tmpE1.ReachableFL).val).f2;_LL4:
# 791
({struct Cyc_Dict_Dict _tmp68F=({struct Cyc_CfFlowInfo_FlowEnv*_tmp68E=fenv;unsigned int _tmp68D=loc;struct Cyc_Dict_Dict _tmp68C=_tmpE6;struct Cyc_CfFlowInfo_Place*_tmp68B=({struct Cyc_CfFlowInfo_Place*_tmpE3=_cycalloc(sizeof(*_tmpE3));
({void*_tmp68A=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->tag=0U,_tmpE2->f1=vd;_tmpE2;});_tmpE3->root=_tmp68A;}),_tmpE3->path=0;_tmpE3;});
# 791
Cyc_CfFlowInfo_assign_place(_tmp68E,_tmp68D,_tmp68C,_tmp68B,rval);});_tmpE6=_tmp68F;});
# 794
({struct Cyc_List_List*_tmp690=Cyc_Relations_reln_assign_var(Cyc_Core_heap_region,_tmpE5,vd,rexp);_tmpE5=_tmp690;});{
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
if((*((struct _tuple24*)x->hd)).f1 == 0)({struct Cyc_List_List*_tmp691=({struct Cyc_List_List*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),_tmpE8->tl=es;_tmpE8;});es=_tmp691;});}}
# 819
({union Cyc_CfFlowInfo_FlowInfo _tmp692=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmpE7,fenv->unknown_all,pat_loc,name_locs);inflow=_tmp692;});
# 821
({union Cyc_CfFlowInfo_FlowInfo _tmp693=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);inflow=_tmp693;});
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 825
struct _tuple16 _tmpE9=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple16 _tmpEA=_tmpE9;union Cyc_CfFlowInfo_FlowInfo _tmpEC;void*_tmpEB;_LL1: _tmpEC=_tmpEA.f1;_tmpEB=_tmpEA.f2;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp694=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmpEC,_tmpEB);inflow=_tmp694;});}}{
# 833
struct Cyc_List_List*_tmpED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _tuple24*_tmpEE=(struct _tuple24*)_tmpED->hd;struct _tuple24*_tmpEF=_tmpEE;struct Cyc_Absyn_Vardecl**_tmp107;struct Cyc_Absyn_Exp*_tmp106;_LL4: _tmp107=_tmpEF->f1;_tmp106=_tmpEF->f2;_LL5:;
if(_tmp107 != 0  && _tmp106 != 0){
if(_tmp106->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp5AF;_tmp5AF.tag=0U,({
struct _dyneither_ptr _tmp695=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp106));_tmp5AF.f1=_tmp695;});_tmp5AF;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF2;({struct _dyneither_ptr _tmp696=({const char*_tmpF1="oops! pattern init expr %s has no type!\n";_tag_dyneither(_tmpF1,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp696,_tag_dyneither(_tmpF0,sizeof(void*),1U));});});{
# 847
struct Cyc_List_List l=({struct Cyc_List_List _tmp5B1;_tmp5B1.hd=_tmp106,_tmp5B1.tl=0;_tmp5B1;});
union Cyc_CfFlowInfo_FlowInfo _tmpF3=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmpF4=Cyc_NewControlFlow_anal_Lexp(env,_tmpF3,0,0,_tmp106);struct _tuple18 _tmpF5=_tmpF4;union Cyc_CfFlowInfo_AbsLVal _tmp105;_LL7: _tmp105=_tmpF5.f2;_LL8:;{
struct _tuple16 _tmpF6=Cyc_NewControlFlow_anal_Rexp(env,1,_tmpF3,_tmp106);struct _tuple16 _tmpF7=_tmpF6;union Cyc_CfFlowInfo_FlowInfo _tmp104;void*_tmp103;_LLA: _tmp104=_tmpF7.f1;_tmp103=_tmpF7.f2;_LLB:;{
union Cyc_CfFlowInfo_FlowInfo _tmpF8=_tmp104;struct Cyc_Dict_Dict _tmp102;struct Cyc_List_List*_tmp101;if((_tmpF8.ReachableFL).tag == 2){_LLD: _tmp102=((_tmpF8.ReachableFL).val).f1;_tmp101=((_tmpF8.ReachableFL).val).f2;_LLE:
# 853
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmpF9=_tmp105;struct Cyc_CfFlowInfo_Place*_tmp100;if((_tmpF9.PlaceL).tag == 1){_LL12: _tmp100=(_tmpF9.PlaceL).val;_LL13:
# 856
({void*_tmp697=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=8U,_tmpFA->f1=*_tmp107,_tmpFA->f2=_tmp103;_tmpFA;});_tmp103=_tmp697;});{
# 859
void*_tmpFB=Cyc_CfFlowInfo_lookup_place(_tmp102,_tmp100);
({void*_tmp698=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=8U,_tmpFC->f1=*_tmp107,_tmpFC->f2=_tmpFB;_tmpFC;});_tmpFB=_tmp698;});
({struct Cyc_Dict_Dict _tmp699=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp102,_tmp100,_tmpFB);_tmp102=_tmp699;});
({union Cyc_CfFlowInfo_FlowInfo _tmp69A=Cyc_CfFlowInfo_ReachableFL(_tmp102,_tmp101);_tmp104=_tmp69A;});
goto _LL11;};}else{_LL14: _LL15:
# 866
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp106->topt)) && !
Cyc_Tcutil_is_noalias_pointer_or_aggr((*_tmp107)->type))
({struct Cyc_String_pa_PrintArg_struct _tmpFF=({struct Cyc_String_pa_PrintArg_struct _tmp5B0;_tmp5B0.tag=0U,({
struct _dyneither_ptr _tmp69B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp106));_tmp5B0.f1=_tmp69B;});_tmp5B0;});void*_tmpFD[1U];_tmpFD[0]=& _tmpFF;({unsigned int _tmp69D=exp_loc;struct _dyneither_ptr _tmp69C=({const char*_tmpFE="aliased pattern expression not an l-value: %s";_tag_dyneither(_tmpFE,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp69D,_tmp69C,_tag_dyneither(_tmpFD,sizeof(void*),1U));});});}_LL11:;}
# 876
({union Cyc_CfFlowInfo_FlowInfo _tmp69E=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp104,*_tmp107,_tmp106,_tmp103,pat_loc);inflow=_tmp69E;});
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
void*_tmp10A=Cyc_Tcutil_compress((void*)_check_null(_tmp10F->topt));void*_tmp10B=_tmp10A;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp10B)->tag == 6U){_LL14: _LL15:
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
return({struct Cyc_Absyn_Exp*_tmp69F=Cyc_Absyn_prim1_exp(Cyc_Absyn_Numelts,_tmp114,0U);Cyc_Relations_exp2relnop(_tmp69F,rop);});}default: _LL5: _LL6:
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
({struct Cyc_String_pa_PrintArg_struct _tmp11F=({struct Cyc_String_pa_PrintArg_struct _tmp5B5;_tmp5B5.tag=0U,_tmp5B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp5B5;});struct Cyc_String_pa_PrintArg_struct _tmp120=({struct Cyc_String_pa_PrintArg_struct _tmp5B4;_tmp5B4.tag=0U,_tmp5B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp5B4;});struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp5B3;_tmp5B3.tag=0U,_tmp5B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp5B3;});struct Cyc_String_pa_PrintArg_struct _tmp122=({struct Cyc_String_pa_PrintArg_struct _tmp5B2;_tmp5B2.tag=0U,({
struct _dyneither_ptr _tmp6A0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_relns2string(_tmp123));_tmp5B2.f1=_tmp6A0;});_tmp5B2;});void*_tmp11D[4U];_tmp11D[0]=& _tmp11F,_tmp11D[1]=& _tmp120,_tmp11D[2]=& _tmp121,_tmp11D[3]=& _tmp122;({unsigned int _tmp6A2=loc;struct _dyneither_ptr _tmp6A1=({const char*_tmp11E="cannot prove that @requires clause %s %s %s is satisfied\n (all I know is %s)";_tag_dyneither(_tmp11E,sizeof(char),77U);});Cyc_CfFlowInfo_aerr(_tmp6A2,_tmp6A1,_tag_dyneither(_tmp11D,sizeof(void*),4U));});});
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
 return({struct _tuple16 _tmp5B6;({union Cyc_CfFlowInfo_FlowInfo _tmp6A3=Cyc_CfFlowInfo_BottomFL();_tmp5B6.f1=_tmp6A3;}),_tmp5B6.f2=_tmp125->unknown_all;_tmp5B6;});}else{_LL3: _tmp128=((_tmp126.ReachableFL).val).f1;_tmp127=((_tmp126.ReachableFL).val).f2;_LL4:
 d=_tmp128;relns=_tmp127;}_LL0:;}
# 989 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
# 1010 "new_control_flow.cyc"
struct _tuple18 _tmp129=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);struct _tuple18 _tmp12A=_tmp129;union Cyc_CfFlowInfo_FlowInfo _tmp136;union Cyc_CfFlowInfo_AbsLVal _tmp135;_LL6: _tmp136=_tmp12A.f1;_tmp135=_tmp12A.f2;_LL7:;{
struct _tuple18 _tmp12B=({struct _tuple18 _tmp5BA;_tmp5BA.f1=_tmp136,_tmp5BA.f2=_tmp135;_tmp5BA;});struct _tuple18 _tmp12C=_tmp12B;struct Cyc_Dict_Dict _tmp134;struct Cyc_List_List*_tmp133;struct Cyc_CfFlowInfo_Place*_tmp132;if(((_tmp12C.f1).ReachableFL).tag == 2){if(((_tmp12C.f2).PlaceL).tag == 1){_LL9: _tmp134=(((_tmp12C.f1).ReachableFL).val).f1;_tmp133=(((_tmp12C.f1).ReachableFL).val).f2;_tmp132=((_tmp12C.f2).PlaceL).val;_LLA: {
# 1013
void*_tmp12D=Cyc_CfFlowInfo_lookup_place(_tmp134,_tmp132);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp12D,& needs_unconsume)){
({void*_tmp12E=0U;({unsigned int _tmp6A5=e->loc;struct _dyneither_ptr _tmp6A4=({const char*_tmp12F="expression attempts to copy a consumed alias-free value";_tag_dyneither(_tmp12F,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6A5,_tmp6A4,_tag_dyneither(_tmp12E,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5B7;({union Cyc_CfFlowInfo_FlowInfo _tmp6A6=Cyc_CfFlowInfo_BottomFL();_tmp5B7.f1=_tmp6A6;}),_tmp5B7.f2=_tmp125->unknown_all;_tmp5B7;});}else{
# 1019
if(needs_unconsume)
# 1021
return({struct _tuple16 _tmp5B8;_tmp5B8.f1=_tmp136,({void*_tmp6A7=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp125,_tmp12D);_tmp5B8.f2=_tmp6A7;});_tmp5B8;});else{
# 1024
void*_tmp130=Cyc_CfFlowInfo_make_unique_consumed(_tmp125,(void*)_check_null(e->topt),e,env->iteration_num,_tmp12D);
struct Cyc_Dict_Dict _tmp131=Cyc_CfFlowInfo_assign_place(_tmp125,e->loc,_tmp134,_tmp132,_tmp130);
# 1036
return({struct _tuple16 _tmp5B9;({union Cyc_CfFlowInfo_FlowInfo _tmp6A8=Cyc_CfFlowInfo_ReachableFL(_tmp131,_tmp133);_tmp5B9.f1=_tmp6A8;}),_tmp5B9.f2=_tmp12D;_tmp5B9;});}}}}else{goto _LLB;}}else{_LLB: _LLC:
# 1040
 goto _LL8;}_LL8:;};}{
# 1043
void*_tmp137=e->r;void*_tmp138=_tmp137;struct Cyc_Absyn_Stmt*_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F5;void*_tmp2F4;int _tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F0;int _tmp2EF;struct Cyc_List_List*_tmp2EE;struct Cyc_List_List*_tmp2ED;enum Cyc_Absyn_AggrKind _tmp2EC;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Exp*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E5;struct Cyc_Absyn_Exp*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E3;struct Cyc_Absyn_Exp*_tmp2E2;struct Cyc_Absyn_Exp*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;struct _dyneither_ptr*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2DD;struct _dyneither_ptr*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DB;struct _dyneither_ptr*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D4;int _tmp2D3;struct Cyc_Absyn_Exp*_tmp2D2;void**_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;int _tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_List_List*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C5;enum Cyc_Absyn_Primop _tmp2C4;struct Cyc_List_List*_tmp2C3;struct Cyc_List_List*_tmp2C2;struct Cyc_Absyn_Datatypedecl*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C0;struct Cyc_Absyn_Vardecl*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2BE;struct _dyneither_ptr _tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2B8;switch(*((int*)_tmp138)){case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp138)->f4 == Cyc_Absyn_Null_to_NonNull){_LLE: _tmp2B8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LLF: {
# 1047
struct _tuple16 _tmp139=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2B8);struct _tuple16 _tmp13A=_tmp139;union Cyc_CfFlowInfo_FlowInfo _tmp140;void*_tmp13F;_LL7F: _tmp140=_tmp13A.f1;_tmp13F=_tmp13A.f2;_LL80:;{
struct _tuple16 _tmp13B=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp140,_tmp2B8,_tmp13F);struct _tuple16 _tmp13C=_tmp13B;union Cyc_CfFlowInfo_FlowInfo _tmp13E;void*_tmp13D;_LL82: _tmp13E=_tmp13C.f1;_tmp13D=_tmp13C.f2;_LL83:;
return({struct _tuple16 _tmp5BB;_tmp5BB.f1=_tmp13E,_tmp5BB.f2=_tmp13F;_tmp5BB;});};}}else{_LL10: _tmp2B9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL11:
# 1053
 _tmp2BA=_tmp2B9;goto _LL13;}case 12U: _LL12: _tmp2BA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL13:
 _tmp2BB=_tmp2BA;goto _LL15;case 41U: _LL14: _tmp2BB=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL15:
 _tmp2BC=_tmp2BB;goto _LL17;case 13U: _LL16: _tmp2BC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL17:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp2BC);case 2U: _LL18: _tmp2BD=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL19:
# 1059
 if(!({struct _dyneither_ptr _tmp6A9=(struct _dyneither_ptr)_tmp2BD;Cyc_NewControlFlow_strcmp(_tmp6A9,({const char*_tmp141="print_flow";_tag_dyneither(_tmp141,sizeof(char),11U);}));})){
struct _dyneither_ptr _tmp142=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp5BC;_tmp5BC.tag=0U,_tmp5BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp142);_tmp5BC;});void*_tmp143[1U];_tmp143[0]=& _tmp145;({struct Cyc___cycFILE*_tmp6AB=Cyc_stderr;struct _dyneither_ptr _tmp6AA=({const char*_tmp144="%s: current flow is\n";_tag_dyneither(_tmp144,sizeof(char),21U);});Cyc_fprintf(_tmp6AB,_tmp6AA,_tag_dyneither(_tmp143,sizeof(void*),1U));});});
Cyc_CfFlowInfo_print_flow(inflow);
({void*_tmp146=0U;({struct Cyc___cycFILE*_tmp6AD=Cyc_stderr;struct _dyneither_ptr _tmp6AC=({const char*_tmp147="\n";_tag_dyneither(_tmp147,sizeof(char),2U);});Cyc_fprintf(_tmp6AD,_tmp6AC,_tag_dyneither(_tmp146,sizeof(void*),0U));});});}else{
# 1065
if(!({struct _dyneither_ptr _tmp6AE=(struct _dyneither_ptr)_tmp2BD;Cyc_NewControlFlow_strcmp(_tmp6AE,({const char*_tmp148="debug_on";_tag_dyneither(_tmp148,sizeof(char),9U);}));}));else{
# 1070
if(!({struct _dyneither_ptr _tmp6AF=(struct _dyneither_ptr)_tmp2BD;Cyc_NewControlFlow_strcmp(_tmp6AF,({const char*_tmp149="debug_off";_tag_dyneither(_tmp149,sizeof(char),10U);}));}));else{
# 1076
({struct Cyc_String_pa_PrintArg_struct _tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp5BD;_tmp5BD.tag=0U,_tmp5BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BD);_tmp5BD;});void*_tmp14A[1U];_tmp14A[0]=& _tmp14C;({unsigned int _tmp6B1=e->loc;struct _dyneither_ptr _tmp6B0=({const char*_tmp14B="unknown pragma %s";_tag_dyneither(_tmp14B,sizeof(char),18U);});Cyc_Warn_warn(_tmp6B1,_tmp6B0,_tag_dyneither(_tmp14A,sizeof(void*),1U));});});}}}
return({struct _tuple16 _tmp5BE;_tmp5BE.f1=inflow,_tmp5BE.f2=_tmp125->zero;_tmp5BE;});case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp138)->f1).Wstring_c).tag){case 1U: _LL1A: _LL1B:
# 1079
 goto _LL1D;case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp138)->f1).Int_c).val).f2 == 0){_LL1C: _LL1D:
 return({struct _tuple16 _tmp5BF;_tmp5BF.f1=inflow,_tmp5BF.f2=_tmp125->zero;_tmp5BF;});}else{_LL1E: _LL1F:
 goto _LL21;}case 8U: _LL20: _LL21:
 goto _LL23;case 9U: _LL22: _LL23:
 goto _LL25;default: _LL28: _LL29:
# 1087
 goto _LL2B;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)){case 2U: _LL24: _LL25:
# 1084
 return({struct _tuple16 _tmp5C0;_tmp5C0.f1=inflow,_tmp5C0.f2=_tmp125->notzeroall;_tmp5C0;});case 1U: _LL34: _LL35:
# 1095
 return({struct _tuple16 _tmp5C1;_tmp5C1.f1=inflow,({void*_tmp6B2=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5C1.f2=_tmp6B2;});_tmp5C1;});case 3U: _LL36: _tmp2BE=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)->f1;_LL37:
# 1098
 _tmp2BF=_tmp2BE;goto _LL39;case 4U: _LL38: _tmp2BF=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)->f1;_LL39:
 _tmp2C0=_tmp2BF;goto _LL3B;case 5U: _LL3A: _tmp2C0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp138)->f1)->f1;_LL3B: {
# 1102
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp5C3;_tmp5C3.tag=0U,_tmp5C3.f1=_tmp2C0;_tmp5C3;});
return({struct _tuple16 _tmp5C2;_tmp5C2.f1=inflow,({void*_tmp6B4=({struct Cyc_Dict_Dict _tmp6B3=d;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp6B3,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));*_tmp14D=vdroot;_tmp14D;}));});_tmp5C2.f2=_tmp6B4;});_tmp5C2;});}default: _LL74: _LL75:
# 1702
 goto _LL77;}case 31U: if(((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp138)->f1 == 0){_LL26: _LL27:
# 1086
 goto _LL29;}else{_LL62: _tmp2C2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL63:
# 1565
 _tmp2E9=_tmp2C2;goto _LL65;}case 18U: _LL2A: _LL2B:
# 1088
 goto _LL2D;case 17U: _LL2C: _LL2D:
 goto _LL2F;case 19U: _LL2E: _LL2F:
 goto _LL31;case 33U: _LL30: _LL31:
 goto _LL33;case 32U: _LL32: _LL33:
 return({struct _tuple16 _tmp5C4;_tmp5C4.f1=inflow,_tmp5C4.f2=_tmp125->unknown_all;_tmp5C4;});case 3U: _LL3C: _tmp2C4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL3D: {
# 1109
struct _tuple16 _tmp14E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp2C3);struct _tuple16 _tmp14F=_tmp14E;union Cyc_CfFlowInfo_FlowInfo _tmp151;void*_tmp150;_LL85: _tmp151=_tmp14F.f1;_tmp150=_tmp14F.f2;_LL86:;
return({struct _tuple16 _tmp5C5;_tmp5C5.f1=_tmp151,_tmp5C5.f2=_tmp150;_tmp5C5;});}case 5U: _LL3E: _tmp2C5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL3F: {
# 1113
struct Cyc_List_List _tmp152=({struct Cyc_List_List _tmp5C9;_tmp5C9.hd=_tmp2C5,_tmp5C9.tl=0;_tmp5C9;});
struct _tuple16 _tmp153=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp152);struct _tuple16 _tmp154=_tmp153;union Cyc_CfFlowInfo_FlowInfo _tmp161;_LL88: _tmp161=_tmp154.f1;_LL89:;{
struct _tuple18 _tmp155=Cyc_NewControlFlow_anal_Lexp(env,_tmp161,0,0,_tmp2C5);struct _tuple18 _tmp156=_tmp155;union Cyc_CfFlowInfo_AbsLVal _tmp160;_LL8B: _tmp160=_tmp156.f2;_LL8C:;{
int iszt=Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(_tmp2C5->topt));
if(iszt){
# 1120
struct _tuple16 _tmp157=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2C5);struct _tuple16 _tmp158=_tmp157;union Cyc_CfFlowInfo_FlowInfo _tmp15A;void*_tmp159;_LL8E: _tmp15A=_tmp158.f1;_tmp159=_tmp158.f2;_LL8F:;
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp15A,_tmp2C5,_tmp159);}
# 1123
{struct _tuple25 _tmp15B=({struct _tuple25 _tmp5C6;_tmp5C6.f1=_tmp160,_tmp5C6.f2=_tmp161;_tmp5C6;});struct _tuple25 _tmp15C=_tmp15B;struct Cyc_CfFlowInfo_Place*_tmp15F;struct Cyc_Dict_Dict _tmp15E;struct Cyc_List_List*_tmp15D;if(((_tmp15C.f1).PlaceL).tag == 1){if(((_tmp15C.f2).ReachableFL).tag == 2){_LL91: _tmp15F=((_tmp15C.f1).PlaceL).val;_tmp15E=(((_tmp15C.f2).ReachableFL).val).f1;_tmp15D=(((_tmp15C.f2).ReachableFL).val).f2;_LL92:
# 1125
({struct Cyc_List_List*_tmp6B5=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp15D,_tmp2C5);_tmp15D=_tmp6B5;});
({union Cyc_CfFlowInfo_FlowInfo _tmp6B7=({struct Cyc_Dict_Dict _tmp6B6=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2C5->loc,_tmp15E,_tmp15F,_tmp125->unknown_all);Cyc_CfFlowInfo_ReachableFL(_tmp6B6,_tmp15D);});_tmp161=_tmp6B7;});
# 1130
goto _LL90;}else{goto _LL93;}}else{_LL93: _LL94:
 goto _LL90;}_LL90:;}
# 1134
if(iszt)
return({struct _tuple16 _tmp5C7;_tmp5C7.f1=_tmp161,_tmp5C7.f2=_tmp125->notzeroall;_tmp5C7;});else{
return({struct _tuple16 _tmp5C8;_tmp5C8.f1=_tmp161,_tmp5C8.f2=_tmp125->unknown_all;_tmp5C8;});}};};}case 4U: if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f2 != 0){_LL40: _tmp2C7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL41:
# 1139
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp162=0U;({unsigned int _tmp6B9=e->loc;struct _dyneither_ptr _tmp6B8=({const char*_tmp163="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp163,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6B9,_tmp6B8,_tag_dyneither(_tmp162,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5CA;({union Cyc_CfFlowInfo_FlowInfo _tmp6BA=Cyc_CfFlowInfo_BottomFL();_tmp5CA.f1=_tmp6BA;}),_tmp5CA.f2=_tmp125->unknown_all;_tmp5CA;});}{
# 1143
struct _tuple16 _tmp164=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C6);struct _tuple16 _tmp165=_tmp164;union Cyc_CfFlowInfo_FlowInfo _tmp170;void*_tmp16F;_LL96: _tmp170=_tmp165.f1;_tmp16F=_tmp165.f2;_LL97:;{
struct _tuple18 _tmp166=Cyc_NewControlFlow_anal_Lexp(env,_tmp170,0,0,_tmp2C7);struct _tuple18 _tmp167=_tmp166;union Cyc_CfFlowInfo_FlowInfo _tmp16E;union Cyc_CfFlowInfo_AbsLVal _tmp16D;_LL99: _tmp16E=_tmp167.f1;_tmp16D=_tmp167.f2;_LL9A:;
{union Cyc_CfFlowInfo_FlowInfo _tmp168=_tmp16E;struct Cyc_Dict_Dict _tmp16C;struct Cyc_List_List*_tmp16B;if((_tmp168.ReachableFL).tag == 2){_LL9C: _tmp16C=((_tmp168.ReachableFL).val).f1;_tmp16B=((_tmp168.ReachableFL).val).f2;_LL9D:
# 1147
{union Cyc_CfFlowInfo_AbsLVal _tmp169=_tmp16D;struct Cyc_CfFlowInfo_Place*_tmp16A;if((_tmp169.PlaceL).tag == 1){_LLA1: _tmp16A=(_tmp169.PlaceL).val;_LLA2:
# 1151
({struct Cyc_List_List*_tmp6BB=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp16B,_tmp2C7);_tmp16B=_tmp6BB;});
({struct Cyc_Dict_Dict _tmp6BC=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2C7->loc,_tmp16C,_tmp16A,_tmp125->unknown_all);_tmp16C=_tmp6BC;});
# 1154
({union Cyc_CfFlowInfo_FlowInfo _tmp6BD=Cyc_CfFlowInfo_ReachableFL(_tmp16C,_tmp16B);_tmp16E=_tmp6BD;});
# 1158
goto _LLA0;}else{_LLA3: _LLA4:
# 1161
 goto _LLA0;}_LLA0:;}
# 1163
goto _LL9B;}else{_LL9E: _LL9F:
 goto _LL9B;}_LL9B:;}
# 1166
return({struct _tuple16 _tmp5CB;_tmp5CB.f1=_tmp16E,_tmp5CB.f2=_tmp125->unknown_all;_tmp5CB;});};};}else{_LL42: _tmp2C9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2C8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL43:
# 1169
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt))){
({void*_tmp171=0U;({unsigned int _tmp6BF=e->loc;struct _dyneither_ptr _tmp6BE=({const char*_tmp172="cannot track alias-free pointers through multiple assignments";_tag_dyneither(_tmp172,sizeof(char),62U);});Cyc_CfFlowInfo_aerr(_tmp6BF,_tmp6BE,_tag_dyneither(_tmp171,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5CC;({union Cyc_CfFlowInfo_FlowInfo _tmp6C0=Cyc_CfFlowInfo_BottomFL();_tmp5CC.f1=_tmp6C0;}),_tmp5CC.f2=_tmp125->unknown_all;_tmp5CC;});}
# 1175
({union Cyc_CfFlowInfo_FlowInfo _tmp6C3=({struct Cyc_NewControlFlow_AnalEnv*_tmp6C2=env;union Cyc_CfFlowInfo_FlowInfo _tmp6C1=inflow;Cyc_NewControlFlow_expand_unique_places(_tmp6C2,_tmp6C1,({struct Cyc_Absyn_Exp*_tmp173[2U];_tmp173[0]=_tmp2C9,_tmp173[1]=_tmp2C8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp173,sizeof(struct Cyc_Absyn_Exp*),2U));}));});inflow=_tmp6C3;});{
# 1177
struct _tuple16 _tmp174=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2C8);struct _tuple16 _tmp175=_tmp174;union Cyc_CfFlowInfo_FlowInfo _tmp17F;void*_tmp17E;_LLA6: _tmp17F=_tmp175.f1;_tmp17E=_tmp175.f2;_LLA7:;{
struct _tuple18 _tmp176=Cyc_NewControlFlow_anal_Lexp(env,_tmp17F,0,0,_tmp2C9);struct _tuple18 _tmp177=_tmp176;union Cyc_CfFlowInfo_FlowInfo _tmp17D;union Cyc_CfFlowInfo_AbsLVal _tmp17C;_LLA9: _tmp17D=_tmp177.f1;_tmp17C=_tmp177.f2;_LLAA:;{
struct _tuple16 _tmp178=Cyc_NewControlFlow_do_assign(_tmp125,env,_tmp17D,_tmp2C9,_tmp17C,_tmp2C8,_tmp17E,e->loc);struct _tuple16 _tmp179=_tmp178;union Cyc_CfFlowInfo_FlowInfo _tmp17B;void*_tmp17A;_LLAC: _tmp17B=_tmp179.f1;_tmp17A=_tmp179.f2;_LLAD:;
# 1183
return({struct _tuple16 _tmp5CD;_tmp5CD.f1=_tmp17B,_tmp5CD.f2=_tmp17A;_tmp5CD;});};};};}case 9U: _LL44: _tmp2CB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2CA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL45: {
# 1186
struct _tuple16 _tmp180=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2CB);struct _tuple16 _tmp181=_tmp180;union Cyc_CfFlowInfo_FlowInfo _tmp183;void*_tmp182;_LLAF: _tmp183=_tmp181.f1;_tmp182=_tmp181.f2;_LLB0:;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp183,_tmp2CA);}case 11U: _LL46: _tmp2CC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL47: {
# 1190
struct _tuple16 _tmp184=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2CC);struct _tuple16 _tmp185=_tmp184;union Cyc_CfFlowInfo_FlowInfo _tmp187;void*_tmp186;_LLB2: _tmp187=_tmp185.f1;_tmp186=_tmp185.f2;_LLB3:;
Cyc_NewControlFlow_use_Rval(env,_tmp2CC->loc,_tmp187,_tmp186);
return({struct _tuple16 _tmp5CE;({union Cyc_CfFlowInfo_FlowInfo _tmp6C5=Cyc_CfFlowInfo_BottomFL();_tmp5CE.f1=_tmp6C5;}),({void*_tmp6C4=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5CE.f2=_tmp6C4;});_tmp5CE;});}case 10U: _LL48: _tmp2CE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2CD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL49: {
# 1195
struct Cyc_List_List*_tmp188=_tmp2CD;
struct _tuple23 _tmp189=({
struct Cyc_NewControlFlow_AnalEnv*_tmp6C8=env;union Cyc_CfFlowInfo_FlowInfo _tmp6C7=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6C8,_tmp6C7,({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7->hd=_tmp2CE,({struct Cyc_List_List*_tmp6C6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp2CD);_tmp1B7->tl=_tmp6C6;});_tmp1B7;}),0,1);});
# 1196
struct _tuple23 _tmp18A=_tmp189;union Cyc_CfFlowInfo_FlowInfo _tmp1B6;struct Cyc_List_List*_tmp1B5;_LLB5: _tmp1B6=_tmp18A.f1;_tmp1B5=_tmp18A.f2;_LLB6:;{
# 1199
union Cyc_CfFlowInfo_FlowInfo _tmp18B=Cyc_NewControlFlow_use_Rval(env,_tmp2CE->loc,_tmp1B6,(void*)((struct Cyc_List_List*)_check_null(_tmp1B5))->hd);
_tmp1B5=_tmp1B5->tl;{
# 1202
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
struct Cyc_List_List*requires;
struct Cyc_List_List*ensures;
{void*_tmp18C=Cyc_Tcutil_compress((void*)_check_null(_tmp2CE->topt));void*_tmp18D=_tmp18C;void*_tmp19F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18D)->tag == 3U){_LLB8: _tmp19F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18D)->f1).elt_type;_LLB9:
# 1209
{void*_tmp18E=Cyc_Tcutil_compress(_tmp19F);void*_tmp18F=_tmp18E;struct Cyc_List_List*_tmp19C;struct Cyc_List_List*_tmp19B;struct Cyc_List_List*_tmp19A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->tag == 5U){_LLBD: _tmp19C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->f1).attributes;_tmp19B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->f1).requires_relns;_tmp19A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18F)->f1).ensures_relns;_LLBE:
# 1211
 requires=_tmp19B;
ensures=_tmp19A;
for(0;_tmp19C != 0;_tmp19C=_tmp19C->tl){
# 1215
void*_tmp190=(void*)_tmp19C->hd;void*_tmp191=_tmp190;int _tmp197;int _tmp196;int _tmp195;switch(*((int*)_tmp191)){case 20U: _LLC2: _tmp195=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp191)->f1;_LLC3:
# 1217
({struct Cyc_List_List*_tmp6C9=({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=(void*)_tmp195,_tmp192->tl=init_params;_tmp192;});init_params=_tmp6C9;});goto _LLC1;case 21U: _LLC4: _tmp196=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp191)->f1;_LLC5:
# 1219
({struct Cyc_List_List*_tmp6CA=({struct Cyc_List_List*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193->hd=(void*)_tmp196,_tmp193->tl=nolive_unique_params;_tmp193;});nolive_unique_params=_tmp6CA;});
goto _LLC1;case 22U: _LLC6: _tmp197=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp191)->f1;_LLC7:
# 1223
({struct Cyc_List_List*_tmp6CB=({struct Cyc_List_List*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->hd=(void*)_tmp197,_tmp194->tl=noconsume_params;_tmp194;});noconsume_params=_tmp6CB;});
goto _LLC1;default: _LLC8: _LLC9:
 goto _LLC1;}_LLC1:;}
# 1227
goto _LLBC;}else{_LLBF: _LLC0:
({void*_tmp198=0U;({struct _dyneither_ptr _tmp6CC=({const char*_tmp199="anal_Rexp: bad function type";_tag_dyneither(_tmp199,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6CC,_tag_dyneither(_tmp198,sizeof(void*),0U));});});}_LLBC:;}
# 1230
goto _LLB7;}else{_LLBA: _LLBB:
({void*_tmp19D=0U;({struct _dyneither_ptr _tmp6CD=({const char*_tmp19E="anal_Rexp: bad function type";_tag_dyneither(_tmp19E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6CD,_tag_dyneither(_tmp19D,sizeof(void*),0U));});});}_LLB7:;}
# 1233
{int i=1;for(0;_tmp1B5 != 0;(_tmp1B5=_tmp1B5->tl,_tmp2CD=((struct Cyc_List_List*)_check_null(_tmp2CD))->tl,++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp1A0=_tmp1B6;struct Cyc_Dict_Dict _tmp1B4;if((_tmp1A0.BottomFL).tag == 1){_LLCB: _LLCC:
 goto _LLCA;}else{_LLCD: _tmp1B4=((_tmp1A0.ReachableFL).val).f1;_LLCE: {
# 1238
struct _tuple15 _tmp1A1=Cyc_CfFlowInfo_unname_rval((void*)_tmp1B5->hd);struct _tuple15 _tmp1A2=_tmp1A1;void*_tmp1B3;_LLD0: _tmp1B3=_tmp1A2.f1;_LLD1:;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp1B4,(void*)_tmp1B5->hd)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer((void*)_tmp1B5->hd))
({void*_tmp1A3=0U;({unsigned int _tmp6CF=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2CD))->hd)->loc;struct _dyneither_ptr _tmp6CE=({const char*_tmp1A4="expression may not be initialized";_tag_dyneither(_tmp1A4,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp6CF,_tmp6CE,_tag_dyneither(_tmp1A3,sizeof(void*),0U));});});
{union Cyc_CfFlowInfo_FlowInfo _tmp1A5=_tmp18B;struct Cyc_Dict_Dict _tmp1B2;struct Cyc_List_List*_tmp1B1;if((_tmp1A5.BottomFL).tag == 1){_LLD3: _LLD4:
 goto _LLD2;}else{_LLD5: _tmp1B2=((_tmp1A5.ReachableFL).val).f1;_tmp1B1=((_tmp1A5.ReachableFL).val).f2;_LLD6: {
# 1246
struct Cyc_Dict_Dict _tmp1A6=Cyc_CfFlowInfo_escape_deref(_tmp125,_tmp1B2,(void*)_tmp1B5->hd);
{void*_tmp1A7=(void*)_tmp1B5->hd;void*_tmp1A8=_tmp1A7;struct Cyc_CfFlowInfo_Place*_tmp1B0;switch(*((int*)_tmp1A8)){case 4U: _LLD8: _tmp1B0=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1A8)->f1;_LLD9:
# 1249
{void*_tmp1A9=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2CD))->hd)->topt));void*_tmp1AA=_tmp1A9;void*_tmp1AD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->tag == 3U){_LLDF: _tmp1AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA)->f1).elt_type;_LLE0:
# 1251
({struct Cyc_Dict_Dict _tmp6D4=({struct Cyc_CfFlowInfo_FlowEnv*_tmp6D3=_tmp125;unsigned int _tmp6D2=((struct Cyc_Absyn_Exp*)_tmp2CD->hd)->loc;struct Cyc_Dict_Dict _tmp6D1=_tmp1A6;struct Cyc_CfFlowInfo_Place*_tmp6D0=_tmp1B0;Cyc_CfFlowInfo_assign_place(_tmp6D3,_tmp6D2,_tmp6D1,_tmp6D0,
Cyc_CfFlowInfo_typ_to_absrval(_tmp125,_tmp1AD,0,_tmp125->esc_all));});
# 1251
_tmp1A6=_tmp6D4;});
# 1255
goto _LLDE;}else{_LLE1: _LLE2:
({void*_tmp1AB=0U;({struct _dyneither_ptr _tmp6D5=({const char*_tmp1AC="anal_Rexp:bad type for initialized arg";_tag_dyneither(_tmp1AC,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6D5,_tag_dyneither(_tmp1AB,sizeof(void*),0U));});});}_LLDE:;}
# 1258
goto _LLD7;case 5U: _LLDA: _LLDB:
# 1260
({void*_tmp1AE=0U;({struct _dyneither_ptr _tmp6D6=({const char*_tmp1AF="anal_Rexp:initialize attribute on unique pointers not yet supported";_tag_dyneither(_tmp1AF,sizeof(char),68U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp6D6,_tag_dyneither(_tmp1AE,sizeof(void*),0U));});});default: _LLDC: _LLDD:
# 1262
 goto _LLD7;}_LLD7:;}
# 1264
({union Cyc_CfFlowInfo_FlowInfo _tmp6D7=Cyc_CfFlowInfo_ReachableFL(_tmp1A6,_tmp1B1);_tmp18B=_tmp6D7;});
goto _LLD2;}}_LLD2:;}
# 1267
goto _LLCA;}}_LLCA:;}else{
# 1270
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1275
({union Cyc_CfFlowInfo_FlowInfo _tmp6DC=({struct Cyc_NewControlFlow_AnalEnv*_tmp6DB=env;unsigned int _tmp6DA=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2CD))->hd)->loc;void*_tmp6D9=(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2CD->hd)->topt);union Cyc_CfFlowInfo_FlowInfo _tmp6D8=_tmp18B;Cyc_NewControlFlow_use_nounique_Rval(_tmp6DB,_tmp6DA,_tmp6D9,_tmp6D8,(void*)_tmp1B5->hd);});_tmp18B=_tmp6DC;});else{
# 1285
({union Cyc_CfFlowInfo_FlowInfo _tmp6DD=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp2CD))->hd)->loc,_tmp18B,(void*)_tmp1B5->hd);_tmp18B=_tmp6DD;});
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2CD->hd)->topt),0))
({union Cyc_CfFlowInfo_FlowInfo _tmp6DE=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp18B,(struct Cyc_Absyn_Exp*)_tmp2CD->hd,((struct Cyc_Absyn_Exp*)_tmp2CD->hd)->loc,(void*)_tmp1B5->hd);_tmp18B=_tmp6DE;});}}}}
# 1292
Cyc_NewControlFlow_check_fn_requires(env,_tmp18B,_tmp188,requires,e->loc);
# 1295
if(Cyc_Tcutil_is_noreturn_fn_type((void*)_check_null(_tmp2CE->topt)))
return({struct _tuple16 _tmp5CF;({union Cyc_CfFlowInfo_FlowInfo _tmp6E0=Cyc_CfFlowInfo_BottomFL();_tmp5CF.f1=_tmp6E0;}),({void*_tmp6DF=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5CF.f2=_tmp6DF;});_tmp5CF;});else{
# 1298
return({struct _tuple16 _tmp5D0;_tmp5D0.f1=_tmp18B,({void*_tmp6E1=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5D0.f2=_tmp6E1;});_tmp5D0;});}};};}case 34U: _LL4A: _tmp2D3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).is_calloc;_tmp2D2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).rgn;_tmp2D1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).elt_type;_tmp2D0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).num_elts;_tmp2CF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp138)->f1).fat_result;_LL4B: {
# 1301
void*place_val;
if(_tmp2CF)place_val=_tmp125->notzeroall;else{
if(_tmp2D3)({void*_tmp6E2=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,*((void**)_check_null(_tmp2D1)),0,_tmp125->zero);place_val=_tmp6E2;});else{
({void*_tmp6E3=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,*((void**)_check_null(_tmp2D1)),0,_tmp125->unknown_none);place_val=_tmp6E3;});}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
if(_tmp2D2 != 0){
struct _tuple23 _tmp1B8=({struct Cyc_NewControlFlow_AnalEnv*_tmp6E5=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E4=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6E5,_tmp6E4,({struct Cyc_Absyn_Exp*_tmp1BC[2U];_tmp1BC[0]=_tmp2D2,_tmp1BC[1]=_tmp2D0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1BC,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1B9=_tmp1B8;union Cyc_CfFlowInfo_FlowInfo _tmp1BB;struct Cyc_List_List*_tmp1BA;_LLE4: _tmp1BB=_tmp1B9.f1;_tmp1BA=_tmp1B9.f2;_LLE5:;
outflow=_tmp1BB;}else{
# 1310
struct _tuple16 _tmp1BD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2D0);struct _tuple16 _tmp1BE=_tmp1BD;union Cyc_CfFlowInfo_FlowInfo _tmp1BF;_LLE7: _tmp1BF=_tmp1BE.f1;_LLE8:;
outflow=_tmp1BF;}
# 1315
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp5D1;_tmp5D1.f1=outflow,({void*_tmp6E6=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->tag=5U,_tmp1C0->f1=place_val;_tmp1C0;});_tmp5D1.f2=_tmp6E6;});_tmp5D1;});else{
# 1319
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->tag=1U,_tmp1C6->f1=_tmp2D0,_tmp1C6->f2=(void*)_check_null(e->topt);_tmp1C6;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->root=root,_tmp1C5->path=0;_tmp1C5;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->tag=4U,_tmp1C4->f1=place;_tmp1C4;});
union Cyc_CfFlowInfo_FlowInfo _tmp1C1=outflow;struct Cyc_Dict_Dict _tmp1C3;struct Cyc_List_List*_tmp1C2;if((_tmp1C1.BottomFL).tag == 1){_LLEA: _LLEB:
 return({struct _tuple16 _tmp5D2;_tmp5D2.f1=outflow,_tmp5D2.f2=rval;_tmp5D2;});}else{_LLEC: _tmp1C3=((_tmp1C1.ReachableFL).val).f1;_tmp1C2=((_tmp1C1.ReachableFL).val).f2;_LLED:
# 1325
 return({struct _tuple16 _tmp5D3;({union Cyc_CfFlowInfo_FlowInfo _tmp6E8=({struct Cyc_Dict_Dict _tmp6E7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1C3,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6E7,_tmp1C2);});_tmp5D3.f1=_tmp6E8;}),_tmp5D3.f2=rval;_tmp5D3;});}_LLE9:;}};}case 35U: _LL4C: _tmp2D5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2D4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL4D: {
# 1330
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1336
struct _tuple23 _tmp1C7=({struct Cyc_NewControlFlow_AnalEnv*_tmp6EA=env;union Cyc_CfFlowInfo_FlowInfo _tmp6E9=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6EA,_tmp6E9,({struct Cyc_Absyn_Exp*_tmp1E2[2U];_tmp1E2[0]=_tmp2D5,_tmp1E2[1]=_tmp2D4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1E2,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,0);});struct _tuple23 _tmp1C8=_tmp1C7;union Cyc_CfFlowInfo_FlowInfo _tmp1E1;struct Cyc_List_List*_tmp1E0;_LLEF: _tmp1E1=_tmp1C8.f1;_tmp1E0=_tmp1C8.f2;_LLF0:;
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1E0))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp1E0->tl))->hd;
outflow=_tmp1E1;{
# 1342
void*t_ign1=Cyc_Absyn_void_type;void*t_ign2=Cyc_Absyn_void_type;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp2D5,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1C9=right_rval;if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp1C9)->tag == 0U){_LLF2: _LLF3:
 goto _LLF1;}else{_LLF4: _LLF5:
({void*_tmp1CA=0U;({unsigned int _tmp6EC=_tmp2D5->loc;struct _dyneither_ptr _tmp6EB=({const char*_tmp1CB="cannot swap value into zeroterm array not known to be 0";_tag_dyneither(_tmp1CB,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6EC,_tmp6EB,_tag_dyneither(_tmp1CA,sizeof(void*),0U));});});}_LLF1:;}else{
# 1353
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp2D4,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp1CC=left_rval;if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp1CC)->tag == 0U){_LLF7: _LLF8:
 goto _LLF6;}else{_LLF9: _LLFA:
({void*_tmp1CD=0U;({unsigned int _tmp6EE=_tmp2D4->loc;struct _dyneither_ptr _tmp6ED=({const char*_tmp1CE="cannot swap value into zeroterm array not known to be 0";_tag_dyneither(_tmp1CE,sizeof(char),56U);});Cyc_CfFlowInfo_aerr(_tmp6EE,_tmp6ED,_tag_dyneither(_tmp1CD,sizeof(void*),0U));});});}_LLF6:;}}{
# 1361
struct _tuple18 _tmp1CF=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2D5);struct _tuple18 _tmp1D0=_tmp1CF;union Cyc_CfFlowInfo_AbsLVal _tmp1DF;_LLFC: _tmp1DF=_tmp1D0.f2;_LLFD:;{
struct _tuple18 _tmp1D1=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp2D4);struct _tuple18 _tmp1D2=_tmp1D1;union Cyc_CfFlowInfo_AbsLVal _tmp1DE;_LLFF: _tmp1DE=_tmp1D2.f2;_LL100:;
{union Cyc_CfFlowInfo_FlowInfo _tmp1D3=outflow;struct Cyc_Dict_Dict _tmp1DD;struct Cyc_List_List*_tmp1DC;if((_tmp1D3.ReachableFL).tag == 2){_LL102: _tmp1DD=((_tmp1D3.ReachableFL).val).f1;_tmp1DC=((_tmp1D3.ReachableFL).val).f2;_LL103:
# 1365
{union Cyc_CfFlowInfo_AbsLVal _tmp1D4=_tmp1DF;struct Cyc_CfFlowInfo_Place*_tmp1D7;if((_tmp1D4.PlaceL).tag == 1){_LL107: _tmp1D7=(_tmp1D4.PlaceL).val;_LL108:
# 1367
({struct Cyc_Dict_Dict _tmp6EF=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2D5->loc,_tmp1DD,_tmp1D7,right_rval);_tmp1DD=_tmp6EF;});
goto _LL106;}else{_LL109: _LL10A:
# 1373
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp1DD,right_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp1D5=0U;({unsigned int _tmp6F1=_tmp2D4->loc;struct _dyneither_ptr _tmp6F0=({const char*_tmp1D6="expression may not be fully initialized";_tag_dyneither(_tmp1D6,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6F1,_tmp6F0,_tag_dyneither(_tmp1D5,sizeof(void*),0U));});});
goto _LL106;}_LL106:;}
# 1377
{union Cyc_CfFlowInfo_AbsLVal _tmp1D8=_tmp1DE;struct Cyc_CfFlowInfo_Place*_tmp1DB;if((_tmp1D8.PlaceL).tag == 1){_LL10C: _tmp1DB=(_tmp1D8.PlaceL).val;_LL10D:
# 1379
({struct Cyc_Dict_Dict _tmp6F2=Cyc_CfFlowInfo_assign_place(_tmp125,_tmp2D4->loc,_tmp1DD,_tmp1DB,left_rval);_tmp1DD=_tmp6F2;});
goto _LL10B;}else{_LL10E: _LL10F:
# 1382
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp1DD,left_rval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp1D9=0U;({unsigned int _tmp6F4=_tmp2D5->loc;struct _dyneither_ptr _tmp6F3=({const char*_tmp1DA="expression may not be fully initialized";_tag_dyneither(_tmp1DA,sizeof(char),40U);});Cyc_CfFlowInfo_aerr(_tmp6F4,_tmp6F3,_tag_dyneither(_tmp1D9,sizeof(void*),0U));});});
goto _LL10B;}_LL10B:;}
# 1387
({struct Cyc_List_List*_tmp6F5=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1DC,_tmp2D5);_tmp1DC=_tmp6F5;});
({struct Cyc_List_List*_tmp6F6=Cyc_Relations_reln_kill_exp(Cyc_Core_heap_region,_tmp1DC,_tmp2D4);_tmp1DC=_tmp6F6;});
# 1390
({union Cyc_CfFlowInfo_FlowInfo _tmp6F7=Cyc_CfFlowInfo_ReachableFL(_tmp1DD,_tmp1DC);outflow=_tmp6F7;});
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL101;}else{_LL104: _LL105:
 goto _LL101;}_LL101:;}
# 1397
return({struct _tuple16 _tmp5D4;_tmp5D4.f1=outflow,_tmp5D4.f2=_tmp125->unknown_all;_tmp5D4;});};};};}case 16U: _LL4E: _tmp2D7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2D6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL4F: {
# 1400
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp2D7 != 0){
struct _tuple23 _tmp1E3=({struct Cyc_NewControlFlow_AnalEnv*_tmp6F9=env;union Cyc_CfFlowInfo_FlowInfo _tmp6F8=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp6F9,_tmp6F8,({struct Cyc_Absyn_Exp*_tmp1E7[2U];_tmp1E7[0]=_tmp2D7,_tmp1E7[1]=_tmp2D6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1E7,sizeof(struct Cyc_Absyn_Exp*),2U));}),1,1);});struct _tuple23 _tmp1E4=_tmp1E3;union Cyc_CfFlowInfo_FlowInfo _tmp1E6;struct Cyc_List_List*_tmp1E5;_LL111: _tmp1E6=_tmp1E4.f1;_tmp1E5=_tmp1E4.f2;_LL112:;
outflow=_tmp1E6;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp1E5))->tl))->hd;}else{
# 1407
struct _tuple16 _tmp1E8=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2D6);struct _tuple16 _tmp1E9=_tmp1E8;union Cyc_CfFlowInfo_FlowInfo _tmp1EB;void*_tmp1EA;_LL114: _tmp1EB=_tmp1E9.f1;_tmp1EA=_tmp1E9.f2;_LL115:;
outflow=_tmp1EB;
place_val=_tmp1EA;}
# 1412
if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e->topt),1))
return({struct _tuple16 _tmp5D5;_tmp5D5.f1=outflow,({void*_tmp6FA=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->tag=5U,_tmp1EC->f1=place_val;_tmp1EC;});_tmp5D5.f2=_tmp6FA;});_tmp5D5;});else{
# 1416
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp1F2=_cycalloc(sizeof(*_tmp1F2));_tmp1F2->tag=1U,_tmp1F2->f1=_tmp2D6,_tmp1F2->f2=(void*)_check_null(e->topt);_tmp1F2;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->root=root,_tmp1F1->path=0;_tmp1F1;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->tag=4U,_tmp1F0->f1=place;_tmp1F0;});
union Cyc_CfFlowInfo_FlowInfo _tmp1ED=outflow;struct Cyc_Dict_Dict _tmp1EF;struct Cyc_List_List*_tmp1EE;if((_tmp1ED.BottomFL).tag == 1){_LL117: _LL118:
 return({struct _tuple16 _tmp5D6;_tmp5D6.f1=outflow,_tmp5D6.f2=rval;_tmp5D6;});}else{_LL119: _tmp1EF=((_tmp1ED.ReachableFL).val).f1;_tmp1EE=((_tmp1ED.ReachableFL).val).f2;_LL11A:
# 1422
 return({struct _tuple16 _tmp5D7;({union Cyc_CfFlowInfo_FlowInfo _tmp6FC=({struct Cyc_Dict_Dict _tmp6FB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp1EF,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp6FB,_tmp1EE);});_tmp5D7.f1=_tmp6FC;}),_tmp5D7.f2=rval;_tmp5D7;});}_LL116:;}}case 15U: _LL50: _tmp2D8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL51: {
# 1427
struct _tuple18 _tmp1F3=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp2D8);struct _tuple18 _tmp1F4=_tmp1F3;union Cyc_CfFlowInfo_FlowInfo _tmp1F9;union Cyc_CfFlowInfo_AbsLVal _tmp1F8;_LL11C: _tmp1F9=_tmp1F4.f1;_tmp1F8=_tmp1F4.f2;_LL11D:;{
union Cyc_CfFlowInfo_AbsLVal _tmp1F5=_tmp1F8;struct Cyc_CfFlowInfo_Place*_tmp1F7;if((_tmp1F5.UnknownL).tag == 2){_LL11F: _LL120:
 return({struct _tuple16 _tmp5D8;_tmp5D8.f1=_tmp1F9,_tmp5D8.f2=_tmp125->notzeroall;_tmp5D8;});}else{_LL121: _tmp1F7=(_tmp1F5.PlaceL).val;_LL122:
 return({struct _tuple16 _tmp5D9;_tmp5D9.f1=_tmp1F9,({void*_tmp6FD=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));_tmp1F6->tag=4U,_tmp1F6->f1=_tmp1F7;_tmp1F6;});_tmp5D9.f2=_tmp6FD;});_tmp5D9;});}_LL11E:;};}case 20U: _LL52: _tmp2D9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL53: {
# 1434
struct _tuple16 _tmp1FA=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2D9);struct _tuple16 _tmp1FB=_tmp1FA;union Cyc_CfFlowInfo_FlowInfo _tmp1FD;void*_tmp1FC;_LL124: _tmp1FD=_tmp1FB.f1;_tmp1FC=_tmp1FB.f2;_LL125:;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1FD,_tmp2D9,_tmp1FC);}case 21U: _LL54: _tmp2DB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2DA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL55: {
# 1438
struct _tuple16 _tmp1FE=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2DB);struct _tuple16 _tmp1FF=_tmp1FE;union Cyc_CfFlowInfo_FlowInfo _tmp20F;void*_tmp20E;_LL127: _tmp20F=_tmp1FF.f1;_tmp20E=_tmp1FF.f2;_LL128:;{
void*_tmp200=(void*)_check_null(_tmp2DB->topt);
# 1441
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp200))
return({struct _tuple16 _tmp5DA;_tmp5DA.f1=_tmp20F,({void*_tmp6FE=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5DA.f2=_tmp6FE;});_tmp5DA;});
if(Cyc_Absyn_is_require_union_type(_tmp200))
Cyc_NewControlFlow_check_union_requires(_tmp2DB->loc,_tmp200,_tmp2DA,_tmp20F);{
struct _tuple15 _tmp201=Cyc_CfFlowInfo_unname_rval(_tmp20E);struct _tuple15 _tmp202=_tmp201;void*_tmp20D;_LL12A: _tmp20D=_tmp202.f1;_LL12B:;{
void*_tmp203=_tmp20D;int _tmp20C;int _tmp20B;struct _dyneither_ptr _tmp20A;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp203)->tag == 6U){_LL12D: _tmp20C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp203)->f1).is_union;_tmp20B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp203)->f1).fieldnum;_tmp20A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp203)->f2;_LL12E: {
# 1448
int _tmp204=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2DB->topt),_tmp2DA);
if((_tmp20C  && _tmp20B != - 1) && _tmp20B != _tmp204)
return({struct _tuple16 _tmp5DB;_tmp5DB.f1=_tmp20F,({void*_tmp6FF=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),1,_tmp125->unknown_none);_tmp5DB.f2=_tmp6FF;});_tmp5DB;});
return({struct _tuple16 _tmp5DC;_tmp5DC.f1=_tmp20F,_tmp5DC.f2=*((void**)_check_dyneither_subscript(_tmp20A,sizeof(void*),_tmp204));_tmp5DC;});}}else{_LL12F: _LL130:
# 1453
({void*_tmp205=0U;({struct Cyc___cycFILE*_tmp701=Cyc_stderr;struct _dyneither_ptr _tmp700=({const char*_tmp206="the bad rexp is :";_tag_dyneither(_tmp206,sizeof(char),18U);});Cyc_fprintf(_tmp701,_tmp700,_tag_dyneither(_tmp205,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(_tmp20D);
({struct Cyc_String_pa_PrintArg_struct _tmp209=({struct Cyc_String_pa_PrintArg_struct _tmp5DD;_tmp5DD.tag=0U,({struct _dyneither_ptr _tmp702=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5DD.f1=_tmp702;});_tmp5DD;});void*_tmp207[1U];_tmp207[0]=& _tmp209;({struct _dyneither_ptr _tmp703=({const char*_tmp208="\nanal_Rexp: AggrMember: %s";_tag_dyneither(_tmp208,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp703,_tag_dyneither(_tmp207,sizeof(void*),1U));});});}_LL12C:;};};};}case 38U: _LL56: _tmp2DD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2DC=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL57: {
# 1461
struct _tuple16 _tmp210=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2DD);struct _tuple16 _tmp211=_tmp210;union Cyc_CfFlowInfo_FlowInfo _tmp21E;void*_tmp21D;_LL132: _tmp21E=_tmp211.f1;_tmp21D=_tmp211.f2;_LL133:;
# 1463
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp2DD->topt)))
return({struct _tuple16 _tmp5DE;_tmp5DE.f1=_tmp21E,_tmp5DE.f2=_tmp125->unknown_all;_tmp5DE;});{
struct _tuple15 _tmp212=Cyc_CfFlowInfo_unname_rval(_tmp21D);struct _tuple15 _tmp213=_tmp212;void*_tmp21C;_LL135: _tmp21C=_tmp213.f1;_LL136:;{
void*_tmp214=_tmp21C;int _tmp21B;int _tmp21A;struct _dyneither_ptr _tmp219;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214)->tag == 6U){_LL138: _tmp21B=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214)->f1).is_union;_tmp21A=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214)->f1).fieldnum;_tmp219=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp214)->f2;_LL139: {
# 1468
int _tmp215=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp2DD->topt),_tmp2DC);
if(_tmp21B  && _tmp21A != - 1){
if(_tmp21A != _tmp215)
return({struct _tuple16 _tmp5DF;_tmp5DF.f1=_tmp21E,_tmp5DF.f2=_tmp125->zero;_tmp5DF;});else{
# 1473
return({struct _tuple16 _tmp5E0;_tmp5E0.f1=_tmp21E,_tmp5E0.f2=_tmp125->notzeroall;_tmp5E0;});}}else{
# 1475
return({struct _tuple16 _tmp5E1;_tmp5E1.f1=_tmp21E,_tmp5E1.f2=_tmp125->unknown_all;_tmp5E1;});}}}else{_LL13A: _LL13B:
# 1477
({struct Cyc_String_pa_PrintArg_struct _tmp218=({struct Cyc_String_pa_PrintArg_struct _tmp5E2;_tmp5E2.tag=0U,({struct _dyneither_ptr _tmp704=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5E2.f1=_tmp704;});_tmp5E2;});void*_tmp216[1U];_tmp216[0]=& _tmp218;({struct _dyneither_ptr _tmp705=({const char*_tmp217="anal_Rexp: TagCheck_e: %s";_tag_dyneither(_tmp217,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp705,_tag_dyneither(_tmp216,sizeof(void*),1U));});});}_LL137:;};};}case 22U: _LL58: _tmp2DF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2DE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL59: {
# 1481
struct _tuple16 _tmp21F=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp2DF);struct _tuple16 _tmp220=_tmp21F;union Cyc_CfFlowInfo_FlowInfo _tmp235;void*_tmp234;_LL13D: _tmp235=_tmp220.f1;_tmp234=_tmp220.f2;_LL13E:;{
# 1484
struct _tuple16 _tmp221=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp235,_tmp2DF,_tmp234);struct _tuple16 _tmp222=_tmp221;union Cyc_CfFlowInfo_FlowInfo _tmp233;void*_tmp232;_LL140: _tmp233=_tmp222.f1;_tmp232=_tmp222.f2;_LL141:;{
# 1487
void*_tmp223=Cyc_Tcutil_compress((void*)_check_null(_tmp2DF->topt));void*_tmp224=_tmp223;void*_tmp231;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp224)->tag == 3U){_LL143: _tmp231=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp224)->f1).elt_type;_LL144:
# 1489
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp231))
# 1491
return({struct _tuple16 _tmp5E3;_tmp5E3.f1=_tmp233,({void*_tmp706=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5E3.f2=_tmp706;});_tmp5E3;});
# 1493
if(Cyc_Absyn_is_require_union_type(_tmp231))
Cyc_NewControlFlow_check_union_requires(_tmp2DF->loc,_tmp231,_tmp2DE,_tmp235);{
# 1496
struct _tuple15 _tmp225=Cyc_CfFlowInfo_unname_rval(_tmp232);struct _tuple15 _tmp226=_tmp225;void*_tmp22E;_LL148: _tmp22E=_tmp226.f1;_LL149:;{
void*_tmp227=_tmp22E;int _tmp22D;int _tmp22C;struct _dyneither_ptr _tmp22B;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp227)->tag == 6U){_LL14B: _tmp22D=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp227)->f1).is_union;_tmp22C=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp227)->f1).fieldnum;_tmp22B=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp227)->f2;_LL14C: {
# 1499
int _tmp228=Cyc_CfFlowInfo_get_field_index(_tmp231,_tmp2DE);
if((_tmp22D  && _tmp22C != - 1) && _tmp22C != _tmp228)
return({struct _tuple16 _tmp5E4;_tmp5E4.f1=_tmp233,({void*_tmp707=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),1,_tmp125->unknown_none);_tmp5E4.f2=_tmp707;});_tmp5E4;});
return({struct _tuple16 _tmp5E5;_tmp5E5.f1=_tmp233,_tmp5E5.f2=*((void**)_check_dyneither_subscript(_tmp22B,sizeof(void*),_tmp228));_tmp5E5;});}}else{_LL14D: _LL14E:
({void*_tmp229=0U;({struct _dyneither_ptr _tmp708=({const char*_tmp22A="anal_Rexp: AggrArrow";_tag_dyneither(_tmp22A,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp708,_tag_dyneither(_tmp229,sizeof(void*),0U));});});}_LL14A:;};};}else{_LL145: _LL146:
# 1505
({void*_tmp22F=0U;({struct _dyneither_ptr _tmp709=({const char*_tmp230="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp230,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp709,_tag_dyneither(_tmp22F,sizeof(void*),0U));});});}_LL142:;};};}case 6U: _LL5A: _tmp2E2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2E1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_tmp2E0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL5B: {
# 1509
struct _tuple19 _tmp236=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2E2);struct _tuple19 _tmp237=_tmp236;union Cyc_CfFlowInfo_FlowInfo _tmp23B;union Cyc_CfFlowInfo_FlowInfo _tmp23A;_LL150: _tmp23B=_tmp237.f1;_tmp23A=_tmp237.f2;_LL151:;{
struct _tuple16 _tmp238=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp23B,_tmp2E1);
struct _tuple16 _tmp239=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp23A,_tmp2E0);
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp125,_tmp238,_tmp239);};}case 7U: _LL5C: _tmp2E4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2E3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL5D: {
# 1515
struct _tuple19 _tmp23C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2E4);struct _tuple19 _tmp23D=_tmp23C;union Cyc_CfFlowInfo_FlowInfo _tmp245;union Cyc_CfFlowInfo_FlowInfo _tmp244;_LL153: _tmp245=_tmp23D.f1;_tmp244=_tmp23D.f2;_LL154:;{
struct _tuple16 _tmp23E=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp245,_tmp2E3);struct _tuple16 _tmp23F=_tmp23E;union Cyc_CfFlowInfo_FlowInfo _tmp243;void*_tmp242;_LL156: _tmp243=_tmp23F.f1;_tmp242=_tmp23F.f2;_LL157:;{
struct _tuple16 _tmp240=({struct _tuple16 _tmp5E7;_tmp5E7.f1=_tmp243,_tmp5E7.f2=_tmp242;_tmp5E7;});
struct _tuple16 _tmp241=({struct _tuple16 _tmp5E6;_tmp5E6.f1=_tmp244,_tmp5E6.f2=_tmp125->zero;_tmp5E6;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp125,_tmp240,_tmp241);};};}case 8U: _LL5E: _tmp2E6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2E5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL5F: {
# 1522
struct _tuple19 _tmp246=Cyc_NewControlFlow_anal_test(env,inflow,_tmp2E6);struct _tuple19 _tmp247=_tmp246;union Cyc_CfFlowInfo_FlowInfo _tmp24F;union Cyc_CfFlowInfo_FlowInfo _tmp24E;_LL159: _tmp24F=_tmp247.f1;_tmp24E=_tmp247.f2;_LL15A:;{
struct _tuple16 _tmp248=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp24E,_tmp2E5);struct _tuple16 _tmp249=_tmp248;union Cyc_CfFlowInfo_FlowInfo _tmp24D;void*_tmp24C;_LL15C: _tmp24D=_tmp249.f1;_tmp24C=_tmp249.f2;_LL15D:;{
struct _tuple16 _tmp24A=({struct _tuple16 _tmp5E9;_tmp5E9.f1=_tmp24D,_tmp5E9.f2=_tmp24C;_tmp5E9;});
struct _tuple16 _tmp24B=({struct _tuple16 _tmp5E8;_tmp5E8.f1=_tmp24F,_tmp5E8.f2=_tmp125->notzeroall;_tmp5E8;});
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp125,_tmp24A,_tmp24B);};};}case 23U: _LL60: _tmp2E8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2E7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL61: {
# 1529
struct _tuple23 _tmp250=({struct Cyc_NewControlFlow_AnalEnv*_tmp70B=env;union Cyc_CfFlowInfo_FlowInfo _tmp70A=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp70B,_tmp70A,({struct Cyc_Absyn_Exp*_tmp26C[2U];_tmp26C[0]=_tmp2E8,_tmp26C[1]=_tmp2E7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26C,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp251=_tmp250;union Cyc_CfFlowInfo_FlowInfo _tmp26B;struct Cyc_List_List*_tmp26A;_LL15F: _tmp26B=_tmp251.f1;_tmp26A=_tmp251.f2;_LL160:;{
# 1533
union Cyc_CfFlowInfo_FlowInfo _tmp252=_tmp26B;
{union Cyc_CfFlowInfo_FlowInfo _tmp253=_tmp26B;struct Cyc_Dict_Dict _tmp258;struct Cyc_List_List*_tmp257;if((_tmp253.ReachableFL).tag == 2){_LL162: _tmp258=((_tmp253.ReachableFL).val).f1;_tmp257=((_tmp253.ReachableFL).val).f2;_LL163:
# 1538
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp258,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp26A))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp254=0U;({unsigned int _tmp70D=_tmp2E7->loc;struct _dyneither_ptr _tmp70C=({const char*_tmp255="expression may not be initialized";_tag_dyneither(_tmp255,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp70D,_tmp70C,_tag_dyneither(_tmp254,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp256=Cyc_NewControlFlow_add_subscript_reln(_tmp257,_tmp2E8,_tmp2E7);
if(_tmp257 != _tmp256)
({union Cyc_CfFlowInfo_FlowInfo _tmp70E=Cyc_CfFlowInfo_ReachableFL(_tmp258,_tmp256);_tmp252=_tmp70E;});
goto _LL161;};}else{_LL164: _LL165:
 goto _LL161;}_LL161:;}{
# 1546
void*_tmp259=Cyc_Tcutil_compress((void*)_check_null(_tmp2E8->topt));void*_tmp25A=_tmp259;struct Cyc_List_List*_tmp269;switch(*((int*)_tmp25A)){case 6U: _LL167: _tmp269=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp25A)->f1;_LL168: {
# 1548
struct _tuple15 _tmp25B=Cyc_CfFlowInfo_unname_rval((void*)((struct Cyc_List_List*)_check_null(_tmp26A))->hd);struct _tuple15 _tmp25C=_tmp25B;void*_tmp261;_LL16E: _tmp261=_tmp25C.f1;_LL16F:;{
void*_tmp25D=_tmp261;struct _dyneither_ptr _tmp260;if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25D)->tag == 6U){_LL171: _tmp260=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp25D)->f2;_LL172: {
# 1551
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp2E7)).f1;
return({struct _tuple16 _tmp5EA;_tmp5EA.f1=_tmp252,_tmp5EA.f2=*((void**)_check_dyneither_subscript(_tmp260,sizeof(void*),(int)i));_tmp5EA;});}}else{_LL173: _LL174:
({void*_tmp25E=0U;({struct _dyneither_ptr _tmp70F=({const char*_tmp25F="anal_Rexp: Subscript";_tag_dyneither(_tmp25F,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp70F,_tag_dyneither(_tmp25E,sizeof(void*),0U));});});}_LL170:;};}case 3U: _LL169: _LL16A: {
# 1556
struct _tuple16 _tmp262=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp26B,_tmp2E8,(void*)((struct Cyc_List_List*)_check_null(_tmp26A))->hd);struct _tuple16 _tmp263=_tmp262;union Cyc_CfFlowInfo_FlowInfo _tmp266;void*_tmp265;_LL176: _tmp266=_tmp263.f1;_tmp265=_tmp263.f2;_LL177:;{
union Cyc_CfFlowInfo_FlowInfo _tmp264=_tmp266;if((_tmp264.BottomFL).tag == 1){_LL179: _LL17A:
 return({struct _tuple16 _tmp5EB;_tmp5EB.f1=_tmp266,_tmp5EB.f2=_tmp265;_tmp5EB;});}else{_LL17B: _LL17C:
 return({struct _tuple16 _tmp5EC;_tmp5EC.f1=_tmp252,_tmp5EC.f2=_tmp265;_tmp5EC;});}_LL178:;};}default: _LL16B: _LL16C:
# 1561
({void*_tmp267=0U;({struct _dyneither_ptr _tmp710=({const char*_tmp268="anal_Rexp: Subscript -- bad type";_tag_dyneither(_tmp268,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp710,_tag_dyneither(_tmp267,sizeof(void*),0U));});});}_LL166:;};};}case 24U: _LL64: _tmp2E9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL65: {
# 1567
struct _tuple23 _tmp26D=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp2E9,1,1);struct _tuple23 _tmp26E=_tmp26D;union Cyc_CfFlowInfo_FlowInfo _tmp274;struct Cyc_List_List*_tmp273;_LL17E: _tmp274=_tmp26E.f1;_tmp273=_tmp26E.f2;_LL17F:;{
struct _dyneither_ptr aggrdict=({unsigned int _tmp272=(unsigned int)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E9);void**_tmp271=_cycalloc(_check_times(_tmp272,sizeof(void*)));({{unsigned int _tmp5EE=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E9);unsigned int i;for(i=0;i < _tmp5EE;++ i){({
void*_tmp711=({void*_tmp270=(void*)((struct Cyc_List_List*)_check_null(_tmp273))->hd;
_tmp273=_tmp273->tl;
_tmp270;});
# 1570
_tmp271[i]=_tmp711;});}}0;});_tag_dyneither(_tmp271,sizeof(void*),_tmp272);});
# 1574
return({struct _tuple16 _tmp5ED;_tmp5ED.f1=_tmp274,({void*_tmp712=(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->tag=6U,(_tmp26F->f1).is_union=0,(_tmp26F->f1).fieldnum=- 1,_tmp26F->f2=aggrdict;_tmp26F;});_tmp5ED.f2=_tmp712;});_tmp5ED;});};}case 30U: _LL66: _tmp2EA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_LL67: {
# 1577
struct Cyc_List_List*fs;
{void*_tmp275=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp276=_tmp275;struct Cyc_List_List*_tmp279;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp276)->tag == 7U){_LL181: _tmp279=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp276)->f2;_LL182:
 fs=_tmp279;goto _LL180;}else{_LL183: _LL184:
({void*_tmp277=0U;({struct _dyneither_ptr _tmp713=({const char*_tmp278="anal_Rexp:anon struct has bad type";_tag_dyneither(_tmp278,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp713,_tag_dyneither(_tmp277,sizeof(void*),0U));});});}_LL180:;}
# 1582
_tmp2ED=_tmp2EA;_tmp2EC=0U;_tmp2EB=fs;goto _LL69;}case 29U: if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4 != 0){if(((struct Cyc_Absyn_Aggrdecl*)((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4)->impl != 0){_LL68: _tmp2ED=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_tmp2EC=(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4)->kind;_tmp2EB=((((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp138)->f4)->impl)->fields;_LL69: {
# 1584
void*exp_type=(void*)_check_null(e->topt);
struct _tuple23 _tmp27A=({struct Cyc_NewControlFlow_AnalEnv*_tmp715=env;union Cyc_CfFlowInfo_FlowInfo _tmp714=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp715,_tmp714,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp2ED),1,1);});struct _tuple23 _tmp27B=_tmp27A;union Cyc_CfFlowInfo_FlowInfo _tmp285;struct Cyc_List_List*_tmp284;_LL186: _tmp285=_tmp27B.f1;_tmp284=_tmp27B.f2;_LL187:;{
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp125,_tmp2EB,_tmp2EC == Cyc_Absyn_UnionA,_tmp125->unknown_all);
# 1589
int field_no=-1;
{int i=0;for(0;_tmp284 != 0;(_tmp284=_tmp284->tl,_tmp2ED=_tmp2ED->tl,++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp2ED))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp27C=(void*)ds->hd;void*_tmp27D=_tmp27C;struct _dyneither_ptr*_tmp281;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp27D)->tag == 0U){_LL189: _LL18A:
({void*_tmp27E=0U;({struct _dyneither_ptr _tmp716=({const char*_tmp27F="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp27F,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp716,_tag_dyneither(_tmp27E,sizeof(void*),0U));});});}else{_LL18B: _tmp281=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp27D)->f1;_LL18C:
# 1596
({int _tmp717=Cyc_CfFlowInfo_get_field_index_fs(_tmp2EB,_tmp281);field_no=_tmp717;});
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp284->hd;
# 1599
if(_tmp2EC == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp280=(*((struct _tuple26*)_tmp2ED->hd)).f2;
({union Cyc_CfFlowInfo_FlowInfo _tmp718=Cyc_NewControlFlow_use_Rval(env,_tmp280->loc,_tmp285,(void*)_tmp284->hd);_tmp285=_tmp718;});
# 1603
Cyc_NewControlFlow_check_union_requires(_tmp280->loc,exp_type,_tmp281,_tmp285);}}_LL188:;}}}{
# 1606
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp282=({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->tag=6U,(_tmp283->f1).is_union=_tmp2EC == Cyc_Absyn_UnionA,(_tmp283->f1).fieldnum=field_no,_tmp283->f2=aggrdict;_tmp283;});
return({struct _tuple16 _tmp5EF;_tmp5EF.f1=_tmp285,_tmp5EF.f2=(void*)_tmp282;_tmp5EF;});};};}}else{goto _LL6A;}}else{_LL6A: _LL6B:
# 1610
({struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp5F0;_tmp5F0.tag=0U,({struct _dyneither_ptr _tmp719=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp5F0.f1=_tmp719;});_tmp5F0;});void*_tmp286[1U];_tmp286[0]=& _tmp288;({struct _dyneither_ptr _tmp71A=({const char*_tmp287="anal_Rexp:missing aggrdeclimpl in %s";_tag_dyneither(_tmp287,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp71A,_tag_dyneither(_tmp286,sizeof(void*),1U));});});}case 26U: _LL6C: _tmp2EE=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL6D: {
# 1612
struct Cyc_List_List*_tmp289=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp2EE);
struct _tuple23 _tmp28A=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp289,1,1);struct _tuple23 _tmp28B=_tmp28A;union Cyc_CfFlowInfo_FlowInfo _tmp28D;struct Cyc_List_List*_tmp28C;_LL18E: _tmp28D=_tmp28B.f1;_tmp28C=_tmp28B.f2;_LL18F:;
for(0;_tmp28C != 0;(_tmp28C=_tmp28C->tl,_tmp289=_tmp289->tl)){
({union Cyc_CfFlowInfo_FlowInfo _tmp71B=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp289))->hd)->loc,_tmp28D,(void*)_tmp28C->hd);_tmp28D=_tmp71B;});}
return({struct _tuple16 _tmp5F1;_tmp5F1.f1=_tmp28D,({void*_tmp71C=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,(void*)_check_null(e->topt),0,_tmp125->unknown_all);_tmp5F1.f2=_tmp71C;});_tmp5F1;});}case 27U: _LL6E: _tmp2F2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_tmp2EF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp138)->f4;_LL6F: {
# 1620
struct _tuple16 _tmp28E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2F1);struct _tuple16 _tmp28F=_tmp28E;union Cyc_CfFlowInfo_FlowInfo _tmp2AA;void*_tmp2A9;_LL191: _tmp2AA=_tmp28F.f1;_tmp2A9=_tmp28F.f2;_LL192:;{
union Cyc_CfFlowInfo_FlowInfo _tmp290=_tmp2AA;struct Cyc_Dict_Dict _tmp2A8;struct Cyc_List_List*_tmp2A7;if((_tmp290.BottomFL).tag == 1){_LL194: _LL195:
 return({struct _tuple16 _tmp5F2;_tmp5F2.f1=_tmp2AA,_tmp5F2.f2=_tmp125->unknown_all;_tmp5F2;});}else{_LL196: _tmp2A8=((_tmp290.ReachableFL).val).f1;_tmp2A7=((_tmp290.ReachableFL).val).f2;_LL197:
# 1624
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2A8,_tmp2A9)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp291=0U;({unsigned int _tmp71E=_tmp2F1->loc;struct _dyneither_ptr _tmp71D=({const char*_tmp292="expression may not be initialized";_tag_dyneither(_tmp292,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp71E,_tmp71D,_tag_dyneither(_tmp291,sizeof(void*),0U));});});{
# 1628
struct Cyc_List_List*new_relns=_tmp2A7;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp2F2);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp2F1,& n2))
({struct Cyc_List_List*_tmp71F=Cyc_Relations_add_relation(Cyc_Core_heap_region,n1,Cyc_Relations_Rlt,n2,_tmp2A7);new_relns=_tmp71F;});
if(_tmp2A7 != new_relns)
({union Cyc_CfFlowInfo_FlowInfo _tmp720=Cyc_CfFlowInfo_ReachableFL(_tmp2A8,new_relns);_tmp2AA=_tmp720;});{
# 1637
void*_tmp293=_tmp2A9;switch(*((int*)_tmp293)){case 0U: _LL199: _LL19A:
 return({struct _tuple16 _tmp5F3;_tmp5F3.f1=_tmp2AA,_tmp5F3.f2=_tmp125->unknown_all;_tmp5F3;});case 1U: _LL19B: _LL19C:
 goto _LL19E;case 4U: _LL19D: _LL19E: {
# 1641
struct Cyc_List_List _tmp294=({struct Cyc_List_List _tmp5F6;_tmp5F6.hd=_tmp2F2,_tmp5F6.tl=0;_tmp5F6;});
({union Cyc_CfFlowInfo_FlowInfo _tmp721=Cyc_NewControlFlow_add_vars(_tmp125,_tmp2AA,& _tmp294,_tmp125->unknown_all,e->loc,0);_tmp2AA=_tmp721;});{
# 1644
struct _tuple16 _tmp295=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2AA,_tmp2F0);struct _tuple16 _tmp296=_tmp295;union Cyc_CfFlowInfo_FlowInfo _tmp29C;void*_tmp29B;_LL1A2: _tmp29C=_tmp296.f1;_tmp29B=_tmp296.f2;_LL1A3:;
{union Cyc_CfFlowInfo_FlowInfo _tmp297=_tmp29C;struct Cyc_Dict_Dict _tmp29A;if((_tmp297.BottomFL).tag == 1){_LL1A5: _LL1A6:
 return({struct _tuple16 _tmp5F4;_tmp5F4.f1=_tmp29C,_tmp5F4.f2=_tmp125->unknown_all;_tmp5F4;});}else{_LL1A7: _tmp29A=((_tmp297.ReachableFL).val).f1;_LL1A8:
# 1648
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp29A,_tmp29B)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp298=0U;({unsigned int _tmp723=_tmp2F1->loc;struct _dyneither_ptr _tmp722=({const char*_tmp299="expression may not be initialized";_tag_dyneither(_tmp299,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp723,_tmp722,_tag_dyneither(_tmp298,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5F5;({union Cyc_CfFlowInfo_FlowInfo _tmp724=Cyc_CfFlowInfo_BottomFL();_tmp5F5.f1=_tmp724;}),_tmp5F5.f2=_tmp125->unknown_all;_tmp5F5;});}}_LL1A4:;}
# 1653
_tmp2AA=_tmp29C;
goto _LL1A0;};}default: _LL19F: _LL1A0:
# 1656
 while(1){
struct Cyc_List_List _tmp29D=({struct Cyc_List_List _tmp5F8;_tmp5F8.hd=_tmp2F2,_tmp5F8.tl=0;_tmp5F8;});
({union Cyc_CfFlowInfo_FlowInfo _tmp725=Cyc_NewControlFlow_add_vars(_tmp125,_tmp2AA,& _tmp29D,_tmp125->unknown_all,e->loc,0);_tmp2AA=_tmp725;});{
struct _tuple16 _tmp29E=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp2AA,_tmp2F0);struct _tuple16 _tmp29F=_tmp29E;union Cyc_CfFlowInfo_FlowInfo _tmp2A6;void*_tmp2A5;_LL1AA: _tmp2A6=_tmp29F.f1;_tmp2A5=_tmp29F.f2;_LL1AB:;
{union Cyc_CfFlowInfo_FlowInfo _tmp2A0=_tmp2A6;struct Cyc_Dict_Dict _tmp2A3;if((_tmp2A0.BottomFL).tag == 1){_LL1AD: _LL1AE:
 goto _LL1AC;}else{_LL1AF: _tmp2A3=((_tmp2A0.ReachableFL).val).f1;_LL1B0:
# 1663
 if(Cyc_CfFlowInfo_initlevel(_tmp125,_tmp2A3,_tmp2A5)!= Cyc_CfFlowInfo_AllIL){
({void*_tmp2A1=0U;({unsigned int _tmp727=_tmp2F1->loc;struct _dyneither_ptr _tmp726=({const char*_tmp2A2="expression may not be initialized";_tag_dyneither(_tmp2A2,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp727,_tmp726,_tag_dyneither(_tmp2A1,sizeof(void*),0U));});});
return({struct _tuple16 _tmp5F7;({union Cyc_CfFlowInfo_FlowInfo _tmp728=Cyc_CfFlowInfo_BottomFL();_tmp5F7.f1=_tmp728;}),_tmp5F7.f2=_tmp125->unknown_all;_tmp5F7;});}}_LL1AC:;}{
# 1668
union Cyc_CfFlowInfo_FlowInfo _tmp2A4=Cyc_CfFlowInfo_join_flow(_tmp125,_tmp2AA,_tmp2A6);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp2A4,_tmp2AA))
break;
_tmp2AA=_tmp2A4;};};}
# 1673
return({struct _tuple16 _tmp5F9;_tmp5F9.f1=_tmp2AA,_tmp5F9.f2=_tmp125->unknown_all;_tmp5F9;});}_LL198:;};};}_LL193:;};}case 28U: _LL70: _tmp2F5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_tmp2F4=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp138)->f2;_tmp2F3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp138)->f3;_LL71: {
# 1679
void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3->tag=1U,_tmp2B3->f1=_tmp2F5,_tmp2B3->f2=(void*)_check_null(e->topt);_tmp2B3;});
struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp2B2=_cycalloc(sizeof(*_tmp2B2));_tmp2B2->root=root,_tmp2B2->path=0;_tmp2B2;});
void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));_tmp2B1->tag=4U,_tmp2B1->f1=place;_tmp2B1;});
void*place_val;
# 1687
({void*_tmp729=Cyc_CfFlowInfo_typ_to_absrval(_tmp125,_tmp2F4,0,_tmp125->unknown_none);place_val=_tmp729;});{
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple16 _tmp2AB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp2F5);struct _tuple16 _tmp2AC=_tmp2AB;union Cyc_CfFlowInfo_FlowInfo _tmp2B0;_LL1B2: _tmp2B0=_tmp2AC.f1;_LL1B3:;
outflow=_tmp2B0;{
union Cyc_CfFlowInfo_FlowInfo _tmp2AD=outflow;struct Cyc_Dict_Dict _tmp2AF;struct Cyc_List_List*_tmp2AE;if((_tmp2AD.BottomFL).tag == 1){_LL1B5: _LL1B6:
 return({struct _tuple16 _tmp5FA;_tmp5FA.f1=outflow,_tmp5FA.f2=rval;_tmp5FA;});}else{_LL1B7: _tmp2AF=((_tmp2AD.ReachableFL).val).f1;_tmp2AE=((_tmp2AD.ReachableFL).val).f2;_LL1B8:
# 1694
 return({struct _tuple16 _tmp5FB;({union Cyc_CfFlowInfo_FlowInfo _tmp72B=({struct Cyc_Dict_Dict _tmp72A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2AF,root,place_val);Cyc_CfFlowInfo_ReachableFL(_tmp72A,_tmp2AE);});_tmp5FB.f1=_tmp72B;}),_tmp5FB.f2=rval;_tmp5FB;});}_LL1B4:;};};}case 37U: _LL72: _tmp2F6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp138)->f1;_LL73: {
# 1698
struct _tuple17 _tmp2B4=({struct _tuple17 _tmp5FD;_tmp5FD.f1=(env->fenv)->unknown_all,_tmp5FD.f2=copy_ctxt;_tmp5FD;});
union Cyc_CfFlowInfo_FlowInfo _tmp2B5=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp2F6,& _tmp2B4);
return({struct _tuple16 _tmp5FC;_tmp5FC.f1=_tmp2B5,_tmp5FC.f2=_tmp2B4.f1;_tmp5FC;});}case 36U: _LL76: _LL77:
# 1703
 goto _LL79;case 25U: _LL78: _LL79:
 goto _LL7B;case 39U: _LL7A: _LL7B:
 goto _LL7D;default: _LL7C: _LL7D:
({void*_tmp2B6=0U;({struct _dyneither_ptr _tmp72C=({const char*_tmp2B7="anal_Rexp, unexpected exp form";_tag_dyneither(_tmp2B7,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp72C,_tag_dyneither(_tmp2B6,sizeof(void*),0U));});});}_LLD:;};}
# 1718 "new_control_flow.cyc"
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place_possibly_null,struct Cyc_List_List*path){
# 1727
struct Cyc_CfFlowInfo_FlowEnv*_tmp2F7=env->fenv;
void*_tmp2F8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2F9=_tmp2F8;void*_tmp31D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F9)->tag == 3U){_LL1: _tmp31D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F9)->f1).ptr_atts).bounds;_LL2: {
# 1730
union Cyc_CfFlowInfo_FlowInfo _tmp2FA=f;struct Cyc_Dict_Dict _tmp31A;struct Cyc_List_List*_tmp319;if((_tmp2FA.BottomFL).tag == 1){_LL6: _LL7:
 return({struct _tuple18 _tmp5FE;_tmp5FE.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp72D=Cyc_CfFlowInfo_UnknownL();_tmp5FE.f2=_tmp72D;});_tmp5FE;});}else{_LL8: _tmp31A=((_tmp2FA.ReachableFL).val).f1;_tmp319=((_tmp2FA.ReachableFL).val).f2;_LL9: {
# 1734
struct _tuple15 _tmp2FB=Cyc_CfFlowInfo_unname_rval(r);struct _tuple15 _tmp2FC=_tmp2FB;void*_tmp318;struct Cyc_List_List*_tmp317;_LLB: _tmp318=_tmp2FC.f1;_tmp317=_tmp2FC.f2;_LLC:;
retry: {void*_tmp2FD=_tmp318;void*_tmp30E;enum Cyc_CfFlowInfo_InitLevel _tmp30D;void*_tmp30C;struct Cyc_List_List*_tmp30B;void*_tmp30A;switch(*((int*)_tmp2FD)){case 8U: _LLE: _LLF:
# 1737
({void*_tmp2FE=0U;({struct _dyneither_ptr _tmp72E=({const char*_tmp2FF="named location in anal_derefL";_tag_dyneither(_tmp2FF,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp72E,_tag_dyneither(_tmp2FE,sizeof(void*),0U));});});case 1U: _LL10: _LL11:
# 1739
({void*_tmp72F=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->tag=Cyc_CfFlowInfo_NotZero,_tmp300->f1=_tmp319;_tmp300;});e->annot=_tmp72F;});goto _LLD;case 5U: _LL12: _tmp30A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp2FD)->f1;_LL13:
# 1742
 if(expanded_place_possibly_null)
# 1745
({void*_tmp730=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->tag=Cyc_CfFlowInfo_UnknownZ,_tmp301->f1=_tmp319;_tmp301;});e->annot=_tmp730;});else{
# 1748
void*_tmp302=e->annot;void*_tmp303=_tmp302;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp303)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1F: _LL20:
# 1752
({void*_tmp731=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->tag=Cyc_CfFlowInfo_UnknownZ,_tmp304->f1=_tmp319;_tmp304;});e->annot=_tmp731;});
goto _LL1E;}else{_LL21: _LL22:
# 1755
({void*_tmp732=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305->tag=Cyc_CfFlowInfo_NotZero,_tmp305->f1=_tmp319;_tmp305;});e->annot=_tmp732;});
goto _LL1E;}_LL1E:;}
# 1760
if(Cyc_Tcutil_is_bound_one(_tmp31D))return({struct _tuple18 _tmp5FF;_tmp5FF.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp733=Cyc_CfFlowInfo_UnknownL();_tmp5FF.f2=_tmp733;});_tmp5FF;});
goto _LLD;case 4U: _LL14: _tmp30C=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2FD)->f1)->root;_tmp30B=(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2FD)->f1)->path;_LL15:
# 1764
({void*_tmp734=(void*)({struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306->tag=Cyc_CfFlowInfo_NotZero,_tmp306->f1=_tmp319;_tmp306;});e->annot=_tmp734;});
if(Cyc_Tcutil_is_bound_one(_tmp31D))
return({struct _tuple18 _tmp600;_tmp600.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp736=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp307=_cycalloc(sizeof(*_tmp307));_tmp307->root=_tmp30C,({struct Cyc_List_List*_tmp735=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30B,path);_tmp307->path=_tmp735;});_tmp307;}));_tmp600.f2=_tmp736;});_tmp600;});
goto _LLD;case 0U: _LL16: _LL17:
# 1769
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;
return({struct _tuple18 _tmp601;({union Cyc_CfFlowInfo_FlowInfo _tmp738=Cyc_CfFlowInfo_BottomFL();_tmp601.f1=_tmp738;}),({union Cyc_CfFlowInfo_AbsLVal _tmp737=Cyc_CfFlowInfo_UnknownL();_tmp601.f2=_tmp737;});_tmp601;});case 2U: _LL18: _tmp30D=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp2FD)->f1;_LL19:
# 1774
 if(Cyc_Tcutil_is_bound_one(_tmp31D))
({union Cyc_CfFlowInfo_FlowInfo _tmp739=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp30D,_tmp317);f=_tmp739;});
({void*_tmp73A=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp308=_cycalloc(sizeof(*_tmp308));_tmp308->tag=Cyc_CfFlowInfo_UnknownZ,_tmp308->f1=_tmp319;_tmp308;});e->annot=_tmp73A;});
goto _LLD;case 7U: _LL1A: _tmp30E=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp2FD)->f3;_LL1B:
# 1779
 if(passthrough_consumes){
_tmp318=_tmp30E;goto retry;}
# 1782
goto _LL1D;default: _LL1C: _LL1D:
# 1784
({void*_tmp73B=(void*)({struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp309=_cycalloc(sizeof(*_tmp309));_tmp309->tag=Cyc_CfFlowInfo_UnknownZ,_tmp309->f1=_tmp319;_tmp309;});e->annot=_tmp73B;});
goto _LLD;}_LLD:;}
# 1787
if(Cyc_CfFlowInfo_initlevel(_tmp2F7,_tmp31A,_tmp318)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp30F=Cyc_CfFlowInfo_unname_rval(_tmp318);struct _tuple15 _tmp310=_tmp30F;void*_tmp316;_LL24: _tmp316=_tmp310.f1;_LL25:;{
void*_tmp311=_tmp316;if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp311)->tag == 7U){_LL27: _LL28:
# 1791
({void*_tmp312=0U;({unsigned int _tmp73D=e->loc;struct _dyneither_ptr _tmp73C=({const char*_tmp313="attempt to dereference an alias-free that has already been copied";_tag_dyneither(_tmp313,sizeof(char),66U);});Cyc_CfFlowInfo_aerr(_tmp73D,_tmp73C,_tag_dyneither(_tmp312,sizeof(void*),0U));});});
goto _LL26;}else{_LL29: _LL2A:
# 1794
({void*_tmp314=0U;({unsigned int _tmp73F=e->loc;struct _dyneither_ptr _tmp73E=({const char*_tmp315="dereference of possibly uninitialized pointer";_tag_dyneither(_tmp315,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp73F,_tmp73E,_tag_dyneither(_tmp314,sizeof(void*),0U));});});
goto _LL26;}_LL26:;};}
# 1798
return({struct _tuple18 _tmp602;_tmp602.f1=f,({union Cyc_CfFlowInfo_AbsLVal _tmp740=Cyc_CfFlowInfo_UnknownL();_tmp602.f2=_tmp740;});_tmp602;});}}_LL5:;}}else{_LL3: _LL4:
# 1800
({void*_tmp31B=0U;({struct _dyneither_ptr _tmp741=({const char*_tmp31C="left deref of non-pointer-type";_tag_dyneither(_tmp31C,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp741,_tag_dyneither(_tmp31B,sizeof(void*),0U));});});}_LL0:;}
# 1806
static struct Cyc_CfFlowInfo_Place*Cyc_NewControlFlow_make_expanded_place(struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*path){
struct Cyc_CfFlowInfo_Place*_tmp31E=p;void*_tmp323;struct Cyc_List_List*_tmp322;_LL1: _tmp323=_tmp31E->root;_tmp322=_tmp31E->path;_LL2:;
({struct Cyc_List_List*_tmp743=({struct Cyc_List_List*_tmp320=_cycalloc(sizeof(*_tmp320));({void*_tmp742=(void*)({struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F->tag=1U;_tmp31F;});_tmp320->hd=_tmp742;}),_tmp320->tl=path;_tmp320;});path=_tmp743;});
return({struct Cyc_CfFlowInfo_Place*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->root=_tmp323,({struct Cyc_List_List*_tmp744=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp322,path);_tmp321->path=_tmp744;});_tmp321;});}
# 1816
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*path){
# 1820
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp324=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp325=inflow;struct Cyc_Dict_Dict _tmp327;struct Cyc_List_List*_tmp326;if((_tmp325.BottomFL).tag == 1){_LL1: _LL2:
 return({struct _tuple18 _tmp603;({union Cyc_CfFlowInfo_FlowInfo _tmp746=Cyc_CfFlowInfo_BottomFL();_tmp603.f1=_tmp746;}),({union Cyc_CfFlowInfo_AbsLVal _tmp745=Cyc_CfFlowInfo_UnknownL();_tmp603.f2=_tmp745;});_tmp603;});}else{_LL3: _tmp327=((_tmp325.ReachableFL).val).f1;_tmp326=((_tmp325.ReachableFL).val).f2;_LL4:
# 1825
 d=_tmp327;}_LL0:;}{
# 1829
void*_tmp328=e->r;void*_tmp329=_tmp328;struct Cyc_Absyn_Exp*_tmp380;struct _dyneither_ptr*_tmp37F;struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_Absyn_Exp*_tmp37B;struct _dyneither_ptr*_tmp37A;struct Cyc_Absyn_Vardecl*_tmp379;struct Cyc_Absyn_Vardecl*_tmp378;struct Cyc_Absyn_Vardecl*_tmp377;struct Cyc_Absyn_Vardecl*_tmp376;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp373;switch(*((int*)_tmp329)){case 14U: _LL6: _tmp373=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp329)->f2;_LL7:
 _tmp374=_tmp373;goto _LL9;case 12U: _LL8: _tmp374=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp329)->f1;_LL9:
 _tmp375=_tmp374;goto _LLB;case 13U: _LLA: _tmp375=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp329)->f1;_LLB:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp375,path);case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp329)->f1)){case 3U: _LLC: _tmp376=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp329)->f1)->f1;_LLD:
# 1834
 _tmp377=_tmp376;goto _LLF;case 4U: _LLE: _tmp377=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp329)->f1)->f1;_LLF:
 _tmp378=_tmp377;goto _LL11;case 5U: _LL10: _tmp378=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp329)->f1)->f1;_LL11:
# 1837
 return({struct _tuple18 _tmp604;_tmp604.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp748=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp32B=_cycalloc(sizeof(*_tmp32B));({void*_tmp747=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->tag=0U,_tmp32A->f1=_tmp378;_tmp32A;});_tmp32B->root=_tmp747;}),_tmp32B->path=path;_tmp32B;}));_tmp604.f2=_tmp748;});_tmp604;});case 1U: _LL12: _tmp379=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp329)->f1)->f1;_LL13:
# 1839
 return({struct _tuple18 _tmp605;_tmp605.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp749=Cyc_CfFlowInfo_UnknownL();_tmp605.f2=_tmp749;});_tmp605;});default: goto _LL1C;}case 22U: _LL14: _tmp37B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp329)->f1;_tmp37A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp329)->f2;_LL15:
# 1842
{void*_tmp32C=Cyc_Tcutil_compress((void*)_check_null(_tmp37B->topt));void*_tmp32D=_tmp32C;void*_tmp332;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32D)->tag == 3U){_LL1F: _tmp332=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32D)->f1).elt_type;_LL20:
# 1844
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp332)){
Cyc_NewControlFlow_check_union_requires(_tmp37B->loc,_tmp332,_tmp37A,inflow);
({struct Cyc_List_List*_tmp74C=({struct Cyc_List_List*_tmp32F=_cycalloc(sizeof(*_tmp32F));({void*_tmp74B=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->tag=0U,({int _tmp74A=Cyc_CfFlowInfo_get_field_index(_tmp332,_tmp37A);_tmp32E->f1=_tmp74A;});_tmp32E;});_tmp32F->hd=_tmp74B;}),_tmp32F->tl=path;_tmp32F;});path=_tmp74C;});}
# 1848
goto _LL1E;}else{_LL21: _LL22:
({void*_tmp330=0U;({struct _dyneither_ptr _tmp74D=({const char*_tmp331="anal_Lexp: AggrArrow ptr";_tag_dyneither(_tmp331,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp74D,_tag_dyneither(_tmp330,sizeof(void*),0U));});});}_LL1E:;}
# 1851
_tmp37C=_tmp37B;goto _LL17;case 20U: _LL16: _tmp37C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp329)->f1;_LL17:
# 1855
 if(Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp37C->topt),1)){
# 1857
struct _tuple18 _tmp333=Cyc_NewControlFlow_anal_Lexp(env,inflow,expand_unique,passthrough_consumes,_tmp37C);struct _tuple18 _tmp334=_tmp333;union Cyc_CfFlowInfo_FlowInfo _tmp353;union Cyc_CfFlowInfo_AbsLVal _tmp352;_LL24: _tmp353=_tmp334.f1;_tmp352=_tmp334.f2;_LL25:;{
struct _tuple18 _tmp335=({struct _tuple18 _tmp609;_tmp609.f1=_tmp353,_tmp609.f2=_tmp352;_tmp609;});struct _tuple18 _tmp336=_tmp335;struct Cyc_Dict_Dict _tmp351;struct Cyc_List_List*_tmp350;struct Cyc_CfFlowInfo_Place*_tmp34F;if(((_tmp336.f1).ReachableFL).tag == 2){if(((_tmp336.f2).PlaceL).tag == 1){_LL27: _tmp351=(((_tmp336.f1).ReachableFL).val).f1;_tmp350=(((_tmp336.f1).ReachableFL).val).f2;_tmp34F=((_tmp336.f2).PlaceL).val;_LL28: {
# 1860
void*_tmp337=Cyc_CfFlowInfo_lookup_place(_tmp351,_tmp34F);
struct _tuple15 _tmp338=Cyc_CfFlowInfo_unname_rval(_tmp337);struct _tuple15 _tmp339=_tmp338;void*_tmp34E;struct Cyc_List_List*_tmp34D;_LL2C: _tmp34E=_tmp339.f1;_tmp34D=_tmp339.f2;_LL2D:;
if(expand_unique){
int _tmp33A=1;
void*_tmp33B=_tmp34E;struct Cyc_Absyn_Vardecl*_tmp348;void*_tmp347;switch(*((int*)_tmp33B)){case 8U: _LL2F: _tmp348=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp33B)->f1;_tmp347=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp33B)->f2;_LL30:
# 1866
({void*_tmp33C=0U;({struct _dyneither_ptr _tmp74E=({const char*_tmp33D="bad named location in anal_Lexp:deref";_tag_dyneither(_tmp33D,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp74E,_tag_dyneither(_tmp33C,sizeof(void*),0U));});});case 7U: switch(*((int*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp33B)->f3)){case 4U: _LL31: _LL32:
 goto _LL34;case 5U: _LL35: _LL36:
# 1870
 goto _LL38;default: goto _LL3B;}case 4U: _LL33: _LL34:
# 1869
 return Cyc_NewControlFlow_anal_derefL(env,_tmp353,_tmp37C,_tmp353,_tmp337,passthrough_consumes,0,path);case 5U: _LL37: _LL38: {
# 1874
struct _tuple18 _tmp33E=Cyc_NewControlFlow_anal_derefL(env,_tmp353,_tmp37C,_tmp353,_tmp337,passthrough_consumes,0,path);struct _tuple18 _tmp33F=_tmp33E;union Cyc_CfFlowInfo_FlowInfo _tmp340;_LL3E: _tmp340=_tmp33F.f1;_LL3F:;
return({struct _tuple18 _tmp606;_tmp606.f1=_tmp340,({union Cyc_CfFlowInfo_AbsLVal _tmp74F=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp34F,path));_tmp606.f2=_tmp74F;});_tmp606;});}case 1U: _LL39: _LL3A:
# 1877
 _tmp33A=0;goto _LL3C;default: _LL3B: _LL3C: {
# 1884
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp324,_tmp351,_tmp34E);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp324->unknown_all: _tmp324->unknown_none;
void*res=(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp346=_cycalloc(sizeof(*_tmp346));_tmp346->tag=5U,({void*_tmp752=({struct Cyc_CfFlowInfo_FlowEnv*_tmp751=_tmp324;void*_tmp750=Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp37C->topt));Cyc_CfFlowInfo_typ_to_absrval(_tmp751,_tmp750,0,leaf);});_tmp346->f1=_tmp752;});_tmp346;});
for(0;_tmp34D != 0;_tmp34D=_tmp34D->tl){
({void*_tmp753=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341->tag=8U,_tmp341->f1=(struct Cyc_Absyn_Vardecl*)_tmp34D->hd,_tmp341->f2=res;_tmp341;});res=_tmp753;});}
({struct Cyc_Dict_Dict _tmp754=Cyc_CfFlowInfo_assign_place(_tmp324,e->loc,_tmp351,_tmp34F,res);_tmp351=_tmp754;});{
union Cyc_CfFlowInfo_FlowInfo _tmp342=Cyc_CfFlowInfo_ReachableFL(_tmp351,_tmp350);
# 1897
struct _tuple18 _tmp343=Cyc_NewControlFlow_anal_derefL(env,_tmp342,_tmp37C,_tmp342,res,passthrough_consumes,_tmp33A,path);struct _tuple18 _tmp344=_tmp343;union Cyc_CfFlowInfo_FlowInfo _tmp345;_LL41: _tmp345=_tmp344.f1;_LL42:;
# 1899
return({struct _tuple18 _tmp607;_tmp607.f1=_tmp345,({union Cyc_CfFlowInfo_AbsLVal _tmp755=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp34F,path));_tmp607.f2=_tmp755;});_tmp607;});};}}_LL2E:;}else{
# 1903
void*_tmp349=_tmp34E;switch(*((int*)_tmp349)){case 7U: if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp349)->f3)->tag == 5U){_LL44: _LL45:
 goto _LL47;}else{goto _LL48;}case 5U: _LL46: _LL47: {
# 1908
struct _tuple18 _tmp34A=Cyc_NewControlFlow_anal_derefL(env,_tmp353,_tmp37C,_tmp353,_tmp337,passthrough_consumes,0,path);struct _tuple18 _tmp34B=_tmp34A;union Cyc_CfFlowInfo_FlowInfo _tmp34C;_LL4B: _tmp34C=_tmp34B.f1;_LL4C:;
return({struct _tuple18 _tmp608;_tmp608.f1=_tmp34C,({union Cyc_CfFlowInfo_AbsLVal _tmp756=Cyc_CfFlowInfo_PlaceL(Cyc_NewControlFlow_make_expanded_place(_tmp34F,path));_tmp608.f2=_tmp756;});_tmp608;});}default: _LL48: _LL49:
# 1912
 goto _LL43;}_LL43:;}
# 1915
goto _LL26;}}else{goto _LL29;}}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;};}{
# 1920
struct _tuple16 _tmp354=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp37C);struct _tuple16 _tmp355=_tmp354;union Cyc_CfFlowInfo_FlowInfo _tmp357;void*_tmp356;_LL4E: _tmp357=_tmp355.f1;_tmp356=_tmp355.f2;_LL4F:;
# 1922
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp37C,_tmp357,_tmp356,passthrough_consumes,0,path);};case 23U: _LL18: _tmp37E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp329)->f1;_tmp37D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp329)->f2;_LL19: {
# 1925
void*_tmp358=Cyc_Tcutil_compress((void*)_check_null(_tmp37E->topt));void*_tmp359=_tmp358;switch(*((int*)_tmp359)){case 6U: _LL51: _LL52: {
# 1927
unsigned int _tmp35A=(Cyc_Evexp_eval_const_uint_exp(_tmp37D)).f1;
return({struct Cyc_NewControlFlow_AnalEnv*_tmp75C=env;union Cyc_CfFlowInfo_FlowInfo _tmp75B=inflow;int _tmp75A=expand_unique;int _tmp759=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp758=_tmp37E;Cyc_NewControlFlow_anal_Lexp_rec(_tmp75C,_tmp75B,_tmp75A,_tmp759,_tmp758,({struct Cyc_List_List*_tmp35C=_cycalloc(sizeof(*_tmp35C));
({void*_tmp757=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->tag=0U,_tmp35B->f1=(int)_tmp35A;_tmp35B;});_tmp35C->hd=_tmp757;}),_tmp35C->tl=path;_tmp35C;}));});}case 3U: _LL53: _LL54: {
# 1931
struct _tuple23 _tmp35D=({struct Cyc_NewControlFlow_AnalEnv*_tmp75E=env;union Cyc_CfFlowInfo_FlowInfo _tmp75D=inflow;Cyc_NewControlFlow_anal_Rexps(_tmp75E,_tmp75D,({struct Cyc_Absyn_Exp*_tmp36D[2U];_tmp36D[0]=_tmp37E,_tmp36D[1]=_tmp37D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0,1);});struct _tuple23 _tmp35E=_tmp35D;union Cyc_CfFlowInfo_FlowInfo _tmp36C;struct Cyc_List_List*_tmp36B;_LL58: _tmp36C=_tmp35E.f1;_tmp36B=_tmp35E.f2;_LL59:;{
union Cyc_CfFlowInfo_FlowInfo _tmp35F=_tmp36C;
{union Cyc_CfFlowInfo_FlowInfo _tmp360=_tmp36C;struct Cyc_Dict_Dict _tmp365;struct Cyc_List_List*_tmp364;if((_tmp360.ReachableFL).tag == 2){_LL5B: _tmp365=((_tmp360.ReachableFL).val).f1;_tmp364=((_tmp360.ReachableFL).val).f2;_LL5C:
# 1935
 if(Cyc_CfFlowInfo_initlevel(_tmp324,_tmp365,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp36B))->tl))->hd)== Cyc_CfFlowInfo_NoneIL)
({void*_tmp361=0U;({unsigned int _tmp760=_tmp37D->loc;struct _dyneither_ptr _tmp75F=({const char*_tmp362="expression may not be initialized";_tag_dyneither(_tmp362,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp760,_tmp75F,_tag_dyneither(_tmp361,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp363=Cyc_NewControlFlow_add_subscript_reln(_tmp364,_tmp37E,_tmp37D);
if(_tmp364 != _tmp363)
({union Cyc_CfFlowInfo_FlowInfo _tmp761=Cyc_CfFlowInfo_ReachableFL(_tmp365,_tmp363);_tmp35F=_tmp761;});
goto _LL5A;};}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}{
# 1944
struct _tuple18 _tmp366=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp37E,_tmp36C,(void*)((struct Cyc_List_List*)_check_null(_tmp36B))->hd,passthrough_consumes,0,path);struct _tuple18 _tmp367=_tmp366;union Cyc_CfFlowInfo_FlowInfo _tmp36A;union Cyc_CfFlowInfo_AbsLVal _tmp369;_LL60: _tmp36A=_tmp367.f1;_tmp369=_tmp367.f2;_LL61:;{
union Cyc_CfFlowInfo_FlowInfo _tmp368=_tmp36A;if((_tmp368.BottomFL).tag == 1){_LL63: _LL64:
 return({struct _tuple18 _tmp60A;_tmp60A.f1=_tmp36A,_tmp60A.f2=_tmp369;_tmp60A;});}else{_LL65: _LL66:
 return({struct _tuple18 _tmp60B;_tmp60B.f1=_tmp35F,_tmp60B.f2=_tmp369;_tmp60B;});}_LL62:;};};};}default: _LL55: _LL56:
# 1949
({void*_tmp36E=0U;({struct _dyneither_ptr _tmp762=({const char*_tmp36F="anal_Lexp: Subscript -- bad type";_tag_dyneither(_tmp36F,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp762,_tag_dyneither(_tmp36E,sizeof(void*),0U));});});}_LL50:;}case 21U: _LL1A: _tmp380=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp329)->f1;_tmp37F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp329)->f2;_LL1B: {
# 1953
void*_tmp370=(void*)_check_null(_tmp380->topt);
if(Cyc_Absyn_is_require_union_type(_tmp370))
Cyc_NewControlFlow_check_union_requires(_tmp380->loc,_tmp370,_tmp37F,inflow);
# 1957
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp370))
return({struct _tuple18 _tmp60C;_tmp60C.f1=inflow,({union Cyc_CfFlowInfo_AbsLVal _tmp763=Cyc_CfFlowInfo_UnknownL();_tmp60C.f2=_tmp763;});_tmp60C;});
# 1960
return({struct Cyc_NewControlFlow_AnalEnv*_tmp76A=env;union Cyc_CfFlowInfo_FlowInfo _tmp769=inflow;int _tmp768=expand_unique;int _tmp767=passthrough_consumes;struct Cyc_Absyn_Exp*_tmp766=_tmp380;Cyc_NewControlFlow_anal_Lexp_rec(_tmp76A,_tmp769,_tmp768,_tmp767,_tmp766,({struct Cyc_List_List*_tmp372=_cycalloc(sizeof(*_tmp372));
({void*_tmp765=(void*)({struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->tag=0U,({int _tmp764=Cyc_CfFlowInfo_get_field_index(_tmp370,_tmp37F);_tmp371->f1=_tmp764;});_tmp371;});_tmp372->hd=_tmp765;}),_tmp372->tl=path;_tmp372;}));});}default: _LL1C: _LL1D:
# 1964
 return({struct _tuple18 _tmp60D;({union Cyc_CfFlowInfo_FlowInfo _tmp76C=Cyc_CfFlowInfo_BottomFL();_tmp60D.f1=_tmp76C;}),({union Cyc_CfFlowInfo_AbsLVal _tmp76B=Cyc_CfFlowInfo_UnknownL();_tmp60D.f2=_tmp76B;});_tmp60D;});}_LL5:;};}
# 1968
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1972
struct _tuple18 _tmp381=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);struct _tuple18 _tmp382=_tmp381;union Cyc_CfFlowInfo_FlowInfo _tmp384;union Cyc_CfFlowInfo_AbsLVal _tmp383;_LL1: _tmp384=_tmp382.f1;_tmp383=_tmp382.f2;_LL2:;
return({struct _tuple18 _tmp60E;_tmp60E.f1=_tmp384,_tmp60E.f2=_tmp383;_tmp60E;});}
# 1979
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1982
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp385=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);struct _tuple18 _tmp386=_tmp385;union Cyc_CfFlowInfo_FlowInfo _tmp387;_LL1: _tmp387=_tmp386.f1;_LL2:;
inflow=_tmp387;}}}
# 1990
return inflow;}
# 1996
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1998
struct Cyc_CfFlowInfo_FlowEnv*_tmp388=env->fenv;
# 2001
struct _tuple16 _tmp389=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);struct _tuple16 _tmp38A=_tmp389;union Cyc_CfFlowInfo_FlowInfo _tmp3C7;void*_tmp3C6;_LL1: _tmp3C7=_tmp38A.f1;_tmp3C6=_tmp38A.f2;_LL2:;{
struct _tuple16 _tmp38B=Cyc_NewControlFlow_anal_Rexp(env,0,_tmp3C7,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);struct _tuple16 _tmp38C=_tmp38B;union Cyc_CfFlowInfo_FlowInfo _tmp3C5;void*_tmp3C4;_LL4: _tmp3C5=_tmp38C.f1;_tmp3C4=_tmp38C.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo _tmp38D=_tmp3C5;
# 2006
union Cyc_CfFlowInfo_FlowInfo _tmp38E=_tmp38D;struct Cyc_Dict_Dict _tmp3C3;struct Cyc_List_List*_tmp3C2;if((_tmp38E.BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple19 _tmp60F;_tmp60F.f1=_tmp38D,_tmp60F.f2=_tmp38D;_tmp60F;});}else{_LL9: _tmp3C3=((_tmp38E.ReachableFL).val).f1;_tmp3C2=((_tmp38E.ReachableFL).val).f2;_LLA: {
# 2009
struct Cyc_Absyn_Exp*_tmp38F=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*_tmp390=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3C3,_tmp3C6)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp3C6))
({void*_tmp391=0U;({unsigned int _tmp76E=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct _dyneither_ptr _tmp76D=({const char*_tmp392="expression may not be initialized";_tag_dyneither(_tmp392,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp76E,_tmp76D,_tag_dyneither(_tmp391,sizeof(void*),0U));});});
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3C3,_tmp3C4)== Cyc_CfFlowInfo_NoneIL  && !Cyc_CfFlowInfo_is_init_pointer(_tmp3C6))
({void*_tmp393=0U;({unsigned int _tmp770=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc;struct _dyneither_ptr _tmp76F=({const char*_tmp394="expression may not be initialized";_tag_dyneither(_tmp394,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp770,_tmp76F,_tag_dyneither(_tmp393,sizeof(void*),0U));});});{
# 2016
union Cyc_CfFlowInfo_FlowInfo _tmp395=_tmp38D;
union Cyc_CfFlowInfo_FlowInfo _tmp396=_tmp38D;
# 2021
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp397=Cyc_CfFlowInfo_unname_rval(_tmp3C6);struct _tuple15 _tmp398=_tmp397;void*_tmp3B0;struct Cyc_List_List*_tmp3AF;_LLC: _tmp3B0=_tmp398.f1;_tmp3AF=_tmp398.f2;_LLD:;{
struct _tuple15 _tmp399=Cyc_CfFlowInfo_unname_rval(_tmp3C4);struct _tuple15 _tmp39A=_tmp399;void*_tmp3AE;struct Cyc_List_List*_tmp3AD;_LLF: _tmp3AE=_tmp39A.f1;_tmp3AD=_tmp39A.f2;_LL10:;{
struct _tuple0 _tmp39B=({struct _tuple0 _tmp610;_tmp610.f1=_tmp3B0,_tmp610.f2=_tmp3AE;_tmp610;});struct _tuple0 _tmp39C=_tmp39B;enum Cyc_CfFlowInfo_InitLevel _tmp3AC;enum Cyc_CfFlowInfo_InitLevel _tmp3AB;switch(*((int*)_tmp39C.f1)){case 2U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp39C.f2)->tag == 0U){_LL12: _tmp3AB=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp39C.f1)->f1;_LL13: {
# 2026
struct _tuple19 _tmp39D=Cyc_NewControlFlow_splitzero(env,inflow,_tmp38D,_tmp38F,_tmp3AB,_tmp3AF);struct _tuple19 _tmp39E=_tmp39D;union Cyc_CfFlowInfo_FlowInfo _tmp3A3;union Cyc_CfFlowInfo_FlowInfo _tmp3A2;_LL23: _tmp3A3=_tmp39E.f1;_tmp3A2=_tmp39E.f2;_LL24:;
{enum Cyc_Absyn_Primop _tmp39F=p;switch(_tmp39F){case Cyc_Absyn_Eq: _LL26: _LL27:
 _tmp395=_tmp3A2;_tmp396=_tmp3A3;goto _LL25;case Cyc_Absyn_Neq: _LL28: _LL29:
 _tmp395=_tmp3A3;_tmp396=_tmp3A2;goto _LL25;default: _LL2A: _LL2B:
({void*_tmp3A0=0U;({struct _dyneither_ptr _tmp771=({const char*_tmp3A1="anal_test, zero-split";_tag_dyneither(_tmp3A1,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp771,_tag_dyneither(_tmp3A0,sizeof(void*),0U));});});}_LL25:;}
# 2032
goto _LL11;}}else{goto _LL20;}case 0U: switch(*((int*)_tmp39C.f2)){case 2U: _LL14: _tmp3AC=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp39C.f2)->f1;_LL15: {
# 2034
struct _tuple19 _tmp3A4=Cyc_NewControlFlow_splitzero(env,_tmp3C5,_tmp38D,_tmp390,_tmp3AC,_tmp3AD);struct _tuple19 _tmp3A5=_tmp3A4;union Cyc_CfFlowInfo_FlowInfo _tmp3AA;union Cyc_CfFlowInfo_FlowInfo _tmp3A9;_LL2D: _tmp3AA=_tmp3A5.f1;_tmp3A9=_tmp3A5.f2;_LL2E:;
{enum Cyc_Absyn_Primop _tmp3A6=p;switch(_tmp3A6){case Cyc_Absyn_Eq: _LL30: _LL31:
 _tmp395=_tmp3A9;_tmp396=_tmp3AA;goto _LL2F;case Cyc_Absyn_Neq: _LL32: _LL33:
 _tmp395=_tmp3AA;_tmp396=_tmp3A9;goto _LL2F;default: _LL34: _LL35:
({void*_tmp3A7=0U;({struct _dyneither_ptr _tmp772=({const char*_tmp3A8="anal_test, zero-split";_tag_dyneither(_tmp3A8,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp772,_tag_dyneither(_tmp3A7,sizeof(void*),0U));});});}_LL2F:;}
# 2040
goto _LL11;}case 0U: _LL16: _LL17:
# 2042
 if(p == Cyc_Absyn_Eq)({union Cyc_CfFlowInfo_FlowInfo _tmp773=Cyc_CfFlowInfo_BottomFL();_tmp396=_tmp773;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp774=Cyc_CfFlowInfo_BottomFL();_tmp395=_tmp774;});}
goto _LL11;case 1U: _LL18: _LL19:
 goto _LL1B;case 4U: _LL1A: _LL1B:
 goto _LL1D;default: goto _LL20;}case 1U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp39C.f2)->tag == 0U){_LL1C: _LL1D:
 goto _LL1F;}else{goto _LL20;}case 4U: if(((struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp39C.f2)->tag == 0U){_LL1E: _LL1F:
# 2049
 if(p == Cyc_Absyn_Neq)({union Cyc_CfFlowInfo_FlowInfo _tmp775=Cyc_CfFlowInfo_BottomFL();_tmp396=_tmp775;});else{
({union Cyc_CfFlowInfo_FlowInfo _tmp776=Cyc_CfFlowInfo_BottomFL();_tmp395=_tmp776;});}
goto _LL11;}else{goto _LL20;}default: _LL20: _LL21:
 goto _LL11;}_LL11:;};};}
# 2060
{struct _tuple0 _tmp3B1=({struct _tuple0 _tmp612;({void*_tmp778=Cyc_Tcutil_compress((void*)_check_null(_tmp38F->topt));_tmp612.f1=_tmp778;}),({
void*_tmp777=Cyc_Tcutil_compress((void*)_check_null(_tmp390->topt));_tmp612.f2=_tmp777;});_tmp612;});
# 2060
struct _tuple0 _tmp3B2=_tmp3B1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f1)->f1)->f1 == Cyc_Absyn_Unsigned){_LL37: _LL38:
# 2062
 goto _LL3A;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL39;else{goto _LL3F;}case 4U: goto _LL3D;default: goto _LL3F;}else{goto _LL3F;}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->f1)->f1 == Cyc_Absyn_Unsigned)goto _LL39;else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f1)->f1)->tag == 4U)goto _LL3B;else{goto _LL3F;}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f1)->f1)->tag == 4U)goto _LL3B;else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->f1)->tag == 4U)goto _LL3D;else{goto _LL3F;}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f1)->f1)->tag == 4U){_LL3B: _LL3C:
# 2064
 goto _LL3E;}else{goto _LL3F;}}}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2.f2)->f1)->f1 == Cyc_Absyn_Unsigned){_LL39: _LL3A:
# 2063
 goto _LL3C;}else{goto _LL3F;}case 4U: _LL3D: _LL3E:
# 2065
 goto _LL36;default: goto _LL3F;}else{_LL3F: _LL40:
 return({struct _tuple19 _tmp611;_tmp611.f1=_tmp395,_tmp611.f2=_tmp396;_tmp611;});}}_LL36:;}
# 2070
{enum Cyc_Absyn_Primop _tmp3B3=p;switch(_tmp3B3){case Cyc_Absyn_Eq: _LL42: _LL43:
 goto _LL45;case Cyc_Absyn_Neq: _LL44: _LL45: goto _LL47;case Cyc_Absyn_Gt: _LL46: _LL47: goto _LL49;case Cyc_Absyn_Gte: _LL48: _LL49: goto _LL4B;case Cyc_Absyn_Lt: _LL4A: _LL4B: goto _LL4D;case Cyc_Absyn_Lte: _LL4C: _LL4D: goto _LL41;default: _LL4E: _LL4F:
 return({struct _tuple19 _tmp613;_tmp613.f1=_tmp395,_tmp613.f2=_tmp396;_tmp613;});}_LL41:;}{
# 2075
struct _RegionHandle*_tmp3B4=Cyc_Core_heap_region;
struct _tuple13 _tmp3B5=Cyc_Relations_primop2relation(_tmp38F,p,_tmp390);struct _tuple13 _tmp3B6=_tmp3B5;struct Cyc_Absyn_Exp*_tmp3C1;enum Cyc_Relations_Relation _tmp3C0;struct Cyc_Absyn_Exp*_tmp3BF;_LL51: _tmp3C1=_tmp3B6.f1;_tmp3C0=_tmp3B6.f2;_tmp3BF=_tmp3B6.f3;_LL52:;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0U);
# 2080
if(Cyc_Relations_exp2relnop(_tmp3C1,& n1) && Cyc_Relations_exp2relnop(_tmp3BF,& n2)){
# 2082
struct Cyc_List_List*_tmp3B7=Cyc_Relations_add_relation(_tmp3B4,n1,_tmp3C0,n2,_tmp3C2);
# 2086
struct Cyc_List_List*_tmp3B8=({struct _RegionHandle*_tmp77C=_tmp3B4;union Cyc_Relations_RelnOp _tmp77B=n2;enum Cyc_Relations_Relation _tmp77A=Cyc_Relations_flip_relation(_tmp3C0);union Cyc_Relations_RelnOp _tmp779=n1;Cyc_Relations_add_relation(_tmp77C,_tmp77B,_tmp77A,_tmp779,_tmp3C2);});
struct _tuple19 _tmp3B9=({struct _tuple19 _tmp618;_tmp618.f1=_tmp395,_tmp618.f2=_tmp396;_tmp618;});struct _tuple19 _tmp3BA=_tmp3B9;struct Cyc_Dict_Dict _tmp3BE;struct Cyc_Dict_Dict _tmp3BD;struct Cyc_Dict_Dict _tmp3BC;struct Cyc_Dict_Dict _tmp3BB;if(((_tmp3BA.f1).ReachableFL).tag == 2){if(((_tmp3BA.f2).ReachableFL).tag == 2){_LL54: _tmp3BC=(((_tmp3BA.f1).ReachableFL).val).f1;_tmp3BB=(((_tmp3BA.f2).ReachableFL).val).f1;_LL55:
# 2089
 return({struct _tuple19 _tmp614;({union Cyc_CfFlowInfo_FlowInfo _tmp77E=Cyc_CfFlowInfo_ReachableFL(_tmp3BC,_tmp3B7);_tmp614.f1=_tmp77E;}),({union Cyc_CfFlowInfo_FlowInfo _tmp77D=Cyc_CfFlowInfo_ReachableFL(_tmp3BB,_tmp3B8);_tmp614.f2=_tmp77D;});_tmp614;});}else{_LL58: _tmp3BD=(((_tmp3BA.f1).ReachableFL).val).f1;_LL59:
# 2093
 return({struct _tuple19 _tmp615;({union Cyc_CfFlowInfo_FlowInfo _tmp77F=Cyc_CfFlowInfo_ReachableFL(_tmp3BD,_tmp3B7);_tmp615.f1=_tmp77F;}),_tmp615.f2=_tmp396;_tmp615;});}}else{if(((_tmp3BA.f2).ReachableFL).tag == 2){_LL56: _tmp3BE=(((_tmp3BA.f2).ReachableFL).val).f1;_LL57:
# 2091
 return({struct _tuple19 _tmp616;_tmp616.f1=_tmp395,({union Cyc_CfFlowInfo_FlowInfo _tmp780=Cyc_CfFlowInfo_ReachableFL(_tmp3BE,_tmp3B8);_tmp616.f2=_tmp780;});_tmp616;});}else{_LL5A: _LL5B:
# 2095
 return({struct _tuple19 _tmp617;_tmp617.f1=_tmp395,_tmp617.f2=_tmp396;_tmp617;});}}_LL53:;}else{
# 2098
return({struct _tuple19 _tmp619;_tmp619.f1=_tmp395,_tmp619.f2=_tmp396;_tmp619;});}};};};}}_LL6:;};};}
# 2104
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3C8=env->fenv;
void*_tmp3C9=e->r;void*_tmp3CA=_tmp3C9;enum Cyc_Absyn_Primop _tmp40E;struct Cyc_List_List*_tmp40D;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Exp*_tmp40B;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp405;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp403;switch(*((int*)_tmp3CA)){case 6U: _LL1: _tmp405=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp404=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_tmp403=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3CA)->f3;_LL2: {
# 2108
struct _tuple19 _tmp3CB=Cyc_NewControlFlow_anal_test(env,inflow,_tmp405);struct _tuple19 _tmp3CC=_tmp3CB;union Cyc_CfFlowInfo_FlowInfo _tmp3D6;union Cyc_CfFlowInfo_FlowInfo _tmp3D5;_LL10: _tmp3D6=_tmp3CC.f1;_tmp3D5=_tmp3CC.f2;_LL11:;{
struct _tuple19 _tmp3CD=Cyc_NewControlFlow_anal_test(env,_tmp3D6,_tmp404);struct _tuple19 _tmp3CE=_tmp3CD;union Cyc_CfFlowInfo_FlowInfo _tmp3D4;union Cyc_CfFlowInfo_FlowInfo _tmp3D3;_LL13: _tmp3D4=_tmp3CE.f1;_tmp3D3=_tmp3CE.f2;_LL14:;{
struct _tuple19 _tmp3CF=Cyc_NewControlFlow_anal_test(env,_tmp3D5,_tmp403);struct _tuple19 _tmp3D0=_tmp3CF;union Cyc_CfFlowInfo_FlowInfo _tmp3D2;union Cyc_CfFlowInfo_FlowInfo _tmp3D1;_LL16: _tmp3D2=_tmp3D0.f1;_tmp3D1=_tmp3D0.f2;_LL17:;
return({struct _tuple19 _tmp61A;({union Cyc_CfFlowInfo_FlowInfo _tmp782=Cyc_CfFlowInfo_join_flow(_tmp3C8,_tmp3D4,_tmp3D2);_tmp61A.f1=_tmp782;}),({
union Cyc_CfFlowInfo_FlowInfo _tmp781=Cyc_CfFlowInfo_join_flow(_tmp3C8,_tmp3D3,_tmp3D1);_tmp61A.f2=_tmp781;});_tmp61A;});};};}case 7U: _LL3: _tmp407=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp406=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL4: {
# 2114
struct _tuple19 _tmp3D7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp407);struct _tuple19 _tmp3D8=_tmp3D7;union Cyc_CfFlowInfo_FlowInfo _tmp3DE;union Cyc_CfFlowInfo_FlowInfo _tmp3DD;_LL19: _tmp3DE=_tmp3D8.f1;_tmp3DD=_tmp3D8.f2;_LL1A:;{
struct _tuple19 _tmp3D9=Cyc_NewControlFlow_anal_test(env,_tmp3DE,_tmp406);struct _tuple19 _tmp3DA=_tmp3D9;union Cyc_CfFlowInfo_FlowInfo _tmp3DC;union Cyc_CfFlowInfo_FlowInfo _tmp3DB;_LL1C: _tmp3DC=_tmp3DA.f1;_tmp3DB=_tmp3DA.f2;_LL1D:;
return({struct _tuple19 _tmp61B;_tmp61B.f1=_tmp3DC,({union Cyc_CfFlowInfo_FlowInfo _tmp783=Cyc_CfFlowInfo_join_flow(_tmp3C8,_tmp3DD,_tmp3DB);_tmp61B.f2=_tmp783;});_tmp61B;});};}case 8U: _LL5: _tmp409=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp408=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL6: {
# 2118
struct _tuple19 _tmp3DF=Cyc_NewControlFlow_anal_test(env,inflow,_tmp409);struct _tuple19 _tmp3E0=_tmp3DF;union Cyc_CfFlowInfo_FlowInfo _tmp3E6;union Cyc_CfFlowInfo_FlowInfo _tmp3E5;_LL1F: _tmp3E6=_tmp3E0.f1;_tmp3E5=_tmp3E0.f2;_LL20:;{
struct _tuple19 _tmp3E1=Cyc_NewControlFlow_anal_test(env,_tmp3E5,_tmp408);struct _tuple19 _tmp3E2=_tmp3E1;union Cyc_CfFlowInfo_FlowInfo _tmp3E4;union Cyc_CfFlowInfo_FlowInfo _tmp3E3;_LL22: _tmp3E4=_tmp3E2.f1;_tmp3E3=_tmp3E2.f2;_LL23:;
return({struct _tuple19 _tmp61C;({union Cyc_CfFlowInfo_FlowInfo _tmp784=Cyc_CfFlowInfo_join_flow(_tmp3C8,_tmp3E6,_tmp3E4);_tmp61C.f1=_tmp784;}),_tmp61C.f2=_tmp3E3;_tmp61C;});};}case 9U: _LL7: _tmp40B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp40A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL8: {
# 2122
struct _tuple16 _tmp3E7=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp40B);struct _tuple16 _tmp3E8=_tmp3E7;union Cyc_CfFlowInfo_FlowInfo _tmp3EA;void*_tmp3E9;_LL25: _tmp3EA=_tmp3E8.f1;_tmp3E9=_tmp3E8.f2;_LL26:;
return Cyc_NewControlFlow_anal_test(env,_tmp3EA,_tmp40A);}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2)->tl == 0){_LL9: _tmp40C=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2)->hd;_LLA: {
# 2125
struct _tuple19 _tmp3EB=Cyc_NewControlFlow_anal_test(env,inflow,_tmp40C);struct _tuple19 _tmp3EC=_tmp3EB;union Cyc_CfFlowInfo_FlowInfo _tmp3EE;union Cyc_CfFlowInfo_FlowInfo _tmp3ED;_LL28: _tmp3EE=_tmp3EC.f1;_tmp3ED=_tmp3EC.f2;_LL29:;
return({struct _tuple19 _tmp61D;_tmp61D.f1=_tmp3ED,_tmp61D.f2=_tmp3EE;_tmp61D;});}}else{goto _LLB;}}else{goto _LLB;}}else{_LLB: _tmp40E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CA)->f1;_tmp40D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LLC:
# 2128
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp40E,_tmp40D);}default: _LLD: _LLE: {
# 2132
struct _tuple16 _tmp3EF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);struct _tuple16 _tmp3F0=_tmp3EF;union Cyc_CfFlowInfo_FlowInfo _tmp402;void*_tmp401;_LL2B: _tmp402=_tmp3F0.f1;_tmp401=_tmp3F0.f2;_LL2C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp3F1=_tmp402;struct Cyc_Dict_Dict _tmp400;if((_tmp3F1.BottomFL).tag == 1){_LL2E: _LL2F:
 return({struct _tuple19 _tmp61E;_tmp61E.f1=_tmp402,_tmp61E.f2=_tmp402;_tmp61E;});}else{_LL30: _tmp400=((_tmp3F1.ReachableFL).val).f1;_LL31: {
# 2136
struct _tuple15 _tmp3F2=Cyc_CfFlowInfo_unname_rval(_tmp401);struct _tuple15 _tmp3F3=_tmp3F2;void*_tmp3FF;struct Cyc_List_List*_tmp3FE;_LL33: _tmp3FF=_tmp3F3.f1;_tmp3FE=_tmp3F3.f2;_LL34:;{
void*_tmp3F4=_tmp3FF;enum Cyc_CfFlowInfo_InitLevel _tmp3FD;struct Cyc_Absyn_Vardecl*_tmp3FC;void*_tmp3FB;switch(*((int*)_tmp3F4)){case 8U: _LL36: _tmp3FC=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3F4)->f1;_tmp3FB=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp3F4)->f2;_LL37:
# 2139
({void*_tmp3F5=0U;({struct _dyneither_ptr _tmp785=({const char*_tmp3F6="anal_test: bad namedlocation";_tag_dyneither(_tmp3F6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp785,_tag_dyneither(_tmp3F5,sizeof(void*),0U));});});case 0U: _LL38: _LL39:
 return({struct _tuple19 _tmp61F;({union Cyc_CfFlowInfo_FlowInfo _tmp786=Cyc_CfFlowInfo_BottomFL();_tmp61F.f1=_tmp786;}),_tmp61F.f2=_tmp402;_tmp61F;});case 1U: _LL3A: _LL3B:
 goto _LL3D;case 5U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F:
 return({struct _tuple19 _tmp620;_tmp620.f1=_tmp402,({union Cyc_CfFlowInfo_FlowInfo _tmp787=Cyc_CfFlowInfo_BottomFL();_tmp620.f2=_tmp787;});_tmp620;});case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3F4)->f1 == Cyc_CfFlowInfo_NoneIL){_LL40: _LL41:
 goto _LL43;}else{_LL46: _tmp3FD=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp3F4)->f1;_LL47:
# 2149
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp402,e,_tmp3FD,_tmp3FE);}case 3U: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp3F4)->f1 == Cyc_CfFlowInfo_NoneIL){_LL42: _LL43:
# 2145
 goto _LL45;}else{_LL48: _LL49:
# 2150
 return({struct _tuple19 _tmp621;_tmp621.f1=_tmp402,_tmp621.f2=_tmp402;_tmp621;});}case 7U: _LL44: _LL45:
# 2147
({void*_tmp3F7=0U;({unsigned int _tmp789=e->loc;struct _dyneither_ptr _tmp788=({const char*_tmp3F8="expression may not be initialized";_tag_dyneither(_tmp3F8,sizeof(char),34U);});Cyc_CfFlowInfo_aerr(_tmp789,_tmp788,_tag_dyneither(_tmp3F7,sizeof(void*),0U));});});
return({struct _tuple19 _tmp622;({union Cyc_CfFlowInfo_FlowInfo _tmp78B=Cyc_CfFlowInfo_BottomFL();_tmp622.f1=_tmp78B;}),({union Cyc_CfFlowInfo_FlowInfo _tmp78A=Cyc_CfFlowInfo_BottomFL();_tmp622.f2=_tmp78A;});_tmp622;});default: _LL4A: _LL4B:
# 2151
({void*_tmp3F9=0U;({struct _dyneither_ptr _tmp78C=({const char*_tmp3FA="anal_test";_tag_dyneither(_tmp3FA,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp78C,_tag_dyneither(_tmp3F9,sizeof(void*),0U));});});}_LL35:;};}}_LL2D:;};}}_LL0:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2157
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2159
struct _tuple27*_tmp40F=ckenv;unsigned int _tmp41B;struct Cyc_NewControlFlow_AnalEnv*_tmp41A;struct Cyc_Dict_Dict _tmp419;_LL1: _tmp41B=_tmp40F->f1;_tmp41A=_tmp40F->f2;_tmp419=_tmp40F->f3;_LL2:;{
void*_tmp410=root;struct Cyc_Absyn_Vardecl*_tmp418;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp410)->tag == 0U){_LL4: _tmp418=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp410)->f1;_LL5:
# 2162
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp418->type)){
struct _tuple15 _tmp411=Cyc_CfFlowInfo_unname_rval(rval);struct _tuple15 _tmp412=_tmp411;void*_tmp417;_LL9: _tmp417=_tmp412.f1;_LLA:;{
void*_tmp413=_tmp417;switch(*((int*)_tmp413)){case 7U: _LLC: _LLD:
 goto _LLF;case 0U: _LLE: _LLF:
 goto _LL11;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp413)->f1 == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 goto _LLB;}else{goto _LL12;}default: _LL12: _LL13:
# 2169
({struct Cyc_String_pa_PrintArg_struct _tmp416=({struct Cyc_String_pa_PrintArg_struct _tmp623;_tmp623.tag=0U,({struct _dyneither_ptr _tmp78D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp418->name));_tmp623.f1=_tmp78D;});_tmp623;});void*_tmp414[1U];_tmp414[0]=& _tmp416;({unsigned int _tmp78F=_tmp41B;struct _dyneither_ptr _tmp78E=({const char*_tmp415="unique pointers reachable from %s may become unreachable";_tag_dyneither(_tmp415,sizeof(char),57U);});Cyc_Warn_warn(_tmp78F,_tmp78E,_tag_dyneither(_tmp414,sizeof(void*),1U));});});
goto _LLB;}_LLB:;};}
# 2173
goto _LL3;}else{_LL6: _LL7:
 goto _LL3;}_LL3:;};}
# 2178
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp41C=flow;struct Cyc_Dict_Dict _tmp421;if((_tmp41C.BottomFL).tag == 1){_LL1: _LL2:
 return;}else{_LL3: _tmp421=((_tmp41C.ReachableFL).val).f1;_LL4:
# 2182
{struct Cyc_List_List*_tmp41D=env->param_roots;for(0;_tmp41D != 0;_tmp41D=_tmp41D->tl){
if(({struct Cyc_CfFlowInfo_FlowEnv*_tmp791=env->fenv;struct Cyc_Dict_Dict _tmp790=_tmp421;Cyc_CfFlowInfo_initlevel(_tmp791,_tmp790,Cyc_CfFlowInfo_lookup_place(_tmp421,(struct Cyc_CfFlowInfo_Place*)_tmp41D->hd));})!= Cyc_CfFlowInfo_AllIL)
({void*_tmp41E=0U;({unsigned int _tmp793=loc;struct _dyneither_ptr _tmp792=({const char*_tmp41F="function may not initialize all the parameters with attribute 'initializes'";_tag_dyneither(_tmp41F,sizeof(char),76U);});Cyc_CfFlowInfo_aerr(_tmp793,_tmp792,_tag_dyneither(_tmp41E,sizeof(void*),0U));});});}}
# 2186
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp420=({struct _tuple27 _tmp624;_tmp624.f1=loc,_tmp624.f2=env,_tmp624.f3=_tmp421;_tmp624;});
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp420,_tmp421);}
# 2190
return;}_LL0:;}
# 2199
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct Cyc_List_List*vds){
# 2201
struct Cyc_List_List*_tmp422=0;
struct Cyc_List_List*_tmp423=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp424=(struct _tuple24*)x->hd;struct _tuple24*_tmp425=_tmp424;struct Cyc_Absyn_Vardecl**_tmp42D;struct Cyc_Absyn_Exp*_tmp42C;_LL1: _tmp42D=_tmp425->f1;_tmp42C=_tmp425->f2;_LL2:;
if((_tmp42D != 0  && _tmp42C != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp42C->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp426=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->tag=0U,_tmp42B->f1=*_tmp42D;_tmp42B;});
struct Cyc_CfFlowInfo_Place*_tmp427=({struct Cyc_CfFlowInfo_Place*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->root=(void*)_tmp426,_tmp42A->path=0;_tmp42A;});
({struct Cyc_List_List*_tmp794=({struct Cyc_List_List*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->hd=_tmp427,_tmp428->tl=_tmp422;_tmp428;});_tmp422=_tmp794;});
({struct Cyc_List_List*_tmp795=({struct Cyc_List_List*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->hd=_tmp42C,_tmp429->tl=_tmp423;_tmp429;});_tmp423=_tmp795;});}}}
# 2213
return({struct _tuple1 _tmp625;_tmp625.f1=_tmp422,_tmp625.f2=_tmp423;_tmp625;});}struct _tuple28{int f1;void*f2;};
# 2219
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2226
union Cyc_CfFlowInfo_FlowInfo _tmp42E=flow;struct Cyc_Dict_Dict _tmp442;if((_tmp42E.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp42F=0U;({struct _dyneither_ptr _tmp796=({const char*_tmp430="noconsume_place_ok: flow became Bottom!";_tag_dyneither(_tmp430,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp796,_tag_dyneither(_tmp42F,sizeof(void*),0U));});});}else{_LL3: _tmp442=((_tmp42E.ReachableFL).val).f1;_LL4: {
# 2234
struct Cyc_Absyn_Exp*_tmp431=Cyc_Absyn_uint_exp(1U,0U);
int _tmp432=0;
int _tmp433=1;
void*_tmp434=Cyc_CfFlowInfo_lookup_place(_tmp442,place);
void*_tmp435=_tmp434;
# 2246
int varok=0;
{void*_tmp436=_tmp434;struct Cyc_Absyn_Vardecl*_tmp441;void*_tmp440;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp436)->tag == 8U){_LL6: _tmp441=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp436)->f1;_tmp440=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp436)->f2;_LL7:
# 2249
 if(vd == _tmp441){
_tmp435=_tmp440;
# 2252
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2254
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp431,_tmp433,_tmp435,& _tmp432)){
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp439=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({
# 2258
struct _dyneither_ptr _tmp797=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp626.f1=_tmp797;});_tmp626;});void*_tmp437[1U];_tmp437[0]=& _tmp439;({unsigned int _tmp799=loc;struct _dyneither_ptr _tmp798=({const char*_tmp438="function consumes parameter %s with attribute 'noconsume'";_tag_dyneither(_tmp438,sizeof(char),58U);});Cyc_CfFlowInfo_aerr(_tmp799,_tmp798,_tag_dyneither(_tmp437,sizeof(void*),1U));});});}else{
# 2261
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp442,_tmp435)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp43C=({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0U,({
# 2264
struct _dyneither_ptr _tmp79A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp627.f1=_tmp79A;});_tmp627;});void*_tmp43A[1U];_tmp43A[0]=& _tmp43C;({unsigned int _tmp79C=loc;struct _dyneither_ptr _tmp79B=({const char*_tmp43B="function consumes value pointed to by parameter %s, which has attribute 'noconsume'";_tag_dyneither(_tmp43B,sizeof(char),84U);});Cyc_CfFlowInfo_aerr(_tmp79C,_tmp79B,_tag_dyneither(_tmp43A,sizeof(void*),1U));});});else{
# 2266
varok=1;}}}else{
# 2269
varok=1;}}else{
# 2272
goto _LL9;}
goto _LL5;}else{_LL8: _LL9:
# 2276
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
varok=1;else{
if(!do_unconsume)
({struct Cyc_String_pa_PrintArg_struct _tmp43F=({struct Cyc_String_pa_PrintArg_struct _tmp628;_tmp628.tag=0U,({
# 2281
struct _dyneither_ptr _tmp79D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp628.f1=_tmp79D;});_tmp628;});void*_tmp43D[1U];_tmp43D[0]=& _tmp43F;({unsigned int _tmp79F=loc;struct _dyneither_ptr _tmp79E=({const char*_tmp43E="function parameter %s with attribute 'noconsume' no longer set to its original value";_tag_dyneither(_tmp43E,sizeof(char),85U);});Cyc_CfFlowInfo_aerr(_tmp79F,_tmp79E,_tag_dyneither(_tmp43D,sizeof(void*),1U));});});}
goto _LL5;}_LL5:;}
# 2288
return({struct _tuple28 _tmp629;_tmp629.f1=varok,_tmp629.f2=_tmp435;_tmp629;});}}_LL0:;}
# 2294
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_CfFlowInfo_Place*_tmp443=p;void*_tmp44B;struct Cyc_List_List*_tmp44A;_LL1: _tmp44B=_tmp443->root;_tmp44A=_tmp443->path;_LL2:;
if(_tmp44A != 0)
({void*_tmp444=0U;({struct _dyneither_ptr _tmp7A0=({const char*_tmp445="unconsume_params: param to unconsume is non-variable";_tag_dyneither(_tmp445,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7A0,_tag_dyneither(_tmp444,sizeof(void*),0U));});});{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp446=_tmp44B;struct Cyc_Absyn_Vardecl*_tmp449;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp446)->tag == 0U){_LL4: _tmp449=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp446)->f1;_LL5:
 return _tmp449;}else{_LL6: _LL7:
({void*_tmp447=0U;({struct _dyneither_ptr _tmp7A1=({const char*_tmp448="unconsume_params: root is not a varroot";_tag_dyneither(_tmp448,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7A1,_tag_dyneither(_tmp447,sizeof(void*),0U));});});}_LL3:;};}
# 2313 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2321
{union Cyc_CfFlowInfo_FlowInfo _tmp44C=flow;struct Cyc_Dict_Dict _tmp463;if((_tmp44C.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _tmp463=((_tmp44C.ReachableFL).val).f1;_LL4: {
# 2328
struct _tuple18 _tmp44D=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);struct _tuple18 _tmp44E=_tmp44D;union Cyc_CfFlowInfo_FlowInfo _tmp462;union Cyc_CfFlowInfo_AbsLVal _tmp461;_LL6: _tmp462=_tmp44E.f1;_tmp461=_tmp44E.f2;_LL7:;
# 2331
{union Cyc_CfFlowInfo_AbsLVal _tmp44F=_tmp461;struct Cyc_CfFlowInfo_Place*_tmp460;if((_tmp44F.PlaceL).tag == 1){_LL9: _tmp460=(_tmp44F.PlaceL).val;_LLA: {
# 2335
void*_tmp450=Cyc_CfFlowInfo_lookup_place(_tmp463,_tmp460);
{void*_tmp451=_tmp450;struct Cyc_Absyn_Vardecl*_tmp45F;void*_tmp45E;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp451)->tag == 8U){_LLE: _tmp45F=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp451)->f1;_tmp45E=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp451)->f2;_LLF: {
# 2341
void*new_rval;
if(_tmp45F == vd){
# 2345
if(varok){
# 2347
({void*_tmp7A2=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp45E);_tmp450=_tmp7A2;});
# 2352
if(ropt != 0){
# 2358
struct _tuple16 _tmp452=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp7A4=env->fenv;struct _tuple16 _tmp7A3=({struct _tuple16 _tmp62A;_tmp62A.f1=_tmp462,_tmp62A.f2=_tmp450;_tmp62A;});Cyc_CfFlowInfo_join_flow_and_rval(_tmp7A4,_tmp7A3,({struct _tuple16 _tmp62B;_tmp62B.f1=_tmp462,_tmp62B.f2=ropt;_tmp62B;}));});
# 2358
struct _tuple16 _tmp453=_tmp452;union Cyc_CfFlowInfo_FlowInfo _tmp455;void*_tmp454;_LL13: _tmp455=_tmp453.f1;_tmp454=_tmp453.f2;_LL14:;
# 2362
_tmp462=_tmp455;new_rval=_tmp454;}else{
# 2367
new_rval=_tmp450;}}else{
# 2370
new_rval=_tmp45E;}
# 2372
{union Cyc_CfFlowInfo_FlowInfo _tmp456=_tmp462;struct Cyc_Dict_Dict _tmp45A;struct Cyc_List_List*_tmp459;if((_tmp456.ReachableFL).tag == 2){_LL16: _tmp45A=((_tmp456.ReachableFL).val).f1;_tmp459=((_tmp456.ReachableFL).val).f2;_LL17:
# 2374
({union Cyc_CfFlowInfo_FlowInfo _tmp7A6=({struct Cyc_Dict_Dict _tmp7A5=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp45A,_tmp460,new_rval);Cyc_CfFlowInfo_ReachableFL(_tmp7A5,_tmp459);});flow=_tmp7A6;});
# 2378
goto _LL15;}else{_LL18: _LL19:
# 2380
({void*_tmp457=0U;({struct _dyneither_ptr _tmp7A7=({const char*_tmp458="unconsume_params: joined flow became bot!";_tag_dyneither(_tmp458,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7A7,_tag_dyneither(_tmp457,sizeof(void*),0U));});});}_LL15:;}
# 2382
goto _LLD;}else{
# 2384
goto _LL11;}
goto _LLD;}}else{_LL10: _LL11:
# 2391
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type))
({struct Cyc_String_pa_PrintArg_struct _tmp45D=({struct Cyc_String_pa_PrintArg_struct _tmp62C;_tmp62C.tag=0U,({
struct _dyneither_ptr _tmp7A8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(unconsume_exp));_tmp62C.f1=_tmp7A8;});_tmp62C;});void*_tmp45B[1U];_tmp45B[0]=& _tmp45D;({unsigned int _tmp7AA=loc;struct _dyneither_ptr _tmp7A9=({const char*_tmp45C="aliased expression %s was overwritten";_tag_dyneither(_tmp45C,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7AA,_tmp7A9,_tag_dyneither(_tmp45B,sizeof(void*),1U));});});
# 2408 "new_control_flow.cyc"
goto _LLD;}_LLD:;}
# 2410
goto _LL8;}}else{_LLB: _LLC:
# 2416
 goto _LL8;}_LL8:;}
# 2418
goto _LL0;}}_LL0:;}
# 2420
return flow;}
# 2433 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2439
{union Cyc_CfFlowInfo_FlowInfo _tmp464=flow;if((_tmp464.BottomFL).tag == 1){_LL1: _LL2:
 return flow;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2443
int _tmp465=unconsume_exps != 0;
struct Cyc_List_List*_tmp466=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(consumed_vals);
struct Cyc_List_List*_tmp467=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(unconsume_exps);
{struct Cyc_List_List*_tmp468=_tmp466;for(0;_tmp468 != 0;(
_tmp468=_tmp468->tl,
_tmp467 != 0?_tmp467=_tmp467->tl: 0)){
# 2452
struct Cyc_Absyn_Vardecl*_tmp469=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp468->hd);
struct _tuple28 _tmp46A=
is_region_open?({struct _tuple28 _tmp62D;_tmp62D.f1=1,_tmp62D.f2=0;_tmp62D;}):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp468->hd,_tmp465,_tmp469,flow,loc);
# 2453
struct _tuple28 _tmp46B=_tmp46A;int _tmp46D;void*_tmp46C;_LL6: _tmp46D=_tmp46B.f1;_tmp46C=_tmp46B.f2;_LL7:;
# 2459
if(_tmp465)
({union Cyc_CfFlowInfo_FlowInfo _tmp7AB=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp467))->hd,_tmp469,_tmp46D,_tmp46C,flow,loc);flow=_tmp7AB;});}}
# 2462
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2466
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2468
struct Cyc_CfFlowInfo_FlowEnv*_tmp46E=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp46F=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_tmp470=_tmp46F;struct Cyc_Core_Opt*_tmp484;struct Cyc_Absyn_Exp*_tmp483;struct Cyc_Absyn_Stmt*_tmp482;unsigned int _tmp481;_LL1: _tmp484=_tmp470->pat_vars;_tmp483=_tmp470->where_clause;_tmp482=_tmp470->body;_tmp481=_tmp470->loc;_LL2:;{
struct _tuple1 _tmp471=Cyc_NewControlFlow_get_unconsume_pat_vars((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp484))->v);struct _tuple1 _tmp472=_tmp471;struct Cyc_List_List*_tmp480;struct Cyc_List_List*_tmp47F;_LL4: _tmp480=_tmp472.f1;_tmp47F=_tmp472.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp484->v,_tmp480 != 0,_tmp481,exp_loc);
# 2476
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp473=env->unique_pat_vars;
# 2479
if(Cyc_Tcpat_has_vars(_tmp484))
({struct Cyc_List_List*_tmp7AD=({struct Cyc_List_List*_tmp475=_cycalloc(sizeof(*_tmp475));
({struct _tuple29*_tmp7AC=({struct _tuple29*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->f1=0,_tmp474->f2=_tmp482,_tmp474->f3=_tmp480,_tmp474->f4=_tmp47F;_tmp474;});_tmp475->hd=_tmp7AC;}),_tmp475->tl=_tmp473;_tmp475;});
# 2480
env->unique_pat_vars=_tmp7AD;});
# 2483
if(_tmp483 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp483;
struct _tuple19 _tmp476=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);struct _tuple19 _tmp477=_tmp476;union Cyc_CfFlowInfo_FlowInfo _tmp479;union Cyc_CfFlowInfo_FlowInfo _tmp478;_LL7: _tmp479=_tmp477.f1;_tmp478=_tmp477.f2;_LL8:;
inflow=_tmp478;
({union Cyc_CfFlowInfo_FlowInfo _tmp7AE=Cyc_NewControlFlow_anal_stmt(env,_tmp479,_tmp482,0);clause_outflow=_tmp7AE;});}else{
# 2489
({union Cyc_CfFlowInfo_FlowInfo _tmp7AF=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp482,0);clause_outflow=_tmp7AF;});}
# 2491
env->unique_pat_vars=_tmp473;{
union Cyc_CfFlowInfo_FlowInfo _tmp47A=clause_outflow;if((_tmp47A.BottomFL).tag == 1){_LLA: _LLB:
 goto _LL9;}else{_LLC: _LLD:
# 2496
({union Cyc_CfFlowInfo_FlowInfo _tmp7B0=Cyc_NewControlFlow_unconsume_params(env,_tmp480,_tmp47F,0,clause_outflow,_tmp481);clause_outflow=_tmp7B0;});
# 2498
if(scs->tl == 0)
return clause_outflow;else{
# 2503
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)
({void*_tmp47B=0U;({unsigned int _tmp7B2=_tmp482->loc;struct _dyneither_ptr _tmp7B1=({const char*_tmp47C="switch clause may implicitly fallthru";_tag_dyneither(_tmp47C,sizeof(char),38U);});Cyc_CfFlowInfo_aerr(_tmp7B2,_tmp7B1,_tag_dyneither(_tmp47B,sizeof(void*),0U));});});else{
# 2506
({void*_tmp47D=0U;({unsigned int _tmp7B4=_tmp482->loc;struct _dyneither_ptr _tmp7B3=({const char*_tmp47E="switch clause may implicitly fallthru";_tag_dyneither(_tmp47E,sizeof(char),38U);});Cyc_Warn_warn(_tmp7B4,_tmp7B3,_tag_dyneither(_tmp47D,sizeof(void*),0U));});});}
# 2508
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2510
goto _LL9;}_LL9:;};};};}
# 2513
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2518
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2520
struct _tuple30*_tmp485=vdenv;struct Cyc_NewControlFlow_AnalEnv*_tmp490;struct Cyc_Dict_Dict _tmp48F;unsigned int _tmp48E;_LL1: _tmp490=_tmp485->f1;_tmp48F=_tmp485->f2;_tmp48E=_tmp485->f3;_LL2:;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(vd->type)){
# 2523
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp62F;_tmp62F.tag=0U,_tmp62F.f1=vd;_tmp62F;});
struct _tuple15 _tmp486=Cyc_CfFlowInfo_unname_rval(({struct Cyc_Dict_Dict _tmp7B5=_tmp48F;((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp7B5,Cyc_CfFlowInfo_root_cmp,(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));*_tmp48D=vdroot;_tmp48D;}));}));struct _tuple15 _tmp487=_tmp486;void*_tmp48C;_LL4: _tmp48C=_tmp487.f1;_LL5:;{
void*_tmp488=_tmp48C;switch(*((int*)_tmp488)){case 7U: _LL7: _LL8:
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 2U: if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp488)->f1 == Cyc_CfFlowInfo_NoneIL){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
# 2530
({struct Cyc_String_pa_PrintArg_struct _tmp48B=({struct Cyc_String_pa_PrintArg_struct _tmp62E;_tmp62E.tag=0U,({struct _dyneither_ptr _tmp7B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp62E.f1=_tmp7B6;});_tmp62E;});void*_tmp489[1U];_tmp489[0]=& _tmp48B;({unsigned int _tmp7B8=_tmp48E;struct _dyneither_ptr _tmp7B7=({const char*_tmp48A="unique pointers may still exist after variable %s goes out of scope";_tag_dyneither(_tmp48A,sizeof(char),68U);});Cyc_Warn_warn(_tmp7B8,_tmp7B7,_tag_dyneither(_tmp489,sizeof(void*),1U));});});
# 2532
goto _LL6;}_LL6:;};}}
# 2537
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
{union Cyc_CfFlowInfo_FlowInfo _tmp491=inflow;struct Cyc_Dict_Dict _tmp49D;if((_tmp491.ReachableFL).tag == 2){_LL1: _tmp49D=((_tmp491.ReachableFL).val).f1;_LL2: {
# 2540
struct _tuple30 _tmp492=({struct _tuple30 _tmp630;_tmp630.f1=env,_tmp630.f2=_tmp49D,_tmp630.f3=decl->loc;_tmp630;});
struct Cyc_CfFlowInfo_FlowEnv*_tmp493=env->fenv;
{void*_tmp494=decl->r;void*_tmp495=_tmp494;struct Cyc_List_List*_tmp49C;struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_Vardecl*_tmp49A;switch(*((int*)_tmp495)){case 0U: _LL6: _tmp49A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp495)->f1;_LL7:
# 2544
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp492,_tmp49A);
goto _LL5;case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp495)->f2 != 0){_LL8: _tmp49B=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp495)->f2)->v;_LL9: {
# 2547
struct _tuple1 _tmp496=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp49B);struct _tuple1 _tmp497=_tmp496;struct Cyc_List_List*_tmp499;_LLF: _tmp499=_tmp497.f1;_LL10:;{
struct Cyc_List_List*_tmp498=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp499);
_tmp49C=_tmp498;goto _LLB;};}}else{goto _LLC;}case 3U: _LLA: _tmp49C=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp495)->f1;_LLB:
# 2551
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp492,_tmp49C);
goto _LL5;default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 2555
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2558
return;}
# 2564
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*src,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2568
int num_pop=((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->pat_pop_table,src);
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;num_pop > 0;(x=x->tl,-- num_pop)){
struct _tuple29*_tmp49E=(struct _tuple29*)((struct Cyc_List_List*)_check_null(x))->hd;struct _tuple29*_tmp49F=_tmp49E;int _tmp4A3;struct Cyc_Absyn_Stmt*_tmp4A2;struct Cyc_List_List*_tmp4A1;struct Cyc_List_List*_tmp4A0;_LL1: _tmp4A3=_tmp49F->f1;_tmp4A2=_tmp49F->f2;_tmp4A1=_tmp49F->f3;_tmp4A0=_tmp49F->f4;_LL2:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7B9=Cyc_NewControlFlow_unconsume_params(env,_tmp4A1,_tmp4A0,_tmp4A3,inflow,dest->loc);inflow=_tmp7B9;});}}
# 2573
return inflow;}
# 2579
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2581
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp4A4=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct _tuple20 _tmp4A5=_tmp4A4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp53F;union Cyc_CfFlowInfo_FlowInfo*_tmp53E;_LL1: _tmp53F=_tmp4A5.f1;_tmp53E=_tmp4A5.f2;_LL2:;
inflow=*_tmp53E;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp4A6=env->fenv;
# 2588
void*_tmp4A7=s->r;void*_tmp4A8=_tmp4A7;struct Cyc_Absyn_Stmt*_tmp53D;struct Cyc_Absyn_Decl*_tmp53C;struct Cyc_Absyn_Stmt*_tmp53B;struct Cyc_Absyn_Vardecl*_tmp53A;struct Cyc_Absyn_Exp*_tmp539;unsigned int _tmp538;struct Cyc_Absyn_Stmt*_tmp537;struct Cyc_List_List*_tmp536;struct Cyc_Absyn_Exp*_tmp535;unsigned int _tmp534;struct Cyc_Absyn_Stmt*_tmp533;struct Cyc_Absyn_Stmt*_tmp532;struct Cyc_List_List*_tmp531;void*_tmp530;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_List_List*_tmp52E;void*_tmp52D;struct Cyc_List_List*_tmp52C;struct Cyc_Absyn_Switch_clause*_tmp52B;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Stmt*_tmp528;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Stmt*_tmp526;struct Cyc_Absyn_Stmt*_tmp525;struct Cyc_Absyn_Stmt*_tmp524;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Stmt*_tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Stmt*_tmp520;struct Cyc_Absyn_Stmt*_tmp51F;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Stmt*_tmp51D;struct Cyc_Absyn_Stmt*_tmp51C;struct Cyc_Absyn_Stmt*_tmp51B;struct Cyc_Absyn_Stmt*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp518;switch(*((int*)_tmp4A8)){case 0U: _LL4: _LL5:
 return inflow;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1 == 0){_LL6: _LL7:
# 2592
 if(env->noreturn)
({void*_tmp4A9=0U;({unsigned int _tmp7BB=s->loc;struct _dyneither_ptr _tmp7BA=({const char*_tmp4AA="`noreturn' function might return";_tag_dyneither(_tmp4AA,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7BB,_tmp7BA,_tag_dyneither(_tmp4A9,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}else{_LL8: _tmp518=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_LL9:
# 2598
 if(env->noreturn)
({void*_tmp4AB=0U;({unsigned int _tmp7BD=s->loc;struct _dyneither_ptr _tmp7BC=({const char*_tmp4AC="`noreturn' function might return";_tag_dyneither(_tmp4AC,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7BD,_tmp7BC,_tag_dyneither(_tmp4AB,sizeof(void*),0U));});});{
struct _tuple16 _tmp4AD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp518));struct _tuple16 _tmp4AE=_tmp4AD;union Cyc_CfFlowInfo_FlowInfo _tmp4B0;void*_tmp4AF;_LL2D: _tmp4B0=_tmp4AE.f1;_tmp4AF=_tmp4AE.f2;_LL2E:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7BE=Cyc_NewControlFlow_use_Rval(env,_tmp518->loc,_tmp4B0,_tmp4AF);_tmp4B0=_tmp7BE;});
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp4B0);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp4B0,s->loc);
return Cyc_CfFlowInfo_BottomFL();};}case 1U: _LLA: _tmp519=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_LLB: {
# 2607
struct _tuple17*_tmp4B1=rval_opt;void**_tmp4B7;int _tmp4B6;if(_tmp4B1 != 0){_LL30: _tmp4B7=(void**)& _tmp4B1->f1;_tmp4B6=_tmp4B1->f2;_LL31: {
# 2609
struct _tuple16 _tmp4B2=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B6,inflow,_tmp519);struct _tuple16 _tmp4B3=_tmp4B2;union Cyc_CfFlowInfo_FlowInfo _tmp4B5;void*_tmp4B4;_LL35: _tmp4B5=_tmp4B3.f1;_tmp4B4=_tmp4B3.f2;_LL36:;
*_tmp4B7=_tmp4B4;
return _tmp4B5;}}else{_LL32: _LL33:
# 2613
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp519)).f1;}_LL2F:;}case 2U: _LLC: _tmp51B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp51A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_LLD:
# 2617
 return({struct Cyc_NewControlFlow_AnalEnv*_tmp7C1=env;union Cyc_CfFlowInfo_FlowInfo _tmp7C0=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp51B,0);struct Cyc_Absyn_Stmt*_tmp7BF=_tmp51A;Cyc_NewControlFlow_anal_stmt(_tmp7C1,_tmp7C0,_tmp7BF,rval_opt);});case 4U: _LLE: _tmp51E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp51D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_tmp51C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f3;_LLF: {
# 2620
struct _tuple19 _tmp4B8=Cyc_NewControlFlow_anal_test(env,inflow,_tmp51E);struct _tuple19 _tmp4B9=_tmp4B8;union Cyc_CfFlowInfo_FlowInfo _tmp4BD;union Cyc_CfFlowInfo_FlowInfo _tmp4BC;_LL38: _tmp4BD=_tmp4B9.f1;_tmp4BC=_tmp4B9.f2;_LL39:;{
# 2627
union Cyc_CfFlowInfo_FlowInfo _tmp4BA=Cyc_NewControlFlow_anal_stmt(env,_tmp4BC,_tmp51C,0);
union Cyc_CfFlowInfo_FlowInfo _tmp4BB=Cyc_NewControlFlow_anal_stmt(env,_tmp4BD,_tmp51D,0);
return Cyc_CfFlowInfo_join_flow(_tmp4A6,_tmp4BB,_tmp4BA);};}case 5U: _LL10: _tmp521=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1).f1;_tmp520=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1).f2;_tmp51F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_LL11: {
# 2635
struct _tuple20 _tmp4BE=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp520);struct _tuple20 _tmp4BF=_tmp4BE;union Cyc_CfFlowInfo_FlowInfo*_tmp4C6;_LL3B: _tmp4C6=_tmp4BF.f2;_LL3C:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4C0=*_tmp4C6;
struct _tuple19 _tmp4C1=Cyc_NewControlFlow_anal_test(env,_tmp4C0,_tmp521);struct _tuple19 _tmp4C2=_tmp4C1;union Cyc_CfFlowInfo_FlowInfo _tmp4C5;union Cyc_CfFlowInfo_FlowInfo _tmp4C4;_LL3E: _tmp4C5=_tmp4C2.f1;_tmp4C4=_tmp4C2.f2;_LL3F:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4C3=Cyc_NewControlFlow_anal_stmt(env,_tmp4C5,_tmp51F,0);
Cyc_NewControlFlow_update_flow(env,_tmp520,_tmp4C3);
return _tmp4C4;};};}case 14U: _LL12: _tmp524=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp523=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2).f1;_tmp522=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2).f2;_LL13: {
# 2645
union Cyc_CfFlowInfo_FlowInfo _tmp4C7=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp524,0);
struct _tuple20 _tmp4C8=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4C7,_tmp522);struct _tuple20 _tmp4C9=_tmp4C8;union Cyc_CfFlowInfo_FlowInfo*_tmp4CF;_LL41: _tmp4CF=_tmp4C9.f2;_LL42:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4CA=*_tmp4CF;
struct _tuple19 _tmp4CB=Cyc_NewControlFlow_anal_test(env,_tmp4CA,_tmp523);struct _tuple19 _tmp4CC=_tmp4CB;union Cyc_CfFlowInfo_FlowInfo _tmp4CE;union Cyc_CfFlowInfo_FlowInfo _tmp4CD;_LL44: _tmp4CE=_tmp4CC.f1;_tmp4CD=_tmp4CC.f2;_LL45:;
Cyc_NewControlFlow_update_flow(env,_tmp524,_tmp4CE);
return _tmp4CD;};}case 9U: _LL14: _tmp52A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp529=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2).f1;_tmp528=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2).f2;_tmp527=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f3).f1;_tmp526=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f3).f2;_tmp525=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f4;_LL15: {
# 2654
union Cyc_CfFlowInfo_FlowInfo _tmp4D0=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp52A)).f1;
struct _tuple20 _tmp4D1=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4D0,_tmp528);struct _tuple20 _tmp4D2=_tmp4D1;union Cyc_CfFlowInfo_FlowInfo*_tmp4DE;_LL47: _tmp4DE=_tmp4D2.f2;_LL48:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D3=*_tmp4DE;
struct _tuple19 _tmp4D4=Cyc_NewControlFlow_anal_test(env,_tmp4D3,_tmp529);struct _tuple19 _tmp4D5=_tmp4D4;union Cyc_CfFlowInfo_FlowInfo _tmp4DD;union Cyc_CfFlowInfo_FlowInfo _tmp4DC;_LL4A: _tmp4DD=_tmp4D5.f1;_tmp4DC=_tmp4D5.f2;_LL4B:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D6=Cyc_NewControlFlow_anal_stmt(env,_tmp4DD,_tmp525,0);
struct _tuple20 _tmp4D7=Cyc_NewControlFlow_pre_stmt_check(env,_tmp4D6,_tmp526);struct _tuple20 _tmp4D8=_tmp4D7;union Cyc_CfFlowInfo_FlowInfo*_tmp4DB;_LL4D: _tmp4DB=_tmp4D8.f2;_LL4E:;{
union Cyc_CfFlowInfo_FlowInfo _tmp4D9=*_tmp4DB;
union Cyc_CfFlowInfo_FlowInfo _tmp4DA=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp4D9,_tmp527)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp528,_tmp4DA);
return _tmp4DC;};};};}case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2 != 0){_LL16: _tmp52C=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp52B=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_LL17: {
# 2666
struct _tuple23 _tmp4DF=Cyc_NewControlFlow_anal_Rexps(env,inflow,_tmp52C,1,1);struct _tuple23 _tmp4E0=_tmp4DF;union Cyc_CfFlowInfo_FlowInfo _tmp4E7;struct Cyc_List_List*_tmp4E6;_LL50: _tmp4E7=_tmp4E0.f1;_tmp4E6=_tmp4E0.f2;_LL51:;
# 2668
({union Cyc_CfFlowInfo_FlowInfo _tmp7C2=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,_tmp52B->body);inflow=_tmp7C2;});{
# 2670
struct Cyc_List_List*_tmp4E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp52B->pat_vars))->v)).f1);
({union Cyc_CfFlowInfo_FlowInfo _tmp7C3=Cyc_NewControlFlow_add_vars(_tmp4A6,_tmp4E7,_tmp4E1,_tmp4A6->unknown_all,s->loc,0);_tmp4E7=_tmp7C3;});
# 2673
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp52B->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp4E2=(struct _tuple24*)x->hd;struct _tuple24*_tmp4E3=_tmp4E2;struct Cyc_Absyn_Vardecl**_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E4;_LL53: _tmp4E5=_tmp4E3->f1;_tmp4E4=_tmp4E3->f2;_LL54:;
if(_tmp4E5 != 0){
({union Cyc_CfFlowInfo_FlowInfo _tmp7CA=({struct Cyc_CfFlowInfo_FlowEnv*_tmp7C9=_tmp4A6;struct Cyc_NewControlFlow_AnalEnv*_tmp7C8=env;union Cyc_CfFlowInfo_FlowInfo _tmp7C7=_tmp4E7;struct Cyc_Absyn_Vardecl*_tmp7C6=*_tmp4E5;struct Cyc_Absyn_Exp*_tmp7C5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp52C))->hd;void*_tmp7C4=(void*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd;Cyc_NewControlFlow_do_initialize_var(_tmp7C9,_tmp7C8,_tmp7C7,_tmp7C6,_tmp7C5,_tmp7C4,s->loc);});_tmp4E7=_tmp7CA;});
_tmp4E6=_tmp4E6->tl;
_tmp52C=_tmp52C->tl;}}}
# 2681
Cyc_NewControlFlow_update_flow(env,_tmp52B->body,_tmp4E7);
return Cyc_CfFlowInfo_BottomFL();};}}else{_LL2A: _LL2B:
# 2845
({void*_tmp516=0U;({struct _dyneither_ptr _tmp7CB=({const char*_tmp517="anal_stmt: bad stmt syntax or unimplemented stmt form";_tag_dyneither(_tmp517,sizeof(char),54U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7CB,_tag_dyneither(_tmp516,sizeof(void*),0U));});});}case 6U: _LL18: _LL19:
# 2687
 if(((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s)== 0){
{union Cyc_CfFlowInfo_FlowInfo _tmp4E8=inflow;if((_tmp4E8.ReachableFL).tag == 2){_LL56: _LL57:
# 2690
 if(!Cyc_Tcutil_is_void_type(env->return_type)){
if(Cyc_Tcutil_is_any_float_type(env->return_type) || 
Cyc_Tcutil_is_any_int_type(env->return_type))
({void*_tmp4E9=0U;({unsigned int _tmp7CD=s->loc;struct _dyneither_ptr _tmp7CC=({const char*_tmp4EA="break may cause function not to return a value";_tag_dyneither(_tmp4EA,sizeof(char),47U);});Cyc_Warn_warn(_tmp7CD,_tmp7CC,_tag_dyneither(_tmp4E9,sizeof(void*),0U));});});else{
# 2695
({void*_tmp4EB=0U;({unsigned int _tmp7CF=s->loc;struct _dyneither_ptr _tmp7CE=({const char*_tmp4EC="break may cause function not to return a value";_tag_dyneither(_tmp4EC,sizeof(char),47U);});Cyc_CfFlowInfo_aerr(_tmp7CF,_tmp7CE,_tag_dyneither(_tmp4EB,sizeof(void*),0U));});});}}
# 2697
goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 2700
if(env->noreturn)
({void*_tmp4ED=0U;({unsigned int _tmp7D1=s->loc;struct _dyneither_ptr _tmp7D0=({const char*_tmp4EE="`noreturn' function might return";_tag_dyneither(_tmp4EE,sizeof(char),33U);});Cyc_CfFlowInfo_aerr(_tmp7D1,_tmp7D0,_tag_dyneither(_tmp4ED,sizeof(void*),0U));});});
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();}
# 2706
goto _LL1B;case 7U: _LL1A: _LL1B:
 goto _LL1D;case 8U: _LL1C: _LL1D: {
# 2710
struct Cyc_Absyn_Stmt*_tmp4EF=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(env->succ_table,s);
if(_tmp4EF == 0)
({void*_tmp4F0=0U;({unsigned int _tmp7D3=s->loc;struct _dyneither_ptr _tmp7D2=({const char*_tmp4F1="jump has no target (should be impossible)";_tag_dyneither(_tmp4F1,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp7D3,_tmp7D2,_tag_dyneither(_tmp4F0,sizeof(void*),0U));});});
({union Cyc_CfFlowInfo_FlowInfo _tmp7D4=Cyc_NewControlFlow_unconsume_pat_vars(env,s,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp4EF));inflow=_tmp7D4;});
# 2715
Cyc_NewControlFlow_update_flow(env,_tmp4EF,inflow);
return Cyc_CfFlowInfo_BottomFL();}case 10U: _LL1E: _tmp52F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp52E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_tmp52D=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f3;_LL1F:
# 2721
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp52E,_tmp52F->loc);case 15U: _LL20: _tmp532=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp531=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_tmp530=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f3;_LL21: {
# 2726
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp532,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2734
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2737
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp531,0U);
{union Cyc_CfFlowInfo_FlowInfo _tmp4F2=scs_outflow;if((_tmp4F2.BottomFL).tag == 1){_LL5B: _LL5C:
 goto _LL5A;}else{_LL5D: _LL5E:
({void*_tmp4F3=0U;({unsigned int _tmp7D6=s->loc;struct _dyneither_ptr _tmp7D5=({const char*_tmp4F4="last catch clause may implicitly fallthru";_tag_dyneither(_tmp4F4,sizeof(char),42U);});Cyc_CfFlowInfo_aerr(_tmp7D6,_tmp7D5,_tag_dyneither(_tmp4F3,sizeof(void*),0U));});});}_LL5A:;}
# 2743
outflow=s1_outflow;
# 2745
return outflow;};};}case 12U: switch(*((int*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->r)){case 2U: if(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)((struct Cyc_Absyn_Decl*)((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->r)->f2 != 0){_LL22: _tmp536=(struct Cyc_List_List*)(((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->r)->f2)->v;_tmp535=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->r)->f3;_tmp534=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->loc;_tmp533=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_LL23: {
# 2755
struct _tuple1 _tmp4F5=Cyc_NewControlFlow_get_unconsume_pat_vars(_tmp536);struct _tuple1 _tmp4F6=_tmp4F5;struct Cyc_List_List*_tmp4FB;struct Cyc_List_List*_tmp4FA;_LL60: _tmp4FB=_tmp4F6.f1;_tmp4FA=_tmp4F6.f2;_LL61:;
({union Cyc_CfFlowInfo_FlowInfo _tmp7D7=Cyc_NewControlFlow_initialize_pat_vars(_tmp4A6,env,inflow,_tmp536,_tmp4FB != 0,_tmp534,_tmp535->loc);inflow=_tmp7D7;});{
struct Cyc_List_List*_tmp4F7=env->unique_pat_vars;
# 2759
({struct Cyc_List_List*_tmp7D9=({struct Cyc_List_List*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));
({struct _tuple29*_tmp7D8=({struct _tuple29*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->f1=0,_tmp4F8->f2=s,_tmp4F8->f3=_tmp4FB,_tmp4F8->f4=_tmp4FA;_tmp4F8;});_tmp4F9->hd=_tmp7D8;}),_tmp4F9->tl=_tmp4F7;_tmp4F9;});
# 2759
env->unique_pat_vars=_tmp7D9;});
# 2764
({union Cyc_CfFlowInfo_FlowInfo _tmp7DA=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp533,rval_opt);inflow=_tmp7DA;});
env->unique_pat_vars=_tmp4F7;
# 2768
({union Cyc_CfFlowInfo_FlowInfo _tmp7DB=Cyc_NewControlFlow_unconsume_params(env,_tmp4FB,_tmp4FA,0,inflow,_tmp534);inflow=_tmp7DB;});
# 2772
return inflow;};}}else{goto _LL26;}case 4U: _LL24: _tmp53A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->r)->f2;_tmp539=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->r)->f3;_tmp538=(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1)->loc;_tmp537=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;if(_tmp539 != 0){_LL25: {
# 2784
struct Cyc_List_List l=({struct Cyc_List_List _tmp632;_tmp632.hd=_tmp539,_tmp632.tl=0;_tmp632;});
union Cyc_CfFlowInfo_FlowInfo _tmp4FC=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp4FD=Cyc_NewControlFlow_anal_Lexp(env,_tmp4FC,0,0,_tmp539);struct _tuple18 _tmp4FE=_tmp4FD;union Cyc_CfFlowInfo_AbsLVal _tmp515;_LL63: _tmp515=_tmp4FE.f2;_LL64:;{
struct _tuple16 _tmp4FF=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp4FC,_tmp539);struct _tuple16 _tmp500=_tmp4FF;union Cyc_CfFlowInfo_FlowInfo _tmp514;_LL66: _tmp514=_tmp500.f1;_LL67:;{
struct Cyc_List_List*_tmp501=0;
struct Cyc_List_List*_tmp502=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp503=_tmp514;struct Cyc_Dict_Dict _tmp50F;struct Cyc_List_List*_tmp50E;if((_tmp503.ReachableFL).tag == 2){_LL69: _tmp50F=((_tmp503.ReachableFL).val).f1;_tmp50E=((_tmp503.ReachableFL).val).f2;_LL6A:
# 2792
{union Cyc_CfFlowInfo_AbsLVal _tmp504=_tmp515;struct Cyc_CfFlowInfo_Place*_tmp50D;if((_tmp504.PlaceL).tag == 1){_LL6E: _tmp50D=(_tmp504.PlaceL).val;_LL6F: {
# 2796
void*_tmp505=Cyc_CfFlowInfo_lookup_place(_tmp50F,_tmp50D);
({void*_tmp7DC=(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->tag=8U,_tmp506->f1=_tmp53A,_tmp506->f2=_tmp505;_tmp506;});_tmp505=_tmp7DC;});
({struct Cyc_Dict_Dict _tmp7DD=Cyc_CfFlowInfo_assign_place(_tmp4A6,_tmp539->loc,_tmp50F,_tmp50D,_tmp505);_tmp50F=_tmp7DD;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7DE=Cyc_CfFlowInfo_ReachableFL(_tmp50F,_tmp50E);_tmp514=_tmp7DE;});{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp507=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->tag=0U,_tmp50C->f1=_tmp53A;_tmp50C;});
struct Cyc_CfFlowInfo_Place*_tmp508=({struct Cyc_CfFlowInfo_Place*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->root=(void*)_tmp507,_tmp50B->path=0;_tmp50B;});
({struct Cyc_List_List*_tmp7DF=({struct Cyc_List_List*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->hd=_tmp508,_tmp509->tl=_tmp501;_tmp509;});_tmp501=_tmp7DF;});
({struct Cyc_List_List*_tmp7E0=({struct Cyc_List_List*_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->hd=_tmp539,_tmp50A->tl=_tmp502;_tmp50A;});_tmp502=_tmp7E0;});
goto _LL6D;};}}else{_LL70: _LL71:
# 2811
 goto _LL6D;}_LL6D:;}
# 2813
goto _LL68;}else{_LL6B: _LL6C:
# 2815
 goto _LL68;}_LL68:;}{
# 2818
struct Cyc_List_List _tmp510=({struct Cyc_List_List _tmp631;_tmp631.hd=_tmp53A,_tmp631.tl=0;_tmp631;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7E1=Cyc_NewControlFlow_add_vars(_tmp4A6,_tmp514,& _tmp510,_tmp4A6->unknown_all,_tmp538,0);_tmp514=_tmp7E1;});{
# 2822
struct Cyc_List_List*_tmp511=env->unique_pat_vars;
({struct Cyc_List_List*_tmp7E3=({struct Cyc_List_List*_tmp513=_cycalloc(sizeof(*_tmp513));({struct _tuple29*_tmp7E2=({struct _tuple29*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->f1=1,_tmp512->f2=s,_tmp512->f3=_tmp501,_tmp512->f4=_tmp502;_tmp512;});_tmp513->hd=_tmp7E2;}),_tmp513->tl=_tmp511;_tmp513;});env->unique_pat_vars=_tmp7E3;});
# 2827
({union Cyc_CfFlowInfo_FlowInfo _tmp7E4=Cyc_NewControlFlow_anal_stmt(env,_tmp514,_tmp537,rval_opt);_tmp514=_tmp7E4;});
env->unique_pat_vars=_tmp511;
# 2831
({union Cyc_CfFlowInfo_FlowInfo _tmp7E5=Cyc_NewControlFlow_unconsume_params(env,_tmp501,_tmp502,1,_tmp514,_tmp538);_tmp514=_tmp7E5;});
# 2835
return _tmp514;};};};};}}else{goto _LL26;}default: _LL26: _tmp53C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f1;_tmp53B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_LL27:
# 2838
({union Cyc_CfFlowInfo_FlowInfo _tmp7E9=({struct Cyc_NewControlFlow_AnalEnv*_tmp7E8=env;union Cyc_CfFlowInfo_FlowInfo _tmp7E7=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp53C);struct Cyc_Absyn_Stmt*_tmp7E6=_tmp53B;Cyc_NewControlFlow_anal_stmt(_tmp7E8,_tmp7E7,_tmp7E6,rval_opt);});outflow=_tmp7E9;});
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp53C);
return outflow;}default: _LL28: _tmp53D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp4A8)->f2;_LL29:
# 2843
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp53D,rval_opt);}_LL3:;};}
# 2849
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2853
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp540=env->fenv;
void*_tmp541=decl->r;void*_tmp542=_tmp541;struct Cyc_Absyn_Tvar*_tmp555;struct Cyc_Absyn_Vardecl*_tmp554;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Fndecl*_tmp552;struct Cyc_List_List*_tmp551;struct Cyc_Absyn_Vardecl*_tmp550;switch(*((int*)_tmp542)){case 0U: _LL1: _tmp550=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp542)->f1;_LL2: {
# 2863
struct Cyc_List_List _tmp543=({struct Cyc_List_List _tmp633;_tmp633.hd=_tmp550,_tmp633.tl=0;_tmp633;});
({union Cyc_CfFlowInfo_FlowInfo _tmp7EA=Cyc_NewControlFlow_add_vars(_tmp540,inflow,& _tmp543,_tmp540->unknown_none,decl->loc,0);inflow=_tmp7EA;});{
struct Cyc_Absyn_Exp*_tmp544=_tmp550->initializer;
if(_tmp544 == 0)
return inflow;{
struct _tuple16 _tmp545=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp544);struct _tuple16 _tmp546=_tmp545;union Cyc_CfFlowInfo_FlowInfo _tmp548;void*_tmp547;_LLC: _tmp548=_tmp546.f1;_tmp547=_tmp546.f2;_LLD:;
return Cyc_NewControlFlow_do_initialize_var(_tmp540,env,_tmp548,_tmp550,_tmp544,_tmp547,decl->loc);};};}case 3U: _LL3: _tmp551=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp542)->f1;_LL4:
# 2872
 return Cyc_NewControlFlow_add_vars(_tmp540,inflow,_tmp551,_tmp540->unknown_none,decl->loc,0);case 1U: _LL5: _tmp552=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp542)->f1;_LL6:
# 2875
 Cyc_NewControlFlow_check_nested_fun(env->all_tables,_tmp540,inflow,_tmp552);{
void*t=(void*)_check_null(_tmp552->cached_type);
struct Cyc_Absyn_Vardecl*_tmp549=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp552->fn_vardecl);
# 2881
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp7EE=_tmp540;union Cyc_CfFlowInfo_FlowInfo _tmp7ED=inflow;struct Cyc_List_List*_tmp7EC=({struct Cyc_List_List*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->hd=_tmp549,_tmp54A->tl=0;_tmp54A;});void*_tmp7EB=_tmp540->unknown_all;Cyc_NewControlFlow_add_vars(_tmp7EE,_tmp7ED,_tmp7EC,_tmp7EB,decl->loc,0);});};case 4U: _LL7: _tmp555=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp542)->f1;_tmp554=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp542)->f2;_tmp553=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp542)->f3;_LL8:
# 2884
 if(_tmp553 != 0)
({void*_tmp54B=0U;({struct _dyneither_ptr _tmp7EF=({const char*_tmp54C="found open expression in declaration!";_tag_dyneither(_tmp54C,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7EF,_tag_dyneither(_tmp54B,sizeof(void*),0U));});});{
struct Cyc_List_List _tmp54D=({struct Cyc_List_List _tmp634;_tmp634.hd=_tmp554,_tmp634.tl=0;_tmp634;});
return Cyc_NewControlFlow_add_vars(_tmp540,inflow,& _tmp54D,_tmp540->unknown_all,decl->loc,0);};default: _LL9: _LLA:
# 2890
({void*_tmp54E=0U;({struct _dyneither_ptr _tmp7F0=({const char*_tmp54F="anal_decl: unexpected decl variant";_tag_dyneither(_tmp54F,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7F0,_tag_dyneither(_tmp54E,sizeof(void*),0U));});});}_LL0:;}
# 2898
static void Cyc_NewControlFlow_check_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct _handler_cons _tmp556;_push_handler(& _tmp556);{int _tmp558=0;if(setjmp(_tmp556.handler))_tmp558=1;if(!_tmp558){
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env();
({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp7F3=tables;struct Cyc_CfFlowInfo_FlowEnv*_tmp7F2=fenv;union Cyc_CfFlowInfo_FlowInfo _tmp7F1=Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0);Cyc_NewControlFlow_check_nested_fun(_tmp7F3,_tmp7F2,_tmp7F1,fd);});}
# 2900
;_pop_handler();}else{void*_tmp557=(void*)_exn_thrown;void*_tmp559=_tmp557;void*_tmp55B;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp559)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 2906
 if(Cyc_Position_num_errors > 0)
goto _LL0;else{
Cyc_Core_rethrow((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->tag=Cyc_Dict_Absent;_tmp55A;}));}}else{_LL3: _tmp55B=_tmp559;_LL4:(int)_rethrow(_tmp55B);}_LL0:;}};}
# 2912
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2917
static union Cyc_Relations_RelnOp Cyc_NewControlFlow_translate_rop(struct Cyc_List_List*vds,union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp55C=r;unsigned int _tmp55E;if((_tmp55C.RParam).tag == 5){_LL1: _tmp55E=(_tmp55C.RParam).val;_LL2: {
# 2920
struct Cyc_Absyn_Vardecl*_tmp55D=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(vds,(int)_tmp55E);
if(!_tmp55D->escapes)
return Cyc_Relations_RVar(_tmp55D);
return r;}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 2928
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2932
unsigned int _tmp55F=(fd->body)->loc;
({union Cyc_CfFlowInfo_FlowInfo _tmp7F4=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp55F,1);inflow=_tmp7F4;});{
# 2937
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
struct _tuple14 _tmp560=({union Cyc_CfFlowInfo_FlowInfo _tmp58F=inflow;if((_tmp58F.ReachableFL).tag != 2)_throw_match();(_tmp58F.ReachableFL).val;});struct _tuple14 _tmp561=_tmp560;struct Cyc_Dict_Dict _tmp58E;struct Cyc_List_List*_tmp58D;_LL1: _tmp58E=_tmp561.f1;_tmp58D=_tmp561.f2;_LL2:;{
# 2942
struct Cyc_List_List*_tmp562=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;
{struct Cyc_List_List*_tmp563=fd->requires_relns;for(0;_tmp563 != 0;_tmp563=_tmp563->tl){
struct Cyc_Relations_Reln*_tmp564=(struct Cyc_Relations_Reln*)_tmp563->hd;
({struct Cyc_List_List*_tmp7F9=({struct _RegionHandle*_tmp7F8=Cyc_Core_heap_region;union Cyc_Relations_RelnOp _tmp7F7=Cyc_NewControlFlow_translate_rop(_tmp562,_tmp564->rop1);enum Cyc_Relations_Relation _tmp7F6=_tmp564->relation;union Cyc_Relations_RelnOp _tmp7F5=
Cyc_NewControlFlow_translate_rop(_tmp562,_tmp564->rop2);
# 2945
Cyc_Relations_add_relation(_tmp7F8,_tmp7F7,_tmp7F6,_tmp7F5,_tmp58D);});_tmp58D=_tmp7F9;});}}{
# 2951
struct Cyc_List_List*atts;
{void*_tmp565=Cyc_Tcutil_compress((void*)_check_null(fd->cached_type));void*_tmp566=_tmp565;struct Cyc_List_List*_tmp569;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp566)->tag == 5U){_LL4: _tmp569=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp566)->f1).attributes;_LL5:
 atts=_tmp569;goto _LL3;}else{_LL6: _LL7:
({void*_tmp567=0U;({struct _dyneither_ptr _tmp7FA=({const char*_tmp568="check_fun: non-function type cached with fndecl_t";_tag_dyneither(_tmp568,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7FA,_tag_dyneither(_tmp567,sizeof(void*),0U));});});}_LL3:;}
# 2958
for(0;atts != 0;atts=atts->tl){
void*_tmp56A=(void*)atts->hd;void*_tmp56B=_tmp56A;int _tmp584;int _tmp583;int _tmp582;switch(*((int*)_tmp56B)){case 21U: _LL9: _tmp582=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp56B)->f1;_LLA: {
# 2961
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0U);
struct Cyc_Absyn_Vardecl*_tmp56C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp582 - 1);
void*t=Cyc_Tcutil_compress(_tmp56C->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp56D=({
struct Cyc_CfFlowInfo_FlowEnv*_tmp7FD=fenv;void*_tmp7FC=elttype;struct Cyc_Absyn_Exp*_tmp7FB=bogus_exp;Cyc_CfFlowInfo_make_unique_consumed(_tmp7FD,_tmp7FC,_tmp7FB,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));});
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp56E=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=2U,_tmp573->f1=_tmp582,_tmp573->f2=t;_tmp573;});
struct Cyc_CfFlowInfo_Place*_tmp56F=({struct Cyc_CfFlowInfo_Place*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->root=(void*)_tmp56E,_tmp572->path=0;_tmp572;});
({struct Cyc_Dict_Dict _tmp7FE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp58E,(void*)_tmp56E,_tmp56D);_tmp58E=_tmp7FE;});
({struct Cyc_Dict_Dict _tmp801=({struct Cyc_Dict_Dict _tmp800=_tmp58E;void*_tmp7FF=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->tag=0U,_tmp570->f1=_tmp56C;_tmp570;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp800,_tmp7FF,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->tag=4U,_tmp571->f1=_tmp56F;_tmp571;}));});_tmp58E=_tmp801;});
goto _LL8;}case 20U: _LLB: _tmp583=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp56B)->f1;_LLC: {
# 2974
struct Cyc_Absyn_Vardecl*_tmp574=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp583 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp574->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp575=({struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=2U,_tmp57B->f1=_tmp583,_tmp57B->f2=elttype;_tmp57B;});
struct Cyc_CfFlowInfo_Place*_tmp576=({struct Cyc_CfFlowInfo_Place*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->root=(void*)_tmp575,_tmp57A->path=0;_tmp57A;});
({struct Cyc_Dict_Dict _tmp804=({struct Cyc_Dict_Dict _tmp803=_tmp58E;void*_tmp802=(void*)_tmp575;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp803,_tmp802,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));});_tmp58E=_tmp804;});
({struct Cyc_Dict_Dict _tmp807=({struct Cyc_Dict_Dict _tmp806=_tmp58E;void*_tmp805=(void*)({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->tag=0U,_tmp577->f1=_tmp574;_tmp577;});((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp806,_tmp805,(void*)({struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->tag=4U,_tmp578->f1=_tmp576;_tmp578;}));});_tmp58E=_tmp807;});
({struct Cyc_List_List*_tmp808=({struct Cyc_List_List*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->hd=_tmp576,_tmp579->tl=param_roots;_tmp579;});param_roots=_tmp808;});
goto _LL8;}case 22U: _LLD: _tmp584=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp56B)->f1;_LLE: {
# 2983
struct Cyc_Absyn_Vardecl*_tmp57C=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp584 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp57C->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp57D=({struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->tag=0U,_tmp581->f1=_tmp57C;_tmp581;});
struct Cyc_CfFlowInfo_Place*_tmp57E=({struct Cyc_CfFlowInfo_Place*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->root=(void*)_tmp57D,_tmp580->path=0;_tmp580;});
({struct Cyc_List_List*_tmp809=({struct Cyc_List_List*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->hd=_tmp57E,_tmp57F->tl=noconsume_roots;_tmp57F;});noconsume_roots=_tmp809;});}
# 2989
goto _LL8;}default: _LLF: _LL10:
 goto _LL8;}_LL8:;}
# 2992
({union Cyc_CfFlowInfo_FlowInfo _tmp80A=Cyc_CfFlowInfo_ReachableFL(_tmp58E,_tmp58D);inflow=_tmp80A;});{
# 2994
int noreturn=Cyc_Tcutil_is_noreturn_fn_type(Cyc_Tcutil_fndecl2type(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create)(33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*_tmp58C=_cycalloc(sizeof(*_tmp58C));
_tmp58C->all_tables=tables,({
struct Cyc_Hashtable_Table*_tmp80C=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->succ_tables,fd);_tmp58C->succ_table=_tmp80C;}),({
struct Cyc_Hashtable_Table*_tmp80B=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(tables->pat_pop_tables,fd);_tmp58C->pat_pop_table=_tmp80B;}),_tmp58C->fenv=fenv,_tmp58C->iterate_again=1,_tmp58C->iteration_num=0,_tmp58C->in_try=0,_tmp58C->tryflow=inflow,_tmp58C->noreturn=noreturn,_tmp58C->return_type=fd->ret_type,_tmp58C->unique_pat_vars=0,_tmp58C->param_roots=param_roots,_tmp58C->noconsume_params=noconsume_roots,_tmp58C->flow_table=flow_table,_tmp58C->return_relns=fd->ensures_relns;_tmp58C;});
# 3004
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 3010
env->iterate_again=0;
({union Cyc_CfFlowInfo_FlowInfo _tmp80D=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);outflow=_tmp80D;});}{
# 3013
union Cyc_CfFlowInfo_FlowInfo _tmp585=outflow;if((_tmp585.BottomFL).tag == 1){_LL12: _LL13:
 goto _LL11;}else{_LL14: _LL15:
# 3016
 Cyc_NewControlFlow_check_init_params(_tmp55F,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp55F);
# 3020
if(noreturn)
({void*_tmp586=0U;({unsigned int _tmp80F=_tmp55F;struct _dyneither_ptr _tmp80E=({const char*_tmp587="`noreturn' function might (implicitly) return";_tag_dyneither(_tmp587,sizeof(char),46U);});Cyc_CfFlowInfo_aerr(_tmp80F,_tmp80E,_tag_dyneither(_tmp586,sizeof(void*),0U));});});else{
if(!Cyc_Tcutil_is_void_type(fd->ret_type)){
if(Cyc_Tcutil_is_any_float_type(fd->ret_type) || 
Cyc_Tcutil_is_any_int_type(fd->ret_type))
({void*_tmp588=0U;({unsigned int _tmp811=_tmp55F;struct _dyneither_ptr _tmp810=({const char*_tmp589="function may not return a value";_tag_dyneither(_tmp589,sizeof(char),32U);});Cyc_Warn_warn(_tmp811,_tmp810,_tag_dyneither(_tmp588,sizeof(void*),0U));});});else{
# 3027
({void*_tmp58A=0U;({unsigned int _tmp813=_tmp55F;struct _dyneither_ptr _tmp812=({const char*_tmp58B="function may not return a value";_tag_dyneither(_tmp58B,sizeof(char),32U);});Cyc_CfFlowInfo_aerr(_tmp813,_tmp812,_tag_dyneither(_tmp58A,sizeof(void*),0U));});});}goto _LL11;}}
# 3029
goto _LL11;}_LL11:;};};};};};}
# 3033
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp590=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp591=_tmp590;struct Cyc_List_List*_tmp595;struct Cyc_List_List*_tmp594;struct Cyc_List_List*_tmp593;struct Cyc_Absyn_Fndecl*_tmp592;switch(*((int*)_tmp591)){case 1U: _LL1: _tmp592=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp591)->f1;_LL2:
 Cyc_NewControlFlow_check_fun(tables,_tmp592);goto _LL0;case 11U: _LL3: _tmp593=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp591)->f1;_LL4:
 _tmp594=_tmp593;goto _LL6;case 10U: _LL5: _tmp594=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp591)->f2;_LL6:
 _tmp595=_tmp594;goto _LL8;case 9U: _LL7: _tmp595=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp591)->f2;_LL8:
 Cyc_NewControlFlow_cf_check(tables,_tmp595);goto _LL0;case 12U: _LL9: _LLA:
 goto _LL0;default: _LLB: _LLC:
 goto _LL0;}_LL0:;};}}
