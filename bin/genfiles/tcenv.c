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
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char *)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  if (!_cks_ptr) _throw_null(); \
  (_cks_ptr) + (elt_sz)*(index); })
#endif

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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 888 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 925
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 927
extern void*Cyc_Absyn_empty_effect;
# 929
extern void*Cyc_Absyn_false_type;
# 931
void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 946
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 978
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 66 "tcenv.h"
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 105
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 109
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
# 112
void*Cyc_Tcutil_compress(void*t);
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 141
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 178
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);struct _tuple10{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 181
struct _tuple10 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 210
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 214
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 267 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 313
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 315
int Cyc_Tcutil_new_tvar_id();
# 317
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10U]="Env_error";
# 50 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 52
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct _tuple11{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{enum Cyc_Tcenv_NewStatus in_new;int in_notreadctxt: 1;int in_lhs: 1;int abstract_ok: 1;int in_stmt_exp: 1;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct _tuple11*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;};
# 112
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(){
# 114
struct Cyc_Tcenv_Genv*_tmp0=({struct Cyc_Tcenv_Genv*_tmp3=_cycalloc(sizeof(*_tmp3));({struct Cyc_Dict_Dict _tmpED=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->aggrdecls=_tmpED;}),({
struct Cyc_Dict_Dict _tmpEC=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->datatypedecls=_tmpEC;}),({
struct Cyc_Dict_Dict _tmpEB=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->enumdecls=_tmpEB;}),({
struct Cyc_Dict_Dict _tmpEA=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->typedefs=_tmpEA;}),({
struct Cyc_Dict_Dict _tmpE9=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp3->ordinaries=_tmpE9;});_tmp3;});
({struct Cyc_Dict_Dict _tmpF1=({struct Cyc_Dict_Dict _tmpF0=_tmp0->datatypedecls;struct _tuple0*_tmpEF=Cyc_Absyn_exn_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpF0,_tmpEF,({struct Cyc_Absyn_Datatypedecl**_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Datatypedecl*_tmpEE=Cyc_Absyn_exn_tud();*_tmp1=_tmpEE;});_tmp1;}));});_tmp0->datatypedecls=_tmpF1;});
return({struct Cyc_Tcenv_Tenv*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->ns=0,_tmp2->ae=_tmp0,_tmp2->le=0,_tmp2->allow_valueof=0,_tmp2->in_extern_c_include=0,_tmp2->in_tempest=0,_tmp2->tempest_generalize=0;_tmp2;});}struct _tuple12{void*f1;int f2;};
# 122
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q,int is_use){
struct _tuple12*_tmp4=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
if(is_use)
(*_tmp4).f2=1;
return(*_tmp4).f1;}
# 128
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->aggrdecls,q);}
# 131
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);}
# 134
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmp5=_region_malloc(r,sizeof(*_tmp5));({struct Cyc_Absyn_Datatypedecl**_tmpF2=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);*_tmp5=_tmpF2;});_tmp5;});}
# 137
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);}
# 140
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->typedefs,q);}
# 146
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp6=te->le;
if(_tmp6 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp6;}
# 151
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp7="put_fenv";_tag_dyneither(_tmp7,sizeof(char),9U);}));{
struct Cyc_Tcenv_Tenv*_tmp8=({struct Cyc_Tcenv_Tenv*_tmp9=_cycalloc(sizeof(*_tmp9));*_tmp9=*te;_tmp9;});
_tmp8->le=fe;
return _tmp8;};}
# 157
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmpA=({struct Cyc_Tcenv_Tenv*_tmpB=_cycalloc(sizeof(*_tmpB));*_tmpB=*te;_tmpB;});
_tmpA->le=0;
return _tmpA;}
# 163
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
return(({struct Cyc_Tcenv_Tenv*_tmpF3=te;Cyc_Tcenv_get_fenv(_tmpF3,({const char*_tmpC="return_typ";_tag_dyneither(_tmpC,sizeof(char),11U);}));})->shared)->return_typ;}
# 167
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD=te->le;
if(te->le == 0)return 0;
return((struct Cyc_Tcenv_Fenv*)_check_null(_tmpD))->type_vars;}
# 173
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpE=_cycalloc(sizeof(*_tmpE));({struct Cyc_List_List*_tmpF4=Cyc_Tcenv_lookup_type_vars(te);_tmpE->v=_tmpF4;});_tmpE;});}
# 177
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
struct Cyc_Tcenv_Fenv*_tmpF=({struct Cyc_Tcenv_Fenv*_tmp12=_cycalloc(sizeof(*_tmp12));({struct Cyc_Tcenv_Fenv _tmpF6=*({struct Cyc_Tcenv_Tenv*_tmpF5=te;Cyc_Tcenv_get_fenv(_tmpF5,({const char*_tmp11="add_type_vars";_tag_dyneither(_tmp11,sizeof(char),14U);}));});*_tmp12=_tmpF6;});_tmp12;});
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpF->type_vars);
Cyc_Tcutil_check_unique_tvars(loc,_tmp10);
_tmpF->type_vars=_tmp10;
return Cyc_Tcenv_put_fenv(te,_tmpF);};}
# 186
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp13=te->le;
if(_tmp13 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp14=({struct Cyc_Tcenv_Fenv*_tmp15=_cycalloc(sizeof(*_tmp15));*_tmp15=*_tmp13;_tmp15;});
(_tmp14->flags).in_new=status;
return Cyc_Tcenv_put_fenv(te,_tmp14);};}
# 193
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16=te->le;
if(_tmp16 == 0)return Cyc_Tcenv_NoneNew;
return(_tmp16->flags).in_new;}
# 198
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17=te->le;
if(_tmp17 == 0)return 0;
return(_tmp17->flags).abstract_ok;}
# 203
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18=te->le;
if(_tmp18 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp19=({struct Cyc_Tcenv_Fenv*_tmp1A=_cycalloc(sizeof(*_tmp1A));*_tmp1A=*_tmp18;_tmp1A;});
(_tmp19->flags).abstract_ok=1;
return Cyc_Tcenv_put_fenv(te,_tmp19);};}
# 210
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1B=te->le;
if(_tmp1B == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1C=({struct Cyc_Tcenv_Fenv*_tmp1D=_cycalloc(sizeof(*_tmp1D));*_tmp1D=*_tmp1B;_tmp1D;});
(_tmp1C->flags).abstract_ok=0;
return Cyc_Tcenv_put_fenv(te,_tmp1C);};}
# 217
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1E=te->le;
if(_tmp1E == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1F=({struct Cyc_Tcenv_Fenv*_tmp20=_cycalloc(sizeof(*_tmp20));*_tmp20=*_tmp1E;_tmp20;});
(_tmp1F->flags).in_notreadctxt=1;
return Cyc_Tcenv_put_fenv(te,_tmp1F);};}
# 224
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp21=te->le;
if(_tmp21 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp22=({struct Cyc_Tcenv_Fenv*_tmp23=_cycalloc(sizeof(*_tmp23));*_tmp23=*_tmp21;_tmp23;});
(_tmp22->flags).in_notreadctxt=0;
return Cyc_Tcenv_put_fenv(te,_tmp22);};}
# 231
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp24=te->le;
if(_tmp24 == 0)return 0;
return(_tmp24->flags).in_notreadctxt;}
# 236
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25=te->le;
if(_tmp25 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp26=({struct Cyc_Tcenv_Fenv*_tmp27=_cycalloc(sizeof(*_tmp27));*_tmp27=*_tmp25;_tmp27;});
(_tmp26->flags).in_lhs=1;
return Cyc_Tcenv_put_fenv(te,_tmp26);};}
# 243
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp28=te->le;
if(_tmp28 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp29=({struct Cyc_Tcenv_Fenv*_tmp2A=_cycalloc(sizeof(*_tmp2A));*_tmp2A=*_tmp28;_tmp2A;});
(_tmp29->flags).in_lhs=0;
return Cyc_Tcenv_put_fenv(te,_tmp29);};}
# 250
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B=te->le;
if(_tmp2B == 0)return 0;
return(_tmp2B->flags).in_lhs;}
# 255
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2C=te->le;
if(_tmp2C == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp2D=({struct Cyc_Tcenv_Fenv*_tmp2E=_cycalloc(sizeof(*_tmp2E));*_tmp2E=*_tmp2C;_tmp2E;});
(_tmp2D->flags).in_stmt_exp=1;
return Cyc_Tcenv_put_fenv(te,_tmp2D);};}
# 262
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2F=te->le;
if(_tmp2F == 0)return 0;
return(_tmp2F->flags).in_stmt_exp;}
# 269
const struct _tuple11*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 271
const struct _tuple11*_tmp30=({struct Cyc_Tcenv_Tenv*_tmpF7=te;Cyc_Tcenv_get_fenv(_tmpF7,({const char*_tmp32="process_fallthru";_tag_dyneither(_tmp32,sizeof(char),17U);}));})->ctrl_env;
if(_tmp30 != (const struct _tuple11*)0)
({struct Cyc_Absyn_Switch_clause**_tmpF8=({struct Cyc_Absyn_Switch_clause**_tmp31=_cycalloc(sizeof(*_tmp31));*_tmp31=(*_tmp30).f1;_tmp31;});*clauseopt=_tmpF8;});
return _tmp30;}
# 277
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 279
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,_tmp33->tl=ft_typ;_tmp33;});}{
struct _tuple11*_tmp34=({struct _tuple11*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->f1=clause,_tmp38->f2=new_tvs,({struct Cyc_List_List*_tmpF9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);_tmp38->f3=_tmpF9;});_tmp38;});
struct Cyc_Tcenv_Fenv*_tmp35=({struct Cyc_Tcenv_Fenv*_tmp37=_cycalloc(sizeof(*_tmp37));({struct Cyc_Tcenv_Fenv _tmpFB=*({struct Cyc_Tcenv_Tenv*_tmpFA=te;Cyc_Tcenv_get_fenv(_tmpFA,({const char*_tmp36="set_fallthru";_tag_dyneither(_tmp36,sizeof(char),13U);}));});*_tmp37=_tmpFB;});_tmp37;});
_tmp35->ctrl_env=(const struct _tuple11*)_tmp34;
return Cyc_Tcenv_put_fenv(te,_tmp35);};}
# 288
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Fenv*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct Cyc_Tcenv_Fenv _tmpFD=*({struct Cyc_Tcenv_Tenv*_tmpFC=te;Cyc_Tcenv_get_fenv(_tmpFC,({const char*_tmp39="clear_fallthru";_tag_dyneither(_tmp39,sizeof(char),15U);}));});*_tmp3A=_tmpFD;});_tmp3A;});
fe->ctrl_env=0;
return Cyc_Tcenv_put_fenv(te,fe);}
# 294
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3B=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp3E=_cycalloc(sizeof(*_tmp3E));*_tmp3E=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp3E;});
struct Cyc_Tcenv_Tenv*_tmp3C=({struct Cyc_Tcenv_Tenv*_tmp3D=_cycalloc(sizeof(*_tmp3D));*_tmp3D=*te;_tmp3D;});
_tmp3C->allow_valueof=1;
return _tmp3C;}
# 300
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3F=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp42=_cycalloc(sizeof(*_tmp42));*_tmp42=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp42;});
struct Cyc_Tcenv_Tenv*_tmp40=({struct Cyc_Tcenv_Tenv*_tmp41=_cycalloc(sizeof(*_tmp41));*_tmp41=*te;_tmp41;});
_tmp40->in_extern_c_include=1;
return _tmp40;}
# 306
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp43=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp46;});
struct Cyc_Tcenv_Tenv*_tmp44=({struct Cyc_Tcenv_Tenv*_tmp45=_cycalloc(sizeof(*_tmp45));*_tmp45=*te;_tmp45;});
_tmp44->in_tempest=1;
return _tmp44;}
# 312
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp47=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp4A;});
struct Cyc_Tcenv_Tenv*_tmp48=({struct Cyc_Tcenv_Tenv*_tmp49=_cycalloc(sizeof(*_tmp49));*_tmp49=*te;_tmp49;});
_tmp48->in_tempest=0;
return _tmp48;}
# 318
int Cyc_Tcenv_in_tempest(struct Cyc_Tcenv_Tenv*te){
return te->in_tempest;}
# 321
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*rgn,int opened){
struct Cyc_Tcenv_Fenv*_tmp4B=({struct Cyc_Tcenv_Fenv*_tmp53=_cycalloc(sizeof(*_tmp53));({struct Cyc_Tcenv_Fenv _tmpFF=*({struct Cyc_Tcenv_Tenv*_tmpFE=te;Cyc_Tcenv_get_fenv(_tmpFE,({const char*_tmp52="add_region";_tag_dyneither(_tmp52,sizeof(char),11U);}));});*_tmp53=_tmpFF;});_tmp53;});
struct Cyc_RgnOrder_RgnPO*_tmp4C=_tmp4B->region_order;
{void*_tmp4D=Cyc_Tcutil_compress(rgn);void*_tmp4E=_tmp4D;struct Cyc_Absyn_Tvar*_tmp4F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->tag == 2U){_LL1: _tmp4F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4E)->f1;_LL2:
# 326
 _tmp4C=Cyc_RgnOrder_add_youngest(_tmp4C,_tmp4F,opened);
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 330
_tmp4B->region_order=_tmp4C;
({void*_tmp102=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));({void*_tmp101=Cyc_Absyn_access_eff(rgn);_tmp51->hd=_tmp101;}),({
struct Cyc_List_List*_tmp100=({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->hd=_tmp4B->capability,_tmp50->tl=0;_tmp50;});_tmp51->tl=_tmp100;});_tmp51;}));
# 331
_tmp4B->capability=_tmp102;});
# 333
return Cyc_Tcenv_put_fenv(te,_tmp4B);}
# 336
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
struct Cyc_Tcenv_Fenv*_tmp54=({struct Cyc_Tcenv_Fenv*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Tcenv_Fenv _tmp104=*({struct Cyc_Tcenv_Tenv*_tmp103=te;Cyc_Tcenv_get_fenv(_tmp103,({const char*_tmp5A="new_named_block";_tag_dyneither(_tmp5A,sizeof(char),16U);}));});*_tmp5B=_tmp104;});_tmp5B;});
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp55=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=2U,_tmp59->f1=block_rgn;_tmp59;});
({struct Cyc_List_List*_tmp105=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->hd=block_rgn,_tmp56->tl=_tmp54->type_vars;_tmp56;});_tmp54->type_vars=_tmp105;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp54->type_vars);
({struct Cyc_RgnOrder_RgnPO*_tmp106=Cyc_RgnOrder_add_youngest(_tmp54->region_order,block_rgn,0);_tmp54->region_order=_tmp106;});
({void*_tmp109=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({void*_tmp108=Cyc_Absyn_access_eff((void*)_tmp55);_tmp58->hd=_tmp108;}),({
struct Cyc_List_List*_tmp107=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=_tmp54->capability,_tmp57->tl=0;_tmp57;});_tmp58->tl=_tmp107;});_tmp58;}));
# 342
_tmp54->capability=_tmp109;});
# 344
_tmp54->curr_rgn=(void*)_tmp55;
return Cyc_Tcenv_put_fenv(te,_tmp54);}
# 348
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0U,& Cyc_Tcutil_rk};
# 350
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 356
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned loc){
# 358
struct Cyc_Tcenv_Fenv*_tmp5C=({struct Cyc_Tcenv_Fenv*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct Cyc_Tcenv_Fenv _tmp10B=*({struct Cyc_Tcenv_Tenv*_tmp10A=te;Cyc_Tcenv_get_fenv(_tmp10A,({const char*_tmp5D="new_outlives_constraints";_tag_dyneither(_tmp5D,sizeof(char),25U);}));});*_tmp5E=_tmp10B;});_tmp5E;});
for(0;cs != 0;cs=cs->tl){
({struct Cyc_RgnOrder_RgnPO*_tmp10C=
Cyc_RgnOrder_add_outlives_constraint(_tmp5C->region_order,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);
# 360
_tmp5C->region_order=_tmp10C;});}
# 362
return Cyc_Tcenv_put_fenv(te,_tmp5C);}
# 365
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**oldtv,unsigned loc){
# 369
void*_tmp5F=Cyc_Tcutil_compress(r1);
void*_tmp60=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp61=Cyc_Tcutil_type_kind(_tmp5F);
struct Cyc_Absyn_Kind*_tmp62=Cyc_Tcutil_type_kind(_tmp60);
# 375
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp61,_tmp62);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp62,_tmp61);
if(!r1_le_r2 && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmpE3;_tmpE3.tag=0U,({
struct _dyneither_ptr _tmp10D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp5F));_tmpE3.f1=_tmp10D;});_tmpE3;});struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmpE2;_tmpE2.tag=0U,({struct _dyneither_ptr _tmp10E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp60));_tmpE2.f1=_tmp10E;});_tmpE2;});struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0U,({struct _dyneither_ptr _tmp10F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp61));_tmpE1.f1=_tmp10F;});_tmpE1;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0U,({struct _dyneither_ptr _tmp110=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp62));_tmpE0.f1=_tmp110;});_tmpE0;});void*_tmp63[4U];_tmp63[0]=& _tmp65,_tmp63[1]=& _tmp66,_tmp63[2]=& _tmp67,_tmp63[3]=& _tmp68;({unsigned _tmp112=loc;struct _dyneither_ptr _tmp111=({const char*_tmp64="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp64,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp112,_tmp111,_tag_dyneither(_tmp63,sizeof(void*),4U));});});
return Cyc_Tcenv_new_outlives_constraints(te,0,loc);}else{
# 382
if(r1_le_r2 && !r2_le_r1)
({struct _tuple10*_tmp115=({struct _tuple10*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _tuple10 _tmp114=({void*_tmp113=_tmp60;Cyc_Tcutil_swap_kind(_tmp113,Cyc_Tcutil_kind_to_bound(_tmp61));});*_tmp69=_tmp114;});_tmp69;});*oldtv=_tmp115;});else{
if(!r1_le_r2 && r2_le_r1)
({struct _tuple10*_tmp118=({struct _tuple10*_tmp6A=_cycalloc(sizeof(*_tmp6A));({struct _tuple10 _tmp117=({void*_tmp116=_tmp5F;Cyc_Tcutil_swap_kind(_tmp116,Cyc_Tcutil_kind_to_bound(_tmp62));});*_tmp6A=_tmp117;});_tmp6A;});*oldtv=_tmp118;});}}{
# 388
struct Cyc_List_List*_tmp6B=0;
if((_tmp5F != Cyc_Absyn_heap_rgn_type && _tmp5F != Cyc_Absyn_unique_rgn_type)&& _tmp5F != Cyc_Absyn_refcnt_rgn_type){
void*eff1=Cyc_Absyn_access_eff(_tmp5F);
_tmp6B=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct _tuple13*_tmp119=({struct _tuple13*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->f1=eff1,_tmp6C->f2=_tmp60;_tmp6C;});_tmp6D->hd=_tmp119;}),_tmp6D->tl=_tmp6B;_tmp6D;});}
# 393
if((_tmp60 != Cyc_Absyn_heap_rgn_type && _tmp60 != Cyc_Absyn_unique_rgn_type)&& _tmp60 != Cyc_Absyn_refcnt_rgn_type){
void*eff2=Cyc_Absyn_access_eff(_tmp60);
_tmp6B=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct _tuple13*_tmp11A=({struct _tuple13*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=eff2,_tmp6E->f2=_tmp5F;_tmp6E;});_tmp6F->hd=_tmp11A;}),_tmp6F->tl=_tmp6B;_tmp6F;});}
# 397
return Cyc_Tcenv_new_outlives_constraints(te,_tmp6B,loc);};}
# 400
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp70=te->le;
if(_tmp70 == 0)return Cyc_Absyn_heap_rgn_type;
return _tmp70->curr_rgn;}
# 409
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Tenv*_tmp11B=te;Cyc_Tcenv_get_fenv(_tmp11B,({const char*_tmp77="check_rgn_accessible";_tag_dyneither(_tmp77,sizeof(char),21U);}));});
struct Cyc_Tcenv_Fenv*_tmp71=fe;void*_tmp76;struct Cyc_RgnOrder_RgnPO*_tmp75;_LL1: _tmp76=_tmp71->capability;_tmp75=_tmp71->region_order;_LL2:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp76)||
 Cyc_Tcutil_region_in_effect(1,rgn,_tmp76))
