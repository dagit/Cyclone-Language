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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 462
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 470
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 705 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 902
void*Cyc_Absyn_compress_kb(void*);
# 908
int Cyc_Absyn_type2bool(int def,void*);
# 913
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1165
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1173
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
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 112 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned int f2;};
# 28 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned int);
# 39
static int Cyc_Absyndump_expand_typedefs;
# 43
static int Cyc_Absyndump_qvar_to_Cids;
# 46
static int Cyc_Absyndump_add_cyc_prefix;
# 49
static int Cyc_Absyndump_generate_line_directives;
# 53
static int Cyc_Absyndump_to_VC;
# 55
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 70 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 73
static void Cyc_Absyndump_dumptyp(void*);
static void Cyc_Absyndump_dumpntyp(void*t);
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned int,int do_semi);
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*,void(*f)(void*),void*a);
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 92
static void Cyc_Absyndump_dumploc(unsigned int);
# 95
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
# 97
static void Cyc_Absyndump_ignore(void*x){;}
# 99
static unsigned int Cyc_Absyndump_pos=0U;
static char Cyc_Absyndump_prev_char='x';
# 102
static int Cyc_Absyndump_need_space_before(){
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
static void Cyc_Absyndump_dump(struct _dyneither_ptr s){
unsigned int sz=Cyc_strlen((struct _dyneither_ptr)s);
# 129
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= (unsigned int)1){
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - (unsigned int)1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 137
static void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 140
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 145
static void Cyc_Absyndump_dump_char(int c){
# 147
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpE[1U]="";
# 151
static void Cyc_Absyndump_dumploc(unsigned int loc){
static struct _dyneither_ptr last_file={_tmpE,_tmpE,_tmpE + 1U};
static unsigned int last_line=0U;
if(loc == (unsigned int)0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 157
struct _tuple13 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple13 _tmp2=_tmp1;struct _dyneither_ptr _tmpD;unsigned int _tmpC;_LL1: _tmpD=_tmp2.f1;_tmpC=_tmp2.f2;_LL2:;
if(({char*_tmp2F9=(char*)_tmpD.curr;_tmp2F9 == (char*)(_tag_dyneither(0,0,0)).curr;}) || (char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line)return;
if((char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line + (unsigned int)1)
({void*_tmp3=0U;({struct Cyc___cycFILE*_tmp2FB=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2FA=({const char*_tmp4="\n";_tag_dyneither(_tmp4,sizeof(char),2U);});Cyc_fprintf(_tmp2FB,_tmp2FA,_tag_dyneither(_tmp3,sizeof(void*),0U));});});else{
if((char*)_tmpD.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp7=({struct Cyc_Int_pa_PrintArg_struct _tmp2EC;_tmp2EC.tag=1U,_tmp2EC.f1=(unsigned long)((int)_tmpC);_tmp2EC;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct Cyc___cycFILE*_tmp2FD=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2FC=({const char*_tmp6="\n# %d\n";_tag_dyneither(_tmp6,sizeof(char),7U);});Cyc_fprintf(_tmp2FD,_tmp2FC,_tag_dyneither(_tmp5,sizeof(void*),1U));});});else{
# 164
({struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp2EE;_tmp2EE.tag=1U,_tmp2EE.f1=(unsigned long)((int)_tmpC);_tmp2EE;});struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmp2ED;_tmp2ED.tag=0U,_tmp2ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD);_tmp2ED;});void*_tmp8[2U];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB;({struct Cyc___cycFILE*_tmp2FF=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2FE=({const char*_tmp9="\n# %d %s\n";_tag_dyneither(_tmp9,sizeof(char),10U);});Cyc_fprintf(_tmp2FF,_tmp2FE,_tag_dyneither(_tmp8,sizeof(void*),2U));});});}}
# 166
last_file=_tmpD;
last_line=_tmpC;};}
# 172
static void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
Cyc_Absyndump_dump(*s);}
# 176
static void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 180
static void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
static void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 196
f(env,l->hd);}
# 198
static void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 200
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 204
static void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 206
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 210
static void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 212
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 218
static void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
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
static void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp19="restrict";_tag_dyneither(_tmp19,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp1A="volatile";_tag_dyneither(_tmp1A,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp1B="const";_tag_dyneither(_tmp1B,sizeof(char),6U);}));}
# 268
static void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1C=sc;switch(_tmp1C){case Cyc_Absyn_Static: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp1D="static";_tag_dyneither(_tmp1D,sizeof(char),7U);}));return;case Cyc_Absyn_Public: _LL3: _LL4:
 return;case Cyc_Absyn_Extern: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp1E="extern";_tag_dyneither(_tmp1E,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1F="extern \"C\"";_tag_dyneither(_tmp1F,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract: _LL9: _LLA:
 Cyc_Absyndump_dump(({const char*_tmp20="abstract";_tag_dyneither(_tmp20,sizeof(char),9U);}));return;case Cyc_Absyn_Register: _LLB: _LLC:
 Cyc_Absyndump_dump(({const char*_tmp21="register";_tag_dyneither(_tmp21,sizeof(char),9U);}));return;default: _LLD: _LLE:
 return;}_LL0:;}
# 279
static void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp22=k;switch(_tmp22){case Cyc_Absyn_StructA: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp23="struct";_tag_dyneither(_tmp23,sizeof(char),7U);}));return;case Cyc_Absyn_UnionA: _LL3: _LL4:
 goto _LL6;default: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp24="union";_tag_dyneither(_tmp24,sizeof(char),6U);}));return;}_LL0:;}
# 287
static void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp25=ka;enum Cyc_Absyn_KindQual _tmp37;enum Cyc_Absyn_AliasQual _tmp36;_LL1: _tmp37=_tmp25->kind;_tmp36=_tmp25->aliasqual;_LL2:;{
enum Cyc_Absyn_KindQual _tmp26=_tmp37;switch(_tmp26){case Cyc_Absyn_EffKind: _LL4: _LL5:
 Cyc_Absyndump_dump(({const char*_tmp27="E";_tag_dyneither(_tmp27,sizeof(char),2U);}));return;case Cyc_Absyn_IntKind: _LL6: _LL7:
 Cyc_Absyndump_dump(({const char*_tmp28="I";_tag_dyneither(_tmp28,sizeof(char),2U);}));return;case Cyc_Absyn_BoolKind: _LL8: _LL9:
 Cyc_Absyndump_dump(({const char*_tmp29="BOOL";_tag_dyneither(_tmp29,sizeof(char),5U);}));return;case Cyc_Absyn_PtrBndKind: _LLA: _LLB:
 Cyc_Absyndump_dump(({const char*_tmp2A="PTRBND";_tag_dyneither(_tmp2A,sizeof(char),7U);}));return;default: _LLC: _LLD: {
# 295
void(*_tmp2B)(struct _dyneither_ptr s)=Cyc_Absyndump_dump;
{enum Cyc_Absyn_AliasQual _tmp2C=_tmp36;switch(_tmp2C){case Cyc_Absyn_Aliasable: _LLF: _LL10:
 goto _LLE;case Cyc_Absyn_Unique: _LL11: _LL12:
 Cyc_Absyndump_dump_char((int)'U');_tmp2B=Cyc_Absyndump_dump_nospace;goto _LLE;case Cyc_Absyn_Top: _LL13: _LL14:
 Cyc_Absyndump_dump_char((int)'T');_tmp2B=Cyc_Absyndump_dump_nospace;goto _LLE;default: _LL15: _LL16:
({void*_tmp2D=0U;({struct _dyneither_ptr _tmp300=({const char*_tmp2E="Absyndump::dumpkind";_tag_dyneither(_tmp2E,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp300,_tag_dyneither(_tmp2D,sizeof(void*),0U));});});}_LLE:;}{
# 302
enum Cyc_Absyn_KindQual _tmp2F=_tmp37;switch(_tmp2F){case Cyc_Absyn_AnyKind: _LL18: _LL19:
({void(*_tmp301)(struct _dyneither_ptr s)=_tmp2B;_tmp301(({const char*_tmp30="A";_tag_dyneither(_tmp30,sizeof(char),2U);}));});return;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
({void(*_tmp302)(struct _dyneither_ptr s)=_tmp2B;_tmp302(({const char*_tmp31="M";_tag_dyneither(_tmp31,sizeof(char),2U);}));});return;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
({void(*_tmp303)(struct _dyneither_ptr s)=_tmp2B;_tmp303(({const char*_tmp32="B";_tag_dyneither(_tmp32,sizeof(char),2U);}));});return;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
({void(*_tmp304)(struct _dyneither_ptr s)=_tmp2B;_tmp304(({const char*_tmp33="R";_tag_dyneither(_tmp33,sizeof(char),2U);}));});return;default: _LL20: _LL21:
({void*_tmp34=0U;({struct _dyneither_ptr _tmp305=({const char*_tmp35="Absyndump::dumpkind";_tag_dyneither(_tmp35,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp305,_tag_dyneither(_tmp34,sizeof(void*),0U));});});}_LL17:;};}}_LL3:;};}
# 312
static void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp308=ts;struct _dyneither_ptr _tmp307=({const char*_tmp38="<";_tag_dyneither(_tmp38,sizeof(char),2U);});struct _dyneither_ptr _tmp306=({const char*_tmp39=">";_tag_dyneither(_tmp39,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp308,_tmp307,_tmp306,({const char*_tmp3A=",";_tag_dyneither(_tmp3A,sizeof(char),2U);}));});}
# 315
static void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if((int)*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== (int)'#'){
Cyc_Absyndump_dump(({const char*_tmp3B="`G";_tag_dyneither(_tmp3B,sizeof(char),3U);}));
{void*_tmp3C=Cyc_Absyn_compress_kb(tv->kind);void*_tmp3D=_tmp3C;struct Cyc_Absyn_Kind*_tmp40;struct Cyc_Absyn_Kind*_tmp3F;switch(*((int*)_tmp3D)){case 0U: _LL1: _tmp3F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp3D)->f1;_LL2:
 _tmp40=_tmp3F;goto _LL4;case 2U: _LL3: _tmp40=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp3D)->f2;_LL4:
 Cyc_Absyndump_dumpkind(_tmp40);goto _LL0;default: _LL5: _LL6:
 Cyc_Absyndump_dump_nospace(({const char*_tmp3E="K";_tag_dyneither(_tmp3E,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 324
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 326
Cyc_Absyndump_dump(n);}}
# 328
static void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp41=Cyc_Absyn_compress_kb(tv->kind);void*_tmp42=_tmp41;struct Cyc_Absyn_Kind*_tmp44;switch(*((int*)_tmp42)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp42)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp42)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL5: _LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _tmp44=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp42)->f1;_LL8:
 Cyc_Absyndump_dump(({const char*_tmp43="::";_tag_dyneither(_tmp43,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(_tmp44);goto _LL0;}}_LL0:;};}
# 337
static void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp30B=tvs;struct _dyneither_ptr _tmp30A=({const char*_tmp45="<";_tag_dyneither(_tmp45,sizeof(char),2U);});struct _dyneither_ptr _tmp309=({const char*_tmp46=">";_tag_dyneither(_tmp46,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp30B,_tmp30A,_tmp309,({const char*_tmp47=",";_tag_dyneither(_tmp47,sizeof(char),2U);}));});}
# 340
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp30E=tvs;struct _dyneither_ptr _tmp30D=({const char*_tmp48="<";_tag_dyneither(_tmp48,sizeof(char),2U);});struct _dyneither_ptr _tmp30C=({const char*_tmp49=">";_tag_dyneither(_tmp49,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp30E,_tmp30D,_tmp30C,({const char*_tmp4A=",";_tag_dyneither(_tmp4A,sizeof(char),2U);}));});}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 344
static void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 347
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp311=ts;struct _dyneither_ptr _tmp310=({const char*_tmp4B="(";_tag_dyneither(_tmp4B,sizeof(char),2U);});struct _dyneither_ptr _tmp30F=({const char*_tmp4C=")";_tag_dyneither(_tmp4C,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp311,_tmp310,_tmp30F,({const char*_tmp4D=",";_tag_dyneither(_tmp4D,sizeof(char),2U);}));});}
# 351
static void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp4E=(void*)atts->hd;void*_tmp4F=_tmp4E;switch(*((int*)_tmp4F)){case 1U: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp50="_stdcall";_tag_dyneither(_tmp50,sizeof(char),9U);}));return;case 2U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp51="_cdecl";_tag_dyneither(_tmp51,sizeof(char),7U);}));return;case 3U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp52="_fastcall";_tag_dyneither(_tmp52,sizeof(char),10U);}));return;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 361
static void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 363
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp53=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp54=_tmp53;switch(*((int*)_tmp54)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 371
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp55="__declspec(";_tag_dyneither(_tmp55,sizeof(char),12U);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp56=(void*)atts->hd;void*_tmp57=_tmp56;switch(*((int*)_tmp57)){case 1U: _LLA: _LLB:
 goto _LLD;case 2U: _LLC: _LLD:
 goto _LLF;case 3U: _LLE: _LLF:
 goto _LL9;default: _LL10: _LL11:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 381
Cyc_Absyndump_dump_char((int)')');}
# 384
static void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp58=Cyc_Cyclone_c_compiler;if(_tmp58 == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 388
 Cyc_Absyndump_dump(({const char*_tmp59=" __attribute__((";_tag_dyneither(_tmp59,sizeof(char),17U);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp5A=",";_tag_dyneither(_tmp5A,sizeof(char),2U);}));}
# 393
Cyc_Absyndump_dump(({const char*_tmp5B=")) ";_tag_dyneither(_tmp5B,sizeof(char),4U);}));
return;}else{_LL3: _LL4:
# 396
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL0:;};}
# 401
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 405
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
{void*_tmp5C=Cyc_Tcutil_compress(t);void*_tmp5D=_tmp5C;struct Cyc_List_List*_tmp65;void*_tmp64;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D)->f2 != 0){_LL1: _tmp64=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D)->f2)->hd;_LL2:
 rgions=({struct Cyc_List_List*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->hd=_tmp64,_tmp5E->tl=rgions;_tmp5E;});goto _LL0;}else{goto _LL5;}case 9U: _LL3: _tmp65=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D)->f2;_LL4:
# 411
 for(0;_tmp65 != 0;_tmp65=_tmp65->tl){
struct _tuple15 _tmp5F=Cyc_Absyndump_effects_split((void*)_tmp65->hd);struct _tuple15 _tmp60=_tmp5F;struct Cyc_List_List*_tmp62;struct Cyc_List_List*_tmp61;_LL8: _tmp62=_tmp60.f1;_tmp61=_tmp60.f2;_LL9:;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp62,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp61,effects);}
# 416
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 effects=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->hd=t,_tmp63->tl=effects;_tmp63;});goto _LL0;}_LL0:;}
# 419
return({struct _tuple15 _tmp2EF;_tmp2EF.f1=rgions,_tmp2EF.f2=effects;_tmp2EF;});}
# 422
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp66=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp67=_tmp66;struct Cyc_List_List*_tmp6F;struct Cyc_List_List*_tmp6E;_LL1: _tmp6F=_tmp67.f1;_tmp6E=_tmp67.f2;_LL2:;
_tmp6F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F);
_tmp6E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E);
if(_tmp6E != 0){
({struct Cyc_List_List*_tmp314=_tmp6E;struct _dyneither_ptr _tmp313=({const char*_tmp68="";_tag_dyneither(_tmp68,sizeof(char),1U);});struct _dyneither_ptr _tmp312=({const char*_tmp69="";_tag_dyneither(_tmp69,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpntyp,_tmp314,_tmp313,_tmp312,({const char*_tmp6A="+";_tag_dyneither(_tmp6A,sizeof(char),2U);}));});
if(_tmp6F != 0)Cyc_Absyndump_dump_char((int)'+');}
# 430
if(_tmp6F != 0  || _tmp6E == 0)
({struct Cyc_List_List*_tmp317=_tmp6F;struct _dyneither_ptr _tmp316=({const char*_tmp6B="{";_tag_dyneither(_tmp6B,sizeof(char),2U);});struct _dyneither_ptr _tmp315=({const char*_tmp6C="}";_tag_dyneither(_tmp6C,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumprgn,_tmp317,_tmp316,_tmp315,({const char*_tmp6D=",";_tag_dyneither(_tmp6D,sizeof(char),2U);}));});}
# 435
static void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp70=t;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;struct _tuple0*_tmpCF;struct Cyc_List_List*_tmpCE;struct Cyc_Absyn_Datatypedecl*_tmpCD;struct Cyc_Absyn_Enumdecl*_tmpCC;struct Cyc_Absyn_Aggrdecl*_tmpCB;enum Cyc_Absyn_AggrKind _tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpC8;struct Cyc_Core_Opt*_tmpC7;void*_tmpC6;int _tmpC5;struct Cyc_Core_Opt*_tmpC4;int _tmpC3;struct Cyc_Absyn_Tvar*_tmpC2;void*_tmpC1;struct Cyc_List_List*_tmpC0;switch(*((int*)_tmp70)){case 4U: _LL1: _LL2:
# 438
 goto _LL4;case 5U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return;case 0U: if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70)->f1)->tag == 0U){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp71="void";_tag_dyneither(_tmp71,sizeof(char),5U);}));return;}else{_LL1F: _tmpC1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70)->f1;_tmpC0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp70)->f2;_LL20: {
# 458
void*_tmp7B=_tmpC1;struct _dyneither_ptr _tmpBF;struct Cyc_List_List*_tmpBE;struct _tuple0*_tmpBD;union Cyc_Absyn_AggrInfo _tmpBC;int _tmpBB;union Cyc_Absyn_DatatypeFieldInfo _tmpBA;union Cyc_Absyn_DatatypeInfo _tmpB9;switch(*((int*)_tmp7B)){case 18U: _LL22: _tmpB9=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp7B)->f1;_LL23:
# 460
{union Cyc_Absyn_DatatypeInfo _tmp7C=_tmpB9;struct _tuple0*_tmp82;int _tmp81;struct _tuple0*_tmp80;int _tmp7F;if((_tmp7C.UnknownDatatype).tag == 1){_LL69: _tmp80=((_tmp7C.UnknownDatatype).val).name;_tmp7F=((_tmp7C.UnknownDatatype).val).is_extensible;_LL6A:
 _tmp82=_tmp80;_tmp81=_tmp7F;goto _LL6C;}else{_LL6B: _tmp82=(*(_tmp7C.KnownDatatype).val)->name;_tmp81=(*(_tmp7C.KnownDatatype).val)->is_extensible;_LL6C:
# 463
 if(_tmp81)Cyc_Absyndump_dump(({const char*_tmp7D="@extensible";_tag_dyneither(_tmp7D,sizeof(char),12U);}));
Cyc_Absyndump_dump(({const char*_tmp7E="datatype";_tag_dyneither(_tmp7E,sizeof(char),9U);}));Cyc_Absyndump_dumpqvar(_tmp82);Cyc_Absyndump_dumptps(_tmpC0);
goto _LL68;}_LL68:;}
# 467
return;case 19U: _LL24: _tmpBA=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp7B)->f1;_LL25:
# 469
{union Cyc_Absyn_DatatypeFieldInfo _tmp83=_tmpBA;struct _tuple0*_tmp8C;int _tmp8B;struct _tuple0*_tmp8A;struct _tuple0*_tmp89;struct _tuple0*_tmp88;int _tmp87;if((_tmp83.UnknownDatatypefield).tag == 1){_LL6E: _tmp89=((_tmp83.UnknownDatatypefield).val).datatype_name;_tmp88=((_tmp83.UnknownDatatypefield).val).field_name;_tmp87=((_tmp83.UnknownDatatypefield).val).is_extensible;_LL6F:
# 471
 _tmp8C=_tmp89;_tmp8B=_tmp87;_tmp8A=_tmp88;goto _LL71;}else{_LL70: _tmp8C=(((_tmp83.KnownDatatypefield).val).f1)->name;_tmp8B=(((_tmp83.KnownDatatypefield).val).f1)->is_extensible;_tmp8A=(((_tmp83.KnownDatatypefield).val).f2)->name;_LL71:
# 474
 if(_tmp8B)Cyc_Absyndump_dump(({const char*_tmp84="@extensible ";_tag_dyneither(_tmp84,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp85="datatype ";_tag_dyneither(_tmp85,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(_tmp8C);Cyc_Absyndump_dump(({const char*_tmp86=".";_tag_dyneither(_tmp86,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(_tmp8A);
Cyc_Absyndump_dumptps(_tmpC0);
goto _LL6D;}_LL6D:;}
# 479
return;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp7B)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp7B)->f2){case Cyc_Absyn_Int_sz: _LL26: _LL27:
 goto _LL29;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
# 482
 goto _LL2D;case Cyc_Absyn_Char_sz: _LL2E: _LL2F:
# 484
 Cyc_Absyndump_dump(({const char*_tmp8F="char";_tag_dyneither(_tmp8F,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz: _LL34: _LL35:
# 487
 goto _LL37;case Cyc_Absyn_LongLong_sz: _LL40: _LL41:
# 493
 goto _LL43;default: goto _LL66;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp7B)->f2){case Cyc_Absyn_Int_sz: _LL28: _LL29:
# 481
 Cyc_Absyndump_dump(({const char*_tmp8D="int";_tag_dyneither(_tmp8D,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL2C: _LL2D:
# 483
 Cyc_Absyndump_dump(({const char*_tmp8E="long";_tag_dyneither(_tmp8E,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz: _LL30: _LL31:
# 485
 Cyc_Absyndump_dump(({const char*_tmp90="signed char";_tag_dyneither(_tmp90,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL36: _LL37:
# 488
 Cyc_Absyndump_dump(({const char*_tmp92="short";_tag_dyneither(_tmp92,sizeof(char),6U);}));return;case Cyc_Absyn_LongLong_sz: _LL42: _LL43: {
# 495
enum Cyc_Cyclone_C_Compilers _tmp97=Cyc_Cyclone_c_compiler;if(_tmp97 == Cyc_Cyclone_Gcc_c){_LL73: _LL74:
 Cyc_Absyndump_dump(({const char*_tmp98="long long";_tag_dyneither(_tmp98,sizeof(char),10U);}));return;}else{_LL75: _LL76:
 Cyc_Absyndump_dump(({const char*_tmp99="__int64";_tag_dyneither(_tmp99,sizeof(char),8U);}));return;}_LL72:;}default: goto _LL66;}case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp7B)->f2){case Cyc_Absyn_Char_sz: _LL32: _LL33:
# 486
 Cyc_Absyndump_dump(({const char*_tmp91="unsigned char";_tag_dyneither(_tmp91,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz: _LL38: _LL39:
# 489
 Cyc_Absyndump_dump(({const char*_tmp93="unsigned short";_tag_dyneither(_tmp93,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz: _LL3A: _LL3B:
 Cyc_Absyndump_dump(({const char*_tmp94="unsigned int";_tag_dyneither(_tmp94,sizeof(char),13U);}));return;case Cyc_Absyn_Long_sz: _LL3C: _LL3D:
 Cyc_Absyndump_dump(({const char*_tmp95="unsigned long";_tag_dyneither(_tmp95,sizeof(char),14U);}));return;case Cyc_Absyn_LongLong_sz: _LL3E: _LL3F:
 Cyc_Absyndump_dump(({const char*_tmp96="unsigned";_tag_dyneither(_tmp96,sizeof(char),9U);}));goto _LL41;default: goto _LL66;}default: goto _LL66;}case 2U: _LL44: _tmpBB=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp7B)->f1;_LL45: {
# 500
int _tmp9A=_tmpBB;switch(_tmp9A){case 0U: _LL78: _LL79:
 Cyc_Absyndump_dump(({const char*_tmp9B="float";_tag_dyneither(_tmp9B,sizeof(char),6U);}));return;case 1U: _LL7A: _LL7B:
 Cyc_Absyndump_dump(({const char*_tmp9C="double";_tag_dyneither(_tmp9C,sizeof(char),7U);}));return;default: _LL7C: _LL7D:
 Cyc_Absyndump_dump(({const char*_tmp9D="long double";_tag_dyneither(_tmp9D,sizeof(char),12U);}));return;}_LL77:;}case 20U: _LL46: _tmpBC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp7B)->f1;_LL47: {
# 506
struct _tuple10 _tmp9E=Cyc_Absyn_aggr_kinded_name(_tmpBC);struct _tuple10 _tmp9F=_tmp9E;enum Cyc_Absyn_AggrKind _tmpA1;struct _tuple0*_tmpA0;_LL7F: _tmpA1=_tmp9F.f1;_tmpA0=_tmp9F.f2;_LL80:;
Cyc_Absyndump_dumpaggr_kind(_tmpA1);Cyc_Absyndump_dumpqvar(_tmpA0);Cyc_Absyndump_dumptps(_tmpC0);
return;}case 15U: _LL48: _tmpBD=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp7B)->f1;_LL49:
 Cyc_Absyndump_dump(({const char*_tmpA2="enum";_tag_dyneither(_tmpA2,sizeof(char),5U);}));Cyc_Absyndump_dumpqvar(_tmpBD);return;case 16U: _LL4A: _tmpBE=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp7B)->f1;_LL4B:
 Cyc_Absyndump_dump(({const char*_tmpA3="enum{";_tag_dyneither(_tmpA3,sizeof(char),6U);}));Cyc_Absyndump_dumpenumfields(_tmpBE);Cyc_Absyndump_dump(({const char*_tmpA4="}";_tag_dyneither(_tmpA4,sizeof(char),2U);}));return;case 17U: _LL4C: _tmpBF=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp7B)->f1;_LL4D:
 Cyc_Absyndump_dump(_tmpBF);return;case 3U: _LL4E: _LL4F:
 Cyc_Absyndump_dump(({const char*_tmpA5="region_t<";_tag_dyneither(_tmpA5,sizeof(char),10U);}));Cyc_Absyndump_dumprgn((void*)((struct Cyc_List_List*)_check_null(_tmpC0))->hd);Cyc_Absyndump_dump(({const char*_tmpA6=">";_tag_dyneither(_tmpA6,sizeof(char),2U);}));return;case 4U: _LL50: _LL51:
 Cyc_Absyndump_dump(({const char*_tmpA7="tag_t<";_tag_dyneither(_tmpA7,sizeof(char),7U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(_tmpC0))->hd);Cyc_Absyndump_dump(({const char*_tmpA8=">";_tag_dyneither(_tmpA8,sizeof(char),2U);}));return;case 6U: _LL52: _LL53:
 Cyc_Absyndump_dump(({const char*_tmpA9="`U";_tag_dyneither(_tmpA9,sizeof(char),3U);}));return;case 7U: _LL54: _LL55:
 Cyc_Absyndump_dump(({const char*_tmpAA="`RC";_tag_dyneither(_tmpAA,sizeof(char),4U);}));return;case 5U: _LL56: _LL57:
 Cyc_Absyndump_dump(({const char*_tmpAB="`H";_tag_dyneither(_tmpAB,sizeof(char),3U);}));return;case 8U: _LL58: _LL59:
 Cyc_Absyndump_dump(({const char*_tmpAC="{";_tag_dyneither(_tmpAC,sizeof(char),2U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(_tmpC0))->hd);Cyc_Absyndump_dump(({const char*_tmpAD="}";_tag_dyneither(_tmpAD,sizeof(char),2U);}));return;case 10U: _LL5A: _LL5B:
 Cyc_Absyndump_dump(({const char*_tmpAE="regions(";_tag_dyneither(_tmpAE,sizeof(char),9U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(_tmpC0))->hd);Cyc_Absyndump_dump(({const char*_tmpAF=")";_tag_dyneither(_tmpAF,sizeof(char),2U);}));return;case 9U: _LL5C: _LL5D:
({struct Cyc_List_List*_tmp31A=_tmpC0;struct _dyneither_ptr _tmp319=({const char*_tmpB0="";_tag_dyneither(_tmpB0,sizeof(char),1U);});struct _dyneither_ptr _tmp318=({const char*_tmpB1="";_tag_dyneither(_tmpB1,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumptyp,_tmp31A,_tmp319,_tmp318,({const char*_tmpB2="+";_tag_dyneither(_tmpB2,sizeof(char),2U);}));});return;case 11U: _LL5E: _LL5F:
 Cyc_Absyndump_dump(({const char*_tmpB3="@true";_tag_dyneither(_tmpB3,sizeof(char),6U);}));return;case 12U: _LL60: _LL61:
 Cyc_Absyndump_dump(({const char*_tmpB4="@false";_tag_dyneither(_tmpB4,sizeof(char),7U);}));return;case 13U: _LL62: _LL63:
 Cyc_Absyndump_dump(({const char*_tmpB5="@thin";_tag_dyneither(_tmpB5,sizeof(char),6U);}));return;case 14U: _LL64: _LL65:
 Cyc_Absyndump_dump(({const char*_tmpB6="@fat";_tag_dyneither(_tmpB6,sizeof(char),5U);}));return;default: _LL66: _LL67:
({void*_tmpB7=0U;({struct _dyneither_ptr _tmp31B=({const char*_tmpB8="bad type constructor";_tag_dyneither(_tmpB8,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp31B,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});}_LL21:;}}case 2U: _LL9: _tmpC2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->f1;_LLA:
# 442
 Cyc_Absyndump_dumptvar(_tmpC2);return;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f2 == 0){_LLB: _tmpC4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f1;_tmpC3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f3;_LLC:
# 444
 Cyc_Absyndump_dump(({const char*_tmp72="`E";_tag_dyneither(_tmp72,sizeof(char),3U);}));
if(_tmpC4 == 0)Cyc_Absyndump_dump(({const char*_tmp73="K";_tag_dyneither(_tmp73,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmpC4->v);}
Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp76=({struct Cyc_Int_pa_PrintArg_struct _tmp2F0;_tmp2F0.tag=1U,_tmp2F0.f1=(unsigned long)_tmpC3;_tmp2F0;});void*_tmp74[1U];_tmp74[0]=& _tmp76;({struct _dyneither_ptr _tmp31C=({const char*_tmp75="%d";_tag_dyneither(_tmp75,sizeof(char),3U);});Cyc_aprintf(_tmp31C,_tag_dyneither(_tmp74,sizeof(void*),1U));});}));return;}else{_LLD: _tmpC7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f1;_tmpC6=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f2;_tmpC5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp70)->f3;_LLE:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmpC6));return;}case 6U: _LLF: _tmpC8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp70)->f1;_LL10:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpC8);return;case 7U: _LL11: _tmpCA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp70)->f1;_tmpC9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp70)->f2;_LL12:
# 450
 Cyc_Absyndump_dumpaggr_kind(_tmpCA);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpC9);Cyc_Absyndump_dump_char((int)'}');return;case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)){case 0U: _LL13: _tmpCB=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)->f1;_LL14:
 Cyc_Absyndump_dump_aggrdecl(_tmpCB);return;case 1U: _LL15: _tmpCC=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)->f1;_LL16:
 Cyc_Absyndump_dump_enumdecl(_tmpCC);return;default: _LL17: _tmpCD=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp70)->f1)->r)->f1;_LL18:
 Cyc_Absyndump_dump_datatypedecl(_tmpCD);return;}case 8U: _LL19: _tmpCF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp70)->f1;_tmpCE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp70)->f2;_LL1A:
 Cyc_Absyndump_dumpqvar(_tmpCF),Cyc_Absyndump_dumptps(_tmpCE);return;case 9U: _LL1B: _tmpD0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp70)->f1;_LL1C:
 Cyc_Absyndump_dump(({const char*_tmp77="valueof_t(";_tag_dyneither(_tmp77,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(_tmpD0);Cyc_Absyndump_dump(({const char*_tmp78=")";_tag_dyneither(_tmp78,sizeof(char),2U);}));return;default: _LL1D: _tmpD1=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp70)->f1;_LL1E:
 Cyc_Absyndump_dump(({const char*_tmp79="typeof(";_tag_dyneither(_tmp79,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmpD1);Cyc_Absyndump_dump(({const char*_tmp7A=")";_tag_dyneither(_tmp7A,sizeof(char),2U);}));return;}_LL0:;}
# 529
static void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 532
static void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 535
static void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
struct _tuple16*_tmpD2=cmp;void*_tmpD4;void*_tmpD3;_LL1: _tmpD4=_tmpD2->f1;_tmpD3=_tmpD2->f2;_LL2:;
Cyc_Absyndump_dumpeff(_tmpD4);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD3);}
# 539
static void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp31D=rgn_po;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp31D,({const char*_tmpD5=",";_tag_dyneither(_tmpD5,sizeof(char),2U);}));});}
# 542
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 546
({struct Cyc_List_List*_tmp320=args;struct _dyneither_ptr _tmp31F=({const char*_tmpD6="(";_tag_dyneither(_tmpD6,sizeof(char),2U);});struct _dyneither_ptr _tmp31E=({const char*_tmpD7="";_tag_dyneither(_tmpD7,sizeof(char),1U);});((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpfunarg,_tmp320,_tmp31F,_tmp31E,({const char*_tmpD8=",";_tag_dyneither(_tmpD8,sizeof(char),2U);}));});
if(c_varargs  || cyc_varargs != 0)
Cyc_Absyndump_dump(({const char*_tmpD9=",...";_tag_dyneither(_tmpD9,sizeof(char),5U);}));
if(cyc_varargs != 0){
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpDA=" inject ";_tag_dyneither(_tmpDA,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(({struct _tuple8*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->f1=cyc_varargs->name,_tmpDB->f2=cyc_varargs->tq,_tmpDB->f3=cyc_varargs->type;_tmpDB;}));}
# 553
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(effopt);}
# 557
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 561
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpDC=" @requires(";_tag_dyneither(_tmpDC,sizeof(char),12U);}));
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 567
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpDD=" @ensures(";_tag_dyneither(_tmpDD,sizeof(char),11U);}));
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 574
static void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _tmp321=Cyc_Absyn_empty_tqual(0U);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp321,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 578
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpDE=d;struct _dyneither_ptr*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDE)->tag == 0U){_LL1: _tmpE0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDE)->f1;_LL2:
 Cyc_Absyndump_dump(({const char*_tmpDF=".[";_tag_dyneither(_tmpDF,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(_tmpE0);Cyc_Absyndump_dump_char((int)']');goto _LL0;}else{_LL3: _tmpE1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpDE)->f1;_LL4:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpE1);goto _LL0;}_LL0:;}
# 584
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_tmpE2=f;unsigned int _tmpE7;struct _dyneither_ptr*_tmpE6;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpE2)->tag == 0U){_LL1: _tmpE6=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpE2)->f1;_LL2:
 Cyc_Absyndump_dump_nospace(*_tmpE6);return;}else{_LL3: _tmpE7=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpE2)->f1;_LL4:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE5=({struct Cyc_Int_pa_PrintArg_struct _tmp2F1;_tmp2F1.tag=1U,_tmp2F1.f1=(unsigned long)((int)_tmpE7);_tmp2F1;});void*_tmpE3[1U];_tmpE3[0]=& _tmpE5;({struct _dyneither_ptr _tmp322=({const char*_tmpE4="%d";_tag_dyneither(_tmpE4,sizeof(char),3U);});Cyc_aprintf(_tmp322,_tag_dyneither(_tmpE3,sizeof(void*),1U));});}));return;}_LL0:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 591
