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
# 111 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 325
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 509
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 911 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 927
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 929
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 932
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 934
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 936
extern void*Cyc_Absyn_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned int i);
# 939
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 943
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 945
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 962
void*Cyc_Absyn_exn_type();
# 970
extern void*Cyc_Absyn_fat_bound_type;
# 972
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 974
void*Cyc_Absyn_bounds_one();
# 981
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 984
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 987
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 992
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 1000
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1004
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1079
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1155
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1157
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1160
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1162
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 76 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 93
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 120
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 124
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 142
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 144
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 149
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 46
int Cyc_Tcutil_is_arithmetic_type(void*);
# 49
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 57
int Cyc_Tcutil_is_bound_one(void*b);
# 59
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 62
int Cyc_Tcutil_is_bits_only_type(void*t);
# 69
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 71
void*Cyc_Tcutil_pointer_region(void*t);
# 74
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 77
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 82
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 84
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 90
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 94
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 102
void*Cyc_Tcutil_copy_type(void*t);
# 105
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 108
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 112
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
# 115
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 125
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 127
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 133
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 152
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 154
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 161
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 170
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 173
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 189
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 191
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 195
void Cyc_Tcutil_explain_failure();
# 197
int Cyc_Tcutil_unify(void*,void*);
# 202
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 205
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 226
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 231
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 273 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 283
void Cyc_Tcutil_check_nonzero_bound(unsigned int,void*);
# 285
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,void*,int do_warn);
# 289
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 302
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 305
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 310
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 315
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple14{int f1;void*f2;};
# 319
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 345
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 370
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 381
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 384
int Cyc_Tcutil_zeroable_type(void*t);
# 388
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 391
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 393
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**te);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,int isCproto,unsigned int);
# 33
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,int isCproto,unsigned int);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 47 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 51
({void*_tmp0=0U;({unsigned int _tmp556=loc;struct _dyneither_ptr _tmp555=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp556,_tmp555,_tag_dyneither(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 55
return*topt;}}
# 64
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 68
if(topt == 0){
# 70
({void*_tmp557=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=26U,_tmp1->f1=des;_tmp1;});e->r=_tmp557;});
return;}{
# 73
void*t=*topt;
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;void*_tmpE;struct Cyc_Absyn_Tqual _tmpD;union Cyc_Absyn_AggrInfo _tmpC;switch(*((int*)_tmp3)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->tag == 20U){_LL1: _tmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3)->f1)->f1;_LL2:
# 76
{union Cyc_Absyn_AggrInfo _tmp4=_tmpC;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp4.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp558=({const char*_tmp6="struct type not properly set";_tag_dyneither(_tmp6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp558,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp4.KnownAggr).val;_LLD:
({void*_tmp559=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=29U,_tmp7->f1=_tmp8->name,_tmp7->f2=0,_tmp7->f3=des,_tmp7->f4=_tmp8;_tmp7;});e->r=_tmp559;});}_LL9:;}
# 80
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4:
({void*_tmp55A=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=26U,_tmp9->f1=des;_tmp9;});e->r=_tmp55A;});goto _LL0;case 7U: _LL5: _LL6:
({void*_tmp55B=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=30U,_tmpA->f1=t,_tmpA->f2=des;_tmpA;});e->r=_tmp55B;});goto _LL0;default: _LL7: _LL8:
({void*_tmp55C=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp55C;});goto _LL0;}_LL0:;};}
# 90
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 94
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 100
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_tmp10=_tmpF;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->f2 == 0){_LL1: _LL2:
# 103
 if(Cyc_Tc_aggressive_warn)({void*_tmp11=0U;({unsigned int _tmp55E=e->loc;struct _dyneither_ptr _tmp55D=({const char*_tmp12="assignment in test";_tag_dyneither(_tmp12,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp55E,_tmp55D,_tag_dyneither(_tmp11,sizeof(void*),0U));});});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 110
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp4D2;_tmp4D2.tag=0U,_tmp4D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);_tmp4D2;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp4D1;_tmp4D1.tag=0U,({
struct _dyneither_ptr _tmp55F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4D1.f1=_tmp55F;});_tmp4D1;});void*_tmp13[2U];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16;({unsigned int _tmp561=e->loc;struct _dyneither_ptr _tmp560=({const char*_tmp14="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp14,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp561,_tmp560,_tag_dyneither(_tmp13,sizeof(void*),2U));});});}
# 136 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 141
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp17=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp18=_tmp17;struct _dyneither_ptr _tmp4B;struct _dyneither_ptr _tmp4A;enum Cyc_Absyn_Sign _tmp49;int _tmp48;int _tmp47;enum Cyc_Absyn_Sign _tmp46;enum Cyc_Absyn_Sign _tmp45;switch((_tmp18.Wstring_c).tag){case 2U: switch(((_tmp18.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_type;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_type;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_type;goto _LL0;}case 3U: _LL7: _LL8:
 t=Cyc_Absyn_wchar_type();goto _LL0;case 4U: _LL9: _tmp45=((_tmp18.Short_c).val).f1;_LLA:
# 151
 t=_tmp45 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;goto _LL0;case 6U: _LLB: _tmp46=((_tmp18.LongLong_c).val).f1;_LLC:
# 153
 t=_tmp46 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_type: Cyc_Absyn_slonglong_type;goto _LL0;case 7U: _LLD: _tmp47=((_tmp18.Float_c).val).f2;_LLE:
# 155
 if(topt == 0)t=Cyc_Absyn_gen_float_type((unsigned int)_tmp47);else{
# 159
void*_tmp19=Cyc_Tcutil_compress(*topt);void*_tmp1A=_tmp19;int _tmp1B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL18: _tmp1B=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->f1;_LL19:
 t=Cyc_Absyn_gen_float_type((unsigned int)_tmp1B);goto _LL17;}else{goto _LL1A;}}else{_LL1A: _LL1B:
# 163
 t=Cyc_Absyn_gen_float_type((unsigned int)_tmp47);
goto _LL17;}_LL17:;}
# 167
goto _LL0;case 5U: _LLF: _tmp49=((_tmp18.Int_c).val).f1;_tmp48=((_tmp18.Int_c).val).f2;_LL10:
# 169
 if(topt == 0)
t=_tmp49 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;else{
# 176
void*_tmp1C=Cyc_Tcutil_compress(*topt);void*_tmp1D=_tmp1C;void*_tmp30;struct Cyc_Absyn_Tqual _tmp2F;void*_tmp2E;void*_tmp2D;void*_tmp2C;void*_tmp2B;void*_tmp2A;enum Cyc_Absyn_Sign _tmp29;enum Cyc_Absyn_Sign _tmp28;enum Cyc_Absyn_Sign _tmp27;enum Cyc_Absyn_Sign _tmp26;switch(*((int*)_tmp1D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f2){case Cyc_Absyn_Char_sz: _LL1D: _tmp26=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL1E:
# 178
{enum Cyc_Absyn_Sign _tmp1E=_tmp26;switch(_tmp1E){case Cyc_Absyn_Unsigned: _LL2C: _LL2D:
 t=Cyc_Absyn_uchar_type;goto _LL2B;case Cyc_Absyn_Signed: _LL2E: _LL2F:
 t=Cyc_Absyn_schar_type;goto _LL2B;default: _LL30: _LL31:
 t=Cyc_Absyn_char_type;goto _LL2B;}_LL2B:;}
# 183
({union Cyc_Absyn_Cnst _tmp562=Cyc_Absyn_Char_c(_tmp26,(char)_tmp48);*c=_tmp562;});
goto _LL1C;case Cyc_Absyn_Short_sz: _LL1F: _tmp27=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL20:
# 186
 t=_tmp27 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;
({union Cyc_Absyn_Cnst _tmp563=Cyc_Absyn_Short_c(_tmp27,(short)_tmp48);*c=_tmp563;});
goto _LL1C;case Cyc_Absyn_Int_sz: _LL21: _tmp28=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL22:
# 190
 t=_tmp28 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 193
({union Cyc_Absyn_Cnst _tmp564=Cyc_Absyn_Int_c(_tmp28,_tmp48);*c=_tmp564;});
goto _LL1C;case Cyc_Absyn_Long_sz: _LL23: _tmp29=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f1)->f1;_LL24:
# 196
 t=_tmp29 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
# 199
({union Cyc_Absyn_Cnst _tmp565=Cyc_Absyn_Int_c(_tmp29,_tmp48);*c=_tmp565;});
goto _LL1C;default: goto _LL29;}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f2 != 0){_LL27: _tmp2A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D)->f2)->hd;_LL28: {
# 213
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp24=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->tag=9U,({struct Cyc_Absyn_Exp*_tmp566=Cyc_Absyn_uint_exp((unsigned int)_tmp48,0U);_tmp25->f1=_tmp566;});_tmp25;});
# 220
t=Cyc_Absyn_tag_type((void*)_tmp24);
goto _LL1C;}}else{goto _LL29;}default: goto _LL29;}case 3U: _LL25: _tmp30=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).elt_type;_tmp2F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).elt_tq;_tmp2E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).rgn;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).nullable;_tmp2C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).bounds;_tmp2B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D)->f1).ptr_atts).zero_term;if(_tmp48 == 0){_LL26: {
# 203
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(Cyc_Tcutil_force_type2bool(1,_tmp2D))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp4D3;_tmp4D3.tag=0U,({
struct _dyneither_ptr _tmp567=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D3.f1=_tmp567;});_tmp4D3;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({unsigned int _tmp569=e->loc;struct _dyneither_ptr _tmp568=({const char*_tmp20="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp20,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp569,_tmp568,_tag_dyneither(_tmp1F,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp22=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23->tag=3U,(_tmp23->f1).elt_type=_tmp30,(_tmp23->f1).elt_tq=_tmp2F,
((_tmp23->f1).ptr_atts).rgn=_tmp2E,((_tmp23->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp23->f1).ptr_atts).bounds=_tmp2C,((_tmp23->f1).ptr_atts).zero_term=_tmp2B,((_tmp23->f1).ptr_atts).ptrloc=0;_tmp23;});
goto _LL1C;};}}else{goto _LL29;}default: _LL29: _LL2A:
# 223
 t=_tmp49 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;
goto _LL1C;}_LL1C:;}
# 226
goto _LL0;case 8U: _LL11: _tmp4A=(_tmp18.String_c).val;_LL12:
# 228
 string_numelts=(int)_get_dyneither_size(_tmp4A,sizeof(char));
_tmp4B=_tmp4A;goto _LL14;case 9U: _LL13: _tmp4B=(_tmp18.Wstring_c).val;_LL14:
# 231
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp4B);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 235
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp56A=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp56A,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 240
void*_tmp31=Cyc_Absyn_thin_bounds_exp(elen);
t=({void*_tmp56E=string_elt_typ;void*_tmp56D=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp56C=Cyc_Absyn_const_tqual(0U);void*_tmp56B=_tmp31;Cyc_Absyn_atb_type(_tmp56E,_tmp56D,_tmp56C,_tmp56B,Cyc_Absyn_true_type);});
# 243
if(topt != 0){
void*_tmp32=Cyc_Tcutil_compress(*topt);void*_tmp33=_tmp32;struct Cyc_Absyn_Tqual _tmp34;switch(*((int*)_tmp33)){case 4U: _LL33: _tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33)->f1).tq;_LL34:
# 249
 return({void*_tmp571=string_elt_typ;struct Cyc_Absyn_Tqual _tmp570=_tmp34;struct Cyc_Absyn_Exp*_tmp56F=elen;Cyc_Absyn_array_type(_tmp571,_tmp570,_tmp56F,Cyc_Tcutil_any_bool(& te),0U);});case 3U: _LL35: _LL36:
# 251
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 257
t=({void*_tmp575=string_elt_typ;void*_tmp574=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp573=
Cyc_Absyn_const_tqual(0U);
# 257
void*_tmp572=_tmp31;Cyc_Absyn_atb_type(_tmp575,_tmp574,_tmp573,_tmp572,Cyc_Absyn_true_type);});
# 259
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 265
goto _LL32;default: _LL37: _LL38:
 goto _LL32;}_LL32:;}
# 269
return t;};};default: _LL15: _LL16:
# 271
 if(topt != 0){
void*_tmp35=Cyc_Tcutil_compress(*topt);void*_tmp36=_tmp35;void*_tmp40;struct Cyc_Absyn_Tqual _tmp3F;void*_tmp3E;void*_tmp3D;void*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->tag == 3U){_LL3A: _tmp40=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).elt_type;_tmp3F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).elt_tq;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).rgn;_tmp3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).nullable;_tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).bounds;_tmp3B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36)->f1).ptr_atts).zero_term;_LL3B:
# 275
 if(Cyc_Tcutil_force_type2bool(1,_tmp3D))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp4D4;_tmp4D4.tag=0U,({
struct _dyneither_ptr _tmp576=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4D4.f1=_tmp576;});_tmp4D4;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({unsigned int _tmp578=e->loc;struct _dyneither_ptr _tmp577=({const char*_tmp38="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp38,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp578,_tmp577,_tag_dyneither(_tmp37,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->tag=3U,(_tmp3A->f1).elt_type=_tmp40,(_tmp3A->f1).elt_tq=_tmp3F,((_tmp3A->f1).ptr_atts).rgn=_tmp3E,((_tmp3A->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp3A->f1).ptr_atts).bounds=_tmp3C,((_tmp3A->f1).ptr_atts).zero_term=_tmp3B,((_tmp3A->f1).ptr_atts).ptrloc=0;_tmp3A;});}else{_LL3C: _LL3D:
# 280
 goto _LL39;}_LL39:;}{
# 283
struct Cyc_List_List*_tmp41=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=3U,({void*_tmp57D=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=_tmp41;_tmp42;}));(_tmp44->f1).elt_type=_tmp57D;}),({
struct Cyc_Absyn_Tqual _tmp57C=Cyc_Absyn_empty_tqual(0U);(_tmp44->f1).elt_tq=_tmp57C;}),
({void*_tmp57B=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->v=_tmp41;_tmp43;}));((_tmp44->f1).ptr_atts).rgn=_tmp57B;}),((_tmp44->f1).ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp57A=Cyc_Tcutil_any_bounds(& te);((_tmp44->f1).ptr_atts).bounds=_tmp57A;}),({
void*_tmp579=Cyc_Tcutil_any_bool(& te);((_tmp44->f1).ptr_atts).zero_term=_tmp579;}),((_tmp44->f1).ptr_atts).ptrloc=0;_tmp44;});
# 290
goto _LL0;};}_LL0:;}
# 292
return t;}
# 296
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 301
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp4C=*((void**)_check_null(b));void*_tmp4D=_tmp4C;struct Cyc_Absyn_Vardecl*_tmp59;struct Cyc_Absyn_Vardecl*_tmp58;struct Cyc_Absyn_Vardecl*_tmp57;struct Cyc_Absyn_Fndecl*_tmp56;struct Cyc_Absyn_Vardecl*_tmp55;struct _tuple1*_tmp54;switch(*((int*)_tmp4D)){case 0U: _LL1: _tmp54=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL2:
# 304
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp57E=({const char*_tmp4E="unresolved binding in tcVar";_tag_dyneither(_tmp4E,sizeof(char),28U);});_tmp4F->f1=_tmp57E;});_tmp4F;}));case 1U: _LL3: _tmp55=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL4:
# 308
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp55->name,1);
return _tmp55->type;case 2U: _LL5: _tmp56=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL6:
# 315
 if(_tmp56->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp56->name,1);
return Cyc_Tcutil_fndecl2type(_tmp56);case 5U: _LL7: _tmp57=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp4D)->f1;_LL8:
 _tmp58=_tmp57;goto _LLA;case 4U: _LL9: _tmp58=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLA:
 _tmp59=_tmp58;goto _LLC;default: _LLB: _tmp59=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp4D)->f1;_LLC:
# 321
 if(te->allow_valueof){
void*_tmp50=Cyc_Tcutil_compress(_tmp59->type);void*_tmp51=_tmp50;void*_tmp53;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f2 != 0){_LLE: _tmp53=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp51)->f2)->hd;_LLF:
# 324
({void*_tmp57F=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=39U,_tmp52->f1=_tmp53;_tmp52;});e->r=_tmp57F;});
goto _LLD;}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 329
return _tmp59->type;}_LL0:;}
# 333
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,int,unsigned int)){
# 341
struct Cyc_List_List*desc_types;
{void*_tmp5A=fmt->r;void*_tmp5B=_tmp5A;struct _dyneither_ptr _tmp60;struct _dyneither_ptr _tmp5F;switch(*((int*)_tmp5B)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).tag == 8){_LL1: _tmp5F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B)->f1).String_c).val;_LL2:
 _tmp60=_tmp5F;goto _LL4;}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp60=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B)->f2)->r)->f1).String_c).val;_LL4:
