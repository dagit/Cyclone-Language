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
# 119 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100 "cycboot.h"
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 235
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d);
# 274
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);
# 46 "graph.h"
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*));
# 49
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*));
# 52
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*s);
# 55
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 58
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*s,struct Cyc_Set_Set*T);
# 62
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 65
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*s,struct Cyc_Set_Set*T);
# 68
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 71
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict,void*s);
# 73
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g);
# 79
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict g);
# 86
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict g);
# 27 "graph.cyc"
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*)){
return((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(cmp);}
# 34
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict g,void*source){
struct Cyc_Set_Set**_tmp0=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_tmp1=_tmp0;struct Cyc_Set_Set*_tmp2;if(_tmp1 == 0){_LL1: _LL2:
 return Cyc_Set_empty(g.rel);}else{_LL3: _tmp2=*_tmp1;_LL4:
 return _tmp2;}_LL0:;}
# 45
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*source){
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(g,source))return g;else{
return({struct Cyc_Dict_Dict _tmp88=g;void*_tmp87=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp88,_tmp87,Cyc_Set_empty(g.rel));});}}
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp3=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp8A=g;void*_tmp89=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp8A,_tmp89,Cyc_Set_insert(_tmp3,target));});}
# 61
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp4=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp8C=g;void*_tmp8B=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp8C,_tmp8B,Cyc_Set_union_two(_tmp4,targets));});}
# 69
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp5=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp8E=g;void*_tmp8D=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp8E,_tmp8D,Cyc_Set_delete(_tmp5,target));});}
# 77
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp6=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp90=g;void*_tmp8F=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp90,_tmp8F,Cyc_Set_diff(_tmp6,targets));});}
# 86
static struct Cyc_List_List*Cyc_Graph_to_list(struct Cyc_Set_Set*ts){
# 88
struct Cyc_List_List*_tmp7=0;
while(!Cyc_Set_is_empty(ts)){
void*_tmp8=Cyc_Set_choose(ts);
ts=Cyc_Set_delete(ts,_tmp8);
_tmp7=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->hd=_tmp8,_tmp9->tl=_tmp7;_tmp9;});}
# 94
return _tmp7;}
# 100
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set**_tmpA=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_tmpB=_tmpA;struct Cyc_Set_Set*_tmpC;if(_tmpB == 0){_LL1: _LL2:
# 103
 return 0;}else{_LL3: _tmpC=*_tmpB;_LL4:
# 105
 return Cyc_Set_member(_tmpC,target);}_LL0:;}struct _tuple0{void*f1;struct Cyc_Set_Set*f2;};
