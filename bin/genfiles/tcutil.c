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
#define _check_null(ptr) \
  ({ void*_cks_null = (void*)(ptr); \
     if (!_cks_null) _throw_null(); \
     _cks_null; })
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
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 899 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 901
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 912
void*Cyc_Absyn_compress_kb(void*);
# 918
int Cyc_Absyn_type2bool(int def,void*);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 928
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 930
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 935
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 937
extern void*Cyc_Absyn_empty_effect;
# 939
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 960
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 966
extern void*Cyc_Absyn_fat_bound_type;
# 968
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 970
void*Cyc_Absyn_bounds_one();
# 972
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 977
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 1001
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 1003
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
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
int Cyc_Absyn_fntype_att(void*);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
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
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
int Cyc_Tcutil_is_void_type(void*);
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_signed_type(void*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 54
int Cyc_Tcutil_is_dyneither_ptr(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
int Cyc_Tcutil_is_bound_one(void*b);
# 59
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 62
int Cyc_Tcutil_is_bits_only_type(void*t);
# 64
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 69
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 71
void*Cyc_Tcutil_pointer_region(void*t);
# 74
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 77
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 80
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 82
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 84
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 86
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 90
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 94
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 102
void*Cyc_Tcutil_copy_type(void*t);
# 105
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 108
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 112
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 125
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 127
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 129
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple14{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 133
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 135
extern int Cyc_Tcutil_warn_alias_coerce;
# 138
extern int Cyc_Tcutil_warn_region_coerce;
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 151
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 156
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 161
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 170
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 175
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 180
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 184
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 189
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 191
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 195
void Cyc_Tcutil_explain_failure();
# 197
int Cyc_Tcutil_unify(void*,void*);
# 199
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 202
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 204
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 209
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 213
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 217
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 220
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 224
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 226
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 230
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple16{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*,struct Cyc_Absyn_Tvar*);
# 236
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 263 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 265
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 273
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 276
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 283
void Cyc_Tcutil_check_nonzero_bound(unsigned int,void*);
# 285
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,void*,int do_warn);
# 287
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 289
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 297
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 302
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 305
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 310
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 315
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple17{int f1;void*f2;};
# 319
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 327
void*Cyc_Tcutil_normalize_effect(void*e);
# 330
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 332
int Cyc_Tcutil_new_tvar_id();
# 334
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 337
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 339
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 342
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 345
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 348
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 351
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 355
int Cyc_Tcutil_supports_default(void*);
# 359
int Cyc_Tcutil_admits_zero(void*t);
# 363
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 367
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 370
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 374
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 377
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 381
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 384
int Cyc_Tcutil_zeroable_type(void*t);
# 388
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 391
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 393
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**te);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 40 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations-ap.h"
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
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 42 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_tmp1=_tmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 49
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 56
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_tmp5=_tmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 63
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_tmp7=_tmp6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 71
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_tmp9=_tmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 79
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 87
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_tmpD=_tmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 95
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_tmpF=_tmpE;void*_tmp11;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {
# 98
void*_tmp10=_tmp11;switch(*((int*)_tmp10)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 105
 return 0;}_LL0:;}
# 109
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_tmp13=_tmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
 return 1;}else{goto _LL5;}case 2U: _LL3: _LL4:
 return 1;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}
# 117
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 121
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 125
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Tcutil_compress(t);void*_tmp15=_tmp14;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 133
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_type_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 141
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp17=_tmp16;void*_tmp19;switch(*((int*)_tmp17)){case 0U: _LL1: _tmp19=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17)->f1;_LL2: {
# 144
void*_tmp18=_tmp19;switch(*((int*)_tmp18)){case 1U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 goto _LLF;case 4U: _LLE: _LLF:
 return 1;default: _LL10: _LL11:
 return 0;}_LL7:;}case 1U: _LL3: _LL4:
# 151
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);default: _LL5: _LL6:
 return 0;}_LL0:;}
# 157
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1A=Cyc_Tcutil_compress(t);void*_tmp1B=_tmp1A;void*_tmp1C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B)->tag == 3U){_LL1: _tmp1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B)->f1).ptr_atts).zero_term;_LL2:
# 160
 return Cyc_Tcutil_force_type2bool(0,_tmp1C);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 166
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1D=Cyc_Tcutil_compress(t);void*_tmp1E=_tmp1D;void*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->tag == 3U){_LL1: _tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).nullable;_LL2:
# 169
 return Cyc_Tcutil_force_type2bool(0,_tmp1F);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 175
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp20=Cyc_Tcutil_compress(t);void*_tmp21=_tmp20;void*_tmp22;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->tag == 3U){_LL1: _tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).bounds;_LL2:
# 178
 return Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp22);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 185
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp23=Cyc_Tcutil_compress(t);void*_tmp24=_tmp23;void*_tmp26;void*_tmp25;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->tag == 3U){_LL1: _tmp26=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).elt_type;_tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_LL2:
# 188
 if(Cyc_Tcutil_unify(_tmp25,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp26;
return 1;}else{
# 192
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 199
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp27=Cyc_Tcutil_compress(t);void*_tmp28=_tmp27;void*_tmp2A;void*_tmp29;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28)->tag == 3U){_LL1: _tmp2A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28)->f1).ptr_atts).zero_term;_LL2:
# 202
*elt_type_dest=_tmp2A;
return Cyc_Absyn_type2bool(0,_tmp29);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 211
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 213
void*_tmp2B=Cyc_Tcutil_compress(t);void*_tmp2C=_tmp2B;void*_tmp35;struct Cyc_Absyn_Tqual _tmp34;struct Cyc_Absyn_Exp*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;void*_tmp2F;switch(*((int*)_tmp2C)){case 3U: _LL1: _tmp31=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C)->f1).elt_type;_tmp30=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C)->f1).ptr_atts).bounds;_tmp2F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C)->f1).ptr_atts).zero_term;_LL2:
# 215
 if(Cyc_Absyn_type2bool(0,_tmp2F)){
*ptr_type=t;
*elt_type=_tmp31;
{void*_tmp2D=Cyc_Tcutil_compress(_tmp30);void*_tmp2E=_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E)->f1)->tag == 14U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 222
return 1;}else{
return 0;}case 4U: _LL3: _tmp35=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).elt_type;_tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).tq;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).num_elts;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C)->f1).zero_term;_LL4:
# 225
 if(Cyc_Absyn_type2bool(0,_tmp32)){
*elt_type=_tmp35;
*is_dyneither=0;
({void*_tmpA34=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmpA34;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 238
int Cyc_Tcutil_is_tagged_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_tagged_pointer_type_elt(t,& ignore);}
# 244
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp36=({void*_tmpA35=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpA35,b);});
if(_tmp36 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp37=_tmp36;
struct _tuple13 _tmp38=Cyc_Evexp_eval_const_uint_exp(_tmp37);struct _tuple13 _tmp39=_tmp38;unsigned int _tmp3B;int _tmp3A;_LL1: _tmp3B=_tmp39.f1;_tmp3A=_tmp39.f2;_LL2:;
return _tmp3A  && _tmp3B == 1;};}
# 253
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3C=Cyc_Tcutil_compress(t);void*_tmp3D=_tmp3C;struct Cyc_List_List*_tmp49;struct Cyc_List_List*_tmp48;void*_tmp47;void*_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp45;struct Cyc_List_List*_tmp44;void*_tmp43;switch(*((int*)_tmp3D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f2 == 0){_LL1: _tmp43=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f1;_LL2: {
# 256
void*_tmp3E=_tmp43;switch(*((int*)_tmp3E)){case 0U: _LL10: _LL11:
 goto _LL13;case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 return 1;case 15U: _LL16: _LL17:
 goto _LL19;case 16U: _LL18: _LL19:
 return 0;default: _LL1A: _LL1B:
 return 0;}_LLF:;}}else{if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f1)->f1).UnknownAggr).tag == 1){_LL7: _LL8:
# 272
 return 0;}else{_LL9: _tmp45=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f1)->f1).KnownAggr).val;_tmp44=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f2;_LLA:
# 274
 if(_tmp45->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp45->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp3F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3F;_push_region(rgn);
{struct Cyc_List_List*_tmp40=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp45->tvs,_tmp44);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp45->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,_tmp40,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp41=0;_npop_handler(0U);return _tmp41;}}}{
int _tmp42=1;_npop_handler(0U);return _tmp42;};}
# 281
;_pop_region(rgn);};};}}else{goto _LLD;}}case 4U: _LL3: _tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).elt_type;_tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).zero_term;_LL4:
# 267
 return !Cyc_Absyn_type2bool(0,_tmp46) && Cyc_Tcutil_is_bits_only_type(_tmp47);case 6U: _LL5: _tmp48=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3D)->f1;_LL6:
# 269
 for(0;_tmp48 != 0;_tmp48=_tmp48->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)_tmp48->hd)).f2))return 0;}
return 1;case 7U: _LLB: _tmp49=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3D)->f2;_LLC:
# 286
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp49->hd)->type))return 0;}
return 1;default: _LLD: _LLE:
 return 0;}_LL0:;}
# 295
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
{void*_tmp4A=Cyc_Tcutil_compress(t);void*_tmp4B=_tmp4A;struct Cyc_List_List*_tmp4F;void*_tmp4E;switch(*((int*)_tmp4B)){case 3U: _LL1: _tmp4E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4E);case 5U: _LL3: _tmp4F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4B)->f1).attributes;_LL4:
# 299
 for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){
void*_tmp4C=(void*)_tmp4F->hd;void*_tmp4D=_tmp4C;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4D)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 304
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 307
return 0;}char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 311
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 313
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 317
int Cyc_Tcutil_warn_region_coerce=0;
# 320
static void*Cyc_Tcutil_t1_failure=0;
static int Cyc_Tcutil_tq1_const=0;
static void*Cyc_Tcutil_t2_failure=0;
static int Cyc_Tcutil_tq2_const=0;
# 325
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 329
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 334
if(({struct _dyneither_ptr _tmpA36=({const char*_tmp50="(qualifiers don't match)";_tag_dyneither(_tmp50,sizeof(char),25U);});Cyc_strcmp(_tmpA36,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp53=({struct Cyc_String_pa_PrintArg_struct _tmp99E;_tmp99E.tag=0U,_tmp99E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp99E;});void*_tmp51[1U];_tmp51[0]=& _tmp53;({struct Cyc___cycFILE*_tmpA38=Cyc_stderr;struct _dyneither_ptr _tmpA37=({const char*_tmp52="  %s\n";_tag_dyneither(_tmp52,sizeof(char),6U);});Cyc_fprintf(_tmpA38,_tmpA37,_tag_dyneither(_tmp51,sizeof(void*),1U));});});
return;}
# 339
if(({struct _dyneither_ptr _tmpA39=({const char*_tmp54="(function effects do not match)";_tag_dyneither(_tmp54,sizeof(char),32U);});Cyc_strcmp(_tmpA39,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp55=Cyc_Absynpp_tc_params_r;
_tmp55.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp55);}{
# 344
void*_tmp56=Cyc_Tcutil_t1_failure;
void*_tmp57=Cyc_Tcutil_t2_failure;
struct _dyneither_ptr s1=(unsigned int)_tmp56?Cyc_Absynpp_typ2string(_tmp56):({const char*_tmp72="<?>";_tag_dyneither(_tmp72,sizeof(char),4U);});
struct _dyneither_ptr s2=(unsigned int)_tmp57?Cyc_Absynpp_typ2string(_tmp57):({const char*_tmp71="<?>";_tag_dyneither(_tmp71,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp99F;_tmp99F.tag=0U,_tmp99F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp99F;});void*_tmp58[1U];_tmp58[0]=& _tmp5A;({struct Cyc___cycFILE*_tmpA3B=Cyc_stderr;struct _dyneither_ptr _tmpA3A=({const char*_tmp59="  %s";_tag_dyneither(_tmp59,sizeof(char),5U);});Cyc_fprintf(_tmpA3B,_tmpA3A,_tag_dyneither(_tmp58,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp5B=0U;({struct Cyc___cycFILE*_tmpA3D=Cyc_stderr;struct _dyneither_ptr _tmpA3C=({const char*_tmp5C="\n\t";_tag_dyneither(_tmp5C,sizeof(char),3U);});Cyc_fprintf(_tmpA3D,_tmpA3C,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});
pos=8;}else{
# 355
({void*_tmp5D=0U;({struct Cyc___cycFILE*_tmpA3F=Cyc_stderr;struct _dyneither_ptr _tmpA3E=({const char*_tmp5E=" ";_tag_dyneither(_tmp5E,sizeof(char),2U);});Cyc_fprintf(_tmpA3F,_tmpA3E,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});
++ pos;}
# 358
({void*_tmp5F=0U;({struct Cyc___cycFILE*_tmpA41=Cyc_stderr;struct _dyneither_ptr _tmpA40=({const char*_tmp60="and ";_tag_dyneither(_tmp60,sizeof(char),5U);});Cyc_fprintf(_tmpA41,_tmpA40,_tag_dyneither(_tmp5F,sizeof(void*),0U));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp61=0U;({struct Cyc___cycFILE*_tmpA43=Cyc_stderr;struct _dyneither_ptr _tmpA42=({const char*_tmp62="\n\t";_tag_dyneither(_tmp62,sizeof(char),3U);});Cyc_fprintf(_tmpA43,_tmpA42,_tag_dyneither(_tmp61,sizeof(void*),0U));});});
pos=8;}
# 364
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp9A0;_tmp9A0.tag=0U,_tmp9A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp9A0;});void*_tmp63[1U];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmpA45=Cyc_stderr;struct _dyneither_ptr _tmpA44=({const char*_tmp64="%s ";_tag_dyneither(_tmp64,sizeof(char),4U);});Cyc_fprintf(_tmpA45,_tmpA44,_tag_dyneither(_tmp63,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp66=0U;({struct Cyc___cycFILE*_tmpA47=Cyc_stderr;struct _dyneither_ptr _tmpA46=({const char*_tmp67="\n\t";_tag_dyneither(_tmp67,sizeof(char),3U);});Cyc_fprintf(_tmpA47,_tmpA46,_tag_dyneither(_tmp66,sizeof(void*),0U));});});
pos=8;}
# 370
({void*_tmp68=0U;({struct Cyc___cycFILE*_tmpA49=Cyc_stderr;struct _dyneither_ptr _tmpA48=({const char*_tmp69="are not compatible. ";_tag_dyneither(_tmp69,sizeof(char),21U);});Cyc_fprintf(_tmpA49,_tmpA48,_tag_dyneither(_tmp68,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpA4A=(char*)Cyc_Tcutil_failure_reason.curr;_tmpA4A != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({int _tmpA4B=pos;_tmpA4B + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80)
({void*_tmp6A=0U;({struct Cyc___cycFILE*_tmpA4D=Cyc_stderr;struct _dyneither_ptr _tmpA4C=({const char*_tmp6B="\n\t";_tag_dyneither(_tmp6B,sizeof(char),3U);});Cyc_fprintf(_tmpA4D,_tmpA4C,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});
# 376
({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp9A1;_tmp9A1.tag=0U,_tmp9A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp9A1;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6E;({struct Cyc___cycFILE*_tmpA4F=Cyc_stderr;struct _dyneither_ptr _tmpA4E=({const char*_tmp6D="%s";_tag_dyneither(_tmp6D,sizeof(char),3U);});Cyc_fprintf(_tmpA4F,_tmpA4E,_tag_dyneither(_tmp6C,sizeof(void*),1U));});});}
# 378
({void*_tmp6F=0U;({struct Cyc___cycFILE*_tmpA51=Cyc_stderr;struct _dyneither_ptr _tmpA50=({const char*_tmp70="\n";_tag_dyneither(_tmp70,sizeof(char),2U);});Cyc_fprintf(_tmpA51,_tmpA50,_tag_dyneither(_tmp6F,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 382
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 384
Cyc_Position_post_error(({unsigned int _tmpA52=loc;Cyc_Position_mk_err(_tmpA52,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 386
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 389
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
if(!Cyc_Position_error_p()){
({struct Cyc_String_pa_PrintArg_struct _tmp75=({struct Cyc_String_pa_PrintArg_struct _tmp9A2;_tmp9A2.tag=0U,_tmp9A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmp9A2;});void*_tmp73[1U];_tmp73[0]=& _tmp75;({struct Cyc___cycFILE*_tmpA54=Cyc_stderr;struct _dyneither_ptr _tmpA53=({const char*_tmp74="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp74,sizeof(char),36U);});Cyc_fprintf(_tmpA54,_tmpA53,_tag_dyneither(_tmp73,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);}
# 394
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=Cyc_Core_Impossible,_tmp76->f1=msg;_tmp76;}));}
# 396
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 398
Cyc_Warn_vwarn(sg,fmt,ap);}
# 401
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp77=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp78=_tmp77;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp78)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp78)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 411
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 416
void*Cyc_Tcutil_compress(void*t){
void*_tmp79=t;void*_tmp82;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp80;void**_tmp7F;void**_tmp7E;switch(*((int*)_tmp79)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp79)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp7E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp79)->f2;_LL8: {
# 427
void*ta=(void*)_check_null(*_tmp7E);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7E=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp79)->f4 == 0){_LL3: _LL4:
# 419
 return t;}else{_LL5: _tmp7F=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp79)->f4;_LL6: {
# 421
void*ta=(void*)_check_null(*_tmp7F);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7F=t2;
return t2;}}case 9U: _LL9: _tmp80=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp79)->f1;_LLA:
# 433
 Cyc_Evexp_eval_const_uint_exp(_tmp80);{
void*_tmp7A=_tmp80->r;void*_tmp7B=_tmp7A;void*_tmp7C;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7B)->tag == 39U){_LL12: _tmp7C=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7B)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp7C);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp81=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp79)->f1;_LLC: {
# 439
void*_tmp7D=_tmp81->topt;
if(_tmp7D != 0)return _tmp7D;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp79)->f2 != 0){_LLD: _tmp82=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp79)->f2;_LLE:
# 443
 return Cyc_Tcutil_compress(_tmp82);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 452
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 456
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp83=Cyc_Absyn_compress_kb(kb);void*_tmp84=_tmp83;struct Cyc_Absyn_Kind*_tmp87;switch(*((int*)_tmp84)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=1U,_tmp85->f1=0;_tmp85;});case 2U: _LL3: _tmp87=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp84)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=2U,_tmp86->f1=0,_tmp86->f2=_tmp87;_tmp86;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 463
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 465
return({struct Cyc_Absyn_Tvar*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->name=tv->name,_tmp88->identity=- 1,({void*_tmpA55=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp88->kind=_tmpA55;});_tmp88;});}
# 467
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 469
struct _tuple10*_tmp89=arg;struct _dyneither_ptr*_tmp8D;struct Cyc_Absyn_Tqual _tmp8C;void*_tmp8B;_LL1: _tmp8D=_tmp89->f1;_tmp8C=_tmp89->f2;_tmp8B=_tmp89->f3;_LL2:;
return({struct _tuple10*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=_tmp8D,_tmp8A->f2=_tmp8C,({void*_tmpA56=Cyc_Tcutil_copy_type(_tmp8B);_tmp8A->f3=_tmpA56;});_tmp8A;});}
# 472
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp8E=arg;struct Cyc_Absyn_Tqual _tmp91;void*_tmp90;_LL1: _tmp91=_tmp8E->f1;_tmp90=_tmp8E->f2;_LL2:;
return({struct _tuple12*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->f1=_tmp91,({void*_tmpA57=Cyc_Tcutil_copy_type(_tmp90);_tmp8F->f2=_tmpA57;});_tmp8F;});}
# 476
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 478
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->name=f->name,_tmp92->tq=f->tq,({void*_tmpA59=Cyc_Tcutil_copy_type(f->type);_tmp92->type=_tmpA59;}),_tmp92->width=f->width,_tmp92->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpA58=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp92->requires_clause=_tmpA58;});_tmp92;});}
# 482
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp93=x;void*_tmp96;void*_tmp95;_LL1: _tmp96=_tmp93->f1;_tmp95=_tmp93->f2;_LL2:;
return({struct _tuple0*_tmp94=_cycalloc(sizeof(*_tmp94));({void*_tmpA5B=Cyc_Tcutil_copy_type(_tmp96);_tmp94->f1=_tmpA5B;}),({void*_tmpA5A=Cyc_Tcutil_copy_type(_tmp95);_tmp94->f2=_tmpA5A;});_tmp94;});}
# 486
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->name=f->name,_tmp97->tag=f->tag,_tmp97->loc=f->loc;_tmp97;});}
# 489
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 493
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp98=Cyc_Tcutil_compress(t);void*_tmp99=_tmp98;struct Cyc_Absyn_Datatypedecl*_tmpDD;struct Cyc_Absyn_Enumdecl*_tmpDC;struct Cyc_Absyn_Aggrdecl*_tmpDB;struct _tuple2*_tmpDA;struct Cyc_List_List*_tmpD9;struct Cyc_Absyn_Typedefdecl*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;enum Cyc_Absyn_AggrKind _tmpD5;struct Cyc_List_List*_tmpD4;struct Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD2;void*_tmpD1;struct Cyc_Absyn_Tqual _tmpD0;void*_tmpCF;struct Cyc_List_List*_tmpCE;int _tmpCD;struct Cyc_Absyn_VarargInfo*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_List_List*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_List_List*_tmpC6;void*_tmpC5;struct Cyc_Absyn_Tqual _tmpC4;struct Cyc_Absyn_Exp*_tmpC3;void*_tmpC2;unsigned int _tmpC1;void*_tmpC0;struct Cyc_Absyn_Tqual _tmpBF;void*_tmpBE;void*_tmpBD;void*_tmpBC;void*_tmpBB;struct Cyc_Absyn_PtrLoc*_tmpBA;struct Cyc_Absyn_Tvar*_tmpB9;void*_tmpB8;struct Cyc_List_List*_tmpB7;void*_tmpB6;switch(*((int*)_tmp99)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp99)->f2 == 0){_LL1: _tmpB6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp99)->f1;_LL2:
 return t;}else{_LL3: _tmpB8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp99)->f1;_tmpB7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp99)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=0U,_tmp9A->f1=_tmpB8,({struct Cyc_List_List*_tmpA5C=Cyc_Tcutil_copy_types(_tmpB7);_tmp9A->f2=_tmpA5C;});_tmp9A;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmpB9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp99)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmpB9));case 3U: _LL9: _tmpC0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99)->f1).elt_type;_tmpBF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99)->f1).elt_tq;_tmpBE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99)->f1).ptr_atts).rgn;_tmpBD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99)->f1).ptr_atts).nullable;_tmpBC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99)->f1).ptr_atts).bounds;_tmpBB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99)->f1).ptr_atts).zero_term;_tmpBA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99)->f1).ptr_atts).ptrloc;_LLA: {
# 500
void*_tmp9B=Cyc_Tcutil_copy_type(_tmpC0);
void*_tmp9C=Cyc_Tcutil_copy_type(_tmpBE);
void*_tmp9D=Cyc_Tcutil_copy_type(_tmpBD);
struct Cyc_Absyn_Tqual _tmp9E=_tmpBF;
# 505
void*_tmp9F=Cyc_Tcutil_copy_type(_tmpBC);
void*_tmpA0=Cyc_Tcutil_copy_type(_tmpBB);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=3U,(_tmpA1->f1).elt_type=_tmp9B,(_tmpA1->f1).elt_tq=_tmp9E,((_tmpA1->f1).ptr_atts).rgn=_tmp9C,((_tmpA1->f1).ptr_atts).nullable=_tmp9D,((_tmpA1->f1).ptr_atts).bounds=_tmp9F,((_tmpA1->f1).ptr_atts).zero_term=_tmpA0,((_tmpA1->f1).ptr_atts).ptrloc=_tmpBA;_tmpA1;});}case 4U: _LLB: _tmpC5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp99)->f1).elt_type;_tmpC4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp99)->f1).tq;_tmpC3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp99)->f1).num_elts;_tmpC2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp99)->f1).zero_term;_tmpC1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp99)->f1).zt_loc;_LLC: {
# 509
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC3);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=4U,({void*_tmpA5E=Cyc_Tcutil_copy_type(_tmpC5);(_tmpA2->f1).elt_type=_tmpA5E;}),(_tmpA2->f1).tq=_tmpC4,(_tmpA2->f1).num_elts=eopt2,({
void*_tmpA5D=Cyc_Tcutil_copy_type(_tmpC2);(_tmpA2->f1).zero_term=_tmpA5D;}),(_tmpA2->f1).zt_loc=_tmpC1;_tmpA2;});}case 5U: _LLD: _tmpD2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).tvars;_tmpD1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).effect;_tmpD0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ret_tqual;_tmpCF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ret_type;_tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).args;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).c_varargs;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).cyc_varargs;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).rgn_po;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).attributes;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).requires_clause;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).requires_relns;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ensures_clause;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ensures_relns;_LLE: {
# 513
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpD2);
void*effopt2=_tmpD1 == 0?0: Cyc_Tcutil_copy_type(_tmpD1);
void*_tmpA4=Cyc_Tcutil_copy_type(_tmpCF);
struct Cyc_List_List*_tmpA5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpCE);
int _tmpA6=_tmpCD;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpCC != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpCC;
({struct Cyc_Absyn_VarargInfo*_tmpA60=({struct Cyc_Absyn_VarargInfo*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->name=cv->name,_tmpA7->tq=cv->tq,({void*_tmpA5F=Cyc_Tcutil_copy_type(cv->type);_tmpA7->type=_tmpA5F;}),_tmpA7->inject=cv->inject;_tmpA7;});cyc_varargs2=_tmpA60;});}{
# 524
struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpCB);
struct Cyc_List_List*_tmpA9=_tmpCA;
struct Cyc_Absyn_Exp*_tmpAA=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC9);
struct Cyc_List_List*_tmpAB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC8);
struct Cyc_Absyn_Exp*_tmpAC=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC7);
struct Cyc_List_List*_tmpAD=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC6);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=5U,(_tmpAE->f1).tvars=_tmpA3,(_tmpAE->f1).effect=effopt2,(_tmpAE->f1).ret_tqual=_tmpD0,(_tmpAE->f1).ret_type=_tmpA4,(_tmpAE->f1).args=_tmpA5,(_tmpAE->f1).c_varargs=_tmpA6,(_tmpAE->f1).cyc_varargs=cyc_varargs2,(_tmpAE->f1).rgn_po=_tmpA8,(_tmpAE->f1).attributes=_tmpA9,(_tmpAE->f1).requires_clause=_tmpAA,(_tmpAE->f1).requires_relns=_tmpAB,(_tmpAE->f1).ensures_clause=_tmpAC,(_tmpAE->f1).ensures_relns=_tmpAD;_tmpAE;});};}case 6U: _LLF: _tmpD3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp99)->f1;_LL10:
# 533
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=6U,({struct Cyc_List_List*_tmpA61=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpD3);_tmpAF->f1=_tmpA61;});_tmpAF;});case 7U: _LL11: _tmpD5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp99)->f1;_tmpD4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp99)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=7U,_tmpB0->f1=_tmpD5,({struct Cyc_List_List*_tmpA62=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpD4);_tmpB0->f2=_tmpA62;});_tmpB0;});case 9U: _LL13: _tmpD6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp99)->f1;_LL14:
# 537
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=9U,_tmpB1->f1=_tmpD6;_tmpB1;});case 11U: _LL15: _tmpD7=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp99)->f1;_LL16:
# 540
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=11U,_tmpB2->f1=_tmpD7;_tmpB2;});case 8U: _LL17: _tmpDA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp99)->f1;_tmpD9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp99)->f2;_tmpD8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp99)->f3;_LL18:
# 542
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=8U,_tmpB3->f1=_tmpDA,({struct Cyc_List_List*_tmpA63=Cyc_Tcutil_copy_types(_tmpD9);_tmpB3->f2=_tmpA63;}),_tmpB3->f3=_tmpD8,_tmpB3->f4=0;_tmpB3;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp99)->f1)->r)){case 0U: _LL19: _tmpDB=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp99)->f1)->r)->f1;_LL1A: {
# 545
struct Cyc_List_List*_tmpB4=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDB->tvs);
return({union Cyc_Absyn_AggrInfo _tmpA64=Cyc_Absyn_UnknownAggr(_tmpDB->kind,_tmpDB->name,0);Cyc_Absyn_aggr_type(_tmpA64,_tmpB4);});}case 1U: _LL1B: _tmpDC=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp99)->f1)->r)->f1;_LL1C:
# 548
 return Cyc_Absyn_enum_type(_tmpDC->name,0);default: _LL1D: _tmpDD=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp99)->f1)->r)->f1;_LL1E: {
# 550
struct Cyc_List_List*_tmpB5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDD->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmpA65=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp9A3;_tmp9A3.name=_tmpDD->name,_tmp9A3.is_extensible=0;_tmp9A3;}));Cyc_Absyn_datatype_type(_tmpA65,_tmpB5);});}}}_LL0:;}
# 565 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpDE=d;struct _dyneither_ptr*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDE)->tag == 0U){_LL1: _tmpE0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDE)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->tag=0U,({struct Cyc_Absyn_Exp*_tmpA66=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpE0);_tmpDF->f1=_tmpA66;});_tmpDF;});}else{_LL3: _tmpE1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpDE)->f1;_LL4:
 return d;}_LL0:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 571
static struct _tuple18*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple18*e){
# 573
return({struct _tuple18*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct Cyc_List_List*_tmpA68=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpE2->f1=_tmpA68;}),({struct Cyc_Absyn_Exp*_tmpA67=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpE2->f2=_tmpA67;});_tmpE2;});}
# 576
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpE3=preserve_types;
{void*_tmpE4=e->r;void*_tmpE5=_tmpE4;int _tmp162;struct _dyneither_ptr _tmp161;void*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;struct _dyneither_ptr*_tmp15E;struct Cyc_Core_Opt*_tmp15D;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;int _tmp159;struct Cyc_Absyn_Exp*_tmp158;void**_tmp157;struct Cyc_Absyn_Exp*_tmp156;int _tmp155;int _tmp154;void*_tmp153;struct Cyc_Absyn_Enumfield*_tmp152;struct Cyc_Absyn_Enumdecl*_tmp151;struct Cyc_Absyn_Enumfield*_tmp150;struct Cyc_List_List*_tmp14F;struct Cyc_Absyn_Datatypedecl*_tmp14E;struct Cyc_Absyn_Datatypefield*_tmp14D;void*_tmp14C;struct Cyc_List_List*_tmp14B;struct _tuple2*_tmp14A;struct Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp148;struct Cyc_Absyn_Aggrdecl*_tmp147;struct Cyc_Absyn_Exp*_tmp146;void*_tmp145;int _tmp144;struct Cyc_Absyn_Vardecl*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp141;int _tmp140;struct Cyc_List_List*_tmp13F;struct _dyneither_ptr*_tmp13E;struct Cyc_Absyn_Tqual _tmp13D;void*_tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct _dyneither_ptr*_tmp136;int _tmp135;int _tmp134;struct Cyc_Absyn_Exp*_tmp133;struct _dyneither_ptr*_tmp132;int _tmp131;int _tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;void*_tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;void*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp127;void*_tmp126;struct Cyc_Absyn_Exp*_tmp125;int _tmp124;enum Cyc_Absyn_Coercion _tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_List_List*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;int _tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_VarargCallInfo*_tmp11B;int _tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;enum Cyc_Absyn_Incrementor _tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Core_Opt*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;enum Cyc_Absyn_Primop _tmp10B;struct Cyc_List_List*_tmp10A;struct _dyneither_ptr _tmp109;void*_tmp108;union Cyc_Absyn_Cnst _tmp107;switch(*((int*)_tmpE5)){case 0U: _LL1: _tmp107=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL2:
({struct Cyc_Absyn_Exp*_tmpA69=Cyc_Absyn_const_exp(_tmp107,e->loc);new_e=_tmpA69;});goto _LL0;case 1U: _LL3: _tmp108=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpA6A=Cyc_Absyn_varb_exp(_tmp108,e->loc);new_e=_tmpA6A;});goto _LL0;case 2U: _LL5: _tmp109=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_Absyn_pragma_exp(_tmp109,e->loc);new_e=_tmpA6B;});goto _LL0;case 3U: _LL7: _tmp10B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp10A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL8:
({struct Cyc_Absyn_Exp*_tmpA6E=({enum Cyc_Absyn_Primop _tmpA6D=_tmp10B;struct Cyc_List_List*_tmpA6C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE3,_tmp10A);Cyc_Absyn_primop_exp(_tmpA6D,_tmpA6C,e->loc);});new_e=_tmpA6E;});goto _LL0;case 4U: _LL9: _tmp10E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp10D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp10C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_LLA:
# 585
({struct Cyc_Absyn_Exp*_tmpA72=({struct Cyc_Absyn_Exp*_tmpA71=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp10E);struct Cyc_Core_Opt*_tmpA70=(unsigned int)_tmp10D?({struct Cyc_Core_Opt*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->v=(void*)_tmp10D->v;_tmpE6;}): 0;struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp10C);Cyc_Absyn_assignop_exp(_tmpA71,_tmpA70,_tmpA6F,e->loc);});new_e=_tmpA72;});
goto _LL0;case 5U: _LLB: _tmp110=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp10F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LLC:
({struct Cyc_Absyn_Exp*_tmpA75=({struct Cyc_Absyn_Exp*_tmpA74=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp110);enum Cyc_Absyn_Incrementor _tmpA73=_tmp10F;Cyc_Absyn_increment_exp(_tmpA74,_tmpA73,e->loc);});new_e=_tmpA75;});goto _LL0;case 6U: _LLD: _tmp113=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp112=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp111=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_LLE:
# 589
({struct Cyc_Absyn_Exp*_tmpA79=({struct Cyc_Absyn_Exp*_tmpA78=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp113);struct Cyc_Absyn_Exp*_tmpA77=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp112);struct Cyc_Absyn_Exp*_tmpA76=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp111);Cyc_Absyn_conditional_exp(_tmpA78,_tmpA77,_tmpA76,e->loc);});new_e=_tmpA79;});goto _LL0;case 7U: _LLF: _tmp115=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp114=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL10:
({struct Cyc_Absyn_Exp*_tmpA7C=({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp115);struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp114);Cyc_Absyn_and_exp(_tmpA7B,_tmpA7A,e->loc);});new_e=_tmpA7C;});goto _LL0;case 8U: _LL11: _tmp117=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp116=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL12:
({struct Cyc_Absyn_Exp*_tmpA7F=({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp117);struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp116);Cyc_Absyn_or_exp(_tmpA7E,_tmpA7D,e->loc);});new_e=_tmpA7F;});goto _LL0;case 9U: _LL13: _tmp119=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp118=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL14:
({struct Cyc_Absyn_Exp*_tmpA82=({struct Cyc_Absyn_Exp*_tmpA81=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp119);struct Cyc_Absyn_Exp*_tmpA80=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp118);Cyc_Absyn_seq_exp(_tmpA81,_tmpA80,e->loc);});new_e=_tmpA82;});goto _LL0;case 10U: _LL15: _tmp11D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp11C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp11B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_tmp11A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE5)->f4;_LL16:
# 594
{struct Cyc_Absyn_VarargCallInfo*_tmpE7=_tmp11B;int _tmpF3;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_VarargInfo*_tmpF1;if(_tmpE7 != 0){_LL56: _tmpF3=_tmpE7->num_varargs;_tmpF2=_tmpE7->injectors;_tmpF1=_tmpE7->vai;_LL57: {
# 596
struct Cyc_Absyn_VarargInfo*_tmpE8=_tmpF1;struct _dyneither_ptr*_tmpEF;struct Cyc_Absyn_Tqual _tmpEE;void*_tmpED;int _tmpEC;_LL5B: _tmpEF=_tmpE8->name;_tmpEE=_tmpE8->tq;_tmpED=_tmpE8->type;_tmpEC=_tmpE8->inject;_LL5C:;
({struct Cyc_Absyn_Exp*_tmpA89=({void*_tmpA88=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->tag=10U,({
struct Cyc_Absyn_Exp*_tmpA87=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp11D);_tmpEB->f1=_tmpA87;}),({struct Cyc_List_List*_tmpA86=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE3,_tmp11C);_tmpEB->f2=_tmpA86;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpA85=({struct Cyc_Absyn_VarargCallInfo*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->num_varargs=_tmpF3,_tmpEA->injectors=_tmpF2,({
struct Cyc_Absyn_VarargInfo*_tmpA84=({struct Cyc_Absyn_VarargInfo*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->name=_tmpEF,_tmpE9->tq=_tmpEE,({void*_tmpA83=Cyc_Tcutil_copy_type(_tmpED);_tmpE9->type=_tmpA83;}),_tmpE9->inject=_tmpEC;_tmpE9;});_tmpEA->vai=_tmpA84;});_tmpEA;});
# 599
_tmpEB->f3=_tmpA85;}),_tmpEB->f4=_tmp11A;_tmpEB;});
# 597
Cyc_Absyn_new_exp(_tmpA88,e->loc);});new_e=_tmpA89;});
# 602
goto _LL55;}}else{_LL58: _LL59:
# 604
({struct Cyc_Absyn_Exp*_tmpA8D=({void*_tmpA8C=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->tag=10U,({struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp11D);_tmpF0->f1=_tmpA8B;}),({struct Cyc_List_List*_tmpA8A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE3,_tmp11C);_tmpF0->f2=_tmpA8A;}),_tmpF0->f3=_tmp11B,_tmpF0->f4=_tmp11A;_tmpF0;});Cyc_Absyn_new_exp(_tmpA8C,e->loc);});new_e=_tmpA8D;});
goto _LL55;}_LL55:;}
# 607
goto _LL0;case 11U: _LL17: _tmp11F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp11E=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL18:
# 609
({struct Cyc_Absyn_Exp*_tmpA90=_tmp11E?({struct Cyc_Absyn_Exp*_tmpA8F=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp11F);Cyc_Absyn_rethrow_exp(_tmpA8F,e->loc);}):({struct Cyc_Absyn_Exp*_tmpA8E=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp11F);Cyc_Absyn_throw_exp(_tmpA8E,e->loc);});new_e=_tmpA90;});
goto _LL0;case 12U: _LL19: _tmp120=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL1A:
({struct Cyc_Absyn_Exp*_tmpA92=({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp120);Cyc_Absyn_noinstantiate_exp(_tmpA91,e->loc);});new_e=_tmpA92;});
goto _LL0;case 13U: _LL1B: _tmp122=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp121=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL1C:
# 614
({struct Cyc_Absyn_Exp*_tmpA95=({struct Cyc_Absyn_Exp*_tmpA94=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp122);struct Cyc_List_List*_tmpA93=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp121);Cyc_Absyn_instantiate_exp(_tmpA94,_tmpA93,e->loc);});new_e=_tmpA95;});
goto _LL0;case 14U: _LL1D: _tmp126=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp125=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp124=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_tmp123=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE5)->f4;_LL1E:
# 617
({struct Cyc_Absyn_Exp*_tmpA9A=({void*_tmpA99=Cyc_Tcutil_copy_type(_tmp126);struct Cyc_Absyn_Exp*_tmpA98=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp125);int _tmpA97=_tmp124;enum Cyc_Absyn_Coercion _tmpA96=_tmp123;Cyc_Absyn_cast_exp(_tmpA99,_tmpA98,_tmpA97,_tmpA96,e->loc);});new_e=_tmpA9A;});goto _LL0;case 15U: _LL1F: _tmp127=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL20:
({struct Cyc_Absyn_Exp*_tmpA9C=({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp127);Cyc_Absyn_address_exp(_tmpA9B,e->loc);});new_e=_tmpA9C;});goto _LL0;case 16U: _LL21: _tmp129=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp128=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL22: {
# 620
struct Cyc_Absyn_Exp*eo1=_tmp129;if(_tmp129 != 0)({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp129);eo1=_tmpA9D;});
({struct Cyc_Absyn_Exp*_tmpAA0=({struct Cyc_Absyn_Exp*_tmpA9F=eo1;struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp128);Cyc_Absyn_New_exp(_tmpA9F,_tmpA9E,e->loc);});new_e=_tmpAA0;});
goto _LL0;}case 17U: _LL23: _tmp12A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL24:
({struct Cyc_Absyn_Exp*_tmpAA2=({void*_tmpAA1=Cyc_Tcutil_copy_type(_tmp12A);Cyc_Absyn_sizeoftype_exp(_tmpAA1,e->loc);});new_e=_tmpAA2;});
goto _LL0;case 18U: _LL25: _tmp12B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL26:
({struct Cyc_Absyn_Exp*_tmpAA4=({struct Cyc_Absyn_Exp*_tmpAA3=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp12B);Cyc_Absyn_sizeofexp_exp(_tmpAA3,e->loc);});new_e=_tmpAA4;});goto _LL0;case 19U: _LL27: _tmp12D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp12C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL28:
# 627
({struct Cyc_Absyn_Exp*_tmpAA7=({void*_tmpAA6=Cyc_Tcutil_copy_type(_tmp12D);struct Cyc_List_List*_tmpAA5=_tmp12C;Cyc_Absyn_offsetof_exp(_tmpAA6,_tmpAA5,e->loc);});new_e=_tmpAA7;});goto _LL0;case 20U: _LL29: _tmp12E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL2A:
({struct Cyc_Absyn_Exp*_tmpAA9=({struct Cyc_Absyn_Exp*_tmpAA8=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp12E);Cyc_Absyn_deref_exp(_tmpAA8,e->loc);});new_e=_tmpAA9;});goto _LL0;case 41U: _LL2B: _tmp12F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL2C:
({struct Cyc_Absyn_Exp*_tmpAAB=({struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp12F);Cyc_Absyn_extension_exp(_tmpAAA,e->loc);});new_e=_tmpAAB;});goto _LL0;case 21U: _LL2D: _tmp133=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp132=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp131=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_tmp130=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE5)->f4;_LL2E:
# 631
({struct Cyc_Absyn_Exp*_tmpAAE=({void*_tmpAAD=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=21U,({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp133);_tmpF4->f1=_tmpAAC;}),_tmpF4->f2=_tmp132,_tmpF4->f3=_tmp131,_tmpF4->f4=_tmp130;_tmpF4;});Cyc_Absyn_new_exp(_tmpAAD,e->loc);});new_e=_tmpAAE;});goto _LL0;case 22U: _LL2F: _tmp137=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp136=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp135=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_tmp134=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE5)->f4;_LL30:
# 633
({struct Cyc_Absyn_Exp*_tmpAB1=({void*_tmpAB0=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=22U,({struct Cyc_Absyn_Exp*_tmpAAF=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp137);_tmpF5->f1=_tmpAAF;}),_tmpF5->f2=_tmp136,_tmpF5->f3=_tmp135,_tmpF5->f4=_tmp134;_tmpF5;});Cyc_Absyn_new_exp(_tmpAB0,e->loc);});new_e=_tmpAB1;});goto _LL0;case 23U: _LL31: _tmp139=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp138=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL32:
({struct Cyc_Absyn_Exp*_tmpAB4=({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp139);struct Cyc_Absyn_Exp*_tmpAB2=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp138);Cyc_Absyn_subscript_exp(_tmpAB3,_tmpAB2,e->loc);});new_e=_tmpAB4;});
goto _LL0;case 24U: _LL33: _tmp13A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL34:
({struct Cyc_Absyn_Exp*_tmpAB6=({struct Cyc_List_List*_tmpAB5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE3,_tmp13A);Cyc_Absyn_tuple_exp(_tmpAB5,e->loc);});new_e=_tmpAB6;});goto _LL0;case 25U: _LL35: _tmp13E=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE5)->f1)->f1;_tmp13D=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE5)->f1)->f2;_tmp13C=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE5)->f1)->f3;_tmp13B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL36: {
# 638
struct _dyneither_ptr*vopt1=_tmp13E;
if(_tmp13E != 0)vopt1=_tmp13E;
({struct Cyc_Absyn_Exp*_tmpABB=({void*_tmpABA=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->tag=25U,({struct _tuple10*_tmpAB9=({struct _tuple10*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->f1=vopt1,_tmpF6->f2=_tmp13D,({void*_tmpAB8=Cyc_Tcutil_copy_type(_tmp13C);_tmpF6->f3=_tmpAB8;});_tmpF6;});_tmpF7->f1=_tmpAB9;}),({
struct Cyc_List_List*_tmpAB7=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE3,_tmp13B);_tmpF7->f2=_tmpAB7;});_tmpF7;});
# 640
Cyc_Absyn_new_exp(_tmpABA,e->loc);});new_e=_tmpABB;});
# 642
goto _LL0;}case 26U: _LL37: _tmp13F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL38:
({struct Cyc_Absyn_Exp*_tmpABE=({void*_tmpABD=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->tag=26U,({struct Cyc_List_List*_tmpABC=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE3,_tmp13F);_tmpF8->f1=_tmpABC;});_tmpF8;});Cyc_Absyn_new_exp(_tmpABD,e->loc);});new_e=_tmpABE;});
goto _LL0;case 27U: _LL39: _tmp143=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp142=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp141=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_tmp140=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE5)->f4;_LL3A:
# 646
({struct Cyc_Absyn_Exp*_tmpAC2=({void*_tmpAC1=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->tag=27U,_tmpF9->f1=_tmp143,({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp142);_tmpF9->f2=_tmpAC0;}),({struct Cyc_Absyn_Exp*_tmpABF=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp141);_tmpF9->f3=_tmpABF;}),_tmpF9->f4=_tmp140;_tmpF9;});Cyc_Absyn_new_exp(_tmpAC1,e->loc);});new_e=_tmpAC2;});
goto _LL0;case 28U: _LL3B: _tmp146=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp145=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp144=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_LL3C:
# 649
({struct Cyc_Absyn_Exp*_tmpAC6=({void*_tmpAC5=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=28U,({struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp146);_tmpFA->f1=_tmpAC4;}),({void*_tmpAC3=Cyc_Tcutil_copy_type(_tmp145);_tmpFA->f2=_tmpAC3;}),_tmpFA->f3=_tmp144;_tmpFA;});Cyc_Absyn_new_exp(_tmpAC5,_tmp146->loc);});new_e=_tmpAC6;});
# 651
goto _LL0;case 29U: _LL3D: _tmp14A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp149=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp148=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_tmp147=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE5)->f4;_LL3E:
# 653
({struct Cyc_Absyn_Exp*_tmpACA=({void*_tmpAC9=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->tag=29U,_tmpFB->f1=_tmp14A,({struct Cyc_List_List*_tmpAC8=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp149);_tmpFB->f2=_tmpAC8;}),({struct Cyc_List_List*_tmpAC7=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE3,_tmp148);_tmpFB->f3=_tmpAC7;}),_tmpFB->f4=_tmp147;_tmpFB;});Cyc_Absyn_new_exp(_tmpAC9,e->loc);});new_e=_tmpACA;});
# 655
goto _LL0;case 30U: _LL3F: _tmp14C=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp14B=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL40:
# 657
({struct Cyc_Absyn_Exp*_tmpACE=({void*_tmpACD=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=30U,({void*_tmpACC=Cyc_Tcutil_copy_type(_tmp14C);_tmpFC->f1=_tmpACC;}),({struct Cyc_List_List*_tmpACB=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE3,_tmp14B);_tmpFC->f2=_tmpACB;});_tmpFC;});Cyc_Absyn_new_exp(_tmpACD,e->loc);});new_e=_tmpACE;});
goto _LL0;case 31U: _LL41: _tmp14F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp14E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_tmp14D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE5)->f3;_LL42:
# 660
({struct Cyc_Absyn_Exp*_tmpAD1=({void*_tmpAD0=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->tag=31U,({struct Cyc_List_List*_tmpACF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE3,_tmp14F);_tmpFD->f1=_tmpACF;}),_tmpFD->f2=_tmp14E,_tmpFD->f3=_tmp14D;_tmpFD;});Cyc_Absyn_new_exp(_tmpAD0,e->loc);});new_e=_tmpAD1;});
goto _LL0;case 32U: _LL43: _tmp151=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp150=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL44:
 new_e=e;goto _LL0;case 33U: _LL45: _tmp153=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp152=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL46:
# 664
({struct Cyc_Absyn_Exp*_tmpAD4=({void*_tmpAD3=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=33U,({void*_tmpAD2=Cyc_Tcutil_copy_type(_tmp153);_tmpFE->f1=_tmpAD2;}),_tmpFE->f2=_tmp152;_tmpFE;});Cyc_Absyn_new_exp(_tmpAD3,e->loc);});new_e=_tmpAD4;});
goto _LL0;case 34U: _LL47: _tmp159=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE5)->f1).is_calloc;_tmp158=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE5)->f1).rgn;_tmp157=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE5)->f1).elt_type;_tmp156=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE5)->f1).num_elts;_tmp155=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE5)->f1).fat_result;_tmp154=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE5)->f1).inline_call;_LL48: {
# 667
struct Cyc_Absyn_Exp*_tmpFF=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp158;if(_tmp158 != 0)({struct Cyc_Absyn_Exp*_tmpAD5=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp158);r1=_tmpAD5;});{
void**t1=_tmp157;if(_tmp157 != 0)({void**_tmpAD7=({void**_tmp100=_cycalloc(sizeof(*_tmp100));({void*_tmpAD6=Cyc_Tcutil_copy_type(*_tmp157);*_tmp100=_tmpAD6;});_tmp100;});t1=_tmpAD7;});
({void*_tmpAD8=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=34U,(_tmp101->f1).is_calloc=_tmp159,(_tmp101->f1).rgn=r1,(_tmp101->f1).elt_type=t1,(_tmp101->f1).num_elts=_tmp156,(_tmp101->f1).fat_result=_tmp155,(_tmp101->f1).inline_call=_tmp154;_tmp101;});_tmpFF->r=_tmpAD8;});
new_e=_tmpFF;
goto _LL0;};}case 35U: _LL49: _tmp15B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp15A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL4A:
({struct Cyc_Absyn_Exp*_tmpADB=({struct Cyc_Absyn_Exp*_tmpADA=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp15B);struct Cyc_Absyn_Exp*_tmpAD9=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp15A);Cyc_Absyn_swap_exp(_tmpADA,_tmpAD9,e->loc);});new_e=_tmpADB;});goto _LL0;case 36U: _LL4B: _tmp15D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp15C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL4C: {
# 675
struct Cyc_Core_Opt*nopt1=_tmp15D;
if(_tmp15D != 0)({struct Cyc_Core_Opt*_tmpADC=({struct Cyc_Core_Opt*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->v=(struct _tuple2*)_tmp15D->v;_tmp102;});nopt1=_tmpADC;});
({struct Cyc_Absyn_Exp*_tmpADF=({void*_tmpADE=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=36U,_tmp103->f1=nopt1,({struct Cyc_List_List*_tmpADD=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE3,_tmp15C);_tmp103->f2=_tmpADD;});_tmp103;});Cyc_Absyn_new_exp(_tmpADE,e->loc);});new_e=_tmpADF;});
goto _LL0;}case 37U: _LL4D: _LL4E:
# 680
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpAE0=({const char*_tmp104="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp104,sizeof(char),45U);});_tmp105->f1=_tmpAE0;});_tmp105;}));case 38U: _LL4F: _tmp15F=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp15E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL50:
({struct Cyc_Absyn_Exp*_tmpAE3=({void*_tmpAE2=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=38U,({struct Cyc_Absyn_Exp*_tmpAE1=Cyc_Tcutil_deep_copy_exp(_tmpE3,_tmp15F);_tmp106->f1=_tmpAE1;}),_tmp106->f2=_tmp15E;_tmp106;});Cyc_Absyn_new_exp(_tmpAE2,e->loc);});new_e=_tmpAE3;});
goto _LL0;case 39U: _LL51: _tmp160=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_LL52:
({struct Cyc_Absyn_Exp*_tmpAE5=({void*_tmpAE4=Cyc_Tcutil_copy_type(_tmp160);Cyc_Absyn_valueof_exp(_tmpAE4,e->loc);});new_e=_tmpAE5;});
goto _LL0;default: _LL53: _tmp162=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE5)->f1;_tmp161=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE5)->f2;_LL54:
({struct Cyc_Absyn_Exp*_tmpAE6=Cyc_Absyn_asm_exp(_tmp162,_tmp161,e->loc);new_e=_tmpAE6;});goto _LL0;}_LL0:;}
# 688
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 692
return new_e;}
# 695
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 708 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp163=ka1;enum Cyc_Absyn_KindQual _tmp16C;enum Cyc_Absyn_AliasQual _tmp16B;_LL1: _tmp16C=_tmp163->kind;_tmp16B=_tmp163->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp164=ka2;enum Cyc_Absyn_KindQual _tmp16A;enum Cyc_Absyn_AliasQual _tmp169;_LL4: _tmp16A=_tmp164->kind;_tmp169=_tmp164->aliasqual;_LL5:;
# 712
if(_tmp16C != _tmp16A){
struct _tuple19 _tmp165=({struct _tuple19 _tmp9A4;_tmp9A4.f1=_tmp16C,_tmp9A4.f2=_tmp16A;_tmp9A4;});struct _tuple19 _tmp166=_tmp165;switch(_tmp166.f1){case Cyc_Absyn_BoxKind: switch(_tmp166.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp166.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 721
if(_tmp16B != _tmp169){
struct _tuple20 _tmp167=({struct _tuple20 _tmp9A5;_tmp9A5.f1=_tmp16B,_tmp9A5.f2=_tmp169;_tmp9A5;});struct _tuple20 _tmp168=_tmp167;switch(_tmp168.f1){case Cyc_Absyn_Aliasable: if(_tmp168.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp168.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 728
return 1;};}
# 731
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp16D=Cyc_Absyn_compress_kb(tv->kind);void*_tmp16E=_tmp16D;struct Cyc_Absyn_Kind*_tmp171;struct Cyc_Absyn_Kind*_tmp170;switch(*((int*)_tmp16E)){case 0U: _LL1: _tmp170=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E)->f1;_LL2:
 return _tmp170;case 2U: _LL3: _tmp171=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E)->f2;_LL4:
 return _tmp171;default: _LL5: _LL6:
# 736
({void*_tmpAE7=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=2U,_tmp16F->f1=0,_tmp16F->f2=def;_tmp16F;});tv->kind=_tmpAE7;});
return def;}_LL0:;}
# 741
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp172=({struct _tuple0 _tmp9A6;({void*_tmpAE9=Cyc_Absyn_compress_kb(kb1);_tmp9A6.f1=_tmpAE9;}),({void*_tmpAE8=Cyc_Absyn_compress_kb(kb2);_tmp9A6.f2=_tmpAE8;});_tmp9A6;});struct _tuple0 _tmp173=_tmp172;struct Cyc_Core_Opt**_tmp188;void*_tmp187;void*_tmp186;struct Cyc_Core_Opt**_tmp185;struct Cyc_Core_Opt**_tmp184;struct Cyc_Absyn_Kind*_tmp183;struct Cyc_Core_Opt**_tmp182;struct Cyc_Absyn_Kind*_tmp181;struct Cyc_Core_Opt**_tmp180;struct Cyc_Absyn_Kind*_tmp17F;struct Cyc_Absyn_Kind*_tmp17E;struct Cyc_Absyn_Kind*_tmp17D;struct Cyc_Core_Opt**_tmp17C;struct Cyc_Absyn_Kind*_tmp17B;struct Cyc_Absyn_Kind*_tmp17A;struct Cyc_Absyn_Kind*_tmp179;switch(*((int*)_tmp173.f1)){case 0U: switch(*((int*)_tmp173.f2)){case 0U: _LL1: _tmp17A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp173.f1)->f1;_tmp179=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp173.f2)->f1;_LL2:
 return _tmp17A == _tmp179;case 2U: _LL5: _tmp17D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp173.f1)->f1;_tmp17C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f2)->f1;_tmp17B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f2)->f2;_LL6:
# 750
 if(Cyc_Tcutil_kind_leq(_tmp17D,_tmp17B)){
({struct Cyc_Core_Opt*_tmpAEA=({struct Cyc_Core_Opt*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->v=kb1;_tmp175;});*_tmp17C=_tmpAEA;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp173.f2)){case 0U: _LL3: _tmp180=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f1)->f1;_tmp17F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f1)->f2;_tmp17E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp173.f2)->f1;_LL4:
# 745
 if(Cyc_Tcutil_kind_leq(_tmp17E,_tmp17F)){
({struct Cyc_Core_Opt*_tmpAEB=({struct Cyc_Core_Opt*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->v=kb2;_tmp174;});*_tmp180=_tmpAEB;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp184=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f1)->f1;_tmp183=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f1)->f2;_tmp182=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f2)->f1;_tmp181=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp173.f2)->f2;_LL8:
# 755
 if(Cyc_Tcutil_kind_leq(_tmp183,_tmp181)){
({struct Cyc_Core_Opt*_tmpAEC=({struct Cyc_Core_Opt*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->v=kb1;_tmp176;});*_tmp182=_tmpAEC;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp181,_tmp183)){
({struct Cyc_Core_Opt*_tmpAED=({struct Cyc_Core_Opt*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->v=kb2;_tmp177;});*_tmp184=_tmpAED;});
return 1;}else{
return 0;}}default: _LLB: _tmp186=_tmp173.f1;_tmp185=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp173.f2)->f1;_LLC:
# 764
({struct Cyc_Core_Opt*_tmpAEE=({struct Cyc_Core_Opt*_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->v=_tmp186;_tmp178;});*_tmp185=_tmpAEE;});
return 1;}default: _LL9: _tmp188=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp173.f1)->f1;_tmp187=_tmp173.f2;_LLA:
# 762
 _tmp186=_tmp187;_tmp185=_tmp188;goto _LLC;}_LL0:;}
# 769
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp189=Cyc_Tcutil_compress(t);void*_tmp18A=_tmp189;struct Cyc_Absyn_Tvar*_tmp18F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp18A)->tag == 2U){_LL1: _tmp18F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp18A)->f1;_LL2: {
# 772
void*_tmp18B=_tmp18F->kind;
_tmp18F->kind=kb;
return({struct _tuple15 _tmp9A7;_tmp9A7.f1=_tmp18F,_tmp9A7.f2=_tmp18B;_tmp9A7;});}}else{_LL3: _LL4:
# 776
({struct Cyc_String_pa_PrintArg_struct _tmp18E=({struct Cyc_String_pa_PrintArg_struct _tmp9A8;_tmp9A8.tag=0U,({struct _dyneither_ptr _tmpAEF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9A8.f1=_tmpAEF;});_tmp9A8;});void*_tmp18C[1U];_tmp18C[0]=& _tmp18E;({struct _dyneither_ptr _tmpAF0=({const char*_tmp18D="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp18D,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF0,_tag_dyneither(_tmp18C,sizeof(void*),1U));});});}_LL0:;}
# 782
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 784
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 788
struct _RegionHandle _tmp190=_new_region("temp");struct _RegionHandle*temp=& _tmp190;_push_region(temp);
{struct Cyc_List_List*_tmp191=0;
# 791
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp192=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp193=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp194=Cyc_Tcutil_tvar_kind(_tmp192,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp195=_tmp194;switch(((struct Cyc_Absyn_Kind*)_tmp195)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 797
({struct Cyc_List_List*_tmpAF2=({struct Cyc_List_List*_tmp197=_region_malloc(temp,sizeof(*_tmp197));({struct _tuple15*_tmpAF1=({struct _tuple15*_tmp196=_region_malloc(temp,sizeof(*_tmp196));_tmp196->f1=_tmp192,_tmp196->f2=_tmp193;_tmp196;});_tmp197->hd=_tmpAF1;}),_tmp197->tl=_tmp191;_tmp197;});_tmp191=_tmpAF2;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 801
if(_tmp191 != 0){
({void*_tmpAF5=({struct _RegionHandle*_tmpAF4=temp;struct Cyc_List_List*_tmpAF3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp191);Cyc_Tcutil_rsubstitute(_tmpAF4,_tmpAF3,field_type);});field_type=_tmpAF5;});
({struct Cyc_Absyn_Kind*_tmpAF6=Cyc_Tcutil_type_kind(field_type);k=_tmpAF6;});}}
# 789
;_pop_region(temp);}
# 806
return k;}
# 813
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 815
void*_tmp198=Cyc_Tcutil_compress(t);void*_tmp199=_tmp198;struct Cyc_Absyn_Typedefdecl*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_PtrInfo _tmp1B5;void*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B2;struct Cyc_Core_Opt*_tmp1B1;switch(*((int*)_tmp199)){case 1U: _LL1: _tmp1B1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp199)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B1))->v;case 2U: _LL3: _tmp1B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp199)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp1B2,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp1B4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f1;_tmp1B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2;_LL6: {
# 819
void*_tmp19A=_tmp1B4;enum Cyc_Absyn_AggrKind _tmp1A7;struct Cyc_List_List*_tmp1A6;struct Cyc_Absyn_AggrdeclImpl*_tmp1A5;int _tmp1A4;struct Cyc_Absyn_Kind*_tmp1A3;enum Cyc_Absyn_Size_of _tmp1A2;switch(*((int*)_tmp19A)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp1A2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp19A)->f2;_LL21:
 return(_tmp1A2 == Cyc_Absyn_Int_sz  || _tmp1A2 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp1A3=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp19A)->f2;_LL31:
 return _tmp1A3;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp19A)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 840
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 842
({void*_tmp19B=0U;({struct _dyneither_ptr _tmpAF7=({const char*_tmp19C="type_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp19C,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF7,_tag_dyneither(_tmp19B,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp19A)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 846
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp1A7=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp19A)->f1).KnownAggr).val)->kind;_tmp1A6=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp19A)->f1).KnownAggr).val)->tvs;_tmp1A5=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp19A)->f1).KnownAggr).val)->impl;_tmp1A4=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp19A)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 848
 if(_tmp1A5 == 0){
if(_tmp1A4)
return& Cyc_Tcutil_mk;else{
# 852
return& Cyc_Tcutil_ak;}}{
# 854
struct Cyc_List_List*_tmp19D=_tmp1A5->fields;
if(_tmp19D == 0)return& Cyc_Tcutil_mk;
# 857
if(_tmp1A7 == Cyc_Absyn_StructA){
for(0;((struct Cyc_List_List*)_check_null(_tmp19D))->tl != 0;_tmp19D=_tmp19D->tl){;}{
void*_tmp19E=((struct Cyc_Absyn_Aggrfield*)_tmp19D->hd)->type;
struct Cyc_Absyn_Kind*_tmp19F=Cyc_Tcutil_field_kind(_tmp19E,_tmp1B3,_tmp1A6);
if(_tmp19F == & Cyc_Tcutil_ak  || _tmp19F == & Cyc_Tcutil_tak)return _tmp19F;};}else{
# 865
for(0;_tmp19D != 0;_tmp19D=_tmp19D->tl){
void*_tmp1A0=((struct Cyc_Absyn_Aggrfield*)_tmp19D->hd)->type;
struct Cyc_Absyn_Kind*_tmp1A1=Cyc_Tcutil_field_kind(_tmp1A0,_tmp1B3,_tmp1A6);
if(_tmp1A1 == & Cyc_Tcutil_ak  || _tmp1A1 == & Cyc_Tcutil_tak)return _tmp1A1;}}
# 871
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 873
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp1B5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp199)->f1;_LLC: {
# 876
void*_tmp1A8=Cyc_Tcutil_compress((_tmp1B5.ptr_atts).bounds);void*_tmp1A9=_tmp1A8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f1)){case 13U: _LL4D: _LL4E: {
# 878
enum Cyc_Absyn_AliasQual _tmp1AA=(Cyc_Tcutil_type_kind((_tmp1B5.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1AB=_tmp1AA;switch(_tmp1AB){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;default: _LL58: _LL59:
 return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 884
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 886
enum Cyc_Absyn_AliasQual _tmp1AC=(Cyc_Tcutil_type_kind((_tmp1B5.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1AD=_tmp1AC;switch(_tmp1AD){case Cyc_Absyn_Aliasable: _LL5B: _LL5C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5D: _LL5E:
 return& Cyc_Tcutil_umk;default: _LL5F: _LL60:
 return& Cyc_Tcutil_tmk;}_LL5A:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 892
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 896
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp1B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp199)->f1).num_elts;_LL12:
# 898
 if(_tmp1B6 == 0  || Cyc_Tcutil_is_const_exp(_tmp1B6))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp1B7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp199)->f3;_LL16:
# 902
 if(_tmp1B7 == 0  || _tmp1B7->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B0=({struct Cyc_String_pa_PrintArg_struct _tmp9A9;_tmp9A9.tag=0U,({struct _dyneither_ptr _tmpAF8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9A9.f1=_tmpAF8;});_tmp9A9;});void*_tmp1AE[1U];_tmp1AE[0]=& _tmp1B0;({struct _dyneither_ptr _tmpAF9=({const char*_tmp1AF="type_kind: typedef found: %s";_tag_dyneither(_tmp1AF,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF9,_tag_dyneither(_tmp1AE,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B7->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp199)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 911
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 916
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1B8;_push_handler(& _tmp1B8);{int _tmp1BA=0;if(setjmp(_tmp1B8.handler))_tmp1BA=1;if(!_tmp1BA){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1BB=1;_npop_handler(0U);return _tmp1BB;};
# 918
;_pop_handler();}else{void*_tmp1B9=(void*)_exn_thrown;void*_tmp1BC=_tmp1B9;void*_tmp1BD;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1BC)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 920
 return 0;}else{_LL3: _tmp1BD=_tmp1BC;_LL4:(int)_rethrow(_tmp1BD);}_LL0:;}};}
# 925
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
({void*_tmpAFA=Cyc_Tcutil_compress(t);t=_tmpAFA;});{
void*_tmp1BE=t;struct Cyc_List_List*_tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*_tmp1DA;void*_tmp1D9;struct Cyc_Absyn_Tqual _tmp1D8;void*_tmp1D7;struct Cyc_List_List*_tmp1D6;int _tmp1D5;struct Cyc_Absyn_VarargInfo*_tmp1D4;struct Cyc_List_List*_tmp1D3;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_List_List*_tmp1CE;void*_tmp1CD;void*_tmp1CC;struct Cyc_Absyn_PtrInfo _tmp1CB;void*_tmp1CA;struct Cyc_Core_Opt**_tmp1C9;struct Cyc_Absyn_Tvar*_tmp1C8;switch(*((int*)_tmp1BE)){case 2U: _LL1: _tmp1C8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1BE)->f1;_LL2:
# 930
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1C8)){
({struct _dyneither_ptr _tmpAFB=({const char*_tmp1BF="(type variable would escape scope)";_tag_dyneither(_tmp1BF,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpAFB;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 934
goto _LL0;case 1U: _LL3: _tmp1CA=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BE)->f2;_tmp1C9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BE)->f4;_LL4:
# 936
 if(t == evar){
({struct _dyneither_ptr _tmpAFC=({const char*_tmp1C0="(occurs check)";_tag_dyneither(_tmp1C0,sizeof(char),15U);});Cyc_Tcutil_failure_reason=_tmpAFC;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 940
if(_tmp1CA != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1CA);else{
# 943
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C9))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 950
if(problem){
struct Cyc_List_List*_tmp1C1=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C9))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
({struct Cyc_List_List*_tmpAFD=({struct Cyc_List_List*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp1C2->tl=_tmp1C1;_tmp1C2;});_tmp1C1=_tmpAFD;});}}
# 956
({struct Cyc_Core_Opt*_tmpAFE=({struct Cyc_Core_Opt*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->v=_tmp1C1;_tmp1C3;});*_tmp1C9=_tmpAFE;});}}}
# 959
goto _LL0;case 3U: _LL5: _tmp1CB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BE)->f1;_LL6:
# 961
 Cyc_Tcutil_occurs(evar,r,env,_tmp1CB.elt_type);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1CB.ptr_atts).rgn);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1CB.ptr_atts).nullable);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1CB.ptr_atts).bounds);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1CB.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp1CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BE)->f1).elt_type;_tmp1CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BE)->f1).zero_term;_LL8:
# 969
 Cyc_Tcutil_occurs(evar,r,env,_tmp1CD);
Cyc_Tcutil_occurs(evar,r,env,_tmp1CC);
goto _LL0;case 5U: _LL9: _tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).tvars;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).effect;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).ret_tqual;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).ret_type;_tmp1D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).args;_tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).c_varargs;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).cyc_varargs;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).rgn_po;_tmp1D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).attributes;_tmp1D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).requires_clause;_tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).requires_relns;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).ensures_clause;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BE)->f1).ensures_relns;_LLA:
# 974
({struct Cyc_List_List*_tmpAFF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1DA,env);env=_tmpAFF;});
if(_tmp1D9 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D9);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D7);
for(0;_tmp1D6 != 0;_tmp1D6=_tmp1D6->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1D6->hd)).f3);}
if(_tmp1D4 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1D4->type);
for(0;_tmp1D3 != 0;_tmp1D3=_tmp1D3->tl){
struct _tuple0*_tmp1C4=(struct _tuple0*)_tmp1D3->hd;struct _tuple0*_tmp1C5=_tmp1C4;void*_tmp1C7;void*_tmp1C6;_LL16: _tmp1C7=_tmp1C5->f1;_tmp1C6=_tmp1C5->f2;_LL17:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1C7);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C6);}
# 986
goto _LL0;case 6U: _LLB: _tmp1DB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1BE)->f1;_LLC:
# 988
 for(0;_tmp1DB != 0;_tmp1DB=_tmp1DB->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1DB->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp1DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1BE)->f2;_LLE:
# 993
 for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1DC->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp1DD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1BE)->f2;_LL10:
 _tmp1DE=_tmp1DD;goto _LL12;case 0U: _LL11: _tmp1DE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f2;_LL12:
# 998
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1DE);goto _LL0;default: _LL13: _LL14:
# 1001
 goto _LL0;}_LL0:;};}
# 1004
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 1006
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 1011
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1019
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1DF=0U;({struct _dyneither_ptr _tmpB00=({const char*_tmp1E0="tq1 real_const not set.";_tag_dyneither(_tmp1E0,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB00,_tag_dyneither(_tmp1DF,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1E1=0U;({struct _dyneither_ptr _tmpB01=({const char*_tmp1E2="tq2 real_const not set.";_tag_dyneither(_tmp1E2,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB01,_tag_dyneither(_tmp1E1,sizeof(void*),0U));});});
# 1025
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 1028
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpB02=({const char*_tmp1E3="(qualifiers don't match)";_tag_dyneither(_tmp1E3,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB02;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1036
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 1040
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 1046
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1E4=({struct _tuple0 _tmp9AA;_tmp9AA.f1=e1->r,_tmp9AA.f2=e2->r;_tmp9AA;});struct _tuple0 _tmp1E5=_tmp1E4;void*_tmp1EF;void*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EC;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E5.f1)->tag == 14U){_LL1: _tmp1EC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E5.f1)->f2;_LL2:
# 1051
 Cyc_Tcutil_unify_cmp_exp(_tmp1EC,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E5.f2)->tag == 14U){_LL3: _tmp1ED=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E5.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1ED);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E5.f1)->tag == 39U){_LL5: _tmp1EE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E5.f1)->f1;_LL6: {
# 1054
void*_tmp1E6=Cyc_Tcutil_compress(_tmp1EE);void*_tmp1E7=_tmp1E6;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E7)->tag == 1U){_LLC: _LLD:
({void*_tmpB03=_tmp1EE;Cyc_Tcutil_unify_it(_tmpB03,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->tag=9U,_tmp1E8->f1=e2;_tmp1E8;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E5.f2)->tag == 39U){_LL7: _tmp1EF=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E5.f2)->f1;_LL8: {
# 1059
void*_tmp1E9=Cyc_Tcutil_compress(_tmp1EF);void*_tmp1EA=_tmp1E9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EA)->tag == 1U){_LL11: _LL12:
({void*_tmpB04=_tmp1EF;Cyc_Tcutil_unify_it(_tmpB04,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));_tmp1EB->tag=9U,_tmp1EB->f1=e1;_tmp1EB;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 1063
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 1067
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp1F0=att;switch(*((int*)_tmp1F0)){case 0U: _LL1: _LL2:
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
# 1094
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp1F1=({struct _tuple0 _tmp9AB;_tmp9AB.f1=att1,_tmp9AB.f2=att2;_tmp9AB;});struct _tuple0 _tmp1F2=_tmp1F1;enum Cyc_Absyn_Format_Type _tmp202;int _tmp201;int _tmp200;enum Cyc_Absyn_Format_Type _tmp1FF;int _tmp1FE;int _tmp1FD;struct _dyneither_ptr _tmp1FC;struct _dyneither_ptr _tmp1FB;struct Cyc_Absyn_Exp*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1F9;int _tmp1F8;int _tmp1F7;int _tmp1F6;int _tmp1F5;switch(*((int*)_tmp1F2.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1F2.f2)->tag == 0U){_LL1: _tmp1F6=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1F2.f1)->f1;_tmp1F5=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1F2.f2)->f1;_LL2:
 _tmp1F8=_tmp1F6;_tmp1F7=_tmp1F5;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1F2.f2)->tag == 20U){_LL3: _tmp1F8=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1F2.f1)->f1;_tmp1F7=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1F2.f2)->f1;_LL4:
# 1098
 return Cyc_Core_intcmp(_tmp1F8,_tmp1F7);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1F2.f2)->tag == 6U){_LL5: _tmp1FA=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1F2.f1)->f1;_tmp1F9=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1F2.f2)->f1;_LL6:
# 1100
 if(_tmp1FA == _tmp1F9)return 0;
if(_tmp1FA == 0)return - 1;
if(_tmp1F9 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp1FA,_tmp1F9);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1F2.f2)->tag == 8U){_LL7: _tmp1FC=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1F2.f1)->f1;_tmp1FB=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1F2.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1FC,(struct _dyneither_ptr)_tmp1FB);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F2.f2)->tag == 19U){_LL9: _tmp202=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F2.f1)->f1;_tmp201=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F2.f1)->f2;_tmp200=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F2.f1)->f3;_tmp1FF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F2.f2)->f1;_tmp1FE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F2.f2)->f2;_tmp1FD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F2.f2)->f3;_LLA: {
# 1106
int _tmp1F3=Cyc_Core_intcmp((int)((unsigned int)_tmp202),(int)((unsigned int)_tmp1FF));
if(_tmp1F3 != 0)return _tmp1F3;{
int _tmp1F4=Cyc_Core_intcmp(_tmp201,_tmp1FE);
if(_tmp1F4 != 0)return _tmp1F4;
return Cyc_Core_intcmp(_tmp200,_tmp1FD);};}}else{goto _LLB;}default: _LLB: _LLC:
# 1112
 return({int _tmpB05=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpB05,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
# 1116
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 1120
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 1129
static void*Cyc_Tcutil_rgns_of(void*t);
# 1131
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 1135
static struct _tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp203=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp204=_tmp203;switch(((struct Cyc_Absyn_Kind*)_tmp204)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp204)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
({void*_tmpB07=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->tag=9U,({struct Cyc_Absyn_Exp*_tmpB06=Cyc_Absyn_uint_exp(0U,0U);_tmp205->f1=_tmpB06;});_tmp205;});t=_tmpB07;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 1146
return({struct _tuple15*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->f1=tv,_tmp206->f2=t;_tmp206;});}
# 1153
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp207=Cyc_Tcutil_compress(t);void*_tmp208=_tmp207;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp21D;struct Cyc_List_List*_tmp21C;void*_tmp21B;struct Cyc_Absyn_Tqual _tmp21A;void*_tmp219;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_VarargInfo*_tmp217;struct Cyc_List_List*_tmp216;struct Cyc_List_List*_tmp215;void*_tmp214;void*_tmp213;void*_tmp212;struct Cyc_List_List*_tmp211;switch(*((int*)_tmp208)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp208)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp208)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp211=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp208)->f2;_LL6: {
# 1158
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp211);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 1162
struct Cyc_Absyn_Kind*_tmp209=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp20A=_tmp209;switch(((struct Cyc_Absyn_Kind*)_tmp20A)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp213=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp208)->f1).elt_type;_tmp212=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp208)->f1).ptr_atts).rgn;_LLC:
# 1170
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp20B[2U];({void*_tmpB09=Cyc_Absyn_access_eff(_tmp212);_tmp20B[0]=_tmpB09;}),({void*_tmpB08=Cyc_Tcutil_rgns_of(_tmp213);_tmp20B[1]=_tmpB08;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20B,sizeof(void*),2U));})));case 4U: _LLD: _tmp214=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp208)->f1).elt_type;_LLE:
# 1172
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp214));case 7U: _LLF: _tmp215=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp208)->f2;_LL10:
# 1174
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp215)));case 5U: _LL11: _tmp21C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208)->f1).tvars;_tmp21B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208)->f1).effect;_tmp21A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208)->f1).ret_tqual;_tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208)->f1).ret_type;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208)->f1).args;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208)->f1).cyc_varargs;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp208)->f1).rgn_po;_LL12: {
# 1183
void*_tmp20C=({struct Cyc_List_List*_tmpB0A=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp21C);Cyc_Tcutil_substitute(_tmpB0A,(void*)_check_null(_tmp21B));});
return Cyc_Tcutil_normalize_effect(_tmp20C);}case 6U: _LL13: _tmp21D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp208)->f1;_LL14: {
# 1186
struct Cyc_List_List*_tmp20D=0;
for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){
({struct Cyc_List_List*_tmpB0B=({struct Cyc_List_List*_tmp20E=_cycalloc(sizeof(*_tmp20E));_tmp20E->hd=(*((struct _tuple12*)_tmp21D->hd)).f2,_tmp20E->tl=_tmp20D;_tmp20E;});_tmp20D=_tmpB0B;});}
_tmp21E=_tmp20D;goto _LL16;}case 8U: _LL15: _tmp21E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp208)->f2;_LL16:
# 1191
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp21E)));case 10U: _LL17: _LL18:
({void*_tmp20F=0U;({struct _dyneither_ptr _tmpB0C=({const char*_tmp210="typedecl in rgns_of";_tag_dyneither(_tmp210,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB0C,_tag_dyneither(_tmp20F,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 1201
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpB0D=Cyc_Tcutil_compress(e);e=_tmpB0D;});{
void*_tmp21F=e;void*_tmp22B;struct Cyc_List_List**_tmp22A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f1)){case 9U: _LL1: _tmp22A=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2;_LL2: {
# 1205
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp22A;for(0;effs != 0;effs=effs->tl){
void*_tmp220=(void*)effs->hd;
({void*_tmpB0E=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp220));effs->hd=_tmpB0E;});{
void*_tmp221=(void*)effs->hd;void*_tmp222=_tmp221;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->tl == 0){_LLE: _LLF:
# 1214
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1218
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp22A;for(0;effs != 0;effs=effs->tl){
void*_tmp223=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp224=_tmp223;void*_tmp227;struct Cyc_List_List*_tmp226;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f1)){case 9U: _LL13: _tmp226=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f2;_LL14:
# 1223
({struct Cyc_List_List*_tmpB0F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp226,effects);effects=_tmpB0F;});
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp224)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp227=_tmp224;_LL1C:
({struct Cyc_List_List*_tmpB10=({struct Cyc_List_List*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->hd=_tmp227,_tmp225->tl=effects;_tmp225;});effects=_tmpB10;});goto _LL12;}_LL12:;}}
# 1231
({struct Cyc_List_List*_tmpB11=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp22A=_tmpB11;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2 != 0){_LL3: _tmp22B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2)->hd;_LL4: {
# 1234
void*_tmp228=Cyc_Tcutil_compress(_tmp22B);void*_tmp229=_tmp228;switch(*((int*)_tmp229)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp22B);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 1239
 return e;}_LL0:;};}
# 1244
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22C=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->tag=5U,(_tmp22D->f1).tvars=0,(_tmp22D->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmpB12=Cyc_Absyn_empty_tqual(0U);(_tmp22D->f1).ret_tqual=_tmpB12;}),(_tmp22D->f1).ret_type=Cyc_Absyn_void_type,(_tmp22D->f1).args=0,(_tmp22D->f1).c_varargs=0,(_tmp22D->f1).cyc_varargs=0,(_tmp22D->f1).rgn_po=0,(_tmp22D->f1).attributes=0,(_tmp22D->f1).requires_clause=0,(_tmp22D->f1).requires_relns=0,(_tmp22D->f1).ensures_clause=0,(_tmp22D->f1).ensures_relns=0;_tmp22D;});
# 1254
return({void*_tmpB16=(void*)_tmp22C;void*_tmpB15=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmpB14=Cyc_Absyn_empty_tqual(0U);void*_tmpB13=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmpB16,_tmpB15,_tmpB14,_tmpB13,Cyc_Absyn_false_type);});}
# 1261
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpB17=Cyc_Tcutil_compress(r);r=_tmpB17;});
if((r == Cyc_Absyn_heap_rgn_type  || r == Cyc_Absyn_unique_rgn_type) || r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp22E=Cyc_Tcutil_compress(e);void*_tmp22F=_tmp22E;struct Cyc_Core_Opt*_tmp24A;void**_tmp249;struct Cyc_Core_Opt*_tmp248;void*_tmp247;struct Cyc_List_List*_tmp246;void*_tmp245;switch(*((int*)_tmp22F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22F)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22F)->f2 != 0){_LL1: _tmp245=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22F)->f2)->hd;_LL2:
# 1268
 if(constrain)return Cyc_Tcutil_unify(r,_tmp245);
({void*_tmpB18=Cyc_Tcutil_compress(_tmp245);_tmp245=_tmpB18;});
if(r == _tmp245)return 1;{
struct _tuple0 _tmp230=({struct _tuple0 _tmp9AC;_tmp9AC.f1=r,_tmp9AC.f2=_tmp245;_tmp9AC;});struct _tuple0 _tmp231=_tmp230;struct Cyc_Absyn_Tvar*_tmp233;struct Cyc_Absyn_Tvar*_tmp232;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f2)->tag == 2U){_LLC: _tmp233=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f1)->f1;_tmp232=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp233,_tmp232)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp246=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22F)->f2;_LL4:
# 1276
 for(0;_tmp246 != 0;_tmp246=_tmp246->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp246->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22F)->f2 != 0){_LL5: _tmp247=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22F)->f2)->hd;_LL6: {
# 1280
void*_tmp234=Cyc_Tcutil_rgns_of(_tmp247);void*_tmp235=_tmp234;void*_tmp23F;void*_tmp23E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp235)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp235)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp235)->f2 != 0){_LL11: _tmp23E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp235)->f2)->hd;_LL12:
# 1282
 if(!constrain)return 0;{
void*_tmp236=Cyc_Tcutil_compress(_tmp23E);void*_tmp237=_tmp236;struct Cyc_Core_Opt*_tmp23D;void**_tmp23C;struct Cyc_Core_Opt*_tmp23B;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->tag == 1U){_LL16: _tmp23D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->f1;_tmp23C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->f2;_tmp23B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp237)->f4;_LL17: {
# 1287
void*_tmp238=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp23B);
# 1290
Cyc_Tcutil_occurs(_tmp238,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23B))->v,r);{
void*_tmp239=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp23A[2U];_tmp23A[0]=_tmp238,({void*_tmpB19=Cyc_Absyn_access_eff(r);_tmp23A[1]=_tmpB19;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp23A,sizeof(void*),2U));})));
*_tmp23C=_tmp239;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp23F=_tmp235;_LL14:
# 1296
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp23F);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp24A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22F)->f1;_tmp249=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22F)->f2;_tmp248=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22F)->f4;_LL8:
# 1299
 if(_tmp24A == 0  || ((struct Cyc_Absyn_Kind*)_tmp24A->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp240=0U;({struct _dyneither_ptr _tmpB1A=({const char*_tmp241="effect evar has wrong kind";_tag_dyneither(_tmp241,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB1A,_tag_dyneither(_tmp240,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1304
void*_tmp242=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp248);
# 1307
Cyc_Tcutil_occurs(_tmp242,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp248))->v,r);{
void*_tmp243=Cyc_Absyn_join_eff(({void*_tmp244[2U];_tmp244[0]=_tmp242,({void*_tmpB1B=Cyc_Absyn_access_eff(r);_tmp244[1]=_tmpB1B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp244,sizeof(void*),2U));}));
*_tmp249=_tmp243;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1318
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpB1C=Cyc_Tcutil_compress(t);t=_tmpB1C;});{
void*_tmp24B=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp24C=_tmp24B;struct Cyc_Core_Opt*_tmp25E;void**_tmp25D;struct Cyc_Core_Opt*_tmp25C;void*_tmp25B;struct Cyc_List_List*_tmp25A;switch(*((int*)_tmp24C)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp25A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f2;_LL4:
# 1323
 for(0;_tmp25A != 0;_tmp25A=_tmp25A->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp25A->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f2 != 0){_LL5: _tmp25B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f2)->hd;_LL6:
# 1328
({void*_tmpB1D=Cyc_Tcutil_compress(_tmp25B);_tmp25B=_tmpB1D;});
if(t == _tmp25B)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp25B);{
void*_tmp24D=Cyc_Tcutil_rgns_of(t);void*_tmp24E=_tmp24D;void*_tmp254;void*_tmp253;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E)->f2 != 0){_LLC: _tmp253=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E)->f2)->hd;_LLD: {
# 1333
struct _tuple0 _tmp24F=({struct _tuple0 _tmp9AD;({void*_tmpB1E=Cyc_Tcutil_compress(_tmp253);_tmp9AD.f1=_tmpB1E;}),_tmp9AD.f2=_tmp25B;_tmp9AD;});struct _tuple0 _tmp250=_tmp24F;struct Cyc_Absyn_Tvar*_tmp252;struct Cyc_Absyn_Tvar*_tmp251;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp250.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp250.f2)->tag == 2U){_LL11: _tmp252=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp250.f1)->f1;_tmp251=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp250.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp25B);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp253 == _tmp25B;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp254=_tmp24E;_LLF:
# 1337
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp254);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp25E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24C)->f1;_tmp25D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24C)->f2;_tmp25C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24C)->f4;_LL8:
# 1340
 if(_tmp25E == 0  || ((struct Cyc_Absyn_Kind*)_tmp25E->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp255=0U;({struct _dyneither_ptr _tmpB1F=({const char*_tmp256="effect evar has wrong kind";_tag_dyneither(_tmp256,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB1F,_tag_dyneither(_tmp255,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1345
void*_tmp257=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp25C);
# 1348
Cyc_Tcutil_occurs(_tmp257,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp25C))->v,t);{
void*_tmp258=Cyc_Absyn_join_eff(({void*_tmp259[2U];_tmp259[0]=_tmp257,({void*_tmpB20=Cyc_Absyn_regionsof_eff(t);_tmp259[1]=_tmpB20;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp259,sizeof(void*),2U));}));
*_tmp25D=_tmp258;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1359
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpB21=Cyc_Tcutil_compress(e);e=_tmpB21;});{
void*_tmp25F=e;struct Cyc_Core_Opt*_tmp276;void**_tmp275;struct Cyc_Core_Opt*_tmp274;void*_tmp273;struct Cyc_List_List*_tmp272;struct Cyc_Absyn_Tvar*_tmp271;switch(*((int*)_tmp25F)){case 2U: _LL1: _tmp271=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp25F)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp271)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f1)){case 9U: _LL3: _tmp272=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2;_LL4:
# 1364
 for(0;_tmp272 != 0;_tmp272=_tmp272->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp272->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2 != 0){_LL5: _tmp273=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2)->hd;_LL6: {
# 1369
void*_tmp260=Cyc_Tcutil_rgns_of(_tmp273);void*_tmp261=_tmp260;void*_tmp26B;void*_tmp26A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->f2 != 0){_LLC: _tmp26A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp261)->f2)->hd;_LLD:
# 1371
 if(!may_constrain_evars)return 0;{
void*_tmp262=Cyc_Tcutil_compress(_tmp26A);void*_tmp263=_tmp262;struct Cyc_Core_Opt*_tmp269;void**_tmp268;struct Cyc_Core_Opt*_tmp267;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp263)->tag == 1U){_LL11: _tmp269=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp263)->f1;_tmp268=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp263)->f2;_tmp267=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp263)->f4;_LL12: {
# 1377
void*_tmp264=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp267);
# 1379
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp267))->v,v))return 0;{
void*_tmp265=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp266[2U];_tmp266[0]=_tmp264,({void*_tmpB22=Cyc_Absyn_var_type(v);_tmp266[1]=_tmpB22;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp266,sizeof(void*),2U));})));
*_tmp268=_tmp265;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp26B=_tmp261;_LLF:
# 1385
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp26B);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp276=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25F)->f1;_tmp275=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25F)->f2;_tmp274=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25F)->f4;_LL8:
# 1388
 if(_tmp276 == 0  || ((struct Cyc_Absyn_Kind*)_tmp276->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp26C=0U;({struct _dyneither_ptr _tmpB23=({const char*_tmp26D="effect evar has wrong kind";_tag_dyneither(_tmp26D,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB23,_tag_dyneither(_tmp26C,sizeof(void*),0U));});});{
# 1392
void*_tmp26E=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp274);
# 1394
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp274))->v,v))
return 0;{
void*_tmp26F=Cyc_Absyn_join_eff(({void*_tmp270[2U];_tmp270[0]=_tmp26E,({void*_tmpB24=Cyc_Absyn_var_type(v);_tmp270[1]=_tmpB24;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp270,sizeof(void*),2U));}));
*_tmp275=_tmp26F;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1404
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpB25=Cyc_Tcutil_compress(e);e=_tmpB25;});{
void*_tmp277=e;void*_tmp27D;struct Cyc_List_List*_tmp27C;switch(*((int*)_tmp277)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->f1)){case 9U: _LL1: _tmp27C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->f2;_LL2:
# 1408
 for(0;_tmp27C != 0;_tmp27C=_tmp27C->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp27C->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->f2 != 0){_LL3: _tmp27D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->f2)->hd;_LL4: {
# 1413
void*_tmp278=Cyc_Tcutil_rgns_of(_tmp27D);void*_tmp279=_tmp278;void*_tmp27B;void*_tmp27A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp279)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp279)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp279)->f2 != 0){_LLA: _tmp27A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp279)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp27B=_tmp279;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp27B);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1417
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1431 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1436
void*_tmp27E=Cyc_Tcutil_compress(e1);void*_tmp27F=_tmp27E;void**_tmp28C;struct Cyc_Core_Opt*_tmp28B;struct Cyc_Absyn_Tvar*_tmp28A;void*_tmp289;void*_tmp288;struct Cyc_List_List*_tmp287;switch(*((int*)_tmp27F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f1)){case 9U: _LL1: _tmp287=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f2;_LL2:
# 1438
 for(0;_tmp287 != 0;_tmp287=_tmp287->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp287->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f2 != 0){_LL3: _tmp288=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f2)->hd;_LL4:
# 1449
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp288,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp288,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f2 != 0){_LL7: _tmp289=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f2)->hd;_LL8: {
# 1453
void*_tmp280=Cyc_Tcutil_rgns_of(_tmp289);void*_tmp281=_tmp280;void*_tmp283;void*_tmp282;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp281)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp281)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp281)->f2 != 0){_LLE: _tmp282=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp281)->f2)->hd;_LLF:
# 1458
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp282,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp282,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp283=_tmp281;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp283,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp28A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp27F)->f1;_LL6:
# 1451
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp28A,e2);case 1U: _LL9: _tmp28C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f2;_tmp28B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f4;_LLA:
# 1463
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1467
*_tmp28C=Cyc_Absyn_empty_effect;
# 1470
return 1;}else{
# 1472
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp9AE;_tmp9AE.tag=0U,({struct _dyneither_ptr _tmpB26=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp9AE.f1=_tmpB26;});_tmp9AE;});void*_tmp284[1U];_tmp284[0]=& _tmp286;({struct _dyneither_ptr _tmpB27=({const char*_tmp285="subset_effect: bad effect: %s";_tag_dyneither(_tmp285,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB27,_tag_dyneither(_tmp284,sizeof(void*),1U));});});}_LL0:;}
# 1488 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpB28=Cyc_Tcutil_normalize_effect(e1);e1=_tmpB28;});
({void*_tmpB29=Cyc_Tcutil_normalize_effect(e2);e2=_tmpB29;});
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1504
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1506
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp28D=(struct _tuple0*)r1->hd;struct _tuple0*_tmp28E=_tmp28D;void*_tmp294;void*_tmp293;_LL1: _tmp294=_tmp28E->f1;_tmp293=_tmp28E->f2;_LL2:;{
int found=_tmp294 == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp28F=(struct _tuple0*)r2->hd;struct _tuple0*_tmp290=_tmp28F;void*_tmp292;void*_tmp291;_LL4: _tmp292=_tmp290->f1;_tmp291=_tmp290->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp294,_tmp292) && Cyc_Tcutil_unify(_tmp293,_tmp291)){
found=1;
break;}}}
# 1516
if(!found)return 0;};}}
# 1518
return 1;}
# 1525
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp295=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp296=({struct Cyc_List_List*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->hd=_tmp295,_tmp297->tl=r1;_tmp297;});
if(Cyc_Relations_consistent_relations(_tmp296))return 0;}
# 1531
return 1;}
# 1536
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1542
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1544
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1547
int Cyc_Tcutil_tycon2int(void*t){
void*_tmp298=t;switch(*((int*)_tmp298)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1553
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1556
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1559
 return 7;default: _LL1B: _LL1C:
# 1562
 return 13;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1551
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1554
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1557
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1560
 return 8;default: _LL1D: _LL1E:
# 1563
 return 14;}default: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1552
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1555
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1558
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1561
 return 9;default: _LL1F: _LL20:
# 1564
 return 15;}}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp298)->f1){case 0U: _LL21: _LL22:
 return 16;case 1U: _LL23: _LL24:
 return 17;default: _LL25: _LL26:
 return 18;}case 3U: _LL27: _LL28:
 return 19;case 4U: _LL29: _LL2A:
 return 20;case 5U: _LL2B: _LL2C:
 return 21;case 6U: _LL2D: _LL2E:
 return 22;case 7U: _LL2F: _LL30:
 return 23;case 8U: _LL31: _LL32:
 return 24;case 9U: _LL33: _LL34:
 return 25;case 10U: _LL35: _LL36:
 return 26;case 11U: _LL37: _LL38:
 return 27;case 12U: _LL39: _LL3A:
 return 28;case 14U: _LL3B: _LL3C:
 return 29;case 13U: _LL3D: _LL3E:
 return 30;case 15U: _LL3F: _LL40:
 return 31;case 16U: _LL41: _LL42:
 return 32;case 17U: _LL43: _LL44:
 return 33;case 18U: _LL45: _LL46:
 return 34;case 19U: _LL47: _LL48:
 return 35;default: _LL49: _LL4A:
 return 36;}_LL0:;}
# 1589
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);
# 1591
struct _tuple2*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp299=i;struct _tuple2*_tmp29B;struct Cyc_Absyn_Datatypedecl*_tmp29A;if((_tmp299.KnownDatatype).tag == 2){_LL1: _tmp29A=*(_tmp299.KnownDatatype).val;_LL2:
 return _tmp29A->name;}else{_LL3: _tmp29B=((_tmp299.UnknownDatatype).val).name;_LL4:
 return _tmp29B;}_LL0:;}struct _tuple21{struct _tuple2*f1;struct _tuple2*f2;};
# 1598
struct _tuple21 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp29C=i;struct _tuple2*_tmp2A0;struct _tuple2*_tmp29F;struct Cyc_Absyn_Datatypedecl*_tmp29E;struct Cyc_Absyn_Datatypefield*_tmp29D;if((_tmp29C.KnownDatatypefield).tag == 2){_LL1: _tmp29E=((_tmp29C.KnownDatatypefield).val).f1;_tmp29D=((_tmp29C.KnownDatatypefield).val).f2;_LL2:
# 1601
 return({struct _tuple21 _tmp9AF;_tmp9AF.f1=_tmp29E->name,_tmp9AF.f2=_tmp29D->name;_tmp9AF;});}else{_LL3: _tmp2A0=((_tmp29C.UnknownDatatypefield).val).datatype_name;_tmp29F=((_tmp29C.UnknownDatatypefield).val).field_name;_LL4:
# 1603
 return({struct _tuple21 _tmp9B0;_tmp9B0.f1=_tmp2A0,_tmp9B0.f2=_tmp29F;_tmp9B0;});}_LL0:;}struct _tuple22{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1607
struct _tuple22 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp2A1=i;struct Cyc_Absyn_Aggrdecl*_tmp2A4;enum Cyc_Absyn_AggrKind _tmp2A3;struct _tuple2*_tmp2A2;if((_tmp2A1.UnknownAggr).tag == 1){_LL1: _tmp2A3=((_tmp2A1.UnknownAggr).val).f1;_tmp2A2=((_tmp2A1.UnknownAggr).val).f2;_LL2:
 return({struct _tuple22 _tmp9B1;_tmp9B1.f1=_tmp2A3,_tmp9B1.f2=_tmp2A2;_tmp9B1;});}else{_LL3: _tmp2A4=*(_tmp2A1.KnownAggr).val;_LL4:
 return({struct _tuple22 _tmp9B2;_tmp9B2.f1=_tmp2A4->kind,_tmp9B2.f2=_tmp2A4->name;_tmp9B2;});}_LL0:;}
# 1614
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1620
struct _tuple0 _tmp2A5=({struct _tuple0 _tmp9B3;_tmp9B3.f1=t1,_tmp9B3.f2=t2;_tmp9B3;});struct _tuple0 _tmp2A6=_tmp2A5;union Cyc_Absyn_AggrInfo _tmp2C4;union Cyc_Absyn_AggrInfo _tmp2C3;union Cyc_Absyn_DatatypeFieldInfo _tmp2C2;union Cyc_Absyn_DatatypeFieldInfo _tmp2C1;union Cyc_Absyn_DatatypeInfo _tmp2C0;union Cyc_Absyn_DatatypeInfo _tmp2BF;struct Cyc_List_List*_tmp2BE;struct Cyc_List_List*_tmp2BD;struct _dyneither_ptr _tmp2BC;struct _dyneither_ptr _tmp2BB;struct _tuple2*_tmp2BA;struct _tuple2*_tmp2B9;switch(*((int*)_tmp2A6.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 15U){_LL1: _tmp2BA=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1;_tmp2B9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp2BA,_tmp2B9);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 17U){_LL3: _tmp2BC=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1;_tmp2BB=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1;_LL4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2BC,(struct _dyneither_ptr)_tmp2BB);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 16U){_LL5: _tmp2BE=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1;_tmp2BD=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1;_LL6:
# 1624
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp2BE,_tmp2BD);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 18U){_LL7: _tmp2C0=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1;_tmp2BF=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1;_LL8: {
# 1626
struct _tuple2*q1=Cyc_Tcutil_get_datatype_qvar(_tmp2C0);
struct _tuple2*q2=Cyc_Tcutil_get_datatype_qvar(_tmp2BF);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 19U){_LL9: _tmp2C2=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1;_tmp2C1=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1;_LLA: {
# 1630
struct _tuple21 _tmp2A7=Cyc_Tcutil_get_datatype_field_qvars(_tmp2C2);struct _tuple21 _tmp2A8=_tmp2A7;struct _tuple2*_tmp2AF;struct _tuple2*_tmp2AE;_LL10: _tmp2AF=_tmp2A8.f1;_tmp2AE=_tmp2A8.f2;_LL11:;{
struct _tuple21 _tmp2A9=Cyc_Tcutil_get_datatype_field_qvars(_tmp2C1);struct _tuple21 _tmp2AA=_tmp2A9;struct _tuple2*_tmp2AD;struct _tuple2*_tmp2AC;_LL13: _tmp2AD=_tmp2AA.f1;_tmp2AC=_tmp2AA.f2;_LL14:;{
int _tmp2AB=Cyc_Absyn_qvar_cmp(_tmp2AF,_tmp2AD);
if(_tmp2AB != 0)return _tmp2AB;
return Cyc_Absyn_qvar_cmp(_tmp2AE,_tmp2AC);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 20U){_LLB: _tmp2C4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1;_tmp2C3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1;_LLC: {
# 1636
struct _tuple22 _tmp2B0=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C4);struct _tuple22 _tmp2B1=_tmp2B0;enum Cyc_Absyn_AggrKind _tmp2B8;struct _tuple2*_tmp2B7;_LL16: _tmp2B8=_tmp2B1.f1;_tmp2B7=_tmp2B1.f2;_LL17:;{
struct _tuple22 _tmp2B2=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C3);struct _tuple22 _tmp2B3=_tmp2B2;enum Cyc_Absyn_AggrKind _tmp2B6;struct _tuple2*_tmp2B5;_LL19: _tmp2B6=_tmp2B3.f1;_tmp2B5=_tmp2B3.f2;_LL1A:;{
int _tmp2B4=Cyc_Absyn_qvar_cmp(_tmp2B7,_tmp2B5);
if(_tmp2B4 != 0)return _tmp2B4;
return(int)_tmp2B8 - (int)_tmp2B6;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}struct _tuple23{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1646
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB2A=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpB2A;});
({void*_tmpB2B=Cyc_Tcutil_compress(t1);t1=_tmpB2B;});
({void*_tmpB2C=Cyc_Tcutil_compress(t2);t2=_tmpB2C;});
if(t1 == t2)return;
{void*_tmp2C5=t1;struct Cyc_Core_Opt*_tmp2D3;void**_tmp2D2;struct Cyc_Core_Opt*_tmp2D1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->tag == 1U){_LL1: _tmp2D3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f1;_tmp2D2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f2;_tmp2D1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f4;_LL2:
# 1657
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2D1))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2C6=Cyc_Tcutil_type_kind(t2);
# 1662
if(Cyc_Tcutil_kind_leq(_tmp2C6,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2D3))->v)){
*_tmp2D2=t2;
return;}else{
# 1666
{void*_tmp2C7=t2;struct Cyc_Absyn_PtrInfo _tmp2CF;void**_tmp2CE;struct Cyc_Core_Opt*_tmp2CD;switch(*((int*)_tmp2C7)){case 1U: _LL6: _tmp2CE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C7)->f2;_tmp2CD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C7)->f4;_LL7: {
# 1669
struct Cyc_List_List*_tmp2C8=(struct Cyc_List_List*)_tmp2D1->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CD))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2C8,(struct Cyc_Absyn_Tvar*)s2->hd)){
({struct _dyneither_ptr _tmpB2D=({const char*_tmp2C9="(type variable would escape scope)";_tag_dyneither(_tmp2C9,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpB2D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1676
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2D3->v,_tmp2C6)){
*_tmp2CE=t1;return;}
# 1679
({struct _dyneither_ptr _tmpB2E=({const char*_tmp2CA="(kinds are incompatible)";_tag_dyneither(_tmp2CA,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB2E;});
goto _LL5;}case 3U: _LL8: _tmp2CF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7)->f1;if(((struct Cyc_Absyn_Kind*)_tmp2D3->v)->kind == Cyc_Absyn_BoxKind){_LL9: {
# 1685
void*_tmp2CB=Cyc_Tcutil_compress((_tmp2CF.ptr_atts).bounds);
{void*_tmp2CC=_tmp2CB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CC)->tag == 1U){_LLD: _LLE:
# 1688
({void*_tmpB2F=_tmp2CB;Cyc_Tcutil_unify(_tmpB2F,Cyc_Absyn_bounds_one());});
*_tmp2D2=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1693
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1696
({struct _dyneither_ptr _tmpB30=({const char*_tmp2D0="(kinds are incompatible)";_tag_dyneither(_tmp2D0,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB30;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1699
 goto _LL0;}_LL0:;}
# 1704
{struct _tuple0 _tmp2D4=({struct _tuple0 _tmp9B9;_tmp9B9.f1=t2,_tmp9B9.f2=t1;_tmp9B9;});struct _tuple0 _tmp2D5=_tmp2D4;struct Cyc_List_List*_tmp363;struct Cyc_Absyn_Typedefdecl*_tmp362;struct Cyc_List_List*_tmp361;struct Cyc_Absyn_Typedefdecl*_tmp360;enum Cyc_Absyn_AggrKind _tmp35F;struct Cyc_List_List*_tmp35E;enum Cyc_Absyn_AggrKind _tmp35D;struct Cyc_List_List*_tmp35C;struct Cyc_List_List*_tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;void*_tmp358;struct Cyc_Absyn_Tqual _tmp357;void*_tmp356;struct Cyc_List_List*_tmp355;int _tmp354;struct Cyc_Absyn_VarargInfo*_tmp353;struct Cyc_List_List*_tmp352;struct Cyc_List_List*_tmp351;struct Cyc_Absyn_Exp*_tmp350;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_List_List*_tmp34C;void*_tmp34B;struct Cyc_Absyn_Tqual _tmp34A;void*_tmp349;struct Cyc_List_List*_tmp348;int _tmp347;struct Cyc_Absyn_VarargInfo*_tmp346;struct Cyc_List_List*_tmp345;struct Cyc_List_List*_tmp344;struct Cyc_Absyn_Exp*_tmp343;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_List_List*_tmp340;void*_tmp33F;struct Cyc_Absyn_Tqual _tmp33E;struct Cyc_Absyn_Exp*_tmp33D;void*_tmp33C;void*_tmp33B;struct Cyc_Absyn_Tqual _tmp33A;struct Cyc_Absyn_Exp*_tmp339;void*_tmp338;struct Cyc_Absyn_Exp*_tmp337;struct Cyc_Absyn_Exp*_tmp336;void*_tmp335;struct Cyc_Absyn_Tqual _tmp334;void*_tmp333;void*_tmp332;void*_tmp331;void*_tmp330;void*_tmp32F;struct Cyc_Absyn_Tqual _tmp32E;void*_tmp32D;void*_tmp32C;void*_tmp32B;void*_tmp32A;struct Cyc_Absyn_Tvar*_tmp329;struct Cyc_Absyn_Tvar*_tmp328;void*_tmp327;struct Cyc_List_List*_tmp326;void*_tmp325;struct Cyc_List_List*_tmp324;switch(*((int*)_tmp2D5.f1)){case 1U: _LL12: _LL13:
# 1707
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f1)->f1)->tag == 9U){_LL14: _LL15:
# 1710
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmp327=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f1)->f1;_tmp326=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f1)->f2;_tmp325=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->f1;_tmp324=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->f2;_LL21:
# 1721
 if(Cyc_Tcutil_tycon_cmp(_tmp327,_tmp325)== 0){
Cyc_Tcutil_unify_list(_tmp326,_tmp324);
return;}else{
# 1725
({struct _dyneither_ptr _tmpB31=({const char*_tmp2D7="(different type constructors)";_tag_dyneither(_tmp2D7,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpB31;});}
goto _LL11;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f1)->f1)){case 8U: _LL18: _LL19:
# 1712
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 1714
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5.f2)->f1)){case 9U: _LL16: _LL17:
# 1711
 goto _LL19;case 8U: _LL1A: _LL1B:
# 1713
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 1716
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
({struct _dyneither_ptr _tmpB32=({const char*_tmp2D6="(effects don't unify)";_tag_dyneither(_tmp2D6,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpB32;});
goto _LL11;default: switch(*((int*)_tmp2D5.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp2D5.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 2U){_LL22: _tmp329=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D5.f1)->f1;_tmp328=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D5.f2)->f1;_LL23: {
# 1729
struct _dyneither_ptr*_tmp2D8=_tmp329->name;
struct _dyneither_ptr*_tmp2D9=_tmp328->name;
# 1732
int _tmp2DA=_tmp329->identity;
int _tmp2DB=_tmp328->identity;
if(_tmp2DB == _tmp2DA)return;
({struct _dyneither_ptr _tmpB33=({const char*_tmp2DC="(variable types are not the same)";_tag_dyneither(_tmp2DC,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpB33;});
goto _LL11;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 3U){_LL24: _tmp335=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f1)->f1).elt_type;_tmp334=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f1)->f1).elt_tq;_tmp333=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ptr_atts).rgn;_tmp332=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ptr_atts).nullable;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ptr_atts).bounds;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ptr_atts).zero_term;_tmp32F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f2)->f1).elt_type;_tmp32E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f2)->f1).elt_tq;_tmp32D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ptr_atts).rgn;_tmp32C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ptr_atts).nullable;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ptr_atts).bounds;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ptr_atts).zero_term;_LL25:
# 1740
 Cyc_Tcutil_unify_it(_tmp32F,_tmp335);
Cyc_Tcutil_unify_it(_tmp333,_tmp32D);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
struct _dyneither_ptr _tmp2DD=Cyc_Tcutil_failure_reason;
({struct _dyneither_ptr _tmpB34=({const char*_tmp2DE="(not both zero terminated)";_tag_dyneither(_tmp2DE,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpB34;});
Cyc_Tcutil_unify_it(_tmp32A,_tmp330);
Cyc_Tcutil_unify_tqual(_tmp32E,_tmp32F,_tmp334,_tmp335);
({struct _dyneither_ptr _tmpB35=({const char*_tmp2DF="(different pointer bounds)";_tag_dyneither(_tmp2DF,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpB35;});
Cyc_Tcutil_unify_it(_tmp32B,_tmp331);{
# 1751
void*_tmp2E0=Cyc_Tcutil_compress(_tmp32B);void*_tmp2E1=_tmp2E0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E1)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E1)->f1)->tag == 14U){_LL35: _LL36:
# 1753
 Cyc_Tcutil_failure_reason=_tmp2DD;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 1756
({struct _dyneither_ptr _tmpB36=({const char*_tmp2E2="(incompatible pointer types)";_tag_dyneither(_tmp2E2,sizeof(char),29U);});Cyc_Tcutil_failure_reason=_tmpB36;});
Cyc_Tcutil_unify_it(_tmp32C,_tmp332);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 9U){_LL26: _tmp337=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D5.f1)->f1;_tmp336=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D5.f2)->f1;_LL27:
# 1762
 if(!Cyc_Evexp_same_const_exp(_tmp337,_tmp336)){
({struct _dyneither_ptr _tmpB37=({const char*_tmp2E3="(cannot prove expressions are the same)";_tag_dyneither(_tmp2E3,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpB37;});
goto _LL11;}
# 1766
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 4U){_LL28: _tmp33F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f1)->f1).elt_type;_tmp33E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f1)->f1).tq;_tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f1)->f1).num_elts;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f1)->f1).zero_term;_tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f2)->f1).elt_type;_tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f2)->f1).tq;_tmp339=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f2)->f1).num_elts;_tmp338=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D5.f2)->f1).zero_term;_LL29:
# 1770
 Cyc_Tcutil_unify_it(_tmp33B,_tmp33F);
Cyc_Tcutil_unify_tqual(_tmp33A,_tmp33B,_tmp33E,_tmp33F);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB38=({const char*_tmp2E4="(not both zero terminated)";_tag_dyneither(_tmp2E4,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpB38;});
Cyc_Tcutil_unify_it(_tmp33C,_tmp338);
if(_tmp33D == _tmp339)return;
if(_tmp33D == 0  || _tmp339 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp33D,_tmp339))
return;
({struct _dyneither_ptr _tmpB39=({const char*_tmp2E5="(different array sizes)";_tag_dyneither(_tmp2E5,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpB39;});
goto _LL11;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 5U){_LL2A: _tmp359=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).tvars;_tmp358=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).effect;_tmp357=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ret_tqual;_tmp356=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ret_type;_tmp355=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).args;_tmp354=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).c_varargs;_tmp353=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).cyc_varargs;_tmp352=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).rgn_po;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).attributes;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).requires_clause;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).requires_relns;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ensures_clause;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f1)->f1).ensures_relns;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).tvars;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).effect;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ret_tqual;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ret_type;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).args;_tmp347=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).c_varargs;_tmp346=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).cyc_varargs;_tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).rgn_po;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).attributes;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).requires_clause;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).requires_relns;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ensures_clause;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D5.f2)->f1).ensures_relns;_LL2B: {
# 1785
int done=0;
{struct _RegionHandle _tmp2E6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2E6;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp34C != 0){
if(_tmp359 == 0){
({struct _dyneither_ptr _tmpB3A=({const char*_tmp2E7="(second function type has too few type variables)";_tag_dyneither(_tmp2E7,sizeof(char),50U);});Cyc_Tcutil_failure_reason=_tmpB3A;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1793
void*_tmp2E8=((struct Cyc_Absyn_Tvar*)_tmp34C->hd)->kind;
void*_tmp2E9=((struct Cyc_Absyn_Tvar*)_tmp359->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2E8,_tmp2E9)){
({struct _dyneither_ptr _tmpB3F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EC=({struct Cyc_String_pa_PrintArg_struct _tmp9B6;_tmp9B6.tag=0U,({
struct _dyneither_ptr _tmpB3B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp34C->hd));_tmp9B6.f1=_tmpB3B;});_tmp9B6;});struct Cyc_String_pa_PrintArg_struct _tmp2ED=({struct Cyc_String_pa_PrintArg_struct _tmp9B5;_tmp9B5.tag=0U,({
struct _dyneither_ptr _tmpB3C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp34C->hd,& Cyc_Tcutil_bk)));_tmp9B5.f1=_tmpB3C;});_tmp9B5;});struct Cyc_String_pa_PrintArg_struct _tmp2EE=({struct Cyc_String_pa_PrintArg_struct _tmp9B4;_tmp9B4.tag=0U,({
struct _dyneither_ptr _tmpB3D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp359->hd,& Cyc_Tcutil_bk)));_tmp9B4.f1=_tmpB3D;});_tmp9B4;});void*_tmp2EA[3U];_tmp2EA[0]=& _tmp2EC,_tmp2EA[1]=& _tmp2ED,_tmp2EA[2]=& _tmp2EE;({struct _dyneither_ptr _tmpB3E=({const char*_tmp2EB="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2EB,sizeof(char),44U);});Cyc_aprintf(_tmpB3E,_tag_dyneither(_tmp2EA,sizeof(void*),3U));});});
# 1796
Cyc_Tcutil_failure_reason=_tmpB3F;});
# 1800
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1802
({struct Cyc_List_List*_tmpB42=({struct Cyc_List_List*_tmp2F0=_region_malloc(rgn,sizeof(*_tmp2F0));({struct _tuple15*_tmpB41=({struct _tuple15*_tmp2EF=_region_malloc(rgn,sizeof(*_tmp2EF));_tmp2EF->f1=(struct Cyc_Absyn_Tvar*)_tmp359->hd,({void*_tmpB40=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp34C->hd);_tmp2EF->f2=_tmpB40;});_tmp2EF;});_tmp2F0->hd=_tmpB41;}),_tmp2F0->tl=inst;_tmp2F0;});inst=_tmpB42;});
_tmp34C=_tmp34C->tl;
_tmp359=_tmp359->tl;};}
# 1806
if(_tmp359 != 0){
({struct _dyneither_ptr _tmpB43=({const char*_tmp2F1="(second function type has too many type variables)";_tag_dyneither(_tmp2F1,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpB43;});
_npop_handler(0U);goto _LL11;}
# 1810
if(inst != 0){
({void*_tmpB46=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));_tmp2F2->tag=5U,(_tmp2F2->f1).tvars=0,(_tmp2F2->f1).effect=_tmp34B,(_tmp2F2->f1).ret_tqual=_tmp34A,(_tmp2F2->f1).ret_type=_tmp349,(_tmp2F2->f1).args=_tmp348,(_tmp2F2->f1).c_varargs=_tmp347,(_tmp2F2->f1).cyc_varargs=_tmp346,(_tmp2F2->f1).rgn_po=_tmp345,(_tmp2F2->f1).attributes=_tmp344,(_tmp2F2->f1).requires_clause=_tmp350,(_tmp2F2->f1).requires_relns=_tmp34F,(_tmp2F2->f1).ensures_clause=_tmp34E,(_tmp2F2->f1).ensures_relns=_tmp34D;_tmp2F2;});Cyc_Tcutil_unify_it(_tmpB46,({
# 1814
struct _RegionHandle*_tmpB45=rgn;struct Cyc_List_List*_tmpB44=inst;Cyc_Tcutil_rsubstitute(_tmpB45,_tmpB44,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));
_tmp2F3->tag=5U,(_tmp2F3->f1).tvars=0,(_tmp2F3->f1).effect=_tmp358,(_tmp2F3->f1).ret_tqual=_tmp357,(_tmp2F3->f1).ret_type=_tmp356,(_tmp2F3->f1).args=_tmp355,(_tmp2F3->f1).c_varargs=_tmp354,(_tmp2F3->f1).cyc_varargs=_tmp353,(_tmp2F3->f1).rgn_po=_tmp352,(_tmp2F3->f1).attributes=_tmp351,(_tmp2F3->f1).requires_clause=_tmp343,(_tmp2F3->f1).requires_relns=_tmp342,(_tmp2F3->f1).ensures_clause=_tmp341,(_tmp2F3->f1).ensures_relns=_tmp340;_tmp2F3;}));}));});
# 1819
done=1;}}
# 1787
;_pop_region(rgn);}
# 1822
if(done)
return;
Cyc_Tcutil_unify_it(_tmp349,_tmp356);
Cyc_Tcutil_unify_tqual(_tmp34A,_tmp349,_tmp357,_tmp356);
for(0;_tmp348 != 0  && _tmp355 != 0;(_tmp348=_tmp348->tl,_tmp355=_tmp355->tl)){
struct _tuple10 _tmp2F4=*((struct _tuple10*)_tmp348->hd);struct _tuple10 _tmp2F5=_tmp2F4;struct Cyc_Absyn_Tqual _tmp2FB;void*_tmp2FA;_LL3A: _tmp2FB=_tmp2F5.f2;_tmp2FA=_tmp2F5.f3;_LL3B:;{
struct _tuple10 _tmp2F6=*((struct _tuple10*)_tmp355->hd);struct _tuple10 _tmp2F7=_tmp2F6;struct Cyc_Absyn_Tqual _tmp2F9;void*_tmp2F8;_LL3D: _tmp2F9=_tmp2F7.f2;_tmp2F8=_tmp2F7.f3;_LL3E:;
Cyc_Tcutil_unify_it(_tmp2FA,_tmp2F8);
Cyc_Tcutil_unify_tqual(_tmp2FB,_tmp2FA,_tmp2F9,_tmp2F8);};}
# 1832
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp348 != 0  || _tmp355 != 0){
({struct _dyneither_ptr _tmpB47=({const char*_tmp2FC="(function types have different number of arguments)";_tag_dyneither(_tmp2FC,sizeof(char),52U);});Cyc_Tcutil_failure_reason=_tmpB47;});
goto _LL11;}
# 1838
if(_tmp347 != _tmp354){
({struct _dyneither_ptr _tmpB48=({const char*_tmp2FD="(only one function type takes C varargs)";_tag_dyneither(_tmp2FD,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpB48;});
goto _LL11;}{
# 1843
int bad_cyc_vararg=0;
{struct _tuple23 _tmp2FE=({struct _tuple23 _tmp9B7;_tmp9B7.f1=_tmp346,_tmp9B7.f2=_tmp353;_tmp9B7;});struct _tuple23 _tmp2FF=_tmp2FE;struct _dyneither_ptr*_tmp309;struct Cyc_Absyn_Tqual _tmp308;void*_tmp307;int _tmp306;struct _dyneither_ptr*_tmp305;struct Cyc_Absyn_Tqual _tmp304;void*_tmp303;int _tmp302;if(_tmp2FF.f1 == 0){if(_tmp2FF.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmp2FF.f2 == 0){_LL44: _LL45:
# 1848
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpB49=({const char*_tmp300="(only one function type takes varargs)";_tag_dyneither(_tmp300,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpB49;});
goto _LL3F;}else{_LL46: _tmp309=(_tmp2FF.f1)->name;_tmp308=(_tmp2FF.f1)->tq;_tmp307=(_tmp2FF.f1)->type;_tmp306=(_tmp2FF.f1)->inject;_tmp305=(_tmp2FF.f2)->name;_tmp304=(_tmp2FF.f2)->tq;_tmp303=(_tmp2FF.f2)->type;_tmp302=(_tmp2FF.f2)->inject;_LL47:
# 1852
 Cyc_Tcutil_unify_it(_tmp307,_tmp303);
Cyc_Tcutil_unify_tqual(_tmp308,_tmp307,_tmp304,_tmp303);
if(_tmp306 != _tmp302){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpB4A=({const char*_tmp301="(only one function type injects varargs)";_tag_dyneither(_tmp301,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpB4A;});}
# 1858
goto _LL3F;}}_LL3F:;}
# 1860
if(bad_cyc_vararg)goto _LL11;{
# 1863
int bad_effect=0;
{struct _tuple0 _tmp30A=({struct _tuple0 _tmp9B8;_tmp9B8.f1=_tmp34B,_tmp9B8.f2=_tmp358;_tmp9B8;});struct _tuple0 _tmp30B=_tmp30A;if(_tmp30B.f1 == 0){if(_tmp30B.f2 == 0){_LL49: _LL4A:
 goto _LL48;}else{_LL4B: _LL4C:
 goto _LL4E;}}else{if(_tmp30B.f2 == 0){_LL4D: _LL4E:
 bad_effect=1;goto _LL48;}else{_LL4F: _LL50:
({int _tmpB4C=!({void*_tmpB4B=(void*)_check_null(_tmp34B);Cyc_Tcutil_unify_effect(_tmpB4B,(void*)_check_null(_tmp358));});bad_effect=_tmpB4C;});goto _LL48;}}_LL48:;}
# 1870
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpB4D=({const char*_tmp30C="(function effects do not match)";_tag_dyneither(_tmp30C,sizeof(char),32U);});Cyc_Tcutil_failure_reason=_tmpB4D;});
goto _LL11;}
# 1876
if(!Cyc_Tcutil_same_atts(_tmp351,_tmp344)){
({struct _dyneither_ptr _tmpB4E=({const char*_tmp30D="(function types have different attributes)";_tag_dyneither(_tmp30D,sizeof(char),43U);});Cyc_Tcutil_failure_reason=_tmpB4E;});
goto _LL11;}
# 1880
if(!Cyc_Tcutil_same_rgn_po(_tmp352,_tmp345)){
({struct _dyneither_ptr _tmpB4F=({const char*_tmp30E="(function types have different region lifetime orderings)";_tag_dyneither(_tmp30E,sizeof(char),58U);});Cyc_Tcutil_failure_reason=_tmpB4F;});
goto _LL11;}
# 1884
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34F,_tmp342)){
({struct _dyneither_ptr _tmpB50=({const char*_tmp30F="(@requires clauses not equivalent)";_tag_dyneither(_tmp30F,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpB50;});
goto _LL11;}
# 1888
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34D,_tmp340)){
({struct _dyneither_ptr _tmpB51=({const char*_tmp310="(@ensures clauses not equivalent)";_tag_dyneither(_tmp310,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpB51;});
goto _LL11;}
# 1892
return;};};}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 6U){_LL2C: _tmp35B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D5.f1)->f1;_tmp35A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D5.f2)->f1;_LL2D:
# 1895
 for(0;_tmp35A != 0  && _tmp35B != 0;(_tmp35A=_tmp35A->tl,_tmp35B=_tmp35B->tl)){
struct _tuple12 _tmp311=*((struct _tuple12*)_tmp35A->hd);struct _tuple12 _tmp312=_tmp311;struct Cyc_Absyn_Tqual _tmp318;void*_tmp317;_LL52: _tmp318=_tmp312.f1;_tmp317=_tmp312.f2;_LL53:;{
struct _tuple12 _tmp313=*((struct _tuple12*)_tmp35B->hd);struct _tuple12 _tmp314=_tmp313;struct Cyc_Absyn_Tqual _tmp316;void*_tmp315;_LL55: _tmp316=_tmp314.f1;_tmp315=_tmp314.f2;_LL56:;
Cyc_Tcutil_unify_it(_tmp317,_tmp315);
Cyc_Tcutil_unify_tqual(_tmp318,_tmp317,_tmp316,_tmp315);};}
# 1901
if(_tmp35A == 0  && _tmp35B == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB52=({const char*_tmp319="(tuple types have different numbers of components)";_tag_dyneither(_tmp319,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpB52;});
goto _LL11;}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 7U){_LL2E: _tmp35F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5.f1)->f1;_tmp35E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5.f1)->f2;_tmp35D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5.f2)->f1;_tmp35C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D5.f2)->f2;_LL2F:
# 1908
 if(_tmp35D != _tmp35F){({struct _dyneither_ptr _tmpB53=({const char*_tmp31A="(struct and union type)";_tag_dyneither(_tmp31A,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpB53;});goto _LL11;}
for(0;_tmp35C != 0  && _tmp35E != 0;(_tmp35C=_tmp35C->tl,_tmp35E=_tmp35E->tl)){
struct Cyc_Absyn_Aggrfield*_tmp31B=(struct Cyc_Absyn_Aggrfield*)_tmp35C->hd;
struct Cyc_Absyn_Aggrfield*_tmp31C=(struct Cyc_Absyn_Aggrfield*)_tmp35E->hd;
if(Cyc_strptrcmp(_tmp31B->name,_tmp31C->name)!= 0){
({struct _dyneither_ptr _tmpB54=({const char*_tmp31D="(different member names)";_tag_dyneither(_tmp31D,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB54;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1916
Cyc_Tcutil_unify_it(_tmp31B->type,_tmp31C->type);
Cyc_Tcutil_unify_tqual(_tmp31B->tq,_tmp31B->type,_tmp31C->tq,_tmp31C->type);
if(!Cyc_Tcutil_same_atts(_tmp31B->attributes,_tmp31C->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB55=({const char*_tmp31E="(different attributes on member)";_tag_dyneither(_tmp31E,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpB55;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1924
if((_tmp31B->width != 0  && _tmp31C->width == 0  || 
_tmp31C->width != 0  && _tmp31B->width == 0) || 
(_tmp31B->width != 0  && _tmp31C->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpB56=(struct Cyc_Absyn_Exp*)_check_null(_tmp31B->width);Cyc_Evexp_same_const_exp(_tmpB56,(struct Cyc_Absyn_Exp*)_check_null(_tmp31C->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB57=({const char*_tmp31F="(different bitfield widths on member)";_tag_dyneither(_tmp31F,sizeof(char),38U);});Cyc_Tcutil_failure_reason=_tmpB57;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1933
if((_tmp31B->requires_clause != 0  && _tmp31C->requires_clause == 0  || 
_tmp31B->requires_clause == 0  && _tmp31C->requires_clause != 0) || 
(_tmp31B->requires_clause == 0  && _tmp31C->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpB58=(struct Cyc_Absyn_Exp*)_check_null(_tmp31B->requires_clause);Cyc_Evexp_same_const_exp(_tmpB58,(struct Cyc_Absyn_Exp*)_check_null(_tmp31C->requires_clause));})){
# 1938
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB59=({const char*_tmp320="(different @requires clauses on member)";_tag_dyneither(_tmp320,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpB59;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1944
if(_tmp35C == 0  && _tmp35E == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB5A=({const char*_tmp321="(different number of members)";_tag_dyneither(_tmp321,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpB5A;});
goto _LL11;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5.f2)->tag == 8U){_LL30: _tmp363=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5.f1)->f2;_tmp362=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5.f1)->f3;_tmp361=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5.f2)->f2;_tmp360=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D5.f2)->f3;_LL31:
# 1950
 if(_tmp362 != _tmp360){
({struct _dyneither_ptr _tmpB5B=({const char*_tmp322="(different abstract typedefs)";_tag_dyneither(_tmp322,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpB5B;});
goto _LL11;}
# 1954
({struct _dyneither_ptr _tmpB5C=({const char*_tmp323="(type parameters to typedef differ)";_tag_dyneither(_tmp323,sizeof(char),36U);});Cyc_Tcutil_failure_reason=_tmpB5C;});
Cyc_Tcutil_unify_list(_tmp363,_tmp361);
return;}else{goto _LL32;}default: _LL32: _LL33:
 goto _LL11;}}}_LL11:;}
# 1959
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1962
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpB5E)(void*,void*)=cmp;void*_tmpB5D=(void*)_check_null(a1);_tmpB5E(_tmpB5D,(void*)_check_null(a2));});}
# 1969
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp364=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp365=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp364,_tmp365);}
# 1975
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp366=tqt1;struct Cyc_Absyn_Tqual _tmp36C;void*_tmp36B;_LL1: _tmp36C=_tmp366->f1;_tmp36B=_tmp366->f2;_LL2:;{
struct _tuple12*_tmp367=tqt2;struct Cyc_Absyn_Tqual _tmp36A;void*_tmp369;_LL4: _tmp36A=_tmp367->f1;_tmp369=_tmp367->f2;_LL5:;{
int _tmp368=Cyc_Tcutil_tqual_cmp(_tmp36C,_tmp36A);
if(_tmp368 != 0)return _tmp368;
return Cyc_Tcutil_typecmp(_tmp36B,_tmp369);};};}
# 1983
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp36D=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp36D != 0)return _tmp36D;{
int _tmp36E=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp36E != 0)return _tmp36E;{
int _tmp36F=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp36F != 0)return _tmp36F;{
int _tmp370=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp370 != 0)return _tmp370;
({int _tmpB5F=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp370=_tmpB5F;});
if(_tmp370 != 0)return _tmp370;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1997
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp371=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp371 != 0)return _tmp371;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 2003
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp372=t;void*_tmp373;switch(*((int*)_tmp372)){case 1U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 2;case 3U: _LL5: _LL6:
 return 3;case 4U: _LL7: _LL8:
 return 4;case 5U: _LL9: _LLA:
 return 5;case 6U: _LLB: _LLC:
 return 6;case 7U: _LLD: _LLE:
 return 7;case 8U: _LLF: _LL10:
 return 8;case 9U: _LL11: _LL12:
 return 9;case 10U: _LL13: _LL14:
 return 10;case 11U: _LL15: _LL16:
 return 11;default: _LL17: _tmp373=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp373);}_LL0:;}
# 2022
int Cyc_Tcutil_typecmp(void*t1,void*t2){
({void*_tmpB60=Cyc_Tcutil_compress(t1);t1=_tmpB60;});
({void*_tmpB61=Cyc_Tcutil_compress(t2);t2=_tmpB61;});
if(t1 == t2)return 0;{
int _tmp374=({int _tmpB62=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpB62,Cyc_Tcutil_type_case_number(t2));});
if(_tmp374 != 0)
return _tmp374;{
# 2031
struct _tuple0 _tmp375=({struct _tuple0 _tmp9BA;_tmp9BA.f1=t2,_tmp9BA.f2=t1;_tmp9BA;});struct _tuple0 _tmp376=_tmp375;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3BF;enum Cyc_Absyn_AggrKind _tmp3BE;struct Cyc_List_List*_tmp3BD;enum Cyc_Absyn_AggrKind _tmp3BC;struct Cyc_List_List*_tmp3BB;struct Cyc_List_List*_tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_FnInfo _tmp3B8;struct Cyc_Absyn_FnInfo _tmp3B7;void*_tmp3B6;struct Cyc_Absyn_Tqual _tmp3B5;struct Cyc_Absyn_Exp*_tmp3B4;void*_tmp3B3;void*_tmp3B2;struct Cyc_Absyn_Tqual _tmp3B1;struct Cyc_Absyn_Exp*_tmp3B0;void*_tmp3AF;void*_tmp3AE;struct Cyc_Absyn_Tqual _tmp3AD;void*_tmp3AC;void*_tmp3AB;void*_tmp3AA;void*_tmp3A9;void*_tmp3A8;struct Cyc_Absyn_Tqual _tmp3A7;void*_tmp3A6;void*_tmp3A5;void*_tmp3A4;void*_tmp3A3;struct Cyc_Absyn_Tvar*_tmp3A2;struct Cyc_Absyn_Tvar*_tmp3A1;void*_tmp3A0;struct Cyc_List_List*_tmp39F;void*_tmp39E;struct Cyc_List_List*_tmp39D;switch(*((int*)_tmp376.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->tag == 0U){_LL1: _tmp3A0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp39F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f1)->f2;_tmp39E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->f1;_tmp39D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->f2;_LL2: {
# 2033
int _tmp377=Cyc_Tcutil_tycon_cmp(_tmp3A0,_tmp39E);
if(_tmp377 != 0)return _tmp377;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp39F,_tmp39D);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp376.f2)->tag == 1U){_LL3: _LL4:
# 2037
({void*_tmp378=0U;({struct _dyneither_ptr _tmpB63=({const char*_tmp379="typecmp: can only compare closed types";_tag_dyneither(_tmp379,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB63,_tag_dyneither(_tmp378,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp376.f2)->tag == 2U){_LL5: _tmp3A2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp3A1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp376.f2)->f1;_LL6:
# 2041
 return Cyc_Core_intcmp(_tmp3A1->identity,_tmp3A2->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->tag == 3U){_LL7: _tmp3AE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).elt_type;_tmp3AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).elt_tq;_tmp3AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).rgn;_tmp3AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).nullable;_tmp3AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).bounds;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).zero_term;_tmp3A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).elt_type;_tmp3A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).elt_tq;_tmp3A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).rgn;_tmp3A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).nullable;_tmp3A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).bounds;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).zero_term;_LL8: {
# 2045
int _tmp37A=Cyc_Tcutil_typecmp(_tmp3A8,_tmp3AE);
if(_tmp37A != 0)return _tmp37A;{
int _tmp37B=Cyc_Tcutil_typecmp(_tmp3A6,_tmp3AC);
if(_tmp37B != 0)return _tmp37B;{
int _tmp37C=Cyc_Tcutil_tqual_cmp(_tmp3A7,_tmp3AD);
if(_tmp37C != 0)return _tmp37C;{
int _tmp37D=Cyc_Tcutil_typecmp(_tmp3A4,_tmp3AA);
if(_tmp37D != 0)return _tmp37D;{
int _tmp37E=Cyc_Tcutil_typecmp(_tmp3A3,_tmp3A9);
if(_tmp37E != 0)return _tmp37E;{
int _tmp37F=Cyc_Tcutil_typecmp(_tmp3A4,_tmp3AA);
if(_tmp37F != 0)return _tmp37F;
return Cyc_Tcutil_typecmp(_tmp3A5,_tmp3AB);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f2)->tag == 4U){_LL9: _tmp3B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f1)->f1).elt_type;_tmp3B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f1)->f1).tq;_tmp3B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f1)->f1).num_elts;_tmp3B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f1)->f1).zero_term;_tmp3B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f2)->f1).elt_type;_tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f2)->f1).tq;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f2)->f1).num_elts;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376.f2)->f1).zero_term;_LLA: {
# 2061
int _tmp380=Cyc_Tcutil_tqual_cmp(_tmp3B1,_tmp3B5);
if(_tmp380 != 0)return _tmp380;{
int _tmp381=Cyc_Tcutil_typecmp(_tmp3B2,_tmp3B6);
if(_tmp381 != 0)return _tmp381;{
int _tmp382=Cyc_Tcutil_typecmp(_tmp3B3,_tmp3AF);
if(_tmp382 != 0)return _tmp382;
if(_tmp3B4 == _tmp3B0)return 0;
if(_tmp3B4 == 0  || _tmp3B0 == 0)
({void*_tmp383=0U;({struct _dyneither_ptr _tmpB64=({const char*_tmp384="missing expression in array index";_tag_dyneither(_tmp384,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB64,_tag_dyneither(_tmp383,sizeof(void*),0U));});});
# 2071
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B4,_tmp3B0);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp376.f2)->tag == 5U){_LLB: _tmp3B8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp3B7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp376.f2)->f1;_LLC:
# 2074
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp3B8.ret_type,_tmp3B7.ret_type);
if(r != 0)return r;
({int _tmpB65=Cyc_Tcutil_tqual_cmp(_tmp3B8.ret_tqual,_tmp3B7.ret_tqual);r=_tmpB65;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp385=_tmp3B8.args;
struct Cyc_List_List*_tmp386=_tmp3B7.args;
for(0;_tmp385 != 0  && _tmp386 != 0;(_tmp385=_tmp385->tl,_tmp386=_tmp386->tl)){
struct _tuple10 _tmp387=*((struct _tuple10*)_tmp385->hd);struct _tuple10 _tmp388=_tmp387;struct Cyc_Absyn_Tqual _tmp38E;void*_tmp38D;_LL18: _tmp38E=_tmp388.f2;_tmp38D=_tmp388.f3;_LL19:;{
struct _tuple10 _tmp389=*((struct _tuple10*)_tmp386->hd);struct _tuple10 _tmp38A=_tmp389;struct Cyc_Absyn_Tqual _tmp38C;void*_tmp38B;_LL1B: _tmp38C=_tmp38A.f2;_tmp38B=_tmp38A.f3;_LL1C:;
({int _tmpB66=Cyc_Tcutil_tqual_cmp(_tmp38E,_tmp38C);r=_tmpB66;});
if(r != 0)return r;
({int _tmpB67=Cyc_Tcutil_typecmp(_tmp38D,_tmp38B);r=_tmpB67;});
if(r != 0)return r;};}
# 2089
if(_tmp385 != 0)return 1;
if(_tmp386 != 0)return - 1;
if(_tmp3B8.c_varargs  && !_tmp3B7.c_varargs)return 1;
if(!_tmp3B8.c_varargs  && _tmp3B7.c_varargs)return - 1;
if(_tmp3B8.cyc_varargs != 0 & _tmp3B7.cyc_varargs == 0)return 1;
if(_tmp3B8.cyc_varargs == 0 & _tmp3B7.cyc_varargs != 0)return - 1;
if(_tmp3B8.cyc_varargs != 0 & _tmp3B7.cyc_varargs != 0){
({int _tmpB69=({struct Cyc_Absyn_Tqual _tmpB68=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B8.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmpB68,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B7.cyc_varargs))->tq);});r=_tmpB69;});
if(r != 0)return r;
({int _tmpB6A=Cyc_Tcutil_typecmp((_tmp3B8.cyc_varargs)->type,(_tmp3B7.cyc_varargs)->type);r=_tmpB6A;});
if(r != 0)return r;
if((_tmp3B8.cyc_varargs)->inject  && !(_tmp3B7.cyc_varargs)->inject)return 1;
if(!(_tmp3B8.cyc_varargs)->inject  && (_tmp3B7.cyc_varargs)->inject)return - 1;}
# 2103
({int _tmpB6B=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp3B8.effect,_tmp3B7.effect);r=_tmpB6B;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp38F=_tmp3B8.rgn_po;
struct Cyc_List_List*_tmp390=_tmp3B7.rgn_po;
for(0;_tmp38F != 0  && _tmp390 != 0;(_tmp38F=_tmp38F->tl,_tmp390=_tmp390->tl)){
struct _tuple0 _tmp391=*((struct _tuple0*)_tmp38F->hd);struct _tuple0 _tmp392=_tmp391;void*_tmp398;void*_tmp397;_LL1E: _tmp398=_tmp392.f1;_tmp397=_tmp392.f2;_LL1F:;{
struct _tuple0 _tmp393=*((struct _tuple0*)_tmp390->hd);struct _tuple0 _tmp394=_tmp393;void*_tmp396;void*_tmp395;_LL21: _tmp396=_tmp394.f1;_tmp395=_tmp394.f2;_LL22:;
({int _tmpB6C=Cyc_Tcutil_typecmp(_tmp398,_tmp396);r=_tmpB6C;});if(r != 0)return r;
({int _tmpB6D=Cyc_Tcutil_typecmp(_tmp397,_tmp395);r=_tmpB6D;});if(r != 0)return r;};}
# 2113
if(_tmp38F != 0)return 1;
if(_tmp390 != 0)return - 1;
({int _tmpB6E=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B8.requires_clause,_tmp3B7.requires_clause);r=_tmpB6E;});
if(r != 0)return r;
({int _tmpB6F=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B8.ensures_clause,_tmp3B7.ensures_clause);r=_tmpB6F;});
if(r != 0)return r;
# 2121
({void*_tmp399=0U;({struct _dyneither_ptr _tmpB70=({const char*_tmp39A="typecmp: function type comparison should never get here!";_tag_dyneither(_tmp39A,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB70,_tag_dyneither(_tmp399,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp376.f2)->tag == 6U){_LLD: _tmp3BA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp3B9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp376.f2)->f1;_LLE:
# 2124
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3B9,_tmp3BA);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp376.f2)->tag == 7U){_LLF: _tmp3BE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp3BD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp376.f1)->f2;_tmp3BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp376.f2)->f1;_tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp376.f2)->f2;_LL10:
# 2127
 if(_tmp3BC != _tmp3BE){
if(_tmp3BC == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3BB,_tmp3BD);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp376.f2)->tag == 9U){_LL11: _tmp3C0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp3BF=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp376.f2)->f1;_LL12:
# 2132
 _tmp3C2=_tmp3C0;_tmp3C1=_tmp3BF;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp376.f2)->tag == 11U){_LL13: _tmp3C2=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp3C1=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp376.f2)->f1;_LL14:
# 2134
 return Cyc_Evexp_const_exp_cmp(_tmp3C2,_tmp3C1);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp39B=0U;({struct _dyneither_ptr _tmpB71=({const char*_tmp39C="Unmatched case in typecmp";_tag_dyneither(_tmp39C,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB71,_tag_dyneither(_tmp39B,sizeof(void*),0U));});});}_LL0:;};};}
# 2141
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
({void*_tmpB72=Cyc_Tcutil_compress(t1);t1=_tmpB72;});
({void*_tmpB73=Cyc_Tcutil_compress(t2);t2=_tmpB73;});{
struct _tuple0 _tmp3C3=({struct _tuple0 _tmp9BC;_tmp9BC.f1=t1,_tmp9BC.f2=t2;_tmp9BC;});struct _tuple0 _tmp3C4=_tmp3C3;void*_tmp3CA;void*_tmp3C9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C4.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C4.f2)->tag == 0U){_LL1: _tmp3CA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C4.f1)->f1;_tmp3C9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C4.f2)->f1;_LL2: {
# 2146
struct _tuple0 _tmp3C5=({struct _tuple0 _tmp9BB;_tmp9BB.f1=_tmp3CA,_tmp9BB.f2=_tmp3C9;_tmp9BB;});struct _tuple0 _tmp3C6=_tmp3C5;int _tmp3C8;int _tmp3C7;switch(*((int*)_tmp3C6.f1)){case 2U: switch(*((int*)_tmp3C6.f2)){case 2U: _LL6: _tmp3C8=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C6.f1)->f1;_tmp3C7=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f1;_LL7:
# 2148
 return _tmp3C7 < _tmp3C8;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp3C6.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 2155
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 2160
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 2163
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f1 == 0){_LL14: _LL15:
# 2158
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp3C6.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 2156
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 2161
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 2164
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f1 == 0){_LL16: _LL17:
# 2159
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 2165
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C6.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 2162
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 2166
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 2168
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2170
 return 0;}_LL0:;};}
# 2176
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2179
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
({struct Cyc_Core_Opt*_tmpB74=({struct Cyc_Core_Opt*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->v=t1;_tmp3CB;});max_arith_type=_tmpB74;});}}}
# 2188
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2192
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CE=({struct Cyc_String_pa_PrintArg_struct _tmp9BE;_tmp9BE.tag=0U,({
struct _dyneither_ptr _tmpB75=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9BE.f1=_tmpB75;});_tmp9BE;});struct Cyc_String_pa_PrintArg_struct _tmp3CF=({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0U,({struct _dyneither_ptr _tmpB76=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp9BD.f1=_tmpB76;});_tmp9BD;});void*_tmp3CC[2U];_tmp3CC[0]=& _tmp3CE,_tmp3CC[1]=& _tmp3CF;({unsigned int _tmpB78=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpB77=({const char*_tmp3CD="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp3CD,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpB78,_tmpB77,_tag_dyneither(_tmp3CC,sizeof(void*),2U));});});
return 0;}}}
# 2198
return 1;}
# 2203
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(te,e)){
void*_tmp3D0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3D1=_tmp3D0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D1)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2209
return 1;}
# 2213
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 2217
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp3D2=0U;({unsigned int _tmpB7A=e->loc;struct _dyneither_ptr _tmpB79=({const char*_tmp3D3="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D3,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpB7A,_tmpB79,_tag_dyneither(_tmp3D2,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2223
return 0;}
# 2227
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 2231
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp3D4=0U;({unsigned int _tmpB7C=e->loc;struct _dyneither_ptr _tmpB7B=({const char*_tmp3D5="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D5,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpB7C,_tmpB7B,_tag_dyneither(_tmp3D4,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2237
return 0;}
# 2242
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Tcutil_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 2248
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmpB7D=t;Cyc_Tcutil_unify(_tmpB7D,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 2253
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp3D6=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp3D7=_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D9;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D7)->tag == 9U){_LL1: _tmp3D9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D7)->f1;_LL2:
 return _tmp3D9;}else{_LL3: _LL4: {
# 2258
struct Cyc_Absyn_Exp*_tmp3D8=Cyc_Absyn_valueof_exp(_tmp3D6,0U);
_tmp3D8->topt=Cyc_Absyn_uint_type;
return _tmp3D8;}}_LL0:;}
# 2267
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Tcutil_unify(def,b);{
void*_tmp3DA=Cyc_Tcutil_compress(b);void*_tmp3DB=_tmp3DA;struct Cyc_List_List*_tmp3DF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp3DF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp3DF);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp3DE=({struct Cyc_String_pa_PrintArg_struct _tmp9BF;_tmp9BF.tag=0U,({struct _dyneither_ptr _tmpB7E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(b));_tmp9BF.f1=_tmpB7E;});_tmp9BF;});void*_tmp3DC[1U];_tmp3DC[0]=& _tmp3DE;({struct _dyneither_ptr _tmpB7F=({const char*_tmp3DD="get_bounds_exp: %s";_tag_dyneither(_tmp3DD,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB7F,_tag_dyneither(_tmp3DC,sizeof(void*),1U));});});}_LL0:;};}
# 2276
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp3E0=Cyc_Tcutil_compress(t);void*_tmp3E1=_tmp3E0;void*_tmp3E5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E1)->tag == 3U){_LL1: _tmp3E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E1)->f1).ptr_atts).bounds;_LL2:
# 2279
 return Cyc_Tcutil_get_bounds_exp(def,_tmp3E5);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp3E4=({struct Cyc_String_pa_PrintArg_struct _tmp9C0;_tmp9C0.tag=0U,({struct _dyneither_ptr _tmpB80=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9C0.f1=_tmpB80;});_tmp9C0;});void*_tmp3E2[1U];_tmp3E2[0]=& _tmp3E4;({struct _dyneither_ptr _tmpB81=({const char*_tmp3E3="get_ptr_bounds_exp not pointer: %s";_tag_dyneither(_tmp3E3,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB81,_tag_dyneither(_tmp3E2,sizeof(void*),1U));});});}_LL0:;}
# 2285
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({struct Cyc_List_List*_tmpB82=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E6->v=_tmpB82;});_tmp3E6;}));else{
# 2289
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->v=0;_tmp3E7;}));}}
# 2292
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));({struct Cyc_List_List*_tmpB83=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E8->v=_tmpB83;});_tmp3E8;}));else{
# 2296
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->v=0;_tmp3E9;}));}}
# 2300
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple24{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 2308
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2310
({void*_tmpB84=Cyc_Tcutil_compress(t1);t1=_tmpB84;});
({void*_tmpB85=Cyc_Tcutil_compress(t2);t2=_tmpB85;});{
struct _tuple0 _tmp3EA=({struct _tuple0 _tmp9C4;_tmp9C4.f1=t1,_tmp9C4.f2=t2;_tmp9C4;});struct _tuple0 _tmp3EB=_tmp3EA;void*_tmp400;struct Cyc_Absyn_Tqual _tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;void*_tmp3FD;void*_tmp3FC;struct Cyc_Absyn_Tqual _tmp3FB;struct Cyc_Absyn_Exp*_tmp3FA;void*_tmp3F9;struct Cyc_Absyn_PtrInfo _tmp3F8;struct Cyc_Absyn_PtrInfo _tmp3F7;switch(*((int*)_tmp3EB.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EB.f2)->tag == 3U){_LL1: _tmp3F8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EB.f1)->f1;_tmp3F7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EB.f2)->f1;_LL2: {
# 2314
int okay=1;
# 2316
if(!Cyc_Tcutil_unify((_tmp3F8.ptr_atts).nullable,(_tmp3F7.ptr_atts).nullable))
# 2318
({int _tmpB86=!Cyc_Tcutil_force_type2bool(0,(_tmp3F8.ptr_atts).nullable);okay=_tmpB86;});
# 2320
if(!Cyc_Tcutil_unify((_tmp3F8.ptr_atts).bounds,(_tmp3F7.ptr_atts).bounds)){
struct _tuple24 _tmp3EC=({struct _tuple24 _tmp9C1;({struct Cyc_Absyn_Exp*_tmpB8A=({void*_tmpB89=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB89,(_tmp3F8.ptr_atts).bounds);});_tmp9C1.f1=_tmpB8A;}),({
struct Cyc_Absyn_Exp*_tmpB88=({void*_tmpB87=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB87,(_tmp3F7.ptr_atts).bounds);});_tmp9C1.f2=_tmpB88;});_tmp9C1;});
# 2321
struct _tuple24 _tmp3ED=_tmp3EC;struct Cyc_Absyn_Exp*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F0;if(_tmp3ED.f2 == 0){_LLA: _LLB:
# 2324
 okay=1;goto _LL9;}else{if(_tmp3ED.f1 == 0){_LLC: _LLD:
# 2327
 if(Cyc_Tcutil_force_type2bool(0,(_tmp3F8.ptr_atts).zero_term) && ({
void*_tmpB8B=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpB8B,(_tmp3F7.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp3F1=_tmp3ED.f1;_tmp3F0=_tmp3ED.f2;_LLF:
# 2333
({int _tmpB8D=okay  && ({struct Cyc_Absyn_Exp*_tmpB8C=(struct Cyc_Absyn_Exp*)_check_null(_tmp3F0);Cyc_Evexp_lte_const_exp(_tmpB8C,(struct Cyc_Absyn_Exp*)_check_null(_tmp3F1));});okay=_tmpB8D;});
# 2337
if(!Cyc_Tcutil_force_type2bool(0,(_tmp3F7.ptr_atts).zero_term))
({void*_tmp3EE=0U;({unsigned int _tmpB8F=loc;struct _dyneither_ptr _tmpB8E=({const char*_tmp3EF="implicit cast to shorter array";_tag_dyneither(_tmp3EF,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpB8F,_tmpB8E,_tag_dyneither(_tmp3EE,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 2344
okay=okay  && (!(_tmp3F8.elt_tq).real_const  || (_tmp3F7.elt_tq).real_const);
# 2347
if(!Cyc_Tcutil_unify((_tmp3F8.ptr_atts).rgn,(_tmp3F7.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp3F8.ptr_atts).rgn,(_tmp3F7.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp3F4=({struct Cyc_String_pa_PrintArg_struct _tmp9C3;_tmp9C3.tag=0U,({
struct _dyneither_ptr _tmpB90=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F8.ptr_atts).rgn));_tmp9C3.f1=_tmpB90;});_tmp9C3;});struct Cyc_String_pa_PrintArg_struct _tmp3F5=({struct Cyc_String_pa_PrintArg_struct _tmp9C2;_tmp9C2.tag=0U,({
struct _dyneither_ptr _tmpB91=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F7.ptr_atts).rgn));_tmp9C2.f1=_tmpB91;});_tmp9C2;});void*_tmp3F2[2U];_tmp3F2[0]=& _tmp3F4,_tmp3F2[1]=& _tmp3F5;({unsigned int _tmpB93=loc;struct _dyneither_ptr _tmpB92=({const char*_tmp3F3="implicit cast from region %s to region %s";_tag_dyneither(_tmp3F3,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpB93,_tmpB92,_tag_dyneither(_tmp3F2,sizeof(void*),2U));});});}else{
okay=0;}}
# 2357
({int _tmpB94=okay  && (Cyc_Tcutil_unify((_tmp3F8.ptr_atts).zero_term,(_tmp3F7.ptr_atts).zero_term) || 
# 2359
Cyc_Tcutil_force_type2bool(1,(_tmp3F8.ptr_atts).zero_term) && (_tmp3F7.elt_tq).real_const);
# 2357
okay=_tmpB94;});{
# 2367
int _tmp3F6=
({void*_tmpB95=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpB95,(_tmp3F7.ptr_atts).bounds);}) && !
Cyc_Tcutil_force_type2bool(0,(_tmp3F7.ptr_atts).zero_term);
# 2373
({int _tmpB96=okay  && (Cyc_Tcutil_unify(_tmp3F8.elt_type,_tmp3F7.elt_type) || 
(_tmp3F6  && ((_tmp3F7.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3F7.elt_type)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp3F8.elt_type,_tmp3F7.elt_type));
# 2373
okay=_tmpB96;});
# 2376
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f2)->tag == 4U){_LL3: _tmp400=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f1)->f1).elt_type;_tmp3FF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f1)->f1).tq;_tmp3FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f1)->f1).num_elts;_tmp3FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f1)->f1).zero_term;_tmp3FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f2)->f1).elt_type;_tmp3FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f2)->f1).tq;_tmp3FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f2)->f1).num_elts;_tmp3F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3EB.f2)->f1).zero_term;_LL4: {
# 2380
int okay;
# 2383
({int _tmpB97=Cyc_Tcutil_unify(_tmp3FD,_tmp3F9) && (
(_tmp3FE != 0  && _tmp3FA != 0) && Cyc_Evexp_same_const_exp(_tmp3FE,_tmp3FA));
# 2383
okay=_tmpB97;});
# 2386
return(okay  && Cyc_Tcutil_unify(_tmp400,_tmp3FC)) && (!_tmp3FF.real_const  || _tmp3FB.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EB.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EB.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EB.f2)->f1)->tag == 1U){_LL5: _LL6:
# 2388
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 2390
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2394
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp401=Cyc_Tcutil_compress(t);void*_tmp402=_tmp401;void*_tmp405;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402)->tag == 3U){_LL1: _tmp405=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp402)->f1).elt_type;_LL2:
 return _tmp405;}else{_LL3: _LL4:
({void*_tmp403=0U;({struct _dyneither_ptr _tmpB98=({const char*_tmp404="pointer_elt_type";_tag_dyneither(_tmp404,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB98,_tag_dyneither(_tmp403,sizeof(void*),0U));});});}_LL0:;}
# 2400
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp406=Cyc_Tcutil_compress(t);void*_tmp407=_tmp406;struct Cyc_Absyn_PtrAtts*_tmp40A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp407)->tag == 3U){_LL1: _tmp40A=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp407)->f1).ptr_atts;_LL2:
 return _tmp40A->rgn;}else{_LL3: _LL4:
({void*_tmp408=0U;({struct _dyneither_ptr _tmpB99=({const char*_tmp409="pointer_elt_type";_tag_dyneither(_tmp409,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB99,_tag_dyneither(_tmp408,sizeof(void*),0U));});});}_LL0:;}
# 2407
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp40B=Cyc_Tcutil_compress(t);void*_tmp40C=_tmp40B;void*_tmp40D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->tag == 3U){_LL1: _tmp40D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).ptr_atts).rgn;_LL2:
# 2410
*rgn=_tmp40D;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2417
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp40E=e->r;void*_tmp40F=_tmp40E;void*_tmp413;struct Cyc_Absyn_Exp*_tmp412;struct _dyneither_ptr _tmp411;switch(*((int*)_tmp40F)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40F)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40F)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40F)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40F)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40F)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp411=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40F)->f1).Wchar_c).val;_LLC: {
# 2426
unsigned long _tmp410=Cyc_strlen((struct _dyneither_ptr)_tmp411);
int i=0;
if(_tmp410 >= 2  && *((const char*)_check_dyneither_subscript(_tmp411,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp411,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp411.curr)[1]== 'x'  && _tmp410 >= 3) && *((const char*)_check_dyneither_subscript(_tmp411,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp410;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp411,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2436
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2424
 return 1;case 14U: _LLD: _tmp413=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40F)->f1;_tmp412=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40F)->f2;_LLE:
# 2437
 return Cyc_Tcutil_is_zero(_tmp412) && Cyc_Tcutil_admits_zero(_tmp413);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2442
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 2451
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2456
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2461
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 2470
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2475
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2480
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp414=ka;enum Cyc_Absyn_KindQual _tmp41D;enum Cyc_Absyn_AliasQual _tmp41C;_LL1: _tmp41D=_tmp414->kind;_tmp41C=_tmp414->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp415=_tmp41C;switch(_tmp415){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2484
enum Cyc_Absyn_KindQual _tmp416=_tmp41D;switch(_tmp416){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL15: _LL16:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL17: _LL18:
 return& Cyc_Tcutil_bko;default: _LL19: _LL1A:
 return& Cyc_Tcutil_ptrbko;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2495
{enum Cyc_Absyn_KindQual _tmp417=_tmp41D;switch(_tmp417){case Cyc_Absyn_AnyKind: _LL1C: _LL1D:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL1E: _LL1F:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL20: _LL21:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL22: _LL23:
 return& Cyc_Tcutil_urko;default: _LL24: _LL25:
 goto _LL1B;}_LL1B:;}
# 2502
goto _LL3;default: _LL8: _LL9:
# 2504
{enum Cyc_Absyn_KindQual _tmp418=_tmp41D;switch(_tmp418){case Cyc_Absyn_AnyKind: _LL27: _LL28:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL29: _LL2A:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_trko;default: _LL2F: _LL30:
 goto _LL26;}_LL26:;}
# 2511
goto _LL3;}_LL3:;}
# 2513
({struct Cyc_String_pa_PrintArg_struct _tmp41B=({struct Cyc_String_pa_PrintArg_struct _tmp9C5;_tmp9C5.tag=0U,({struct _dyneither_ptr _tmpB9A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp9C5.f1=_tmpB9A;});_tmp9C5;});void*_tmp419[1U];_tmp419[0]=& _tmp41B;({struct _dyneither_ptr _tmpB9B=({const char*_tmp41A="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp41A,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB9B,_tag_dyneither(_tmp419,sizeof(void*),1U));});});}
# 2516
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->tag=0U,_tmp41E->f1=k;_tmp41E;});}
# 2519
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp41F=_cycalloc(sizeof(*_tmp41F));({void*_tmpB9C=Cyc_Tcutil_kind_to_bound(k);_tmp41F->v=_tmpB9C;});_tmp41F;});}
# 2525
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpB9D=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->tag=0U,_tmp420->f1=Cyc_Absyn_Null_c;_tmp420;});e1->r=_tmpB9D;});{
struct Cyc_Core_Opt*_tmp421=Cyc_Tcenv_lookup_opt_type_vars(te);
void*_tmp422=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9C8;({void*_tmpBA2=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp421);_tmp9C8.elt_type=_tmpBA2;}),({
struct Cyc_Absyn_Tqual _tmpBA1=Cyc_Absyn_empty_tqual(0U);_tmp9C8.elt_tq=_tmpBA1;}),
({void*_tmpBA0=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp421);(_tmp9C8.ptr_atts).rgn=_tmpBA0;}),(_tmp9C8.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 2533
void*_tmpB9F=Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,_tmp421);(_tmp9C8.ptr_atts).bounds=_tmpB9F;}),({
void*_tmpB9E=Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,_tmp421);(_tmp9C8.ptr_atts).zero_term=_tmpB9E;}),(_tmp9C8.ptr_atts).ptrloc=0;_tmp9C8;}));
e1->topt=_tmp422;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp425=({struct Cyc_String_pa_PrintArg_struct _tmp9C7;_tmp9C7.tag=0U,({
struct _dyneither_ptr _tmpBA3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9C7.f1=_tmpBA3;});_tmp9C7;});struct Cyc_String_pa_PrintArg_struct _tmp426=({struct Cyc_String_pa_PrintArg_struct _tmp9C6;_tmp9C6.tag=0U,({struct _dyneither_ptr _tmpBA4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmp9C6.f1=_tmpBA4;});_tmp9C6;});void*_tmp423[2U];_tmp423[0]=& _tmp425,_tmp423[1]=& _tmp426;({struct _dyneither_ptr _tmpBA5=({const char*_tmp424="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp424,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA5,_tag_dyneither(_tmp423,sizeof(void*),2U));});});
return retv;};};}
# 2543
return 0;}
# 2546
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp427=c;switch(_tmp427){case Cyc_Absyn_Unknown_coercion: _LL1: _LL2:
 return({const char*_tmp428="unknown";_tag_dyneither(_tmp428,sizeof(char),8U);});case Cyc_Absyn_No_coercion: _LL3: _LL4:
 return({const char*_tmp429="no coercion";_tag_dyneither(_tmp429,sizeof(char),12U);});case Cyc_Absyn_Null_to_NonNull: _LL5: _LL6:
 return({const char*_tmp42A="null check";_tag_dyneither(_tmp42A,sizeof(char),11U);});default: _LL7: _LL8:
 return({const char*_tmp42B="other coercion";_tag_dyneither(_tmp42B,sizeof(char),15U);});}_LL0:;}
# 2555
int Cyc_Tcutil_warn_alias_coerce=0;
# 2561
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2565
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpBA8=({struct _dyneither_ptr*_tmp43E=_cycalloc(sizeof(*_tmp43E));({struct _dyneither_ptr _tmpBA7=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp43D=({struct Cyc_Int_pa_PrintArg_struct _tmp9CA;_tmp9CA.tag=1U,_tmp9CA.f1=(unsigned long)counter ++;_tmp9CA;});void*_tmp43B[1U];_tmp43B[0]=& _tmp43D;({struct _dyneither_ptr _tmpBA6=({const char*_tmp43C="__aliasvar%d";_tag_dyneither(_tmp43C,sizeof(char),13U);});Cyc_aprintf(_tmpBA6,_tag_dyneither(_tmp43B,sizeof(void*),1U));});});*_tmp43E=_tmpBA7;});_tmp43E;});_tmp43F->f2=_tmpBA8;});_tmp43F;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpBA9=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=4U,_tmp43A->f1=vd;_tmp43A;});Cyc_Absyn_varb_exp(_tmpBA9,e->loc);});
# 2574
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2576
{void*_tmp42C=Cyc_Tcutil_compress(e_type);void*_tmp42D=_tmp42C;void*_tmp439;struct Cyc_Absyn_Tqual _tmp438;void*_tmp437;void*_tmp436;void*_tmp435;void*_tmp434;struct Cyc_Absyn_PtrLoc*_tmp433;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->tag == 3U){_LL1: _tmp439=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->f1).elt_type;_tmp438=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->f1).elt_tq;_tmp437=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->f1).ptr_atts).rgn;_tmp436=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->f1).ptr_atts).nullable;_tmp435=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->f1).ptr_atts).bounds;_tmp434=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->f1).ptr_atts).zero_term;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42D)->f1).ptr_atts).ptrloc;_LL2:
# 2578
{void*_tmp42E=Cyc_Tcutil_compress(_tmp437);void*_tmp42F=_tmp42E;void**_tmp432;struct Cyc_Core_Opt*_tmp431;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42F)->tag == 1U){_LL6: _tmp432=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42F)->f2;_tmp431=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42F)->f4;_LL7: {
# 2580
void*_tmp430=Cyc_Absyn_var_type(tv);
*_tmp432=_tmp430;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2585
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2589
e->topt=0;
vd->initializer=0;{
# 2593
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2595
return({struct _tuple14 _tmp9C9;_tmp9C9.f1=d,_tmp9C9.f2=ve;_tmp9C9;});};}
# 2600
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned int loc){
# 2603
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_type,0)) && 
Cyc_Tcutil_is_pointer_type(e_type)){
# 2608
void*_tmp440=Cyc_Tcutil_compress(wants_type);void*_tmp441=_tmp440;void*_tmp443;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp441)->tag == 3U){_LL1: _tmp443=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp441)->f1).ptr_atts).rgn;_LL2:
# 2610
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp443))return 0;{
struct Cyc_Absyn_Kind*_tmp442=Cyc_Tcutil_type_kind(_tmp443);
return _tmp442->kind == Cyc_Absyn_RgnKind  && _tmp442->aliasqual == Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2616
return 0;}
# 2620
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2625
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2627
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2629
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp446=({struct Cyc_String_pa_PrintArg_struct _tmp9CC;_tmp9CC.tag=0U,({
struct _dyneither_ptr _tmpBAA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9CC.f1=_tmpBAA;});_tmp9CC;});struct Cyc_String_pa_PrintArg_struct _tmp447=({struct Cyc_String_pa_PrintArg_struct _tmp9CB;_tmp9CB.tag=0U,({struct _dyneither_ptr _tmpBAB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9CB.f1=_tmpBAB;});_tmp9CB;});void*_tmp444[2U];_tmp444[0]=& _tmp446,_tmp444[1]=& _tmp447;({unsigned int _tmpBAD=e->loc;struct _dyneither_ptr _tmpBAC=({const char*_tmp445="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp445,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpBAD,_tmpBAC,_tag_dyneither(_tmp444,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2637
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp44A=({struct Cyc_String_pa_PrintArg_struct _tmp9CF;_tmp9CF.tag=0U,({
struct _dyneither_ptr _tmpBAE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp9CF.f1=_tmpBAE;});_tmp9CF;});struct Cyc_String_pa_PrintArg_struct _tmp44B=({struct Cyc_String_pa_PrintArg_struct _tmp9CE;_tmp9CE.tag=0U,({struct _dyneither_ptr _tmpBAF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9CE.f1=_tmpBAF;});_tmp9CE;});struct Cyc_String_pa_PrintArg_struct _tmp44C=({struct Cyc_String_pa_PrintArg_struct _tmp9CD;_tmp9CD.tag=0U,({struct _dyneither_ptr _tmpBB0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9CD.f1=_tmpBB0;});_tmp9CD;});void*_tmp448[3U];_tmp448[0]=& _tmp44A,_tmp448[1]=& _tmp44B,_tmp448[2]=& _tmp44C;({unsigned int _tmpBB2=e->loc;struct _dyneither_ptr _tmpBB1=({const char*_tmp449="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp449,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpBB2,_tmpBB1,_tag_dyneither(_tmp448,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2644
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({enum Cyc_Absyn_Coercion _tmpBB3=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpBB3;})!= Cyc_Absyn_Unknown_coercion){
# 2652
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp44F=({struct Cyc_String_pa_PrintArg_struct _tmp9D1;_tmp9D1.tag=0U,({
struct _dyneither_ptr _tmpBB4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9D1.f1=_tmpBB4;});_tmp9D1;});struct Cyc_String_pa_PrintArg_struct _tmp450=({struct Cyc_String_pa_PrintArg_struct _tmp9D0;_tmp9D0.tag=0U,({struct _dyneither_ptr _tmpBB5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9D0.f1=_tmpBB5;});_tmp9D0;});void*_tmp44D[2U];_tmp44D[0]=& _tmp44F,_tmp44D[1]=& _tmp450;({unsigned int _tmpBB7=e->loc;struct _dyneither_ptr _tmpBB6=({const char*_tmp44E="implicit cast from %s to %s";_tag_dyneither(_tmp44E,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpBB7,_tmpBB6,_tag_dyneither(_tmp44D,sizeof(void*),2U));});});
return 1;}else{
# 2658
return 0;}}}}}
# 2665
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2668
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2682 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple25{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2686
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple25*env,struct Cyc_Absyn_Aggrfield*x){
# 2689
struct _tuple25 _tmp451=*env;struct _tuple25 _tmp452=_tmp451;struct Cyc_List_List*_tmp45A;struct _RegionHandle*_tmp459;struct Cyc_Tcenv_Tenv*_tmp458;int _tmp457;_LL1: _tmp45A=_tmp452.f1;_tmp459=_tmp452.f2;_tmp458=_tmp452.f3;_tmp457=_tmp452.f4;_LL2:;{
# 2691
void*_tmp453=_tmp45A == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp459,_tmp45A,x->type);
struct Cyc_List_List*_tmp454=Cyc_Tcutil_flatten_type(_tmp459,_tmp457,_tmp458,_tmp453);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp454)== 1)
return({struct Cyc_List_List*_tmp456=_region_malloc(_tmp459,sizeof(*_tmp456));({struct _tuple12*_tmpBB8=({struct _tuple12*_tmp455=_region_malloc(_tmp459,sizeof(*_tmp455));_tmp455->f1=x->tq,_tmp455->f2=_tmp453;_tmp455;});_tmp456->hd=_tmpBB8;}),_tmp456->tl=0;_tmp456;});else{
return _tmp454;}};}struct _tuple26{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2697
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple26*env,struct _tuple12*x){
# 2699
struct _tuple26 _tmp45B=*env;struct _tuple26 _tmp45C=_tmp45B;struct _RegionHandle*_tmp466;struct Cyc_Tcenv_Tenv*_tmp465;int _tmp464;_LL1: _tmp466=_tmp45C.f1;_tmp465=_tmp45C.f2;_tmp464=_tmp45C.f3;_LL2:;{
struct _tuple12 _tmp45D=*x;struct _tuple12 _tmp45E=_tmp45D;struct Cyc_Absyn_Tqual _tmp463;void*_tmp462;_LL4: _tmp463=_tmp45E.f1;_tmp462=_tmp45E.f2;_LL5:;{
struct Cyc_List_List*_tmp45F=Cyc_Tcutil_flatten_type(_tmp466,_tmp464,_tmp465,_tmp462);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45F)== 1)
return({struct Cyc_List_List*_tmp461=_region_malloc(_tmp466,sizeof(*_tmp461));({struct _tuple12*_tmpBB9=({struct _tuple12*_tmp460=_region_malloc(_tmp466,sizeof(*_tmp460));_tmp460->f1=_tmp463,_tmp460->f2=_tmp462;_tmp460;});_tmp461->hd=_tmpBB9;}),_tmp461->tl=0;_tmp461;});else{
return _tmp45F;}};};}
# 2706
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2710
if(flatten){
({void*_tmpBBA=Cyc_Tcutil_compress(t1);t1=_tmpBBA;});{
void*_tmp467=t1;struct Cyc_List_List*_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Aggrdecl*_tmp484;struct Cyc_List_List*_tmp483;switch(*((int*)_tmp467)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp467)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp467)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp484=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp467)->f1)->f1).KnownAggr).val;_tmp483=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp467)->f2;_LL6:
# 2728
 if(((_tmp484->kind == Cyc_Absyn_UnionA  || _tmp484->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->rgn_po != 0)
# 2730
return({struct Cyc_List_List*_tmp472=_region_malloc(r,sizeof(*_tmp472));({struct _tuple12*_tmpBBC=({struct _tuple12*_tmp471=_region_malloc(r,sizeof(*_tmp471));({struct Cyc_Absyn_Tqual _tmpBBB=Cyc_Absyn_empty_tqual(0U);_tmp471->f1=_tmpBBB;}),_tmp471->f2=t1;_tmp471;});_tmp472->hd=_tmpBBC;}),_tmp472->tl=0;_tmp472;});{
struct Cyc_List_List*_tmp473=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp484->tvs,_tmp483);
struct _tuple25 env=({struct _tuple25 _tmp9D2;_tmp9D2.f1=_tmp473,_tmp9D2.f2=r,_tmp9D2.f3=te,_tmp9D2.f4=flatten;_tmp9D2;});
struct Cyc_List_List*_tmp474=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp484->impl))->fields;struct Cyc_List_List*_tmp475=_tmp474;struct Cyc_Absyn_Aggrfield*_tmp47B;struct Cyc_List_List*_tmp47A;if(_tmp475 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp47B=(struct Cyc_Absyn_Aggrfield*)_tmp475->hd;_tmp47A=_tmp475->tl;_LL14: {
# 2736
struct Cyc_List_List*_tmp476=Cyc_Tcutil_flatten_type_f(& env,_tmp47B);
env.f4=0;{
struct Cyc_List_List*_tmp477=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp47A);
struct Cyc_List_List*_tmp478=({struct Cyc_List_List*_tmp479=_region_malloc(r,sizeof(*_tmp479));_tmp479->hd=_tmp476,_tmp479->tl=_tmp477;_tmp479;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp478);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp485=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp467)->f1;_LL4: {
# 2715
struct _tuple26 _tmp468=({struct _tuple26 _tmp9D3;_tmp9D3.f1=r,_tmp9D3.f2=te,_tmp9D3.f3=flatten;_tmp9D3;});
# 2717
struct Cyc_List_List*_tmp469=_tmp485;struct _tuple12*_tmp470;struct Cyc_List_List*_tmp46F;if(_tmp469 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp470=(struct _tuple12*)_tmp469->hd;_tmp46F=_tmp469->tl;_LLF: {
# 2720
struct Cyc_List_List*_tmp46A=Cyc_Tcutil_rcopy_tqt(& _tmp468,_tmp470);
_tmp468.f3=0;{
struct Cyc_List_List*_tmp46B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple26*,struct _tuple12*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp468,_tmp485);
struct Cyc_List_List*_tmp46C=({struct Cyc_List_List*_tmp46E=_region_malloc(r,sizeof(*_tmp46E));_tmp46E->hd=_tmp46A,_tmp46E->tl=_tmp46B;_tmp46E;});
return({struct _RegionHandle*_tmpBBD=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpBBD,({struct Cyc_List_List*_tmp46D=_region_malloc(r,sizeof(*_tmp46D));_tmp46D->hd=_tmp46A,_tmp46D->tl=_tmp46B;_tmp46D;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp467)->f1 == Cyc_Absyn_StructA){_LL7: _tmp486=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp467)->f2;_LL8: {
# 2743
struct _tuple25 env=({struct _tuple25 _tmp9D4;_tmp9D4.f1=0,_tmp9D4.f2=r,_tmp9D4.f3=te,_tmp9D4.f4=flatten;_tmp9D4;});
struct Cyc_List_List*_tmp47C=_tmp486;struct Cyc_Absyn_Aggrfield*_tmp482;struct Cyc_List_List*_tmp481;if(_tmp47C == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp482=(struct Cyc_Absyn_Aggrfield*)_tmp47C->hd;_tmp481=_tmp47C->tl;_LL19: {
# 2747
struct Cyc_List_List*_tmp47D=Cyc_Tcutil_flatten_type_f(& env,_tmp482);
env.f4=0;{
struct Cyc_List_List*_tmp47E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp481);
struct Cyc_List_List*_tmp47F=({struct Cyc_List_List*_tmp480=_region_malloc(r,sizeof(*_tmp480));_tmp480->hd=_tmp47D,_tmp480->tl=_tmp47E;_tmp480;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp47F);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2753
 goto _LL0;}_LL0:;};}
# 2756
return({struct Cyc_List_List*_tmp488=_region_malloc(r,sizeof(*_tmp488));({struct _tuple12*_tmpBBF=({struct _tuple12*_tmp487=_region_malloc(r,sizeof(*_tmp487));({struct Cyc_Absyn_Tqual _tmpBBE=Cyc_Absyn_empty_tqual(0U);_tmp487->f1=_tmpBBE;}),_tmp487->f2=t1;_tmp487;});_tmp488->hd=_tmpBBF;}),_tmp488->tl=0;_tmp488;});}
# 2760
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp489=(void*)t->hd;void*_tmp48A=_tmp489;switch(*((int*)_tmp48A)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2766
 continue;default: _LL7: _LL8:
# 2768
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2771
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2774
return 1;}
# 2777
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp48B=({struct _tuple0 _tmp9D5;({void*_tmpBC1=Cyc_Tcutil_compress(t1);_tmp9D5.f1=_tmpBC1;}),({void*_tmpBC0=Cyc_Tcutil_compress(t2);_tmp9D5.f2=_tmpBC0;});_tmp9D5;});struct _tuple0 _tmp48C=_tmp48B;enum Cyc_Absyn_Size_of _tmp48E;enum Cyc_Absyn_Size_of _tmp48D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->f1)->tag == 1U){_LL1: _tmp48E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->f1)->f2;_tmp48D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->f1)->f2;_LL2:
# 2780
 return(_tmp48E == _tmp48D  || _tmp48E == Cyc_Absyn_Int_sz  && _tmp48D == Cyc_Absyn_Long_sz) || 
_tmp48E == Cyc_Absyn_Long_sz  && _tmp48D == Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2788
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2791
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
({void*_tmpBC2=Cyc_Tcutil_compress(t1);t1=_tmpBC2;});
({void*_tmpBC3=Cyc_Tcutil_compress(t2);t2=_tmpBC3;});{
struct _tuple0 _tmp48F=({struct _tuple0 _tmp9D6;_tmp9D6.f1=t1,_tmp9D6.f2=t2;_tmp9D6;});struct _tuple0 _tmp490=_tmp48F;struct Cyc_Absyn_FnInfo _tmp4BA;struct Cyc_Absyn_FnInfo _tmp4B9;struct Cyc_Absyn_Datatypedecl*_tmp4B8;struct Cyc_Absyn_Datatypefield*_tmp4B7;struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_Datatypedecl*_tmp4B5;struct Cyc_List_List*_tmp4B4;void*_tmp4B3;struct Cyc_Absyn_Tqual _tmp4B2;void*_tmp4B1;void*_tmp4B0;void*_tmp4AF;void*_tmp4AE;void*_tmp4AD;struct Cyc_Absyn_Tqual _tmp4AC;void*_tmp4AB;void*_tmp4AA;void*_tmp4A9;void*_tmp4A8;switch(*((int*)_tmp490.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f2)->tag == 3U){_LL1: _tmp4B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f1)->f1).elt_type;_tmp4B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f1)->f1).elt_tq;_tmp4B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f1)->f1).ptr_atts).rgn;_tmp4B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f1)->f1).ptr_atts).nullable;_tmp4AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f1)->f1).ptr_atts).bounds;_tmp4AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f1)->f1).ptr_atts).zero_term;_tmp4AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f2)->f1).elt_type;_tmp4AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f2)->f1).elt_tq;_tmp4AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f2)->f1).ptr_atts).rgn;_tmp4AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f2)->f1).ptr_atts).nullable;_tmp4A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f2)->f1).ptr_atts).bounds;_tmp4A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490.f2)->f1).ptr_atts).zero_term;_LL2:
# 2803
 if(_tmp4B2.real_const  && !_tmp4AC.real_const)
return 0;
# 2806
if((!Cyc_Tcutil_unify(_tmp4B0,_tmp4AA) && 
Cyc_Absyn_type2bool(0,_tmp4B0)) && !Cyc_Absyn_type2bool(0,_tmp4AA))
return 0;
# 2810
if((Cyc_Tcutil_unify(_tmp4AE,_tmp4A8) && !
Cyc_Absyn_type2bool(0,_tmp4AE)) && Cyc_Absyn_type2bool(0,_tmp4A8))
return 0;
# 2814
if((!Cyc_Tcutil_unify(_tmp4B1,_tmp4AB) && !Cyc_Tcenv_region_outlives(te,_tmp4B1,_tmp4AB)) && !
Cyc_Tcutil_subtype(te,assume,_tmp4B1,_tmp4AB))
return 0;
# 2818
if(!Cyc_Tcutil_unify(_tmp4AF,_tmp4A9)){
struct Cyc_Absyn_Exp*_tmp491=({void*_tmpBC4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBC4,_tmp4AF);});
struct Cyc_Absyn_Exp*_tmp492=({void*_tmpBC5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBC5,_tmp4A9);});
if(_tmp491 != _tmp492){
if((_tmp491 == 0  || _tmp492 == 0) || !Cyc_Evexp_lte_const_exp(_tmp492,_tmp492))
return 0;}}
# 2828
if(!_tmp4AC.real_const  && _tmp4B2.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4AD)))
return 0;}{
# 2834
int _tmp493=
({void*_tmpBC6=_tmp4A9;Cyc_Tcutil_unify(_tmpBC6,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4A8);
# 2839
return(_tmp493  && ({struct Cyc_Tcenv_Tenv*_tmpBCA=te;struct Cyc_List_List*_tmpBC9=({struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));({struct _tuple0*_tmpBC7=({struct _tuple0*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->f1=t1,_tmp494->f2=t2;_tmp494;});_tmp495->hd=_tmpBC7;}),_tmp495->tl=assume;_tmp495;});void*_tmpBC8=_tmp4B3;Cyc_Tcutil_ptrsubtype(_tmpBCA,_tmpBC9,_tmpBC8,_tmp4AD);}) || Cyc_Tcutil_unify(_tmp4B3,_tmp4AD)) || Cyc_Tcutil_isomorphic(_tmp4B3,_tmp4AD);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp4B8=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp4B7=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp4B6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f1)->f2;_tmp4B5=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f2)->f1)->f1).KnownDatatype).val;_tmp4B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp490.f2)->f2;_LL4:
# 2846
 if(_tmp4B8 != _tmp4B5  && Cyc_Absyn_qvar_cmp(_tmp4B8->name,_tmp4B5->name)!= 0)return 0;
# 2848
if(({int _tmpBCB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B6);_tmpBCB != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B4);}))return 0;
for(0;_tmp4B6 != 0;(_tmp4B6=_tmp4B6->tl,_tmp4B4=_tmp4B4->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4B6->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp4B4))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp490.f2)->tag == 5U){_LL5: _tmp4BA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp490.f1)->f1;_tmp4B9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp490.f2)->f1;_LL6:
# 2855
 if(_tmp4BA.tvars != 0  || _tmp4B9.tvars != 0){
struct Cyc_List_List*_tmp496=_tmp4BA.tvars;
struct Cyc_List_List*_tmp497=_tmp4B9.tvars;
if(({int _tmpBCC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp496);_tmpBCC != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp497);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp496 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp496->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp497))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpBCF=({struct Cyc_List_List*_tmp499=_cycalloc(sizeof(*_tmp499));({struct _tuple15*_tmpBCE=({struct _tuple15*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->f1=(struct Cyc_Absyn_Tvar*)_tmp497->hd,({void*_tmpBCD=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp496->hd);_tmp498->f2=_tmpBCD;});_tmp498;});_tmp499->hd=_tmpBCE;}),_tmp499->tl=inst;_tmp499;});inst=_tmpBCF;});
_tmp496=_tmp496->tl;
_tmp497=_tmp497->tl;}
# 2866
if(inst != 0){
_tmp4BA.tvars=0;
_tmp4B9.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpBD2=te;struct Cyc_List_List*_tmpBD1=assume;void*_tmpBD0=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A->tag=5U,_tmp49A->f1=_tmp4BA;_tmp49A;});Cyc_Tcutil_subtype(_tmpBD2,_tmpBD1,_tmpBD0,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->tag=5U,_tmp49B->f1=_tmp4B9;_tmp49B;}));});}};}
# 2873
if(!Cyc_Tcutil_subtype(te,assume,_tmp4BA.ret_type,_tmp4B9.ret_type))return 0;{
struct Cyc_List_List*_tmp49C=_tmp4BA.args;
struct Cyc_List_List*_tmp49D=_tmp4B9.args;
# 2878
if(({int _tmpBD3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49C);_tmpBD3 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49D);}))return 0;
# 2880
for(0;_tmp49C != 0;(_tmp49C=_tmp49C->tl,_tmp49D=_tmp49D->tl)){
struct _tuple10 _tmp49E=*((struct _tuple10*)_tmp49C->hd);struct _tuple10 _tmp49F=_tmp49E;struct Cyc_Absyn_Tqual _tmp4A5;void*_tmp4A4;_LLA: _tmp4A5=_tmp49F.f2;_tmp4A4=_tmp49F.f3;_LLB:;{
struct _tuple10 _tmp4A0=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp49D))->hd);struct _tuple10 _tmp4A1=_tmp4A0;struct Cyc_Absyn_Tqual _tmp4A3;void*_tmp4A2;_LLD: _tmp4A3=_tmp4A1.f2;_tmp4A2=_tmp4A1.f3;_LLE:;
# 2884
if(_tmp4A3.real_const  && !_tmp4A5.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp4A2,_tmp4A4))
return 0;};}
# 2888
if(_tmp4BA.c_varargs != _tmp4B9.c_varargs)return 0;
if(_tmp4BA.cyc_varargs != 0  && _tmp4B9.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp4A6=*_tmp4BA.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp4A7=*_tmp4B9.cyc_varargs;
# 2893
if((_tmp4A7.tq).real_const  && !(_tmp4A6.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp4A7.type,_tmp4A6.type))
return 0;}else{
if(_tmp4BA.cyc_varargs != 0  || _tmp4B9.cyc_varargs != 0)return 0;}
# 2898
if(!({void*_tmpBD4=(void*)_check_null(_tmp4BA.effect);Cyc_Tcutil_subset_effect(1,_tmpBD4,(void*)_check_null(_tmp4B9.effect));}))return 0;
# 2900
if(!Cyc_Tcutil_sub_rgnpo(_tmp4BA.rgn_po,_tmp4B9.rgn_po))return 0;
# 2902
if(!Cyc_Tcutil_sub_attributes(_tmp4BA.attributes,_tmp4B9.attributes))return 0;
# 2904
if(!Cyc_Tcutil_check_logical_implication(_tmp4B9.requires_relns,_tmp4BA.requires_relns))
return 0;
# 2907
if(!Cyc_Tcutil_check_logical_implication(_tmp4BA.ensures_relns,_tmp4B9.ensures_relns))
return 0;
# 2910
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2922 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2924
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp4BB=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp4BC=_tmp4BB;struct Cyc_Absyn_Tqual _tmp4C2;void*_tmp4C1;_LL1: _tmp4C2=_tmp4BC->f1;_tmp4C1=_tmp4BC->f2;_LL2:;{
struct _tuple12*_tmp4BD=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp4BE=_tmp4BD;struct Cyc_Absyn_Tqual _tmp4C0;void*_tmp4BF;_LL4: _tmp4C0=_tmp4BE->f1;_tmp4BF=_tmp4BE->f2;_LL5:;
# 2931
if(_tmp4C2.real_const  && !_tmp4C0.real_const)return 0;
# 2933
if((_tmp4C0.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4BF))) && 
Cyc_Tcutil_subtype(te,assume,_tmp4C1,_tmp4BF))
# 2936
continue;
# 2938
if(Cyc_Tcutil_unify(_tmp4C1,_tmp4BF))
# 2940
continue;
# 2942
if(Cyc_Tcutil_isomorphic(_tmp4C1,_tmp4BF))
# 2944
continue;
# 2947
return 0;};};}
# 2949
return 1;}
# 2954
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpBD5=Cyc_Tcutil_compress(t1);t1=_tmpBD5;});
({void*_tmpBD6=Cyc_Tcutil_compress(t2);t2=_tmpBD6;});
# 2960
{void*_tmp4C3=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C3)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C3)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C3)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2963
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2965
 if((Cyc_Tcutil_type_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2969
void*_tmp4C4=t1;void*_tmp4E9;struct Cyc_Absyn_Enumdecl*_tmp4E8;void*_tmp4E7;struct Cyc_Absyn_Tqual _tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;void*_tmp4E4;void*_tmp4E3;struct Cyc_Absyn_Tqual _tmp4E2;void*_tmp4E1;void*_tmp4E0;void*_tmp4DF;void*_tmp4DE;switch(*((int*)_tmp4C4)){case 3U: _LLA: _tmp4E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C4)->f1).elt_type;_tmp4E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C4)->f1).elt_tq;_tmp4E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C4)->f1).ptr_atts).rgn;_tmp4E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C4)->f1).ptr_atts).nullable;_tmp4DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C4)->f1).ptr_atts).bounds;_tmp4DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C4)->f1).ptr_atts).zero_term;_LLB:
# 2978
{void*_tmp4C5=t2;void*_tmp4D4;struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D2;void*_tmp4D1;void*_tmp4D0;void*_tmp4CF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C5)->tag == 3U){_LL19: _tmp4D4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C5)->f1).elt_type;_tmp4D3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C5)->f1).elt_tq;_tmp4D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C5)->f1).ptr_atts).rgn;_tmp4D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C5)->f1).ptr_atts).nullable;_tmp4D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C5)->f1).ptr_atts).bounds;_tmp4CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C5)->f1).ptr_atts).zero_term;_LL1A: {
# 2982
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp4C6=({struct Cyc_List_List*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));({struct _tuple0*_tmpBD7=({struct _tuple0*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->f1=t1,_tmp4CD->f2=t2;_tmp4CD;});_tmp4CE->hd=_tmpBD7;}),_tmp4CE->tl=0;_tmp4CE;});
int _tmp4C7=_tmp4D3.real_const  || !_tmp4E2.real_const;
# 2996 "tcutil.cyc"
int _tmp4C8=
({void*_tmpBD8=_tmp4D0;Cyc_Tcutil_unify(_tmpBD8,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4CF);
# 2999
int _tmp4C9=_tmp4C7  && (
((_tmp4C8  && Cyc_Tcutil_ptrsubtype(te,_tmp4C6,_tmp4E3,_tmp4D4) || 
Cyc_Tcutil_unify(_tmp4E3,_tmp4D4)) || Cyc_Tcutil_isomorphic(_tmp4E3,_tmp4D4)) || Cyc_Tcutil_unify(_tmp4D4,Cyc_Absyn_void_type));
# 3003
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=Cyc_Tcutil_unify(_tmp4DE,_tmp4CF) || !Cyc_Absyn_type2bool(0,_tmp4CF);
# 3007
int _tmp4CA=_tmp4C9?0:((Cyc_Tcutil_is_bits_only_type(_tmp4E3) && Cyc_Tcutil_is_char_type(_tmp4D4)) && !
Cyc_Tcutil_force_type2bool(0,_tmp4CF)) && (
_tmp4D3.real_const  || !_tmp4E2.real_const);
int bounds_ok=Cyc_Tcutil_unify(_tmp4DF,_tmp4D0);
if(!bounds_ok  && !_tmp4CA){
struct Cyc_Absyn_Exp*_tmp4CB=({void*_tmpBD9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBD9,_tmp4DF);});
struct Cyc_Absyn_Exp*_tmp4CC=({void*_tmpBDA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBDA,_tmp4D0);});
if((_tmp4CB != 0  && _tmp4CC != 0) && Cyc_Evexp_lte_const_exp(_tmp4CC,_tmp4CB))
bounds_ok=1;else{
if(_tmp4CB == 0  || _tmp4CC == 0)
bounds_ok=1;}}{
# 3019
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4E0);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4D1);
if(t1_nullable  && !t2_nullable)
coercion=2U;
# 3026
if(((bounds_ok  && zeroterm_ok) && (_tmp4C9  || _tmp4CA)) && (
Cyc_Tcutil_unify(_tmp4E1,_tmp4D2) || Cyc_Tcenv_region_outlives(te,_tmp4E1,_tmp4D2)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 3032
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp4E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C4)->f1).elt_type;_tmp4E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C4)->f1).tq;_tmp4E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C4)->f1).num_elts;_tmp4E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C4)->f1).zero_term;_LLD:
# 3034
{void*_tmp4D5=t2;void*_tmp4D9;struct Cyc_Absyn_Tqual _tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;void*_tmp4D6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D5)->tag == 4U){_LL1E: _tmp4D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D5)->f1).elt_type;_tmp4D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D5)->f1).tq;_tmp4D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D5)->f1).num_elts;_tmp4D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D5)->f1).zero_term;_LL1F: {
# 3036
int okay;
({int _tmpBDB=
(((_tmp4E5 != 0  && _tmp4D7 != 0) && Cyc_Tcutil_unify(_tmp4E4,_tmp4D6)) && 
Cyc_Evexp_lte_const_exp(_tmp4D7,_tmp4E5)) && 
Cyc_Evexp_lte_const_exp(_tmp4E5,_tmp4D7);
# 3037
okay=_tmpBDB;});
# 3041
return
# 3043
(okay  && Cyc_Tcutil_unify(_tmp4E7,_tmp4D9)) && (!_tmp4E6.real_const  || _tmp4D8.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 3045
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 3047
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f1)){case 15U: _LLE: _tmp4E8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f1)->f2;_LLF:
# 3051
{void*_tmp4DA=t2;struct Cyc_Absyn_Enumdecl*_tmp4DB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DA)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DA)->f1)->tag == 15U){_LL23: _tmp4DB=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DA)->f1)->f2;_LL24:
# 3053
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4E8))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4DB))->fields != 0) && ({
int _tmpBDC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->fields))->v);_tmpBDC >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4DB->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 3059
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 3062
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f2 != 0){_LL14: _tmp4E9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C4)->f2)->hd;_LL15:
# 3065
{void*_tmp4DC=t2;void*_tmp4DD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DC)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DC)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DC)->f2 != 0){_LL28: _tmp4DD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DC)->f2)->hd;_LL29:
# 3067
 if(Cyc_Tcenv_region_outlives(te,_tmp4E9,_tmp4DD))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 3071
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 3077
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp4EA=Cyc_Absyn_copy_exp(e);
({void*_tmpBDD=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->tag=14U,_tmp4EB->f1=t,_tmp4EB->f2=_tmp4EA,_tmp4EB->f3=0,_tmp4EB->f4=c;_tmp4EB;});e->r=_tmpBDD;});
e->topt=t;}}
# 3085
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple0 _tmp4EC=({struct _tuple0 _tmp9D8;({void*_tmpBDF=Cyc_Tcutil_compress(t1);_tmp9D8.f1=_tmpBDF;}),({void*_tmpBDE=Cyc_Tcutil_compress(t2);_tmp9D8.f2=_tmpBDE;});_tmp9D8;});struct _tuple0 _tmp4ED=_tmp4EC;void*_tmp4F3;void*_tmp4F2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4ED.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4ED.f2)->tag == 0U){_LL1: _tmp4F3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4ED.f1)->f1;_tmp4F2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4ED.f2)->f1;_LL2:
# 3088
{struct _tuple0 _tmp4EE=({struct _tuple0 _tmp9D7;_tmp9D7.f1=_tmp4F3,_tmp9D7.f2=_tmp4F2;_tmp9D7;});struct _tuple0 _tmp4EF=_tmp4EE;int _tmp4F1;int _tmp4F0;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EF.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EF.f2)->tag == 2U){_LL6: _tmp4F1=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f1;_tmp4F0=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f1;_LL7:
# 3090
 if(_tmp4F1 != 0  && _tmp4F1 != 1)return t1;else{
if(_tmp4F0 != 0  && _tmp4F0 != 1)return t2;else{
if(_tmp4F1 >= _tmp4F0)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EF.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1A;else{goto _LL20;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL18;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL1E;else{goto _LL20;}}}}}else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2){case Cyc_Absyn_Long_sz: _LL14: _LL15:
# 3100
 goto _LL17;case Cyc_Absyn_Int_sz: _LL18: _LL19:
# 3103
 goto _LL1B;default: goto _LL20;}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1A;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL1C;else{goto _LL20;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL1E;else{goto _LL20;}}}}}}else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL10: _LL11:
# 3098
 goto _LL13;case Cyc_Absyn_Long_sz: _LL1C: _LL1D:
# 3105
 goto _LL1F;default: goto _LL20;}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 3097
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 3101
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_Int_sz: _LL1A: _LL1B:
# 3104
 return Cyc_Absyn_uint_type;default: goto _LL20;}else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EF.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
# 3099
 return Cyc_Absyn_slonglong_type;case Cyc_Absyn_Long_sz: _LL1E: _LL1F:
# 3106
 return Cyc_Absyn_slong_type;default: goto _LL20;}}}else{_LL20: _LL21:
 goto _LL5;}}}}_LL5:;}
# 3109
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3112
return Cyc_Absyn_sint_type;}
# 3122 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpBE0=Cyc_Absyn_compress_kb(c1);c1=_tmpBE0;});
({void*_tmpBE1=Cyc_Absyn_compress_kb(c2);c2=_tmpBE1;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp4F4=({struct _tuple0 _tmp9D9;_tmp9D9.f1=c1,_tmp9D9.f2=c2;_tmp9D9;});struct _tuple0 _tmp4F5=_tmp4F4;struct Cyc_Core_Opt**_tmp509;struct Cyc_Absyn_Kind*_tmp508;struct Cyc_Core_Opt**_tmp507;struct Cyc_Absyn_Kind*_tmp506;struct Cyc_Core_Opt**_tmp505;struct Cyc_Absyn_Kind*_tmp504;struct Cyc_Absyn_Kind*_tmp503;struct Cyc_Core_Opt**_tmp502;struct Cyc_Core_Opt**_tmp501;struct Cyc_Absyn_Kind*_tmp500;struct Cyc_Core_Opt**_tmp4FF;struct Cyc_Absyn_Kind*_tmp4FE;struct Cyc_Absyn_Kind*_tmp4FD;struct Cyc_Absyn_Kind*_tmp4FC;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->tag == 0U)switch(*((int*)_tmp4F5.f2)){case 0U: _LL1: _tmp4FD=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->f1;_tmp4FC=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->f1;_LL2:
 return _tmp4FD == _tmp4FC;case 1U: goto _LL3;default: _LL9: _tmp500=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->f1;_tmp4FF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->f1;_tmp4FE=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->f2;_LLA:
# 3135
 if(Cyc_Tcutil_kind_leq(_tmp500,_tmp4FE)){
({struct Cyc_Core_Opt*_tmpBE2=({struct Cyc_Core_Opt*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->v=c1;_tmp4F9;});*_tmp4FF=_tmpBE2;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->tag == 1U){_LL3: _tmp501=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->f1;_LL4:
# 3128
({struct Cyc_Core_Opt*_tmpBE3=({struct Cyc_Core_Opt*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->v=c1;_tmp4F6;});*_tmp501=_tmpBE3;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->tag == 1U){_LL5: _tmp502=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpBE4=({struct Cyc_Core_Opt*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->v=c2;_tmp4F7;});*_tmp502=_tmpBE4;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->tag == 0U){_LL7: _tmp505=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->f1;_tmp504=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->f2;_tmp503=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->f1;_LL8:
# 3131
 if(Cyc_Tcutil_kind_leq(_tmp503,_tmp504)){
({struct Cyc_Core_Opt*_tmpBE5=({struct Cyc_Core_Opt*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->v=c2;_tmp4F8;});*_tmp505=_tmpBE5;});return 1;}else{
return 0;}}else{_LLB: _tmp509=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->f1;_tmp508=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f1)->f2;_tmp507=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->f1;_tmp506=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F5.f2)->f2;_LLC:
# 3139
 if(Cyc_Tcutil_kind_leq(_tmp508,_tmp506)){
({struct Cyc_Core_Opt*_tmpBE6=({struct Cyc_Core_Opt*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->v=c1;_tmp4FA;});*_tmp507=_tmpBE6;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp506,_tmp508)){
({struct Cyc_Core_Opt*_tmpBE7=({struct Cyc_Core_Opt*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->v=c2;_tmp4FB;});*_tmp509=_tmpBE7;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3148
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3153
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp50F=({struct Cyc_Int_pa_PrintArg_struct _tmp9DA;_tmp9DA.tag=1U,_tmp9DA.f1=(unsigned long)i;_tmp9DA;});void*_tmp50D[1U];_tmp50D[0]=& _tmp50F;({struct _dyneither_ptr _tmpBE8=({const char*_tmp50E="#%d";_tag_dyneither(_tmp50E,sizeof(char),4U);});Cyc_aprintf(_tmpBE8,_tag_dyneither(_tmp50D,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp50C=_cycalloc(sizeof(*_tmp50C));({struct _dyneither_ptr*_tmpBE9=({unsigned int _tmp50B=1;struct _dyneither_ptr*_tmp50A=_cycalloc(_check_times(_tmp50B,sizeof(struct _dyneither_ptr)));_tmp50A[0]=s;_tmp50A;});_tmp50C->name=_tmpBE9;}),_tmp50C->identity=- 1,_tmp50C->kind=k;_tmp50C;});}
# 3160
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp510=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp510,sizeof(char),0))== '#';}
# 3165
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp513=({struct Cyc_String_pa_PrintArg_struct _tmp9DB;_tmp9DB.tag=0U,_tmp9DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmp9DB;});void*_tmp511[1U];_tmp511[0]=& _tmp513;({struct _dyneither_ptr _tmpBEA=({const char*_tmp512="%s";_tag_dyneither(_tmp512,sizeof(char),3U);});Cyc_printf(_tmpBEA,_tag_dyneither(_tmp511,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp514=({struct _dyneither_ptr _tmpBEB=({const char*_tmp51A="`";_tag_dyneither(_tmp51A,sizeof(char),2U);});Cyc_strconcat(_tmpBEB,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp515=_dyneither_ptr_plus(_tmp514,sizeof(char),1);char _tmp516=*((char*)_check_dyneither_subscript(_tmp515,sizeof(char),0U));char _tmp517='t';if(_get_dyneither_size(_tmp515,sizeof(char))== 1U  && (_tmp516 == '\000'  && _tmp517 != '\000'))_throw_arraybounds();*((char*)_tmp515.curr)=_tmp517;});
({struct _dyneither_ptr*_tmpBEC=({unsigned int _tmp519=1;struct _dyneither_ptr*_tmp518=_cycalloc(_check_times(_tmp519,sizeof(struct _dyneither_ptr)));_tmp518[0]=(struct _dyneither_ptr)_tmp514;_tmp518;});t->name=_tmpBEC;});};}
# 3174
static struct _tuple10*Cyc_Tcutil_fndecl2type_f(struct _tuple10*x){
# 3176
return({struct _tuple10*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->f1=(*x).f1,_tmp51B->f2=(*x).f2,_tmp51B->f3=(*x).f3;_tmp51B;});}
# 3179
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 3186
struct Cyc_List_List*_tmp51C=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpBED=({struct Cyc_List_List*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->hd=(void*)atts->hd,_tmp51D->tl=_tmp51C;_tmp51D;});_tmp51C=_tmpBED;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->tag=5U,(_tmp51E->f1).tvars=fd->tvs,(_tmp51E->f1).effect=fd->effect,(_tmp51E->f1).ret_tqual=fd->ret_tqual,(_tmp51E->f1).ret_type=fd->ret_type,({
struct Cyc_List_List*_tmpBEE=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2type_f,fd->args);(_tmp51E->f1).args=_tmpBEE;}),(_tmp51E->f1).c_varargs=fd->c_varargs,(_tmp51E->f1).cyc_varargs=fd->cyc_varargs,(_tmp51E->f1).rgn_po=fd->rgn_po,(_tmp51E->f1).attributes=_tmp51C,(_tmp51E->f1).requires_clause=fd->requires_clause,(_tmp51E->f1).requires_relns=fd->requires_relns,(_tmp51E->f1).ensures_clause=fd->ensures_clause,(_tmp51E->f1).ensures_relns=fd->ensures_relns;_tmp51E;});}
# 3199
return(void*)_check_null(fd->cached_type);}
# 3205
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3207
union Cyc_Relations_RelnOp _tmp51F=*rop;union Cyc_Relations_RelnOp _tmp520=_tmp51F;struct Cyc_Absyn_Vardecl*_tmp533;struct Cyc_Absyn_Vardecl*_tmp532;switch((_tmp520.RNumelts).tag){case 2U: _LL1: _tmp532=(_tmp520.RVar).val;_LL2: {
# 3209
struct _tuple2 _tmp521=*_tmp532->name;struct _tuple2 _tmp522=_tmp521;union Cyc_Absyn_Nmspace _tmp529;struct _dyneither_ptr*_tmp528;_LL8: _tmp529=_tmp522.f1;_tmp528=_tmp522.f2;_LL9:;
if(!((int)((_tmp529.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpBEF=(struct _dyneither_ptr)*_tmp528;Cyc_strcmp(_tmpBEF,({const char*_tmp523="return_value";_tag_dyneither(_tmp523,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpBF0=Cyc_Relations_RReturn();*rop=_tmpBF0;});
goto _LL0;}{
# 3215
unsigned int c=0U;
{struct Cyc_List_List*_tmp524=args;for(0;_tmp524 != 0;(_tmp524=_tmp524->tl,c ++)){
struct _tuple10*_tmp525=(struct _tuple10*)_tmp524->hd;struct _tuple10*_tmp526=_tmp525;struct _dyneither_ptr*_tmp527;_LLB: _tmp527=_tmp526->f1;_LLC:;
if(_tmp527 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp528,(struct _dyneither_ptr)*_tmp527)== 0){
({union Cyc_Relations_RelnOp _tmpBF1=Cyc_Relations_RParam(c);*rop=_tmpBF1;});
break;}}}}
# 3225
goto _LL0;};}case 3U: _LL3: _tmp533=(_tmp520.RNumelts).val;_LL4: {
# 3227
struct _tuple2 _tmp52A=*_tmp533->name;struct _tuple2 _tmp52B=_tmp52A;union Cyc_Absyn_Nmspace _tmp531;struct _dyneither_ptr*_tmp530;_LLE: _tmp531=_tmp52B.f1;_tmp530=_tmp52B.f2;_LLF:;
if(!((int)((_tmp531.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp52C=args;for(0;_tmp52C != 0;(_tmp52C=_tmp52C->tl,c ++)){
struct _tuple10*_tmp52D=(struct _tuple10*)_tmp52C->hd;struct _tuple10*_tmp52E=_tmp52D;struct _dyneither_ptr*_tmp52F;_LL11: _tmp52F=_tmp52E->f1;_LL12:;
if(_tmp52F != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp530,(struct _dyneither_ptr)*_tmp52F)== 0){
({union Cyc_Relations_RelnOp _tmpBF2=Cyc_Relations_RParamNumelts(c);*rop=_tmpBF2;});
break;}}}}
# 3239
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3244
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3246
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3250
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3252
if(e == 0)return 0;{
struct Cyc_List_List*_tmp534=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp534);
return _tmp534;};}struct _tuple27{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3259
static void*Cyc_Tcutil_fst_fdarg(struct _tuple27*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp535=pr;struct Cyc_Absyn_Tqual _tmp538;void*_tmp537;_LL1: _tmp538=_tmp535->f1;_tmp537=_tmp535->f2;_LL2:;
if(_tmp537 == t)return pr;else{
return({struct _tuple12*_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536->f1=_tmp538,_tmp536->f2=t;_tmp536;});}}struct _tuple28{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple29{struct _tuple28*f1;void*f2;};
# 3266
static struct _tuple29*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3268
return({struct _tuple29*_tmp53A=_region_malloc(rgn,sizeof(*_tmp53A));({struct _tuple28*_tmpBF3=({struct _tuple28*_tmp539=_region_malloc(rgn,sizeof(*_tmp539));_tmp539->f1=(*y).f1,_tmp539->f2=(*y).f2;_tmp539;});_tmp53A->f1=_tmpBF3;}),_tmp53A->f2=(*y).f3;_tmp53A;});}
# 3270
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3272
struct _tuple10 _tmp53B=*orig_arg;struct _tuple10 _tmp53C=_tmp53B;struct _dyneither_ptr*_tmp540;struct Cyc_Absyn_Tqual _tmp53F;void*_tmp53E;_LL1: _tmp540=_tmp53C.f1;_tmp53F=_tmp53C.f2;_tmp53E=_tmp53C.f3;_LL2:;
if(t == _tmp53E)return orig_arg;
return({struct _tuple10*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->f1=_tmp540,_tmp53D->f2=_tmp53F,_tmp53D->f3=t;_tmp53D;});}
# 3276
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3279
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3284
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3286
return({struct Cyc_Absyn_Exp*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->topt=old->topt,_tmp541->r=r,_tmp541->loc=old->loc,_tmp541->annot=old->annot;_tmp541;});}
# 3291
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp542=e->r;void*_tmp543=_tmp542;void*_tmp583;void*_tmp582;struct Cyc_List_List*_tmp581;struct Cyc_Absyn_Exp*_tmp580;struct Cyc_Absyn_Exp*_tmp57F;void*_tmp57E;void*_tmp57D;struct Cyc_Absyn_Exp*_tmp57C;int _tmp57B;enum Cyc_Absyn_Coercion _tmp57A;struct Cyc_Absyn_Exp*_tmp579;struct Cyc_Absyn_Exp*_tmp578;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Exp*_tmp573;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp571;enum Cyc_Absyn_Primop _tmp570;struct Cyc_List_List*_tmp56F;switch(*((int*)_tmp543)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp570=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp56F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLC:
# 3300
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56F)== 1){
struct Cyc_Absyn_Exp*_tmp544=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56F))->hd;
struct Cyc_Absyn_Exp*_tmp545=Cyc_Tcutil_rsubsexp(r,inst,_tmp544);
if(_tmp545 == _tmp544)return e;
return({struct Cyc_Absyn_Exp*_tmpBF5=e;Cyc_Tcutil_copye(_tmpBF5,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp547=_cycalloc(sizeof(*_tmp547));_tmp547->tag=3U,_tmp547->f1=_tmp570,({struct Cyc_List_List*_tmpBF4=({struct Cyc_Absyn_Exp*_tmp546[1U];_tmp546[0]=_tmp545;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp546,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp547->f2=_tmpBF4;});_tmp547;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56F)== 2){
struct Cyc_Absyn_Exp*_tmp548=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56F))->hd;
struct Cyc_Absyn_Exp*_tmp549=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56F->tl))->hd;
struct Cyc_Absyn_Exp*_tmp54A=Cyc_Tcutil_rsubsexp(r,inst,_tmp548);
struct Cyc_Absyn_Exp*_tmp54B=Cyc_Tcutil_rsubsexp(r,inst,_tmp549);
if(_tmp54A == _tmp548  && _tmp54B == _tmp549)return e;
return({struct Cyc_Absyn_Exp*_tmpBF7=e;Cyc_Tcutil_copye(_tmpBF7,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->tag=3U,_tmp54D->f1=_tmp570,({struct Cyc_List_List*_tmpBF6=({struct Cyc_Absyn_Exp*_tmp54C[2U];_tmp54C[0]=_tmp54A,_tmp54C[1]=_tmp54B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp54C,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp54D->f2=_tmpBF6;});_tmp54D;}));});}else{
return({void*_tmp54E=0U;({struct _dyneither_ptr _tmpBF8=({const char*_tmp54F="primop does not have 1 or 2 args!";_tag_dyneither(_tmp54F,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBF8,_tag_dyneither(_tmp54E,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp573=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp572=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp571=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_LLE: {
# 3314
struct Cyc_Absyn_Exp*_tmp550=Cyc_Tcutil_rsubsexp(r,inst,_tmp573);
struct Cyc_Absyn_Exp*_tmp551=Cyc_Tcutil_rsubsexp(r,inst,_tmp572);
struct Cyc_Absyn_Exp*_tmp552=Cyc_Tcutil_rsubsexp(r,inst,_tmp571);
if((_tmp550 == _tmp573  && _tmp551 == _tmp572) && _tmp552 == _tmp571)return e;
return({struct Cyc_Absyn_Exp*_tmpBF9=e;Cyc_Tcutil_copye(_tmpBF9,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=6U,_tmp553->f1=_tmp550,_tmp553->f2=_tmp551,_tmp553->f3=_tmp552;_tmp553;}));});}case 7U: _LLF: _tmp575=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp574=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL10: {
# 3320
struct Cyc_Absyn_Exp*_tmp554=Cyc_Tcutil_rsubsexp(r,inst,_tmp575);
struct Cyc_Absyn_Exp*_tmp555=Cyc_Tcutil_rsubsexp(r,inst,_tmp574);
if(_tmp554 == _tmp575  && _tmp555 == _tmp574)return e;
return({struct Cyc_Absyn_Exp*_tmpBFA=e;Cyc_Tcutil_copye(_tmpBFA,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->tag=7U,_tmp556->f1=_tmp554,_tmp556->f2=_tmp555;_tmp556;}));});}case 8U: _LL11: _tmp577=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp576=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL12: {
# 3325
struct Cyc_Absyn_Exp*_tmp557=Cyc_Tcutil_rsubsexp(r,inst,_tmp577);
struct Cyc_Absyn_Exp*_tmp558=Cyc_Tcutil_rsubsexp(r,inst,_tmp576);
if(_tmp557 == _tmp577  && _tmp558 == _tmp576)return e;
return({struct Cyc_Absyn_Exp*_tmpBFB=e;Cyc_Tcutil_copye(_tmpBFB,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->tag=8U,_tmp559->f1=_tmp557,_tmp559->f2=_tmp558;_tmp559;}));});}case 9U: _LL13: _tmp579=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp578=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL14: {
# 3330
struct Cyc_Absyn_Exp*_tmp55A=Cyc_Tcutil_rsubsexp(r,inst,_tmp579);
struct Cyc_Absyn_Exp*_tmp55B=Cyc_Tcutil_rsubsexp(r,inst,_tmp578);
if(_tmp55A == _tmp579  && _tmp55B == _tmp578)return e;
return({struct Cyc_Absyn_Exp*_tmpBFC=e;Cyc_Tcutil_copye(_tmpBFC,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->tag=9U,_tmp55C->f1=_tmp55A,_tmp55C->f2=_tmp55B;_tmp55C;}));});}case 14U: _LL15: _tmp57D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp57C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp57B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_tmp57A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f4;_LL16: {
# 3335
struct Cyc_Absyn_Exp*_tmp55D=Cyc_Tcutil_rsubsexp(r,inst,_tmp57C);
void*_tmp55E=Cyc_Tcutil_rsubstitute(r,inst,_tmp57D);
if(_tmp55D == _tmp57C  && _tmp55E == _tmp57D)return e;
return({struct Cyc_Absyn_Exp*_tmpBFD=e;Cyc_Tcutil_copye(_tmpBFD,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->tag=14U,_tmp55F->f1=_tmp55E,_tmp55F->f2=_tmp55D,_tmp55F->f3=_tmp57B,_tmp55F->f4=_tmp57A;_tmp55F;}));});}case 17U: _LL17: _tmp57E=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL18: {
# 3340
void*_tmp560=Cyc_Tcutil_rsubstitute(r,inst,_tmp57E);
if(_tmp560 == _tmp57E)return e;
return({struct Cyc_Absyn_Exp*_tmpBFE=e;Cyc_Tcutil_copye(_tmpBFE,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->tag=17U,_tmp561->f1=_tmp560;_tmp561;}));});}case 18U: _LL19: _tmp57F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL1A: {
# 3344
struct Cyc_Absyn_Exp*_tmp562=Cyc_Tcutil_rsubsexp(r,inst,_tmp57F);
if(_tmp562 == _tmp57F)return e;
return({struct Cyc_Absyn_Exp*_tmpBFF=e;Cyc_Tcutil_copye(_tmpBFF,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=18U,_tmp563->f1=_tmp562;_tmp563;}));});}case 41U: _LL1B: _tmp580=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL1C: {
# 3348
struct Cyc_Absyn_Exp*_tmp564=Cyc_Tcutil_rsubsexp(r,inst,_tmp580);
if(_tmp564 == _tmp580)return e;
return({struct Cyc_Absyn_Exp*_tmpC00=e;Cyc_Tcutil_copye(_tmpC00,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->tag=41U,_tmp565->f1=_tmp564;_tmp565;}));});}case 19U: _LL1D: _tmp582=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp581=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL1E: {
# 3352
void*_tmp566=Cyc_Tcutil_rsubstitute(r,inst,_tmp582);
if(_tmp566 == _tmp582)return e;
return({struct Cyc_Absyn_Exp*_tmpC01=e;Cyc_Tcutil_copye(_tmpC01,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->tag=19U,_tmp567->f1=_tmp566,_tmp567->f2=_tmp581;_tmp567;}));});}case 39U: _LL1F: _tmp583=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL20: {
# 3356
void*_tmp568=Cyc_Tcutil_rsubstitute(r,inst,_tmp583);
if(_tmp568 == _tmp583)return e;{
# 3359
void*_tmp569=Cyc_Tcutil_compress(_tmp568);void*_tmp56A=_tmp569;struct Cyc_Absyn_Exp*_tmp56C;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp56A)->tag == 9U){_LL24: _tmp56C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp56A)->f1;_LL25:
 return _tmp56C;}else{_LL26: _LL27:
# 3362
 return({struct Cyc_Absyn_Exp*_tmpC02=e;Cyc_Tcutil_copye(_tmpC02,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->tag=39U,_tmp56B->f1=_tmp568;_tmp56B;}));});}_LL23:;};}default: _LL21: _LL22:
# 3365
 return({void*_tmp56D=0U;({struct _dyneither_ptr _tmpC03=({const char*_tmp56E="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp56E,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC03,_tag_dyneither(_tmp56D,sizeof(void*),0U));});});}_LL0:;}
# 3369
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3372
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3376
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3379
void*_tmp584=f->type;
struct Cyc_Absyn_Exp*_tmp585=f->requires_clause;
void*_tmp586=Cyc_Tcutil_rsubstitute(r,inst,_tmp584);
struct Cyc_Absyn_Exp*_tmp587=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp585);
if(_tmp584 == _tmp586  && _tmp585 == _tmp587)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->name=f->name,_tmp588->tq=f->tq,_tmp588->type=_tmp586,_tmp588->width=f->width,_tmp588->attributes=f->attributes,_tmp588->requires_clause=_tmp587;_tmp588;});}}
# 3389
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3392
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp589=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp58A=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp58B=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp589);
struct Cyc_List_List*_tmp58C=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp58A);
if(_tmp58B == _tmp589  && _tmp58C == _tmp58A)return fs;
return({struct Cyc_List_List*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->hd=_tmp58B,_tmp58D->tl=_tmp58C;_tmp58D;});};}
# 3401
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3404
struct _tuple1 _tmp58E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp58F=_tmp58E;struct Cyc_List_List*_tmp593;struct Cyc_List_List*_tmp592;_LL1: _tmp593=_tmp58F.f1;_tmp592=_tmp58F.f2;_LL2:;{
struct Cyc_List_List*_tmp590=Cyc_Tcutil_substs(rgn,inst,_tmp593);
struct Cyc_List_List*_tmp591=Cyc_Tcutil_substs(rgn,inst,_tmp592);
if(_tmp590 == _tmp593  && _tmp591 == _tmp592)
return rgn_po;else{
# 3410
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp590,_tmp591);}};}
# 3413
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3416
void*_tmp594=Cyc_Tcutil_compress(t);void*_tmp595=_tmp594;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp*_tmp5EC;void*_tmp5EB;struct Cyc_List_List*_tmp5EA;void*_tmp5E9;void*_tmp5E8;enum Cyc_Absyn_AggrKind _tmp5E7;struct Cyc_List_List*_tmp5E6;struct Cyc_List_List*_tmp5E5;struct Cyc_List_List*_tmp5E4;void*_tmp5E3;struct Cyc_Absyn_Tqual _tmp5E2;void*_tmp5E1;struct Cyc_List_List*_tmp5E0;int _tmp5DF;struct Cyc_Absyn_VarargInfo*_tmp5DE;struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DA;void*_tmp5D9;struct Cyc_Absyn_Tqual _tmp5D8;void*_tmp5D7;void*_tmp5D6;void*_tmp5D5;void*_tmp5D4;void*_tmp5D3;struct Cyc_Absyn_Tqual _tmp5D2;struct Cyc_Absyn_Exp*_tmp5D1;void*_tmp5D0;unsigned int _tmp5CF;struct _tuple2*_tmp5CE;struct Cyc_List_List*_tmp5CD;struct Cyc_Absyn_Typedefdecl*_tmp5CC;void*_tmp5CB;struct Cyc_Absyn_Tvar*_tmp5CA;switch(*((int*)_tmp595)){case 2U: _LL1: _tmp5CA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp595)->f1;_LL2: {
# 3419
struct _handler_cons _tmp596;_push_handler(& _tmp596);{int _tmp598=0;if(setjmp(_tmp596.handler))_tmp598=1;if(!_tmp598){{void*_tmp599=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp5CA);_npop_handler(0U);return _tmp599;};_pop_handler();}else{void*_tmp597=(void*)_exn_thrown;void*_tmp59A=_tmp597;void*_tmp59B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp59A)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp59B=_tmp59A;_LL1F:(int)_rethrow(_tmp59B);}_LL1B:;}};}case 8U: _LL3: _tmp5CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp595)->f1;_tmp5CD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp595)->f2;_tmp5CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp595)->f3;_tmp5CB=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp595)->f4;_LL4: {
# 3422
struct Cyc_List_List*_tmp59C=Cyc_Tcutil_substs(rgn,inst,_tmp5CD);
return _tmp59C == _tmp5CD?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->tag=8U,_tmp59D->f1=_tmp5CE,_tmp59D->f2=_tmp59C,_tmp59D->f3=_tmp5CC,_tmp59D->f4=_tmp5CB;_tmp59D;});}case 4U: _LL5: _tmp5D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp595)->f1).elt_type;_tmp5D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp595)->f1).tq;_tmp5D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp595)->f1).num_elts;_tmp5D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp595)->f1).zero_term;_tmp5CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp595)->f1).zt_loc;_LL6: {
# 3425
void*_tmp59E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D3);
struct Cyc_Absyn_Exp*_tmp59F=_tmp5D1 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5D1);
void*_tmp5A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D0);
return(_tmp59E == _tmp5D3  && _tmp59F == _tmp5D1) && _tmp5A0 == _tmp5D0?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));
_tmp5A1->tag=4U,(_tmp5A1->f1).elt_type=_tmp59E,(_tmp5A1->f1).tq=_tmp5D2,(_tmp5A1->f1).num_elts=_tmp59F,(_tmp5A1->f1).zero_term=_tmp5A0,(_tmp5A1->f1).zt_loc=_tmp5CF;_tmp5A1;});}case 3U: _LL7: _tmp5D9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).elt_type;_tmp5D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).elt_tq;_tmp5D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).rgn;_tmp5D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).nullable;_tmp5D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).bounds;_tmp5D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595)->f1).ptr_atts).zero_term;_LL8: {
# 3431
void*_tmp5A2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D9);
void*_tmp5A3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D7);
void*_tmp5A4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D5);
void*_tmp5A5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D4);
if(((_tmp5A2 == _tmp5D9  && _tmp5A3 == _tmp5D7) && _tmp5A4 == _tmp5D5) && _tmp5A5 == _tmp5D4)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9DC;_tmp9DC.elt_type=_tmp5A2,_tmp9DC.elt_tq=_tmp5D8,(_tmp9DC.ptr_atts).rgn=_tmp5A3,(_tmp9DC.ptr_atts).nullable=_tmp5D6,(_tmp9DC.ptr_atts).bounds=_tmp5A4,(_tmp9DC.ptr_atts).zero_term=_tmp5A5,(_tmp9DC.ptr_atts).ptrloc=0;_tmp9DC;}));}case 5U: _LL9: _tmp5E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).tvars;_tmp5E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).effect;_tmp5E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).ret_tqual;_tmp5E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).ret_type;_tmp5E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).args;_tmp5DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).c_varargs;_tmp5DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).cyc_varargs;_tmp5DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).rgn_po;_tmp5DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).attributes;_tmp5DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).requires_clause;_tmp5DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595)->f1).ensures_clause;_LLA:
# 3441
{struct Cyc_List_List*_tmp5A6=_tmp5E4;for(0;_tmp5A6 != 0;_tmp5A6=_tmp5A6->tl){
({struct Cyc_List_List*_tmpC06=({struct Cyc_List_List*_tmp5A8=_region_malloc(rgn,sizeof(*_tmp5A8));({struct _tuple15*_tmpC05=({struct _tuple15*_tmp5A7=_region_malloc(rgn,sizeof(*_tmp5A7));_tmp5A7->f1=(struct Cyc_Absyn_Tvar*)_tmp5A6->hd,({void*_tmpC04=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp5A6->hd);_tmp5A7->f2=_tmpC04;});_tmp5A7;});_tmp5A8->hd=_tmpC05;}),_tmp5A8->tl=inst;_tmp5A8;});inst=_tmpC06;});}}{
struct _tuple1 _tmp5A9=({struct _RegionHandle*_tmpC08=rgn;struct _RegionHandle*_tmpC07=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpC08,_tmpC07,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple29*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp5E0));});
# 3443
struct _tuple1 _tmp5AA=_tmp5A9;struct Cyc_List_List*_tmp5B9;struct Cyc_List_List*_tmp5B8;_LL21: _tmp5B9=_tmp5AA.f1;_tmp5B8=_tmp5AA.f2;_LL22:;{
# 3445
struct Cyc_List_List*_tmp5AB=_tmp5E0;
struct Cyc_List_List*_tmp5AC=Cyc_Tcutil_substs(rgn,inst,_tmp5B8);
if(_tmp5AC != _tmp5B8)
({struct Cyc_List_List*_tmpC09=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp5E0,_tmp5AC);_tmp5AB=_tmpC09;});{
void*eff2;
if(_tmp5E3 == 0)
eff2=0;else{
# 3453
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E3);
if(new_eff == _tmp5E3)
eff2=_tmp5E3;else{
# 3457
eff2=new_eff;}}{
# 3459
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp5DE == 0)
cyc_varargs2=0;else{
# 3463
struct Cyc_Absyn_VarargInfo _tmp5AD=*_tmp5DE;struct Cyc_Absyn_VarargInfo _tmp5AE=_tmp5AD;struct _dyneither_ptr*_tmp5B4;struct Cyc_Absyn_Tqual _tmp5B3;void*_tmp5B2;int _tmp5B1;_LL24: _tmp5B4=_tmp5AE.name;_tmp5B3=_tmp5AE.tq;_tmp5B2=_tmp5AE.type;_tmp5B1=_tmp5AE.inject;_LL25:;{
void*_tmp5AF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B2);
if(_tmp5AF == _tmp5B2)cyc_varargs2=_tmp5DE;else{
# 3467
({struct Cyc_Absyn_VarargInfo*_tmpC0A=({struct Cyc_Absyn_VarargInfo*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->name=_tmp5B4,_tmp5B0->tq=_tmp5B3,_tmp5B0->type=_tmp5AF,_tmp5B0->inject=_tmp5B1;_tmp5B0;});cyc_varargs2=_tmpC0A;});}};}{
# 3469
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5DD);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5DB);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5DA);
struct Cyc_List_List*_tmp5B5=Cyc_Tcutil_extract_relns(_tmp5AB,req2);
struct Cyc_List_List*_tmp5B6=Cyc_Tcutil_extract_relns(_tmp5AB,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));
_tmp5B7->tag=5U,(_tmp5B7->f1).tvars=_tmp5E4,(_tmp5B7->f1).effect=eff2,(_tmp5B7->f1).ret_tqual=_tmp5E2,({void*_tmpC0B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E1);(_tmp5B7->f1).ret_type=_tmpC0B;}),(_tmp5B7->f1).args=_tmp5AB,(_tmp5B7->f1).c_varargs=_tmp5DF,(_tmp5B7->f1).cyc_varargs=cyc_varargs2,(_tmp5B7->f1).rgn_po=rgn_po2,(_tmp5B7->f1).attributes=_tmp5DC,(_tmp5B7->f1).requires_clause=req2,(_tmp5B7->f1).requires_relns=_tmp5B5,(_tmp5B7->f1).ensures_clause=ens2,(_tmp5B7->f1).ensures_relns=_tmp5B6;_tmp5B7;});};};};};};case 6U: _LLB: _tmp5E5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp595)->f1;_LLC: {
# 3479
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp5BA=_tmp5E5;for(0;_tmp5BA != 0;_tmp5BA=_tmp5BA->tl){
void*_tmp5BB=(*((struct _tuple12*)_tmp5BA->hd)).f2;
void*_tmp5BC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5BB);
if(_tmp5BB != _tmp5BC)
change=1;
# 3487
({struct Cyc_List_List*_tmpC0C=({struct Cyc_List_List*_tmp5BD=_region_malloc(rgn,sizeof(*_tmp5BD));_tmp5BD->hd=_tmp5BC,_tmp5BD->tl=ts2;_tmp5BD;});ts2=_tmpC0C;});}}
# 3489
if(!change)
return t;{
struct Cyc_List_List*_tmp5BE=({struct Cyc_List_List*_tmpC0D=_tmp5E5;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpC0D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->tag=6U,_tmp5BF->f1=_tmp5BE;_tmp5BF;});};}case 7U: _LLD: _tmp5E7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp595)->f1;_tmp5E6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp595)->f2;_LLE: {
# 3494
struct Cyc_List_List*_tmp5C0=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp5E6);
if(_tmp5E6 == _tmp5C0)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->tag=7U,_tmp5C1->f1=_tmp5E7,_tmp5C1->f2=_tmp5C0;_tmp5C1;});}case 1U: _LLF: _tmp5E8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp595)->f2;_LL10:
# 3498
 if(_tmp5E8 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E8);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp595)->f2 == 0){_LL11: _tmp5E9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp595)->f1;_LL12:
 return t;}else{_LL13: _tmp5EB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp595)->f1;_tmp5EA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp595)->f2;_LL14: {
# 3502
struct Cyc_List_List*_tmp5C2=Cyc_Tcutil_substs(rgn,inst,_tmp5EA);
if(_tmp5EA == _tmp5C2)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->tag=0U,_tmp5C3->f1=_tmp5EB,_tmp5C3->f2=_tmp5C2;_tmp5C3;});}}}case 9U: _LL15: _tmp5EC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp595)->f1;_LL16: {
# 3506
struct Cyc_Absyn_Exp*_tmp5C4=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5EC);
return _tmp5C4 == _tmp5EC?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->tag=9U,_tmp5C5->f1=_tmp5C4;_tmp5C5;});}case 11U: _LL17: _tmp5ED=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp595)->f1;_LL18: {
# 3509
struct Cyc_Absyn_Exp*_tmp5C6=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5ED);
return _tmp5C6 == _tmp5ED?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->tag=11U,_tmp5C7->f1=_tmp5C6;_tmp5C7;});}default: _LL19: _LL1A:
({void*_tmp5C8=0U;({struct _dyneither_ptr _tmpC0E=({const char*_tmp5C9="found typedecltype in rsubs";_tag_dyneither(_tmp5C9,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0E,_tag_dyneither(_tmp5C8,sizeof(void*),0U));});});}_LL0:;}
# 3515
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3518
if(ts == 0)
return 0;{
void*_tmp5EE=(void*)ts->hd;
struct Cyc_List_List*_tmp5EF=ts->tl;
void*_tmp5F0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5EE);
struct Cyc_List_List*_tmp5F1=Cyc_Tcutil_substs(rgn,inst,_tmp5EF);
if(_tmp5EE == _tmp5F0  && _tmp5EF == _tmp5F1)
return ts;
return({struct Cyc_List_List*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->hd=_tmp5F0,_tmp5F2->tl=_tmp5F1;_tmp5F2;});};}
# 3529
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3536
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp5F3=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->f1=tv,({void*_tmpC10=({struct Cyc_Core_Opt*_tmpC0F=_tmp5F3;Cyc_Absyn_new_evar(_tmpC0F,({struct Cyc_Core_Opt*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->v=s;_tmp5F4;}));});_tmp5F5->f2=_tmpC10;});_tmp5F5;});}
# 3541
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,struct Cyc_Absyn_Tvar*tv){
# 3543
struct _tuple16*_tmp5F6=env;struct Cyc_List_List*_tmp5FB;struct _RegionHandle*_tmp5FA;_LL1: _tmp5FB=_tmp5F6->f1;_tmp5FA=_tmp5F6->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp5F7=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F9=_region_malloc(_tmp5FA,sizeof(*_tmp5F9));_tmp5F9->f1=tv,({void*_tmpC12=({struct Cyc_Core_Opt*_tmpC11=_tmp5F7;Cyc_Absyn_new_evar(_tmpC11,({struct Cyc_Core_Opt*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->v=_tmp5FB;_tmp5F8;}));});_tmp5F9->f2=_tmpC12;});_tmp5F9;});};}
# 3553
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3557
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp5FE=({struct Cyc_String_pa_PrintArg_struct _tmp9DF;_tmp9DF.tag=0U,_tmp9DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmp9DF;});struct Cyc_String_pa_PrintArg_struct _tmp5FF=({struct Cyc_String_pa_PrintArg_struct _tmp9DE;_tmp9DE.tag=0U,({
struct _dyneither_ptr _tmpC13=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp9DE.f1=_tmpC13;});_tmp9DE;});struct Cyc_String_pa_PrintArg_struct _tmp600=({struct Cyc_String_pa_PrintArg_struct _tmp9DD;_tmp9DD.tag=0U,({struct _dyneither_ptr _tmpC14=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmp9DD.f1=_tmpC14;});_tmp9DD;});void*_tmp5FC[3U];_tmp5FC[0]=& _tmp5FE,_tmp5FC[1]=& _tmp5FF,_tmp5FC[2]=& _tmp600;({unsigned int _tmpC16=loc;struct _dyneither_ptr _tmpC15=({const char*_tmp5FD="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp5FD,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpC16,_tmpC15,_tag_dyneither(_tmp5FC,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp601=0U;({struct _dyneither_ptr _tmpC17=({const char*_tmp602="same type variable has different identity!";_tag_dyneither(_tmp602,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC17,_tag_dyneither(_tmp601,sizeof(void*),0U));});});}
return tvs;}}}
# 3571
({int _tmpC18=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpC18;});
return({struct Cyc_List_List*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->hd=tv,_tmp603->tl=tvs;_tmp603;});}
# 3577
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3579
if(tv->identity == - 1)
({void*_tmp604=0U;({struct _dyneither_ptr _tmpC19=({const char*_tmp605="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp605,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC19,_tag_dyneither(_tmp604,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3583
struct Cyc_Absyn_Tvar*_tmp606=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp606->identity == - 1)
({void*_tmp607=0U;({struct _dyneither_ptr _tmpC1A=({const char*_tmp608="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp608,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC1A,_tag_dyneither(_tmp607,sizeof(void*),0U));});});
if(_tmp606->identity == tv->identity)
return tvs;}}
# 3590
return({struct Cyc_List_List*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->hd=tv,_tmp609->tl=tvs;_tmp609;});}struct _tuple30{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3596
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3601
if(tv->identity == - 1)
({void*_tmp60A=0U;({struct _dyneither_ptr _tmpC1B=({const char*_tmp60B="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp60B,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC1B,_tag_dyneither(_tmp60A,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3605
struct _tuple30*_tmp60C=(struct _tuple30*)tvs2->hd;struct _tuple30*_tmp60D=_tmp60C;struct Cyc_Absyn_Tvar*_tmp611;int*_tmp610;_LL1: _tmp611=_tmp60D->f1;_tmp610=(int*)& _tmp60D->f2;_LL2:;
if(_tmp611->identity == - 1)
({void*_tmp60E=0U;({struct _dyneither_ptr _tmpC1C=({const char*_tmp60F="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp60F,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC1C,_tag_dyneither(_tmp60E,sizeof(void*),0U));});});
if(_tmp611->identity == tv->identity){
*_tmp610=*_tmp610  || b;
return tvs;}}}
# 3613
return({struct Cyc_List_List*_tmp613=_region_malloc(r,sizeof(*_tmp613));({struct _tuple30*_tmpC1D=({struct _tuple30*_tmp612=_region_malloc(r,sizeof(*_tmp612));_tmp612->f1=tv,_tmp612->f2=b;_tmp612;});_tmp613->hd=_tmpC1D;}),_tmp613->tl=tvs;_tmp613;});}
# 3617
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3619
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp616=({struct Cyc_String_pa_PrintArg_struct _tmp9E0;_tmp9E0.tag=0U,({struct _dyneither_ptr _tmpC1E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp9E0.f1=_tmpC1E;});_tmp9E0;});void*_tmp614[1U];_tmp614[0]=& _tmp616;({struct _dyneither_ptr _tmpC1F=({const char*_tmp615="bound tvar id for %s is NULL";_tag_dyneither(_tmp615,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC1F,_tag_dyneither(_tmp614,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->hd=tv,_tmp617->tl=tvs;_tmp617;});}struct _tuple31{void*f1;int f2;};
# 3628
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3631
void*_tmp618=Cyc_Tcutil_compress(e);void*_tmp619=_tmp618;int _tmp623;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp619)->tag == 1U){_LL1: _tmp623=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp619)->f3;_LL2:
# 3633
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple31*_tmp61A=(struct _tuple31*)es2->hd;struct _tuple31*_tmp61B=_tmp61A;void*_tmp620;int*_tmp61F;_LL6: _tmp620=_tmp61B->f1;_tmp61F=(int*)& _tmp61B->f2;_LL7:;{
void*_tmp61C=Cyc_Tcutil_compress(_tmp620);void*_tmp61D=_tmp61C;int _tmp61E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61D)->tag == 1U){_LL9: _tmp61E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61D)->f3;_LLA:
# 3637
 if(_tmp623 == _tmp61E){
if(b != *_tmp61F)*_tmp61F=1;
return es;}
# 3641
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3645
return({struct Cyc_List_List*_tmp622=_region_malloc(r,sizeof(*_tmp622));({struct _tuple31*_tmpC20=({struct _tuple31*_tmp621=_region_malloc(r,sizeof(*_tmp621));_tmp621->f1=e,_tmp621->f2=b;_tmp621;});_tmp622->hd=_tmpC20;}),_tmp622->tl=es;_tmp622;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3650
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3653
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3662
if(!present)({struct Cyc_List_List*_tmpC21=({struct Cyc_List_List*_tmp624=_region_malloc(rgn,sizeof(*_tmp624));_tmp624->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp624->tl=r;_tmp624;});r=_tmpC21;});}
# 3664
({struct Cyc_List_List*_tmpC22=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpC22;});
return r;}
# 3669
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3673
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple30 _tmp625=*((struct _tuple30*)tvs->hd);struct _tuple30 _tmp626=_tmp625;struct Cyc_Absyn_Tvar*_tmp629;int _tmp628;_LL1: _tmp629=_tmp626.f1;_tmp628=_tmp626.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp629->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3683
if(!present)({struct Cyc_List_List*_tmpC23=({struct Cyc_List_List*_tmp627=_region_malloc(r,sizeof(*_tmp627));_tmp627->hd=(struct _tuple30*)tvs->hd,_tmp627->tl=res;_tmp627;});res=_tmpC23;});};}
# 3685
({struct Cyc_List_List*_tmpC24=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpC24;});
return res;}
# 3689
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_type,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3691
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp62C=({struct Cyc_String_pa_PrintArg_struct _tmp9E1;_tmp9E1.tag=0U,_tmp9E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9E1;});void*_tmp62A[1U];_tmp62A[0]=& _tmp62C;({unsigned int _tmpC26=loc;struct _dyneither_ptr _tmpC25=({const char*_tmp62B="bitfield %s does not have constant width";_tag_dyneither(_tmp62B,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpC26,_tmpC25,_tag_dyneither(_tmp62A,sizeof(void*),1U));});});else{
# 3696
struct _tuple13 _tmp62D=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _tmp62E=_tmp62D;unsigned int _tmp634;int _tmp633;_LL1: _tmp634=_tmp62E.f1;_tmp633=_tmp62E.f2;_LL2:;
if(!_tmp633)
({void*_tmp62F=0U;({unsigned int _tmpC28=loc;struct _dyneither_ptr _tmpC27=({const char*_tmp630="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp630,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpC28,_tmpC27,_tag_dyneither(_tmp62F,sizeof(void*),0U));});});
if((int)_tmp634 < 0)
({void*_tmp631=0U;({unsigned int _tmpC2A=loc;struct _dyneither_ptr _tmpC29=({const char*_tmp632="bitfield has negative width";_tag_dyneither(_tmp632,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpC2A,_tmpC29,_tag_dyneither(_tmp631,sizeof(void*),0U));});});
w=_tmp634;}{
# 3703
void*_tmp635=Cyc_Tcutil_compress(field_type);void*_tmp636=_tmp635;enum Cyc_Absyn_Size_of _tmp644;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp636)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp636)->f1)->tag == 1U){_LL4: _tmp644=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp636)->f1)->f2;_LL5:
# 3706
{enum Cyc_Absyn_Size_of _tmp637=_tmp644;switch(_tmp637){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > 8)({void*_tmp638=0U;({unsigned int _tmpC2C=loc;struct _dyneither_ptr _tmpC2B=({const char*_tmp639="bitfield larger than type";_tag_dyneither(_tmp639,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC2C,_tmpC2B,_tag_dyneither(_tmp638,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > 16)({void*_tmp63A=0U;({unsigned int _tmpC2E=loc;struct _dyneither_ptr _tmpC2D=({const char*_tmp63B="bitfield larger than type";_tag_dyneither(_tmp63B,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC2E,_tmpC2D,_tag_dyneither(_tmp63A,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3711
 if(w > 32)({void*_tmp63C=0U;({unsigned int _tmpC30=loc;struct _dyneither_ptr _tmpC2F=({const char*_tmp63D="bitfield larger than type";_tag_dyneither(_tmp63D,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC30,_tmpC2F,_tag_dyneither(_tmp63C,sizeof(void*),0U));});});goto _LL8;default: _LL11: _LL12:
# 3713
 if(w > 64)({void*_tmp63E=0U;({unsigned int _tmpC32=loc;struct _dyneither_ptr _tmpC31=({const char*_tmp63F="bitfield larger than type";_tag_dyneither(_tmp63F,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC32,_tmpC31,_tag_dyneither(_tmp63E,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3715
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 3717
({struct Cyc_String_pa_PrintArg_struct _tmp642=({struct Cyc_String_pa_PrintArg_struct _tmp9E3;_tmp9E3.tag=0U,_tmp9E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9E3;});struct Cyc_String_pa_PrintArg_struct _tmp643=({struct Cyc_String_pa_PrintArg_struct _tmp9E2;_tmp9E2.tag=0U,({
struct _dyneither_ptr _tmpC33=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_type));_tmp9E2.f1=_tmpC33;});_tmp9E2;});void*_tmp640[2U];_tmp640[0]=& _tmp642,_tmp640[1]=& _tmp643;({unsigned int _tmpC35=loc;struct _dyneither_ptr _tmpC34=({const char*_tmp641="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp641,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpC35,_tmpC34,_tag_dyneither(_tmp640,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3724
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp645=(void*)atts->hd;void*_tmp646=_tmp645;switch(*((int*)_tmp646)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp649=({struct Cyc_String_pa_PrintArg_struct _tmp9E5;_tmp9E5.tag=0U,({
struct _dyneither_ptr _tmpC36=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp9E5.f1=_tmpC36;});_tmp9E5;});struct Cyc_String_pa_PrintArg_struct _tmp64A=({struct Cyc_String_pa_PrintArg_struct _tmp9E4;_tmp9E4.tag=0U,_tmp9E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9E4;});void*_tmp647[2U];_tmp647[0]=& _tmp649,_tmp647[1]=& _tmp64A;({unsigned int _tmpC38=loc;struct _dyneither_ptr _tmpC37=({const char*_tmp648="bad attribute %s on member %s";_tag_dyneither(_tmp648,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpC38,_tmpC37,_tag_dyneither(_tmp647,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3752
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp64B=t;struct Cyc_Absyn_Typedefdecl*_tmp64F;void*_tmp64E;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp64B)->tag == 8U){_LL1: _tmp64F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp64B)->f3;_tmp64E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp64B)->f4;_LL2:
# 3755
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp64F))->tq).real_const  || (_tmp64F->tq).print_const){
if(declared_const)({void*_tmp64C=0U;({unsigned int _tmpC3A=loc;struct _dyneither_ptr _tmpC39=({const char*_tmp64D="extra const";_tag_dyneither(_tmp64D,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpC3A,_tmpC39,_tag_dyneither(_tmp64C,sizeof(void*),0U));});});
return 1;}
# 3760
if((unsigned int)_tmp64E)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp64E);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3767
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp650=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp651=_tmp650;void*_tmp655;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp651)->tag == 3U){_LL1: _tmp655=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp651)->f1).ptr_atts).rgn;_LL2:
# 3772
{void*_tmp652=Cyc_Tcutil_compress(_tmp655);void*_tmp653=_tmp652;struct Cyc_Absyn_Tvar*_tmp654;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp653)->tag == 2U){_LL6: _tmp654=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp653)->f1;_LL7:
# 3774
 return Cyc_Absyn_tvar_cmp(tvar,_tmp654)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3777
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3783
return 1;}
return 0;}
# 3787
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3790
void*_tmp656=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp657=_tmp656;switch(*((int*)_tmp657)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp657)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp657)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp657)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp657)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3799
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3802
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3808
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3814
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple32{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3818
static struct _tuple32 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3821
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp65A=({struct Cyc_String_pa_PrintArg_struct _tmp9E7;_tmp9E7.tag=0U,_tmp9E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9E7;});struct Cyc_String_pa_PrintArg_struct _tmp65B=({struct Cyc_String_pa_PrintArg_struct _tmp9E6;_tmp9E6.tag=0U,({
struct _dyneither_ptr _tmpC3B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp9E6.f1=_tmpC3B;});_tmp9E6;});void*_tmp658[2U];_tmp658[0]=& _tmp65A,_tmp658[1]=& _tmp65B;({unsigned int _tmpC3D=loc;struct _dyneither_ptr _tmpC3C=({const char*_tmp659="%s clause has type %s instead of integral type";_tag_dyneither(_tmp659,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpC3D,_tmpC3C,_tag_dyneither(_tmp658,sizeof(void*),2U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC3E=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpC3E;});
({struct Cyc_List_List*_tmpC3F=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpC3F;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp65E=({struct Cyc_String_pa_PrintArg_struct _tmp9E9;_tmp9E9.tag=0U,_tmp9E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9E9;});struct Cyc_String_pa_PrintArg_struct _tmp65F=({struct Cyc_String_pa_PrintArg_struct _tmp9E8;_tmp9E8.tag=0U,({
struct _dyneither_ptr _tmpC40=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmp9E8.f1=_tmpC40;});_tmp9E8;});void*_tmp65C[2U];_tmp65C[0]=& _tmp65E,_tmp65C[1]=& _tmp65F;({unsigned int _tmpC42=clause->loc;struct _dyneither_ptr _tmpC41=({const char*_tmp65D="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp65D,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpC42,_tmpC41,_tag_dyneither(_tmp65C,sizeof(void*),2U));});});}
# 3833
return({struct _tuple32 _tmp9EA;_tmp9EA.f1=cvtenv,_tmp9EA.f2=relns;_tmp9EA;});}
# 3863 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr);
# 3870
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_aggr(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 3876
{union Cyc_Absyn_AggrInfo _tmp660=*info;union Cyc_Absyn_AggrInfo _tmp661=_tmp660;struct Cyc_Absyn_Aggrdecl*_tmp684;enum Cyc_Absyn_AggrKind _tmp683;struct _tuple2*_tmp682;struct Cyc_Core_Opt*_tmp681;if((_tmp661.UnknownAggr).tag == 1){_LL1: _tmp683=((_tmp661.UnknownAggr).val).f1;_tmp682=((_tmp661.UnknownAggr).val).f2;_tmp681=((_tmp661.UnknownAggr).val).f3;_LL2: {
# 3878
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp662;_push_handler(& _tmp662);{int _tmp664=0;if(setjmp(_tmp662.handler))_tmp664=1;if(!_tmp664){
({struct Cyc_Absyn_Aggrdecl**_tmpC43=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp682);adp=_tmpC43;});{
struct Cyc_Absyn_Aggrdecl*_tmp665=*adp;
if(_tmp665->kind != _tmp683){
if(_tmp665->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp668=({struct Cyc_String_pa_PrintArg_struct _tmp9EC;_tmp9EC.tag=0U,({
struct _dyneither_ptr _tmpC44=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp682));_tmp9EC.f1=_tmpC44;});_tmp9EC;});struct Cyc_String_pa_PrintArg_struct _tmp669=({struct Cyc_String_pa_PrintArg_struct _tmp9EB;_tmp9EB.tag=0U,({struct _dyneither_ptr _tmpC45=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp682));_tmp9EB.f1=_tmpC45;});_tmp9EB;});void*_tmp666[2U];_tmp666[0]=& _tmp668,_tmp666[1]=& _tmp669;({unsigned int _tmpC47=loc;struct _dyneither_ptr _tmpC46=({const char*_tmp667="expecting struct %s instead of union %s";_tag_dyneither(_tmp667,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC47,_tmpC46,_tag_dyneither(_tmp666,sizeof(void*),2U));});});else{
# 3887
({struct Cyc_String_pa_PrintArg_struct _tmp66C=({struct Cyc_String_pa_PrintArg_struct _tmp9EE;_tmp9EE.tag=0U,({
struct _dyneither_ptr _tmpC48=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp682));_tmp9EE.f1=_tmpC48;});_tmp9EE;});struct Cyc_String_pa_PrintArg_struct _tmp66D=({struct Cyc_String_pa_PrintArg_struct _tmp9ED;_tmp9ED.tag=0U,({struct _dyneither_ptr _tmpC49=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp682));_tmp9ED.f1=_tmpC49;});_tmp9ED;});void*_tmp66A[2U];_tmp66A[0]=& _tmp66C,_tmp66A[1]=& _tmp66D;({unsigned int _tmpC4B=loc;struct _dyneither_ptr _tmpC4A=({const char*_tmp66B="expecting union %s instead of struct %s";_tag_dyneither(_tmp66B,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC4B,_tmpC4A,_tag_dyneither(_tmp66A,sizeof(void*),2U));});});}}
# 3890
if((unsigned int)_tmp681  && (int)_tmp681->v){
if(!((unsigned int)_tmp665->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp665->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp670=({struct Cyc_String_pa_PrintArg_struct _tmp9EF;_tmp9EF.tag=0U,({
struct _dyneither_ptr _tmpC4C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp682));_tmp9EF.f1=_tmpC4C;});_tmp9EF;});void*_tmp66E[1U];_tmp66E[0]=& _tmp670;({unsigned int _tmpC4E=loc;struct _dyneither_ptr _tmpC4D=({const char*_tmp66F="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp66F,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpC4E,_tmpC4D,_tag_dyneither(_tmp66E,sizeof(void*),1U));});});}
# 3896
({union Cyc_Absyn_AggrInfo _tmpC4F=Cyc_Absyn_KnownAggr(adp);*info=_tmpC4F;});};
# 3880
;_pop_handler();}else{void*_tmp663=(void*)_exn_thrown;void*_tmp671=_tmp663;void*_tmp677;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp671)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3900
struct Cyc_Absyn_Aggrdecl*_tmp672=({struct Cyc_Absyn_Aggrdecl*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->kind=_tmp683,_tmp676->sc=Cyc_Absyn_Extern,_tmp676->name=_tmp682,_tmp676->tvs=0,_tmp676->impl=0,_tmp676->attributes=0,_tmp676->expected_mem_kind=0;_tmp676;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp672);
({struct Cyc_Absyn_Aggrdecl**_tmpC50=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp682);adp=_tmpC50;});
({union Cyc_Absyn_AggrInfo _tmpC51=Cyc_Absyn_KnownAggr(adp);*info=_tmpC51;});
# 3905
if(*targs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp675=({struct Cyc_String_pa_PrintArg_struct _tmp9F0;_tmp9F0.tag=0U,({struct _dyneither_ptr _tmpC52=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp682));_tmp9F0.f1=_tmpC52;});_tmp9F0;});void*_tmp673[1U];_tmp673[0]=& _tmp675;({unsigned int _tmpC54=loc;struct _dyneither_ptr _tmpC53=({const char*_tmp674="declare parameterized type %s before using";_tag_dyneither(_tmp674,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpC54,_tmpC53,_tag_dyneither(_tmp673,sizeof(void*),1U));});});
return cvtenv;}
# 3909
goto _LL5;}}else{_LL8: _tmp677=_tmp671;_LL9:(int)_rethrow(_tmp677);}_LL5:;}};}
# 3911
_tmp684=*adp;goto _LL4;}}else{_LL3: _tmp684=*(_tmp661.KnownAggr).val;_LL4: {
# 3913
struct Cyc_List_List*tvs=_tmp684->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp678=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp679=(void*)ts->hd;
# 3921
{struct _tuple0 _tmp67A=({struct _tuple0 _tmp9F1;({void*_tmpC55=Cyc_Absyn_compress_kb(_tmp678->kind);_tmp9F1.f1=_tmpC55;}),_tmp9F1.f2=_tmp679;_tmp9F1;});struct _tuple0 _tmp67B=_tmp67A;struct Cyc_Absyn_Tvar*_tmp67C;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp67B.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp67B.f2)->tag == 2U){_LLB: _tmp67C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp67B.f2)->f1;_LLC:
# 3923
({struct Cyc_List_List*_tmpC56=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp67C);cvtenv.kind_env=_tmpC56;});
({struct Cyc_List_List*_tmpC57=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp67C,1);cvtenv.free_vars=_tmpC57;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3928
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpC58=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpC58;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 3932
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp67F=({struct Cyc_String_pa_PrintArg_struct _tmp9F2;_tmp9F2.tag=0U,({struct _dyneither_ptr _tmpC59=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp684->name));_tmp9F2.f1=_tmpC59;});_tmp9F2;});void*_tmp67D[1U];_tmp67D[0]=& _tmp67F;({unsigned int _tmpC5B=loc;struct _dyneither_ptr _tmpC5A=({const char*_tmp67E="too many parameters for type %s";_tag_dyneither(_tmp67E,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpC5B,_tmpC5A,_tag_dyneither(_tmp67D,sizeof(void*),1U));});});
if(tvs != 0){
# 3936
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpC5C=({struct Cyc_List_List*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680->hd=e,_tmp680->tl=hidden_ts;_tmp680;});hidden_ts=_tmpC5C;});
({struct Cyc_Tcutil_CVTEnv _tmpC5D=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpC5D;});}
# 3943
({struct Cyc_List_List*_tmpC5F=({struct Cyc_List_List*_tmpC5E=*targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC5E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs=_tmpC5F;});}
# 3945
if((allow_abs_aggr  && _tmp684->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 3950
_tmp684->expected_mem_kind=1;}}_LL0:;}
# 3953
return cvtenv;}
# 3957
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 3963
struct Cyc_List_List*_tmp685=*targsp;
{union Cyc_Absyn_DatatypeInfo _tmp686=*info;union Cyc_Absyn_DatatypeInfo _tmp687=_tmp686;struct Cyc_Absyn_Datatypedecl*_tmp69E;struct _tuple2*_tmp69D;int _tmp69C;if((_tmp687.UnknownDatatype).tag == 1){_LL1: _tmp69D=((_tmp687.UnknownDatatype).val).name;_tmp69C=((_tmp687.UnknownDatatype).val).is_extensible;_LL2: {
# 3966
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp688;_push_handler(& _tmp688);{int _tmp68A=0;if(setjmp(_tmp688.handler))_tmp68A=1;if(!_tmp68A){({struct Cyc_Absyn_Datatypedecl**_tmpC60=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp69D);tudp=_tmpC60;});;_pop_handler();}else{void*_tmp689=(void*)_exn_thrown;void*_tmp68B=_tmp689;void*_tmp691;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp68B)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3970
struct Cyc_Absyn_Datatypedecl*_tmp68C=({struct Cyc_Absyn_Datatypedecl*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->sc=Cyc_Absyn_Extern,_tmp690->name=_tmp69D,_tmp690->tvs=0,_tmp690->fields=0,_tmp690->is_extensible=_tmp69C;_tmp690;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp68C);
({struct Cyc_Absyn_Datatypedecl**_tmpC61=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp69D);tudp=_tmpC61;});
# 3974
if(_tmp685 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp68F=({struct Cyc_String_pa_PrintArg_struct _tmp9F3;_tmp9F3.tag=0U,({
struct _dyneither_ptr _tmpC62=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69D));_tmp9F3.f1=_tmpC62;});_tmp9F3;});void*_tmp68D[1U];_tmp68D[0]=& _tmp68F;({unsigned int _tmpC64=loc;struct _dyneither_ptr _tmpC63=({const char*_tmp68E="declare parameterized datatype %s before using";_tag_dyneither(_tmp68E,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpC64,_tmpC63,_tag_dyneither(_tmp68D,sizeof(void*),1U));});});
return cvtenv;}
# 3979
goto _LL5;}}else{_LL8: _tmp691=_tmp68B;_LL9:(int)_rethrow(_tmp691);}_LL5:;}};}
# 3983
if(_tmp69C  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp694=({struct Cyc_String_pa_PrintArg_struct _tmp9F4;_tmp9F4.tag=0U,({struct _dyneither_ptr _tmpC65=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69D));_tmp9F4.f1=_tmpC65;});_tmp9F4;});void*_tmp692[1U];_tmp692[0]=& _tmp694;({unsigned int _tmpC67=loc;struct _dyneither_ptr _tmpC66=({const char*_tmp693="datatype %s was not declared @extensible";_tag_dyneither(_tmp693,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpC67,_tmpC66,_tag_dyneither(_tmp692,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfo _tmpC68=Cyc_Absyn_KnownDatatype(tudp);*info=_tmpC68;});
_tmp69E=*tudp;goto _LL4;}}else{_LL3: _tmp69E=*(_tmp687.KnownDatatype).val;_LL4: {
# 3989
struct Cyc_List_List*tvs=_tmp69E->tvs;
for(0;_tmp685 != 0  && tvs != 0;(_tmp685=_tmp685->tl,tvs=tvs->tl)){
void*t=(void*)_tmp685->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3995
{struct _tuple0 _tmp695=({struct _tuple0 _tmp9F5;({void*_tmpC69=Cyc_Absyn_compress_kb(tv->kind);_tmp9F5.f1=_tmpC69;}),_tmp9F5.f2=t;_tmp9F5;});struct _tuple0 _tmp696=_tmp695;struct Cyc_Absyn_Tvar*_tmp697;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp696.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp696.f2)->tag == 2U){_LLB: _tmp697=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp696.f2)->f1;_LLC:
# 3997
({struct Cyc_List_List*_tmpC6A=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp697);cvtenv.kind_env=_tmpC6A;});
({struct Cyc_List_List*_tmpC6B=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp697,1);cvtenv.free_vars=_tmpC6B;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 4002
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpC6C=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpC6C;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4006
if(_tmp685 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp69A=({struct Cyc_String_pa_PrintArg_struct _tmp9F6;_tmp9F6.tag=0U,({
struct _dyneither_ptr _tmpC6D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69E->name));_tmp9F6.f1=_tmpC6D;});_tmp9F6;});void*_tmp698[1U];_tmp698[0]=& _tmp69A;({unsigned int _tmpC6F=loc;struct _dyneither_ptr _tmpC6E=({const char*_tmp699="too many type arguments for datatype %s";_tag_dyneither(_tmp699,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC6F,_tmpC6E,_tag_dyneither(_tmp698,sizeof(void*),1U));});});
if(tvs != 0){
# 4011
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpC70=({struct Cyc_List_List*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->hd=e,_tmp69B->tl=hidden_ts;_tmp69B;});hidden_ts=_tmpC70;});
({struct Cyc_Tcutil_CVTEnv _tmpC71=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpC71;});}
# 4018
({struct Cyc_List_List*_tmpC73=({struct Cyc_List_List*_tmpC72=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC72,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmpC73;});}
# 4020
goto _LL0;}}_LL0:;}
# 4022
return cvtenv;}
# 4026
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype_field(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 4032
{union Cyc_Absyn_DatatypeFieldInfo _tmp69F=*info;union Cyc_Absyn_DatatypeFieldInfo _tmp6A0=_tmp69F;struct Cyc_Absyn_Datatypedecl*_tmp6B3;struct Cyc_Absyn_Datatypefield*_tmp6B2;struct _tuple2*_tmp6B1;struct _tuple2*_tmp6B0;int _tmp6AF;if((_tmp6A0.UnknownDatatypefield).tag == 1){_LL1: _tmp6B1=((_tmp6A0.UnknownDatatypefield).val).datatype_name;_tmp6B0=((_tmp6A0.UnknownDatatypefield).val).field_name;_tmp6AF=((_tmp6A0.UnknownDatatypefield).val).is_extensible;_LL2: {
# 4035
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp6B1);
struct Cyc_Absyn_Datatypefield*tuf;
# 4040
{struct Cyc_List_List*_tmp6A1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp6A1=_tmp6A1->tl){
if(_tmp6A1 == 0)({void*_tmp6A2=0U;({struct _dyneither_ptr _tmpC74=({const char*_tmp6A3="Tcutil found a bad datatypefield";_tag_dyneither(_tmp6A3,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC74,_tag_dyneither(_tmp6A2,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp6A1->hd)->name,_tmp6B0)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp6A1->hd;
break;}}}
# 4047
({union Cyc_Absyn_DatatypeFieldInfo _tmpC75=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmpC75;});
_tmp6B3=tud;_tmp6B2=tuf;goto _LL4;}}else{_LL3: _tmp6B3=((_tmp6A0.KnownDatatypefield).val).f1;_tmp6B2=((_tmp6A0.KnownDatatypefield).val).f2;_LL4: {
# 4051
struct Cyc_List_List*tvs=_tmp6B3->tvs;
for(0;targs != 0  && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4057
{struct _tuple0 _tmp6A4=({struct _tuple0 _tmp9F7;({void*_tmpC76=Cyc_Absyn_compress_kb(tv->kind);_tmp9F7.f1=_tmpC76;}),_tmp9F7.f2=t;_tmp9F7;});struct _tuple0 _tmp6A5=_tmp6A4;struct Cyc_Absyn_Tvar*_tmp6A6;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6A5.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A5.f2)->tag == 2U){_LL6: _tmp6A6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A5.f2)->f1;_LL7:
# 4059
({struct Cyc_List_List*_tmpC77=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6A6);cvtenv.kind_env=_tmpC77;});
({struct Cyc_List_List*_tmpC78=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp6A6,1);cvtenv.free_vars=_tmpC78;});
continue;}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}{
# 4064
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpC79=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpC79;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4068
if(targs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6A9=({struct Cyc_String_pa_PrintArg_struct _tmp9F9;_tmp9F9.tag=0U,({
struct _dyneither_ptr _tmpC7A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B3->name));_tmp9F9.f1=_tmpC7A;});_tmp9F9;});struct Cyc_String_pa_PrintArg_struct _tmp6AA=({struct Cyc_String_pa_PrintArg_struct _tmp9F8;_tmp9F8.tag=0U,({struct _dyneither_ptr _tmpC7B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B2->name));_tmp9F8.f1=_tmpC7B;});_tmp9F8;});void*_tmp6A7[2U];_tmp6A7[0]=& _tmp6A9,_tmp6A7[1]=& _tmp6AA;({unsigned int _tmpC7D=loc;struct _dyneither_ptr _tmpC7C=({const char*_tmp6A8="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp6A8,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpC7D,_tmpC7C,_tag_dyneither(_tmp6A7,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6AD=({struct Cyc_String_pa_PrintArg_struct _tmp9FB;_tmp9FB.tag=0U,({
struct _dyneither_ptr _tmpC7E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B3->name));_tmp9FB.f1=_tmpC7E;});_tmp9FB;});struct Cyc_String_pa_PrintArg_struct _tmp6AE=({struct Cyc_String_pa_PrintArg_struct _tmp9FA;_tmp9FA.tag=0U,({struct _dyneither_ptr _tmpC7F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B2->name));_tmp9FA.f1=_tmpC7F;});_tmp9FA;});void*_tmp6AB[2U];_tmp6AB[0]=& _tmp6AD,_tmp6AB[1]=& _tmp6AE;({unsigned int _tmpC81=loc;struct _dyneither_ptr _tmpC80=({const char*_tmp6AC="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp6AC,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpC81,_tmpC80,_tag_dyneither(_tmp6AB,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 4076
return cvtenv;}
# 4079
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_app(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 4084
struct Cyc_List_List*_tmp6B4=*targsp;
{void*_tmp6B5=c;union Cyc_Absyn_DatatypeFieldInfo*_tmp6DC;union Cyc_Absyn_DatatypeInfo*_tmp6DB;union Cyc_Absyn_AggrInfo*_tmp6DA;struct Cyc_List_List*_tmp6D9;struct _tuple2*_tmp6D8;struct Cyc_Absyn_Enumdecl**_tmp6D7;switch(*((int*)_tmp6B5)){case 1U: _LL1: _LL2:
# 4087
 goto _LL4;case 2U: _LL3: _LL4: goto _LL6;case 0U: _LL5: _LL6: goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA: goto _LLC;case 5U: _LLB: _LLC: goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10: goto _LL12;case 14U: _LL11: _LL12: goto _LL14;case 17U: _LL13: _LL14:
# 4091
 if(_tmp6B4 != 0)({struct Cyc_String_pa_PrintArg_struct _tmp6B8=({struct Cyc_String_pa_PrintArg_struct _tmp9FC;_tmp9FC.tag=0U,({
struct _dyneither_ptr _tmpC82=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tag=0U,_tmp6B9->f1=c,_tmp6B9->f2=_tmp6B4;_tmp6B9;})));_tmp9FC.f1=_tmpC82;});_tmp9FC;});void*_tmp6B6[1U];_tmp6B6[0]=& _tmp6B8;({struct _dyneither_ptr _tmpC83=({const char*_tmp6B7="%s applied to argument(s)";_tag_dyneither(_tmp6B7,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC83,_tag_dyneither(_tmp6B6,sizeof(void*),1U));});});
goto _LL0;case 9U: _LL15: _LL16:
# 4095
 for(0;_tmp6B4 != 0;_tmp6B4=_tmp6B4->tl){
({struct Cyc_Tcutil_CVTEnv _tmpC84=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp6B4->hd,1,1);cvtenv=_tmpC84;});}
goto _LL0;case 4U: _LL17: _LL18:
# 4099
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B4)!= 1)({void*_tmp6BA=0U;({struct _dyneither_ptr _tmpC85=({const char*_tmp6BB="tag_t applied to wrong number of arguments";_tag_dyneither(_tmp6BB,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC85,_tag_dyneither(_tmp6BA,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC86=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B4))->hd,0,1);cvtenv=_tmpC86;});goto _LL0;case 15U: _LL19: _tmp6D8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B5)->f1;_tmp6D7=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B5)->f2;_LL1A:
# 4102
 if(_tmp6B4 != 0)({void*_tmp6BC=0U;({struct _dyneither_ptr _tmpC87=({const char*_tmp6BD="enum applied to argument(s)";_tag_dyneither(_tmp6BD,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC87,_tag_dyneither(_tmp6BC,sizeof(void*),0U));});});
if(*_tmp6D7 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6D7))->fields == 0){
struct _handler_cons _tmp6BE;_push_handler(& _tmp6BE);{int _tmp6C0=0;if(setjmp(_tmp6BE.handler))_tmp6C0=1;if(!_tmp6C0){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D8);
*_tmp6D7=*ed;}
# 4105
;_pop_handler();}else{void*_tmp6BF=(void*)_exn_thrown;void*_tmp6C1=_tmp6BF;void*_tmp6C4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6C1)->tag == Cyc_Dict_Absent){_LL2C: _LL2D: {
# 4109
struct Cyc_Absyn_Enumdecl*_tmp6C2=({struct Cyc_Absyn_Enumdecl*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->sc=Cyc_Absyn_Extern,_tmp6C3->name=_tmp6D8,_tmp6C3->fields=0;_tmp6C3;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6C2);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D8);
*_tmp6D7=*ed;
goto _LL2B;};}}else{_LL2E: _tmp6C4=_tmp6C1;_LL2F:(int)_rethrow(_tmp6C4);}_LL2B:;}};}
# 4115
goto _LL0;case 16U: _LL1B: _tmp6D9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp6B5)->f1;_LL1C:
# 4117
 if(_tmp6B4 != 0)({void*_tmp6C5=0U;({struct _dyneither_ptr _tmpC88=({const char*_tmp6C6="enum applied to argument(s)";_tag_dyneither(_tmp6C6,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC88,_tag_dyneither(_tmp6C5,sizeof(void*),0U));});});{
# 4119
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp6D9 != 0;_tmp6D9=_tmp6D9->tl){
struct Cyc_Absyn_Enumfield*_tmp6C7=(struct Cyc_Absyn_Enumfield*)_tmp6D9->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6C7->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6CA=({struct Cyc_String_pa_PrintArg_struct _tmp9FD;_tmp9FD.tag=0U,_tmp9FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C7->name).f2);_tmp9FD;});void*_tmp6C8[1U];_tmp6C8[0]=& _tmp6CA;({unsigned int _tmpC8A=_tmp6C7->loc;struct _dyneither_ptr _tmpC89=({const char*_tmp6C9="duplicate enum field name %s";_tag_dyneither(_tmp6C9,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpC8A,_tmpC89,_tag_dyneither(_tmp6C8,sizeof(void*),1U));});});else{
# 4126
({struct Cyc_List_List*_tmpC8B=({struct Cyc_List_List*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->hd=(*_tmp6C7->name).f2,_tmp6CB->tl=prev_fields;_tmp6CB;});prev_fields=_tmpC8B;});}
if(_tmp6C7->tag == 0)
({struct Cyc_Absyn_Exp*_tmpC8C=Cyc_Absyn_uint_exp(tag_count,_tmp6C7->loc);_tmp6C7->tag=_tmpC8C;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C7->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6CE=({struct Cyc_String_pa_PrintArg_struct _tmp9FE;_tmp9FE.tag=0U,_tmp9FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C7->name).f2);_tmp9FE;});void*_tmp6CC[1U];_tmp6CC[0]=& _tmp6CE;({unsigned int _tmpC8E=loc;struct _dyneither_ptr _tmpC8D=({const char*_tmp6CD="enum field %s: expression is not constant";_tag_dyneither(_tmp6CD,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpC8E,_tmpC8D,_tag_dyneither(_tmp6CC,sizeof(void*),1U));});});}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C7->tag))).f1;
tag_count=t1 + 1;};}
# 4134
goto _LL0;};case 10U: _LL1D: _LL1E:
# 4136
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B4)!= 1)({void*_tmp6CF=0U;({struct _dyneither_ptr _tmpC8F=({const char*_tmp6D0="regions has wrong number of arguments";_tag_dyneither(_tmp6D0,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC8F,_tag_dyneither(_tmp6CF,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC90=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(_tmp6B4))->hd,1,1);cvtenv=_tmpC90;});goto _LL0;case 3U: _LL1F: _LL20:
# 4139
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B4)!= 1)({void*_tmp6D1=0U;({struct _dyneither_ptr _tmpC91=({const char*_tmp6D2="region_t has wrong number of arguments";_tag_dyneither(_tmp6D2,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC91,_tag_dyneither(_tmp6D1,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC92=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B4))->hd,1,1);cvtenv=_tmpC92;});
goto _LL0;case 13U: _LL21: _LL22:
# 4143
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B4)!= 1)({void*_tmp6D3=0U;({struct _dyneither_ptr _tmpC93=({const char*_tmp6D4="@thin has wrong number of arguments";_tag_dyneither(_tmp6D4,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC93,_tag_dyneither(_tmp6D3,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC94=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B4))->hd,0,1);cvtenv=_tmpC94;});
goto _LL0;case 8U: _LL23: _LL24:
# 4147
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B4)!= 1)({void*_tmp6D5=0U;({struct _dyneither_ptr _tmpC95=({const char*_tmp6D6="access has wrong number of arguments";_tag_dyneither(_tmp6D6,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC95,_tag_dyneither(_tmp6D5,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC96=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B4))->hd,1,1);cvtenv=_tmpC96;});goto _LL0;case 20U: _LL25: _tmp6DA=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp6B5)->f1;_LL26:
# 4150
({struct Cyc_Tcutil_CVTEnv _tmpC97=Cyc_Tcutil_i_check_valid_aggr(loc,te,cvtenv,expected_kind,_tmp6DA,targsp,allow_abs_aggr);cvtenv=_tmpC97;});
# 4152
goto _LL0;case 18U: _LL27: _tmp6DB=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp6B5)->f1;_LL28:
# 4154
({struct Cyc_Tcutil_CVTEnv _tmpC98=Cyc_Tcutil_i_check_valid_datatype(loc,te,cvtenv,expected_kind,_tmp6DB,targsp,allow_abs_aggr);cvtenv=_tmpC98;});
# 4156
goto _LL0;default: _LL29: _tmp6DC=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp6B5)->f1;_LL2A:
# 4158
({struct Cyc_Tcutil_CVTEnv _tmpC99=Cyc_Tcutil_i_check_valid_datatype_field(loc,te,cvtenv,expected_kind,_tmp6DC,_tmp6B4,allow_abs_aggr);cvtenv=_tmpC99;});
# 4160
goto _LL0;}_LL0:;}
# 4162
return cvtenv;}struct _tuple33{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 4166
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 4174
{void*_tmp6DD=Cyc_Tcutil_compress(t);void*_tmp6DE=_tmp6DD;struct _tuple2*_tmp7E2;struct Cyc_List_List**_tmp7E1;struct Cyc_Absyn_Typedefdecl**_tmp7E0;void**_tmp7DF;enum Cyc_Absyn_AggrKind _tmp7DE;struct Cyc_List_List*_tmp7DD;struct Cyc_List_List*_tmp7DC;struct Cyc_List_List**_tmp7DB;void**_tmp7DA;struct Cyc_Absyn_Tqual*_tmp7D9;void*_tmp7D8;struct Cyc_List_List*_tmp7D7;int _tmp7D6;struct Cyc_Absyn_VarargInfo*_tmp7D5;struct Cyc_List_List*_tmp7D4;struct Cyc_List_List*_tmp7D3;struct Cyc_Absyn_Exp*_tmp7D2;struct Cyc_List_List**_tmp7D1;struct Cyc_Absyn_Exp*_tmp7D0;struct Cyc_List_List**_tmp7CF;void*_tmp7CE;struct Cyc_Absyn_Tqual*_tmp7CD;struct Cyc_Absyn_Exp**_tmp7CC;void*_tmp7CB;unsigned int _tmp7CA;struct Cyc_Absyn_Exp*_tmp7C9;struct Cyc_Absyn_Exp*_tmp7C8;void*_tmp7C7;struct Cyc_Absyn_Tqual*_tmp7C6;void*_tmp7C5;void*_tmp7C4;void*_tmp7C3;void*_tmp7C2;void*_tmp7C1;void***_tmp7C0;struct Cyc_Absyn_Tvar*_tmp7BF;struct Cyc_Core_Opt**_tmp7BE;void**_tmp7BD;void*_tmp7BC;struct Cyc_List_List**_tmp7BB;switch(*((int*)_tmp6DE)){case 0U: _LL1: _tmp7BC=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DE)->f1;_tmp7BB=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DE)->f2;_LL2:
# 4176
({struct Cyc_Tcutil_CVTEnv _tmpC9A=Cyc_Tcutil_i_check_valid_type_app(loc,te,cvtenv,expected_kind,_tmp7BC,_tmp7BB,put_in_effect,allow_abs_aggr);cvtenv=_tmpC9A;});
# 4178
goto _LL0;case 1U: _LL3: _tmp7BE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DE)->f1;_tmp7BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DE)->f2;_LL4:
# 4181
 if(*_tmp7BE == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BE))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BE))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpC9B=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp7BE=_tmpC9B;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 4186
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp7BD=Cyc_Absyn_unique_rgn_type;else{
# 4189
*_tmp7BD=Cyc_Absyn_heap_rgn_type;}}else{
if((cvtenv.generalize_evars  && expected_kind->kind != Cyc_Absyn_BoolKind) && expected_kind->kind != Cyc_Absyn_PtrBndKind){
# 4193
struct Cyc_Absyn_Tvar*_tmp6DF=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0->tag=2U,_tmp6E0->f1=0,_tmp6E0->f2=expected_kind;_tmp6E0;}));
({void*_tmpC9C=Cyc_Absyn_var_type(_tmp6DF);*_tmp7BD=_tmpC9C;});
_tmp7BF=_tmp6DF;goto _LL6;}else{
# 4197
({struct Cyc_List_List*_tmpC9D=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpC9D;});}}
# 4199
goto _LL0;case 2U: _LL5: _tmp7BF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6DE)->f1;_LL6:
# 4201
{void*_tmp6E1=Cyc_Absyn_compress_kb(_tmp7BF->kind);void*_tmp6E2=_tmp6E1;struct Cyc_Core_Opt**_tmp6E5;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6E2)->tag == 1U){_LL1A: _tmp6E5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6E2)->f1;_LL1B:
# 4203
({struct Cyc_Core_Opt*_tmpC9F=({struct Cyc_Core_Opt*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));({void*_tmpC9E=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3->tag=2U,_tmp6E3->f1=0,_tmp6E3->f2=expected_kind;_tmp6E3;});_tmp6E4->v=_tmpC9E;});_tmp6E4;});*_tmp6E5=_tmpC9F;});goto _LL19;}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 4208
({struct Cyc_List_List*_tmpCA0=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7BF);cvtenv.kind_env=_tmpCA0;});
# 4211
({struct Cyc_List_List*_tmpCA1=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7BF,put_in_effect);cvtenv.free_vars=_tmpCA1;});
# 4213
{void*_tmp6E6=Cyc_Absyn_compress_kb(_tmp7BF->kind);void*_tmp6E7=_tmp6E6;struct Cyc_Core_Opt**_tmp6EB;struct Cyc_Absyn_Kind*_tmp6EA;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E7)->tag == 2U){_LL1F: _tmp6EB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E7)->f1;_tmp6EA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E7)->f2;_LL20:
# 4215
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6EA))
({struct Cyc_Core_Opt*_tmpCA3=({struct Cyc_Core_Opt*_tmp6E9=_cycalloc(sizeof(*_tmp6E9));({void*_tmpCA2=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->tag=2U,_tmp6E8->f1=0,_tmp6E8->f2=expected_kind;_tmp6E8;});_tmp6E9->v=_tmpCA2;});_tmp6E9;});*_tmp6EB=_tmpCA3;});
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 4220
goto _LL0;case 10U: _LL7: _tmp7C1=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DE)->f1)->r;_tmp7C0=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DE)->f2;_LL8: {
# 4226
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6EC=_tmp7C1;struct Cyc_Absyn_Datatypedecl*_tmp6EF;struct Cyc_Absyn_Enumdecl*_tmp6EE;struct Cyc_Absyn_Aggrdecl*_tmp6ED;switch(*((int*)_tmp6EC)){case 0U: _LL24: _tmp6ED=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6EC)->f1;_LL25:
# 4229
 if(te->in_extern_c_include)
_tmp6ED->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6ED);goto _LL23;case 1U: _LL26: _tmp6EE=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6EC)->f1;_LL27:
# 4233
 if(te->in_extern_c_include)
_tmp6EE->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6EE);goto _LL23;default: _LL28: _tmp6EF=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6EC)->f1;_LL29:
# 4237
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6EF);goto _LL23;}_LL23:;}
# 4239
({void**_tmpCA4=({void**_tmp6F0=_cycalloc(sizeof(*_tmp6F0));*_tmp6F0=new_t;_tmp6F0;});*_tmp7C0=_tmpCA4;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 3U: _LL9: _tmp7C7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DE)->f1).elt_type;_tmp7C6=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DE)->f1).elt_tq;_tmp7C5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DE)->f1).ptr_atts).rgn;_tmp7C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DE)->f1).ptr_atts).nullable;_tmp7C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DE)->f1).ptr_atts).bounds;_tmp7C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DE)->f1).ptr_atts).zero_term;_LLA: {
# 4245
int is_zero_terminated;
# 4247
({struct Cyc_Tcutil_CVTEnv _tmpCA5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7C7,1,1);cvtenv=_tmpCA5;});
({int _tmpCA6=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7C6->print_const,_tmp7C7);_tmp7C6->real_const=_tmpCA6;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp6F1=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp6F2=_tmp6F1;switch(_tmp6F2){case Cyc_Absyn_Aliasable: _LL2B: _LL2C:
 k=& Cyc_Tcutil_rk;goto _LL2A;case Cyc_Absyn_Unique: _LL2D: _LL2E:
 k=& Cyc_Tcutil_urk;goto _LL2A;default: _LL2F: _LL30:
 k=& Cyc_Tcutil_trk;goto _LL2A;}_LL2A:;}
# 4255
({struct Cyc_Tcutil_CVTEnv _tmpCA7=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7C5,1,1);cvtenv=_tmpCA7;});
# 4258
({struct Cyc_Tcutil_CVTEnv _tmpCA8=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7C2,0,1);cvtenv=_tmpCA8;});
({struct Cyc_Tcutil_CVTEnv _tmpCA9=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7C4,0,1);cvtenv=_tmpCA9;});
({void*_tmpCAA=_tmp7C2;Cyc_Tcutil_unify(_tmpCAA,Cyc_Tcutil_is_char_type(_tmp7C7)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
({int _tmpCAB=Cyc_Absyn_type2bool(0,_tmp7C2);is_zero_terminated=_tmpCAB;});
if(is_zero_terminated){
# 4264
if(!Cyc_Tcutil_admits_zero(_tmp7C7))
({struct Cyc_String_pa_PrintArg_struct _tmp6F5=({struct Cyc_String_pa_PrintArg_struct _tmp9FF;_tmp9FF.tag=0U,({
struct _dyneither_ptr _tmpCAC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7C7));_tmp9FF.f1=_tmpCAC;});_tmp9FF;});void*_tmp6F3[1U];_tmp6F3[0]=& _tmp6F5;({unsigned int _tmpCAE=loc;struct _dyneither_ptr _tmpCAD=({const char*_tmp6F4="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp6F4,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpCAE,_tmpCAD,_tag_dyneither(_tmp6F3,sizeof(void*),1U));});});}
# 4269
({struct Cyc_Tcutil_CVTEnv _tmpCAF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ptrbk,_tmp7C3,0,allow_abs_aggr);cvtenv=_tmpCAF;});{
struct Cyc_Absyn_Exp*_tmp6F6=({void*_tmpCB0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCB0,_tmp7C3);});
if(_tmp6F6 != 0){
struct _tuple13 _tmp6F7=Cyc_Evexp_eval_const_uint_exp(_tmp6F6);struct _tuple13 _tmp6F8=_tmp6F7;unsigned int _tmp6FC;int _tmp6FB;_LL32: _tmp6FC=_tmp6F8.f1;_tmp6FB=_tmp6F8.f2;_LL33:;
if(is_zero_terminated  && (!_tmp6FB  || _tmp6FC < 1))
({void*_tmp6F9=0U;({unsigned int _tmpCB2=loc;struct _dyneither_ptr _tmpCB1=({const char*_tmp6FA="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp6FA,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpCB2,_tmpCB1,_tag_dyneither(_tmp6F9,sizeof(void*),0U));});});}
# 4276
goto _LL0;};};}case 9U: _LLB: _tmp7C8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6DE)->f1;_LLC:
# 4281
({struct Cyc_Tcenv_Tenv*_tmpCB3=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpCB3,0,_tmp7C8);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp7C8))
({void*_tmp6FD=0U;({unsigned int _tmpCB5=loc;struct _dyneither_ptr _tmpCB4=({const char*_tmp6FE="valueof_t requires an int expression";_tag_dyneither(_tmp6FE,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpCB5,_tmpCB4,_tag_dyneither(_tmp6FD,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpCB6=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7C8,te,cvtenv);cvtenv=_tmpCB6;});
goto _LL0;case 11U: _LLD: _tmp7C9=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6DE)->f1;_LLE:
# 4290
({struct Cyc_Tcenv_Tenv*_tmpCB7=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpCB7,0,_tmp7C9);});
goto _LL0;case 4U: _LLF: _tmp7CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DE)->f1).elt_type;_tmp7CD=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DE)->f1).tq;_tmp7CC=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DE)->f1).num_elts;_tmp7CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DE)->f1).zero_term;_tmp7CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DE)->f1).zt_loc;_LL10: {
# 4295
struct Cyc_Absyn_Exp*_tmp6FF=*_tmp7CC;
({struct Cyc_Tcutil_CVTEnv _tmpCB8=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7CE,1,allow_abs_aggr);cvtenv=_tmpCB8;});
({int _tmpCB9=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7CD->print_const,_tmp7CE);_tmp7CD->real_const=_tmpCB9;});{
# 4299
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,_tmp7CB);
if(is_zero_terminated){
# 4302
if(!Cyc_Tcutil_admits_zero(_tmp7CE))
({struct Cyc_String_pa_PrintArg_struct _tmp702=({struct Cyc_String_pa_PrintArg_struct _tmpA00;_tmpA00.tag=0U,({
struct _dyneither_ptr _tmpCBA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7CE));_tmpA00.f1=_tmpCBA;});_tmpA00;});void*_tmp700[1U];_tmp700[0]=& _tmp702;({unsigned int _tmpCBC=loc;struct _dyneither_ptr _tmpCBB=({const char*_tmp701="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp701,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpCBC,_tmpCBB,_tag_dyneither(_tmp700,sizeof(void*),1U));});});}
# 4308
if(_tmp6FF == 0){
# 4310
if(is_zero_terminated)
# 4312
({struct Cyc_Absyn_Exp*_tmpCBE=({struct Cyc_Absyn_Exp*_tmpCBD=Cyc_Absyn_uint_exp(1U,0U);_tmp6FF=_tmpCBD;});*_tmp7CC=_tmpCBE;});else{
# 4315
({void*_tmp703=0U;({unsigned int _tmpCC0=loc;struct _dyneither_ptr _tmpCBF=({const char*_tmp704="array bound defaults to 1 here";_tag_dyneither(_tmp704,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpCC0,_tmpCBF,_tag_dyneither(_tmp703,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpCC2=({struct Cyc_Absyn_Exp*_tmpCC1=Cyc_Absyn_uint_exp(1U,0U);_tmp6FF=_tmpCC1;});*_tmp7CC=_tmpCC2;});}}
# 4319
({struct Cyc_Tcenv_Tenv*_tmpCC3=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpCC3,0,_tmp6FF);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp6FF))
({void*_tmp705=0U;({unsigned int _tmpCC5=loc;struct _dyneither_ptr _tmpCC4=({const char*_tmp706="array bounds expression is not an unsigned int";_tag_dyneither(_tmp706,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCC5,_tmpCC4,_tag_dyneither(_tmp705,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpCC6=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp6FF,te,cvtenv);cvtenv=_tmpCC6;});{
# 4327
struct _tuple13 _tmp707=Cyc_Evexp_eval_const_uint_exp(_tmp6FF);struct _tuple13 _tmp708=_tmp707;unsigned int _tmp70E;int _tmp70D;_LL35: _tmp70E=_tmp708.f1;_tmp70D=_tmp708.f2;_LL36:;
# 4329
if((is_zero_terminated  && _tmp70D) && _tmp70E < 1)
({void*_tmp709=0U;({unsigned int _tmpCC8=loc;struct _dyneither_ptr _tmpCC7=({const char*_tmp70A="zero terminated array cannot have zero elements";_tag_dyneither(_tmp70A,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpCC8,_tmpCC7,_tag_dyneither(_tmp709,sizeof(void*),0U));});});
# 4332
if((_tmp70D  && _tmp70E < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp70B=0U;({unsigned int _tmpCCA=loc;struct _dyneither_ptr _tmpCC9=({const char*_tmp70C="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp70C,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpCCA,_tmpCC9,_tag_dyneither(_tmp70B,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpCCB=Cyc_Absyn_uint_exp(1U,0U);*_tmp7CC=_tmpCCB;});}
# 4336
goto _LL0;};};}case 5U: _LL11: _tmp7DB=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).tvars;_tmp7DA=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).effect;_tmp7D9=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).ret_tqual;_tmp7D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).ret_type;_tmp7D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).args;_tmp7D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).c_varargs;_tmp7D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).cyc_varargs;_tmp7D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).rgn_po;_tmp7D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).attributes;_tmp7D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).requires_clause;_tmp7D1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).requires_relns;_tmp7D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).ensures_clause;_tmp7CF=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DE)->f1).ensures_relns;_LL12: {
# 4343
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp7D3 != 0;_tmp7D3=_tmp7D3->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7D3->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp711=({struct Cyc_String_pa_PrintArg_struct _tmpA01;_tmpA01.tag=0U,({struct _dyneither_ptr _tmpCCC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7D3->hd));_tmpA01.f1=_tmpCCC;});_tmpA01;});void*_tmp70F[1U];_tmp70F[0]=& _tmp711;({unsigned int _tmpCCE=loc;struct _dyneither_ptr _tmpCCD=({const char*_tmp710="bad function type attribute %s";_tag_dyneither(_tmp710,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpCCE,_tmpCCD,_tag_dyneither(_tmp70F,sizeof(void*),1U));});});{
void*_tmp712=(void*)_tmp7D3->hd;void*_tmp713=_tmp712;enum Cyc_Absyn_Format_Type _tmp718;int _tmp717;int _tmp716;switch(*((int*)_tmp713)){case 1U: _LL38: _LL39:
# 4356
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL37;case 2U: _LL3A: _LL3B:
# 4358
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL37;case 3U: _LL3C: _LL3D:
# 4360
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL37;case 19U: _LL3E: _tmp718=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp713)->f1;_tmp717=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp713)->f2;_tmp716=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp713)->f3;_LL3F:
# 4362
 if(!seen_format){
seen_format=1;
ft=_tmp718;
fmt_desc_arg=_tmp717;
fmt_arg_start=_tmp716;}else{
# 4368
({void*_tmp714=0U;({unsigned int _tmpCD0=loc;struct _dyneither_ptr _tmpCCF=({const char*_tmp715="function can't have multiple format attributes";_tag_dyneither(_tmp715,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCD0,_tmpCCF,_tag_dyneither(_tmp714,sizeof(void*),0U));});});}
goto _LL37;default: _LL40: _LL41:
 goto _LL37;}_LL37:;};}
# 4373
if(num_convs > 1)
({void*_tmp719=0U;({unsigned int _tmpCD2=loc;struct _dyneither_ptr _tmpCD1=({const char*_tmp71A="function can't have multiple calling conventions";_tag_dyneither(_tmp71A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpCD2,_tmpCD1,_tag_dyneither(_tmp719,sizeof(void*),0U));});});
# 4378
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7DB);
{struct Cyc_List_List*b=*_tmp7DB;for(0;b != 0;b=b->tl){
({int _tmpCD3=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpCD3;});
({struct Cyc_List_List*_tmpCD4=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpCD4;});{
void*_tmp71B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp71C=_tmp71B;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp71C)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp71C)->f1)->kind == Cyc_Absyn_MemKind){_LL43: _LL44:
# 4384
({struct Cyc_String_pa_PrintArg_struct _tmp71F=({struct Cyc_String_pa_PrintArg_struct _tmpA02;_tmpA02.tag=0U,_tmpA02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmpA02;});void*_tmp71D[1U];_tmp71D[0]=& _tmp71F;({unsigned int _tmpCD6=loc;struct _dyneither_ptr _tmpCD5=({const char*_tmp71E="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp71E,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpCD6,_tmpCD5,_tag_dyneither(_tmp71D,sizeof(void*),1U));});});
goto _LL42;}else{goto _LL45;}}else{_LL45: _LL46:
 goto _LL42;}_LL42:;};}}{
# 4392
struct Cyc_Tcutil_CVTEnv _tmp720=({struct Cyc_Tcutil_CVTEnv _tmpA05;_tmpA05.r=Cyc_Core_heap_region,_tmpA05.kind_env=cvtenv.kind_env,_tmpA05.free_vars=0,_tmpA05.free_evars=0,_tmpA05.generalize_evars=cvtenv.generalize_evars,_tmpA05.fn_result=1;_tmpA05;});
# 4397
({struct Cyc_Tcutil_CVTEnv _tmpCD7=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp720,& Cyc_Tcutil_tmk,_tmp7D8,1,1);_tmp720=_tmpCD7;});
({int _tmpCD8=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7D9->print_const,_tmp7D8);_tmp7D9->real_const=_tmpCD8;});
_tmp720.fn_result=0;
{struct Cyc_List_List*a=_tmp7D7;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp721=(struct _tuple10*)a->hd;
void*_tmp722=(*_tmp721).f3;
({struct Cyc_Tcutil_CVTEnv _tmpCD9=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp720,& Cyc_Tcutil_tmk,_tmp722,1,1);_tmp720=_tmpCD9;});{
int _tmp723=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp721).f2).print_const,_tmp722);
((*_tmp721).f2).real_const=_tmp723;
# 4408
if(Cyc_Tcutil_is_array_type(_tmp722)){
# 4410
void*_tmp724=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpCDA=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp720,& Cyc_Tcutil_rk,_tmp724,1,1);_tmp720=_tmpCDA;});
({void*_tmpCDB=Cyc_Tcutil_promote_array(_tmp722,_tmp724,0);(*_tmp721).f3=_tmpCDB;});}};}}
# 4417
if(_tmp7D5 != 0){
if(_tmp7D6)({void*_tmp725=0U;({struct _dyneither_ptr _tmpCDC=({const char*_tmp726="both c_vararg and cyc_vararg";_tag_dyneither(_tmp726,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDC,_tag_dyneither(_tmp725,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp727=*_tmp7D5;struct Cyc_Absyn_VarargInfo _tmp728=_tmp727;void*_tmp739;int _tmp738;_LL48: _tmp739=_tmp728.type;_tmp738=_tmp728.inject;_LL49:;
({struct Cyc_Tcutil_CVTEnv _tmpCDD=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp720,& Cyc_Tcutil_tmk,_tmp739,1,1);_tmp720=_tmpCDD;});
({int _tmpCDE=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp7D5->tq).print_const,_tmp739);(_tmp7D5->tq).real_const=_tmpCDE;});
# 4423
if(_tmp738){
void*_tmp729=Cyc_Tcutil_compress(_tmp739);void*_tmp72A=_tmp729;void*_tmp737;void*_tmp736;void*_tmp735;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72A)->tag == 3U){_LL4B: _tmp737=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72A)->f1).elt_type;_tmp736=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72A)->f1).ptr_atts).bounds;_tmp735=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72A)->f1).ptr_atts).zero_term;_LL4C:
# 4426
{void*_tmp72B=Cyc_Tcutil_compress(_tmp737);void*_tmp72C=_tmp72B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72C)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72C)->f1)->tag == 18U){_LL50: _LL51:
# 4428
 if(Cyc_Tcutil_force_type2bool(0,_tmp735))
({void*_tmp72D=0U;({unsigned int _tmpCE0=loc;struct _dyneither_ptr _tmpCDF=({const char*_tmp72E="can't inject into a zeroterm pointer";_tag_dyneither(_tmp72E,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpCE0,_tmpCDF,_tag_dyneither(_tmp72D,sizeof(void*),0U));});});
if(!({void*_tmpCE1=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpCE1,_tmp736);}))
({void*_tmp72F=0U;({unsigned int _tmpCE3=loc;struct _dyneither_ptr _tmpCE2=({const char*_tmp730="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp730,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpCE3,_tmpCE2,_tag_dyneither(_tmp72F,sizeof(void*),0U));});});
goto _LL4F;}else{goto _LL52;}}else{_LL52: _LL53:
({void*_tmp731=0U;({unsigned int _tmpCE5=loc;struct _dyneither_ptr _tmpCE4=({const char*_tmp732="can't inject a non-datatype type";_tag_dyneither(_tmp732,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpCE5,_tmpCE4,_tag_dyneither(_tmp731,sizeof(void*),0U));});});goto _LL4F;}_LL4F:;}
# 4435
goto _LL4A;}else{_LL4D: _LL4E:
({void*_tmp733=0U;({unsigned int _tmpCE7=loc;struct _dyneither_ptr _tmpCE6=({const char*_tmp734="expecting a datatype pointer type";_tag_dyneither(_tmp734,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpCE7,_tmpCE6,_tag_dyneither(_tmp733,sizeof(void*),0U));});});goto _LL4A;}_LL4A:;}};}
# 4441
if(seen_format){
int _tmp73A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7D7);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp73A) || fmt_arg_start < 0) || 
# 4445
(_tmp7D5 == 0  && !_tmp7D6) && fmt_arg_start != 0) || 
(_tmp7D5 != 0  || _tmp7D6) && fmt_arg_start != _tmp73A + 1)
# 4448
({void*_tmp73B=0U;({unsigned int _tmpCE9=loc;struct _dyneither_ptr _tmpCE8=({const char*_tmp73C="bad format descriptor";_tag_dyneither(_tmp73C,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpCE9,_tmpCE8,_tag_dyneither(_tmp73B,sizeof(void*),0U));});});else{
# 4451
struct _tuple10 _tmp73D=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7D7,fmt_desc_arg - 1);struct _tuple10 _tmp73E=_tmp73D;void*_tmp753;_LL55: _tmp753=_tmp73E.f3;_LL56:;
# 4453
{void*_tmp73F=Cyc_Tcutil_compress(_tmp753);void*_tmp740=_tmp73F;void*_tmp74C;void*_tmp74B;void*_tmp74A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp740)->tag == 3U){_LL58: _tmp74C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp740)->f1).elt_type;_tmp74B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp740)->f1).ptr_atts).bounds;_tmp74A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp740)->f1).ptr_atts).zero_term;_LL59:
# 4456
 if(!Cyc_Tcutil_is_char_type(_tmp74C))
({void*_tmp741=0U;({unsigned int _tmpCEB=loc;struct _dyneither_ptr _tmpCEA=({const char*_tmp742="format descriptor is not a string";_tag_dyneither(_tmp742,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpCEB,_tmpCEA,_tag_dyneither(_tmp741,sizeof(void*),0U));});});else{
# 4459
struct Cyc_Absyn_Exp*_tmp743=({void*_tmpCEC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCEC,_tmp74B);});
if(_tmp743 == 0  && _tmp7D6)
({void*_tmp744=0U;({unsigned int _tmpCEE=loc;struct _dyneither_ptr _tmpCED=({const char*_tmp745="format descriptor is not a char * type";_tag_dyneither(_tmp745,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCEE,_tmpCED,_tag_dyneither(_tmp744,sizeof(void*),0U));});});else{
if(_tmp743 != 0  && !_tmp7D6)
({void*_tmp746=0U;({unsigned int _tmpCF0=loc;struct _dyneither_ptr _tmpCEF=({const char*_tmp747="format descriptor is not a char ? type";_tag_dyneither(_tmp747,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCF0,_tmpCEF,_tag_dyneither(_tmp746,sizeof(void*),0U));});});}}
# 4465
goto _LL57;}else{_LL5A: _LL5B:
({void*_tmp748=0U;({unsigned int _tmpCF2=loc;struct _dyneither_ptr _tmpCF1=({const char*_tmp749="format descriptor is not a string type";_tag_dyneither(_tmp749,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCF2,_tmpCF1,_tag_dyneither(_tmp748,sizeof(void*),0U));});});goto _LL57;}_LL57:;}
# 4468
if(fmt_arg_start != 0  && !_tmp7D6){
# 4472
int problem;
{struct _tuple33 _tmp74D=({struct _tuple33 _tmpA03;_tmpA03.f1=ft,({void*_tmpCF3=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7D5))->type));_tmpA03.f2=_tmpCF3;});_tmpA03;});struct _tuple33 _tmp74E=_tmp74D;struct Cyc_Absyn_Datatypedecl*_tmp750;struct Cyc_Absyn_Datatypedecl*_tmp74F;if(_tmp74E.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5D: _tmp74F=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->f1)->f1).KnownDatatype).val;_LL5E:
# 4475
({int _tmpCF4=Cyc_Absyn_qvar_cmp(_tmp74F->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;problem=_tmpCF4;});goto _LL5C;}else{goto _LL61;}}else{goto _LL61;}}else{goto _LL61;}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5F: _tmp750=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74E.f2)->f1)->f1).KnownDatatype).val;_LL60:
# 4477
({int _tmpCF5=Cyc_Absyn_qvar_cmp(_tmp750->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;problem=_tmpCF5;});goto _LL5C;}else{goto _LL61;}}else{goto _LL61;}}else{_LL61: _LL62:
# 4479
 problem=1;goto _LL5C;}}_LL5C:;}
# 4481
if(problem)
({void*_tmp751=0U;({unsigned int _tmpCF7=loc;struct _dyneither_ptr _tmpCF6=({const char*_tmp752="format attribute and vararg types don't match";_tag_dyneither(_tmp752,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpCF7,_tmpCF6,_tag_dyneither(_tmp751,sizeof(void*),0U));});});}}}
# 4489
{struct Cyc_List_List*rpo=_tmp7D4;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp754=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp755=_tmp754;void*_tmp757;void*_tmp756;_LL64: _tmp757=_tmp755->f1;_tmp756=_tmp755->f2;_LL65:;
({struct Cyc_Tcutil_CVTEnv _tmpCF8=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp720,& Cyc_Tcutil_ek,_tmp757,1,1);_tmp720=_tmpCF8;});
({struct Cyc_Tcutil_CVTEnv _tmpCF9=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp720,& Cyc_Tcutil_trk,_tmp756,1,1);_tmp720=_tmpCF9;});}}{
# 4497
struct Cyc_Tcenv_Fenv*_tmp758=Cyc_Tcenv_bogus_fenv(_tmp7D8,_tmp7D7);
struct Cyc_Tcenv_Tenv*_tmp759=({struct Cyc_Tcenv_Tenv*_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797->ns=te->ns,_tmp797->ae=te->ae,_tmp797->le=_tmp758,_tmp797->allow_valueof=1,_tmp797->in_extern_c_include=te->in_extern_c_include;_tmp797;});
struct _tuple32 _tmp75A=({unsigned int _tmpCFD=loc;struct Cyc_Tcenv_Tenv*_tmpCFC=_tmp759;struct Cyc_Tcutil_CVTEnv _tmpCFB=_tmp720;struct _dyneither_ptr _tmpCFA=({const char*_tmp796="@requires";_tag_dyneither(_tmp796,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpCFD,_tmpCFC,_tmpCFB,_tmpCFA,_tmp7D2);});struct _tuple32 _tmp75B=_tmp75A;struct Cyc_Tcutil_CVTEnv _tmp795;struct Cyc_List_List*_tmp794;_LL67: _tmp795=_tmp75B.f1;_tmp794=_tmp75B.f2;_LL68:;
_tmp720=_tmp795;
*_tmp7D1=_tmp794;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D7,_tmp794);{
# 4510
struct _tuple32 _tmp75C=({unsigned int _tmpD01=loc;struct Cyc_Tcenv_Tenv*_tmpD00=_tmp759;struct Cyc_Tcutil_CVTEnv _tmpCFF=_tmp720;struct _dyneither_ptr _tmpCFE=({const char*_tmp793="@ensures";_tag_dyneither(_tmp793,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpD01,_tmpD00,_tmpCFF,_tmpCFE,_tmp7D0);});struct _tuple32 _tmp75D=_tmp75C;struct Cyc_Tcutil_CVTEnv _tmp792;struct Cyc_List_List*_tmp791;_LL6A: _tmp792=_tmp75D.f1;_tmp791=_tmp75D.f2;_LL6B:;
_tmp720=_tmp792;
*_tmp7CF=_tmp791;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D7,_tmp791);
# 4515
if(*_tmp7DA != 0)
({struct Cyc_Tcutil_CVTEnv _tmpD02=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp720,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp7DA),1,1);_tmp720=_tmpD02;});else{
# 4518
struct Cyc_List_List*effect=0;
# 4523
{struct Cyc_List_List*tvs=_tmp720.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple30 _tmp75E=*((struct _tuple30*)tvs->hd);struct _tuple30 _tmp75F=_tmp75E;struct Cyc_Absyn_Tvar*_tmp76D;int _tmp76C;_LL6D: _tmp76D=_tmp75F.f1;_tmp76C=_tmp75F.f2;_LL6E:;
if(!_tmp76C)continue;{
void*_tmp760=Cyc_Absyn_compress_kb(_tmp76D->kind);void*_tmp761=_tmp760;struct Cyc_Core_Opt**_tmp76B;struct Cyc_Absyn_Kind*_tmp76A;struct Cyc_Core_Opt**_tmp769;struct Cyc_Core_Opt**_tmp768;struct Cyc_Absyn_Kind*_tmp767;switch(*((int*)_tmp761)){case 2U: _LL70: _tmp768=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761)->f1;_tmp767=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761)->f2;if(_tmp767->kind == Cyc_Absyn_RgnKind){_LL71:
# 4529
 if(_tmp767->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpD03=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp768=_tmpD03;});_tmp76A=_tmp767;goto _LL73;}
# 4532
({struct Cyc_Core_Opt*_tmpD04=Cyc_Tcutil_kind_to_bound_opt(_tmp767);*_tmp768=_tmpD04;});_tmp76A=_tmp767;goto _LL73;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761)->f2)->kind){case Cyc_Absyn_BoolKind: _LL74: _LL75:
# 4535
 goto _LL77;case Cyc_Absyn_PtrBndKind: _LL76: _LL77:
 goto _LL79;case Cyc_Absyn_IntKind: _LL78: _LL79:
 goto _LL7B;case Cyc_Absyn_EffKind: _LL80: _tmp769=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761)->f1;_LL81:
# 4542
({struct Cyc_Core_Opt*_tmpD05=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp769=_tmpD05;});goto _LL83;default: goto _LL86;}}case 0U: _LL72: _tmp76A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761)->f1;if(_tmp76A->kind == Cyc_Absyn_RgnKind){_LL73:
# 4534
({struct Cyc_List_List*_tmpD07=({struct Cyc_List_List*_tmp762=_cycalloc(sizeof(*_tmp762));({void*_tmpD06=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(_tmp76D));_tmp762->hd=_tmpD06;}),_tmp762->tl=effect;_tmp762;});effect=_tmpD07;});goto _LL6F;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7A: _LL7B:
# 4538
 goto _LL7D;case Cyc_Absyn_PtrBndKind: _LL7C: _LL7D:
 goto _LL7F;case Cyc_Absyn_IntKind: _LL7E: _LL7F:
 goto _LL6F;case Cyc_Absyn_EffKind: _LL82: _LL83:
# 4544
({struct Cyc_List_List*_tmpD09=({struct Cyc_List_List*_tmp763=_cycalloc(sizeof(*_tmp763));({void*_tmpD08=Cyc_Absyn_var_type(_tmp76D);_tmp763->hd=_tmpD08;}),_tmp763->tl=effect;_tmp763;});effect=_tmpD09;});goto _LL6F;default: _LL86: _LL87:
# 4549
({struct Cyc_List_List*_tmpD0B=({struct Cyc_List_List*_tmp766=_cycalloc(sizeof(*_tmp766));({void*_tmpD0A=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(_tmp76D));_tmp766->hd=_tmpD0A;}),_tmp766->tl=effect;_tmp766;});effect=_tmpD0B;});goto _LL6F;}}default: _LL84: _tmp76B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp761)->f1;_LL85:
# 4546
({struct Cyc_Core_Opt*_tmpD0D=({struct Cyc_Core_Opt*_tmp765=_cycalloc(sizeof(*_tmp765));({void*_tmpD0C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->tag=2U,_tmp764->f1=0,_tmp764->f2=& Cyc_Tcutil_ak;_tmp764;});_tmp765->v=_tmpD0C;});_tmp765;});*_tmp76B=_tmpD0D;});goto _LL87;}_LL6F:;};}}
# 4553
{struct Cyc_List_List*ts=_tmp720.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple31 _tmp76E=*((struct _tuple31*)ts->hd);struct _tuple31 _tmp76F=_tmp76E;void*_tmp776;int _tmp775;_LL89: _tmp776=_tmp76F.f1;_tmp775=_tmp76F.f2;_LL8A:;
if(!_tmp775)continue;{
struct Cyc_Absyn_Kind*_tmp770=Cyc_Tcutil_type_kind(_tmp776);struct Cyc_Absyn_Kind*_tmp771=_tmp770;switch(((struct Cyc_Absyn_Kind*)_tmp771)->kind){case Cyc_Absyn_RgnKind: _LL8C: _LL8D:
# 4558
({struct Cyc_List_List*_tmpD0F=({struct Cyc_List_List*_tmp772=_cycalloc(sizeof(*_tmp772));({void*_tmpD0E=Cyc_Absyn_access_eff(_tmp776);_tmp772->hd=_tmpD0E;}),_tmp772->tl=effect;_tmp772;});effect=_tmpD0F;});goto _LL8B;case Cyc_Absyn_EffKind: _LL8E: _LL8F:
# 4560
({struct Cyc_List_List*_tmpD10=({struct Cyc_List_List*_tmp773=_cycalloc(sizeof(*_tmp773));_tmp773->hd=_tmp776,_tmp773->tl=effect;_tmp773;});effect=_tmpD10;});goto _LL8B;case Cyc_Absyn_IntKind: _LL90: _LL91:
 goto _LL8B;default: _LL92: _LL93:
# 4563
({struct Cyc_List_List*_tmpD12=({struct Cyc_List_List*_tmp774=_cycalloc(sizeof(*_tmp774));({void*_tmpD11=Cyc_Absyn_regionsof_eff(_tmp776);_tmp774->hd=_tmpD11;}),_tmp774->tl=effect;_tmp774;});effect=_tmpD12;});goto _LL8B;}_LL8B:;};}}
# 4566
({void*_tmpD13=Cyc_Absyn_join_eff(effect);*_tmp7DA=_tmpD13;});}
# 4572
if(*_tmp7DB != 0){
struct Cyc_List_List*bs=*_tmp7DB;for(0;bs != 0;bs=bs->tl){
void*_tmp777=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp778=_tmp777;struct Cyc_Core_Opt**_tmp788;struct Cyc_Absyn_Kind*_tmp787;struct Cyc_Core_Opt**_tmp786;struct Cyc_Core_Opt**_tmp785;struct Cyc_Core_Opt**_tmp784;struct Cyc_Core_Opt**_tmp783;struct Cyc_Core_Opt**_tmp782;struct Cyc_Core_Opt**_tmp781;struct Cyc_Core_Opt**_tmp780;struct Cyc_Core_Opt**_tmp77F;struct Cyc_Core_Opt**_tmp77E;switch(*((int*)_tmp778)){case 1U: _LL95: _tmp77E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LL96:
# 4576
({struct Cyc_String_pa_PrintArg_struct _tmp77B=({struct Cyc_String_pa_PrintArg_struct _tmpA04;_tmpA04.tag=0U,_tmpA04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmpA04;});void*_tmp779[1U];_tmp779[0]=& _tmp77B;({unsigned int _tmpD15=loc;struct _dyneither_ptr _tmpD14=({const char*_tmp77A="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp77A,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpD15,_tmpD14,_tag_dyneither(_tmp779,sizeof(void*),1U));});});
# 4578
_tmp77F=_tmp77E;goto _LL98;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f2)->aliasqual == Cyc_Absyn_Top){_LL97: _tmp77F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LL98:
 _tmp780=_tmp77F;goto _LL9A;}else{goto _LLA7;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f2)->aliasqual){case Cyc_Absyn_Top: _LL99: _tmp780=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LL9A:
 _tmp781=_tmp780;goto _LL9C;case Cyc_Absyn_Aliasable: _LL9B: _tmp781=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LL9C:
 _tmp783=_tmp781;goto _LL9E;default: _LLA1: _tmp782=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LLA2:
# 4585
 _tmp785=_tmp782;goto _LLA4;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9D: _tmp783=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LL9E:
# 4582
 _tmp784=_tmp783;goto _LLA0;case Cyc_Absyn_Aliasable: _LL9F: _tmp784=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LLA0:
# 4584
({struct Cyc_Core_Opt*_tmpD16=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp784=_tmpD16;});goto _LL94;default: _LLA3: _tmp785=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LLA4:
# 4587
({struct Cyc_Core_Opt*_tmpD17=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp785=_tmpD17;});goto _LL94;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f2)->aliasqual == Cyc_Absyn_Top){_LLA5: _tmp786=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_LLA6:
# 4589
({struct Cyc_Core_Opt*_tmpD18=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp786=_tmpD18;});goto _LL94;}else{goto _LLA7;}default: _LLA7: _tmp788=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f1;_tmp787=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp778)->f2;_LLA8:
# 4591
({struct Cyc_Core_Opt*_tmpD19=Cyc_Tcutil_kind_to_bound_opt(_tmp787);*_tmp788=_tmpD19;});goto _LL94;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp778)->f1)->kind == Cyc_Absyn_MemKind){_LLA9: _LLAA:
# 4593
({void*_tmp77C=0U;({unsigned int _tmpD1B=loc;struct _dyneither_ptr _tmpD1A=({const char*_tmp77D="functions can't abstract M types";_tag_dyneither(_tmp77D,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpD1B,_tmpD1A,_tag_dyneither(_tmp77C,sizeof(void*),0U));});});goto _LL94;}else{_LLAB: _LLAC:
 goto _LL94;}}_LL94:;}}
# 4598
({struct Cyc_List_List*_tmpD1C=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp720.kind_env,*_tmp7DB);cvtenv.kind_env=_tmpD1C;});
({struct Cyc_List_List*_tmpD1D=Cyc_Tcutil_remove_bound_tvars_bool(_tmp720.r,_tmp720.free_vars,*_tmp7DB);_tmp720.free_vars=_tmpD1D;});
# 4601
{struct Cyc_List_List*tvs=_tmp720.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple30 _tmp789=*((struct _tuple30*)tvs->hd);struct _tuple30 _tmp78A=_tmp789;struct Cyc_Absyn_Tvar*_tmp78C;int _tmp78B;_LLAE: _tmp78C=_tmp78A.f1;_tmp78B=_tmp78A.f2;_LLAF:;
({struct Cyc_List_List*_tmpD1E=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp78C,_tmp78B);cvtenv.free_vars=_tmpD1E;});}}
# 4606
{struct Cyc_List_List*evs=_tmp720.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple31 _tmp78D=*((struct _tuple31*)evs->hd);struct _tuple31 _tmp78E=_tmp78D;void*_tmp790;int _tmp78F;_LLB1: _tmp790=_tmp78E.f1;_tmp78F=_tmp78E.f2;_LLB2:;
({struct Cyc_List_List*_tmpD1F=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp790,_tmp78F);cvtenv.free_evars=_tmpD1F;});}}
# 4610
goto _LL0;};};};}case 6U: _LL13: _tmp7DC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6DE)->f1;_LL14:
# 4613
 for(0;_tmp7DC != 0;_tmp7DC=_tmp7DC->tl){
struct _tuple12*_tmp798=(struct _tuple12*)_tmp7DC->hd;
({struct Cyc_Tcutil_CVTEnv _tmpD20=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp798).f2,1,0);cvtenv=_tmpD20;});
({int _tmpD21=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp798).f1).print_const,(*_tmp798).f2);
# 4616
((*_tmp798).f1).real_const=_tmpD21;});}
# 4619
goto _LL0;case 7U: _LL15: _tmp7DE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DE)->f1;_tmp7DD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DE)->f2;_LL16: {
# 4623
struct Cyc_List_List*prev_fields=0;
for(0;_tmp7DD != 0;_tmp7DD=_tmp7DD->tl){
struct Cyc_Absyn_Aggrfield*_tmp799=(struct Cyc_Absyn_Aggrfield*)_tmp7DD->hd;struct Cyc_Absyn_Aggrfield*_tmp79A=_tmp799;struct _dyneither_ptr*_tmp7AA;struct Cyc_Absyn_Tqual*_tmp7A9;void*_tmp7A8;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_List_List*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A5;_LLB4: _tmp7AA=_tmp79A->name;_tmp7A9=(struct Cyc_Absyn_Tqual*)& _tmp79A->tq;_tmp7A8=_tmp79A->type;_tmp7A7=_tmp79A->width;_tmp7A6=_tmp79A->attributes;_tmp7A5=_tmp79A->requires_clause;_LLB5:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7AA))
({struct Cyc_String_pa_PrintArg_struct _tmp79D=({struct Cyc_String_pa_PrintArg_struct _tmpA06;_tmpA06.tag=0U,_tmpA06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7AA);_tmpA06;});void*_tmp79B[1U];_tmp79B[0]=& _tmp79D;({unsigned int _tmpD23=loc;struct _dyneither_ptr _tmpD22=({const char*_tmp79C="duplicate field %s";_tag_dyneither(_tmp79C,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpD23,_tmpD22,_tag_dyneither(_tmp79B,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpD24=(struct _dyneither_ptr)*_tmp7AA;Cyc_strcmp(_tmpD24,({const char*_tmp79E="";_tag_dyneither(_tmp79E,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpD25=({struct Cyc_List_List*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F->hd=_tmp7AA,_tmp79F->tl=prev_fields;_tmp79F;});prev_fields=_tmpD25;});
({struct Cyc_Tcutil_CVTEnv _tmpD26=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7A8,1,0);cvtenv=_tmpD26;});
({int _tmpD27=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A9->print_const,_tmp7A8);_tmp7A9->real_const=_tmpD27;});
Cyc_Tcutil_check_bitfield(loc,te,_tmp7A8,_tmp7A7,_tmp7AA);
Cyc_Tcutil_check_field_atts(loc,_tmp7AA,_tmp7A6);
if(_tmp7A5 != 0){
# 4636
if(_tmp7DE != Cyc_Absyn_UnionA)
({void*_tmp7A0=0U;({unsigned int _tmpD29=loc;struct _dyneither_ptr _tmpD28=({const char*_tmp7A1="@requires clause is only allowed on union members";_tag_dyneither(_tmp7A1,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpD29,_tmpD28,_tag_dyneither(_tmp7A0,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpD2A=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD2A,0,_tmp7A5);});
if(!Cyc_Tcutil_is_integral(_tmp7A5))
({struct Cyc_String_pa_PrintArg_struct _tmp7A4=({struct Cyc_String_pa_PrintArg_struct _tmpA07;_tmpA07.tag=0U,({
struct _dyneither_ptr _tmpD2B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7A5->topt)));_tmpA07.f1=_tmpD2B;});_tmpA07;});void*_tmp7A2[1U];_tmp7A2[0]=& _tmp7A4;({unsigned int _tmpD2D=loc;struct _dyneither_ptr _tmpD2C=({const char*_tmp7A3="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp7A3,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpD2D,_tmpD2C,_tag_dyneither(_tmp7A2,sizeof(void*),1U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD2E=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7A5,te,cvtenv);cvtenv=_tmpD2E;});}}
# 4645
goto _LL0;}default: _LL17: _tmp7E2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DE)->f1;_tmp7E1=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DE)->f2;_tmp7E0=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DE)->f3;_tmp7DF=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DE)->f4;_LL18: {
# 4648
struct Cyc_List_List*targs=*_tmp7E1;
# 4650
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp7AB;_push_handler(& _tmp7AB);{int _tmp7AD=0;if(setjmp(_tmp7AB.handler))_tmp7AD=1;if(!_tmp7AD){({struct Cyc_Absyn_Typedefdecl*_tmpD2F=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7E2);td=_tmpD2F;});;_pop_handler();}else{void*_tmp7AC=(void*)_exn_thrown;void*_tmp7AE=_tmp7AC;void*_tmp7B2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7AE)->tag == Cyc_Dict_Absent){_LLB7: _LLB8:
# 4653
({struct Cyc_String_pa_PrintArg_struct _tmp7B1=({struct Cyc_String_pa_PrintArg_struct _tmpA08;_tmpA08.tag=0U,({struct _dyneither_ptr _tmpD30=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E2));_tmpA08.f1=_tmpD30;});_tmpA08;});void*_tmp7AF[1U];_tmp7AF[0]=& _tmp7B1;({unsigned int _tmpD32=loc;struct _dyneither_ptr _tmpD31=({const char*_tmp7B0="unbound typedef name %s";_tag_dyneither(_tmp7B0,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpD32,_tmpD31,_tag_dyneither(_tmp7AF,sizeof(void*),1U));});});
return cvtenv;}else{_LLB9: _tmp7B2=_tmp7AE;_LLBA:(int)_rethrow(_tmp7B2);}_LLB6:;}};}
# 4656
*_tmp7E0=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4661
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4666
({struct Cyc_Tcutil_CVTEnv _tmpD33=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpD33;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpD35=({struct Cyc_List_List*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));({struct _tuple15*_tmpD34=({struct _tuple15*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7B3->f2=(void*)ts->hd;_tmp7B3;});_tmp7B4->hd=_tmpD34;}),_tmp7B4->tl=inst;_tmp7B4;});inst=_tmpD35;});}
# 4670
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B7=({struct Cyc_String_pa_PrintArg_struct _tmpA09;_tmpA09.tag=0U,({struct _dyneither_ptr _tmpD36=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E2));_tmpA09.f1=_tmpD36;});_tmpA09;});void*_tmp7B5[1U];_tmp7B5[0]=& _tmp7B7;({unsigned int _tmpD38=loc;struct _dyneither_ptr _tmpD37=({const char*_tmp7B6="too many parameters for typedef %s";_tag_dyneither(_tmp7B6,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpD38,_tmpD37,_tag_dyneither(_tmp7B5,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4675
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpD39=({struct Cyc_List_List*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->hd=e,_tmp7B8->tl=hidden_ts;_tmp7B8;});hidden_ts=_tmpD39;});
({struct Cyc_Tcutil_CVTEnv _tmpD3A=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpD3A;});
({struct Cyc_List_List*_tmpD3C=({struct Cyc_List_List*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));({struct _tuple15*_tmpD3B=({struct _tuple15*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));_tmp7B9->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7B9->f2=e;_tmp7B9;});_tmp7BA->hd=_tmpD3B;}),_tmp7BA->tl=inst;_tmp7BA;});inst=_tmpD3C;});}
# 4683
({struct Cyc_List_List*_tmpD3E=({struct Cyc_List_List*_tmpD3D=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD3D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp7E1=_tmpD3E;});}
# 4685
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp7DF=new_typ;}
# 4691
goto _LL0;};}}_LL0:;}
# 4693
if(!({struct Cyc_Absyn_Kind*_tmpD3F=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmpD3F,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp7E5=({struct Cyc_String_pa_PrintArg_struct _tmpA0C;_tmpA0C.tag=0U,({
struct _dyneither_ptr _tmpD40=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA0C.f1=_tmpD40;});_tmpA0C;});struct Cyc_String_pa_PrintArg_struct _tmp7E6=({struct Cyc_String_pa_PrintArg_struct _tmpA0B;_tmpA0B.tag=0U,({struct _dyneither_ptr _tmpD41=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_type_kind(t)));_tmpA0B.f1=_tmpD41;});_tmpA0B;});struct Cyc_String_pa_PrintArg_struct _tmp7E7=({struct Cyc_String_pa_PrintArg_struct _tmpA0A;_tmpA0A.tag=0U,({struct _dyneither_ptr _tmpD42=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmpA0A.f1=_tmpD42;});_tmpA0A;});void*_tmp7E3[3U];_tmp7E3[0]=& _tmp7E5,_tmp7E3[1]=& _tmp7E6,_tmp7E3[2]=& _tmp7E7;({unsigned int _tmpD44=loc;struct _dyneither_ptr _tmpD43=({const char*_tmp7E4="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp7E4,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD44,_tmpD43,_tag_dyneither(_tmp7E3,sizeof(void*),3U));});});
# 4697
return cvtenv;}
# 4705
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4707
{void*_tmp7E8=e->r;void*_tmp7E9=_tmp7E8;struct Cyc_Absyn_Exp*_tmp7FC;struct Cyc_Absyn_Exp*_tmp7FB;void*_tmp7FA;void*_tmp7F9;void*_tmp7F8;void*_tmp7F7;struct Cyc_Absyn_Exp*_tmp7F6;struct Cyc_Absyn_Exp*_tmp7F5;struct Cyc_Absyn_Exp*_tmp7F4;struct Cyc_Absyn_Exp*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F2;struct Cyc_Absyn_Exp*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F0;struct Cyc_Absyn_Exp*_tmp7EF;struct Cyc_Absyn_Exp*_tmp7EE;struct Cyc_Absyn_Exp*_tmp7ED;struct Cyc_List_List*_tmp7EC;switch(*((int*)_tmp7E9)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp7EC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7E9)->f2;_LLC:
# 4714
 for(0;_tmp7EC != 0;_tmp7EC=_tmp7EC->tl){
({struct Cyc_Tcutil_CVTEnv _tmpD45=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp7EC->hd,te,cvtenv);cvtenv=_tmpD45;});}
goto _LL0;case 6U: _LLD: _tmp7EF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_tmp7EE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E9)->f2;_tmp7ED=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E9)->f3;_LLE:
# 4718
({struct Cyc_Tcutil_CVTEnv _tmpD46=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EF,te,cvtenv);cvtenv=_tmpD46;});
({struct Cyc_Tcutil_CVTEnv _tmpD47=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EE,te,cvtenv);cvtenv=_tmpD47;});
({struct Cyc_Tcutil_CVTEnv _tmpD48=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7ED,te,cvtenv);cvtenv=_tmpD48;});
goto _LL0;case 7U: _LLF: _tmp7F1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_tmp7F0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E9)->f2;_LL10:
 _tmp7F3=_tmp7F1;_tmp7F2=_tmp7F0;goto _LL12;case 8U: _LL11: _tmp7F3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_tmp7F2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E9)->f2;_LL12:
 _tmp7F5=_tmp7F3;_tmp7F4=_tmp7F2;goto _LL14;case 9U: _LL13: _tmp7F5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_tmp7F4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E9)->f2;_LL14:
# 4725
({struct Cyc_Tcutil_CVTEnv _tmpD49=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F5,te,cvtenv);cvtenv=_tmpD49;});
({struct Cyc_Tcutil_CVTEnv _tmpD4A=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F4,te,cvtenv);cvtenv=_tmpD4A;});
goto _LL0;case 14U: _LL15: _tmp7F7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_tmp7F6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E9)->f2;_LL16:
# 4729
({struct Cyc_Tcutil_CVTEnv _tmpD4B=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F6,te,cvtenv);cvtenv=_tmpD4B;});
({struct Cyc_Tcutil_CVTEnv _tmpD4C=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F7,1,0);cvtenv=_tmpD4C;});
goto _LL0;case 19U: _LL17: _tmp7F8=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_LL18:
 _tmp7F9=_tmp7F8;goto _LL1A;case 17U: _LL19: _tmp7F9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_LL1A:
# 4734
({struct Cyc_Tcutil_CVTEnv _tmpD4D=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F9,1,0);cvtenv=_tmpD4D;});
goto _LL0;case 39U: _LL1B: _tmp7FA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_LL1C:
# 4737
({struct Cyc_Tcutil_CVTEnv _tmpD4E=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7FA,1,0);cvtenv=_tmpD4E;});
goto _LL0;case 18U: _LL1D: _tmp7FB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_LL1E:
# 4740
({struct Cyc_Tcutil_CVTEnv _tmpD4F=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7FB,te,cvtenv);cvtenv=_tmpD4F;});
goto _LL0;case 41U: _LL1F: _tmp7FC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7E9)->f1;_LL20:
# 4743
({struct Cyc_Tcutil_CVTEnv _tmpD50=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7FC,te,cvtenv);cvtenv=_tmpD50;});
goto _LL0;default: _LL21: _LL22:
# 4746
({void*_tmp7EA=0U;({struct _dyneither_ptr _tmpD51=({const char*_tmp7EB="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp7EB,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD51,_tag_dyneither(_tmp7EA,sizeof(void*),0U));});});}_LL0:;}
# 4748
return cvtenv;}
# 4751
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4756
struct Cyc_List_List*_tmp7FD=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpD52=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpD52;});
# 4759
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple30 _tmp7FE=*((struct _tuple30*)vs->hd);struct _tuple30 _tmp7FF=_tmp7FE;struct Cyc_Absyn_Tvar*_tmp800;_LL1: _tmp800=_tmp7FF.f1;_LL2:;
({struct Cyc_List_List*_tmpD53=Cyc_Tcutil_fast_add_free_tvar(_tmp7FD,_tmp800);cvt.kind_env=_tmpD53;});}}
# 4767
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple31 _tmp801=*((struct _tuple31*)evs->hd);struct _tuple31 _tmp802=_tmp801;void*_tmp80B;_LL4: _tmp80B=_tmp802.f1;_LL5:;{
void*_tmp803=Cyc_Tcutil_compress(_tmp80B);void*_tmp804=_tmp803;struct Cyc_Core_Opt**_tmp80A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp804)->tag == 1U){_LL7: _tmp80A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp804)->f4;_LL8:
# 4771
 if(*_tmp80A == 0)
({struct Cyc_Core_Opt*_tmpD54=({struct Cyc_Core_Opt*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805->v=_tmp7FD;_tmp805;});*_tmp80A=_tmpD54;});else{
# 4775
struct Cyc_List_List*_tmp806=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp80A))->v;
struct Cyc_List_List*_tmp807=0;
for(0;_tmp806 != 0;_tmp806=_tmp806->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp7FD,(struct Cyc_Absyn_Tvar*)_tmp806->hd))
({struct Cyc_List_List*_tmpD55=({struct Cyc_List_List*_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808->hd=(struct Cyc_Absyn_Tvar*)_tmp806->hd,_tmp808->tl=_tmp807;_tmp808;});_tmp807=_tmpD55;});}
({struct Cyc_Core_Opt*_tmpD56=({struct Cyc_Core_Opt*_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809->v=_tmp807;_tmp809;});*_tmp80A=_tmpD56;});}
# 4782
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4786
return cvt;}
# 4793
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
{void*_tmp80C=Cyc_Tcutil_compress(e);void*_tmp80D=_tmp80C;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp80D)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
# 4799
 continue;}_LL0:;}{
# 4801
struct Cyc_Absyn_Kind*_tmp80E=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_tmp80F=_tmp80E;switch(((struct Cyc_Absyn_Kind*)_tmp80F)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp80F)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
# 4803
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_unique_rgn_type))
({void*_tmp810=0U;({struct _dyneither_ptr _tmpD57=({const char*_tmp811="can't unify evar with unique region!";_tag_dyneither(_tmp811,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD57,_tag_dyneither(_tmp810,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;default: _LLA: _LLB:
# 4808
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_heap_rgn_type))({void*_tmp812=0U;({struct _dyneither_ptr _tmpD58=({const char*_tmp813="can't unify evar with heap!";_tag_dyneither(_tmp813,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD58,_tag_dyneither(_tmp812,sizeof(void*),0U));});});
goto _LL5;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 4811
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp814=0U;({struct _dyneither_ptr _tmpD59=({const char*_tmp815="can't unify evar with {}!";_tag_dyneither(_tmp815,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD59,_tag_dyneither(_tmp814,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
# 4814
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_false_type))({struct Cyc_String_pa_PrintArg_struct _tmp818=({struct Cyc_String_pa_PrintArg_struct _tmpA0D;_tmpA0D.tag=0U,({
struct _dyneither_ptr _tmpD5A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmpA0D.f1=_tmpD5A;});_tmpA0D;});void*_tmp816[1U];_tmp816[0]=& _tmp818;({struct _dyneither_ptr _tmpD5B=({const char*_tmp817="can't unify evar %s with @false!";_tag_dyneither(_tmp817,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD5B,_tag_dyneither(_tmp816,sizeof(void*),1U));});});
goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
# 4818
 if(!({void*_tmpD5C=e;Cyc_Tcutil_unify(_tmpD5C,Cyc_Absyn_bounds_one());}))({void*_tmp819=0U;({struct _dyneither_ptr _tmpD5D=({const char*_tmp81A="can't unify evar with bounds_one!";_tag_dyneither(_tmp81A,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD5D,_tag_dyneither(_tmp819,sizeof(void*),0U));});});
goto _LL5;default: _LL12: _LL13:
# 4821
({struct Cyc_String_pa_PrintArg_struct _tmp81D=({struct Cyc_String_pa_PrintArg_struct _tmpA0F;_tmpA0F.tag=0U,({
struct _dyneither_ptr _tmpD5E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmpA0F.f1=_tmpD5E;});_tmpA0F;});struct Cyc_String_pa_PrintArg_struct _tmp81E=({struct Cyc_String_pa_PrintArg_struct _tmpA0E;_tmpA0E.tag=0U,({struct _dyneither_ptr _tmpD5F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmpA0E.f1=_tmpD5F;});_tmpA0E;});void*_tmp81B[2U];_tmp81B[0]=& _tmp81D,_tmp81B[1]=& _tmp81E;({unsigned int _tmpD61=loc;struct _dyneither_ptr _tmpD60=({const char*_tmp81C="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp81C,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpD61,_tmpD60,_tag_dyneither(_tmp81B,sizeof(void*),2U));});});
goto _LL5;}_LL5:;};}}
# 4833
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp81F=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp820=({unsigned int _tmpD65=loc;struct Cyc_Tcenv_Tenv*_tmpD64=te;struct Cyc_Tcutil_CVTEnv _tmpD63=({struct Cyc_Tcutil_CVTEnv _tmpA13;_tmpA13.r=Cyc_Core_heap_region,_tmpA13.kind_env=_tmp81F,_tmpA13.free_vars=0,_tmpA13.free_evars=0,_tmpA13.generalize_evars=generalize_evars,_tmpA13.fn_result=0;_tmpA13;});struct Cyc_Absyn_Kind*_tmpD62=expected_kind;Cyc_Tcutil_check_valid_type(_tmpD65,_tmpD64,_tmpD63,_tmpD62,1,t);});
# 4841
struct Cyc_List_List*_tmp821=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple30*))Cyc_Core_fst,_tmp820.free_vars);
struct Cyc_List_List*_tmp822=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple31*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple31*))Cyc_Core_fst,_tmp820.free_evars);
# 4845
if(_tmp81F != 0){
struct Cyc_List_List*_tmp823=0;
{struct Cyc_List_List*_tmp824=_tmp821;for(0;(unsigned int)_tmp824;_tmp824=_tmp824->tl){
struct Cyc_Absyn_Tvar*_tmp825=(struct Cyc_Absyn_Tvar*)_tmp824->hd;
int found=0;
{struct Cyc_List_List*_tmp826=_tmp81F;for(0;(unsigned int)_tmp826;_tmp826=_tmp826->tl){
if(Cyc_Absyn_tvar_cmp(_tmp825,(struct Cyc_Absyn_Tvar*)_tmp826->hd)== 0){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpD66=({struct Cyc_List_List*_tmp827=_cycalloc(sizeof(*_tmp827));_tmp827->hd=(struct Cyc_Absyn_Tvar*)_tmp824->hd,_tmp827->tl=_tmp823;_tmp827;});_tmp823=_tmpD66;});}}
# 4855
({struct Cyc_List_List*_tmpD67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp823);_tmp821=_tmpD67;});}
# 4861
{struct Cyc_List_List*x=_tmp821;for(0;x != 0;x=x->tl){
void*_tmp828=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp829=_tmp828;enum Cyc_Absyn_AliasQual _tmp837;struct Cyc_Core_Opt**_tmp836;struct Cyc_Absyn_Kind*_tmp835;struct Cyc_Core_Opt**_tmp834;struct Cyc_Core_Opt**_tmp833;struct Cyc_Core_Opt**_tmp832;struct Cyc_Core_Opt**_tmp831;struct Cyc_Core_Opt**_tmp830;struct Cyc_Core_Opt**_tmp82F;switch(*((int*)_tmp829)){case 1U: _LL1: _tmp82F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp829)->f1;_LL2:
 _tmp830=_tmp82F;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp830=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f1;_LL4:
 _tmp831=_tmp830;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp831=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f1;_LL6:
 _tmp832=_tmp831;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp832=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f1;_LL8:
# 4867
({struct Cyc_Core_Opt*_tmpD68=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp832=_tmpD68;});goto _LL0;default: _LL9: _tmp833=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f1;_LLA:
# 4869
({struct Cyc_Core_Opt*_tmpD69=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp833=_tmpD69;});goto _LL0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp834=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f1;_LLC:
# 4871
({struct Cyc_Core_Opt*_tmpD6A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp834=_tmpD6A;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp836=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f1;_tmp835=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp829)->f2;_LLE:
# 4873
({struct Cyc_Core_Opt*_tmpD6B=Cyc_Tcutil_kind_to_bound_opt(_tmp835);*_tmp836=_tmpD6B;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp829)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp837=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp829)->f1)->aliasqual;_LL10:
# 4875
({struct Cyc_String_pa_PrintArg_struct _tmp82C=({struct Cyc_String_pa_PrintArg_struct _tmpA11;_tmpA11.tag=0U,({
struct _dyneither_ptr _tmpD6C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmpA11.f1=_tmpD6C;});_tmpA11;});struct Cyc_String_pa_PrintArg_struct _tmp82D=({struct Cyc_String_pa_PrintArg_struct _tmpA10;_tmpA10.tag=0U,({struct _dyneither_ptr _tmpD6D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp82E=_cycalloc(sizeof(*_tmp82E));_tmp82E->kind=Cyc_Absyn_MemKind,_tmp82E->aliasqual=_tmp837;_tmp82E;})));_tmpA10.f1=_tmpD6D;});_tmpA10;});void*_tmp82A[2U];_tmp82A[0]=& _tmp82C,_tmp82A[1]=& _tmp82D;({unsigned int _tmpD6F=loc;struct _dyneither_ptr _tmpD6E=({const char*_tmp82B="type variable %s cannot have kind %s";_tag_dyneither(_tmp82B,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD6F,_tmpD6E,_tag_dyneither(_tmp82A,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4882
if(_tmp821 != 0  || _tmp822 != 0){
{void*_tmp838=Cyc_Tcutil_compress(t);void*_tmp839=_tmp838;struct Cyc_List_List**_tmp83A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp839)->tag == 5U){_LL14: _tmp83A=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp839)->f1).tvars;_LL15:
# 4885
 if(*_tmp83A == 0){
# 4887
({struct Cyc_List_List*_tmpD70=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp821);*_tmp83A=_tmpD70;});
_tmp821=0;}
# 4890
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4893
if(_tmp821 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp83D=({struct Cyc_String_pa_PrintArg_struct _tmpA12;_tmpA12.tag=0U,_tmpA12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp821->hd)->name);_tmpA12;});void*_tmp83B[1U];_tmp83B[0]=& _tmp83D;({unsigned int _tmpD72=loc;struct _dyneither_ptr _tmpD71=({const char*_tmp83C="unbound type variable %s";_tag_dyneither(_tmp83C,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpD72,_tmpD71,_tag_dyneither(_tmp83B,sizeof(void*),1U));});});
Cyc_Tcutil_check_free_evars(_tmp822,t,loc);}}
# 4903
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 4906
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp83E=Cyc_Tcutil_compress(t);void*_tmp83F=_tmp83E;struct Cyc_List_List*_tmp84B;void*_tmp84A;struct Cyc_Absyn_Tqual _tmp849;void*_tmp848;struct Cyc_List_List*_tmp847;struct Cyc_Absyn_Exp*_tmp846;struct Cyc_List_List*_tmp845;struct Cyc_Absyn_Exp*_tmp844;struct Cyc_List_List*_tmp843;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->tag == 5U){_LL1: _tmp84B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).tvars;_tmp84A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).effect;_tmp849=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).ret_tqual;_tmp848=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).ret_type;_tmp847=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).args;_tmp846=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).requires_clause;_tmp845=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).requires_relns;_tmp844=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).ensures_clause;_tmp843=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83F)->f1).ensures_relns;_LL2:
# 4909
 fd->tvs=_tmp84B;
fd->effect=_tmp84A;
{struct Cyc_List_List*_tmp840=fd->args;for(0;_tmp840 != 0;(_tmp840=_tmp840->tl,_tmp847=_tmp847->tl)){
# 4913
(*((struct _tuple10*)_tmp840->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp847))->hd)).f2;
(*((struct _tuple10*)_tmp840->hd)).f3=(*((struct _tuple10*)_tmp847->hd)).f3;}}
# 4916
fd->ret_tqual=_tmp849;
fd->ret_type=_tmp848;
({int _tmpD73=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp848);(fd->ret_tqual).real_const=_tmpD73;});
fd->requires_clause=_tmp846;
fd->requires_relns=_tmp845;
fd->ensures_clause=_tmp844;
fd->ensures_relns=_tmp843;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp841=0U;({struct _dyneither_ptr _tmpD74=({const char*_tmp842="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp842,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD74,_tag_dyneither(_tmp841,sizeof(void*),0U));});});}_LL0:;}
# 4926
({struct Cyc_List_List*_tmpD76=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpD75=loc;Cyc_Tcutil_check_unique_vars(_tmpD76,_tmpD75,({const char*_tmp84C="function declaration has repeated parameter";_tag_dyneither(_tmp84C,sizeof(char),44U);}));});
# 4929
fd->cached_type=t;}
# 4934
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4940
struct Cyc_Tcutil_CVTEnv _tmp84D=({unsigned int _tmpD7B=loc;struct Cyc_Tcenv_Tenv*_tmpD7A=te;struct Cyc_Tcutil_CVTEnv _tmpD79=({struct Cyc_Tcutil_CVTEnv _tmpA16;_tmpA16.r=Cyc_Core_heap_region,_tmpA16.kind_env=bound_tvars,_tmpA16.free_vars=0,_tmpA16.free_evars=0,_tmpA16.generalize_evars=0,_tmpA16.fn_result=0;_tmpA16;});struct Cyc_Absyn_Kind*_tmpD78=expected_kind;int _tmpD77=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpD7B,_tmpD7A,_tmpD79,_tmpD78,_tmpD77,t);});
# 4944
struct Cyc_List_List*_tmp84E=({struct _RegionHandle*_tmpD7D=Cyc_Core_heap_region;struct Cyc_List_List*_tmpD7C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple30*))Cyc_Core_fst,_tmp84D.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpD7D,_tmpD7C,bound_tvars);});
# 4946
struct Cyc_List_List*_tmp84F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple31*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple31*))Cyc_Core_fst,_tmp84D.free_evars);
{struct Cyc_List_List*fs=_tmp84E;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp850=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp853=({struct Cyc_String_pa_PrintArg_struct _tmpA15;_tmpA15.tag=0U,_tmpA15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp850);_tmpA15;});struct Cyc_String_pa_PrintArg_struct _tmp854=({struct Cyc_String_pa_PrintArg_struct _tmpA14;_tmpA14.tag=0U,({struct _dyneither_ptr _tmpD7E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA14.f1=_tmpD7E;});_tmpA14;});void*_tmp851[2U];_tmp851[0]=& _tmp853,_tmp851[1]=& _tmp854;({unsigned int _tmpD80=loc;struct _dyneither_ptr _tmpD7F=({const char*_tmp852="unbound type variable %s in type %s";_tag_dyneither(_tmp852,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpD80,_tmpD7F,_tag_dyneither(_tmp851,sizeof(void*),2U));});});}}
# 4951
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp84F,t,loc);}
# 4955
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpD81=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpD81;});}
# 4960
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4966
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4969
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp857=({struct Cyc_String_pa_PrintArg_struct _tmpA18;_tmpA18.tag=0U,_tmpA18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpA18;});struct Cyc_String_pa_PrintArg_struct _tmp858=({struct Cyc_String_pa_PrintArg_struct _tmpA17;_tmpA17.tag=0U,({struct _dyneither_ptr _tmpD82=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmpA17.f1=_tmpD82;});_tmpA17;});void*_tmp855[2U];_tmp855[0]=& _tmp857,_tmp855[1]=& _tmp858;({unsigned int _tmpD84=loc;struct _dyneither_ptr _tmpD83=({const char*_tmp856="%s: %s";_tag_dyneither(_tmp856,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpD84,_tmpD83,_tag_dyneither(_tmp855,sizeof(void*),2U));});});}}}
# 4975
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4979
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4983
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpD86=tvs;unsigned int _tmpD85=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpD86,_tmpD85,Cyc_Absynpp_tvar2string,({const char*_tmp859="duplicate type variable";_tag_dyneither(_tmp859,sizeof(char),24U);}));});}struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple35{struct Cyc_List_List*f1;void*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4997 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5002
struct _RegionHandle _tmp85A=_new_region("temp");struct _RegionHandle*temp=& _tmp85A;_push_region(temp);
# 5006
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpD87=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpD87,({const char*_tmp85B="";_tag_dyneither(_tmp85B,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpD89=({struct Cyc_List_List*_tmp85D=_region_malloc(temp,sizeof(*_tmp85D));({struct _tuple34*_tmpD88=({struct _tuple34*_tmp85C=_region_malloc(temp,sizeof(*_tmp85C));_tmp85C->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp85C->f2=0;_tmp85C;});_tmp85D->hd=_tmpD88;}),_tmp85D->tl=fields;_tmp85D;});fields=_tmpD89;});}}
# 5011
({struct Cyc_List_List*_tmpD8A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpD8A;});{
# 5013
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp882="struct";_tag_dyneither(_tmp882,sizeof(char),7U);}):({const char*_tmp883="union";_tag_dyneither(_tmp883,sizeof(char),6U);});
# 5016
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple35*_tmp85E=(struct _tuple35*)des->hd;struct _tuple35*_tmp85F=_tmp85E;struct Cyc_List_List*_tmp87A;void*_tmp879;_LL1: _tmp87A=_tmp85F->f1;_tmp879=_tmp85F->f2;_LL2:;
if(_tmp87A == 0){
# 5021
struct Cyc_List_List*_tmp860=fields;
for(0;_tmp860 != 0;_tmp860=_tmp860->tl){
if(!(*((struct _tuple34*)_tmp860->hd)).f2){
(*((struct _tuple34*)_tmp860->hd)).f2=1;
({struct Cyc_List_List*_tmpD8C=({struct Cyc_List_List*_tmp862=_cycalloc(sizeof(*_tmp862));({void*_tmpD8B=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp861=_cycalloc(sizeof(*_tmp861));_tmp861->tag=1U,_tmp861->f1=((*((struct _tuple34*)_tmp860->hd)).f1)->name;_tmp861;});_tmp862->hd=_tmpD8B;}),_tmp862->tl=0;_tmp862;});(*((struct _tuple35*)des->hd)).f1=_tmpD8C;});
({struct Cyc_List_List*_tmpD8E=({struct Cyc_List_List*_tmp864=_region_malloc(rgn,sizeof(*_tmp864));({struct _tuple36*_tmpD8D=({struct _tuple36*_tmp863=_region_malloc(rgn,sizeof(*_tmp863));_tmp863->f1=(*((struct _tuple34*)_tmp860->hd)).f1,_tmp863->f2=_tmp879;_tmp863;});_tmp864->hd=_tmpD8D;}),_tmp864->tl=ans;_tmp864;});ans=_tmpD8E;});
break;}}
# 5029
if(_tmp860 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp867=({struct Cyc_String_pa_PrintArg_struct _tmpA19;_tmpA19.tag=0U,_tmpA19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpA19;});void*_tmp865[1U];_tmp865[0]=& _tmp867;({unsigned int _tmpD90=loc;struct _dyneither_ptr _tmpD8F=({const char*_tmp866="too many arguments to %s";_tag_dyneither(_tmp866,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpD90,_tmpD8F,_tag_dyneither(_tmp865,sizeof(void*),1U));});});}else{
if(_tmp87A->tl != 0)
# 5033
({void*_tmp868=0U;({unsigned int _tmpD92=loc;struct _dyneither_ptr _tmpD91=({const char*_tmp869="multiple designators are not yet supported";_tag_dyneither(_tmp869,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpD92,_tmpD91,_tag_dyneither(_tmp868,sizeof(void*),0U));});});else{
# 5036
void*_tmp86A=(void*)_tmp87A->hd;void*_tmp86B=_tmp86A;struct _dyneither_ptr*_tmp878;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp86B)->tag == 0U){_LL4: _LL5:
# 5038
({struct Cyc_String_pa_PrintArg_struct _tmp86E=({struct Cyc_String_pa_PrintArg_struct _tmpA1A;_tmpA1A.tag=0U,_tmpA1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpA1A;});void*_tmp86C[1U];_tmp86C[0]=& _tmp86E;({unsigned int _tmpD94=loc;struct _dyneither_ptr _tmpD93=({const char*_tmp86D="array designator used in argument to %s";_tag_dyneither(_tmp86D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpD94,_tmpD93,_tag_dyneither(_tmp86C,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp878=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp86B)->f1;_LL7: {
# 5041
struct Cyc_List_List*_tmp86F=fields;
for(0;_tmp86F != 0;_tmp86F=_tmp86F->tl){
if(Cyc_strptrcmp(_tmp878,((*((struct _tuple34*)_tmp86F->hd)).f1)->name)== 0){
if((*((struct _tuple34*)_tmp86F->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp872=({struct Cyc_String_pa_PrintArg_struct _tmpA1B;_tmpA1B.tag=0U,_tmpA1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp878);_tmpA1B;});void*_tmp870[1U];_tmp870[0]=& _tmp872;({unsigned int _tmpD96=loc;struct _dyneither_ptr _tmpD95=({const char*_tmp871="member %s has already been used as an argument";_tag_dyneither(_tmp871,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD96,_tmpD95,_tag_dyneither(_tmp870,sizeof(void*),1U));});});
(*((struct _tuple34*)_tmp86F->hd)).f2=1;
({struct Cyc_List_List*_tmpD98=({struct Cyc_List_List*_tmp874=_region_malloc(rgn,sizeof(*_tmp874));({struct _tuple36*_tmpD97=({struct _tuple36*_tmp873=_region_malloc(rgn,sizeof(*_tmp873));_tmp873->f1=(*((struct _tuple34*)_tmp86F->hd)).f1,_tmp873->f2=_tmp879;_tmp873;});_tmp874->hd=_tmpD97;}),_tmp874->tl=ans;_tmp874;});ans=_tmpD98;});
break;}}
# 5050
if(_tmp86F == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp877=({struct Cyc_String_pa_PrintArg_struct _tmpA1C;_tmpA1C.tag=0U,_tmpA1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp878);_tmpA1C;});void*_tmp875[1U];_tmp875[0]=& _tmp877;({unsigned int _tmpD9A=loc;struct _dyneither_ptr _tmpD99=({const char*_tmp876="bad field designator %s";_tag_dyneither(_tmp876,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpD9A,_tmpD99,_tag_dyneither(_tmp875,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 5055
if(aggr_kind == Cyc_Absyn_StructA)
# 5057
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple34*)fields->hd)).f2){
({void*_tmp87B=0U;({unsigned int _tmpD9C=loc;struct _dyneither_ptr _tmpD9B=({const char*_tmp87C="too few arguments to struct";_tag_dyneither(_tmp87C,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpD9C,_tmpD9B,_tag_dyneither(_tmp87B,sizeof(void*),0U));});});
break;}}else{
# 5064
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple34*)fields->hd)).f2){
if(found)({void*_tmp87D=0U;({unsigned int _tmpD9E=loc;struct _dyneither_ptr _tmpD9D=({const char*_tmp87E="only one member of a union is allowed";_tag_dyneither(_tmp87E,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpD9E,_tmpD9D,_tag_dyneither(_tmp87D,sizeof(void*),0U));});});
found=1;}}
# 5071
if(!found)({void*_tmp87F=0U;({unsigned int _tmpDA0=loc;struct _dyneither_ptr _tmpD9F=({const char*_tmp880="missing member for union";_tag_dyneither(_tmp880,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpDA0,_tmpD9F,_tag_dyneither(_tmp87F,sizeof(void*),0U));});});}{
# 5074
struct Cyc_List_List*_tmp881=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp881;};};}
# 5006
;_pop_region(temp);}
# 5080
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5082
void*_tmp884=e1->r;void*_tmp885=_tmp884;struct Cyc_Absyn_Exp*_tmp897;struct Cyc_Absyn_Exp*_tmp896;struct Cyc_Absyn_Exp*_tmp895;struct Cyc_Absyn_Exp*_tmp894;struct Cyc_Absyn_Exp*_tmp893;struct Cyc_Absyn_Exp*_tmp892;switch(*((int*)_tmp885)){case 14U: _LL1: _LL2:
# 5084
({struct Cyc_String_pa_PrintArg_struct _tmp888=({struct Cyc_String_pa_PrintArg_struct _tmpA1D;_tmpA1D.tag=0U,({struct _dyneither_ptr _tmpDA1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA1D.f1=_tmpDA1;});_tmpA1D;});void*_tmp886[1U];_tmp886[0]=& _tmp888;({struct _dyneither_ptr _tmpDA2=({const char*_tmp887="we have a cast in a lhs:  %s";_tag_dyneither(_tmp887,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA2,_tag_dyneither(_tmp886,sizeof(void*),1U));});});case 20U: _LL3: _tmp892=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp885)->f1;_LL4:
 _tmp893=_tmp892;goto _LL6;case 23U: _LL5: _tmp893=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp885)->f1;_LL6:
# 5087
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp893->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp894=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp885)->f1;_LL8:
 _tmp895=_tmp894;goto _LLA;case 21U: _LL9: _tmp895=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp885)->f1;_LLA:
# 5091
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp895->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp88B=({struct Cyc_String_pa_PrintArg_struct _tmpA1E;_tmpA1E.tag=0U,({
struct _dyneither_ptr _tmpDA3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA1E.f1=_tmpDA3;});_tmpA1E;});void*_tmp889[1U];_tmp889[0]=& _tmp88B;({struct _dyneither_ptr _tmpDA4=({const char*_tmp88A="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp88A,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA4,_tag_dyneither(_tmp889,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp896=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp885)->f1;_LLC:
 _tmp897=_tmp896;goto _LLE;case 12U: _LLD: _tmp897=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp885)->f1;_LLE:
# 5097
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp897->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp88E=({struct Cyc_String_pa_PrintArg_struct _tmpA1F;_tmpA1F.tag=0U,({
struct _dyneither_ptr _tmpDA5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA1F.f1=_tmpDA5;});_tmpA1F;});void*_tmp88C[1U];_tmp88C[0]=& _tmp88E;({struct _dyneither_ptr _tmpDA6=({const char*_tmp88D="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp88D,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA6,_tag_dyneither(_tmp88C,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp891=({struct Cyc_String_pa_PrintArg_struct _tmpA20;_tmpA20.tag=0U,({struct _dyneither_ptr _tmpDA7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA20.f1=_tmpDA7;});_tmpA20;});void*_tmp88F[1U];_tmp88F[0]=& _tmp891;({struct _dyneither_ptr _tmpDA8=({const char*_tmp890="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp890,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA8,_tag_dyneither(_tmp88F,sizeof(void*),1U));});});}_LL0:;}
# 5112
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5115
void*_tmp898=Cyc_Tcutil_compress(r);void*_tmp899=_tmp898;struct Cyc_Absyn_Tvar*_tmp8A5;enum Cyc_Absyn_KindQual _tmp8A4;enum Cyc_Absyn_AliasQual _tmp8A3;switch(*((int*)_tmp899)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp899)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp899)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp899)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp899)->f4 == 0){_LL5: _tmp8A4=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp899)->f3)->kind)->v)->kind;_tmp8A3=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp899)->f3)->kind)->v)->aliasqual;_LL6:
# 5119
 return _tmp8A4 == Cyc_Absyn_RgnKind  && (_tmp8A3 == Cyc_Absyn_Unique  || _tmp8A3 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp8A5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp899)->f1;_LL8: {
# 5123
struct Cyc_Absyn_Kind*_tmp89A=Cyc_Tcutil_tvar_kind(_tmp8A5,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp89B=_tmp89A;enum Cyc_Absyn_KindQual _tmp8A2;enum Cyc_Absyn_AliasQual _tmp8A1;_LLC: _tmp8A2=_tmp89B->kind;_tmp8A1=_tmp89B->aliasqual;_LLD:;
if(_tmp8A2 == Cyc_Absyn_RgnKind  && (_tmp8A1 == Cyc_Absyn_Unique  || _tmp8A1 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp89C=Cyc_Absyn_compress_kb(_tmp8A5->kind);void*_tmp89D=_tmp89C;struct Cyc_Core_Opt**_tmp8A0;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp8A0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL10:
# 5127
({struct Cyc_Core_Opt*_tmpDAA=({struct Cyc_Core_Opt*_tmp89F=_cycalloc(sizeof(*_tmp89F));({void*_tmpDA9=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E->tag=2U,_tmp89E->f1=0,_tmp89E->f2=& Cyc_Tcutil_rk;_tmp89E;});_tmp89F->v=_tmpDA9;});_tmp89F;});*_tmp8A0=_tmpDAA;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5132
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5139
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp8A6=Cyc_Tcutil_compress(t);void*_tmp8A7=_tmp8A6;struct Cyc_Absyn_Tvar*_tmp8B5;void*_tmp8B4;switch(*((int*)_tmp8A7)){case 3U: _LL1: _tmp8B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A7)->f1).ptr_atts).rgn;_LL2:
# 5142
 return Cyc_Tcutil_is_noalias_region(_tmp8B4,must_be_unique);case 2U: _LL3: _tmp8B5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8A7)->f1;_LL4: {
# 5144
struct Cyc_Absyn_Kind*_tmp8A8=Cyc_Tcutil_tvar_kind(_tmp8B5,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp8A9=_tmp8A8;enum Cyc_Absyn_KindQual _tmp8B3;enum Cyc_Absyn_AliasQual _tmp8B2;_LL8: _tmp8B3=_tmp8A9->kind;_tmp8B2=_tmp8A9->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp8AA=_tmp8B3;switch(_tmp8AA){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if(_tmp8B2 == Cyc_Absyn_Unique  || _tmp8B2 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp8AB=Cyc_Absyn_compress_kb(_tmp8B5->kind);void*_tmp8AC=_tmp8AB;struct Cyc_Core_Opt**_tmp8B1;enum Cyc_Absyn_KindQual _tmp8B0;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AC)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AC)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp8B1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AC)->f1;_tmp8B0=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AC)->f2)->kind;_LL15:
# 5150
({struct Cyc_Core_Opt*_tmpDAD=({struct Cyc_Core_Opt*_tmp8AF=_cycalloc(sizeof(*_tmp8AF));({void*_tmpDAC=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8AE=_cycalloc(sizeof(*_tmp8AE));_tmp8AE->tag=2U,_tmp8AE->f1=0,({struct Cyc_Absyn_Kind*_tmpDAB=({struct Cyc_Absyn_Kind*_tmp8AD=_cycalloc(sizeof(*_tmp8AD));_tmp8AD->kind=_tmp8B0,_tmp8AD->aliasqual=Cyc_Absyn_Aliasable;_tmp8AD;});_tmp8AE->f2=_tmpDAB;});_tmp8AE;});_tmp8AF->v=_tmpDAC;});_tmp8AF;});*_tmp8B1=_tmpDAD;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5154
 return 1;}_LL13:;}
# 5157
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5160
 return 0;}_LL0:;}
# 5163
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp8B6=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp8B6,0))return 1;{
void*_tmp8B7=_tmp8B6;struct Cyc_List_List*_tmp8CB;union Cyc_Absyn_DatatypeFieldInfo _tmp8CA;struct Cyc_List_List*_tmp8C9;union Cyc_Absyn_DatatypeInfo _tmp8C8;struct Cyc_List_List*_tmp8C7;struct Cyc_Absyn_Aggrdecl**_tmp8C6;struct Cyc_List_List*_tmp8C5;struct Cyc_List_List*_tmp8C4;switch(*((int*)_tmp8B7)){case 6U: _LL1: _tmp8C4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8B7)->f1;_LL2:
# 5168
 while(_tmp8C4 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp8C4->hd)).f2))return 1;
_tmp8C4=_tmp8C4->tl;}
# 5172
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp8C6=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f1)->f1).KnownAggr).val;_tmp8C5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f2;_LL4:
# 5174
 if((*_tmp8C6)->impl == 0)return 0;else{
# 5176
struct Cyc_List_List*_tmp8B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp8C6)->tvs,_tmp8C5);
struct Cyc_List_List*_tmp8B9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp8C6)->impl))->fields;
void*t;
while(_tmp8B9 != 0){
({void*_tmpDAE=_tmp8B8 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp8B9->hd)->type: Cyc_Tcutil_substitute(_tmp8B8,((struct Cyc_Absyn_Aggrfield*)_tmp8B9->hd)->type);t=_tmpDAE;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp8B9=_tmp8B9->tl;}
# 5184
return 0;}}else{_LL7: _LL8:
# 5194
 return 0;}case 18U: _LL9: _tmp8C8=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f1)->f1;_tmp8C7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f2;_LLA: {
# 5196
union Cyc_Absyn_DatatypeInfo _tmp8BA=_tmp8C8;struct Cyc_List_List*_tmp8BE;struct Cyc_Core_Opt*_tmp8BD;struct _tuple2*_tmp8BC;int _tmp8BB;if((_tmp8BA.UnknownDatatype).tag == 1){_LL10: _tmp8BC=((_tmp8BA.UnknownDatatype).val).name;_tmp8BB=((_tmp8BA.UnknownDatatype).val).is_extensible;_LL11:
# 5199
 return 0;}else{_LL12: _tmp8BE=(*(_tmp8BA.KnownDatatype).val)->tvs;_tmp8BD=(*(_tmp8BA.KnownDatatype).val)->fields;_LL13:
# 5202
 return 0;}_LLF:;}case 19U: _LLB: _tmp8CA=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f1)->f1;_tmp8C9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B7)->f2;_LLC: {
# 5205
union Cyc_Absyn_DatatypeFieldInfo _tmp8BF=_tmp8CA;struct Cyc_Absyn_Datatypedecl*_tmp8C3;struct Cyc_Absyn_Datatypefield*_tmp8C2;if((_tmp8BF.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5208
 return 0;}else{_LL17: _tmp8C3=((_tmp8BF.KnownDatatypefield).val).f1;_tmp8C2=((_tmp8BF.KnownDatatypefield).val).f2;_LL18: {
# 5210
struct Cyc_List_List*_tmp8C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp8C3->tvs,_tmp8C9);
struct Cyc_List_List*_tmp8C1=_tmp8C2->typs;
while(_tmp8C1 != 0){
({void*_tmpDAF=_tmp8C0 == 0?(*((struct _tuple12*)_tmp8C1->hd)).f2: Cyc_Tcutil_substitute(_tmp8C0,(*((struct _tuple12*)_tmp8C1->hd)).f2);_tmp8B6=_tmpDAF;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp8B6))return 1;
_tmp8C1=_tmp8C1->tl;}
# 5217
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp8CB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8B7)->f2;_LL6:
# 5187
 while(_tmp8CB != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp8CB->hd)->type))return 1;
_tmp8CB=_tmp8CB->tl;}
# 5191
return 0;default: _LLD: _LLE:
# 5219
 return 0;}_LL0:;};}
# 5226
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp8CC=e->r;void*_tmp8CD=_tmp8CC;struct Cyc_Absyn_Stmt*_tmp8E3;struct Cyc_Absyn_Exp*_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E1;struct Cyc_Absyn_Exp*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8DE;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DC;struct _dyneither_ptr*_tmp8DB;struct Cyc_Absyn_Exp*_tmp8DA;struct Cyc_Absyn_Exp*_tmp8D9;switch(*((int*)_tmp8CD)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8CD)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp8D9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8CD)->f1;_LL4:
 _tmp8DA=_tmp8D9;goto _LL6;case 20U: _LL5: _tmp8DA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8CD)->f1;_LL6:
# 5231
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp8DA->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp8DA);case 21U: _LL7: _tmp8DC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8CD)->f1;_tmp8DB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8CD)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp8DC);case 23U: _LL9: _tmp8DE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8CD)->f1;_tmp8DD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8CD)->f2;_LLA: {
# 5234
void*_tmp8CE=Cyc_Tcutil_compress((void*)_check_null(_tmp8DE->topt));void*_tmp8CF=_tmp8CE;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8CF)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp8DE);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp8E0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8CD)->f2;_tmp8DF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8CD)->f3;_LLC:
# 5239
 return Cyc_Tcutil_is_noalias_path(_tmp8E0) && Cyc_Tcutil_is_noalias_path(_tmp8DF);case 9U: _LLD: _tmp8E1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8CD)->f2;_LLE:
 _tmp8E2=_tmp8E1;goto _LL10;case 14U: _LLF: _tmp8E2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8CD)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp8E2);case 37U: _LL11: _tmp8E3=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8CD)->f1;_LL12:
# 5243
 while(1){
void*_tmp8D0=_tmp8E3->r;void*_tmp8D1=_tmp8D0;struct Cyc_Absyn_Exp*_tmp8D8;struct Cyc_Absyn_Decl*_tmp8D7;struct Cyc_Absyn_Stmt*_tmp8D6;struct Cyc_Absyn_Stmt*_tmp8D5;struct Cyc_Absyn_Stmt*_tmp8D4;switch(*((int*)_tmp8D1)){case 2U: _LL1B: _tmp8D5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D1)->f1;_tmp8D4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8D1)->f2;_LL1C:
 _tmp8E3=_tmp8D4;goto _LL1A;case 12U: _LL1D: _tmp8D7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D1)->f1;_tmp8D6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8D1)->f2;_LL1E:
 _tmp8E3=_tmp8D6;goto _LL1A;case 1U: _LL1F: _tmp8D8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8D1)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp8D8);default: _LL21: _LL22:
({void*_tmp8D2=0U;({struct _dyneither_ptr _tmpDB0=({const char*_tmp8D3="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp8D3,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDB0,_tag_dyneither(_tmp8D2,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5251
 return 1;}_LL0:;}
# 5268 "tcutil.cyc"
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5270
struct _tuple17 bogus_ans=({struct _tuple17 _tmpA2D;_tmpA2D.f1=0,_tmpA2D.f2=Cyc_Absyn_heap_rgn_type;_tmpA2D;});
void*_tmp8E4=e->r;void*_tmp8E5=_tmp8E4;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_Absyn_Exp*_tmp91D;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91B;struct _dyneither_ptr*_tmp91A;int _tmp919;struct Cyc_Absyn_Exp*_tmp918;struct _dyneither_ptr*_tmp917;int _tmp916;void*_tmp915;switch(*((int*)_tmp8E5)){case 1U: _LL1: _tmp915=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8E5)->f1;_LL2: {
# 5274
void*_tmp8E6=_tmp915;struct Cyc_Absyn_Vardecl*_tmp8EE;struct Cyc_Absyn_Vardecl*_tmp8ED;struct Cyc_Absyn_Vardecl*_tmp8EC;struct Cyc_Absyn_Vardecl*_tmp8EB;switch(*((int*)_tmp8E6)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp8EB=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp8E6)->f1;_LL13: {
# 5278
void*_tmp8E7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8E8=_tmp8E7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E8)->tag == 4U){_LL1B: _LL1C:
# 5280
 return({struct _tuple17 _tmpA21;_tmpA21.f1=1,_tmpA21.f2=Cyc_Absyn_heap_rgn_type;_tmpA21;});}else{_LL1D: _LL1E:
 return({struct _tuple17 _tmpA22;_tmpA22.f1=(_tmp8EB->tq).real_const,_tmpA22.f2=Cyc_Absyn_heap_rgn_type;_tmpA22;});}_LL1A:;}case 4U: _LL14: _tmp8EC=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp8E6)->f1;_LL15: {
# 5284
void*_tmp8E9=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8EA=_tmp8E9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8EA)->tag == 4U){_LL20: _LL21:
 return({struct _tuple17 _tmpA23;_tmpA23.f1=1,_tmpA23.f2=(void*)_check_null(_tmp8EC->rgn);_tmpA23;});}else{_LL22: _LL23:
# 5287
 _tmp8EC->escapes=1;
return({struct _tuple17 _tmpA24;_tmpA24.f1=(_tmp8EC->tq).real_const,_tmpA24.f2=(void*)_check_null(_tmp8EC->rgn);_tmpA24;});}_LL1F:;}case 5U: _LL16: _tmp8ED=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp8E6)->f1;_LL17:
# 5290
 _tmp8EE=_tmp8ED;goto _LL19;default: _LL18: _tmp8EE=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp8E6)->f1;_LL19:
# 5292
 _tmp8EE->escapes=1;
return({struct _tuple17 _tmpA25;_tmpA25.f1=(_tmp8EE->tq).real_const,_tmpA25.f2=(void*)_check_null(_tmp8EE->rgn);_tmpA25;});}_LLD:;}case 21U: _LL3: _tmp918=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8E5)->f1;_tmp917=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8E5)->f2;_tmp916=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8E5)->f3;_LL4:
# 5297
 if(_tmp916)return bogus_ans;{
# 5300
void*_tmp8EF=Cyc_Tcutil_compress((void*)_check_null(_tmp918->topt));void*_tmp8F0=_tmp8EF;struct Cyc_Absyn_Aggrdecl*_tmp8FC;struct Cyc_List_List*_tmp8FB;switch(*((int*)_tmp8F0)){case 7U: _LL25: _tmp8FB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8F0)->f2;_LL26: {
# 5302
struct Cyc_Absyn_Aggrfield*_tmp8F1=Cyc_Absyn_lookup_field(_tmp8FB,_tmp917);
if(_tmp8F1 != 0  && _tmp8F1->width == 0){
struct _tuple17 _tmp8F2=Cyc_Tcutil_addressof_props(te,_tmp918);struct _tuple17 _tmp8F3=_tmp8F2;int _tmp8F5;void*_tmp8F4;_LL2C: _tmp8F5=_tmp8F3.f1;_tmp8F4=_tmp8F3.f2;_LL2D:;
return({struct _tuple17 _tmpA26;_tmpA26.f1=(_tmp8F1->tq).real_const  || _tmp8F5,_tmpA26.f2=_tmp8F4;_tmpA26;});}
# 5307
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F0)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F0)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp8FC=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F0)->f1)->f1).KnownAggr).val;_LL28: {
# 5309
struct Cyc_Absyn_Aggrfield*_tmp8F6=Cyc_Absyn_lookup_decl_field(_tmp8FC,_tmp917);
if(_tmp8F6 != 0  && _tmp8F6->width == 0){
struct _tuple17 _tmp8F7=Cyc_Tcutil_addressof_props(te,_tmp918);struct _tuple17 _tmp8F8=_tmp8F7;int _tmp8FA;void*_tmp8F9;_LL2F: _tmp8FA=_tmp8F8.f1;_tmp8F9=_tmp8F8.f2;_LL30:;
return({struct _tuple17 _tmpA27;_tmpA27.f1=(_tmp8F6->tq).real_const  || _tmp8FA,_tmpA27.f2=_tmp8F9;_tmpA27;});}
# 5314
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp91B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8E5)->f1;_tmp91A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8E5)->f2;_tmp919=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8E5)->f3;_LL6:
# 5319
 if(_tmp919)return bogus_ans;{
# 5322
void*_tmp8FD=Cyc_Tcutil_compress((void*)_check_null(_tmp91B->topt));void*_tmp8FE=_tmp8FD;void*_tmp904;void*_tmp903;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FE)->tag == 3U){_LL32: _tmp904=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FE)->f1).elt_type;_tmp903=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FE)->f1).ptr_atts).rgn;_LL33: {
# 5324
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp8FF=Cyc_Tcutil_compress(_tmp904);void*_tmp900=_tmp8FF;struct Cyc_Absyn_Aggrdecl*_tmp902;struct Cyc_List_List*_tmp901;switch(*((int*)_tmp900)){case 7U: _LL37: _tmp901=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp900)->f2;_LL38:
# 5327
({struct Cyc_Absyn_Aggrfield*_tmpDB1=Cyc_Absyn_lookup_field(_tmp901,_tmp91A);finfo=_tmpDB1;});goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp900)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp900)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp902=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp900)->f1)->f1).KnownAggr).val;_LL3A:
# 5329
({struct Cyc_Absyn_Aggrfield*_tmpDB2=Cyc_Absyn_lookup_decl_field(_tmp902,_tmp91A);finfo=_tmpDB2;});goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5332
if(finfo != 0  && finfo->width == 0)
return({struct _tuple17 _tmpA28;_tmpA28.f1=(finfo->tq).real_const,_tmpA28.f2=_tmp903;_tmpA28;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp91C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8E5)->f1;_LL8: {
# 5339
void*_tmp905=Cyc_Tcutil_compress((void*)_check_null(_tmp91C->topt));void*_tmp906=_tmp905;struct Cyc_Absyn_Tqual _tmp908;void*_tmp907;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp906)->tag == 3U){_LL3E: _tmp908=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp906)->f1).elt_tq;_tmp907=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp906)->f1).ptr_atts).rgn;_LL3F:
# 5341
 return({struct _tuple17 _tmpA29;_tmpA29.f1=_tmp908.real_const,_tmpA29.f2=_tmp907;_tmpA29;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp91E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8E5)->f1;_tmp91D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8E5)->f2;_LLA: {
# 5347
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp91E->topt));
void*_tmp909=t;struct Cyc_Absyn_Tqual _tmp912;struct Cyc_Absyn_Tqual _tmp911;void*_tmp910;struct Cyc_List_List*_tmp90F;switch(*((int*)_tmp909)){case 6U: _LL43: _tmp90F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp909)->f1;_LL44: {
# 5351
struct _tuple13 _tmp90A=Cyc_Evexp_eval_const_uint_exp(_tmp91D);struct _tuple13 _tmp90B=_tmp90A;unsigned int _tmp90E;int _tmp90D;_LL4C: _tmp90E=_tmp90B.f1;_tmp90D=_tmp90B.f2;_LL4D:;
if(!_tmp90D)
return bogus_ans;{
struct _tuple12*_tmp90C=Cyc_Absyn_lookup_tuple_field(_tmp90F,(int)_tmp90E);
if(_tmp90C != 0)
return({struct _tuple17 _tmpA2A;_tmpA2A.f1=((*_tmp90C).f1).real_const,({void*_tmpDB3=(Cyc_Tcutil_addressof_props(te,_tmp91E)).f2;_tmpA2A.f2=_tmpDB3;});_tmpA2A;});
return bogus_ans;};}case 3U: _LL45: _tmp911=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp909)->f1).elt_tq;_tmp910=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp909)->f1).ptr_atts).rgn;_LL46:
# 5359
 return({struct _tuple17 _tmpA2B;_tmpA2B.f1=_tmp911.real_const,_tmpA2B.f2=_tmp910;_tmpA2B;});case 4U: _LL47: _tmp912=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp909)->f1).tq;_LL48:
# 5365
 return({struct _tuple17 _tmpA2C;_tmpA2C.f1=_tmp912.real_const,({void*_tmpDB4=(Cyc_Tcutil_addressof_props(te,_tmp91E)).f2;_tmpA2C.f2=_tmpDB4;});_tmpA2C;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5369
({void*_tmp913=0U;({unsigned int _tmpDB6=e->loc;struct _dyneither_ptr _tmpDB5=({const char*_tmp914="unary & applied to non-lvalue";_tag_dyneither(_tmp914,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpDB6,_tmpDB5,_tag_dyneither(_tmp913,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5376
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp91F=({void*_tmpDB7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpDB7,b);});
if(_tmp91F == 0)return;{
struct Cyc_Absyn_Exp*_tmp920=_tmp91F;
struct _tuple13 _tmp921=Cyc_Evexp_eval_const_uint_exp(_tmp920);struct _tuple13 _tmp922=_tmp921;unsigned int _tmp92C;int _tmp92B;_LL1: _tmp92C=_tmp922.f1;_tmp92B=_tmp922.f2;_LL2:;
if(_tmp92B  && _tmp92C <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp925=({struct Cyc_Int_pa_PrintArg_struct _tmpA2F;_tmpA2F.tag=1U,_tmpA2F.f1=(unsigned long)((int)_tmp92C);_tmpA2F;});struct Cyc_Int_pa_PrintArg_struct _tmp926=({struct Cyc_Int_pa_PrintArg_struct _tmpA2E;_tmpA2E.tag=1U,_tmpA2E.f1=(unsigned long)((int)i);_tmpA2E;});void*_tmp923[2U];_tmp923[0]=& _tmp925,_tmp923[1]=& _tmp926;({unsigned int _tmpDB9=loc;struct _dyneither_ptr _tmpDB8=({const char*_tmp924="a dereference will be out of bounds: %d <= %d";_tag_dyneither(_tmp924,sizeof(char),46U);});Cyc_Tcutil_warn(_tmpDB9,_tmpDB8,_tag_dyneither(_tmp923,sizeof(void*),2U));});});else{
# 5385
({struct Cyc_Int_pa_PrintArg_struct _tmp929=({struct Cyc_Int_pa_PrintArg_struct _tmpA31;_tmpA31.tag=1U,_tmpA31.f1=(unsigned long)((int)_tmp92C);_tmpA31;});struct Cyc_Int_pa_PrintArg_struct _tmp92A=({struct Cyc_Int_pa_PrintArg_struct _tmpA30;_tmpA30.tag=1U,_tmpA30.f1=(unsigned long)((int)i);_tmpA30;});void*_tmp927[2U];_tmp927[0]=& _tmp929,_tmp927[1]=& _tmp92A;({unsigned int _tmpDBB=loc;struct _dyneither_ptr _tmpDBA=({const char*_tmp928="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp928,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpDBB,_tmpDBA,_tag_dyneither(_tmp927,sizeof(void*),2U));});});}}
return;};}
# 5389
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 5394
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp92D=e->r;void*_tmp92E=_tmp92D;struct Cyc_Absyn_Exp*_tmp930;struct Cyc_Absyn_Exp*_tmp92F;switch(*((int*)_tmp92E)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp92F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp92E)->f1;_LL4:
 _tmp930=_tmp92F;goto _LL6;case 13U: _LL5: _tmp930=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp92E)->f1;_LL6:
 e=_tmp930;continue;default: _LL7: _LL8:
# 5401
 return 0;}_LL0:;}}
# 5411
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp931=e->r;void*_tmp932=_tmp931;struct Cyc_List_List*_tmp951;struct Cyc_List_List*_tmp950;enum Cyc_Absyn_Primop _tmp94F;struct Cyc_List_List*_tmp94E;struct Cyc_List_List*_tmp94D;struct Cyc_List_List*_tmp94C;struct Cyc_List_List*_tmp94B;struct Cyc_Absyn_Exp*_tmp94A;struct Cyc_Absyn_Exp*_tmp949;struct Cyc_Absyn_Exp*_tmp948;struct Cyc_Absyn_Exp*_tmp947;void*_tmp946;struct Cyc_Absyn_Exp*_tmp945;void*_tmp944;struct Cyc_Absyn_Exp*_tmp943;struct Cyc_Absyn_Exp*_tmp942;struct Cyc_Absyn_Exp*_tmp941;struct Cyc_Absyn_Exp*_tmp940;struct Cyc_Absyn_Exp*_tmp93F;struct Cyc_Absyn_Exp*_tmp93E;struct Cyc_Absyn_Exp*_tmp93D;struct Cyc_Absyn_Exp*_tmp93C;struct Cyc_Absyn_Exp*_tmp93B;void*_tmp93A;switch(*((int*)_tmp932)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp93A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL10: {
# 5423
void*_tmp933=_tmp93A;struct Cyc_Absyn_Vardecl*_tmp939;struct Cyc_Absyn_Vardecl*_tmp938;switch(*((int*)_tmp933)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp938=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp933)->f1;_LL37: {
# 5426
void*_tmp934=Cyc_Tcutil_compress(_tmp938->type);void*_tmp935=_tmp934;switch(*((int*)_tmp935)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp939=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp933)->f1;_LL39:
# 5433
 if(_tmp939->sc == Cyc_Absyn_Static){
void*_tmp936=Cyc_Tcutil_compress(_tmp939->type);void*_tmp937=_tmp936;switch(*((int*)_tmp937)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 5440
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp93D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_tmp93C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp932)->f2;_tmp93B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp932)->f3;_LL12:
# 5446
 return(Cyc_Tcutil_cnst_exp(0,_tmp93D) && 
Cyc_Tcutil_cnst_exp(0,_tmp93C)) && 
Cyc_Tcutil_cnst_exp(0,_tmp93B);case 9U: _LL13: _tmp93F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_tmp93E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp932)->f2;_LL14:
# 5450
 return Cyc_Tcutil_cnst_exp(0,_tmp93F) && Cyc_Tcutil_cnst_exp(0,_tmp93E);case 41U: _LL15: _tmp940=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL16:
 _tmp941=_tmp940;goto _LL18;case 12U: _LL17: _tmp941=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL18:
 _tmp942=_tmp941;goto _LL1A;case 13U: _LL19: _tmp942=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL1A:
# 5454
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp942);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp932)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp944=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_tmp943=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp932)->f2;_LL1C:
# 5456
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp943);}else{_LL1D: _tmp946=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_tmp945=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp932)->f2;_LL1E:
# 5459
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp945);}case 15U: _LL1F: _tmp947=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL20:
# 5461
 return Cyc_Tcutil_cnst_exp(1,_tmp947);case 27U: _LL21: _tmp949=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp932)->f2;_tmp948=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp932)->f3;_LL22:
# 5463
 return Cyc_Tcutil_cnst_exp(0,_tmp949) && Cyc_Tcutil_cnst_exp(0,_tmp948);case 28U: _LL23: _tmp94A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL24:
# 5465
 return Cyc_Tcutil_cnst_exp(0,_tmp94A);case 26U: _LL25: _tmp94B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL26:
 _tmp94C=_tmp94B;goto _LL28;case 30U: _LL27: _tmp94C=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp932)->f2;_LL28:
 _tmp94D=_tmp94C;goto _LL2A;case 29U: _LL29: _tmp94D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp932)->f3;_LL2A:
# 5469
 for(0;_tmp94D != 0;_tmp94D=_tmp94D->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple18*)_tmp94D->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp94F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_tmp94E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp932)->f2;_LL2C:
# 5474
 _tmp950=_tmp94E;goto _LL2E;case 24U: _LL2D: _tmp950=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL2E:
 _tmp951=_tmp950;goto _LL30;case 31U: _LL2F: _tmp951=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp932)->f1;_LL30:
# 5477
 for(0;_tmp951 != 0;_tmp951=_tmp951->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp951->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 5485
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5489
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5491
int Cyc_Tcutil_supports_default(void*t){
void*_tmp952=Cyc_Tcutil_compress(t);void*_tmp953=_tmp952;struct Cyc_List_List*_tmp95D;struct Cyc_List_List*_tmp95C;void*_tmp95B;void*_tmp95A;void*_tmp959;void*_tmp958;struct Cyc_List_List*_tmp957;switch(*((int*)_tmp953)){case 0U: _LL1: _tmp958=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp953)->f1;_tmp957=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp953)->f2;_LL2: {
# 5494
void*_tmp954=_tmp958;union Cyc_Absyn_AggrInfo _tmp956;switch(*((int*)_tmp954)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 16U: _LL14: _LL15:
 goto _LL17;case 15U: _LL16: _LL17:
 return 1;case 20U: _LL18: _tmp956=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp954)->f1;_LL19: {
# 5501
struct Cyc_Absyn_Aggrdecl*_tmp955=Cyc_Absyn_get_known_aggrdecl(_tmp956);
if(_tmp955->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp955->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmp955->tvs,_tmp957,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp955->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp95A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp953)->f1).ptr_atts).nullable;_tmp959=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp953)->f1).ptr_atts).bounds;_LL4:
# 5508
 return Cyc_Tcutil_unify(_tmp959,Cyc_Absyn_fat_bound_type) || Cyc_Tcutil_force_type2bool(1,_tmp95A);case 4U: _LL5: _tmp95B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp953)->f1).elt_type;_LL6:
 return Cyc_Tcutil_supports_default(_tmp95B);case 6U: _LL7: _tmp95C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp953)->f1;_LL8:
# 5511
 for(0;_tmp95C != 0;_tmp95C=_tmp95C->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp95C->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp95D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp953)->f2;_LLA:
 return Cyc_Tcutil_fields_support_default(0,0,_tmp95D);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 5520
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp95E=t;struct Cyc_Absyn_Typedefdecl*_tmp963;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95E)->tag == 8U){_LL1: _tmp963=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95E)->f3;_LL2:
# 5523
 if(_tmp963 != 0){
struct Cyc_Absyn_Tqual _tmp95F=_tmp963->tq;
if(((_tmp95F.print_const  || _tmp95F.q_volatile) || _tmp95F.q_restrict) || _tmp95F.real_const)
# 5528
({struct Cyc_String_pa_PrintArg_struct _tmp962=({struct Cyc_String_pa_PrintArg_struct _tmpA32;_tmpA32.tag=0U,({struct _dyneither_ptr _tmpDBC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA32.f1=_tmpDBC;});_tmpA32;});void*_tmp960[1U];_tmp960[0]=& _tmp962;({unsigned int _tmpDBE=loc;struct _dyneither_ptr _tmpDBD=({const char*_tmp961="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp961,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpDBE,_tmpDBD,_tag_dyneither(_tmp960,sizeof(void*),1U));});});}
# 5531
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5536
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5538
struct _RegionHandle _tmp964=_new_region("rgn");struct _RegionHandle*rgn=& _tmp964;_push_region(rgn);
{struct Cyc_List_List*_tmp965=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmp966=1;_npop_handler(0U);return _tmp966;}
({void*_tmpDBF=Cyc_Tcutil_rsubstitute(rgn,_tmp965,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpDBF;});
if(!Cyc_Tcutil_supports_default(t)){int _tmp967=0;_npop_handler(0U);return _tmp967;}}{
# 5546
int _tmp968=1;_npop_handler(0U);return _tmp968;};}
# 5539
;_pop_region(rgn);}
# 5552
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp969=Cyc_Tcutil_compress(t);void*_tmp96A=_tmp969;void*_tmp96C;void*_tmp96B;switch(*((int*)_tmp96A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp96A)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp96C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96A)->f1).ptr_atts).nullable;_tmp96B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96A)->f1).ptr_atts).bounds;_LL6:
# 5560
 return !Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp96B) && Cyc_Tcutil_force_type2bool(0,_tmp96C);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 5567
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp96D=Cyc_Tcutil_compress(t);void*_tmp96E=_tmp96D;struct Cyc_List_List**_tmp974;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp96E)->tag == 5U){_LL1: _tmp974=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp96E)->f1).attributes;_LL2: {
# 5570
struct Cyc_List_List*_tmp96F=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmp974,(void*)atts->hd))
({struct Cyc_List_List*_tmpDC0=({struct Cyc_List_List*_tmp970=_cycalloc(sizeof(*_tmp970));_tmp970->hd=(void*)atts->hd,_tmp970->tl=*_tmp974;_tmp970;});*_tmp974=_tmpDC0;});}else{
# 5577
({struct Cyc_List_List*_tmpDC1=({struct Cyc_List_List*_tmp971=_cycalloc(sizeof(*_tmp971));_tmp971->hd=(void*)atts->hd,_tmp971->tl=_tmp96F;_tmp971;});_tmp96F=_tmpDC1;});}}
return _tmp96F;}}else{_LL3: _LL4:
({void*_tmp972=0U;({struct _dyneither_ptr _tmpDC2=({const char*_tmp973="transfer_fn_type_atts";_tag_dyneither(_tmp973,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDC2,_tag_dyneither(_tmp972,sizeof(void*),0U));});});}_LL0:;}
# 5584
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp975=Cyc_Tcutil_compress(t);void*_tmp976=_tmp975;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_PtrInfo*_tmp977;switch(*((int*)_tmp976)){case 3U: _LL1: _tmp977=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp976)->f1;_LL2:
# 5587
 return({void*_tmpDC3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpDC3,(_tmp977->ptr_atts).bounds);});case 4U: _LL3: _tmp978=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp976)->f1).num_elts;_LL4:
# 5589
 return _tmp978;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5596
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmp979=b;struct Cyc_Absyn_Vardecl*_tmp97D;struct Cyc_Absyn_Vardecl*_tmp97C;struct Cyc_Absyn_Vardecl*_tmp97B;struct Cyc_Absyn_Vardecl*_tmp97A;switch(*((int*)_tmp979)){case 5U: _LL1: _tmp97A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp979)->f1;_LL2:
 _tmp97B=_tmp97A;goto _LL4;case 4U: _LL3: _tmp97B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp979)->f1;_LL4:
 _tmp97C=_tmp97B;goto _LL6;case 3U: _LL5: _tmp97C=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp979)->f1;_LL6:
 _tmp97D=_tmp97C;goto _LL8;case 1U: _LL7: _tmp97D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp979)->f1;_LL8:
# 5602
 if(!_tmp97D->escapes)return _tmp97D;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5606
return 0;}
# 5610
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp97E=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpDC4=({struct Cyc_List_List*_tmp97F=_cycalloc(sizeof(*_tmp97F));_tmp97F->hd=*((void**)_check_null((void**)x->hd)),_tmp97F->tl=_tmp97E;_tmp97F;});_tmp97E=_tmpDC4;});}}
return _tmp97E;}
# 5617
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp980=Cyc_Tcutil_compress(t);void*_tmp981=_tmp980;void*_tmp98B;struct Cyc_Absyn_Tqual _tmp98A;struct Cyc_Absyn_Exp*_tmp989;void*_tmp988;unsigned int _tmp987;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp981)->tag == 4U){_LL1: _tmp98B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp981)->f1).elt_type;_tmp98A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp981)->f1).tq;_tmp989=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp981)->f1).num_elts;_tmp988=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp981)->f1).zero_term;_tmp987=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp981)->f1).zt_loc;_LL2: {
# 5620
void*b;
if(_tmp989 == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 5624
struct Cyc_Absyn_Exp*bnd=_tmp989;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmp982=0U;({struct _dyneither_ptr _tmpDC5=({const char*_tmp983="cannot convert tag without type!";_tag_dyneither(_tmp983,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDC5,_tag_dyneither(_tmp982,sizeof(void*),0U));});});{
void*_tmp984=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp985=_tmp984;void*_tmp986;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp985)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp985)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp985)->f2 != 0){_LL6: _tmp986=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp985)->f2)->hd;_LL7:
# 5630
({void*_tmpDC7=Cyc_Absyn_thin_bounds_exp(({void*_tmpDC6=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmpDC6,Cyc_Absyn_valueof_exp(_tmp986,0U),0,Cyc_Absyn_No_coercion,0U);}));b=_tmpDC7;});
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 5633
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpDC8=Cyc_Absyn_thin_bounds_exp(bnd);b=_tmpDC8;});else{
# 5636
b=Cyc_Absyn_fat_bound_type;}}_LL5:;};}else{
# 5640
({void*_tmpDC9=Cyc_Absyn_thin_bounds_exp(bnd);b=_tmpDC9;});}}
# 5642
return Cyc_Absyn_atb_type(_tmp98B,rgn,_tmp98A,b,_tmp988);}}else{_LL3: _LL4:
 return t;}_LL0:;}
# 5648
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp98C=Cyc_Tcutil_compress(t);void*_tmp98D=_tmp98C;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp99C;void*_tmp99B;void*_tmp99A;void*_tmp999;struct Cyc_List_List*_tmp998;switch(*((int*)_tmp98D)){case 0U: _LL1: _tmp999=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp98D)->f1;_tmp998=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp98D)->f2;_LL2: {
# 5651
void*_tmp98E=_tmp999;union Cyc_Absyn_AggrInfo _tmp997;switch(*((int*)_tmp98E)){case 0U: _LL1A: _LL1B:
 goto _LL1D;case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 return 1;case 15U: _LL20: _LL21:
 goto _LL23;case 4U: _LL22: _LL23:
 return 1;case 16U: _LL24: _LL25:
 return 1;case 3U: _LL26: _LL27:
 goto _LL29;case 18U: _LL28: _LL29:
 goto _LL2B;case 19U: _LL2A: _LL2B:
 goto _LL2D;case 17U: _LL2C: _LL2D:
 return 0;case 5U: _LL2E: _LL2F:
 goto _LL31;case 6U: _LL30: _LL31:
 goto _LL33;case 7U: _LL32: _LL33:
 goto _LL35;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 11U: _LL38: _LL39:
 goto _LL3B;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 13U: _LL3C: _LL3D:
 goto _LL3F;case 14U: _LL3E: _LL3F:
 goto _LL41;case 10U: _LL40: _LL41:
({struct Cyc_String_pa_PrintArg_struct _tmp991=({struct Cyc_String_pa_PrintArg_struct _tmpA33;_tmpA33.tag=0U,({struct _dyneither_ptr _tmpDCA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA33.f1=_tmpDCA;});_tmpA33;});void*_tmp98F[1U];_tmp98F[0]=& _tmp991;({struct _dyneither_ptr _tmpDCB=({const char*_tmp990="bad type `%s' in zeroable type";_tag_dyneither(_tmp990,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDCB,_tag_dyneither(_tmp98F,sizeof(void*),1U));});});default: _LL42: _tmp997=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp98E)->f1;_LL43: {
# 5673
struct Cyc_Absyn_Aggrdecl*_tmp992=Cyc_Absyn_get_known_aggrdecl(_tmp997);
if(_tmp992->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp992->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp993=_new_region("r");struct _RegionHandle*r=& _tmp993;_push_region(r);
{struct Cyc_List_List*_tmp994=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp992->tvs,_tmp998);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp992->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp994,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp995=0;_npop_handler(0U);return _tmp995;}}}{
int _tmp996=1;_npop_handler(0U);return _tmp996;};}
# 5677
;_pop_region(r);};}}_LL19:;}case 1U: _LL3: _LL4:
# 5682
 goto _LL6;case 2U: _LL5: _LL6:
 return 0;case 3U: _LL7: _tmp99A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98D)->f1).ptr_atts).nullable;_LL8:
# 5685
 return Cyc_Absyn_type2bool(1,_tmp99A);case 4U: _LL9: _tmp99B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp98D)->f1).elt_type;_LLA:
 return Cyc_Tcutil_zeroable_type(_tmp99B);case 5U: _LLB: _LLC:
 return 0;case 6U: _LLD: _tmp99C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp98D)->f1;_LLE:
# 5689
 for(0;(unsigned int)_tmp99C;_tmp99C=_tmp99C->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmp99C->hd)).f2))return 0;}
return 1;case 7U: _LLF: _tmp99D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp98D)->f2;_LL10:
# 5693
 for(0;_tmp99D != 0;_tmp99D=_tmp99D->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp99D->hd)->type))return 0;}
return 1;case 8U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;default: _LL17: _LL18:
 return 0;}_LL0:;}