# 345
 desc_types=type_getter(te,(struct _dyneither_ptr)_tmp60,isCproto,fmt->loc);goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 349
 if(opt_args != 0){
struct Cyc_List_List*_tmp5C=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp5C != 0;_tmp5C=_tmp5C->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp5C->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp5C->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp5C->hd))
({void*_tmp5D=0U;({unsigned int _tmp581=((struct Cyc_Absyn_Exp*)_tmp5C->hd)->loc;struct _dyneither_ptr _tmp580=({const char*_tmp5E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp5E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp581,_tmp580,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});}}
# 358
return;}_LL0:;}
# 360
if(opt_args != 0){
struct Cyc_List_List*_tmp61=(struct Cyc_List_List*)opt_args->v;
# 363
for(0;desc_types != 0  && _tmp61 != 0;(
desc_types=desc_types->tl,_tmp61=_tmp61->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp61->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp4D6;_tmp4D6.tag=0U,({
struct _dyneither_ptr _tmp582=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4D6.f1=_tmp582;});_tmp4D6;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp4D5;_tmp4D5.tag=0U,({struct _dyneither_ptr _tmp583=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4D5.f1=_tmp583;});_tmp4D5;});void*_tmp62[2U];_tmp62[0]=& _tmp64,_tmp62[1]=& _tmp65;({unsigned int _tmp585=e->loc;struct _dyneither_ptr _tmp584=({const char*_tmp63="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp63,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp585,_tmp584,_tag_dyneither(_tmp62,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 374
if(alias_coercion)
({struct Cyc_List_List*_tmp586=({struct Cyc_List_List*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->hd=(void*)arg_cnt,_tmp66->tl=*alias_arg_exps;_tmp66;});*alias_arg_exps=_tmp586;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp67=0U;({unsigned int _tmp588=((struct Cyc_Absyn_Exp*)_tmp61->hd)->loc;struct _dyneither_ptr _tmp587=({const char*_tmp68="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp68,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp588,_tmp587,_tag_dyneither(_tmp67,sizeof(void*),0U));});});}
# 381
if(desc_types != 0)
({void*_tmp69=0U;({unsigned int _tmp58A=fmt->loc;struct _dyneither_ptr _tmp589=({const char*_tmp6A="too few arguments";_tag_dyneither(_tmp6A,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp58A,_tmp589,_tag_dyneither(_tmp69,sizeof(void*),0U));});});
if(_tmp61 != 0){
({void*_tmp6B=0U;({unsigned int _tmp58C=((struct Cyc_Absyn_Exp*)_tmp61->hd)->loc;struct _dyneither_ptr _tmp58B=({const char*_tmp6C="too many arguments";_tag_dyneither(_tmp6C,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp58C,_tmp58B,_tag_dyneither(_tmp6B,sizeof(void*),0U));});});
# 386
for(0;_tmp61 != 0;_tmp61=_tmp61->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp61->hd);}}}}
# 391
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 393
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp6D=p;switch(_tmp6D){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 397
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp4D7;_tmp4D7.tag=0U,({struct _dyneither_ptr _tmp58D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4D7.f1=_tmp58D;});_tmp4D7;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({unsigned int _tmp58F=loc;struct _dyneither_ptr _tmp58E=({const char*_tmp6F="expecting numeric type but found %s";_tag_dyneither(_tmp6F,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp58F,_tmp58E,_tag_dyneither(_tmp6E,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 401
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp4D8;_tmp4D8.tag=0U,({struct _dyneither_ptr _tmp590=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4D8.f1=_tmp590;});_tmp4D8;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({unsigned int _tmp592=loc;struct _dyneither_ptr _tmp591=({const char*_tmp72="expecting integral or * type but found %s";_tag_dyneither(_tmp72,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp592,_tmp591,_tag_dyneither(_tmp71,sizeof(void*),1U));});});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 406
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp76=({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0U,({struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4D9.f1=_tmp593;});_tmp4D9;});void*_tmp74[1U];_tmp74[0]=& _tmp76;({unsigned int _tmp595=loc;struct _dyneither_ptr _tmp594=({const char*_tmp75="expecting integral type but found %s";_tag_dyneither(_tmp75,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp595,_tmp594,_tag_dyneither(_tmp74,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 410
{void*_tmp77=t;void*_tmp7F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->tag == 3U){_LLE: _tmp7F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77)->f1).ptr_atts).bounds;_LLF: {
# 412
struct Cyc_Absyn_Exp*_tmp78=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp7F);
if(_tmp78 != 0){
struct Cyc_Absyn_Exp*_tmp79=_tmp78;
if(!Cyc_Evexp_c_can_eval(_tmp79) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp7A=0U;({unsigned int _tmp597=loc;struct _dyneither_ptr _tmp596=({const char*_tmp7B="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp7B,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp597,_tmp596,_tag_dyneither(_tmp7A,sizeof(void*),0U));});});}
# 418
goto _LLD;}}else{_LL10: _LL11:
# 420
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0U,({struct _dyneither_ptr _tmp598=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4DA.f1=_tmp598;});_tmp4DA;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp59A=loc;struct _dyneither_ptr _tmp599=({const char*_tmp7D="numelts requires pointer type, not %s";_tag_dyneither(_tmp7D,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp59A,_tmp599,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});}_LLD:;}
# 422
return Cyc_Absyn_uint_type;default: _LLB: _LLC:
({void*_tmp80=0U;({struct _dyneither_ptr _tmp59B=({const char*_tmp81="Non-unary primop";_tag_dyneither(_tmp81,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp59B,_tag_dyneither(_tmp80,sizeof(void*),0U));});});}_LL0:;}
# 428
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 431
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp84=({struct Cyc_String_pa_PrintArg_struct _tmp4DB;_tmp4DB.tag=0U,({struct _dyneither_ptr _tmp59C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4DB.f1=_tmp59C;});_tmp4DB;});void*_tmp82[1U];_tmp82[0]=& _tmp84;({unsigned int _tmp59E=e1->loc;struct _dyneither_ptr _tmp59D=({const char*_tmp83="type %s cannot be used here";_tag_dyneither(_tmp83,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp59E,_tmp59D,_tag_dyneither(_tmp82,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 435
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp4DC;_tmp4DC.tag=0U,({struct _dyneither_ptr _tmp59F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4DC.f1=_tmp59F;});_tmp4DC;});void*_tmp85[1U];_tmp85[0]=& _tmp87;({unsigned int _tmp5A1=e2->loc;struct _dyneither_ptr _tmp5A0=({const char*_tmp86="type %s cannot be used here";_tag_dyneither(_tmp86,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5A1,_tmp5A0,_tag_dyneither(_tmp85,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 439
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 444
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp88=t1;void*_tmp9F;struct Cyc_Absyn_Tqual _tmp9E;void*_tmp9D;void*_tmp9C;void*_tmp9B;void*_tmp9A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88)->tag == 3U){_LL1: _tmp9F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88)->f1).elt_type;_tmp9E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88)->f1).elt_tq;_tmp9D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88)->f1).ptr_atts).rgn;_tmp9C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88)->f1).ptr_atts).nullable;_tmp9B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88)->f1).ptr_atts).bounds;_tmp9A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp88)->f1).ptr_atts).zero_term;_LL2:
# 449
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmp9F),& Cyc_Tcutil_tmk))
({void*_tmp89=0U;({unsigned int _tmp5A3=e1->loc;struct _dyneither_ptr _tmp5A2=({const char*_tmp8A="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp8A,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5A3,_tmp5A2,_tag_dyneither(_tmp89,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp8B=0U;({unsigned int _tmp5A5=e1->loc;struct _dyneither_ptr _tmp5A4=({const char*_tmp8C="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp8C,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5A5,_tmp5A4,_tag_dyneither(_tmp8B,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmp8F=({struct Cyc_String_pa_PrintArg_struct _tmp4DD;_tmp4DD.tag=0U,({struct _dyneither_ptr _tmp5A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4DD.f1=_tmp5A6;});_tmp4DD;});void*_tmp8D[1U];_tmp8D[0]=& _tmp8F;({unsigned int _tmp5A8=e2->loc;struct _dyneither_ptr _tmp5A7=({const char*_tmp8E="expecting int but found %s";_tag_dyneither(_tmp8E,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5A8,_tmp5A7,_tag_dyneither(_tmp8D,sizeof(void*),1U));});});{
struct Cyc_Absyn_Exp*_tmp90=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp9B);
if(_tmp90 == 0)return t1;{
struct Cyc_Absyn_Exp*_tmp91=_tmp90;
# 460
if(Cyc_Tcutil_force_type2bool(0,_tmp9A)){
struct _tuple15 _tmp92=Cyc_Evexp_eval_const_uint_exp(_tmp91);struct _tuple15 _tmp93=_tmp92;unsigned int _tmp97;int _tmp96;_LL6: _tmp97=_tmp93.f1;_tmp96=_tmp93.f2;_LL7:;
if(_tmp96  && _tmp97 == 1)
({void*_tmp94=0U;({unsigned int _tmp5AA=e1->loc;struct _dyneither_ptr _tmp5A9=({const char*_tmp95="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmp95,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5AA,_tmp5A9,_tag_dyneither(_tmp94,sizeof(void*),0U));});});}{
# 471
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp98=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=3U,(_tmp99->f1).elt_type=_tmp9F,(_tmp99->f1).elt_tq=_tmp9E,
((_tmp99->f1).ptr_atts).rgn=_tmp9D,((_tmp99->f1).ptr_atts).nullable=Cyc_Absyn_true_type,((_tmp99->f1).ptr_atts).bounds=Cyc_Absyn_fat_bound_type,((_tmp99->f1).ptr_atts).zero_term=_tmp9A,((_tmp99->f1).ptr_atts).ptrloc=0;_tmp99;});
# 475
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp98,Cyc_Absyn_Other_coercion);
return(void*)_tmp98;};};};}else{_LL3: _LL4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 482
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpA2=({struct Cyc_String_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=0U,({
# 492
struct _dyneither_ptr _tmp5AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4DF.f1=_tmp5AB;});_tmp4DF;});struct Cyc_String_pa_PrintArg_struct _tmpA3=({struct Cyc_String_pa_PrintArg_struct _tmp4DE;_tmp4DE.tag=0U,({struct _dyneither_ptr _tmp5AC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4DE.f1=_tmp5AC;});_tmp4DE;});void*_tmpA0[2U];_tmpA0[0]=& _tmpA2,_tmpA0[1]=& _tmpA3;({unsigned int _tmp5AE=e1->loc;struct _dyneither_ptr _tmp5AD=({const char*_tmpA1="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpA1,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5AE,_tmp5AD,_tag_dyneither(_tmpA0,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 495
return Cyc_Absyn_sint_type;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp5AF=t1_elt;Cyc_Tcutil_unify(_tmp5AF,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp4E1;_tmp4E1.tag=0U,({
# 500
struct _dyneither_ptr _tmp5B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E1.f1=_tmp5B0;});_tmp4E1;});struct Cyc_String_pa_PrintArg_struct _tmpA7=({struct Cyc_String_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=0U,({struct _dyneither_ptr _tmp5B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E0.f1=_tmp5B1;});_tmp4E0;});void*_tmpA4[2U];_tmpA4[0]=& _tmpA6,_tmpA4[1]=& _tmpA7;({unsigned int _tmp5B3=e1->loc;struct _dyneither_ptr _tmp5B2=({const char*_tmpA5="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpA5,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5B3,_tmp5B2,_tag_dyneither(_tmpA4,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 504
({void*_tmpA8=0U;({unsigned int _tmp5B5=e1->loc;struct _dyneither_ptr _tmp5B4=({const char*_tmpA9="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpA9,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5B5,_tmp5B4,_tag_dyneither(_tmpA8,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5BA=te;struct Cyc_Absyn_Exp*_tmp5B9=e1;Cyc_Tcutil_unchecked_cast(_tmp5BA,_tmp5B9,({void*_tmp5B8=t1_elt;void*_tmp5B7=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5B6=
Cyc_Absyn_empty_tqual(0U);
# 505
Cyc_Absyn_star_type(_tmp5B8,_tmp5B7,_tmp5B6,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});
# 508
return Cyc_Absyn_sint_type;}else{
# 510
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpAA=0U;({unsigned int _tmp5BC=e1->loc;struct _dyneither_ptr _tmp5BB=({const char*_tmpAB="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpAB,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5BC,_tmp5BB,_tag_dyneither(_tmpAA,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpAC=0U;({unsigned int _tmp5BE=e1->loc;struct _dyneither_ptr _tmp5BD=({const char*_tmpAD="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpAD,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5BE,_tmp5BD,_tag_dyneither(_tmpAC,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_type(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp4E3;_tmp4E3.tag=0U,({
struct _dyneither_ptr _tmp5BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E3.f1=_tmp5BF;});_tmp4E3;});struct Cyc_String_pa_PrintArg_struct _tmpB1=({struct Cyc_String_pa_PrintArg_struct _tmp4E2;_tmp4E2.tag=0U,({struct _dyneither_ptr _tmp5C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E2.f1=_tmp5C0;});_tmp4E2;});void*_tmpAE[2U];_tmpAE[0]=& _tmpB0,_tmpAE[1]=& _tmpB1;({unsigned int _tmp5C2=e2->loc;struct _dyneither_ptr _tmp5C1=({const char*_tmpAF="expecting either %s or int but found %s";_tag_dyneither(_tmpAF,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5C2,_tmp5C1,_tag_dyneither(_tmpAE,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 519
return t1;}}}
# 523
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5C3=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5C3,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
({void*_tmpB2=0U;({unsigned int _tmp5C5=e1->loc;struct _dyneither_ptr _tmp5C4=({const char*_tmpB3="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpB3,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5C5,_tmp5C4,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5CA=te;struct Cyc_Absyn_Exp*_tmp5C9=e2;Cyc_Tcutil_unchecked_cast(_tmp5CA,_tmp5C9,({void*_tmp5C8=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5C7=
Cyc_Tcutil_pointer_region(t2);
# 528
struct Cyc_Absyn_Tqual _tmp5C6=
# 530
Cyc_Absyn_empty_tqual(0U);
# 528
Cyc_Absyn_star_type(_tmp5C8,_tmp5C7,_tmp5C6,Cyc_Absyn_false_type);}),Cyc_Absyn_Other_coercion);});}
# 533
({void*_tmpB4=0U;({unsigned int _tmp5CC=e1->loc;struct _dyneither_ptr _tmp5CB=({const char*_tmpB5="thin pointer subtraction!";_tag_dyneither(_tmpB5,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5CC,_tmp5CB,_tag_dyneither(_tmpB4,sizeof(void*),0U));});});
return Cyc_Absyn_sint_type;}
# 536
({void*_tmpB6=0U;({unsigned int _tmp5CE=e1->loc;struct _dyneither_ptr _tmp5CD=({const char*_tmpB7="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpB7,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5CE,_tmp5CD,_tag_dyneither(_tmpB6,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 539
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpB8=0U;({unsigned int _tmp5D0=e1->loc;struct _dyneither_ptr _tmp5CF=({const char*_tmpB9="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpB9,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp5D0,_tmp5CF,_tag_dyneither(_tmpB8,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_type,Cyc_Absyn_Other_coercion);}
# 544
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 547
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_type;else{
# 555
if((Cyc_Tcutil_type_kind(t1))->kind == Cyc_Absyn_BoxKind  || ({
void*_tmp5D1=t1;Cyc_Tcutil_unify(_tmp5D1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_type;else{
# 560
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_type;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_type;else{
goto pointer_cmp;}}}}}else{
# 573
pointer_cmp: {
struct _tuple0 _tmpBA=({struct _tuple0 _tmp4E4;({void*_tmp5D3=Cyc_Tcutil_compress(t1);_tmp4E4.f1=_tmp5D3;}),({void*_tmp5D2=Cyc_Tcutil_compress(t2);_tmp4E4.f2=_tmp5D2;});_tmp4E4;});struct _tuple0 _tmpBB=_tmpBA;void*_tmpBD;void*_tmpBC;switch(*((int*)_tmpBB.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBB.f2)->tag == 3U){_LL1: _tmpBD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBB.f1)->f1).elt_type;_tmpBC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBB.f2)->f1).elt_type;_LL2:
# 577
 if(Cyc_Tcutil_unify(_tmpBD,_tmpBC))return Cyc_Absyn_sint_type;goto _LL0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB.f1)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB.f2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB.f2)->f1)->tag == 3U){_LL3: _LL4:
# 579
 return Cyc_Absyn_sint_type;}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 582
({struct Cyc_String_pa_PrintArg_struct _tmpC0=({struct Cyc_String_pa_PrintArg_struct _tmp4E6;_tmp4E6.tag=0U,({
struct _dyneither_ptr _tmp5D4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4E6.f1=_tmp5D4;});_tmp4E6;});struct Cyc_String_pa_PrintArg_struct _tmpC1=({struct Cyc_String_pa_PrintArg_struct _tmp4E5;_tmp4E5.tag=0U,({struct _dyneither_ptr _tmp5D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4E5.f1=_tmp5D5;});_tmp4E5;});void*_tmpBE[2U];_tmpBE[0]=& _tmpC0,_tmpBE[1]=& _tmpC1;({unsigned int _tmp5D7=loc;struct _dyneither_ptr _tmp5D6=({const char*_tmpBF="comparison not allowed between %s and %s";_tag_dyneither(_tmpBF,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5D7,_tmp5D6,_tag_dyneither(_tmpBE,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 589
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 591
if(({void*_tmp5D8=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5D8 == Cyc_Absyn_sint_type;}))
return Cyc_Absyn_sint_type;
({struct Cyc_String_pa_PrintArg_struct _tmpC4=({struct Cyc_String_pa_PrintArg_struct _tmp4E8;_tmp4E8.tag=0U,({
struct _dyneither_ptr _tmp5D9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4E8.f1=_tmp5D9;});_tmp4E8;});struct Cyc_String_pa_PrintArg_struct _tmpC5=({struct Cyc_String_pa_PrintArg_struct _tmp4E7;_tmp4E7.tag=0U,({struct _dyneither_ptr _tmp5DA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4E7.f1=_tmp5DA;});_tmp4E7;});void*_tmpC2[2U];_tmpC2[0]=& _tmpC4,_tmpC2[1]=& _tmpC5;({unsigned int _tmp5DC=loc;struct _dyneither_ptr _tmp5DB=({const char*_tmpC3="comparison not allowed between %s and %s";_tag_dyneither(_tmpC3,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5DC,_tmp5DB,_tag_dyneither(_tmpC2,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 601
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 603
enum Cyc_Absyn_Primop _tmpC6=p;switch(_tmpC6){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 607
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 610
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Bitarshift: _LL15: _LL16:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL17: _LL18:
# 620
 goto _LL1A;case Cyc_Absyn_Neq: _LL19: _LL1A:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LL1B: _LL1C:
# 623
 goto _LL1E;case Cyc_Absyn_Lt: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Gte: _LL1F: _LL20:
 goto _LL22;case Cyc_Absyn_Lte: _LL21: _LL22:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LL23: _LL24:
# 628
({void*_tmpC7=0U;({struct _dyneither_ptr _tmp5DD=({const char*_tmpC8="bad binary primop";_tag_dyneither(_tmpC8,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5DD,_tag_dyneither(_tmpC7,sizeof(void*),0U));});});}_LL0:;}
# 632
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 640
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpC9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpCA=Cyc_Tcexp_tcExp(te,topt,_tmpC9);
if(!Cyc_Tcutil_is_numeric(_tmpC9))
({struct Cyc_String_pa_PrintArg_struct _tmpCD=({struct Cyc_String_pa_PrintArg_struct _tmp4E9;_tmp4E9.tag=0U,({struct _dyneither_ptr _tmp5DE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpCA));_tmp4E9.f1=_tmp5DE;});_tmp4E9;});void*_tmpCB[1U];_tmpCB[0]=& _tmpCD;({unsigned int _tmp5E0=_tmpC9->loc;struct _dyneither_ptr _tmp5DF=({const char*_tmpCC="expecting numeric type but found %s";_tag_dyneither(_tmpCC,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5E0,_tmp5DF,_tag_dyneither(_tmpCB,sizeof(void*),1U));});});
return _tmpCA;}
# 647
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpCE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpCF=_tmpCE;switch(_tmpCF){case 0U: _LL1: _LL2:
 return({void*_tmpD0=0U;({struct Cyc_Tcenv_Tenv*_tmp5E4=te;unsigned int _tmp5E3=loc;void**_tmp5E2=topt;struct _dyneither_ptr _tmp5E1=({const char*_tmpD1="primitive operator has 0 arguments";_tag_dyneither(_tmpD1,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp5E4,_tmp5E3,_tmp5E2,_tmp5E1,_tag_dyneither(_tmpD0,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LL0;case 2U: _LL5: _LL6:
 t=({struct Cyc_Tcenv_Tenv*_tmp5E9=te;unsigned int _tmp5E8=loc;void**_tmp5E7=topt;enum Cyc_Absyn_Primop _tmp5E6=p;struct Cyc_Absyn_Exp*_tmp5E5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5E9,_tmp5E8,_tmp5E7,_tmp5E6,_tmp5E5,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpD2=0U;({struct Cyc_Tcenv_Tenv*_tmp5ED=te;unsigned int _tmp5EC=loc;void**_tmp5EB=topt;struct _dyneither_ptr _tmp5EA=({const char*_tmpD3="primitive operator has > 2 arguments";_tag_dyneither(_tmpD3,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp5ED,_tmp5EC,_tmp5EB,_tmp5EA,_tag_dyneither(_tmpD2,sizeof(void*),0U));});});}_LL0:;}
# 655
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 658
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpD4=Cyc_Tcutil_compress(t);
void*_tmpD5=_tmpD4;struct Cyc_List_List*_tmpF1;void*_tmpF0;struct Cyc_Absyn_Tqual _tmpEF;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Datatypefield*_tmpED;struct Cyc_Absyn_Aggrdecl*_tmpEC;switch(*((int*)_tmpD5)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD5)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD5)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmpEC=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD5)->f1)->f1).KnownAggr).val;_LL2:
# 662
 if(_tmpEC->impl == 0){
({void*_tmpD6=0U;({unsigned int _tmp5EF=loc;struct _dyneither_ptr _tmp5EE=({const char*_tmpD7="attempt to write an abstract aggregate";_tag_dyneither(_tmpD7,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp5EF,_tmp5EE,_tag_dyneither(_tmpD6,sizeof(void*),0U));});});
return 0;}else{
# 666
_tmpEE=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEC->impl))->fields;goto _LL4;}}else{goto _LLB;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD5)->f1)->f1).KnownDatatypefield).tag == 2){_LL5: _tmpED=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD5)->f1)->f1).KnownDatatypefield).val).f2;_LL6:
# 678
{struct Cyc_List_List*_tmpDC=_tmpED->typs;for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){
struct _tuple17*_tmpDD=(struct _tuple17*)_tmpDC->hd;struct _tuple17*_tmpDE=_tmpDD;struct Cyc_Absyn_Tqual _tmpE3;void*_tmpE2;_LLE: _tmpE3=_tmpDE->f1;_tmpE2=_tmpDE->f2;_LLF:;
if(_tmpE3.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpE1=({struct Cyc_String_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=0U,({struct _dyneither_ptr _tmp5F0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpED->name));_tmp4EA.f1=_tmp5F0;});_tmp4EA;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({unsigned int _tmp5F2=loc;struct _dyneither_ptr _tmp5F1=({const char*_tmpE0="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpE0,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp5F2,_tmp5F1,_tag_dyneither(_tmpDF,sizeof(void*),1U));});});
return 0;}
# 684
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpE2))return 0;}}
# 686
return 1;}else{goto _LLB;}default: goto _LLB;}case 7U: _LL3: _tmpEE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD5)->f2;_LL4:
# 668
 for(0;_tmpEE != 0;_tmpEE=_tmpEE->tl){
struct Cyc_Absyn_Aggrfield*_tmpD8=(struct Cyc_Absyn_Aggrfield*)_tmpEE->hd;
if((_tmpD8->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp4EB;_tmp4EB.tag=0U,_tmp4EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD8->name);_tmp4EB;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({unsigned int _tmp5F4=loc;struct _dyneither_ptr _tmp5F3=({const char*_tmpDA="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpDA,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp5F4,_tmp5F3,_tag_dyneither(_tmpD9,sizeof(void*),1U));});});
return 0;}
# 674
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpD8->type))return 0;}
# 676
return 1;case 4U: _LL7: _tmpF0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD5)->f1).elt_type;_tmpEF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD5)->f1).tq;_LL8:
# 688
 if(_tmpEF.real_const){
({void*_tmpE4=0U;({unsigned int _tmp5F6=loc;struct _dyneither_ptr _tmp5F5=({const char*_tmpE5="attempt to over-write a const array";_tag_dyneither(_tmpE5,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5F6,_tmp5F5,_tag_dyneither(_tmpE4,sizeof(void*),0U));});});
return 0;}
# 692
return Cyc_Tcexp_check_writable_aggr(loc,_tmpF0);case 6U: _LL9: _tmpF1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD5)->f1;_LLA:
# 694
 for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
struct _tuple17*_tmpE6=(struct _tuple17*)_tmpF1->hd;struct _tuple17*_tmpE7=_tmpE6;struct Cyc_Absyn_Tqual _tmpEB;void*_tmpEA;_LL11: _tmpEB=_tmpE7->f1;_tmpEA=_tmpE7->f2;_LL12:;
if(_tmpEB.real_const){
({void*_tmpE8=0U;({unsigned int _tmp5F8=loc;struct _dyneither_ptr _tmp5F7=({const char*_tmpE9="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmpE9,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp5F8,_tmp5F7,_tag_dyneither(_tmpE8,sizeof(void*),0U));});});
return 0;}
# 700
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpEA))return 0;}
# 702
return 1;default: _LLB: _LLC:
 return 1;}_LL0:;}
# 710
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 713
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmpF2=e->r;void*_tmpF3=_tmpF2;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct _dyneither_ptr*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct _dyneither_ptr*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Vardecl*_tmp11A;struct Cyc_Absyn_Vardecl*_tmp119;struct Cyc_Absyn_Vardecl*_tmp118;struct Cyc_Absyn_Vardecl*_tmp117;switch(*((int*)_tmpF3)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF3)->f1)){case 3U: _LL1: _tmp117=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF3)->f1)->f1;_LL2:
 _tmp118=_tmp117;goto _LL4;case 4U: _LL3: _tmp118=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF3)->f1)->f1;_LL4:
 _tmp119=_tmp118;goto _LL6;case 5U: _LL5: _tmp119=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF3)->f1)->f1;_LL6:
 _tmp11A=_tmp119;goto _LL8;case 1U: _LL7: _tmp11A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpF3)->f1)->f1;_LL8:
 if(!(_tmp11A->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp11C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF3)->f1;_tmp11B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF3)->f2;_LLA:
# 720
{void*_tmpF4=Cyc_Tcutil_compress((void*)_check_null(_tmp11C->topt));void*_tmpF5=_tmpF4;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Tqual _tmp105;struct Cyc_Absyn_Tqual _tmp104;switch(*((int*)_tmpF5)){case 3U: _LL18: _tmp104=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF5)->f1).elt_tq;_LL19:
 _tmp105=_tmp104;goto _LL1B;case 4U: _LL1A: _tmp105=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF5)->f1).tq;_LL1B:
 if(!_tmp105.real_const)return;goto _LL17;case 6U: _LL1C: _tmp106=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF5)->f1;_LL1D: {
# 724
struct _tuple15 _tmpF6=Cyc_Evexp_eval_const_uint_exp(_tmp11B);struct _tuple15 _tmpF7=_tmpF6;unsigned int _tmp103;int _tmp102;_LL21: _tmp103=_tmpF7.f1;_tmp102=_tmpF7.f2;_LL22:;
if(!_tmp102){
({void*_tmpF8=0U;({unsigned int _tmp5FA=e->loc;struct _dyneither_ptr _tmp5F9=({const char*_tmpF9="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmpF9,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp5FA,_tmp5F9,_tag_dyneither(_tmpF8,sizeof(void*),0U));});});
return;}
# 729
{struct _handler_cons _tmpFA;_push_handler(& _tmpFA);{int _tmpFC=0;if(setjmp(_tmpFA.handler))_tmpFC=1;if(!_tmpFC){
{struct _tuple17*_tmpFD=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp106,(int)_tmp103);struct _tuple17*_tmpFE=_tmpFD;struct Cyc_Absyn_Tqual _tmpFF;_LL24: _tmpFF=_tmpFE->f1;_LL25:;
if(!_tmpFF.real_const){_npop_handler(0U);return;}}
# 730
;_pop_handler();}else{void*_tmpFB=(void*)_exn_thrown;void*_tmp100=_tmpFB;void*_tmp101;if(((struct Cyc_List_Nth_exn_struct*)_tmp100)->tag == Cyc_List_Nth){_LL27: _LL28:
# 732
 return;}else{_LL29: _tmp101=_tmp100;_LL2A:(int)_rethrow(_tmp101);}_LL26:;}};}
goto _LL17;}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 736
goto _LL0;case 21U: _LLB: _tmp11E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF3)->f1;_tmp11D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpF3)->f2;_LLC:
# 738
{void*_tmp107=Cyc_Tcutil_compress((void*)_check_null(_tmp11E->topt));void*_tmp108=_tmp107;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Aggrdecl**_tmp109;switch(*((int*)_tmp108)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp108)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp108)->f1)->f1).KnownAggr).tag == 2){_LL2C: _tmp109=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp108)->f1)->f1).KnownAggr).val;_LL2D: {
# 740
struct Cyc_Absyn_Aggrfield*sf=
_tmp109 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp109,_tmp11D);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}}else{goto _LL30;}case 7U: _LL2E: _tmp10A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp108)->f2;_LL2F: {
# 745
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp10A,_tmp11D);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 750
goto _LL0;case 22U: _LLD: _tmp120=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF3)->f1;_tmp11F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpF3)->f2;_LLE:
# 752
{void*_tmp10B=Cyc_Tcutil_compress((void*)_check_null(_tmp120->topt));void*_tmp10C=_tmp10B;void*_tmp112;struct Cyc_Absyn_Tqual _tmp111;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10C)->tag == 3U){_LL33: _tmp112=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10C)->f1).elt_type;_tmp111=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10C)->f1).elt_tq;_LL34:
# 754
 if(!_tmp111.real_const){
void*_tmp10D=Cyc_Tcutil_compress(_tmp112);void*_tmp10E=_tmp10D;struct Cyc_List_List*_tmp110;struct Cyc_Absyn_Aggrdecl**_tmp10F;switch(*((int*)_tmp10E)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).KnownAggr).tag == 2){_LL38: _tmp10F=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10E)->f1)->f1).KnownAggr).val;_LL39: {
# 757
struct Cyc_Absyn_Aggrfield*sf=
_tmp10F == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp10F,_tmp11F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}}else{goto _LL3C;}case 7U: _LL3A: _tmp110=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10E)->f2;_LL3B: {
# 762
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp110,_tmp11F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 768
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 771
goto _LL0;case 20U: _LLF: _tmp121=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpF3)->f1;_LL10:
# 773
{void*_tmp113=Cyc_Tcutil_compress((void*)_check_null(_tmp121->topt));void*_tmp114=_tmp113;struct Cyc_Absyn_Tqual _tmp116;struct Cyc_Absyn_Tqual _tmp115;switch(*((int*)_tmp114)){case 3U: _LL3F: _tmp115=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp114)->f1).elt_tq;_LL40:
 _tmp116=_tmp115;goto _LL42;case 4U: _LL41: _tmp116=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp114)->f1).tq;_LL42:
 if(!_tmp116.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 778
goto _LL0;case 12U: _LL11: _tmp122=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpF3)->f1;_LL12:
 _tmp123=_tmp122;goto _LL14;case 13U: _LL13: _tmp123=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpF3)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp123);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 783
({struct Cyc_String_pa_PrintArg_struct _tmp126=({struct Cyc_String_pa_PrintArg_struct _tmp4EC;_tmp4EC.tag=0U,({struct _dyneither_ptr _tmp5FB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4EC.f1=_tmp5FB;});_tmp4EC;});void*_tmp124[1U];_tmp124[0]=& _tmp126;({unsigned int _tmp5FD=e->loc;struct _dyneither_ptr _tmp5FC=({const char*_tmp125="attempt to write a const location: %s";_tag_dyneither(_tmp125,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp5FD,_tmp5FC,_tag_dyneither(_tmp124,sizeof(void*),1U));});});}
# 786
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 789
({struct Cyc_Tcenv_Tenv*_tmp5FE=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp5FE,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp127=0U;({struct Cyc_Tcenv_Tenv*_tmp602=te;unsigned int _tmp601=loc;void**_tmp600=topt;struct _dyneither_ptr _tmp5FF=({const char*_tmp128="increment/decrement of non-lvalue";_tag_dyneither(_tmp128,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp602,_tmp601,_tmp600,_tmp5FF,_tag_dyneither(_tmp127,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 795
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp129=0U;({unsigned int _tmp604=e->loc;struct _dyneither_ptr _tmp603=({const char*_tmp12A="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp12A,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp604,_tmp603,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp12B=0U;({unsigned int _tmp606=e->loc;struct _dyneither_ptr _tmp605=({const char*_tmp12C="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp12C,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp606,_tmp605,_tag_dyneither(_tmp12B,sizeof(void*),0U));});});}else{
# 804
({struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=0U,({struct _dyneither_ptr _tmp607=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4ED.f1=_tmp607;});_tmp4ED;});void*_tmp12D[1U];_tmp12D[0]=& _tmp12F;({unsigned int _tmp609=e->loc;struct _dyneither_ptr _tmp608=({const char*_tmp12E="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp12E,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp609,_tmp608,_tag_dyneither(_tmp12D,sizeof(void*),1U));});});}}
# 806
return t;};}
# 810
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 812
({struct Cyc_Tcenv_Tenv*_tmp60B=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp60A=e1;Cyc_Tcexp_tcTest(_tmp60B,_tmp60A,({const char*_tmp130="conditional expression";_tag_dyneither(_tmp130,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 819
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp131=({struct Cyc_List_List _tmp4F1;_tmp4F1.hd=e3,_tmp4F1.tl=0;_tmp4F1;});
struct Cyc_List_List _tmp132=({struct Cyc_List_List _tmp4F0;_tmp4F0.hd=e2,_tmp4F0.tl=& _tmp131;_tmp4F0;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp132)){
({struct Cyc_String_pa_PrintArg_struct _tmp135=({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U,({
struct _dyneither_ptr _tmp60C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4EF.f1=_tmp60C;});_tmp4EF;});struct Cyc_String_pa_PrintArg_struct _tmp136=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,({struct _dyneither_ptr _tmp60D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp4EE.f1=_tmp60D;});_tmp4EE;});void*_tmp133[2U];_tmp133[0]=& _tmp135,_tmp133[1]=& _tmp136;({unsigned int _tmp60F=loc;struct _dyneither_ptr _tmp60E=({const char*_tmp134="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp134,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp60F,_tmp60E,_tag_dyneither(_tmp133,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 827
return t;};};}
# 831
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 833
({struct Cyc_Tcenv_Tenv*_tmp611=te;struct Cyc_Absyn_Exp*_tmp610=e1;Cyc_Tcexp_tcTest(_tmp611,_tmp610,({const char*_tmp137="logical-and expression";_tag_dyneither(_tmp137,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp613=te;struct Cyc_Absyn_Exp*_tmp612=e2;Cyc_Tcexp_tcTest(_tmp613,_tmp612,({const char*_tmp138="logical-and expression";_tag_dyneither(_tmp138,sizeof(char),23U);}));});
return Cyc_Absyn_sint_type;}
# 839
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 841
({struct Cyc_Tcenv_Tenv*_tmp615=te;struct Cyc_Absyn_Exp*_tmp614=e1;Cyc_Tcexp_tcTest(_tmp615,_tmp614,({const char*_tmp139="logical-or expression";_tag_dyneither(_tmp139,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp617=te;struct Cyc_Absyn_Exp*_tmp616=e2;Cyc_Tcexp_tcTest(_tmp617,_tmp616,({const char*_tmp13A="logical-or expression";_tag_dyneither(_tmp13A,sizeof(char),22U);}));});
return Cyc_Absyn_sint_type;}
# 847
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 854
({struct Cyc_Tcenv_Tenv*_tmp618=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp618,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 859
{void*_tmp13B=Cyc_Tcutil_compress(t1);void*_tmp13C=_tmp13B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->tag == 4U){_LL1: _LL2:
({void*_tmp13D=0U;({unsigned int _tmp61A=loc;struct _dyneither_ptr _tmp619=({const char*_tmp13E="cannot assign to an array";_tag_dyneither(_tmp13E,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp61A,_tmp619,_tag_dyneither(_tmp13D,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 864
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp13F=0U;({unsigned int _tmp61C=loc;struct _dyneither_ptr _tmp61B=({const char*_tmp140="type is abstract (can't determine size).";_tag_dyneither(_tmp140,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp61C,_tmp61B,_tag_dyneither(_tmp13F,sizeof(void*),0U));});});
# 868
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp141=0U;({struct Cyc_Tcenv_Tenv*_tmp620=te;unsigned int _tmp61F=loc;void**_tmp61E=topt;struct _dyneither_ptr _tmp61D=({const char*_tmp142="assignment to non-lvalue";_tag_dyneither(_tmp142,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp620,_tmp61F,_tmp61E,_tmp61D,_tag_dyneither(_tmp141,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp143=0U;({unsigned int _tmp622=e2->loc;struct _dyneither_ptr _tmp621=({const char*_tmp144="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp144,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp622,_tmp621,_tag_dyneither(_tmp143,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp145=({struct Cyc_String_pa_PrintArg_struct _tmp148=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({
struct _dyneither_ptr _tmp623=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F3.f1=_tmp623;});_tmp4F3;});struct Cyc_String_pa_PrintArg_struct _tmp149=({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0U,({struct _dyneither_ptr _tmp624=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F2.f1=_tmp624;});_tmp4F2;});void*_tmp146[2U];_tmp146[0]=& _tmp148,_tmp146[1]=& _tmp149;({struct Cyc_Tcenv_Tenv*_tmp628=te;unsigned int _tmp627=loc;void**_tmp626=topt;struct _dyneither_ptr _tmp625=({const char*_tmp147="type mismatch: %s != %s";_tag_dyneither(_tmp147,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp628,_tmp627,_tmp626,_tmp625,_tag_dyneither(_tmp146,sizeof(void*),2U));});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp145;}}else{
# 882
enum Cyc_Absyn_Primop _tmp14A=(enum Cyc_Absyn_Primop)po->v;
void*_tmp14B=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp14A,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp14B,t1) || Cyc_Tcutil_is_arithmetic_type(_tmp14B) && Cyc_Tcutil_is_arithmetic_type(t1))){
void*_tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp14F=({struct Cyc_String_pa_PrintArg_struct _tmp4F5;_tmp4F5.tag=0U,({
# 887
struct _dyneither_ptr _tmp629=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F5.f1=_tmp629;});_tmp4F5;});struct Cyc_String_pa_PrintArg_struct _tmp150=({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U,({
struct _dyneither_ptr _tmp62A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F4.f1=_tmp62A;});_tmp4F4;});void*_tmp14D[2U];_tmp14D[0]=& _tmp14F,_tmp14D[1]=& _tmp150;({struct Cyc_Tcenv_Tenv*_tmp62E=te;unsigned int _tmp62D=loc;void**_tmp62C=topt;struct _dyneither_ptr _tmp62B=({const char*_tmp14E="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp14E,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp62E,_tmp62D,_tmp62C,_tmp62B,_tag_dyneither(_tmp14D,sizeof(void*),2U));});});
Cyc_Tcutil_unify(_tmp14B,t1);
Cyc_Tcutil_explain_failure();
return _tmp14C;}
# 893
return _tmp14B;}
# 895
return t1;};};}
# 899
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp62F=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp62F,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp631=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp630=topt;Cyc_Tcexp_tcExp(_tmp631,_tmp630,e2);});
return(void*)_check_null(e2->topt);}
# 906
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 909
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 912
{void*_tmp151=Cyc_Tcutil_compress(t1);void*_tmp152=_tmp151;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp152)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp152)->f1)){case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp152)->f1)->f1 == 0){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_double_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp152)->f1)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}
# 919
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp153=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp154=_tmp153;struct _tuple1*_tmp158;struct Cyc_List_List*_tmp157;unsigned int _tmp156;enum Cyc_Absyn_Scope _tmp155;_LLA: _tmp158=_tmp154.name;_tmp157=_tmp154.typs;_tmp156=_tmp154.loc;_tmp155=_tmp154.sc;_LLB:;
# 922
if(_tmp157 == 0  || _tmp157->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp157->hd)).f2);
# 925
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 930
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp159=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp15A=_tmp159;struct _tuple1*_tmp15E;struct Cyc_List_List*_tmp15D;unsigned int _tmp15C;enum Cyc_Absyn_Scope _tmp15B;_LLD: _tmp15E=_tmp15A.name;_tmp15D=_tmp15A.typs;_tmp15C=_tmp15A.loc;_tmp15B=_tmp15A.sc;_LLE:;
# 933
if(_tmp15D == 0  || _tmp15D->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp15D->hd)).f2);
# 937
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 942
({struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U,({
struct _dyneither_ptr _tmp632=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp4F7.f1=_tmp632;});_tmp4F7;});struct Cyc_String_pa_PrintArg_struct _tmp162=({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0U,({struct _dyneither_ptr _tmp633=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F6.f1=_tmp633;});_tmp4F6;});void*_tmp15F[2U];_tmp15F[0]=& _tmp161,_tmp15F[1]=& _tmp162;({unsigned int _tmp635=e->loc;struct _dyneither_ptr _tmp634=({const char*_tmp160="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp160,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp635,_tmp634,_tag_dyneither(_tmp15F,sizeof(void*),2U));});});
return 0;}
# 948
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 954
struct Cyc_List_List*_tmp163=args;
int _tmp164=0;
struct Cyc_Tcenv_Tenv*_tmp165=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp166=Cyc_Tcenv_clear_abstract_val_ok(_tmp165);
Cyc_Tcexp_tcExp(_tmp166,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 963
void*_tmp167=t;void*_tmp1C6;struct Cyc_Absyn_Tqual _tmp1C5;void*_tmp1C4;void*_tmp1C3;void*_tmp1C2;void*_tmp1C1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->tag == 3U){_LL1: _tmp1C6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->f1).elt_type;_tmp1C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->f1).elt_tq;_tmp1C4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->f1).ptr_atts).rgn;_tmp1C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->f1).ptr_atts).nullable;_tmp1C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->f1).ptr_atts).bounds;_tmp1C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167)->f1).ptr_atts).zero_term;_LL2:
# 968
 Cyc_Tcenv_check_rgn_accessible(_tmp166,loc,_tmp1C4);
# 970
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1C2);{
void*_tmp168=Cyc_Tcutil_compress(_tmp1C6);void*_tmp169=_tmp168;struct Cyc_List_List*_tmp1BD;void*_tmp1BC;struct Cyc_Absyn_Tqual _tmp1BB;void*_tmp1BA;struct Cyc_List_List*_tmp1B9;int _tmp1B8;struct Cyc_Absyn_VarargInfo*_tmp1B7;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_List_List*_tmp1B1;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->tag == 5U){_LL6: _tmp1BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).tvars;_tmp1BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).effect;_tmp1BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).ret_tqual;_tmp1BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).ret_type;_tmp1B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).args;_tmp1B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).c_varargs;_tmp1B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).cyc_varargs;_tmp1B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).rgn_po;_tmp1B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).attributes;_tmp1B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).requires_clause;_tmp1B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).requires_relns;_tmp1B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).ensures_clause;_tmp1B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp169)->f1).ensures_relns;_LL7:
# 975
 if(_tmp1BD != 0  || _tmp1B6 != 0)
({void*_tmp16A=0U;({unsigned int _tmp637=e->loc;struct _dyneither_ptr _tmp636=({const char*_tmp16B="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp16B,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp637,_tmp636,_tag_dyneither(_tmp16A,sizeof(void*),0U));});});
# 979
if(topt != 0)Cyc_Tcutil_unify(_tmp1BA,*topt);
# 981
while(_tmp163 != 0  && _tmp1B9 != 0){
# 983
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp163->hd;
void*t2=(*((struct _tuple9*)_tmp1B9->hd)).f3;
Cyc_Tcexp_tcExp(_tmp166,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp166,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp17B="actual argument has type ";_tag_dyneither(_tmp17B,sizeof(char),26U);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp17A=" but formal has type ";_tag_dyneither(_tmp17A,sizeof(char),22U);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp63A=({unsigned long _tmp639=({unsigned long _tmp638=Cyc_strlen((struct _dyneither_ptr)s0);_tmp638 + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp639 + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp63A + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp16C=0U;({unsigned int _tmp63D=e1->loc;struct _dyneither_ptr _tmp63C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16F=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp4FB;});struct Cyc_String_pa_PrintArg_struct _tmp170=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,_tmp4FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp4FA;});struct Cyc_String_pa_PrintArg_struct _tmp171=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,_tmp4F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp4F9;});struct Cyc_String_pa_PrintArg_struct _tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U,_tmp4F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp4F8;});void*_tmp16D[4U];_tmp16D[0]=& _tmp16F,_tmp16D[1]=& _tmp170,_tmp16D[2]=& _tmp171,_tmp16D[3]=& _tmp172;({struct _dyneither_ptr _tmp63B=({const char*_tmp16E="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp16E,sizeof(char),15U);});Cyc_aprintf(_tmp63B,_tag_dyneither(_tmp16D,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp63D,_tmp63C,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});else{
# 995
({void*_tmp173=0U;({unsigned int _tmp640=e1->loc;struct _dyneither_ptr _tmp63F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp176=({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0U,_tmp4FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp4FF;});struct Cyc_String_pa_PrintArg_struct _tmp177=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,_tmp4FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp4FE;});struct Cyc_String_pa_PrintArg_struct _tmp178=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,_tmp4FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp4FD;});struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,_tmp4FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp4FC;});void*_tmp174[4U];_tmp174[0]=& _tmp176,_tmp174[1]=& _tmp177,_tmp174[2]=& _tmp178,_tmp174[3]=& _tmp179;({struct _dyneither_ptr _tmp63E=({const char*_tmp175="%s%s%s%s.";_tag_dyneither(_tmp175,sizeof(char),10U);});Cyc_aprintf(_tmp63E,_tag_dyneither(_tmp174,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp640,_tmp63F,_tag_dyneither(_tmp173,sizeof(void*),0U));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1000
if(alias_coercion)
({struct Cyc_List_List*_tmp641=({struct Cyc_List_List*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->hd=(void*)_tmp164,_tmp17C->tl=*alias_arg_exps;_tmp17C;});*alias_arg_exps=_tmp641;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp17D=0U;({unsigned int _tmp643=e1->loc;struct _dyneither_ptr _tmp642=({const char*_tmp17E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp17E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp643,_tmp642,_tag_dyneither(_tmp17D,sizeof(void*),0U));});});
_tmp163=_tmp163->tl;
_tmp1B9=_tmp1B9->tl;
++ _tmp164;}{
# 1011
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1B5;for(0;a != 0;a=a->tl){
void*_tmp17F=(void*)a->hd;void*_tmp180=_tmp17F;enum Cyc_Absyn_Format_Type _tmp18D;int _tmp18C;int _tmp18B;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp180)->tag == 19U){_LLB: _tmp18D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp180)->f1;_tmp18C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp180)->f2;_tmp18B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp180)->f3;_LLC:
# 1015
{struct _handler_cons _tmp181;_push_handler(& _tmp181);{int _tmp183=0;if(setjmp(_tmp181.handler))_tmp183=1;if(!_tmp183){
# 1017
{struct Cyc_Absyn_Exp*_tmp184=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp18C - 1);
# 1019
struct Cyc_Core_Opt*fmt_args;
if(_tmp18B == 0)
fmt_args=0;else{
# 1023
fmt_args=({struct Cyc_Core_Opt*_tmp185=_cycalloc(sizeof(*_tmp185));({struct Cyc_List_List*_tmp644=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp18B - 1);_tmp185->v=_tmp644;});_tmp185;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp186=_tmp18D;if(_tmp186 == Cyc_Absyn_Printf_ft){_LL10: _LL11:
# 1027
 Cyc_Tcexp_check_format_args(_tmp166,_tmp184,fmt_args,_tmp18B - 1,alias_arg_exps,_tmp1B8,Cyc_Formatstr_get_format_types);
# 1030
goto _LLF;}else{_LL12: _LL13:
# 1032
 Cyc_Tcexp_check_format_args(_tmp166,_tmp184,fmt_args,_tmp18B - 1,alias_arg_exps,_tmp1B8,Cyc_Formatstr_get_scanf_types);
# 1035
goto _LLF;}_LLF:;};}
# 1017
;_pop_handler();}else{void*_tmp182=(void*)_exn_thrown;void*_tmp187=_tmp182;void*_tmp18A;if(((struct Cyc_List_Nth_exn_struct*)_tmp187)->tag == Cyc_List_Nth){_LL15: _LL16:
# 1037
({void*_tmp188=0U;({unsigned int _tmp646=loc;struct _dyneither_ptr _tmp645=({const char*_tmp189="bad format arguments";_tag_dyneither(_tmp189,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp646,_tmp645,_tag_dyneither(_tmp188,sizeof(void*),0U));});});goto _LL14;}else{_LL17: _tmp18A=_tmp187;_LL18:(int)_rethrow(_tmp18A);}_LL14:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1042
if(_tmp1B9 != 0)({void*_tmp18E=0U;({unsigned int _tmp648=loc;struct _dyneither_ptr _tmp647=({const char*_tmp18F="too few arguments for function";_tag_dyneither(_tmp18F,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp648,_tmp647,_tag_dyneither(_tmp18E,sizeof(void*),0U));});});else{
# 1044
if((_tmp163 != 0  || _tmp1B8) || _tmp1B7 != 0){
if(_tmp1B8)
for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
Cyc_Tcexp_tcExp(_tmp166,0,(struct Cyc_Absyn_Exp*)_tmp163->hd);}else{
if(_tmp1B7 == 0)
({void*_tmp190=0U;({unsigned int _tmp64A=loc;struct _dyneither_ptr _tmp649=({const char*_tmp191="too many arguments for function";_tag_dyneither(_tmp191,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp64A,_tmp649,_tag_dyneither(_tmp190,sizeof(void*),0U));});});else{
# 1051
struct Cyc_Absyn_VarargInfo _tmp192=*_tmp1B7;struct Cyc_Absyn_VarargInfo _tmp193=_tmp192;void*_tmp1AD;int _tmp1AC;_LL1A: _tmp1AD=_tmp193.type;_tmp1AC=_tmp193.inject;_LL1B:;{
struct Cyc_Absyn_VarargCallInfo*_tmp194=({struct Cyc_Absyn_VarargCallInfo*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->num_varargs=0,_tmp1AB->injectors=0,_tmp1AB->vai=_tmp1B7;_tmp1AB;});
# 1055
*vararg_call_info=_tmp194;
# 1057
if(!_tmp1AC)
# 1059
for(0;_tmp163 != 0;(_tmp163=_tmp163->tl,_tmp164 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp163->hd;
++ _tmp194->num_varargs;
Cyc_Tcexp_tcExp(_tmp166,& _tmp1AD,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp166,e1,_tmp1AD,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp197=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,({
struct _dyneither_ptr _tmp64B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1AD));_tmp501.f1=_tmp64B;});_tmp501;});struct Cyc_String_pa_PrintArg_struct _tmp198=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,({struct _dyneither_ptr _tmp64C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp500.f1=_tmp64C;});_tmp500;});void*_tmp195[2U];_tmp195[0]=& _tmp197,_tmp195[1]=& _tmp198;({unsigned int _tmp64E=loc;struct _dyneither_ptr _tmp64D=({const char*_tmp196="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp196,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp64E,_tmp64D,_tag_dyneither(_tmp195,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 1069
if(alias_coercion)
({struct Cyc_List_List*_tmp64F=({struct Cyc_List_List*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->hd=(void*)_tmp164,_tmp199->tl=*alias_arg_exps;_tmp199;});*alias_arg_exps=_tmp64F;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1AD) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp19A=0U;({unsigned int _tmp651=e1->loc;struct _dyneither_ptr _tmp650=({const char*_tmp19B="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp19B,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp651,_tmp650,_tag_dyneither(_tmp19A,sizeof(void*),0U));});});}else{
# 1078
void*_tmp19C=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1AD));void*_tmp19D=_tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp1AA;struct Cyc_List_List*_tmp1A9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp1AA=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f1)->f1).KnownDatatype).val;_tmp1A9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2;_LL1E: {
# 1082
struct Cyc_Absyn_Datatypedecl*_tmp19E=*Cyc_Tcenv_lookup_datatypedecl(_tmp166,loc,_tmp1AA->name);
struct Cyc_List_List*fields=0;
if(_tmp19E->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1A1=({struct Cyc_String_pa_PrintArg_struct _tmp502;_tmp502.tag=0U,({
struct _dyneither_ptr _tmp652=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1AD));_tmp502.f1=_tmp652;});_tmp502;});void*_tmp19F[1U];_tmp19F[0]=& _tmp1A1;({unsigned int _tmp654=loc;struct _dyneither_ptr _tmp653=({const char*_tmp1A0="can't inject into abstract datatype %s";_tag_dyneither(_tmp1A0,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp654,_tmp653,_tag_dyneither(_tmp19F,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp19E->fields))->v;}
# 1093
({void*_tmp655=Cyc_Tcutil_pointer_region(_tmp1AD);Cyc_Tcutil_unify(_tmp655,Cyc_Tcenv_curr_rgn(_tmp166));});{
# 1095
struct Cyc_List_List*_tmp1A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp19E->tvs,_tmp1A9);
for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
++ _tmp194->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp163->hd;
# 1100
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp166,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1A3=0U;({unsigned int _tmp657=e1->loc;struct _dyneither_ptr _tmp656=({const char*_tmp1A4="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1A4,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp657,_tmp656,_tag_dyneither(_tmp1A3,sizeof(void*),0U));});});}{
# 1106
struct Cyc_Absyn_Datatypefield*_tmp1A5=({struct Cyc_Tcenv_Tenv*_tmp65B=_tmp166;struct Cyc_Absyn_Exp*_tmp65A=e1;void*_tmp659=Cyc_Tcutil_pointer_elt_type(_tmp1AD);struct Cyc_List_List*_tmp658=_tmp1A2;Cyc_Tcexp_tcInjection(_tmp65B,_tmp65A,_tmp659,_tmp658,fields);});
if(_tmp1A5 != 0)
({struct Cyc_List_List*_tmp65D=({
struct Cyc_List_List*_tmp65C=_tmp194->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp65C,({struct Cyc_List_List*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));
_tmp1A6->hd=_tmp1A5,_tmp1A6->tl=0;_tmp1A6;}));});
# 1108
_tmp194->injectors=_tmp65D;});};};}
# 1112
goto _LL1C;};}}else{goto _LL1F;}}else{goto _LL1F;}}else{_LL1F: _LL20:
({void*_tmp1A7=0U;({unsigned int _tmp65F=loc;struct _dyneither_ptr _tmp65E=({const char*_tmp1A8="bad inject vararg type";_tag_dyneither(_tmp1A8,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp65F,_tmp65E,_tag_dyneither(_tmp1A7,sizeof(void*),0U));});});goto _LL1C;}_LL1C:;}};}}}}
# 1118
if(*alias_arg_exps == 0)
# 1127 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp166,loc,(void*)_check_null(_tmp1BC));
# 1129
return _tmp1BA;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1B0=({struct Cyc_String_pa_PrintArg_struct _tmp503;_tmp503.tag=0U,({struct _dyneither_ptr _tmp660=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp503.f1=_tmp660;});_tmp503;});void*_tmp1AE[1U];_tmp1AE[0]=& _tmp1B0;({struct Cyc_Tcenv_Tenv*_tmp664=_tmp166;unsigned int _tmp663=loc;void**_tmp662=topt;struct _dyneither_ptr _tmp661=({const char*_tmp1AF="expected pointer to function but found %s";_tag_dyneither(_tmp1AF,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp664,_tmp663,_tmp662,_tmp661,_tag_dyneither(_tmp1AE,sizeof(void*),1U));});});}_LL5:;};}else{_LL3: _LL4:
# 1132
 return({struct Cyc_String_pa_PrintArg_struct _tmp1C0=({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U,({struct _dyneither_ptr _tmp665=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp504.f1=_tmp665;});_tmp504;});void*_tmp1BE[1U];_tmp1BE[0]=& _tmp1C0;({struct Cyc_Tcenv_Tenv*_tmp669=_tmp166;unsigned int _tmp668=loc;void**_tmp667=topt;struct _dyneither_ptr _tmp666=({const char*_tmp1BF="expected pointer to function but found %s";_tag_dyneither(_tmp1BF,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp669,_tmp668,_tmp667,_tmp666,_tag_dyneither(_tmp1BE,sizeof(void*),1U));});});}_LL0:;};}
# 1136
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1C7=Cyc_Absyn_exn_type();
Cyc_Tcexp_tcExp(te,& _tmp1C7,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1C7,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1CA=({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U,({struct _dyneither_ptr _tmp66A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C7));_tmp506.f1=_tmp66A;});_tmp506;});struct Cyc_String_pa_PrintArg_struct _tmp1CB=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({
struct _dyneither_ptr _tmp66B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp505.f1=_tmp66B;});_tmp505;});void*_tmp1C8[2U];_tmp1C8[0]=& _tmp1CA,_tmp1C8[1]=& _tmp1CB;({unsigned int _tmp66D=loc;struct _dyneither_ptr _tmp66C=({const char*_tmp1C9="expected %s but found %s";_tag_dyneither(_tmp1C9,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp66D,_tmp66C,_tag_dyneither(_tmp1C8,sizeof(void*),2U));});});
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1148
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1150
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1152
({void*_tmp66E=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp66E;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1CC=t1;void*_tmp1E7;struct Cyc_Absyn_Tqual _tmp1E6;void*_tmp1E5;void*_tmp1E4;void*_tmp1E3;void*_tmp1E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CC)->tag == 3U){_LL1: _tmp1E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CC)->f1).elt_type;_tmp1E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CC)->f1).elt_tq;_tmp1E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CC)->f1).ptr_atts).rgn;_tmp1E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CC)->f1).ptr_atts).nullable;_tmp1E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CC)->f1).ptr_atts).bounds;_tmp1E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CC)->f1).ptr_atts).zero_term;_LL2:
# 1156
{void*_tmp1CD=Cyc_Tcutil_compress(_tmp1E7);void*_tmp1CE=_tmp1CD;struct Cyc_List_List*_tmp1E1;void*_tmp1E0;struct Cyc_Absyn_Tqual _tmp1DF;void*_tmp1DE;struct Cyc_List_List*_tmp1DD;int _tmp1DC;struct Cyc_Absyn_VarargInfo*_tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_List_List*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_List_List*_tmp1D5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->tag == 5U){_LL6: _tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).tvars;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).effect;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ret_tqual;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ret_type;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).args;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).c_varargs;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).cyc_varargs;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).rgn_po;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).attributes;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).requires_clause;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).requires_relns;_tmp1D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ensures_clause;_tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ensures_relns;_LL7: {
# 1158
struct Cyc_List_List*instantiation=0;
# 1160
for(0;ts != 0  && _tmp1E1 != 0;(ts=ts->tl,_tmp1E1=_tmp1E1->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1E1->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp672=loc;struct Cyc_Tcenv_Tenv*_tmp671=te;struct Cyc_List_List*_tmp670=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp66F=k;Cyc_Tcutil_check_type(_tmp672,_tmp671,_tmp670,_tmp66F,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct _tuple13*_tmp673=({struct _tuple13*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->f1=(struct Cyc_Absyn_Tvar*)_tmp1E1->hd,_tmp1CF->f2=(void*)ts->hd;_tmp1CF;});_tmp1D0->hd=_tmp673;}),_tmp1D0->tl=instantiation;_tmp1D0;});}
# 1166
if(ts != 0)
return({void*_tmp1D1=0U;({struct Cyc_Tcenv_Tenv*_tmp677=te;unsigned int _tmp676=loc;void**_tmp675=topt;struct _dyneither_ptr _tmp674=({const char*_tmp1D2="too many type variables in instantiation";_tag_dyneither(_tmp1D2,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp677,_tmp676,_tmp675,_tmp674,_tag_dyneither(_tmp1D1,sizeof(void*),0U));});});
# 1171
if(_tmp1E1 == 0){
_tmp1DA=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1DA);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1DA);
_tmp1DA=0;}{
# 1176
void*new_fn_typ=({
struct Cyc_List_List*_tmp678=instantiation;Cyc_Tcutil_substitute(_tmp678,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));
_tmp1D4->tag=5U,(_tmp1D4->f1).tvars=_tmp1E1,(_tmp1D4->f1).effect=_tmp1E0,(_tmp1D4->f1).ret_tqual=_tmp1DF,(_tmp1D4->f1).ret_type=_tmp1DE,(_tmp1D4->f1).args=_tmp1DD,(_tmp1D4->f1).c_varargs=_tmp1DC,(_tmp1D4->f1).cyc_varargs=_tmp1DB,(_tmp1D4->f1).rgn_po=_tmp1DA,(_tmp1D4->f1).attributes=_tmp1D9,(_tmp1D4->f1).requires_clause=_tmp1D8,(_tmp1D4->f1).requires_relns=_tmp1D7,(_tmp1D4->f1).ensures_clause=_tmp1D6,(_tmp1D4->f1).ensures_relns=_tmp1D5;_tmp1D4;}));});
# 1181
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->tag=3U,(_tmp1D3->f1).elt_type=new_fn_typ,(_tmp1D3->f1).elt_tq=_tmp1E6,((_tmp1D3->f1).ptr_atts).rgn=_tmp1E5,((_tmp1D3->f1).ptr_atts).nullable=_tmp1E4,((_tmp1D3->f1).ptr_atts).bounds=_tmp1E3,((_tmp1D3->f1).ptr_atts).zero_term=_tmp1E2,((_tmp1D3->f1).ptr_atts).ptrloc=0;_tmp1D3;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1184
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1187
return({struct Cyc_String_pa_PrintArg_struct _tmp1EA=({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0U,({
struct _dyneither_ptr _tmp679=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp507.f1=_tmp679;});_tmp507;});void*_tmp1E8[1U];_tmp1E8[0]=& _tmp1EA;({struct Cyc_Tcenv_Tenv*_tmp67D=te;unsigned int _tmp67C=loc;void**_tmp67B=topt;struct _dyneither_ptr _tmp67A=({const char*_tmp1E9="expecting polymorphic type but found %s";_tag_dyneither(_tmp1E9,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp67D,_tmp67C,_tmp67B,_tmp67A,_tag_dyneither(_tmp1E8,sizeof(void*),1U));});});};}
# 1192
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1194
({unsigned int _tmp681=loc;struct Cyc_Tcenv_Tenv*_tmp680=te;struct Cyc_List_List*_tmp67F=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp67E=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1194
Cyc_Tcutil_check_type(_tmp681,_tmp680,_tmp67F,_tmp67E,1,0,t);});
# 1196
Cyc_Tcutil_check_no_qual(loc,t);
# 1198
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1203
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1210
Cyc_Tcutil_unify(t2,t);{
void*_tmp1EB=({struct Cyc_String_pa_PrintArg_struct _tmp1EE=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({
struct _dyneither_ptr _tmp682=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp509.f1=_tmp682;});_tmp509;});struct Cyc_String_pa_PrintArg_struct _tmp1EF=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({struct _dyneither_ptr _tmp683=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp508.f1=_tmp683;});_tmp508;});void*_tmp1EC[2U];_tmp1EC[0]=& _tmp1EE,_tmp1EC[1]=& _tmp1EF;({struct Cyc_Tcenv_Tenv*_tmp686=te;unsigned int _tmp685=loc;struct _dyneither_ptr _tmp684=({const char*_tmp1ED="cannot cast %s to %s";_tag_dyneither(_tmp1ED,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp686,_tmp685,& t,_tmp684,_tag_dyneither(_tmp1EC,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return _tmp1EB;};}}}
# 1220
{struct _tuple0 _tmp1F0=({struct _tuple0 _tmp50A;_tmp50A.f1=e->r,({void*_tmp687=Cyc_Tcutil_compress(t);_tmp50A.f2=_tmp687;});_tmp50A;});struct _tuple0 _tmp1F1=_tmp1F0;int _tmp1F9;void*_tmp1F8;void*_tmp1F7;void*_tmp1F6;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F1.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1.f2)->tag == 3U){_LL1: _tmp1F9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1F1.f1)->f1).fat_result;_tmp1F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1.f2)->f1).ptr_atts).nullable;_tmp1F7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1.f2)->f1).ptr_atts).bounds;_tmp1F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F1.f2)->f1).ptr_atts).zero_term;_LL2:
# 1224
 if((_tmp1F9  && !Cyc_Tcutil_force_type2bool(0,_tmp1F6)) && Cyc_Tcutil_force_type2bool(0,_tmp1F8)){
struct Cyc_Absyn_Exp*_tmp1F2=({void*_tmp688=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp688,_tmp1F7);});
if(_tmp1F2 != 0){
struct Cyc_Absyn_Exp*_tmp1F3=e;
if((Cyc_Evexp_eval_const_uint_exp(_tmp1F3)).f1 == 1)
({void*_tmp1F4=0U;({unsigned int _tmp68A=loc;struct _dyneither_ptr _tmp689=({const char*_tmp1F5="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp1F5,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp68A,_tmp689,_tag_dyneither(_tmp1F4,sizeof(void*),0U));});});}}
# 1232
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1235
return t;};}
# 1239
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp1FA=0;
struct Cyc_Absyn_Tqual _tmp1FB=Cyc_Absyn_empty_tqual(0U);
int _tmp1FC=0;
if(topt != 0){
void*_tmp1FD=Cyc_Tcutil_compress(*topt);void*_tmp1FE=_tmp1FD;void**_tmp201;struct Cyc_Absyn_Tqual _tmp200;void*_tmp1FF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE)->tag == 3U){_LL1: _tmp201=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE)->f1).elt_type;_tmp200=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE)->f1).elt_tq;_tmp1FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE)->f1).ptr_atts).nullable;_LL2:
# 1246
 _tmp1FA=_tmp201;
_tmp1FB=_tmp200;
_tmp1FC=Cyc_Tcutil_force_type2bool(0,_tmp1FF);
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1260
({struct Cyc_Tcenv_Tenv*_tmp68C=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp68B=_tmp1FA;Cyc_Tcexp_tcExpNoInst(_tmp68C,_tmp68B,e);});
# 1263
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp202=0U;({unsigned int _tmp68E=e->loc;struct _dyneither_ptr _tmp68D=({const char*_tmp203="Cannot take the address of an alias-free path";_tag_dyneither(_tmp203,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp68E,_tmp68D,_tag_dyneither(_tmp202,sizeof(void*),0U));});});
# 1269
{void*_tmp204=e->r;void*_tmp205=_tmp204;struct Cyc_Absyn_Exp*_tmp209;struct Cyc_Absyn_Exp*_tmp208;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp205)->tag == 23U){_LL6: _tmp209=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp205)->f1;_tmp208=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp205)->f2;_LL7:
# 1271
{void*_tmp206=Cyc_Tcutil_compress((void*)_check_null(_tmp209->topt));void*_tmp207=_tmp206;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp207)->tag == 6U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1276
({void*_tmp68F=(Cyc_Absyn_add_exp(_tmp209,_tmp208,0U))->r;e0->r=_tmp68F;});
return Cyc_Tcexp_tcPlus(te,_tmp209,_tmp208);}_LLA:;}
# 1279
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1284
{void*_tmp20A=e->r;void*_tmp20B=_tmp20A;switch(*((int*)_tmp20B)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp20B)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp20B)->f3 == 1){_LL12: _LL13:
# 1287
({void*_tmp20C=0U;({unsigned int _tmp691=e->loc;struct _dyneither_ptr _tmp690=({const char*_tmp20D="cannot take the address of a @tagged union member";_tag_dyneither(_tmp20D,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp691,_tmp690,_tag_dyneither(_tmp20C,sizeof(void*),0U));});});goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1292
struct _tuple14 _tmp20E=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp20F=_tmp20E;int _tmp213;void*_tmp212;_LL17: _tmp213=_tmp20F.f1;_tmp212=_tmp20F.f2;_LL18:;
# 1294
if(Cyc_Tcutil_is_noalias_region(_tmp212,0))
({void*_tmp210=0U;({unsigned int _tmp693=e->loc;struct _dyneither_ptr _tmp692=({const char*_tmp211="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp211,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp693,_tmp692,_tag_dyneither(_tmp210,sizeof(void*),0U));});});{
# 1297
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp213){
tq.print_const=1;
tq.real_const=1;}{
# 1303
void*t=_tmp1FC?
Cyc_Absyn_star_type((void*)_check_null(e->topt),_tmp212,tq,Cyc_Absyn_false_type):
 Cyc_Absyn_at_type((void*)_check_null(e->topt),_tmp212,tq,Cyc_Absyn_false_type);
return t;};};};}
# 1310
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1314
return Cyc_Absyn_uint_type;
# 1316
({unsigned int _tmp696=loc;struct Cyc_Tcenv_Tenv*_tmp695=te;struct Cyc_List_List*_tmp694=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp696,_tmp695,_tmp694,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp216=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,({
struct _dyneither_ptr _tmp697=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp50B.f1=_tmp697;});_tmp50B;});void*_tmp214[1U];_tmp214[0]=& _tmp216;({unsigned int _tmp699=loc;struct _dyneither_ptr _tmp698=({const char*_tmp215="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp215,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp699,_tmp698,_tag_dyneither(_tmp214,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp217=Cyc_Tcutil_compress(*topt);void*_tmp218=_tmp217;void*_tmp21D;void*_tmp21C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->f2 != 0){_LL1: _tmp21D=_tmp218;_tmp21C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->f2)->hd;_LL2: {
# 1325
struct Cyc_Absyn_Exp*_tmp219=Cyc_Absyn_sizeoftype_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21A=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->tag=9U,_tmp21B->f1=_tmp219;_tmp21B;});
if(Cyc_Tcutil_unify(_tmp21C,(void*)_tmp21A))return _tmp21D;
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1331
return Cyc_Absyn_uint_type;}
# 1334
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1341
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1343
({unsigned int _tmp69C=loc;struct Cyc_Tcenv_Tenv*_tmp69B=te;struct Cyc_List_List*_tmp69A=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp69C,_tmp69B,_tmp69A,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp21E=fs;
void*_tmp21F=t_orig;
for(0;_tmp21E != 0;_tmp21E=_tmp21E->tl){
void*_tmp220=(void*)_tmp21E->hd;
void*_tmp221=_tmp220;unsigned int _tmp25B;struct _dyneither_ptr*_tmp25A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp221)->tag == 0U){_LL1: _tmp25A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp221)->f1;_LL2: {
# 1351
int bad_type=1;
{void*_tmp222=Cyc_Tcutil_compress(_tmp21F);void*_tmp223=_tmp222;struct Cyc_List_List*_tmp22B;struct Cyc_Absyn_Aggrdecl**_tmp22A;switch(*((int*)_tmp223)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp223)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp223)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp22A=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp223)->f1)->f1).KnownAggr).val;_LL7:
# 1354
 if((*_tmp22A)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp25A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp22A)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp226=({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0U,_tmp50C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp25A);_tmp50C;});void*_tmp224[1U];_tmp224[0]=& _tmp226;({unsigned int _tmp69E=loc;struct _dyneither_ptr _tmp69D=({const char*_tmp225="no field of struct/union has name %s";_tag_dyneither(_tmp225,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp69E,_tmp69D,_tag_dyneither(_tmp224,sizeof(void*),1U));});});else{
# 1359
_tmp21F=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp22B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp223)->f2;_LL9: {
# 1363
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp25A,_tmp22B);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp229=({struct Cyc_String_pa_PrintArg_struct _tmp50D;_tmp50D.tag=0U,_tmp50D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp25A);_tmp50D;});void*_tmp227[1U];_tmp227[0]=& _tmp229;({unsigned int _tmp6A0=loc;struct _dyneither_ptr _tmp69F=({const char*_tmp228="no field of struct/union has name %s";_tag_dyneither(_tmp228,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6A0,_tmp69F,_tag_dyneither(_tmp227,sizeof(void*),1U));});});else{
# 1367
_tmp21F=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1372
if(bad_type){
if(_tmp21E == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp22E=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0U,({struct _dyneither_ptr _tmp6A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21F));_tmp50E.f1=_tmp6A1;});_tmp50E;});void*_tmp22C[1U];_tmp22C[0]=& _tmp22E;({unsigned int _tmp6A3=loc;struct _dyneither_ptr _tmp6A2=({const char*_tmp22D="%s is not a known struct/union type";_tag_dyneither(_tmp22D,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6A3,_tmp6A2,_tag_dyneither(_tmp22C,sizeof(void*),1U));});});else{
# 1376
struct _dyneither_ptr _tmp22F=({struct Cyc_String_pa_PrintArg_struct _tmp242=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,({struct _dyneither_ptr _tmp6A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp515.f1=_tmp6A4;});_tmp515;});void*_tmp240[1U];_tmp240[0]=& _tmp242;({struct _dyneither_ptr _tmp6A5=({const char*_tmp241="(%s)";_tag_dyneither(_tmp241,sizeof(char),5U);});Cyc_aprintf(_tmp6A5,_tag_dyneither(_tmp240,sizeof(void*),1U));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp21E;x=x->tl){
void*_tmp230=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp231=_tmp230;unsigned int _tmp23B;struct _dyneither_ptr*_tmp23A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp231)->tag == 0U){_LLD: _tmp23A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp231)->f1;_LLE:
# 1381
 _tmp22F=({struct Cyc_String_pa_PrintArg_struct _tmp234=({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0U,_tmp510.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22F);_tmp510;});struct Cyc_String_pa_PrintArg_struct _tmp235=({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0U,_tmp50F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp23A);_tmp50F;});void*_tmp232[2U];_tmp232[0]=& _tmp234,_tmp232[1]=& _tmp235;({struct _dyneither_ptr _tmp6A6=({const char*_tmp233="%s.%s";_tag_dyneither(_tmp233,sizeof(char),6U);});Cyc_aprintf(_tmp6A6,_tag_dyneither(_tmp232,sizeof(void*),2U));});});goto _LLC;}else{_LLF: _tmp23B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp231)->f1;_LL10:
# 1383
 _tmp22F=({struct Cyc_String_pa_PrintArg_struct _tmp238=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0U,_tmp512.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22F);_tmp512;});struct Cyc_Int_pa_PrintArg_struct _tmp239=({struct Cyc_Int_pa_PrintArg_struct _tmp511;_tmp511.tag=1U,_tmp511.f1=(unsigned long)((int)_tmp23B);_tmp511;});void*_tmp236[2U];_tmp236[0]=& _tmp238,_tmp236[1]=& _tmp239;({struct _dyneither_ptr _tmp6A7=({const char*_tmp237="%s.%d";_tag_dyneither(_tmp237,sizeof(char),6U);});Cyc_aprintf(_tmp6A7,_tag_dyneither(_tmp236,sizeof(void*),2U));});});goto _LLC;}_LLC:;}
# 1385
({struct Cyc_String_pa_PrintArg_struct _tmp23E=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,_tmp514.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22F);_tmp514;});struct Cyc_String_pa_PrintArg_struct _tmp23F=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,({struct _dyneither_ptr _tmp6A8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21F));_tmp513.f1=_tmp6A8;});_tmp513;});void*_tmp23C[2U];_tmp23C[0]=& _tmp23E,_tmp23C[1]=& _tmp23F;({unsigned int _tmp6AA=loc;struct _dyneither_ptr _tmp6A9=({const char*_tmp23D="%s == %s is not a struct/union type";_tag_dyneither(_tmp23D,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6AA,_tmp6A9,_tag_dyneither(_tmp23C,sizeof(void*),2U));});});}}
# 1388
goto _LL0;}}else{_LL3: _tmp25B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp221)->f1;_LL4: {
# 1390
int bad_type=1;
{void*_tmp243=Cyc_Tcutil_compress(_tmp21F);void*_tmp244=_tmp243;struct Cyc_List_List*_tmp256;struct Cyc_List_List*_tmp255;struct Cyc_Absyn_Datatypefield*_tmp254;struct Cyc_Absyn_Aggrdecl**_tmp253;switch(*((int*)_tmp244)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)->f1).KnownAggr).tag == 2){_LL12: _tmp253=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)->f1).KnownAggr).val;_LL13:
# 1393
 if((*_tmp253)->impl == 0)
