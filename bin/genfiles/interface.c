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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};
# 117 "core.h"
void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 145
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 131 "dict.h"
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 946
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 1090
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc);
# 1095
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_RgnOrder_RgnPO;
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
# 68 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty();struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 66 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);
# 75
void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 33 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc =0U,Cyc_Extern_sc =1U,Cyc_ExternC_sc =2U,Cyc_Static_sc =3U,Cyc_Auto_sc =4U,Cyc_Register_sc =5U,Cyc_Abstract_sc =6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 33 "tcutil.h"
void Cyc_Tcutil_terr(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 47
int Cyc_Tcutil_is_function_type(void*t);
# 112
void*Cyc_Tcutil_compress(void*t);
# 141
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 178
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 39 "tcdecl.h"
void Cyc_Tcdecl_merr(unsigned loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 60 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 63
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 65
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 67
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 76
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 30 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 45 "interface.cyc"
void Cyc_Lex_lex_init();struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};struct Cyc_Interface_I{struct Cyc_Interface_Ienv*imports;struct Cyc_Interface_Ienv*exports;struct Cyc_List_List*tds;};
# 73
static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));({
struct Cyc_Dict_Dict _tmp239=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->aggrdecls=_tmp239;}),({
struct Cyc_Dict_Dict _tmp238=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->datatypedecls=_tmp238;}),({
struct Cyc_Dict_Dict _tmp237=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->enumdecls=_tmp237;}),({
struct Cyc_Dict_Dict _tmp236=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->typedefdecls=_tmp236;}),({
struct Cyc_Dict_Dict _tmp235=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->vardecls=_tmp235;}),({
struct Cyc_Dict_Dict _tmp234=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp0->xdatatypefielddecls=_tmp234;});_tmp0;});}
# 83
struct Cyc_Interface_I*Cyc_Interface_empty(){
return({struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Interface_Ienv*_tmp23B=Cyc_Interface_new_ienv();_tmp1->imports=_tmp23B;}),({struct Cyc_Interface_Ienv*_tmp23A=Cyc_Interface_new_ienv();_tmp1->exports=_tmp23A;}),_tmp1->tds=0;_tmp1;});}
# 88
struct Cyc_Interface_I*Cyc_Interface_final(){
struct Cyc_Interface_I*_tmp2=Cyc_Interface_empty();
# 91
struct Cyc_Absyn_Datatypedecl*_tmp3=({struct Cyc_Absyn_Datatypedecl*_tmp9=_cycalloc(sizeof(*_tmp9));({struct Cyc_Absyn_Datatypedecl _tmp23C=*Cyc_Absyn_exn_tud();*_tmp9=_tmp23C;});_tmp9;});
_tmp3->sc=Cyc_Absyn_Public;
({struct Cyc_Dict_Dict _tmp23D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)((_tmp2->exports)->datatypedecls,Cyc_Absyn_exn_name,_tmp3);(_tmp2->exports)->datatypedecls=_tmp23D;});
# 95
{struct Cyc_List_List*_tmp4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((Cyc_Absyn_exn_tud())->fields))->v;for(0;_tmp4 != 0;_tmp4=_tmp4->tl){
struct Cyc_Absyn_Datatypefield*_tmp5=({struct Cyc_Absyn_Datatypefield*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=*((struct Cyc_Absyn_Datatypefield*)_tmp4->hd);_tmp8;});
_tmp5->sc=Cyc_Absyn_Public;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp6=({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->base=_tmp3,_tmp7->field=_tmp5;_tmp7;});
({struct Cyc_Dict_Dict _tmp23E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)((_tmp2->exports)->xdatatypefielddecls,((struct Cyc_Absyn_Datatypefield*)_tmp4->hd)->name,_tmp6);(_tmp2->exports)->xdatatypefielddecls=_tmp23E;});};}}
# 120 "interface.cyc"
_tmp2->imports=_tmp2->exports;
return _tmp2;}
# 140 "interface.cyc"
static void Cyc_Interface_err(struct _dyneither_ptr msg){
({void*_tmpA=0U;({struct _dyneither_ptr _tmp23F=msg;Cyc_Tcutil_terr(0U,_tmp23F,_tag_dyneither(_tmpA,sizeof(void*),0U));});});}
# 143
static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=Cyc_Core_Invalid_argument,_tmpB->f1=s;_tmpB;}));}
# 146
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0U,_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);_tmp203;});struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0U,_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);_tmp202;});struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0U,({
struct _dyneither_ptr _tmp240=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n));_tmp201.f1=_tmp240;});_tmp201;});void*_tmpC[3U];_tmpC[0]=& _tmpE,_tmpC[1]=& _tmpF,_tmpC[2]=& _tmp10;({struct _dyneither_ptr _tmp241=({const char*_tmpD="fields of %s %s %s have never been defined";_tag_dyneither(_tmpD,sizeof(char),43U);});Cyc_aprintf(_tmp241,_tag_dyneither(_tmpC,sizeof(void*),3U));});}));}
# 150
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0U,_tmp205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc);_tmp205;});struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0U,({
struct _dyneither_ptr _tmp242=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n));_tmp204.f1=_tmp242;});_tmp204;});void*_tmp11[2U];_tmp11[0]=& _tmp13,_tmp11[1]=& _tmp14;({struct _dyneither_ptr _tmp243=({const char*_tmp12="the body of %s function %s has never been defined";_tag_dyneither(_tmp12,sizeof(char),50U);});Cyc_aprintf(_tmp243,_tag_dyneither(_tmp11,sizeof(void*),2U));});}));}
# 156
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if(({char*_tmp244=(char*)obj1.curr;_tmp244 != (char*)(_tag_dyneither(0,0,0)).curr;}))
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp209;_tmp209.tag=0U,_tmp209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);_tmp209;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp208;_tmp208.tag=0U,({
struct _dyneither_ptr _tmp245=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1));_tmp208.f1=_tmp245;});_tmp208;});struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0U,_tmp207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);_tmp207;});struct Cyc_String_pa_PrintArg_struct _tmp1A=({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0U,({
struct _dyneither_ptr _tmp246=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name2));_tmp206.f1=_tmp246;});_tmp206;});void*_tmp15[4U];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18,_tmp15[2]=& _tmp19,_tmp15[3]=& _tmp1A;({struct _dyneither_ptr _tmp247=({const char*_tmp16="declaration of %s %s relies on static %s %s";_tag_dyneither(_tmp16,sizeof(char),44U);});Cyc_aprintf(_tmp247,_tag_dyneither(_tmp15,sizeof(void*),4U));});}));}
# 164
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct _tuple0*name2){
if(({char*_tmp248=(char*)obj1.curr;_tmp248 != (char*)(_tag_dyneither(0,0,0)).curr;}))
Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0U,_tmp20D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1);_tmp20D;});struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp20C;_tmp20C.tag=0U,({
struct _dyneither_ptr _tmp249=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1));_tmp20C.f1=_tmp249;});_tmp20C;});struct Cyc_String_pa_PrintArg_struct _tmp1F=({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0U,_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj2);_tmp20B;});struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp20A;_tmp20A.tag=0U,({
struct _dyneither_ptr _tmp24A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name2));_tmp20A.f1=_tmp24A;});_tmp20A;});void*_tmp1B[4U];_tmp1B[0]=& _tmp1D,_tmp1B[1]=& _tmp1E,_tmp1B[2]=& _tmp1F,_tmp1B[3]=& _tmp20;({struct _dyneither_ptr _tmp24B=({const char*_tmp1C="declaration of %s %s relies on fields of abstract %s %s";_tag_dyneither(_tmp1C,sizeof(char),56U);});Cyc_aprintf(_tmp24B,_tag_dyneither(_tmp1B,sizeof(void*),4U));});}));}struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;};
# 181
static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){
return({struct Cyc_Interface_Seen*_tmp21=_cycalloc(sizeof(*_tmp21));({struct Cyc_Dict_Dict _tmp24D=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp21->aggrs=_tmp24D;}),({
struct Cyc_Dict_Dict _tmp24C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp21->datatypes=_tmp24C;});_tmp21;});}
# 187
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);
# 190
static int Cyc_Interface_check_public_type_list(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){
# 192
int _tmp22=1;
for(0;l != 0;l=l->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp251=ae;struct Cyc_Interface_Seen*_tmp250=seen;struct _dyneither_ptr _tmp24F=obj;struct _tuple0*_tmp24E=name;Cyc_Interface_check_public_type(_tmp251,_tmp250,_tmp24F,_tmp24E,f(l->hd));}))
_tmp22=0;}
return _tmp22;}
# 199
static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){
{struct _handler_cons _tmp23;_push_handler(& _tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{int _tmp26=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0U);return _tmp26;};_pop_handler();}else{void*_tmp24=(void*)Cyc_Core_get_exn_thrown();void*_tmp27=_tmp24;void*_tmp28;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp27)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp28=_tmp27;_LL4:(int)_rethrow(_tmp28);}_LL0:;}};}{
int _tmp29=1;
({struct Cyc_Dict_Dict _tmp252=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp29);seen->aggrs=_tmp252;});
if(d->impl != 0){
struct Cyc_List_List*_tmp2A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;for(0;_tmp2A != 0;_tmp2A=_tmp2A->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp256=ae;struct Cyc_Interface_Seen*_tmp255=seen;struct _dyneither_ptr _tmp254=({const char*_tmp2B="type";_tag_dyneither(_tmp2B,sizeof(char),5U);});struct _tuple0*_tmp253=d->name;Cyc_Interface_check_public_type(_tmp256,_tmp255,_tmp254,_tmp253,((struct Cyc_Absyn_Aggrfield*)_tmp2A->hd)->type);}))
_tmp29=0;}}
# 209
({struct Cyc_Dict_Dict _tmp257=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp29);seen->aggrs=_tmp257;});
return _tmp29;};}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
# 213
static void*Cyc_Interface_get_type1(struct _tuple20*x){
return(*x).f2;}
# 216
static void*Cyc_Interface_get_type2(struct _tuple8*x){
return(*x).f3;}
# 220
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*d){
{struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!_tmp2E){{int _tmp2F=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0U);return _tmp2F;};_pop_handler();}else{void*_tmp2D=(void*)Cyc_Core_get_exn_thrown();void*_tmp30=_tmp2D;void*_tmp31;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp30)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp31=_tmp30;_LL4:(int)_rethrow(_tmp31);}_LL0:;}};}{
int _tmp32=1;
({struct Cyc_Dict_Dict _tmp258=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp32);seen->datatypes=_tmp258;});
if(d->fields != 0){
struct Cyc_List_List*_tmp33=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
if(!({struct Cyc_Tcenv_Genv*_tmp25C=ae;struct Cyc_Interface_Seen*_tmp25B=seen;struct _dyneither_ptr _tmp25A=({const char*_tmp34="datatype";_tag_dyneither(_tmp34,sizeof(char),9U);});struct _tuple0*_tmp259=d->name;((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(_tmp25C,_tmp25B,_tmp25A,_tmp259,Cyc_Interface_get_type1,((struct Cyc_Absyn_Datatypefield*)_tmp33->hd)->typs);}))
# 229
_tmp32=0;}}
# 231
({struct Cyc_Dict_Dict _tmp25D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,_tmp32);seen->datatypes=_tmp25D;});
return _tmp32;};}
# 235
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}
# 239
static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){
if(d->defn != 0)
return({struct Cyc_Tcenv_Genv*_tmp261=ae;struct Cyc_Interface_Seen*_tmp260=seen;struct _dyneither_ptr _tmp25F=_tag_dyneither(0,0,0);struct _tuple0*_tmp25E=d->name;Cyc_Interface_check_public_type(_tmp261,_tmp260,_tmp25F,_tmp25E,(void*)_check_null(d->defn));});else{
return 1;}}
# 246
static int Cyc_Interface_check_public_vardecl(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){
return({struct Cyc_Tcenv_Genv*_tmp265=ae;struct Cyc_Interface_Seen*_tmp264=seen;struct _dyneither_ptr _tmp263=({const char*_tmp35="variable";_tag_dyneither(_tmp35,sizeof(char),9U);});struct _tuple0*_tmp262=d->name;Cyc_Interface_check_public_type(_tmp265,_tmp264,_tmp263,_tmp262,d->type);});}
# 250
static int Cyc_Interface_check_public_type(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t){
void*_tmp36=Cyc_Tcutil_compress(t);void*_tmp37=_tmp36;struct Cyc_Absyn_Datatypedecl*_tmp71;struct Cyc_Absyn_Datatypefield*_tmp70;struct Cyc_List_List*_tmp6F;struct Cyc_Absyn_Datatypedecl*_tmp6E;struct Cyc_List_List*_tmp6D;struct _tuple0*_tmp6C;union Cyc_Absyn_AggrInfo _tmp6B;struct Cyc_List_List*_tmp6A;struct Cyc_List_List*_tmp69;void*_tmp68;struct Cyc_List_List*_tmp67;struct Cyc_Absyn_VarargInfo*_tmp66;void*_tmp65;void*_tmp64;void*_tmp63;switch(*((int*)_tmp37)){case 3U: _LL1: _tmp63=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37)->f1).elt_type;_LL2:
 _tmp64=_tmp63;goto _LL4;case 4U: _LL3: _tmp64=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37)->f1).elt_type;_LL4:
 _tmp65=_tmp64;goto _LL6;case 8U: _LL5: _tmp65=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37)->f4;if(_tmp65 != 0){_LL6:
# 255
 return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp65);}else{goto _LL13;}case 5U: _LL7: _tmp68=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp37)->f1).ret_type;_tmp67=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp37)->f1).args;_tmp66=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp37)->f1).cyc_varargs;_LL8: {
# 259
int b=((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple8*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,_tmp67)&&
 Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp68);
if(_tmp66 != 0){
struct Cyc_Absyn_VarargInfo _tmp38=*_tmp66;struct Cyc_Absyn_VarargInfo _tmp39=_tmp38;void*_tmp3A;_LL16: _tmp3A=_tmp39.type;_LL17:;
b=Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp3A);}
# 265
return b;}case 6U: _LL9: _tmp69=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp37)->f1;_LLA:
# 268
 return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple20*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,_tmp69);case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)){case 20U: _LLB: _tmp6B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1;_tmp6A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f2;_LLC: {
# 271
struct Cyc_Absyn_Aggrdecl*_tmp3B=Cyc_Absyn_get_known_aggrdecl(_tmp6B);
if((int)_tmp3B->sc == (int)Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp268=obj;struct _tuple0*_tmp267=name;struct _dyneither_ptr _tmp266=({const char*_tmp3C="type";_tag_dyneither(_tmp3C,sizeof(char),5U);});Cyc_Interface_static_err(_tmp268,_tmp267,_tmp266,_tmp3B->name);});
return 0;}
# 276
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp6A)&&
 Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp3B);}case 15U: _LLD: _tmp6C=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1;_LLE: {
# 280
struct Cyc_Absyn_Enumdecl*ed;
{struct _handler_cons _tmp3D;_push_handler(& _tmp3D);{int _tmp3F=0;if(setjmp(_tmp3D.handler))_tmp3F=1;if(!_tmp3F){ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->enumdecls,_tmp6C);;_pop_handler();}else{void*_tmp3E=(void*)Cyc_Core_get_exn_thrown();void*_tmp40=_tmp3E;void*_tmp44;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp40)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
# 283
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp43=({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0U,({
struct _dyneither_ptr _tmp269=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6C));_tmp20E.f1=_tmp269;});_tmp20E;});void*_tmp41[1U];_tmp41[0]=& _tmp43;({struct _dyneither_ptr _tmp26A=({const char*_tmp42="check_public_type (can't find enum %s)";_tag_dyneither(_tmp42,sizeof(char),39U);});Cyc_aprintf(_tmp26A,_tag_dyneither(_tmp41,sizeof(void*),1U));});}));}else{_LL1B: _tmp44=_tmp40;_LL1C:(int)_rethrow(_tmp44);}_LL18:;}};}
# 286
if((int)ed->sc == (int)Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp26D=obj;struct _tuple0*_tmp26C=_tmp6C;struct _dyneither_ptr _tmp26B=({const char*_tmp45="enum";_tag_dyneither(_tmp45,sizeof(char),5U);});Cyc_Interface_static_err(_tmp26D,_tmp26C,_tmp26B,ed->name);});
return 0;}
# 290
return 1;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatype).tag == 2){_LLF: _tmp6E=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatype).val;_tmp6D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f2;_LL10: {
# 293
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler))_tmp48=1;if(!_tmp48){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp6E->name);;_pop_handler();}else{void*_tmp47=(void*)Cyc_Core_get_exn_thrown();void*_tmp49=_tmp47;void*_tmp4D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp49)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 296
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4C=({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0U,({
struct _dyneither_ptr _tmp26E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6E->name));_tmp20F.f1=_tmp26E;});_tmp20F;});void*_tmp4A[1U];_tmp4A[0]=& _tmp4C;({struct _dyneither_ptr _tmp26F=({const char*_tmp4B="check_public_type (can't find datatype %s)";_tag_dyneither(_tmp4B,sizeof(char),43U);});Cyc_aprintf(_tmp26F,_tag_dyneither(_tmp4A,sizeof(void*),1U));});}));}else{_LL20: _tmp4D=_tmp49;_LL21:(int)_rethrow(_tmp4D);}_LL1D:;}};}
# 299
if((int)tud->sc == (int)Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp272=obj;struct _tuple0*_tmp271=name;struct _dyneither_ptr _tmp270=({const char*_tmp4E="datatype";_tag_dyneither(_tmp4E,sizeof(char),9U);});Cyc_Interface_static_err(_tmp272,_tmp271,_tmp270,tud->name);});
return 0;}
# 303
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp6D)&&
 Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}else{goto _LL13;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatypefield).tag == 2){_LL11: _tmp71=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatypefield).val).f1;_tmp70=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f1)->f1).KnownDatatypefield).val).f2;_tmp6F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37)->f2;_LL12: {
# 307
struct Cyc_Absyn_Datatypedecl*tud;
{struct _handler_cons _tmp4F;_push_handler(& _tmp4F);{int _tmp51=0;if(setjmp(_tmp4F.handler))_tmp51=1;if(!_tmp51){tud=*((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(ae->datatypedecls,_tmp71->name);;_pop_handler();}else{void*_tmp50=(void*)Cyc_Core_get_exn_thrown();void*_tmp52=_tmp50;void*_tmp56;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp52)->tag == Cyc_Dict_Absent){_LL23: _LL24:
# 310
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0U,({
struct _dyneither_ptr _tmp273=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp71->name));_tmp210.f1=_tmp273;});_tmp210;});void*_tmp53[1U];_tmp53[0]=& _tmp55;({struct _dyneither_ptr _tmp274=({const char*_tmp54="check_public_type (can't find datatype %s and search its fields)";_tag_dyneither(_tmp54,sizeof(char),65U);});Cyc_aprintf(_tmp274,_tag_dyneither(_tmp53,sizeof(void*),1U));});}));}else{_LL25: _tmp56=_tmp52;_LL26:(int)_rethrow(_tmp56);}_LL22:;}};}
# 313
if(tud->fields == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0U,({
struct _dyneither_ptr _tmp275=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tud->name));_tmp211.f1=_tmp275;});_tmp211;});void*_tmp57[1U];_tmp57[0]=& _tmp59;({struct _dyneither_ptr _tmp276=({const char*_tmp58="check_public_type (datatype %s has no fields)";_tag_dyneither(_tmp58,sizeof(char),46U);});Cyc_aprintf(_tmp276,_tag_dyneither(_tmp57,sizeof(void*),1U));});}));{
# 317
struct Cyc_Absyn_Datatypefield*tuf1=0;
{struct Cyc_List_List*_tmp5A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){
if(Cyc_strptrcmp((*_tmp70->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp5A->hd)->name).f2)== 0){
tuf1=(struct Cyc_Absyn_Datatypefield*)_tmp5A->hd;
break;}}}
# 324
if(tuf1 == 0)
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp212;_tmp212.tag=0U,({
struct _dyneither_ptr _tmp277=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp70->name));_tmp212.f1=_tmp277;});_tmp212;});void*_tmp5B[1U];_tmp5B[0]=& _tmp5D;({struct _dyneither_ptr _tmp278=({const char*_tmp5C="check_public_type (can't find datatypefield %s)";_tag_dyneither(_tmp5C,sizeof(char),48U);});Cyc_aprintf(_tmp278,_tag_dyneither(_tmp5B,sizeof(void*),1U));});}));{
struct Cyc_Absyn_Datatypefield*tuf=tuf1;
# 329
if((int)tud->sc == (int)Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp27B=obj;struct _tuple0*_tmp27A=name;struct _dyneither_ptr _tmp279=({const char*_tmp5E="datatype";_tag_dyneither(_tmp5E,sizeof(char),9U);});Cyc_Interface_static_err(_tmp27B,_tmp27A,_tmp279,tud->name);});
return 0;}
# 333
if((int)tud->sc == (int)Cyc_Absyn_Abstract){
({struct _dyneither_ptr _tmp27E=obj;struct _tuple0*_tmp27D=name;struct _dyneither_ptr _tmp27C=({const char*_tmp5F="datatype";_tag_dyneither(_tmp5F,sizeof(char),9U);});Cyc_Interface_abstract_err(_tmp27E,_tmp27D,_tmp27C,tud->name);});
return 0;}
# 337
if((int)tuf->sc == (int)Cyc_Absyn_Static){
({struct _dyneither_ptr _tmp283=obj;struct _tuple0*_tmp282=name;struct _dyneither_ptr _tmp281=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp213;_tmp213.tag=0U,({
struct _dyneither_ptr _tmp27F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp213.f1=_tmp27F;});_tmp213;});void*_tmp60[1U];_tmp60[0]=& _tmp62;({struct _dyneither_ptr _tmp280=({const char*_tmp61="field %s of";_tag_dyneither(_tmp61,sizeof(char),12U);});Cyc_aprintf(_tmp280,_tag_dyneither(_tmp60,sizeof(void*),1U));});});
# 338
Cyc_Interface_static_err(_tmp283,_tmp282,_tmp281,tud->name);});
# 340
return 0;}
# 342
return((int(*)(struct Cyc_Tcenv_Genv*ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,(void*(*)(void*))Cyc_Core_identity,_tmp6F)&&
 Cyc_Interface_check_public_datatypedecl(ae,seen,tud);};};}}else{goto _LL13;}default: goto _LL13;}default: _LL13: _LL14:
# 345
 return 1;}_LL0:;}struct _tuple21{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Tcenv_Genv*f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*f6;};
# 351
static void Cyc_Interface_extract_aggrdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Aggrdecl**dp){
# 353
struct _tuple21*_tmp72=env;struct Cyc_Interface_Ienv*_tmp83;struct Cyc_Interface_Ienv*_tmp82;int _tmp81;struct Cyc_Tcenv_Genv*_tmp80;struct Cyc_Interface_Seen*_tmp7F;_LL1: _tmp83=_tmp72->f1;_tmp82=_tmp72->f2;_tmp81=_tmp72->f3;_tmp80=_tmp72->f4;_tmp7F=_tmp72->f5;_LL2:;{
struct Cyc_Absyn_Aggrdecl*_tmp73=*dp;
enum Cyc_Absyn_Scope _tmp74=_tmp73->sc;enum Cyc_Absyn_Scope _tmp75=_tmp74;switch(_tmp75){case Cyc_Absyn_Static: _LL4: _LL5:
# 357
 if(_tmp81 && _tmp73->impl == 0)
({struct _dyneither_ptr _tmp285=({const char*_tmp76="static";_tag_dyneither(_tmp76,sizeof(char),7U);});struct _dyneither_ptr _tmp284=({const char*_tmp77="struct/union";_tag_dyneither(_tmp77,sizeof(char),13U);});Cyc_Interface_fields_err(_tmp285,_tmp284,_tmp73->name);});
goto _LL3;case Cyc_Absyn_Abstract: _LL6: _LL7:
# 361
 if(_tmp73->impl == 0){
if(_tmp81)
({struct _dyneither_ptr _tmp287=({const char*_tmp78="abstract";_tag_dyneither(_tmp78,sizeof(char),9U);});struct _dyneither_ptr _tmp286=({const char*_tmp79="struct/union";_tag_dyneither(_tmp79,sizeof(char),13U);});Cyc_Interface_fields_err(_tmp287,_tmp286,_tmp73->name);});}else{
# 365
_tmp73=({struct Cyc_Absyn_Aggrdecl*_tmp7A=_cycalloc(sizeof(*_tmp7A));*_tmp7A=*_tmp73;_tmp7A;});
_tmp73->impl=0;}
# 368
if(Cyc_Interface_check_public_aggrdecl(_tmp80,_tmp7F,_tmp73))
({struct Cyc_Dict_Dict _tmp288=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp82->aggrdecls,x,_tmp73);_tmp82->aggrdecls=_tmp288;});
goto _LL3;case Cyc_Absyn_Public: _LL8: _LL9:
# 372
 if(_tmp73->impl == 0){
({struct _dyneither_ptr _tmp28A=({const char*_tmp7B="public";_tag_dyneither(_tmp7B,sizeof(char),7U);});struct _dyneither_ptr _tmp289=({const char*_tmp7C="struct/union";_tag_dyneither(_tmp7C,sizeof(char),13U);});Cyc_Interface_fields_err(_tmp28A,_tmp289,_tmp73->name);});
_tmp73=({struct Cyc_Absyn_Aggrdecl*_tmp7D=_cycalloc(sizeof(*_tmp7D));*_tmp7D=*_tmp73;_tmp7D;});
_tmp73->sc=Cyc_Absyn_Abstract;}
# 377
if(Cyc_Interface_check_public_aggrdecl(_tmp80,_tmp7F,_tmp73))
({struct Cyc_Dict_Dict _tmp28B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp82->aggrdecls,x,_tmp73);_tmp82->aggrdecls=_tmp28B;});
goto _LL3;case Cyc_Absyn_ExternC: _LLA: _LLB:
 goto _LLD;case Cyc_Absyn_Extern: _LLC: _LLD:
# 382
 if(Cyc_Interface_check_public_aggrdecl(_tmp80,_tmp7F,_tmp73))
({struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp83->aggrdecls,x,_tmp73);_tmp83->aggrdecls=_tmp28C;});
goto _LL3;case Cyc_Absyn_Register: _LLE: _LLF:
 goto _LL11;default: _LL10: _LL11:
# 387
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp7E="add_aggrdecl";_tag_dyneither(_tmp7E,sizeof(char),13U);}));
goto _LL3;}_LL3:;};}
# 391
static void Cyc_Interface_extract_enumdecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Enumdecl**dp){
# 393
struct _tuple21*_tmp84=env;struct Cyc_Interface_Ienv*_tmp95;struct Cyc_Interface_Ienv*_tmp94;int _tmp93;struct Cyc_Tcenv_Genv*_tmp92;struct Cyc_Interface_Seen*_tmp91;_LL1: _tmp95=_tmp84->f1;_tmp94=_tmp84->f2;_tmp93=_tmp84->f3;_tmp92=_tmp84->f4;_tmp91=_tmp84->f5;_LL2:;{
struct Cyc_Absyn_Enumdecl*_tmp85=*dp;
enum Cyc_Absyn_Scope _tmp86=_tmp85->sc;enum Cyc_Absyn_Scope _tmp87=_tmp86;switch(_tmp87){case Cyc_Absyn_Static: _LL4: _LL5:
# 397
 if(_tmp93 && _tmp85->fields == 0)
({struct _dyneither_ptr _tmp28E=({const char*_tmp88="static";_tag_dyneither(_tmp88,sizeof(char),7U);});struct _dyneither_ptr _tmp28D=({const char*_tmp89="enum";_tag_dyneither(_tmp89,sizeof(char),5U);});Cyc_Interface_fields_err(_tmp28E,_tmp28D,_tmp85->name);});
goto _LL3;case Cyc_Absyn_Abstract: _LL6: _LL7:
# 401
 if(_tmp85->fields == 0){
if(_tmp93)
({struct _dyneither_ptr _tmp290=({const char*_tmp8A="abstract";_tag_dyneither(_tmp8A,sizeof(char),9U);});struct _dyneither_ptr _tmp28F=({const char*_tmp8B="enum";_tag_dyneither(_tmp8B,sizeof(char),5U);});Cyc_Interface_fields_err(_tmp290,_tmp28F,_tmp85->name);});}else{
# 405
_tmp85=({struct Cyc_Absyn_Enumdecl*_tmp8C=_cycalloc(sizeof(*_tmp8C));*_tmp8C=*_tmp85;_tmp8C;});
_tmp85->fields=0;}
# 408
if(Cyc_Interface_check_public_enumdecl(_tmp92,_tmp91,_tmp85))
({struct Cyc_Dict_Dict _tmp291=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp94->enumdecls,x,_tmp85);_tmp94->enumdecls=_tmp291;});
goto _LL3;case Cyc_Absyn_Public: _LL8: _LL9:
# 412
 if(_tmp85->fields == 0){
({struct _dyneither_ptr _tmp293=({const char*_tmp8D="public";_tag_dyneither(_tmp8D,sizeof(char),7U);});struct _dyneither_ptr _tmp292=({const char*_tmp8E="enum";_tag_dyneither(_tmp8E,sizeof(char),5U);});Cyc_Interface_fields_err(_tmp293,_tmp292,_tmp85->name);});
_tmp85=({struct Cyc_Absyn_Enumdecl*_tmp8F=_cycalloc(sizeof(*_tmp8F));*_tmp8F=*_tmp85;_tmp8F;});
_tmp85->sc=Cyc_Absyn_Abstract;}
# 417
if(Cyc_Interface_check_public_enumdecl(_tmp92,_tmp91,_tmp85))
({struct Cyc_Dict_Dict _tmp294=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp94->enumdecls,x,_tmp85);_tmp94->enumdecls=_tmp294;});
goto _LL3;case Cyc_Absyn_ExternC: _LLA: _LLB:
 goto _LLD;case Cyc_Absyn_Extern: _LLC: _LLD:
# 422
 if(Cyc_Interface_check_public_enumdecl(_tmp92,_tmp91,_tmp85))
({struct Cyc_Dict_Dict _tmp295=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp95->enumdecls,x,_tmp85);_tmp95->enumdecls=_tmp295;});
goto _LL3;case Cyc_Absyn_Register: _LLE: _LLF:
 goto _LL11;default: _LL10: _LL11:
# 427
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp90="add_enumdecl";_tag_dyneither(_tmp90,sizeof(char),13U);}));
goto _LL3;}_LL3:;};}
# 432
static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){
# 434
struct Cyc_Interface_Ienv*env;
{enum Cyc_Absyn_Scope _tmp96=f->sc;enum Cyc_Absyn_Scope _tmp97=_tmp96;switch(_tmp97){case Cyc_Absyn_Static: _LL1: _LL2:
 return;case Cyc_Absyn_Extern: _LL3: _LL4:
 env=i->imports;goto _LL0;case Cyc_Absyn_Public: _LL5: _LL6:
 env=i->exports;goto _LL0;default: _LL7: _LL8:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmp98="add_xdatatypefielddecl";_tag_dyneither(_tmp98,sizeof(char),23U);}));}_LL0:;}
# 442
({struct Cyc_Dict_Dict _tmp298=({
struct Cyc_Dict_Dict _tmp297=env->xdatatypefielddecls;struct _tuple0*_tmp296=f->name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Tcdecl_Xdatatypefielddecl*v))Cyc_Dict_insert)(_tmp297,_tmp296,({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp99=_cycalloc(sizeof(*_tmp99));
_tmp99->base=d,_tmp99->field=f;_tmp99;}));});
# 442
env->xdatatypefielddecls=_tmp298;});}
# 447
static void Cyc_Interface_extract_datatypedecl(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Datatypedecl**dp){
# 449
struct _tuple21*_tmp9A=env;struct Cyc_Interface_Ienv*_tmpB2;struct Cyc_Interface_Ienv*_tmpB1;int _tmpB0;struct Cyc_Tcenv_Genv*_tmpAF;struct Cyc_Interface_Seen*_tmpAE;struct Cyc_Interface_I*_tmpAD;_LL1: _tmpB2=_tmp9A->f1;_tmpB1=_tmp9A->f2;_tmpB0=_tmp9A->f3;_tmpAF=_tmp9A->f4;_tmpAE=_tmp9A->f5;_tmpAD=_tmp9A->f6;_LL2:;{
struct Cyc_Absyn_Datatypedecl*_tmp9B=*dp;
# 452
enum Cyc_Absyn_Scope _tmp9C=_tmp9B->sc;enum Cyc_Absyn_Scope _tmp9D=_tmp9C;switch(_tmp9D){case Cyc_Absyn_Static: _LL4: _LL5:
# 454
 if((!_tmp9B->is_extensible && _tmpB0)&& _tmp9B->fields == 0)
({struct _dyneither_ptr _tmp29A=({const char*_tmp9E="static";_tag_dyneither(_tmp9E,sizeof(char),7U);});struct _dyneither_ptr _tmp299=({const char*_tmp9F="datatype";_tag_dyneither(_tmp9F,sizeof(char),9U);});Cyc_Interface_fields_err(_tmp29A,_tmp299,_tmp9B->name);});
# 457
goto _LL3;case Cyc_Absyn_Abstract: _LL6: _LL7:
# 459
 if(_tmp9B->fields == 0){
if(!_tmp9B->is_extensible && _tmpB0)
({struct _dyneither_ptr _tmp29C=({const char*_tmpA0="abstract";_tag_dyneither(_tmpA0,sizeof(char),9U);});struct _dyneither_ptr _tmp29B=({const char*_tmpA1="datatype";_tag_dyneither(_tmpA1,sizeof(char),9U);});Cyc_Interface_fields_err(_tmp29C,_tmp29B,_tmp9B->name);});}else{
# 464
_tmp9B=({struct Cyc_Absyn_Datatypedecl*_tmpA2=_cycalloc(sizeof(*_tmpA2));*_tmpA2=*_tmp9B;_tmpA2;});
_tmp9B->fields=0;}
# 467
if(Cyc_Interface_check_public_datatypedecl(_tmpAF,_tmpAE,_tmp9B))
({struct Cyc_Dict_Dict _tmp29D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB1->datatypedecls,x,_tmp9B);_tmpB1->datatypedecls=_tmp29D;});
goto _LL3;case Cyc_Absyn_Public: _LL8: _LL9:
# 471
 _tmp9B=({struct Cyc_Absyn_Datatypedecl*_tmpA3=_cycalloc(sizeof(*_tmpA3));*_tmpA3=*_tmp9B;_tmpA3;});
if(!_tmp9B->is_extensible && _tmp9B->fields == 0){
({struct _dyneither_ptr _tmp29F=({const char*_tmpA4="public";_tag_dyneither(_tmpA4,sizeof(char),7U);});struct _dyneither_ptr _tmp29E=({const char*_tmpA5="datatype";_tag_dyneither(_tmpA5,sizeof(char),9U);});Cyc_Interface_fields_err(_tmp29F,_tmp29E,_tmp9B->name);});
_tmp9B->sc=Cyc_Absyn_Abstract;}
# 476
if(Cyc_Interface_check_public_datatypedecl(_tmpAF,_tmpAE,_tmp9B)){
if(_tmp9B->is_extensible && _tmp9B->fields != 0){
struct Cyc_List_List*_tmpA6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9B->fields))->v;
_tmp9B->fields=0;{
struct Cyc_List_List*_tmpA7=_tmpA6;for(0;_tmpA7 != 0;_tmpA7=_tmpA7->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpAD,_tmp9B,(struct Cyc_Absyn_Datatypefield*)_tmpA7->hd);}};}
# 484
({struct Cyc_Dict_Dict _tmp2A0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB1->datatypedecls,x,_tmp9B);_tmpB1->datatypedecls=_tmp2A0;});}
# 486
goto _LL3;case Cyc_Absyn_ExternC: _LLA: _LLB:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpA8="extract_datatypedecl";_tag_dyneither(_tmpA8,sizeof(char),21U);}));case Cyc_Absyn_Extern: _LLC: _LLD:
# 489
 if(Cyc_Interface_check_public_datatypedecl(_tmpAF,_tmpAE,_tmp9B)){
if(_tmp9B->is_extensible && _tmp9B->fields != 0){
_tmp9B=({struct Cyc_Absyn_Datatypedecl*_tmpA9=_cycalloc(sizeof(*_tmpA9));*_tmpA9=*_tmp9B;_tmpA9;});{
struct Cyc_List_List*_tmpAA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9B->fields))->v;
_tmp9B->fields=0;{
struct Cyc_List_List*_tmpAB=_tmpAA;for(0;_tmpAB != 0;_tmpAB=_tmpAB->tl){
Cyc_Interface_extract_xdatatypefielddecl(_tmpAD,_tmp9B,(struct Cyc_Absyn_Datatypefield*)_tmpAB->hd);}};};}
# 498
({struct Cyc_Dict_Dict _tmp2A1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpB2->datatypedecls,x,_tmp9B);_tmpB2->datatypedecls=_tmp2A1;});}
# 500
goto _LL3;case Cyc_Absyn_Register: _LLE: _LLF:
 goto _LL11;default: _LL10: _LL11:
# 503
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpAC="add_datatypedecl";_tag_dyneither(_tmpAC,sizeof(char),17U);}));
goto _LL3;}_LL3:;};}
# 508
static void Cyc_Interface_extract_typedef(struct _tuple21*env,struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d){
# 510
struct _tuple21*_tmpB3=env;struct Cyc_Interface_Ienv*_tmpB6;struct Cyc_Tcenv_Genv*_tmpB5;struct Cyc_Interface_Seen*_tmpB4;_LL1: _tmpB6=_tmpB3->f2;_tmpB5=_tmpB3->f4;_tmpB4=_tmpB3->f5;_LL2:;
if(Cyc_Interface_check_public_typedefdecl(_tmpB5,_tmpB4,d))
({struct Cyc_Dict_Dict _tmp2A2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpB6->typedefdecls,x,d);_tmpB6->typedefdecls=_tmp2A2;});}struct _tuple22{void*f1;int f2;};
# 515
static void Cyc_Interface_extract_ordinarie(struct _tuple21*env,struct _tuple0*x,struct _tuple22*v){
# 517
struct _tuple21*_tmpB7=env;struct Cyc_Interface_Ienv*_tmpC8;struct Cyc_Interface_Ienv*_tmpC7;int _tmpC6;struct Cyc_Tcenv_Genv*_tmpC5;struct Cyc_Interface_Seen*_tmpC4;_LL1: _tmpC8=_tmpB7->f1;_tmpC7=_tmpB7->f2;_tmpC6=_tmpB7->f3;_tmpC5=_tmpB7->f4;_tmpC4=_tmpB7->f5;_LL2:;{
# 519
void*_tmpB8=(*v).f1;void*_tmpB9=_tmpB8;struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Fndecl*_tmpC2;switch(*((int*)_tmpB9)){case 2U: _LL4: _tmpC2=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpB9)->f1;_LL5: {
# 521
struct Cyc_Absyn_Vardecl*_tmpBA=({struct Cyc_Absyn_Vardecl*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->sc=_tmpC2->sc,_tmpBB->name=_tmpC2->name,_tmpBB->varloc=0U,({
# 525
struct Cyc_Absyn_Tqual _tmp2A3=Cyc_Absyn_empty_tqual(0U);_tmpBB->tq=_tmp2A3;}),_tmpBB->type=(void*)_check_null(_tmpC2->cached_type),_tmpBB->initializer=0,_tmpBB->rgn=0,_tmpBB->attributes=0,_tmpBB->escapes=0;_tmpBB;});
# 532
_tmpC6=0;
_tmpC3=_tmpBA;goto _LL7;}case 1U: _LL6: _tmpC3=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB9)->f1;_LL7:
# 535
 if(_tmpC3->initializer != 0){
_tmpC3=({struct Cyc_Absyn_Vardecl*_tmpBC=_cycalloc(sizeof(*_tmpBC));*_tmpBC=*_tmpC3;_tmpBC;});
_tmpC3->initializer=0;}
# 539
{enum Cyc_Absyn_Scope _tmpBD=_tmpC3->sc;enum Cyc_Absyn_Scope _tmpBE=_tmpBD;switch(_tmpBE){case Cyc_Absyn_Static: _LLB: _LLC:
# 541
 if(_tmpC6 && Cyc_Tcutil_is_function_type(_tmpC3->type))
({struct _dyneither_ptr _tmp2A4=({const char*_tmpBF="static";_tag_dyneither(_tmpBF,sizeof(char),7U);});Cyc_Interface_body_err(_tmp2A4,_tmpC3->name);});
# 544
goto _LLA;case Cyc_Absyn_Register: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Abstract: _LLF: _LL10:
((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpC0="extract_ordinarie";_tag_dyneither(_tmpC0,sizeof(char),18U);}));case Cyc_Absyn_Public: _LL11: _LL12:
# 548
 if(_tmpC6 && Cyc_Tcutil_is_function_type(_tmpC3->type))
({struct _dyneither_ptr _tmp2A5=({const char*_tmpC1="public";_tag_dyneither(_tmpC1,sizeof(char),7U);});Cyc_Interface_body_err(_tmp2A5,_tmpC3->name);});
# 551
if(Cyc_Interface_check_public_vardecl(_tmpC5,_tmpC4,_tmpC3))
({struct Cyc_Dict_Dict _tmp2A6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpC7->vardecls,x,_tmpC3);_tmpC7->vardecls=_tmp2A6;});
goto _LLA;case Cyc_Absyn_ExternC: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Extern: _LL15: _LL16:
 goto _LL18;default: _LL17: _LL18:
# 557
 if(Cyc_Interface_check_public_vardecl(_tmpC5,_tmpC4,_tmpC3))
({struct Cyc_Dict_Dict _tmp2A7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpC8->vardecls,x,_tmpC3);_tmpC8->vardecls=_tmp2A7;});
goto _LLA;}_LLA:;}
# 561
goto _LL3;default: _LL8: _LL9:
 goto _LL3;}_LL3:;};}struct _tuple23{void*f1;void*f2;};
# 566
static struct Cyc_List_List*Cyc_Interface_remove_decl_from_list(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*d){
if(l == 0)return 0;
{struct _tuple23 _tmpC9=({struct _tuple23 _tmp214;_tmp214.f1=d->r,_tmp214.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmp214;});struct _tuple23 _tmpCA=_tmpC9;struct Cyc_Absyn_Vardecl**_tmpD1;struct Cyc_Absyn_Vardecl*_tmpD0;struct Cyc_Absyn_Enumdecl*_tmpCF;struct Cyc_Absyn_Enumdecl*_tmpCE;struct Cyc_Absyn_Typedefdecl*_tmpCD;struct Cyc_Absyn_Typedefdecl*_tmpCC;switch(*((int*)_tmpCA.f1)){case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCA.f2)->tag == 8U){_LL1: _tmpCD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCA.f1)->f1;_tmpCC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCA.f2)->f1;_LL2:
# 570
 if(Cyc_Absyn_qvar_cmp(_tmpCD->name,_tmpCC->name)!= 0)goto _LL0;
return Cyc_Interface_remove_decl_from_list(l->tl,d);}else{goto _LL7;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCA.f2)->tag == 7U){_LL3: _tmpCF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCA.f1)->f1;_tmpCE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCA.f2)->f1;_LL4:
# 573
 if(Cyc_Absyn_qvar_cmp(_tmpCF->name,_tmpCE->name)!= 0)goto _LL0;
if((int)_tmpCF->sc == (int)Cyc_Absyn_Extern)_tmpCF->sc=_tmpCE->sc;
return Cyc_Interface_remove_decl_from_list(l->tl,d);
goto _LL0;}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCA.f2)->tag == 0U){_LL5: _tmpD1=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCA.f1)->f1;_tmpD0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCA.f2)->f1;_LL6:
# 578
 if(Cyc_Absyn_qvar_cmp((*_tmpD1)->name,_tmpD0->name)!= 0)goto _LL0;
if((int)(*_tmpD1)->sc == (int)Cyc_Absyn_Extern)({struct Cyc_Absyn_Vardecl*_tmp2A8=({struct Cyc_Absyn_Vardecl*_tmpCB=_cycalloc(sizeof(*_tmpCB));*_tmpCB=*_tmpD0;_tmpCB;});*_tmpD1=_tmp2A8;});
return Cyc_Interface_remove_decl_from_list(l->tl,d);}else{goto _LL7;}default: _LL7: _LL8:
# 582
 goto _LL0;}_LL0:;}
# 584
return({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=(struct Cyc_Absyn_Decl*)l->hd,({struct Cyc_List_List*_tmp2A9=Cyc_Interface_remove_decl_from_list(l->tl,d);_tmpD2->tl=_tmp2A9;});_tmpD2;});}
# 587
static struct Cyc_List_List*Cyc_Interface_uniqify_decl_list(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
if(accum == 0)return({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=d,_tmpD3->tl=0;_tmpD3;});{
struct Cyc_List_List*l;
for(l=accum;l != 0;l=l->tl){
struct _tuple23 _tmpD4=({struct _tuple23 _tmp215;_tmp215.f1=d->r,_tmp215.f2=((struct Cyc_Absyn_Decl*)l->hd)->r;_tmp215;});struct _tuple23 _tmpD5=_tmpD4;struct _dyneither_ptr*_tmpE1;struct Cyc_List_List**_tmpE0;struct _dyneither_ptr*_tmpDF;struct Cyc_List_List*_tmpDE;struct Cyc_Absyn_Enumdecl*_tmpDD;struct Cyc_Absyn_Enumdecl*_tmpDC;struct Cyc_Absyn_Typedefdecl*_tmpDB;struct Cyc_Absyn_Typedefdecl*_tmpDA;struct Cyc_Absyn_Vardecl*_tmpD9;struct Cyc_Absyn_Vardecl**_tmpD8;switch(*((int*)_tmpD5.f1)){case 0U: if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->tag == 0U){_LL1: _tmpD9=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD5.f1)->f1;_tmpD8=(struct Cyc_Absyn_Vardecl**)&((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->f1;_LL2:
# 593
 if(Cyc_Absyn_qvar_cmp(_tmpD9->name,(*_tmpD8)->name)!= 0)goto _LL0;
if((int)(*_tmpD8)->sc == (int)Cyc_Absyn_Extern)({struct Cyc_Absyn_Vardecl*_tmp2AA=({struct Cyc_Absyn_Vardecl*_tmpD6=_cycalloc(sizeof(*_tmpD6));*_tmpD6=*_tmpD9;_tmpD6;});*_tmpD8=_tmp2AA;});
return accum;}else{goto _LL9;}case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->tag == 8U){_LL3: _tmpDB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD5.f1)->f1;_tmpDA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->f1;_LL4:
# 597
 if(Cyc_Absyn_qvar_cmp(_tmpDB->name,_tmpDA->name)!= 0)goto _LL0;
return accum;}else{goto _LL9;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->tag == 7U){_LL5: _tmpDD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD5.f1)->f1;_tmpDC=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->f1;_LL6:
# 600
 if(Cyc_Absyn_qvar_cmp(_tmpDD->name,_tmpDC->name)!= 0)goto _LL0;
return accum;}else{goto _LL9;}case 9U: if(((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->tag == 9U){_LL7: _tmpE1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD5.f1)->f1;_tmpE0=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD5.f1)->f2;_tmpDF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->f1;_tmpDE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpD5.f2)->f2;_LL8:
# 603
 if(Cyc_strptrcmp(_tmpE1,_tmpDF)!= 0)goto _LL0;{
struct Cyc_List_List*_tmpD7=_tmpDE;
for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
({struct Cyc_List_List*_tmp2AB=Cyc_Interface_remove_decl_from_list(*_tmpE0,(struct Cyc_Absyn_Decl*)_tmpD7->hd);*_tmpE0=_tmp2AB;});}
# 608
goto _LL0;};}else{goto _LL9;}default: _LL9: _LLA:
# 610
 goto _LL0;}_LL0:;}
# 613
return({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=d,_tmpE2->tl=accum;_tmpE2;});};}
# 616
static struct Cyc_List_List*Cyc_Interface_filterstatics(struct Cyc_List_List*accum,struct Cyc_Absyn_Decl*d){
{void*_tmpE3=d->r;void*_tmpE4=_tmpE3;struct Cyc_List_List*_tmpF7;struct _dyneither_ptr*_tmpF6;struct Cyc_List_List*_tmpF5;struct Cyc_Absyn_Typedefdecl*_tmpF4;struct Cyc_Absyn_Enumdecl*_tmpF3;struct Cyc_Absyn_Datatypedecl*_tmpF2;struct Cyc_Absyn_Aggrdecl*_tmpF1;struct Cyc_Absyn_Fndecl*_tmpF0;struct Cyc_Absyn_Vardecl*_tmpEF;switch(*((int*)_tmpE4)){case 0U: _LL1: _tmpEF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpE4)->f1;_LL2:
# 619
 if((int)_tmpEF->sc == (int)Cyc_Absyn_ExternC)return accum;
if((int)_tmpEF->sc == (int)Cyc_Absyn_Static)return accum;{
struct Cyc_Absyn_Vardecl*_tmpE5=({struct Cyc_Absyn_Vardecl*_tmpE8=_cycalloc(sizeof(*_tmpE8));*_tmpE8=*_tmpEF;_tmpE8;});
_tmpE5->initializer=0;
if(Cyc_Tcutil_is_function_type(_tmpE5->type)&&(int)_tmpE5->sc != (int)Cyc_Absyn_Extern)
_tmpE5->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_Absyn_Decl*_tmp2AC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->tag=0U,_tmpE6->f1=_tmpE5;_tmpE6;}),0U);_tmpE7->hd=_tmp2AC;}),_tmpE7->tl=accum;_tmpE7;});};case 1U: _LL3: _tmpF0=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpE4)->f1;_LL4:
# 627
 if((int)_tmpF0->sc == (int)Cyc_Absyn_Static)return accum;
if((int)_tmpF0->sc == (int)Cyc_Absyn_ExternC)return accum;{
struct Cyc_Absyn_Vardecl*_tmpE9=Cyc_Absyn_new_vardecl(0U,_tmpF0->name,(void*)_check_null(_tmpF0->cached_type),0);
_tmpE9->sc=_tmpF0->sc;
return({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));({struct Cyc_Absyn_Decl*_tmp2AD=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->tag=0U,_tmpEA->f1=_tmpE9;_tmpEA;}),0U);_tmpEB->hd=_tmp2AD;}),_tmpEB->tl=accum;_tmpEB;});};case 5U: _LL5: _tmpF1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpE4)->f1;_LL6:
# 634
 if((int)_tmpF1->sc == (int)Cyc_Absyn_ExternC)return accum;
goto _LL0;case 6U: _LL7: _tmpF2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpE4)->f1;_LL8:
# 638
 if((int)_tmpF2->sc == (int)Cyc_Absyn_ExternC)return accum;
goto _LL0;case 7U: _LL9: _tmpF3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpE4)->f1;_LLA:
# 641
 if((int)_tmpF3->sc == (int)Cyc_Absyn_Static)return accum;
if((int)_tmpF3->sc == (int)Cyc_Absyn_ExternC)return accum;
goto _LL0;case 8U: _LLB: _tmpF4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpE4)->f1;_LLC:
# 645
 goto _LL0;case 9U: _LLD: _tmpF6=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpE4)->f1;_tmpF5=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpE4)->f2;_LLE: {
# 647
struct Cyc_List_List*_tmpEC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,_tmpF5));
return({struct Cyc_List_List*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct Cyc_Absyn_Decl*_tmp2AE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED->tag=9U,_tmpED->f1=_tmpF6,_tmpED->f2=_tmpEC;_tmpED;}),0U);_tmpEE->hd=_tmp2AE;}),_tmpEE->tl=accum;_tmpEE;});}case 10U: _LLF: _tmpF7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpE4)->f2;_LL10:
# 650
 return({struct Cyc_List_List*_tmp2AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,_tmpF7);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2AF,accum);});case 2U: _LL11: _LL12:
 goto _LL14;case 3U: _LL13: _LL14:
 goto _LL16;case 4U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 660
 return accum;}_LL0:;}
