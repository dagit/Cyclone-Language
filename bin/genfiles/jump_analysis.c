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

/* This is not a macro since initialization order matters.  Defined in
   runtime_zeroterm.c. */
extern struct _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
  unsigned int sz,
  unsigned int numelts);

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
 struct Cyc_Core_Opt{void*v;};
# 126 "core.h"
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Position_Error;
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 28
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 39
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1188 "absyn.h"
extern int Cyc_Absyn_no_regions;struct Cyc_RgnOrder_RgnPO;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple10*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 115 "tcpat.h"
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct{int tag;struct Cyc_Absyn_Switch_clause*f1;};struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct{int tag;};struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct{int tag;struct _dyneither_ptr*f1;};
# 54 "jump_analysis.cyc"
struct Cyc_JumpAnalysis_NotLoop_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_NotLoop_j_val={0U};
struct Cyc_JumpAnalysis_CaseEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_CaseEnd_j_val={1U};
struct Cyc_JumpAnalysis_FnEnd_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_FnEnd_j_val={2U};
struct Cyc_JumpAnalysis_Return_j_JumpAnalysis_Jumpee_struct Cyc_JumpAnalysis_Return_j_val={5U};struct Cyc_JumpAnalysis_SaveEnv{int try_depth;int pat_var_depth;struct Cyc_List_List*enclosers;};struct Cyc_JumpAnalysis_FnEnv{struct Cyc_JumpAnalysis_Jump_Anal_Result*tables;struct Cyc_Hashtable_Table*stmt_info;struct Cyc_List_List*jumpers;struct Cyc_Hashtable_Table*labels;};struct Cyc_JumpAnalysis_StmtEnv{struct Cyc_JumpAnalysis_FnEnv*fenv;void*continue_stmt;void*break_stmt;void*next_stmt;struct Cyc_JumpAnalysis_SaveEnv save_env;};
# 81
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Exp*e);
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd);
# 85
static int Cyc_JumpAnalysis_hash_ptr(void*s){
return(int)s;}
# 88
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_ptr_table(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(void*,void*),int(*hash)(void*)))Cyc_Hashtable_create)(33,Cyc_Core_ptrcmp,(int(*)(void*s))Cyc_JumpAnalysis_hash_ptr);}
# 91
static struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_make_var_table(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(33,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 94
static struct Cyc_JumpAnalysis_StmtEnv Cyc_JumpAnalysis_add_encloser(struct Cyc_Absyn_Stmt*s,struct Cyc_JumpAnalysis_StmtEnv env){
struct Cyc_JumpAnalysis_StmtEnv _tmp0=env;
({struct Cyc_List_List*_tmpB2=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=s,_tmp1->tl=(_tmp0.save_env).enclosers;_tmp1;});(_tmp0.save_env).enclosers=_tmpB2;});
return _tmp0;}
# 99
static void Cyc_JumpAnalysis_anal_exps(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_JumpAnalysis_anal_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 103
static void Cyc_JumpAnalysis_anal_exp(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Exp*e){
void*_tmp2=e->r;void*_tmp3=_tmp2;struct Cyc_List_List*_tmp32;struct Cyc_List_List*_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_List_List*_tmp2F;struct Cyc_Absyn_Exp*_tmp2E;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_List_List*_tmpC;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmp9;struct Cyc_Absyn_Stmt*_tmp8;switch(*((int*)_tmp3)){case 37U: _LL1: _tmp8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2:
({struct Cyc_JumpAnalysis_StmtEnv _tmpB3=Cyc_JumpAnalysis_add_encloser(_tmp8,env);Cyc_JumpAnalysis_anal_stmt(_tmpB3,_tmp8);});goto _LL0;case 0U: _LL3: _LL4:
# 107
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 39U: _LLB: _LLC:
 goto _LLE;case 32U: _LLD: _LLE:
 goto _LL10;case 33U: _LLF: _LL10:
 goto _LL12;case 2U: _LL11: _LL12:
 goto _LL14;case 1U: _LL13: _LL14:
 goto _LL0;case 31U: _LL15: _tmp9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL16:
# 117
 _tmpA=_tmp9;goto _LL18;case 24U: _LL17: _tmpA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL18:
 _tmpB=_tmpA;goto _LL1A;case 3U: _LL19: _tmpB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL1A:
 Cyc_JumpAnalysis_anal_exps(env,_tmpB);goto _LL0;case 10U: _LL1B: _tmpD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmpC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL1C:
# 121
 Cyc_JumpAnalysis_anal_exp(env,_tmpD);Cyc_JumpAnalysis_anal_exps(env,_tmpC);goto _LL0;case 28U: _LL1D: _tmpE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL1E:
# 123
 _tmpF=_tmpE;goto _LL20;case 41U: _LL1F: _tmpF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL20:
 _tmp10=_tmpF;goto _LL22;case 38U: _LL21: _tmp10=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL22:
 _tmp11=_tmp10;goto _LL24;case 15U: _LL23: _tmp11=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL24:
 _tmp12=_tmp11;goto _LL26;case 20U: _LL25: _tmp12=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL26:
 _tmp13=_tmp12;goto _LL28;case 21U: _LL27: _tmp13=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL28:
 _tmp14=_tmp13;goto _LL2A;case 22U: _LL29: _tmp14=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2A:
 _tmp15=_tmp14;goto _LL2C;case 14U: _LL2B: _tmp15=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL2C:
 _tmp16=_tmp15;goto _LL2E;case 12U: _LL2D: _tmp16=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL2E:
 _tmp17=_tmp16;goto _LL30;case 13U: _LL2F: _tmp17=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL30:
 _tmp18=_tmp17;goto _LL32;case 11U: _LL31: _tmp18=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL32:
 _tmp19=_tmp18;goto _LL34;case 5U: _LL33: _tmp19=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL34:
 Cyc_JumpAnalysis_anal_exp(env,_tmp19);goto _LL0;case 6U: _LL35: _tmp1C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp1B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_tmp1A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL36:
# 137
 Cyc_JumpAnalysis_anal_exp(env,_tmp1C);Cyc_JumpAnalysis_anal_exp(env,_tmp1B);Cyc_JumpAnalysis_anal_exp(env,_tmp1A);goto _LL0;case 27U: _LL37: _tmp1E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_tmp1D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL38:
# 139
 _tmp20=_tmp1E;_tmp1F=_tmp1D;goto _LL3A;case 4U: _LL39: _tmp20=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp1F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL3A:
 _tmp22=_tmp20;_tmp21=_tmp1F;goto _LL3C;case 35U: _LL3B: _tmp22=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp21=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL3C:
 _tmp24=_tmp22;_tmp23=_tmp21;goto _LL3E;case 23U: _LL3D: _tmp24=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp23=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL3E:
 _tmp26=_tmp24;_tmp25=_tmp23;goto _LL40;case 7U: _LL3F: _tmp26=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp25=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL40:
 _tmp28=_tmp26;_tmp27=_tmp25;goto _LL42;case 8U: _LL41: _tmp28=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp27=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL42:
 _tmp2A=_tmp28;_tmp29=_tmp27;goto _LL44;case 9U: _LL43: _tmp2A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp29=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL44:
 Cyc_JumpAnalysis_anal_exp(env,_tmp2A);Cyc_JumpAnalysis_anal_exp(env,_tmp29);goto _LL0;case 34U: _LL45: _tmp2C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3)->f1).rgn;_tmp2B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp3)->f1).num_elts;_LL46:
# 147
 _tmp2E=_tmp2C;_tmp2D=_tmp2B;goto _LL48;case 16U: _LL47: _tmp2E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_tmp2D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL48:
# 149
 if(_tmp2E != 0)Cyc_JumpAnalysis_anal_exp(env,_tmp2E);
Cyc_JumpAnalysis_anal_exp(env,_tmp2D);
goto _LL0;case 30U: _LL49: _tmp2F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL4A:
# 153
 _tmp30=_tmp2F;goto _LL4C;case 29U: _LL4B: _tmp30=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp3)->f3;_LL4C:
 _tmp31=_tmp30;goto _LL4E;case 25U: _LL4D: _tmp31=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp3)->f2;_LL4E:
 _tmp32=_tmp31;goto _LL50;case 26U: _LL4F: _tmp32=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3)->f1;_LL50:
# 157
 for(0;_tmp32 != 0;_tmp32=_tmp32->tl){
Cyc_JumpAnalysis_anal_exp(env,(*((struct _tuple11*)_tmp32->hd)).f2);}
goto _LL0;case 36U: _LL51: _LL52:
# 161
({void*_tmp4=0U;({struct _dyneither_ptr _tmpB4=({const char*_tmp5="jump_anaysis found UnresolvedMem_e";_tag_dyneither(_tmp5,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB4,_tag_dyneither(_tmp4,sizeof(void*),0U));});});default: _LL53: _LL54:
({void*_tmp6=0U;({struct _dyneither_ptr _tmpB5=({const char*_tmp7="jump_analysis found Asm_e";_tag_dyneither(_tmp7,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB5,_tag_dyneither(_tmp6,sizeof(void*),0U));});});}_LL0:;}
# 165
static void Cyc_JumpAnalysis_anal_scs(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_List_List*scs){
# 168
env.break_stmt=env.next_stmt;
env.next_stmt=(void*)& Cyc_JumpAnalysis_CaseEnd_j_val;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp33=(struct Cyc_Absyn_Switch_clause*)scs->hd;
struct Cyc_Absyn_Exp*_tmp34=_tmp33->where_clause;
if(_tmp34 != 0)
Cyc_JumpAnalysis_anal_exp(env,_tmp34);{
struct Cyc_JumpAnalysis_StmtEnv _tmp35=env;
if(Cyc_Tcpat_has_vars(_tmp33->pat_vars)){
_tmp35=Cyc_JumpAnalysis_add_encloser(_tmp33->body,_tmp35);
++(_tmp35.save_env).pat_var_depth;}
# 180
Cyc_JumpAnalysis_anal_stmt(_tmp35,_tmp33->body);};}}struct _tuple12{struct Cyc_Absyn_Stmt*f1;void*f2;};
# 183
static void Cyc_JumpAnalysis_anal_stmt(struct Cyc_JumpAnalysis_StmtEnv env,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Hashtable_Table*_tmpB7=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpB6=s;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpB7,_tmpB6,({struct Cyc_JumpAnalysis_SaveEnv*_tmp36=_cycalloc(sizeof(*_tmp36));*_tmp36=env.save_env;_tmp36;}));});{
void*_tmp37=s->r;void*_tmp38=_tmp37;struct Cyc_Absyn_Decl*_tmp7C;struct Cyc_Absyn_Stmt*_tmp7B;struct Cyc_Absyn_Stmt*_tmp7A;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_Exp*_tmp78;struct Cyc_List_List*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_Switch_clause**_tmp75;struct _dyneither_ptr*_tmp74;struct Cyc_Absyn_Stmt*_tmp73;struct _dyneither_ptr*_tmp72;struct Cyc_Absyn_Stmt*_tmp71;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Stmt*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Exp*_tmp6D;struct Cyc_Absyn_Stmt*_tmp6C;struct Cyc_Absyn_Exp*_tmp6B;struct Cyc_Absyn_Stmt*_tmp6A;struct Cyc_Absyn_Stmt*_tmp69;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Stmt*_tmp67;struct Cyc_Absyn_Stmt*_tmp66;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Stmt*_tmp63;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Stmt*_tmp61;struct Cyc_Absyn_Stmt*_tmp60;struct Cyc_Absyn_Exp*_tmp5F;switch(*((int*)_tmp38)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp5F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_LL4:
 Cyc_JumpAnalysis_anal_exp(env,_tmp5F);goto _LL0;case 2U: _LL5: _tmp61=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp60=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_LL6: {
# 189
struct Cyc_JumpAnalysis_StmtEnv _tmp39=env;
({void*_tmpB8=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->tag=3U,_tmp3A->f1=_tmp60;_tmp3A;});_tmp39.next_stmt=_tmpB8;});
Cyc_JumpAnalysis_anal_stmt(_tmp39,_tmp61);
Cyc_JumpAnalysis_anal_stmt(env,_tmp60);
goto _LL0;}case 4U: _LL7: _tmp64=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp63=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_tmp62=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp38)->f3;_LL8:
# 195
 Cyc_JumpAnalysis_anal_exp(env,_tmp64);
