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
   (((char *)ptr) + (elt_sz)*(index))
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char *)ptr) + (elt_sz)*(index))

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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned int nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 66 "arg.h"
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 50 "buffer.h"
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 58
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 83
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _dyneither_ptr,int offset,int len);
# 92 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);char Cyc_Arg_Bad[4U]="Bad";char Cyc_Arg_Error[6U]="Error";struct Cyc_Arg_Prefix_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Exact_Arg_Flag_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Unknown_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Missing_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Message_Arg_error_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Wrong_Arg_error_struct{int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple0{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 68 "arg.cyc"
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _dyneither_ptr x){
while(l != 0){
struct _dyneither_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;
unsigned long _tmp1=Cyc_strlen((struct _dyneither_ptr)_tmp0);
if(_tmp1 > (unsigned long)0  && (*((struct _tuple0*)l->hd)).f2){
if(Cyc_strncmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1,_tmp1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}else{
# 76
if(Cyc_strcmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((struct _tuple0*)l->hd)).f1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}
l=l->tl;}
# 80
(int)_throw((void*)({struct Cyc_Core_Not_found_exn_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_Core_Not_found;_tmp2;}));}
# 89
static struct _dyneither_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct _dyneither_ptr s){
if(({char*_tmp67=(char*)s.curr;_tmp67 == (char*)(_tag_dyneither(0,0,0)).curr;}))return _tag_dyneither(0,0,0);
if(howmuch < 0)howmuch=0;{
unsigned long _tmp3=Cyc_strlen((struct _dyneither_ptr)s);
_tmp3 <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmp68=({const char*_tmp4="len <= numelts(s)";_tag_dyneither(_tmp4,sizeof(char),18U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(_tmp68,({const char*_tmp5="arg.cyc";_tag_dyneither(_tmp5,sizeof(char),8U);}),93U);});
if((unsigned long)howmuch > _tmp3){
Cyc_Buffer_add_string(b,s);
return _tag_dyneither(0,0,0);}{
# 100
int i;
for(i=howmuch - 1;i >= 0  && ! isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));-- i){
;}
# 105
if(i < 0)
for(i=howmuch?howmuch - 1: 0;((unsigned long)i < _tmp3  && (int)((const char*)s.curr)[i]) && ! isspace((int)((const char*)s.curr)[i]);++ i){
;}
# 111
Cyc_Buffer_add_substring(b,s,0,i);{
# 114
struct _dyneither_ptr whatsleft=_tag_dyneither(0,0,0);
# 116
for(0;((unsigned long)i < _tmp3  && (int)((const char*)s.curr)[i]) &&  isspace((int)((const char*)s.curr)[i]);++ i){
;}
if((unsigned long)i < _tmp3  && (int)((const char*)s.curr)[i])whatsleft=_dyneither_ptr_plus(s,sizeof(char),i);
return whatsleft;};};};}
# 127
void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct _dyneither_ptr desc){
# 129
if(({char*_tmp69=(char*)item.curr;_tmp69 != (char*)(_tag_dyneither(0,0,0)).curr;}))Cyc_Buffer_add_string(b,item);
if(({char*_tmp6A=(char*)desc.curr;_tmp6A == (char*)(_tag_dyneither(0,0,0)).curr;}))return;
if(indent < 0)indent=0;
if(margin < 0)margin=0;{
# 135
struct _dyneither_ptr indentstr=({unsigned int _tmp11=(unsigned int)(indent + 2)+ 1U;char*_tmp10=_cycalloc_atomic(_check_times(_tmp11,sizeof(char)));({{unsigned int _tmp59=(unsigned int)(indent + 2);unsigned int i;for(i=0;i < _tmp59;++ i){_tmp10[i]='\000';}_tmp10[_tmp59]=0;}0;});_tag_dyneither(_tmp10,sizeof(char),_tmp11);});
{unsigned int i=0U;for(0;i < (unsigned int)(indent + 1);++ i){
({struct _dyneither_ptr _tmp6=_dyneither_ptr_plus(indentstr,sizeof(char),(int)i);char _tmp7=*((char*)_check_dyneither_subscript(_tmp6,sizeof(char),0U));char _tmp8=i == (unsigned int)0?'\n':' ';if(_get_dyneither_size(_tmp6,sizeof(char))== 1U  && (_tmp7 == 0  && _tmp8 != 0))_throw_arraybounds();*((char*)_tmp6.curr)=_tmp8;});}}{
unsigned long _tmp9=Cyc_strlen((struct _dyneither_ptr)item);
struct _dyneither_ptr itemsep;
if(Cyc_strlen((struct _dyneither_ptr)desc)> (unsigned long)0){
if(_tmp9 + (unsigned long)1 > (unsigned long)indent)
itemsep=indentstr;else{
# 144
struct _dyneither_ptr temp=({unsigned int _tmpE=(((unsigned long)indent - _tmp9)+ (unsigned long)1)+ 1U;char*_tmpD=_cycalloc_atomic(_check_times(_tmpE,sizeof(char)));({{unsigned int _tmp58=((unsigned long)indent - _tmp9)+ (unsigned long)1;unsigned int i;for(i=0;i < _tmp58;++ i){_tmpD[i]='\000';}_tmpD[_tmp58]=0;}0;});_tag_dyneither(_tmpD,sizeof(char),_tmpE);});
{unsigned int i=0U;for(0;i < (unsigned long)indent - _tmp9;++ i){({struct _dyneither_ptr _tmpA=_dyneither_ptr_plus(temp,sizeof(char),(int)i);char _tmpB=*((char*)_check_dyneither_subscript(_tmpA,sizeof(char),0U));char _tmpC=' ';if(_get_dyneither_size(_tmpA,sizeof(char))== 1U  && (_tmpB == 0  && _tmpC != 0))_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}}
itemsep=temp;}}else{
# 149
return;}
# 151
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)itemsep);
# 153
while(({char*_tmp6B=(char*)desc.curr;_tmp6B != (char*)(_tag_dyneither(0,0,0)).curr;})){
desc=Cyc_Arg_Justify_break_line(b,margin - indent,desc);
if(({char*_tmp6C=(char*)desc.curr;_tmp6C != (char*)(_tag_dyneither(0,0,0)).curr;}))
Cyc_Buffer_add_string(b,(struct _dyneither_ptr)indentstr);else{
# 158
({struct Cyc_Buffer_t*_tmp6D=b;Cyc_Buffer_add_string(_tmp6D,({const char*_tmpF="\n";_tag_dyneither(_tmpF,sizeof(char),2U);}));});}}
# 160
return;};};}
# 164
void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 166
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0U,_tmp5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)errmsg);_tmp5A;});void*_tmp12[1U];_tmp12[0]=& _tmp14;({struct Cyc___cycFILE*_tmp6F=Cyc_stderr;struct _dyneither_ptr _tmp6E=({const char*_tmp13="%s\n";_tag_dyneither(_tmp13,sizeof(char),4U);});Cyc_fprintf(_tmp6F,_tmp6E,_tag_dyneither(_tmp12,sizeof(void*),1U));});});{
struct Cyc_Buffer_t*_tmp15=Cyc_Buffer_create(1024U);
while(speclist != 0){
({struct Cyc_Buffer_t*_tmp71=_tmp15;struct _dyneither_ptr _tmp70=(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f1,(struct _dyneither_ptr)(*((struct _tuple0*)speclist->hd)).f3);
# 169
Cyc_Arg_Justify_justify_b(_tmp71,12,72,_tmp70,(*((struct _tuple0*)speclist->hd)).f5);});
# 172
speclist=speclist->tl;}
# 174
({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0U,({struct _dyneither_ptr _tmp72=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Buffer_contents(_tmp15));_tmp5B.f1=_tmp72;});_tmp5B;});void*_tmp16[1U];_tmp16[0]=& _tmp18;({struct Cyc___cycFILE*_tmp74=Cyc_stderr;struct _dyneither_ptr _tmp73=({const char*_tmp17="%s";_tag_dyneither(_tmp17,sizeof(char),3U);});Cyc_fprintf(_tmp74,_tmp73,_tag_dyneither(_tmp16,sizeof(void*),1U));});});};}
# 177
int Cyc_Arg_current=0;
static struct _dyneither_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
# 180
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct _dyneither_ptr errmsg){
# 183
struct _dyneither_ptr progname=(unsigned int)prog_pos < _get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr))?*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),prog_pos)):({const char*_tmp33="(?)";_tag_dyneither(_tmp33,sizeof(char),4U);});
{void*_tmp19=e;struct _dyneither_ptr _tmp32;struct _dyneither_ptr _tmp31;struct _dyneither_ptr _tmp30;struct _dyneither_ptr _tmp2F;struct _dyneither_ptr _tmp2E;struct _dyneither_ptr _tmp2D;switch(*((int*)_tmp19)){case 0U: _LL1: _tmp2D=((struct Cyc_Arg_Unknown_Arg_error_struct*)_tmp19)->f1;_LL2:
# 186
 if(({struct _dyneither_ptr _tmp75=(struct _dyneither_ptr)_tmp2D;Cyc_strcmp(_tmp75,({const char*_tmp1A="-help";_tag_dyneither(_tmp1A,sizeof(char),6U);}));})!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0U,_tmp5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);_tmp5D;});struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0U,_tmp5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2D);_tmp5C;});void*_tmp1B[2U];_tmp1B[0]=& _tmp1D,_tmp1B[1]=& _tmp1E;({struct Cyc___cycFILE*_tmp77=Cyc_stderr;struct _dyneither_ptr _tmp76=({const char*_tmp1C="%s: unknown option `%s'.\n";_tag_dyneither(_tmp1C,sizeof(char),26U);});Cyc_fprintf(_tmp77,_tmp76,_tag_dyneither(_tmp1B,sizeof(void*),2U));});});
