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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
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
# 896
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress_kb(void*);
# 913
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 953
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 978
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 986
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1063
struct _tuple0*Cyc_Absyn_uniquergn_qvar();
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83 "dict.h"
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 69 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned,struct Cyc_Absyn_Fndecl*);
# 79
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple0*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 152
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 35
void Cyc_Tcutil_warn(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 47
int Cyc_Tcutil_is_function_type(void*t);
# 49
int Cyc_Tcutil_is_array_type(void*t);
# 56
int Cyc_Tcutil_is_bound_one(void*b);
# 61
int Cyc_Tcutil_is_bits_only_type(void*t);
# 92
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 112
void*Cyc_Tcutil_compress(void*t);
# 115
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 148
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 178
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 192
void Cyc_Tcutil_explain_failure();
# 194
int Cyc_Tcutil_unify(void*,void*);
# 216
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 226
void Cyc_Tcutil_check_bitfield(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 253 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
# 255
void Cyc_Tcutil_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 263
void Cyc_Tcutil_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 267
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 293
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 303
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 318
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 335
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 339
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 356
int Cyc_Tcutil_zeroable_type(void*t);
# 360
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 363
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);struct _tuple10{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 33
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 37
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 40
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 60 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 63
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 65
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned loc,struct _dyneither_ptr*msg);
# 73
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned loc,struct _dyneither_ptr*msg);
# 82
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 46 "graph.h"
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*));
# 92
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict g);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);
void Cyc_Callgraph_print_callgraph(struct Cyc___cycFILE*,struct Cyc_Dict_Dict);static char _tmp0[1U]="";
# 38 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1U};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 42
int Cyc_Tc_aggressive_warn=0;struct _tuple11{unsigned f1;struct _tuple0*f2;int f3;};
# 44
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple11*_tmp1=(struct _tuple11*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 50
(*_tmp1).f3=1;
return 1;}}
# 54
return 0;}
# 57
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned loc,int i,int nargs,void*att){
if(i < 1 || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp1D5;_tmp1D5.tag=0U,({struct _dyneither_ptr _tmp213=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));_tmp1D5.f1=_tmp213;});_tmp1D5;});void*_tmp2[1U];_tmp2[0]=& _tmp4;({unsigned _tmp215=loc;struct _dyneither_ptr _tmp214=({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp215,_tmp214,_tag_dyneither(_tmp2,sizeof(void*),1U));});});
return 0;}
# 62
return 1;}struct _tuple12{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 64
static void Cyc_Tc_fnTypeAttsOverlap(unsigned loc,int i,struct _tuple12 lst1,struct _tuple12 lst2){
# 67
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp1D8;_tmp1D8.tag=0U,_tmp1D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);_tmp1D8;});struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp1D7;_tmp1D7.tag=0U,_tmp1D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);_tmp1D7;});struct Cyc_Int_pa_PrintArg_struct _tmp9=({struct Cyc_Int_pa_PrintArg_struct _tmp1D6;_tmp1D6.tag=1U,_tmp1D6.f1=(unsigned long)i;_tmp1D6;});void*_tmp5[3U];_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8,_tmp5[2]=& _tmp9;({unsigned _tmp217=loc;struct _dyneither_ptr _tmp216=({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp217,_tmp216,_tag_dyneither(_tmp5,sizeof(void*),3U));});});}
# 71
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned loc){
struct _tuple12 init_params=({struct _tuple12 _tmp1DF;_tmp1DF.f1=0,({struct _dyneither_ptr _tmp218=({const char*_tmp3A="initializes";_tag_dyneither(_tmp3A,sizeof(char),12U);});_tmp1DF.f2=_tmp218;});_tmp1DF;});
struct _tuple12 nolive_unique_params=({struct _tuple12 _tmp1DE;_tmp1DE.f1=0,({struct _dyneither_ptr _tmp219=({const char*_tmp39="noliveunique";_tag_dyneither(_tmp39,sizeof(char),13U);});_tmp1DE.f2=_tmp219;});_tmp1DE;});
struct _tuple12 consume_params=({struct _tuple12 _tmp1DD;_tmp1DD.f1=0,({struct _dyneither_ptr _tmp21A=({const char*_tmp38="consume";_tag_dyneither(_tmp38,sizeof(char),8U);});_tmp1DD.f2=_tmp21A;});_tmp1DD;});
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;struct Cyc_List_List*_tmp37;struct Cyc_List_List*_tmp36;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->tag == 5U){_LL1: _tmp37=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).attributes;_tmp36=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).args;_LL2: {
# 77
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36);
for(0;_tmp37 != 0;_tmp37=_tmp37->tl){
void*_tmpC=(void*)_tmp37->hd;void*_tmpD=_tmpC;int _tmp33;int _tmp32;int _tmp31;switch(*((int*)_tmpD)){case 20U: _LL6: _tmp31=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL7:
# 81
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp37->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,init_params,consume_params);{
struct _tuple8*_tmpE=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp36,_tmp31 - 1);struct _tuple8*_tmpF=_tmpE;void*_tmp24;_LLF: _tmp24=_tmpF->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp23="initializes attribute allowed only on";_tag_dyneither(_tmp23,sizeof(char),38U);});
{void*_tmp10=Cyc_Tcutil_compress(_tmp24);void*_tmp11=_tmp10;void*_tmp21;void*_tmp20;void*_tmp1F;void*_tmp1E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->tag == 3U){_LL12: _tmp21=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).elt_type;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).nullable;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).bounds;_tmp1E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).zero_term;_LL13:
# 88
 if(Cyc_Tcutil_force_type2bool(0,_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp1D9;_tmp1D9.tag=0U,_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1D9;});void*_tmp12[1U];_tmp12[0]=& _tmp14;({unsigned _tmp21C=loc;struct _dyneither_ptr _tmp21B=({const char*_tmp13="%s non-null pointers";_tag_dyneither(_tmp13,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp21C,_tmp21B,_tag_dyneither(_tmp12,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_bound_one(_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp1DA;_tmp1DA.tag=0U,_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1DA;});void*_tmp15[1U];_tmp15[0]=& _tmp17;({unsigned _tmp21E=loc;struct _dyneither_ptr _tmp21D=({const char*_tmp16="%s pointers of size 1";_tag_dyneither(_tmp16,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp21E,_tmp21D,_tag_dyneither(_tmp15,sizeof(void*),1U));});});
if(Cyc_Tcutil_force_type2bool(0,_tmp1E))
({struct Cyc_String_pa_PrintArg_struct _tmp1A=({struct Cyc_String_pa_PrintArg_struct _tmp1DB;_tmp1DB.tag=0U,_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1DB;});void*_tmp18[1U];_tmp18[0]=& _tmp1A;({unsigned _tmp220=loc;struct _dyneither_ptr _tmp21F=({const char*_tmp19="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp19,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp220,_tmp21F,_tag_dyneither(_tmp18,sizeof(void*),1U));});});
goto _LL11;}else{_LL14: _LL15:
# 96
({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp1DC;_tmp1DC.tag=0U,_tmp1DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1DC;});void*_tmp1B[1U];_tmp1B[0]=& _tmp1D;({unsigned _tmp222=loc;struct _dyneither_ptr _tmp221=({const char*_tmp1C="%s pointers";_tag_dyneither(_tmp1C,sizeof(char),12U);});Cyc_Tcutil_terr(_tmp222,_tmp221,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});}_LL11:;}
# 98
({struct Cyc_List_List*_tmp223=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=(void*)_tmp31,_tmp22->tl=init_params.f1;_tmp22;});init_params.f1=_tmp223;});
goto _LL5;};};case 21U: _LL8: _tmp32=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL9:
# 101
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp32,nargs,(void*)_tmp37->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,nolive_unique_params,init_params);{
# 104
struct _tuple8*_tmp25=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp36,_tmp32 - 1);struct _tuple8*_tmp26=_tmp25;void*_tmp2A;_LL17: _tmp2A=_tmp26->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2A,0))
({void*_tmp27=0U;({unsigned _tmp225=loc;struct _dyneither_ptr _tmp224=({const char*_tmp28="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp28,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp225,_tmp224,_tag_dyneither(_tmp27,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp226=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=(void*)_tmp32,_tmp29->tl=nolive_unique_params.f1;_tmp29;});nolive_unique_params.f1=_tmp226;});
goto _LL5;};case 22U: _LLA: _tmp33=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpD)->f1;_LLB:
# 110
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp33,nargs,(void*)_tmp37->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp33,consume_params,init_params);{
# 113
struct _tuple8*_tmp2B=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp36,_tmp33 - 1);struct _tuple8*_tmp2C=_tmp2B;void*_tmp30;_LL1A: _tmp30=_tmp2C->f3;_LL1B:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp30,0))
({void*_tmp2D=0U;({unsigned _tmp228=loc;struct _dyneither_ptr _tmp227=({const char*_tmp2E="consume attribute allowed only on unique pointers";_tag_dyneither(_tmp2E,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp228,_tmp227,_tag_dyneither(_tmp2D,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp229=({struct Cyc_List_List*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->hd=(void*)_tmp33,_tmp2F->tl=consume_params.f1;_tmp2F;});consume_params.f1=_tmp229;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 121
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp34=0U;({struct _dyneither_ptr _tmp22A=({const char*_tmp35="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp35,sizeof(char),34U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp22A,_tag_dyneither(_tmp34,sizeof(void*),0U));});});}_LL0:;}struct _tuple13{void*f1;int f2;};
# 126
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 128
struct Cyc_Absyn_Vardecl*_tmp3B=vd;enum Cyc_Absyn_Scope _tmp93;struct _tuple0*_tmp92;void*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_List_List*_tmp8F;_LL1: _tmp93=_tmp3B->sc;_tmp92=_tmp3B->name;_tmp91=_tmp3B->type;_tmp90=_tmp3B->initializer;_tmp8F=_tmp3B->attributes;_LL2:;
# 135
{void*_tmp3C=Cyc_Tcutil_compress(_tmp91);void*_tmp3D=_tmp3C;void*_tmp49;struct Cyc_Absyn_Tqual _tmp48;void*_tmp47;unsigned _tmp46;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->tag == 4U){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).num_elts == 0){_LL4: _tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).elt_type;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).tq;_tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).zero_term;_tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D)->f1).zt_loc;if(_tmp90 != 0){_LL5:
# 137
{void*_tmp3E=_tmp90->r;void*_tmp3F=_tmp3E;struct Cyc_List_List*_tmp45;struct Cyc_List_List*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct _dyneither_ptr _tmp41;struct _dyneither_ptr _tmp40;switch(*((int*)_tmp3F)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3F)->f1).Wstring_c).tag){case 8U: _LL9: _tmp40=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3F)->f1).String_c).val;_LLA:
# 139
 _tmp91=({void*_tmp22F=({void*_tmp22E=_tmp49;struct Cyc_Absyn_Tqual _tmp22D=_tmp48;struct Cyc_Absyn_Exp*_tmp22C=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp40,sizeof(char)),0U);void*_tmp22B=_tmp47;Cyc_Absyn_array_type(_tmp22E,_tmp22D,_tmp22C,_tmp22B,_tmp46);});vd->type=_tmp22F;});
goto _LL8;case 9U: _LLB: _tmp41=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3F)->f1).Wstring_c).val;_LLC:
# 142
 _tmp91=({void*_tmp234=({void*_tmp233=_tmp49;struct Cyc_Absyn_Tqual _tmp232=_tmp48;struct Cyc_Absyn_Exp*_tmp231=Cyc_Absyn_uint_exp(1U,0U);void*_tmp230=_tmp47;Cyc_Absyn_array_type(_tmp233,_tmp232,_tmp231,_tmp230,_tmp46);});vd->type=_tmp234;});
goto _LL8;default: goto _LL15;}case 27U: _LLD: _tmp42=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3F)->f2;_LLE:
 _tmp43=_tmp42;goto _LL10;case 28U: _LLF: _tmp43=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3F)->f1;_LL10:
# 147
 _tmp91=({void*_tmp235=Cyc_Absyn_array_type(_tmp49,_tmp48,_tmp43,_tmp47,_tmp46);vd->type=_tmp235;});
goto _LL8;case 36U: _LL11: _tmp44=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp3F)->f2;_LL12:
 _tmp45=_tmp44;goto _LL14;case 26U: _LL13: _tmp45=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3F)->f1;_LL14:
# 151
 _tmp91=({void*_tmp23A=({void*_tmp239=_tmp49;struct Cyc_Absyn_Tqual _tmp238=_tmp48;struct Cyc_Absyn_Exp*_tmp237=Cyc_Absyn_uint_exp((unsigned)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45),0U);void*_tmp236=_tmp47;Cyc_Absyn_array_type(_tmp239,_tmp238,_tmp237,_tmp236,_tmp46);});vd->type=_tmp23A;});
goto _LL8;default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 155
goto _LL3;}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 159
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp91);
# 161
({int _tmp23B=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp91);(vd->tq).real_const=_tmp23B;});
# 164
({int _tmp23C=!Cyc_Tcutil_is_array_type(_tmp91);vd->escapes=_tmp23C;});
# 166
if(Cyc_Tcutil_is_function_type(_tmp91)){
_tmp8F=Cyc_Tcutil_transfer_fn_type_atts(_tmp91,_tmp8F);
Cyc_Tc_fnTypeAttsOK(te,_tmp91,loc);}
# 171
if((int)_tmp93 == (int)3U ||(int)_tmp93 == (int)4U){
if(_tmp90 != 0 && !in_cinclude)
({void*_tmp4A=0U;({unsigned _tmp23E=loc;struct _dyneither_ptr _tmp23D=({const char*_tmp4B="extern declaration should not have initializer";_tag_dyneither(_tmp4B,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp23E,_tmp23D,_tag_dyneither(_tmp4A,sizeof(void*),0U));});});}else{
if(!Cyc_Tcutil_is_function_type(_tmp91)){
# 178
for(0;_tmp8F != 0;_tmp8F=_tmp8F->tl){
void*_tmp4C=(void*)_tmp8F->hd;void*_tmp4D=_tmp4C;switch(*((int*)_tmp4D)){case 6U: _LL18: _LL19:
 goto _LL1B;case 8U: _LL1A: _LL1B:
# 183
 goto _LL1D;case 9U: _LL1C: _LL1D:
 goto _LL1F;case 10U: _LL1E: _LL1F:
 goto _LL21;case 11U: _LL20: _LL21:
 goto _LL23;case 12U: _LL22: _LL23:
 goto _LL25;case 13U: _LL24: _LL25:
 goto _LL27;case 14U: _LL26: _LL27:
 continue;default: _LL28: _LL29:
# 191
({struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0U,({
struct _dyneither_ptr _tmp23F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8F->hd));_tmp1E1.f1=_tmp23F;});_tmp1E1;});struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp1E0;_tmp1E0.tag=0U,({struct _dyneither_ptr _tmp240=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1E0.f1=_tmp240;});_tmp1E0;});void*_tmp4E[2U];_tmp4E[0]=& _tmp50,_tmp4E[1]=& _tmp51;({unsigned _tmp242=loc;struct _dyneither_ptr _tmp241=({const char*_tmp4F="bad attribute %s for variable %s";_tag_dyneither(_tmp4F,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp242,_tmp241,_tag_dyneither(_tmp4E,sizeof(void*),2U));});});
goto _LL17;}_LL17:;}
# 196
if(_tmp90 == 0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(_tmp91))
({struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp1E3;_tmp1E3.tag=0U,({
struct _dyneither_ptr _tmp243=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1E3.f1=_tmp243;});_tmp1E3;});struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp1E2;_tmp1E2.tag=0U,({struct _dyneither_ptr _tmp244=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp91));_tmp1E2.f1=_tmp244;});_tmp1E2;});void*_tmp52[2U];_tmp52[0]=& _tmp54,_tmp52[1]=& _tmp55;({unsigned _tmp246=loc;struct _dyneither_ptr _tmp245=({const char*_tmp53="initializer required for variable %s of type %s";_tag_dyneither(_tmp53,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp246,_tmp245,_tag_dyneither(_tmp52,sizeof(void*),2U));});});}else{
# 201
struct Cyc_Absyn_Exp*_tmp56=_tmp90;
# 205
struct _handler_cons _tmp57;_push_handler(& _tmp57);{int _tmp59=0;if(setjmp(_tmp57.handler))_tmp59=1;if(!_tmp59){
{void*_tmp5A=Cyc_Tcexp_tcExpInitializer(te,& _tmp91,_tmp56);
if(!Cyc_Tcutil_coerce_assign(te,_tmp56,_tmp91)){
struct _dyneither_ptr _tmp5B=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp5C=" declared with type ";
struct _dyneither_ptr _tmp5D=Cyc_Absynpp_typ2string(_tmp91);
const char*_tmp5E=" but initialized with type ";
struct _dyneither_ptr _tmp5F=Cyc_Absynpp_typ2string(_tmp5A);
if(({unsigned long _tmp24A=({unsigned long _tmp249=({unsigned long _tmp248=({unsigned long _tmp247=Cyc_strlen((struct _dyneither_ptr)_tmp5B);_tmp247 + Cyc_strlen(({const char*_tmp60=_tmp5C;_tag_dyneither(_tmp60,sizeof(char),21U);}));});_tmp248 + Cyc_strlen((struct _dyneither_ptr)_tmp5D);});_tmp249 + Cyc_strlen(({const char*_tmp61=_tmp5E;_tag_dyneither(_tmp61,sizeof(char),28U);}));});_tmp24A + Cyc_strlen((struct _dyneither_ptr)_tmp5F);})> (unsigned long)70)
({struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp1E8;_tmp1E8.tag=0U,_tmp1E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5B);_tmp1E8;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0U,({struct _dyneither_ptr _tmp24B=(struct _dyneither_ptr)({const char*_tmp6A=_tmp5C;_tag_dyneither(_tmp6A,sizeof(char),21U);});_tmp1E7.f1=_tmp24B;});_tmp1E7;});struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0U,_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);_tmp1E6;});struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0U,({struct _dyneither_ptr _tmp24C=(struct _dyneither_ptr)({const char*_tmp69=_tmp5E;_tag_dyneither(_tmp69,sizeof(char),28U);});_tmp1E5.f1=_tmp24C;});_tmp1E5;});struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp1E4;_tmp1E4.tag=0U,_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);_tmp1E4;});void*_tmp62[5U];_tmp62[0]=& _tmp64,_tmp62[1]=& _tmp65,_tmp62[2]=& _tmp66,_tmp62[3]=& _tmp67,_tmp62[4]=& _tmp68;({unsigned _tmp24E=loc;struct _dyneither_ptr _tmp24D=({const char*_tmp63="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp63,sizeof(char),16U);});Cyc_Tcutil_terr(_tmp24E,_tmp24D,_tag_dyneither(_tmp62,sizeof(void*),5U));});});else{
# 216
({struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp1ED;_tmp1ED.tag=0U,_tmp1ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5B);_tmp1ED;});struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0U,({struct _dyneither_ptr _tmp24F=(struct _dyneither_ptr)({const char*_tmp73=_tmp5C;_tag_dyneither(_tmp73,sizeof(char),21U);});_tmp1EC.f1=_tmp24F;});_tmp1EC;});struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0U,_tmp1EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);_tmp1EB;});struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0U,({struct _dyneither_ptr _tmp250=(struct _dyneither_ptr)({const char*_tmp72=_tmp5E;_tag_dyneither(_tmp72,sizeof(char),28U);});_tmp1EA.f1=_tmp250;});_tmp1EA;});struct Cyc_String_pa_PrintArg_struct _tmp71=({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0U,_tmp1E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);_tmp1E9;});void*_tmp6B[5U];_tmp6B[0]=& _tmp6D,_tmp6B[1]=& _tmp6E,_tmp6B[2]=& _tmp6F,_tmp6B[3]=& _tmp70,_tmp6B[4]=& _tmp71;({unsigned _tmp252=loc;struct _dyneither_ptr _tmp251=({const char*_tmp6C="%s%s%s%s%s";_tag_dyneither(_tmp6C,sizeof(char),11U);});Cyc_Tcutil_terr(_tmp252,_tmp251,_tag_dyneither(_tmp6B,sizeof(void*),5U));});});}
Cyc_Tcutil_explain_failure();}
# 220
if(!Cyc_Tcutil_is_const_exp(_tmp56))
({void*_tmp74=0U;({unsigned _tmp254=loc;struct _dyneither_ptr _tmp253=({const char*_tmp75="initializer is not a constant expression";_tag_dyneither(_tmp75,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp254,_tmp253,_tag_dyneither(_tmp74,sizeof(void*),0U));});});}
# 206
;_pop_handler();}else{void*_tmp58=(void*)Cyc_Core_get_exn_thrown();void*_tmp76=_tmp58;void*_tmp79;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp76)->tag == Cyc_Tcenv_Env_error){_LL2B: _LL2C:
# 224
({void*_tmp77=0U;({unsigned _tmp256=loc;struct _dyneither_ptr _tmp255=({const char*_tmp78="initializer is not a constant expression";_tag_dyneither(_tmp78,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp256,_tmp255,_tag_dyneither(_tmp77,sizeof(void*),0U));});});
goto _LL2A;}else{_LL2D: _tmp79=_tmp76;_LL2E:(int)_rethrow(_tmp79);}_LL2A:;}};}}else{
# 230
for(0;_tmp8F != 0;_tmp8F=_tmp8F->tl){
void*_tmp7A=(void*)_tmp8F->hd;void*_tmp7B=_tmp7A;switch(*((int*)_tmp7B)){case 0U: _LL30: _LL31:
# 233
 goto _LL33;case 1U: _LL32: _LL33:
 goto _LL35;case 2U: _LL34: _LL35:
 goto _LL37;case 3U: _LL36: _LL37:
 goto _LL39;case 4U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 goto _LL3D;case 20U: _LL3C: _LL3D:
 goto _LL3F;case 23U: _LL3E: _LL3F:
 goto _LL41;case 26U: _LL40: _LL41:
 goto _LL43;case 5U: _LL42: _LL43:
({void*_tmp7C=0U;({struct _dyneither_ptr _tmp257=({const char*_tmp7D="tcVardecl: fn type atts in function var decl";_tag_dyneither(_tmp7D,sizeof(char),45U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp257,_tag_dyneither(_tmp7C,sizeof(void*),0U));});});case 6U: _LL44: _LL45:
# 244
 goto _LL47;case 7U: _LL46: _LL47:
# 246
({struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmp1EE;_tmp1EE.tag=0U,({
struct _dyneither_ptr _tmp258=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8F->hd));_tmp1EE.f1=_tmp258;});_tmp1EE;});void*_tmp7E[1U];_tmp7E[0]=& _tmp80;({unsigned _tmp25A=loc;struct _dyneither_ptr _tmp259=({const char*_tmp7F="bad attribute %s in function declaration";_tag_dyneither(_tmp7F,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp25A,_tmp259,_tag_dyneither(_tmp7E,sizeof(void*),1U));});});
goto _LL2F;default: _LL48: _LL49:
 continue;}_LL2F:;}}}
