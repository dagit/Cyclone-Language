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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned);
# 99
struct _dyneither_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 305 "core.h"
struct _dyneither_ptr Cyc_Core_mkfat(void*arr,unsigned s,unsigned n);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 310 "cycboot.h"
int toupper(int);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 63
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
# 65
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*,struct Cyc_List_List*);
# 67
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);
# 72
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 100 "string.h"
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*,struct _dyneither_ptr s,unsigned long sz);
# 102
struct _dyneither_ptr Cyc_cond_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz);
# 105
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*,struct _dyneither_ptr src);
# 110
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*,struct _dyneither_ptr,int ofs,unsigned long n);
# 115
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix);
# 120
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c);
# 127
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept);
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept);
# 135
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*,struct _dyneither_ptr s);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line);
# 39 "string.cyc"
unsigned long Cyc_strlen(struct _dyneither_ptr s){
unsigned long i;
unsigned sz=_get_dyneither_size(s,sizeof(char));
for(i=0U;i < sz;++ i){
if((int)((const char*)s.curr)[(int)i]== (int)'\000')
return i;}
# 46
return i;}
# 52
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned sz1=_get_dyneither_size(s1,sizeof(char));
unsigned sz2=_get_dyneither_size(s2,sizeof(char));
unsigned minsz=sz1 < sz2?sz1: sz2;
minsz <= _get_dyneither_size(s1,sizeof(char))&& minsz <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmp80=({const char*_tmp0="minsz <= numelts(s1) && minsz <= numelts(s2)";_tag_dyneither(_tmp0,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp80,({const char*_tmp1="string.cyc";_tag_dyneither(_tmp1,sizeof(char),11U);}),59U);});
while((unsigned)i < minsz){
char c1=((const char*)s1.curr)[i];
char c2=((const char*)s2.curr)[i];
if((int)c1 == (int)'\000'){
if((int)c2 == (int)'\000')return 0;else{
return - 1;}}else{
if((int)c2 == (int)'\000')return 1;else{
# 68
int diff=(int)c1 - (int)c2;
if(diff != 0)return diff;}}
# 71
++ i;}
# 73
if(sz1 == sz2)return 0;
if(minsz < sz2){
if((int)*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== (int)'\000')return 0;else{
return - 1;}}else{
# 78
if((int)*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== (int)'\000')return 0;else{
return 1;}}};}
# 83
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2){
return Cyc_strcmp((struct _dyneither_ptr)*s1,(struct _dyneither_ptr)*s2);}
# 87
inline static int Cyc_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 90
if(n <= (unsigned long)0)return 0;{
# 92
unsigned long min_len=len1 > len2?len2: len1;
unsigned long bound=min_len > n?n: min_len;
# 95
bound <= _get_dyneither_size(s1,sizeof(char))&& bound <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmp81=({const char*_tmp2="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp2,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp81,({const char*_tmp3="string.cyc";_tag_dyneither(_tmp3,sizeof(char),11U);}),95U);});
# 97
{int i=0;for(0;(unsigned long)i < bound;++ i){
int retc;
if((retc=(int)((const char*)s1.curr)[i]- (int)((const char*)s2.curr)[i])!= 0)
return retc;}}
# 102
if(len1 < n || len2 < n)
return(int)len1 - (int)len2;
return 0;};}
# 109
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_ncmp(s1,len1,s2,len2,n);}
# 119
int Cyc_zstrcmp(struct _dyneither_ptr a,struct _dyneither_ptr b){
if((char*)a.curr == (char*)b.curr)
return 0;{
unsigned long as=_get_dyneither_size(a,sizeof(char));
unsigned long bs=_get_dyneither_size(b,sizeof(char));
unsigned long min_length=as < bs?as: bs;
int i=-1;
# 127
min_length <= _get_dyneither_size(a,sizeof(char))&& min_length <= _get_dyneither_size(b,sizeof(char))?0:({struct _dyneither_ptr _tmp82=({const char*_tmp4="min_length <= numelts(a) && min_length <= numelts(b)";_tag_dyneither(_tmp4,sizeof(char),53U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp82,({const char*_tmp5="string.cyc";_tag_dyneither(_tmp5,sizeof(char),11U);}),127U);});
# 129
while((++ i,(unsigned long)i < min_length)){
int diff=(int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];
if(diff != 0)
return diff;}
# 134
return(int)as - (int)bs;};}
# 137
int Cyc_zstrncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
if(n <= (unsigned long)0)return 0;{
# 140
unsigned long s1size=_get_dyneither_size(s1,sizeof(char));
unsigned long s2size=_get_dyneither_size(s2,sizeof(char));
unsigned long min_size=s1size > s2size?s2size: s1size;
unsigned long bound=min_size > n?n: min_size;
# 145
bound <= _get_dyneither_size(s1,sizeof(char))&& bound <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmp83=({const char*_tmp6="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp6,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp83,({const char*_tmp7="string.cyc";_tag_dyneither(_tmp7,sizeof(char),11U);}),145U);});
# 147
{int i=0;for(0;(unsigned long)i < bound;++ i){
if((int)((const char*)s1.curr)[i]< (int)((const char*)s2.curr)[i])
return - 1;else{
if((int)((const char*)s2.curr)[i]< (int)((const char*)s1.curr)[i])
return 1;}}}
# 153
if(min_size <= bound)
return 0;
if(s1size < s2size)
return - 1;else{
# 158
return 1;}};}
# 162
int Cyc_zstrptrcmp(struct _dyneither_ptr*a,struct _dyneither_ptr*b){
if((int)a == (int)b)return 0;
return Cyc_zstrcmp((struct _dyneither_ptr)*a,(struct _dyneither_ptr)*b);}
# 172
inline static struct _dyneither_ptr Cyc_int_strcato(struct _dyneither_ptr dest,struct _dyneither_ptr src){
int i;
unsigned long dsize;unsigned long slen;unsigned long dlen;
# 176
dsize=_get_dyneither_size(dest,sizeof(char));
dlen=Cyc_strlen((struct _dyneither_ptr)dest);
slen=Cyc_strlen(src);
# 180
if(slen + dlen <= dsize){
slen <= _get_dyneither_size(src,sizeof(char))?0:({struct _dyneither_ptr _tmp84=({const char*_tmp8="slen <= numelts(src)";_tag_dyneither(_tmp8,sizeof(char),21U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp84,({const char*_tmp9="string.cyc";_tag_dyneither(_tmp9,sizeof(char),11U);}),181U);});
for(i=0;(unsigned long)i < slen;++ i){
({struct _dyneither_ptr _tmpA=_dyneither_ptr_plus(dest,sizeof(char),(int)((unsigned long)i + dlen));char _tmpB=*((char*)_check_dyneither_subscript(_tmpA,sizeof(char),0U));char _tmpC=((const char*)src.curr)[i];if(_get_dyneither_size(_tmpA,sizeof(char))== 1U &&(_tmpB == 0 && _tmpC != 0))_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}
# 185
if((unsigned long)i + dlen < dsize)
({struct _dyneither_ptr _tmpD=_dyneither_ptr_plus(dest,sizeof(char),(int)((unsigned long)i + dlen));char _tmpE=*((char*)_check_dyneither_subscript(_tmpD,sizeof(char),0U));char _tmpF='\000';if(_get_dyneither_size(_tmpD,sizeof(char))== 1U &&(_tmpE == 0 && _tmpF != 0))_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});}else{
# 189
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp85=({const char*_tmp10="strcat";_tag_dyneither(_tmp10,sizeof(char),7U);});_tmp11->f1=_tmp85;});_tmp11;}));}
return dest;}
# 195
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src){
return Cyc_int_strcato(dest,src);}
# 200
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*r,struct _dyneither_ptr a,struct _dyneither_ptr b){
unsigned long _tmp12=Cyc_strlen(a);
unsigned long _tmp13=Cyc_strlen(b);
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,(_tmp12 + _tmp13)+ (unsigned long)1);
int i;int j;
_tmp12 <= _get_dyneither_size(ans,sizeof(char))&& _tmp12 <= _get_dyneither_size(a,sizeof(char))?0:({struct _dyneither_ptr _tmp86=({const char*_tmp14="alen <= numelts(ans) && alen <= numelts(a)";_tag_dyneither(_tmp14,sizeof(char),43U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp86,({const char*_tmp15="string.cyc";_tag_dyneither(_tmp15,sizeof(char),11U);}),205U);});
for(i=0;(unsigned long)i < _tmp12;++ i){({struct _dyneither_ptr _tmp16=_dyneither_ptr_plus(ans,sizeof(char),i);char _tmp17=*((char*)_check_dyneither_subscript(_tmp16,sizeof(char),0U));char _tmp18=((const char*)a.curr)[i];if(_get_dyneither_size(_tmp16,sizeof(char))== 1U &&(_tmp17 == 0 && _tmp18 != 0))_throw_arraybounds();*((char*)_tmp16.curr)=_tmp18;});}
_tmp13 <= _get_dyneither_size(b,sizeof(char))?0:({struct _dyneither_ptr _tmp87=({const char*_tmp19="blen <= numelts(b)";_tag_dyneither(_tmp19,sizeof(char),19U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp87,({const char*_tmp1A="string.cyc";_tag_dyneither(_tmp1A,sizeof(char),11U);}),207U);});
for(j=0;(unsigned long)j < _tmp13;++ j){({struct _dyneither_ptr _tmp1B=_dyneither_ptr_plus(ans,sizeof(char),i + j);char _tmp1C=*((char*)_check_dyneither_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=((const char*)b.curr)[j];if(_get_dyneither_size(_tmp1B,sizeof(char))== 1U &&(_tmp1C == 0 && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});}
return ans;}
# 212
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr a,struct _dyneither_ptr b){
return Cyc_rstrconcat(Cyc_Core_heap_region,a,b);}
# 217
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*r,struct Cyc_List_List*strs){
# 219
unsigned long len;
unsigned long total_len=0U;
struct _dyneither_ptr ans;
struct _RegionHandle _tmp1E=_new_region("temp");struct _RegionHandle*temp=& _tmp1E;_push_region(temp);{
struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp21=_region_malloc(temp,sizeof(*_tmp21));
_tmp21->hd=(void*)0U,_tmp21->tl=0;_tmp21;});
# 226
struct Cyc_List_List*end=lens;
{struct Cyc_List_List*p=strs;for(0;p != 0;p=p->tl){
len=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)p->hd));
total_len +=len;
({struct Cyc_List_List*_tmp88=({struct Cyc_List_List*_tmp1F=_region_malloc(temp,sizeof(*_tmp1F));_tmp1F->hd=(void*)len,_tmp1F->tl=0;_tmp1F;});((struct Cyc_List_List*)_check_null(end))->tl=_tmp88;});
end=end->tl;}}
# 233
lens=lens->tl;
ans=Cyc_Core_rnew_string(r,total_len + (unsigned long)1);{
unsigned long i=0U;
while(strs != 0){
struct _dyneither_ptr _tmp20=*((struct _dyneither_ptr*)strs->hd);
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
({struct _dyneither_ptr _tmp8A=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp89=(struct _dyneither_ptr)_tmp20;Cyc_strncpy(_tmp8A,_tmp89,len);});
i +=len;
strs=strs->tl;
lens=lens->tl;}};}{
# 245
struct _dyneither_ptr _tmp22=ans;_npop_handler(0U);return _tmp22;};
# 222
;_pop_region(temp);}
# 248
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*strs){
return Cyc_rstrconcat_l(Cyc_Core_heap_region,strs);}
# 254
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*r,struct Cyc_List_List*strs,struct _dyneither_ptr separator){
if(strs == 0)return Cyc_Core_rnew_string(r,0U);
if(strs->tl == 0)return Cyc_rstrdup(r,(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd));{
struct Cyc_List_List*_tmp23=strs;
struct _RegionHandle _tmp24=_new_region("temp");struct _RegionHandle*temp=& _tmp24;_push_region(temp);
{struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp28=_region_malloc(temp,sizeof(*_tmp28));_tmp28->hd=(void*)0U,_tmp28->tl=0;_tmp28;});
struct Cyc_List_List*end=lens;
unsigned long len;
unsigned long total_len=0U;
unsigned long list_len=0U;
for(0;_tmp23 != 0;_tmp23=_tmp23->tl){
len=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp23->hd));
total_len +=len;
({struct Cyc_List_List*_tmp8B=({struct Cyc_List_List*_tmp25=_region_malloc(temp,sizeof(*_tmp25));_tmp25->hd=(void*)len,_tmp25->tl=0;_tmp25;});((struct Cyc_List_List*)_check_null(end))->tl=_tmp8B;});
end=end->tl;
++ list_len;}
# 271
lens=lens->tl;{
unsigned long seplen=Cyc_strlen(separator);
total_len +=(list_len - (unsigned long)1)* seplen;{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,total_len + (unsigned long)1);
unsigned long i=0U;
while(((struct Cyc_List_List*)_check_null(strs))->tl != 0){
struct _dyneither_ptr _tmp26=*((struct _dyneither_ptr*)strs->hd);
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
({struct _dyneither_ptr _tmp8D=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp8C=(struct _dyneither_ptr)_tmp26;Cyc_strncpy(_tmp8D,_tmp8C,len);});
i +=len;
({struct _dyneither_ptr _tmp8F=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp8E=separator;Cyc_strncpy(_tmp8F,_tmp8E,seplen);});
i +=seplen;
strs=strs->tl;
lens=lens->tl;}
# 286
({struct _dyneither_ptr _tmp91=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _dyneither_ptr _tmp90=(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd);Cyc_strncpy(_tmp91,_tmp90,(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd);});{
struct _dyneither_ptr _tmp27=ans;_npop_handler(0U);return _tmp27;};};};}
# 259
;_pop_region(temp);};}
# 291
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*strs,struct _dyneither_ptr separator){
return Cyc_rstr_sepstr(Cyc_Core_heap_region,strs,separator);}
# 296
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
int i;
n <= _get_dyneither_size(src,sizeof(char))&& n <= _get_dyneither_size(dest,sizeof(char))?0:({struct _dyneither_ptr _tmp92=({const char*_tmp29="n <= numelts(src) && n <= numelts(dest)";_tag_dyneither(_tmp29,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp92,({const char*_tmp2A="string.cyc";_tag_dyneither(_tmp2A,sizeof(char),11U);}),298U);});
for(i=0;(unsigned long)i < n;++ i){
char _tmp2B=((const char*)src.curr)[i];
if((int)_tmp2B == (int)'\000')break;
((char*)dest.curr)[i]=_tmp2B;}
# 304
for(0;(unsigned long)i < n;++ i){
((char*)dest.curr)[i]='\000';}
# 307
return dest;}
# 311
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
n <= _get_dyneither_size(dest,sizeof(char))&& n <= _get_dyneither_size(src,sizeof(char))?0:({struct _dyneither_ptr _tmp93=({const char*_tmp2C="n <= numelts(dest) && n <= numelts(src)";_tag_dyneither(_tmp2C,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp93,({const char*_tmp2D="string.cyc";_tag_dyneither(_tmp2D,sizeof(char),11U);}),312U);});{
int i;
for(i=0;(unsigned long)i < n;++ i){
((char*)dest.curr)[i]=((const char*)src.curr)[i];}
return dest;};}
# 319
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src){
unsigned ssz=_get_dyneither_size(src,sizeof(char));
unsigned dsz=_get_dyneither_size(dest,sizeof(char));
if(ssz <= dsz){
unsigned i;
for(i=0U;i < ssz;++ i){
char _tmp2E=((const char*)src.curr)[(int)i];
({struct _dyneither_ptr _tmp2F=_dyneither_ptr_plus(dest,sizeof(char),(int)i);char _tmp30=*((char*)_check_dyneither_subscript(_tmp2F,sizeof(char),0U));char _tmp31=_tmp2E;if(_get_dyneither_size(_tmp2F,sizeof(char))== 1U &&(_tmp30 == 0 && _tmp31 != 0))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});
if((int)_tmp2E == (int)'\000')break;}}else{
# 331
unsigned long len=Cyc_strlen(src);
({struct _dyneither_ptr _tmp95=_dyneither_ptr_decrease_size(dest,sizeof(char),1U);struct _dyneither_ptr _tmp94=src;Cyc_strncpy(_tmp95,_tmp94,len);});
if(len < _get_dyneither_size(dest,sizeof(char)))
({struct _dyneither_ptr _tmp32=_dyneither_ptr_plus(dest,sizeof(char),(int)len);char _tmp33=*((char*)_check_dyneither_subscript(_tmp32,sizeof(char),0U));char _tmp34='\000';if(_get_dyneither_size(_tmp32,sizeof(char))== 1U &&(_tmp33 == 0 && _tmp34 != 0))_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});}
# 336
return dest;}
# 342
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _dyneither_ptr src){
unsigned long len;
struct _dyneither_ptr temp;
# 346
len=Cyc_strlen(src);
temp=Cyc_Core_rnew_string(r,len + (unsigned long)1);
{struct _dyneither_ptr _tmp35=_dyneither_ptr_decrease_size(temp,sizeof(char),1U);
struct _dyneither_ptr _tmp36=_tmp35;struct _dyneither_ptr _tmp37;_LL1: _tmp37=_tmp36;_LL2:;
Cyc_strncpy(_tmp37,src,len);}
# 352
return temp;}
# 355
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src){
return Cyc_rstrdup(Cyc_Core_heap_region,src);}
# 359
struct _dyneither_ptr Cyc_rrealloc(struct _RegionHandle*r,struct _dyneither_ptr s,unsigned long sz){
struct _dyneither_ptr temp;
unsigned long slen;
# 363
slen=_get_dyneither_size(s,sizeof(char));
sz=sz > slen?sz: slen;
temp=({unsigned _tmp38=sz;_tag_dyneither(_region_calloc(r,sizeof(char),_tmp38),sizeof(char),_tmp38);});
# 367
{struct _dyneither_ptr _tmp39=temp;struct _dyneither_ptr _tmp3A;_LL1: _tmp3A=_tmp39;_LL2:;
Cyc_strncpy((struct _dyneither_ptr)_tmp3A,(struct _dyneither_ptr)s,slen);}
# 371
return temp;}
# 374
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*r,struct _dyneither_ptr s,unsigned long sz){
struct _dyneither_ptr temp;
unsigned long slen;
# 378
slen=Cyc_strlen(s);
sz=sz > slen?sz: slen;
temp=Cyc_Core_rnew_string(r,sz);
# 382
{struct _dyneither_ptr _tmp3B=temp;struct _dyneither_ptr _tmp3C;_LL1: _tmp3C=_tmp3B;_LL2:;
Cyc_strncpy((struct _dyneither_ptr)_tmp3C,s,slen);}
# 386
if(slen != _get_dyneither_size(s,sizeof(char)))
({struct _dyneither_ptr _tmp3D=_dyneither_ptr_plus(temp,sizeof(char),(int)slen);char _tmp3E=*((char*)_check_dyneither_subscript(_tmp3D,sizeof(char),0U));char _tmp3F='\000';if(_get_dyneither_size(_tmp3D,sizeof(char))== 1U &&(_tmp3E == 0 && _tmp3F != 0))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});
# 389
return temp;}
# 392
struct _dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned long sz){
return Cyc_rexpand(Cyc_Core_heap_region,s,sz);}
# 396
struct _dyneither_ptr Cyc_cond_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz){
unsigned long maxsizeP=_get_dyneither_size(str,sizeof(char));
struct _dyneither_ptr _tmp40=_tag_dyneither(0,0,0);
# 400
if(maxsizeP == (unsigned long)0){
maxsizeP=(unsigned long)30 > sz?30U: sz;
_tmp40=Cyc_Core_rnew_string(r,maxsizeP);
({struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(_tmp40,sizeof(char),0);char _tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0U));char _tmp43='\000';if(_get_dyneither_size(_tmp41,sizeof(char))== 1U &&(_tmp42 == 0 && _tmp43 != 0))_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});}else{
# 405
if(sz > maxsizeP){
maxsizeP=maxsizeP * (unsigned long)2 > (sz * (unsigned long)5)/ (unsigned long)4?maxsizeP * (unsigned long)2:(sz * (unsigned long)5)/ (unsigned long)4;{
struct _dyneither_ptr _tmp44=str;struct _dyneither_ptr _tmp45;_LL1: _tmp45=_tmp44;_LL2:;
_tmp40=Cyc_rexpand(r,(struct _dyneither_ptr)_tmp45,maxsizeP);};}}
# 412
return _tmp40;}
# 415
struct _dyneither_ptr Cyc_realloc_str(struct _dyneither_ptr str,unsigned long sz){
struct _dyneither_ptr _tmp46=Cyc_cond_rrealloc_str(Cyc_Core_heap_region,str,sz);
if(({char*_tmp96=(char*)_tmp46.curr;_tmp96 == (char*)(_tag_dyneither(0,0,0)).curr;}))return str;else{
return _tmp46;}}
# 426
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*r,struct _dyneither_ptr s,int start,unsigned long amt){
# 430
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,amt + (unsigned long)1);
s=_dyneither_ptr_plus(s,sizeof(char),start);
amt < _get_dyneither_size(ans,sizeof(char))&& amt <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmp97=({const char*_tmp47="amt < numelts(ans) && amt <= numelts(s)";_tag_dyneither(_tmp47,sizeof(char),40U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmp97,({const char*_tmp48="string.cyc";_tag_dyneither(_tmp48,sizeof(char),11U);}),432U);});
{unsigned long i=0U;for(0;i < amt;++ i){
({struct _dyneither_ptr _tmp49=_dyneither_ptr_plus(ans,sizeof(char),(int)i);char _tmp4A=*((char*)_check_dyneither_subscript(_tmp49,sizeof(char),0U));char _tmp4B=((const char*)s.curr)[(int)i];if(_get_dyneither_size(_tmp49,sizeof(char))== 1U &&(_tmp4A == 0 && _tmp4B != 0))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});}}
({struct _dyneither_ptr _tmp4C=_dyneither_ptr_plus(ans,sizeof(char),(int)amt);char _tmp4D=*((char*)_check_dyneither_subscript(_tmp4C,sizeof(char),0U));char _tmp4E='\000';if(_get_dyneither_size(_tmp4C,sizeof(char))== 1U &&(_tmp4D == 0 && _tmp4E != 0))_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});
return ans;}
# 439
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr s,int start,unsigned long amt){
return Cyc_rsubstring(Cyc_Core_heap_region,s,start,amt);}
# 445
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
unsigned long m=_get_dyneither_size(src,sizeof(char));
unsigned long n=_get_dyneither_size(curr_suffix,sizeof(char));
struct _dyneither_ptr err=({const char*_tmp51="replace_suffix";_tag_dyneither(_tmp51,sizeof(char),15U);});
if(m < n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=Cyc_Core_Invalid_argument,_tmp4F->f1=err;_tmp4F;}));
{unsigned long i=1U;for(0;i <= n;++ i){
if(({int _tmp98=(int)*((const char*)_check_dyneither_subscript(src,sizeof(char),(int)(m - i)));_tmp98 != (int)*((const char*)_check_dyneither_subscript(curr_suffix,sizeof(char),(int)(n - i)));}))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=Cyc_Core_Invalid_argument,_tmp50->f1=err;_tmp50;}));}}{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,((m - n)+ _get_dyneither_size(new_suffix,sizeof(char)))+ (unsigned long)1);
({struct _dyneither_ptr _tmp9A=_dyneither_ptr_decrease_size(ans,sizeof(char),1U);struct _dyneither_ptr _tmp99=src;Cyc_strncpy(_tmp9A,_tmp99,m - n);});
({struct _dyneither_ptr _tmp9C=_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)(m - n)),sizeof(char),1U);struct _dyneither_ptr _tmp9B=new_suffix;Cyc_strncpy(_tmp9C,_tmp9B,_get_dyneither_size(new_suffix,sizeof(char)));});
return ans;};}
# 460
struct _dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
return Cyc_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,new_suffix);}
# 467
struct _dyneither_ptr Cyc_strpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
int len=(int)_get_dyneither_size(s,sizeof(char));
unsigned asize=_get_dyneither_size(accept,sizeof(char));
char c;
unsigned i;
for(i=0U;i < (unsigned)len &&(int)(c=((const char*)s.curr)[(int)i])!= 0;++ i){
unsigned j=0U;for(0;j < asize;++ j){
if((int)c == (int)((const char*)accept.curr)[(int)j])
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 477
return _tag_dyneither(0,0,0);}
# 480
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
int len=(int)_get_dyneither_size(s,sizeof(char));
unsigned asize=_get_dyneither_size(accept,sizeof(char));
char c;
unsigned i;
for(i=0U;i < (unsigned)len &&(int)(c=((char*)s.curr)[(int)i])!= 0;++ i){
unsigned j=0U;for(0;j < asize;++ j){
if((int)c == (int)((const char*)accept.curr)[(int)j])
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 490
return _tag_dyneither(0,0,0);}
# 495
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c){
int len=(int)_get_dyneither_size(s,sizeof(char));
char c2;
unsigned i;
# 500
for(i=0U;i < (unsigned)len &&(int)(c2=((char*)s.curr)[(int)i])!= 0;++ i){
if((int)c2 == (int)c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 503
return _tag_dyneither(0,0,0);}
# 506
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c){
int len=(int)_get_dyneither_size(s,sizeof(char));
char c2;
unsigned i;
# 511
for(i=0U;i < (unsigned)len &&(int)(c2=((const char*)s.curr)[(int)i])!= 0;++ i){
if((int)c2 == (int)c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 514
return _tag_dyneither(0,0,0);}
# 519
struct _dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s0,char c){
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
int i=len - 1;
struct _dyneither_ptr s=s0;
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 525
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),0U))== (int)c)
return(struct _dyneither_ptr)s;}
# 529
return _tag_dyneither(0,0,0);}
# 532
struct _dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s0,char c){
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
int i=len - 1;
struct _dyneither_ptr s=s0;
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 538
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if((int)*((char*)_check_dyneither_subscript(s,sizeof(char),0U))== (int)c)
return(struct _dyneither_ptr)s;}
# 542
return _tag_dyneither(0,0,0);}
# 547
struct _dyneither_ptr Cyc_strstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
if(!((unsigned)haystack.curr)|| !((unsigned)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp9D=({const char*_tmp52="strstr";_tag_dyneither(_tmp52,sizeof(char),7U);});_tmp53->f1=_tmp9D;});_tmp53;}));
if((int)*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U))== (int)'\000')return haystack;{
# 551
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
{struct _dyneither_ptr start=haystack;for(0;({
char*_tmp9E=(char*)(start=Cyc_strchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)))).curr;_tmp9E != (char*)(_tag_dyneither(0,0,0)).curr;});start=({
struct _dyneither_ptr _tmp9F=_dyneither_ptr_plus(start,sizeof(char),1);Cyc_strchr(_tmp9F,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)));})){
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
return start;}}
# 558
return _tag_dyneither(0,0,0);};}
# 561
struct _dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
if(!((unsigned)haystack.curr)|| !((unsigned)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpA0=({const char*_tmp54="mstrstr";_tag_dyneither(_tmp54,sizeof(char),8U);});_tmp55->f1=_tmpA0;});_tmp55;}));
if((int)*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U))== (int)'\000')return haystack;{
# 565
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
{struct _dyneither_ptr start=haystack;for(0;({
char*_tmpA1=(char*)(start=Cyc_mstrchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)))).curr;_tmpA1 != (char*)(_tag_dyneither(0,0,0)).curr;});start=({
struct _dyneither_ptr _tmpA2=_dyneither_ptr_plus(start,sizeof(char),1);Cyc_mstrchr(_tmpA2,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0U)));})){
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
return start;}}
# 572
return _tag_dyneither(0,0,0);};}
# 578
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
unsigned asize=_get_dyneither_size(accept,sizeof(char));
# 582
len <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmpA3=({const char*_tmp56="len <= numelts(s)";_tag_dyneither(_tmp56,sizeof(char),18U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmpA3,({const char*_tmp57="string.cyc";_tag_dyneither(_tmp57,sizeof(char),11U);}),582U);});
{unsigned long i=0U;for(0;i < len;++ i){
int j;
for(j=0;(unsigned)j < asize;++ j){
if((int)((const char*)s.curr)[(int)i]== (int)((const char*)accept.curr)[j])
break;}
if((unsigned)j == asize)
return i;}}
# 592
return len;}
# 598
unsigned long Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
unsigned asize=_get_dyneither_size(accept,sizeof(char));
# 602
len <= _get_dyneither_size(s,sizeof(char))?0:({struct _dyneither_ptr _tmpA4=({const char*_tmp58="len <= numelts(s)";_tag_dyneither(_tmp58,sizeof(char),18U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmpA4,({const char*_tmp59="string.cyc";_tag_dyneither(_tmp59,sizeof(char),11U);}),602U);});
{unsigned long i=0U;for(0;i < len;++ i){
int j;
for(j=0;(unsigned)j < asize;++ j){
if((int)((const char*)s.curr)[(int)i]== (int)((const char*)accept.curr)[j])return i;}}}
# 608
return len;}
# 615
struct _dyneither_ptr Cyc_strtok(struct _dyneither_ptr s,struct _dyneither_ptr delim){
# 617
static struct _dyneither_ptr olds={(void*)0,(void*)0,(void*)(0 + 0)};
struct _dyneither_ptr token;
# 620
if(({char*_tmpA5=(char*)s.curr;_tmpA5 == (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({char*_tmpA6=(char*)olds.curr;_tmpA6 == (char*)(_tag_dyneither(0,0,0)).curr;}))
return _tag_dyneither(0,0,0);
s=olds;}{
# 627
unsigned long inc=Cyc_strspn((struct _dyneither_ptr)s,delim);
if(inc >= _get_dyneither_size(s,sizeof(char))||(int)*((char*)_check_dyneither_subscript(_dyneither_ptr_plus(s,sizeof(char),(int)inc),sizeof(char),0U))== (int)'\000'){
# 630
olds=_tag_dyneither(0,0,0);
return _tag_dyneither(0,0,0);}else{
# 634
_dyneither_ptr_inplace_plus(& s,sizeof(char),(int)inc);}
# 637
token=s;
s=Cyc_mstrpbrk(token,(struct _dyneither_ptr)delim);
if(({char*_tmpA7=(char*)s.curr;_tmpA7 == (char*)(_tag_dyneither(0,0,0)).curr;}))
# 641
olds=_tag_dyneither(0,0,0);else{
# 645
({struct _dyneither_ptr _tmp5A=s;char _tmp5B=*((char*)_check_dyneither_subscript(_tmp5A,sizeof(char),0U));char _tmp5C='\000';if(_get_dyneither_size(_tmp5A,sizeof(char))== 1U &&(_tmp5B == 0 && _tmp5C != 0))_throw_arraybounds();*((char*)_tmp5A.curr)=_tmp5C;});
olds=_dyneither_ptr_plus(s,sizeof(char),1);}
# 648
return token;};}
# 652
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*r,struct _dyneither_ptr s){
struct Cyc_List_List*result=0;
{int i=(int)(Cyc_strlen(s)- (unsigned long)1);for(0;i >= 0;-- i){
result=({struct Cyc_List_List*_tmp5D=_region_malloc(r,sizeof(*_tmp5D));_tmp5D->hd=(void*)((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i))),_tmp5D->tl=result;_tmp5D;});}}
return result;}
# 659
struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s){
return Cyc_rexplode(Cyc_Core_heap_region,s);}
# 663
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*chars){
struct _dyneither_ptr s=Cyc_Core_new_string((unsigned)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(chars)+ 1));
unsigned long i=0U;
while(chars != 0){
({struct _dyneither_ptr _tmp5E=_dyneither_ptr_plus(s,sizeof(char),(int)i ++);char _tmp5F=*((char*)_check_dyneither_subscript(_tmp5E,sizeof(char),0U));char _tmp60=(char)((int)chars->hd);if(_get_dyneither_size(_tmp5E,sizeof(char))== 1U &&(_tmp5F == 0 && _tmp60 != 0))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});
chars=chars->tl;}
# 670
return s;}
# 674
inline static int Cyc_casecmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2){
# 677
unsigned long min_length=len1 < len2?len1: len2;
# 679
min_length <= _get_dyneither_size(s1,sizeof(char))&& min_length <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmpA8=({const char*_tmp61="min_length <= numelts(s1) && min_length <= numelts(s2)";_tag_dyneither(_tmp61,sizeof(char),55U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmpA8,({const char*_tmp62="string.cyc";_tag_dyneither(_tmp62,sizeof(char),11U);}),679U);});{
# 681
int i=-1;
while((++ i,(unsigned long)i < min_length)){
int diff=({int _tmpA9=toupper((int)((const char*)s1.curr)[i]);_tmpA9 - toupper((int)((const char*)s2.curr)[i]);});
if(diff != 0)
return diff;}
# 687
return(int)len1 - (int)len2;};}
# 690
int Cyc_strcasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_casecmp(s1,len1,s2,len2);};}
# 698
inline static int Cyc_caseless_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 701
if(n <= (unsigned long)0)return 0;{
# 703
unsigned long min_len=len1 > len2?len2: len1;
unsigned long bound=min_len > n?n: min_len;
# 706
bound <= _get_dyneither_size(s1,sizeof(char))&& bound <= _get_dyneither_size(s2,sizeof(char))?0:({struct _dyneither_ptr _tmpAA=({const char*_tmp63="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp63,sizeof(char),45U);});((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned line))Cyc___assert_fail)(_tmpAA,({const char*_tmp64="string.cyc";_tag_dyneither(_tmp64,sizeof(char),11U);}),706U);});
# 708
{int i=0;for(0;(unsigned long)i < bound;++ i){
int retc;
if((retc=({int _tmpAB=toupper((int)((const char*)s1.curr)[i]);_tmpAB - toupper((int)((const char*)s2.curr)[i]);}))!= 0)
return retc;}}
# 713
if(len1 < n || len2 < n)
return(int)len1 - (int)len2;
return 0;};}
# 719
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_caseless_ncmp(s1,len1,s2,len2,n);}
# 728
void*memcpy(void*,const void*,unsigned long n);
void*memmove(void*,const void*,unsigned long n);
int memcmp(const void*,const void*,unsigned long n);
char*memchr(const char*,char c,unsigned long n);
void*memset(void*,int c,unsigned long n);
void bcopy(const void*src,void*dest,unsigned long n);
void bzero(void*s,unsigned long n);
char*GC_realloc(char*,unsigned n);
# 740
struct _dyneither_ptr Cyc_realloc(struct _dyneither_ptr s,unsigned long n){
unsigned long _tmp65=n;unsigned _tmp67;_LL1: _tmp67=_tmp65;_LL2:;{
char*_tmp66=GC_realloc((char*)_untag_dyneither_ptr(s,sizeof(char),1U),_tmp67);
return((struct _dyneither_ptr(*)(char*arr,unsigned s,unsigned n))Cyc_Core_mkfat)((char*)_check_null(_tmp66),sizeof(char),_tmp67);};}
# 746
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned sz){
if(((({void*_tmpAD=(void*)d.curr;_tmpAD == (void*)(_tag_dyneither(0,0,0)).curr;})|| _get_dyneither_size(d,sizeof(void))< n)||({void*_tmpAC=(void*)s.curr;_tmpAC == (void*)(_tag_dyneither(0,0,0)).curr;}))|| _get_dyneither_size(s,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpAE=({const char*_tmp68="memcpy";_tag_dyneither(_tmp68,sizeof(char),7U);});_tmp69->f1=_tmpAE;});_tmp69;}));
({void*_tmpB0=(void*)_untag_dyneither_ptr(d,sizeof(void),1U);const void*_tmpAF=(const void*)_untag_dyneither_ptr(s,sizeof(void),1U);memcpy(_tmpB0,_tmpAF,n * sz);});
return d;}
# 753
struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned sz){
if(((({void*_tmpB2=(void*)d.curr;_tmpB2 == (void*)(_tag_dyneither(0,0,0)).curr;})|| _get_dyneither_size(d,sizeof(void))< n)||({void*_tmpB1=(void*)s.curr;_tmpB1 == (void*)(_tag_dyneither(0,0,0)).curr;}))|| _get_dyneither_size(s,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpB3=({const char*_tmp6A="memove";_tag_dyneither(_tmp6A,sizeof(char),7U);});_tmp6B->f1=_tmpB3;});_tmp6B;}));
({void*_tmpB5=(void*)_untag_dyneither_ptr(d,sizeof(void),1U);const void*_tmpB4=(const void*)_untag_dyneither_ptr(s,sizeof(void),1U);memmove(_tmpB5,_tmpB4,n * sz);});
return d;}
# 760
int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
if(((({char*_tmpB7=(char*)s1.curr;_tmpB7 == (char*)(_tag_dyneither(0,0,0)).curr;})||({char*_tmpB6=(char*)s2.curr;_tmpB6 == (char*)(_tag_dyneither(0,0,0)).curr;}))|| _get_dyneither_size(s1,sizeof(char))< n)|| _get_dyneither_size(s2,sizeof(char))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpB8=({const char*_tmp6C="memcmp";_tag_dyneither(_tmp6C,sizeof(char),7U);});_tmp6D->f1=_tmpB8;});_tmp6D;}));
return({const void*_tmpBA=(const void*)_untag_dyneither_ptr(s1,sizeof(char),1U);const void*_tmpB9=(const void*)_untag_dyneither_ptr(s2,sizeof(char),1U);memcmp(_tmpBA,_tmpB9,n);});}
# 766
struct _dyneither_ptr Cyc_memchr(struct _dyneither_ptr s,char c,unsigned long n){
unsigned sz=_get_dyneither_size(s,sizeof(char));
if(({char*_tmpBB=(char*)s.curr;_tmpBB == (char*)(_tag_dyneither(0,0,0)).curr;})|| n > sz)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpBC=({const char*_tmp6E="memchr";_tag_dyneither(_tmp6E,sizeof(char),7U);});_tmp6F->f1=_tmpBC;});_tmp6F;}));{
char*_tmp70=memchr((const char*)_untag_dyneither_ptr(s,sizeof(char),1U),c,n);
if(_tmp70 == 0)return _tag_dyneither(0,0,0);{
unsigned _tmp71=(unsigned)((const char*)_untag_dyneither_ptr(s,sizeof(char),1U));
unsigned _tmp72=(unsigned)((const char*)_tmp70);
unsigned _tmp73=_tmp72 - _tmp71;
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp73);};};}
# 778
struct _dyneither_ptr Cyc_mmemchr(struct _dyneither_ptr s,char c,unsigned long n){
unsigned sz=_get_dyneither_size(s,sizeof(char));
if(({char*_tmpBD=(char*)s.curr;_tmpBD == (char*)(_tag_dyneither(0,0,0)).curr;})|| n > sz)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpBE=({const char*_tmp74="mmemchr";_tag_dyneither(_tmp74,sizeof(char),8U);});_tmp75->f1=_tmpBE;});_tmp75;}));{
char*_tmp76=memchr((const char*)_untag_dyneither_ptr(s,sizeof(char),1U),c,n);
if(_tmp76 == 0)return _tag_dyneither(0,0,0);{
unsigned _tmp77=(unsigned)((const char*)_untag_dyneither_ptr(s,sizeof(char),1U));
unsigned _tmp78=(unsigned)_tmp76;
unsigned _tmp79=_tmp78 - _tmp77;
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp79);};};}
# 790
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n){
if(({char*_tmpBF=(char*)s.curr;_tmpBF == (char*)(_tag_dyneither(0,0,0)).curr;})|| n > _get_dyneither_size(s,sizeof(char)))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpC0=({const char*_tmp7A="memset";_tag_dyneither(_tmp7A,sizeof(char),7U);});_tmp7B->f1=_tmpC0;});_tmp7B;}));
memset((void*)((char*)_untag_dyneither_ptr(s,sizeof(char),1U)),(int)c,n);
return s;}
# 797
void Cyc_bzero(struct _dyneither_ptr s,unsigned long n){
if(({char*_tmpC1=(char*)s.curr;_tmpC1 == (char*)(_tag_dyneither(0,0,0)).curr;})|| _get_dyneither_size(s,sizeof(char))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpC2=({const char*_tmp7C="bzero";_tag_dyneither(_tmp7C,sizeof(char),6U);});_tmp7D->f1=_tmpC2;});_tmp7D;}));
((void(*)(char*s,unsigned long n))bzero)((char*)_untag_dyneither_ptr(s,sizeof(char),1U),n);}
# 803
void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,unsigned long n,unsigned sz){
if(((({void*_tmpC4=(void*)src.curr;_tmpC4 == (void*)(_tag_dyneither(0,0,0)).curr;})|| _get_dyneither_size(src,sizeof(void))< n)||({void*_tmpC3=(void*)dst.curr;_tmpC3 == (void*)(_tag_dyneither(0,0,0)).curr;}))|| _get_dyneither_size(dst,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmpC5=({const char*_tmp7E="bcopy";_tag_dyneither(_tmp7E,sizeof(char),6U);});_tmp7F->f1=_tmpC5;});_tmp7F;}));
({const void*_tmpC7=(const void*)_untag_dyneither_ptr(src,sizeof(void),1U);void*_tmpC6=(void*)_untag_dyneither_ptr(dst,sizeof(void),1U);bcopy(_tmpC7,_tmpC6,n * sz);});}
