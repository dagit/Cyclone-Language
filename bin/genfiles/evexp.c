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

//extern struct _xtunion_struct *_exn_thrown;
extern struct _xtunion_struct* Cyc_Core_get_exn_thrown();
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned int nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 505
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 507
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
extern void*Cyc_Absyn_uint_type;
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
# 109
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 112
void*Cyc_Tcutil_compress(void*t);
# 141
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 194
int Cyc_Tcutil_unify(void*,void*);
# 196
int Cyc_Tcutil_typecmp(void*,void*);
# 201
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple10{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{union Cyc_Absyn_Cnst f1;int f2;};
# 34 "evexp.cyc"
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 36
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;enum Cyc_Absyn_Sign _tmp4;short _tmp3;enum Cyc_Absyn_Sign _tmp2;char _tmp1;switch((_tmp0.Short_c).tag){case 2U: _LL1: _tmp2=((_tmp0.Char_c).val).f1;_tmp1=((_tmp0.Char_c).val).f2;_LL2:
 return Cyc_Absyn_Int_c(_tmp2,(int)_tmp1);case 4U: _LL3: _tmp4=((_tmp0.Short_c).val).f1;_tmp3=((_tmp0.Short_c).val).f2;_LL4:
# 40
 return Cyc_Absyn_Int_c(_tmp4,(int)_tmp3);default: _LL5: _LL6:
 return cn;}_LL0:;}
# 45
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp5=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _tmp6=_tmp5;union Cyc_Absyn_Cnst _tmpF;int _tmpE;_LL1: _tmpF=_tmp6.f1;_tmpE=_tmp6.f2;_LL2:;
if(!_tmpE)
return({struct _tuple10 _tmp116;_tmp116.f1=0U,_tmp116.f2=0;_tmp116;});{
union Cyc_Absyn_Cnst _tmp7=Cyc_Evexp_promote_const(_tmpF);union Cyc_Absyn_Cnst _tmp8=_tmp7;long long _tmpD;int _tmpC;switch((_tmp8.Null_c).tag){case 5U: _LL4: _tmpC=((_tmp8.Int_c).val).f2;_LL5:
 return({struct _tuple10 _tmp117;_tmp117.f1=(unsigned int)_tmpC,_tmp117.f2=1;_tmp117;});case 6U: _LL6: _tmpD=((_tmp8.LongLong_c).val).f2;_LL7: {
# 53
unsigned long long y=(unsigned long long)_tmpD >> (unsigned long long)32;
if(y != (unsigned long long)-1  && y != (unsigned long long)0)
return({struct _tuple10 _tmp118;_tmp118.f1=0U,_tmp118.f2=0;_tmp118;});else{
return({struct _tuple10 _tmp119;_tmp119.f1=(unsigned int)_tmpD,_tmp119.f2=1;_tmp119;});}}case 7U: _LL8: _LL9:
 return({struct _tuple10 _tmp11A;_tmp11A.f1=0U,_tmp11A.f2=0;_tmp11A;});case 1U: _LLA: _LLB:
 return({struct _tuple10 _tmp11B;_tmp11B.f1=0U,_tmp11B.f2=1;_tmp11B;});default: _LLC: _LLD:
# 60
({struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmp11C;_tmp11C.tag=0U,({struct _dyneither_ptr _tmp143=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp11C.f1=_tmp143;});_tmp11C;});void*_tmp9[1U];_tmp9[0]=& _tmpB;({unsigned int _tmp145=e->loc;struct _dyneither_ptr _tmp144=({const char*_tmpA="expecting unsigned int found %s";_tag_dyneither(_tmpA,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp145,_tmp144,_tag_dyneither(_tmp9,sizeof(void*),1U));});});return({struct _tuple10 _tmp11D;_tmp11D.f1=0U,_tmp11D.f2=1;_tmp11D;});}_LL3:;};}struct _tuple12{int f1;int f2;};
# 64
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp10=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _tmp11=_tmp10;union Cyc_Absyn_Cnst _tmp19;int _tmp18;_LL1: _tmp19=_tmp11.f1;_tmp18=_tmp11.f2;_LL2:;
if(!_tmp18)
return({struct _tuple12 _tmp11E;_tmp11E.f1=0,_tmp11E.f2=0;_tmp11E;});{
union Cyc_Absyn_Cnst _tmp12=Cyc_Evexp_promote_const(_tmp19);union Cyc_Absyn_Cnst _tmp13=_tmp12;long long _tmp17;int _tmp16;switch((_tmp13.Float_c).tag){case 5U: _LL4: _tmp16=((_tmp13.Int_c).val).f2;_LL5:
 return({struct _tuple12 _tmp11F;_tmp11F.f1=_tmp16 != 0,_tmp11F.f2=1;_tmp11F;});case 6U: _LL6: _tmp17=((_tmp13.LongLong_c).val).f2;_LL7:
 return({struct _tuple12 _tmp120;_tmp120.f1=_tmp17 != (long long)0,_tmp120.f2=1;_tmp120;});case 1U: _LL8: _LL9:
 return({struct _tuple12 _tmp121;_tmp121.f1=0,_tmp121.f2=0;_tmp121;});case 7U: _LLA: _LLB:
 return({struct _tuple12 _tmp122;_tmp122.f1=0,_tmp122.f2=1;_tmp122;});default: _LLC: _LLD:
({void*_tmp14=0U;({unsigned int _tmp147=e->loc;struct _dyneither_ptr _tmp146=({const char*_tmp15="expecting bool";_tag_dyneither(_tmp15,sizeof(char),15U);});Cyc_Tcutil_terr(_tmp147,_tmp146,_tag_dyneither(_tmp14,sizeof(void*),0U));});});return({struct _tuple12 _tmp123;_tmp123.f1=0,_tmp123.f2=0;_tmp123;});}_LL3:;};}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 77
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp1A=Cyc_Evexp_eval_const_exp(e);struct _tuple11 _tmp1B=_tmp1A;union Cyc_Absyn_Cnst _tmp33;int _tmp32;_LL1: _tmp33=_tmp1B.f1;_tmp32=_tmp1B.f2;_LL2:;
if(!_tmp32)
return({struct _tuple11 _tmp124;_tmp124.f1=_tmp33,_tmp124.f2=0;_tmp124;});
{struct _tuple13 _tmp1C=({struct _tuple13 _tmp126;_tmp126.f1=p,_tmp126.f2=_tmp33;_tmp126;});struct _tuple13 _tmp1D=_tmp1C;long long _tmp31;char _tmp30;short _tmp2F;int _tmp2E;enum Cyc_Absyn_Sign _tmp2D;long long _tmp2C;enum Cyc_Absyn_Sign _tmp2B;char _tmp2A;enum Cyc_Absyn_Sign _tmp29;short _tmp28;enum Cyc_Absyn_Sign _tmp27;int _tmp26;enum Cyc_Absyn_Sign _tmp25;long long _tmp24;enum Cyc_Absyn_Sign _tmp23;char _tmp22;enum Cyc_Absyn_Sign _tmp21;short _tmp20;enum Cyc_Absyn_Sign _tmp1F;int _tmp1E;switch(_tmp1D.f1){case Cyc_Absyn_Plus: _LL4: _LL5:
 goto _LL3;case Cyc_Absyn_Minus: switch(((_tmp1D.f2).LongLong_c).tag){case 5U: _LL6: _tmp1F=(((_tmp1D.f2).Int_c).val).f1;_tmp1E=(((_tmp1D.f2).Int_c).val).f2;_LL7:
 _tmp33=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- _tmp1E);goto _LL3;case 4U: _LL8: _tmp21=(((_tmp1D.f2).Short_c).val).f1;_tmp20=(((_tmp1D.f2).Short_c).val).f2;_LL9:
 _tmp33=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,- _tmp20);goto _LL3;case 2U: _LLA: _tmp23=(((_tmp1D.f2).Char_c).val).f1;_tmp22=(((_tmp1D.f2).Char_c).val).f2;_LLB:
 _tmp33=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,- _tmp22);goto _LL3;case 6U: _LLC: _tmp25=(((_tmp1D.f2).LongLong_c).val).f1;_tmp24=(((_tmp1D.f2).LongLong_c).val).f2;_LLD:
 _tmp33=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,- _tmp24);goto _LL3;default: goto _LL20;}case Cyc_Absyn_Bitnot: switch(((_tmp1D.f2).LongLong_c).tag){case 5U: _LLE: _tmp27=(((_tmp1D.f2).Int_c).val).f1;_tmp26=(((_tmp1D.f2).Int_c).val).f2;_LLF:
 _tmp33=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ _tmp26);goto _LL3;case 4U: _LL10: _tmp29=(((_tmp1D.f2).Short_c).val).f1;_tmp28=(((_tmp1D.f2).Short_c).val).f2;_LL11:
 _tmp33=Cyc_Absyn_Short_c(Cyc_Absyn_Unsigned,~ _tmp28);goto _LL3;case 2U: _LL12: _tmp2B=(((_tmp1D.f2).Char_c).val).f1;_tmp2A=(((_tmp1D.f2).Char_c).val).f2;_LL13:
 _tmp33=Cyc_Absyn_Char_c(Cyc_Absyn_Unsigned,~ _tmp2A);goto _LL3;case 6U: _LL14: _tmp2D=(((_tmp1D.f2).LongLong_c).val).f1;_tmp2C=(((_tmp1D.f2).LongLong_c).val).f2;_LL15:
 _tmp33=Cyc_Absyn_LongLong_c(Cyc_Absyn_Unsigned,~ _tmp2C);goto _LL3;default: goto _LL20;}case Cyc_Absyn_Not: switch(((_tmp1D.f2).Null_c).tag){case 5U: _LL16: _tmp2E=(((_tmp1D.f2).Int_c).val).f2;_LL17:
 _tmp33=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,_tmp2E == 0?1: 0);goto _LL3;case 4U: _LL18: _tmp2F=(((_tmp1D.f2).Short_c).val).f2;_LL19:
 _tmp33=Cyc_Absyn_Short_c(Cyc_Absyn_Signed,(int)_tmp2F == 0?1: 0);goto _LL3;case 2U: _LL1A: _tmp30=(((_tmp1D.f2).Char_c).val).f2;_LL1B:
 _tmp33=Cyc_Absyn_Char_c(Cyc_Absyn_Signed,(int)_tmp30 == 0?'\001':'\000');goto _LL3;case 6U: _LL1C: _tmp31=(((_tmp1D.f2).LongLong_c).val).f2;_LL1D:
 _tmp33=Cyc_Absyn_LongLong_c(Cyc_Absyn_Signed,(long long)(_tmp31 == (long long)0?1: 0));goto _LL3;case 1U: _LL1E: _LL1F:
 _tmp33=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL3;default: goto _LL20;}default: _LL20: _LL21:
 return({struct _tuple11 _tmp125;_tmp125.f1=_tmp33,_tmp125.f2=0;_tmp125;});}_LL3:;}
