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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 994 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1068
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned loc);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
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
void*Cyc_Warn_impos_loc(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 328 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Hashtable_Table;
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple1*Cyc_Toc_temp_var();extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_finish();
# 27 "toseqc.h"
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*decls);
# 35 "toseqc.cyc"
enum Cyc_Toseqc_SideEffect{Cyc_Toseqc_Const =0U,Cyc_Toseqc_NoEffect =1U,Cyc_Toseqc_ExnEffect =2U,Cyc_Toseqc_AnyEffect =3U};
# 39
static struct _dyneither_ptr Cyc_Toseqc_eff_to_string(enum Cyc_Toseqc_SideEffect e){
enum Cyc_Toseqc_SideEffect _tmp0=e;switch(_tmp0){case Cyc_Toseqc_Const: _LL1: _LL2:
 return({const char*_tmp1="Const";_tag_dyneither(_tmp1,sizeof(char),6U);});case Cyc_Toseqc_NoEffect: _LL3: _LL4:
 return({const char*_tmp2="NoEffect";_tag_dyneither(_tmp2,sizeof(char),9U);});case Cyc_Toseqc_ExnEffect: _LL5: _LL6:
 return({const char*_tmp3="ExnEffect";_tag_dyneither(_tmp3,sizeof(char),10U);});default: _LL7: _LL8:
 return({const char*_tmp4="AnyEffect";_tag_dyneither(_tmp4,sizeof(char),10U);});}_LL0:;}struct _tuple11{enum Cyc_Toseqc_SideEffect f1;enum Cyc_Toseqc_SideEffect f2;};