Cyc_JumpAnalysis_anal_stmt(env,_tmp63);
Cyc_JumpAnalysis_anal_stmt(env,_tmp62);
goto _LL0;case 3U: _LL9: _tmp65=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_LLA:
# 201
 if(_tmp65 != 0)Cyc_JumpAnalysis_anal_exp(env,_tmp65);
({struct Cyc_List_List*_tmpBA=({struct Cyc_List_List*_tmp3C=_cycalloc(sizeof(*_tmp3C));({struct _tuple12*_tmpB9=({struct _tuple12*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=s,_tmp3B->f2=(void*)& Cyc_JumpAnalysis_Return_j_val;_tmp3B;});_tmp3C->hd=_tmpB9;}),_tmp3C->tl=(env.fenv)->jumpers;_tmp3C;});(env.fenv)->jumpers=_tmpBA;});
goto _LL0;case 5U: _LLB: _tmp68=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp38)->f1).f1;_tmp67=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp38)->f1).f2;_tmp66=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_LLC:
# 206
({struct Cyc_Hashtable_Table*_tmpBC=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpBB=_tmp67;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpBC,_tmpBB,({struct Cyc_JumpAnalysis_SaveEnv*_tmp3D=_cycalloc(sizeof(*_tmp3D));*_tmp3D=env.save_env;_tmp3D;}));});
Cyc_JumpAnalysis_anal_exp(env,_tmp68);{
struct Cyc_JumpAnalysis_StmtEnv _tmp3E=env;
({void*_tmpBD=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=3U,_tmp3F->f1=_tmp67;_tmp3F;});_tmp3E.continue_stmt=_tmpBD;});
_tmp3E.next_stmt=_tmp3E.continue_stmt;
_tmp3E.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(_tmp3E,_tmp66);
goto _LL0;};case 9U: _LLD: _tmp6E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp6D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp38)->f2).f1;_tmp6C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp38)->f2).f2;_tmp6B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp38)->f3).f1;_tmp6A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp38)->f3).f2;_tmp69=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp38)->f4;_LLE:
# 215
({struct Cyc_Hashtable_Table*_tmpBF=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpBE=_tmp6C;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpBF,_tmpBE,({struct Cyc_JumpAnalysis_SaveEnv*_tmp40=_cycalloc(sizeof(*_tmp40));*_tmp40=env.save_env;_tmp40;}));});
({struct Cyc_Hashtable_Table*_tmpC1=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpC0=_tmp6A;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpC1,_tmpC0,({struct Cyc_JumpAnalysis_SaveEnv*_tmp41=_cycalloc(sizeof(*_tmp41));*_tmp41=env.save_env;_tmp41;}));});
Cyc_JumpAnalysis_anal_exp(env,_tmp6E);
Cyc_JumpAnalysis_anal_exp(env,_tmp6D);{
# 220
struct Cyc_JumpAnalysis_StmtEnv _tmp42=env;
({void*_tmpC2=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->tag=3U,_tmp43->f1=_tmp6A;_tmp43;});_tmp42.continue_stmt=_tmpC2;});
_tmp42.next_stmt=_tmp42.continue_stmt;
_tmp42.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(_tmp42,_tmp69);
Cyc_JumpAnalysis_anal_exp(_tmp42,_tmp6B);
goto _LL0;};case 14U: _LLF: _tmp71=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp70=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp38)->f2).f1;_tmp6F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp38)->f2).f2;_LL10:
# 228
({struct Cyc_Hashtable_Table*_tmpC4=(env.fenv)->stmt_info;struct Cyc_Absyn_Stmt*_tmpC3=_tmp6F;((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_JumpAnalysis_SaveEnv*val))Cyc_Hashtable_insert)(_tmpC4,_tmpC3,({struct Cyc_JumpAnalysis_SaveEnv*_tmp44=_cycalloc(sizeof(*_tmp44));*_tmp44=env.save_env;_tmp44;}));});{
struct Cyc_JumpAnalysis_StmtEnv _tmp45=env;
({void*_tmpC5=(void*)({struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->tag=3U,_tmp46->f1=_tmp6F;_tmp46;});_tmp45.continue_stmt=_tmpC5;});
_tmp45.next_stmt=_tmp45.continue_stmt;
_tmp45.break_stmt=env.next_stmt;
Cyc_JumpAnalysis_anal_stmt(_tmp45,_tmp71);
Cyc_JumpAnalysis_anal_exp(env,_tmp70);
goto _LL0;};case 6U: _LL11: _LL12:
# 238
({struct Cyc_List_List*_tmpC7=({struct Cyc_List_List*_tmp48=_cycalloc(sizeof(*_tmp48));({struct _tuple12*_tmpC6=({struct _tuple12*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->f1=s,_tmp47->f2=env.break_stmt;_tmp47;});_tmp48->hd=_tmpC6;}),_tmp48->tl=(env.fenv)->jumpers;_tmp48;});(env.fenv)->jumpers=_tmpC7;});
goto _LL0;case 7U: _LL13: _LL14:
# 241
({struct Cyc_List_List*_tmpC9=({struct Cyc_List_List*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct _tuple12*_tmpC8=({struct _tuple12*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->f1=s,_tmp49->f2=env.continue_stmt;_tmp49;});_tmp4A->hd=_tmpC8;}),_tmp4A->tl=(env.fenv)->jumpers;_tmp4A;});(env.fenv)->jumpers=_tmpC9;});
goto _LL0;case 8U: _LL15: _tmp72=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_LL16:
# 244
({struct Cyc_List_List*_tmpCC=({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _tuple12*_tmpCB=({struct _tuple12*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->f1=s,({void*_tmpCA=(void*)({struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=6U,_tmp4B->f1=_tmp72;_tmp4B;});_tmp4C->f2=_tmpCA;});_tmp4C;});_tmp4D->hd=_tmpCB;}),_tmp4D->tl=(env.fenv)->jumpers;_tmp4D;});(env.fenv)->jumpers=_tmpCC;});
goto _LL0;case 13U: _LL17: _tmp74=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp73=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_LL18:
# 248
{struct _handler_cons _tmp4E;_push_handler(& _tmp4E);{int _tmp50=0;if(setjmp(_tmp4E.handler))_tmp50=1;if(!_tmp50){
((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)((env.fenv)->labels,_tmp74);
({void*_tmp51=0U;({unsigned int _tmpCE=s->loc;struct _dyneither_ptr _tmpCD=({const char*_tmp52="repated label";_tag_dyneither(_tmp52,sizeof(char),14U);});Cyc_Warn_err(_tmpCE,_tmpCD,_tag_dyneither(_tmp51,sizeof(void*),0U));});});
# 249
;_pop_handler();}else{void*_tmp4F=(void*)_exn_thrown;void*_tmp53=_tmp4F;void*_tmp54;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp53)->tag == Cyc_Core_Not_found){_LL22: _LL23:
# 251
 goto _LL21;}else{_LL24: _tmp54=_tmp53;_LL25:(int)_rethrow(_tmp54);}_LL21:;}};}
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)((env.fenv)->labels,_tmp74,s);
Cyc_JumpAnalysis_anal_stmt(env,_tmp73);
goto _LL0;case 11U: _LL19: _tmp76=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp75=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_LL1A:
# 258
 Cyc_JumpAnalysis_anal_exps(env,_tmp76);
