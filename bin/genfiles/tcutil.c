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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 119
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 159
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 180
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 185
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 213
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 215
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 890 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 892
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 900
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 903
void*Cyc_Absyn_compress_kb(void*);
# 909
int Cyc_Absyn_type2bool(int def,void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 919
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 921
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 928
extern void*Cyc_Absyn_empty_effect;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 951
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 957
extern void*Cyc_Absyn_fat_bound_type;
# 959
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 961
void*Cyc_Absyn_bounds_one();
# 963
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 968
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 990
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 992
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1151
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1153
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1155
int Cyc_Absyn_fntype_att(void*);
# 1161
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 74
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 72 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 77
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 80
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 146
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr,struct _dyneither_ptr);
# 40
int Cyc_Tcutil_is_void_type(void*);
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_signed_type(void*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 53
int Cyc_Tcutil_is_dyneither_ptr(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
int Cyc_Tcutil_is_bound_one(void*b);
# 58
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 61
int Cyc_Tcutil_is_bits_only_type(void*t);
# 63
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 68
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 70
void*Cyc_Tcutil_pointer_region(void*t);
# 73
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 76
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 79
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 81
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 83
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 85
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 89
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 92
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 99
void*Cyc_Tcutil_copy_type(void*t);
# 102
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 105
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 109
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 122
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 124
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 126
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple14{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 130
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 132
extern int Cyc_Tcutil_warn_alias_coerce;
# 135
extern int Cyc_Tcutil_warn_region_coerce;
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 148
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 153
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 158
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 167
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 172
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 177
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 181
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 186
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 188
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 192
void Cyc_Tcutil_explain_failure();
# 194
int Cyc_Tcutil_unify(void*,void*);
# 196
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 199
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 201
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 207
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 210
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 214
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 216
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 220
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple16{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*,struct Cyc_Absyn_Tvar*);
# 226
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 253 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 255
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 263
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 266
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 273
void Cyc_Tcutil_check_nonzero_bound(unsigned int,void*);
# 275
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,void*,int do_warn);
# 277
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 279
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 287
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 292
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 295
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 300
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 305
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple17{int f1;void*f2;};
# 309
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 312
void*Cyc_Tcutil_normalize_effect(void*e);
# 315
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 317
int Cyc_Tcutil_new_tvar_id();
# 319
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 322
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 324
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 327
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 330
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 333
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 337
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 341
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 344
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 348
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 351
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 355
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 358
int Cyc_Tcutil_zeroable_type(void*t);
# 362
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 365
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 367
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**te);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 40 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 42 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_tmp1=_tmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 48
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 54
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_tmp5=_tmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 60
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_tmp7=_tmp6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 67
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_tmp9=_tmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 74
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 81
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_tmpD=_tmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 88
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_tmpF=_tmpE;void*_tmp11;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {
# 91
void*_tmp10=_tmp11;switch(*((int*)_tmp10)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 98
 return 0;}_LL0:;}
# 101
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_tmp13=_tmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
 return 1;}else{goto _LL5;}case 2U: _LL3: _LL4:
 return 1;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}
# 108
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 111
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 114
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Tcutil_compress(t);void*_tmp15=_tmp14;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 121
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 129
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp17=_tmp16;void*_tmp18;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17)->tag == 1U){_LL1: _LL2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp18=_tmp17;_LL4:
 return Cyc_Tcutil_is_integral_type(_tmp18);}_LL0:;}
# 137
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp19=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp1A=_tmp19;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 147
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1B=Cyc_Tcutil_compress(t);void*_tmp1C=_tmp1B;void*_tmp1D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->tag == 3U){_LL1: _tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->f1).ptr_atts).zero_term;_LL2:
# 150
 return Cyc_Tcutil_force_type2bool(0,_tmp1D);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 156
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_tmp1F=_tmp1E;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).nullable;_LL2:
# 159
 return Cyc_Tcutil_force_type2bool(0,_tmp20);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 165
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp21=Cyc_Tcutil_compress(t);void*_tmp22=_tmp21;void*_tmp23;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->tag == 3U){_LL1: _tmp23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).ptr_atts).bounds;_LL2:
# 168
 return Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp23);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 175
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp24=Cyc_Tcutil_compress(t);void*_tmp25=_tmp24;void*_tmp27;void*_tmp26;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->tag == 3U){_LL1: _tmp27=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).elt_type;_tmp26=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).ptr_atts).bounds;_LL2:
# 178
 if(Cyc_Tcutil_unify(_tmp26,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp27;
return 1;}else{
# 182
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 189
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp28=Cyc_Tcutil_compress(t);void*_tmp29=_tmp28;void*_tmp2B;void*_tmp2A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->tag == 3U){_LL1: _tmp2B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).elt_type;_tmp2A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).ptr_atts).zero_term;_LL2:
# 192
*elt_type_dest=_tmp2B;
return Cyc_Absyn_type2bool(0,_tmp2A);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 201
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 203
void*_tmp2C=Cyc_Tcutil_compress(t);void*_tmp2D=_tmp2C;void*_tmp36;struct Cyc_Absyn_Tqual _tmp35;struct Cyc_Absyn_Exp*_tmp34;void*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;switch(*((int*)_tmp2D)){case 3U: _LL1: _tmp32=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp31=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).bounds;_tmp30=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).zero_term;_LL2:
# 205
 if(Cyc_Absyn_type2bool(0,_tmp30)){
*ptr_type=t;
*elt_type=_tmp32;
{void*_tmp2E=Cyc_Tcutil_compress(_tmp31);void*_tmp2F=_tmp2E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)->tag == 14U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 212
return 1;}else{
return 0;}case 4U: _LL3: _tmp36=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp35=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).tq;_tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).zero_term;_LL4:
# 215
 if(Cyc_Absyn_type2bool(0,_tmp33)){
*elt_type=_tmp36;
*is_dyneither=0;
({void*_tmpA11=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmpA11;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 228
int Cyc_Tcutil_is_tagged_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_tagged_pointer_type_elt(t,& ignore);}
# 234
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmpA12=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpA12,b);});
if(_tmp37 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=_tmp37;
struct _tuple13 _tmp39=Cyc_Evexp_eval_const_uint_exp(_tmp38);struct _tuple13 _tmp3A=_tmp39;unsigned int _tmp3C;int _tmp3B;_LL1: _tmp3C=_tmp3A.f1;_tmp3B=_tmp3A.f2;_LL2:;
return _tmp3B  && _tmp3C == (unsigned int)1;};}
# 243
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3D=Cyc_Tcutil_compress(t);void*_tmp3E=_tmp3D;struct Cyc_List_List*_tmp4A;struct Cyc_List_List*_tmp49;void*_tmp48;void*_tmp47;void*_tmp46;struct Cyc_List_List*_tmp45;switch(*((int*)_tmp3E)){case 0U: _LL1: _tmp46=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f2;_LL2: {
# 246
void*_tmp3F=_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp44=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).KnownAggr).val;_LL19:
# 254
 if(_tmp44->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp40=_new_region("rgn");struct _RegionHandle*rgn=& _tmp40;_push_region(rgn);
{struct Cyc_List_List*_tmp41=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp44->tvs,_tmp45);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,_tmp41,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp42=0;_npop_handler(0U);return _tmp42;}}}{
int _tmp43=1;_npop_handler(0U);return _tmp43;};}
# 261
;_pop_region(rgn);};};}default: _LL1A: _LL1B:
# 265
 return 0;}_LLB:;}case 4U: _LL3: _tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_LL4:
# 270
 return !Cyc_Absyn_type2bool(0,_tmp47) && Cyc_Tcutil_is_bits_only_type(_tmp48);case 6U: _LL5: _tmp49=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E)->f1;_LL6:
# 272
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)_tmp49->hd)).f2))return 0;}
return 1;case 7U: _LL7: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E)->f2;_LL8:
# 276
 for(0;_tmp4A != 0;_tmp4A=_tmp4A->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp4A->hd)->type))return 0;}
return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 285
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_List_List*_tmp50;void*_tmp4F;switch(*((int*)_tmp4C)){case 3U: _LL1: _tmp4F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4F);case 5U: _LL3: _tmp50=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_LL4:
# 289
 for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
void*_tmp4D=(void*)_tmp50->hd;void*_tmp4E=_tmp4D;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4E)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 294
return 0;default: _LL5: _LL6:
 return 0;}_LL0:;}char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 300
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 302
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 306
int Cyc_Tcutil_warn_region_coerce=0;
# 309
static void*Cyc_Tcutil_t1_failure=0;
static int Cyc_Tcutil_tq1_const=0;
static void*Cyc_Tcutil_t2_failure=0;
static int Cyc_Tcutil_tq2_const=0;
# 314
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 318
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 323
if(({struct _dyneither_ptr _tmpA13=({const char*_tmp51="(qualifiers don't match)";_tag_dyneither(_tmp51,sizeof(char),25U);});Cyc_strcmp(_tmpA13,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp97D;_tmp97D.tag=0U,_tmp97D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp97D;});void*_tmp52[1U];_tmp52[0]=& _tmp54;({struct Cyc___cycFILE*_tmpA15=Cyc_stderr;struct _dyneither_ptr _tmpA14=({const char*_tmp53="  %s\n";_tag_dyneither(_tmp53,sizeof(char),6U);});Cyc_fprintf(_tmpA15,_tmpA14,_tag_dyneither(_tmp52,sizeof(void*),1U));});});
return;}
# 328
if(({struct _dyneither_ptr _tmpA16=({const char*_tmp55="(function effects do not match)";_tag_dyneither(_tmp55,sizeof(char),32U);});Cyc_strcmp(_tmpA16,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp56=Cyc_Absynpp_tc_params_r;
_tmp56.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp56);}{
# 333
void*_tmp57=Cyc_Tcutil_t1_failure;
void*_tmp58=Cyc_Tcutil_t2_failure;
struct _dyneither_ptr s1=(unsigned int)_tmp57?Cyc_Absynpp_typ2string(_tmp57):({const char*_tmp73="<?>";_tag_dyneither(_tmp73,sizeof(char),4U);});
struct _dyneither_ptr s2=(unsigned int)_tmp58?Cyc_Absynpp_typ2string(_tmp58):({const char*_tmp72="<?>";_tag_dyneither(_tmp72,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp5B=({struct Cyc_String_pa_PrintArg_struct _tmp97E;_tmp97E.tag=0U,_tmp97E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp97E;});void*_tmp59[1U];_tmp59[0]=& _tmp5B;({struct Cyc___cycFILE*_tmpA18=Cyc_stderr;struct _dyneither_ptr _tmpA17=({const char*_tmp5A="  %s";_tag_dyneither(_tmp5A,sizeof(char),5U);});Cyc_fprintf(_tmpA18,_tmpA17,_tag_dyneither(_tmp59,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp5C=0U;({struct Cyc___cycFILE*_tmpA1A=Cyc_stderr;struct _dyneither_ptr _tmpA19=({const char*_tmp5D="\n\t";_tag_dyneither(_tmp5D,sizeof(char),3U);});Cyc_fprintf(_tmpA1A,_tmpA19,_tag_dyneither(_tmp5C,sizeof(void*),0U));});});
pos=8;}else{
# 344
({void*_tmp5E=0U;({struct Cyc___cycFILE*_tmpA1C=Cyc_stderr;struct _dyneither_ptr _tmpA1B=({const char*_tmp5F=" ";_tag_dyneither(_tmp5F,sizeof(char),2U);});Cyc_fprintf(_tmpA1C,_tmpA1B,_tag_dyneither(_tmp5E,sizeof(void*),0U));});});
++ pos;}
# 347
({void*_tmp60=0U;({struct Cyc___cycFILE*_tmpA1E=Cyc_stderr;struct _dyneither_ptr _tmpA1D=({const char*_tmp61="and ";_tag_dyneither(_tmp61,sizeof(char),5U);});Cyc_fprintf(_tmpA1E,_tmpA1D,_tag_dyneither(_tmp60,sizeof(void*),0U));});});
pos +=4;
if((unsigned int)pos + _get_dyneither_size(s2,sizeof(char))>= (unsigned int)80){
({void*_tmp62=0U;({struct Cyc___cycFILE*_tmpA20=Cyc_stderr;struct _dyneither_ptr _tmpA1F=({const char*_tmp63="\n\t";_tag_dyneither(_tmp63,sizeof(char),3U);});Cyc_fprintf(_tmpA20,_tmpA1F,_tag_dyneither(_tmp62,sizeof(void*),0U));});});
pos=8;}
# 353
({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp97F;_tmp97F.tag=0U,_tmp97F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp97F;});void*_tmp64[1U];_tmp64[0]=& _tmp66;({struct Cyc___cycFILE*_tmpA22=Cyc_stderr;struct _dyneither_ptr _tmpA21=({const char*_tmp65="%s ";_tag_dyneither(_tmp65,sizeof(char),4U);});Cyc_fprintf(_tmpA22,_tmpA21,_tag_dyneither(_tmp64,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ (unsigned int)1;
if(pos + 17 >= 80){
({void*_tmp67=0U;({struct Cyc___cycFILE*_tmpA24=Cyc_stderr;struct _dyneither_ptr _tmpA23=({const char*_tmp68="\n\t";_tag_dyneither(_tmp68,sizeof(char),3U);});Cyc_fprintf(_tmpA24,_tmpA23,_tag_dyneither(_tmp67,sizeof(void*),0U));});});
pos=8;}
# 359
({void*_tmp69=0U;({struct Cyc___cycFILE*_tmpA26=Cyc_stderr;struct _dyneither_ptr _tmpA25=({const char*_tmp6A="are not compatible. ";_tag_dyneither(_tmp6A,sizeof(char),21U);});Cyc_fprintf(_tmpA26,_tmpA25,_tag_dyneither(_tmp69,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpA27=(char*)Cyc_Tcutil_failure_reason.curr;_tmpA27 != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({unsigned long _tmpA28=(unsigned long)pos;_tmpA28 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= (unsigned long)80)
({void*_tmp6B=0U;({struct Cyc___cycFILE*_tmpA2A=Cyc_stderr;struct _dyneither_ptr _tmpA29=({const char*_tmp6C="\n\t";_tag_dyneither(_tmp6C,sizeof(char),3U);});Cyc_fprintf(_tmpA2A,_tmpA29,_tag_dyneither(_tmp6B,sizeof(void*),0U));});});
# 365
({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp980;_tmp980.tag=0U,_tmp980.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp980;});void*_tmp6D[1U];_tmp6D[0]=& _tmp6F;({struct Cyc___cycFILE*_tmpA2C=Cyc_stderr;struct _dyneither_ptr _tmpA2B=({const char*_tmp6E="%s";_tag_dyneither(_tmp6E,sizeof(char),3U);});Cyc_fprintf(_tmpA2C,_tmpA2B,_tag_dyneither(_tmp6D,sizeof(void*),1U));});});}
# 367
({void*_tmp70=0U;({struct Cyc___cycFILE*_tmpA2E=Cyc_stderr;struct _dyneither_ptr _tmpA2D=({const char*_tmp71="\n";_tag_dyneither(_tmp71,sizeof(char),2U);});Cyc_fprintf(_tmpA2E,_tmpA2D,_tag_dyneither(_tmp70,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 371
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 373
Cyc_Warn_verr(loc,fmt,ap);}
# 375
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 378
Cyc_Warn_vimpos(fmt,ap);}
# 380
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 382
Cyc_Warn_vwarn(sg,fmt,ap);}
# 386
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 391
void*Cyc_Tcutil_compress(void*t){
void*_tmp74=t;void*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Exp*_tmp7B;void**_tmp7A;void**_tmp79;switch(*((int*)_tmp74)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp79=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f2;_LL8: {
# 402
void*ta=(void*)_check_null(*_tmp79);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp79=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74)->f4 == 0){_LL3: _LL4:
# 394
 return t;}else{_LL5: _tmp7A=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74)->f4;_LL6: {
# 396
void*ta=(void*)_check_null(*_tmp7A);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7A=t2;
return t2;}}case 9U: _LL9: _tmp7B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp74)->f1;_LLA:
# 408
 Cyc_Evexp_eval_const_uint_exp(_tmp7B);{
void*_tmp75=_tmp7B->r;void*_tmp76=_tmp75;void*_tmp77;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp76)->tag == 39U){_LL12: _tmp77=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp76)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp77);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp7C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp74)->f1;_LLC: {
# 414
void*_tmp78=_tmp7C->topt;
if(_tmp78 != 0)return _tmp78;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74)->f2 != 0){_LLD: _tmp7D=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74)->f2;_LLE:
# 418
 return Cyc_Tcutil_compress(_tmp7D);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 427
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 431
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp7E=Cyc_Absyn_compress_kb(kb);void*_tmp7F=_tmp7E;struct Cyc_Absyn_Kind*_tmp82;switch(*((int*)_tmp7F)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=1U,_tmp80->f1=0;_tmp80;});case 2U: _LL3: _tmp82=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=2U,_tmp81->f1=0,_tmp81->f2=_tmp82;_tmp81;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 438
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 440
return({struct Cyc_Absyn_Tvar*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->name=tv->name,_tmp83->identity=- 1,({void*_tmpA2F=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp83->kind=_tmpA2F;});_tmp83;});}
# 442
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 444
struct _tuple10*_tmp84=arg;struct _dyneither_ptr*_tmp88;struct Cyc_Absyn_Tqual _tmp87;void*_tmp86;_LL1: _tmp88=_tmp84->f1;_tmp87=_tmp84->f2;_tmp86=_tmp84->f3;_LL2:;
return({struct _tuple10*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=_tmp88,_tmp85->f2=_tmp87,({void*_tmpA30=Cyc_Tcutil_copy_type(_tmp86);_tmp85->f3=_tmpA30;});_tmp85;});}
# 447
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp89=arg;struct Cyc_Absyn_Tqual _tmp8C;void*_tmp8B;_LL1: _tmp8C=_tmp89->f1;_tmp8B=_tmp89->f2;_LL2:;
return({struct _tuple12*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=_tmp8C,({void*_tmpA31=Cyc_Tcutil_copy_type(_tmp8B);_tmp8A->f2=_tmpA31;});_tmp8A;});}
# 451
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 453
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->name=f->name,_tmp8D->tq=f->tq,({void*_tmpA33=Cyc_Tcutil_copy_type(f->type);_tmp8D->type=_tmpA33;}),_tmp8D->width=f->width,_tmp8D->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpA32=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp8D->requires_clause=_tmpA32;});_tmp8D;});}
# 457
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp8E=x;void*_tmp91;void*_tmp90;_LL1: _tmp91=_tmp8E->f1;_tmp90=_tmp8E->f2;_LL2:;
return({struct _tuple0*_tmp8F=_cycalloc(sizeof(*_tmp8F));({void*_tmpA35=Cyc_Tcutil_copy_type(_tmp91);_tmp8F->f1=_tmpA35;}),({void*_tmpA34=Cyc_Tcutil_copy_type(_tmp90);_tmp8F->f2=_tmpA34;});_tmp8F;});}
# 461
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->name=f->name,_tmp92->tag=f->tag,_tmp92->loc=f->loc;_tmp92;});}
# 464
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 468
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp93=Cyc_Tcutil_compress(t);void*_tmp94=_tmp93;struct Cyc_Absyn_Datatypedecl*_tmpD8;struct Cyc_Absyn_Enumdecl*_tmpD7;struct Cyc_Absyn_Aggrdecl*_tmpD6;struct _tuple2*_tmpD5;struct Cyc_List_List*_tmpD4;struct Cyc_Absyn_Typedefdecl*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD1;enum Cyc_Absyn_AggrKind _tmpD0;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCD;void*_tmpCC;struct Cyc_Absyn_Tqual _tmpCB;void*_tmpCA;struct Cyc_List_List*_tmpC9;int _tmpC8;struct Cyc_Absyn_VarargInfo*_tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_List_List*_tmpC3;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_List_List*_tmpC1;void*_tmpC0;struct Cyc_Absyn_Tqual _tmpBF;struct Cyc_Absyn_Exp*_tmpBE;void*_tmpBD;unsigned int _tmpBC;void*_tmpBB;struct Cyc_Absyn_Tqual _tmpBA;void*_tmpB9;void*_tmpB8;void*_tmpB7;void*_tmpB6;struct Cyc_Absyn_PtrLoc*_tmpB5;struct Cyc_Absyn_Tvar*_tmpB4;void*_tmpB3;struct Cyc_List_List*_tmpB2;void*_tmpB1;switch(*((int*)_tmp94)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f2 == 0){_LL1: _tmpB1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f1;_LL2:
 return t;}else{_LL3: _tmpB3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f1;_tmpB2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp94)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=0U,_tmp95->f1=_tmpB3,({struct Cyc_List_List*_tmpA36=Cyc_Tcutil_copy_types(_tmpB2);_tmp95->f2=_tmpA36;});_tmp95;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmpB4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp94)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmpB4));case 3U: _LL9: _tmpBB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_type;_tmpBA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).elt_tq;_tmpB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).rgn;_tmpB8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).nullable;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).bounds;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).zero_term;_tmpB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp94)->f1).ptr_atts).ptrloc;_LLA: {
# 475
void*_tmp96=Cyc_Tcutil_copy_type(_tmpBB);
void*_tmp97=Cyc_Tcutil_copy_type(_tmpB9);
void*_tmp98=Cyc_Tcutil_copy_type(_tmpB8);
struct Cyc_Absyn_Tqual _tmp99=_tmpBA;
# 480
void*_tmp9A=Cyc_Tcutil_copy_type(_tmpB7);
void*_tmp9B=Cyc_Tcutil_copy_type(_tmpB6);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=3U,(_tmp9C->f1).elt_type=_tmp96,(_tmp9C->f1).elt_tq=_tmp99,((_tmp9C->f1).ptr_atts).rgn=_tmp97,((_tmp9C->f1).ptr_atts).nullable=_tmp98,((_tmp9C->f1).ptr_atts).bounds=_tmp9A,((_tmp9C->f1).ptr_atts).zero_term=_tmp9B,((_tmp9C->f1).ptr_atts).ptrloc=_tmpB5;_tmp9C;});}case 4U: _LLB: _tmpC0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).elt_type;_tmpBF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).tq;_tmpBE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).num_elts;_tmpBD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).zero_term;_tmpBC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp94)->f1).zt_loc;_LLC: {
# 484
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBE);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=4U,({void*_tmpA38=Cyc_Tcutil_copy_type(_tmpC0);(_tmp9D->f1).elt_type=_tmpA38;}),(_tmp9D->f1).tq=_tmpBF,(_tmp9D->f1).num_elts=eopt2,({
void*_tmpA37=Cyc_Tcutil_copy_type(_tmpBD);(_tmp9D->f1).zero_term=_tmpA37;}),(_tmp9D->f1).zt_loc=_tmpBC;_tmp9D;});}case 5U: _LLD: _tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).tvars;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).effect;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ret_tqual;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ret_type;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).args;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).c_varargs;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).cyc_varargs;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).rgn_po;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).attributes;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).requires_clause;_tmpC3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).requires_relns;_tmpC2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ensures_clause;_tmpC1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94)->f1).ensures_relns;_LLE: {
# 488
struct Cyc_List_List*_tmp9E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpCD);
void*effopt2=_tmpCC == 0?0: Cyc_Tcutil_copy_type(_tmpCC);
void*_tmp9F=Cyc_Tcutil_copy_type(_tmpCA);
struct Cyc_List_List*_tmpA0=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpC9);
int _tmpA1=_tmpC8;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpC7 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpC7;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->name=cv->name,_tmpA2->tq=cv->tq,({void*_tmpA39=Cyc_Tcutil_copy_type(cv->type);_tmpA2->type=_tmpA39;}),_tmpA2->inject=cv->inject;_tmpA2;});}{
# 499
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpC6);
struct Cyc_List_List*_tmpA4=_tmpC5;
struct Cyc_Absyn_Exp*_tmpA5=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC4);
struct Cyc_List_List*_tmpA6=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC3);
struct Cyc_Absyn_Exp*_tmpA7=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC2);
struct Cyc_List_List*_tmpA8=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC1);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=5U,(_tmpA9->f1).tvars=_tmp9E,(_tmpA9->f1).effect=effopt2,(_tmpA9->f1).ret_tqual=_tmpCB,(_tmpA9->f1).ret_type=_tmp9F,(_tmpA9->f1).args=_tmpA0,(_tmpA9->f1).c_varargs=_tmpA1,(_tmpA9->f1).cyc_varargs=cyc_varargs2,(_tmpA9->f1).rgn_po=_tmpA3,(_tmpA9->f1).attributes=_tmpA4,(_tmpA9->f1).requires_clause=_tmpA5,(_tmpA9->f1).requires_relns=_tmpA6,(_tmpA9->f1).ensures_clause=_tmpA7,(_tmpA9->f1).ensures_relns=_tmpA8;_tmpA9;});};}case 6U: _LLF: _tmpCE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp94)->f1;_LL10:
# 508
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=6U,({struct Cyc_List_List*_tmpA3A=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpCE);_tmpAA->f1=_tmpA3A;});_tmpAA;});case 7U: _LL11: _tmpD0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp94)->f1;_tmpCF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp94)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=7U,_tmpAB->f1=_tmpD0,({struct Cyc_List_List*_tmpA3B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpCF);_tmpAB->f2=_tmpA3B;});_tmpAB;});case 9U: _LL13: _tmpD1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp94)->f1;_LL14:
# 512
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=9U,_tmpAC->f1=_tmpD1;_tmpAC;});case 11U: _LL15: _tmpD2=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp94)->f1;_LL16:
# 515
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=11U,_tmpAD->f1=_tmpD2;_tmpAD;});case 8U: _LL17: _tmpD5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp94)->f1;_tmpD4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp94)->f2;_tmpD3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp94)->f3;_LL18:
# 517
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=8U,_tmpAE->f1=_tmpD5,({struct Cyc_List_List*_tmpA3C=Cyc_Tcutil_copy_types(_tmpD4);_tmpAE->f2=_tmpA3C;}),_tmpAE->f3=_tmpD3,_tmpAE->f4=0;_tmpAE;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)){case 0U: _LL19: _tmpD6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)->f1;_LL1A: {
# 520
struct Cyc_List_List*_tmpAF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD6->tvs);
return({union Cyc_Absyn_AggrInfo _tmpA3D=Cyc_Absyn_UnknownAggr(_tmpD6->kind,_tmpD6->name,0);Cyc_Absyn_aggr_type(_tmpA3D,_tmpAF);});}case 1U: _LL1B: _tmpD7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)->f1;_LL1C:
# 523
 return Cyc_Absyn_enum_type(_tmpD7->name,0);default: _LL1D: _tmpD8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp94)->f1)->r)->f1;_LL1E: {
# 525
struct Cyc_List_List*_tmpB0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD8->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmpA3E=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp981;_tmp981.name=_tmpD8->name,_tmp981.is_extensible=0;_tmp981;}));Cyc_Absyn_datatype_type(_tmpA3E,_tmpB0);});}}}_LL0:;}
# 540 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD9=d;struct _dyneither_ptr*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD9)->tag == 0U){_LL1: _tmpDB=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD9)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->tag=0U,({struct Cyc_Absyn_Exp*_tmpA3F=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpDB);_tmpDA->f1=_tmpA3F;});_tmpDA;});}else{_LL3: _tmpDC=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD9)->f1;_LL4:
 return d;}_LL0:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 546
static struct _tuple18*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple18*e){
# 548
return({struct _tuple18*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_List_List*_tmpA41=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpDD->f1=_tmpA41;}),({struct Cyc_Absyn_Exp*_tmpA40=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpDD->f2=_tmpA40;});_tmpDD;});}
# 551
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpDE=preserve_types;
{void*_tmpDF=e->r;void*_tmpE0=_tmpDF;int _tmp15D;struct _dyneither_ptr _tmp15C;void*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct _dyneither_ptr*_tmp159;struct Cyc_Core_Opt*_tmp158;struct Cyc_List_List*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp155;int _tmp154;struct Cyc_Absyn_Exp*_tmp153;void**_tmp152;struct Cyc_Absyn_Exp*_tmp151;int _tmp150;int _tmp14F;void*_tmp14E;struct Cyc_Absyn_Enumfield*_tmp14D;struct Cyc_Absyn_Enumdecl*_tmp14C;struct Cyc_Absyn_Enumfield*_tmp14B;struct Cyc_List_List*_tmp14A;struct Cyc_Absyn_Datatypedecl*_tmp149;struct Cyc_Absyn_Datatypefield*_tmp148;void*_tmp147;struct Cyc_List_List*_tmp146;struct _tuple2*_tmp145;struct Cyc_List_List*_tmp144;struct Cyc_List_List*_tmp143;struct Cyc_Absyn_Aggrdecl*_tmp142;struct Cyc_Absyn_Exp*_tmp141;void*_tmp140;int _tmp13F;struct Cyc_Absyn_Vardecl*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;int _tmp13B;struct Cyc_List_List*_tmp13A;struct _dyneither_ptr*_tmp139;struct Cyc_Absyn_Tqual _tmp138;void*_tmp137;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct _dyneither_ptr*_tmp131;int _tmp130;int _tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct _dyneither_ptr*_tmp12D;int _tmp12C;int _tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;void*_tmp128;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp126;void*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;void*_tmp121;struct Cyc_Absyn_Exp*_tmp120;int _tmp11F;enum Cyc_Absyn_Coercion _tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;int _tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_VarargCallInfo*_tmp116;int _tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;enum Cyc_Absyn_Incrementor _tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Core_Opt*_tmp108;struct Cyc_Absyn_Exp*_tmp107;enum Cyc_Absyn_Primop _tmp106;struct Cyc_List_List*_tmp105;struct _dyneither_ptr _tmp104;void*_tmp103;union Cyc_Absyn_Cnst _tmp102;switch(*((int*)_tmpE0)){case 0U: _LL1: _tmp102=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL2:
 new_e=Cyc_Absyn_const_exp(_tmp102,e->loc);goto _LL0;case 1U: _LL3: _tmp103=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL4:
 new_e=Cyc_Absyn_varb_exp(_tmp103,e->loc);goto _LL0;case 2U: _LL5: _tmp104=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL6:
 new_e=Cyc_Absyn_pragma_exp(_tmp104,e->loc);goto _LL0;case 3U: _LL7: _tmp106=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp105=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL8:
 new_e=({enum Cyc_Absyn_Primop _tmpA43=_tmp106;struct Cyc_List_List*_tmpA42=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp105);Cyc_Absyn_primop_exp(_tmpA43,_tmpA42,e->loc);});goto _LL0;case 4U: _LL9: _tmp109=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp108=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp107=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LLA:
# 560
 new_e=({struct Cyc_Absyn_Exp*_tmpA46=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp109);struct Cyc_Core_Opt*_tmpA45=(unsigned int)_tmp108?({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=(void*)_tmp108->v;_tmpE1;}): 0;struct Cyc_Absyn_Exp*_tmpA44=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp107);Cyc_Absyn_assignop_exp(_tmpA46,_tmpA45,_tmpA44,e->loc);});
goto _LL0;case 5U: _LLB: _tmp10B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp10A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LLC:
 new_e=({struct Cyc_Absyn_Exp*_tmpA48=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10B);enum Cyc_Absyn_Incrementor _tmpA47=_tmp10A;Cyc_Absyn_increment_exp(_tmpA48,_tmpA47,e->loc);});goto _LL0;case 6U: _LLD: _tmp10E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp10D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp10C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LLE:
# 564
 new_e=({struct Cyc_Absyn_Exp*_tmpA4B=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10E);struct Cyc_Absyn_Exp*_tmpA4A=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10D);struct Cyc_Absyn_Exp*_tmpA49=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10C);Cyc_Absyn_conditional_exp(_tmpA4B,_tmpA4A,_tmpA49,e->loc);});goto _LL0;case 7U: _LLF: _tmp110=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp10F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL10:
 new_e=({struct Cyc_Absyn_Exp*_tmpA4D=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp110);struct Cyc_Absyn_Exp*_tmpA4C=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp10F);Cyc_Absyn_and_exp(_tmpA4D,_tmpA4C,e->loc);});goto _LL0;case 8U: _LL11: _tmp112=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp111=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL12:
 new_e=({struct Cyc_Absyn_Exp*_tmpA4F=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp112);struct Cyc_Absyn_Exp*_tmpA4E=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp111);Cyc_Absyn_or_exp(_tmpA4F,_tmpA4E,e->loc);});goto _LL0;case 9U: _LL13: _tmp114=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp113=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL14:
 new_e=({struct Cyc_Absyn_Exp*_tmpA51=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp114);struct Cyc_Absyn_Exp*_tmpA50=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp113);Cyc_Absyn_seq_exp(_tmpA51,_tmpA50,e->loc);});goto _LL0;case 10U: _LL15: _tmp118=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp117=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp116=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp115=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL16:
# 569
{struct Cyc_Absyn_VarargCallInfo*_tmpE2=_tmp116;int _tmpEE;struct Cyc_List_List*_tmpED;struct Cyc_Absyn_VarargInfo*_tmpEC;if(_tmpE2 != 0){_LL56: _tmpEE=_tmpE2->num_varargs;_tmpED=_tmpE2->injectors;_tmpEC=_tmpE2->vai;_LL57: {
# 571
struct Cyc_Absyn_VarargInfo*_tmpE3=_tmpEC;struct _dyneither_ptr*_tmpEA;struct Cyc_Absyn_Tqual _tmpE9;void*_tmpE8;int _tmpE7;_LL5B: _tmpEA=_tmpE3->name;_tmpE9=_tmpE3->tq;_tmpE8=_tmpE3->type;_tmpE7=_tmpE3->inject;_LL5C:;
new_e=({void*_tmpA57=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->tag=10U,({
struct Cyc_Absyn_Exp*_tmpA56=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp118);_tmpE6->f1=_tmpA56;}),({struct Cyc_List_List*_tmpA55=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp117);_tmpE6->f2=_tmpA55;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpA54=({struct Cyc_Absyn_VarargCallInfo*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->num_varargs=_tmpEE,_tmpE5->injectors=_tmpED,({
struct Cyc_Absyn_VarargInfo*_tmpA53=({struct Cyc_Absyn_VarargInfo*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->name=_tmpEA,_tmpE4->tq=_tmpE9,({void*_tmpA52=Cyc_Tcutil_copy_type(_tmpE8);_tmpE4->type=_tmpA52;}),_tmpE4->inject=_tmpE7;_tmpE4;});_tmpE5->vai=_tmpA53;});_tmpE5;});
# 574
_tmpE6->f3=_tmpA54;}),_tmpE6->f4=_tmp115;_tmpE6;});
# 572
Cyc_Absyn_new_exp(_tmpA57,e->loc);});
# 577
goto _LL55;}}else{_LL58: _LL59:
# 579
 new_e=({void*_tmpA5A=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->tag=10U,({struct Cyc_Absyn_Exp*_tmpA59=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp118);_tmpEB->f1=_tmpA59;}),({struct Cyc_List_List*_tmpA58=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp117);_tmpEB->f2=_tmpA58;}),_tmpEB->f3=_tmp116,_tmpEB->f4=_tmp115;_tmpEB;});Cyc_Absyn_new_exp(_tmpA5A,e->loc);});
goto _LL55;}_LL55:;}
# 582
goto _LL0;case 11U: _LL17: _tmp11A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp119=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL18:
# 584
 new_e=_tmp119?({struct Cyc_Absyn_Exp*_tmpA5C=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11A);Cyc_Absyn_rethrow_exp(_tmpA5C,e->loc);}):({struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11A);Cyc_Absyn_throw_exp(_tmpA5B,e->loc);});
goto _LL0;case 12U: _LL19: _tmp11B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL1A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA5D=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11B);Cyc_Absyn_noinstantiate_exp(_tmpA5D,e->loc);});
goto _LL0;case 13U: _LL1B: _tmp11D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp11C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL1C:
# 589
 new_e=({struct Cyc_Absyn_Exp*_tmpA5F=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp11D);struct Cyc_List_List*_tmpA5E=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp11C);Cyc_Absyn_instantiate_exp(_tmpA5F,_tmpA5E,e->loc);});
goto _LL0;case 14U: _LL1D: _tmp121=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp120=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp11F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp11E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL1E:
# 592
 new_e=({void*_tmpA63=Cyc_Tcutil_copy_type(_tmp121);struct Cyc_Absyn_Exp*_tmpA62=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp120);int _tmpA61=_tmp11F;enum Cyc_Absyn_Coercion _tmpA60=_tmp11E;Cyc_Absyn_cast_exp(_tmpA63,_tmpA62,_tmpA61,_tmpA60,e->loc);});goto _LL0;case 15U: _LL1F: _tmp122=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL20:
 new_e=({struct Cyc_Absyn_Exp*_tmpA64=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp122);Cyc_Absyn_address_exp(_tmpA64,e->loc);});goto _LL0;case 16U: _LL21: _tmp124=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp123=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL22: {
# 595
struct Cyc_Absyn_Exp*eo1=_tmp124;if(_tmp124 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp124);
new_e=({struct Cyc_Absyn_Exp*_tmpA66=eo1;struct Cyc_Absyn_Exp*_tmpA65=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp123);Cyc_Absyn_New_exp(_tmpA66,_tmpA65,e->loc);});
goto _LL0;}case 17U: _LL23: _tmp125=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL24:
 new_e=({void*_tmpA67=Cyc_Tcutil_copy_type(_tmp125);Cyc_Absyn_sizeoftype_exp(_tmpA67,e->loc);});
goto _LL0;case 18U: _LL25: _tmp126=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL26:
 new_e=({struct Cyc_Absyn_Exp*_tmpA68=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp126);Cyc_Absyn_sizeofexp_exp(_tmpA68,e->loc);});goto _LL0;case 19U: _LL27: _tmp128=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp127=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL28:
# 602
 new_e=({void*_tmpA6A=Cyc_Tcutil_copy_type(_tmp128);struct Cyc_List_List*_tmpA69=_tmp127;Cyc_Absyn_offsetof_exp(_tmpA6A,_tmpA69,e->loc);});goto _LL0;case 20U: _LL29: _tmp129=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL2A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp129);Cyc_Absyn_deref_exp(_tmpA6B,e->loc);});goto _LL0;case 41U: _LL2B: _tmp12A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL2C:
 new_e=({struct Cyc_Absyn_Exp*_tmpA6C=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp12A);Cyc_Absyn_extension_exp(_tmpA6C,e->loc);});goto _LL0;case 21U: _LL2D: _tmp12E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp12D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp12C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp12B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL2E:
# 606
 new_e=({void*_tmpA6E=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->tag=21U,({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp12E);_tmpEF->f1=_tmpA6D;}),_tmpEF->f2=_tmp12D,_tmpEF->f3=_tmp12C,_tmpEF->f4=_tmp12B;_tmpEF;});Cyc_Absyn_new_exp(_tmpA6E,e->loc);});goto _LL0;case 22U: _LL2F: _tmp132=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp131=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp130=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp12F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL30:
# 608
 new_e=({void*_tmpA70=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->tag=22U,({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp132);_tmpF0->f1=_tmpA6F;}),_tmpF0->f2=_tmp131,_tmpF0->f3=_tmp130,_tmpF0->f4=_tmp12F;_tmpF0;});Cyc_Absyn_new_exp(_tmpA70,e->loc);});goto _LL0;case 23U: _LL31: _tmp134=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp133=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL32:
 new_e=({struct Cyc_Absyn_Exp*_tmpA72=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp134);struct Cyc_Absyn_Exp*_tmpA71=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp133);Cyc_Absyn_subscript_exp(_tmpA72,_tmpA71,e->loc);});
goto _LL0;case 24U: _LL33: _tmp135=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL34:
 new_e=({struct Cyc_List_List*_tmpA73=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp135);Cyc_Absyn_tuple_exp(_tmpA73,e->loc);});goto _LL0;case 25U: _LL35: _tmp139=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1)->f1;_tmp138=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1)->f2;_tmp137=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1)->f3;_tmp136=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL36: {
# 613
struct _dyneither_ptr*vopt1=_tmp139;
if(_tmp139 != 0)vopt1=_tmp139;
new_e=({void*_tmpA77=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->tag=25U,({struct _tuple10*_tmpA76=({struct _tuple10*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->f1=vopt1,_tmpF1->f2=_tmp138,({void*_tmpA75=Cyc_Tcutil_copy_type(_tmp137);_tmpF1->f3=_tmpA75;});_tmpF1;});_tmpF2->f1=_tmpA76;}),({
struct Cyc_List_List*_tmpA74=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp136);_tmpF2->f2=_tmpA74;});_tmpF2;});
# 615
Cyc_Absyn_new_exp(_tmpA77,e->loc);});
# 617
goto _LL0;}case 26U: _LL37: _tmp13A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL38:
 new_e=({void*_tmpA79=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=26U,({struct Cyc_List_List*_tmpA78=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp13A);_tmpF3->f1=_tmpA78;});_tmpF3;});Cyc_Absyn_new_exp(_tmpA79,e->loc);});
goto _LL0;case 27U: _LL39: _tmp13E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp13D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp13C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp13B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL3A:
# 621
 new_e=({void*_tmpA7C=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=27U,_tmpF4->f1=_tmp13E,({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp13D);_tmpF4->f2=_tmpA7B;}),({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp13C);_tmpF4->f3=_tmpA7A;}),_tmpF4->f4=_tmp13B;_tmpF4;});Cyc_Absyn_new_exp(_tmpA7C,e->loc);});
goto _LL0;case 28U: _LL3B: _tmp141=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp140=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp13F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LL3C:
# 624
 new_e=({void*_tmpA7F=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=28U,({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp141);_tmpF5->f1=_tmpA7E;}),({void*_tmpA7D=Cyc_Tcutil_copy_type(_tmp140);_tmpF5->f2=_tmpA7D;}),_tmpF5->f3=_tmp13F;_tmpF5;});Cyc_Absyn_new_exp(_tmpA7F,_tmp141->loc);});
# 626
goto _LL0;case 29U: _LL3D: _tmp145=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp144=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp143=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_tmp142=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE0)->f4;_LL3E:
# 628
 new_e=({void*_tmpA82=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->tag=29U,_tmpF6->f1=_tmp145,({struct Cyc_List_List*_tmpA81=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp144);_tmpF6->f2=_tmpA81;}),({struct Cyc_List_List*_tmpA80=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp143);_tmpF6->f3=_tmpA80;}),_tmpF6->f4=_tmp142;_tmpF6;});Cyc_Absyn_new_exp(_tmpA82,e->loc);});
# 630
goto _LL0;case 30U: _LL3F: _tmp147=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp146=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL40:
# 632
 new_e=({void*_tmpA85=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->tag=30U,({void*_tmpA84=Cyc_Tcutil_copy_type(_tmp147);_tmpF7->f1=_tmpA84;}),({struct Cyc_List_List*_tmpA83=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp146);_tmpF7->f2=_tmpA83;});_tmpF7;});Cyc_Absyn_new_exp(_tmpA85,e->loc);});
goto _LL0;case 31U: _LL41: _tmp14A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp149=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_tmp148=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE0)->f3;_LL42:
# 635
 new_e=({void*_tmpA87=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->tag=31U,({struct Cyc_List_List*_tmpA86=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDE,_tmp14A);_tmpF8->f1=_tmpA86;}),_tmpF8->f2=_tmp149,_tmpF8->f3=_tmp148;_tmpF8;});Cyc_Absyn_new_exp(_tmpA87,e->loc);});
goto _LL0;case 32U: _LL43: _tmp14C=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp14B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL44:
 new_e=e;goto _LL0;case 33U: _LL45: _tmp14E=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp14D=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL46:
# 639
 new_e=({void*_tmpA89=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->tag=33U,({void*_tmpA88=Cyc_Tcutil_copy_type(_tmp14E);_tmpF9->f1=_tmpA88;}),_tmpF9->f2=_tmp14D;_tmpF9;});Cyc_Absyn_new_exp(_tmpA89,e->loc);});
goto _LL0;case 34U: _LL47: _tmp154=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).is_calloc;_tmp153=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).rgn;_tmp152=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).elt_type;_tmp151=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).num_elts;_tmp150=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).fat_result;_tmp14F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE0)->f1).inline_call;_LL48: {
# 642
struct Cyc_Absyn_Exp*_tmpFA=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp153;if(_tmp153 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp153);{
void**t1=_tmp152;if(_tmp152 != 0)t1=({void**_tmpFB=_cycalloc(sizeof(*_tmpFB));({void*_tmpA8A=Cyc_Tcutil_copy_type(*_tmp152);*_tmpFB=_tmpA8A;});_tmpFB;});
({void*_tmpA8B=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=34U,(_tmpFC->f1).is_calloc=_tmp154,(_tmpFC->f1).rgn=r1,(_tmpFC->f1).elt_type=t1,(_tmpFC->f1).num_elts=_tmp151,(_tmpFC->f1).fat_result=_tmp150,(_tmpFC->f1).inline_call=_tmp14F;_tmpFC;});_tmpFA->r=_tmpA8B;});
new_e=_tmpFA;
goto _LL0;};}case 35U: _LL49: _tmp156=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp155=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL4A:
 new_e=({struct Cyc_Absyn_Exp*_tmpA8D=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp156);struct Cyc_Absyn_Exp*_tmpA8C=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp155);Cyc_Absyn_swap_exp(_tmpA8D,_tmpA8C,e->loc);});goto _LL0;case 36U: _LL4B: _tmp158=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp157=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL4C: {
# 650
struct Cyc_Core_Opt*nopt1=_tmp158;
if(_tmp158 != 0)nopt1=({struct Cyc_Core_Opt*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->v=(struct _tuple2*)_tmp158->v;_tmpFD;});
new_e=({void*_tmpA8F=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=36U,_tmpFE->f1=nopt1,({struct Cyc_List_List*_tmpA8E=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDE,_tmp157);_tmpFE->f2=_tmpA8E;});_tmpFE;});Cyc_Absyn_new_exp(_tmpA8F,e->loc);});
goto _LL0;}case 37U: _LL4D: _LL4E:
# 655
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpA90=({const char*_tmpFF="deep_copy: statement expressions unsupported";_tag_dyneither(_tmpFF,sizeof(char),45U);});_tmp100->f1=_tmpA90;});_tmp100;}));case 38U: _LL4F: _tmp15A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp159=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL50:
 new_e=({void*_tmpA92=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=38U,({struct Cyc_Absyn_Exp*_tmpA91=Cyc_Tcutil_deep_copy_exp(_tmpDE,_tmp15A);_tmp101->f1=_tmpA91;}),_tmp101->f2=_tmp159;_tmp101;});Cyc_Absyn_new_exp(_tmpA92,e->loc);});
goto _LL0;case 39U: _LL51: _tmp15B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_LL52:
 new_e=({void*_tmpA93=Cyc_Tcutil_copy_type(_tmp15B);Cyc_Absyn_valueof_exp(_tmpA93,e->loc);});
goto _LL0;default: _LL53: _tmp15D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE0)->f1;_tmp15C=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE0)->f2;_LL54:
 new_e=Cyc_Absyn_asm_exp(_tmp15D,_tmp15C,e->loc);goto _LL0;}_LL0:;}
# 663
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 667
return new_e;}
# 670
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 683 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp15E=ka1;enum Cyc_Absyn_KindQual _tmp167;enum Cyc_Absyn_AliasQual _tmp166;_LL1: _tmp167=_tmp15E->kind;_tmp166=_tmp15E->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp15F=ka2;enum Cyc_Absyn_KindQual _tmp165;enum Cyc_Absyn_AliasQual _tmp164;_LL4: _tmp165=_tmp15F->kind;_tmp164=_tmp15F->aliasqual;_LL5:;
# 687
if((int)_tmp167 != (int)_tmp165){
struct _tuple19 _tmp160=({struct _tuple19 _tmp982;_tmp982.f1=_tmp167,_tmp982.f2=_tmp165;_tmp982;});struct _tuple19 _tmp161=_tmp160;switch(_tmp161.f1){case Cyc_Absyn_BoxKind: switch(_tmp161.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp161.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 696
if((int)_tmp166 != (int)_tmp164){
struct _tuple20 _tmp162=({struct _tuple20 _tmp983;_tmp983.f1=_tmp166,_tmp983.f2=_tmp164;_tmp983;});struct _tuple20 _tmp163=_tmp162;switch(_tmp163.f1){case Cyc_Absyn_Aliasable: if(_tmp163.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp163.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 703
return 1;};}
# 706
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp168=Cyc_Absyn_compress_kb(tv->kind);void*_tmp169=_tmp168;struct Cyc_Absyn_Kind*_tmp16C;struct Cyc_Absyn_Kind*_tmp16B;switch(*((int*)_tmp169)){case 0U: _LL1: _tmp16B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp169)->f1;_LL2:
 return _tmp16B;case 2U: _LL3: _tmp16C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169)->f2;_LL4:
 return _tmp16C;default: _LL5: _LL6:
# 711
({void*_tmpA94=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=2U,_tmp16A->f1=0,_tmp16A->f2=def;_tmp16A;});tv->kind=_tmpA94;});
return def;}_LL0:;}
# 716
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp16D=({struct _tuple0 _tmp984;({void*_tmpA96=Cyc_Absyn_compress_kb(kb1);_tmp984.f1=_tmpA96;}),({void*_tmpA95=Cyc_Absyn_compress_kb(kb2);_tmp984.f2=_tmpA95;});_tmp984;});struct _tuple0 _tmp16E=_tmp16D;struct Cyc_Core_Opt**_tmp183;void*_tmp182;void*_tmp181;struct Cyc_Core_Opt**_tmp180;struct Cyc_Core_Opt**_tmp17F;struct Cyc_Absyn_Kind*_tmp17E;struct Cyc_Core_Opt**_tmp17D;struct Cyc_Absyn_Kind*_tmp17C;struct Cyc_Core_Opt**_tmp17B;struct Cyc_Absyn_Kind*_tmp17A;struct Cyc_Absyn_Kind*_tmp179;struct Cyc_Absyn_Kind*_tmp178;struct Cyc_Core_Opt**_tmp177;struct Cyc_Absyn_Kind*_tmp176;struct Cyc_Absyn_Kind*_tmp175;struct Cyc_Absyn_Kind*_tmp174;switch(*((int*)_tmp16E.f1)){case 0U: switch(*((int*)_tmp16E.f2)){case 0U: _LL1: _tmp175=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp174=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_LL2:
 return _tmp175 == _tmp174;case 2U: _LL5: _tmp178=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp177=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_tmp176=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f2;_LL6:
# 725
 if(Cyc_Tcutil_kind_leq(_tmp178,_tmp176)){
({struct Cyc_Core_Opt*_tmpA97=({struct Cyc_Core_Opt*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->v=kb1;_tmp170;});*_tmp177=_tmpA97;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp16E.f2)){case 0U: _LL3: _tmp17B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp17A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f2;_tmp179=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_LL4:
# 720
 if(Cyc_Tcutil_kind_leq(_tmp179,_tmp17A)){
({struct Cyc_Core_Opt*_tmpA98=({struct Cyc_Core_Opt*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->v=kb2;_tmp16F;});*_tmp17B=_tmpA98;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp17F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp17E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f2;_tmp17D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_tmp17C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f2;_LL8:
# 730
 if(Cyc_Tcutil_kind_leq(_tmp17E,_tmp17C)){
({struct Cyc_Core_Opt*_tmpA99=({struct Cyc_Core_Opt*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->v=kb1;_tmp171;});*_tmp17D=_tmpA99;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp17C,_tmp17E)){
({struct Cyc_Core_Opt*_tmpA9A=({struct Cyc_Core_Opt*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->v=kb2;_tmp172;});*_tmp17F=_tmpA9A;});
return 1;}else{
return 0;}}default: _LLB: _tmp181=_tmp16E.f1;_tmp180=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16E.f2)->f1;_LLC:
# 739
({struct Cyc_Core_Opt*_tmpA9B=({struct Cyc_Core_Opt*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->v=_tmp181;_tmp173;});*_tmp180=_tmpA9B;});
return 1;}default: _LL9: _tmp183=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp16E.f1)->f1;_tmp182=_tmp16E.f2;_LLA:
# 737
 _tmp181=_tmp182;_tmp180=_tmp183;goto _LLC;}_LL0:;}
# 744
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp184=Cyc_Tcutil_compress(t);void*_tmp185=_tmp184;struct Cyc_Absyn_Tvar*_tmp18A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185)->tag == 2U){_LL1: _tmp18A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185)->f1;_LL2: {
# 747
void*_tmp186=_tmp18A->kind;
_tmp18A->kind=kb;
return({struct _tuple15 _tmp985;_tmp985.f1=_tmp18A,_tmp985.f2=_tmp186;_tmp985;});}}else{_LL3: _LL4:
# 751
({struct Cyc_String_pa_PrintArg_struct _tmp189=({struct Cyc_String_pa_PrintArg_struct _tmp986;_tmp986.tag=0U,({struct _dyneither_ptr _tmpA9C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp986.f1=_tmpA9C;});_tmp986;});void*_tmp187[1U];_tmp187[0]=& _tmp189;({struct _dyneither_ptr _tmpA9D=({const char*_tmp188="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp188,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA9D,_tag_dyneither(_tmp187,sizeof(void*),1U));});});}_LL0:;}
# 757
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 759
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 763
struct _RegionHandle _tmp18B=_new_region("temp");struct _RegionHandle*temp=& _tmp18B;_push_region(temp);
{struct Cyc_List_List*_tmp18C=0;
# 766
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp18D=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp18E=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp18F=Cyc_Tcutil_tvar_kind(_tmp18D,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp190=_tmp18F;switch(((struct Cyc_Absyn_Kind*)_tmp190)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 772
 _tmp18C=({struct Cyc_List_List*_tmp192=_region_malloc(temp,sizeof(*_tmp192));({struct _tuple15*_tmpA9E=({struct _tuple15*_tmp191=_region_malloc(temp,sizeof(*_tmp191));_tmp191->f1=_tmp18D,_tmp191->f2=_tmp18E;_tmp191;});_tmp192->hd=_tmpA9E;}),_tmp192->tl=_tmp18C;_tmp192;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 776
if(_tmp18C != 0){
field_type=({struct _RegionHandle*_tmpAA0=temp;struct Cyc_List_List*_tmpA9F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp18C);Cyc_Tcutil_rsubstitute(_tmpAA0,_tmpA9F,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 764
;_pop_region(temp);}
# 781
return k;}
# 788
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 790
void*_tmp193=Cyc_Tcutil_compress(t);void*_tmp194=_tmp193;struct Cyc_Absyn_Typedefdecl*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_Absyn_PtrInfo _tmp1B0;void*_tmp1AF;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Tvar*_tmp1AD;struct Cyc_Core_Opt*_tmp1AC;switch(*((int*)_tmp194)){case 1U: _LL1: _tmp1AC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp194)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1AC))->v;case 2U: _LL3: _tmp1AD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp194)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp1AD,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp1AF=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1;_tmp1AE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL6: {
# 794
void*_tmp195=_tmp1AF;enum Cyc_Absyn_AggrKind _tmp1A2;struct Cyc_List_List*_tmp1A1;struct Cyc_Absyn_AggrdeclImpl*_tmp1A0;int _tmp19F;struct Cyc_Absyn_Kind*_tmp19E;enum Cyc_Absyn_Size_of _tmp19D;switch(*((int*)_tmp195)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp19D=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp195)->f2;_LL21:
 return((int)_tmp19D == (int)2U  || (int)_tmp19D == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp19E=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp195)->f2;_LL31:
 return _tmp19E;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 815
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 817
({void*_tmp196=0U;({struct _dyneither_ptr _tmpAA1=({const char*_tmp197="type_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp197,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA1,_tag_dyneither(_tmp196,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 821
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp1A2=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->kind;_tmp1A1=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->tvs;_tmp1A0=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->impl;_tmp19F=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp195)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 823
 if(_tmp1A0 == 0){
if(_tmp19F)
return& Cyc_Tcutil_mk;else{
# 827
return& Cyc_Tcutil_ak;}}{
# 829
struct Cyc_List_List*_tmp198=_tmp1A0->fields;
if(_tmp198 == 0)return& Cyc_Tcutil_mk;
# 832
if((int)_tmp1A2 == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(_tmp198))->tl != 0;_tmp198=_tmp198->tl){;}{
void*_tmp199=((struct Cyc_Absyn_Aggrfield*)_tmp198->hd)->type;
struct Cyc_Absyn_Kind*_tmp19A=Cyc_Tcutil_field_kind(_tmp199,_tmp1AE,_tmp1A1);
if(_tmp19A == & Cyc_Tcutil_ak  || _tmp19A == & Cyc_Tcutil_tak)return _tmp19A;};}else{
# 840
for(0;_tmp198 != 0;_tmp198=_tmp198->tl){
void*_tmp19B=((struct Cyc_Absyn_Aggrfield*)_tmp198->hd)->type;
struct Cyc_Absyn_Kind*_tmp19C=Cyc_Tcutil_field_kind(_tmp19B,_tmp1AE,_tmp1A1);
if(_tmp19C == & Cyc_Tcutil_ak  || _tmp19C == & Cyc_Tcutil_tak)return _tmp19C;}}
# 846
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 848
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp1B0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp194)->f1;_LLC: {
# 851
void*_tmp1A3=Cyc_Tcutil_compress((_tmp1B0.ptr_atts).bounds);void*_tmp1A4=_tmp1A3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)){case 13U: _LL4D: _LL4E: {
# 853
enum Cyc_Absyn_AliasQual _tmp1A5=(Cyc_Tcutil_type_kind((_tmp1B0.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A6=_tmp1A5;switch(_tmp1A6){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 859
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 861
enum Cyc_Absyn_AliasQual _tmp1A7=(Cyc_Tcutil_type_kind((_tmp1B0.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A8=_tmp1A7;switch(_tmp1A8){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 867
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 871
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp1B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp194)->f1).num_elts;_LL12:
# 873
 if(_tmp1B1 == 0  || Cyc_Tcutil_is_const_exp(_tmp1B1))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp1B2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp194)->f3;_LL16:
# 877
 if(_tmp1B2 == 0  || _tmp1B2->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp987;_tmp987.tag=0U,({struct _dyneither_ptr _tmpAA2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp987.f1=_tmpAA2;});_tmp987;});void*_tmp1A9[1U];_tmp1A9[0]=& _tmp1AB;({struct _dyneither_ptr _tmpAA3=({const char*_tmp1AA="type_kind: typedef found: %s";_tag_dyneither(_tmp1AA,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA3,_tag_dyneither(_tmp1A9,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B2->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp194)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 886
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || (int)k1->kind == (int)k2->kind  && (int)k1->aliasqual == (int)k2->aliasqual;}
# 891
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1B3;_push_handler(& _tmp1B3);{int _tmp1B5=0;if(setjmp(_tmp1B3.handler))_tmp1B5=1;if(!_tmp1B5){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1B6=1;_npop_handler(0U);return _tmp1B6;};
# 893
;_pop_handler();}else{void*_tmp1B4=(void*)_exn_thrown;void*_tmp1B7=_tmp1B4;void*_tmp1B8;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1B7)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 895
 return 0;}else{_LL3: _tmp1B8=_tmp1B7;_LL4:(int)_rethrow(_tmp1B8);}_LL0:;}};}
# 900
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1B9=t;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1D8;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D6;struct Cyc_List_List*_tmp1D5;void*_tmp1D4;struct Cyc_Absyn_Tqual _tmp1D3;void*_tmp1D2;struct Cyc_List_List*_tmp1D1;int _tmp1D0;struct Cyc_Absyn_VarargInfo*_tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_List_List*_tmp1C9;void*_tmp1C8;void*_tmp1C7;struct Cyc_Absyn_PtrInfo _tmp1C6;void*_tmp1C5;struct Cyc_Core_Opt**_tmp1C4;struct Cyc_Absyn_Tvar*_tmp1C3;switch(*((int*)_tmp1B9)){case 2U: _LL1: _tmp1C3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B9)->f1;_LL2:
# 905
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1C3)){
Cyc_Tcutil_failure_reason=({const char*_tmp1BA="(type variable would escape scope)";_tag_dyneither(_tmp1BA,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 909
goto _LL0;case 1U: _LL3: _tmp1C5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->f2;_tmp1C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->f4;_LL4:
# 911
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1BB="(occurs check)";_tag_dyneither(_tmp1BB,sizeof(char),15U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 915
if(_tmp1C5 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1C5);else{
# 918
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C4))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 925
if(problem){
struct Cyc_List_List*_tmp1BC=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C4))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1BC=({struct Cyc_List_List*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp1BD->tl=_tmp1BC;_tmp1BD;});}}
# 931
({struct Cyc_Core_Opt*_tmpAA4=({struct Cyc_Core_Opt*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE->v=_tmp1BC;_tmp1BE;});*_tmp1C4=_tmpAA4;});}}}
# 934
goto _LL0;case 3U: _LL5: _tmp1C6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B9)->f1;_LL6:
# 936
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C6.elt_type);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).rgn);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).nullable);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).bounds);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C6.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp1C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B9)->f1).elt_type;_tmp1C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B9)->f1).zero_term;_LL8:
# 944
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C8);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C7);
goto _LL0;case 5U: _LL9: _tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).tvars;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).effect;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ret_tqual;_tmp1D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ret_type;_tmp1D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).args;_tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).c_varargs;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).cyc_varargs;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).rgn_po;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).attributes;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).requires_clause;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).requires_relns;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ensures_clause;_tmp1C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B9)->f1).ensures_relns;_LLA:
# 949
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1D5,env);
if(_tmp1D4 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D4);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D2);
for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1D1->hd)).f3);}
if(_tmp1CF != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1CF->type);
for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){
struct _tuple0*_tmp1BF=(struct _tuple0*)_tmp1CE->hd;struct _tuple0*_tmp1C0=_tmp1BF;void*_tmp1C2;void*_tmp1C1;_LL16: _tmp1C2=_tmp1C0->f1;_tmp1C1=_tmp1C0->f2;_LL17:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1C2);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C1);}
# 961
goto _LL0;case 6U: _LLB: _tmp1D6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B9)->f1;_LLC:
# 963
 for(0;_tmp1D6 != 0;_tmp1D6=_tmp1D6->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1D6->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp1D7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B9)->f2;_LLE:
# 968
 for(0;_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1D7->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp1D8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B9)->f2;_LL10:
 _tmp1D9=_tmp1D8;goto _LL12;case 0U: _LL11: _tmp1D9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f2;_LL12:
# 973
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1D9);goto _LL0;default: _LL13: _LL14:
# 976
 goto _LL0;}_LL0:;};}
# 979
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 981
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 986
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 994
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1DA=0U;({struct _dyneither_ptr _tmpAA5=({const char*_tmp1DB="tq1 real_const not set.";_tag_dyneither(_tmp1DB,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA5,_tag_dyneither(_tmp1DA,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1DC=0U;({struct _dyneither_ptr _tmpAA6=({const char*_tmp1DD="tq2 real_const not set.";_tag_dyneither(_tmp1DD,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAA6,_tag_dyneither(_tmp1DC,sizeof(void*),0U));});});
# 1000
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 1003
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp1DE="(qualifiers don't match)";_tag_dyneither(_tmp1DE,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1011
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 1015
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 1021
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1DF=({struct _tuple0 _tmp988;_tmp988.f1=e1->r,_tmp988.f2=e2->r;_tmp988;});struct _tuple0 _tmp1E0=_tmp1DF;void*_tmp1EA;void*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E7;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->tag == 14U){_LL1: _tmp1E7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->f2;_LL2:
# 1026
 Cyc_Tcutil_unify_cmp_exp(_tmp1E7,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->tag == 14U){_LL3: _tmp1E8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1E8);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->tag == 39U){_LL5: _tmp1E9=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f1)->f1;_LL6: {
# 1029
void*_tmp1E1=Cyc_Tcutil_compress(_tmp1E9);void*_tmp1E2=_tmp1E1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->tag == 1U){_LLC: _LLD:
({void*_tmpAA7=_tmp1E9;Cyc_Tcutil_unify_it(_tmpAA7,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3->tag=9U,_tmp1E3->f1=e2;_tmp1E3;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->tag == 39U){_LL7: _tmp1EA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E0.f2)->f1;_LL8: {
# 1034
void*_tmp1E4=Cyc_Tcutil_compress(_tmp1EA);void*_tmp1E5=_tmp1E4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E5)->tag == 1U){_LL11: _LL12:
({void*_tmpAA8=_tmp1EA;Cyc_Tcutil_unify_it(_tmpAA8,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->tag=9U,_tmp1E6->f1=e1;_tmp1E6;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 1038
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 1042
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp1EB=att;switch(*((int*)_tmp1EB)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 9U: _LL13: _LL14:
 return 9;case 10U: _LL15: _LL16:
 return 10;case 11U: _LL17: _LL18:
 return 11;case 12U: _LL19: _LL1A:
 return 12;case 13U: _LL1B: _LL1C:
 return 13;case 14U: _LL1D: _LL1E:
 return 14;case 15U: _LL1F: _LL20:
 return 15;case 16U: _LL21: _LL22:
 return 16;case 17U: _LL23: _LL24:
 return 17;case 18U: _LL25: _LL26:
 return 18;case 19U: _LL27: _LL28:
 return 19;case 20U: _LL29: _LL2A:
 return 20;default: _LL2B: _LL2C:
 return 21;}_LL0:;}
# 1069
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp1EC=({struct _tuple0 _tmp989;_tmp989.f1=att1,_tmp989.f2=att2;_tmp989;});struct _tuple0 _tmp1ED=_tmp1EC;enum Cyc_Absyn_Format_Type _tmp1FD;int _tmp1FC;int _tmp1FB;enum Cyc_Absyn_Format_Type _tmp1FA;int _tmp1F9;int _tmp1F8;struct _dyneither_ptr _tmp1F7;struct _dyneither_ptr _tmp1F6;struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F4;int _tmp1F3;int _tmp1F2;int _tmp1F1;int _tmp1F0;switch(*((int*)_tmp1ED.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1ED.f2)->tag == 0U){_LL1: _tmp1F1=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1ED.f1)->f1;_tmp1F0=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1ED.f2)->f1;_LL2:
 _tmp1F3=_tmp1F1;_tmp1F2=_tmp1F0;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1ED.f2)->tag == 20U){_LL3: _tmp1F3=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1ED.f1)->f1;_tmp1F2=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1ED.f2)->f1;_LL4:
# 1073
 return Cyc_Core_intcmp(_tmp1F3,_tmp1F2);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1ED.f2)->tag == 6U){_LL5: _tmp1F5=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1ED.f1)->f1;_tmp1F4=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1ED.f2)->f1;_LL6:
# 1075
 if(_tmp1F5 == _tmp1F4)return 0;
if(_tmp1F5 == 0)return - 1;
if(_tmp1F4 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp1F5,_tmp1F4);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1ED.f2)->tag == 8U){_LL7: _tmp1F7=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1ED.f1)->f1;_tmp1F6=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1ED.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1F7,(struct _dyneither_ptr)_tmp1F6);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1ED.f2)->tag == 19U){_LL9: _tmp1FD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1ED.f1)->f1;_tmp1FC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1ED.f1)->f2;_tmp1FB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1ED.f1)->f3;_tmp1FA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1ED.f2)->f1;_tmp1F9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1ED.f2)->f2;_tmp1F8=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1ED.f2)->f3;_LLA: {
# 1081
int _tmp1EE=Cyc_Core_intcmp((int)((unsigned int)_tmp1FD),(int)((unsigned int)_tmp1FA));
if(_tmp1EE != 0)return _tmp1EE;{
int _tmp1EF=Cyc_Core_intcmp(_tmp1FC,_tmp1F9);
if(_tmp1EF != 0)return _tmp1EF;
return Cyc_Core_intcmp(_tmp1FB,_tmp1F8);};}}else{goto _LLB;}default: _LLB: _LLC:
# 1087
 return({int _tmpAA9=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpAA9,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
# 1091
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 1095
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 1104
static void*Cyc_Tcutil_rgns_of(void*t);
# 1106
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 1110
static struct _tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp1FE=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1FF=_tmp1FE;switch(((struct Cyc_Absyn_Kind*)_tmp1FF)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp1FF)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp200=_cycalloc(sizeof(*_tmp200));_tmp200->tag=9U,({struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Absyn_uint_exp(0U,0U);_tmp200->f1=_tmpAAA;});_tmp200;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 1121
return({struct _tuple15*_tmp201=_cycalloc(sizeof(*_tmp201));_tmp201->f1=tv,_tmp201->f2=t;_tmp201;});}
# 1128
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp202=Cyc_Tcutil_compress(t);void*_tmp203=_tmp202;struct Cyc_List_List*_tmp219;struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp217;void*_tmp216;struct Cyc_Absyn_Tqual _tmp215;void*_tmp214;struct Cyc_List_List*_tmp213;struct Cyc_Absyn_VarargInfo*_tmp212;struct Cyc_List_List*_tmp211;struct Cyc_List_List*_tmp210;void*_tmp20F;void*_tmp20E;void*_tmp20D;struct Cyc_List_List*_tmp20C;switch(*((int*)_tmp203)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp203)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp203)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp20C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp203)->f2;_LL6: {
# 1133
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp20C);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 1137
struct Cyc_Absyn_Kind*_tmp204=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp205=_tmp204;switch(((struct Cyc_Absyn_Kind*)_tmp205)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp20E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->f1).elt_type;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp203)->f1).ptr_atts).rgn;_LLC:
# 1145
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp206[2U];({void*_tmpAAC=Cyc_Absyn_access_eff(_tmp20D);_tmp206[0]=_tmpAAC;}),({void*_tmpAAB=Cyc_Tcutil_rgns_of(_tmp20E);_tmp206[1]=_tmpAAB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp206,sizeof(void*),2U));})));case 4U: _LLD: _tmp20F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp203)->f1).elt_type;_LLE:
# 1147
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp20F));case 7U: _LLF: _tmp210=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp203)->f2;_LL10:
# 1149
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp210)));case 5U: _LL11: _tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp203)->f1).tvars;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp203)->f1).effect;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp203)->f1).ret_tqual;_tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp203)->f1).ret_type;_tmp213=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp203)->f1).args;_tmp212=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp203)->f1).cyc_varargs;_tmp211=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp203)->f1).rgn_po;_LL12: {
# 1158
void*_tmp207=({struct Cyc_List_List*_tmpAAD=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp217);Cyc_Tcutil_substitute(_tmpAAD,(void*)_check_null(_tmp216));});
return Cyc_Tcutil_normalize_effect(_tmp207);}case 6U: _LL13: _tmp218=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp203)->f1;_LL14: {
# 1161
struct Cyc_List_List*_tmp208=0;
for(0;_tmp218 != 0;_tmp218=_tmp218->tl){
_tmp208=({struct Cyc_List_List*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209->hd=(*((struct _tuple12*)_tmp218->hd)).f2,_tmp209->tl=_tmp208;_tmp209;});}
_tmp219=_tmp208;goto _LL16;}case 8U: _LL15: _tmp219=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp203)->f2;_LL16:
# 1166
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp219)));case 10U: _LL17: _LL18:
({void*_tmp20A=0U;({struct _dyneither_ptr _tmpAAE=({const char*_tmp20B="typedecl in rgns_of";_tag_dyneither(_tmp20B,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAAE,_tag_dyneither(_tmp20A,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 1176
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp21A=e;void*_tmp226;struct Cyc_List_List**_tmp225;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21A)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21A)->f1)){case 9U: _LL1: _tmp225=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21A)->f2;_LL2: {
# 1180
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp225;for(0;effs != 0;effs=effs->tl){
void*_tmp21B=(void*)effs->hd;
({void*_tmpAAF=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp21B));effs->hd=_tmpAAF;});{
void*_tmp21C=(void*)effs->hd;void*_tmp21D=_tmp21C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2)->tl == 0){_LLE: _LLF:
# 1189
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1193
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp225;for(0;effs != 0;effs=effs->tl){
void*_tmp21E=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp21F=_tmp21E;void*_tmp222;struct Cyc_List_List*_tmp221;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f1)){case 9U: _LL13: _tmp221=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2;_LL14:
# 1198
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp221,effects);
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21F)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp222=_tmp21F;_LL1C:
 effects=({struct Cyc_List_List*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->hd=_tmp222,_tmp220->tl=effects;_tmp220;});goto _LL12;}_LL12:;}}
# 1206
({struct Cyc_List_List*_tmpAB0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp225=_tmpAB0;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21A)->f2 != 0){_LL3: _tmp226=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21A)->f2)->hd;_LL4: {
# 1209
void*_tmp223=Cyc_Tcutil_compress(_tmp226);void*_tmp224=_tmp223;switch(*((int*)_tmp224)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp226);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 1214
 return e;}_LL0:;};}
# 1219
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp227=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp228=_cycalloc(sizeof(*_tmp228));_tmp228->tag=5U,(_tmp228->f1).tvars=0,(_tmp228->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmpAB1=Cyc_Absyn_empty_tqual(0U);(_tmp228->f1).ret_tqual=_tmpAB1;}),(_tmp228->f1).ret_type=Cyc_Absyn_void_type,(_tmp228->f1).args=0,(_tmp228->f1).c_varargs=0,(_tmp228->f1).cyc_varargs=0,(_tmp228->f1).rgn_po=0,(_tmp228->f1).attributes=0,(_tmp228->f1).requires_clause=0,(_tmp228->f1).requires_relns=0,(_tmp228->f1).ensures_clause=0,(_tmp228->f1).ensures_relns=0;_tmp228;});
# 1229
return({void*_tmpAB5=(void*)_tmp227;void*_tmpAB4=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmpAB3=Cyc_Absyn_empty_tqual(0U);void*_tmpAB2=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmpAB5,_tmpAB4,_tmpAB3,_tmpAB2,Cyc_Absyn_false_type);});}
# 1236
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type  || r == Cyc_Absyn_unique_rgn_type) || r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp229=Cyc_Tcutil_compress(e);void*_tmp22A=_tmp229;struct Cyc_Core_Opt*_tmp245;void**_tmp244;struct Cyc_Core_Opt*_tmp243;void*_tmp242;struct Cyc_List_List*_tmp241;void*_tmp240;switch(*((int*)_tmp22A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f2 != 0){_LL1: _tmp240=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f2)->hd;_LL2:
# 1243
 if(constrain)return Cyc_Tcutil_unify(r,_tmp240);
_tmp240=Cyc_Tcutil_compress(_tmp240);
if(r == _tmp240)return 1;{
struct _tuple0 _tmp22B=({struct _tuple0 _tmp98A;_tmp98A.f1=r,_tmp98A.f2=_tmp240;_tmp98A;});struct _tuple0 _tmp22C=_tmp22B;struct Cyc_Absyn_Tvar*_tmp22E;struct Cyc_Absyn_Tvar*_tmp22D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22C.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22C.f2)->tag == 2U){_LLC: _tmp22E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22C.f1)->f1;_tmp22D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22C.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp22E,_tmp22D)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp241=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f2;_LL4:
# 1251
 for(0;_tmp241 != 0;_tmp241=_tmp241->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp241->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f2 != 0){_LL5: _tmp242=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22A)->f2)->hd;_LL6: {
# 1255
void*_tmp22F=Cyc_Tcutil_rgns_of(_tmp242);void*_tmp230=_tmp22F;void*_tmp23A;void*_tmp239;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->f2 != 0){_LL11: _tmp239=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->f2)->hd;_LL12:
# 1257
 if(!constrain)return 0;{
void*_tmp231=Cyc_Tcutil_compress(_tmp239);void*_tmp232=_tmp231;struct Cyc_Core_Opt*_tmp238;void**_tmp237;struct Cyc_Core_Opt*_tmp236;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp232)->tag == 1U){_LL16: _tmp238=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp232)->f1;_tmp237=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp232)->f2;_tmp236=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp232)->f4;_LL17: {
# 1262
void*_tmp233=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp236);
# 1265
Cyc_Tcutil_occurs(_tmp233,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp236))->v,r);{
void*_tmp234=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp235[2U];_tmp235[0]=_tmp233,({void*_tmpAB6=Cyc_Absyn_access_eff(r);_tmp235[1]=_tmpAB6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp235,sizeof(void*),2U));})));
*_tmp237=_tmp234;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp23A=_tmp230;_LL14:
# 1271
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp23A);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp245=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22A)->f1;_tmp244=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22A)->f2;_tmp243=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22A)->f4;_LL8:
# 1274
 if(_tmp245 == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp245->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp23B=0U;({struct _dyneither_ptr _tmpAB7=({const char*_tmp23C="effect evar has wrong kind";_tag_dyneither(_tmp23C,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAB7,_tag_dyneither(_tmp23B,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1279
void*_tmp23D=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp243);
# 1282
Cyc_Tcutil_occurs(_tmp23D,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp243))->v,r);{
void*_tmp23E=Cyc_Absyn_join_eff(({void*_tmp23F[2U];_tmp23F[0]=_tmp23D,({void*_tmpAB8=Cyc_Absyn_access_eff(r);_tmp23F[1]=_tmpAB8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp23F,sizeof(void*),2U));}));
*_tmp244=_tmp23E;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1293
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp246=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp247=_tmp246;struct Cyc_Core_Opt*_tmp259;void**_tmp258;struct Cyc_Core_Opt*_tmp257;void*_tmp256;struct Cyc_List_List*_tmp255;switch(*((int*)_tmp247)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp255=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2;_LL4:
# 1298
 for(0;_tmp255 != 0;_tmp255=_tmp255->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp255->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2 != 0){_LL5: _tmp256=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp247)->f2)->hd;_LL6:
# 1303
 _tmp256=Cyc_Tcutil_compress(_tmp256);
if(t == _tmp256)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp256);{
void*_tmp248=Cyc_Tcutil_rgns_of(t);void*_tmp249=_tmp248;void*_tmp24F;void*_tmp24E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f2 != 0){_LLC: _tmp24E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249)->f2)->hd;_LLD: {
# 1308
struct _tuple0 _tmp24A=({struct _tuple0 _tmp98B;({void*_tmpAB9=Cyc_Tcutil_compress(_tmp24E);_tmp98B.f1=_tmpAB9;}),_tmp98B.f2=_tmp256;_tmp98B;});struct _tuple0 _tmp24B=_tmp24A;struct Cyc_Absyn_Tvar*_tmp24D;struct Cyc_Absyn_Tvar*_tmp24C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24B.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24B.f2)->tag == 2U){_LL11: _tmp24D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24B.f1)->f1;_tmp24C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24B.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp256);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp24E == _tmp256;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp24F=_tmp249;_LLF:
# 1312
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp24F);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp259=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp247)->f1;_tmp258=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp247)->f2;_tmp257=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp247)->f4;_LL8:
# 1315
 if(_tmp259 == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp259->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp250=0U;({struct _dyneither_ptr _tmpABA=({const char*_tmp251="effect evar has wrong kind";_tag_dyneither(_tmp251,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpABA,_tag_dyneither(_tmp250,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1320
void*_tmp252=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp257);
# 1323
Cyc_Tcutil_occurs(_tmp252,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp257))->v,t);{
void*_tmp253=Cyc_Absyn_join_eff(({void*_tmp254[2U];_tmp254[0]=_tmp252,({void*_tmpABB=Cyc_Absyn_regionsof_eff(t);_tmp254[1]=_tmpABB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp254,sizeof(void*),2U));}));
*_tmp258=_tmp253;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1334
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp25A=e;struct Cyc_Core_Opt*_tmp271;void**_tmp270;struct Cyc_Core_Opt*_tmp26F;void*_tmp26E;struct Cyc_List_List*_tmp26D;struct Cyc_Absyn_Tvar*_tmp26C;switch(*((int*)_tmp25A)){case 2U: _LL1: _tmp26C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp25A)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp26C)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->f1)){case 9U: _LL3: _tmp26D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->f2;_LL4:
# 1339
 for(0;_tmp26D != 0;_tmp26D=_tmp26D->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp26D->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->f2 != 0){_LL5: _tmp26E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->f2)->hd;_LL6: {
# 1344
void*_tmp25B=Cyc_Tcutil_rgns_of(_tmp26E);void*_tmp25C=_tmp25B;void*_tmp266;void*_tmp265;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25C)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25C)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25C)->f2 != 0){_LLC: _tmp265=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25C)->f2)->hd;_LLD:
# 1346
 if(!may_constrain_evars)return 0;{
void*_tmp25D=Cyc_Tcutil_compress(_tmp265);void*_tmp25E=_tmp25D;struct Cyc_Core_Opt*_tmp264;void**_tmp263;struct Cyc_Core_Opt*_tmp262;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25E)->tag == 1U){_LL11: _tmp264=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25E)->f1;_tmp263=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25E)->f2;_tmp262=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25E)->f4;_LL12: {
# 1352
void*_tmp25F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp262);
# 1354
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp262))->v,v))return 0;{
void*_tmp260=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp261[2U];_tmp261[0]=_tmp25F,({void*_tmpABC=Cyc_Absyn_var_type(v);_tmp261[1]=_tmpABC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp261,sizeof(void*),2U));})));
*_tmp263=_tmp260;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp266=_tmp25C;_LLF:
# 1360
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp266);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp271=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25A)->f1;_tmp270=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25A)->f2;_tmp26F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25A)->f4;_LL8:
# 1363
 if(_tmp271 == 0  || (int)((struct Cyc_Absyn_Kind*)_tmp271->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp267=0U;({struct _dyneither_ptr _tmpABD=({const char*_tmp268="effect evar has wrong kind";_tag_dyneither(_tmp268,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpABD,_tag_dyneither(_tmp267,sizeof(void*),0U));});});{
# 1367
void*_tmp269=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp26F);
# 1369
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp26F))->v,v))
return 0;{
void*_tmp26A=Cyc_Absyn_join_eff(({void*_tmp26B[2U];_tmp26B[0]=_tmp269,({void*_tmpABE=Cyc_Absyn_var_type(v);_tmp26B[1]=_tmpABE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26B,sizeof(void*),2U));}));
*_tmp270=_tmp26A;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1379
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp272=e;void*_tmp278;struct Cyc_List_List*_tmp277;switch(*((int*)_tmp272)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp272)->f1)){case 9U: _LL1: _tmp277=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp272)->f2;_LL2:
# 1383
 for(0;_tmp277 != 0;_tmp277=_tmp277->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp277->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp272)->f2 != 0){_LL3: _tmp278=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp272)->f2)->hd;_LL4: {
# 1388
void*_tmp273=Cyc_Tcutil_rgns_of(_tmp278);void*_tmp274=_tmp273;void*_tmp276;void*_tmp275;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp274)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp274)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp274)->f2 != 0){_LLA: _tmp275=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp274)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp276=_tmp274;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp276);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1392
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1406 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1411
void*_tmp279=Cyc_Tcutil_compress(e1);void*_tmp27A=_tmp279;void**_tmp287;struct Cyc_Core_Opt*_tmp286;struct Cyc_Absyn_Tvar*_tmp285;void*_tmp284;void*_tmp283;struct Cyc_List_List*_tmp282;switch(*((int*)_tmp27A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f1)){case 9U: _LL1: _tmp282=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f2;_LL2:
# 1413
 for(0;_tmp282 != 0;_tmp282=_tmp282->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp282->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f2 != 0){_LL3: _tmp283=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f2)->hd;_LL4:
# 1424
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp283,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp283,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f2 != 0){_LL7: _tmp284=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A)->f2)->hd;_LL8: {
# 1428
void*_tmp27B=Cyc_Tcutil_rgns_of(_tmp284);void*_tmp27C=_tmp27B;void*_tmp27E;void*_tmp27D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27C)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27C)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27C)->f2 != 0){_LLE: _tmp27D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27C)->f2)->hd;_LLF:
# 1433
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp27D,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp27D,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp27E=_tmp27C;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp27E,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp285=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp27A)->f1;_LL6:
# 1426
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp285,e2);case 1U: _LL9: _tmp287=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27A)->f2;_tmp286=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27A)->f4;_LLA:
# 1438
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1442
*_tmp287=Cyc_Absyn_empty_effect;
# 1445
return 1;}else{
# 1447
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp281=({struct Cyc_String_pa_PrintArg_struct _tmp98C;_tmp98C.tag=0U,({struct _dyneither_ptr _tmpABF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp98C.f1=_tmpABF;});_tmp98C;});void*_tmp27F[1U];_tmp27F[0]=& _tmp281;({struct _dyneither_ptr _tmpAC0=({const char*_tmp280="subset_effect: bad effect: %s";_tag_dyneither(_tmp280,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAC0,_tag_dyneither(_tmp27F,sizeof(void*),1U));});});}_LL0:;}
# 1463 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1479
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1481
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp288=(struct _tuple0*)r1->hd;struct _tuple0*_tmp289=_tmp288;void*_tmp28F;void*_tmp28E;_LL1: _tmp28F=_tmp289->f1;_tmp28E=_tmp289->f2;_LL2:;{
int found=_tmp28F == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp28A=(struct _tuple0*)r2->hd;struct _tuple0*_tmp28B=_tmp28A;void*_tmp28D;void*_tmp28C;_LL4: _tmp28D=_tmp28B->f1;_tmp28C=_tmp28B->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp28F,_tmp28D) && Cyc_Tcutil_unify(_tmp28E,_tmp28C)){
found=1;
break;}}}
# 1491
if(!found)return 0;};}}
# 1493
return 1;}
# 1500
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp290=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp291=({struct Cyc_List_List*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->hd=_tmp290,_tmp292->tl=r1;_tmp292;});
if(Cyc_Relations_consistent_relations(_tmp291))return 0;}
# 1506
return 1;}
# 1511
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1517
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1519
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1522
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp293=t;switch(*((int*)_tmp293)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1528
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1531
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1534
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1537
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1526
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1529
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1532
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1535
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1538
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp293)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1527
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1530
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1533
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1536
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1539
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1561
({void*_tmp294=0U;({struct _dyneither_ptr _tmpAC1=({const char*_tmp295="bad con";_tag_dyneither(_tmp295,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAC1,_tag_dyneither(_tmp294,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp293)->f1){case 0U: _LL21: _LL22:
# 1540
 return 16;case 1U: _LL23: _LL24:
 return 17;default: _LL25: _LL26:
 return 18;}case 3U: _LL27: _LL28:
 return 19;case 4U: _LL29: _LL2A:
 return 20;case 5U: _LL2B: _LL2C:
 return 21;case 6U: _LL2D: _LL2E:
 return 22;case 7U: _LL2F: _LL30:
 return 23;case 8U: _LL31: _LL32:
 return 24;case 9U: _LL33: _LL34:
 return 25;case 10U: _LL35: _LL36:
 return 26;case 11U: _LL37: _LL38:
 return 27;case 12U: _LL39: _LL3A:
 return 28;case 14U: _LL3B: _LL3C:
 return 29;case 13U: _LL3D: _LL3E:
 return 30;case 15U: _LL3F: _LL40:
 return 31;case 16U: _LL41: _LL42:
 return 32;case 17U: _LL43: _LL44:
 return 33;case 18U: _LL45: _LL46:
 return 34;case 19U: _LL47: _LL48:
 return 35;default: _LL49: _LL4A:
 return 36;}_LL0:;}
# 1564
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp296=t;void*_tmp297;switch(*((int*)_tmp296)){case 1U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 2;case 3U: _LL5: _LL6:
 return 3;case 4U: _LL7: _LL8:
 return 4;case 5U: _LL9: _LLA:
 return 5;case 6U: _LLB: _LLC:
 return 6;case 7U: _LLD: _LLE:
 return 7;case 8U: _LLF: _LL10:
 return 8;case 9U: _LL11: _LL12:
 return 9;case 10U: _LL13: _LL14:
 return 10;case 11U: _LL15: _LL16:
 return 11;default: _LL17: _tmp297=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp297);}_LL0:;}
# 1581
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);
# 1583
struct _tuple2*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp298=i;struct _tuple2*_tmp29A;struct Cyc_Absyn_Datatypedecl*_tmp299;if((_tmp298.KnownDatatype).tag == 2){_LL1: _tmp299=*(_tmp298.KnownDatatype).val;_LL2:
 return _tmp299->name;}else{_LL3: _tmp29A=((_tmp298.UnknownDatatype).val).name;_LL4:
 return _tmp29A;}_LL0:;}struct _tuple21{struct _tuple2*f1;struct _tuple2*f2;};
# 1590
struct _tuple21 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp29B=i;struct _tuple2*_tmp29F;struct _tuple2*_tmp29E;struct Cyc_Absyn_Datatypedecl*_tmp29D;struct Cyc_Absyn_Datatypefield*_tmp29C;if((_tmp29B.KnownDatatypefield).tag == 2){_LL1: _tmp29D=((_tmp29B.KnownDatatypefield).val).f1;_tmp29C=((_tmp29B.KnownDatatypefield).val).f2;_LL2:
# 1593
 return({struct _tuple21 _tmp98D;_tmp98D.f1=_tmp29D->name,_tmp98D.f2=_tmp29C->name;_tmp98D;});}else{_LL3: _tmp29F=((_tmp29B.UnknownDatatypefield).val).datatype_name;_tmp29E=((_tmp29B.UnknownDatatypefield).val).field_name;_LL4:
# 1595
 return({struct _tuple21 _tmp98E;_tmp98E.f1=_tmp29F,_tmp98E.f2=_tmp29E;_tmp98E;});}_LL0:;}struct _tuple22{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1599
struct _tuple22 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp2A0=i;struct Cyc_Absyn_Aggrdecl*_tmp2A3;enum Cyc_Absyn_AggrKind _tmp2A2;struct _tuple2*_tmp2A1;if((_tmp2A0.UnknownAggr).tag == 1){_LL1: _tmp2A2=((_tmp2A0.UnknownAggr).val).f1;_tmp2A1=((_tmp2A0.UnknownAggr).val).f2;_LL2:
 return({struct _tuple22 _tmp98F;_tmp98F.f1=_tmp2A2,_tmp98F.f2=_tmp2A1;_tmp98F;});}else{_LL3: _tmp2A3=*(_tmp2A0.KnownAggr).val;_LL4:
 return({struct _tuple22 _tmp990;_tmp990.f1=_tmp2A3->kind,_tmp990.f2=_tmp2A3->name;_tmp990;});}_LL0:;}
# 1606
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1612
struct _tuple0 _tmp2A4=({struct _tuple0 _tmp991;_tmp991.f1=t1,_tmp991.f2=t2;_tmp991;});struct _tuple0 _tmp2A5=_tmp2A4;union Cyc_Absyn_AggrInfo _tmp2C3;union Cyc_Absyn_AggrInfo _tmp2C2;union Cyc_Absyn_DatatypeFieldInfo _tmp2C1;union Cyc_Absyn_DatatypeFieldInfo _tmp2C0;union Cyc_Absyn_DatatypeInfo _tmp2BF;union Cyc_Absyn_DatatypeInfo _tmp2BE;struct Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BC;struct _dyneither_ptr _tmp2BB;struct _dyneither_ptr _tmp2BA;struct _tuple2*_tmp2B9;struct _tuple2*_tmp2B8;switch(*((int*)_tmp2A5.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 15U){_LL1: _tmp2B9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f1;_tmp2B8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp2B9,_tmp2B8);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 17U){_LL3: _tmp2BB=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f1;_tmp2BA=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1;_LL4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2BB,(struct _dyneither_ptr)_tmp2BA);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 16U){_LL5: _tmp2BD=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f1;_tmp2BC=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1;_LL6:
# 1616
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp2BD,_tmp2BC);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 18U){_LL7: _tmp2BF=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f1;_tmp2BE=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1;_LL8: {
# 1618
struct _tuple2*q1=Cyc_Tcutil_get_datatype_qvar(_tmp2BF);
struct _tuple2*q2=Cyc_Tcutil_get_datatype_qvar(_tmp2BE);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 19U){_LL9: _tmp2C1=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f1;_tmp2C0=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1;_LLA: {
# 1622
struct _tuple21 _tmp2A6=Cyc_Tcutil_get_datatype_field_qvars(_tmp2C1);struct _tuple21 _tmp2A7=_tmp2A6;struct _tuple2*_tmp2AE;struct _tuple2*_tmp2AD;_LL10: _tmp2AE=_tmp2A7.f1;_tmp2AD=_tmp2A7.f2;_LL11:;{
struct _tuple21 _tmp2A8=Cyc_Tcutil_get_datatype_field_qvars(_tmp2C0);struct _tuple21 _tmp2A9=_tmp2A8;struct _tuple2*_tmp2AC;struct _tuple2*_tmp2AB;_LL13: _tmp2AC=_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;_LL14:;{
int _tmp2AA=Cyc_Absyn_qvar_cmp(_tmp2AE,_tmp2AC);
if(_tmp2AA != 0)return _tmp2AA;
return Cyc_Absyn_qvar_cmp(_tmp2AD,_tmp2AB);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 20U){_LLB: _tmp2C3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f1;_tmp2C2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1;_LLC: {
# 1628
struct _tuple22 _tmp2AF=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C3);struct _tuple22 _tmp2B0=_tmp2AF;enum Cyc_Absyn_AggrKind _tmp2B7;struct _tuple2*_tmp2B6;_LL16: _tmp2B7=_tmp2B0.f1;_tmp2B6=_tmp2B0.f2;_LL17:;{
struct _tuple22 _tmp2B1=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C2);struct _tuple22 _tmp2B2=_tmp2B1;enum Cyc_Absyn_AggrKind _tmp2B5;struct _tuple2*_tmp2B4;_LL19: _tmp2B5=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;_LL1A:;{
int _tmp2B3=Cyc_Absyn_qvar_cmp(_tmp2B6,_tmp2B4);
if(_tmp2B3 != 0)return _tmp2B3;
return(int)_tmp2B7 - (int)_tmp2B5;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}struct _tuple23{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1638
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp2C4=t1;struct Cyc_Core_Opt*_tmp2D2;void**_tmp2D1;struct Cyc_Core_Opt*_tmp2D0;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->tag == 1U){_LL1: _tmp2D2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->f1;_tmp2D1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->f2;_tmp2D0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->f4;_LL2:
# 1649
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2D0))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2C5=Cyc_Tcutil_type_kind(t2);
# 1654
if(Cyc_Tcutil_kind_leq(_tmp2C5,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2D2))->v)){
*_tmp2D1=t2;
return;}else{
# 1658
{void*_tmp2C6=t2;struct Cyc_Absyn_PtrInfo _tmp2CE;void**_tmp2CD;struct Cyc_Core_Opt*_tmp2CC;switch(*((int*)_tmp2C6)){case 1U: _LL6: _tmp2CD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C6)->f2;_tmp2CC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C6)->f4;_LL7: {
# 1661
struct Cyc_List_List*_tmp2C7=(struct Cyc_List_List*)_tmp2D0->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CC))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2C7,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp2C8="(type variable would escape scope)";_tag_dyneither(_tmp2C8,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1668
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2D2->v,_tmp2C5)){
*_tmp2CD=t1;return;}
# 1671
Cyc_Tcutil_failure_reason=({const char*_tmp2C9="(kinds are incompatible)";_tag_dyneither(_tmp2C9,sizeof(char),25U);});
goto _LL5;}case 3U: _LL8: _tmp2CE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C6)->f1;if((int)((struct Cyc_Absyn_Kind*)_tmp2D2->v)->kind == (int)Cyc_Absyn_BoxKind){_LL9: {
# 1677
void*_tmp2CA=Cyc_Tcutil_compress((_tmp2CE.ptr_atts).bounds);
{void*_tmp2CB=_tmp2CA;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CB)->tag == 1U){_LLD: _LLE:
# 1680
({void*_tmpAC2=_tmp2CA;Cyc_Tcutil_unify(_tmpAC2,Cyc_Absyn_bounds_one());});
*_tmp2D1=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1685
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1688
Cyc_Tcutil_failure_reason=({const char*_tmp2CF="(kinds are incompatible)";_tag_dyneither(_tmp2CF,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1691
 goto _LL0;}_LL0:;}
# 1696
{struct _tuple0 _tmp2D3=({struct _tuple0 _tmp997;_tmp997.f1=t2,_tmp997.f2=t1;_tmp997;});struct _tuple0 _tmp2D4=_tmp2D3;struct Cyc_List_List*_tmp362;struct Cyc_Absyn_Typedefdecl*_tmp361;struct Cyc_List_List*_tmp360;struct Cyc_Absyn_Typedefdecl*_tmp35F;enum Cyc_Absyn_AggrKind _tmp35E;struct Cyc_List_List*_tmp35D;enum Cyc_Absyn_AggrKind _tmp35C;struct Cyc_List_List*_tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;struct Cyc_List_List*_tmp358;void*_tmp357;struct Cyc_Absyn_Tqual _tmp356;void*_tmp355;struct Cyc_List_List*_tmp354;int _tmp353;struct Cyc_Absyn_VarargInfo*_tmp352;struct Cyc_List_List*_tmp351;struct Cyc_List_List*_tmp350;struct Cyc_Absyn_Exp*_tmp34F;struct Cyc_List_List*_tmp34E;struct Cyc_Absyn_Exp*_tmp34D;struct Cyc_List_List*_tmp34C;struct Cyc_List_List*_tmp34B;void*_tmp34A;struct Cyc_Absyn_Tqual _tmp349;void*_tmp348;struct Cyc_List_List*_tmp347;int _tmp346;struct Cyc_Absyn_VarargInfo*_tmp345;struct Cyc_List_List*_tmp344;struct Cyc_List_List*_tmp343;struct Cyc_Absyn_Exp*_tmp342;struct Cyc_List_List*_tmp341;struct Cyc_Absyn_Exp*_tmp340;struct Cyc_List_List*_tmp33F;void*_tmp33E;struct Cyc_Absyn_Tqual _tmp33D;struct Cyc_Absyn_Exp*_tmp33C;void*_tmp33B;void*_tmp33A;struct Cyc_Absyn_Tqual _tmp339;struct Cyc_Absyn_Exp*_tmp338;void*_tmp337;struct Cyc_Absyn_Exp*_tmp336;struct Cyc_Absyn_Exp*_tmp335;void*_tmp334;struct Cyc_Absyn_Tqual _tmp333;void*_tmp332;void*_tmp331;void*_tmp330;void*_tmp32F;void*_tmp32E;struct Cyc_Absyn_Tqual _tmp32D;void*_tmp32C;void*_tmp32B;void*_tmp32A;void*_tmp329;struct Cyc_Absyn_Tvar*_tmp328;struct Cyc_Absyn_Tvar*_tmp327;void*_tmp326;struct Cyc_List_List*_tmp325;void*_tmp324;struct Cyc_List_List*_tmp323;switch(*((int*)_tmp2D4.f1)){case 1U: _LL12: _LL13:
# 1699
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f1)->f1)->tag == 9U){_LL14: _LL15:
# 1702
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmp326=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f1)->f1;_tmp325=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f1)->f2;_tmp324=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->f1;_tmp323=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->f2;_LL21:
# 1713
 if(Cyc_Tcutil_tycon_cmp(_tmp326,_tmp324)== 0){
Cyc_Tcutil_unify_list(_tmp325,_tmp323);
return;}else{
# 1717
Cyc_Tcutil_failure_reason=({const char*_tmp2D6="(different type constructors)";_tag_dyneither(_tmp2D6,sizeof(char),30U);});}
goto _LL11;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f1)->f1)){case 8U: _LL18: _LL19:
# 1704
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 1706
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4.f2)->f1)){case 9U: _LL16: _LL17:
# 1703
 goto _LL19;case 8U: _LL1A: _LL1B:
# 1705
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 1708
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp2D5="(effects don't unify)";_tag_dyneither(_tmp2D5,sizeof(char),22U);});
goto _LL11;default: switch(*((int*)_tmp2D4.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp2D4.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 2U){_LL22: _tmp328=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D4.f1)->f1;_tmp327=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D4.f2)->f1;_LL23: {
# 1721
struct _dyneither_ptr*_tmp2D7=_tmp328->name;
struct _dyneither_ptr*_tmp2D8=_tmp327->name;
# 1724
int _tmp2D9=_tmp328->identity;
int _tmp2DA=_tmp327->identity;
if(_tmp2DA == _tmp2D9)return;
Cyc_Tcutil_failure_reason=({const char*_tmp2DB="(variable types are not the same)";_tag_dyneither(_tmp2DB,sizeof(char),34U);});
goto _LL11;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 3U){_LL24: _tmp334=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f1)->f1).elt_type;_tmp333=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f1)->f1).elt_tq;_tmp332=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ptr_atts).rgn;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ptr_atts).nullable;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ptr_atts).bounds;_tmp32F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ptr_atts).zero_term;_tmp32E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f2)->f1).elt_type;_tmp32D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f2)->f1).elt_tq;_tmp32C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ptr_atts).rgn;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ptr_atts).nullable;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ptr_atts).bounds;_tmp329=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ptr_atts).zero_term;_LL25:
# 1732
 Cyc_Tcutil_unify_it(_tmp32E,_tmp334);
Cyc_Tcutil_unify_it(_tmp332,_tmp32C);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
struct _dyneither_ptr _tmp2DC=Cyc_Tcutil_failure_reason;
Cyc_Tcutil_failure_reason=({const char*_tmp2DD="(not both zero terminated)";_tag_dyneither(_tmp2DD,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp329,_tmp32F);
Cyc_Tcutil_unify_tqual(_tmp32D,_tmp32E,_tmp333,_tmp334);
Cyc_Tcutil_failure_reason=({const char*_tmp2DE="(different pointer bounds)";_tag_dyneither(_tmp2DE,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp32A,_tmp330);{
# 1743
void*_tmp2DF=Cyc_Tcutil_compress(_tmp32A);void*_tmp2E0=_tmp2DF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E0)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E0)->f1)->tag == 14U){_LL35: _LL36:
# 1745
 Cyc_Tcutil_failure_reason=_tmp2DC;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 1748
 Cyc_Tcutil_failure_reason=({const char*_tmp2E1="(incompatible pointer types)";_tag_dyneither(_tmp2E1,sizeof(char),29U);});
Cyc_Tcutil_unify_it(_tmp32B,_tmp331);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 9U){_LL26: _tmp336=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D4.f1)->f1;_tmp335=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D4.f2)->f1;_LL27:
# 1754
 if(!Cyc_Evexp_same_const_exp(_tmp336,_tmp335)){
Cyc_Tcutil_failure_reason=({const char*_tmp2E2="(cannot prove expressions are the same)";_tag_dyneither(_tmp2E2,sizeof(char),40U);});
goto _LL11;}
# 1758
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 4U){_LL28: _tmp33E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f1)->f1).elt_type;_tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f1)->f1).tq;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f1)->f1).num_elts;_tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f1)->f1).zero_term;_tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f2)->f1).elt_type;_tmp339=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f2)->f1).tq;_tmp338=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f2)->f1).num_elts;_tmp337=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D4.f2)->f1).zero_term;_LL29:
# 1762
 Cyc_Tcutil_unify_it(_tmp33A,_tmp33E);
Cyc_Tcutil_unify_tqual(_tmp339,_tmp33A,_tmp33D,_tmp33E);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2E3="(not both zero terminated)";_tag_dyneither(_tmp2E3,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp33B,_tmp337);
if(_tmp33C == _tmp338)return;
if(_tmp33C == 0  || _tmp338 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp33C,_tmp338))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp2E4="(different array sizes)";_tag_dyneither(_tmp2E4,sizeof(char),24U);});
goto _LL11;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 5U){_LL2A: _tmp358=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).tvars;_tmp357=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).effect;_tmp356=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ret_tqual;_tmp355=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ret_type;_tmp354=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).args;_tmp353=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).c_varargs;_tmp352=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).cyc_varargs;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).rgn_po;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).attributes;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).requires_clause;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).requires_relns;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ensures_clause;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f1)->f1).ensures_relns;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).tvars;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).effect;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ret_tqual;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ret_type;_tmp347=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).args;_tmp346=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).c_varargs;_tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).cyc_varargs;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).rgn_po;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).attributes;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).requires_clause;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).requires_relns;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ensures_clause;_tmp33F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D4.f2)->f1).ensures_relns;_LL2B: {
# 1777
int done=0;
{struct _RegionHandle _tmp2E5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2E5;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp34B != 0){
if(_tmp358 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2E6="(second function type has too few type variables)";_tag_dyneither(_tmp2E6,sizeof(char),50U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1785
void*_tmp2E7=((struct Cyc_Absyn_Tvar*)_tmp34B->hd)->kind;
void*_tmp2E8=((struct Cyc_Absyn_Tvar*)_tmp358->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2E7,_tmp2E8)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EB=({struct Cyc_String_pa_PrintArg_struct _tmp994;_tmp994.tag=0U,({
struct _dyneither_ptr _tmpAC3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp34B->hd));_tmp994.f1=_tmpAC3;});_tmp994;});struct Cyc_String_pa_PrintArg_struct _tmp2EC=({struct Cyc_String_pa_PrintArg_struct _tmp993;_tmp993.tag=0U,({
struct _dyneither_ptr _tmpAC4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp34B->hd,& Cyc_Tcutil_bk)));_tmp993.f1=_tmpAC4;});_tmp993;});struct Cyc_String_pa_PrintArg_struct _tmp2ED=({struct Cyc_String_pa_PrintArg_struct _tmp992;_tmp992.tag=0U,({
struct _dyneither_ptr _tmpAC5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp358->hd,& Cyc_Tcutil_bk)));_tmp992.f1=_tmpAC5;});_tmp992;});void*_tmp2E9[3U];_tmp2E9[0]=& _tmp2EB,_tmp2E9[1]=& _tmp2EC,_tmp2E9[2]=& _tmp2ED;({struct _dyneither_ptr _tmpAC6=({const char*_tmp2EA="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2EA,sizeof(char),44U);});Cyc_aprintf(_tmpAC6,_tag_dyneither(_tmp2E9,sizeof(void*),3U));});});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1794
inst=({struct Cyc_List_List*_tmp2EF=_region_malloc(rgn,sizeof(*_tmp2EF));({struct _tuple15*_tmpAC8=({struct _tuple15*_tmp2EE=_region_malloc(rgn,sizeof(*_tmp2EE));_tmp2EE->f1=(struct Cyc_Absyn_Tvar*)_tmp358->hd,({void*_tmpAC7=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp34B->hd);_tmp2EE->f2=_tmpAC7;});_tmp2EE;});_tmp2EF->hd=_tmpAC8;}),_tmp2EF->tl=inst;_tmp2EF;});
_tmp34B=_tmp34B->tl;
_tmp358=_tmp358->tl;};}
# 1798
if(_tmp358 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2F0="(second function type has too many type variables)";_tag_dyneither(_tmp2F0,sizeof(char),51U);});
_npop_handler(0U);goto _LL11;}
# 1802
if(inst != 0){
({void*_tmpACB=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));_tmp2F1->tag=5U,(_tmp2F1->f1).tvars=0,(_tmp2F1->f1).effect=_tmp34A,(_tmp2F1->f1).ret_tqual=_tmp349,(_tmp2F1->f1).ret_type=_tmp348,(_tmp2F1->f1).args=_tmp347,(_tmp2F1->f1).c_varargs=_tmp346,(_tmp2F1->f1).cyc_varargs=_tmp345,(_tmp2F1->f1).rgn_po=_tmp344,(_tmp2F1->f1).attributes=_tmp343,(_tmp2F1->f1).requires_clause=_tmp34F,(_tmp2F1->f1).requires_relns=_tmp34E,(_tmp2F1->f1).ensures_clause=_tmp34D,(_tmp2F1->f1).ensures_relns=_tmp34C;_tmp2F1;});Cyc_Tcutil_unify_it(_tmpACB,({
# 1806
struct _RegionHandle*_tmpACA=rgn;struct Cyc_List_List*_tmpAC9=inst;Cyc_Tcutil_rsubstitute(_tmpACA,_tmpAC9,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));
_tmp2F2->tag=5U,(_tmp2F2->f1).tvars=0,(_tmp2F2->f1).effect=_tmp357,(_tmp2F2->f1).ret_tqual=_tmp356,(_tmp2F2->f1).ret_type=_tmp355,(_tmp2F2->f1).args=_tmp354,(_tmp2F2->f1).c_varargs=_tmp353,(_tmp2F2->f1).cyc_varargs=_tmp352,(_tmp2F2->f1).rgn_po=_tmp351,(_tmp2F2->f1).attributes=_tmp350,(_tmp2F2->f1).requires_clause=_tmp342,(_tmp2F2->f1).requires_relns=_tmp341,(_tmp2F2->f1).ensures_clause=_tmp340,(_tmp2F2->f1).ensures_relns=_tmp33F;_tmp2F2;}));}));});
# 1811
done=1;}}
# 1779
;_pop_region(rgn);}
# 1814
if(done)
return;
Cyc_Tcutil_unify_it(_tmp348,_tmp355);
Cyc_Tcutil_unify_tqual(_tmp349,_tmp348,_tmp356,_tmp355);
for(0;_tmp347 != 0  && _tmp354 != 0;(_tmp347=_tmp347->tl,_tmp354=_tmp354->tl)){
struct _tuple10 _tmp2F3=*((struct _tuple10*)_tmp347->hd);struct _tuple10 _tmp2F4=_tmp2F3;struct Cyc_Absyn_Tqual _tmp2FA;void*_tmp2F9;_LL3A: _tmp2FA=_tmp2F4.f2;_tmp2F9=_tmp2F4.f3;_LL3B:;{
struct _tuple10 _tmp2F5=*((struct _tuple10*)_tmp354->hd);struct _tuple10 _tmp2F6=_tmp2F5;struct Cyc_Absyn_Tqual _tmp2F8;void*_tmp2F7;_LL3D: _tmp2F8=_tmp2F6.f2;_tmp2F7=_tmp2F6.f3;_LL3E:;
Cyc_Tcutil_unify_it(_tmp2F9,_tmp2F7);
Cyc_Tcutil_unify_tqual(_tmp2FA,_tmp2F9,_tmp2F8,_tmp2F7);};}
# 1824
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp347 != 0  || _tmp354 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2FB="(function types have different number of arguments)";_tag_dyneither(_tmp2FB,sizeof(char),52U);});
goto _LL11;}
# 1830
if(_tmp346 != _tmp353){
Cyc_Tcutil_failure_reason=({const char*_tmp2FC="(only one function type takes C varargs)";_tag_dyneither(_tmp2FC,sizeof(char),41U);});
goto _LL11;}{
# 1835
int bad_cyc_vararg=0;
{struct _tuple23 _tmp2FD=({struct _tuple23 _tmp995;_tmp995.f1=_tmp345,_tmp995.f2=_tmp352;_tmp995;});struct _tuple23 _tmp2FE=_tmp2FD;struct _dyneither_ptr*_tmp308;struct Cyc_Absyn_Tqual _tmp307;void*_tmp306;int _tmp305;struct _dyneither_ptr*_tmp304;struct Cyc_Absyn_Tqual _tmp303;void*_tmp302;int _tmp301;if(_tmp2FE.f1 == 0){if(_tmp2FE.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmp2FE.f2 == 0){_LL44: _LL45:
# 1840
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp2FF="(only one function type takes varargs)";_tag_dyneither(_tmp2FF,sizeof(char),39U);});
goto _LL3F;}else{_LL46: _tmp308=(_tmp2FE.f1)->name;_tmp307=(_tmp2FE.f1)->tq;_tmp306=(_tmp2FE.f1)->type;_tmp305=(_tmp2FE.f1)->inject;_tmp304=(_tmp2FE.f2)->name;_tmp303=(_tmp2FE.f2)->tq;_tmp302=(_tmp2FE.f2)->type;_tmp301=(_tmp2FE.f2)->inject;_LL47:
# 1844
 Cyc_Tcutil_unify_it(_tmp306,_tmp302);
Cyc_Tcutil_unify_tqual(_tmp307,_tmp306,_tmp303,_tmp302);
if(_tmp305 != _tmp301){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp300="(only one function type injects varargs)";_tag_dyneither(_tmp300,sizeof(char),41U);});}
# 1850
goto _LL3F;}}_LL3F:;}
# 1852
if(bad_cyc_vararg)goto _LL11;{
# 1855
int bad_effect=0;
{struct _tuple0 _tmp309=({struct _tuple0 _tmp996;_tmp996.f1=_tmp34A,_tmp996.f2=_tmp357;_tmp996;});struct _tuple0 _tmp30A=_tmp309;if(_tmp30A.f1 == 0){if(_tmp30A.f2 == 0){_LL49: _LL4A:
 goto _LL48;}else{_LL4B: _LL4C:
 goto _LL4E;}}else{if(_tmp30A.f2 == 0){_LL4D: _LL4E:
 bad_effect=1;goto _LL48;}else{_LL4F: _LL50:
 bad_effect=!({void*_tmpACC=(void*)_check_null(_tmp34A);Cyc_Tcutil_unify_effect(_tmpACC,(void*)_check_null(_tmp357));});goto _LL48;}}_LL48:;}
# 1862
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp30B="(function effects do not match)";_tag_dyneither(_tmp30B,sizeof(char),32U);});
goto _LL11;}
# 1868
if(!Cyc_Tcutil_same_atts(_tmp350,_tmp343)){
Cyc_Tcutil_failure_reason=({const char*_tmp30C="(function types have different attributes)";_tag_dyneither(_tmp30C,sizeof(char),43U);});
goto _LL11;}
# 1872
if(!Cyc_Tcutil_same_rgn_po(_tmp351,_tmp344)){
Cyc_Tcutil_failure_reason=({const char*_tmp30D="(function types have different region lifetime orderings)";_tag_dyneither(_tmp30D,sizeof(char),58U);});
goto _LL11;}
# 1876
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34E,_tmp341)){
Cyc_Tcutil_failure_reason=({const char*_tmp30E="(@requires clauses not equivalent)";_tag_dyneither(_tmp30E,sizeof(char),35U);});
goto _LL11;}
# 1880
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34C,_tmp33F)){
Cyc_Tcutil_failure_reason=({const char*_tmp30F="(@ensures clauses not equivalent)";_tag_dyneither(_tmp30F,sizeof(char),34U);});
goto _LL11;}
# 1884
return;};};}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 6U){_LL2C: _tmp35A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D4.f1)->f1;_tmp359=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D4.f2)->f1;_LL2D:
# 1887
 for(0;_tmp359 != 0  && _tmp35A != 0;(_tmp359=_tmp359->tl,_tmp35A=_tmp35A->tl)){
struct _tuple12 _tmp310=*((struct _tuple12*)_tmp359->hd);struct _tuple12 _tmp311=_tmp310;struct Cyc_Absyn_Tqual _tmp317;void*_tmp316;_LL52: _tmp317=_tmp311.f1;_tmp316=_tmp311.f2;_LL53:;{
struct _tuple12 _tmp312=*((struct _tuple12*)_tmp35A->hd);struct _tuple12 _tmp313=_tmp312;struct Cyc_Absyn_Tqual _tmp315;void*_tmp314;_LL55: _tmp315=_tmp313.f1;_tmp314=_tmp313.f2;_LL56:;
Cyc_Tcutil_unify_it(_tmp316,_tmp314);
Cyc_Tcutil_unify_tqual(_tmp317,_tmp316,_tmp315,_tmp314);};}
# 1893
if(_tmp359 == 0  && _tmp35A == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp318="(tuple types have different numbers of components)";_tag_dyneither(_tmp318,sizeof(char),51U);});
goto _LL11;}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 7U){_LL2E: _tmp35E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D4.f1)->f1;_tmp35D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D4.f1)->f2;_tmp35C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D4.f2)->f1;_tmp35B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D4.f2)->f2;_LL2F:
# 1900
 if((int)_tmp35C != (int)_tmp35E){Cyc_Tcutil_failure_reason=({const char*_tmp319="(struct and union type)";_tag_dyneither(_tmp319,sizeof(char),24U);});goto _LL11;}
for(0;_tmp35B != 0  && _tmp35D != 0;(_tmp35B=_tmp35B->tl,_tmp35D=_tmp35D->tl)){
struct Cyc_Absyn_Aggrfield*_tmp31A=(struct Cyc_Absyn_Aggrfield*)_tmp35B->hd;
struct Cyc_Absyn_Aggrfield*_tmp31B=(struct Cyc_Absyn_Aggrfield*)_tmp35D->hd;
if(Cyc_strptrcmp(_tmp31A->name,_tmp31B->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp31C="(different member names)";_tag_dyneither(_tmp31C,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1908
Cyc_Tcutil_unify_it(_tmp31A->type,_tmp31B->type);
Cyc_Tcutil_unify_tqual(_tmp31A->tq,_tmp31A->type,_tmp31B->tq,_tmp31B->type);
if(!Cyc_Tcutil_same_atts(_tmp31A->attributes,_tmp31B->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp31D="(different attributes on member)";_tag_dyneither(_tmp31D,sizeof(char),33U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1916
if((_tmp31A->width != 0  && _tmp31B->width == 0  || 
_tmp31B->width != 0  && _tmp31A->width == 0) || 
(_tmp31A->width != 0  && _tmp31B->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpACD=(struct Cyc_Absyn_Exp*)_check_null(_tmp31A->width);Cyc_Evexp_same_const_exp(_tmpACD,(struct Cyc_Absyn_Exp*)_check_null(_tmp31B->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp31E="(different bitfield widths on member)";_tag_dyneither(_tmp31E,sizeof(char),38U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1925
if((_tmp31A->requires_clause != 0  && _tmp31B->requires_clause == 0  || 
_tmp31A->requires_clause == 0  && _tmp31B->requires_clause != 0) || 
(_tmp31A->requires_clause == 0  && _tmp31B->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpACE=(struct Cyc_Absyn_Exp*)_check_null(_tmp31A->requires_clause);Cyc_Evexp_same_const_exp(_tmpACE,(struct Cyc_Absyn_Exp*)_check_null(_tmp31B->requires_clause));})){
# 1930
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp31F="(different @requires clauses on member)";_tag_dyneither(_tmp31F,sizeof(char),40U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1936
if(_tmp35B == 0  && _tmp35D == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp320="(different number of members)";_tag_dyneither(_tmp320,sizeof(char),30U);});
goto _LL11;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D4.f2)->tag == 8U){_LL30: _tmp362=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D4.f1)->f2;_tmp361=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D4.f1)->f3;_tmp360=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D4.f2)->f2;_tmp35F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D4.f2)->f3;_LL31:
# 1942
 if(_tmp361 != _tmp35F){
Cyc_Tcutil_failure_reason=({const char*_tmp321="(different abstract typedefs)";_tag_dyneither(_tmp321,sizeof(char),30U);});
goto _LL11;}
# 1946
Cyc_Tcutil_failure_reason=({const char*_tmp322="(type parameters to typedef differ)";_tag_dyneither(_tmp322,sizeof(char),36U);});
Cyc_Tcutil_unify_list(_tmp362,_tmp360);
return;}else{goto _LL32;}default: _LL32: _LL33:
 goto _LL11;}}}_LL11:;}
# 1951
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1954
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpAD0)(void*,void*)=cmp;void*_tmpACF=(void*)_check_null(a1);_tmpAD0(_tmpACF,(void*)_check_null(a2));});}
# 1961
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp363=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp364=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp363,_tmp364);}
# 1967
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp365=tqt1;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36A;_LL1: _tmp36B=_tmp365->f1;_tmp36A=_tmp365->f2;_LL2:;{
struct _tuple12*_tmp366=tqt2;struct Cyc_Absyn_Tqual _tmp369;void*_tmp368;_LL4: _tmp369=_tmp366->f1;_tmp368=_tmp366->f2;_LL5:;{
int _tmp367=Cyc_Tcutil_tqual_cmp(_tmp36B,_tmp369);
if(_tmp367 != 0)return _tmp367;
return Cyc_Tcutil_typecmp(_tmp36A,_tmp368);};};}
# 1975
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp36C=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp36C != 0)return _tmp36C;{
int _tmp36D=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp36D != 0)return _tmp36D;{
int _tmp36E=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp36E != 0)return _tmp36E;{
int _tmp36F=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp36F != 0)return _tmp36F;
_tmp36F=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp36F != 0)return _tmp36F;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1989
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp370=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp370 != 0)return _tmp370;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1998
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp371=({int _tmpAD1=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpAD1,Cyc_Tcutil_type_case_number(t2));});
if(_tmp371 != 0)
return _tmp371;{
# 2007
struct _tuple0 _tmp372=({struct _tuple0 _tmp998;_tmp998.f1=t2,_tmp998.f2=t1;_tmp998;});struct _tuple0 _tmp373=_tmp372;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BC;enum Cyc_Absyn_AggrKind _tmp3BB;struct Cyc_List_List*_tmp3BA;enum Cyc_Absyn_AggrKind _tmp3B9;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*_tmp3B7;struct Cyc_List_List*_tmp3B6;struct Cyc_Absyn_FnInfo _tmp3B5;struct Cyc_Absyn_FnInfo _tmp3B4;void*_tmp3B3;struct Cyc_Absyn_Tqual _tmp3B2;struct Cyc_Absyn_Exp*_tmp3B1;void*_tmp3B0;void*_tmp3AF;struct Cyc_Absyn_Tqual _tmp3AE;struct Cyc_Absyn_Exp*_tmp3AD;void*_tmp3AC;void*_tmp3AB;struct Cyc_Absyn_Tqual _tmp3AA;void*_tmp3A9;void*_tmp3A8;void*_tmp3A7;void*_tmp3A6;void*_tmp3A5;struct Cyc_Absyn_Tqual _tmp3A4;void*_tmp3A3;void*_tmp3A2;void*_tmp3A1;void*_tmp3A0;struct Cyc_Absyn_Tvar*_tmp39F;struct Cyc_Absyn_Tvar*_tmp39E;void*_tmp39D;struct Cyc_List_List*_tmp39C;void*_tmp39B;struct Cyc_List_List*_tmp39A;switch(*((int*)_tmp373.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f2)->tag == 0U){_LL1: _tmp39D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp39C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f1)->f2;_tmp39B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f2)->f1;_tmp39A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f2)->f2;_LL2: {
# 2009
int _tmp374=Cyc_Tcutil_tycon_cmp(_tmp39D,_tmp39B);
if(_tmp374 != 0)return _tmp374;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp39C,_tmp39A);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp373.f2)->tag == 1U){_LL3: _LL4:
# 2013
({void*_tmp375=0U;({struct _dyneither_ptr _tmpAD2=({const char*_tmp376="typecmp: can only compare closed types";_tag_dyneither(_tmp376,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD2,_tag_dyneither(_tmp375,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp373.f2)->tag == 2U){_LL5: _tmp39F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp39E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp373.f2)->f1;_LL6:
# 2017
 return Cyc_Core_intcmp(_tmp39E->identity,_tmp39F->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->tag == 3U){_LL7: _tmp3AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1).elt_type;_tmp3AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1).elt_tq;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1).ptr_atts).rgn;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1).ptr_atts).nullable;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1).ptr_atts).bounds;_tmp3A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f1)->f1).ptr_atts).zero_term;_tmp3A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->f1).elt_type;_tmp3A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->f1).elt_tq;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->f1).ptr_atts).rgn;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->f1).ptr_atts).nullable;_tmp3A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->f1).ptr_atts).bounds;_tmp3A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp373.f2)->f1).ptr_atts).zero_term;_LL8: {
# 2021
int _tmp377=Cyc_Tcutil_typecmp(_tmp3A5,_tmp3AB);
if(_tmp377 != 0)return _tmp377;{
int _tmp378=Cyc_Tcutil_typecmp(_tmp3A3,_tmp3A9);
if(_tmp378 != 0)return _tmp378;{
int _tmp379=Cyc_Tcutil_tqual_cmp(_tmp3A4,_tmp3AA);
if(_tmp379 != 0)return _tmp379;{
int _tmp37A=Cyc_Tcutil_typecmp(_tmp3A1,_tmp3A7);
if(_tmp37A != 0)return _tmp37A;{
int _tmp37B=Cyc_Tcutil_typecmp(_tmp3A0,_tmp3A6);
if(_tmp37B != 0)return _tmp37B;{
int _tmp37C=Cyc_Tcutil_typecmp(_tmp3A1,_tmp3A7);
if(_tmp37C != 0)return _tmp37C;
return Cyc_Tcutil_typecmp(_tmp3A2,_tmp3A8);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f2)->tag == 4U){_LL9: _tmp3B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f1)->f1).elt_type;_tmp3B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f1)->f1).tq;_tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f1)->f1).num_elts;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f1)->f1).zero_term;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f2)->f1).elt_type;_tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f2)->f1).tq;_tmp3AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f2)->f1).num_elts;_tmp3AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373.f2)->f1).zero_term;_LLA: {
# 2037
int _tmp37D=Cyc_Tcutil_tqual_cmp(_tmp3AE,_tmp3B2);
if(_tmp37D != 0)return _tmp37D;{
int _tmp37E=Cyc_Tcutil_typecmp(_tmp3AF,_tmp3B3);
if(_tmp37E != 0)return _tmp37E;{
int _tmp37F=Cyc_Tcutil_typecmp(_tmp3B0,_tmp3AC);
if(_tmp37F != 0)return _tmp37F;
if(_tmp3B1 == _tmp3AD)return 0;
if(_tmp3B1 == 0  || _tmp3AD == 0)
({void*_tmp380=0U;({struct _dyneither_ptr _tmpAD3=({const char*_tmp381="missing expression in array index";_tag_dyneither(_tmp381,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD3,_tag_dyneither(_tmp380,sizeof(void*),0U));});});
# 2047
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B1,_tmp3AD);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp373.f2)->tag == 5U){_LLB: _tmp3B5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp3B4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp373.f2)->f1;_LLC:
# 2050
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp3B5.ret_type,_tmp3B4.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(_tmp3B5.ret_tqual,_tmp3B4.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp382=_tmp3B5.args;
struct Cyc_List_List*_tmp383=_tmp3B4.args;
for(0;_tmp382 != 0  && _tmp383 != 0;(_tmp382=_tmp382->tl,_tmp383=_tmp383->tl)){
struct _tuple10 _tmp384=*((struct _tuple10*)_tmp382->hd);struct _tuple10 _tmp385=_tmp384;struct Cyc_Absyn_Tqual _tmp38B;void*_tmp38A;_LL18: _tmp38B=_tmp385.f2;_tmp38A=_tmp385.f3;_LL19:;{
struct _tuple10 _tmp386=*((struct _tuple10*)_tmp383->hd);struct _tuple10 _tmp387=_tmp386;struct Cyc_Absyn_Tqual _tmp389;void*_tmp388;_LL1B: _tmp389=_tmp387.f2;_tmp388=_tmp387.f3;_LL1C:;
r=Cyc_Tcutil_tqual_cmp(_tmp38B,_tmp389);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp38A,_tmp388);
if(r != 0)return r;};}
# 2065
if(_tmp382 != 0)return 1;
if(_tmp383 != 0)return - 1;
if(_tmp3B5.c_varargs  && !_tmp3B4.c_varargs)return 1;
if(!_tmp3B5.c_varargs  && _tmp3B4.c_varargs)return - 1;
if(_tmp3B5.cyc_varargs != 0 & _tmp3B4.cyc_varargs == 0)return 1;
if(_tmp3B5.cyc_varargs == 0 & _tmp3B4.cyc_varargs != 0)return - 1;
if(_tmp3B5.cyc_varargs != 0 & _tmp3B4.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmpAD4=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B5.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmpAD4,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B4.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((_tmp3B5.cyc_varargs)->type,(_tmp3B4.cyc_varargs)->type);
if(r != 0)return r;
if((_tmp3B5.cyc_varargs)->inject  && !(_tmp3B4.cyc_varargs)->inject)return 1;
if(!(_tmp3B5.cyc_varargs)->inject  && (_tmp3B4.cyc_varargs)->inject)return - 1;}
# 2079
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp3B5.effect,_tmp3B4.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp38C=_tmp3B5.rgn_po;
struct Cyc_List_List*_tmp38D=_tmp3B4.rgn_po;
for(0;_tmp38C != 0  && _tmp38D != 0;(_tmp38C=_tmp38C->tl,_tmp38D=_tmp38D->tl)){
struct _tuple0 _tmp38E=*((struct _tuple0*)_tmp38C->hd);struct _tuple0 _tmp38F=_tmp38E;void*_tmp395;void*_tmp394;_LL1E: _tmp395=_tmp38F.f1;_tmp394=_tmp38F.f2;_LL1F:;{
struct _tuple0 _tmp390=*((struct _tuple0*)_tmp38D->hd);struct _tuple0 _tmp391=_tmp390;void*_tmp393;void*_tmp392;_LL21: _tmp393=_tmp391.f1;_tmp392=_tmp391.f2;_LL22:;
r=Cyc_Tcutil_typecmp(_tmp395,_tmp393);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp394,_tmp392);if(r != 0)return r;};}
# 2089
if(_tmp38C != 0)return 1;
if(_tmp38D != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B5.requires_clause,_tmp3B4.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B5.ensures_clause,_tmp3B4.ensures_clause);
if(r != 0)return r;
# 2097
({void*_tmp396=0U;({struct _dyneither_ptr _tmpAD5=({const char*_tmp397="typecmp: function type comparison should never get here!";_tag_dyneither(_tmp397,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD5,_tag_dyneither(_tmp396,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp373.f2)->tag == 6U){_LLD: _tmp3B7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp3B6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp373.f2)->f1;_LLE:
# 2100
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3B6,_tmp3B7);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp373.f2)->tag == 7U){_LLF: _tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp3BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp373.f1)->f2;_tmp3B9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp373.f2)->f1;_tmp3B8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp373.f2)->f2;_LL10:
# 2103
 if((int)_tmp3B9 != (int)_tmp3BB){
if((int)_tmp3B9 == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3B8,_tmp3BA);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp373.f2)->tag == 9U){_LL11: _tmp3BD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp3BC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp373.f2)->f1;_LL12:
# 2108
 _tmp3BF=_tmp3BD;_tmp3BE=_tmp3BC;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp373.f2)->tag == 11U){_LL13: _tmp3BF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp373.f1)->f1;_tmp3BE=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp373.f2)->f1;_LL14:
# 2110
 return Cyc_Evexp_const_exp_cmp(_tmp3BF,_tmp3BE);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp398=0U;({struct _dyneither_ptr _tmpAD6=({const char*_tmp399="Unmatched case in typecmp";_tag_dyneither(_tmp399,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAD6,_tag_dyneither(_tmp398,sizeof(void*),0U));});});}_LL0:;};};}
# 2117
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp3C0=({struct _tuple0 _tmp99A;_tmp99A.f1=t1,_tmp99A.f2=t2;_tmp99A;});struct _tuple0 _tmp3C1=_tmp3C0;void*_tmp3C7;void*_tmp3C6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C1.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C1.f2)->tag == 0U){_LL1: _tmp3C7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C1.f1)->f1;_tmp3C6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C1.f2)->f1;_LL2: {
# 2122
struct _tuple0 _tmp3C2=({struct _tuple0 _tmp999;_tmp999.f1=_tmp3C7,_tmp999.f2=_tmp3C6;_tmp999;});struct _tuple0 _tmp3C3=_tmp3C2;int _tmp3C5;int _tmp3C4;switch(*((int*)_tmp3C3.f1)){case 2U: switch(*((int*)_tmp3C3.f2)){case 2U: _LL6: _tmp3C5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C3.f1)->f1;_tmp3C4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f1;_LL7:
# 2124
 return _tmp3C4 < _tmp3C5;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp3C3.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 2131
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 2136
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 2139
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f1 == 0){_LL14: _LL15:
# 2134
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp3C3.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 2132
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 2137
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 2140
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f1 == 0){_LL16: _LL17:
# 2135
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 2141
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C3.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 2138
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 2142
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 2144
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2146
 return 0;}_LL0:;};}
# 2152
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2155
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->v=t1;_tmp3C8;});}}}
# 2164
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2168
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CB=({struct Cyc_String_pa_PrintArg_struct _tmp99C;_tmp99C.tag=0U,({
struct _dyneither_ptr _tmpAD7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp99C.f1=_tmpAD7;});_tmp99C;});struct Cyc_String_pa_PrintArg_struct _tmp3CC=({struct Cyc_String_pa_PrintArg_struct _tmp99B;_tmp99B.tag=0U,({struct _dyneither_ptr _tmpAD8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp99B.f1=_tmpAD8;});_tmp99B;});void*_tmp3C9[2U];_tmp3C9[0]=& _tmp3CB,_tmp3C9[1]=& _tmp3CC;({unsigned int _tmpADA=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpAD9=({const char*_tmp3CA="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp3CA,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpADA,_tmpAD9,_tag_dyneither(_tmp3C9,sizeof(void*),2U));});});
return 0;}}}
# 2174
return 1;}
# 2179
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(te,e)){
void*_tmp3CD=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3CE=_tmp3CD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CE)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2185
return 1;}
# 2189
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 2193
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp3CF=0U;({unsigned int _tmpADC=e->loc;struct _dyneither_ptr _tmpADB=({const char*_tmp3D0="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D0,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpADC,_tmpADB,_tag_dyneither(_tmp3CF,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2199
return 0;}
# 2203
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 2207
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp3D1=0U;({unsigned int _tmpADE=e->loc;struct _dyneither_ptr _tmpADD=({const char*_tmp3D2="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D2,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpADE,_tmpADD,_tag_dyneither(_tmp3D1,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2213
return 0;}
# 2218
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Tcutil_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 2224
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmpADF=t;Cyc_Tcutil_unify(_tmpADF,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 2229
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp3D3=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp3D4=_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D6;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D4)->tag == 9U){_LL1: _tmp3D6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D4)->f1;_LL2:
 return _tmp3D6;}else{_LL3: _LL4: {
# 2234
struct Cyc_Absyn_Exp*_tmp3D5=Cyc_Absyn_valueof_exp(_tmp3D3,0U);
_tmp3D5->topt=Cyc_Absyn_uint_type;
return _tmp3D5;}}_LL0:;}
# 2243
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Tcutil_unify(def,b);{
void*_tmp3D7=Cyc_Tcutil_compress(b);void*_tmp3D8=_tmp3D7;struct Cyc_List_List*_tmp3DC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D8)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D8)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp3DC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D8)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp3DC);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp3DB=({struct Cyc_String_pa_PrintArg_struct _tmp99D;_tmp99D.tag=0U,({struct _dyneither_ptr _tmpAE0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(b));_tmp99D.f1=_tmpAE0;});_tmp99D;});void*_tmp3D9[1U];_tmp3D9[0]=& _tmp3DB;({struct _dyneither_ptr _tmpAE1=({const char*_tmp3DA="get_bounds_exp: %s";_tag_dyneither(_tmp3DA,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE1,_tag_dyneither(_tmp3D9,sizeof(void*),1U));});});}_LL0:;};}
# 2252
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp3DD=Cyc_Tcutil_compress(t);void*_tmp3DE=_tmp3DD;void*_tmp3E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->tag == 3U){_LL1: _tmp3E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DE)->f1).ptr_atts).bounds;_LL2:
# 2255
 return Cyc_Tcutil_get_bounds_exp(def,_tmp3E2);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp3E1=({struct Cyc_String_pa_PrintArg_struct _tmp99E;_tmp99E.tag=0U,({struct _dyneither_ptr _tmpAE2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp99E.f1=_tmpAE2;});_tmp99E;});void*_tmp3DF[1U];_tmp3DF[0]=& _tmp3E1;({struct _dyneither_ptr _tmpAE3=({const char*_tmp3E0="get_ptr_bounds_exp not pointer: %s";_tag_dyneither(_tmp3E0,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE3,_tag_dyneither(_tmp3DF,sizeof(void*),1U));});});}_LL0:;}
# 2261
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));({struct Cyc_List_List*_tmpAE4=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E3->v=_tmpAE4;});_tmp3E3;}));else{
# 2265
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->v=0;_tmp3E4;}));}}
# 2268
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));({struct Cyc_List_List*_tmpAE5=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E5->v=_tmpAE5;});_tmp3E5;}));else{
# 2272
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->v=0;_tmp3E6;}));}}
# 2276
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple24{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 2284
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2286
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp3E7=({struct _tuple0 _tmp9A2;_tmp9A2.f1=t1,_tmp9A2.f2=t2;_tmp9A2;});struct _tuple0 _tmp3E8=_tmp3E7;void*_tmp3FD;struct Cyc_Absyn_Tqual _tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;void*_tmp3FA;void*_tmp3F9;struct Cyc_Absyn_Tqual _tmp3F8;struct Cyc_Absyn_Exp*_tmp3F7;void*_tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F5;struct Cyc_Absyn_PtrInfo _tmp3F4;switch(*((int*)_tmp3E8.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E8.f2)->tag == 3U){_LL1: _tmp3F5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E8.f1)->f1;_tmp3F4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E8.f2)->f1;_LL2: {
# 2290
int okay=1;
# 2292
if(!Cyc_Tcutil_unify((_tmp3F5.ptr_atts).nullable,(_tmp3F4.ptr_atts).nullable))
# 2294
okay=!Cyc_Tcutil_force_type2bool(0,(_tmp3F5.ptr_atts).nullable);
# 2296
if(!Cyc_Tcutil_unify((_tmp3F5.ptr_atts).bounds,(_tmp3F4.ptr_atts).bounds)){
struct _tuple24 _tmp3E9=({struct _tuple24 _tmp99F;({struct Cyc_Absyn_Exp*_tmpAE9=({void*_tmpAE8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpAE8,(_tmp3F5.ptr_atts).bounds);});_tmp99F.f1=_tmpAE9;}),({
struct Cyc_Absyn_Exp*_tmpAE7=({void*_tmpAE6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpAE6,(_tmp3F4.ptr_atts).bounds);});_tmp99F.f2=_tmpAE7;});_tmp99F;});
# 2297
struct _tuple24 _tmp3EA=_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3ED;if(_tmp3EA.f2 == 0){_LLA: _LLB:
# 2300
 okay=1;goto _LL9;}else{if(_tmp3EA.f1 == 0){_LLC: _LLD:
# 2303
 if(Cyc_Tcutil_force_type2bool(0,(_tmp3F5.ptr_atts).zero_term) && ({
void*_tmpAEA=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpAEA,(_tmp3F4.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp3EE=_tmp3EA.f1;_tmp3ED=_tmp3EA.f2;_LLF:
# 2309
 okay=okay  && ({struct Cyc_Absyn_Exp*_tmpAEB=(struct Cyc_Absyn_Exp*)_check_null(_tmp3ED);Cyc_Evexp_lte_const_exp(_tmpAEB,(struct Cyc_Absyn_Exp*)_check_null(_tmp3EE));});
# 2313
if(!Cyc_Tcutil_force_type2bool(0,(_tmp3F4.ptr_atts).zero_term))
({void*_tmp3EB=0U;({unsigned int _tmpAED=loc;struct _dyneither_ptr _tmpAEC=({const char*_tmp3EC="implicit cast to shorter array";_tag_dyneither(_tmp3EC,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpAED,_tmpAEC,_tag_dyneither(_tmp3EB,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 2320
okay=okay  && (!(_tmp3F5.elt_tq).real_const  || (_tmp3F4.elt_tq).real_const);
# 2323
if(!Cyc_Tcutil_unify((_tmp3F5.ptr_atts).rgn,(_tmp3F4.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp3F5.ptr_atts).rgn,(_tmp3F4.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp3F1=({struct Cyc_String_pa_PrintArg_struct _tmp9A1;_tmp9A1.tag=0U,({
struct _dyneither_ptr _tmpAEE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F5.ptr_atts).rgn));_tmp9A1.f1=_tmpAEE;});_tmp9A1;});struct Cyc_String_pa_PrintArg_struct _tmp3F2=({struct Cyc_String_pa_PrintArg_struct _tmp9A0;_tmp9A0.tag=0U,({
struct _dyneither_ptr _tmpAEF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F4.ptr_atts).rgn));_tmp9A0.f1=_tmpAEF;});_tmp9A0;});void*_tmp3EF[2U];_tmp3EF[0]=& _tmp3F1,_tmp3EF[1]=& _tmp3F2;({unsigned int _tmpAF1=loc;struct _dyneither_ptr _tmpAF0=({const char*_tmp3F0="implicit cast from region %s to region %s";_tag_dyneither(_tmp3F0,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpAF1,_tmpAF0,_tag_dyneither(_tmp3EF,sizeof(void*),2U));});});}else{
okay=0;}}
# 2333
okay=okay  && (Cyc_Tcutil_unify((_tmp3F5.ptr_atts).zero_term,(_tmp3F4.ptr_atts).zero_term) || 
# 2335
Cyc_Tcutil_force_type2bool(1,(_tmp3F5.ptr_atts).zero_term) && (_tmp3F4.elt_tq).real_const);{
# 2343
int _tmp3F3=
({void*_tmpAF2=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpAF2,(_tmp3F4.ptr_atts).bounds);}) && !
Cyc_Tcutil_force_type2bool(0,(_tmp3F4.ptr_atts).zero_term);
# 2349
okay=okay  && (Cyc_Tcutil_unify(_tmp3F5.elt_type,_tmp3F4.elt_type) || 
(_tmp3F3  && ((_tmp3F4.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3F4.elt_type)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp3F5.elt_type,_tmp3F4.elt_type));
# 2352
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f2)->tag == 4U){_LL3: _tmp3FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f1)->f1).elt_type;_tmp3FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f1)->f1).tq;_tmp3FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f1)->f1).num_elts;_tmp3FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f1)->f1).zero_term;_tmp3F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f2)->f1).elt_type;_tmp3F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f2)->f1).tq;_tmp3F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f2)->f1).num_elts;_tmp3F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E8.f2)->f1).zero_term;_LL4: {
# 2356
int okay;
# 2359
okay=Cyc_Tcutil_unify(_tmp3FA,_tmp3F6) && (
(_tmp3FB != 0  && _tmp3F7 != 0) && Cyc_Evexp_same_const_exp(_tmp3FB,_tmp3F7));
# 2362
return(okay  && Cyc_Tcutil_unify(_tmp3FD,_tmp3F9)) && (!_tmp3FC.real_const  || _tmp3F8.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E8.f2)->f1)->tag == 1U){_LL5: _LL6:
# 2364
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 2366
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2370
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp3FE=Cyc_Tcutil_compress(t);void*_tmp3FF=_tmp3FE;void*_tmp402;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FF)->tag == 3U){_LL1: _tmp402=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FF)->f1).elt_type;_LL2:
 return _tmp402;}else{_LL3: _LL4:
({void*_tmp400=0U;({struct _dyneither_ptr _tmpAF3=({const char*_tmp401="pointer_elt_type";_tag_dyneither(_tmp401,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF3,_tag_dyneither(_tmp400,sizeof(void*),0U));});});}_LL0:;}
# 2376
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp403=Cyc_Tcutil_compress(t);void*_tmp404=_tmp403;struct Cyc_Absyn_PtrAtts*_tmp407;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404)->tag == 3U){_LL1: _tmp407=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404)->f1).ptr_atts;_LL2:
 return _tmp407->rgn;}else{_LL3: _LL4:
({void*_tmp405=0U;({struct _dyneither_ptr _tmpAF4=({const char*_tmp406="pointer_elt_type";_tag_dyneither(_tmp406,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF4,_tag_dyneither(_tmp405,sizeof(void*),0U));});});}_LL0:;}
# 2383
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp408=Cyc_Tcutil_compress(t);void*_tmp409=_tmp408;void*_tmp40A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409)->tag == 3U){_LL1: _tmp40A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409)->f1).ptr_atts).rgn;_LL2:
# 2386
*rgn=_tmp40A;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2395
static int Cyc_Tcutil_admits_zero(void*t){
void*_tmp40B=Cyc_Tcutil_compress(t);void*_tmp40C=_tmp40B;void*_tmp40E;void*_tmp40D;switch(*((int*)_tmp40C)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40C)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp40E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).ptr_atts).nullable;_tmp40D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).ptr_atts).bounds;_LL6:
# 2403
 return !Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp40D) && Cyc_Tcutil_force_type2bool(0,_tmp40E);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2409
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp40F=e->r;void*_tmp410=_tmp40F;void*_tmp414;struct Cyc_Absyn_Exp*_tmp413;struct _dyneither_ptr _tmp412;switch(*((int*)_tmp410)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp410)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp410)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp410)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp410)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp410)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp412=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp410)->f1).Wchar_c).val;_LLC: {
# 2418
unsigned long _tmp411=Cyc_strlen((struct _dyneither_ptr)_tmp412);
int i=0;
if(_tmp411 >= (unsigned long)2  && (int)*((const char*)_check_dyneither_subscript(_tmp412,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_dyneither_subscript(_tmp412,sizeof(char),1))== (int)'0')i=2;else{
if(((int)*((const char*)_check_dyneither_subscript(_tmp412,sizeof(char),1))== (int)'x'  && _tmp411 >= (unsigned long)3) && (int)*((const char*)_check_dyneither_subscript(_tmp412,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp411;++ i){
if((int)*((const char*)_check_dyneither_subscript(_tmp412,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 2428
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2416
 return 1;case 14U: _LLD: _tmp414=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp410)->f1;_tmp413=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp410)->f2;_LLE:
# 2429
 return Cyc_Tcutil_is_zero(_tmp413) && Cyc_Tcutil_admits_zero(_tmp414);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2434
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 2443
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2448
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2453
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 2462
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2467
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2472
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp415=ka;enum Cyc_Absyn_KindQual _tmp41E;enum Cyc_Absyn_AliasQual _tmp41D;_LL1: _tmp41E=_tmp415->kind;_tmp41D=_tmp415->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp416=_tmp41D;switch(_tmp416){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2476
enum Cyc_Absyn_KindQual _tmp417=_tmp41E;switch(_tmp417){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2487
{enum Cyc_Absyn_KindQual _tmp418=_tmp41E;switch(_tmp418){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 2494
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 2496
{enum Cyc_Absyn_KindQual _tmp419=_tmp41E;switch(_tmp419){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 2503
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 2506
({struct Cyc_String_pa_PrintArg_struct _tmp41C=({struct Cyc_String_pa_PrintArg_struct _tmp9A3;_tmp9A3.tag=0U,({struct _dyneither_ptr _tmpAF5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp9A3.f1=_tmpAF5;});_tmp9A3;});void*_tmp41A[1U];_tmp41A[0]=& _tmp41C;({struct _dyneither_ptr _tmpAF6=({const char*_tmp41B="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp41B,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF6,_tag_dyneither(_tmp41A,sizeof(void*),1U));});});}
# 2509
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->tag=0U,_tmp41F->f1=k;_tmp41F;});}
# 2512
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp420=_cycalloc(sizeof(*_tmp420));({void*_tmpAF7=Cyc_Tcutil_kind_to_bound(k);_tmp420->v=_tmpAF7;});_tmp420;});}
# 2518
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpAF8=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->tag=0U,_tmp421->f1=Cyc_Absyn_Null_c;_tmp421;});e1->r=_tmpAF8;});{
struct Cyc_Core_Opt*_tmp422=Cyc_Tcenv_lookup_opt_type_vars(te);
void*_tmp423=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9A6;({void*_tmpAFD=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp422);_tmp9A6.elt_type=_tmpAFD;}),({
struct Cyc_Absyn_Tqual _tmpAFC=Cyc_Absyn_empty_tqual(0U);_tmp9A6.elt_tq=_tmpAFC;}),
({void*_tmpAFB=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp422);(_tmp9A6.ptr_atts).rgn=_tmpAFB;}),(_tmp9A6.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 2526
void*_tmpAFA=Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,_tmp422);(_tmp9A6.ptr_atts).bounds=_tmpAFA;}),({
void*_tmpAF9=Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,_tmp422);(_tmp9A6.ptr_atts).zero_term=_tmpAF9;}),(_tmp9A6.ptr_atts).ptrloc=0;_tmp9A6;}));
e1->topt=_tmp423;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp426=({struct Cyc_String_pa_PrintArg_struct _tmp9A5;_tmp9A5.tag=0U,({
struct _dyneither_ptr _tmpAFE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9A5.f1=_tmpAFE;});_tmp9A5;});struct Cyc_String_pa_PrintArg_struct _tmp427=({struct Cyc_String_pa_PrintArg_struct _tmp9A4;_tmp9A4.tag=0U,({struct _dyneither_ptr _tmpAFF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmp9A4.f1=_tmpAFF;});_tmp9A4;});void*_tmp424[2U];_tmp424[0]=& _tmp426,_tmp424[1]=& _tmp427;({struct _dyneither_ptr _tmpB00=({const char*_tmp425="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp425,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB00,_tag_dyneither(_tmp424,sizeof(void*),2U));});});
return retv;};};}
# 2536
return 0;}
# 2539
int Cyc_Tcutil_warn_alias_coerce=0;
# 2545
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2549
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB03=({struct _dyneither_ptr*_tmp43A=_cycalloc(sizeof(*_tmp43A));({struct _dyneither_ptr _tmpB02=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp439=({struct Cyc_Int_pa_PrintArg_struct _tmp9A8;_tmp9A8.tag=1U,_tmp9A8.f1=(unsigned long)counter ++;_tmp9A8;});void*_tmp437[1U];_tmp437[0]=& _tmp439;({struct _dyneither_ptr _tmpB01=({const char*_tmp438="__aliasvar%d";_tag_dyneither(_tmp438,sizeof(char),13U);});Cyc_aprintf(_tmpB01,_tag_dyneither(_tmp437,sizeof(void*),1U));});});*_tmp43A=_tmpB02;});_tmp43A;});_tmp43B->f2=_tmpB03;});_tmp43B;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpB04=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->tag=4U,_tmp436->f1=vd;_tmp436;});Cyc_Absyn_varb_exp(_tmpB04,e->loc);});
# 2558
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2560
{void*_tmp428=Cyc_Tcutil_compress(e_type);void*_tmp429=_tmp428;void*_tmp435;struct Cyc_Absyn_Tqual _tmp434;void*_tmp433;void*_tmp432;void*_tmp431;void*_tmp430;struct Cyc_Absyn_PtrLoc*_tmp42F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->tag == 3U){_LL1: _tmp435=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).elt_type;_tmp434=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).elt_tq;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).ptr_atts).rgn;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).ptr_atts).nullable;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).ptr_atts).bounds;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).ptr_atts).zero_term;_tmp42F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429)->f1).ptr_atts).ptrloc;_LL2:
# 2562
{void*_tmp42A=Cyc_Tcutil_compress(_tmp433);void*_tmp42B=_tmp42A;void**_tmp42E;struct Cyc_Core_Opt*_tmp42D;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42B)->tag == 1U){_LL6: _tmp42E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42B)->f2;_tmp42D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42B)->f4;_LL7: {
# 2564
void*_tmp42C=Cyc_Absyn_var_type(tv);
*_tmp42E=_tmp42C;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2569
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2573
e->topt=0;
vd->initializer=0;{
# 2577
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2579
return({struct _tuple14 _tmp9A7;_tmp9A7.f1=d,_tmp9A7.f2=ve;_tmp9A7;});};}
# 2584
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned int loc){
# 2587
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_type,0)) && 
Cyc_Tcutil_is_pointer_type(e_type)){
# 2592
void*_tmp43C=Cyc_Tcutil_compress(wants_type);void*_tmp43D=_tmp43C;void*_tmp43F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->tag == 3U){_LL1: _tmp43F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43D)->f1).ptr_atts).rgn;_LL2:
# 2594
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp43F))return 0;{
struct Cyc_Absyn_Kind*_tmp43E=Cyc_Tcutil_type_kind(_tmp43F);
return(int)_tmp43E->kind == (int)Cyc_Absyn_RgnKind  && (int)_tmp43E->aliasqual == (int)Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2600
return 0;}
# 2604
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2609
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2611
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2613
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp442=({struct Cyc_String_pa_PrintArg_struct _tmp9AA;_tmp9AA.tag=0U,({
struct _dyneither_ptr _tmpB05=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9AA.f1=_tmpB05;});_tmp9AA;});struct Cyc_String_pa_PrintArg_struct _tmp443=({struct Cyc_String_pa_PrintArg_struct _tmp9A9;_tmp9A9.tag=0U,({struct _dyneither_ptr _tmpB06=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9A9.f1=_tmpB06;});_tmp9A9;});void*_tmp440[2U];_tmp440[0]=& _tmp442,_tmp440[1]=& _tmp443;({unsigned int _tmpB08=e->loc;struct _dyneither_ptr _tmpB07=({const char*_tmp441="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp441,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpB08,_tmpB07,_tag_dyneither(_tmp440,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2621
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp446=({struct Cyc_String_pa_PrintArg_struct _tmp9AD;_tmp9AD.tag=0U,({
struct _dyneither_ptr _tmpB09=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp9AD.f1=_tmpB09;});_tmp9AD;});struct Cyc_String_pa_PrintArg_struct _tmp447=({struct Cyc_String_pa_PrintArg_struct _tmp9AC;_tmp9AC.tag=0U,({struct _dyneither_ptr _tmpB0A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9AC.f1=_tmpB0A;});_tmp9AC;});struct Cyc_String_pa_PrintArg_struct _tmp448=({struct Cyc_String_pa_PrintArg_struct _tmp9AB;_tmp9AB.tag=0U,({struct _dyneither_ptr _tmpB0B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9AB.f1=_tmpB0B;});_tmp9AB;});void*_tmp444[3U];_tmp444[0]=& _tmp446,_tmp444[1]=& _tmp447,_tmp444[2]=& _tmp448;({unsigned int _tmpB0D=e->loc;struct _dyneither_ptr _tmpB0C=({const char*_tmp445="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp445,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpB0D,_tmpB0C,_tag_dyneither(_tmp444,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2628
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((int)(c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 2635
if((int)c != (int)1U)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp44B=({struct Cyc_String_pa_PrintArg_struct _tmp9AF;_tmp9AF.tag=0U,({
struct _dyneither_ptr _tmpB0E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9AF.f1=_tmpB0E;});_tmp9AF;});struct Cyc_String_pa_PrintArg_struct _tmp44C=({struct Cyc_String_pa_PrintArg_struct _tmp9AE;_tmp9AE.tag=0U,({struct _dyneither_ptr _tmpB0F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9AE.f1=_tmpB0F;});_tmp9AE;});void*_tmp449[2U];_tmp449[0]=& _tmp44B,_tmp449[1]=& _tmp44C;({unsigned int _tmpB11=e->loc;struct _dyneither_ptr _tmpB10=({const char*_tmp44A="implicit cast from %s to %s";_tag_dyneither(_tmp44A,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpB11,_tmpB10,_tag_dyneither(_tmp449,sizeof(void*),2U));});});
return 1;}else{
# 2641
return 0;}}}}}
# 2648
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2651
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2665 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple25{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2669
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple25*env,struct Cyc_Absyn_Aggrfield*x){
# 2672
struct _tuple25 _tmp44D=*env;struct _tuple25 _tmp44E=_tmp44D;struct Cyc_List_List*_tmp456;struct _RegionHandle*_tmp455;struct Cyc_Tcenv_Tenv*_tmp454;int _tmp453;_LL1: _tmp456=_tmp44E.f1;_tmp455=_tmp44E.f2;_tmp454=_tmp44E.f3;_tmp453=_tmp44E.f4;_LL2:;{
# 2674
void*_tmp44F=_tmp456 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp455,_tmp456,x->type);
struct Cyc_List_List*_tmp450=Cyc_Tcutil_flatten_type(_tmp455,_tmp453,_tmp454,_tmp44F);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp450)== 1)
return({struct Cyc_List_List*_tmp452=_region_malloc(_tmp455,sizeof(*_tmp452));({struct _tuple12*_tmpB12=({struct _tuple12*_tmp451=_region_malloc(_tmp455,sizeof(*_tmp451));_tmp451->f1=x->tq,_tmp451->f2=_tmp44F;_tmp451;});_tmp452->hd=_tmpB12;}),_tmp452->tl=0;_tmp452;});else{
return _tmp450;}};}struct _tuple26{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2680
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple26*env,struct _tuple12*x){
# 2682
struct _tuple26 _tmp457=*env;struct _tuple26 _tmp458=_tmp457;struct _RegionHandle*_tmp462;struct Cyc_Tcenv_Tenv*_tmp461;int _tmp460;_LL1: _tmp462=_tmp458.f1;_tmp461=_tmp458.f2;_tmp460=_tmp458.f3;_LL2:;{
struct _tuple12 _tmp459=*x;struct _tuple12 _tmp45A=_tmp459;struct Cyc_Absyn_Tqual _tmp45F;void*_tmp45E;_LL4: _tmp45F=_tmp45A.f1;_tmp45E=_tmp45A.f2;_LL5:;{
struct Cyc_List_List*_tmp45B=Cyc_Tcutil_flatten_type(_tmp462,_tmp460,_tmp461,_tmp45E);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45B)== 1)
return({struct Cyc_List_List*_tmp45D=_region_malloc(_tmp462,sizeof(*_tmp45D));({struct _tuple12*_tmpB13=({struct _tuple12*_tmp45C=_region_malloc(_tmp462,sizeof(*_tmp45C));_tmp45C->f1=_tmp45F,_tmp45C->f2=_tmp45E;_tmp45C;});_tmp45D->hd=_tmpB13;}),_tmp45D->tl=0;_tmp45D;});else{
return _tmp45B;}};};}
# 2689
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2693
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp463=t1;struct Cyc_List_List*_tmp482;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_Aggrdecl*_tmp480;struct Cyc_List_List*_tmp47F;switch(*((int*)_tmp463)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp463)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp463)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp480=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp463)->f1)->f1).KnownAggr).val;_tmp47F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp463)->f2;_LL6:
# 2711
 if((((int)_tmp480->kind == (int)Cyc_Absyn_UnionA  || _tmp480->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp480->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp480->impl))->rgn_po != 0)
# 2713
return({struct Cyc_List_List*_tmp46E=_region_malloc(r,sizeof(*_tmp46E));({struct _tuple12*_tmpB15=({struct _tuple12*_tmp46D=_region_malloc(r,sizeof(*_tmp46D));({struct Cyc_Absyn_Tqual _tmpB14=Cyc_Absyn_empty_tqual(0U);_tmp46D->f1=_tmpB14;}),_tmp46D->f2=t1;_tmp46D;});_tmp46E->hd=_tmpB15;}),_tmp46E->tl=0;_tmp46E;});{
struct Cyc_List_List*_tmp46F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp480->tvs,_tmp47F);
struct _tuple25 env=({struct _tuple25 _tmp9B0;_tmp9B0.f1=_tmp46F,_tmp9B0.f2=r,_tmp9B0.f3=te,_tmp9B0.f4=flatten;_tmp9B0;});
struct Cyc_List_List*_tmp470=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp480->impl))->fields;struct Cyc_List_List*_tmp471=_tmp470;struct Cyc_Absyn_Aggrfield*_tmp477;struct Cyc_List_List*_tmp476;if(_tmp471 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp477=(struct Cyc_Absyn_Aggrfield*)_tmp471->hd;_tmp476=_tmp471->tl;_LL14: {
# 2719
struct Cyc_List_List*_tmp472=Cyc_Tcutil_flatten_type_f(& env,_tmp477);
env.f4=0;{
struct Cyc_List_List*_tmp473=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp476);
struct Cyc_List_List*_tmp474=({struct Cyc_List_List*_tmp475=_region_malloc(r,sizeof(*_tmp475));_tmp475->hd=_tmp472,_tmp475->tl=_tmp473;_tmp475;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp474);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp481=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp463)->f1;_LL4: {
# 2698
struct _tuple26 _tmp464=({struct _tuple26 _tmp9B1;_tmp9B1.f1=r,_tmp9B1.f2=te,_tmp9B1.f3=flatten;_tmp9B1;});
# 2700
struct Cyc_List_List*_tmp465=_tmp481;struct _tuple12*_tmp46C;struct Cyc_List_List*_tmp46B;if(_tmp465 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp46C=(struct _tuple12*)_tmp465->hd;_tmp46B=_tmp465->tl;_LLF: {
# 2703
struct Cyc_List_List*_tmp466=Cyc_Tcutil_rcopy_tqt(& _tmp464,_tmp46C);
_tmp464.f3=0;{
struct Cyc_List_List*_tmp467=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple26*,struct _tuple12*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp464,_tmp481);
struct Cyc_List_List*_tmp468=({struct Cyc_List_List*_tmp46A=_region_malloc(r,sizeof(*_tmp46A));_tmp46A->hd=_tmp466,_tmp46A->tl=_tmp467;_tmp46A;});
return({struct _RegionHandle*_tmpB16=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpB16,({struct Cyc_List_List*_tmp469=_region_malloc(r,sizeof(*_tmp469));_tmp469->hd=_tmp466,_tmp469->tl=_tmp467;_tmp469;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp463)->f1 == Cyc_Absyn_StructA){_LL7: _tmp482=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp463)->f2;_LL8: {
# 2726
struct _tuple25 env=({struct _tuple25 _tmp9B2;_tmp9B2.f1=0,_tmp9B2.f2=r,_tmp9B2.f3=te,_tmp9B2.f4=flatten;_tmp9B2;});
struct Cyc_List_List*_tmp478=_tmp482;struct Cyc_Absyn_Aggrfield*_tmp47E;struct Cyc_List_List*_tmp47D;if(_tmp478 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp47E=(struct Cyc_Absyn_Aggrfield*)_tmp478->hd;_tmp47D=_tmp478->tl;_LL19: {
# 2730
struct Cyc_List_List*_tmp479=Cyc_Tcutil_flatten_type_f(& env,_tmp47E);
env.f4=0;{
struct Cyc_List_List*_tmp47A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp47D);
struct Cyc_List_List*_tmp47B=({struct Cyc_List_List*_tmp47C=_region_malloc(r,sizeof(*_tmp47C));_tmp47C->hd=_tmp479,_tmp47C->tl=_tmp47A;_tmp47C;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp47B);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2736
 goto _LL0;}_LL0:;};}
# 2739
return({struct Cyc_List_List*_tmp484=_region_malloc(r,sizeof(*_tmp484));({struct _tuple12*_tmpB18=({struct _tuple12*_tmp483=_region_malloc(r,sizeof(*_tmp483));({struct Cyc_Absyn_Tqual _tmpB17=Cyc_Absyn_empty_tqual(0U);_tmp483->f1=_tmpB17;}),_tmp483->f2=t1;_tmp483;});_tmp484->hd=_tmpB18;}),_tmp484->tl=0;_tmp484;});}
# 2743
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp485=(void*)t->hd;void*_tmp486=_tmp485;switch(*((int*)_tmp486)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2749
 continue;default: _LL7: _LL8:
# 2751
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2754
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2757
return 1;}
# 2760
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp487=({struct _tuple0 _tmp9B3;({void*_tmpB1A=Cyc_Tcutil_compress(t1);_tmp9B3.f1=_tmpB1A;}),({void*_tmpB19=Cyc_Tcutil_compress(t2);_tmp9B3.f2=_tmpB19;});_tmp9B3;});struct _tuple0 _tmp488=_tmp487;enum Cyc_Absyn_Size_of _tmp48A;enum Cyc_Absyn_Size_of _tmp489;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp488.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp488.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp488.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp488.f2)->f1)->tag == 1U){_LL1: _tmp48A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp488.f1)->f1)->f2;_tmp489=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp488.f2)->f1)->f2;_LL2:
# 2763
 return((int)_tmp48A == (int)_tmp489  || (int)_tmp48A == (int)2U  && (int)_tmp489 == (int)3U) || 
(int)_tmp48A == (int)3U  && (int)_tmp489 == (int)Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2771
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2774
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp48B=({struct _tuple0 _tmp9B4;_tmp9B4.f1=t1,_tmp9B4.f2=t2;_tmp9B4;});struct _tuple0 _tmp48C=_tmp48B;struct Cyc_Absyn_FnInfo _tmp4B6;struct Cyc_Absyn_FnInfo _tmp4B5;struct Cyc_Absyn_Datatypedecl*_tmp4B4;struct Cyc_Absyn_Datatypefield*_tmp4B3;struct Cyc_List_List*_tmp4B2;struct Cyc_Absyn_Datatypedecl*_tmp4B1;struct Cyc_List_List*_tmp4B0;void*_tmp4AF;struct Cyc_Absyn_Tqual _tmp4AE;void*_tmp4AD;void*_tmp4AC;void*_tmp4AB;void*_tmp4AA;void*_tmp4A9;struct Cyc_Absyn_Tqual _tmp4A8;void*_tmp4A7;void*_tmp4A6;void*_tmp4A5;void*_tmp4A4;switch(*((int*)_tmp48C.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f2)->tag == 3U){_LL1: _tmp4AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f1)->f1).elt_type;_tmp4AE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f1)->f1).elt_tq;_tmp4AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f1)->f1).ptr_atts).rgn;_tmp4AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f1)->f1).ptr_atts).nullable;_tmp4AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f1)->f1).ptr_atts).bounds;_tmp4AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f1)->f1).ptr_atts).zero_term;_tmp4A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f2)->f1).elt_type;_tmp4A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f2)->f1).elt_tq;_tmp4A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f2)->f1).ptr_atts).rgn;_tmp4A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f2)->f1).ptr_atts).nullable;_tmp4A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f2)->f1).ptr_atts).bounds;_tmp4A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C.f2)->f1).ptr_atts).zero_term;_LL2:
# 2786
 if(_tmp4AE.real_const  && !_tmp4A8.real_const)
return 0;
# 2789
if((!Cyc_Tcutil_unify(_tmp4AC,_tmp4A6) && 
Cyc_Absyn_type2bool(0,_tmp4AC)) && !Cyc_Absyn_type2bool(0,_tmp4A6))
return 0;
# 2793
if((Cyc_Tcutil_unify(_tmp4AA,_tmp4A4) && !
Cyc_Absyn_type2bool(0,_tmp4AA)) && Cyc_Absyn_type2bool(0,_tmp4A4))
return 0;
# 2797
if((!Cyc_Tcutil_unify(_tmp4AD,_tmp4A7) && !Cyc_Tcenv_region_outlives(te,_tmp4AD,_tmp4A7)) && !
Cyc_Tcutil_subtype(te,assume,_tmp4AD,_tmp4A7))
return 0;
# 2801
if(!Cyc_Tcutil_unify(_tmp4AB,_tmp4A5)){
struct Cyc_Absyn_Exp*_tmp48D=({void*_tmpB1B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB1B,_tmp4AB);});
struct Cyc_Absyn_Exp*_tmp48E=({void*_tmpB1C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB1C,_tmp4A5);});
if(_tmp48D != _tmp48E){
if((_tmp48D == 0  || _tmp48E == 0) || !Cyc_Evexp_lte_const_exp(_tmp48E,_tmp48E))
return 0;}}
# 2811
if(!_tmp4A8.real_const  && _tmp4AE.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4A9)))
return 0;}{
# 2817
int _tmp48F=
({void*_tmpB1D=_tmp4A5;Cyc_Tcutil_unify(_tmpB1D,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4A4);
# 2822
return(_tmp48F  && ({struct Cyc_Tcenv_Tenv*_tmpB21=te;struct Cyc_List_List*_tmpB20=({struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));({struct _tuple0*_tmpB1E=({struct _tuple0*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->f1=t1,_tmp490->f2=t2;_tmp490;});_tmp491->hd=_tmpB1E;}),_tmp491->tl=assume;_tmp491;});void*_tmpB1F=_tmp4AF;Cyc_Tcutil_ptrsubtype(_tmpB21,_tmpB20,_tmpB1F,_tmp4A9);}) || Cyc_Tcutil_unify(_tmp4AF,_tmp4A9)) || Cyc_Tcutil_isomorphic(_tmp4AF,_tmp4A9);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp4B4=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp4B3=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp4B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f1)->f2;_tmp4B1=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->f1)->f1).KnownDatatype).val;_tmp4B0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48C.f2)->f2;_LL4:
# 2829
 if(_tmp4B4 != _tmp4B1  && Cyc_Absyn_qvar_cmp(_tmp4B4->name,_tmp4B1->name)!= 0)return 0;
# 2831
if(({int _tmpB22=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B2);_tmpB22 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B0);}))return 0;
for(0;_tmp4B2 != 0;(_tmp4B2=_tmp4B2->tl,_tmp4B0=_tmp4B0->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4B2->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp4B0))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48C.f2)->tag == 5U){_LL5: _tmp4B6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48C.f1)->f1;_tmp4B5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48C.f2)->f1;_LL6:
# 2838
 if(_tmp4B6.tvars != 0  || _tmp4B5.tvars != 0){
struct Cyc_List_List*_tmp492=_tmp4B6.tvars;
struct Cyc_List_List*_tmp493=_tmp4B5.tvars;
if(({int _tmpB23=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp492);_tmpB23 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp493);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp492 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp492->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp493))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));({struct _tuple15*_tmpB25=({struct _tuple15*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->f1=(struct Cyc_Absyn_Tvar*)_tmp493->hd,({void*_tmpB24=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp492->hd);_tmp494->f2=_tmpB24;});_tmp494;});_tmp495->hd=_tmpB25;}),_tmp495->tl=inst;_tmp495;});
_tmp492=_tmp492->tl;
_tmp493=_tmp493->tl;}
# 2849
if(inst != 0){
_tmp4B6.tvars=0;
_tmp4B5.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpB28=te;struct Cyc_List_List*_tmpB27=assume;void*_tmpB26=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->tag=5U,_tmp496->f1=_tmp4B6;_tmp496;});Cyc_Tcutil_subtype(_tmpB28,_tmpB27,_tmpB26,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->tag=5U,_tmp497->f1=_tmp4B5;_tmp497;}));});}};}
# 2856
if(!Cyc_Tcutil_subtype(te,assume,_tmp4B6.ret_type,_tmp4B5.ret_type))return 0;{
struct Cyc_List_List*_tmp498=_tmp4B6.args;
struct Cyc_List_List*_tmp499=_tmp4B5.args;
# 2861
if(({int _tmpB29=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp498);_tmpB29 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp499);}))return 0;
# 2863
for(0;_tmp498 != 0;(_tmp498=_tmp498->tl,_tmp499=_tmp499->tl)){
struct _tuple10 _tmp49A=*((struct _tuple10*)_tmp498->hd);struct _tuple10 _tmp49B=_tmp49A;struct Cyc_Absyn_Tqual _tmp4A1;void*_tmp4A0;_LLA: _tmp4A1=_tmp49B.f2;_tmp4A0=_tmp49B.f3;_LLB:;{
struct _tuple10 _tmp49C=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp499))->hd);struct _tuple10 _tmp49D=_tmp49C;struct Cyc_Absyn_Tqual _tmp49F;void*_tmp49E;_LLD: _tmp49F=_tmp49D.f2;_tmp49E=_tmp49D.f3;_LLE:;
# 2867
if(_tmp49F.real_const  && !_tmp4A1.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp49E,_tmp4A0))
return 0;};}
# 2871
if(_tmp4B6.c_varargs != _tmp4B5.c_varargs)return 0;
if(_tmp4B6.cyc_varargs != 0  && _tmp4B5.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp4A2=*_tmp4B6.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp4A3=*_tmp4B5.cyc_varargs;
# 2876
if((_tmp4A3.tq).real_const  && !(_tmp4A2.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp4A3.type,_tmp4A2.type))
return 0;}else{
if(_tmp4B6.cyc_varargs != 0  || _tmp4B5.cyc_varargs != 0)return 0;}
# 2881
if(!({void*_tmpB2A=(void*)_check_null(_tmp4B6.effect);Cyc_Tcutil_subset_effect(1,_tmpB2A,(void*)_check_null(_tmp4B5.effect));}))return 0;
# 2883
if(!Cyc_Tcutil_sub_rgnpo(_tmp4B6.rgn_po,_tmp4B5.rgn_po))return 0;
# 2885
if(!Cyc_Tcutil_sub_attributes(_tmp4B6.attributes,_tmp4B5.attributes))return 0;
# 2887
if(!Cyc_Tcutil_check_logical_implication(_tmp4B5.requires_relns,_tmp4B6.requires_relns))
return 0;
# 2890
if(!Cyc_Tcutil_check_logical_implication(_tmp4B6.ensures_relns,_tmp4B5.ensures_relns))
return 0;
# 2893
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2905 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2907
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp4B7=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp4B8=_tmp4B7;struct Cyc_Absyn_Tqual _tmp4BE;void*_tmp4BD;_LL1: _tmp4BE=_tmp4B8->f1;_tmp4BD=_tmp4B8->f2;_LL2:;{
struct _tuple12*_tmp4B9=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp4BA=_tmp4B9;struct Cyc_Absyn_Tqual _tmp4BC;void*_tmp4BB;_LL4: _tmp4BC=_tmp4BA->f1;_tmp4BB=_tmp4BA->f2;_LL5:;
# 2914
if(_tmp4BE.real_const  && !_tmp4BC.real_const)return 0;
# 2916
if((_tmp4BC.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4BB))) && 
Cyc_Tcutil_subtype(te,assume,_tmp4BD,_tmp4BB))
# 2919
continue;
# 2921
if(Cyc_Tcutil_unify(_tmp4BD,_tmp4BB))
# 2923
continue;
# 2925
if(Cyc_Tcutil_isomorphic(_tmp4BD,_tmp4BB))
# 2927
continue;
# 2930
return 0;};};}
# 2932
return 1;}
# 2937
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2943
{void*_tmp4BF=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BF)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BF)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BF)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2946
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2948
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2952
void*_tmp4C0=t1;void*_tmp4E5;struct Cyc_Absyn_Enumdecl*_tmp4E4;void*_tmp4E3;struct Cyc_Absyn_Tqual _tmp4E2;struct Cyc_Absyn_Exp*_tmp4E1;void*_tmp4E0;void*_tmp4DF;struct Cyc_Absyn_Tqual _tmp4DE;void*_tmp4DD;void*_tmp4DC;void*_tmp4DB;void*_tmp4DA;switch(*((int*)_tmp4C0)){case 3U: _LLA: _tmp4DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C0)->f1).elt_type;_tmp4DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C0)->f1).elt_tq;_tmp4DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C0)->f1).ptr_atts).rgn;_tmp4DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C0)->f1).ptr_atts).nullable;_tmp4DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C0)->f1).ptr_atts).bounds;_tmp4DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C0)->f1).ptr_atts).zero_term;_LLB:
# 2961
{void*_tmp4C1=t2;void*_tmp4D0;struct Cyc_Absyn_Tqual _tmp4CF;void*_tmp4CE;void*_tmp4CD;void*_tmp4CC;void*_tmp4CB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->tag == 3U){_LL19: _tmp4D0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).elt_type;_tmp4CF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).elt_tq;_tmp4CE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).rgn;_tmp4CD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).nullable;_tmp4CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).bounds;_tmp4CB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1)->f1).ptr_atts).zero_term;_LL1A: {
# 2965
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp4C2=({struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));({struct _tuple0*_tmpB2B=({struct _tuple0*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->f1=t1,_tmp4C9->f2=t2;_tmp4C9;});_tmp4CA->hd=_tmpB2B;}),_tmp4CA->tl=0;_tmp4CA;});
int _tmp4C3=_tmp4CF.real_const  || !_tmp4DE.real_const;
# 2979 "tcutil.cyc"
int _tmp4C4=
({void*_tmpB2C=_tmp4CC;Cyc_Tcutil_unify(_tmpB2C,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4CB);
# 2982
int _tmp4C5=_tmp4C3  && (
((_tmp4C4  && Cyc_Tcutil_ptrsubtype(te,_tmp4C2,_tmp4DF,_tmp4D0) || 
Cyc_Tcutil_unify(_tmp4DF,_tmp4D0)) || Cyc_Tcutil_isomorphic(_tmp4DF,_tmp4D0)) || Cyc_Tcutil_unify(_tmp4D0,Cyc_Absyn_void_type));
# 2986
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=Cyc_Tcutil_unify(_tmp4DA,_tmp4CB) || !Cyc_Absyn_type2bool(0,_tmp4CB);
# 2990
int _tmp4C6=_tmp4C5?0:((Cyc_Tcutil_is_bits_only_type(_tmp4DF) && Cyc_Tcutil_is_char_type(_tmp4D0)) && !
Cyc_Tcutil_force_type2bool(0,_tmp4CB)) && (
_tmp4CF.real_const  || !_tmp4DE.real_const);
int bounds_ok=Cyc_Tcutil_unify(_tmp4DB,_tmp4CC);
if(!bounds_ok  && !_tmp4C6){
struct Cyc_Absyn_Exp*_tmp4C7=({void*_tmpB2D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB2D,_tmp4DB);});
struct Cyc_Absyn_Exp*_tmp4C8=({void*_tmpB2E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB2E,_tmp4CC);});
if((_tmp4C7 != 0  && _tmp4C8 != 0) && Cyc_Evexp_lte_const_exp(_tmp4C8,_tmp4C7))
bounds_ok=1;else{
if(_tmp4C7 == 0  || _tmp4C8 == 0)
bounds_ok=1;}}{
# 3002
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4DC);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4CD);
if(t1_nullable  && !t2_nullable)
coercion=2U;
# 3009
if(((bounds_ok  && zeroterm_ok) && (_tmp4C5  || _tmp4C6)) && (
Cyc_Tcutil_unify(_tmp4DD,_tmp4CE) || Cyc_Tcenv_region_outlives(te,_tmp4DD,_tmp4CE)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 3015
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp4E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C0)->f1).elt_type;_tmp4E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C0)->f1).tq;_tmp4E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C0)->f1).num_elts;_tmp4E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C0)->f1).zero_term;_LLD:
# 3017
{void*_tmp4D1=t2;void*_tmp4D5;struct Cyc_Absyn_Tqual _tmp4D4;struct Cyc_Absyn_Exp*_tmp4D3;void*_tmp4D2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D1)->tag == 4U){_LL1E: _tmp4D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D1)->f1).elt_type;_tmp4D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D1)->f1).tq;_tmp4D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D1)->f1).num_elts;_tmp4D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D1)->f1).zero_term;_LL1F: {
# 3019
int okay;
okay=
(((_tmp4E1 != 0  && _tmp4D3 != 0) && Cyc_Tcutil_unify(_tmp4E0,_tmp4D2)) && 
Cyc_Evexp_lte_const_exp(_tmp4D3,_tmp4E1)) && 
Cyc_Evexp_lte_const_exp(_tmp4E1,_tmp4D3);
return
# 3026
(okay  && Cyc_Tcutil_unify(_tmp4E3,_tmp4D5)) && (!_tmp4E2.real_const  || _tmp4D4.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 3028
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 3030
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C0)->f1)){case 15U: _LLE: _tmp4E4=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C0)->f1)->f2;_LLF:
# 3034
{void*_tmp4D6=t2;struct Cyc_Absyn_Enumdecl*_tmp4D7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D6)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D6)->f1)->tag == 15U){_LL23: _tmp4D7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D6)->f1)->f2;_LL24:
# 3036
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4E4))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4D7))->fields != 0) && ({
int _tmpB2F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4E4->fields))->v);_tmpB2F >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4D7->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 3042
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 3045
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C0)->f2 != 0){_LL14: _tmp4E5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C0)->f2)->hd;_LL15:
# 3048
{void*_tmp4D8=t2;void*_tmp4D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->f2 != 0){_LL28: _tmp4D9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->f2)->hd;_LL29:
# 3050
 if(Cyc_Tcenv_region_outlives(te,_tmp4E5,_tmp4D9))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 3054
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 3060
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp4E6=Cyc_Absyn_copy_exp(e);
({void*_tmpB30=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->tag=14U,_tmp4E7->f1=t,_tmp4E7->f2=_tmp4E6,_tmp4E7->f3=0,_tmp4E7->f4=c;_tmp4E7;});e->r=_tmpB30;});
e->topt=t;}}
# 3068
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple0 _tmp4E8=({struct _tuple0 _tmp9B6;({void*_tmpB32=Cyc_Tcutil_compress(t1);_tmp9B6.f1=_tmpB32;}),({void*_tmpB31=Cyc_Tcutil_compress(t2);_tmp9B6.f2=_tmpB31;});_tmp9B6;});struct _tuple0 _tmp4E9=_tmp4E8;void*_tmp4EF;void*_tmp4EE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E9.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E9.f2)->tag == 0U){_LL1: _tmp4EF=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E9.f1)->f1;_tmp4EE=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E9.f2)->f1;_LL2:
# 3071
{struct _tuple0 _tmp4EA=({struct _tuple0 _tmp9B5;_tmp9B5.f1=_tmp4EF,_tmp9B5.f2=_tmp4EE;_tmp9B5;});struct _tuple0 _tmp4EB=_tmp4EA;int _tmp4ED;int _tmp4EC;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EB.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 2U){_LL6: _tmp4ED=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f1;_tmp4EC=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f1;_LL7:
# 3073
 if(_tmp4ED != 0  && _tmp4ED != 1)return t1;else{
if(_tmp4EC != 0  && _tmp4EC != 1)return t2;else{
if(_tmp4ED >= _tmp4EC)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 3083
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 3088
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 3081
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 3090
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 3080
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 3084
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EB.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 3089
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 3082
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EB.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4EB.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 3091
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EB.f1)->tag == 4U){_LL18: _LL19:
# 3086
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp4EB.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 3092
 goto _LL5;}}}}}}_LL5:;}
# 3094
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3097
return Cyc_Absyn_sint_type;}
# 3107 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp4F0=({struct _tuple0 _tmp9B7;_tmp9B7.f1=c1,_tmp9B7.f2=c2;_tmp9B7;});struct _tuple0 _tmp4F1=_tmp4F0;struct Cyc_Core_Opt**_tmp505;struct Cyc_Absyn_Kind*_tmp504;struct Cyc_Core_Opt**_tmp503;struct Cyc_Absyn_Kind*_tmp502;struct Cyc_Core_Opt**_tmp501;struct Cyc_Absyn_Kind*_tmp500;struct Cyc_Absyn_Kind*_tmp4FF;struct Cyc_Core_Opt**_tmp4FE;struct Cyc_Core_Opt**_tmp4FD;struct Cyc_Absyn_Kind*_tmp4FC;struct Cyc_Core_Opt**_tmp4FB;struct Cyc_Absyn_Kind*_tmp4FA;struct Cyc_Absyn_Kind*_tmp4F9;struct Cyc_Absyn_Kind*_tmp4F8;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->tag == 0U)switch(*((int*)_tmp4F1.f2)){case 0U: _LL1: _tmp4F9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->f1;_tmp4F8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->f1;_LL2:
 return _tmp4F9 == _tmp4F8;case 1U: goto _LL3;default: _LL9: _tmp4FC=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->f1;_tmp4FB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->f1;_tmp4FA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->f2;_LLA:
# 3120
 if(Cyc_Tcutil_kind_leq(_tmp4FC,_tmp4FA)){
({struct Cyc_Core_Opt*_tmpB33=({struct Cyc_Core_Opt*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->v=c1;_tmp4F5;});*_tmp4FB=_tmpB33;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->tag == 1U){_LL3: _tmp4FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->f1;_LL4:
# 3113
({struct Cyc_Core_Opt*_tmpB34=({struct Cyc_Core_Opt*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->v=c1;_tmp4F2;});*_tmp4FD=_tmpB34;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->tag == 1U){_LL5: _tmp4FE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpB35=({struct Cyc_Core_Opt*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->v=c2;_tmp4F3;});*_tmp4FE=_tmpB35;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->tag == 0U){_LL7: _tmp501=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->f1;_tmp500=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->f2;_tmp4FF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->f1;_LL8:
# 3116
 if(Cyc_Tcutil_kind_leq(_tmp4FF,_tmp500)){
({struct Cyc_Core_Opt*_tmpB36=({struct Cyc_Core_Opt*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->v=c2;_tmp4F4;});*_tmp501=_tmpB36;});return 1;}else{
return 0;}}else{_LLB: _tmp505=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->f1;_tmp504=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f1)->f2;_tmp503=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->f1;_tmp502=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F1.f2)->f2;_LLC:
# 3124
 if(Cyc_Tcutil_kind_leq(_tmp504,_tmp502)){
({struct Cyc_Core_Opt*_tmpB37=({struct Cyc_Core_Opt*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->v=c1;_tmp4F6;});*_tmp503=_tmpB37;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp502,_tmp504)){
({struct Cyc_Core_Opt*_tmpB38=({struct Cyc_Core_Opt*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->v=c2;_tmp4F7;});*_tmp505=_tmpB38;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3133
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3138
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp50B=({struct Cyc_Int_pa_PrintArg_struct _tmp9B8;_tmp9B8.tag=1U,_tmp9B8.f1=(unsigned long)i;_tmp9B8;});void*_tmp509[1U];_tmp509[0]=& _tmp50B;({struct _dyneither_ptr _tmpB39=({const char*_tmp50A="#%d";_tag_dyneither(_tmp50A,sizeof(char),4U);});Cyc_aprintf(_tmpB39,_tag_dyneither(_tmp509,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp508=_cycalloc(sizeof(*_tmp508));({struct _dyneither_ptr*_tmpB3A=({unsigned int _tmp507=1;struct _dyneither_ptr*_tmp506=_cycalloc(_check_times(_tmp507,sizeof(struct _dyneither_ptr)));_tmp506[0]=s;_tmp506;});_tmp508->name=_tmpB3A;}),_tmp508->identity=- 1,_tmp508->kind=k;_tmp508;});}
# 3145
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp50C=*t->name;
return(int)*((const char*)_check_dyneither_subscript(_tmp50C,sizeof(char),0))== (int)'#';}
# 3150
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp50F=({struct Cyc_String_pa_PrintArg_struct _tmp9B9;_tmp9B9.tag=0U,_tmp9B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmp9B9;});void*_tmp50D[1U];_tmp50D[0]=& _tmp50F;({struct _dyneither_ptr _tmpB3B=({const char*_tmp50E="%s";_tag_dyneither(_tmp50E,sizeof(char),3U);});Cyc_printf(_tmpB3B,_tag_dyneither(_tmp50D,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp510=({struct _dyneither_ptr _tmpB3C=({const char*_tmp516="`";_tag_dyneither(_tmp516,sizeof(char),2U);});Cyc_strconcat(_tmpB3C,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp511=_dyneither_ptr_plus(_tmp510,sizeof(char),1);char _tmp512=*((char*)_check_dyneither_subscript(_tmp511,sizeof(char),0U));char _tmp513='t';if(_get_dyneither_size(_tmp511,sizeof(char))== 1U  && (_tmp512 == 0  && _tmp513 != 0))_throw_arraybounds();*((char*)_tmp511.curr)=_tmp513;});
({struct _dyneither_ptr*_tmpB3D=({unsigned int _tmp515=1;struct _dyneither_ptr*_tmp514=_cycalloc(_check_times(_tmp515,sizeof(struct _dyneither_ptr)));_tmp514[0]=(struct _dyneither_ptr)_tmp510;_tmp514;});t->name=_tmpB3D;});};}
# 3159
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 3166
struct Cyc_List_List*_tmp517=0;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp517=({struct Cyc_List_List*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->hd=(void*)atts->hd,_tmp518->tl=_tmp517;_tmp518;});}}{
struct Cyc_Absyn_FnInfo _tmp519=fd->i;
_tmp519.attributes=_tmp517;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->tag=5U,_tmp51A->f1=_tmp519;_tmp51A;});};}
# 3174
return(void*)_check_null(fd->cached_type);}
# 3180
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3182
union Cyc_Relations_RelnOp _tmp51B=*rop;union Cyc_Relations_RelnOp _tmp51C=_tmp51B;struct Cyc_Absyn_Vardecl*_tmp52F;struct Cyc_Absyn_Vardecl*_tmp52E;switch((_tmp51C.RNumelts).tag){case 2U: _LL1: _tmp52E=(_tmp51C.RVar).val;_LL2: {
# 3184
struct _tuple2 _tmp51D=*_tmp52E->name;struct _tuple2 _tmp51E=_tmp51D;union Cyc_Absyn_Nmspace _tmp525;struct _dyneither_ptr*_tmp524;_LL8: _tmp525=_tmp51E.f1;_tmp524=_tmp51E.f2;_LL9:;
if(!((int)((_tmp525.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpB3E=(struct _dyneither_ptr)*_tmp524;Cyc_strcmp(_tmpB3E,({const char*_tmp51F="return_value";_tag_dyneither(_tmp51F,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpB3F=Cyc_Relations_RReturn();*rop=_tmpB3F;});
goto _LL0;}{
# 3190
unsigned int c=0U;
{struct Cyc_List_List*_tmp520=args;for(0;_tmp520 != 0;(_tmp520=_tmp520->tl,c ++)){
struct _tuple10*_tmp521=(struct _tuple10*)_tmp520->hd;struct _tuple10*_tmp522=_tmp521;struct _dyneither_ptr*_tmp523;_LLB: _tmp523=_tmp522->f1;_LLC:;
if(_tmp523 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp524,(struct _dyneither_ptr)*_tmp523)== 0){
({union Cyc_Relations_RelnOp _tmpB40=Cyc_Relations_RParam(c);*rop=_tmpB40;});
break;}}}}
# 3200
goto _LL0;};}case 3U: _LL3: _tmp52F=(_tmp51C.RNumelts).val;_LL4: {
# 3202
struct _tuple2 _tmp526=*_tmp52F->name;struct _tuple2 _tmp527=_tmp526;union Cyc_Absyn_Nmspace _tmp52D;struct _dyneither_ptr*_tmp52C;_LLE: _tmp52D=_tmp527.f1;_tmp52C=_tmp527.f2;_LLF:;
if(!((int)((_tmp52D.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp528=args;for(0;_tmp528 != 0;(_tmp528=_tmp528->tl,c ++)){
struct _tuple10*_tmp529=(struct _tuple10*)_tmp528->hd;struct _tuple10*_tmp52A=_tmp529;struct _dyneither_ptr*_tmp52B;_LL11: _tmp52B=_tmp52A->f1;_LL12:;
if(_tmp52B != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp52C,(struct _dyneither_ptr)*_tmp52B)== 0){
({union Cyc_Relations_RelnOp _tmpB41=Cyc_Relations_RParamNumelts(c);*rop=_tmpB41;});
break;}}}}
# 3214
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3219
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3221
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3225
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3227
if(e == 0)return 0;{
struct Cyc_List_List*_tmp530=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp530);
return _tmp530;};}
# 3234
static struct _dyneither_ptr*Cyc_Tcutil_fst_fdarg(struct _tuple10*t){return(struct _dyneither_ptr*)_check_null((*t).f1);}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp531=pr;struct Cyc_Absyn_Tqual _tmp534;void*_tmp533;_LL1: _tmp534=_tmp531->f1;_tmp533=_tmp531->f2;_LL2:;
if(_tmp533 == t)return pr;else{
return({struct _tuple12*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->f1=_tmp534,_tmp532->f2=t;_tmp532;});}}struct _tuple27{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple28{struct _tuple27*f1;void*f2;};
# 3241
static struct _tuple28*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3243
return({struct _tuple28*_tmp536=_region_malloc(rgn,sizeof(*_tmp536));({struct _tuple27*_tmpB42=({struct _tuple27*_tmp535=_region_malloc(rgn,sizeof(*_tmp535));_tmp535->f1=(*y).f1,_tmp535->f2=(*y).f2;_tmp535;});_tmp536->f1=_tmpB42;}),_tmp536->f2=(*y).f3;_tmp536;});}
# 3245
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3247
struct _tuple10 _tmp537=*orig_arg;struct _tuple10 _tmp538=_tmp537;struct _dyneither_ptr*_tmp53C;struct Cyc_Absyn_Tqual _tmp53B;void*_tmp53A;_LL1: _tmp53C=_tmp538.f1;_tmp53B=_tmp538.f2;_tmp53A=_tmp538.f3;_LL2:;
if(t == _tmp53A)return orig_arg;
return({struct _tuple10*_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539->f1=_tmp53C,_tmp539->f2=_tmp53B,_tmp539->f3=t;_tmp539;});}
# 3251
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3254
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3259
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3261
return({struct Cyc_Absyn_Exp*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->topt=old->topt,_tmp53D->r=r,_tmp53D->loc=old->loc,_tmp53D->annot=old->annot;_tmp53D;});}
# 3266
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp53E=e->r;void*_tmp53F=_tmp53E;void*_tmp57F;void*_tmp57E;struct Cyc_List_List*_tmp57D;struct Cyc_Absyn_Exp*_tmp57C;struct Cyc_Absyn_Exp*_tmp57B;void*_tmp57A;void*_tmp579;struct Cyc_Absyn_Exp*_tmp578;int _tmp577;enum Cyc_Absyn_Coercion _tmp576;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Exp*_tmp573;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Exp*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*_tmp56D;enum Cyc_Absyn_Primop _tmp56C;struct Cyc_List_List*_tmp56B;switch(*((int*)_tmp53F)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp56C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_tmp56B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp53F)->f2;_LLC:
# 3275
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56B)== 1){
struct Cyc_Absyn_Exp*_tmp540=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56B))->hd;
struct Cyc_Absyn_Exp*_tmp541=Cyc_Tcutil_rsubsexp(r,inst,_tmp540);
if(_tmp541 == _tmp540)return e;
return({struct Cyc_Absyn_Exp*_tmpB44=e;Cyc_Tcutil_copye(_tmpB44,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->tag=3U,_tmp543->f1=_tmp56C,({struct Cyc_List_List*_tmpB43=({struct Cyc_Absyn_Exp*_tmp542[1U];_tmp542[0]=_tmp541;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp542,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp543->f2=_tmpB43;});_tmp543;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56B)== 2){
struct Cyc_Absyn_Exp*_tmp544=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56B))->hd;
struct Cyc_Absyn_Exp*_tmp545=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56B->tl))->hd;
struct Cyc_Absyn_Exp*_tmp546=Cyc_Tcutil_rsubsexp(r,inst,_tmp544);
struct Cyc_Absyn_Exp*_tmp547=Cyc_Tcutil_rsubsexp(r,inst,_tmp545);
if(_tmp546 == _tmp544  && _tmp547 == _tmp545)return e;
return({struct Cyc_Absyn_Exp*_tmpB46=e;Cyc_Tcutil_copye(_tmpB46,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->tag=3U,_tmp549->f1=_tmp56C,({struct Cyc_List_List*_tmpB45=({struct Cyc_Absyn_Exp*_tmp548[2U];_tmp548[0]=_tmp546,_tmp548[1]=_tmp547;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp548,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp549->f2=_tmpB45;});_tmp549;}));});}else{
return({void*_tmp54A=0U;({struct _dyneither_ptr _tmpB47=({const char*_tmp54B="primop does not have 1 or 2 args!";_tag_dyneither(_tmp54B,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB47,_tag_dyneither(_tmp54A,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp56F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_tmp56E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53F)->f2;_tmp56D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53F)->f3;_LLE: {
# 3289
struct Cyc_Absyn_Exp*_tmp54C=Cyc_Tcutil_rsubsexp(r,inst,_tmp56F);
struct Cyc_Absyn_Exp*_tmp54D=Cyc_Tcutil_rsubsexp(r,inst,_tmp56E);
struct Cyc_Absyn_Exp*_tmp54E=Cyc_Tcutil_rsubsexp(r,inst,_tmp56D);
if((_tmp54C == _tmp56F  && _tmp54D == _tmp56E) && _tmp54E == _tmp56D)return e;
return({struct Cyc_Absyn_Exp*_tmpB48=e;Cyc_Tcutil_copye(_tmpB48,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->tag=6U,_tmp54F->f1=_tmp54C,_tmp54F->f2=_tmp54D,_tmp54F->f3=_tmp54E;_tmp54F;}));});}case 7U: _LLF: _tmp571=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_tmp570=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp53F)->f2;_LL10: {
# 3295
struct Cyc_Absyn_Exp*_tmp550=Cyc_Tcutil_rsubsexp(r,inst,_tmp571);
struct Cyc_Absyn_Exp*_tmp551=Cyc_Tcutil_rsubsexp(r,inst,_tmp570);
if(_tmp550 == _tmp571  && _tmp551 == _tmp570)return e;
return({struct Cyc_Absyn_Exp*_tmpB49=e;Cyc_Tcutil_copye(_tmpB49,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->tag=7U,_tmp552->f1=_tmp550,_tmp552->f2=_tmp551;_tmp552;}));});}case 8U: _LL11: _tmp573=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_tmp572=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp53F)->f2;_LL12: {
# 3300
struct Cyc_Absyn_Exp*_tmp553=Cyc_Tcutil_rsubsexp(r,inst,_tmp573);
struct Cyc_Absyn_Exp*_tmp554=Cyc_Tcutil_rsubsexp(r,inst,_tmp572);
if(_tmp553 == _tmp573  && _tmp554 == _tmp572)return e;
return({struct Cyc_Absyn_Exp*_tmpB4A=e;Cyc_Tcutil_copye(_tmpB4A,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->tag=8U,_tmp555->f1=_tmp553,_tmp555->f2=_tmp554;_tmp555;}));});}case 9U: _LL13: _tmp575=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_tmp574=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53F)->f2;_LL14: {
# 3305
struct Cyc_Absyn_Exp*_tmp556=Cyc_Tcutil_rsubsexp(r,inst,_tmp575);
struct Cyc_Absyn_Exp*_tmp557=Cyc_Tcutil_rsubsexp(r,inst,_tmp574);
if(_tmp556 == _tmp575  && _tmp557 == _tmp574)return e;
return({struct Cyc_Absyn_Exp*_tmpB4B=e;Cyc_Tcutil_copye(_tmpB4B,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->tag=9U,_tmp558->f1=_tmp556,_tmp558->f2=_tmp557;_tmp558;}));});}case 14U: _LL15: _tmp579=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_tmp578=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53F)->f2;_tmp577=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53F)->f3;_tmp576=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53F)->f4;_LL16: {
# 3310
struct Cyc_Absyn_Exp*_tmp559=Cyc_Tcutil_rsubsexp(r,inst,_tmp578);
void*_tmp55A=Cyc_Tcutil_rsubstitute(r,inst,_tmp579);
if(_tmp559 == _tmp578  && _tmp55A == _tmp579)return e;
return({struct Cyc_Absyn_Exp*_tmpB4C=e;Cyc_Tcutil_copye(_tmpB4C,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->tag=14U,_tmp55B->f1=_tmp55A,_tmp55B->f2=_tmp559,_tmp55B->f3=_tmp577,_tmp55B->f4=_tmp576;_tmp55B;}));});}case 17U: _LL17: _tmp57A=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_LL18: {
# 3315
void*_tmp55C=Cyc_Tcutil_rsubstitute(r,inst,_tmp57A);
if(_tmp55C == _tmp57A)return e;
return({struct Cyc_Absyn_Exp*_tmpB4D=e;Cyc_Tcutil_copye(_tmpB4D,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=17U,_tmp55D->f1=_tmp55C;_tmp55D;}));});}case 18U: _LL19: _tmp57B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_LL1A: {
# 3319
struct Cyc_Absyn_Exp*_tmp55E=Cyc_Tcutil_rsubsexp(r,inst,_tmp57B);
if(_tmp55E == _tmp57B)return e;
return({struct Cyc_Absyn_Exp*_tmpB4E=e;Cyc_Tcutil_copye(_tmpB4E,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->tag=18U,_tmp55F->f1=_tmp55E;_tmp55F;}));});}case 41U: _LL1B: _tmp57C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_LL1C: {
# 3323
struct Cyc_Absyn_Exp*_tmp560=Cyc_Tcutil_rsubsexp(r,inst,_tmp57C);
if(_tmp560 == _tmp57C)return e;
return({struct Cyc_Absyn_Exp*_tmpB4F=e;Cyc_Tcutil_copye(_tmpB4F,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->tag=41U,_tmp561->f1=_tmp560;_tmp561;}));});}case 19U: _LL1D: _tmp57E=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_tmp57D=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp53F)->f2;_LL1E: {
# 3327
void*_tmp562=Cyc_Tcutil_rsubstitute(r,inst,_tmp57E);
if(_tmp562 == _tmp57E)return e;
return({struct Cyc_Absyn_Exp*_tmpB50=e;Cyc_Tcutil_copye(_tmpB50,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=19U,_tmp563->f1=_tmp562,_tmp563->f2=_tmp57D;_tmp563;}));});}case 39U: _LL1F: _tmp57F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp53F)->f1;_LL20: {
# 3331
void*_tmp564=Cyc_Tcutil_rsubstitute(r,inst,_tmp57F);
if(_tmp564 == _tmp57F)return e;{
# 3334
void*_tmp565=Cyc_Tcutil_compress(_tmp564);void*_tmp566=_tmp565;struct Cyc_Absyn_Exp*_tmp568;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp566)->tag == 9U){_LL24: _tmp568=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp566)->f1;_LL25:
 return _tmp568;}else{_LL26: _LL27:
# 3337
 return({struct Cyc_Absyn_Exp*_tmpB51=e;Cyc_Tcutil_copye(_tmpB51,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->tag=39U,_tmp567->f1=_tmp564;_tmp567;}));});}_LL23:;};}default: _LL21: _LL22:
# 3340
 return({void*_tmp569=0U;({struct _dyneither_ptr _tmpB52=({const char*_tmp56A="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp56A,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB52,_tag_dyneither(_tmp569,sizeof(void*),0U));});});}_LL0:;}
# 3344
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3347
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3351
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3355
void*_tmp580=f->type;
struct Cyc_Absyn_Exp*_tmp581=f->requires_clause;
void*_tmp582=Cyc_Tcutil_rsubstitute(r,inst,_tmp580);
struct Cyc_Absyn_Exp*_tmp583=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp581);
if(_tmp580 == _tmp582  && _tmp581 == _tmp583)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->name=f->name,_tmp584->tq=f->tq,_tmp584->type=_tmp582,_tmp584->width=f->width,_tmp584->attributes=f->attributes,_tmp584->requires_clause=_tmp583;_tmp584;});}}
# 3365
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3368
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp585=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp586=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp587=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp585);
struct Cyc_List_List*_tmp588=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp586);
if(_tmp587 == _tmp585  && _tmp588 == _tmp586)return fs;
return({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->hd=_tmp587,_tmp589->tl=_tmp588;_tmp589;});};}
# 3377
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3380
struct _tuple1 _tmp58A=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp58B=_tmp58A;struct Cyc_List_List*_tmp58F;struct Cyc_List_List*_tmp58E;_LL1: _tmp58F=_tmp58B.f1;_tmp58E=_tmp58B.f2;_LL2:;{
struct Cyc_List_List*_tmp58C=Cyc_Tcutil_substs(rgn,inst,_tmp58F);
struct Cyc_List_List*_tmp58D=Cyc_Tcutil_substs(rgn,inst,_tmp58E);
if(_tmp58C == _tmp58F  && _tmp58D == _tmp58E)
return rgn_po;else{
# 3386
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp58C,_tmp58D);}};}
# 3389
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3392
void*_tmp590=Cyc_Tcutil_compress(t);void*_tmp591=_tmp590;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5E8;void*_tmp5E7;struct Cyc_List_List*_tmp5E6;void*_tmp5E5;void*_tmp5E4;enum Cyc_Absyn_AggrKind _tmp5E3;struct Cyc_List_List*_tmp5E2;struct Cyc_List_List*_tmp5E1;struct Cyc_List_List*_tmp5E0;void*_tmp5DF;struct Cyc_Absyn_Tqual _tmp5DE;void*_tmp5DD;struct Cyc_List_List*_tmp5DC;int _tmp5DB;struct Cyc_Absyn_VarargInfo*_tmp5DA;struct Cyc_List_List*_tmp5D9;struct Cyc_List_List*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D6;void*_tmp5D5;struct Cyc_Absyn_Tqual _tmp5D4;void*_tmp5D3;void*_tmp5D2;void*_tmp5D1;void*_tmp5D0;void*_tmp5CF;struct Cyc_Absyn_Tqual _tmp5CE;struct Cyc_Absyn_Exp*_tmp5CD;void*_tmp5CC;unsigned int _tmp5CB;struct _tuple2*_tmp5CA;struct Cyc_List_List*_tmp5C9;struct Cyc_Absyn_Typedefdecl*_tmp5C8;void*_tmp5C7;struct Cyc_Absyn_Tvar*_tmp5C6;switch(*((int*)_tmp591)){case 2U: _LL1: _tmp5C6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp591)->f1;_LL2: {
# 3395
struct _handler_cons _tmp592;_push_handler(& _tmp592);{int _tmp594=0;if(setjmp(_tmp592.handler))_tmp594=1;if(!_tmp594){{void*_tmp595=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp5C6);_npop_handler(0U);return _tmp595;};_pop_handler();}else{void*_tmp593=(void*)_exn_thrown;void*_tmp596=_tmp593;void*_tmp597;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp596)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp597=_tmp596;_LL1F:(int)_rethrow(_tmp597);}_LL1B:;}};}case 8U: _LL3: _tmp5CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp591)->f1;_tmp5C9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp591)->f2;_tmp5C8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp591)->f3;_tmp5C7=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp591)->f4;_LL4: {
# 3398
struct Cyc_List_List*_tmp598=Cyc_Tcutil_substs(rgn,inst,_tmp5C9);
return _tmp598 == _tmp5C9?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->tag=8U,_tmp599->f1=_tmp5CA,_tmp599->f2=_tmp598,_tmp599->f3=_tmp5C8,_tmp599->f4=_tmp5C7;_tmp599;});}case 4U: _LL5: _tmp5CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp591)->f1).elt_type;_tmp5CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp591)->f1).tq;_tmp5CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp591)->f1).num_elts;_tmp5CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp591)->f1).zero_term;_tmp5CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp591)->f1).zt_loc;_LL6: {
# 3401
void*_tmp59A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5CF);
struct Cyc_Absyn_Exp*_tmp59B=_tmp5CD == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5CD);
void*_tmp59C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5CC);
return(_tmp59A == _tmp5CF  && _tmp59B == _tmp5CD) && _tmp59C == _tmp5CC?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));
_tmp59D->tag=4U,(_tmp59D->f1).elt_type=_tmp59A,(_tmp59D->f1).tq=_tmp5CE,(_tmp59D->f1).num_elts=_tmp59B,(_tmp59D->f1).zero_term=_tmp59C,(_tmp59D->f1).zt_loc=_tmp5CB;_tmp59D;});}case 3U: _LL7: _tmp5D5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).elt_type;_tmp5D4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).elt_tq;_tmp5D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).rgn;_tmp5D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).nullable;_tmp5D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).bounds;_tmp5D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).zero_term;_LL8: {
# 3407
void*_tmp59E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D5);
void*_tmp59F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D3);
void*_tmp5A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D1);
void*_tmp5A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D0);
if(((_tmp59E == _tmp5D5  && _tmp59F == _tmp5D3) && _tmp5A0 == _tmp5D1) && _tmp5A1 == _tmp5D0)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9BA;_tmp9BA.elt_type=_tmp59E,_tmp9BA.elt_tq=_tmp5D4,(_tmp9BA.ptr_atts).rgn=_tmp59F,(_tmp9BA.ptr_atts).nullable=_tmp5D2,(_tmp9BA.ptr_atts).bounds=_tmp5A0,(_tmp9BA.ptr_atts).zero_term=_tmp5A1,(_tmp9BA.ptr_atts).ptrloc=0;_tmp9BA;}));}case 5U: _LL9: _tmp5E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).tvars;_tmp5DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).effect;_tmp5DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).ret_tqual;_tmp5DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).ret_type;_tmp5DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).args;_tmp5DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).c_varargs;_tmp5DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).cyc_varargs;_tmp5D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).rgn_po;_tmp5D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).attributes;_tmp5D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).requires_clause;_tmp5D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591)->f1).ensures_clause;_LLA:
# 3417
{struct Cyc_List_List*_tmp5A2=_tmp5E0;for(0;_tmp5A2 != 0;_tmp5A2=_tmp5A2->tl){
inst=({struct Cyc_List_List*_tmp5A4=_region_malloc(rgn,sizeof(*_tmp5A4));({struct _tuple15*_tmpB54=({struct _tuple15*_tmp5A3=_region_malloc(rgn,sizeof(*_tmp5A3));_tmp5A3->f1=(struct Cyc_Absyn_Tvar*)_tmp5A2->hd,({void*_tmpB53=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp5A2->hd);_tmp5A3->f2=_tmpB53;});_tmp5A3;});_tmp5A4->hd=_tmpB54;}),_tmp5A4->tl=inst;_tmp5A4;});}}{
struct _tuple1 _tmp5A5=({struct _RegionHandle*_tmpB56=rgn;struct _RegionHandle*_tmpB55=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpB56,_tmpB55,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp5DC));});
# 3419
struct _tuple1 _tmp5A6=_tmp5A5;struct Cyc_List_List*_tmp5B5;struct Cyc_List_List*_tmp5B4;_LL21: _tmp5B5=_tmp5A6.f1;_tmp5B4=_tmp5A6.f2;_LL22:;{
# 3421
struct Cyc_List_List*_tmp5A7=_tmp5DC;
struct Cyc_List_List*_tmp5A8=Cyc_Tcutil_substs(rgn,inst,_tmp5B4);
if(_tmp5A8 != _tmp5B4)
_tmp5A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp5DC,_tmp5A8);{
void*eff2;
if(_tmp5DF == 0)
eff2=0;else{
# 3429
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5DF);
if(new_eff == _tmp5DF)
eff2=_tmp5DF;else{
# 3433
eff2=new_eff;}}{
# 3435
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp5DA == 0)
cyc_varargs2=0;else{
# 3439
struct Cyc_Absyn_VarargInfo _tmp5A9=*_tmp5DA;struct Cyc_Absyn_VarargInfo _tmp5AA=_tmp5A9;struct _dyneither_ptr*_tmp5B0;struct Cyc_Absyn_Tqual _tmp5AF;void*_tmp5AE;int _tmp5AD;_LL24: _tmp5B0=_tmp5AA.name;_tmp5AF=_tmp5AA.tq;_tmp5AE=_tmp5AA.type;_tmp5AD=_tmp5AA.inject;_LL25:;{
void*_tmp5AB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5AE);
if(_tmp5AB == _tmp5AE)cyc_varargs2=_tmp5DA;else{
# 3443
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC->name=_tmp5B0,_tmp5AC->tq=_tmp5AF,_tmp5AC->type=_tmp5AB,_tmp5AC->inject=_tmp5AD;_tmp5AC;});}};}{
# 3445
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5D9);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5D7);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5D6);
struct Cyc_List_List*_tmp5B1=Cyc_Tcutil_extract_relns(_tmp5A7,req2);
struct Cyc_List_List*_tmp5B2=Cyc_Tcutil_extract_relns(_tmp5A7,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));
_tmp5B3->tag=5U,(_tmp5B3->f1).tvars=_tmp5E0,(_tmp5B3->f1).effect=eff2,(_tmp5B3->f1).ret_tqual=_tmp5DE,({void*_tmpB57=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5DD);(_tmp5B3->f1).ret_type=_tmpB57;}),(_tmp5B3->f1).args=_tmp5A7,(_tmp5B3->f1).c_varargs=_tmp5DB,(_tmp5B3->f1).cyc_varargs=cyc_varargs2,(_tmp5B3->f1).rgn_po=rgn_po2,(_tmp5B3->f1).attributes=_tmp5D8,(_tmp5B3->f1).requires_clause=req2,(_tmp5B3->f1).requires_relns=_tmp5B1,(_tmp5B3->f1).ensures_clause=ens2,(_tmp5B3->f1).ensures_relns=_tmp5B2;_tmp5B3;});};};};};};case 6U: _LLB: _tmp5E1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp591)->f1;_LLC: {
# 3455
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp5B6=_tmp5E1;for(0;_tmp5B6 != 0;_tmp5B6=_tmp5B6->tl){
void*_tmp5B7=(*((struct _tuple12*)_tmp5B6->hd)).f2;
void*_tmp5B8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B7);
if(_tmp5B7 != _tmp5B8)
change=1;
# 3463
ts2=({struct Cyc_List_List*_tmp5B9=_region_malloc(rgn,sizeof(*_tmp5B9));_tmp5B9->hd=_tmp5B8,_tmp5B9->tl=ts2;_tmp5B9;});}}
# 3465
if(!change)
return t;{
struct Cyc_List_List*_tmp5BA=({struct Cyc_List_List*_tmpB58=_tmp5E1;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpB58,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->tag=6U,_tmp5BB->f1=_tmp5BA;_tmp5BB;});};}case 7U: _LLD: _tmp5E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp591)->f1;_tmp5E2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp591)->f2;_LLE: {
# 3470
struct Cyc_List_List*_tmp5BC=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp5E2);
if(_tmp5E2 == _tmp5BC)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->tag=7U,_tmp5BD->f1=_tmp5E3,_tmp5BD->f2=_tmp5BC;_tmp5BD;});}case 1U: _LLF: _tmp5E4=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp591)->f2;_LL10:
# 3474
 if(_tmp5E4 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E4);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp591)->f2 == 0){_LL11: _tmp5E5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp591)->f1;_LL12:
 return t;}else{_LL13: _tmp5E7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp591)->f1;_tmp5E6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp591)->f2;_LL14: {
# 3478
struct Cyc_List_List*_tmp5BE=Cyc_Tcutil_substs(rgn,inst,_tmp5E6);
if(_tmp5E6 == _tmp5BE)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->tag=0U,_tmp5BF->f1=_tmp5E7,_tmp5BF->f2=_tmp5BE;_tmp5BF;});}}}case 9U: _LL15: _tmp5E8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp591)->f1;_LL16: {
# 3482
struct Cyc_Absyn_Exp*_tmp5C0=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5E8);
return _tmp5C0 == _tmp5E8?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->tag=9U,_tmp5C1->f1=_tmp5C0;_tmp5C1;});}case 11U: _LL17: _tmp5E9=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp591)->f1;_LL18: {
# 3485
struct Cyc_Absyn_Exp*_tmp5C2=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5E9);
return _tmp5C2 == _tmp5E9?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->tag=11U,_tmp5C3->f1=_tmp5C2;_tmp5C3;});}default: _LL19: _LL1A:
({void*_tmp5C4=0U;({struct _dyneither_ptr _tmpB59=({const char*_tmp5C5="found typedecltype in rsubs";_tag_dyneither(_tmp5C5,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB59,_tag_dyneither(_tmp5C4,sizeof(void*),0U));});});}_LL0:;}
# 3491
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3494
if(ts == 0)
return 0;{
void*_tmp5EA=(void*)ts->hd;
struct Cyc_List_List*_tmp5EB=ts->tl;
void*_tmp5EC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5EA);
struct Cyc_List_List*_tmp5ED=Cyc_Tcutil_substs(rgn,inst,_tmp5EB);
if(_tmp5EA == _tmp5EC  && _tmp5EB == _tmp5ED)
return ts;
return({struct Cyc_List_List*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->hd=_tmp5EC,_tmp5EE->tl=_tmp5ED;_tmp5EE;});};}
# 3505
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3512
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp5EF=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->f1=tv,({void*_tmpB5B=({struct Cyc_Core_Opt*_tmpB5A=_tmp5EF;Cyc_Absyn_new_evar(_tmpB5A,({struct Cyc_Core_Opt*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->v=s;_tmp5F0;}));});_tmp5F1->f2=_tmpB5B;});_tmp5F1;});}
# 3517
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,struct Cyc_Absyn_Tvar*tv){
# 3519
struct _tuple16*_tmp5F2=env;struct Cyc_List_List*_tmp5F7;struct _RegionHandle*_tmp5F6;_LL1: _tmp5F7=_tmp5F2->f1;_tmp5F6=_tmp5F2->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp5F3=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F5=_region_malloc(_tmp5F6,sizeof(*_tmp5F5));_tmp5F5->f1=tv,({void*_tmpB5D=({struct Cyc_Core_Opt*_tmpB5C=_tmp5F3;Cyc_Absyn_new_evar(_tmpB5C,({struct Cyc_Core_Opt*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->v=_tmp5F7;_tmp5F4;}));});_tmp5F5->f2=_tmpB5D;});_tmp5F5;});};}
# 3529
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3533
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp5FA=({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0U,_tmp9BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmp9BD;});struct Cyc_String_pa_PrintArg_struct _tmp5FB=({struct Cyc_String_pa_PrintArg_struct _tmp9BC;_tmp9BC.tag=0U,({
struct _dyneither_ptr _tmpB5E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp9BC.f1=_tmpB5E;});_tmp9BC;});struct Cyc_String_pa_PrintArg_struct _tmp5FC=({struct Cyc_String_pa_PrintArg_struct _tmp9BB;_tmp9BB.tag=0U,({struct _dyneither_ptr _tmpB5F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmp9BB.f1=_tmpB5F;});_tmp9BB;});void*_tmp5F8[3U];_tmp5F8[0]=& _tmp5FA,_tmp5F8[1]=& _tmp5FB,_tmp5F8[2]=& _tmp5FC;({unsigned int _tmpB61=loc;struct _dyneither_ptr _tmpB60=({const char*_tmp5F9="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp5F9,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpB61,_tmpB60,_tag_dyneither(_tmp5F8,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp5FD=0U;({struct _dyneither_ptr _tmpB62=({const char*_tmp5FE="same type variable has different identity!";_tag_dyneither(_tmp5FE,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB62,_tag_dyneither(_tmp5FD,sizeof(void*),0U));});});}
return tvs;}}}
# 3547
({int _tmpB63=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpB63;});
return({struct Cyc_List_List*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->hd=tv,_tmp5FF->tl=tvs;_tmp5FF;});}
# 3553
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3555
if(tv->identity == - 1)
({void*_tmp600=0U;({struct _dyneither_ptr _tmpB64=({const char*_tmp601="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp601,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB64,_tag_dyneither(_tmp600,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3559
struct Cyc_Absyn_Tvar*_tmp602=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp602->identity == - 1)
({void*_tmp603=0U;({struct _dyneither_ptr _tmpB65=({const char*_tmp604="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp604,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB65,_tag_dyneither(_tmp603,sizeof(void*),0U));});});
if(_tmp602->identity == tv->identity)
return tvs;}}
# 3566
return({struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->hd=tv,_tmp605->tl=tvs;_tmp605;});}struct _tuple29{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3572
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3577
if(tv->identity == - 1)
({void*_tmp606=0U;({struct _dyneither_ptr _tmpB66=({const char*_tmp607="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp607,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB66,_tag_dyneither(_tmp606,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3581
struct _tuple29*_tmp608=(struct _tuple29*)tvs2->hd;struct _tuple29*_tmp609=_tmp608;struct Cyc_Absyn_Tvar*_tmp60D;int*_tmp60C;_LL1: _tmp60D=_tmp609->f1;_tmp60C=(int*)& _tmp609->f2;_LL2:;
if(_tmp60D->identity == - 1)
({void*_tmp60A=0U;({struct _dyneither_ptr _tmpB67=({const char*_tmp60B="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp60B,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB67,_tag_dyneither(_tmp60A,sizeof(void*),0U));});});
if(_tmp60D->identity == tv->identity){
*_tmp60C=*_tmp60C  || b;
return tvs;}}}
# 3589
return({struct Cyc_List_List*_tmp60F=_region_malloc(r,sizeof(*_tmp60F));({struct _tuple29*_tmpB68=({struct _tuple29*_tmp60E=_region_malloc(r,sizeof(*_tmp60E));_tmp60E->f1=tv,_tmp60E->f2=b;_tmp60E;});_tmp60F->hd=_tmpB68;}),_tmp60F->tl=tvs;_tmp60F;});}
# 3593
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3595
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp612=({struct Cyc_String_pa_PrintArg_struct _tmp9BE;_tmp9BE.tag=0U,({struct _dyneither_ptr _tmpB69=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp9BE.f1=_tmpB69;});_tmp9BE;});void*_tmp610[1U];_tmp610[0]=& _tmp612;({struct _dyneither_ptr _tmpB6A=({const char*_tmp611="bound tvar id for %s is NULL";_tag_dyneither(_tmp611,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB6A,_tag_dyneither(_tmp610,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->hd=tv,_tmp613->tl=tvs;_tmp613;});}struct _tuple30{void*f1;int f2;};
# 3604
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3607
void*_tmp614=Cyc_Tcutil_compress(e);void*_tmp615=_tmp614;int _tmp61F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp615)->tag == 1U){_LL1: _tmp61F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp615)->f3;_LL2:
# 3609
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple30*_tmp616=(struct _tuple30*)es2->hd;struct _tuple30*_tmp617=_tmp616;void*_tmp61C;int*_tmp61B;_LL6: _tmp61C=_tmp617->f1;_tmp61B=(int*)& _tmp617->f2;_LL7:;{
void*_tmp618=Cyc_Tcutil_compress(_tmp61C);void*_tmp619=_tmp618;int _tmp61A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp619)->tag == 1U){_LL9: _tmp61A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp619)->f3;_LLA:
# 3613
 if(_tmp61F == _tmp61A){
if(b != *_tmp61B)*_tmp61B=1;
return es;}
# 3617
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3621
return({struct Cyc_List_List*_tmp61E=_region_malloc(r,sizeof(*_tmp61E));({struct _tuple30*_tmpB6B=({struct _tuple30*_tmp61D=_region_malloc(r,sizeof(*_tmp61D));_tmp61D->f1=e,_tmp61D->f2=b;_tmp61D;});_tmp61E->hd=_tmpB6B;}),_tmp61E->tl=es;_tmp61E;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3626
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3629
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3638
if(!present)r=({struct Cyc_List_List*_tmp620=_region_malloc(rgn,sizeof(*_tmp620));_tmp620->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp620->tl=r;_tmp620;});}
# 3640
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3645
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3649
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp621=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp622=_tmp621;struct Cyc_Absyn_Tvar*_tmp625;int _tmp624;_LL1: _tmp625=_tmp622.f1;_tmp624=_tmp622.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp625->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3659
if(!present)res=({struct Cyc_List_List*_tmp623=_region_malloc(r,sizeof(*_tmp623));_tmp623->hd=(struct _tuple29*)tvs->hd,_tmp623->tl=res;_tmp623;});};}
# 3661
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3665
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_type,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3667
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp628=({struct Cyc_String_pa_PrintArg_struct _tmp9BF;_tmp9BF.tag=0U,_tmp9BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9BF;});void*_tmp626[1U];_tmp626[0]=& _tmp628;({unsigned int _tmpB6D=loc;struct _dyneither_ptr _tmpB6C=({const char*_tmp627="bitfield %s does not have constant width";_tag_dyneither(_tmp627,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpB6D,_tmpB6C,_tag_dyneither(_tmp626,sizeof(void*),1U));});});else{
# 3672
struct _tuple13 _tmp629=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _tmp62A=_tmp629;unsigned int _tmp630;int _tmp62F;_LL1: _tmp630=_tmp62A.f1;_tmp62F=_tmp62A.f2;_LL2:;
if(!_tmp62F)
({void*_tmp62B=0U;({unsigned int _tmpB6F=loc;struct _dyneither_ptr _tmpB6E=({const char*_tmp62C="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp62C,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpB6F,_tmpB6E,_tag_dyneither(_tmp62B,sizeof(void*),0U));});});
if((int)_tmp630 < 0)
({void*_tmp62D=0U;({unsigned int _tmpB71=loc;struct _dyneither_ptr _tmpB70=({const char*_tmp62E="bitfield has negative width";_tag_dyneither(_tmp62E,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpB71,_tmpB70,_tag_dyneither(_tmp62D,sizeof(void*),0U));});});
w=_tmp630;}{
# 3679
void*_tmp631=Cyc_Tcutil_compress(field_type);void*_tmp632=_tmp631;enum Cyc_Absyn_Size_of _tmp640;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp632)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp632)->f1)->tag == 1U){_LL4: _tmp640=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp632)->f1)->f2;_LL5:
# 3682
{enum Cyc_Absyn_Size_of _tmp633=_tmp640;switch(_tmp633){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned int)8)({void*_tmp634=0U;({unsigned int _tmpB73=loc;struct _dyneither_ptr _tmpB72=({const char*_tmp635="bitfield larger than type";_tag_dyneither(_tmp635,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB73,_tmpB72,_tag_dyneither(_tmp634,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned int)16)({void*_tmp636=0U;({unsigned int _tmpB75=loc;struct _dyneither_ptr _tmpB74=({const char*_tmp637="bitfield larger than type";_tag_dyneither(_tmp637,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB75,_tmpB74,_tag_dyneither(_tmp636,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned int)32)({void*_tmp638=0U;({unsigned int _tmpB77=loc;struct _dyneither_ptr _tmpB76=({const char*_tmp639="bitfield larger than type";_tag_dyneither(_tmp639,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB77,_tmpB76,_tag_dyneither(_tmp638,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned int)64)({void*_tmp63A=0U;({unsigned int _tmpB79=loc;struct _dyneither_ptr _tmpB78=({const char*_tmp63B="bitfield larger than type";_tag_dyneither(_tmp63B,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpB79,_tmpB78,_tag_dyneither(_tmp63A,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3690
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 3692
({struct Cyc_String_pa_PrintArg_struct _tmp63E=({struct Cyc_String_pa_PrintArg_struct _tmp9C1;_tmp9C1.tag=0U,_tmp9C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9C1;});struct Cyc_String_pa_PrintArg_struct _tmp63F=({struct Cyc_String_pa_PrintArg_struct _tmp9C0;_tmp9C0.tag=0U,({
struct _dyneither_ptr _tmpB7A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_type));_tmp9C0.f1=_tmpB7A;});_tmp9C0;});void*_tmp63C[2U];_tmp63C[0]=& _tmp63E,_tmp63C[1]=& _tmp63F;({unsigned int _tmpB7C=loc;struct _dyneither_ptr _tmpB7B=({const char*_tmp63D="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp63D,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpB7C,_tmpB7B,_tag_dyneither(_tmp63C,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3699
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp641=(void*)atts->hd;void*_tmp642=_tmp641;switch(*((int*)_tmp642)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp645=({struct Cyc_String_pa_PrintArg_struct _tmp9C3;_tmp9C3.tag=0U,({
struct _dyneither_ptr _tmpB7D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp9C3.f1=_tmpB7D;});_tmp9C3;});struct Cyc_String_pa_PrintArg_struct _tmp646=({struct Cyc_String_pa_PrintArg_struct _tmp9C2;_tmp9C2.tag=0U,_tmp9C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9C2;});void*_tmp643[2U];_tmp643[0]=& _tmp645,_tmp643[1]=& _tmp646;({unsigned int _tmpB7F=loc;struct _dyneither_ptr _tmpB7E=({const char*_tmp644="bad attribute %s on member %s";_tag_dyneither(_tmp644,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpB7F,_tmpB7E,_tag_dyneither(_tmp643,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3727
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp647=t;struct Cyc_Absyn_Typedefdecl*_tmp64B;void*_tmp64A;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp647)->tag == 8U){_LL1: _tmp64B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp647)->f3;_tmp64A=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp647)->f4;_LL2:
# 3730
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp64B))->tq).real_const  || (_tmp64B->tq).print_const){
if(declared_const)({void*_tmp648=0U;({unsigned int _tmpB81=loc;struct _dyneither_ptr _tmpB80=({const char*_tmp649="extra const";_tag_dyneither(_tmp649,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpB81,_tmpB80,_tag_dyneither(_tmp648,sizeof(void*),0U));});});
return 1;}
# 3735
if((unsigned int)_tmp64A)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp64A);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3742
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp64C=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp64D=_tmp64C;void*_tmp651;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64D)->tag == 3U){_LL1: _tmp651=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64D)->f1).ptr_atts).rgn;_LL2:
# 3747
{void*_tmp64E=Cyc_Tcutil_compress(_tmp651);void*_tmp64F=_tmp64E;struct Cyc_Absyn_Tvar*_tmp650;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp64F)->tag == 2U){_LL6: _tmp650=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp64F)->f1;_LL7:
# 3749
 return Cyc_Absyn_tvar_cmp(tvar,_tmp650)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3752
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3758
return 1;}
return 0;}
# 3762
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3765
void*_tmp652=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp653=_tmp652;switch(*((int*)_tmp653)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp653)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp653)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp653)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp653)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3774
 if((((int)expected_kind->kind == (int)Cyc_Absyn_BoxKind  || (int)expected_kind->kind == (int)Cyc_Absyn_MemKind) || (int)expected_kind->kind == (int)Cyc_Absyn_AnyKind) && 
# 3777
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3783
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3789
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple31{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3793
static struct _tuple31 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3796
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp656=({struct Cyc_String_pa_PrintArg_struct _tmp9C5;_tmp9C5.tag=0U,_tmp9C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9C5;});struct Cyc_String_pa_PrintArg_struct _tmp657=({struct Cyc_String_pa_PrintArg_struct _tmp9C4;_tmp9C4.tag=0U,({
struct _dyneither_ptr _tmpB82=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp9C4.f1=_tmpB82;});_tmp9C4;});void*_tmp654[2U];_tmp654[0]=& _tmp656,_tmp654[1]=& _tmp657;({unsigned int _tmpB84=loc;struct _dyneither_ptr _tmpB83=({const char*_tmp655="%s clause has type %s instead of integral type";_tag_dyneither(_tmp655,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpB84,_tmpB83,_tag_dyneither(_tmp654,sizeof(void*),2U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp65A=({struct Cyc_String_pa_PrintArg_struct _tmp9C7;_tmp9C7.tag=0U,_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9C7;});struct Cyc_String_pa_PrintArg_struct _tmp65B=({struct Cyc_String_pa_PrintArg_struct _tmp9C6;_tmp9C6.tag=0U,({
struct _dyneither_ptr _tmpB85=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmp9C6.f1=_tmpB85;});_tmp9C6;});void*_tmp658[2U];_tmp658[0]=& _tmp65A,_tmp658[1]=& _tmp65B;({unsigned int _tmpB87=clause->loc;struct _dyneither_ptr _tmpB86=({const char*_tmp659="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp659,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpB87,_tmpB86,_tag_dyneither(_tmp658,sizeof(void*),2U));});});}
# 3808
return({struct _tuple31 _tmp9C8;_tmp9C8.f1=cvtenv,_tmp9C8.f2=relns;_tmp9C8;});}
# 3838 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr);
# 3845
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_aggr(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 3851
{union Cyc_Absyn_AggrInfo _tmp65C=*info;union Cyc_Absyn_AggrInfo _tmp65D=_tmp65C;struct Cyc_Absyn_Aggrdecl*_tmp680;enum Cyc_Absyn_AggrKind _tmp67F;struct _tuple2*_tmp67E;struct Cyc_Core_Opt*_tmp67D;if((_tmp65D.UnknownAggr).tag == 1){_LL1: _tmp67F=((_tmp65D.UnknownAggr).val).f1;_tmp67E=((_tmp65D.UnknownAggr).val).f2;_tmp67D=((_tmp65D.UnknownAggr).val).f3;_LL2: {
# 3853
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp65E;_push_handler(& _tmp65E);{int _tmp660=0;if(setjmp(_tmp65E.handler))_tmp660=1;if(!_tmp660){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp67E);{
struct Cyc_Absyn_Aggrdecl*_tmp661=*adp;
if((int)_tmp661->kind != (int)_tmp67F){
if((int)_tmp661->kind == (int)Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp664=({struct Cyc_String_pa_PrintArg_struct _tmp9CA;_tmp9CA.tag=0U,({
struct _dyneither_ptr _tmpB88=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67E));_tmp9CA.f1=_tmpB88;});_tmp9CA;});struct Cyc_String_pa_PrintArg_struct _tmp665=({struct Cyc_String_pa_PrintArg_struct _tmp9C9;_tmp9C9.tag=0U,({struct _dyneither_ptr _tmpB89=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67E));_tmp9C9.f1=_tmpB89;});_tmp9C9;});void*_tmp662[2U];_tmp662[0]=& _tmp664,_tmp662[1]=& _tmp665;({unsigned int _tmpB8B=loc;struct _dyneither_ptr _tmpB8A=({const char*_tmp663="expecting struct %s instead of union %s";_tag_dyneither(_tmp663,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpB8B,_tmpB8A,_tag_dyneither(_tmp662,sizeof(void*),2U));});});else{
# 3862
({struct Cyc_String_pa_PrintArg_struct _tmp668=({struct Cyc_String_pa_PrintArg_struct _tmp9CC;_tmp9CC.tag=0U,({
struct _dyneither_ptr _tmpB8C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67E));_tmp9CC.f1=_tmpB8C;});_tmp9CC;});struct Cyc_String_pa_PrintArg_struct _tmp669=({struct Cyc_String_pa_PrintArg_struct _tmp9CB;_tmp9CB.tag=0U,({struct _dyneither_ptr _tmpB8D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67E));_tmp9CB.f1=_tmpB8D;});_tmp9CB;});void*_tmp666[2U];_tmp666[0]=& _tmp668,_tmp666[1]=& _tmp669;({unsigned int _tmpB8F=loc;struct _dyneither_ptr _tmpB8E=({const char*_tmp667="expecting union %s instead of struct %s";_tag_dyneither(_tmp667,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpB8F,_tmpB8E,_tag_dyneither(_tmp666,sizeof(void*),2U));});});}}
# 3865
if((unsigned int)_tmp67D  && (int)_tmp67D->v){
if(!((unsigned int)_tmp661->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp661->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp66C=({struct Cyc_String_pa_PrintArg_struct _tmp9CD;_tmp9CD.tag=0U,({
struct _dyneither_ptr _tmpB90=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67E));_tmp9CD.f1=_tmpB90;});_tmp9CD;});void*_tmp66A[1U];_tmp66A[0]=& _tmp66C;({unsigned int _tmpB92=loc;struct _dyneither_ptr _tmpB91=({const char*_tmp66B="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp66B,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpB92,_tmpB91,_tag_dyneither(_tmp66A,sizeof(void*),1U));});});}
# 3871
({union Cyc_Absyn_AggrInfo _tmpB93=Cyc_Absyn_KnownAggr(adp);*info=_tmpB93;});};
# 3855
;_pop_handler();}else{void*_tmp65F=(void*)_exn_thrown;void*_tmp66D=_tmp65F;void*_tmp673;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp66D)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3875
struct Cyc_Absyn_Aggrdecl*_tmp66E=({struct Cyc_Absyn_Aggrdecl*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->kind=_tmp67F,_tmp672->sc=Cyc_Absyn_Extern,_tmp672->name=_tmp67E,_tmp672->tvs=0,_tmp672->impl=0,_tmp672->attributes=0,_tmp672->expected_mem_kind=0;_tmp672;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp66E);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp67E);
({union Cyc_Absyn_AggrInfo _tmpB94=Cyc_Absyn_KnownAggr(adp);*info=_tmpB94;});
# 3880
if(*targs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp671=({struct Cyc_String_pa_PrintArg_struct _tmp9CE;_tmp9CE.tag=0U,({struct _dyneither_ptr _tmpB95=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp67E));_tmp9CE.f1=_tmpB95;});_tmp9CE;});void*_tmp66F[1U];_tmp66F[0]=& _tmp671;({unsigned int _tmpB97=loc;struct _dyneither_ptr _tmpB96=({const char*_tmp670="declare parameterized type %s before using";_tag_dyneither(_tmp670,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpB97,_tmpB96,_tag_dyneither(_tmp66F,sizeof(void*),1U));});});
return cvtenv;}
# 3884
goto _LL5;}}else{_LL8: _tmp673=_tmp66D;_LL9:(int)_rethrow(_tmp673);}_LL5:;}};}
# 3886
_tmp680=*adp;goto _LL4;}}else{_LL3: _tmp680=*(_tmp65D.KnownAggr).val;_LL4: {
# 3888
struct Cyc_List_List*tvs=_tmp680->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp674=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp675=(void*)ts->hd;
# 3896
{struct _tuple0 _tmp676=({struct _tuple0 _tmp9CF;({void*_tmpB98=Cyc_Absyn_compress_kb(_tmp674->kind);_tmp9CF.f1=_tmpB98;}),_tmp9CF.f2=_tmp675;_tmp9CF;});struct _tuple0 _tmp677=_tmp676;struct Cyc_Absyn_Tvar*_tmp678;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp677.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp677.f2)->tag == 2U){_LLB: _tmp678=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp677.f2)->f1;_LLC:
# 3898
({struct Cyc_List_List*_tmpB99=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp678);cvtenv.kind_env=_tmpB99;});
({struct Cyc_List_List*_tmpB9A=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp678,1);cvtenv.free_vars=_tmpB9A;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3903
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 3907
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp67B=({struct Cyc_String_pa_PrintArg_struct _tmp9D0;_tmp9D0.tag=0U,({struct _dyneither_ptr _tmpB9B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp680->name));_tmp9D0.f1=_tmpB9B;});_tmp9D0;});void*_tmp679[1U];_tmp679[0]=& _tmp67B;({unsigned int _tmpB9D=loc;struct _dyneither_ptr _tmpB9C=({const char*_tmp67A="too many parameters for type %s";_tag_dyneither(_tmp67A,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpB9D,_tmpB9C,_tag_dyneither(_tmp679,sizeof(void*),1U));});});
if(tvs != 0){
# 3911
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->hd=e,_tmp67C->tl=hidden_ts;_tmp67C;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 3918
({struct Cyc_List_List*_tmpB9F=({struct Cyc_List_List*_tmpB9E=*targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB9E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs=_tmpB9F;});}
# 3920
if((allow_abs_aggr  && _tmp680->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 3925
_tmp680->expected_mem_kind=1;}}_LL0:;}
# 3928
return cvtenv;}
# 3932
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 3938
struct Cyc_List_List*_tmp681=*targsp;
{union Cyc_Absyn_DatatypeInfo _tmp682=*info;union Cyc_Absyn_DatatypeInfo _tmp683=_tmp682;struct Cyc_Absyn_Datatypedecl*_tmp69A;struct _tuple2*_tmp699;int _tmp698;if((_tmp683.UnknownDatatype).tag == 1){_LL1: _tmp699=((_tmp683.UnknownDatatype).val).name;_tmp698=((_tmp683.UnknownDatatype).val).is_extensible;_LL2: {
# 3941
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp684;_push_handler(& _tmp684);{int _tmp686=0;if(setjmp(_tmp684.handler))_tmp686=1;if(!_tmp686){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp699);;_pop_handler();}else{void*_tmp685=(void*)_exn_thrown;void*_tmp687=_tmp685;void*_tmp68D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp687)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3945
struct Cyc_Absyn_Datatypedecl*_tmp688=({struct Cyc_Absyn_Datatypedecl*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->sc=Cyc_Absyn_Extern,_tmp68C->name=_tmp699,_tmp68C->tvs=0,_tmp68C->fields=0,_tmp68C->is_extensible=_tmp698;_tmp68C;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp688);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp699);
# 3949
if(_tmp681 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp68B=({struct Cyc_String_pa_PrintArg_struct _tmp9D1;_tmp9D1.tag=0U,({
struct _dyneither_ptr _tmpBA0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp699));_tmp9D1.f1=_tmpBA0;});_tmp9D1;});void*_tmp689[1U];_tmp689[0]=& _tmp68B;({unsigned int _tmpBA2=loc;struct _dyneither_ptr _tmpBA1=({const char*_tmp68A="declare parameterized datatype %s before using";_tag_dyneither(_tmp68A,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBA2,_tmpBA1,_tag_dyneither(_tmp689,sizeof(void*),1U));});});
return cvtenv;}
# 3954
goto _LL5;}}else{_LL8: _tmp68D=_tmp687;_LL9:(int)_rethrow(_tmp68D);}_LL5:;}};}
# 3958
if(_tmp698  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp690=({struct Cyc_String_pa_PrintArg_struct _tmp9D2;_tmp9D2.tag=0U,({struct _dyneither_ptr _tmpBA3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp699));_tmp9D2.f1=_tmpBA3;});_tmp9D2;});void*_tmp68E[1U];_tmp68E[0]=& _tmp690;({unsigned int _tmpBA5=loc;struct _dyneither_ptr _tmpBA4=({const char*_tmp68F="datatype %s was not declared @extensible";_tag_dyneither(_tmp68F,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpBA5,_tmpBA4,_tag_dyneither(_tmp68E,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfo _tmpBA6=Cyc_Absyn_KnownDatatype(tudp);*info=_tmpBA6;});
_tmp69A=*tudp;goto _LL4;}}else{_LL3: _tmp69A=*(_tmp683.KnownDatatype).val;_LL4: {
# 3964
struct Cyc_List_List*tvs=_tmp69A->tvs;
for(0;_tmp681 != 0  && tvs != 0;(_tmp681=_tmp681->tl,tvs=tvs->tl)){
void*t=(void*)_tmp681->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3970
{struct _tuple0 _tmp691=({struct _tuple0 _tmp9D3;({void*_tmpBA7=Cyc_Absyn_compress_kb(tv->kind);_tmp9D3.f1=_tmpBA7;}),_tmp9D3.f2=t;_tmp9D3;});struct _tuple0 _tmp692=_tmp691;struct Cyc_Absyn_Tvar*_tmp693;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp692.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp692.f2)->tag == 2U){_LLB: _tmp693=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp692.f2)->f1;_LLC:
# 3972
({struct Cyc_List_List*_tmpBA8=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp693);cvtenv.kind_env=_tmpBA8;});
({struct Cyc_List_List*_tmpBA9=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp693,1);cvtenv.free_vars=_tmpBA9;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3977
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3981
if(_tmp681 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp696=({struct Cyc_String_pa_PrintArg_struct _tmp9D4;_tmp9D4.tag=0U,({
struct _dyneither_ptr _tmpBAA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69A->name));_tmp9D4.f1=_tmpBAA;});_tmp9D4;});void*_tmp694[1U];_tmp694[0]=& _tmp696;({unsigned int _tmpBAC=loc;struct _dyneither_ptr _tmpBAB=({const char*_tmp695="too many type arguments for datatype %s";_tag_dyneither(_tmp695,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpBAC,_tmpBAB,_tag_dyneither(_tmp694,sizeof(void*),1U));});});
if(tvs != 0){
# 3986
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->hd=e,_tmp697->tl=hidden_ts;_tmp697;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 3993
({struct Cyc_List_List*_tmpBAE=({struct Cyc_List_List*_tmpBAD=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpBAD,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmpBAE;});}
# 3995
goto _LL0;}}_LL0:;}
# 3997
return cvtenv;}
# 4001
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype_field(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 4007
{union Cyc_Absyn_DatatypeFieldInfo _tmp69B=*info;union Cyc_Absyn_DatatypeFieldInfo _tmp69C=_tmp69B;struct Cyc_Absyn_Datatypedecl*_tmp6AF;struct Cyc_Absyn_Datatypefield*_tmp6AE;struct _tuple2*_tmp6AD;struct _tuple2*_tmp6AC;int _tmp6AB;if((_tmp69C.UnknownDatatypefield).tag == 1){_LL1: _tmp6AD=((_tmp69C.UnknownDatatypefield).val).datatype_name;_tmp6AC=((_tmp69C.UnknownDatatypefield).val).field_name;_tmp6AB=((_tmp69C.UnknownDatatypefield).val).is_extensible;_LL2: {
# 4010
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp6AD);
struct Cyc_Absyn_Datatypefield*tuf;
# 4015
{struct Cyc_List_List*_tmp69D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp69D=_tmp69D->tl){
if(_tmp69D == 0)({void*_tmp69E=0U;({struct _dyneither_ptr _tmpBAF=({const char*_tmp69F="Tcutil found a bad datatypefield";_tag_dyneither(_tmp69F,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBAF,_tag_dyneither(_tmp69E,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp69D->hd)->name,_tmp6AC)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp69D->hd;
break;}}}
# 4022
({union Cyc_Absyn_DatatypeFieldInfo _tmpBB0=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmpBB0;});
_tmp6AF=tud;_tmp6AE=tuf;goto _LL4;}}else{_LL3: _tmp6AF=((_tmp69C.KnownDatatypefield).val).f1;_tmp6AE=((_tmp69C.KnownDatatypefield).val).f2;_LL4: {
# 4026
struct Cyc_List_List*tvs=_tmp6AF->tvs;
for(0;targs != 0  && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4032
{struct _tuple0 _tmp6A0=({struct _tuple0 _tmp9D5;({void*_tmpBB1=Cyc_Absyn_compress_kb(tv->kind);_tmp9D5.f1=_tmpBB1;}),_tmp9D5.f2=t;_tmp9D5;});struct _tuple0 _tmp6A1=_tmp6A0;struct Cyc_Absyn_Tvar*_tmp6A2;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6A1.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A1.f2)->tag == 2U){_LL6: _tmp6A2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A1.f2)->f1;_LL7:
# 4034
({struct Cyc_List_List*_tmpBB2=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6A2);cvtenv.kind_env=_tmpBB2;});
({struct Cyc_List_List*_tmpBB3=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp6A2,1);cvtenv.free_vars=_tmpBB3;});
continue;}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}{
# 4039
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4043
if(targs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6A5=({struct Cyc_String_pa_PrintArg_struct _tmp9D7;_tmp9D7.tag=0U,({
struct _dyneither_ptr _tmpBB4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6AF->name));_tmp9D7.f1=_tmpBB4;});_tmp9D7;});struct Cyc_String_pa_PrintArg_struct _tmp6A6=({struct Cyc_String_pa_PrintArg_struct _tmp9D6;_tmp9D6.tag=0U,({struct _dyneither_ptr _tmpBB5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6AE->name));_tmp9D6.f1=_tmpBB5;});_tmp9D6;});void*_tmp6A3[2U];_tmp6A3[0]=& _tmp6A5,_tmp6A3[1]=& _tmp6A6;({unsigned int _tmpBB7=loc;struct _dyneither_ptr _tmpBB6=({const char*_tmp6A4="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp6A4,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpBB7,_tmpBB6,_tag_dyneither(_tmp6A3,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6A9=({struct Cyc_String_pa_PrintArg_struct _tmp9D9;_tmp9D9.tag=0U,({
struct _dyneither_ptr _tmpBB8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6AF->name));_tmp9D9.f1=_tmpBB8;});_tmp9D9;});struct Cyc_String_pa_PrintArg_struct _tmp6AA=({struct Cyc_String_pa_PrintArg_struct _tmp9D8;_tmp9D8.tag=0U,({struct _dyneither_ptr _tmpBB9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6AE->name));_tmp9D8.f1=_tmpBB9;});_tmp9D8;});void*_tmp6A7[2U];_tmp6A7[0]=& _tmp6A9,_tmp6A7[1]=& _tmp6AA;({unsigned int _tmpBBB=loc;struct _dyneither_ptr _tmpBBA=({const char*_tmp6A8="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp6A8,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBBB,_tmpBBA,_tag_dyneither(_tmp6A7,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 4051
return cvtenv;}
# 4054
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_app(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 4059
struct Cyc_List_List*_tmp6B0=*targsp;
{void*_tmp6B1=c;union Cyc_Absyn_DatatypeFieldInfo*_tmp6D8;union Cyc_Absyn_DatatypeInfo*_tmp6D7;union Cyc_Absyn_AggrInfo*_tmp6D6;struct Cyc_List_List*_tmp6D5;struct _tuple2*_tmp6D4;struct Cyc_Absyn_Enumdecl**_tmp6D3;switch(*((int*)_tmp6B1)){case 1U: _LL1: _LL2:
# 4062
 goto _LL4;case 2U: _LL3: _LL4: goto _LL6;case 0U: _LL5: _LL6: goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA: goto _LLC;case 5U: _LLB: _LLC: goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10: goto _LL12;case 14U: _LL11: _LL12: goto _LL14;case 17U: _LL13: _LL14:
# 4066
 if(_tmp6B0 != 0)({struct Cyc_String_pa_PrintArg_struct _tmp6B4=({struct Cyc_String_pa_PrintArg_struct _tmp9DA;_tmp9DA.tag=0U,({
struct _dyneither_ptr _tmpBBC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->tag=0U,_tmp6B5->f1=c,_tmp6B5->f2=_tmp6B0;_tmp6B5;})));_tmp9DA.f1=_tmpBBC;});_tmp9DA;});void*_tmp6B2[1U];_tmp6B2[0]=& _tmp6B4;({struct _dyneither_ptr _tmpBBD=({const char*_tmp6B3="%s applied to argument(s)";_tag_dyneither(_tmp6B3,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBBD,_tag_dyneither(_tmp6B2,sizeof(void*),1U));});});
goto _LL0;case 9U: _LL15: _LL16:
# 4070
 for(0;_tmp6B0 != 0;_tmp6B0=_tmp6B0->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp6B0->hd,1,1);}
goto _LL0;case 4U: _LL17: _LL18:
# 4074
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B0)!= 1)({void*_tmp6B6=0U;({struct _dyneither_ptr _tmpBBE=({const char*_tmp6B7="tag_t applied to wrong number of arguments";_tag_dyneither(_tmp6B7,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBBE,_tag_dyneither(_tmp6B6,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B0))->hd,0,1);goto _LL0;case 15U: _LL19: _tmp6D4=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B1)->f1;_tmp6D3=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B1)->f2;_LL1A:
# 4077
 if(_tmp6B0 != 0)({void*_tmp6B8=0U;({struct _dyneither_ptr _tmpBBF=({const char*_tmp6B9="enum applied to argument(s)";_tag_dyneither(_tmp6B9,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBBF,_tag_dyneither(_tmp6B8,sizeof(void*),0U));});});
if(*_tmp6D3 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6D3))->fields == 0){
struct _handler_cons _tmp6BA;_push_handler(& _tmp6BA);{int _tmp6BC=0;if(setjmp(_tmp6BA.handler))_tmp6BC=1;if(!_tmp6BC){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D4);
*_tmp6D3=*ed;}
# 4080
;_pop_handler();}else{void*_tmp6BB=(void*)_exn_thrown;void*_tmp6BD=_tmp6BB;void*_tmp6C0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6BD)->tag == Cyc_Dict_Absent){_LL2C: _LL2D: {
# 4084
struct Cyc_Absyn_Enumdecl*_tmp6BE=({struct Cyc_Absyn_Enumdecl*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->sc=Cyc_Absyn_Extern,_tmp6BF->name=_tmp6D4,_tmp6BF->fields=0;_tmp6BF;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6BE);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D4);
*_tmp6D3=*ed;
goto _LL2B;};}}else{_LL2E: _tmp6C0=_tmp6BD;_LL2F:(int)_rethrow(_tmp6C0);}_LL2B:;}};}
# 4090
goto _LL0;case 16U: _LL1B: _tmp6D5=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp6B1)->f1;_LL1C:
# 4092
 if(_tmp6B0 != 0)({void*_tmp6C1=0U;({struct _dyneither_ptr _tmpBC0=({const char*_tmp6C2="enum applied to argument(s)";_tag_dyneither(_tmp6C2,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC0,_tag_dyneither(_tmp6C1,sizeof(void*),0U));});});{
# 4094
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp6D5 != 0;_tmp6D5=_tmp6D5->tl){
struct Cyc_Absyn_Enumfield*_tmp6C3=(struct Cyc_Absyn_Enumfield*)_tmp6D5->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6C3->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6C6=({struct Cyc_String_pa_PrintArg_struct _tmp9DB;_tmp9DB.tag=0U,_tmp9DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C3->name).f2);_tmp9DB;});void*_tmp6C4[1U];_tmp6C4[0]=& _tmp6C6;({unsigned int _tmpBC2=_tmp6C3->loc;struct _dyneither_ptr _tmpBC1=({const char*_tmp6C5="duplicate enum field name %s";_tag_dyneither(_tmp6C5,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpBC2,_tmpBC1,_tag_dyneither(_tmp6C4,sizeof(void*),1U));});});else{
# 4101
prev_fields=({struct Cyc_List_List*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->hd=(*_tmp6C3->name).f2,_tmp6C7->tl=prev_fields;_tmp6C7;});}
if(_tmp6C3->tag == 0)
({struct Cyc_Absyn_Exp*_tmpBC3=Cyc_Absyn_uint_exp(tag_count,_tmp6C3->loc);_tmp6C3->tag=_tmpBC3;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C3->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6CA=({struct Cyc_String_pa_PrintArg_struct _tmp9DC;_tmp9DC.tag=0U,_tmp9DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C3->name).f2);_tmp9DC;});void*_tmp6C8[1U];_tmp6C8[0]=& _tmp6CA;({unsigned int _tmpBC5=loc;struct _dyneither_ptr _tmpBC4=({const char*_tmp6C9="enum field %s: expression is not constant";_tag_dyneither(_tmp6C9,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpBC5,_tmpBC4,_tag_dyneither(_tmp6C8,sizeof(void*),1U));});});}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C3->tag))).f1;
tag_count=t1 + (unsigned int)1;};}
# 4109
goto _LL0;};case 10U: _LL1D: _LL1E:
# 4111
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B0)!= 1)({void*_tmp6CB=0U;({struct _dyneither_ptr _tmpBC6=({const char*_tmp6CC="regions has wrong number of arguments";_tag_dyneither(_tmp6CC,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC6,_tag_dyneither(_tmp6CB,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(_tmp6B0))->hd,1,1);goto _LL0;case 3U: _LL1F: _LL20:
# 4114
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B0)!= 1)({void*_tmp6CD=0U;({struct _dyneither_ptr _tmpBC7=({const char*_tmp6CE="region_t has wrong number of arguments";_tag_dyneither(_tmp6CE,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC7,_tag_dyneither(_tmp6CD,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B0))->hd,1,1);
goto _LL0;case 13U: _LL21: _LL22:
# 4118
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B0)!= 1)({void*_tmp6CF=0U;({struct _dyneither_ptr _tmpBC8=({const char*_tmp6D0="@thin has wrong number of arguments";_tag_dyneither(_tmp6D0,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC8,_tag_dyneither(_tmp6CF,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B0))->hd,0,1);
goto _LL0;case 8U: _LL23: _LL24:
# 4122
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B0)!= 1)({void*_tmp6D1=0U;({struct _dyneither_ptr _tmpBC9=({const char*_tmp6D2="access has wrong number of arguments";_tag_dyneither(_tmp6D2,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBC9,_tag_dyneither(_tmp6D1,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B0))->hd,1,1);goto _LL0;case 20U: _LL25: _tmp6D6=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp6B1)->f1;_LL26:
# 4125
 cvtenv=Cyc_Tcutil_i_check_valid_aggr(loc,te,cvtenv,expected_kind,_tmp6D6,targsp,allow_abs_aggr);
# 4127
goto _LL0;case 18U: _LL27: _tmp6D7=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp6B1)->f1;_LL28:
# 4129
 cvtenv=Cyc_Tcutil_i_check_valid_datatype(loc,te,cvtenv,expected_kind,_tmp6D7,targsp,allow_abs_aggr);
# 4131
goto _LL0;default: _LL29: _tmp6D8=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp6B1)->f1;_LL2A:
# 4133
 cvtenv=Cyc_Tcutil_i_check_valid_datatype_field(loc,te,cvtenv,expected_kind,_tmp6D8,_tmp6B0,allow_abs_aggr);
# 4135
goto _LL0;}_LL0:;}
# 4137
return cvtenv;}struct _tuple32{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 4141
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 4149
{void*_tmp6D9=Cyc_Tcutil_compress(t);void*_tmp6DA=_tmp6D9;struct _tuple2*_tmp7DE;struct Cyc_List_List**_tmp7DD;struct Cyc_Absyn_Typedefdecl**_tmp7DC;void**_tmp7DB;enum Cyc_Absyn_AggrKind _tmp7DA;struct Cyc_List_List*_tmp7D9;struct Cyc_List_List*_tmp7D8;struct Cyc_List_List**_tmp7D7;void**_tmp7D6;struct Cyc_Absyn_Tqual*_tmp7D5;void*_tmp7D4;struct Cyc_List_List*_tmp7D3;int _tmp7D2;struct Cyc_Absyn_VarargInfo*_tmp7D1;struct Cyc_List_List*_tmp7D0;struct Cyc_List_List*_tmp7CF;struct Cyc_Absyn_Exp*_tmp7CE;struct Cyc_List_List**_tmp7CD;struct Cyc_Absyn_Exp*_tmp7CC;struct Cyc_List_List**_tmp7CB;void*_tmp7CA;struct Cyc_Absyn_Tqual*_tmp7C9;struct Cyc_Absyn_Exp**_tmp7C8;void*_tmp7C7;unsigned int _tmp7C6;struct Cyc_Absyn_Exp*_tmp7C5;struct Cyc_Absyn_Exp*_tmp7C4;void*_tmp7C3;struct Cyc_Absyn_Tqual*_tmp7C2;void*_tmp7C1;void*_tmp7C0;void*_tmp7BF;void*_tmp7BE;void*_tmp7BD;void***_tmp7BC;struct Cyc_Absyn_Tvar*_tmp7BB;struct Cyc_Core_Opt**_tmp7BA;void**_tmp7B9;void*_tmp7B8;struct Cyc_List_List**_tmp7B7;switch(*((int*)_tmp6DA)){case 0U: _LL1: _tmp7B8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DA)->f1;_tmp7B7=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DA)->f2;_LL2:
# 4151
 cvtenv=Cyc_Tcutil_i_check_valid_type_app(loc,te,cvtenv,expected_kind,_tmp7B8,_tmp7B7,put_in_effect,allow_abs_aggr);
# 4153
goto _LL0;case 1U: _LL3: _tmp7BA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DA)->f1;_tmp7B9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DA)->f2;_LL4:
# 4156
 if(*_tmp7BA == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BA))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BA))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpBCA=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp7BA=_tmpBCA;});
if(((cvtenv.fn_result  && cvtenv.generalize_evars) && (int)expected_kind->kind == (int)Cyc_Absyn_RgnKind) && !te->tempest_generalize){
# 4161
if((int)expected_kind->aliasqual == (int)Cyc_Absyn_Unique)
*_tmp7B9=Cyc_Absyn_unique_rgn_type;else{
# 4164
*_tmp7B9=Cyc_Absyn_heap_rgn_type;}}else{
if((cvtenv.generalize_evars  && (int)expected_kind->kind != (int)Cyc_Absyn_BoolKind) && (int)expected_kind->kind != (int)Cyc_Absyn_PtrBndKind){
# 4168
struct Cyc_Absyn_Tvar*_tmp6DB=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC->tag=2U,_tmp6DC->f1=0,_tmp6DC->f2=expected_kind;_tmp6DC;}));
({void*_tmpBCB=Cyc_Absyn_var_type(_tmp6DB);*_tmp7B9=_tmpBCB;});
_tmp7BB=_tmp6DB;goto _LL6;}else{
# 4172
({struct Cyc_List_List*_tmpBCC=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpBCC;});}}
# 4174
goto _LL0;case 2U: _LL5: _tmp7BB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6DA)->f1;_LL6:
# 4176
{void*_tmp6DD=Cyc_Absyn_compress_kb(_tmp7BB->kind);void*_tmp6DE=_tmp6DD;struct Cyc_Core_Opt**_tmp6E1;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6DE)->tag == 1U){_LL1A: _tmp6E1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6DE)->f1;_LL1B:
# 4178
({struct Cyc_Core_Opt*_tmpBCE=({struct Cyc_Core_Opt*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));({void*_tmpBCD=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->tag=2U,_tmp6DF->f1=0,_tmp6DF->f2=expected_kind;_tmp6DF;});_tmp6E0->v=_tmpBCD;});_tmp6E0;});*_tmp6E1=_tmpBCE;});goto _LL19;}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 4183
({struct Cyc_List_List*_tmpBCF=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7BB);cvtenv.kind_env=_tmpBCF;});
# 4186
({struct Cyc_List_List*_tmpBD0=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7BB,put_in_effect);cvtenv.free_vars=_tmpBD0;});
# 4188
{void*_tmp6E2=Cyc_Absyn_compress_kb(_tmp7BB->kind);void*_tmp6E3=_tmp6E2;struct Cyc_Core_Opt**_tmp6E7;struct Cyc_Absyn_Kind*_tmp6E6;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E3)->tag == 2U){_LL1F: _tmp6E7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E3)->f1;_tmp6E6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E3)->f2;_LL20:
# 4190
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6E6))
({struct Cyc_Core_Opt*_tmpBD2=({struct Cyc_Core_Opt*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));({void*_tmpBD1=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->tag=2U,_tmp6E4->f1=0,_tmp6E4->f2=expected_kind;_tmp6E4;});_tmp6E5->v=_tmpBD1;});_tmp6E5;});*_tmp6E7=_tmpBD2;});
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 4195
goto _LL0;case 10U: _LL7: _tmp7BD=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DA)->f1)->r;_tmp7BC=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DA)->f2;_LL8: {
# 4201
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6E8=_tmp7BD;struct Cyc_Absyn_Datatypedecl*_tmp6EB;struct Cyc_Absyn_Enumdecl*_tmp6EA;struct Cyc_Absyn_Aggrdecl*_tmp6E9;switch(*((int*)_tmp6E8)){case 0U: _LL24: _tmp6E9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6E8)->f1;_LL25:
# 4204
 if(te->in_extern_c_include)
_tmp6E9->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6E9);goto _LL23;case 1U: _LL26: _tmp6EA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6E8)->f1;_LL27:
# 4208
 if(te->in_extern_c_include)
_tmp6EA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6EA);goto _LL23;default: _LL28: _tmp6EB=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6E8)->f1;_LL29:
# 4212
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6EB);goto _LL23;}_LL23:;}
# 4214
({void**_tmpBD3=({void**_tmp6EC=_cycalloc(sizeof(*_tmp6EC));*_tmp6EC=new_t;_tmp6EC;});*_tmp7BC=_tmpBD3;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 3U: _LL9: _tmp7C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DA)->f1).elt_type;_tmp7C2=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DA)->f1).elt_tq;_tmp7C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DA)->f1).ptr_atts).rgn;_tmp7C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DA)->f1).ptr_atts).nullable;_tmp7BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DA)->f1).ptr_atts).bounds;_tmp7BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DA)->f1).ptr_atts).zero_term;_LLA: {
# 4220
int is_zero_terminated;
# 4222
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7C3,1,1);
({int _tmpBD4=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7C2->print_const,_tmp7C3);_tmp7C2->real_const=_tmpBD4;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp6ED=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp6EE=_tmp6ED;switch(_tmp6EE){case Cyc_Absyn_Aliasable: _LL2B: _LL2C:
 k=& Cyc_Tcutil_rk;goto _LL2A;case Cyc_Absyn_Unique: _LL2D: _LL2E:
 k=& Cyc_Tcutil_urk;goto _LL2A;case Cyc_Absyn_Top: _LL2F: _LL30:
 goto _LL32;default: _LL31: _LL32:
 k=& Cyc_Tcutil_trk;goto _LL2A;}_LL2A:;}
# 4231
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7C1,1,1);
# 4234
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7BE,0,1);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7C0,0,1);
({void*_tmpBD5=_tmp7BE;Cyc_Tcutil_unify(_tmpBD5,Cyc_Tcutil_is_char_type(_tmp7C3)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
is_zero_terminated=Cyc_Absyn_type2bool(0,_tmp7BE);
if(is_zero_terminated){
# 4240
if(!Cyc_Tcutil_admits_zero(_tmp7C3))
({struct Cyc_String_pa_PrintArg_struct _tmp6F1=({struct Cyc_String_pa_PrintArg_struct _tmp9DD;_tmp9DD.tag=0U,({
struct _dyneither_ptr _tmpBD6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7C3));_tmp9DD.f1=_tmpBD6;});_tmp9DD;});void*_tmp6EF[1U];_tmp6EF[0]=& _tmp6F1;({unsigned int _tmpBD8=loc;struct _dyneither_ptr _tmpBD7=({const char*_tmp6F0="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp6F0,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpBD8,_tmpBD7,_tag_dyneither(_tmp6EF,sizeof(void*),1U));});});}
# 4245
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ptrbk,_tmp7BF,0,allow_abs_aggr);{
struct Cyc_Absyn_Exp*_tmp6F2=({void*_tmpBD9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBD9,_tmp7BF);});
if(_tmp6F2 != 0){
struct _tuple13 _tmp6F3=Cyc_Evexp_eval_const_uint_exp(_tmp6F2);struct _tuple13 _tmp6F4=_tmp6F3;unsigned int _tmp6F8;int _tmp6F7;_LL34: _tmp6F8=_tmp6F4.f1;_tmp6F7=_tmp6F4.f2;_LL35:;
if(is_zero_terminated  && (!_tmp6F7  || _tmp6F8 < (unsigned int)1))
({void*_tmp6F5=0U;({unsigned int _tmpBDB=loc;struct _dyneither_ptr _tmpBDA=({const char*_tmp6F6="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp6F6,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpBDB,_tmpBDA,_tag_dyneither(_tmp6F5,sizeof(void*),0U));});});}
# 4252
goto _LL0;};};}case 9U: _LLB: _tmp7C4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6DA)->f1;_LLC:
# 4257
({struct Cyc_Tcenv_Tenv*_tmpBDC=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBDC,0,_tmp7C4);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp7C4))
({void*_tmp6F9=0U;({unsigned int _tmpBDE=loc;struct _dyneither_ptr _tmpBDD=({const char*_tmp6FA="valueof_t requires an int expression";_tag_dyneither(_tmp6FA,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpBDE,_tmpBDD,_tag_dyneither(_tmp6F9,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7C4,te,cvtenv);
goto _LL0;case 11U: _LLD: _tmp7C5=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6DA)->f1;_LLE:
# 4266
({struct Cyc_Tcenv_Tenv*_tmpBDF=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBDF,0,_tmp7C5);});
goto _LL0;case 4U: _LLF: _tmp7CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DA)->f1).elt_type;_tmp7C9=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DA)->f1).tq;_tmp7C8=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DA)->f1).num_elts;_tmp7C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DA)->f1).zero_term;_tmp7C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DA)->f1).zt_loc;_LL10: {
# 4271
struct Cyc_Absyn_Exp*_tmp6FB=*_tmp7C8;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7CA,1,allow_abs_aggr);
({int _tmpBE0=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7C9->print_const,_tmp7CA);_tmp7C9->real_const=_tmpBE0;});{
# 4275
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,_tmp7C7);
if(is_zero_terminated){
# 4278
if(!Cyc_Tcutil_admits_zero(_tmp7CA))
({struct Cyc_String_pa_PrintArg_struct _tmp6FE=({struct Cyc_String_pa_PrintArg_struct _tmp9DE;_tmp9DE.tag=0U,({
struct _dyneither_ptr _tmpBE1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7CA));_tmp9DE.f1=_tmpBE1;});_tmp9DE;});void*_tmp6FC[1U];_tmp6FC[0]=& _tmp6FE;({unsigned int _tmpBE3=loc;struct _dyneither_ptr _tmpBE2=({const char*_tmp6FD="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp6FD,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpBE3,_tmpBE2,_tag_dyneither(_tmp6FC,sizeof(void*),1U));});});}
# 4284
if(_tmp6FB == 0){
# 4286
if(is_zero_terminated)
# 4288
({struct Cyc_Absyn_Exp*_tmpBE4=_tmp6FB=Cyc_Absyn_uint_exp(1U,0U);*_tmp7C8=_tmpBE4;});else{
# 4291
({void*_tmp6FF=0U;({unsigned int _tmpBE6=loc;struct _dyneither_ptr _tmpBE5=({const char*_tmp700="array bound defaults to 1 here";_tag_dyneither(_tmp700,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpBE6,_tmpBE5,_tag_dyneither(_tmp6FF,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpBE7=_tmp6FB=Cyc_Absyn_uint_exp(1U,0U);*_tmp7C8=_tmpBE7;});}}
# 4295
({struct Cyc_Tcenv_Tenv*_tmpBE8=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpBE8,0,_tmp6FB);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp6FB))
({void*_tmp701=0U;({unsigned int _tmpBEA=loc;struct _dyneither_ptr _tmpBE9=({const char*_tmp702="array bounds expression is not an unsigned int";_tag_dyneither(_tmp702,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBEA,_tmpBE9,_tag_dyneither(_tmp701,sizeof(void*),0U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp6FB,te,cvtenv);{
# 4303
struct _tuple13 _tmp703=Cyc_Evexp_eval_const_uint_exp(_tmp6FB);struct _tuple13 _tmp704=_tmp703;unsigned int _tmp70A;int _tmp709;_LL37: _tmp70A=_tmp704.f1;_tmp709=_tmp704.f2;_LL38:;
# 4305
if((is_zero_terminated  && _tmp709) && _tmp70A < (unsigned int)1)
({void*_tmp705=0U;({unsigned int _tmpBEC=loc;struct _dyneither_ptr _tmpBEB=({const char*_tmp706="zero terminated array cannot have zero elements";_tag_dyneither(_tmp706,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpBEC,_tmpBEB,_tag_dyneither(_tmp705,sizeof(void*),0U));});});
# 4308
if((_tmp709  && _tmp70A < (unsigned int)1) && Cyc_Cyclone_tovc_r){
({void*_tmp707=0U;({unsigned int _tmpBEE=loc;struct _dyneither_ptr _tmpBED=({const char*_tmp708="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp708,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpBEE,_tmpBED,_tag_dyneither(_tmp707,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpBEF=Cyc_Absyn_uint_exp(1U,0U);*_tmp7C8=_tmpBEF;});}
# 4312
goto _LL0;};};}case 5U: _LL11: _tmp7D7=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).tvars;_tmp7D6=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).effect;_tmp7D5=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).ret_tqual;_tmp7D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).ret_type;_tmp7D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).args;_tmp7D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).c_varargs;_tmp7D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).cyc_varargs;_tmp7D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).rgn_po;_tmp7CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).attributes;_tmp7CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).requires_clause;_tmp7CD=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).requires_relns;_tmp7CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).ensures_clause;_tmp7CB=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DA)->f1).ensures_relns;_LL12: {
# 4319
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp7CF != 0;_tmp7CF=_tmp7CF->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7CF->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp70D=({struct Cyc_String_pa_PrintArg_struct _tmp9DF;_tmp9DF.tag=0U,({struct _dyneither_ptr _tmpBF0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7CF->hd));_tmp9DF.f1=_tmpBF0;});_tmp9DF;});void*_tmp70B[1U];_tmp70B[0]=& _tmp70D;({unsigned int _tmpBF2=loc;struct _dyneither_ptr _tmpBF1=({const char*_tmp70C="bad function type attribute %s";_tag_dyneither(_tmp70C,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpBF2,_tmpBF1,_tag_dyneither(_tmp70B,sizeof(void*),1U));});});{
void*_tmp70E=(void*)_tmp7CF->hd;void*_tmp70F=_tmp70E;enum Cyc_Absyn_Format_Type _tmp714;int _tmp713;int _tmp712;switch(*((int*)_tmp70F)){case 1U: _LL3A: _LL3B:
# 4332
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL39;case 2U: _LL3C: _LL3D:
# 4334
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL39;case 3U: _LL3E: _LL3F:
# 4336
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL39;case 19U: _LL40: _tmp714=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp70F)->f1;_tmp713=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp70F)->f2;_tmp712=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp70F)->f3;_LL41:
# 4338
 if(!seen_format){
seen_format=1;
ft=_tmp714;
fmt_desc_arg=_tmp713;
fmt_arg_start=_tmp712;}else{
# 4344
({void*_tmp710=0U;({unsigned int _tmpBF4=loc;struct _dyneither_ptr _tmpBF3=({const char*_tmp711="function can't have multiple format attributes";_tag_dyneither(_tmp711,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpBF4,_tmpBF3,_tag_dyneither(_tmp710,sizeof(void*),0U));});});}
goto _LL39;default: _LL42: _LL43:
 goto _LL39;}_LL39:;};}
# 4349
if(num_convs > 1)
({void*_tmp715=0U;({unsigned int _tmpBF6=loc;struct _dyneither_ptr _tmpBF5=({const char*_tmp716="function can't have multiple calling conventions";_tag_dyneither(_tmp716,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpBF6,_tmpBF5,_tag_dyneither(_tmp715,sizeof(void*),0U));});});
# 4354
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7D7);
{struct Cyc_List_List*b=*_tmp7D7;for(0;b != 0;b=b->tl){
({int _tmpBF7=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpBF7;});
({struct Cyc_List_List*_tmpBF8=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpBF8;});{
void*_tmp717=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp718=_tmp717;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp718)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp718)->f1)->kind == Cyc_Absyn_MemKind){_LL45: _LL46:
# 4360
({struct Cyc_String_pa_PrintArg_struct _tmp71B=({struct Cyc_String_pa_PrintArg_struct _tmp9E0;_tmp9E0.tag=0U,_tmp9E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmp9E0;});void*_tmp719[1U];_tmp719[0]=& _tmp71B;({unsigned int _tmpBFA=loc;struct _dyneither_ptr _tmpBF9=({const char*_tmp71A="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp71A,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpBFA,_tmpBF9,_tag_dyneither(_tmp719,sizeof(void*),1U));});});
goto _LL44;}else{goto _LL47;}}else{_LL47: _LL48:
 goto _LL44;}_LL44:;};}}{
# 4368
struct Cyc_Tcutil_CVTEnv _tmp71C=({struct Cyc_Tcutil_CVTEnv _tmp9E3;_tmp9E3.r=Cyc_Core_heap_region,_tmp9E3.kind_env=cvtenv.kind_env,_tmp9E3.free_vars=0,_tmp9E3.free_evars=0,_tmp9E3.generalize_evars=cvtenv.generalize_evars,_tmp9E3.fn_result=1;_tmp9E3;});
# 4370
_tmp71C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71C,& Cyc_Tcutil_tmk,_tmp7D4,1,1);
({int _tmpBFB=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7D5->print_const,_tmp7D4);_tmp7D5->real_const=_tmpBFB;});
_tmp71C.fn_result=0;
# 4376
{struct Cyc_List_List*a=_tmp7D3;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp71D=(struct _tuple10*)a->hd;
void*_tmp71E=(*_tmp71D).f3;
_tmp71C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71C,& Cyc_Tcutil_tmk,_tmp71E,1,1);{
int _tmp71F=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp71D).f2).print_const,_tmp71E);
((*_tmp71D).f2).real_const=_tmp71F;
# 4384
if(Cyc_Tcutil_is_array_type(_tmp71E)){
# 4386
void*_tmp720=Cyc_Absyn_new_evar(0,0);
_tmp71C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71C,& Cyc_Tcutil_rk,_tmp720,1,1);
({void*_tmpBFC=Cyc_Tcutil_promote_array(_tmp71E,_tmp720,0);(*_tmp71D).f3=_tmpBFC;});}};}}
# 4393
if(_tmp7D1 != 0){
if(_tmp7D2)({void*_tmp721=0U;({struct _dyneither_ptr _tmpBFD=({const char*_tmp722="both c_vararg and cyc_vararg";_tag_dyneither(_tmp722,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBFD,_tag_dyneither(_tmp721,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp723=*_tmp7D1;struct Cyc_Absyn_VarargInfo _tmp724=_tmp723;void*_tmp735;int _tmp734;_LL4A: _tmp735=_tmp724.type;_tmp734=_tmp724.inject;_LL4B:;
_tmp71C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71C,& Cyc_Tcutil_tmk,_tmp735,1,1);
({int _tmpBFE=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp7D1->tq).print_const,_tmp735);(_tmp7D1->tq).real_const=_tmpBFE;});
# 4399
if(_tmp734){
void*_tmp725=Cyc_Tcutil_compress(_tmp735);void*_tmp726=_tmp725;void*_tmp733;void*_tmp732;void*_tmp731;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp726)->tag == 3U){_LL4D: _tmp733=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp726)->f1).elt_type;_tmp732=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp726)->f1).ptr_atts).bounds;_tmp731=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp726)->f1).ptr_atts).zero_term;_LL4E:
# 4402
{void*_tmp727=Cyc_Tcutil_compress(_tmp733);void*_tmp728=_tmp727;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp728)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp728)->f1)->tag == 18U){_LL52: _LL53:
# 4404
 if(Cyc_Tcutil_force_type2bool(0,_tmp731))
({void*_tmp729=0U;({unsigned int _tmpC00=loc;struct _dyneither_ptr _tmpBFF=({const char*_tmp72A="can't inject into a zeroterm pointer";_tag_dyneither(_tmp72A,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpC00,_tmpBFF,_tag_dyneither(_tmp729,sizeof(void*),0U));});});
if(!({void*_tmpC01=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpC01,_tmp732);}))
({void*_tmp72B=0U;({unsigned int _tmpC03=loc;struct _dyneither_ptr _tmpC02=({const char*_tmp72C="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp72C,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpC03,_tmpC02,_tag_dyneither(_tmp72B,sizeof(void*),0U));});});
goto _LL51;}else{goto _LL54;}}else{_LL54: _LL55:
({void*_tmp72D=0U;({unsigned int _tmpC05=loc;struct _dyneither_ptr _tmpC04=({const char*_tmp72E="can't inject a non-datatype type";_tag_dyneither(_tmp72E,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpC05,_tmpC04,_tag_dyneither(_tmp72D,sizeof(void*),0U));});});goto _LL51;}_LL51:;}
# 4411
goto _LL4C;}else{_LL4F: _LL50:
({void*_tmp72F=0U;({unsigned int _tmpC07=loc;struct _dyneither_ptr _tmpC06=({const char*_tmp730="expecting a datatype pointer type";_tag_dyneither(_tmp730,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpC07,_tmpC06,_tag_dyneither(_tmp72F,sizeof(void*),0U));});});goto _LL4C;}_LL4C:;}};}
# 4417
if(seen_format){
int _tmp736=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7D3);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp736) || fmt_arg_start < 0) || 
# 4421
(_tmp7D1 == 0  && !_tmp7D2) && fmt_arg_start != 0) || 
(_tmp7D1 != 0  || _tmp7D2) && fmt_arg_start != _tmp736 + 1)
# 4424
({void*_tmp737=0U;({unsigned int _tmpC09=loc;struct _dyneither_ptr _tmpC08=({const char*_tmp738="bad format descriptor";_tag_dyneither(_tmp738,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpC09,_tmpC08,_tag_dyneither(_tmp737,sizeof(void*),0U));});});else{
# 4427
struct _tuple10 _tmp739=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7D3,fmt_desc_arg - 1);struct _tuple10 _tmp73A=_tmp739;void*_tmp74F;_LL57: _tmp74F=_tmp73A.f3;_LL58:;
# 4429
{void*_tmp73B=Cyc_Tcutil_compress(_tmp74F);void*_tmp73C=_tmp73B;void*_tmp748;void*_tmp747;void*_tmp746;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73C)->tag == 3U){_LL5A: _tmp748=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73C)->f1).elt_type;_tmp747=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73C)->f1).ptr_atts).bounds;_tmp746=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73C)->f1).ptr_atts).zero_term;_LL5B:
# 4432
 if(!Cyc_Tcutil_is_char_type(_tmp748))
({void*_tmp73D=0U;({unsigned int _tmpC0B=loc;struct _dyneither_ptr _tmpC0A=({const char*_tmp73E="format descriptor is not a string";_tag_dyneither(_tmp73E,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpC0B,_tmpC0A,_tag_dyneither(_tmp73D,sizeof(void*),0U));});});else{
# 4435
struct Cyc_Absyn_Exp*_tmp73F=({void*_tmpC0C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpC0C,_tmp747);});
if(_tmp73F == 0  && _tmp7D2)
({void*_tmp740=0U;({unsigned int _tmpC0E=loc;struct _dyneither_ptr _tmpC0D=({const char*_tmp741="format descriptor is not a char * type";_tag_dyneither(_tmp741,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpC0E,_tmpC0D,_tag_dyneither(_tmp740,sizeof(void*),0U));});});else{
if(_tmp73F != 0  && !_tmp7D2)
({void*_tmp742=0U;({unsigned int _tmpC10=loc;struct _dyneither_ptr _tmpC0F=({const char*_tmp743="format descriptor is not a char ? type";_tag_dyneither(_tmp743,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpC10,_tmpC0F,_tag_dyneither(_tmp742,sizeof(void*),0U));});});}}
# 4441
goto _LL59;}else{_LL5C: _LL5D:
({void*_tmp744=0U;({unsigned int _tmpC12=loc;struct _dyneither_ptr _tmpC11=({const char*_tmp745="format descriptor is not a string type";_tag_dyneither(_tmp745,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpC12,_tmpC11,_tag_dyneither(_tmp744,sizeof(void*),0U));});});goto _LL59;}_LL59:;}
# 4444
if(fmt_arg_start != 0  && !_tmp7D2){
# 4448
int problem;
{struct _tuple32 _tmp749=({struct _tuple32 _tmp9E1;_tmp9E1.f1=ft,({void*_tmpC13=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7D1))->type));_tmp9E1.f2=_tmpC13;});_tmp9E1;});struct _tuple32 _tmp74A=_tmp749;struct Cyc_Absyn_Datatypedecl*_tmp74C;struct Cyc_Absyn_Datatypedecl*_tmp74B;switch(_tmp74A.f1){case Cyc_Absyn_Printf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5F: _tmp74B=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->f1)->f1).KnownDatatype).val;_LL60:
# 4451
 problem=Cyc_Absyn_qvar_cmp(_tmp74B->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}case Cyc_Absyn_Scanf_ft: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->f1)->f1).KnownDatatype).tag == 2){_LL61: _tmp74C=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74A.f2)->f1)->f1).KnownDatatype).val;_LL62:
# 4453
 problem=Cyc_Absyn_qvar_cmp(_tmp74C->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL5E;}else{goto _LL63;}}else{goto _LL63;}}else{goto _LL63;}default: _LL63: _LL64:
# 4455
 problem=1;goto _LL5E;}_LL5E:;}
# 4457
if(problem)
({void*_tmp74D=0U;({unsigned int _tmpC15=loc;struct _dyneither_ptr _tmpC14=({const char*_tmp74E="format attribute and vararg types don't match";_tag_dyneither(_tmp74E,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpC15,_tmpC14,_tag_dyneither(_tmp74D,sizeof(void*),0U));});});}}}
# 4465
{struct Cyc_List_List*rpo=_tmp7D0;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp750=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp751=_tmp750;void*_tmp753;void*_tmp752;_LL66: _tmp753=_tmp751->f1;_tmp752=_tmp751->f2;_LL67:;
_tmp71C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71C,& Cyc_Tcutil_ek,_tmp753,1,1);
_tmp71C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71C,& Cyc_Tcutil_trk,_tmp752,1,1);}}{
# 4473
struct Cyc_Tcenv_Fenv*_tmp754=Cyc_Tcenv_bogus_fenv(_tmp7D4,_tmp7D3);
struct Cyc_Tcenv_Tenv*_tmp755=({struct Cyc_Tcenv_Tenv*_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793->ns=te->ns,_tmp793->ae=te->ae,_tmp793->le=_tmp754,_tmp793->allow_valueof=1,_tmp793->in_extern_c_include=te->in_extern_c_include,_tmp793->in_tempest=te->in_tempest,_tmp793->tempest_generalize=te->tempest_generalize;_tmp793;});
struct _tuple31 _tmp756=({unsigned int _tmpC19=loc;struct Cyc_Tcenv_Tenv*_tmpC18=_tmp755;struct Cyc_Tcutil_CVTEnv _tmpC17=_tmp71C;struct _dyneither_ptr _tmpC16=({const char*_tmp792="@requires";_tag_dyneither(_tmp792,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpC19,_tmpC18,_tmpC17,_tmpC16,_tmp7CE);});struct _tuple31 _tmp757=_tmp756;struct Cyc_Tcutil_CVTEnv _tmp791;struct Cyc_List_List*_tmp790;_LL69: _tmp791=_tmp757.f1;_tmp790=_tmp757.f2;_LL6A:;
_tmp71C=_tmp791;
*_tmp7CD=_tmp790;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D3,_tmp790);{
# 4486
struct _tuple31 _tmp758=({unsigned int _tmpC1D=loc;struct Cyc_Tcenv_Tenv*_tmpC1C=_tmp755;struct Cyc_Tcutil_CVTEnv _tmpC1B=_tmp71C;struct _dyneither_ptr _tmpC1A=({const char*_tmp78F="@ensures";_tag_dyneither(_tmp78F,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpC1D,_tmpC1C,_tmpC1B,_tmpC1A,_tmp7CC);});struct _tuple31 _tmp759=_tmp758;struct Cyc_Tcutil_CVTEnv _tmp78E;struct Cyc_List_List*_tmp78D;_LL6C: _tmp78E=_tmp759.f1;_tmp78D=_tmp759.f2;_LL6D:;
_tmp71C=_tmp78E;
*_tmp7CB=_tmp78D;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D3,_tmp78D);
# 4491
if(*_tmp7D6 != 0)
_tmp71C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71C,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp7D6),1,1);else{
# 4494
struct Cyc_List_List*effect=0;
# 4499
{struct Cyc_List_List*tvs=_tmp71C.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp75A=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp75B=_tmp75A;struct Cyc_Absyn_Tvar*_tmp769;int _tmp768;_LL6F: _tmp769=_tmp75B.f1;_tmp768=_tmp75B.f2;_LL70:;
if(!_tmp768)continue;{
void*_tmp75C=Cyc_Absyn_compress_kb(_tmp769->kind);void*_tmp75D=_tmp75C;struct Cyc_Core_Opt**_tmp767;struct Cyc_Absyn_Kind*_tmp766;struct Cyc_Core_Opt**_tmp765;struct Cyc_Core_Opt**_tmp764;struct Cyc_Absyn_Kind*_tmp763;switch(*((int*)_tmp75D)){case 2U: _LL72: _tmp764=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75D)->f1;_tmp763=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75D)->f2;if((int)_tmp763->kind == (int)Cyc_Absyn_RgnKind){_LL73:
# 4505
 if((int)_tmp763->aliasqual == (int)Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpC1E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp764=_tmpC1E;});_tmp766=_tmp763;goto _LL75;}
# 4508
({struct Cyc_Core_Opt*_tmpC1F=Cyc_Tcutil_kind_to_bound_opt(_tmp763);*_tmp764=_tmpC1F;});_tmp766=_tmp763;goto _LL75;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75D)->f2)->kind){case Cyc_Absyn_BoolKind: _LL76: _LL77:
# 4511
 goto _LL79;case Cyc_Absyn_PtrBndKind: _LL78: _LL79:
 goto _LL7B;case Cyc_Absyn_IntKind: _LL7A: _LL7B:
 goto _LL7D;case Cyc_Absyn_EffKind: _LL82: _tmp765=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75D)->f1;_LL83:
# 4518
({struct Cyc_Core_Opt*_tmpC20=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp765=_tmpC20;});goto _LL85;default: goto _LL88;}}case 0U: _LL74: _tmp766=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75D)->f1;if((int)_tmp766->kind == (int)Cyc_Absyn_RgnKind){_LL75:
# 4510
 effect=({struct Cyc_List_List*_tmp75E=_cycalloc(sizeof(*_tmp75E));({void*_tmpC21=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(_tmp769));_tmp75E->hd=_tmpC21;}),_tmp75E->tl=effect;_tmp75E;});goto _LL71;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75D)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7C: _LL7D:
# 4514
 goto _LL7F;case Cyc_Absyn_PtrBndKind: _LL7E: _LL7F:
 goto _LL81;case Cyc_Absyn_IntKind: _LL80: _LL81:
 goto _LL71;case Cyc_Absyn_EffKind: _LL84: _LL85:
# 4520
 effect=({struct Cyc_List_List*_tmp75F=_cycalloc(sizeof(*_tmp75F));({void*_tmpC22=Cyc_Absyn_var_type(_tmp769);_tmp75F->hd=_tmpC22;}),_tmp75F->tl=effect;_tmp75F;});goto _LL71;default: _LL88: _LL89:
# 4525
 effect=({struct Cyc_List_List*_tmp762=_cycalloc(sizeof(*_tmp762));({void*_tmpC23=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(_tmp769));_tmp762->hd=_tmpC23;}),_tmp762->tl=effect;_tmp762;});goto _LL71;}}default: _LL86: _tmp767=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp75D)->f1;_LL87:
# 4522
({struct Cyc_Core_Opt*_tmpC25=({struct Cyc_Core_Opt*_tmp761=_cycalloc(sizeof(*_tmp761));({void*_tmpC24=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->tag=2U,_tmp760->f1=0,_tmp760->f2=& Cyc_Tcutil_ak;_tmp760;});_tmp761->v=_tmpC24;});_tmp761;});*_tmp767=_tmpC25;});goto _LL89;}_LL71:;};}}
# 4529
{struct Cyc_List_List*ts=_tmp71C.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple30 _tmp76A=*((struct _tuple30*)ts->hd);struct _tuple30 _tmp76B=_tmp76A;void*_tmp772;int _tmp771;_LL8B: _tmp772=_tmp76B.f1;_tmp771=_tmp76B.f2;_LL8C:;
if(!_tmp771)continue;{
struct Cyc_Absyn_Kind*_tmp76C=Cyc_Tcutil_type_kind(_tmp772);struct Cyc_Absyn_Kind*_tmp76D=_tmp76C;switch(((struct Cyc_Absyn_Kind*)_tmp76D)->kind){case Cyc_Absyn_RgnKind: _LL8E: _LL8F:
# 4534
 effect=({struct Cyc_List_List*_tmp76E=_cycalloc(sizeof(*_tmp76E));({void*_tmpC26=Cyc_Absyn_access_eff(_tmp772);_tmp76E->hd=_tmpC26;}),_tmp76E->tl=effect;_tmp76E;});goto _LL8D;case Cyc_Absyn_EffKind: _LL90: _LL91:
# 4536
 effect=({struct Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F->hd=_tmp772,_tmp76F->tl=effect;_tmp76F;});goto _LL8D;case Cyc_Absyn_IntKind: _LL92: _LL93:
 goto _LL8D;default: _LL94: _LL95:
# 4539
 effect=({struct Cyc_List_List*_tmp770=_cycalloc(sizeof(*_tmp770));({void*_tmpC27=Cyc_Absyn_regionsof_eff(_tmp772);_tmp770->hd=_tmpC27;}),_tmp770->tl=effect;_tmp770;});goto _LL8D;}_LL8D:;};}}
# 4542
({void*_tmpC28=Cyc_Absyn_join_eff(effect);*_tmp7D6=_tmpC28;});}
# 4548
if(*_tmp7D7 != 0){
struct Cyc_List_List*bs=*_tmp7D7;for(0;bs != 0;bs=bs->tl){
void*_tmp773=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp774=_tmp773;struct Cyc_Core_Opt**_tmp784;struct Cyc_Absyn_Kind*_tmp783;struct Cyc_Core_Opt**_tmp782;struct Cyc_Core_Opt**_tmp781;struct Cyc_Core_Opt**_tmp780;struct Cyc_Core_Opt**_tmp77F;struct Cyc_Core_Opt**_tmp77E;struct Cyc_Core_Opt**_tmp77D;struct Cyc_Core_Opt**_tmp77C;struct Cyc_Core_Opt**_tmp77B;struct Cyc_Core_Opt**_tmp77A;switch(*((int*)_tmp774)){case 1U: _LL97: _tmp77A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LL98:
# 4552
({struct Cyc_String_pa_PrintArg_struct _tmp777=({struct Cyc_String_pa_PrintArg_struct _tmp9E2;_tmp9E2.tag=0U,_tmp9E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmp9E2;});void*_tmp775[1U];_tmp775[0]=& _tmp777;({unsigned int _tmpC2A=loc;struct _dyneither_ptr _tmpC29=({const char*_tmp776="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp776,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpC2A,_tmpC29,_tag_dyneither(_tmp775,sizeof(void*),1U));});});
# 4554
_tmp77B=_tmp77A;goto _LL9A;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f2)->aliasqual == Cyc_Absyn_Top){_LL99: _tmp77B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LL9A:
 _tmp77C=_tmp77B;goto _LL9C;}else{goto _LLA9;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9B: _tmp77C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LL9C:
 _tmp77D=_tmp77C;goto _LL9E;case Cyc_Absyn_Aliasable: _LL9D: _tmp77D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LL9E:
 _tmp77F=_tmp77D;goto _LLA0;case Cyc_Absyn_Unique: _LLA3: _tmp77E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LLA4:
# 4561
 _tmp781=_tmp77E;goto _LLA6;default: goto _LLA9;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9F: _tmp77F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LLA0:
# 4558
 _tmp780=_tmp77F;goto _LLA2;case Cyc_Absyn_Aliasable: _LLA1: _tmp780=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LLA2:
# 4560
({struct Cyc_Core_Opt*_tmpC2B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp780=_tmpC2B;});goto _LL96;case Cyc_Absyn_Unique: _LLA5: _tmp781=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LLA6:
# 4563
({struct Cyc_Core_Opt*_tmpC2C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp781=_tmpC2C;});goto _LL96;default: goto _LLA9;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f2)->aliasqual == Cyc_Absyn_Top){_LLA7: _tmp782=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_LLA8:
# 4565
({struct Cyc_Core_Opt*_tmpC2D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp782=_tmpC2D;});goto _LL96;}else{goto _LLA9;}default: _LLA9: _tmp784=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_tmp783=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f2;_LLAA:
# 4567
({struct Cyc_Core_Opt*_tmpC2E=Cyc_Tcutil_kind_to_bound_opt(_tmp783);*_tmp784=_tmpC2E;});goto _LL96;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp774)->f1)->kind == Cyc_Absyn_MemKind){_LLAB: _LLAC:
# 4569
({void*_tmp778=0U;({unsigned int _tmpC30=loc;struct _dyneither_ptr _tmpC2F=({const char*_tmp779="functions can't abstract M types";_tag_dyneither(_tmp779,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpC30,_tmpC2F,_tag_dyneither(_tmp778,sizeof(void*),0U));});});goto _LL96;}else{_LLAD: _LLAE:
 goto _LL96;}}_LL96:;}}
# 4574
({struct Cyc_List_List*_tmpC31=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp71C.kind_env,*_tmp7D7);cvtenv.kind_env=_tmpC31;});
({struct Cyc_List_List*_tmpC32=Cyc_Tcutil_remove_bound_tvars_bool(_tmp71C.r,_tmp71C.free_vars,*_tmp7D7);_tmp71C.free_vars=_tmpC32;});
# 4577
{struct Cyc_List_List*tvs=_tmp71C.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple29 _tmp785=*((struct _tuple29*)tvs->hd);struct _tuple29 _tmp786=_tmp785;struct Cyc_Absyn_Tvar*_tmp788;int _tmp787;_LLB0: _tmp788=_tmp786.f1;_tmp787=_tmp786.f2;_LLB1:;
({struct Cyc_List_List*_tmpC33=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp788,_tmp787);cvtenv.free_vars=_tmpC33;});}}
# 4582
{struct Cyc_List_List*evs=_tmp71C.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple30 _tmp789=*((struct _tuple30*)evs->hd);struct _tuple30 _tmp78A=_tmp789;void*_tmp78C;int _tmp78B;_LLB3: _tmp78C=_tmp78A.f1;_tmp78B=_tmp78A.f2;_LLB4:;
({struct Cyc_List_List*_tmpC34=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp78C,_tmp78B);cvtenv.free_evars=_tmpC34;});}}
# 4586
goto _LL0;};};};}case 6U: _LL13: _tmp7D8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6DA)->f1;_LL14:
# 4589
 for(0;_tmp7D8 != 0;_tmp7D8=_tmp7D8->tl){
struct _tuple12*_tmp794=(struct _tuple12*)_tmp7D8->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp794).f2,1,0);
({int _tmpC35=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp794).f1).print_const,(*_tmp794).f2);
# 4592
((*_tmp794).f1).real_const=_tmpC35;});}
# 4595
goto _LL0;case 7U: _LL15: _tmp7DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DA)->f1;_tmp7D9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DA)->f2;_LL16: {
# 4599
struct Cyc_List_List*prev_fields=0;
for(0;_tmp7D9 != 0;_tmp7D9=_tmp7D9->tl){
struct Cyc_Absyn_Aggrfield*_tmp795=(struct Cyc_Absyn_Aggrfield*)_tmp7D9->hd;struct Cyc_Absyn_Aggrfield*_tmp796=_tmp795;struct _dyneither_ptr*_tmp7A6;struct Cyc_Absyn_Tqual*_tmp7A5;void*_tmp7A4;struct Cyc_Absyn_Exp*_tmp7A3;struct Cyc_List_List*_tmp7A2;struct Cyc_Absyn_Exp*_tmp7A1;_LLB6: _tmp7A6=_tmp796->name;_tmp7A5=(struct Cyc_Absyn_Tqual*)& _tmp796->tq;_tmp7A4=_tmp796->type;_tmp7A3=_tmp796->width;_tmp7A2=_tmp796->attributes;_tmp7A1=_tmp796->requires_clause;_LLB7:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7A6))
({struct Cyc_String_pa_PrintArg_struct _tmp799=({struct Cyc_String_pa_PrintArg_struct _tmp9E4;_tmp9E4.tag=0U,_tmp9E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7A6);_tmp9E4;});void*_tmp797[1U];_tmp797[0]=& _tmp799;({unsigned int _tmpC37=loc;struct _dyneither_ptr _tmpC36=({const char*_tmp798="duplicate field %s";_tag_dyneither(_tmp798,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpC37,_tmpC36,_tag_dyneither(_tmp797,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpC38=(struct _dyneither_ptr)*_tmp7A6;Cyc_strcmp(_tmpC38,({const char*_tmp79A="";_tag_dyneither(_tmp79A,sizeof(char),1U);}));})!= 0)
prev_fields=({struct Cyc_List_List*_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B->hd=_tmp7A6,_tmp79B->tl=prev_fields;_tmp79B;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7A4,1,0);
({int _tmpC39=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A5->print_const,_tmp7A4);_tmp7A5->real_const=_tmpC39;});
Cyc_Tcutil_check_bitfield(loc,te,_tmp7A4,_tmp7A3,_tmp7A6);
Cyc_Tcutil_check_field_atts(loc,_tmp7A6,_tmp7A2);
if(_tmp7A1 != 0){
# 4612
if((int)_tmp7DA != (int)1U)
({void*_tmp79C=0U;({unsigned int _tmpC3B=loc;struct _dyneither_ptr _tmpC3A=({const char*_tmp79D="@requires clause is only allowed on union members";_tag_dyneither(_tmp79D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpC3B,_tmpC3A,_tag_dyneither(_tmp79C,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpC3C=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpC3C,0,_tmp7A1);});
if(!Cyc_Tcutil_is_integral(_tmp7A1))
({struct Cyc_String_pa_PrintArg_struct _tmp7A0=({struct Cyc_String_pa_PrintArg_struct _tmp9E5;_tmp9E5.tag=0U,({
struct _dyneither_ptr _tmpC3D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7A1->topt)));_tmp9E5.f1=_tmpC3D;});_tmp9E5;});void*_tmp79E[1U];_tmp79E[0]=& _tmp7A0;({unsigned int _tmpC3F=loc;struct _dyneither_ptr _tmpC3E=({const char*_tmp79F="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp79F,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpC3F,_tmpC3E,_tag_dyneither(_tmp79E,sizeof(void*),1U));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7A1,te,cvtenv);}}
# 4621
goto _LL0;}default: _LL17: _tmp7DE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DA)->f1;_tmp7DD=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DA)->f2;_tmp7DC=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DA)->f3;_tmp7DB=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DA)->f4;_LL18: {
# 4624
struct Cyc_List_List*targs=*_tmp7DD;
# 4626
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp7A7;_push_handler(& _tmp7A7);{int _tmp7A9=0;if(setjmp(_tmp7A7.handler))_tmp7A9=1;if(!_tmp7A9){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7DE);;_pop_handler();}else{void*_tmp7A8=(void*)_exn_thrown;void*_tmp7AA=_tmp7A8;void*_tmp7AE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7AA)->tag == Cyc_Dict_Absent){_LLB9: _LLBA:
# 4629
({struct Cyc_String_pa_PrintArg_struct _tmp7AD=({struct Cyc_String_pa_PrintArg_struct _tmp9E6;_tmp9E6.tag=0U,({struct _dyneither_ptr _tmpC40=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7DE));_tmp9E6.f1=_tmpC40;});_tmp9E6;});void*_tmp7AB[1U];_tmp7AB[0]=& _tmp7AD;({unsigned int _tmpC42=loc;struct _dyneither_ptr _tmpC41=({const char*_tmp7AC="unbound typedef name %s";_tag_dyneither(_tmp7AC,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpC42,_tmpC41,_tag_dyneither(_tmp7AB,sizeof(void*),1U));});});
return cvtenv;}else{_LLBB: _tmp7AE=_tmp7AA;_LLBC:(int)_rethrow(_tmp7AE);}_LLB8:;}};}
# 4632
*_tmp7DC=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4637
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4642
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));({struct _tuple15*_tmpC43=({struct _tuple15*_tmp7AF=_cycalloc(sizeof(*_tmp7AF));_tmp7AF->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7AF->f2=(void*)ts->hd;_tmp7AF;});_tmp7B0->hd=_tmpC43;}),_tmp7B0->tl=inst;_tmp7B0;});}
# 4646
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B3=({struct Cyc_String_pa_PrintArg_struct _tmp9E7;_tmp9E7.tag=0U,({struct _dyneither_ptr _tmpC44=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7DE));_tmp9E7.f1=_tmpC44;});_tmp9E7;});void*_tmp7B1[1U];_tmp7B1[0]=& _tmp7B3;({unsigned int _tmpC46=loc;struct _dyneither_ptr _tmpC45=({const char*_tmp7B2="too many parameters for typedef %s";_tag_dyneither(_tmp7B2,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpC46,_tmpC45,_tag_dyneither(_tmp7B1,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4651
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7B4=_cycalloc(sizeof(*_tmp7B4));_tmp7B4->hd=e,_tmp7B4->tl=hidden_ts;_tmp7B4;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));({struct _tuple15*_tmpC47=({struct _tuple15*_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7B5->f2=e;_tmp7B5;});_tmp7B6->hd=_tmpC47;}),_tmp7B6->tl=inst;_tmp7B6;});}
# 4659
({struct Cyc_List_List*_tmpC49=({struct Cyc_List_List*_tmpC48=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC48,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp7DD=_tmpC49;});}
# 4661
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp7DB=new_typ;}
# 4667
goto _LL0;};}}_LL0:;}
# 4669
if(!({struct Cyc_Absyn_Kind*_tmpC4A=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmpC4A,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp7E1=({struct Cyc_String_pa_PrintArg_struct _tmp9EA;_tmp9EA.tag=0U,({
struct _dyneither_ptr _tmpC4B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9EA.f1=_tmpC4B;});_tmp9EA;});struct Cyc_String_pa_PrintArg_struct _tmp7E2=({struct Cyc_String_pa_PrintArg_struct _tmp9E9;_tmp9E9.tag=0U,({struct _dyneither_ptr _tmpC4C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_type_kind(t)));_tmp9E9.f1=_tmpC4C;});_tmp9E9;});struct Cyc_String_pa_PrintArg_struct _tmp7E3=({struct Cyc_String_pa_PrintArg_struct _tmp9E8;_tmp9E8.tag=0U,({struct _dyneither_ptr _tmpC4D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmp9E8.f1=_tmpC4D;});_tmp9E8;});void*_tmp7DF[3U];_tmp7DF[0]=& _tmp7E1,_tmp7DF[1]=& _tmp7E2,_tmp7DF[2]=& _tmp7E3;({unsigned int _tmpC4F=loc;struct _dyneither_ptr _tmpC4E=({const char*_tmp7E0="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp7E0,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpC4F,_tmpC4E,_tag_dyneither(_tmp7DF,sizeof(void*),3U));});});
# 4673
return cvtenv;}
# 4681
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4683
{void*_tmp7E4=e->r;void*_tmp7E5=_tmp7E4;struct Cyc_Absyn_Exp*_tmp7F8;struct Cyc_Absyn_Exp*_tmp7F7;void*_tmp7F6;void*_tmp7F5;void*_tmp7F4;void*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F2;struct Cyc_Absyn_Exp*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F0;struct Cyc_Absyn_Exp*_tmp7EF;struct Cyc_Absyn_Exp*_tmp7EE;struct Cyc_Absyn_Exp*_tmp7ED;struct Cyc_Absyn_Exp*_tmp7EC;struct Cyc_Absyn_Exp*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EA;struct Cyc_Absyn_Exp*_tmp7E9;struct Cyc_List_List*_tmp7E8;switch(*((int*)_tmp7E5)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp7E8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7E5)->f2;_LLC:
# 4690
 for(0;_tmp7E8 != 0;_tmp7E8=_tmp7E8->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp7E8->hd,te,cvtenv);}
goto _LL0;case 6U: _LLD: _tmp7EB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_tmp7EA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E5)->f2;_tmp7E9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E5)->f3;_LLE:
# 4694
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EB,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EA,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7E9,te,cvtenv);
goto _LL0;case 7U: _LLF: _tmp7ED=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_tmp7EC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E5)->f2;_LL10:
 _tmp7EF=_tmp7ED;_tmp7EE=_tmp7EC;goto _LL12;case 8U: _LL11: _tmp7EF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_tmp7EE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E5)->f2;_LL12:
 _tmp7F1=_tmp7EF;_tmp7F0=_tmp7EE;goto _LL14;case 9U: _LL13: _tmp7F1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_tmp7F0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E5)->f2;_LL14:
# 4701
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F1,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F0,te,cvtenv);
goto _LL0;case 14U: _LL15: _tmp7F3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_tmp7F2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E5)->f2;_LL16:
# 4705
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F2,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F3,1,0);
goto _LL0;case 19U: _LL17: _tmp7F4=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_LL18:
 _tmp7F5=_tmp7F4;goto _LL1A;case 17U: _LL19: _tmp7F5=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_LL1A:
# 4710
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F5,1,0);
goto _LL0;case 39U: _LL1B: _tmp7F6=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_LL1C:
# 4713
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7F6,1,0);
goto _LL0;case 18U: _LL1D: _tmp7F7=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_LL1E:
# 4716
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F7,te,cvtenv);
goto _LL0;case 41U: _LL1F: _tmp7F8=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7E5)->f1;_LL20:
# 4719
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F8,te,cvtenv);
goto _LL0;default: _LL21: _LL22:
# 4722
({void*_tmp7E6=0U;({struct _dyneither_ptr _tmpC50=({const char*_tmp7E7="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp7E7,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC50,_tag_dyneither(_tmp7E6,sizeof(void*),0U));});});}_LL0:;}
# 4724
return cvtenv;}
# 4727
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4732
struct Cyc_List_List*_tmp7F9=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4735
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple29 _tmp7FA=*((struct _tuple29*)vs->hd);struct _tuple29 _tmp7FB=_tmp7FA;struct Cyc_Absyn_Tvar*_tmp7FC;_LL1: _tmp7FC=_tmp7FB.f1;_LL2:;
({struct Cyc_List_List*_tmpC51=Cyc_Tcutil_fast_add_free_tvar(_tmp7F9,_tmp7FC);cvt.kind_env=_tmpC51;});}}
# 4743
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple30 _tmp7FD=*((struct _tuple30*)evs->hd);struct _tuple30 _tmp7FE=_tmp7FD;void*_tmp807;_LL4: _tmp807=_tmp7FE.f1;_LL5:;{
void*_tmp7FF=Cyc_Tcutil_compress(_tmp807);void*_tmp800=_tmp7FF;struct Cyc_Core_Opt**_tmp806;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp800)->tag == 1U){_LL7: _tmp806=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp800)->f4;_LL8:
# 4747
 if(*_tmp806 == 0)
({struct Cyc_Core_Opt*_tmpC52=({struct Cyc_Core_Opt*_tmp801=_cycalloc(sizeof(*_tmp801));_tmp801->v=_tmp7F9;_tmp801;});*_tmp806=_tmpC52;});else{
# 4751
struct Cyc_List_List*_tmp802=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp806))->v;
struct Cyc_List_List*_tmp803=0;
for(0;_tmp802 != 0;_tmp802=_tmp802->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp7F9,(struct Cyc_Absyn_Tvar*)_tmp802->hd))
_tmp803=({struct Cyc_List_List*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804->hd=(struct Cyc_Absyn_Tvar*)_tmp802->hd,_tmp804->tl=_tmp803;_tmp804;});}
({struct Cyc_Core_Opt*_tmpC53=({struct Cyc_Core_Opt*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805->v=_tmp803;_tmp805;});*_tmp806=_tmpC53;});}
# 4758
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4762
return cvt;}
# 4769
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
{void*_tmp808=Cyc_Tcutil_compress(e);void*_tmp809=_tmp808;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp809)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
# 4775
 continue;}_LL0:;}{
# 4777
struct Cyc_Absyn_Kind*_tmp80A=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_tmp80B=_tmp80A;switch(((struct Cyc_Absyn_Kind*)_tmp80B)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp80B)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
# 4779
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_unique_rgn_type))
({void*_tmp80C=0U;({struct _dyneither_ptr _tmpC54=({const char*_tmp80D="can't unify evar with unique region!";_tag_dyneither(_tmp80D,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC54,_tag_dyneither(_tmp80C,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;case Cyc_Absyn_Top: _LLA: _LLB:
# 4784
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_heap_rgn_type))({void*_tmp80E=0U;({struct _dyneither_ptr _tmpC55=({const char*_tmp80F="can't unify evar with heap!";_tag_dyneither(_tmp80F,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC55,_tag_dyneither(_tmp80E,sizeof(void*),0U));});});
goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 4787
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp810=0U;({struct _dyneither_ptr _tmpC56=({const char*_tmp811="can't unify evar with {}!";_tag_dyneither(_tmp811,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC56,_tag_dyneither(_tmp810,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
# 4790
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_false_type))({struct Cyc_String_pa_PrintArg_struct _tmp814=({struct Cyc_String_pa_PrintArg_struct _tmp9EB;_tmp9EB.tag=0U,({
struct _dyneither_ptr _tmpC57=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp9EB.f1=_tmpC57;});_tmp9EB;});void*_tmp812[1U];_tmp812[0]=& _tmp814;({struct _dyneither_ptr _tmpC58=({const char*_tmp813="can't unify evar %s with @false!";_tag_dyneither(_tmp813,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC58,_tag_dyneither(_tmp812,sizeof(void*),1U));});});
goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
# 4794
 if(!({void*_tmpC59=e;Cyc_Tcutil_unify(_tmpC59,Cyc_Absyn_bounds_one());}))({void*_tmp815=0U;({struct _dyneither_ptr _tmpC5A=({const char*_tmp816="can't unify evar with bounds_one!";_tag_dyneither(_tmp816,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5A,_tag_dyneither(_tmp815,sizeof(void*),0U));});});
goto _LL5;default: _LL12: _LL13:
# 4797
({struct Cyc_String_pa_PrintArg_struct _tmp819=({struct Cyc_String_pa_PrintArg_struct _tmp9ED;_tmp9ED.tag=0U,({
struct _dyneither_ptr _tmpC5B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp9ED.f1=_tmpC5B;});_tmp9ED;});struct Cyc_String_pa_PrintArg_struct _tmp81A=({struct Cyc_String_pa_PrintArg_struct _tmp9EC;_tmp9EC.tag=0U,({struct _dyneither_ptr _tmpC5C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmp9EC.f1=_tmpC5C;});_tmp9EC;});void*_tmp817[2U];_tmp817[0]=& _tmp819,_tmp817[1]=& _tmp81A;({unsigned int _tmpC5E=loc;struct _dyneither_ptr _tmpC5D=({const char*_tmp818="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp818,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpC5E,_tmpC5D,_tag_dyneither(_tmp817,sizeof(void*),2U));});});
goto _LL5;}_LL5:;};}}
# 4809
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
if(te->in_tempest  && !te->tempest_generalize)generalize_evars=0;{
struct Cyc_List_List*_tmp81B=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp81C=({unsigned int _tmpC62=loc;struct Cyc_Tcenv_Tenv*_tmpC61=te;struct Cyc_Tcutil_CVTEnv _tmpC60=({struct Cyc_Tcutil_CVTEnv _tmp9F1;_tmp9F1.r=Cyc_Core_heap_region,_tmp9F1.kind_env=_tmp81B,_tmp9F1.free_vars=0,_tmp9F1.free_evars=0,_tmp9F1.generalize_evars=generalize_evars,_tmp9F1.fn_result=0;_tmp9F1;});struct Cyc_Absyn_Kind*_tmpC5F=expected_kind;Cyc_Tcutil_check_valid_type(_tmpC62,_tmpC61,_tmpC60,_tmpC5F,1,t);});
# 4818
struct Cyc_List_List*_tmp81D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,_tmp81C.free_vars);
struct Cyc_List_List*_tmp81E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple30*))Cyc_Core_fst,_tmp81C.free_evars);
# 4822
if(_tmp81B != 0){
struct Cyc_List_List*_tmp81F=0;
{struct Cyc_List_List*_tmp820=_tmp81D;for(0;(unsigned int)_tmp820;_tmp820=_tmp820->tl){
struct Cyc_Absyn_Tvar*_tmp821=(struct Cyc_Absyn_Tvar*)_tmp820->hd;
int found=0;
{struct Cyc_List_List*_tmp822=_tmp81B;for(0;(unsigned int)_tmp822;_tmp822=_tmp822->tl){
if(Cyc_Absyn_tvar_cmp(_tmp821,(struct Cyc_Absyn_Tvar*)_tmp822->hd)== 0){found=1;break;}}}
if(!found)
_tmp81F=({struct Cyc_List_List*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823->hd=(struct Cyc_Absyn_Tvar*)_tmp820->hd,_tmp823->tl=_tmp81F;_tmp823;});}}
# 4832
_tmp81D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp81F);}
# 4838
{struct Cyc_List_List*x=_tmp81D;for(0;x != 0;x=x->tl){
void*_tmp824=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp825=_tmp824;enum Cyc_Absyn_AliasQual _tmp833;struct Cyc_Core_Opt**_tmp832;struct Cyc_Absyn_Kind*_tmp831;struct Cyc_Core_Opt**_tmp830;struct Cyc_Core_Opt**_tmp82F;struct Cyc_Core_Opt**_tmp82E;struct Cyc_Core_Opt**_tmp82D;struct Cyc_Core_Opt**_tmp82C;struct Cyc_Core_Opt**_tmp82B;switch(*((int*)_tmp825)){case 1U: _LL1: _tmp82B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp825)->f1;_LL2:
 _tmp82C=_tmp82B;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp82C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f1;_LL4:
 _tmp82D=_tmp82C;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp82D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f1;_LL6:
 _tmp82E=_tmp82D;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp82E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f1;_LL8:
# 4844
({struct Cyc_Core_Opt*_tmpC63=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp82E=_tmpC63;});goto _LL0;case Cyc_Absyn_Unique: _LL9: _tmp82F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f1;_LLA:
# 4846
({struct Cyc_Core_Opt*_tmpC64=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp82F=_tmpC64;});goto _LL0;default: goto _LLD;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp830=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f1;_LLC:
# 4848
({struct Cyc_Core_Opt*_tmpC65=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp830=_tmpC65;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp832=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f1;_tmp831=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp825)->f2;_LLE:
# 4850
({struct Cyc_Core_Opt*_tmpC66=Cyc_Tcutil_kind_to_bound_opt(_tmp831);*_tmp832=_tmpC66;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp825)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp833=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp825)->f1)->aliasqual;_LL10:
# 4852
({struct Cyc_String_pa_PrintArg_struct _tmp828=({struct Cyc_String_pa_PrintArg_struct _tmp9EF;_tmp9EF.tag=0U,({
struct _dyneither_ptr _tmpC67=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmp9EF.f1=_tmpC67;});_tmp9EF;});struct Cyc_String_pa_PrintArg_struct _tmp829=({struct Cyc_String_pa_PrintArg_struct _tmp9EE;_tmp9EE.tag=0U,({struct _dyneither_ptr _tmpC68=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp82A=_cycalloc(sizeof(*_tmp82A));_tmp82A->kind=Cyc_Absyn_MemKind,_tmp82A->aliasqual=_tmp833;_tmp82A;})));_tmp9EE.f1=_tmpC68;});_tmp9EE;});void*_tmp826[2U];_tmp826[0]=& _tmp828,_tmp826[1]=& _tmp829;({unsigned int _tmpC6A=loc;struct _dyneither_ptr _tmpC69=({const char*_tmp827="type variable %s cannot have kind %s";_tag_dyneither(_tmp827,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpC6A,_tmpC69,_tag_dyneither(_tmp826,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4859
if(_tmp81D != 0  || _tmp81E != 0){
{void*_tmp834=Cyc_Tcutil_compress(t);void*_tmp835=_tmp834;struct Cyc_List_List**_tmp836;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp835)->tag == 5U){_LL14: _tmp836=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp835)->f1).tvars;_LL15:
# 4862
 if(*_tmp836 == 0){
# 4864
({struct Cyc_List_List*_tmpC6B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp81D);*_tmp836=_tmpC6B;});
_tmp81D=0;}
# 4867
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4870
if(_tmp81D != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp839=({struct Cyc_String_pa_PrintArg_struct _tmp9F0;_tmp9F0.tag=0U,_tmp9F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp81D->hd)->name);_tmp9F0;});void*_tmp837[1U];_tmp837[0]=& _tmp839;({unsigned int _tmpC6D=loc;struct _dyneither_ptr _tmpC6C=({const char*_tmp838="unbound type variable %s";_tag_dyneither(_tmp838,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC6D,_tmpC6C,_tag_dyneither(_tmp837,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_function_type(t) || !te->in_tempest)
Cyc_Tcutil_check_free_evars(_tmp81E,t,loc);}};}
# 4881
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 4884
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp83A=Cyc_Tcutil_compress(t);void*_tmp83B=_tmp83A;struct Cyc_Absyn_FnInfo _tmp83E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83B)->tag == 5U){_LL1: _tmp83E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83B)->f1;_LL2:
# 4887
 fd->i=_tmp83E;
({int _tmpC6E=
Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp83E.ret_tqual).print_const,_tmp83E.ret_type);
# 4888
((fd->i).ret_tqual).real_const=_tmpC6E;});
# 4890
goto _LL0;}else{_LL3: _LL4:
({void*_tmp83C=0U;({struct _dyneither_ptr _tmpC6F=({const char*_tmp83D="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp83D,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC6F,_tag_dyneither(_tmp83C,sizeof(void*),0U));});});}_LL0:;}{
# 4893
struct _RegionHandle _tmp83F=_new_region("r");struct _RegionHandle*r=& _tmp83F;_push_region(r);
({struct Cyc_List_List*_tmpC71=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,(fd->i).args);unsigned int _tmpC70=loc;Cyc_Tcutil_check_unique_vars(_tmpC71,_tmpC70,({const char*_tmp840="function declaration has repeated parameter";_tag_dyneither(_tmp840,sizeof(char),44U);}));});
# 4897
fd->cached_type=t;
# 4894
;_pop_region(r);};}
# 4902
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4908
struct Cyc_Tcutil_CVTEnv _tmp841=({unsigned int _tmpC76=loc;struct Cyc_Tcenv_Tenv*_tmpC75=te;struct Cyc_Tcutil_CVTEnv _tmpC74=({struct Cyc_Tcutil_CVTEnv _tmp9F4;_tmp9F4.r=Cyc_Core_heap_region,_tmp9F4.kind_env=bound_tvars,_tmp9F4.free_vars=0,_tmp9F4.free_evars=0,_tmp9F4.generalize_evars=0,_tmp9F4.fn_result=0;_tmp9F4;});struct Cyc_Absyn_Kind*_tmpC73=expected_kind;int _tmpC72=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpC76,_tmpC75,_tmpC74,_tmpC73,_tmpC72,t);});
# 4912
struct Cyc_List_List*_tmp842=({struct _RegionHandle*_tmpC78=Cyc_Core_heap_region;struct Cyc_List_List*_tmpC77=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,_tmp841.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpC78,_tmpC77,bound_tvars);});
# 4914
struct Cyc_List_List*_tmp843=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple30*))Cyc_Core_fst,_tmp841.free_evars);
{struct Cyc_List_List*fs=_tmp842;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp844=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp847=({struct Cyc_String_pa_PrintArg_struct _tmp9F3;_tmp9F3.tag=0U,_tmp9F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp844);_tmp9F3;});struct Cyc_String_pa_PrintArg_struct _tmp848=({struct Cyc_String_pa_PrintArg_struct _tmp9F2;_tmp9F2.tag=0U,({struct _dyneither_ptr _tmpC79=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9F2.f1=_tmpC79;});_tmp9F2;});void*_tmp845[2U];_tmp845[0]=& _tmp847,_tmp845[1]=& _tmp848;({unsigned int _tmpC7B=loc;struct _dyneither_ptr _tmpC7A=({const char*_tmp846="unbound type variable %s in type %s";_tag_dyneither(_tmp846,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpC7B,_tmpC7A,_tag_dyneither(_tmp845,sizeof(void*),2U));});});}}
# 4919
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp843,t,loc);}
# 4923
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpC7C=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpC7C;});}
# 4928
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4934
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4937
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp84B=({struct Cyc_String_pa_PrintArg_struct _tmp9F6;_tmp9F6.tag=0U,_tmp9F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmp9F6;});struct Cyc_String_pa_PrintArg_struct _tmp84C=({struct Cyc_String_pa_PrintArg_struct _tmp9F5;_tmp9F5.tag=0U,({struct _dyneither_ptr _tmpC7D=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmp9F5.f1=_tmpC7D;});_tmp9F5;});void*_tmp849[2U];_tmp849[0]=& _tmp84B,_tmp849[1]=& _tmp84C;({unsigned int _tmpC7F=loc;struct _dyneither_ptr _tmpC7E=({const char*_tmp84A="%s: %s";_tag_dyneither(_tmp84A,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpC7F,_tmpC7E,_tag_dyneither(_tmp849,sizeof(void*),2U));});});}}}
# 4943
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4947
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4951
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpC81=tvs;unsigned int _tmpC80=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpC81,_tmpC80,Cyc_Absynpp_tvar2string,({const char*_tmp84D="duplicate type variable";_tag_dyneither(_tmp84D,sizeof(char),24U);}));});}struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple34{struct Cyc_List_List*f1;void*f2;};struct _tuple35{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4965 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4970
struct _RegionHandle _tmp84E=_new_region("temp");struct _RegionHandle*temp=& _tmp84E;_push_region(temp);
# 4974
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpC82=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpC82,({const char*_tmp84F="";_tag_dyneither(_tmp84F,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp851=_region_malloc(temp,sizeof(*_tmp851));({struct _tuple33*_tmpC83=({struct _tuple33*_tmp850=_region_malloc(temp,sizeof(*_tmp850));_tmp850->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp850->f2=0;_tmp850;});_tmp851->hd=_tmpC83;}),_tmp851->tl=fields;_tmp851;});}}
# 4979
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4981
struct _dyneither_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp876="struct";_tag_dyneither(_tmp876,sizeof(char),7U);}):({const char*_tmp877="union";_tag_dyneither(_tmp877,sizeof(char),6U);});
# 4984
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple34*_tmp852=(struct _tuple34*)des->hd;struct _tuple34*_tmp853=_tmp852;struct Cyc_List_List*_tmp86E;void*_tmp86D;_LL1: _tmp86E=_tmp853->f1;_tmp86D=_tmp853->f2;_LL2:;
if(_tmp86E == 0){
# 4989
struct Cyc_List_List*_tmp854=fields;
for(0;_tmp854 != 0;_tmp854=_tmp854->tl){
if(!(*((struct _tuple33*)_tmp854->hd)).f2){
(*((struct _tuple33*)_tmp854->hd)).f2=1;
({struct Cyc_List_List*_tmpC85=({struct Cyc_List_List*_tmp856=_cycalloc(sizeof(*_tmp856));({void*_tmpC84=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855->tag=1U,_tmp855->f1=((*((struct _tuple33*)_tmp854->hd)).f1)->name;_tmp855;});_tmp856->hd=_tmpC84;}),_tmp856->tl=0;_tmp856;});(*((struct _tuple34*)des->hd)).f1=_tmpC85;});
ans=({struct Cyc_List_List*_tmp858=_region_malloc(rgn,sizeof(*_tmp858));({struct _tuple35*_tmpC86=({struct _tuple35*_tmp857=_region_malloc(rgn,sizeof(*_tmp857));_tmp857->f1=(*((struct _tuple33*)_tmp854->hd)).f1,_tmp857->f2=_tmp86D;_tmp857;});_tmp858->hd=_tmpC86;}),_tmp858->tl=ans;_tmp858;});
break;}}
# 4997
if(_tmp854 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp85B=({struct Cyc_String_pa_PrintArg_struct _tmp9F7;_tmp9F7.tag=0U,_tmp9F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp9F7;});void*_tmp859[1U];_tmp859[0]=& _tmp85B;({unsigned int _tmpC88=loc;struct _dyneither_ptr _tmpC87=({const char*_tmp85A="too many arguments to %s";_tag_dyneither(_tmp85A,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC88,_tmpC87,_tag_dyneither(_tmp859,sizeof(void*),1U));});});}else{
if(_tmp86E->tl != 0)
# 5001
({void*_tmp85C=0U;({unsigned int _tmpC8A=loc;struct _dyneither_ptr _tmpC89=({const char*_tmp85D="multiple designators are not yet supported";_tag_dyneither(_tmp85D,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpC8A,_tmpC89,_tag_dyneither(_tmp85C,sizeof(void*),0U));});});else{
# 5004
void*_tmp85E=(void*)_tmp86E->hd;void*_tmp85F=_tmp85E;struct _dyneither_ptr*_tmp86C;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp85F)->tag == 0U){_LL4: _LL5:
# 5006
({struct Cyc_String_pa_PrintArg_struct _tmp862=({struct Cyc_String_pa_PrintArg_struct _tmp9F8;_tmp9F8.tag=0U,_tmp9F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmp9F8;});void*_tmp860[1U];_tmp860[0]=& _tmp862;({unsigned int _tmpC8C=loc;struct _dyneither_ptr _tmpC8B=({const char*_tmp861="array designator used in argument to %s";_tag_dyneither(_tmp861,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC8C,_tmpC8B,_tag_dyneither(_tmp860,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp86C=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp85F)->f1;_LL7: {
# 5009
struct Cyc_List_List*_tmp863=fields;
for(0;_tmp863 != 0;_tmp863=_tmp863->tl){
if(Cyc_strptrcmp(_tmp86C,((*((struct _tuple33*)_tmp863->hd)).f1)->name)== 0){
if((*((struct _tuple33*)_tmp863->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp866=({struct Cyc_String_pa_PrintArg_struct _tmp9F9;_tmp9F9.tag=0U,_tmp9F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp86C);_tmp9F9;});void*_tmp864[1U];_tmp864[0]=& _tmp866;({unsigned int _tmpC8E=loc;struct _dyneither_ptr _tmpC8D=({const char*_tmp865="member %s has already been used as an argument";_tag_dyneither(_tmp865,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpC8E,_tmpC8D,_tag_dyneither(_tmp864,sizeof(void*),1U));});});
(*((struct _tuple33*)_tmp863->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp868=_region_malloc(rgn,sizeof(*_tmp868));({struct _tuple35*_tmpC8F=({struct _tuple35*_tmp867=_region_malloc(rgn,sizeof(*_tmp867));_tmp867->f1=(*((struct _tuple33*)_tmp863->hd)).f1,_tmp867->f2=_tmp86D;_tmp867;});_tmp868->hd=_tmpC8F;}),_tmp868->tl=ans;_tmp868;});
break;}}
# 5018
if(_tmp863 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp86B=({struct Cyc_String_pa_PrintArg_struct _tmp9FA;_tmp9FA.tag=0U,_tmp9FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp86C);_tmp9FA;});void*_tmp869[1U];_tmp869[0]=& _tmp86B;({unsigned int _tmpC91=loc;struct _dyneither_ptr _tmpC90=({const char*_tmp86A="bad field designator %s";_tag_dyneither(_tmp86A,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpC91,_tmpC90,_tag_dyneither(_tmp869,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 5023
if((int)aggr_kind == (int)0U)
# 5025
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple33*)fields->hd)).f2){
({void*_tmp86F=0U;({unsigned int _tmpC93=loc;struct _dyneither_ptr _tmpC92=({const char*_tmp870="too few arguments to struct";_tag_dyneither(_tmp870,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpC93,_tmpC92,_tag_dyneither(_tmp86F,sizeof(void*),0U));});});
break;}}else{
# 5032
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple33*)fields->hd)).f2){
if(found)({void*_tmp871=0U;({unsigned int _tmpC95=loc;struct _dyneither_ptr _tmpC94=({const char*_tmp872="only one member of a union is allowed";_tag_dyneither(_tmp872,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpC95,_tmpC94,_tag_dyneither(_tmp871,sizeof(void*),0U));});});
found=1;}}
# 5039
if(!found)({void*_tmp873=0U;({unsigned int _tmpC97=loc;struct _dyneither_ptr _tmpC96=({const char*_tmp874="missing member for union";_tag_dyneither(_tmp874,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpC97,_tmpC96,_tag_dyneither(_tmp873,sizeof(void*),0U));});});}{
# 5042
struct Cyc_List_List*_tmp875=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp875;};};}
# 4974
;_pop_region(temp);}
# 5048
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5050
void*_tmp878=e1->r;void*_tmp879=_tmp878;struct Cyc_Absyn_Exp*_tmp88B;struct Cyc_Absyn_Exp*_tmp88A;struct Cyc_Absyn_Exp*_tmp889;struct Cyc_Absyn_Exp*_tmp888;struct Cyc_Absyn_Exp*_tmp887;struct Cyc_Absyn_Exp*_tmp886;switch(*((int*)_tmp879)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp87C=({struct Cyc_String_pa_PrintArg_struct _tmp9FB;_tmp9FB.tag=0U,({struct _dyneither_ptr _tmpC98=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9FB.f1=_tmpC98;});_tmp9FB;});void*_tmp87A[1U];_tmp87A[0]=& _tmp87C;({struct _dyneither_ptr _tmpC99=({const char*_tmp87B="we have a cast in a lhs:  %s";_tag_dyneither(_tmp87B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC99,_tag_dyneither(_tmp87A,sizeof(void*),1U));});});case 20U: _LL3: _tmp886=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp879)->f1;_LL4:
 _tmp887=_tmp886;goto _LL6;case 23U: _LL5: _tmp887=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp879)->f1;_LL6:
# 5054
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp887->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp888=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp879)->f1;_LL8:
 _tmp889=_tmp888;goto _LLA;case 21U: _LL9: _tmp889=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp879)->f1;_LLA:
# 5058
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp889->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp87F=({struct Cyc_String_pa_PrintArg_struct _tmp9FC;_tmp9FC.tag=0U,({
struct _dyneither_ptr _tmpC9A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9FC.f1=_tmpC9A;});_tmp9FC;});void*_tmp87D[1U];_tmp87D[0]=& _tmp87F;({struct _dyneither_ptr _tmpC9B=({const char*_tmp87E="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp87E,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC9B,_tag_dyneither(_tmp87D,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp88A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp879)->f1;_LLC:
 _tmp88B=_tmp88A;goto _LLE;case 12U: _LLD: _tmp88B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp879)->f1;_LLE:
# 5064
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp88B->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp882=({struct Cyc_String_pa_PrintArg_struct _tmp9FD;_tmp9FD.tag=0U,({
struct _dyneither_ptr _tmpC9C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9FD.f1=_tmpC9C;});_tmp9FD;});void*_tmp880[1U];_tmp880[0]=& _tmp882;({struct _dyneither_ptr _tmpC9D=({const char*_tmp881="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp881,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC9D,_tag_dyneither(_tmp880,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp885=({struct Cyc_String_pa_PrintArg_struct _tmp9FE;_tmp9FE.tag=0U,({struct _dyneither_ptr _tmpC9E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9FE.f1=_tmpC9E;});_tmp9FE;});void*_tmp883[1U];_tmp883[0]=& _tmp885;({struct _dyneither_ptr _tmpC9F=({const char*_tmp884="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp884,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC9F,_tag_dyneither(_tmp883,sizeof(void*),1U));});});}_LL0:;}
# 5079
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5082
void*_tmp88C=Cyc_Tcutil_compress(r);void*_tmp88D=_tmp88C;struct Cyc_Absyn_Tvar*_tmp899;enum Cyc_Absyn_KindQual _tmp898;enum Cyc_Absyn_AliasQual _tmp897;switch(*((int*)_tmp88D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp88D)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88D)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88D)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88D)->f4 == 0){_LL5: _tmp898=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88D)->f3)->kind)->v)->kind;_tmp897=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp88D)->f3)->kind)->v)->aliasqual;_LL6:
# 5086
 return(int)_tmp898 == (int)3U  && ((int)_tmp897 == (int)1U  || (int)_tmp897 == (int)2U  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp899=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp88D)->f1;_LL8: {
# 5090
struct Cyc_Absyn_Kind*_tmp88E=Cyc_Tcutil_tvar_kind(_tmp899,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp88F=_tmp88E;enum Cyc_Absyn_KindQual _tmp896;enum Cyc_Absyn_AliasQual _tmp895;_LLC: _tmp896=_tmp88F->kind;_tmp895=_tmp88F->aliasqual;_LLD:;
if((int)_tmp896 == (int)3U  && ((int)_tmp895 == (int)1U  || (int)_tmp895 == (int)2U  && !must_be_unique)){
void*_tmp890=Cyc_Absyn_compress_kb(_tmp899->kind);void*_tmp891=_tmp890;struct Cyc_Core_Opt**_tmp894;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp891)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp891)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp891)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp894=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp891)->f1;_LL10:
# 5094
({struct Cyc_Core_Opt*_tmpCA1=({struct Cyc_Core_Opt*_tmp893=_cycalloc(sizeof(*_tmp893));({void*_tmpCA0=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892->tag=2U,_tmp892->f1=0,_tmp892->f2=& Cyc_Tcutil_rk;_tmp892;});_tmp893->v=_tmpCA0;});_tmp893;});*_tmp894=_tmpCA1;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5099
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5106
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp89A=Cyc_Tcutil_compress(t);void*_tmp89B=_tmp89A;struct Cyc_Absyn_Tvar*_tmp8A9;void*_tmp8A8;switch(*((int*)_tmp89B)){case 3U: _LL1: _tmp8A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp89B)->f1).ptr_atts).rgn;_LL2:
# 5109
 return Cyc_Tcutil_is_noalias_region(_tmp8A8,must_be_unique);case 2U: _LL3: _tmp8A9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp89B)->f1;_LL4: {
# 5111
struct Cyc_Absyn_Kind*_tmp89C=Cyc_Tcutil_tvar_kind(_tmp8A9,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp89D=_tmp89C;enum Cyc_Absyn_KindQual _tmp8A7;enum Cyc_Absyn_AliasQual _tmp8A6;_LL8: _tmp8A7=_tmp89D->kind;_tmp8A6=_tmp89D->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp89E=_tmp8A7;switch(_tmp89E){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)_tmp8A6 == (int)1U  || (int)_tmp8A6 == (int)2U  && !must_be_unique){
void*_tmp89F=Cyc_Absyn_compress_kb(_tmp8A9->kind);void*_tmp8A0=_tmp89F;struct Cyc_Core_Opt**_tmp8A5;enum Cyc_Absyn_KindQual _tmp8A4;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A0)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A0)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp8A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A0)->f1;_tmp8A4=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A0)->f2)->kind;_LL15:
# 5117
({struct Cyc_Core_Opt*_tmpCA4=({struct Cyc_Core_Opt*_tmp8A3=_cycalloc(sizeof(*_tmp8A3));({void*_tmpCA3=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8A2=_cycalloc(sizeof(*_tmp8A2));_tmp8A2->tag=2U,_tmp8A2->f1=0,({struct Cyc_Absyn_Kind*_tmpCA2=({struct Cyc_Absyn_Kind*_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1->kind=_tmp8A4,_tmp8A1->aliasqual=Cyc_Absyn_Aliasable;_tmp8A1;});_tmp8A2->f2=_tmpCA2;});_tmp8A2;});_tmp8A3->v=_tmpCA3;});_tmp8A3;});*_tmp8A5=_tmpCA4;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5121
 return 1;}_LL13:;}
# 5124
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5127
 return 0;}_LL0:;}
# 5130
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp8AA=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp8AA,0))return 1;{
void*_tmp8AB=_tmp8AA;struct Cyc_List_List*_tmp8BF;union Cyc_Absyn_DatatypeFieldInfo _tmp8BE;struct Cyc_List_List*_tmp8BD;union Cyc_Absyn_DatatypeInfo _tmp8BC;struct Cyc_List_List*_tmp8BB;struct Cyc_Absyn_Aggrdecl**_tmp8BA;struct Cyc_List_List*_tmp8B9;struct Cyc_List_List*_tmp8B8;switch(*((int*)_tmp8AB)){case 6U: _LL1: _tmp8B8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8AB)->f1;_LL2:
# 5135
 while(_tmp8B8 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp8B8->hd)).f2))return 1;
_tmp8B8=_tmp8B8->tl;}
# 5139
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp8BA=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f1)->f1).KnownAggr).val;_tmp8B9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f2;_LL4:
# 5141
 if((*_tmp8BA)->impl == 0)return 0;else{
# 5143
struct Cyc_List_List*_tmp8AC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp8BA)->tvs,_tmp8B9);
struct Cyc_List_List*_tmp8AD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp8BA)->impl))->fields;
void*t;
while(_tmp8AD != 0){
t=_tmp8AC == 0?((struct Cyc_Absyn_Aggrfield*)_tmp8AD->hd)->type: Cyc_Tcutil_substitute(_tmp8AC,((struct Cyc_Absyn_Aggrfield*)_tmp8AD->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp8AD=_tmp8AD->tl;}
# 5151
return 0;}}else{_LL7: _LL8:
# 5161
 return 0;}case 18U: _LL9: _tmp8BC=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f1)->f1;_tmp8BB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f2;_LLA: {
# 5163
union Cyc_Absyn_DatatypeInfo _tmp8AE=_tmp8BC;struct Cyc_List_List*_tmp8B2;struct Cyc_Core_Opt*_tmp8B1;struct _tuple2*_tmp8B0;int _tmp8AF;if((_tmp8AE.UnknownDatatype).tag == 1){_LL10: _tmp8B0=((_tmp8AE.UnknownDatatype).val).name;_tmp8AF=((_tmp8AE.UnknownDatatype).val).is_extensible;_LL11:
# 5166
 return 0;}else{_LL12: _tmp8B2=(*(_tmp8AE.KnownDatatype).val)->tvs;_tmp8B1=(*(_tmp8AE.KnownDatatype).val)->fields;_LL13:
# 5169
 return 0;}_LLF:;}case 19U: _LLB: _tmp8BE=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f1)->f1;_tmp8BD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8AB)->f2;_LLC: {
# 5172
union Cyc_Absyn_DatatypeFieldInfo _tmp8B3=_tmp8BE;struct Cyc_Absyn_Datatypedecl*_tmp8B7;struct Cyc_Absyn_Datatypefield*_tmp8B6;if((_tmp8B3.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5175
 return 0;}else{_LL17: _tmp8B7=((_tmp8B3.KnownDatatypefield).val).f1;_tmp8B6=((_tmp8B3.KnownDatatypefield).val).f2;_LL18: {
# 5177
struct Cyc_List_List*_tmp8B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp8B7->tvs,_tmp8BD);
struct Cyc_List_List*_tmp8B5=_tmp8B6->typs;
while(_tmp8B5 != 0){
_tmp8AA=_tmp8B4 == 0?(*((struct _tuple12*)_tmp8B5->hd)).f2: Cyc_Tcutil_substitute(_tmp8B4,(*((struct _tuple12*)_tmp8B5->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp8AA))return 1;
_tmp8B5=_tmp8B5->tl;}
# 5184
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp8BF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8AB)->f2;_LL6:
# 5154
 while(_tmp8BF != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp8BF->hd)->type))return 1;
_tmp8BF=_tmp8BF->tl;}
# 5158
return 0;default: _LLD: _LLE:
# 5186
 return 0;}_LL0:;};}
# 5193
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp8C0=e->r;void*_tmp8C1=_tmp8C0;struct Cyc_Absyn_Stmt*_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Exp*_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D4;struct Cyc_Absyn_Exp*_tmp8D3;struct Cyc_Absyn_Exp*_tmp8D2;struct Cyc_Absyn_Exp*_tmp8D1;struct Cyc_Absyn_Exp*_tmp8D0;struct _dyneither_ptr*_tmp8CF;struct Cyc_Absyn_Exp*_tmp8CE;struct Cyc_Absyn_Exp*_tmp8CD;switch(*((int*)_tmp8C1)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8C1)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp8CD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8C1)->f1;_LL4:
 _tmp8CE=_tmp8CD;goto _LL6;case 20U: _LL5: _tmp8CE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8C1)->f1;_LL6:
# 5198
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp8CE->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp8CE);case 21U: _LL7: _tmp8D0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C1)->f1;_tmp8CF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8C1)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp8D0);case 23U: _LL9: _tmp8D2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8C1)->f1;_tmp8D1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8C1)->f2;_LLA: {
# 5201
void*_tmp8C2=Cyc_Tcutil_compress((void*)_check_null(_tmp8D2->topt));void*_tmp8C3=_tmp8C2;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8C3)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp8D2);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp8D4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8C1)->f2;_tmp8D3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8C1)->f3;_LLC:
# 5206
 return Cyc_Tcutil_is_noalias_path(_tmp8D4) && Cyc_Tcutil_is_noalias_path(_tmp8D3);case 9U: _LLD: _tmp8D5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8C1)->f2;_LLE:
 _tmp8D6=_tmp8D5;goto _LL10;case 14U: _LLF: _tmp8D6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8C1)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp8D6);case 37U: _LL11: _tmp8D7=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8C1)->f1;_LL12:
# 5210
 while(1){
void*_tmp8C4=_tmp8D7->r;void*_tmp8C5=_tmp8C4;struct Cyc_Absyn_Exp*_tmp8CC;struct Cyc_Absyn_Decl*_tmp8CB;struct Cyc_Absyn_Stmt*_tmp8CA;struct Cyc_Absyn_Stmt*_tmp8C9;struct Cyc_Absyn_Stmt*_tmp8C8;switch(*((int*)_tmp8C5)){case 2U: _LL1B: _tmp8C9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C5)->f1;_tmp8C8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8C5)->f2;_LL1C:
 _tmp8D7=_tmp8C8;goto _LL1A;case 12U: _LL1D: _tmp8CB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C5)->f1;_tmp8CA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8C5)->f2;_LL1E:
 _tmp8D7=_tmp8CA;goto _LL1A;case 1U: _LL1F: _tmp8CC=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8C5)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp8CC);default: _LL21: _LL22:
({void*_tmp8C6=0U;({struct _dyneither_ptr _tmpCA5=({const char*_tmp8C7="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp8C7,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCA5,_tag_dyneither(_tmp8C6,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5218
 return 1;}_LL0:;}
# 5235 "tcutil.cyc"
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5237
struct _tuple17 bogus_ans=({struct _tuple17 _tmpA0B;_tmpA0B.f1=0,_tmpA0B.f2=Cyc_Absyn_heap_rgn_type;_tmpA0B;});
void*_tmp8D8=e->r;void*_tmp8D9=_tmp8D8;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp910;struct Cyc_Absyn_Exp*_tmp90F;struct _dyneither_ptr*_tmp90E;int _tmp90D;struct Cyc_Absyn_Exp*_tmp90C;struct _dyneither_ptr*_tmp90B;int _tmp90A;void*_tmp909;switch(*((int*)_tmp8D9)){case 1U: _LL1: _tmp909=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8D9)->f1;_LL2: {
# 5241
void*_tmp8DA=_tmp909;struct Cyc_Absyn_Vardecl*_tmp8E2;struct Cyc_Absyn_Vardecl*_tmp8E1;struct Cyc_Absyn_Vardecl*_tmp8E0;struct Cyc_Absyn_Vardecl*_tmp8DF;switch(*((int*)_tmp8DA)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp8DF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp8DA)->f1;_LL13: {
# 5245
void*_tmp8DB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8DC=_tmp8DB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DC)->tag == 4U){_LL1B: _LL1C:
# 5247
 return({struct _tuple17 _tmp9FF;_tmp9FF.f1=1,_tmp9FF.f2=Cyc_Absyn_heap_rgn_type;_tmp9FF;});}else{_LL1D: _LL1E:
 return({struct _tuple17 _tmpA00;_tmpA00.f1=(_tmp8DF->tq).real_const,_tmpA00.f2=Cyc_Absyn_heap_rgn_type;_tmpA00;});}_LL1A:;}case 4U: _LL14: _tmp8E0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp8DA)->f1;_LL15: {
# 5251
void*_tmp8DD=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8DE=_tmp8DD;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DE)->tag == 4U){_LL20: _LL21:
 return({struct _tuple17 _tmpA01;_tmpA01.f1=1,_tmpA01.f2=(void*)_check_null(_tmp8E0->rgn);_tmpA01;});}else{_LL22: _LL23:
# 5254
 _tmp8E0->escapes=1;
return({struct _tuple17 _tmpA02;_tmpA02.f1=(_tmp8E0->tq).real_const,_tmpA02.f2=(void*)_check_null(_tmp8E0->rgn);_tmpA02;});}_LL1F:;}case 5U: _LL16: _tmp8E1=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp8DA)->f1;_LL17:
# 5257
 _tmp8E2=_tmp8E1;goto _LL19;default: _LL18: _tmp8E2=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp8DA)->f1;_LL19:
# 5259
 _tmp8E2->escapes=1;
return({struct _tuple17 _tmpA03;_tmpA03.f1=(_tmp8E2->tq).real_const,_tmpA03.f2=(void*)_check_null(_tmp8E2->rgn);_tmpA03;});}_LLD:;}case 21U: _LL3: _tmp90C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8D9)->f1;_tmp90B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8D9)->f2;_tmp90A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8D9)->f3;_LL4:
# 5264
 if(_tmp90A)return bogus_ans;{
# 5267
void*_tmp8E3=Cyc_Tcutil_compress((void*)_check_null(_tmp90C->topt));void*_tmp8E4=_tmp8E3;struct Cyc_Absyn_Aggrdecl*_tmp8F0;struct Cyc_List_List*_tmp8EF;switch(*((int*)_tmp8E4)){case 7U: _LL25: _tmp8EF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8E4)->f2;_LL26: {
# 5269
struct Cyc_Absyn_Aggrfield*_tmp8E5=Cyc_Absyn_lookup_field(_tmp8EF,_tmp90B);
if(_tmp8E5 != 0  && _tmp8E5->width == 0){
struct _tuple17 _tmp8E6=Cyc_Tcutil_addressof_props(te,_tmp90C);struct _tuple17 _tmp8E7=_tmp8E6;int _tmp8E9;void*_tmp8E8;_LL2C: _tmp8E9=_tmp8E7.f1;_tmp8E8=_tmp8E7.f2;_LL2D:;
return({struct _tuple17 _tmpA04;_tmpA04.f1=(_tmp8E5->tq).real_const  || _tmp8E9,_tmpA04.f2=_tmp8E8;_tmpA04;});}
# 5274
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E4)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E4)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp8F0=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8E4)->f1)->f1).KnownAggr).val;_LL28: {
# 5276
struct Cyc_Absyn_Aggrfield*_tmp8EA=Cyc_Absyn_lookup_decl_field(_tmp8F0,_tmp90B);
if(_tmp8EA != 0  && _tmp8EA->width == 0){
struct _tuple17 _tmp8EB=Cyc_Tcutil_addressof_props(te,_tmp90C);struct _tuple17 _tmp8EC=_tmp8EB;int _tmp8EE;void*_tmp8ED;_LL2F: _tmp8EE=_tmp8EC.f1;_tmp8ED=_tmp8EC.f2;_LL30:;
return({struct _tuple17 _tmpA05;_tmpA05.f1=(_tmp8EA->tq).real_const  || _tmp8EE,_tmpA05.f2=_tmp8ED;_tmpA05;});}
# 5281
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp90F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8D9)->f1;_tmp90E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8D9)->f2;_tmp90D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8D9)->f3;_LL6:
# 5286
 if(_tmp90D)return bogus_ans;{
# 5289
void*_tmp8F1=Cyc_Tcutil_compress((void*)_check_null(_tmp90F->topt));void*_tmp8F2=_tmp8F1;void*_tmp8F8;void*_tmp8F7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F2)->tag == 3U){_LL32: _tmp8F8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F2)->f1).elt_type;_tmp8F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F2)->f1).ptr_atts).rgn;_LL33: {
# 5291
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp8F3=Cyc_Tcutil_compress(_tmp8F8);void*_tmp8F4=_tmp8F3;struct Cyc_Absyn_Aggrdecl*_tmp8F6;struct Cyc_List_List*_tmp8F5;switch(*((int*)_tmp8F4)){case 7U: _LL37: _tmp8F5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8F4)->f2;_LL38:
# 5294
 finfo=Cyc_Absyn_lookup_field(_tmp8F5,_tmp90E);goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F4)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F4)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp8F6=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8F4)->f1)->f1).KnownAggr).val;_LL3A:
# 5296
 finfo=Cyc_Absyn_lookup_decl_field(_tmp8F6,_tmp90E);goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5299
if(finfo != 0  && finfo->width == 0)
return({struct _tuple17 _tmpA06;_tmpA06.f1=(finfo->tq).real_const,_tmpA06.f2=_tmp8F7;_tmpA06;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp910=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8D9)->f1;_LL8: {
# 5306
void*_tmp8F9=Cyc_Tcutil_compress((void*)_check_null(_tmp910->topt));void*_tmp8FA=_tmp8F9;struct Cyc_Absyn_Tqual _tmp8FC;void*_tmp8FB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FA)->tag == 3U){_LL3E: _tmp8FC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FA)->f1).elt_tq;_tmp8FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FA)->f1).ptr_atts).rgn;_LL3F:
# 5308
 return({struct _tuple17 _tmpA07;_tmpA07.f1=_tmp8FC.real_const,_tmpA07.f2=_tmp8FB;_tmpA07;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp912=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8D9)->f1;_tmp911=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8D9)->f2;_LLA: {
# 5314
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp912->topt));
void*_tmp8FD=t;struct Cyc_Absyn_Tqual _tmp906;struct Cyc_Absyn_Tqual _tmp905;void*_tmp904;struct Cyc_List_List*_tmp903;switch(*((int*)_tmp8FD)){case 6U: _LL43: _tmp903=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8FD)->f1;_LL44: {
# 5318
struct _tuple13 _tmp8FE=Cyc_Evexp_eval_const_uint_exp(_tmp911);struct _tuple13 _tmp8FF=_tmp8FE;unsigned int _tmp902;int _tmp901;_LL4C: _tmp902=_tmp8FF.f1;_tmp901=_tmp8FF.f2;_LL4D:;
if(!_tmp901)
return bogus_ans;{
struct _tuple12*_tmp900=Cyc_Absyn_lookup_tuple_field(_tmp903,(int)_tmp902);
if(_tmp900 != 0)
return({struct _tuple17 _tmpA08;_tmpA08.f1=((*_tmp900).f1).real_const,({void*_tmpCA6=(Cyc_Tcutil_addressof_props(te,_tmp912)).f2;_tmpA08.f2=_tmpCA6;});_tmpA08;});
return bogus_ans;};}case 3U: _LL45: _tmp905=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FD)->f1).elt_tq;_tmp904=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FD)->f1).ptr_atts).rgn;_LL46:
# 5326
 return({struct _tuple17 _tmpA09;_tmpA09.f1=_tmp905.real_const,_tmpA09.f2=_tmp904;_tmpA09;});case 4U: _LL47: _tmp906=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8FD)->f1).tq;_LL48:
# 5332
 return({struct _tuple17 _tmpA0A;_tmpA0A.f1=_tmp906.real_const,({void*_tmpCA7=(Cyc_Tcutil_addressof_props(te,_tmp912)).f2;_tmpA0A.f2=_tmpCA7;});_tmpA0A;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5336
({void*_tmp907=0U;({unsigned int _tmpCA9=e->loc;struct _dyneither_ptr _tmpCA8=({const char*_tmp908="unary & applied to non-lvalue";_tag_dyneither(_tmp908,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpCA9,_tmpCA8,_tag_dyneither(_tmp907,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5343
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp913=({void*_tmpCAA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCAA,b);});
if(_tmp913 == 0)return;{
struct Cyc_Absyn_Exp*_tmp914=_tmp913;
struct _tuple13 _tmp915=Cyc_Evexp_eval_const_uint_exp(_tmp914);struct _tuple13 _tmp916=_tmp915;unsigned int _tmp920;int _tmp91F;_LL1: _tmp920=_tmp916.f1;_tmp91F=_tmp916.f2;_LL2:;
if(_tmp91F  && _tmp920 <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp919=({struct Cyc_Int_pa_PrintArg_struct _tmpA0D;_tmpA0D.tag=1U,_tmpA0D.f1=(unsigned long)((int)_tmp920);_tmpA0D;});struct Cyc_Int_pa_PrintArg_struct _tmp91A=({struct Cyc_Int_pa_PrintArg_struct _tmpA0C;_tmpA0C.tag=1U,_tmpA0C.f1=(unsigned long)((int)i);_tmpA0C;});void*_tmp917[2U];_tmp917[0]=& _tmp919,_tmp917[1]=& _tmp91A;({unsigned int _tmpCAC=loc;struct _dyneither_ptr _tmpCAB=({const char*_tmp918="a dereference will be out of bounds: %d <= %d";_tag_dyneither(_tmp918,sizeof(char),46U);});Cyc_Tcutil_warn(_tmpCAC,_tmpCAB,_tag_dyneither(_tmp917,sizeof(void*),2U));});});else{
# 5352
({struct Cyc_Int_pa_PrintArg_struct _tmp91D=({struct Cyc_Int_pa_PrintArg_struct _tmpA0F;_tmpA0F.tag=1U,_tmpA0F.f1=(unsigned long)((int)_tmp920);_tmpA0F;});struct Cyc_Int_pa_PrintArg_struct _tmp91E=({struct Cyc_Int_pa_PrintArg_struct _tmpA0E;_tmpA0E.tag=1U,_tmpA0E.f1=(unsigned long)((int)i);_tmpA0E;});void*_tmp91B[2U];_tmp91B[0]=& _tmp91D,_tmp91B[1]=& _tmp91E;({unsigned int _tmpCAE=loc;struct _dyneither_ptr _tmpCAD=({const char*_tmp91C="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp91C,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCAE,_tmpCAD,_tag_dyneither(_tmp91B,sizeof(void*),2U));});});}}
return;};}
# 5356
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 5365
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp921=e->r;void*_tmp922=_tmp921;struct Cyc_List_List*_tmp941;struct Cyc_List_List*_tmp940;enum Cyc_Absyn_Primop _tmp93F;struct Cyc_List_List*_tmp93E;struct Cyc_List_List*_tmp93D;struct Cyc_List_List*_tmp93C;struct Cyc_List_List*_tmp93B;struct Cyc_Absyn_Exp*_tmp93A;struct Cyc_Absyn_Exp*_tmp939;struct Cyc_Absyn_Exp*_tmp938;struct Cyc_Absyn_Exp*_tmp937;void*_tmp936;struct Cyc_Absyn_Exp*_tmp935;void*_tmp934;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Exp*_tmp932;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_Absyn_Exp*_tmp930;struct Cyc_Absyn_Exp*_tmp92F;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_Absyn_Exp*_tmp92D;struct Cyc_Absyn_Exp*_tmp92C;struct Cyc_Absyn_Exp*_tmp92B;void*_tmp92A;switch(*((int*)_tmp922)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp92A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL10: {
# 5377
void*_tmp923=_tmp92A;struct Cyc_Absyn_Vardecl*_tmp929;struct Cyc_Absyn_Vardecl*_tmp928;switch(*((int*)_tmp923)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp928=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp923)->f1;_LL37: {
# 5380
void*_tmp924=Cyc_Tcutil_compress(_tmp928->type);void*_tmp925=_tmp924;switch(*((int*)_tmp925)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp929=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp923)->f1;_LL39:
# 5387
 if((int)_tmp929->sc == (int)Cyc_Absyn_Static){
void*_tmp926=Cyc_Tcutil_compress(_tmp929->type);void*_tmp927=_tmp926;switch(*((int*)_tmp927)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 5394
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp92D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_tmp92C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp922)->f2;_tmp92B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp922)->f3;_LL12:
# 5400
 return(Cyc_Tcutil_cnst_exp(0,_tmp92D) && 
Cyc_Tcutil_cnst_exp(0,_tmp92C)) && 
Cyc_Tcutil_cnst_exp(0,_tmp92B);case 9U: _LL13: _tmp92F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_tmp92E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp922)->f2;_LL14:
# 5404
 return Cyc_Tcutil_cnst_exp(0,_tmp92F) && Cyc_Tcutil_cnst_exp(0,_tmp92E);case 41U: _LL15: _tmp930=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL16:
 _tmp931=_tmp930;goto _LL18;case 12U: _LL17: _tmp931=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL18:
 _tmp932=_tmp931;goto _LL1A;case 13U: _LL19: _tmp932=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL1A:
# 5408
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp932);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp922)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp934=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_tmp933=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp922)->f2;_LL1C:
# 5410
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp933);}else{_LL1D: _tmp936=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_tmp935=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp922)->f2;_LL1E:
# 5413
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp935);}case 15U: _LL1F: _tmp937=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL20:
# 5415
 return Cyc_Tcutil_cnst_exp(1,_tmp937);case 27U: _LL21: _tmp939=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp922)->f2;_tmp938=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp922)->f3;_LL22:
# 5417
 return Cyc_Tcutil_cnst_exp(0,_tmp939) && Cyc_Tcutil_cnst_exp(0,_tmp938);case 28U: _LL23: _tmp93A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL24:
# 5419
 return Cyc_Tcutil_cnst_exp(0,_tmp93A);case 26U: _LL25: _tmp93B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL26:
 _tmp93C=_tmp93B;goto _LL28;case 30U: _LL27: _tmp93C=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp922)->f2;_LL28:
 _tmp93D=_tmp93C;goto _LL2A;case 29U: _LL29: _tmp93D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp922)->f3;_LL2A:
# 5423
 for(0;_tmp93D != 0;_tmp93D=_tmp93D->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple18*)_tmp93D->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp93F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_tmp93E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp922)->f2;_LL2C:
# 5428
 _tmp940=_tmp93E;goto _LL2E;case 24U: _LL2D: _tmp940=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL2E:
 _tmp941=_tmp940;goto _LL30;case 31U: _LL2F: _tmp941=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp922)->f1;_LL30:
# 5431
 for(0;_tmp941 != 0;_tmp941=_tmp941->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp941->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 5438
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5442
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp942=Cyc_Tcutil_compress(t);void*_tmp943=_tmp942;struct Cyc_List_List*_tmp953;struct Cyc_List_List*_tmp952;void*_tmp951;void*_tmp950;void*_tmp94F;struct Cyc_List_List*_tmp94E;switch(*((int*)_tmp943)){case 0U: _LL1: _tmp94F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp943)->f1;_tmp94E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp943)->f2;_LL2: {
# 5446
void*_tmp944=_tmp94F;union Cyc_Absyn_AggrInfo _tmp94D;struct Cyc_List_List*_tmp94C;struct Cyc_Absyn_Enumdecl*_tmp94B;switch(*((int*)_tmp944)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp94B=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp944)->f2;_LL15:
# 5451
 if(_tmp94B == 0  || _tmp94B->fields == 0)
return 0;
_tmp94C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp94B->fields))->v;goto _LL17;case 16U: _LL16: _tmp94C=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp944)->f1;_LL17:
# 5455
{struct Cyc_List_List*_tmp945=_tmp94C;for(0;_tmp945 != 0;_tmp945=_tmp945->tl){
if(((struct Cyc_Absyn_Enumfield*)_tmp945->hd)->tag == 0)
return _tmp945 == _tmp94C;{
struct _tuple13 _tmp946=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_tmp945->hd)->tag));struct _tuple13 _tmp947=_tmp946;unsigned int _tmp949;int _tmp948;_LL1D: _tmp949=_tmp947.f1;_tmp948=_tmp947.f2;_LL1E:;
if(_tmp948  && _tmp949 == (unsigned int)0)
return 1;};}}
# 5462
return 0;case 20U: _LL18: _tmp94D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp944)->f1;_LL19: {
# 5465
struct Cyc_Absyn_Aggrdecl*_tmp94A=Cyc_Absyn_get_known_aggrdecl(_tmp94D);
if(_tmp94A->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp94A->impl))->exist_vars != 0)return 0;
if((int)_tmp94A->kind == (int)Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp94A->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(_tmp94A->tvs,_tmp94E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp94A->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp950=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp943)->f1).ptr_atts).nullable;_LL4:
# 5473
 return Cyc_Tcutil_force_type2bool(1,_tmp950);case 4U: _LL5: _tmp951=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp943)->f1).elt_type;_LL6:
 return Cyc_Tcutil_zeroable_type(_tmp951);case 6U: _LL7: _tmp952=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp943)->f1;_LL8:
# 5476
 for(0;_tmp952 != 0;_tmp952=_tmp952->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmp952->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp953=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp943)->f2;_LLA:
 return Cyc_Tcutil_fields_zeroable(0,0,_tmp953);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 5483
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5485
struct _RegionHandle _tmp954=_new_region("rgn");struct _RegionHandle*rgn=& _tmp954;_push_region(rgn);
{struct Cyc_List_List*_tmp955=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,_tmp955,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp956=0;_npop_handler(0U);return _tmp956;}}{
# 5493
int _tmp957=1;_npop_handler(0U);return _tmp957;};}
# 5486
;_pop_region(rgn);}
# 5497
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp958=t;struct Cyc_Absyn_Typedefdecl*_tmp95D;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp958)->tag == 8U){_LL1: _tmp95D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp958)->f3;_LL2:
# 5500
 if(_tmp95D != 0){
struct Cyc_Absyn_Tqual _tmp959=_tmp95D->tq;
if(((_tmp959.print_const  || _tmp959.q_volatile) || _tmp959.q_restrict) || _tmp959.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp95C=({struct Cyc_String_pa_PrintArg_struct _tmpA10;_tmpA10.tag=0U,({struct _dyneither_ptr _tmpCAF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA10.f1=_tmpCAF;});_tmpA10;});void*_tmp95A[1U];_tmp95A[0]=& _tmp95C;({unsigned int _tmpCB1=loc;struct _dyneither_ptr _tmpCB0=({const char*_tmp95B="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp95B,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpCB1,_tmpCB0,_tag_dyneither(_tmp95A,sizeof(void*),1U));});});}
# 5505
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5512
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp95E=Cyc_Tcutil_compress(t);void*_tmp95F=_tmp95E;struct Cyc_List_List**_tmp965;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95F)->tag == 5U){_LL1: _tmp965=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95F)->f1).attributes;_LL2: {
# 5515
struct Cyc_List_List*_tmp960=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmp965,(void*)atts->hd))
({struct Cyc_List_List*_tmpCB2=({struct Cyc_List_List*_tmp961=_cycalloc(sizeof(*_tmp961));_tmp961->hd=(void*)atts->hd,_tmp961->tl=*_tmp965;_tmp961;});*_tmp965=_tmpCB2;});}else{
# 5522
_tmp960=({struct Cyc_List_List*_tmp962=_cycalloc(sizeof(*_tmp962));_tmp962->hd=(void*)atts->hd,_tmp962->tl=_tmp960;_tmp962;});}}
return _tmp960;}}else{_LL3: _LL4:
({void*_tmp963=0U;({struct _dyneither_ptr _tmpCB3=({const char*_tmp964="transfer_fn_type_atts";_tag_dyneither(_tmp964,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB3,_tag_dyneither(_tmp963,sizeof(void*),0U));});});}_LL0:;}
# 5529
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp966=Cyc_Tcutil_compress(t);void*_tmp967=_tmp966;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_PtrInfo _tmp968;switch(*((int*)_tmp967)){case 3U: _LL1: _tmp968=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp967)->f1;_LL2:
 return({void*_tmpCB4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCB4,(_tmp968.ptr_atts).bounds);});case 4U: _LL3: _tmp969=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp967)->f1).num_elts;_LL4:
 return _tmp969;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5539
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmp96A=b;struct Cyc_Absyn_Vardecl*_tmp96E;struct Cyc_Absyn_Vardecl*_tmp96D;struct Cyc_Absyn_Vardecl*_tmp96C;struct Cyc_Absyn_Vardecl*_tmp96B;switch(*((int*)_tmp96A)){case 5U: _LL1: _tmp96B=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp96A)->f1;_LL2:
 _tmp96C=_tmp96B;goto _LL4;case 4U: _LL3: _tmp96C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp96A)->f1;_LL4:
 _tmp96D=_tmp96C;goto _LL6;case 3U: _LL5: _tmp96D=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp96A)->f1;_LL6:
 _tmp96E=_tmp96D;goto _LL8;case 1U: _LL7: _tmp96E=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp96A)->f1;_LL8:
# 5545
 if(!_tmp96E->escapes)return _tmp96E;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5549
return 0;}
# 5553
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp96F=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmp96F=({struct Cyc_List_List*_tmp970=_cycalloc(sizeof(*_tmp970));_tmp970->hd=*((void**)_check_null((void**)x->hd)),_tmp970->tl=_tmp96F;_tmp970;});}}
return _tmp96F;}
# 5560
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp971=Cyc_Tcutil_compress(t);void*_tmp972=_tmp971;void*_tmp97C;struct Cyc_Absyn_Tqual _tmp97B;struct Cyc_Absyn_Exp*_tmp97A;void*_tmp979;unsigned int _tmp978;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp972)->tag == 4U){_LL1: _tmp97C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp972)->f1).elt_type;_tmp97B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp972)->f1).tq;_tmp97A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp972)->f1).num_elts;_tmp979=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp972)->f1).zero_term;_tmp978=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp972)->f1).zt_loc;_LL2: {
# 5563
void*b;
if(_tmp97A == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 5567
struct Cyc_Absyn_Exp*bnd=_tmp97A;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmp973=0U;({struct _dyneither_ptr _tmpCB5=({const char*_tmp974="cannot convert tag without type!";_tag_dyneither(_tmp974,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB5,_tag_dyneither(_tmp973,sizeof(void*),0U));});});{
void*_tmp975=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp976=_tmp975;void*_tmp977;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp976)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp976)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp976)->f2 != 0){_LL6: _tmp977=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp976)->f2)->hd;_LL7:
# 5573
 b=Cyc_Absyn_thin_bounds_exp(({void*_tmpCB6=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmpCB6,Cyc_Absyn_valueof_exp(_tmp977,0U),0,Cyc_Absyn_No_coercion,0U);}));
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 5576
 if(Cyc_Tcutil_is_const_exp(bnd))
b=Cyc_Absyn_thin_bounds_exp(bnd);else{
# 5579
b=Cyc_Absyn_fat_bound_type;}}_LL5:;};}else{
# 5583
b=Cyc_Absyn_thin_bounds_exp(bnd);}}
# 5585
return Cyc_Absyn_atb_type(_tmp97C,rgn,_tmp97B,b,_tmp979);}}else{_LL3: _LL4:
 return t;}_LL0:;}
