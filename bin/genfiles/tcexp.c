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
#define _check_known_subscript_notnullX(bound,index)\
   ((char *)ptr) + (elt_sz)*(index))

#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  (_cks_ptr) + _cks_elt_sz*_cks_index; })

#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#define _check_known_subscript_nullX(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  (_cks_ptr) + _cks_elt_sz*_cks_index; })

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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 325
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 509
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 907 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 925
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 928
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 930
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 932
extern void*Cyc_Absyn_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned int i);
# 935
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 939
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 958
void*Cyc_Absyn_exn_type();
# 966
extern void*Cyc_Absyn_fat_bound_type;
# 968
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 970
void*Cyc_Absyn_bounds_one();
# 977
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 980
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 983
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 988
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 998
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1002
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
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
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 46
int Cyc_Tcutil_is_arithmetic_type(void*);
# 49
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 57
int Cyc_Tcutil_is_bound_one(void*b);
# 59
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 62
int Cyc_Tcutil_is_bits_only_type(void*t);
# 69
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 71
void*Cyc_Tcutil_pointer_region(void*t);
# 74
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 77
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 82
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 84
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
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
# 115
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
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 133
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 152
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 154
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 161
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 170
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 173
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 189
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 191
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 195
void Cyc_Tcutil_explain_failure();
# 197
int Cyc_Tcutil_unify(void*,void*);
# 202
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 205
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 226
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 231
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 273 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 283
void Cyc_Tcutil_check_nonzero_bound(unsigned int,void*);
# 285
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,void*,int do_warn);
# 289
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 302
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 305
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 310
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 315
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple14{int f1;void*f2;};
# 319
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 345
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 370
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 381
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 384
int Cyc_Tcutil_zeroable_type(void*t);
# 388
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 391
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 393
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**te);struct _tuple15{unsigned int f1;int f2;};
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
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 33
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 47 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 51
({void*_tmp0=0U;({unsigned int _tmp55B=loc;struct _dyneither_ptr _tmp55A=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp55B,_tmp55A,_tag_dyneither(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 55
return*topt;}}
# 64
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 68
if(topt == 0){
# 70
({void*_tmp55C=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=26U,_tmp1->f1=des;_tmp1;});e->r=_tmp55C;});
return;}{
# 73
void*t=*topt;
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;void*_tmpE;struct Cyc_Absyn_Tqual _tmpD;union Cyc_Absyn_AggrInfo _tmpC;switch(*((int*)_tmp3)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->tag == 20U){_LL1: _tmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->f1;_LL2:
# 76
{union Cyc_Absyn_AggrInfo _tmp4=_tmpC;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp4.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp55D=({const char*_tmp6="struct type not properly set";_tag_dyneither(_tmp6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp55D,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp4.KnownAggr).val;_LLD:
({void*_tmp55E=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=29U,_tmp7->f1=_tmp8->name,_tmp7->f2=0,_tmp7->f3=des,_tmp7->f4=_tmp8;_tmp7;});e->r=_tmp55E;});}_LL9:;}
# 80
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4:
({void*_tmp55F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=26U,_tmp9->f1=des;_tmp9;});e->r=_tmp55F;});goto _LL0;case 7U: _LL5: _LL6:
({void*_tmp560=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=30U,_tmpA->f1=t,_tmpA->f2=des;_tmpA;});e->r=_tmp560;});goto _LL0;default: _LL7: _LL8:
({void*_tmp561=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp561;});goto _LL0;}_LL0:;};}
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
 if(Cyc_Tc_aggressive_warn)({void*_tmp11=0U;({unsigned int _tmp563=e->loc;struct _dyneither_ptr _tmp562=({const char*_tmp12="assignment in test";_tag_dyneither(_tmp12,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp563,_tmp562,_tag_dyneither(_tmp11,sizeof(void*),0U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 110
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp4D6;_tmp4D6.tag=0U,_tmp4D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);_tmp4D6;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp4D5;_tmp4D5.tag=0U,({
struct _dyneither_ptr _tmp564=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4D5.f1=_tmp564;});_tmp4D5;});void*_tmp13[2U];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16;({unsigned int _tmp566=e->loc;struct _dyneither_ptr _tmp565=({const char*_tmp14="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp14,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp566,_tmp565,_tag_dyneither(_tmp13,sizeof(void*),2U));});});}
# 136 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 141
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp17=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp18=_tmp17;struct _dyneither_ptr _tmp4B;struct _dyneither_ptr _tmp4A;enum Cyc_Absyn_Sign _tmp49;int _tmp48;int _tmp47;enum Cyc_Absyn_Sign _tmp46;enum Cyc_Absyn_Sign _tmp45;switch((_tmp18.Wstring_c).tag){case 2U: switch(((_tmp18.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_type;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_type;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_type;goto _LL0;}case 3U: _LL7: _LL8:
({void*_tmp567=Cyc_Absyn_wchar_type();t=_tmp567;});goto _LL0;case 4U: _LL9: _tmp45=((_tmp18.Short_c).val).f1;_LLA:
# 151
 t=_tmp45 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;goto _LL0;case 6U: _LLB: _tmp46=((_tmp18.LongLong_c).val).f1;_LLC:
# 153
 t=_tmp46 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_type: Cyc_Absyn_slonglong_type;goto _LL0;case 7U: _LLD: _tmp47=((_tmp18.Float_c).val).f2;_LLE:
# 155
 if(topt == 0)({void*_tmp568=Cyc_Absyn_gen_float_type((unsigned int)_tmp47);t=_tmp568;});else{
# 159
void*_tmp19=Cyc_Tcutil_compress(*topt);void*_tmp1A=_tmp19;int _tmp1B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL18: _tmp1B=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->f1;_LL19:
({void*_tmp569=Cyc_Absyn_gen_float_type((unsigned int)_tmp1B);t=_tmp569;});goto _LL17;}else{goto _LL1A;}}else{_LL1A: _LL1B:
# 163
({void*_tmp56A=Cyc_Absyn_gen_float_type((unsigned int)_tmp47);t=_tmp56A;});
goto _LL17;}_LL17:;}
# 167
goto _LL0;case 5U: _LLF: _tmp49=((_tmp18.Int_c).val).f1;_tmp48=((_tmp18.Int_c).val).f2;_LL10:
# 169
 if(topt == 0)
t=_tmp49 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;else{
# 176
void*_tmp1C=Cyc_Tcutil_compress(*topt);void*_tmp1D=_tmp1C;void*_tmp30;struct Cyc_Absyn_Tqual _tmp2F;void*_tmp2E;void*_tmp2D;void*_tmp2C;void*_tmp2B;void*_tmp2A;enum Cyc_Absyn_Sign _tmp29;enum Cyc_Absyn_Sign _tmp28;enum Cyc_Absyn_Sign _tmp27;enum Cyc_Absyn_Sign _tmp26;switch(*((int*)_tmp1D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1D: _tmp26=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL1E:
# 178
{enum Cyc_Absyn_Sign _tmp1E=_tmp26;switch(_tmp1E){case Cyc_Absyn_Unsigned: _LL2C: _LL2D:
 t=Cyc_Absyn_uchar_type;goto _LL2B;case Cyc_Absyn_Signed: _LL2E: _LL2F:
 t=Cyc_Absyn_schar_type;goto _LL2B;default: _LL30: _LL31:
 t=Cyc_Absyn_char_type;goto _LL2B;}_LL2B:;}
# 183
({union Cyc_Absyn_Cnst _tmp56B=Cyc_Absyn_Char_c(_tmp26,(char)_tmp48);*c=_tmp56B;});
goto _LL1C;case Cyc_Absyn_Short_sz: _LL1F: _tmp27=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL20:
# 186
 t=_tmp27 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;
({union Cyc_Absyn_Cnst _tmp56C=Cyc_Absyn_Short_c(_tmp27,(short)_tmp48);*c=_tmp56C;});
goto _LL1C;case Cyc_Absyn_Int_sz: _LL21: _tmp28=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL22:
# 190
 t=_tmp28 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 193
({union Cyc_Absyn_Cnst _tmp56D=Cyc_Absyn_Int_c(_tmp28,_tmp48);*c=_tmp56D;});
goto _LL1C;case Cyc_Absyn_Long_sz: _LL23: _tmp29=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL24:
# 196
 t=_tmp29 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 199
({union Cyc_Absyn_Cnst _tmp56E=Cyc_Absyn_Int_c(_tmp29,_tmp48);*c=_tmp56E;});
goto _LL1C;default: goto _LL29;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f2 != 0){_LL27: _tmp2A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f2)->hd;_LL28: {
# 213
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->tag=9U,({struct Cyc_Absyn_Exp*_tmp56F=Cyc_Absyn_uint_exp((unsigned int)_tmp48,0U);_tmp25->f1=_tmp56F;});_tmp25;});
# 220
({void*_tmp570=Cyc_Absyn_tag_type((void*)_tmp24);t=_tmp570;});
goto _LL1C;}}else{goto _LL29;}default: goto _LL29;}case 3U: _LL25: _tmp30=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).elt_type;_tmp2F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).elt_tq;_tmp2E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).rgn;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).nullable;_tmp2C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).bounds;_tmp2B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).zero_term;if(_tmp48 == 0){_LL26: {
# 203
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(Cyc_Tcutil_force_type2bool(1,_tmp2D))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp4D7;_tmp4D7.tag=0U,({
struct _dyneither_ptr _tmp571=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D7.f1=_tmp571;});_tmp4D7;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({unsigned int _tmp573=e->loc;struct _dyneither_ptr _tmp572=({const char*_tmp20="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp20,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp573,_tmp572,_tag_dyneither(_tmp1F,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp22=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp574=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=3U,(_tmp23->f1).elt_type=_tmp30,(_tmp23->f1).elt_tq=_tmp2F,
((_tmp23->f1).ptr_atts).rgn=_tmp2E,((_tmp23->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp23->f1).ptr_atts).bounds=_tmp2C,((_tmp23->f1).ptr_atts).zero_term=_tmp2B,((_tmp23->f1).ptr_atts).ptrloc=0;_tmp23;});
# 209
t=_tmp574;});
# 211
goto _LL1C;};}}else{goto _LL29;}default: _LL29: _LL2A:
# 223
 t=_tmp49 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
goto _LL1C;}_LL1C:;}
# 226
goto _LL0;case 8U: _LL11: _tmp4A=(_tmp18.String_c).val;_LL12:
# 228
 string_numelts=(int)_get_dyneither_size(_tmp4A,sizeof(char));
_tmp4B=_tmp4A;goto _LL14;case 9U: _LL13: _tmp4B=(_tmp18.Wstring_c).val;_LL14:
# 231
 if(string_numelts == 0){
({int _tmp575=Cyc_Tcexp_wchar_numelts(_tmp4B);string_numelts=_tmp575;});
({void*_tmp576=Cyc_Absyn_wchar_type();string_elt_typ=_tmp576;});}{
# 235
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp577=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp577,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 240
void*_tmp31=Cyc_Absyn_thin_bounds_exp(elen);
({void*_tmp57C=({void*_tmp57B=string_elt_typ;void*_tmp57A=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp579=Cyc_Absyn_const_tqual(0U);void*_tmp578=_tmp31;Cyc_Absyn_atb_type(_tmp57B,_tmp57A,_tmp579,_tmp578,Cyc_Absyn_true_type);});t=_tmp57C;});
# 243
if(topt != 0){
void*_tmp32=Cyc_Tcutil_compress(*topt);void*_tmp33=_tmp32;struct Cyc_Absyn_Tqual _tmp34;switch(*((int*)_tmp33)){case 4U: _LL33: _tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33)->f1).tq;_LL34:
# 249
 return({void*_tmp57F=string_elt_typ;struct Cyc_Absyn_Tqual _tmp57E=_tmp34;struct Cyc_Absyn_Exp*_tmp57D=elen;Cyc_Absyn_array_type(_tmp57F,_tmp57E,_tmp57D,Cyc_Tcutil_any_bool(& te),0U);});case 3U: _LL35: _LL36:
# 251
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 257
({void*_tmp584=({void*_tmp583=string_elt_typ;void*_tmp582=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp581=
Cyc_Absyn_const_tqual(0U);
# 257
void*_tmp580=_tmp31;Cyc_Absyn_atb_type(_tmp583,_tmp582,_tmp581,_tmp580,Cyc_Absyn_true_type);});t=_tmp584;});
# 259
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 265
goto _LL32;default: _LL37: _LL38:
 goto _LL32;}_LL32:;}
# 269
return t;};};default: _LL15: _LL16:
# 271
 if(topt != 0){
void*_tmp35=Cyc_Tcutil_compress(*topt);void*_tmp36=_tmp35;void*_tmp40;struct Cyc_Absyn_Tqual _tmp3F;void*_tmp3E;void*_tmp3D;void*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->tag == 3U){_LL3A: _tmp40=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).elt_type;_tmp3F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).elt_tq;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).rgn;_tmp3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).nullable;_tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).bounds;_tmp3B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).zero_term;_LL3B:
# 275
 if(Cyc_Tcutil_force_type2bool(1,_tmp3D))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp4D8;_tmp4D8.tag=0U,({
struct _dyneither_ptr _tmp585=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D8.f1=_tmp585;});_tmp4D8;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({unsigned int _tmp587=e->loc;struct _dyneither_ptr _tmp586=({const char*_tmp38="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp38,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp587,_tmp586,_tag_dyneither(_tmp37,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->tag=3U,(_tmp3A->f1).elt_type=_tmp40,(_tmp3A->f1).elt_tq=_tmp3F,((_tmp3A->f1).ptr_atts).rgn=_tmp3E,((_tmp3A->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp3A->f1).ptr_atts).bounds=_tmp3C,((_tmp3A->f1).ptr_atts).zero_term=_tmp3B,((_tmp3A->f1).ptr_atts).ptrloc=0;_tmp3A;});}else{_LL3C: _LL3D:
# 280
 goto _LL39;}_LL39:;}{
# 283
struct Cyc_List_List*_tmp41=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp58D=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=3U,({void*_tmp58C=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=_tmp41;_tmp42;}));(_tmp44->f1).elt_type=_tmp58C;}),({
struct Cyc_Absyn_Tqual _tmp58B=Cyc_Absyn_empty_tqual(0U);(_tmp44->f1).elt_tq=_tmp58B;}),
({void*_tmp58A=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->v=_tmp41;_tmp43;}));((_tmp44->f1).ptr_atts).rgn=_tmp58A;}),((_tmp44->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp589=Cyc_Tcutil_any_bounds(& te);((_tmp44->f1).ptr_atts).bounds=_tmp589;}),({
void*_tmp588=Cyc_Tcutil_any_bool(& te);((_tmp44->f1).ptr_atts).zero_term=_tmp588;}),((_tmp44->f1).ptr_atts).ptrloc=0;_tmp44;});
# 284
t=_tmp58D;});
# 290
goto _LL0;};}_LL0:;}
# 292
return t;}
# 296
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 301
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp4C=*((void**)_check_null(b));void*_tmp4D=_tmp4C;struct Cyc_Absyn_Vardecl*_tmp59;struct Cyc_Absyn_Vardecl*_tmp58;struct Cyc_Absyn_Vardecl*_tmp57;struct Cyc_Absyn_Fndecl*_tmp56;struct Cyc_Absyn_Vardecl*_tmp55;struct _tuple1*_tmp54;switch(*((int*)_tmp4D)){case 0U: _LL1: _tmp54=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL2:
# 304
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp58E=({const char*_tmp4E="unresolved binding in tcVar";_tag_dyneither(_tmp4E,sizeof(char),28U);});_tmp4F->f1=_tmp58E;});_tmp4F;}));case 1U: _LL3: _tmp55=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL4:
# 308
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp55->name,1);
return _tmp55->type;case 2U: _LL5: _tmp56=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL6:
# 315
 if(_tmp56->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp56->name,1);
return Cyc_Tcutil_fndecl2type(_tmp56);case 5U: _LL7: _tmp57=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL8:
 _tmp58=_tmp57;goto _LLA;case 4U: _LL9: _tmp58=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLA:
 _tmp59=_tmp58;goto _LLC;default: _LLB: _tmp59=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLC:
# 321
 if(te->allow_valueof){
void*_tmp50=Cyc_Tcutil_compress(_tmp59->type);void*_tmp51=_tmp50;void*_tmp53;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f2 != 0){_LLE: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f2)->hd;_LLF:
# 324
({void*_tmp58F=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=39U,_tmp52->f1=_tmp53;_tmp52;});e->r=_tmp58F;});
goto _LLD;}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 329
return _tmp59->type;}_LL0:;}
# 333
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 340
struct Cyc_List_List*desc_types;
{void*_tmp5A=fmt->r;void*_tmp5B=_tmp5A;struct _dyneither_ptr _tmp60;struct _dyneither_ptr _tmp5F;switch(*((int*)_tmp5B)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).tag == 8){_LL1: _tmp5F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).val;_LL2:
 _tmp60=_tmp5F;goto _LL4;}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp60=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).val;_LL4:
# 344
({struct Cyc_List_List*_tmp590=type_getter(te,(struct _dyneither_ptr)_tmp60,fmt->loc);desc_types=_tmp590;});goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 348
 if(opt_args != 0){
struct Cyc_List_List*_tmp5C=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp5C != 0;_tmp5C=_tmp5C->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp5C->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp5C->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp5C->hd))
({void*_tmp5D=0U;({unsigned int _tmp592=((struct Cyc_Absyn_Exp*)_tmp5C->hd)->loc;struct _dyneither_ptr _tmp591=({const char*_tmp5E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp5E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp592,_tmp591,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});}}
# 357
return;}_LL0:;}
# 359
if(opt_args != 0){
struct Cyc_List_List*_tmp61=(struct Cyc_List_List*)opt_args->v;
# 362
for(0;desc_types != 0  && _tmp61 != 0;(
desc_types=desc_types->tl,_tmp61=_tmp61->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp61->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0U,({
struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4DA.f1=_tmp593;});_tmp4DA;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0U,({struct _dyneither_ptr _tmp594=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4D9.f1=_tmp594;});_tmp4D9;});void*_tmp62[2U];_tmp62[0]=& _tmp64,_tmp62[1]=& _tmp65;({unsigned int _tmp596=e->loc;struct _dyneither_ptr _tmp595=({const char*_tmp63="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp63,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp596,_tmp595,_tag_dyneither(_tmp62,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 373
if(alias_coercion)
({struct Cyc_List_List*_tmp597=({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->hd=(void*)arg_cnt,_tmp66->tl=*alias_arg_exps;_tmp66;});*alias_arg_exps=_tmp597;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp67=0U;({unsigned int _tmp599=((struct Cyc_Absyn_Exp*)_tmp61->hd)->loc;struct _dyneither_ptr _tmp598=({const char*_tmp68="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp68,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp599,_tmp598,_tag_dyneither(_tmp67,sizeof(void*),0U));});});}
# 380
if(desc_types != 0)
({void*_tmp69=0U;({unsigned int _tmp59B=fmt->loc;struct _dyneither_ptr _tmp59A=({const char*_tmp6A="too few arguments";_tag_dyneither(_tmp6A,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp59B,_tmp59A,_tag_dyneither(_tmp69,sizeof(void*),0U));});});
if(_tmp61 != 0){
({void*_tmp6B=0U;({unsigned int _tmp59D=((struct Cyc_Absyn_Exp*)_tmp61->hd)->loc;struct _dyneither_ptr _tmp59C=({const char*_tmp6C="too many arguments";_tag_dyneither(_tmp6C,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp59D,_tmp59C,_tag_dyneither(_tmp6B,sizeof(void*),0U));});});
# 385
for(0;_tmp61 != 0;_tmp61=_tmp61->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp61->hd);}}}}
# 390
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 392
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6D=p;switch(_tmp6D){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 396
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp4DB;_tmp4DB.tag=0U,({struct _dyneither_ptr _tmp59E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4DB.f1=_tmp59E;});_tmp4DB;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({unsigned int _tmp5A0=loc;struct _dyneither_ptr _tmp59F=({const char*_tmp6F="expecting numeric type but found %s";_tag_dyneither(_tmp6F,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5A0,_tmp59F,_tag_dyneither(_tmp6E,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 400
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp4DC;_tmp4DC.tag=0U,({struct _dyneither_ptr _tmp5A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4DC.f1=_tmp5A1;});_tmp4DC;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({unsigned int _tmp5A3=loc;struct _dyneither_ptr _tmp5A2=({const char*_tmp72="expecting integral or * type but found %s";_tag_dyneither(_tmp72,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp5A3,_tmp5A2,_tag_dyneither(_tmp71,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 405
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp4DD;_tmp4DD.tag=0U,({struct _dyneither_ptr _tmp5A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4DD.f1=_tmp5A4;});_tmp4DD;});void*_tmp74[1U];_tmp74[0]=& _tmp76;({unsigned int _tmp5A6=loc;struct _dyneither_ptr _tmp5A5=({const char*_tmp75="expecting integral type but found %s";_tag_dyneither(_tmp75,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp5A6,_tmp5A5,_tag_dyneither(_tmp74,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 409
{void*_tmp77=t;void*_tmp82;struct Cyc_Absyn_Exp*_tmp81;switch(*((int*)_tmp77)){case 4U: _LLE: _tmp81=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp77)->f1).num_elts;_LLF:
# 411
 if(_tmp81 == 0  || !Cyc_Evexp_c_can_eval(_tmp81))
({void*_tmp78=0U;({unsigned int _tmp5A8=loc;struct _dyneither_ptr _tmp5A7=({const char*_tmp79="can't calculate numelts";_tag_dyneither(_tmp79,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp5A8,_tmp5A7,_tag_dyneither(_tmp78,sizeof(void*),0U));});});
goto _LLD;case 3U: _LL10: _tmp82=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LL11: {
# 415
struct Cyc_Absyn_Exp*_tmp7A=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp82);
if(_tmp7A != 0){
struct Cyc_Absyn_Exp*_tmp7B=_tmp7A;
if(!Cyc_Evexp_c_can_eval(_tmp7B) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp7C=0U;({unsigned int _tmp5AA=loc;struct _dyneither_ptr _tmp5A9=({const char*_tmp7D="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp7D,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp5AA,_tmp5A9,_tag_dyneither(_tmp7C,sizeof(void*),0U));});});}
# 421
goto _LLD;}default: _LL12: _LL13:
# 423
({struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmp4DE;_tmp4DE.tag=0U,({struct _dyneither_ptr _tmp5AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4DE.f1=_tmp5AB;});_tmp4DE;});void*_tmp7E[1U];_tmp7E[0]=& _tmp80;({unsigned int _tmp5AD=loc;struct _dyneither_ptr _tmp5AC=({const char*_tmp7F="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp7F,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp5AD,_tmp5AC,_tag_dyneither(_tmp7E,sizeof(void*),1U));});});}_LLD:;}
# 425
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({void*_tmp83=0U;({struct _dyneither_ptr _tmp5AE=({const char*_tmp84="Non-unary primop";_tag_dyneither(_tmp84,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5AE,_tag_dyneither(_tmp83,sizeof(void*),0U));});});}_LL0:;}
# 431
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 434
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=0U,({struct _dyneither_ptr _tmp5AF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4DF.f1=_tmp5AF;});_tmp4DF;});void*_tmp85[1U];_tmp85[0]=& _tmp87;({unsigned int _tmp5B1=e1->loc;struct _dyneither_ptr _tmp5B0=({const char*_tmp86="type %s cannot be used here";_tag_dyneither(_tmp86,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5B1,_tmp5B0,_tag_dyneither(_tmp85,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 438
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=0U,({struct _dyneither_ptr _tmp5B2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4E0.f1=_tmp5B2;});_tmp4E0;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({unsigned int _tmp5B4=e2->loc;struct _dyneither_ptr _tmp5B3=({const char*_tmp89="type %s cannot be used here";_tag_dyneither(_tmp89,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5B4,_tmp5B3,_tag_dyneither(_tmp88,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 442
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 447
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp8B=t1;void*_tmpA2;struct Cyc_Absyn_Tqual _tmpA1;void*_tmpA0;void*_tmp9F;void*_tmp9E;void*_tmp9D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->tag == 3U){_LL1: _tmpA2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_type;_tmpA1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).elt_tq;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).rgn;_tmp9F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).nullable;_tmp9E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).bounds;_tmp9D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B)->f1).ptr_atts).zero_term;_LL2:
# 452
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmpA2),& Cyc_Tcutil_tmk))
({void*_tmp8C=0U;({unsigned int _tmp5B6=e1->loc;struct _dyneither_ptr _tmp5B5=({const char*_tmp8D="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp8D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5B6,_tmp5B5,_tag_dyneither(_tmp8C,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp8E=0U;({unsigned int _tmp5B8=e1->loc;struct _dyneither_ptr _tmp5B7=({const char*_tmp8F="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp8F,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5B8,_tmp5B7,_tag_dyneither(_tmp8E,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmp92=({struct Cyc_String_pa_PrintArg_struct _tmp4E1;_tmp4E1.tag=0U,({struct _dyneither_ptr _tmp5B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E1.f1=_tmp5B9;});_tmp4E1;});void*_tmp90[1U];_tmp90[0]=& _tmp92;({unsigned int _tmp5BB=e2->loc;struct _dyneither_ptr _tmp5BA=({const char*_tmp91="expecting int but found %s";_tag_dyneither(_tmp91,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5BB,_tmp5BA,_tag_dyneither(_tmp90,sizeof(void*),1U));});});{
struct Cyc_Absyn_Exp*_tmp93=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp9E);
if(_tmp93 == 0)return t1;{
struct Cyc_Absyn_Exp*_tmp94=_tmp93;
# 463
if(Cyc_Tcutil_force_type2bool(0,_tmp9D)){
struct _tuple15 _tmp95=Cyc_Evexp_eval_const_uint_exp(_tmp94);struct _tuple15 _tmp96=_tmp95;unsigned int _tmp9A;int _tmp99;_LL6: _tmp9A=_tmp96.f1;_tmp99=_tmp96.f2;_LL7:;
if(_tmp99  && _tmp9A == 1)
({void*_tmp97=0U;({unsigned int _tmp5BD=e1->loc;struct _dyneither_ptr _tmp5BC=({const char*_tmp98="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmp98,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5BD,_tmp5BC,_tag_dyneither(_tmp97,sizeof(void*),0U));});});}{
# 474
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9B=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=3U,(_tmp9C->f1).elt_type=_tmpA2,(_tmp9C->f1).elt_tq=_tmpA1,
((_tmp9C->f1).ptr_atts).rgn=_tmpA0,((_tmp9C->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp9C->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmp9C->f1).ptr_atts).zero_term=_tmp9D,((_tmp9C->f1).ptr_atts).ptrloc=0;_tmp9C;});
# 478
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp9B,Cyc_Absyn_Other_coercion);
return(void*)_tmp9B;};};};}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 485
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp4E3;_tmp4E3.tag=0U,({
# 495
struct _dyneither_ptr _tmp5BE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E3.f1=_tmp5BE;});_tmp4E3;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp4E2;_tmp4E2.tag=0U,({struct _dyneither_ptr _tmp5BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E2.f1=_tmp5BF;});_tmp4E2;});void*_tmpA3[2U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6;({unsigned int _tmp5C1=e1->loc;struct _dyneither_ptr _tmp5C0=({const char*_tmpA4="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpA4,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5C1,_tmp5C0,_tag_dyneither(_tmpA3,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 498
return Cyc_Absyn_sint_type;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp5C2=t1_elt;Cyc_Tcutil_unify(_tmp5C2,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp4E5;_tmp4E5.tag=0U,({
# 503
struct _dyneither_ptr _tmp5C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E5.f1=_tmp5C3;});_tmp4E5;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp4E4;_tmp4E4.tag=0U,({struct _dyneither_ptr _tmp5C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E4.f1=_tmp5C4;});_tmp4E4;});void*_tmpA7[2U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({unsigned int _tmp5C6=e1->loc;struct _dyneither_ptr _tmp5C5=({const char*_tmpA8="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpA8,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5C6,_tmp5C5,_tag_dyneither(_tmpA7,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 507
({void*_tmpAB=0U;({unsigned int _tmp5C8=e1->loc;struct _dyneither_ptr _tmp5C7=({const char*_tmpAC="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpAC,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5C8,_tmp5C7,_tag_dyneither(_tmpAB,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5CD=te;struct Cyc_Absyn_Exp*_tmp5CC=e1;Cyc_Tcutil_unchecked_cast(_tmp5CD,_tmp5CC,({void*_tmp5CB=t1_elt;void*_tmp5CA=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5C9=
Cyc_Absyn_empty_tqual(0U);
# 508
Cyc_Absyn_star_type(_tmp5CB,_tmp5CA,_tmp5C9,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 511
return Cyc_Absyn_sint_type;}else{
# 513
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpAD=0U;({unsigned int _tmp5CF=e1->loc;struct _dyneither_ptr _tmp5CE=({const char*_tmpAE="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpAE,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5CF,_tmp5CE,_tag_dyneither(_tmpAD,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpAF=0U;({unsigned int _tmp5D1=e1->loc;struct _dyneither_ptr _tmp5D0=({const char*_tmpB0="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpB0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5D1,_tmp5D0,_tag_dyneither(_tmpAF,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpB3=({struct Cyc_String_pa_PrintArg_struct _tmp4E7;_tmp4E7.tag=0U,({
struct _dyneither_ptr _tmp5D2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E7.f1=_tmp5D2;});_tmp4E7;});struct Cyc_String_pa_PrintArg_struct _tmpB4=({struct Cyc_String_pa_PrintArg_struct _tmp4E6;_tmp4E6.tag=0U,({struct _dyneither_ptr _tmp5D3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E6.f1=_tmp5D3;});_tmp4E6;});void*_tmpB1[2U];_tmpB1[0]=& _tmpB3,_tmpB1[1]=& _tmpB4;({unsigned int _tmp5D5=e2->loc;struct _dyneither_ptr _tmp5D4=({const char*_tmpB2="expecting either %s or int but found %s";_tag_dyneither(_tmpB2,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5D5,_tmp5D4,_tag_dyneither(_tmpB1,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 522
return t1;}}}
# 526
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5D6=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5D6,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
({void*_tmpB5=0U;({unsigned int _tmp5D8=e1->loc;struct _dyneither_ptr _tmp5D7=({const char*_tmpB6="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpB6,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5D8,_tmp5D7,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5DD=te;struct Cyc_Absyn_Exp*_tmp5DC=e2;Cyc_Tcutil_unchecked_cast(_tmp5DD,_tmp5DC,({void*_tmp5DB=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5DA=
Cyc_Tcutil_pointer_region(t2);
# 531
struct Cyc_Absyn_Tqual _tmp5D9=
# 533
Cyc_Absyn_empty_tqual(0U);
# 531
Cyc_Absyn_star_type(_tmp5DB,_tmp5DA,_tmp5D9,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 536
({void*_tmpB7=0U;({unsigned int _tmp5DF=e1->loc;struct _dyneither_ptr _tmp5DE=({const char*_tmpB8="thin pointer subtraction!";_tag_dyneither(_tmpB8,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5DF,_tmp5DE,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 539
({void*_tmpB9=0U;({unsigned int _tmp5E1=e1->loc;struct _dyneither_ptr _tmp5E0=({const char*_tmpBA="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpBA,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5E1,_tmp5E0,_tag_dyneither(_tmpB9,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 542
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpBB=0U;({unsigned int _tmp5E3=e1->loc;struct _dyneither_ptr _tmp5E2=({const char*_tmpBC="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpBC,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp5E3,_tmp5E2,_tag_dyneither(_tmpBB,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 547
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 550
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_type;else{
# 558
if((Cyc_Tcutil_type_kind(t1))->kind == Cyc_Absyn_BoxKind  || ({
void*_tmp5E4=t1;Cyc_Tcutil_unify(_tmp5E4,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_type;else{
# 563
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_type;else{
goto pointer_cmp;}}}}}else{
# 576
pointer_cmp: {
struct _tuple0 _tmpBD=({struct _tuple0 _tmp4E8;({void*_tmp5E6=Cyc_Tcutil_compress(t1);_tmp4E8.f1=_tmp5E6;}),({void*_tmp5E5=Cyc_Tcutil_compress(t2);_tmp4E8.f2=_tmp5E5;});_tmp4E8;});struct _tuple0 _tmpBE=_tmpBD;void*_tmpC0;void*_tmpBF;switch(*((int*)_tmpBE.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f2)->tag == 3U){_LL1: _tmpC0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f1)->f1).elt_type;_tmpBF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBE.f2)->f1).elt_type;_LL2:
# 580
 if(Cyc_Tcutil_unify(_tmpC0,_tmpBF))return Cyc_Absyn_sint_type;goto _LL0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBE.f2)->f1)->tag == 3U){_LL3: _LL4:
# 582
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 585
({struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=0U,({
struct _dyneither_ptr _tmp5E7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4EA.f1=_tmp5E7;});_tmp4EA;});struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp4E9;_tmp4E9.tag=0U,({struct _dyneither_ptr _tmp5E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E9.f1=_tmp5E8;});_tmp4E9;});void*_tmpC1[2U];_tmpC1[0]=& _tmpC3,_tmpC1[1]=& _tmpC4;({unsigned int _tmp5EA=loc;struct _dyneither_ptr _tmp5E9=({const char*_tmpC2="comparison not allowed between %s and %s";_tag_dyneither(_tmpC2,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5EA,_tmp5E9,_tag_dyneither(_tmpC1,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 592
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 594
if(({void*_tmp5EB=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5EB == Cyc_Absyn_sint_type;}))
return Cyc_Absyn_sint_type;
({struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp4EC;_tmp4EC.tag=0U,({
struct _dyneither_ptr _tmp5EC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4EC.f1=_tmp5EC;});_tmp4EC;});struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp4EB;_tmp4EB.tag=0U,({struct _dyneither_ptr _tmp5ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4EB.f1=_tmp5ED;});_tmp4EB;});void*_tmpC5[2U];_tmpC5[0]=& _tmpC7,_tmpC5[1]=& _tmpC8;({unsigned int _tmp5EF=loc;struct _dyneither_ptr _tmp5EE=({const char*_tmpC6="comparison not allowed between %s and %s";_tag_dyneither(_tmpC6,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5EF,_tmp5EE,_tag_dyneither(_tmpC5,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 604
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 606
enum Cyc_Absyn_Primop _tmpC9=p;switch(_tmpC9){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 610
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 613
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Bitarshift: _LL15: _LL16:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL17: _LL18:
# 623
 goto _LL1A;case Cyc_Absyn_Neq: _LL19: _LL1A:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LL1B: _LL1C:
# 626
 goto _LL1E;case Cyc_Absyn_Lt: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Gte: _LL1F: _LL20:
 goto _LL22;case Cyc_Absyn_Lte: _LL21: _LL22:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LL23: _LL24:
# 631
({void*_tmpCA=0U;({struct _dyneither_ptr _tmp5F0=({const char*_tmpCB="bad binary primop";_tag_dyneither(_tmpCB,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5F0,_tag_dyneither(_tmpCA,sizeof(void*),0U));});});}_LL0:;}
# 635
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 643
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpCC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpCD=Cyc_Tcexp_tcExp(te,topt,_tmpCC);
if(!Cyc_Tcutil_is_numeric(_tmpCC))
({struct Cyc_String_pa_PrintArg_struct _tmpD0=({struct Cyc_String_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=0U,({struct _dyneither_ptr _tmp5F1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpCD));_tmp4ED.f1=_tmp5F1;});_tmp4ED;});void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({unsigned int _tmp5F3=_tmpCC->loc;struct _dyneither_ptr _tmp5F2=({const char*_tmpCF="expecting numeric type but found %s";_tag_dyneither(_tmpCF,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5F3,_tmp5F2,_tag_dyneither(_tmpCE,sizeof(void*),1U));});});
return _tmpCD;}
# 650
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpD1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpD2=_tmpD1;switch(_tmpD2){case 0U: _LL1: _LL2:
 return({void*_tmpD3=0U;({struct Cyc_Tcenv_Tenv*_tmp5F7=te;unsigned int _tmp5F6=loc;void**_tmp5F5=topt;struct _dyneither_ptr _tmp5F4=({const char*_tmpD4="primitive operator has 0 arguments";_tag_dyneither(_tmpD4,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp5F7,_tmp5F6,_tmp5F5,_tmp5F4,_tag_dyneither(_tmpD3,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
({void*_tmp5F8=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp5F8;});goto _LL0;case 2U: _LL5: _LL6:
({void*_tmp5FE=({struct Cyc_Tcenv_Tenv*_tmp5FD=te;unsigned int _tmp5FC=loc;void**_tmp5FB=topt;enum Cyc_Absyn_Primop _tmp5FA=p;struct Cyc_Absyn_Exp*_tmp5F9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5FD,_tmp5FC,_tmp5FB,_tmp5FA,_tmp5F9,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp5FE;});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpD5=0U;({struct Cyc_Tcenv_Tenv*_tmp602=te;unsigned int _tmp601=loc;void**_tmp600=topt;struct _dyneither_ptr _tmp5FF=({const char*_tmpD6="primitive operator has > 2 arguments";_tag_dyneither(_tmpD6,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp602,_tmp601,_tmp600,_tmp5FF,_tag_dyneither(_tmpD5,sizeof(void*),0U));});});}_LL0:;}
# 658
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 661
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpD7=Cyc_Tcutil_compress(t);
void*_tmpD8=_tmpD7;struct Cyc_List_List*_tmpF4;void*_tmpF3;struct Cyc_Absyn_Tqual _tmpF2;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_Datatypefield*_tmpF0;struct Cyc_Absyn_Aggrdecl*_tmpEF;switch(*((int*)_tmpD8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpEF=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownAggr).val;_LL2:
# 665
 if(_tmpEF->impl == 0){
({void*_tmpD9=0U;({unsigned int _tmp604=loc;struct _dyneither_ptr _tmp603=({const char*_tmpDA="attempt to write an abstract aggregate";_tag_dyneither(_tmpDA,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp604,_tmp603,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});
return 0;}else{
# 669
_tmpF1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEF->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpF0=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD8)->f1)->f1).KnownDatatypefield).val).f2;_LL6:
# 681
{struct Cyc_List_List*_tmpDF=_tmpF0->typs;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){
struct _tuple17*_tmpE0=(struct _tuple17*)_tmpDF->hd;struct _tuple17*_tmpE1=_tmpE0;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE5;_LLE: _tmpE6=_tmpE1->f1;_tmpE5=_tmpE1->f2;_LLF:;
if(_tmpE6.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpE4=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,({struct _dyneither_ptr _tmp605=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF0->name));_tmp4EE.f1=_tmp605;});_tmp4EE;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({unsigned int _tmp607=loc;struct _dyneither_ptr _tmp606=({const char*_tmpE3="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpE3,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp607,_tmp606,_tag_dyneither(_tmpE2,sizeof(void*),1U));});});
return 0;}
# 687
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpE5))return 0;}}
# 689
return 1;}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpF1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD8)->f2;_LL4:
# 671
 for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
struct Cyc_Absyn_Aggrfield*_tmpDB=(struct Cyc_Absyn_Aggrfield*)_tmpF1->hd;
if((_tmpDB->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U,_tmp4EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpDB->name);_tmp4EF;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({unsigned int _tmp609=loc;struct _dyneither_ptr _tmp608=({const char*_tmpDD="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpDD,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp609,_tmp608,_tag_dyneither(_tmpDC,sizeof(void*),1U));});});
return 0;}
# 677
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpDB->type))return 0;}
# 679
return 1;case 4U: _LL7: _tmpF3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).elt_type;_tmpF2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD8)->f1).tq;_LL8:
# 691
 if(_tmpF2.real_const){
({void*_tmpE7=0U;({unsigned int _tmp60B=loc;struct _dyneither_ptr _tmp60A=({const char*_tmpE8="attempt to over-write a const array";_tag_dyneither(_tmpE8,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp60B,_tmp60A,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});
return 0;}
# 695
return Cyc_Tcexp_check_writable_aggr(loc,_tmpF3);case 6U: _LL9: _tmpF4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD8)->f1;_LLA:
# 697
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
struct _tuple17*_tmpE9=(struct _tuple17*)_tmpF4->hd;struct _tuple17*_tmpEA=_tmpE9;struct Cyc_Absyn_Tqual _tmpEE;void*_tmpED;_LL11: _tmpEE=_tmpEA->f1;_tmpED=_tmpEA->f2;_LL12:;
if(_tmpEE.real_const){
({void*_tmpEB=0U;({unsigned int _tmp60D=loc;struct _dyneither_ptr _tmp60C=({const char*_tmpEC="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmpEC,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp60D,_tmp60C,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});
return 0;}
# 703
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpED))return 0;}
# 705
return 1;default: _LLB: _LLC:
 return 1;}_LL0:;}
# 713
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 716
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmpF5=e->r;void*_tmpF6=_tmpF5;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct _dyneither_ptr*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct _dyneither_ptr*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Vardecl*_tmp11D;struct Cyc_Absyn_Vardecl*_tmp11C;struct Cyc_Absyn_Vardecl*_tmp11B;struct Cyc_Absyn_Vardecl*_tmp11A;switch(*((int*)_tmpF6)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)){case 3U: _LL1: _tmp11A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL2:
 _tmp11B=_tmp11A;goto _LL4;case 4U: _LL3: _tmp11B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL4:
 _tmp11C=_tmp11B;goto _LL6;case 5U: _LL5: _tmp11C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL6:
 _tmp11D=_tmp11C;goto _LL8;case 1U: _LL7: _tmp11D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF6)->f1)->f1;_LL8:
 if(!(_tmp11D->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp11F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmp11E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLA:
# 723
{void*_tmpF7=Cyc_Tcutil_compress((void*)_check_null(_tmp11F->topt));void*_tmpF8=_tmpF7;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Tqual _tmp108;struct Cyc_Absyn_Tqual _tmp107;switch(*((int*)_tmpF8)){case 3U: _LL18: _tmp107=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF8)->f1).elt_tq;_LL19:
 _tmp108=_tmp107;goto _LL1B;case 4U: _LL1A: _tmp108=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF8)->f1).tq;_LL1B:
 if(!_tmp108.real_const)return;goto _LL17;case 6U: _LL1C: _tmp109=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF8)->f1;_LL1D: {
# 727
struct _tuple15 _tmpF9=Cyc_Evexp_eval_const_uint_exp(_tmp11E);struct _tuple15 _tmpFA=_tmpF9;unsigned int _tmp106;int _tmp105;_LL21: _tmp106=_tmpFA.f1;_tmp105=_tmpFA.f2;_LL22:;
if(!_tmp105){
({void*_tmpFB=0U;({unsigned int _tmp60F=e->loc;struct _dyneither_ptr _tmp60E=({const char*_tmpFC="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmpFC,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp60F,_tmp60E,_tag_dyneither(_tmpFB,sizeof(void*),0U));});});
return;}
# 732
{struct _handler_cons _tmpFD;_push_handler(& _tmpFD);{int _tmpFF=0;if(setjmp(_tmpFD.handler))_tmpFF=1;if(!_tmpFF){
{struct _tuple17*_tmp100=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp109,(int)_tmp106);struct _tuple17*_tmp101=_tmp100;struct Cyc_Absyn_Tqual _tmp102;_LL24: _tmp102=_tmp101->f1;_LL25:;
if(!_tmp102.real_const){_npop_handler(0U);return;}}
# 733
;_pop_handler();}else{void*_tmpFE=(void*)_exn_thrown;void*_tmp103=_tmpFE;void*_tmp104;if(((struct Cyc_List_Nth_exn_struct*)_tmp103)->tag == Cyc_List_Nth){_LL27: _LL28:
# 735
 return;}else{_LL29: _tmp104=_tmp103;_LL2A:(int)_rethrow(_tmp104);}_LL26:;}};}
goto _LL17;}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 739
goto _LL0;case 21U: _LLB: _tmp121=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmp120=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLC:
# 741
{void*_tmp10A=Cyc_Tcutil_compress((void*)_check_null(_tmp121->topt));void*_tmp10B=_tmp10A;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_Aggrdecl**_tmp10C;switch(*((int*)_tmp10B)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10B)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp10C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10B)->f1)->f1).KnownAggr).val;_LL2D: {
# 743
struct Cyc_Absyn_Aggrfield*sf=
_tmp10C == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp10C,_tmp120);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp10D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10B)->f2;_LL2F: {
# 748
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp10D,_tmp120);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 753
goto _LL0;case 22U: _LLD: _tmp123=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_tmp122=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF6)->f2;_LLE:
# 755
{void*_tmp10E=Cyc_Tcutil_compress((void*)_check_null(_tmp123->topt));void*_tmp10F=_tmp10E;void*_tmp115;struct Cyc_Absyn_Tqual _tmp114;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10F)->tag == 3U){_LL33: _tmp115=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10F)->f1).elt_type;_tmp114=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10F)->f1).elt_tq;_LL34:
# 757
 if(!_tmp114.real_const){
void*_tmp110=Cyc_Tcutil_compress(_tmp115);void*_tmp111=_tmp110;struct Cyc_List_List*_tmp113;struct Cyc_Absyn_Aggrdecl**_tmp112;switch(*((int*)_tmp111)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp111)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp111)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp112=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp111)->f1)->f1).KnownAggr).val;_LL39: {
# 760
struct Cyc_Absyn_Aggrfield*sf=
_tmp112 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp112,_tmp122);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp113=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp111)->f2;_LL3B: {
# 765
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp113,_tmp122);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 771
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 774
goto _LL0;case 20U: _LLF: _tmp124=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL10:
# 776
{void*_tmp116=Cyc_Tcutil_compress((void*)_check_null(_tmp124->topt));void*_tmp117=_tmp116;struct Cyc_Absyn_Tqual _tmp119;struct Cyc_Absyn_Tqual _tmp118;switch(*((int*)_tmp117)){case 3U: _LL3F: _tmp118=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp117)->f1).elt_tq;_LL40:
 _tmp119=_tmp118;goto _LL42;case 4U: _LL41: _tmp119=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp117)->f1).tq;_LL42:
 if(!_tmp119.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 781
goto _LL0;case 12U: _LL11: _tmp125=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL12:
 _tmp126=_tmp125;goto _LL14;case 13U: _LL13: _tmp126=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF6)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp126);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 786
({struct Cyc_String_pa_PrintArg_struct _tmp129=({struct Cyc_String_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=0U,({struct _dyneither_ptr _tmp610=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4F0.f1=_tmp610;});_tmp4F0;});void*_tmp127[1U];_tmp127[0]=& _tmp129;({unsigned int _tmp612=e->loc;struct _dyneither_ptr _tmp611=({const char*_tmp128="attempt to write a const location: %s";_tag_dyneither(_tmp128,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp612,_tmp611,_tag_dyneither(_tmp127,sizeof(void*),1U));});});}
# 789
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 792
({struct Cyc_Tcenv_Tenv*_tmp613=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp613,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp12A=0U;({struct Cyc_Tcenv_Tenv*_tmp617=te;unsigned int _tmp616=loc;void**_tmp615=topt;struct _dyneither_ptr _tmp614=({const char*_tmp12B="increment/decrement of non-lvalue";_tag_dyneither(_tmp12B,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp617,_tmp616,_tmp615,_tmp614,_tag_dyneither(_tmp12A,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 798
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp12C=0U;({unsigned int _tmp619=e->loc;struct _dyneither_ptr _tmp618=({const char*_tmp12D="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp12D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp619,_tmp618,_tag_dyneither(_tmp12C,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp12E=0U;({unsigned int _tmp61B=e->loc;struct _dyneither_ptr _tmp61A=({const char*_tmp12F="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp12F,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp61B,_tmp61A,_tag_dyneither(_tmp12E,sizeof(void*),0U));});});}else{
# 807
({struct Cyc_String_pa_PrintArg_struct _tmp132=({struct Cyc_String_pa_PrintArg_struct _tmp4F1;_tmp4F1.tag=0U,({struct _dyneither_ptr _tmp61C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F1.f1=_tmp61C;});_tmp4F1;});void*_tmp130[1U];_tmp130[0]=& _tmp132;({unsigned int _tmp61E=e->loc;struct _dyneither_ptr _tmp61D=({const char*_tmp131="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp131,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp61E,_tmp61D,_tag_dyneither(_tmp130,sizeof(void*),1U));});});}}
# 809
return t;};}
# 813
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 815
({struct Cyc_Tcenv_Tenv*_tmp620=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp61F=e1;Cyc_Tcexp_tcTest(_tmp620,_tmp61F,({const char*_tmp133="conditional expression";_tag_dyneither(_tmp133,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp621=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp621;});else{
# 822
({void*_tmp622=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp622;});}{
struct Cyc_List_List _tmp134=({struct Cyc_List_List _tmp4F5;_tmp4F5.hd=e3,_tmp4F5.tl=0;_tmp4F5;});
struct Cyc_List_List _tmp135=({struct Cyc_List_List _tmp4F4;_tmp4F4.hd=e2,_tmp4F4.tl=& _tmp134;_tmp4F4;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp135)){
({struct Cyc_String_pa_PrintArg_struct _tmp138=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({
struct _dyneither_ptr _tmp623=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4F3.f1=_tmp623;});_tmp4F3;});struct Cyc_String_pa_PrintArg_struct _tmp139=({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0U,({struct _dyneither_ptr _tmp624=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp4F2.f1=_tmp624;});_tmp4F2;});void*_tmp136[2U];_tmp136[0]=& _tmp138,_tmp136[1]=& _tmp139;({unsigned int _tmp626=loc;struct _dyneither_ptr _tmp625=({const char*_tmp137="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp137,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp626,_tmp625,_tag_dyneither(_tmp136,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 830
return t;};};}
# 834
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 836
({struct Cyc_Tcenv_Tenv*_tmp628=te;struct Cyc_Absyn_Exp*_tmp627=e1;Cyc_Tcexp_tcTest(_tmp628,_tmp627,({const char*_tmp13A="logical-and expression";_tag_dyneither(_tmp13A,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp62A=te;struct Cyc_Absyn_Exp*_tmp629=e2;Cyc_Tcexp_tcTest(_tmp62A,_tmp629,({const char*_tmp13B="logical-and expression";_tag_dyneither(_tmp13B,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 842
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 844
({struct Cyc_Tcenv_Tenv*_tmp62C=te;struct Cyc_Absyn_Exp*_tmp62B=e1;Cyc_Tcexp_tcTest(_tmp62C,_tmp62B,({const char*_tmp13C="logical-or expression";_tag_dyneither(_tmp13C,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp62E=te;struct Cyc_Absyn_Exp*_tmp62D=e2;Cyc_Tcexp_tcTest(_tmp62E,_tmp62D,({const char*_tmp13D="logical-or expression";_tag_dyneither(_tmp13D,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 850
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 857
({struct Cyc_Tcenv_Tenv*_tmp62F=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp62F,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 862
{void*_tmp13E=Cyc_Tcutil_compress(t1);void*_tmp13F=_tmp13E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13F)->tag == 4U){_LL1: _LL2:
({void*_tmp140=0U;({unsigned int _tmp631=loc;struct _dyneither_ptr _tmp630=({const char*_tmp141="cannot assign to an array";_tag_dyneither(_tmp141,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp631,_tmp630,_tag_dyneither(_tmp140,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 867
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp142=0U;({unsigned int _tmp633=loc;struct _dyneither_ptr _tmp632=({const char*_tmp143="type is abstract (can't determine size).";_tag_dyneither(_tmp143,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp633,_tmp632,_tag_dyneither(_tmp142,sizeof(void*),0U));});});
# 871
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp144=0U;({struct Cyc_Tcenv_Tenv*_tmp637=te;unsigned int _tmp636=loc;void**_tmp635=topt;struct _dyneither_ptr _tmp634=({const char*_tmp145="assignment to non-lvalue";_tag_dyneither(_tmp145,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp637,_tmp636,_tmp635,_tmp634,_tag_dyneither(_tmp144,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp146=0U;({unsigned int _tmp639=e2->loc;struct _dyneither_ptr _tmp638=({const char*_tmp147="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp147,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp639,_tmp638,_tag_dyneither(_tmp146,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp148=({struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U,({
struct _dyneither_ptr _tmp63A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F7.f1=_tmp63A;});_tmp4F7;});struct Cyc_String_pa_PrintArg_struct _tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0U,({struct _dyneither_ptr _tmp63B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F6.f1=_tmp63B;});_tmp4F6;});void*_tmp149[2U];_tmp149[0]=& _tmp14B,_tmp149[1]=& _tmp14C;({struct Cyc_Tcenv_Tenv*_tmp63F=te;unsigned int _tmp63E=loc;void**_tmp63D=topt;struct _dyneither_ptr _tmp63C=({const char*_tmp14A="type mismatch: %s != %s";_tag_dyneither(_tmp14A,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp63F,_tmp63E,_tmp63D,_tmp63C,_tag_dyneither(_tmp149,sizeof(void*),2U));});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp148;}}else{
# 885
enum Cyc_Absyn_Primop _tmp14D=(enum Cyc_Absyn_Primop)po->v;
void*_tmp14E=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp14D,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp14E,t1) || Cyc_Tcutil_is_arithmetic_type(_tmp14E) && Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp14F=({struct Cyc_String_pa_PrintArg_struct _tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,({
# 890
struct _dyneither_ptr _tmp640=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F9.f1=_tmp640;});_tmp4F9;});struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U,({
struct _dyneither_ptr _tmp641=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F8.f1=_tmp641;});_tmp4F8;});void*_tmp150[2U];_tmp150[0]=& _tmp152,_tmp150[1]=& _tmp153;({struct Cyc_Tcenv_Tenv*_tmp645=te;unsigned int _tmp644=loc;void**_tmp643=topt;struct _dyneither_ptr _tmp642=({const char*_tmp151="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp151,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp645,_tmp644,_tmp643,_tmp642,_tag_dyneither(_tmp150,sizeof(void*),2U));});});
Cyc_Tcutil_unify(_tmp14E,t1);
Cyc_Tcutil_explain_failure();
return _tmp14F;}
# 896
return _tmp14E;}
# 898
return t1;};};}
# 902
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp646=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp646,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp648=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp647=topt;Cyc_Tcexp_tcExp(_tmp648,_tmp647,e2);});
return(void*)_check_null(e2->topt);}
# 909
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 912
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 915
{void*_tmp154=Cyc_Tcutil_compress(t1);void*_tmp155=_tmp154;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f1)->f1 == 0){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 922
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp156=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp157=_tmp156;struct _tuple1*_tmp15B;struct Cyc_List_List*_tmp15A;unsigned int _tmp159;enum Cyc_Absyn_Scope _tmp158;_LLA: _tmp15B=_tmp157.name;_tmp15A=_tmp157.typs;_tmp159=_tmp157.loc;_tmp158=_tmp157.sc;_LLB:;
# 925
if(_tmp15A == 0  || _tmp15A->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp15A->hd)).f2);
# 928
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 933
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp15C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp15D=_tmp15C;struct _tuple1*_tmp161;struct Cyc_List_List*_tmp160;unsigned int _tmp15F;enum Cyc_Absyn_Scope _tmp15E;_LLD: _tmp161=_tmp15D.name;_tmp160=_tmp15D.typs;_tmp15F=_tmp15D.loc;_tmp15E=_tmp15D.sc;_LLE:;
# 936
if(_tmp160 == 0  || _tmp160->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp160->hd)).f2);
# 940
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 945
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,({
struct _dyneither_ptr _tmp649=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp4FB.f1=_tmp649;});_tmp4FB;});struct Cyc_String_pa_PrintArg_struct _tmp165=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,({struct _dyneither_ptr _tmp64A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FA.f1=_tmp64A;});_tmp4FA;});void*_tmp162[2U];_tmp162[0]=& _tmp164,_tmp162[1]=& _tmp165;({unsigned int _tmp64C=e->loc;struct _dyneither_ptr _tmp64B=({const char*_tmp163="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp163,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp64C,_tmp64B,_tag_dyneither(_tmp162,sizeof(void*),2U));});});
return 0;}
# 951
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 957
struct Cyc_List_List*_tmp166=args;
int _tmp167=0;
struct Cyc_Tcenv_Tenv*_tmp168=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp169=Cyc_Tcenv_clear_abstract_val_ok(_tmp168);
Cyc_Tcexp_tcExp(_tmp169,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 966
void*_tmp16A=t;void*_tmp1CD;struct Cyc_Absyn_Tqual _tmp1CC;void*_tmp1CB;void*_tmp1CA;void*_tmp1C9;void*_tmp1C8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->tag == 3U){_LL1: _tmp1CD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).elt_type;_tmp1CC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).elt_tq;_tmp1CB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).rgn;_tmp1CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).nullable;_tmp1C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).bounds;_tmp1C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16A)->f1).ptr_atts).zero_term;_LL2:
# 971
 Cyc_Tcenv_check_rgn_accessible(_tmp169,loc,_tmp1CB);
# 973
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1C9);{
void*_tmp16B=Cyc_Tcutil_compress(_tmp1CD);void*_tmp16C=_tmp16B;struct Cyc_List_List*_tmp1C4;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C2;void*_tmp1C1;struct Cyc_List_List*_tmp1C0;int _tmp1BF;struct Cyc_Absyn_VarargInfo*_tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_List_List*_tmp1BA;struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_List_List*_tmp1B8;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->tag == 5U){_LL6: _tmp1C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).tvars;_tmp1C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).effect;_tmp1C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).ret_tqual;_tmp1C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).ret_type;_tmp1C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).args;_tmp1BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).c_varargs;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).cyc_varargs;_tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).rgn_po;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).attributes;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).requires_clause;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).requires_relns;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).ensures_clause;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16C)->f1).ensures_relns;_LL7:
# 978
 if(_tmp1C4 != 0  || _tmp1BD != 0)
({void*_tmp16D=0U;({unsigned int _tmp64E=e->loc;struct _dyneither_ptr _tmp64D=({const char*_tmp16E="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp16E,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp64E,_tmp64D,_tag_dyneither(_tmp16D,sizeof(void*),0U));});});
# 982
if(topt != 0)Cyc_Tcutil_unify(_tmp1C1,*topt);
# 984
while(_tmp166 != 0  && _tmp1C0 != 0){
# 986
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp166->hd;
void*t2=(*((struct _tuple9*)_tmp1C0->hd)).f3;
Cyc_Tcexp_tcExp(_tmp169,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp169,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp17E="actual argument has type ";_tag_dyneither(_tmp17E,sizeof(char),26U);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp17D=" but formal has type ";_tag_dyneither(_tmp17D,sizeof(char),22U);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp651=({unsigned long _tmp650=({unsigned long _tmp64F=Cyc_strlen((struct _dyneither_ptr)s0);_tmp64F + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp650 + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp651 + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp16F=0U;({unsigned int _tmp654=e1->loc;struct _dyneither_ptr _tmp653=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0U,_tmp4FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp4FF;});struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,_tmp4FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp4FE;});struct Cyc_String_pa_PrintArg_struct _tmp174=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,_tmp4FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp4FD;});struct Cyc_String_pa_PrintArg_struct _tmp175=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,_tmp4FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp4FC;});void*_tmp170[4U];_tmp170[0]=& _tmp172,_tmp170[1]=& _tmp173,_tmp170[2]=& _tmp174,_tmp170[3]=& _tmp175;({struct _dyneither_ptr _tmp652=({const char*_tmp171="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp171,sizeof(char),15U);});Cyc_aprintf(_tmp652,_tag_dyneither(_tmp170,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp654,_tmp653,_tag_dyneither(_tmp16F,sizeof(void*),0U));});});else{
# 998
({void*_tmp176=0U;({unsigned int _tmp657=e1->loc;struct _dyneither_ptr _tmp656=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp503;_tmp503.tag=0U,_tmp503.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp503;});struct Cyc_String_pa_PrintArg_struct _tmp17A=({struct Cyc_String_pa_PrintArg_struct _tmp502;_tmp502.tag=0U,_tmp502.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp502;});struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,_tmp501.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp501;});struct Cyc_String_pa_PrintArg_struct _tmp17C=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,_tmp500.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp500;});void*_tmp177[4U];_tmp177[0]=& _tmp179,_tmp177[1]=& _tmp17A,_tmp177[2]=& _tmp17B,_tmp177[3]=& _tmp17C;({struct _dyneither_ptr _tmp655=({const char*_tmp178="%s%s%s%s.";_tag_dyneither(_tmp178,sizeof(char),10U);});Cyc_aprintf(_tmp655,_tag_dyneither(_tmp177,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp657,_tmp656,_tag_dyneither(_tmp176,sizeof(void*),0U));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1003
if(alias_coercion)
({struct Cyc_List_List*_tmp658=({struct Cyc_List_List*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->hd=(void*)_tmp167,_tmp17F->tl=*alias_arg_exps;_tmp17F;});*alias_arg_exps=_tmp658;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp180=0U;({unsigned int _tmp65A=e1->loc;struct _dyneither_ptr _tmp659=({const char*_tmp181="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp181,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp65A,_tmp659,_tag_dyneither(_tmp180,sizeof(void*),0U));});});
_tmp166=_tmp166->tl;
_tmp1C0=_tmp1C0->tl;
++ _tmp167;}{
# 1014
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1BC;for(0;a != 0;a=a->tl){
void*_tmp182=(void*)a->hd;void*_tmp183=_tmp182;enum Cyc_Absyn_Format_Type _tmp190;int _tmp18F;int _tmp18E;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp183)->tag == 19U){_LLB: _tmp190=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp183)->f1;_tmp18F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp183)->f2;_tmp18E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp183)->f3;_LLC:
# 1018
{struct _handler_cons _tmp184;_push_handler(& _tmp184);{int _tmp186=0;if(setjmp(_tmp184.handler))_tmp186=1;if(!_tmp186){
# 1020
{struct Cyc_Absyn_Exp*_tmp187=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp18F - 1);
# 1022
struct Cyc_Core_Opt*fmt_args;
if(_tmp18E == 0)
fmt_args=0;else{
# 1026
({struct Cyc_Core_Opt*_tmp65C=({struct Cyc_Core_Opt*_tmp188=_cycalloc(sizeof(*_tmp188));({struct Cyc_List_List*_tmp65B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp18E - 1);_tmp188->v=_tmp65B;});_tmp188;});fmt_args=_tmp65C;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp189=_tmp190;if(_tmp189 == Cyc_Absyn_Printf_ft){_LL10: _LL11:
# 1030
 Cyc_Tcexp_check_format_args(_tmp169,_tmp187,fmt_args,_tmp18E - 1,alias_arg_exps,Cyc_Formatstr_get_format_types);
# 1033
goto _LLF;}else{_LL12: _LL13:
# 1035
 Cyc_Tcexp_check_format_args(_tmp169,_tmp187,fmt_args,_tmp18E - 1,alias_arg_exps,Cyc_Formatstr_get_scanf_types);
# 1038
goto _LLF;}_LLF:;};}
# 1020
;_pop_handler();}else{void*_tmp185=(void*)_exn_thrown;void*_tmp18A=_tmp185;void*_tmp18D;if(((struct Cyc_List_Nth_exn_struct*)_tmp18A)->tag == Cyc_List_Nth){_LL15: _LL16:
# 1040
({void*_tmp18B=0U;({unsigned int _tmp65E=loc;struct _dyneither_ptr _tmp65D=({const char*_tmp18C="bad format arguments";_tag_dyneither(_tmp18C,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp65E,_tmp65D,_tag_dyneither(_tmp18B,sizeof(void*),0U));});});goto _LL14;}else{_LL17: _tmp18D=_tmp18A;_LL18:(int)_rethrow(_tmp18D);}_LL14:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1045
if(_tmp1C0 != 0)({void*_tmp191=0U;({unsigned int _tmp660=loc;struct _dyneither_ptr _tmp65F=({const char*_tmp192="too few arguments for function";_tag_dyneither(_tmp192,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp660,_tmp65F,_tag_dyneither(_tmp191,sizeof(void*),0U));});});else{
# 1047
if((_tmp166 != 0  || _tmp1BF) || _tmp1BE != 0){
if(_tmp1BF)
for(0;_tmp166 != 0;_tmp166=_tmp166->tl){
Cyc_Tcexp_tcExp(_tmp169,0,(struct Cyc_Absyn_Exp*)_tmp166->hd);}else{
if(_tmp1BE == 0)
({void*_tmp193=0U;({unsigned int _tmp662=loc;struct _dyneither_ptr _tmp661=({const char*_tmp194="too many arguments for function";_tag_dyneither(_tmp194,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp662,_tmp661,_tag_dyneither(_tmp193,sizeof(void*),0U));});});else{
# 1054
struct Cyc_Absyn_VarargInfo _tmp195=*_tmp1BE;struct Cyc_Absyn_VarargInfo _tmp196=_tmp195;void*_tmp1B4;int _tmp1B3;_LL1A: _tmp1B4=_tmp196.type;_tmp1B3=_tmp196.inject;_LL1B:;{
struct Cyc_Absyn_VarargCallInfo*_tmp197=({struct Cyc_Absyn_VarargCallInfo*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->num_varargs=0,_tmp1B2->injectors=0,_tmp1B2->vai=_tmp1BE;_tmp1B2;});
# 1058
*vararg_call_info=_tmp197;
# 1060
if(!_tmp1B3)
# 1062
for(0;_tmp166 != 0;(_tmp166=_tmp166->tl,_tmp167 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp166->hd;
++ _tmp197->num_varargs;
Cyc_Tcexp_tcExp(_tmp169,& _tmp1B4,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp169,e1,_tmp1B4,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({
struct _dyneither_ptr _tmp663=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1B4));_tmp505.f1=_tmp663;});_tmp505;});struct Cyc_String_pa_PrintArg_struct _tmp19B=({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U,({struct _dyneither_ptr _tmp664=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp504.f1=_tmp664;});_tmp504;});void*_tmp198[2U];_tmp198[0]=& _tmp19A,_tmp198[1]=& _tmp19B;({unsigned int _tmp666=loc;struct _dyneither_ptr _tmp665=({const char*_tmp199="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp199,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp666,_tmp665,_tag_dyneither(_tmp198,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 1072
if(alias_coercion)
({struct Cyc_List_List*_tmp667=({struct Cyc_List_List*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->hd=(void*)_tmp167,_tmp19C->tl=*alias_arg_exps;_tmp19C;});*alias_arg_exps=_tmp667;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1B4) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp19D=0U;({unsigned int _tmp669=e1->loc;struct _dyneither_ptr _tmp668=({const char*_tmp19E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp19E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp669,_tmp668,_tag_dyneither(_tmp19D,sizeof(void*),0U));});});}else{
# 1081
void*_tmp19F=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1B4));void*_tmp1A0=_tmp19F;struct Cyc_Absyn_Datatypedecl*_tmp1B1;struct Cyc_List_List*_tmp1B0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp1B1=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f1)->f1).KnownDatatype).val;_tmp1B0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2;_LL1E: {
# 1085
struct Cyc_Absyn_Datatypedecl*_tmp1A1=*Cyc_Tcenv_lookup_datatypedecl(_tmp169,loc,_tmp1B1->name);
struct Cyc_List_List*fields=0;
if(_tmp1A1->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1A4=({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U,({
struct _dyneither_ptr _tmp66A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1B4));_tmp506.f1=_tmp66A;});_tmp506;});void*_tmp1A2[1U];_tmp1A2[0]=& _tmp1A4;({unsigned int _tmp66C=loc;struct _dyneither_ptr _tmp66B=({const char*_tmp1A3="can't inject into abstract datatype %s";_tag_dyneither(_tmp1A3,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp66C,_tmp66B,_tag_dyneither(_tmp1A2,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1A1->fields))->v;}
# 1096
if(!({void*_tmp66D=Cyc_Tcutil_pointer_region(_tmp1B4);Cyc_Tcutil_unify(_tmp66D,Cyc_Tcenv_curr_rgn(_tmp169));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1A7=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({
struct _dyneither_ptr _tmp66E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1B4));_tmp508.f1=_tmp66E;});_tmp508;});struct Cyc_String_pa_PrintArg_struct _tmp1A8=({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0U,({struct _dyneither_ptr _tmp66F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp169)));_tmp507.f1=_tmp66F;});_tmp507;});void*_tmp1A5[2U];_tmp1A5[0]=& _tmp1A7,_tmp1A5[1]=& _tmp1A8;({unsigned int _tmp671=loc;struct _dyneither_ptr _tmp670=({const char*_tmp1A6="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1A6,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp671,_tmp670,_tag_dyneither(_tmp1A5,sizeof(void*),2U));});});{
# 1100
struct Cyc_List_List*_tmp1A9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1A1->tvs,_tmp1B0);
for(0;_tmp166 != 0;_tmp166=_tmp166->tl){
++ _tmp197->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp166->hd;
# 1105
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp169,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1AA=0U;({unsigned int _tmp673=e1->loc;struct _dyneither_ptr _tmp672=({const char*_tmp1AB="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1AB,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp673,_tmp672,_tag_dyneither(_tmp1AA,sizeof(void*),0U));});});}{
# 1111
struct Cyc_Absyn_Datatypefield*_tmp1AC=({struct Cyc_Tcenv_Tenv*_tmp677=_tmp169;struct Cyc_Absyn_Exp*_tmp676=e1;void*_tmp675=Cyc_Tcutil_pointer_elt_type(_tmp1B4);struct Cyc_List_List*_tmp674=_tmp1A9;Cyc_Tcexp_tcInjection(_tmp677,_tmp676,_tmp675,_tmp674,fields);});
if(_tmp1AC != 0)
({struct Cyc_List_List*_tmp679=({
struct Cyc_List_List*_tmp678=_tmp197->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp678,({struct Cyc_List_List*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));
_tmp1AD->hd=_tmp1AC,_tmp1AD->tl=0;_tmp1AD;}));});
# 1113
_tmp197->injectors=_tmp679;});};};}
# 1117
goto _LL1C;};}}else{goto _LL1F;}}else{goto _LL1F;}}else{_LL1F: _LL20:
({void*_tmp1AE=0U;({unsigned int _tmp67B=loc;struct _dyneither_ptr _tmp67A=({const char*_tmp1AF="bad inject vararg type";_tag_dyneither(_tmp1AF,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp67B,_tmp67A,_tag_dyneither(_tmp1AE,sizeof(void*),0U));});});goto _LL1C;}_LL1C:;}};}}}}
# 1123
if(*alias_arg_exps == 0)
# 1132 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp169,loc,(void*)_check_null(_tmp1C3));
# 1134
return _tmp1C1;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1B7=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({struct _dyneither_ptr _tmp67C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp509.f1=_tmp67C;});_tmp509;});void*_tmp1B5[1U];_tmp1B5[0]=& _tmp1B7;({struct Cyc_Tcenv_Tenv*_tmp680=_tmp169;unsigned int _tmp67F=loc;void**_tmp67E=topt;struct _dyneither_ptr _tmp67D=({const char*_tmp1B6="expected pointer to function but found %s";_tag_dyneither(_tmp1B6,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp680,_tmp67F,_tmp67E,_tmp67D,_tag_dyneither(_tmp1B5,sizeof(void*),1U));});});}_LL5:;};}else{_LL3: _LL4:
# 1137
 return({struct Cyc_String_pa_PrintArg_struct _tmp1C7=({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0U,({struct _dyneither_ptr _tmp681=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp50A.f1=_tmp681;});_tmp50A;});void*_tmp1C5[1U];_tmp1C5[0]=& _tmp1C7;({struct Cyc_Tcenv_Tenv*_tmp685=_tmp169;unsigned int _tmp684=loc;void**_tmp683=topt;struct _dyneither_ptr _tmp682=({const char*_tmp1C6="expected pointer to function but found %s";_tag_dyneither(_tmp1C6,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp685,_tmp684,_tmp683,_tmp682,_tag_dyneither(_tmp1C5,sizeof(void*),1U));});});}_LL0:;};}
# 1141
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1CE=Cyc_Absyn_exn_type();
Cyc_Tcexp_tcExp(te,& _tmp1CE,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1CE,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1D1=({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0U,({struct _dyneither_ptr _tmp686=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1CE));_tmp50C.f1=_tmp686;});_tmp50C;});struct Cyc_String_pa_PrintArg_struct _tmp1D2=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,({
struct _dyneither_ptr _tmp687=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp50B.f1=_tmp687;});_tmp50B;});void*_tmp1CF[2U];_tmp1CF[0]=& _tmp1D1,_tmp1CF[1]=& _tmp1D2;({unsigned int _tmp689=loc;struct _dyneither_ptr _tmp688=({const char*_tmp1D0="expected %s but found %s";_tag_dyneither(_tmp1D0,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp689,_tmp688,_tag_dyneither(_tmp1CF,sizeof(void*),2U));});});
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1153
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1155
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1157
({void*_tmp68A=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp68A;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1D3=t1;void*_tmp1EE;struct Cyc_Absyn_Tqual _tmp1ED;void*_tmp1EC;void*_tmp1EB;void*_tmp1EA;void*_tmp1E9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->tag == 3U){_LL1: _tmp1EE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).elt_type;_tmp1ED=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).elt_tq;_tmp1EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).rgn;_tmp1EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).nullable;_tmp1EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).bounds;_tmp1E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D3)->f1).ptr_atts).zero_term;_LL2:
# 1161
{void*_tmp1D4=Cyc_Tcutil_compress(_tmp1EE);void*_tmp1D5=_tmp1D4;struct Cyc_List_List*_tmp1E8;void*_tmp1E7;struct Cyc_Absyn_Tqual _tmp1E6;void*_tmp1E5;struct Cyc_List_List*_tmp1E4;int _tmp1E3;struct Cyc_Absyn_VarargInfo*_tmp1E2;struct Cyc_List_List*_tmp1E1;struct Cyc_List_List*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_List_List*_tmp1DC;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->tag == 5U){_LL6: _tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).tvars;_tmp1E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).effect;_tmp1E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).ret_tqual;_tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).ret_type;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).args;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).c_varargs;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).cyc_varargs;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).rgn_po;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).attributes;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).requires_clause;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).requires_relns;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).ensures_clause;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1D5)->f1).ensures_relns;_LL7: {
# 1163
struct Cyc_List_List*instantiation=0;
# 1165
for(0;ts != 0  && _tmp1E8 != 0;(ts=ts->tl,_tmp1E8=_tmp1E8->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1E8->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp68E=loc;struct Cyc_Tcenv_Tenv*_tmp68D=te;struct Cyc_List_List*_tmp68C=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp68B=k;Cyc_Tcutil_check_type(_tmp68E,_tmp68D,_tmp68C,_tmp68B,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp690=({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));({struct _tuple13*_tmp68F=({struct _tuple13*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->f1=(struct Cyc_Absyn_Tvar*)_tmp1E8->hd,_tmp1D6->f2=(void*)ts->hd;_tmp1D6;});_tmp1D7->hd=_tmp68F;}),_tmp1D7->tl=instantiation;_tmp1D7;});instantiation=_tmp690;});}
# 1171
if(ts != 0)
return({void*_tmp1D8=0U;({struct Cyc_Tcenv_Tenv*_tmp694=te;unsigned int _tmp693=loc;void**_tmp692=topt;struct _dyneither_ptr _tmp691=({const char*_tmp1D9="too many type variables in instantiation";_tag_dyneither(_tmp1D9,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp694,_tmp693,_tmp692,_tmp691,_tag_dyneither(_tmp1D8,sizeof(void*),0U));});});
# 1176
if(_tmp1E8 == 0){
({struct Cyc_List_List*_tmp695=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1E1);_tmp1E1=_tmp695;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1E1);
_tmp1E1=0;}{
# 1181
void*new_fn_typ=({
struct Cyc_List_List*_tmp696=instantiation;Cyc_Tcutil_substitute(_tmp696,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));
_tmp1DB->tag=5U,(_tmp1DB->f1).tvars=_tmp1E8,(_tmp1DB->f1).effect=_tmp1E7,(_tmp1DB->f1).ret_tqual=_tmp1E6,(_tmp1DB->f1).ret_type=_tmp1E5,(_tmp1DB->f1).args=_tmp1E4,(_tmp1DB->f1).c_varargs=_tmp1E3,(_tmp1DB->f1).cyc_varargs=_tmp1E2,(_tmp1DB->f1).rgn_po=_tmp1E1,(_tmp1DB->f1).attributes=_tmp1E0,(_tmp1DB->f1).requires_clause=_tmp1DF,(_tmp1DB->f1).requires_relns=_tmp1DE,(_tmp1DB->f1).ensures_clause=_tmp1DD,(_tmp1DB->f1).ensures_relns=_tmp1DC;_tmp1DB;}));});
# 1186
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->tag=3U,(_tmp1DA->f1).elt_type=new_fn_typ,(_tmp1DA->f1).elt_tq=_tmp1ED,((_tmp1DA->f1).ptr_atts).rgn=_tmp1EC,((_tmp1DA->f1).ptr_atts).nullable=_tmp1EB,((_tmp1DA->f1).ptr_atts).bounds=_tmp1EA,((_tmp1DA->f1).ptr_atts).zero_term=_tmp1E9,((_tmp1DA->f1).ptr_atts).ptrloc=0;_tmp1DA;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1189
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1192
return({struct Cyc_String_pa_PrintArg_struct _tmp1F1=({struct Cyc_String_pa_PrintArg_struct _tmp50D;_tmp50D.tag=0U,({
struct _dyneither_ptr _tmp697=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp50D.f1=_tmp697;});_tmp50D;});void*_tmp1EF[1U];_tmp1EF[0]=& _tmp1F1;({struct Cyc_Tcenv_Tenv*_tmp69B=te;unsigned int _tmp69A=loc;void**_tmp699=topt;struct _dyneither_ptr _tmp698=({const char*_tmp1F0="expecting polymorphic type but found %s";_tag_dyneither(_tmp1F0,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp69B,_tmp69A,_tmp699,_tmp698,_tag_dyneither(_tmp1EF,sizeof(void*),1U));});});};}
# 1197
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1199
({unsigned int _tmp69F=loc;struct Cyc_Tcenv_Tenv*_tmp69E=te;struct Cyc_List_List*_tmp69D=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp69C=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1199
Cyc_Tcutil_check_type(_tmp69F,_tmp69E,_tmp69D,_tmp69C,1,0,t);});
# 1201
Cyc_Tcutil_check_no_qual(loc,t);
# 1203
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1208
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1215
Cyc_Tcutil_unify(t2,t);{
void*_tmp1F2=({struct Cyc_String_pa_PrintArg_struct _tmp1F5=({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0U,({
struct _dyneither_ptr _tmp6A0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp50F.f1=_tmp6A0;});_tmp50F;});struct Cyc_String_pa_PrintArg_struct _tmp1F6=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0U,({struct _dyneither_ptr _tmp6A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp50E.f1=_tmp6A1;});_tmp50E;});void*_tmp1F3[2U];_tmp1F3[0]=& _tmp1F5,_tmp1F3[1]=& _tmp1F6;({struct Cyc_Tcenv_Tenv*_tmp6A4=te;unsigned int _tmp6A3=loc;struct _dyneither_ptr _tmp6A2=({const char*_tmp1F4="cannot cast %s to %s";_tag_dyneither(_tmp1F4,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6A4,_tmp6A3,& t,_tmp6A2,_tag_dyneither(_tmp1F3,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return _tmp1F2;};}}}
# 1225
{struct _tuple0 _tmp1F7=({struct _tuple0 _tmp510;_tmp510.f1=e->r,({void*_tmp6A5=Cyc_Tcutil_compress(t);_tmp510.f2=_tmp6A5;});_tmp510;});struct _tuple0 _tmp1F8=_tmp1F7;int _tmp200;void*_tmp1FF;void*_tmp1FE;void*_tmp1FD;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F8.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8.f2)->tag == 3U){_LL1: _tmp200=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F8.f1)->f1).fat_result;_tmp1FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8.f2)->f1).ptr_atts).nullable;_tmp1FE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8.f2)->f1).ptr_atts).bounds;_tmp1FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8.f2)->f1).ptr_atts).zero_term;_LL2:
# 1229
 if((_tmp200  && !Cyc_Tcutil_force_type2bool(0,_tmp1FD)) && Cyc_Tcutil_force_type2bool(0,_tmp1FF)){
struct Cyc_Absyn_Exp*_tmp1F9=({void*_tmp6A6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6A6,_tmp1FE);});
if(_tmp1F9 != 0){
struct Cyc_Absyn_Exp*_tmp1FA=e;
if((Cyc_Evexp_eval_const_uint_exp(_tmp1FA)).f1 == 1)
({void*_tmp1FB=0U;({unsigned int _tmp6A8=loc;struct _dyneither_ptr _tmp6A7=({const char*_tmp1FC="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp1FC,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6A8,_tmp6A7,_tag_dyneither(_tmp1FB,sizeof(void*),0U));});});}}
# 1237
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1240
return t;};}
# 1244
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp201=0;
struct Cyc_Absyn_Tqual _tmp202=Cyc_Absyn_empty_tqual(0U);
int _tmp203=0;
if(topt != 0){
void*_tmp204=Cyc_Tcutil_compress(*topt);void*_tmp205=_tmp204;void**_tmp208;struct Cyc_Absyn_Tqual _tmp207;void*_tmp206;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205)->tag == 3U){_LL1: _tmp208=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205)->f1).elt_type;_tmp207=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205)->f1).elt_tq;_tmp206=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp205)->f1).ptr_atts).nullable;_LL2:
# 1251
 _tmp201=_tmp208;
_tmp202=_tmp207;
({int _tmp6A9=Cyc_Tcutil_force_type2bool(0,_tmp206);_tmp203=_tmp6A9;});
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1265
({struct Cyc_Tcenv_Tenv*_tmp6AB=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp6AA=_tmp201;Cyc_Tcexp_tcExpNoInst(_tmp6AB,_tmp6AA,e);});
# 1268
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp209=0U;({unsigned int _tmp6AD=e->loc;struct _dyneither_ptr _tmp6AC=({const char*_tmp20A="Cannot take the address of an alias-free path";_tag_dyneither(_tmp20A,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6AD,_tmp6AC,_tag_dyneither(_tmp209,sizeof(void*),0U));});});
# 1274
{void*_tmp20B=e->r;void*_tmp20C=_tmp20B;struct Cyc_Absyn_Exp*_tmp210;struct Cyc_Absyn_Exp*_tmp20F;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp20C)->tag == 23U){_LL6: _tmp210=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp20C)->f1;_tmp20F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp20C)->f2;_LL7:
# 1276
{void*_tmp20D=Cyc_Tcutil_compress((void*)_check_null(_tmp210->topt));void*_tmp20E=_tmp20D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp20E)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1281
({void*_tmp6AE=(Cyc_Absyn_add_exp(_tmp210,_tmp20F,0U))->r;e0->r=_tmp6AE;});
return Cyc_Tcexp_tcPlus(te,_tmp210,_tmp20F);}_LLA:;}
# 1284
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1289
{void*_tmp211=e->r;void*_tmp212=_tmp211;switch(*((int*)_tmp212)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp212)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp212)->f3 == 1){_LL12: _LL13:
# 1292
({void*_tmp213=0U;({unsigned int _tmp6B0=e->loc;struct _dyneither_ptr _tmp6AF=({const char*_tmp214="cannot take the address of a @tagged union member";_tag_dyneither(_tmp214,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6B0,_tmp6AF,_tag_dyneither(_tmp213,sizeof(void*),0U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1298
struct _tuple14 _tmp215=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp216=_tmp215;int _tmp21A;void*_tmp219;_LL17: _tmp21A=_tmp216.f1;_tmp219=_tmp216.f2;_LL18:;
# 1300
if(Cyc_Tcutil_is_noalias_region(_tmp219,0))
({void*_tmp217=0U;({unsigned int _tmp6B2=e->loc;struct _dyneither_ptr _tmp6B1=({const char*_tmp218="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp218,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp6B2,_tmp6B1,_tag_dyneither(_tmp217,sizeof(void*),0U));});});{
# 1303
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp21A){
tq.print_const=1;
tq.real_const=1;}{
# 1309
void*t=_tmp203?
Cyc_Absyn_star_type((void*)_check_null(e->topt),_tmp219,tq,Cyc_Absyn_false_type):
 Cyc_Absyn_at_type((void*)_check_null(e->topt),_tmp219,tq,Cyc_Absyn_false_type);
return t;};};};}
# 1316
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1320
return Cyc_Absyn_uint_type;
# 1322
({unsigned int _tmp6B5=loc;struct Cyc_Tcenv_Tenv*_tmp6B4=te;struct Cyc_List_List*_tmp6B3=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6B5,_tmp6B4,_tmp6B3,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp21D=({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0U,({
struct _dyneither_ptr _tmp6B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp511.f1=_tmp6B6;});_tmp511;});void*_tmp21B[1U];_tmp21B[0]=& _tmp21D;({unsigned int _tmp6B8=loc;struct _dyneither_ptr _tmp6B7=({const char*_tmp21C="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp21C,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp6B8,_tmp6B7,_tag_dyneither(_tmp21B,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp21E=Cyc_Tcutil_compress(*topt);void*_tmp21F=_tmp21E;void*_tmp224;void*_tmp223;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2 != 0){_LL1: _tmp224=_tmp21F;_tmp223=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2)->hd;_LL2: {
# 1331
struct Cyc_Absyn_Exp*_tmp220=Cyc_Absyn_sizeoftype_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp221=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp222=_cycalloc(sizeof(*_tmp222));_tmp222->tag=9U,_tmp222->f1=_tmp220;_tmp222;});
if(Cyc_Tcutil_unify(_tmp223,(void*)_tmp221))return _tmp224;
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1337
return Cyc_Absyn_uint_type;}
# 1340
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1347
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1349
({unsigned int _tmp6BB=loc;struct Cyc_Tcenv_Tenv*_tmp6BA=te;struct Cyc_List_List*_tmp6B9=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6BB,_tmp6BA,_tmp6B9,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp225=fs;
void*_tmp226=t_orig;
for(0;_tmp225 != 0;_tmp225=_tmp225->tl){
void*_tmp227=(void*)_tmp225->hd;
void*_tmp228=_tmp227;unsigned int _tmp262;struct _dyneither_ptr*_tmp261;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp228)->tag == 0U){_LL1: _tmp261=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp228)->f1;_LL2: {
# 1357
int bad_type=1;
{void*_tmp229=Cyc_Tcutil_compress(_tmp226);void*_tmp22A=_tmp229;struct Cyc_List_List*_tmp232;struct Cyc_Absyn_Aggrdecl**_tmp231;switch(*((int*)_tmp22A)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp231=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f1)->f1).KnownAggr).val;_LL7:
# 1360
 if((*_tmp231)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp261,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp231)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp22D=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0U,_tmp512.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp261);_tmp512;});void*_tmp22B[1U];_tmp22B[0]=& _tmp22D;({unsigned int _tmp6BD=loc;struct _dyneither_ptr _tmp6BC=({const char*_tmp22C="no field of struct/union has name %s";_tag_dyneither(_tmp22C,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6BD,_tmp6BC,_tag_dyneither(_tmp22B,sizeof(void*),1U));});});else{
# 1365
_tmp226=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp232=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22A)->f2;_LL9: {
# 1369
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp261,_tmp232);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp230=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,_tmp513.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp261);_tmp513;});void*_tmp22E[1U];_tmp22E[0]=& _tmp230;({unsigned int _tmp6BF=loc;struct _dyneither_ptr _tmp6BE=({const char*_tmp22F="no field of struct/union has name %s";_tag_dyneither(_tmp22F,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6BF,_tmp6BE,_tag_dyneither(_tmp22E,sizeof(void*),1U));});});else{
# 1373
_tmp226=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1378
if(bad_type){
if(_tmp225 == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp235=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,({struct _dyneither_ptr _tmp6C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp226));_tmp514.f1=_tmp6C0;});_tmp514;});void*_tmp233[1U];_tmp233[0]=& _tmp235;({unsigned int _tmp6C2=loc;struct _dyneither_ptr _tmp6C1=({const char*_tmp234="%s is not a known struct/union type";_tag_dyneither(_tmp234,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6C2,_tmp6C1,_tag_dyneither(_tmp233,sizeof(void*),1U));});});else{
# 1382
struct _dyneither_ptr _tmp236=({struct Cyc_String_pa_PrintArg_struct _tmp249=({struct Cyc_String_pa_PrintArg_struct _tmp51B;_tmp51B.tag=0U,({struct _dyneither_ptr _tmp6C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp51B.f1=_tmp6C3;});_tmp51B;});void*_tmp247[1U];_tmp247[0]=& _tmp249;({struct _dyneither_ptr _tmp6C4=({const char*_tmp248="(%s)";_tag_dyneither(_tmp248,sizeof(char),5U);});Cyc_aprintf(_tmp6C4,_tag_dyneither(_tmp247,sizeof(void*),1U));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp225;x=x->tl){
void*_tmp237=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp238=_tmp237;unsigned int _tmp242;struct _dyneither_ptr*_tmp241;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp238)->tag == 0U){_LLD: _tmp241=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp238)->f1;_LLE:
# 1387
({struct _dyneither_ptr _tmp6C6=({struct Cyc_String_pa_PrintArg_struct _tmp23B=({struct Cyc_String_pa_PrintArg_struct _tmp516;_tmp516.tag=0U,_tmp516.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp236);_tmp516;});struct Cyc_String_pa_PrintArg_struct _tmp23C=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp241);_tmp515;});void*_tmp239[2U];_tmp239[0]=& _tmp23B,_tmp239[1]=& _tmp23C;({struct _dyneither_ptr _tmp6C5=({const char*_tmp23A="%s.%s";_tag_dyneither(_tmp23A,sizeof(char),6U);});Cyc_aprintf(_tmp6C5,_tag_dyneither(_tmp239,sizeof(void*),2U));});});_tmp236=_tmp6C6;});goto _LLC;}else{_LLF: _tmp242=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp238)->f1;_LL10:
# 1389
({struct _dyneither_ptr _tmp6C8=({struct Cyc_String_pa_PrintArg_struct _tmp23F=({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0U,_tmp518.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp236);_tmp518;});struct Cyc_Int_pa_PrintArg_struct _tmp240=({struct Cyc_Int_pa_PrintArg_struct _tmp517;_tmp517.tag=1U,_tmp517.f1=(unsigned long)((int)_tmp242);_tmp517;});void*_tmp23D[2U];_tmp23D[0]=& _tmp23F,_tmp23D[1]=& _tmp240;({struct _dyneither_ptr _tmp6C7=({const char*_tmp23E="%s.%d";_tag_dyneither(_tmp23E,sizeof(char),6U);});Cyc_aprintf(_tmp6C7,_tag_dyneither(_tmp23D,sizeof(void*),2U));});});_tmp236=_tmp6C8;});goto _LLC;}_LLC:;}
# 1391
({struct Cyc_String_pa_PrintArg_struct _tmp245=({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0U,_tmp51A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp236);_tmp51A;});struct Cyc_String_pa_PrintArg_struct _tmp246=({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0U,({struct _dyneither_ptr _tmp6C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp226));_tmp519.f1=_tmp6C9;});_tmp519;});void*_tmp243[2U];_tmp243[0]=& _tmp245,_tmp243[1]=& _tmp246;({unsigned int _tmp6CB=loc;struct _dyneither_ptr _tmp6CA=({const char*_tmp244="%s == %s is not a struct/union type";_tag_dyneither(_tmp244,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6CB,_tmp6CA,_tag_dyneither(_tmp243,sizeof(void*),2U));});});}}
# 1394
goto _LL0;}}else{_LL3: _tmp262=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp228)->f1;_LL4: {
# 1396
int bad_type=1;
{void*_tmp24A=Cyc_Tcutil_compress(_tmp226);void*_tmp24B=_tmp24A;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25C;struct Cyc_Absyn_Datatypefield*_tmp25B;struct Cyc_Absyn_Aggrdecl**_tmp25A;switch(*((int*)_tmp24B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24B)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24B)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp25A=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24B)->f1)->f1).KnownAggr).val;_LL13:
# 1399
 if((*_tmp25A)->impl == 0)
goto _LL11;
_tmp25C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp25A)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24B)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp25B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24B)->f1)->f1).KnownDatatypefield).val).f2;_LL19:
# 1419
 if(({int _tmp6CC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25B->typs);_tmp6CC < _tmp262;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp256=({struct Cyc_Int_pa_PrintArg_struct _tmp51D;_tmp51D.tag=1U,({
unsigned long _tmp6CD=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25B->typs);_tmp51D.f1=_tmp6CD;});_tmp51D;});struct Cyc_Int_pa_PrintArg_struct _tmp257=({struct Cyc_Int_pa_PrintArg_struct _tmp51C;_tmp51C.tag=1U,_tmp51C.f1=(unsigned long)((int)_tmp262);_tmp51C;});void*_tmp254[2U];_tmp254[0]=& _tmp256,_tmp254[1]=& _tmp257;({unsigned int _tmp6CF=loc;struct _dyneither_ptr _tmp6CE=({const char*_tmp255="datatype field has too few components: %d < %d";_tag_dyneither(_tmp255,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6CF,_tmp6CE,_tag_dyneither(_tmp254,sizeof(void*),2U));});});else{
# 1423
if(_tmp262 != 0)
({void*_tmp6D0=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp25B->typs,(int)(_tmp262 - 1))).f2;_tmp226=_tmp6D0;});else{
if(_tmp225->tl != 0)
({void*_tmp258=0U;({unsigned int _tmp6D2=loc;struct _dyneither_ptr _tmp6D1=({const char*_tmp259="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp259,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6D2,_tmp6D1,_tag_dyneither(_tmp258,sizeof(void*),0U));});});}}
# 1428
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp25C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24B)->f2;_LL15:
# 1403
 if(({int _tmp6D3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25C);_tmp6D3 <= _tmp262;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp24E=({struct Cyc_Int_pa_PrintArg_struct _tmp51F;_tmp51F.tag=1U,({
unsigned long _tmp6D4=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25C);_tmp51F.f1=_tmp6D4;});_tmp51F;});struct Cyc_Int_pa_PrintArg_struct _tmp24F=({struct Cyc_Int_pa_PrintArg_struct _tmp51E;_tmp51E.tag=1U,_tmp51E.f1=(unsigned long)((int)_tmp262);_tmp51E;});void*_tmp24C[2U];_tmp24C[0]=& _tmp24E,_tmp24C[1]=& _tmp24F;({unsigned int _tmp6D6=loc;struct _dyneither_ptr _tmp6D5=({const char*_tmp24D="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp24D,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6D6,_tmp6D5,_tag_dyneither(_tmp24C,sizeof(void*),2U));});});else{
# 1407
({void*_tmp6D7=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp25C,(int)_tmp262))->type;_tmp226=_tmp6D7;});}
bad_type=0;
goto _LL11;case 6U: _LL16: _tmp25D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24B)->f1;_LL17:
# 1411
 if(({int _tmp6D8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25D);_tmp6D8 <= _tmp262;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp252=({struct Cyc_Int_pa_PrintArg_struct _tmp521;_tmp521.tag=1U,({
unsigned long _tmp6D9=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25D);_tmp521.f1=_tmp6D9;});_tmp521;});struct Cyc_Int_pa_PrintArg_struct _tmp253=({struct Cyc_Int_pa_PrintArg_struct _tmp520;_tmp520.tag=1U,_tmp520.f1=(unsigned long)((int)_tmp262);_tmp520;});void*_tmp250[2U];_tmp250[0]=& _tmp252,_tmp250[1]=& _tmp253;({unsigned int _tmp6DB=loc;struct _dyneither_ptr _tmp6DA=({const char*_tmp251="tuple has too few components: %d <= %d";_tag_dyneither(_tmp251,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6DB,_tmp6DA,_tag_dyneither(_tmp250,sizeof(void*),2U));});});else{
# 1415
({void*_tmp6DC=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp25D,(int)_tmp262)).f2;_tmp226=_tmp6DC;});}
bad_type=0;
goto _LL11;default: _LL1A: _LL1B:
# 1430
 goto _LL11;}_LL11:;}
# 1432
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp260=({struct Cyc_String_pa_PrintArg_struct _tmp522;_tmp522.tag=0U,({struct _dyneither_ptr _tmp6DD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp226));_tmp522.f1=_tmp6DD;});_tmp522;});void*_tmp25E[1U];_tmp25E[0]=& _tmp260;({unsigned int _tmp6DF=loc;struct _dyneither_ptr _tmp6DE=({const char*_tmp25F="%s is not a known type";_tag_dyneither(_tmp25F,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6DF,_tmp6DE,_tag_dyneither(_tmp25E,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1437
return Cyc_Absyn_uint_type;};}
# 1441
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp263=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp263,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp264=t;void*_tmp27B;void*_tmp27A;void*_tmp279;void*_tmp278;switch(*((int*)_tmp264)){case 1U: _LL1: _LL2: {
# 1447
struct Cyc_List_List*_tmp265=Cyc_Tcenv_lookup_type_vars(_tmp263);
void*_tmp266=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->v=_tmp265;_tmp26E;}));
void*_tmp267=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->v=_tmp265;_tmp26D;}));
void*_tmp268=Cyc_Tcutil_any_bounds(& _tmp263);
void*_tmp269=Cyc_Tcutil_any_bool(& _tmp263);
struct Cyc_Absyn_PtrAtts _tmp26A=({struct Cyc_Absyn_PtrAtts _tmp523;_tmp523.rgn=_tmp267,({void*_tmp6E0=Cyc_Tcutil_any_bool(& _tmp263);_tmp523.nullable=_tmp6E0;}),_tmp523.bounds=_tmp268,_tmp523.zero_term=_tmp269,_tmp523.ptrloc=0;_tmp523;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26B=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->tag=3U,(_tmp26C->f1).elt_type=_tmp266,({struct Cyc_Absyn_Tqual _tmp6E1=Cyc_Absyn_empty_tqual(0U);(_tmp26C->f1).elt_tq=_tmp6E1;}),(_tmp26C->f1).ptr_atts=_tmp26A;_tmp26C;});
Cyc_Tcutil_unify(t,(void*)_tmp26B);
_tmp27B=_tmp266;_tmp27A=_tmp267;_tmp279=_tmp268;_tmp278=_tmp269;goto _LL4;}case 3U: _LL3: _tmp27B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp264)->f1).elt_type;_tmp27A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp264)->f1).ptr_atts).rgn;_tmp279=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp264)->f1).ptr_atts).bounds;_tmp278=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp264)->f1).ptr_atts).zero_term;_LL4:
# 1457
 Cyc_Tcenv_check_rgn_accessible(_tmp263,loc,_tmp27A);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp279);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmp27B),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp263)){
void*_tmp26F=Cyc_Tcutil_compress(_tmp27B);void*_tmp270=_tmp26F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp270)->tag == 5U){_LL8: _LL9:
# 1462
 if(Cyc_Tc_aggressive_warn)
({void*_tmp271=0U;({unsigned int _tmp6E3=loc;struct _dyneither_ptr _tmp6E2=({const char*_tmp272="unnecessary dereference for function type";_tag_dyneither(_tmp272,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6E3,_tmp6E2,_tag_dyneither(_tmp271,sizeof(void*),0U));});});
return t;}else{_LLA: _LLB:
# 1466
({void*_tmp273=0U;({unsigned int _tmp6E5=loc;struct _dyneither_ptr _tmp6E4=({const char*_tmp274="can't dereference abstract pointer type";_tag_dyneither(_tmp274,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6E5,_tmp6E4,_tag_dyneither(_tmp273,sizeof(void*),0U));});});}_LL7:;}
# 1469
return _tmp27B;default: _LL5: _LL6:
# 1471
 return({struct Cyc_String_pa_PrintArg_struct _tmp277=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,({struct _dyneither_ptr _tmp6E6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp524.f1=_tmp6E6;});_tmp524;});void*_tmp275[1U];_tmp275[0]=& _tmp277;({struct Cyc_Tcenv_Tenv*_tmp6EA=_tmp263;unsigned int _tmp6E9=loc;void**_tmp6E8=topt;struct _dyneither_ptr _tmp6E7=({const char*_tmp276="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp276,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp6EA,_tmp6E9,_tmp6E8,_tmp6E7,_tag_dyneither(_tmp275,sizeof(void*),1U));});});}_LL0:;};}
# 1476
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1480
({struct Cyc_Tcenv_Tenv*_tmp6EB=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6EB,0,e);});
# 1482
({int _tmp6EC=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6EC;});{
void*_tmp27C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp27D=_tmp27C;enum Cyc_Absyn_AggrKind _tmp297;struct Cyc_List_List*_tmp296;struct Cyc_Absyn_Aggrdecl*_tmp295;struct Cyc_List_List*_tmp294;switch(*((int*)_tmp27D)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp295=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f1)->f1).KnownAggr).val;_tmp294=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2;_LL2: {
# 1485
struct Cyc_Absyn_Aggrfield*_tmp27E=Cyc_Absyn_lookup_decl_field(_tmp295,f);
if(_tmp27E == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp281=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,({
struct _dyneither_ptr _tmp6ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp295->name));_tmp526.f1=_tmp6ED;});_tmp526;});struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0U,_tmp525.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp525;});void*_tmp27F[2U];_tmp27F[0]=& _tmp281,_tmp27F[1]=& _tmp282;({struct Cyc_Tcenv_Tenv*_tmp6F1=te;unsigned int _tmp6F0=loc;void**_tmp6EF=topt;struct _dyneither_ptr _tmp6EE=({const char*_tmp280="%s has no %s member";_tag_dyneither(_tmp280,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp6F1,_tmp6F0,_tmp6EF,_tmp6EE,_tag_dyneither(_tmp27F,sizeof(void*),2U));});});
# 1490
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp295->impl))->tagged)*is_tagged=1;{
void*t2=_tmp27E->type;
if(_tmp294 != 0){
struct Cyc_List_List*_tmp283=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp295->tvs,_tmp294);
({void*_tmp6F2=Cyc_Tcutil_substitute(_tmp283,_tmp27E->type);t2=_tmp6F2;});}
# 1498
if((((_tmp295->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp295->impl))->tagged) && !Cyc_Tcutil_is_bits_only_type(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp27E->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,_tmp527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp527;});void*_tmp284[1U];_tmp284[0]=& _tmp286;({struct Cyc_Tcenv_Tenv*_tmp6F6=te;unsigned int _tmp6F5=loc;void**_tmp6F4=topt;struct _dyneither_ptr _tmp6F3=({const char*_tmp285="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp285,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6F6,_tmp6F5,_tmp6F4,_tmp6F3,_tag_dyneither(_tmp284,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp295->impl))->exist_vars != 0){
# 1503
if(!({void*_tmp6F7=t2;Cyc_Tcutil_unify(_tmp6F7,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0U,_tmp528.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp528;});void*_tmp287[1U];_tmp287[0]=& _tmp289;({struct Cyc_Tcenv_Tenv*_tmp6FB=te;unsigned int _tmp6FA=loc;void**_tmp6F9=topt;struct _dyneither_ptr _tmp6F8=({const char*_tmp288="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp288,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp6FB,_tmp6FA,_tmp6F9,_tmp6F8,_tag_dyneither(_tmp287,sizeof(void*),1U));});});}
# 1506
return t2;};}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp297=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27D)->f1;_tmp296=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp27D)->f2;_LL4: {
# 1508
struct Cyc_Absyn_Aggrfield*_tmp28A=Cyc_Absyn_lookup_field(_tmp296,f);
if(_tmp28A == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp28D=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,_tmp529.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp529;});void*_tmp28B[1U];_tmp28B[0]=& _tmp28D;({struct Cyc_Tcenv_Tenv*_tmp6FF=te;unsigned int _tmp6FE=loc;void**_tmp6FD=topt;struct _dyneither_ptr _tmp6FC=({const char*_tmp28C="type has no %s member";_tag_dyneither(_tmp28C,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp6FF,_tmp6FE,_tmp6FD,_tmp6FC,_tag_dyneither(_tmp28B,sizeof(void*),1U));});});
# 1513
if(((_tmp297 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_is_bits_only_type(_tmp28A->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp28A->requires_clause == 0)
# 1515
return({struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0U,_tmp52A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp52A;});void*_tmp28E[1U];_tmp28E[0]=& _tmp290;({struct Cyc_Tcenv_Tenv*_tmp703=te;unsigned int _tmp702=loc;void**_tmp701=topt;struct _dyneither_ptr _tmp700=({const char*_tmp28F="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp28F,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp703,_tmp702,_tmp701,_tmp700,_tag_dyneither(_tmp28E,sizeof(void*),1U));});});
return _tmp28A->type;}default: _LL5: _LL6:
# 1518
 return({struct Cyc_String_pa_PrintArg_struct _tmp293=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0U,({
struct _dyneither_ptr _tmp704=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp52B.f1=_tmp704;});_tmp52B;});void*_tmp291[1U];_tmp291[0]=& _tmp293;({struct Cyc_Tcenv_Tenv*_tmp708=te;unsigned int _tmp707=loc;void**_tmp706=topt;struct _dyneither_ptr _tmp705=({const char*_tmp292="expecting struct or union, found %s";_tag_dyneither(_tmp292,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp708,_tmp707,_tmp706,_tmp705,_tag_dyneither(_tmp291,sizeof(void*),1U));});});}_LL0:;};}
# 1524
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1527
({struct Cyc_Tcenv_Tenv*_tmp709=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp709,0,e);});
# 1529
({int _tmp70A=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp70A;});
{void*_tmp298=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp299=_tmp298;void*_tmp2BC;void*_tmp2BB;void*_tmp2BA;void*_tmp2B9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp299)->tag == 3U){_LL1: _tmp2BC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp299)->f1).elt_type;_tmp2BB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp299)->f1).ptr_atts).rgn;_tmp2BA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp299)->f1).ptr_atts).bounds;_tmp2B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp299)->f1).ptr_atts).zero_term;_LL2:
# 1532
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2BA);
{void*_tmp29A=Cyc_Tcutil_compress(_tmp2BC);void*_tmp29B=_tmp29A;enum Cyc_Absyn_AggrKind _tmp2B8;struct Cyc_List_List*_tmp2B7;struct Cyc_Absyn_Aggrdecl*_tmp2B6;struct Cyc_List_List*_tmp2B5;switch(*((int*)_tmp29B)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp2B6=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->f1)->f1).KnownAggr).val;_tmp2B5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->f2;_LL7: {
# 1535
struct Cyc_Absyn_Aggrfield*_tmp29C=Cyc_Absyn_lookup_decl_field(_tmp2B6,f);
if(_tmp29C == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp29F=({struct Cyc_String_pa_PrintArg_struct _tmp52D;_tmp52D.tag=0U,({
struct _dyneither_ptr _tmp70B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B6->name));_tmp52D.f1=_tmp70B;});_tmp52D;});struct Cyc_String_pa_PrintArg_struct _tmp2A0=({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0U,_tmp52C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp52C;});void*_tmp29D[2U];_tmp29D[0]=& _tmp29F,_tmp29D[1]=& _tmp2A0;({struct Cyc_Tcenv_Tenv*_tmp70F=te;unsigned int _tmp70E=loc;void**_tmp70D=topt;struct _dyneither_ptr _tmp70C=({const char*_tmp29E="type %s has no %s member";_tag_dyneither(_tmp29E,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp70F,_tmp70E,_tmp70D,_tmp70C,_tag_dyneither(_tmp29D,sizeof(void*),2U));});});
# 1540
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B6->impl))->tagged)*is_tagged=1;{
void*t3=_tmp29C->type;
if(_tmp2B5 != 0){
struct Cyc_List_List*_tmp2A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2B6->tvs,_tmp2B5);
({void*_tmp710=Cyc_Tcutil_substitute(_tmp2A1,_tmp29C->type);t3=_tmp710;});}{
# 1546
struct Cyc_Absyn_Kind*_tmp2A2=Cyc_Tcutil_type_kind(t3);
# 1549
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2A2) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2A3=Cyc_Tcutil_compress(t3);void*_tmp2A4=_tmp2A3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A4)->tag == 4U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1553
 return({struct Cyc_String_pa_PrintArg_struct _tmp2A7=({struct Cyc_String_pa_PrintArg_struct _tmp52E;_tmp52E.tag=0U,_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp52E;});void*_tmp2A5[1U];_tmp2A5[0]=& _tmp2A7;({struct Cyc_Tcenv_Tenv*_tmp714=te;unsigned int _tmp713=loc;void**_tmp712=topt;struct _dyneither_ptr _tmp711=({const char*_tmp2A6="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2A6,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp714,_tmp713,_tmp712,_tmp711,_tag_dyneither(_tmp2A5,sizeof(void*),1U));});});}_LLC:;}
# 1558
if((((_tmp2B6->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B6->impl))->tagged) && !
Cyc_Tcutil_is_bits_only_type(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp29C->requires_clause == 0)
# 1561
return({struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp52F;_tmp52F.tag=0U,_tmp52F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp52F;});void*_tmp2A8[1U];_tmp2A8[0]=& _tmp2AA;({struct Cyc_Tcenv_Tenv*_tmp718=te;unsigned int _tmp717=loc;void**_tmp716=topt;struct _dyneither_ptr _tmp715=({const char*_tmp2A9="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2A9,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp718,_tmp717,_tmp716,_tmp715,_tag_dyneither(_tmp2A8,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B6->impl))->exist_vars != 0){
if(!({void*_tmp719=t3;Cyc_Tcutil_unify(_tmp719,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2AD=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,_tmp530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp530;});void*_tmp2AB[1U];_tmp2AB[0]=& _tmp2AD;({struct Cyc_Tcenv_Tenv*_tmp71D=te;unsigned int _tmp71C=loc;void**_tmp71B=topt;struct _dyneither_ptr _tmp71A=({const char*_tmp2AC="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2AC,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp71D,_tmp71C,_tmp71B,_tmp71A,_tag_dyneither(_tmp2AB,sizeof(void*),1U));});});}
# 1568
return t3;};};}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp2B8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29B)->f1;_tmp2B7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29B)->f2;_LL9: {
# 1570
struct Cyc_Absyn_Aggrfield*_tmp2AE=Cyc_Absyn_lookup_field(_tmp2B7,f);
if(_tmp2AE == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2B1=({struct Cyc_String_pa_PrintArg_struct _tmp531;_tmp531.tag=0U,_tmp531.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp531;});void*_tmp2AF[1U];_tmp2AF[0]=& _tmp2B1;({struct Cyc_Tcenv_Tenv*_tmp721=te;unsigned int _tmp720=loc;void**_tmp71F=topt;struct _dyneither_ptr _tmp71E=({const char*_tmp2B0="type has no %s field";_tag_dyneither(_tmp2B0,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp721,_tmp720,_tmp71F,_tmp71E,_tag_dyneither(_tmp2AF,sizeof(void*),1U));});});
# 1575
if((_tmp2B8 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_is_bits_only_type(_tmp2AE->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B4=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,_tmp532.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp532;});void*_tmp2B2[1U];_tmp2B2[0]=& _tmp2B4;({struct Cyc_Tcenv_Tenv*_tmp725=te;unsigned int _tmp724=loc;void**_tmp723=topt;struct _dyneither_ptr _tmp722=({const char*_tmp2B3="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B3,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp725,_tmp724,_tmp723,_tmp722,_tag_dyneither(_tmp2B2,sizeof(void*),1U));});});
return _tmp2AE->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1580
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1583
return({struct Cyc_String_pa_PrintArg_struct _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp533;_tmp533.tag=0U,({
struct _dyneither_ptr _tmp726=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp533.f1=_tmp726;});_tmp533;});void*_tmp2BD[1U];_tmp2BD[0]=& _tmp2BF;({struct Cyc_Tcenv_Tenv*_tmp72A=te;unsigned int _tmp729=loc;void**_tmp728=topt;struct _dyneither_ptr _tmp727=({const char*_tmp2BE="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2BE,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp72A,_tmp729,_tmp728,_tmp727,_tag_dyneither(_tmp2BD,sizeof(void*),1U));});});}
# 1588
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1590
struct _tuple15 _tmp2C0=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2C1=_tmp2C0;unsigned int _tmp2CF;int _tmp2CE;_LL1: _tmp2CF=_tmp2C1.f1;_tmp2CE=_tmp2C1.f2;_LL2:;
if(!_tmp2CE)
return({void*_tmp2C2=0U;({struct Cyc_Tcenv_Tenv*_tmp72D=te;unsigned int _tmp72C=loc;struct _dyneither_ptr _tmp72B=({const char*_tmp2C3="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2C3,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp72D,_tmp72C,0,_tmp72B,_tag_dyneither(_tmp2C2,sizeof(void*),0U));});});{
# 1594
struct _handler_cons _tmp2C4;_push_handler(& _tmp2C4);{int _tmp2C6=0;if(setjmp(_tmp2C4.handler))_tmp2C6=1;if(!_tmp2C6){
{void*_tmp2C7=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2CF)).f2;_npop_handler(0U);return _tmp2C7;};_pop_handler();}else{void*_tmp2C5=(void*)_exn_thrown;void*_tmp2C8=_tmp2C5;void*_tmp2CD;if(((struct Cyc_List_Nth_exn_struct*)_tmp2C8)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1597
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2CB=({struct Cyc_Int_pa_PrintArg_struct _tmp535;_tmp535.tag=1U,_tmp535.f1=(unsigned long)((int)_tmp2CF);_tmp535;});struct Cyc_Int_pa_PrintArg_struct _tmp2CC=({struct Cyc_Int_pa_PrintArg_struct _tmp534;_tmp534.tag=1U,({
unsigned long _tmp72E=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp534.f1=_tmp72E;});_tmp534;});void*_tmp2C9[2U];_tmp2C9[0]=& _tmp2CB,_tmp2C9[1]=& _tmp2CC;({struct Cyc_Tcenv_Tenv*_tmp731=te;unsigned int _tmp730=loc;struct _dyneither_ptr _tmp72F=({const char*_tmp2CA="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2CA,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp731,_tmp730,0,_tmp72F,_tag_dyneither(_tmp2C9,sizeof(void*),2U));});});}else{_LL6: _tmp2CD=_tmp2C8;_LL7:(int)_rethrow(_tmp2CD);}_LL3:;}};};}
# 1602
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1604
struct Cyc_Tcenv_Tenv*_tmp2D0=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp732=Cyc_Tcenv_clear_abstract_val_ok(_tmp2D0);Cyc_Tcexp_tcExp(_tmp732,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp733=Cyc_Tcenv_clear_abstract_val_ok(_tmp2D0);Cyc_Tcexp_tcExp(_tmp733,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(_tmp2D0,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D3=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,({
struct _dyneither_ptr _tmp734=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp536.f1=_tmp734;});_tmp536;});void*_tmp2D1[1U];_tmp2D1[0]=& _tmp2D3;({struct Cyc_Tcenv_Tenv*_tmp738=_tmp2D0;unsigned int _tmp737=e2->loc;void**_tmp736=topt;struct _dyneither_ptr _tmp735=({const char*_tmp2D2="expecting int subscript, found %s";_tag_dyneither(_tmp2D2,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp738,_tmp737,_tmp736,_tmp735,_tag_dyneither(_tmp2D1,sizeof(void*),1U));});});{
# 1614
void*_tmp2D4=t1;struct Cyc_List_List*_tmp2F7;void*_tmp2F6;struct Cyc_Absyn_Tqual _tmp2F5;void*_tmp2F4;void*_tmp2F3;void*_tmp2F2;switch(*((int*)_tmp2D4)){case 3U: _LL1: _tmp2F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4)->f1).elt_type;_tmp2F5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4)->f1).elt_tq;_tmp2F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4)->f1).ptr_atts).rgn;_tmp2F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4)->f1).ptr_atts).bounds;_tmp2F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4)->f1).ptr_atts).zero_term;_LL2:
# 1618
 if(Cyc_Tcutil_force_type2bool(0,_tmp2F2)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp2D5=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp2F3);
if(_tmp2D5 != 0){
struct Cyc_Absyn_Exp*_tmp2D6=_tmp2D5;
struct _tuple15 _tmp2D7=Cyc_Evexp_eval_const_uint_exp(_tmp2D6);struct _tuple15 _tmp2D8=_tmp2D7;unsigned int _tmp2E4;int _tmp2E3;_LL8: _tmp2E4=_tmp2D8.f1;_tmp2E3=_tmp2D8.f2;_LL9:;
if(_tmp2E3  && _tmp2E4 == 1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2D9=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2DA=_tmp2D9;unsigned int _tmp2E2;int _tmp2E1;_LLB: _tmp2E2=_tmp2DA.f1;_tmp2E1=_tmp2DA.f2;_LLC:;
if(_tmp2E1){
struct _tuple15 _tmp2DB=Cyc_Evexp_eval_const_uint_exp(_tmp2D6);struct _tuple15 _tmp2DC=_tmp2DB;unsigned int _tmp2E0;int _tmp2DF;_LLE: _tmp2E0=_tmp2DC.f1;_tmp2DF=_tmp2DC.f2;_LLF:;
if((_tmp2E3  && _tmp2E0 > _tmp2E4) && _tmp2E4 != 1)
({void*_tmp2DD=0U;({unsigned int _tmp73A=loc;struct _dyneither_ptr _tmp739=({const char*_tmp2DE="subscript will fail at run-time";_tag_dyneither(_tmp2DE,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp73A,_tmp739,_tag_dyneither(_tmp2DD,sizeof(void*),0U));});});}}}
# 1634
if(emit_warning)
({void*_tmp2E5=0U;({unsigned int _tmp73C=e2->loc;struct _dyneither_ptr _tmp73B=({const char*_tmp2E6="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp2E6,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp73C,_tmp73B,_tag_dyneither(_tmp2E5,sizeof(void*),0U));});});}else{
# 1638
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2E7=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2E8=_tmp2E7;unsigned int _tmp2EA;int _tmp2E9;_LL11: _tmp2EA=_tmp2E8.f1;_tmp2E9=_tmp2E8.f2;_LL12:;
if(_tmp2E9)
# 1643
({unsigned int _tmp73F=loc;unsigned int _tmp73E=_tmp2EA;void*_tmp73D=_tmp2F3;Cyc_Tcutil_check_bound(_tmp73F,_tmp73E,_tmp73D,Cyc_Tcenv_abstract_val_ok(_tmp2D0));});}else{
# 1646
if(Cyc_Tcutil_is_bound_one(_tmp2F3) && !Cyc_Tcutil_force_type2bool(0,_tmp2F2))
({void*_tmp2EB=0U;({unsigned int _tmp741=e1->loc;struct _dyneither_ptr _tmp740=({const char*_tmp2EC="subscript applied to pointer to one object";_tag_dyneither(_tmp2EC,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp741,_tmp740,_tag_dyneither(_tmp2EB,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2F3);}}
# 1651
Cyc_Tcenv_check_rgn_accessible(_tmp2D0,loc,_tmp2F4);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmp2F6),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2D0))
({void*_tmp2ED=0U;({unsigned int _tmp743=e1->loc;struct _dyneither_ptr _tmp742=({const char*_tmp2EE="can't subscript an abstract pointer";_tag_dyneither(_tmp2EE,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp743,_tmp742,_tag_dyneither(_tmp2ED,sizeof(void*),0U));});});
return _tmp2F6;case 6U: _LL3: _tmp2F7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D4)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2D0,loc,_tmp2F7,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp2F1=({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0U,({struct _dyneither_ptr _tmp744=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp537.f1=_tmp744;});_tmp537;});void*_tmp2EF[1U];_tmp2EF[0]=& _tmp2F1;({struct Cyc_Tcenv_Tenv*_tmp748=_tmp2D0;unsigned int _tmp747=loc;void**_tmp746=topt;struct _dyneither_ptr _tmp745=({const char*_tmp2F0="subscript applied to %s";_tag_dyneither(_tmp2F0,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp748,_tmp747,_tmp746,_tmp745,_tag_dyneither(_tmp2EF,sizeof(void*),1U));});});}_LL0:;};};}
# 1661
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp2F8=Cyc_Tcutil_compress(*topt);void*_tmp2F9=_tmp2F8;struct Cyc_List_List*_tmp2FD;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F9)->tag == 6U){_LL1: _tmp2FD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F9)->f1;_LL2:
# 1667
 if(({int _tmp749=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FD);_tmp749 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1670
goto _LL0;
# 1672
for(0;es != 0;(es=es->tl,_tmp2FD=_tmp2FD->tl)){
int bogus=0;
void*_tmp2FA=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp2FD))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp74A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp74A,& _tmp2FA,(struct Cyc_Absyn_Exp*)es->hd);});
# 1677
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp2FD->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp74C=({struct Cyc_List_List*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));({struct _tuple17*_tmp74B=({struct _tuple17*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->f1=(*((struct _tuple17*)_tmp2FD->hd)).f1,_tmp2FB->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2FB;});_tmp2FC->hd=_tmp74B;}),_tmp2FC->tl=fields;_tmp2FC;});fields=_tmp74C;});}
# 1680
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1684
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp74D=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp74D,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp750=({struct Cyc_List_List*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));({struct _tuple17*_tmp74F=({struct _tuple17*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));({struct Cyc_Absyn_Tqual _tmp74E=Cyc_Absyn_empty_tqual(0U);_tmp2FE->f1=_tmp74E;}),_tmp2FE->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2FE;});_tmp2FF->hd=_tmp74F;}),_tmp2FF->tl=fields;_tmp2FF;});fields=_tmp750;});}
# 1689
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->tag=6U,({struct Cyc_List_List*_tmp751=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp300->f1=_tmp751;});_tmp300;});}
# 1693
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1696
return({void*_tmp301=0U;({struct Cyc_Tcenv_Tenv*_tmp755=te;unsigned int _tmp754=loc;void**_tmp753=topt;struct _dyneither_ptr _tmp752=({const char*_tmp302="tcCompoundLit";_tag_dyneither(_tmp302,sizeof(char),14U);});Cyc_Tcexp_expr_err(_tmp755,_tmp754,_tmp753,_tmp752,_tag_dyneither(_tmp301,sizeof(void*),0U));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1706 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1709
void*res_t2;
int _tmp303=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp304=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp31A=_cycalloc(sizeof(*_tmp31A));_tmp31A->tag=0U,({union Cyc_Absyn_Cnst _tmp756=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp303);_tmp31A->f1=_tmp756;});_tmp31A;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp304,loc);
# 1717
if(zero_term){
struct Cyc_Absyn_Exp*_tmp305=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp303 - 1);
if(!Cyc_Tcutil_is_zero(_tmp305))
({void*_tmp306=0U;({unsigned int _tmp758=_tmp305->loc;struct _dyneither_ptr _tmp757=({const char*_tmp307="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp307,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp758,_tmp757,_tag_dyneither(_tmp306,sizeof(void*),0U));});});}
# 1722
sz_exp->topt=Cyc_Absyn_uint_type;
({void*_tmp75C=({void*_tmp75B=res;struct Cyc_Absyn_Tqual _tmp75A=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1723
struct Cyc_Absyn_Exp*_tmp759=sz_exp;Cyc_Absyn_array_type(_tmp75B,_tmp75A,_tmp759,
# 1725
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1723
res_t2=_tmp75C;});
# 1727
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1730
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1732
({struct Cyc_String_pa_PrintArg_struct _tmp30A=({struct Cyc_String_pa_PrintArg_struct _tmp538;_tmp538.tag=0U,({
struct _dyneither_ptr _tmp75D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));_tmp538.f1=_tmp75D;});_tmp538;});void*_tmp308[1U];_tmp308[0]=& _tmp30A;({unsigned int _tmp75F=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp75E=({const char*_tmp309="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp309,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp75F,_tmp75E,_tag_dyneither(_tmp308,sizeof(void*),1U));});});
# 1735
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1740
void*_tmp30B=(void*)ds->hd;void*_tmp30C=_tmp30B;struct Cyc_Absyn_Exp*_tmp319;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp30C)->tag == 1U){_LL1: _LL2:
# 1742
({void*_tmp30D=0U;({unsigned int _tmp761=loc;struct _dyneither_ptr _tmp760=({const char*_tmp30E="only array index designators are supported";_tag_dyneither(_tmp30E,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp761,_tmp760,_tag_dyneither(_tmp30D,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp319=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp30C)->f1;_LL4:
# 1745
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp319);{
struct _tuple15 _tmp30F=Cyc_Evexp_eval_const_uint_exp(_tmp319);struct _tuple15 _tmp310=_tmp30F;unsigned int _tmp318;int _tmp317;_LL6: _tmp318=_tmp310.f1;_tmp317=_tmp310.f2;_LL7:;
if(!_tmp317)
({void*_tmp311=0U;({unsigned int _tmp763=_tmp319->loc;struct _dyneither_ptr _tmp762=({const char*_tmp312="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp312,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp763,_tmp762,_tag_dyneither(_tmp311,sizeof(void*),0U));});});else{
if(_tmp318 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp315=({struct Cyc_Int_pa_PrintArg_struct _tmp53A;_tmp53A.tag=1U,_tmp53A.f1=(unsigned long)offset;_tmp53A;});struct Cyc_Int_pa_PrintArg_struct _tmp316=({struct Cyc_Int_pa_PrintArg_struct _tmp539;_tmp539.tag=1U,_tmp539.f1=(unsigned long)((int)_tmp318);_tmp539;});void*_tmp313[2U];_tmp313[0]=& _tmp315,_tmp313[1]=& _tmp316;({unsigned int _tmp765=_tmp319->loc;struct _dyneither_ptr _tmp764=({const char*_tmp314="expecting index designator %d but found %d";_tag_dyneither(_tmp314,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp765,_tmp764,_tag_dyneither(_tmp313,sizeof(void*),2U));});});}
# 1752
goto _LL0;};}_LL0:;}}}
# 1756
return res_t2;}
# 1760
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1763
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp31B=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp31C=_tmp31B;void*_tmp321;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31C)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31C)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31C)->f2 != 0){_LL1: _tmp321=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31C)->f2)->hd;_LL2:
# 1768
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp31D=({void*_tmp766=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp766,Cyc_Absyn_valueof_exp(_tmp321,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp31D->topt=bound->topt;
# 1773
bound=_tmp31D;}
# 1775
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1777
 if(!Cyc_Tcutil_coerce_uint_type(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp320=({struct Cyc_String_pa_PrintArg_struct _tmp53B;_tmp53B.tag=0U,({
struct _dyneither_ptr _tmp767=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp53B.f1=_tmp767;});_tmp53B;});void*_tmp31E[1U];_tmp31E[0]=& _tmp320;({unsigned int _tmp769=bound->loc;struct _dyneither_ptr _tmp768=({const char*_tmp31F="expecting unsigned int, found %s";_tag_dyneither(_tmp31F,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp769,_tmp768,_tag_dyneither(_tmp31E,sizeof(void*),1U));});});}_LL0:;}
# 1782
if(!(vd->tq).real_const)
({void*_tmp322=0U;({struct _dyneither_ptr _tmp76A=({const char*_tmp323="comprehension index variable is not declared const!";_tag_dyneither(_tmp323,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp76A,_tag_dyneither(_tmp322,sizeof(void*),0U));});});
# 1785
if(te->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp76B=Cyc_Tcenv_new_block(loc,te);te=_tmp76B;});{
void**_tmp324=0;
struct Cyc_Absyn_Tqual*_tmp325=0;
void**_tmp326=0;
# 1791
if(topt != 0){
void*_tmp327=Cyc_Tcutil_compress(*topt);void*_tmp328=_tmp327;void*_tmp333;struct Cyc_Absyn_Tqual _tmp332;struct Cyc_Absyn_Exp*_tmp331;void*_tmp330;struct Cyc_Absyn_PtrInfo _tmp32F;switch(*((int*)_tmp328)){case 3U: _LL6: _tmp32F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328)->f1;_LL7:
# 1794
({void**_tmp76C=({void**_tmp329=_cycalloc(sizeof(*_tmp329));*_tmp329=_tmp32F.elt_type;_tmp329;});_tmp324=_tmp76C;});
({struct Cyc_Absyn_Tqual*_tmp76D=({struct Cyc_Absyn_Tqual*_tmp32A=_cycalloc(sizeof(*_tmp32A));*_tmp32A=_tmp32F.elt_tq;_tmp32A;});_tmp325=_tmp76D;});
({void**_tmp76E=({void**_tmp32B=_cycalloc(sizeof(*_tmp32B));*_tmp32B=(_tmp32F.ptr_atts).zero_term;_tmp32B;});_tmp326=_tmp76E;});
goto _LL5;case 4U: _LL8: _tmp333=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp328)->f1).elt_type;_tmp332=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp328)->f1).tq;_tmp331=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp328)->f1).num_elts;_tmp330=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp328)->f1).zero_term;_LL9:
# 1799
({void**_tmp76F=({void**_tmp32C=_cycalloc(sizeof(*_tmp32C));*_tmp32C=_tmp333;_tmp32C;});_tmp324=_tmp76F;});
({struct Cyc_Absyn_Tqual*_tmp770=({struct Cyc_Absyn_Tqual*_tmp32D=_cycalloc(sizeof(*_tmp32D));*_tmp32D=_tmp332;_tmp32D;});_tmp325=_tmp770;});
({void**_tmp771=({void**_tmp32E=_cycalloc(sizeof(*_tmp32E));*_tmp32E=_tmp330;_tmp32E;});_tmp326=_tmp771;});
goto _LL5;default: _LLA: _LLB:
# 1804
 goto _LL5;}_LL5:;}{
# 1807
void*t=Cyc_Tcexp_tcExp(te,_tmp324,body);
# 1809
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp334=0U;({unsigned int _tmp773=body->loc;struct _dyneither_ptr _tmp772=({const char*_tmp335="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp335,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp773,_tmp772,_tag_dyneither(_tmp334,sizeof(void*),0U));});});
if(te->le == 0){
# 1813
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp336=0U;({unsigned int _tmp775=bound->loc;struct _dyneither_ptr _tmp774=({const char*_tmp337="bound is not constant";_tag_dyneither(_tmp337,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp775,_tmp774,_tag_dyneither(_tmp336,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp338=0U;({unsigned int _tmp777=bound->loc;struct _dyneither_ptr _tmp776=({const char*_tmp339="body is not constant";_tag_dyneither(_tmp339,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp777,_tmp776,_tag_dyneither(_tmp338,sizeof(void*),0U));});});}
# 1818
if(_tmp326 != 0  && Cyc_Tcutil_force_type2bool(0,*_tmp326)){
# 1821
struct Cyc_Absyn_Exp*_tmp33A=Cyc_Absyn_uint_exp(1U,0U);_tmp33A->topt=Cyc_Absyn_uint_type;
({struct Cyc_Absyn_Exp*_tmp778=Cyc_Absyn_add_exp(bound,_tmp33A,0U);bound=_tmp778;});bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1825
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp33B=0U;({unsigned int _tmp77A=body->loc;struct _dyneither_ptr _tmp779=({const char*_tmp33C="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp33C,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_dyneither(_tmp33B,sizeof(void*),0U));});});{
# 1829
void*_tmp33D=({void*_tmp77D=t;struct Cyc_Absyn_Tqual _tmp77C=_tmp325 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp325;struct Cyc_Absyn_Exp*_tmp77B=bound;Cyc_Absyn_array_type(_tmp77D,_tmp77C,_tmp77B,
_tmp326 == 0?Cyc_Absyn_false_type:*_tmp326,0U);});
return _tmp33D;};};};}
# 1835
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1838
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp33E=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp33F=_tmp33E;void*_tmp344;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f2 != 0){_LL1: _tmp344=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f2)->hd;_LL2:
# 1843
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp340=({void*_tmp77E=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp77E,Cyc_Absyn_valueof_exp(_tmp344,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp340->topt=bound->topt;
# 1848
bound=_tmp340;}
# 1850
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1852
 if(!Cyc_Tcutil_coerce_uint_type(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp343=({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0U,({
struct _dyneither_ptr _tmp77F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp53C.f1=_tmp77F;});_tmp53C;});void*_tmp341[1U];_tmp341[0]=& _tmp343;({unsigned int _tmp781=bound->loc;struct _dyneither_ptr _tmp780=({const char*_tmp342="expecting unsigned int, found %s";_tag_dyneither(_tmp342,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp781,_tmp780,_tag_dyneither(_tmp341,sizeof(void*),1U));});});}_LL0:;}{
# 1857
void**_tmp345=0;
struct Cyc_Absyn_Tqual*_tmp346=0;
void**_tmp347=0;
# 1861
if(topt != 0){
void*_tmp348=Cyc_Tcutil_compress(*topt);void*_tmp349=_tmp348;void*_tmp354;struct Cyc_Absyn_Tqual _tmp353;struct Cyc_Absyn_Exp*_tmp352;void*_tmp351;struct Cyc_Absyn_PtrInfo _tmp350;switch(*((int*)_tmp349)){case 3U: _LL6: _tmp350=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp349)->f1;_LL7:
# 1864
({void**_tmp782=({void**_tmp34A=_cycalloc(sizeof(*_tmp34A));*_tmp34A=_tmp350.elt_type;_tmp34A;});_tmp345=_tmp782;});
({struct Cyc_Absyn_Tqual*_tmp783=({struct Cyc_Absyn_Tqual*_tmp34B=_cycalloc(sizeof(*_tmp34B));*_tmp34B=_tmp350.elt_tq;_tmp34B;});_tmp346=_tmp783;});
({void**_tmp784=({void**_tmp34C=_cycalloc(sizeof(*_tmp34C));*_tmp34C=(_tmp350.ptr_atts).zero_term;_tmp34C;});_tmp347=_tmp784;});
goto _LL5;case 4U: _LL8: _tmp354=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp349)->f1).elt_type;_tmp353=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp349)->f1).tq;_tmp352=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp349)->f1).num_elts;_tmp351=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp349)->f1).zero_term;_LL9:
# 1869
({void**_tmp785=({void**_tmp34D=_cycalloc(sizeof(*_tmp34D));*_tmp34D=_tmp354;_tmp34D;});_tmp345=_tmp785;});
({struct Cyc_Absyn_Tqual*_tmp786=({struct Cyc_Absyn_Tqual*_tmp34E=_cycalloc(sizeof(*_tmp34E));*_tmp34E=_tmp353;_tmp34E;});_tmp346=_tmp786;});
({void**_tmp787=({void**_tmp34F=_cycalloc(sizeof(*_tmp34F));*_tmp34F=_tmp351;_tmp34F;});_tmp347=_tmp787;});
goto _LL5;default: _LLA: _LLB:
# 1874
 goto _LL5;}_LL5:;}
# 1877
({unsigned int _tmp78A=loc;struct Cyc_Tcenv_Tenv*_tmp789=te;struct Cyc_List_List*_tmp788=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp78A,_tmp789,_tmp788,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp345 != 0)Cyc_Tcutil_unify(*_tmp345,t);
# 1880
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp355=0U;({unsigned int _tmp78C=bound->loc;struct _dyneither_ptr _tmp78B=({const char*_tmp356="bound is not constant";_tag_dyneither(_tmp356,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp78C,_tmp78B,_tag_dyneither(_tmp355,sizeof(void*),0U));});});}
# 1884
if(_tmp347 != 0  && Cyc_Tcutil_force_type2bool(0,*_tmp347)){
# 1887
struct Cyc_Absyn_Exp*_tmp357=Cyc_Absyn_uint_exp(1U,0U);_tmp357->topt=Cyc_Absyn_uint_type;
({struct Cyc_Absyn_Exp*_tmp78D=Cyc_Absyn_add_exp(bound,_tmp357,0U);bound=_tmp78D;});bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1891
({void*_tmp358=0U;({unsigned int _tmp78F=loc;struct _dyneither_ptr _tmp78E=({const char*_tmp359="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp359,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp78F,_tmp78E,_tag_dyneither(_tmp358,sizeof(void*),0U));});});}{
# 1894
void*_tmp35A=({void*_tmp792=t;struct Cyc_Absyn_Tqual _tmp791=_tmp346 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp346;struct Cyc_Absyn_Exp*_tmp790=bound;Cyc_Absyn_array_type(_tmp792,_tmp791,_tmp790,
_tmp347 == 0?Cyc_Absyn_false_type:*_tmp347,0U);});
return _tmp35A;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1909 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1913
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp793=({struct Cyc_Absyn_Aggrdecl**_tmp35B=_cycalloc(sizeof(*_tmp35B));*_tmp35B=ad;_tmp35B;});adptr=_tmp793;});}else{
# 1919
{struct _handler_cons _tmp35C;_push_handler(& _tmp35C);{int _tmp35E=0;if(setjmp(_tmp35C.handler))_tmp35E=1;if(!_tmp35E){({struct Cyc_Absyn_Aggrdecl**_tmp794=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp794;});
ad=*adptr;
# 1919
;_pop_handler();}else{void*_tmp35D=(void*)_exn_thrown;void*_tmp35F=_tmp35D;void*_tmp363;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp35F)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1922
({struct Cyc_String_pa_PrintArg_struct _tmp362=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,({struct _dyneither_ptr _tmp795=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp53D.f1=_tmp795;});_tmp53D;});void*_tmp360[1U];_tmp360[0]=& _tmp362;({unsigned int _tmp797=loc;struct _dyneither_ptr _tmp796=({const char*_tmp361="unbound struct/union name %s";_tag_dyneither(_tmp361,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp797,_tmp796,_tag_dyneither(_tmp360,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp363=_tmp35F;_LL4:(int)_rethrow(_tmp363);}_LL0:;}};}
# 1925
*ad_opt=ad;
*tn=ad->name;}
# 1928
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp366=({struct Cyc_String_pa_PrintArg_struct _tmp53E;_tmp53E.tag=0U,({struct _dyneither_ptr _tmp798=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp367="struct";_tag_dyneither(_tmp367,sizeof(char),7U);}):({const char*_tmp368="union";_tag_dyneither(_tmp368,sizeof(char),6U);}));_tmp53E.f1=_tmp798;});_tmp53E;});void*_tmp364[1U];_tmp364[0]=& _tmp366;({unsigned int _tmp79A=loc;struct _dyneither_ptr _tmp799=({const char*_tmp365="can't construct abstract %s";_tag_dyneither(_tmp365,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp79A,_tmp799,_tag_dyneither(_tmp364,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1936
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp369=Cyc_Tcenv_new_status(te);
if(_tmp369 == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp79B=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp79B;});else{
# 1945
({struct Cyc_Tcenv_Tenv*_tmp79C=Cyc_Tcenv_set_new_status(_tmp369,te);te2=_tmp79C;});}{
# 1947
struct _tuple12 _tmp36A=({struct _tuple12 _tmp544;({struct Cyc_List_List*_tmp79D=Cyc_Tcenv_lookup_type_vars(te2);_tmp544.f1=_tmp79D;}),_tmp544.f2=Cyc_Core_heap_region;_tmp544;});
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp36A,ad->tvs);
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp36A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp36D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp36B);
struct Cyc_List_List*_tmp36E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp36C);
struct Cyc_List_List*_tmp36F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36B,_tmp36C);
void*res_typ;
# 1958
if(topt != 0){
void*_tmp370=Cyc_Tcutil_compress(*topt);void*_tmp371=_tmp370;struct Cyc_Absyn_Aggrdecl**_tmp374;struct Cyc_List_List*_tmp373;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp371)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp371)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp371)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp374=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp371)->f1)->f1).KnownAggr).val;_tmp373=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp371)->f2;_LL7:
# 1961
 if(*_tmp374 == *adptr){
{struct Cyc_List_List*_tmp372=_tmp36D;for(0;_tmp372 != 0  && _tmp373 != 0;(
_tmp372=_tmp372->tl,_tmp373=_tmp373->tl)){
Cyc_Tcutil_unify((void*)_tmp372->hd,(void*)_tmp373->hd);}}
# 1966
res_typ=*topt;
goto _LL5;}
# 1969
goto _LL9;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1971
({void*_tmp79F=({union Cyc_Absyn_AggrInfo _tmp79E=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp79E,_tmp36D);});res_typ=_tmp79F;});}_LL5:;}else{
# 1974
({void*_tmp7A1=({union Cyc_Absyn_AggrInfo _tmp7A0=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp7A0,_tmp36D);});res_typ=_tmp7A1;});}{
# 1977
struct Cyc_List_List*_tmp375=*ts;
struct Cyc_List_List*_tmp376=_tmp36E;
while(_tmp375 != 0  && _tmp376 != 0){
# 1981
({unsigned int _tmp7A4=loc;struct Cyc_Tcenv_Tenv*_tmp7A3=te2;struct Cyc_List_List*_tmp7A2=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp7A4,_tmp7A3,_tmp7A2,& Cyc_Tcutil_ak,1,0,(void*)_tmp375->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp375->hd);
Cyc_Tcutil_unify((void*)_tmp375->hd,(void*)_tmp376->hd);
_tmp375=_tmp375->tl;
_tmp376=_tmp376->tl;}
# 1987
if(_tmp375 != 0)
({void*_tmp377=0U;({unsigned int _tmp7A6=loc;struct _dyneither_ptr _tmp7A5=({const char*_tmp378="too many explicit witness types";_tag_dyneither(_tmp378,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7A6,_tmp7A5,_tag_dyneither(_tmp377,sizeof(void*),0U));});});
# 1990
*ts=_tmp36E;{
# 1993
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp379=(struct _tuple19*)fields->hd;struct _tuple19*_tmp37A=_tmp379;struct Cyc_Absyn_Aggrfield*_tmp386;struct Cyc_Absyn_Exp*_tmp385;_LLB: _tmp386=_tmp37A->f1;_tmp385=_tmp37A->f2;_LLC:;{
void*_tmp37B=Cyc_Tcutil_substitute(_tmp36F,_tmp386->type);
({struct Cyc_Tcenv_Tenv*_tmp7A7=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7A7,& _tmp37B,_tmp385);});
# 2004
if(!Cyc_Tcutil_coerce_arg(te2,_tmp385,_tmp37B,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,_tmp543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp386->name);_tmp543;});struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,({
struct _dyneither_ptr _tmp7A8=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp383="struct";_tag_dyneither(_tmp383,sizeof(char),7U);}):({const char*_tmp384="union";_tag_dyneither(_tmp384,sizeof(char),6U);}));_tmp542.f1=_tmp7A8;});_tmp542;});struct Cyc_String_pa_PrintArg_struct _tmp380=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,({
struct _dyneither_ptr _tmp7A9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp541.f1=_tmp7A9;});_tmp541;});struct Cyc_String_pa_PrintArg_struct _tmp381=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,({struct _dyneither_ptr _tmp7AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp37B));_tmp540.f1=_tmp7AA;});_tmp540;});struct Cyc_String_pa_PrintArg_struct _tmp382=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,({
struct _dyneither_ptr _tmp7AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp385->topt)));_tmp53F.f1=_tmp7AB;});_tmp53F;});void*_tmp37C[5U];_tmp37C[0]=& _tmp37E,_tmp37C[1]=& _tmp37F,_tmp37C[2]=& _tmp380,_tmp37C[3]=& _tmp381,_tmp37C[4]=& _tmp382;({unsigned int _tmp7AD=_tmp385->loc;struct _dyneither_ptr _tmp7AC=({const char*_tmp37D="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp37D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp7AD,_tmp7AC,_tag_dyneither(_tmp37C,sizeof(void*),5U));});});
Cyc_Tcutil_explain_failure();}};}{
# 2013
struct Cyc_List_List*_tmp387=0;
{struct Cyc_List_List*_tmp388=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp388 != 0;_tmp388=_tmp388->tl){
({struct Cyc_List_List*_tmp7B1=({struct Cyc_List_List*_tmp38A=_cycalloc(sizeof(*_tmp38A));({struct _tuple0*_tmp7B0=({struct _tuple0*_tmp389=_cycalloc(sizeof(*_tmp389));({void*_tmp7AF=Cyc_Tcutil_substitute(_tmp36F,(*((struct _tuple0*)_tmp388->hd)).f1);_tmp389->f1=_tmp7AF;}),({
void*_tmp7AE=Cyc_Tcutil_substitute(_tmp36F,(*((struct _tuple0*)_tmp388->hd)).f2);_tmp389->f2=_tmp7AE;});_tmp389;});
# 2015
_tmp38A->hd=_tmp7B0;}),_tmp38A->tl=_tmp387;_tmp38A;});_tmp387=_tmp7B1;});}}
# 2018
({struct Cyc_List_List*_tmp7B2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp387);_tmp387=_tmp7B2;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp387);
return res_typ;};};};};};}
# 2024
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2026
{void*_tmp38B=Cyc_Tcutil_compress(ts);void*_tmp38C=_tmp38B;enum Cyc_Absyn_AggrKind _tmp39B;struct Cyc_List_List*_tmp39A;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C)->tag == 7U){_LL1: _tmp39B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C)->f1;_tmp39A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C)->f2;_LL2:
# 2028
 if(_tmp39B == Cyc_Absyn_UnionA)
({void*_tmp38D=0U;({unsigned int _tmp7B4=loc;struct _dyneither_ptr _tmp7B3=({const char*_tmp38E="expecting struct but found union";_tag_dyneither(_tmp38E,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7B4,_tmp7B3,_tag_dyneither(_tmp38D,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp39A);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp38F=(struct _tuple19*)fields->hd;struct _tuple19*_tmp390=_tmp38F;struct Cyc_Absyn_Aggrfield*_tmp397;struct Cyc_Absyn_Exp*_tmp396;_LL6: _tmp397=_tmp390->f1;_tmp396=_tmp390->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp7B6=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp7B5=& _tmp397->type;Cyc_Tcexp_tcExpInitializer(_tmp7B6,_tmp7B5,_tmp396);});
# 2037
if(!Cyc_Tcutil_coerce_arg(te,_tmp396,_tmp397->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0U,_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp397->name);_tmp547;});struct Cyc_String_pa_PrintArg_struct _tmp394=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,({
struct _dyneither_ptr _tmp7B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp397->type));_tmp546.f1=_tmp7B7;});_tmp546;});struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,({
struct _dyneither_ptr _tmp7B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp396->topt)));_tmp545.f1=_tmp7B8;});_tmp545;});void*_tmp391[3U];_tmp391[0]=& _tmp393,_tmp391[1]=& _tmp394,_tmp391[2]=& _tmp395;({unsigned int _tmp7BA=_tmp396->loc;struct _dyneither_ptr _tmp7B9=({const char*_tmp392="field %s of struct expects type %s != %s";_tag_dyneither(_tmp392,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp7BA,_tmp7B9,_tag_dyneither(_tmp391,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2044
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp398=0U;({struct _dyneither_ptr _tmp7BB=({const char*_tmp399="tcAnonStruct: wrong type";_tag_dyneither(_tmp399,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7BB,_tag_dyneither(_tmp398,sizeof(void*),0U));});});}_LL0:;}
# 2047
return ts;}
# 2051
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2054
struct _tuple12 _tmp39C=({struct _tuple12 _tmp54D;({struct Cyc_List_List*_tmp7BC=Cyc_Tcenv_lookup_type_vars(te);_tmp54D.f1=_tmp7BC;}),_tmp54D.f2=Cyc_Core_heap_region;_tmp54D;});
struct Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp39C,tud->tvs);
struct Cyc_List_List*_tmp39E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp39D);
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp7BD=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp7BD,_tmp39E);});
# 2059
if(topt != 0){
void*_tmp39F=Cyc_Tcutil_compress(*topt);void*_tmp3A0=_tmp39F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A0)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A0)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Tcutil_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2065
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp39D != 0)({void*_tmp7BE=Cyc_Tcutil_substitute(_tmp39D,t);t=_tmp7BE;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3A3=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,({
# 2075
struct _dyneither_ptr _tmp7BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp54A.f1=_tmp7BF;});_tmp54A;});struct Cyc_String_pa_PrintArg_struct _tmp3A4=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,({struct _dyneither_ptr _tmp7C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp549.f1=_tmp7C0;});_tmp549;});struct Cyc_String_pa_PrintArg_struct _tmp3A5=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,({
struct _dyneither_ptr _tmp7C1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3A6="?";_tag_dyneither(_tmp3A6,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp548.f1=_tmp7C1;});_tmp548;});void*_tmp3A1[3U];_tmp3A1[0]=& _tmp3A3,_tmp3A1[1]=& _tmp3A4,_tmp3A1[2]=& _tmp3A5;({unsigned int _tmp7C3=e->loc;struct _dyneither_ptr _tmp7C2=({const char*_tmp3A2="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3A2,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp7C3,_tmp7C2,_tag_dyneither(_tmp3A1,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2080
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3A9=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,({
# 2083
struct _dyneither_ptr _tmp7C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp54B.f1=_tmp7C4;});_tmp54B;});void*_tmp3A7[1U];_tmp3A7[0]=& _tmp3A9;({struct Cyc_Tcenv_Tenv*_tmp7C8=te;unsigned int _tmp7C7=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7C6=topt;struct _dyneither_ptr _tmp7C5=({const char*_tmp3A8="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3A8,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp7C8,_tmp7C7,_tmp7C6,_tmp7C5,_tag_dyneither(_tmp3A7,sizeof(void*),1U));});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3AC=({struct Cyc_String_pa_PrintArg_struct _tmp54C;_tmp54C.tag=0U,({
struct _dyneither_ptr _tmp7C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp54C.f1=_tmp7C9;});_tmp54C;});void*_tmp3AA[1U];_tmp3AA[0]=& _tmp3AC;({struct Cyc_Tcenv_Tenv*_tmp7CD=te;unsigned int _tmp7CC=loc;void**_tmp7CB=topt;struct _dyneither_ptr _tmp7CA=({const char*_tmp3AB="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3AB,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7CD,_tmp7CC,_tmp7CB,_tmp7CA,_tag_dyneither(_tmp3AA,sizeof(void*),1U));});});
return res;};}
# 2090
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2092
if(Cyc_Tcutil_is_bits_only_type(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2094
if(topt != 0){
void*_tmp3AD=Cyc_Tcutil_compress(*topt);void*_tmp3AE=_tmp3AD;void*_tmp3AF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AE)->tag == 3U){_LL1: _tmp3AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AE)->f1).elt_type;_LL2:
# 2097
 Cyc_Tcutil_unify(_tmp3AF,t);
if(Cyc_Tcutil_is_bits_only_type(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2103
({struct Cyc_String_pa_PrintArg_struct _tmp3B2=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({
struct _dyneither_ptr _tmp7CE=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3B4="calloc";_tag_dyneither(_tmp3B4,sizeof(char),7U);}):(struct _dyneither_ptr)({const char*_tmp3B5="malloc";_tag_dyneither(_tmp3B5,sizeof(char),7U);})));_tmp54F.f1=_tmp7CE;});_tmp54F;});struct Cyc_String_pa_PrintArg_struct _tmp3B3=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,({struct _dyneither_ptr _tmp7CF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp54E.f1=_tmp7CF;});_tmp54E;});void*_tmp3B0[2U];_tmp3B0[0]=& _tmp3B2,_tmp3B0[1]=& _tmp3B3;({unsigned int _tmp7D1=loc;struct _dyneither_ptr _tmp7D0=({const char*_tmp3B1="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3B1,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp7D1,_tmp7D0,_tag_dyneither(_tmp3B0,sizeof(void*),2U));});});}
# 2107
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2109
enum Cyc_Absyn_AliasQual _tmp3B6=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3B7=_tmp3B6;if(_tmp3B7 == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 2115
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2120
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 2124
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3B8=Cyc_Tcutil_compress(handle_type);void*_tmp3B9=_tmp3B8;void*_tmp3BD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B9)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B9)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B9)->f2 != 0){_LL1: _tmp3BD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B9)->f2)->hd;_LL2:
# 2129
 rgn=_tmp3BD;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2133
({struct Cyc_String_pa_PrintArg_struct _tmp3BC=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({
struct _dyneither_ptr _tmp7D2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp550.f1=_tmp7D2;});_tmp550;});void*_tmp3BA[1U];_tmp3BA[0]=& _tmp3BC;({unsigned int _tmp7D4=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7D3=({const char*_tmp3BB="expecting region_t type but found %s";_tag_dyneither(_tmp3BB,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7D4,_tmp7D3,_tag_dyneither(_tmp3BA,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2140
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7D5=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7D5;});
if(rgn == Cyc_Absyn_unique_rgn_type)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2147
({struct Cyc_Tcenv_Tenv*_tmp7D6=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7D6,& Cyc_Absyn_uint_type,*e);});{
# 2156 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3BE=0U;({struct _dyneither_ptr _tmp7D7=({const char*_tmp3BF="calloc with empty type";_tag_dyneither(_tmp3BF,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D7,_tag_dyneither(_tmp3BE,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7DA=loc;struct Cyc_Tcenv_Tenv*_tmp7D9=te;struct Cyc_List_List*_tmp7D8=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7DA,_tmp7D9,_tmp7D8,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2168
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3C0=er;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D4;void*_tmp3D3;switch(*((int*)_tmp3C0)){case 17U: _LL6: _tmp3D3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1;_LL7:
# 2172
 elt_type=_tmp3D3;
({void**_tmp7DB=({void**_tmp3C1=_cycalloc(sizeof(*_tmp3C1));*_tmp3C1=elt_type;_tmp3C1;});*t=_tmp7DB;});
({struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Absyn_uint_exp(1U,0U);num_elts=_tmp7DC;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C0)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->tl)->tl == 0){_LL8: _tmp3D5=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->hd;_tmp3D4=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C0)->f2)->tl)->hd;_LL9:
# 2179
{struct _tuple0 _tmp3C2=({struct _tuple0 _tmp551;_tmp551.f1=_tmp3D5->r,_tmp551.f2=_tmp3D4->r;_tmp551;});struct _tuple0 _tmp3C3=_tmp3C2;void*_tmp3C7;void*_tmp3C6;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C3.f1)->tag == 17U){_LLD: _tmp3C6=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C3.f1)->f1;_LLE:
# 2181
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C6);
elt_type=_tmp3C6;
({void**_tmp7DD=({void**_tmp3C4=_cycalloc(sizeof(*_tmp3C4));*_tmp3C4=elt_type;_tmp3C4;});*t=_tmp7DD;});
num_elts=_tmp3D4;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C3.f2)->tag == 17U){_LLF: _tmp3C7=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3C3.f2)->f1;_LL10:
# 2188
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C7);
elt_type=_tmp3C7;
({void**_tmp7DE=({void**_tmp3C5=_cycalloc(sizeof(*_tmp3C5));*_tmp3C5=elt_type;_tmp3C5;});*t=_tmp7DE;});
num_elts=_tmp3D5;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2196
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2198
 No_sizeof: {
# 2201
struct Cyc_Absyn_Exp*_tmp3C8=*e;
{void*_tmp3C9=_tmp3C8->r;void*_tmp3CA=_tmp3C9;struct Cyc_Absyn_Exp*_tmp3CB;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CA)->tag == 14U){_LL14: _tmp3CB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CA)->f2;_LL15:
 _tmp3C8=_tmp3CB;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2206
{void*_tmp3CC=Cyc_Tcutil_compress((void*)_check_null(_tmp3C8->topt));void*_tmp3CD=_tmp3CC;void*_tmp3D1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CD)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CD)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CD)->f2 != 0){_LL19: _tmp3D1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CD)->f2)->hd;_LL1A:
# 2208
{void*_tmp3CE=Cyc_Tcutil_compress(_tmp3D1);void*_tmp3CF=_tmp3CE;struct Cyc_Absyn_Exp*_tmp3D0;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3CF)->tag == 9U){_LL1E: _tmp3D0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3CF)->f1;_LL1F:
# 2210
 er=_tmp3D0->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2213
goto _LL18;}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2216
elt_type=Cyc_Absyn_char_type;
({void**_tmp7DF=({void**_tmp3D2=_cycalloc(sizeof(*_tmp3D2));*_tmp3D2=elt_type;_tmp3D2;});*t=_tmp7DF;});
num_elts=*e;
one_elt=0;}
# 2221
goto _LL5;}_LL5:;}}
# 2225
*is_fat=!one_elt;
# 2228
{void*_tmp3D6=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D6)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D6)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D6)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp3D9=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D6)->f1)->f1).KnownAggr).val;_LL24:
# 2230
 if(_tmp3D9->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D9->impl))->exist_vars != 0)
({void*_tmp3D7=0U;({unsigned int _tmp7E1=loc;struct _dyneither_ptr _tmp7E0=({const char*_tmp3D8="malloc with existential types not yet implemented";_tag_dyneither(_tmp3D8,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp7E1,_tmp7E0,_tag_dyneither(_tmp3D7,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2237
void*(*_tmp3DA)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term)=Cyc_Absyn_at_type;
void*_tmp3DB=Cyc_Absyn_false_type;
if(topt != 0){
void*_tmp3DC=Cyc_Tcutil_compress(*topt);void*_tmp3DD=_tmp3DC;void*_tmp3E9;void*_tmp3E8;void*_tmp3E7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DD)->tag == 3U){_LL28: _tmp3E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DD)->f1).ptr_atts).nullable;_tmp3E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DD)->f1).ptr_atts).bounds;_tmp3E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DD)->f1).ptr_atts).zero_term;_LL29:
# 2242
 _tmp3DB=_tmp3E7;
if(Cyc_Tcutil_force_type2bool(0,_tmp3E9))
_tmp3DA=Cyc_Absyn_star_type;
# 2247
if(Cyc_Tcutil_force_type2bool(0,_tmp3E7) && !(*is_calloc)){
({void*_tmp3DE=0U;({unsigned int _tmp7E3=loc;struct _dyneither_ptr _tmp7E2=({const char*_tmp3DF="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp3DF,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp7E3,_tmp7E2,_tag_dyneither(_tmp3DE,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2253
struct Cyc_Absyn_Exp*_tmp3E0=({void*_tmp7E4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7E4,_tmp3E8);});
if(_tmp3E0 != 0  && !one_elt){
struct Cyc_Absyn_Exp*_tmp3E1=_tmp3E0;
int _tmp3E2=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp3E2  && Cyc_Evexp_same_const_exp(_tmp3E1,num_elts)){
*is_fat=0;
return({void*_tmp7E8=elt_type;void*_tmp7E7=rgn;struct Cyc_Absyn_Tqual _tmp7E6=Cyc_Absyn_empty_tqual(0U);void*_tmp7E5=_tmp3E8;Cyc_Absyn_atb_type(_tmp7E8,_tmp7E7,_tmp7E6,_tmp7E5,_tmp3DB);});}{
# 2261
void*_tmp3E3=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp3E4=_tmp3E3;void*_tmp3E6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f2 != 0){_LL2D: _tmp3E6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f2)->hd;_LL2E: {
# 2263
struct Cyc_Absyn_Exp*_tmp3E5=({void*_tmp7E9=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7E9,Cyc_Absyn_valueof_exp(_tmp3E6,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2265
if(Cyc_Evexp_same_const_exp(_tmp3E5,_tmp3E1)){
*is_fat=0;
return({void*_tmp7ED=elt_type;void*_tmp7EC=rgn;struct Cyc_Absyn_Tqual _tmp7EB=Cyc_Absyn_empty_tqual(0U);void*_tmp7EA=_tmp3E8;Cyc_Absyn_atb_type(_tmp7ED,_tmp7EC,_tmp7EB,_tmp7EA,_tmp3DB);});}
# 2269
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;};}
# 2273
goto _LL27;};}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2276
if(!one_elt)_tmp3DA=Cyc_Absyn_fatptr_type;
return({void*(*_tmp7F1)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term)=_tmp3DA;void*_tmp7F0=elt_type;void*_tmp7EF=rgn;struct Cyc_Absyn_Tqual _tmp7EE=Cyc_Absyn_empty_tqual(0U);_tmp7F1(_tmp7F0,_tmp7EF,_tmp7EE,_tmp3DB);});};};}
# 2281
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2283
struct Cyc_Tcenv_Tenv*_tmp3EA=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp3EA,0,e1);{
void*_tmp3EB=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp3EA,& _tmp3EB,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2290
{void*_tmp3EC=Cyc_Tcutil_compress(t1);void*_tmp3ED=_tmp3EC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3ED)->tag == 4U){_LL1: _LL2:
({void*_tmp3EE=0U;({unsigned int _tmp7F3=loc;struct _dyneither_ptr _tmp7F2=({const char*_tmp3EF="cannot assign to an array";_tag_dyneither(_tmp3EF,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp7F3,_tmp7F2,_tag_dyneither(_tmp3EE,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2295
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp3F0=0U;({unsigned int _tmp7F5=loc;struct _dyneither_ptr _tmp7F4=({const char*_tmp3F1="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp3F1,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp7F5,_tmp7F4,_tag_dyneither(_tmp3F0,sizeof(void*),0U));});});
# 2299
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp3F2=0U;({struct Cyc_Tcenv_Tenv*_tmp7F9=te;unsigned int _tmp7F8=e1->loc;void**_tmp7F7=topt;struct _dyneither_ptr _tmp7F6=({const char*_tmp3F3="swap non-lvalue";_tag_dyneither(_tmp3F3,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7F9,_tmp7F8,_tmp7F7,_tmp7F6,_tag_dyneither(_tmp3F2,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp3F4=0U;({struct Cyc_Tcenv_Tenv*_tmp7FD=te;unsigned int _tmp7FC=e2->loc;void**_tmp7FB=topt;struct _dyneither_ptr _tmp7FA=({const char*_tmp3F5="swap non-lvalue";_tag_dyneither(_tmp3F5,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7FD,_tmp7FC,_tmp7FB,_tmp7FA,_tag_dyneither(_tmp3F4,sizeof(void*),0U));});});
# 2304
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp3F6=({struct Cyc_String_pa_PrintArg_struct _tmp3F9=({struct Cyc_String_pa_PrintArg_struct _tmp553;_tmp553.tag=0U,({
struct _dyneither_ptr _tmp7FE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp553.f1=_tmp7FE;});_tmp553;});struct Cyc_String_pa_PrintArg_struct _tmp3FA=({struct Cyc_String_pa_PrintArg_struct _tmp552;_tmp552.tag=0U,({struct _dyneither_ptr _tmp7FF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp552.f1=_tmp7FF;});_tmp552;});void*_tmp3F7[2U];_tmp3F7[0]=& _tmp3F9,_tmp3F7[1]=& _tmp3FA;({struct Cyc_Tcenv_Tenv*_tmp803=te;unsigned int _tmp802=loc;void**_tmp801=topt;struct _dyneither_ptr _tmp800=({const char*_tmp3F8="type mismatch: %s != %s";_tag_dyneither(_tmp3F8,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp803,_tmp802,_tmp801,_tmp800,_tag_dyneither(_tmp3F7,sizeof(void*),2U));});});
return _tmp3F6;}
# 2311
return Cyc_Absyn_void_type;};};}
# 2316
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp804=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp804,s,1);});
# 2319
while(1){
void*_tmp3FB=s->r;void*_tmp3FC=_tmp3FB;struct Cyc_Absyn_Decl*_tmp407;struct Cyc_Absyn_Stmt*_tmp406;struct Cyc_Absyn_Stmt*_tmp405;struct Cyc_Absyn_Stmt*_tmp404;struct Cyc_Absyn_Exp*_tmp403;switch(*((int*)_tmp3FC)){case 1U: _LL1: _tmp403=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3FC)->f1;_LL2: {
# 2323
void*_tmp3FD=(void*)_check_null(_tmp403->topt);
if(!({void*_tmp805=_tmp3FD;Cyc_Tcutil_unify(_tmp805,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp400=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,({
struct _dyneither_ptr _tmp806=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3FD));_tmp554.f1=_tmp806;});_tmp554;});void*_tmp3FE[1U];_tmp3FE[0]=& _tmp400;({unsigned int _tmp808=loc;struct _dyneither_ptr _tmp807=({const char*_tmp3FF="statement expression returns type %s";_tag_dyneither(_tmp3FF,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp808,_tmp807,_tag_dyneither(_tmp3FE,sizeof(void*),1U));});});
Cyc_Tcutil_explain_failure();}
# 2329
return _tmp3FD;}case 2U: _LL3: _tmp405=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3FC)->f1;_tmp404=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3FC)->f2;_LL4:
 s=_tmp404;continue;case 12U: _LL5: _tmp407=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FC)->f1;_tmp406=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3FC)->f2;_LL6:
 s=_tmp406;continue;default: _LL7: _LL8:
# 2333
 return({void*_tmp401=0U;({struct Cyc_Tcenv_Tenv*_tmp80C=te;unsigned int _tmp80B=loc;void**_tmp80A=topt;struct _dyneither_ptr _tmp809=({const char*_tmp402="statement expression must end with expression";_tag_dyneither(_tmp402,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp80C,_tmp80B,_tmp80A,_tmp809,_tag_dyneither(_tmp401,sizeof(void*),0U));});});}_LL0:;}}
# 2338
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp80D=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp80D,0,e);}));
{void*_tmp408=t;struct Cyc_Absyn_Aggrdecl*_tmp40C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp40C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp408)->f1)->f1).KnownAggr).val;_LL2:
# 2342
 if((_tmp40C->kind == Cyc_Absyn_UnionA  && _tmp40C->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp40C->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2345
({struct Cyc_String_pa_PrintArg_struct _tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0U,({struct _dyneither_ptr _tmp80E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp555.f1=_tmp80E;});_tmp555;});void*_tmp409[1U];_tmp409[0]=& _tmp40B;({unsigned int _tmp810=loc;struct _dyneither_ptr _tmp80F=({const char*_tmp40A="expecting @tagged union but found %s";_tag_dyneither(_tmp40A,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp810,_tmp80F,_tag_dyneither(_tmp409,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2348
return Cyc_Absyn_uint_type;}
# 2352
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2356
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp40D=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2361
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp40D)));
void*handle_type=Cyc_Tcexp_tcExp(_tmp40D,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp40E=Cyc_Tcutil_compress(handle_type);void*_tmp40F=_tmp40E;void*_tmp413;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40F)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40F)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40F)->f2 != 0){_LL1: _tmp413=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40F)->f2)->hd;_LL2:
# 2366
 rgn=_tmp413;
Cyc_Tcenv_check_rgn_accessible(_tmp40D,loc,rgn);
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2370
({struct Cyc_String_pa_PrintArg_struct _tmp412=({struct Cyc_String_pa_PrintArg_struct _tmp556;_tmp556.tag=0U,({
struct _dyneither_ptr _tmp811=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp556.f1=_tmp811;});_tmp556;});void*_tmp410[1U];_tmp410[0]=& _tmp412;({unsigned int _tmp813=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp812=({const char*_tmp411="expecting region_t type but found %s";_tag_dyneither(_tmp411,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp813,_tmp812,_tag_dyneither(_tmp410,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2377
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp814=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp814;});
if(rgn == Cyc_Absyn_unique_rgn_type)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2385
void*_tmp414=e1->r;void*_tmp415=_tmp414;struct Cyc_List_List*_tmp42D;struct Cyc_Core_Opt*_tmp42C;struct Cyc_List_List*_tmp42B;switch(*((int*)_tmp415)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2390
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp40D,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp416=0U;({struct _dyneither_ptr _tmp815=({const char*_tmp417="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp417,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp815,_tag_dyneither(_tmp416,sizeof(void*),0U));});});
({void*_tmp816=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp816;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp40D,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp40D,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2401
return res_typ;}case 36U: _LLA: _tmp42C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp415)->f1;_tmp42B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp415)->f2;_LLB:
# 2403
({void*_tmp817=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->tag=26U,_tmp418->f1=_tmp42B;_tmp418;});e1->r=_tmp817;});
_tmp42D=_tmp42B;goto _LLD;case 26U: _LLC: _tmp42D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp415)->f1;_LLD: {
# 2406
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp419=Cyc_Tcutil_compress(*topt);void*_tmp41A=_tmp419;void**_tmp41D;struct Cyc_Absyn_Tqual _tmp41C;void*_tmp41B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41A)->tag == 3U){_LL15: _tmp41D=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41A)->f1).elt_type;_tmp41C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41A)->f1).elt_tq;_tmp41B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41A)->f1).ptr_atts).zero_term;_LL16:
# 2412
 elt_typ_opt=_tmp41D;
({int _tmp818=Cyc_Tcutil_force_type2bool(0,_tmp41B);zero_term=_tmp818;});
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2418
void*res_typ=Cyc_Tcexp_tcArray(_tmp40D,e1->loc,elt_typ_opt,0,zero_term,_tmp42D);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp41E=0U;({struct _dyneither_ptr _tmp819=({const char*_tmp41F="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp41F,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp819,_tag_dyneither(_tmp41E,sizeof(void*),0U));});});
({void*_tmp81A=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp81A;});
if(topt != 0){
# 2427
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp40D,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp40D,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2433
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp415)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2438
void*_tmp420=({void*_tmp81E=Cyc_Absyn_char_type;void*_tmp81D=rgn;struct Cyc_Absyn_Tqual _tmp81C=Cyc_Absyn_const_tqual(0U);void*_tmp81B=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp81E,_tmp81D,_tmp81C,_tmp81B,Cyc_Absyn_true_type);});
# 2440
void*_tmp421=Cyc_Tcexp_tcExp(_tmp40D,& _tmp420,e1);
return({void*_tmp822=_tmp421;void*_tmp821=rgn;struct Cyc_Absyn_Tqual _tmp820=Cyc_Absyn_empty_tqual(0U);void*_tmp81F=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp822,_tmp821,_tmp820,_tmp81F,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2445
void*_tmp422=({void*_tmp826=Cyc_Absyn_wchar_type();void*_tmp825=rgn;struct Cyc_Absyn_Tqual _tmp824=Cyc_Absyn_const_tqual(0U);void*_tmp823=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp826,_tmp825,_tmp824,_tmp823,Cyc_Absyn_true_type);});
# 2447
void*_tmp423=Cyc_Tcexp_tcExp(_tmp40D,& _tmp422,e1);
return({void*_tmp82A=_tmp423;void*_tmp829=rgn;struct Cyc_Absyn_Tqual _tmp828=Cyc_Absyn_empty_tqual(0U);void*_tmp827=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp82A,_tmp829,_tmp828,_tmp827,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2454
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp424=Cyc_Tcutil_compress(*topt);void*_tmp425=_tmp424;void**_tmp427;struct Cyc_Absyn_Tqual _tmp426;switch(*((int*)_tmp425)){case 3U: _LL1A: _tmp427=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp425)->f1).elt_type;_tmp426=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp425)->f1).elt_tq;_LL1B:
# 2460
 topt2=_tmp427;goto _LL19;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp425)->f1)->tag == 18U){_LL1C: _LL1D:
# 2464
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2470
void*telt=Cyc_Tcexp_tcExp(_tmp40D,topt2,e1);
# 2472
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp428=0U;({unsigned int _tmp82C=e1->loc;struct _dyneither_ptr _tmp82B=({const char*_tmp429="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp429,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp82C,_tmp82B,_tag_dyneither(_tmp428,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));
_tmp42A->tag=3U,(_tmp42A->f1).elt_type=telt,({struct Cyc_Absyn_Tqual _tmp82F=Cyc_Absyn_empty_tqual(0U);(_tmp42A->f1).elt_tq=_tmp82F;}),
((_tmp42A->f1).ptr_atts).rgn=rgn,({void*_tmp82E=Cyc_Tcutil_any_bool(& _tmp40D);((_tmp42A->f1).ptr_atts).nullable=_tmp82E;}),({
void*_tmp82D=Cyc_Absyn_bounds_one();((_tmp42A->f1).ptr_atts).bounds=_tmp82D;}),((_tmp42A->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp42A->f1).ptr_atts).ptrloc=0;_tmp42A;});
# 2479
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp40D,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp40D,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2486
return res_typ;};};}}_LL5:;};}
# 2492
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp832=({void*_tmp831=({void*_tmp830=t;Cyc_Tcutil_promote_array(_tmp830,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp831;});e->topt=_tmp832;});
return t;}
# 2502
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2506
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2511
({void*_tmp42E=0U;({unsigned int _tmp834=e->loc;struct _dyneither_ptr _tmp833=({const char*_tmp42F="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp42F,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp834,_tmp833,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});{
# 2513
void*_tmp430=e->r;void*_tmp431=_tmp430;switch(*((int*)_tmp431)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp431)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2520
({void*_tmp835=Cyc_Tcutil_compress(t);t=_tmp835;});
if(Cyc_Tcutil_is_array_type(t)){
({void*_tmp837=({void*_tmp836=t;Cyc_Tcutil_promote_array(_tmp836,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp837;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2525
return t;}_LL0:;};}
# 2536 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp432=e->r;void*_tmp433=_tmp432;struct Cyc_Absyn_Exp*_tmp451;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp433)->tag == 12U){_LL1: _tmp451=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp433)->f1;_LL2:
# 2540
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp451);
({void*_tmp838=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp451->topt),0);_tmp451->topt=_tmp838;});
e->topt=_tmp451->topt;
goto _LL0;}else{_LL3: _LL4:
# 2546
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp839=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp839;});
# 2549
{void*_tmp434=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp435=_tmp434;void*_tmp450;struct Cyc_Absyn_Tqual _tmp44F;void*_tmp44E;void*_tmp44D;void*_tmp44C;void*_tmp44B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->tag == 3U){_LL6: _tmp450=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->f1).elt_type;_tmp44F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->f1).elt_tq;_tmp44E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->f1).ptr_atts).rgn;_tmp44D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->f1).ptr_atts).nullable;_tmp44C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->f1).ptr_atts).bounds;_tmp44B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp435)->f1).ptr_atts).zero_term;_LL7:
# 2551
{void*_tmp436=Cyc_Tcutil_compress(_tmp450);void*_tmp437=_tmp436;struct Cyc_List_List*_tmp44A;void*_tmp449;struct Cyc_Absyn_Tqual _tmp448;void*_tmp447;struct Cyc_List_List*_tmp446;int _tmp445;struct Cyc_Absyn_VarargInfo*_tmp444;struct Cyc_List_List*_tmp443;struct Cyc_List_List*_tmp442;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_List_List*_tmp440;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_List_List*_tmp43E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->tag == 5U){_LLB: _tmp44A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).tvars;_tmp449=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).effect;_tmp448=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).ret_tqual;_tmp447=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).ret_type;_tmp446=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).args;_tmp445=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).c_varargs;_tmp444=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).cyc_varargs;_tmp443=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).rgn_po;_tmp442=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).attributes;_tmp441=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).requires_clause;_tmp440=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).requires_relns;_tmp43F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).ensures_clause;_tmp43E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437)->f1).ensures_relns;_LLC:
# 2553
 if(_tmp44A != 0){
struct _tuple12 _tmp438=({struct _tuple12 _tmp557;({struct Cyc_List_List*_tmp83A=Cyc_Tcenv_lookup_type_vars(te);_tmp557.f1=_tmp83A;}),_tmp557.f2=Cyc_Core_heap_region;_tmp557;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp438,_tmp44A);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2561
({struct Cyc_List_List*_tmp83B=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp443);_tmp443=_tmp83B;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp443);{
void*ftyp=({struct Cyc_List_List*_tmp83C=inst;Cyc_Tcutil_substitute(_tmp83C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));
_tmp43D->tag=5U,(_tmp43D->f1).tvars=0,(_tmp43D->f1).effect=_tmp449,(_tmp43D->f1).ret_tqual=_tmp448,(_tmp43D->f1).ret_type=_tmp447,(_tmp43D->f1).args=_tmp446,(_tmp43D->f1).c_varargs=_tmp445,(_tmp43D->f1).cyc_varargs=_tmp444,(_tmp43D->f1).rgn_po=0,(_tmp43D->f1).attributes=_tmp442,(_tmp43D->f1).requires_clause=_tmp441,(_tmp43D->f1).requires_relns=_tmp440,(_tmp43D->f1).ensures_clause=_tmp43F,(_tmp43D->f1).ensures_relns=_tmp43E;_tmp43D;}));});
# 2568
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp439=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=3U,(_tmp43C->f1).elt_type=ftyp,(_tmp43C->f1).elt_tq=_tmp44F,((_tmp43C->f1).ptr_atts).rgn=_tmp44E,((_tmp43C->f1).ptr_atts).nullable=_tmp44D,((_tmp43C->f1).ptr_atts).bounds=_tmp44C,((_tmp43C->f1).ptr_atts).zero_term=_tmp44B,((_tmp43C->f1).ptr_atts).ptrloc=0;_tmp43C;});
# 2570
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Absyn_copy_exp(e);
({void*_tmp83D=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->tag=13U,_tmp43B->f1=_tmp43A,_tmp43B->f2=ts;_tmp43B;});e->r=_tmp83D;});
e->topt=(void*)_tmp439;};}
# 2574
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2577
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2580
goto _LL0;}_LL0:;}
# 2582
return(void*)_check_null(e->topt);}
# 2590
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2592
struct Cyc_List_List*_tmp452=0;
# 2594
{void*_tmp453=fn_exp->r;void*_tmp454=_tmp453;struct Cyc_List_List*_tmp468;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp454)->tag == 10U){_LL1: _tmp468=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp454)->f2;_LL2:
# 2596
{void*_tmp455=e->r;void*_tmp456=_tmp455;struct Cyc_List_List*_tmp465;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp456)->tag == 10U){_LL6: _tmp465=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp456)->f2;_LL7: {
# 2598
struct Cyc_List_List*_tmp457=alias_arg_exps;
int _tmp458=0;
while(_tmp457 != 0){
while(_tmp458 != (int)_tmp457->hd){
if(_tmp468 == 0)
({void*_tmp459=0U;({struct _dyneither_ptr _tmp83F=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp45C=({struct Cyc_Int_pa_PrintArg_struct _tmp559;_tmp559.tag=1U,_tmp559.f1=(unsigned long)_tmp458;_tmp559;});struct Cyc_Int_pa_PrintArg_struct _tmp45D=({struct Cyc_Int_pa_PrintArg_struct _tmp558;_tmp558.tag=1U,_tmp558.f1=(unsigned long)((int)_tmp457->hd);_tmp558;});void*_tmp45A[2U];_tmp45A[0]=& _tmp45C,_tmp45A[1]=& _tmp45D;({struct _dyneither_ptr _tmp83E=({const char*_tmp45B="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp45B,sizeof(char),36U);});Cyc_aprintf(_tmp83E,_tag_dyneither(_tmp45A,sizeof(void*),2U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp83F,_tag_dyneither(_tmp459,sizeof(void*),0U));});});
# 2605
++ _tmp458;
_tmp468=_tmp468->tl;
_tmp465=((struct Cyc_List_List*)_check_null(_tmp465))->tl;}{
# 2610
struct _tuple11 _tmp45E=({struct Cyc_Absyn_Exp*_tmp840=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp468))->hd;Cyc_Tcutil_insert_alias(_tmp840,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp465))->hd)->topt)));});struct _tuple11 _tmp45F=_tmp45E;struct Cyc_Absyn_Decl*_tmp462;struct Cyc_Absyn_Exp*_tmp461;_LLB: _tmp462=_tmp45F.f1;_tmp461=_tmp45F.f2;_LLC:;
_tmp468->hd=(void*)_tmp461;
({struct Cyc_List_List*_tmp841=({struct Cyc_List_List*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->hd=_tmp462,_tmp460->tl=_tmp452;_tmp460;});_tmp452=_tmp841;});
_tmp457=_tmp457->tl;};}
# 2615
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp463=0U;({struct _dyneither_ptr _tmp842=({const char*_tmp464="not a function call!";_tag_dyneither(_tmp464,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp842,_tag_dyneither(_tmp463,sizeof(void*),0U));});});}_LL5:;}
# 2618
goto _LL0;}else{_LL3: _LL4:
({void*_tmp466=0U;({struct _dyneither_ptr _tmp843=({const char*_tmp467="not a function call!";_tag_dyneither(_tmp467,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp843,_tag_dyneither(_tmp466,sizeof(void*),0U));});});}_LL0:;}
# 2623
while(_tmp452 != 0){
struct Cyc_Absyn_Decl*_tmp469=(struct Cyc_Absyn_Decl*)_tmp452->hd;
({struct Cyc_Absyn_Exp*_tmp847=({struct Cyc_Absyn_Stmt*_tmp846=({struct Cyc_Absyn_Decl*_tmp845=_tmp469;struct Cyc_Absyn_Stmt*_tmp844=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp845,_tmp844,e->loc);});Cyc_Absyn_stmt_exp(_tmp846,e->loc);});fn_exp=_tmp847;});
_tmp452=_tmp452->tl;}
# 2630
e->topt=0;
e->r=fn_exp->r;}
# 2636
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2640
{void*_tmp46A=e->r;void*_tmp46B=_tmp46A;void*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D2;struct _dyneither_ptr*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;int*_tmp4CE;struct Cyc_Absyn_Exp**_tmp4CD;void***_tmp4CC;struct Cyc_Absyn_Exp**_tmp4CB;int*_tmp4CA;void*_tmp4C9;struct Cyc_Absyn_Enumfield*_tmp4C8;struct Cyc_Absyn_Enumdecl*_tmp4C7;struct Cyc_Absyn_Enumfield*_tmp4C6;struct Cyc_List_List*_tmp4C5;struct Cyc_Absyn_Datatypedecl*_tmp4C4;struct Cyc_Absyn_Datatypefield*_tmp4C3;void*_tmp4C2;struct Cyc_List_List*_tmp4C1;struct _tuple1**_tmp4C0;struct Cyc_List_List**_tmp4BF;struct Cyc_List_List*_tmp4BE;struct Cyc_Absyn_Aggrdecl**_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BC;void*_tmp4BB;int*_tmp4BA;struct Cyc_Absyn_Vardecl*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;int*_tmp4B6;struct Cyc_Absyn_Stmt*_tmp4B5;struct Cyc_List_List*_tmp4B4;struct _tuple9*_tmp4B3;struct Cyc_List_List*_tmp4B2;struct Cyc_List_List*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;struct _dyneither_ptr*_tmp4AD;int*_tmp4AC;int*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct _dyneither_ptr*_tmp4A9;int*_tmp4A8;int*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;void*_tmp4A5;struct Cyc_List_List*_tmp4A4;void*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp**_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;void*_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;enum Cyc_Absyn_Coercion*_tmp49C;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_List_List*_tmp49A;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Core_Opt*_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;enum Cyc_Absyn_Incrementor _tmp48B;enum Cyc_Absyn_Primop _tmp48A;struct Cyc_List_List*_tmp489;void**_tmp488;union Cyc_Absyn_Cnst*_tmp487;struct Cyc_Core_Opt*_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_List_List*_tmp483;struct Cyc_Absyn_VarargCallInfo**_tmp482;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_List_List*_tmp480;struct Cyc_Absyn_VarargCallInfo**_tmp47F;int*_tmp47E;struct Cyc_Absyn_Exp*_tmp47D;switch(*((int*)_tmp46B)){case 12U: _LL1: _tmp47D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL2:
# 2645
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp47D);
return;case 10U: _LL3: _tmp481=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp480=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp47F=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_tmp47E=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;if(!(*_tmp47E)){_LL4:
# 2649
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp848=({const char*_tmp46C="unresolved function in tcExpNoInst";_tag_dyneither(_tmp46C,sizeof(char),35U);});_tmp46D->f1=_tmp848;});_tmp46D;}));}else{_LL1B: _tmp484=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp483=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp482=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_LL1C: {
# 2682
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp46E;_push_handler(& _tmp46E);{int _tmp470=0;if(setjmp(_tmp46E.handler))_tmp470=1;if(!_tmp470){
({struct Cyc_Absyn_Exp*_tmp849=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp849;});;_pop_handler();}else{void*_tmp46F=(void*)_exn_thrown;void*_tmp471=_tmp46F;void*_tmp472;if(((struct Cyc_Core_Failure_exn_struct*)_tmp471)->tag == Cyc_Core_Failure){_LL58: _LL59:
# 2689
 ok=0;
fn_exp=e;
goto _LL57;}else{_LL5A: _tmp472=_tmp471;_LL5B:(int)_rethrow(_tmp472);}_LL57:;}};}
# 2693
({void*_tmp84A=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp484,_tmp483,_tmp482,& alias_arg_exps);t=_tmp84A;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp84B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp84B;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2700
goto _LL0;}}case 36U: _LL5: _tmp486=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp485=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL6:
# 2653
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp485);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp487=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL8:
# 2658
({void*_tmp84C=Cyc_Tcexp_tcConst(te,loc,topt,_tmp487,e);t=_tmp84C;});goto _LL0;case 1U: _LL9: _tmp488=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LLA:
# 2660
({void*_tmp84D=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp488);t=_tmp84D;});goto _LL0;case 2U: _LLB: _LLC:
# 2662
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp48A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp489=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LLE:
# 2664
({void*_tmp84E=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp48A,_tmp489);t=_tmp84E;});goto _LL0;case 5U: _LLF: _tmp48C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp48B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL10:
# 2666
({void*_tmp84F=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp48C,_tmp48B);t=_tmp84F;});goto _LL0;case 4U: _LL11: _tmp48F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp48E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp48D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_LL12:
# 2668
({void*_tmp850=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp48F,_tmp48E,_tmp48D);t=_tmp850;});goto _LL0;case 6U: _LL13: _tmp492=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp491=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp490=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_LL14:
# 2670
({void*_tmp851=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp492,_tmp491,_tmp490);t=_tmp851;});goto _LL0;case 7U: _LL15: _tmp494=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp493=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL16:
# 2672
({void*_tmp852=Cyc_Tcexp_tcAnd(te,loc,_tmp494,_tmp493);t=_tmp852;});goto _LL0;case 8U: _LL17: _tmp496=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp495=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL18:
# 2674
({void*_tmp853=Cyc_Tcexp_tcOr(te,loc,_tmp496,_tmp495);t=_tmp853;});goto _LL0;case 9U: _LL19: _tmp498=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp497=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL1A:
# 2676
({void*_tmp854=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp498,_tmp497);t=_tmp854;});goto _LL0;case 11U: _LL1D: _tmp499=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL1E:
# 2702
({void*_tmp855=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp499);t=_tmp855;});goto _LL0;case 13U: _LL1F: _tmp49B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp49A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL20:
# 2704
({void*_tmp856=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp49B,_tmp49A);t=_tmp856;});goto _LL0;case 14U: _LL21: _tmp49E=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp49D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp49C=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;_LL22:
# 2706
({void*_tmp857=Cyc_Tcexp_tcCast(te,loc,topt,_tmp49E,_tmp49D,_tmp49C);t=_tmp857;});goto _LL0;case 15U: _LL23: _tmp49F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL24:
# 2708
({void*_tmp858=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp49F);t=_tmp858;});goto _LL0;case 16U: _LL25: _tmp4A1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4A0=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL26:
# 2710
({void*_tmp859=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4A1,e,_tmp4A0);t=_tmp859;});goto _LL0;case 18U: _LL27: _tmp4A2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL28: {
# 2712
void*_tmp473=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4A2);
({void*_tmp85A=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp473);t=_tmp85A;});goto _LL0;}case 17U: _LL29: _tmp4A3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL2A:
# 2715
({void*_tmp85B=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4A3);t=_tmp85B;});goto _LL0;case 19U: _LL2B: _tmp4A5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4A4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL2C:
# 2717
({void*_tmp85C=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4A5,_tmp4A4);t=_tmp85C;});goto _LL0;case 20U: _LL2D: _tmp4A6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL2E:
# 2719
({void*_tmp85D=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4A6);t=_tmp85D;});goto _LL0;case 21U: _LL2F: _tmp4AA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4A9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp4A8=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_tmp4A7=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;_LL30:
# 2721
({void*_tmp85E=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4AA,_tmp4A9,_tmp4A8,_tmp4A7);t=_tmp85E;});goto _LL0;case 22U: _LL31: _tmp4AE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4AD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp4AC=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_tmp4AB=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;_LL32:
# 2723
({void*_tmp85F=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4AE,_tmp4AD,_tmp4AC,_tmp4AB);t=_tmp85F;});goto _LL0;case 23U: _LL33: _tmp4B0=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4AF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL34:
# 2725
({void*_tmp860=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4B0,_tmp4AF);t=_tmp860;});goto _LL0;case 24U: _LL35: _tmp4B1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL36:
# 2727
({void*_tmp861=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4B1);t=_tmp861;});goto _LL0;case 25U: _LL37: _tmp4B3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4B2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL38:
# 2729
({void*_tmp862=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4B3,_tmp4B2);t=_tmp862;});goto _LL0;case 26U: _LL39: _tmp4B4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL3A: {
# 2733
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp474=Cyc_Tcutil_compress(*topt);void*_tmp475=_tmp474;void**_tmp478;struct Cyc_Absyn_Tqual*_tmp477;void*_tmp476;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp475)->tag == 4U){_LL5D: _tmp478=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp475)->f1).elt_type;_tmp477=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp475)->f1).tq;_tmp476=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp475)->f1).zero_term;_LL5E:
# 2739
 elt_topt=_tmp478;
