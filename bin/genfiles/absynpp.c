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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc___cycFILE;
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
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
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
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
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
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_tmp555=Cyc_Absynpp_curr_namespace;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp555,({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v,_tmp1->tl=0;_tmp1;}));});}
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
return({struct _dyneither_ptr _tmp558=({const char*_tmp6F="";_tag_dyneither(_tmp6F,sizeof(char),1U);});struct _dyneither_ptr _tmp557=({const char*_tmp70=" ";_tag_dyneither(_tmp70,sizeof(char),2U);});struct _dyneither_ptr _tmp556=({const char*_tmp71=" ";_tag_dyneither(_tmp71,sizeof(char),2U);});Cyc_PP_egroup(_tmp558,_tmp557,_tmp556,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));});}
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
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp539;_tmp539.tag=0U,({struct _dyneither_ptr _tmp559=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp92));_tmp539.f1=_tmp559;});_tmp539;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({struct _dyneither_ptr _tmp55A=({const char*_tmp8F="<=%s";_tag_dyneither(_tmp8F,sizeof(char),5U);});Cyc_aprintf(_tmp55A,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});}_LL0:;}
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
return({struct _dyneither_ptr _tmp55D=({const char*_tmp99="<";_tag_dyneither(_tmp99,sizeof(char),2U);});struct _dyneither_ptr _tmp55C=({const char*_tmp9A=">";_tag_dyneither(_tmp9A,sizeof(char),2U);});struct _dyneither_ptr _tmp55B=({const char*_tmp9B=",";_tag_dyneither(_tmp9B,sizeof(char),2U);});Cyc_PP_egroup(_tmp55D,_tmp55C,_tmp55B,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));});}
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
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp53B;_tmp53B.tag=0U,_tmp53B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring);_tmp53B;});struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U,({struct _dyneither_ptr _tmp55E=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9C,sizeof(char),1));_tmp53A.f1=_tmp55E;});_tmp53A;});void*_tmpA1[2U];_tmpA1[0]=& _tmpA3,_tmpA1[1]=& _tmpA4;({struct _dyneither_ptr _tmp55F=({const char*_tmpA2="`G%s%s";_tag_dyneither(_tmpA2,sizeof(char),7U);});Cyc_aprintf(_tmp55F,_tag_dyneither(_tmpA1,sizeof(void*),2U));});}));}
# 441
return Cyc_PP_text(_tmp9C);}
# 444
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAA;switch(*((int*)_tmpA7)){case 1U: _LL1: _LL2:
 goto _LL4;case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL3: _LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{goto _LL7;}}else{_LL7: _tmpAA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7)->f1;_LL8:
# 449
 return({struct Cyc_PP_Doc*_tmpA8[3U];({struct Cyc_PP_Doc*_tmp562=Cyc_Absynpp_tvar2doc(tv);_tmpA8[0]=_tmp562;}),({struct Cyc_PP_Doc*_tmp561=Cyc_PP_text(({const char*_tmpA9="::";_tag_dyneither(_tmpA9,sizeof(char),3U);}));_tmpA8[1]=_tmp561;}),({struct Cyc_PP_Doc*_tmp560=Cyc_Absynpp_kind2doc(_tmpAA);_tmpA8[2]=_tmp560;});Cyc_PP_cat(_tag_dyneither(_tmpA8,sizeof(struct Cyc_PP_Doc*),3U));});}default: _LL5: _tmpAB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7)->f2;_LL6:
# 448
 _tmpAA=_tmpAB;goto _LL8;}_LL0:;}
# 453
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _dyneither_ptr _tmp565=({const char*_tmpAC="<";_tag_dyneither(_tmpAC,sizeof(char),2U);});struct _dyneither_ptr _tmp564=({const char*_tmpAD=">";_tag_dyneither(_tmpAD,sizeof(char),2U);});struct _dyneither_ptr _tmp563=({const char*_tmpAE=",";_tag_dyneither(_tmpAE,sizeof(char),2U);});Cyc_PP_egroup(_tmp565,_tmp564,_tmp563,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));});}
# 457
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);
return({struct _dyneither_ptr _tmp568=({const char*_tmpAF="<";_tag_dyneither(_tmpAF,sizeof(char),2U);});struct _dyneither_ptr _tmp567=({const char*_tmpB0=">";_tag_dyneither(_tmpB0,sizeof(char),2U);});struct _dyneither_ptr _tmp566=({const char*_tmpB1=",";_tag_dyneither(_tmpB1,sizeof(char),2U);});Cyc_PP_egroup(_tmp568,_tmp567,_tmp566,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 463
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 467
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _dyneither_ptr _tmp56B=({const char*_tmpB2="(";_tag_dyneither(_tmpB2,sizeof(char),2U);});struct _dyneither_ptr _tmp56A=({const char*_tmpB3=")";_tag_dyneither(_tmpB3,sizeof(char),2U);});struct _dyneither_ptr _tmp569=({const char*_tmpB4=",";_tag_dyneither(_tmpB4,sizeof(char),2U);});Cyc_PP_group(_tmp56B,_tmp56A,_tmp569,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));});}
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
return({struct Cyc_PP_Doc*_tmpBD[3U];({struct Cyc_PP_Doc*_tmp571=Cyc_PP_text(({const char*_tmpBE=" __declspec(";_tag_dyneither(_tmpBE,sizeof(char),13U);}));_tmpBD[0]=_tmp571;}),({
struct Cyc_PP_Doc*_tmp570=({struct _dyneither_ptr _tmp56F=({const char*_tmpBF="";_tag_dyneither(_tmpBF,sizeof(char),1U);});struct _dyneither_ptr _tmp56E=({const char*_tmpC0="";_tag_dyneither(_tmpC0,sizeof(char),1U);});struct _dyneither_ptr _tmp56D=({const char*_tmpC1=" ";_tag_dyneither(_tmpC1,sizeof(char),2U);});Cyc_PP_group(_tmp56F,_tmp56E,_tmp56D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));});_tmpBD[1]=_tmp570;}),({
struct Cyc_PP_Doc*_tmp56C=Cyc_PP_text(({const char*_tmpC2=")";_tag_dyneither(_tmpC2,sizeof(char),2U);}));_tmpBD[2]=_tmp56C;});Cyc_PP_cat(_tag_dyneither(_tmpBD,sizeof(struct Cyc_PP_Doc*),3U));});}
# 507
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 511
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();{
enum Cyc_Cyclone_C_Compilers _tmpC3=Cyc_Cyclone_c_compiler;if(_tmpC3 == Cyc_Cyclone_Vc_c){_LL1: _LL2:
 return Cyc_Absynpp_noncallconv2doc(atts);}else{_LL3: _LL4:
# 516
 return({struct Cyc_PP_Doc*_tmpC4[2U];({struct Cyc_PP_Doc*_tmp576=Cyc_PP_text(({const char*_tmpC5=" __attribute__";_tag_dyneither(_tmpC5,sizeof(char),15U);}));_tmpC4[0]=_tmp576;}),({
struct Cyc_PP_Doc*_tmp575=({struct _dyneither_ptr _tmp574=({const char*_tmpC6="((";_tag_dyneither(_tmpC6,sizeof(char),3U);});struct _dyneither_ptr _tmp573=({const char*_tmpC7="))";_tag_dyneither(_tmpC7,sizeof(char),3U);});struct _dyneither_ptr _tmp572=({const char*_tmpC8=",";_tag_dyneither(_tmpC8,sizeof(char),2U);});Cyc_PP_group(_tmp574,_tmp573,_tmp572,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));});_tmpC4[1]=_tmp575;});Cyc_PP_cat(_tag_dyneither(_tmpC4,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;};}
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
return({struct Cyc_PP_Doc*_tmpD4[3U];({struct Cyc_PP_Doc*_tmp579=Cyc_Absynpp_lb();_tmpD4[0]=_tmp579;}),({struct Cyc_PP_Doc*_tmp578=Cyc_PP_seq(sep,ss);_tmpD4[1]=_tmp578;}),({struct Cyc_PP_Doc*_tmp577=Cyc_Absynpp_rb();_tmpD4[2]=_tmp577;});Cyc_PP_cat(_tag_dyneither(_tmpD4,sizeof(struct Cyc_PP_Doc*),3U));});}
# 578
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD5=(void*)tms->hd;void*_tmpD6=_tmpD5;struct Cyc_List_List*_tmpED;switch(*((int*)_tmpD6)){case 0U: _LL1: _LL2:
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp57B=Cyc_stderr;struct _dyneither_ptr _tmp57A=({const char*_tmpD8="Carray_mod ";_tag_dyneither(_tmpD8,sizeof(char),12U);});Cyc_fprintf(_tmp57B,_tmp57A,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmpD9=0U;({struct Cyc___cycFILE*_tmp57D=Cyc_stderr;struct _dyneither_ptr _tmp57C=({const char*_tmpDA="ConstArray_mod ";_tag_dyneither(_tmpDA,sizeof(char),16U);});Cyc_fprintf(_tmp57D,_tmp57C,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});goto _LL0;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->tag == 1U){_LL5: _tmpED=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD6)->f1)->f1;_LL6:
# 584
({void*_tmpDB=0U;({struct Cyc___cycFILE*_tmp57F=Cyc_stderr;struct _dyneither_ptr _tmp57E=({const char*_tmpDC="Function_mod(";_tag_dyneither(_tmpDC,sizeof(char),14U);});Cyc_fprintf(_tmp57F,_tmp57E,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
for(0;_tmpED != 0;_tmpED=_tmpED->tl){
struct _dyneither_ptr*_tmpDD=(*((struct _tuple8*)_tmpED->hd)).f1;
if(_tmpDD == 0)({void*_tmpDE=0U;({struct Cyc___cycFILE*_tmp581=Cyc_stderr;struct _dyneither_ptr _tmp580=({const char*_tmpDF="?";_tag_dyneither(_tmpDF,sizeof(char),2U);});Cyc_fprintf(_tmp581,_tmp580,_tag_dyneither(_tmpDE,sizeof(void*),0U));});});else{
({void*_tmpE0=0U;({struct Cyc___cycFILE*_tmp583=Cyc_stderr;struct _dyneither_ptr _tmp582=*_tmpDD;Cyc_fprintf(_tmp583,_tmp582,_tag_dyneither(_tmpE0,sizeof(void*),0U));});});}
if(_tmpED->tl != 0)({void*_tmpE1=0U;({struct Cyc___cycFILE*_tmp585=Cyc_stderr;struct _dyneither_ptr _tmp584=({const char*_tmpE2=",";_tag_dyneither(_tmpE2,sizeof(char),2U);});Cyc_fprintf(_tmp585,_tmp584,_tag_dyneither(_tmpE1,sizeof(void*),0U));});});}
# 591
({void*_tmpE3=0U;({struct Cyc___cycFILE*_tmp587=Cyc_stderr;struct _dyneither_ptr _tmp586=({const char*_tmpE4=") ";_tag_dyneither(_tmpE4,sizeof(char),3U);});Cyc_fprintf(_tmp587,_tmp586,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});
goto _LL0;}else{_LL7: _LL8:
# 594
({void*_tmpE5=0U;({struct Cyc___cycFILE*_tmp589=Cyc_stderr;struct _dyneither_ptr _tmp588=({const char*_tmpE6="Function_mod()";_tag_dyneither(_tmpE6,sizeof(char),15U);});Cyc_fprintf(_tmp589,_tmp588,_tag_dyneither(_tmpE5,sizeof(void*),0U));});});goto _LL0;}case 5U: _LL9: _LLA:
({void*_tmpE7=0U;({struct Cyc___cycFILE*_tmp58B=Cyc_stderr;struct _dyneither_ptr _tmp58A=({const char*_tmpE8="Attributes_mod ";_tag_dyneither(_tmpE8,sizeof(char),16U);});Cyc_fprintf(_tmp58B,_tmp58A,_tag_dyneither(_tmpE7,sizeof(void*),0U));});});goto _LL0;case 4U: _LLB: _LLC:
({void*_tmpE9=0U;({struct Cyc___cycFILE*_tmp58D=Cyc_stderr;struct _dyneither_ptr _tmp58C=({const char*_tmpEA="TypeParams_mod ";_tag_dyneither(_tmpEA,sizeof(char),16U);});Cyc_fprintf(_tmp58D,_tmp58C,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmpEB=0U;({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _dyneither_ptr _tmp58E=({const char*_tmpEC="Pointer_mod ";_tag_dyneither(_tmpEC,sizeof(char),13U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_dyneither(_tmpEB,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 599
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp591=Cyc_stderr;struct _dyneither_ptr _tmp590=({const char*_tmpEF="\n";_tag_dyneither(_tmpEF,sizeof(char),2U);});Cyc_fprintf(_tmp591,_tmp590,_tag_dyneither(_tmpEE,sizeof(void*),0U));});});}
# 602
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 604
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp12E[3U];({struct Cyc_PP_Doc*_tmp593=Cyc_PP_text(({const char*_tmp12F="(";_tag_dyneither(_tmp12F,sizeof(char),2U);}));_tmp12E[0]=_tmp593;}),_tmp12E[1]=rest,({struct Cyc_PP_Doc*_tmp592=Cyc_PP_text(({const char*_tmp130=")";_tag_dyneither(_tmp130,sizeof(char),2U);}));_tmp12E[2]=_tmp592;});Cyc_PP_cat(_tag_dyneither(_tmp12E,sizeof(struct Cyc_PP_Doc*),3U));});
void*_tmpF0=(void*)tms->hd;void*_tmpF1=_tmpF0;void*_tmp12D;void*_tmp12C;void*_tmp12B;void*_tmp12A;struct Cyc_Absyn_Tqual _tmp129;struct Cyc_List_List*_tmp128;unsigned int _tmp127;int _tmp126;struct Cyc_List_List*_tmp125;void*_tmp124;struct Cyc_Absyn_Exp*_tmp123;void*_tmp122;void*_tmp121;switch(*((int*)_tmpF1)){case 0U: _LL1: _tmp121=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL2:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF2[2U];_tmpF2[0]=rest,Cyc_Absyn_type2bool(0,_tmp121)?({
struct Cyc_PP_Doc*_tmp595=Cyc_PP_text(({const char*_tmpF3="[]@zeroterm";_tag_dyneither(_tmpF3,sizeof(char),12U);}));_tmpF2[1]=_tmp595;}):({struct Cyc_PP_Doc*_tmp594=Cyc_PP_text(({const char*_tmpF4="[]";_tag_dyneither(_tmpF4,sizeof(char),3U);}));_tmpF2[1]=_tmp594;});Cyc_PP_cat(_tag_dyneither(_tmpF2,sizeof(struct Cyc_PP_Doc*),2U));});case 1U: _LL3: _tmp123=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp122=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL4:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmpF5[4U];_tmpF5[0]=rest,({struct Cyc_PP_Doc*_tmp599=Cyc_PP_text(({const char*_tmpF6="[";_tag_dyneither(_tmpF6,sizeof(char),2U);}));_tmpF5[1]=_tmp599;}),({struct Cyc_PP_Doc*_tmp598=Cyc_Absynpp_exp2doc(_tmp123);_tmpF5[2]=_tmp598;}),
Cyc_Absyn_type2bool(0,_tmp122)?({struct Cyc_PP_Doc*_tmp597=Cyc_PP_text(({const char*_tmpF7="]@zeroterm";_tag_dyneither(_tmpF7,sizeof(char),11U);}));_tmpF5[3]=_tmp597;}):({struct Cyc_PP_Doc*_tmp596=Cyc_PP_text(({const char*_tmpF8="]";_tag_dyneither(_tmpF8,sizeof(char),2U);}));_tmpF5[3]=_tmp596;});Cyc_PP_cat(_tag_dyneither(_tmpF5,sizeof(struct Cyc_PP_Doc*),4U));});case 3U: _LL5: _tmp124=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_LL6:
# 618
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmpF9=_tmp124;struct Cyc_List_List*_tmp107;unsigned int _tmp106;struct Cyc_List_List*_tmp105;int _tmp104;struct Cyc_Absyn_VarargInfo*_tmp103;void*_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->tag == 1U){_LLE: _tmp105=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp104=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_tmp103=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f3;_tmp102=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f4;_tmp101=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f5;_tmp100=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f6;_tmpFF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpF9)->f7;_LLF:
# 621
 return({struct Cyc_PP_Doc*_tmpFA[2U];_tmpFA[0]=rest,({struct Cyc_PP_Doc*_tmp59A=Cyc_Absynpp_funargs2doc(_tmp105,_tmp104,_tmp103,_tmp102,_tmp101,_tmp100,_tmpFF);_tmpFA[1]=_tmp59A;});Cyc_PP_cat(_tag_dyneither(_tmpFA,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL10: _tmp107=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f1;_tmp106=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpF9)->f2;_LL11:
# 623
 return({struct Cyc_PP_Doc*_tmpFB[2U];_tmpFB[0]=rest,({struct Cyc_PP_Doc*_tmp59E=({struct _dyneither_ptr _tmp59D=({const char*_tmpFC="(";_tag_dyneither(_tmpFC,sizeof(char),2U);});struct _dyneither_ptr _tmp59C=({const char*_tmpFD=")";_tag_dyneither(_tmpFD,sizeof(char),2U);});struct _dyneither_ptr _tmp59B=({const char*_tmpFE=",";_tag_dyneither(_tmpFE,sizeof(char),2U);});Cyc_PP_group(_tmp59D,_tmp59C,_tmp59B,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp107));});_tmpFB[1]=_tmp59E;});Cyc_PP_cat(_tag_dyneither(_tmpFB,sizeof(struct Cyc_PP_Doc*),2U));});}_LLD:;};case 5U: _LL7: _tmp125=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LL8: {
# 626
enum Cyc_Cyclone_C_Compilers _tmp108=Cyc_Cyclone_c_compiler;if(_tmp108 == Cyc_Cyclone_Gcc_c){_LL13: _LL14:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_tmp109[2U];_tmp109[0]=rest,({struct Cyc_PP_Doc*_tmp59F=Cyc_Absynpp_atts2doc(_tmp125);_tmp109[1]=_tmp59F;});Cyc_PP_cat(_tag_dyneither(_tmp109,sizeof(struct Cyc_PP_Doc*),2U));});}else{_LL15: _LL16:
# 631
 if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_tmp10A[2U];({struct Cyc_PP_Doc*_tmp5A0=Cyc_Absynpp_callconv2doc(_tmp125);_tmp10A[0]=_tmp5A0;}),_tmp10A[1]=rest;Cyc_PP_cat(_tag_dyneither(_tmp10A,sizeof(struct Cyc_PP_Doc*),2U));});
return rest;}_LL12:;}case 4U: _LL9: _tmp128=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1;_tmp127=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_tmp126=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1)->f3;_LLA:
# 636
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp126)
return({struct Cyc_PP_Doc*_tmp10B[2U];_tmp10B[0]=rest,({struct Cyc_PP_Doc*_tmp5A1=Cyc_Absynpp_ktvars2doc(_tmp128);_tmp10B[1]=_tmp5A1;});Cyc_PP_cat(_tag_dyneither(_tmp10B,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 640
return({struct Cyc_PP_Doc*_tmp10C[2U];_tmp10C[0]=rest,({struct Cyc_PP_Doc*_tmp5A2=Cyc_Absynpp_tvars2doc(_tmp128);_tmp10C[1]=_tmp5A2;});Cyc_PP_cat(_tag_dyneither(_tmp10C,sizeof(struct Cyc_PP_Doc*),2U));});}default: _LLB: _tmp12D=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).rgn;_tmp12C=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).nullable;_tmp12B=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).bounds;_tmp12A=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f1).zero_term;_tmp129=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1)->f2;_LLC: {
# 644
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp129);
{void*_tmp10D=Cyc_Tcutil_compress(_tmp12B);void*_tmp10E=_tmp10D;struct Cyc_Absyn_Exp*_tmp119;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)){case 14U: _LL18: _LL19:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2 != 0){if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2)->hd)->tag == 9U){_LL1A: _tmp119=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f2)->hd)->f1;_LL1B:
# 652
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp12C)?({const char*_tmp10F="*";_tag_dyneither(_tmp10F,sizeof(char),2U);}):({const char*_tmp110="@";_tag_dyneither(_tmp110,sizeof(char),2U);}));{
struct _tuple11 _tmp111=Cyc_Evexp_eval_const_uint_exp(_tmp119);struct _tuple11 _tmp112=_tmp111;unsigned int _tmp115;int _tmp114;_LL1F: _tmp115=_tmp112.f1;_tmp114=_tmp112.f2;_LL20:;
if(!_tmp114  || _tmp115 != (unsigned int)1)
ptr=({struct Cyc_PP_Doc*_tmp113[4U];_tmp113[0]=ptr,({struct Cyc_PP_Doc*_tmp5A5=Cyc_Absynpp_lb();_tmp113[1]=_tmp5A5;}),({struct Cyc_PP_Doc*_tmp5A4=Cyc_Absynpp_exp2doc(_tmp119);_tmp113[2]=_tmp5A4;}),({struct Cyc_PP_Doc*_tmp5A3=Cyc_Absynpp_rb();_tmp113[3]=_tmp5A3;});Cyc_PP_cat(_tag_dyneither(_tmp113,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;};}else{goto _LL1C;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C: _LL1D:
# 658
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,_tmp12C)?({const char*_tmp116="*";_tag_dyneither(_tmp116,sizeof(char),2U);}):({const char*_tmp117="@";_tag_dyneither(_tmp117,sizeof(char),2U);}));
ptr=({struct Cyc_PP_Doc*_tmp118[4U];_tmp118[0]=ptr,({struct Cyc_PP_Doc*_tmp5A8=Cyc_Absynpp_lb();_tmp118[1]=_tmp5A8;}),({struct Cyc_PP_Doc*_tmp5A7=Cyc_Absynpp_typ2doc(_tmp12B);_tmp118[2]=_tmp5A7;}),({struct Cyc_PP_Doc*_tmp5A6=Cyc_Absynpp_rb();_tmp118[3]=_tmp5A6;});Cyc_PP_cat(_tag_dyneither(_tmp118,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL17;}_LL17:;}
# 663
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && Cyc_Absyn_type2bool(0,_tmp12A))
ztd=Cyc_PP_text(({const char*_tmp11A="@zeroterm";_tag_dyneither(_tmp11A,sizeof(char),10U);}));else{
if(is_char_ptr  && !Cyc_Absyn_type2bool(0,_tmp12A))
ztd=Cyc_PP_text(({const char*_tmp11B="@nozeroterm";_tag_dyneither(_tmp11B,sizeof(char),12U);}));}}
# 669
{void*_tmp11C=Cyc_Tcutil_compress(_tmp12D);void*_tmp11D=_tmp11C;switch(*((int*)_tmp11D)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11D)->f1)->tag == 5U){_LL22: _LL23:
 goto _LL21;}else{goto _LL26;}case 1U: _LL24: if(Cyc_Absynpp_print_for_cycdoc){_LL25:
 goto _LL21;}else{goto _LL26;}default: _LL26: _LL27:
 rgd=Cyc_Absynpp_rgn2doc(_tmp12D);}_LL21:;}{
# 674
struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(({const char*_tmp120=" ";_tag_dyneither(_tmp120,sizeof(char),2U);})): mt;
struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(({const char*_tmp11F=" ";_tag_dyneither(_tmp11F,sizeof(char),2U);})): mt;
return({struct Cyc_PP_Doc*_tmp11E[7U];_tmp11E[0]=ptr,_tmp11E[1]=ztd,_tmp11E[2]=rgd,_tmp11E[3]=spacer1,_tmp11E[4]=tqd,_tmp11E[5]=spacer2,_tmp11E[6]=rest;Cyc_PP_cat(_tag_dyneither(_tmp11E,sizeof(struct Cyc_PP_Doc*),7U));});};}}_LL0:;};}
# 680
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp131=Cyc_Tcutil_compress(t);void*_tmp132=_tmp131;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp132)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp132)->f1)){case 5U: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp133="`H";_tag_dyneither(_tmp133,sizeof(char),3U);}));case 6U: _LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp134="`U";_tag_dyneither(_tmp134,sizeof(char),3U);}));case 7U: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp135="`RC";_tag_dyneither(_tmp135,sizeof(char),4U);}));default: goto _LL7;}else{_LL7: _LL8:
 return Cyc_Absynpp_ntyp2doc(t);}_LL0:;}
# 690
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 694
void*_tmp136=Cyc_Tcutil_compress(t);void*_tmp137=_tmp136;struct Cyc_List_List*_tmp13B;void*_tmp13A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2 != 0){_LL1: _tmp13A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2)->hd;_LL2:
({struct Cyc_List_List*_tmp5AA=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));({struct Cyc_PP_Doc*_tmp5A9=Cyc_Absynpp_rgn2doc(_tmp13A);_tmp138->hd=_tmp5A9;}),_tmp138->tl=*rgions;_tmp138;});*rgions=_tmp5AA;});goto _LL0;}else{goto _LL5;}case 9U: _LL3: _tmp13B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f2;_LL4:
# 697
 for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp13B->hd);}
# 700
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_List_List*_tmp5AC=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));({struct Cyc_PP_Doc*_tmp5AB=Cyc_Absynpp_typ2doc(t);_tmp139->hd=_tmp5AB;}),_tmp139->tl=*effects;_tmp139;});*effects=_tmp5AC;});goto _LL0;}_LL0:;}
# 705
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0)
return({struct _dyneither_ptr _tmp5AF=({const char*_tmp13C="";_tag_dyneither(_tmp13C,sizeof(char),1U);});struct _dyneither_ptr _tmp5AE=({const char*_tmp13D="";_tag_dyneither(_tmp13D,sizeof(char),1U);});struct _dyneither_ptr _tmp5AD=({const char*_tmp13E="+";_tag_dyneither(_tmp13E,sizeof(char),2U);});Cyc_PP_group(_tmp5AF,_tmp5AE,_tmp5AD,effects);});else{
# 713
struct Cyc_PP_Doc*_tmp13F=({struct _dyneither_ptr _tmp5B0=({const char*_tmp144=",";_tag_dyneither(_tmp144,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp5B0,rgions);});
return({struct _dyneither_ptr _tmp5B4=({const char*_tmp140="";_tag_dyneither(_tmp140,sizeof(char),1U);});struct _dyneither_ptr _tmp5B3=({const char*_tmp141="";_tag_dyneither(_tmp141,sizeof(char),1U);});struct _dyneither_ptr _tmp5B2=({const char*_tmp142="+";_tag_dyneither(_tmp142,sizeof(char),2U);});Cyc_PP_group(_tmp5B4,_tmp5B3,_tmp5B2,({struct Cyc_List_List*_tmp5B1=effects;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5B1,({struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=_tmp13F,_tmp143->tl=0;_tmp143;}));}));});}}
# 718
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp145=k;if(_tmp145 == Cyc_Absyn_StructA){_LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp146="struct ";_tag_dyneither(_tmp146,sizeof(char),8U);}));}else{_LL3: _LL4:
 return Cyc_PP_text(({const char*_tmp147="union ";_tag_dyneither(_tmp147,sizeof(char),7U);}));}_LL0:;}