# 48
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_join_side_effect(enum Cyc_Toseqc_SideEffect e1,enum Cyc_Toseqc_SideEffect e2){
struct _tuple11 _tmp5=({struct _tuple11 _tmpD4;_tmpD4.f1=e1,_tmpD4.f2=e2;_tmpD4;});struct _tuple11 _tmp6=_tmp5;if(_tmp6.f1 == Cyc_Toseqc_AnyEffect){_LL1: _LL2:
 return Cyc_Toseqc_AnyEffect;}else{if(_tmp6.f2 == Cyc_Toseqc_AnyEffect){_LL3: _LL4:
 return Cyc_Toseqc_AnyEffect;}else{if(_tmp6.f1 == Cyc_Toseqc_ExnEffect){if(_tmp6.f2 == Cyc_Toseqc_ExnEffect){_LL5: _LL6:
# 53
 return Cyc_Toseqc_AnyEffect;}else{_LL7: _LL8:
 return Cyc_Toseqc_ExnEffect;}}else{if(_tmp6.f2 == Cyc_Toseqc_ExnEffect){_LL9: _LLA:
 return Cyc_Toseqc_ExnEffect;}else{if(_tmp6.f1 == Cyc_Toseqc_NoEffect){_LLB: _LLC:
 return Cyc_Toseqc_NoEffect;}else{if(_tmp6.f2 == Cyc_Toseqc_NoEffect){_LLD: _LLE:
 return Cyc_Toseqc_NoEffect;}else{if(_tmp6.f1 == Cyc_Toseqc_Const){if(_tmp6.f2 == Cyc_Toseqc_Const){_LLF: _LL10:
 return Cyc_Toseqc_Const;}else{goto _LL11;}}else{_LL11: _LL12:
({void*_tmp7=0U;({struct _dyneither_ptr _tmpDB=({const char*_tmp8="join_side_effect";_tag_dyneither(_tmp8,sizeof(char),17U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmpDB,_tag_dyneither(_tmp7,sizeof(void*),0U));});});}}}}}}}_LL0:;}static char _tmp9[20U]="_get_dyneither_size";static char _tmpA[24U]="_get_zero_arr_size_char";static char _tmpB[25U]="_get_zero_arr_size_short";static char _tmpC[23U]="_get_zero_arr_size_int";static char _tmpD[25U]="_get_zero_arr_size_float";static char _tmpE[26U]="_get_zero_arr_size_double";static char _tmpF[30U]="_get_zero_arr_size_longdouble";static char _tmp10[28U]="_get_zero_arr_size_voidstar";
# 64
static struct _dyneither_ptr Cyc_Toseqc_pure_funs[8U]={{_tmp9,_tmp9,_tmp9 + 20U},{_tmpA,_tmpA,_tmpA + 24U},{_tmpB,_tmpB,_tmpB + 25U},{_tmpC,_tmpC,_tmpC + 23U},{_tmpD,_tmpD,_tmpD + 25U},{_tmpE,_tmpE,_tmpE + 26U},{_tmpF,_tmpF,_tmpF + 30U},{_tmp10,_tmp10,_tmp10 + 28U}};static char _tmp11[12U]="_check_null";static char _tmp12[28U]="_check_known_subscript_null";static char _tmp13[31U]="_check_known_subscript_notnull";static char _tmp14[27U]="_check_dyneither_subscript";static char _tmp15[21U]="_untag_dyneither_ptr";static char _tmp16[15U]="_region_malloc";
# 75
static struct _dyneither_ptr Cyc_Toseqc_exn_effect_funs[6U]={{_tmp11,_tmp11,_tmp11 + 12U},{_tmp12,_tmp12,_tmp12 + 28U},{_tmp13,_tmp13,_tmp13 + 31U},{_tmp14,_tmp14,_tmp14 + 27U},{_tmp15,_tmp15,_tmp15 + 21U},{_tmp16,_tmp16,_tmp16 + 15U}};
# 83
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_fun_effect(struct _dyneither_ptr fn){
{int i=0;for(0;(unsigned)i < 8U;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)Cyc_Toseqc_pure_funs[i],(struct _dyneither_ptr)fn))return Cyc_Toseqc_NoEffect;}}
# 87
{int i=0;for(0;(unsigned)i < 6U;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)Cyc_Toseqc_exn_effect_funs[i],(struct _dyneither_ptr)fn))return Cyc_Toseqc_ExnEffect;}}
# 90
return Cyc_Toseqc_AnyEffect;}
# 93
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exps_effect(struct Cyc_List_List*es){
enum Cyc_Toseqc_SideEffect res=0U;
# 99
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
enum Cyc_Toseqc_SideEffect _tmp17=Cyc_Toseqc_exp_effect((struct Cyc_Absyn_Exp*)x->hd);
res=Cyc_Toseqc_join_side_effect(_tmp17,res);
# 104
if((int)res == (int)3U)
# 106
return res;}}
# 110
return res;}
# 114
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmp4F="";_tag_dyneither(_tmp4F,sizeof(char),1U);});
{void*_tmp18=e->r;void*_tmp19=_tmp18;struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Exp*_tmp49;struct Cyc_Absyn_Exp*_tmp48;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_Absyn_Exp*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Exp*_tmp3A;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp34;struct Cyc_Absyn_Exp*_tmp33;switch(*((int*)_tmp19)){case 0U: _LL1: _LL2:
 goto _LL4;case 18U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 goto _LLA;case 33U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 return Cyc_Toseqc_Const;case 1U: _LLD: _LLE:
 return Cyc_Toseqc_NoEffect;case 4U: _LLF: _LL10:
# 125
 goto _LL12;case 40U: _LL11: _LL12:
 return Cyc_Toseqc_AnyEffect;case 10U: _LL13: _tmp33=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL14: {
# 129
void*_tmp1A=_tmp33->r;void*_tmp1B=_tmp1A;struct _tuple1*_tmp1E;struct Cyc_Absyn_Vardecl*_tmp1D;struct Cyc_Absyn_Fndecl*_tmp1C;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1B)->tag == 1U)switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1B)->f1)){case 2U: _LL56: _tmp1C=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1B)->f1)->f1;_LL57:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1C->name));case 1U: _LL58: _tmp1D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1B)->f1)->f1;_LL59:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1D->name));case 0U: _LL5A: _tmp1E=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1B)->f1)->f1;_LL5B:
 return Cyc_Toseqc_fun_effect(Cyc_Absynpp_qvar2string(_tmp1E));default: goto _LL5C;}else{_LL5C: _LL5D:
# 134
 return Cyc_Toseqc_AnyEffect;}_LL55:;}case 36U: _LL15: _tmp34=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_LL16: {
# 138
struct _tuple0 _tmp1F=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp34);struct _tuple0 _tmp20=_tmp1F;struct Cyc_List_List*_tmp21;_LL5F: _tmp21=_tmp20.f2;_LL60:;
_tmp35=_tmp21;goto _LL18;}case 3U: _LL17: _tmp35=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_LL18:
# 142
 return Cyc_Toseqc_exps_effect(_tmp35);case 6U: _LL19: _tmp38=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_tmp37=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_tmp36=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp19)->f3;_LL1A:
# 145
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp22[3U];_tmp22[0]=_tmp38,_tmp22[1]=_tmp37,_tmp22[2]=_tmp36;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22,sizeof(struct Cyc_Absyn_Exp*),3U));}));case 23U: _LL1B: _tmp3A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_tmp39=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_LL1C:
# 147
 _tmp3C=_tmp3A;_tmp3B=_tmp39;goto _LL1E;case 7U: _LL1D: _tmp3C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_tmp3B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_LL1E:
 _tmp3E=_tmp3C;_tmp3D=_tmp3B;goto _LL20;case 8U: _LL1F: _tmp3E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_tmp3D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_LL20:
 _tmp40=_tmp3E;_tmp3F=_tmp3D;goto _LL22;case 9U: _LL21: _tmp40=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_tmp3F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_LL22:
# 151
 return Cyc_Toseqc_exps_effect(({struct Cyc_Absyn_Exp*_tmp23[2U];_tmp23[0]=_tmp40,_tmp23[1]=_tmp3F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp23,sizeof(struct Cyc_Absyn_Exp*),2U));}));case 41U: _LL23: _tmp41=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL24:
# 153
 _tmp42=_tmp41;goto _LL26;case 12U: _LL25: _tmp42=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL26:
 _tmp43=_tmp42;goto _LL28;case 13U: _LL27: _tmp43=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL28:
 _tmp44=_tmp43;goto _LL2A;case 14U: _LL29: _tmp44=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp19)->f2;_LL2A:
 _tmp45=_tmp44;goto _LL2C;case 15U: _LL2B: _tmp45=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL2C:
 _tmp46=_tmp45;goto _LL2E;case 20U: _LL2D: _tmp46=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL2E:
 _tmp47=_tmp46;goto _LL30;case 21U: _LL2F: _tmp47=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL30:
 _tmp48=_tmp47;goto _LL32;case 22U: _LL31: _tmp48=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL32:
 _tmp49=_tmp48;goto _LL34;case 5U: _LL33: _tmp49=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL34:
# 162
 return Cyc_Toseqc_exp_effect(_tmp49);case 37U: _LL35: _tmp4A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp19)->f1;_LL36:
# 165
 return Cyc_Toseqc_stmt_effect(_tmp4A);case 2U: _LL37: _LL38:
# 167
 bad_form=({const char*_tmp24="Pragma_e";_tag_dyneither(_tmp24,sizeof(char),9U);});goto _LL0;case 11U: _LL39: _LL3A:
 bad_form=({const char*_tmp25="Throw_e";_tag_dyneither(_tmp25,sizeof(char),8U);});goto _LL0;case 16U: _LL3B: _LL3C:
 bad_form=({const char*_tmp26="New_e";_tag_dyneither(_tmp26,sizeof(char),6U);});goto _LL0;case 24U: _LL3D: _LL3E:
 bad_form=({const char*_tmp27="Tuple_e";_tag_dyneither(_tmp27,sizeof(char),8U);});goto _LL0;case 25U: _LL3F: _LL40:
 bad_form=({const char*_tmp28="CompoundLit_e";_tag_dyneither(_tmp28,sizeof(char),14U);});goto _LL0;case 26U: _LL41: _LL42:
 bad_form=({const char*_tmp29="Array_e";_tag_dyneither(_tmp29,sizeof(char),8U);});goto _LL0;case 27U: _LL43: _LL44:
 bad_form=({const char*_tmp2A="Comprehension_e";_tag_dyneither(_tmp2A,sizeof(char),16U);});goto _LL0;case 28U: _LL45: _LL46:
 bad_form=({const char*_tmp2B="ComprehensionNoinit_e";_tag_dyneither(_tmp2B,sizeof(char),22U);});goto _LL0;case 29U: _LL47: _LL48:
 bad_form=({const char*_tmp2C="Aggregate_e";_tag_dyneither(_tmp2C,sizeof(char),12U);});goto _LL0;case 30U: _LL49: _LL4A:
 bad_form=({const char*_tmp2D="AnonStruct_e";_tag_dyneither(_tmp2D,sizeof(char),13U);});goto _LL0;case 31U: _LL4B: _LL4C:
 bad_form=({const char*_tmp2E="Datatype_e";_tag_dyneither(_tmp2E,sizeof(char),11U);});goto _LL0;case 34U: _LL4D: _LL4E:
 bad_form=({const char*_tmp2F="Malloc_e";_tag_dyneither(_tmp2F,sizeof(char),9U);});goto _LL0;case 35U: _LL4F: _LL50:
 bad_form=({const char*_tmp30="Swap_e";_tag_dyneither(_tmp30,sizeof(char),7U);});goto _LL0;case 38U: _LL51: _LL52:
 bad_form=({const char*_tmp31="Tagcheck_e";_tag_dyneither(_tmp31,sizeof(char),11U);});goto _LL0;default: _LL53: _LL54:
 bad_form=({const char*_tmp32="Valueof_e";_tag_dyneither(_tmp32,sizeof(char),10U);});goto _LL0;}_LL0:;}