elt_tqopt=_tmp477;
({int _tmp863=Cyc_Tcutil_force_type2bool(0,_tmp476);zero_term=_tmp863;});
goto _LL5C;}else{_LL5F: _LL60:
 goto _LL5C;}_LL5C:;}
# 2746
({void*_tmp864=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4B4);t=_tmp864;});goto _LL0;}case 37U: _LL3B: _tmp4B5=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL3C:
# 2748
({void*_tmp865=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4B5);t=_tmp865;});goto _LL0;case 27U: _LL3D: _tmp4B9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4B8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp4B7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_tmp4B6=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;_LL3E:
# 2750
({void*_tmp866=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4B9,_tmp4B8,_tmp4B7,_tmp4B6);t=_tmp866;});goto _LL0;case 28U: _LL3F: _tmp4BC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4BB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp4BA=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_LL40:
# 2752
({void*_tmp867=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4BC,_tmp4BB,_tmp4BA);t=_tmp867;});goto _LL0;case 29U: _LL41: _tmp4C0=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4BF=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp4BE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_tmp4BD=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp46B)->f4;_LL42:
# 2754
({void*_tmp868=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4C0,_tmp4BF,_tmp4BE,_tmp4BD);t=_tmp868;});goto _LL0;case 30U: _LL43: _tmp4C2=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4C1=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL44:
# 2756
({void*_tmp869=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4C2,_tmp4C1);t=_tmp869;});goto _LL0;case 31U: _LL45: _tmp4C5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4C4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_tmp4C3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp46B)->f3;_LL46:
# 2758
({void*_tmp86A=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp4C5,_tmp4C4,_tmp4C3);t=_tmp86A;});goto _LL0;case 32U: _LL47: _tmp4C7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4C6=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL48:
# 2760
({void*_tmp86B=Cyc_Absyn_enum_type(_tmp4C7->name,_tmp4C7);t=_tmp86B;});goto _LL0;case 33U: _LL49: _tmp4C9=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4C8=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL4A:
# 2762
 t=_tmp4C9;goto _LL0;case 34U: _LL4B: _tmp4CE=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46B)->f1).is_calloc;_tmp4CD=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46B)->f1).rgn;_tmp4CC=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46B)->f1).elt_type;_tmp4CB=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46B)->f1).num_elts;_tmp4CA=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp46B)->f1).fat_result;_LL4C:
# 2764
({void*_tmp86C=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4CD,_tmp4CC,_tmp4CB,_tmp4CE,_tmp4CA);t=_tmp86C;});goto _LL0;case 35U: _LL4D: _tmp4D0=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4CF=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL4E:
# 2766
({void*_tmp86D=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4D0,_tmp4CF);t=_tmp86D;});goto _LL0;case 38U: _LL4F: _tmp4D2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_tmp4D1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp46B)->f2;_LL50:
# 2768
({void*_tmp86E=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp4D2,_tmp4D1);t=_tmp86E;});goto _LL0;case 41U: _LL51: _tmp4D3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL52:
# 2770
({void*_tmp86F=Cyc_Tcexp_tcExp(te,topt,_tmp4D3);t=_tmp86F;});goto _LL0;case 39U: _LL53: _tmp4D4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp46B)->f1;_LL54:
# 2772
 if(!te->allow_valueof)
({void*_tmp479=0U;({unsigned int _tmp871=e->loc;struct _dyneither_ptr _tmp870=({const char*_tmp47A="valueof(-) can only occur within types";_tag_dyneither(_tmp47A,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp871,_tmp870,_tag_dyneither(_tmp479,sizeof(void*),0U));});});
# 2780
t=Cyc_Absyn_sint_type;
goto _LL0;default: _LL55: _LL56:
# 2783
({void*_tmp47B=0U;({unsigned int _tmp873=e->loc;struct _dyneither_ptr _tmp872=({const char*_tmp47C="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp47C,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp873,_tmp872,_tag_dyneither(_tmp47B,sizeof(void*),0U));});});
({void*_tmp874=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp874;});}_LL0:;}
# 2786
e->topt=t;}