# 662
return({struct Cyc_List_List*_tmpF9=_cycalloc(sizeof(*_tmpF9));({struct Cyc_Absyn_Decl*_tmp2B0=({struct Cyc_Absyn_Decl*_tmpF8=_cycalloc(sizeof(*_tmpF8));*_tmpF8=*d;_tmpF8;});_tmpF9->hd=_tmp2B0;}),_tmpF9->tl=accum;_tmpF9;});}struct _tuple24{struct Cyc_Interface_I*f1;int f2;struct Cyc_Tcenv_Genv*f3;struct Cyc_Interface_Seen*f4;};
# 665
static void Cyc_Interface_extract_f(struct _tuple24*env_f){
struct _tuple24*_tmpFA=env_f;struct Cyc_Interface_I*_tmpFF;int _tmpFE;struct Cyc_Tcenv_Genv*_tmpFD;struct Cyc_Interface_Seen*_tmpFC;_LL1: _tmpFF=_tmpFA->f1;_tmpFE=_tmpFA->f2;_tmpFD=_tmpFA->f3;_tmpFC=_tmpFA->f4;_LL2:;{
struct _tuple21 _tmpFB=({struct _tuple21 _tmp216;_tmp216.f1=_tmpFF->imports,_tmp216.f2=_tmpFF->exports,_tmp216.f3=_tmpFE,_tmp216.f4=_tmpFD,_tmp216.f5=_tmpFC,_tmp216.f6=_tmpFF;_tmp216;});
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmpFB,_tmpFD->aggrdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_datatypedecl,& _tmpFB,_tmpFD->datatypedecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Enumdecl**),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,& _tmpFB,_tmpFD->enumdecls);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmpFB,_tmpFD->typedefs);
((void(*)(void(*f)(struct _tuple21*,struct _tuple0*,struct _tuple22*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmpFB,_tmpFD->ordinaries);};}
# 675
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Tcenv_Genv*ae,int check_complete_defs,struct Cyc_List_List*tds){
struct _tuple24 env=({struct _tuple24 _tmp217;({
struct Cyc_Interface_I*_tmp2B2=Cyc_Interface_empty();_tmp217.f1=_tmp2B2;}),_tmp217.f2=check_complete_defs,_tmp217.f3=ae,({struct Cyc_Interface_Seen*_tmp2B1=Cyc_Interface_new_seen();_tmp217.f4=_tmp2B1;});_tmp217;});
Cyc_Interface_extract_f(& env);{
struct Cyc_Interface_I*_tmp100=env.f1;
({struct Cyc_List_List*_tmp2B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_filterstatics,0,tds));_tmp100->tds=_tmp2B3;});
({struct Cyc_List_List*_tmp2B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,0,_tmp100->tds));_tmp100->tds=_tmp2B4;});
return _tmp100;};}
# 685
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*ae,struct Cyc_List_List*tds){
return Cyc_Interface_gen_extract(ae,1,tds);}
# 691
inline static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){
({struct Cyc_String_pa_PrintArg_struct _tmp103=({struct Cyc_String_pa_PrintArg_struct _tmp218;_tmp218.tag=0U,_tmp218.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg2);_tmp218;});void*_tmp101[1U];_tmp101[0]=& _tmp103;({struct _dyneither_ptr*_tmp2B6=msg1;struct _dyneither_ptr _tmp2B5=({const char*_tmp102="%s";_tag_dyneither(_tmp102,sizeof(char),3U);});Cyc_Tcdecl_merr(0U,_tmp2B6,_tmp2B5,_tag_dyneither(_tmp101,sizeof(void*),1U));});});}struct _tuple25{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 695
static void Cyc_Interface_incl_dict_f(struct _tuple25*env,struct _tuple0*x,void*y1){
# 702
struct _tuple25*_tmp104=env;int*_tmp113;struct Cyc_Dict_Dict _tmp112;int(*_tmp111)(void*,void*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp110;struct _dyneither_ptr*_tmp10F;_LL1: _tmp113=(int*)& _tmp104->f1;_tmp112=_tmp104->f2;_tmp111=_tmp104->f3;_tmp110=_tmp104->f4;_tmp10F=_tmp104->f5;_LL2:;{
struct _handler_cons _tmp105;_push_handler(& _tmp105);{int _tmp107=0;if(setjmp(_tmp105.handler))_tmp107=1;if(!_tmp107){
{void*_tmp108=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp112,x);
if(!_tmp111(y1,_tmp108,_tmp10F))*_tmp113=0;}
# 704
;_pop_handler();}else{void*_tmp106=(void*)Cyc_Core_get_exn_thrown();void*_tmp109=_tmp106;void*_tmp10E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp109)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 708
({struct _dyneither_ptr*_tmp2B9=_tmp10F;Cyc_Interface_check_err(_tmp2B9,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10C=({struct Cyc_String_pa_PrintArg_struct _tmp21A;_tmp21A.tag=0U,_tmp21A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp110);_tmp21A;});struct Cyc_String_pa_PrintArg_struct _tmp10D=({struct Cyc_String_pa_PrintArg_struct _tmp219;_tmp219.tag=0U,({struct _dyneither_ptr _tmp2B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));_tmp219.f1=_tmp2B7;});_tmp219;});void*_tmp10A[2U];_tmp10A[0]=& _tmp10C,_tmp10A[1]=& _tmp10D;({struct _dyneither_ptr _tmp2B8=({const char*_tmp10B="%s %s is missing";_tag_dyneither(_tmp10B,sizeof(char),17U);});Cyc_aprintf(_tmp2B8,_tag_dyneither(_tmp10A,sizeof(void*),2U));});}));});
*_tmp113=0;
goto _LL3;}else{_LL6: _tmp10E=_tmp109;_LL7:(int)_rethrow(_tmp10E);}_LL3:;}};};}
# 714
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 719
struct _tuple25 _tmp114=({struct _tuple25 _tmp21B;_tmp21B.f1=1,_tmp21B.f2=dic2,_tmp21B.f3=incl_f,_tmp21B.f4=t,_tmp21B.f5=msg;_tmp21B;});
((void(*)(void(*f)(struct _tuple25*,struct _tuple0*,void*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp114,dic1);
return _tmp114.f1;}
# 736 "interface.cyc"
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*_tmp115=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0U,msg);if(_tmp115 == 0)return 0;if(d0 != _tmp115){({struct _dyneither_ptr*_tmp2BC=msg;Cyc_Interface_check_err(_tmp2BC,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp118=({struct Cyc_String_pa_PrintArg_struct _tmp21C;_tmp21C.tag=0U,({struct _dyneither_ptr _tmp2BA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp21C.f1=_tmp2BA;});_tmp21C;});void*_tmp116[1U];_tmp116[0]=& _tmp118;({struct _dyneither_ptr _tmp2BB=({const char*_tmp117="declaration of type %s discloses too much information";_tag_dyneither(_tmp117,sizeof(char),54U);});Cyc_aprintf(_tmp2BB,_tag_dyneither(_tmp116,sizeof(void*),1U));});}));});return 0;}return 1;}
# 738
static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmp119=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0U,msg);if(_tmp119 == 0)return 0;if(d0 != _tmp119){({struct _dyneither_ptr*_tmp2BF=msg;Cyc_Interface_check_err(_tmp2BF,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp11C=({struct Cyc_String_pa_PrintArg_struct _tmp21D;_tmp21D.tag=0U,({struct _dyneither_ptr _tmp2BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp21D.f1=_tmp2BD;});_tmp21D;});void*_tmp11A[1U];_tmp11A[0]=& _tmp11C;({struct _dyneither_ptr _tmp2BE=({const char*_tmp11B="declaration of datatype %s discloses too much information";_tag_dyneither(_tmp11B,sizeof(char),58U);});Cyc_aprintf(_tmp2BE,_tag_dyneither(_tmp11A,sizeof(void*),1U));});}));});return 0;}return 1;}
# 740
static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*_tmp11D=Cyc_Tcdecl_merge_enumdecl(d0,d1,0U,msg);if(_tmp11D == 0)return 0;if(d0 != _tmp11D){({struct _dyneither_ptr*_tmp2C2=msg;Cyc_Interface_check_err(_tmp2C2,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp120=({struct Cyc_String_pa_PrintArg_struct _tmp21E;_tmp21E.tag=0U,({struct _dyneither_ptr _tmp2C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp21E.f1=_tmp2C0;});_tmp21E;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _dyneither_ptr _tmp2C1=({const char*_tmp11F="declaration of enum %s discloses too much information";_tag_dyneither(_tmp11F,sizeof(char),54U);});Cyc_aprintf(_tmp2C1,_tag_dyneither(_tmp11E,sizeof(void*),1U));});}));});return 0;}return 1;}
# 742
static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*_tmp121=Cyc_Tcdecl_merge_vardecl(d0,d1,0U,msg);if(_tmp121 == 0)return 0;if(d0 != _tmp121){({struct _dyneither_ptr*_tmp2C5=msg;Cyc_Interface_check_err(_tmp2C5,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp124=({struct Cyc_String_pa_PrintArg_struct _tmp21F;_tmp21F.tag=0U,({struct _dyneither_ptr _tmp2C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp21F.f1=_tmp2C3;});_tmp21F;});void*_tmp122[1U];_tmp122[0]=& _tmp124;({struct _dyneither_ptr _tmp2C4=({const char*_tmp123="declaration of variable %s discloses too much information";_tag_dyneither(_tmp123,sizeof(char),58U);});Cyc_aprintf(_tmp2C4,_tag_dyneither(_tmp122,sizeof(void*),1U));});}));});return 0;}return 1;}
# 744
static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp125=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0U,msg);if(_tmp125 == 0)return 0;if(d0 != _tmp125){({struct _dyneither_ptr*_tmp2C8=msg;Cyc_Interface_check_err(_tmp2C8,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp128=({struct Cyc_String_pa_PrintArg_struct _tmp220;_tmp220.tag=0U,({struct _dyneither_ptr _tmp2C6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));_tmp220.f1=_tmp2C6;});_tmp220;});void*_tmp126[1U];_tmp126[0]=& _tmp128;({struct _dyneither_ptr _tmp2C7=({const char*_tmp127="declaration of typedef %s discloses too much information";_tag_dyneither(_tmp127,sizeof(char),57U);});Cyc_aprintf(_tmp2C7,_tag_dyneither(_tmp126,sizeof(void*),1U));});}));});return 0;}return 1;}
# 746
static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp129=Cyc_Tcdecl_merge_xdatatypefielddecl(d0,d1,0U,msg);if(_tmp129 == 0)return 0;if(d0 != _tmp129){({struct _dyneither_ptr*_tmp2CB=msg;Cyc_Interface_check_err(_tmp2CB,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12C=({struct Cyc_String_pa_PrintArg_struct _tmp221;_tmp221.tag=0U,({struct _dyneither_ptr _tmp2C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name));_tmp221.f1=_tmp2C9;});_tmp221;});void*_tmp12A[1U];_tmp12A[0]=& _tmp12C;({struct _dyneither_ptr _tmp2CA=({const char*_tmp12B="declaration of xdatatypefield %s discloses too much information";_tag_dyneither(_tmp12B,sizeof(char),64U);});Cyc_aprintf(_tmp2CA,_tag_dyneither(_tmp12A,sizeof(void*),1U));});}));});return 0;}return 1;}
# 751
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);
# 760
static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
int _tmp12D=({struct Cyc_Dict_Dict _tmp2CE=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2CD=ie2->aggrdecls;struct _dyneither_ptr _tmp2CC=({const char*_tmp138="type";_tag_dyneither(_tmp138,sizeof(char),5U);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2CE,_tmp2CD,Cyc_Interface_incl_aggrdecl,_tmp2CC,msg);});
int _tmp12E=({struct Cyc_Dict_Dict _tmp2D1=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2D0=ie2->datatypedecls;struct _dyneither_ptr _tmp2CF=({const char*_tmp137="datatype";_tag_dyneither(_tmp137,sizeof(char),9U);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2D1,_tmp2D0,Cyc_Interface_incl_datatypedecl,_tmp2CF,msg);});
int _tmp12F=({struct Cyc_Dict_Dict _tmp2D4=ie1->enumdecls;struct Cyc_Dict_Dict _tmp2D3=ie2->enumdecls;struct _dyneither_ptr _tmp2D2=({const char*_tmp136="enum";_tag_dyneither(_tmp136,sizeof(char),5U);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2D4,_tmp2D3,Cyc_Interface_incl_enumdecl,_tmp2D2,msg);});
# 765
int _tmp130=({struct Cyc_Dict_Dict _tmp2D8=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp2D7=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp2D6=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 765
struct _dyneither_ptr _tmp2D5=({const char*_tmp135="typedef";_tag_dyneither(_tmp135,sizeof(char),8U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2D8,_tmp2D7,_tmp2D6,Cyc_Tcdecl_merge_typedefdecl,_tmp2D5,msg);})!= 0;
# 768
int _tmp131=({struct Cyc_Dict_Dict _tmp2DB=ie1->vardecls;struct Cyc_Dict_Dict _tmp2DA=ie2->vardecls;struct _dyneither_ptr _tmp2D9=({const char*_tmp134="variable";_tag_dyneither(_tmp134,sizeof(char),9U);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2DB,_tmp2DA,Cyc_Interface_incl_vardecl,_tmp2D9,msg);});
int _tmp132=({struct Cyc_Dict_Dict _tmp2DE=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2DD=ie2->xdatatypefielddecls;struct _dyneither_ptr _tmp2DC=({const char*_tmp133="xdatatypefield";_tag_dyneither(_tmp133,sizeof(char),15U);});((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(_tmp2DE,_tmp2DD,Cyc_Interface_incl_xdatatypefielddecl,_tmp2DC,msg);});
return((((_tmp12D && _tmp12E)&& _tmp12F)&& _tmp130)&& _tmp131)&& _tmp132;}
# 773
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp139;_push_handler(& _tmp139);{int _tmp13B=0;if(setjmp(_tmp139.handler))_tmp13B=1;if(!_tmp13B){
{int _tmp13C=1;
struct _dyneither_ptr*msg=0;
# 778
if(info != 0)
msg=({struct _dyneither_ptr*_tmp141=_cycalloc(sizeof(*_tmp141));({struct _dyneither_ptr _tmp2E0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp13F=({struct Cyc_String_pa_PrintArg_struct _tmp223;_tmp223.tag=0U,_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);_tmp223;});struct Cyc_String_pa_PrintArg_struct _tmp140=({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0U,_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);_tmp222;});void*_tmp13D[2U];_tmp13D[0]=& _tmp13F,_tmp13D[1]=& _tmp140;({struct _dyneither_ptr _tmp2DF=({const char*_tmp13E="checking inclusion of %s exports into %s exports,";_tag_dyneither(_tmp13E,sizeof(char),50U);});Cyc_aprintf(_tmp2DF,_tag_dyneither(_tmp13D,sizeof(void*),2U));});});*_tmp141=_tmp2E0;});_tmp141;});
if(!Cyc_Interface_incl_ienv(i1->exports,i2->exports,msg))_tmp13C=0;
# 782
if(info != 0)
msg=({struct _dyneither_ptr*_tmp146=_cycalloc(sizeof(*_tmp146));({struct _dyneither_ptr _tmp2E2=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp144=({struct Cyc_String_pa_PrintArg_struct _tmp225;_tmp225.tag=0U,_tmp225.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);_tmp225;});struct Cyc_String_pa_PrintArg_struct _tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp224;_tmp224.tag=0U,_tmp224.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);_tmp224;});void*_tmp142[2U];_tmp142[0]=& _tmp144,_tmp142[1]=& _tmp145;({struct _dyneither_ptr _tmp2E1=({const char*_tmp143="checking inclusion of %s imports into %s imports,";_tag_dyneither(_tmp143,sizeof(char),50U);});Cyc_aprintf(_tmp2E1,_tag_dyneither(_tmp142,sizeof(void*),2U));});});*_tmp146=_tmp2E2;});_tmp146;});
if(!Cyc_Interface_incl_ienv(i2->imports,i1->imports,msg))_tmp13C=0;{
# 786
int _tmp147=_tmp13C;_npop_handler(0U);return _tmp147;};}
# 775
;_pop_handler();}else{void*_tmp13A=(void*)Cyc_Core_get_exn_thrown();void*_tmp148=_tmp13A;void*_tmp149;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp148)->tag == Cyc_Tcdecl_Incompatible){_LL1: _LL2:
# 788
 return 0;}else{_LL3: _tmp149=_tmp148;_LL4:(int)_rethrow(_tmp149);}_LL0:;}};}struct _tuple26{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;void*(*f5)(void*,void*,unsigned,struct _dyneither_ptr*);struct _dyneither_ptr f6;struct _dyneither_ptr*f7;};