goto _LL11;
_tmp255=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp253)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)->f1).KnownDatatypefield).tag == 2){_LL18: _tmp254=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244)->f1)->f1).KnownDatatypefield).val).f2;_LL19:
# 1413
 if(({int _tmp6AB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp254->typs);_tmp6AB < _tmp25B;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp24F=({struct Cyc_Int_pa_PrintArg_struct _tmp517;_tmp517.tag=1U,({
unsigned long _tmp6AC=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp254->typs);_tmp517.f1=_tmp6AC;});_tmp517;});struct Cyc_Int_pa_PrintArg_struct _tmp250=({struct Cyc_Int_pa_PrintArg_struct _tmp516;_tmp516.tag=1U,_tmp516.f1=(unsigned long)((int)_tmp25B);_tmp516;});void*_tmp24D[2U];_tmp24D[0]=& _tmp24F,_tmp24D[1]=& _tmp250;({unsigned int _tmp6AE=loc;struct _dyneither_ptr _tmp6AD=({const char*_tmp24E="datatype field has too few components: %d < %d";_tag_dyneither(_tmp24E,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6AE,_tmp6AD,_tag_dyneither(_tmp24D,sizeof(void*),2U));});});else{
# 1417
if(_tmp25B != 0)
_tmp21F=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp254->typs,(int)(_tmp25B - 1))).f2;else{
if(_tmp21E->tl != 0)
({void*_tmp251=0U;({unsigned int _tmp6B0=loc;struct _dyneither_ptr _tmp6AF=({const char*_tmp252="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp252,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6B0,_tmp6AF,_tag_dyneither(_tmp251,sizeof(void*),0U));});});}}
# 1422
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: goto _LL1A;}case 7U: _LL14: _tmp255=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp244)->f2;_LL15:
# 1397
 if(({int _tmp6B1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp255);_tmp6B1 <= _tmp25B;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp247=({struct Cyc_Int_pa_PrintArg_struct _tmp519;_tmp519.tag=1U,({
unsigned long _tmp6B2=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp255);_tmp519.f1=_tmp6B2;});_tmp519;});struct Cyc_Int_pa_PrintArg_struct _tmp248=({struct Cyc_Int_pa_PrintArg_struct _tmp518;_tmp518.tag=1U,_tmp518.f1=(unsigned long)((int)_tmp25B);_tmp518;});void*_tmp245[2U];_tmp245[0]=& _tmp247,_tmp245[1]=& _tmp248;({unsigned int _tmp6B4=loc;struct _dyneither_ptr _tmp6B3=({const char*_tmp246="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp246,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6B4,_tmp6B3,_tag_dyneither(_tmp245,sizeof(void*),2U));});});else{
# 1401
_tmp21F=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp255,(int)_tmp25B))->type;}
bad_type=0;
goto _LL11;case 6U: _LL16: _tmp256=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp244)->f1;_LL17:
# 1405
 if(({int _tmp6B5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp256);_tmp6B5 <= _tmp25B;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp24B=({struct Cyc_Int_pa_PrintArg_struct _tmp51B;_tmp51B.tag=1U,({
unsigned long _tmp6B6=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp256);_tmp51B.f1=_tmp6B6;});_tmp51B;});struct Cyc_Int_pa_PrintArg_struct _tmp24C=({struct Cyc_Int_pa_PrintArg_struct _tmp51A;_tmp51A.tag=1U,_tmp51A.f1=(unsigned long)((int)_tmp25B);_tmp51A;});void*_tmp249[2U];_tmp249[0]=& _tmp24B,_tmp249[1]=& _tmp24C;({unsigned int _tmp6B8=loc;struct _dyneither_ptr _tmp6B7=({const char*_tmp24A="tuple has too few components: %d <= %d";_tag_dyneither(_tmp24A,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6B8,_tmp6B7,_tag_dyneither(_tmp249,sizeof(void*),2U));});});else{
# 1409
_tmp21F=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp256,(int)_tmp25B)).f2;}
bad_type=0;
goto _LL11;default: _LL1A: _LL1B:
# 1424
 goto _LL11;}_LL11:;}
# 1426
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp259=({struct Cyc_String_pa_PrintArg_struct _tmp51C;_tmp51C.tag=0U,({struct _dyneither_ptr _tmp6B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp21F));_tmp51C.f1=_tmp6B9;});_tmp51C;});void*_tmp257[1U];_tmp257[0]=& _tmp259;({unsigned int _tmp6BB=loc;struct _dyneither_ptr _tmp6BA=({const char*_tmp258="%s is not a known type";_tag_dyneither(_tmp258,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6BB,_tmp6BA,_tag_dyneither(_tmp257,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1431
return Cyc_Absyn_uint_type;};}
# 1435
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp25C=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp25C,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp25D=t;void*_tmp274;void*_tmp273;void*_tmp272;void*_tmp271;switch(*((int*)_tmp25D)){case 1U: _LL1: _LL2: {
# 1441
struct Cyc_List_List*_tmp25E=Cyc_Tcenv_lookup_type_vars(_tmp25C);
void*_tmp25F=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->v=_tmp25E;_tmp267;}));
void*_tmp260=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->v=_tmp25E;_tmp266;}));
void*_tmp261=Cyc_Tcutil_any_bounds(& _tmp25C);
void*_tmp262=Cyc_Tcutil_any_bool(& _tmp25C);
struct Cyc_Absyn_PtrAtts _tmp263=({struct Cyc_Absyn_PtrAtts _tmp51D;_tmp51D.rgn=_tmp260,({void*_tmp6BC=Cyc_Tcutil_any_bool(& _tmp25C);_tmp51D.nullable=_tmp6BC;}),_tmp51D.bounds=_tmp261,_tmp51D.zero_term=_tmp262,_tmp51D.ptrloc=0;_tmp51D;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp264=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->tag=3U,(_tmp265->f1).elt_type=_tmp25F,({struct Cyc_Absyn_Tqual _tmp6BD=Cyc_Absyn_empty_tqual(0U);(_tmp265->f1).elt_tq=_tmp6BD;}),(_tmp265->f1).ptr_atts=_tmp263;_tmp265;});
Cyc_Tcutil_unify(t,(void*)_tmp264);
_tmp274=_tmp25F;_tmp273=_tmp260;_tmp272=_tmp261;_tmp271=_tmp262;goto _LL4;}case 3U: _LL3: _tmp274=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25D)->f1).elt_type;_tmp273=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25D)->f1).ptr_atts).rgn;_tmp272=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25D)->f1).ptr_atts).bounds;_tmp271=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25D)->f1).ptr_atts).zero_term;_LL4:
# 1451
 Cyc_Tcenv_check_rgn_accessible(_tmp25C,loc,_tmp273);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp272);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmp274),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp25C)){
void*_tmp268=Cyc_Tcutil_compress(_tmp274);void*_tmp269=_tmp268;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp269)->tag == 5U){_LL8: _LL9:
# 1456
 if(Cyc_Tc_aggressive_warn)
({void*_tmp26A=0U;({unsigned int _tmp6BF=loc;struct _dyneither_ptr _tmp6BE=({const char*_tmp26B="unnecessary dereference for function type";_tag_dyneither(_tmp26B,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6BF,_tmp6BE,_tag_dyneither(_tmp26A,sizeof(void*),0U));});});
return t;}else{_LLA: _LLB:
# 1460
({void*_tmp26C=0U;({unsigned int _tmp6C1=loc;struct _dyneither_ptr _tmp6C0=({const char*_tmp26D="can't dereference abstract pointer type";_tag_dyneither(_tmp26D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6C1,_tmp6C0,_tag_dyneither(_tmp26C,sizeof(void*),0U));});});}_LL7:;}
# 1463
return _tmp274;default: _LL5: _LL6:
# 1465
 return({struct Cyc_String_pa_PrintArg_struct _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp51E;_tmp51E.tag=0U,({struct _dyneither_ptr _tmp6C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp51E.f1=_tmp6C2;});_tmp51E;});void*_tmp26E[1U];_tmp26E[0]=& _tmp270;({struct Cyc_Tcenv_Tenv*_tmp6C6=_tmp25C;unsigned int _tmp6C5=loc;void**_tmp6C4=topt;struct _dyneither_ptr _tmp6C3=({const char*_tmp26F="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp26F,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp6C6,_tmp6C5,_tmp6C4,_tmp6C3,_tag_dyneither(_tmp26E,sizeof(void*),1U));});});}_LL0:;};}
# 1470
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1474
({struct Cyc_Tcenv_Tenv*_tmp6C7=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6C7,0,e);});
# 1476
({int _tmp6C8=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6C8;});{
void*_tmp275=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp276=_tmp275;enum Cyc_Absyn_AggrKind _tmp290;struct Cyc_List_List*_tmp28F;struct Cyc_Absyn_Aggrdecl*_tmp28E;struct Cyc_List_List*_tmp28D;switch(*((int*)_tmp276)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp276)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp276)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp28E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp276)->f1)->f1).KnownAggr).val;_tmp28D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp276)->f2;_LL2: {
# 1479
struct Cyc_Absyn_Aggrfield*_tmp277=Cyc_Absyn_lookup_decl_field(_tmp28E,f);
if(_tmp277 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp520;_tmp520.tag=0U,({
struct _dyneither_ptr _tmp6C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28E->name));_tmp520.f1=_tmp6C9;});_tmp520;});struct Cyc_String_pa_PrintArg_struct _tmp27B=({struct Cyc_String_pa_PrintArg_struct _tmp51F;_tmp51F.tag=0U,_tmp51F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp51F;});void*_tmp278[2U];_tmp278[0]=& _tmp27A,_tmp278[1]=& _tmp27B;({struct Cyc_Tcenv_Tenv*_tmp6CD=te;unsigned int _tmp6CC=loc;void**_tmp6CB=topt;struct _dyneither_ptr _tmp6CA=({const char*_tmp279="%s has no %s member";_tag_dyneither(_tmp279,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp6CD,_tmp6CC,_tmp6CB,_tmp6CA,_tag_dyneither(_tmp278,sizeof(void*),2U));});});
# 1484
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp28E->impl))->tagged)*is_tagged=1;{
void*t2=_tmp277->type;
if(_tmp28D != 0){
struct Cyc_List_List*_tmp27C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp28E->tvs,_tmp28D);
t2=Cyc_Tcutil_substitute(_tmp27C,_tmp277->type);}
# 1492
if((((_tmp28E->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp28E->impl))->tagged) && !Cyc_Tcutil_is_bits_only_type(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp277->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0U,_tmp521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp521;});void*_tmp27D[1U];_tmp27D[0]=& _tmp27F;({struct Cyc_Tcenv_Tenv*_tmp6D1=te;unsigned int _tmp6D0=loc;void**_tmp6CF=topt;struct _dyneither_ptr _tmp6CE=({const char*_tmp27E="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp27E,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6D1,_tmp6D0,_tmp6CF,_tmp6CE,_tag_dyneither(_tmp27D,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp28E->impl))->exist_vars != 0){
# 1497
if(!({void*_tmp6D2=t2;Cyc_Tcutil_unify(_tmp6D2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp522;_tmp522.tag=0U,_tmp522.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp522;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct Cyc_Tcenv_Tenv*_tmp6D6=te;unsigned int _tmp6D5=loc;void**_tmp6D4=topt;struct _dyneither_ptr _tmp6D3=({const char*_tmp281="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp281,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp6D6,_tmp6D5,_tmp6D4,_tmp6D3,_tag_dyneither(_tmp280,sizeof(void*),1U));});});}
# 1500
return t2;};}}else{goto _LL5;}}else{goto _LL5;}case 7U: _LL3: _tmp290=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp276)->f1;_tmp28F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp276)->f2;_LL4: {
# 1502
struct Cyc_Absyn_Aggrfield*_tmp283=Cyc_Absyn_lookup_field(_tmp28F,f);
if(_tmp283 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0U,_tmp523.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp523;});void*_tmp284[1U];_tmp284[0]=& _tmp286;({struct Cyc_Tcenv_Tenv*_tmp6DA=te;unsigned int _tmp6D9=loc;void**_tmp6D8=topt;struct _dyneither_ptr _tmp6D7=({const char*_tmp285="type has no %s member";_tag_dyneither(_tmp285,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp6DA,_tmp6D9,_tmp6D8,_tmp6D7,_tag_dyneither(_tmp284,sizeof(void*),1U));});});
# 1507
if(((_tmp290 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_is_bits_only_type(_tmp283->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp283->requires_clause == 0)
# 1509
return({struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,_tmp524.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp524;});void*_tmp287[1U];_tmp287[0]=& _tmp289;({struct Cyc_Tcenv_Tenv*_tmp6DE=te;unsigned int _tmp6DD=loc;void**_tmp6DC=topt;struct _dyneither_ptr _tmp6DB=({const char*_tmp288="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp288,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6DE,_tmp6DD,_tmp6DC,_tmp6DB,_tag_dyneither(_tmp287,sizeof(void*),1U));});});
return _tmp283->type;}default: _LL5: _LL6:
# 1512
 return({struct Cyc_String_pa_PrintArg_struct _tmp28C=({struct Cyc_String_pa_PrintArg_struct _tmp525;_tmp525.tag=0U,({
struct _dyneither_ptr _tmp6DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp525.f1=_tmp6DF;});_tmp525;});void*_tmp28A[1U];_tmp28A[0]=& _tmp28C;({struct Cyc_Tcenv_Tenv*_tmp6E3=te;unsigned int _tmp6E2=loc;void**_tmp6E1=topt;struct _dyneither_ptr _tmp6E0=({const char*_tmp28B="expecting struct or union, found %s";_tag_dyneither(_tmp28B,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp6E3,_tmp6E2,_tmp6E1,_tmp6E0,_tag_dyneither(_tmp28A,sizeof(void*),1U));});});}_LL0:;};}
# 1518
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1521
({struct Cyc_Tcenv_Tenv*_tmp6E4=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp6E4,0,e);});
# 1523
({int _tmp6E5=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6E5;});
{void*_tmp291=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp292=_tmp291;void*_tmp2B5;void*_tmp2B4;void*_tmp2B3;void*_tmp2B2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292)->tag == 3U){_LL1: _tmp2B5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292)->f1).elt_type;_tmp2B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292)->f1).ptr_atts).rgn;_tmp2B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292)->f1).ptr_atts).bounds;_tmp2B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292)->f1).ptr_atts).zero_term;_LL2:
# 1526
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2B3);
{void*_tmp293=Cyc_Tcutil_compress(_tmp2B5);void*_tmp294=_tmp293;enum Cyc_Absyn_AggrKind _tmp2B1;struct Cyc_List_List*_tmp2B0;struct Cyc_Absyn_Aggrdecl*_tmp2AF;struct Cyc_List_List*_tmp2AE;switch(*((int*)_tmp294)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp2AF=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f1)->f1).KnownAggr).val;_tmp2AE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp294)->f2;_LL7: {
# 1529
struct Cyc_Absyn_Aggrfield*_tmp295=Cyc_Absyn_lookup_decl_field(_tmp2AF,f);
if(_tmp295 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp298=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,({
struct _dyneither_ptr _tmp6E6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2AF->name));_tmp527.f1=_tmp6E6;});_tmp527;});struct Cyc_String_pa_PrintArg_struct _tmp299=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,_tmp526.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp526;});void*_tmp296[2U];_tmp296[0]=& _tmp298,_tmp296[1]=& _tmp299;({struct Cyc_Tcenv_Tenv*_tmp6EA=te;unsigned int _tmp6E9=loc;void**_tmp6E8=topt;struct _dyneither_ptr _tmp6E7=({const char*_tmp297="type %s has no %s member";_tag_dyneither(_tmp297,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp6EA,_tmp6E9,_tmp6E8,_tmp6E7,_tag_dyneither(_tmp296,sizeof(void*),2U));});});
# 1534
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AF->impl))->tagged)*is_tagged=1;{
void*t3=_tmp295->type;
if(_tmp2AE != 0){
struct Cyc_List_List*_tmp29A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2AF->tvs,_tmp2AE);
t3=Cyc_Tcutil_substitute(_tmp29A,_tmp295->type);}{
# 1540
struct Cyc_Absyn_Kind*_tmp29B=Cyc_Tcutil_type_kind(t3);
# 1543
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp29B) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp29C=Cyc_Tcutil_compress(t3);void*_tmp29D=_tmp29C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp29D)->tag == 4U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1547
 return({struct Cyc_String_pa_PrintArg_struct _tmp2A0=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0U,_tmp528.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp528;});void*_tmp29E[1U];_tmp29E[0]=& _tmp2A0;({struct Cyc_Tcenv_Tenv*_tmp6EE=te;unsigned int _tmp6ED=loc;void**_tmp6EC=topt;struct _dyneither_ptr _tmp6EB=({const char*_tmp29F="cannot get member %s since its type is abstract";_tag_dyneither(_tmp29F,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp6EE,_tmp6ED,_tmp6EC,_tmp6EB,_tag_dyneither(_tmp29E,sizeof(void*),1U));});});}_LLC:;}