# 254
{struct _handler_cons _tmp81;_push_handler(& _tmp81);{int _tmp83=0;if(setjmp(_tmp81.handler))_tmp83=1;if(!_tmp83){
{struct _tuple13*_tmp84=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,_tmp92);
void*_tmp85=(*_tmp84).f1;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp86=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=1U,_tmp8A->f1=vd;_tmp8A;});
void**_tmp87=Cyc_Tcdecl_merge_binding(_tmp85,(void*)_tmp86,loc,Cyc_Tc_tc_msg);
if(_tmp87 == 0){_npop_handler(0U);return;}{
void*_tmp88=*_tmp87;
# 262
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp88 != _tmp85 ||(*_tmp84).f2)
# 265
({struct Cyc_Dict_Dict _tmp25D=({struct Cyc_Dict_Dict _tmp25C=(te->ae)->ordinaries;struct _tuple0*_tmp25B=_tmp92;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp25C,_tmp25B,({struct _tuple13*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->f1=_tmp88,_tmp89->f2=1;_tmp89;}));});(te->ae)->ordinaries=_tmp25D;});}
_npop_handler(0U);return;};}
# 255
;_pop_handler();}else{void*_tmp82=(void*)Cyc_Core_get_exn_thrown();void*_tmp8B=_tmp82;void*_tmp8C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8B)->tag == Cyc_Dict_Absent){_LL4B: _LL4C:
# 267
 goto _LL4A;}else{_LL4D: _tmp8C=_tmp8B;_LL4E:(int)_rethrow(_tmp8C);}_LL4A:;}};}
