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
# 50
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*));
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*s);
# 56
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 59
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*s,struct Cyc_Set_Set*T);
# 63
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 66
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*s,struct Cyc_Set_Set*T);
# 69
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict g,void*s,void*t);
# 72
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict,void*s);
# 74
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g);
# 80
struct Cyc_Dict_Dict Cyc_Graph_tkernel(struct Cyc_Dict_Dict g);
# 85
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict g);
# 92
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
return({struct Cyc_Dict_Dict _tmp97=g;void*_tmp96=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp97,_tmp96,Cyc_Set_empty(g.rel));});}}
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp3=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp99=g;void*_tmp98=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp99,_tmp98,Cyc_Set_insert(_tmp3,target));});}
# 61
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp4=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp9B=g;void*_tmp9A=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp9B,_tmp9A,Cyc_Set_union_two(_tmp4,targets));});}
# 69
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp5=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp9D=g;void*_tmp9C=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp9D,_tmp9C,Cyc_Set_delete(_tmp5,target));});}
# 77
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*_tmp6=Cyc_Graph_get_targets(g,source);
return({struct Cyc_Dict_Dict _tmp9F=g;void*_tmp9E=source;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp9F,_tmp9E,Cyc_Set_diff(_tmp6,targets));});}
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
({void*_tmpD=0U;({struct Cyc___cycFILE*_tmpA1=f;struct _dyneither_ptr _tmpA0=({const char*_tmpE="digraph {\n";_tag_dyneither(_tmpE,sizeof(char),11U);});Cyc_fprintf(_tmpA1,_tmpA0,_tag_dyneither(_tmpD,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);
for(0;(unsigned int)_tmpF;_tmpF=_tmpF->tl){
struct _tuple0*_tmp10=(struct _tuple0*)_tmpF->hd;struct _tuple0*_tmp11=_tmp10;void*_tmp1C;struct Cyc_Set_Set*_tmp1B;_LL1: _tmp1C=_tmp11->f1;_tmp1B=_tmp11->f2;_LL2:;{
struct Cyc_List_List*_tmp12=Cyc_Graph_to_list(_tmp1B);
if((unsigned int)_tmp12)
for(0;(unsigned int)_tmp12;_tmp12=_tmp12->tl){
nodeprint(f,_tmp1C);
({void*_tmp13=0U;({struct Cyc___cycFILE*_tmpA3=f;struct _dyneither_ptr _tmpA2=({const char*_tmp14=" -> ";_tag_dyneither(_tmp14,sizeof(char),5U);});Cyc_fprintf(_tmpA3,_tmpA2,_tag_dyneither(_tmp13,sizeof(void*),0U));});});
nodeprint(f,_tmp12->hd);
({void*_tmp15=0U;({struct Cyc___cycFILE*_tmpA5=f;struct _dyneither_ptr _tmpA4=({const char*_tmp16=";\n";_tag_dyneither(_tmp16,sizeof(char),3U);});Cyc_fprintf(_tmpA5,_tmpA4,_tag_dyneither(_tmp15,sizeof(void*),0U));});});}else{
# 126
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmpA7=f;struct _dyneither_ptr _tmpA6=({const char*_tmp18="node ";_tag_dyneither(_tmp18,sizeof(char),6U);});Cyc_fprintf(_tmpA7,_tmpA6,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
nodeprint(f,_tmp1C);
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmpA9=f;struct _dyneither_ptr _tmpA8=({const char*_tmp1A="; // no targets\n";_tag_dyneither(_tmp1A,sizeof(char),17U);});Cyc_fprintf(_tmpA9,_tmpA8,_tag_dyneither(_tmp19,sizeof(void*),0U));});});}};}
# 131
({void*_tmp1D=0U;({struct Cyc___cycFILE*_tmpAB=f;struct _dyneither_ptr _tmpAA=({const char*_tmp1E="}\n";_tag_dyneither(_tmp1E,sizeof(char),3U);});Cyc_fprintf(_tmpAB,_tmpAA,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});};}
# 141 "graph.cyc"
static struct Cyc_List_List*Cyc_Graph_sourcesOf(struct Cyc_Dict_Dict g,void*node){
struct Cyc_List_List*_tmp1F=0;
# 144
{struct Cyc_List_List*_tmp20=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);for(0;(unsigned int)_tmp20;_tmp20=_tmp20->tl){
struct _tuple0*_tmp21=(struct _tuple0*)_tmp20->hd;struct _tuple0*_tmp22=_tmp21;void*_tmp25;struct Cyc_Set_Set*_tmp24;_LL1: _tmp25=_tmp22->f1;_tmp24=_tmp22->f2;_LL2:;
if(Cyc_Set_member(_tmp24,node))_tmp1F=({struct Cyc_List_List*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->hd=_tmp25,_tmp23->tl=_tmp1F;_tmp23;});}}
# 148
return _tmp1F;}struct _tuple1{struct Cyc_Set_Set*f1;struct Cyc_Dict_Dict f2;};
# 157
static struct _tuple1 Cyc_Graph_divideGraph(struct Cyc_Dict_Dict g,void*source){
# 160
struct Cyc_Set_Set**_tmp26=((struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(g,source);struct Cyc_Set_Set**_tmp27=_tmp26;struct Cyc_Set_Set*_tmp28;if(_tmp27 == 0){_LL1: _LL2:
 return({struct _tuple1 _tmp94;({struct Cyc_Set_Set*_tmpAC=Cyc_Set_empty(g.rel);_tmp94.f1=_tmpAC;}),_tmp94.f2=g;_tmp94;});}else{_LL3: _tmp28=*_tmp27;_LL4:
 return({struct _tuple1 _tmp95;_tmp95.f1=_tmp28,({struct Cyc_Dict_Dict _tmpAD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_delete)(g,source);_tmp95.f2=_tmpAD;});_tmp95;});}_LL0:;}
# 169
static struct Cyc_Dict_Dict Cyc_Graph_add_edgeTc(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*_tmp29=Cyc_Graph_get_targets(g,target);
struct _tuple1 _tmp2A=Cyc_Graph_divideGraph(g,source);struct _tuple1 _tmp2B=_tmp2A;struct Cyc_Set_Set*_tmp31;struct Cyc_Dict_Dict _tmp30;_LL1: _tmp31=_tmp2B.f1;_tmp30=_tmp2B.f2;_LL2:;{
struct Cyc_List_List*_tmp2C=Cyc_Graph_sourcesOf(_tmp30,source);
struct Cyc_Set_Set*_tmp2D=({struct Cyc_Set_Set*_tmpAE=Cyc_Set_union_two(_tmp31,_tmp29);Cyc_Set_insert(_tmpAE,target);});
struct Cyc_Dict_Dict _tmp2E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Set_Set*v))Cyc_Dict_insert)(_tmp30,source,_tmp2D);
{struct Cyc_List_List*_tmp2F=_tmp2C;for(0;(unsigned int)_tmp2F;_tmp2F=_tmp2F->tl){
_tmp2E=Cyc_Graph_add_edges(_tmp2E,_tmp2F->hd,_tmp2D);}}
# 178
return _tmp2E;};}
# 180
struct Cyc_Dict_Dict Cyc_Graph_tc_slow(struct Cyc_Dict_Dict g){
struct Cyc_Dict_Dict _tmp32=Cyc_Graph_empty(g.rel);
struct Cyc_List_List*_tmp33=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);
for(0;(unsigned int)_tmp33;_tmp33=_tmp33->tl){
struct _tuple0*_tmp34=(struct _tuple0*)_tmp33->hd;struct _tuple0*_tmp35=_tmp34;void*_tmp39;struct Cyc_Set_Set*_tmp38;_LL1: _tmp39=_tmp35->f1;_tmp38=_tmp35->f2;_LL2:;{
struct Cyc_List_List*_tmp36=Cyc_Graph_to_list(_tmp38);
if((unsigned int)_tmp36){
struct Cyc_List_List*_tmp37=_tmp36;for(0;(unsigned int)_tmp37;_tmp37=_tmp37->tl){
_tmp32=Cyc_Graph_add_edgeTc(_tmp32,_tmp39,_tmp37->hd);}}else{
_tmp32=Cyc_Graph_add_node(_tmp32,_tmp39);}};}
# 191
return _tmp32;}
# 198
struct Cyc_Dict_Dict Cyc_Graph_tkernel(struct Cyc_Dict_Dict g){
int(*_tmp3A)(void*,void*)=g.rel;
struct Cyc_Dict_Dict _tmp3B=Cyc_Graph_empty(_tmp3A);
struct Cyc_Dict_Dict _tmp3C=Cyc_Graph_empty(_tmp3A);
{struct Cyc_List_List*_tmp3D=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);for(0;(unsigned int)_tmp3D;_tmp3D=_tmp3D->tl){
struct _tuple0*_tmp3E=(struct _tuple0*)_tmp3D->hd;struct _tuple0*_tmp3F=_tmp3E;void*_tmp42;struct Cyc_Set_Set*_tmp41;_LL1: _tmp42=_tmp3F->f1;_tmp41=_tmp3F->f2;_LL2:;
while(!Cyc_Set_is_empty(_tmp41)){
void*_tmp40=Cyc_Set_choose(_tmp41);
_tmp41=Cyc_Set_delete(_tmp41,_tmp40);
if(Cyc_Graph_is_edge(_tmp3B,_tmp42,_tmp40))continue;
_tmp3B=Cyc_Graph_add_edgeTc(_tmp3B,_tmp42,_tmp40);
_tmp3C=Cyc_Graph_add_edge(_tmp3C,_tmp42,_tmp40);}}}
# 212
return _tmp3C;}struct Cyc_Graph_nodestate{void*root;int C;int visitindex;};struct Cyc_Graph_componentstate{struct Cyc_Set_Set*Succ;struct Cyc_Set_Set*nodes;};struct Cyc_Graph_tcstate{struct Cyc_Set_Set*visited;int visitindex;struct Cyc_Dict_Dict ns;struct Cyc_Dict_Dict cs;int Cindex;struct Cyc_List_List*nstack;struct Cyc_List_List*cstack;};
# 238
static void Cyc_Graph_comp_tc(struct Cyc_Graph_tcstate*ts,struct Cyc_Dict_Dict g,void*v){
# 240
int(*_tmp43)(void*,void*)=g.rel;
({struct Cyc_Set_Set*_tmpAF=Cyc_Set_insert(ts->visited,v);ts->visited=_tmpAF;});{
struct Cyc_Graph_nodestate*_tmp44=({struct Cyc_Graph_nodestate*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->root=v,_tmp58->C=0,_tmp58->visitindex=ts->visitindex ++;_tmp58;});
({struct Cyc_Dict_Dict _tmpB0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Graph_nodestate*v))Cyc_Dict_insert)(ts->ns,v,_tmp44);ts->ns=_tmpB0;});
({struct Cyc_List_List*_tmpB1=({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->hd=v,_tmp45->tl=ts->nstack;_tmp45;});ts->nstack=_tmpB1;});{
int _tmp46=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts->cstack);
struct Cyc_Set_Set*_tmp47=Cyc_Graph_get_targets(g,v);
while(!Cyc_Set_is_empty(_tmp47)){
void*_tmp48=Cyc_Set_choose(_tmp47);
_tmp47=Cyc_Set_delete(_tmp47,_tmp48);
if(_tmp43(v,_tmp48)== 0)continue;{
int is_forward_edge;
struct Cyc_Graph_nodestate*nsw;
if(Cyc_Set_member(ts->visited,_tmp48)){
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp48);
is_forward_edge=nsw->visitindex < _tmp44->visitindex?0: 1;}else{
# 258
is_forward_edge=0;
Cyc_Graph_comp_tc(ts,g,_tmp48);
nsw=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp48);}{
# 262
int _tmp49=nsw->C;
if(_tmp49 == 0){
struct Cyc_Graph_nodestate*_tmp4A=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp44->root);
struct Cyc_Graph_nodestate*_tmp4B=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,nsw->root);
# 267
if(_tmp4A->visitindex > _tmp4B->visitindex)
_tmp44->root=nsw->root;}else{
# 271
if(!is_forward_edge)
({struct Cyc_List_List*_tmpB2=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=(void*)_tmp49,_tmp4C->tl=ts->cstack;_tmp4C;});ts->cstack=_tmpB2;});}};};}
# 274
if(_tmp43(_tmp44->root,v)!= 0)
return;{
# 277
int _tmp4D=ts->Cindex ++;
struct Cyc_Graph_componentstate*_tmp4E=({struct Cyc_Graph_componentstate*_tmp57=_cycalloc(sizeof(*_tmp57));({struct Cyc_Set_Set*_tmpB4=((struct Cyc_Set_Set*(*)(int(*cmp)(int,int)))Cyc_Set_empty)(Cyc_Core_intcmp);_tmp57->Succ=_tmpB4;}),({
struct Cyc_Set_Set*_tmpB3=Cyc_Set_empty(_tmp43);_tmp57->nodes=_tmpB3;});_tmp57;});
({struct Cyc_Dict_Dict _tmpB5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,int k,struct Cyc_Graph_componentstate*v))Cyc_Dict_insert)(ts->cs,_tmp4D,_tmp4E);ts->cs=_tmpB5;});
{struct Cyc_List_List*_tmp4F=ts->nstack;struct Cyc_List_List*_tmp50=_tmp4F;void*_tmp51;if(_tmp50 == 0){_LL1: _LL2:
 goto _LL0;}else{_LL3: _tmp51=(void*)_tmp50->hd;_LL4:
# 284
 if(_tmp43(_tmp51,v)!= 0  || 
Cyc_Graph_is_edge(g,v,v))
({struct Cyc_Set_Set*_tmpB6=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_insert)(_tmp4E->Succ,_tmp4D);_tmp4E->Succ=_tmpB6;});
# 288
goto _LL0;}_LL0:;}
# 290
{int _tmp52=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts->cstack);for(0;_tmp52 > _tmp46;-- _tmp52){
int _tmp53=(int)((struct Cyc_List_List*)_check_null(ts->cstack))->hd;
ts->cstack=((struct Cyc_List_List*)_check_null(ts->cstack))->tl;
if(!((int(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_member)(_tmp4E->Succ,_tmp53)){
struct Cyc_Set_Set*_tmp54=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_insert)(_tmp4E->Succ,_tmp53);
_tmp54=({struct Cyc_Set_Set*_tmpB7=_tmp54;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmpB7,(((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(ts->cs,_tmp53))->Succ);});
_tmp4E->Succ=_tmp54;}}}
# 299
while((unsigned int)ts->nstack){
void*_tmp55=((struct Cyc_List_List*)_check_null(ts->nstack))->hd;
ts->nstack=((struct Cyc_List_List*)_check_null(ts->nstack))->tl;{
struct Cyc_Graph_nodestate*_tmp56=((struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(ts->ns,_tmp55);
_tmp56->C=_tmp4D;
({struct Cyc_Set_Set*_tmpB8=Cyc_Set_insert(_tmp4E->nodes,_tmp55);_tmp4E->nodes=_tmpB8;});
if(_tmp43(_tmp55,v)== 0)break;};}};};};}
# 308
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g){
int(*_tmp59)(void*,void*)=g.rel;
struct Cyc_Graph_tcstate*_tmp5A=({struct Cyc_Graph_tcstate*_tmp68=_cycalloc(sizeof(*_tmp68));
({
struct Cyc_Set_Set*_tmpBB=Cyc_Set_empty(_tmp59);_tmp68->visited=_tmpBB;}),_tmp68->visitindex=1,({
# 314
struct Cyc_Dict_Dict _tmpBA=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp59);_tmp68->ns=_tmpBA;}),({
struct Cyc_Dict_Dict _tmpB9=((struct Cyc_Dict_Dict(*)(int(*cmp)(int,int)))Cyc_Dict_empty)(Cyc_Core_intcmp);_tmp68->cs=_tmpB9;}),_tmp68->Cindex=1,_tmp68->nstack=0,_tmp68->cstack=0;_tmp68;});
# 320
{struct Cyc_List_List*_tmp5B=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(g);for(0;(unsigned int)_tmp5B;_tmp5B=_tmp5B->tl){
struct _tuple0*_tmp5C=(struct _tuple0*)_tmp5B->hd;struct _tuple0*_tmp5D=_tmp5C;void*_tmp5F;struct Cyc_Set_Set*_tmp5E;_LL1: _tmp5F=_tmp5D->f1;_tmp5E=_tmp5D->f2;_LL2:;
if(!Cyc_Set_member(_tmp5A->visited,_tmp5F))Cyc_Graph_comp_tc(_tmp5A,g,_tmp5F);}}{
# 325
struct Cyc_Dict_Dict _tmp60=Cyc_Graph_empty(_tmp59);
{int _tmp61=1;for(0;_tmp61 < _tmp5A->Cindex;++ _tmp61){
struct Cyc_Graph_componentstate*_tmp62=((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(_tmp5A->cs,_tmp61);
struct Cyc_Set_Set*_tmp63=Cyc_Set_empty(_tmp59);
struct Cyc_Set_Set*_tmp64=_tmp62->Succ;
while(!((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp64)){
int _tmp65=((int(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp64);
_tmp64=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,int elt))Cyc_Set_delete)(_tmp64,_tmp65);
_tmp63=({struct Cyc_Set_Set*_tmpBC=_tmp63;Cyc_Set_union_two(_tmpBC,(
((struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict d,int k))Cyc_Dict_lookup)(_tmp5A->cs,_tmp65))->nodes);});}{
# 336
struct Cyc_Set_Set*_tmp66=_tmp62->nodes;
while(!Cyc_Set_is_empty(_tmp66)){
void*_tmp67=Cyc_Set_choose(_tmp66);
_tmp66=Cyc_Set_delete(_tmp66,_tmp67);
_tmp60=Cyc_Graph_add_edges(_tmp60,_tmp67,_tmp63);}};}}
# 343
return _tmp60;};}
# 356 "graph.cyc"
static void Cyc_Graph_traverse(struct Cyc_Dict_Dict input,struct Cyc_Dict_Dict*output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned int k){
# 360
({struct Cyc_List_List*_tmpBD=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=x,_tmp69->tl=*S;_tmp69;});*S=_tmpBD;});
({struct Cyc_Dict_Dict _tmpBE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,k);*N=_tmpBE;});
{struct Cyc_Set_Set*_tmp6A=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(_tmp6A);0){
void*_tmp6B=Cyc_Set_choose(_tmp6A);
_tmp6A=Cyc_Set_delete(_tmp6A,_tmp6B);
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp6B)== (unsigned int)0)
Cyc_Graph_traverse(input,output,N,S,_tmp6B,k + (unsigned int)1);{
unsigned int _tmp6C=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp6B);
if(({unsigned int _tmpBF=_tmp6C;_tmpBF < ((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpC0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,_tmp6C);*N=_tmpC0;});};}}
# 371
if(({unsigned int _tmpC1=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);_tmpC1 == k;})){
int(*_tmp6D)(void*,void*)=(*output).rel;
struct Cyc_Set_Set*_tmp6E=Cyc_Set_empty(_tmp6D);
{struct Cyc_List_List*_tmp6F=*S;for(0;(unsigned int)_tmp6F;_tmp6F=_tmp6F->tl){
void*_tmp70=_tmp6F->hd;
_tmp6E=Cyc_Set_insert(_tmp6E,_tmp70);
if(_tmp6D(_tmp70,x)== 0)break;}}
# 379
for(0;(unsigned int)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp71=((struct Cyc_List_List*)_check_null(*S))->hd;
({struct Cyc_Dict_Dict _tmpC2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,_tmp71,- 1U);*N=_tmpC2;});
({struct Cyc_Dict_Dict _tmpC3=Cyc_Graph_add_node(*output,_tmp71);*output=_tmpC3;});
({struct Cyc_Dict_Dict _tmpC4=Cyc_Graph_add_edges(*output,_tmp71,_tmp6E);*output=_tmpC4;});
if(_tmp6D(_tmp71,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 391
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict input){
int(*_tmp72)(void*,void*)=input.rel;
struct Cyc_List_List*_tmp73=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(input);
struct Cyc_Dict_Dict _tmp74=Cyc_Graph_empty(_tmp72);
struct Cyc_List_List*_tmp75=0;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp72);
{struct Cyc_List_List*_tmp76=_tmp73;for(0;(unsigned int)_tmp76;_tmp76=_tmp76->tl){
struct _tuple0*_tmp77=(struct _tuple0*)_tmp76->hd;struct _tuple0*_tmp78=_tmp77;void*_tmp79;_LL1: _tmp79=_tmp78->f1;_LL2:;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(N,_tmp79,0U);}}
# 401
{struct Cyc_List_List*_tmp7A=_tmp73;for(0;(unsigned int)_tmp7A;_tmp7A=_tmp7A->tl){
struct _tuple0*_tmp7B=(struct _tuple0*)_tmp7A->hd;struct _tuple0*_tmp7C=_tmp7B;void*_tmp7D;_LL4: _tmp7D=_tmp7C->f1;_LL5:;
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(N,_tmp7D)== (unsigned int)0)
Cyc_Graph_traverse(input,& _tmp74,& N,& _tmp75,_tmp7D,1U);}}
# 406
return _tmp74;}
# 410
static void Cyc_Graph_tsort0(struct Cyc_Dict_Dict input,struct Cyc_List_List**output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned int k){
# 414
({struct Cyc_List_List*_tmpC5=({struct Cyc_List_List*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->hd=x,_tmp7E->tl=*S;_tmp7E;});*S=_tmpC5;});
({struct Cyc_Dict_Dict _tmpC6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,k);*N=_tmpC6;});
{struct Cyc_Set_Set*_tmp7F=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(_tmp7F);0){
void*_tmp80=Cyc_Set_choose(_tmp7F);
_tmp7F=Cyc_Set_delete(_tmp7F,_tmp80);
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp80)== (unsigned int)0)
Cyc_Graph_tsort0(input,output,N,S,_tmp80,k + (unsigned int)1);{
unsigned int _tmp81=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,_tmp80);
if(({unsigned int _tmpC7=_tmp81;_tmpC7 < ((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);}))
({struct Cyc_Dict_Dict _tmpC8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,x,_tmp81);*N=_tmpC8;});};}}
# 425
if(({unsigned int _tmpC9=((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(*N,x);_tmpC9 == k;})){
int(*_tmp82)(void*,void*)=input.rel;
struct Cyc_Set_Set*_tmp83=Cyc_Set_empty(_tmp82);
{struct Cyc_List_List*_tmp84=*S;for(0;(unsigned int)_tmp84;_tmp84=_tmp84->tl){
void*_tmp85=_tmp84->hd;
_tmp83=Cyc_Set_insert(_tmp83,_tmp85);
if(_tmp82(_tmp85,x)== 0)break;}}
# 433
for(0;(unsigned int)*S;*S=((struct Cyc_List_List*)_check_null(*S))->tl){
void*_tmp86=((struct Cyc_List_List*)_check_null(*S))->hd;
({struct Cyc_Dict_Dict _tmpCA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*N,_tmp86,- 1U);*N=_tmpCA;});
({struct Cyc_List_List*_tmpCB=({struct Cyc_List_List*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->hd=_tmp86,_tmp87->tl=*output;_tmp87;});*output=_tmpCB;});
if(_tmp82(_tmp86,x)== 0){
*S=((struct Cyc_List_List*)_check_null(*S))->tl;
break;}}}}
# 444
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict input){
int(*_tmp88)(void*,void*)=input.rel;
struct Cyc_List_List*_tmp89=((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d))Cyc_Dict_to_list)(input);
struct Cyc_List_List*_tmp8A=0;
struct Cyc_List_List*_tmp8B=0;
struct Cyc_Dict_Dict N=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(_tmp88);
{struct Cyc_List_List*_tmp8C=_tmp89;for(0;(unsigned int)_tmp8C;_tmp8C=_tmp8C->tl){
struct _tuple0*_tmp8D=(struct _tuple0*)_tmp8C->hd;struct _tuple0*_tmp8E=_tmp8D;void*_tmp8F;_LL1: _tmp8F=_tmp8E->f1;_LL2:;
N=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(N,_tmp8F,0U);}}
# 454
{struct Cyc_List_List*_tmp90=_tmp89;for(0;(unsigned int)_tmp90;_tmp90=_tmp90->tl){
struct _tuple0*_tmp91=(struct _tuple0*)_tmp90->hd;struct _tuple0*_tmp92=_tmp91;void*_tmp93;_LL4: _tmp93=_tmp92->f1;_LL5:;
if(((unsigned int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(N,_tmp93)== (unsigned int)0)
Cyc_Graph_tsort0(input,& _tmp8A,& N,& _tmp8B,_tmp93,1U);}}
# 459
return _tmp8A;}
