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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 463
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 471
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 912
void*Cyc_Absyn_compress_kb(void*);
# 918
int Cyc_Absyn_type2bool(int def,void*);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
# 1162
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1168
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1176
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 61
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 78
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 86
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 115
void*Cyc_Tcutil_compress(void*t);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned int f2;};
# 31 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned int);
# 42
static int Cyc_Absyndump_expand_typedefs;
# 46
static int Cyc_Absyndump_qvar_to_Cids;
# 49
static int Cyc_Absyndump_add_cyc_prefix;
# 52
static int Cyc_Absyndump_generate_line_directives;
# 56
static int Cyc_Absyndump_to_VC;
# 58
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 73 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 76
void Cyc_Absyndump_dumptyp(void*);
void Cyc_Absyndump_dumpntyp(void*t);
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned int,int do_semi);
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 92
void Cyc_Absyndump_dumploc(unsigned int);
# 95
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
# 97
void Cyc_Absyndump_ignore(void*x){return;}
# 99
static unsigned int Cyc_Absyndump_pos=0U;
static char Cyc_Absyndump_prev_char='x';
# 102
int Cyc_Absyndump_need_space_before(){
char _tmp0=Cyc_Absyndump_prev_char;switch(_tmp0){case 123U: _LL1: _LL2:
 goto _LL4;case 125U: _LL3: _LL4:
 goto _LL6;case 40U: _LL5: _LL6:
 goto _LL8;case 41U: _LL7: _LL8:
 goto _LLA;case 91U: _LL9: _LLA:
 goto _LLC;case 93U: _LLB: _LLC:
 goto _LLE;case 59U: _LLD: _LLE:
 goto _LL10;case 44U: _LLF: _LL10:
 goto _LL12;case 61U: _LL11: _LL12:
 goto _LL14;case 63U: _LL13: _LL14:
 goto _LL16;case 33U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 10U: _LL19: _LL1A:
 goto _LL1C;case 42U: _LL1B: _LL1C:
 return 0;default: _LL1D: _LL1E:
 return 1;}_LL0:;}
# 122
void Cyc_Absyndump_dump(struct _dyneither_ptr s){
unsigned int sz=Cyc_strlen((struct _dyneither_ptr)s);
# 129
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= 1){
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - 1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 137
void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 140
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 145
void Cyc_Absyndump_dump_char(int c){
# 147
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpE[1U]="";
# 151
void Cyc_Absyndump_dumploc(unsigned int loc){
static struct _dyneither_ptr last_file={_tmpE,_tmpE,_tmpE + 1U};
static unsigned int last_line=0U;
if(loc == 0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 157
struct _tuple13 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple13 _tmp2=_tmp1;struct _dyneither_ptr _tmpD;unsigned int _tmpC;_LL1: _tmpD=_tmp2.f1;_tmpC=_tmp2.f2;_LL2:;
if(({char*_tmp303=(char*)_tmpD.curr;_tmp303 == (char*)(_tag_dyneither(0,0,0)).curr;}) || (char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line)return;
if((char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line + 1)
({void*_tmp3=0U;({struct Cyc___cycFILE*_tmp305=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp304=({const char*_tmp4="\n";_tag_dyneither(_tmp4,sizeof(char),2U);});Cyc_fprintf(_tmp305,_tmp304,_tag_dyneither(_tmp3,sizeof(void*),0U));});});else{
if((char*)_tmpD.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp7=({struct Cyc_Int_pa_PrintArg_struct _tmp2F6;_tmp2F6.tag=1U,_tmp2F6.f1=(unsigned long)((int)_tmpC);_tmp2F6;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct Cyc___cycFILE*_tmp307=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp306=({const char*_tmp6="\n# %d\n";_tag_dyneither(_tmp6,sizeof(char),7U);});Cyc_fprintf(_tmp307,_tmp306,_tag_dyneither(_tmp5,sizeof(void*),1U));});});else{
# 164
({struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp2F8;_tmp2F8.tag=1U,_tmp2F8.f1=(unsigned long)((int)_tmpC);_tmp2F8;});struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=0U,_tmp2F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD);_tmp2F7;});void*_tmp8[2U];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB;({struct Cyc___cycFILE*_tmp309=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp308=({const char*_tmp9="\n# %d %s\n";_tag_dyneither(_tmp9,sizeof(char),10U);});Cyc_fprintf(_tmp309,_tmp308,_tag_dyneither(_tmp8,sizeof(void*),2U));});});}}
# 166
last_file=_tmpD;
last_line=_tmpC;};}
# 172
void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
Cyc_Absyndump_dump(*s);}
# 176
void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 180
void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 196
f(env,l->hd);}
# 198
void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 200
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 204
void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 206
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 210
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 212
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 218
void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmpF=0;
struct _dyneither_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp10=(*v).f1;union Cyc_Absyn_Nmspace _tmp11=_tmp10;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp12;switch((_tmp11.C_n).tag){case 4U: _LL1: _LL2:
 _tmp12=0;goto _LL4;case 1U: _LL3: _tmp12=(_tmp11.Rel_n).val;_LL4:
# 224
 _tmpF=_tmp12;
goto _LL0;case 3U: _LL5: _tmp13=(_tmp11.C_n).val;_LL6:
# 230
 Cyc_Absyndump_dump_char((int)' ');
goto _LL0;default: _LL7: _tmp14=(_tmp11.Abs_n).val;_LL8:
# 233
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpF=_tmp14;
goto _LL0;}_LL0:;}
# 238
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp15="::";_tag_dyneither(_tmp15,sizeof(char),3U);}));}}
# 243
if(_tmpF != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));
for(_tmpF=_tmpF->tl;_tmpF != 0;_tmpF=_tmpF->tl){
# 248
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp16="::";_tag_dyneither(_tmp16,sizeof(char),3U);}));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));}
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(({const char*_tmp17="_";_tag_dyneither(_tmp17,sizeof(char),2U);}));else{
Cyc_Absyndump_dump_nospace(({const char*_tmp18="::";_tag_dyneither(_tmp18,sizeof(char),3U);}));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 256
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 259
Cyc_Absyndump_dump_str((*v).f2);}}}
# 263
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp19="restrict";_tag_dyneither(_tmp19,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp1A="volatile";_tag_dyneither(_tmp1A,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp1B="const";_tag_dyneither(_tmp1B,sizeof(char),6U);}));}
# 269
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1C=sc;switch(_tmp1C){case Cyc_Absyn_Static: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp1D="static";_tag_dyneither(_tmp1D,sizeof(char),7U);}));return;case Cyc_Absyn_Public: _LL3: _LL4:
 return;case Cyc_Absyn_Extern: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp1E="extern";_tag_dyneither(_tmp1E,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1F="extern \"C\"";_tag_dyneither(_tmp1F,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract: _LL9: _LLA:
 Cyc_Absyndump_dump(({const char*_tmp20="abstract";_tag_dyneither(_tmp20,sizeof(char),9U);}));return;default: _LLB: _LLC:
 Cyc_Absyndump_dump(({const char*_tmp21="register";_tag_dyneither(_tmp21,sizeof(char),9U);}));return;}_LL0:;}
# 280
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp22=ka;enum Cyc_Absyn_KindQual _tmp39;enum Cyc_Absyn_AliasQual _tmp38;_LL1: _tmp39=_tmp22->kind;_tmp38=_tmp22->aliasqual;_LL2:;{
enum Cyc_Absyn_KindQual _tmp23=_tmp39;switch(_tmp23){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 284
enum Cyc_Absyn_AliasQual _tmp24=_tmp38;switch(_tmp24){case Cyc_Absyn_Aliasable: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp25="A";_tag_dyneither(_tmp25,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL17: _LL18:
 Cyc_Absyndump_dump(({const char*_tmp26="UA";_tag_dyneither(_tmp26,sizeof(char),3U);}));return;default: _LL19: _LL1A:
 Cyc_Absyndump_dump(({const char*_tmp27="TA";_tag_dyneither(_tmp27,sizeof(char),3U);}));return;}_LL14:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 290
enum Cyc_Absyn_AliasQual _tmp28=_tmp38;switch(_tmp28){case Cyc_Absyn_Aliasable: _LL1C: _LL1D:
 Cyc_Absyndump_dump(({const char*_tmp29="M";_tag_dyneither(_tmp29,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL1E: _LL1F:
 Cyc_Absyndump_dump(({const char*_tmp2A="UM";_tag_dyneither(_tmp2A,sizeof(char),3U);}));return;default: _LL20: _LL21:
 Cyc_Absyndump_dump(({const char*_tmp2B="TM";_tag_dyneither(_tmp2B,sizeof(char),3U);}));return;}_LL1B:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 296
enum Cyc_Absyn_AliasQual _tmp2C=_tmp38;switch(_tmp2C){case Cyc_Absyn_Aliasable: _LL23: _LL24:
 Cyc_Absyndump_dump(({const char*_tmp2D="B";_tag_dyneither(_tmp2D,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL25: _LL26:
 Cyc_Absyndump_dump(({const char*_tmp2E="UB";_tag_dyneither(_tmp2E,sizeof(char),3U);}));return;default: _LL27: _LL28:
 Cyc_Absyndump_dump(({const char*_tmp2F="TB";_tag_dyneither(_tmp2F,sizeof(char),3U);}));return;}_LL22:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 302
enum Cyc_Absyn_AliasQual _tmp30=_tmp38;switch(_tmp30){case Cyc_Absyn_Aliasable: _LL2A: _LL2B:
 Cyc_Absyndump_dump(({const char*_tmp31="R";_tag_dyneither(_tmp31,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL2C: _LL2D:
 Cyc_Absyndump_dump(({const char*_tmp32="UR";_tag_dyneither(_tmp32,sizeof(char),3U);}));return;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump(({const char*_tmp33="TR";_tag_dyneither(_tmp33,sizeof(char),3U);}));return;}_LL29:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 307
 Cyc_Absyndump_dump(({const char*_tmp34="E";_tag_dyneither(_tmp34,sizeof(char),2U);}));return;case Cyc_Absyn_IntKind: _LLE: _LLF:
 Cyc_Absyndump_dump(({const char*_tmp35="I";_tag_dyneither(_tmp35,sizeof(char),2U);}));return;case Cyc_Absyn_BoolKind: _LL10: _LL11:
 Cyc_Absyndump_dump(({const char*_tmp36="BOOL";_tag_dyneither(_tmp36,sizeof(char),5U);}));return;default: _LL12: _LL13:
 Cyc_Absyndump_dump(({const char*_tmp37="PTRBND";_tag_dyneither(_tmp37,sizeof(char),7U);}));return;}_LL3:;};}
# 314
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp3A=k;if(_tmp3A == Cyc_Absyn_StructA){_LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp3B="struct ";_tag_dyneither(_tmp3B,sizeof(char),8U);}));return;}else{_LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp3C="union ";_tag_dyneither(_tmp3C,sizeof(char),7U);}));return;}_LL0:;}
# 321
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp30C=ts;struct _dyneither_ptr _tmp30B=({const char*_tmp3D="<";_tag_dyneither(_tmp3D,sizeof(char),2U);});struct _dyneither_ptr _tmp30A=({const char*_tmp3E=">";_tag_dyneither(_tmp3E,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp30C,_tmp30B,_tmp30A,({const char*_tmp3F=",";_tag_dyneither(_tmp3F,sizeof(char),2U);}));});}
# 324
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
Cyc_Absyndump_dump(({const char*_tmp40="`G";_tag_dyneither(_tmp40,sizeof(char),3U);}));
{void*_tmp41=Cyc_Absyn_compress_kb(tv->kind);void*_tmp42=_tmp41;struct Cyc_Absyn_Kind*_tmp45;struct Cyc_Absyn_Kind*_tmp44;switch(*((int*)_tmp42)){case 0U: _LL1: _tmp44=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp42)->f1;_LL2:
 _tmp45=_tmp44;goto _LL4;case 2U: _LL3: _tmp45=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp42)->f2;_LL4:
 Cyc_Absyndump_dumpkind(_tmp45);goto _LL0;default: _LL5: _LL6:
 Cyc_Absyndump_dump_nospace(({const char*_tmp43="K";_tag_dyneither(_tmp43,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 333
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 335
Cyc_Absyndump_dump(n);}}
# 337
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp46=Cyc_Absyn_compress_kb(tv->kind);void*_tmp47=_tmp46;struct Cyc_Absyn_Kind*_tmp4A;struct Cyc_Absyn_Kind*_tmp49;switch(*((int*)_tmp47)){case 1U: _LL1: _LL2:
 goto _LL0;case 2U: _LL3: _tmp49=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp47)->f2;_LL4:
 goto _LL0;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp47)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp47)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL5: _LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _tmp4A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp47)->f1;_LL8:
 Cyc_Absyndump_dump(({const char*_tmp48="::";_tag_dyneither(_tmp48,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(_tmp4A);goto _LL0;}}_LL0:;};}
# 346
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp30F=tvs;struct _dyneither_ptr _tmp30E=({const char*_tmp4B="<";_tag_dyneither(_tmp4B,sizeof(char),2U);});struct _dyneither_ptr _tmp30D=({const char*_tmp4C=">";_tag_dyneither(_tmp4C,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp30F,_tmp30E,_tmp30D,({const char*_tmp4D=",";_tag_dyneither(_tmp4D,sizeof(char),2U);}));});}
# 349
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp312=tvs;struct _dyneither_ptr _tmp311=({const char*_tmp4E="<";_tag_dyneither(_tmp4E,sizeof(char),2U);});struct _dyneither_ptr _tmp310=({const char*_tmp4F=">";_tag_dyneither(_tmp4F,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp312,_tmp311,_tmp310,({const char*_tmp50=",";_tag_dyneither(_tmp50,sizeof(char),2U);}));});}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 353
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 356
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp315=ts;struct _dyneither_ptr _tmp314=({const char*_tmp51="(";_tag_dyneither(_tmp51,sizeof(char),2U);});struct _dyneither_ptr _tmp313=({const char*_tmp52=")";_tag_dyneither(_tmp52,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp315,_tmp314,_tmp313,({const char*_tmp53=",";_tag_dyneither(_tmp53,sizeof(char),2U);}));});}
# 360
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp54=(void*)atts->hd;void*_tmp55=_tmp54;switch(*((int*)_tmp55)){case 1U: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp56="_stdcall";_tag_dyneither(_tmp56,sizeof(char),9U);}));return;case 2U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp57="_cdecl";_tag_dyneither(_tmp57,sizeof(char),7U);}));return;case 3U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp58="_fastcall";_tag_dyneither(_tmp58,sizeof(char),10U);}));return;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 370
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 372
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp59=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp5A=_tmp59;switch(*((int*)_tmp5A)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 380
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp5B="__declspec(";_tag_dyneither(_tmp5B,sizeof(char),12U);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp5C=(void*)atts->hd;void*_tmp5D=_tmp5C;switch(*((int*)_tmp5D)){case 1U: _LLA: _LLB:
 goto _LLD;case 2U: _LLC: _LLD:
 goto _LLF;case 3U: _LLE: _LLF:
 goto _LL9;default: _LL10: _LL11:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 390
Cyc_Absyndump_dump_char((int)')');}
# 393
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp5E=Cyc_Cyclone_c_compiler;if(_tmp5E == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 397
 Cyc_Absyndump_dump(({const char*_tmp5F=" __attribute__((";_tag_dyneither(_tmp5F,sizeof(char),17U);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp60=",";_tag_dyneither(_tmp60,sizeof(char),2U);}));}
# 402
Cyc_Absyndump_dump(({const char*_tmp61=")) ";_tag_dyneither(_tmp61,sizeof(char),4U);}));
return;}else{_LL3: _LL4:
# 405
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL0:;};}
# 410
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 414
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 418
{void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp63=_tmp62;struct Cyc_List_List*_tmp6B;void*_tmp6A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63)->f2 != 0){_LL1: _tmp6A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63)->f2)->hd;_LL2:
({struct Cyc_List_List*_tmp316=({struct Cyc_List_List*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->hd=_tmp6A,_tmp64->tl=rgions;_tmp64;});rgions=_tmp316;});goto _LL0;}else{goto _LL5;}case 9U: _LL3: _tmp6B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp63)->f2;_LL4:
# 421
 for(0;_tmp6B != 0;_tmp6B=_tmp6B->tl){
struct _tuple15 _tmp65=Cyc_Absyndump_effects_split((void*)_tmp6B->hd);struct _tuple15 _tmp66=_tmp65;struct Cyc_List_List*_tmp68;struct Cyc_List_List*_tmp67;_LL8: _tmp68=_tmp66.f1;_tmp67=_tmp66.f2;_LL9:;
({struct Cyc_List_List*_tmp317=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp68,rgions);rgions=_tmp317;});
({struct Cyc_List_List*_tmp318=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp67,effects);effects=_tmp318;});}
# 426
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp319=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=t,_tmp69->tl=effects;_tmp69;});effects=_tmp319;});goto _LL0;}_LL0:;}
# 429
return({struct _tuple15 _tmp2F9;_tmp2F9.f1=rgions,_tmp2F9.f2=effects;_tmp2F9;});}
# 432
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp6C=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp6D=_tmp6C;struct Cyc_List_List*_tmp70;struct Cyc_List_List*_tmp6F;_LL1: _tmp70=_tmp6D.f1;_tmp6F=_tmp6D.f2;_LL2:;
({struct Cyc_List_List*_tmp31A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp70);_tmp70=_tmp31A;});
({struct Cyc_List_List*_tmp31B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F);_tmp6F=_tmp31B;});
if(_tmp6F != 0){
{struct Cyc_List_List*_tmp6E=_tmp6F;for(0;_tmp6E != 0;_tmp6E=_tmp6E->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp6E->hd);
if(_tmp6E->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 442
if(_tmp70 != 0)Cyc_Absyndump_dump_char((int)'+');}
# 444
if(_tmp70 != 0  || _tmp6F == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp70 != 0;_tmp70=_tmp70->tl){
Cyc_Absyndump_dumprgn((void*)_tmp70->hd);
if(_tmp70->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 450
Cyc_Absyndump_dump_char((int)'}');}}
# 454
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 459
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp71=t;struct Cyc_Absyn_Datatypedecl*_tmpD6;struct Cyc_Absyn_Enumdecl*_tmpD5;struct Cyc_Absyn_Aggrdecl*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct _tuple0*_tmpD1;struct Cyc_List_List*_tmpD0;enum Cyc_Absyn_AggrKind _tmpCF;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCD;struct Cyc_Core_Opt*_tmpCC;void*_tmpCB;int _tmpCA;struct Cyc_Core_Opt*_tmpC9;int _tmpC8;struct Cyc_Absyn_Tvar*_tmpC7;struct Cyc_List_List*_tmpC6;void*_tmpC5;void*_tmpC4;void*_tmpC3;void*_tmpC2;struct _dyneither_ptr _tmpC1;struct Cyc_List_List*_tmpC0;struct _tuple0*_tmpBF;union Cyc_Absyn_AggrInfo _tmpBE;struct Cyc_List_List*_tmpBD;int _tmpBC;union Cyc_Absyn_DatatypeFieldInfo _tmpBB;struct Cyc_List_List*_tmpBA;union Cyc_Absyn_DatatypeInfo _tmpB9;struct Cyc_List_List*_tmpB8;switch(*((int*)_tmp71)){case 4U: _LL1: _LL2:
# 462
 goto _LL4;case 5U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)){case 0U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp72="void";_tag_dyneither(_tmp72,sizeof(char),5U);}));return;case 18U: _LLF: _tmpB9=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1;_tmpB8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2;_LL10:
# 473
{union Cyc_Absyn_DatatypeInfo _tmp78=_tmpB9;struct _tuple0*_tmp7E;int _tmp7D;struct _tuple0*_tmp7C;int _tmp7B;if((_tmp78.UnknownDatatype).tag == 1){_LL66: _tmp7C=((_tmp78.UnknownDatatype).val).name;_tmp7B=((_tmp78.UnknownDatatype).val).is_extensible;_LL67:
 _tmp7E=_tmp7C;_tmp7D=_tmp7B;goto _LL69;}else{_LL68: _tmp7E=(*(_tmp78.KnownDatatype).val)->name;_tmp7D=(*(_tmp78.KnownDatatype).val)->is_extensible;_LL69:
# 476
 if(_tmp7D)Cyc_Absyndump_dump(({const char*_tmp79="@extensible ";_tag_dyneither(_tmp79,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp7A="datatype ";_tag_dyneither(_tmp7A,sizeof(char),10U);}));
Cyc_Absyndump_dumpqvar(_tmp7E);
Cyc_Absyndump_dumptps(_tmpB8);
goto _LL65;}_LL65:;}
# 482
goto _LL0;case 19U: _LL11: _tmpBB=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1;_tmpBA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2;_LL12:
# 484
{union Cyc_Absyn_DatatypeFieldInfo _tmp7F=_tmpBB;struct _tuple0*_tmp88;int _tmp87;struct _tuple0*_tmp86;struct _tuple0*_tmp85;struct _tuple0*_tmp84;int _tmp83;if((_tmp7F.UnknownDatatypefield).tag == 1){_LL6B: _tmp85=((_tmp7F.UnknownDatatypefield).val).datatype_name;_tmp84=((_tmp7F.UnknownDatatypefield).val).field_name;_tmp83=((_tmp7F.UnknownDatatypefield).val).is_extensible;_LL6C:
# 486
 _tmp88=_tmp85;_tmp87=_tmp83;_tmp86=_tmp84;goto _LL6E;}else{_LL6D: _tmp88=(((_tmp7F.KnownDatatypefield).val).f1)->name;_tmp87=(((_tmp7F.KnownDatatypefield).val).f1)->is_extensible;_tmp86=(((_tmp7F.KnownDatatypefield).val).f2)->name;_LL6E:
# 489
 if(_tmp87)Cyc_Absyndump_dump(({const char*_tmp80="@extensible ";_tag_dyneither(_tmp80,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp81="datatype ";_tag_dyneither(_tmp81,sizeof(char),10U);}));
Cyc_Absyndump_dumpqvar(_tmp88);
Cyc_Absyndump_dump(({const char*_tmp82=".";_tag_dyneither(_tmp82,sizeof(char),2U);}));
Cyc_Absyndump_dumpqvar(_tmp86);
Cyc_Absyndump_dumptps(_tmpBA);
goto _LL6A;}_LL6A:;}
# 497
goto _LL0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f2){case Cyc_Absyn_Int_sz: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 500
 goto _LL1A;case Cyc_Absyn_Char_sz: _LL1B: _LL1C:
# 502
 Cyc_Absyndump_dump(({const char*_tmp8B="char";_tag_dyneither(_tmp8B,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz: _LL21: _LL22:
# 505
 goto _LL24;default: _LL2B: _LL2C:
# 511
 goto _LL2E;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f2){case Cyc_Absyn_Int_sz: _LL15: _LL16:
# 499
 Cyc_Absyndump_dump(({const char*_tmp89="int";_tag_dyneither(_tmp89,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 501
 Cyc_Absyndump_dump(({const char*_tmp8A="long";_tag_dyneither(_tmp8A,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz: _LL1D: _LL1E:
# 503
 Cyc_Absyndump_dump(({const char*_tmp8C="signed char";_tag_dyneither(_tmp8C,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL23: _LL24:
# 506
 Cyc_Absyndump_dump(({const char*_tmp8E="short";_tag_dyneither(_tmp8E,sizeof(char),6U);}));return;default: _LL2D: _LL2E: {
# 513
enum Cyc_Cyclone_C_Compilers _tmp92=Cyc_Cyclone_c_compiler;if(_tmp92 == Cyc_Cyclone_Gcc_c){_LL70: _LL71:
 Cyc_Absyndump_dump(({const char*_tmp93="long long";_tag_dyneither(_tmp93,sizeof(char),10U);}));return;}else{_LL72: _LL73:
 Cyc_Absyndump_dump(({const char*_tmp94="__int64";_tag_dyneither(_tmp94,sizeof(char),8U);}));return;}_LL6F:;}}default: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1F: _LL20:
# 504
 Cyc_Absyndump_dump(({const char*_tmp8D="unsigned char";_tag_dyneither(_tmp8D,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz: _LL25: _LL26:
# 508
 Cyc_Absyndump_dump(({const char*_tmp8F="unsigned short";_tag_dyneither(_tmp8F,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz: _LL27: _LL28:
 Cyc_Absyndump_dump(({const char*_tmp90="unsigned int";_tag_dyneither(_tmp90,sizeof(char),13U);}));return;case Cyc_Absyn_Long_sz: _LL29: _LL2A:
 Cyc_Absyndump_dump(({const char*_tmp91="unsigned long";_tag_dyneither(_tmp91,sizeof(char),14U);}));return;default: _LL2F: _LL30: {
# 518
enum Cyc_Cyclone_C_Compilers _tmp95=Cyc_Cyclone_c_compiler;if(_tmp95 == Cyc_Cyclone_Vc_c){_LL75: _LL76:
 Cyc_Absyndump_dump(({const char*_tmp96="unsigned __int64";_tag_dyneither(_tmp96,sizeof(char),17U);}));return;}else{_LL77: _LL78:
 Cyc_Absyndump_dump(({const char*_tmp97="unsigned long long";_tag_dyneither(_tmp97,sizeof(char),19U);}));return;}_LL74:;}}}case 2U: _LL31: _tmpBC=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1;_LL32:
# 523
 if(_tmpBC == 0)
Cyc_Absyndump_dump(({const char*_tmp98="float";_tag_dyneither(_tmp98,sizeof(char),6U);}));else{
if(_tmpBC == 1)
Cyc_Absyndump_dump(({const char*_tmp99="double";_tag_dyneither(_tmp99,sizeof(char),7U);}));else{
# 528
Cyc_Absyndump_dump(({const char*_tmp9A="long double";_tag_dyneither(_tmp9A,sizeof(char),12U);}));}}
return;case 20U: _LL35: _tmpBE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1;_tmpBD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2;_LL36: {
# 532
struct _tuple10 _tmp9B=Cyc_Absyn_aggr_kinded_name(_tmpBE);struct _tuple10 _tmp9C=_tmp9B;enum Cyc_Absyn_AggrKind _tmp9E;struct _tuple0*_tmp9D;_LL7A: _tmp9E=_tmp9C.f1;_tmp9D=_tmp9C.f2;_LL7B:;
Cyc_Absyndump_dumpaggr_kind(_tmp9E);Cyc_Absyndump_dumpqvar(_tmp9D);Cyc_Absyndump_dumptps(_tmpBD);
return;}case 15U: _LL39: _tmpBF=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1;_LL3A:
# 538
 Cyc_Absyndump_dump(({const char*_tmp9F="enum ";_tag_dyneither(_tmp9F,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(_tmpBF);return;case 16U: _LL3B: _tmpC0=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1;_LL3C:
# 540
 Cyc_Absyndump_dump(({const char*_tmpA0="enum {";_tag_dyneither(_tmpA0,sizeof(char),7U);}));Cyc_Absyndump_dumpenumfields(_tmpC0);Cyc_Absyndump_dump(({const char*_tmpA1="}";_tag_dyneither(_tmpA1,sizeof(char),2U);}));return;case 17U: _LL43: _tmpC1=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f1)->f1;_LL44:
# 544
 Cyc_Absyndump_dump(_tmpC1);return;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2 != 0){_LL45: _tmpC2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2)->hd;_LL46:
# 546
 Cyc_Absyndump_dump(({const char*_tmpA6="region_t<";_tag_dyneither(_tmpA6,sizeof(char),10U);}));Cyc_Absyndump_dumprgn(_tmpC2);Cyc_Absyndump_dump(({const char*_tmpA7=">";_tag_dyneither(_tmpA7,sizeof(char),2U);}));return;}else{goto _LL63;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2 != 0){_LL47: _tmpC3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2)->hd;_LL48:
# 548
 Cyc_Absyndump_dump(({const char*_tmpA8="tag_t<";_tag_dyneither(_tmpA8,sizeof(char),7U);}));Cyc_Absyndump_dumpntyp(_tmpC3);Cyc_Absyndump_dump(({const char*_tmpA9=">";_tag_dyneither(_tmpA9,sizeof(char),2U);}));return;}else{goto _LL63;}case 6U: _LL49: _LL4A:
 Cyc_Absyndump_dump(({const char*_tmpAA="`U";_tag_dyneither(_tmpAA,sizeof(char),3U);}));goto _LL0;case 7U: _LL4B: _LL4C:
 Cyc_Absyndump_dump(({const char*_tmpAB="`RC";_tag_dyneither(_tmpAB,sizeof(char),4U);}));goto _LL0;case 5U: _LL53: _LL54:
# 557
 Cyc_Absyndump_dump(({const char*_tmpAC="`H";_tag_dyneither(_tmpAC,sizeof(char),3U);}));goto _LL0;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2 != 0){_LL55: _tmpC4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2)->hd;_LL56:
# 559
 Cyc_Absyndump_dump(({const char*_tmpAD="{";_tag_dyneither(_tmpAD,sizeof(char),2U);}));Cyc_Absyndump_dumptyp(_tmpC4);Cyc_Absyndump_dump(({const char*_tmpAE="}";_tag_dyneither(_tmpAE,sizeof(char),2U);}));goto _LL0;}else{goto _LL63;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2 != 0){_LL57: _tmpC5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2)->hd;_LL58:
# 561
 Cyc_Absyndump_dump(({const char*_tmpAF="regions(";_tag_dyneither(_tmpAF,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmpC5);Cyc_Absyndump_dump(({const char*_tmpB0=")";_tag_dyneither(_tmpB0,sizeof(char),2U);}));goto _LL0;}else{_LL63: _LL64:
# 573
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp31C=({const char*_tmpB6="bad type constructor";_tag_dyneither(_tmpB6,sizeof(char),21U);});_tmpB7->f1=_tmp31C;});_tmpB7;}));}case 9U: _LL59: _tmpC6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp71)->f2;_LL5A:
# 563
 for(0;_tmpC6 != 0;_tmpC6=_tmpC6->tl){
Cyc_Absyndump_dumptyp((void*)_tmpC6->hd);
if(_tmpC6->tl != 0)Cyc_Absyndump_dump(({const char*_tmpB1="+";_tag_dyneither(_tmpB1,sizeof(char),2U);}));}
# 567
goto _LL0;case 11U: _LL5B: _LL5C:
 Cyc_Absyndump_dump(({const char*_tmpB2="@true";_tag_dyneither(_tmpB2,sizeof(char),6U);}));goto _LL0;case 12U: _LL5D: _LL5E:
 Cyc_Absyndump_dump(({const char*_tmpB3="@false";_tag_dyneither(_tmpB3,sizeof(char),7U);}));goto _LL0;case 13U: _LL5F: _LL60:
 Cyc_Absyndump_dump(({const char*_tmpB4="@thin";_tag_dyneither(_tmpB4,sizeof(char),6U);}));goto _LL0;default: _LL61: _LL62:
 Cyc_Absyndump_dump(({const char*_tmpB5="@fat";_tag_dyneither(_tmpB5,sizeof(char),5U);}));goto _LL0;}case 2U: _LL9: _tmpC7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp71)->f1;_LLA:
# 466
 Cyc_Absyndump_dumptvar(_tmpC7);return;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71)->f2 == 0){_LLB: _tmpC9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71)->f1;_tmpC8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71)->f3;_LLC:
# 468
 Cyc_Absyndump_dump(({const char*_tmp73="`E";_tag_dyneither(_tmp73,sizeof(char),3U);}));
if(_tmpC9 == 0)Cyc_Absyndump_dump(({const char*_tmp74="K";_tag_dyneither(_tmp74,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmpC9->v);}
Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp77=({struct Cyc_Int_pa_PrintArg_struct _tmp2FA;_tmp2FA.tag=1U,_tmp2FA.f1=(unsigned long)_tmpC8;_tmp2FA;});void*_tmp75[1U];_tmp75[0]=& _tmp77;({struct _dyneither_ptr _tmp31D=({const char*_tmp76="%d";_tag_dyneither(_tmp76,sizeof(char),3U);});Cyc_aprintf(_tmp31D,_tag_dyneither(_tmp75,sizeof(void*),1U));});}));return;}else{_LLD: _tmpCC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71)->f1;_tmpCB=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71)->f2;_tmpCA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71)->f3;_LLE:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmpCB));return;}case 6U: _LL33: _tmpCD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp71)->f1;_LL34:
# 530
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpCD);return;case 7U: _LL37: _tmpCF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp71)->f1;_tmpCE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp71)->f2;_LL38:
# 536
 Cyc_Absyndump_dumpaggr_kind(_tmpCF);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpCE);Cyc_Absyndump_dump_char((int)'}');return;case 8U: _LL3D: _tmpD1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp71)->f1;_tmpD0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp71)->f2;_LL3E:
# 541
 Cyc_Absyndump_dumpqvar(_tmpD1),Cyc_Absyndump_dumptps(_tmpD0);return;case 9U: _LL3F: _tmpD2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp71)->f1;_LL40:
 Cyc_Absyndump_dump(({const char*_tmpA2="valueof_t(";_tag_dyneither(_tmpA2,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(_tmpD2);Cyc_Absyndump_dump(({const char*_tmpA3=")";_tag_dyneither(_tmpA3,sizeof(char),2U);}));return;case 11U: _LL41: _tmpD3=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp71)->f1;_LL42:
 Cyc_Absyndump_dump(({const char*_tmpA4="typeof(";_tag_dyneither(_tmpA4,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmpD3);Cyc_Absyndump_dump(({const char*_tmpA5=")";_tag_dyneither(_tmpA5,sizeof(char),2U);}));return;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71)->f1)->r)){case 0U: _LL4D: _tmpD4=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71)->f1)->r)->f1;_LL4E:
# 552
 Cyc_Absyndump_dump_aggrdecl(_tmpD4);return;case 1U: _LL4F: _tmpD5=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71)->f1)->r)->f1;_LL50:
# 554
 Cyc_Absyndump_dump_enumdecl(_tmpD5);return;default: _LL51: _tmpD6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71)->f1)->r)->f1;_LL52:
# 556
 Cyc_Absyndump_dump_datatypedecl(_tmpD6);return;}}_LL0:;}
# 577
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 580
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 583
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
struct _tuple16*_tmpD7=cmp;void*_tmpD9;void*_tmpD8;_LL1: _tmpD9=_tmpD7->f1;_tmpD8=_tmpD7->f2;_LL2:;
Cyc_Absyndump_dumpeff(_tmpD9);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD8);}
# 587
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp31E=rgn_po;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp31E,({const char*_tmpDA=",";_tag_dyneither(_tmpDA,sizeof(char),2U);}));});}
# 590
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 594
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 599
if(c_varargs)
Cyc_Absyndump_dump(({const char*_tmpDB="...";_tag_dyneither(_tmpDB,sizeof(char),4U);}));else{
if(cyc_varargs != 0){
struct _tuple8*_tmpDC=({struct _tuple8*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->f1=cyc_varargs->name,_tmpDF->f2=cyc_varargs->tq,_tmpDF->f3=cyc_varargs->type;_tmpDF;});
Cyc_Absyndump_dump(({const char*_tmpDD="...";_tag_dyneither(_tmpDD,sizeof(char),4U);}));
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpDE=" inject ";_tag_dyneither(_tmpDE,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(_tmpDC);}}
# 607
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(effopt);}
# 611
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 615
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpE0=" @requires(";_tag_dyneither(_tmpE0,sizeof(char),12U);}));
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 621
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpE1=" @ensures(";_tag_dyneither(_tmpE1,sizeof(char),11U);}));
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 628
void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _tmp31F=Cyc_Absyn_empty_tqual(0U);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp31F,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 632
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpE2=d;struct _dyneither_ptr*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE2)->tag == 0U){_LL1: _tmpE4=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpE2)->f1;_LL2:
 Cyc_Absyndump_dump(({const char*_tmpE3=".[";_tag_dyneither(_tmpE3,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(_tmpE4);Cyc_Absyndump_dump_char((int)']');goto _LL0;}else{_LL3: _tmpE5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpE2)->f1;_LL4:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpE5);goto _LL0;}_LL0:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 639
void Cyc_Absyndump_dumpde(struct _tuple17*de){
({struct Cyc_List_List*_tmp322=(*de).f1;struct _dyneither_ptr _tmp321=({const char*_tmpE6="";_tag_dyneither(_tmpE6,sizeof(char),1U);});struct _dyneither_ptr _tmp320=({const char*_tmpE7="=";_tag_dyneither(_tmpE7,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp322,_tmp321,_tmp320,({const char*_tmpE8="=";_tag_dyneither(_tmpE8,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 644
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp326=inprec;struct Cyc_List_List*_tmp325=es;struct _dyneither_ptr _tmp324=({const char*_tmpE9="";_tag_dyneither(_tmpE9,sizeof(char),1U);});struct _dyneither_ptr _tmp323=({const char*_tmpEA="";_tag_dyneither(_tmpEA,sizeof(char),1U);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp326,_tmp325,_tmp324,_tmp323,({const char*_tmpEB=",";_tag_dyneither(_tmpEB,sizeof(char),2U);}));});}
# 648
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpEC="(";_tag_dyneither(_tmpEC,sizeof(char),2U);}));
{void*_tmpED=e->r;void*_tmpEE=_tmpED;struct Cyc_Absyn_Stmt*_tmp198;struct Cyc_Core_Opt*_tmp197;struct Cyc_List_List*_tmp196;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp194;int _tmp193;struct Cyc_Absyn_Exp*_tmp192;void**_tmp191;struct Cyc_Absyn_Exp*_tmp190;int _tmp18F;struct Cyc_Absyn_Enumfield*_tmp18E;struct Cyc_Absyn_Enumfield*_tmp18D;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Datatypefield*_tmp18B;struct Cyc_List_List*_tmp18A;struct _tuple0*_tmp189;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_Exp*_tmp186;void*_tmp185;struct Cyc_Absyn_Vardecl*_tmp184;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_List_List*_tmp181;struct _tuple8*_tmp180;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17B;struct _dyneither_ptr*_tmp17A;struct Cyc_Absyn_Exp*_tmp179;struct _dyneither_ptr*_tmp178;struct Cyc_Absyn_Exp*_tmp177;void*_tmp176;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Exp*_tmp174;struct _dyneither_ptr*_tmp173;int _tmp172;struct _dyneither_ptr _tmp171;void*_tmp170;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*_tmp16E;void*_tmp16D;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16A;void*_tmp169;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_List_List*_tmp163;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Core_Opt*_tmp154;struct Cyc_Absyn_Exp*_tmp153;enum Cyc_Absyn_Primop _tmp152;struct Cyc_List_List*_tmp151;struct _dyneither_ptr _tmp150;void*_tmp14F;struct _dyneither_ptr _tmp14E;struct _dyneither_ptr _tmp14D;struct _dyneither_ptr _tmp14C;enum Cyc_Absyn_Sign _tmp14B;long long _tmp14A;int _tmp149;int _tmp148;int _tmp147;enum Cyc_Absyn_Sign _tmp146;short _tmp145;struct _dyneither_ptr _tmp144;enum Cyc_Absyn_Sign _tmp143;char _tmp142;switch(*((int*)_tmpEE)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).String_c).tag){case 2U: _LL1: _tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Char_c).val).f1;_tmp142=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Char_c).val).f2;_LL2:
# 654
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp142));Cyc_Absyndump_dump_char((int)'\'');
goto _LL0;case 3U: _LL3: _tmp144=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Wchar_c).val;_LL4:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF1=({struct Cyc_String_pa_PrintArg_struct _tmp2FB;_tmp2FB.tag=0U,_tmp2FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp144);_tmp2FB;});void*_tmpEF[1U];_tmpEF[0]=& _tmpF1;({struct _dyneither_ptr _tmp327=({const char*_tmpF0="L'%s'";_tag_dyneither(_tmpF0,sizeof(char),6U);});Cyc_aprintf(_tmp327,_tag_dyneither(_tmpEF,sizeof(void*),1U));});}));goto _LL0;case 4U: _LL5: _tmp146=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Short_c).val).f1;_tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Short_c).val).f2;_LL6:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF4=({struct Cyc_Int_pa_PrintArg_struct _tmp2FC;_tmp2FC.tag=1U,_tmp2FC.f1=(unsigned long)((int)_tmp145);_tmp2FC;});void*_tmpF2[1U];_tmpF2[0]=& _tmpF4;({struct _dyneither_ptr _tmp328=({const char*_tmpF3="%d";_tag_dyneither(_tmpF3,sizeof(char),3U);});Cyc_aprintf(_tmp328,_tag_dyneither(_tmpF2,sizeof(void*),1U));});}));goto _LL0;case 5U: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL7: _tmp147=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Int_c).val).f2;_LL8:
 _tmp148=_tmp147;goto _LLA;case Cyc_Absyn_Signed: _LL9: _tmp148=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Int_c).val).f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF7=({struct Cyc_Int_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=1U,_tmp2FD.f1=(unsigned long)_tmp148;_tmp2FD;});void*_tmpF5[1U];_tmpF5[0]=& _tmpF7;({struct _dyneither_ptr _tmp329=({const char*_tmpF6="%d";_tag_dyneither(_tmpF6,sizeof(char),3U);});Cyc_aprintf(_tmp329,_tag_dyneither(_tmpF5,sizeof(void*),1U));});}));goto _LL0;default: _LLB: _tmp149=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Int_c).val).f2;_LLC:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFA=({struct Cyc_Int_pa_PrintArg_struct _tmp2FE;_tmp2FE.tag=1U,_tmp2FE.f1=(unsigned int)_tmp149;_tmp2FE;});void*_tmpF8[1U];_tmpF8[0]=& _tmpFA;({struct _dyneither_ptr _tmp32A=({const char*_tmpF9="%uU";_tag_dyneither(_tmpF9,sizeof(char),4U);});Cyc_aprintf(_tmp32A,_tag_dyneither(_tmpF8,sizeof(void*),1U));});}));goto _LL0;}case 6U: _LLD: _tmp14B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).LongLong_c).val).f1;_tmp14A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).LongLong_c).val).f2;_LLE:
# 663
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp14A));goto _LL0;case 7U: _LLF: _tmp14C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Float_c).val).f1;_LL10:
 Cyc_Absyndump_dump(_tmp14C);goto _LL0;case 1U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmpFB="NULL";_tag_dyneither(_tmpFB,sizeof(char),5U);}));goto _LL0;case 8U: _LL13: _tmp14D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).String_c).val;_LL14:
# 667
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp14D));Cyc_Absyndump_dump_char((int)'"');
goto _LL0;default: _LL15: _tmp14E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).Wstring_c).val;_LL16:
# 670
 Cyc_Absyndump_dump(({const char*_tmpFC="L\"";_tag_dyneither(_tmpFC,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(_tmp14E);Cyc_Absyndump_dump_char((int)'"');
goto _LL0;}case 1U: _LL17: _tmp14F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL18:
# 673
 Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(_tmp14F));goto _LL0;case 2U: _LL19: _tmp150=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL1A:
# 675
 Cyc_Absyndump_dump(({const char*_tmpFD="__cyclone_pragma__(";_tag_dyneither(_tmpFD,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(_tmp150);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 3U: _LL1B: _tmp152=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp151=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL1C: {
# 678
struct _dyneither_ptr _tmpFE=Cyc_Absynpp_prim2str(_tmp152);
{int _tmpFF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp151);int _tmp100=_tmpFF;switch(_tmp100){case 1U: _LL70: _LL71:
# 681
 if(_tmp152 == Cyc_Absyn_Numelts){
Cyc_Absyndump_dump(({const char*_tmp101="numelts(";_tag_dyneither(_tmp101,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp151))->hd);
Cyc_Absyndump_dump(({const char*_tmp102=")";_tag_dyneither(_tmp102,sizeof(char),2U);}));}else{
# 686
Cyc_Absyndump_dump(_tmpFE);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp151))->hd);}
# 689
goto _LL6F;case 2U: _LL72: _LL73:
# 691
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp151))->hd);
Cyc_Absyndump_dump(_tmpFE);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp151->tl))->hd);
goto _LL6F;default: _LL74: _LL75:
# 697
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp32B=({const char*_tmp103="Absyndump -- Bad number of arguments to primop";_tag_dyneither(_tmp103,sizeof(char),47U);});_tmp104->f1=_tmp32B;});_tmp104;}));}_LL6F:;}
# 699
goto _LL0;}case 4U: _LL1D: _tmp155=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp154=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_tmp153=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEE)->f3;_LL1E:
# 702
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp155);
if(_tmp154 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp154->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp105="=";_tag_dyneither(_tmp105,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp153);
goto _LL0;case 5U: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEE)->f2){case Cyc_Absyn_PreInc: _LL1F: _tmp156=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL20:
# 709
 Cyc_Absyndump_dump(({const char*_tmp106="++";_tag_dyneither(_tmp106,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp156);goto _LL0;case Cyc_Absyn_PreDec: _LL21: _tmp157=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL22:
 Cyc_Absyndump_dump(({const char*_tmp107="--";_tag_dyneither(_tmp107,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);goto _LL0;case Cyc_Absyn_PostInc: _LL23: _tmp158=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL24:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp158);Cyc_Absyndump_dump(({const char*_tmp108="++";_tag_dyneither(_tmp108,sizeof(char),3U);}));goto _LL0;default: _LL25: _tmp159=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL26:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp159);Cyc_Absyndump_dump(({const char*_tmp109="--";_tag_dyneither(_tmp109,sizeof(char),3U);}));goto _LL0;}case 6U: _LL27: _tmp15C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp15B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_tmp15A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEE)->f3;_LL28:
# 715
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15C);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp15B);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);
goto _LL0;case 7U: _LL29: _tmp15E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp15D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL2A:
# 722
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15E);Cyc_Absyndump_dump(({const char*_tmp10A=" && ";_tag_dyneither(_tmp10A,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);goto _LL0;case 8U: _LL2B: _tmp160=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp15F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL2C:
# 726
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp160);Cyc_Absyndump_dump(({const char*_tmp10B=" || ";_tag_dyneither(_tmp10B,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15F);goto _LL0;case 9U: _LL2D: _tmp162=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp161=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL2E:
# 732
 Cyc_Absyndump_dumploc(e->loc);
# 735
Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp162);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp161);
goto _LL0;case 10U: _LL2F: _tmp164=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp163=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL30:
# 739
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp164);
Cyc_Absyndump_dump_nospace(({const char*_tmp10C="(";_tag_dyneither(_tmp10C,sizeof(char),2U);}));{
# 745
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp164->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp163);
Cyc_Absyndump_dump_nospace(({const char*_tmp10D=")";_tag_dyneither(_tmp10D,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;};case 11U: _LL31: _tmp165=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL32:
# 754
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump(({const char*_tmp10E="throw";_tag_dyneither(_tmp10E,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp165);goto _LL0;case 12U: _LL33: _tmp166=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL34:
# 757
 _tmp167=_tmp166;goto _LL36;case 13U: _LL35: _tmp167=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL36:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp167);goto _LL0;case 14U: _LL37: _tmp169=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp168=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL38:
# 761
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp169);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp168);
goto _LL0;case 15U: _LL39: _tmp16A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL3A:
# 765
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp16A);goto _LL0;case 16U: _LL3B: _tmp16C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp16B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL3C:
# 768
 Cyc_Absyndump_dump(({const char*_tmp10F="new ";_tag_dyneither(_tmp10F,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp16B);goto _LL0;case 17U: _LL3D: _tmp16D=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL3E:
# 771
 Cyc_Absyndump_dump(({const char*_tmp110="sizeof(";_tag_dyneither(_tmp110,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(_tmp16D);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 18U: _LL3F: _tmp16E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL40:
# 774
 Cyc_Absyndump_dump(({const char*_tmp111="sizeof(";_tag_dyneither(_tmp111,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp16E);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 41U: _LL41: _tmp16F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL42:
# 777
 Cyc_Absyndump_dump(({const char*_tmp112="__extension__(";_tag_dyneither(_tmp112,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(_tmp16F);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 39U: _LL43: _tmp170=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL44:
# 780
 Cyc_Absyndump_dump(({const char*_tmp113="valueof(";_tag_dyneither(_tmp113,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp170);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 40U: _LL45: _tmp172=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp171=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL46:
# 783
 Cyc_Absyndump_dump(({const char*_tmp114="__asm__";_tag_dyneither(_tmp114,sizeof(char),8U);}));
if(_tmp172)Cyc_Absyndump_dump(({const char*_tmp115=" volatile ";_tag_dyneither(_tmp115,sizeof(char),11U);}));
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp171);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 38U: _LL47: _tmp174=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp173=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL48:
# 791
 Cyc_Absyndump_dump(({const char*_tmp116="tagcheck(";_tag_dyneither(_tmp116,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(_tmp174);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp173);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 19U: _LL49: _tmp176=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp175=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL4A:
# 796
 Cyc_Absyndump_dump(({const char*_tmp117="offsetof(";_tag_dyneither(_tmp117,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(_tmp176);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp175 != 0;_tmp175=_tmp175->tl){
{void*_tmp118=(void*)_tmp175->hd;void*_tmp119=_tmp118;unsigned int _tmp11E;struct _dyneither_ptr*_tmp11D;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp119)->tag == 0U){_LL77: _tmp11D=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp119)->f1;_LL78:
# 800
 Cyc_Absyndump_dump_nospace(*_tmp11D);
goto _LL76;}else{_LL79: _tmp11E=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp119)->f1;_LL7A:
# 803
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11C=({struct Cyc_Int_pa_PrintArg_struct _tmp2FF;_tmp2FF.tag=1U,_tmp2FF.f1=(unsigned long)((int)_tmp11E);_tmp2FF;});void*_tmp11A[1U];_tmp11A[0]=& _tmp11C;({struct _dyneither_ptr _tmp32C=({const char*_tmp11B="%d";_tag_dyneither(_tmp11B,sizeof(char),3U);});Cyc_aprintf(_tmp32C,_tag_dyneither(_tmp11A,sizeof(void*),1U));});}));
goto _LL76;}_LL76:;}
# 806
if(_tmp175->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 808
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 20U: _LL4B: _tmp177=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL4C:
# 812
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp177);goto _LL0;case 21U: _LL4D: _tmp179=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp178=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL4E:
# 815
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp179);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp178);
goto _LL0;case 22U: _LL4F: _tmp17B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp17A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL50:
# 819
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17B);Cyc_Absyndump_dump_nospace(({const char*_tmp11F="->";_tag_dyneither(_tmp11F,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*_tmp17A);
goto _LL0;case 23U: _LL51: _tmp17D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp17C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL52:
# 823
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17D);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp17C);Cyc_Absyndump_dump_char((int)']');goto _LL0;case 24U: _LL53: _tmp17E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL54:
# 827
 Cyc_Absyndump_dump(({const char*_tmp120="$(";_tag_dyneither(_tmp120,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp17E);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 25U: _LL55: _tmp180=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp17F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL56:
# 830
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp180).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp32F=_tmp17F;struct _dyneither_ptr _tmp32E=({const char*_tmp121="{";_tag_dyneither(_tmp121,sizeof(char),2U);});struct _dyneither_ptr _tmp32D=({const char*_tmp122="}";_tag_dyneither(_tmp122,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp32F,_tmp32E,_tmp32D,({const char*_tmp123=",";_tag_dyneither(_tmp123,sizeof(char),2U);}));});
goto _LL0;case 26U: _LL57: _tmp181=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL58:
# 837
({struct Cyc_List_List*_tmp332=_tmp181;struct _dyneither_ptr _tmp331=({const char*_tmp124="{";_tag_dyneither(_tmp124,sizeof(char),2U);});struct _dyneither_ptr _tmp330=({const char*_tmp125="}";_tag_dyneither(_tmp125,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp332,_tmp331,_tmp330,({const char*_tmp126=",";_tag_dyneither(_tmp126,sizeof(char),2U);}));});
goto _LL0;case 27U: _LL59: _tmp184=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp183=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_tmp182=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEE)->f3;_LL5A:
# 841
 Cyc_Absyndump_dump(({const char*_tmp127="{for";_tag_dyneither(_tmp127,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*_tmp184->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp183);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp182);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 28U: _LL5B: _tmp186=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp185=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL5C:
# 846
 Cyc_Absyndump_dump(({const char*_tmp128="{for x ";_tag_dyneither(_tmp128,sizeof(char),8U);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp186);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp185);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 29U: _LL5D: _tmp189=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp188=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_tmp187=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEE)->f3;_LL5E:
# 851
 Cyc_Absyndump_dumpqvar(_tmp189);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp188 != 0)
Cyc_Absyndump_dumptps(_tmp188);
({struct Cyc_List_List*_tmp335=_tmp187;struct _dyneither_ptr _tmp334=({const char*_tmp129="";_tag_dyneither(_tmp129,sizeof(char),1U);});struct _dyneither_ptr _tmp333=({const char*_tmp12A="}";_tag_dyneither(_tmp12A,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp335,_tmp334,_tmp333,({const char*_tmp12B=",";_tag_dyneither(_tmp12B,sizeof(char),2U);}));});
goto _LL0;case 30U: _LL5F: _tmp18A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL60:
# 859
({struct Cyc_List_List*_tmp338=_tmp18A;struct _dyneither_ptr _tmp337=({const char*_tmp12C="{";_tag_dyneither(_tmp12C,sizeof(char),2U);});struct _dyneither_ptr _tmp336=({const char*_tmp12D="}";_tag_dyneither(_tmp12D,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp338,_tmp337,_tmp336,({const char*_tmp12E=",";_tag_dyneither(_tmp12E,sizeof(char),2U);}));});
goto _LL0;case 31U: _LL61: _tmp18C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp18B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpEE)->f3;_LL62:
# 863
 Cyc_Absyndump_dumpqvar(_tmp18B->name);
if(_tmp18C != 0)({struct Cyc_List_List*_tmp33B=_tmp18C;struct _dyneither_ptr _tmp33A=({const char*_tmp12F="(";_tag_dyneither(_tmp12F,sizeof(char),2U);});struct _dyneither_ptr _tmp339=({const char*_tmp130=")";_tag_dyneither(_tmp130,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp33B,_tmp33A,_tmp339,({const char*_tmp131=",";_tag_dyneither(_tmp131,sizeof(char),2U);}));});
goto _LL0;case 32U: _LL63: _tmp18D=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL64:
# 867
 Cyc_Absyndump_dumpqvar(_tmp18D->name);goto _LL0;case 33U: _LL65: _tmp18E=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL66:
 Cyc_Absyndump_dumpqvar(_tmp18E->name);goto _LL0;case 34U: _LL67: _tmp193=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).is_calloc;_tmp192=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).rgn;_tmp191=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).elt_type;_tmp190=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).num_elts;_tmp18F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEE)->f1).inline_call;_LL68:
# 871
 Cyc_Absyndump_dumploc(_tmp190->loc);
if(_tmp193){
if(_tmp192 != 0){
Cyc_Absyndump_dump(({const char*_tmp132="rcalloc(";_tag_dyneither(_tmp132,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(_tmp192);Cyc_Absyndump_dump(({const char*_tmp133=",";_tag_dyneither(_tmp133,sizeof(char),2U);}));}else{
# 877
Cyc_Absyndump_dump(({const char*_tmp134="calloc";_tag_dyneither(_tmp134,sizeof(char),7U);}));}
# 879
Cyc_Absyndump_dumpexp(_tmp190);
Cyc_Absyndump_dump(({const char*_tmp135=",";_tag_dyneither(_tmp135,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(_tmp191)),0U));
Cyc_Absyndump_dump(({const char*_tmp136=")";_tag_dyneither(_tmp136,sizeof(char),2U);}));}else{
# 884
if(_tmp192 != 0){
if(_tmp18F)
Cyc_Absyndump_dump(({const char*_tmp137="rmalloc_inline(";_tag_dyneither(_tmp137,sizeof(char),16U);}));else{
# 888
Cyc_Absyndump_dump(({const char*_tmp138="rmalloc(";_tag_dyneither(_tmp138,sizeof(char),9U);}));}
Cyc_Absyndump_dumpexp(_tmp192);Cyc_Absyndump_dump(({const char*_tmp139=",";_tag_dyneither(_tmp139,sizeof(char),2U);}));}else{
# 891
Cyc_Absyndump_dump(({const char*_tmp13A="malloc(";_tag_dyneither(_tmp13A,sizeof(char),8U);}));}
# 894
if(_tmp191 != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp33C=Cyc_Absyn_sizeoftype_exp(*_tmp191,0U);Cyc_Absyn_times_exp(_tmp33C,_tmp190,0U);}));else{
# 897
Cyc_Absyndump_dumpexp(_tmp190);}
Cyc_Absyndump_dump(({const char*_tmp13B=")";_tag_dyneither(_tmp13B,sizeof(char),2U);}));}
# 900
goto _LL0;case 35U: _LL69: _tmp195=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp194=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL6A:
# 903
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp195);
Cyc_Absyndump_dump_nospace(({const char*_tmp13C=":=:";_tag_dyneither(_tmp13C,sizeof(char),4U);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp194);
goto _LL0;case 36U: _LL6B: _tmp197=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_tmp196=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpEE)->f2;_LL6C:
# 910
({struct Cyc_List_List*_tmp33F=_tmp196;struct _dyneither_ptr _tmp33E=({const char*_tmp13D="{";_tag_dyneither(_tmp13D,sizeof(char),2U);});struct _dyneither_ptr _tmp33D=({const char*_tmp13E="}";_tag_dyneither(_tmp13E,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp33F,_tmp33E,_tmp33D,({const char*_tmp13F=",";_tag_dyneither(_tmp13F,sizeof(char),2U);}));});
goto _LL0;default: _LL6D: _tmp198=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpEE)->f1;_LL6E:
# 914
 Cyc_Absyndump_dump_nospace(({const char*_tmp140="({";_tag_dyneither(_tmp140,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(_tmp198,1);
Cyc_Absyndump_dump_nospace(({const char*_tmp141="})";_tag_dyneither(_tmp141,sizeof(char),3U);}));
goto _LL0;}_LL0:;}
# 919
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 923
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 927
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp199=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp199->where_clause == 0  && (_tmp199->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp19A="default:";_tag_dyneither(_tmp19A,sizeof(char),9U);}));else{
# 933
Cyc_Absyndump_dump(({const char*_tmp19B="case";_tag_dyneither(_tmp19B,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(_tmp199->pattern);
if(_tmp199->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp19C="&&";_tag_dyneither(_tmp19C,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp199->where_clause));}
# 939
Cyc_Absyndump_dump_nospace(({const char*_tmp19D=":";_tag_dyneither(_tmp19D,sizeof(char),2U);}));}
# 941
Cyc_Absyndump_dumpstmt(_tmp199->body,0);}}
# 945
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp19E=s->r;void*_tmp19F=_tmp19E;struct Cyc_Absyn_Stmt*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D3;struct Cyc_Absyn_Stmt*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D1;struct _dyneither_ptr*_tmp1D0;struct Cyc_Absyn_Stmt*_tmp1CF;struct Cyc_Absyn_Decl*_tmp1CE;struct Cyc_Absyn_Stmt*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Stmt*_tmp1C7;struct _dyneither_ptr*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Absyn_Stmt*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Stmt*_tmp1C2;struct Cyc_Absyn_Stmt*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Stmt*_tmp1BF;struct Cyc_Absyn_Stmt*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BD;switch(*((int*)_tmp19F)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1U: _LL3: _tmp1BD=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL4:
 Cyc_Absyndump_dumpexp_prec(- 100,_tmp1BD);Cyc_Absyndump_dump_semi();goto _LL0;case 2U: _LL5: _tmp1BF=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1BE=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL6:
# 954
 if(Cyc_Absynpp_is_declaration(_tmp1BF)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BF,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 957
Cyc_Absyndump_dumpstmt(_tmp1BF,0);}
if(Cyc_Absynpp_is_declaration(_tmp1BE)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BE,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 964
Cyc_Absyndump_dumpstmt(_tmp1BE,expstmt);}
goto _LL0;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1A0="return;";_tag_dyneither(_tmp1A0,sizeof(char),8U);}));goto _LL0;}else{_LL9: _tmp1C0=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LLA:
 Cyc_Absyndump_dump(({const char*_tmp1A1="return";_tag_dyneither(_tmp1A1,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1C0));Cyc_Absyndump_dump_semi();goto _LL0;}case 4U: _LLB: _tmp1C3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1C2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_tmp1C1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3;_LLC:
# 969
 Cyc_Absyndump_dump(({const char*_tmp1A2="if(";_tag_dyneither(_tmp1A2,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(_tmp1C3);
{void*_tmp1A3=_tmp1C2->r;void*_tmp1A4=_tmp1A3;switch(*((int*)_tmp1A4)){case 2U: _LL26: _LL27:
 goto _LL29;case 12U: _LL28: _LL29:
 goto _LL2B;case 4U: _LL2A: _LL2B:
 goto _LL2D;case 13U: _LL2C: _LL2D:
 Cyc_Absyndump_dump_nospace(({const char*_tmp1A5="){";_tag_dyneither(_tmp1A5,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1C2,0);Cyc_Absyndump_dump_char((int)'}');goto _LL25;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1C2,0);}_LL25:;}
# 977
{void*_tmp1A6=_tmp1C1->r;void*_tmp1A7=_tmp1A6;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1A7)->tag == 0U){_LL31: _LL32:
 goto _LL30;}else{_LL33: _LL34:
 Cyc_Absyndump_dump(({const char*_tmp1A8="else{";_tag_dyneither(_tmp1A8,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(_tmp1C1,0);Cyc_Absyndump_dump_char((int)'}');goto _LL30;}_LL30:;}
# 981
goto _LL0;case 5U: _LLD: _tmp1C5=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1).f1;_tmp1C4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LLE:
# 983
 Cyc_Absyndump_dump(({const char*_tmp1A9="while(";_tag_dyneither(_tmp1A9,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(_tmp1C5);Cyc_Absyndump_dump_nospace(({const char*_tmp1AA="){";_tag_dyneither(_tmp1AA,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(_tmp1C4,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp1AB="break;";_tag_dyneither(_tmp1AB,sizeof(char),7U);}));goto _LL0;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp1AC="continue;";_tag_dyneither(_tmp1AC,sizeof(char),10U);}));goto _LL0;case 8U: _LL13: _tmp1C6=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL14:
 Cyc_Absyndump_dump(({const char*_tmp1AD="goto";_tag_dyneither(_tmp1AD,sizeof(char),5U);}));Cyc_Absyndump_dump_str(_tmp1C6);Cyc_Absyndump_dump_semi();goto _LL0;case 9U: _LL15: _tmp1CA=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1C9=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_tmp1C8=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f3).f1;_tmp1C7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19F)->f4;_LL16:
# 992
 Cyc_Absyndump_dump(({const char*_tmp1AE="for(";_tag_dyneither(_tmp1AE,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(_tmp1CA);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C9);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C8);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AF="){";_tag_dyneither(_tmp1AF,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1C7,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 10U: _LL17: _tmp1CC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1CB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL18:
# 996
 Cyc_Absyndump_dump(({const char*_tmp1B0="switch(";_tag_dyneither(_tmp1B0,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1CC);Cyc_Absyndump_dump_nospace(({const char*_tmp1B1="){";_tag_dyneither(_tmp1B1,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1CB);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 12U: _LL19: _tmp1CE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1CD=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1A:
# 1001
 Cyc_Absyndump_dumpdecl(_tmp1CE);Cyc_Absyndump_dumpstmt(_tmp1CD,expstmt);goto _LL0;case 13U: _LL1B: _tmp1D0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1CF=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL1C:
# 1007
 if(Cyc_Absynpp_is_declaration(_tmp1CF)){
Cyc_Absyndump_dump_str(_tmp1D0);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1B2=": ({";_tag_dyneither(_tmp1B2,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1B3=": {";_tag_dyneither(_tmp1B3,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(_tmp1CF,expstmt);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1B4="});}";_tag_dyneither(_tmp1B4,sizeof(char),5U);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 1013
Cyc_Absyndump_dump_str(_tmp1D0);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1CF,expstmt);}
# 1015
goto _LL0;case 14U: _LL1D: _tmp1D2=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1D1=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2).f1;_LL1E:
# 1018
 Cyc_Absyndump_dump(({const char*_tmp1B5="do{";_tag_dyneither(_tmp1B5,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1D2,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1B6="}while(";_tag_dyneither(_tmp1B6,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1D1);Cyc_Absyndump_dump_nospace(({const char*_tmp1B7=");";_tag_dyneither(_tmp1B7,sizeof(char),3U);}));
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1 == 0){_LL1F: _LL20:
# 1022
 Cyc_Absyndump_dump(({const char*_tmp1B8="fallthru;";_tag_dyneither(_tmp1B8,sizeof(char),10U);}));goto _LL0;}else{_LL21: _tmp1D3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_LL22:
# 1024
 Cyc_Absyndump_dump(({const char*_tmp1B9="fallthru(";_tag_dyneither(_tmp1B9,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp1D3);Cyc_Absyndump_dump_nospace(({const char*_tmp1BA=");";_tag_dyneither(_tmp1BA,sizeof(char),3U);}));goto _LL0;}default: _LL23: _tmp1D5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f1;_tmp1D4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19F)->f2;_LL24:
# 1027
 Cyc_Absyndump_dump(({const char*_tmp1BB="try";_tag_dyneither(_tmp1BB,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1D5,0);
Cyc_Absyndump_dump(({const char*_tmp1BC="catch{";_tag_dyneither(_tmp1BC,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1D4);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}_LL0:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1035
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
({struct Cyc_List_List*_tmp342=(*dp).f1;struct _dyneither_ptr _tmp341=({const char*_tmp1D6="";_tag_dyneither(_tmp1D6,sizeof(char),1U);});struct _dyneither_ptr _tmp340=({const char*_tmp1D7="=";_tag_dyneither(_tmp1D7,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp342,_tmp341,_tmp340,({const char*_tmp1D8="=";_tag_dyneither(_tmp1D8,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 1040
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1D9=p->r;void*_tmp1DA=_tmp1D9;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Enumfield*_tmp22A;struct Cyc_Absyn_Enumfield*_tmp229;struct Cyc_Absyn_Datatypefield*_tmp228;struct Cyc_List_List*_tmp227;int _tmp226;struct Cyc_List_List*_tmp225;struct Cyc_List_List*_tmp224;int _tmp223;union Cyc_Absyn_AggrInfo _tmp222;struct Cyc_List_List*_tmp221;struct Cyc_List_List*_tmp220;int _tmp21F;struct _tuple0*_tmp21E;struct Cyc_List_List*_tmp21D;int _tmp21C;struct _tuple0*_tmp21B;struct Cyc_Absyn_Tvar*_tmp21A;struct Cyc_Absyn_Vardecl*_tmp219;struct Cyc_Absyn_Vardecl*_tmp218;struct Cyc_Absyn_Pat*_tmp217;struct Cyc_Absyn_Vardecl*_tmp216;struct Cyc_Absyn_Pat*_tmp215;struct Cyc_List_List*_tmp214;int _tmp213;struct Cyc_Absyn_Tvar*_tmp212;struct Cyc_Absyn_Vardecl*_tmp211;struct Cyc_Absyn_Vardecl*_tmp210;struct Cyc_Absyn_Pat*_tmp20F;struct Cyc_Absyn_Vardecl*_tmp20E;struct _dyneither_ptr _tmp20D;char _tmp20C;int _tmp20B;int _tmp20A;int _tmp209;switch(*((int*)_tmp1DA)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_char((int)'_');goto _LL0;case 9U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp1DB="NULL";_tag_dyneither(_tmp1DB,sizeof(char),5U);}));goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1){case Cyc_Absyn_None: _LL5: _tmp209=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL6:
 _tmp20A=_tmp209;goto _LL8;case Cyc_Absyn_Signed: _LL7: _tmp20A=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL8:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1DE=({struct Cyc_Int_pa_PrintArg_struct _tmp300;_tmp300.tag=1U,_tmp300.f1=(unsigned long)_tmp20A;_tmp300;});void*_tmp1DC[1U];_tmp1DC[0]=& _tmp1DE;({struct _dyneither_ptr _tmp343=({const char*_tmp1DD="%d";_tag_dyneither(_tmp1DD,sizeof(char),3U);});Cyc_aprintf(_tmp343,_tag_dyneither(_tmp1DC,sizeof(void*),1U));});}));goto _LL0;default: _LL9: _tmp20B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1E1=({struct Cyc_Int_pa_PrintArg_struct _tmp301;_tmp301.tag=1U,_tmp301.f1=(unsigned int)_tmp20B;_tmp301;});void*_tmp1DF[1U];_tmp1DF[0]=& _tmp1E1;({struct _dyneither_ptr _tmp344=({const char*_tmp1E0="%u";_tag_dyneither(_tmp1E0,sizeof(char),3U);});Cyc_aprintf(_tmp344,_tag_dyneither(_tmp1DF,sizeof(void*),1U));});}));goto _LL0;}case 11U: _LLB: _tmp20C=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LLC:
# 1048
 Cyc_Absyndump_dump(({const char*_tmp1E2="'";_tag_dyneither(_tmp1E2,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp20C));Cyc_Absyndump_dump_nospace(({const char*_tmp1E3="'";_tag_dyneither(_tmp1E3,sizeof(char),2U);}));goto _LL0;case 12U: _LLD: _tmp20D=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LLE:
 Cyc_Absyndump_dump(_tmp20D);goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2)->r)->tag == 0U){_LLF: _tmp20E=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL10:
 Cyc_Absyndump_dumpqvar(_tmp20E->name);goto _LL0;}else{_LL11: _tmp210=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp20F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL12:
 Cyc_Absyndump_dumpqvar(_tmp210->name);Cyc_Absyndump_dump(({const char*_tmp1E4=" as ";_tag_dyneither(_tmp1E4,sizeof(char),5U);}));Cyc_Absyndump_dumppat(_tmp20F);goto _LL0;}case 2U: _LL13: _tmp212=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp211=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL14:
# 1053
 Cyc_Absyndump_dump(({const char*_tmp1E5="alias";_tag_dyneither(_tmp1E5,sizeof(char),6U);}));
Cyc_Absyndump_dump(({const char*_tmp1E6="<";_tag_dyneither(_tmp1E6,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp212);Cyc_Absyndump_dump(({const char*_tmp1E7=">";_tag_dyneither(_tmp1E7,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(_tmp211,p->loc,0);
goto _LL0;case 5U: _LL15: _tmp214=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp213=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL16: {
# 1058
struct _dyneither_ptr term=_tmp213?({const char*_tmp1EA=", ...)";_tag_dyneither(_tmp1EA,sizeof(char),7U);}):({const char*_tmp1EB=")";_tag_dyneither(_tmp1EB,sizeof(char),2U);});
({struct Cyc_List_List*_tmp347=_tmp214;struct _dyneither_ptr _tmp346=({const char*_tmp1E8="$(";_tag_dyneither(_tmp1E8,sizeof(char),3U);});struct _dyneither_ptr _tmp345=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp347,_tmp346,_tmp345,({const char*_tmp1E9=",";_tag_dyneither(_tmp1E9,sizeof(char),2U);}));});goto _LL0;}case 6U: _LL17: _tmp215=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL18:
 Cyc_Absyndump_dump(({const char*_tmp1EC="&";_tag_dyneither(_tmp1EC,sizeof(char),2U);}));Cyc_Absyndump_dumppat(_tmp215);goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2)->r)->tag == 0U){_LL19: _tmp216=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL1A:
# 1062
 Cyc_Absyndump_dump(({const char*_tmp1ED="*";_tag_dyneither(_tmp1ED,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(_tmp216->name);goto _LL0;}else{_LL1B: _tmp218=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp217=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1C:
# 1064
 Cyc_Absyndump_dump(({const char*_tmp1EE="*";_tag_dyneither(_tmp1EE,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(_tmp218->name);Cyc_Absyndump_dump(({const char*_tmp1EF=" as ";_tag_dyneither(_tmp1EF,sizeof(char),5U);}));Cyc_Absyndump_dumppat(_tmp217);goto _LL0;}case 4U: _LL1D: _tmp21A=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp219=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1E:
# 1066
 Cyc_Absyndump_dumpqvar(_tmp219->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp21A);Cyc_Absyndump_dump_char((int)'>');goto _LL0;case 15U: _LL1F: _tmp21B=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL20:
 Cyc_Absyndump_dumpqvar(_tmp21B);goto _LL0;case 16U: _LL21: _tmp21E=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp21D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp21C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_LL22: {
# 1069
struct _dyneither_ptr term=_tmp21C?({const char*_tmp1F2=", ...)";_tag_dyneither(_tmp1F2,sizeof(char),7U);}):({const char*_tmp1F3=")";_tag_dyneither(_tmp1F3,sizeof(char),2U);});
Cyc_Absyndump_dumpqvar(_tmp21E);({struct Cyc_List_List*_tmp34A=_tmp21D;struct _dyneither_ptr _tmp349=({const char*_tmp1F0="(";_tag_dyneither(_tmp1F0,sizeof(char),2U);});struct _dyneither_ptr _tmp348=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp34A,_tmp349,_tmp348,({const char*_tmp1F1=",";_tag_dyneither(_tmp1F1,sizeof(char),2U);}));});goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1 != 0){_LL23: _tmp222=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp221=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp220=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp21F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL24: {
# 1072
struct _tuple10 _tmp1F4=Cyc_Absyn_aggr_kinded_name(_tmp222);struct _tuple10 _tmp1F5=_tmp1F4;struct _tuple0*_tmp1FD;_LL30: _tmp1FD=_tmp1F5.f2;_LL31:;{
struct _dyneither_ptr term=_tmp21F?({const char*_tmp1FB=", ...)";_tag_dyneither(_tmp1FB,sizeof(char),7U);}):({const char*_tmp1FC=")";_tag_dyneither(_tmp1FC,sizeof(char),2U);});
Cyc_Absyndump_dumpqvar(_tmp1FD);Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp34D=_tmp221;struct _dyneither_ptr _tmp34C=({const char*_tmp1F6="[";_tag_dyneither(_tmp1F6,sizeof(char),2U);});struct _dyneither_ptr _tmp34B=({const char*_tmp1F7="]";_tag_dyneither(_tmp1F7,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp34D,_tmp34C,_tmp34B,({const char*_tmp1F8=",";_tag_dyneither(_tmp1F8,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp350=_tmp220;struct _dyneither_ptr _tmp34F=({const char*_tmp1F9="";_tag_dyneither(_tmp1F9,sizeof(char),1U);});struct _dyneither_ptr _tmp34E=term;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp350,_tmp34F,_tmp34E,({const char*_tmp1FA=",";_tag_dyneither(_tmp1FA,sizeof(char),2U);}));});
goto _LL0;};}}else{_LL25: _tmp225=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp224=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp223=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL26: {
# 1079
struct _dyneither_ptr term=_tmp223?({const char*_tmp203=", ...)";_tag_dyneither(_tmp203,sizeof(char),7U);}):({const char*_tmp204=")";_tag_dyneither(_tmp204,sizeof(char),2U);});
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp353=_tmp225;struct _dyneither_ptr _tmp352=({const char*_tmp1FE="[";_tag_dyneither(_tmp1FE,sizeof(char),2U);});struct _dyneither_ptr _tmp351=({const char*_tmp1FF="]";_tag_dyneither(_tmp1FF,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp353,_tmp352,_tmp351,({const char*_tmp200=",";_tag_dyneither(_tmp200,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp356=_tmp224;struct _dyneither_ptr _tmp355=({const char*_tmp201="";_tag_dyneither(_tmp201,sizeof(char),1U);});struct _dyneither_ptr _tmp354=term;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp356,_tmp355,_tmp354,({const char*_tmp202=",";_tag_dyneither(_tmp202,sizeof(char),2U);}));});
goto _LL0;}}case 8U: _LL27: _tmp228=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp227=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp226=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL28: {
# 1085
struct _dyneither_ptr term=_tmp226?({const char*_tmp207=", ...)";_tag_dyneither(_tmp207,sizeof(char),7U);}):({const char*_tmp208=")";_tag_dyneither(_tmp208,sizeof(char),2U);});
Cyc_Absyndump_dumpqvar(_tmp228->name);
if(_tmp227 != 0)({struct Cyc_List_List*_tmp359=_tmp227;struct _dyneither_ptr _tmp358=({const char*_tmp205="(";_tag_dyneither(_tmp205,sizeof(char),2U);});struct _dyneither_ptr _tmp357=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp359,_tmp358,_tmp357,({const char*_tmp206=",";_tag_dyneither(_tmp206,sizeof(char),2U);}));});
goto _LL0;}case 13U: _LL29: _tmp229=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL2A:
 _tmp22A=_tmp229;goto _LL2C;case 14U: _LL2B: _tmp22A=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL2C:
 Cyc_Absyndump_dumpqvar(_tmp22A->name);goto _LL0;default: _LL2D: _tmp22B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL2E:
 Cyc_Absyndump_dumpexp(_tmp22B);goto _LL0;}_LL0:;}
# 1095
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1100
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp35A=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp35A,({const char*_tmp22C=",";_tag_dyneither(_tmp22C,sizeof(char),2U);}));});}
# 1103
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp22D=" = ";_tag_dyneither(_tmp22D,sizeof(char),4U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1110
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp35B=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp35B,({const char*_tmp22E=",";_tag_dyneither(_tmp22E,sizeof(char),2U);}));});}
# 1114
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp22F=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp230=_tmp22F;struct _dyneither_ptr*_tmp238;struct Cyc_Absyn_Tqual _tmp237;void*_tmp236;struct Cyc_Absyn_Exp*_tmp235;struct Cyc_List_List*_tmp234;struct Cyc_Absyn_Exp*_tmp233;_LL1: _tmp238=_tmp230->name;_tmp237=_tmp230->tq;_tmp236=_tmp230->type;_tmp235=_tmp230->width;_tmp234=_tmp230->attributes;_tmp233=_tmp230->requires_clause;_LL2:;
# 1118
{enum Cyc_Cyclone_C_Compilers _tmp231=Cyc_Cyclone_c_compiler;if(_tmp231 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1120
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp237,_tmp236,Cyc_Absyndump_dump_str,_tmp238);
Cyc_Absyndump_dumpatts(_tmp234);
goto _LL3;}else{_LL6: _LL7:
# 1124
 Cyc_Absyndump_dumpatts(_tmp234);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp237,_tmp236,Cyc_Absyndump_dump_str,_tmp238);
goto _LL3;}_LL3:;}
# 1128
if((unsigned int)_tmp233){
Cyc_Absyndump_dump(({const char*_tmp232="@requires ";_tag_dyneither(_tmp232,sizeof(char),11U);}));
Cyc_Absyndump_dumpexp(_tmp233);}
# 1132
if(_tmp235 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp235);}
# 1136
Cyc_Absyndump_dump_semi();}}
# 1140
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1145
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1149
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp239=(*pr).f1;void*_tmp23A=_tmp239;switch(*((int*)_tmp23A)){case 11U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp23B="_stdcall";_tag_dyneither(_tmp23B,sizeof(char),9U);}));goto _LL0;case 2U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp23C="_cdecl";_tag_dyneither(_tmp23C,sizeof(char),7U);}));goto _LL0;case 3U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp23D="_fastcall";_tag_dyneither(_tmp23D,sizeof(char),10U);}));goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 1157
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1159
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1164
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1171
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1173
void*_tmp23E=t;void*_tmp243;void*_tmp242;switch(*((int*)_tmp23E)){case 1U: _LL1: _tmp242=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23E)->f2;if(_tmp242 != 0){_LL2:
 return Cyc_Absyndump_is_char_ptr(_tmp242);}else{goto _LL5;}case 3U: _LL3: _tmp243=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E)->f1).elt_type;_LL4:
# 1176
 L: {
void*_tmp23F=_tmp243;void*_tmp241;void*_tmp240;switch(*((int*)_tmp23F)){case 1U: _LL8: _tmp240=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23F)->f2;if(_tmp240 != 0){_LL9:
 _tmp243=_tmp240;goto L;}else{goto _LLE;}case 8U: _LLA: _tmp241=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23F)->f4;if(_tmp241 != 0){_LLB:
 _tmp243=_tmp241;goto L;}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23F)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp23F)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 1183
 return 0;}_LL0:;}
# 1187
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp244=vd;enum Cyc_Absyn_Scope _tmp25A;struct _tuple0*_tmp259;unsigned int _tmp258;struct Cyc_Absyn_Tqual _tmp257;void*_tmp256;struct Cyc_Absyn_Exp*_tmp255;struct Cyc_List_List*_tmp254;_LL1: _tmp25A=_tmp244->sc;_tmp259=_tmp244->name;_tmp258=_tmp244->varloc;_tmp257=_tmp244->tq;_tmp256=_tmp244->type;_tmp255=_tmp244->initializer;_tmp254=_tmp244->attributes;_LL2:;
{enum Cyc_Cyclone_C_Compilers _tmp245=Cyc_Cyclone_c_compiler;if(_tmp245 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1192
 if(_tmp25A == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp246=Cyc_Tcutil_compress(_tmp256);void*_tmp247=_tmp246;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp247)->tag == 5U){_LL9: _LLA:
 goto _LL8;}else{_LLB: _LLC:
 Cyc_Absyndump_dumpscope(_tmp25A);}_LL8:;}else{
# 1198
Cyc_Absyndump_dumpscope(_tmp25A);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp257,_tmp256,Cyc_Absyndump_dumpqvar,_tmp259);
Cyc_Absyndump_dumpatts(_tmp254);
goto _LL3;}else{_LL6: _LL7:
# 1203
 Cyc_Absyndump_dumpatts(_tmp254);
Cyc_Absyndump_dumpscope(_tmp25A);{
struct _RegionHandle _tmp248=_new_region("temp");struct _RegionHandle*temp=& _tmp248;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp256);
struct _tuple11 _tmp249=Cyc_Absynpp_to_tms(temp,_tmp257,_tmp256);struct _tuple11 _tmp24A=_tmp249;struct Cyc_Absyn_Tqual _tmp253;void*_tmp252;struct Cyc_List_List*_tmp251;_LLE: _tmp253=_tmp24A.f1;_tmp252=_tmp24A.f2;_tmp251=_tmp24A.f3;_LLF:;{
# 1209
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp251;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp24B=(void*)tms2->hd;void*_tmp24C=_tmp24B;struct Cyc_List_List*_tmp24F;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp24C)->tag == 5U){_LL11: _tmp24F=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp24C)->f2;_LL12:
# 1213
 for(0;_tmp24F != 0;_tmp24F=_tmp24F->tl){
void*_tmp24D=(void*)_tmp24F->hd;void*_tmp24E=_tmp24D;switch(*((int*)_tmp24E)){case 1U: _LL16: _LL17:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL15;case 2U: _LL18: _LL19:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL15;case 3U: _LL1A: _LL1B:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL15;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 1220
goto _LL10;}else{_LL13: _LL14:
 goto _LL10;}_LL10:;}}
# 1223
Cyc_Absyndump_dumptq(_tmp253);
Cyc_Absyndump_dumpntyp(_tmp252);{
struct _tuple19 _tmp250=({struct _tuple19 _tmp302;_tmp302.f1=call_conv,_tmp302.f2=_tmp259;_tmp302;});
({int _tmp35C=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(_tmp35C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp251),Cyc_Absyndump_dump_callconv_qvar,& _tmp250);});};};}
# 1228
_npop_handler(0U);goto _LL3;
# 1205
;_pop_region(temp);};}_LL3:;}
# 1231
if(_tmp255 != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp255);}
# 1235
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1238
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp25B="@tagged ";_tag_dyneither(_tmp25B,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1247
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_List_List*_tmp35F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _dyneither_ptr _tmp35E=({const char*_tmp25C="<";_tag_dyneither(_tmp25C,sizeof(char),2U);});struct _dyneither_ptr _tmp35D=({const char*_tmp25D=">";_tag_dyneither(_tmp25D,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp35F,_tmp35E,_tmp35D,({const char*_tmp25E=",";_tag_dyneither(_tmp25E,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1254
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump(({const char*_tmp25F="}";_tag_dyneither(_tmp25F,sizeof(char),2U);}));
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1259
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp260=ed;enum Cyc_Absyn_Scope _tmp265;struct _tuple0*_tmp264;struct Cyc_Core_Opt*_tmp263;_LL1: _tmp265=_tmp260->sc;_tmp264=_tmp260->name;_tmp263=_tmp260->fields;_LL2:;
Cyc_Absyndump_dumpscope(_tmp265);
Cyc_Absyndump_dump(({const char*_tmp261="enum ";_tag_dyneither(_tmp261,sizeof(char),6U);}));
Cyc_Absyndump_dumpqvar(_tmp264);
if(_tmp263 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp263->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp262="}";_tag_dyneither(_tmp262,sizeof(char),2U);}));}}
# 1270
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp266=dd;enum Cyc_Absyn_Scope _tmp26E;struct _tuple0*_tmp26D;struct Cyc_List_List*_tmp26C;struct Cyc_Core_Opt*_tmp26B;int _tmp26A;_LL1: _tmp26E=_tmp266->sc;_tmp26D=_tmp266->name;_tmp26C=_tmp266->tvs;_tmp26B=_tmp266->fields;_tmp26A=_tmp266->is_extensible;_LL2:;
Cyc_Absyndump_dumpscope(_tmp26E);
if(_tmp26A)Cyc_Absyndump_dump(({const char*_tmp267="@extensible ";_tag_dyneither(_tmp267,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp268="datatype ";_tag_dyneither(_tmp268,sizeof(char),10U);}));
Cyc_Absyndump_dumpqvar(_tmp26D);
Cyc_Absyndump_dumptvars(_tmp26C);
if(_tmp26B != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp26B->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp269="}";_tag_dyneither(_tmp269,sizeof(char),2U);}));}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1284
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp26F=d->r;void*_tmp270=_tmp26F;struct Cyc_List_List*_tmp29D;struct Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29B;struct _tuple0*_tmp29A;struct Cyc_List_List*_tmp299;struct _dyneither_ptr*_tmp298;struct Cyc_List_List*_tmp297;struct Cyc_Absyn_Typedefdecl*_tmp296;struct Cyc_Absyn_Tvar*_tmp295;struct Cyc_Absyn_Vardecl*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_List_List*_tmp292;struct Cyc_Absyn_Pat*_tmp291;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Enumdecl*_tmp28F;struct Cyc_Absyn_Datatypedecl*_tmp28E;struct Cyc_Absyn_Aggrdecl*_tmp28D;struct Cyc_Absyn_Fndecl*_tmp28C;struct Cyc_Absyn_Vardecl*_tmp28B;switch(*((int*)_tmp270)){case 0U: _LL1: _tmp28B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LL2:
# 1288
 Cyc_Absyndump_dumpvardecl(_tmp28B,d->loc,1);
goto _LL0;case 1U: _LL3: _tmp28C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LL4:
# 1292
{enum Cyc_Cyclone_C_Compilers _tmp271=Cyc_Cyclone_c_compiler;if(_tmp271 == Cyc_Cyclone_Vc_c){_LL20: _LL21:
 Cyc_Absyndump_dumpatts(_tmp28C->attributes);goto _LL1F;}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 1296
if(_tmp28C->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp272=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp275;if(_tmp272 == Cyc_Cyclone_Vc_c){_LL25: _LL26:
 Cyc_Absyndump_dump(({const char*_tmp273="__inline";_tag_dyneither(_tmp273,sizeof(char),9U);}));goto _LL24;}else{_LL27: _tmp275=_tmp272;_LL28:
 Cyc_Absyndump_dump(({const char*_tmp274="inline";_tag_dyneither(_tmp274,sizeof(char),7U);}));goto _LL24;}_LL24:;}
# 1301
Cyc_Absyndump_dumpscope(_tmp28C->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->tag=5U,(_tmp277->f1).tvars=_tmp28C->tvs,(_tmp277->f1).effect=_tmp28C->effect,(_tmp277->f1).ret_tqual=_tmp28C->ret_tqual,(_tmp277->f1).ret_type=_tmp28C->ret_type,({
# 1304
struct Cyc_List_List*_tmp360=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp28C->args);(_tmp277->f1).args=_tmp360;}),(_tmp277->f1).c_varargs=_tmp28C->c_varargs,(_tmp277->f1).cyc_varargs=_tmp28C->cyc_varargs,(_tmp277->f1).rgn_po=_tmp28C->rgn_po,(_tmp277->f1).attributes=0,(_tmp277->f1).requires_clause=_tmp28C->requires_clause,(_tmp277->f1).requires_relns=_tmp28C->requires_relns,(_tmp277->f1).ensures_clause=_tmp28C->ensures_clause,(_tmp277->f1).ensures_relns=_tmp28C->ensures_relns;_tmp277;});
# 1309
{enum Cyc_Cyclone_C_Compilers _tmp276=Cyc_Cyclone_c_compiler;if(_tmp276 == Cyc_Cyclone_Gcc_c){_LL2A: _LL2B:
# 1311
({struct Cyc_Absyn_Tqual _tmp362=Cyc_Absyn_empty_tqual(0U);void*_tmp361=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp362,_tmp361,Cyc_Absyndump_dump_atts_qvar,_tmp28C);});
goto _LL29;}else{_LL2C: _LL2D:
# 1314
({struct Cyc_Absyn_Tqual _tmp364=Cyc_Absyn_empty_tqual(0U);void*_tmp363=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp364,_tmp363,Cyc_Absyndump_dump_callconv_fdqvar,_tmp28C);});
goto _LL29;}_LL29:;}
# 1317
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp28C->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;};case 5U: _LL5: _tmp28D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LL6:
# 1322
 Cyc_Absyndump_dump_aggrdecl(_tmp28D);Cyc_Absyndump_dump_semi();
goto _LL0;case 6U: _LL7: _tmp28E=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LL8:
# 1325
 Cyc_Absyndump_dump_datatypedecl(_tmp28E);Cyc_Absyndump_dump_semi();
goto _LL0;case 7U: _LL9: _tmp28F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LLA:
# 1328
 Cyc_Absyndump_dump_enumdecl(_tmp28F);Cyc_Absyndump_dump_semi();
goto _LL0;case 2U: _LLB: _tmp291=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_tmp290=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp270)->f3;_LLC:
# 1331
 Cyc_Absyndump_dump(({const char*_tmp278="let";_tag_dyneither(_tmp278,sizeof(char),4U);}));
Cyc_Absyndump_dumppat(_tmp291);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp290);
Cyc_Absyndump_dump_semi();
goto _LL0;case 3U: _LLD: _tmp292=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LLE:
# 1338
 Cyc_Absyndump_dump(({const char*_tmp279="let ";_tag_dyneither(_tmp279,sizeof(char),5U);}));
Cyc_Absyndump_dumpids(_tmp292);
Cyc_Absyndump_dump_semi();
goto _LL0;case 4U: _LLF: _tmp295=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_tmp294=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp270)->f2;_tmp293=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp270)->f3;_LL10:
# 1343
 Cyc_Absyndump_dump(({const char*_tmp27A="region";_tag_dyneither(_tmp27A,sizeof(char),7U);}));
Cyc_Absyndump_dump(({const char*_tmp27B="<";_tag_dyneither(_tmp27B,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp295);Cyc_Absyndump_dump(({const char*_tmp27C="> ";_tag_dyneither(_tmp27C,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(_tmp294->name);
if((unsigned int)_tmp293){
Cyc_Absyndump_dump(({const char*_tmp27D=" = open(";_tag_dyneither(_tmp27D,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(_tmp293);
Cyc_Absyndump_dump(({const char*_tmp27E=")";_tag_dyneither(_tmp27E,sizeof(char),2U);}));}
# 1350
Cyc_Absyndump_dump_semi();
goto _LL0;case 8U: _LL11: _tmp296=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LL12:
# 1357
 Cyc_Absyndump_dump(({const char*_tmp27F="typedef";_tag_dyneither(_tmp27F,sizeof(char),8U);}));{
void*t;
if(_tmp296->defn == 0)
({void*_tmp365=Cyc_Absyn_new_evar(_tmp296->kind,0);t=_tmp365;});else{
# 1362
t=(void*)_check_null(_tmp296->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp296->tq,t,Cyc_Absyndump_dumptypedefname,_tmp296);
Cyc_Absyndump_dumpatts(_tmp296->atts);
Cyc_Absyndump_dump_semi();
# 1367
goto _LL0;};case 9U: _LL13: _tmp298=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_tmp297=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp270)->f2;_LL14:
# 1369
 Cyc_Absyndump_dump(({const char*_tmp280="namespace ";_tag_dyneither(_tmp280,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(_tmp298);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp297 != 0;_tmp297=_tmp297->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp297->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 10U: _LL15: _tmp29A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_tmp299=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp270)->f2;_LL16:
# 1377
 Cyc_Absyndump_dump(({const char*_tmp281="using ";_tag_dyneither(_tmp281,sizeof(char),7U);}));
Cyc_Absyndump_dumpqvar(_tmp29A);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp299 != 0;_tmp299=_tmp299->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp299->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 11U: _LL17: _tmp29B=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_LL18:
# 1385
 Cyc_Absyndump_dump(({const char*_tmp282="extern \"C\" {";_tag_dyneither(_tmp282,sizeof(char),13U);}));
for(0;_tmp29B != 0;_tmp29B=_tmp29B->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp29B->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 12U: _LL19: _tmp29D=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp270)->f1;_tmp29C=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp270)->f2;_LL1A:
# 1391
 Cyc_Absyndump_dump(({const char*_tmp283="extern \"C include\" {";_tag_dyneither(_tmp283,sizeof(char),21U);}));
for(0;_tmp29D != 0;_tmp29D=_tmp29D->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp29D->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp29C != 0){
Cyc_Absyndump_dump(({const char*_tmp284=" export {";_tag_dyneither(_tmp284,sizeof(char),10U);}));
for(0;_tmp29C != 0;_tmp29C=_tmp29C->tl){
struct _tuple20 _tmp285=*((struct _tuple20*)_tmp29C->hd);struct _tuple20 _tmp286=_tmp285;struct _tuple0*_tmp287;_LL2F: _tmp287=_tmp286.f2;_LL30:;
Cyc_Absyndump_dumpqvar(_tmp287);
if(_tmp29C->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 1402
Cyc_Absyndump_dump(({const char*_tmp288="}";_tag_dyneither(_tmp288,sizeof(char),2U);}));}
# 1404
goto _LL0;case 13U: _LL1B: _LL1C:
# 1406
 Cyc_Absyndump_dump(({const char*_tmp289=" __cyclone_port_on__; ";_tag_dyneither(_tmp289,sizeof(char),23U);}));
goto _LL0;default: _LL1D: _LL1E:
# 1409
 Cyc_Absyndump_dump(({const char*_tmp28A=" __cyclone_port_off__; ";_tag_dyneither(_tmp28A,sizeof(char),24U);}));
goto _LL0;}_LL0:;};}
# 1414
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1421
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
if(tms == 0){
f(a);
return;}{
# 1426
void*_tmp29E=(void*)tms->hd;void*_tmp29F=_tmp29E;void*_tmp2CF;void*_tmp2CE;void*_tmp2CD;void*_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CB;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29F)->tag == 2U){_LL1: _tmp2CF=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29F)->f1).rgn;_tmp2CE=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29F)->f1).nullable;_tmp2CD=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29F)->f1).bounds;_tmp2CC=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29F)->f1).zero_term;_tmp2CB=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29F)->f2;_LL2:
# 1430
{void*_tmp2A0=Cyc_Tcutil_compress(_tmp2CD);void*_tmp2A1=_tmp2A0;struct Cyc_Absyn_Exp*_tmp2A2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1)->f1)){case 14U: _LL6: _LL7:
 Cyc_Absyndump_dump_char((int)'?');goto _LL5;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1)->f2)->hd)->tag == 9U){_LL8: _tmp2A2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1)->f2)->hd)->f1;_LL9:
# 1433
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,_tmp2CE)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp2A2);
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA: _LLB:
# 1437
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,_tmp2CE)?'*':'@'));}_LL5:;}
# 1439
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && Cyc_Absyn_type2bool(0,_tmp2CC))Cyc_Absyndump_dump(({const char*_tmp2A3="@zeroterm";_tag_dyneither(_tmp2A3,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !Cyc_Absyn_type2bool(0,_tmp2CC))Cyc_Absyndump_dump(({const char*_tmp2A4="@nozeroterm";_tag_dyneither(_tmp2A4,sizeof(char),12U);}));
{void*_tmp2A5=Cyc_Tcutil_compress(_tmp2CF);void*_tmp2A6=_tmp2A5;struct Cyc_Absyn_Tvar*_tmp2AE;switch(*((int*)_tmp2A6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A6)->f1)){case 5U: _LLD: _LLE:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp2A7="`H";_tag_dyneither(_tmp2A7,sizeof(char),3U);}));goto _LLC;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp2A8="`U";_tag_dyneither(_tmp2A8,sizeof(char),3U);}));goto _LLC;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp2A9="`RC";_tag_dyneither(_tmp2A9,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2U: _LL13: _tmp2AE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2A6)->f1;_LL14:
 Cyc_Absyndump_dumptvar(_tmp2AE);goto _LLC;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f2 == 0){_LL15: _LL16:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2CF));goto _LLC;}else{goto _LL19;}case 8U: _LL17: _LL18:
 Cyc_Absyndump_dump(({const char*_tmp2AA="@region(";_tag_dyneither(_tmp2AA,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp2CF);Cyc_Absyndump_dump(({const char*_tmp2AB=")";_tag_dyneither(_tmp2AB,sizeof(char),2U);}));goto _LLC;default: _LL19: _LL1A:
({void*_tmp2AC=0U;({struct _dyneither_ptr _tmp366=({const char*_tmp2AD="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp2AD,sizeof(char),37U);});Cyc_Tcutil_impos(_tmp366,_tag_dyneither(_tmp2AC,sizeof(void*),0U));});});}_LLC:;}
# 1450
Cyc_Absyndump_dumptq(_tmp2CB);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL3: _LL4: {
# 1455
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2AF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp2B0=_tmp2AF;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2B0)->tag == 2U){_LL1C: _LL1D:
 next_is_pointer=1;goto _LL1B;}else{_LL1E: _LL1F:
 goto _LL1B;}_LL1B:;}
# 1461
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2B1=(void*)tms->hd;void*_tmp2B2=_tmp2B1;struct Cyc_List_List*_tmp2CA;struct Cyc_List_List*_tmp2C9;unsigned int _tmp2C8;int _tmp2C7;struct Cyc_List_List*_tmp2C6;unsigned int _tmp2C5;struct Cyc_List_List*_tmp2C4;int _tmp2C3;struct Cyc_Absyn_VarargInfo*_tmp2C2;void*_tmp2C1;struct Cyc_List_List*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;void*_tmp2BC;void*_tmp2BB;switch(*((int*)_tmp2B2)){case 0U: _LL21: _tmp2BB=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1;_LL22:
# 1468
 Cyc_Absyndump_dump(({const char*_tmp2B3="[]";_tag_dyneither(_tmp2B3,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,_tmp2BB))Cyc_Absyndump_dump(({const char*_tmp2B4="@zeroterm";_tag_dyneither(_tmp2B4,sizeof(char),10U);}));
goto _LL20;case 1U: _LL23: _tmp2BD=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1;_tmp2BC=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f2;_LL24:
# 1472
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2BD);Cyc_Absyndump_dump_char((int)']');
if(Cyc_Absyn_type2bool(0,_tmp2BC))Cyc_Absyndump_dump(({const char*_tmp2B5="@zeroterm";_tag_dyneither(_tmp2B5,sizeof(char),10U);}));
goto _LL20;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->tag == 1U){_LL25: _tmp2C4=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f1;_tmp2C3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f2;_tmp2C2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f3;_tmp2C1=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f4;_tmp2C0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f5;_tmp2BF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f6;_tmp2BE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f7;_LL26:
# 1476
 Cyc_Absyndump_dumpfunargs(_tmp2C4,_tmp2C3,_tmp2C2,_tmp2C1,_tmp2C0,_tmp2BF,_tmp2BE);goto _LL20;}else{_LL27: _tmp2C6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f1;_tmp2C5=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1)->f2;_LL28:
# 1478
({struct Cyc_List_List*_tmp369=_tmp2C6;struct _dyneither_ptr _tmp368=({const char*_tmp2B6="(";_tag_dyneither(_tmp2B6,sizeof(char),2U);});struct _dyneither_ptr _tmp367=({const char*_tmp2B7=")";_tag_dyneither(_tmp2B7,sizeof(char),2U);});((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp369,_tmp368,_tmp367,({const char*_tmp2B8=",";_tag_dyneither(_tmp2B8,sizeof(char),2U);}));});goto _LL20;}case 4U: _LL29: _tmp2C9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f1;_tmp2C8=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f2;_tmp2C7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f3;_LL2A:
# 1480
 if(_tmp2C7)Cyc_Absyndump_dumpkindedtvars(_tmp2C9);else{Cyc_Absyndump_dumptvars(_tmp2C9);}goto _LL20;case 5U: _LL2B: _tmp2CA=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2B2)->f2;_LL2C:
 Cyc_Absyndump_dumpatts(_tmp2CA);goto _LL20;default: _LL2D: _LL2E:
({void*_tmp2B9=0U;({struct _dyneither_ptr _tmp36A=({const char*_tmp2BA="dumptms";_tag_dyneither(_tmp2BA,sizeof(char),8U);});Cyc_Tcutil_impos(_tmp36A,_tag_dyneither(_tmp2B9,sizeof(void*),0U));});});}_LL20:;}
# 1484
return;}}_LL0:;};}
# 1488
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2D0=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2D1=_new_region("temp");struct _RegionHandle*temp=& _tmp2D1;_push_region(temp);
{struct _tuple11 _tmp2D2=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2D3=_tmp2D2;struct Cyc_Absyn_Tqual _tmp2D6;void*_tmp2D5;struct Cyc_List_List*_tmp2D4;_LL1: _tmp2D6=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;_tmp2D4=_tmp2D3.f3;_LL2:;
Cyc_Absyndump_dumptq(_tmp2D6);
Cyc_Absyndump_dumpntyp(_tmp2D5);
({int _tmp36D=_tmp2D0;struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2D4);void(*_tmp36B)(void*)=f;Cyc_Absyndump_dumptms(_tmp36D,_tmp36C,_tmp36B,a);});}
# 1491
;_pop_region(temp);}
# 1498
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1500
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2D7=0U;({struct Cyc___cycFILE*_tmp36F=f;struct _dyneither_ptr _tmp36E=({const char*_tmp2D8="\n";_tag_dyneither(_tmp2D8,sizeof(char),2U);});Cyc_fprintf(_tmp36F,_tmp36E,_tag_dyneither(_tmp2D7,sizeof(void*),0U));});});}
# 1506
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2D9=d->r;void*_tmp2DA=_tmp2D9;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F3;struct Cyc_List_List*_tmp2F2;struct _dyneither_ptr*_tmp2F1;struct Cyc_List_List*_tmp2F0;struct Cyc_Absyn_Typedefdecl*_tmp2EF;struct Cyc_Absyn_Enumdecl*_tmp2EE;struct Cyc_Absyn_Datatypedecl*_tmp2ED;struct Cyc_Absyn_Aggrdecl*_tmp2EC;struct Cyc_Absyn_Fndecl*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EA;switch(*((int*)_tmp2DA)){case 0U: _LL1: _tmp2EA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LL2:
# 1509
 if(_tmp2EA->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2EA->initializer;
_tmp2EA->initializer=0;
if(_tmp2EA->sc == Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2DB="extern ";_tag_dyneither(_tmp2DB,sizeof(char),8U);}));
Cyc_Absyndump_dumpvardecl(_tmp2EA,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2DC="\n";_tag_dyneither(_tmp2DC,sizeof(char),2U);}));
_tmp2EA->initializer=init;
goto _LL0;};case 1U: _LL3: _tmp2EB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LL4:
# 1519
 if(_tmp2EB->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2EB->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));_tmp2DE->tag=5U,(_tmp2DE->f1).tvars=_tmp2EB->tvs,(_tmp2DE->f1).effect=_tmp2EB->effect,(_tmp2DE->f1).ret_tqual=_tmp2EB->ret_tqual,(_tmp2DE->f1).ret_type=_tmp2EB->ret_type,({
# 1523
struct Cyc_List_List*_tmp370=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2EB->args);(_tmp2DE->f1).args=_tmp370;}),(_tmp2DE->f1).c_varargs=_tmp2EB->c_varargs,(_tmp2DE->f1).cyc_varargs=_tmp2EB->cyc_varargs,(_tmp2DE->f1).rgn_po=_tmp2EB->rgn_po,(_tmp2DE->f1).attributes=0,(_tmp2DE->f1).requires_clause=_tmp2EB->requires_clause,(_tmp2DE->f1).requires_relns=_tmp2EB->requires_relns,(_tmp2DE->f1).ensures_clause=_tmp2EB->ensures_clause,(_tmp2DE->f1).ensures_relns=_tmp2EB->ensures_relns;_tmp2DE;});
# 1528
({struct Cyc_Absyn_Tqual _tmp372=Cyc_Absyn_empty_tqual(0U);void*_tmp371=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp372,_tmp371,Cyc_Absyndump_dump_atts_qvar,_tmp2EB);});
Cyc_Absyndump_dump(({const char*_tmp2DD=";\n";_tag_dyneither(_tmp2DD,sizeof(char),3U);}));
goto _LL0;};case 5U: _LL5: _tmp2EC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LL6:
# 1532
 if(_tmp2EC->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2EC->impl;
if(_tmp2EC->sc == Cyc_Absyn_Abstract)_tmp2EC->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2EC);
_tmp2EC->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2DF=";\n";_tag_dyneither(_tmp2DF,sizeof(char),3U);}));
goto _LL0;};case 6U: _LL7: _tmp2ED=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LL8:
# 1540
 Cyc_Absyndump_dump_datatypedecl(_tmp2ED);