if(exports == 0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp261=({struct Cyc_Dict_Dict _tmp260=(te->ae)->ordinaries;struct _tuple0*_tmp25F=_tmp92;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp260,_tmp25F,({struct _tuple13*_tmp8E=_cycalloc(sizeof(*_tmp8E));
({void*_tmp25E=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=1U,_tmp8D->f1=vd;_tmp8D;});_tmp8E->f1=_tmp25E;}),_tmp8E->f2=0;_tmp8E;}));});
# 269
(te->ae)->ordinaries=_tmp261;});}
# 273
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp94=n;union Cyc_Absyn_Nmspace _tmp98;struct _dyneither_ptr*_tmp97;_LL1: _tmp98=_tmp94->f1;_tmp97=_tmp94->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp95=_tmp98;if((_tmp95.Abs_n).tag == 2){if((_tmp95.Abs_n).val == 0){_LL4: _LL5:
# 277
 return({struct _dyneither_ptr _tmp262=(struct _dyneither_ptr)*_tmp97;Cyc_strcmp(_tmp262,({const char*_tmp96="main";_tag_dyneither(_tmp96,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;};}
# 282
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 284
struct _tuple0*q=fd->name;
# 288
if((int)fd->sc == (int)Cyc_Absyn_ExternC && !te->in_extern_c_include)
({void*_tmp99=0U;({unsigned _tmp264=loc;struct _dyneither_ptr _tmp263=({const char*_tmp9A="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmp9A,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp264,_tmp263,_tag_dyneither(_tmp99,sizeof(void*),0U));});});
# 292
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp9B=(void*)atts->hd;void*_tmp9C=_tmp9B;switch(*((int*)_tmp9C)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
# 296
({struct Cyc_String_pa_PrintArg_struct _tmp9F=({struct Cyc_String_pa_PrintArg_struct _tmp1EF;_tmp1EF.tag=0U,({struct _dyneither_ptr _tmp265=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1EF.f1=_tmp265;});_tmp1EF;});void*_tmp9D[1U];_tmp9D[0]=& _tmp9F;({unsigned _tmp267=loc;struct _dyneither_ptr _tmp266=({const char*_tmp9E="bad attribute %s for function";_tag_dyneither(_tmp9E,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp267,_tmp266,_tag_dyneither(_tmp9D,sizeof(void*),1U));});});
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}}
# 302
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 305
({struct Cyc_List_List*_tmp268=Cyc_Tcutil_transfer_fn_type_atts(t,(fd->i).attributes);(fd->i).attributes=_tmp268;});
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 309
{struct _handler_cons _tmpA0;_push_handler(& _tmpA0);{int _tmpA2=0;if(setjmp(_tmpA0.handler))_tmpA2=1;if(!_tmpA2){
{struct _tuple13*_tmpA3=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
void*_tmpA4=(*_tmpA3).f1;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA5=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=2U,_tmpA9->f1=fd;_tmpA9;});
void**_tmpA6=Cyc_Tcdecl_merge_binding(_tmpA4,(void*)_tmpA5,loc,Cyc_Tc_tc_msg);
if(_tmpA6 != 0){
void*_tmpA7=*_tmpA6;
# 317
if(exports == 0 || Cyc_Tc_export_member(q,*exports)){
if(!(_tmpA7 == _tmpA4 &&(*_tmpA3).f2))
({struct Cyc_Dict_Dict _tmp26B=({struct Cyc_Dict_Dict _tmp26A=(te->ae)->ordinaries;struct _tuple0*_tmp269=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp26A,_tmp269,({struct _tuple13*_tmpA8=_cycalloc(sizeof(*_tmpA8));
_tmpA8->f1=_tmpA7,_tmpA8->f2=1;_tmpA8;}));});
# 319
(te->ae)->ordinaries=_tmp26B;});}}}
# 310
;_pop_handler();}else{void*_tmpA1=(void*)Cyc_Core_get_exn_thrown();void*_tmpAA=_tmpA1;void*_tmpAF;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpAA)->tag == Cyc_Dict_Absent){_LL8: _LL9:
# 323
 if(exports == 0 || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAB=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=2U,_tmpAE->f1=fd;_tmpAE;});
({struct Cyc_Dict_Dict _tmp26F=({struct Cyc_Dict_Dict _tmp26E=(te->ae)->ordinaries;struct _tuple0*_tmp26D=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp26E,_tmp26D,({struct _tuple13*_tmpAD=_cycalloc(sizeof(*_tmpAD));
({void*_tmp26C=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=2U,_tmpAC->f1=fd;_tmpAC;});_tmpAD->f1=_tmp26C;}),_tmpAD->f2=0;_tmpAD;}));});
# 325
(te->ae)->ordinaries=_tmp26F;});}
# 328
goto _LL7;}else{_LLA: _tmpAF=_tmpAA;_LLB:(int)_rethrow(_tmpAF);}_LL7:;}};}
# 332
if(te->in_extern_c_include)return;{
# 337
struct Cyc_Tcenv_Fenv*_tmpB0=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_tmpB1=({struct Cyc_Tcenv_Tenv*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->ns=te->ns,_tmpD5->ae=te->ae,_tmpD5->le=_tmpB0,_tmpD5->allow_valueof=0,_tmpD5->in_extern_c_include=0,_tmpD5->in_tempest=te->in_tempest,_tmpD5->tempest_generalize=te->tempest_generalize;_tmpD5;});
# 340
Cyc_Tcstmt_tcStmt(_tmpB1,fd->body,0);
# 343
Cyc_Tcenv_check_delayed_effects(_tmpB1);
Cyc_Tcenv_check_delayed_constraints(_tmpB1);
# 346
if(_tmpB1->in_tempest){
_tmpB1->tempest_generalize=1;
Cyc_Tcutil_check_fndecl_valid_type(loc,_tmpB1,fd);
_tmpB1->tempest_generalize=0;}
# 352
if(Cyc_Tc_is_main(q)){
# 354
{void*_tmpB2=Cyc_Tcutil_compress((fd->i).ret_type);void*_tmpB3=_tmpB2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB3)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB3)->f1)){case 0U: _LLD: _LLE:
({void*_tmpB4=0U;({unsigned _tmp271=loc;struct _dyneither_ptr _tmp270=({const char*_tmpB5="main declared with return type void";_tag_dyneither(_tmpB5,sizeof(char),36U);});Cyc_Tcutil_warn(_tmp271,_tmp270,_tag_dyneither(_tmpB4,sizeof(void*),0U));});});goto _LLC;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB3)->f1)->f2){case Cyc_Absyn_Int_sz: _LLF: _LL10:
 goto _LLC;case Cyc_Absyn_Long_sz: _LL11: _LL12:
 goto _LLC;default: goto _LL13;}default: goto _LL13;}else{_LL13: _LL14:
# 359
({struct Cyc_String_pa_PrintArg_struct _tmpB8=({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0U,({
struct _dyneither_ptr _tmp272=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((fd->i).ret_type));_tmp1F0.f1=_tmp272;});_tmp1F0;});void*_tmpB6[1U];_tmpB6[0]=& _tmpB8;({unsigned _tmp274=loc;struct _dyneither_ptr _tmp273=({const char*_tmpB7="main declared with return type %s instead of int or void";_tag_dyneither(_tmpB7,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp274,_tmp273,_tag_dyneither(_tmpB6,sizeof(void*),1U));});});
goto _LLC;}_LLC:;}
# 363
if((fd->i).c_varargs ||(fd->i).cyc_varargs != 0)
({void*_tmpB9=0U;({unsigned _tmp276=loc;struct _dyneither_ptr _tmp275=({const char*_tmpBA="main declared with varargs";_tag_dyneither(_tmpBA,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp276,_tmp275,_tag_dyneither(_tmpB9,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpBB=(fd->i).args;
if(_tmpBB != 0){
struct _tuple8*_tmpBC=(struct _tuple8*)_tmpBB->hd;struct _tuple8*_tmpBD=_tmpBC;void*_tmpD4;_LL16: _tmpD4=_tmpBD->f3;_LL17:;
{void*_tmpBE=Cyc_Tcutil_compress(_tmpD4);void*_tmpBF=_tmpBE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBF)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBF)->f1)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBF)->f1)->f2){case Cyc_Absyn_Int_sz: _LL19: _LL1A:
 goto _LL18;case Cyc_Absyn_Long_sz: _LL1B: _LL1C:
 goto _LL18;default: goto _LL1D;}else{goto _LL1D;}}else{_LL1D: _LL1E:
# 372
({struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0U,({
struct _dyneither_ptr _tmp277=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD4));_tmp1F1.f1=_tmp277;});_tmp1F1;});void*_tmpC0[1U];_tmpC0[0]=& _tmpC2;({unsigned _tmp279=loc;struct _dyneither_ptr _tmp278=({const char*_tmpC1="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpC1,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp279,_tmp278,_tag_dyneither(_tmpC0,sizeof(void*),1U));});});
goto _LL18;}_LL18:;}
# 376
_tmpBB=_tmpBB->tl;
if(_tmpBB != 0){
struct _tuple8*_tmpC3=(struct _tuple8*)_tmpBB->hd;struct _tuple8*_tmpC4=_tmpC3;void*_tmpD3;_LL20: _tmpD3=_tmpC4->f3;_LL21:;
_tmpBB=_tmpBB->tl;
if(_tmpBB != 0)
({void*_tmpC5=0U;({unsigned _tmp27B=loc;struct _dyneither_ptr _tmp27A=({const char*_tmpC6="main declared with too many arguments";_tag_dyneither(_tmpC6,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp27B,_tmp27A,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=(fd->i).tvars;_tmpD2;});
if(((!({void*_tmp293=_tmpD3;Cyc_Tcutil_unify(_tmp293,({void*_tmp292=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp28E=({struct Cyc_Core_Opt*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->v=& Cyc_Tcutil_rk;_tmpC7;});Cyc_Absyn_new_evar(_tmp28E,tvs);}));void*_tmp291=({
struct Cyc_Core_Opt*_tmp28F=({struct Cyc_Core_Opt*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->v=& Cyc_Tcutil_rk;_tmpC8;});Cyc_Absyn_new_evar(_tmp28F,tvs);});
# 383
struct Cyc_Absyn_Tqual _tmp290=
# 385
Cyc_Absyn_empty_tqual(0U);
# 383
Cyc_Absyn_fatptr_type(_tmp292,_tmp291,_tmp290,
# 385
Cyc_Tcutil_any_bool(& _tmpB1));}));})&& !({
void*_tmp28D=_tmpD3;Cyc_Tcutil_unify(_tmp28D,({void*_tmp28C=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp288=({struct Cyc_Core_Opt*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->v=& Cyc_Tcutil_rk;_tmpC9;});Cyc_Absyn_new_evar(_tmp288,tvs);}));void*_tmp28B=({
# 388
struct Cyc_Core_Opt*_tmp289=({struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=& Cyc_Tcutil_rk;_tmpCA;});Cyc_Absyn_new_evar(_tmp289,tvs);});
# 386
struct Cyc_Absyn_Tqual _tmp28A=
# 389
Cyc_Absyn_empty_tqual(0U);
# 386
Cyc_Absyn_fatptr_type(_tmp28C,_tmp28B,_tmp28A,
# 389
Cyc_Tcutil_any_bool(& _tmpB1));}));}))&& !({
void*_tmp287=_tmpD3;Cyc_Tcutil_unify(_tmp287,({void*_tmp286=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp282=({struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->v=& Cyc_Tcutil_rk;_tmpCB;});Cyc_Absyn_new_evar(_tmp282,tvs);}));void*_tmp285=({
struct Cyc_Core_Opt*_tmp283=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=& Cyc_Tcutil_rk;_tmpCC;});Cyc_Absyn_new_evar(_tmp283,tvs);});
# 390
struct Cyc_Absyn_Tqual _tmp284=
# 392
Cyc_Absyn_const_tqual(0U);
# 390
Cyc_Absyn_fatptr_type(_tmp286,_tmp285,_tmp284,
# 392
Cyc_Tcutil_any_bool(& _tmpB1));}));}))&& !({
void*_tmp281=_tmpD3;Cyc_Tcutil_unify(_tmp281,({void*_tmp280=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp27C=({struct Cyc_Core_Opt*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->v=& Cyc_Tcutil_rk;_tmpCD;});Cyc_Absyn_new_evar(_tmp27C,tvs);}));void*_tmp27F=({
# 395
struct Cyc_Core_Opt*_tmp27D=({struct Cyc_Core_Opt*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->v=& Cyc_Tcutil_rk;_tmpCE;});Cyc_Absyn_new_evar(_tmp27D,tvs);});
# 393
struct Cyc_Absyn_Tqual _tmp27E=
# 396
Cyc_Absyn_const_tqual(0U);
# 393
Cyc_Absyn_fatptr_type(_tmp280,_tmp27F,_tmp27E,
# 396
Cyc_Tcutil_any_bool(& _tmpB1));}));}))
({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0U,({
struct _dyneither_ptr _tmp294=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD3));_tmp1F2.f1=_tmp294;});_tmp1F2;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned _tmp296=loc;struct _dyneither_ptr _tmp295=({const char*_tmpD0="second argument of main has type %s instead of char??";_tag_dyneither(_tmpD0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp296,_tmp295,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});};}}};}};};}
# 405
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 412
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpD8=({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0U,_tmp1F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp1F3;});void*_tmpD6[1U];_tmpD6[0]=& _tmpD8;({unsigned _tmp298=loc;struct _dyneither_ptr _tmp297=({const char*_tmpD7="redeclaration of typedef %s";_tag_dyneither(_tmpD7,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp298,_tmp297,_tag_dyneither(_tmpD6,sizeof(void*),1U));});});
return;}
# 417
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp299=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 421
(td->tq).real_const=_tmp299;});}
# 426
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpD9=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpDA=_tmpD9;struct Cyc_Core_Opt**_tmpE2;struct Cyc_Absyn_Kind*_tmpE1;struct Cyc_Core_Opt**_tmpE0;switch(*((int*)_tmpDA)){case 1U: _LL1: _tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpDA)->f1;_LL2:
# 429
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDD=({struct Cyc_String_pa_PrintArg_struct _tmp1F4;_tmp1F4.tag=0U,_tmp1F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);_tmp1F4;});void*_tmpDB[1U];_tmpDB[0]=& _tmpDD;({unsigned _tmp29B=loc;struct _dyneither_ptr _tmp29A=({const char*_tmpDC="type variable %s is not used in typedef definition";_tag_dyneither(_tmpDC,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp29B,_tmp29A,_tag_dyneither(_tmpDB,sizeof(void*),1U));});});
# 432
({struct Cyc_Core_Opt*_tmp29D=({struct Cyc_Core_Opt*_tmpDE=_cycalloc(sizeof(*_tmpDE));({void*_tmp29C=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpDE->v=_tmp29C;});_tmpDE;});*_tmpE0=_tmp29D;});goto _LL0;case 2U: _LL3: _tmpE2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDA)->f1;_tmpE1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDA)->f2;_LL4:
# 439
({struct Cyc_Core_Opt*_tmp29F=({struct Cyc_Core_Opt*_tmpDF=_cycalloc(sizeof(*_tmpDF));({void*_tmp29E=Cyc_Tcutil_kind_to_bound(_tmpE1);_tmpDF->v=_tmp29E;});_tmpDF;});*_tmpE2=_tmp29F;});
goto _LL0;default: _LL5: _LL6:
 continue;}_LL0:;}}