# 1552
if((((_tmp2AF->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AF->impl))->tagged) && !
Cyc_Tcutil_is_bits_only_type(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp295->requires_clause == 0)
# 1555
return({struct Cyc_String_pa_PrintArg_struct _tmp2A3=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,_tmp529.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp529;});void*_tmp2A1[1U];_tmp2A1[0]=& _tmp2A3;({struct Cyc_Tcenv_Tenv*_tmp6F2=te;unsigned int _tmp6F1=loc;void**_tmp6F0=topt;struct _dyneither_ptr _tmp6EF=({const char*_tmp2A2="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2A2,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6F2,_tmp6F1,_tmp6F0,_tmp6EF,_tag_dyneither(_tmp2A1,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2AF->impl))->exist_vars != 0){
if(!({void*_tmp6F3=t3;Cyc_Tcutil_unify(_tmp6F3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2A6=({struct Cyc_String_pa_PrintArg_struct _tmp52A;_tmp52A.tag=0U,_tmp52A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp52A;});void*_tmp2A4[1U];_tmp2A4[0]=& _tmp2A6;({struct Cyc_Tcenv_Tenv*_tmp6F7=te;unsigned int _tmp6F6=loc;void**_tmp6F5=topt;struct _dyneither_ptr _tmp6F4=({const char*_tmp2A5="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2A5,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp6F7,_tmp6F6,_tmp6F5,_tmp6F4,_tag_dyneither(_tmp2A4,sizeof(void*),1U));});});}
# 1562
return t3;};};}}else{goto _LLA;}}else{goto _LLA;}case 7U: _LL8: _tmp2B1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp294)->f1;_tmp2B0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp294)->f2;_LL9: {
# 1564
struct Cyc_Absyn_Aggrfield*_tmp2A7=Cyc_Absyn_lookup_field(_tmp2B0,f);
if(_tmp2A7 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp52B;_tmp52B.tag=0U,_tmp52B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp52B;});void*_tmp2A8[1U];_tmp2A8[0]=& _tmp2AA;({struct Cyc_Tcenv_Tenv*_tmp6FB=te;unsigned int _tmp6FA=loc;void**_tmp6F9=topt;struct _dyneither_ptr _tmp6F8=({const char*_tmp2A9="type has no %s field";_tag_dyneither(_tmp2A9,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6FB,_tmp6FA,_tmp6F9,_tmp6F8,_tag_dyneither(_tmp2A8,sizeof(void*),1U));});});
# 1569
if((_tmp2B1 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_is_bits_only_type(_tmp2A7->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2AD=({struct Cyc_String_pa_PrintArg_struct _tmp52C;_tmp52C.tag=0U,_tmp52C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp52C;});void*_tmp2AB[1U];_tmp2AB[0]=& _tmp2AD;({struct Cyc_Tcenv_Tenv*_tmp6FF=te;unsigned int _tmp6FE=loc;void**_tmp6FD=topt;struct _dyneither_ptr _tmp6FC=({const char*_tmp2AC="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2AC,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6FF,_tmp6FE,_tmp6FD,_tmp6FC,_tag_dyneither(_tmp2AB,sizeof(void*),1U));});});
return _tmp2A7->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1574
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1577
return({struct Cyc_String_pa_PrintArg_struct _tmp2B8=({struct Cyc_String_pa_PrintArg_struct _tmp52D;_tmp52D.tag=0U,({
struct _dyneither_ptr _tmp700=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp52D.f1=_tmp700;});_tmp52D;});void*_tmp2B6[1U];_tmp2B6[0]=& _tmp2B8;({struct Cyc_Tcenv_Tenv*_tmp704=te;unsigned int _tmp703=loc;void**_tmp702=topt;struct _dyneither_ptr _tmp701=({const char*_tmp2B7="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2B7,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp704,_tmp703,_tmp702,_tmp701,_tag_dyneither(_tmp2B6,sizeof(void*),1U));});});}
# 1582
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1584
struct _tuple15 _tmp2B9=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2BA=_tmp2B9;unsigned int _tmp2C8;int _tmp2C7;_LL1: _tmp2C8=_tmp2BA.f1;_tmp2C7=_tmp2BA.f2;_LL2:;
if(!_tmp2C7)
return({void*_tmp2BB=0U;({struct Cyc_Tcenv_Tenv*_tmp707=te;unsigned int _tmp706=loc;struct _dyneither_ptr _tmp705=({const char*_tmp2BC="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2BC,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp707,_tmp706,0,_tmp705,_tag_dyneither(_tmp2BB,sizeof(void*),0U));});});{
# 1588
struct _handler_cons _tmp2BD;_push_handler(& _tmp2BD);{int _tmp2BF=0;if(setjmp(_tmp2BD.handler))_tmp2BF=1;if(!_tmp2BF){
{void*_tmp2C0=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2C8)).f2;_npop_handler(0U);return _tmp2C0;};_pop_handler();}else{void*_tmp2BE=(void*)_exn_thrown;void*_tmp2C1=_tmp2BE;void*_tmp2C6;if(((struct Cyc_List_Nth_exn_struct*)_tmp2C1)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1591
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2C4=({struct Cyc_Int_pa_PrintArg_struct _tmp52F;_tmp52F.tag=1U,_tmp52F.f1=(unsigned long)((int)_tmp2C8);_tmp52F;});struct Cyc_Int_pa_PrintArg_struct _tmp2C5=({struct Cyc_Int_pa_PrintArg_struct _tmp52E;_tmp52E.tag=1U,({
unsigned long _tmp708=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp52E.f1=_tmp708;});_tmp52E;});void*_tmp2C2[2U];_tmp2C2[0]=& _tmp2C4,_tmp2C2[1]=& _tmp2C5;({struct Cyc_Tcenv_Tenv*_tmp70B=te;unsigned int _tmp70A=loc;struct _dyneither_ptr _tmp709=({const char*_tmp2C3="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2C3,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp70B,_tmp70A,0,_tmp709,_tag_dyneither(_tmp2C2,sizeof(void*),2U));});});}else{_LL6: _tmp2C6=_tmp2C1;_LL7:(int)_rethrow(_tmp2C6);}_LL3:;}};};}
# 1596
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1598
struct Cyc_Tcenv_Tenv*_tmp2C9=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp70C=Cyc_Tcenv_clear_abstract_val_ok(_tmp2C9);Cyc_Tcexp_tcExp(_tmp70C,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp70D=Cyc_Tcenv_clear_abstract_val_ok(_tmp2C9);Cyc_Tcexp_tcExp(_tmp70D,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(_tmp2C9,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2CC=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,({
struct _dyneither_ptr _tmp70E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp530.f1=_tmp70E;});_tmp530;});void*_tmp2CA[1U];_tmp2CA[0]=& _tmp2CC;({struct Cyc_Tcenv_Tenv*_tmp712=_tmp2C9;unsigned int _tmp711=e2->loc;void**_tmp710=topt;struct _dyneither_ptr _tmp70F=({const char*_tmp2CB="expecting int subscript, found %s";_tag_dyneither(_tmp2CB,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp712,_tmp711,_tmp710,_tmp70F,_tag_dyneither(_tmp2CA,sizeof(void*),1U));});});{
# 1608
void*_tmp2CD=t1;struct Cyc_List_List*_tmp2F0;void*_tmp2EF;struct Cyc_Absyn_Tqual _tmp2EE;void*_tmp2ED;void*_tmp2EC;void*_tmp2EB;switch(*((int*)_tmp2CD)){case 3U: _LL1: _tmp2EF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CD)->f1).elt_type;_tmp2EE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CD)->f1).elt_tq;_tmp2ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CD)->f1).ptr_atts).rgn;_tmp2EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CD)->f1).ptr_atts).bounds;_tmp2EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CD)->f1).ptr_atts).zero_term;_LL2:
# 1612
 if(Cyc_Tcutil_force_type2bool(0,_tmp2EB)){
int emit_warning=0;
struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp2EC);
if(_tmp2CE != 0){
struct Cyc_Absyn_Exp*_tmp2CF=_tmp2CE;
struct _tuple15 _tmp2D0=Cyc_Evexp_eval_const_uint_exp(_tmp2CF);struct _tuple15 _tmp2D1=_tmp2D0;unsigned int _tmp2DD;int _tmp2DC;_LL8: _tmp2DD=_tmp2D1.f1;_tmp2DC=_tmp2D1.f2;_LL9:;
if(_tmp2DC  && _tmp2DD == 1)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2D2=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2D3=_tmp2D2;unsigned int _tmp2DB;int _tmp2DA;_LLB: _tmp2DB=_tmp2D3.f1;_tmp2DA=_tmp2D3.f2;_LLC:;
if(_tmp2DA){
struct _tuple15 _tmp2D4=Cyc_Evexp_eval_const_uint_exp(_tmp2CF);struct _tuple15 _tmp2D5=_tmp2D4;unsigned int _tmp2D9;int _tmp2D8;_LLE: _tmp2D9=_tmp2D5.f1;_tmp2D8=_tmp2D5.f2;_LLF:;
if((_tmp2DC  && _tmp2D9 > _tmp2DD) && _tmp2DD != 1)
({void*_tmp2D6=0U;({unsigned int _tmp714=loc;struct _dyneither_ptr _tmp713=({const char*_tmp2D7="subscript will fail at run-time";_tag_dyneither(_tmp2D7,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp714,_tmp713,_tag_dyneither(_tmp2D6,sizeof(void*),0U));});});}}}
# 1628
if(emit_warning)
({void*_tmp2DE=0U;({unsigned int _tmp716=e2->loc;struct _dyneither_ptr _tmp715=({const char*_tmp2DF="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp2DF,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp716,_tmp715,_tag_dyneither(_tmp2DE,sizeof(void*),0U));});});}else{
# 1632
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2E0=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2E1=_tmp2E0;unsigned int _tmp2E3;int _tmp2E2;_LL11: _tmp2E3=_tmp2E1.f1;_tmp2E2=_tmp2E1.f2;_LL12:;
if(_tmp2E2)
# 1637
({unsigned int _tmp719=loc;unsigned int _tmp718=_tmp2E3;void*_tmp717=_tmp2EC;Cyc_Tcutil_check_bound(_tmp719,_tmp718,_tmp717,Cyc_Tcenv_abstract_val_ok(_tmp2C9));});}else{
# 1640
if(Cyc_Tcutil_is_bound_one(_tmp2EC) && !Cyc_Tcutil_force_type2bool(0,_tmp2EB))
({void*_tmp2E4=0U;({unsigned int _tmp71B=e1->loc;struct _dyneither_ptr _tmp71A=({const char*_tmp2E5="subscript applied to pointer to one object";_tag_dyneither(_tmp2E5,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp71B,_tmp71A,_tag_dyneither(_tmp2E4,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2EC);}}
# 1645
Cyc_Tcenv_check_rgn_accessible(_tmp2C9,loc,_tmp2ED);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(_tmp2EF),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2C9))
({void*_tmp2E6=0U;({unsigned int _tmp71D=e1->loc;struct _dyneither_ptr _tmp71C=({const char*_tmp2E7="can't subscript an abstract pointer";_tag_dyneither(_tmp2E7,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp71D,_tmp71C,_tag_dyneither(_tmp2E6,sizeof(void*),0U));});});
return _tmp2EF;case 6U: _LL3: _tmp2F0=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2CD)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2C9,loc,_tmp2F0,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp2EA=({struct Cyc_String_pa_PrintArg_struct _tmp531;_tmp531.tag=0U,({struct _dyneither_ptr _tmp71E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp531.f1=_tmp71E;});_tmp531;});void*_tmp2E8[1U];_tmp2E8[0]=& _tmp2EA;({struct Cyc_Tcenv_Tenv*_tmp722=_tmp2C9;unsigned int _tmp721=loc;void**_tmp720=topt;struct _dyneither_ptr _tmp71F=({const char*_tmp2E9="subscript applied to %s";_tag_dyneither(_tmp2E9,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp722,_tmp721,_tmp720,_tmp71F,_tag_dyneither(_tmp2E8,sizeof(void*),1U));});});}_LL0:;};};}
# 1655
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp2F1=Cyc_Tcutil_compress(*topt);void*_tmp2F2=_tmp2F1;struct Cyc_List_List*_tmp2F6;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F2)->tag == 6U){_LL1: _tmp2F6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F2)->f1;_LL2:
# 1661
 if(({int _tmp723=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6);_tmp723 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1664
goto _LL0;
# 1666
for(0;es != 0;(es=es->tl,_tmp2F6=_tmp2F6->tl)){
int bogus=0;
void*_tmp2F3=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp2F6))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp724=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp724,& _tmp2F3,(struct Cyc_Absyn_Exp*)es->hd);});
# 1671
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp2F6->hd)).f2,& bogus);
fields=({struct Cyc_List_List*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));({struct _tuple17*_tmp725=({struct _tuple17*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->f1=(*((struct _tuple17*)_tmp2F6->hd)).f1,_tmp2F4->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2F4;});_tmp2F5->hd=_tmp725;}),_tmp2F5->tl=fields;_tmp2F5;});}
# 1674
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1678
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp726=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp726,0,(struct Cyc_Absyn_Exp*)es->hd);});
fields=({struct Cyc_List_List*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));({struct _tuple17*_tmp728=({struct _tuple17*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));({struct Cyc_Absyn_Tqual _tmp727=Cyc_Absyn_empty_tqual(0U);_tmp2F7->f1=_tmp727;}),_tmp2F7->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2F7;});_tmp2F8->hd=_tmp728;}),_tmp2F8->tl=fields;_tmp2F8;});}
# 1683
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->tag=6U,({struct Cyc_List_List*_tmp729=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp2F9->f1=_tmp729;});_tmp2F9;});}
# 1687
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1690
return({void*_tmp2FA=0U;({struct Cyc_Tcenv_Tenv*_tmp72D=te;unsigned int _tmp72C=loc;void**_tmp72B=topt;struct _dyneither_ptr _tmp72A=({const char*_tmp2FB="tcCompoundLit";_tag_dyneither(_tmp2FB,sizeof(char),14U);});Cyc_Tcexp_expr_err(_tmp72D,_tmp72C,_tmp72B,_tmp72A,_tag_dyneither(_tmp2FA,sizeof(void*),0U));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1700 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1703
void*res_t2;
int _tmp2FC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2FD=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->tag=0U,({union Cyc_Absyn_Cnst _tmp72E=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp2FC);_tmp313->f1=_tmp72E;});_tmp313;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp2FD,loc);
# 1711
if(zero_term){
struct Cyc_Absyn_Exp*_tmp2FE=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2FC - 1);
if(!Cyc_Tcutil_is_zero(_tmp2FE))
({void*_tmp2FF=0U;({unsigned int _tmp730=_tmp2FE->loc;struct _dyneither_ptr _tmp72F=({const char*_tmp300="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp300,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp730,_tmp72F,_tag_dyneither(_tmp2FF,sizeof(void*),0U));});});}
# 1716
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_tmp733=res;struct Cyc_Absyn_Tqual _tmp732=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1717
struct Cyc_Absyn_Exp*_tmp731=sz_exp;Cyc_Absyn_array_type(_tmp733,_tmp732,_tmp731,
# 1719
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1721
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1724
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1726
({struct Cyc_String_pa_PrintArg_struct _tmp303=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,({
struct _dyneither_ptr _tmp734=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));_tmp532.f1=_tmp734;});_tmp532;});void*_tmp301[1U];_tmp301[0]=& _tmp303;({unsigned int _tmp736=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp735=({const char*_tmp302="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp302,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp736,_tmp735,_tag_dyneither(_tmp301,sizeof(void*),1U));});});
# 1729
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1734
void*_tmp304=(void*)ds->hd;void*_tmp305=_tmp304;struct Cyc_Absyn_Exp*_tmp312;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp305)->tag == 1U){_LL1: _LL2:
# 1736
({void*_tmp306=0U;({unsigned int _tmp738=loc;struct _dyneither_ptr _tmp737=({const char*_tmp307="only array index designators are supported";_tag_dyneither(_tmp307,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp738,_tmp737,_tag_dyneither(_tmp306,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp312=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp305)->f1;_LL4:
# 1739
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp312);{
struct _tuple15 _tmp308=Cyc_Evexp_eval_const_uint_exp(_tmp312);struct _tuple15 _tmp309=_tmp308;unsigned int _tmp311;int _tmp310;_LL6: _tmp311=_tmp309.f1;_tmp310=_tmp309.f2;_LL7:;
if(!_tmp310)
({void*_tmp30A=0U;({unsigned int _tmp73A=_tmp312->loc;struct _dyneither_ptr _tmp739=({const char*_tmp30B="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp30B,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp73A,_tmp739,_tag_dyneither(_tmp30A,sizeof(void*),0U));});});else{
if(_tmp311 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp30E=({struct Cyc_Int_pa_PrintArg_struct _tmp534;_tmp534.tag=1U,_tmp534.f1=(unsigned long)offset;_tmp534;});struct Cyc_Int_pa_PrintArg_struct _tmp30F=({struct Cyc_Int_pa_PrintArg_struct _tmp533;_tmp533.tag=1U,_tmp533.f1=(unsigned long)((int)_tmp311);_tmp533;});void*_tmp30C[2U];_tmp30C[0]=& _tmp30E,_tmp30C[1]=& _tmp30F;({unsigned int _tmp73C=_tmp312->loc;struct _dyneither_ptr _tmp73B=({const char*_tmp30D="expecting index designator %d but found %d";_tag_dyneither(_tmp30D,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp73C,_tmp73B,_tag_dyneither(_tmp30C,sizeof(void*),2U));});});}
# 1746
goto _LL0;};}_LL0:;}}}
# 1750
return res_t2;}
# 1754
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1757
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp314=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp315=_tmp314;void*_tmp31A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp315)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp315)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp315)->f2 != 0){_LL1: _tmp31A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp315)->f2)->hd;_LL2:
# 1762
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp316=({void*_tmp73D=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp73D,Cyc_Absyn_valueof_exp(_tmp31A,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp316->topt=bound->topt;
# 1767
bound=_tmp316;}
# 1769
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1771
 if(!Cyc_Tcutil_coerce_uint_type(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp319=({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U,({
struct _dyneither_ptr _tmp73E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp535.f1=_tmp73E;});_tmp535;});void*_tmp317[1U];_tmp317[0]=& _tmp319;({unsigned int _tmp740=bound->loc;struct _dyneither_ptr _tmp73F=({const char*_tmp318="expecting unsigned int, found %s";_tag_dyneither(_tmp318,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp740,_tmp73F,_tag_dyneither(_tmp317,sizeof(void*),1U));});});}_LL0:;}
# 1776
if(!(vd->tq).real_const)
({void*_tmp31B=0U;({struct _dyneither_ptr _tmp741=({const char*_tmp31C="comprehension index variable is not declared const!";_tag_dyneither(_tmp31C,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp741,_tag_dyneither(_tmp31B,sizeof(void*),0U));});});
# 1779
if(te->le != 0)
te=Cyc_Tcenv_new_block(loc,te);{
void**_tmp31D=0;
struct Cyc_Absyn_Tqual*_tmp31E=0;
void**_tmp31F=0;
# 1785
if(topt != 0){
void*_tmp320=Cyc_Tcutil_compress(*topt);void*_tmp321=_tmp320;void*_tmp32C;struct Cyc_Absyn_Tqual _tmp32B;struct Cyc_Absyn_Exp*_tmp32A;void*_tmp329;struct Cyc_Absyn_PtrInfo _tmp328;switch(*((int*)_tmp321)){case 3U: _LL6: _tmp328=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp321)->f1;_LL7:
# 1788
 _tmp31D=({void**_tmp322=_cycalloc(sizeof(*_tmp322));*_tmp322=_tmp328.elt_type;_tmp322;});
_tmp31E=({struct Cyc_Absyn_Tqual*_tmp323=_cycalloc(sizeof(*_tmp323));*_tmp323=_tmp328.elt_tq;_tmp323;});
_tmp31F=({void**_tmp324=_cycalloc(sizeof(*_tmp324));*_tmp324=(_tmp328.ptr_atts).zero_term;_tmp324;});
goto _LL5;case 4U: _LL8: _tmp32C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp321)->f1).elt_type;_tmp32B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp321)->f1).tq;_tmp32A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp321)->f1).num_elts;_tmp329=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp321)->f1).zero_term;_LL9:
# 1793
 _tmp31D=({void**_tmp325=_cycalloc(sizeof(*_tmp325));*_tmp325=_tmp32C;_tmp325;});
_tmp31E=({struct Cyc_Absyn_Tqual*_tmp326=_cycalloc(sizeof(*_tmp326));*_tmp326=_tmp32B;_tmp326;});
_tmp31F=({void**_tmp327=_cycalloc(sizeof(*_tmp327));*_tmp327=_tmp329;_tmp327;});
goto _LL5;default: _LLA: _LLB:
# 1798
 goto _LL5;}_LL5:;}{
# 1801
void*t=Cyc_Tcexp_tcExp(te,_tmp31D,body);
# 1803
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp32D=0U;({unsigned int _tmp743=body->loc;struct _dyneither_ptr _tmp742=({const char*_tmp32E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp32E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp743,_tmp742,_tag_dyneither(_tmp32D,sizeof(void*),0U));});});
if(te->le == 0){
# 1807
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp32F=0U;({unsigned int _tmp745=bound->loc;struct _dyneither_ptr _tmp744=({const char*_tmp330="bound is not constant";_tag_dyneither(_tmp330,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp745,_tmp744,_tag_dyneither(_tmp32F,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp331=0U;({unsigned int _tmp747=bound->loc;struct _dyneither_ptr _tmp746=({const char*_tmp332="body is not constant";_tag_dyneither(_tmp332,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp747,_tmp746,_tag_dyneither(_tmp331,sizeof(void*),0U));});});}
# 1812
if(_tmp31F != 0  && Cyc_Tcutil_force_type2bool(0,*_tmp31F)){
# 1815
struct Cyc_Absyn_Exp*_tmp333=Cyc_Absyn_uint_exp(1U,0U);_tmp333->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,_tmp333,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1819
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp334=0U;({unsigned int _tmp749=body->loc;struct _dyneither_ptr _tmp748=({const char*_tmp335="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp335,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp749,_tmp748,_tag_dyneither(_tmp334,sizeof(void*),0U));});});{
# 1823
void*_tmp336=({void*_tmp74C=t;struct Cyc_Absyn_Tqual _tmp74B=_tmp31E == 0?Cyc_Absyn_empty_tqual(0U):*_tmp31E;struct Cyc_Absyn_Exp*_tmp74A=bound;Cyc_Absyn_array_type(_tmp74C,_tmp74B,_tmp74A,
_tmp31F == 0?Cyc_Absyn_false_type:*_tmp31F,0U);});
return _tmp336;};};};}
# 1829
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1832
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp337=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp338=_tmp337;void*_tmp33D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->f2 != 0){_LL1: _tmp33D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp338)->f2)->hd;_LL2:
# 1837
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp339=({void*_tmp74D=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp74D,Cyc_Absyn_valueof_exp(_tmp33D,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp339->topt=bound->topt;
# 1842
bound=_tmp339;}
# 1844
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1846
 if(!Cyc_Tcutil_coerce_uint_type(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp33C=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,({
struct _dyneither_ptr _tmp74E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp536.f1=_tmp74E;});_tmp536;});void*_tmp33A[1U];_tmp33A[0]=& _tmp33C;({unsigned int _tmp750=bound->loc;struct _dyneither_ptr _tmp74F=({const char*_tmp33B="expecting unsigned int, found %s";_tag_dyneither(_tmp33B,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp750,_tmp74F,_tag_dyneither(_tmp33A,sizeof(void*),1U));});});}_LL0:;}{
# 1851
void**_tmp33E=0;
struct Cyc_Absyn_Tqual*_tmp33F=0;
void**_tmp340=0;
# 1855
if(topt != 0){
void*_tmp341=Cyc_Tcutil_compress(*topt);void*_tmp342=_tmp341;void*_tmp34D;struct Cyc_Absyn_Tqual _tmp34C;struct Cyc_Absyn_Exp*_tmp34B;void*_tmp34A;struct Cyc_Absyn_PtrInfo _tmp349;switch(*((int*)_tmp342)){case 3U: _LL6: _tmp349=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp342)->f1;_LL7:
# 1858
 _tmp33E=({void**_tmp343=_cycalloc(sizeof(*_tmp343));*_tmp343=_tmp349.elt_type;_tmp343;});
_tmp33F=({struct Cyc_Absyn_Tqual*_tmp344=_cycalloc(sizeof(*_tmp344));*_tmp344=_tmp349.elt_tq;_tmp344;});
_tmp340=({void**_tmp345=_cycalloc(sizeof(*_tmp345));*_tmp345=(_tmp349.ptr_atts).zero_term;_tmp345;});
goto _LL5;case 4U: _LL8: _tmp34D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp342)->f1).elt_type;_tmp34C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp342)->f1).tq;_tmp34B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp342)->f1).num_elts;_tmp34A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp342)->f1).zero_term;_LL9:
# 1863
 _tmp33E=({void**_tmp346=_cycalloc(sizeof(*_tmp346));*_tmp346=_tmp34D;_tmp346;});
_tmp33F=({struct Cyc_Absyn_Tqual*_tmp347=_cycalloc(sizeof(*_tmp347));*_tmp347=_tmp34C;_tmp347;});
_tmp340=({void**_tmp348=_cycalloc(sizeof(*_tmp348));*_tmp348=_tmp34A;_tmp348;});
goto _LL5;default: _LLA: _LLB:
# 1868
 goto _LL5;}_LL5:;}
# 1871
({unsigned int _tmp753=loc;struct Cyc_Tcenv_Tenv*_tmp752=te;struct Cyc_List_List*_tmp751=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp753,_tmp752,_tmp751,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp33E != 0)Cyc_Tcutil_unify(*_tmp33E,t);
# 1874
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp34E=0U;({unsigned int _tmp755=bound->loc;struct _dyneither_ptr _tmp754=({const char*_tmp34F="bound is not constant";_tag_dyneither(_tmp34F,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp755,_tmp754,_tag_dyneither(_tmp34E,sizeof(void*),0U));});});}
# 1878
if(_tmp340 != 0  && Cyc_Tcutil_force_type2bool(0,*_tmp340)){
# 1881
struct Cyc_Absyn_Exp*_tmp350=Cyc_Absyn_uint_exp(1U,0U);_tmp350->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,_tmp350,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;
# 1885
({void*_tmp351=0U;({unsigned int _tmp757=loc;struct _dyneither_ptr _tmp756=({const char*_tmp352="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp352,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp757,_tmp756,_tag_dyneither(_tmp351,sizeof(void*),0U));});});}{
# 1888
void*_tmp353=({void*_tmp75A=t;struct Cyc_Absyn_Tqual _tmp759=_tmp33F == 0?Cyc_Absyn_empty_tqual(0U):*_tmp33F;struct Cyc_Absyn_Exp*_tmp758=bound;Cyc_Absyn_array_type(_tmp75A,_tmp759,_tmp758,
_tmp340 == 0?Cyc_Absyn_false_type:*_tmp340,0U);});
return _tmp353;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1903 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1907
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp354=_cycalloc(sizeof(*_tmp354));*_tmp354=ad;_tmp354;});}else{
# 1913
{struct _handler_cons _tmp355;_push_handler(& _tmp355);{int _tmp357=0;if(setjmp(_tmp355.handler))_tmp357=1;if(!_tmp357){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1913
;_pop_handler();}else{void*_tmp356=(void*)_exn_thrown;void*_tmp358=_tmp356;void*_tmp35C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp358)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1916
({struct Cyc_String_pa_PrintArg_struct _tmp35B=({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0U,({struct _dyneither_ptr _tmp75B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp537.f1=_tmp75B;});_tmp537;});void*_tmp359[1U];_tmp359[0]=& _tmp35B;({unsigned int _tmp75D=loc;struct _dyneither_ptr _tmp75C=({const char*_tmp35A="unbound struct/union name %s";_tag_dyneither(_tmp35A,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp75D,_tmp75C,_tag_dyneither(_tmp359,sizeof(void*),1U));});});
return topt != 0?*topt: Cyc_Absyn_void_type;}else{_LL3: _tmp35C=_tmp358;_LL4:(int)_rethrow(_tmp35C);}_LL0:;}};}
# 1919
*ad_opt=ad;
*tn=ad->name;}
# 1922
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp35F=({struct Cyc_String_pa_PrintArg_struct _tmp538;_tmp538.tag=0U,({struct _dyneither_ptr _tmp75E=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp360="struct";_tag_dyneither(_tmp360,sizeof(char),7U);}):({const char*_tmp361="union";_tag_dyneither(_tmp361,sizeof(char),6U);}));_tmp538.f1=_tmp75E;});_tmp538;});void*_tmp35D[1U];_tmp35D[0]=& _tmp35F;({unsigned int _tmp760=loc;struct _dyneither_ptr _tmp75F=({const char*_tmp35E="can't construct abstract %s";_tag_dyneither(_tmp35E,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp760,_tmp75F,_tag_dyneither(_tmp35D,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1930
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp362=Cyc_Tcenv_new_status(te);
if(_tmp362 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);else{
# 1939
te2=Cyc_Tcenv_set_new_status(_tmp362,te);}{
# 1941
struct _tuple12 _tmp363=({struct _tuple12 _tmp53E;({struct Cyc_List_List*_tmp761=Cyc_Tcenv_lookup_type_vars(te2);_tmp53E.f1=_tmp761;}),_tmp53E.f2=Cyc_Core_heap_region;_tmp53E;});
struct Cyc_List_List*_tmp364=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp363,ad->tvs);
struct Cyc_List_List*_tmp365=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp363,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp366=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp364);
struct Cyc_List_List*_tmp367=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp365);
struct Cyc_List_List*_tmp368=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp364,_tmp365);
void*res_typ;
# 1952
if(topt != 0){
void*_tmp369=Cyc_Tcutil_compress(*topt);void*_tmp36A=_tmp369;struct Cyc_Absyn_Aggrdecl**_tmp36D;struct Cyc_List_List*_tmp36C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp36D=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A)->f1)->f1).KnownAggr).val;_tmp36C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A)->f2;_LL7:
# 1955
 if(*_tmp36D == *adptr){
{struct Cyc_List_List*_tmp36B=_tmp366;for(0;_tmp36B != 0  && _tmp36C != 0;(
_tmp36B=_tmp36B->tl,_tmp36C=_tmp36C->tl)){
Cyc_Tcutil_unify((void*)_tmp36B->hd,(void*)_tmp36C->hd);}}
# 1960
res_typ=*topt;
goto _LL5;}
# 1963
goto _LL9;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1965
 res_typ=({union Cyc_Absyn_AggrInfo _tmp762=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp762,_tmp366);});}_LL5:;}else{
# 1968
res_typ=({union Cyc_Absyn_AggrInfo _tmp763=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_tmp763,_tmp366);});}{
# 1971
struct Cyc_List_List*_tmp36E=*ts;
struct Cyc_List_List*_tmp36F=_tmp367;
while(_tmp36E != 0  && _tmp36F != 0){
# 1975
({unsigned int _tmp766=loc;struct Cyc_Tcenv_Tenv*_tmp765=te2;struct Cyc_List_List*_tmp764=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp766,_tmp765,_tmp764,& Cyc_Tcutil_ak,1,0,(void*)_tmp36E->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp36E->hd);
Cyc_Tcutil_unify((void*)_tmp36E->hd,(void*)_tmp36F->hd);
_tmp36E=_tmp36E->tl;
_tmp36F=_tmp36F->tl;}
# 1981
if(_tmp36E != 0)
({void*_tmp370=0U;({unsigned int _tmp768=loc;struct _dyneither_ptr _tmp767=({const char*_tmp371="too many explicit witness types";_tag_dyneither(_tmp371,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp768,_tmp767,_tag_dyneither(_tmp370,sizeof(void*),0U));});});
# 1984
*ts=_tmp367;{
# 1987
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp372=(struct _tuple19*)fields->hd;struct _tuple19*_tmp373=_tmp372;struct Cyc_Absyn_Aggrfield*_tmp37F;struct Cyc_Absyn_Exp*_tmp37E;_LLB: _tmp37F=_tmp373->f1;_tmp37E=_tmp373->f2;_LLC:;{
void*_tmp374=Cyc_Tcutil_substitute(_tmp368,_tmp37F->type);
({struct Cyc_Tcenv_Tenv*_tmp769=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp769,& _tmp374,_tmp37E);});
# 1998
if(!Cyc_Tcutil_coerce_arg(te2,_tmp37E,_tmp374,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp377=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,_tmp53D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp37F->name);_tmp53D;});struct Cyc_String_pa_PrintArg_struct _tmp378=({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0U,({
struct _dyneither_ptr _tmp76A=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp37C="struct";_tag_dyneither(_tmp37C,sizeof(char),7U);}):({const char*_tmp37D="union";_tag_dyneither(_tmp37D,sizeof(char),6U);}));_tmp53C.f1=_tmp76A;});_tmp53C;});struct Cyc_String_pa_PrintArg_struct _tmp379=({struct Cyc_String_pa_PrintArg_struct _tmp53B;_tmp53B.tag=0U,({
struct _dyneither_ptr _tmp76B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp53B.f1=_tmp76B;});_tmp53B;});struct Cyc_String_pa_PrintArg_struct _tmp37A=({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U,({struct _dyneither_ptr _tmp76C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp374));_tmp53A.f1=_tmp76C;});_tmp53A;});struct Cyc_String_pa_PrintArg_struct _tmp37B=({struct Cyc_String_pa_PrintArg_struct _tmp539;_tmp539.tag=0U,({
struct _dyneither_ptr _tmp76D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp37E->topt)));_tmp539.f1=_tmp76D;});_tmp539;});void*_tmp375[5U];_tmp375[0]=& _tmp377,_tmp375[1]=& _tmp378,_tmp375[2]=& _tmp379,_tmp375[3]=& _tmp37A,_tmp375[4]=& _tmp37B;({unsigned int _tmp76F=_tmp37E->loc;struct _dyneither_ptr _tmp76E=({const char*_tmp376="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp376,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp76F,_tmp76E,_tag_dyneither(_tmp375,sizeof(void*),5U));});});
Cyc_Tcutil_explain_failure();}};}{
# 2007
struct Cyc_List_List*_tmp380=0;
{struct Cyc_List_List*_tmp381=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp381 != 0;_tmp381=_tmp381->tl){
_tmp380=({struct Cyc_List_List*_tmp383=_cycalloc(sizeof(*_tmp383));({struct _tuple0*_tmp772=({struct _tuple0*_tmp382=_cycalloc(sizeof(*_tmp382));({void*_tmp771=Cyc_Tcutil_substitute(_tmp368,(*((struct _tuple0*)_tmp381->hd)).f1);_tmp382->f1=_tmp771;}),({
void*_tmp770=Cyc_Tcutil_substitute(_tmp368,(*((struct _tuple0*)_tmp381->hd)).f2);_tmp382->f2=_tmp770;});_tmp382;});
# 2009
_tmp383->hd=_tmp772;}),_tmp383->tl=_tmp380;_tmp383;});}}
# 2012
_tmp380=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp380);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp380);
return res_typ;};};};};};}
# 2018
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2020
{void*_tmp384=Cyc_Tcutil_compress(ts);void*_tmp385=_tmp384;enum Cyc_Absyn_AggrKind _tmp394;struct Cyc_List_List*_tmp393;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp385)->tag == 7U){_LL1: _tmp394=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp385)->f1;_tmp393=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp385)->f2;_LL2:
# 2022
 if(_tmp394 == Cyc_Absyn_UnionA)