({struct Cyc_List_List*_tmpD1=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));({struct _tuple12*_tmpD0=({struct _tuple12*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->f1=s,({void*_tmpCF=(void*)({struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=4U,_tmp55->f1=*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp75));_tmp55;});_tmp56->f2=_tmpCF;});_tmp56;});_tmp57->hd=_tmpD0;}),_tmp57->tl=(env.fenv)->jumpers;_tmp57;});(env.fenv)->jumpers=_tmpD1;});
# 261
goto _LL0;case 10U: _LL1B: _tmp78=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp77=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_LL1C:
# 263
 Cyc_JumpAnalysis_anal_exp(env,_tmp78);
Cyc_JumpAnalysis_anal_scs(env,_tmp77);
goto _LL0;case 15U: _LL1D: _tmp7A=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp79=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_LL1E: {
# 267
struct Cyc_JumpAnalysis_StmtEnv _tmp58=Cyc_JumpAnalysis_add_encloser(_tmp7A,env);
++(_tmp58.save_env).try_depth;
Cyc_JumpAnalysis_anal_stmt(_tmp58,_tmp7A);
Cyc_JumpAnalysis_anal_scs(env,_tmp79);
goto _LL0;}default: _LL1F: _tmp7C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp38)->f1;_tmp7B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp38)->f2;_LL20:
# 276
 env=Cyc_JumpAnalysis_add_encloser(s,env);
{void*_tmp59=_tmp7C->r;void*_tmp5A=_tmp59;struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_Absyn_Fndecl*_tmp5D;struct Cyc_Absyn_Exp*_tmp5C;struct Cyc_Absyn_Exp*_tmp5B;switch(*((int*)_tmp5A)){case 4U: _LL27: _tmp5B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5A)->f3;_LL28:
# 280
 if(_tmp5B != 0){
Cyc_JumpAnalysis_anal_exp(env,_tmp5B);
++(env.save_env).pat_var_depth;}
# 284
if(!Cyc_Absyn_no_regions)
++(env.save_env).try_depth;
goto _LL26;case 0U: _LL29: _tmp5C=(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5A)->f1)->initializer;_LL2A:
# 288
 if(_tmp5C != 0)Cyc_JumpAnalysis_anal_exp(env,_tmp5C);