# 112
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*)){
struct Cyc_List_List*_tmpD=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);
for(0;(unsigned int)_tmpD;_tmpD=_tmpD->tl){
struct _tuple0*_tmpE=(struct _tuple0*)_tmpD->hd;struct _tuple0*_tmpF=_tmpE;void*_tmp18;struct Cyc_Set_Set*_tmp17;_LL1: _tmp18=_tmpF->f1;_tmp17=_tmpF->f2;_LL2:;{
struct Cyc_List_List*_tmp10=Cyc_Graph_to_list(_tmp17);
if((unsigned int)_tmp10)
for(0;(unsigned int)_tmp10;_tmp10=_tmp10->tl){
nodeprint(f,_tmp18);
({void*_tmp11=0U;({struct Cyc___cycFILE*_tmp92=f;struct _dyneither_ptr _tmp91=({const char*_tmp12=" --> ";_tag_dyneither(_tmp12,sizeof(char),6U);});Cyc_fprintf(_tmp92,_tmp91,_tag_dyneither(_tmp11,sizeof(void*),0U));});});
nodeprint(f,_tmp10->hd);
({void*_tmp13=0U;({struct Cyc___cycFILE*_tmp94=f;struct _dyneither_ptr _tmp93=({const char*_tmp14="\n";_tag_dyneither(_tmp14,sizeof(char),2U);});Cyc_fprintf(_tmp94,_tmp93,_tag_dyneither(_tmp13,sizeof(void*),0U));});});}else{
# 125
nodeprint(f,_tmp18);
({void*_tmp15=0U;({struct Cyc___cycFILE*_tmp96=f;struct _dyneither_ptr _tmp95=({const char*_tmp16=" --> <no targets>\n";_tag_dyneither(_tmp16,sizeof(char),19U);});Cyc_fprintf(_tmp96,_tmp95,_tag_dyneither(_tmp15,sizeof(void*),0U));});});}};}}
# 138 "graph.cyc"
static struct Cyc_List_List*Cyc_Graph_sourcesOf(struct Cyc_Dict_Dict g,void*node){
struct Cyc_List_List*_tmp19=0;
# 141
{struct Cyc_List_List*_tmp1A=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);for(0;(unsigned int)_tmp1A;_tmp1A=_tmp1A->tl){
struct _tuple0*_tmp1B=(struct _tuple0*)_tmp1A->hd;struct _tuple0*_tmp1C=_tmp1B;void*_tmp1F;struct Cyc_Set_Set*_tmp1E;_LL1: _tmp1F=_tmp1C->f1;_tmp1E=_tmp1C->f2;_LL2:;
if(Cyc_Set_member(_tmp1E,node))_tmp19=({struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=_tmp1F,_tmp1D->tl=_tmp19;_tmp1D;});}}
# 145
return _tmp19;}struct _tuple1{struct Cyc_Set_Set*f1;struct Cyc_Dict_Dict f2;};
# 154
static struct _tuple1 Cyc_Graph_divideGraph(struct Cyc_Dict_Dict g,void*source){
# 157
struct Cyc_Set_Set**_tmp20=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_tmp21=_tmp20;struct Cyc_Set_Set*_tmp22;if(_tmp21 == 0){_LL1: _LL2:
 return({struct _tuple1 _tmp85;({struct Cyc_Set_Set*_tmp97=Cyc_Set_empty(g.rel);_tmp85.f1=_tmp97;}),_tmp85.f2=g;_tmp85;});}else{_LL3: _tmp22=*_tmp21;_LL4:
 return({struct _tuple1 _tmp86;_tmp86.f1=_tmp22,({struct Cyc_Dict_Dict _tmp98=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_delete)(g,source);_tmp86.f2=_tmp98;});_tmp86;});}_LL0:;}
