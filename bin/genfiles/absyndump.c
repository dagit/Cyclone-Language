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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 462
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 470
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 705 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress_kb(void*);
# 908
int Cyc_Absyn_type2bool(int def,void*);
# 913
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
# 1151
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1161
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1169
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 61
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 78
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 112 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 28
void Cyc_Warn_warn(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Warn_err(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 39
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned f2;};
# 27 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned);
# 37
static int Cyc_Absyndump_expand_typedefs;
# 41
static int Cyc_Absyndump_qvar_to_Cids;
# 44
static int Cyc_Absyndump_add_cyc_prefix;
# 47
static int Cyc_Absyndump_generate_line_directives;
# 51
static int Cyc_Absyndump_to_VC;
# 53
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 68 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 71
static void Cyc_Absyndump_dumptyp(void*);
static void Cyc_Absyndump_dumpntyp(void*t);
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned,int do_semi);
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*,void(*f)(void*),void*a);
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 90
static void Cyc_Absyndump_dumploc(unsigned);
# 93
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
static char Cyc_Absyndump_prev_char='x';
# 96
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
# 116
static void Cyc_Absyndump_dump(struct _dyneither_ptr s){
unsigned sz=Cyc_strlen((struct _dyneither_ptr)s);
# 123
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= (unsigned)1){
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - (unsigned)1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 131
static void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 134
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 139
static void Cyc_Absyndump_dump_char(int c){
# 141
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpE[1U]="";
# 145
static void Cyc_Absyndump_dumploc(unsigned loc){
static struct _dyneither_ptr last_file={_tmpE,_tmpE,_tmpE + 1U};
static unsigned last_line=0U;
if(loc == (unsigned)0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 151
struct _tuple13 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple13 _tmp2=_tmp1;struct _dyneither_ptr _tmpD;unsigned _tmpC;_LL1: _tmpD=_tmp2.f1;_tmpC=_tmp2.f2;_LL2:;
if(({char*_tmp2DE=(char*)_tmpD.curr;_tmp2DE == (char*)(_tag_dyneither(0,0,0)).curr;})||(char*)_tmpD.curr == (char*)last_file.curr && _tmpC == last_line)return;
if((char*)_tmpD.curr == (char*)last_file.curr && _tmpC == last_line + (unsigned)1)
({void*_tmp3=0U;({struct Cyc___cycFILE*_tmp2E0=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2DF=({const char*_tmp4="\n";_tag_dyneither(_tmp4,sizeof(char),2U);});Cyc_fprintf(_tmp2E0,_tmp2DF,_tag_dyneither(_tmp3,sizeof(void*),0U));});});else{
if((char*)_tmpD.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp7=({struct Cyc_Int_pa_PrintArg_struct _tmp2D1;_tmp2D1.tag=1U,_tmp2D1.f1=(unsigned long)((int)_tmpC);_tmp2D1;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct Cyc___cycFILE*_tmp2E2=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2E1=({const char*_tmp6="\n# %d\n";_tag_dyneither(_tmp6,sizeof(char),7U);});Cyc_fprintf(_tmp2E2,_tmp2E1,_tag_dyneither(_tmp5,sizeof(void*),1U));});});else{
# 158
({struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp2D3;_tmp2D3.tag=1U,_tmp2D3.f1=(unsigned long)((int)_tmpC);_tmp2D3;});struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmp2D2;_tmp2D2.tag=0U,_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD);_tmp2D2;});void*_tmp8[2U];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB;({struct Cyc___cycFILE*_tmp2E4=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp2E3=({const char*_tmp9="\n# %d %s\n";_tag_dyneither(_tmp9,sizeof(char),10U);});Cyc_fprintf(_tmp2E4,_tmp2E3,_tag_dyneither(_tmp8,sizeof(void*),2U));});});}}
# 160
last_file=_tmpD;
last_line=_tmpC;};}
# 166
static void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
Cyc_Absyndump_dump(*s);}
# 169
static void Cyc_Absyndump_dump_str_nospace(struct _dyneither_ptr*s){
Cyc_Absyndump_dump_nospace(*s);}
# 173
static void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 177
static void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 184
f(l->hd);}
# 186
static void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 193
f(env,l->hd);}
# 195
static void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 197
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 201
static void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 203
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 207
static void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 209
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 213
static void Cyc_Absyndump_ignore(void*x){;}
# 215
static void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmpF=0;
struct _dyneither_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp10=(*v).f1;union Cyc_Absyn_Nmspace _tmp11=_tmp10;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp12;switch((_tmp11.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp12=(_tmp11.Rel_n).val;_LL4:
 _tmpF=_tmp12;goto _LL0;case 3U: _LL5: _tmp13=(_tmp11.C_n).val;_LL6:
 goto _LL0;default: _LL7: _tmp14=(_tmp11.Abs_n).val;_LL8:
# 223
 if(Cyc_Absyndump_qvar_to_Cids || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpF=_tmp14;
goto _LL0;}_LL0:;}
# 228
if(Cyc_Absyndump_need_space_before())
Cyc_Absyndump_dump_char((int)' ');{
struct _dyneither_ptr sep=Cyc_Absyndump_qvar_to_Cids?({const char*_tmp15="_";_tag_dyneither(_tmp15,sizeof(char),2U);}):({const char*_tmp16="::";_tag_dyneither(_tmp16,sizeof(char),3U);});
if(prefix != 0){
Cyc_Absyndump_dump_nospace(*(*prefix));Cyc_Absyndump_dump_nospace(sep);}
# 234
if(_tmpF != 0){
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_str_nospace,_tmpF,sep);Cyc_Absyndump_dump_nospace(sep);}
# 237
Cyc_Absyndump_dump_nospace(*(*v).f2);};}
# 240
static void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp17="restrict";_tag_dyneither(_tmp17,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp18="volatile";_tag_dyneither(_tmp18,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp19="const";_tag_dyneither(_tmp19,sizeof(char),6U);}));}
# 245
static void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1A=sc;switch(_tmp1A){case Cyc_Absyn_Static: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp1B="static";_tag_dyneither(_tmp1B,sizeof(char),7U);}));return;case Cyc_Absyn_Public: _LL3: _LL4:
 return;case Cyc_Absyn_Extern: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp1C="extern";_tag_dyneither(_tmp1C,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1D="extern \"C\"";_tag_dyneither(_tmp1D,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract: _LL9: _LLA:
 Cyc_Absyndump_dump(({const char*_tmp1E="abstract";_tag_dyneither(_tmp1E,sizeof(char),9U);}));return;case Cyc_Absyn_Register: _LLB: _LLC:
 Cyc_Absyndump_dump(({const char*_tmp1F="register";_tag_dyneither(_tmp1F,sizeof(char),9U);}));return;default: _LLD: _LLE:
 return;}_LL0:;}
# 256
static void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp20=k;if(_tmp20 == Cyc_Absyn_StructA){_LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp21="struct";_tag_dyneither(_tmp21,sizeof(char),7U);}));return;}else{_LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp22="union";_tag_dyneither(_tmp22,sizeof(char),6U);}));return;}_LL0:;}
# 263
static void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
Cyc_Absyndump_dump(Cyc_Absynpp_kind2string(ka));}
# 267
static void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp2E7=ts;struct _dyneither_ptr _tmp2E6=({const char*_tmp23="<";_tag_dyneither(_tmp23,sizeof(char),2U);});struct _dyneither_ptr _tmp2E5=({const char*_tmp24=">";_tag_dyneither(_tmp24,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp2E7,_tmp2E6,_tmp2E5,({const char*_tmp25=",";_tag_dyneither(_tmp25,sizeof(char),2U);}));});}
# 270
static void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if((int)*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== (int)'#'){
Cyc_Absyndump_dump(({const char*_tmp26="`G";_tag_dyneither(_tmp26,sizeof(char),3U);}));
{void*_tmp27=Cyc_Absyn_compress_kb(tv->kind);void*_tmp28=_tmp27;struct Cyc_Absyn_Kind*_tmp2B;struct Cyc_Absyn_Kind*_tmp2A;switch(*((int*)_tmp28)){case 0U: _LL1: _tmp2A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp28)->f1;_LL2:
 _tmp2B=_tmp2A;goto _LL4;case 2U: _LL3: _tmp2B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp28)->f2;_LL4:
 Cyc_Absyndump_dumpkind(_tmp2B);goto _LL0;default: _LL5: _LL6:
 Cyc_Absyndump_dump_nospace(({const char*_tmp29="K";_tag_dyneither(_tmp29,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 279
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 281
Cyc_Absyndump_dump(n);}}
# 283
static void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp2C=Cyc_Absyn_compress_kb(tv->kind);void*_tmp2D=_tmp2C;struct Cyc_Absyn_Kind*_tmp2F;switch(*((int*)_tmp2D)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2D)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2D)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL5: _LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _tmp2F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp2D)->f1;_LL8:
 Cyc_Absyndump_dump(({const char*_tmp2E="::";_tag_dyneither(_tmp2E,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(_tmp2F);goto _LL0;}}_LL0:;};}
# 292
static void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp2EA=tvs;struct _dyneither_ptr _tmp2E9=({const char*_tmp30="<";_tag_dyneither(_tmp30,sizeof(char),2U);});struct _dyneither_ptr _tmp2E8=({const char*_tmp31=">";_tag_dyneither(_tmp31,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp2EA,_tmp2E9,_tmp2E8,({const char*_tmp32=",";_tag_dyneither(_tmp32,sizeof(char),2U);}));});}
# 295
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp2ED=tvs;struct _dyneither_ptr _tmp2EC=({const char*_tmp33="<";_tag_dyneither(_tmp33,sizeof(char),2U);});struct _dyneither_ptr _tmp2EB=({const char*_tmp34=">";_tag_dyneither(_tmp34,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp2ED,_tmp2EC,_tmp2EB,({const char*_tmp35=",";_tag_dyneither(_tmp35,sizeof(char),2U);}));});}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 298
static void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 301
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp2F0=ts;struct _dyneither_ptr _tmp2EF=({const char*_tmp36="(";_tag_dyneither(_tmp36,sizeof(char),2U);});struct _dyneither_ptr _tmp2EE=({const char*_tmp37=")";_tag_dyneither(_tmp37,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp2F0,_tmp2EF,_tmp2EE,({const char*_tmp38=",";_tag_dyneither(_tmp38,sizeof(char),2U);}));});}
# 305
static void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp39=(void*)atts->hd;void*_tmp3A=_tmp39;switch(*((int*)_tmp3A)){case 1U: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp3B="_stdcall";_tag_dyneither(_tmp3B,sizeof(char),9U);}));return;case 2U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp3C="_cdecl";_tag_dyneither(_tmp3C,sizeof(char),7U);}));return;case 3U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp3D="_fastcall";_tag_dyneither(_tmp3D,sizeof(char),10U);}));return;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 315
static void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 317
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp3E=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp3F=_tmp3E;switch(*((int*)_tmp3F)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 325
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp40="__declspec(";_tag_dyneither(_tmp40,sizeof(char),12U);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp41=(void*)atts->hd;void*_tmp42=_tmp41;switch(*((int*)_tmp42)){case 1U: _LLA: _LLB:
 goto _LLD;case 2U: _LLC: _LLD:
 goto _LLF;case 3U: _LLE: _LLF:
 goto _LL9;default: _LL10: _LL11:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 335
Cyc_Absyndump_dump_char((int)')');}
# 338
static void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp43=Cyc_Cyclone_c_compiler;if(_tmp43 == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 342
 Cyc_Absyndump_dump(({const char*_tmp44=" __attribute__((";_tag_dyneither(_tmp44,sizeof(char),17U);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp45=",";_tag_dyneither(_tmp45,sizeof(char),2U);}));}
# 347
Cyc_Absyndump_dump(({const char*_tmp46=")) ";_tag_dyneither(_tmp46,sizeof(char),4U);}));
return;}else{_LL3: _LL4:
# 350
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL0:;};}
# 355
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 359
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
{void*_tmp47=Cyc_Tcutil_compress(t);void*_tmp48=_tmp47;struct Cyc_List_List*_tmp50;void*_tmp4F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2 != 0){_LL1: _tmp4F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2)->hd;_LL2:
 rgions=({struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->hd=_tmp4F,_tmp49->tl=rgions;_tmp49;});goto _LL0;}else{goto _LL5;}case 9U: _LL3: _tmp50=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2;_LL4:
# 365
 for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