# 98
return({struct _tuple11 _tmp127;_tmp127.f1=_tmp33,_tmp127.f2=1;_tmp127;});}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 102
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp34=Cyc_Evexp_eval_const_exp(e1);struct _tuple11 _tmp35=_tmp34;union Cyc_Absyn_Cnst _tmp4C;int _tmp4B;_LL1: _tmp4C=_tmp35.f1;_tmp4B=_tmp35.f2;_LL2:;{
struct _tuple11 _tmp36=Cyc_Evexp_eval_const_exp(e2);struct _tuple11 _tmp37=_tmp36;union Cyc_Absyn_Cnst _tmp4A;int _tmp49;_LL4: _tmp4A=_tmp37.f1;_tmp49=_tmp37.f2;_LL5:;
if(!_tmp4B  || !_tmp49)
return({struct _tuple11 _tmp128;_tmp128.f1=_tmp4C,_tmp128.f2=0;_tmp128;});
_tmp4C=Cyc_Evexp_promote_const(_tmp4C);
_tmp4A=Cyc_Evexp_promote_const(_tmp4A);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp38=_tmp4C;enum Cyc_Absyn_Sign _tmp3A;int _tmp39;if((_tmp38.Int_c).tag == 5){_LL7: _tmp3A=((_tmp38.Int_c).val).f1;_tmp39=((_tmp38.Int_c).val).f2;_LL8:
 s1=_tmp3A;i1=_tmp39;goto _LL6;}else{_LL9: _LLA:
 return({struct _tuple11 _tmp129;_tmp129.f1=_tmp4C,_tmp129.f2=0;_tmp129;});}_LL6:;}