static void Cyc_Absyndump_dumpde(struct _tuple17*de){
({struct Cyc_List_List*_tmp325=(*de).f1;struct _dyneither_ptr _tmp324=({const char*_tmpE8="";_tag_dyneither(_tmpE8,sizeof(char),1U);});struct _dyneither_ptr _tmp323=({const char*_tmpE9="=";_tag_dyneither(_tmpE9,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp325,_tmp324,_tmp323,({const char*_tmpEA="=";_tag_dyneither(_tmpEA,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 596
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp329=inprec;struct Cyc_List_List*_tmp328=es;struct _dyneither_ptr _tmp327=({const char*_tmpEB="";_tag_dyneither(_tmpEB,sizeof(char),1U);});struct _dyneither_ptr _tmp326=({const char*_tmpEC="";_tag_dyneither(_tmpEC,sizeof(char),1U);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp329,_tmp328,_tmp327,_tmp326,({const char*_tmpED=",";_tag_dyneither(_tmpED,sizeof(char),2U);}));});}
# 600
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpEE="(";_tag_dyneither(_tmpEE,sizeof(char),2U);}));
{void*_tmpEF=e->r;void*_tmpF0=_tmpEF;struct Cyc_Absyn_Stmt*_tmp196;struct Cyc_Core_Opt*_tmp195;struct Cyc_List_List*_tmp194;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp192;int _tmp191;struct Cyc_Absyn_Exp*_tmp190;void**_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;int _tmp18D;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Datatypefield*_tmp18B;struct Cyc_Absyn_Enumfield*_tmp18A;struct Cyc_Absyn_Enumfield*_tmp189;struct Cyc_List_List*_tmp188;struct _tuple0*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_List_List*_tmp185;struct Cyc_Absyn_Exp*_tmp184;void*_tmp183;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_List_List*_tmp17F;struct _tuple8*_tmp17E;struct Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17C;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp179;struct _dyneither_ptr*_tmp178;struct Cyc_Absyn_Exp*_tmp177;struct _dyneither_ptr*_tmp176;void*_tmp175;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Exp*_tmp173;struct _dyneither_ptr*_tmp172;int _tmp171;struct _dyneither_ptr _tmp170;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp16E;struct Cyc_Absyn_Exp*_tmp16D;void*_tmp16C;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp168;void*_tmp167;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Core_Opt*_tmp152;struct Cyc_Absyn_Exp*_tmp151;enum Cyc_Absyn_Primop _tmp150;struct Cyc_List_List*_tmp14F;struct _dyneither_ptr _tmp14E;void*_tmp14D;struct _dyneither_ptr _tmp14C;struct _dyneither_ptr _tmp14B;struct _dyneither_ptr _tmp14A;enum Cyc_Absyn_Sign _tmp149;long long _tmp148;int _tmp147;int _tmp146;int _tmp145;enum Cyc_Absyn_Sign _tmp144;short _tmp143;struct _dyneither_ptr _tmp142;enum Cyc_Absyn_Sign _tmp141;char _tmp140;switch(*((int*)_tmpF0)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).String_c).tag){case 2U: _LL1: _tmp141=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Char_c).val).f1;_tmp140=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Char_c).val).f2;_LL2:
# 606
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp140));Cyc_Absyndump_dump_char((int)'\'');
goto _LL0;case 3U: _LL3: _tmp142=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Wchar_c).val;_LL4:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF3=({struct Cyc_String_pa_PrintArg_struct _tmp2F2;_tmp2F2.tag=0U,_tmp2F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp142);_tmp2F2;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({struct _dyneither_ptr _tmp32A=({const char*_tmpF2="L'%s'";_tag_dyneither(_tmpF2,sizeof(char),6U);});Cyc_aprintf(_tmp32A,_tag_dyneither(_tmpF1,sizeof(void*),1U));});}));goto _LL0;case 4U: _LL5: _tmp144=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Short_c).val).f1;_tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Short_c).val).f2;_LL6:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=1U,_tmp2F3.f1=(unsigned long)((int)_tmp143);_tmp2F3;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp32B=({const char*_tmpF5="%d";_tag_dyneither(_tmpF5,sizeof(char),3U);});Cyc_aprintf(_tmp32B,_tag_dyneither(_tmpF4,sizeof(void*),1U));});}));goto _LL0;case 5U: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL7: _tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Int_c).val).f2;_LL8:
 _tmp146=_tmp145;goto _LLA;case Cyc_Absyn_Signed: _LL9: _tmp146=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Int_c).val).f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF9=({struct Cyc_Int_pa_PrintArg_struct _tmp2F4;_tmp2F4.tag=1U,_tmp2F4.f1=(unsigned long)_tmp146;_tmp2F4;});void*_tmpF7[1U];_tmpF7[0]=& _tmpF9;({struct _dyneither_ptr _tmp32C=({const char*_tmpF8="%d";_tag_dyneither(_tmpF8,sizeof(char),3U);});Cyc_aprintf(_tmp32C,_tag_dyneither(_tmpF7,sizeof(void*),1U));});}));goto _LL0;default: _LLB: _tmp147=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Int_c).val).f2;_LLC:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFC=({struct Cyc_Int_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=1U,_tmp2F5.f1=(unsigned int)_tmp147;_tmp2F5;});void*_tmpFA[1U];_tmpFA[0]=& _tmpFC;({struct _dyneither_ptr _tmp32D=({const char*_tmpFB="%uU";_tag_dyneither(_tmpFB,sizeof(char),4U);});Cyc_aprintf(_tmp32D,_tag_dyneither(_tmpFA,sizeof(void*),1U));});}));goto _LL0;}case 6U: _LLD: _tmp149=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).LongLong_c).val).f1;_tmp148=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).LongLong_c).val).f2;_LLE:
# 615
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp148));goto _LL0;case 7U: _LLF: _tmp14A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Float_c).val).f1;_LL10:
 Cyc_Absyndump_dump(_tmp14A);goto _LL0;case 1U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmpFD="NULL";_tag_dyneither(_tmpFD,sizeof(char),5U);}));goto _LL0;case 8U: _LL13: _tmp14B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).String_c).val;_LL14:
# 619
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp14B));Cyc_Absyndump_dump_char((int)'"');
goto _LL0;default: _LL15: _tmp14C=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).Wstring_c).val;_LL16:
# 622
 Cyc_Absyndump_dump(({const char*_tmpFE="L\"";_tag_dyneither(_tmpFE,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(_tmp14C);Cyc_Absyndump_dump_char((int)'"');goto _LL0;}case 1U: _LL17: _tmp14D=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL18:
# 624
 Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(_tmp14D));goto _LL0;case 2U: _LL19: _tmp14E=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL1A:
# 627
 Cyc_Absyndump_dump(({const char*_tmpFF="__cyclone_pragma__(";_tag_dyneither(_tmpFF,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(_tmp14E);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 3U: _LL1B: _tmp150=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp14F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL1C: {
# 630
struct _dyneither_ptr _tmp100=Cyc_Absynpp_prim2str(_tmp150);
{int _tmp101=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp14F);int _tmp102=_tmp101;switch(_tmp102){case 1U: _LL70: _LL71:
# 633
 if((int)_tmp150 == (int)19U){
Cyc_Absyndump_dump(({const char*_tmp103="numelts(";_tag_dyneither(_tmp103,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14F))->hd);
Cyc_Absyndump_dump(({const char*_tmp104=")";_tag_dyneither(_tmp104,sizeof(char),2U);}));}else{
# 638
Cyc_Absyndump_dump(_tmp100);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14F))->hd);}
# 641
goto _LL6F;case 2U: _LL72: _LL73:
# 643
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14F))->hd);
Cyc_Absyndump_dump(_tmp100);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14F->tl))->hd);
goto _LL6F;default: _LL74: _LL75:
({void*_tmp105=0U;({struct _dyneither_ptr _tmp32E=({const char*_tmp106="Absyndump -- Bad number of arguments to primop";_tag_dyneither(_tmp106,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp32E,_tag_dyneither(_tmp105,sizeof(void*),0U));});});}_LL6F:;}
# 650
goto _LL0;}case 4U: _LL1D: _tmp153=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp152=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_tmp151=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpF0)->f3;_LL1E:
# 653
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp153);
if(_tmp152 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp152->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp107="=";_tag_dyneither(_tmp107,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp151);
goto _LL0;case 5U: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF0)->f2){case Cyc_Absyn_PreInc: _LL1F: _tmp154=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL20:
# 660
 Cyc_Absyndump_dump(({const char*_tmp108="++";_tag_dyneither(_tmp108,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp154);goto _LL0;case Cyc_Absyn_PreDec: _LL21: _tmp155=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL22:
 Cyc_Absyndump_dump(({const char*_tmp109="--";_tag_dyneither(_tmp109,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp155);goto _LL0;case Cyc_Absyn_PostInc: _LL23: _tmp156=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL24:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp156);Cyc_Absyndump_dump(({const char*_tmp10A="++";_tag_dyneither(_tmp10A,sizeof(char),3U);}));goto _LL0;default: _LL25: _tmp157=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL26:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);Cyc_Absyndump_dump(({const char*_tmp10B="--";_tag_dyneither(_tmp10B,sizeof(char),3U);}));goto _LL0;}case 6U: _LL27: _tmp15A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp159=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_tmp158=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF0)->f3;_LL28:
# 666
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp159);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp158);
goto _LL0;case 7U: _LL29: _tmp15C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp15B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL2A:
# 673
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15C);Cyc_Absyndump_dump(({const char*_tmp10C=" && ";_tag_dyneither(_tmp10C,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15B);goto _LL0;case 8U: _LL2B: _tmp15E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp15D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL2C:
# 677
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15E);Cyc_Absyndump_dump(({const char*_tmp10D=" || ";_tag_dyneither(_tmp10D,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);goto _LL0;case 9U: _LL2D: _tmp160=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp15F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL2E:
# 683
 Cyc_Absyndump_dumploc(e->loc);
# 686
Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp160);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp15F);
goto _LL0;case 10U: _LL2F: _tmp162=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp161=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL30:
# 690
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp162);
Cyc_Absyndump_dump_nospace(({const char*_tmp10E="(";_tag_dyneither(_tmp10E,sizeof(char),2U);}));{
# 696
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == (unsigned int)0)) && !(_tmp162->loc == (unsigned int)0);
Cyc_Absyndump_dumpexps_prec(20,_tmp161);
Cyc_Absyndump_dump_nospace(({const char*_tmp10F=")";_tag_dyneither(_tmp10F,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;};case 11U: _LL31: _tmp163=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL32:
# 705
 Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(({const char*_tmp110="throw";_tag_dyneither(_tmp110,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp163);goto _LL0;case 12U: _LL33: _tmp164=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL34:
# 707
 _tmp165=_tmp164;goto _LL36;case 13U: _LL35: _tmp165=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL36:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp165);goto _LL0;case 14U: _LL37: _tmp167=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp166=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL38:
# 711
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp167);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp166);
goto _LL0;case 15U: _LL39: _tmp168=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL3A:
# 714
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp168);goto _LL0;case 20U: _LL3B: _tmp169=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL3C:
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp169);goto _LL0;case 16U: _LL3D: _tmp16B=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp16A=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL3E:
 Cyc_Absyndump_dump(({const char*_tmp111="new ";_tag_dyneither(_tmp111,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp16A);goto _LL0;case 17U: _LL3F: _tmp16C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL40:
 Cyc_Absyndump_dump(({const char*_tmp112="sizeof(";_tag_dyneither(_tmp112,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(_tmp16C);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 18U: _LL41: _tmp16D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL42:
 Cyc_Absyndump_dump(({const char*_tmp113="sizeof(";_tag_dyneither(_tmp113,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp16D);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 39U: _LL43: _tmp16E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL44:
 Cyc_Absyndump_dump(({const char*_tmp114="valueof(";_tag_dyneither(_tmp114,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp16E);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 41U: _LL45: _tmp16F=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL46:
# 722
 Cyc_Absyndump_dump(({const char*_tmp115="__extension__(";_tag_dyneither(_tmp115,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(_tmp16F);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 40U: _LL47: _tmp171=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp170=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL48:
# 725
 Cyc_Absyndump_dump(({const char*_tmp116="__asm__";_tag_dyneither(_tmp116,sizeof(char),8U);}));
if(_tmp171)Cyc_Absyndump_dump(({const char*_tmp117=" volatile ";_tag_dyneither(_tmp117,sizeof(char),11U);}));
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dump_nospace(_tmp170);Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 38U: _LL49: _tmp173=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp172=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL4A:
# 731
 Cyc_Absyndump_dump(({const char*_tmp118="tagcheck(";_tag_dyneither(_tmp118,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(_tmp173);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp172);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 19U: _LL4B: _tmp175=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp174=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL4C:
# 736
 Cyc_Absyndump_dump(({const char*_tmp119="offsetof(";_tag_dyneither(_tmp119,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(_tmp175);({struct Cyc_List_List*_tmp331=_tmp174;struct _dyneither_ptr _tmp330=({const char*_tmp11A=",";_tag_dyneither(_tmp11A,sizeof(char),2U);});struct _dyneither_ptr _tmp32F=({const char*_tmp11B=")";_tag_dyneither(_tmp11B,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpoffset_field,_tmp331,_tmp330,_tmp32F,({const char*_tmp11C=".";_tag_dyneither(_tmp11C,sizeof(char),2U);}));});goto _LL0;case 21U: _LL4D: _tmp177=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp176=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL4E:
# 739
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp177);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp176);goto _LL0;case 22U: _LL4F: _tmp179=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp178=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL50:
# 741
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp179);Cyc_Absyndump_dump_nospace(({const char*_tmp11D="->";_tag_dyneither(_tmp11D,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*_tmp178);goto _LL0;case 23U: _LL51: _tmp17B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp17A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL52:
# 744
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17B);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp17A);Cyc_Absyndump_dump_char((int)']');goto _LL0;case 24U: _LL53: _tmp17C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL54:
# 746
 Cyc_Absyndump_dump(({const char*_tmp11E="$(";_tag_dyneither(_tmp11E,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp17C);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 25U: _LL55: _tmp17E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp17D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL56:
# 749
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp17E).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp334=_tmp17D;struct _dyneither_ptr _tmp333=({const char*_tmp11F="{";_tag_dyneither(_tmp11F,sizeof(char),2U);});struct _dyneither_ptr _tmp332=({const char*_tmp120="}";_tag_dyneither(_tmp120,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp334,_tmp333,_tmp332,({const char*_tmp121=",";_tag_dyneither(_tmp121,sizeof(char),2U);}));});
goto _LL0;case 26U: _LL57: _tmp17F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL58:
# 755
({struct Cyc_List_List*_tmp337=_tmp17F;struct _dyneither_ptr _tmp336=({const char*_tmp122="{";_tag_dyneither(_tmp122,sizeof(char),2U);});struct _dyneither_ptr _tmp335=({const char*_tmp123="}";_tag_dyneither(_tmp123,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp337,_tmp336,_tmp335,({const char*_tmp124=",";_tag_dyneither(_tmp124,sizeof(char),2U);}));});goto _LL0;case 27U: _LL59: _tmp182=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp181=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_tmp180=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF0)->f3;_LL5A:
# 758
 Cyc_Absyndump_dump(({const char*_tmp125="{for";_tag_dyneither(_tmp125,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*_tmp182->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp181);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp180);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 28U: _LL5B: _tmp184=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp183=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL5C:
# 763
 Cyc_Absyndump_dump(({const char*_tmp126="{for x ";_tag_dyneither(_tmp126,sizeof(char),8U);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp184);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp183);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 29U: _LL5D: _tmp187=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp186=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_tmp185=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF0)->f3;_LL5E:
# 768
 Cyc_Absyndump_dumpqvar(_tmp187);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp186 != 0)
Cyc_Absyndump_dumptps(_tmp186);
({struct Cyc_List_List*_tmp33A=_tmp185;struct _dyneither_ptr _tmp339=({const char*_tmp127="";_tag_dyneither(_tmp127,sizeof(char),1U);});struct _dyneither_ptr _tmp338=({const char*_tmp128="}";_tag_dyneither(_tmp128,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp33A,_tmp339,_tmp338,({const char*_tmp129=",";_tag_dyneither(_tmp129,sizeof(char),2U);}));});
goto _LL0;case 30U: _LL5F: _tmp188=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL60:
# 775
({struct Cyc_List_List*_tmp33D=_tmp188;struct _dyneither_ptr _tmp33C=({const char*_tmp12A="{";_tag_dyneither(_tmp12A,sizeof(char),2U);});struct _dyneither_ptr _tmp33B=({const char*_tmp12B="}";_tag_dyneither(_tmp12B,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp33D,_tmp33C,_tmp33B,({const char*_tmp12C=",";_tag_dyneither(_tmp12C,sizeof(char),2U);}));});goto _LL0;case 32U: _LL61: _tmp189=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL62:
 _tmp18A=_tmp189;goto _LL64;case 33U: _LL63: _tmp18A=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL64:
 Cyc_Absyndump_dumpqvar(_tmp18A->name);goto _LL0;case 31U: _LL65: _tmp18C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp18B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpF0)->f3;_LL66:
# 780
 Cyc_Absyndump_dumpqvar(_tmp18B->name);
if(_tmp18C != 0)({struct Cyc_List_List*_tmp340=_tmp18C;struct _dyneither_ptr _tmp33F=({const char*_tmp12D="(";_tag_dyneither(_tmp12D,sizeof(char),2U);});struct _dyneither_ptr _tmp33E=({const char*_tmp12E=")";_tag_dyneither(_tmp12E,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp340,_tmp33F,_tmp33E,({const char*_tmp12F=",";_tag_dyneither(_tmp12F,sizeof(char),2U);}));});
goto _LL0;case 34U: _LL67: _tmp191=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).is_calloc;_tmp190=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).rgn;_tmp18F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).elt_type;_tmp18E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).num_elts;_tmp18D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpF0)->f1).inline_call;_LL68:
# 785
 Cyc_Absyndump_dumploc(_tmp18E->loc);
if(_tmp191){
if(_tmp190 != 0){
Cyc_Absyndump_dump(({const char*_tmp130="rcalloc(";_tag_dyneither(_tmp130,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(_tmp190);Cyc_Absyndump_dump(({const char*_tmp131=",";_tag_dyneither(_tmp131,sizeof(char),2U);}));}else{
# 791
Cyc_Absyndump_dump(({const char*_tmp132="calloc";_tag_dyneither(_tmp132,sizeof(char),7U);}));}
# 793
Cyc_Absyndump_dumpexp(_tmp18E);
Cyc_Absyndump_dump(({const char*_tmp133=",";_tag_dyneither(_tmp133,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(_tmp18F)),0U));
Cyc_Absyndump_dump(({const char*_tmp134=")";_tag_dyneither(_tmp134,sizeof(char),2U);}));}else{
# 798
if(_tmp190 != 0){
if(_tmp18D)
Cyc_Absyndump_dump(({const char*_tmp135="rmalloc_inline(";_tag_dyneither(_tmp135,sizeof(char),16U);}));else{
# 802
Cyc_Absyndump_dump(({const char*_tmp136="rmalloc(";_tag_dyneither(_tmp136,sizeof(char),9U);}));}
Cyc_Absyndump_dumpexp(_tmp190);Cyc_Absyndump_dump(({const char*_tmp137=",";_tag_dyneither(_tmp137,sizeof(char),2U);}));}else{
# 805
Cyc_Absyndump_dump(({const char*_tmp138="malloc(";_tag_dyneither(_tmp138,sizeof(char),8U);}));}
# 808
if(_tmp18F != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp341=Cyc_Absyn_sizeoftype_exp(*_tmp18F,0U);Cyc_Absyn_times_exp(_tmp341,_tmp18E,0U);}));else{
# 811
Cyc_Absyndump_dumpexp(_tmp18E);}
Cyc_Absyndump_dump(({const char*_tmp139=")";_tag_dyneither(_tmp139,sizeof(char),2U);}));}
# 814
goto _LL0;case 35U: _LL69: _tmp193=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp192=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL6A:
# 817
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp193);Cyc_Absyndump_dump_nospace(({const char*_tmp13A=":=:";_tag_dyneither(_tmp13A,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp192);goto _LL0;case 36U: _LL6B: _tmp195=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_tmp194=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF0)->f2;_LL6C:
# 820
({struct Cyc_List_List*_tmp344=_tmp194;struct _dyneither_ptr _tmp343=({const char*_tmp13B="{";_tag_dyneither(_tmp13B,sizeof(char),2U);});struct _dyneither_ptr _tmp342=({const char*_tmp13C="}";_tag_dyneither(_tmp13C,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp344,_tmp343,_tmp342,({const char*_tmp13D=",";_tag_dyneither(_tmp13D,sizeof(char),2U);}));});goto _LL0;default: _LL6D: _tmp196=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpF0)->f1;_LL6E:
# 823
 Cyc_Absyndump_dump_nospace(({const char*_tmp13E="({";_tag_dyneither(_tmp13E,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp196,1);Cyc_Absyndump_dump_nospace(({const char*_tmp13F="})";_tag_dyneither(_tmp13F,sizeof(char),3U);}));goto _LL0;}_LL0:;}
# 825
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 829
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 833
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp197=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp197->where_clause == 0  && (_tmp197->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp198="default:";_tag_dyneither(_tmp198,sizeof(char),9U);}));else{
# 839
Cyc_Absyndump_dump(({const char*_tmp199="case";_tag_dyneither(_tmp199,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(_tmp197->pattern);
if(_tmp197->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp19A="&&";_tag_dyneither(_tmp19A,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp197->where_clause));}
# 845
Cyc_Absyndump_dump_nospace(({const char*_tmp19B=":";_tag_dyneither(_tmp19B,sizeof(char),2U);}));}
# 847
Cyc_Absyndump_dumpstmt(_tmp197->body,0);}}
# 851
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp19C=s->r;void*_tmp19D=_tmp19C;struct Cyc_List_List*_tmp1D3;struct _dyneither_ptr*_tmp1D2;struct Cyc_Absyn_Stmt*_tmp1D1;struct Cyc_Absyn_Decl*_tmp1D0;struct Cyc_Absyn_Stmt*_tmp1CF;struct Cyc_Absyn_Stmt*_tmp1CE;struct Cyc_List_List*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct _dyneither_ptr*_tmp1CA;struct Cyc_Absyn_Stmt*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Absyn_Stmt*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Stmt*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Stmt*_tmp1C0;struct Cyc_Absyn_Stmt*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Stmt*_tmp1BD;struct Cyc_Absyn_Stmt*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BB;switch(*((int*)_tmp19D)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1U: _LL3: _tmp1BB=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_LL4:
 Cyc_Absyndump_dumpexp_prec(- 100,_tmp1BB);Cyc_Absyndump_dump_semi();goto _LL0;case 2U: _LL5: _tmp1BD=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1BC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2;_LL6:
# 860
 if(Cyc_Absynpp_is_declaration(_tmp1BD)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BD,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 863
Cyc_Absyndump_dumpstmt(_tmp1BD,0);}
if(Cyc_Absynpp_is_declaration(_tmp1BC)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BC,expstmt);Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 869
Cyc_Absyndump_dumpstmt(_tmp1BC,expstmt);}
goto _LL0;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1 == 0){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp19E="return;";_tag_dyneither(_tmp19E,sizeof(char),8U);}));goto _LL0;}else{_LL9: _tmp1BE=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_LLA:
 Cyc_Absyndump_dump(({const char*_tmp19F="return";_tag_dyneither(_tmp19F,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1BE));Cyc_Absyndump_dump_semi();goto _LL0;}case 4U: _LLB: _tmp1C1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1C0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2;_tmp1BF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19D)->f3;_LLC:
# 874
 Cyc_Absyndump_dump(({const char*_tmp1A0="if(";_tag_dyneither(_tmp1A0,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(_tmp1C1);
{void*_tmp1A1=_tmp1C0->r;void*_tmp1A2=_tmp1A1;switch(*((int*)_tmp1A2)){case 2U: _LL26: _LL27:
 goto _LL29;case 12U: _LL28: _LL29:
 goto _LL2B;case 4U: _LL2A: _LL2B:
 goto _LL2D;case 13U: _LL2C: _LL2D:
# 880
 Cyc_Absyndump_dump_nospace(({const char*_tmp1A3="){";_tag_dyneither(_tmp1A3,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1C0,0);Cyc_Absyndump_dump_char((int)'}');goto _LL25;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1C0,0);}_LL25:;}
# 883
{void*_tmp1A4=_tmp1BF->r;void*_tmp1A5=_tmp1A4;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1A5)->tag == 0U){_LL31: _LL32:
 goto _LL30;}else{_LL33: _LL34:
 Cyc_Absyndump_dump(({const char*_tmp1A6="else{";_tag_dyneither(_tmp1A6,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(_tmp1BF,0);Cyc_Absyndump_dump_char((int)'}');goto _LL30;}_LL30:;}
# 887
goto _LL0;case 5U: _LLD: _tmp1C3=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1).f1;_tmp1C2=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2;_LLE:
# 890
 Cyc_Absyndump_dump(({const char*_tmp1A7="while(";_tag_dyneither(_tmp1A7,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(_tmp1C3);Cyc_Absyndump_dump_nospace(({const char*_tmp1A8="){";_tag_dyneither(_tmp1A8,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(_tmp1C2,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 9U: _LLF: _tmp1C7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1C6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2).f1;_tmp1C5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19D)->f3).f1;_tmp1C4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19D)->f4;_LL10:
# 894
 Cyc_Absyndump_dump(({const char*_tmp1A9="for(";_tag_dyneither(_tmp1A9,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(_tmp1C7);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C6);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C5);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AA="){";_tag_dyneither(_tmp1AA,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1C4,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 14U: _LL11: _tmp1C9=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1C8=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2).f1;_LL12:
# 898
 Cyc_Absyndump_dump(({const char*_tmp1AB="do{";_tag_dyneither(_tmp1AB,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1C9,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AC="}while(";_tag_dyneither(_tmp1AC,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1C8);Cyc_Absyndump_dump_nospace(({const char*_tmp1AD=");";_tag_dyneither(_tmp1AD,sizeof(char),3U);}));
goto _LL0;case 6U: _LL13: _LL14:
# 902
 Cyc_Absyndump_dump(({const char*_tmp1AE="break;";_tag_dyneither(_tmp1AE,sizeof(char),7U);}));goto _LL0;case 7U: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp1AF="continue;";_tag_dyneither(_tmp1AF,sizeof(char),10U);}));goto _LL0;case 8U: _LL17: _tmp1CA=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_LL18:
 Cyc_Absyndump_dump(({const char*_tmp1B0="goto";_tag_dyneither(_tmp1B0,sizeof(char),5U);}));Cyc_Absyndump_dump_str(_tmp1CA);Cyc_Absyndump_dump_semi();goto _LL0;case 10U: _LL19: _tmp1CC=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1CB=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2;_LL1A:
# 907
 Cyc_Absyndump_dump(({const char*_tmp1B1="switch(";_tag_dyneither(_tmp1B1,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1CC);Cyc_Absyndump_dump_nospace(({const char*_tmp1B2="){";_tag_dyneither(_tmp1B2,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1CB);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 15U: _LL1B: _tmp1CE=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1CD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2;_LL1C:
# 912
 Cyc_Absyndump_dump(({const char*_tmp1B3="try";_tag_dyneither(_tmp1B3,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1CE,0);
Cyc_Absyndump_dump(({const char*_tmp1B4="catch{";_tag_dyneither(_tmp1B4,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1CD);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 12U: _LL1D: _tmp1D0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1CF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2;_LL1E:
# 917
 Cyc_Absyndump_dumpdecl(_tmp1D0);Cyc_Absyndump_dumpstmt(_tmp1CF,expstmt);goto _LL0;case 13U: _LL1F: _tmp1D2=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_tmp1D1=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19D)->f2;_LL20:
# 923
 if(Cyc_Absynpp_is_declaration(_tmp1D1)){
Cyc_Absyndump_dump_str(_tmp1D2);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1B5=": ({";_tag_dyneither(_tmp1B5,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1B6=": {";_tag_dyneither(_tmp1B6,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(_tmp1D1,expstmt);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1B7="});}";_tag_dyneither(_tmp1B7,sizeof(char),5U);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 929
Cyc_Absyndump_dump_str(_tmp1D2);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1D1,expstmt);}
# 931
goto _LL0;default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1 == 0){_LL21: _LL22:
# 933
 Cyc_Absyndump_dump(({const char*_tmp1B8="fallthru;";_tag_dyneither(_tmp1B8,sizeof(char),10U);}));goto _LL0;}else{_LL23: _tmp1D3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19D)->f1;_LL24:
# 935
 Cyc_Absyndump_dump(({const char*_tmp1B9="fallthru(";_tag_dyneither(_tmp1B9,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp1D3);Cyc_Absyndump_dump_nospace(({const char*_tmp1BA=");";_tag_dyneither(_tmp1BA,sizeof(char),3U);}));goto _LL0;}}_LL0:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 939
static void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
({struct Cyc_List_List*_tmp347=(*dp).f1;struct _dyneither_ptr _tmp346=({const char*_tmp1D4="";_tag_dyneither(_tmp1D4,sizeof(char),1U);});struct _dyneither_ptr _tmp345=({const char*_tmp1D5="=";_tag_dyneither(_tmp1D5,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp347,_tmp346,_tmp345,({const char*_tmp1D6="=";_tag_dyneither(_tmp1D6,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 943
static struct _dyneither_ptr Cyc_Absyndump_pat_term(int dots){return dots?({const char*_tmp1D7="...)";_tag_dyneither(_tmp1D7,sizeof(char),5U);}):({const char*_tmp1D8=")";_tag_dyneither(_tmp1D8,sizeof(char),2U);});}
# 945
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1D9=p->r;void*_tmp1DA=_tmp1D9;struct Cyc_Absyn_Exp*_tmp218;struct Cyc_Absyn_Datatypefield*_tmp217;struct Cyc_List_List*_tmp216;int _tmp215;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp213;int _tmp212;union Cyc_Absyn_AggrInfo _tmp211;struct Cyc_List_List*_tmp210;struct Cyc_List_List*_tmp20F;int _tmp20E;struct _tuple0*_tmp20D;struct Cyc_List_List*_tmp20C;int _tmp20B;struct _tuple0*_tmp20A;struct Cyc_Absyn_Tvar*_tmp209;struct Cyc_Absyn_Vardecl*_tmp208;struct Cyc_Absyn_Pat*_tmp207;struct Cyc_List_List*_tmp206;int _tmp205;struct Cyc_Absyn_Tvar*_tmp204;struct Cyc_Absyn_Vardecl*_tmp203;struct Cyc_Absyn_Vardecl*_tmp202;struct Cyc_Absyn_Pat*_tmp201;struct Cyc_Absyn_Vardecl*_tmp200;struct Cyc_Absyn_Vardecl*_tmp1FF;struct Cyc_Absyn_Pat*_tmp1FE;struct Cyc_Absyn_Vardecl*_tmp1FD;char _tmp1FC;struct Cyc_Absyn_Enumfield*_tmp1FB;struct Cyc_Absyn_Enumfield*_tmp1FA;struct _dyneither_ptr _tmp1F9;int _tmp1F8;int _tmp1F7;int _tmp1F6;switch(*((int*)_tmp1DA)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_char((int)'_');goto _LL0;case 9U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp1DB="NULL";_tag_dyneither(_tmp1DB,sizeof(char),5U);}));goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1){case Cyc_Absyn_None: _LL5: _tmp1F6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL6:
 _tmp1F7=_tmp1F6;goto _LL8;case Cyc_Absyn_Signed: _LL7: _tmp1F7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL8:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1DE=({struct Cyc_Int_pa_PrintArg_struct _tmp2F6;_tmp2F6.tag=1U,_tmp2F6.f1=(unsigned long)_tmp1F7;_tmp2F6;});void*_tmp1DC[1U];_tmp1DC[0]=& _tmp1DE;({struct _dyneither_ptr _tmp348=({const char*_tmp1DD="%d";_tag_dyneither(_tmp1DD,sizeof(char),3U);});Cyc_aprintf(_tmp348,_tag_dyneither(_tmp1DC,sizeof(void*),1U));});}));goto _LL0;default: _LL9: _tmp1F8=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1E1=({struct Cyc_Int_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=1U,_tmp2F7.f1=(unsigned int)_tmp1F8;_tmp2F7;});void*_tmp1DF[1U];_tmp1DF[0]=& _tmp1E1;({struct _dyneither_ptr _tmp349=({const char*_tmp1E0="%u";_tag_dyneither(_tmp1E0,sizeof(char),3U);});Cyc_aprintf(_tmp349,_tag_dyneither(_tmp1DF,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LLB: _tmp1F9=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LLC:
 Cyc_Absyndump_dump(_tmp1F9);goto _LL0;case 13U: _LLD: _tmp1FA=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LLE:
 _tmp1FB=_tmp1FA;goto _LL10;case 14U: _LLF: _tmp1FB=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL10:
 Cyc_Absyndump_dumpqvar(_tmp1FB->name);goto _LL0;case 11U: _LL11: _tmp1FC=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL12:
# 956
 Cyc_Absyndump_dump(({const char*_tmp1E2="'";_tag_dyneither(_tmp1E2,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1FC));Cyc_Absyndump_dump_nospace(({const char*_tmp1E3="'";_tag_dyneither(_tmp1E3,sizeof(char),2U);}));goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2)->r)->tag == 0U){_LL13: _tmp1FD=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL14:
 Cyc_Absyndump_dump(({const char*_tmp1E4="*";_tag_dyneither(_tmp1E4,sizeof(char),2U);}));_tmp200=_tmp1FD;goto _LL16;}else{_LL17: _tmp1FF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp1FE=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL18:
# 959
 Cyc_Absyndump_dump(({const char*_tmp1E5="*";_tag_dyneither(_tmp1E5,sizeof(char),2U);}));_tmp202=_tmp1FF;_tmp201=_tmp1FE;goto _LL1A;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2)->r)->tag == 0U){_LL15: _tmp200=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL16:
# 958
 Cyc_Absyndump_dumpqvar(_tmp200->name);goto _LL0;}else{_LL19: _tmp202=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp201=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1A:
# 960
 Cyc_Absyndump_dumpqvar(_tmp202->name);Cyc_Absyndump_dump(({const char*_tmp1E6=" as ";_tag_dyneither(_tmp1E6,sizeof(char),5U);}));Cyc_Absyndump_dumppat(_tmp201);goto _LL0;}case 2U: _LL1B: _tmp204=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp203=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1C:
# 962
 Cyc_Absyndump_dump(({const char*_tmp1E7="alias";_tag_dyneither(_tmp1E7,sizeof(char),6U);}));Cyc_Absyndump_dump(({const char*_tmp1E8="<";_tag_dyneither(_tmp1E8,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp204);Cyc_Absyndump_dump(({const char*_tmp1E9=">";_tag_dyneither(_tmp1E9,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(_tmp203,p->loc,0);
goto _LL0;case 5U: _LL1D: _tmp206=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp205=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL1E:
({struct Cyc_List_List*_tmp34C=_tmp206;struct _dyneither_ptr _tmp34B=({const char*_tmp1EA="$(";_tag_dyneither(_tmp1EA,sizeof(char),3U);});struct _dyneither_ptr _tmp34A=Cyc_Absyndump_pat_term(_tmp205);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp34C,_tmp34B,_tmp34A,({const char*_tmp1EB=",";_tag_dyneither(_tmp1EB,sizeof(char),2U);}));});goto _LL0;case 6U: _LL1F: _tmp207=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL20:
 Cyc_Absyndump_dump(({const char*_tmp1EC="&";_tag_dyneither(_tmp1EC,sizeof(char),2U);}));Cyc_Absyndump_dumppat(_tmp207);goto _LL0;case 4U: _LL21: _tmp209=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp208=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_LL22:
# 968
 Cyc_Absyndump_dumpqvar(_tmp208->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp209);Cyc_Absyndump_dump_char((int)'>');goto _LL0;case 15U: _LL23: _tmp20A=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL24:
 Cyc_Absyndump_dumpqvar(_tmp20A);goto _LL0;case 16U: _LL25: _tmp20D=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp20C=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp20B=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_LL26:
# 971
 Cyc_Absyndump_dumpqvar(_tmp20D);({struct Cyc_List_List*_tmp34F=_tmp20C;struct _dyneither_ptr _tmp34E=({const char*_tmp1ED="(";_tag_dyneither(_tmp1ED,sizeof(char),2U);});struct _dyneither_ptr _tmp34D=Cyc_Absyndump_pat_term(_tmp20B);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp34F,_tmp34E,_tmp34D,({const char*_tmp1EE=",";_tag_dyneither(_tmp1EE,sizeof(char),2U);}));});goto _LL0;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1 != 0){_LL27: _tmp211=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_tmp210=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp20F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp20E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL28:
# 973
 Cyc_Absyndump_dumpqvar((Cyc_Absyn_aggr_kinded_name(_tmp211)).f2);
_tmp214=_tmp210;_tmp213=_tmp20F;_tmp212=_tmp20E;goto _LL2A;}else{_LL29: _tmp214=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp213=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp212=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL2A:
# 976
 Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp352=_tmp214;struct _dyneither_ptr _tmp351=({const char*_tmp1EF="<";_tag_dyneither(_tmp1EF,sizeof(char),2U);});struct _dyneither_ptr _tmp350=({const char*_tmp1F0=">";_tag_dyneither(_tmp1F0,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp352,_tmp351,_tmp350,({const char*_tmp1F1=",";_tag_dyneither(_tmp1F1,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp355=_tmp213;struct _dyneither_ptr _tmp354=({const char*_tmp1F2="";_tag_dyneither(_tmp1F2,sizeof(char),1U);});struct _dyneither_ptr _tmp353=Cyc_Absyndump_pat_term(_tmp212);((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp355,_tmp354,_tmp353,({const char*_tmp1F3=",";_tag_dyneither(_tmp1F3,sizeof(char),2U);}));});
goto _LL0;}case 8U: _LL2B: _tmp217=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f2;_tmp216=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f3;_tmp215=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1DA)->f4;_LL2C:
# 981
 Cyc_Absyndump_dumpqvar(_tmp217->name);
if(_tmp216 != 0)({struct Cyc_List_List*_tmp358=_tmp216;struct _dyneither_ptr _tmp357=({const char*_tmp1F4="(";_tag_dyneither(_tmp1F4,sizeof(char),2U);});struct _dyneither_ptr _tmp356=Cyc_Absyndump_pat_term(_tmp215);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp358,_tmp357,_tmp356,({const char*_tmp1F5=",";_tag_dyneither(_tmp1F5,sizeof(char),2U);}));});
goto _LL0;default: _LL2D: _tmp218=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1DA)->f1;_LL2E:
 Cyc_Absyndump_dumpexp(_tmp218);goto _LL0;}_LL0:;}
# 988
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 993
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp359=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp359,({const char*_tmp219=",";_tag_dyneither(_tmp219,sizeof(char),2U);}));});}
# 996
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp21A="=";_tag_dyneither(_tmp21A,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1003
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp35A=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp35A,({const char*_tmp21B=",";_tag_dyneither(_tmp21B,sizeof(char),2U);}));});}
# 1007
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp21C=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp21D=_tmp21C;struct _dyneither_ptr*_tmp225;struct Cyc_Absyn_Tqual _tmp224;void*_tmp223;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_List_List*_tmp221;struct Cyc_Absyn_Exp*_tmp220;_LL1: _tmp225=_tmp21D->name;_tmp224=_tmp21D->tq;_tmp223=_tmp21D->type;_tmp222=_tmp21D->width;_tmp221=_tmp21D->attributes;_tmp220=_tmp21D->requires_clause;_LL2:;
{enum Cyc_Cyclone_C_Compilers _tmp21E=Cyc_Cyclone_c_compiler;if(_tmp21E == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp224,_tmp223,Cyc_Absyndump_dump_str,_tmp225);Cyc_Absyndump_dumpatts(_tmp221);goto _LL3;}else{_LL6: _LL7:
 Cyc_Absyndump_dumpatts(_tmp221);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp224,_tmp223,Cyc_Absyndump_dump_str,_tmp225);goto _LL3;}_LL3:;}
# 1014
if((unsigned int)_tmp220){
Cyc_Absyndump_dump(({const char*_tmp21F="@requires ";_tag_dyneither(_tmp21F,sizeof(char),11U);}));
Cyc_Absyndump_dumpexp(_tmp220);}
# 1018
if(_tmp222 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp222);}
# 1022
Cyc_Absyndump_dump_semi();}}
# 1026
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1031
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1035
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp226=(*pr).f1;void*_tmp227=_tmp226;switch(*((int*)_tmp227)){case 11U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp228="_stdcall";_tag_dyneither(_tmp228,sizeof(char),9U);}));goto _LL0;case 2U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp229="_cdecl";_tag_dyneither(_tmp229,sizeof(char),7U);}));goto _LL0;case 3U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp22A="_fastcall";_tag_dyneither(_tmp22A,sizeof(char),10U);}));goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 1043
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1045
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1049
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 1053
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1055
void*_tmp22B=t;void*_tmp230;void*_tmp22F;switch(*((int*)_tmp22B)){case 1U: _LL1: _tmp22F=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22B)->f2;if(_tmp22F != 0){_LL2:
 return Cyc_Absyndump_is_char_ptr(_tmp22F);}else{goto _LL5;}case 3U: _LL3: _tmp230=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_type;_LL4:
# 1058
 while(1){
void*_tmp22C=_tmp230;void*_tmp22E;void*_tmp22D;switch(*((int*)_tmp22C)){case 1U: _LL8: _tmp22D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22C)->f2;if(_tmp22D != 0){_LL9:
 _tmp230=_tmp22D;goto _LL7;}else{goto _LLE;}case 8U: _LLA: _tmp22E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp22C)->f4;if(_tmp22E != 0){_LLB:
 _tmp230=_tmp22E;goto _LL7;}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22C)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22C)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 1065
 return 0;}_LL0:;}
# 1069
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp231=vd;enum Cyc_Absyn_Scope _tmp247;struct _tuple0*_tmp246;unsigned int _tmp245;struct Cyc_Absyn_Tqual _tmp244;void*_tmp243;struct Cyc_Absyn_Exp*_tmp242;struct Cyc_List_List*_tmp241;_LL1: _tmp247=_tmp231->sc;_tmp246=_tmp231->name;_tmp245=_tmp231->varloc;_tmp244=_tmp231->tq;_tmp243=_tmp231->type;_tmp242=_tmp231->initializer;_tmp241=_tmp231->attributes;_LL2:;
{enum Cyc_Cyclone_C_Compilers _tmp232=Cyc_Cyclone_c_compiler;if(_tmp232 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1074
 if((int)_tmp247 == (int)3U  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp233=Cyc_Tcutil_compress(_tmp243);void*_tmp234=_tmp233;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp234)->tag == 5U){_LL9: _LLA:
 goto _LL8;}else{_LLB: _LLC:
 Cyc_Absyndump_dumpscope(_tmp247);}_LL8:;}else{
# 1080
Cyc_Absyndump_dumpscope(_tmp247);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp244,_tmp243,Cyc_Absyndump_dumpqvar,_tmp246);
Cyc_Absyndump_dumpatts(_tmp241);
goto _LL3;}else{_LL6: _LL7:
# 1085
 Cyc_Absyndump_dumpatts(_tmp241);
Cyc_Absyndump_dumpscope(_tmp247);{
struct _RegionHandle _tmp235=_new_region("temp");struct _RegionHandle*temp=& _tmp235;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(_tmp243);
struct _tuple11 _tmp236=Cyc_Absynpp_to_tms(temp,_tmp244,_tmp243);struct _tuple11 _tmp237=_tmp236;struct Cyc_Absyn_Tqual _tmp240;void*_tmp23F;struct Cyc_List_List*_tmp23E;_LLE: _tmp240=_tmp237.f1;_tmp23F=_tmp237.f2;_tmp23E=_tmp237.f3;_LLF:;{
# 1091
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp23E;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp238=(void*)tms2->hd;void*_tmp239=_tmp238;struct Cyc_List_List*_tmp23C;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp239)->tag == 5U){_LL11: _tmp23C=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp239)->f2;_LL12:
# 1095
 for(0;_tmp23C != 0;_tmp23C=_tmp23C->tl){
void*_tmp23A=(void*)_tmp23C->hd;void*_tmp23B=_tmp23A;switch(*((int*)_tmp23B)){case 1U: _LL16: _LL17:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL15;case 2U: _LL18: _LL19:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL15;case 3U: _LL1A: _LL1B:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL15;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 1102
goto _LL10;}else{_LL13: _LL14:
 goto _LL10;}_LL10:;}}
# 1105
Cyc_Absyndump_dumptq(_tmp240);
Cyc_Absyndump_dumpntyp(_tmp23F);{
struct _tuple19 _tmp23D=({struct _tuple19 _tmp2F8;_tmp2F8.f1=call_conv,_tmp2F8.f2=_tmp246;_tmp2F8;});
({int _tmp35B=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(_tmp35B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp23E),Cyc_Absyndump_dump_callconv_qvar,& _tmp23D);});
_npop_handler(0U);goto _LL3;};};}
# 1088
;_pop_region(temp);};}_LL3:;}
# 1112
if(_tmp242 != 0){
Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp242);}
# 1115
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1118
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp248="@tagged ";_tag_dyneither(_tmp248,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl != 0){
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp35E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _dyneither_ptr _tmp35D=({const char*_tmp249="<";_tag_dyneither(_tmp249,sizeof(char),2U);});struct _dyneither_ptr _tmp35C=({const char*_tmp24A=">";_tag_dyneither(_tmp24A,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp35E,_tmp35D,_tmp35C,({const char*_tmp24B=",";_tag_dyneither(_tmp24B,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1132
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump(({const char*_tmp24C="}";_tag_dyneither(_tmp24C,sizeof(char),2U);}));
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1137
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp24D=ed;enum Cyc_Absyn_Scope _tmp252;struct _tuple0*_tmp251;struct Cyc_Core_Opt*_tmp250;_LL1: _tmp252=_tmp24D->sc;_tmp251=_tmp24D->name;_tmp250=_tmp24D->fields;_LL2:;
Cyc_Absyndump_dumpscope(_tmp252);
Cyc_Absyndump_dump(({const char*_tmp24E="enum ";_tag_dyneither(_tmp24E,sizeof(char),6U);}));
Cyc_Absyndump_dumpqvar(_tmp251);
if(_tmp250 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp250->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp24F="}";_tag_dyneither(_tmp24F,sizeof(char),2U);}));}}
# 1148
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp253=dd;enum Cyc_Absyn_Scope _tmp25B;struct _tuple0*_tmp25A;struct Cyc_List_List*_tmp259;struct Cyc_Core_Opt*_tmp258;int _tmp257;_LL1: _tmp25B=_tmp253->sc;_tmp25A=_tmp253->name;_tmp259=_tmp253->tvs;_tmp258=_tmp253->fields;_tmp257=_tmp253->is_extensible;_LL2:;
Cyc_Absyndump_dumpscope(_tmp25B);
if(_tmp257)Cyc_Absyndump_dump(({const char*_tmp254="@extensible ";_tag_dyneither(_tmp254,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp255="datatype ";_tag_dyneither(_tmp255,sizeof(char),10U);}));
Cyc_Absyndump_dumpqvar(_tmp25A);
Cyc_Absyndump_dumptvars(_tmp259);
if(_tmp258 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp258->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp256="}";_tag_dyneither(_tmp256,sizeof(char),2U);}));}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1162
static void Cyc_Absyndump_dumpexport(struct _tuple20*tup){
Cyc_Absyndump_dumpqvar((*tup).f2);}
# 1165
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char((int)'{');
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char((int)'}');}
# 1172
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp25C=d->r;void*_tmp25D=_tmp25C;struct Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp291;struct Cyc_List_List*_tmp290;struct Cyc_List_List*_tmp28F;struct _tuple0*_tmp28E;struct Cyc_List_List*_tmp28D;struct _dyneither_ptr*_tmp28C;struct Cyc_List_List*_tmp28B;struct Cyc_Absyn_Typedefdecl*_tmp28A;struct Cyc_Absyn_Tvar*_tmp289;struct Cyc_Absyn_Vardecl*_tmp288;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Pat*_tmp286;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_List_List*_tmp284;struct Cyc_Absyn_Enumdecl*_tmp283;struct Cyc_Absyn_Datatypedecl*_tmp282;struct Cyc_Absyn_Aggrdecl*_tmp281;struct Cyc_Absyn_Vardecl*_tmp280;struct Cyc_Absyn_Fndecl*_tmp27F;switch(*((int*)_tmp25D)){case 1U: _LL1: _tmp27F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LL2:
# 1176
{enum Cyc_Cyclone_C_Compilers _tmp25E=Cyc_Cyclone_c_compiler;if(_tmp25E == Cyc_Cyclone_Vc_c){_LL24: _LL25:
 Cyc_Absyndump_dumpatts((_tmp27F->i).attributes);goto _LL23;}else{_LL26: _LL27:
 goto _LL23;}_LL23:;}
# 1180
if(_tmp27F->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp25F=Cyc_Cyclone_c_compiler;if(_tmp25F == Cyc_Cyclone_Vc_c){_LL29: _LL2A:
 Cyc_Absyndump_dump(({const char*_tmp260="__inline";_tag_dyneither(_tmp260,sizeof(char),9U);}));goto _LL28;}else{_LL2B: _LL2C:
 Cyc_Absyndump_dump(({const char*_tmp261="inline";_tag_dyneither(_tmp261,sizeof(char),7U);}));goto _LL28;}_LL28:;}
# 1185
Cyc_Absyndump_dumpscope(_tmp27F->sc);{
# 1187
struct Cyc_Absyn_FnInfo type_info=_tmp27F->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->tag=5U,_tmp268->f1=type_info;_tmp268;});
if(_tmp27F->cached_type != 0){
void*_tmp262=Cyc_Tcutil_compress((void*)_check_null(_tmp27F->cached_type));void*_tmp263=_tmp262;struct Cyc_Absyn_FnInfo _tmp266;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp263)->tag == 5U){_LL2E: _tmp266=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp263)->f1;_LL2F:
# 1193
({struct Cyc_List_List*_tmp35F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((_tmp27F->i).attributes,_tmp266.attributes);(_tmp27F->i).attributes=_tmp35F;});goto _LL2D;}else{_LL30: _LL31:
({void*_tmp264=0U;({struct _dyneither_ptr _tmp360=({const char*_tmp265="function has non-function type";_tag_dyneither(_tmp265,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp360,_tag_dyneither(_tmp264,sizeof(void*),0U));});});}_LL2D:;}
# 1197
{enum Cyc_Cyclone_C_Compilers _tmp267=Cyc_Cyclone_c_compiler;if(_tmp267 == Cyc_Cyclone_Gcc_c){_LL33: _LL34:
({struct Cyc_Absyn_Tqual _tmp362=Cyc_Absyn_empty_tqual(0U);void*_tmp361=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp362,_tmp361,Cyc_Absyndump_dump_atts_qvar,_tmp27F);});goto _LL32;}else{_LL35: _LL36:
({struct Cyc_Absyn_Tqual _tmp364=Cyc_Absyn_empty_tqual(0U);void*_tmp363=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp364,_tmp363,Cyc_Absyndump_dump_callconv_fdqvar,_tmp27F);});goto _LL32;}_LL32:;}
# 1201
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp27F->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;};};case 0U: _LL3: _tmp280=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LL4:
 Cyc_Absyndump_dumpvardecl(_tmp280,d->loc,1);goto _LL0;case 5U: _LL5: _tmp281=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LL6:
 Cyc_Absyndump_dump_aggrdecl(_tmp281);Cyc_Absyndump_dump_semi();goto _LL0;case 6U: _LL7: _tmp282=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LL8:
 Cyc_Absyndump_dump_datatypedecl(_tmp282);Cyc_Absyndump_dump_semi();goto _LL0;case 7U: _LL9: _tmp283=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LLA:
 Cyc_Absyndump_dump_enumdecl(_tmp283);Cyc_Absyndump_dump_semi();goto _LL0;case 3U: _LLB: _tmp284=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LLC:
({struct Cyc_List_List*_tmp367=_tmp284;struct _dyneither_ptr _tmp366=({const char*_tmp269=" let ";_tag_dyneither(_tmp269,sizeof(char),6U);});struct _dyneither_ptr _tmp365=({const char*_tmp26A=";";_tag_dyneither(_tmp26A,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpid,_tmp367,_tmp366,_tmp365,({const char*_tmp26B=",";_tag_dyneither(_tmp26B,sizeof(char),2U);}));});goto _LL0;case 2U: _LLD: _tmp286=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_tmp285=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp25D)->f3;_LLE:
# 1211
 Cyc_Absyndump_dump(({const char*_tmp26C="let";_tag_dyneither(_tmp26C,sizeof(char),4U);}));Cyc_Absyndump_dumppat(_tmp286);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp285);Cyc_Absyndump_dump_semi();goto _LL0;case 4U: _LLF: _tmp289=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_tmp288=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp25D)->f2;_tmp287=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp25D)->f3;_LL10:
# 1213
 Cyc_Absyndump_dump(({const char*_tmp26D="region";_tag_dyneither(_tmp26D,sizeof(char),7U);}));Cyc_Absyndump_dump(({const char*_tmp26E="<";_tag_dyneither(_tmp26E,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp289);Cyc_Absyndump_dump(({const char*_tmp26F="> ";_tag_dyneither(_tmp26F,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(_tmp288->name);
if((unsigned int)_tmp287){
Cyc_Absyndump_dump(({const char*_tmp270=" = open(";_tag_dyneither(_tmp270,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(_tmp287);Cyc_Absyndump_dump(({const char*_tmp271=")";_tag_dyneither(_tmp271,sizeof(char),2U);}));}
# 1217
Cyc_Absyndump_dump_semi();
goto _LL0;case 8U: _LL11: _tmp28A=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LL12:
# 1224
 Cyc_Absyndump_dump(({const char*_tmp272="typedef";_tag_dyneither(_tmp272,sizeof(char),8U);}));{
void*t=_tmp28A->defn == 0?Cyc_Absyn_new_evar(_tmp28A->kind,0):(void*)_check_null(_tmp28A->defn);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp28A->tq,t,Cyc_Absyndump_dumptypedefname,_tmp28A);
Cyc_Absyndump_dumpatts(_tmp28A->atts);
Cyc_Absyndump_dump_semi();
# 1230
goto _LL0;};case 9U: _LL13: _tmp28C=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_tmp28B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp25D)->f2;_LL14:
# 1232
 Cyc_Absyndump_dump(({const char*_tmp273="namespace";_tag_dyneither(_tmp273,sizeof(char),10U);}));Cyc_Absyndump_dump_str(_tmp28C);Cyc_Absyndump_dump_braced_decls(_tmp28B);goto _LL0;case 10U: _LL15: _tmp28E=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_tmp28D=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp25D)->f2;_LL16:
# 1234
 Cyc_Absyndump_dump(({const char*_tmp274="using";_tag_dyneither(_tmp274,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(_tmp28E);Cyc_Absyndump_dump_braced_decls(_tmp28D);goto _LL0;case 11U: _LL17: _tmp28F=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_LL18:
# 1236
 Cyc_Absyndump_dump(({const char*_tmp275="extern \"C\" ";_tag_dyneither(_tmp275,sizeof(char),12U);}));Cyc_Absyndump_dump_braced_decls(_tmp28F);goto _LL0;case 12U: _LL19: _tmp292=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp25D)->f1;_tmp291=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp25D)->f2;_tmp290=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp25D)->f3;_LL1A:
# 1238
 Cyc_Absyndump_dump(({const char*_tmp276="extern \"C include\" ";_tag_dyneither(_tmp276,sizeof(char),20U);}));Cyc_Absyndump_dump_braced_decls(_tmp292);
if(_tmp291 != 0){
Cyc_Absyndump_dump(({const char*_tmp277=" cyclone_override ";_tag_dyneither(_tmp277,sizeof(char),19U);}));
Cyc_Absyndump_dump_braced_decls(_tmp291);}
# 1243
({struct Cyc_List_List*_tmp36A=_tmp290;struct _dyneither_ptr _tmp369=({const char*_tmp278=" export {";_tag_dyneither(_tmp278,sizeof(char),10U);});struct _dyneither_ptr _tmp368=({const char*_tmp279="}";_tag_dyneither(_tmp279,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple20*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpexport,_tmp36A,_tmp369,_tmp368,({const char*_tmp27A=",";_tag_dyneither(_tmp27A,sizeof(char),2U);}));});
goto _LL0;case 13U: _LL1B: _LL1C:
 Cyc_Absyndump_dump(({const char*_tmp27B=" __cyclone_port_on__; ";_tag_dyneither(_tmp27B,sizeof(char),23U);}));goto _LL0;case 14U: _LL1D: _LL1E:
 Cyc_Absyndump_dump(({const char*_tmp27C=" __cyclone_port_off__; ";_tag_dyneither(_tmp27C,sizeof(char),24U);}));goto _LL0;case 15U: _LL1F: _LL20:
 Cyc_Absyndump_dump(({const char*_tmp27D=" __tempest_on__; ";_tag_dyneither(_tmp27D,sizeof(char),18U);}));goto _LL0;default: _LL21: _LL22:
 Cyc_Absyndump_dump(({const char*_tmp27E=" __tempest_off__; ";_tag_dyneither(_tmp27E,sizeof(char),19U);}));goto _LL0;}_LL0:;};}
# 1252
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != (unsigned int)1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1259
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1261
if(tms == 0){
f(a);
return;}{
# 1265
void*_tmp293=(void*)tms->hd;void*_tmp294=_tmp293;void*_tmp2C4;void*_tmp2C3;void*_tmp2C2;void*_tmp2C1;struct Cyc_Absyn_Tqual _tmp2C0;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp294)->tag == 2U){_LL1: _tmp2C4=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp294)->f1).rgn;_tmp2C3=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp294)->f1).nullable;_tmp2C2=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp294)->f1).bounds;_tmp2C1=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp294)->f1).zero_term;_tmp2C0=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp294)->f2;_LL2:
# 1269
{void*_tmp295=Cyc_Tcutil_compress(_tmp2C2);void*_tmp296=_tmp295;struct Cyc_Absyn_Exp*_tmp297;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296)->f1)){case 14U: _LL6: _LL7:
 Cyc_Absyndump_dump_char((int)'?');goto _LL5;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296)->f2)->hd)->tag == 9U){_LL8: _tmp297=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296)->f2)->hd)->f1;_LL9:
# 1272
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,_tmp2C3)?'*':'@'));Cyc_Absyndump_dump_upperbound(_tmp297);goto _LL5;}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA: _LLB:
# 1274
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,_tmp2C3)?'*':'@'));}_LL5:;}
# 1276
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && Cyc_Absyn_type2bool(0,_tmp2C1))Cyc_Absyndump_dump(({const char*_tmp298="@zeroterm";_tag_dyneither(_tmp298,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !Cyc_Absyn_type2bool(0,_tmp2C1))Cyc_Absyndump_dump(({const char*_tmp299="@nozeroterm";_tag_dyneither(_tmp299,sizeof(char),12U);}));
{void*_tmp29A=Cyc_Tcutil_compress(_tmp2C4);void*_tmp29B=_tmp29A;struct Cyc_Absyn_Tvar*_tmp2A3;switch(*((int*)_tmp29B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->f1)){case 5U: _LLD: _LLE:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp29C="`H";_tag_dyneither(_tmp29C,sizeof(char),3U);}));goto _LLC;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp29D="`U";_tag_dyneither(_tmp29D,sizeof(char),3U);}));goto _LLC;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp29E="`RC";_tag_dyneither(_tmp29E,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2U: _LL13: _tmp2A3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29B)->f1;_LL14:
 Cyc_Absyndump_dumptvar(_tmp2A3);goto _LLC;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29B)->f2 == 0){_LL15: _LL16:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2C4));goto _LLC;}else{goto _LL19;}case 8U: _LL17: _LL18:
 Cyc_Absyndump_dump(({const char*_tmp29F="@region(";_tag_dyneither(_tmp29F,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp2C4);Cyc_Absyndump_dump(({const char*_tmp2A0=")";_tag_dyneither(_tmp2A0,sizeof(char),2U);}));goto _LLC;default: _LL19: _LL1A:
({void*_tmp2A1=0U;({struct _dyneither_ptr _tmp36B=({const char*_tmp2A2="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp2A2,sizeof(char),37U);});Cyc_Warn_impos(_tmp36B,_tag_dyneither(_tmp2A1,sizeof(void*),0U));});});}_LLC:;}
# 1287
Cyc_Absyndump_dumptq(_tmp2C0);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL3: _LL4: {
# 1292
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2A4=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp2A5=_tmp2A4;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2A5)->tag == 2U){_LL1C: _LL1D:
 next_is_pointer=1;goto _LL1B;}else{_LL1E: _LL1F:
 goto _LL1B;}_LL1B:;}
# 1298
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2A6=(void*)tms->hd;void*_tmp2A7=_tmp2A6;struct Cyc_List_List*_tmp2BF;struct Cyc_List_List*_tmp2BE;unsigned int _tmp2BD;int _tmp2BC;struct Cyc_List_List*_tmp2BB;unsigned int _tmp2BA;struct Cyc_List_List*_tmp2B9;int _tmp2B8;struct Cyc_Absyn_VarargInfo*_tmp2B7;void*_tmp2B6;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;void*_tmp2B1;void*_tmp2B0;switch(*((int*)_tmp2A7)){case 0U: _LL21: _tmp2B0=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1;_LL22:
# 1305
 Cyc_Absyndump_dump(({const char*_tmp2A8="[]";_tag_dyneither(_tmp2A8,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,_tmp2B0))Cyc_Absyndump_dump(({const char*_tmp2A9="@zeroterm";_tag_dyneither(_tmp2A9,sizeof(char),10U);}));
goto _LL20;case 1U: _LL23: _tmp2B2=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1;_tmp2B1=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f2;_LL24:
# 1309
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2B2);Cyc_Absyndump_dump_char((int)']');
if(Cyc_Absyn_type2bool(0,_tmp2B1))Cyc_Absyndump_dump(({const char*_tmp2AA="@zeroterm";_tag_dyneither(_tmp2AA,sizeof(char),10U);}));
goto _LL20;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->tag == 1U){_LL25: _tmp2B9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f1;_tmp2B8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f2;_tmp2B7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f3;_tmp2B6=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f4;_tmp2B5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f5;_tmp2B4=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f6;_tmp2B3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f7;_LL26:
# 1313
 Cyc_Absyndump_dumpfunargs(_tmp2B9,_tmp2B8,_tmp2B7,_tmp2B6,_tmp2B5,_tmp2B4,_tmp2B3);goto _LL20;}else{_LL27: _tmp2BB=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f1;_tmp2BA=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1)->f2;_LL28:
# 1315
({struct Cyc_List_List*_tmp36E=_tmp2BB;struct _dyneither_ptr _tmp36D=({const char*_tmp2AB="(";_tag_dyneither(_tmp2AB,sizeof(char),2U);});struct _dyneither_ptr _tmp36C=({const char*_tmp2AC=")";_tag_dyneither(_tmp2AC,sizeof(char),2U);});((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp36E,_tmp36D,_tmp36C,({const char*_tmp2AD=",";_tag_dyneither(_tmp2AD,sizeof(char),2U);}));});goto _LL20;}case 4U: _LL29: _tmp2BE=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f1;_tmp2BD=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f2;_tmp2BC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f3;_LL2A:
# 1317
 if(_tmp2BC)Cyc_Absyndump_dumpkindedtvars(_tmp2BE);else{Cyc_Absyndump_dumptvars(_tmp2BE);}goto _LL20;case 5U: _LL2B: _tmp2BF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2A7)->f2;_LL2C:
 Cyc_Absyndump_dumpatts(_tmp2BF);goto _LL20;default: _LL2D: _LL2E:
({void*_tmp2AE=0U;({struct _dyneither_ptr _tmp36F=({const char*_tmp2AF="dumptms";_tag_dyneither(_tmp2AF,sizeof(char),8U);});Cyc_Warn_impos(_tmp36F,_tag_dyneither(_tmp2AE,sizeof(void*),0U));});});}_LL20:;}
# 1321
return;}}_LL0:;};}
# 1325
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2C5=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2C6=_new_region("temp");struct _RegionHandle*temp=& _tmp2C6;_push_region(temp);
{struct _tuple11 _tmp2C7=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2C8=_tmp2C7;struct Cyc_Absyn_Tqual _tmp2CB;void*_tmp2CA;struct Cyc_List_List*_tmp2C9;_LL1: _tmp2CB=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;_tmp2C9=_tmp2C8.f3;_LL2:;
Cyc_Absyndump_dumptq(_tmp2CB);
Cyc_Absyndump_dumpntyp(_tmp2CA);
({int _tmp372=_tmp2C5;struct Cyc_List_List*_tmp371=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2C9);void(*_tmp370)(void*)=f;Cyc_Absyndump_dumptms(_tmp372,_tmp371,_tmp370,a);});}
# 1328
;_pop_region(temp);}
# 1334
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1336
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2CC=0U;({struct Cyc___cycFILE*_tmp374=f;struct _dyneither_ptr _tmp373=({const char*_tmp2CD="\n";_tag_dyneither(_tmp2CD,sizeof(char),2U);});Cyc_fprintf(_tmp374,_tmp373,_tag_dyneither(_tmp2CC,sizeof(void*),0U));});});}
# 1342
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2CE=d->r;void*_tmp2CF=_tmp2CE;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List*_tmp2E8;struct Cyc_List_List*_tmp2E7;struct _dyneither_ptr*_tmp2E6;struct Cyc_List_List*_tmp2E5;struct Cyc_Absyn_Typedefdecl*_tmp2E4;struct Cyc_Absyn_Enumdecl*_tmp2E3;struct Cyc_Absyn_Datatypedecl*_tmp2E2;struct Cyc_Absyn_Aggrdecl*_tmp2E1;struct Cyc_Absyn_Fndecl*_tmp2E0;struct Cyc_Absyn_Vardecl*_tmp2DF;switch(*((int*)_tmp2CF)){case 0U: _LL1: _tmp2DF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_LL2:
# 1345
 if((int)_tmp2DF->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2DF->initializer;
_tmp2DF->initializer=0;
if((int)_tmp2DF->sc == (int)Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2D0="extern ";_tag_dyneither(_tmp2D0,sizeof(char),8U);}));
Cyc_Absyndump_dumpvardecl(_tmp2DF,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2D1="\n";_tag_dyneither(_tmp2D1,sizeof(char),2U);}));
_tmp2DF->initializer=init;
goto _LL0;};case 1U: _LL3: _tmp2E0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_LL4:
# 1355
 if((int)_tmp2E0->sc == (int)Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2E0->sc);{
struct Cyc_Absyn_FnInfo type_info=_tmp2E0->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->tag=5U,_tmp2D3->f1=type_info;_tmp2D3;});
({struct Cyc_Absyn_Tqual _tmp376=Cyc_Absyn_empty_tqual(0U);void*_tmp375=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp376,_tmp375,Cyc_Absyndump_dump_atts_qvar,_tmp2E0);});
Cyc_Absyndump_dump(({const char*_tmp2D2=";\n";_tag_dyneither(_tmp2D2,sizeof(char),3U);}));
goto _LL0;};};case 5U: _LL5: _tmp2E1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_LL6:
# 1364
 if((int)_tmp2E1->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2E1->impl;
if((int)_tmp2E1->sc == (int)Cyc_Absyn_Abstract)_tmp2E1->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2E1);
_tmp2E1->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2D4=";\n";_tag_dyneither(_tmp2D4,sizeof(char),3U);}));
goto _LL0;};case 6U: _LL7: _tmp2E2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_LL8:
 Cyc_Absyndump_dump_datatypedecl(_tmp2E2);Cyc_Absyndump_dump(({const char*_tmp2D5=";\n";_tag_dyneither(_tmp2D5,sizeof(char),3U);}));goto _LL0;case 7U: _LL9: _tmp2E3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_LLA:
 Cyc_Absyndump_dump_enumdecl(_tmp2E3);Cyc_Absyndump_dump(({const char*_tmp2D6=";\n";_tag_dyneither(_tmp2D6,sizeof(char),3U);}));goto _LL0;case 8U: _LLB: _tmp2E4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_LLC:
# 1374
 if(_tmp2E4->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2D7="\n";_tag_dyneither(_tmp2D7,sizeof(char),2U);}));}
# 1378
goto _LL0;case 9U: _LLD: _tmp2E6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_tmp2E5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2CF)->f2;_LLE:
# 1380
 Cyc_Absyndump_dump(({const char*_tmp2D8="namespace ";_tag_dyneither(_tmp2D8,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(_tmp2E6);
Cyc_Absyndump_dump(({const char*_tmp2D9="{\n";_tag_dyneither(_tmp2D9,sizeof(char),3U);}));
for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E5->hd);}
Cyc_Absyndump_dump(({const char*_tmp2DA="}\n";_tag_dyneither(_tmp2DA,sizeof(char),3U);}));
goto _LL0;case 10U: _LLF: _tmp2E7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2CF)->f2;_LL10:
# 1388
 for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E7->hd);}
goto _LL0;case 11U: _LL11: _tmp2E8=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_LL12:
# 1392
 Cyc_Absyndump_dump(({const char*_tmp2DB="extern \"C\" {";_tag_dyneither(_tmp2DB,sizeof(char),13U);}));
for(0;_tmp2E8 != 0;_tmp2E8=_tmp2E8->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2E8->hd);}
Cyc_Absyndump_dump(({const char*_tmp2DC="}\n";_tag_dyneither(_tmp2DC,sizeof(char),3U);}));
goto _LL0;case 12U: _LL13: _tmp2EB=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CF)->f1;_tmp2EA=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CF)->f2;_tmp2E9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CF)->f3;_LL14:
# 1413 "absyndump.cyc"
 goto _LL0;default: _LL15: _LL16:
({void*_tmp2DD=0U;({struct _dyneither_ptr _tmp377=({const char*_tmp2DE="bad top-level declaration";_tag_dyneither(_tmp2DE,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp377,_tag_dyneither(_tmp2DD,sizeof(void*),0U));});});}_LL0:;}
# 1418
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