Cyc_Absyndump_dump(({const char*_tmp2E0=";\n";_tag_dyneither(_tmp2E0,sizeof(char),3U);}));
goto _LL0;case 7U: _LL9: _tmp2EE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LLA:
# 1544
 Cyc_Absyndump_dump_enumdecl(_tmp2EE);
Cyc_Absyndump_dump(({const char*_tmp2E1=";\n";_tag_dyneither(_tmp2E1,sizeof(char),3U);}));
goto _LL0;case 8U: _LLB: _tmp2EF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LLC:
# 1548
 if(_tmp2EF->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2E2="\n";_tag_dyneither(_tmp2E2,sizeof(char),2U);}));}
# 1552
goto _LL0;case 9U: _LLD: _tmp2F1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_tmp2F0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2DA)->f2;_LLE:
# 1554
 Cyc_Absyndump_dump(({const char*_tmp2E3="namespace ";_tag_dyneither(_tmp2E3,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(_tmp2F1);
Cyc_Absyndump_dump(({const char*_tmp2E4="{\n";_tag_dyneither(_tmp2E4,sizeof(char),3U);}));
for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F0->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E5="}\n";_tag_dyneither(_tmp2E5,sizeof(char),3U);}));
goto _LL0;case 10U: _LLF: _tmp2F2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2DA)->f2;_LL10:
# 1562
 for(0;_tmp2F2 != 0;_tmp2F2=_tmp2F2->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F2->hd);}
goto _LL0;case 11U: _LL11: _tmp2F3=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_LL12:
# 1566
 Cyc_Absyndump_dump(({const char*_tmp2E6="extern \"C\" {";_tag_dyneither(_tmp2E6,sizeof(char),13U);}));
for(0;_tmp2F3 != 0;_tmp2F3=_tmp2F3->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F3->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E7="}\n";_tag_dyneither(_tmp2E7,sizeof(char),3U);}));
goto _LL0;case 12U: _LL13: _tmp2F5=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2DA)->f1;_tmp2F4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2DA)->f2;_LL14:
# 1587 "absyndump.cyc"
 goto _LL0;default: _LL15: _LL16:
({void*_tmp2E8=0U;({struct _dyneither_ptr _tmp373=({const char*_tmp2E9="bad top-level declaration";_tag_dyneither(_tmp2E9,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp373,_tag_dyneither(_tmp2E8,sizeof(void*),0U));});});}_LL0:;}
# 1592
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