# 726
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp148=t;struct Cyc_Absyn_Datatypedecl*_tmp1BD;struct Cyc_Absyn_Enumdecl*_tmp1BC;struct Cyc_Absyn_Aggrdecl*_tmp1BB;struct _tuple0*_tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Typedefdecl*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B6;enum Cyc_Absyn_AggrKind _tmp1B5;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B2;struct Cyc_Core_Opt*_tmp1B1;void*_tmp1B0;int _tmp1AF;struct Cyc_Core_Opt*_tmp1AE;void*_tmp1AD;void*_tmp1AC;void*_tmp1AB;void*_tmp1AA;struct _dyneither_ptr _tmp1A9;struct _tuple0*_tmp1A8;struct Cyc_List_List*_tmp1A7;union Cyc_Absyn_AggrInfo _tmp1A6;struct Cyc_List_List*_tmp1A5;int _tmp1A4;enum Cyc_Absyn_Sign _tmp1A3;enum Cyc_Absyn_Size_of _tmp1A2;union Cyc_Absyn_DatatypeFieldInfo _tmp1A1;struct Cyc_List_List*_tmp1A0;union Cyc_Absyn_DatatypeInfo _tmp19F;struct Cyc_List_List*_tmp19E;switch(*((int*)_tmp148)){case 4U: _LL1: _LL2:
# 730
 return Cyc_PP_text(({const char*_tmp149="[[[array]]]";_tag_dyneither(_tmp149,sizeof(char),12U);}));case 5U: _LL3: _LL4:
 return Cyc_PP_nil_doc();case 3U: _LL5: _LL6:
 return Cyc_PP_nil_doc();case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)){case 0U: _LL7: _LL8:
# 734
 s=Cyc_PP_text(({const char*_tmp14A="void";_tag_dyneither(_tmp14A,sizeof(char),5U);}));goto _LL0;case 18U: _LLD: _tmp19F=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp19E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2;_LLE:
# 765 "absynpp.cyc"
{union Cyc_Absyn_DatatypeInfo _tmp152=_tmp19F;struct _tuple0*_tmp15B;int _tmp15A;struct _tuple0*_tmp159;int _tmp158;if((_tmp152.UnknownDatatype).tag == 1){_LL48: _tmp159=((_tmp152.UnknownDatatype).val).name;_tmp158=((_tmp152.UnknownDatatype).val).is_extensible;_LL49:
# 767
 _tmp15B=_tmp159;_tmp15A=_tmp158;goto _LL4B;}else{_LL4A: _tmp15B=(*(_tmp152.KnownDatatype).val)->name;_tmp15A=(*(_tmp152.KnownDatatype).val)->is_extensible;_LL4B: {
# 769
struct Cyc_PP_Doc*_tmp153=Cyc_PP_text(({const char*_tmp157="datatype ";_tag_dyneither(_tmp157,sizeof(char),10U);}));
struct Cyc_PP_Doc*_tmp154=_tmp15A?Cyc_PP_text(({const char*_tmp156="@extensible ";_tag_dyneither(_tmp156,sizeof(char),13U);})): Cyc_PP_nil_doc();
s=({struct Cyc_PP_Doc*_tmp155[4U];_tmp155[0]=_tmp154,_tmp155[1]=_tmp153,({struct Cyc_PP_Doc*_tmp5B6=Cyc_Absynpp_qvar2doc(_tmp15B);_tmp155[2]=_tmp5B6;}),({struct Cyc_PP_Doc*_tmp5B5=Cyc_Absynpp_tps2doc(_tmp19E);_tmp155[3]=_tmp5B5;});Cyc_PP_cat(_tag_dyneither(_tmp155,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL47;}}_LL47:;}
# 774
goto _LL0;case 19U: _LLF: _tmp1A1=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp1A0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2;_LL10:
# 776
{union Cyc_Absyn_DatatypeFieldInfo _tmp15C=_tmp1A1;struct _tuple0*_tmp167;int _tmp166;struct _tuple0*_tmp165;struct _tuple0*_tmp164;struct _tuple0*_tmp163;int _tmp162;if((_tmp15C.UnknownDatatypefield).tag == 1){_LL4D: _tmp164=((_tmp15C.UnknownDatatypefield).val).datatype_name;_tmp163=((_tmp15C.UnknownDatatypefield).val).field_name;_tmp162=((_tmp15C.UnknownDatatypefield).val).is_extensible;_LL4E:
# 778
 _tmp167=_tmp164;_tmp166=_tmp162;_tmp165=_tmp163;goto _LL50;}else{_LL4F: _tmp167=(((_tmp15C.KnownDatatypefield).val).f1)->name;_tmp166=(((_tmp15C.KnownDatatypefield).val).f1)->is_extensible;_tmp165=(((_tmp15C.KnownDatatypefield).val).f2)->name;_LL50: {
# 781
struct Cyc_PP_Doc*_tmp15D=Cyc_PP_text(_tmp166?({const char*_tmp160="@extensible datatype ";_tag_dyneither(_tmp160,sizeof(char),22U);}):({const char*_tmp161="datatype ";_tag_dyneither(_tmp161,sizeof(char),10U);}));
s=({struct Cyc_PP_Doc*_tmp15E[4U];_tmp15E[0]=_tmp15D,({struct Cyc_PP_Doc*_tmp5B9=Cyc_Absynpp_qvar2doc(_tmp167);_tmp15E[1]=_tmp5B9;}),({struct Cyc_PP_Doc*_tmp5B8=Cyc_PP_text(({const char*_tmp15F=".";_tag_dyneither(_tmp15F,sizeof(char),2U);}));_tmp15E[2]=_tmp5B8;}),({struct Cyc_PP_Doc*_tmp5B7=Cyc_Absynpp_qvar2doc(_tmp165);_tmp15E[3]=_tmp5B7;});Cyc_PP_cat(_tag_dyneither(_tmp15E,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL4C;}}_LL4C:;}
# 785
goto _LL0;case 1U: _LL11: _tmp1A3=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp1A2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f2;_LL12: {
# 787
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
{enum Cyc_Absyn_Sign _tmp168=_tmp1A3;switch(_tmp168){case Cyc_Absyn_None: _LL52: _LL53:
 goto _LL55;case Cyc_Absyn_Signed: _LL54: _LL55:
 sns=({const char*_tmp169="";_tag_dyneither(_tmp169,sizeof(char),1U);});goto _LL51;default: _LL56: _LL57:
 sns=({const char*_tmp16A="unsigned ";_tag_dyneither(_tmp16A,sizeof(char),10U);});goto _LL51;}_LL51:;}
# 794
{enum Cyc_Absyn_Size_of _tmp16B=_tmp1A2;switch(_tmp16B){case Cyc_Absyn_Char_sz: _LL59: _LL5A:
# 796
{enum Cyc_Absyn_Sign _tmp16C=_tmp1A3;switch(_tmp16C){case Cyc_Absyn_None: _LL66: _LL67:
 sns=({const char*_tmp16D="";_tag_dyneither(_tmp16D,sizeof(char),1U);});goto _LL65;case Cyc_Absyn_Signed: _LL68: _LL69:
 sns=({const char*_tmp16E="signed ";_tag_dyneither(_tmp16E,sizeof(char),8U);});goto _LL65;default: _LL6A: _LL6B:
 sns=({const char*_tmp16F="unsigned ";_tag_dyneither(_tmp16F,sizeof(char),10U);});goto _LL65;}_LL65:;}
# 801
ts=({const char*_tmp170="char";_tag_dyneither(_tmp170,sizeof(char),5U);});
goto _LL58;case Cyc_Absyn_Short_sz: _LL5B: _LL5C:
 ts=({const char*_tmp171="short";_tag_dyneither(_tmp171,sizeof(char),6U);});goto _LL58;case Cyc_Absyn_Int_sz: _LL5D: _LL5E:
 ts=({const char*_tmp172="int";_tag_dyneither(_tmp172,sizeof(char),4U);});goto _LL58;case Cyc_Absyn_Long_sz: _LL5F: _LL60:
 ts=({const char*_tmp173="long";_tag_dyneither(_tmp173,sizeof(char),5U);});goto _LL58;case Cyc_Absyn_LongLong_sz: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64:
# 808
{enum Cyc_Cyclone_C_Compilers _tmp174=Cyc_Cyclone_c_compiler;if(_tmp174 == Cyc_Cyclone_Gcc_c){_LL6D: _LL6E:
 ts=({const char*_tmp175="long long";_tag_dyneither(_tmp175,sizeof(char),10U);});goto _LL6C;}else{_LL6F: _LL70:
 ts=({const char*_tmp176="__int64";_tag_dyneither(_tmp176,sizeof(char),8U);});goto _LL6C;}_LL6C:;}
# 812
goto _LL58;}_LL58:;}
# 814
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,_tmp53D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns);_tmp53D;});struct Cyc_String_pa_PrintArg_struct _tmp17A=({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0U,_tmp53C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts);_tmp53C;});void*_tmp177[2U];_tmp177[0]=& _tmp179,_tmp177[1]=& _tmp17A;({struct _dyneither_ptr _tmp5BA=({const char*_tmp178="%s%s";_tag_dyneither(_tmp178,sizeof(char),5U);});Cyc_aprintf(_tmp5BA,_tag_dyneither(_tmp177,sizeof(void*),2U));});}));
goto _LL0;}case 2U: _LL13: _tmp1A4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL14:
# 817
 if(_tmp1A4 == 0)
s=Cyc_PP_text(({const char*_tmp17B="float";_tag_dyneither(_tmp17B,sizeof(char),6U);}));else{
if(_tmp1A4 == 1)
s=Cyc_PP_text(({const char*_tmp17C="double";_tag_dyneither(_tmp17C,sizeof(char),7U);}));else{
# 822
s=Cyc_PP_text(({const char*_tmp17D="long double";_tag_dyneither(_tmp17D,sizeof(char),12U);}));}}
goto _LL0;case 20U: _LL17: _tmp1A6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_tmp1A5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2;_LL18: {
# 828
struct _tuple10 _tmp17F=Cyc_Absyn_aggr_kinded_name(_tmp1A6);struct _tuple10 _tmp180=_tmp17F;enum Cyc_Absyn_AggrKind _tmp183;struct _tuple0*_tmp182;_LL72: _tmp183=_tmp180.f1;_tmp182=_tmp180.f2;_LL73:;
s=({struct Cyc_PP_Doc*_tmp181[3U];({struct Cyc_PP_Doc*_tmp5BD=Cyc_Absynpp_aggr_kind2doc(_tmp183);_tmp181[0]=_tmp5BD;}),({struct Cyc_PP_Doc*_tmp5BC=Cyc_Absynpp_qvar2doc(_tmp182);_tmp181[1]=_tmp5BC;}),({struct Cyc_PP_Doc*_tmp5BB=Cyc_Absynpp_tps2doc(_tmp1A5);_tmp181[2]=_tmp5BB;});Cyc_PP_cat(_tag_dyneither(_tmp181,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}case 16U: _LL1B: _tmp1A7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL1C:
# 837
 s=({struct Cyc_PP_Doc*_tmp185[4U];({struct Cyc_PP_Doc*_tmp5C1=Cyc_PP_text(({const char*_tmp186="enum ";_tag_dyneither(_tmp186,sizeof(char),6U);}));_tmp185[0]=_tmp5C1;}),({struct Cyc_PP_Doc*_tmp5C0=Cyc_Absynpp_lb();_tmp185[1]=_tmp5C0;}),({struct Cyc_PP_Doc*_tmp5BF=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp1A7));_tmp185[2]=_tmp5BF;}),({struct Cyc_PP_Doc*_tmp5BE=Cyc_Absynpp_rb();_tmp185[3]=_tmp5BE;});Cyc_PP_cat(_tag_dyneither(_tmp185,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 15U: _LL1D: _tmp1A8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL1E:
# 839
 s=({struct Cyc_PP_Doc*_tmp187[2U];({struct Cyc_PP_Doc*_tmp5C3=Cyc_PP_text(({const char*_tmp188="enum ";_tag_dyneither(_tmp188,sizeof(char),6U);}));_tmp187[0]=_tmp5C3;}),({struct Cyc_PP_Doc*_tmp5C2=Cyc_Absynpp_qvar2doc(_tmp1A8);_tmp187[1]=_tmp5C2;});Cyc_PP_cat(_tag_dyneither(_tmp187,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 17U: _LL23: _tmp1A9=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1)->f1;_LL24:
# 845
 s=Cyc_PP_text(_tmp1A9);goto _LL0;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2 != 0){_LL27: _tmp1AA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2)->hd;_LL28:
# 855
 s=({struct Cyc_PP_Doc*_tmp190[3U];({struct Cyc_PP_Doc*_tmp5C6=Cyc_PP_text(({const char*_tmp191="region_t<";_tag_dyneither(_tmp191,sizeof(char),10U);}));_tmp190[0]=_tmp5C6;}),({struct Cyc_PP_Doc*_tmp5C5=Cyc_Absynpp_rgn2doc(_tmp1AA);_tmp190[1]=_tmp5C5;}),({struct Cyc_PP_Doc*_tmp5C4=Cyc_PP_text(({const char*_tmp192=">";_tag_dyneither(_tmp192,sizeof(char),2U);}));_tmp190[2]=_tmp5C4;});Cyc_PP_cat(_tag_dyneither(_tmp190,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}else{goto _LL45;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2 != 0){_LL29: _tmp1AB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2)->hd;_LL2A:
# 857
 s=({struct Cyc_PP_Doc*_tmp193[3U];({struct Cyc_PP_Doc*_tmp5C9=Cyc_PP_text(({const char*_tmp194="tag_t<";_tag_dyneither(_tmp194,sizeof(char),7U);}));_tmp193[0]=_tmp5C9;}),({struct Cyc_PP_Doc*_tmp5C8=Cyc_Absynpp_typ2doc(_tmp1AB);_tmp193[1]=_tmp5C8;}),({struct Cyc_PP_Doc*_tmp5C7=Cyc_PP_text(({const char*_tmp195=">";_tag_dyneither(_tmp195,sizeof(char),2U);}));_tmp193[2]=_tmp5C7;});Cyc_PP_cat(_tag_dyneither(_tmp193,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}else{goto _LL45;}case 6U: _LL2B: _LL2C:
 goto _LL2E;case 5U: _LL2D: _LL2E:
 goto _LL30;case 7U: _LL2F: _LL30:
# 861
 s=Cyc_Absynpp_rgn2doc(t);goto _LL0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2 != 0){_LL31: _tmp1AC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f2)->hd;_LL32:
# 863
 s=({struct Cyc_PP_Doc*_tmp196[3U];({struct Cyc_PP_Doc*_tmp5CC=Cyc_PP_text(({const char*_tmp197="regions(";_tag_dyneither(_tmp197,sizeof(char),9U);}));_tmp196[0]=_tmp5CC;}),({struct Cyc_PP_Doc*_tmp5CB=Cyc_Absynpp_typ2doc(_tmp1AC);_tmp196[1]=_tmp5CB;}),({struct Cyc_PP_Doc*_tmp5CA=Cyc_PP_text(({const char*_tmp198=")";_tag_dyneither(_tmp198,sizeof(char),2U);}));_tmp196[2]=_tmp5CA;});Cyc_PP_cat(_tag_dyneither(_tmp196,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}else{_LL45: _tmp1AD=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp148)->f1;_LL46:
# 882
 s=Cyc_PP_text(({const char*_tmp19D="*bad type application*";_tag_dyneither(_tmp19D,sizeof(char),23U);}));goto _LL0;}case 8U: _LL33: _LL34:
# 864
 goto _LL36;case 9U: _LL35: _LL36:
# 866
 s=Cyc_Absynpp_eff2doc(t);goto _LL0;case 11U: _LL3D: _LL3E:
# 874
 s=Cyc_PP_text(({const char*_tmp199="@true";_tag_dyneither(_tmp199,sizeof(char),6U);}));goto _LL0;case 12U: _LL3F: _LL40:
# 876
 s=Cyc_PP_text(({const char*_tmp19A="@false";_tag_dyneither(_tmp19A,sizeof(char),7U);}));goto _LL0;case 13U: _LL41: _LL42:
# 878
 s=Cyc_PP_text(({const char*_tmp19B="@thin";_tag_dyneither(_tmp19B,sizeof(char),6U);}));goto _LL0;default: _LL43: _LL44:
# 880
 s=Cyc_PP_text(({const char*_tmp19C="@fat";_tag_dyneither(_tmp19C,sizeof(char),5U);}));goto _LL0;}case 1U: _LL9: _tmp1B1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f1;_tmp1B0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f2;_tmp1AF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f3;_tmp1AE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp148)->f4;_LLA:
# 736 "absynpp.cyc"
 if(_tmp1B0 != 0)
# 738
return Cyc_Absynpp_ntyp2doc(_tmp1B0);else{
# 740
struct _dyneither_ptr kindstring=_tmp1B1 == 0?({const char*_tmp14F="K";_tag_dyneither(_tmp14F,sizeof(char),2U);}): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp1B1->v);
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp14D=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,_tmp53F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring);_tmp53F;});struct Cyc_Int_pa_PrintArg_struct _tmp14E=({struct Cyc_Int_pa_PrintArg_struct _tmp53E;_tmp53E.tag=1U,_tmp53E.f1=(unsigned long)_tmp1AF;_tmp53E;});void*_tmp14B[2U];_tmp14B[0]=& _tmp14D,_tmp14B[1]=& _tmp14E;({struct _dyneither_ptr _tmp5CD=({const char*_tmp14C="`E%s%d";_tag_dyneither(_tmp14C,sizeof(char),7U);});Cyc_aprintf(_tmp5CD,_tag_dyneither(_tmp14B,sizeof(void*),2U));});}));}
# 757 "absynpp.cyc"
goto _LL0;case 2U: _LLB: _tmp1B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp148)->f1;_LLC:
# 759
 s=Cyc_Absynpp_tvar2doc(_tmp1B2);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp150[3U];_tmp150[0]=s,({struct Cyc_PP_Doc*_tmp5CF=Cyc_PP_text(({const char*_tmp151="::";_tag_dyneither(_tmp151,sizeof(char),3U);}));_tmp150[1]=_tmp5CF;}),({struct Cyc_PP_Doc*_tmp5CE=Cyc_Absynpp_kindbound2doc(_tmp1B2->kind);_tmp150[2]=_tmp5CE;});Cyc_PP_cat(_tag_dyneither(_tmp150,sizeof(struct Cyc_PP_Doc*),3U));});
# 763
goto _LL0;case 6U: _LL15: _tmp1B3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp148)->f1;_LL16:
# 825
 s=({struct Cyc_PP_Doc*_tmp17E[2U];({struct Cyc_PP_Doc*_tmp5D1=Cyc_Absynpp_dollar();_tmp17E[0]=_tmp5D1;}),({struct Cyc_PP_Doc*_tmp5D0=Cyc_Absynpp_args2doc(_tmp1B3);_tmp17E[1]=_tmp5D0;});Cyc_PP_cat(_tag_dyneither(_tmp17E,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 7U: _LL19: _tmp1B5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp148)->f1;_tmp1B4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp148)->f2;_LL1A:
# 832
 s=({struct Cyc_PP_Doc*_tmp184[4U];({struct Cyc_PP_Doc*_tmp5D5=Cyc_Absynpp_aggr_kind2doc(_tmp1B5);_tmp184[0]=_tmp5D5;}),({struct Cyc_PP_Doc*_tmp5D4=Cyc_Absynpp_lb();_tmp184[1]=_tmp5D4;}),({
struct Cyc_PP_Doc*_tmp5D3=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp1B4));_tmp184[2]=_tmp5D3;}),({
struct Cyc_PP_Doc*_tmp5D2=Cyc_Absynpp_rb();_tmp184[3]=_tmp5D2;});Cyc_PP_cat(_tag_dyneither(_tmp184,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 9U: _LL1F: _tmp1B6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp148)->f1;_LL20:
# 841
 s=({struct Cyc_PP_Doc*_tmp189[3U];({struct Cyc_PP_Doc*_tmp5D8=Cyc_PP_text(({const char*_tmp18A="valueof_t(";_tag_dyneither(_tmp18A,sizeof(char),11U);}));_tmp189[0]=_tmp5D8;}),({struct Cyc_PP_Doc*_tmp5D7=Cyc_Absynpp_exp2doc(_tmp1B6);_tmp189[1]=_tmp5D7;}),({struct Cyc_PP_Doc*_tmp5D6=Cyc_PP_text(({const char*_tmp18B=")";_tag_dyneither(_tmp18B,sizeof(char),2U);}));_tmp189[2]=_tmp5D6;});Cyc_PP_cat(_tag_dyneither(_tmp189,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 11U: _LL21: _tmp1B7=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp148)->f1;_LL22:
# 843
 s=({struct Cyc_PP_Doc*_tmp18C[3U];({struct Cyc_PP_Doc*_tmp5DB=Cyc_PP_text(({const char*_tmp18D="typeof(";_tag_dyneither(_tmp18D,sizeof(char),8U);}));_tmp18C[0]=_tmp5DB;}),({struct Cyc_PP_Doc*_tmp5DA=Cyc_Absynpp_exp2doc(_tmp1B7);_tmp18C[1]=_tmp5DA;}),({struct Cyc_PP_Doc*_tmp5D9=Cyc_PP_text(({const char*_tmp18E=")";_tag_dyneither(_tmp18E,sizeof(char),2U);}));_tmp18C[2]=_tmp5D9;});Cyc_PP_cat(_tag_dyneither(_tmp18C,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 8U: _LL25: _tmp1BA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f1;_tmp1B9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f2;_tmp1B8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp148)->f3;_LL26:
# 852
 s=({struct Cyc_PP_Doc*_tmp18F[2U];({struct Cyc_PP_Doc*_tmp5DD=Cyc_Absynpp_qvar2doc(_tmp1BA);_tmp18F[0]=_tmp5DD;}),({struct Cyc_PP_Doc*_tmp5DC=Cyc_Absynpp_tps2doc(_tmp1B9);_tmp18F[1]=_tmp5DC;});Cyc_PP_cat(_tag_dyneither(_tmp18F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)){case 0U: _LL37: _tmp1BB=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL38:
# 868
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1BB);goto _LL0;case 1U: _LL39: _tmp1BC=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL3A:
# 870
 s=Cyc_Absynpp_enumdecl2doc(_tmp1BC);goto _LL0;default: _LL3B: _tmp1BD=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp148)->f1)->r)->f1;_LL3C:
# 872
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1BD);goto _LL0;}}_LL0:;}
# 884
return s;}
# 887
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 891
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
struct _tuple16*_tmp1BE=cmp;void*_tmp1C2;void*_tmp1C1;_LL1: _tmp1C2=_tmp1BE->f1;_tmp1C1=_tmp1BE->f2;_LL2:;
return({struct Cyc_PP_Doc*_tmp1BF[3U];({struct Cyc_PP_Doc*_tmp5E0=Cyc_Absynpp_rgn2doc(_tmp1C2);_tmp1BF[0]=_tmp5E0;}),({struct Cyc_PP_Doc*_tmp5DF=Cyc_PP_text(({const char*_tmp1C0=" > ";_tag_dyneither(_tmp1C0,sizeof(char),4U);}));_tmp1BF[1]=_tmp5DF;}),({struct Cyc_PP_Doc*_tmp5DE=Cyc_Absynpp_rgn2doc(_tmp1C1);_tmp1BF[2]=_tmp5DE;});Cyc_PP_cat(_tag_dyneither(_tmp1BF,sizeof(struct Cyc_PP_Doc*),3U));});}
# 896
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct _dyneither_ptr _tmp5E3=({const char*_tmp1C3="";_tag_dyneither(_tmp1C3,sizeof(char),1U);});struct _dyneither_ptr _tmp5E2=({const char*_tmp1C4="";_tag_dyneither(_tmp1C4,sizeof(char),1U);});struct _dyneither_ptr _tmp5E1=({const char*_tmp1C5=",";_tag_dyneither(_tmp1C5,sizeof(char),2U);});Cyc_PP_group(_tmp5E3,_tmp5E2,_tmp5E1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));});}
# 900
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1C6=(*t).f1;
struct Cyc_Core_Opt*dopt=_tmp1C6 == 0?0:({struct Cyc_Core_Opt*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({struct Cyc_PP_Doc*_tmp5E4=Cyc_PP_text(*_tmp1C6);_tmp1C7->v=_tmp5E4;});_tmp1C7;});
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 906
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 910
struct Cyc_List_List*_tmp1C8=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
_tmp1C8=({struct Cyc_List_List*_tmp5E6=_tmp1C8;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5E6,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));({struct Cyc_PP_Doc*_tmp5E5=Cyc_PP_text(({const char*_tmp1C9="...";_tag_dyneither(_tmp1C9,sizeof(char),4U);}));_tmp1CA->hd=_tmp5E5;}),_tmp1CA->tl=0;_tmp1CA;}));});else{
if(cyc_varargs != 0){
struct Cyc_PP_Doc*_tmp1CB=({struct Cyc_PP_Doc*_tmp1CD[3U];({struct Cyc_PP_Doc*_tmp5EA=Cyc_PP_text(({const char*_tmp1CE="...";_tag_dyneither(_tmp1CE,sizeof(char),4U);}));_tmp1CD[0]=_tmp5EA;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_tmp5E9=Cyc_PP_text(({const char*_tmp1CF=" inject ";_tag_dyneither(_tmp1CF,sizeof(char),9U);}));_tmp1CD[1]=_tmp5E9;}):({struct Cyc_PP_Doc*_tmp5E8=Cyc_PP_text(({const char*_tmp1D0=" ";_tag_dyneither(_tmp1D0,sizeof(char),2U);}));_tmp1CD[1]=_tmp5E8;}),({
struct Cyc_PP_Doc*_tmp5E7=Cyc_Absynpp_funarg2doc(({struct _tuple8*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->f1=cyc_varargs->name,_tmp1D1->f2=cyc_varargs->tq,_tmp1D1->f3=cyc_varargs->type;_tmp1D1;}));_tmp1CD[2]=_tmp5E7;});Cyc_PP_cat(_tag_dyneither(_tmp1CD,sizeof(struct Cyc_PP_Doc*),3U));});
# 919
_tmp1C8=({struct Cyc_List_List*_tmp5EB=_tmp1C8;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5EB,({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=_tmp1CB,_tmp1CC->tl=0;_tmp1CC;}));});}}{
# 921
struct Cyc_PP_Doc*_tmp1D2=({struct _dyneither_ptr _tmp5EE=({const char*_tmp1E1="";_tag_dyneither(_tmp1E1,sizeof(char),1U);});struct _dyneither_ptr _tmp5ED=({const char*_tmp1E2="";_tag_dyneither(_tmp1E2,sizeof(char),1U);});struct _dyneither_ptr _tmp5EC=({const char*_tmp1E3=",";_tag_dyneither(_tmp1E3,sizeof(char),2U);});Cyc_PP_group(_tmp5EE,_tmp5ED,_tmp5EC,_tmp1C8);});
if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1D2=({struct Cyc_PP_Doc*_tmp1D3[3U];_tmp1D3[0]=_tmp1D2,({struct Cyc_PP_Doc*_tmp5F0=Cyc_PP_text(({const char*_tmp1D4=";";_tag_dyneither(_tmp1D4,sizeof(char),2U);}));_tmp1D3[1]=_tmp5F0;}),({struct Cyc_PP_Doc*_tmp5EF=Cyc_Absynpp_eff2doc(effopt);_tmp1D3[2]=_tmp5EF;});Cyc_PP_cat(_tag_dyneither(_tmp1D3,sizeof(struct Cyc_PP_Doc*),3U));});
if(rgn_po != 0)
_tmp1D2=({struct Cyc_PP_Doc*_tmp1D5[3U];_tmp1D5[0]=_tmp1D2,({struct Cyc_PP_Doc*_tmp5F2=Cyc_PP_text(({const char*_tmp1D6=":";_tag_dyneither(_tmp1D6,sizeof(char),2U);}));_tmp1D5[1]=_tmp5F2;}),({struct Cyc_PP_Doc*_tmp5F1=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1D5[2]=_tmp5F1;});Cyc_PP_cat(_tag_dyneither(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3U));});{
struct Cyc_PP_Doc*_tmp1D7=({struct Cyc_PP_Doc*_tmp1DE[3U];({struct Cyc_PP_Doc*_tmp5F4=Cyc_PP_text(({const char*_tmp1DF="(";_tag_dyneither(_tmp1DF,sizeof(char),2U);}));_tmp1DE[0]=_tmp5F4;}),_tmp1DE[1]=_tmp1D2,({struct Cyc_PP_Doc*_tmp5F3=Cyc_PP_text(({const char*_tmp1E0=")";_tag_dyneither(_tmp1E0,sizeof(char),2U);}));_tmp1DE[2]=_tmp5F3;});Cyc_PP_cat(_tag_dyneither(_tmp1DE,sizeof(struct Cyc_PP_Doc*),3U));});
if(req != 0)
_tmp1D7=({struct Cyc_PP_Doc*_tmp1D8[4U];_tmp1D8[0]=_tmp1D7,({struct Cyc_PP_Doc*_tmp5F7=Cyc_PP_text(({const char*_tmp1D9=" @requires(";_tag_dyneither(_tmp1D9,sizeof(char),12U);}));_tmp1D8[1]=_tmp5F7;}),({struct Cyc_PP_Doc*_tmp5F6=Cyc_Absynpp_exp2doc(req);_tmp1D8[2]=_tmp5F6;}),({struct Cyc_PP_Doc*_tmp5F5=Cyc_PP_text(({const char*_tmp1DA=")";_tag_dyneither(_tmp1DA,sizeof(char),2U);}));_tmp1D8[3]=_tmp5F5;});Cyc_PP_cat(_tag_dyneither(_tmp1D8,sizeof(struct Cyc_PP_Doc*),4U));});
if(ens != 0)
_tmp1D7=({struct Cyc_PP_Doc*_tmp1DB[4U];_tmp1DB[0]=_tmp1D7,({struct Cyc_PP_Doc*_tmp5FA=Cyc_PP_text(({const char*_tmp1DC=" @ensures(";_tag_dyneither(_tmp1DC,sizeof(char),11U);}));_tmp1DB[1]=_tmp5FA;}),({struct Cyc_PP_Doc*_tmp5F9=Cyc_Absynpp_exp2doc(ens);_tmp1DB[2]=_tmp5F9;}),({struct Cyc_PP_Doc*_tmp5F8=Cyc_PP_text(({const char*_tmp1DD=")";_tag_dyneither(_tmp1DD,sizeof(char),2U);}));_tmp1DB[3]=_tmp5F8;});Cyc_PP_cat(_tag_dyneither(_tmp1DB,sizeof(struct Cyc_PP_Doc*),4U));});
return _tmp1D7;};};}
# 934
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 936
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp1E4=0;
int match;
{union Cyc_Absyn_Nmspace _tmp1E5=(*q).f1;union Cyc_Absyn_Nmspace _tmp1E6=_tmp1E5;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;switch((_tmp1E6.C_n).tag){case 4U: _LL1: _LL2:
 _tmp1E8=0;goto _LL4;case 1U: _LL3: _tmp1E8=(_tmp1E6.Rel_n).val;_LL4:
# 942
 match=0;
_tmp1E4=_tmp1E8;
goto _LL0;case 3U: _LL5: _tmp1E9=(_tmp1E6.C_n).val;_LL6:
# 946
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1E9,Cyc_Absynpp_curr_namespace);
# 948
goto _LL0;default: _LL7: _tmp1EA=(_tmp1E6.Abs_n).val;_LL8:
# 950
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1EA,Cyc_Absynpp_curr_namespace);
_tmp1E4=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=Cyc_Absynpp_cyc_stringptr,_tmp1E7->tl=_tmp1EA;_tmp1E7;}): _tmp1EA;
goto _LL0;}_LL0:;}
# 954
if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp5FC=({struct Cyc_List_List*_tmp5FB=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5FB,({struct Cyc_List_List*_tmp1EB=_cycalloc(sizeof(*_tmp1EB));
_tmp1EB->hd=(*q).f2,_tmp1EB->tl=0;_tmp1EB;}));});
# 955
Cyc_str_sepstr(_tmp5FC,({const char*_tmp1EC="_";_tag_dyneither(_tmp1EC,sizeof(char),2U);}));});else{
# 959
if(match)
return*(*q).f2;else{
# 962
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmp5FE=({struct Cyc_List_List*_tmp5FD=_tmp1E4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp5FD,({struct Cyc_List_List*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->hd=(*q).f2,_tmp1ED->tl=0;_tmp1ED;}));});Cyc_str_sepstr(_tmp5FE,({const char*_tmp1EE="::";_tag_dyneither(_tmp1EE,sizeof(char),3U);}));});}}}
# 966
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 970
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1EF=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
# 974
return({struct _dyneither_ptr _tmp601=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp600=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp5FF=({const char*_tmp1F0="\\textbf{";_tag_dyneither(_tmp1F0,sizeof(char),9U);});Cyc_strconcat(_tmp5FF,(struct _dyneither_ptr)_tmp1EF);});Cyc_strconcat(_tmp600,({const char*_tmp1F1="}";_tag_dyneither(_tmp1F1,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp601,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1EF));});else{
return Cyc_PP_text(_tmp1EF);}}
# 979
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 981
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 984
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp1F2=(*v).f1;union Cyc_Absyn_Nmspace _tmp1F3=_tmp1F2;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F5;switch((_tmp1F3.C_n).tag){case 4U: _LL1: _LL2:
 goto _LL4;case 1U: if((_tmp1F3.Rel_n).val == 0){_LL3: _LL4:
 return*(*v).f2;}else{_LL9: _LLA:
# 995
 return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp602=({const char*_tmp1F4="/* bad namespace : */ ";_tag_dyneither(_tmp1F4,sizeof(char),23U);});Cyc_strconcat(_tmp602,(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));});}case 3U: _LL5: _tmp1F5=(_tmp1F3.C_n).val;_LL6:
# 988
 _tmp1F6=_tmp1F5;goto _LL8;default: _LL7: _tmp1F6=(_tmp1F3.Abs_n).val;_LL8:
# 990
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1F6,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 993
goto _LLA;}}_LL0:;}else{
# 998
return*(*v).f2;}}
# 1001
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 1004
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp1F7=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
# 1008
return({struct _dyneither_ptr _tmp605=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp604=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp603=({const char*_tmp1F8="\\textbf{";_tag_dyneither(_tmp1F8,sizeof(char),9U);});Cyc_strconcat(_tmp603,(struct _dyneither_ptr)_tmp1F7);});Cyc_strconcat(_tmp604,({const char*_tmp1F9="}";_tag_dyneither(_tmp1F9,sizeof(char),2U);}));});Cyc_PP_text_width(_tmp605,(int)
Cyc_strlen((struct _dyneither_ptr)_tmp1F7));});else{
return Cyc_PP_text(_tmp1F7);}}
# 1013
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _tmp606=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_tmp606,t,0);});}
# 1024 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp1FA=e->r;void*_tmp1FB=_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp1FE;enum Cyc_Absyn_Primop _tmp1FD;switch(*((int*)_tmp1FB)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return 10000;case 3U: _LL5: _tmp1FD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL6: {
# 1029
enum Cyc_Absyn_Primop _tmp1FC=_tmp1FD;switch(_tmp1FC){case Cyc_Absyn_Plus: _LL56: _LL57:
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
# 1052
 return 20;case 5U: _LL9: _LLA:
 return 130;case 6U: _LLB: _LLC:
 return 30;case 7U: _LLD: _LLE:
 return 35;case 8U: _LLF: _LL10:
 return 30;case 9U: _LL11: _LL12:
 return - 10;case 10U: _LL13: _LL14:
 return 140;case 2U: _LL15: _LL16:
 return 140;case 11U: _LL17: _LL18:
 return 130;case 12U: _LL19: _tmp1FE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL1A:
 return Cyc_Absynpp_exp_prec(_tmp1FE);case 13U: _LL1B: _tmp1FF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1FB)->f1;_LL1C:
 return Cyc_Absynpp_exp_prec(_tmp1FF);case 14U: _LL1D: _LL1E:
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
# 1094
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1098
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp200=e->r;void*_tmp201=_tmp200;struct Cyc_Absyn_Stmt*_tmp2D6;struct Cyc_Core_Opt*_tmp2D5;struct Cyc_List_List*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D2;int _tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;void**_tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;int _tmp2CD;struct Cyc_Absyn_Enumfield*_tmp2CC;struct Cyc_Absyn_Enumfield*_tmp2CB;struct Cyc_List_List*_tmp2CA;struct Cyc_Absyn_Datatypefield*_tmp2C9;struct Cyc_List_List*_tmp2C8;struct _tuple0*_tmp2C7;struct Cyc_List_List*_tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_Absyn_Exp*_tmp2C4;void*_tmp2C3;struct Cyc_Absyn_Vardecl*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_List_List*_tmp2BF;struct _tuple8*_tmp2BE;struct Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2B9;struct _dyneither_ptr*_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B7;struct _dyneither_ptr*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B5;void*_tmp2B4;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct _dyneither_ptr*_tmp2B1;int _tmp2B0;struct _dyneither_ptr _tmp2AF;void*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AC;void*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;void*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A6;struct Cyc_Absyn_Exp*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A4;struct Cyc_Absyn_Exp*_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_List_List*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp29F;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp297;enum Cyc_Absyn_Incrementor _tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Core_Opt*_tmp294;struct Cyc_Absyn_Exp*_tmp293;enum Cyc_Absyn_Primop _tmp292;struct Cyc_List_List*_tmp291;struct _dyneither_ptr _tmp290;void*_tmp28F;union Cyc_Absyn_Cnst _tmp28E;switch(*((int*)_tmp201)){case 0U: _LL1: _tmp28E=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL2:
 s=Cyc_Absynpp_cnst2doc(_tmp28E);goto _LL0;case 1U: _LL3: _tmp28F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL4:
 s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(_tmp28F));goto _LL0;case 2U: _LL5: _tmp290=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL6:
# 1105
 s=({struct Cyc_PP_Doc*_tmp202[4U];({struct Cyc_PP_Doc*_tmp60A=Cyc_PP_text(({const char*_tmp203="__cyclone_pragma__";_tag_dyneither(_tmp203,sizeof(char),19U);}));_tmp202[0]=_tmp60A;}),({struct Cyc_PP_Doc*_tmp609=Cyc_PP_text(({const char*_tmp204="(";_tag_dyneither(_tmp204,sizeof(char),2U);}));_tmp202[1]=_tmp609;}),({struct Cyc_PP_Doc*_tmp608=Cyc_PP_text(_tmp290);_tmp202[2]=_tmp608;}),({struct Cyc_PP_Doc*_tmp607=Cyc_PP_text(({const char*_tmp205=")";_tag_dyneither(_tmp205,sizeof(char),2U);}));_tmp202[3]=_tmp607;});Cyc_PP_cat(_tag_dyneither(_tmp202,sizeof(struct Cyc_PP_Doc*),4U));});goto _LL0;case 3U: _LL7: _tmp292=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp291=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL8:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp292,_tmp291);goto _LL0;case 4U: _LL9: _tmp295=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp294=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp293=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LLA:
# 1108
 s=({struct Cyc_PP_Doc*_tmp206[5U];({struct Cyc_PP_Doc*_tmp610=Cyc_Absynpp_exp2doc_prec(myprec,_tmp295);_tmp206[0]=_tmp610;}),({
struct Cyc_PP_Doc*_tmp60F=Cyc_PP_text(({const char*_tmp207=" ";_tag_dyneither(_tmp207,sizeof(char),2U);}));_tmp206[1]=_tmp60F;}),
_tmp294 == 0?({struct Cyc_PP_Doc*_tmp60E=Cyc_PP_nil_doc();_tmp206[2]=_tmp60E;}):({struct Cyc_PP_Doc*_tmp60D=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp294->v);_tmp206[2]=_tmp60D;}),({
struct Cyc_PP_Doc*_tmp60C=Cyc_PP_text(({const char*_tmp208="= ";_tag_dyneither(_tmp208,sizeof(char),3U);}));_tmp206[3]=_tmp60C;}),({
struct Cyc_PP_Doc*_tmp60B=Cyc_Absynpp_exp2doc_prec(myprec,_tmp293);_tmp206[4]=_tmp60B;});Cyc_PP_cat(_tag_dyneither(_tmp206,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 5U: _LLB: _tmp297=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp296=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LLC: {
# 1115
struct Cyc_PP_Doc*_tmp209=Cyc_Absynpp_exp2doc_prec(myprec,_tmp297);
{enum Cyc_Absyn_Incrementor _tmp20A=_tmp296;switch(_tmp20A){case Cyc_Absyn_PreInc: _LL56: _LL57:
 s=({struct Cyc_PP_Doc*_tmp20B[2U];({struct Cyc_PP_Doc*_tmp611=Cyc_PP_text(({const char*_tmp20C="++";_tag_dyneither(_tmp20C,sizeof(char),3U);}));_tmp20B[0]=_tmp611;}),_tmp20B[1]=_tmp209;Cyc_PP_cat(_tag_dyneither(_tmp20B,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PreDec: _LL58: _LL59:
 s=({struct Cyc_PP_Doc*_tmp20D[2U];({struct Cyc_PP_Doc*_tmp612=Cyc_PP_text(({const char*_tmp20E="--";_tag_dyneither(_tmp20E,sizeof(char),3U);}));_tmp20D[0]=_tmp612;}),_tmp20D[1]=_tmp209;Cyc_PP_cat(_tag_dyneither(_tmp20D,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostInc: _LL5A: _LL5B:
 s=({struct Cyc_PP_Doc*_tmp20F[2U];_tmp20F[0]=_tmp209,({struct Cyc_PP_Doc*_tmp613=Cyc_PP_text(({const char*_tmp210="++";_tag_dyneither(_tmp210,sizeof(char),3U);}));_tmp20F[1]=_tmp613;});Cyc_PP_cat(_tag_dyneither(_tmp20F,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;case Cyc_Absyn_PostDec: _LL5C: _LL5D:
 goto _LL5F;default: _LL5E: _LL5F:
 s=({struct Cyc_PP_Doc*_tmp211[2U];_tmp211[0]=_tmp209,({struct Cyc_PP_Doc*_tmp614=Cyc_PP_text(({const char*_tmp212="--";_tag_dyneither(_tmp212,sizeof(char),3U);}));_tmp211[1]=_tmp614;});Cyc_PP_cat(_tag_dyneither(_tmp211,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL55;}_LL55:;}
# 1123
goto _LL0;}case 6U: _LLD: _tmp29A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp299=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp298=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LLE:
# 1125
 s=({struct Cyc_PP_Doc*_tmp213[5U];({struct Cyc_PP_Doc*_tmp619=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29A);_tmp213[0]=_tmp619;}),({struct Cyc_PP_Doc*_tmp618=Cyc_PP_text(({const char*_tmp214=" ? ";_tag_dyneither(_tmp214,sizeof(char),4U);}));_tmp213[1]=_tmp618;}),({struct Cyc_PP_Doc*_tmp617=Cyc_Absynpp_exp2doc_prec(0,_tmp299);_tmp213[2]=_tmp617;}),({
struct Cyc_PP_Doc*_tmp616=Cyc_PP_text(({const char*_tmp215=" : ";_tag_dyneither(_tmp215,sizeof(char),4U);}));_tmp213[3]=_tmp616;}),({struct Cyc_PP_Doc*_tmp615=Cyc_Absynpp_exp2doc_prec(myprec,_tmp298);_tmp213[4]=_tmp615;});Cyc_PP_cat(_tag_dyneither(_tmp213,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 7U: _LLF: _tmp29C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL10:
# 1129
 s=({struct Cyc_PP_Doc*_tmp216[3U];({struct Cyc_PP_Doc*_tmp61C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29C);_tmp216[0]=_tmp61C;}),({struct Cyc_PP_Doc*_tmp61B=Cyc_PP_text(({const char*_tmp217=" && ";_tag_dyneither(_tmp217,sizeof(char),5U);}));_tmp216[1]=_tmp61B;}),({struct Cyc_PP_Doc*_tmp61A=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29B);_tmp216[2]=_tmp61A;});Cyc_PP_cat(_tag_dyneither(_tmp216,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 8U: _LL11: _tmp29E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL12:
# 1132
 s=({struct Cyc_PP_Doc*_tmp218[3U];({struct Cyc_PP_Doc*_tmp61F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29E);_tmp218[0]=_tmp61F;}),({struct Cyc_PP_Doc*_tmp61E=Cyc_PP_text(({const char*_tmp219=" || ";_tag_dyneither(_tmp219,sizeof(char),5U);}));_tmp218[1]=_tmp61E;}),({struct Cyc_PP_Doc*_tmp61D=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29D);_tmp218[2]=_tmp61D;});Cyc_PP_cat(_tag_dyneither(_tmp218,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 9U: _LL13: _tmp2A0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp29F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL14:
# 1137
 s=({struct Cyc_PP_Doc*_tmp21A[3U];({struct Cyc_PP_Doc*_tmp622=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp2A0);_tmp21A[0]=_tmp622;}),({struct Cyc_PP_Doc*_tmp621=Cyc_PP_text(({const char*_tmp21B=", ";_tag_dyneither(_tmp21B,sizeof(char),3U);}));_tmp21A[1]=_tmp621;}),({struct Cyc_PP_Doc*_tmp620=Cyc_Absynpp_exp2doc_prec(myprec - 1,_tmp29F);_tmp21A[2]=_tmp620;});Cyc_PP_cat(_tag_dyneither(_tmp21A,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 10U: _LL15: _tmp2A2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL16:
# 1140
 s=({struct Cyc_PP_Doc*_tmp21C[4U];({struct Cyc_PP_Doc*_tmp626=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A2);_tmp21C[0]=_tmp626;}),({
struct Cyc_PP_Doc*_tmp625=Cyc_PP_text(({const char*_tmp21D="(";_tag_dyneither(_tmp21D,sizeof(char),2U);}));_tmp21C[1]=_tmp625;}),({
struct Cyc_PP_Doc*_tmp624=Cyc_Absynpp_exps2doc_prec(20,_tmp2A1);_tmp21C[2]=_tmp624;}),({
struct Cyc_PP_Doc*_tmp623=Cyc_PP_text(({const char*_tmp21E=")";_tag_dyneither(_tmp21E,sizeof(char),2U);}));_tmp21C[3]=_tmp623;});Cyc_PP_cat(_tag_dyneither(_tmp21C,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 11U: _LL17: _tmp2A3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL18:
# 1146
 s=({struct Cyc_PP_Doc*_tmp21F[2U];({struct Cyc_PP_Doc*_tmp628=Cyc_PP_text(({const char*_tmp220="throw ";_tag_dyneither(_tmp220,sizeof(char),7U);}));_tmp21F[0]=_tmp628;}),({struct Cyc_PP_Doc*_tmp627=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A3);_tmp21F[1]=_tmp627;});Cyc_PP_cat(_tag_dyneither(_tmp21F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 12U: _LL19: _tmp2A4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL1A:
# 1149
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A4);
goto _LL0;case 13U: _LL1B: _tmp2A5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL1C:
# 1152
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2A5);
goto _LL0;case 14U: _LL1D: _tmp2A7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL1E:
# 1155
 s=({struct Cyc_PP_Doc*_tmp221[4U];({struct Cyc_PP_Doc*_tmp62C=Cyc_PP_text(({const char*_tmp222="(";_tag_dyneither(_tmp222,sizeof(char),2U);}));_tmp221[0]=_tmp62C;}),({
struct Cyc_PP_Doc*_tmp62B=Cyc_Absynpp_typ2doc(_tmp2A7);_tmp221[1]=_tmp62B;}),({
struct Cyc_PP_Doc*_tmp62A=Cyc_PP_text(({const char*_tmp223=")";_tag_dyneither(_tmp223,sizeof(char),2U);}));_tmp221[2]=_tmp62A;}),({
struct Cyc_PP_Doc*_tmp629=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A6);_tmp221[3]=_tmp629;});Cyc_PP_cat(_tag_dyneither(_tmp221,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 15U: _LL1F: _tmp2A8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL20:
# 1161
 s=({struct Cyc_PP_Doc*_tmp224[2U];({struct Cyc_PP_Doc*_tmp62E=Cyc_PP_text(({const char*_tmp225="&";_tag_dyneither(_tmp225,sizeof(char),2U);}));_tmp224[0]=_tmp62E;}),({
struct Cyc_PP_Doc*_tmp62D=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A8);_tmp224[1]=_tmp62D;});Cyc_PP_cat(_tag_dyneither(_tmp224,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 16U: _LL21: _tmp2AA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2A9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL22:
# 1165
 if(_tmp2AA == 0)
s=({struct Cyc_PP_Doc*_tmp226[2U];({struct Cyc_PP_Doc*_tmp630=Cyc_PP_text(({const char*_tmp227="new ";_tag_dyneither(_tmp227,sizeof(char),5U);}));_tmp226[0]=_tmp630;}),({struct Cyc_PP_Doc*_tmp62F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A9);_tmp226[1]=_tmp62F;});Cyc_PP_cat(_tag_dyneither(_tmp226,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 1168
s=({struct Cyc_PP_Doc*_tmp228[4U];({struct Cyc_PP_Doc*_tmp634=Cyc_PP_text(({const char*_tmp229="rnew(";_tag_dyneither(_tmp229,sizeof(char),6U);}));_tmp228[0]=_tmp634;}),({struct Cyc_PP_Doc*_tmp633=Cyc_Absynpp_exp2doc(_tmp2AA);_tmp228[1]=_tmp633;}),({struct Cyc_PP_Doc*_tmp632=Cyc_PP_text(({const char*_tmp22A=") ";_tag_dyneither(_tmp22A,sizeof(char),3U);}));_tmp228[2]=_tmp632;}),({
struct Cyc_PP_Doc*_tmp631=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A9);_tmp228[3]=_tmp631;});Cyc_PP_cat(_tag_dyneither(_tmp228,sizeof(struct Cyc_PP_Doc*),4U));});}
goto _LL0;case 17U: _LL23: _tmp2AB=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL24:
 s=({struct Cyc_PP_Doc*_tmp22B[3U];({struct Cyc_PP_Doc*_tmp637=Cyc_PP_text(({const char*_tmp22C="sizeof(";_tag_dyneither(_tmp22C,sizeof(char),8U);}));_tmp22B[0]=_tmp637;}),({struct Cyc_PP_Doc*_tmp636=Cyc_Absynpp_typ2doc(_tmp2AB);_tmp22B[1]=_tmp636;}),({struct Cyc_PP_Doc*_tmp635=Cyc_PP_text(({const char*_tmp22D=")";_tag_dyneither(_tmp22D,sizeof(char),2U);}));_tmp22B[2]=_tmp635;});Cyc_PP_cat(_tag_dyneither(_tmp22B,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 18U: _LL25: _tmp2AC=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL26:
 s=({struct Cyc_PP_Doc*_tmp22E[3U];({struct Cyc_PP_Doc*_tmp63A=Cyc_PP_text(({const char*_tmp22F="sizeof(";_tag_dyneither(_tmp22F,sizeof(char),8U);}));_tmp22E[0]=_tmp63A;}),({struct Cyc_PP_Doc*_tmp639=Cyc_Absynpp_exp2doc(_tmp2AC);_tmp22E[1]=_tmp639;}),({struct Cyc_PP_Doc*_tmp638=Cyc_PP_text(({const char*_tmp230=")";_tag_dyneither(_tmp230,sizeof(char),2U);}));_tmp22E[2]=_tmp638;});Cyc_PP_cat(_tag_dyneither(_tmp22E,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 41U: _LL27: _tmp2AD=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL28:
 s=({struct Cyc_PP_Doc*_tmp231[3U];({struct Cyc_PP_Doc*_tmp63D=Cyc_PP_text(({const char*_tmp232="__extension__(";_tag_dyneither(_tmp232,sizeof(char),15U);}));_tmp231[0]=_tmp63D;}),({struct Cyc_PP_Doc*_tmp63C=Cyc_Absynpp_exp2doc(_tmp2AD);_tmp231[1]=_tmp63C;}),({struct Cyc_PP_Doc*_tmp63B=Cyc_PP_text(({const char*_tmp233=")";_tag_dyneither(_tmp233,sizeof(char),2U);}));_tmp231[2]=_tmp63B;});Cyc_PP_cat(_tag_dyneither(_tmp231,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 39U: _LL29: _tmp2AE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL2A:
 s=({struct Cyc_PP_Doc*_tmp234[3U];({struct Cyc_PP_Doc*_tmp640=Cyc_PP_text(({const char*_tmp235="valueof(";_tag_dyneither(_tmp235,sizeof(char),9U);}));_tmp234[0]=_tmp640;}),({struct Cyc_PP_Doc*_tmp63F=Cyc_Absynpp_typ2doc(_tmp2AE);_tmp234[1]=_tmp63F;}),({struct Cyc_PP_Doc*_tmp63E=Cyc_PP_text(({const char*_tmp236=")";_tag_dyneither(_tmp236,sizeof(char),2U);}));_tmp234[2]=_tmp63E;});Cyc_PP_cat(_tag_dyneither(_tmp234,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;case 40U: _LL2B: _tmp2B0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2AF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2C:
# 1176
 if(_tmp2B0)
s=({struct Cyc_PP_Doc*_tmp237[4U];({struct Cyc_PP_Doc*_tmp644=Cyc_PP_text(({const char*_tmp238="__asm__";_tag_dyneither(_tmp238,sizeof(char),8U);}));_tmp237[0]=_tmp644;}),({struct Cyc_PP_Doc*_tmp643=Cyc_PP_text(({const char*_tmp239=" volatile (";_tag_dyneither(_tmp239,sizeof(char),12U);}));_tmp237[1]=_tmp643;}),({struct Cyc_PP_Doc*_tmp642=Cyc_PP_text(_tmp2AF);_tmp237[2]=_tmp642;}),({struct Cyc_PP_Doc*_tmp641=Cyc_PP_text(({const char*_tmp23A=")";_tag_dyneither(_tmp23A,sizeof(char),2U);}));_tmp237[3]=_tmp641;});Cyc_PP_cat(_tag_dyneither(_tmp237,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1179
s=({struct Cyc_PP_Doc*_tmp23B[3U];({struct Cyc_PP_Doc*_tmp647=Cyc_PP_text(({const char*_tmp23C="__asm__(";_tag_dyneither(_tmp23C,sizeof(char),9U);}));_tmp23B[0]=_tmp647;}),({struct Cyc_PP_Doc*_tmp646=Cyc_PP_text(_tmp2AF);_tmp23B[1]=_tmp646;}),({struct Cyc_PP_Doc*_tmp645=Cyc_PP_text(({const char*_tmp23D=")";_tag_dyneither(_tmp23D,sizeof(char),2U);}));_tmp23B[2]=_tmp645;});Cyc_PP_cat(_tag_dyneither(_tmp23B,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 38U: _LL2D: _tmp2B2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL2E:
# 1182
 s=({struct Cyc_PP_Doc*_tmp23E[5U];({struct Cyc_PP_Doc*_tmp64C=Cyc_PP_text(({const char*_tmp23F="tagcheck(";_tag_dyneither(_tmp23F,sizeof(char),10U);}));_tmp23E[0]=_tmp64C;}),({struct Cyc_PP_Doc*_tmp64B=Cyc_Absynpp_exp2doc(_tmp2B2);_tmp23E[1]=_tmp64B;}),({struct Cyc_PP_Doc*_tmp64A=Cyc_PP_text(({const char*_tmp240=".";_tag_dyneither(_tmp240,sizeof(char),2U);}));_tmp23E[2]=_tmp64A;}),({struct Cyc_PP_Doc*_tmp649=Cyc_PP_textptr(_tmp2B1);_tmp23E[3]=_tmp649;}),({struct Cyc_PP_Doc*_tmp648=Cyc_PP_text(({const char*_tmp241=")";_tag_dyneither(_tmp241,sizeof(char),2U);}));_tmp23E[4]=_tmp648;});Cyc_PP_cat(_tag_dyneither(_tmp23E,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 19U: _LL2F: _tmp2B4=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B3=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL30:
# 1185
 s=({struct Cyc_PP_Doc*_tmp242[3U];({struct Cyc_PP_Doc*_tmp64F=Cyc_PP_text(({const char*_tmp243="offsetof(";_tag_dyneither(_tmp243,sizeof(char),10U);}));_tmp242[0]=_tmp64F;}),({struct Cyc_PP_Doc*_tmp64E=Cyc_Absynpp_typ2doc(_tmp2B4);_tmp242[1]=_tmp64E;}),({struct Cyc_PP_Doc*_tmp64D=Cyc_PP_text(({const char*_tmp244=",";_tag_dyneither(_tmp244,sizeof(char),2U);}));_tmp242[2]=_tmp64D;});Cyc_PP_cat(_tag_dyneither(_tmp242,sizeof(struct Cyc_PP_Doc*),3U));});
for(0;_tmp2B3 != 0;_tmp2B3=_tmp2B3->tl){
void*_tmp245=(void*)_tmp2B3->hd;void*_tmp246=_tmp245;unsigned int _tmp24F;struct _dyneither_ptr*_tmp24E;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp246)->tag == 0U){_LL61: _tmp24E=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp246)->f1;_LL62:
 s=({struct Cyc_PP_Doc*_tmp247[3U];_tmp247[0]=s,({struct Cyc_PP_Doc*_tmp652=Cyc_PP_textptr(_tmp24E);_tmp247[1]=_tmp652;}),
_tmp2B3->tl != 0?({struct Cyc_PP_Doc*_tmp651=Cyc_PP_text(({const char*_tmp248=".";_tag_dyneither(_tmp248,sizeof(char),2U);}));_tmp247[2]=_tmp651;}):({struct Cyc_PP_Doc*_tmp650=Cyc_PP_nil_doc();_tmp247[2]=_tmp650;});Cyc_PP_cat(_tag_dyneither(_tmp247,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL60;}else{_LL63: _tmp24F=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp246)->f1;_LL64:
 s=({struct Cyc_PP_Doc*_tmp249[3U];_tmp249[0]=s,({struct Cyc_PP_Doc*_tmp656=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp24C=({struct Cyc_Int_pa_PrintArg_struct _tmp540;_tmp540.tag=1U,_tmp540.f1=(unsigned long)((int)_tmp24F);_tmp540;});void*_tmp24A[1U];_tmp24A[0]=& _tmp24C;({struct _dyneither_ptr _tmp655=({const char*_tmp24B="%d";_tag_dyneither(_tmp24B,sizeof(char),3U);});Cyc_aprintf(_tmp655,_tag_dyneither(_tmp24A,sizeof(void*),1U));});}));_tmp249[1]=_tmp656;}),
_tmp2B3->tl != 0?({struct Cyc_PP_Doc*_tmp654=Cyc_PP_text(({const char*_tmp24D=".";_tag_dyneither(_tmp24D,sizeof(char),2U);}));_tmp249[2]=_tmp654;}):({struct Cyc_PP_Doc*_tmp653=Cyc_PP_nil_doc();_tmp249[2]=_tmp653;});Cyc_PP_cat(_tag_dyneither(_tmp249,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL60;}_LL60:;}
# 1195
s=({struct Cyc_PP_Doc*_tmp250[2U];_tmp250[0]=s,({struct Cyc_PP_Doc*_tmp657=Cyc_PP_text(({const char*_tmp251=")";_tag_dyneither(_tmp251,sizeof(char),2U);}));_tmp250[1]=_tmp657;});Cyc_PP_cat(_tag_dyneither(_tmp250,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 20U: _LL31: _tmp2B5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL32:
# 1198
 s=({struct Cyc_PP_Doc*_tmp252[2U];({struct Cyc_PP_Doc*_tmp659=Cyc_PP_text(({const char*_tmp253="*";_tag_dyneither(_tmp253,sizeof(char),2U);}));_tmp252[0]=_tmp659;}),({struct Cyc_PP_Doc*_tmp658=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5);_tmp252[1]=_tmp658;});Cyc_PP_cat(_tag_dyneither(_tmp252,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 21U: _LL33: _tmp2B7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL34:
# 1201
 s=({struct Cyc_PP_Doc*_tmp254[3U];({struct Cyc_PP_Doc*_tmp65C=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B7);_tmp254[0]=_tmp65C;}),({struct Cyc_PP_Doc*_tmp65B=Cyc_PP_text(({const char*_tmp255=".";_tag_dyneither(_tmp255,sizeof(char),2U);}));_tmp254[1]=_tmp65B;}),({struct Cyc_PP_Doc*_tmp65A=Cyc_PP_textptr(_tmp2B6);_tmp254[2]=_tmp65A;});Cyc_PP_cat(_tag_dyneither(_tmp254,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 22U: _LL35: _tmp2B9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2B8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL36:
# 1204
 s=({struct Cyc_PP_Doc*_tmp256[3U];({struct Cyc_PP_Doc*_tmp65F=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B9);_tmp256[0]=_tmp65F;}),({struct Cyc_PP_Doc*_tmp65E=Cyc_PP_text(({const char*_tmp257="->";_tag_dyneither(_tmp257,sizeof(char),3U);}));_tmp256[1]=_tmp65E;}),({struct Cyc_PP_Doc*_tmp65D=Cyc_PP_textptr(_tmp2B8);_tmp256[2]=_tmp65D;});Cyc_PP_cat(_tag_dyneither(_tmp256,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 23U: _LL37: _tmp2BB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL38:
# 1207
 s=({struct Cyc_PP_Doc*_tmp258[4U];({struct Cyc_PP_Doc*_tmp663=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2BB);_tmp258[0]=_tmp663;}),({
struct Cyc_PP_Doc*_tmp662=Cyc_PP_text(({const char*_tmp259="[";_tag_dyneither(_tmp259,sizeof(char),2U);}));_tmp258[1]=_tmp662;}),({
struct Cyc_PP_Doc*_tmp661=Cyc_Absynpp_exp2doc(_tmp2BA);_tmp258[2]=_tmp661;}),({
struct Cyc_PP_Doc*_tmp660=Cyc_PP_text(({const char*_tmp25A="]";_tag_dyneither(_tmp25A,sizeof(char),2U);}));_tmp258[3]=_tmp660;});Cyc_PP_cat(_tag_dyneither(_tmp258,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 24U: _LL39: _tmp2BC=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL3A:
# 1213
 s=({struct Cyc_PP_Doc*_tmp25B[4U];({struct Cyc_PP_Doc*_tmp667=Cyc_Absynpp_dollar();_tmp25B[0]=_tmp667;}),({
struct Cyc_PP_Doc*_tmp666=Cyc_PP_text(({const char*_tmp25C="(";_tag_dyneither(_tmp25C,sizeof(char),2U);}));_tmp25B[1]=_tmp666;}),({
struct Cyc_PP_Doc*_tmp665=Cyc_Absynpp_exps2doc_prec(20,_tmp2BC);_tmp25B[2]=_tmp665;}),({
struct Cyc_PP_Doc*_tmp664=Cyc_PP_text(({const char*_tmp25D=")";_tag_dyneither(_tmp25D,sizeof(char),2U);}));_tmp25B[3]=_tmp664;});Cyc_PP_cat(_tag_dyneither(_tmp25B,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 25U: _LL3B: _tmp2BE=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2BD=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL3C:
# 1219
 s=({struct Cyc_PP_Doc*_tmp25E[4U];({struct Cyc_PP_Doc*_tmp66C=Cyc_PP_text(({const char*_tmp25F="(";_tag_dyneither(_tmp25F,sizeof(char),2U);}));_tmp25E[0]=_tmp66C;}),({
struct Cyc_PP_Doc*_tmp66B=Cyc_Absynpp_typ2doc((*_tmp2BE).f3);_tmp25E[1]=_tmp66B;}),({
struct Cyc_PP_Doc*_tmp66A=Cyc_PP_text(({const char*_tmp260=")";_tag_dyneither(_tmp260,sizeof(char),2U);}));_tmp25E[2]=_tmp66A;}),({
struct Cyc_PP_Doc*_tmp669=({struct _dyneither_ptr _tmp668=({const char*_tmp261=",";_tag_dyneither(_tmp261,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp668,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BD));});_tmp25E[3]=_tmp669;});Cyc_PP_cat(_tag_dyneither(_tmp25E,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 26U: _LL3D: _tmp2BF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL3E:
# 1225
 s=({struct _dyneither_ptr _tmp66D=({const char*_tmp262=",";_tag_dyneither(_tmp262,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp66D,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BF));});
goto _LL0;case 27U: _LL3F: _tmp2C2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2C0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL40:
# 1228
 s=({struct Cyc_PP_Doc*_tmp263[8U];({struct Cyc_PP_Doc*_tmp675=Cyc_Absynpp_lb();_tmp263[0]=_tmp675;}),({struct Cyc_PP_Doc*_tmp674=Cyc_PP_text(({const char*_tmp264="for ";_tag_dyneither(_tmp264,sizeof(char),5U);}));_tmp263[1]=_tmp674;}),({
struct Cyc_PP_Doc*_tmp673=Cyc_PP_text(*(*_tmp2C2->name).f2);_tmp263[2]=_tmp673;}),({
struct Cyc_PP_Doc*_tmp672=Cyc_PP_text(({const char*_tmp265=" < ";_tag_dyneither(_tmp265,sizeof(char),4U);}));_tmp263[3]=_tmp672;}),({
struct Cyc_PP_Doc*_tmp671=Cyc_Absynpp_exp2doc(_tmp2C1);_tmp263[4]=_tmp671;}),({
struct Cyc_PP_Doc*_tmp670=Cyc_PP_text(({const char*_tmp266=" : ";_tag_dyneither(_tmp266,sizeof(char),4U);}));_tmp263[5]=_tmp670;}),({
struct Cyc_PP_Doc*_tmp66F=Cyc_Absynpp_exp2doc(_tmp2C0);_tmp263[6]=_tmp66F;}),({
struct Cyc_PP_Doc*_tmp66E=Cyc_Absynpp_rb();_tmp263[7]=_tmp66E;});Cyc_PP_cat(_tag_dyneither(_tmp263,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 28U: _LL41: _tmp2C4=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C3=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL42:
# 1237
 s=({struct Cyc_PP_Doc*_tmp267[7U];({struct Cyc_PP_Doc*_tmp67C=Cyc_Absynpp_lb();_tmp267[0]=_tmp67C;}),({struct Cyc_PP_Doc*_tmp67B=Cyc_PP_text(({const char*_tmp268="for x ";_tag_dyneither(_tmp268,sizeof(char),7U);}));_tmp267[1]=_tmp67B;}),({
struct Cyc_PP_Doc*_tmp67A=Cyc_PP_text(({const char*_tmp269=" < ";_tag_dyneither(_tmp269,sizeof(char),4U);}));_tmp267[2]=_tmp67A;}),({
struct Cyc_PP_Doc*_tmp679=Cyc_Absynpp_exp2doc(_tmp2C4);_tmp267[3]=_tmp679;}),({
struct Cyc_PP_Doc*_tmp678=Cyc_PP_text(({const char*_tmp26A=" : ";_tag_dyneither(_tmp26A,sizeof(char),4U);}));_tmp267[4]=_tmp678;}),({
struct Cyc_PP_Doc*_tmp677=Cyc_Absynpp_typ2doc(_tmp2C3);_tmp267[5]=_tmp677;}),({
struct Cyc_PP_Doc*_tmp676=Cyc_Absynpp_rb();_tmp267[6]=_tmp676;});Cyc_PP_cat(_tag_dyneither(_tmp267,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 29U: _LL43: _tmp2C7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_tmp2C5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL44: {
# 1245
struct Cyc_List_List*_tmp26B=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C5);
s=({struct Cyc_PP_Doc*_tmp26C[2U];({struct Cyc_PP_Doc*_tmp680=Cyc_Absynpp_qvar2doc(_tmp2C7);_tmp26C[0]=_tmp680;}),({
struct Cyc_PP_Doc*_tmp67F=({struct _dyneither_ptr _tmp67E=({const char*_tmp26D=",";_tag_dyneither(_tmp26D,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp67E,
_tmp2C6 != 0?({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*_tmp26E));({struct Cyc_PP_Doc*_tmp67D=Cyc_Absynpp_tps2doc(_tmp2C6);_tmp26E->hd=_tmp67D;}),_tmp26E->tl=_tmp26B;_tmp26E;}): _tmp26B);});
# 1247
_tmp26C[1]=_tmp67F;});Cyc_PP_cat(_tag_dyneither(_tmp26C,sizeof(struct Cyc_PP_Doc*),2U));});
# 1249
goto _LL0;}case 30U: _LL45: _tmp2C8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL46:
# 1251
 s=({struct _dyneither_ptr _tmp681=({const char*_tmp26F=",";_tag_dyneither(_tmp26F,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp681,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C8));});
goto _LL0;case 31U: _LL47: _tmp2CA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2C9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp201)->f3;_LL48:
# 1254
 if(_tmp2CA == 0)
# 1256
s=Cyc_Absynpp_qvar2doc(_tmp2C9->name);else{
# 1258
s=({struct Cyc_PP_Doc*_tmp270[2U];({struct Cyc_PP_Doc*_tmp686=Cyc_Absynpp_qvar2doc(_tmp2C9->name);_tmp270[0]=_tmp686;}),({
struct Cyc_PP_Doc*_tmp685=({struct _dyneither_ptr _tmp684=({const char*_tmp271="(";_tag_dyneither(_tmp271,sizeof(char),2U);});struct _dyneither_ptr _tmp683=({const char*_tmp272=")";_tag_dyneither(_tmp272,sizeof(char),2U);});struct _dyneither_ptr _tmp682=({const char*_tmp273=",";_tag_dyneither(_tmp273,sizeof(char),2U);});Cyc_PP_egroup(_tmp684,_tmp683,_tmp682,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2CA));});_tmp270[1]=_tmp685;});Cyc_PP_cat(_tag_dyneither(_tmp270,sizeof(struct Cyc_PP_Doc*),2U));});}
goto _LL0;case 32U: _LL49: _tmp2CB=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL4A:
# 1262
 s=Cyc_Absynpp_qvar2doc(_tmp2CB->name);
goto _LL0;case 33U: _LL4B: _tmp2CC=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL4C:
# 1265
 s=Cyc_Absynpp_qvar2doc(_tmp2CC->name);
goto _LL0;case 34U: _LL4D: _tmp2D1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).is_calloc;_tmp2D0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).rgn;_tmp2CF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).elt_type;_tmp2CE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).num_elts;_tmp2CD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp201)->f1).inline_call;_LL4E:
# 1270
 if(_tmp2D1){
# 1272
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*((void**)_check_null(_tmp2CF)),0U);
if(_tmp2D0 == 0)
s=({struct Cyc_PP_Doc*_tmp274[5U];({struct Cyc_PP_Doc*_tmp68B=Cyc_PP_text(({const char*_tmp275="calloc(";_tag_dyneither(_tmp275,sizeof(char),8U);}));_tmp274[0]=_tmp68B;}),({struct Cyc_PP_Doc*_tmp68A=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp274[1]=_tmp68A;}),({struct Cyc_PP_Doc*_tmp689=Cyc_PP_text(({const char*_tmp276=",";_tag_dyneither(_tmp276,sizeof(char),2U);}));_tmp274[2]=_tmp689;}),({struct Cyc_PP_Doc*_tmp688=Cyc_Absynpp_exp2doc(st);_tmp274[3]=_tmp688;}),({struct Cyc_PP_Doc*_tmp687=Cyc_PP_text(({const char*_tmp277=")";_tag_dyneither(_tmp277,sizeof(char),2U);}));_tmp274[4]=_tmp687;});Cyc_PP_cat(_tag_dyneither(_tmp274,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1276
s=({struct Cyc_PP_Doc*_tmp278[7U];({struct Cyc_PP_Doc*_tmp692=Cyc_PP_text(({const char*_tmp279="rcalloc(";_tag_dyneither(_tmp279,sizeof(char),9U);}));_tmp278[0]=_tmp692;}),({struct Cyc_PP_Doc*_tmp691=Cyc_Absynpp_exp2doc(_tmp2D0);_tmp278[1]=_tmp691;}),({struct Cyc_PP_Doc*_tmp690=Cyc_PP_text(({const char*_tmp27A=",";_tag_dyneither(_tmp27A,sizeof(char),2U);}));_tmp278[2]=_tmp690;}),({
struct Cyc_PP_Doc*_tmp68F=Cyc_Absynpp_exp2doc(_tmp2CE);_tmp278[3]=_tmp68F;}),({struct Cyc_PP_Doc*_tmp68E=Cyc_PP_text(({const char*_tmp27B=",";_tag_dyneither(_tmp27B,sizeof(char),2U);}));_tmp278[4]=_tmp68E;}),({struct Cyc_PP_Doc*_tmp68D=Cyc_Absynpp_exp2doc(st);_tmp278[5]=_tmp68D;}),({struct Cyc_PP_Doc*_tmp68C=Cyc_PP_text(({const char*_tmp27C=")";_tag_dyneither(_tmp27C,sizeof(char),2U);}));_tmp278[6]=_tmp68C;});Cyc_PP_cat(_tag_dyneither(_tmp278,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1279
struct Cyc_Absyn_Exp*new_e;
# 1281
if(_tmp2CF == 0)
new_e=_tmp2CE;else{
# 1284
new_e=({struct Cyc_Absyn_Exp*_tmp693=Cyc_Absyn_sizeoftype_exp(*_tmp2CF,0U);Cyc_Absyn_times_exp(_tmp693,_tmp2CE,0U);});}
# 1286
if(_tmp2D0 == 0)
s=({struct Cyc_PP_Doc*_tmp27D[3U];({struct Cyc_PP_Doc*_tmp696=Cyc_PP_text(({const char*_tmp27E="malloc(";_tag_dyneither(_tmp27E,sizeof(char),8U);}));_tmp27D[0]=_tmp696;}),({struct Cyc_PP_Doc*_tmp695=Cyc_Absynpp_exp2doc(new_e);_tmp27D[1]=_tmp695;}),({struct Cyc_PP_Doc*_tmp694=Cyc_PP_text(({const char*_tmp27F=")";_tag_dyneither(_tmp27F,sizeof(char),2U);}));_tmp27D[2]=_tmp694;});Cyc_PP_cat(_tag_dyneither(_tmp27D,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1289
if(_tmp2CD)
s=({struct Cyc_PP_Doc*_tmp280[5U];({struct Cyc_PP_Doc*_tmp69B=Cyc_PP_text(({const char*_tmp281="rmalloc_inline(";_tag_dyneither(_tmp281,sizeof(char),16U);}));_tmp280[0]=_tmp69B;}),({struct Cyc_PP_Doc*_tmp69A=Cyc_Absynpp_exp2doc(_tmp2D0);_tmp280[1]=_tmp69A;}),({struct Cyc_PP_Doc*_tmp699=Cyc_PP_text(({const char*_tmp282=",";_tag_dyneither(_tmp282,sizeof(char),2U);}));_tmp280[2]=_tmp699;}),({
struct Cyc_PP_Doc*_tmp698=Cyc_Absynpp_exp2doc(new_e);_tmp280[3]=_tmp698;}),({struct Cyc_PP_Doc*_tmp697=Cyc_PP_text(({const char*_tmp283=")";_tag_dyneither(_tmp283,sizeof(char),2U);}));_tmp280[4]=_tmp697;});Cyc_PP_cat(_tag_dyneither(_tmp280,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1293
s=({struct Cyc_PP_Doc*_tmp284[5U];({struct Cyc_PP_Doc*_tmp6A0=Cyc_PP_text(({const char*_tmp285="rmalloc(";_tag_dyneither(_tmp285,sizeof(char),9U);}));_tmp284[0]=_tmp6A0;}),({struct Cyc_PP_Doc*_tmp69F=Cyc_Absynpp_exp2doc(_tmp2D0);_tmp284[1]=_tmp69F;}),({struct Cyc_PP_Doc*_tmp69E=Cyc_PP_text(({const char*_tmp286=",";_tag_dyneither(_tmp286,sizeof(char),2U);}));_tmp284[2]=_tmp69E;}),({
struct Cyc_PP_Doc*_tmp69D=Cyc_Absynpp_exp2doc(new_e);_tmp284[3]=_tmp69D;}),({struct Cyc_PP_Doc*_tmp69C=Cyc_PP_text(({const char*_tmp287=")";_tag_dyneither(_tmp287,sizeof(char),2U);}));_tmp284[4]=_tmp69C;});Cyc_PP_cat(_tag_dyneither(_tmp284,sizeof(struct Cyc_PP_Doc*),5U));});}}}
# 1297
goto _LL0;case 35U: _LL4F: _tmp2D3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D2=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL50:
# 1299
 s=({struct Cyc_PP_Doc*_tmp288[3U];({struct Cyc_PP_Doc*_tmp6A3=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D3);_tmp288[0]=_tmp6A3;}),({
struct Cyc_PP_Doc*_tmp6A2=Cyc_PP_text(({const char*_tmp289=" :=: ";_tag_dyneither(_tmp289,sizeof(char),6U);}));_tmp288[1]=_tmp6A2;}),({
struct Cyc_PP_Doc*_tmp6A1=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D2);_tmp288[2]=_tmp6A1;});Cyc_PP_cat(_tag_dyneither(_tmp288,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 36U: _LL51: _tmp2D5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_tmp2D4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp201)->f2;_LL52:
# 1305
 s=({struct _dyneither_ptr _tmp6A4=({const char*_tmp28A=",";_tag_dyneither(_tmp28A,sizeof(char),2U);});Cyc_Absynpp_group_braces(_tmp6A4,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D4));});
goto _LL0;default: _LL53: _tmp2D6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp201)->f1;_LL54:
# 1308
 s=({struct Cyc_PP_Doc*_tmp28B[7U];({struct Cyc_PP_Doc*_tmp6AB=Cyc_PP_text(({const char*_tmp28C="(";_tag_dyneither(_tmp28C,sizeof(char),2U);}));_tmp28B[0]=_tmp6AB;}),({struct Cyc_PP_Doc*_tmp6AA=Cyc_Absynpp_lb();_tmp28B[1]=_tmp6AA;}),({struct Cyc_PP_Doc*_tmp6A9=Cyc_PP_blank_doc();_tmp28B[2]=_tmp6A9;}),({
struct Cyc_PP_Doc*_tmp6A8=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2D6,1));_tmp28B[3]=_tmp6A8;}),({
struct Cyc_PP_Doc*_tmp6A7=Cyc_PP_blank_doc();_tmp28B[4]=_tmp6A7;}),({struct Cyc_PP_Doc*_tmp6A6=Cyc_Absynpp_rb();_tmp28B[5]=_tmp6A6;}),({struct Cyc_PP_Doc*_tmp6A5=Cyc_PP_text(({const char*_tmp28D=")";_tag_dyneither(_tmp28D,sizeof(char),2U);}));_tmp28B[6]=_tmp6A5;});Cyc_PP_cat(_tag_dyneither(_tmp28B,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;}_LL0:;}
# 1313
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_tmp2D7[3U];({struct Cyc_PP_Doc*_tmp6AD=Cyc_PP_text(({const char*_tmp2D8="(";_tag_dyneither(_tmp2D8,sizeof(char),2U);}));_tmp2D7[0]=_tmp6AD;}),_tmp2D7[1]=s,({struct Cyc_PP_Doc*_tmp6AC=Cyc_PP_text(({const char*_tmp2D9=")";_tag_dyneither(_tmp2D9,sizeof(char),2U);}));_tmp2D7[2]=_tmp6AC;});Cyc_PP_cat(_tag_dyneither(_tmp2D7,sizeof(struct Cyc_PP_Doc*),3U));});
return s;}
# 1318
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp2DA=d;struct _dyneither_ptr*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E0;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DA)->tag == 0U){_LL1: _tmp2E0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2DA)->f1;_LL2:
 return({struct Cyc_PP_Doc*_tmp2DB[3U];({struct Cyc_PP_Doc*_tmp6B0=Cyc_PP_text(({const char*_tmp2DC=".[";_tag_dyneither(_tmp2DC,sizeof(char),3U);}));_tmp2DB[0]=_tmp6B0;}),({struct Cyc_PP_Doc*_tmp6AF=Cyc_Absynpp_exp2doc(_tmp2E0);_tmp2DB[1]=_tmp6AF;}),({struct Cyc_PP_Doc*_tmp6AE=Cyc_PP_text(({const char*_tmp2DD="]";_tag_dyneither(_tmp2DD,sizeof(char),2U);}));_tmp2DB[2]=_tmp6AE;});Cyc_PP_cat(_tag_dyneither(_tmp2DB,sizeof(struct Cyc_PP_Doc*),3U));});}else{_LL3: _tmp2E1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2DA)->f1;_LL4:
 return({struct Cyc_PP_Doc*_tmp2DE[2U];({struct Cyc_PP_Doc*_tmp6B2=Cyc_PP_text(({const char*_tmp2DF=".";_tag_dyneither(_tmp2DF,sizeof(char),2U);}));_tmp2DE[0]=_tmp6B2;}),({struct Cyc_PP_Doc*_tmp6B1=Cyc_PP_textptr(_tmp2E1);_tmp2DE[1]=_tmp6B1;});Cyc_PP_cat(_tag_dyneither(_tmp2DE,sizeof(struct Cyc_PP_Doc*),2U));});}_LL0:;}
# 1325
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
return({struct Cyc_PP_Doc*_tmp2E2[2U];({struct Cyc_PP_Doc*_tmp6B7=({struct _dyneither_ptr _tmp6B6=({const char*_tmp2E3="";_tag_dyneither(_tmp2E3,sizeof(char),1U);});struct _dyneither_ptr _tmp6B5=({const char*_tmp2E4="=";_tag_dyneither(_tmp2E4,sizeof(char),2U);});struct _dyneither_ptr _tmp6B4=({const char*_tmp2E5="=";_tag_dyneither(_tmp2E5,sizeof(char),2U);});Cyc_PP_egroup(_tmp6B6,_tmp6B5,_tmp6B4,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));});_tmp2E2[0]=_tmp6B7;}),({
struct Cyc_PP_Doc*_tmp6B3=Cyc_Absynpp_exp2doc((*de).f2);_tmp2E2[1]=_tmp6B3;});Cyc_PP_cat(_tag_dyneither(_tmp2E2,sizeof(struct Cyc_PP_Doc*),2U));});}}
# 1331
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _dyneither_ptr _tmp6BA=({const char*_tmp2E6="";_tag_dyneither(_tmp2E6,sizeof(char),1U);});struct _dyneither_ptr _tmp6B9=({const char*_tmp2E7="";_tag_dyneither(_tmp2E7,sizeof(char),1U);});struct _dyneither_ptr _tmp6B8=({const char*_tmp2E8=",";_tag_dyneither(_tmp2E8,sizeof(char),2U);});Cyc_PP_group(_tmp6BA,_tmp6B9,_tmp6B8,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1335
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _dyneither_ptr x=({char*_tmp2FD=({unsigned int _tmp2FC=28U + 1U;char*_tmp2FB=_cycalloc_atomic(_check_times(_tmp2FC,sizeof(char)));({{unsigned int _tmp541=28U;unsigned int i;for(i=0;i < _tmp541;++ i){_tmp2FB[i]='z';}_tmp2FB[_tmp541]=0;}0;});_tmp2FB;});_tag_dyneither(_tmp2FD,sizeof(char),29U);});
({struct _dyneither_ptr _tmp2E9=_dyneither_ptr_plus(x,sizeof(char),27);char _tmp2EA=*((char*)_check_dyneither_subscript(_tmp2E9,sizeof(char),0U));char _tmp2EB='\000';if(_get_dyneither_size(_tmp2E9,sizeof(char))== 1U  && (_tmp2EA == 0  && _tmp2EB != 0))_throw_arraybounds();*((char*)_tmp2E9.curr)=_tmp2EB;});
({struct _dyneither_ptr _tmp2EC=_dyneither_ptr_plus(x,sizeof(char),26);char _tmp2ED=*((char*)_check_dyneither_subscript(_tmp2EC,sizeof(char),0U));char _tmp2EE='L';if(_get_dyneither_size(_tmp2EC,sizeof(char))== 1U  && (_tmp2ED == 0  && _tmp2EE != 0))_throw_arraybounds();*((char*)_tmp2EC.curr)=_tmp2EE;});
({struct _dyneither_ptr _tmp2EF=_dyneither_ptr_plus(x,sizeof(char),25);char _tmp2F0=*((char*)_check_dyneither_subscript(_tmp2EF,sizeof(char),0U));char _tmp2F1='L';if(_get_dyneither_size(_tmp2EF,sizeof(char))== 1U  && (_tmp2F0 == 0  && _tmp2F1 != 0))_throw_arraybounds();*((char*)_tmp2EF.curr)=_tmp2F1;});
({struct _dyneither_ptr _tmp2F2=_dyneither_ptr_plus(x,sizeof(char),24);char _tmp2F3=*((char*)_check_dyneither_subscript(_tmp2F2,sizeof(char),0U));char _tmp2F4='U';if(_get_dyneither_size(_tmp2F2,sizeof(char))== 1U  && (_tmp2F3 == 0  && _tmp2F4 != 0))_throw_arraybounds();*((char*)_tmp2F2.curr)=_tmp2F4;});
({struct _dyneither_ptr _tmp2F5=_dyneither_ptr_plus(x,sizeof(char),23);char _tmp2F6=*((char*)_check_dyneither_subscript(_tmp2F5,sizeof(char),0U));char _tmp2F7='0';if(_get_dyneither_size(_tmp2F5,sizeof(char))== 1U  && (_tmp2F6 == 0  && _tmp2F7 != 0))_throw_arraybounds();*((char*)_tmp2F5.curr)=_tmp2F7;});{
int index=23;
while(i != (unsigned long long)0){
char c=(char)((unsigned long long)'0' + i % (unsigned long long)10);
({struct _dyneither_ptr _tmp2F8=_dyneither_ptr_plus(x,sizeof(char),index);char _tmp2F9=*((char*)_check_dyneither_subscript(_tmp2F8,sizeof(char),0U));char _tmp2FA=c;if(_get_dyneither_size(_tmp2F8,sizeof(char))== 1U  && (_tmp2F9 == 0  && _tmp2FA != 0))_throw_arraybounds();*((char*)_tmp2F8.curr)=_tmp2FA;});
i=i / (unsigned long long)10;
-- index;}
# 1349
return(struct _dyneither_ptr)_dyneither_ptr_plus(_dyneither_ptr_plus(x,sizeof(char),index),sizeof(char),1);};}
# 1353
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp2FE=c;struct _dyneither_ptr _tmp320;struct _dyneither_ptr _tmp31F;struct _dyneither_ptr _tmp31E;enum Cyc_Absyn_Sign _tmp31D;long long _tmp31C;enum Cyc_Absyn_Sign _tmp31B;int _tmp31A;enum Cyc_Absyn_Sign _tmp319;short _tmp318;struct _dyneither_ptr _tmp317;enum Cyc_Absyn_Sign _tmp316;char _tmp315;switch((_tmp2FE.String_c).tag){case 2U: _LL1: _tmp316=((_tmp2FE.Char_c).val).f1;_tmp315=((_tmp2FE.Char_c).val).f2;_LL2:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp301=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,({struct _dyneither_ptr _tmp6BB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp315));_tmp542.f1=_tmp6BB;});_tmp542;});void*_tmp2FF[1U];_tmp2FF[0]=& _tmp301;({struct _dyneither_ptr _tmp6BC=({const char*_tmp300="'%s'";_tag_dyneither(_tmp300,sizeof(char),5U);});Cyc_aprintf(_tmp6BC,_tag_dyneither(_tmp2FF,sizeof(void*),1U));});}));case 3U: _LL3: _tmp317=(_tmp2FE.Wchar_c).val;_LL4:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp304=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,_tmp543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp317);_tmp543;});void*_tmp302[1U];_tmp302[0]=& _tmp304;({struct _dyneither_ptr _tmp6BD=({const char*_tmp303="L'%s'";_tag_dyneither(_tmp303,sizeof(char),6U);});Cyc_aprintf(_tmp6BD,_tag_dyneither(_tmp302,sizeof(void*),1U));});}));case 4U: _LL5: _tmp319=((_tmp2FE.Short_c).val).f1;_tmp318=((_tmp2FE.Short_c).val).f2;_LL6:
 return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp307=({struct Cyc_Int_pa_PrintArg_struct _tmp544;_tmp544.tag=1U,_tmp544.f1=(unsigned long)((int)_tmp318);_tmp544;});void*_tmp305[1U];_tmp305[0]=& _tmp307;({struct _dyneither_ptr _tmp6BE=({const char*_tmp306="%d";_tag_dyneither(_tmp306,sizeof(char),3U);});Cyc_aprintf(_tmp6BE,_tag_dyneither(_tmp305,sizeof(void*),1U));});}));case 5U: _LL7: _tmp31B=((_tmp2FE.Int_c).val).f1;_tmp31A=((_tmp2FE.Int_c).val).f2;_LL8:
# 1359
 if((int)_tmp31B == (int)1U)
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30A=({struct Cyc_Int_pa_PrintArg_struct _tmp545;_tmp545.tag=1U,_tmp545.f1=(unsigned int)_tmp31A;_tmp545;});void*_tmp308[1U];_tmp308[0]=& _tmp30A;({struct _dyneither_ptr _tmp6BF=({const char*_tmp309="%uU";_tag_dyneither(_tmp309,sizeof(char),4U);});Cyc_aprintf(_tmp6BF,_tag_dyneither(_tmp308,sizeof(void*),1U));});}));else{
# 1362
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30D=({struct Cyc_Int_pa_PrintArg_struct _tmp546;_tmp546.tag=1U,_tmp546.f1=(unsigned long)_tmp31A;_tmp546;});void*_tmp30B[1U];_tmp30B[0]=& _tmp30D;({struct _dyneither_ptr _tmp6C0=({const char*_tmp30C="%d";_tag_dyneither(_tmp30C,sizeof(char),3U);});Cyc_aprintf(_tmp6C0,_tag_dyneither(_tmp30B,sizeof(void*),1U));});}));}case 6U: _LL9: _tmp31D=((_tmp2FE.LongLong_c).val).f1;_tmp31C=((_tmp2FE.LongLong_c).val).f2;_LLA:
# 1365
 return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)_tmp31C));case 7U: _LLB: _tmp31E=((_tmp2FE.Float_c).val).f1;_LLC:
 return Cyc_PP_text(_tmp31E);case 1U: _LLD: _LLE:
 return Cyc_PP_text(({const char*_tmp30E="NULL";_tag_dyneither(_tmp30E,sizeof(char),5U);}));case 8U: _LLF: _tmp31F=(_tmp2FE.String_c).val;_LL10:
# 1369
 return({struct Cyc_PP_Doc*_tmp30F[3U];({struct Cyc_PP_Doc*_tmp6C3=Cyc_PP_text(({const char*_tmp310="\"";_tag_dyneither(_tmp310,sizeof(char),2U);}));_tmp30F[0]=_tmp6C3;}),({struct Cyc_PP_Doc*_tmp6C2=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp31F));_tmp30F[1]=_tmp6C2;}),({struct Cyc_PP_Doc*_tmp6C1=Cyc_PP_text(({const char*_tmp311="\"";_tag_dyneither(_tmp311,sizeof(char),2U);}));_tmp30F[2]=_tmp6C1;});Cyc_PP_cat(_tag_dyneither(_tmp30F,sizeof(struct Cyc_PP_Doc*),3U));});default: _LL11: _tmp320=(_tmp2FE.Wstring_c).val;_LL12:
# 1371
 return({struct Cyc_PP_Doc*_tmp312[3U];({struct Cyc_PP_Doc*_tmp6C6=Cyc_PP_text(({const char*_tmp313="L\"";_tag_dyneither(_tmp313,sizeof(char),3U);}));_tmp312[0]=_tmp6C6;}),({struct Cyc_PP_Doc*_tmp6C5=Cyc_PP_text(_tmp320);_tmp312[1]=_tmp6C5;}),({struct Cyc_PP_Doc*_tmp6C4=Cyc_PP_text(({const char*_tmp314="\"";_tag_dyneither(_tmp314,sizeof(char),2U);}));_tmp312[2]=_tmp6C4;});Cyc_PP_cat(_tag_dyneither(_tmp312,sizeof(struct Cyc_PP_Doc*),3U));});}_LL0:;}
# 1375
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p == (int)19U){
if(es == 0  || es->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6C9=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp547;_tmp547.tag=0U,({struct _dyneither_ptr _tmp6C7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp547.f1=_tmp6C7;});_tmp547;});void*_tmp321[1U];_tmp321[0]=& _tmp323;({struct _dyneither_ptr _tmp6C8=({const char*_tmp322="Absynpp::primapp2doc Numelts: %s with bad args";_tag_dyneither(_tmp322,sizeof(char),47U);});Cyc_aprintf(_tmp6C8,_tag_dyneither(_tmp321,sizeof(void*),1U));});});_tmp324->f1=_tmp6C9;});_tmp324;}));
# 1381
return({struct Cyc_PP_Doc*_tmp325[3U];({struct Cyc_PP_Doc*_tmp6CC=Cyc_PP_text(({const char*_tmp326="numelts(";_tag_dyneither(_tmp326,sizeof(char),9U);}));_tmp325[0]=_tmp6CC;}),({struct Cyc_PP_Doc*_tmp6CB=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp325[1]=_tmp6CB;}),({struct Cyc_PP_Doc*_tmp6CA=Cyc_PP_text(({const char*_tmp327=")";_tag_dyneither(_tmp327,sizeof(char),2U);}));_tmp325[2]=_tmp6CA;});Cyc_PP_cat(_tag_dyneither(_tmp325,sizeof(struct Cyc_PP_Doc*),3U));});}else{
# 1383
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6CF=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32A=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,({struct _dyneither_ptr _tmp6CD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp548.f1=_tmp6CD;});_tmp548;});void*_tmp328[1U];_tmp328[0]=& _tmp32A;({struct _dyneither_ptr _tmp6CE=({const char*_tmp329="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp329,sizeof(char),38U);});Cyc_aprintf(_tmp6CE,_tag_dyneither(_tmp328,sizeof(void*),1U));});});_tmp32B->f1=_tmp6CF;});_tmp32B;}));else{
# 1387
if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp32C[3U];_tmp32C[0]=ps,({struct Cyc_PP_Doc*_tmp6D0=Cyc_PP_text(({const char*_tmp32D=" ";_tag_dyneither(_tmp32D,sizeof(char),2U);}));_tmp32C[1]=_tmp6D0;}),_tmp32C[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp32C,sizeof(struct Cyc_PP_Doc*),3U));});else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp6D3=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,({struct _dyneither_ptr _tmp6D1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));_tmp549.f1=_tmp6D1;});_tmp549;});void*_tmp32E[1U];_tmp32E[0]=& _tmp330;({struct _dyneither_ptr _tmp6D2=({const char*_tmp32F="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp32F,sizeof(char),47U);});Cyc_aprintf(_tmp6D2,_tag_dyneither(_tmp32E,sizeof(void*),1U));});});_tmp331->f1=_tmp6D3;});_tmp331;}));else{
# 1393
return({struct Cyc_PP_Doc*_tmp332[5U];_tmp332[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_tmp6D5=Cyc_PP_text(({const char*_tmp333=" ";_tag_dyneither(_tmp333,sizeof(char),2U);}));_tmp332[1]=_tmp6D5;}),_tmp332[2]=ps,({struct Cyc_PP_Doc*_tmp6D4=Cyc_PP_text(({const char*_tmp334=" ";_tag_dyneither(_tmp334,sizeof(char),2U);}));_tmp332[3]=_tmp6D4;}),_tmp332[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd;Cyc_PP_cat(_tag_dyneither(_tmp332,sizeof(struct Cyc_PP_Doc*),5U));});}}}}}
# 1397
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp335=p;switch(_tmp335){case Cyc_Absyn_Plus: _LL1: _LL2:
 return({const char*_tmp336="+";_tag_dyneither(_tmp336,sizeof(char),2U);});case Cyc_Absyn_Times: _LL3: _LL4:
 return({const char*_tmp337="*";_tag_dyneither(_tmp337,sizeof(char),2U);});case Cyc_Absyn_Minus: _LL5: _LL6:
 return({const char*_tmp338="-";_tag_dyneither(_tmp338,sizeof(char),2U);});case Cyc_Absyn_Div: _LL7: _LL8:
 return({const char*_tmp339="/";_tag_dyneither(_tmp339,sizeof(char),2U);});case Cyc_Absyn_Mod: _LL9: _LLA:
 return Cyc_Absynpp_print_for_cycdoc?({const char*_tmp33A="\\%";_tag_dyneither(_tmp33A,sizeof(char),3U);}):({const char*_tmp33B="%";_tag_dyneither(_tmp33B,sizeof(char),2U);});case Cyc_Absyn_Eq: _LLB: _LLC:
 return({const char*_tmp33C="==";_tag_dyneither(_tmp33C,sizeof(char),3U);});case Cyc_Absyn_Neq: _LLD: _LLE:
 return({const char*_tmp33D="!=";_tag_dyneither(_tmp33D,sizeof(char),3U);});case Cyc_Absyn_Gt: _LLF: _LL10:
 return({const char*_tmp33E=">";_tag_dyneither(_tmp33E,sizeof(char),2U);});case Cyc_Absyn_Lt: _LL11: _LL12:
 return({const char*_tmp33F="<";_tag_dyneither(_tmp33F,sizeof(char),2U);});case Cyc_Absyn_Gte: _LL13: _LL14:
 return({const char*_tmp340=">=";_tag_dyneither(_tmp340,sizeof(char),3U);});case Cyc_Absyn_Lte: _LL15: _LL16:
 return({const char*_tmp341="<=";_tag_dyneither(_tmp341,sizeof(char),3U);});case Cyc_Absyn_Not: _LL17: _LL18:
 return({const char*_tmp342="!";_tag_dyneither(_tmp342,sizeof(char),2U);});case Cyc_Absyn_Bitnot: _LL19: _LL1A:
 return({const char*_tmp343="~";_tag_dyneither(_tmp343,sizeof(char),2U);});case Cyc_Absyn_Bitand: _LL1B: _LL1C:
 return({const char*_tmp344="&";_tag_dyneither(_tmp344,sizeof(char),2U);});case Cyc_Absyn_Bitor: _LL1D: _LL1E:
 return({const char*_tmp345="|";_tag_dyneither(_tmp345,sizeof(char),2U);});case Cyc_Absyn_Bitxor: _LL1F: _LL20:
 return({const char*_tmp346="^";_tag_dyneither(_tmp346,sizeof(char),2U);});case Cyc_Absyn_Bitlshift: _LL21: _LL22:
 return({const char*_tmp347="<<";_tag_dyneither(_tmp347,sizeof(char),3U);});case Cyc_Absyn_Bitlrshift: _LL23: _LL24:
 return({const char*_tmp348=">>";_tag_dyneither(_tmp348,sizeof(char),3U);});case Cyc_Absyn_Bitarshift: _LL25: _LL26:
 return({const char*_tmp349=">>>";_tag_dyneither(_tmp349,sizeof(char),4U);});case Cyc_Absyn_Numelts: _LL27: _LL28:
 return({const char*_tmp34A="numelts";_tag_dyneither(_tmp34A,sizeof(char),8U);});default: _LL29: _LL2A:
 return({const char*_tmp34B="?";_tag_dyneither(_tmp34B,sizeof(char),2U);});}_LL0:;}
# 1423
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1427
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp34C=s->r;void*_tmp34D=_tmp34C;if(((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34D)->tag == 12U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1434
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp34E=st->r;void*_tmp34F=_tmp34E;struct Cyc_Absyn_Stmt*_tmp3A9;struct Cyc_List_List*_tmp3A8;struct Cyc_Absyn_Stmt*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A6;struct _dyneither_ptr*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A4;struct Cyc_Absyn_Decl*_tmp3A3;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_List_List*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_List_List*_tmp39F;struct Cyc_Absyn_Exp*_tmp39E;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_Absyn_Stmt*_tmp39B;struct _dyneither_ptr*_tmp39A;struct Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Stmt*_tmp398;struct Cyc_Absyn_Exp*_tmp397;struct Cyc_Absyn_Stmt*_tmp396;struct Cyc_Absyn_Stmt*_tmp395;struct Cyc_Absyn_Exp*_tmp394;struct Cyc_Absyn_Stmt*_tmp393;struct Cyc_Absyn_Stmt*_tmp392;struct Cyc_Absyn_Exp*_tmp391;switch(*((int*)_tmp34F)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp350=";";_tag_dyneither(_tmp350,sizeof(char),2U);}));goto _LL0;case 1U: _LL3: _tmp391=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_LL4:
 s=({struct Cyc_PP_Doc*_tmp351[2U];({struct Cyc_PP_Doc*_tmp6D7=Cyc_Absynpp_exp2doc(_tmp391);_tmp351[0]=_tmp6D7;}),({struct Cyc_PP_Doc*_tmp6D6=Cyc_PP_text(({const char*_tmp352=";";_tag_dyneither(_tmp352,sizeof(char),2U);}));_tmp351[1]=_tmp6D6;});Cyc_PP_cat(_tag_dyneither(_tmp351,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL0;case 2U: _LL5: _tmp393=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp392=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2;_LL6:
# 1440
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp393))
s=({struct Cyc_PP_Doc*_tmp353[7U];({struct Cyc_PP_Doc*_tmp6EC=Cyc_Absynpp_lb();_tmp353[0]=_tmp6EC;}),({
struct Cyc_PP_Doc*_tmp6EB=Cyc_PP_blank_doc();_tmp353[1]=_tmp6EB;}),({
struct Cyc_PP_Doc*_tmp6EA=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp393,0));_tmp353[2]=_tmp6EA;}),({
struct Cyc_PP_Doc*_tmp6E9=Cyc_PP_line_doc();_tmp353[3]=_tmp6E9;}),({
struct Cyc_PP_Doc*_tmp6E8=Cyc_Absynpp_rb();_tmp353[4]=_tmp6E8;}),({
struct Cyc_PP_Doc*_tmp6E7=Cyc_PP_line_doc();_tmp353[5]=_tmp6E7;}),
Cyc_Absynpp_is_declaration(_tmp392)?
stmtexp?({
struct Cyc_PP_Doc*_tmp6E6=({struct Cyc_PP_Doc*_tmp354[7U];({struct Cyc_PP_Doc*_tmp6E5=Cyc_PP_text(({const char*_tmp355="(";_tag_dyneither(_tmp355,sizeof(char),2U);}));_tmp354[0]=_tmp6E5;}),({struct Cyc_PP_Doc*_tmp6E4=Cyc_Absynpp_lb();_tmp354[1]=_tmp6E4;}),({struct Cyc_PP_Doc*_tmp6E3=Cyc_PP_blank_doc();_tmp354[2]=_tmp6E3;}),({
struct Cyc_PP_Doc*_tmp6E2=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp354[3]=_tmp6E2;}),({
struct Cyc_PP_Doc*_tmp6E1=Cyc_Absynpp_rb();_tmp354[4]=_tmp6E1;}),({struct Cyc_PP_Doc*_tmp6E0=Cyc_PP_text(({const char*_tmp356=");";_tag_dyneither(_tmp356,sizeof(char),3U);}));_tmp354[5]=_tmp6E0;}),({
struct Cyc_PP_Doc*_tmp6DF=Cyc_PP_line_doc();_tmp354[6]=_tmp6DF;});Cyc_PP_cat(_tag_dyneither(_tmp354,sizeof(struct Cyc_PP_Doc*),7U));});
# 1450
_tmp353[6]=_tmp6E6;}):({
# 1454
struct Cyc_PP_Doc*_tmp6DE=({struct Cyc_PP_Doc*_tmp357[5U];({struct Cyc_PP_Doc*_tmp6DD=Cyc_Absynpp_lb();_tmp357[0]=_tmp6DD;}),({struct Cyc_PP_Doc*_tmp6DC=Cyc_PP_blank_doc();_tmp357[1]=_tmp6DC;}),({
struct Cyc_PP_Doc*_tmp6DB=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp357[2]=_tmp6DB;}),({
struct Cyc_PP_Doc*_tmp6DA=Cyc_Absynpp_rb();_tmp357[3]=_tmp6DA;}),({
struct Cyc_PP_Doc*_tmp6D9=Cyc_PP_line_doc();_tmp357[4]=_tmp6D9;});Cyc_PP_cat(_tag_dyneither(_tmp357,sizeof(struct Cyc_PP_Doc*),5U));});
# 1454
_tmp353[6]=_tmp6DE;}):({
# 1458
struct Cyc_PP_Doc*_tmp6D8=Cyc_Absynpp_stmt2doc(_tmp392,stmtexp);_tmp353[6]=_tmp6D8;});Cyc_PP_cat(_tag_dyneither(_tmp353,sizeof(struct Cyc_PP_Doc*),7U));});else{
if(Cyc_Absynpp_is_declaration(_tmp392))
s=({struct Cyc_PP_Doc*_tmp358[4U];({struct Cyc_PP_Doc*_tmp6FB=Cyc_Absynpp_stmt2doc(_tmp393,0);_tmp358[0]=_tmp6FB;}),({
struct Cyc_PP_Doc*_tmp6FA=Cyc_PP_line_doc();_tmp358[1]=_tmp6FA;}),
stmtexp?({
struct Cyc_PP_Doc*_tmp6F9=({struct Cyc_PP_Doc*_tmp359[6U];({struct Cyc_PP_Doc*_tmp6F8=Cyc_PP_text(({const char*_tmp35A="(";_tag_dyneither(_tmp35A,sizeof(char),2U);}));_tmp359[0]=_tmp6F8;}),({struct Cyc_PP_Doc*_tmp6F7=Cyc_Absynpp_lb();_tmp359[1]=_tmp6F7;}),({struct Cyc_PP_Doc*_tmp6F6=Cyc_PP_blank_doc();_tmp359[2]=_tmp6F6;}),({
struct Cyc_PP_Doc*_tmp6F5=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp359[3]=_tmp6F5;}),({
struct Cyc_PP_Doc*_tmp6F4=Cyc_Absynpp_rb();_tmp359[4]=_tmp6F4;}),({struct Cyc_PP_Doc*_tmp6F3=Cyc_PP_text(({const char*_tmp35B=");";_tag_dyneither(_tmp35B,sizeof(char),3U);}));_tmp359[5]=_tmp6F3;});Cyc_PP_cat(_tag_dyneither(_tmp359,sizeof(struct Cyc_PP_Doc*),6U));});
# 1463
_tmp358[2]=_tmp6F9;}):({
# 1467
struct Cyc_PP_Doc*_tmp6F2=({struct Cyc_PP_Doc*_tmp35C[4U];({struct Cyc_PP_Doc*_tmp6F1=Cyc_Absynpp_lb();_tmp35C[0]=_tmp6F1;}),({struct Cyc_PP_Doc*_tmp6F0=Cyc_PP_blank_doc();_tmp35C[1]=_tmp6F0;}),({
struct Cyc_PP_Doc*_tmp6EF=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp392,stmtexp));_tmp35C[2]=_tmp6EF;}),({
struct Cyc_PP_Doc*_tmp6EE=Cyc_Absynpp_rb();_tmp35C[3]=_tmp6EE;});Cyc_PP_cat(_tag_dyneither(_tmp35C,sizeof(struct Cyc_PP_Doc*),4U));});
# 1467
_tmp358[2]=_tmp6F2;}),({
# 1470
struct Cyc_PP_Doc*_tmp6ED=Cyc_PP_line_doc();_tmp358[3]=_tmp6ED;});Cyc_PP_cat(_tag_dyneither(_tmp358,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1472
s=({struct Cyc_PP_Doc*_tmp35D[3U];({struct Cyc_PP_Doc*_tmp6FE=Cyc_Absynpp_stmt2doc(_tmp393,0);_tmp35D[0]=_tmp6FE;}),({struct Cyc_PP_Doc*_tmp6FD=Cyc_PP_line_doc();_tmp35D[1]=_tmp6FD;}),({struct Cyc_PP_Doc*_tmp6FC=Cyc_Absynpp_stmt2doc(_tmp392,stmtexp);_tmp35D[2]=_tmp6FC;});Cyc_PP_cat(_tag_dyneither(_tmp35D,sizeof(struct Cyc_PP_Doc*),3U));});}}}else{
# 1475
s=({struct Cyc_PP_Doc*_tmp35E[3U];({struct Cyc_PP_Doc*_tmp701=Cyc_Absynpp_stmt2doc(_tmp393,0);_tmp35E[0]=_tmp701;}),({struct Cyc_PP_Doc*_tmp700=Cyc_PP_line_doc();_tmp35E[1]=_tmp700;}),({struct Cyc_PP_Doc*_tmp6FF=Cyc_Absynpp_stmt2doc(_tmp392,stmtexp);_tmp35E[2]=_tmp6FF;});Cyc_PP_cat(_tag_dyneither(_tmp35E,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 3U: _LL7: _tmp394=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_LL8:
# 1478
 if(_tmp394 == 0)
s=Cyc_PP_text(({const char*_tmp35F="return;";_tag_dyneither(_tmp35F,sizeof(char),8U);}));else{
# 1481
s=({struct Cyc_PP_Doc*_tmp360[3U];({struct Cyc_PP_Doc*_tmp705=Cyc_PP_text(({const char*_tmp361="return ";_tag_dyneither(_tmp361,sizeof(char),8U);}));_tmp360[0]=_tmp705;}),
_tmp394 == 0?({struct Cyc_PP_Doc*_tmp704=Cyc_PP_nil_doc();_tmp360[1]=_tmp704;}):({struct Cyc_PP_Doc*_tmp703=Cyc_Absynpp_exp2doc(_tmp394);_tmp360[1]=_tmp703;}),({
struct Cyc_PP_Doc*_tmp702=Cyc_PP_text(({const char*_tmp362=";";_tag_dyneither(_tmp362,sizeof(char),2U);}));_tmp360[2]=_tmp702;});Cyc_PP_cat(_tag_dyneither(_tmp360,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 4U: _LL9: _tmp397=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp396=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2;_tmp395=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34F)->f3;_LLA: {
# 1486
int print_else;
{void*_tmp363=_tmp395->r;void*_tmp364=_tmp363;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp364)->tag == 0U){_LL24: _LL25:
 print_else=0;goto _LL23;}else{_LL26: _LL27:
 print_else=1;goto _LL23;}_LL23:;}
# 1491
s=({struct Cyc_PP_Doc*_tmp365[8U];({struct Cyc_PP_Doc*_tmp718=Cyc_PP_text(({const char*_tmp366="if (";_tag_dyneither(_tmp366,sizeof(char),5U);}));_tmp365[0]=_tmp718;}),({
struct Cyc_PP_Doc*_tmp717=Cyc_Absynpp_exp2doc(_tmp397);_tmp365[1]=_tmp717;}),({
struct Cyc_PP_Doc*_tmp716=Cyc_PP_text(({const char*_tmp367=") ";_tag_dyneither(_tmp367,sizeof(char),3U);}));_tmp365[2]=_tmp716;}),({
struct Cyc_PP_Doc*_tmp715=Cyc_Absynpp_lb();_tmp365[3]=_tmp715;}),({
struct Cyc_PP_Doc*_tmp714=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp368[2U];({struct Cyc_PP_Doc*_tmp713=Cyc_PP_line_doc();_tmp368[0]=_tmp713;}),({struct Cyc_PP_Doc*_tmp712=Cyc_Absynpp_stmt2doc(_tmp396,0);_tmp368[1]=_tmp712;});Cyc_PP_cat(_tag_dyneither(_tmp368,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp365[4]=_tmp714;}),({
struct Cyc_PP_Doc*_tmp711=Cyc_PP_line_doc();_tmp365[5]=_tmp711;}),({
struct Cyc_PP_Doc*_tmp710=Cyc_Absynpp_rb();_tmp365[6]=_tmp710;}),
print_else?({
struct Cyc_PP_Doc*_tmp70F=({struct Cyc_PP_Doc*_tmp369[6U];({struct Cyc_PP_Doc*_tmp70E=Cyc_PP_line_doc();_tmp369[0]=_tmp70E;}),({
struct Cyc_PP_Doc*_tmp70D=Cyc_PP_text(({const char*_tmp36A="else ";_tag_dyneither(_tmp36A,sizeof(char),6U);}));_tmp369[1]=_tmp70D;}),({
struct Cyc_PP_Doc*_tmp70C=Cyc_Absynpp_lb();_tmp369[2]=_tmp70C;}),({
struct Cyc_PP_Doc*_tmp70B=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36B[2U];({struct Cyc_PP_Doc*_tmp70A=Cyc_PP_line_doc();_tmp36B[0]=_tmp70A;}),({struct Cyc_PP_Doc*_tmp709=Cyc_Absynpp_stmt2doc(_tmp395,0);_tmp36B[1]=_tmp709;});Cyc_PP_cat(_tag_dyneither(_tmp36B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp369[3]=_tmp70B;}),({
struct Cyc_PP_Doc*_tmp708=Cyc_PP_line_doc();_tmp369[4]=_tmp708;}),({
struct Cyc_PP_Doc*_tmp707=Cyc_Absynpp_rb();_tmp369[5]=_tmp707;});Cyc_PP_cat(_tag_dyneither(_tmp369,sizeof(struct Cyc_PP_Doc*),6U));});
# 1499
_tmp365[7]=_tmp70F;}):({
# 1505
struct Cyc_PP_Doc*_tmp706=Cyc_PP_nil_doc();_tmp365[7]=_tmp706;});Cyc_PP_cat(_tag_dyneither(_tmp365,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;}case 5U: _LLB: _tmp399=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1).f1;_tmp398=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2;_LLC:
# 1508
 s=({struct Cyc_PP_Doc*_tmp36C[7U];({struct Cyc_PP_Doc*_tmp721=Cyc_PP_text(({const char*_tmp36D="while (";_tag_dyneither(_tmp36D,sizeof(char),8U);}));_tmp36C[0]=_tmp721;}),({
struct Cyc_PP_Doc*_tmp720=Cyc_Absynpp_exp2doc(_tmp399);_tmp36C[1]=_tmp720;}),({
struct Cyc_PP_Doc*_tmp71F=Cyc_PP_text(({const char*_tmp36E=") ";_tag_dyneither(_tmp36E,sizeof(char),3U);}));_tmp36C[2]=_tmp71F;}),({
struct Cyc_PP_Doc*_tmp71E=Cyc_Absynpp_lb();_tmp36C[3]=_tmp71E;}),({
struct Cyc_PP_Doc*_tmp71D=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp36F[2U];({struct Cyc_PP_Doc*_tmp71C=Cyc_PP_line_doc();_tmp36F[0]=_tmp71C;}),({struct Cyc_PP_Doc*_tmp71B=Cyc_Absynpp_stmt2doc(_tmp398,0);_tmp36F[1]=_tmp71B;});Cyc_PP_cat(_tag_dyneither(_tmp36F,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp36C[4]=_tmp71D;}),({
struct Cyc_PP_Doc*_tmp71A=Cyc_PP_line_doc();_tmp36C[5]=_tmp71A;}),({
struct Cyc_PP_Doc*_tmp719=Cyc_Absynpp_rb();_tmp36C[6]=_tmp719;});Cyc_PP_cat(_tag_dyneither(_tmp36C,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 6U: _LLD: _LLE:
 s=Cyc_PP_text(({const char*_tmp370="break;";_tag_dyneither(_tmp370,sizeof(char),7U);}));goto _LL0;case 7U: _LLF: _LL10:
 s=Cyc_PP_text(({const char*_tmp371="continue;";_tag_dyneither(_tmp371,sizeof(char),10U);}));goto _LL0;case 8U: _LL11: _tmp39A=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_LL12:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp374=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,_tmp54A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39A);_tmp54A;});void*_tmp372[1U];_tmp372[0]=& _tmp374;({struct _dyneither_ptr _tmp722=({const char*_tmp373="goto %s;";_tag_dyneither(_tmp373,sizeof(char),9U);});Cyc_aprintf(_tmp722,_tag_dyneither(_tmp372,sizeof(void*),1U));});}));goto _LL0;case 9U: _LL13: _tmp39E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp39D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2).f1;_tmp39C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34F)->f3).f1;_tmp39B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34F)->f4;_LL14:
# 1520
 s=({struct Cyc_PP_Doc*_tmp375[11U];({struct Cyc_PP_Doc*_tmp72F=Cyc_PP_text(({const char*_tmp376="for(";_tag_dyneither(_tmp376,sizeof(char),5U);}));_tmp375[0]=_tmp72F;}),({
struct Cyc_PP_Doc*_tmp72E=Cyc_Absynpp_exp2doc(_tmp39E);_tmp375[1]=_tmp72E;}),({
struct Cyc_PP_Doc*_tmp72D=Cyc_PP_text(({const char*_tmp377="; ";_tag_dyneither(_tmp377,sizeof(char),3U);}));_tmp375[2]=_tmp72D;}),({
struct Cyc_PP_Doc*_tmp72C=Cyc_Absynpp_exp2doc(_tmp39D);_tmp375[3]=_tmp72C;}),({
struct Cyc_PP_Doc*_tmp72B=Cyc_PP_text(({const char*_tmp378="; ";_tag_dyneither(_tmp378,sizeof(char),3U);}));_tmp375[4]=_tmp72B;}),({
struct Cyc_PP_Doc*_tmp72A=Cyc_Absynpp_exp2doc(_tmp39C);_tmp375[5]=_tmp72A;}),({
struct Cyc_PP_Doc*_tmp729=Cyc_PP_text(({const char*_tmp379=") ";_tag_dyneither(_tmp379,sizeof(char),3U);}));_tmp375[6]=_tmp729;}),({
struct Cyc_PP_Doc*_tmp728=Cyc_Absynpp_lb();_tmp375[7]=_tmp728;}),({
struct Cyc_PP_Doc*_tmp727=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp37A[2U];({struct Cyc_PP_Doc*_tmp726=Cyc_PP_line_doc();_tmp37A[0]=_tmp726;}),({struct Cyc_PP_Doc*_tmp725=Cyc_Absynpp_stmt2doc(_tmp39B,0);_tmp37A[1]=_tmp725;});Cyc_PP_cat(_tag_dyneither(_tmp37A,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp375[8]=_tmp727;}),({
struct Cyc_PP_Doc*_tmp724=Cyc_PP_line_doc();_tmp375[9]=_tmp724;}),({
struct Cyc_PP_Doc*_tmp723=Cyc_Absynpp_rb();_tmp375[10]=_tmp723;});Cyc_PP_cat(_tag_dyneither(_tmp375,sizeof(struct Cyc_PP_Doc*),11U));});
goto _LL0;case 10U: _LL15: _tmp3A0=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp39F=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2;_LL16:
# 1533
 s=({struct Cyc_PP_Doc*_tmp37B[8U];({struct Cyc_PP_Doc*_tmp737=Cyc_PP_text(({const char*_tmp37C="switch (";_tag_dyneither(_tmp37C,sizeof(char),9U);}));_tmp37B[0]=_tmp737;}),({
struct Cyc_PP_Doc*_tmp736=Cyc_Absynpp_exp2doc(_tmp3A0);_tmp37B[1]=_tmp736;}),({
struct Cyc_PP_Doc*_tmp735=Cyc_PP_text(({const char*_tmp37D=") ";_tag_dyneither(_tmp37D,sizeof(char),3U);}));_tmp37B[2]=_tmp735;}),({
struct Cyc_PP_Doc*_tmp734=Cyc_Absynpp_lb();_tmp37B[3]=_tmp734;}),({
struct Cyc_PP_Doc*_tmp733=Cyc_PP_line_doc();_tmp37B[4]=_tmp733;}),({
struct Cyc_PP_Doc*_tmp732=Cyc_Absynpp_switchclauses2doc(_tmp39F);_tmp37B[5]=_tmp732;}),({
struct Cyc_PP_Doc*_tmp731=Cyc_PP_line_doc();_tmp37B[6]=_tmp731;}),({
struct Cyc_PP_Doc*_tmp730=Cyc_Absynpp_rb();_tmp37B[7]=_tmp730;});Cyc_PP_cat(_tag_dyneither(_tmp37B,sizeof(struct Cyc_PP_Doc*),8U));});
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1 == 0){_LL17: _LL18:
 s=Cyc_PP_text(({const char*_tmp37E="fallthru;";_tag_dyneither(_tmp37E,sizeof(char),10U);}));goto _LL0;}else{_LL19: _tmp3A1=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_LL1A:
# 1544
 s=({struct Cyc_PP_Doc*_tmp37F[3U];({struct Cyc_PP_Doc*_tmp73A=Cyc_PP_text(({const char*_tmp380="fallthru(";_tag_dyneither(_tmp380,sizeof(char),10U);}));_tmp37F[0]=_tmp73A;}),({struct Cyc_PP_Doc*_tmp739=Cyc_Absynpp_exps2doc_prec(20,_tmp3A1);_tmp37F[1]=_tmp739;}),({struct Cyc_PP_Doc*_tmp738=Cyc_PP_text(({const char*_tmp381=");";_tag_dyneither(_tmp381,sizeof(char),3U);}));_tmp37F[2]=_tmp738;});Cyc_PP_cat(_tag_dyneither(_tmp37F,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 12U: _LL1B: _tmp3A3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp3A2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2;_LL1C:
# 1546
 s=({struct Cyc_PP_Doc*_tmp382[3U];({struct Cyc_PP_Doc*_tmp73D=Cyc_Absynpp_decl2doc(_tmp3A3);_tmp382[0]=_tmp73D;}),({
struct Cyc_PP_Doc*_tmp73C=Cyc_PP_line_doc();_tmp382[1]=_tmp73C;}),({
struct Cyc_PP_Doc*_tmp73B=Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp);_tmp382[2]=_tmp73B;});Cyc_PP_cat(_tag_dyneither(_tmp382,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 13U: _LL1D: _tmp3A5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp3A4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2;_LL1E:
# 1551
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3A4)){
if(stmtexp)
s=({struct Cyc_PP_Doc*_tmp383[8U];({struct Cyc_PP_Doc*_tmp745=Cyc_PP_textptr(_tmp3A5);_tmp383[0]=_tmp745;}),({
struct Cyc_PP_Doc*_tmp744=Cyc_PP_text(({const char*_tmp384=": (";_tag_dyneither(_tmp384,sizeof(char),4U);}));_tmp383[1]=_tmp744;}),({
struct Cyc_PP_Doc*_tmp743=Cyc_Absynpp_lb();_tmp383[2]=_tmp743;}),({
struct Cyc_PP_Doc*_tmp742=Cyc_PP_line_doc();_tmp383[3]=_tmp742;}),({
struct Cyc_PP_Doc*_tmp741=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A4,1));_tmp383[4]=_tmp741;}),({
struct Cyc_PP_Doc*_tmp740=Cyc_PP_line_doc();_tmp383[5]=_tmp740;}),({
struct Cyc_PP_Doc*_tmp73F=Cyc_Absynpp_rb();_tmp383[6]=_tmp73F;}),({struct Cyc_PP_Doc*_tmp73E=Cyc_PP_text(({const char*_tmp385=");";_tag_dyneither(_tmp385,sizeof(char),3U);}));_tmp383[7]=_tmp73E;});Cyc_PP_cat(_tag_dyneither(_tmp383,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1561
s=({struct Cyc_PP_Doc*_tmp386[7U];({struct Cyc_PP_Doc*_tmp74C=Cyc_PP_textptr(_tmp3A5);_tmp386[0]=_tmp74C;}),({
struct Cyc_PP_Doc*_tmp74B=Cyc_PP_text(({const char*_tmp387=": ";_tag_dyneither(_tmp387,sizeof(char),3U);}));_tmp386[1]=_tmp74B;}),({
struct Cyc_PP_Doc*_tmp74A=Cyc_Absynpp_lb();_tmp386[2]=_tmp74A;}),({
struct Cyc_PP_Doc*_tmp749=Cyc_PP_line_doc();_tmp386[3]=_tmp749;}),({
struct Cyc_PP_Doc*_tmp748=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A4,0));_tmp386[4]=_tmp748;}),({
struct Cyc_PP_Doc*_tmp747=Cyc_PP_line_doc();_tmp386[5]=_tmp747;}),({
struct Cyc_PP_Doc*_tmp746=Cyc_Absynpp_rb();_tmp386[6]=_tmp746;});Cyc_PP_cat(_tag_dyneither(_tmp386,sizeof(struct Cyc_PP_Doc*),7U));});}}else{
# 1569
s=({struct Cyc_PP_Doc*_tmp388[3U];({struct Cyc_PP_Doc*_tmp74F=Cyc_PP_textptr(_tmp3A5);_tmp388[0]=_tmp74F;}),({struct Cyc_PP_Doc*_tmp74E=Cyc_PP_text(({const char*_tmp389=": ";_tag_dyneither(_tmp389,sizeof(char),3U);}));_tmp388[1]=_tmp74E;}),({struct Cyc_PP_Doc*_tmp74D=Cyc_Absynpp_stmt2doc(_tmp3A4,stmtexp);_tmp388[2]=_tmp74D;});Cyc_PP_cat(_tag_dyneither(_tmp388,sizeof(struct Cyc_PP_Doc*),3U));});}
goto _LL0;case 14U: _LL1F: _tmp3A7=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp3A6=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2).f1;_LL20:
# 1572
 s=({struct Cyc_PP_Doc*_tmp38A[9U];({struct Cyc_PP_Doc*_tmp758=Cyc_PP_text(({const char*_tmp38B="do ";_tag_dyneither(_tmp38B,sizeof(char),4U);}));_tmp38A[0]=_tmp758;}),({
struct Cyc_PP_Doc*_tmp757=Cyc_Absynpp_lb();_tmp38A[1]=_tmp757;}),({
struct Cyc_PP_Doc*_tmp756=Cyc_PP_line_doc();_tmp38A[2]=_tmp756;}),({
struct Cyc_PP_Doc*_tmp755=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A7,0));_tmp38A[3]=_tmp755;}),({
struct Cyc_PP_Doc*_tmp754=Cyc_PP_line_doc();_tmp38A[4]=_tmp754;}),({
struct Cyc_PP_Doc*_tmp753=Cyc_Absynpp_rb();_tmp38A[5]=_tmp753;}),({
struct Cyc_PP_Doc*_tmp752=Cyc_PP_text(({const char*_tmp38C=" while (";_tag_dyneither(_tmp38C,sizeof(char),9U);}));_tmp38A[6]=_tmp752;}),({
struct Cyc_PP_Doc*_tmp751=Cyc_Absynpp_exp2doc(_tmp3A6);_tmp38A[7]=_tmp751;}),({
struct Cyc_PP_Doc*_tmp750=Cyc_PP_text(({const char*_tmp38D=");";_tag_dyneither(_tmp38D,sizeof(char),3U);}));_tmp38A[8]=_tmp750;});Cyc_PP_cat(_tag_dyneither(_tmp38A,sizeof(struct Cyc_PP_Doc*),9U));});
goto _LL0;default: _LL21: _tmp3A9=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34F)->f1;_tmp3A8=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34F)->f2;_LL22:
# 1583
 s=({struct Cyc_PP_Doc*_tmp38E[12U];({struct Cyc_PP_Doc*_tmp764=Cyc_PP_text(({const char*_tmp38F="try ";_tag_dyneither(_tmp38F,sizeof(char),5U);}));_tmp38E[0]=_tmp764;}),({
struct Cyc_PP_Doc*_tmp763=Cyc_Absynpp_lb();_tmp38E[1]=_tmp763;}),({
struct Cyc_PP_Doc*_tmp762=Cyc_PP_line_doc();_tmp38E[2]=_tmp762;}),({
struct Cyc_PP_Doc*_tmp761=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A9,0));_tmp38E[3]=_tmp761;}),({
struct Cyc_PP_Doc*_tmp760=Cyc_PP_line_doc();_tmp38E[4]=_tmp760;}),({
struct Cyc_PP_Doc*_tmp75F=Cyc_Absynpp_rb();_tmp38E[5]=_tmp75F;}),({
struct Cyc_PP_Doc*_tmp75E=Cyc_PP_text(({const char*_tmp390=" catch ";_tag_dyneither(_tmp390,sizeof(char),8U);}));_tmp38E[6]=_tmp75E;}),({
struct Cyc_PP_Doc*_tmp75D=Cyc_Absynpp_lb();_tmp38E[7]=_tmp75D;}),({
struct Cyc_PP_Doc*_tmp75C=Cyc_PP_line_doc();_tmp38E[8]=_tmp75C;}),({
struct Cyc_PP_Doc*_tmp75B=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3A8));_tmp38E[9]=_tmp75B;}),({
struct Cyc_PP_Doc*_tmp75A=Cyc_PP_line_doc();_tmp38E[10]=_tmp75A;}),({
struct Cyc_PP_Doc*_tmp759=Cyc_Absynpp_rb();_tmp38E[11]=_tmp759;});Cyc_PP_cat(_tag_dyneither(_tmp38E,sizeof(struct Cyc_PP_Doc*),12U));});
goto _LL0;}_LL0:;}
# 1597
return s;}
# 1600
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp3AA=p->r;void*_tmp3AB=_tmp3AA;struct Cyc_Absyn_Exp*_tmp40B;struct Cyc_Absyn_Datatypefield*_tmp40A;struct Cyc_List_List*_tmp409;int _tmp408;struct Cyc_Absyn_Datatypefield*_tmp407;struct Cyc_Absyn_Enumfield*_tmp406;struct Cyc_Absyn_Enumfield*_tmp405;struct Cyc_List_List*_tmp404;struct Cyc_List_List*_tmp403;int _tmp402;union Cyc_Absyn_AggrInfo _tmp401;struct Cyc_List_List*_tmp400;struct Cyc_List_List*_tmp3FF;int _tmp3FE;struct _tuple0*_tmp3FD;struct Cyc_List_List*_tmp3FC;int _tmp3FB;struct _tuple0*_tmp3FA;struct Cyc_Absyn_Vardecl*_tmp3F9;struct Cyc_Absyn_Pat*_tmp3F8;struct Cyc_Absyn_Vardecl*_tmp3F7;struct Cyc_Absyn_Pat*_tmp3F6;struct Cyc_List_List*_tmp3F5;int _tmp3F4;struct Cyc_Absyn_Tvar*_tmp3F3;struct Cyc_Absyn_Vardecl*_tmp3F2;struct Cyc_Absyn_Tvar*_tmp3F1;struct Cyc_Absyn_Vardecl*_tmp3F0;struct Cyc_Absyn_Vardecl*_tmp3EF;struct Cyc_Absyn_Pat*_tmp3EE;struct Cyc_Absyn_Vardecl*_tmp3ED;struct _dyneither_ptr _tmp3EC;char _tmp3EB;enum Cyc_Absyn_Sign _tmp3EA;int _tmp3E9;switch(*((int*)_tmp3AB)){case 0U: _LL1: _LL2:
 s=Cyc_PP_text(({const char*_tmp3AC="_";_tag_dyneither(_tmp3AC,sizeof(char),2U);}));goto _LL0;case 9U: _LL3: _LL4:
 s=Cyc_PP_text(({const char*_tmp3AD="NULL";_tag_dyneither(_tmp3AD,sizeof(char),5U);}));goto _LL0;case 10U: _LL5: _tmp3EA=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3E9=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL6:
# 1606
 if((int)_tmp3EA != (int)1U)
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B0=({struct Cyc_Int_pa_PrintArg_struct _tmp54B;_tmp54B.tag=1U,_tmp54B.f1=(unsigned long)_tmp3E9;_tmp54B;});void*_tmp3AE[1U];_tmp3AE[0]=& _tmp3B0;({struct _dyneither_ptr _tmp765=({const char*_tmp3AF="%d";_tag_dyneither(_tmp3AF,sizeof(char),3U);});Cyc_aprintf(_tmp765,_tag_dyneither(_tmp3AE,sizeof(void*),1U));});}));else{
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3B3=({struct Cyc_Int_pa_PrintArg_struct _tmp54C;_tmp54C.tag=1U,_tmp54C.f1=(unsigned int)_tmp3E9;_tmp54C;});void*_tmp3B1[1U];_tmp3B1[0]=& _tmp3B3;({struct _dyneither_ptr _tmp766=({const char*_tmp3B2="%u";_tag_dyneither(_tmp3B2,sizeof(char),3U);});Cyc_aprintf(_tmp766,_tag_dyneither(_tmp3B1,sizeof(void*),1U));});}));}
goto _LL0;case 11U: _LL7: _tmp3EB=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL8:
 s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B6=({struct Cyc_String_pa_PrintArg_struct _tmp54D;_tmp54D.tag=0U,({struct _dyneither_ptr _tmp767=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp3EB));_tmp54D.f1=_tmp767;});_tmp54D;});void*_tmp3B4[1U];_tmp3B4[0]=& _tmp3B6;({struct _dyneither_ptr _tmp768=({const char*_tmp3B5="'%s'";_tag_dyneither(_tmp3B5,sizeof(char),5U);});Cyc_aprintf(_tmp768,_tag_dyneither(_tmp3B4,sizeof(void*),1U));});}));goto _LL0;case 12U: _LL9: _tmp3EC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LLA:
 s=Cyc_PP_text(_tmp3EC);goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2)->r)->tag == 0U){_LLB: _tmp3ED=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LLC:
# 1613
 s=Cyc_Absynpp_qvar2doc(_tmp3ED->name);goto _LL0;}else{_LLD: _tmp3EF=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3EE=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LLE:
# 1615
 s=({struct Cyc_PP_Doc*_tmp3B7[3U];({struct Cyc_PP_Doc*_tmp76B=Cyc_Absynpp_qvar2doc(_tmp3EF->name);_tmp3B7[0]=_tmp76B;}),({struct Cyc_PP_Doc*_tmp76A=Cyc_PP_text(({const char*_tmp3B8=" as ";_tag_dyneither(_tmp3B8,sizeof(char),5U);}));_tmp3B7[1]=_tmp76A;}),({struct Cyc_PP_Doc*_tmp769=Cyc_Absynpp_pat2doc(_tmp3EE);_tmp3B7[2]=_tmp769;});Cyc_PP_cat(_tag_dyneither(_tmp3B7,sizeof(struct Cyc_PP_Doc*),3U));});goto _LL0;}case 2U: _LLF: _tmp3F1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3F0=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL10:
# 1617
 s=({struct Cyc_PP_Doc*_tmp3B9[5U];({struct Cyc_PP_Doc*_tmp770=Cyc_PP_text(({const char*_tmp3BA="alias";_tag_dyneither(_tmp3BA,sizeof(char),6U);}));_tmp3B9[0]=_tmp770;}),({struct Cyc_PP_Doc*_tmp76F=Cyc_PP_text(({const char*_tmp3BB=" <";_tag_dyneither(_tmp3BB,sizeof(char),3U);}));_tmp3B9[1]=_tmp76F;}),({struct Cyc_PP_Doc*_tmp76E=Cyc_Absynpp_tvar2doc(_tmp3F1);_tmp3B9[2]=_tmp76E;}),({struct Cyc_PP_Doc*_tmp76D=Cyc_PP_text(({const char*_tmp3BC="> ";_tag_dyneither(_tmp3BC,sizeof(char),3U);}));_tmp3B9[3]=_tmp76D;}),({
struct Cyc_PP_Doc*_tmp76C=Cyc_Absynpp_vardecl2doc(_tmp3F0);_tmp3B9[4]=_tmp76C;});Cyc_PP_cat(_tag_dyneither(_tmp3B9,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 4U: _LL11: _tmp3F3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3F2=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL12:
# 1621
 s=({struct Cyc_PP_Doc*_tmp3BD[4U];({struct Cyc_PP_Doc*_tmp774=Cyc_Absynpp_qvar2doc(_tmp3F2->name);_tmp3BD[0]=_tmp774;}),({struct Cyc_PP_Doc*_tmp773=Cyc_PP_text(({const char*_tmp3BE="<";_tag_dyneither(_tmp3BE,sizeof(char),2U);}));_tmp3BD[1]=_tmp773;}),({struct Cyc_PP_Doc*_tmp772=Cyc_Absynpp_tvar2doc(_tmp3F3);_tmp3BD[2]=_tmp772;}),({struct Cyc_PP_Doc*_tmp771=Cyc_PP_text(({const char*_tmp3BF=">";_tag_dyneither(_tmp3BF,sizeof(char),2U);}));_tmp3BD[3]=_tmp771;});Cyc_PP_cat(_tag_dyneither(_tmp3BD,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 5U: _LL13: _tmp3F5=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3F4=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL14:
# 1624
 s=({struct Cyc_PP_Doc*_tmp3C0[4U];({struct Cyc_PP_Doc*_tmp77A=Cyc_Absynpp_dollar();_tmp3C0[0]=_tmp77A;}),({struct Cyc_PP_Doc*_tmp779=Cyc_PP_text(({const char*_tmp3C1="(";_tag_dyneither(_tmp3C1,sizeof(char),2U);}));_tmp3C0[1]=_tmp779;}),({struct Cyc_PP_Doc*_tmp778=({struct _dyneither_ptr _tmp777=({const char*_tmp3C2=",";_tag_dyneither(_tmp3C2,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tmp777,_tmp3F5);});_tmp3C0[2]=_tmp778;}),
_tmp3F4?({struct Cyc_PP_Doc*_tmp776=Cyc_PP_text(({const char*_tmp3C3=", ...)";_tag_dyneither(_tmp3C3,sizeof(char),7U);}));_tmp3C0[3]=_tmp776;}):({struct Cyc_PP_Doc*_tmp775=Cyc_PP_text(({const char*_tmp3C4=")";_tag_dyneither(_tmp3C4,sizeof(char),2U);}));_tmp3C0[3]=_tmp775;});Cyc_PP_cat(_tag_dyneither(_tmp3C0,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;case 6U: _LL15: _tmp3F6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL16:
# 1628
 s=({struct Cyc_PP_Doc*_tmp3C5[2U];({struct Cyc_PP_Doc*_tmp77C=Cyc_PP_text(({const char*_tmp3C6="&";_tag_dyneither(_tmp3C6,sizeof(char),2U);}));_tmp3C5[0]=_tmp77C;}),({struct Cyc_PP_Doc*_tmp77B=Cyc_Absynpp_pat2doc(_tmp3F6);_tmp3C5[1]=_tmp77B;});Cyc_PP_cat(_tag_dyneither(_tmp3C5,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2)->r)->tag == 0U){_LL17: _tmp3F7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL18:
# 1631
 s=({struct Cyc_PP_Doc*_tmp3C7[2U];({struct Cyc_PP_Doc*_tmp77E=Cyc_PP_text(({const char*_tmp3C8="*";_tag_dyneither(_tmp3C8,sizeof(char),2U);}));_tmp3C7[0]=_tmp77E;}),({struct Cyc_PP_Doc*_tmp77D=Cyc_Absynpp_qvar2doc(_tmp3F7->name);_tmp3C7[1]=_tmp77D;});Cyc_PP_cat(_tag_dyneither(_tmp3C7,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}else{_LL19: _tmp3F9=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3F8=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL1A:
# 1634
 s=({struct Cyc_PP_Doc*_tmp3C9[4U];({struct Cyc_PP_Doc*_tmp782=Cyc_PP_text(({const char*_tmp3CA="*";_tag_dyneither(_tmp3CA,sizeof(char),2U);}));_tmp3C9[0]=_tmp782;}),({struct Cyc_PP_Doc*_tmp781=Cyc_Absynpp_qvar2doc(_tmp3F9->name);_tmp3C9[1]=_tmp781;}),({struct Cyc_PP_Doc*_tmp780=Cyc_PP_text(({const char*_tmp3CB=" as ";_tag_dyneither(_tmp3CB,sizeof(char),5U);}));_tmp3C9[2]=_tmp780;}),({struct Cyc_PP_Doc*_tmp77F=Cyc_Absynpp_pat2doc(_tmp3F8);_tmp3C9[3]=_tmp77F;});Cyc_PP_cat(_tag_dyneither(_tmp3C9,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}case 15U: _LL1B: _tmp3FA=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL1C:
# 1637
 s=Cyc_Absynpp_qvar2doc(_tmp3FA);
goto _LL0;case 16U: _LL1D: _tmp3FD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp3FC=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp3FB=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_LL1E: {
# 1640
struct _dyneither_ptr term=_tmp3FB?({const char*_tmp3CF=", ...)";_tag_dyneither(_tmp3CF,sizeof(char),7U);}):({const char*_tmp3D0=")";_tag_dyneither(_tmp3D0,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3CC[2U];({struct Cyc_PP_Doc*_tmp787=Cyc_Absynpp_qvar2doc(_tmp3FD);_tmp3CC[0]=_tmp787;}),({struct Cyc_PP_Doc*_tmp786=({struct _dyneither_ptr _tmp785=({const char*_tmp3CD="(";_tag_dyneither(_tmp3CD,sizeof(char),2U);});struct _dyneither_ptr _tmp784=term;struct _dyneither_ptr _tmp783=({const char*_tmp3CE=",";_tag_dyneither(_tmp3CE,sizeof(char),2U);});Cyc_PP_group(_tmp785,_tmp784,_tmp783,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp3FC));});_tmp3CC[1]=_tmp786;});Cyc_PP_cat(_tag_dyneither(_tmp3CC,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1 != 0){_LL1F: _tmp401=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_tmp400=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp3FF=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_tmp3FE=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f4;_LL20: {
# 1644
struct _dyneither_ptr term=_tmp3FE?({const char*_tmp3DA=", ...}";_tag_dyneither(_tmp3DA,sizeof(char),7U);}):({const char*_tmp3DB="}";_tag_dyneither(_tmp3DB,sizeof(char),2U);});
struct _tuple10 _tmp3D1=Cyc_Absyn_aggr_kinded_name(_tmp401);struct _tuple10 _tmp3D2=_tmp3D1;struct _tuple0*_tmp3D9;_LL2E: _tmp3D9=_tmp3D2.f2;_LL2F:;
s=({struct Cyc_PP_Doc*_tmp3D3[4U];({struct Cyc_PP_Doc*_tmp791=Cyc_Absynpp_qvar2doc(_tmp3D9);_tmp3D3[0]=_tmp791;}),({struct Cyc_PP_Doc*_tmp790=Cyc_Absynpp_lb();_tmp3D3[1]=_tmp790;}),({
struct Cyc_PP_Doc*_tmp78F=({struct _dyneither_ptr _tmp78E=({const char*_tmp3D4="[";_tag_dyneither(_tmp3D4,sizeof(char),2U);});struct _dyneither_ptr _tmp78D=({const char*_tmp3D5="]";_tag_dyneither(_tmp3D5,sizeof(char),2U);});struct _dyneither_ptr _tmp78C=({const char*_tmp3D6=",";_tag_dyneither(_tmp3D6,sizeof(char),2U);});Cyc_PP_egroup(_tmp78E,_tmp78D,_tmp78C,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp400));});_tmp3D3[2]=_tmp78F;}),({
struct Cyc_PP_Doc*_tmp78B=({struct _dyneither_ptr _tmp78A=({const char*_tmp3D7="";_tag_dyneither(_tmp3D7,sizeof(char),1U);});struct _dyneither_ptr _tmp789=term;struct _dyneither_ptr _tmp788=({const char*_tmp3D8=",";_tag_dyneither(_tmp3D8,sizeof(char),2U);});Cyc_PP_group(_tmp78A,_tmp789,_tmp788,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp3FF));});_tmp3D3[3]=_tmp78B;});Cyc_PP_cat(_tag_dyneither(_tmp3D3,sizeof(struct Cyc_PP_Doc*),4U));});
goto _LL0;}}else{_LL21: _tmp404=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp403=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_tmp402=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp3AB)->f4;_LL22: {
# 1651
struct _dyneither_ptr term=_tmp402?({const char*_tmp3E2=", ...}";_tag_dyneither(_tmp3E2,sizeof(char),7U);}):({const char*_tmp3E3="}";_tag_dyneither(_tmp3E3,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3DC[3U];({struct Cyc_PP_Doc*_tmp79A=Cyc_Absynpp_lb();_tmp3DC[0]=_tmp79A;}),({
struct Cyc_PP_Doc*_tmp799=({struct _dyneither_ptr _tmp798=({const char*_tmp3DD="[";_tag_dyneither(_tmp3DD,sizeof(char),2U);});struct _dyneither_ptr _tmp797=({const char*_tmp3DE="]";_tag_dyneither(_tmp3DE,sizeof(char),2U);});struct _dyneither_ptr _tmp796=({const char*_tmp3DF=",";_tag_dyneither(_tmp3DF,sizeof(char),2U);});Cyc_PP_egroup(_tmp798,_tmp797,_tmp796,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp404));});_tmp3DC[1]=_tmp799;}),({
struct Cyc_PP_Doc*_tmp795=({struct _dyneither_ptr _tmp794=({const char*_tmp3E0="";_tag_dyneither(_tmp3E0,sizeof(char),1U);});struct _dyneither_ptr _tmp793=term;struct _dyneither_ptr _tmp792=({const char*_tmp3E1=",";_tag_dyneither(_tmp3E1,sizeof(char),2U);});Cyc_PP_group(_tmp794,_tmp793,_tmp792,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp403));});_tmp3DC[2]=_tmp795;});Cyc_PP_cat(_tag_dyneither(_tmp3DC,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;}}case 13U: _LL23: _tmp405=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL24:
 _tmp406=_tmp405;goto _LL26;case 14U: _LL25: _tmp406=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL26:
 s=Cyc_Absynpp_qvar2doc(_tmp406->name);goto _LL0;case 8U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3 == 0){_LL27: _tmp407=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_LL28:
 s=Cyc_Absynpp_qvar2doc(_tmp407->name);goto _LL0;}else{_LL29: _tmp40A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f2;_tmp409=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f3;_tmp408=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp3AB)->f4;_LL2A: {
# 1660
struct _dyneither_ptr term=_tmp408?({const char*_tmp3E7=", ...)";_tag_dyneither(_tmp3E7,sizeof(char),7U);}):({const char*_tmp3E8=")";_tag_dyneither(_tmp3E8,sizeof(char),2U);});
s=({struct Cyc_PP_Doc*_tmp3E4[2U];({struct Cyc_PP_Doc*_tmp79F=Cyc_Absynpp_qvar2doc(_tmp40A->name);_tmp3E4[0]=_tmp79F;}),({struct Cyc_PP_Doc*_tmp79E=({struct _dyneither_ptr _tmp79D=({const char*_tmp3E5="(";_tag_dyneither(_tmp3E5,sizeof(char),2U);});struct _dyneither_ptr _tmp79C=term;struct _dyneither_ptr _tmp79B=({const char*_tmp3E6=",";_tag_dyneither(_tmp3E6,sizeof(char),2U);});Cyc_PP_egroup(_tmp79D,_tmp79C,_tmp79B,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp409));});_tmp3E4[1]=_tmp79E;});Cyc_PP_cat(_tag_dyneither(_tmp3E4,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}}default: _LL2B: _tmp40B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp3AB)->f1;_LL2C:
# 1664
 s=Cyc_Absynpp_exp2doc(_tmp40B);goto _LL0;}_LL0:;}
# 1666
return s;}
# 1669
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
return({struct Cyc_PP_Doc*_tmp40C[2U];({struct Cyc_PP_Doc*_tmp7A4=({struct _dyneither_ptr _tmp7A3=({const char*_tmp40D="";_tag_dyneither(_tmp40D,sizeof(char),1U);});struct _dyneither_ptr _tmp7A2=({const char*_tmp40E="=";_tag_dyneither(_tmp40E,sizeof(char),2U);});struct _dyneither_ptr _tmp7A1=({const char*_tmp40F="=";_tag_dyneither(_tmp40F,sizeof(char),2U);});Cyc_PP_egroup(_tmp7A3,_tmp7A2,_tmp7A1,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));});_tmp40C[0]=_tmp7A4;}),({
struct Cyc_PP_Doc*_tmp7A0=Cyc_Absynpp_pat2doc((*dp).f2);_tmp40C[1]=_tmp7A0;});Cyc_PP_cat(_tag_dyneither(_tmp40C,sizeof(struct Cyc_PP_Doc*),2U));});}
# 1674
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_tmp410[2U];({struct Cyc_PP_Doc*_tmp7A8=Cyc_PP_text(({const char*_tmp411="default: ";_tag_dyneither(_tmp411,sizeof(char),10U);}));_tmp410[0]=_tmp7A8;}),({
struct Cyc_PP_Doc*_tmp7A7=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp412[2U];({struct Cyc_PP_Doc*_tmp7A6=Cyc_PP_line_doc();_tmp412[0]=_tmp7A6;}),({struct Cyc_PP_Doc*_tmp7A5=Cyc_Absynpp_stmt2doc(c->body,0);_tmp412[1]=_tmp7A5;});Cyc_PP_cat(_tag_dyneither(_tmp412,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp410[1]=_tmp7A7;});Cyc_PP_cat(_tag_dyneither(_tmp410,sizeof(struct Cyc_PP_Doc*),2U));});else{
if(c->where_clause == 0)
return({struct Cyc_PP_Doc*_tmp413[4U];({struct Cyc_PP_Doc*_tmp7AE=Cyc_PP_text(({const char*_tmp414="case ";_tag_dyneither(_tmp414,sizeof(char),6U);}));_tmp413[0]=_tmp7AE;}),({
struct Cyc_PP_Doc*_tmp7AD=Cyc_Absynpp_pat2doc(c->pattern);_tmp413[1]=_tmp7AD;}),({
struct Cyc_PP_Doc*_tmp7AC=Cyc_PP_text(({const char*_tmp415=": ";_tag_dyneither(_tmp415,sizeof(char),3U);}));_tmp413[2]=_tmp7AC;}),({
struct Cyc_PP_Doc*_tmp7AB=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp416[2U];({struct Cyc_PP_Doc*_tmp7AA=Cyc_PP_line_doc();_tmp416[0]=_tmp7AA;}),({struct Cyc_PP_Doc*_tmp7A9=Cyc_Absynpp_stmt2doc(c->body,0);_tmp416[1]=_tmp7A9;});Cyc_PP_cat(_tag_dyneither(_tmp416,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp413[3]=_tmp7AB;});Cyc_PP_cat(_tag_dyneither(_tmp413,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1684
return({struct Cyc_PP_Doc*_tmp417[6U];({struct Cyc_PP_Doc*_tmp7B6=Cyc_PP_text(({const char*_tmp418="case ";_tag_dyneither(_tmp418,sizeof(char),6U);}));_tmp417[0]=_tmp7B6;}),({
struct Cyc_PP_Doc*_tmp7B5=Cyc_Absynpp_pat2doc(c->pattern);_tmp417[1]=_tmp7B5;}),({
struct Cyc_PP_Doc*_tmp7B4=Cyc_PP_text(({const char*_tmp419=" && ";_tag_dyneither(_tmp419,sizeof(char),5U);}));_tmp417[2]=_tmp7B4;}),({
struct Cyc_PP_Doc*_tmp7B3=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp417[3]=_tmp7B3;}),({
struct Cyc_PP_Doc*_tmp7B2=Cyc_PP_text(({const char*_tmp41A=": ";_tag_dyneither(_tmp41A,sizeof(char),3U);}));_tmp417[4]=_tmp7B2;}),({
struct Cyc_PP_Doc*_tmp7B1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp41B[2U];({struct Cyc_PP_Doc*_tmp7B0=Cyc_PP_line_doc();_tmp41B[0]=_tmp7B0;}),({struct Cyc_PP_Doc*_tmp7AF=Cyc_Absynpp_stmt2doc(c->body,0);_tmp41B[1]=_tmp7AF;});Cyc_PP_cat(_tag_dyneither(_tmp41B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp417[5]=_tmp7B1;});Cyc_PP_cat(_tag_dyneither(_tmp417,sizeof(struct Cyc_PP_Doc*),6U));});}}}
# 1692
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct _dyneither_ptr _tmp7B7=({const char*_tmp41C="";_tag_dyneither(_tmp41C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,_tmp7B7,cs);});}
# 1696
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1700
return({struct Cyc_PP_Doc*_tmp41D[3U];({struct Cyc_PP_Doc*_tmp7BA=Cyc_Absynpp_qvar2doc(f->name);_tmp41D[0]=_tmp7BA;}),({struct Cyc_PP_Doc*_tmp7B9=Cyc_PP_text(({const char*_tmp41E=" = ";_tag_dyneither(_tmp41E,sizeof(char),4U);}));_tmp41D[1]=_tmp7B9;}),({struct Cyc_PP_Doc*_tmp7B8=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp41D[2]=_tmp7B8;});Cyc_PP_cat(_tag_dyneither(_tmp41D,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 1703
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct _dyneither_ptr _tmp7BB=({const char*_tmp41F=",";_tag_dyneither(_tmp41F,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tmp7BB,fs);});}
# 1707
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1711
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct _dyneither_ptr _tmp7BC=({const char*_tmp420=",";_tag_dyneither(_tmp420,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tmp7BC,vds);});}
# 1715
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_Absyn_Vardecl*_tmp421=vd;enum Cyc_Absyn_Scope _tmp433;struct _tuple0*_tmp432;unsigned int _tmp431;struct Cyc_Absyn_Tqual _tmp430;void*_tmp42F;struct Cyc_Absyn_Exp*_tmp42E;struct Cyc_List_List*_tmp42D;_LL1: _tmp433=_tmp421->sc;_tmp432=_tmp421->name;_tmp431=_tmp421->varloc;_tmp430=_tmp421->tq;_tmp42F=_tmp421->type;_tmp42E=_tmp421->initializer;_tmp42D=_tmp421->attributes;_LL2:;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp432);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp42D);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp422=Cyc_Cyclone_c_compiler;if(_tmp422 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
 beforenamedoc=attsdoc;goto _LL3;}else{_LL6: _LL7:
# 1724
{void*_tmp423=Cyc_Tcutil_compress(_tmp42F);void*_tmp424=_tmp423;struct Cyc_List_List*_tmp425;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp424)->tag == 5U){_LL9: _tmp425=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp424)->f1).attributes;_LLA:
# 1726
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp425);
goto _LL8;}else{_LLB: _LLC:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1730
goto _LL3;}_LL3:;}{
# 1733
struct Cyc_PP_Doc*tmp_doc;
{enum Cyc_Cyclone_C_Compilers _tmp426=Cyc_Cyclone_c_compiler;if(_tmp426 == Cyc_Cyclone_Gcc_c){_LLE: _LLF:
 tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{_LL10: _LL11:
 tmp_doc=attsdoc;goto _LLD;}_LLD:;}
# 1738
s=({struct Cyc_PP_Doc*_tmp427[5U];_tmp427[0]=tmp_doc,({
# 1740
struct Cyc_PP_Doc*_tmp7C6=Cyc_Absynpp_scope2doc(_tmp433);_tmp427[1]=_tmp7C6;}),({
struct Cyc_PP_Doc*_tmp7C5=({struct Cyc_Absyn_Tqual _tmp7C4=_tmp430;void*_tmp7C3=_tmp42F;Cyc_Absynpp_tqtd2doc(_tmp7C4,_tmp7C3,({struct Cyc_Core_Opt*_tmp429=_cycalloc(sizeof(*_tmp429));({struct Cyc_PP_Doc*_tmp7C2=({struct Cyc_PP_Doc*_tmp428[2U];_tmp428[0]=beforenamedoc,_tmp428[1]=sn;Cyc_PP_cat(_tag_dyneither(_tmp428,sizeof(struct Cyc_PP_Doc*),2U));});_tmp429->v=_tmp7C2;});_tmp429;}));});_tmp427[2]=_tmp7C5;}),
_tmp42E == 0?({
struct Cyc_PP_Doc*_tmp7C1=Cyc_PP_nil_doc();_tmp427[3]=_tmp7C1;}):({
struct Cyc_PP_Doc*_tmp7C0=({struct Cyc_PP_Doc*_tmp42A[2U];({struct Cyc_PP_Doc*_tmp7BF=Cyc_PP_text(({const char*_tmp42B=" = ";_tag_dyneither(_tmp42B,sizeof(char),4U);}));_tmp42A[0]=_tmp7BF;}),({struct Cyc_PP_Doc*_tmp7BE=Cyc_Absynpp_exp2doc(_tmp42E);_tmp42A[1]=_tmp7BE;});Cyc_PP_cat(_tag_dyneither(_tmp42A,sizeof(struct Cyc_PP_Doc*),2U));});_tmp427[3]=_tmp7C0;}),({
struct Cyc_PP_Doc*_tmp7BD=Cyc_PP_text(({const char*_tmp42C=";";_tag_dyneither(_tmp42C,sizeof(char),2U);}));_tmp427[4]=_tmp7BD;});Cyc_PP_cat(_tag_dyneither(_tmp427,sizeof(struct Cyc_PP_Doc*),5U));});
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1749
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp434=*x;struct _tuple17 _tmp435=_tmp434;struct _tuple0*_tmp436;_LL1: _tmp436=_tmp435.f2;_LL2:;
return Cyc_Absynpp_qvar2doc(_tmp436);}
# 1754
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0)
return({struct Cyc_PP_Doc*_tmp437[4U];({struct Cyc_PP_Doc*_tmp7CA=Cyc_Absynpp_scope2doc(ad->sc);_tmp437[0]=_tmp7CA;}),({
struct Cyc_PP_Doc*_tmp7C9=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp437[1]=_tmp7C9;}),({
struct Cyc_PP_Doc*_tmp7C8=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp437[2]=_tmp7C8;}),({
struct Cyc_PP_Doc*_tmp7C7=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp437[3]=_tmp7C7;});Cyc_PP_cat(_tag_dyneither(_tmp437,sizeof(struct Cyc_PP_Doc*),4U));});else{
# 1761
return({struct Cyc_PP_Doc*_tmp438[12U];({struct Cyc_PP_Doc*_tmp7DB=Cyc_Absynpp_scope2doc(ad->sc);_tmp438[0]=_tmp7DB;}),({
struct Cyc_PP_Doc*_tmp7DA=Cyc_Absynpp_aggr_kind2doc(ad->kind);_tmp438[1]=_tmp7DA;}),({
struct Cyc_PP_Doc*_tmp7D9=Cyc_Absynpp_qvar2bolddoc(ad->name);_tmp438[2]=_tmp7D9;}),({
struct Cyc_PP_Doc*_tmp7D8=Cyc_Absynpp_ktvars2doc(ad->tvs);_tmp438[3]=_tmp7D8;}),({
struct Cyc_PP_Doc*_tmp7D7=Cyc_PP_blank_doc();_tmp438[4]=_tmp7D7;}),({struct Cyc_PP_Doc*_tmp7D6=Cyc_Absynpp_lb();_tmp438[5]=_tmp7D6;}),({
struct Cyc_PP_Doc*_tmp7D5=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);_tmp438[6]=_tmp7D5;}),
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?({struct Cyc_PP_Doc*_tmp7D4=Cyc_PP_nil_doc();_tmp438[7]=_tmp7D4;}):({
struct Cyc_PP_Doc*_tmp7D3=({struct Cyc_PP_Doc*_tmp439[2U];({struct Cyc_PP_Doc*_tmp7D2=Cyc_PP_text(({const char*_tmp43A=":";_tag_dyneither(_tmp43A,sizeof(char),2U);}));_tmp439[0]=_tmp7D2;}),({struct Cyc_PP_Doc*_tmp7D1=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);_tmp439[1]=_tmp7D1;});Cyc_PP_cat(_tag_dyneither(_tmp439,sizeof(struct Cyc_PP_Doc*),2U));});_tmp438[7]=_tmp7D3;}),({
struct Cyc_PP_Doc*_tmp7D0=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp43B[2U];({struct Cyc_PP_Doc*_tmp7CF=Cyc_PP_line_doc();_tmp43B[0]=_tmp7CF;}),({struct Cyc_PP_Doc*_tmp7CE=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);_tmp43B[1]=_tmp7CE;});Cyc_PP_cat(_tag_dyneither(_tmp43B,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp438[8]=_tmp7D0;}),({
struct Cyc_PP_Doc*_tmp7CD=Cyc_PP_line_doc();_tmp438[9]=_tmp7CD;}),({
struct Cyc_PP_Doc*_tmp7CC=Cyc_Absynpp_rb();_tmp438[10]=_tmp7CC;}),({
struct Cyc_PP_Doc*_tmp7CB=Cyc_Absynpp_atts2doc(ad->attributes);_tmp438[11]=_tmp7CB;});Cyc_PP_cat(_tag_dyneither(_tmp438,sizeof(struct Cyc_PP_Doc*),12U));});}}
# 1775
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp43C=dd;enum Cyc_Absyn_Scope _tmp448;struct _tuple0*_tmp447;struct Cyc_List_List*_tmp446;struct Cyc_Core_Opt*_tmp445;int _tmp444;_LL1: _tmp448=_tmp43C->sc;_tmp447=_tmp43C->name;_tmp446=_tmp43C->tvs;_tmp445=_tmp43C->fields;_tmp444=_tmp43C->is_extensible;_LL2:;
if(_tmp445 == 0)
return({struct Cyc_PP_Doc*_tmp43D[5U];({struct Cyc_PP_Doc*_tmp7E2=Cyc_Absynpp_scope2doc(_tmp448);_tmp43D[0]=_tmp7E2;}),
_tmp444?({struct Cyc_PP_Doc*_tmp7E1=Cyc_PP_text(({const char*_tmp43E="@extensible ";_tag_dyneither(_tmp43E,sizeof(char),13U);}));_tmp43D[1]=_tmp7E1;}):({struct Cyc_PP_Doc*_tmp7E0=Cyc_PP_blank_doc();_tmp43D[1]=_tmp7E0;}),({
struct Cyc_PP_Doc*_tmp7DF=Cyc_PP_text(({const char*_tmp43F="datatype ";_tag_dyneither(_tmp43F,sizeof(char),10U);}));_tmp43D[2]=_tmp7DF;}),
_tmp444?({struct Cyc_PP_Doc*_tmp7DE=Cyc_Absynpp_qvar2bolddoc(_tmp447);_tmp43D[3]=_tmp7DE;}):({struct Cyc_PP_Doc*_tmp7DD=Cyc_Absynpp_typedef_name2bolddoc(_tmp447);_tmp43D[3]=_tmp7DD;}),({
struct Cyc_PP_Doc*_tmp7DC=Cyc_Absynpp_ktvars2doc(_tmp446);_tmp43D[4]=_tmp7DC;});Cyc_PP_cat(_tag_dyneither(_tmp43D,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 1784
return({struct Cyc_PP_Doc*_tmp440[10U];({struct Cyc_PP_Doc*_tmp7F0=Cyc_Absynpp_scope2doc(_tmp448);_tmp440[0]=_tmp7F0;}),
_tmp444?({struct Cyc_PP_Doc*_tmp7EF=Cyc_PP_text(({const char*_tmp441="@extensible ";_tag_dyneither(_tmp441,sizeof(char),13U);}));_tmp440[1]=_tmp7EF;}):({struct Cyc_PP_Doc*_tmp7EE=Cyc_PP_blank_doc();_tmp440[1]=_tmp7EE;}),({
struct Cyc_PP_Doc*_tmp7ED=Cyc_PP_text(({const char*_tmp442="datatype ";_tag_dyneither(_tmp442,sizeof(char),10U);}));_tmp440[2]=_tmp7ED;}),
_tmp444?({struct Cyc_PP_Doc*_tmp7EC=Cyc_Absynpp_qvar2bolddoc(_tmp447);_tmp440[3]=_tmp7EC;}):({struct Cyc_PP_Doc*_tmp7EB=Cyc_Absynpp_typedef_name2bolddoc(_tmp447);_tmp440[3]=_tmp7EB;}),({
struct Cyc_PP_Doc*_tmp7EA=Cyc_Absynpp_ktvars2doc(_tmp446);_tmp440[4]=_tmp7EA;}),({
struct Cyc_PP_Doc*_tmp7E9=Cyc_PP_blank_doc();_tmp440[5]=_tmp7E9;}),({struct Cyc_PP_Doc*_tmp7E8=Cyc_Absynpp_lb();_tmp440[6]=_tmp7E8;}),({
struct Cyc_PP_Doc*_tmp7E7=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp443[2U];({struct Cyc_PP_Doc*_tmp7E6=Cyc_PP_line_doc();_tmp443[0]=_tmp7E6;}),({struct Cyc_PP_Doc*_tmp7E5=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp445->v);_tmp443[1]=_tmp7E5;});Cyc_PP_cat(_tag_dyneither(_tmp443,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp440[7]=_tmp7E7;}),({
struct Cyc_PP_Doc*_tmp7E4=Cyc_PP_line_doc();_tmp440[8]=_tmp7E4;}),({
struct Cyc_PP_Doc*_tmp7E3=Cyc_Absynpp_rb();_tmp440[9]=_tmp7E3;});Cyc_PP_cat(_tag_dyneither(_tmp440,sizeof(struct Cyc_PP_Doc*),10U));});}}
# 1795
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp449=ed;enum Cyc_Absyn_Scope _tmp451;struct _tuple0*_tmp450;struct Cyc_Core_Opt*_tmp44F;_LL1: _tmp451=_tmp449->sc;_tmp450=_tmp449->name;_tmp44F=_tmp449->fields;_LL2:;
if(_tmp44F == 0)
return({struct Cyc_PP_Doc*_tmp44A[3U];({struct Cyc_PP_Doc*_tmp7F3=Cyc_Absynpp_scope2doc(_tmp451);_tmp44A[0]=_tmp7F3;}),({
struct Cyc_PP_Doc*_tmp7F2=Cyc_PP_text(({const char*_tmp44B="enum ";_tag_dyneither(_tmp44B,sizeof(char),6U);}));_tmp44A[1]=_tmp7F2;}),({
struct Cyc_PP_Doc*_tmp7F1=Cyc_Absynpp_typedef_name2bolddoc(_tmp450);_tmp44A[2]=_tmp7F1;});Cyc_PP_cat(_tag_dyneither(_tmp44A,sizeof(struct Cyc_PP_Doc*),3U));});else{
# 1803
return({struct Cyc_PP_Doc*_tmp44C[8U];({struct Cyc_PP_Doc*_tmp7FD=Cyc_Absynpp_scope2doc(_tmp451);_tmp44C[0]=_tmp7FD;}),({
struct Cyc_PP_Doc*_tmp7FC=Cyc_PP_text(({const char*_tmp44D="enum ";_tag_dyneither(_tmp44D,sizeof(char),6U);}));_tmp44C[1]=_tmp7FC;}),({
struct Cyc_PP_Doc*_tmp7FB=Cyc_Absynpp_qvar2bolddoc(_tmp450);_tmp44C[2]=_tmp7FB;}),({
struct Cyc_PP_Doc*_tmp7FA=Cyc_PP_blank_doc();_tmp44C[3]=_tmp7FA;}),({struct Cyc_PP_Doc*_tmp7F9=Cyc_Absynpp_lb();_tmp44C[4]=_tmp7F9;}),({
struct Cyc_PP_Doc*_tmp7F8=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp44E[2U];({struct Cyc_PP_Doc*_tmp7F7=Cyc_PP_line_doc();_tmp44E[0]=_tmp7F7;}),({struct Cyc_PP_Doc*_tmp7F6=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp44F->v);_tmp44E[1]=_tmp7F6;});Cyc_PP_cat(_tag_dyneither(_tmp44E,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp44C[5]=_tmp7F8;}),({
struct Cyc_PP_Doc*_tmp7F5=Cyc_PP_line_doc();_tmp44C[6]=_tmp7F5;}),({
struct Cyc_PP_Doc*_tmp7F4=Cyc_Absynpp_rb();_tmp44C[7]=_tmp7F4;});Cyc_PP_cat(_tag_dyneither(_tmp44C,sizeof(struct Cyc_PP_Doc*),8U));});}}
# 1812
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp452=d->r;void*_tmp453=_tmp452;struct Cyc_List_List*_tmp4BE;struct Cyc_List_List*_tmp4BD;struct Cyc_List_List*_tmp4BC;struct Cyc_List_List*_tmp4BB;struct _tuple0*_tmp4BA;struct Cyc_List_List*_tmp4B9;struct _dyneither_ptr*_tmp4B8;struct Cyc_List_List*_tmp4B7;struct Cyc_Absyn_Typedefdecl*_tmp4B6;struct Cyc_Absyn_Enumdecl*_tmp4B5;struct Cyc_List_List*_tmp4B4;struct Cyc_Absyn_Pat*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Datatypedecl*_tmp4B1;struct Cyc_Absyn_Tvar*_tmp4B0;struct Cyc_Absyn_Vardecl*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Vardecl*_tmp4AD;struct Cyc_Absyn_Aggrdecl*_tmp4AC;struct Cyc_Absyn_Fndecl*_tmp4AB;switch(*((int*)_tmp453)){case 1U: _LL1: _tmp4AB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL2: {
# 1816
struct Cyc_Absyn_FnInfo type_info=_tmp4AB->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->tag=5U,_tmp464->f1=type_info;_tmp464;});
if(_tmp4AB->cached_type != 0){
void*_tmp454=Cyc_Tcutil_compress((void*)_check_null(_tmp4AB->cached_type));void*_tmp455=_tmp454;struct Cyc_Absyn_FnInfo _tmp458;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->tag == 5U){_LL24: _tmp458=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1;_LL25:
# 1822
({struct Cyc_List_List*_tmp7FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((_tmp4AB->i).attributes,_tmp458.attributes);(_tmp4AB->i).attributes=_tmp7FE;});goto _LL23;}else{_LL26: _LL27:
({void*_tmp456=0U;({struct _dyneither_ptr _tmp7FF=({const char*_tmp457="function has non-function type";_tag_dyneither(_tmp457,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp7FF,_tag_dyneither(_tmp456,sizeof(void*),0U));});});}_LL23:;}{
# 1825
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc((_tmp4AB->i).attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4AB->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp459=Cyc_Cyclone_c_compiler;if(_tmp459 == Cyc_Cyclone_Gcc_c){_LL29: _LL2A:
 inlinedoc=Cyc_PP_text(({const char*_tmp45A="inline ";_tag_dyneither(_tmp45A,sizeof(char),8U);}));goto _LL28;}else{_LL2B: _LL2C:
 inlinedoc=Cyc_PP_text(({const char*_tmp45B="__inline ";_tag_dyneither(_tmp45B,sizeof(char),10U);}));goto _LL28;}_LL28:;}else{
# 1833
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4AB->sc);
struct Cyc_PP_Doc*beforenamedoc;
{enum Cyc_Cyclone_C_Compilers _tmp45C=Cyc_Cyclone_c_compiler;if(_tmp45C == Cyc_Cyclone_Gcc_c){_LL2E: _LL2F:
 beforenamedoc=attsdoc;goto _LL2D;}else{_LL30: _LL31:
 beforenamedoc=Cyc_Absynpp_callconv2doc((_tmp4AB->i).attributes);goto _LL2D;}_LL2D:;}{
# 1840
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4AB->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _tmp802=Cyc_Absyn_empty_tqual(0U);void*_tmp801=t;Cyc_Absynpp_tqtd2doc(_tmp802,_tmp801,({struct Cyc_Core_Opt*_tmp463=_cycalloc(sizeof(*_tmp463));({
struct Cyc_PP_Doc*_tmp800=({struct Cyc_PP_Doc*_tmp462[2U];_tmp462[0]=beforenamedoc,_tmp462[1]=namedoc;Cyc_PP_cat(_tag_dyneither(_tmp462,sizeof(struct Cyc_PP_Doc*),2U));});_tmp463->v=_tmp800;});_tmp463;}));});
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_tmp460[5U];({struct Cyc_PP_Doc*_tmp809=Cyc_PP_blank_doc();_tmp460[0]=_tmp809;}),({struct Cyc_PP_Doc*_tmp808=Cyc_Absynpp_lb();_tmp460[1]=_tmp808;}),({
struct Cyc_PP_Doc*_tmp807=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp461[2U];({struct Cyc_PP_Doc*_tmp806=Cyc_PP_line_doc();_tmp461[0]=_tmp806;}),({struct Cyc_PP_Doc*_tmp805=Cyc_Absynpp_stmt2doc(_tmp4AB->body,0);_tmp461[1]=_tmp805;});Cyc_PP_cat(_tag_dyneither(_tmp461,sizeof(struct Cyc_PP_Doc*),2U));}));_tmp460[2]=_tmp807;}),({
struct Cyc_PP_Doc*_tmp804=Cyc_PP_line_doc();_tmp460[3]=_tmp804;}),({
struct Cyc_PP_Doc*_tmp803=Cyc_Absynpp_rb();_tmp460[4]=_tmp803;});Cyc_PP_cat(_tag_dyneither(_tmp460,sizeof(struct Cyc_PP_Doc*),5U));});
s=({struct Cyc_PP_Doc*_tmp45D[4U];_tmp45D[0]=inlinedoc,_tmp45D[1]=scopedoc,_tmp45D[2]=tqtddoc,_tmp45D[3]=bodydoc;Cyc_PP_cat(_tag_dyneither(_tmp45D,sizeof(struct Cyc_PP_Doc*),4U));});
# 1849
{enum Cyc_Cyclone_C_Compilers _tmp45E=Cyc_Cyclone_c_compiler;if(_tmp45E == Cyc_Cyclone_Vc_c){_LL33: _LL34:
 s=({struct Cyc_PP_Doc*_tmp45F[2U];_tmp45F[0]=attsdoc,_tmp45F[1]=s;Cyc_PP_cat(_tag_dyneither(_tmp45F,sizeof(struct Cyc_PP_Doc*),2U));});goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 1854
goto _LL0;};};};};}case 5U: _LL3: _tmp4AC=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL4:
# 1857
 s=({struct Cyc_PP_Doc*_tmp465[2U];({struct Cyc_PP_Doc*_tmp80B=Cyc_Absynpp_aggrdecl2doc(_tmp4AC);_tmp465[0]=_tmp80B;}),({struct Cyc_PP_Doc*_tmp80A=Cyc_PP_text(({const char*_tmp466=";";_tag_dyneither(_tmp466,sizeof(char),2U);}));_tmp465[1]=_tmp80A;});Cyc_PP_cat(_tag_dyneither(_tmp465,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 0U: _LL5: _tmp4AD=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL6:
# 1860
 s=Cyc_Absynpp_vardecl2doc(_tmp4AD);
goto _LL0;case 4U: _LL7: _tmp4B0=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4AF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_tmp4AE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LL8:
# 1863
 s=({struct Cyc_PP_Doc*_tmp467[7U];({struct Cyc_PP_Doc*_tmp816=Cyc_PP_text(({const char*_tmp468="region";_tag_dyneither(_tmp468,sizeof(char),7U);}));_tmp467[0]=_tmp816;}),({
struct Cyc_PP_Doc*_tmp815=Cyc_PP_text(({const char*_tmp469="<";_tag_dyneither(_tmp469,sizeof(char),2U);}));_tmp467[1]=_tmp815;}),({
struct Cyc_PP_Doc*_tmp814=Cyc_Absynpp_tvar2doc(_tmp4B0);_tmp467[2]=_tmp814;}),({
struct Cyc_PP_Doc*_tmp813=Cyc_PP_text(({const char*_tmp46A=">";_tag_dyneither(_tmp46A,sizeof(char),2U);}));_tmp467[3]=_tmp813;}),({
struct Cyc_PP_Doc*_tmp812=Cyc_Absynpp_qvar2doc(_tmp4AF->name);_tmp467[4]=_tmp812;}),
(unsigned int)_tmp4AE?({struct Cyc_PP_Doc*_tmp811=({struct Cyc_PP_Doc*_tmp46B[3U];({struct Cyc_PP_Doc*_tmp810=Cyc_PP_text(({const char*_tmp46C=" = open(";_tag_dyneither(_tmp46C,sizeof(char),9U);}));_tmp46B[0]=_tmp810;}),({struct Cyc_PP_Doc*_tmp80F=Cyc_Absynpp_exp2doc(_tmp4AE);_tmp46B[1]=_tmp80F;}),({
struct Cyc_PP_Doc*_tmp80E=Cyc_PP_text(({const char*_tmp46D=")";_tag_dyneither(_tmp46D,sizeof(char),2U);}));_tmp46B[2]=_tmp80E;});Cyc_PP_cat(_tag_dyneither(_tmp46B,sizeof(struct Cyc_PP_Doc*),3U));});
# 1868
_tmp467[5]=_tmp811;}):({
struct Cyc_PP_Doc*_tmp80D=Cyc_PP_nil_doc();_tmp467[5]=_tmp80D;}),({
struct Cyc_PP_Doc*_tmp80C=Cyc_PP_text(({const char*_tmp46E=";";_tag_dyneither(_tmp46E,sizeof(char),2U);}));_tmp467[6]=_tmp80C;});Cyc_PP_cat(_tag_dyneither(_tmp467,sizeof(struct Cyc_PP_Doc*),7U));});
goto _LL0;case 6U: _LL9: _tmp4B1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LLA:
# 1873
 s=({struct Cyc_PP_Doc*_tmp46F[2U];({struct Cyc_PP_Doc*_tmp818=Cyc_Absynpp_datatypedecl2doc(_tmp4B1);_tmp46F[0]=_tmp818;}),({struct Cyc_PP_Doc*_tmp817=Cyc_PP_text(({const char*_tmp470=";";_tag_dyneither(_tmp470,sizeof(char),2U);}));_tmp46F[1]=_tmp817;});Cyc_PP_cat(_tag_dyneither(_tmp46F,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 2U: _LLB: _tmp4B3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4B2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LLC:
# 1876
 s=({struct Cyc_PP_Doc*_tmp471[5U];({struct Cyc_PP_Doc*_tmp81D=Cyc_PP_text(({const char*_tmp472="let ";_tag_dyneither(_tmp472,sizeof(char),5U);}));_tmp471[0]=_tmp81D;}),({
struct Cyc_PP_Doc*_tmp81C=Cyc_Absynpp_pat2doc(_tmp4B3);_tmp471[1]=_tmp81C;}),({
struct Cyc_PP_Doc*_tmp81B=Cyc_PP_text(({const char*_tmp473=" = ";_tag_dyneither(_tmp473,sizeof(char),4U);}));_tmp471[2]=_tmp81B;}),({
struct Cyc_PP_Doc*_tmp81A=Cyc_Absynpp_exp2doc(_tmp4B2);_tmp471[3]=_tmp81A;}),({
struct Cyc_PP_Doc*_tmp819=Cyc_PP_text(({const char*_tmp474=";";_tag_dyneither(_tmp474,sizeof(char),2U);}));_tmp471[4]=_tmp819;});Cyc_PP_cat(_tag_dyneither(_tmp471,sizeof(struct Cyc_PP_Doc*),5U));});
goto _LL0;case 3U: _LLD: _tmp4B4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LLE:
# 1883
 s=({struct Cyc_PP_Doc*_tmp475[3U];({struct Cyc_PP_Doc*_tmp820=Cyc_PP_text(({const char*_tmp476="let ";_tag_dyneither(_tmp476,sizeof(char),5U);}));_tmp475[0]=_tmp820;}),({struct Cyc_PP_Doc*_tmp81F=Cyc_Absynpp_ids2doc(_tmp4B4);_tmp475[1]=_tmp81F;}),({struct Cyc_PP_Doc*_tmp81E=Cyc_PP_text(({const char*_tmp477=";";_tag_dyneither(_tmp477,sizeof(char),2U);}));_tmp475[2]=_tmp81E;});Cyc_PP_cat(_tag_dyneither(_tmp475,sizeof(struct Cyc_PP_Doc*),3U));});
goto _LL0;case 7U: _LLF: _tmp4B5=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL10:
# 1886
 s=({struct Cyc_PP_Doc*_tmp478[2U];({struct Cyc_PP_Doc*_tmp822=Cyc_Absynpp_enumdecl2doc(_tmp4B5);_tmp478[0]=_tmp822;}),({struct Cyc_PP_Doc*_tmp821=Cyc_PP_text(({const char*_tmp479=";";_tag_dyneither(_tmp479,sizeof(char),2U);}));_tmp478[1]=_tmp821;});Cyc_PP_cat(_tag_dyneither(_tmp478,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 8U: _LL11: _tmp4B6=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL12: {
# 1889
void*t;
if(_tmp4B6->defn != 0)
t=(void*)_check_null(_tmp4B6->defn);else{
# 1893
t=Cyc_Absyn_new_evar(_tmp4B6->kind,0);}
s=({struct Cyc_PP_Doc*_tmp47A[4U];({struct Cyc_PP_Doc*_tmp82B=Cyc_PP_text(({const char*_tmp47B="typedef ";_tag_dyneither(_tmp47B,sizeof(char),9U);}));_tmp47A[0]=_tmp82B;}),({
struct Cyc_PP_Doc*_tmp82A=({struct Cyc_Absyn_Tqual _tmp829=_tmp4B6->tq;void*_tmp828=t;Cyc_Absynpp_tqtd2doc(_tmp829,_tmp828,({struct Cyc_Core_Opt*_tmp47D=_cycalloc(sizeof(*_tmp47D));({
# 1897
struct Cyc_PP_Doc*_tmp827=({struct Cyc_PP_Doc*_tmp47C[2U];({struct Cyc_PP_Doc*_tmp826=Cyc_Absynpp_typedef_name2bolddoc(_tmp4B6->name);_tmp47C[0]=_tmp826;}),({
struct Cyc_PP_Doc*_tmp825=Cyc_Absynpp_tvars2doc(_tmp4B6->tvs);_tmp47C[1]=_tmp825;});Cyc_PP_cat(_tag_dyneither(_tmp47C,sizeof(struct Cyc_PP_Doc*),2U));});
# 1897
_tmp47D->v=_tmp827;});_tmp47D;}));});
# 1895
_tmp47A[1]=_tmp82A;}),({
# 1900
struct Cyc_PP_Doc*_tmp824=Cyc_Absynpp_atts2doc(_tmp4B6->atts);_tmp47A[2]=_tmp824;}),({
struct Cyc_PP_Doc*_tmp823=Cyc_PP_text(({const char*_tmp47E=";";_tag_dyneither(_tmp47E,sizeof(char),2U);}));_tmp47A[3]=_tmp823;});Cyc_PP_cat(_tag_dyneither(_tmp47A,sizeof(struct Cyc_PP_Doc*),4U));});
# 1903
goto _LL0;}case 9U: _LL13: _tmp4B8=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4B7=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL14:
# 1905
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4B8);
s=({struct Cyc_PP_Doc*_tmp47F[8U];({struct Cyc_PP_Doc*_tmp834=Cyc_PP_text(({const char*_tmp480="namespace ";_tag_dyneither(_tmp480,sizeof(char),11U);}));_tmp47F[0]=_tmp834;}),({
struct Cyc_PP_Doc*_tmp833=Cyc_PP_textptr(_tmp4B8);_tmp47F[1]=_tmp833;}),({
struct Cyc_PP_Doc*_tmp832=Cyc_PP_blank_doc();_tmp47F[2]=_tmp832;}),({struct Cyc_PP_Doc*_tmp831=Cyc_Absynpp_lb();_tmp47F[3]=_tmp831;}),({
struct Cyc_PP_Doc*_tmp830=Cyc_PP_line_doc();_tmp47F[4]=_tmp830;}),({
struct Cyc_PP_Doc*_tmp82F=({struct _dyneither_ptr _tmp82E=({const char*_tmp481="";_tag_dyneither(_tmp481,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp82E,_tmp4B7);});_tmp47F[5]=_tmp82F;}),({
struct Cyc_PP_Doc*_tmp82D=Cyc_PP_line_doc();_tmp47F[6]=_tmp82D;}),({
struct Cyc_PP_Doc*_tmp82C=Cyc_Absynpp_rb();_tmp47F[7]=_tmp82C;});Cyc_PP_cat(_tag_dyneither(_tmp47F,sizeof(struct Cyc_PP_Doc*),8U));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;case 10U: _LL15: _tmp4BA=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4B9=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_LL16:
# 1916
 if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_tmp482[8U];({struct Cyc_PP_Doc*_tmp83D=Cyc_PP_text(({const char*_tmp483="using ";_tag_dyneither(_tmp483,sizeof(char),7U);}));_tmp482[0]=_tmp83D;}),({
struct Cyc_PP_Doc*_tmp83C=Cyc_Absynpp_qvar2doc(_tmp4BA);_tmp482[1]=_tmp83C;}),({
struct Cyc_PP_Doc*_tmp83B=Cyc_PP_blank_doc();_tmp482[2]=_tmp83B;}),({struct Cyc_PP_Doc*_tmp83A=Cyc_Absynpp_lb();_tmp482[3]=_tmp83A;}),({
struct Cyc_PP_Doc*_tmp839=Cyc_PP_line_doc();_tmp482[4]=_tmp839;}),({
struct Cyc_PP_Doc*_tmp838=({struct _dyneither_ptr _tmp837=({const char*_tmp484="";_tag_dyneither(_tmp484,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp837,_tmp4B9);});_tmp482[5]=_tmp838;}),({
struct Cyc_PP_Doc*_tmp836=Cyc_PP_line_doc();_tmp482[6]=_tmp836;}),({
struct Cyc_PP_Doc*_tmp835=Cyc_Absynpp_rb();_tmp482[7]=_tmp835;});Cyc_PP_cat(_tag_dyneither(_tmp482,sizeof(struct Cyc_PP_Doc*),8U));});else{
# 1925
s=({struct Cyc_PP_Doc*_tmp485[11U];({struct Cyc_PP_Doc*_tmp849=Cyc_PP_text(({const char*_tmp486="/* using ";_tag_dyneither(_tmp486,sizeof(char),10U);}));_tmp485[0]=_tmp849;}),({
struct Cyc_PP_Doc*_tmp848=Cyc_Absynpp_qvar2doc(_tmp4BA);_tmp485[1]=_tmp848;}),({
struct Cyc_PP_Doc*_tmp847=Cyc_PP_blank_doc();_tmp485[2]=_tmp847;}),({
struct Cyc_PP_Doc*_tmp846=Cyc_Absynpp_lb();_tmp485[3]=_tmp846;}),({
struct Cyc_PP_Doc*_tmp845=Cyc_PP_text(({const char*_tmp487=" */";_tag_dyneither(_tmp487,sizeof(char),4U);}));_tmp485[4]=_tmp845;}),({
struct Cyc_PP_Doc*_tmp844=Cyc_PP_line_doc();_tmp485[5]=_tmp844;}),({
struct Cyc_PP_Doc*_tmp843=({struct _dyneither_ptr _tmp842=({const char*_tmp488="";_tag_dyneither(_tmp488,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp842,_tmp4B9);});_tmp485[6]=_tmp843;}),({
struct Cyc_PP_Doc*_tmp841=Cyc_PP_line_doc();_tmp485[7]=_tmp841;}),({
struct Cyc_PP_Doc*_tmp840=Cyc_PP_text(({const char*_tmp489="/* ";_tag_dyneither(_tmp489,sizeof(char),4U);}));_tmp485[8]=_tmp840;}),({
struct Cyc_PP_Doc*_tmp83F=Cyc_Absynpp_rb();_tmp485[9]=_tmp83F;}),({
struct Cyc_PP_Doc*_tmp83E=Cyc_PP_text(({const char*_tmp48A=" */";_tag_dyneither(_tmp48A,sizeof(char),4U);}));_tmp485[10]=_tmp83E;});Cyc_PP_cat(_tag_dyneither(_tmp485,sizeof(struct Cyc_PP_Doc*),11U));});}
goto _LL0;case 11U: _LL17: _tmp4BB=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_LL18:
# 1938
 if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp48B[6U];({struct Cyc_PP_Doc*_tmp850=Cyc_PP_text(({const char*_tmp48C="extern \"C\" ";_tag_dyneither(_tmp48C,sizeof(char),12U);}));_tmp48B[0]=_tmp850;}),({
struct Cyc_PP_Doc*_tmp84F=Cyc_Absynpp_lb();_tmp48B[1]=_tmp84F;}),({
struct Cyc_PP_Doc*_tmp84E=Cyc_PP_line_doc();_tmp48B[2]=_tmp84E;}),({
struct Cyc_PP_Doc*_tmp84D=({struct _dyneither_ptr _tmp84C=({const char*_tmp48D="";_tag_dyneither(_tmp48D,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp84C,_tmp4BB);});_tmp48B[3]=_tmp84D;}),({
struct Cyc_PP_Doc*_tmp84B=Cyc_PP_line_doc();_tmp48B[4]=_tmp84B;}),({
struct Cyc_PP_Doc*_tmp84A=Cyc_Absynpp_rb();_tmp48B[5]=_tmp84A;});Cyc_PP_cat(_tag_dyneither(_tmp48B,sizeof(struct Cyc_PP_Doc*),6U));});else{
# 1946
s=({struct Cyc_PP_Doc*_tmp48E[9U];({struct Cyc_PP_Doc*_tmp85A=Cyc_PP_text(({const char*_tmp48F="/* extern \"C\" ";_tag_dyneither(_tmp48F,sizeof(char),15U);}));_tmp48E[0]=_tmp85A;}),({
struct Cyc_PP_Doc*_tmp859=Cyc_Absynpp_lb();_tmp48E[1]=_tmp859;}),({
struct Cyc_PP_Doc*_tmp858=Cyc_PP_text(({const char*_tmp490=" */";_tag_dyneither(_tmp490,sizeof(char),4U);}));_tmp48E[2]=_tmp858;}),({
struct Cyc_PP_Doc*_tmp857=Cyc_PP_line_doc();_tmp48E[3]=_tmp857;}),({
struct Cyc_PP_Doc*_tmp856=({struct _dyneither_ptr _tmp855=({const char*_tmp491="";_tag_dyneither(_tmp491,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp855,_tmp4BB);});_tmp48E[4]=_tmp856;}),({
struct Cyc_PP_Doc*_tmp854=Cyc_PP_line_doc();_tmp48E[5]=_tmp854;}),({
struct Cyc_PP_Doc*_tmp853=Cyc_PP_text(({const char*_tmp492="/* ";_tag_dyneither(_tmp492,sizeof(char),4U);}));_tmp48E[6]=_tmp853;}),({
struct Cyc_PP_Doc*_tmp852=Cyc_Absynpp_rb();_tmp48E[7]=_tmp852;}),({
struct Cyc_PP_Doc*_tmp851=Cyc_PP_text(({const char*_tmp493=" */";_tag_dyneither(_tmp493,sizeof(char),4U);}));_tmp48E[8]=_tmp851;});Cyc_PP_cat(_tag_dyneither(_tmp48E,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 12U: _LL19: _tmp4BE=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f1;_tmp4BD=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f2;_tmp4BC=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp453)->f3;_LL1A:
# 1957
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(_tmp4BC != 0)
exs_doc=({struct Cyc_PP_Doc*_tmp494[7U];({struct Cyc_PP_Doc*_tmp862=Cyc_Absynpp_rb();_tmp494[0]=_tmp862;}),({struct Cyc_PP_Doc*_tmp861=Cyc_PP_text(({const char*_tmp495=" export ";_tag_dyneither(_tmp495,sizeof(char),9U);}));_tmp494[1]=_tmp861;}),({struct Cyc_PP_Doc*_tmp860=Cyc_Absynpp_lb();_tmp494[2]=_tmp860;}),({
struct Cyc_PP_Doc*_tmp85F=Cyc_PP_line_doc();_tmp494[3]=_tmp85F;}),({struct Cyc_PP_Doc*_tmp85E=({struct _dyneither_ptr _tmp85D=({const char*_tmp496=",";_tag_dyneither(_tmp496,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,_tmp85D,_tmp4BC);});_tmp494[4]=_tmp85E;}),({
struct Cyc_PP_Doc*_tmp85C=Cyc_PP_line_doc();_tmp494[5]=_tmp85C;}),({struct Cyc_PP_Doc*_tmp85B=Cyc_Absynpp_rb();_tmp494[6]=_tmp85B;});Cyc_PP_cat(_tag_dyneither(_tmp494,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1965
exs_doc=Cyc_Absynpp_rb();}
if(_tmp4BD != 0)
ovrs_doc=({struct Cyc_PP_Doc*_tmp497[7U];({struct Cyc_PP_Doc*_tmp86A=Cyc_Absynpp_rb();_tmp497[0]=_tmp86A;}),({struct Cyc_PP_Doc*_tmp869=Cyc_PP_text(({const char*_tmp498=" cycdef ";_tag_dyneither(_tmp498,sizeof(char),9U);}));_tmp497[1]=_tmp869;}),({struct Cyc_PP_Doc*_tmp868=Cyc_Absynpp_lb();_tmp497[2]=_tmp868;}),({
struct Cyc_PP_Doc*_tmp867=Cyc_PP_line_doc();_tmp497[3]=_tmp867;}),({struct Cyc_PP_Doc*_tmp866=({struct _dyneither_ptr _tmp865=({const char*_tmp499="";_tag_dyneither(_tmp499,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp865,_tmp4BD);});_tmp497[4]=_tmp866;}),({
struct Cyc_PP_Doc*_tmp864=Cyc_PP_line_doc();_tmp497[5]=_tmp864;}),({struct Cyc_PP_Doc*_tmp863=Cyc_Absynpp_rb();_tmp497[6]=_tmp863;});Cyc_PP_cat(_tag_dyneither(_tmp497,sizeof(struct Cyc_PP_Doc*),7U));});else{
# 1971
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_tmp49A[6U];({struct Cyc_PP_Doc*_tmp870=Cyc_PP_text(({const char*_tmp49B="extern \"C include\" ";_tag_dyneither(_tmp49B,sizeof(char),20U);}));_tmp49A[0]=_tmp870;}),({
struct Cyc_PP_Doc*_tmp86F=Cyc_Absynpp_lb();_tmp49A[1]=_tmp86F;}),({
struct Cyc_PP_Doc*_tmp86E=Cyc_PP_line_doc();_tmp49A[2]=_tmp86E;}),({
struct Cyc_PP_Doc*_tmp86D=({struct _dyneither_ptr _tmp86C=({const char*_tmp49C="";_tag_dyneither(_tmp49C,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp86C,_tmp4BE);});_tmp49A[3]=_tmp86D;}),({
struct Cyc_PP_Doc*_tmp86B=Cyc_PP_line_doc();_tmp49A[4]=_tmp86B;}),_tmp49A[5]=exs_doc;Cyc_PP_cat(_tag_dyneither(_tmp49A,sizeof(struct Cyc_PP_Doc*),6U));});}else{
# 1979
s=({struct Cyc_PP_Doc*_tmp49D[9U];({struct Cyc_PP_Doc*_tmp87A=Cyc_PP_text(({const char*_tmp49E="/* extern \"C include\" ";_tag_dyneither(_tmp49E,sizeof(char),23U);}));_tmp49D[0]=_tmp87A;}),({
struct Cyc_PP_Doc*_tmp879=Cyc_Absynpp_lb();_tmp49D[1]=_tmp879;}),({
struct Cyc_PP_Doc*_tmp878=Cyc_PP_text(({const char*_tmp49F=" */";_tag_dyneither(_tmp49F,sizeof(char),4U);}));_tmp49D[2]=_tmp878;}),({
struct Cyc_PP_Doc*_tmp877=Cyc_PP_line_doc();_tmp49D[3]=_tmp877;}),({
struct Cyc_PP_Doc*_tmp876=({struct _dyneither_ptr _tmp875=({const char*_tmp4A0="";_tag_dyneither(_tmp4A0,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tmp875,_tmp4BE);});_tmp49D[4]=_tmp876;}),({
struct Cyc_PP_Doc*_tmp874=Cyc_PP_line_doc();_tmp49D[5]=_tmp874;}),({
struct Cyc_PP_Doc*_tmp873=Cyc_PP_text(({const char*_tmp4A1="/* ";_tag_dyneither(_tmp4A1,sizeof(char),4U);}));_tmp49D[6]=_tmp873;}),({
struct Cyc_PP_Doc*_tmp872=Cyc_Absynpp_rb();_tmp49D[7]=_tmp872;}),({
struct Cyc_PP_Doc*_tmp871=Cyc_PP_text(({const char*_tmp4A2=" */";_tag_dyneither(_tmp4A2,sizeof(char),4U);}));_tmp49D[8]=_tmp871;});Cyc_PP_cat(_tag_dyneither(_tmp49D,sizeof(struct Cyc_PP_Doc*),9U));});}
goto _LL0;case 13U: _LL1B: _LL1C:
# 1990
 s=({struct Cyc_PP_Doc*_tmp4A3[2U];({struct Cyc_PP_Doc*_tmp87C=Cyc_PP_text(({const char*_tmp4A4="__cyclone_port_on__;";_tag_dyneither(_tmp4A4,sizeof(char),21U);}));_tmp4A3[0]=_tmp87C;}),({struct Cyc_PP_Doc*_tmp87B=Cyc_Absynpp_lb();_tmp4A3[1]=_tmp87B;});Cyc_PP_cat(_tag_dyneither(_tmp4A3,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 14U: _LL1D: _LL1E:
# 1993
 s=({struct Cyc_PP_Doc*_tmp4A5[2U];({struct Cyc_PP_Doc*_tmp87E=Cyc_PP_text(({const char*_tmp4A6="__cyclone_port_off__;";_tag_dyneither(_tmp4A6,sizeof(char),22U);}));_tmp4A5[0]=_tmp87E;}),({struct Cyc_PP_Doc*_tmp87D=Cyc_Absynpp_lb();_tmp4A5[1]=_tmp87D;});Cyc_PP_cat(_tag_dyneither(_tmp4A5,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;case 15U: _LL1F: _LL20:
# 1996
 s=({struct Cyc_PP_Doc*_tmp4A7[2U];({struct Cyc_PP_Doc*_tmp880=Cyc_PP_text(({const char*_tmp4A8="__tempest_on__;";_tag_dyneither(_tmp4A8,sizeof(char),16U);}));_tmp4A7[0]=_tmp880;}),({struct Cyc_PP_Doc*_tmp87F=Cyc_Absynpp_lb();_tmp4A7[1]=_tmp87F;});Cyc_PP_cat(_tag_dyneither(_tmp4A7,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;default: _LL21: _LL22:
# 1999
 s=({struct Cyc_PP_Doc*_tmp4A9[2U];({struct Cyc_PP_Doc*_tmp882=Cyc_PP_text(({const char*_tmp4AA="__tempest_off__;";_tag_dyneither(_tmp4AA,sizeof(char),17U);}));_tmp4A9[0]=_tmp882;}),({struct Cyc_PP_Doc*_tmp881=Cyc_Absynpp_lb();_tmp4A9[1]=_tmp881;});Cyc_PP_cat(_tag_dyneither(_tmp4A9,sizeof(struct Cyc_PP_Doc*),2U));});
goto _LL0;}_LL0:;}
# 2003
return s;}
# 2006
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();{
enum Cyc_Absyn_Scope _tmp4BF=sc;switch(_tmp4BF){case Cyc_Absyn_Static: _LL1: _LL2:
 return Cyc_PP_text(({const char*_tmp4C0="static ";_tag_dyneither(_tmp4C0,sizeof(char),8U);}));case Cyc_Absyn_Public: _LL3: _LL4:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL5: _LL6:
 return Cyc_PP_text(({const char*_tmp4C1="extern ";_tag_dyneither(_tmp4C1,sizeof(char),8U);}));case Cyc_Absyn_ExternC: _LL7: _LL8:
 return Cyc_PP_text(({const char*_tmp4C2="extern \"C\" ";_tag_dyneither(_tmp4C2,sizeof(char),12U);}));case Cyc_Absyn_Abstract: _LL9: _LLA:
 return Cyc_PP_text(({const char*_tmp4C3="abstract ";_tag_dyneither(_tmp4C3,sizeof(char),10U);}));case Cyc_Absyn_Register: _LLB: _LLC:
 return Cyc_PP_text(({const char*_tmp4C4="register ";_tag_dyneither(_tmp4C4,sizeof(char),10U);}));default: _LLD: _LLE:
 return Cyc_PP_nil_doc();}_LL0:;};}
# 2020
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp4C5=Cyc_Tcutil_compress(t);void*_tmp4C6=_tmp4C5;struct Cyc_List_List*_tmp4C8;struct Cyc_Absyn_Tvar*_tmp4C7;switch(*((int*)_tmp4C6)){case 2U: _LL1: _tmp4C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C6)->f1;_LL2:
 return Cyc_Tcutil_is_temp_tvar(_tmp4C7);case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C6)->f1)->tag == 9U){_LL3: _tmp4C8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C6)->f2;_LL4:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp4C8);}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2032
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp4C9=t;struct Cyc_Absyn_Typedefdecl*_tmp4CB;void*_tmp4CA;switch(*((int*)_tmp4C9)){case 7U: _LL1: _LL2:
 return 1;case 0U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C9)->f1)->tag == 16U){_LL3: _LL4:
 return 1;}else{goto _LL7;}case 8U: _LL5: _tmp4CB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f3;_tmp4CA=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f4;if(_tmp4CA != 0){_LL6:
# 2039
 return Cyc_Absynpp_is_anon_aggrtype(_tmp4CA);}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2046
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2049
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmp4CC=({struct _tuple16 _tmp54E;_tmp54E.f1=(void*)tms->hd,_tmp54E.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp54E;});struct _tuple16 _tmp4CD=_tmp4CC;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp4CD.f1)->tag == 2U){if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp4CD.f2)->tag == 3U){_LL1: _LL2:
# 2052
 return({struct Cyc_List_List*_tmp4CF=_region_malloc(r,sizeof(*_tmp4CF));_tmp4CF->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp884=({struct Cyc_List_List*_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE));_tmp4CE->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,({struct Cyc_List_List*_tmp883=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl);_tmp4CE->tl=_tmp883;});_tmp4CE;});_tmp4CF->tl=_tmp884;});_tmp4CF;});}else{goto _LL3;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0));_tmp4D0->hd=atts,_tmp4D0->tl=tms;_tmp4D0;});}_LL0:;}else{
# 2055
return({struct Cyc_List_List*_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1));_tmp4D1->hd=atts,_tmp4D1->tl=tms;_tmp4D1;});}}
# 2060
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2062
void*_tmp4D2=t;struct _tuple0*_tmp50F;struct Cyc_List_List*_tmp50E;struct Cyc_Absyn_Typedefdecl*_tmp50D;void*_tmp50C;struct Cyc_Core_Opt*_tmp50B;void*_tmp50A;int _tmp509;struct Cyc_List_List*_tmp508;void*_tmp507;struct Cyc_Absyn_Tqual _tmp506;void*_tmp505;struct Cyc_List_List*_tmp504;int _tmp503;struct Cyc_Absyn_VarargInfo*_tmp502;struct Cyc_List_List*_tmp501;struct Cyc_List_List*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp4FE;void*_tmp4FD;struct Cyc_Absyn_Tqual _tmp4FC;struct Cyc_Absyn_PtrAtts _tmp4FB;void*_tmp4FA;struct Cyc_Absyn_Tqual _tmp4F9;struct Cyc_Absyn_Exp*_tmp4F8;void*_tmp4F7;unsigned int _tmp4F6;switch(*((int*)_tmp4D2)){case 4U: _LL1: _tmp4FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).elt_type;_tmp4F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).tq;_tmp4F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).num_elts;_tmp4F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).zero_term;_tmp4F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D2)->f1).zt_loc;_LL2: {
# 2065
struct _tuple12 _tmp4D3=Cyc_Absynpp_to_tms(r,_tmp4F9,_tmp4FA);struct _tuple12 _tmp4D4=_tmp4D3;struct Cyc_Absyn_Tqual _tmp4DA;void*_tmp4D9;struct Cyc_List_List*_tmp4D8;_LLE: _tmp4DA=_tmp4D4.f1;_tmp4D9=_tmp4D4.f2;_tmp4D8=_tmp4D4.f3;_LLF:;{
void*tm;
if(_tmp4F8 == 0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4D5=_region_malloc(r,sizeof(*_tmp4D5));_tmp4D5->tag=0U,_tmp4D5->f1=_tmp4F7,_tmp4D5->f2=_tmp4F6;_tmp4D5;});else{
# 2070
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6));_tmp4D6->tag=1U,_tmp4D6->f1=_tmp4F8,_tmp4D6->f2=_tmp4F7,_tmp4D6->f3=_tmp4F6;_tmp4D6;});}
return({struct _tuple12 _tmp54F;_tmp54F.f1=_tmp4DA,_tmp54F.f2=_tmp4D9,({struct Cyc_List_List*_tmp885=({struct Cyc_List_List*_tmp4D7=_region_malloc(r,sizeof(*_tmp4D7));_tmp4D7->hd=tm,_tmp4D7->tl=_tmp4D8;_tmp4D7;});_tmp54F.f3=_tmp885;});_tmp54F;});};}case 3U: _LL3: _tmp4FD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D2)->f1).elt_type;_tmp4FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D2)->f1).elt_tq;_tmp4FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D2)->f1).ptr_atts;_LL4: {
# 2074
struct _tuple12 _tmp4DB=Cyc_Absynpp_to_tms(r,_tmp4FC,_tmp4FD);struct _tuple12 _tmp4DC=_tmp4DB;struct Cyc_Absyn_Tqual _tmp4E1;void*_tmp4E0;struct Cyc_List_List*_tmp4DF;_LL11: _tmp4E1=_tmp4DC.f1;_tmp4E0=_tmp4DC.f2;_tmp4DF=_tmp4DC.f3;_LL12:;
_tmp4DF=({struct Cyc_List_List*_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE));({void*_tmp886=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4DD=_region_malloc(r,sizeof(*_tmp4DD));_tmp4DD->tag=2U,_tmp4DD->f1=_tmp4FB,_tmp4DD->f2=tq;_tmp4DD;});_tmp4DE->hd=_tmp886;}),_tmp4DE->tl=_tmp4DF;_tmp4DE;});
return({struct _tuple12 _tmp550;_tmp550.f1=_tmp4E1,_tmp550.f2=_tmp4E0,_tmp550.f3=_tmp4DF;_tmp550;});}case 5U: _LL5: _tmp508=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).tvars;_tmp507=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).effect;_tmp506=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).ret_tqual;_tmp505=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).ret_type;_tmp504=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).args;_tmp503=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).c_varargs;_tmp502=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).cyc_varargs;_tmp501=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).rgn_po;_tmp500=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).attributes;_tmp4FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).requires_clause;_tmp4FE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D2)->f1).ensures_clause;_LL6:
# 2080
 if(!Cyc_Absynpp_print_all_tvars){
# 2084
if(_tmp507 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp507)){
_tmp507=0;
_tmp508=0;}}else{
# 2089
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2092
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp508);}{
# 2097
struct _tuple12 _tmp4E2=Cyc_Absynpp_to_tms(r,_tmp506,_tmp505);struct _tuple12 _tmp4E3=_tmp4E2;struct Cyc_Absyn_Tqual _tmp4F5;void*_tmp4F4;struct Cyc_List_List*_tmp4F3;_LL14: _tmp4F5=_tmp4E3.f1;_tmp4F4=_tmp4E3.f2;_tmp4F3=_tmp4E3.f3;_LL15:;{
struct Cyc_List_List*tms=_tmp4F3;
# 2108 "absynpp.cyc"
{enum Cyc_Cyclone_C_Compilers _tmp4E4=Cyc_Cyclone_c_compiler;if(_tmp4E4 == Cyc_Cyclone_Gcc_c){_LL17: _LL18:
# 2110
 if(_tmp500 != 0)
# 2113
tms=({struct _RegionHandle*_tmp888=r;void*_tmp887=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5));_tmp4E5->tag=5U,_tmp4E5->f1=0U,_tmp4E5->f2=_tmp500;_tmp4E5;});Cyc_Absynpp_bubble_attributes(_tmp888,_tmp887,tms);});
# 2115
tms=({struct Cyc_List_List*_tmp4E8=_region_malloc(r,sizeof(*_tmp4E8));({void*_tmp88A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E7=_region_malloc(r,sizeof(*_tmp4E7));
_tmp4E7->tag=3U,({void*_tmp889=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6));_tmp4E6->tag=1U,_tmp4E6->f1=_tmp504,_tmp4E6->f2=_tmp503,_tmp4E6->f3=_tmp502,_tmp4E6->f4=_tmp507,_tmp4E6->f5=_tmp501,_tmp4E6->f6=_tmp4FF,_tmp4E6->f7=_tmp4FE;_tmp4E6;});_tmp4E7->f1=_tmp889;});_tmp4E7;});
# 2115
_tmp4E8->hd=_tmp88A;}),_tmp4E8->tl=tms;_tmp4E8;});
# 2119
goto _LL16;}else{_LL19: _LL1A:
# 2121
 tms=({struct Cyc_List_List*_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB));({void*_tmp88C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(r,sizeof(*_tmp4EA));
_tmp4EA->tag=3U,({void*_tmp88B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9));_tmp4E9->tag=1U,_tmp4E9->f1=_tmp504,_tmp4E9->f2=_tmp503,_tmp4E9->f3=_tmp502,_tmp4E9->f4=_tmp507,_tmp4E9->f5=_tmp501,_tmp4E9->f6=_tmp4FF,_tmp4E9->f7=_tmp4FE;_tmp4E9;});_tmp4EA->f1=_tmp88B;});_tmp4EA;});
# 2121
_tmp4EB->hd=_tmp88C;}),_tmp4EB->tl=tms;_tmp4EB;});
# 2125
for(0;_tmp500 != 0;_tmp500=_tmp500->tl){
void*_tmp4EC=(void*)_tmp500->hd;void*_tmp4ED=_tmp4EC;switch(*((int*)_tmp4ED)){case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
# 2130
 tms=({struct Cyc_List_List*_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0));({void*_tmp88E=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF));_tmp4EF->tag=5U,_tmp4EF->f1=0U,({struct Cyc_List_List*_tmp88D=({struct Cyc_List_List*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE->hd=(void*)_tmp500->hd,_tmp4EE->tl=0;_tmp4EE;});_tmp4EF->f2=_tmp88D;});_tmp4EF;});_tmp4F0->hd=_tmp88E;}),_tmp4F0->tl=tms;_tmp4F0;});
goto AfterAtts;default: _LL22: _LL23:
 goto _LL1B;}_LL1B:;}
# 2134
goto _LL16;}_LL16:;}
# 2137
AfterAtts:
 if(_tmp508 != 0)
tms=({struct Cyc_List_List*_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2));({void*_tmp88F=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F1=_region_malloc(r,sizeof(*_tmp4F1));_tmp4F1->tag=4U,_tmp4F1->f1=_tmp508,_tmp4F1->f2=0U,_tmp4F1->f3=1;_tmp4F1;});_tmp4F2->hd=_tmp88F;}),_tmp4F2->tl=tms;_tmp4F2;});
return({struct _tuple12 _tmp551;_tmp551.f1=_tmp4F5,_tmp551.f2=_tmp4F4,_tmp551.f3=tms;_tmp551;});};};case 1U: _LL7: _tmp50B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D2)->f1;_tmp50A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D2)->f2;_tmp509=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D2)->f3;_LL8:
# 2143
 if(_tmp50A == 0)
return({struct _tuple12 _tmp552;_tmp552.f1=tq,_tmp552.f2=t,_tmp552.f3=0;_tmp552;});else{
# 2146
return Cyc_Absynpp_to_tms(r,tq,_tmp50A);}case 8U: _LL9: _tmp50F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D2)->f1;_tmp50E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D2)->f2;_tmp50D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D2)->f3;_tmp50C=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D2)->f4;_LLA:
# 2152
 if(_tmp50C == 0  || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple12 _tmp553;_tmp553.f1=tq,_tmp553.f2=t,_tmp553.f3=0;_tmp553;});else{
# 2155
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp50C);}default: _LLB: _LLC:
# 2160
 return({struct _tuple12 _tmp554;_tmp554.f1=tq,_tmp554.f2=t,_tmp554.f3=0;_tmp554;});}_LL0:;}
# 2164
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2166
void*_tmp510=t;void*_tmp515;void*_tmp514;switch(*((int*)_tmp510)){case 1U: _LL1: _tmp514=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp510)->f2;if(_tmp514 != 0){_LL2:
 return Cyc_Absynpp_is_char_ptr(_tmp514);}else{goto _LL5;}case 3U: _LL3: _tmp515=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510)->f1).elt_type;_LL4:
# 2169
 L: {
void*_tmp511=_tmp515;void*_tmp513;void*_tmp512;switch(*((int*)_tmp511)){case 1U: _LL8: _tmp512=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp511)->f2;if(_tmp512 != 0){_LL9:
 _tmp515=_tmp512;goto L;}else{goto _LLE;}case 8U: _LLA: _tmp513=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp511)->f4;if(_tmp513 != 0){_LLB:
 _tmp515=_tmp513;goto L;}else{goto _LLE;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp511)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp511)->f1)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 2176
 return 0;}_LL0:;}
