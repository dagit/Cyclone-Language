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
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
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
 static char Cyc_rcsid[52U]="$Id: ap.c,v 1.9 2004-07-28 21:46:17 djg Exp $";
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
({struct Cyc_AP_T*_tmp4E=Cyc_AP_fromint(0);Cyc_AP_zero=_tmp4E;});
({struct Cyc_AP_T*_tmp4F=Cyc_AP_fromint(1);Cyc_AP_one=_tmp4F;});}
# 32
static struct Cyc_AP_T*Cyc_mk(int size){
struct Cyc_AP_T*z;
if(!Cyc_init)Cyc_AP_init();
({struct Cyc_AP_T*_tmp50=_cycalloc(sizeof(struct Cyc_AP_T));z=_tmp50;});
({struct _dyneither_ptr _tmp51=({unsigned int _tmp0=size;_tag_dyneither(_cyccalloc_atomic(sizeof(unsigned char),_tmp0),sizeof(unsigned char),_tmp0);});z->digits=_tmp51;});
size > 0?0:({struct _dyneither_ptr _tmp52=({const char*_tmp1="size > 0";_tag_dyneither(_tmp1,sizeof(char),9U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp52,({const char*_tmp2="ap.cyc";_tag_dyneither(_tmp2,sizeof(char),7U);}),37U);});
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
({int _tmp53=Cyc_XP_length(n,((struct Cyc_AP_T*)_check_null(z))->digits);z->ndigits=_tmp53;});
return z;}
# 57
static struct Cyc_AP_T*Cyc_add(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int n=((struct Cyc_AP_T*)_check_null(y))->ndigits;
if(((struct Cyc_AP_T*)_check_null(x))->ndigits < n)
return Cyc_add(z,y,x);else{
if(x->ndigits > n){
int carry=Cyc_XP_add(n,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits,0);
# 64
({unsigned char _tmp57=(unsigned char)({int _tmp56=x->ndigits - n;struct _dyneither_ptr _tmp55=
_dyneither_ptr_plus(z->digits,sizeof(unsigned char),n);
# 64
struct _dyneither_ptr _tmp54=
_dyneither_ptr_plus(x->digits,sizeof(unsigned char),n);
# 64
Cyc_XP_sum(_tmp56,_tmp55,_tmp54,carry);});*((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),z->size - 1))=_tmp57;});}else{
# 67
({unsigned char _tmp58=(unsigned char)Cyc_XP_add(n,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits,0);*((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),n))=_tmp58;});}}
# 69
return Cyc_normalize(z,z->size);}
# 71
static struct Cyc_AP_T*Cyc_sub(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int borrow;int n=((struct Cyc_AP_T*)_check_null(y))->ndigits;
({int _tmp5C=({int _tmp5B=n;struct _dyneither_ptr _tmp5A=((struct Cyc_AP_T*)_check_null(z))->digits;struct _dyneither_ptr _tmp59=((struct Cyc_AP_T*)_check_null(x))->digits;Cyc_XP_sub(_tmp5B,_tmp5A,_tmp59,y->digits,0);});borrow=_tmp5C;});
# 75
if(x->ndigits > n)
({int _tmp60=({int _tmp5F=x->ndigits - n;struct _dyneither_ptr _tmp5E=_dyneither_ptr_plus(z->digits,sizeof(unsigned char),n);struct _dyneither_ptr _tmp5D=
_dyneither_ptr_plus(x->digits,sizeof(unsigned char),n);
# 76
Cyc_XP_diff(_tmp5F,_tmp5E,_tmp5D,borrow);});borrow=_tmp60;});
# 78
borrow == 0?0:({struct _dyneither_ptr _tmp61=({const char*_tmp3="borrow == 0";_tag_dyneither(_tmp3,sizeof(char),12U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp61,({const char*_tmp4="ap.cyc";_tag_dyneither(_tmp4,sizeof(char),7U);}),78U);});
return Cyc_normalize(z,z->size);}
# 81
static struct Cyc_AP_T*Cyc_mulmod(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;struct Cyc_AP_T*xy=Cyc_AP_mul(x,y);
({struct Cyc_AP_T*_tmp62=Cyc_AP_mod(xy,p);z=_tmp62;});
return z;}
# 86
static int Cyc_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
if(({int _tmp63=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmp63 != ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return x->ndigits - y->ndigits;else{
# 90
return Cyc_XP_cmp(x->ndigits,x->digits,y->digits);}}
# 92
struct Cyc_AP_T*Cyc_AP_new(long n){
return({struct Cyc_AP_T*_tmp64=Cyc_mk((int)sizeof(long));Cyc_set(_tmp64,n);});}
# 95
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp65=({const char*_tmp5="x";_tag_dyneither(_tmp5,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp65,({const char*_tmp6="ap.cyc";_tag_dyneither(_tmp6,sizeof(char),7U);}),97U);});
({struct Cyc_AP_T*_tmp66=Cyc_mk(x->ndigits);z=_tmp66;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=z->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;
return z;}
# 104
struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp67=({const char*_tmp7="x";_tag_dyneither(_tmp7,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp67,({const char*_tmp8="ap.cyc";_tag_dyneither(_tmp8,sizeof(char),7U);}),106U);});
({struct Cyc_AP_T*_tmp68=Cyc_mk(x->ndigits);z=_tmp68;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=1;
return z;}
# 113
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp69=({const char*_tmp9="x";_tag_dyneither(_tmp9,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp69,({const char*_tmpA="ap.cyc";_tag_dyneither(_tmpA,sizeof(char),7U);}),115U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp6A=({const char*_tmpB="y";_tag_dyneither(_tmpB,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp6A,({const char*_tmpC="ap.cyc";_tag_dyneither(_tmpC,sizeof(char),7U);}),116U);});
({struct Cyc_AP_T*_tmp6B=Cyc_mk(x->ndigits + y->ndigits);z=_tmp6B;});
Cyc_XP_mul(((struct Cyc_AP_T*)_check_null(z))->digits,x->ndigits,x->digits,y->ndigits,y->digits);
# 120
Cyc_normalize(z,z->size);
z->sign=(z->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0  || (x->sign ^ y->sign)== 0)?1: - 1;
# 123
return z;}
# 125
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp6C=({const char*_tmpD="x";_tag_dyneither(_tmpD,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp6C,({const char*_tmpE="ap.cyc";_tag_dyneither(_tmpE,sizeof(char),7U);}),127U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp6D=({const char*_tmpF="y";_tag_dyneither(_tmpF,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp6D,({const char*_tmp10="ap.cyc";_tag_dyneither(_tmp10,sizeof(char),7U);}),128U);});
if((x->sign ^ y->sign)== 0){
({struct Cyc_AP_T*_tmp70=({struct Cyc_AP_T*_tmp6F=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_tmp6E=x;Cyc_add(_tmp6F,_tmp6E,y);});z=_tmp70;});
({int _tmp71=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp71;});}else{
# 133
if(Cyc_cmp(x,y)> 0){
({struct Cyc_AP_T*_tmp74=({struct Cyc_AP_T*_tmp73=Cyc_mk(x->ndigits);struct Cyc_AP_T*_tmp72=x;Cyc_sub(_tmp73,_tmp72,y);});z=_tmp74;});
({int _tmp75=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp75;});}else{
# 138
({struct Cyc_AP_T*_tmp78=({struct Cyc_AP_T*_tmp77=Cyc_mk(y->ndigits);struct Cyc_AP_T*_tmp76=y;Cyc_sub(_tmp77,_tmp76,x);});z=_tmp78;});
({int _tmp79=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;z->sign=_tmp79;});}}
# 141
return z;}
# 143
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp7A=({const char*_tmp11="x";_tag_dyneither(_tmp11,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp7A,({const char*_tmp12="ap.cyc";_tag_dyneither(_tmp12,sizeof(char),7U);}),145U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp7B=({const char*_tmp13="y";_tag_dyneither(_tmp13,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp7B,({const char*_tmp14="ap.cyc";_tag_dyneither(_tmp14,sizeof(char),7U);}),146U);});
if(!((x->sign ^ y->sign)== 0)){
({struct Cyc_AP_T*_tmp7E=({struct Cyc_AP_T*_tmp7D=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_tmp7C=x;Cyc_add(_tmp7D,_tmp7C,y);});z=_tmp7E;});
({int _tmp7F=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp7F;});}else{
# 151
if(Cyc_cmp(x,y)> 0){
({struct Cyc_AP_T*_tmp82=({struct Cyc_AP_T*_tmp81=Cyc_mk(x->ndigits);struct Cyc_AP_T*_tmp80=x;Cyc_sub(_tmp81,_tmp80,y);});z=_tmp82;});
({int _tmp83=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp83;});}else{
# 155
({struct Cyc_AP_T*_tmp86=({struct Cyc_AP_T*_tmp85=Cyc_mk(y->ndigits);struct Cyc_AP_T*_tmp84=y;Cyc_sub(_tmp85,_tmp84,x);});z=_tmp86;});
({int _tmp87=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;z->sign=_tmp87;});}}
# 158
return z;}
# 160
struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned int)x?0:({struct _dyneither_ptr _tmp88=({const char*_tmp15="x";_tag_dyneither(_tmp15,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp88,({const char*_tmp16="ap.cyc";_tag_dyneither(_tmp16,sizeof(char),7U);}),162U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp89=({const char*_tmp17="y";_tag_dyneither(_tmp17,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp89,({const char*_tmp18="ap.cyc";_tag_dyneither(_tmp18,sizeof(char),7U);}),163U);});
!(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)?0:({struct _dyneither_ptr _tmp8A=({const char*_tmp19="!((y)->ndigits==1 && (y)->digits[0]==0)";_tag_dyneither(_tmp19,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp8A,({const char*_tmp1A="ap.cyc";_tag_dyneither(_tmp1A,sizeof(char),7U);}),164U);});
({struct Cyc_AP_T*_tmp8B=Cyc_mk(x->ndigits);q=_tmp8B;});
({struct Cyc_AP_T*_tmp8C=Cyc_mk(y->ndigits);r=_tmp8C;});
{
struct _dyneither_ptr tmp=({unsigned int _tmp1B=((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);_tag_dyneither(_cycalloc_atomic(_tmp1B),1U,_tmp1B);});
({int _tmp92=x->ndigits;struct _dyneither_ptr _tmp91=((struct Cyc_AP_T*)_check_null(q))->digits;struct _dyneither_ptr _tmp90=x->digits;int _tmp8F=y->ndigits;struct _dyneither_ptr _tmp8E=y->digits;struct _dyneither_ptr _tmp8D=((struct Cyc_AP_T*)_check_null(r))->digits;Cyc_XP_div(_tmp92,_tmp91,_tmp90,_tmp8F,_tmp8E,_tmp8D,tmp);});}
# 172
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
q->sign=(q->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(q->digits,sizeof(unsigned char),0))== 0  || (x->sign ^ y->sign)== 0)?1: - 1;
# 176
return q;}
# 178
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned int)x?0:({struct _dyneither_ptr _tmp93=({const char*_tmp1C="x";_tag_dyneither(_tmp1C,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp93,({const char*_tmp1D="ap.cyc";_tag_dyneither(_tmp1D,sizeof(char),7U);}),180U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp94=({const char*_tmp1E="y";_tag_dyneither(_tmp1E,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp94,({const char*_tmp1F="ap.cyc";_tag_dyneither(_tmp1F,sizeof(char),7U);}),181U);});
!(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)?0:({struct _dyneither_ptr _tmp95=({const char*_tmp20="!((y)->ndigits==1 && (y)->digits[0]==0)";_tag_dyneither(_tmp20,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp95,({const char*_tmp21="ap.cyc";_tag_dyneither(_tmp21,sizeof(char),7U);}),182U);});
({struct Cyc_AP_T*_tmp96=Cyc_mk(x->ndigits);q=_tmp96;});
({struct Cyc_AP_T*_tmp97=Cyc_mk(y->ndigits);r=_tmp97;});
{
struct _dyneither_ptr tmp=({unsigned int _tmp22=((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);_tag_dyneither(_cycalloc_atomic(_tmp22),1U,_tmp22);});
({int _tmp9D=x->ndigits;struct _dyneither_ptr _tmp9C=((struct Cyc_AP_T*)_check_null(q))->digits;struct _dyneither_ptr _tmp9B=x->digits;int _tmp9A=y->ndigits;struct _dyneither_ptr _tmp99=y->digits;struct _dyneither_ptr _tmp98=((struct Cyc_AP_T*)_check_null(r))->digits;Cyc_XP_div(_tmp9D,_tmp9C,_tmp9B,_tmp9A,_tmp99,_tmp98,tmp);});}
# 190
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
r->sign=r->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(r->digits,sizeof(unsigned char),0))== 0?1: x->sign;
return r;}
# 195
struct Cyc_AP_T*Cyc_AP_pow(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmp9E=({const char*_tmp23="x";_tag_dyneither(_tmp23,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp9E,({const char*_tmp24="ap.cyc";_tag_dyneither(_tmp24,sizeof(char),7U);}),197U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmp9F=({const char*_tmp25="y";_tag_dyneither(_tmp25,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp9F,({const char*_tmp26="ap.cyc";_tag_dyneither(_tmp26,sizeof(char),7U);}),198U);});
y->sign == 1?0:({struct _dyneither_ptr _tmpA0=({const char*_tmp27="y->sign == 1";_tag_dyneither(_tmp27,sizeof(char),13U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpA0,({const char*_tmp28="ap.cyc";_tag_dyneither(_tmp28,sizeof(char),7U);}),199U);});
(!((unsigned int)p) || (p->sign == 1  && !(p->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(p->digits,sizeof(unsigned char),0))== 0)) && !(p->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(p->digits,sizeof(unsigned char),0))== 1))?0:({struct _dyneither_ptr _tmpA1=({const char*_tmp29="!p || p->sign==1 && !((p)->ndigits==1 && (p)->digits[0]==0) && !((p)->ndigits==1 && (p)->digits[0]==1)";_tag_dyneither(_tmp29,sizeof(char),103U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpA1,({const char*_tmp2A="ap.cyc";_tag_dyneither(_tmp2A,sizeof(char),7U);}),200U);});
if(x->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(x->digits,sizeof(unsigned char),0))== 0)
return Cyc_AP_new(0);
if(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)
return Cyc_AP_new(1);
if(x->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(x->digits,sizeof(unsigned char),0))== 1)
return Cyc_AP_new((*((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0?1: x->sign);
if((unsigned int)p){
if(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 1)
({struct Cyc_AP_T*_tmpA2=Cyc_AP_mod(x,p);z=_tmpA2;});else{
# 211
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,p);
({struct Cyc_AP_T*_tmpA3=Cyc_mulmod(t,t,p);z=_tmpA3;});
if(!((*((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0))
({struct Cyc_AP_T*_tmpA7=({struct Cyc_AP_T*_tmpA6=({struct Cyc_AP_T*_tmpA4=Cyc_AP_mod(x,p);y2=_tmpA4;});struct Cyc_AP_T*_tmpA5=t=z;Cyc_mulmod(_tmpA6,_tmpA5,p);});z=_tmpA7;});}}else{
# 218
if(y->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 1)
({struct Cyc_AP_T*_tmpA8=Cyc_AP_addi(x,0);z=_tmpA8;});else{
# 221
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,0);
({struct Cyc_AP_T*_tmpA9=Cyc_AP_mul(t,t);z=_tmpA9;});
if(!((*((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0))
({struct Cyc_AP_T*_tmpAB=({struct Cyc_AP_T*_tmpAA=x;Cyc_AP_mul(_tmpAA,(t=z));});z=_tmpAB;});}}
# 227
return z;}
# 229
int Cyc_AP_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
(unsigned int)x?0:({struct _dyneither_ptr _tmpAC=({const char*_tmp2B="x";_tag_dyneither(_tmp2B,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpAC,({const char*_tmp2C="ap.cyc";_tag_dyneither(_tmp2C,sizeof(char),7U);}),230U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmpAD=({const char*_tmp2D="y";_tag_dyneither(_tmp2D,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpAD,({const char*_tmp2E="ap.cyc";_tag_dyneither(_tmp2E,sizeof(char),7U);}),231U);});
if(!((x->sign ^ y->sign)== 0))
return x->sign;else{
if(x->sign == 1)
return Cyc_cmp(x,y);else{
# 237
return Cyc_cmp(y,x);}}}
# 239
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpAE=x;Cyc_AP_add(_tmpAE,Cyc_set(t,y));});}
# 243
struct Cyc_AP_T*Cyc_AP_subi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpAF=x;Cyc_AP_sub(_tmpAF,Cyc_set(t,y));});}
# 247
struct Cyc_AP_T*Cyc_AP_muli(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB0=x;Cyc_AP_mul(_tmpB0,Cyc_set(t,y));});}
# 251
struct Cyc_AP_T*Cyc_AP_divi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB1=x;Cyc_AP_div(_tmpB1,Cyc_set(t,y));});}
# 255
int Cyc_AP_cmpi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpB2=x;Cyc_AP_cmp(_tmpB2,Cyc_set(t,y));});}
# 259
long Cyc_AP_modi(struct Cyc_AP_T*x,long y){
long rem;
struct Cyc_AP_T*r;
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
({struct Cyc_AP_T*_tmpB4=({struct Cyc_AP_T*_tmpB3=x;Cyc_AP_mod(_tmpB3,Cyc_set(t,y));});r=_tmpB4;});
({long _tmpB5=(long)Cyc_XP_toint(((struct Cyc_AP_T*)_check_null(r))->ndigits,r->digits);rem=_tmpB5;});
return rem;}
# 267
struct Cyc_AP_T*Cyc_AP_lshift(struct Cyc_AP_T*x,int s){
struct Cyc_AP_T*z;
(unsigned int)x?0:({struct _dyneither_ptr _tmpB6=({const char*_tmp2F="x";_tag_dyneither(_tmp2F,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpB6,({const char*_tmp30="ap.cyc";_tag_dyneither(_tmp30,sizeof(char),7U);}),269U);});
s >= 0?0:({struct _dyneither_ptr _tmpB7=({const char*_tmp31="s >= 0";_tag_dyneither(_tmp31,sizeof(char),7U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpB7,({const char*_tmp32="ap.cyc";_tag_dyneither(_tmp32,sizeof(char),7U);}),270U);});
({struct Cyc_AP_T*_tmpB8=Cyc_mk(x->ndigits + (s + 7 & ~ 7)/ 8);z=_tmpB8;});
Cyc_XP_lshift(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,x->ndigits,x->digits,s,0);
# 274
z->sign=x->sign;
return Cyc_normalize(z,z->size);}
# 277
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*x,int s){
(unsigned int)x?0:({struct _dyneither_ptr _tmpB9=({const char*_tmp33="x";_tag_dyneither(_tmp33,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpB9,({const char*_tmp34="ap.cyc";_tag_dyneither(_tmp34,sizeof(char),7U);}),278U);});
s >= 0?0:({struct _dyneither_ptr _tmpBA=({const char*_tmp35="s >= 0";_tag_dyneither(_tmp35,sizeof(char),7U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpBA,({const char*_tmp36="ap.cyc";_tag_dyneither(_tmp36,sizeof(char),7U);}),279U);});
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
if(({int _tmpBB=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpBB < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_and(y,x);
({struct Cyc_AP_T*_tmpBC=Cyc_mk(y->ndigits);z=_tmpBC;});
Cyc_XP_and(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
Cyc_normalize(z,z->size);
z->sign=1;
return z;}
# 302
struct Cyc_AP_T*Cyc_AP_or(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpBD=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpBD < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_or(y,x);
({struct Cyc_AP_T*_tmpBE=Cyc_mk(x->ndigits);z=_tmpBE;});
Cyc_XP_or(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
for(i=y->ndigits;i < x->ndigits;++ i){
Cyc_normalize(z,z->size);}
z->sign=1;
return z;}
# 314
struct Cyc_AP_T*Cyc_AP_xor(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpBF=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpBF < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_xor(y,x);
({struct Cyc_AP_T*_tmpC0=Cyc_mk(x->ndigits);z=_tmpC0;});
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
(unsigned int)x?0:({struct _dyneither_ptr _tmpC1=({const char*_tmp37="x";_tag_dyneither(_tmp37,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpC1,({const char*_tmp38="ap.cyc";_tag_dyneither(_tmp38,sizeof(char),7U);}),332U);});
({unsigned long _tmpC2=Cyc_XP_toint(x->ndigits,x->digits)% (2147483647 + 1U);u=_tmpC2;});
if(x->sign == - 1)
return -(long)u;else{
# 337
return(long)u;}}
# 339
struct Cyc_AP_T*Cyc_AP_fromstr(const char*str,int base){
struct Cyc_AP_T*z;
struct _dyneither_ptr p=({const char*_tmp40=str;_tag_dyneither(_tmp40,sizeof(char),_get_zero_arr_size_char((void*)_tmp40,1U));});
char sign='\000';
int carry;
(unsigned int)p.curr?0:({struct _dyneither_ptr _tmpC3=({const char*_tmp39="p";_tag_dyneither(_tmp39,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpC3,({const char*_tmp3A="ap.cyc";_tag_dyneither(_tmp3A,sizeof(char),7U);}),344U);});
base >= 2  && base <= 36?0:({struct _dyneither_ptr _tmpC4=({const char*_tmp3B="base >= 2 && base <= 36";_tag_dyneither(_tmp3B,sizeof(char),24U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpC4,({const char*_tmp3C="ap.cyc";_tag_dyneither(_tmp3C,sizeof(char),7U);}),345U);});
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
({struct Cyc_AP_T*_tmpC5=Cyc_mk((k * n + 7 & ~ 7)/ 8);z=_tmpC5;});
({struct _dyneither_ptr _tmpC6=({const char*_tmp3D=start;_tag_dyneither(_tmp3D,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D,1U));});p=_tmpC6;});}
# 365
({int _tmpCA=({int _tmpC9=((struct Cyc_AP_T*)_check_null(z))->size;struct _dyneither_ptr _tmpC8=z->digits;const char*_tmpC7=(const char*)_untag_dyneither_ptr(p,sizeof(char),1U);Cyc_XP_fromstr(_tmpC9,_tmpC8,_tmpC7,base);});carry=_tmpCA;});
# 367
carry == 0?0:({struct _dyneither_ptr _tmpCB=({const char*_tmp3E="carry == 0";_tag_dyneither(_tmp3E,sizeof(char),11U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpCB,({const char*_tmp3F="ap.cyc";_tag_dyneither(_tmp3F,sizeof(char),7U);}),367U);});
Cyc_normalize(z,z->size);
z->sign=(z->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(z->digits,sizeof(unsigned char),0))== 0  || sign != '-')?1: - 1;
return z;}
# 372
char*Cyc_AP_tostr(struct Cyc_AP_T*x,int base){
struct _dyneither_ptr q;
struct _dyneither_ptr str;
int size;int k;
(unsigned int)x?0:({struct _dyneither_ptr _tmpCC=({const char*_tmp41="x";_tag_dyneither(_tmp41,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpCC,({const char*_tmp42="ap.cyc";_tag_dyneither(_tmp42,sizeof(char),7U);}),376U);});
base >= 2  && base <= 36?0:({struct _dyneither_ptr _tmpCD=({const char*_tmp43="base >= 2 && base <= 36";_tag_dyneither(_tmp43,sizeof(char),24U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpCD,({const char*_tmp44="ap.cyc";_tag_dyneither(_tmp44,sizeof(char),7U);}),377U);});
for(k=5;1 << k > base;-- k){
;}
size=((8 * x->ndigits)/ k + 1)+ 1;
if(x->sign == - 1)
++ size;
({struct _dyneither_ptr _tmpCE=({unsigned int _tmp45=size;_tag_dyneither(_cyccalloc_atomic(sizeof(char),_tmp45),sizeof(char),_tmp45);});str=_tmpCE;});
({struct _dyneither_ptr _tmpCF=({unsigned int _tmp46=x->ndigits * sizeof(unsigned char);_tag_dyneither(_cycalloc_atomic(_tmp46),1U,_tmp46);});q=_tmpCF;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(q,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
if(x->sign == - 1){
({struct _dyneither_ptr _tmp47=_dyneither_ptr_plus(str,sizeof(char),0);char _tmp48=*((char*)_check_dyneither_subscript(_tmp47,sizeof(char),0U));char _tmp49='-';if(_get_dyneither_size(_tmp47,sizeof(char))== 1U  && (_tmp48 == '\000'  && _tmp49 != '\000'))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});
({struct _dyneither_ptr _tmpD3=_dyneither_ptr_plus(str,sizeof(char),1);int _tmpD2=size - 1;int _tmpD1=base;int _tmpD0=x->ndigits;Cyc_XP_tostr(_tmpD3,_tmpD2,_tmpD1,_tmpD0,q);});}else{
# 390
Cyc_XP_tostr(str,size,base,x->ndigits,q);}
return(char*)_untag_dyneither_ptr(str,sizeof(char),1U);}
# 393
struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;struct Cyc_AP_T*tmp;
(unsigned int)x?0:({struct _dyneither_ptr _tmpD4=({const char*_tmp4A="x";_tag_dyneither(_tmp4A,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpD4,({const char*_tmp4B="ap.cyc";_tag_dyneither(_tmp4B,sizeof(char),7U);}),395U);});
(unsigned int)y?0:({struct _dyneither_ptr _tmpD5=({const char*_tmp4C="y";_tag_dyneither(_tmp4C,sizeof(char),2U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmpD5,({const char*_tmp4D="ap.cyc";_tag_dyneither(_tmp4D,sizeof(char),7U);}),396U);});
while(!(((struct Cyc_AP_T*)_check_null(y))->ndigits == 1  && *((unsigned char*)_check_dyneither_subscript(y->digits,sizeof(unsigned char),0))== 0)){
({struct Cyc_AP_T*_tmpD6=Cyc_AP_mod(x,y);tmp=_tmpD6;});
x=y;
y=tmp;}
# 402
({struct Cyc_AP_T*_tmpD7=Cyc_mk(x->ndigits);z=_tmpD7;});
((struct _dyneither_ptr(*)(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int))Cyc__memcpy)(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _dyneither_ptr)x->digits,x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=x->sign;
return z;}
# 408
struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
return({struct Cyc_AP_T*_tmpD9=x;Cyc_AP_mul(_tmpD9,({struct Cyc_AP_T*_tmpD8=y;Cyc_AP_div(_tmpD8,Cyc_AP_gcd(x,y));}));});}