# 445
({struct Cyc_Dict_Dict _tmp2A0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp2A0;});}struct _tuple14{void*f1;void*f2;};
# 448
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 455
struct _RegionHandle _tmpE3=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpE3;_push_region(uprev_rgn);
# 457
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple14*_tmpE4=(struct _tuple14*)rpo->hd;struct _tuple14*_tmpE5=_tmpE4;void*_tmpE7;void*_tmpE6;_LL1: _tmpE7=_tmpE5->f1;_tmpE6=_tmpE5->f2;_LL2:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmpE7);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmpE6);}{
# 463
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 466
struct Cyc_List_List*_tmpE8=fields;for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct Cyc_Absyn_Aggrfield*_tmpE9=(struct Cyc_Absyn_Aggrfield*)_tmpE8->hd;struct Cyc_Absyn_Aggrfield*_tmpEA=_tmpE9;struct _dyneither_ptr*_tmp105;struct Cyc_Absyn_Tqual _tmp104;void*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp100;_LL4: _tmp105=_tmpEA->name;_tmp104=_tmpEA->tq;_tmp103=_tmpEA->type;_tmp102=_tmpEA->width;_tmp101=_tmpEA->attributes;_tmp100=_tmpEA->requires_clause;_LL5:;
# 469
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp105))
({struct Cyc_String_pa_PrintArg_struct _tmpED=({struct Cyc_String_pa_PrintArg_struct _tmp1F5;_tmp1F5.tag=0U,_tmp1F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp105);_tmp1F5;});void*_tmpEB[1U];_tmpEB[0]=& _tmpED;({unsigned _tmp2A2=loc;struct _dyneither_ptr _tmp2A1=({const char*_tmpEC="duplicate member %s";_tag_dyneither(_tmpEC,sizeof(char),20U);});Cyc_Tcutil_terr(_tmp2A2,_tmp2A1,_tag_dyneither(_tmpEB,sizeof(void*),1U));});});
# 473
if(({struct _dyneither_ptr _tmp2A3=(struct _dyneither_ptr)*_tmp105;Cyc_strcmp(_tmp2A3,({const char*_tmpEE="";_tag_dyneither(_tmpEE,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmpEF=_region_malloc(uprev_rgn,sizeof(*_tmpEF));_tmpEF->hd=_tmp105,_tmpEF->tl=prev_fields;_tmpEF;});{
# 476
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 480
if((int)str_or_union == (int)1U ||
 _tmpE8->tl == 0 &&(int)str_or_union == (int)0U)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp103);
# 485
({int _tmp2A4=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpE8->hd)->tq).print_const,_tmp103);(((struct Cyc_Absyn_Aggrfield*)_tmpE8->hd)->tq).real_const=_tmp2A4;});
# 488
Cyc_Tcutil_check_bitfield(loc,te,_tmp103,_tmp102,_tmp105);
# 490
if((unsigned)_tmp100){
if((int)str_or_union != (int)1U)
({void*_tmpF0=0U;({unsigned _tmp2A6=loc;struct _dyneither_ptr _tmp2A5=({const char*_tmpF1="@requires clauses are only allowed on union members";_tag_dyneither(_tmpF1,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2A6,_tmp2A5,_tag_dyneither(_tmpF0,sizeof(void*),0U));});});{
struct Cyc_Tcenv_Tenv*_tmpF2=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(_tmpF2,0,_tmp100);
if(!Cyc_Tcutil_is_integral(_tmp100))
({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp1F6;_tmp1F6.tag=0U,({
# 498
struct _dyneither_ptr _tmp2A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp100->topt)));_tmp1F6.f1=_tmp2A7;});_tmp1F6;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({unsigned _tmp2A9=_tmp100->loc;struct _dyneither_ptr _tmp2A8=({const char*_tmpF4="@requires clause has type %s instead of integral type";_tag_dyneither(_tmpF4,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp2A9,_tmp2A8,_tag_dyneither(_tmpF3,sizeof(void*),1U));});});else{
# 500
({unsigned _tmp2AC=_tmp100->loc;struct Cyc_Tcenv_Tenv*_tmp2AB=te;struct Cyc_List_List*_tmp2AA=tvs;Cyc_Tcutil_check_type(_tmp2AC,_tmp2AB,_tmp2AA,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->tag=9U,_tmpF6->f1=_tmp100;_tmpF6;}));});{
# 502
struct Cyc_List_List*_tmpF7=Cyc_Relations_exp2relns(uprev_rgn,_tmp100);
# 509
if(!Cyc_Relations_consistent_relations(_tmpF7))
({void*_tmpF8=0U;({unsigned _tmp2AE=_tmp100->loc;struct _dyneither_ptr _tmp2AD=({const char*_tmpF9="@requires clause may be unsatisfiable";_tag_dyneither(_tmpF9,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp2AE,_tmp2AD,_tag_dyneither(_tmpF8,sizeof(void*),0U));});});
# 515
{struct Cyc_List_List*_tmpFA=prev_relations;for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmpF7,(struct Cyc_List_List*)_tmpFA->hd)))
({void*_tmpFB=0U;({unsigned _tmp2B0=_tmp100->loc;struct _dyneither_ptr _tmp2AF=({const char*_tmpFC="@requires clause may overlap with previous clauses";_tag_dyneither(_tmpFC,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2B0,_tmp2AF,_tag_dyneither(_tmpFB,sizeof(void*),0U));});});}}
# 520
prev_relations=({struct Cyc_List_List*_tmpFD=_region_malloc(uprev_rgn,sizeof(*_tmpFD));_tmpFD->hd=_tmpF7,_tmpFD->tl=prev_relations;_tmpFD;});};}};}else{
# 523
if(prev_relations != 0)
({void*_tmpFE=0U;({unsigned _tmp2B2=loc;struct _dyneither_ptr _tmp2B1=({const char*_tmpFF="if one field has a @requires clause, they all must";_tag_dyneither(_tmpFF,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2B2,_tmp2B1,_tag_dyneither(_tmpFE,sizeof(void*),0U));});});}};}};
# 457
;_pop_region(uprev_rgn);}
# 529
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 531
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp106=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp107=_tmp106;enum Cyc_Absyn_AliasQual _tmp11C;struct Cyc_Core_Opt**_tmp11B;struct Cyc_Absyn_Kind*_tmp11A;struct Cyc_Core_Opt**_tmp119;struct Cyc_Core_Opt**_tmp118;enum Cyc_Absyn_AliasQual _tmp117;struct Cyc_Core_Opt**_tmp116;switch(*((int*)_tmp107)){case 1U: _LL1: _tmp116=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp107)->f1;_LL2:
# 534
({struct Cyc_Core_Opt*_tmp2B4=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));({void*_tmp2B3=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp108->v=_tmp2B3;});_tmp108;});*_tmp116=_tmp2B4;});continue;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmp118=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f1;_tmp117=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2)->aliasqual;_LL4:
# 536
 if(constrain_top_kind &&(int)_tmp117 == (int)2U)