# 2180
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp516=_new_region("temp");struct _RegionHandle*temp=& _tmp516;_push_region(temp);
{struct _tuple12 _tmp517=Cyc_Absynpp_to_tms(temp,tq,typ);struct _tuple12 _tmp518=_tmp517;struct Cyc_Absyn_Tqual _tmp520;void*_tmp51F;struct Cyc_List_List*_tmp51E;_LL1: _tmp520=_tmp518.f1;_tmp51F=_tmp518.f2;_tmp51E=_tmp518.f3;_LL2:;
_tmp51E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp51E);
if(_tmp51E == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmp51A=({struct Cyc_PP_Doc*_tmp519[2U];({struct Cyc_PP_Doc*_tmp891=Cyc_Absynpp_tqual2doc(_tmp520);_tmp519[0]=_tmp891;}),({struct Cyc_PP_Doc*_tmp890=Cyc_Absynpp_ntyp2doc(_tmp51F);_tmp519[1]=_tmp890;});Cyc_PP_cat(_tag_dyneither(_tmp519,sizeof(struct Cyc_PP_Doc*),2U));});_npop_handler(0U);return _tmp51A;}else{
# 2187
struct Cyc_PP_Doc*_tmp51D=({struct Cyc_PP_Doc*_tmp51B[4U];({
struct Cyc_PP_Doc*_tmp897=Cyc_Absynpp_tqual2doc(_tmp520);_tmp51B[0]=_tmp897;}),({
struct Cyc_PP_Doc*_tmp896=Cyc_Absynpp_ntyp2doc(_tmp51F);_tmp51B[1]=_tmp896;}),({
struct Cyc_PP_Doc*_tmp895=Cyc_PP_text(({const char*_tmp51C=" ";_tag_dyneither(_tmp51C,sizeof(char),2U);}));_tmp51B[2]=_tmp895;}),({
struct Cyc_PP_Doc*_tmp894=({int _tmp893=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_tmp892=dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_tmp893,_tmp892,_tmp51E);});_tmp51B[3]=_tmp894;});Cyc_PP_cat(_tag_dyneither(_tmp51B,sizeof(struct Cyc_PP_Doc*),4U));});_npop_handler(0U);return _tmp51D;}}
# 2182
;_pop_region(temp);}
# 2195
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp521=f->requires_clause;
if((unsigned int)_tmp521)
requires_doc=({struct Cyc_PP_Doc*_tmp522[2U];({struct Cyc_PP_Doc*_tmp899=Cyc_PP_text(({const char*_tmp523="@requires ";_tag_dyneither(_tmp523,sizeof(char),11U);}));_tmp522[0]=_tmp899;}),({struct Cyc_PP_Doc*_tmp898=Cyc_Absynpp_exp2doc(_tmp521);_tmp522[1]=_tmp898;});Cyc_PP_cat(_tag_dyneither(_tmp522,sizeof(struct Cyc_PP_Doc*),2U));});else{
# 2201
requires_doc=Cyc_PP_nil_doc();}{
# 2203
enum Cyc_Cyclone_C_Compilers _tmp524=Cyc_Cyclone_c_compiler;if(_tmp524 == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 2206
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp525[5U];({struct Cyc_PP_Doc*_tmp8A1=({struct Cyc_Absyn_Tqual _tmp8A0=f->tq;void*_tmp89F=f->type;Cyc_Absynpp_tqtd2doc(_tmp8A0,_tmp89F,({struct Cyc_Core_Opt*_tmp526=_cycalloc(sizeof(*_tmp526));({struct Cyc_PP_Doc*_tmp89E=Cyc_PP_textptr(f->name);_tmp526->v=_tmp89E;});_tmp526;}));});_tmp525[0]=_tmp8A1;}),({
struct Cyc_PP_Doc*_tmp89D=Cyc_PP_text(({const char*_tmp527=":";_tag_dyneither(_tmp527,sizeof(char),2U);}));_tmp525[1]=_tmp89D;}),({struct Cyc_PP_Doc*_tmp89C=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp525[2]=_tmp89C;}),({
struct Cyc_PP_Doc*_tmp89B=Cyc_Absynpp_atts2doc(f->attributes);_tmp525[3]=_tmp89B;}),({struct Cyc_PP_Doc*_tmp89A=Cyc_PP_text(({const char*_tmp528=";";_tag_dyneither(_tmp528,sizeof(char),2U);}));_tmp525[4]=_tmp89A;});Cyc_PP_cat(_tag_dyneither(_tmp525,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2211
return({struct Cyc_PP_Doc*_tmp529[4U];({struct Cyc_PP_Doc*_tmp8A7=({struct Cyc_Absyn_Tqual _tmp8A6=f->tq;void*_tmp8A5=f->type;Cyc_Absynpp_tqtd2doc(_tmp8A6,_tmp8A5,({struct Cyc_Core_Opt*_tmp52A=_cycalloc(sizeof(*_tmp52A));({struct Cyc_PP_Doc*_tmp8A4=Cyc_PP_textptr(f->name);_tmp52A->v=_tmp8A4;});_tmp52A;}));});_tmp529[0]=_tmp8A7;}),({
struct Cyc_PP_Doc*_tmp8A3=Cyc_Absynpp_atts2doc(f->attributes);_tmp529[1]=_tmp8A3;}),_tmp529[2]=requires_doc,({struct Cyc_PP_Doc*_tmp8A2=Cyc_PP_text(({const char*_tmp52B=";";_tag_dyneither(_tmp52B,sizeof(char),2U);}));_tmp529[3]=_tmp8A2;});Cyc_PP_cat(_tag_dyneither(_tmp529,sizeof(struct Cyc_PP_Doc*),4U));});}}else{_LL3: _LL4:
# 2214
 if(f->width != 0)
return({struct Cyc_PP_Doc*_tmp52C[5U];({struct Cyc_PP_Doc*_tmp8AF=Cyc_Absynpp_atts2doc(f->attributes);_tmp52C[0]=_tmp8AF;}),({
struct Cyc_PP_Doc*_tmp8AE=({struct Cyc_Absyn_Tqual _tmp8AD=f->tq;void*_tmp8AC=f->type;Cyc_Absynpp_tqtd2doc(_tmp8AD,_tmp8AC,({struct Cyc_Core_Opt*_tmp52D=_cycalloc(sizeof(*_tmp52D));({struct Cyc_PP_Doc*_tmp8AB=Cyc_PP_textptr(f->name);_tmp52D->v=_tmp8AB;});_tmp52D;}));});_tmp52C[1]=_tmp8AE;}),({
struct Cyc_PP_Doc*_tmp8AA=Cyc_PP_text(({const char*_tmp52E=":";_tag_dyneither(_tmp52E,sizeof(char),2U);}));_tmp52C[2]=_tmp8AA;}),({struct Cyc_PP_Doc*_tmp8A9=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp52C[3]=_tmp8A9;}),({struct Cyc_PP_Doc*_tmp8A8=Cyc_PP_text(({const char*_tmp52F=";";_tag_dyneither(_tmp52F,sizeof(char),2U);}));_tmp52C[4]=_tmp8A8;});Cyc_PP_cat(_tag_dyneither(_tmp52C,sizeof(struct Cyc_PP_Doc*),5U));});else{
# 2219
return({struct Cyc_PP_Doc*_tmp530[4U];({struct Cyc_PP_Doc*_tmp8B5=Cyc_Absynpp_atts2doc(f->attributes);_tmp530[0]=_tmp8B5;}),({
struct Cyc_PP_Doc*_tmp8B4=({struct Cyc_Absyn_Tqual _tmp8B3=f->tq;void*_tmp8B2=f->type;Cyc_Absynpp_tqtd2doc(_tmp8B3,_tmp8B2,({struct Cyc_Core_Opt*_tmp531=_cycalloc(sizeof(*_tmp531));({struct Cyc_PP_Doc*_tmp8B1=Cyc_PP_textptr(f->name);_tmp531->v=_tmp8B1;});_tmp531;}));});_tmp530[1]=_tmp8B4;}),_tmp530[2]=requires_doc,({
struct Cyc_PP_Doc*_tmp8B0=Cyc_PP_text(({const char*_tmp532=";";_tag_dyneither(_tmp532,sizeof(char),2U);}));_tmp530[3]=_tmp8B0;});Cyc_PP_cat(_tag_dyneither(_tmp530,sizeof(struct Cyc_PP_Doc*),4U));});}}_LL0:;};}
# 2226
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp8B6=({const char*_tmp533="";_tag_dyneither(_tmp533,sizeof(char),1U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tmp8B6,fields);});}
# 2230
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_tmp534[3U];({struct Cyc_PP_Doc*_tmp8BA=Cyc_Absynpp_scope2doc(f->sc);_tmp534[0]=_tmp8BA;}),({struct Cyc_PP_Doc*_tmp8B9=Cyc_Absynpp_typedef_name2doc(f->name);_tmp534[1]=_tmp8B9;}),
f->typs == 0?({struct Cyc_PP_Doc*_tmp8B8=Cyc_PP_nil_doc();_tmp534[2]=_tmp8B8;}):({struct Cyc_PP_Doc*_tmp8B7=Cyc_Absynpp_args2doc(f->typs);_tmp534[2]=_tmp8B7;});Cyc_PP_cat(_tag_dyneither(_tmp534,sizeof(struct Cyc_PP_Doc*),3U));});}
# 2235
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct _dyneither_ptr _tmp8BB=({const char*_tmp535=",";_tag_dyneither(_tmp535,sizeof(char),2U);});((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,_tmp8BB,fields);});}
# 2244
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_tmp8BC=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_tmp8BC,72,f);});
({void*_tmp536=0U;({struct Cyc___cycFILE*_tmp8BE=f;struct _dyneither_ptr _tmp8BD=({const char*_tmp537="\n";_tag_dyneither(_tmp537,sizeof(char),2U);});Cyc_fprintf(_tmp8BE,_tmp8BD,_tag_dyneither(_tmp536,sizeof(void*),0U));});});}}
# 2251
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _dyneither_ptr _tmp8BF=({const char*_tmp538="";_tag_dyneither(_tmp538,sizeof(char),1U);});Cyc_PP_seql(_tmp8BF,((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2254
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2258
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
# 2269
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