# 115
{union Cyc_Absyn_Cnst _tmp3B=_tmp4A;enum Cyc_Absyn_Sign _tmp3D;int _tmp3C;if((_tmp3B.Int_c).tag == 5){_LLC: _tmp3D=((_tmp3B.Int_c).val).f1;_tmp3C=((_tmp3B.Int_c).val).f2;_LLD:
 s2=_tmp3D;i2=_tmp3C;goto _LLB;}else{_LLE: _LLF:
 return({struct _tuple11 _tmp12A;_tmp12A.f1=_tmp4C,_tmp12A.f2=0;_tmp12A;});}_LLB:;}
# 119
{enum Cyc_Absyn_Primop _tmp3E=p;switch(_tmp3E){case Cyc_Absyn_Div: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Mod: _LL13: _LL14:
# 122
 if(i2 == 0){
({void*_tmp3F=0U;({unsigned int _tmp149=e2->loc;struct _dyneither_ptr _tmp148=({const char*_tmp40="division by zero in constant expression";_tag_dyneither(_tmp40,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp149,_tmp148,_tag_dyneither(_tmp3F,sizeof(void*),0U));});});
return({struct _tuple11 _tmp12B;_tmp12B.f1=_tmp4C,_tmp12B.f2=1;_tmp12B;});}
# 126
goto _LL10;default: _LL15: _LL16:
 goto _LL10;}_LL10:;}{
# 129
int has_u_arg=(int)s1 == (int)1U  || (int)s2 == (int)Cyc_Absyn_Unsigned;
unsigned int u1=(unsigned int)i1;
unsigned int u2=(unsigned int)i2;
int i3=0;
unsigned int u3=0U;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple14 _tmp41=({struct _tuple14 _tmp12D;_tmp12D.f1=p,_tmp12D.f2=has_u_arg;_tmp12D;});struct _tuple14 _tmp42=_tmp41;switch(_tmp42.f1){case Cyc_Absyn_Plus: switch(_tmp42.f2){case 0U: _LL18: _LL19:
 i3=i1 + i2;use_i3=1;goto _LL17;case 1U: _LL22: _LL23:
# 144
 u3=u1 + u2;use_u3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Times: switch(_tmp42.f2){case 0U: _LL1A: _LL1B:
# 140
 i3=i1 * i2;use_i3=1;goto _LL17;case 1U: _LL24: _LL25:
# 145
 u3=u1 * u2;use_u3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Minus: switch(_tmp42.f2){case 0U: _LL1C: _LL1D:
# 141
 i3=i1 - i2;use_i3=1;goto _LL17;case 1U: _LL26: _LL27:
# 146
 u3=u1 - u2;use_u3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Div: switch(_tmp42.f2){case 0U: _LL1E: _LL1F:
# 142
 i3=i1 / i2;use_i3=1;goto _LL17;case 1U: _LL28: _LL29:
# 147
 u3=u1 / u2;use_u3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Mod: switch(_tmp42.f2){case 0U: _LL20: _LL21:
# 143
 i3=i1 % i2;use_i3=1;goto _LL17;case 1U: _LL2A: _LL2B:
# 148
 u3=u1 % u2;use_u3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Eq: _LL2C: _LL2D:
 b3=i1 == i2;use_b3=1;goto _LL17;case Cyc_Absyn_Neq: _LL2E: _LL2F:
 b3=i1 != i2;use_b3=1;goto _LL17;case Cyc_Absyn_Gt: switch(_tmp42.f2){case 0U: _LL30: _LL31:
 b3=i1 > i2;use_b3=1;goto _LL17;case 1U: _LL38: _LL39:
# 155
 b3=u1 > u2;use_b3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Lt: switch(_tmp42.f2){case 0U: _LL32: _LL33:
# 152
 b3=i1 < i2;use_b3=1;goto _LL17;case 1U: _LL3A: _LL3B:
# 156
 b3=u1 < u2;use_b3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Gte: switch(_tmp42.f2){case 0U: _LL34: _LL35:
# 153
 b3=i1 >= i2;use_b3=1;goto _LL17;case 1U: _LL3C: _LL3D:
# 157
 b3=u1 >= u2;use_b3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Lte: switch(_tmp42.f2){case 0U: _LL36: _LL37:
# 154
 b3=i1 <= i2;use_b3=1;goto _LL17;case 1U: _LL3E: _LL3F:
# 158
 b3=u1 <= u2;use_b3=1;goto _LL17;default: goto _LL4C;}case Cyc_Absyn_Bitand: _LL40: _LL41:
 u3=u1 & u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitor: _LL42: _LL43:
 u3=u1 | u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitxor: _LL44: _LL45:
 u3=u1 ^ u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlshift: _LL46: _LL47:
 u3=u1 << u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitlrshift: _LL48: _LL49:
 u3=u1 >> u2;use_u3=1;goto _LL17;case Cyc_Absyn_Bitarshift: _LL4A: _LL4B:
({void*_tmp43=0U;({unsigned int _tmp14B=e1->loc;struct _dyneither_ptr _tmp14A=({const char*_tmp44=">>> NOT IMPLEMENTED";_tag_dyneither(_tmp44,sizeof(char),20U);});Cyc_Tcutil_terr(_tmp14B,_tmp14A,_tag_dyneither(_tmp43,sizeof(void*),0U));});});goto _LL4D;default: _LL4C: _LL4D:
({void*_tmp45=0U;({unsigned int _tmp14D=e1->loc;struct _dyneither_ptr _tmp14C=({const char*_tmp46="bad constant expression";_tag_dyneither(_tmp46,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp14D,_tmp14C,_tag_dyneither(_tmp45,sizeof(void*),0U));});});return({struct _tuple11 _tmp12C;_tmp12C.f1=_tmp4C,_tmp12C.f2=1;_tmp12C;});}_LL17:;}
# 167
if(use_i3)return({struct _tuple11 _tmp12E;({union Cyc_Absyn_Cnst _tmp14E=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3);_tmp12E.f1=_tmp14E;}),_tmp12E.f2=1;_tmp12E;});
if(use_u3)return({struct _tuple11 _tmp12F;({union Cyc_Absyn_Cnst _tmp14F=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3);_tmp12F.f1=_tmp14F;}),_tmp12F.f2=1;_tmp12F;});
if(use_b3)return({struct _tuple11 _tmp130;({union Cyc_Absyn_Cnst _tmp150=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0);_tmp130.f1=_tmp150;}),_tmp130.f2=1;_tmp130;});
({void*_tmp47=0U;({struct _dyneither_ptr _tmp151=({const char*_tmp48="Evexp::eval_const_binop";_tag_dyneither(_tmp48,sizeof(char),24U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp151,_tag_dyneither(_tmp47,sizeof(void*),0U));});});};};};}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 176
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 ans;
{void*_tmp4D=e->r;void*_tmp4E=_tmp4D;struct Cyc_Absyn_Enumfield*_tmp8D;struct Cyc_Absyn_Enumfield*_tmp8C;void*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;void*_tmp89;enum Cyc_Absyn_Primop _tmp88;struct Cyc_List_List*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp80;union Cyc_Absyn_Cnst _tmp7F;switch(*((int*)_tmp4E)){case 1U: _LL1: _LL2:
# 180
 return({struct _tuple11 _tmp131;({union Cyc_Absyn_Cnst _tmp152=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp131.f1=_tmp152;}),_tmp131.f2=0;_tmp131;});case 0U: _LL3: _tmp7F=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_LL4:
 return({struct _tuple11 _tmp132;_tmp132.f1=_tmp7F,_tmp132.f2=1;_tmp132;});case 2U: _LL5: _LL6:
 return({struct _tuple11 _tmp133;({union Cyc_Absyn_Cnst _tmp153=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp133.f1=_tmp153;}),_tmp133.f2=1;_tmp133;});case 6U: _LL7: _tmp82=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_tmp81=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_tmp80=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4E)->f3;_LL8: {
# 184
struct _tuple12 _tmp4F=Cyc_Evexp_eval_const_bool_exp(_tmp82);struct _tuple12 _tmp50=_tmp4F;int _tmp52;int _tmp51;_LL20: _tmp52=_tmp50.f1;_tmp51=_tmp50.f2;_LL21:;
if(!_tmp51){
Cyc_Evexp_eval_const_exp(_tmp81);
Cyc_Evexp_eval_const_exp(_tmp80);
return({struct _tuple11 _tmp134;({union Cyc_Absyn_Cnst _tmp154=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp134.f1=_tmp154;}),_tmp134.f2=0;_tmp134;});}
# 190
ans=_tmp52?Cyc_Evexp_eval_const_exp(_tmp81): Cyc_Evexp_eval_const_exp(_tmp80);
goto _LL0;}case 7U: _LL9: _tmp84=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_tmp83=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LLA: {
# 193
struct _tuple12 _tmp53=Cyc_Evexp_eval_const_bool_exp(_tmp84);struct _tuple12 _tmp54=_tmp53;int _tmp56;int _tmp55;_LL23: _tmp56=_tmp54.f1;_tmp55=_tmp54.f2;_LL24:;
if(!_tmp55){
Cyc_Evexp_eval_const_exp(_tmp83);
return({struct _tuple11 _tmp135;({union Cyc_Absyn_Cnst _tmp155=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp135.f1=_tmp155;}),_tmp135.f2=0;_tmp135;});}
# 198
if(_tmp56)ans=Cyc_Evexp_eval_const_exp(_tmp83);else{
ans=({struct _tuple11 _tmp136;({union Cyc_Absyn_Cnst _tmp156=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp136.f1=_tmp156;}),_tmp136.f2=1;_tmp136;});}
goto _LL0;}case 8U: _LLB: _tmp86=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_tmp85=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LLC: {
# 202
struct _tuple12 _tmp57=Cyc_Evexp_eval_const_bool_exp(_tmp86);struct _tuple12 _tmp58=_tmp57;int _tmp5A;int _tmp59;_LL26: _tmp5A=_tmp58.f1;_tmp59=_tmp58.f2;_LL27:;
if(!_tmp59){
Cyc_Evexp_eval_const_exp(_tmp85);
return({struct _tuple11 _tmp137;({union Cyc_Absyn_Cnst _tmp157=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp137.f1=_tmp157;}),_tmp137.f2=0;_tmp137;});}
# 207
if(!_tmp5A)ans=Cyc_Evexp_eval_const_exp(_tmp85);else{
ans=({struct _tuple11 _tmp138;({union Cyc_Absyn_Cnst _tmp158=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);_tmp138.f1=_tmp158;}),_tmp138.f2=1;_tmp138;});}
goto _LL0;}case 3U: _LLD: _tmp88=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_tmp87=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LLE:
# 211
 if(_tmp87 == 0){
({void*_tmp5B=0U;({unsigned int _tmp15A=e->loc;struct _dyneither_ptr _tmp159=({const char*_tmp5C="bad static expression (no args to primop)";_tag_dyneither(_tmp5C,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp15A,_tmp159,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});
return({struct _tuple11 _tmp139;({union Cyc_Absyn_Cnst _tmp15B=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp139.f1=_tmp15B;}),_tmp139.f2=1;_tmp139;});}
# 215
if(_tmp87->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(_tmp88,(struct Cyc_Absyn_Exp*)_tmp87->hd);
goto _LL0;}
# 219
if(((struct Cyc_List_List*)_check_null(_tmp87->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(_tmp88,(struct Cyc_Absyn_Exp*)_tmp87->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp87->tl))->hd);
goto _LL0;}
# 223
({void*_tmp5D=0U;({unsigned int _tmp15D=e->loc;struct _dyneither_ptr _tmp15C=({const char*_tmp5E="bad static expression (too many args to primop)";_tag_dyneither(_tmp5E,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp15D,_tmp15C,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});
return({struct _tuple11 _tmp13A;({union Cyc_Absyn_Cnst _tmp15E=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp13A.f1=_tmp15E;}),_tmp13A.f2=1;_tmp13A;});case 39U: _LLF: _tmp89=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_LL10:
# 227
{void*_tmp5F=Cyc_Tcutil_compress(_tmp89);void*_tmp60=_tmp5F;struct Cyc_Absyn_Exp*_tmp61;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp60)->tag == 9U){_LL29: _tmp61=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp60)->f1;_LL2A:
# 229
 e->r=_tmp61->r;
return Cyc_Evexp_eval_const_exp(_tmp61);}else{_LL2B: _LL2C:
 goto _LL28;}_LL28:;}
# 233
goto _LL12;case 17U: _LL11: _LL12:
 goto _LL14;case 18U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 ans=({struct _tuple11 _tmp13B;({union Cyc_Absyn_Cnst _tmp15F=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp13B.f1=_tmp15F;}),_tmp13B.f2=0;_tmp13B;});goto _LL0;case 14U: _LL17: _tmp8B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_tmp8A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LL18:
# 239
 ans=Cyc_Evexp_eval_const_exp(_tmp8A);
if(ans.f2){
struct _tuple15 _tmp62=({struct _tuple15 _tmp13D;({void*_tmp160=Cyc_Tcutil_compress(_tmp8B);_tmp13D.f1=_tmp160;}),_tmp13D.f2=ans.f1;_tmp13D;});struct _tuple15 _tmp63=_tmp62;void*_tmp7E;enum Cyc_Absyn_Sign _tmp7D;int _tmp7C;void*_tmp7B;enum Cyc_Absyn_Sign _tmp7A;short _tmp79;void*_tmp78;enum Cyc_Absyn_Sign _tmp77;char _tmp76;enum Cyc_Absyn_Sign _tmp75;enum Cyc_Absyn_Size_of _tmp74;enum Cyc_Absyn_Sign _tmp73;int _tmp72;enum Cyc_Absyn_Sign _tmp71;enum Cyc_Absyn_Size_of _tmp70;enum Cyc_Absyn_Sign _tmp6F;short _tmp6E;enum Cyc_Absyn_Sign _tmp6D;enum Cyc_Absyn_Size_of _tmp6C;enum Cyc_Absyn_Sign _tmp6B;char _tmp6A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f1)){case 1U: switch(((_tmp63.f2).Int_c).tag){case 2U: _LL2E: _tmp6D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f1)->f1;_tmp6C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f1)->f2;_tmp6B=(((_tmp63.f2).Char_c).val).f1;_tmp6A=(((_tmp63.f2).Char_c).val).f2;_LL2F:
# 244
 _tmp71=_tmp6D;_tmp70=_tmp6C;_tmp6F=_tmp6B;_tmp6E=(short)_tmp6A;goto _LL31;case 4U: _LL30: _tmp71=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f1)->f1;_tmp70=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f1)->f2;_tmp6F=(((_tmp63.f2).Short_c).val).f1;_tmp6E=(((_tmp63.f2).Short_c).val).f2;_LL31:
# 246
 _tmp75=_tmp71;_tmp74=_tmp70;_tmp73=_tmp6F;_tmp72=(int)_tmp6E;goto _LL33;case 5U: _LL32: _tmp75=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f1)->f1;_tmp74=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f1)->f2;_tmp73=(((_tmp63.f2).Int_c).val).f1;_tmp72=(((_tmp63.f2).Int_c).val).f2;_LL33:
# 248
 if((int)_tmp75 != (int)_tmp73)
({union Cyc_Absyn_Cnst _tmp161=Cyc_Absyn_Int_c(_tmp75,_tmp72);ans.f1=_tmp161;});
goto _LL2D;default: goto _LL3A;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f2 != 0)switch(((_tmp63.f2).Int_c).tag){case 2U: _LL34: _tmp78=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f2)->hd;_tmp77=(((_tmp63.f2).Char_c).val).f1;_tmp76=(((_tmp63.f2).Char_c).val).f2;_LL35:
# 253
 _tmp7B=_tmp78;_tmp7A=_tmp77;_tmp79=(short)_tmp76;goto _LL37;case 4U: _LL36: _tmp7B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f2)->hd;_tmp7A=(((_tmp63.f2).Short_c).val).f1;_tmp79=(((_tmp63.f2).Short_c).val).f2;_LL37:
# 255
 _tmp7E=_tmp7B;_tmp7D=_tmp7A;_tmp7C=(int)_tmp79;goto _LL39;case 5U: _LL38: _tmp7E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63.f1)->f2)->hd;_tmp7D=(((_tmp63.f2).Int_c).val).f1;_tmp7C=(((_tmp63.f2).Int_c).val).f2;_LL39:
# 257
 if(_tmp7C < 0)
({void*_tmp64=0U;({unsigned int _tmp163=e->loc;struct _dyneither_ptr _tmp162=({const char*_tmp65="cannot cast negative number to a tag type";_tag_dyneither(_tmp65,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp163,_tmp162,_tag_dyneither(_tmp64,sizeof(void*),0U));});});
({void*_tmp165=_tmp7E;Cyc_Tcutil_unify(_tmp165,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=9U,({struct Cyc_Absyn_Exp*_tmp164=Cyc_Absyn_const_exp(ans.f1,0U);_tmp66->f1=_tmp164;});_tmp66;}));});
({union Cyc_Absyn_Cnst _tmp166=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp7C);ans.f1=_tmp166;});
goto _LL2D;default: goto _LL3A;}else{goto _LL3A;}default: goto _LL3A;}else{_LL3A: _LL3B:
({struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp13C;_tmp13C.tag=0U,({struct _dyneither_ptr _tmp167=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp8B));_tmp13C.f1=_tmp167;});_tmp13C;});void*_tmp67[1U];_tmp67[0]=& _tmp69;({unsigned int _tmp169=e->loc;struct _dyneither_ptr _tmp168=({const char*_tmp68="cannot cast to %s";_tag_dyneither(_tmp68,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp169,_tmp168,_tag_dyneither(_tmp67,sizeof(void*),1U));});});goto _LL2D;}_LL2D:;}
# 264
goto _LL0;case 33U: _LL19: _tmp8C=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LL1A:
 _tmp8D=_tmp8C;goto _LL1C;case 32U: _LL1B: _tmp8D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LL1C:
 ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp8D->tag));goto _LL0;default: _LL1D: _LL1E:
# 269
 return({struct _tuple11 _tmp13E;({union Cyc_Absyn_Cnst _tmp16A=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmp13E.f1=_tmp16A;}),_tmp13E.f2=0;_tmp13E;});}_LL0:;}
# 271
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmp8E=ans.f1;
({void*_tmp16B=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=0U,_tmp8F->f1=_tmp8E;_tmp8F;});e->r=_tmp16B;});}
# 276
return ans;}
# 279
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmp90=e->r;void*_tmp91=_tmp90;struct Cyc_Absyn_Exp*_tmp9B;enum Cyc_Absyn_Primop _tmp9A;struct Cyc_List_List*_tmp99;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp92;switch(*((int*)_tmp91)){case 33U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 0U: _LLB: _LLC:
 return 1;case 6U: _LLD: _tmp94=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp91)->f1;_tmp93=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp91)->f2;_tmp92=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp91)->f3;_LLE:
# 288
 return(Cyc_Evexp_c_can_eval(_tmp94) && Cyc_Evexp_c_can_eval(_tmp93)) && Cyc_Evexp_c_can_eval(_tmp92);case 7U: _LLF: _tmp96=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp91)->f1;_tmp95=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp91)->f2;_LL10:
 _tmp98=_tmp96;_tmp97=_tmp95;goto _LL12;case 8U: _LL11: _tmp98=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp91)->f1;_tmp97=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp91)->f2;_LL12:
# 291
 return Cyc_Evexp_c_can_eval(_tmp98) && Cyc_Evexp_c_can_eval(_tmp97);case 3U: _LL13: _tmp9A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp91)->f1;_tmp99=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp91)->f2;_LL14:
# 293
 for(0;_tmp99 != 0;_tmp99=_tmp99->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmp99->hd))return 0;}
return 1;case 39U: _LL15: _LL16:
 return 0;case 14U: _LL17: _tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp91)->f2;_LL18:
 return Cyc_Evexp_c_can_eval(_tmp9B);default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 302
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmp9C=e->r;void*_tmp9D=_tmp9C;switch(*((int*)_tmp9D)){case 0U: _LL1: _LL2:
 return 1;case 6U: _LL3: _LL4:
 return 2;case 3U: _LL5: _LL6:
 return 3;case 17U: _LL7: _LL8:
 goto _LLA;case 18U: _LL9: _LLA:
 return 4;case 19U: _LLB: _LLC:
 return 5;case 14U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 39U: _LL13: _LL14:
 return 9;default: _LL15: _LL16:
# 315
({struct Cyc_String_pa_PrintArg_struct _tmpA0=({struct Cyc_String_pa_PrintArg_struct _tmp13F;_tmp13F.tag=0U,({struct _dyneither_ptr _tmp16C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp13F.f1=_tmp16C;});_tmp13F;});void*_tmp9E[1U];_tmp9E[0]=& _tmpA0;({unsigned int _tmp16E=e->loc;struct _dyneither_ptr _tmp16D=({const char*_tmp9F="bad static expression %s";_tag_dyneither(_tmp9F,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp16E,_tmp16D,_tag_dyneither(_tmp9E,sizeof(void*),1U));});});return 0;}_LL0:;}
# 320
static struct Cyc_Absyn_Exp*Cyc_Evexp_strip_cast(struct Cyc_Absyn_Exp*e){
{void*_tmpA1=e->r;void*_tmpA2=_tmpA1;void*_tmpA6;struct Cyc_Absyn_Exp*_tmpA5;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA2)->tag == 14U){_LL1: _tmpA6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA2)->f1;_tmpA5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA2)->f2;_LL2:
# 323
 if(Cyc_Tcutil_unify(_tmpA6,Cyc_Absyn_uint_type)){
void*_tmpA3=_tmpA5->r;void*_tmpA4=_tmpA3;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA4)->tag == 39U){_LL6: _LL7:
 return _tmpA5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 329
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 332
return e;}struct _tuple16{void*f1;void*f2;};
# 335
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 337
e1=Cyc_Evexp_strip_cast(e1);
e2=Cyc_Evexp_strip_cast(e2);{
struct _tuple10 _tmpA7=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple10 _tmpA8=_tmpA7;unsigned int _tmpF2;int _tmpF1;_LL1: _tmpF2=_tmpA8.f1;_tmpF1=_tmpA8.f2;_LL2:;{
struct _tuple10 _tmpA9=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple10 _tmpAA=_tmpA9;unsigned int _tmpF0;int _tmpEF;_LL4: _tmpF0=_tmpAA.f1;_tmpEF=_tmpAA.f2;_LL5:;
if(_tmpF1  && _tmpEF)
return(int)(_tmpF2 - _tmpF0);{
int _tmpAB=Cyc_Evexp_const_exp_case_number(e1);
int _tmpAC=Cyc_Evexp_const_exp_case_number(e2);
if(_tmpAB != _tmpAC)
return _tmpAB - _tmpAC;{
struct _tuple16 _tmpAD=({struct _tuple16 _tmp141;_tmp141.f1=e1->r,_tmp141.f2=e2->r;_tmp141;});struct _tuple16 _tmpAE=_tmpAD;void*_tmpEE;void*_tmpED;void*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpE8;struct Cyc_List_List*_tmpE7;void*_tmpE6;struct Cyc_List_List*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;void*_tmpE1;void*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;void*_tmpDE;void*_tmpDD;enum Cyc_Absyn_Primop _tmpDC;struct Cyc_List_List*_tmpDB;enum Cyc_Absyn_Primop _tmpDA;struct Cyc_List_List*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;switch(*((int*)_tmpAE.f1)){case 6U: if(((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->tag == 6U){_LL7: _tmpD0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpCF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f2;_tmpCE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f3;_tmpCD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_tmpCC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f2;_tmpCB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f3;_LL8: {
# 352
int _tmpAF=Cyc_Evexp_const_exp_cmp(_tmpCE,_tmpCB);
if(_tmpAF != 0)return _tmpAF;
_tmpD4=_tmpD0;_tmpD3=_tmpCF;_tmpD2=_tmpCD;_tmpD1=_tmpCC;goto _LLA;}}else{goto _LL1D;}case 7U: if(((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->tag == 7U){_LL9: _tmpD4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpD3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f2;_tmpD2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_tmpD1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f2;_LLA:
 _tmpD8=_tmpD4;_tmpD7=_tmpD3;_tmpD6=_tmpD2;_tmpD5=_tmpD1;goto _LLC;}else{goto _LL1D;}case 8U: if(((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->tag == 8U){_LLB: _tmpD8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpD7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f2;_tmpD6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_tmpD5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f2;_LLC: {
# 357
int _tmpB0=Cyc_Evexp_const_exp_cmp(_tmpD8,_tmpD6);
if(_tmpB0 != 0)return _tmpB0;
return Cyc_Evexp_const_exp_cmp(_tmpD7,_tmpD5);}}else{goto _LL1D;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->tag == 3U){_LLD: _tmpDC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpDB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f2;_tmpDA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_tmpD9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f2;_LLE: {
# 361
int _tmpB1=(int)_tmpDC - (int)_tmpDA;
if(_tmpB1 != 0)return _tmpB1;
for(0;_tmpDB != 0  && _tmpD9 != 0;(_tmpDB=_tmpDB->tl,_tmpD9=_tmpD9->tl)){
int _tmpB2=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmpDB->hd,(struct Cyc_Absyn_Exp*)_tmpD9->hd);
if(_tmpB2 != 0)
return _tmpB2;}
# 368
return 0;}}else{goto _LL1D;}case 17U: switch(*((int*)_tmpAE.f2)){case 17U: _LLF: _tmpDE=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpDD=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_LL10:
# 370
 return Cyc_Tcutil_typecmp(_tmpDE,_tmpDD);case 18U: _LL11: _tmpE0=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpDF=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_LL12: {
# 372
void*_tmpB3=_tmpDF->topt;
if(_tmpB3 == 0)
({void*_tmpB4=0U;({unsigned int _tmp170=e2->loc;struct _dyneither_ptr _tmp16F=({const char*_tmpB5="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpB5,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp170,_tmp16F,_tag_dyneither(_tmpB4,sizeof(void*),0U));});});
return Cyc_Tcutil_typecmp(_tmpE0,(void*)_check_null(_tmpB3));}default: goto _LL1D;}case 18U: switch(*((int*)_tmpAE.f2)){case 17U: _LL13: _tmpE2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpE1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_LL14: {
# 377
void*_tmpB6=_tmpE2->topt;
if(_tmpB6 == 0)
({void*_tmpB7=0U;({unsigned int _tmp172=e1->loc;struct _dyneither_ptr _tmp171=({const char*_tmpB8="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpB8,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp172,_tmp171,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});
return Cyc_Tcutil_typecmp((void*)_check_null(_tmpB6),_tmpE1);}case 18U: _LL15: _tmpE4=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpE3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_LL16: {
# 382
void*_tmpB9=_tmpE4->topt;
void*_tmpBA=_tmpE3->topt;
if(_tmpB9 == 0)
({void*_tmpBB=0U;({unsigned int _tmp174=e1->loc;struct _dyneither_ptr _tmp173=({const char*_tmpBC="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpBC,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp174,_tmp173,_tag_dyneither(_tmpBB,sizeof(void*),0U));});});
if(_tmpBA == 0)
({void*_tmpBD=0U;({unsigned int _tmp176=e2->loc;struct _dyneither_ptr _tmp175=({const char*_tmpBE="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(_tmpBE,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp176,_tmp175,_tag_dyneither(_tmpBD,sizeof(void*),0U));});});
return({void*_tmp177=(void*)_check_null(_tmpB9);Cyc_Tcutil_typecmp(_tmp177,(void*)_check_null(_tmpBA));});}default: goto _LL1D;}case 19U: if(((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->tag == 19U){_LL17: _tmpE8=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpE7=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f2;_tmpE6=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_tmpE5=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f2;_LL18: {
# 390
int _tmpBF=Cyc_Tcutil_typecmp(_tmpE8,_tmpE6);
if(_tmpBF != 0)return _tmpBF;{
int _tmpC0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE7);
int _tmpC1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE5);
if(_tmpC0 < _tmpC1)return - 1;
if(_tmpC1 < _tmpC0)return 1;
for(0;_tmpE7 != 0  && _tmpE5 != 0;(_tmpE7=_tmpE7->tl,_tmpE5=_tmpE5->tl)){
struct _tuple16 _tmpC2=({struct _tuple16 _tmp140;_tmp140.f1=(void*)_tmpE7->hd,_tmp140.f2=(void*)_tmpE5->hd;_tmp140;});struct _tuple16 _tmpC3=_tmpC2;unsigned int _tmpC7;unsigned int _tmpC6;struct _dyneither_ptr*_tmpC5;struct _dyneither_ptr*_tmpC4;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpC3.f1)->tag == 0U){if(((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpC3.f2)->tag == 1U){_LL20: _LL21:
 return - 1;}else{_LL24: _tmpC5=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpC3.f1)->f1;_tmpC4=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpC3.f2)->f1;_LL25: {
# 401
int c=Cyc_strptrcmp(_tmpC5,_tmpC4);
if(c != 0)return c;
goto _LL1F;}}}else{if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpC3.f2)->tag == 0U){_LL22: _LL23:
# 399
 return 1;}else{_LL26: _tmpC7=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpC3.f1)->f1;_tmpC6=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpC3.f2)->f1;_LL27: {
# 405
int c=(int)(_tmpC7 - _tmpC6);
if(c != 0)return c;
goto _LL1F;}}}_LL1F:;}
# 409
return 0;};}}else{goto _LL1D;}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->tag == 14U){_LL19: _tmpEC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpEB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f2;_tmpEA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_tmpE9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f2;_LL1A: {
# 411
int _tmpC8=Cyc_Tcutil_typecmp(_tmpEC,_tmpEA);
if(_tmpC8 != 0)return _tmpC8;
return Cyc_Evexp_const_exp_cmp(_tmpEB,_tmpE9);}}else{goto _LL1D;}case 39U: if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->tag == 39U){_LL1B: _tmpEE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAE.f1)->f1;_tmpED=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAE.f2)->f1;_LL1C:
# 415
 if(Cyc_Tcutil_unify(_tmpEE,_tmpED))return 0;
return Cyc_Tcutil_typecmp(_tmpEE,_tmpED);}else{goto _LL1D;}default: _LL1D: _LL1E:
({void*_tmpC9=0U;({struct _dyneither_ptr _tmp178=({const char*_tmpCA="Evexp::const_exp_cmp, unexpected case";_tag_dyneither(_tmpCA,sizeof(char),38U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp178,_tag_dyneither(_tmpC9,sizeof(void*),0U));});});}_LL6:;};};};};}
# 420
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 423
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmpF3=Cyc_Evexp_eval_const_uint_exp(e1);struct _tuple10 _tmpF4=_tmpF3;unsigned int _tmpFA;int _tmpF9;_LL1: _tmpFA=_tmpF4.f1;_tmpF9=_tmpF4.f2;_LL2:;{
struct _tuple10 _tmpF5=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple10 _tmpF6=_tmpF5;unsigned int _tmpF8;int _tmpF7;_LL4: _tmpF8=_tmpF6.f1;_tmpF7=_tmpF6.f2;_LL5:;
if(_tmpF9  && _tmpF7)
return _tmpFA <= _tmpF8;
return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 433
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmpFB=Cyc_Tcutil_compress(t);void*_tmpFC=_tmpFB;struct Cyc_Absyn_Typedefdecl*_tmp115;struct Cyc_List_List*_tmp114;void*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_Tvar*_tmp110;void*_tmp10F;struct Cyc_List_List*_tmp10E;switch(*((int*)_tmpFC)){case 0U: _LL1: _tmp10F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFC)->f1;_tmp10E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpFC)->f2;_LL2: {
# 436
void*_tmpFD=_tmp10F;struct Cyc_Absyn_Aggrdecl*_tmp107;struct Cyc_Absyn_Datatypefield*_tmp106;switch(*((int*)_tmpFD)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpFD)->f1).KnownDatatypefield).tag == 2){_LL1C: _tmp106=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmpFD)->f1).KnownDatatypefield).val).f2;_LL1D:
# 438
{struct Cyc_List_List*_tmpFE=_tmp106->typs;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmpFE->hd)).f2))
return 0;}}
return 1;}else{_LL1E: _LL1F:
 return 0;}case 4U: _LL20: _LL21:
 goto _LL23;case 1U: _LL22: _LL23:
 goto _LL25;case 2U: _LL24: _LL25:
 goto _LL27;case 17U: _LL26: _LL27:
 goto _LL29;case 15U: _LL28: _LL29:
 goto _LL2B;case 3U: _LL2A: _LL2B:
 goto _LL2D;case 18U: _LL2C: _LL2D:
 goto _LL2F;case 16U: _LL2E: _LL2F:
 return 1;case 0U: _LL30: _LL31:
# 452
 goto _LL33;case 5U: _LL32: _LL33:
 goto _LL35;case 7U: _LL34: _LL35:
 goto _LL37;case 6U: _LL36: _LL37:
 goto _LL39;case 9U: _LL38: _LL39:
 goto _LL3B;case 8U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 goto _LL3F;case 12U: _LL3E: _LL3F:
 goto _LL41;case 13U: _LL40: _LL41:
 goto _LL43;case 14U: _LL42: _LL43:
 goto _LL45;case 10U: _LL44: _LL45:
 return 0;default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpFD)->f1).UnknownAggr).tag == 1){_LL46: _LL47:
# 464
({void*_tmpFF=0U;({struct _dyneither_ptr _tmp179=({const char*_tmp100="szof on unchecked StructType";_tag_dyneither(_tmp100,sizeof(char),29U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp179,_tag_dyneither(_tmpFF,sizeof(void*),0U));});});}else{_LL48: _tmp107=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmpFD)->f1).KnownAggr).val;_LL49:
# 466
 if(_tmp107->impl == 0)return 0;else{
# 468
struct _RegionHandle _tmp101=_new_region("temp");struct _RegionHandle*temp=& _tmp101;_push_region(temp);
{struct Cyc_List_List*_tmp102=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp107->tvs,_tmp10E);
{struct Cyc_List_List*_tmp103=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp107->impl))->fields;for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,_tmp102,((struct Cyc_Absyn_Aggrfield*)_tmp103->hd)->type))){int _tmp104=0;_npop_handler(0U);return _tmp104;}}}{
int _tmp105=1;_npop_handler(0U);return _tmp105;};}
# 469
;_pop_region(temp);}}}_LL1B:;}case 2U: _LL3: _tmp110=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpFC)->f1;_LL4: {
# 476
enum Cyc_Absyn_KindQual _tmp108=(Cyc_Tcutil_tvar_kind(_tmp110,& Cyc_Tcutil_bk))->kind;enum Cyc_Absyn_KindQual _tmp109=_tmp108;if(_tmp109 == Cyc_Absyn_BoxKind){_LL4B: _LL4C:
 return 1;}else{_LL4D: _LL4E:
 return 0;}_LL4A:;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFC)->f1 != 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpFC)->f1)->v)->kind == Cyc_Absyn_BoxKind){_LL5: _LL6:
# 480
 return 1;}else{goto _LL7;}}else{_LL7: _LL8:
 return 0;}case 6U: _LL9: _tmp111=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFC)->f1;_LLA:
# 483
 for(0;_tmp111 != 0;_tmp111=_tmp111->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmp111->hd)).f2))
return 0;}
return 1;case 3U: _LLB: _LLC:
# 489
 return 1;case 4U: _LLD: _tmp113=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFC)->f1).elt_type;_tmp112=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFC)->f1).num_elts;_LLE:
# 492
 return _tmp112 != 0;case 5U: _LLF: _LL10:
 return 0;case 7U: _LL11: _tmp114=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpFC)->f2;_LL12:
# 495
 for(0;_tmp114 != 0;_tmp114=_tmp114->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)_tmp114->hd)->type))
return 0;}
return 1;case 10U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 return 0;default: _LL19: _tmp115=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpFC)->f3;_LL1A:
# 504
 if(_tmp115 == 0  || _tmp115->kind == 0)
({void*_tmp10A=0U;({struct _dyneither_ptr _tmp17C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10D=({struct Cyc_String_pa_PrintArg_struct _tmp142;_tmp142.tag=0U,({struct _dyneither_ptr _tmp17A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp142.f1=_tmp17A;});_tmp142;});void*_tmp10B[1U];_tmp10B[0]=& _tmp10D;({struct _dyneither_ptr _tmp17B=({const char*_tmp10C="szof typedeftype %s";_tag_dyneither(_tmp10C,sizeof(char),20U);});Cyc_aprintf(_tmp17B,_tag_dyneither(_tmp10B,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp17C,_tag_dyneither(_tmp10A,sizeof(void*),0U));});});
return(int)((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp115->kind))->v)->kind == (int)Cyc_Absyn_BoxKind;}_LL0:;}
