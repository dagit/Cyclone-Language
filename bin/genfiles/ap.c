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

# 1 "ap.cyc"
 static char Cyc_rcsid[52U]="$Id: ap.c,v 1.7 2004-07-28 16:45:58 djg Exp $";
# 22 "ctype.h"
int isspace(int);struct _tuple0{int quot;int rem;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 81 "string.h"
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
extern struct Cyc_AP_T*Cyc_AP_one;
struct Cyc_AP_T*Cyc_AP_new(long n);
struct Cyc_AP_T*Cyc_AP_fromint(long x);
struct Cyc_AP_T*Cyc_AP_fromstr(const char*str,int base);
long Cyc_AP_toint(struct Cyc_AP_T*x);
char*Cyc_AP_tostr(struct Cyc_AP_T*x,int base);
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x);
struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*x);
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_pow(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p);
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*x,long y);
struct Cyc_AP_T*Cyc_AP_subi(struct Cyc_AP_T*x,long y);
struct Cyc_AP_T*Cyc_AP_muli(struct Cyc_AP_T*x,long y);
struct Cyc_AP_T*Cyc_AP_divi(struct Cyc_AP_T*x,long y);
long Cyc_AP_modi(struct Cyc_AP_T*x,long y);
struct Cyc_AP_T*Cyc_AP_lshift(struct Cyc_AP_T*x,int s);
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*x,int s);
struct Cyc_AP_T*Cyc_AP_and(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_or(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_xor(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
int Cyc_AP_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
int Cyc_AP_cmpi(struct Cyc_AP_T*x,long y);
struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
# 8 "xp.h"
int Cyc_XP_add(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y,int carry);
int Cyc_XP_sub(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y,int borrow);
int Cyc_XP_mul(struct _dyneither_ptr z,int n,struct _dyneither_ptr x,int m,struct _dyneither_ptr y);
int Cyc_XP_div(int n,struct _dyneither_ptr q,struct _dyneither_ptr x,int m,struct _dyneither_ptr y,struct _dyneither_ptr r,struct _dyneither_ptr tmp);
int Cyc_XP_sum(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y);
int Cyc_XP_diff(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,int y);
# 17
int Cyc_XP_cmp(int n,struct _dyneither_ptr x,struct _dyneither_ptr y);
void Cyc_XP_lshift(int n,struct _dyneither_ptr z,int m,struct _dyneither_ptr x,int s,int fill);
# 20
void Cyc_XP_rshift(int n,struct _dyneither_ptr z,int m,struct _dyneither_ptr x,int s,int fill);
# 22
void Cyc_XP_and(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y);
void Cyc_XP_or(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y);
void Cyc_XP_xor(int n,struct _dyneither_ptr z,struct _dyneither_ptr x,struct _dyneither_ptr y);
int Cyc_XP_length(int n,struct _dyneither_ptr x);
unsigned long Cyc_XP_fromint(int n,struct _dyneither_ptr z,unsigned long u);
# 28
unsigned long Cyc_XP_toint(int n,struct _dyneither_ptr x);
int Cyc_XP_fromstr(int n,struct _dyneither_ptr z,const char*str,int base);
# 31
struct _dyneither_ptr Cyc_XP_tostr(struct _dyneither_ptr str,int size,int base,int n,struct _dyneither_ptr x);struct Cyc_AP_T{int sign;int ndigits;int size;struct _dyneither_ptr digits;};
# 21 "ap.cyc"
struct Cyc_AP_T*Cyc_AP_zero;
struct Cyc_AP_T*Cyc_AP_one;
int Cyc_init=0;
# 25
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*z,int n);
static int Cyc_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
static void Cyc_AP_init(){
Cyc_init=1;
({struct Cyc_AP_T*_tmp53=Cyc_AP_fromint(0);Cyc_AP_zero=_tmp53;});
({struct Cyc_AP_T*_tmp54=Cyc_AP_fromint(1);Cyc_AP_one=_tmp54;});}
# 32
static struct Cyc_AP_T*Cyc_mk(int size){
struct Cyc_AP_T*z;
if(!Cyc_init)Cyc_AP_init();
({struct Cyc_AP_T*_tmp55=_cycalloc(sizeof(struct Cyc_AP_T));z=_tmp55;});
({struct _dyneither_ptr _tmp56=({unsigned int _tmp0=size;unsigned char*_tmp1=_cyccalloc_atomic(sizeof(unsigned char),_tmp0);_tag_dyneither(_tmp1,sizeof(unsigned char),_tmp0);});z->digits=_tmp56;});
size > 0?0:({struct _dyneither_ptr _tmp57=({const char*_tmp2="size > 0";_tag_dyneither(_tmp2,sizeof(char),9U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp57,({const char*_tmp3="ap.cyc";_tag_dyneither(_tmp3,sizeof(char),7U);}),37U);});
z->sign=1;
z->size=size;
z->ndigits=1;
return z;}
# 43
static struct Cyc_AP_T*Cyc_set(struct Cyc_AP_T*z,long n){
if(n == (- 2147483647)- 1)
Cyc_XP_fromint(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,2147483647 + 1U);else{
if(n < 0)
Cyc_XP_fromint(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,(unsigned long)(- n));else{
# 49
Cyc_XP_fromint(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,(unsigned long)n);}}
z->sign=n < 0?- 1: 1;
return Cyc_normalize(z,z->size);}
# 53
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*z,int n){
({int _tmp58=Cyc_XP_length(n,((struct Cyc_AP_T*)_check_null(z))->digits);z->ndigits=_tmp58;});
return z;}
# 57
static struct Cyc_AP_T*Cyc_add(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int n=((struct Cyc_AP_T*)_check_null(y))->ndigits;
if(((struct Cyc_AP_T*)_check_null(x))->ndigits < n)
return Cyc_add(z,y,x);else{
if(x->ndigits > n){
int carry=Cyc_XP_add(n,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits,0);
# 64
({unsigned char _tmp5C=(unsigned char)({int _tmp5B=x->ndigits - n;struct _dyneither_ptr _tmp5A=
_dyneither_ptr_plus(z->digits,sizeof(unsigned char),n);
# 64
struct _dyneither_ptr _tmp59=
_dyneither_ptr_plus(x->digits,sizeof(unsigned char),n);
# 64
Cyc_XP_sum(_tmp5B,_tmp5A,_tmp59,carry);});*((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),z->size - 1))=_tmp5C;});}else{
# 67
({unsigned char _tmp5D=(unsigned char)Cyc_XP_add(n,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits,0);*((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),n))=_tmp5D;});}}
# 69
return Cyc_normalize(z,z->size);}
# 71
static struct Cyc_AP_T*Cyc_sub(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int borrow;int n=((struct Cyc_AP_T*)_check_null(y))->ndigits;
({int _tmp61=({int _tmp60=n;struct _dyneither_ptr _tmp5F=((struct Cyc_AP_T*)_check_null(z))->digits;struct _dyneither_ptr _tmp5E=((struct Cyc_AP_T*)_check_null(x))->digits;Cyc_XP_sub(_tmp60,_tmp5F,_tmp5E,y->digits,0);});borrow=_tmp61;});
# 75
if(x->ndigits > n)
({int _tmp65=({int _tmp64=x->ndigits - n;struct _dyneither_ptr _tmp63=_dyneither_ptr_plus(z->digits,sizeof(unsigned char),n);struct _dyneither_ptr _tmp62=
_dyneither_ptr_plus(x->digits,sizeof(unsigned char),n);
# 76
Cyc_XP_diff(_tmp64,_tmp63,_tmp62,borrow);});borrow=_tmp65;});
# 78
borrow == 0?0:({struct _dyneither_ptr _tmp66=({const char*_tmp4="borrow == 0";_tag_dyneither(_tmp4,sizeof(char),12U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp66,({const char*_tmp5="ap.cyc";_tag_dyneither(_tmp5,sizeof(char),7U);}),78U);});
return Cyc_normalize(z,z->size);}
# 81
static struct Cyc_AP_T*Cyc_mulmod(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;struct Cyc_AP_T*xy=Cyc_AP_mul(x,y);
({struct Cyc_AP_T*_tmp67=Cyc_AP_mod(xy,p);z=_tmp67;});
return z;}
# 86
static int Cyc_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
if(({int _tmp68=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmp68 != ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return x->ndigits - y->ndigits;else{
# 90
return Cyc_XP_cmp(x->ndigits,x->digits,y->digits);}}
# 92
struct Cyc_AP_T*Cyc_AP_new(long n){
return({struct Cyc_AP_T*_tmp69=Cyc_mk((int)sizeof(long));Cyc_set(_tmp69,n);});}
# 95
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp6A=({const char*_tmp6="x";_tag_dyneither(_tmp6,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp6A,({const char*_tmp7="ap.cyc";_tag_dyneither(_tmp7,sizeof(char),7U);}),97U);});
({struct Cyc_AP_T*_tmp6B=Cyc_mk(x->ndigits);z=_tmp6B;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=z->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;
return z;}
# 104
struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp6C=({const char*_tmp8="x";_tag_dyneither(_tmp8,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp6C,({const char*_tmp9="ap.cyc";_tag_dyneither(_tmp9,sizeof(char),7U);}),106U);});
({struct Cyc_AP_T*_tmp6D=Cyc_mk(x->ndigits);z=_tmp6D;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=1;
return z;}
# 113
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp6E=({const char*_tmpA="x";_tag_dyneither(_tmpA,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp6E,({const char*_tmpB="ap.cyc";_tag_dyneither(_tmpB,sizeof(char),7U);}),115U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp6F=({const char*_tmpC="y";_tag_dyneither(_tmpC,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp6F,({const char*_tmpD="ap.cyc";_tag_dyneither(_tmpD,sizeof(char),7U);}),116U);});
({struct Cyc_AP_T*_tmp70=Cyc_mk(x->ndigits + y->ndigits);z=_tmp70;});
Cyc_XP_mul(((struct Cyc_AP_T*)_check_null(z))->digits,x->ndigits,x->digits,y->ndigits,y->digits);
# 120
Cyc_normalize(z,z->size);
z->sign=(z->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0  || (x->sign ^ y->sign)== 0)?1: - 1;
# 123
return z;}
# 125
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp71=({const char*_tmpE="x";_tag_dyneither(_tmpE,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp71,({const char*_tmpF="ap.cyc";_tag_dyneither(_tmpF,sizeof(char),7U);}),127U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp72=({const char*_tmp10="y";_tag_dyneither(_tmp10,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp72,({const char*_tmp11="ap.cyc";_tag_dyneither(_tmp11,sizeof(char),7U);}),128U);});
if((x->sign ^ y->sign)== 0){
({struct Cyc_AP_T*_tmp75=({struct Cyc_AP_T*_tmp74=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_tmp73=x;Cyc_add(_tmp74,_tmp73,y);});z=_tmp75;});
({int _tmp76=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp76;});}else{
# 133
if(Cyc_cmp(x,y)> 0){
({struct Cyc_AP_T*_tmp79=({struct Cyc_AP_T*_tmp78=Cyc_mk(x->ndigits);struct Cyc_AP_T*_tmp77=x;Cyc_sub(_tmp78,_tmp77,y);});z=_tmp79;});
({int _tmp7A=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp7A;});}else{
# 138
({struct Cyc_AP_T*_tmp7D=({struct Cyc_AP_T*_tmp7C=Cyc_mk(y->ndigits);struct Cyc_AP_T*_tmp7B=y;Cyc_sub(_tmp7C,_tmp7B,x);});z=_tmp7D;});
({int _tmp7E=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;z->sign=_tmp7E;});}}
# 141
return z;}
# 143
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp7F=({const char*_tmp12="x";_tag_dyneither(_tmp12,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp7F,({const char*_tmp13="ap.cyc";_tag_dyneither(_tmp13,sizeof(char),7U);}),145U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp80=({const char*_tmp14="y";_tag_dyneither(_tmp14,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp80,({const char*_tmp15="ap.cyc";_tag_dyneither(_tmp15,sizeof(char),7U);}),146U);});
if(!((x->sign ^ y->sign)== 0)){
({struct Cyc_AP_T*_tmp83=({struct Cyc_AP_T*_tmp82=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_tmp81=x;Cyc_add(_tmp82,_tmp81,y);});z=_tmp83;});
({int _tmp84=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp84;});}else{
# 151
if(Cyc_cmp(x,y)> 0){
({struct Cyc_AP_T*_tmp87=({struct Cyc_AP_T*_tmp86=Cyc_mk(x->ndigits);struct Cyc_AP_T*_tmp85=x;Cyc_sub(_tmp86,_tmp85,y);});z=_tmp87;});
({int _tmp88=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp88;});}else{
# 155
({struct Cyc_AP_T*_tmp8B=({struct Cyc_AP_T*_tmp8A=Cyc_mk(y->ndigits);struct Cyc_AP_T*_tmp89=y;Cyc_sub(_tmp8A,_tmp89,x);});z=_tmp8B;});
({int _tmp8C=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;z->sign=_tmp8C;});}}
# 158
return z;}
# 160
struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned int)x?0:({struct _dyneither_ptr _tmp8D=({const char*_tmp16="x";_tag_dyneither(_tmp16,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp8D,({const char*_tmp17="ap.cyc";_tag_dyneither(_tmp17,sizeof(char),7U);}),162U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp8E=({const char*_tmp18="y";_tag_dyneither(_tmp18,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp8E,({const char*_tmp19="ap.cyc";_tag_dyneither(_tmp19,sizeof(char),7U);}),163U);});
!(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)?0:({struct _dyneither_ptr _tmp8F=({const char*_tmp1A="!((y)->ndigits==1 && (y)->digits[0]==0)";_tag_dyneither(_tmp1A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp8F,({const char*_tmp1B="ap.cyc";_tag_dyneither(_tmp1B,sizeof(char),7U);}),164U);});
({struct Cyc_AP_T*_tmp90=Cyc_mk(x->ndigits);q=_tmp90;});
({struct Cyc_AP_T*_tmp91=Cyc_mk(y->ndigits);r=_tmp91;});
{
struct _dyneither_ptr tmp=({unsigned int _tmp1C=((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);unsigned char*_tmp1D=_cycalloc_atomic(_tmp1C);_tag_dyneither(_tmp1D,1U,_tmp1C);});
({int _tmp97=x->ndigits;struct _dyneither_ptr _tmp96=((struct Cyc_AP_T*)_check_null(q))->digits;struct _dyneither_ptr _tmp95=x->digits;int _tmp94=y->ndigits;struct _dyneither_ptr _tmp93=y->digits;struct _dyneither_ptr _tmp92=((struct Cyc_AP_T*)_check_null(r))->digits;Cyc_XP_div(_tmp97,_tmp96,_tmp95,_tmp94,_tmp93,_tmp92,tmp);});}
# 172
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
q->sign=(q->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(q->digits,sizeof(unsigned char),0))== 0  || (x->sign ^ y->sign)== 0)?1: - 1;
# 176
return q;}
# 178
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned int)x?0:({struct _dyneither_ptr _tmp98=({const char*_tmp1E="x";_tag_dyneither(_tmp1E,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp98,({const char*_tmp1F="ap.cyc";_tag_dyneither(_tmp1F,sizeof(char),7U);}),180U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp99=({const char*_tmp20="y";_tag_dyneither(_tmp20,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp99,({const char*_tmp21="ap.cyc";_tag_dyneither(_tmp21,sizeof(char),7U);}),181U);});
!(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)?0:({struct _dyneither_ptr _tmp9A=({const char*_tmp22="!((y)->ndigits==1 && (y)->digits[0]==0)";_tag_dyneither(_tmp22,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp9A,({const char*_tmp23="ap.cyc";_tag_dyneither(_tmp23,sizeof(char),7U);}),182U);});
({struct Cyc_AP_T*_tmp9B=Cyc_mk(x->ndigits);q=_tmp9B;});
({struct Cyc_AP_T*_tmp9C=Cyc_mk(y->ndigits);r=_tmp9C;});
{
struct _dyneither_ptr tmp=({unsigned int _tmp24=((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);unsigned char*_tmp25=_cycalloc_atomic(_tmp24);_tag_dyneither(_tmp25,1U,_tmp24);});
({int _tmpA2=x->ndigits;struct _dyneither_ptr _tmpA1=((struct Cyc_AP_T*)_check_null(q))->digits;struct _dyneither_ptr _tmpA0=x->digits;int _tmp9F=y->ndigits;struct _dyneither_ptr _tmp9E=y->digits;struct _dyneither_ptr _tmp9D=((struct Cyc_AP_T*)_check_null(r))->digits;Cyc_XP_div(_tmpA2,_tmpA1,_tmpA0,_tmp9F,_tmp9E,_tmp9D,tmp);});}
# 190
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
r->sign=r->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(r->digits,sizeof(unsigned char),0))== 0?1: x->sign;
return r;}
# 195
struct Cyc_AP_T*Cyc_AP_pow(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmpA3=({const char*_tmp26="x";_tag_dyneither(_tmp26,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpA3,({const char*_tmp27="ap.cyc";_tag_dyneither(_tmp27,sizeof(char),7U);}),197U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmpA4=({const char*_tmp28="y";_tag_dyneither(_tmp28,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpA4,({const char*_tmp29="ap.cyc";_tag_dyneither(_tmp29,sizeof(char),7U);}),198U);});
y->sign == 1?0:({struct _dyneither_ptr _tmpA5=({const char*_tmp2A="y->sign == 1";_tag_dyneither(_tmp2A,sizeof(char),13U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpA5,({const char*_tmp2B="ap.cyc";_tag_dyneither(_tmp2B,sizeof(char),7U);}),199U);});
(!((unsigned int)p) || (p->sign == 1  && !(p->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(p->digits,sizeof(unsigned char),0))== 0)) && !(p->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(p->digits,sizeof(unsigned char),0))== 1))?0:({struct _dyneither_ptr _tmpA6=({const char*_tmp2C="!p || p->sign==1 && !((p)->ndigits==1 && (p)->digits[0]==0) && !((p)->ndigits==1 && (p)->digits[0]==1)";_tag_dyneither(_tmp2C,sizeof(char),103U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpA6,({const char*_tmp2D="ap.cyc";_tag_dyneither(_tmp2D,sizeof(char),7U);}),200U);});
if(x->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(x->digits,sizeof(unsigned char),0))== 0)
return Cyc_AP_new(0);
if(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)
return Cyc_AP_new(1);
if(x->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(x->digits,sizeof(unsigned char),0))== 1)
return Cyc_AP_new((*((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0?1: x->sign);
if((unsigned int)p){
if(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 1)
({struct Cyc_AP_T*_tmpA7=Cyc_AP_mod(x,p);z=_tmpA7;});else{
# 211
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,p);
({struct Cyc_AP_T*_tmpA8=Cyc_mulmod(t,t,p);z=_tmpA8;});
if(!((*((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0))
({struct Cyc_AP_T*_tmpAC=({struct Cyc_AP_T*_tmpAB=({struct Cyc_AP_T*_tmpA9=Cyc_AP_mod(x,p);y2=_tmpA9;});struct Cyc_AP_T*_tmpAA=t=z;Cyc_mulmod(_tmpAB,_tmpAA,p);});z=_tmpAC;});}}else{
# 218
if(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 1)
({struct Cyc_AP_T*_tmpAD=Cyc_AP_addi(x,0);z=_tmpAD;});else{
# 221
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,0);
({struct Cyc_AP_T*_tmpAE=Cyc_AP_mul(t,t);z=_tmpAE;});
if(!((*((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0))
({struct Cyc_AP_T*_tmpB0=({struct Cyc_AP_T*_tmpAF=x;Cyc_AP_mul(_tmpAF,(t=z));});z=_tmpB0;});}}
# 227
return z;}
# 229
int Cyc_AP_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
(unsigned int)x?0:({struct _dyneither_ptr _tmpB1=({const char*_tmp2E="x";_tag_dyneither(_tmp2E,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpB1,({const char*_tmp2F="ap.cyc";_tag_dyneither(_tmp2F,sizeof(char),7U);}),230U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmpB2=({const char*_tmp30="y";_tag_dyneither(_tmp30,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpB2,({const char*_tmp31="ap.cyc";_tag_dyneither(_tmp31,sizeof(char),7U);}),231U);});
if(!((x->sign ^ y->sign)== 0))
return x->sign;else{
if(x->sign == 1)
return Cyc_cmp(x,y);else{
# 237
return Cyc_cmp(y,x);}}}
# 239
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB3=x;Cyc_AP_add(_tmpB3,Cyc_set(t,y));});}
# 243
struct Cyc_AP_T*Cyc_AP_subi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB4=x;Cyc_AP_sub(_tmpB4,Cyc_set(t,y));});}
# 247
struct Cyc_AP_T*Cyc_AP_muli(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB5=x;Cyc_AP_mul(_tmpB5,Cyc_set(t,y));});}
# 251
struct Cyc_AP_T*Cyc_AP_divi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB6=x;Cyc_AP_div(_tmpB6,Cyc_set(t,y));});}
# 255
int Cyc_AP_cmpi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB7=x;Cyc_AP_cmp(_tmpB7,Cyc_set(t,y));});}
# 259
long Cyc_AP_modi(struct Cyc_AP_T*x,long y){
long rem;
struct Cyc_AP_T*r;
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
({struct Cyc_AP_T*_tmpB9=({struct Cyc_AP_T*_tmpB8=x;Cyc_AP_mod(_tmpB8,Cyc_set(t,y));});r=_tmpB9;});
({long _tmpBA=(long)Cyc_XP_toint(((struct Cyc_AP_T*)_check_null(r))->ndigits,r->digits);rem=_tmpBA;});
return rem;}
# 267
struct Cyc_AP_T*Cyc_AP_lshift(struct Cyc_AP_T*x,int s){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmpBB=({const char*_tmp32="x";_tag_dyneither(_tmp32,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpBB,({const char*_tmp33="ap.cyc";_tag_dyneither(_tmp33,sizeof(char),7U);}),269U);});
s >= 0?0:({struct _dyneither_ptr _tmpBC=({const char*_tmp34="s >= 0";_tag_dyneither(_tmp34,sizeof(char),7U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpBC,({const char*_tmp35="ap.cyc";_tag_dyneither(_tmp35,sizeof(char),7U);}),270U);});
({struct Cyc_AP_T*_tmpBD=Cyc_mk(x->ndigits + (s + 7 & ~ 7)/ 8);z=_tmpBD;});
Cyc_XP_lshift(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,x->ndigits,x->digits,s,0);
# 274
z->sign=x->sign;
return Cyc_normalize(z,z->size);}
# 277
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*x,int s){
(unsigned int)x?0:({struct _dyneither_ptr _tmpBE=({const char*_tmp36="x";_tag_dyneither(_tmp36,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpBE,({const char*_tmp37="ap.cyc";_tag_dyneither(_tmp37,sizeof(char),7U);}),278U);});
s >= 0?0:({struct _dyneither_ptr _tmpBF=({const char*_tmp38="s >= 0";_tag_dyneither(_tmp38,sizeof(char),7U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpBF,({const char*_tmp39="ap.cyc";_tag_dyneither(_tmp39,sizeof(char),7U);}),279U);});
if(s >= 8 * x->ndigits)
return Cyc_AP_new(0);else{
# 283
struct Cyc_AP_T*z=Cyc_mk(x->ndigits - s / 8);
Cyc_XP_rshift(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,x->ndigits,x->digits,s,0);
# 286
Cyc_normalize(z,z->size);
z->sign=z->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;
return z;}}
# 291
struct Cyc_AP_T*Cyc_AP_and(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpC0=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpC0 < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_and(y,x);
({struct Cyc_AP_T*_tmpC1=Cyc_mk(y->ndigits);z=_tmpC1;});
Cyc_XP_and(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
Cyc_normalize(z,z->size);
z->sign=1;
return z;}
# 302
struct Cyc_AP_T*Cyc_AP_or(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpC2=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpC2 < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_or(y,x);
({struct Cyc_AP_T*_tmpC3=Cyc_mk(x->ndigits);z=_tmpC3;});
Cyc_XP_or(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
for(i=y->ndigits;i < x->ndigits;++ i){
Cyc_normalize(z,z->size);}
z->sign=1;
return z;}
# 314
struct Cyc_AP_T*Cyc_AP_xor(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpC4=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpC4 < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_xor(y,x);
({struct Cyc_AP_T*_tmpC5=Cyc_mk(x->ndigits);z=_tmpC5;});
Cyc_XP_xor(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
for(i=y->ndigits;i < x->ndigits;++ i){
Cyc_normalize(z,z->size);}
z->sign=1;
return z;}
# 326
struct Cyc_AP_T*Cyc_AP_fromint(long x){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return Cyc_set(t,x);}
# 330
long Cyc_AP_toint(struct Cyc_AP_T*x){
unsigned long u;
(unsigned int)x?0:({struct _dyneither_ptr _tmpC6=({const char*_tmp3A="x";_tag_dyneither(_tmp3A,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpC6,({const char*_tmp3B="ap.cyc";_tag_dyneither(_tmp3B,sizeof(char),7U);}),332U);});
({unsigned long _tmpC7=Cyc_XP_toint(x->ndigits,x->digits)% (2147483647 + 1U);u=_tmpC7;});
if(x->sign == - 1)
return -(long)u;else{
# 337
return(long)u;}}
# 339
struct Cyc_AP_T*Cyc_AP_fromstr(const char*str,int base){
struct Cyc_AP_T*z;
struct _dyneither_ptr p=({const char*_tmp43=str;_tag_dyneither(_tmp43,sizeof(char),_get_zero_arr_size_char((void*)_tmp43,1U));});
char sign='\000';
int carry;
(unsigned int)p.curr?0:({struct _dyneither_ptr _tmpC8=({const char*_tmp3C="p";_tag_dyneither(_tmp3C,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpC8,({const char*_tmp3D="ap.cyc";_tag_dyneither(_tmp3D,sizeof(char),7U);}),344U);});
base >= 2  && base <= 36?0:({struct _dyneither_ptr _tmpC9=({const char*_tmp3E="base >= 2 && base <= 36";_tag_dyneither(_tmp3E,sizeof(char),24U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpC9,({const char*_tmp3F="ap.cyc";_tag_dyneither(_tmp3F,sizeof(char),7U);}),345U);});
while((int)*((const char*)_check_dyneither_subscript(p,sizeof(char),0U)) &&  isspace((int)*((const char*)_check_dyneither_subscript(p,sizeof(char),0U)))){
_dyneither_ptr_inplace_plus(& p,sizeof(char),1);}
if(*((const char*)_check_dyneither_subscript(p,sizeof(char),0U))== '-'  || *((const char*)_check_dyneither_subscript(p,sizeof(char),0U))== '+')
sign=*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),sizeof(char),0U));
{
const char*start;
int k;int n=0;
for(0;*((const char*)_check_dyneither_subscript(p,sizeof(char),0U))== '0'  && *((const char*)_check_dyneither_subscript(p,sizeof(char),1))== '0';_dyneither_ptr_inplace_plus(& p,sizeof(char),1)){
;}
start=(const char*)_untag_dyneither_ptr(p,sizeof(char),1U);
for(0;(('0' <= *((const char*)_check_dyneither_subscript(p,sizeof(char),0U)) && *((const char*)_check_dyneither_subscript(p,sizeof(char),0U))<= '9') && *((const char*)_check_dyneither_subscript(p,sizeof(char),0U))< '0' + base  || 
('a' <= *((const char*)_check_dyneither_subscript(p,sizeof(char),0U)) && *((const char*)_check_dyneither_subscript(p,sizeof(char),0U))<= 'z') && *((const char*)_check_dyneither_subscript(p,sizeof(char),0U))< ('a' + base)- 10) || 
('A' <= *((const char*)_check_dyneither_subscript(p,sizeof(char),0U)) && *((const char*)_check_dyneither_subscript(p,sizeof(char),0U))<= 'Z') && *((const char*)_check_dyneither_subscript(p,sizeof(char),0U))< ('A' + base)- 10;_dyneither_ptr_inplace_plus(& p,sizeof(char),1)){
++ n;}
for(k=1;1 << k < base;++ k){
;}
({struct Cyc_AP_T*_tmpCA=Cyc_mk((k * n + 7 & ~ 7)/ 8);z=_tmpCA;});
({struct _dyneither_ptr _tmpCB=({const char*_tmp40=start;_tag_dyneither(_tmp40,sizeof(char),_get_zero_arr_size_char((void*)_tmp40,1U));});p=_tmpCB;});}
# 365
({int _tmpCF=({int _tmpCE=((struct Cyc_AP_T*)_check_null(z))->size;struct _dyneither_ptr _tmpCD=z->digits;const char*_tmpCC=(const char*)_untag_dyneither_ptr(p,sizeof(char),1U);Cyc_XP_fromstr(_tmpCE,_tmpCD,_tmpCC,base);});carry=_tmpCF;});
# 367
carry == 0?0:({struct _dyneither_ptr _tmpD0=({const char*_tmp41="carry == 0";_tag_dyneither(_tmp41,sizeof(char),11U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpD0,({const char*_tmp42="ap.cyc";_tag_dyneither(_tmp42,sizeof(char),7U);}),367U);});
Cyc_normalize(z,z->size);
z->sign=(z->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0  || sign != '-')?1: - 1;
return z;}
# 372
char*Cyc_AP_tostr(struct Cyc_AP_T*x,int base){
struct _dyneither_ptr q;
struct _dyneither_ptr str;
int size;int k;
(unsigned int)x?0:({struct _dyneither_ptr _tmpD1=({const char*_tmp44="x";_tag_dyneither(_tmp44,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpD1,({const char*_tmp45="ap.cyc";_tag_dyneither(_tmp45,sizeof(char),7U);}),376U);});
base >= 2  && base <= 36?0:({struct _dyneither_ptr _tmpD2=({const char*_tmp46="base >= 2 && base <= 36";_tag_dyneither(_tmp46,sizeof(char),24U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpD2,({const char*_tmp47="ap.cyc";_tag_dyneither(_tmp47,sizeof(char),7U);}),377U);});
for(k=5;1 << k > base;-- k){
;}
size=((8 * x->ndigits)/ k + 1)+ 1;
if(x->sign == - 1)
++ size;
({struct _dyneither_ptr _tmpD3=({unsigned int _tmp48=size;char*_tmp49=_cyccalloc_atomic(sizeof(char),_tmp48);_tag_dyneither(_tmp49,sizeof(char),_tmp48);});str=_tmpD3;});
({struct _dyneither_ptr _tmpD4=({unsigned int _tmp4A=x->ndigits * sizeof(unsigned char);unsigned char*_tmp4B=_cycalloc_atomic(_tmp4A);_tag_dyneither(_tmp4B,1U,_tmp4A);});q=_tmpD4;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(q,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
if(x->sign == - 1){
({struct _dyneither_ptr _tmp4C=_dyneither_ptr_plus(str,sizeof(char),0);char _tmp4D=*((char*)_check_dyneither_subscript(_tmp4C,sizeof(char),0U));char _tmp4E='-';if(_get_dyneither_size(_tmp4C,sizeof(char))== 1U  && (_tmp4D == '\000'  && _tmp4E != '\000'))_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});
({struct _dyneither_ptr _tmpD8=_dyneither_ptr_plus(str,sizeof(char),1);int _tmpD7=size - 1;int _tmpD6=base;int _tmpD5=x->ndigits;Cyc_XP_tostr(_tmpD8,_tmpD7,_tmpD6,_tmpD5,q);});}else{
# 390
Cyc_XP_tostr(str,size,base,x->ndigits,q);}
return(char*)_untag_dyneither_ptr(str,sizeof(char),1U);}
# 393
struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;struct Cyc_AP_T*tmp;
(unsigned int)x?0:({struct _dyneither_ptr _tmpD9=({const char*_tmp4F="x";_tag_dyneither(_tmp4F,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpD9,({const char*_tmp50="ap.cyc";_tag_dyneither(_tmp50,sizeof(char),7U);}),395U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmpDA=({const char*_tmp51="y";_tag_dyneither(_tmp51,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpDA,({const char*_tmp52="ap.cyc";_tag_dyneither(_tmp52,sizeof(char),7U);}),396U);});
while(!(((struct Cyc_AP_T*)_check_null(y))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)){
({struct Cyc_AP_T*_tmpDB=Cyc_AP_mod(x,y);tmp=_tmpDB;});
x=y;
y=tmp;}
# 402
({struct Cyc_AP_T*_tmpDC=Cyc_mk(x->ndigits);z=_tmpDC;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=x->sign;
return z;}
# 408
struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
return({struct Cyc_AP_T*_tmpDE=x;Cyc_AP_mul(_tmpDE,({struct Cyc_AP_T*_tmpDD=y;Cyc_AP_div(_tmpDD,Cyc_AP_gcd(x,y));}));});}