goto _LL26;case 1U: _LL2B: _tmp5D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5A)->f1;_LL2C:
# 291
 Cyc_JumpAnalysis_anal_fd((env.fenv)->tables,_tmp5D);
goto _LL26;case 2U: _LL2D: _tmp5E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5A)->f3;_LL2E:
# 294
 Cyc_JumpAnalysis_anal_exp(env,_tmp5E);
++(env.save_env).pat_var_depth;
goto _LL26;case 3U: _LL2F: _LL30:
# 298
 goto _LL32;case 8U: _LL31: _LL32:
 goto _LL34;case 5U: _LL33: _LL34:
 goto _LL36;case 6U: _LL35: _LL36:
 goto _LL38;case 7U: _LL37: _LL38:
 goto _LL3A;case 13U: _LL39: _LL3A:
 goto _LL3C;case 14U: _LL3B: _LL3C:
 goto _LL3E;case 15U: _LL3D: _LL3E:
 goto _LL40;case 16U: _LL3F: _LL40:
 goto _LL42;case 11U: _LL41: _LL42:
 goto _LL44;case 12U: _LL43: _LL44:
 goto _LL46;case 9U: _LL45: _LL46:
 goto _LL48;default: _LL47: _LL48:
 goto _LL26;}_LL26:;}