struct _tuple15 _tmp4A=Cyc_Absyndump_effects_split((void*)_tmp50->hd);struct _tuple15 _tmp4B=_tmp4A;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4C;_LL8: _tmp4D=_tmp4B.f1;_tmp4C=_tmp4B.f2;_LL9:;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp4D,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp4C,effects);}
# 370
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 effects=({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=t,_tmp4E->tl=effects;_tmp4E;});goto _LL0;}_LL0:;}
# 373
return({struct _tuple15 _tmp2D4;_tmp2D4.f1=rgions,_tmp2D4.f2=effects;_tmp2D4;});}
# 376
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp51=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp52=_tmp51;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp59;_LL1: _tmp5A=_tmp52.f1;_tmp59=_tmp52.f2;_LL2:;
_tmp5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5A);
_tmp59=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp59);
if(_tmp59 != 0){
({struct Cyc_List_List*_tmp2F3=_tmp59;struct _dyneither_ptr _tmp2F2=({const char*_tmp53="";_tag_dyneither(_tmp53,sizeof(char),1U);});struct _dyneither_ptr _tmp2F1=({const char*_tmp54="";_tag_dyneither(_tmp54,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpntyp,_tmp2F3,_tmp2F2,_tmp2F1,({const char*_tmp55="+";_tag_dyneither(_tmp55,sizeof(char),2U);}));});
if(_tmp5A != 0)Cyc_Absyndump_dump_char((int)'+');}
# 384
if(_tmp5A != 0 || _tmp59 == 0)
({struct Cyc_List_List*_tmp2F6=_tmp5A;struct _dyneither_ptr _tmp2F5=({const char*_tmp56="{";_tag_dyneither(_tmp56,sizeof(char),2U);});struct _dyneither_ptr _tmp2F4=({const char*_tmp57="}";_tag_dyneither(_tmp57,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumprgn,_tmp2F6,_tmp2F5,_tmp2F4,({const char*_tmp58=",";_tag_dyneither(_tmp58,sizeof(char),2U);}));});}
# 389
static void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp5B=t;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBB;struct _tuple0*_tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_Absyn_Datatypedecl*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Aggrdecl*_tmpB6;enum Cyc_Absyn_AggrKind _tmpB5;struct Cyc_List_List*_tmpB4;struct Cyc_List_List*_tmpB3;struct Cyc_Core_Opt*_tmpB2;void*_tmpB1;int _tmpB0;struct Cyc_Core_Opt*_tmpAF;int _tmpAE;struct Cyc_Absyn_Tvar*_tmpAD;void*_tmpAC;struct Cyc_List_List*_tmpAB;switch(*((int*)_tmp5B)){case 4U: _LL1: _LL2:
# 392
 goto _LL4;case 5U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return;case 0U: if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B)->f1)->tag == 0U){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp5C="void";_tag_dyneither(_tmp5C,sizeof(char),5U);}));return;}else{_LL1F: _tmpAC=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B)->f1;_tmpAB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B)->f2;_LL20: {
# 412
void*_tmp66=_tmpAC;struct _dyneither_ptr _tmpAA;struct Cyc_List_List*_tmpA9;struct _tuple0*_tmpA8;union Cyc_Absyn_AggrInfo _tmpA7;int _tmpA6;union Cyc_Absyn_DatatypeFieldInfo _tmpA5;union Cyc_Absyn_DatatypeInfo _tmpA4;switch(*((int*)_tmp66)){case 18U: _LL22: _tmpA4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp66)->f1;_LL23:
# 414
{union Cyc_Absyn_DatatypeInfo _tmp67=_tmpA4;struct _tuple0*_tmp6D;int _tmp6C;struct _tuple0*_tmp6B;int _tmp6A;if((_tmp67.UnknownDatatype).tag == 1){_LL69: _tmp6B=((_tmp67.UnknownDatatype).val).name;_tmp6A=((_tmp67.UnknownDatatype).val).is_extensible;_LL6A:
 _tmp6D=_tmp6B;_tmp6C=_tmp6A;goto _LL6C;}else{_LL6B: _tmp6D=(*(_tmp67.KnownDatatype).val)->name;_tmp6C=(*(_tmp67.KnownDatatype).val)->is_extensible;_LL6C:
# 417
 if(_tmp6C)Cyc_Absyndump_dump(({const char*_tmp68="@extensible";_tag_dyneither(_tmp68,sizeof(char),12U);}));
Cyc_Absyndump_dump(({const char*_tmp69="datatype";_tag_dyneither(_tmp69,sizeof(char),9U);}));Cyc_Absyndump_dumpqvar(_tmp6D);Cyc_Absyndump_dumptps(_tmpAB);
goto _LL68;}_LL68:;}
# 421
return;case 19U: _LL24: _tmpA5=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp66)->f1;_LL25:
# 423
{union Cyc_Absyn_DatatypeFieldInfo _tmp6E=_tmpA5;struct _tuple0*_tmp77;int _tmp76;struct _tuple0*_tmp75;struct _tuple0*_tmp74;struct _tuple0*_tmp73;int _tmp72;if((_tmp6E.UnknownDatatypefield).tag == 1){_LL6E: _tmp74=((_tmp6E.UnknownDatatypefield).val).datatype_name;_tmp73=((_tmp6E.UnknownDatatypefield).val).field_name;_tmp72=((_tmp6E.UnknownDatatypefield).val).is_extensible;_LL6F:
# 425
 _tmp77=_tmp74;_tmp76=_tmp72;_tmp75=_tmp73;goto _LL71;}else{_LL70: _tmp77=(((_tmp6E.KnownDatatypefield).val).f1)->name;_tmp76=(((_tmp6E.KnownDatatypefield).val).f1)->is_extensible;_tmp75=(((_tmp6E.KnownDatatypefield).val).f2)->name;_LL71:
# 428
 if(_tmp76)Cyc_Absyndump_dump(({const char*_tmp6F="@extensible ";_tag_dyneither(_tmp6F,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp70="datatype ";_tag_dyneither(_tmp70,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(_tmp77);Cyc_Absyndump_dump(({const char*_tmp71=".";_tag_dyneither(_tmp71,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(_tmp75);
Cyc_Absyndump_dumptps(_tmpAB);
goto _LL6D;}_LL6D:;}
# 433
return;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp66)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp66)->f2){case Cyc_Absyn_Int_sz: _LL26: _LL27:
 goto _LL29;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
# 436
 goto _LL2D;case Cyc_Absyn_Char_sz: _LL2E: _LL2F:
# 438
 Cyc_Absyndump_dump(({const char*_tmp7A="char";_tag_dyneither(_tmp7A,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz: _LL34: _LL35:
# 441
 goto _LL37;case Cyc_Absyn_LongLong_sz: _LL40: _LL41:
# 447
 goto _LL43;default: goto _LL66;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp66)->f2){case Cyc_Absyn_Int_sz: _LL28: _LL29:
# 435
 Cyc_Absyndump_dump(({const char*_tmp78="int";_tag_dyneither(_tmp78,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL2C: _LL2D:
# 437
 Cyc_Absyndump_dump(({const char*_tmp79="long";_tag_dyneither(_tmp79,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz: _LL30: _LL31:
# 439
 Cyc_Absyndump_dump(({const char*_tmp7B="signed char";_tag_dyneither(_tmp7B,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL36: _LL37:
# 442
 Cyc_Absyndump_dump(({const char*_tmp7D="short";_tag_dyneither(_tmp7D,sizeof(char),6U);}));return;case Cyc_Absyn_LongLong_sz: _LL42: _LL43: {
# 449
enum Cyc_Cyclone_C_Compilers _tmp82=Cyc_Cyclone_c_compiler;if(_tmp82 == Cyc_Cyclone_Gcc_c){_LL73: _LL74:
 Cyc_Absyndump_dump(({const char*_tmp83="long long";_tag_dyneither(_tmp83,sizeof(char),10U);}));return;}else{_LL75: _LL76:
 Cyc_Absyndump_dump(({const char*_tmp84="__int64";_tag_dyneither(_tmp84,sizeof(char),8U);}));return;}_LL72:;}default: goto _LL66;}case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp66)->f2){case Cyc_Absyn_Char_sz: _LL32: _LL33:
# 440
 Cyc_Absyndump_dump(({const char*_tmp7C="unsigned char";_tag_dyneither(_tmp7C,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz: _LL38: _LL39:
# 443
 Cyc_Absyndump_dump(({const char*_tmp7E="unsigned short";_tag_dyneither(_tmp7E,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz: _LL3A: _LL3B:
 Cyc_Absyndump_dump(({const char*_tmp7F="unsigned";_tag_dyneither(_tmp7F,sizeof(char),9U);}));return;case Cyc_Absyn_Long_sz: _LL3C: _LL3D:
 Cyc_Absyndump_dump(({const char*_tmp80="unsigned long";_tag_dyneither(_tmp80,sizeof(char),14U);}));return;case Cyc_Absyn_LongLong_sz: _LL3E: _LL3F:
 Cyc_Absyndump_dump(({const char*_tmp81="unsigned";_tag_dyneither(_tmp81,sizeof(char),9U);}));goto _LL41;default: goto _LL66;}default: goto _LL66;}case 2U: _LL44: _tmpA6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp66)->f1;_LL45: {
# 454
int _tmp85=_tmpA6;switch(_tmp85){case 0U: _LL78: _LL79:
 Cyc_Absyndump_dump(({const char*_tmp86="float";_tag_dyneither(_tmp86,sizeof(char),6U);}));return;case 1U: _LL7A: _LL7B:
 Cyc_Absyndump_dump(({const char*_tmp87="double";_tag_dyneither(_tmp87,sizeof(char),7U);}));return;default: _LL7C: _LL7D:
 Cyc_Absyndump_dump(({const char*_tmp88="long double";_tag_dyneither(_tmp88,sizeof(char),12U);}));return;}_LL77:;}case 20U: _LL46: _tmpA7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp66)->f1;_LL47: {
# 460
struct _tuple10 _tmp89=Cyc_Absyn_aggr_kinded_name(_tmpA7);struct _tuple10 _tmp8A=_tmp89;enum Cyc_Absyn_AggrKind _tmp8C;struct _tuple0*_tmp8B;_LL7F: _tmp8C=_tmp8A.f1;_tmp8B=_tmp8A.f2;_LL80:;
Cyc_Absyndump_dumpaggr_kind(_tmp8C);Cyc_Absyndump_dumpqvar(_tmp8B);Cyc_Absyndump_dumptps(_tmpAB);
return;}case 15U: _LL48: _tmpA8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp66)->f1;_LL49:
 Cyc_Absyndump_dump(({const char*_tmp8D="enum";_tag_dyneither(_tmp8D,sizeof(char),5U);}));Cyc_Absyndump_dumpqvar(_tmpA8);return;case 16U: _LL4A: _tmpA9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp66)->f1;_LL4B:
 Cyc_Absyndump_dump(({const char*_tmp8E="enum{";_tag_dyneither(_tmp8E,sizeof(char),6U);}));Cyc_Absyndump_dumpenumfields(_tmpA9);Cyc_Absyndump_dump(({const char*_tmp8F="}";_tag_dyneither(_tmp8F,sizeof(char),2U);}));return;case 17U: _LL4C: _tmpAA=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp66)->f1;_LL4D:
 Cyc_Absyndump_dump(_tmpAA);return;case 3U: _LL4E: _LL4F:
 Cyc_Absyndump_dump(({const char*_tmp90="region_t<";_tag_dyneither(_tmp90,sizeof(char),10U);}));Cyc_Absyndump_dumprgn((void*)((struct Cyc_List_List*)_check_null(_tmpAB))->hd);Cyc_Absyndump_dump(({const char*_tmp91=">";_tag_dyneither(_tmp91,sizeof(char),2U);}));return;case 4U: _LL50: _LL51:
 Cyc_Absyndump_dump(({const char*_tmp92="tag_t<";_tag_dyneither(_tmp92,sizeof(char),7U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(_tmpAB))->hd);Cyc_Absyndump_dump(({const char*_tmp93=">";_tag_dyneither(_tmp93,sizeof(char),2U);}));return;case 6U: _LL52: _LL53:
 Cyc_Absyndump_dump(({const char*_tmp94="`U";_tag_dyneither(_tmp94,sizeof(char),3U);}));return;case 7U: _LL54: _LL55:
 Cyc_Absyndump_dump(({const char*_tmp95="`RC";_tag_dyneither(_tmp95,sizeof(char),4U);}));return;case 5U: _LL56: _LL57:
 Cyc_Absyndump_dump(({const char*_tmp96="`H";_tag_dyneither(_tmp96,sizeof(char),3U);}));return;case 8U: _LL58: _LL59:
 Cyc_Absyndump_dump(({const char*_tmp97="{";_tag_dyneither(_tmp97,sizeof(char),2U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(_tmpAB))->hd);Cyc_Absyndump_dump(({const char*_tmp98="}";_tag_dyneither(_tmp98,sizeof(char),2U);}));return;case 10U: _LL5A: _LL5B:
 Cyc_Absyndump_dump(({const char*_tmp99="regions(";_tag_dyneither(_tmp99,sizeof(char),9U);}));Cyc_Absyndump_dumptyp((void*)((struct Cyc_List_List*)_check_null(_tmpAB))->hd);Cyc_Absyndump_dump(({const char*_tmp9A=")";_tag_dyneither(_tmp9A,sizeof(char),2U);}));return;case 9U: _LL5C: _LL5D:
({struct Cyc_List_List*_tmp2F9=_tmpAB;struct _dyneither_ptr _tmp2F8=({const char*_tmp9B="";_tag_dyneither(_tmp9B,sizeof(char),1U);});struct _dyneither_ptr _tmp2F7=({const char*_tmp9C="";_tag_dyneither(_tmp9C,sizeof(char),1U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumptyp,_tmp2F9,_tmp2F8,_tmp2F7,({const char*_tmp9D="+";_tag_dyneither(_tmp9D,sizeof(char),2U);}));});return;case 11U: _LL5E: _LL5F:
 Cyc_Absyndump_dump(({const char*_tmp9E="@true";_tag_dyneither(_tmp9E,sizeof(char),6U);}));return;case 12U: _LL60: _LL61:
 Cyc_Absyndump_dump(({const char*_tmp9F="@false";_tag_dyneither(_tmp9F,sizeof(char),7U);}));return;case 13U: _LL62: _LL63:
 Cyc_Absyndump_dump(({const char*_tmpA0="@thin";_tag_dyneither(_tmpA0,sizeof(char),6U);}));return;case 14U: _LL64: _LL65:
 Cyc_Absyndump_dump(({const char*_tmpA1="@fat";_tag_dyneither(_tmpA1,sizeof(char),5U);}));return;default: _LL66: _LL67:
({void*_tmpA2=0U;({struct _dyneither_ptr _tmp2FA=({const char*_tmpA3="bad type constructor";_tag_dyneither(_tmpA3,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2FA,_tag_dyneither(_tmpA2,sizeof(void*),0U));});});}_LL21:;}}case 2U: _LL9: _tmpAD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5B)->f1;_LLA:
# 396
 Cyc_Absyndump_dumptvar(_tmpAD);return;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f2 == 0){_LLB: _tmpAF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f1;_tmpAE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f3;_LLC:
# 398
 Cyc_Absyndump_dump(({const char*_tmp5D="`E";_tag_dyneither(_tmp5D,sizeof(char),3U);}));
if(_tmpAF == 0)Cyc_Absyndump_dump(({const char*_tmp5E="K";_tag_dyneither(_tmp5E,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmpAF->v);}
Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp61=({struct Cyc_Int_pa_PrintArg_struct _tmp2D5;_tmp2D5.tag=1U,_tmp2D5.f1=(unsigned long)_tmpAE;_tmp2D5;});void*_tmp5F[1U];_tmp5F[0]=& _tmp61;({struct _dyneither_ptr _tmp2FB=({const char*_tmp60="%d";_tag_dyneither(_tmp60,sizeof(char),3U);});Cyc_aprintf(_tmp2FB,_tag_dyneither(_tmp5F,sizeof(void*),1U));});}));return;}else{_LLD: _tmpB2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f1;_tmpB1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f2;_tmpB0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5B)->f3;_LLE:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmpB1));return;}case 6U: _LLF: _tmpB3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B)->f1;_LL10:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpB3);return;case 7U: _LL11: _tmpB5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B)->f1;_tmpB4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B)->f2;_LL12:
# 404
 Cyc_Absyndump_dumpaggr_kind(_tmpB5);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpB4);Cyc_Absyndump_dump_char((int)'}');return;case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)){case 0U: _LL13: _tmpB6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)->f1;_LL14:
 Cyc_Absyndump_dump_aggrdecl(_tmpB6);return;case 1U: _LL15: _tmpB7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)->f1;_LL16:
 Cyc_Absyndump_dump_enumdecl(_tmpB7);return;default: _LL17: _tmpB8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5B)->f1)->r)->f1;_LL18:
 Cyc_Absyndump_dump_datatypedecl(_tmpB8);return;}case 8U: _LL19: _tmpBA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5B)->f1;_tmpB9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5B)->f2;_LL1A:
 Cyc_Absyndump_dumpqvar(_tmpBA),Cyc_Absyndump_dumptps(_tmpB9);return;case 9U: _LL1B: _tmpBB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5B)->f1;_LL1C:
 Cyc_Absyndump_dump(({const char*_tmp62="valueof_t(";_tag_dyneither(_tmp62,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(_tmpBB);Cyc_Absyndump_dump(({const char*_tmp63=")";_tag_dyneither(_tmp63,sizeof(char),2U);}));return;default: _LL1D: _tmpBC=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5B)->f1;_LL1E:
 Cyc_Absyndump_dump(({const char*_tmp64="typeof(";_tag_dyneither(_tmp64,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmpBC);Cyc_Absyndump_dump(({const char*_tmp65=")";_tag_dyneither(_tmp65,sizeof(char),2U);}));return;}_LL0:;}
# 483
static void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 486
static void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 489
static void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
Cyc_Absyndump_dumpeff((*cmp).f1);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn((*cmp).f2);}
# 492
static void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp2FC=rgn_po;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp2FC,({const char*_tmpBD=",";_tag_dyneither(_tmpBD,sizeof(char),2U);}));});}
# 495
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 499
({struct Cyc_List_List*_tmp2FF=args;struct _dyneither_ptr _tmp2FE=({const char*_tmpBE="(";_tag_dyneither(_tmpBE,sizeof(char),2U);});struct _dyneither_ptr _tmp2FD=({const char*_tmpBF="";_tag_dyneither(_tmpBF,sizeof(char),1U);});((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpfunarg,_tmp2FF,_tmp2FE,_tmp2FD,({const char*_tmpC0=",";_tag_dyneither(_tmpC0,sizeof(char),2U);}));});
if(c_varargs || cyc_varargs != 0)
Cyc_Absyndump_dump(({const char*_tmpC1=",...";_tag_dyneither(_tmpC1,sizeof(char),5U);}));
if(cyc_varargs != 0){
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpC2=" inject ";_tag_dyneither(_tmpC2,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(({struct _tuple8*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=cyc_varargs->name,_tmpC3->f2=cyc_varargs->tq,_tmpC3->f3=cyc_varargs->type;_tmpC3;}));}
# 506
if(effopt != 0){
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff(effopt);}
# 509
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 512
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpC4=" @requires(";_tag_dyneither(_tmpC4,sizeof(char),12U);}));Cyc_Absyndump_dumpexp(req);Cyc_Absyndump_dump_char((int)')');}
# 516
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpC5=" @ensures(";_tag_dyneither(_tmpC5,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(ens);Cyc_Absyndump_dump_char((int)')');}}
# 521
static void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _tmp300=Cyc_Absyn_empty_tqual(0U);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp300,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 525
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpC6=d;struct _dyneither_ptr*_tmpC9;struct Cyc_Absyn_Exp*_tmpC8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC6)->tag == 0U){_LL1: _tmpC8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC6)->f1;_LL2:
 Cyc_Absyndump_dump(({const char*_tmpC7=".[";_tag_dyneither(_tmpC7,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(_tmpC8);Cyc_Absyndump_dump_char((int)']');goto _LL0;}else{_LL3: _tmpC9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpC6)->f1;_LL4:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpC9);goto _LL0;}_LL0:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 531
static void Cyc_Absyndump_dumpde(struct _tuple17*de){
({struct Cyc_List_List*_tmp303=(*de).f1;struct _dyneither_ptr _tmp302=({const char*_tmpCA="";_tag_dyneither(_tmpCA,sizeof(char),1U);});struct _dyneither_ptr _tmp301=({const char*_tmpCB="=";_tag_dyneither(_tmpCB,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp303,_tmp302,_tmp301,({const char*_tmpCC="=";_tag_dyneither(_tmpCC,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 536
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_tmpCD=f;unsigned _tmpD2;struct _dyneither_ptr*_tmpD1;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpCD)->tag == 0U){_LL1: _tmpD1=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpCD)->f1;_LL2:
 Cyc_Absyndump_dump_nospace(*_tmpD1);return;}else{_LL3: _tmpD2=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmpCD)->f1;_LL4:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD0=({struct Cyc_Int_pa_PrintArg_struct _tmp2D6;_tmp2D6.tag=1U,_tmp2D6.f1=(unsigned long)((int)_tmpD2);_tmp2D6;});void*_tmpCE[1U];_tmpCE[0]=& _tmpD0;({struct _dyneither_ptr _tmp304=({const char*_tmpCF="%d";_tag_dyneither(_tmpCF,sizeof(char),3U);});Cyc_aprintf(_tmp304,_tag_dyneither(_tmpCE,sizeof(void*),1U));});}));return;}_LL0:;}
# 543
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp308=inprec;struct Cyc_List_List*_tmp307=es;struct _dyneither_ptr _tmp306=({const char*_tmpD3="";_tag_dyneither(_tmpD3,sizeof(char),1U);});struct _dyneither_ptr _tmp305=({const char*_tmpD4="";_tag_dyneither(_tmpD4,sizeof(char),1U);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp308,_tmp307,_tmp306,_tmp305,({const char*_tmpD5=",";_tag_dyneither(_tmpD5,sizeof(char),2U);}));});}
# 547
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpD6="(";_tag_dyneither(_tmpD6,sizeof(char),2U);}));
{void*_tmpD7=e->r;void*_tmpD8=_tmpD7;struct Cyc_Absyn_Stmt*_tmp17E;struct Cyc_Core_Opt*_tmp17D;struct Cyc_List_List*_tmp17C;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*_tmp17A;int _tmp179;struct Cyc_Absyn_Exp*_tmp178;void**_tmp177;struct Cyc_Absyn_Exp*_tmp176;int _tmp175;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Datatypefield*_tmp173;struct Cyc_Absyn_Enumfield*_tmp172;struct Cyc_Absyn_Enumfield*_tmp171;struct Cyc_List_List*_tmp170;struct _tuple0*_tmp16F;struct Cyc_List_List*_tmp16E;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_Exp*_tmp16C;void*_tmp16B;struct Cyc_Absyn_Vardecl*_tmp16A;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_List_List*_tmp167;struct _tuple8*_tmp166;struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp161;struct _dyneither_ptr*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;struct _dyneither_ptr*_tmp15E;void*_tmp15D;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct _dyneither_ptr*_tmp15A;int _tmp159;struct _dyneither_ptr _tmp158;struct Cyc_Absyn_Exp*_tmp157;void*_tmp156;struct Cyc_Absyn_Exp*_tmp155;void*_tmp154;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp150;void*_tmp14F;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Core_Opt*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;enum Cyc_Absyn_Primop _tmp138;struct Cyc_List_List*_tmp137;struct _dyneither_ptr _tmp136;void*_tmp135;struct _dyneither_ptr _tmp134;struct _dyneither_ptr _tmp133;struct _dyneither_ptr _tmp132;enum Cyc_Absyn_Sign _tmp131;long long _tmp130;int _tmp12F;int _tmp12E;int _tmp12D;enum Cyc_Absyn_Sign _tmp12C;short _tmp12B;struct _dyneither_ptr _tmp12A;enum Cyc_Absyn_Sign _tmp129;char _tmp128;switch(*((int*)_tmpD8)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).String_c).tag){case 2U: _LL1: _tmp129=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Char_c).val).f1;_tmp128=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Char_c).val).f2;_LL2:
# 553
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp128));Cyc_Absyndump_dump_char((int)'\'');
goto _LL0;case 3U: _LL3: _tmp12A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Wchar_c).val;_LL4:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp2D7;_tmp2D7.tag=0U,_tmp2D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp12A);_tmp2D7;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({struct _dyneither_ptr _tmp309=({const char*_tmpDA="L'%s'";_tag_dyneither(_tmpDA,sizeof(char),6U);});Cyc_aprintf(_tmp309,_tag_dyneither(_tmpD9,sizeof(void*),1U));});}));goto _LL0;case 4U: _LL5: _tmp12C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Short_c).val).f1;_tmp12B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Short_c).val).f2;_LL6:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDE=({struct Cyc_Int_pa_PrintArg_struct _tmp2D8;_tmp2D8.tag=1U,_tmp2D8.f1=(unsigned long)((int)_tmp12B);_tmp2D8;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({struct _dyneither_ptr _tmp30A=({const char*_tmpDD="%d";_tag_dyneither(_tmpDD,sizeof(char),3U);});Cyc_aprintf(_tmp30A,_tag_dyneither(_tmpDC,sizeof(void*),1U));});}));goto _LL0;case 5U: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL7: _tmp12D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f2;_LL8:
 _tmp12E=_tmp12D;goto _LLA;case Cyc_Absyn_Signed: _LL9: _tmp12E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp2D9;_tmp2D9.tag=1U,_tmp2D9.f1=(unsigned long)_tmp12E;_tmp2D9;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _dyneither_ptr _tmp30B=({const char*_tmpE0="%d";_tag_dyneither(_tmpE0,sizeof(char),3U);});Cyc_aprintf(_tmp30B,_tag_dyneither(_tmpDF,sizeof(void*),1U));});}));goto _LL0;default: _LLB: _tmp12F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Int_c).val).f2;_LLC:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE4=({struct Cyc_Int_pa_PrintArg_struct _tmp2DA;_tmp2DA.tag=1U,_tmp2DA.f1=(unsigned)_tmp12F;_tmp2DA;});void*_tmpE2[1U];_tmpE2[0]=& _tmpE4;({struct _dyneither_ptr _tmp30C=({const char*_tmpE3="%uU";_tag_dyneither(_tmpE3,sizeof(char),4U);});Cyc_aprintf(_tmp30C,_tag_dyneither(_tmpE2,sizeof(void*),1U));});}));goto _LL0;}case 6U: _LLD: _tmp131=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).LongLong_c).val).f1;_tmp130=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).LongLong_c).val).f2;_LLE:
# 562
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp130));goto _LL0;case 7U: _LLF: _tmp132=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Float_c).val).f1;_LL10:
 Cyc_Absyndump_dump(_tmp132);goto _LL0;case 1U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmpE5="NULL";_tag_dyneither(_tmpE5,sizeof(char),5U);}));goto _LL0;case 8U: _LL13: _tmp133=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).String_c).val;_LL14:
# 566
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp133));Cyc_Absyndump_dump_char((int)'"');
goto _LL0;default: _LL15: _tmp134=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).Wstring_c).val;_LL16:
# 569
 Cyc_Absyndump_dump(({const char*_tmpE6="L\"";_tag_dyneither(_tmpE6,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(_tmp134);Cyc_Absyndump_dump_char((int)'"');goto _LL0;}case 1U: _LL17: _tmp135=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL18:
# 571
 Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(_tmp135));goto _LL0;case 2U: _LL19: _tmp136=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL1A:
# 574
 Cyc_Absyndump_dump(({const char*_tmpE7="__cyclone_pragma__(";_tag_dyneither(_tmpE7,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(_tmp136);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 3U: _LL1B: _tmp138=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp137=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL1C: {
# 577
struct _dyneither_ptr _tmpE8=Cyc_Absynpp_prim2str(_tmp138);
{int _tmpE9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp137);int _tmpEA=_tmpE9;switch(_tmpEA){case 1U: _LL70: _LL71:
# 580
 if((int)_tmp138 == (int)19U){
Cyc_Absyndump_dump(({const char*_tmpEB="numelts(";_tag_dyneither(_tmpEB,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp137))->hd);
Cyc_Absyndump_dump(({const char*_tmpEC=")";_tag_dyneither(_tmpEC,sizeof(char),2U);}));}else{
# 585
Cyc_Absyndump_dump(_tmpE8);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp137))->hd);}
# 588
goto _LL6F;case 2U: _LL72: _LL73:
# 590
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp137))->hd);
Cyc_Absyndump_dump(_tmpE8);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp137->tl))->hd);
goto _LL6F;default: _LL74: _LL75:
({void*_tmpED=0U;({struct _dyneither_ptr _tmp30D=({const char*_tmpEE="Absyndump -- Bad number of arguments to primop";_tag_dyneither(_tmpEE,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp30D,_tag_dyneither(_tmpED,sizeof(void*),0U));});});}_LL6F:;}
# 597
goto _LL0;}case 4U: _LL1D: _tmp13B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp13A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmp139=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL1E:
# 600
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp13B);
if(_tmp13A != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp13A->v));
Cyc_Absyndump_dump_nospace(({const char*_tmpEF="=";_tag_dyneither(_tmpEF,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp139);
goto _LL0;case 5U: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f2){case Cyc_Absyn_PreInc: _LL1F: _tmp13C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL20:
# 607
 Cyc_Absyndump_dump(({const char*_tmpF0="++";_tag_dyneither(_tmpF0,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp13C);goto _LL0;case Cyc_Absyn_PreDec: _LL21: _tmp13D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL22:
 Cyc_Absyndump_dump(({const char*_tmpF1="--";_tag_dyneither(_tmpF1,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp13D);goto _LL0;case Cyc_Absyn_PostInc: _LL23: _tmp13E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL24:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp13E);Cyc_Absyndump_dump(({const char*_tmpF2="++";_tag_dyneither(_tmpF2,sizeof(char),3U);}));goto _LL0;default: _LL25: _tmp13F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL26:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp13F);Cyc_Absyndump_dump(({const char*_tmpF3="--";_tag_dyneither(_tmpF3,sizeof(char),3U);}));goto _LL0;}case 6U: _LL27: _tmp142=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp141=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmp140=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL28:
# 613
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp142);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp141);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp140);
goto _LL0;case 7U: _LL29: _tmp144=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp143=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL2A:
# 620
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp144);Cyc_Absyndump_dump(({const char*_tmpF4="&&";_tag_dyneither(_tmpF4,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp143);goto _LL0;case 8U: _LL2B: _tmp146=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp145=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL2C:
# 624
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp146);Cyc_Absyndump_dump(({const char*_tmpF5="||";_tag_dyneither(_tmpF5,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp145);goto _LL0;case 9U: _LL2D: _tmp148=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp147=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL2E:
# 630
 Cyc_Absyndump_dumploc(e->loc);
# 633
Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp148);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp147);
goto _LL0;case 10U: _LL2F: _tmp14A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp149=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL30:
# 637
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp14A);
Cyc_Absyndump_dump_nospace(({const char*_tmpF6="(";_tag_dyneither(_tmpF6,sizeof(char),2U);}));{
# 642
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives && !(e->loc == (unsigned)0))&& !(_tmp14A->loc == (unsigned)0);
Cyc_Absyndump_dumpexps_prec(20,_tmp149);
Cyc_Absyndump_dump_nospace(({const char*_tmpF7=")";_tag_dyneither(_tmpF7,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;};case 11U: _LL31: _tmp14B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL32:
# 651
 Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(({const char*_tmpF8="throw";_tag_dyneither(_tmpF8,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp14B);goto _LL0;case 12U: _LL33: _tmp14C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL34:
# 653
 _tmp14D=_tmp14C;goto _LL36;case 13U: _LL35: _tmp14D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL36:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp14D);goto _LL0;case 14U: _LL37: _tmp14F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp14E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL38:
# 657
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp14F);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp14E);
goto _LL0;case 15U: _LL39: _tmp150=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL3A:
# 660
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp150);goto _LL0;case 20U: _LL3B: _tmp151=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL3C:
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp151);goto _LL0;case 16U: _LL3D: _tmp153=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp152=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL3E:
 Cyc_Absyndump_dump(({const char*_tmpF9="new";_tag_dyneither(_tmpF9,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp152);goto _LL0;case 17U: _LL3F: _tmp154=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL40:
 Cyc_Absyndump_dump(({const char*_tmpFA="sizeof(";_tag_dyneither(_tmpFA,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(_tmp154);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 18U: _LL41: _tmp155=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL42:
 Cyc_Absyndump_dump(({const char*_tmpFB="sizeof(";_tag_dyneither(_tmpFB,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp155);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 39U: _LL43: _tmp156=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL44:
 Cyc_Absyndump_dump(({const char*_tmpFC="valueof(";_tag_dyneither(_tmpFC,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp156);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 41U: _LL45: _tmp157=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL46:
# 668
 Cyc_Absyndump_dump(({const char*_tmpFD="__extension__(";_tag_dyneither(_tmpFD,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(_tmp157);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 40U: _LL47: _tmp159=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp158=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL48:
# 671
 Cyc_Absyndump_dump(({const char*_tmpFE="__asm__";_tag_dyneither(_tmpFE,sizeof(char),8U);}));
if(_tmp159)Cyc_Absyndump_dump(({const char*_tmpFF="volatile";_tag_dyneither(_tmpFF,sizeof(char),9U);}));
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dump_nospace(_tmp158);Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 38U: _LL49: _tmp15B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp15A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL4A:
# 677
 Cyc_Absyndump_dump(({const char*_tmp100="tagcheck(";_tag_dyneither(_tmp100,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(_tmp15B);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp15A);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 19U: _LL4B: _tmp15D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp15C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL4C:
# 682
 Cyc_Absyndump_dump(({const char*_tmp101="offsetof(";_tag_dyneither(_tmp101,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(_tmp15D);({struct Cyc_List_List*_tmp310=_tmp15C;struct _dyneither_ptr _tmp30F=({const char*_tmp102=",";_tag_dyneither(_tmp102,sizeof(char),2U);});struct _dyneither_ptr _tmp30E=({const char*_tmp103=")";_tag_dyneither(_tmp103,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpoffset_field,_tmp310,_tmp30F,_tmp30E,({const char*_tmp104=".";_tag_dyneither(_tmp104,sizeof(char),2U);}));});goto _LL0;case 21U: _LL4D: _tmp15F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp15E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL4E:
# 685
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp15F);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp15E);goto _LL0;case 22U: _LL4F: _tmp161=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp160=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL50:
# 687
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp161);Cyc_Absyndump_dump_nospace(({const char*_tmp105="->";_tag_dyneither(_tmp105,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*_tmp160);goto _LL0;case 23U: _LL51: _tmp163=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp162=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL52:
# 690
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp163);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp162);Cyc_Absyndump_dump_char((int)']');goto _LL0;case 24U: _LL53: _tmp164=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL54:
# 692
 Cyc_Absyndump_dump(({const char*_tmp106="$(";_tag_dyneither(_tmp106,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp164);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 25U: _LL55: _tmp166=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp165=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL56:
# 695
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp166).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp313=_tmp165;struct _dyneither_ptr _tmp312=({const char*_tmp107="{";_tag_dyneither(_tmp107,sizeof(char),2U);});struct _dyneither_ptr _tmp311=({const char*_tmp108="}";_tag_dyneither(_tmp108,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp313,_tmp312,_tmp311,({const char*_tmp109=",";_tag_dyneither(_tmp109,sizeof(char),2U);}));});
goto _LL0;case 26U: _LL57: _tmp167=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL58:
# 701
({struct Cyc_List_List*_tmp316=_tmp167;struct _dyneither_ptr _tmp315=({const char*_tmp10A="{";_tag_dyneither(_tmp10A,sizeof(char),2U);});struct _dyneither_ptr _tmp314=({const char*_tmp10B="}";_tag_dyneither(_tmp10B,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp316,_tmp315,_tmp314,({const char*_tmp10C=",";_tag_dyneither(_tmp10C,sizeof(char),2U);}));});goto _LL0;case 27U: _LL59: _tmp16A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp169=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmp168=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL5A:
# 704
 Cyc_Absyndump_dump(({const char*_tmp10D="{for";_tag_dyneither(_tmp10D,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*_tmp16A->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp169);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp168);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 28U: _LL5B: _tmp16C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp16B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL5C:
# 709
 Cyc_Absyndump_dump(({const char*_tmp10E="{for x ";_tag_dyneither(_tmp10E,sizeof(char),8U);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp16C);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp16B);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 29U: _LL5D: _tmp16F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp16E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmp16D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL5E:
# 714
 Cyc_Absyndump_dumpqvar(_tmp16F);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp16E != 0)
Cyc_Absyndump_dumptps(_tmp16E);
({struct Cyc_List_List*_tmp319=_tmp16D;struct _dyneither_ptr _tmp318=({const char*_tmp10F="";_tag_dyneither(_tmp10F,sizeof(char),1U);});struct _dyneither_ptr _tmp317=({const char*_tmp110="}";_tag_dyneither(_tmp110,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp319,_tmp318,_tmp317,({const char*_tmp111=",";_tag_dyneither(_tmp111,sizeof(char),2U);}));});
goto _LL0;case 30U: _LL5F: _tmp170=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL60:
# 721
({struct Cyc_List_List*_tmp31C=_tmp170;struct _dyneither_ptr _tmp31B=({const char*_tmp112="{";_tag_dyneither(_tmp112,sizeof(char),2U);});struct _dyneither_ptr _tmp31A=({const char*_tmp113="}";_tag_dyneither(_tmp113,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp31C,_tmp31B,_tmp31A,({const char*_tmp114=",";_tag_dyneither(_tmp114,sizeof(char),2U);}));});goto _LL0;case 32U: _LL61: _tmp171=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL62:
 _tmp172=_tmp171;goto _LL64;case 33U: _LL63: _tmp172=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL64:
 Cyc_Absyndump_dumpqvar(_tmp172->name);goto _LL0;case 31U: _LL65: _tmp174=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp173=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LL66:
# 726
 Cyc_Absyndump_dumpqvar(_tmp173->name);
if(_tmp174 != 0)({struct Cyc_List_List*_tmp31F=_tmp174;struct _dyneither_ptr _tmp31E=({const char*_tmp115="(";_tag_dyneither(_tmp115,sizeof(char),2U);});struct _dyneither_ptr _tmp31D=({const char*_tmp116=")";_tag_dyneither(_tmp116,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp31F,_tmp31E,_tmp31D,({const char*_tmp117=",";_tag_dyneither(_tmp117,sizeof(char),2U);}));});
goto _LL0;case 34U: _LL67: _tmp179=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).is_calloc;_tmp178=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).rgn;_tmp177=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).elt_type;_tmp176=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).num_elts;_tmp175=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).inline_call;_LL68:
# 731
 Cyc_Absyndump_dumploc(_tmp176->loc);
if(_tmp179){
if(_tmp178 != 0){
Cyc_Absyndump_dump(({const char*_tmp118="rcalloc(";_tag_dyneither(_tmp118,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(_tmp178);Cyc_Absyndump_dump(({const char*_tmp119=",";_tag_dyneither(_tmp119,sizeof(char),2U);}));}else{
# 736
Cyc_Absyndump_dump(({const char*_tmp11A="calloc(";_tag_dyneither(_tmp11A,sizeof(char),8U);}));}
# 738
Cyc_Absyndump_dumpexp(_tmp176);Cyc_Absyndump_dump(({const char*_tmp11B=",";_tag_dyneither(_tmp11B,sizeof(char),2U);}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(_tmp177)),0U));Cyc_Absyndump_dump(({const char*_tmp11C=")";_tag_dyneither(_tmp11C,sizeof(char),2U);}));}else{
# 740
if(_tmp178 != 0){
Cyc_Absyndump_dump(_tmp175?({const char*_tmp11D="rmalloc_inline(";_tag_dyneither(_tmp11D,sizeof(char),16U);}):({const char*_tmp11E="rmalloc(";_tag_dyneither(_tmp11E,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(_tmp178);Cyc_Absyndump_dump(({const char*_tmp11F=",";_tag_dyneither(_tmp11F,sizeof(char),2U);}));}else{
# 744
Cyc_Absyndump_dump(({const char*_tmp120="malloc(";_tag_dyneither(_tmp120,sizeof(char),8U);}));}
# 747
if(_tmp177 != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp320=Cyc_Absyn_sizeoftype_exp(*_tmp177,0U);Cyc_Absyn_times_exp(_tmp320,_tmp176,0U);}));else{
# 750
Cyc_Absyndump_dumpexp(_tmp176);}
Cyc_Absyndump_dump(({const char*_tmp121=")";_tag_dyneither(_tmp121,sizeof(char),2U);}));}
# 753
goto _LL0;case 35U: _LL69: _tmp17B=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp17A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL6A:
# 756
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17B);Cyc_Absyndump_dump_nospace(({const char*_tmp122=":=:";_tag_dyneither(_tmp122,sizeof(char),4U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp17A);goto _LL0;case 36U: _LL6B: _tmp17D=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmp17C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL6C:
# 759
({struct Cyc_List_List*_tmp323=_tmp17C;struct _dyneither_ptr _tmp322=({const char*_tmp123="{";_tag_dyneither(_tmp123,sizeof(char),2U);});struct _dyneither_ptr _tmp321=({const char*_tmp124="}";_tag_dyneither(_tmp124,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp323,_tmp322,_tmp321,({const char*_tmp125=",";_tag_dyneither(_tmp125,sizeof(char),2U);}));});goto _LL0;default: _LL6D: _tmp17E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL6E:
# 762
 Cyc_Absyndump_dump_nospace(({const char*_tmp126="({";_tag_dyneither(_tmp126,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp17E,1);Cyc_Absyndump_dump_nospace(({const char*_tmp127="})";_tag_dyneither(_tmp127,sizeof(char),3U);}));goto _LL0;}_LL0:;}
# 764
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 768
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 772
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp17F=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp17F->where_clause == 0 &&(_tmp17F->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp180="default:";_tag_dyneither(_tmp180,sizeof(char),9U);}));else{
# 778
Cyc_Absyndump_dump(({const char*_tmp181="case";_tag_dyneither(_tmp181,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(_tmp17F->pattern);
if(_tmp17F->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp182="&&";_tag_dyneither(_tmp182,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp17F->where_clause));}
# 784
Cyc_Absyndump_dump_nospace(({const char*_tmp183=":";_tag_dyneither(_tmp183,sizeof(char),2U);}));}
# 786
Cyc_Absyndump_dumpstmt(_tmp17F->body,0);}}
# 790
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp184=s->r;void*_tmp185=_tmp184;struct Cyc_List_List*_tmp1BB;struct _dyneither_ptr*_tmp1BA;struct Cyc_Absyn_Stmt*_tmp1B9;struct Cyc_Absyn_Decl*_tmp1B8;struct Cyc_Absyn_Stmt*_tmp1B7;struct Cyc_Absyn_Stmt*_tmp1B6;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct _dyneither_ptr*_tmp1B2;struct Cyc_Absyn_Stmt*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Stmt*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Stmt*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Stmt*_tmp1A8;struct Cyc_Absyn_Stmt*_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Stmt*_tmp1A5;struct Cyc_Absyn_Stmt*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A3;switch(*((int*)_tmp185)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1U: _LL3: _tmp1A3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_LL4:
 Cyc_Absyndump_dumpexp_prec(- 100,_tmp1A3);Cyc_Absyndump_dump_semi();goto _LL0;case 2U: _LL5: _tmp1A5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1A4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp185)->f2;_LL6:
# 799
 if(Cyc_Absynpp_is_declaration(_tmp1A5)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1A5,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 802
Cyc_Absyndump_dumpstmt(_tmp1A5,0);}
if(Cyc_Absynpp_is_declaration(_tmp1A4)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1A4,expstmt);Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 808
Cyc_Absyndump_dumpstmt(_tmp1A4,expstmt);}
goto _LL0;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp185)->f1 == 0){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp186="return;";_tag_dyneither(_tmp186,sizeof(char),8U);}));goto _LL0;}else{_LL9: _tmp1A6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_LLA:
 Cyc_Absyndump_dump(({const char*_tmp187="return";_tag_dyneither(_tmp187,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1A6));Cyc_Absyndump_dump_semi();goto _LL0;}case 4U: _LLB: _tmp1A9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1A8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp185)->f2;_tmp1A7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp185)->f3;_LLC:
# 813
 Cyc_Absyndump_dump(({const char*_tmp188="if(";_tag_dyneither(_tmp188,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(_tmp1A9);
{void*_tmp189=_tmp1A8->r;void*_tmp18A=_tmp189;switch(*((int*)_tmp18A)){case 2U: _LL26: _LL27:
 goto _LL29;case 12U: _LL28: _LL29:
 goto _LL2B;case 4U: _LL2A: _LL2B:
 goto _LL2D;case 13U: _LL2C: _LL2D:
# 819
 Cyc_Absyndump_dump_nospace(({const char*_tmp18B="){";_tag_dyneither(_tmp18B,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1A8,0);Cyc_Absyndump_dump_char((int)'}');goto _LL25;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1A8,0);}_LL25:;}
# 822
{void*_tmp18C=_tmp1A7->r;void*_tmp18D=_tmp18C;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp18D)->tag == 0U){_LL31: _LL32:
 goto _LL30;}else{_LL33: _LL34:
 Cyc_Absyndump_dump(({const char*_tmp18E="else{";_tag_dyneither(_tmp18E,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(_tmp1A7,0);Cyc_Absyndump_dump_char((int)'}');goto _LL30;}_LL30:;}
# 826
goto _LL0;case 5U: _LLD: _tmp1AB=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp185)->f1).f1;_tmp1AA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp185)->f2;_LLE:
# 829
 Cyc_Absyndump_dump(({const char*_tmp18F="while(";_tag_dyneither(_tmp18F,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(_tmp1AB);Cyc_Absyndump_dump_nospace(({const char*_tmp190="){";_tag_dyneither(_tmp190,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(_tmp1AA,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 9U: _LLF: _tmp1AF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1AE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp185)->f2).f1;_tmp1AD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp185)->f3).f1;_tmp1AC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp185)->f4;_LL10:
# 833
 Cyc_Absyndump_dump(({const char*_tmp191="for(";_tag_dyneither(_tmp191,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(_tmp1AF);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1AE);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1AD);
Cyc_Absyndump_dump_nospace(({const char*_tmp192="){";_tag_dyneither(_tmp192,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1AC,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 14U: _LL11: _tmp1B1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1B0=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp185)->f2).f1;_LL12:
# 837
 Cyc_Absyndump_dump(({const char*_tmp193="do{";_tag_dyneither(_tmp193,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1B1,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp194="}while(";_tag_dyneither(_tmp194,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1B0);Cyc_Absyndump_dump_nospace(({const char*_tmp195=");";_tag_dyneither(_tmp195,sizeof(char),3U);}));
goto _LL0;case 6U: _LL13: _LL14:
# 841
 Cyc_Absyndump_dump(({const char*_tmp196="break;";_tag_dyneither(_tmp196,sizeof(char),7U);}));goto _LL0;case 7U: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp197="continue;";_tag_dyneither(_tmp197,sizeof(char),10U);}));goto _LL0;case 8U: _LL17: _tmp1B2=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_LL18:
 Cyc_Absyndump_dump(({const char*_tmp198="goto";_tag_dyneither(_tmp198,sizeof(char),5U);}));Cyc_Absyndump_dump_str(_tmp1B2);Cyc_Absyndump_dump_semi();goto _LL0;case 10U: _LL19: _tmp1B4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1B3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp185)->f2;_LL1A:
# 846
 Cyc_Absyndump_dump(({const char*_tmp199="switch(";_tag_dyneither(_tmp199,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1B4);Cyc_Absyndump_dump_nospace(({const char*_tmp19A="){";_tag_dyneither(_tmp19A,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1B3);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 15U: _LL1B: _tmp1B6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1B5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp185)->f2;_LL1C:
# 851
 Cyc_Absyndump_dump(({const char*_tmp19B="try";_tag_dyneither(_tmp19B,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1B6,0);
Cyc_Absyndump_dump(({const char*_tmp19C="catch{";_tag_dyneither(_tmp19C,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1B5);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 12U: _LL1D: _tmp1B8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1B7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp185)->f2;_LL1E:
# 856
 Cyc_Absyndump_dumpdecl(_tmp1B8);Cyc_Absyndump_dumpstmt(_tmp1B7,expstmt);goto _LL0;case 13U: _LL1F: _tmp1BA=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_tmp1B9=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp185)->f2;_LL20:
# 862
 if(Cyc_Absynpp_is_declaration(_tmp1B9)){
Cyc_Absyndump_dump_str(_tmp1BA);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp19D=": ({";_tag_dyneither(_tmp19D,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp19E=": {";_tag_dyneither(_tmp19E,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(_tmp1B9,expstmt);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp19F="});}";_tag_dyneither(_tmp19F,sizeof(char),5U);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 868
Cyc_Absyndump_dump_str(_tmp1BA);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1B9,expstmt);}
# 870
goto _LL0;default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp185)->f1 == 0){_LL21: _LL22:
# 872
 Cyc_Absyndump_dump(({const char*_tmp1A0="fallthru;";_tag_dyneither(_tmp1A0,sizeof(char),10U);}));goto _LL0;}else{_LL23: _tmp1BB=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp185)->f1;_LL24:
# 874
 Cyc_Absyndump_dump(({const char*_tmp1A1="fallthru(";_tag_dyneither(_tmp1A1,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp1BB);Cyc_Absyndump_dump_nospace(({const char*_tmp1A2=");";_tag_dyneither(_tmp1A2,sizeof(char),3U);}));goto _LL0;}}_LL0:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 878
static void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
({struct Cyc_List_List*_tmp326=(*dp).f1;struct _dyneither_ptr _tmp325=({const char*_tmp1BC="";_tag_dyneither(_tmp1BC,sizeof(char),1U);});struct _dyneither_ptr _tmp324=({const char*_tmp1BD="=";_tag_dyneither(_tmp1BD,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp326,_tmp325,_tmp324,({const char*_tmp1BE="=";_tag_dyneither(_tmp1BE,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 882
static struct _dyneither_ptr Cyc_Absyndump_pat_term(int dots){return dots?({const char*_tmp1BF="...)";_tag_dyneither(_tmp1BF,sizeof(char),5U);}):({const char*_tmp1C0=")";_tag_dyneither(_tmp1C0,sizeof(char),2U);});}
# 884
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1C1=p->r;void*_tmp1C2=_tmp1C1;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_Absyn_Datatypefield*_tmp1FF;struct Cyc_List_List*_tmp1FE;int _tmp1FD;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FB;int _tmp1FA;union Cyc_Absyn_AggrInfo _tmp1F9;struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F7;int _tmp1F6;struct _tuple0*_tmp1F5;struct Cyc_List_List*_tmp1F4;int _tmp1F3;struct _tuple0*_tmp1F2;struct Cyc_Absyn_Tvar*_tmp1F1;struct Cyc_Absyn_Vardecl*_tmp1F0;struct Cyc_Absyn_Pat*_tmp1EF;struct Cyc_List_List*_tmp1EE;int _tmp1ED;struct Cyc_Absyn_Tvar*_tmp1EC;struct Cyc_Absyn_Vardecl*_tmp1EB;struct Cyc_Absyn_Vardecl*_tmp1EA;struct Cyc_Absyn_Pat*_tmp1E9;struct Cyc_Absyn_Vardecl*_tmp1E8;struct Cyc_Absyn_Vardecl*_tmp1E7;struct Cyc_Absyn_Pat*_tmp1E6;struct Cyc_Absyn_Vardecl*_tmp1E5;char _tmp1E4;struct Cyc_Absyn_Enumfield*_tmp1E3;struct Cyc_Absyn_Enumfield*_tmp1E2;struct _dyneither_ptr _tmp1E1;int _tmp1E0;int _tmp1DF;int _tmp1DE;switch(*((int*)_tmp1C2)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_char((int)'_');goto _LL0;case 9U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp1C3="NULL";_tag_dyneither(_tmp1C3,sizeof(char),5U);}));goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1){case Cyc_Absyn_None: _LL5: _tmp1DE=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL6:
 _tmp1DF=_tmp1DE;goto _LL8;case Cyc_Absyn_Signed: _LL7: _tmp1DF=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL8:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C6=({struct Cyc_Int_pa_PrintArg_struct _tmp2DB;_tmp2DB.tag=1U,_tmp2DB.f1=(unsigned long)_tmp1DF;_tmp2DB;});void*_tmp1C4[1U];_tmp1C4[0]=& _tmp1C6;({struct _dyneither_ptr _tmp327=({const char*_tmp1C5="%d";_tag_dyneither(_tmp1C5,sizeof(char),3U);});Cyc_aprintf(_tmp327,_tag_dyneither(_tmp1C4,sizeof(void*),1U));});}));goto _LL0;default: _LL9: _tmp1E0=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1C9=({struct Cyc_Int_pa_PrintArg_struct _tmp2DC;_tmp2DC.tag=1U,_tmp2DC.f1=(unsigned)_tmp1E0;_tmp2DC;});void*_tmp1C7[1U];_tmp1C7[0]=& _tmp1C9;({struct _dyneither_ptr _tmp328=({const char*_tmp1C8="%u";_tag_dyneither(_tmp1C8,sizeof(char),3U);});Cyc_aprintf(_tmp328,_tag_dyneither(_tmp1C7,sizeof(void*),1U));});}));goto _LL0;}case 12U: _LLB: _tmp1E1=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_LLC:
 Cyc_Absyndump_dump(_tmp1E1);goto _LL0;case 13U: _LLD: _tmp1E2=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LLE:
 _tmp1E3=_tmp1E2;goto _LL10;case 14U: _LLF: _tmp1E3=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL10:
 Cyc_Absyndump_dumpqvar(_tmp1E3->name);goto _LL0;case 11U: _LL11: _tmp1E4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_LL12:
# 895
 Cyc_Absyndump_dump(({const char*_tmp1CA="'";_tag_dyneither(_tmp1CA,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1E4));Cyc_Absyndump_dump_nospace(({const char*_tmp1CB="'";_tag_dyneither(_tmp1CB,sizeof(char),2U);}));goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2)->r)->tag == 0U){_LL13: _tmp1E5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_LL14:
 Cyc_Absyndump_dump(({const char*_tmp1CC="*";_tag_dyneither(_tmp1CC,sizeof(char),2U);}));_tmp1E8=_tmp1E5;goto _LL16;}else{_LL17: _tmp1E7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_tmp1E6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL18:
# 898
 Cyc_Absyndump_dump(({const char*_tmp1CD="*";_tag_dyneither(_tmp1CD,sizeof(char),2U);}));_tmp1EA=_tmp1E7;_tmp1E9=_tmp1E6;goto _LL1A;}case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2)->r)->tag == 0U){_LL15: _tmp1E8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_LL16:
# 897
 Cyc_Absyndump_dumpqvar(_tmp1E8->name);goto _LL0;}else{_LL19: _tmp1EA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_tmp1E9=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL1A:
# 899
 Cyc_Absyndump_dumpqvar(_tmp1EA->name);Cyc_Absyndump_dump(({const char*_tmp1CE=" as ";_tag_dyneither(_tmp1CE,sizeof(char),5U);}));Cyc_Absyndump_dumppat(_tmp1E9);goto _LL0;}case 2U: _LL1B: _tmp1EC=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_tmp1EB=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL1C:
# 901
 Cyc_Absyndump_dump(({const char*_tmp1CF="alias";_tag_dyneither(_tmp1CF,sizeof(char),6U);}));Cyc_Absyndump_dump(({const char*_tmp1D0="<";_tag_dyneither(_tmp1D0,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp1EC);Cyc_Absyndump_dump(({const char*_tmp1D1=">";_tag_dyneither(_tmp1D1,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(_tmp1EB,p->loc,0);
goto _LL0;case 5U: _LL1D: _tmp1EE=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_tmp1ED=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL1E:
({struct Cyc_List_List*_tmp32B=_tmp1EE;struct _dyneither_ptr _tmp32A=({const char*_tmp1D2="$(";_tag_dyneither(_tmp1D2,sizeof(char),3U);});struct _dyneither_ptr _tmp329=Cyc_Absyndump_pat_term(_tmp1ED);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp32B,_tmp32A,_tmp329,({const char*_tmp1D3=",";_tag_dyneither(_tmp1D3,sizeof(char),2U);}));});goto _LL0;case 6U: _LL1F: _tmp1EF=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_LL20:
 Cyc_Absyndump_dump(({const char*_tmp1D4="&";_tag_dyneither(_tmp1D4,sizeof(char),2U);}));Cyc_Absyndump_dumppat(_tmp1EF);goto _LL0;case 4U: _LL21: _tmp1F1=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_tmp1F0=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_LL22:
# 907
 Cyc_Absyndump_dumpqvar(_tmp1F0->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp1F1);Cyc_Absyndump_dump_char((int)'>');goto _LL0;case 15U: _LL23: _tmp1F2=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_LL24:
 Cyc_Absyndump_dumpqvar(_tmp1F2);goto _LL0;case 16U: _LL25: _tmp1F5=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_tmp1F4=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_tmp1F3=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1C2)->f3;_LL26:
# 910
 Cyc_Absyndump_dumpqvar(_tmp1F5);({struct Cyc_List_List*_tmp32E=_tmp1F4;struct _dyneither_ptr _tmp32D=({const char*_tmp1D5="(";_tag_dyneither(_tmp1D5,sizeof(char),2U);});struct _dyneither_ptr _tmp32C=Cyc_Absyndump_pat_term(_tmp1F3);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp32E,_tmp32D,_tmp32C,({const char*_tmp1D6=",";_tag_dyneither(_tmp1D6,sizeof(char),2U);}));});goto _LL0;case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1 != 0){_LL27: _tmp1F9=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_tmp1F8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_tmp1F7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f3;_tmp1F6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f4;_LL28:
# 912
 Cyc_Absyndump_dumpqvar((Cyc_Absyn_aggr_kinded_name(_tmp1F9)).f2);
_tmp1FC=_tmp1F8;_tmp1FB=_tmp1F7;_tmp1FA=_tmp1F6;goto _LL2A;}else{_LL29: _tmp1FC=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_tmp1FB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f3;_tmp1FA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C2)->f4;_LL2A:
# 915
 Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp331=_tmp1FC;struct _dyneither_ptr _tmp330=({const char*_tmp1D7="<";_tag_dyneither(_tmp1D7,sizeof(char),2U);});struct _dyneither_ptr _tmp32F=({const char*_tmp1D8=">";_tag_dyneither(_tmp1D8,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp331,_tmp330,_tmp32F,({const char*_tmp1D9=",";_tag_dyneither(_tmp1D9,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp334=_tmp1FB;struct _dyneither_ptr _tmp333=({const char*_tmp1DA="";_tag_dyneither(_tmp1DA,sizeof(char),1U);});struct _dyneither_ptr _tmp332=Cyc_Absyndump_pat_term(_tmp1FA);((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp334,_tmp333,_tmp332,({const char*_tmp1DB=",";_tag_dyneither(_tmp1DB,sizeof(char),2U);}));});
goto _LL0;}case 8U: _LL2B: _tmp1FF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C2)->f2;_tmp1FE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C2)->f3;_tmp1FD=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C2)->f4;_LL2C:
# 920
 Cyc_Absyndump_dumpqvar(_tmp1FF->name);
if(_tmp1FE != 0)({struct Cyc_List_List*_tmp337=_tmp1FE;struct _dyneither_ptr _tmp336=({const char*_tmp1DC="(";_tag_dyneither(_tmp1DC,sizeof(char),2U);});struct _dyneither_ptr _tmp335=Cyc_Absyndump_pat_term(_tmp1FD);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp337,_tmp336,_tmp335,({const char*_tmp1DD=",";_tag_dyneither(_tmp1DD,sizeof(char),2U);}));});
goto _LL0;default: _LL2D: _tmp200=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1C2)->f1;_LL2E:
 Cyc_Absyndump_dumpexp(_tmp200);goto _LL0;}_LL0:;}
# 927
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 932
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp338=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp338,({const char*_tmp201=",";_tag_dyneither(_tmp201,sizeof(char),2U);}));});}
# 935
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp202="=";_tag_dyneither(_tmp202,sizeof(char),2U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 941
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp339=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp339,({const char*_tmp203=",";_tag_dyneither(_tmp203,sizeof(char),2U);}));});}
# 945
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp204=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp205=_tmp204;struct _dyneither_ptr*_tmp20D;struct Cyc_Absyn_Tqual _tmp20C;void*_tmp20B;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Exp*_tmp208;_LL1: _tmp20D=_tmp205->name;_tmp20C=_tmp205->tq;_tmp20B=_tmp205->type;_tmp20A=_tmp205->width;_tmp209=_tmp205->attributes;_tmp208=_tmp205->requires_clause;_LL2:;
{enum Cyc_Cyclone_C_Compilers _tmp206=Cyc_Cyclone_c_compiler;if(_tmp206 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp20C,_tmp20B,Cyc_Absyndump_dump_str,_tmp20D);Cyc_Absyndump_dumpatts(_tmp209);goto _LL3;}else{_LL6: _LL7:
 Cyc_Absyndump_dumpatts(_tmp209);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp20C,_tmp20B,Cyc_Absyndump_dump_str,_tmp20D);goto _LL3;}_LL3:;}
# 952
if((unsigned)_tmp208){
Cyc_Absyndump_dump(({const char*_tmp207="@requires ";_tag_dyneither(_tmp207,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(_tmp208);}
# 955
if(_tmp20A != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp20A);}
# 958
Cyc_Absyndump_dump_semi();}}
# 962
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 967
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 971
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp20E=(*pr).f1;void*_tmp20F=_tmp20E;switch(*((int*)_tmp20F)){case 11U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp210="_stdcall";_tag_dyneither(_tmp210,sizeof(char),9U);}));goto _LL0;case 2U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp211="_cdecl";_tag_dyneither(_tmp211,sizeof(char),7U);}));goto _LL0;case 3U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp212="_fastcall";_tag_dyneither(_tmp212,sizeof(char),10U);}));goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 979
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 981
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv((fd->i).attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 985
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 989
static int Cyc_Absyndump_is_char_ptr(void*t){
# 991
void*_tmp213=t;void*_tmp218;void*_tmp217;switch(*((int*)_tmp213)){case 1U: _LL1: _tmp217=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp213)->f2;if(_tmp217 != 0){_LL2:
 return Cyc_Absyndump_is_char_ptr(_tmp217);}else{goto _LL5;}case 3U: _LL3: _tmp218=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213)->f1).elt_type;_LL4:
# 994
 while(1){
void*_tmp214=_tmp218;void*_tmp216;void*_tmp215;switch(*((int*)_tmp214)){case 1U: _LL8: _tmp215=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp214)->f2;if(_tmp215 != 0){_LL9:
 _tmp218=_tmp215;goto _LL7;}else{goto _LLE;}case 8U: _LLA: _tmp216=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp214)->f4;if(_tmp216 != 0){_LLB:
 _tmp218=_tmp216;goto _LL7;}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp214)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp214)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 1001
 return 0;}_LL0:;}
# 1005
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp219=vd;enum Cyc_Absyn_Scope _tmp22F;struct _tuple0*_tmp22E;unsigned _tmp22D;struct Cyc_Absyn_Tqual _tmp22C;void*_tmp22B;struct Cyc_Absyn_Exp*_tmp22A;struct Cyc_List_List*_tmp229;_LL1: _tmp22F=_tmp219->sc;_tmp22E=_tmp219->name;_tmp22D=_tmp219->varloc;_tmp22C=_tmp219->tq;_tmp22B=_tmp219->type;_tmp22A=_tmp219->initializer;_tmp229=_tmp219->attributes;_LL2:;
{enum Cyc_Cyclone_C_Compilers _tmp21A=Cyc_Cyclone_c_compiler;if(_tmp21A == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1010
 if((int)_tmp22F == (int)3U && Cyc_Absyndump_qvar_to_Cids){
void*_tmp21B=Cyc_Tcutil_compress(_tmp22B);void*_tmp21C=_tmp21B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21C)->tag == 5U){_LL9: _LLA:
 goto _LL8;}else{_LLB: _LLC:
 Cyc_Absyndump_dumpscope(_tmp22F);}_LL8:;}else{
# 1016
Cyc_Absyndump_dumpscope(_tmp22F);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp22C,_tmp22B,Cyc_Absyndump_dumpqvar,_tmp22E);
Cyc_Absyndump_dumpatts(_tmp229);
goto _LL3;}else{_LL6: _LL7:
# 1021
 Cyc_Absyndump_dumpatts(_tmp229);
Cyc_Absyndump_dumpscope(_tmp22F);{
struct _RegionHandle _tmp21D=_new_region("temp");struct _RegionHandle*temp=& _tmp21D;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(_tmp22B);
struct _tuple11 _tmp21E=Cyc_Absynpp_to_tms(temp,_tmp22C,_tmp22B);struct _tuple11 _tmp21F=_tmp21E;struct Cyc_Absyn_Tqual _tmp228;void*_tmp227;struct Cyc_List_List*_tmp226;_LLE: _tmp228=_tmp21F.f1;_tmp227=_tmp21F.f2;_tmp226=_tmp21F.f3;_LLF:;{
# 1027
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp226;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp220=(void*)tms2->hd;void*_tmp221=_tmp220;struct Cyc_List_List*_tmp224;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp221)->tag == 5U){_LL11: _tmp224=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp221)->f2;_LL12:
# 1031
 for(0;_tmp224 != 0;_tmp224=_tmp224->tl){
void*_tmp222=(void*)_tmp224->hd;void*_tmp223=_tmp222;switch(*((int*)_tmp223)){case 1U: _LL16: _LL17:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL15;case 2U: _LL18: _LL19:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL15;case 3U: _LL1A: _LL1B:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL15;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 1038
goto _LL10;}else{_LL13: _LL14:
 goto _LL10;}_LL10:;}}
# 1041
Cyc_Absyndump_dumptq(_tmp228);
Cyc_Absyndump_dumpntyp(_tmp227);{
struct _tuple19 _tmp225=({struct _tuple19 _tmp2DD;_tmp2DD.f1=call_conv,_tmp2DD.f2=_tmp22E;_tmp2DD;});
({int _tmp33A=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(_tmp33A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp226),Cyc_Absyndump_dump_callconv_qvar,& _tmp225);});
_npop_handler(0U);goto _LL3;};};}
# 1024
;_pop_region(temp);};}_LL3:;}
# 1048
if(_tmp22A != 0){
Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp22A);}
# 1051
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1054
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0 &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp230="@tagged ";_tag_dyneither(_tmp230,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl != 0){
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp33D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _dyneither_ptr _tmp33C=({const char*_tmp231="<";_tag_dyneither(_tmp231,sizeof(char),2U);});struct _dyneither_ptr _tmp33B=({const char*_tmp232=">";_tag_dyneither(_tmp232,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp33D,_tmp33C,_tmp33B,({const char*_tmp233=",";_tag_dyneither(_tmp233,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1065
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump_char((int)'}');
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1070
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp234=ed;enum Cyc_Absyn_Scope _tmp238;struct _tuple0*_tmp237;struct Cyc_Core_Opt*_tmp236;_LL1: _tmp238=_tmp234->sc;_tmp237=_tmp234->name;_tmp236=_tmp234->fields;_LL2:;
Cyc_Absyndump_dumpscope(_tmp238);Cyc_Absyndump_dump(({const char*_tmp235="enum ";_tag_dyneither(_tmp235,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(_tmp237);
if(_tmp236 != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp236->v);Cyc_Absyndump_dump_char((int)'}');}}
# 1077
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp239=dd;enum Cyc_Absyn_Scope _tmp240;struct _tuple0*_tmp23F;struct Cyc_List_List*_tmp23E;struct Cyc_Core_Opt*_tmp23D;int _tmp23C;_LL1: _tmp240=_tmp239->sc;_tmp23F=_tmp239->name;_tmp23E=_tmp239->tvs;_tmp23D=_tmp239->fields;_tmp23C=_tmp239->is_extensible;_LL2:;
Cyc_Absyndump_dumpscope(_tmp240);
if(_tmp23C)
Cyc_Absyndump_dump(({const char*_tmp23A="@extensible ";_tag_dyneither(_tmp23A,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp23B="datatype ";_tag_dyneither(_tmp23B,sizeof(char),10U);}));Cyc_Absyndump_dumpqvar(_tmp23F);Cyc_Absyndump_dumptvars(_tmp23E);
if(_tmp23D != 0){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp23D->v);Cyc_Absyndump_dump_char((int)'}');}}struct _tuple20{unsigned f1;struct _tuple0*f2;int f3;};
# 1088
static void Cyc_Absyndump_dumpexport(struct _tuple20*tup){
Cyc_Absyndump_dumpqvar((*tup).f2);}
# 1091
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char((int)'{');
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char((int)'}');}
# 1098
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp241=d->r;void*_tmp242=_tmp241;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp274;struct _tuple0*_tmp273;struct Cyc_List_List*_tmp272;struct _dyneither_ptr*_tmp271;struct Cyc_List_List*_tmp270;struct Cyc_Absyn_Typedefdecl*_tmp26F;struct Cyc_Absyn_Tvar*_tmp26E;struct Cyc_Absyn_Vardecl*_tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Pat*_tmp26B;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_List_List*_tmp269;struct Cyc_Absyn_Enumdecl*_tmp268;struct Cyc_Absyn_Datatypedecl*_tmp267;struct Cyc_Absyn_Aggrdecl*_tmp266;struct Cyc_Absyn_Vardecl*_tmp265;struct Cyc_Absyn_Fndecl*_tmp264;switch(*((int*)_tmp242)){case 1U: _LL1: _tmp264=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LL2:
# 1102
{enum Cyc_Cyclone_C_Compilers _tmp243=Cyc_Cyclone_c_compiler;if(_tmp243 == Cyc_Cyclone_Vc_c){_LL24: _LL25:
 Cyc_Absyndump_dumpatts((_tmp264->i).attributes);goto _LL23;}else{_LL26: _LL27:
 goto _LL23;}_LL23:;}
# 1106
if(_tmp264->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp244=Cyc_Cyclone_c_compiler;if(_tmp244 == Cyc_Cyclone_Vc_c){_LL29: _LL2A:
 Cyc_Absyndump_dump(({const char*_tmp245="__inline";_tag_dyneither(_tmp245,sizeof(char),9U);}));goto _LL28;}else{_LL2B: _LL2C:
 Cyc_Absyndump_dump(({const char*_tmp246="inline";_tag_dyneither(_tmp246,sizeof(char),7U);}));goto _LL28;}_LL28:;}
# 1111
Cyc_Absyndump_dumpscope(_tmp264->sc);{
# 1113
struct Cyc_Absyn_FnInfo type_info=_tmp264->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->tag=5U,_tmp24D->f1=type_info;_tmp24D;});
if(_tmp264->cached_type != 0){
void*_tmp247=Cyc_Tcutil_compress((void*)_check_null(_tmp264->cached_type));void*_tmp248=_tmp247;struct Cyc_Absyn_FnInfo _tmp24B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp248)->tag == 5U){_LL2E: _tmp24B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp248)->f1;_LL2F:
# 1119
({struct Cyc_List_List*_tmp33E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((_tmp264->i).attributes,_tmp24B.attributes);(_tmp264->i).attributes=_tmp33E;});goto _LL2D;}else{_LL30: _LL31:
({void*_tmp249=0U;({struct _dyneither_ptr _tmp33F=({const char*_tmp24A="function has non-function type";_tag_dyneither(_tmp24A,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp33F,_tag_dyneither(_tmp249,sizeof(void*),0U));});});}_LL2D:;}
# 1123
{enum Cyc_Cyclone_C_Compilers _tmp24C=Cyc_Cyclone_c_compiler;if(_tmp24C == Cyc_Cyclone_Gcc_c){_LL33: _LL34:
({struct Cyc_Absyn_Tqual _tmp341=Cyc_Absyn_empty_tqual(0U);void*_tmp340=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp341,_tmp340,Cyc_Absyndump_dump_atts_qvar,_tmp264);});goto _LL32;}else{_LL35: _LL36:
({struct Cyc_Absyn_Tqual _tmp343=Cyc_Absyn_empty_tqual(0U);void*_tmp342=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp343,_tmp342,Cyc_Absyndump_dump_callconv_fdqvar,_tmp264);});goto _LL32;}_LL32:;}
# 1127
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp264->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;};};case 0U: _LL3: _tmp265=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LL4:
 Cyc_Absyndump_dumpvardecl(_tmp265,d->loc,1);goto _LL0;case 5U: _LL5: _tmp266=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LL6:
 Cyc_Absyndump_dump_aggrdecl(_tmp266);Cyc_Absyndump_dump_semi();goto _LL0;case 6U: _LL7: _tmp267=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LL8:
 Cyc_Absyndump_dump_datatypedecl(_tmp267);Cyc_Absyndump_dump_semi();goto _LL0;case 7U: _LL9: _tmp268=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LLA:
 Cyc_Absyndump_dump_enumdecl(_tmp268);Cyc_Absyndump_dump_semi();goto _LL0;case 3U: _LLB: _tmp269=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LLC:
({struct Cyc_List_List*_tmp346=_tmp269;struct _dyneither_ptr _tmp345=({const char*_tmp24E=" let ";_tag_dyneither(_tmp24E,sizeof(char),6U);});struct _dyneither_ptr _tmp344=({const char*_tmp24F=";";_tag_dyneither(_tmp24F,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpid,_tmp346,_tmp345,_tmp344,({const char*_tmp250=",";_tag_dyneither(_tmp250,sizeof(char),2U);}));});goto _LL0;case 2U: _LLD: _tmp26B=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_tmp26A=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp242)->f3;_LLE:
# 1137
 Cyc_Absyndump_dump(({const char*_tmp251="let";_tag_dyneither(_tmp251,sizeof(char),4U);}));Cyc_Absyndump_dumppat(_tmp26B);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp26A);Cyc_Absyndump_dump_semi();goto _LL0;case 4U: _LLF: _tmp26E=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_tmp26D=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp242)->f2;_tmp26C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp242)->f3;_LL10:
# 1139
 Cyc_Absyndump_dump(({const char*_tmp252="region";_tag_dyneither(_tmp252,sizeof(char),7U);}));Cyc_Absyndump_dump(({const char*_tmp253="<";_tag_dyneither(_tmp253,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp26E);Cyc_Absyndump_dump(({const char*_tmp254="> ";_tag_dyneither(_tmp254,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(_tmp26D->name);
if((unsigned)_tmp26C){
Cyc_Absyndump_dump(({const char*_tmp255=" = open(";_tag_dyneither(_tmp255,sizeof(char),9U);}));Cyc_Absyndump_dumpexp(_tmp26C);Cyc_Absyndump_dump(({const char*_tmp256=")";_tag_dyneither(_tmp256,sizeof(char),2U);}));}
# 1143
Cyc_Absyndump_dump_semi();
goto _LL0;case 8U: _LL11: _tmp26F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LL12:
# 1150
 Cyc_Absyndump_dump(({const char*_tmp257="typedef";_tag_dyneither(_tmp257,sizeof(char),8U);}));{
void*t=_tmp26F->defn == 0?Cyc_Absyn_new_evar(_tmp26F->kind,0):(void*)_check_null(_tmp26F->defn);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp26F->tq,t,Cyc_Absyndump_dumptypedefname,_tmp26F);
Cyc_Absyndump_dumpatts(_tmp26F->atts);
Cyc_Absyndump_dump_semi();
# 1156
goto _LL0;};case 9U: _LL13: _tmp271=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_tmp270=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp242)->f2;_LL14:
# 1158
 Cyc_Absyndump_dump(({const char*_tmp258="namespace";_tag_dyneither(_tmp258,sizeof(char),10U);}));Cyc_Absyndump_dump_str(_tmp271);Cyc_Absyndump_dump_braced_decls(_tmp270);goto _LL0;case 10U: _LL15: _tmp273=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_tmp272=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp242)->f2;_LL16:
# 1160
 Cyc_Absyndump_dump(({const char*_tmp259="using";_tag_dyneither(_tmp259,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(_tmp273);Cyc_Absyndump_dump_braced_decls(_tmp272);goto _LL0;case 11U: _LL17: _tmp274=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_LL18:
# 1162
 Cyc_Absyndump_dump(({const char*_tmp25A="extern \"C\" ";_tag_dyneither(_tmp25A,sizeof(char),12U);}));Cyc_Absyndump_dump_braced_decls(_tmp274);goto _LL0;case 12U: _LL19: _tmp277=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp242)->f1;_tmp276=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp242)->f2;_tmp275=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp242)->f3;_LL1A:
# 1164
 Cyc_Absyndump_dump(({const char*_tmp25B="extern \"C include\" ";_tag_dyneither(_tmp25B,sizeof(char),20U);}));Cyc_Absyndump_dump_braced_decls(_tmp277);
if(_tmp276 != 0){
Cyc_Absyndump_dump(({const char*_tmp25C=" cyclone_override ";_tag_dyneither(_tmp25C,sizeof(char),19U);}));
Cyc_Absyndump_dump_braced_decls(_tmp276);}
# 1169
({struct Cyc_List_List*_tmp349=_tmp275;struct _dyneither_ptr _tmp348=({const char*_tmp25D=" export {";_tag_dyneither(_tmp25D,sizeof(char),10U);});struct _dyneither_ptr _tmp347=({const char*_tmp25E="}";_tag_dyneither(_tmp25E,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple20*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpexport,_tmp349,_tmp348,_tmp347,({const char*_tmp25F=",";_tag_dyneither(_tmp25F,sizeof(char),2U);}));});
goto _LL0;case 13U: _LL1B: _LL1C:
 Cyc_Absyndump_dump(({const char*_tmp260=" __cyclone_port_on__; ";_tag_dyneither(_tmp260,sizeof(char),23U);}));goto _LL0;case 14U: _LL1D: _LL1E:
 Cyc_Absyndump_dump(({const char*_tmp261=" __cyclone_port_off__; ";_tag_dyneither(_tmp261,sizeof(char),24U);}));goto _LL0;case 15U: _LL1F: _LL20:
 Cyc_Absyndump_dump(({const char*_tmp262=" __tempest_on__; ";_tag_dyneither(_tmp262,sizeof(char),18U);}));goto _LL0;default: _LL21: _LL22:
 Cyc_Absyndump_dump(({const char*_tmp263=" __tempest_off__; ";_tag_dyneither(_tmp263,sizeof(char),19U);}));goto _LL0;}_LL0:;};}
# 1178
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != (unsigned)1 || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1185
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1187
if(tms == 0){
f(a);
return;}{
# 1191
void*_tmp278=(void*)tms->hd;void*_tmp279=_tmp278;void*_tmp2A9;void*_tmp2A8;void*_tmp2A7;void*_tmp2A6;struct Cyc_Absyn_Tqual _tmp2A5;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp279)->tag == 2U){_LL1: _tmp2A9=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp279)->f1).rgn;_tmp2A8=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp279)->f1).nullable;_tmp2A7=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp279)->f1).bounds;_tmp2A6=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp279)->f1).zero_term;_tmp2A5=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp279)->f2;_LL2:
# 1195
{void*_tmp27A=Cyc_Tcutil_compress(_tmp2A7);void*_tmp27B=_tmp27A;struct Cyc_Absyn_Exp*_tmp27C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f1)){case 14U: _LL6: _LL7:
 Cyc_Absyndump_dump_char((int)'?');goto _LL5;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2)->hd)->tag == 9U){_LL8: _tmp27C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27B)->f2)->hd)->f1;_LL9:
# 1198
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,_tmp2A8)?'*':'@'));Cyc_Absyndump_dump_upperbound(_tmp27C);goto _LL5;}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA: _LLB:
# 1200
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,_tmp2A8)?'*':'@'));}_LL5:;}
# 1202
if((!Cyc_Absyndump_qvar_to_Cids && !is_char_ptr)&& Cyc_Absyn_type2bool(0,_tmp2A6))Cyc_Absyndump_dump(({const char*_tmp27D="@zeroterm";_tag_dyneither(_tmp27D,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids && is_char_ptr)&& !Cyc_Absyn_type2bool(0,_tmp2A6))Cyc_Absyndump_dump(({const char*_tmp27E="@nozeroterm";_tag_dyneither(_tmp27E,sizeof(char),12U);}));
{void*_tmp27F=Cyc_Tcutil_compress(_tmp2A9);void*_tmp280=_tmp27F;struct Cyc_Absyn_Tvar*_tmp288;switch(*((int*)_tmp280)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp280)->f1)){case 5U: _LLD: _LLE:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp281="`H";_tag_dyneither(_tmp281,sizeof(char),3U);}));goto _LLC;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp282="`U";_tag_dyneither(_tmp282,sizeof(char),3U);}));goto _LLC;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp283="`RC";_tag_dyneither(_tmp283,sizeof(char),4U);}));goto _LLC;default: goto _LL19;}case 2U: _LL13: _tmp288=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp280)->f1;_LL14:
 Cyc_Absyndump_dumptvar(_tmp288);goto _LLC;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f2 == 0){_LL15: _LL16:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2A9));goto _LLC;}else{goto _LL19;}case 8U: _LL17: _LL18:
 Cyc_Absyndump_dump(({const char*_tmp284="@region(";_tag_dyneither(_tmp284,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp2A9);Cyc_Absyndump_dump(({const char*_tmp285=")";_tag_dyneither(_tmp285,sizeof(char),2U);}));goto _LLC;default: _LL19: _LL1A:
({void*_tmp286=0U;({struct _dyneither_ptr _tmp34A=({const char*_tmp287="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp287,sizeof(char),37U);});Cyc_Warn_impos(_tmp34A,_tag_dyneither(_tmp286,sizeof(void*),0U));});});}_LLC:;}
# 1213
Cyc_Absyndump_dumptq(_tmp2A5);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL3: _LL4: {
# 1218
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp289=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp28A=_tmp289;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp28A)->tag == 2U){_LL1C: _LL1D:
 next_is_pointer=1;goto _LL1B;}else{_LL1E: _LL1F:
 goto _LL1B;}_LL1B:;}
# 1224
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp28B=(void*)tms->hd;void*_tmp28C=_tmp28B;struct Cyc_List_List*_tmp2A4;struct Cyc_List_List*_tmp2A3;unsigned _tmp2A2;int _tmp2A1;struct Cyc_List_List*_tmp2A0;unsigned _tmp29F;struct Cyc_List_List*_tmp29E;int _tmp29D;struct Cyc_Absyn_VarargInfo*_tmp29C;void*_tmp29B;struct Cyc_List_List*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;void*_tmp296;void*_tmp295;switch(*((int*)_tmp28C)){case 0U: _LL21: _tmp295=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1;_LL22:
# 1231
 Cyc_Absyndump_dump(({const char*_tmp28D="[]";_tag_dyneither(_tmp28D,sizeof(char),3U);}));
if(Cyc_Absyn_type2bool(0,_tmp295))Cyc_Absyndump_dump(({const char*_tmp28E="@zeroterm";_tag_dyneither(_tmp28E,sizeof(char),10U);}));
goto _LL20;case 1U: _LL23: _tmp297=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1;_tmp296=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp28C)->f2;_LL24:
# 1235
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp297);Cyc_Absyndump_dump_char((int)']');
if(Cyc_Absyn_type2bool(0,_tmp296))Cyc_Absyndump_dump(({const char*_tmp28F="@zeroterm";_tag_dyneither(_tmp28F,sizeof(char),10U);}));
goto _LL20;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->tag == 1U){_LL25: _tmp29E=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f1;_tmp29D=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f2;_tmp29C=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f3;_tmp29B=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f4;_tmp29A=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f5;_tmp299=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f6;_tmp298=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f7;_LL26:
# 1239
 Cyc_Absyndump_dumpfunargs(_tmp29E,_tmp29D,_tmp29C,_tmp29B,_tmp29A,_tmp299,_tmp298);goto _LL20;}else{_LL27: _tmp2A0=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f1;_tmp29F=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1)->f2;_LL28:
# 1241
({struct Cyc_List_List*_tmp34D=_tmp2A0;struct _dyneither_ptr _tmp34C=({const char*_tmp290="(";_tag_dyneither(_tmp290,sizeof(char),2U);});struct _dyneither_ptr _tmp34B=({const char*_tmp291=")";_tag_dyneither(_tmp291,sizeof(char),2U);});((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp34D,_tmp34C,_tmp34B,({const char*_tmp292=",";_tag_dyneither(_tmp292,sizeof(char),2U);}));});goto _LL20;}case 4U: _LL29: _tmp2A3=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp28C)->f1;_tmp2A2=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp28C)->f2;_tmp2A1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp28C)->f3;_LL2A:
# 1243
 if(_tmp2A1)Cyc_Absyndump_dumpkindedtvars(_tmp2A3);else{Cyc_Absyndump_dumptvars(_tmp2A3);}goto _LL20;case 5U: _LL2B: _tmp2A4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp28C)->f2;_LL2C:
 Cyc_Absyndump_dumpatts(_tmp2A4);goto _LL20;default: _LL2D: _LL2E:
({void*_tmp293=0U;({struct _dyneither_ptr _tmp34E=({const char*_tmp294="dumptms";_tag_dyneither(_tmp294,sizeof(char),8U);});Cyc_Warn_impos(_tmp34E,_tag_dyneither(_tmp293,sizeof(void*),0U));});});}_LL20:;}
# 1247
return;}}_LL0:;};}
# 1251
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2AA=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2AB=_new_region("temp");struct _RegionHandle*temp=& _tmp2AB;_push_region(temp);
{struct _tuple11 _tmp2AC=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2AD=_tmp2AC;struct Cyc_Absyn_Tqual _tmp2B0;void*_tmp2AF;struct Cyc_List_List*_tmp2AE;_LL1: _tmp2B0=_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;_tmp2AE=_tmp2AD.f3;_LL2:;
Cyc_Absyndump_dumptq(_tmp2B0);
Cyc_Absyndump_dumpntyp(_tmp2AF);
({int _tmp351=_tmp2AA;struct Cyc_List_List*_tmp350=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2AE);void(*_tmp34F)(void*)=f;Cyc_Absyndump_dumptms(_tmp351,_tmp350,_tmp34F,a);});}
# 1254
;_pop_region(temp);}
# 1260
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1262
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2B1=0U;({struct Cyc___cycFILE*_tmp353=f;struct _dyneither_ptr _tmp352=({const char*_tmp2B2="\n";_tag_dyneither(_tmp2B2,sizeof(char),2U);});Cyc_fprintf(_tmp353,_tmp352,_tag_dyneither(_tmp2B1,sizeof(void*),0U));});});}
# 1268
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2B3=d->r;void*_tmp2B4=_tmp2B3;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*_tmp2CF;struct Cyc_List_List*_tmp2CE;struct Cyc_List_List*_tmp2CD;struct Cyc_List_List*_tmp2CC;struct _dyneither_ptr*_tmp2CB;struct Cyc_List_List*_tmp2CA;struct Cyc_Absyn_Typedefdecl*_tmp2C9;struct Cyc_Absyn_Enumdecl*_tmp2C8;struct Cyc_Absyn_Datatypedecl*_tmp2C7;struct Cyc_Absyn_Aggrdecl*_tmp2C6;struct Cyc_Absyn_Fndecl*_tmp2C5;struct Cyc_Absyn_Vardecl*_tmp2C4;switch(*((int*)_tmp2B4)){case 0U: _LL1: _tmp2C4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_LL2:
# 1271
 if((int)_tmp2C4->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2C4->initializer;
_tmp2C4->initializer=0;
if((int)_tmp2C4->sc == (int)Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2B5="extern ";_tag_dyneither(_tmp2B5,sizeof(char),8U);}));
Cyc_Absyndump_dumpvardecl(_tmp2C4,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2B6="\n";_tag_dyneither(_tmp2B6,sizeof(char),2U);}));
_tmp2C4->initializer=init;
goto _LL0;};case 1U: _LL3: _tmp2C5=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_LL4:
# 1281
 if((int)_tmp2C5->sc == (int)Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2C5->sc);{
struct Cyc_Absyn_FnInfo type_info=_tmp2C5->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->tag=5U,_tmp2B8->f1=type_info;_tmp2B8;});
({struct Cyc_Absyn_Tqual _tmp355=Cyc_Absyn_empty_tqual(0U);void*_tmp354=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp355,_tmp354,Cyc_Absyndump_dump_atts_qvar,_tmp2C5);});
Cyc_Absyndump_dump(({const char*_tmp2B7=";\n";_tag_dyneither(_tmp2B7,sizeof(char),3U);}));
goto _LL0;};};case 5U: _LL5: _tmp2C6=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_LL6:
# 1290
 if((int)_tmp2C6->sc == (int)Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2C6->impl;
if((int)_tmp2C6->sc == (int)Cyc_Absyn_Abstract)_tmp2C6->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2C6);
_tmp2C6->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2B9=";\n";_tag_dyneither(_tmp2B9,sizeof(char),3U);}));
goto _LL0;};case 6U: _LL7: _tmp2C7=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_LL8:
 Cyc_Absyndump_dump_datatypedecl(_tmp2C7);Cyc_Absyndump_dump(({const char*_tmp2BA=";\n";_tag_dyneither(_tmp2BA,sizeof(char),3U);}));goto _LL0;case 7U: _LL9: _tmp2C8=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_LLA:
 Cyc_Absyndump_dump_enumdecl(_tmp2C8);Cyc_Absyndump_dump(({const char*_tmp2BB=";\n";_tag_dyneither(_tmp2BB,sizeof(char),3U);}));goto _LL0;case 8U: _LLB: _tmp2C9=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_LLC:
# 1300
 if(_tmp2C9->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2BC="\n";_tag_dyneither(_tmp2BC,sizeof(char),2U);}));}
# 1304
goto _LL0;case 9U: _LLD: _tmp2CB=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_tmp2CA=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2B4)->f2;_LLE:
# 1306
 Cyc_Absyndump_dump(({const char*_tmp2BD="namespace ";_tag_dyneither(_tmp2BD,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(_tmp2CB);
Cyc_Absyndump_dump(({const char*_tmp2BE="{\n";_tag_dyneither(_tmp2BE,sizeof(char),3U);}));
for(0;_tmp2CA != 0;_tmp2CA=_tmp2CA->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2CA->hd);}
Cyc_Absyndump_dump(({const char*_tmp2BF="}\n";_tag_dyneither(_tmp2BF,sizeof(char),3U);}));
goto _LL0;case 10U: _LLF: _tmp2CC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2B4)->f2;_LL10:
# 1314
 for(0;_tmp2CC != 0;_tmp2CC=_tmp2CC->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2CC->hd);}
goto _LL0;case 11U: _LL11: _tmp2CD=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_LL12:
# 1318
 Cyc_Absyndump_dump(({const char*_tmp2C0="extern \"C\" {";_tag_dyneither(_tmp2C0,sizeof(char),13U);}));
for(0;_tmp2CD != 0;_tmp2CD=_tmp2CD->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2CD->hd);}
Cyc_Absyndump_dump(({const char*_tmp2C1="}\n";_tag_dyneither(_tmp2C1,sizeof(char),3U);}));
goto _LL0;case 12U: _LL13: _tmp2D0=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2B4)->f1;_tmp2CF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2B4)->f2;_tmp2CE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2B4)->f3;_LL14:
# 1339 "absyndump.cyc"
 goto _LL0;default: _LL15: _LL16:
({void*_tmp2C2=0U;({struct _dyneither_ptr _tmp356=({const char*_tmp2C3="bad top-level declaration";_tag_dyneither(_tmp2C3,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp356,_tag_dyneither(_tmp2C2,sizeof(void*),0U));});});}_LL0:;}
# 1344
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