({struct Cyc_Core_Opt*_tmp2B6=({struct Cyc_Core_Opt*_tmp10A=_cycalloc(sizeof(*_tmp10A));({void*_tmp2B5=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->kind=Cyc_Absyn_BoxKind,_tmp109->aliasqual=Cyc_Absyn_Aliasable;_tmp109;}));_tmp10A->v=_tmp2B5;});_tmp10A;});*_tmp118=_tmp2B6;});else{
# 539
({struct Cyc_Core_Opt*_tmp2B8=({struct Cyc_Core_Opt*_tmp10C=_cycalloc(sizeof(*_tmp10C));({void*_tmp2B7=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->kind=Cyc_Absyn_BoxKind,_tmp10B->aliasqual=_tmp117;_tmp10B;}));_tmp10C->v=_tmp2B7;});_tmp10C;});*_tmp118=_tmp2B8;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmp119=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f1;if(constrain_top_kind){_LL6:
# 542
({struct Cyc_Core_Opt*_tmp2BA=({struct Cyc_Core_Opt*_tmp10E=_cycalloc(sizeof(*_tmp10E));({void*_tmp2B9=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->kind=Cyc_Absyn_BoxKind,_tmp10D->aliasqual=Cyc_Absyn_Aliasable;_tmp10D;}));_tmp10E->v=_tmp2B9;});_tmp10E;});*_tmp119=_tmp2BA;});
continue;}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmp11B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f1;_tmp11A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2;_LL8:
# 545
({struct Cyc_Core_Opt*_tmp2BC=({struct Cyc_Core_Opt*_tmp10F=_cycalloc(sizeof(*_tmp10F));({void*_tmp2BB=Cyc_Tcutil_kind_to_bound(_tmp11A);_tmp10F->v=_tmp2BB;});_tmp10F;});*_tmp11B=_tmp2BC;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp107)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmp11C=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp107)->f1)->aliasqual;_LLA:
# 547
({struct Cyc_String_pa_PrintArg_struct _tmp112=({struct Cyc_String_pa_PrintArg_struct _tmp1F9;_tmp1F9.tag=0U,_tmp1F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*n).f2);_tmp1F9;});struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp1F8;_tmp1F8.tag=0U,_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp1F8;});struct Cyc_String_pa_PrintArg_struct _tmp114=({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0U,({
struct _dyneither_ptr _tmp2BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->kind=Cyc_Absyn_MemKind,_tmp115->aliasqual=_tmp11C;_tmp115;})));_tmp1F7.f1=_tmp2BD;});_tmp1F7;});void*_tmp110[3U];_tmp110[0]=& _tmp112,_tmp110[1]=& _tmp113,_tmp110[2]=& _tmp114;({unsigned _tmp2BF=loc;struct _dyneither_ptr _tmp2BE=({const char*_tmp111="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp111,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2BF,_tmp2BE,_tag_dyneither(_tmp110,sizeof(void*),3U));});});
continue;}else{_LLB: _LLC:
 continue;}}_LL0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 555
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp11D=ad->name;
# 562
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp11E=(void*)atts->hd;void*_tmp11F=_tmp11E;switch(*((int*)_tmp11F)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
# 567
({struct Cyc_String_pa_PrintArg_struct _tmp122=({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0U,({
struct _dyneither_ptr _tmp2C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1FB.f1=_tmp2C0;});_tmp1FB;});struct Cyc_String_pa_PrintArg_struct _tmp123=({struct Cyc_String_pa_PrintArg_struct _tmp1FA;_tmp1FA.tag=0U,_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp11D).f2);_tmp1FA;});void*_tmp120[2U];_tmp120[0]=& _tmp122,_tmp120[1]=& _tmp123;({unsigned _tmp2C2=loc;struct _dyneither_ptr _tmp2C1=({const char*_tmp121="bad attribute %s in %s definition";_tag_dyneither(_tmp121,sizeof(char),34U);});Cyc_Tcutil_terr(_tmp2C2,_tmp2C1,_tag_dyneither(_tmp120,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}}{
# 572
struct Cyc_List_List*_tmp124=ad->tvs;
# 575
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 580
struct _tuple15 _tmp125=({struct _tuple15 _tmp1FF;_tmp1FF.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp2C3=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,_tmp11D);_tmp1FF.f2=_tmp2C3;});_tmp1FF;});struct _tuple15 _tmp126=_tmp125;struct Cyc_List_List*_tmp142;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp140;int _tmp13F;struct Cyc_Absyn_Aggrdecl**_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13B;int _tmp13A;struct Cyc_Absyn_Aggrdecl**_tmp139;if(_tmp126.f1 == 0){if(_tmp126.f2 == 0){_LL8: _LL9:
# 583
 Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp124,0);
# 585
({struct Cyc_Dict_Dict _tmp2C6=({struct Cyc_Dict_Dict _tmp2C5=(te->ae)->aggrdecls;struct _tuple0*_tmp2C4=_tmp11D;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp2C5,_tmp2C4,({struct Cyc_Absyn_Aggrdecl**_tmp127=_cycalloc(sizeof(*_tmp127));*_tmp127=ad;_tmp127;}));});(te->ae)->aggrdecls=_tmp2C6;});
goto _LL7;}else{_LLE: _tmp139=*_tmp126.f2;_LLF: {
# 645
struct Cyc_Absyn_Aggrdecl*_tmp138=Cyc_Tcdecl_merge_aggrdecl(*_tmp139,ad,loc,Cyc_Tc_tc_msg);
if(_tmp138 == 0)
return;else{
# 649
Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp124,0);
# 652
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp11D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 655
*_tmp139=_tmp138;
ad=_tmp138;
goto _LL7;}}}}else{if(_tmp126.f2 == 0){_LLA: _tmp13D=(_tmp126.f1)->exist_vars;_tmp13C=(_tmp126.f1)->rgn_po;_tmp13B=(_tmp126.f1)->fields;_tmp13A=(_tmp126.f1)->tagged;_LLB: {
# 590
struct Cyc_Absyn_Aggrdecl**_tmp128=({struct Cyc_Absyn_Aggrdecl**_tmp131=_cycalloc(sizeof(*_tmp131));({struct Cyc_Absyn_Aggrdecl*_tmp2C7=({struct Cyc_Absyn_Aggrdecl*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->kind=ad->kind,_tmp130->sc=Cyc_Absyn_Extern,_tmp130->name=ad->name,_tmp130->tvs=_tmp124,_tmp130->impl=0,_tmp130->attributes=ad->attributes,_tmp130->expected_mem_kind=0;_tmp130;});*_tmp131=_tmp2C7;});_tmp131;});
# 592
({struct Cyc_Dict_Dict _tmp2C8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp11D,_tmp128);(te->ae)->aggrdecls=_tmp2C8;});
# 597
Cyc_Tcutil_check_unique_tvars(loc,_tmp13D);
Cyc_Tcutil_add_tvar_identities(_tmp13D);
# 601
if(_tmp13A &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp129=0U;({unsigned _tmp2CA=loc;struct _dyneither_ptr _tmp2C9=({const char*_tmp12A="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp12A,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2CA,_tmp2C9,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2CF=te;unsigned _tmp2CE=loc;enum Cyc_Absyn_AggrKind _tmp2CD=ad->kind;struct Cyc_List_List*_tmp2CC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp124,_tmp13D);struct Cyc_List_List*_tmp2CB=_tmp13C;Cyc_Tc_tcAggrImpl(_tmp2CF,_tmp2CE,_tmp2CD,_tmp2CC,_tmp2CB,_tmp13B);});
# 605
Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp124,0);
# 608
Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp13D,1);
# 610
if((int)ad->kind == (int)Cyc_Absyn_UnionA && !_tmp13A){
# 613
struct Cyc_List_List*f=_tmp13B;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp12D=({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0U,_tmp1FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);_tmp1FE;});struct Cyc_String_pa_PrintArg_struct _tmp12E=({struct Cyc_String_pa_PrintArg_struct _tmp1FD;_tmp1FD.tag=0U,_tmp1FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp11D).f2);_tmp1FD;});struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0U,({
struct _dyneither_ptr _tmp2D0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));_tmp1FC.f1=_tmp2D0;});_tmp1FC;});void*_tmp12B[3U];_tmp12B[0]=& _tmp12D,_tmp12B[1]=& _tmp12E,_tmp12B[2]=& _tmp12F;({unsigned _tmp2D2=loc;struct _dyneither_ptr _tmp2D1=({const char*_tmp12C="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp12C,sizeof(char),98U);});Cyc_Tcutil_warn(_tmp2D2,_tmp2D1,_tag_dyneither(_tmp12B,sizeof(void*),3U));});});}}
# 619
*_tmp128=ad;
goto _LL7;}}else{_LLC: _tmp142=(_tmp126.f1)->exist_vars;_tmp141=(_tmp126.f1)->rgn_po;_tmp140=(_tmp126.f1)->fields;_tmp13F=(_tmp126.f1)->tagged;_tmp13E=*_tmp126.f2;_LLD:
# 623
 if((int)ad->kind != (int)(*_tmp13E)->kind)
({void*_tmp132=0U;({unsigned _tmp2D4=loc;struct _dyneither_ptr _tmp2D3=({const char*_tmp133="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp133,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2D4,_tmp2D3,_tag_dyneither(_tmp132,sizeof(void*),0U));});});{
# 626
struct Cyc_Absyn_Aggrdecl*_tmp134=*_tmp13E;
# 628
({struct Cyc_Absyn_Aggrdecl*_tmp2D5=({struct Cyc_Absyn_Aggrdecl*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->kind=ad->kind,_tmp135->sc=Cyc_Absyn_Extern,_tmp135->name=ad->name,_tmp135->tvs=_tmp124,_tmp135->impl=0,_tmp135->attributes=ad->attributes,_tmp135->expected_mem_kind=0;_tmp135;});*_tmp13E=_tmp2D5;});
# 634
Cyc_Tcutil_check_unique_tvars(loc,_tmp142);
Cyc_Tcutil_add_tvar_identities(_tmp142);
# 637
if(_tmp13F &&(int)ad->kind == (int)Cyc_Absyn_StructA)
({void*_tmp136=0U;({unsigned _tmp2D7=loc;struct _dyneither_ptr _tmp2D6=({const char*_tmp137="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp137,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2D7,_tmp2D6,_tag_dyneither(_tmp136,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2DC=te;unsigned _tmp2DB=loc;enum Cyc_Absyn_AggrKind _tmp2DA=ad->kind;struct Cyc_List_List*_tmp2D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp124,_tmp142);struct Cyc_List_List*_tmp2D8=_tmp141;Cyc_Tc_tcAggrImpl(_tmp2DC,_tmp2DB,_tmp2DA,_tmp2D9,_tmp2D8,_tmp140);});
# 641
*_tmp13E=_tmp134;
_tmp139=_tmp13E;goto _LLF;};}}_LL7:;};};}
# 662
static void Cyc_Tc_rule_out_mem_and_unique(unsigned loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp143=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp144=_tmp143;enum Cyc_Absyn_AliasQual _tmp161;enum Cyc_Absyn_KindQual _tmp160;struct Cyc_Core_Opt**_tmp15F;enum Cyc_Absyn_KindQual _tmp15E;struct Cyc_Core_Opt**_tmp15D;struct Cyc_Core_Opt**_tmp15C;struct Cyc_Core_Opt**_tmp15B;struct Cyc_Core_Opt**_tmp15A;struct Cyc_Core_Opt**_tmp159;struct Cyc_Core_Opt**_tmp158;switch(*((int*)_tmp144)){case 1U: _LL1: _tmp158=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL2:
 _tmp159=_tmp158;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmp159=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL4:
# 667
 _tmp15A=_tmp159;goto _LL6;case Cyc_Absyn_Aliasable: _LL5: _tmp15A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL6:
# 669
({struct Cyc_Core_Opt*_tmp2DE=({struct Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));({void*_tmp2DD=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp145->v=_tmp2DD;});_tmp145;});*_tmp15A=_tmp2DE;});goto _LL0;case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmp15B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL8:
 _tmp15C=_tmp15B;goto _LLA;case Cyc_Absyn_Aliasable: _LL9: _tmp15C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LLA:
# 672
({struct Cyc_Core_Opt*_tmp2E0=({struct Cyc_Core_Opt*_tmp146=_cycalloc(sizeof(*_tmp146));({void*_tmp2DF=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp146->v=_tmp2DF;});_tmp146;});*_tmp15C=_tmp2E0;});goto _LL0;case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmp15D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LLC:
# 674
({struct Cyc_Core_Opt*_tmp2E2=({struct Cyc_Core_Opt*_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp2E1=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp147->v=_tmp2E1;});_tmp147;});*_tmp15D=_tmp2E2;});goto _LL0;case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmp15F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_tmp15E=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->kind;_LL10:
# 678
 _tmp160=_tmp15E;goto _LL12;}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 676
({struct Cyc_String_pa_PrintArg_struct _tmp14A=({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0U,_tmp201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp201;});struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0U,_tmp200.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp200;});void*_tmp148[2U];_tmp148[0]=& _tmp14A,_tmp148[1]=& _tmp14B;({unsigned _tmp2E4=loc;struct _dyneither_ptr _tmp2E3=({const char*_tmp149="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp149,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2E4,_tmp2E3,_tag_dyneither(_tmp148,sizeof(void*),2U));});});
goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmp160=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->kind;_LL12:
# 680
({struct Cyc_String_pa_PrintArg_struct _tmp14E=({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0U,_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp204;});struct Cyc_String_pa_PrintArg_struct _tmp14F=({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0U,_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp203;});struct Cyc_String_pa_PrintArg_struct _tmp150=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0U,({
# 682
struct _dyneither_ptr _tmp2E5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->kind=_tmp160,_tmp151->aliasqual=Cyc_Absyn_Unique;_tmp151;})));_tmp202.f1=_tmp2E5;});_tmp202;});void*_tmp14C[3U];_tmp14C[0]=& _tmp14E,_tmp14C[1]=& _tmp14F,_tmp14C[2]=& _tmp150;({unsigned _tmp2E7=loc;struct _dyneither_ptr _tmp2E6=({const char*_tmp14D="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp14D,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2E7,_tmp2E6,_tag_dyneither(_tmp14C,sizeof(void*),3U));});});goto _LL0;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmp161=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->aliasqual;_LL14:
# 684
({struct Cyc_String_pa_PrintArg_struct _tmp154=({struct Cyc_String_pa_PrintArg_struct _tmp207;_tmp207.tag=0U,_tmp207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp207;});struct Cyc_String_pa_PrintArg_struct _tmp155=({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0U,_tmp206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp206;});struct Cyc_String_pa_PrintArg_struct _tmp156=({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0U,({
# 686
struct _dyneither_ptr _tmp2E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->kind=Cyc_Absyn_MemKind,_tmp157->aliasqual=_tmp161;_tmp157;})));_tmp205.f1=_tmp2E8;});_tmp205;});void*_tmp152[3U];_tmp152[0]=& _tmp154,_tmp152[1]=& _tmp155,_tmp152[2]=& _tmp156;({unsigned _tmp2EA=loc;struct _dyneither_ptr _tmp2E9=({const char*_tmp153="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp153,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2EA,_tmp2E9,_tag_dyneither(_tmp152,sizeof(void*),3U));});});goto _LL0;}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 692
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 700
{struct Cyc_List_List*_tmp162=fields;for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
struct Cyc_Absyn_Datatypefield*_tmp163=(struct Cyc_Absyn_Datatypefield*)_tmp162->hd;
struct Cyc_List_List*typs=_tmp163->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp163->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 705
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp166=({struct Cyc_String_pa_PrintArg_struct _tmp208;_tmp208.tag=0U,({
# 708
struct _dyneither_ptr _tmp2EB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp163->name));_tmp208.f1=_tmp2EB;});_tmp208;});void*_tmp164[1U];_tmp164[0]=& _tmp166;({unsigned _tmp2ED=_tmp163->loc;struct _dyneither_ptr _tmp2EC=({const char*_tmp165="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp165,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2ED,_tmp2EC,_tag_dyneither(_tmp164,sizeof(void*),1U));});});
({int _tmp2EE=
Cyc_Tcutil_extract_const_from_typedef(_tmp163->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 709
((*((struct _tuple16*)typs->hd)).f1).real_const=_tmp2EE;});}}}{
# 715
struct Cyc_List_List*fields2;
if(is_extensible){
# 718
int _tmp167=1;
struct Cyc_List_List*_tmp168=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp167,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp167)
fields2=_tmp168;else{
# 723
fields2=0;}}else{
# 725
struct _RegionHandle _tmp169=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp169;_push_region(uprev_rgn);
# 727
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp16A=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp16A->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp16D=({struct Cyc_String_pa_PrintArg_struct _tmp20A;_tmp20A.tag=0U,_tmp20A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp16A->name).f2);_tmp20A;});struct Cyc_String_pa_PrintArg_struct _tmp16E=({struct Cyc_String_pa_PrintArg_struct _tmp209;_tmp209.tag=0U,_tmp209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);_tmp209;});void*_tmp16B[2U];_tmp16B[0]=& _tmp16D,_tmp16B[1]=& _tmp16E;({unsigned _tmp2F0=_tmp16A->loc;struct _dyneither_ptr _tmp2EF=({const char*_tmp16C="duplicate field name %s in %s";_tag_dyneither(_tmp16C,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp2F0,_tmp2EF,_tag_dyneither(_tmp16B,sizeof(void*),2U));});});else{
# 733
prev_fields=({struct Cyc_List_List*_tmp16F=_region_malloc(uprev_rgn,sizeof(*_tmp16F));_tmp16F->hd=(*_tmp16A->name).f2,_tmp16F->tl=prev_fields;_tmp16F;});}
# 735
if((int)_tmp16A->sc != (int)Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0U,_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp16A->name).f2);_tmp20B;});void*_tmp170[1U];_tmp170[0]=& _tmp172;({unsigned _tmp2F2=loc;struct _dyneither_ptr _tmp2F1=({const char*_tmp171="ignoring scope of field %s";_tag_dyneither(_tmp171,sizeof(char),27U);});Cyc_Tcutil_warn(_tmp2F2,_tmp2F1,_tag_dyneither(_tmp170,sizeof(void*),1U));});});
_tmp16A->sc=Cyc_Absyn_Public;}}}
# 740
fields2=fields;}
# 727
;_pop_region(uprev_rgn);}
# 742
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 745
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp188="@extensible datatype";_tag_dyneither(_tmp188,sizeof(char),21U);}):({const char*_tmp189="datatype";_tag_dyneither(_tmp189,sizeof(char),9U);});
# 752
struct Cyc_List_List*tvs=tud->tvs;
# 755
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 761
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 771 "tc.cyc"
{struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt == 0 && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->tag=Cyc_Dict_Absent;_tmp176;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 777
({union Cyc_Absyn_Nmspace _tmp2F3=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp2F3;});}
# 772
;_pop_handler();}else{void*_tmp174=(void*)Cyc_Core_get_exn_thrown();void*_tmp177=_tmp174;void*_tmp17A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp177)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 781
struct Cyc_Absyn_Datatypedecl***_tmp178=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);
tud_opt=(unsigned)_tmp178?({struct Cyc_Absyn_Datatypedecl***_tmp179=_cycalloc(sizeof(*_tmp179));*_tmp179=*_tmp178;_tmp179;}): 0;
goto _LL0;}}else{_LL3: _tmp17A=_tmp177;_LL4:(int)_rethrow(_tmp17A);}_LL0:;}};}{
# 788
struct _tuple17 _tmp17B=({struct _tuple17 _tmp20C;_tmp20C.f1=tud->fields,_tmp20C.f2=tud_opt;_tmp20C;});struct _tuple17 _tmp17C=_tmp17B;struct Cyc_List_List**_tmp187;struct Cyc_Absyn_Datatypedecl**_tmp186;struct Cyc_List_List**_tmp185;struct Cyc_Absyn_Datatypedecl**_tmp184;if(_tmp17C.f1 == 0){if(_tmp17C.f2 == 0){_LL6: _LL7:
# 791
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp2F6=({struct Cyc_Dict_Dict _tmp2F5=(te->ae)->datatypedecls;struct _tuple0*_tmp2F4=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp2F5,_tmp2F4,({struct Cyc_Absyn_Datatypedecl**_tmp17D=_cycalloc(sizeof(*_tmp17D));*_tmp17D=tud;_tmp17D;}));});(te->ae)->datatypedecls=_tmp2F6;});
goto _LL5;}else{_LLC: _tmp184=*_tmp17C.f2;_LLD: {
# 824
struct Cyc_Absyn_Datatypedecl*_tmp183=Cyc_Tcdecl_merge_datatypedecl(*_tmp184,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(_tmp183 == 0)
return;else{
# 829
*_tmp184=_tmp183;
goto _LL5;}}}}else{if(_tmp17C.f2 == 0){_LL8: _tmp185=(struct Cyc_List_List**)&(_tmp17C.f1)->v;_LL9: {
# 796
struct Cyc_Absyn_Datatypedecl**_tmp17E=({struct Cyc_Absyn_Datatypedecl**_tmp180=_cycalloc(sizeof(*_tmp180));({struct Cyc_Absyn_Datatypedecl*_tmp2F7=({struct Cyc_Absyn_Datatypedecl*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->sc=Cyc_Absyn_Extern,_tmp17F->name=tud->name,_tmp17F->tvs=tvs,_tmp17F->fields=0,_tmp17F->is_extensible=tud->is_extensible;_tmp17F;});*_tmp180=_tmp2F7;});_tmp180;});
# 798
({struct Cyc_Dict_Dict _tmp2F8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,_tmp17E);(te->ae)->datatypedecls=_tmp2F8;});
# 801
({struct Cyc_List_List*_tmp2F9=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp185,tvs,tud);*_tmp185=_tmp2F9;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*_tmp17E=tud;
goto _LL5;}}else{_LLA: _tmp187=(struct Cyc_List_List**)&(_tmp17C.f1)->v;_tmp186=*_tmp17C.f2;_LLB: {
# 806
struct Cyc_Absyn_Datatypedecl*_tmp181=*_tmp186;
# 809
if((!tud->is_extensible &&(unsigned)_tmp181)&& _tmp181->is_extensible)
tud->is_extensible=1;
# 812
({struct Cyc_Absyn_Datatypedecl*_tmp2FA=({struct Cyc_Absyn_Datatypedecl*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->sc=Cyc_Absyn_Extern,_tmp182->name=tud->name,_tmp182->tvs=tvs,_tmp182->fields=0,_tmp182->is_extensible=tud->is_extensible;_tmp182;});*_tmp186=_tmp2FA;});
# 816
({struct Cyc_List_List*_tmp2FB=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp187,tvs,tud);*_tmp187=_tmp2FB;});
# 819
*_tmp186=_tmp181;
_tmp184=_tmp186;goto _LLD;}}}_LL5:;};};}
# 835
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 842
if(ed->fields != 0){struct _RegionHandle _tmp18A=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp18A;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp18B=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 848
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp18B->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp18E=({struct Cyc_String_pa_PrintArg_struct _tmp20D;_tmp20D.tag=0U,_tmp20D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18B->name).f2);_tmp20D;});void*_tmp18C[1U];_tmp18C[0]=& _tmp18E;({unsigned _tmp2FD=_tmp18B->loc;struct _dyneither_ptr _tmp2FC=({const char*_tmp18D="duplicate enum constructor %s";_tag_dyneither(_tmp18D,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp2FD,_tmp2FC,_tag_dyneither(_tmp18C,sizeof(void*),1U));});});else{
# 851
prev_fields=({struct Cyc_List_List*_tmp18F=_region_malloc(uprev_rgn,sizeof(*_tmp18F));_tmp18F->hd=(*_tmp18B->name).f2,_tmp18F->tl=prev_fields;_tmp18F;});}
# 853
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,_tmp18B->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp192=({struct Cyc_String_pa_PrintArg_struct _tmp20E;_tmp20E.tag=0U,_tmp20E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18B->name).f2);_tmp20E;});void*_tmp190[1U];_tmp190[0]=& _tmp192;({unsigned _tmp2FF=_tmp18B->loc;struct _dyneither_ptr _tmp2FE=({const char*_tmp191="enum field name %s shadows global name";_tag_dyneither(_tmp191,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp2FF,_tmp2FE,_tag_dyneither(_tmp190,sizeof(void*),1U));});});
# 856
if(_tmp18B->tag == 0){
({struct Cyc_Absyn_Exp*_tmp300=Cyc_Absyn_uint_exp(tag_count,_tmp18B->loc);_tmp18B->tag=_tmp300;});
++ tag_count;}else{
# 861
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp18B->tag))){
struct _tuple10 _tmp193=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp18B->tag));struct _tuple10 _tmp194=_tmp193;unsigned _tmp196;int _tmp195;_LL1: _tmp196=_tmp194.f1;_tmp195=_tmp194.f2;_LL2:;
if(_tmp195)tag_count=_tmp196 + (unsigned)1;}}}}
# 843
;_pop_region(uprev_rgn);}
# 869
{struct _handler_cons _tmp197;_push_handler(& _tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){
{struct Cyc_Absyn_Enumdecl**_tmp19A=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);
struct Cyc_Absyn_Enumdecl*_tmp19B=Cyc_Tcdecl_merge_enumdecl(*_tmp19A,ed,loc,Cyc_Tc_tc_msg);
if(_tmp19B == 0){_npop_handler(0U);return;}
*_tmp19A=_tmp19B;}
# 870
;_pop_handler();}else{void*_tmp198=(void*)Cyc_Core_get_exn_thrown();void*_tmp19C=_tmp198;void*_tmp19F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp19C)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 875
struct Cyc_Absyn_Enumdecl**_tmp19D=({struct Cyc_Absyn_Enumdecl**_tmp19E=_cycalloc(sizeof(*_tmp19E));*_tmp19E=ed;_tmp19E;});
({struct Cyc_Dict_Dict _tmp301=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,_tmp19D);(te->ae)->enumdecls=_tmp301;});
goto _LL3;}}else{_LL6: _tmp19F=_tmp19C;_LL7:(int)_rethrow(_tmp19F);}_LL3:;}};}
# 881
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1A0=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1A0->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1A0->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1A3=({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0U,_tmp210.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp210;});struct Cyc_String_pa_PrintArg_struct _tmp1A4=({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0U,_tmp20F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1A0->name).f2);_tmp20F;});void*_tmp1A1[2U];_tmp1A1[0]=& _tmp1A3,_tmp1A1[1]=& _tmp1A4;({unsigned _tmp303=loc;struct _dyneither_ptr _tmp302=({const char*_tmp1A2="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1A2,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp303,_tmp302,_tag_dyneither(_tmp1A1,sizeof(void*),2U));});});}}}
# 891
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1A5=sc;switch(_tmp1A5){case Cyc_Absyn_Static: _LL1: _LL2:
# 894
 if(!in_include)
({void*_tmp1A6=0U;({unsigned _tmp305=loc;struct _dyneither_ptr _tmp304=({const char*_tmp1A7="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1A7,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp305,_tmp304,_tag_dyneither(_tmp1A6,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 898
({void*_tmp1A8=0U;({unsigned _tmp307=loc;struct _dyneither_ptr _tmp306=({const char*_tmp1A9="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1A9,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp307,_tmp306,_tag_dyneither(_tmp1A8,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;case Cyc_Absyn_ExternC: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
# 905
({void*_tmp1AA=0U;({unsigned _tmp309=loc;struct _dyneither_ptr _tmp308=({const char*_tmp1AB="nested extern \"C\" declaration";_tag_dyneither(_tmp1AB,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp309,_tmp308,_tag_dyneither(_tmp1AA,sizeof(void*),0U));});});
return 1;}_LL0:;}
# 915
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 920
struct Cyc_List_List*last=0;
# 922
struct Cyc_List_List*_tmp1AC=ds0;for(0;_tmp1AC != 0;(last=_tmp1AC,_tmp1AC=_tmp1AC->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1AC->hd;
unsigned loc=d->loc;
# 926
void*_tmp1AD=d->r;void*_tmp1AE=_tmp1AD;struct Cyc_List_List*_tmp1CA;struct Cyc_List_List*_tmp1C9;struct Cyc_List_List*_tmp1C8;struct Cyc_List_List*_tmp1C7;union Cyc_Absyn_Nmspace _tmp1C6;struct _dyneither_ptr*_tmp1C5;struct Cyc_List_List*_tmp1C4;struct _dyneither_ptr*_tmp1C3;struct Cyc_List_List*_tmp1C2;struct Cyc_Absyn_Enumdecl*_tmp1C1;struct Cyc_Absyn_Datatypedecl*_tmp1C0;struct Cyc_Absyn_Aggrdecl*_tmp1BF;struct Cyc_Absyn_Typedefdecl*_tmp1BE;struct Cyc_Absyn_Fndecl*_tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BC;switch(*((int*)_tmp1AE)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
# 929
({void*_tmp1AF=0U;({unsigned _tmp30B=loc;struct _dyneither_ptr _tmp30A=({const char*_tmp1B0="top level let-declarations are not implemented";_tag_dyneither(_tmp1B0,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp30B,_tmp30A,_tag_dyneither(_tmp1AF,sizeof(void*),0U));});});
goto _LL0;case 4U: _LL5: _LL6:
# 932
({void*_tmp1B1=0U;({unsigned _tmp30D=loc;struct _dyneither_ptr _tmp30C=({const char*_tmp1B2="top level region declarations are not implemented";_tag_dyneither(_tmp1B2,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp30D,_tmp30C,_tag_dyneither(_tmp1B1,sizeof(void*),0U));});});
goto _LL0;case 0U: _LL7: _tmp1BC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL8:
# 935
 if(in_externC && Cyc_Tc_okay_externC(d->loc,_tmp1BC->sc,te->in_extern_c_include))
_tmp1BC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,_tmp1BC,check_var_init,te->in_extern_c_include,exports);
goto _LL0;case 1U: _LL9: _tmp1BD=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LLA:
# 940
 if(in_externC && Cyc_Tc_okay_externC(d->loc,_tmp1BD->sc,te->in_extern_c_include))
_tmp1BD->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,_tmp1BD,exports);
goto _LL0;case 8U: _LLB: _tmp1BE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LLC:
# 945
 _tmp1BE->extern_c=te->in_extern_c_include;
# 949
Cyc_Tc_tcTypedefdecl(te,loc,_tmp1BE);
goto _LL0;case 5U: _LLD: _tmp1BF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LLE:
# 952
 if(in_externC && Cyc_Tc_okay_externC(d->loc,_tmp1BF->sc,te->in_extern_c_include))
_tmp1BF->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp1BF);
goto _LL0;case 6U: _LLF: _tmp1C0=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL10:
# 957
 if(in_externC && Cyc_Tc_okay_externC(d->loc,_tmp1C0->sc,te->in_extern_c_include))
_tmp1C0->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,_tmp1C0);
goto _LL0;case 7U: _LL11: _tmp1C1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL12:
# 962
 if(in_externC && Cyc_Tc_okay_externC(d->loc,_tmp1C1->sc,te->in_extern_c_include))
_tmp1C1->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp1C1);
goto _LL0;case 13U: _LL13: _LL14:
# 967
({void*_tmp1B3=0U;({unsigned _tmp30F=d->loc;struct _dyneither_ptr _tmp30E=({const char*_tmp1B4="spurious __cyclone_port_on__";_tag_dyneither(_tmp1B4,sizeof(char),29U);});Cyc_Tcutil_warn(_tmp30F,_tmp30E,_tag_dyneither(_tmp1B3,sizeof(void*),0U));});});
goto _LL0;case 14U: _LL15: _LL16:
# 970
 goto _LL0;case 15U: _LL17: _LL18:
# 972
 te=Cyc_Tcenv_enter_tempest(te);
goto _LL0;case 16U: _LL19: _LL1A:
# 975
 te=Cyc_Tcenv_clear_tempest(te);
goto _LL0;case 9U: _LL1B: _tmp1C3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_tmp1C2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AE)->f2;_LL1C: {
# 979
struct Cyc_List_List*_tmp1B5=te->ns;
({struct Cyc_List_List*_tmp311=({struct Cyc_List_List*_tmp310=_tmp1B5;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp310,({struct Cyc_List_List*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->hd=_tmp1C3,_tmp1B6->tl=0;_tmp1B6;}));});te->ns=_tmp311;});
Cyc_Tc_tc_decls(te,_tmp1C2,in_externC,check_var_init,exports);
te->ns=_tmp1B5;
goto _LL0;}case 10U: _LL1D: _tmp1C6=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1)->f1;_tmp1C5=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1)->f2;_tmp1C4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AE)->f2;_LL1E:
# 987
 Cyc_Tc_tc_decls(te,_tmp1C4,in_externC,check_var_init,exports);
goto _LL0;case 11U: _LL1F: _tmp1C7=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL20:
# 991
 Cyc_Tc_tc_decls(te,_tmp1C7,1,check_var_init,exports);
goto _LL0;default: _LL21: _tmp1CA=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_tmp1C9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AE)->f2;_tmp1C8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AE)->f3;_LL22: {
# 997
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C8,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1B7=Cyc_Tcenv_enter_extern_c_include(te);
Cyc_Tc_tc_decls(_tmp1B7,_tmp1CA,1,check_var_init,& newexs);
# 1002
for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
struct _tuple11*_tmp1B8=(struct _tuple11*)_tmp1C8->hd;
if(!(*_tmp1B8).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1BB=({struct Cyc_String_pa_PrintArg_struct _tmp211;_tmp211.tag=0U,({
struct _dyneither_ptr _tmp312=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*_tmp1B8).f2));_tmp211.f1=_tmp312;});_tmp211;});void*_tmp1B9[1U];_tmp1B9[0]=& _tmp1BB;({unsigned _tmp314=(*_tmp1B8).f1;struct _dyneither_ptr _tmp313=({const char*_tmp1BA="%s is exported but not defined";_tag_dyneither(_tmp1BA,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp314,_tmp313,_tag_dyneither(_tmp1B9,sizeof(void*),1U));});});}
# 1008
goto _LL0;}}_LL0:;}}
# 1013
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict _tmp1CB=Cyc_Callgraph_compute_callgraph(ds);
# 1018
struct Cyc_Dict_Dict _tmp1CC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict g))Cyc_Graph_scc)(_tmp1CB);
# 1021
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);};}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1039 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1041
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1045
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1CD=d->r;void*_tmp1CE=_tmp1CD;struct Cyc_List_List**_tmp1D3;struct Cyc_List_List**_tmp1D2;struct Cyc_List_List**_tmp1D1;struct Cyc_List_List**_tmp1D0;struct Cyc_Absyn_Vardecl*_tmp1CF;switch(*((int*)_tmp1CE)){case 0U: _LL1: _tmp1CF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1CE)->f1;_LL2:
# 1050
 if((env->in_cinclude ||(int)_tmp1CF->sc != (int)Cyc_Absyn_Extern &&(int)_tmp1CF->sc != (int)Cyc_Absyn_ExternC)|| !({
struct _tuple0*_tmp315=_tmp1CF->name;Cyc_Absyn_qvar_cmp(_tmp315,Cyc_Absyn_uniquergn_qvar());}))
# 1053
return 1;
# 1055
return(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,_tmp1CF->name)).f2;case 11U: _LL3: _tmp1D0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1CE)->f1;_LL4:
 _tmp1D1=_tmp1D0;goto _LL6;case 10U: _LL5: _tmp1D1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1CE)->f2;_LL6:
 _tmp1D2=_tmp1D1;goto _LL8;case 9U: _LL7: _tmp1D2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1CE)->f2;_LL8:
# 1059
({struct Cyc_List_List*_tmp316=Cyc_Tc_treeshake_f(env,*_tmp1D2);*_tmp1D2=_tmp316;});
return 1;case 12U: _LL9: _tmp1D3=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1CE)->f1;_LLA: {
# 1062
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp317=Cyc_Tc_treeshake_f(env,*_tmp1D3);*_tmp1D3=_tmp317;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 1071
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1D4=({struct Cyc_Tc_TreeshakeEnv _tmp212;_tmp212.in_cinclude=0,_tmp212.ordinaries=(te->ae)->ordinaries;_tmp212;});
return Cyc_Tc_treeshake_f(& _tmp1D4,ds);}
