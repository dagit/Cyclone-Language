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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned int nelts;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Buffer_t;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 320 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 322
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
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
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 51 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 53
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 61
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 76
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 39 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*requires,struct Cyc_Absyn_Exp*ensures);
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 67
static int Cyc_Absynpp_expand_typedefs;
# 71
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4U]="Cyc";
# 73
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4U};
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 76
static int Cyc_Absynpp_add_cyc_prefix;
# 80
static int Cyc_Absynpp_to_VC;
# 83
static int Cyc_Absynpp_decls_first;
# 87
static int Cyc_Absynpp_rewrite_temp_tvars;
# 90
static int Cyc_Absynpp_print_all_tvars;
# 93
static int Cyc_Absynpp_print_all_kinds;
# 96
static int Cyc_Absynpp_print_all_effects;
# 99
static int Cyc_Absynpp_print_using_stmts;
# 104
static int Cyc_Absynpp_print_externC_stmts;
# 108
static int Cyc_Absynpp_print_full_evars;
# 111
static int Cyc_Absynpp_generate_line_directives;
# 114
static int Cyc_Absynpp_use_curr_namespace;
# 117
static int Cyc_Absynpp_print_zeroterm;
# 120
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 123
int Cyc_Absynpp_print_for_cycdoc=0;
# 144
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 163
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 183
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 203
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 223
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 244
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp55D=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp55D,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
# 248
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 252
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 256
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 260
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){
char _tmp2=c;switch(_tmp2){case 7U: _LL1: _LL2:
 return({const char*_tmp3="\\a";_tag_dyneither(_tmp3,sizeof(char),3U);});case 8U: _LL3: _LL4:
 return({const char*_tmp4="\\b";_tag_dyneither(_tmp4,sizeof(char),3U);});case 12U: _LL5: _LL6:
 return({const char*_tmp5="\\f";_tag_dyneither(_tmp5,sizeof(char),3U);});case 10U: _LL7: _LL8:
 return({const char*_tmp6="\\n";_tag_dyneither(_tmp6,sizeof(char),3U);});case 13U: _LL9: _LLA:
 return({const char*_tmp7="\\r";_tag_dyneither(_tmp7,sizeof(char),3U);});case 9U: _LLB: _LLC:
 return({const char*_tmp8="\\t";_tag_dyneither(_tmp8,sizeof(char),3U);});case 11U: _LLD: _LLE:
 return({const char*_tmp9="\\v";_tag_dyneither(_tmp9,sizeof(char),3U);});case 92U: _LLF: _LL10:
 return({const char*_tmpA="\\\\";_tag_dyneither(_tmpA,sizeof(char),3U);});case 34U: _LL11: _LL12:
 return({const char*_tmpB="\"";_tag_dyneither(_tmpB,sizeof(char),2U);});case 39U: _LL13: _LL14:
 return({const char*_tmpC="\\'";_tag_dyneither(_tmpC,sizeof(char),3U);});default: _LL15: _LL16:
# 273
 if((int)c >= (int)' '  && (int)c <= (int)'~'){
struct _dyneither_ptr _tmpD=Cyc_Core_new_string(2U);
({struct _dyneither_ptr _tmpE=_dyneither_ptr_plus(_tmpD,sizeof(char),0);char _tmpF=*((char*)_check_dyneither_subscript(_tmpE,sizeof(char),0U));char _tmp10=c;if(_get_dyneither_size(_tmpE,sizeof(char))== 1U  && (_tmpF == 0  && _tmp10 != 0))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
return(struct _dyneither_ptr)_tmpD;}else{
# 278
struct _dyneither_ptr _tmp11=Cyc_Core_new_string(5U);
int j=0;
({struct _dyneither_ptr _tmp12=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp13=*((char*)_check_dyneither_subscript(_tmp12,sizeof(char),0U));char _tmp14='\\';if(_get_dyneither_size(_tmp12,sizeof(char))== 1U  && (_tmp13 == 0  && _tmp14 != 0))_throw_arraybounds();*((char*)_tmp12.curr)=_tmp14;});
({struct _dyneither_ptr _tmp15=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp16=*((char*)_check_dyneither_subscript(_tmp15,sizeof(char),0U));char _tmp17=(char)((int)'0' + ((int)((unsigned char)c)>> 6 & 3));if(_get_dyneither_size(_tmp15,sizeof(char))== 1U  && (_tmp16 == 0  && _tmp17 != 0))_throw_arraybounds();*((char*)_tmp15.curr)=_tmp17;});
({struct _dyneither_ptr _tmp18=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp19=*((char*)_check_dyneither_subscript(_tmp18,sizeof(char),0U));char _tmp1A=(char)((int)'0' + ((int)c >> 3 & 7));if(_get_dyneither_size(_tmp18,sizeof(char))== 1U  && (_tmp19 == 0  && _tmp1A != 0))_throw_arraybounds();*((char*)_tmp18.curr)=_tmp1A;});
({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(_tmp11,sizeof(char),j ++);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=(char)((int)'0' + ((int)c & 7));if(_get_dyneither_size(_tmp1B,sizeof(char))== 1U  && (_tmp1C == 0  && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
return(struct _dyneither_ptr)_tmp11;}}_LL0:;}
# 289
static int Cyc_Absynpp_special(struct _dyneither_ptr s){
int sz=(int)(_get_dyneither_size(s,sizeof(char))- (unsigned int)1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if((((int)c <= (int)' '  || (int)c >= (int)'~') || (int)c == (int)'"') || (int)c == (int)'\\')
return 1;}}
# 296
return 0;}
# 299
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 302
int n=(int)(_get_dyneither_size(s,sizeof(char))- (unsigned int)1);
# 304
if(n > 0  && (int)*((const char*)_check_dyneither_subscript(s,sizeof(char),n))== (int)'\000')-- n;{
# 306
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1F=_tmp1E;char _tmp20;switch(_tmp1F){case 7U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL8;case 10U: _LL7: _LL8:
 goto _LLA;case 13U: _LL9: _LLA:
 goto _LLC;case 9U: _LLB: _LLC:
 goto _LLE;case 11U: _LLD: _LLE:
 goto _LL10;case 92U: _LLF: _LL10:
 goto _LL12;case 34U: _LL11: _LL12:
 len +=2;goto _LL0;default: _LL13: _tmp20=_tmp1F;_LL14:
# 319
 if((int)_tmp20 >= (int)' '  && (int)_tmp20 <= (int)'~')++ len;else{
len +=4;}
goto _LL0;}_LL0:;}}{
# 324
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp21=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp22=_tmp21;char _tmp68;switch(_tmp22){case 7U: _LL16: _LL17:
({struct _dyneither_ptr _tmp23=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp24=*((char*)_check_dyneither_subscript(_tmp23,sizeof(char),0U));char _tmp25='\\';if(_get_dyneither_size(_tmp23,sizeof(char))== 1U  && (_tmp24 == 0  && _tmp25 != 0))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});({struct _dyneither_ptr _tmp26=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp27=*((char*)_check_dyneither_subscript(_tmp26,sizeof(char),0U));char _tmp28='a';if(_get_dyneither_size(_tmp26,sizeof(char))== 1U  && (_tmp27 == 0  && _tmp28 != 0))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});goto _LL15;case 8U: _LL18: _LL19:
({struct _dyneither_ptr _tmp29=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2A=*((char*)_check_dyneither_subscript(_tmp29,sizeof(char),0U));char _tmp2B='\\';if(_get_dyneither_size(_tmp29,sizeof(char))== 1U  && (_tmp2A == 0  && _tmp2B != 0))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});({struct _dyneither_ptr _tmp2C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2D=*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0U));char _tmp2E='b';if(_get_dyneither_size(_tmp2C,sizeof(char))== 1U  && (_tmp2D == 0  && _tmp2E != 0))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});goto _LL15;case 12U: _LL1A: _LL1B:
({struct _dyneither_ptr _tmp2F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0U));char _tmp31='\\';if(_get_dyneither_size(_tmp2F,sizeof(char))== 1U  && (_tmp30 == 0  && _tmp31 != 0))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0U));char _tmp34='f';if(_get_dyneither_size(_tmp32,sizeof(char))== 1U  && (_tmp33 == 0  && _tmp34 != 0))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});goto _LL15;case 10U: _LL1C: _LL1D:
({struct _dyneither_ptr _tmp35=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp36=*((char*)_check_dyneither_subscript(_tmp35,sizeof(char),0U));char _tmp37='\\';if(_get_dyneither_size(_tmp35,sizeof(char))== 1U  && (_tmp36 == 0  && _tmp37 != 0))_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});({struct _dyneither_ptr _tmp38=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp39=*((char*)_check_dyneither_subscript(_tmp38,sizeof(char),0U));char _tmp3A='n';if(_get_dyneither_size(_tmp38,sizeof(char))== 1U  && (_tmp39 == 0  && _tmp3A != 0))_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});goto _LL15;case 13U: _LL1E: _LL1F:
({struct _dyneither_ptr _tmp3B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3C=*((char*)_check_dyneither_subscript(_tmp3B,sizeof(char),0U));char _tmp3D='\\';if(_get_dyneither_size(_tmp3B,sizeof(char))== 1U  && (_tmp3C == 0  && _tmp3D != 0))_throw_arraybounds();*((char*)_tmp3B.curr)=_tmp3D;});({struct _dyneither_ptr _tmp3E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3F=*((char*)_check_dyneither_subscript(_tmp3E,sizeof(char),0U));char _tmp40='r';if(_get_dyneither_size(_tmp3E,sizeof(char))== 1U  && (_tmp3F == 0  && _tmp40 != 0))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});goto _LL15;case 9U: _LL20: _LL21:
({struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0U));char _tmp43='\\';if(_get_dyneither_size(_tmp41,sizeof(char))== 1U  && (_tmp42 == 0  && _tmp43 != 0))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});({struct _dyneither_ptr _tmp44=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp45=*((char*)_check_dyneither_subscript(_tmp44,sizeof(char),0U));char _tmp46='t';if(_get_dyneither_size(_tmp44,sizeof(char))== 1U  && (_tmp45 == 0  && _tmp46 != 0))_throw_arraybounds();*((char*)_tmp44.curr)=_tmp46;});goto _LL15;case 11U: _LL22: _LL23:
({struct _dyneither_ptr _tmp47=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp48=*((char*)_check_dyneither_subscript(_tmp47,sizeof(char),0U));char _tmp49='\\';if(_get_dyneither_size(_tmp47,sizeof(char))== 1U  && (_tmp48 == 0  && _tmp49 != 0))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});({struct _dyneither_ptr _tmp4A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4B=*((char*)_check_dyneither_subscript(_tmp4A,sizeof(char),0U));char _tmp4C='v';if(_get_dyneither_size(_tmp4A,sizeof(char))== 1U  && (_tmp4B == 0  && _tmp4C != 0))_throw_arraybounds();*((char*)_tmp4A.curr)=_tmp4C;});goto _LL15;case 92U: _LL24: _LL25:
({struct _dyneither_ptr _tmp4D=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4E=*((char*)_check_dyneither_subscript(_tmp4D,sizeof(char),0U));char _tmp4F='\\';if(_get_dyneither_size(_tmp4D,sizeof(char))== 1U  && (_tmp4E == 0  && _tmp4F != 0))_throw_arraybounds();*((char*)_tmp4D.curr)=_tmp4F;});({struct _dyneither_ptr _tmp50=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp51=*((char*)_check_dyneither_subscript(_tmp50,sizeof(char),0U));char _tmp52='\\';if(_get_dyneither_size(_tmp50,sizeof(char))== 1U  && (_tmp51 == 0  && _tmp52 != 0))_throw_arraybounds();*((char*)_tmp50.curr)=_tmp52;});goto _LL15;case 34U: _LL26: _LL27:
({struct _dyneither_ptr _tmp53=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp54=*((char*)_check_dyneither_subscript(_tmp53,sizeof(char),0U));char _tmp55='\\';if(_get_dyneither_size(_tmp53,sizeof(char))== 1U  && (_tmp54 == 0  && _tmp55 != 0))_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});({struct _dyneither_ptr _tmp56=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp57=*((char*)_check_dyneither_subscript(_tmp56,sizeof(char),0U));char _tmp58='"';if(_get_dyneither_size(_tmp56,sizeof(char))== 1U  && (_tmp57 == 0  && _tmp58 != 0))_throw_arraybounds();*((char*)_tmp56.curr)=_tmp58;});goto _LL15;default: _LL28: _tmp68=_tmp22;_LL29:
# 338
 if((int)_tmp68 >= (int)' '  && (int)_tmp68 <= (int)'~')({struct _dyneither_ptr _tmp59=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5A=*((char*)_check_dyneither_subscript(_tmp59,sizeof(char),0U));char _tmp5B=_tmp68;if(_get_dyneither_size(_tmp59,sizeof(char))== 1U  && (_tmp5A == 0  && _tmp5B != 0))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});else{
# 340
({struct _dyneither_ptr _tmp5C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5D=*((char*)_check_dyneither_subscript(_tmp5C,sizeof(char),0U));char _tmp5E='\\';if(_get_dyneither_size(_tmp5C,sizeof(char))== 1U  && (_tmp5D == 0  && _tmp5E != 0))_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});
({struct _dyneither_ptr _tmp5F=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp60=*((char*)_check_dyneither_subscript(_tmp5F,sizeof(char),0U));char _tmp61=(char)((int)'0' + ((int)_tmp68 >> 6 & 7));if(_get_dyneither_size(_tmp5F,sizeof(char))== 1U  && (_tmp60 == 0  && _tmp61 != 0))_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});
({struct _dyneither_ptr _tmp62=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp63=*((char*)_check_dyneither_subscript(_tmp62,sizeof(char),0U));char _tmp64=(char)((int)'0' + ((int)_tmp68 >> 3 & 7));if(_get_dyneither_size(_tmp62,sizeof(char))== 1U  && (_tmp63 == 0  && _tmp64 != 0))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;});
({struct _dyneither_ptr _tmp65=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp66=*((char*)_check_dyneither_subscript(_tmp65,sizeof(char),0U));char _tmp67=(char)((int)'0' + ((int)_tmp68 & 7));if(_get_dyneither_size(_tmp65,sizeof(char))== 1U  && (_tmp66 == 0  && _tmp67 != 0))_throw_arraybounds();*((char*)_tmp65.curr)=_tmp67;});}
# 345
goto _LL15;}_LL15:;}}
# 347
return(struct _dyneither_ptr)t;};};};}static char _tmp69[9U]="restrict";
# 350
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp69,_tmp69,_tmp69 + 9U};static char _tmp6A[9U]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp6A,_tmp6A,_tmp6A + 9U};static char _tmp6B[6U]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6B,_tmp6B,_tmp6B + 6U};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 357
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 360
if(tq.q_restrict)l=({struct Cyc_List_List*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->hd=Cyc_Absynpp_restrict_sp,_tmp6C->tl=l;_tmp6C;});
if(tq.q_volatile)l=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->hd=Cyc_Absynpp_volatile_sp,_tmp6D->tl=l;_tmp6D;});
if(tq.print_const)l=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=Cyc_Absynpp_const_sp,_tmp6E->tl=l;_tmp6E;});
return({struct _dyneither_ptr _tmp560=({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1U);});struct _dyneither_ptr _tmp55F=({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2U);});struct _dyneither_ptr _tmp55E=({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp560,_tmp55F,_tmp55E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
# 366
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp72=ka;enum Cyc_Absyn_KindQual _tmp89;enum Cyc_Absyn_AliasQual _tmp88;_LL1: _tmp89=_tmp72->kind;_tmp88=_tmp72->aliasqual;_LL2:;{
enum Cyc_Absyn_KindQual _tmp73=_tmp89;switch(_tmp73){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 370
enum Cyc_Absyn_AliasQual _tmp74=_tmp88;switch(_tmp74){case Cyc_Absyn_Aliasable: _LL17: _LL18:
 return({const char*_tmp75="A";_tag_dyneither(_tmp75,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL19: _LL1A:
 return({const char*_tmp76="UA";_tag_dyneither(_tmp76,sizeof(char),3U);});case Cyc_Absyn_Top: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return({const char*_tmp77="TA";_tag_dyneither(_tmp77,sizeof(char),3U);});}_LL16:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 376
enum Cyc_Absyn_AliasQual _tmp78=_tmp88;switch(_tmp78){case Cyc_Absyn_Aliasable: _LL20: _LL21:
 return({const char*_tmp79="M";_tag_dyneither(_tmp79,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL22: _LL23:
 return({const char*_tmp7A="UM";_tag_dyneither(_tmp7A,sizeof(char),3U);});case Cyc_Absyn_Top: _LL24: _LL25:
 goto _LL27;default: _LL26: _LL27: return({const char*_tmp7B="TM";_tag_dyneither(_tmp7B,sizeof(char),3U);});}_LL1F:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 382
enum Cyc_Absyn_AliasQual _tmp7C=_tmp88;switch(_tmp7C){case Cyc_Absyn_Aliasable: _LL29: _LL2A:
 return({const char*_tmp7D="B";_tag_dyneither(_tmp7D,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL2B: _LL2C:
 return({const char*_tmp7E="UB";_tag_dyneither(_tmp7E,sizeof(char),3U);});case Cyc_Absyn_Top: _LL2D: _LL2E:
 goto _LL30;default: _LL2F: _LL30: return({const char*_tmp7F="TB";_tag_dyneither(_tmp7F,sizeof(char),3U);});}_LL28:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 388
enum Cyc_Absyn_AliasQual _tmp80=_tmp88;switch(_tmp80){case Cyc_Absyn_Aliasable: _LL32: _LL33:
 return({const char*_tmp81="R";_tag_dyneither(_tmp81,sizeof(char),2U);});case Cyc_Absyn_Unique: _LL34: _LL35:
 return({const char*_tmp82="UR";_tag_dyneither(_tmp82,sizeof(char),3U);});case Cyc_Absyn_Top: _LL36: _LL37:
 goto _LL39;default: _LL38: _LL39: return({const char*_tmp83="TR";_tag_dyneither(_tmp83,sizeof(char),3U);});}_LL31:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 393
 return({const char*_tmp84="E";_tag_dyneither(_tmp84,sizeof(char),2U);});case Cyc_Absyn_IntKind: _LLE: _LLF:
 return({const char*_tmp85="I";_tag_dyneither(_tmp85,sizeof(char),2U);});case Cyc_Absyn_BoolKind: _LL10: _LL11:
 return({const char*_tmp86="BOOL";_tag_dyneither(_tmp86,sizeof(char),5U);});case Cyc_Absyn_PtrBndKind: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15: return({const char*_tmp87="PTRBND";_tag_dyneither(_tmp87,sizeof(char),7U);});}_LL3:;};}
# 399
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 401
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp8A=Cyc_Absyn_compress_kb(c);void*_tmp8B=_tmp8A;struct Cyc_Absyn_Kind*_tmp92;struct Cyc_Absyn_Kind*_tmp91;switch(*((int*)_tmp8B)){case 0U: _LL1: _tmp91=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B)->f1;_LL2:
 return Cyc_Absynpp_kind2string(_tmp91);case 1U: _LL3: _LL4:
# 405
 if(Cyc_PP_tex_output)
return({const char*_tmp8C="{?}";_tag_dyneither(_tmp8C,sizeof(char),4U);});else{
return({const char*_tmp8D="?";_tag_dyneither(_tmp8D,sizeof(char),2U);});}default: _LL5: _tmp92=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B)->f2;_LL6:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,({struct _dyneither_ptr _tmp561=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp92));_tmp541.f1=_tmp561;});_tmp541;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({struct _dyneither_ptr _tmp562=({const char*_tmp8F="<=%s";_tag_dyneither(_tmp8F,sizeof(char),5U);});Cyc_aprintf(_tmp562,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});}_LL0:;}
# 412
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp93=Cyc_Absyn_compress_kb(c);void*_tmp94=_tmp93;struct Cyc_Absyn_Kind*_tmp98;struct Cyc_Absyn_Kind*_tmp97;switch(*((int*)_tmp94)){case 0U: _LL1: _tmp97=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94)->f1;_LL2:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp97));case 1U: _LL3: _LL4:
# 416
 if(Cyc_PP_tex_output)
return Cyc_PP_text_width(({const char*_tmp95="{?}";_tag_dyneither(_tmp95,sizeof(char),4U);}),1);else{
return Cyc_PP_text(({const char*_tmp96="?";_tag_dyneither(_tmp96,sizeof(char),2U);}));}default: _LL5: _tmp98=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94)->f2;_LL6:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp98));}_LL0:;}
# 423
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp565=({const char*_tmp99="<";_tag_dyneither(_tmp99,sizeof(char),2U);});struct _dyneither_ptr _tmp564=({const char*_tmp9A=">";_tag_dyneither(_tmp9A,sizeof(char),2U);});struct _dyneither_ptr _tmp563=({const char*_tmp9B=",";_tag_dyneither(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp565,_tmp564,_tmp563,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
# 427
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9C=*tv->name;
# 431
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 433
struct _dyneither_ptr kstring=({const char*_tmpA5="K";_tag_dyneither(_tmpA5,sizeof(char),2U);});
{void*_tmp9D=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9E=_tmp9D;struct Cyc_Absyn_Kind*_tmpA0;struct Cyc_Absyn_Kind*_tmp9F;switch(*((int*)_tmp9E)){case 2U: _LL1: _tmp9F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E)->f2;_LL2:
 _tmpA0=_tmp9F;goto _LL4;case 0U: _LL3: _tmpA0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9E)->f1;_LL4:
 kstring=Cyc_Absynpp_kind2string(_tmpA0);goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 439
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,_tmp543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);_tmp543;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,({struct _dyneither_ptr _tmp566=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9C,sizeof(char),1));_tmp542.f1=_tmp566;});_tmp542;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _dyneither_ptr _tmp567=({const char*_tmpA2="`G%s%s";_tag_dyneither(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp567,_tag_dyneither(_tmpA1,sizeof(void*),2U));});}));}
# 441
return Cyc_PP_text(_tmp9C);}
# 444
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAA;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpAA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8:
# 449
 return({struct Cyc_PP_Doc*_tmpA8[3U];({struct Cyc_PP_Doc*_tmp56A=Cyc_Absynpp_tvar2doc(tv);_tmpA8[0]=_tmp56A;}),({struct Cyc_PP_Doc*_tmp569=Cyc_PP_text(({const char*_tmpA9="::";_tag_dyneither(_tmpA9,sizeof(char),3U);}));_tmpA8[1]=_tmp569;}),({struct Cyc_PP_Doc*_tmp568=Cyc_Absynpp_kind2doc(_tmpAA);_tmpA8[2]=_tmp568;});Cyc_PP_cat(_tag_dyneither(_tmpA8,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL5: _tmpAB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6:
# 448
 _tmpAA=_tmpAB;goto _LL8;}_LL0:;}
# 453
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _dyneither_ptr _tmp56D=({const char*_tmpAC="<";_tag_dyneither(_tmpAC,sizeof(char),2U);});struct _dyneither_ptr _tmp56C=({const char*_tmpAD=">";_tag_dyneither(_tmpAD,sizeof(char),2U);});struct _dyneither_ptr _tmp56B=({const char*_tmpAE=",";_tag_dyneither(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp56D,_tmp56C,_tmp56B,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 457
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _dyneither_ptr _tmp570=({const char*_tmpAF="<";_tag_dyneither(_tmpAF,sizeof(char),2U);});struct _dyneither_ptr _tmp56F=({const char*_tmpB0=">";_tag_dyneither(_tmpB0,sizeof(char),2U);});struct _dyneither_ptr _tmp56E=({const char*_tmpB1=",";_tag_dyneither(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp570,_tmp56F,_tmp56E,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 463
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 467
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp573=({const char*_tmpB2="(";_tag_dyneither(_tmpB2,sizeof(char),2U);});struct _dyneither_ptr _tmp572=({const char*_tmpB3=")";_tag_dyneither(_tmpB3,sizeof(char),2U);});struct _dyneither_ptr _tmp571=({const char*_tmpB4=",";_tag_dyneither(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp573,_tmp572,_tmp571,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
# 471
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB5=att;switch(*((int*)_tmpB5)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();default: _LL7: _LL8:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));}_LL0:;}
# 480
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpB6=(void*)atts->hd;void*_tmpB7=_tmpB6;switch(*((int*)_tmpB7)){case 1U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmpB8=" _stdcall ";_tag_dyneither(_tmpB8,sizeof(char),11U);}));case 2U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmpB9=" _cdecl ";_tag_dyneither(_tmpB9,sizeof(char),9U);}));case 3U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmpBA=" _fastcall ";_tag_dyneither(_tmpBA,sizeof(char),12U);}));default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 488
return Cyc_PP_nil_doc();}
# 491
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpBB=(void*)atts2->hd;void*_tmpBC=_tmpBB;switch(*((int*)_tmpBC)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 500
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp579=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_dyneither(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp579;}),({
struct Cyc_PP_Doc*_tmp578=({struct _dyneither_ptr _tmp577=({const char*_tmpBF="";_tag_dyneither(_tmpBF,sizeof(char),1U);});struct _dyneither_ptr _tmp576=({const char*_tmpC0="";_tag_dyneither(_tmpC0,sizeof(char),1U);});struct _dyneither_ptr _tmp575=({const char*_tmpC1=" ";_tag_dyneither(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp577,_tmp576,_tmp575,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp578;}),({
struct Cyc_PP_Doc*_tmp574=Cyc_PP_text(({const char*_tmpC2=")";_tag_dyneither(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp574;});Cyc_PP_cat(_tag_dyneither(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 507
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 511
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC3=Cyc_Cyclone_c_compiler;if(_tmpC3 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 516
 return({struct Cyc_PP_Doc*_tmpC4[2U];({struct Cyc_PP_Doc*_tmp57E=Cyc_PP_text(({const char*_tmpC5=" __attribute__";_tag_dyneither(_tmpC5,sizeof(char),15U);}));_tmpC4[0]=_tmp57E;}),({
struct Cyc_PP_Doc*_tmp57D=({struct _dyneither_ptr _tmp57C=({const char*_tmpC6="((";_tag_dyneither(_tmpC6,sizeof(char),3U);});struct _dyneither_ptr _tmp57B=({const char*_tmpC7="))";_tag_dyneither(_tmpC7,sizeof(char),3U);});struct _dyneither_ptr _tmp57A=({const char*_tmpC8=",";_tag_dyneither(_tmpC8,sizeof(char),2U);});Cyc_PP_group(_tmp57C,_tmp57B,_tmp57A,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC4[1]=_tmp57D;});Cyc_PP_cat(_tag_dyneither(_tmpC4,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;};}
# 521
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC9=(void*)tms->hd;void*_tmpCA=_tmpC9;switch(*((int*)_tmpCA)){case 2U: _LL1: _LL2:
 return 1;case 5U: _LL3: _LL4: {
# 526
enum Cyc_Cyclone_C_Compilers _tmpCB=Cyc_Cyclone_c_compiler;if(_tmpCB == Cyc_Cyclone_Gcc_c){_LL8: _LL9:
 return 0;}else{_LLA: _LLB:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL7:;}default: _LL5: _LL6:
# 530
 return 0;}_LL0:;};}
# 534
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 537
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=Cyc_PP_text_width(({const char*_tmpCC="{?}";_tag_dyneither(_tmpCC,sizeof(char),4U);}),1);else{
Cyc_Absynpp_cache_question=Cyc_PP_text(({const char*_tmpCD="?";_tag_dyneither(_tmpCD,sizeof(char),2U);}));}}
# 544
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 546
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_lb=Cyc_PP_text_width(({const char*_tmpCE="{\\lb}";_tag_dyneither(_tmpCE,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_lb=Cyc_PP_text(({const char*_tmpCF="{";_tag_dyneither(_tmpCF,sizeof(char),2U);}));}}
# 553
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 555
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_rb=Cyc_PP_text_width(({const char*_tmpD0="{\\rb}";_tag_dyneither(_tmpD0,sizeof(char),6U);}),1);else{
Cyc_Absynpp_cache_rb=Cyc_PP_text(({const char*_tmpD1="}";_tag_dyneither(_tmpD1,sizeof(char),2U);}));}}
# 562
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 564
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(({const char*_tmpD2="\\$";_tag_dyneither(_tmpD2,sizeof(char),3U);}),1);else{
Cyc_Absynpp_cache_dollar=Cyc_PP_text(({const char*_tmpD3="$";_tag_dyneither(_tmpD3,sizeof(char),2U);}));}}
# 571
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 573
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmpD4[3U];({struct Cyc_PP_Doc*_tmp581=Cyc_Absynpp_lb();_tmpD4[0]=_tmp581;}),({struct Cyc_PP_Doc*_tmp580=Cyc_PP_seq(sep,ss);_tmpD4[1]=_tmp580;}),({struct Cyc_PP_Doc*_tmp57F=Cyc_Absynpp_rb();_tmpD4[2]=_tmp57F;});Cyc_PP_cat(_tag_dyneither(_tmpD4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 578
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD5=(void*)tms->hd;void*_tmpD6=_tmpD5;struct Cyc_List_List*_tmpED;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp583=Cyc_stderr;struct _dyneither_ptr _tmp582=({const char*_tmpD8="Carray_mod ";_tag_dyneither(_tmpD8,sizeof(char),12U);});Cyc_fprintf(_tmp583,_tmp582,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpD9=0U;({struct Cyc___cycFILE*_tmp585=Cyc_stderr;struct _dyneither_ptr _tmp584=({const char*_tmpDA="ConstArray_mod ";_tag_dyneither(_tmpDA,sizeof(char),16U);});Cyc_fprintf(_tmp585,_tmp584,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->tag == 1U){_LL5: _tmpED=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->f1;_LL6:
# 584
({void*_tmpDB=0U;({struct Cyc___cycFILE*_tmp587=Cyc_stderr;struct _dyneither_ptr _tmp586=({const char*_tmpDC="Function_mod(";_tag_dyneither(_tmpDC,sizeof(char),14U);});Cyc_fprintf(_tmp587,_tmp586,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _dyneither_ptr*_tmpDD=(*((struct _tuple8*)_tmpED->hd)).f1;
if(_tmpDD == 0)({void*_tmpDE=0U;({struct Cyc___cycFILE*_tmp589=Cyc_stderr;struct _dyneither_ptr _tmp588=({const char*_tmpDF="?";_tag_dyneither(_tmpDF,sizeof(char),2U);});Cyc_fprintf(_tmp589,_tmp588,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});else{
({void*_tmpE0=0U;({struct Cyc___cycFILE*_tmp58B=Cyc_stderr;struct _dyneither_ptr _tmp58A=*_tmpDD;Cyc_fprintf(_tmp58B,_tmp58A,_tag_dyneither(_tmpE0,sizeof(void*),0U));});});}
if(_tmpED->tl != 0)({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp58D=Cyc_stderr;struct _dyneither_ptr _tmp58C=({const char*_tmpE2=",";_tag_dyneither(_tmpE2,sizeof(char),2U);});Cyc_fprintf(_tmp58D,_tmp58C,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});}
# 591
({void*_tmpE3=0U;({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _dyneither_ptr _tmp58E=({const char*_tmpE4=") ";_tag_dyneither(_tmpE4,sizeof(char),3U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});
goto _LL0;}else{_LL7: _LL8:
# 594
({void*_tmpE5=0U;({struct Cyc___cycFILE*_tmp591=Cyc_stderr;struct _dyneither_ptr _tmp590=({const char*_tmpE6="Function_mod()";_tag_dyneither(_tmpE6,sizeof(char),15U);});Cyc_fprintf(_tmp591,_tmp590,_tag_dyneither(_tmpE5,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE7=0U;({struct Cyc___cycFILE*_tmp593=Cyc_stderr;struct _dyneither_ptr _tmp592=({const char*_tmpE8="Attributes_mod ";_tag_dyneither(_tmpE8,sizeof(char),16U);});Cyc_fprintf(_tmp593,_tmp592,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpE9=0U;({struct Cyc___cycFILE*_tmp595=Cyc_stderr;struct _dyneither_ptr _tmp594=({const char*_tmpEA="TypeParams_mod ";_tag_dyneither(_tmpEA,sizeof(char),16U);});Cyc_fprintf(_tmp595,_tmp594,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpEB=0U;({struct Cyc___cycFILE*_tmp597=Cyc_stderr;struct _dyneither_ptr _tmp596=({const char*_tmpEC="Pointer_mod ";_tag_dyneither(_tmpEC,sizeof(char),13U);});Cyc_fprintf(_tmp597,_tmp596,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 599
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp599=Cyc_stderr;struct _dyneither_ptr _tmp598=({const char*_tmpEF="\n";_tag_dyneither(_tmpEF,sizeof(char),2U);});Cyc_fprintf(_tmp599,_tmp598,_tag_dyneither(_tmpEE,sizeof(void*),0U));});});}
# 602
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 604
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp134[3U];({struct Cyc_PP_Doc*_tmp59B=Cyc_PP_text(({const char*_tmp135="(";_tag_dyneither(_tmp135,sizeof(char),2U);}));_tmp134[0]=_tmp59B;}),_tmp134[1]=rest,({struct Cyc_PP_Doc*_tmp59A=Cyc_PP_text(({const char*_tmp136=")";_tag_dyneither(_tmp136,sizeof(char),2U);}));_tmp134[2]=_tmp59A;});Cyc_PP_cat(_tag_dyneither(_tmp134,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpF0=(void*)tms->hd;void*_tmpF1=_tmpF0;void*_tmp133;void*_tmp132;void*_tmp131;void*_tmp130;struct Cyc_Absyn_Tqual _tmp12F;struct Cyc_List_List*_tmp12E;unsigned int _tmp12D;int _tmp12C;struct Cyc_List_List*_tmp12B;void*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;void*_tmp128;void*_tmp127;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmp127=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL2:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF2[2U];_tmpF2[0]=rest,Cyc_Absyn_type2bool(0,_tmp127)?({
struct Cyc_PP_Doc*_tmp59D=Cyc_PP_text(({const char*_tmpF3="[]@zeroterm";_tag_dyneither(_tmpF3,sizeof(char),12U);}));_tmpF2[1]=_tmp59D;}):({struct Cyc_PP_Doc*_tmp59C=Cyc_PP_text(({const char*_tmpF4="[]";_tag_dyneither(_tmpF4,sizeof(char),3U);}));_tmpF2[1]=_tmp59C;});Cyc_PP_cat(_tag_dyneither(_tmpF2,sizeof(struct Cyc_PP_Doc*),2U));});case 1U: _LL3: _tmp129=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp128=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL4:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF5[4U];_tmpF5[0]=rest,({struct Cyc_PP_Doc*_tmp5A1=Cyc_PP_text(({const char*_tmpF6="[";_tag_dyneither(_tmpF6,sizeof(char),2U);}));_tmpF5[1]=_tmp5A1;}),({struct Cyc_PP_Doc*_tmp5A0=Cyc_Absynpp_exp2doc(_tmp129);_tmpF5[2]=_tmp5A0;}),
Cyc_Absyn_type2bool(0,_tmp128)?({struct Cyc_PP_Doc*_tmp59F=Cyc_PP_text(({const char*_tmpF7="]@zeroterm";_tag_dyneither(_tmpF7,sizeof(char),11U);}));_tmpF5[3]=_tmp59F;}):({struct Cyc_PP_Doc*_tmp59E=Cyc_PP_text(({const char*_tmpF8="]";_tag_dyneither(_tmpF8,sizeof(char),2U);}));_tmpF5[3]=_tmp59E;});Cyc_PP_cat(_tag_dyneither(_tmpF5,sizeof(struct Cyc_PP_Doc*),4U));});case 3U: _LL5: _tmp12A=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL6:
# 618
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF9=_tmp12A;struct Cyc_List_List*_tmp107;unsigned int _tmp106;struct Cyc_List_List*_tmp105;int _tmp104;struct Cyc_Absyn_VarargInfo*_tmp103;void*_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->tag == 1U){_LLE: _tmp105=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp104=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f3;_tmp102=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f4;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f5;_tmp100=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f6;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f7;_LLF:
# 621
 return({struct Cyc_PP_Doc*_tmpFA[2U];_tmpFA[0]=rest,({struct Cyc_PP_Doc*_tmp5A2=Cyc_Absynpp_funargs2doc(_tmp105,_tmp104,_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF);_tmpFA[1]=_tmp5A2;});Cyc_PP_cat(_tag_dyneither(_tmpFA,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL10: _tmp107=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp106=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_LL11:
# 623
 return({struct Cyc_PP_Doc*_tmpFB[2U];_tmpFB[0]=rest,({struct Cyc_PP_Doc*_tmp5A6=({struct _dyneither_ptr _tmp5A5=({const char*_tmpFC="(";_tag_dyneither(_tmpFC,sizeof(char),2U);});struct _dyneither_ptr _tmp5A4=({const char*_tmpFD=")";_tag_dyneither(_tmpFD,sizeof(char),2U);});struct _dyneither_ptr _tmp5A3=({const char*_tmpFE=",";_tag_dyneither(_tmpFE,sizeof(char),2U);});Cyc_PP_group(_tmp5A5,_tmp5A4,_tmp5A3,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp107));});_tmpFB[1]=_tmp5A6;});Cyc_PP_cat(_tag_dyneither(_tmpFB,sizeof(struct Cyc_PP_Doc*),2U));});}_LLD:;};case 5U: _LL7: _tmp12B=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL8: {
# 626
enum Cyc_Cyclone_C_Compilers _tmp108=Cyc_Cyclone_c_compiler;if(_tmp108 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_tmp109[2U];_tmp109[0]=rest,({struct Cyc_PP_Doc*_tmp5A7=Cyc_Absynpp_atts2doc(_tmp12B);_tmp109[1]=_tmp5A7;});Cyc_PP_cat(_tag_dyneither(_tmp109,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 632
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp10A[2U];({struct Cyc_PP_Doc*_tmp5A8=Cyc_Absynpp_callconv2doc(_tmp12B);_tmp10A[0]=_tmp5A8;}),_tmp10A[1]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp12E=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp12D=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_tmp12C=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f3;_LLA:
# 637
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp12C)
return({struct Cyc_PP_Doc*_tmp10B[2U];_tmp10B[0]=rest,({struct Cyc_PP_Doc*_tmp5A9=Cyc_Absynpp_ktvars2doc(_tmp12E);_tmp10B[1]=_tmp5A9;});Cyc_PP_cat(_tag_dyneither(_tmp10B,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 641
return({struct Cyc_PP_Doc*_tmp10C[2U];_tmp10C[0]=rest,({struct Cyc_PP_Doc*_tmp5AA=Cyc_Absynpp_tvars2doc(_tmp12E);_tmp10C[1]=_tmp5AA;});Cyc_PP_cat(_tag_dyneither(_tmp10C,sizeof(struct Cyc_PP_Doc*),2U));});}default: _LLB: _tmp133=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).rgn;_tmp132=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).nullable;_tmp131=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).bounds;_tmp130=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).zero_term;_tmp12F=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LLC: {
# 645
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp12F);
{void*_tmp10D=Cyc_Tcutil_compress(_tmp131);void*_tmp10E=_tmp10D;void*_tmp11F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2 != 0){_LL1A: _tmp11F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2)->hd;_LL1B:
# 653
{void*_tmp10F=Cyc_Tcutil_compress(_tmp11F);void*_tmp110=_tmp10F;struct Cyc_Absyn_Exp*_tmp11B;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp110)->tag == 9U){_LL1F: _tmp11B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp110)->f1;_LL20:
# 655
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp132)?({const char*_tmp111="*";_tag_dyneither(_tmp111,sizeof(char),2U);}):({const char*_tmp112="@";_tag_dyneither(_tmp112,sizeof(char),2U);}));{
struct _tuple11 _tmp113=Cyc_Evexp_eval_const_uint_exp(_tmp11B);struct _tuple11 _tmp114=_tmp113;unsigned int _tmp117;int _tmp116;_LL24: _tmp117=_tmp114.f1;_tmp116=_tmp114.f2;_LL25:;
if(!_tmp116  || _tmp117 != (unsigned int)1)
ptr=({struct Cyc_PP_Doc*_tmp115[4U];_tmp115[0]=ptr,({struct Cyc_PP_Doc*_tmp5AD=Cyc_Absynpp_lb();_tmp115[1]=_tmp5AD;}),({struct Cyc_PP_Doc*_tmp5AC=Cyc_Absynpp_exp2doc(_tmp11B);_tmp115[2]=_tmp5AC;}),({struct Cyc_PP_Doc*_tmp5AB=Cyc_Absynpp_rb();_tmp115[3]=_tmp5AB;});Cyc_PP_cat(_tag_dyneither(_tmp115,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;};}else{_LL21: _LL22:
# 661
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp132)?({const char*_tmp118="*";_tag_dyneither(_tmp118,sizeof(char),2U);}):({const char*_tmp119="@";_tag_dyneither(_tmp119,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp11A[4U];_tmp11A[0]=ptr,({struct Cyc_PP_Doc*_tmp5B0=Cyc_Absynpp_lb();_tmp11A[1]=_tmp5B0;}),({struct Cyc_PP_Doc*_tmp5AF=Cyc_Absynpp_typ2doc(_tmp11F);_tmp11A[2]=_tmp5AF;}),({struct Cyc_PP_Doc*_tmp5AE=Cyc_Absynpp_rb();_tmp11A[3]=_tmp5AE;});Cyc_PP_cat(_tag_dyneither(_tmp11A,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL1E;}_LL1E:;}
# 665
goto _LL17;}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 667
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp132)?({const char*_tmp11C="*";_tag_dyneither(_tmp11C,sizeof(char),2U);}):({const char*_tmp11D="@";_tag_dyneither(_tmp11D,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp11E[4U];_tmp11E[0]=ptr,({struct Cyc_PP_Doc*_tmp5B3=Cyc_Absynpp_lb();_tmp11E[1]=_tmp5B3;}),({struct Cyc_PP_Doc*_tmp5B2=Cyc_Absynpp_typ2doc(_tmp131);_tmp11E[2]=_tmp5B2;}),({struct Cyc_PP_Doc*_tmp5B1=Cyc_Absynpp_rb();_tmp11E[3]=_tmp5B1;});Cyc_PP_cat(_tag_dyneither(_tmp11E,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 672
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && Cyc_Absyn_type2bool(0,_tmp130))
ztd=Cyc_PP_text(({const char*_tmp120="@zeroterm";_tag_dyneither(_tmp120,sizeof(char),10U);}));else{
if(is_char_ptr  && !Cyc_Absyn_type2bool(0,_tmp130))
ztd=Cyc_PP_text(({const char*_tmp121="@nozeroterm";_tag_dyneither(_tmp121,sizeof(char),12U);}));}}
# 678
{void*_tmp122=Cyc_Tcutil_compress(_tmp133);void*_tmp123=_tmp122;switch(*((int*)_tmp123)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp123)->f1)->tag == 5U){_LL27: _LL28:
 goto _LL26;}else{goto _LL2B;}case 1U: _LL29: if(Cyc_Absynpp_print_for_cycdoc){_LL2A:
 goto _LL26;}else{goto _LL2B;}default: _LL2B: _LL2C:
 rgd=Cyc_Absynpp_rgn2doc(_tmp133);}_LL26:;}{
# 683
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp126=" ";_tag_dyneither(_tmp126,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp125=" ";_tag_dyneither(_tmp125,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp124[7U];_tmp124[0]=ptr,_tmp124[1]=ztd,_tmp124[2]=rgd,_tmp124[3]=spacer1,_tmp124[4]=tqd,_tmp124[5]=spacer2,_tmp124[6]=rest;Cyc_PP_cat(_tag_dyneither(_tmp124,sizeof(struct Cyc_PP_Doc*),7U));});};}}_LL0:;};}
# 689
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp138)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp139="`H";_tag_dyneither(_tmp139,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp13A="`U";_tag_dyneither(_tmp13A,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp13B="`RC";_tag_dyneither(_tmp13B,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 699
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 703
void*_tmp13C=Cyc_Tcutil_compress(t);void*_tmp13D=_tmp13C;struct Cyc_List_List*_tmp141;void*_tmp140;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2 != 0){_LL1: _tmp140=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2)->hd;_LL2:
({struct Cyc_List_List*_tmp5B5=({struct Cyc_List_List*_tmp13E=_cycalloc(sizeof(*_tmp13E));({struct Cyc_PP_Doc*_tmp5B4=Cyc_Absynpp_rgn2doc(_tmp140);_tmp13E->hd=_tmp5B4;}),_tmp13E->tl=*rgions;_tmp13E;});*rgions=_tmp5B5;});goto _LL0;}else{goto _LL5;}case 9U: _LL3: _tmp141=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13D)->f2;_LL4:
# 706
 for(0;_tmp141 != 0;_tmp141=_tmp141->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp141->hd);}
# 709
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp5B7=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));({struct Cyc_PP_Doc*_tmp5B6=Cyc_Absynpp_typ2doc(t);_tmp13F->hd=_tmp5B6;}),_tmp13F->tl=*effects;_tmp13F;});*effects=_tmp5B7;});goto _LL0;}_LL0:;}
# 714
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0)
return({struct _dyneither_ptr _tmp5BA=({const char*_tmp142="";_tag_dyneither(_tmp142,sizeof(char),1U);});struct _dyneither_ptr _tmp5B9=({const char*_tmp143="";_tag_dyneither(_tmp143,sizeof(char),1U);});struct _dyneither_ptr _tmp5B8=({const char*_tmp144="+";_tag_dyneither(_tmp144,sizeof(char),2U);});Cyc_PP_group(_tmp5BA,_tmp5B9,_tmp5B8,effects);});else{
# 722
struct Cyc_PP_Doc*_tmp145=({struct _dyneither_ptr _tmp5BB=({const char*_tmp14A=",";_tag_dyneither(_tmp14A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5BB,rgions);});
return({struct _dyneither_ptr _tmp5BF=({const char*_tmp146="";_tag_dyneither(_tmp146,sizeof(char),1U);});struct _dyneither_ptr _tmp5BE=({const char*_tmp147="";_tag_dyneither(_tmp147,sizeof(char),1U);});struct _dyneither_ptr _tmp5BD=({const char*_tmp148="+";_tag_dyneither(_tmp148,sizeof(char),2U);});Cyc_PP_group(_tmp5BF,_tmp5BE,_tmp5BD,({struct Cyc_List_List*_tmp5BC=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5BC,({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=_tmp145,_tmp149->tl=0;_tmp149;}));}));});}}
# 727
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp14B=k;if(_tmp14B == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp14C="struct ";_tag_dyneither(_tmp14C,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp14D="union ";_tag_dyneither(_tmp14D,sizeof(char),7U);}));}_LL0:;}
# 735
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp14E=t;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C4;struct Cyc_Absyn_Aggrdecl*_tmp1C3;struct _tuple0*_tmp1C2;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_Typedefdecl*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1BE;enum Cyc_Absyn_AggrKind _tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BB;struct Cyc_Absyn_Tvar*_tmp1BA;struct Cyc_Core_Opt*_tmp1B9;void*_tmp1B8;int _tmp1B7;struct Cyc_Core_Opt*_tmp1B6;void*_tmp1B5;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_List_List*_tmp1B2;struct _dyneither_ptr _tmp1B1;struct _tuple0*_tmp1B0;struct Cyc_List_List*_tmp1AF;union Cyc_Absyn_AggrInfo _tmp1AE;struct Cyc_List_List*_tmp1AD;int _tmp1AC;enum Cyc_Absyn_Sign _tmp1AB;enum Cyc_Absyn_Size_of _tmp1AA;union Cyc_Absyn_DatatypeFieldInfo _tmp1A9;struct Cyc_List_List*_tmp1A8;union Cyc_Absyn_DatatypeInfo _tmp1A7;struct Cyc_List_List*_tmp1A6;switch(*((int*)_tmp14E)){case 4U: _LL1: _LL2:
# 739
 return Cyc_PP_text(({const char*_tmp14F="[[[array]]]";_tag_dyneither(_tmp14F,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)){case 0U: _LL7: _LL8:
# 743
 s=Cyc_PP_text(({const char*_tmp150="void";_tag_dyneither(_tmp150,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp1A7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LLE:
# 759
{union Cyc_Absyn_DatatypeInfo _tmp158=_tmp1A7;struct _tuple0*_tmp161;int _tmp160;struct _tuple0*_tmp15F;int _tmp15E;if((_tmp158.UnknownDatatype).tag == 1){_LL48: _tmp15F=((_tmp158.UnknownDatatype).val).name;_tmp15E=((_tmp158.UnknownDatatype).val).is_extensible;_LL49:
 _tmp161=_tmp15F;_tmp160=_tmp15E;goto _LL4B;}else{_LL4A: _tmp161=(*(_tmp158.KnownDatatype).val)->name;_tmp160=(*(_tmp158.KnownDatatype).val)->is_extensible;_LL4B: {
# 762
struct Cyc_PP_Doc*_tmp159=Cyc_PP_text(({const char*_tmp15D="datatype ";_tag_dyneither(_tmp15D,sizeof(char),10U);}));
struct Cyc_PP_Doc*_tmp15A=_tmp160?Cyc_PP_text(({const char*_tmp15C="@extensible ";_tag_dyneither(_tmp15C,sizeof(char),13U);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_tmp15B[4U];_tmp15B[0]=_tmp15A,_tmp15B[1]=_tmp159,({struct Cyc_PP_Doc*_tmp5C1=Cyc_Absynpp_qvar2doc(_tmp161);_tmp15B[2]=_tmp5C1;}),({struct Cyc_PP_Doc*_tmp5C0=Cyc_Absynpp_tps2doc(_tmp1A6);_tmp15B[3]=_tmp5C0;});Cyc_PP_cat(_tag_dyneither(_tmp15B,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 767
goto _LL0;case 19U: _LLF: _tmp1A9=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1A8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL10:
# 769
{union Cyc_Absyn_DatatypeFieldInfo _tmp162=_tmp1A9;struct _tuple0*_tmp16D;int _tmp16C;struct _tuple0*_tmp16B;struct _tuple0*_tmp16A;struct _tuple0*_tmp169;int _tmp168;if((_tmp162.UnknownDatatypefield).tag == 1){_LL4D: _tmp16A=((_tmp162.UnknownDatatypefield).val).datatype_name;_tmp169=((_tmp162.UnknownDatatypefield).val).field_name;_tmp168=((_tmp162.UnknownDatatypefield).val).is_extensible;_LL4E:
# 771
 _tmp16D=_tmp16A;_tmp16C=_tmp168;_tmp16B=_tmp169;goto _LL50;}else{_LL4F: _tmp16D=(((_tmp162.KnownDatatypefield).val).f1)->name;_tmp16C=(((_tmp162.KnownDatatypefield).val).f1)->is_extensible;_tmp16B=(((_tmp162.KnownDatatypefield).val).f2)->name;_LL50: {
# 774
struct Cyc_PP_Doc*_tmp163=Cyc_PP_text(_tmp16C?({const char*_tmp166="@extensible datatype ";_tag_dyneither(_tmp166,sizeof(char),22U);}):({const char*_tmp167="datatype ";_tag_dyneither(_tmp167,sizeof(char),10U);}));
s=({struct Cyc_PP_Doc*_tmp164[4U];_tmp164[0]=_tmp163,({struct Cyc_PP_Doc*_tmp5C4=Cyc_Absynpp_qvar2doc(_tmp16D);_tmp164[1]=_tmp5C4;}),({struct Cyc_PP_Doc*_tmp5C3=Cyc_PP_text(({const char*_tmp165=".";_tag_dyneither(_tmp165,sizeof(char),2U);}));_tmp164[2]=_tmp5C3;}),({struct Cyc_PP_Doc*_tmp5C2=Cyc_Absynpp_qvar2doc(_tmp16B);_tmp164[3]=_tmp5C2;});Cyc_PP_cat(_tag_dyneither(_tmp164,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 778
goto _LL0;case 1U: _LL11: _tmp1AB=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1AA=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f2;_LL12: {
# 780
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp16E=_tmp1AB;switch(_tmp16E){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp16F="";_tag_dyneither(_tmp16F,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp170="unsigned ";_tag_dyneither(_tmp170,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 787
{enum Cyc_Absyn_Size_of _tmp171=_tmp1AA;switch(_tmp171){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 789
{enum Cyc_Absyn_Sign _tmp172=_tmp1AB;switch(_tmp172){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp173="";_tag_dyneither(_tmp173,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp174="signed ";_tag_dyneither(_tmp174,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp175="unsigned ";_tag_dyneither(_tmp175,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 794
ts=({const char*_tmp176="char";_tag_dyneither(_tmp176,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp177="short";_tag_dyneither(_tmp177,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp178="int";_tag_dyneither(_tmp178,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp179="long";_tag_dyneither(_tmp179,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 801
{enum Cyc_Cyclone_C_Compilers _tmp17A=Cyc_Cyclone_c_compiler;if(_tmp17A == Cyc_Cyclone_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp17B="long long";_tag_dyneither(_tmp17B,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp17C="__int64";_tag_dyneither(_tmp17C,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 805
goto _LL58;}_LL58:;}
# 807
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,_tmp545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);_tmp545;});struct Cyc_String_pa_PrintArg_struct _tmp180=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);_tmp544;});void*_tmp17D[2U];_tmp17D[0]=& _tmp17F,_tmp17D[1]=& _tmp180;({struct _dyneither_ptr _tmp5C5=({const char*_tmp17E="%s%s";_tag_dyneither(_tmp17E,sizeof(char),5U);});Cyc_aprintf(_tmp5C5,_tag_dyneither(_tmp17D,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp1AC=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL14:
# 810
{int _tmp181=_tmp1AC;switch(_tmp181){case 0U: _LL72: _LL73:
 s=Cyc_PP_text(({const char*_tmp182="float";_tag_dyneither(_tmp182,sizeof(char),6U);}));goto _LL71;case 1U: _LL74: _LL75:
 s=Cyc_PP_text(({const char*_tmp183="double";_tag_dyneither(_tmp183,sizeof(char),7U);}));goto _LL71;default: _LL76: _LL77:
 s=Cyc_PP_text(({const char*_tmp184="long double";_tag_dyneither(_tmp184,sizeof(char),12U);}));goto _LL71;}_LL71:;}
# 815
goto _LL0;case 20U: _LL17: _tmp1AE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_tmp1AD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL18: {
# 818
struct _tuple10 _tmp186=Cyc_Absyn_aggr_kinded_name(_tmp1AE);struct _tuple10 _tmp187=_tmp186;enum Cyc_Absyn_AggrKind _tmp18A;struct _tuple0*_tmp189;_LL79: _tmp18A=_tmp187.f1;_tmp189=_tmp187.f2;_LL7A:;
s=({struct Cyc_PP_Doc*_tmp188[3U];({struct Cyc_PP_Doc*_tmp5C8=Cyc_Absynpp_aggr_kind2doc(_tmp18A);_tmp188[0]=_tmp5C8;}),({struct Cyc_PP_Doc*_tmp5C7=Cyc_Absynpp_qvar2doc(_tmp189);_tmp188[1]=_tmp5C7;}),({struct Cyc_PP_Doc*_tmp5C6=Cyc_Absynpp_tps2doc(_tmp1AD);_tmp188[2]=_tmp5C6;});Cyc_PP_cat(_tag_dyneither(_tmp188,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 16U: _LL1B: _tmp1AF=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1C:
# 827
 s=({struct Cyc_PP_Doc*_tmp18C[4U];({struct Cyc_PP_Doc*_tmp5CC=Cyc_PP_text(({const char*_tmp18D="enum ";_tag_dyneither(_tmp18D,sizeof(char),6U);}));_tmp18C[0]=_tmp5CC;}),({struct Cyc_PP_Doc*_tmp5CB=Cyc_Absynpp_lb();_tmp18C[1]=_tmp5CB;}),({struct Cyc_PP_Doc*_tmp5CA=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1AF));_tmp18C[2]=_tmp5CA;}),({struct Cyc_PP_Doc*_tmp5C9=Cyc_Absynpp_rb();_tmp18C[3]=_tmp5C9;});Cyc_PP_cat(_tag_dyneither(_tmp18C,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 15U: _LL1D: _tmp1B0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL1E:
 s=({struct Cyc_PP_Doc*_tmp18E[2U];({struct Cyc_PP_Doc*_tmp5CE=Cyc_PP_text(({const char*_tmp18F="enum ";_tag_dyneither(_tmp18F,sizeof(char),6U);}));_tmp18E[0]=_tmp5CE;}),({struct Cyc_PP_Doc*_tmp5CD=Cyc_Absynpp_qvar2doc(_tmp1B0);_tmp18E[1]=_tmp5CD;});Cyc_PP_cat(_tag_dyneither(_tmp18E,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 17U: _LL23: _tmp1B1=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1)->f1;_LL24:
# 831
 s=Cyc_PP_text(_tmp1B1);goto _LL0;case 3U: _LL27: _tmp1B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL28:
# 841
 s=({struct Cyc_PP_Doc*_tmp197[3U];({struct Cyc_PP_Doc*_tmp5D1=Cyc_PP_text(({const char*_tmp198="region_t<";_tag_dyneither(_tmp198,sizeof(char),10U);}));_tmp197[0]=_tmp5D1;}),({struct Cyc_PP_Doc*_tmp5D0=Cyc_Absynpp_rgn2doc((void*)((struct Cyc_List_List*)_check_null(_tmp1B2))->hd);_tmp197[1]=_tmp5D0;}),({struct Cyc_PP_Doc*_tmp5CF=Cyc_PP_text(({const char*_tmp199=">";_tag_dyneither(_tmp199,sizeof(char),2U);}));_tmp197[2]=_tmp5CF;});Cyc_PP_cat(_tag_dyneither(_tmp197,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 4U: _LL29: _tmp1B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL2A:
# 843
 s=({struct Cyc_PP_Doc*_tmp19A[3U];({struct Cyc_PP_Doc*_tmp5D4=Cyc_PP_text(({const char*_tmp19B="tag_t<";_tag_dyneither(_tmp19B,sizeof(char),7U);}));_tmp19A[0]=_tmp5D4;}),({struct Cyc_PP_Doc*_tmp5D3=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(_tmp1B3))->hd);_tmp19A[1]=_tmp5D3;}),({struct Cyc_PP_Doc*_tmp5D2=Cyc_PP_text(({const char*_tmp19C=">";_tag_dyneither(_tmp19C,sizeof(char),2U);}));_tmp19A[2]=_tmp5D2;});Cyc_PP_cat(_tag_dyneither(_tmp19A,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: _LL31: _tmp1B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL32:
# 848
 s=({struct Cyc_PP_Doc*_tmp19D[3U];({struct Cyc_PP_Doc*_tmp5D7=Cyc_PP_text(({const char*_tmp19E="regions(";_tag_dyneither(_tmp19E,sizeof(char),9U);}));_tmp19D[0]=_tmp5D7;}),({struct Cyc_PP_Doc*_tmp5D6=Cyc_Absynpp_typ2doc((void*)((struct Cyc_List_List*)_check_null(_tmp1B4))->hd);_tmp19D[1]=_tmp5D6;}),({struct Cyc_PP_Doc*_tmp5D5=Cyc_PP_text(({const char*_tmp19F=")";_tag_dyneither(_tmp19F,sizeof(char),2U);}));_tmp19D[2]=_tmp5D5;});Cyc_PP_cat(_tag_dyneither(_tmp19D,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 8U: _LL33: _LL34:
 goto _LL36;case 9U: _LL35: _LL36:
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 854
 s=Cyc_PP_text(({const char*_tmp1A0="@true";_tag_dyneither(_tmp1A0,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
 s=Cyc_PP_text(({const char*_tmp1A1="@false";_tag_dyneither(_tmp1A1,sizeof(char),7U);}));goto _LL0;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2 != 0){_LL41: _tmp1B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2)->hd;_LL42:
# 857
 s=({struct Cyc_PP_Doc*_tmp1A2[4U];({struct Cyc_PP_Doc*_tmp5DB=Cyc_PP_text(({const char*_tmp1A3="@thin @numelts";_tag_dyneither(_tmp1A3,sizeof(char),15U);}));_tmp1A2[0]=_tmp5DB;}),({struct Cyc_PP_Doc*_tmp5DA=Cyc_Absynpp_lb();_tmp1A2[1]=_tmp5DA;}),({struct Cyc_PP_Doc*_tmp5D9=Cyc_Absynpp_typ2doc(_tmp1B5);_tmp1A2[2]=_tmp5D9;}),({struct Cyc_PP_Doc*_tmp5D8=Cyc_Absynpp_rb();_tmp1A2[3]=_tmp5D8;});Cyc_PP_cat(_tag_dyneither(_tmp1A2,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;}else{_LL43: _LL44:
# 859
 s=Cyc_PP_text(({const char*_tmp1A4="@thin";_tag_dyneither(_tmp1A4,sizeof(char),6U);}));goto _LL0;}default: _LL45: _LL46:
 s=Cyc_PP_text(({const char*_tmp1A5="@fat";_tag_dyneither(_tmp1A5,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp1B9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f1;_tmp1B8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f2;_tmp1B7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f3;_tmp1B6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f4;_LLA:
# 745
 if(_tmp1B8 != 0)
# 747
return Cyc_Absynpp_ntyp2doc(_tmp1B8);else{
# 749
struct _dyneither_ptr kindstring=_tmp1B9 == 0?({const char*_tmp155="K";_tag_dyneither(_tmp155,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp1B9->v);
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0U,_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);_tmp547;});struct Cyc_Int_pa_PrintArg_struct _tmp154=({struct Cyc_Int_pa_PrintArg_struct _tmp546;_tmp546.tag=1U,_tmp546.f1=(unsigned long)_tmp1B7;_tmp546;});void*_tmp151[2U];_tmp151[0]=& _tmp153,_tmp151[1]=& _tmp154;({struct _dyneither_ptr _tmp5DC=({const char*_tmp152="`E%s%d";_tag_dyneither(_tmp152,sizeof(char),7U);});Cyc_aprintf(_tmp5DC,_tag_dyneither(_tmp151,sizeof(void*),2U));});}));}
# 752
goto _LL0;case 2U: _LLB: _tmp1BA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LLC:
# 754
 s=Cyc_Absynpp_tvar2doc(_tmp1BA);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp156[3U];_tmp156[0]=s,({struct Cyc_PP_Doc*_tmp5DE=Cyc_PP_text(({const char*_tmp157="::";_tag_dyneither(_tmp157,sizeof(char),3U);}));_tmp156[1]=_tmp5DE;}),({struct Cyc_PP_Doc*_tmp5DD=Cyc_Absynpp_kindbound2doc(_tmp1BA->kind);_tmp156[2]=_tmp5DD;});Cyc_PP_cat(_tag_dyneither(_tmp156,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 6U: _LL15: _tmp1BB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14E)->f1;_LL16:
# 816
 s=({struct Cyc_PP_Doc*_tmp185[2U];({struct Cyc_PP_Doc*_tmp5E0=Cyc_Absynpp_dollar();_tmp185[0]=_tmp5E0;}),({struct Cyc_PP_Doc*_tmp5DF=Cyc_Absynpp_args2doc(_tmp1BB);_tmp185[1]=_tmp5DF;});Cyc_PP_cat(_tag_dyneither(_tmp185,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 7U: _LL19: _tmp1BD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f1;_tmp1BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E)->f2;_LL1A:
# 822
 s=({struct Cyc_PP_Doc*_tmp18B[4U];({struct Cyc_PP_Doc*_tmp5E4=Cyc_Absynpp_aggr_kind2doc(_tmp1BD);_tmp18B[0]=_tmp5E4;}),({struct Cyc_PP_Doc*_tmp5E3=Cyc_Absynpp_lb();_tmp18B[1]=_tmp5E3;}),({
struct Cyc_PP_Doc*_tmp5E2=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1BC));_tmp18B[2]=_tmp5E2;}),({
struct Cyc_PP_Doc*_tmp5E1=Cyc_Absynpp_rb();_tmp18B[3]=_tmp5E1;});Cyc_PP_cat(_tag_dyneither(_tmp18B,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 9U: _LL1F: _tmp1BE=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14E)->f1;_LL20:
# 829
 s=({struct Cyc_PP_Doc*_tmp190[3U];({struct Cyc_PP_Doc*_tmp5E7=Cyc_PP_text(({const char*_tmp191="valueof_t(";_tag_dyneither(_tmp191,sizeof(char),11U);}));_tmp190[0]=_tmp5E7;}),({struct Cyc_PP_Doc*_tmp5E6=Cyc_Absynpp_exp2doc(_tmp1BE);_tmp190[1]=_tmp5E6;}),({struct Cyc_PP_Doc*_tmp5E5=Cyc_PP_text(({const char*_tmp192=")";_tag_dyneither(_tmp192,sizeof(char),2U);}));_tmp190[2]=_tmp5E5;});Cyc_PP_cat(_tag_dyneither(_tmp190,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 11U: _LL21: _tmp1BF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp14E)->f1;_LL22:
 s=({struct Cyc_PP_Doc*_tmp193[3U];({struct Cyc_PP_Doc*_tmp5EA=Cyc_PP_text(({const char*_tmp194="typeof(";_tag_dyneither(_tmp194,sizeof(char),8U);}));_tmp193[0]=_tmp5EA;}),({struct Cyc_PP_Doc*_tmp5E9=Cyc_Absynpp_exp2doc(_tmp1BF);_tmp193[1]=_tmp5E9;}),({struct Cyc_PP_Doc*_tmp5E8=Cyc_PP_text(({const char*_tmp195=")";_tag_dyneither(_tmp195,sizeof(char),2U);}));_tmp193[2]=_tmp5E8;});Cyc_PP_cat(_tag_dyneither(_tmp193,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 8U: _LL25: _tmp1C2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f1;_tmp1C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f2;_tmp1C0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f3;_LL26:
# 838
 s=({struct Cyc_PP_Doc*_tmp196[2U];({struct Cyc_PP_Doc*_tmp5EC=Cyc_Absynpp_qvar2doc(_tmp1C2);_tmp196[0]=_tmp5EC;}),({struct Cyc_PP_Doc*_tmp5EB=Cyc_Absynpp_tps2doc(_tmp1C1);_tmp196[1]=_tmp5EB;});Cyc_PP_cat(_tag_dyneither(_tmp196,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)){case 0U: _LL37: _tmp1C3=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL38:
# 851
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1C3);goto _LL0;case 1U: _LL39: _tmp1C4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3A:
 s=Cyc_Absynpp_enumdecl2doc(_tmp1C4);goto _LL0;default: _LL3B: _tmp1C5=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)->f1;_LL3C:
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1C5);goto _LL0;}}_LL0:;}
# 862
return s;}
# 865
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 869
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1C6=cmp;void*_tmp1CA;void*_tmp1C9;_LL1: _tmp1CA=_tmp1C6->f1;_tmp1C9=_tmp1C6->f2;_LL2:;
return({struct Cyc_PP_Doc*_tmp1C7[3U];({struct Cyc_PP_Doc*_tmp5EF=Cyc_Absynpp_rgn2doc(_tmp1CA);_tmp1C7[0]=_tmp5EF;}),({struct Cyc_PP_Doc*_tmp5EE=Cyc_PP_text(({const char*_tmp1C8=" > ";_tag_dyneither(_tmp1C8,sizeof(char),4U);}));_tmp1C7[1]=_tmp5EE;}),({struct Cyc_PP_Doc*_tmp5ED=Cyc_Absynpp_rgn2doc(_tmp1C9);_tmp1C7[2]=_tmp5ED;});Cyc_PP_cat(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_PP_Doc*),3U));});}
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _dyneither_ptr _tmp5F2=({const char*_tmp1CB="";_tag_dyneither(_tmp1CB,sizeof(char),1U);});struct _dyneither_ptr _tmp5F1=({const char*_tmp1CC="";_tag_dyneither(_tmp1CC,sizeof(char),1U);});struct _dyneither_ptr _tmp5F0=({const char*_tmp1CD=",";_tag_dyneither(_tmp1CD,sizeof(char),2U);});Cyc_PP_group(_tmp5F2,_tmp5F1,_tmp5F0,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 878
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1CE=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1CE == 0?0:({struct Cyc_Core_Opt*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct Cyc_PP_Doc*_tmp5F3=Cyc_PP_text(*_tmp1CE);_tmp1CF->v=_tmp5F3;});_tmp1CF;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 884
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 888
struct Cyc_List_List*_tmp1D0=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
_tmp1D0=({struct Cyc_List_List*_tmp5F5=_tmp1D0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5F5,({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({struct Cyc_PP_Doc*_tmp5F4=Cyc_PP_text(({const char*_tmp1D1="...";_tag_dyneither(_tmp1D1,sizeof(char),4U);}));_tmp1D2->hd=_tmp5F4;}),_tmp1D2->tl=0;_tmp1D2;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1D3=({struct Cyc_PP_Doc*_tmp1D5[3U];({struct Cyc_PP_Doc*_tmp5F9=Cyc_PP_text(({const char*_tmp1D6="...";_tag_dyneither(_tmp1D6,sizeof(char),4U);}));_tmp1D5[0]=_tmp5F9;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp5F8=Cyc_PP_text(({const char*_tmp1D7=" inject ";_tag_dyneither(_tmp1D7,sizeof(char),9U);}));_tmp1D5[1]=_tmp5F8;}):({struct Cyc_PP_Doc*_tmp5F7=Cyc_PP_text(({const char*_tmp1D8=" ";_tag_dyneither(_tmp1D8,sizeof(char),2U);}));_tmp1D5[1]=_tmp5F7;}),({
struct Cyc_PP_Doc*_tmp5F6=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->f1=cyc_varargs->name,_tmp1D9->f2=cyc_varargs->tq,_tmp1D9->f3=cyc_varargs->type;_tmp1D9;}));_tmp1D5[2]=_tmp5F6;});Cyc_PP_cat(_tag_dyneither(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3U));});
# 897
_tmp1D0=({struct Cyc_List_List*_tmp5FA=_tmp1D0;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5FA,({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=_tmp1D3,_tmp1D4->tl=0;_tmp1D4;}));});}}{
# 899
struct Cyc_PP_Doc*_tmp1DA=({struct _dyneither_ptr _tmp5FD=({const char*_tmp1E9="";_tag_dyneither(_tmp1E9,sizeof(char),1U);});struct _dyneither_ptr _tmp5FC=({const char*_tmp1EA="";_tag_dyneither(_tmp1EA,sizeof(char),1U);});struct _dyneither_ptr _tmp5FB=({const char*_tmp1EB=",";_tag_dyneither(_tmp1EB,sizeof(char),2U);});Cyc_PP_group(_tmp5FD,_tmp5FC,_tmp5FB,_tmp1D0);});
if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1DA=({struct Cyc_PP_Doc*_tmp1DB[3U];_tmp1DB[0]=_tmp1DA,({struct Cyc_PP_Doc*_tmp5FF=Cyc_PP_text(({const char*_tmp1DC=";";_tag_dyneither(_tmp1DC,sizeof(char),2U);}));_tmp1DB[1]=_tmp5FF;}),({struct Cyc_PP_Doc*_tmp5FE=Cyc_Absynpp_eff2doc(effopt);_tmp1DB[2]=_tmp5FE;});Cyc_PP_cat(_tag_dyneither(_tmp1DB,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
_tmp1DA=({struct Cyc_PP_Doc*_tmp1DD[3U];_tmp1DD[0]=_tmp1DA,({struct Cyc_PP_Doc*_tmp601=Cyc_PP_text(({const char*_tmp1DE=":";_tag_dyneither(_tmp1DE,sizeof(char),2U);}));_tmp1DD[1]=_tmp601;}),({struct Cyc_PP_Doc*_tmp600=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1DD[2]=_tmp600;});Cyc_PP_cat(_tag_dyneither(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp1DF=({struct Cyc_PP_Doc*_tmp1E6[3U];({struct Cyc_PP_Doc*_tmp603=Cyc_PP_text(({const char*_tmp1E7="(";_tag_dyneither(_tmp1E7,sizeof(char),2U);}));_tmp1E6[0]=_tmp603;}),_tmp1E6[1]=_tmp1DA,({struct Cyc_PP_Doc*_tmp602=Cyc_PP_text(({const char*_tmp1E8=")";_tag_dyneither(_tmp1E8,sizeof(char),2U);}));_tmp1E6[2]=_tmp602;});Cyc_PP_cat(_tag_dyneither(_tmp1E6,sizeof(struct Cyc_PP_Doc*),3U));});
if(req != 0)
_tmp1DF=({struct Cyc_PP_Doc*_tmp1E0[4U];_tmp1E0[0]=_tmp1DF,({struct Cyc_PP_Doc*_tmp606=Cyc_PP_text(({const char*_tmp1E1=" @requires(";_tag_dyneither(_tmp1E1,sizeof(char),12U);}));_tmp1E0[1]=_tmp606;}),({struct Cyc_PP_Doc*_tmp605=Cyc_Absynpp_exp2doc(req);_tmp1E0[2]=_tmp605;}),({struct Cyc_PP_Doc*_tmp604=Cyc_PP_text(({const char*_tmp1E2=")";_tag_dyneither(_tmp1E2,sizeof(char),2U);}));_tmp1E0[3]=_tmp604;});Cyc_PP_cat(_tag_dyneither(_tmp1E0,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
_tmp1DF=({struct Cyc_PP_Doc*_tmp1E3[4U];_tmp1E3[0]=_tmp1DF,({struct Cyc_PP_Doc*_tmp609=Cyc_PP_text(({const char*_tmp1E4=" @ensures(";_tag_dyneither(_tmp1E4,sizeof(char),11U);}));_tmp1E3[1]=_tmp609;}),({struct Cyc_PP_Doc*_tmp608=Cyc_Absynpp_exp2doc(ens);_tmp1E3[2]=_tmp608;}),({struct Cyc_PP_Doc*_tmp607=Cyc_PP_text(({const char*_tmp1E5=")";_tag_dyneither(_tmp1E5,sizeof(char),2U);}));_tmp1E3[3]=_tmp607;});Cyc_PP_cat(_tag_dyneither(_tmp1E3,sizeof(struct Cyc_PP_Doc*),4U));});
return _tmp1DF;};};}
# 912
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 914
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1EC=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1ED=(*q).f1;union Cyc_Absyn_Nmspace _tmp1EE=_tmp1ED;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F1;struct Cyc_List_List*_tmp1F0;switch((_tmp1EE.C_n).tag){case 4U: _LL1: _LL2:
 _tmp1F0=0;goto _LL4;case 1U: _LL3: _tmp1F0=(_tmp1EE.Rel_n).val;_LL4:
# 920
 match=0;
_tmp1EC=_tmp1F0;
goto _LL0;case 3U: _LL5: _tmp1F1=(_tmp1EE.C_n).val;_LL6:
# 924
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1F1,Cyc_Absynpp_curr_namespace);
# 926
goto _LL0;default: _LL7: _tmp1F2=(_tmp1EE.Abs_n).val;_LL8:
# 928
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1F2,Cyc_Absynpp_curr_namespace);
_tmp1EC=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->hd=Cyc_Absynpp_cyc_stringptr,_tmp1EF->tl=_tmp1F2;_tmp1EF;}): _tmp1F2;
goto _LL0;}_LL0:;}
# 932
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp60B=({struct Cyc_List_List*_tmp60A=_tmp1EC;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60A,({struct Cyc_List_List*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));
_tmp1F3->hd=(*q).f2,_tmp1F3->tl=0;_tmp1F3;}));});
# 933
Cyc_str_sepstr(_tmp60B,({const char*_tmp1F4="_";_tag_dyneither(_tmp1F4,sizeof(char),2U);}));});else{
# 937
if(match)
return*(*q).f2;else{
# 940
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp60D=({struct Cyc_List_List*_tmp60C=_tmp1EC;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp60C,({struct Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=(*q).f2,_tmp1F5->tl=0;_tmp1F5;}));});Cyc_str_sepstr(_tmp60D,({const char*_tmp1F6="::";_tag_dyneither(_tmp1F6,sizeof(char),3U);}));});}}}
# 944
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 948
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1F7=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 952
return({struct _dyneither_ptr _tmp610=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp60F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp60E=({const char*_tmp1F8="\\textbf{";_tag_dyneither(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp60E,(struct _dyneither_ptr)_tmp1F7);});Cyc_strconcat(_tmp60F,({const char*_tmp1F9="}";_tag_dyneither(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp610,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));});else{
return Cyc_PP_text(_tmp1F7);}}
# 957
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 959
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 962
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1FA=(*v).f1;union Cyc_Absyn_Nmspace _tmp1FB=_tmp1FA;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FD;switch((_tmp1FB.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp1FB.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 973
 return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp611=({const char*_tmp1FC="/* bad namespace : */ ";_tag_dyneither(_tmp1FC,sizeof(char),23U);});Cyc_strconcat(_tmp611,(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1FD=(_tmp1FB.C_n).val;_LL6:
# 966
 _tmp1FE=_tmp1FD;goto _LL8;default: _LL7: _tmp1FE=(_tmp1FB.Abs_n).val;_LL8:
# 968
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1FE,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 971
goto _LLA;}}_LL0:;}else{
# 976
return*(*v).f2;}}
# 979
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 982
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp1FF=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 986
return({struct _dyneither_ptr _tmp614=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp613=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp612=({const char*_tmp200="\\textbf{";_tag_dyneither(_tmp200,sizeof(char),9U);});Cyc_strconcat(_tmp612,(struct _dyneither_ptr)_tmp1FF);});Cyc_strconcat(_tmp613,({const char*_tmp201="}";_tag_dyneither(_tmp201,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp614,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1FF));});else{
return Cyc_PP_text(_tmp1FF);}}
# 991
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp615=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp615,t,0);});}
# 1002 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp202=e->r;void*_tmp203=_tmp202;struct Cyc_Absyn_Exp*_tmp207;struct Cyc_Absyn_Exp*_tmp206;enum Cyc_Absyn_Primop _tmp205;switch(*((int*)_tmp203)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp205=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp203)->f1;_LL6: {
# 1007
enum Cyc_Absyn_Primop _tmp204=_tmp205;switch(_tmp204){case Cyc_Absyn_Plus: _LL56: _LL57:
 return 100;case Cyc_Absyn_Times: _LL58: _LL59:
 return 110;case Cyc_Absyn_Minus: _LL5A: _LL5B:
 return 100;case Cyc_Absyn_Div: _LL5C: _LL5D:
 goto _LL5F;case Cyc_Absyn_Mod: _LL5E: _LL5F:
 return 110;case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 return 70;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Gte: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B:
 return 80;case Cyc_Absyn_Not: _LL6C: _LL6D:
 goto _LL6F;case Cyc_Absyn_Bitnot: _LL6E: _LL6F:
 return 130;case Cyc_Absyn_Bitand: _LL70: _LL71:
 return 60;case Cyc_Absyn_Bitor: _LL72: _LL73:
 return 40;case Cyc_Absyn_Bitxor: _LL74: _LL75:
 return 50;case Cyc_Absyn_Bitlshift: _LL76: _LL77:
 return 90;case Cyc_Absyn_Bitlrshift: _LL78: _LL79:
 return 80;case Cyc_Absyn_Bitarshift: _LL7A: _LL7B:
 return 80;case Cyc_Absyn_Numelts: _LL7C: _LL7D:
 return 140;default: _LL7E: _LL7F:
 return 140;}_LL55:;}case 4U: _LL7: _LL8:
# 1030
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp206=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp203)->f1;_LL1A:
 return Cyc_Absynpp_exp_prec(_tmp206);case 13U: _LL1B: _tmp207=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp203)->f1;_LL1C:
 return Cyc_Absynpp_exp_prec(_tmp207);case 14U: _LL1D: _LL1E:
 return 120;case 16U: _LL1F: _LL20:
 return 15;case 15U: _LL21: _LL22:
 goto _LL24;case 17U: _LL23: _LL24:
 goto _LL26;case 18U: _LL25: _LL26:
 goto _LL28;case 39U: _LL27: _LL28:
 goto _LL2A;case 40U: _LL29: _LL2A:
 goto _LL2C;case 38U: _LL2B: _LL2C:
 goto _LL2E;case 19U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 41U: _LL31: _LL32:
 return 130;case 21U: _LL33: _LL34:
 goto _LL36;case 22U: _LL35: _LL36:
 goto _LL38;case 23U: _LL37: _LL38:
 return 140;case 24U: _LL39: _LL3A:
 return 150;case 25U: _LL3B: _LL3C:
 goto _LL3E;case 26U: _LL3D: _LL3E:
 goto _LL40;case 27U: _LL3F: _LL40:
 goto _LL42;case 28U: _LL41: _LL42:
 goto _LL44;case 29U: _LL43: _LL44:
 goto _LL46;case 30U: _LL45: _LL46:
 goto _LL48;case 31U: _LL47: _LL48:
 goto _LL4A;case 32U: _LL49: _LL4A:
 goto _LL4C;case 33U: _LL4B: _LL4C:
 goto _LL4E;case 34U: _LL4D: _LL4E:
 goto _LL50;case 35U: _LL4F: _LL50:
 goto _LL52;case 36U: _LL51: _LL52:
 return 140;default: _LL53: _LL54:
 return 10000;}_LL0:;}
# 1072
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1076
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp208=e->r;void*_tmp209=_tmp208;struct Cyc_Absyn_Stmt*_tmp2DE;struct Cyc_Core_Opt*_tmp2DD;struct Cyc_List_List*_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DA;int _tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;void**_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D6;int _tmp2D5;struct Cyc_Absyn_Enumfield*_tmp2D4;struct Cyc_Absyn_Enumfield*_tmp2D3;struct Cyc_List_List*_tmp2D2;struct Cyc_Absyn_Datatypefield*_tmp2D1;struct Cyc_List_List*_tmp2D0;struct _tuple0*_tmp2CF;struct Cyc_List_List*_tmp2CE;struct Cyc_List_List*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;void*_tmp2CB;struct Cyc_Absyn_Vardecl*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_List_List*_tmp2C7;struct _tuple8*_tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_List_List*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;struct _dyneither_ptr*_tmp2C0;struct Cyc_Absyn_Exp*_tmp2BF;struct _dyneither_ptr*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;void*_tmp2BC;struct Cyc_List_List*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;struct _dyneither_ptr*_tmp2B9;int _tmp2B8;struct _dyneither_ptr _tmp2B7;void*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B4;void*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B0;void*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_List_List*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;enum Cyc_Absyn_Incrementor _tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Core_Opt*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;enum Cyc_Absyn_Primop _tmp29A;struct Cyc_List_List*_tmp299;struct _dyneither_ptr _tmp298;void*_tmp297;union Cyc_Absyn_Cnst _tmp296;switch(*((int*)_tmp209)){case 0U: _LL1: _tmp296=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL2:
 s=Cyc_Absynpp_cnst2doc(_tmp296);goto _LL0;case 1U: _LL3: _tmp297=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL4:
 s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp297));goto _LL0;case 2U: _LL5: _tmp298=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL6:
# 1083
 s=({struct Cyc_PP_Doc*_tmp20A[4U];({struct Cyc_PP_Doc*_tmp619=Cyc_PP_text(({const char*_tmp20B="__cyclone_pragma__";_tag_dyneither(_tmp20B,sizeof(char),19U);}));_tmp20A[0]=_tmp619;}),({struct Cyc_PP_Doc*_tmp618=Cyc_PP_text(({const char*_tmp20C="(";_tag_dyneither(_tmp20C,sizeof(char),2U);}));_tmp20A[1]=_tmp618;}),({struct Cyc_PP_Doc*_tmp617=Cyc_PP_text(_tmp298);_tmp20A[2]=_tmp617;}),({struct Cyc_PP_Doc*_tmp616=Cyc_PP_text(({const char*_tmp20D=")";_tag_dyneither(_tmp20D,sizeof(char),2U);}));_tmp20A[3]=_tmp616;});Cyc_PP_cat(_tag_dyneither(_tmp20A,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 3U: _LL7: _tmp29A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp299=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL8:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp29A,_tmp299);goto _LL0;case 4U: _LL9: _tmp29D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp29C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_tmp29B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp209)->f3;_LLA:
# 1086
 s=({struct Cyc_PP_Doc*_tmp20E[5U];({struct Cyc_PP_Doc*_tmp61F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29D);_tmp20E[0]=_tmp61F;}),({
struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp20F=" ";_tag_dyneither(_tmp20F,sizeof(char),2U);}));_tmp20E[1]=_tmp61E;}),
_tmp29C == 0?({struct Cyc_PP_Doc*_tmp61D=Cyc_PP_nil_doc();_tmp20E[2]=_tmp61D;}):({struct Cyc_PP_Doc*_tmp61C=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp29C->v);_tmp20E[2]=_tmp61C;}),({
struct Cyc_PP_Doc*_tmp61B=Cyc_PP_text(({const char*_tmp210="= ";_tag_dyneither(_tmp210,sizeof(char),3U);}));_tmp20E[3]=_tmp61B;}),({
struct Cyc_PP_Doc*_tmp61A=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29B);_tmp20E[4]=_tmp61A;});Cyc_PP_cat(_tag_dyneither(_tmp20E,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 5U: _LLB: _tmp29F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp29E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LLC: {
# 1093
struct Cyc_PP_Doc*_tmp211=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29F);
{enum Cyc_Absyn_Incrementor _tmp212=_tmp29E;switch(_tmp212){case Cyc_Absyn_PreInc: _LL56: _LL57:
 s=({struct Cyc_PP_Doc*_tmp213[2U];({struct Cyc_PP_Doc*_tmp620=Cyc_PP_text(({const char*_tmp214="++";_tag_dyneither(_tmp214,sizeof(char),3U);}));_tmp213[0]=_tmp620;}),_tmp213[1]=_tmp211;Cyc_PP_cat(_tag_dyneither(_tmp213,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PreDec: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp215[2U];({struct Cyc_PP_Doc*_tmp621=Cyc_PP_text(({const char*_tmp216="--";_tag_dyneither(_tmp216,sizeof(char),3U);}));_tmp215[0]=_tmp621;}),_tmp215[1]=_tmp211;Cyc_PP_cat(_tag_dyneither(_tmp215,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostInc: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp217[2U];_tmp217[0]=_tmp211,({struct Cyc_PP_Doc*_tmp622=Cyc_PP_text(({const char*_tmp218="++";_tag_dyneither(_tmp218,sizeof(char),3U);}));_tmp217[1]=_tmp622;});Cyc_PP_cat(_tag_dyneither(_tmp217,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostDec: _LL5C: _LL5D:
 goto _LL5F;default: _LL5E: _LL5F:
 s=({struct Cyc_PP_Doc*_tmp219[2U];_tmp219[0]=_tmp211,({struct Cyc_PP_Doc*_tmp623=Cyc_PP_text(({const char*_tmp21A="--";_tag_dyneither(_tmp21A,sizeof(char),3U);}));_tmp219[1]=_tmp623;});Cyc_PP_cat(_tag_dyneither(_tmp219,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;}_LL55:;}
# 1101
goto _LL0;}case 6U: _LLD: _tmp2A2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2A1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_tmp2A0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp209)->f3;_LLE:
# 1103
 s=({struct Cyc_PP_Doc*_tmp21B[5U];({struct Cyc_PP_Doc*_tmp628=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2);_tmp21B[0]=_tmp628;}),({struct Cyc_PP_Doc*_tmp627=Cyc_PP_text(({const char*_tmp21C=" ? ";_tag_dyneither(_tmp21C,sizeof(char),4U);}));_tmp21B[1]=_tmp627;}),({struct Cyc_PP_Doc*_tmp626=Cyc_Absynpp_exp2doc_prec(0,_tmp2A1);_tmp21B[2]=_tmp626;}),({
struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(({const char*_tmp21D=" : ";_tag_dyneither(_tmp21D,sizeof(char),4U);}));_tmp21B[3]=_tmp625;}),({struct Cyc_PP_Doc*_tmp624=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A0);_tmp21B[4]=_tmp624;});Cyc_PP_cat(_tag_dyneither(_tmp21B,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 7U: _LLF: _tmp2A4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2A3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL10:
# 1107
 s=({struct Cyc_PP_Doc*_tmp21E[3U];({struct Cyc_PP_Doc*_tmp62B=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A4);_tmp21E[0]=_tmp62B;}),({struct Cyc_PP_Doc*_tmp62A=Cyc_PP_text(({const char*_tmp21F=" && ";_tag_dyneither(_tmp21F,sizeof(char),5U);}));_tmp21E[1]=_tmp62A;}),({struct Cyc_PP_Doc*_tmp629=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A3);_tmp21E[2]=_tmp629;});Cyc_PP_cat(_tag_dyneither(_tmp21E,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 8U: _LL11: _tmp2A6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2A5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL12:
# 1110
 s=({struct Cyc_PP_Doc*_tmp220[3U];({struct Cyc_PP_Doc*_tmp62E=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A6);_tmp220[0]=_tmp62E;}),({struct Cyc_PP_Doc*_tmp62D=Cyc_PP_text(({const char*_tmp221=" || ";_tag_dyneither(_tmp221,sizeof(char),5U);}));_tmp220[1]=_tmp62D;}),({struct Cyc_PP_Doc*_tmp62C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A5);_tmp220[2]=_tmp62C;});Cyc_PP_cat(_tag_dyneither(_tmp220,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 9U: _LL13: _tmp2A8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2A7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL14:
# 1115
 s=({struct Cyc_PP_Doc*_tmp222[3U];({struct Cyc_PP_Doc*_tmp631=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp2A8);_tmp222[0]=_tmp631;}),({struct Cyc_PP_Doc*_tmp630=Cyc_PP_text(({const char*_tmp223=", ";_tag_dyneither(_tmp223,sizeof(char),3U);}));_tmp222[1]=_tmp630;}),({struct Cyc_PP_Doc*_tmp62F=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp2A7);_tmp222[2]=_tmp62F;});Cyc_PP_cat(_tag_dyneither(_tmp222,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 10U: _LL15: _tmp2AA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2A9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL16:
# 1118
 s=({struct Cyc_PP_Doc*_tmp224[4U];({struct Cyc_PP_Doc*_tmp635=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AA);_tmp224[0]=_tmp635;}),({
struct Cyc_PP_Doc*_tmp634=Cyc_PP_text(({const char*_tmp225="(";_tag_dyneither(_tmp225,sizeof(char),2U);}));_tmp224[1]=_tmp634;}),({
struct Cyc_PP_Doc*_tmp633=Cyc_Absynpp_exps2doc_prec(20,_tmp2A9);_tmp224[2]=_tmp633;}),({
struct Cyc_PP_Doc*_tmp632=Cyc_PP_text(({const char*_tmp226=")";_tag_dyneither(_tmp226,sizeof(char),2U);}));_tmp224[3]=_tmp632;});Cyc_PP_cat(_tag_dyneither(_tmp224,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 11U: _LL17: _tmp2AB=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL18:
# 1124
 s=({struct Cyc_PP_Doc*_tmp227[2U];({struct Cyc_PP_Doc*_tmp637=Cyc_PP_text(({const char*_tmp228="throw ";_tag_dyneither(_tmp228,sizeof(char),7U);}));_tmp227[0]=_tmp637;}),({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AB);_tmp227[1]=_tmp636;});Cyc_PP_cat(_tag_dyneither(_tmp227,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 12U: _LL19: _tmp2AC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL1A:
# 1127
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2AC);
goto _LL0;case 13U: _LL1B: _tmp2AD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL1C:
# 1130
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2AD);
goto _LL0;case 14U: _LL1D: _tmp2AF=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2AE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL1E:
# 1133
 s=({struct Cyc_PP_Doc*_tmp229[4U];({struct Cyc_PP_Doc*_tmp63B=Cyc_PP_text(({const char*_tmp22A="(";_tag_dyneither(_tmp22A,sizeof(char),2U);}));_tmp229[0]=_tmp63B;}),({
struct Cyc_PP_Doc*_tmp63A=Cyc_Absynpp_typ2doc(_tmp2AF);_tmp229[1]=_tmp63A;}),({
struct Cyc_PP_Doc*_tmp639=Cyc_PP_text(({const char*_tmp22B=")";_tag_dyneither(_tmp22B,sizeof(char),2U);}));_tmp229[2]=_tmp639;}),({
struct Cyc_PP_Doc*_tmp638=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AE);_tmp229[3]=_tmp638;});Cyc_PP_cat(_tag_dyneither(_tmp229,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 15U: _LL1F: _tmp2B0=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL20:
# 1139
 s=({struct Cyc_PP_Doc*_tmp22C[2U];({struct Cyc_PP_Doc*_tmp63D=Cyc_PP_text(({const char*_tmp22D="&";_tag_dyneither(_tmp22D,sizeof(char),2U);}));_tmp22C[0]=_tmp63D;}),({
struct Cyc_PP_Doc*_tmp63C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B0);_tmp22C[1]=_tmp63C;});Cyc_PP_cat(_tag_dyneither(_tmp22C,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 16U: _LL21: _tmp2B2=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2B1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL22:
# 1143
 if(_tmp2B2 == 0)
s=({struct Cyc_PP_Doc*_tmp22E[2U];({struct Cyc_PP_Doc*_tmp63F=Cyc_PP_text(({const char*_tmp22F="new ";_tag_dyneither(_tmp22F,sizeof(char),5U);}));_tmp22E[0]=_tmp63F;}),({struct Cyc_PP_Doc*_tmp63E=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B1);_tmp22E[1]=_tmp63E;});Cyc_PP_cat(_tag_dyneither(_tmp22E,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1146
s=({struct Cyc_PP_Doc*_tmp230[4U];({struct Cyc_PP_Doc*_tmp643=Cyc_PP_text(({const char*_tmp231="rnew(";_tag_dyneither(_tmp231,sizeof(char),6U);}));_tmp230[0]=_tmp643;}),({struct Cyc_PP_Doc*_tmp642=Cyc_Absynpp_exp2doc(_tmp2B2);_tmp230[1]=_tmp642;}),({struct Cyc_PP_Doc*_tmp641=Cyc_PP_text(({const char*_tmp232=") ";_tag_dyneither(_tmp232,sizeof(char),3U);}));_tmp230[2]=_tmp641;}),({
struct Cyc_PP_Doc*_tmp640=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B1);_tmp230[3]=_tmp640;});Cyc_PP_cat(_tag_dyneither(_tmp230,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;case 17U: _LL23: _tmp2B3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL24:
 s=({struct Cyc_PP_Doc*_tmp233[3U];({struct Cyc_PP_Doc*_tmp646=Cyc_PP_text(({const char*_tmp234="sizeof(";_tag_dyneither(_tmp234,sizeof(char),8U);}));_tmp233[0]=_tmp646;}),({struct Cyc_PP_Doc*_tmp645=Cyc_Absynpp_typ2doc(_tmp2B3);_tmp233[1]=_tmp645;}),({struct Cyc_PP_Doc*_tmp644=Cyc_PP_text(({const char*_tmp235=")";_tag_dyneither(_tmp235,sizeof(char),2U);}));_tmp233[2]=_tmp644;});Cyc_PP_cat(_tag_dyneither(_tmp233,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 18U: _LL25: _tmp2B4=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL26:
 s=({struct Cyc_PP_Doc*_tmp236[3U];({struct Cyc_PP_Doc*_tmp649=Cyc_PP_text(({const char*_tmp237="sizeof(";_tag_dyneither(_tmp237,sizeof(char),8U);}));_tmp236[0]=_tmp649;}),({struct Cyc_PP_Doc*_tmp648=Cyc_Absynpp_exp2doc(_tmp2B4);_tmp236[1]=_tmp648;}),({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp238=")";_tag_dyneither(_tmp238,sizeof(char),2U);}));_tmp236[2]=_tmp647;});Cyc_PP_cat(_tag_dyneither(_tmp236,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 41U: _LL27: _tmp2B5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL28:
 s=({struct Cyc_PP_Doc*_tmp239[3U];({struct Cyc_PP_Doc*_tmp64C=Cyc_PP_text(({const char*_tmp23A="__extension__(";_tag_dyneither(_tmp23A,sizeof(char),15U);}));_tmp239[0]=_tmp64C;}),({struct Cyc_PP_Doc*_tmp64B=Cyc_Absynpp_exp2doc(_tmp2B5);_tmp239[1]=_tmp64B;}),({struct Cyc_PP_Doc*_tmp64A=Cyc_PP_text(({const char*_tmp23B=")";_tag_dyneither(_tmp23B,sizeof(char),2U);}));_tmp239[2]=_tmp64A;});Cyc_PP_cat(_tag_dyneither(_tmp239,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 39U: _LL29: _tmp2B6=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL2A:
 s=({struct Cyc_PP_Doc*_tmp23C[3U];({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp23D="valueof(";_tag_dyneither(_tmp23D,sizeof(char),9U);}));_tmp23C[0]=_tmp64F;}),({struct Cyc_PP_Doc*_tmp64E=Cyc_Absynpp_typ2doc(_tmp2B6);_tmp23C[1]=_tmp64E;}),({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp23E=")";_tag_dyneither(_tmp23E,sizeof(char),2U);}));_tmp23C[2]=_tmp64D;});Cyc_PP_cat(_tag_dyneither(_tmp23C,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 40U: _LL2B: _tmp2B8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2B7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL2C:
# 1154
 if(_tmp2B8)
s=({struct Cyc_PP_Doc*_tmp23F[4U];({struct Cyc_PP_Doc*_tmp653=Cyc_PP_text(({const char*_tmp240="__asm__";_tag_dyneither(_tmp240,sizeof(char),8U);}));_tmp23F[0]=_tmp653;}),({struct Cyc_PP_Doc*_tmp652=Cyc_PP_text(({const char*_tmp241=" volatile (";_tag_dyneither(_tmp241,sizeof(char),12U);}));_tmp23F[1]=_tmp652;}),({struct Cyc_PP_Doc*_tmp651=Cyc_PP_text(_tmp2B7);_tmp23F[2]=_tmp651;}),({struct Cyc_PP_Doc*_tmp650=Cyc_PP_text(({const char*_tmp242=")";_tag_dyneither(_tmp242,sizeof(char),2U);}));_tmp23F[3]=_tmp650;});Cyc_PP_cat(_tag_dyneither(_tmp23F,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1157
s=({struct Cyc_PP_Doc*_tmp243[3U];({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text(({const char*_tmp244="__asm__(";_tag_dyneither(_tmp244,sizeof(char),9U);}));_tmp243[0]=_tmp656;}),({struct Cyc_PP_Doc*_tmp655=Cyc_PP_text(_tmp2B7);_tmp243[1]=_tmp655;}),({struct Cyc_PP_Doc*_tmp654=Cyc_PP_text(({const char*_tmp245=")";_tag_dyneither(_tmp245,sizeof(char),2U);}));_tmp243[2]=_tmp654;});Cyc_PP_cat(_tag_dyneither(_tmp243,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 38U: _LL2D: _tmp2BA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2B9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL2E:
# 1160
 s=({struct Cyc_PP_Doc*_tmp246[5U];({struct Cyc_PP_Doc*_tmp65B=Cyc_PP_text(({const char*_tmp247="tagcheck(";_tag_dyneither(_tmp247,sizeof(char),10U);}));_tmp246[0]=_tmp65B;}),({struct Cyc_PP_Doc*_tmp65A=Cyc_Absynpp_exp2doc(_tmp2BA);_tmp246[1]=_tmp65A;}),({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp248=".";_tag_dyneither(_tmp248,sizeof(char),2U);}));_tmp246[2]=_tmp659;}),({struct Cyc_PP_Doc*_tmp658=Cyc_PP_textptr(_tmp2B9);_tmp246[3]=_tmp658;}),({struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp249=")";_tag_dyneither(_tmp249,sizeof(char),2U);}));_tmp246[4]=_tmp657;});Cyc_PP_cat(_tag_dyneither(_tmp246,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 19U: _LL2F: _tmp2BC=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2BB=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL30:
# 1163
 s=({struct Cyc_PP_Doc*_tmp24A[3U];({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_text(({const char*_tmp24B="offsetof(";_tag_dyneither(_tmp24B,sizeof(char),10U);}));_tmp24A[0]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_Absynpp_typ2doc(_tmp2BC);_tmp24A[1]=_tmp65D;}),({struct Cyc_PP_Doc*_tmp65C=Cyc_PP_text(({const char*_tmp24C=",";_tag_dyneither(_tmp24C,sizeof(char),2U);}));_tmp24A[2]=_tmp65C;});Cyc_PP_cat(_tag_dyneither(_tmp24A,sizeof(struct Cyc_PP_Doc*),3U));});
for(0;_tmp2BB != 0;_tmp2BB=_tmp2BB->tl){
void*_tmp24D=(void*)_tmp2BB->hd;void*_tmp24E=_tmp24D;unsigned int _tmp257;struct _dyneither_ptr*_tmp256;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24E)->tag == 0U){_LL61: _tmp256=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24E)->f1;_LL62:
 s=({struct Cyc_PP_Doc*_tmp24F[3U];_tmp24F[0]=s,({struct Cyc_PP_Doc*_tmp661=Cyc_PP_textptr(_tmp256);_tmp24F[1]=_tmp661;}),
_tmp2BB->tl != 0?({struct Cyc_PP_Doc*_tmp660=Cyc_PP_text(({const char*_tmp250=".";_tag_dyneither(_tmp250,sizeof(char),2U);}));_tmp24F[2]=_tmp660;}):({struct Cyc_PP_Doc*_tmp65F=Cyc_PP_nil_doc();_tmp24F[2]=_tmp65F;});Cyc_PP_cat(_tag_dyneither(_tmp24F,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL60;}else{_LL63: _tmp257=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp24E)->f1;_LL64:
 s=({struct Cyc_PP_Doc*_tmp251[3U];_tmp251[0]=s,({struct Cyc_PP_Doc*_tmp665=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp254=({struct Cyc_Int_pa_PrintArg_struct _tmp548;_tmp548.tag=1U,_tmp548.f1=(unsigned long)((int)_tmp257);_tmp548;});void*_tmp252[1U];_tmp252[0]=& _tmp254;({struct _dyneither_ptr _tmp664=({const char*_tmp253="%d";_tag_dyneither(_tmp253,sizeof(char),3U);});Cyc_aprintf(_tmp664,_tag_dyneither(_tmp252,sizeof(void*),1U));});}));_tmp251[1]=_tmp665;}),
_tmp2BB->tl != 0?({struct Cyc_PP_Doc*_tmp663=Cyc_PP_text(({const char*_tmp255=".";_tag_dyneither(_tmp255,sizeof(char),2U);}));_tmp251[2]=_tmp663;}):({struct Cyc_PP_Doc*_tmp662=Cyc_PP_nil_doc();_tmp251[2]=_tmp662;});Cyc_PP_cat(_tag_dyneither(_tmp251,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL60;}_LL60:;}
# 1173
s=({struct Cyc_PP_Doc*_tmp258[2U];_tmp258[0]=s,({struct Cyc_PP_Doc*_tmp666=Cyc_PP_text(({const char*_tmp259=")";_tag_dyneither(_tmp259,sizeof(char),2U);}));_tmp258[1]=_tmp666;});Cyc_PP_cat(_tag_dyneither(_tmp258,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 20U: _LL31: _tmp2BD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL32:
# 1176
 s=({struct Cyc_PP_Doc*_tmp25A[2U];({struct Cyc_PP_Doc*_tmp668=Cyc_PP_text(({const char*_tmp25B="*";_tag_dyneither(_tmp25B,sizeof(char),2U);}));_tmp25A[0]=_tmp668;}),({struct Cyc_PP_Doc*_tmp667=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BD);_tmp25A[1]=_tmp667;});Cyc_PP_cat(_tag_dyneither(_tmp25A,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 21U: _LL33: _tmp2BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2BE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL34:
# 1179
 s=({struct Cyc_PP_Doc*_tmp25C[3U];({struct Cyc_PP_Doc*_tmp66B=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BF);_tmp25C[0]=_tmp66B;}),({struct Cyc_PP_Doc*_tmp66A=Cyc_PP_text(({const char*_tmp25D=".";_tag_dyneither(_tmp25D,sizeof(char),2U);}));_tmp25C[1]=_tmp66A;}),({struct Cyc_PP_Doc*_tmp669=Cyc_PP_textptr(_tmp2BE);_tmp25C[2]=_tmp669;});Cyc_PP_cat(_tag_dyneither(_tmp25C,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 22U: _LL35: _tmp2C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2C0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL36:
# 1182
 s=({struct Cyc_PP_Doc*_tmp25E[3U];({struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C1);_tmp25E[0]=_tmp66E;}),({struct Cyc_PP_Doc*_tmp66D=Cyc_PP_text(({const char*_tmp25F="->";_tag_dyneither(_tmp25F,sizeof(char),3U);}));_tmp25E[1]=_tmp66D;}),({struct Cyc_PP_Doc*_tmp66C=Cyc_PP_textptr(_tmp2C0);_tmp25E[2]=_tmp66C;});Cyc_PP_cat(_tag_dyneither(_tmp25E,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 23U: _LL37: _tmp2C3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2C2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL38:
# 1185
 s=({struct Cyc_PP_Doc*_tmp260[4U];({struct Cyc_PP_Doc*_tmp672=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C3);_tmp260[0]=_tmp672;}),({
struct Cyc_PP_Doc*_tmp671=Cyc_PP_text(({const char*_tmp261="[";_tag_dyneither(_tmp261,sizeof(char),2U);}));_tmp260[1]=_tmp671;}),({
struct Cyc_PP_Doc*_tmp670=Cyc_Absynpp_exp2doc(_tmp2C2);_tmp260[2]=_tmp670;}),({
struct Cyc_PP_Doc*_tmp66F=Cyc_PP_text(({const char*_tmp262="]";_tag_dyneither(_tmp262,sizeof(char),2U);}));_tmp260[3]=_tmp66F;});Cyc_PP_cat(_tag_dyneither(_tmp260,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 24U: _LL39: _tmp2C4=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL3A:
# 1191
 s=({struct Cyc_PP_Doc*_tmp263[4U];({struct Cyc_PP_Doc*_tmp676=Cyc_Absynpp_dollar();_tmp263[0]=_tmp676;}),({
struct Cyc_PP_Doc*_tmp675=Cyc_PP_text(({const char*_tmp264="(";_tag_dyneither(_tmp264,sizeof(char),2U);}));_tmp263[1]=_tmp675;}),({
struct Cyc_PP_Doc*_tmp674=Cyc_Absynpp_exps2doc_prec(20,_tmp2C4);_tmp263[2]=_tmp674;}),({
struct Cyc_PP_Doc*_tmp673=Cyc_PP_text(({const char*_tmp265=")";_tag_dyneither(_tmp265,sizeof(char),2U);}));_tmp263[3]=_tmp673;});Cyc_PP_cat(_tag_dyneither(_tmp263,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 25U: _LL3B: _tmp2C6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2C5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL3C:
# 1197
 s=({struct Cyc_PP_Doc*_tmp266[4U];({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp267="(";_tag_dyneither(_tmp267,sizeof(char),2U);}));_tmp266[0]=_tmp67B;}),({
struct Cyc_PP_Doc*_tmp67A=Cyc_Absynpp_typ2doc((*_tmp2C6).f3);_tmp266[1]=_tmp67A;}),({
struct Cyc_PP_Doc*_tmp679=Cyc_PP_text(({const char*_tmp268=")";_tag_dyneither(_tmp268,sizeof(char),2U);}));_tmp266[2]=_tmp679;}),({
struct Cyc_PP_Doc*_tmp678=({struct _dyneither_ptr _tmp677=({const char*_tmp269=",";_tag_dyneither(_tmp269,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp677,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C5));});_tmp266[3]=_tmp678;});Cyc_PP_cat(_tag_dyneither(_tmp266,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 26U: _LL3D: _tmp2C7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL3E:
# 1203
 s=({struct _dyneither_ptr _tmp67C=({const char*_tmp26A=",";_tag_dyneither(_tmp26A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp67C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C7));});
goto _LL0;case 27U: _LL3F: _tmp2CA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2C9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_tmp2C8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp209)->f3;_LL40:
# 1206
 s=({struct Cyc_PP_Doc*_tmp26B[8U];({struct Cyc_PP_Doc*_tmp684=Cyc_Absynpp_lb();_tmp26B[0]=_tmp684;}),({struct Cyc_PP_Doc*_tmp683=Cyc_PP_text(({const char*_tmp26C="for ";_tag_dyneither(_tmp26C,sizeof(char),5U);}));_tmp26B[1]=_tmp683;}),({
struct Cyc_PP_Doc*_tmp682=Cyc_PP_text(*(*_tmp2CA->name).f2);_tmp26B[2]=_tmp682;}),({
struct Cyc_PP_Doc*_tmp681=Cyc_PP_text(({const char*_tmp26D=" < ";_tag_dyneither(_tmp26D,sizeof(char),4U);}));_tmp26B[3]=_tmp681;}),({
struct Cyc_PP_Doc*_tmp680=Cyc_Absynpp_exp2doc(_tmp2C9);_tmp26B[4]=_tmp680;}),({
struct Cyc_PP_Doc*_tmp67F=Cyc_PP_text(({const char*_tmp26E=" : ";_tag_dyneither(_tmp26E,sizeof(char),4U);}));_tmp26B[5]=_tmp67F;}),({
struct Cyc_PP_Doc*_tmp67E=Cyc_Absynpp_exp2doc(_tmp2C8);_tmp26B[6]=_tmp67E;}),({
struct Cyc_PP_Doc*_tmp67D=Cyc_Absynpp_rb();_tmp26B[7]=_tmp67D;});Cyc_PP_cat(_tag_dyneither(_tmp26B,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 28U: _LL41: _tmp2CC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2CB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL42:
# 1215
 s=({struct Cyc_PP_Doc*_tmp26F[7U];({struct Cyc_PP_Doc*_tmp68B=Cyc_Absynpp_lb();_tmp26F[0]=_tmp68B;}),({struct Cyc_PP_Doc*_tmp68A=Cyc_PP_text(({const char*_tmp270="for x ";_tag_dyneither(_tmp270,sizeof(char),7U);}));_tmp26F[1]=_tmp68A;}),({
struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp271=" < ";_tag_dyneither(_tmp271,sizeof(char),4U);}));_tmp26F[2]=_tmp689;}),({
struct Cyc_PP_Doc*_tmp688=Cyc_Absynpp_exp2doc(_tmp2CC);_tmp26F[3]=_tmp688;}),({
struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp272=" : ";_tag_dyneither(_tmp272,sizeof(char),4U);}));_tmp26F[4]=_tmp687;}),({
struct Cyc_PP_Doc*_tmp686=Cyc_Absynpp_typ2doc(_tmp2CB);_tmp26F[5]=_tmp686;}),({
struct Cyc_PP_Doc*_tmp685=Cyc_Absynpp_rb();_tmp26F[6]=_tmp685;});Cyc_PP_cat(_tag_dyneither(_tmp26F,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 29U: _LL43: _tmp2CF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2CE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_tmp2CD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp209)->f3;_LL44: {
# 1223
struct Cyc_List_List*_tmp273=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2CD);
s=({struct Cyc_PP_Doc*_tmp274[2U];({struct Cyc_PP_Doc*_tmp68F=Cyc_Absynpp_qvar2doc(_tmp2CF);_tmp274[0]=_tmp68F;}),({
struct Cyc_PP_Doc*_tmp68E=({struct _dyneither_ptr _tmp68D=({const char*_tmp275=",";_tag_dyneither(_tmp275,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp68D,
_tmp2CE != 0?({struct Cyc_List_List*_tmp276=_cycalloc(sizeof(*_tmp276));({struct Cyc_PP_Doc*_tmp68C=Cyc_Absynpp_tps2doc(_tmp2CE);_tmp276->hd=_tmp68C;}),_tmp276->tl=_tmp273;_tmp276;}): _tmp273);});
# 1225
_tmp274[1]=_tmp68E;});Cyc_PP_cat(_tag_dyneither(_tmp274,sizeof(struct Cyc_PP_Doc*),2U));});
# 1227
goto _LL0;}case 30U: _LL45: _tmp2D0=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL46:
# 1229
 s=({struct _dyneither_ptr _tmp690=({const char*_tmp277=",";_tag_dyneither(_tmp277,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp690,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D0));});
goto _LL0;case 31U: _LL47: _tmp2D2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2D1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp209)->f3;_LL48:
# 1232
 if(_tmp2D2 == 0)
# 1234
s=Cyc_Absynpp_qvar2doc(_tmp2D1->name);else{
# 1236
s=({struct Cyc_PP_Doc*_tmp278[2U];({struct Cyc_PP_Doc*_tmp695=Cyc_Absynpp_qvar2doc(_tmp2D1->name);_tmp278[0]=_tmp695;}),({
struct Cyc_PP_Doc*_tmp694=({struct _dyneither_ptr _tmp693=({const char*_tmp279="(";_tag_dyneither(_tmp279,sizeof(char),2U);});struct _dyneither_ptr _tmp692=({const char*_tmp27A=")";_tag_dyneither(_tmp27A,sizeof(char),2U);});struct _dyneither_ptr _tmp691=({const char*_tmp27B=",";_tag_dyneither(_tmp27B,sizeof(char),2U);});Cyc_PP_egroup(_tmp693,_tmp692,_tmp691,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2D2));});_tmp278[1]=_tmp694;});Cyc_PP_cat(_tag_dyneither(_tmp278,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;case 32U: _LL49: _tmp2D3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL4A:
# 1240
 s=Cyc_Absynpp_qvar2doc(_tmp2D3->name);
goto _LL0;case 33U: _LL4B: _tmp2D4=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL4C:
# 1243
 s=Cyc_Absynpp_qvar2doc(_tmp2D4->name);
goto _LL0;case 34U: _LL4D: _tmp2D9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp209)->f1).is_calloc;_tmp2D8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp209)->f1).rgn;_tmp2D7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp209)->f1).elt_type;_tmp2D6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp209)->f1).num_elts;_tmp2D5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp209)->f1).inline_call;_LL4E:
# 1248
 if(_tmp2D9){
# 1250
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(_tmp2D7)),0U);
if(_tmp2D8 == 0)
s=({struct Cyc_PP_Doc*_tmp27C[5U];({struct Cyc_PP_Doc*_tmp69A=Cyc_PP_text(({const char*_tmp27D="calloc(";_tag_dyneither(_tmp27D,sizeof(char),8U);}));_tmp27C[0]=_tmp69A;}),({struct Cyc_PP_Doc*_tmp699=Cyc_Absynpp_exp2doc(_tmp2D6);_tmp27C[1]=_tmp699;}),({struct Cyc_PP_Doc*_tmp698=Cyc_PP_text(({const char*_tmp27E=",";_tag_dyneither(_tmp27E,sizeof(char),2U);}));_tmp27C[2]=_tmp698;}),({struct Cyc_PP_Doc*_tmp697=Cyc_Absynpp_exp2doc(st);_tmp27C[3]=_tmp697;}),({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp27F=")";_tag_dyneither(_tmp27F,sizeof(char),2U);}));_tmp27C[4]=_tmp696;});Cyc_PP_cat(_tag_dyneither(_tmp27C,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1254
s=({struct Cyc_PP_Doc*_tmp280[7U];({struct Cyc_PP_Doc*_tmp6A1=Cyc_PP_text(({const char*_tmp281="rcalloc(";_tag_dyneither(_tmp281,sizeof(char),9U);}));_tmp280[0]=_tmp6A1;}),({struct Cyc_PP_Doc*_tmp6A0=Cyc_Absynpp_exp2doc(_tmp2D8);_tmp280[1]=_tmp6A0;}),({struct Cyc_PP_Doc*_tmp69F=Cyc_PP_text(({const char*_tmp282=",";_tag_dyneither(_tmp282,sizeof(char),2U);}));_tmp280[2]=_tmp69F;}),({
struct Cyc_PP_Doc*_tmp69E=Cyc_Absynpp_exp2doc(_tmp2D6);_tmp280[3]=_tmp69E;}),({struct Cyc_PP_Doc*_tmp69D=Cyc_PP_text(({const char*_tmp283=",";_tag_dyneither(_tmp283,sizeof(char),2U);}));_tmp280[4]=_tmp69D;}),({struct Cyc_PP_Doc*_tmp69C=Cyc_Absynpp_exp2doc(st);_tmp280[5]=_tmp69C;}),({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_text(({const char*_tmp284=")";_tag_dyneither(_tmp284,sizeof(char),2U);}));_tmp280[6]=_tmp69B;});Cyc_PP_cat(_tag_dyneither(_tmp280,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1257
struct Cyc_Absyn_Exp*new_e;
# 1259
if(_tmp2D7 == 0)
new_e=_tmp2D6;else{
# 1262
new_e=({struct Cyc_Absyn_Exp*_tmp6A2=Cyc_Absyn_sizeoftype_exp(*_tmp2D7,0U);Cyc_Absyn_times_exp(_tmp6A2,_tmp2D6,0U);});}
# 1264
if(_tmp2D8 == 0)
s=({struct Cyc_PP_Doc*_tmp285[3U];({struct Cyc_PP_Doc*_tmp6A5=Cyc_PP_text(({const char*_tmp286="malloc(";_tag_dyneither(_tmp286,sizeof(char),8U);}));_tmp285[0]=_tmp6A5;}),({struct Cyc_PP_Doc*_tmp6A4=Cyc_Absynpp_exp2doc(new_e);_tmp285[1]=_tmp6A4;}),({struct Cyc_PP_Doc*_tmp6A3=Cyc_PP_text(({const char*_tmp287=")";_tag_dyneither(_tmp287,sizeof(char),2U);}));_tmp285[2]=_tmp6A3;});Cyc_PP_cat(_tag_dyneither(_tmp285,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1267
if(_tmp2D5)
s=({struct Cyc_PP_Doc*_tmp288[5U];({struct Cyc_PP_Doc*_tmp6AA=Cyc_PP_text(({const char*_tmp289="rmalloc_inline(";_tag_dyneither(_tmp289,sizeof(char),16U);}));_tmp288[0]=_tmp6AA;}),({struct Cyc_PP_Doc*_tmp6A9=Cyc_Absynpp_exp2doc(_tmp2D8);_tmp288[1]=_tmp6A9;}),({struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_text(({const char*_tmp28A=",";_tag_dyneither(_tmp28A,sizeof(char),2U);}));_tmp288[2]=_tmp6A8;}),({
struct Cyc_PP_Doc*_tmp6A7=Cyc_Absynpp_exp2doc(new_e);_tmp288[3]=_tmp6A7;}),({struct Cyc_PP_Doc*_tmp6A6=Cyc_PP_text(({const char*_tmp28B=")";_tag_dyneither(_tmp28B,sizeof(char),2U);}));_tmp288[4]=_tmp6A6;});Cyc_PP_cat(_tag_dyneither(_tmp288,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1271
s=({struct Cyc_PP_Doc*_tmp28C[5U];({struct Cyc_PP_Doc*_tmp6AF=Cyc_PP_text(({const char*_tmp28D="rmalloc(";_tag_dyneither(_tmp28D,sizeof(char),9U);}));_tmp28C[0]=_tmp6AF;}),({struct Cyc_PP_Doc*_tmp6AE=Cyc_Absynpp_exp2doc(_tmp2D8);_tmp28C[1]=_tmp6AE;}),({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp28E=",";_tag_dyneither(_tmp28E,sizeof(char),2U);}));_tmp28C[2]=_tmp6AD;}),({
struct Cyc_PP_Doc*_tmp6AC=Cyc_Absynpp_exp2doc(new_e);_tmp28C[3]=_tmp6AC;}),({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(({const char*_tmp28F=")";_tag_dyneither(_tmp28F,sizeof(char),2U);}));_tmp28C[4]=_tmp6AB;});Cyc_PP_cat(_tag_dyneither(_tmp28C,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1275
goto _LL0;case 35U: _LL4F: _tmp2DB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2DA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL50:
# 1277
 s=({struct Cyc_PP_Doc*_tmp290[3U];({struct Cyc_PP_Doc*_tmp6B2=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2DB);_tmp290[0]=_tmp6B2;}),({
struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_text(({const char*_tmp291=" :=: ";_tag_dyneither(_tmp291,sizeof(char),6U);}));_tmp290[1]=_tmp6B1;}),({
struct Cyc_PP_Doc*_tmp6B0=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2DA);_tmp290[2]=_tmp6B0;});Cyc_PP_cat(_tag_dyneither(_tmp290,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 36U: _LL51: _tmp2DD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_tmp2DC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp209)->f2;_LL52:
# 1283
 s=({struct _dyneither_ptr _tmp6B3=({const char*_tmp292=",";_tag_dyneither(_tmp292,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6B3,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2DC));});
goto _LL0;default: _LL53: _tmp2DE=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp209)->f1;_LL54:
# 1286
 s=({struct Cyc_PP_Doc*_tmp293[7U];({struct Cyc_PP_Doc*_tmp6BA=Cyc_PP_text(({const char*_tmp294="(";_tag_dyneither(_tmp294,sizeof(char),2U);}));_tmp293[0]=_tmp6BA;}),({struct Cyc_PP_Doc*_tmp6B9=Cyc_Absynpp_lb();_tmp293[1]=_tmp6B9;}),({struct Cyc_PP_Doc*_tmp6B8=Cyc_PP_blank_doc();_tmp293[2]=_tmp6B8;}),({
struct Cyc_PP_Doc*_tmp6B7=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2DE,1));_tmp293[3]=_tmp6B7;}),({
struct Cyc_PP_Doc*_tmp6B6=Cyc_PP_blank_doc();_tmp293[4]=_tmp6B6;}),({struct Cyc_PP_Doc*_tmp6B5=Cyc_Absynpp_rb();_tmp293[5]=_tmp6B5;}),({struct Cyc_PP_Doc*_tmp6B4=Cyc_PP_text(({const char*_tmp295=")";_tag_dyneither(_tmp295,sizeof(char),2U);}));_tmp293[6]=_tmp6B4;});Cyc_PP_cat(_tag_dyneither(_tmp293,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}_LL0:;}
# 1291
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2DF[3U];({struct Cyc_PP_Doc*_tmp6BC=Cyc_PP_text(({const char*_tmp2E0="(";_tag_dyneither(_tmp2E0,sizeof(char),2U);}));_tmp2DF[0]=_tmp6BC;}),_tmp2DF[1]=s,({struct Cyc_PP_Doc*_tmp6BB=Cyc_PP_text(({const char*_tmp2E1=")";_tag_dyneither(_tmp2E1,sizeof(char),2U);}));_tmp2DF[2]=_tmp6BB;});Cyc_PP_cat(_tag_dyneither(_tmp2DF,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1296
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2E2=d;struct _dyneither_ptr*_tmp2E9;struct Cyc_Absyn_Exp*_tmp2E8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E2)->tag == 0U){_LL1: _tmp2E8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2E2)->f1;_LL2:
 return({struct Cyc_PP_Doc*_tmp2E3[3U];({struct Cyc_PP_Doc*_tmp6BF=Cyc_PP_text(({const char*_tmp2E4=".[";_tag_dyneither(_tmp2E4,sizeof(char),3U);}));_tmp2E3[0]=_tmp6BF;}),({struct Cyc_PP_Doc*_tmp6BE=Cyc_Absynpp_exp2doc(_tmp2E8);_tmp2E3[1]=_tmp6BE;}),({struct Cyc_PP_Doc*_tmp6BD=Cyc_PP_text(({const char*_tmp2E5="]";_tag_dyneither(_tmp2E5,sizeof(char),2U);}));_tmp2E3[2]=_tmp6BD;});Cyc_PP_cat(_tag_dyneither(_tmp2E3,sizeof(struct Cyc_PP_Doc*),3U));});}else{_LL3: _tmp2E9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2E2)->f1;_LL4:
 return({struct Cyc_PP_Doc*_tmp2E6[2U];({struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_text(({const char*_tmp2E7=".";_tag_dyneither(_tmp2E7,sizeof(char),2U);}));_tmp2E6[0]=_tmp6C1;}),({struct Cyc_PP_Doc*_tmp6C0=Cyc_PP_textptr(_tmp2E9);_tmp2E6[1]=_tmp6C0;});Cyc_PP_cat(_tag_dyneither(_tmp2E6,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}
# 1303
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2EA[2U];({struct Cyc_PP_Doc*_tmp6C6=({struct _dyneither_ptr _tmp6C5=({const char*_tmp2EB="";_tag_dyneither(_tmp2EB,sizeof(char),1U);});struct _dyneither_ptr _tmp6C4=({const char*_tmp2EC="=";_tag_dyneither(_tmp2EC,sizeof(char),2U);});struct _dyneither_ptr _tmp6C3=({const char*_tmp2ED="=";_tag_dyneither(_tmp2ED,sizeof(char),2U);});Cyc_PP_egroup(_tmp6C5,_tmp6C4,_tmp6C3,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2EA[0]=_tmp6C6;}),({
struct Cyc_PP_Doc*_tmp6C2=Cyc_Absynpp_exp2doc((*de).f2);_tmp2EA[1]=_tmp6C2;});Cyc_PP_cat(_tag_dyneither(_tmp2EA,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1309
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _dyneither_ptr _tmp6C9=({const char*_tmp2EE="";_tag_dyneither(_tmp2EE,sizeof(char),1U);});struct _dyneither_ptr _tmp6C8=({const char*_tmp2EF="";_tag_dyneither(_tmp2EF,sizeof(char),1U);});struct _dyneither_ptr _tmp6C7=({const char*_tmp2F0=",";_tag_dyneither(_tmp2F0,sizeof(char),2U);});Cyc_PP_group(_tmp6C9,_tmp6C8,_tmp6C7,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1313
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _dyneither_ptr x=({char*_tmp305=({unsigned int _tmp304=28U + 1U;char*_tmp303=_cycalloc_atomic(_check_times(_tmp304,sizeof(char)));({{unsigned int _tmp549=28U;unsigned int i;for(i=0;i < _tmp549;++ i){_tmp303[i]='z';}_tmp303[_tmp549]=0;}0;});_tmp303;});_tag_dyneither(_tmp305,sizeof(char),29U);});
({struct _dyneither_ptr _tmp2F1=_dyneither_ptr_plus(x,sizeof(char),27);char _tmp2F2=*((char*)_check_dyneither_subscript(_tmp2F1,sizeof(char),0U));char _tmp2F3='\000';if(_get_dyneither_size(_tmp2F1,sizeof(char))== 1U  && (_tmp2F2 == 0  && _tmp2F3 != 0))_throw_arraybounds();*((char*)_tmp2F1.curr)=_tmp2F3;});
({struct _dyneither_ptr _tmp2F4=_dyneither_ptr_plus(x,sizeof(char),26);char _tmp2F5=*((char*)_check_dyneither_subscript(_tmp2F4,sizeof(char),0U));char _tmp2F6='L';if(_get_dyneither_size(_tmp2F4,sizeof(char))== 1U  && (_tmp2F5 == 0  && _tmp2F6 != 0))_throw_arraybounds();*((char*)_tmp2F4.curr)=_tmp2F6;});
({struct _dyneither_ptr _tmp2F7=_dyneither_ptr_plus(x,sizeof(char),25);char _tmp2F8=*((char*)_check_dyneither_subscript(_tmp2F7,sizeof(char),0U));char _tmp2F9='L';if(_get_dyneither_size(_tmp2F7,sizeof(char))== 1U  && (_tmp2F8 == 0  && _tmp2F9 != 0))_throw_arraybounds();*((char*)_tmp2F7.curr)=_tmp2F9;});
({struct _dyneither_ptr _tmp2FA=_dyneither_ptr_plus(x,sizeof(char),24);char _tmp2FB=*((char*)_check_dyneither_subscript(_tmp2FA,sizeof(char),0U));char _tmp2FC='U';if(_get_dyneither_size(_tmp2FA,sizeof(char))== 1U  && (_tmp2FB == 0  && _tmp2FC != 0))_throw_arraybounds();*((char*)_tmp2FA.curr)=_tmp2FC;});
({struct _dyneither_ptr _tmp2FD=_dyneither_ptr_plus(x,sizeof(char),23);char _tmp2FE=*((char*)_check_dyneither_subscript(_tmp2FD,sizeof(char),0U));char _tmp2FF='0';if(_get_dyneither_size(_tmp2FD,sizeof(char))== 1U  && (_tmp2FE == 0  && _tmp2FF != 0))_throw_arraybounds();*((char*)_tmp2FD.curr)=_tmp2FF;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _dyneither_ptr _tmp300=_dyneither_ptr_plus(x,sizeof(char),index);char _tmp301=*((char*)_check_dyneither_subscript(_tmp300,sizeof(char),0U));char _tmp302=c;if(_get_dyneither_size(_tmp300,sizeof(char))== 1U  && (_tmp301 == 0  && _tmp302 != 0))_throw_arraybounds();*((char*)_tmp300.curr)=_tmp302;});
i=i / (unsigned long long)10;
-- index;}
# 1327
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1331
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp306=c;struct _dyneither_ptr _tmp328;struct _dyneither_ptr _tmp327;struct _dyneither_ptr _tmp326;enum Cyc_Absyn_Sign _tmp325;long long _tmp324;enum Cyc_Absyn_Sign _tmp323;int _tmp322;enum Cyc_Absyn_Sign _tmp321;short _tmp320;struct _dyneither_ptr _tmp31F;enum Cyc_Absyn_Sign _tmp31E;char _tmp31D;switch((_tmp306.String_c).tag){case 2U: _LL1: _tmp31E=((_tmp306.Char_c).val).f1;_tmp31D=((_tmp306.Char_c).val).f2;_LL2:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp309=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,({struct _dyneither_ptr _tmp6CA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp31D));_tmp54A.f1=_tmp6CA;});_tmp54A;});void*_tmp307[1U];_tmp307[0]=& _tmp309;({struct _dyneither_ptr _tmp6CB=({const char*_tmp308="'%s'";_tag_dyneither(_tmp308,sizeof(char),5U);});Cyc_aprintf(_tmp6CB,_tag_dyneither(_tmp307,sizeof(void*),1U));});}));case 3U: _LL3: _tmp31F=(_tmp306.Wchar_c).val;_LL4:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30C=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,_tmp54B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31F);_tmp54B;});void*_tmp30A[1U];_tmp30A[0]=& _tmp30C;({struct _dyneither_ptr _tmp6CC=({const char*_tmp30B="L'%s'";_tag_dyneither(_tmp30B,sizeof(char),6U);});Cyc_aprintf(_tmp6CC,_tag_dyneither(_tmp30A,sizeof(void*),1U));});}));case 4U: _LL5: _tmp321=((_tmp306.Short_c).val).f1;_tmp320=((_tmp306.Short_c).val).f2;_LL6:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30F=({struct Cyc_Int_pa_PrintArg_struct _tmp54C;_tmp54C.tag=1U,_tmp54C.f1=(unsigned long)((int)_tmp320);_tmp54C;});void*_tmp30D[1U];_tmp30D[0]=& _tmp30F;({struct _dyneither_ptr _tmp6CD=({const char*_tmp30E="%d";_tag_dyneither(_tmp30E,sizeof(char),3U);});Cyc_aprintf(_tmp6CD,_tag_dyneither(_tmp30D,sizeof(void*),1U));});}));case 5U: _LL7: _tmp323=((_tmp306.Int_c).val).f1;_tmp322=((_tmp306.Int_c).val).f2;_LL8:
# 1337
 if((int)_tmp323 == (int)1U)
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp312=({struct Cyc_Int_pa_PrintArg_struct _tmp54D;_tmp54D.tag=1U,_tmp54D.f1=(unsigned int)_tmp322;_tmp54D;});void*_tmp310[1U];_tmp310[0]=& _tmp312;({struct _dyneither_ptr _tmp6CE=({const char*_tmp311="%uU";_tag_dyneither(_tmp311,sizeof(char),4U);});Cyc_aprintf(_tmp6CE,_tag_dyneither(_tmp310,sizeof(void*),1U));});}));else{
# 1340
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp315=({struct Cyc_Int_pa_PrintArg_struct _tmp54E;_tmp54E.tag=1U,_tmp54E.f1=(unsigned long)_tmp322;_tmp54E;});void*_tmp313[1U];_tmp313[0]=& _tmp315;({struct _dyneither_ptr _tmp6CF=({const char*_tmp314="%d";_tag_dyneither(_tmp314,sizeof(char),3U);});Cyc_aprintf(_tmp6CF,_tag_dyneither(_tmp313,sizeof(void*),1U));});}));}case 6U: _LL9: _tmp325=((_tmp306.LongLong_c).val).f1;_tmp324=((_tmp306.LongLong_c).val).f2;_LLA:
# 1343
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp324));case 7U: _LLB: _tmp326=((_tmp306.Float_c).val).f1;_LLC:
 return Cyc_PP_text(_tmp326);case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp316="NULL";_tag_dyneither(_tmp316,sizeof(char),5U);}));case 8U: _LLF: _tmp327=(_tmp306.String_c).val;_LL10:
# 1347
 return({struct Cyc_PP_Doc*_tmp317[3U];({struct Cyc_PP_Doc*_tmp6D2=Cyc_PP_text(({const char*_tmp318="\"";_tag_dyneither(_tmp318,sizeof(char),2U);}));_tmp317[0]=_tmp6D2;}),({struct Cyc_PP_Doc*_tmp6D1=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp327));_tmp317[1]=_tmp6D1;}),({struct Cyc_PP_Doc*_tmp6D0=Cyc_PP_text(({const char*_tmp319="\"";_tag_dyneither(_tmp319,sizeof(char),2U);}));_tmp317[2]=_tmp6D0;});Cyc_PP_cat(_tag_dyneither(_tmp317,sizeof(struct Cyc_PP_Doc*),3U));});default: _LL11: _tmp328=(_tmp306.Wstring_c).val;_LL12:
# 1349
 return({struct Cyc_PP_Doc*_tmp31A[3U];({struct Cyc_PP_Doc*_tmp6D5=Cyc_PP_text(({const char*_tmp31B="L\"";_tag_dyneither(_tmp31B,sizeof(char),3U);}));_tmp31A[0]=_tmp6D5;}),({struct Cyc_PP_Doc*_tmp6D4=Cyc_PP_text(_tmp328);_tmp31A[1]=_tmp6D4;}),({struct Cyc_PP_Doc*_tmp6D3=Cyc_PP_text(({const char*_tmp31C="\"";_tag_dyneither(_tmp31C,sizeof(char),2U);}));_tmp31A[2]=_tmp6D3;});Cyc_PP_cat(_tag_dyneither(_tmp31A,sizeof(struct Cyc_PP_Doc*),3U));});}_LL0:;}
# 1353
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)19U){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6D8=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32B=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({struct _dyneither_ptr _tmp6D6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp54F.f1=_tmp6D6;});_tmp54F;});void*_tmp329[1U];_tmp329[0]=& _tmp32B;({struct _dyneither_ptr _tmp6D7=({const char*_tmp32A="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp32A,sizeof(char),47U);});Cyc_aprintf(_tmp6D7,_tag_dyneither(_tmp329,sizeof(void*),1U));});});_tmp32C->f1=_tmp6D8;});_tmp32C;}));
# 1359
return({struct Cyc_PP_Doc*_tmp32D[3U];({struct Cyc_PP_Doc*_tmp6DB=Cyc_PP_text(({const char*_tmp32E="numelts(";_tag_dyneither(_tmp32E,sizeof(char),9U);}));_tmp32D[0]=_tmp6DB;}),({struct Cyc_PP_Doc*_tmp6DA=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp32D[1]=_tmp6DA;}),({struct Cyc_PP_Doc*_tmp6D9=Cyc_PP_text(({const char*_tmp32F=")";_tag_dyneither(_tmp32F,sizeof(char),2U);}));_tmp32D[2]=_tmp6D9;});Cyc_PP_cat(_tag_dyneither(_tmp32D,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1361
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6DE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp332=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({struct _dyneither_ptr _tmp6DC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp550.f1=_tmp6DC;});_tmp550;});void*_tmp330[1U];_tmp330[0]=& _tmp332;({struct _dyneither_ptr _tmp6DD=({const char*_tmp331="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp331,sizeof(char),38U);});Cyc_aprintf(_tmp6DD,_tag_dyneither(_tmp330,sizeof(void*),1U));});});_tmp333->f1=_tmp6DE;});_tmp333;}));else{
# 1365
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp334[3U];_tmp334[0]=ps,({struct Cyc_PP_Doc*_tmp6DF=Cyc_PP_text(({const char*_tmp335=" ";_tag_dyneither(_tmp335,sizeof(char),2U);}));_tmp334[1]=_tmp6DF;}),_tmp334[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp334,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6E2=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp338=({struct Cyc_String_pa_PrintArg_struct _tmp551;_tmp551.tag=0U,({struct _dyneither_ptr _tmp6E0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp551.f1=_tmp6E0;});_tmp551;});void*_tmp336[1U];_tmp336[0]=& _tmp338;({struct _dyneither_ptr _tmp6E1=({const char*_tmp337="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp337,sizeof(char),47U);});Cyc_aprintf(_tmp6E1,_tag_dyneither(_tmp336,sizeof(void*),1U));});});_tmp339->f1=_tmp6E2;});_tmp339;}));else{
# 1371
return({struct Cyc_PP_Doc*_tmp33A[5U];_tmp33A[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp6E4=Cyc_PP_text(({const char*_tmp33B=" ";_tag_dyneither(_tmp33B,sizeof(char),2U);}));_tmp33A[1]=_tmp6E4;}),_tmp33A[2]=ps,({struct Cyc_PP_Doc*_tmp6E3=Cyc_PP_text(({const char*_tmp33C=" ";_tag_dyneither(_tmp33C,sizeof(char),2U);}));_tmp33A[3]=_tmp6E3;}),_tmp33A[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_dyneither(_tmp33A,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1375
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp33D=p;switch(_tmp33D){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp33E="+";_tag_dyneither(_tmp33E,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp33F="*";_tag_dyneither(_tmp33F,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp340="-";_tag_dyneither(_tmp340,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp341="/";_tag_dyneither(_tmp341,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp342="\\%";_tag_dyneither(_tmp342,sizeof(char),3U);}):({const char*_tmp343="%";_tag_dyneither(_tmp343,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp344="==";_tag_dyneither(_tmp344,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp345="!=";_tag_dyneither(_tmp345,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp346=">";_tag_dyneither(_tmp346,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp347="<";_tag_dyneither(_tmp347,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp348=">=";_tag_dyneither(_tmp348,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp349="<=";_tag_dyneither(_tmp349,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp34A="!";_tag_dyneither(_tmp34A,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp34B="~";_tag_dyneither(_tmp34B,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp34C="&";_tag_dyneither(_tmp34C,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp34D="|";_tag_dyneither(_tmp34D,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp34E="^";_tag_dyneither(_tmp34E,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp34F="<<";_tag_dyneither(_tmp34F,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp350=">>";_tag_dyneither(_tmp350,sizeof(char),3U);});case Cyc_Absyn_Bitarshift: _LL25: _LL26:
 return({const char*_tmp351=">>>";_tag_dyneither(_tmp351,sizeof(char),4U);});case Cyc_Absyn_Numelts: _LL27: _LL28:
 return({const char*_tmp352="numelts";_tag_dyneither(_tmp352,sizeof(char),8U);});default: _LL29: _LL2A:
 return({const char*_tmp353="?";_tag_dyneither(_tmp353,sizeof(char),2U);});}_LL0:;}
# 1401
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1405
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp354=s->r;void*_tmp355=_tmp354;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp355)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1412
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp356=st->r;void*_tmp357=_tmp356;struct Cyc_Absyn_Stmt*_tmp3B1;struct Cyc_List_List*_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3AE;struct _dyneither_ptr*_tmp3AD;struct Cyc_Absyn_Stmt*_tmp3AC;struct Cyc_Absyn_Decl*_tmp3AB;struct Cyc_Absyn_Stmt*_tmp3AA;struct Cyc_List_List*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3A8;struct Cyc_List_List*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Stmt*_tmp3A3;struct _dyneither_ptr*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Stmt*_tmp3A0;struct Cyc_Absyn_Exp*_tmp39F;struct Cyc_Absyn_Stmt*_tmp39E;struct Cyc_Absyn_Stmt*_tmp39D;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_Absyn_Stmt*_tmp39B;struct Cyc_Absyn_Stmt*_tmp39A;struct Cyc_Absyn_Exp*_tmp399;switch(*((int*)_tmp357)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp358=";";_tag_dyneither(_tmp358,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp399=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_LL4:
 s=({struct Cyc_PP_Doc*_tmp359[2U];({struct Cyc_PP_Doc*_tmp6E6=Cyc_Absynpp_exp2doc(_tmp399);_tmp359[0]=_tmp6E6;}),({struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_text(({const char*_tmp35A=";";_tag_dyneither(_tmp35A,sizeof(char),2U);}));_tmp359[1]=_tmp6E5;});Cyc_PP_cat(_tag_dyneither(_tmp359,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 2U: _LL5: _tmp39B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp39A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp357)->f2;_LL6:
# 1418
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp39B))
s=({struct Cyc_PP_Doc*_tmp35B[7U];({struct Cyc_PP_Doc*_tmp6FB=Cyc_Absynpp_lb();_tmp35B[0]=_tmp6FB;}),({
struct Cyc_PP_Doc*_tmp6FA=Cyc_PP_blank_doc();_tmp35B[1]=_tmp6FA;}),({
struct Cyc_PP_Doc*_tmp6F9=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39B,0));_tmp35B[2]=_tmp6F9;}),({
struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_line_doc();_tmp35B[3]=_tmp6F8;}),({
struct Cyc_PP_Doc*_tmp6F7=Cyc_Absynpp_rb();_tmp35B[4]=_tmp6F7;}),({
struct Cyc_PP_Doc*_tmp6F6=Cyc_PP_line_doc();_tmp35B[5]=_tmp6F6;}),
Cyc_Absynpp_is_declaration(_tmp39A)?
stmtexp?({
struct Cyc_PP_Doc*_tmp6F5=({struct Cyc_PP_Doc*_tmp35C[7U];({struct Cyc_PP_Doc*_tmp6F4=Cyc_PP_text(({const char*_tmp35D="(";_tag_dyneither(_tmp35D,sizeof(char),2U);}));_tmp35C[0]=_tmp6F4;}),({struct Cyc_PP_Doc*_tmp6F3=Cyc_Absynpp_lb();_tmp35C[1]=_tmp6F3;}),({struct Cyc_PP_Doc*_tmp6F2=Cyc_PP_blank_doc();_tmp35C[2]=_tmp6F2;}),({
struct Cyc_PP_Doc*_tmp6F1=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp));_tmp35C[3]=_tmp6F1;}),({
struct Cyc_PP_Doc*_tmp6F0=Cyc_Absynpp_rb();_tmp35C[4]=_tmp6F0;}),({struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_text(({const char*_tmp35E=");";_tag_dyneither(_tmp35E,sizeof(char),3U);}));_tmp35C[5]=_tmp6EF;}),({
struct Cyc_PP_Doc*_tmp6EE=Cyc_PP_line_doc();_tmp35C[6]=_tmp6EE;});Cyc_PP_cat(_tag_dyneither(_tmp35C,sizeof(struct Cyc_PP_Doc*),7U));});
# 1428
_tmp35B[6]=_tmp6F5;}):({
# 1432
struct Cyc_PP_Doc*_tmp6ED=({struct Cyc_PP_Doc*_tmp35F[5U];({struct Cyc_PP_Doc*_tmp6EC=Cyc_Absynpp_lb();_tmp35F[0]=_tmp6EC;}),({struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_blank_doc();_tmp35F[1]=_tmp6EB;}),({
struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp));_tmp35F[2]=_tmp6EA;}),({
struct Cyc_PP_Doc*_tmp6E9=Cyc_Absynpp_rb();_tmp35F[3]=_tmp6E9;}),({
struct Cyc_PP_Doc*_tmp6E8=Cyc_PP_line_doc();_tmp35F[4]=_tmp6E8;});Cyc_PP_cat(_tag_dyneither(_tmp35F,sizeof(struct Cyc_PP_Doc*),5U));});
# 1432
_tmp35B[6]=_tmp6ED;}):({
# 1436
struct Cyc_PP_Doc*_tmp6E7=Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp);_tmp35B[6]=_tmp6E7;});Cyc_PP_cat(_tag_dyneither(_tmp35B,sizeof(struct Cyc_PP_Doc*),7U));});else{
if(Cyc_Absynpp_is_declaration(_tmp39A))
s=({struct Cyc_PP_Doc*_tmp360[4U];({struct Cyc_PP_Doc*_tmp70A=Cyc_Absynpp_stmt2doc(_tmp39B,0);_tmp360[0]=_tmp70A;}),({
struct Cyc_PP_Doc*_tmp709=Cyc_PP_line_doc();_tmp360[1]=_tmp709;}),
stmtexp?({
struct Cyc_PP_Doc*_tmp708=({struct Cyc_PP_Doc*_tmp361[6U];({struct Cyc_PP_Doc*_tmp707=Cyc_PP_text(({const char*_tmp362="(";_tag_dyneither(_tmp362,sizeof(char),2U);}));_tmp361[0]=_tmp707;}),({struct Cyc_PP_Doc*_tmp706=Cyc_Absynpp_lb();_tmp361[1]=_tmp706;}),({struct Cyc_PP_Doc*_tmp705=Cyc_PP_blank_doc();_tmp361[2]=_tmp705;}),({
struct Cyc_PP_Doc*_tmp704=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp));_tmp361[3]=_tmp704;}),({
struct Cyc_PP_Doc*_tmp703=Cyc_Absynpp_rb();_tmp361[4]=_tmp703;}),({struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp363=");";_tag_dyneither(_tmp363,sizeof(char),3U);}));_tmp361[5]=_tmp702;});Cyc_PP_cat(_tag_dyneither(_tmp361,sizeof(struct Cyc_PP_Doc*),6U));});
# 1441
_tmp360[2]=_tmp708;}):({
# 1445
struct Cyc_PP_Doc*_tmp701=({struct Cyc_PP_Doc*_tmp364[4U];({struct Cyc_PP_Doc*_tmp700=Cyc_Absynpp_lb();_tmp364[0]=_tmp700;}),({struct Cyc_PP_Doc*_tmp6FF=Cyc_PP_blank_doc();_tmp364[1]=_tmp6FF;}),({
struct Cyc_PP_Doc*_tmp6FE=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp));_tmp364[2]=_tmp6FE;}),({
struct Cyc_PP_Doc*_tmp6FD=Cyc_Absynpp_rb();_tmp364[3]=_tmp6FD;});Cyc_PP_cat(_tag_dyneither(_tmp364,sizeof(struct Cyc_PP_Doc*),4U));});
# 1445
_tmp360[2]=_tmp701;}),({
# 1448
struct Cyc_PP_Doc*_tmp6FC=Cyc_PP_line_doc();_tmp360[3]=_tmp6FC;});Cyc_PP_cat(_tag_dyneither(_tmp360,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1450
s=({struct Cyc_PP_Doc*_tmp365[3U];({struct Cyc_PP_Doc*_tmp70D=Cyc_Absynpp_stmt2doc(_tmp39B,0);_tmp365[0]=_tmp70D;}),({struct Cyc_PP_Doc*_tmp70C=Cyc_PP_line_doc();_tmp365[1]=_tmp70C;}),({struct Cyc_PP_Doc*_tmp70B=Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp);_tmp365[2]=_tmp70B;});Cyc_PP_cat(_tag_dyneither(_tmp365,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1453
s=({struct Cyc_PP_Doc*_tmp366[3U];({struct Cyc_PP_Doc*_tmp710=Cyc_Absynpp_stmt2doc(_tmp39B,0);_tmp366[0]=_tmp710;}),({struct Cyc_PP_Doc*_tmp70F=Cyc_PP_line_doc();_tmp366[1]=_tmp70F;}),({struct Cyc_PP_Doc*_tmp70E=Cyc_Absynpp_stmt2doc(_tmp39A,stmtexp);_tmp366[2]=_tmp70E;});Cyc_PP_cat(_tag_dyneither(_tmp366,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 3U: _LL7: _tmp39C=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_LL8:
# 1456
 if(_tmp39C == 0)
s=Cyc_PP_text(({const char*_tmp367="return;";_tag_dyneither(_tmp367,sizeof(char),8U);}));else{
# 1459
s=({struct Cyc_PP_Doc*_tmp368[3U];({struct Cyc_PP_Doc*_tmp714=Cyc_PP_text(({const char*_tmp369="return ";_tag_dyneither(_tmp369,sizeof(char),8U);}));_tmp368[0]=_tmp714;}),
_tmp39C == 0?({struct Cyc_PP_Doc*_tmp713=Cyc_PP_nil_doc();_tmp368[1]=_tmp713;}):({struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_exp2doc(_tmp39C);_tmp368[1]=_tmp712;}),({
struct Cyc_PP_Doc*_tmp711=Cyc_PP_text(({const char*_tmp36A=";";_tag_dyneither(_tmp36A,sizeof(char),2U);}));_tmp368[2]=_tmp711;});Cyc_PP_cat(_tag_dyneither(_tmp368,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 4U: _LL9: _tmp39F=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp39E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp357)->f2;_tmp39D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp357)->f3;_LLA: {
# 1464
int print_else;
{void*_tmp36B=_tmp39D->r;void*_tmp36C=_tmp36B;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp36C)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1469
s=({struct Cyc_PP_Doc*_tmp36D[8U];({struct Cyc_PP_Doc*_tmp727=Cyc_PP_text(({const char*_tmp36E="if (";_tag_dyneither(_tmp36E,sizeof(char),5U);}));_tmp36D[0]=_tmp727;}),({
struct Cyc_PP_Doc*_tmp726=Cyc_Absynpp_exp2doc(_tmp39F);_tmp36D[1]=_tmp726;}),({
struct Cyc_PP_Doc*_tmp725=Cyc_PP_text(({const char*_tmp36F=") ";_tag_dyneither(_tmp36F,sizeof(char),3U);}));_tmp36D[2]=_tmp725;}),({
struct Cyc_PP_Doc*_tmp724=Cyc_Absynpp_lb();_tmp36D[3]=_tmp724;}),({
struct Cyc_PP_Doc*_tmp723=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp370[2U];({struct Cyc_PP_Doc*_tmp722=Cyc_PP_line_doc();_tmp370[0]=_tmp722;}),({struct Cyc_PP_Doc*_tmp721=Cyc_Absynpp_stmt2doc(_tmp39E,0);_tmp370[1]=_tmp721;});Cyc_PP_cat(_tag_dyneither(_tmp370,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp36D[4]=_tmp723;}),({
struct Cyc_PP_Doc*_tmp720=Cyc_PP_line_doc();_tmp36D[5]=_tmp720;}),({
struct Cyc_PP_Doc*_tmp71F=Cyc_Absynpp_rb();_tmp36D[6]=_tmp71F;}),
print_else?({
struct Cyc_PP_Doc*_tmp71E=({struct Cyc_PP_Doc*_tmp371[6U];({struct Cyc_PP_Doc*_tmp71D=Cyc_PP_line_doc();_tmp371[0]=_tmp71D;}),({
struct Cyc_PP_Doc*_tmp71C=Cyc_PP_text(({const char*_tmp372="else ";_tag_dyneither(_tmp372,sizeof(char),6U);}));_tmp371[1]=_tmp71C;}),({
struct Cyc_PP_Doc*_tmp71B=Cyc_Absynpp_lb();_tmp371[2]=_tmp71B;}),({
struct Cyc_PP_Doc*_tmp71A=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp373[2U];({struct Cyc_PP_Doc*_tmp719=Cyc_PP_line_doc();_tmp373[0]=_tmp719;}),({struct Cyc_PP_Doc*_tmp718=Cyc_Absynpp_stmt2doc(_tmp39D,0);_tmp373[1]=_tmp718;});Cyc_PP_cat(_tag_dyneither(_tmp373,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp371[3]=_tmp71A;}),({
struct Cyc_PP_Doc*_tmp717=Cyc_PP_line_doc();_tmp371[4]=_tmp717;}),({
struct Cyc_PP_Doc*_tmp716=Cyc_Absynpp_rb();_tmp371[5]=_tmp716;});Cyc_PP_cat(_tag_dyneither(_tmp371,sizeof(struct Cyc_PP_Doc*),6U));});
# 1477
_tmp36D[7]=_tmp71E;}):({
# 1483
struct Cyc_PP_Doc*_tmp715=Cyc_PP_nil_doc();_tmp36D[7]=_tmp715;});Cyc_PP_cat(_tag_dyneither(_tmp36D,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 5U: _LLB: _tmp3A1=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp357)->f1).f1;_tmp3A0=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp357)->f2;_LLC:
# 1486
 s=({struct Cyc_PP_Doc*_tmp374[7U];({struct Cyc_PP_Doc*_tmp730=Cyc_PP_text(({const char*_tmp375="while (";_tag_dyneither(_tmp375,sizeof(char),8U);}));_tmp374[0]=_tmp730;}),({
struct Cyc_PP_Doc*_tmp72F=Cyc_Absynpp_exp2doc(_tmp3A1);_tmp374[1]=_tmp72F;}),({
struct Cyc_PP_Doc*_tmp72E=Cyc_PP_text(({const char*_tmp376=") ";_tag_dyneither(_tmp376,sizeof(char),3U);}));_tmp374[2]=_tmp72E;}),({
struct Cyc_PP_Doc*_tmp72D=Cyc_Absynpp_lb();_tmp374[3]=_tmp72D;}),({
struct Cyc_PP_Doc*_tmp72C=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp377[2U];({struct Cyc_PP_Doc*_tmp72B=Cyc_PP_line_doc();_tmp377[0]=_tmp72B;}),({struct Cyc_PP_Doc*_tmp72A=Cyc_Absynpp_stmt2doc(_tmp3A0,0);_tmp377[1]=_tmp72A;});Cyc_PP_cat(_tag_dyneither(_tmp377,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp374[4]=_tmp72C;}),({
struct Cyc_PP_Doc*_tmp729=Cyc_PP_line_doc();_tmp374[5]=_tmp729;}),({
struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_rb();_tmp374[6]=_tmp728;});Cyc_PP_cat(_tag_dyneither(_tmp374,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp378="break;";_tag_dyneither(_tmp378,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp379="continue;";_tag_dyneither(_tmp379,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp3A2=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_LL12:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37C=({struct Cyc_String_pa_PrintArg_struct _tmp552;_tmp552.tag=0U,_tmp552.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3A2);_tmp552;});void*_tmp37A[1U];_tmp37A[0]=& _tmp37C;({struct _dyneither_ptr _tmp731=({const char*_tmp37B="goto %s;";_tag_dyneither(_tmp37B,sizeof(char),9U);});Cyc_aprintf(_tmp731,_tag_dyneither(_tmp37A,sizeof(void*),1U));});}));goto _LL0;case 9U: _LL13: _tmp3A6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp3A5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp357)->f2).f1;_tmp3A4=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp357)->f3).f1;_tmp3A3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp357)->f4;_LL14:
# 1498
 s=({struct Cyc_PP_Doc*_tmp37D[11U];({struct Cyc_PP_Doc*_tmp73E=Cyc_PP_text(({const char*_tmp37E="for(";_tag_dyneither(_tmp37E,sizeof(char),5U);}));_tmp37D[0]=_tmp73E;}),({
struct Cyc_PP_Doc*_tmp73D=Cyc_Absynpp_exp2doc(_tmp3A6);_tmp37D[1]=_tmp73D;}),({
struct Cyc_PP_Doc*_tmp73C=Cyc_PP_text(({const char*_tmp37F="; ";_tag_dyneither(_tmp37F,sizeof(char),3U);}));_tmp37D[2]=_tmp73C;}),({
struct Cyc_PP_Doc*_tmp73B=Cyc_Absynpp_exp2doc(_tmp3A5);_tmp37D[3]=_tmp73B;}),({
struct Cyc_PP_Doc*_tmp73A=Cyc_PP_text(({const char*_tmp380="; ";_tag_dyneither(_tmp380,sizeof(char),3U);}));_tmp37D[4]=_tmp73A;}),({
struct Cyc_PP_Doc*_tmp739=Cyc_Absynpp_exp2doc(_tmp3A4);_tmp37D[5]=_tmp739;}),({
struct Cyc_PP_Doc*_tmp738=Cyc_PP_text(({const char*_tmp381=") ";_tag_dyneither(_tmp381,sizeof(char),3U);}));_tmp37D[6]=_tmp738;}),({
struct Cyc_PP_Doc*_tmp737=Cyc_Absynpp_lb();_tmp37D[7]=_tmp737;}),({
struct Cyc_PP_Doc*_tmp736=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp382[2U];({struct Cyc_PP_Doc*_tmp735=Cyc_PP_line_doc();_tmp382[0]=_tmp735;}),({struct Cyc_PP_Doc*_tmp734=Cyc_Absynpp_stmt2doc(_tmp3A3,0);_tmp382[1]=_tmp734;});Cyc_PP_cat(_tag_dyneither(_tmp382,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp37D[8]=_tmp736;}),({
struct Cyc_PP_Doc*_tmp733=Cyc_PP_line_doc();_tmp37D[9]=_tmp733;}),({
struct Cyc_PP_Doc*_tmp732=Cyc_Absynpp_rb();_tmp37D[10]=_tmp732;});Cyc_PP_cat(_tag_dyneither(_tmp37D,sizeof(struct Cyc_PP_Doc*),11U));});
goto _LL0;case 10U: _LL15: _tmp3A8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp3A7=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp357)->f2;_LL16:
# 1511
 s=({struct Cyc_PP_Doc*_tmp383[8U];({struct Cyc_PP_Doc*_tmp746=Cyc_PP_text(({const char*_tmp384="switch (";_tag_dyneither(_tmp384,sizeof(char),9U);}));_tmp383[0]=_tmp746;}),({
struct Cyc_PP_Doc*_tmp745=Cyc_Absynpp_exp2doc(_tmp3A8);_tmp383[1]=_tmp745;}),({
struct Cyc_PP_Doc*_tmp744=Cyc_PP_text(({const char*_tmp385=") ";_tag_dyneither(_tmp385,sizeof(char),3U);}));_tmp383[2]=_tmp744;}),({
struct Cyc_PP_Doc*_tmp743=Cyc_Absynpp_lb();_tmp383[3]=_tmp743;}),({
struct Cyc_PP_Doc*_tmp742=Cyc_PP_line_doc();_tmp383[4]=_tmp742;}),({
struct Cyc_PP_Doc*_tmp741=Cyc_Absynpp_switchclauses2doc(_tmp3A7);_tmp383[5]=_tmp741;}),({
struct Cyc_PP_Doc*_tmp740=Cyc_PP_line_doc();_tmp383[6]=_tmp740;}),({
struct Cyc_PP_Doc*_tmp73F=Cyc_Absynpp_rb();_tmp383[7]=_tmp73F;});Cyc_PP_cat(_tag_dyneither(_tmp383,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp357)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp386="fallthru;";_tag_dyneither(_tmp386,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp3A9=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_LL1A:
# 1522
 s=({struct Cyc_PP_Doc*_tmp387[3U];({struct Cyc_PP_Doc*_tmp749=Cyc_PP_text(({const char*_tmp388="fallthru(";_tag_dyneither(_tmp388,sizeof(char),10U);}));_tmp387[0]=_tmp749;}),({struct Cyc_PP_Doc*_tmp748=Cyc_Absynpp_exps2doc_prec(20,_tmp3A9);_tmp387[1]=_tmp748;}),({struct Cyc_PP_Doc*_tmp747=Cyc_PP_text(({const char*_tmp389=");";_tag_dyneither(_tmp389,sizeof(char),3U);}));_tmp387[2]=_tmp747;});Cyc_PP_cat(_tag_dyneither(_tmp387,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 12U: _LL1B: _tmp3AB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp3AA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp357)->f2;_LL1C:
# 1524
 s=({struct Cyc_PP_Doc*_tmp38A[3U];({struct Cyc_PP_Doc*_tmp74C=Cyc_Absynpp_decl2doc(_tmp3AB);_tmp38A[0]=_tmp74C;}),({
struct Cyc_PP_Doc*_tmp74B=Cyc_PP_line_doc();_tmp38A[1]=_tmp74B;}),({
struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_stmt2doc(_tmp3AA,stmtexp);_tmp38A[2]=_tmp74A;});Cyc_PP_cat(_tag_dyneither(_tmp38A,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 13U: _LL1D: _tmp3AD=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp3AC=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp357)->f2;_LL1E:
# 1529
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3AC)){
if(stmtexp)
s=({struct Cyc_PP_Doc*_tmp38B[8U];({struct Cyc_PP_Doc*_tmp754=Cyc_PP_textptr(_tmp3AD);_tmp38B[0]=_tmp754;}),({
struct Cyc_PP_Doc*_tmp753=Cyc_PP_text(({const char*_tmp38C=": (";_tag_dyneither(_tmp38C,sizeof(char),4U);}));_tmp38B[1]=_tmp753;}),({
struct Cyc_PP_Doc*_tmp752=Cyc_Absynpp_lb();_tmp38B[2]=_tmp752;}),({
struct Cyc_PP_Doc*_tmp751=Cyc_PP_line_doc();_tmp38B[3]=_tmp751;}),({
struct Cyc_PP_Doc*_tmp750=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3AC,1));_tmp38B[4]=_tmp750;}),({
struct Cyc_PP_Doc*_tmp74F=Cyc_PP_line_doc();_tmp38B[5]=_tmp74F;}),({
struct Cyc_PP_Doc*_tmp74E=Cyc_Absynpp_rb();_tmp38B[6]=_tmp74E;}),({struct Cyc_PP_Doc*_tmp74D=Cyc_PP_text(({const char*_tmp38D=");";_tag_dyneither(_tmp38D,sizeof(char),3U);}));_tmp38B[7]=_tmp74D;});Cyc_PP_cat(_tag_dyneither(_tmp38B,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1539
s=({struct Cyc_PP_Doc*_tmp38E[7U];({struct Cyc_PP_Doc*_tmp75B=Cyc_PP_textptr(_tmp3AD);_tmp38E[0]=_tmp75B;}),({
struct Cyc_PP_Doc*_tmp75A=Cyc_PP_text(({const char*_tmp38F=": ";_tag_dyneither(_tmp38F,sizeof(char),3U);}));_tmp38E[1]=_tmp75A;}),({
struct Cyc_PP_Doc*_tmp759=Cyc_Absynpp_lb();_tmp38E[2]=_tmp759;}),({
struct Cyc_PP_Doc*_tmp758=Cyc_PP_line_doc();_tmp38E[3]=_tmp758;}),({
struct Cyc_PP_Doc*_tmp757=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3AC,0));_tmp38E[4]=_tmp757;}),({
struct Cyc_PP_Doc*_tmp756=Cyc_PP_line_doc();_tmp38E[5]=_tmp756;}),({
struct Cyc_PP_Doc*_tmp755=Cyc_Absynpp_rb();_tmp38E[6]=_tmp755;});Cyc_PP_cat(_tag_dyneither(_tmp38E,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1547
s=({struct Cyc_PP_Doc*_tmp390[3U];({struct Cyc_PP_Doc*_tmp75E=Cyc_PP_textptr(_tmp3AD);_tmp390[0]=_tmp75E;}),({struct Cyc_PP_Doc*_tmp75D=Cyc_PP_text(({const char*_tmp391=": ";_tag_dyneither(_tmp391,sizeof(char),3U);}));_tmp390[1]=_tmp75D;}),({struct Cyc_PP_Doc*_tmp75C=Cyc_Absynpp_stmt2doc(_tmp3AC,stmtexp);_tmp390[2]=_tmp75C;});Cyc_PP_cat(_tag_dyneither(_tmp390,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 14U: _LL1F: _tmp3AF=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp3AE=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp357)->f2).f1;_LL20:
# 1550
 s=({struct Cyc_PP_Doc*_tmp392[9U];({struct Cyc_PP_Doc*_tmp767=Cyc_PP_text(({const char*_tmp393="do ";_tag_dyneither(_tmp393,sizeof(char),4U);}));_tmp392[0]=_tmp767;}),({
struct Cyc_PP_Doc*_tmp766=Cyc_Absynpp_lb();_tmp392[1]=_tmp766;}),({
struct Cyc_PP_Doc*_tmp765=Cyc_PP_line_doc();_tmp392[2]=_tmp765;}),({
struct Cyc_PP_Doc*_tmp764=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3AF,0));_tmp392[3]=_tmp764;}),({
struct Cyc_PP_Doc*_tmp763=Cyc_PP_line_doc();_tmp392[4]=_tmp763;}),({
struct Cyc_PP_Doc*_tmp762=Cyc_Absynpp_rb();_tmp392[5]=_tmp762;}),({
struct Cyc_PP_Doc*_tmp761=Cyc_PP_text(({const char*_tmp394=" while (";_tag_dyneither(_tmp394,sizeof(char),9U);}));_tmp392[6]=_tmp761;}),({
struct Cyc_PP_Doc*_tmp760=Cyc_Absynpp_exp2doc(_tmp3AE);_tmp392[7]=_tmp760;}),({
struct Cyc_PP_Doc*_tmp75F=Cyc_PP_text(({const char*_tmp395=");";_tag_dyneither(_tmp395,sizeof(char),3U);}));_tmp392[8]=_tmp75F;});Cyc_PP_cat(_tag_dyneither(_tmp392,sizeof(struct Cyc_PP_Doc*),9U));});
goto _LL0;default: _LL21: _tmp3B1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp357)->f1;_tmp3B0=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp357)->f2;_LL22:
# 1561
 s=({struct Cyc_PP_Doc*_tmp396[12U];({struct Cyc_PP_Doc*_tmp773=Cyc_PP_text(({const char*_tmp397="try ";_tag_dyneither(_tmp397,sizeof(char),5U);}));_tmp396[0]=_tmp773;}),({
struct Cyc_PP_Doc*_tmp772=Cyc_Absynpp_lb();_tmp396[1]=_tmp772;}),({
struct Cyc_PP_Doc*_tmp771=Cyc_PP_line_doc();_tmp396[2]=_tmp771;}),({
struct Cyc_PP_Doc*_tmp770=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3B1,0));_tmp396[3]=_tmp770;}),({
struct Cyc_PP_Doc*_tmp76F=Cyc_PP_line_doc();_tmp396[4]=_tmp76F;}),({
struct Cyc_PP_Doc*_tmp76E=Cyc_Absynpp_rb();_tmp396[5]=_tmp76E;}),({
struct Cyc_PP_Doc*_tmp76D=Cyc_PP_text(({const char*_tmp398=" catch ";_tag_dyneither(_tmp398,sizeof(char),8U);}));_tmp396[6]=_tmp76D;}),({
struct Cyc_PP_Doc*_tmp76C=Cyc_Absynpp_lb();_tmp396[7]=_tmp76C;}),({
struct Cyc_PP_Doc*_tmp76B=Cyc_PP_line_doc();_tmp396[8]=_tmp76B;}),({
struct Cyc_PP_Doc*_tmp76A=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3B0));_tmp396[9]=_tmp76A;}),({
struct Cyc_PP_Doc*_tmp769=Cyc_PP_line_doc();_tmp396[10]=_tmp769;}),({
struct Cyc_PP_Doc*_tmp768=Cyc_Absynpp_rb();_tmp396[11]=_tmp768;});Cyc_PP_cat(_tag_dyneither(_tmp396,sizeof(struct Cyc_PP_Doc*),12U));});
goto _LL0;}_LL0:;}
# 1575
return s;}
# 1578
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3B2=p->r;void*_tmp3B3=_tmp3B2;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Datatypefield*_tmp412;struct Cyc_List_List*_tmp411;int _tmp410;struct Cyc_Absyn_Datatypefield*_tmp40F;struct Cyc_Absyn_Enumfield*_tmp40E;struct Cyc_Absyn_Enumfield*_tmp40D;struct Cyc_List_List*_tmp40C;struct Cyc_List_List*_tmp40B;int _tmp40A;union Cyc_Absyn_AggrInfo _tmp409;struct Cyc_List_List*_tmp408;struct Cyc_List_List*_tmp407;int _tmp406;struct _tuple0*_tmp405;struct Cyc_List_List*_tmp404;int _tmp403;struct _tuple0*_tmp402;struct Cyc_Absyn_Vardecl*_tmp401;struct Cyc_Absyn_Pat*_tmp400;struct Cyc_Absyn_Vardecl*_tmp3FF;struct Cyc_Absyn_Pat*_tmp3FE;struct Cyc_List_List*_tmp3FD;int _tmp3FC;struct Cyc_Absyn_Tvar*_tmp3FB;struct Cyc_Absyn_Vardecl*_tmp3FA;struct Cyc_Absyn_Tvar*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3F8;struct Cyc_Absyn_Vardecl*_tmp3F7;struct Cyc_Absyn_Pat*_tmp3F6;struct Cyc_Absyn_Vardecl*_tmp3F5;struct _dyneither_ptr _tmp3F4;char _tmp3F3;enum Cyc_Absyn_Sign _tmp3F2;int _tmp3F1;switch(*((int*)_tmp3B3)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3B4="_";_tag_dyneither(_tmp3B4,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp3B5="NULL";_tag_dyneither(_tmp3B5,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp3F2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp3F1=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL6:
# 1584
 if((int)_tmp3F2 != (int)1U)
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B8=({struct Cyc_Int_pa_PrintArg_struct _tmp553;_tmp553.tag=1U,_tmp553.f1=(unsigned long)_tmp3F1;_tmp553;});void*_tmp3B6[1U];_tmp3B6[0]=& _tmp3B8;({struct _dyneither_ptr _tmp774=({const char*_tmp3B7="%d";_tag_dyneither(_tmp3B7,sizeof(char),3U);});Cyc_aprintf(_tmp774,_tag_dyneither(_tmp3B6,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3BB=({struct Cyc_Int_pa_PrintArg_struct _tmp554;_tmp554.tag=1U,_tmp554.f1=(unsigned int)_tmp3F1;_tmp554;});void*_tmp3B9[1U];_tmp3B9[0]=& _tmp3BB;({struct _dyneither_ptr _tmp775=({const char*_tmp3BA="%u";_tag_dyneither(_tmp3BA,sizeof(char),3U);});Cyc_aprintf(_tmp775,_tag_dyneither(_tmp3B9,sizeof(void*),1U));});}));}
goto _LL0;case 11U: _LL7: _tmp3F3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_LL8:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3BE=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0U,({struct _dyneither_ptr _tmp776=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3F3));_tmp555.f1=_tmp776;});_tmp555;});void*_tmp3BC[1U];_tmp3BC[0]=& _tmp3BE;({struct _dyneither_ptr _tmp777=({const char*_tmp3BD="'%s'";_tag_dyneither(_tmp3BD,sizeof(char),5U);});Cyc_aprintf(_tmp777,_tag_dyneither(_tmp3BC,sizeof(void*),1U));});}));goto _LL0;case 12U: _LL9: _tmp3F4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_LLA:
 s=Cyc_PP_text(_tmp3F4);goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2)->r)->tag == 0U){_LLB: _tmp3F5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_LLC:
# 1591
 s=Cyc_Absynpp_qvar2doc(_tmp3F5->name);goto _LL0;}else{_LLD: _tmp3F7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp3F6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LLE:
# 1593
 s=({struct Cyc_PP_Doc*_tmp3BF[3U];({struct Cyc_PP_Doc*_tmp77A=Cyc_Absynpp_qvar2doc(_tmp3F7->name);_tmp3BF[0]=_tmp77A;}),({struct Cyc_PP_Doc*_tmp779=Cyc_PP_text(({const char*_tmp3C0=" as ";_tag_dyneither(_tmp3C0,sizeof(char),5U);}));_tmp3BF[1]=_tmp779;}),({struct Cyc_PP_Doc*_tmp778=Cyc_Absynpp_pat2doc(_tmp3F6);_tmp3BF[2]=_tmp778;});Cyc_PP_cat(_tag_dyneither(_tmp3BF,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 2U: _LLF: _tmp3F9=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp3F8=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL10:
# 1595
 s=({struct Cyc_PP_Doc*_tmp3C1[5U];({struct Cyc_PP_Doc*_tmp77F=Cyc_PP_text(({const char*_tmp3C2="alias";_tag_dyneither(_tmp3C2,sizeof(char),6U);}));_tmp3C1[0]=_tmp77F;}),({struct Cyc_PP_Doc*_tmp77E=Cyc_PP_text(({const char*_tmp3C3=" <";_tag_dyneither(_tmp3C3,sizeof(char),3U);}));_tmp3C1[1]=_tmp77E;}),({struct Cyc_PP_Doc*_tmp77D=Cyc_Absynpp_tvar2doc(_tmp3F9);_tmp3C1[2]=_tmp77D;}),({struct Cyc_PP_Doc*_tmp77C=Cyc_PP_text(({const char*_tmp3C4="> ";_tag_dyneither(_tmp3C4,sizeof(char),3U);}));_tmp3C1[3]=_tmp77C;}),({
struct Cyc_PP_Doc*_tmp77B=Cyc_Absynpp_vardecl2doc(_tmp3F8);_tmp3C1[4]=_tmp77B;});Cyc_PP_cat(_tag_dyneither(_tmp3C1,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 4U: _LL11: _tmp3FB=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp3FA=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL12:
# 1599
 s=({struct Cyc_PP_Doc*_tmp3C5[4U];({struct Cyc_PP_Doc*_tmp783=Cyc_Absynpp_qvar2doc(_tmp3FA->name);_tmp3C5[0]=_tmp783;}),({struct Cyc_PP_Doc*_tmp782=Cyc_PP_text(({const char*_tmp3C6="<";_tag_dyneither(_tmp3C6,sizeof(char),2U);}));_tmp3C5[1]=_tmp782;}),({struct Cyc_PP_Doc*_tmp781=Cyc_Absynpp_tvar2doc(_tmp3FB);_tmp3C5[2]=_tmp781;}),({struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(({const char*_tmp3C7=">";_tag_dyneither(_tmp3C7,sizeof(char),2U);}));_tmp3C5[3]=_tmp780;});Cyc_PP_cat(_tag_dyneither(_tmp3C5,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 5U: _LL13: _tmp3FD=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp3FC=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL14:
# 1602
 s=({struct Cyc_PP_Doc*_tmp3C8[4U];({struct Cyc_PP_Doc*_tmp789=Cyc_Absynpp_dollar();_tmp3C8[0]=_tmp789;}),({struct Cyc_PP_Doc*_tmp788=Cyc_PP_text(({const char*_tmp3C9="(";_tag_dyneither(_tmp3C9,sizeof(char),2U);}));_tmp3C8[1]=_tmp788;}),({struct Cyc_PP_Doc*_tmp787=({struct _dyneither_ptr _tmp786=({const char*_tmp3CA=",";_tag_dyneither(_tmp3CA,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp786,_tmp3FD);});_tmp3C8[2]=_tmp787;}),
_tmp3FC?({struct Cyc_PP_Doc*_tmp785=Cyc_PP_text(({const char*_tmp3CB=", ...)";_tag_dyneither(_tmp3CB,sizeof(char),7U);}));_tmp3C8[3]=_tmp785;}):({struct Cyc_PP_Doc*_tmp784=Cyc_PP_text(({const char*_tmp3CC=")";_tag_dyneither(_tmp3CC,sizeof(char),2U);}));_tmp3C8[3]=_tmp784;});Cyc_PP_cat(_tag_dyneither(_tmp3C8,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 6U: _LL15: _tmp3FE=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_LL16:
# 1606
 s=({struct Cyc_PP_Doc*_tmp3CD[2U];({struct Cyc_PP_Doc*_tmp78B=Cyc_PP_text(({const char*_tmp3CE="&";_tag_dyneither(_tmp3CE,sizeof(char),2U);}));_tmp3CD[0]=_tmp78B;}),({struct Cyc_PP_Doc*_tmp78A=Cyc_Absynpp_pat2doc(_tmp3FE);_tmp3CD[1]=_tmp78A;});Cyc_PP_cat(_tag_dyneither(_tmp3CD,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2)->r)->tag == 0U){_LL17: _tmp3FF=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_LL18:
# 1609
 s=({struct Cyc_PP_Doc*_tmp3CF[2U];({struct Cyc_PP_Doc*_tmp78D=Cyc_PP_text(({const char*_tmp3D0="*";_tag_dyneither(_tmp3D0,sizeof(char),2U);}));_tmp3CF[0]=_tmp78D;}),({struct Cyc_PP_Doc*_tmp78C=Cyc_Absynpp_qvar2doc(_tmp3FF->name);_tmp3CF[1]=_tmp78C;});Cyc_PP_cat(_tag_dyneither(_tmp3CF,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}else{_LL19: _tmp401=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp400=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL1A:
# 1612
 s=({struct Cyc_PP_Doc*_tmp3D1[4U];({struct Cyc_PP_Doc*_tmp791=Cyc_PP_text(({const char*_tmp3D2="*";_tag_dyneither(_tmp3D2,sizeof(char),2U);}));_tmp3D1[0]=_tmp791;}),({struct Cyc_PP_Doc*_tmp790=Cyc_Absynpp_qvar2doc(_tmp401->name);_tmp3D1[1]=_tmp790;}),({struct Cyc_PP_Doc*_tmp78F=Cyc_PP_text(({const char*_tmp3D3=" as ";_tag_dyneither(_tmp3D3,sizeof(char),5U);}));_tmp3D1[2]=_tmp78F;}),({struct Cyc_PP_Doc*_tmp78E=Cyc_Absynpp_pat2doc(_tmp400);_tmp3D1[3]=_tmp78E;});Cyc_PP_cat(_tag_dyneither(_tmp3D1,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 15U: _LL1B: _tmp402=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_LL1C:
# 1615
 s=Cyc_Absynpp_qvar2doc(_tmp402);
goto _LL0;case 16U: _LL1D: _tmp405=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp404=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_tmp403=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3B3)->f3;_LL1E: {
# 1618
struct _dyneither_ptr term=_tmp403?({const char*_tmp3D7=", ...)";_tag_dyneither(_tmp3D7,sizeof(char),7U);}):({const char*_tmp3D8=")";_tag_dyneither(_tmp3D8,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3D4[2U];({struct Cyc_PP_Doc*_tmp796=Cyc_Absynpp_qvar2doc(_tmp405);_tmp3D4[0]=_tmp796;}),({struct Cyc_PP_Doc*_tmp795=({struct _dyneither_ptr _tmp794=({const char*_tmp3D5="(";_tag_dyneither(_tmp3D5,sizeof(char),2U);});struct _dyneither_ptr _tmp793=term;struct _dyneither_ptr _tmp792=({const char*_tmp3D6=",";_tag_dyneither(_tmp3D6,sizeof(char),2U);});Cyc_PP_group(_tmp794,_tmp793,_tmp792,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp404));});_tmp3D4[1]=_tmp795;});Cyc_PP_cat(_tag_dyneither(_tmp3D4,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1 != 0){_LL1F: _tmp409=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_tmp408=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_tmp407=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f3;_tmp406=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f4;_LL20: {
# 1622
struct _dyneither_ptr term=_tmp406?({const char*_tmp3E2=", ...}";_tag_dyneither(_tmp3E2,sizeof(char),7U);}):({const char*_tmp3E3="}";_tag_dyneither(_tmp3E3,sizeof(char),2U);});
struct _tuple10 _tmp3D9=Cyc_Absyn_aggr_kinded_name(_tmp409);struct _tuple10 _tmp3DA=_tmp3D9;struct _tuple0*_tmp3E1;_LL2E: _tmp3E1=_tmp3DA.f2;_LL2F:;
s=({struct Cyc_PP_Doc*_tmp3DB[4U];({struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_qvar2doc(_tmp3E1);_tmp3DB[0]=_tmp7A0;}),({struct Cyc_PP_Doc*_tmp79F=Cyc_Absynpp_lb();_tmp3DB[1]=_tmp79F;}),({
struct Cyc_PP_Doc*_tmp79E=({struct _dyneither_ptr _tmp79D=({const char*_tmp3DC="[";_tag_dyneither(_tmp3DC,sizeof(char),2U);});struct _dyneither_ptr _tmp79C=({const char*_tmp3DD="]";_tag_dyneither(_tmp3DD,sizeof(char),2U);});struct _dyneither_ptr _tmp79B=({const char*_tmp3DE=",";_tag_dyneither(_tmp3DE,sizeof(char),2U);});Cyc_PP_egroup(_tmp79D,_tmp79C,_tmp79B,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp408));});_tmp3DB[2]=_tmp79E;}),({
struct Cyc_PP_Doc*_tmp79A=({struct _dyneither_ptr _tmp799=({const char*_tmp3DF="";_tag_dyneither(_tmp3DF,sizeof(char),1U);});struct _dyneither_ptr _tmp798=term;struct _dyneither_ptr _tmp797=({const char*_tmp3E0=",";_tag_dyneither(_tmp3E0,sizeof(char),2U);});Cyc_PP_group(_tmp799,_tmp798,_tmp797,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp407));});_tmp3DB[3]=_tmp79A;});Cyc_PP_cat(_tag_dyneither(_tmp3DB,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}else{_LL21: _tmp40C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_tmp40B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f3;_tmp40A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3B3)->f4;_LL22: {
# 1629
struct _dyneither_ptr term=_tmp40A?({const char*_tmp3EA=", ...}";_tag_dyneither(_tmp3EA,sizeof(char),7U);}):({const char*_tmp3EB="}";_tag_dyneither(_tmp3EB,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3E4[3U];({struct Cyc_PP_Doc*_tmp7A9=Cyc_Absynpp_lb();_tmp3E4[0]=_tmp7A9;}),({
struct Cyc_PP_Doc*_tmp7A8=({struct _dyneither_ptr _tmp7A7=({const char*_tmp3E5="[";_tag_dyneither(_tmp3E5,sizeof(char),2U);});struct _dyneither_ptr _tmp7A6=({const char*_tmp3E6="]";_tag_dyneither(_tmp3E6,sizeof(char),2U);});struct _dyneither_ptr _tmp7A5=({const char*_tmp3E7=",";_tag_dyneither(_tmp3E7,sizeof(char),2U);});Cyc_PP_egroup(_tmp7A7,_tmp7A6,_tmp7A5,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp40C));});_tmp3E4[1]=_tmp7A8;}),({
struct Cyc_PP_Doc*_tmp7A4=({struct _dyneither_ptr _tmp7A3=({const char*_tmp3E8="";_tag_dyneither(_tmp3E8,sizeof(char),1U);});struct _dyneither_ptr _tmp7A2=term;struct _dyneither_ptr _tmp7A1=({const char*_tmp3E9=",";_tag_dyneither(_tmp3E9,sizeof(char),2U);});Cyc_PP_group(_tmp7A3,_tmp7A2,_tmp7A1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp40B));});_tmp3E4[2]=_tmp7A4;});Cyc_PP_cat(_tag_dyneither(_tmp3E4,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp40D=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL24:
 _tmp40E=_tmp40D;goto _LL26;case 14U: _LL25: _tmp40E=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL26:
 s=Cyc_Absynpp_qvar2doc(_tmp40E->name);goto _LL0;case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3B3)->f3 == 0){_LL27: _tmp40F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_LL28:
 s=Cyc_Absynpp_qvar2doc(_tmp40F->name);goto _LL0;}else{_LL29: _tmp412=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3B3)->f2;_tmp411=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3B3)->f3;_tmp410=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3B3)->f4;_LL2A: {
# 1638
struct _dyneither_ptr term=_tmp410?({const char*_tmp3EF=", ...)";_tag_dyneither(_tmp3EF,sizeof(char),7U);}):({const char*_tmp3F0=")";_tag_dyneither(_tmp3F0,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3EC[2U];({struct Cyc_PP_Doc*_tmp7AE=Cyc_Absynpp_qvar2doc(_tmp412->name);_tmp3EC[0]=_tmp7AE;}),({struct Cyc_PP_Doc*_tmp7AD=({struct _dyneither_ptr _tmp7AC=({const char*_tmp3ED="(";_tag_dyneither(_tmp3ED,sizeof(char),2U);});struct _dyneither_ptr _tmp7AB=term;struct _dyneither_ptr _tmp7AA=({const char*_tmp3EE=",";_tag_dyneither(_tmp3EE,sizeof(char),2U);});Cyc_PP_egroup(_tmp7AC,_tmp7AB,_tmp7AA,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp411));});_tmp3EC[1]=_tmp7AD;});Cyc_PP_cat(_tag_dyneither(_tmp3EC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp413=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3B3)->f1;_LL2C:
# 1642
 s=Cyc_Absynpp_exp2doc(_tmp413);goto _LL0;}_LL0:;}
# 1644
return s;}
# 1647
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp414[2U];({struct Cyc_PP_Doc*_tmp7B3=({struct _dyneither_ptr _tmp7B2=({const char*_tmp415="";_tag_dyneither(_tmp415,sizeof(char),1U);});struct _dyneither_ptr _tmp7B1=({const char*_tmp416="=";_tag_dyneither(_tmp416,sizeof(char),2U);});struct _dyneither_ptr _tmp7B0=({const char*_tmp417="=";_tag_dyneither(_tmp417,sizeof(char),2U);});Cyc_PP_egroup(_tmp7B2,_tmp7B1,_tmp7B0,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp414[0]=_tmp7B3;}),({
struct Cyc_PP_Doc*_tmp7AF=Cyc_Absynpp_pat2doc((*dp).f2);_tmp414[1]=_tmp7AF;});Cyc_PP_cat(_tag_dyneither(_tmp414,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1652
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp418[2U];({struct Cyc_PP_Doc*_tmp7B7=Cyc_PP_text(({const char*_tmp419="default: ";_tag_dyneither(_tmp419,sizeof(char),10U);}));_tmp418[0]=_tmp7B7;}),({
struct Cyc_PP_Doc*_tmp7B6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41A[2U];({struct Cyc_PP_Doc*_tmp7B5=Cyc_PP_line_doc();_tmp41A[0]=_tmp7B5;}),({struct Cyc_PP_Doc*_tmp7B4=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41A[1]=_tmp7B4;});Cyc_PP_cat(_tag_dyneither(_tmp41A,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp418[1]=_tmp7B6;});Cyc_PP_cat(_tag_dyneither(_tmp418,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp41B[4U];({struct Cyc_PP_Doc*_tmp7BD=Cyc_PP_text(({const char*_tmp41C="case ";_tag_dyneither(_tmp41C,sizeof(char),6U);}));_tmp41B[0]=_tmp7BD;}),({
struct Cyc_PP_Doc*_tmp7BC=Cyc_Absynpp_pat2doc(c->pattern);_tmp41B[1]=_tmp7BC;}),({
struct Cyc_PP_Doc*_tmp7BB=Cyc_PP_text(({const char*_tmp41D=": ";_tag_dyneither(_tmp41D,sizeof(char),3U);}));_tmp41B[2]=_tmp7BB;}),({
struct Cyc_PP_Doc*_tmp7BA=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41E[2U];({struct Cyc_PP_Doc*_tmp7B9=Cyc_PP_line_doc();_tmp41E[0]=_tmp7B9;}),({struct Cyc_PP_Doc*_tmp7B8=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41E[1]=_tmp7B8;});Cyc_PP_cat(_tag_dyneither(_tmp41E,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp41B[3]=_tmp7BA;});Cyc_PP_cat(_tag_dyneither(_tmp41B,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1662
return({struct Cyc_PP_Doc*_tmp41F[6U];({struct Cyc_PP_Doc*_tmp7C5=Cyc_PP_text(({const char*_tmp420="case ";_tag_dyneither(_tmp420,sizeof(char),6U);}));_tmp41F[0]=_tmp7C5;}),({
struct Cyc_PP_Doc*_tmp7C4=Cyc_Absynpp_pat2doc(c->pattern);_tmp41F[1]=_tmp7C4;}),({
struct Cyc_PP_Doc*_tmp7C3=Cyc_PP_text(({const char*_tmp421=" && ";_tag_dyneither(_tmp421,sizeof(char),5U);}));_tmp41F[2]=_tmp7C3;}),({
struct Cyc_PP_Doc*_tmp7C2=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp41F[3]=_tmp7C2;}),({
struct Cyc_PP_Doc*_tmp7C1=Cyc_PP_text(({const char*_tmp422=": ";_tag_dyneither(_tmp422,sizeof(char),3U);}));_tmp41F[4]=_tmp7C1;}),({
struct Cyc_PP_Doc*_tmp7C0=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp423[2U];({struct Cyc_PP_Doc*_tmp7BF=Cyc_PP_line_doc();_tmp423[0]=_tmp7BF;}),({struct Cyc_PP_Doc*_tmp7BE=Cyc_Absynpp_stmt2doc(c->body,0);_tmp423[1]=_tmp7BE;});Cyc_PP_cat(_tag_dyneither(_tmp423,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp41F[5]=_tmp7C0;});Cyc_PP_cat(_tag_dyneither(_tmp41F,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1670
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _dyneither_ptr _tmp7C6=({const char*_tmp424="";_tag_dyneither(_tmp424,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp7C6,cs);});}
# 1674
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1678
return({struct Cyc_PP_Doc*_tmp425[3U];({struct Cyc_PP_Doc*_tmp7C9=Cyc_Absynpp_qvar2doc(f->name);_tmp425[0]=_tmp7C9;}),({struct Cyc_PP_Doc*_tmp7C8=Cyc_PP_text(({const char*_tmp426=" = ";_tag_dyneither(_tmp426,sizeof(char),4U);}));_tmp425[1]=_tmp7C8;}),({struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp425[2]=_tmp7C7;});Cyc_PP_cat(_tag_dyneither(_tmp425,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1681
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _dyneither_ptr _tmp7CA=({const char*_tmp427=",";_tag_dyneither(_tmp427,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp7CA,fs);});}
# 1685
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1689
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _dyneither_ptr _tmp7CB=({const char*_tmp428=",";_tag_dyneither(_tmp428,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp7CB,vds);});}
# 1693
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp429=vd;enum Cyc_Absyn_Scope _tmp43B;struct _tuple0*_tmp43A;unsigned int _tmp439;struct Cyc_Absyn_Tqual _tmp438;void*_tmp437;struct Cyc_Absyn_Exp*_tmp436;struct Cyc_List_List*_tmp435;_LL1: _tmp43B=_tmp429->sc;_tmp43A=_tmp429->name;_tmp439=_tmp429->varloc;_tmp438=_tmp429->tq;_tmp437=_tmp429->type;_tmp436=_tmp429->initializer;_tmp435=_tmp429->attributes;_LL2:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp43A);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp435);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp42A=Cyc_Cyclone_c_compiler;if(_tmp42A == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1702
{void*_tmp42B=Cyc_Tcutil_compress(_tmp437);void*_tmp42C=_tmp42B;struct Cyc_List_List*_tmp42D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42C)->tag == 5U){_LL9: _tmp42D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42C)->f1).attributes;_LLA:
# 1704
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp42D);
goto _LL8;}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1708
goto _LL3;}_LL3:;}{
# 1711
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp42E=Cyc_Cyclone_c_compiler;if(_tmp42E == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1716
s=({struct Cyc_PP_Doc*_tmp42F[5U];_tmp42F[0]=tmp_doc,({
# 1718
struct Cyc_PP_Doc*_tmp7D5=Cyc_Absynpp_scope2doc(_tmp43B);_tmp42F[1]=_tmp7D5;}),({
struct Cyc_PP_Doc*_tmp7D4=({struct Cyc_Absyn_Tqual _tmp7D3=_tmp438;void*_tmp7D2=_tmp437;Cyc_Absynpp_tqtd2doc(_tmp7D3,_tmp7D2,({struct Cyc_Core_Opt*_tmp431=_cycalloc(sizeof(*_tmp431));({struct Cyc_PP_Doc*_tmp7D1=({struct Cyc_PP_Doc*_tmp430[2U];_tmp430[0]=beforenamedoc,_tmp430[1]=sn;Cyc_PP_cat(_tag_dyneither(_tmp430,sizeof(struct Cyc_PP_Doc*),2U));});_tmp431->v=_tmp7D1;});_tmp431;}));});_tmp42F[2]=_tmp7D4;}),
_tmp436 == 0?({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_nil_doc();_tmp42F[3]=_tmp7D0;}):({
struct Cyc_PP_Doc*_tmp7CF=({struct Cyc_PP_Doc*_tmp432[2U];({struct Cyc_PP_Doc*_tmp7CE=Cyc_PP_text(({const char*_tmp433=" = ";_tag_dyneither(_tmp433,sizeof(char),4U);}));_tmp432[0]=_tmp7CE;}),({struct Cyc_PP_Doc*_tmp7CD=Cyc_Absynpp_exp2doc(_tmp436);_tmp432[1]=_tmp7CD;});Cyc_PP_cat(_tag_dyneither(_tmp432,sizeof(struct Cyc_PP_Doc*),2U));});_tmp42F[3]=_tmp7CF;}),({
struct Cyc_PP_Doc*_tmp7CC=Cyc_PP_text(({const char*_tmp434=";";_tag_dyneither(_tmp434,sizeof(char),2U);}));_tmp42F[4]=_tmp7CC;});Cyc_PP_cat(_tag_dyneither(_tmp42F,sizeof(struct Cyc_PP_Doc*),5U));});
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1727
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp43C=*x;struct _tuple17 _tmp43D=_tmp43C;struct _tuple0*_tmp43E;_LL1: _tmp43E=_tmp43D.f2;_LL2:;
return Cyc_Absynpp_qvar2doc(_tmp43E);}
# 1732
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp43F[4U];({struct Cyc_PP_Doc*_tmp7D9=Cyc_Absynpp_scope2doc(ad->sc);_tmp43F[0]=_tmp7D9;}),({
struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp43F[1]=_tmp7D8;}),({
struct Cyc_PP_Doc*_tmp7D7=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp43F[2]=_tmp7D7;}),({
struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp43F[3]=_tmp7D6;});Cyc_PP_cat(_tag_dyneither(_tmp43F,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1739
return({struct Cyc_PP_Doc*_tmp440[12U];({struct Cyc_PP_Doc*_tmp7EA=Cyc_Absynpp_scope2doc(ad->sc);_tmp440[0]=_tmp7EA;}),({
struct Cyc_PP_Doc*_tmp7E9=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp440[1]=_tmp7E9;}),({
struct Cyc_PP_Doc*_tmp7E8=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp440[2]=_tmp7E8;}),({
struct Cyc_PP_Doc*_tmp7E7=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp440[3]=_tmp7E7;}),({
struct Cyc_PP_Doc*_tmp7E6=Cyc_PP_blank_doc();_tmp440[4]=_tmp7E6;}),({struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_lb();_tmp440[5]=_tmp7E5;}),({
struct Cyc_PP_Doc*_tmp7E4=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp440[6]=_tmp7E4;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp7E3=Cyc_PP_nil_doc();_tmp440[7]=_tmp7E3;}):({
struct Cyc_PP_Doc*_tmp7E2=({struct Cyc_PP_Doc*_tmp441[2U];({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_text(({const char*_tmp442=":";_tag_dyneither(_tmp442,sizeof(char),2U);}));_tmp441[0]=_tmp7E1;}),({struct Cyc_PP_Doc*_tmp7E0=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp441[1]=_tmp7E0;});Cyc_PP_cat(_tag_dyneither(_tmp441,sizeof(struct Cyc_PP_Doc*),2U));});_tmp440[7]=_tmp7E2;}),({
struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp443[2U];({struct Cyc_PP_Doc*_tmp7DE=Cyc_PP_line_doc();_tmp443[0]=_tmp7DE;}),({struct Cyc_PP_Doc*_tmp7DD=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp443[1]=_tmp7DD;});Cyc_PP_cat(_tag_dyneither(_tmp443,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp440[8]=_tmp7DF;}),({
struct Cyc_PP_Doc*_tmp7DC=Cyc_PP_line_doc();_tmp440[9]=_tmp7DC;}),({
struct Cyc_PP_Doc*_tmp7DB=Cyc_Absynpp_rb();_tmp440[10]=_tmp7DB;}),({
struct Cyc_PP_Doc*_tmp7DA=Cyc_Absynpp_atts2doc(ad->attributes);_tmp440[11]=_tmp7DA;});Cyc_PP_cat(_tag_dyneither(_tmp440,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1753
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp444=dd;enum Cyc_Absyn_Scope _tmp450;struct _tuple0*_tmp44F;struct Cyc_List_List*_tmp44E;struct Cyc_Core_Opt*_tmp44D;int _tmp44C;_LL1: _tmp450=_tmp444->sc;_tmp44F=_tmp444->name;_tmp44E=_tmp444->tvs;_tmp44D=_tmp444->fields;_tmp44C=_tmp444->is_extensible;_LL2:;
if(_tmp44D == 0)
return({struct Cyc_PP_Doc*_tmp445[5U];({struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_scope2doc(_tmp450);_tmp445[0]=_tmp7F1;}),
_tmp44C?({struct Cyc_PP_Doc*_tmp7F0=Cyc_PP_text(({const char*_tmp446="@extensible ";_tag_dyneither(_tmp446,sizeof(char),13U);}));_tmp445[1]=_tmp7F0;}):({struct Cyc_PP_Doc*_tmp7EF=Cyc_PP_blank_doc();_tmp445[1]=_tmp7EF;}),({
struct Cyc_PP_Doc*_tmp7EE=Cyc_PP_text(({const char*_tmp447="datatype ";_tag_dyneither(_tmp447,sizeof(char),10U);}));_tmp445[2]=_tmp7EE;}),
_tmp44C?({struct Cyc_PP_Doc*_tmp7ED=Cyc_Absynpp_qvar2bolddoc(_tmp44F);_tmp445[3]=_tmp7ED;}):({struct Cyc_PP_Doc*_tmp7EC=Cyc_Absynpp_typedef_name2bolddoc(_tmp44F);_tmp445[3]=_tmp7EC;}),({
struct Cyc_PP_Doc*_tmp7EB=Cyc_Absynpp_ktvars2doc(_tmp44E);_tmp445[4]=_tmp7EB;});Cyc_PP_cat(_tag_dyneither(_tmp445,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1762
return({struct Cyc_PP_Doc*_tmp448[10U];({struct Cyc_PP_Doc*_tmp7FF=Cyc_Absynpp_scope2doc(_tmp450);_tmp448[0]=_tmp7FF;}),
_tmp44C?({struct Cyc_PP_Doc*_tmp7FE=Cyc_PP_text(({const char*_tmp449="@extensible ";_tag_dyneither(_tmp449,sizeof(char),13U);}));_tmp448[1]=_tmp7FE;}):({struct Cyc_PP_Doc*_tmp7FD=Cyc_PP_blank_doc();_tmp448[1]=_tmp7FD;}),({
struct Cyc_PP_Doc*_tmp7FC=Cyc_PP_text(({const char*_tmp44A="datatype ";_tag_dyneither(_tmp44A,sizeof(char),10U);}));_tmp448[2]=_tmp7FC;}),
_tmp44C?({struct Cyc_PP_Doc*_tmp7FB=Cyc_Absynpp_qvar2bolddoc(_tmp44F);_tmp448[3]=_tmp7FB;}):({struct Cyc_PP_Doc*_tmp7FA=Cyc_Absynpp_typedef_name2bolddoc(_tmp44F);_tmp448[3]=_tmp7FA;}),({
struct Cyc_PP_Doc*_tmp7F9=Cyc_Absynpp_ktvars2doc(_tmp44E);_tmp448[4]=_tmp7F9;}),({
struct Cyc_PP_Doc*_tmp7F8=Cyc_PP_blank_doc();_tmp448[5]=_tmp7F8;}),({struct Cyc_PP_Doc*_tmp7F7=Cyc_Absynpp_lb();_tmp448[6]=_tmp7F7;}),({
struct Cyc_PP_Doc*_tmp7F6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp44B[2U];({struct Cyc_PP_Doc*_tmp7F5=Cyc_PP_line_doc();_tmp44B[0]=_tmp7F5;}),({struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp44D->v);_tmp44B[1]=_tmp7F4;});Cyc_PP_cat(_tag_dyneither(_tmp44B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp448[7]=_tmp7F6;}),({
struct Cyc_PP_Doc*_tmp7F3=Cyc_PP_line_doc();_tmp448[8]=_tmp7F3;}),({
struct Cyc_PP_Doc*_tmp7F2=Cyc_Absynpp_rb();_tmp448[9]=_tmp7F2;});Cyc_PP_cat(_tag_dyneither(_tmp448,sizeof(struct Cyc_PP_Doc*),10U));});}}
# 1773
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp451=ed;enum Cyc_Absyn_Scope _tmp459;struct _tuple0*_tmp458;struct Cyc_Core_Opt*_tmp457;_LL1: _tmp459=_tmp451->sc;_tmp458=_tmp451->name;_tmp457=_tmp451->fields;_LL2:;
if(_tmp457 == 0)
return({struct Cyc_PP_Doc*_tmp452[3U];({struct Cyc_PP_Doc*_tmp802=Cyc_Absynpp_scope2doc(_tmp459);_tmp452[0]=_tmp802;}),({
struct Cyc_PP_Doc*_tmp801=Cyc_PP_text(({const char*_tmp453="enum ";_tag_dyneither(_tmp453,sizeof(char),6U);}));_tmp452[1]=_tmp801;}),({
struct Cyc_PP_Doc*_tmp800=Cyc_Absynpp_typedef_name2bolddoc(_tmp458);_tmp452[2]=_tmp800;});Cyc_PP_cat(_tag_dyneither(_tmp452,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1781
return({struct Cyc_PP_Doc*_tmp454[8U];({struct Cyc_PP_Doc*_tmp80C=Cyc_Absynpp_scope2doc(_tmp459);_tmp454[0]=_tmp80C;}),({
struct Cyc_PP_Doc*_tmp80B=Cyc_PP_text(({const char*_tmp455="enum ";_tag_dyneither(_tmp455,sizeof(char),6U);}));_tmp454[1]=_tmp80B;}),({
struct Cyc_PP_Doc*_tmp80A=Cyc_Absynpp_qvar2bolddoc(_tmp458);_tmp454[2]=_tmp80A;}),({
struct Cyc_PP_Doc*_tmp809=Cyc_PP_blank_doc();_tmp454[3]=_tmp809;}),({struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_lb();_tmp454[4]=_tmp808;}),({
struct Cyc_PP_Doc*_tmp807=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp456[2U];({struct Cyc_PP_Doc*_tmp806=Cyc_PP_line_doc();_tmp456[0]=_tmp806;}),({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp457->v);_tmp456[1]=_tmp805;});Cyc_PP_cat(_tag_dyneither(_tmp456,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp454[5]=_tmp807;}),({
struct Cyc_PP_Doc*_tmp804=Cyc_PP_line_doc();_tmp454[6]=_tmp804;}),({
struct Cyc_PP_Doc*_tmp803=Cyc_Absynpp_rb();_tmp454[7]=_tmp803;});Cyc_PP_cat(_tag_dyneither(_tmp454,sizeof(struct Cyc_PP_Doc*),8U));});}}
# 1790
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp45A=d->r;void*_tmp45B=_tmp45A;struct Cyc_List_List*_tmp4C6;struct Cyc_List_List*_tmp4C5;struct Cyc_List_List*_tmp4C4;struct Cyc_List_List*_tmp4C3;struct _tuple0*_tmp4C2;struct Cyc_List_List*_tmp4C1;struct _dyneither_ptr*_tmp4C0;struct Cyc_List_List*_tmp4BF;struct Cyc_Absyn_Typedefdecl*_tmp4BE;struct Cyc_Absyn_Enumdecl*_tmp4BD;struct Cyc_List_List*_tmp4BC;struct Cyc_Absyn_Pat*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BA;struct Cyc_Absyn_Datatypedecl*_tmp4B9;struct Cyc_Absyn_Tvar*_tmp4B8;struct Cyc_Absyn_Vardecl*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_Vardecl*_tmp4B5;struct Cyc_Absyn_Aggrdecl*_tmp4B4;struct Cyc_Absyn_Fndecl*_tmp4B3;switch(*((int*)_tmp45B)){case 1U: _LL1: _tmp4B3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LL2: {
# 1794
struct Cyc_Absyn_FnInfo type_info=_tmp4B3->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->tag=5U,_tmp46C->f1=type_info;_tmp46C;});
if(_tmp4B3->cached_type != 0){
void*_tmp45C=Cyc_Tcutil_compress((void*)_check_null(_tmp4B3->cached_type));void*_tmp45D=_tmp45C;struct Cyc_Absyn_FnInfo _tmp460;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45D)->tag == 5U){_LL24: _tmp460=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45D)->f1;_LL25:
# 1800
({struct Cyc_List_List*_tmp80D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((_tmp4B3->i).attributes,_tmp460.attributes);(_tmp4B3->i).attributes=_tmp80D;});goto _LL23;}else{_LL26: _LL27:
({void*_tmp45E=0U;({struct _dyneither_ptr _tmp80E=({const char*_tmp45F="function has non-function type";_tag_dyneither(_tmp45F,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp80E,_tag_dyneither(_tmp45E,sizeof(void*),0U));});});}_LL23:;}{
# 1803
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((_tmp4B3->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4B3->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp461=Cyc_Cyclone_c_compiler;if(_tmp461 == Cyc_Cyclone_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp462="inline ";_tag_dyneither(_tmp462,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp463="__inline ";_tag_dyneither(_tmp463,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1811
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4B3->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp464=Cyc_Cyclone_c_compiler;if(_tmp464 == Cyc_Cyclone_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((_tmp4B3->i).attributes);goto _LL2D;}_LL2D:;}{
# 1818
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4B3->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp811=Cyc_Absyn_empty_tqual(0U);void*_tmp810=t;Cyc_Absynpp_tqtd2doc(_tmp811,_tmp810,({struct Cyc_Core_Opt*_tmp46B=_cycalloc(sizeof(*_tmp46B));({
struct Cyc_PP_Doc*_tmp80F=({struct Cyc_PP_Doc*_tmp46A[2U];_tmp46A[0]=beforenamedoc,_tmp46A[1]=namedoc;Cyc_PP_cat(_tag_dyneither(_tmp46A,sizeof(struct Cyc_PP_Doc*),2U));});_tmp46B->v=_tmp80F;});_tmp46B;}));});
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp468[5U];({struct Cyc_PP_Doc*_tmp818=Cyc_PP_blank_doc();_tmp468[0]=_tmp818;}),({struct Cyc_PP_Doc*_tmp817=Cyc_Absynpp_lb();_tmp468[1]=_tmp817;}),({
struct Cyc_PP_Doc*_tmp816=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp469[2U];({struct Cyc_PP_Doc*_tmp815=Cyc_PP_line_doc();_tmp469[0]=_tmp815;}),({struct Cyc_PP_Doc*_tmp814=Cyc_Absynpp_stmt2doc(_tmp4B3->body,0);_tmp469[1]=_tmp814;});Cyc_PP_cat(_tag_dyneither(_tmp469,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp468[2]=_tmp816;}),({
struct Cyc_PP_Doc*_tmp813=Cyc_PP_line_doc();_tmp468[3]=_tmp813;}),({
struct Cyc_PP_Doc*_tmp812=Cyc_Absynpp_rb();_tmp468[4]=_tmp812;});Cyc_PP_cat(_tag_dyneither(_tmp468,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp465[4U];_tmp465[0]=inlinedoc,_tmp465[1]=scopedoc,_tmp465[2]=tqtddoc,_tmp465[3]=bodydoc;Cyc_PP_cat(_tag_dyneither(_tmp465,sizeof(struct Cyc_PP_Doc*),4U));});
# 1827
{enum Cyc_Cyclone_C_Compilers _tmp466=Cyc_Cyclone_c_compiler;if(_tmp466 == Cyc_Cyclone_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp467[2U];_tmp467[0]=attsdoc,_tmp467[1]=s;Cyc_PP_cat(_tag_dyneither(_tmp467,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1832
goto _LL0;};};};};}case 5U: _LL3: _tmp4B4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LL4:
# 1835
 s=({struct Cyc_PP_Doc*_tmp46D[2U];({struct Cyc_PP_Doc*_tmp81A=Cyc_Absynpp_aggrdecl2doc(_tmp4B4);_tmp46D[0]=_tmp81A;}),({struct Cyc_PP_Doc*_tmp819=Cyc_PP_text(({const char*_tmp46E=";";_tag_dyneither(_tmp46E,sizeof(char),2U);}));_tmp46D[1]=_tmp819;});Cyc_PP_cat(_tag_dyneither(_tmp46D,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 0U: _LL5: _tmp4B5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LL6:
# 1838
 s=Cyc_Absynpp_vardecl2doc(_tmp4B5);
goto _LL0;case 4U: _LL7: _tmp4B8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_tmp4B7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp45B)->f2;_tmp4B6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp45B)->f3;_LL8:
# 1841
 s=({struct Cyc_PP_Doc*_tmp46F[7U];({struct Cyc_PP_Doc*_tmp825=Cyc_PP_text(({const char*_tmp470="region";_tag_dyneither(_tmp470,sizeof(char),7U);}));_tmp46F[0]=_tmp825;}),({
struct Cyc_PP_Doc*_tmp824=Cyc_PP_text(({const char*_tmp471="<";_tag_dyneither(_tmp471,sizeof(char),2U);}));_tmp46F[1]=_tmp824;}),({
struct Cyc_PP_Doc*_tmp823=Cyc_Absynpp_tvar2doc(_tmp4B8);_tmp46F[2]=_tmp823;}),({
struct Cyc_PP_Doc*_tmp822=Cyc_PP_text(({const char*_tmp472=">";_tag_dyneither(_tmp472,sizeof(char),2U);}));_tmp46F[3]=_tmp822;}),({
struct Cyc_PP_Doc*_tmp821=Cyc_Absynpp_qvar2doc(_tmp4B7->name);_tmp46F[4]=_tmp821;}),
(unsigned int)_tmp4B6?({struct Cyc_PP_Doc*_tmp820=({struct Cyc_PP_Doc*_tmp473[3U];({struct Cyc_PP_Doc*_tmp81F=Cyc_PP_text(({const char*_tmp474=" = open(";_tag_dyneither(_tmp474,sizeof(char),9U);}));_tmp473[0]=_tmp81F;}),({struct Cyc_PP_Doc*_tmp81E=Cyc_Absynpp_exp2doc(_tmp4B6);_tmp473[1]=_tmp81E;}),({
struct Cyc_PP_Doc*_tmp81D=Cyc_PP_text(({const char*_tmp475=")";_tag_dyneither(_tmp475,sizeof(char),2U);}));_tmp473[2]=_tmp81D;});Cyc_PP_cat(_tag_dyneither(_tmp473,sizeof(struct Cyc_PP_Doc*),3U));});
# 1846
_tmp46F[5]=_tmp820;}):({
struct Cyc_PP_Doc*_tmp81C=Cyc_PP_nil_doc();_tmp46F[5]=_tmp81C;}),({
struct Cyc_PP_Doc*_tmp81B=Cyc_PP_text(({const char*_tmp476=";";_tag_dyneither(_tmp476,sizeof(char),2U);}));_tmp46F[6]=_tmp81B;});Cyc_PP_cat(_tag_dyneither(_tmp46F,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 6U: _LL9: _tmp4B9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LLA:
# 1851
 s=({struct Cyc_PP_Doc*_tmp477[2U];({struct Cyc_PP_Doc*_tmp827=Cyc_Absynpp_datatypedecl2doc(_tmp4B9);_tmp477[0]=_tmp827;}),({struct Cyc_PP_Doc*_tmp826=Cyc_PP_text(({const char*_tmp478=";";_tag_dyneither(_tmp478,sizeof(char),2U);}));_tmp477[1]=_tmp826;});Cyc_PP_cat(_tag_dyneither(_tmp477,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 2U: _LLB: _tmp4BB=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_tmp4BA=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp45B)->f3;_LLC:
# 1854
 s=({struct Cyc_PP_Doc*_tmp479[5U];({struct Cyc_PP_Doc*_tmp82C=Cyc_PP_text(({const char*_tmp47A="let ";_tag_dyneither(_tmp47A,sizeof(char),5U);}));_tmp479[0]=_tmp82C;}),({
struct Cyc_PP_Doc*_tmp82B=Cyc_Absynpp_pat2doc(_tmp4BB);_tmp479[1]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=Cyc_PP_text(({const char*_tmp47B=" = ";_tag_dyneither(_tmp47B,sizeof(char),4U);}));_tmp479[2]=_tmp82A;}),({
struct Cyc_PP_Doc*_tmp829=Cyc_Absynpp_exp2doc(_tmp4BA);_tmp479[3]=_tmp829;}),({
struct Cyc_PP_Doc*_tmp828=Cyc_PP_text(({const char*_tmp47C=";";_tag_dyneither(_tmp47C,sizeof(char),2U);}));_tmp479[4]=_tmp828;});Cyc_PP_cat(_tag_dyneither(_tmp479,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 3U: _LLD: _tmp4BC=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LLE:
# 1861
 s=({struct Cyc_PP_Doc*_tmp47D[3U];({struct Cyc_PP_Doc*_tmp82F=Cyc_PP_text(({const char*_tmp47E="let ";_tag_dyneither(_tmp47E,sizeof(char),5U);}));_tmp47D[0]=_tmp82F;}),({struct Cyc_PP_Doc*_tmp82E=Cyc_Absynpp_ids2doc(_tmp4BC);_tmp47D[1]=_tmp82E;}),({struct Cyc_PP_Doc*_tmp82D=Cyc_PP_text(({const char*_tmp47F=";";_tag_dyneither(_tmp47F,sizeof(char),2U);}));_tmp47D[2]=_tmp82D;});Cyc_PP_cat(_tag_dyneither(_tmp47D,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 7U: _LLF: _tmp4BD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LL10:
# 1864
 s=({struct Cyc_PP_Doc*_tmp480[2U];({struct Cyc_PP_Doc*_tmp831=Cyc_Absynpp_enumdecl2doc(_tmp4BD);_tmp480[0]=_tmp831;}),({struct Cyc_PP_Doc*_tmp830=Cyc_PP_text(({const char*_tmp481=";";_tag_dyneither(_tmp481,sizeof(char),2U);}));_tmp480[1]=_tmp830;});Cyc_PP_cat(_tag_dyneither(_tmp480,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 8U: _LL11: _tmp4BE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LL12: {
# 1867
void*t;
if(_tmp4BE->defn != 0)
t=(void*)_check_null(_tmp4BE->defn);else{
# 1871
t=Cyc_Absyn_new_evar(_tmp4BE->kind,0);}
s=({struct Cyc_PP_Doc*_tmp482[4U];({struct Cyc_PP_Doc*_tmp83A=Cyc_PP_text(({const char*_tmp483="typedef ";_tag_dyneither(_tmp483,sizeof(char),9U);}));_tmp482[0]=_tmp83A;}),({
struct Cyc_PP_Doc*_tmp839=({struct Cyc_Absyn_Tqual _tmp838=_tmp4BE->tq;void*_tmp837=t;Cyc_Absynpp_tqtd2doc(_tmp838,_tmp837,({struct Cyc_Core_Opt*_tmp485=_cycalloc(sizeof(*_tmp485));({
# 1875
struct Cyc_PP_Doc*_tmp836=({struct Cyc_PP_Doc*_tmp484[2U];({struct Cyc_PP_Doc*_tmp835=Cyc_Absynpp_typedef_name2bolddoc(_tmp4BE->name);_tmp484[0]=_tmp835;}),({
struct Cyc_PP_Doc*_tmp834=Cyc_Absynpp_tvars2doc(_tmp4BE->tvs);_tmp484[1]=_tmp834;});Cyc_PP_cat(_tag_dyneither(_tmp484,sizeof(struct Cyc_PP_Doc*),2U));});
# 1875
_tmp485->v=_tmp836;});_tmp485;}));});
# 1873
_tmp482[1]=_tmp839;}),({
# 1878
struct Cyc_PP_Doc*_tmp833=Cyc_Absynpp_atts2doc(_tmp4BE->atts);_tmp482[2]=_tmp833;}),({
struct Cyc_PP_Doc*_tmp832=Cyc_PP_text(({const char*_tmp486=";";_tag_dyneither(_tmp486,sizeof(char),2U);}));_tmp482[3]=_tmp832;});Cyc_PP_cat(_tag_dyneither(_tmp482,sizeof(struct Cyc_PP_Doc*),4U));});
# 1881
goto _LL0;}case 9U: _LL13: _tmp4C0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_tmp4BF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp45B)->f2;_LL14:
# 1883
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4C0);
s=({struct Cyc_PP_Doc*_tmp487[8U];({struct Cyc_PP_Doc*_tmp843=Cyc_PP_text(({const char*_tmp488="namespace ";_tag_dyneither(_tmp488,sizeof(char),11U);}));_tmp487[0]=_tmp843;}),({
struct Cyc_PP_Doc*_tmp842=Cyc_PP_textptr(_tmp4C0);_tmp487[1]=_tmp842;}),({
struct Cyc_PP_Doc*_tmp841=Cyc_PP_blank_doc();_tmp487[2]=_tmp841;}),({struct Cyc_PP_Doc*_tmp840=Cyc_Absynpp_lb();_tmp487[3]=_tmp840;}),({
struct Cyc_PP_Doc*_tmp83F=Cyc_PP_line_doc();_tmp487[4]=_tmp83F;}),({
struct Cyc_PP_Doc*_tmp83E=({struct _dyneither_ptr _tmp83D=({const char*_tmp489="";_tag_dyneither(_tmp489,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp83D,_tmp4BF);});_tmp487[5]=_tmp83E;}),({
struct Cyc_PP_Doc*_tmp83C=Cyc_PP_line_doc();_tmp487[6]=_tmp83C;}),({
struct Cyc_PP_Doc*_tmp83B=Cyc_Absynpp_rb();_tmp487[7]=_tmp83B;});Cyc_PP_cat(_tag_dyneither(_tmp487,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;case 10U: _LL15: _tmp4C2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_tmp4C1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp45B)->f2;_LL16:
# 1894
 if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp48A[8U];({struct Cyc_PP_Doc*_tmp84C=Cyc_PP_text(({const char*_tmp48B="using ";_tag_dyneither(_tmp48B,sizeof(char),7U);}));_tmp48A[0]=_tmp84C;}),({
struct Cyc_PP_Doc*_tmp84B=Cyc_Absynpp_qvar2doc(_tmp4C2);_tmp48A[1]=_tmp84B;}),({
struct Cyc_PP_Doc*_tmp84A=Cyc_PP_blank_doc();_tmp48A[2]=_tmp84A;}),({struct Cyc_PP_Doc*_tmp849=Cyc_Absynpp_lb();_tmp48A[3]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_PP_line_doc();_tmp48A[4]=_tmp848;}),({
struct Cyc_PP_Doc*_tmp847=({struct _dyneither_ptr _tmp846=({const char*_tmp48C="";_tag_dyneither(_tmp48C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp846,_tmp4C1);});_tmp48A[5]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp845=Cyc_PP_line_doc();_tmp48A[6]=_tmp845;}),({
struct Cyc_PP_Doc*_tmp844=Cyc_Absynpp_rb();_tmp48A[7]=_tmp844;});Cyc_PP_cat(_tag_dyneither(_tmp48A,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1903
s=({struct Cyc_PP_Doc*_tmp48D[11U];({struct Cyc_PP_Doc*_tmp858=Cyc_PP_text(({const char*_tmp48E="/* using ";_tag_dyneither(_tmp48E,sizeof(char),10U);}));_tmp48D[0]=_tmp858;}),({
struct Cyc_PP_Doc*_tmp857=Cyc_Absynpp_qvar2doc(_tmp4C2);_tmp48D[1]=_tmp857;}),({
struct Cyc_PP_Doc*_tmp856=Cyc_PP_blank_doc();_tmp48D[2]=_tmp856;}),({
struct Cyc_PP_Doc*_tmp855=Cyc_Absynpp_lb();_tmp48D[3]=_tmp855;}),({
struct Cyc_PP_Doc*_tmp854=Cyc_PP_text(({const char*_tmp48F=" */";_tag_dyneither(_tmp48F,sizeof(char),4U);}));_tmp48D[4]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp853=Cyc_PP_line_doc();_tmp48D[5]=_tmp853;}),({
struct Cyc_PP_Doc*_tmp852=({struct _dyneither_ptr _tmp851=({const char*_tmp490="";_tag_dyneither(_tmp490,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp851,_tmp4C1);});_tmp48D[6]=_tmp852;}),({
struct Cyc_PP_Doc*_tmp850=Cyc_PP_line_doc();_tmp48D[7]=_tmp850;}),({
struct Cyc_PP_Doc*_tmp84F=Cyc_PP_text(({const char*_tmp491="/* ";_tag_dyneither(_tmp491,sizeof(char),4U);}));_tmp48D[8]=_tmp84F;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_Absynpp_rb();_tmp48D[9]=_tmp84E;}),({
struct Cyc_PP_Doc*_tmp84D=Cyc_PP_text(({const char*_tmp492=" */";_tag_dyneither(_tmp492,sizeof(char),4U);}));_tmp48D[10]=_tmp84D;});Cyc_PP_cat(_tag_dyneither(_tmp48D,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;case 11U: _LL17: _tmp4C3=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_LL18:
# 1916
 if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp493[6U];({struct Cyc_PP_Doc*_tmp85F=Cyc_PP_text(({const char*_tmp494="extern \"C\" ";_tag_dyneither(_tmp494,sizeof(char),12U);}));_tmp493[0]=_tmp85F;}),({
struct Cyc_PP_Doc*_tmp85E=Cyc_Absynpp_lb();_tmp493[1]=_tmp85E;}),({
struct Cyc_PP_Doc*_tmp85D=Cyc_PP_line_doc();_tmp493[2]=_tmp85D;}),({
struct Cyc_PP_Doc*_tmp85C=({struct _dyneither_ptr _tmp85B=({const char*_tmp495="";_tag_dyneither(_tmp495,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp85B,_tmp4C3);});_tmp493[3]=_tmp85C;}),({
struct Cyc_PP_Doc*_tmp85A=Cyc_PP_line_doc();_tmp493[4]=_tmp85A;}),({
struct Cyc_PP_Doc*_tmp859=Cyc_Absynpp_rb();_tmp493[5]=_tmp859;});Cyc_PP_cat(_tag_dyneither(_tmp493,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1924
s=({struct Cyc_PP_Doc*_tmp496[9U];({struct Cyc_PP_Doc*_tmp869=Cyc_PP_text(({const char*_tmp497="/* extern \"C\" ";_tag_dyneither(_tmp497,sizeof(char),15U);}));_tmp496[0]=_tmp869;}),({
struct Cyc_PP_Doc*_tmp868=Cyc_Absynpp_lb();_tmp496[1]=_tmp868;}),({
struct Cyc_PP_Doc*_tmp867=Cyc_PP_text(({const char*_tmp498=" */";_tag_dyneither(_tmp498,sizeof(char),4U);}));_tmp496[2]=_tmp867;}),({
struct Cyc_PP_Doc*_tmp866=Cyc_PP_line_doc();_tmp496[3]=_tmp866;}),({
struct Cyc_PP_Doc*_tmp865=({struct _dyneither_ptr _tmp864=({const char*_tmp499="";_tag_dyneither(_tmp499,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp864,_tmp4C3);});_tmp496[4]=_tmp865;}),({
struct Cyc_PP_Doc*_tmp863=Cyc_PP_line_doc();_tmp496[5]=_tmp863;}),({
struct Cyc_PP_Doc*_tmp862=Cyc_PP_text(({const char*_tmp49A="/* ";_tag_dyneither(_tmp49A,sizeof(char),4U);}));_tmp496[6]=_tmp862;}),({
struct Cyc_PP_Doc*_tmp861=Cyc_Absynpp_rb();_tmp496[7]=_tmp861;}),({
struct Cyc_PP_Doc*_tmp860=Cyc_PP_text(({const char*_tmp49B=" */";_tag_dyneither(_tmp49B,sizeof(char),4U);}));_tmp496[8]=_tmp860;});Cyc_PP_cat(_tag_dyneither(_tmp496,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 12U: _LL19: _tmp4C6=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp45B)->f1;_tmp4C5=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp45B)->f2;_tmp4C4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp45B)->f3;_LL1A:
# 1935
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(_tmp4C4 != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp49C[7U];({struct Cyc_PP_Doc*_tmp871=Cyc_Absynpp_rb();_tmp49C[0]=_tmp871;}),({struct Cyc_PP_Doc*_tmp870=Cyc_PP_text(({const char*_tmp49D=" export ";_tag_dyneither(_tmp49D,sizeof(char),9U);}));_tmp49C[1]=_tmp870;}),({struct Cyc_PP_Doc*_tmp86F=Cyc_Absynpp_lb();_tmp49C[2]=_tmp86F;}),({
struct Cyc_PP_Doc*_tmp86E=Cyc_PP_line_doc();_tmp49C[3]=_tmp86E;}),({struct Cyc_PP_Doc*_tmp86D=({struct _dyneither_ptr _tmp86C=({const char*_tmp49E=",";_tag_dyneither(_tmp49E,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp86C,_tmp4C4);});_tmp49C[4]=_tmp86D;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_PP_line_doc();_tmp49C[5]=_tmp86B;}),({struct Cyc_PP_Doc*_tmp86A=Cyc_Absynpp_rb();_tmp49C[6]=_tmp86A;});Cyc_PP_cat(_tag_dyneither(_tmp49C,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1943
exs_doc=Cyc_Absynpp_rb();}
if(_tmp4C5 != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp49F[7U];({struct Cyc_PP_Doc*_tmp879=Cyc_Absynpp_rb();_tmp49F[0]=_tmp879;}),({struct Cyc_PP_Doc*_tmp878=Cyc_PP_text(({const char*_tmp4A0=" cycdef ";_tag_dyneither(_tmp4A0,sizeof(char),9U);}));_tmp49F[1]=_tmp878;}),({struct Cyc_PP_Doc*_tmp877=Cyc_Absynpp_lb();_tmp49F[2]=_tmp877;}),({
struct Cyc_PP_Doc*_tmp876=Cyc_PP_line_doc();_tmp49F[3]=_tmp876;}),({struct Cyc_PP_Doc*_tmp875=({struct _dyneither_ptr _tmp874=({const char*_tmp4A1="";_tag_dyneither(_tmp4A1,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp874,_tmp4C5);});_tmp49F[4]=_tmp875;}),({
struct Cyc_PP_Doc*_tmp873=Cyc_PP_line_doc();_tmp49F[5]=_tmp873;}),({struct Cyc_PP_Doc*_tmp872=Cyc_Absynpp_rb();_tmp49F[6]=_tmp872;});Cyc_PP_cat(_tag_dyneither(_tmp49F,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1949
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp4A2[6U];({struct Cyc_PP_Doc*_tmp87F=Cyc_PP_text(({const char*_tmp4A3="extern \"C include\" ";_tag_dyneither(_tmp4A3,sizeof(char),20U);}));_tmp4A2[0]=_tmp87F;}),({
struct Cyc_PP_Doc*_tmp87E=Cyc_Absynpp_lb();_tmp4A2[1]=_tmp87E;}),({
struct Cyc_PP_Doc*_tmp87D=Cyc_PP_line_doc();_tmp4A2[2]=_tmp87D;}),({
struct Cyc_PP_Doc*_tmp87C=({struct _dyneither_ptr _tmp87B=({const char*_tmp4A4="";_tag_dyneither(_tmp4A4,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp87B,_tmp4C6);});_tmp4A2[3]=_tmp87C;}),({
struct Cyc_PP_Doc*_tmp87A=Cyc_PP_line_doc();_tmp4A2[4]=_tmp87A;}),_tmp4A2[5]=exs_doc;Cyc_PP_cat(_tag_dyneither(_tmp4A2,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1957
s=({struct Cyc_PP_Doc*_tmp4A5[9U];({struct Cyc_PP_Doc*_tmp889=Cyc_PP_text(({const char*_tmp4A6="/* extern \"C include\" ";_tag_dyneither(_tmp4A6,sizeof(char),23U);}));_tmp4A5[0]=_tmp889;}),({
struct Cyc_PP_Doc*_tmp888=Cyc_Absynpp_lb();_tmp4A5[1]=_tmp888;}),({
struct Cyc_PP_Doc*_tmp887=Cyc_PP_text(({const char*_tmp4A7=" */";_tag_dyneither(_tmp4A7,sizeof(char),4U);}));_tmp4A5[2]=_tmp887;}),({
struct Cyc_PP_Doc*_tmp886=Cyc_PP_line_doc();_tmp4A5[3]=_tmp886;}),({
struct Cyc_PP_Doc*_tmp885=({struct _dyneither_ptr _tmp884=({const char*_tmp4A8="";_tag_dyneither(_tmp4A8,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp884,_tmp4C6);});_tmp4A5[4]=_tmp885;}),({
struct Cyc_PP_Doc*_tmp883=Cyc_PP_line_doc();_tmp4A5[5]=_tmp883;}),({
struct Cyc_PP_Doc*_tmp882=Cyc_PP_text(({const char*_tmp4A9="/* ";_tag_dyneither(_tmp4A9,sizeof(char),4U);}));_tmp4A5[6]=_tmp882;}),({
struct Cyc_PP_Doc*_tmp881=Cyc_Absynpp_rb();_tmp4A5[7]=_tmp881;}),({
struct Cyc_PP_Doc*_tmp880=Cyc_PP_text(({const char*_tmp4AA=" */";_tag_dyneither(_tmp4AA,sizeof(char),4U);}));_tmp4A5[8]=_tmp880;});Cyc_PP_cat(_tag_dyneither(_tmp4A5,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 13U: _LL1B: _LL1C:
# 1968
 s=({struct Cyc_PP_Doc*_tmp4AB[2U];({struct Cyc_PP_Doc*_tmp88B=Cyc_PP_text(({const char*_tmp4AC="__cyclone_port_on__;";_tag_dyneither(_tmp4AC,sizeof(char),21U);}));_tmp4AB[0]=_tmp88B;}),({struct Cyc_PP_Doc*_tmp88A=Cyc_Absynpp_lb();_tmp4AB[1]=_tmp88A;});Cyc_PP_cat(_tag_dyneither(_tmp4AB,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1971
 s=({struct Cyc_PP_Doc*_tmp4AD[2U];({struct Cyc_PP_Doc*_tmp88D=Cyc_PP_text(({const char*_tmp4AE="__cyclone_port_off__;";_tag_dyneither(_tmp4AE,sizeof(char),22U);}));_tmp4AD[0]=_tmp88D;}),({struct Cyc_PP_Doc*_tmp88C=Cyc_Absynpp_lb();_tmp4AD[1]=_tmp88C;});Cyc_PP_cat(_tag_dyneither(_tmp4AD,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1974
 s=({struct Cyc_PP_Doc*_tmp4AF[2U];({struct Cyc_PP_Doc*_tmp88F=Cyc_PP_text(({const char*_tmp4B0="__tempest_on__;";_tag_dyneither(_tmp4B0,sizeof(char),16U);}));_tmp4AF[0]=_tmp88F;}),({struct Cyc_PP_Doc*_tmp88E=Cyc_Absynpp_lb();_tmp4AF[1]=_tmp88E;});Cyc_PP_cat(_tag_dyneither(_tmp4AF,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1977
 s=({struct Cyc_PP_Doc*_tmp4B1[2U];({struct Cyc_PP_Doc*_tmp891=Cyc_PP_text(({const char*_tmp4B2="__tempest_off__;";_tag_dyneither(_tmp4B2,sizeof(char),17U);}));_tmp4B1[0]=_tmp891;}),({struct Cyc_PP_Doc*_tmp890=Cyc_Absynpp_lb();_tmp4B1[1]=_tmp890;});Cyc_PP_cat(_tag_dyneither(_tmp4B1,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 1981
return s;}
# 1984
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4C7=sc;switch(_tmp4C7){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4C8="static ";_tag_dyneither(_tmp4C8,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4C9="extern ";_tag_dyneither(_tmp4C9,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4CA="extern \"C\" ";_tag_dyneither(_tmp4CA,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4CB="abstract ";_tag_dyneither(_tmp4CB,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4CC="register ";_tag_dyneither(_tmp4CC,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;};}
# 1998
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4CD=Cyc_Tcutil_compress(t);void*_tmp4CE=_tmp4CD;struct Cyc_List_List*_tmp4D0;struct Cyc_Absyn_Tvar*_tmp4CF;switch(*((int*)_tmp4CE)){case 2U: _LL1: _tmp4CF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4CE)->f1;_LL2:
 return Cyc_Tcutil_is_temp_tvar(_tmp4CF);case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CE)->f1)->tag == 9U){_LL3: _tmp4D0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CE)->f2;_LL4:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4D0);}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2010
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4D1=t;struct Cyc_Absyn_Typedefdecl*_tmp4D3;void*_tmp4D2;switch(*((int*)_tmp4D1)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D1)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp4D3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D1)->f3;_tmp4D2=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D1)->f4;if(_tmp4D2 != 0){_LL6:
# 2017
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4D2);}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2024
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2027
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4D4=({struct _tuple16 _tmp556;_tmp556.f1=(void*)tms->hd,_tmp556.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp556;});struct _tuple16 _tmp4D5=_tmp4D4;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4D5.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4D5.f2)->tag == 3U){_LL1: _LL2:
# 2030
 return({struct Cyc_List_List*_tmp4D7=_region_malloc(r,sizeof(*_tmp4D7));_tmp4D7->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp893=({struct Cyc_List_List*_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6));_tmp4D6->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp892=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4D6->tl=_tmp892;});_tmp4D6;});_tmp4D7->tl=_tmp893;});_tmp4D7;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8));_tmp4D8->hd=atts,_tmp4D8->tl=tms;_tmp4D8;});}_LL0:;}else{
# 2033
return({struct Cyc_List_List*_tmp4D9=_region_malloc(r,sizeof(*_tmp4D9));_tmp4D9->hd=atts,_tmp4D9->tl=tms;_tmp4D9;});}}
# 2038
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2040
void*_tmp4DA=t;struct _tuple0*_tmp517;struct Cyc_List_List*_tmp516;struct Cyc_Absyn_Typedefdecl*_tmp515;void*_tmp514;struct Cyc_Core_Opt*_tmp513;void*_tmp512;int _tmp511;struct Cyc_List_List*_tmp510;void*_tmp50F;struct Cyc_Absyn_Tqual _tmp50E;void*_tmp50D;struct Cyc_List_List*_tmp50C;int _tmp50B;struct Cyc_Absyn_VarargInfo*_tmp50A;struct Cyc_List_List*_tmp509;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp506;void*_tmp505;struct Cyc_Absyn_Tqual _tmp504;struct Cyc_Absyn_PtrAtts _tmp503;void*_tmp502;struct Cyc_Absyn_Tqual _tmp501;struct Cyc_Absyn_Exp*_tmp500;void*_tmp4FF;unsigned int _tmp4FE;switch(*((int*)_tmp4DA)){case 4U: _LL1: _tmp502=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DA)->f1).elt_type;_tmp501=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DA)->f1).tq;_tmp500=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DA)->f1).num_elts;_tmp4FF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DA)->f1).zero_term;_tmp4FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DA)->f1).zt_loc;_LL2: {
# 2043
struct _tuple12 _tmp4DB=Cyc_Absynpp_to_tms(r,_tmp501,_tmp502);struct _tuple12 _tmp4DC=_tmp4DB;struct Cyc_Absyn_Tqual _tmp4E2;void*_tmp4E1;struct Cyc_List_List*_tmp4E0;_LLE: _tmp4E2=_tmp4DC.f1;_tmp4E1=_tmp4DC.f2;_tmp4E0=_tmp4DC.f3;_LLF:;{
void*tm;
if(_tmp500 == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4DD=_region_malloc(r,sizeof(*_tmp4DD));_tmp4DD->tag=0U,_tmp4DD->f1=_tmp4FF,_tmp4DD->f2=_tmp4FE;_tmp4DD;});else{
# 2048
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE));_tmp4DE->tag=1U,_tmp4DE->f1=_tmp500,_tmp4DE->f2=_tmp4FF,_tmp4DE->f3=_tmp4FE;_tmp4DE;});}
return({struct _tuple12 _tmp557;_tmp557.f1=_tmp4E2,_tmp557.f2=_tmp4E1,({struct Cyc_List_List*_tmp894=({struct Cyc_List_List*_tmp4DF=_region_malloc(r,sizeof(*_tmp4DF));_tmp4DF->hd=tm,_tmp4DF->tl=_tmp4E0;_tmp4DF;});_tmp557.f3=_tmp894;});_tmp557;});};}case 3U: _LL3: _tmp505=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DA)->f1).elt_type;_tmp504=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DA)->f1).elt_tq;_tmp503=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DA)->f1).ptr_atts;_LL4: {
# 2052
struct _tuple12 _tmp4E3=Cyc_Absynpp_to_tms(r,_tmp504,_tmp505);struct _tuple12 _tmp4E4=_tmp4E3;struct Cyc_Absyn_Tqual _tmp4E9;void*_tmp4E8;struct Cyc_List_List*_tmp4E7;_LL11: _tmp4E9=_tmp4E4.f1;_tmp4E8=_tmp4E4.f2;_tmp4E7=_tmp4E4.f3;_LL12:;
_tmp4E7=({struct Cyc_List_List*_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6));({void*_tmp895=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5));_tmp4E5->tag=2U,_tmp4E5->f1=_tmp503,_tmp4E5->f2=tq;_tmp4E5;});_tmp4E6->hd=_tmp895;}),_tmp4E6->tl=_tmp4E7;_tmp4E6;});
return({struct _tuple12 _tmp558;_tmp558.f1=_tmp4E9,_tmp558.f2=_tmp4E8,_tmp558.f3=_tmp4E7;_tmp558;});}case 5U: _LL5: _tmp510=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).tvars;_tmp50F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).effect;_tmp50E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).ret_tqual;_tmp50D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).ret_type;_tmp50C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).args;_tmp50B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).c_varargs;_tmp50A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).cyc_varargs;_tmp509=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).rgn_po;_tmp508=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).attributes;_tmp507=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).requires_clause;_tmp506=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DA)->f1).ensures_clause;_LL6:
# 2058
 if(!Cyc_Absynpp_print_all_tvars){
# 2062
if(_tmp50F == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp50F)){
_tmp50F=0;
_tmp510=0;}}else{
# 2067
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2070
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp510);}{
# 2075
struct _tuple12 _tmp4EA=Cyc_Absynpp_to_tms(r,_tmp50E,_tmp50D);struct _tuple12 _tmp4EB=_tmp4EA;struct Cyc_Absyn_Tqual _tmp4FD;void*_tmp4FC;struct Cyc_List_List*_tmp4FB;_LL14: _tmp4FD=_tmp4EB.f1;_tmp4FC=_tmp4EB.f2;_tmp4FB=_tmp4EB.f3;_LL15:;{
struct Cyc_List_List*tms=_tmp4FB;
# 2086 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4EC=Cyc_Cyclone_c_compiler;if(_tmp4EC == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2088
 if(_tmp508 != 0)
# 2091
tms=({struct _RegionHandle*_tmp897=r;void*_tmp896=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED));_tmp4ED->tag=5U,_tmp4ED->f1=0U,_tmp4ED->f2=_tmp508;_tmp4ED;});Cyc_Absynpp_bubble_attributes(_tmp897,_tmp896,tms);});
# 2093
tms=({struct Cyc_List_List*_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0));({void*_tmp899=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF));
_tmp4EF->tag=3U,({void*_tmp898=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4EE=_region_malloc(r,sizeof(*_tmp4EE));_tmp4EE->tag=1U,_tmp4EE->f1=_tmp50C,_tmp4EE->f2=_tmp50B,_tmp4EE->f3=_tmp50A,_tmp4EE->f4=_tmp50F,_tmp4EE->f5=_tmp509,_tmp4EE->f6=_tmp507,_tmp4EE->f7=_tmp506;_tmp4EE;});_tmp4EF->f1=_tmp898;});_tmp4EF;});
# 2093
_tmp4F0->hd=_tmp899;}),_tmp4F0->tl=tms;_tmp4F0;});
# 2097
goto _LL16;}else{_LL19: _LL1A:
# 2099
 tms=({struct Cyc_List_List*_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3));({void*_tmp89B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2));
_tmp4F2->tag=3U,({void*_tmp89A=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4F1=_region_malloc(r,sizeof(*_tmp4F1));_tmp4F1->tag=1U,_tmp4F1->f1=_tmp50C,_tmp4F1->f2=_tmp50B,_tmp4F1->f3=_tmp50A,_tmp4F1->f4=_tmp50F,_tmp4F1->f5=_tmp509,_tmp4F1->f6=_tmp507,_tmp4F1->f7=_tmp506;_tmp4F1;});_tmp4F2->f1=_tmp89A;});_tmp4F2;});
# 2099
_tmp4F3->hd=_tmp89B;}),_tmp4F3->tl=tms;_tmp4F3;});
# 2103
for(0;_tmp508 != 0;_tmp508=_tmp508->tl){
void*_tmp4F4=(void*)_tmp508->hd;void*_tmp4F5=_tmp4F4;switch(*((int*)_tmp4F5)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2108
 tms=({struct Cyc_List_List*_tmp4F8=_region_malloc(r,sizeof(*_tmp4F8));({void*_tmp89D=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F7=_region_malloc(r,sizeof(*_tmp4F7));_tmp4F7->tag=5U,_tmp4F7->f1=0U,({struct Cyc_List_List*_tmp89C=({struct Cyc_List_List*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->hd=(void*)_tmp508->hd,_tmp4F6->tl=0;_tmp4F6;});_tmp4F7->f2=_tmp89C;});_tmp4F7;});_tmp4F8->hd=_tmp89D;}),_tmp4F8->tl=tms;_tmp4F8;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2112
goto _LL16;}_LL16:;}
# 2115
AfterAtts:
 if(_tmp510 != 0)
tms=({struct Cyc_List_List*_tmp4FA=_region_malloc(r,sizeof(*_tmp4FA));({void*_tmp89E=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F9=_region_malloc(r,sizeof(*_tmp4F9));_tmp4F9->tag=4U,_tmp4F9->f1=_tmp510,_tmp4F9->f2=0U,_tmp4F9->f3=1;_tmp4F9;});_tmp4FA->hd=_tmp89E;}),_tmp4FA->tl=tms;_tmp4FA;});
return({struct _tuple12 _tmp559;_tmp559.f1=_tmp4FD,_tmp559.f2=_tmp4FC,_tmp559.f3=tms;_tmp559;});};};case 1U: _LL7: _tmp513=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DA)->f1;_tmp512=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DA)->f2;_tmp511=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DA)->f3;_LL8:
# 2121
 if(_tmp512 == 0)
return({struct _tuple12 _tmp55A;_tmp55A.f1=tq,_tmp55A.f2=t,_tmp55A.f3=0;_tmp55A;});else{
# 2124
return Cyc_Absynpp_to_tms(r,tq,_tmp512);}case 8U: _LL9: _tmp517=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DA)->f1;_tmp516=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DA)->f2;_tmp515=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DA)->f3;_tmp514=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DA)->f4;_LLA:
# 2130
 if(_tmp514 == 0  || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple12 _tmp55B;_tmp55B.f1=tq,_tmp55B.f2=t,_tmp55B.f3=0;_tmp55B;});else{
# 2133
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp514);}default: _LLB: _LLC:
# 2138
 return({struct _tuple12 _tmp55C;_tmp55C.f1=tq,_tmp55C.f2=t,_tmp55C.f3=0;_tmp55C;});}_LL0:;}
# 2142
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2144
void*_tmp518=t;void*_tmp51D;void*_tmp51C;switch(*((int*)_tmp518)){case 1U: _LL1: _tmp51C=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp518)->f2;if(_tmp51C != 0){_LL2:
 return Cyc_Absynpp_is_char_ptr(_tmp51C);}else{goto _LL5;}case 3U: _LL3: _tmp51D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp518)->f1).elt_type;_LL4:
# 2147
 L: {
void*_tmp519=_tmp51D;void*_tmp51B;void*_tmp51A;switch(*((int*)_tmp519)){case 1U: _LL8: _tmp51A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp519)->f2;if(_tmp51A != 0){_LL9:
 _tmp51D=_tmp51A;goto L;}else{goto _LLE;}case 8U: _LLA: _tmp51B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp519)->f4;if(_tmp51B != 0){_LLB:
 _tmp51D=_tmp51B;goto L;}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp519)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp519)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 2154
 return 0;}_LL0:;}
# 2158
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp51E=_new_region("temp");struct _RegionHandle*temp=& _tmp51E;_push_region(temp);
{struct _tuple12 _tmp51F=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp520=_tmp51F;struct Cyc_Absyn_Tqual _tmp528;void*_tmp527;struct Cyc_List_List*_tmp526;_LL1: _tmp528=_tmp520.f1;_tmp527=_tmp520.f2;_tmp526=_tmp520.f3;_LL2:;
_tmp526=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp526);
if(_tmp526 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp522=({struct Cyc_PP_Doc*_tmp521[2U];({struct Cyc_PP_Doc*_tmp8A0=Cyc_Absynpp_tqual2doc(_tmp528);_tmp521[0]=_tmp8A0;}),({struct Cyc_PP_Doc*_tmp89F=Cyc_Absynpp_ntyp2doc(_tmp527);_tmp521[1]=_tmp89F;});Cyc_PP_cat(_tag_dyneither(_tmp521,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp522;}else{
# 2165
struct Cyc_PP_Doc*_tmp525=({struct Cyc_PP_Doc*_tmp523[4U];({
struct Cyc_PP_Doc*_tmp8A6=Cyc_Absynpp_tqual2doc(_tmp528);_tmp523[0]=_tmp8A6;}),({
struct Cyc_PP_Doc*_tmp8A5=Cyc_Absynpp_ntyp2doc(_tmp527);_tmp523[1]=_tmp8A5;}),({
struct Cyc_PP_Doc*_tmp8A4=Cyc_PP_text(({const char*_tmp524=" ";_tag_dyneither(_tmp524,sizeof(char),2U);}));_tmp523[2]=_tmp8A4;}),({
struct Cyc_PP_Doc*_tmp8A3=({int _tmp8A2=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp8A1=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp8A2,_tmp8A1,_tmp526);});_tmp523[3]=_tmp8A3;});Cyc_PP_cat(_tag_dyneither(_tmp523,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp525;}}
# 2160
;_pop_region(temp);}
# 2173
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp529=f->requires_clause;
if((unsigned int)_tmp529)
requires_doc=({struct Cyc_PP_Doc*_tmp52A[2U];({struct Cyc_PP_Doc*_tmp8A8=Cyc_PP_text(({const char*_tmp52B="@requires ";_tag_dyneither(_tmp52B,sizeof(char),11U);}));_tmp52A[0]=_tmp8A8;}),({struct Cyc_PP_Doc*_tmp8A7=Cyc_Absynpp_exp2doc(_tmp529);_tmp52A[1]=_tmp8A7;});Cyc_PP_cat(_tag_dyneither(_tmp52A,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2179
requires_doc=Cyc_PP_nil_doc();}{
# 2181
enum Cyc_Cyclone_C_Compilers _tmp52C=Cyc_Cyclone_c_compiler;if(_tmp52C == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2184
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp52D[5U];({struct Cyc_PP_Doc*_tmp8B0=({struct Cyc_Absyn_Tqual _tmp8AF=f->tq;void*_tmp8AE=f->type;Cyc_Absynpp_tqtd2doc(_tmp8AF,_tmp8AE,({struct Cyc_Core_Opt*_tmp52E=_cycalloc(sizeof(*_tmp52E));({struct Cyc_PP_Doc*_tmp8AD=Cyc_PP_textptr(f->name);_tmp52E->v=_tmp8AD;});_tmp52E;}));});_tmp52D[0]=_tmp8B0;}),({
struct Cyc_PP_Doc*_tmp8AC=Cyc_PP_text(({const char*_tmp52F=":";_tag_dyneither(_tmp52F,sizeof(char),2U);}));_tmp52D[1]=_tmp8AC;}),({struct Cyc_PP_Doc*_tmp8AB=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp52D[2]=_tmp8AB;}),({
struct Cyc_PP_Doc*_tmp8AA=Cyc_Absynpp_atts2doc(f->attributes);_tmp52D[3]=_tmp8AA;}),({struct Cyc_PP_Doc*_tmp8A9=Cyc_PP_text(({const char*_tmp530=";";_tag_dyneither(_tmp530,sizeof(char),2U);}));_tmp52D[4]=_tmp8A9;});Cyc_PP_cat(_tag_dyneither(_tmp52D,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2189
return({struct Cyc_PP_Doc*_tmp531[4U];({struct Cyc_PP_Doc*_tmp8B6=({struct Cyc_Absyn_Tqual _tmp8B5=f->tq;void*_tmp8B4=f->type;Cyc_Absynpp_tqtd2doc(_tmp8B5,_tmp8B4,({struct Cyc_Core_Opt*_tmp532=_cycalloc(sizeof(*_tmp532));({struct Cyc_PP_Doc*_tmp8B3=Cyc_PP_textptr(f->name);_tmp532->v=_tmp8B3;});_tmp532;}));});_tmp531[0]=_tmp8B6;}),({
struct Cyc_PP_Doc*_tmp8B2=Cyc_Absynpp_atts2doc(f->attributes);_tmp531[1]=_tmp8B2;}),_tmp531[2]=requires_doc,({struct Cyc_PP_Doc*_tmp8B1=Cyc_PP_text(({const char*_tmp533=";";_tag_dyneither(_tmp533,sizeof(char),2U);}));_tmp531[3]=_tmp8B1;});Cyc_PP_cat(_tag_dyneither(_tmp531,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2192
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp534[5U];({struct Cyc_PP_Doc*_tmp8BE=Cyc_Absynpp_atts2doc(f->attributes);_tmp534[0]=_tmp8BE;}),({
struct Cyc_PP_Doc*_tmp8BD=({struct Cyc_Absyn_Tqual _tmp8BC=f->tq;void*_tmp8BB=f->type;Cyc_Absynpp_tqtd2doc(_tmp8BC,_tmp8BB,({struct Cyc_Core_Opt*_tmp535=_cycalloc(sizeof(*_tmp535));({struct Cyc_PP_Doc*_tmp8BA=Cyc_PP_textptr(f->name);_tmp535->v=_tmp8BA;});_tmp535;}));});_tmp534[1]=_tmp8BD;}),({
struct Cyc_PP_Doc*_tmp8B9=Cyc_PP_text(({const char*_tmp536=":";_tag_dyneither(_tmp536,sizeof(char),2U);}));_tmp534[2]=_tmp8B9;}),({struct Cyc_PP_Doc*_tmp8B8=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp534[3]=_tmp8B8;}),({struct Cyc_PP_Doc*_tmp8B7=Cyc_PP_text(({const char*_tmp537=";";_tag_dyneither(_tmp537,sizeof(char),2U);}));_tmp534[4]=_tmp8B7;});Cyc_PP_cat(_tag_dyneither(_tmp534,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2197
return({struct Cyc_PP_Doc*_tmp538[4U];({struct Cyc_PP_Doc*_tmp8C4=Cyc_Absynpp_atts2doc(f->attributes);_tmp538[0]=_tmp8C4;}),({
struct Cyc_PP_Doc*_tmp8C3=({struct Cyc_Absyn_Tqual _tmp8C2=f->tq;void*_tmp8C1=f->type;Cyc_Absynpp_tqtd2doc(_tmp8C2,_tmp8C1,({struct Cyc_Core_Opt*_tmp539=_cycalloc(sizeof(*_tmp539));({struct Cyc_PP_Doc*_tmp8C0=Cyc_PP_textptr(f->name);_tmp539->v=_tmp8C0;});_tmp539;}));});_tmp538[1]=_tmp8C3;}),_tmp538[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp8BF=Cyc_PP_text(({const char*_tmp53A=";";_tag_dyneither(_tmp53A,sizeof(char),2U);}));_tmp538[3]=_tmp8BF;});Cyc_PP_cat(_tag_dyneither(_tmp538,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;};}
# 2204
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp8C5=({const char*_tmp53B="";_tag_dyneither(_tmp53B,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp8C5,fields);});}
# 2208
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp53C[3U];({struct Cyc_PP_Doc*_tmp8C9=Cyc_Absynpp_scope2doc(f->sc);_tmp53C[0]=_tmp8C9;}),({struct Cyc_PP_Doc*_tmp8C8=Cyc_Absynpp_typedef_name2doc(f->name);_tmp53C[1]=_tmp8C8;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp8C7=Cyc_PP_nil_doc();_tmp53C[2]=_tmp8C7;}):({struct Cyc_PP_Doc*_tmp8C6=Cyc_Absynpp_args2doc(f->typs);_tmp53C[2]=_tmp8C6;});Cyc_PP_cat(_tag_dyneither(_tmp53C,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2213
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp8CA=({const char*_tmp53D=",";_tag_dyneither(_tmp53D,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp8CA,fields);});}
# 2222
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp8CB=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp8CB,72,f);});
({void*_tmp53E=0U;({struct Cyc___cycFILE*_tmp8CD=f;struct _dyneither_ptr _tmp8CC=({const char*_tmp53F="\n";_tag_dyneither(_tmp53F,sizeof(char),2U);});Cyc_fprintf(_tmp8CD,_tmp8CC,_tag_dyneither(_tmp53E,sizeof(void*),0U));});});}}
# 2229
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _dyneither_ptr _tmp8CE=({const char*_tmp540="";_tag_dyneither(_tmp540,sizeof(char),1U);});Cyc_PP_seql(_tmp8CE,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2232
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2236
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2247
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