({void*_tmp386=0U;({unsigned int _tmp774=loc;struct _dyneither_ptr _tmp773=({const char*_tmp387="expecting struct but found union";_tag_dyneither(_tmp387,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp774,_tmp773,_tag_dyneither(_tmp386,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp393);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp388=(struct _tuple19*)fields->hd;struct _tuple19*_tmp389=_tmp388;struct Cyc_Absyn_Aggrfield*_tmp390;struct Cyc_Absyn_Exp*_tmp38F;_LL6: _tmp390=_tmp389->f1;_tmp38F=_tmp389->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp776=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp775=& _tmp390->type;Cyc_Tcexp_tcExpInitializer(_tmp776,_tmp775,_tmp38F);});
# 2031
if(!Cyc_Tcutil_coerce_arg(te,_tmp38F,_tmp390->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp38C=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,_tmp541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp390->name);_tmp541;});struct Cyc_String_pa_PrintArg_struct _tmp38D=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,({
struct _dyneither_ptr _tmp777=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp390->type));_tmp540.f1=_tmp777;});_tmp540;});struct Cyc_String_pa_PrintArg_struct _tmp38E=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,({
struct _dyneither_ptr _tmp778=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp38F->topt)));_tmp53F.f1=_tmp778;});_tmp53F;});void*_tmp38A[3U];_tmp38A[0]=& _tmp38C,_tmp38A[1]=& _tmp38D,_tmp38A[2]=& _tmp38E;({unsigned int _tmp77A=_tmp38F->loc;struct _dyneither_ptr _tmp779=({const char*_tmp38B="field %s of struct expects type %s != %s";_tag_dyneither(_tmp38B,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_dyneither(_tmp38A,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2038
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp391=0U;({struct _dyneither_ptr _tmp77B=({const char*_tmp392="tcAnonStruct: wrong type";_tag_dyneither(_tmp392,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp77B,_tag_dyneither(_tmp391,sizeof(void*),0U));});});}_LL0:;}
# 2041
return ts;}
# 2045
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2048
struct _tuple12 _tmp395=({struct _tuple12 _tmp547;({struct Cyc_List_List*_tmp77C=Cyc_Tcenv_lookup_type_vars(te);_tmp547.f1=_tmp77C;}),_tmp547.f2=Cyc_Core_heap_region;_tmp547;});
struct Cyc_List_List*_tmp396=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp395,tud->tvs);
struct Cyc_List_List*_tmp397=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp396);
void*res=({union Cyc_Absyn_DatatypeFieldInfo _tmp77D=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_tmp77D,_tmp397);});
# 2053
if(topt != 0){
void*_tmp398=Cyc_Tcutil_compress(*topt);void*_tmp399=_tmp398;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp399)->tag == 0U){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp399)->f1)->tag == 19U){_LL1: _LL2:
 Cyc_Tcutil_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2059
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp396 != 0)t=Cyc_Tcutil_substitute(_tmp396,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp39C=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,({
# 2069
struct _dyneither_ptr _tmp77E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp544.f1=_tmp77E;});_tmp544;});struct Cyc_String_pa_PrintArg_struct _tmp39D=({struct Cyc_String_pa_PrintArg_struct _tmp543;_tmp543.tag=0U,({struct _dyneither_ptr _tmp77F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp543.f1=_tmp77F;});_tmp543;});struct Cyc_String_pa_PrintArg_struct _tmp39E=({struct Cyc_String_pa_PrintArg_struct _tmp542;_tmp542.tag=0U,({
struct _dyneither_ptr _tmp780=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp39F="?";_tag_dyneither(_tmp39F,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp542.f1=_tmp780;});_tmp542;});void*_tmp39A[3U];_tmp39A[0]=& _tmp39C,_tmp39A[1]=& _tmp39D,_tmp39A[2]=& _tmp39E;({unsigned int _tmp782=e->loc;struct _dyneither_ptr _tmp781=({const char*_tmp39B="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp39B,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp782,_tmp781,_tag_dyneither(_tmp39A,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2074
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3A2=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,({
# 2077
struct _dyneither_ptr _tmp783=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp545.f1=_tmp783;});_tmp545;});void*_tmp3A0[1U];_tmp3A0[0]=& _tmp3A2;({struct Cyc_Tcenv_Tenv*_tmp787=te;unsigned int _tmp786=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp785=topt;struct _dyneither_ptr _tmp784=({const char*_tmp3A1="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3A1,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp787,_tmp786,_tmp785,_tmp784,_tag_dyneither(_tmp3A0,sizeof(void*),1U));});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3A5=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,({
struct _dyneither_ptr _tmp788=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp546.f1=_tmp788;});_tmp546;});void*_tmp3A3[1U];_tmp3A3[0]=& _tmp3A5;({struct Cyc_Tcenv_Tenv*_tmp78C=te;unsigned int _tmp78B=loc;void**_tmp78A=topt;struct _dyneither_ptr _tmp789=({const char*_tmp3A4="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3A4,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp78C,_tmp78B,_tmp78A,_tmp789,_tag_dyneither(_tmp3A3,sizeof(void*),1U));});});
return res;};}
# 2085
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2087
if(Cyc_Tcutil_is_bits_only_type(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return 1;
# 2089
if(topt != 0){
void*_tmp3A6=Cyc_Tcutil_compress(*topt);void*_tmp3A7=_tmp3A6;void*_tmp3A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A7)->tag == 3U){_LL1: _tmp3A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A7)->f1).elt_type;_LL2:
# 2092
 Cyc_Tcutil_unify(_tmp3A8,t);
if(Cyc_Tcutil_is_bits_only_type(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return 1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2098
return 0;}
# 2105
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2107
enum Cyc_Absyn_AliasQual _tmp3A9=(Cyc_Tcutil_type_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3AA=_tmp3A9;if(_tmp3AA == Cyc_Absyn_Unique){_LL1: _LL2:
 return Cyc_Absyn_unique_rgn_type;}else{_LL3: _LL4:
 return Cyc_Absyn_heap_rgn_type;}_LL0:;}
# 2113
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2118
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt != 0){
# 2122
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3AB=Cyc_Tcutil_compress(handle_type);void*_tmp3AC=_tmp3AB;void*_tmp3B0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f2 != 0){_LL1: _tmp3B0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f2)->hd;_LL2:
# 2127
 rgn=_tmp3B0;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2131
({struct Cyc_String_pa_PrintArg_struct _tmp3AF=({struct Cyc_String_pa_PrintArg_struct _tmp548;_tmp548.tag=0U,({
struct _dyneither_ptr _tmp78D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp548.f1=_tmp78D;});_tmp548;});void*_tmp3AD[1U];_tmp3AD[0]=& _tmp3AF;({unsigned int _tmp78F=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp78E=({const char*_tmp3AE="expecting region_t type but found %s";_tag_dyneither(_tmp3AE,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp78F,_tmp78E,_tag_dyneither(_tmp3AD,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2138
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2145
({struct Cyc_Tcenv_Tenv*_tmp790=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp790,& Cyc_Absyn_uint_type,*e);});{
# 2154 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3B1=0U;({struct _dyneither_ptr _tmp791=({const char*_tmp3B2="calloc with empty type";_tag_dyneither(_tmp3B2,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp791,_tag_dyneither(_tmp3B1,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp794=loc;struct Cyc_Tcenv_Tenv*_tmp793=te;struct Cyc_List_List*_tmp792=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp794,_tmp793,_tmp792,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp3B5=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,({struct _dyneither_ptr _tmp795=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(elt_type));_tmp549.f1=_tmp795;});_tmp549;});void*_tmp3B3[1U];_tmp3B3[0]=& _tmp3B5;({unsigned int _tmp797=loc;struct _dyneither_ptr _tmp796=({const char*_tmp3B4="calloc cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3B4,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp797,_tmp796,_tag_dyneither(_tmp3B3,sizeof(void*),1U));});});
num_elts=*e;
one_elt=0;}else{
# 2167
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3B6=er;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D0;void*_tmp3CF;switch(*((int*)_tmp3B6)){case 17U: _LL6: _tmp3CF=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3B6)->f1;_LL7:
# 2171
 elt_type=_tmp3CF;
({void**_tmp798=({void**_tmp3B7=_cycalloc(sizeof(*_tmp3B7));*_tmp3B7=elt_type;_tmp3B7;});*t=_tmp798;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3B6)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3B6)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3B6)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3B6)->f2)->tl)->tl == 0){_LL8: _tmp3D1=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3B6)->f2)->hd;_tmp3D0=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3B6)->f2)->tl)->hd;_LL9:
# 2178
{struct _tuple0 _tmp3B8=({struct _tuple0 _tmp54C;_tmp54C.f1=_tmp3D1->r,_tmp54C.f2=_tmp3D0->r;_tmp54C;});struct _tuple0 _tmp3B9=_tmp3B8;void*_tmp3C3;void*_tmp3C2;if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3B9.f1)->tag == 17U){_LLD: _tmp3C2=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3B9.f1)->f1;_LLE:
# 2181
 if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C2)){
# 2184
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,_tmp3C2))
({struct Cyc_String_pa_PrintArg_struct _tmp3BC=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,({struct _dyneither_ptr _tmp799=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3C2));_tmp54A.f1=_tmp799;});_tmp54A;});void*_tmp3BA[1U];_tmp3BA[0]=& _tmp3BC;({unsigned int _tmp79B=loc;struct _dyneither_ptr _tmp79A=({const char*_tmp3BB="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3BB,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp79B,_tmp79A,_tag_dyneither(_tmp3BA,sizeof(void*),1U));});});else{
# 2187
*is_calloc=1;}}
# 2190
elt_type=_tmp3C2;
({void**_tmp79C=({void**_tmp3BD=_cycalloc(sizeof(*_tmp3BD));*_tmp3BD=elt_type;_tmp3BD;});*t=_tmp79C;});
num_elts=_tmp3D0;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3B9.f2)->tag == 17U){_LLF: _tmp3C3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3B9.f2)->f1;_LL10:
# 2197
 if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C3)){
# 2200
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,_tmp3C3))
({struct Cyc_String_pa_PrintArg_struct _tmp3C0=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,({struct _dyneither_ptr _tmp79D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3C3));_tmp54B.f1=_tmp79D;});_tmp54B;});void*_tmp3BE[1U];_tmp3BE[0]=& _tmp3C0;({unsigned int _tmp79F=loc;struct _dyneither_ptr _tmp79E=({const char*_tmp3BF="malloc cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3BF,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp79F,_tmp79E,_tag_dyneither(_tmp3BE,sizeof(void*),1U));});});else{
# 2203
*is_calloc=1;}}
# 2206
elt_type=_tmp3C3;
({void**_tmp7A0=({void**_tmp3C1=_cycalloc(sizeof(*_tmp3C1));*_tmp3C1=elt_type;_tmp3C1;});*t=_tmp7A0;});
num_elts=_tmp3D1;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2213
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2215
 No_sizeof: {
# 2218
struct Cyc_Absyn_Exp*_tmp3C4=*e;
{void*_tmp3C5=_tmp3C4->r;void*_tmp3C6=_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C7;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C6)->tag == 14U){_LL14: _tmp3C7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3C6)->f2;_LL15:
 _tmp3C4=_tmp3C7;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2223
{void*_tmp3C8=Cyc_Tcutil_compress((void*)_check_null(_tmp3C4->topt));void*_tmp3C9=_tmp3C8;void*_tmp3CD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C9)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C9)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C9)->f2 != 0){_LL19: _tmp3CD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C9)->f2)->hd;_LL1A:
# 2225
{void*_tmp3CA=Cyc_Tcutil_compress(_tmp3CD);void*_tmp3CB=_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CC;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3CB)->tag == 9U){_LL1E: _tmp3CC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3CB)->f1;_LL1F:
# 2227
 er=_tmp3CC->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2230