# 166
static struct Cyc_Dict_Dict Cyc_Graph_add_edgeTc(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp23=Cyc_Graph_get_targets(g,target);
struct _tuple1 _tmp24=Cyc_Graph_divideGraph(g,source);struct _tuple1 _tmp25=_tmp24;struct Cyc_Set_Set*_tmp2B;struct Cyc_Dict_Dict _tmp2A;_LL1: _tmp2B=_tmp25.f1;_tmp2A=_tmp25.f2;_LL2:;{
struct Cyc_List_List*_tmp26=Cyc_Graph_sourcesOf(_tmp2A,source);
struct Cyc_Set_Set*_tmp27=({struct Cyc_Set_Set*_tmp99=Cyc_Set_union_two(_tmp2B,_tmp23);Cyc_Set_insert(_tmp99,target);});
struct Cyc_Dict_Dict _tmp28=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp2A,source,_tmp27);
{struct Cyc_List_List*_tmp29=_tmp26;for(0;(unsigned int)_tmp29;_tmp29=_tmp29->tl){
_tmp28=Cyc_Graph_add_edges(_tmp28,_tmp29->hd,_tmp27);}}
# 175
return _tmp28;};}
# 177
struct Cyc_Dict_Dict Cyc_Graph_tc_slow(struct Cyc_Dict_Dict g){
struct Cyc_Dict_Dict _tmp2C=Cyc_Graph_empty(g.rel);
struct Cyc_List_List*_tmp2D=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);
for(0;(unsigned int)_tmp2D;_tmp2D=_tmp2D->tl){
struct _tuple0*_tmp2E=(struct _tuple0*)_tmp2D->hd;struct _tuple0*_tmp2F=_tmp2E;void*_tmp33;struct Cyc_Set_Set*_tmp32;_LL1: _tmp33=_tmp2F->f1;_tmp32=_tmp2F->f2;_LL2:;{
struct Cyc_List_List*_tmp30=Cyc_Graph_to_list(_tmp32);
if((unsigned int)_tmp30){
struct Cyc_List_List*_tmp31=_tmp30;for(0;(unsigned int)_tmp31;_tmp31=_tmp31->tl){
_tmp2C=Cyc_Graph_add_edgeTc(_tmp2C,_tmp33,_tmp31->hd);}}else{
_tmp2C=Cyc_Graph_add_node(_tmp2C,_tmp33);}};}
# 188
return _tmp2C;}struct Cyc_Graph_nodestate{void*root;int C;int visitindex;};struct Cyc_Graph_componentstate{struct Cyc_Set_Set*Succ;struct Cyc_Set_Set*nodes;};struct Cyc_Graph_tcstate{struct Cyc_Set_Set*visited;int visitindex;struct Cyc_Dict_Dict ns;struct Cyc_Dict_Dict cs;int Cindex;struct Cyc_List_List*nstack;struct Cyc_List_List*cstack;};
# 214
static void Cyc_Graph_comp_tc(struct Cyc_Graph_tcstate*ts,struct Cyc_Dict_Dict g,void*v){
# 216
int(*_tmp34)(void*,void*)=g.rel;
({struct Cyc_Set_Set*_tmp9A=Cyc_Set_insert(ts->visited,v);ts->visited=_tmp9A;});{
struct Cyc_Graph_nodestate*_tmp35=({struct Cyc_Graph_nodestate*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->root=v,_tmp49->C=0,_tmp49->visitindex=ts->visitindex ++;_tmp49;});
({struct Cyc_Dict_Dict _tmp9B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Graph_nodestate*v))Cyc_Dict_insert)(ts->ns,v,_tmp35);ts->ns=_tmp9B;});
({struct Cyc_List_List*_tmp9C=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->hd=v,_tmp36->tl=ts->nstack;_tmp36;});ts->nstack=_tmp9C;});{
int _tmp37=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts->cstack);
struct Cyc_Set_Set*_tmp38=Cyc_Graph_get_targets(g,v);
while(!Cyc_Set_is_empty(_tmp38)){
void*_tmp39=Cyc_Set_choose(_tmp38);
_tmp38=Cyc_Set_delete(_tmp38,_tmp39);
if(_tmp34(v,_tmp39)== 0)continue;{
int is_forward_edge;
struct Cyc_Graph_nodestate*nsw;
if(Cyc_Set_member(ts->visited,_tmp39)){
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp39);
is_forward_edge=nsw->visitindex < _tmp35->visitindex?0: 1;}else{
# 234
is_forward_edge=0;
Cyc_Graph_comp_tc(ts,g,_tmp39);
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp39);}{
# 238
int _tmp3A=nsw->C;
if(_tmp3A == 0){
struct Cyc_Graph_nodestate*_tmp3B=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp35->root);
struct Cyc_Graph_nodestate*_tmp3C=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,nsw->root);
# 243
if(_tmp3B->visitindex > _tmp3C->visitindex)
_tmp35->root=nsw->root;}else{
# 247
if(!is_forward_edge)
({struct Cyc_List_List*_tmp9D=({struct Cyc_List_List*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->hd=(void*)_tmp3A,_tmp3D->tl=ts->cstack;_tmp3D;});ts->cstack=_tmp9D;});}};};}
# 250
if(_tmp34(_tmp35->root,v)!= 0)
return;{
# 253
int _tmp3E=ts->Cindex ++;
struct Cyc_Graph_componentstate*_tmp3F=({struct Cyc_Graph_componentstate*_tmp48=_cycalloc(sizeof(*_tmp48));({struct Cyc_Set_Set*_tmp9F=((struct Cyc_Set_Set*(*)(int(*cmp)(int,int)))Cyc_Set_empty)(Cyc_Core_intcmp);_tmp48->Succ=_tmp9F;}),({
struct Cyc_Set_Set*_tmp9E=Cyc_Set_empty(_tmp34);_tmp48->nodes=_tmp9E;});_tmp48;});
({struct Cyc_Dict_Dict _tmpA0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,int k,struct Cyc_Graph_componentstate*v))Cyc_Dict_insert)(ts->cs,_tmp3E,_tmp3F);ts->cs=_tmpA0;});
{struct Cyc_List_List*_tmp40=ts->nstack;struct Cyc_List_List*_tmp41=_tmp40;void*_tmp42;if(_tmp41 == 0){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp42=(void*)_tmp41->hd;_LL4:
# 260
 if(_tmp34(_tmp42,v)!= 0  || 
Cyc_Graph_is_edge(g,v,v))
({struct Cyc_Set_Set*_tmpA1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_insert)(_tmp3F->Succ,_tmp3E);_tmp3F->Succ=_tmpA1;});
# 264
goto _LL0;}_LL0:;}
# 266
{int _tmp43=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts->cstack);for(0;_tmp43 > _tmp37;-- _tmp43){
int _tmp44=(int)((struct Cyc_List_List*)_check_null(ts->cstack))->hd;
ts->cstack=((struct Cyc_List_List*)_check_null(ts->cstack))->tl;
if(!((int(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_member)(_tmp3F->Succ,_tmp44)){
struct Cyc_Set_Set*_tmp45=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_insert)(_tmp3F->Succ,_tmp44);
_tmp45=({struct Cyc_Set_Set*_tmpA2=_tmp45;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmpA2,(((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(ts->cs,_tmp44))->Succ);});
_tmp3F->Succ=_tmp45;}}}
# 275
while((unsigned int)ts->nstack){
void*_tmp46=((struct Cyc_List_List*)_check_null(ts->nstack))->hd;
ts->nstack=((struct Cyc_List_List*)_check_null(ts->nstack))->tl;{
struct Cyc_Graph_nodestate*_tmp47=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp46);
_tmp47->C=_tmp3E;
({struct Cyc_Set_Set*_tmpA3=Cyc_Set_insert(_tmp3F->nodes,_tmp46);_tmp3F->nodes=_tmpA3;});
if(_tmp34(_tmp46,v)== 0)break;};}};};};}
# 284
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g){
int(*_tmp4A)(void*,void*)=g.rel;
struct Cyc_Graph_tcstate*_tmp4B=({struct Cyc_Graph_tcstate*_tmp59=_cycalloc(sizeof(*_tmp59));
({
struct Cyc_Set_Set*_tmpA6=Cyc_Set_empty(_tmp4A);_tmp59->visited=_tmpA6;}),_tmp59->visitindex=1,({
# 290
struct Cyc_Dict_Dict _tmpA5=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp4A);_tmp59->ns=_tmpA5;}),({
struct Cyc_Dict_Dict _tmpA4=((struct Cyc_Dict_Dict(*)(int(*cmp)(int,int)))Cyc_Dict_empty)(Cyc_Core_intcmp);_tmp59->cs=_tmpA4;}),_tmp59->Cindex=1,_tmp59->nstack=0,_tmp59->cstack=0;_tmp59;});
# 296
{struct Cyc_List_List*_tmp4C=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);for(0;(unsigned int)_tmp4C;_tmp4C=_tmp4C->tl){
struct _tuple0*_tmp4D=(struct _tuple0*)_tmp4C->hd;struct _tuple0*_tmp4E=_tmp4D;void*_tmp50;struct Cyc_Set_Set*_tmp4F;_LL1: _tmp50=_tmp4E->f1;_tmp4F=_tmp4E->f2;_LL2:;
if(!Cyc_Set_member(_tmp4B->visited,_tmp50))Cyc_Graph_comp_tc(_tmp4B,g,_tmp50);}}{
# 301
struct Cyc_Dict_Dict _tmp51=Cyc_Graph_empty(_tmp4A);
{int _tmp52=1;for(0;_tmp52 < _tmp4B->Cindex;++ _tmp52){
struct Cyc_Graph_componentstate*_tmp53=((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(_tmp4B->cs,_tmp52);
struct Cyc_Set_Set*_tmp54=Cyc_Set_empty(_tmp4A);
struct Cyc_Set_Set*_tmp55=_tmp53->Succ;
while(!((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp55)){
int _tmp56=((int(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp55);
_tmp55=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_delete)(_tmp55,_tmp56);
_tmp54=({struct Cyc_Set_Set*_tmpA7=_tmp54;Cyc_Set_union_two(_tmpA7,(
((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(_tmp4B->cs,_tmp56))->nodes);});}{
# 312
struct Cyc_Set_Set*_tmp57=_tmp53->nodes;
while(!Cyc_Set_is_empty(_tmp57)){
void*_tmp58=Cyc_Set_choose(_tmp57);
_tmp57=Cyc_Set_delete(_tmp57,_tmp58);
_tmp51=Cyc_Graph_add_edges(_tmp51,_tmp58,_tmp54);}};}}
# 319
return _tmp51;};}
# 332 "graph.cyc"
static void Cyc_Graph_traverse(struct Cyc_Dict_Dict input,struct Cyc_Dict_Dict*output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned int k){
# 336
({struct Cyc_List_List*_tmpA8=({struct Cyc_List_List*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->hd=x,_tmp5A->tl=*S;_tmp5A;});*S=_tmpA8;});
({struct Cyc_Dict_Dict _tmpA9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,k);*N=_tmpA9;});
{struct Cyc_Set_Set*_tmp5B=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(_tmp5B);0){
void*_tmp5C=Cyc_Set_choose(_tmp5B);
_tmp5B=Cyc_Set_delete(_tmp5B,_tmp5C);
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp5C)== (unsigned int)0)
Cyc_Graph_traverse(input,output,N,S,_tmp5C,k + (unsigned int)1);{
unsigned int _tmp5D=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp5C);
if(({unsigned int _tmpAA=_tmp5D;_tmpAA < ((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpAB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,_tmp5D);*N=_tmpAB;});};}}
# 347
if(({unsigned int _tmpAC=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);_tmpAC == k;})){
int(*_tmp5E)(void*,void*)=(*output).rel;
struct Cyc_Set_Set*_tmp5F=Cyc_Set_empty(_tmp5E);
{struct Cyc_List_List*_tmp60=*S;for(0;(unsigned int)_tmp60;_tmp60=_tmp60->tl){
void*_tmp61=_tmp60->hd;
_tmp5F=Cyc_Set_insert(_tmp5F,_tmp61);
if(_tmp5E(_tmp61,x)== 0)break;}}
# 355
for(0;(unsigned int)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp62=((struct Cyc_List_List*)_check_null(*S))->hd;
({struct Cyc_Dict_Dict _tmpAD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,_tmp62,- 1U);*N=_tmpAD;});
({struct Cyc_Dict_Dict _tmpAE=Cyc_Graph_add_node(*output,_tmp62);*output=_tmpAE;});
({struct Cyc_Dict_Dict _tmpAF=Cyc_Graph_add_edges(*output,_tmp62,_tmp5F);*output=_tmpAF;});
if(_tmp5E(_tmp62,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 367
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict input){
int(*_tmp63)(void*,void*)=input.rel;
struct Cyc_List_List*_tmp64=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(input);
struct Cyc_Dict_Dict _tmp65=Cyc_Graph_empty(_tmp63);
struct Cyc_List_List*_tmp66=0;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp63);
{struct Cyc_List_List*_tmp67=_tmp64;for(0;(unsigned int)_tmp67;_tmp67=_tmp67->tl){
struct _tuple0*_tmp68=(struct _tuple0*)_tmp67->hd;struct _tuple0*_tmp69=_tmp68;void*_tmp6A;_LL1: _tmp6A=_tmp69->f1;_LL2:;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(N,_tmp6A,0U);}}
# 377
{struct Cyc_List_List*_tmp6B=_tmp64;for(0;(unsigned int)_tmp6B;_tmp6B=_tmp6B->tl){
struct _tuple0*_tmp6C=(struct _tuple0*)_tmp6B->hd;struct _tuple0*_tmp6D=_tmp6C;void*_tmp6E;_LL4: _tmp6E=_tmp6D->f1;_LL5:;
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(N,_tmp6E)== (unsigned int)0)
Cyc_Graph_traverse(input,& _tmp65,& N,& _tmp66,_tmp6E,1U);}}
# 382
return _tmp65;}
# 386
static void Cyc_Graph_tsort0(struct Cyc_Dict_Dict input,struct Cyc_List_List**output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned int k){
# 390
({struct Cyc_List_List*_tmpB0=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->hd=x,_tmp6F->tl=*S;_tmp6F;});*S=_tmpB0;});
({struct Cyc_Dict_Dict _tmpB1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,k);*N=_tmpB1;});
{struct Cyc_Set_Set*_tmp70=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(_tmp70);0){
void*_tmp71=Cyc_Set_choose(_tmp70);
_tmp70=Cyc_Set_delete(_tmp70,_tmp71);
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp71)== (unsigned int)0)
Cyc_Graph_tsort0(input,output,N,S,_tmp71,k + (unsigned int)1);{
unsigned int _tmp72=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp71);
if(({unsigned int _tmpB2=_tmp72;_tmpB2 < ((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpB3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,_tmp72);*N=_tmpB3;});};}}
# 401
if(({unsigned int _tmpB4=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);_tmpB4 == k;})){
int(*_tmp73)(void*,void*)=input.rel;
struct Cyc_Set_Set*_tmp74=Cyc_Set_empty(_tmp73);
{struct Cyc_List_List*_tmp75=*S;for(0;(unsigned int)_tmp75;_tmp75=_tmp75->tl){
void*_tmp76=_tmp75->hd;
_tmp74=Cyc_Set_insert(_tmp74,_tmp76);
if(_tmp73(_tmp76,x)== 0)break;}}
# 409
for(0;(unsigned int)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp77=((struct Cyc_List_List*)_check_null(*S))->hd;
({struct Cyc_Dict_Dict _tmpB5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,_tmp77,- 1U);*N=_tmpB5;});
({struct Cyc_List_List*_tmpB6=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->hd=_tmp77,_tmp78->tl=*output;_tmp78;});*output=_tmpB6;});
if(_tmp73(_tmp77,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 420
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict input){
int(*_tmp79)(void*,void*)=input.rel;
struct Cyc_List_List*_tmp7A=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(input);
struct Cyc_List_List*_tmp7B=0;
struct Cyc_List_List*_tmp7C=0;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp79);
{struct Cyc_List_List*_tmp7D=_tmp7A;for(0;(unsigned int)_tmp7D;_tmp7D=_tmp7D->tl){
struct _tuple0*_tmp7E=(struct _tuple0*)_tmp7D->hd;struct _tuple0*_tmp7F=_tmp7E;void*_tmp80;_LL1: _tmp80=_tmp7F->f1;_LL2:;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(N,_tmp80,0U);}}
# 430
{struct Cyc_List_List*_tmp81=_tmp7A;for(0;(unsigned int)_tmp81;_tmp81=_tmp81->tl){
struct _tuple0*_tmp82=(struct _tuple0*)_tmp81->hd;struct _tuple0*_tmp83=_tmp82;void*_tmp84;_LL4: _tmp84=_tmp83->f1;_LL5:;
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(N,_tmp84)== (unsigned int)0)
Cyc_Graph_tsort0(input,& _tmp7B,& N,& _tmp7C,_tmp84,1U);}}
# 435
return _tmp7B;}