# 183
({struct Cyc_String_pa_PrintArg_struct _tmp4D=({struct Cyc_String_pa_PrintArg_struct _tmpD6;_tmpD6.tag=0U,_tmpD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);_tmpD6;});struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmpD5;_tmpD5.tag=0U,({
struct _dyneither_ptr _tmpDC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpD5.f1=_tmpDC;});_tmpD5;});void*_tmp4B[2U];_tmp4B[0]=& _tmp4D,_tmp4B[1]=& _tmp4E;({struct _dyneither_ptr _tmpDD=({const char*_tmp4C="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmp4C,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpDD,_tag_dyneither(_tmp4B,sizeof(void*),2U));});});}
# 186
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s){
# 188
enum Cyc_Toseqc_SideEffect res=0U;
while(1){
void*_tmp50=s->r;void*_tmp51=_tmp50;struct Cyc_Absyn_Stmt*_tmp57;struct Cyc_Absyn_Stmt*_tmp56;struct Cyc_Absyn_Stmt*_tmp55;struct Cyc_Absyn_Exp*_tmp54;switch(*((int*)_tmp51)){case 0U: _LL1: _LL2:
 return res;case 1U: _LL3: _tmp54=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp51)->f1;_LL4:
 return({enum Cyc_Toseqc_SideEffect _tmpDE=res;Cyc_Toseqc_join_side_effect(_tmpDE,Cyc_Toseqc_exp_effect(_tmp54));});case 13U: _LL5: _tmp55=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp51)->f2;_LL6:
# 195
 s=_tmp55;
continue;case 2U: _LL7: _tmp57=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp51)->f1;_tmp56=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp51)->f2;_LL8:
# 198
 res=({enum Cyc_Toseqc_SideEffect _tmpDF=res;Cyc_Toseqc_join_side_effect(_tmpDF,Cyc_Toseqc_stmt_effect(_tmp57));});
s=_tmp56;
continue;case 6U: _LL9: _LLA:
# 202
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 8U: _LLD: _LLE:
 goto _LL10;case 3U: _LLF: _LL10:
 goto _LL12;case 4U: _LL11: _LL12:
 goto _LL14;case 5U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 14U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 10U: _LL1B: _LL1C:
 return Cyc_Toseqc_AnyEffect;default: _LL1D: _LL1E:
({void*_tmp52=0U;({struct _dyneither_ptr _tmpE0=({const char*_tmp53="bad stmt after xlation to C";_tag_dyneither(_tmp53,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpE0,_tag_dyneither(_tmp52,sizeof(void*),0U));});});}_LL0:;}}
# 219
static int Cyc_Toseqc_is_toc_var(struct Cyc_Absyn_Exp*e){
void*_tmp58=e->r;void*_tmp59=_tmp58;struct _tuple1*_tmp5A;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59)->f1)->tag == 0U){_LL1: _tmp5A=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59)->f1)->f1;_LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 226
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*s);
# 244 "toseqc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toseqc_exps_to_seqc(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
# 246
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toseqc_exp_to_seqc,es);
# 249
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)<= 1 ||(int)Cyc_Toseqc_exps_effect(es)!= (int)Cyc_Toseqc_AnyEffect)return 0;{
# 253
struct Cyc_Absyn_Stmt*stmt=({struct Cyc_Absyn_Exp*_tmpE1=Cyc_Absyn_copy_exp(e);Cyc_Absyn_exp_stmt(_tmpE1,e->loc);});
struct Cyc_Absyn_Stmt*laststmt=stmt;
int did_skip=0;
int did_convert=0;
{struct Cyc_List_List*x=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(es);for(0;x != 0;x=x->tl){
struct Cyc_Absyn_Exp*_tmp5B=(struct Cyc_Absyn_Exp*)x->hd;
# 261
if(Cyc_Tcutil_is_const_exp(_tmp5B)|| Cyc_Toseqc_is_toc_var(_tmp5B))continue;
# 266
if(!did_skip){
did_skip=1;
continue;}
# 270
did_convert=1;{
# 273
struct _tuple1*_tmp5C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5D=Cyc_Absyn_var_exp(_tmp5C,_tmp5B->loc);
# 277
struct Cyc_Absyn_Exp*_tmp5E=Cyc_Absyn_new_exp(_tmp5B->r,_tmp5B->loc);
_tmp5E->annot=_tmp5B->annot;
_tmp5E->topt=_tmp5B->topt;{
# 281
void*t=_tmp5E->topt == 0?(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=11U,_tmp5F->f1=_tmp5E;_tmp5F;}): Cyc_Toc_typ_to_c((void*)_check_null(_tmp5E->topt));
# 285
stmt=Cyc_Absyn_declare_stmt(_tmp5C,t,_tmp5E,stmt,e->loc);
# 288
_tmp5B->r=_tmp5D->r;};};}}
# 292
if(did_convert){
({void*_tmpE2=(Cyc_Absyn_stmt_exp(stmt,e->loc))->r;e->r=_tmpE2;});
return laststmt;}else{
# 296
return 0;}};}
# 302
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmp9A="";_tag_dyneither(_tmp9A,sizeof(char),1U);});
{void*_tmp60=e->r;void*_tmp61=_tmp60;struct Cyc_Absyn_Stmt*_tmp95;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_Absyn_Exp*_tmp81;struct Cyc_Absyn_Exp*_tmp80;struct Cyc_Core_Opt*_tmp7F;struct Cyc_Absyn_Exp*_tmp7E;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_List_List*_tmp7B;struct Cyc_Absyn_Exp*_tmp7A;struct Cyc_List_List*_tmp79;switch(*((int*)_tmp61)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _LL4:
 return;case 10U: _LL5: _tmp7A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_tmp79=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LL6:
# 308
 _tmp7B=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=_tmp7A,_tmp62->tl=_tmp79;_tmp62;});goto _LL8;case 3U: _LL7: _tmp7B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LL8:
# 310
 Cyc_Toseqc_exps_to_seqc(e,_tmp7B);
return;case 23U: _LL9: _tmp7D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_tmp7C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LLA:
# 314
({struct Cyc_Absyn_Exp*_tmpE3=e;Cyc_Toseqc_exps_to_seqc(_tmpE3,({struct Cyc_Absyn_Exp*_tmp63[2U];_tmp63[0]=_tmp7D,_tmp63[1]=_tmp7C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp63,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
return;case 4U: _LLB: _tmp80=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_tmp7F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_tmp7E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp61)->f3;_LLC:
# 317
 if(_tmp7F == 0){
void*_tmp64=_tmp80->r;void*_tmp65=_tmp64;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp65)->tag == 1U){_LL56: _LL57:
 Cyc_Toseqc_exp_to_seqc(_tmp7E);return;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 323
({struct Cyc_Absyn_Exp*_tmpE4=e;Cyc_Toseqc_exps_to_seqc(_tmpE4,({struct Cyc_Absyn_Exp*_tmp66[2U];_tmp66[0]=_tmp7E,_tmp66[1]=_tmp80;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp66,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
return;case 6U: _LLD: _tmp83=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_tmp82=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_tmp81=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp61)->f3;_LLE:
# 327
 Cyc_Toseqc_exp_to_seqc(_tmp81);_tmp85=_tmp83;_tmp84=_tmp82;goto _LL10;case 7U: _LLF: _tmp85=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_tmp84=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LL10:
 _tmp87=_tmp85;_tmp86=_tmp84;goto _LL12;case 8U: _LL11: _tmp87=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_tmp86=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LL12:
 _tmp89=_tmp87;_tmp88=_tmp86;goto _LL14;case 9U: _LL13: _tmp89=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_tmp88=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LL14:
# 331
 Cyc_Toseqc_exp_to_seqc(_tmp88);_tmp8A=_tmp89;goto _LL16;case 41U: _LL15: _tmp8A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL16:
# 333
 _tmp8B=_tmp8A;goto _LL18;case 12U: _LL17: _tmp8B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL18:
 _tmp8C=_tmp8B;goto _LL1A;case 13U: _LL19: _tmp8C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL1A:
 _tmp8D=_tmp8C;goto _LL1C;case 14U: _LL1B: _tmp8D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LL1C:
 _tmp8E=_tmp8D;goto _LL1E;case 15U: _LL1D: _tmp8E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL1E:
 _tmp8F=_tmp8E;goto _LL20;case 18U: _LL1F: _tmp8F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL20:
 _tmp90=_tmp8F;goto _LL22;case 20U: _LL21: _tmp90=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL22:
 _tmp91=_tmp90;goto _LL24;case 21U: _LL23: _tmp91=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL24:
 _tmp92=_tmp91;goto _LL26;case 22U: _LL25: _tmp92=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL26:
 _tmp93=_tmp92;goto _LL28;case 5U: _LL27: _tmp93=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL28:
# 343
 Cyc_Toseqc_exp_to_seqc(_tmp93);
return;case 17U: _LL29: _LL2A:
# 346
 goto _LL2C;case 19U: _LL2B: _LL2C:
 goto _LL2E;case 33U: _LL2D: _LL2E:
 goto _LL30;case 32U: _LL2F: _LL30:
 return;case 36U: _LL31: _tmp94=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp61)->f2;_LL32: {
# 352
struct _tuple0 _tmp67=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp94);struct _tuple0 _tmp68=_tmp67;struct Cyc_List_List*_tmp69;_LL5B: _tmp69=_tmp68.f2;_LL5C:;
Cyc_Toseqc_exps_to_seqc(e,_tmp69);
return;}case 37U: _LL33: _tmp95=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp61)->f1;_LL34:
# 357
 Cyc_Toseqc_stmt_to_seqc(_tmp95);
return;case 40U: _LL35: _LL36:
# 360
 return;case 2U: _LL37: _LL38:
# 362
 bad_form=({const char*_tmp6A="Pragma_e";_tag_dyneither(_tmp6A,sizeof(char),9U);});goto _LL0;case 11U: _LL39: _LL3A:
 bad_form=({const char*_tmp6B="Throw_e";_tag_dyneither(_tmp6B,sizeof(char),8U);});goto _LL0;case 16U: _LL3B: _LL3C:
 bad_form=({const char*_tmp6C="New_e";_tag_dyneither(_tmp6C,sizeof(char),6U);});goto _LL0;case 24U: _LL3D: _LL3E:
 bad_form=({const char*_tmp6D="Tuple_e";_tag_dyneither(_tmp6D,sizeof(char),8U);});goto _LL0;case 25U: _LL3F: _LL40:
 bad_form=({const char*_tmp6E="CompoundLit_e";_tag_dyneither(_tmp6E,sizeof(char),14U);});goto _LL0;case 26U: _LL41: _LL42:
 bad_form=({const char*_tmp6F="Array_e";_tag_dyneither(_tmp6F,sizeof(char),8U);});goto _LL0;case 27U: _LL43: _LL44:
 bad_form=({const char*_tmp70="Comprehension_e";_tag_dyneither(_tmp70,sizeof(char),16U);});goto _LL0;case 28U: _LL45: _LL46:
 bad_form=({const char*_tmp71="ComprehensionNoinit_e";_tag_dyneither(_tmp71,sizeof(char),22U);});goto _LL0;case 29U: _LL47: _LL48:
 bad_form=({const char*_tmp72="Aggregate_e";_tag_dyneither(_tmp72,sizeof(char),12U);});goto _LL0;case 30U: _LL49: _LL4A:
 bad_form=({const char*_tmp73="AnonStruct_e";_tag_dyneither(_tmp73,sizeof(char),13U);});goto _LL0;case 31U: _LL4B: _LL4C:
# 373
 bad_form=({const char*_tmp74="Datatype_e";_tag_dyneither(_tmp74,sizeof(char),11U);});goto _LL0;case 34U: _LL4D: _LL4E:
 bad_form=({const char*_tmp75="Malloc_e";_tag_dyneither(_tmp75,sizeof(char),9U);});goto _LL0;case 35U: _LL4F: _LL50:
 bad_form=({const char*_tmp76="Swap_e";_tag_dyneither(_tmp76,sizeof(char),7U);});goto _LL0;case 38U: _LL51: _LL52:
 bad_form=({const char*_tmp77="Tagcheck_e";_tag_dyneither(_tmp77,sizeof(char),11U);});goto _LL0;default: _LL53: _LL54:
 bad_form=({const char*_tmp78="Valueof_e";_tag_dyneither(_tmp78,sizeof(char),10U);});goto _LL0;}_LL0:;}
# 379
({struct Cyc_String_pa_PrintArg_struct _tmp98=({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0U,_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);_tmpD8;});struct Cyc_String_pa_PrintArg_struct _tmp99=({struct Cyc_String_pa_PrintArg_struct _tmpD7;_tmpD7.tag=0U,({
struct _dyneither_ptr _tmpE5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpD7.f1=_tmpE5;});_tmpD7;});void*_tmp96[2U];_tmp96[0]=& _tmp98,_tmp96[1]=& _tmp99;({struct _dyneither_ptr _tmpE6=({const char*_tmp97="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmp97,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpE6,_tag_dyneither(_tmp96,sizeof(void*),2U));});});}
# 383
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s){
# 385
while(1){
void*_tmp9B=s->r;void*_tmp9C=_tmp9B;struct Cyc_Absyn_Decl*_tmpC8;struct Cyc_Absyn_Stmt*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Stmt*_tmpC4;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Stmt*_tmpBF;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Stmt*_tmpBD;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Stmt*_tmpBB;struct Cyc_Absyn_Stmt*_tmpBA;struct Cyc_Absyn_Stmt*_tmpB9;struct Cyc_Absyn_Stmt*_tmpB8;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Stmt*_tmpB5;switch(*((int*)_tmp9C)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 goto _LL8;case 8U: _LL7: _LL8:
 return;case 13U: _LL9: _tmpB5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2;_LLA:
# 392
 s=_tmpB5;
continue;case 3U: _LLB: _tmpB6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_LLC:
# 395
 if(_tmpB6 == 0)
return;
_tmpB7=_tmpB6;goto _LLE;case 1U: _LLD: _tmpB7=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_LLE:
# 399
 Cyc_Toseqc_exp_to_seqc(_tmpB7);
# 407
return;case 2U: _LLF: _tmpB9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_tmpB8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2;_LL10:
# 409
 Cyc_Toseqc_stmt_to_seqc(_tmpB9);
s=_tmpB8;
continue;case 4U: _LL11: _tmpBC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_tmpBB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2;_tmpBA=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9C)->f3;_LL12:
# 413
 Cyc_Toseqc_exp_to_seqc(_tmpBC);
Cyc_Toseqc_stmt_to_seqc(_tmpBB);
s=_tmpBA;
continue;case 5U: _LL13: _tmpBE=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1).f1;_tmpBD=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2;_LL14:
# 418
 Cyc_Toseqc_exp_to_seqc(_tmpBE);
s=_tmpBD;
continue;case 9U: _LL15: _tmpC2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_tmpC1=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2).f1;_tmpC0=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9C)->f3).f1;_tmpBF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9C)->f4;_LL16:
# 422
 Cyc_Toseqc_exp_to_seqc(_tmpC2);
Cyc_Toseqc_exp_to_seqc(_tmpC1);
Cyc_Toseqc_exp_to_seqc(_tmpC0);
s=_tmpBF;
continue;case 14U: _LL17: _tmpC4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_tmpC3=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2).f1;_LL18:
# 428
 Cyc_Toseqc_exp_to_seqc(_tmpC3);
s=_tmpC4;
continue;case 10U: _LL19: _tmpC6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_tmpC5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2;_LL1A:
# 434
 Cyc_Toseqc_exp_to_seqc(_tmpC6);
for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){
Cyc_Toseqc_stmt_to_seqc(((struct Cyc_Absyn_Switch_clause*)_tmpC5->hd)->body);}
return;case 12U: _LL1B: _tmpC8=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9C)->f1;_tmpC7=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9C)->f2;_LL1C:
# 439
{void*_tmp9D=_tmpC8->r;void*_tmp9E=_tmp9D;struct Cyc_Absyn_Vardecl*_tmpB2;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9E)->tag == 0U){_LL20: _tmpB2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9E)->f1;_LL21:
# 441
 if(_tmpB2->initializer != 0){
# 444
void*_tmp9F=((struct Cyc_Absyn_Exp*)_check_null(_tmpB2->initializer))->r;void*_tmpA0=_tmp9F;struct Cyc_List_List*_tmpB1;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA0)->tag == 36U){_LL25: _tmpB1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA0)->f2;_LL26: {
# 457 "toseqc.cyc"
struct _tuple0 _tmpA1=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpB1);struct _tuple0 _tmpA2=_tmpA1;struct Cyc_List_List*_tmpB0;_LL2A: _tmpB0=_tmpA2.f2;_LL2B:;{
struct Cyc_Absyn_Stmt*_tmpA3=Cyc_Toseqc_exps_to_seqc((struct Cyc_Absyn_Exp*)_check_null(_tmpB2->initializer),_tmpB0);
# 460
if(_tmpA3 == 0)goto _LL24;
# 466
{void*_tmpA4=_tmpA3->r;void*_tmpA5=_tmpA4;struct Cyc_Absyn_Exp*_tmpAF;if(((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA5)->tag == 1U){_LL2D: _tmpAF=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA5)->f1;_LL2E:
# 468
{void*_tmpA6=((struct Cyc_Absyn_Exp*)_check_null(_tmpB2->initializer))->r;void*_tmpA7=_tmpA6;struct Cyc_Absyn_Stmt*_tmpAB;if(((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA7)->tag == 37U){_LL32: _tmpAB=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL33:
# 470
 _tmpB2->initializer=_tmpAF;
_tmpA3->r=s->r;
s->r=_tmpAB->r;
goto _LL31;}else{_LL34: _LL35:
# 475
({struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0U,({
struct _dyneither_ptr _tmpE7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmpB2->initializer)));_tmpD9.f1=_tmpE7;});_tmpD9;});void*_tmpA8[1U];_tmpA8[0]=& _tmpAA;({struct _dyneither_ptr _tmpE8=({const char*_tmpA9="exps_to_seqc updated to non-stmt-exp |%s|";_tag_dyneither(_tmpA9,sizeof(char),42U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpE8,_tag_dyneither(_tmpA8,sizeof(void*),1U));});});}_LL31:;}
# 478
goto _LL2C;}else{_LL2F: _LL30:
# 480
({struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmpDA;_tmpDA.tag=0U,({
struct _dyneither_ptr _tmpE9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmpA3));_tmpDA.f1=_tmpE9;});_tmpDA;});void*_tmpAC[1U];_tmpAC[0]=& _tmpAE;({struct _dyneither_ptr _tmpEA=({const char*_tmpAD="exps_to_seqc returned non-exp-stmt |%s|";_tag_dyneither(_tmpAD,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpEA,_tag_dyneither(_tmpAC,sizeof(void*),1U));});});}_LL2C:;}
# 483
goto _LL24;};}}else{_LL27: _LL28:
# 486
 Cyc_Toseqc_exp_to_seqc((struct Cyc_Absyn_Exp*)_check_null(_tmpB2->initializer));
goto _LL24;}_LL24:;}
# 490
s=_tmpC7;
continue;}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 494
goto _LL1E;default: _LL1D: _LL1E:
({void*_tmpB3=0U;({struct _dyneither_ptr _tmpEB=({const char*_tmpB4="bad stmt after xlation to C";_tag_dyneither(_tmpB4,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpEB,_tag_dyneither(_tmpB3,sizeof(void*),0U));});});}_LL0:;}}
# 500
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmpC9=ds;
# 503
for(0;_tmpC9 != 0;_tmpC9=_tmpC9->tl){
struct Cyc_Absyn_Decl*_tmpCA=(struct Cyc_Absyn_Decl*)_tmpC9->hd;
void*_tmpCB=_tmpCA->r;void*_tmpCC=_tmpCB;struct Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD0;struct Cyc_Absyn_Fndecl*_tmpCF;switch(*((int*)_tmpCC)){case 1U: _LL1: _tmpCF=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpCC)->f1;_LL2:
# 507
 Cyc_Toseqc_stmt_to_seqc(_tmpCF->body);
goto _LL0;case 0U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 8U: _LL11: _LL12:
# 517
 goto _LL0;case 9U: _LL13: _tmpD0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpCC)->f2;_LL14:
 _tmpD1=_tmpD0;goto _LL16;case 10U: _LL15: _tmpD1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpCC)->f2;_LL16:
 _tmpD2=_tmpD1;goto _LL18;case 11U: _LL17: _tmpD2=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpCC)->f1;_LL18:
 _tmpD3=_tmpD2;goto _LL1A;case 12U: _LL19: _tmpD3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpCC)->f1;_LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 526
({void*_tmpCD=0U;({struct _dyneither_ptr _tmpEC=({const char*_tmpCE="nested translation unit after translation to C";_tag_dyneither(_tmpCE,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpEC,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});}_LL0:;}
# 529
return ds;}