goto _LL18;}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2233
elt_type=Cyc_Absyn_char_type;
({void**_tmp7A1=({void**_tmp3CE=_cycalloc(sizeof(*_tmp3CE));*_tmp3CE=elt_type;_tmp3CE;});*t=_tmp7A1;});
num_elts=*e;
one_elt=0;}
# 2238
goto _LL5;}_LL5:;}}
# 2242
*is_fat=!one_elt;
# 2245
{void*_tmp3D2=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3D5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D2)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D2)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D2)->f1)->f1).KnownAggr).tag == 2){_LL23: _tmp3D5=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D2)->f1)->f1).KnownAggr).val;_LL24:
# 2247
 if(_tmp3D5->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D5->impl))->exist_vars != 0)
({void*_tmp3D3=0U;({unsigned int _tmp7A3=loc;struct _dyneither_ptr _tmp7A2=({const char*_tmp3D4="malloc with existential types not yet implemented";_tag_dyneither(_tmp3D4,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp7A3,_tmp7A2,_tag_dyneither(_tmp3D3,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2254
void*(*_tmp3D6)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term)=Cyc_Absyn_at_type;
void*_tmp3D7=Cyc_Absyn_false_type;
if(topt != 0){
void*_tmp3D8=Cyc_Tcutil_compress(*topt);void*_tmp3D9=_tmp3D8;void*_tmp3E5;void*_tmp3E4;void*_tmp3E3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9)->tag == 3U){_LL28: _tmp3E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9)->f1).ptr_atts).nullable;_tmp3E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9)->f1).ptr_atts).bounds;_tmp3E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D9)->f1).ptr_atts).zero_term;_LL29:
# 2259
 _tmp3D7=_tmp3E3;
if(Cyc_Tcutil_force_type2bool(0,_tmp3E5))
_tmp3D6=Cyc_Absyn_star_type;
# 2264
if(Cyc_Tcutil_force_type2bool(0,_tmp3E3) && !(*is_calloc)){
({void*_tmp3DA=0U;({unsigned int _tmp7A5=loc;struct _dyneither_ptr _tmp7A4=({const char*_tmp3DB="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp3DB,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp7A5,_tmp7A4,_tag_dyneither(_tmp3DA,sizeof(void*),0U));});});
*is_calloc=1;}{
# 2270
struct Cyc_Absyn_Exp*_tmp3DC=({void*_tmp7A6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A6,_tmp3E4);});
if(_tmp3DC != 0  && !one_elt){
struct Cyc_Absyn_Exp*_tmp3DD=_tmp3DC;
int _tmp3DE=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp3DE  && Cyc_Evexp_same_const_exp(_tmp3DD,num_elts)){
*is_fat=0;
return({void*_tmp7AA=elt_type;void*_tmp7A9=rgn;struct Cyc_Absyn_Tqual _tmp7A8=Cyc_Absyn_empty_tqual(0U);void*_tmp7A7=_tmp3E4;Cyc_Absyn_atb_type(_tmp7AA,_tmp7A9,_tmp7A8,_tmp7A7,_tmp3D7);});}{
# 2278
void*_tmp3DF=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp3E0=_tmp3DF;void*_tmp3E2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->f2 != 0){_LL2D: _tmp3E2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->f2)->hd;_LL2E: {
# 2280
struct Cyc_Absyn_Exp*_tmp3E1=({void*_tmp7AB=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmp7AB,Cyc_Absyn_valueof_exp(_tmp3E2,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2282
if(Cyc_Evexp_same_const_exp(_tmp3E1,_tmp3DD)){
*is_fat=0;
return({void*_tmp7AF=elt_type;void*_tmp7AE=rgn;struct Cyc_Absyn_Tqual _tmp7AD=Cyc_Absyn_empty_tqual(0U);void*_tmp7AC=_tmp3E4;Cyc_Absyn_atb_type(_tmp7AF,_tmp7AE,_tmp7AD,_tmp7AC,_tmp3D7);});}
# 2286
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F: _LL30:
 goto _LL2C;}_LL2C:;};}
# 2290
goto _LL27;};}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2293
if(!one_elt)_tmp3D6=Cyc_Absyn_fatptr_type;
return({void*(*_tmp7B3)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term)=_tmp3D6;void*_tmp7B2=elt_type;void*_tmp7B1=rgn;struct Cyc_Absyn_Tqual _tmp7B0=Cyc_Absyn_empty_tqual(0U);_tmp7B3(_tmp7B2,_tmp7B1,_tmp7B0,_tmp3D7);});};};}
# 2298
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2300
struct Cyc_Tcenv_Tenv*_tmp3E6=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp3E6,0,e1);{
void*_tmp3E7=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp3E6,& _tmp3E7,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2307
{void*_tmp3E8=Cyc_Tcutil_compress(t1);void*_tmp3E9=_tmp3E8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9)->tag == 4U){_LL1: _LL2:
({void*_tmp3EA=0U;({unsigned int _tmp7B5=loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp3EB="cannot assign to an array";_tag_dyneither(_tmp3EB,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp3EA,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2312
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp3EC=0U;({unsigned int _tmp7B7=loc;struct _dyneither_ptr _tmp7B6=({const char*_tmp3ED="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp3ED,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp7B7,_tmp7B6,_tag_dyneither(_tmp3EC,sizeof(void*),0U));});});
# 2316
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp3EE=0U;({struct Cyc_Tcenv_Tenv*_tmp7BB=te;unsigned int _tmp7BA=e1->loc;void**_tmp7B9=topt;struct _dyneither_ptr _tmp7B8=({const char*_tmp3EF="swap non-lvalue";_tag_dyneither(_tmp3EF,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7BB,_tmp7BA,_tmp7B9,_tmp7B8,_tag_dyneither(_tmp3EE,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp3F0=0U;({struct Cyc_Tcenv_Tenv*_tmp7BF=te;unsigned int _tmp7BE=e2->loc;void**_tmp7BD=topt;struct _dyneither_ptr _tmp7BC=({const char*_tmp3F1="swap non-lvalue";_tag_dyneither(_tmp3F1,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7BF,_tmp7BE,_tmp7BD,_tmp7BC,_tag_dyneither(_tmp3F0,sizeof(void*),0U));});});
# 2321
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp3F2=({struct Cyc_String_pa_PrintArg_struct _tmp3F5=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,({
struct _dyneither_ptr _tmp7C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp54E.f1=_tmp7C0;});_tmp54E;});struct Cyc_String_pa_PrintArg_struct _tmp3F6=({struct Cyc_String_pa_PrintArg_struct _tmp54D;_tmp54D.tag=0U,({struct _dyneither_ptr _tmp7C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp54D.f1=_tmp7C1;});_tmp54D;});void*_tmp3F3[2U];_tmp3F3[0]=& _tmp3F5,_tmp3F3[1]=& _tmp3F6;({struct Cyc_Tcenv_Tenv*_tmp7C5=te;unsigned int _tmp7C4=loc;void**_tmp7C3=topt;struct _dyneither_ptr _tmp7C2=({const char*_tmp3F4="type mismatch: %s != %s";_tag_dyneither(_tmp3F4,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp7C5,_tmp7C4,_tmp7C3,_tmp7C2,_tag_dyneither(_tmp3F3,sizeof(void*),2U));});});
return _tmp3F2;}
# 2328
return Cyc_Absyn_void_type;};};}
# 2333
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp7C6=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp7C6,s,1);});
# 2336
while(1){
void*_tmp3F7=s->r;void*_tmp3F8=_tmp3F7;struct Cyc_Absyn_Decl*_tmp403;struct Cyc_Absyn_Stmt*_tmp402;struct Cyc_Absyn_Stmt*_tmp401;struct Cyc_Absyn_Stmt*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;switch(*((int*)_tmp3F8)){case 1U: _LL1: _tmp3FF=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3F8)->f1;_LL2: {
# 2340
void*_tmp3F9=(void*)_check_null(_tmp3FF->topt);
if(!({void*_tmp7C7=_tmp3F9;Cyc_Tcutil_unify(_tmp7C7,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp3FC=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({
struct _dyneither_ptr _tmp7C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3F9));_tmp54F.f1=_tmp7C8;});_tmp54F;});void*_tmp3FA[1U];_tmp3FA[0]=& _tmp3FC;({unsigned int _tmp7CA=loc;struct _dyneither_ptr _tmp7C9=({const char*_tmp3FB="statement expression returns type %s";_tag_dyneither(_tmp3FB,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7CA,_tmp7C9,_tag_dyneither(_tmp3FA,sizeof(void*),1U));});});
Cyc_Tcutil_explain_failure();}
# 2346
return _tmp3F9;}case 2U: _LL3: _tmp401=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F8)->f1;_tmp400=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3F8)->f2;_LL4:
 s=_tmp400;continue;case 12U: _LL5: _tmp403=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F8)->f1;_tmp402=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3F8)->f2;_LL6:
 s=_tmp402;continue;default: _LL7: _LL8:
# 2350
 return({void*_tmp3FD=0U;({struct Cyc_Tcenv_Tenv*_tmp7CE=te;unsigned int _tmp7CD=loc;void**_tmp7CC=topt;struct _dyneither_ptr _tmp7CB=({const char*_tmp3FE="statement expression must end with expression";_tag_dyneither(_tmp3FE,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7CE,_tmp7CD,_tmp7CC,_tmp7CB,_tag_dyneither(_tmp3FD,sizeof(void*),0U));});});}_LL0:;}}
# 2355
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp7CF=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7CF,0,e);}));
{void*_tmp404=t;struct Cyc_Absyn_Aggrdecl*_tmp408;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp404)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp404)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp404)->f1)->f1).KnownAggr).tag == 2){_LL1: _tmp408=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp404)->f1)->f1).KnownAggr).val;_LL2:
# 2359
 if((_tmp408->kind == Cyc_Absyn_UnionA  && _tmp408->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp408->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2362
({struct Cyc_String_pa_PrintArg_struct _tmp407=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({struct _dyneither_ptr _tmp7D0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp550.f1=_tmp7D0;});_tmp550;});void*_tmp405[1U];_tmp405[0]=& _tmp407;({unsigned int _tmp7D2=loc;struct _dyneither_ptr _tmp7D1=({const char*_tmp406="expecting @tagged union but found %s";_tag_dyneither(_tmp406,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7D2,_tmp7D1,_tag_dyneither(_tmp405,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2365
return Cyc_Absyn_uint_type;}
# 2369
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2373
void*rgn=Cyc_Absyn_heap_rgn_type;
struct Cyc_Tcenv_Tenv*_tmp409=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2378
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp409)));
void*handle_type=Cyc_Tcexp_tcExp(_tmp409,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp40A=Cyc_Tcutil_compress(handle_type);void*_tmp40B=_tmp40A;void*_tmp40F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40B)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40B)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40B)->f2 != 0){_LL1: _tmp40F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40B)->f2)->hd;_LL2:
# 2383
 rgn=_tmp40F;