# 796
void Cyc_Interface_compat_merge_dict_f(struct _tuple26*env,struct _tuple0*x,void*y2){
# 803
struct _tuple26*_tmp14A=env;int*_tmp163;struct Cyc_Dict_Dict*_tmp162;struct Cyc_Dict_Dict _tmp161;struct Cyc_Dict_Dict _tmp160;void*(*_tmp15F)(void*,void*,unsigned,struct _dyneither_ptr*);struct _dyneither_ptr _tmp15E;struct _dyneither_ptr*_tmp15D;_LL1: _tmp163=(int*)& _tmp14A->f1;_tmp162=(struct Cyc_Dict_Dict*)& _tmp14A->f2;_tmp161=_tmp14A->f3;_tmp160=_tmp14A->f4;_tmp15F=_tmp14A->f5;_tmp15E=_tmp14A->f6;_tmp15D=_tmp14A->f7;_LL2:;{
void*y;
{struct _handler_cons _tmp14B;_push_handler(& _tmp14B);{int _tmp14D=0;if(setjmp(_tmp14B.handler))_tmp14D=1;if(!_tmp14D){
{void*_tmp14E=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp161,x);
# 809
void*_tmp14F=_tmp15F(_tmp14E,y2,0U,_tmp15D);
if(!((unsigned)_tmp14F)){
*_tmp163=0;
_npop_handler(0U);return;}
# 814
y=_tmp14F;}
# 806
;_pop_handler();}else{void*_tmp14C=(void*)Cyc_Core_get_exn_thrown();void*_tmp150=_tmp14C;void*_tmp151;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp150)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 817
 y=y2;
goto _LL3;}else{_LL6: _tmp151=_tmp150;_LL7:(int)_rethrow(_tmp151);}_LL3:;}};}{
# 821
struct _handler_cons _tmp152;_push_handler(& _tmp152);{int _tmp154=0;if(setjmp(_tmp152.handler))_tmp154=1;if(!_tmp154){
{void*_tmp155=((void*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp160,x);
# 826
void*_tmp156=_tmp15F(_tmp155,y,0U,_tmp15D);
if(_tmp156 != _tmp155){
if((unsigned)_tmp156)
({struct _dyneither_ptr*_tmp2E5=_tmp15D;Cyc_Interface_check_err(_tmp2E5,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp227;_tmp227.tag=0U,_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp15E);_tmp227;});struct Cyc_String_pa_PrintArg_struct _tmp15A=({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0U,({
struct _dyneither_ptr _tmp2E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));_tmp226.f1=_tmp2E3;});_tmp226;});void*_tmp157[2U];_tmp157[0]=& _tmp159,_tmp157[1]=& _tmp15A;({struct _dyneither_ptr _tmp2E4=({const char*_tmp158="abstract %s %s is being imported as non-abstract";_tag_dyneither(_tmp158,sizeof(char),49U);});Cyc_aprintf(_tmp2E4,_tag_dyneither(_tmp157,sizeof(void*),2U));});}));});
*_tmp163=0;}}
# 822
;_pop_handler();}else{void*_tmp153=(void*)Cyc_Core_get_exn_thrown();void*_tmp15B=_tmp153;void*_tmp15C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15B)->tag == Cyc_Dict_Absent){_LL9: _LLA:
# 835
 if(*_tmp163)
({struct Cyc_Dict_Dict _tmp2E6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp162,x,y);*_tmp162=_tmp2E6;});
goto _LL8;}else{_LLB: _tmp15C=_tmp15B;_LLC:(int)_rethrow(_tmp15C);}_LL8:;}};};};}
# 841
struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 849
struct _tuple26 _tmp164=({struct _tuple26 _tmp228;_tmp228.f1=1,_tmp228.f2=dic1,_tmp228.f3=dic1,_tmp228.f4=excl,_tmp228.f5=merge_f,_tmp228.f6=t,_tmp228.f7=msg;_tmp228;});
# 851
((void(*)(void(*f)(struct _tuple26*,struct _tuple0*,void*),struct _tuple26*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,& _tmp164,dic2);
# 853
if(_tmp164.f1)
return({struct Cyc_Dict_Dict*_tmp165=_cycalloc(sizeof(*_tmp165));*_tmp165=_tmp164.f2;_tmp165;});
return 0;}
# 863
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp166=({struct Cyc_Dict_Dict _tmp2EA=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp2E9=ie2->aggrdecls;struct Cyc_Dict_Dict _tmp2E8=iexcl->aggrdecls;struct _dyneither_ptr _tmp2E7=({const char*_tmp172="type";_tag_dyneither(_tmp172,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2EA,_tmp2E9,_tmp2E8,Cyc_Tcdecl_merge_aggrdecl,_tmp2E7,msg);});
struct Cyc_Dict_Dict*_tmp167=({struct Cyc_Dict_Dict _tmp2EE=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp2ED=ie2->datatypedecls;struct Cyc_Dict_Dict _tmp2EC=iexcl->datatypedecls;struct _dyneither_ptr _tmp2EB=({const char*_tmp171="datatype";_tag_dyneither(_tmp171,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2EE,_tmp2ED,_tmp2EC,Cyc_Tcdecl_merge_datatypedecl,_tmp2EB,msg);});
struct Cyc_Dict_Dict*_tmp168=({struct Cyc_Dict_Dict _tmp2F2=ie1->enumdecls;struct Cyc_Dict_Dict _tmp2F1=ie2->enumdecls;struct Cyc_Dict_Dict _tmp2F0=iexcl->enumdecls;struct _dyneither_ptr _tmp2EF=({const char*_tmp170="enum";_tag_dyneither(_tmp170,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2F2,_tmp2F1,_tmp2F0,Cyc_Tcdecl_merge_enumdecl,_tmp2EF,msg);});
struct Cyc_Dict_Dict*_tmp169=({struct Cyc_Dict_Dict _tmp2F6=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp2F5=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp2F4=iexcl->typedefdecls;struct _dyneither_ptr _tmp2F3=({const char*_tmp16F="typedef";_tag_dyneither(_tmp16F,sizeof(char),8U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2F6,_tmp2F5,_tmp2F4,Cyc_Tcdecl_merge_typedefdecl,_tmp2F3,msg);});
struct Cyc_Dict_Dict*_tmp16A=({struct Cyc_Dict_Dict _tmp2FA=ie1->vardecls;struct Cyc_Dict_Dict _tmp2F9=ie2->vardecls;struct Cyc_Dict_Dict _tmp2F8=iexcl->vardecls;struct _dyneither_ptr _tmp2F7=({const char*_tmp16E="variable";_tag_dyneither(_tmp16E,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2FA,_tmp2F9,_tmp2F8,Cyc_Tcdecl_merge_vardecl,_tmp2F7,msg);});
struct Cyc_Dict_Dict*_tmp16B=({struct Cyc_Dict_Dict _tmp2FE=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2FD=ie2->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp2FC=iexcl->xdatatypefielddecls;struct _dyneither_ptr _tmp2FB=({const char*_tmp16D="xdatatypefield";_tag_dyneither(_tmp16D,sizeof(char),15U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp2FE,_tmp2FD,_tmp2FC,Cyc_Tcdecl_merge_xdatatypefielddecl,_tmp2FB,msg);});
if(((((!((unsigned)_tmp166)|| !((unsigned)_tmp167))|| !((unsigned)_tmp168))|| !((unsigned)_tmp169))|| !((unsigned)_tmp16A))|| !((unsigned)_tmp16B))
return 0;
return({struct Cyc_Interface_Ienv*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->aggrdecls=*_tmp166,_tmp16C->datatypedecls=*_tmp167,_tmp16C->enumdecls=*_tmp168,_tmp16C->typedefdecls=*_tmp169,_tmp16C->vardecls=*_tmp16A,_tmp16C->xdatatypefielddecls=*_tmp16B;_tmp16C;});}struct _tuple27{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};
# 876
void Cyc_Interface_disj_merge_dict_f(struct _tuple27*env,struct _tuple0*x,void*y){
# 879
struct _tuple27*_tmp173=env;int*_tmp17C;struct Cyc_Dict_Dict*_tmp17B;struct Cyc_Dict_Dict _tmp17A;struct _dyneither_ptr _tmp179;struct _dyneither_ptr*_tmp178;_LL1: _tmp17C=(int*)& _tmp173->f1;_tmp17B=(struct Cyc_Dict_Dict*)& _tmp173->f2;_tmp17A=_tmp173->f3;_tmp179=_tmp173->f4;_tmp178=_tmp173->f5;_LL2:;
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(_tmp17A,x)){
({struct _dyneither_ptr*_tmp301=_tmp178;Cyc_Interface_check_err(_tmp301,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp176=({struct Cyc_String_pa_PrintArg_struct _tmp22A;_tmp22A.tag=0U,_tmp22A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp179);_tmp22A;});struct Cyc_String_pa_PrintArg_struct _tmp177=({struct Cyc_String_pa_PrintArg_struct _tmp229;_tmp229.tag=0U,({
struct _dyneither_ptr _tmp2FF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));_tmp229.f1=_tmp2FF;});_tmp229;});void*_tmp174[2U];_tmp174[0]=& _tmp176,_tmp174[1]=& _tmp177;({struct _dyneither_ptr _tmp300=({const char*_tmp175="%s %s is exported more than once";_tag_dyneither(_tmp175,sizeof(char),33U);});Cyc_aprintf(_tmp300,_tag_dyneither(_tmp174,sizeof(void*),2U));});}));});
*_tmp17C=0;}else{
# 885
if(*_tmp17C)({struct Cyc_Dict_Dict _tmp302=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,void*v))Cyc_Dict_insert)(*_tmp17B,x,y);*_tmp17B=_tmp302;});}}
# 889
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg){
# 893
struct _tuple27 _tmp17D=({struct _tuple27 _tmp22B;_tmp22B.f1=1,_tmp22B.f2=dic1,_tmp22B.f3=dic1,_tmp22B.f4=t,_tmp22B.f5=msg;_tmp22B;});
((void(*)(void(*f)(struct _tuple27*,struct _tuple0*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp17D,dic2);
if(_tmp17D.f1)
return({struct Cyc_Dict_Dict*_tmp17E=_cycalloc(sizeof(*_tmp17E));*_tmp17E=_tmp17D.f2;_tmp17E;});
return 0;}
# 900
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){
struct Cyc_Dict_Dict*_tmp17F=({struct Cyc_Dict_Dict _tmp306=ie1->aggrdecls;struct Cyc_Dict_Dict _tmp305=ie2->aggrdecls;struct Cyc_Dict_Dict _tmp304=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 901
struct _dyneither_ptr _tmp303=({const char*_tmp18B="type";_tag_dyneither(_tmp18B,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp306,_tmp305,_tmp304,Cyc_Tcdecl_merge_aggrdecl,_tmp303,msg);});
# 904
struct Cyc_Dict_Dict*_tmp180=({struct Cyc_Dict_Dict _tmp309=ie1->datatypedecls;struct Cyc_Dict_Dict _tmp308=ie2->datatypedecls;struct _dyneither_ptr _tmp307=({const char*_tmp18A="datatype";_tag_dyneither(_tmp18A,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp309,_tmp308,_tmp307,msg);});
struct Cyc_Dict_Dict*_tmp181=({struct Cyc_Dict_Dict _tmp30C=ie1->enumdecls;struct Cyc_Dict_Dict _tmp30B=ie2->enumdecls;struct _dyneither_ptr _tmp30A=({const char*_tmp189="enum";_tag_dyneither(_tmp189,sizeof(char),5U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp30C,_tmp30B,_tmp30A,msg);});
# 907
struct Cyc_Dict_Dict*_tmp182=({struct Cyc_Dict_Dict _tmp310=ie1->typedefdecls;struct Cyc_Dict_Dict _tmp30F=ie2->typedefdecls;struct Cyc_Dict_Dict _tmp30E=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
# 907
struct _dyneither_ptr _tmp30D=({const char*_tmp188="typedef";_tag_dyneither(_tmp188,sizeof(char),8U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(_tmp310,_tmp30F,_tmp30E,Cyc_Tcdecl_merge_typedefdecl,_tmp30D,msg);});
# 910
struct Cyc_Dict_Dict*_tmp183=({struct Cyc_Dict_Dict _tmp313=ie1->vardecls;struct Cyc_Dict_Dict _tmp312=ie2->vardecls;struct _dyneither_ptr _tmp311=({const char*_tmp187="variable";_tag_dyneither(_tmp187,sizeof(char),9U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp313,_tmp312,_tmp311,msg);});
struct Cyc_Dict_Dict*_tmp184=({struct Cyc_Dict_Dict _tmp316=ie1->xdatatypefielddecls;struct Cyc_Dict_Dict _tmp315=ie2->xdatatypefielddecls;struct _dyneither_ptr _tmp314=({const char*_tmp186="xdatatypefield";_tag_dyneither(_tmp186,sizeof(char),15U);});((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(_tmp316,_tmp315,_tmp314,msg);});
# 914
if(((((!((unsigned)_tmp17F)|| !((unsigned)_tmp180))|| !((unsigned)_tmp181))|| !((unsigned)_tmp182))|| !((unsigned)_tmp183))|| !((unsigned)_tmp184))
return 0;
return({struct Cyc_Interface_Ienv*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->aggrdecls=*_tmp17F,_tmp185->datatypedecls=*_tmp180,_tmp185->enumdecls=*_tmp181,_tmp185->typedefdecls=*_tmp182,_tmp185->vardecls=*_tmp183,_tmp185->xdatatypefielddecls=*_tmp184;_tmp185;});}
# 920
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){
struct _handler_cons _tmp18C;_push_handler(& _tmp18C);{int _tmp18E=0;if(setjmp(_tmp18C.handler))_tmp18E=1;if(!_tmp18E){
{struct _dyneither_ptr*msg=0;
# 924
if(info != 0)
msg=({struct _dyneither_ptr*_tmp193=_cycalloc(sizeof(*_tmp193));({struct _dyneither_ptr _tmp318=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp191=({struct Cyc_String_pa_PrintArg_struct _tmp22D;_tmp22D.tag=0U,_tmp22D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);_tmp22D;});struct Cyc_String_pa_PrintArg_struct _tmp192=({struct Cyc_String_pa_PrintArg_struct _tmp22C;_tmp22C.tag=0U,_tmp22C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);_tmp22C;});void*_tmp18F[2U];_tmp18F[0]=& _tmp191,_tmp18F[1]=& _tmp192;({struct _dyneither_ptr _tmp317=({const char*_tmp190="merging exports of %s and %s,";_tag_dyneither(_tmp190,sizeof(char),30U);});Cyc_aprintf(_tmp317,_tag_dyneither(_tmp18F,sizeof(void*),2U));});});*_tmp193=_tmp318;});_tmp193;});{
# 928
struct Cyc_Interface_Ienv*_tmp194=Cyc_Interface_disj_merge_ienv(i1->exports,i2->exports,msg);
if(_tmp194 == 0){struct Cyc_Interface_I*_tmp195=0;_npop_handler(0U);return _tmp195;}
# 931
if(info != 0)
msg=({struct _dyneither_ptr*_tmp19A=_cycalloc(sizeof(*_tmp19A));({struct _dyneither_ptr _tmp31A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp198=({struct Cyc_String_pa_PrintArg_struct _tmp22F;_tmp22F.tag=0U,_tmp22F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);_tmp22F;});struct Cyc_String_pa_PrintArg_struct _tmp199=({struct Cyc_String_pa_PrintArg_struct _tmp22E;_tmp22E.tag=0U,_tmp22E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);_tmp22E;});void*_tmp196[2U];_tmp196[0]=& _tmp198,_tmp196[1]=& _tmp199;({struct _dyneither_ptr _tmp319=({const char*_tmp197="merging imports of %s and %s,";_tag_dyneither(_tmp197,sizeof(char),30U);});Cyc_aprintf(_tmp319,_tag_dyneither(_tmp196,sizeof(void*),2U));});});*_tmp19A=_tmp31A;});_tmp19A;});{
# 935
struct Cyc_Interface_Ienv*_tmp19B=Cyc_Interface_compat_merge_ienv(i1->imports,i2->imports,_tmp194,msg);
if(_tmp19B == 0){struct Cyc_Interface_I*_tmp19C=0;_npop_handler(0U);return _tmp19C;}{
# 938
struct Cyc_List_List*newtds=0;struct Cyc_List_List*l=i2->tds;
for(0;l != 0;l=l->tl){
newtds=({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*_tmp19E));({struct Cyc_Absyn_Decl*_tmp31B=({struct Cyc_Absyn_Decl*_tmp19D=_cycalloc(sizeof(*_tmp19D));*_tmp19D=*((struct Cyc_Absyn_Decl*)l->hd);_tmp19D;});_tmp19E->hd=_tmp31B;}),_tmp19E->tl=newtds;_tmp19E;});}
# 942
newtds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Interface_uniqify_decl_list,newtds,i1->tds));{
struct Cyc_Interface_I*_tmp1A0=({struct Cyc_Interface_I*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->imports=_tmp19B,_tmp19F->exports=_tmp194,_tmp19F->tds=newtds;_tmp19F;});_npop_handler(0U);return _tmp1A0;};};};};}
# 922
;_pop_handler();}else{void*_tmp18D=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A1=_tmp18D;void*_tmp1A2;if(((struct Cyc_Tcdecl_Incompatible_exn_struct*)_tmp1A1)->tag == Cyc_Tcdecl_Incompatible){_LL1: _LL2:
# 944
 return 0;}else{_LL3: _tmp1A2=_tmp1A1;_LL4:(int)_rethrow(_tmp1A2);}_LL0:;}};}
# 947
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){
if(li == 0)return Cyc_Interface_empty();{
# 950
struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
li=li->tl;
if(linfo != 0)linfo=linfo->tl;
# 955
for(0;li != 0;li=li->tl){
struct Cyc_Interface_I*_tmp1A3=({struct Cyc_Interface_I*_tmp31D=curr_i;struct Cyc_Interface_I*_tmp31C=(struct Cyc_Interface_I*)li->hd;Cyc_Interface_merge(_tmp31D,_tmp31C,
curr_info != 0 && linfo != 0?({struct _tuple10*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->f1=*curr_info,_tmp1A9->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp1A9;}): 0);});
if(_tmp1A3 == 0)return 0;
curr_i=_tmp1A3;
if(curr_info != 0)
curr_info=linfo != 0?({struct _dyneither_ptr*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));({struct _dyneither_ptr _tmp31F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A6=({struct Cyc_String_pa_PrintArg_struct _tmp231;_tmp231.tag=0U,_tmp231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);_tmp231;});struct Cyc_String_pa_PrintArg_struct _tmp1A7=({struct Cyc_String_pa_PrintArg_struct _tmp230;_tmp230.tag=0U,_tmp230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));_tmp230;});void*_tmp1A4[2U];_tmp1A4[0]=& _tmp1A6,_tmp1A4[1]=& _tmp1A7;({struct _dyneither_ptr _tmp31E=({const char*_tmp1A5="%s+%s";_tag_dyneither(_tmp1A5,sizeof(char),6U);});Cyc_aprintf(_tmp31E,_tag_dyneither(_tmp1A4,sizeof(void*),2U));});});*_tmp1A8=_tmp31F;});_tmp1A8;}): 0;
# 963
if(linfo != 0)linfo=linfo->tl;}
# 965
return curr_i;};}
# 968
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo){
if(la == 0)return Cyc_Interface_empty();{
# 971
struct Cyc_Interface_I*curr_i=get(la->hd);
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)linfo->hd: 0;
la=la->tl;
if(linfo != 0)linfo=linfo->tl;
# 976
for(0;la != 0;la=la->tl){
struct Cyc_Interface_I*_tmp1AA=({struct Cyc_Interface_I*_tmp321=curr_i;struct Cyc_Interface_I*_tmp320=get(la->hd);Cyc_Interface_merge(_tmp321,_tmp320,
curr_info != 0 && linfo != 0?({struct _tuple10*_tmp1B0=_cycalloc(sizeof(*_tmp1B0));_tmp1B0->f1=*curr_info,_tmp1B0->f2=*((struct _dyneither_ptr*)linfo->hd);_tmp1B0;}): 0);});
if(_tmp1AA == 0)return 0;
curr_i=_tmp1AA;
if(curr_info != 0)
curr_info=linfo != 0?({struct _dyneither_ptr*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));({struct _dyneither_ptr _tmp323=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1AD=({struct Cyc_String_pa_PrintArg_struct _tmp233;_tmp233.tag=0U,_tmp233.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*curr_info);_tmp233;});struct Cyc_String_pa_PrintArg_struct _tmp1AE=({struct Cyc_String_pa_PrintArg_struct _tmp232;_tmp232.tag=0U,_tmp232.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));_tmp232;});void*_tmp1AB[2U];_tmp1AB[0]=& _tmp1AD,_tmp1AB[1]=& _tmp1AE;({struct _dyneither_ptr _tmp322=({const char*_tmp1AC="%s+%s";_tag_dyneither(_tmp1AC,sizeof(char),6U);});Cyc_aprintf(_tmp322,_tag_dyneither(_tmp1AB,sizeof(void*),2U));});});*_tmp1AF=_tmp323;});_tmp1AF;}): 0;
# 984
if(linfo != 0)linfo=linfo->tl;}
# 986
return curr_i;};}
# 992
static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*tds){
struct Cyc_List_List*_tmp1B1=0;
for(0;tds != 0;tds=tds->tl){
struct _tuple0*qv;
struct Cyc_Absyn_Decl*_tmp1B2=(struct Cyc_Absyn_Decl*)tds->hd;
{void*_tmp1B3=_tmp1B2->r;void*_tmp1B4=_tmp1B3;struct Cyc_Absyn_Typedefdecl*_tmp1BB;struct Cyc_Absyn_Enumdecl*_tmp1BA;struct Cyc_Absyn_Datatypedecl*_tmp1B9;struct Cyc_Absyn_Aggrdecl*_tmp1B8;struct Cyc_Absyn_Fndecl*_tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B6;switch(*((int*)_tmp1B4)){case 0U: _LL1: _tmp1B6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1B4)->f1;_LL2:
 qv=_tmp1B6->name;goto _LL0;case 1U: _LL3: _tmp1B7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1B4)->f1;_LL4:
 qv=_tmp1B7->name;goto _LL0;case 5U: _LL5: _tmp1B8=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1B4)->f1;_LL6:
 qv=_tmp1B8->name;goto _LL0;case 6U: _LL7: _tmp1B9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1B4)->f1;_LL8:
 qv=_tmp1B9->name;goto _LL0;case 7U: _LL9: _tmp1BA=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1B4)->f1;_LLA:
 qv=_tmp1BA->name;goto _LL0;case 8U: _LLB: _tmp1BB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1B4)->f1;_LLC:
 qv=_tmp1BB->name;goto _LL0;default: _LLD: _LLE:
 Cyc_Interface_err(({const char*_tmp1B5="bad decl form in Interface::add_namespace";_tag_dyneither(_tmp1B5,sizeof(char),42U);}));return 0;}_LL0:;}{
# 1006
struct Cyc_List_List*vs;
{union Cyc_Absyn_Nmspace _tmp1BC=(*qv).f1;union Cyc_Absyn_Nmspace _tmp1BD=_tmp1BC;struct Cyc_List_List*_tmp1C0;struct Cyc_List_List*_tmp1BF;switch((_tmp1BD.C_n).tag){case 2U: _LL10: _tmp1BF=(_tmp1BD.Abs_n).val;_LL11:
 vs=_tmp1BF;goto _LLF;case 3U: _LL12: _tmp1C0=(_tmp1BD.C_n).val;_LL13:
 vs=_tmp1C0;goto _LLF;default: _LL14: _LL15:
 Cyc_Interface_err(({const char*_tmp1BE="bad namespace in Interface::add_namespace";_tag_dyneither(_tmp1BE,sizeof(char),42U);}));return 0;}_LLF:;}
# 1012
vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);
({union Cyc_Absyn_Nmspace _tmp324=Cyc_Absyn_Rel_n(0);(*qv).f1=_tmp324;});
for(0;vs != 0;vs=vs->tl){
_tmp1B2=({void*_tmp326=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->tag=9U,_tmp1C2->f1=(struct _dyneither_ptr*)vs->hd,({struct Cyc_List_List*_tmp325=({struct Cyc_List_List*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=_tmp1B2,_tmp1C1->tl=0;_tmp1C1;});_tmp1C2->f2=_tmp325;});_tmp1C2;});Cyc_Absyn_new_decl(_tmp326,_tmp1B2->loc);});}
_tmp1B1=({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=_tmp1B2,_tmp1C3->tl=_tmp1B1;_tmp1C3;});};}
# 1018
_tmp1B1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1B1);
return _tmp1B1;}
# 1022
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));({struct Cyc_Absyn_Decl*_tmp327=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->tag=5U,_tmp1C4->f1=d;_tmp1C4;}),0U);_tmp1C5->hd=_tmp327;}),_tmp1C5->tl=tds;_tmp1C5;});}
# 1026
static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _tuple0*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
# 1028
d=({struct Cyc_Absyn_Aggrdecl*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));*_tmp1C6=*d;_tmp1C6;});
d->impl=0;
if((int)d->sc != (int)Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));({struct Cyc_Absyn_Decl*_tmp328=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->tag=5U,_tmp1C7->f1=d;_tmp1C7;}),0U);_tmp1C8->hd=_tmp328;}),_tmp1C8->tl=tds;_tmp1C8;});}
# 1034
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));({struct Cyc_Absyn_Decl*_tmp329=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));_tmp1C9->tag=6U,_tmp1C9->f1=d;_tmp1C9;}),0U);_tmp1CA->hd=_tmp329;}),_tmp1CA->tl=tds;_tmp1CA;});}static char _tmp1CB[2U]="_";
# 1038
static struct _dyneither_ptr Cyc_Interface_us={_tmp1CB,_tmp1CB,_tmp1CB + 2U};
static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
# 1041
static struct _tuple20*Cyc_Interface_rewrite_datatypefield_type(struct _tuple20*x){
return({struct _tuple20*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));({struct Cyc_Absyn_Tqual _tmp32D=Cyc_Absyn_empty_tqual(0U);_tmp1CE->f1=_tmp32D;}),({
void*_tmp32C=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->tag=2U,({struct Cyc_Absyn_Tvar*_tmp32B=({struct Cyc_Absyn_Tvar*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->name=Cyc_Interface_us_p,_tmp1CC->identity=- 1,({void*_tmp32A=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp1CC->kind=_tmp32A;});_tmp1CC;});_tmp1CD->f1=_tmp32B;});_tmp1CD;});_tmp1CE->f2=_tmp32C;});_tmp1CE;});}
# 1046
static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){
f=({struct Cyc_Absyn_Datatypefield*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));*_tmp1CF=*f;_tmp1CF;});
({struct Cyc_List_List*_tmp32E=((struct Cyc_List_List*(*)(struct _tuple20*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);f->typs=_tmp32E;});
return f;}
# 1052
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _tuple0*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){
# 1054
d=({struct Cyc_Absyn_Datatypedecl*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));*_tmp1D0=*d;_tmp1D0;});
# 1056
if(d->fields != 0)({struct Cyc_Core_Opt*_tmp330=({struct Cyc_Core_Opt*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));({struct Cyc_List_List*_tmp32F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(struct Cyc_List_List*)(d->fields)->v);_tmp1D1->v=_tmp32F;});_tmp1D1;});d->fields=_tmp330;});
if((int)d->sc != (int)Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));({struct Cyc_Absyn_Decl*_tmp331=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->tag=6U,_tmp1D2->f1=d;_tmp1D2;}),0U);_tmp1D3->hd=_tmp331;}),_tmp1D3->tl=tds;_tmp1D3;});}
# 1061
static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));({struct Cyc_Absyn_Decl*_tmp332=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->tag=7U,_tmp1D4->f1=d;_tmp1D4;}),0U);_tmp1D5->hd=_tmp332;}),_tmp1D5->tl=tds;_tmp1D5;});}
# 1065
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _tuple0*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){
# 1067
d=({struct Cyc_Absyn_Enumdecl*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));*_tmp1D6=*d;_tmp1D6;});
d->fields=0;
if((int)d->sc != (int)Cyc_Absyn_ExternC)d->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));({struct Cyc_Absyn_Decl*_tmp333=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->tag=7U,_tmp1D7->f1=d;_tmp1D7;}),0U);_tmp1D8->hd=_tmp333;}),_tmp1D8->tl=tds;_tmp1D8;});}
# 1073
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _tuple0*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
# 1075
return({struct Cyc_List_List*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));({struct Cyc_Absyn_Decl*_tmp334=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->tag=8U,_tmp1D9->f1=d;_tmp1D9;}),0U);_tmp1DA->hd=_tmp334;}),_tmp1DA->tl=tds;_tmp1DA;});}
# 1078
static struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _tuple0*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));({struct Cyc_Absyn_Decl*_tmp335=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->tag=0U,_tmp1DB->f1=d;_tmp1DB;}),0U);_tmp1DC->hd=_tmp335;}),_tmp1DC->tl=tds;_tmp1DC;});}
# 1082
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1084
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1DD=d;struct Cyc_Absyn_Datatypedecl*_tmp1E4;struct Cyc_Absyn_Datatypefield*_tmp1E3;_LL1: _tmp1E4=_tmp1DD->base;_tmp1E3=_tmp1DD->field;_LL2:;
_tmp1E4=({struct Cyc_Absyn_Datatypedecl*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));*_tmp1DE=*_tmp1E4;_tmp1DE;});
({struct Cyc_Core_Opt*_tmp337=({struct Cyc_Core_Opt*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));({struct Cyc_List_List*_tmp336=({struct Cyc_List_List*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->hd=_tmp1E3,_tmp1DF->tl=0;_tmp1DF;});_tmp1E0->v=_tmp336;});_tmp1E0;});_tmp1E4->fields=_tmp337;});
_tmp1E4->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));({struct Cyc_Absyn_Decl*_tmp338=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->tag=6U,_tmp1E1->f1=_tmp1E4;_tmp1E1;}),0U);_tmp1E2->hd=_tmp338;}),_tmp1E2->tl=tds;_tmp1E2;});}
# 1091
static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct _tuple0*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
# 1093
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp1E5=d;struct Cyc_Absyn_Datatypedecl*_tmp1EC;struct Cyc_Absyn_Datatypefield*_tmp1EB;_LL1: _tmp1EC=_tmp1E5->base;_tmp1EB=_tmp1E5->field;_LL2:;
_tmp1EC=({struct Cyc_Absyn_Datatypedecl*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));*_tmp1E6=*_tmp1EC;_tmp1E6;});
_tmp1EB=Cyc_Interface_rewrite_datatypefield(_tmp1EB);
_tmp1EB->sc=Cyc_Absyn_Extern;
({struct Cyc_Core_Opt*_tmp33A=({struct Cyc_Core_Opt*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));({struct Cyc_List_List*_tmp339=({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=_tmp1EB,_tmp1E7->tl=0;_tmp1E7;});_tmp1E8->v=_tmp339;});_tmp1E8;});_tmp1EC->fields=_tmp33A;});
_tmp1EC->sc=Cyc_Absyn_Extern;
return({struct Cyc_List_List*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));({struct Cyc_Absyn_Decl*_tmp33B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->tag=6U,_tmp1E9->f1=_tmp1EC;_tmp1E9;}),0U);_tmp1EA->hd=_tmp33B;}),_tmp1EA->tl=tds;_tmp1EA;});}
# 1102
static void Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1ED=0;
_tmp1ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp1ED);
_tmp1ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp1ED);
_tmp1ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp1ED);
# 1108
if(_tmp1ED != 0){
_tmp1ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1ED);
_tmp1ED=Cyc_Interface_add_namespace(_tmp1ED);
Cyc_Absynpp_decllist2file(_tmp1ED,f);}}
# 1115
static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);
if(_tmp1EE != 0){
_tmp1EE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1EE);
_tmp1EE=Cyc_Interface_add_namespace(_tmp1EE);
Cyc_Absynpp_decllist2file(_tmp1EE,f);}}
# 1124
static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,0);
if(_tmp1EF != 0){
_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1EF);
_tmp1EF=Cyc_Interface_add_namespace(_tmp1EF);
Cyc_Absynpp_decllist2file(_tmp1EF,f);}}
# 1133
static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1F0=0;
_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp1F0);
_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp1F0);
_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp1F0);
_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,_tmp1F0);
_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tuple0*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp1F0);
# 1141
if(_tmp1F0 != 0){
_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1F0);
_tmp1F0=Cyc_Interface_add_namespace(_tmp1F0);
Cyc_Absynpp_decllist2file(_tmp1F0,f);}}
# 1151
void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);
({void*_tmp1F1=0U;({struct Cyc___cycFILE*_tmp33D=f;struct _dyneither_ptr _tmp33C=({const char*_tmp1F2="/****** needed (headers) ******/\n";_tag_dyneither(_tmp1F2,sizeof(char),34U);});Cyc_fprintf(_tmp33D,_tmp33C,_tag_dyneither(_tmp1F1,sizeof(void*),0U));});});
Cyc_Interface_print_ns_headers(f,i->imports);
# 1156
({void*_tmp1F3=0U;({struct Cyc___cycFILE*_tmp33F=f;struct _dyneither_ptr _tmp33E=({const char*_tmp1F4="\n/****** provided (headers) ******/\n";_tag_dyneither(_tmp1F4,sizeof(char),37U);});Cyc_fprintf(_tmp33F,_tmp33E,_tag_dyneither(_tmp1F3,sizeof(void*),0U));});});
Cyc_Interface_print_ns_headers(f,i->exports);
# 1161
({void*_tmp1F5=0U;({struct Cyc___cycFILE*_tmp341=f;struct _dyneither_ptr _tmp340=({const char*_tmp1F6="\n/****** needed (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp1F6,sizeof(char),58U);});Cyc_fprintf(_tmp341,_tmp340,_tag_dyneither(_tmp1F5,sizeof(void*),0U));});});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->imports);
# 1164
({void*_tmp1F7=0U;({struct Cyc___cycFILE*_tmp343=f;struct _dyneither_ptr _tmp342=({const char*_tmp1F8="\n/****** provided (headers of xdatatypefielddecls) ******/\n";_tag_dyneither(_tmp1F8,sizeof(char),60U);});Cyc_fprintf(_tmp343,_tmp342,_tag_dyneither(_tmp1F7,sizeof(void*),0U));});});
Cyc_Interface_print_ns_xdatatypefielddecl_headers(f,i->exports);
# 1168
({void*_tmp1F9=0U;({struct Cyc___cycFILE*_tmp345=f;struct _dyneither_ptr _tmp344=({const char*_tmp1FA="\n/****** provided (typedefs) ******/\n";_tag_dyneither(_tmp1FA,sizeof(char),38U);});Cyc_fprintf(_tmp345,_tmp344,_tag_dyneither(_tmp1F9,sizeof(void*),0U));});});
Cyc_Interface_print_ns_typedefs(f,i->exports);
# 1171
({void*_tmp1FB=0U;({struct Cyc___cycFILE*_tmp347=f;struct _dyneither_ptr _tmp346=({const char*_tmp1FC="\n/****** needed (declarations) ******/\n";_tag_dyneither(_tmp1FC,sizeof(char),40U);});Cyc_fprintf(_tmp347,_tmp346,_tag_dyneither(_tmp1FB,sizeof(void*),0U));});});
Cyc_Interface_print_ns_decls(f,i->imports);
# 1174
({void*_tmp1FD=0U;({struct Cyc___cycFILE*_tmp349=f;struct _dyneither_ptr _tmp348=({const char*_tmp1FE="\n/****** provided (declarations) ******/\n";_tag_dyneither(_tmp1FE,sizeof(char),42U);});Cyc_fprintf(_tmp349,_tmp348,_tag_dyneither(_tmp1FD,sizeof(void*),0U));});});
Cyc_Interface_print_ns_decls(f,i->exports);}
# 1181
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp1FF=Cyc_Parse_parse_file(f);
Cyc_Binding_resolve_all(_tmp1FF);{
struct Cyc_Tcenv_Tenv*_tmp200=Cyc_Tcenv_tc_init();
Cyc_Tc_tc(_tmp200,0,_tmp1FF);
return Cyc_Interface_gen_extract(_tmp200->ae,0,_tmp1FF);};};}
# 1194
void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){
Cyc_Interface_print(i,f);}
# 1200
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){
return Cyc_Interface_parse(f);}