return;
if(({struct Cyc_RgnOrder_RgnPO*_tmp11D=_tmp75;void*_tmp11C=Cyc_Absyn_access_eff(rgn);Cyc_RgnOrder_eff_outlives_eff(_tmp11D,_tmp11C,_tmp76);}))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp74=({struct Cyc_String_pa_PrintArg_struct _tmpE4;_tmpE4.tag=0U,({struct _dyneither_ptr _tmp11E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));_tmpE4.f1=_tmp11E;});_tmpE4;});void*_tmp72[1U];_tmp72[0]=& _tmp74;({unsigned _tmp120=loc;struct _dyneither_ptr _tmp11F=({const char*_tmp73="Expression accesses unavailable region %s";_tag_dyneither(_tmp73,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp120,_tmp11F,_tag_dyneither(_tmp72,sizeof(void*),1U));});});}
# 423
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp78=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 428
if(_tmp78 == 0){
void*_tmp79=rt_a;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp79)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp79)->f1)){case 7U: _LL1: _LL2:
 return rt_b != Cyc_Absyn_unique_rgn_type;case 6U: _LL3: _LL4:
 return rt_b != Cyc_Absyn_refcnt_rgn_type;case 5U: _LL5: _LL6:
 return rt_b == Cyc_Absyn_heap_rgn_type;default: goto _LL7;}else{_LL7: _LL8:
 return 0;}_LL0:;}{
# 436
int _tmp7A=({struct Cyc_RgnOrder_RgnPO*_tmp122=_tmp78->region_order;void*_tmp121=Cyc_Absyn_access_eff(rt_a);Cyc_RgnOrder_effect_outlives(_tmp122,_tmp121,rt_b);});
# 440
return _tmp7A;};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned f4;};
# 445
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp7B=({
struct Cyc_Tcenv_Tenv*_tmp123=te;Cyc_Tcenv_get_fenv(_tmp123,({const char*_tmp82="check_effect_accessible";_tag_dyneither(_tmp82,sizeof(char),24U);}));});
# 446
struct Cyc_Tcenv_Fenv*_tmp7C=_tmp7B;void*_tmp81;struct Cyc_RgnOrder_RgnPO*_tmp80;struct Cyc_Tcenv_SharedFenv*_tmp7F;_LL1: _tmp81=_tmp7C->capability;_tmp80=_tmp7C->region_order;_tmp7F=_tmp7C->shared;_LL2:;
# 448
if(Cyc_Tcutil_subset_effect(0,eff,_tmp81))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp80,eff,_tmp81))
return;
({struct Cyc_List_List*_tmp125=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));
({struct _tuple14*_tmp124=({struct _tuple14*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->f1=_tmp81,_tmp7D->f2=eff,_tmp7D->f3=_tmp80,_tmp7D->f4=loc;_tmp7D;});_tmp7E->hd=_tmp124;}),_tmp7E->tl=_tmp7F->delayed_effect_checks;_tmp7E;});
# 452
_tmp7F->delayed_effect_checks=_tmp125;});}
# 455
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp83=(({
struct Cyc_Tcenv_Tenv*_tmp126=te;Cyc_Tcenv_get_fenv(_tmp126,({const char*_tmp8E="check_delayed_constraints";_tag_dyneither(_tmp8E,sizeof(char),26U);}));})->shared)->delayed_effect_checks;
for(0;_tmp83 != 0;_tmp83=_tmp83->tl){
struct _tuple14*_tmp84=(struct _tuple14*)_tmp83->hd;struct _tuple14*_tmp85=_tmp84;void*_tmp8D;void*_tmp8C;struct Cyc_RgnOrder_RgnPO*_tmp8B;unsigned _tmp8A;_LL1: _tmp8D=_tmp85->f1;_tmp8C=_tmp85->f2;_tmp8B=_tmp85->f3;_tmp8A=_tmp85->f4;_LL2:;
if(Cyc_Tcutil_subset_effect(1,_tmp8C,_tmp8D))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp8B,_tmp8C,_tmp8D))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp88=({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0U,({
struct _dyneither_ptr _tmp127=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp8D));_tmpE6.f1=_tmp127;});_tmpE6;});struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmpE5;_tmpE5.tag=0U,({struct _dyneither_ptr _tmp128=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp8C));_tmpE5.f1=_tmp128;});_tmpE5;});void*_tmp86[2U];_tmp86[0]=& _tmp88,_tmp86[1]=& _tmp89;({unsigned _tmp12A=_tmp8A;struct _dyneither_ptr _tmp129=({const char*_tmp87="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp87,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp12A,_tmp129,_tag_dyneither(_tmp86,sizeof(void*),2U));});});}}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned f3;};
# 473
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*po){
# 475
struct Cyc_Tcenv_Fenv*_tmp8F=te->le;
if(_tmp8F == 0){
# 478
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect)|| !({
void*_tmp12B=Cyc_Absyn_access_eff((*((struct _tuple13*)po->hd)).f2);Cyc_Tcutil_subset_effect(1,_tmp12B,Cyc_Absyn_empty_effect);}))
({void*_tmp90=0U;({unsigned _tmp12D=loc;struct _dyneither_ptr _tmp12C=({const char*_tmp91="the required region ordering is not satisfied here";_tag_dyneither(_tmp91,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp12D,_tmp12C,_tag_dyneither(_tmp90,sizeof(void*),0U));});});}
return;}{
# 484
struct Cyc_Tcenv_Fenv*_tmp92=_tmp8F;struct Cyc_Tcenv_Fenv*_tmp93=_tmp92;struct Cyc_RgnOrder_RgnPO*_tmp97;struct Cyc_Tcenv_SharedFenv*_tmp96;_LL1: _tmp97=_tmp93->region_order;_tmp96=_tmp93->shared;_LL2:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp97,po,Cyc_Absyn_heap_rgn_type,0))
({struct Cyc_List_List*_tmp12F=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));
({struct _tuple15*_tmp12E=({struct _tuple15*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=_tmp97,_tmp94->f2=po,_tmp94->f3=loc;_tmp94;});_tmp95->hd=_tmp12E;}),_tmp95->tl=_tmp96->delayed_constraint_checks;_tmp95;});
# 486
_tmp96->delayed_constraint_checks=_tmp12F;});};}
# 490
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp98=(({
struct Cyc_Tcenv_Tenv*_tmp130=te;Cyc_Tcenv_get_fenv(_tmp130,({const char*_tmpA0="check_delayed_constraints";_tag_dyneither(_tmpA0,sizeof(char),26U);}));})->shared)->delayed_constraint_checks;
for(0;_tmp98 != 0;_tmp98=_tmp98->tl){
struct _tuple15*_tmp99=(struct _tuple15*)_tmp98->hd;struct _tuple15*_tmp9A=_tmp99;struct Cyc_RgnOrder_RgnPO*_tmp9F;struct Cyc_List_List*_tmp9E;unsigned _tmp9D;_LL1: _tmp9F=_tmp9A->f1;_tmp9E=_tmp9A->f2;_tmp9D=_tmp9A->f3;_LL2:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp9F,_tmp9E,Cyc_Absyn_heap_rgn_type,1))
({void*_tmp9B=0U;({unsigned _tmp132=_tmp9D;struct _dyneither_ptr _tmp131=({const char*_tmp9C="the required region ordering is not satisfied here";_tag_dyneither(_tmp9C,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp132,_tmp131,_tag_dyneither(_tmp9B,sizeof(void*),0U));});});}}
# 500
static struct Cyc_Tcenv_SharedFenv*Cyc_Tcenv_new_shared_fenv(void*ret){
return({struct Cyc_Tcenv_SharedFenv*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->return_typ=ret,_tmpA1->delayed_effect_checks=0,_tmpA1->delayed_constraint_checks=0;_tmpA1;});}
# 506
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpB6=_cycalloc(sizeof(*_tmpB6));
({struct _dyneither_ptr*_tmp136=({struct _dyneither_ptr*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _dyneither_ptr _tmp135=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB4=({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0U,_tmpE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);_tmpE7;});void*_tmpB2[1U];_tmpB2[0]=& _tmpB4;({struct _dyneither_ptr _tmp134=({const char*_tmpB3="`%s";_tag_dyneither(_tmpB3,sizeof(char),4U);});Cyc_aprintf(_tmp134,_tag_dyneither(_tmpB2,sizeof(void*),1U));});});*_tmpB5=_tmp135;});_tmpB5;});_tmpB6->name=_tmp136;}),({
int _tmp133=Cyc_Tcutil_new_tvar_id();_tmpB6->identity=_tmp133;}),_tmpB6->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpB6;});
struct Cyc_List_List*_tmpA2=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->hd=rgn0,_tmpB1->tl=(fd->i).tvars;_tmpB1;});
Cyc_Tcutil_check_unique_tvars(loc,_tmpA2);{
# 513
struct Cyc_RgnOrder_RgnPO*_tmpA3=Cyc_RgnOrder_initial_fn_po((fd->i).tvars,(fd->i).rgn_po,(void*)_check_null((fd->i).effect),rgn0,loc);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=2U,_tmpB0->f1=rgn0;_tmpB0;});
{struct Cyc_List_List*_tmpA4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpA4 != 0;_tmpA4=_tmpA4->tl){
((struct Cyc_Absyn_Vardecl*)_tmpA4->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpA5=*((struct Cyc_Absyn_VarargInfo*)_check_null((fd->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmpA6=_tmpA5;struct _dyneither_ptr*_tmpAC;struct Cyc_Absyn_Tqual _tmpAB;void*_tmpAA;int _tmpA9;_LL1: _tmpAC=_tmpA6.name;_tmpAB=_tmpA6.tq;_tmpAA=_tmpA6.type;_tmpA9=_tmpA6.inject;_LL2:;{
# 520
struct Cyc_List_List*_tmpA7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpA7 != 0;_tmpA7=_tmpA7->tl){
struct Cyc_Absyn_Vardecl*_tmpA8=(struct Cyc_Absyn_Vardecl*)_tmpA7->hd;
if(Cyc_strptrcmp((*_tmpA8->name).f2,(struct _dyneither_ptr*)_check_null(_tmpAC))== 0){
({void*_tmp137=Cyc_Absyn_fatptr_type(_tmpAA,param_rgn,_tmpAB,Cyc_Absyn_false_type);_tmpA8->type=_tmp137;});
break;}}};}
# 528
return({struct Cyc_Tcenv_Fenv*_tmpAF=_cycalloc(sizeof(*_tmpAF));
({struct Cyc_Tcenv_SharedFenv*_tmp13B=Cyc_Tcenv_new_shared_fenv((fd->i).ret_type);_tmpAF->shared=_tmp13B;}),_tmpAF->type_vars=_tmpA2,_tmpAF->region_order=_tmpA3,_tmpAF->ctrl_env=0,({
# 533
void*_tmp13A=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpAE=_cycalloc(sizeof(*_tmpAE));({void*_tmp139=Cyc_Absyn_access_eff(param_rgn);_tmpAE->hd=_tmp139;}),({
struct Cyc_List_List*_tmp138=({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->hd=(void*)_check_null((fd->i).effect),_tmpAD->tl=0;_tmpAD;});_tmpAE->tl=_tmp138;});_tmpAE;}));
# 533
_tmpAF->capability=_tmp13A;}),_tmpAF->curr_rgn=param_rgn,
# 536
(_tmpAF->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpAF->flags).in_notreadctxt=0,(_tmpAF->flags).in_lhs=0,(_tmpAF->flags).abstract_ok=0,(_tmpAF->flags).in_stmt_exp=0;_tmpAF;});};}
# 540
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmpB7=old_fenv;struct Cyc_RgnOrder_RgnPO*_tmpD6;struct Cyc_List_List*_tmpD5;struct Cyc_Tcenv_SharedFenv*_tmpD4;_LL1: _tmpD6=_tmpB7->region_order;_tmpD5=_tmpB7->type_vars;_tmpD4=_tmpB7->shared;_LL2:;{
# 543
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpD3=_cycalloc(sizeof(*_tmpD3));
({struct _dyneither_ptr*_tmp13F=({struct _dyneither_ptr*_tmpD2=_cycalloc(sizeof(*_tmpD2));({struct _dyneither_ptr _tmp13E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmpE8;_tmpE8.tag=0U,_tmpE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);_tmpE8;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({struct _dyneither_ptr _tmp13D=({const char*_tmpD0="`%s";_tag_dyneither(_tmpD0,sizeof(char),4U);});Cyc_aprintf(_tmp13D,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});*_tmpD2=_tmp13E;});_tmpD2;});_tmpD3->name=_tmp13F;}),({
int _tmp13C=Cyc_Tcutil_new_tvar_id();_tmpD3->identity=_tmp13C;}),_tmpD3->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpD3;});
{struct Cyc_List_List*_tmpB8=(fd->i).tvars;for(0;_tmpB8 != 0;_tmpB8=_tmpB8->tl){
struct Cyc_Absyn_Kind*_tmpB9=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmpB8->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpBA=_tmpB9;enum Cyc_Absyn_KindQual _tmpBE;enum Cyc_Absyn_AliasQual _tmpBD;_LL4: _tmpBE=_tmpBA->kind;_tmpBD=_tmpBA->aliasqual;_LL5:;
if((int)_tmpBE == (int)3U){
if((int)_tmpBD == (int)0U)
_tmpD6=Cyc_RgnOrder_add_unordered(_tmpD6,(struct Cyc_Absyn_Tvar*)_tmpB8->hd);else{
# 552
({void*_tmpBB=0U;({struct _dyneither_ptr _tmp140=({const char*_tmpBC="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmpBC,sizeof(char),39U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp140,_tag_dyneither(_tmpBB,sizeof(void*),0U));});});}}}}
# 554
_tmpD6=Cyc_RgnOrder_add_youngest(_tmpD6,rgn0,0);
{struct Cyc_List_List*_tmpBF=(fd->i).rgn_po;for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
_tmpD6=Cyc_RgnOrder_add_outlives_constraint(_tmpD6,(*((struct _tuple13*)_tmpBF->hd)).f1,(*((struct _tuple13*)_tmpBF->hd)).f2,loc);}}{
struct Cyc_List_List*_tmpC0=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=rgn0,({struct Cyc_List_List*_tmp141=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((fd->i).tvars,_tmpD5);_tmpCE->tl=_tmp141;});_tmpCE;});
Cyc_Tcutil_check_unique_tvars(loc,_tmpC0);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->tag=2U,_tmpCD->f1=rgn0;_tmpCD;});
{struct Cyc_List_List*_tmpC1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){
((struct Cyc_Absyn_Vardecl*)_tmpC1->hd)->rgn=param_rgn;}}
if((fd->i).cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpC2=*((struct Cyc_Absyn_VarargInfo*)_check_null((fd->i).cyc_varargs));struct Cyc_Absyn_VarargInfo _tmpC3=_tmpC2;struct _dyneither_ptr*_tmpC9;struct Cyc_Absyn_Tqual _tmpC8;void*_tmpC7;int _tmpC6;_LL7: _tmpC9=_tmpC3.name;_tmpC8=_tmpC3.tq;_tmpC7=_tmpC3.type;_tmpC6=_tmpC3.inject;_LL8:;{
# 565
struct Cyc_List_List*_tmpC4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpC4 != 0;_tmpC4=_tmpC4->tl){
struct Cyc_Absyn_Vardecl*_tmpC5=(struct Cyc_Absyn_Vardecl*)_tmpC4->hd;
if(Cyc_strptrcmp((*_tmpC5->name).f2,(struct _dyneither_ptr*)_check_null(_tmpC9))== 0){
({void*_tmp142=Cyc_Absyn_fatptr_type(_tmpC7,param_rgn,_tmpC8,Cyc_Absyn_false_type);_tmpC5->type=_tmp142;});
break;}}};}
# 573
return({struct Cyc_Tcenv_Fenv*_tmpCC=_cycalloc(sizeof(*_tmpCC));
({struct Cyc_Tcenv_SharedFenv*_tmp146=Cyc_Tcenv_new_shared_fenv((fd->i).ret_type);_tmpCC->shared=_tmp146;}),_tmpCC->type_vars=_tmpC0,_tmpCC->region_order=_tmpD6,_tmpCC->ctrl_env=0,({
# 578
void*_tmp145=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));({void*_tmp144=Cyc_Absyn_access_eff(param_rgn);_tmpCB->hd=_tmp144;}),({
struct Cyc_List_List*_tmp143=({struct Cyc_List_List*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->hd=(void*)_check_null((fd->i).effect),_tmpCA->tl=0;_tmpCA;});_tmpCB->tl=_tmp143;});_tmpCB;}));
# 578
_tmpCC->capability=_tmp145;}),_tmpCC->curr_rgn=param_rgn,
# 581
(_tmpCC->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpCC->flags).in_notreadctxt=0,(_tmpCC->flags).in_lhs=0,(_tmpCC->flags).abstract_ok=0,(_tmpCC->flags).in_stmt_exp=0;_tmpCC;});};};};}
# 585
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args){
# 587
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpDF=_cycalloc(sizeof(*_tmpDF));({struct _dyneither_ptr*_tmp149=({struct _dyneither_ptr*_tmpDE=_cycalloc(sizeof(*_tmpDE));({struct _dyneither_ptr _tmp148=({const char*_tmpDD="bogus";_tag_dyneither(_tmpDD,sizeof(char),6U);});*_tmpDE=_tmp148;});_tmpDE;});_tmpDF->name=_tmp149;}),({int _tmp147=Cyc_Tcutil_new_tvar_id();_tmpDF->identity=_tmp147;}),_tmpDF->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpDF;});
struct Cyc_List_List*_tmpD7=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=rgn0,_tmpDC->tl=0;_tmpDC;});
struct Cyc_RgnOrder_RgnPO*_tmpD8=({void*_tmp14A=Cyc_Absyn_join_eff(0);Cyc_RgnOrder_initial_fn_po(0,0,_tmp14A,rgn0,0U);});
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->tag=2U,_tmpDB->f1=rgn0;_tmpDB;});
return({struct Cyc_Tcenv_Fenv*_tmpDA=_cycalloc(sizeof(*_tmpDA));
({struct Cyc_Tcenv_SharedFenv*_tmp14D=Cyc_Tcenv_new_shared_fenv(ret_type);_tmpDA->shared=_tmp14D;}),_tmpDA->type_vars=_tmpD7,_tmpDA->region_order=_tmpD8,_tmpDA->ctrl_env=0,({
# 596
void*_tmp14C=Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));({void*_tmp14B=Cyc_Absyn_access_eff(param_rgn);_tmpD9->hd=_tmp14B;}),_tmpD9->tl=0;_tmpD9;}));_tmpDA->capability=_tmp14C;}),_tmpDA->curr_rgn=param_rgn,
# 598
(_tmpDA->flags).in_new=Cyc_Tcenv_NoneNew,(_tmpDA->flags).in_notreadctxt=0,(_tmpDA->flags).in_lhs=0,(_tmpDA->flags).abstract_ok=0,(_tmpDA->flags).in_stmt_exp=0;_tmpDA;});}