Cyc_Tcenv_check_rgn_accessible(_tmp409,loc,rgn);
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2387
({struct Cyc_String_pa_PrintArg_struct _tmp40E=({struct Cyc_String_pa_PrintArg_struct _tmp551;_tmp551.tag=0U,({
struct _dyneither_ptr _tmp7D3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp551.f1=_tmp7D3;});_tmp551;});void*_tmp40C[1U];_tmp40C[0]=& _tmp40E;({unsigned int _tmp7D5=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp7D4=({const char*_tmp40D="expecting region_t type but found %s";_tag_dyneither(_tmp40D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7D5,_tmp7D4,_tag_dyneither(_tmp40C,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2394
if(topt != 0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == Cyc_Absyn_unique_rgn_type)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2402
void*_tmp410=e1->r;void*_tmp411=_tmp410;struct Cyc_List_List*_tmp429;struct Cyc_Core_Opt*_tmp428;struct Cyc_List_List*_tmp427;switch(*((int*)_tmp411)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2407
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp409,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp412=0U;({struct _dyneither_ptr _tmp7D6=({const char*_tmp413="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp413,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D6,_tag_dyneither(_tmp412,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp409,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp409,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2418
return res_typ;}case 36U: _LLA: _tmp428=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp411)->f1;_tmp427=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp411)->f2;_LLB:
# 2420
({void*_tmp7D7=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->tag=26U,_tmp414->f1=_tmp427;_tmp414;});e1->r=_tmp7D7;});
_tmp429=_tmp427;goto _LLD;case 26U: _LLC: _tmp429=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp411)->f1;_LLD: {
# 2423
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp415=Cyc_Tcutil_compress(*topt);void*_tmp416=_tmp415;void**_tmp419;struct Cyc_Absyn_Tqual _tmp418;void*_tmp417;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->tag == 3U){_LL15: _tmp419=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).elt_type;_tmp418=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).elt_tq;_tmp417=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).ptr_atts).zero_term;_LL16:
# 2429
 elt_typ_opt=_tmp419;
zero_term=Cyc_Tcutil_force_type2bool(0,_tmp417);
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2435
void*res_typ=Cyc_Tcexp_tcArray(_tmp409,e1->loc,elt_typ_opt,0,zero_term,_tmp429);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({void*_tmp41A=0U;({struct _dyneither_ptr _tmp7D8=({const char*_tmp41B="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp41B,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7D8,_tag_dyneither(_tmp41A,sizeof(void*),0U));});});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2444
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp409,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp409,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2450
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp411)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2455
void*_tmp41C=({void*_tmp7DC=Cyc_Absyn_char_type;void*_tmp7DB=rgn;struct Cyc_Absyn_Tqual _tmp7DA=Cyc_Absyn_const_tqual(0U);void*_tmp7D9=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp7DC,_tmp7DB,_tmp7DA,_tmp7D9,Cyc_Absyn_true_type);});
# 2457
void*_tmp41D=Cyc_Tcexp_tcExp(_tmp409,& _tmp41C,e1);
return({void*_tmp7E0=_tmp41D;void*_tmp7DF=rgn;struct Cyc_Absyn_Tqual _tmp7DE=Cyc_Absyn_empty_tqual(0U);void*_tmp7DD=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp7E0,_tmp7DF,_tmp7DE,_tmp7DD,Cyc_Absyn_false_type);});}case 9U: _LL10: _LL11: {
# 2462
void*_tmp41E=({void*_tmp7E4=Cyc_Absyn_wchar_type();void*_tmp7E3=rgn;struct Cyc_Absyn_Tqual _tmp7E2=Cyc_Absyn_const_tqual(0U);void*_tmp7E1=Cyc_Absyn_fat_bound_type;Cyc_Absyn_atb_type(_tmp7E4,_tmp7E3,_tmp7E2,_tmp7E1,Cyc_Absyn_true_type);});
# 2464
void*_tmp41F=Cyc_Tcexp_tcExp(_tmp409,& _tmp41E,e1);
return({void*_tmp7E8=_tmp41F;void*_tmp7E7=rgn;struct Cyc_Absyn_Tqual _tmp7E6=Cyc_Absyn_empty_tqual(0U);void*_tmp7E5=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));Cyc_Absyn_atb_type(_tmp7E8,_tmp7E7,_tmp7E6,_tmp7E5,Cyc_Absyn_false_type);});}default: goto _LL12;}default: _LL12: _LL13:
# 2471
 RG: {
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt != 0){
void*_tmp420=Cyc_Tcutil_compress(*topt);void*_tmp421=_tmp420;void**_tmp423;struct Cyc_Absyn_Tqual _tmp422;switch(*((int*)_tmp421)){case 3U: _LL1A: _tmp423=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp421)->f1).elt_type;_tmp422=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp421)->f1).elt_tq;_LL1B:
# 2477
 topt2=_tmp423;goto _LL19;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp421)->f1)->tag == 18U){_LL1C: _LL1D:
# 2481
 bogus=*topt;
topt2=& bogus;
goto _LL19;}else{goto _LL1E;}default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2487
void*telt=Cyc_Tcexp_tcExp(_tmp409,topt2,e1);
# 2489
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp424=0U;({unsigned int _tmp7EA=e1->loc;struct _dyneither_ptr _tmp7E9=({const char*_tmp425="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp425,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp7EA,_tmp7E9,_tag_dyneither(_tmp424,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp426=_cycalloc(sizeof(*_tmp426));
_tmp426->tag=3U,(_tmp426->f1).elt_type=telt,({struct Cyc_Absyn_Tqual _tmp7ED=Cyc_Absyn_empty_tqual(0U);(_tmp426->f1).elt_tq=_tmp7ED;}),
((_tmp426->f1).ptr_atts).rgn=rgn,({void*_tmp7EC=Cyc_Tcutil_any_bool(& _tmp409);((_tmp426->f1).ptr_atts).nullable=_tmp7EC;}),({
void*_tmp7EB=Cyc_Absyn_bounds_one();((_tmp426->f1).ptr_atts).bounds=_tmp7EB;}),((_tmp426->f1).ptr_atts).zero_term=Cyc_Absyn_false_type,((_tmp426->f1).ptr_atts).ptrloc=0;_tmp426;});
# 2496
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp409,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp409,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2503
return res_typ;};};}}_LL5:;};}
# 2509
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array_type(t))
({void*_tmp7EF=t=({void*_tmp7EE=t;Cyc_Tcutil_promote_array(_tmp7EE,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});e->topt=_tmp7EF;});
return t;}
# 2519
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2523
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2528
({void*_tmp42A=0U;({unsigned int _tmp7F1=e->loc;struct _dyneither_ptr _tmp7F0=({const char*_tmp42B="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp42B,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp7F1,_tmp7F0,_tag_dyneither(_tmp42A,sizeof(void*),0U));});});{
# 2530
void*_tmp42C=e->r;void*_tmp42D=_tmp42C;switch(*((int*)_tmp42D)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp42D)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2537
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array_type(t)){
t=({void*_tmp7F2=t;Cyc_Tcutil_promote_array(_tmp7F2,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2542
return t;}_LL0:;};}
# 2553 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp42E=e->r;void*_tmp42F=_tmp42E;struct Cyc_Absyn_Exp*_tmp44D;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp42F)->tag == 12U){_LL1: _tmp44D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp42F)->f1;_LL2:
# 2557
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp44D);
({void*_tmp7F3=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp44D->topt),0);_tmp44D->topt=_tmp7F3;});
e->topt=_tmp44D->topt;
goto _LL0;}else{_LL3: _LL4:
# 2563
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp7F4=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp7F4;});
# 2566
{void*_tmp430=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp431=_tmp430;void*_tmp44C;struct Cyc_Absyn_Tqual _tmp44B;void*_tmp44A;void*_tmp449;void*_tmp448;void*_tmp447;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431)->tag == 3U){_LL6: _tmp44C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431)->f1).elt_type;_tmp44B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431)->f1).elt_tq;_tmp44A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431)->f1).ptr_atts).rgn;_tmp449=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431)->f1).ptr_atts).nullable;_tmp448=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431)->f1).ptr_atts).bounds;_tmp447=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431)->f1).ptr_atts).zero_term;_LL7:
# 2568
{void*_tmp432=Cyc_Tcutil_compress(_tmp44C);void*_tmp433=_tmp432;struct Cyc_List_List*_tmp446;void*_tmp445;struct Cyc_Absyn_Tqual _tmp444;void*_tmp443;struct Cyc_List_List*_tmp442;int _tmp441;struct Cyc_Absyn_VarargInfo*_tmp440;struct Cyc_List_List*_tmp43F;struct Cyc_List_List*_tmp43E;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_List_List*_tmp43C;struct Cyc_Absyn_Exp*_tmp43B;struct Cyc_List_List*_tmp43A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->tag == 5U){_LLB: _tmp446=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).tvars;_tmp445=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).effect;_tmp444=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).ret_tqual;_tmp443=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).ret_type;_tmp442=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).args;_tmp441=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).c_varargs;_tmp440=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).cyc_varargs;_tmp43F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).rgn_po;_tmp43E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).attributes;_tmp43D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).requires_clause;_tmp43C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).requires_relns;_tmp43B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).ensures_clause;_tmp43A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp433)->f1).ensures_relns;_LLC:
# 2570
 if(_tmp446 != 0){
struct _tuple12 _tmp434=({struct _tuple12 _tmp552;({struct Cyc_List_List*_tmp7F5=Cyc_Tcenv_lookup_type_vars(te);_tmp552.f1=_tmp7F5;}),_tmp552.f2=Cyc_Core_heap_region;_tmp552;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp434,_tmp446);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2578
_tmp43F=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp43F);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp43F);{
void*ftyp=({struct Cyc_List_List*_tmp7F6=inst;Cyc_Tcutil_substitute(_tmp7F6,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp439=_cycalloc(sizeof(*_tmp439));
_tmp439->tag=5U,(_tmp439->f1).tvars=0,(_tmp439->f1).effect=_tmp445,(_tmp439->f1).ret_tqual=_tmp444,(_tmp439->f1).ret_type=_tmp443,(_tmp439->f1).args=_tmp442,(_tmp439->f1).c_varargs=_tmp441,(_tmp439->f1).cyc_varargs=_tmp440,(_tmp439->f1).rgn_po=0,(_tmp439->f1).attributes=_tmp43E,(_tmp439->f1).requires_clause=_tmp43D,(_tmp439->f1).requires_relns=_tmp43C,(_tmp439->f1).ensures_clause=_tmp43B,(_tmp439->f1).ensures_relns=_tmp43A;_tmp439;}));});
# 2585
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp435=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=3U,(_tmp438->f1).elt_type=ftyp,(_tmp438->f1).elt_tq=_tmp44B,((_tmp438->f1).ptr_atts).rgn=_tmp44A,((_tmp438->f1).ptr_atts).nullable=_tmp449,((_tmp438->f1).ptr_atts).bounds=_tmp448,((_tmp438->f1).ptr_atts).zero_term=_tmp447,((_tmp438->f1).ptr_atts).ptrloc=0;_tmp438;});
# 2587
struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_copy_exp(e);
({void*_tmp7F7=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=13U,_tmp437->f1=_tmp436,_tmp437->f2=ts;_tmp437;});e->r=_tmp7F7;});
e->topt=(void*)_tmp435;};}
# 2591
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2594
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2597
goto _LL0;}_LL0:;}
# 2599
return(void*)_check_null(e->topt);}
# 2607
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2609
struct Cyc_List_List*_tmp44E=0;
# 2611
{void*_tmp44F=fn_exp->r;void*_tmp450=_tmp44F;struct Cyc_List_List*_tmp464;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp450)->tag == 10U){_LL1: _tmp464=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp450)->f2;_LL2:
# 2613
{void*_tmp451=e->r;void*_tmp452=_tmp451;struct Cyc_List_List*_tmp461;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp452)->tag == 10U){_LL6: _tmp461=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp452)->f2;_LL7: {
# 2615
struct Cyc_List_List*_tmp453=alias_arg_exps;
int _tmp454=0;
while(_tmp453 != 0){
while(_tmp454 != (int)_tmp453->hd){
if(_tmp464 == 0)
({void*_tmp455=0U;({struct _dyneither_ptr _tmp7F9=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp458=({struct Cyc_Int_pa_PrintArg_struct _tmp554;_tmp554.tag=1U,_tmp554.f1=(unsigned long)_tmp454;_tmp554;});struct Cyc_Int_pa_PrintArg_struct _tmp459=({struct Cyc_Int_pa_PrintArg_struct _tmp553;_tmp553.tag=1U,_tmp553.f1=(unsigned long)((int)_tmp453->hd);_tmp553;});void*_tmp456[2U];_tmp456[0]=& _tmp458,_tmp456[1]=& _tmp459;({struct _dyneither_ptr _tmp7F8=({const char*_tmp457="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp457,sizeof(char),36U);});Cyc_aprintf(_tmp7F8,_tag_dyneither(_tmp456,sizeof(void*),2U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F9,_tag_dyneither(_tmp455,sizeof(void*),0U));});});
# 2622
++ _tmp454;
_tmp464=_tmp464->tl;
_tmp461=((struct Cyc_List_List*)_check_null(_tmp461))->tl;}{
# 2627
struct _tuple11 _tmp45A=({struct Cyc_Absyn_Exp*_tmp7FA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp464))->hd;Cyc_Tcutil_insert_alias(_tmp7FA,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp461))->hd)->topt)));});struct _tuple11 _tmp45B=_tmp45A;struct Cyc_Absyn_Decl*_tmp45E;struct Cyc_Absyn_Exp*_tmp45D;_LLB: _tmp45E=_tmp45B.f1;_tmp45D=_tmp45B.f2;_LLC:;
_tmp464->hd=(void*)_tmp45D;
_tmp44E=({struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->hd=_tmp45E,_tmp45C->tl=_tmp44E;_tmp45C;});
_tmp453=_tmp453->tl;};}
# 2632
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp45F=0U;({struct _dyneither_ptr _tmp7FB=({const char*_tmp460="not a function call!";_tag_dyneither(_tmp460,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7FB,_tag_dyneither(_tmp45F,sizeof(void*),0U));});});}_LL5:;}
# 2635
goto _LL0;}else{_LL3: _LL4:
({void*_tmp462=0U;({struct _dyneither_ptr _tmp7FC=({const char*_tmp463="not a function call!";_tag_dyneither(_tmp463,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7FC,_tag_dyneither(_tmp462,sizeof(void*),0U));});});}_LL0:;}
# 2640
while(_tmp44E != 0){
struct Cyc_Absyn_Decl*_tmp465=(struct Cyc_Absyn_Decl*)_tmp44E->hd;
fn_exp=({struct Cyc_Absyn_Stmt*_tmp7FF=({struct Cyc_Absyn_Decl*_tmp7FE=_tmp465;struct Cyc_Absyn_Stmt*_tmp7FD=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp7FE,_tmp7FD,e->loc);});Cyc_Absyn_stmt_exp(_tmp7FF,e->loc);});
_tmp44E=_tmp44E->tl;}
# 2647
e->topt=0;
e->r=fn_exp->r;}
# 2653
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2657
{void*_tmp466=e->r;void*_tmp467=_tmp466;void*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Exp*_tmp4CE;struct _dyneither_ptr*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CB;int*_tmp4CA;struct Cyc_Absyn_Exp**_tmp4C9;void***_tmp4C8;struct Cyc_Absyn_Exp**_tmp4C7;int*_tmp4C6;void*_tmp4C5;struct Cyc_Absyn_Enumfield*_tmp4C4;struct Cyc_Absyn_Enumdecl*_tmp4C3;struct Cyc_Absyn_Enumfield*_tmp4C2;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_Datatypedecl*_tmp4C0;struct Cyc_Absyn_Datatypefield*_tmp4BF;void*_tmp4BE;struct Cyc_List_List*_tmp4BD;struct _tuple1**_tmp4BC;struct Cyc_List_List**_tmp4BB;struct Cyc_List_List*_tmp4BA;struct Cyc_Absyn_Aggrdecl**_tmp4B9;struct Cyc_Absyn_Exp*_tmp4B8;void*_tmp4B7;int*_tmp4B6;struct Cyc_Absyn_Vardecl*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B3;int*_tmp4B2;struct Cyc_Absyn_Stmt*_tmp4B1;struct Cyc_List_List*_tmp4B0;struct _tuple9*_tmp4AF;struct Cyc_List_List*_tmp4AE;struct Cyc_List_List*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;struct _dyneither_ptr*_tmp4A9;int*_tmp4A8;int*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct _dyneither_ptr*_tmp4A5;int*_tmp4A4;int*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A2;void*_tmp4A1;struct Cyc_List_List*_tmp4A0;void*_tmp49F;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp**_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49B;void*_tmp49A;struct Cyc_Absyn_Exp*_tmp499;enum Cyc_Absyn_Coercion*_tmp498;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_List_List*_tmp496;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Core_Opt*_tmp48A;struct Cyc_Absyn_Exp*_tmp489;struct Cyc_Absyn_Exp*_tmp488;enum Cyc_Absyn_Incrementor _tmp487;enum Cyc_Absyn_Primop _tmp486;struct Cyc_List_List*_tmp485;void**_tmp484;union Cyc_Absyn_Cnst*_tmp483;struct Cyc_Core_Opt*_tmp482;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_List_List*_tmp47F;struct Cyc_Absyn_VarargCallInfo**_tmp47E;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_List_List*_tmp47C;struct Cyc_Absyn_VarargCallInfo**_tmp47B;int*_tmp47A;struct Cyc_Absyn_Exp*_tmp479;switch(*((int*)_tmp467)){case 12U: _LL1: _tmp479=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL2:
# 2662
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp479);
return;case 10U: _LL3: _tmp47D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp47C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp47B=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_tmp47A=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp467)->f4;if(!(*_tmp47A)){_LL4:
# 2666
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp800=({const char*_tmp468="unresolved function in tcExpNoInst";_tag_dyneither(_tmp468,sizeof(char),35U);});_tmp469->f1=_tmp800;});_tmp469;}));}else{_LL1B: _tmp480=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp47F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp47E=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_LL1C: {
# 2699
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp46A;_push_handler(& _tmp46A);{int _tmp46C=0;if(setjmp(_tmp46A.handler))_tmp46C=1;if(!_tmp46C){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp46B=(void*)_exn_thrown;void*_tmp46D=_tmp46B;void*_tmp46E;if(((struct Cyc_Core_Failure_exn_struct*)_tmp46D)->tag == Cyc_Core_Failure){_LL58: _LL59:
# 2706
 ok=0;
fn_exp=e;
goto _LL57;}else{_LL5A: _tmp46E=_tmp46D;_LL5B:(int)_rethrow(_tmp46E);}_LL57:;}};}
# 2710
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp480,_tmp47F,_tmp47E,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2717
goto _LL0;}}case 36U: _LL5: _tmp482=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp481=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL6:
# 2670
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp481);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp483=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL8:
# 2675
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp483,e);goto _LL0;case 1U: _LL9: _tmp484=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LLA:
# 2677
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp484);goto _LL0;case 2U: _LLB: _LLC:
# 2679
 t=Cyc_Absyn_sint_type;goto _LL0;case 3U: _LLD: _tmp486=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp485=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LLE:
# 2681
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp486,_tmp485);goto _LL0;case 5U: _LLF: _tmp488=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp487=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL10:
# 2683
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp488,_tmp487);goto _LL0;case 4U: _LL11: _tmp48B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp48A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp489=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_LL12:
# 2685
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp48B,_tmp48A,_tmp489);goto _LL0;case 6U: _LL13: _tmp48E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp48D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp48C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_LL14:
# 2687
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp48E,_tmp48D,_tmp48C);goto _LL0;case 7U: _LL15: _tmp490=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp48F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL16:
# 2689
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp490,_tmp48F);goto _LL0;case 8U: _LL17: _tmp492=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp491=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL18:
# 2691
 t=Cyc_Tcexp_tcOr(te,loc,_tmp492,_tmp491);goto _LL0;case 9U: _LL19: _tmp494=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp493=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL1A:
# 2693
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp494,_tmp493);goto _LL0;case 11U: _LL1D: _tmp495=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL1E:
# 2719
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp495);goto _LL0;case 13U: _LL1F: _tmp497=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp496=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL20:
# 2721
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp497,_tmp496);goto _LL0;case 14U: _LL21: _tmp49A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp499=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp498=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp467)->f4;_LL22:
# 2723
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp49A,_tmp499,_tmp498);goto _LL0;case 15U: _LL23: _tmp49B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL24:
# 2725
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp49B);goto _LL0;case 16U: _LL25: _tmp49D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp49C=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL26:
# 2727
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp49D,e,_tmp49C);goto _LL0;case 18U: _LL27: _tmp49E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL28: {
# 2729
void*_tmp46F=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp49E);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp46F);goto _LL0;}case 17U: _LL29: _tmp49F=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL2A:
# 2732
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp49F);goto _LL0;case 19U: _LL2B: _tmp4A1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4A0=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL2C:
# 2734
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4A1,_tmp4A0);goto _LL0;case 20U: _LL2D: _tmp4A2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL2E:
# 2736
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4A2);goto _LL0;case 21U: _LL2F: _tmp4A6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4A5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp4A4=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_tmp4A3=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp467)->f4;_LL30:
# 2738
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4A6,_tmp4A5,_tmp4A4,_tmp4A3);goto _LL0;case 22U: _LL31: _tmp4AA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4A9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp4A8=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_tmp4A7=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp467)->f4;_LL32:
# 2740
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4AA,_tmp4A9,_tmp4A8,_tmp4A7);goto _LL0;case 23U: _LL33: _tmp4AC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4AB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL34:
# 2742
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4AC,_tmp4AB);goto _LL0;case 24U: _LL35: _tmp4AD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL36:
# 2744
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4AD);goto _LL0;case 25U: _LL37: _tmp4AF=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4AE=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL38:
# 2746
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4AF,_tmp4AE);goto _LL0;case 26U: _LL39: _tmp4B0=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL3A: {
# 2750
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp470=Cyc_Tcutil_compress(*topt);void*_tmp471=_tmp470;void**_tmp474;struct Cyc_Absyn_Tqual*_tmp473;void*_tmp472;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp471)->tag == 4U){_LL5D: _tmp474=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp471)->f1).elt_type;_tmp473=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp471)->f1).tq;_tmp472=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp471)->f1).zero_term;_LL5E:
# 2756
 elt_topt=_tmp474;
elt_tqopt=_tmp473;
zero_term=Cyc_Tcutil_force_type2bool(0,_tmp472);
goto _LL5C;}else{_LL5F: _LL60:
 goto _LL5C;}_LL5C:;}
# 2763
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4B0);goto _LL0;}case 37U: _LL3B: _tmp4B1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL3C:
# 2765
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4B1);goto _LL0;case 27U: _LL3D: _tmp4B5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4B4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp4B3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_tmp4B2=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp467)->f4;_LL3E:
# 2767
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4B5,_tmp4B4,_tmp4B3,_tmp4B2);goto _LL0;case 28U: _LL3F: _tmp4B8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4B7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp4B6=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_LL40:
# 2769
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4B8,_tmp4B7,_tmp4B6);goto _LL0;case 29U: _LL41: _tmp4BC=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4BB=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp4BA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_tmp4B9=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp467)->f4;_LL42:
# 2771
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4BC,_tmp4BB,_tmp4BA,_tmp4B9);goto _LL0;case 30U: _LL43: _tmp4BE=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4BD=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL44:
# 2773
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4BE,_tmp4BD);goto _LL0;case 31U: _LL45: _tmp4C1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4C0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_tmp4BF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp467)->f3;_LL46:
# 2775
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp4C1,_tmp4C0,_tmp4BF);goto _LL0;case 32U: _LL47: _tmp4C3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4C2=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL48:
# 2777
 t=Cyc_Absyn_enum_type(_tmp4C3->name,_tmp4C3);goto _LL0;case 33U: _LL49: _tmp4C5=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4C4=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL4A:
# 2779
 t=_tmp4C5;goto _LL0;case 34U: _LL4B: _tmp4CA=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp467)->f1).is_calloc;_tmp4C9=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp467)->f1).rgn;_tmp4C8=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp467)->f1).elt_type;_tmp4C7=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp467)->f1).num_elts;_tmp4C6=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp467)->f1).fat_result;_LL4C:
# 2781
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4C9,_tmp4C8,_tmp4C7,_tmp4CA,_tmp4C6);goto _LL0;case 35U: _LL4D: _tmp4CC=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4CB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL4E:
# 2783
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4CC,_tmp4CB);goto _LL0;case 38U: _LL4F: _tmp4CE=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_tmp4CD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp467)->f2;_LL50:
# 2785
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp4CE,_tmp4CD);goto _LL0;case 41U: _LL51: _tmp4CF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL52:
# 2787
 t=Cyc_Tcexp_tcExp(te,topt,_tmp4CF);goto _LL0;case 39U: _LL53: _tmp4D0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp467)->f1;_LL54:
# 2789
 if(!te->allow_valueof)
({void*_tmp475=0U;({unsigned int _tmp802=e->loc;struct _dyneither_ptr _tmp801=({const char*_tmp476="valueof(-) can only occur within types";_tag_dyneither(_tmp476,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp802,_tmp801,_tag_dyneither(_tmp475,sizeof(void*),0U));});});
# 2797
t=Cyc_Absyn_sint_type;
goto _LL0;default: _LL55: _LL56:
# 2800
({void*_tmp477=0U;({unsigned int _tmp804=e->loc;struct _dyneither_ptr _tmp803=({const char*_tmp478="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp478,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp804,_tmp803,_tag_dyneither(_tmp477,sizeof(void*),0U));});});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}_LL0:;}
# 2803
e->topt=t;}