# 312
Cyc_JumpAnalysis_anal_stmt(env,_tmp7B);
goto _LL0;}_LL0:;};}
# 318
static void Cyc_JumpAnalysis_anal_fd(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_JumpAnalysis_FnEnv*_tmp7D=({struct Cyc_JumpAnalysis_FnEnv*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tables=tables,({
struct Cyc_Hashtable_Table*_tmpD3=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpA8->stmt_info=_tmpD3;}),_tmpA8->jumpers=0,({
# 322
struct Cyc_Hashtable_Table*_tmpD2=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_var_table)();_tmpA8->labels=_tmpD2;});_tmpA8;});
struct Cyc_JumpAnalysis_StmtEnv _tmp7E=({struct Cyc_JumpAnalysis_StmtEnv _tmpB1;_tmpB1.fenv=_tmp7D,_tmpB1.continue_stmt=(void*)& Cyc_JumpAnalysis_NotLoop_j_val,_tmpB1.break_stmt=(void*)& Cyc_JumpAnalysis_NotLoop_j_val,_tmpB1.next_stmt=(void*)& Cyc_JumpAnalysis_FnEnd_j_val,
# 327
(_tmpB1.save_env).try_depth=0,(_tmpB1.save_env).pat_var_depth=0,(_tmpB1.save_env).enclosers=0;_tmpB1;});
# 330
Cyc_JumpAnalysis_anal_stmt(_tmp7E,fd->body);{
# 332
struct Cyc_Hashtable_Table*pop_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
struct Cyc_Hashtable_Table*succ_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
struct Cyc_Hashtable_Table*pat_pop_table=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();
# 336
struct Cyc_Hashtable_Table*_tmp7F=_tmp7D->stmt_info;
{struct Cyc_List_List*_tmp80=_tmp7D->jumpers;for(0;_tmp80 != 0;_tmp80=_tmp80->tl){
struct _tuple12*_tmp81=(struct _tuple12*)_tmp80->hd;struct _tuple12*_tmp82=_tmp81;struct Cyc_Absyn_Stmt*_tmpA7;void*_tmpA6;_LL1: _tmpA7=_tmp82->f1;_tmpA6=_tmp82->f2;_LL2:;{
struct Cyc_JumpAnalysis_SaveEnv*_tmp83=
# 341
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7F,_tmpA7);
# 339
struct Cyc_JumpAnalysis_SaveEnv*_tmp84=_tmp83;int _tmpA5;int _tmpA4;struct Cyc_List_List*_tmpA3;_LL4: _tmpA5=_tmp84->try_depth;_tmpA4=_tmp84->pat_var_depth;_tmpA3=_tmp84->enclosers;_LL5:;{
# 342
void*_tmp85=_tmpA6;struct Cyc_Absyn_Switch_clause*_tmpA2;struct Cyc_Absyn_Stmt*_tmpA1;struct _dyneither_ptr*_tmpA0;switch(*((int*)_tmp85)){case 0U: _LL7: _LL8:
# 344
({void*_tmp86=0U;({unsigned int _tmpD5=_tmpA7->loc;struct _dyneither_ptr _tmpD4=({const char*_tmp87="jump has no target";_tag_dyneither(_tmp87,sizeof(char),19U);});Cyc_Warn_err(_tmpD5,_tmpD4,_tag_dyneither(_tmp86,sizeof(void*),0U));});});
goto _LL6;case 1U: _LL9: _LLA:
# 347
({void*_tmp88=0U;({unsigned int _tmpD7=_tmpA7->loc;struct _dyneither_ptr _tmpD6=({const char*_tmp89="break causes outer switch to implicitly fallthru";_tag_dyneither(_tmp89,sizeof(char),49U);});Cyc_Warn_err(_tmpD7,_tmpD6,_tag_dyneither(_tmp88,sizeof(void*),0U));});});
# 349
goto _LL6;case 2U: _LLB: _LLC:
# 351
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,_tmpA7,0);
goto _LLE;case 5U: _LLD: _LLE:
# 354
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,_tmpA7,_tmpA5);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,_tmpA7,_tmpA4);
goto _LL6;case 6U: _LLF: _tmpA0=((struct Cyc_JumpAnalysis_Lab_j_JumpAnalysis_Jumpee_struct*)_tmp85)->f1;_LL10: {
# 358
struct Cyc_Absyn_Stmt*dest_stmt;
{struct _handler_cons _tmp8A;_push_handler(& _tmp8A);{int _tmp8C=0;if(setjmp(_tmp8A.handler))_tmp8C=1;if(!_tmp8C){dest_stmt=((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(_tmp7D->labels,_tmpA0);;_pop_handler();}else{void*_tmp8B=(void*)_exn_thrown;void*_tmp8D=_tmp8B;void*_tmp91;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp8D)->tag == Cyc_Core_Not_found){_LL16: _LL17:
# 362
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmpB0;_tmpB0.tag=0U,_tmpB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpA0);_tmpB0;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({unsigned int _tmpD9=_tmpA7->loc;struct _dyneither_ptr _tmpD8=({const char*_tmp8F="label %s undefined";_tag_dyneither(_tmp8F,sizeof(char),19U);});Cyc_Warn_err(_tmpD9,_tmpD8,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});
goto L;}else{_LL18: _tmp91=_tmp8D;_LL19:(int)_rethrow(_tmp91);}_LL15:;}};}{
# 365
struct Cyc_JumpAnalysis_SaveEnv*_tmp92=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7F,dest_stmt);
# 365
struct Cyc_JumpAnalysis_SaveEnv*_tmp93=_tmp92;struct Cyc_List_List*_tmp97;_LL1B: _tmp97=_tmp93->enclosers;_LL1C:;
# 367
if(_tmp97 != 0){
struct Cyc_Absyn_Stmt*_tmp94=(struct Cyc_Absyn_Stmt*)_tmp97->hd;
for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){
if((struct Cyc_Absyn_Stmt*)_tmpA3->hd == _tmp94)
break;}
if(_tmpA3 == 0){
({void*_tmp95=0U;({unsigned int _tmpDB=_tmpA7->loc;struct _dyneither_ptr _tmpDA=({const char*_tmp96="goto enters local scope or exception handler";_tag_dyneither(_tmp96,sizeof(char),45U);});Cyc_Warn_err(_tmpDB,_tmpDA,_tag_dyneither(_tmp95,sizeof(void*),0U));});});
# 375
goto _LL6;}}
# 378
_tmpA1=dest_stmt;goto _LL12;};}case 3U: _LL11: _tmpA1=((struct Cyc_JumpAnalysis_Stmt_j_JumpAnalysis_Jumpee_struct*)_tmp85)->f1;_LL12: {
# 380
struct Cyc_JumpAnalysis_SaveEnv*_tmp98=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7F,_tmpA1);
# 380
struct Cyc_JumpAnalysis_SaveEnv*_tmp99=_tmp98;int _tmp9B;int _tmp9A;_LL1E: _tmp9B=_tmp99->try_depth;_tmp9A=_tmp99->pat_var_depth;_LL1F:;
# 382
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,_tmpA7,_tmpA5 - _tmp9B);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,_tmpA7,_tmpA4 - _tmp9A);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,_tmpA7,_tmpA1);
goto _LL6;}default: _LL13: _tmpA2=((struct Cyc_JumpAnalysis_Fallthru_j_JumpAnalysis_Jumpee_struct*)_tmp85)->f1;_LL14: {
# 387
struct Cyc_JumpAnalysis_SaveEnv*_tmp9C=
((struct Cyc_JumpAnalysis_SaveEnv*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(_tmp7F,_tmpA2->body);
# 387
struct Cyc_JumpAnalysis_SaveEnv*_tmp9D=_tmp9C;int _tmp9F;int _tmp9E;_LL21: _tmp9F=_tmp9D->try_depth;_tmp9E=_tmp9D->pat_var_depth;_LL22:;
# 389
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pop_table,_tmpA7,_tmpA5 - _tmp9F);{
int pv_pop;
if(Cyc_Tcpat_has_vars(_tmpA2->pat_vars))
pv_pop=(1 + _tmpA4)- _tmp9E;else{
# 394
pv_pop=_tmpA4 - _tmp9E;}
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,int val))Cyc_Hashtable_insert)(pat_pop_table,_tmpA7,pv_pop);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,struct Cyc_Absyn_Stmt*val))Cyc_Hashtable_insert)(succ_table,_tmpA7,_tmpA2->body);
goto _LL6;};}}_LL6:;};};}}
# 400
L:((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->pop_tables,fd,pop_table);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->succ_tables,fd,succ_table);
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key,struct Cyc_Hashtable_Table*val))Cyc_Hashtable_insert)(tables->pat_pop_tables,fd,pat_pop_table);};}
# 405
static void Cyc_JumpAnalysis_jump_analysis_iter(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 408
for(0;tds != 0;tds=tds->tl){
void*_tmpA9=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_tmpAA=_tmpA9;struct Cyc_Absyn_Fndecl*_tmpAD;struct Cyc_List_List*_tmpAC;struct Cyc_List_List*_tmpAB;switch(*((int*)_tmpAA)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 goto _LL8;case 0U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 15U: _LL15: _LL16:
 goto _LL18;case 16U: _LL17: _LL18:
 goto _LL1A;case 11U: _LL19: _LL1A:
 goto _LL1C;case 12U: _LL1B: _LL1C:
 continue;case 9U: _LL1D: _tmpAB=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpAA)->f2;_LL1E:
 _tmpAC=_tmpAB;goto _LL20;case 10U: _LL1F: _tmpAC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpAA)->f2;_LL20:
 Cyc_JumpAnalysis_jump_analysis_iter(tables,_tmpAC);goto _LL0;default: _LL21: _tmpAD=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpAA)->f1;_LL22:
 Cyc_JumpAnalysis_anal_fd(tables,_tmpAD);goto _LL0;}_LL0:;}}
# 430
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds){
struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpAE=({struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpAF=_cycalloc(sizeof(*_tmpAF));({struct Cyc_Hashtable_Table*_tmpDE=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpAF->pop_tables=_tmpDE;}),({
struct Cyc_Hashtable_Table*_tmpDD=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpAF->succ_tables=_tmpDD;}),({
struct Cyc_Hashtable_Table*_tmpDC=((struct Cyc_Hashtable_Table*(*)())Cyc_JumpAnalysis_make_ptr_table)();_tmpAF->pat_pop_tables=_tmpDC;});_tmpAF;});
Cyc_JumpAnalysis_jump_analysis_iter(_tmpAE,tds);
return _tmpAE;}