goto _LL0;case 1U: _LL3: _tmp2E=((struct Cyc_Arg_Missing_Arg_error_struct*)_tmp19)->f1;_LL4:
# 190
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0U,_tmp5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);_tmp5F;});struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0U,_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E);_tmp5E;});void*_tmp1F[2U];_tmp1F[0]=& _tmp21,_tmp1F[1]=& _tmp22;({struct Cyc___cycFILE*_tmp79=Cyc_stderr;struct _dyneither_ptr _tmp78=({const char*_tmp20="%s: option `%s' needs an argument.\n";_tag_dyneither(_tmp20,sizeof(char),36U);});Cyc_fprintf(_tmp79,_tmp78,_tag_dyneither(_tmp1F,sizeof(void*),2U));});});
goto _LL0;case 3U: _LL5: _tmp31=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f1;_tmp30=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f2;_tmp2F=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f3;_LL6:
# 193
({struct Cyc_String_pa_PrintArg_struct _tmp25=({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0U,_tmp63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);_tmp63;});struct Cyc_String_pa_PrintArg_struct _tmp26=({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0U,_tmp62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp30);_tmp62;});struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0U,_tmp61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31);_tmp61;});struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp60;_tmp60.tag=0U,_tmp60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F);_tmp60;});void*_tmp23[4U];_tmp23[0]=& _tmp25,_tmp23[1]=& _tmp26,_tmp23[2]=& _tmp27,_tmp23[3]=& _tmp28;({struct Cyc___cycFILE*_tmp7B=Cyc_stderr;struct _dyneither_ptr _tmp7A=({const char*_tmp24="%s: wrong argument `%s'; option `%s' expects %s.\n";_tag_dyneither(_tmp24,sizeof(char),50U);});Cyc_fprintf(_tmp7B,_tmp7A,_tag_dyneither(_tmp23,sizeof(void*),4U));});});
# 195
goto _LL0;default: _LL7: _tmp32=((struct Cyc_Arg_Message_Arg_error_struct*)_tmp19)->f1;_LL8:
# 197
({struct Cyc_String_pa_PrintArg_struct _tmp2B=({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0U,_tmp65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)progname);_tmp65;});struct Cyc_String_pa_PrintArg_struct _tmp2C=({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0U,_tmp64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32);_tmp64;});void*_tmp29[2U];_tmp29[0]=& _tmp2B,_tmp29[1]=& _tmp2C;({struct Cyc___cycFILE*_tmp7D=Cyc_stderr;struct _dyneither_ptr _tmp7C=({const char*_tmp2A="%s: %s.\n";_tag_dyneither(_tmp2A,sizeof(char),9U);});Cyc_fprintf(_tmp7D,_tmp7C,_tag_dyneither(_tmp29,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}
# 200
Cyc_Arg_usage(speclist,errmsg);
Cyc_Arg_current=(int)_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));}
# 204
void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr orig_args){
# 209
Cyc_Arg_args=orig_args;{
# 211
int initpos=Cyc_Arg_current;
unsigned int l=_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));
# 214
if(({char*_tmp7E=(char*)((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),(int)(l - (unsigned int)1)))->curr;_tmp7E == (char*)(_tag_dyneither(0,0,0)).curr;}))l=l - (unsigned int)1;
++ Cyc_Arg_current;
while((unsigned int)Cyc_Arg_current < l){
struct _dyneither_ptr s=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current));
if((({char*_tmp7F=(char*)s.curr;_tmp7F != (char*)(_tag_dyneither(0,0,0)).curr;}) && _get_dyneither_size(s,sizeof(char))>= (unsigned int)1) && (int)((const char*)s.curr)[0]== (int)'-'){
void*action;
{struct _handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=0;if(setjmp(_tmp34.handler))_tmp36=1;if(!_tmp36){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{void*_tmp35=(void*)Cyc_Core_get_exn_thrown();void*_tmp37=_tmp35;void*_tmp39;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp37)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 224
 if(!anonflagfun(s)){
({int _tmp82=initpos;void*_tmp81=(void*)({struct Cyc_Arg_Unknown_Arg_error_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->tag=0U,_tmp38->f1=s;_tmp38;});struct Cyc_List_List*_tmp80=speclist;Cyc_Arg_stop(_tmp82,_tmp81,_tmp80,errmsg);});
return;}else{
# 229
++ Cyc_Arg_current;
continue;}}else{_LL3: _tmp39=_tmp37;_LL4:(int)_rethrow(_tmp39);}_LL0:;}};}
# 233
{struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=0;if(setjmp(_tmp3A.handler))_tmp3C=1;if(!_tmp3C){
{void*_tmp3D=action;void(*_tmp4C)(struct _dyneither_ptr);void(*_tmp4B)(int);void(*_tmp4A)(struct _dyneither_ptr);void(*_tmp49)(struct _dyneither_ptr,struct _dyneither_ptr);int*_tmp48;int*_tmp47;void(*_tmp46)(struct _dyneither_ptr);void(*_tmp45)();switch(*((int*)_tmp3D)){case 0U: _LL6: _tmp45=((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL7:
 _tmp45();goto _LL5;case 1U: _LL8: _tmp46=((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL9:
 _tmp46(s);goto _LL5;case 3U: _LLA: _tmp47=((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_tmp3D)->f1;_LLB:
*_tmp47=1;goto _LL5;case 4U: _LLC: _tmp48=((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)_tmp3D)->f1;_LLD:
*_tmp48=0;goto _LL5;case 2U: _LLE: _tmp49=((struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*)_tmp3D)->f1;_LLF:
# 240
 if((unsigned int)(Cyc_Arg_current + 1)< l){
_tmp49(s,*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 244
({int _tmp85=initpos;void*_tmp84=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->tag=1U,_tmp3E->f1=s;_tmp3E;});struct Cyc_List_List*_tmp83=speclist;Cyc_Arg_stop(_tmp85,_tmp84,_tmp83,errmsg);});}
goto _LL5;case 5U: _LL10: _tmp4A=((struct Cyc_Arg_String_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL11:
# 247
 if((unsigned int)(Cyc_Arg_current + 1)< l){
_tmp4A(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 251
({int _tmp88=initpos;void*_tmp87=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=1U,_tmp3F->f1=s;_tmp3F;});struct Cyc_List_List*_tmp86=speclist;Cyc_Arg_stop(_tmp88,_tmp87,_tmp86,errmsg);});}
goto _LL5;case 6U: _LL12: _tmp4B=((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL13: {
# 254
struct _dyneither_ptr arg=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1));
int n=0;
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp42=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp66;_tmp66.tag=2U,_tmp66.f1=& n;_tmp66;});void*_tmp40[1U];_tmp40[0]=& _tmp42;({struct _dyneither_ptr _tmp8A=arg;struct _dyneither_ptr _tmp89=({const char*_tmp41="%d";_tag_dyneither(_tmp41,sizeof(char),3U);});Cyc_sscanf(_tmp8A,_tmp89,_tag_dyneither(_tmp40,sizeof(void*),1U));});})!= 1){
({int _tmp8E=initpos;void*_tmp8D=(void*)({struct Cyc_Arg_Wrong_Arg_error_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=3U,_tmp44->f1=s,_tmp44->f2=arg,({struct _dyneither_ptr _tmp8B=({const char*_tmp43="an integer";_tag_dyneither(_tmp43,sizeof(char),11U);});_tmp44->f3=_tmp8B;});_tmp44;});struct Cyc_List_List*_tmp8C=speclist;Cyc_Arg_stop(_tmp8E,_tmp8D,_tmp8C,errmsg);});
_npop_handler(0U);return;}
# 260
_tmp4B(n);
++ Cyc_Arg_current;
goto _LL5;}default: _LL14: _tmp4C=((struct Cyc_Arg_Rest_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL15:
# 264
 while((unsigned int)Cyc_Arg_current < l - (unsigned int)1){
_tmp4C(*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}
# 268
goto _LL5;}_LL5:;}
# 234
;_pop_handler();}else{void*_tmp3B=(void*)Cyc_Core_get_exn_thrown();void*_tmp4D=_tmp3B;void*_tmp50;struct _dyneither_ptr _tmp4F;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp4D)->tag == Cyc_Arg_Bad){_LL17: _tmp4F=((struct Cyc_Arg_Bad_exn_struct*)_tmp4D)->f1;_LL18:
# 271
({int _tmp91=initpos;void*_tmp90=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=2U,_tmp4E->f1=_tmp4F;_tmp4E;});struct Cyc_List_List*_tmp8F=speclist;Cyc_Arg_stop(_tmp91,_tmp90,_tmp8F,errmsg);});goto _LL16;}else{_LL19: _tmp50=_tmp4D;_LL1A:(int)_rethrow(_tmp50);}_LL16:;}};}
# 273
++ Cyc_Arg_current;}else{
# 276
{struct _handler_cons _tmp51;_push_handler(& _tmp51);{int _tmp53=0;if(setjmp(_tmp51.handler))_tmp53=1;if(!_tmp53){anonfun(s);;_pop_handler();}else{void*_tmp52=(void*)Cyc_Core_get_exn_thrown();void*_tmp54=_tmp52;void*_tmp57;struct _dyneither_ptr _tmp56;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp54)->tag == Cyc_Arg_Bad){_LL1C: _tmp56=((struct Cyc_Arg_Bad_exn_struct*)_tmp54)->f1;_LL1D:
# 278
({int _tmp94=initpos;void*_tmp93=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=2U,_tmp55->f1=_tmp56;_tmp55;});struct Cyc_List_List*_tmp92=speclist;Cyc_Arg_stop(_tmp94,_tmp93,_tmp92,errmsg);});goto _LL1B;}else{_LL1E: _tmp57=_tmp54;_LL1F:(int)_rethrow(_tmp57);}_LL1B:;}};}
# 280
++ Cyc_Arg_current;}}};}
