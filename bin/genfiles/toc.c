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

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct
 _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  unsigned delta = sz * numelts;
  /* Don't let someone decrease the size so much that it wraps around.
   * This is crucial to get NULL right. */
  if (x.last_plus_one - x.base >= delta)
    x.last_plus_one -= delta;
  else x.last_plus_one = x.base;
  return x; 
}

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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(void*ptr);
# 190
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 239
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(const char*file,const char*func,int lineno);
# 257 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 67
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Position_Error;
# 25 "warn.h"
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 36
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 38
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 507
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 509
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 892
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 899
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 930
extern void*Cyc_Absyn_sint_type;
# 935
extern void*Cyc_Absyn_heap_rgn_type;
# 939
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;
# 958
void*Cyc_Absyn_exn_type();
# 966
extern void*Cyc_Absyn_fat_bound_type;
# 970
void*Cyc_Absyn_bounds_one();
# 980
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 986
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq);
# 988
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 990
void*Cyc_Absyn_void_star_type();
# 992
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_type(struct _tuple1*name);
# 998
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1003
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1096
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1098
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1103
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1107
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1112
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1155
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1158
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1168
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1170
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1181
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1186
extern int Cyc_Absyn_no_regions;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
int Cyc_Tcutil_is_void_type(void*);
# 46
int Cyc_Tcutil_is_arithmetic_type(void*);
# 49
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 54
int Cyc_Tcutil_is_dyneither_ptr(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
# 59
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 69
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 77
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 82
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 84
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 90
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 105
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 108
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 112
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
# 115
void*Cyc_Tcutil_compress(void*t);
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 153
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 197
int Cyc_Tcutil_unify(void*,void*);
# 200
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 202
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 204
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 289 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 297
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 351
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 377
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 388
int Cyc_Tcutil_force_type2bool(int desired,void*t);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 36 "insert_checks.h"
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
# 42
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 34 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 60 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 75 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 77
({struct _dyneither_ptr _tmp735=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp734=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp734,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp735,ap);});}
# 79
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 81
({struct _dyneither_ptr _tmp737=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp736=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp736,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp737,ap);});}
# 84
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 87
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 97
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 99
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 125
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp2=*x;struct _tuple14 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple14 _tmp4=*y;struct _tuple14 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 134
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp743=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp743;}),({
struct Cyc_List_List**_tmp742=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp742;}),({
struct Cyc_Dict_Dict*_tmp741=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp740=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp740;});_tmpC;});_tmp11->aggrs_so_far=_tmp741;}),({
struct Cyc_List_List**_tmp73F=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp73F;}),({
struct Cyc_Set_Set**_tmp73E=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp73D=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp73D;});_tmpE;});_tmp11->datatypes_so_far=_tmp73E;}),({
struct Cyc_Dict_Dict*_tmp73C=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp73B=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp73B;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp73C;}),({
struct Cyc_Dict_Dict*_tmp73A=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp739=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp739;});_tmp10;});_tmp11->qvar_tags=_tmp73A;}),({
struct Cyc_Xarray_Xarray*_tmp738=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp738;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 153
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 159
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 162
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp744=({struct Cyc_Toc_TocStateWrap _tmp703;_tmp703.dyn=_tmp1C,_tmp703.state=_tmp1B;_tmp703;});*ts=_tmp744;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 _tmp1A=({struct _tuple15 _tmp704;_tmp704.f1=_tmp1B,_tmp704.f2=arg;_tmp704;});
({void*_tmp745=f(h,& _tmp1A);res=_tmp745;});}
# 170
;}
# 172
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 176
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 179
struct _tuple17 _tmp1D=*env;struct _tuple17 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp24;struct _tuple1*_tmp23;void*(*_tmp22)(struct _tuple1*);_LL1: _tmp24=_tmp1E.f1;_tmp23=(_tmp1E.f2)->f1;_tmp22=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp24->aggrs_so_far,_tmp23);
if(v == 0)
return _tmp22(_tmp23);else{
# 184
struct _tuple18*_tmp1F=*v;struct _tuple18*_tmp20=_tmp1F;void*_tmp21;_LL4: _tmp21=_tmp20->f2;_LL5:;
return _tmp21;}};}
# 189
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _tmp705;_tmp705.f1=q,_tmp705.f2=type_maker;_tmp705;});
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp25[5U]="curr";
# 202 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp25,_tmp25,_tmp25 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp26[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp26,_tmp26,_tmp26 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp27[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp27,_tmp27,_tmp27 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp28[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp28,_tmp28,_tmp28 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp29[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp29,_tmp29,_tmp29 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2A[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2B[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2B,_tmp2B,_tmp2B + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2C[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2C,_tmp2C,_tmp2C + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2D[7U]="_throw";
# 223 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp2D,_tmp2D,_tmp2D + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp2E[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp2E,_tmp2E,_tmp2E + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp2F[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp2F,_tmp2F,_tmp2F + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp30[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp30,_tmp30,_tmp30 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp31[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp31,_tmp31,_tmp31 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp32[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp32,_tmp32,_tmp32 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp33[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp33,_tmp33,_tmp33 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp34[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp34,_tmp34,_tmp34 + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp35[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp35,_tmp35,_tmp35 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp36,_tmp36,_tmp36 + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp37[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp37,_tmp37,_tmp37 + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp38[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp38,_tmp38,_tmp38 + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp39[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39 + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3A[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3A,_tmp3A,_tmp3A + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3B[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3B,_tmp3B,_tmp3B + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3C[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3C,_tmp3C,_tmp3C + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp3D[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp3D,_tmp3D,_tmp3D + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp3E[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp3E,_tmp3E,_tmp3E + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp3F[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp3F,_tmp3F,_tmp3F + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp40[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp40,_tmp40,_tmp40 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp41[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp41,_tmp41,_tmp41 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp42[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp42,_tmp42,_tmp42 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp43[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp43,_tmp43,_tmp43 + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp44[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp44,_tmp44,_tmp44 + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp45[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp45,_tmp45,_tmp45 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp46[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp46,_tmp46,_tmp46 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp47[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp47,_tmp47,_tmp47 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp48[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp48,_tmp48,_tmp48 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp49[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp49,_tmp49,_tmp49 + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp4A[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp4A,_tmp4A,_tmp4A + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp4B[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp4B,_tmp4B,_tmp4B + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp4C[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp4C,_tmp4C,_tmp4C + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp4D[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp4D,_tmp4D,_tmp4D + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp4E[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp4E,_tmp4E,_tmp4E + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp4F[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp4F,_tmp4F,_tmp4F + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp50[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp50,_tmp50,_tmp50 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp51[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp51,_tmp51,_tmp51 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp52[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp52,_tmp52,_tmp52 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp53[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp53,_tmp53,_tmp53 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp54[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp54,_tmp54,_tmp54 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp55[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp55,_tmp55,_tmp55 + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp56[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp56,_tmp56,_tmp56 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp57[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp57,_tmp57,_tmp57 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp58[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp58,_tmp58,_tmp58 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp59[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp59,_tmp59,_tmp59 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp5A[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp5A,_tmp5A,_tmp5A + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp5B[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp5B,_tmp5B,_tmp5B + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp5C[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp5C,_tmp5C,_tmp5C + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp5D[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp5D,_tmp5D,_tmp5D + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp5E[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp5E,_tmp5E,_tmp5E + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp5F[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp5F,_tmp5F,_tmp5F + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp60[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp60,_tmp60,_tmp60 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp61[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp61,_tmp61,_tmp61 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp62[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp62,_tmp62,_tmp62 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp63[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp63,_tmp63,_tmp63 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp64[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp64,_tmp64,_tmp64 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp65[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp65,_tmp65,_tmp65 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp66[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp66,_tmp66,_tmp66 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp67[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp67,_tmp67,_tmp67 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp68[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp68,_tmp68,_tmp68 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp69[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp69,_tmp69,_tmp69 + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp6A[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp6A,_tmp6A,_tmp6A + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp6B[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp6B,_tmp6B,_tmp6B + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp6C[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp6C,_tmp6C,_tmp6C + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 291
void*Cyc_Toc_dyneither_ptr_type(){
static void*dpt=0;
void*_tmp6D=dpt;
if(_tmp6D == 0){
({void*_tmp746=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0),0);_tmp6D=_tmp746;});
dpt=_tmp6D;}
# 298
return _tmp6D;}
# 301
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 303
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 307
static struct _tuple19*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple19*_tmp70=_cycalloc(sizeof(*_tmp70));({struct Cyc_List_List*_tmp748=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({void*_tmp747=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=1U,_tmp6E->f1=name;_tmp6E;});_tmp6F->hd=_tmp747;}),_tmp6F->tl=0;_tmp6F;});_tmp70->f1=_tmp748;}),_tmp70->f2=e;_tmp70;});}
# 313
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp71=e->r;void*_tmp72=_tmp71;struct Cyc_Absyn_Exp*_tmp73;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp73=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp72)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp73);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 319
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=14U,_tmp74->f1=t,_tmp74->f2=e,_tmp74->f3=0,_tmp74->f4=Cyc_Absyn_No_coercion;_tmp74;});}
# 322
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=20U,_tmp75->f1=e;_tmp75;});}
# 325
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=23U,_tmp76->f1=e1,_tmp76->f2=e2;_tmp76;});}
# 328
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=37U,_tmp77->f1=s;_tmp77;});}
# 331
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=17U,_tmp78->f1=t;_tmp78;});}
# 334
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=10U,_tmp79->f1=e,_tmp79->f2=es,_tmp79->f3=0,_tmp79->f4=1;_tmp79;});}
# 337
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=1U,_tmp7A->f1=e;_tmp7A;});}
# 340
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=2U,_tmp7B->f1=s1,_tmp7B->f2=s2;_tmp7B;});}
# 343
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=6U,_tmp7C->f1=e1,_tmp7C->f2=e2,_tmp7C->f3=e3;_tmp7C;});}
# 346
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 349
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=22U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});}
# 352
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 355
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=36U,_tmp7F->f1=tdopt,_tmp7F->f2=ds;_tmp7F;});}
# 357
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=8U,_tmp80->f1=v;_tmp80;});}
# 360
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 364
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp81=e->r;void*_tmp82=_tmp81;struct Cyc_Absyn_Exp*_tmp83;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->tag == 20U){_LL1: _tmp83=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp83,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 383
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 392
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 401
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 410
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 421
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp84=Cyc_Tcutil_compress(t);void*_tmp85=_tmp84;enum Cyc_Absyn_Size_of _tmp8C;switch(*((int*)_tmp85)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)){case 1U: _LL1: _tmp8C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f2;_LL2:
# 425
{enum Cyc_Absyn_Size_of _tmp86=_tmp8C;switch(_tmp86){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp87=0U;({struct _dyneither_ptr _tmp749=({const char*_tmp88="impossible IntCon (not char, short or int)";_tag_dyneither(_tmp88,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp749,_tag_dyneither(_tmp87,sizeof(void*),0U));});});}_LLD:;}
# 431
goto _LL0;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp85)->f1)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}default: goto _LLB;}case 3U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp706;_tmp706.tag=0U,({struct _dyneither_ptr _tmp74A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp706.f1=_tmp74A;});_tmp706;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _dyneither_ptr _tmp74B=({const char*_tmp8A="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8A,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp74B,_tag_dyneither(_tmp89,sizeof(void*),1U));});});}_LL0:;}
# 438
return function;}
# 440
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8D=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8E=_tmp8D;void*_tmp91;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->tag == 3U){_LL1: _tmp91=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->f1).elt_type;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp91);}else{_LL3: _LL4:
({void*_tmp8F=0U;({struct _dyneither_ptr _tmp74C=({const char*_tmp90="impossible type (not pointer)";_tag_dyneither(_tmp90,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp74C,_tag_dyneither(_tmp8F,sizeof(void*),0U));});});}_LL0:;}
# 450
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp92=e->r;void*_tmp93=_tmp92;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;long long _tmp99;int _tmp98;short _tmp97;struct _dyneither_ptr _tmp96;char _tmp95;switch(*((int*)_tmp93)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Null_c).tag){case 2U: _LL1: _tmp95=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Char_c).val).f2;_LL2:
 return _tmp95 == '\000';case 3U: _LL3: _tmp96=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Wchar_c).val;_LL4: {
# 454
unsigned long _tmp94=Cyc_strlen((struct _dyneither_ptr)_tmp96);
int i=0;
if(_tmp94 >= 2  && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),1))== 'x'  && _tmp94 >= 3) && *((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp94;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 464
return 0;}}case 4U: _LL5: _tmp97=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Short_c).val).f2;_LL6:
 return _tmp97 == 0;case 5U: _LL7: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).Int_c).val).f2;_LL8:
 return _tmp98 == 0;case 6U: _LL9: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp93)->f1).LongLong_c).val).f2;_LLA:
 return _tmp99 == 0;case 1U: _LLD: _LLE:
# 469
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 468
 goto _LLE;case 14U: _LLF: _tmp9A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL10:
# 470
 return Cyc_Toc_is_zero(_tmp9A);case 24U: _LL11: _tmp9B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9B);case 26U: _LL13: _tmp9C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp93)->f1;_LL14:
 _tmp9D=_tmp9C;goto _LL16;case 29U: _LL15: _tmp9D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp93)->f3;_LL16:
 _tmp9E=_tmp9D;goto _LL18;case 25U: _LL17: _tmp9E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL18:
 _tmp9F=_tmp9E;goto _LL1A;case 36U: _LL19: _tmp9F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp93)->f2;_LL1A:
# 476
 for(0;_tmp9F != 0;_tmp9F=_tmp9F->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp9F->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 485
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA0=x;union Cyc_Absyn_Nmspace _tmpB2;struct _dyneither_ptr*_tmpB1;_LL1: _tmpB2=_tmpA0->f1;_tmpB1=_tmpA0->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA1=_tmpB2;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpAE;switch((_tmpA1.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAE=0;goto _LL7;case 1U: _LL6: _tmpAE=(_tmpA1.Rel_n).val;_LL7:
 _tmpAF=_tmpAE;goto _LL9;case 2U: _LL8: _tmpAF=(_tmpA1.Abs_n).val;_LL9:
 _tmpB0=_tmpAF;goto _LLB;default: _LLA: _tmpB0=(_tmpA1.C_n).val;_LLB:
# 494
 if(_tmpB0 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA4=({struct Cyc_String_pa_PrintArg_struct _tmp708;_tmp708.tag=0U,_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp708;});struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp707;_tmp707.tag=0U,_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp707;});void*_tmpA2[2U];_tmpA2[0]=& _tmpA4,_tmpA2[1]=& _tmpA5;({struct _dyneither_ptr _tmp74D=({const char*_tmpA3="%s_%s_struct";_tag_dyneither(_tmpA3,sizeof(char),13U);});Cyc_aprintf(_tmp74D,_tag_dyneither(_tmpA2,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA6=_new_region("r");struct _RegionHandle*r=& _tmpA6;_push_region(r);
{struct _dyneither_ptr _tmpAD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp70B;_tmp70B.tag=0U,_tmp70B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp70B;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp70A;_tmp70A.tag=0U,({struct _dyneither_ptr _tmp750=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp74F=r;struct Cyc_List_List*_tmp74E=_tmpB0;Cyc_rstr_sepstr(_tmp74F,_tmp74E,({const char*_tmpAC="_";_tag_dyneither(_tmpAC,sizeof(char),2U);}));}));_tmp70A.f1=_tmp750;});_tmp70A;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp709;_tmp709.tag=0U,_tmp709.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB1);_tmp709;});void*_tmpA7[3U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA,_tmpA7[2]=& _tmpAB;({struct _dyneither_ptr _tmp751=({const char*_tmpA8="%s_%s_%s_struct";_tag_dyneither(_tmpA8,sizeof(char),16U);});Cyc_aprintf(_tmp751,_tag_dyneither(_tmpA7,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAD;};_pop_region(r);};}_LL3:;};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};
# 507
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 510
struct _tuple20 _tmpB3=*env;struct _tuple20 _tmpB4=_tmpB3;struct Cyc_Dict_Dict*_tmpC7;struct _tuple14*_tmpC6;_LL1: _tmpC7=(_tmpB4.f1)->qvar_tags;_tmpC6=_tmpB4.f2;_LL2:;{
struct _tuple14 _tmpB5=*_tmpC6;struct _tuple14 _tmpB6=_tmpB5;struct _tuple1*_tmpC5;struct _tuple1*_tmpC4;_LL4: _tmpC5=_tmpB6.f1;_tmpC4=_tmpB6.f2;_LL5:;{
struct _handler_cons _tmpB7;_push_handler(& _tmpB7);{int _tmpB9=0;if(setjmp(_tmpB7.handler))_tmpB9=1;if(!_tmpB9){{struct _tuple1*_tmpBA=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmpC7,Cyc_Toc_qvar_tag_cmp,_tmpC6);_npop_handler(0U);return _tmpBA;};_pop_handler();}else{void*_tmpB8=(void*)_exn_thrown;void*_tmpBB=_tmpB8;void*_tmpC3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBB)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 514
struct _tuple14*_tmpBC=({struct _tuple14*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->f1=_tmpC5,_tmpC2->f2=_tmpC4;_tmpC2;});
struct _tuple1*_tmpBD=_tmpC5;union Cyc_Absyn_Nmspace _tmpC1;struct _dyneither_ptr*_tmpC0;_LLC: _tmpC1=_tmpBD->f1;_tmpC0=_tmpBD->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC0,_tmpC4);
struct _tuple1*res=({struct _tuple1*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->f1=_tmpC1,({struct _dyneither_ptr*_tmp752=({struct _dyneither_ptr*_tmpBE=_cycalloc(sizeof(*_tmpBE));*_tmpBE=newvar;_tmpBE;});_tmpBF->f2=_tmp752;});_tmpBF;});
({struct Cyc_Dict_Dict _tmp753=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC7,_tmpBC,res);*_tmpC7=_tmp753;});
return res;};}}else{_LL9: _tmpC3=_tmpBB;_LLA:(int)_rethrow(_tmpC3);}_LL6:;}};};};}
# 523
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _tmp70C;_tmp70C.f1=fieldname,_tmp70C.f2=dtname;_tmp70C;});
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 530
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->kind=Cyc_Absyn_StructA,_tmpCA->sc=Cyc_Absyn_Public,_tmpCA->tvs=0,_tmpCA->attributes=0,_tmpCA->expected_mem_kind=0,({
# 533
struct _tuple1*_tmp756=({struct _tuple1*_tmpC8=_cycalloc(sizeof(*_tmpC8));({union Cyc_Absyn_Nmspace _tmp755=Cyc_Absyn_Rel_n(0);_tmpC8->f1=_tmp755;}),_tmpC8->f2=name;_tmpC8;});_tmpCA->name=_tmp756;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp754=({struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->exist_vars=0,_tmpC9->rgn_po=0,_tmpC9->tagged=0,_tmpC9->fields=fs;_tmpC9;});_tmpCA->impl=_tmp754;});_tmpCA;});}struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 542
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 545
struct _tuple21 _tmpCB=*env;struct _tuple21 _tmpCC=_tmpCB;struct Cyc_List_List**_tmpE2;struct Cyc_List_List*_tmpE1;_LL1: _tmpE2=(_tmpCC.f1)->tuple_types;_tmpE1=_tmpCC.f2;_LL2:;
# 547
{struct Cyc_List_List*_tmpCD=*_tmpE2;for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct _tuple22*_tmpCE=(struct _tuple22*)_tmpCD->hd;struct _tuple22*_tmpCF=_tmpCE;void*_tmpD2;struct Cyc_List_List*_tmpD1;_LL4: _tmpD2=_tmpCF->f1;_tmpD1=_tmpCF->f2;_LL5:;{
struct Cyc_List_List*_tmpD0=_tmpE1;
for(0;_tmpD0 != 0  && _tmpD1 != 0;(_tmpD0=_tmpD0->tl,_tmpD1=_tmpD1->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD0->hd)).f2,(void*)_tmpD1->hd))
break;}
if(_tmpD0 == 0  && _tmpD1 == 0)
return _tmpD2;};}}{
# 558
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct _dyneither_ptr _tmp758=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpDF=({struct Cyc_Int_pa_PrintArg_struct _tmp70D;_tmp70D.tag=1U,_tmp70D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70D;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct _dyneither_ptr _tmp757=({const char*_tmpDE="_tuple%d";_tag_dyneither(_tmpDE,sizeof(char),9U);});Cyc_aprintf(_tmp757,_tag_dyneither(_tmpDD,sizeof(void*),1U));});});*_tmpE0=_tmp758;});_tmpE0;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE1);
struct Cyc_List_List*_tmpD3=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp75B=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct Cyc_Absyn_Aggrfield*_tmp75A=({struct Cyc_Absyn_Aggrfield*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct _dyneither_ptr*_tmp759=Cyc_Absyn_fieldname(i);_tmpD4->name=_tmp759;}),_tmpD4->tq=Cyc_Toc_mt_tq,_tmpD4->type=(void*)ts2->hd,_tmpD4->width=0,_tmpD4->attributes=0,_tmpD4->requires_clause=0;_tmpD4;});_tmpD5->hd=_tmp75A;}),_tmpD5->tl=_tmpD3;_tmpD5;});_tmpD3=_tmp75B;});}}
({struct Cyc_List_List*_tmp75C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD3);_tmpD3=_tmp75C;});{
struct Cyc_Absyn_Aggrdecl*_tmpD6=Cyc_Toc_make_c_struct_defn(xname,_tmpD3);
void*_tmpD7=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDC=_cycalloc(sizeof(*_tmpDC));*_tmpDC=_tmpD6;_tmpDC;})),0);
({struct Cyc_List_List*_tmp75E=({struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct Cyc_Absyn_Decl*_tmp75D=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=5U,_tmpD8->f1=_tmpD6;_tmpD8;}),0U);_tmpD9->hd=_tmp75D;}),_tmpD9->tl=Cyc_Toc_result_decls;_tmpD9;});Cyc_Toc_result_decls=_tmp75E;});
({struct Cyc_List_List*_tmp760=({struct Cyc_List_List*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));({struct _tuple22*_tmp75F=({struct _tuple22*_tmpDA=_region_malloc(d,sizeof(*_tmpDA));_tmpDA->f1=_tmpD7,_tmpDA->f2=ts;_tmpDA;});_tmpDB->hd=_tmp75F;}),_tmpDB->tl=*_tmpE2;_tmpDB;});*_tmpE2=_tmp760;});
return _tmpD7;};};}
# 571
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 576
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 579
struct _tuple24*_tmpE3=env;struct Cyc_List_List**_tmpF7;enum Cyc_Absyn_AggrKind _tmpF6;struct Cyc_List_List*_tmpF5;_LL1: _tmpF7=(_tmpE3->f1)->anon_aggr_types;_tmpF6=(_tmpE3->f2)->f1;_tmpF5=(_tmpE3->f2)->f2;_LL2:;
# 581
{struct Cyc_List_List*_tmpE4=*_tmpF7;for(0;_tmpE4 != 0;_tmpE4=_tmpE4->tl){
struct _tuple25*_tmpE5=(struct _tuple25*)_tmpE4->hd;struct _tuple25*_tmpE6=_tmpE5;void*_tmpE9;enum Cyc_Absyn_AggrKind _tmpE8;struct Cyc_List_List*_tmpE7;_LL4: _tmpE9=_tmpE6->f1;_tmpE8=_tmpE6->f2;_tmpE7=_tmpE6->f3;_LL5:;
if(_tmpF6 != _tmpE8)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE7,_tmpF5))
return _tmpE9;}}{
# 590
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct _dyneither_ptr _tmp762=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF3=({struct Cyc_Int_pa_PrintArg_struct _tmp70E;_tmp70E.tag=1U,_tmp70E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70E;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({struct _dyneither_ptr _tmp761=({const char*_tmpF2="_tuple%d";_tag_dyneither(_tmpF2,sizeof(char),9U);});Cyc_aprintf(_tmp761,_tag_dyneither(_tmpF1,sizeof(void*),1U));});});*_tmpF4=_tmp762;});_tmpF4;});
struct Cyc_Absyn_Aggrdecl*_tmpEA=Cyc_Toc_make_c_struct_defn(xname,_tmpF5);
_tmpEA->kind=_tmpF6;{
void*_tmpEB=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF0=_cycalloc(sizeof(*_tmpF0));*_tmpF0=_tmpEA;_tmpF0;})),0);
({struct Cyc_List_List*_tmp764=({struct Cyc_List_List*_tmpED=_cycalloc(sizeof(*_tmpED));({struct Cyc_Absyn_Decl*_tmp763=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=5U,_tmpEC->f1=_tmpEA;_tmpEC;}),0U);_tmpED->hd=_tmp763;}),_tmpED->tl=Cyc_Toc_result_decls;_tmpED;});Cyc_Toc_result_decls=_tmp764;});
({struct Cyc_List_List*_tmp766=({struct Cyc_List_List*_tmpEF=_region_malloc(d,sizeof(*_tmpEF));({struct _tuple25*_tmp765=({struct _tuple25*_tmpEE=_region_malloc(d,sizeof(*_tmpEE));_tmpEE->f1=_tmpEB,_tmpEE->f2=_tmpF6,_tmpEE->f3=_tmpF5;_tmpEE;});_tmpEF->hd=_tmp765;}),_tmpEF->tl=*_tmpF7;_tmpEF;});*_tmpF7=_tmp766;});
return _tmpEB;};};}
# 598
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(({struct _tuple23*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=ak,_tmpF8->f2=fs;_tmpF8;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple26{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple27{struct Cyc_Toc_TocState*f1;struct _tuple26*f2;};struct _tuple28{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 607
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple27*env){
# 616
struct _tuple27 _tmpF9=*env;struct _tuple27 _tmpFA=_tmpF9;struct Cyc_List_List**_tmp11D;struct _tuple1*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_List_List*_tmp119;_LL1: _tmp11D=(_tmpFA.f1)->abs_struct_types;_tmp11C=(_tmpFA.f2)->f1;_tmp11B=(_tmpFA.f2)->f2;_tmp11A=(_tmpFA.f2)->f3;_tmp119=(_tmpFA.f2)->f4;_LL2:;
# 620
{struct Cyc_List_List*_tmpFB=*_tmp11D;for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){
struct _tuple28*_tmpFC=(struct _tuple28*)_tmpFB->hd;struct _tuple28*_tmpFD=_tmpFC;struct _tuple1*_tmp105;struct Cyc_List_List*_tmp104;void*_tmp103;_LL4: _tmp105=_tmpFD->f1;_tmp104=_tmpFD->f2;_tmp103=_tmpFD->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp105,_tmp11C)== 0  && ({
int _tmp767=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11A);_tmp767 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);})){
int okay=1;
{struct Cyc_List_List*_tmpFE=_tmp11A;for(0;_tmpFE != 0;(_tmpFE=_tmpFE->tl,_tmp104=_tmp104->tl)){
void*_tmpFF=(void*)_tmpFE->hd;
void*_tmp100=(void*)((struct Cyc_List_List*)_check_null(_tmp104))->hd;
{struct Cyc_Absyn_Kind*_tmp101=Cyc_Tcutil_type_kind(_tmpFF);struct Cyc_Absyn_Kind*_tmp102=_tmp101;switch(((struct Cyc_Absyn_Kind*)_tmp102)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 631
 continue;default: _LLB: _LLC:
# 634
 if(Cyc_Tcutil_unify(_tmpFF,_tmp100) || ({void*_tmp768=Cyc_Toc_typ_to_c(_tmpFF);Cyc_Tcutil_unify(_tmp768,Cyc_Toc_typ_to_c(_tmp100));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 639
break;}}
# 641
if(okay)
return _tmp103;}}}{
# 648
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _dyneither_ptr _tmp76A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp117=({struct Cyc_Int_pa_PrintArg_struct _tmp70F;_tmp70F.tag=1U,_tmp70F.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp70F;});void*_tmp115[1U];_tmp115[0]=& _tmp117;({struct _dyneither_ptr _tmp769=({const char*_tmp116="_tuple%d";_tag_dyneither(_tmp116,sizeof(char),9U);});Cyc_aprintf(_tmp769,_tag_dyneither(_tmp115,sizeof(void*),1U));});});*_tmp118=_tmp76A;});_tmp118;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp106=0;
# 652
({struct Cyc_List_List*_tmp76C=({struct Cyc_List_List*_tmp108=_region_malloc(d,sizeof(*_tmp108));({struct _tuple28*_tmp76B=({struct _tuple28*_tmp107=_region_malloc(d,sizeof(*_tmp107));_tmp107->f1=_tmp11C,_tmp107->f2=_tmp11A,_tmp107->f3=x;_tmp107;});_tmp108->hd=_tmp76B;}),_tmp108->tl=*_tmp11D;_tmp108;});*_tmp11D=_tmp76C;});{
# 655
struct _RegionHandle _tmp109=_new_region("r");struct _RegionHandle*r=& _tmp109;_push_region(r);
{struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11B,_tmp11A);
for(0;_tmp119 != 0;_tmp119=_tmp119->tl){
struct Cyc_Absyn_Aggrfield*_tmp10B=(struct Cyc_Absyn_Aggrfield*)_tmp119->hd;
void*t=_tmp10B->type;
struct Cyc_List_List*atts=_tmp10B->attributes;
# 665
if((_tmp119->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t))) && !
Cyc_Tcutil_is_array_type(t))
({struct Cyc_List_List*_tmp76E=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*_tmp10D));({void*_tmp76D=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=6U,_tmp10C->f1=0;_tmp10C;});_tmp10D->hd=_tmp76D;}),_tmp10D->tl=atts;_tmp10D;});atts=_tmp76E;});
({void*_tmp76F=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10A,t));t=_tmp76F;});
# 672
if(Cyc_Tcutil_unify(t,Cyc_Absyn_void_type))
({void*_tmp773=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=4U,({void*_tmp772=Cyc_Absyn_void_star_type();(_tmp10E->f1).elt_type=_tmp772;}),({struct Cyc_Absyn_Tqual _tmp771=Cyc_Absyn_empty_tqual(0U);(_tmp10E->f1).tq=_tmp771;}),({
struct Cyc_Absyn_Exp*_tmp770=Cyc_Absyn_uint_exp(0U,0U);(_tmp10E->f1).num_elts=_tmp770;}),(_tmp10E->f1).zero_term=Cyc_Absyn_false_type,(_tmp10E->f1).zt_loc=0U;_tmp10E;});
# 673
t=_tmp773;});
# 676
({struct Cyc_List_List*_tmp775=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Aggrfield*_tmp774=({struct Cyc_Absyn_Aggrfield*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->name=_tmp10B->name,_tmp10F->tq=Cyc_Toc_mt_tq,_tmp10F->type=t,_tmp10F->width=_tmp10B->width,_tmp10F->attributes=atts,_tmp10F->requires_clause=0;_tmp10F;});_tmp110->hd=_tmp774;}),_tmp110->tl=_tmp106;_tmp110;});_tmp106=_tmp775;});}
# 678
({struct Cyc_List_List*_tmp776=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp106);_tmp106=_tmp776;});{
struct Cyc_Absyn_Aggrdecl*_tmp111=Cyc_Toc_make_c_struct_defn(xname,_tmp106);
({struct Cyc_List_List*_tmp778=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Decl*_tmp777=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=5U,_tmp112->f1=_tmp111;_tmp112;}),0U);_tmp113->hd=_tmp777;}),_tmp113->tl=Cyc_Toc_result_decls;_tmp113;});Cyc_Toc_result_decls=_tmp778;});{
void*_tmp114=x;_npop_handler(0U);return _tmp114;};};}
# 656
;_pop_region(r);};};}
# 684
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 688
struct _tuple26 env=({struct _tuple26 _tmp710;_tmp710.f1=struct_name,_tmp710.f2=type_vars,_tmp710.f3=type_args,_tmp710.f4=fields;_tmp710;});
return((void*(*)(struct _tuple26*arg,void*(*f)(struct _RegionHandle*,struct _tuple27*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 696
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp77B=({struct _dyneither_ptr*_tmp121=_cycalloc(sizeof(*_tmp121));({struct _dyneither_ptr _tmp77A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp120=({struct Cyc_Int_pa_PrintArg_struct _tmp711;_tmp711.tag=1U,_tmp711.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp711;});void*_tmp11E[1U];_tmp11E[0]=& _tmp120;({struct _dyneither_ptr _tmp779=({const char*_tmp11F="_tmp%X";_tag_dyneither(_tmp11F,sizeof(char),7U);});Cyc_aprintf(_tmp779,_tag_dyneither(_tmp11E,sizeof(void*),1U));});});*_tmp121=_tmp77A;});_tmp121;});_tmp122->f2=_tmp77B;});_tmp122;});}struct _tuple29{struct Cyc_Toc_TocState*f1;int f2;};
# 702
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple29*env){
struct _tuple29 _tmp123=*env;struct _tuple29 _tmp124=_tmp123;struct Cyc_Xarray_Xarray*_tmp12C;_LL1: _tmp12C=(_tmp124.f1)->temp_labels;_LL2:;{
int _tmp125=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp77C=_tmp125;_tmp77C < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12C);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12C,_tmp125);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12B=_cycalloc(sizeof(*_tmp12B));({struct _dyneither_ptr _tmp77E=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12A=({struct Cyc_Int_pa_PrintArg_struct _tmp712;_tmp712.tag=1U,_tmp712.f1=(unsigned int)_tmp125;_tmp712;});void*_tmp128[1U];_tmp128[0]=& _tmp12A;({struct _dyneither_ptr _tmp77D=({const char*_tmp129="_LL%X";_tag_dyneither(_tmp129,sizeof(char),6U);});Cyc_aprintf(_tmp77D,_tag_dyneither(_tmp128,sizeof(void*),1U));});});*_tmp12B=_tmp77E;});_tmp12B;});
if(({int _tmp77F=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12C,res);_tmp77F != _tmp125;}))
({void*_tmp126=0U;({struct _dyneither_ptr _tmp780=({const char*_tmp127="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp127,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp780,_tag_dyneither(_tmp126,sizeof(void*),0U));});});
return res;};};}
# 713
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple29*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 719
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp12D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp12D))->hd)->name)!= 0){
++ ans;
_tmp12D=_tmp12D->tl;}
# 726
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 732
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp12E=*a;struct _tuple9 _tmp12F=_tmp12E;struct _dyneither_ptr*_tmp133;struct Cyc_Absyn_Tqual _tmp132;void*_tmp131;_LL1: _tmp133=_tmp12F.f1;_tmp132=_tmp12F.f2;_tmp131=_tmp12F.f3;_LL2:;
return({struct _tuple9*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=_tmp133,_tmp130->f2=_tmp132,({void*_tmp781=Cyc_Toc_typ_to_c(_tmp131);_tmp130->f3=_tmp781;});_tmp130;});}
# 755 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp134=Cyc_Tcutil_compress(t);void*_tmp135=_tmp134;void*_tmp13B;void*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;struct Cyc_Absyn_Exp*_tmp138;void*_tmp137;unsigned int _tmp136;switch(*((int*)_tmp135)){case 4U: _LL1: _tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).elt_type;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).tq;_tmp138=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).num_elts;_tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zero_term;_tmp136=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).zt_loc;_LL2:
# 758
 return({void*_tmp782=Cyc_Toc_typ_to_c_array(_tmp13A);Cyc_Absyn_cstar_type(_tmp782,_tmp139);});case 1U: _LL3: _tmp13B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp135)->f2;if(_tmp13B != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13B);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 764
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 766
struct Cyc_Absyn_Aggrfield*_tmp13C=({struct Cyc_Absyn_Aggrfield*_tmp13D=_cycalloc(sizeof(*_tmp13D));*_tmp13D=*f;_tmp13D;});
_tmp13C->type=new_type;
_tmp13C->requires_clause=0;
_tmp13C->tq=Cyc_Toc_mt_tq;
return _tmp13C;}
# 773
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 775
return;}
# 778
static void*Cyc_Toc_char_star_type(){
static void**cs=0;
if(cs == 0)
({void**_tmp784=({void**_tmp13E=_cycalloc(sizeof(*_tmp13E));({void*_tmp783=Cyc_Absyn_star_type(Cyc_Absyn_char_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type);*_tmp13E=_tmp783;});_tmp13E;});cs=_tmp784;});
return*cs;}
# 784
static void*Cyc_Toc_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp787=({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp786=({void*_tmp785=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_tmp785,Cyc_Toc_mt_tq);});*_tmp13F=_tmp786;});_tmp13F;});r=_tmp787;});
return*r;}
# 790
static void*Cyc_Toc_dyn_rgn_type(){
static void**r=0;
if(r == 0)
({void**_tmp78A=({void**_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp789=({void*_tmp788=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_type(_tmp788,Cyc_Toc_mt_tq);});*_tmp140=_tmp789;});_tmp140;});r=_tmp78A;});
return*r;}
# 796
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp141=Cyc_Tcutil_compress(t);void*_tmp142=_tmp141;struct Cyc_Absyn_Tvar*_tmp143;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->tag == 2U){_LL1: _tmp143=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp142)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 802
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp144=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp145=_tmp144;if(((struct Cyc_Absyn_Kind*)_tmp145)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 809
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp146=t;struct Cyc_Absyn_Datatypedecl*_tmp18D;void**_tmp18C;struct Cyc_Absyn_Enumdecl*_tmp18B;struct Cyc_Absyn_Aggrdecl*_tmp18A;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp188;struct _tuple1*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Typedefdecl*_tmp185;void*_tmp184;enum Cyc_Absyn_AggrKind _tmp183;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Tqual _tmp180;void*_tmp17F;struct Cyc_List_List*_tmp17E;int _tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17B;void*_tmp17A;struct Cyc_Absyn_Tqual _tmp179;struct Cyc_Absyn_Exp*_tmp178;unsigned int _tmp177;void*_tmp176;struct Cyc_Absyn_Tqual _tmp175;void*_tmp174;struct Cyc_Absyn_Tvar*_tmp173;void**_tmp172;struct Cyc_List_List*_tmp171;struct _tuple1*_tmp170;union Cyc_Absyn_AggrInfo _tmp16F;struct Cyc_List_List*_tmp16E;struct Cyc_Absyn_Datatypedecl*_tmp16D;struct Cyc_Absyn_Datatypefield*_tmp16C;switch(*((int*)_tmp146)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)){case 0U: _LL1: _LL2:
 return t;case 18U: _LL7: _LL8:
# 824
 return Cyc_Absyn_void_type;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).tag == 2){_LL9: _tmp16D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f1;_tmp16C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1).KnownDatatypefield).val).f2;_LLA:
# 826
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp16C->name,_tmp16D->name));}else{_LLB: _LLC:
# 828
({void*_tmp147=0U;({struct _dyneither_ptr _tmp78B=({const char*_tmp148="unresolved DatatypeFieldType";_tag_dyneither(_tmp148,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp78B,_tag_dyneither(_tmp147,sizeof(void*),0U));});});}case 1U: _LLF: _LL10:
# 837
 goto _LL12;case 2U: _LL11: _LL12:
 return t;case 20U: _LL1B: _tmp16F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_tmp16E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f2;_LL1C:
# 884
{union Cyc_Absyn_AggrInfo _tmp158=_tmp16F;if((_tmp158.UnknownAggr).tag == 1){_LL45: _LL46:
 return t;}else{_LL47: _LL48:
 goto _LL44;}_LL44:;}{
# 888
struct Cyc_Absyn_Aggrdecl*_tmp159=Cyc_Absyn_get_known_aggrdecl(_tmp16F);
if(_tmp159->expected_mem_kind){
# 891
if(_tmp159->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp15C=({struct Cyc_String_pa_PrintArg_struct _tmp714;_tmp714.tag=0U,({
struct _dyneither_ptr _tmp78C=(struct _dyneither_ptr)(_tmp159->kind == Cyc_Absyn_UnionA?({const char*_tmp15E="union";_tag_dyneither(_tmp15E,sizeof(char),6U);}):({const char*_tmp15F="struct";_tag_dyneither(_tmp15F,sizeof(char),7U);}));_tmp714.f1=_tmp78C;});_tmp714;});struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp713;_tmp713.tag=0U,({
struct _dyneither_ptr _tmp78D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp713.f1=_tmp78D;});_tmp713;});void*_tmp15A[2U];_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D;({struct _dyneither_ptr _tmp78E=({const char*_tmp15B="%s %s was never defined.";_tag_dyneither(_tmp15B,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp78E,_tag_dyneither(_tmp15A,sizeof(void*),2U));});});}
# 899
if(_tmp159->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*_tmp160=_tmp159->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields;
if(_tmp160 == 0)return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp160))->tl != 0;_tmp160=_tmp160->tl){;}{
void*_tmp161=((struct Cyc_Absyn_Aggrfield*)_tmp160->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp161))){
if(_tmp159->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp715;_tmp715.tag=0U,({struct _dyneither_ptr _tmp78F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp159->name));_tmp715.f1=_tmp78F;});_tmp715;});void*_tmp162[1U];_tmp162[0]=& _tmp164;({struct _dyneither_ptr _tmp790=({const char*_tmp163="struct %s ended up being abstract.";_tag_dyneither(_tmp163,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp790,_tag_dyneither(_tmp162,sizeof(void*),1U));});});{
# 912
struct _RegionHandle _tmp165=_new_region("r");struct _RegionHandle*r=& _tmp165;_push_region(r);
{struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp159->tvs,_tmp16E);
void*_tmp167=Cyc_Tcutil_rsubstitute(r,_tmp166,_tmp161);
if(Cyc_Toc_is_abstract_type(_tmp167)){void*_tmp168=Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp168;}{
void*_tmp169=Cyc_Toc_add_struct_type(_tmp159->name,_tmp159->tvs,_tmp16E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp159->impl))->fields);_npop_handler(0U);return _tmp169;};}
# 913
;_pop_region(r);};}
# 918
return Cyc_Toc_aggrdecl_type(_tmp159->name,Cyc_Absyn_strctq);};};};case 15U: _LL1D: _tmp170=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL1E:
 return t;case 16U: _LL1F: _tmp171=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp146)->f1)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp171);return t;case 4U: _LL23: _LL24:
# 930
 return Cyc_Absyn_uint_type;case 3U: _LL25: _LL26:
 return Cyc_Toc_rgn_type();case 17U: _LL27: _LL28:
 return t;default: _LL29: _LL2A:
# 935
 return Cyc_Absyn_void_star_type();}case 1U: _LL3: _tmp172=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp146)->f2;_LL4:
# 813
 if(*_tmp172 == 0){
*_tmp172=Cyc_Absyn_sint_type;
return Cyc_Absyn_sint_type;}
# 817
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp172));case 2U: _LL5: _tmp173=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->f1;_LL6:
# 819
 if((Cyc_Tcutil_tvar_kind(_tmp173,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 821
return Cyc_Absyn_void_type;else{
# 823
return Cyc_Absyn_void_star_type();}case 3U: _LLD: _tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp175=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).elt_tq;_tmp174=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp146)->f1).ptr_atts).bounds;_LLE:
# 832
({void*_tmp791=Cyc_Toc_typ_to_c(_tmp176);_tmp176=_tmp791;});
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp174)== 0)
return Cyc_Toc_dyneither_ptr_type();else{
# 836
return Cyc_Absyn_star_type(_tmp176,Cyc_Absyn_heap_rgn_type,_tmp175,Cyc_Absyn_false_type);}case 4U: _LL13: _tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).elt_type;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).tq;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).num_elts;_tmp177=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp146)->f1).zt_loc;_LL14:
# 840
 return({void*_tmp795=Cyc_Toc_typ_to_c(_tmp17A);struct Cyc_Absyn_Tqual _tmp794=_tmp179;struct Cyc_Absyn_Exp*_tmp793=_tmp178;void*_tmp792=Cyc_Absyn_false_type;Cyc_Absyn_array_type(_tmp795,_tmp794,_tmp793,_tmp792,_tmp177);});case 5U: _LL15: _tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_tqual;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).ret_type;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).args;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).c_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).cyc_varargs;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp146)->f1).attributes;_LL16: {
# 846
struct Cyc_List_List*_tmp149=0;
for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
void*_tmp14A=(void*)_tmp17B->hd;void*_tmp14B=_tmp14A;switch(*((int*)_tmp14B)){case 4U: _LL36: _LL37:
 goto _LL39;case 5U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 continue;case 22U: _LL3C: _LL3D:
 continue;case 21U: _LL3E: _LL3F:
 continue;case 20U: _LL40: _LL41:
 continue;default: _LL42: _LL43:
({struct Cyc_List_List*_tmp796=({struct Cyc_List_List*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->hd=(void*)_tmp17B->hd,_tmp14C->tl=_tmp149;_tmp14C;});_tmp149=_tmp796;});goto _LL35;}_LL35:;}{
# 857
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17E);
if(_tmp17C != 0){
# 860
void*_tmp14E=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp17C->type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type));
struct _tuple9*_tmp14F=({struct _tuple9*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->f1=_tmp17C->name,_tmp151->f2=_tmp17C->tq,_tmp151->f3=_tmp14E;_tmp151;});
({struct Cyc_List_List*_tmp798=({struct Cyc_List_List*_tmp797=_tmp14D;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp797,({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->hd=_tmp14F,_tmp150->tl=0;_tmp150;}));});_tmp14D=_tmp798;});}
# 864
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->tag=5U,(_tmp152->f1).tvars=0,(_tmp152->f1).effect=0,(_tmp152->f1).ret_tqual=_tmp180,({void*_tmp799=Cyc_Toc_typ_to_c(_tmp17F);(_tmp152->f1).ret_type=_tmp799;}),(_tmp152->f1).args=_tmp14D,(_tmp152->f1).c_varargs=_tmp17D,(_tmp152->f1).cyc_varargs=0,(_tmp152->f1).rgn_po=0,(_tmp152->f1).attributes=_tmp149,(_tmp152->f1).requires_clause=0,(_tmp152->f1).requires_relns=0,(_tmp152->f1).ensures_clause=0,(_tmp152->f1).ensures_relns=0;_tmp152;});};}case 6U: _LL17: _tmp181=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp146)->f1;_LL18: {
# 869
struct Cyc_List_List*_tmp153=0;
for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
({struct Cyc_List_List*_tmp79C=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));({struct _tuple12*_tmp79B=({struct _tuple12*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->f1=(*((struct _tuple12*)_tmp181->hd)).f1,({void*_tmp79A=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp181->hd)).f2);_tmp154->f2=_tmp79A;});_tmp154;});_tmp155->hd=_tmp79B;}),_tmp155->tl=_tmp153;_tmp155;});_tmp153=_tmp79C;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp153));}case 7U: _LL19: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f1;_tmp182=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp146)->f2;_LL1A: {
# 878
struct Cyc_List_List*_tmp156=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp79F=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));({struct Cyc_Absyn_Aggrfield*_tmp79E=({struct Cyc_Absyn_Aggrfield*_tmp79D=(struct Cyc_Absyn_Aggrfield*)_tmp182->hd;Cyc_Toc_aggrfield_to_c(_tmp79D,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp182->hd)->type));});_tmp157->hd=_tmp79E;}),_tmp157->tl=_tmp156;_tmp157;});_tmp156=_tmp79F;});}
return({enum Cyc_Absyn_AggrKind _tmp7A0=_tmp183;Cyc_Toc_add_anon_aggr_type(_tmp7A0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp156));});}case 8U: _LL21: _tmp187=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f1;_tmp186=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f2;_tmp185=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f3;_tmp184=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp146)->f4;_LL22:
# 924
 if(_tmp184 == 0){
if(_tmp186 != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=8U,_tmp16A->f1=_tmp187,_tmp16A->f2=0,_tmp16A->f3=_tmp185,_tmp16A->f4=0;_tmp16A;});else{
return t;}}else{
# 929
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->tag=8U,_tmp16B->f1=_tmp187,_tmp16B->f2=0,_tmp16B->f3=_tmp185,({void*_tmp7A1=Cyc_Toc_typ_to_c(_tmp184);_tmp16B->f4=_tmp7A1;});_tmp16B;});}case 9U: _LL2B: _tmp188=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp146)->f1;_LL2C:
# 940
 return t;case 11U: _LL2D: _tmp189=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp146)->f1;_LL2E:
# 945
 if(_tmp189->topt != 0)return Cyc_Toc_typ_to_c((void*)_check_null(_tmp189->topt));else{
return t;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)){case 0U: _LL2F: _tmp18A=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL30:
# 948
 Cyc_Toc_aggrdecl_to_c(_tmp18A);
if(_tmp18A->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_unionq_type);else{
return Cyc_Toc_aggrdecl_type(_tmp18A->name,Cyc_Absyn_strctq);}case 1U: _LL31: _tmp18B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_LL32:
# 953
 Cyc_Toc_enumdecl_to_c(_tmp18B);
return t;default: _LL33: _tmp18D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f1)->r)->f1;_tmp18C=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp146)->f2;_LL34:
# 956
 Cyc_Toc_datatypedecl_to_c(_tmp18D);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp18C)));}}_LL0:;}
# 961
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp18E=t;void*_tmp190;struct Cyc_Absyn_Tqual _tmp18F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->tag == 4U){_LL1: _tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).elt_type;_tmp18F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18E)->f1).tq;_LL2:
# 964
 return({void*_tmp7A2=Cyc_Absyn_star_type(_tmp190,Cyc_Absyn_heap_rgn_type,_tmp18F,Cyc_Absyn_false_type);Cyc_Toc_cast_it(_tmp7A2,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 971
static int Cyc_Toc_atomic_type(void*t){
void*_tmp191=Cyc_Tcutil_compress(t);void*_tmp192=_tmp191;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A4;void*_tmp1A3;void*_tmp1A2;struct Cyc_List_List*_tmp1A1;switch(*((int*)_tmp192)){case 2U: _LL1: _LL2:
 return 0;case 0U: _LL3: _tmp1A2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1;_tmp1A1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL4: {
# 975
void*_tmp193=_tmp1A2;struct Cyc_Absyn_Datatypedecl*_tmp19D;struct Cyc_Absyn_Datatypefield*_tmp19C;union Cyc_Absyn_AggrInfo _tmp19B;switch(*((int*)_tmp193)){case 0U: _LL12: _LL13:
 goto _LL15;case 1U: _LL14: _LL15: goto _LL17;case 2U: _LL16: _LL17: goto _LL19;case 4U: _LL18: _LL19:
 goto _LL1B;case 15U: _LL1A: _LL1B: goto _LL1D;case 16U: _LL1C: _LL1D:
 return 1;case 18U: _LL1E: _LL1F:
 goto _LL21;case 3U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 return 0;case 20U: _LL24: _tmp19B=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp193)->f1;_LL25:
# 983
{union Cyc_Absyn_AggrInfo _tmp194=_tmp19B;if((_tmp194.UnknownAggr).tag == 1){_LL2B: _LL2C:
 return 0;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}{
# 987
struct Cyc_Absyn_Aggrdecl*_tmp195=Cyc_Absyn_get_known_aggrdecl(_tmp19B);
if(_tmp195->impl == 0)
return 0;
{struct Cyc_List_List*_tmp196=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp195->impl))->fields;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp196->hd)->type))return 0;}}
return 1;};case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).tag == 2){_LL26: _tmp19D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f1;_tmp19C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp193)->f1).KnownDatatypefield).val).f2;_LL27:
# 994
{struct Cyc_List_List*_tmp197=_tmp19C->typs;for(0;_tmp197 != 0;_tmp197=_tmp197->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp197->hd)).f2))return 0;}}
return 1;}else{goto _LL28;}default: _LL28: _LL29:
({struct Cyc_String_pa_PrintArg_struct _tmp19A=({struct Cyc_String_pa_PrintArg_struct _tmp716;_tmp716.tag=0U,({struct _dyneither_ptr _tmp7A3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp716.f1=_tmp7A3;});_tmp716;});void*_tmp198[1U];_tmp198[0]=& _tmp19A;({struct _dyneither_ptr _tmp7A4=({const char*_tmp199="atomic_typ:  bad type %s";_tag_dyneither(_tmp199,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7A4,_tag_dyneither(_tmp198,sizeof(void*),1U));});});}_LL11:;}case 5U: _LL5: _LL6:
# 999
 return 1;case 4U: _LL7: _tmp1A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192)->f1).elt_type;_LL8:
 return Cyc_Toc_atomic_type(_tmp1A3);case 7U: _LL9: _tmp1A4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp192)->f2;_LLA:
# 1007
 for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)_tmp1A4->hd)->type))return 0;}
return 1;case 6U: _LLB: _tmp1A5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp192)->f1;_LLC:
# 1011
 for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple12*)_tmp1A5->hd)).f2))return 0;}
return 1;case 3U: _LLD: _LLE:
# 1016
 return 0;default: _LLF: _LL10:
({struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp717;_tmp717.tag=0U,({struct _dyneither_ptr _tmp7A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp717.f1=_tmp7A5;});_tmp717;});void*_tmp19E[1U];_tmp19E[0]=& _tmp1A0;({struct _dyneither_ptr _tmp7A6=({const char*_tmp19F="atomic_typ:  bad type %s";_tag_dyneither(_tmp19F,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7A6,_tag_dyneither(_tmp19E,sizeof(void*),1U));});});}_LL0:;}
# 1021
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A6=Cyc_Tcutil_compress(t);void*_tmp1A7=_tmp1A6;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->tag == 3U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A7)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_void_type(_tmp1A8);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1028
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1032
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1037
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfo _tmp1B5;switch(*((int*)_tmp1AA)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 20U){_LL1: _tmp1B5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->f1;_LL2: {
# 1040
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp7A7=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7A7,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1045
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp718;_tmp718.tag=0U,_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp718;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp7A8=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7A8,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp719;_tmp719.tag=0U,({struct _dyneither_ptr _tmp7A9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp719.f1=_tmp7A9;});_tmp719;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp7AA=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7AA,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1056
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1059
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1062
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 3U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_type;_LL9:
# 1064
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,({struct _dyneither_ptr _tmp7AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp71A.f1=_tmp7AB;});_tmp71A;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp7AC=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7AC,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1067
 return 0;}_LL0:;}
# 1072
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7AD=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp7AD,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1076
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7AE=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp7AE,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1080
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_type(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1086
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7AF=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp7AF,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1090
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp7B0=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp7B0,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1094
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_type(elt_type))
return({struct Cyc_Absyn_Exp*_tmp7B1=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp7B1,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1098
return({struct Cyc_Absyn_Exp*_tmp7B2=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp7B2,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1101
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp7B3=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp7B3,({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1105
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7B4=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp7B4,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1108
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp7B5=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp7B5,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1112
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp7B6,({void*_tmp1CC=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1125
int is_string=0;
{void*_tmp1CD=e->r;void*_tmp1CE=_tmp1CD;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1131
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp7B8=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp7B7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=0U,_tmp1CF->f1=vd;_tmp1CF;}),0U);_tmp1D0->hd=_tmp7B7;}),_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});Cyc_Toc_result_decls=_tmp7B8;});
({struct Cyc_Absyn_Exp*_tmp7B9=Cyc_Absyn_var_exp(x,0U);xexp=_tmp7B9;});
({struct Cyc_Absyn_Exp*_tmp7BA=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp7BA;});}else{
# 1141
({struct Cyc_Absyn_Exp*_tmp7BC=({void*_tmp7BB=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7BB,e);});xexp=_tmp7BC;});
# 1143
({struct Cyc_Absyn_Exp*_tmp7BE=({void*_tmp7BD=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp7BD,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp7BE;});}
# 1145
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_tmp1D1[3U];({struct _tuple19*_tmp7C1=({struct _tuple19*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D1[0]=_tmp7C1;}),({
struct _tuple19*_tmp7C0=({struct _tuple19*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D1[1]=_tmp7C0;}),({
struct _tuple19*_tmp7BF=({struct _tuple19*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xplussz;_tmp1D4;});_tmp1D1[2]=_tmp7BF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D1,sizeof(struct _tuple19*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1182 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1186
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D7=nv;int*_tmp1D8;_LL1: _tmp1D8=_tmp1D7->in_lhs;_LL2:;
return*_tmp1D8;}
# 1192
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=0,_tmp1DB->fallthru_info=0,_tmp1DB->toplevel=1,({
# 1197
int*_tmp7C3=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});_tmp1DB->in_lhs=_tmp7C3;}),({
int*_tmp7C2=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DB->in_sizeof=_tmp7C2;}),_tmp1DB->rgn=r;_tmp1DB;});}
# 1202
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DC=e;struct _dyneither_ptr**_tmp1E3;struct _dyneither_ptr**_tmp1E2;struct Cyc_Toc_FallthruInfo*_tmp1E1;int _tmp1E0;int*_tmp1DF;int*_tmp1DE;_LL1: _tmp1E3=_tmp1DC->break_lab;_tmp1E2=_tmp1DC->continue_lab;_tmp1E1=_tmp1DC->fallthru_info;_tmp1E0=_tmp1DC->toplevel;_tmp1DF=_tmp1DC->in_lhs;_tmp1DE=_tmp1DC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=_tmp1E3,_tmp1DD->continue_lab=_tmp1E2,_tmp1DD->fallthru_info=_tmp1E1,_tmp1DD->toplevel=_tmp1E0,_tmp1DD->in_lhs=_tmp1DF,_tmp1DD->in_sizeof=_tmp1DE,_tmp1DD->rgn=r;_tmp1DD;});}
# 1207
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E4=e;struct _dyneither_ptr**_tmp1EB;struct _dyneither_ptr**_tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;int _tmp1E8;int*_tmp1E7;int*_tmp1E6;_LL1: _tmp1EB=_tmp1E4->break_lab;_tmp1EA=_tmp1E4->continue_lab;_tmp1E9=_tmp1E4->fallthru_info;_tmp1E8=_tmp1E4->toplevel;_tmp1E7=_tmp1E4->in_lhs;_tmp1E6=_tmp1E4->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->break_lab=_tmp1EB,_tmp1E5->continue_lab=_tmp1EA,_tmp1E5->fallthru_info=_tmp1E9,_tmp1E5->toplevel=0,_tmp1E5->in_lhs=_tmp1E7,_tmp1E5->in_sizeof=_tmp1E6,_tmp1E5->rgn=r;_tmp1E5;});}
# 1211
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;struct _dyneither_ptr**_tmp1F3;struct _dyneither_ptr**_tmp1F2;struct Cyc_Toc_FallthruInfo*_tmp1F1;int _tmp1F0;int*_tmp1EF;int*_tmp1EE;_LL1: _tmp1F3=_tmp1EC->break_lab;_tmp1F2=_tmp1EC->continue_lab;_tmp1F1=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1EF=_tmp1EC->in_lhs;_tmp1EE=_tmp1EC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=_tmp1F3,_tmp1ED->continue_lab=_tmp1F2,_tmp1ED->fallthru_info=_tmp1F1,_tmp1ED->toplevel=1,_tmp1ED->in_lhs=_tmp1EF,_tmp1ED->in_sizeof=_tmp1EE,_tmp1ED->rgn=r;_tmp1ED;});}
# 1215
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F4=e;int*_tmp1F5;_LL1: _tmp1F5=_tmp1F4->in_lhs;_LL2:;
*_tmp1F5=b;}
# 1219
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F6=e;int*_tmp1F8;_LL1: _tmp1F8=_tmp1F6->in_sizeof;_LL2:;{
int _tmp1F7=*_tmp1F8;
*_tmp1F8=b;
return _tmp1F7;};}
# 1225
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F9=e;int*_tmp1FA;_LL1: _tmp1FA=_tmp1F9->in_sizeof;_LL2:;
return*_tmp1FA;}
# 1232
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FB=e;struct _dyneither_ptr**_tmp202;struct _dyneither_ptr**_tmp201;struct Cyc_Toc_FallthruInfo*_tmp200;int _tmp1FF;int*_tmp1FE;int*_tmp1FD;_LL1: _tmp202=_tmp1FB->break_lab;_tmp201=_tmp1FB->continue_lab;_tmp200=_tmp1FB->fallthru_info;_tmp1FF=_tmp1FB->toplevel;_tmp1FE=_tmp1FB->in_lhs;_tmp1FD=_tmp1FB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));_tmp1FC->break_lab=0,_tmp1FC->continue_lab=0,_tmp1FC->fallthru_info=_tmp200,_tmp1FC->toplevel=_tmp1FF,_tmp1FC->in_lhs=_tmp1FE,_tmp1FC->in_sizeof=_tmp1FD,_tmp1FC->rgn=r;_tmp1FC;});}
# 1238
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1242
struct Cyc_Toc_Env*_tmp203=e;struct _dyneither_ptr**_tmp20C;struct _dyneither_ptr**_tmp20B;struct Cyc_Toc_FallthruInfo*_tmp20A;int _tmp209;int*_tmp208;int*_tmp207;_LL1: _tmp20C=_tmp203->break_lab;_tmp20B=_tmp203->continue_lab;_tmp20A=_tmp203->fallthru_info;_tmp209=_tmp203->toplevel;_tmp208=_tmp203->in_lhs;_tmp207=_tmp203->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp206=_region_malloc(r,sizeof(*_tmp206));
_tmp206->label=fallthru_l,_tmp206->binders=fallthru_binders;_tmp206;});
return({struct Cyc_Toc_Env*_tmp205=_region_malloc(r,sizeof(*_tmp205));({struct _dyneither_ptr**_tmp7C4=({struct _dyneither_ptr**_tmp204=_region_malloc(r,sizeof(*_tmp204));*_tmp204=break_l;_tmp204;});_tmp205->break_lab=_tmp7C4;}),_tmp205->continue_lab=_tmp20B,_tmp205->fallthru_info=fi,_tmp205->toplevel=_tmp209,_tmp205->in_lhs=_tmp208,_tmp205->in_sizeof=_tmp207,_tmp205->rgn=r;_tmp205;});};}
# 1248
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1250
struct Cyc_Toc_Env*_tmp20D=e;struct _dyneither_ptr**_tmp215;struct _dyneither_ptr**_tmp214;struct Cyc_Toc_FallthruInfo*_tmp213;int _tmp212;int*_tmp211;int*_tmp210;_LL1: _tmp215=_tmp20D->break_lab;_tmp214=_tmp20D->continue_lab;_tmp213=_tmp20D->fallthru_info;_tmp212=_tmp20D->toplevel;_tmp211=_tmp20D->in_lhs;_tmp210=_tmp20D->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));({struct _dyneither_ptr**_tmp7C5=({struct _dyneither_ptr**_tmp20E=_region_malloc(r,sizeof(*_tmp20E));*_tmp20E=break_l;_tmp20E;});_tmp20F->break_lab=_tmp7C5;}),_tmp20F->continue_lab=_tmp214,_tmp20F->fallthru_info=0,_tmp20F->toplevel=_tmp212,_tmp20F->in_lhs=_tmp211,_tmp20F->in_sizeof=_tmp210,_tmp20F->rgn=r;_tmp20F;});}
# 1257
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1260
struct Cyc_Toc_Env*_tmp216=e;struct _dyneither_ptr**_tmp21E;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21C;int _tmp21B;int*_tmp21A;int*_tmp219;_LL1: _tmp21E=_tmp216->break_lab;_tmp21D=_tmp216->continue_lab;_tmp21C=_tmp216->fallthru_info;_tmp21B=_tmp216->toplevel;_tmp21A=_tmp216->in_lhs;_tmp219=_tmp216->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->break_lab=0,_tmp218->continue_lab=_tmp21D,({struct Cyc_Toc_FallthruInfo*_tmp7C6=({struct Cyc_Toc_FallthruInfo*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->label=next_l,_tmp217->binders=0;_tmp217;});_tmp218->fallthru_info=_tmp7C6;}),_tmp218->toplevel=_tmp21B,_tmp218->in_lhs=_tmp21A,_tmp218->in_sizeof=_tmp219,_tmp218->rgn=r;_tmp218;});}
# 1273 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1276
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp21F=e->annot;void*_tmp220=_tmp21F;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NoCheck){_LL1: _LL2:
 return 0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullOnly){_LL3: _LL4:
 goto _LL6;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullAndFatBound){_LL5: _LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_NullAndThinBound){_LL7: _LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_FatBound){_LL9: _LLA:
 goto _LLC;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_InsertChecks_ThinBound){_LLB: _LLC:
 return 0;}else{_LLD: _LLE:
({void*_tmp221=0U;({unsigned int _tmp7C8=e->loc;struct _dyneither_ptr _tmp7C7=({const char*_tmp222="Toc: do_null_check";_tag_dyneither(_tmp222,sizeof(char),19U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp7C8,_tmp7C7,_tag_dyneither(_tmp221,sizeof(void*),0U));});});}}}}}}_LL0:;}
# 1292
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1294
int ans;
int _tmp223=Cyc_Toc_in_lhs(nv);
void*_tmp224=Cyc_Tcutil_compress((void*)_check_null(ptr->topt));
void*_tmp225=Cyc_Toc_typ_to_c(_tmp224);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index != 0)
Cyc_Toc_exp_to_c(nv,index);{
void*_tmp226=_tmp224;void*_tmp23A;struct Cyc_Absyn_Tqual _tmp239;void*_tmp238;void*_tmp237;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226)->tag == 3U){_LL1: _tmp23A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226)->f1).elt_type;_tmp239=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226)->f1).elt_tq;_tmp238=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226)->f1).ptr_atts).bounds;_tmp237=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226)->f1).ptr_atts).zero_term;_LL2:
# 1305
{void*_tmp227=annot;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Exp*_tmp233;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_tmp227)->tag == Cyc_InsertChecks_NoCheck){_LL6: _LL7:
# 1307
 if(!((unsigned int)({void*_tmp7C9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7C9,_tmp238);})))
({void*_tmp7CD=({void*_tmp7CC=({void*_tmp7CA=Cyc_Toc_typ_to_c(_tmp23A);Cyc_Absyn_cstar_type(_tmp7CA,_tmp239);});Cyc_Toc_cast_it_r(_tmp7CC,({
struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_tmp7CB,Cyc_Toc_curr_sp,0U);}));});
# 1308
ptr->r=_tmp7CD;});
# 1310
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_tmp227)->tag == Cyc_InsertChecks_NullOnly){_LL8: _LL9:
# 1313
 if(!((unsigned int)({void*_tmp7CE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7CE,_tmp238);})))
goto _LLB;
# 1316
({void*_tmp7D2=({void*_tmp7D1=_tmp225;Cyc_Toc_cast_it_r(_tmp7D1,({
struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp7D0,({struct Cyc_Absyn_Exp*_tmp228[1U];({struct Cyc_Absyn_Exp*_tmp7CF=Cyc_Absyn_new_exp(ptr->r,0U);_tmp228[0]=_tmp7CF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp228,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1316
ptr->r=_tmp7D2;});
# 1318
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_tmp227)->tag == Cyc_InsertChecks_NullAndFatBound){_LLA: _LLB:
 goto _LLD;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_tmp227)->tag == Cyc_InsertChecks_FatBound){_LLC: _LLD: {
# 1323
void*ta1=Cyc_Toc_typ_to_c(_tmp23A);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_tmp7D7=({void*_tmp7D6=Cyc_Absyn_cstar_type(ta1,_tmp239);Cyc_Toc_cast_it_r(_tmp7D6,({
struct Cyc_Absyn_Exp*_tmp7D5=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp7D5,({struct Cyc_Absyn_Exp*_tmp229[3U];({
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Absyn_new_exp(ptr->r,0U);_tmp229[0]=_tmp7D4;}),({
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp229[1]=_tmp7D3;}),_tmp229[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp229,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1325
ptr->r=_tmp7D7;});
# 1330
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp227)->tag == Cyc_InsertChecks_NullAndThinBound){_LLE: _tmp233=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_tmp227)->f1;_LLF:
# 1333
 fn_e=Cyc_Toc__check_known_subscript_null_e;
_tmp234=_tmp233;goto _LL11;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp227)->tag == Cyc_InsertChecks_ThinBound){_LL10: _tmp234=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_tmp227)->f1;_LL11: {
# 1336
void*ta1=Cyc_Toc_typ_to_c(_tmp23A);
struct Cyc_Absyn_Exp*ind=(unsigned int)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1341
struct _tuple13 _tmp22A=Cyc_Evexp_eval_const_uint_exp(_tmp234);struct _tuple13 _tmp22B=_tmp22A;unsigned int _tmp230;int _tmp22F;_LL15: _tmp230=_tmp22B.f1;_tmp22F=_tmp22B.f2;_LL16:;
if((!_tmp22F  || _tmp230 != 1) || !Cyc_Tcutil_is_zeroterm_pointer_type((void*)_check_null(ptr->topt))){
# 1344
({void*_tmp7DC=({void*_tmp7DB=Cyc_Absyn_cstar_type(ta1,_tmp239);Cyc_Toc_cast_it_r(_tmp7DB,({
struct Cyc_Absyn_Exp*_tmp7DA=fn_e;Cyc_Absyn_fncall_exp(_tmp7DA,({struct Cyc_Absyn_Exp*_tmp22C[4U];({struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22C[0]=_tmp7D9;}),_tmp22C[1]=_tmp234,({
struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Absyn_sizeoftype_exp(ta1,0U);_tmp22C[2]=_tmp7D8;}),_tmp22C[3]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22C,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 1344
ptr->r=_tmp7DC;});
# 1348
ans=1;}else{
# 1351
if(Cyc_Toc_is_zero(_tmp234)){
if(fn_e == Cyc_Toc__check_known_subscript_null_e)
# 1354
({void*_tmp7E0=({void*_tmp7DF=_tmp225;Cyc_Toc_cast_it_r(_tmp7DF,({
struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp7DE,({struct Cyc_Absyn_Exp*_tmp22D[1U];({struct Cyc_Absyn_Exp*_tmp7DD=
Cyc_Absyn_new_exp(ptr->r,0U);_tmp22D[0]=_tmp7DD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22D,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 1354
ptr->r=_tmp7E0;});
# 1357
ans=0;}else{
# 1361
({struct Cyc_Absyn_Exp*_tmp7E1=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,ptr);fn_e=_tmp7E1;});
({void*_tmp7E5=({void*_tmp7E4=Cyc_Absyn_cstar_type(ta1,_tmp239);Cyc_Toc_cast_it_r(_tmp7E4,({
struct Cyc_Absyn_Exp*_tmp7E3=fn_e;Cyc_Absyn_fncall_exp(_tmp7E3,({struct Cyc_Absyn_Exp*_tmp22E[3U];({struct Cyc_Absyn_Exp*_tmp7E2=Cyc_Absyn_new_exp(ptr->r,0U);_tmp22E[0]=_tmp7E2;}),_tmp22E[1]=_tmp234,_tmp22E[2]=ind;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp22E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 1362
ptr->r=_tmp7E5;});
# 1365
ans=1;}}
# 1368
goto _LL5;}}else{_LL12: _LL13:
({void*_tmp231=0U;({struct _dyneither_ptr _tmp7E6=({const char*_tmp232="FIX: ptr_use_to_c, bad annotation";_tag_dyneither(_tmp232,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7E6,_tag_dyneither(_tmp231,sizeof(void*),0U));});});}}}}}}_LL5:;}
# 1371
Cyc_Toc_set_lhs(nv,_tmp223);
return ans;}else{_LL3: _LL4:
({void*_tmp235=0U;({struct _dyneither_ptr _tmp7E7=({const char*_tmp236="ptr_use_to_c: non-pointer-type";_tag_dyneither(_tmp236,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7E7,_tag_dyneither(_tmp235,sizeof(void*),0U));});});}_LL0:;};}
# 1377
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp23B=0U;({struct _dyneither_ptr _tmp7E8=({const char*_tmp23C="Missing type in primop ";_tag_dyneither(_tmp23C,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7E8,_tag_dyneither(_tmp23B,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1381
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D->f1=Cyc_Toc_mt_tq,({void*_tmp7E9=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp23D->f2=_tmp7E9;});_tmp23D;});}
# 1386
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp23E=e->r;void*_tmp23F=_tmp23E;struct Cyc_Absyn_Exp*_tmp247;int _tmp246;struct Cyc_Absyn_Exp*_tmp245;int _tmp244;struct Cyc_List_List*_tmp243;switch(*((int*)_tmp23F)){case 26U: _LL1: _tmp243=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp23F)->f1;_LL2:
# 1389
{struct Cyc_List_List*_tmp240=_tmp243;for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
if((*((struct _tuple19*)_tmp240->hd)).f1 != 0)
({void*_tmp241=0U;({struct _dyneither_ptr _tmp7EA=({const char*_tmp242="array designators for abstract-field initialization";_tag_dyneither(_tmp242,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp7EA,_tag_dyneither(_tmp241,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp7EB=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp243),0U);_tmp245=_tmp7EB;});_tmp244=0;goto _LL4;case 27U: _LL3: _tmp245=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23F)->f2;_tmp244=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23F)->f4;_LL4:
 _tmp247=_tmp245;_tmp246=_tmp244;goto _LL6;case 28U: _LL5: _tmp247=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23F)->f1;_tmp246=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp23F)->f3;_LL6:
# 1395
({struct Cyc_Absyn_Exp*_tmp7EC=Cyc_Absyn_copy_exp(_tmp247);_tmp247=_tmp7EC;});
return _tmp246?({struct Cyc_Absyn_Exp*_tmp7ED=_tmp247;Cyc_Absyn_add_exp(_tmp7ED,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp247;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1404
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1412
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp248=e->r;void*_tmp249=_tmp248;struct Cyc_List_List*_tmp24A;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp249)->tag == 29U){_LL1: _tmp24A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp249)->f3;_LL2:
 dles=_tmp24A;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1418
{void*_tmp24B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp24C=_tmp24B;struct Cyc_Absyn_Aggrdecl*_tmp24D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f1)->f1).KnownAggr).tag == 2){_LL6: _tmp24D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f1)->f1).KnownAggr).val;_LL7:
# 1420
 if(_tmp24D->impl == 0)
return 0;
if(_tmp24D->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24D->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 1428
 return 0;}_LL5:;}
# 1430
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp24E=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple19*_tmp24F=(struct _tuple19*)dles->hd;struct _tuple19*_tmp250=_tmp24F;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Exp*_tmp258;_LLB: _tmp259=_tmp250->f1;_tmp258=_tmp250->f2;_LLC:;{
struct _dyneither_ptr*_tmp251=Cyc_Absyn_designatorlist_to_fieldname(_tmp259);
if(!Cyc_strptrcmp(_tmp251,_tmp24E->name)){
struct Cyc_Absyn_Exp*_tmp252=Cyc_Toc_get_varsizeexp(_tmp258);
if(_tmp252 != 0)
return _tmp252;{
void*_tmp253=Cyc_Tcutil_compress(_tmp24E->type);void*_tmp254=_tmp253;void*_tmp257;struct Cyc_Absyn_Exp*_tmp256;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp254)->tag == 4U){_LLE: _tmp257=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp254)->f1).elt_type;_tmp256=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp254)->f1).num_elts;_LLF:
# 1445
 if(_tmp256 == 0  || !Cyc_Toc_is_zero(_tmp256))
return 0;
# 1451
return({struct Cyc_Absyn_Exp*_tmp7F0=({struct Cyc_Absyn_Exp*_tmp7EF=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp7EF,({struct Cyc_Absyn_Exp*_tmp255[2U];_tmp255[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp258)),({
# 1453
struct Cyc_Absyn_Exp*_tmp7EE=Cyc_Absyn_sizeoftype_exp(_tmp257,0U);_tmp255[1]=_tmp7EE;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp255,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1451
Cyc_Absyn_add_exp(_tmp7F0,
# 1454
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1459
({void*_tmp25A=0U;({struct _dyneither_ptr _tmp7F1=({const char*_tmp25B="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp25B,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F1,_tag_dyneither(_tmp25A,sizeof(void*),0U));});});};}
# 1462
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp25C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){
struct Cyc_Absyn_Aggrfield*_tmp25D=(struct Cyc_Absyn_Aggrfield*)_tmp25C->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp25D->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1469
({void*_tmp25E=0U;({struct _dyneither_ptr _tmp7F3=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp261=({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0U,_tmp71B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp71B;});void*_tmp25F[1U];_tmp25F[0]=& _tmp261;({struct _dyneither_ptr _tmp7F2=({const char*_tmp260="get_member_offset %s failed";_tag_dyneither(_tmp260,sizeof(char),28U);});Cyc_aprintf(_tmp7F2,_tag_dyneither(_tmp25F,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F3,_tag_dyneither(_tmp25E,sizeof(void*),0U));});});}struct _tuple30{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1473
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple30*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1476
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1479
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->tag=5U,_tmp262->f1=e1,_tmp262->f2=incr;_tmp262;}),0U);}
# 1483
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1492
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1494
void*_tmp263=e1->r;void*_tmp264=_tmp263;struct Cyc_Absyn_Exp*_tmp26C;struct _dyneither_ptr*_tmp26B;int _tmp26A;int _tmp269;void*_tmp268;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Stmt*_tmp266;switch(*((int*)_tmp264)){case 37U: _LL1: _tmp266=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp264)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp266,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp268=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp264)->f1;_tmp267=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp264)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp267,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp26C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp264)->f1;_tmp26B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp264)->f2;_tmp26A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp264)->f3;_tmp269=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp264)->f4;_LL6:
# 1499
 e1->r=_tmp26C->r;
({struct Cyc_Absyn_Exp*_tmp7F6=e1;struct Cyc_List_List*_tmp7F5=({struct Cyc_List_List*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->hd=_tmp26B,_tmp265->tl=fs;_tmp265;});struct Cyc_Absyn_Exp*(*_tmp7F4)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp7F6,_tmp7F5,_tmp7F4,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1507
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1509
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp7F7;});}
({void*_tmp7F8=(f(e1_copy,f_env))->r;e1->r=_tmp7F8;});
goto _LL0;}}_LL0:;}
# 1515
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1517
void*_tmp26D=s->r;void*_tmp26E=_tmp26D;struct Cyc_Absyn_Stmt*_tmp275;struct Cyc_Absyn_Decl*_tmp274;struct Cyc_Absyn_Stmt*_tmp273;struct Cyc_Absyn_Exp*_tmp272;switch(*((int*)_tmp26E)){case 1U: _LL1: _tmp272=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp26E)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp272,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp274=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26E)->f1;_tmp273=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp26E)->f2;_LL4:
# 1520
 Cyc_Toc_lvalue_assign_stmt(_tmp273,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp275=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp26E)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp275,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp271=({struct Cyc_String_pa_PrintArg_struct _tmp71C;_tmp71C.tag=0U,({struct _dyneither_ptr _tmp7F9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp71C.f1=_tmp7F9;});_tmp71C;});void*_tmp26F[1U];_tmp26F[0]=& _tmp271;({struct _dyneither_ptr _tmp7FA=({const char*_tmp270="lvalue_assign_stmt: %s";_tag_dyneither(_tmp270,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp7FA,_tag_dyneither(_tmp26F,sizeof(void*),1U));});});}_LL0:;}
# 1526
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1530
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp276=e->r;void*_tmp277=_tmp276;struct Cyc_Absyn_Stmt*_tmp27E;struct Cyc_Absyn_Exp*_tmp27D;void**_tmp27C;struct Cyc_Absyn_Exp**_tmp27B;switch(*((int*)_tmp277)){case 14U: _LL1: _tmp27C=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp277)->f1;_tmp27B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp277)->f2;_LL2:
# 1533
({struct Cyc_Absyn_Exp*_tmp7FB=Cyc_Toc_push_address_exp(*_tmp27B);*_tmp27B=_tmp7FB;});
({void*_tmp7FC=Cyc_Absyn_cstar_type(*_tmp27C,Cyc_Toc_mt_tq);*_tmp27C=_tmp7FC;});
return e;case 20U: _LL3: _tmp27D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp277)->f1;_LL4:
# 1537
 return _tmp27D;case 37U: _LL5: _tmp27E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp277)->f1;_LL6:
# 1541
 Cyc_Toc_push_address_stmt(_tmp27E);
return e;default: _LL7: _LL8:
# 1544
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp71D;_tmp71D.tag=0U,({struct _dyneither_ptr _tmp7FD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp71D.f1=_tmp7FD;});_tmp71D;});void*_tmp278[1U];_tmp278[0]=& _tmp27A;({struct _dyneither_ptr _tmp7FE=({const char*_tmp279="can't take & of exp %s";_tag_dyneither(_tmp279,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7FE,_tag_dyneither(_tmp278,sizeof(void*),1U));});});}_LL0:;}
# 1549
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp27F=s->r;void*_tmp280=_tmp27F;struct Cyc_Absyn_Exp**_tmp286;struct Cyc_Absyn_Stmt*_tmp285;struct Cyc_Absyn_Stmt*_tmp284;switch(*((int*)_tmp280)){case 2U: _LL1: _tmp284=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp280)->f2;_LL2:
 _tmp285=_tmp284;goto _LL4;case 12U: _LL3: _tmp285=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp280)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp285);goto _LL0;case 1U: _LL5: _tmp286=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp280)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp7FF=Cyc_Toc_push_address_exp(*_tmp286);*_tmp286=_tmp7FF;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp71E;_tmp71E.tag=0U,({struct _dyneither_ptr _tmp800=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp71E.f1=_tmp800;});_tmp71E;});void*_tmp281[1U];_tmp281[0]=& _tmp283;({struct _dyneither_ptr _tmp801=({const char*_tmp282="can't take & of stmt %s";_tag_dyneither(_tmp282,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp801,_tag_dyneither(_tmp281,sizeof(void*),1U));});});}_LL0:;}
# 1558
static void*Cyc_Toc_get_ptr_elt_type(void*t){
void*_tmp287=Cyc_Tcutil_compress(t);void*_tmp288=_tmp287;struct Cyc_Absyn_PtrInfo _tmp28B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp288)->tag == 3U){_LL1: _tmp28B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp288)->f1;_LL2:
 return _tmp28B.elt_type;}else{_LL3: _LL4:
({void*_tmp289=0U;({struct _dyneither_ptr _tmp802=({const char*_tmp28A="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp28A,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp802,_tag_dyneither(_tmp289,sizeof(void*),0U));});});}_LL0:;}
# 1568
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp28C=Cyc_Tcutil_compress(t);void*_tmp28D=_tmp28C;void*_tmp29E;switch(*((int*)_tmp28D)){case 3U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp803=Cyc_Absyn_null_exp(0U);res=_tmp803;});goto _LL0;case 0U: _LL3: _tmp29E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp28D)->f1;_LL4:
# 1573
{void*_tmp28E=_tmp29E;int _tmp29A;enum Cyc_Absyn_Sign _tmp299;enum Cyc_Absyn_Sign _tmp298;enum Cyc_Absyn_Sign _tmp297;enum Cyc_Absyn_Sign _tmp296;enum Cyc_Absyn_Sign _tmp295;switch(*((int*)_tmp28E)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28E)->f2){case Cyc_Absyn_Char_sz: _LL8: _tmp295=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28E)->f1;_LL9:
# 1575
({struct Cyc_Absyn_Exp*_tmp804=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp295,'\000'),0U);res=_tmp804;});goto _LL7;case Cyc_Absyn_Short_sz: _LLA: _tmp296=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28E)->f1;_LLB:
# 1577
({struct Cyc_Absyn_Exp*_tmp805=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp296,0),0U);res=_tmp805;});goto _LL7;case Cyc_Absyn_Int_sz: _LL10: _tmp297=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28E)->f1;_LL11:
# 1580
 _tmp298=_tmp297;goto _LL13;case Cyc_Absyn_Long_sz: _LL12: _tmp298=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28E)->f1;_LL13:
# 1582
({struct Cyc_Absyn_Exp*_tmp806=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp298,0),0U);res=_tmp806;});goto _LL7;default: _LL14: _tmp299=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp28E)->f1;_LL15:
# 1584
({struct Cyc_Absyn_Exp*_tmp807=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp299,(long long)0),0U);res=_tmp807;});goto _LL7;}case 15U: _LLC: _LLD:
# 1578
 goto _LLF;case 16U: _LLE: _LLF:
 _tmp297=1U;goto _LL11;case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp28E)->f1){case 0U: _LL16: _LL17:
# 1585
({struct Cyc_Absyn_Exp*_tmp808=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp28F="0.0F";_tag_dyneither(_tmp28F,sizeof(char),5U);}),0),0U);res=_tmp808;});goto _LL7;case 1U: _LL18: _LL19:
({struct Cyc_Absyn_Exp*_tmp809=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp290="0.0";_tag_dyneither(_tmp290,sizeof(char),4U);}),1),0U);res=_tmp809;});goto _LL7;default: _LL1A: _tmp29A=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp28E)->f1;_LL1B:
({struct Cyc_Absyn_Exp*_tmp80B=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp80A=({const char*_tmp291="0.0L";_tag_dyneither(_tmp291,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp80A,_tmp29A);}),0U);res=_tmp80B;});goto _LL7;}default: _LL1C: _LL1D:
# 1589
({struct Cyc_String_pa_PrintArg_struct _tmp294=({struct Cyc_String_pa_PrintArg_struct _tmp71F;_tmp71F.tag=0U,({struct _dyneither_ptr _tmp80C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp71F.f1=_tmp80C;});_tmp71F;});void*_tmp292[1U];_tmp292[0]=& _tmp294;({struct _dyneither_ptr _tmp80D=({const char*_tmp293="found non-zero type %s in generate_zero";_tag_dyneither(_tmp293,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80D,_tag_dyneither(_tmp292,sizeof(void*),1U));});});}_LL7:;}
# 1591
goto _LL0;default: _LL5: _LL6:
# 1593
({struct Cyc_String_pa_PrintArg_struct _tmp29D=({struct Cyc_String_pa_PrintArg_struct _tmp720;_tmp720.tag=0U,({struct _dyneither_ptr _tmp80E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp720.f1=_tmp80E;});_tmp720;});void*_tmp29B[1U];_tmp29B[0]=& _tmp29D;({struct _dyneither_ptr _tmp80F=({const char*_tmp29C="found non-zero type %s in generate_zero";_tag_dyneither(_tmp29C,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80F,_tag_dyneither(_tmp29B,sizeof(void*),1U));});});}_LL0:;}
# 1595
res->topt=t;
return res;}
# 1602
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1615 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type);
void*_tmp29F=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A0=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A1=Cyc_Absyn_cstar_type(_tmp29F,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A2=({struct Cyc_Core_Opt*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->v=_tmp2A1;_tmp2D3;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A3=e1->r;void*_tmp2A4=_tmp2A3;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;switch(*((int*)_tmp2A4)){case 20U: _LL1: _tmp2A8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A4)->f1;_LL2:
# 1623
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp810=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2A8,0,Cyc_Absyn_Other_coercion,0U);_tmp2A8=_tmp810;});
_tmp2A8->topt=fat_ptr_type;
_tmp2A8->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;}
# 1628
Cyc_Toc_exp_to_c(nv,_tmp2A8);xinit=_tmp2A8;goto _LL0;case 23U: _LL3: _tmp2AA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A4)->f1;_tmp2A9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A4)->f2;_LL4:
# 1630
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp811=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AA,0,Cyc_Absyn_Other_coercion,0U);_tmp2AA=_tmp811;});
_tmp2AA->topt=fat_ptr_type;
_tmp2AA->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;}
# 1635
Cyc_Toc_exp_to_c(nv,_tmp2AA);Cyc_Toc_exp_to_c(nv,_tmp2A9);
({struct Cyc_Absyn_Exp*_tmp814=({struct Cyc_Absyn_Exp*_tmp813=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp813,({struct Cyc_Absyn_Exp*_tmp2A5[3U];_tmp2A5[0]=_tmp2AA,({
struct Cyc_Absyn_Exp*_tmp812=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A5[1]=_tmp812;}),_tmp2A5[2]=_tmp2A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A5,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1636
xinit=_tmp814;});
# 1638
goto _LL0;default: _LL5: _LL6:
({void*_tmp2A6=0U;({struct _dyneither_ptr _tmp815=({const char*_tmp2A7="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2A7,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp815,_tag_dyneither(_tmp2A6,sizeof(void*),0U));});});}_LL0:;}{
# 1641
struct _tuple1*_tmp2AB=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2AC=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2AC;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2AD=({struct Cyc_Absyn_Vardecl*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->sc=Cyc_Absyn_Public,_tmp2D2->name=_tmp2AB,_tmp2D2->varloc=0U,_tmp2D2->tq=Cyc_Toc_mt_tq,_tmp2D2->type=_tmp2A0,_tmp2D2->initializer=xinit,_tmp2D2->rgn=0,_tmp2D2->attributes=0,_tmp2D2->escapes=0;_tmp2D2;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2AE=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->tag=4U,_tmp2D1->f1=_tmp2AD;_tmp2D1;});
struct Cyc_Absyn_Exp*_tmp2AF=Cyc_Absyn_varb_exp((void*)_tmp2AE,0U);
_tmp2AF->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_deref_exp(_tmp2AF,0U);
_tmp2B0->topt=elt_type;
_tmp2B0->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,_tmp2B0);{
struct _tuple1*_tmp2B1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B2=({struct Cyc_Absyn_Vardecl*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->sc=Cyc_Absyn_Public,_tmp2D0->name=_tmp2B1,_tmp2D0->varloc=0U,_tmp2D0->tq=Cyc_Toc_mt_tq,_tmp2D0->type=_tmp29F,_tmp2D0->initializer=_tmp2B0,_tmp2D0->rgn=0,_tmp2D0->attributes=0,_tmp2D0->escapes=0;_tmp2D0;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B3=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF->tag=4U,_tmp2CF->f1=_tmp2B2;_tmp2CF;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_varb_exp((void*)_tmp2B3,0U);
_tmp2B4->topt=_tmp2B0->topt;
({struct Cyc_Absyn_Exp*_tmp816=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B4,e2,0U);z_init=_tmp816;});
z_init->topt=_tmp2B4->topt;}
# 1661
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2B5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B6=({struct Cyc_Absyn_Vardecl*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->sc=Cyc_Absyn_Public,_tmp2CE->name=_tmp2B5,_tmp2CE->varloc=0U,_tmp2CE->tq=Cyc_Toc_mt_tq,_tmp2CE->type=_tmp29F,_tmp2CE->initializer=z_init,_tmp2CE->rgn=0,_tmp2CE->attributes=0,_tmp2CE->escapes=0;_tmp2CE;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B7=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->tag=4U,_tmp2CD->f1=_tmp2B6;_tmp2CD;});
# 1667
struct Cyc_Absyn_Exp*_tmp2B8=Cyc_Absyn_varb_exp((void*)_tmp2B3,0U);_tmp2B8->topt=_tmp2B0->topt;{
struct Cyc_Absyn_Exp*_tmp2B9=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BA=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2B8,_tmp2B9,0U);
_tmp2BA->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2BA);{
# 1673
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_varb_exp((void*)_tmp2B7,0U);_tmp2BB->topt=_tmp2B0->topt;{
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2BB,_tmp2BC,0U);
_tmp2BD->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,_tmp2BD);{
# 1679
struct Cyc_List_List*_tmp2BE=({struct Cyc_Absyn_Exp*_tmp2CC[2U];({struct Cyc_Absyn_Exp*_tmp818=Cyc_Absyn_varb_exp((void*)_tmp2AE,0U);_tmp2CC[0]=_tmp818;}),({
struct Cyc_Absyn_Exp*_tmp817=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2CC[1]=_tmp817;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2CC,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp81A=({struct Cyc_Absyn_Exp*_tmp819=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2BE,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp819,_tmp2BF,0U);});xsize=_tmp81A;});{
# 1686
struct Cyc_Absyn_Exp*_tmp2C0=({struct Cyc_Absyn_Exp*_tmp81B=xsize;Cyc_Absyn_and_exp(_tmp81B,Cyc_Absyn_and_exp(_tmp2BA,_tmp2BD,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C1=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C2=({struct Cyc_Absyn_Exp*_tmp81C=Cyc_Absyn_varb_exp((void*)_tmp2AE,0U);Cyc_Toc_member_exp(_tmp81C,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp81D=Cyc_Toc_cast_it(_tmp2A1,_tmp2C2);_tmp2C2=_tmp81D;});{
struct Cyc_Absyn_Exp*_tmp2C3=Cyc_Absyn_deref_exp(_tmp2C2,0U);
struct Cyc_Absyn_Exp*_tmp2C4=({struct Cyc_Absyn_Exp*_tmp81E=_tmp2C3;Cyc_Absyn_assign_exp(_tmp81E,Cyc_Absyn_var_exp(_tmp2B5,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C5=Cyc_Absyn_exp_stmt(_tmp2C4,0U);
({struct Cyc_Absyn_Stmt*_tmp822=({struct Cyc_Absyn_Stmt*_tmp821=({struct Cyc_Absyn_Exp*_tmp820=_tmp2C0;struct Cyc_Absyn_Stmt*_tmp81F=_tmp2C1;Cyc_Absyn_ifthenelse_stmt(_tmp820,_tmp81F,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp821,_tmp2C5,0U);});_tmp2C5=_tmp822;});
({struct Cyc_Absyn_Stmt*_tmp825=({struct Cyc_Absyn_Decl*_tmp824=({struct Cyc_Absyn_Decl*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));({void*_tmp823=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6->tag=0U,_tmp2C6->f1=_tmp2B6;_tmp2C6;});_tmp2C7->r=_tmp823;}),_tmp2C7->loc=0U;_tmp2C7;});Cyc_Absyn_decl_stmt(_tmp824,_tmp2C5,0U);});_tmp2C5=_tmp825;});
({struct Cyc_Absyn_Stmt*_tmp828=({struct Cyc_Absyn_Decl*_tmp827=({struct Cyc_Absyn_Decl*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));({void*_tmp826=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->tag=0U,_tmp2C8->f1=_tmp2B2;_tmp2C8;});_tmp2C9->r=_tmp826;}),_tmp2C9->loc=0U;_tmp2C9;});Cyc_Absyn_decl_stmt(_tmp827,_tmp2C5,0U);});_tmp2C5=_tmp828;});
({struct Cyc_Absyn_Stmt*_tmp82B=({struct Cyc_Absyn_Decl*_tmp82A=({struct Cyc_Absyn_Decl*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));({void*_tmp829=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->tag=0U,_tmp2CA->f1=_tmp2AD;_tmp2CA;});_tmp2CB->r=_tmp829;}),_tmp2CB->loc=0U;_tmp2CB;});Cyc_Absyn_decl_stmt(_tmp82A,_tmp2C5,0U);});_tmp2C5=_tmp82B;});
({void*_tmp82C=Cyc_Toc_stmt_exp_r(_tmp2C5);e->r=_tmp82C;});};};};};};};};};};}
# 1643
;_pop_region(rgn2);};}
# 1712 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1716
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D4=Cyc_Tcutil_compress(aggrtype);void*_tmp2D5=_tmp2D4;union Cyc_Absyn_AggrInfo _tmp2DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5)->f1)->tag == 20U){_LL1: _tmp2DA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D5)->f1)->f1;_LL2:
# 1719
({struct Cyc_Absyn_Aggrdecl*_tmp82D=Cyc_Absyn_get_known_aggrdecl(_tmp2DA);ad=_tmp82D;});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmp2D6=0U;({struct _dyneither_ptr _tmp830=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D9=({struct Cyc_String_pa_PrintArg_struct _tmp721;_tmp721.tag=0U,({
struct _dyneither_ptr _tmp82E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp721.f1=_tmp82E;});_tmp721;});void*_tmp2D7[1U];_tmp2D7[0]=& _tmp2D9;({struct _dyneither_ptr _tmp82F=({const char*_tmp2D8="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2D8,sizeof(char),51U);});Cyc_aprintf(_tmp82F,_tag_dyneither(_tmp2D7,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp830,_tag_dyneither(_tmp2D6,sizeof(void*),0U));});});}_LL0:;}{
# 1723
struct _tuple1*_tmp2DB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2DC=Cyc_Absyn_var_exp(_tmp2DB,0U);
struct Cyc_Absyn_Exp*_tmp2DD=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2DE=Cyc_Absyn_aggrarrow_exp(_tmp2DC,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2DF=Cyc_Absyn_neq_exp(_tmp2DE,_tmp2DD,0U);
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_aggrarrow_exp(_tmp2DC,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E1=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E0,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E2=({struct Cyc_Absyn_Exp*_tmp832=_tmp2DF;struct Cyc_Absyn_Stmt*_tmp831=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp832,_tmp831,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E3=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E4=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E5=({struct _tuple1*_tmp835=_tmp2DB;void*_tmp834=_tmp2E3;struct Cyc_Absyn_Exp*_tmp833=_tmp2E4;Cyc_Absyn_declare_stmt(_tmp835,_tmp834,_tmp833,Cyc_Absyn_seq_stmt(_tmp2E2,_tmp2E1,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2E5);}else{
# 1737
struct Cyc_Absyn_Exp*_tmp2E6=({struct Cyc_Absyn_Exp*_tmp836=aggrproj(_tmp2DC,f,0U);Cyc_Toc_member_exp(_tmp836,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2E7=Cyc_Absyn_neq_exp(_tmp2E6,_tmp2DD,0U);
struct Cyc_Absyn_Exp*_tmp2E8=({struct Cyc_Absyn_Exp*_tmp837=aggrproj(_tmp2DC,f,0U);Cyc_Toc_member_exp(_tmp837,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2E9=Cyc_Absyn_exp_stmt(_tmp2E8,0U);
struct Cyc_Absyn_Stmt*_tmp2EA=({struct Cyc_Absyn_Exp*_tmp839=_tmp2E7;struct Cyc_Absyn_Stmt*_tmp838=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp839,_tmp838,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2EB=({struct _tuple1*_tmp83C=_tmp2DB;void*_tmp83B=e1_c_type;struct Cyc_Absyn_Exp*_tmp83A=e1;Cyc_Absyn_declare_stmt(_tmp83C,_tmp83B,_tmp83A,Cyc_Absyn_seq_stmt(_tmp2EA,_tmp2E9,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EB);}};}
# 1747
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1750
void*_tmp2EC=Cyc_Tcutil_compress(t);void*_tmp2ED=_tmp2EC;union Cyc_Absyn_AggrInfo _tmp2F4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2ED)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2ED)->f1)->tag == 20U){_LL1: _tmp2F4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2ED)->f1)->f1;_LL2: {
# 1752
struct Cyc_Absyn_Aggrdecl*_tmp2EE=Cyc_Absyn_get_known_aggrdecl(_tmp2F4);
({int _tmp83D=Cyc_Toc_get_member_offset(_tmp2EE,f);*f_tag=_tmp83D;});{
# 1755
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F2=({struct Cyc_String_pa_PrintArg_struct _tmp723;_tmp723.tag=0U,_tmp723.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2EE->name).f2);_tmp723;});struct Cyc_String_pa_PrintArg_struct _tmp2F3=({struct Cyc_String_pa_PrintArg_struct _tmp722;_tmp722.tag=0U,_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp722;});void*_tmp2F0[2U];_tmp2F0[0]=& _tmp2F2,_tmp2F0[1]=& _tmp2F3;({struct _dyneither_ptr _tmp83E=({const char*_tmp2F1="_union_%s_%s";_tag_dyneither(_tmp2F1,sizeof(char),13U);});Cyc_aprintf(_tmp83E,_tag_dyneither(_tmp2F0,sizeof(void*),2U));});});
({void*_tmp83F=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));*_tmp2EF=str;_tmp2EF;}));*tagged_member_type=_tmp83F;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2EE->impl))->tagged;};}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1766
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1769
void*_tmp2F5=e->r;void*_tmp2F6=_tmp2F5;struct Cyc_Absyn_Exp*_tmp302;struct _dyneither_ptr*_tmp301;int*_tmp300;struct Cyc_Absyn_Exp*_tmp2FF;struct _dyneither_ptr*_tmp2FE;int*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FC;switch(*((int*)_tmp2F6)){case 14U: _LL1: _tmp2FC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F6)->f2;_LL2:
({void*_tmp2F7=0U;({struct _dyneither_ptr _tmp840=({const char*_tmp2F8="cast on lhs!";_tag_dyneither(_tmp2F8,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp840,_tag_dyneither(_tmp2F7,sizeof(void*),0U));});});case 21U: _LL3: _tmp2FF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F6)->f1;_tmp2FE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F6)->f2;_tmp2FD=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F6)->f4;_LL4:
# 1772
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2FF->topt),_tmp2FE,f_tag,tagged_member_type,clear_read,_tmp2FD);case 22U: _LL5: _tmp302=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F6)->f1;_tmp301=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F6)->f2;_tmp300=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F6)->f4;_LL6: {
# 1775
void*_tmp2F9=Cyc_Tcutil_compress((void*)_check_null(_tmp302->topt));void*_tmp2FA=_tmp2F9;void*_tmp2FB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->tag == 3U){_LLA: _tmp2FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_type;_LLB:
# 1777
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2FB,_tmp301,f_tag,tagged_member_type,clear_read,_tmp300);}else{_LLC: _LLD:
# 1779
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1781
 return 0;}_LL0:;}
# 1794 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1798
struct _tuple1*_tmp303=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp303,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp841=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp841;});else{
# 1808
struct Cyc_Absyn_Exp*_tmp304=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp844=({struct Cyc_Absyn_Exp*_tmp843=_tmp304;struct Cyc_Absyn_Stmt*_tmp842=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp843,_tmp842,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp844;});}{
# 1811
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp847=_tmp303;void*_tmp846=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp845=
Cyc_Toc_push_address_exp(e1);
# 1811
Cyc_Absyn_declare_stmt(_tmp847,_tmp846,_tmp845,
# 1813
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple31{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple32{void*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1817
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp307=({struct Cyc_String_pa_PrintArg_struct _tmp724;_tmp724.tag=0U,({struct _dyneither_ptr _tmp848=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp724.f1=_tmp848;});_tmp724;});void*_tmp305[1U];_tmp305[0]=& _tmp307;({unsigned int _tmp84A=e->loc;struct _dyneither_ptr _tmp849=({const char*_tmp306="exp_to_c: no type for %s";_tag_dyneither(_tmp306,sizeof(char),25U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp84A,_tmp849,_tag_dyneither(_tmp305,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp308=e->r;void*_tmp309=_tmp308;struct Cyc_Absyn_Stmt*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct _dyneither_ptr*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;int _tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;void**_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;int _tmp4A3;int _tmp4A2;struct Cyc_List_List*_tmp4A1;struct Cyc_Absyn_Datatypedecl*_tmp4A0;struct Cyc_Absyn_Datatypefield*_tmp49F;struct _tuple1**_tmp49E;struct Cyc_List_List*_tmp49D;struct Cyc_List_List*_tmp49C;struct Cyc_Absyn_Aggrdecl*_tmp49B;void*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_Exp*_tmp498;void*_tmp497;int _tmp496;struct Cyc_Absyn_Vardecl*_tmp495;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*_tmp493;int _tmp492;struct Cyc_List_List*_tmp491;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_Absyn_Exp*_tmp48E;struct Cyc_Absyn_Exp*_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;struct _dyneither_ptr*_tmp48B;int _tmp48A;int _tmp489;struct Cyc_Absyn_Exp*_tmp488;struct _dyneither_ptr*_tmp487;int _tmp486;int _tmp485;void*_tmp484;struct Cyc_List_List*_tmp483;void*_tmp482;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_Absyn_Exp*_tmp47E;void**_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;int _tmp47B;enum Cyc_Absyn_Coercion _tmp47A;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_List_List*_tmp478;struct Cyc_Absyn_Exp*_tmp477;struct Cyc_Absyn_Exp*_tmp476;int _tmp475;struct Cyc_Absyn_Exp*_tmp474;struct Cyc_List_List*_tmp473;int _tmp472;struct Cyc_List_List*_tmp471;struct Cyc_Absyn_VarargInfo*_tmp470;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_List_List*_tmp46E;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46C;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Exp*_tmp46A;struct Cyc_Absyn_Exp*_tmp469;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp467;struct Cyc_Absyn_Exp*_tmp466;struct Cyc_Absyn_Exp*_tmp465;struct Cyc_Absyn_Exp*_tmp464;struct Cyc_Core_Opt*_tmp463;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Absyn_Exp*_tmp461;enum Cyc_Absyn_Incrementor _tmp460;enum Cyc_Absyn_Primop _tmp45F;struct Cyc_List_List*_tmp45E;struct Cyc_Absyn_Exp*_tmp45D;switch(*((int*)_tmp309)){case 2U: _LL1: _LL2:
# 1823
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp309)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1830
struct Cyc_Absyn_Exp*_tmp30A=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp84B=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30A,_tmp30A))->r;e->r=_tmp84B;});else{
# 1835
({void*_tmp84D=({struct Cyc_Absyn_Exp*_tmp84C=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp84C,({struct Cyc_Absyn_Exp*_tmp30B[3U];_tmp30B[0]=_tmp30A,_tmp30B[1]=_tmp30A,_tmp30B[2]=_tmp30A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp30B,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp84D;});}}else{
# 1837
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1839
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 41U: _LL9: _tmp45D=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LLA:
 Cyc_Toc_exp_to_c(nv,_tmp45D);goto _LL0;case 3U: _LLB: _tmp45F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp45E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LLC: {
# 1845
struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_type,_tmp45E);
# 1847
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp45E);
{enum Cyc_Absyn_Primop _tmp30D=_tmp45F;switch(_tmp30D){case Cyc_Absyn_Numelts: _LL5A: _LL5B: {
# 1850
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E))->hd;
{void*_tmp30E=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp30F=_tmp30E;void*_tmp31D;void*_tmp31C;void*_tmp31B;void*_tmp31A;struct Cyc_Absyn_Exp*_tmp319;switch(*((int*)_tmp30F)){case 4U: _LL6F: _tmp319=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp30F)->f1).num_elts;_LL70:
# 1855
 e->r=((struct Cyc_Absyn_Exp*)_check_null(_tmp319))->r;
goto _LL6E;case 3U: _LL71: _tmp31D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).elt_type;_tmp31C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).ptr_atts).nullable;_tmp31B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).ptr_atts).bounds;_tmp31A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).ptr_atts).zero_term;_LL72: {
# 1858
struct Cyc_Absyn_Exp*_tmp310=({void*_tmp84E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp84E,_tmp31B);});
if(_tmp310 == 0)
# 1861
({void*_tmp851=({struct Cyc_Absyn_Exp*_tmp850=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp850,({struct Cyc_Absyn_Exp*_tmp311[2U];_tmp311[0]=(struct Cyc_Absyn_Exp*)_tmp45E->hd,({
# 1863
struct Cyc_Absyn_Exp*_tmp84F=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp31D),0U);_tmp311[1]=_tmp84F;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp311,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1861
e->r=_tmp851;});else{
# 1865
struct Cyc_Absyn_Exp*_tmp312=_tmp310;
# 1867
if(Cyc_Tcutil_force_type2bool(0,_tmp31A)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp45E->hd);
# 1871
({void*_tmp853=({struct Cyc_Absyn_Exp*_tmp852=function_e;Cyc_Toc_fncall_exp_r(_tmp852,({struct Cyc_Absyn_Exp*_tmp313[2U];_tmp313[0]=(struct Cyc_Absyn_Exp*)_tmp45E->hd,_tmp313[1]=_tmp312;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp313,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp853;});}else{
if(Cyc_Tcutil_force_type2bool(0,_tmp31C)){
if(!Cyc_Evexp_c_can_eval(_tmp312))
({void*_tmp314=0U;({unsigned int _tmp855=e->loc;struct _dyneither_ptr _tmp854=({const char*_tmp315="can't calculate numelts";_tag_dyneither(_tmp315,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp855,_tmp854,_tag_dyneither(_tmp314,sizeof(void*),0U));});});
# 1876
({void*_tmp858=({struct Cyc_Absyn_Exp*_tmp857=arg;struct Cyc_Absyn_Exp*_tmp856=_tmp312;Cyc_Toc_conditional_exp_r(_tmp857,_tmp856,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp858;});}else{
# 1878
e->r=_tmp312->r;goto _LL6E;}}}
# 1881
goto _LL6E;}default: _LL73: _LL74:
# 1883
({struct Cyc_String_pa_PrintArg_struct _tmp318=({struct Cyc_String_pa_PrintArg_struct _tmp725;_tmp725.tag=0U,({
struct _dyneither_ptr _tmp859=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp725.f1=_tmp859;});_tmp725;});void*_tmp316[1U];_tmp316[0]=& _tmp318;({struct _dyneither_ptr _tmp85A=({const char*_tmp317="numelts primop applied to non-array %s";_tag_dyneither(_tmp317,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85A,_tag_dyneither(_tmp316,sizeof(void*),1U));});});}_LL6E:;}
# 1886
goto _LL59;}case Cyc_Absyn_Plus: _LL5C: _LL5D:
# 1891
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp31E=0U;({struct _dyneither_ptr _tmp85B=({const char*_tmp31F="can't do pointer arithmetic at top-level";_tag_dyneither(_tmp31F,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp85B,_tag_dyneither(_tmp31E,sizeof(void*),0U));});});
{void*_tmp320=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp30C))->hd);void*_tmp321=_tmp320;void*_tmp327;void*_tmp326;void*_tmp325;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp321)->tag == 3U){_LL76: _tmp327=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp321)->f1).elt_type;_tmp326=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp321)->f1).ptr_atts).bounds;_tmp325=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp321)->f1).ptr_atts).zero_term;_LL77: {
# 1895
struct Cyc_Absyn_Exp*_tmp322=({void*_tmp85C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp85C,_tmp326);});
if(_tmp322 == 0){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E->tl))->hd;
({void*_tmp85F=({struct Cyc_Absyn_Exp*_tmp85E=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp85E,({struct Cyc_Absyn_Exp*_tmp323[3U];_tmp323[0]=e1,({
struct Cyc_Absyn_Exp*_tmp85D=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp327),0U);_tmp323[1]=_tmp85D;}),_tmp323[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp323,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1899
e->r=_tmp85F;});}else{
# 1902
if(Cyc_Tcutil_force_type2bool(0,_tmp325)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E->tl))->hd;
({void*_tmp861=({struct Cyc_Absyn_Exp*_tmp860=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp860,({struct Cyc_Absyn_Exp*_tmp324[3U];_tmp324[0]=e1,_tmp324[1]=_tmp322,_tmp324[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp324,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp861;});}}
# 1908
goto _LL75;}}else{_LL78: _LL79:
 goto _LL75;}_LL75:;}
# 1911
goto _LL59;case Cyc_Absyn_Minus: _LL5E: _LL5F: {
# 1916
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt((void*)((struct Cyc_List_List*)_check_null(_tmp30C))->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_type((void*)((struct Cyc_List_List*)_check_null(_tmp30C->tl))->hd)){
({void*_tmp863=({struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp862,Cyc_Toc_curr_sp);});e1->r=_tmp863;});
({void*_tmp865=({struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp864,Cyc_Toc_curr_sp);});e2->r=_tmp865;});
({void*_tmp867=({void*_tmp866=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_tmp866;});e1->topt=_tmp867;});
({void*_tmp869=({struct Cyc_Absyn_Exp*_tmp868=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp868,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1924
e->r=_tmp869;});}else{
# 1927
({void*_tmp86D=({struct Cyc_Absyn_Exp*_tmp86C=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp86C,({struct Cyc_Absyn_Exp*_tmp328[3U];_tmp328[0]=e1,({
struct Cyc_Absyn_Exp*_tmp86B=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp328[1]=_tmp86B;}),({
struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp328[2]=_tmp86A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp328,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1927
e->r=_tmp86D;});}}
# 1931
goto _LL59;}case Cyc_Absyn_Eq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Neq: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gt: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Gte: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lt: _LL68: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6A: _LL6B: {
# 1939
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp45E->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp30C))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp30C->tl))->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(t1,& elt_type)){
void*_tmp329=({void*_tmp86E=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp86E,Cyc_Toc_mt_tq);});
({void*_tmp871=({void*_tmp870=_tmp329;Cyc_Toc_cast_it_r(_tmp870,({struct Cyc_Absyn_Exp*_tmp86F=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp86F,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp871;});
e1->topt=_tmp329;}
# 1949
if(Cyc_Tcutil_is_tagged_pointer_type(t2)){
void*_tmp32A=({void*_tmp872=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_tmp872,Cyc_Toc_mt_tq);});
({void*_tmp875=({void*_tmp874=_tmp32A;Cyc_Toc_cast_it_r(_tmp874,({struct Cyc_Absyn_Exp*_tmp873=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp873,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp875;});
e2->topt=_tmp32A;}
# 1954
goto _LL59;}default: _LL6C: _LL6D:
 goto _LL59;}_LL59:;}
# 1957
goto _LL0;}case 5U: _LLD: _tmp461=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp460=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LLE: {
# 1959
void*e2_cyc_typ=(void*)_check_null(_tmp461->topt);
# 1968 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _dyneither_ptr incr_str=({const char*_tmp33D="increment";_tag_dyneither(_tmp33D,sizeof(char),10U);});
if(_tmp460 == Cyc_Absyn_PreDec  || _tmp460 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp876=({const char*_tmp32B="decrement";_tag_dyneither(_tmp32B,sizeof(char),10U);});incr_str=_tmp876;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp461,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_String_pa_PrintArg_struct _tmp32E=({struct Cyc_String_pa_PrintArg_struct _tmp726;_tmp726.tag=0U,_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp726;});void*_tmp32C[1U];_tmp32C[0]=& _tmp32E;({unsigned int _tmp878=e->loc;struct _dyneither_ptr _tmp877=({const char*_tmp32D="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp32D,sizeof(char),74U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp878,_tmp877,_tag_dyneither(_tmp32C,sizeof(void*),1U));});});
# 1976
if(Cyc_Toc_is_tagged_union_project(_tmp461,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
{enum Cyc_Absyn_Incrementor _tmp32F=_tmp460;switch(_tmp32F){case Cyc_Absyn_PreInc: _LL7B: _LL7C:
 op=0U;goto _LL7A;case Cyc_Absyn_PreDec: _LL7D: _LL7E:
 op=2U;goto _LL7A;default: _LL7F: _LL80:
({struct Cyc_String_pa_PrintArg_struct _tmp332=({struct Cyc_String_pa_PrintArg_struct _tmp727;_tmp727.tag=0U,_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp727;});void*_tmp330[1U];_tmp330[0]=& _tmp332;({unsigned int _tmp87A=e->loc;struct _dyneither_ptr _tmp879=({const char*_tmp331="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp331,sizeof(char),58U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmp87A,_tmp879,_tag_dyneither(_tmp330,sizeof(void*),1U));});});}_LL7A:;}
# 1986
({void*_tmp87C=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp334=_cycalloc(sizeof(*_tmp334));_tmp334->tag=4U,_tmp334->f1=_tmp461,({struct Cyc_Core_Opt*_tmp87B=({struct Cyc_Core_Opt*_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333->v=(void*)op;_tmp333;});_tmp334->f2=_tmp87B;}),_tmp334->f3=one;_tmp334;});e->r=_tmp87C;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1990
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp461);
Cyc_Toc_set_lhs(nv,0);{
# 1995
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp460 == Cyc_Absyn_PostInc  || _tmp460 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2001
if(_tmp460 == Cyc_Absyn_PreDec  || _tmp460 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp881=({struct Cyc_Absyn_Exp*_tmp880=fn_e;Cyc_Toc_fncall_exp_r(_tmp880,({struct Cyc_Absyn_Exp*_tmp335[3U];({struct Cyc_Absyn_Exp*_tmp87F=Cyc_Toc_push_address_exp(_tmp461);_tmp335[0]=_tmp87F;}),({
struct Cyc_Absyn_Exp*_tmp87E=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp335[1]=_tmp87E;}),({
struct Cyc_Absyn_Exp*_tmp87D=Cyc_Absyn_signed_int_exp(change,0U);_tmp335[2]=_tmp87D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp335,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2003
e->r=_tmp881;});}else{
# 2006
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2012
if(_tmp460 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp336=Cyc_Toc_temp_var();
void*_tmp337=({void*_tmp882=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_tmp882,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp338=Cyc_Toc_push_address_exp(_tmp461);
struct Cyc_Absyn_Exp*_tmp339=({struct Cyc_Absyn_Exp*_tmp883=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp336,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp883,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
({struct Cyc_Absyn_Exp*_tmp886=({struct Cyc_Absyn_Exp*_tmp885=({struct Cyc_Absyn_Exp*_tmp884=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp336,0U),0U);Cyc_Absyn_neq_exp(_tmp884,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2019
Cyc_Absyn_and_exp(_tmp885,_tmp339,0U);});_tmp339=_tmp886;});{
# 2021
struct Cyc_Absyn_Stmt*_tmp33A=({struct Cyc_Absyn_Exp*_tmp888=_tmp339;struct Cyc_Absyn_Stmt*_tmp887=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp336,0U),0U),0U),0U);
# 2021
Cyc_Absyn_ifthenelse_stmt(_tmp888,_tmp887,
# 2023
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp88A=({struct Cyc_Absyn_Stmt*_tmp889=_tmp33A;Cyc_Absyn_seq_stmt(_tmp889,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp336,0U),0U),0U),0U);});_tmp33A=_tmp88A;});
({void*_tmp88B=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp336,_tmp337,_tmp338,_tmp33A,0U),0U))->r;e->r=_tmp88B;});};}else{
# 2028
struct Cyc_Toc_functionSet*_tmp33B=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp33B,_tmp461);
({void*_tmp88F=({struct Cyc_Absyn_Exp*_tmp88E=fn_e;Cyc_Toc_fncall_exp_r(_tmp88E,({struct Cyc_Absyn_Exp*_tmp33C[2U];({struct Cyc_Absyn_Exp*_tmp88D=Cyc_Toc_push_address_exp(_tmp461);_tmp33C[0]=_tmp88D;}),({
struct Cyc_Absyn_Exp*_tmp88C=Cyc_Absyn_signed_int_exp(1,0U);_tmp33C[1]=_tmp88C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33C,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2030
e->r=_tmp88F;});}}else{
# 2033
if(elt_typ == Cyc_Absyn_void_type  && !Cyc_Absyn_is_lvalue(_tmp461)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp461,0,Cyc_Toc_incr_lvalue,_tmp460);
e->r=_tmp461->r;}}}
# 2037
goto _LL0;};}case 4U: _LLF: _tmp464=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp463=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp462=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LL10: {
# 2056 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp464->topt);
void*e2_old_typ=(void*)_check_null(_tmp462->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(_tmp464,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp464);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp462);
({void*_tmp890=Cyc_Toc_tagged_union_assignop(_tmp464,e1_old_typ,_tmp463,_tmp462,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp890;});
# 2067
return;}{
# 2069
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp464,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp464,_tmp463,_tmp462,ptr_type,is_dyneither,elt_type);
# 2075
return;}{
# 2079
int e1_poly=Cyc_Toc_is_poly_project(_tmp464);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp464);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp462);{
# 2085
int done=0;
if(_tmp463 != 0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_tagged_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp33E=(enum Cyc_Absyn_Primop)_tmp463->v;enum Cyc_Absyn_Primop _tmp33F=_tmp33E;switch(_tmp33F){case Cyc_Absyn_Plus: _LL82: _LL83:
 change=_tmp462;goto _LL81;case Cyc_Absyn_Minus: _LL84: _LL85:
# 2093
({struct Cyc_Absyn_Exp*_tmp891=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp462,0U);change=_tmp891;});goto _LL81;default: _LL86: _LL87:
({void*_tmp340=0U;({struct _dyneither_ptr _tmp892=({const char*_tmp341="bad t ? pointer arithmetic";_tag_dyneither(_tmp341,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp892,_tag_dyneither(_tmp340,sizeof(void*),0U));});});}_LL81:;}
# 2096
done=1;{
# 2098
struct Cyc_Absyn_Exp*_tmp342=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp896=({struct Cyc_Absyn_Exp*_tmp895=_tmp342;Cyc_Toc_fncall_exp_r(_tmp895,({struct Cyc_Absyn_Exp*_tmp343[3U];({struct Cyc_Absyn_Exp*_tmp894=Cyc_Toc_push_address_exp(_tmp464);_tmp343[0]=_tmp894;}),({
struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp343[1]=_tmp893;}),_tmp343[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp343,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2099
e->r=_tmp896;});};}else{
# 2102
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 2105
enum Cyc_Absyn_Primop _tmp344=(enum Cyc_Absyn_Primop)_tmp463->v;enum Cyc_Absyn_Primop _tmp345=_tmp344;if(_tmp345 == Cyc_Absyn_Plus){_LL89: _LL8A:
# 2107
 done=1;
({void*_tmp898=({struct Cyc_Absyn_Exp*_tmp897=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp464);Cyc_Toc_fncall_exp_r(_tmp897,({struct Cyc_Absyn_Exp*_tmp346[2U];_tmp346[0]=_tmp464,_tmp346[1]=_tmp462;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp346,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp898;});
goto _LL88;}else{_LL8B: _LL8C:
({void*_tmp347=0U;({struct _dyneither_ptr _tmp899=({const char*_tmp348="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp348,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp899,_tag_dyneither(_tmp347,sizeof(void*),0U));});});}_LL88:;}}}
# 2114
if(!done){
# 2116
if(e1_poly)
({void*_tmp89B=({void*_tmp89A=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp89A,Cyc_Absyn_new_exp(_tmp462->r,0U));});_tmp462->r=_tmp89B;});
# 2122
if(!Cyc_Absyn_is_lvalue(_tmp464)){
({struct Cyc_Absyn_Exp*_tmp89C=_tmp464;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple30*),struct _tuple30*f_env))Cyc_Toc_lvalue_assign)(_tmp89C,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp34A=1;struct _tuple30*_tmp349=_cycalloc(_check_times(_tmp34A,sizeof(struct _tuple30)));(_tmp349[0]).f1=_tmp463,(_tmp349[0]).f2=_tmp462;_tmp349;}));});
e->r=_tmp464->r;}}
# 2127
goto _LL0;};};};}case 6U: _LL11: _tmp467=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp466=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp465=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LL12:
# 2129
 Cyc_Toc_exp_to_c(nv,_tmp467);
Cyc_Toc_exp_to_c(nv,_tmp466);
Cyc_Toc_exp_to_c(nv,_tmp465);
goto _LL0;case 7U: _LL13: _tmp469=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp468=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL14:
 _tmp46B=_tmp469;_tmp46A=_tmp468;goto _LL16;case 8U: _LL15: _tmp46B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp46A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL16:
 _tmp46D=_tmp46B;_tmp46C=_tmp46A;goto _LL18;case 9U: _LL17: _tmp46D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp46C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL18:
# 2136
 Cyc_Toc_exp_to_c(nv,_tmp46D);
Cyc_Toc_exp_to_c(nv,_tmp46C);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3 == 0){_LL19: _tmp46F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp46E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL1A: {
# 2140
void*e1_typ=(void*)_check_null(_tmp46F->topt);
Cyc_Toc_exp_to_c(nv,_tmp46F);
if(Cyc_Toc_do_null_check(e))
# 2144
({void*_tmp8A0=({void*_tmp89F=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp89F,({
struct Cyc_Absyn_Exp*_tmp89E=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp89E,({struct Cyc_List_List*_tmp34B=_cycalloc(sizeof(*_tmp34B));
({struct Cyc_Absyn_Exp*_tmp89D=Cyc_Absyn_copy_exp(_tmp46F);_tmp34B->hd=_tmp89D;}),_tmp34B->tl=0;_tmp34B;}),0U);}));});
# 2144
_tmp46F->r=_tmp8A0;});
# 2147
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp46E);
goto _LL0;}}else{_LL1B: _tmp474=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp473=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp472=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3)->num_varargs;_tmp471=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3)->injectors;_tmp470=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3)->vai;_LL1C: {
# 2158 "toc.cyc"
struct _RegionHandle _tmp34C=_new_region("r");struct _RegionHandle*r=& _tmp34C;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp472,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp470->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2166
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp473);
int num_normargs=num_args - _tmp472;
# 2170
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp473=_tmp473->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp473))->hd);
({struct Cyc_List_List*_tmp8A1=({struct Cyc_List_List*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->hd=(struct Cyc_Absyn_Exp*)_tmp473->hd,_tmp34D->tl=new_args;_tmp34D;});new_args=_tmp8A1;});}}
# 2175
({struct Cyc_List_List*_tmp8A5=({struct Cyc_List_List*_tmp34F=_cycalloc(sizeof(*_tmp34F));({struct Cyc_Absyn_Exp*_tmp8A4=({struct Cyc_Absyn_Exp*_tmp8A3=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8A3,({struct Cyc_Absyn_Exp*_tmp34E[3U];_tmp34E[0]=argvexp,({
# 2177
struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_tmp34E[1]=_tmp8A2;}),_tmp34E[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2175
_tmp34F->hd=_tmp8A4;}),_tmp34F->tl=new_args;_tmp34F;});new_args=_tmp8A5;});
# 2180
({struct Cyc_List_List*_tmp8A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp8A6;});{
# 2182
void*e1_typ=(void*)_check_null(_tmp474->topt);
Cyc_Toc_exp_to_c(nv,_tmp474);
if(Cyc_Toc_do_null_check(e))
# 2186
({void*_tmp8AA=({void*_tmp8A9=Cyc_Toc_typ_to_c(e1_typ);Cyc_Toc_cast_it_r(_tmp8A9,({
struct Cyc_Absyn_Exp*_tmp8A8=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8A8,({struct Cyc_List_List*_tmp350=_cycalloc(sizeof(*_tmp350));
({struct Cyc_Absyn_Exp*_tmp8A7=Cyc_Absyn_copy_exp(_tmp474);_tmp350->hd=_tmp8A7;}),_tmp350->tl=0;_tmp350;}),0U);}));});
# 2186
_tmp474->r=_tmp8AA;});{
# 2189
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp474,new_args,0U),0U);
# 2192
if(_tmp470->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp351=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp470->type));void*_tmp352=_tmp351;struct Cyc_Absyn_Datatypedecl*_tmp355;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352)->f1)->f1).KnownDatatype).tag == 2){_LL8E: _tmp355=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp352)->f1)->f1).KnownDatatype).val;_LL8F:
 tud=_tmp355;goto _LL8D;}else{goto _LL90;}}else{goto _LL90;}}else{_LL90: _LL91:
({void*_tmp353=0U;({struct _dyneither_ptr _tmp8AB=({const char*_tmp354="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp354,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8AB,_tag_dyneither(_tmp353,sizeof(void*),0U));});});}_LL8D:;}{
# 2198
struct _dyneither_ptr vs=({unsigned int _tmp366=(unsigned int)_tmp472;struct _tuple1**_tmp365=({struct _RegionHandle*_tmp8AC=r;_region_malloc(_tmp8AC,_check_times(_tmp366,sizeof(struct _tuple1*)));});({{unsigned int _tmp728=(unsigned int)_tmp472;unsigned int i;for(i=0;i < _tmp728;++ i){({struct _tuple1*_tmp8AD=Cyc_Toc_temp_var();_tmp365[i]=_tmp8AD;});}}0;});_tag_dyneither(_tmp365,sizeof(struct _tuple1*),_tmp366);});
# 2200
if(_tmp472 != 0){
# 2204
struct Cyc_List_List*_tmp356=0;
{int i=_tmp472 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp8AF=({struct Cyc_List_List*_tmp357=_cycalloc(sizeof(*_tmp357));({struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp357->hd=_tmp8AE;}),_tmp357->tl=_tmp356;_tmp357;});_tmp356=_tmp8AF;});}}
({struct Cyc_Absyn_Stmt*_tmp8B3=({struct _tuple1*_tmp8B2=argv;void*_tmp8B1=arr_type;struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Absyn_array_exp(_tmp356,0U);Cyc_Absyn_declare_stmt(_tmp8B2,_tmp8B1,_tmp8B0,s,0U);});s=_tmp8B3;});
# 2209
({struct Cyc_List_List*_tmp8B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp473);_tmp473=_tmp8B4;});
({struct Cyc_List_List*_tmp8B5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp471);_tmp471=_tmp8B5;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp473)- 1;
for(0;_tmp473 != 0;(_tmp473=_tmp473->tl,_tmp471=_tmp471->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp473->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp358=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp471))->hd;struct Cyc_Absyn_Datatypefield*_tmp359=_tmp358;struct _tuple1*_tmp35F;struct Cyc_List_List*_tmp35E;_LL93: _tmp35F=_tmp359->name;_tmp35E=_tmp359->typs;_LL94:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp35E))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp8B6;});{
# 2223
struct _tuple1*_tmp35A=Cyc_Toc_collapse_qvars(_tmp35F,tud->name);
struct Cyc_List_List*_tmp35B=({struct _tuple19*_tmp35D[2U];({struct _tuple19*_tmp8BA=({struct _dyneither_ptr*_tmp8B9=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp8B9,Cyc_Toc_datatype_tag(tud,_tmp35F));});_tmp35D[0]=_tmp8BA;}),({
struct _tuple19*_tmp8B8=({struct _dyneither_ptr*_tmp8B7=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp8B7,arg);});_tmp35D[1]=_tmp8B8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35D,sizeof(struct _tuple19*),2U));});
({struct Cyc_Absyn_Stmt*_tmp8BE=({struct _tuple1*_tmp8BD=var;void*_tmp8BC=Cyc_Absyn_strctq(_tmp35A);struct Cyc_Absyn_Exp*_tmp8BB=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp35C=_cycalloc(sizeof(*_tmp35C));_tmp35C->tag=29U,_tmp35C->f1=_tmp35A,_tmp35C->f2=0,_tmp35C->f3=_tmp35B,_tmp35C->f4=0;_tmp35C;}),0U);
# 2226
Cyc_Absyn_declare_stmt(_tmp8BD,_tmp8BC,_tmp8BB,s,0U);});s=_tmp8BE;});};};}};}else{
# 2234
struct Cyc_List_List*_tmp360=({struct _tuple31*_tmp361[3U];({struct _tuple31*_tmp8C7=({struct _tuple31*_tmp362=_cycalloc(sizeof(*_tmp362));({struct _dyneither_ptr _tmp8C6=_tag_dyneither(0,0,0);_tmp362->f1=_tmp8C6;}),({struct Cyc_Absyn_Exp*_tmp8C5=Cyc_Absyn_uint_exp(0U,0U);_tmp362->f2=_tmp8C5;});_tmp362;});_tmp361[0]=_tmp8C7;}),({
struct _tuple31*_tmp8C4=({struct _tuple31*_tmp363=_cycalloc(sizeof(*_tmp363));({struct _dyneither_ptr _tmp8C3=_tag_dyneither(0,0,0);_tmp363->f1=_tmp8C3;}),({struct Cyc_Absyn_Exp*_tmp8C2=Cyc_Absyn_uint_exp(0U,0U);_tmp363->f2=_tmp8C2;});_tmp363;});_tmp361[1]=_tmp8C4;}),({
struct _tuple31*_tmp8C1=({struct _tuple31*_tmp364=_cycalloc(sizeof(*_tmp364));({struct _dyneither_ptr _tmp8C0=_tag_dyneither(0,0,0);_tmp364->f1=_tmp8C0;}),({struct Cyc_Absyn_Exp*_tmp8BF=Cyc_Absyn_uint_exp(0U,0U);_tmp364->f2=_tmp8BF;});_tmp364;});_tmp361[2]=_tmp8C1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp361,sizeof(struct _tuple31*),3U));});
({struct Cyc_Absyn_Stmt*_tmp8CB=({struct _tuple1*_tmp8CA=argv;void*_tmp8C9=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8C8=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8CA,_tmp8C9,_tmp8C8,s,0U);});s=_tmp8CB;});}};}else{
# 2242
if(_tmp472 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp473 != 0;_tmp473=_tmp473->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp473->hd);
({struct Cyc_List_List*_tmp8CC=({struct Cyc_List_List*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->hd=(struct Cyc_Absyn_Exp*)_tmp473->hd,_tmp367->tl=array_args;_tmp367;});array_args=_tmp8CC;});}{
# 2248
struct Cyc_Absyn_Exp*_tmp368=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp8CD=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp368,s,0U);s=_tmp8CD;});};}else{
# 2251
({struct Cyc_Absyn_Stmt*_tmp8D1=({struct _tuple1*_tmp8D0=argv;void*_tmp8CF=Cyc_Absyn_void_star_type();struct Cyc_Absyn_Exp*_tmp8CE=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp8D0,_tmp8CF,_tmp8CE,s,0U);});s=_tmp8D1;});}}
# 2254
({void*_tmp8D2=Cyc_Toc_stmt_exp_r(s);e->r=_tmp8D2;});};};}
# 2256
_npop_handler(0U);goto _LL0;
# 2158
;_pop_region(r);}}case 11U: _LL1D: _tmp476=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp475=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL1E:
# 2259
 Cyc_Toc_exp_to_c(nv,_tmp476);{
struct Cyc_Absyn_Exp*_tmp369=_tmp475?Cyc_Toc_newrethrow_exp(_tmp476): Cyc_Toc_newthrow_exp(_tmp476);
({void*_tmp8D4=({void*_tmp8D3=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp8D3,_tmp369,0U);})->r;e->r=_tmp8D4;});
goto _LL0;};case 12U: _LL1F: _tmp477=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL20:
 Cyc_Toc_exp_to_c(nv,_tmp477);goto _LL0;case 13U: _LL21: _tmp479=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp478=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL22:
# 2265
 Cyc_Toc_exp_to_c(nv,_tmp479);
# 2274 "toc.cyc"
for(0;_tmp478 != 0;_tmp478=_tmp478->tl){
enum Cyc_Absyn_KindQual _tmp36A=(Cyc_Tcutil_type_kind((void*)_tmp478->hd))->kind;
if(_tmp36A != Cyc_Absyn_EffKind  && _tmp36A != Cyc_Absyn_RgnKind){
{void*_tmp36B=Cyc_Tcutil_compress((void*)_tmp478->hd);void*_tmp36C=_tmp36B;switch(*((int*)_tmp36C)){case 2U: _LL96: _LL97:
 goto _LL99;case 0U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36C)->f1)->tag == 18U){_LL98: _LL99:
 continue;}else{goto _LL9A;}default: _LL9A: _LL9B:
# 2281
({void*_tmp8D6=({void*_tmp8D5=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp8D5,_tmp479,0U);})->r;e->r=_tmp8D6;});
goto _LL95;}_LL95:;}
# 2284
break;}}
# 2287
goto _LL0;case 14U: _LL23: _tmp47D=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp47C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp47B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp47A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL24: {
# 2289
void*old_t2=(void*)_check_null(_tmp47C->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp47D;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp47D=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp47C);
# 2296
{struct _tuple32 _tmp36D=({struct _tuple32 _tmp72A;({void*_tmp8D8=Cyc_Tcutil_compress(old_t2);_tmp72A.f1=_tmp8D8;}),({void*_tmp8D7=Cyc_Tcutil_compress(new_typ);_tmp72A.f2=_tmp8D7;});_tmp72A;});struct _tuple32 _tmp36E=_tmp36D;struct Cyc_Absyn_PtrInfo _tmp392;struct Cyc_Absyn_PtrInfo _tmp391;struct Cyc_Absyn_PtrInfo _tmp390;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36E.f1)->tag == 3U)switch(*((int*)_tmp36E.f2)){case 3U: _LL9D: _tmp391=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36E.f1)->f1;_tmp390=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36E.f2)->f1;_LL9E: {
# 2298
struct Cyc_Absyn_Exp*_tmp36F=({void*_tmp8D9=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8D9,(_tmp391.ptr_atts).bounds);});
struct Cyc_Absyn_Exp*_tmp370=({void*_tmp8DA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8DA,(_tmp390.ptr_atts).bounds);});
int _tmp371=Cyc_Tcutil_force_type2bool(0,(_tmp391.ptr_atts).zero_term);
int _tmp372=Cyc_Tcutil_force_type2bool(0,(_tmp390.ptr_atts).zero_term);
{struct _tuple33 _tmp373=({struct _tuple33 _tmp729;_tmp729.f1=_tmp36F,_tmp729.f2=_tmp370;_tmp729;});struct _tuple33 _tmp374=_tmp373;if(_tmp374.f1 != 0){if(_tmp374.f2 != 0){_LLA4: _LLA5:
# 2305
 if(Cyc_Toc_do_null_check(e))
({void*_tmp8DD=({void*_tmp8DC=*_tmp47D;Cyc_Toc_cast_it_r(_tmp8DC,({struct Cyc_Absyn_Exp*_tmp8DB=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp8DB,({struct Cyc_List_List*_tmp375=_cycalloc(sizeof(*_tmp375));
_tmp375->hd=_tmp47C,_tmp375->tl=0;_tmp375;}),0U);}));});
# 2306
e->r=_tmp8DD;});else{
# 2308
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp47C->r;}
# 2314
goto _LLA3;}else{_LLA6: _LLA7: {
# 2316
struct Cyc_Absyn_Exp*_tmp376=(struct Cyc_Absyn_Exp*)_check_null(_tmp36F);
struct _tuple13 _tmp377=Cyc_Evexp_eval_const_uint_exp(_tmp376);struct _tuple13 _tmp378=_tmp377;unsigned int _tmp386;int _tmp385;_LLAD: _tmp386=_tmp378.f1;_tmp385=_tmp378.f2;_LLAE:;
if(Cyc_Toc_is_toplevel(nv)){
# 2322
if((_tmp371  && !(_tmp390.elt_tq).real_const) && !_tmp372)
({struct Cyc_Absyn_Exp*_tmp8DF=({struct Cyc_Absyn_Exp*_tmp8DE=_tmp376;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8DE,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp376=_tmp8DF;});
({void*_tmp8E0=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp376,_tmp47C))->r;e->r=_tmp8E0;});}else{
# 2327
if(_tmp371){
# 2332
struct _tuple1*_tmp379=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp37A=Cyc_Absyn_var_exp(_tmp379,0U);
struct Cyc_Absyn_Exp*arg3;
# 2337
{void*_tmp37B=_tmp47C->r;void*_tmp37C=_tmp37B;struct Cyc_Absyn_Vardecl*_tmp37F;struct Cyc_Absyn_Vardecl*_tmp37E;switch(*((int*)_tmp37C)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp37C)->f1).Wstring_c).tag){case 8U: _LLB0: _LLB1:
 arg3=_tmp376;goto _LLAF;case 9U: _LLB2: _LLB3:
 arg3=_tmp376;goto _LLAF;default: goto _LLB8;}case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37C)->f1)){case 1U: _LLB4: _tmp37E=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37C)->f1)->f1;_LLB5:
 _tmp37F=_tmp37E;goto _LLB7;case 4U: _LLB6: _tmp37F=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp37C)->f1)->f1;_LLB7:
# 2342
 if(!Cyc_Tcutil_is_array_type(_tmp37F->type))
goto _LLB9;
arg3=_tmp376;
goto _LLAF;default: goto _LLB8;}default: _LLB8: _LLB9:
# 2349
 if(_tmp385  && _tmp386 != 1)
arg3=_tmp376;else{
# 2352
({struct Cyc_Absyn_Exp*_tmp8E4=({struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp47C);Cyc_Absyn_fncall_exp(_tmp8E3,({struct Cyc_Absyn_Exp*_tmp37D[2U];({
struct Cyc_Absyn_Exp*_tmp8E2=({void*_tmp8E1=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it(_tmp8E1,_tmp37A);});_tmp37D[0]=_tmp8E2;}),_tmp37D[1]=_tmp376;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp37D,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2352
arg3=_tmp8E4;});}
# 2354
goto _LLAF;}_LLAF:;}
# 2356
if(!_tmp372  && !(_tmp390.elt_tq).real_const)
# 2359
({struct Cyc_Absyn_Exp*_tmp8E6=({struct Cyc_Absyn_Exp*_tmp8E5=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp8E5,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp8E6;});{
# 2361
struct Cyc_Absyn_Exp*_tmp380=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp390.elt_type),0U);
struct Cyc_Absyn_Exp*_tmp381=({struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8E7,({struct Cyc_Absyn_Exp*_tmp383[3U];_tmp383[0]=_tmp37A,_tmp383[1]=_tmp380,_tmp383[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp383,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp382=Cyc_Absyn_exp_stmt(_tmp381,0U);
({struct Cyc_Absyn_Stmt*_tmp8EB=({struct _tuple1*_tmp8EA=_tmp379;void*_tmp8E9=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp8E8=_tmp47C;Cyc_Absyn_declare_stmt(_tmp8EA,_tmp8E9,_tmp8E8,_tmp382,0U);});_tmp382=_tmp8EB;});
({void*_tmp8EC=Cyc_Toc_stmt_exp_r(_tmp382);e->r=_tmp8EC;});};}else{
# 2368
({void*_tmp8EF=({struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp8EE,({struct Cyc_Absyn_Exp*_tmp384[3U];_tmp384[0]=_tmp47C,({
# 2370
struct Cyc_Absyn_Exp*_tmp8ED=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp390.elt_type),0U);_tmp384[1]=_tmp8ED;}),_tmp384[2]=_tmp376;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp384,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2368
e->r=_tmp8EF;});}}
# 2373
goto _LLA3;}}}else{if(_tmp374.f2 != 0){_LLA8: _LLA9: {
# 2383 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp387=(struct Cyc_Absyn_Exp*)_check_null(_tmp370);
if(_tmp371  && !_tmp372)
({struct Cyc_Absyn_Exp*_tmp8F1=({struct Cyc_Absyn_Exp*_tmp8F0=_tmp370;Cyc_Absyn_add_exp(_tmp8F0,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp387=_tmp8F1;});{
# 2390
struct Cyc_Absyn_Exp*_tmp388=({struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Toc__untag_dyneither_ptr_e;Cyc_Absyn_fncall_exp(_tmp8F3,({struct Cyc_Absyn_Exp*_tmp38A[3U];_tmp38A[0]=_tmp47C,({
# 2392
struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp391.elt_type),0U);_tmp38A[1]=_tmp8F2;}),_tmp38A[2]=_tmp387;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp38A,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2394
if(Cyc_Toc_do_null_check(e))
({void*_tmp8F6=({struct Cyc_Absyn_Exp*_tmp8F5=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp8F5,({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));
({struct Cyc_Absyn_Exp*_tmp8F4=Cyc_Absyn_copy_exp(_tmp388);_tmp389->hd=_tmp8F4;}),_tmp389->tl=0;_tmp389;}));});
# 2395
_tmp388->r=_tmp8F6;});
# 2397
({void*_tmp8F7=Cyc_Toc_cast_it_r(*_tmp47D,_tmp388);e->r=_tmp8F7;});
goto _LLA3;};}}else{_LLAA: _LLAB:
# 2402
 if((_tmp371  && !_tmp372) && !(_tmp390.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp38B=0U;({struct _dyneither_ptr _tmp8F8=({const char*_tmp38C="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp38C,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp8F8,_tag_dyneither(_tmp38B,sizeof(void*),0U));});});
({void*_tmp8FC=({struct Cyc_Absyn_Exp*_tmp8FB=Cyc_Toc__dyneither_ptr_decrease_size_e;Cyc_Toc_fncall_exp_r(_tmp8FB,({struct Cyc_Absyn_Exp*_tmp38D[3U];_tmp38D[0]=_tmp47C,({
struct Cyc_Absyn_Exp*_tmp8FA=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(_tmp391.elt_type),0U);_tmp38D[1]=_tmp8FA;}),({
struct Cyc_Absyn_Exp*_tmp8F9=Cyc_Absyn_uint_exp(1U,0U);_tmp38D[2]=_tmp8F9;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp38D,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2405
e->r=_tmp8FC;});}
# 2409
goto _LLA3;}}_LLA3:;}
# 2411
goto _LL9C;}case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36E.f2)->f1)->tag == 1U){_LL9F: _tmp392=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36E.f1)->f1;_LLA0:
# 2413
{struct Cyc_Absyn_Exp*_tmp38E=({void*_tmp8FD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp8FD,(_tmp392.ptr_atts).bounds);});struct Cyc_Absyn_Exp*_tmp38F=_tmp38E;if(_tmp38F == 0){_LLBB: _LLBC:
# 2415
({void*_tmp8FF=({struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_new_exp(_tmp47C->r,_tmp47C->loc);Cyc_Toc_aggrmember_exp_r(_tmp8FE,Cyc_Toc_curr_sp);});_tmp47C->r=_tmp8FF;});
_tmp47C->topt=new_typ_c;
goto _LLBA;}else{_LLBD: _LLBE:
 goto _LLBA;}_LLBA:;}
# 2420
goto _LL9C;}else{goto _LLA1;}default: goto _LLA1;}else{_LLA1: _LLA2:
# 2422
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp47C->r;
goto _LL9C;}_LL9C:;}
# 2426
goto _LL0;}case 15U: _LL25: _tmp47E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL26:
# 2429
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp47E);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp47E)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp47E,0,Cyc_Toc_address_lvalue,1);
# 2435
({void*_tmp901=({void*_tmp900=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp900,_tmp47E);});e->r=_tmp901;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind((void*)_check_null(_tmp47E->topt))))
# 2439
({void*_tmp903=({void*_tmp902=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp902,_tmp47E);});e->r=_tmp903;});}
# 2441
goto _LL0;case 16U: _LL27: _tmp480=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp47F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL28:
# 2448
 Cyc_Toc_exp_to_c(nv,_tmp47F);{
# 2450
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp47F->topt));
struct _tuple1*_tmp393=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp394=Cyc_Absyn_var_exp(_tmp393,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp395=Cyc_Tcutil_compress(elt_typ);void*_tmp396=_tmp395;void*_tmp397;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->tag == 4U){_LLC0: _tmp397=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).elt_type;_LLC1:
# 2456
({void*_tmp904=Cyc_Toc_typ_to_c(_tmp397);elt_typ=_tmp904;});
({struct Cyc_Absyn_Exp*_tmp905=Cyc_Absyn_copy_exp(_tmp394);lhs=_tmp905;});
goto _LLBF;}else{_LLC2: _LLC3:
# 2460
({struct Cyc_Absyn_Exp*_tmp906=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp394),0U);lhs=_tmp906;});
goto _LLBF;}_LLBF:;}{
# 2463
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp47F);
struct _tuple1*_tmp398=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp399=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp398),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp909=({struct Cyc_Absyn_Exp*_tmp908=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp908,({struct Cyc_Absyn_Exp*_tmp39A[2U];_tmp39A[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp399),({
struct Cyc_Absyn_Exp*_tmp907=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp39A[1]=_tmp907;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39A,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2468
mexp=_tmp909;});else{
# 2471
({struct Cyc_Absyn_Exp*_tmp90A=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp394,0U),0U);mexp=_tmp90A;});}{
# 2474
struct Cyc_Absyn_Exp*_tmp39B=Cyc_Toc_get_varsizeexp(_tmp47F);
if(_tmp39B != 0)
({struct Cyc_Absyn_Exp*_tmp90B=Cyc_Absyn_add_exp(mexp,_tmp39B,0U);mexp=_tmp90B;});
# 2478
if(_tmp480 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp90C=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp90C;});else{
# 2481
struct Cyc_Absyn_Exp*r=_tmp480;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp90D=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp90D;});}{
# 2485
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp912=({struct Cyc_Absyn_Exp*_tmp911=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp911,({struct Cyc_Absyn_Exp*_tmp39C[3U];({
struct Cyc_Absyn_Exp*_tmp910=Cyc_Absyn_copy_exp(_tmp394);_tmp39C[0]=_tmp910;}),({
struct Cyc_Absyn_Exp*_tmp90F=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_tmp39C[1]=_tmp90F;}),
(unsigned int)_tmp399?_tmp39C[2]=_tmp399:({struct Cyc_Absyn_Exp*_tmp90E=Cyc_Absyn_uint_exp(1U,0U);_tmp39C[2]=_tmp90E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2487
answer=_tmp912;});else{
# 2492
({struct Cyc_Absyn_Exp*_tmp913=Cyc_Absyn_copy_exp(_tmp394);answer=_tmp913;});}
({void*_tmp914=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->tag=Cyc_Toc_Dest,_tmp39D->f1=_tmp394;_tmp39D;});e->annot=_tmp914;});{
struct Cyc_Absyn_Stmt*_tmp39E=({struct _tuple1*_tmp918=_tmp393;void*_tmp917=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp916=mexp;Cyc_Absyn_declare_stmt(_tmp918,_tmp917,_tmp916,({
struct Cyc_Absyn_Stmt*_tmp915=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp915,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2498
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp919=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp398),Cyc_Absyn_uint_type,array_len,_tmp39E,0U);_tmp39E=_tmp919;});
({void*_tmp91A=Cyc_Toc_stmt_exp_r(_tmp39E);e->r=_tmp91A;});
if(_tmp39B != 0)
({void*_tmp91C=({void*_tmp91B=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp91B,Cyc_Absyn_copy_exp(e));});e->r=_tmp91C;});
goto _LL0;};};};};};case 18U: _LL29: _tmp481=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL2A: {
# 2507
int _tmp39F=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp481);
Cyc_Toc_set_in_sizeof(nv,_tmp39F);
goto _LL0;}case 17U: _LL2B: _tmp482=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL2C:
({void*_tmp91E=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->tag=17U,({void*_tmp91D=Cyc_Toc_typ_to_c(_tmp482);_tmp3A0->f1=_tmp91D;});_tmp3A0;});e->r=_tmp91E;});goto _LL0;case 19U: _LL2D: _tmp484=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp483=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL2E: {
# 2513
void*_tmp3A1=_tmp484;
struct Cyc_List_List*_tmp3A2=_tmp483;
for(0;_tmp3A2 != 0;_tmp3A2=_tmp3A2->tl){
void*_tmp3A3=(void*)_tmp3A2->hd;void*_tmp3A4=_tmp3A3;unsigned int _tmp3B6;struct _dyneither_ptr*_tmp3B5;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3A4)->tag == 0U){_LLC5: _tmp3B5=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3A4)->f1;_LLC6:
 goto _LLC4;}else{_LLC7: _tmp3B6=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3A4)->f1;_LLC8:
# 2519
{void*_tmp3A5=Cyc_Tcutil_compress(_tmp3A1);void*_tmp3A6=_tmp3A5;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*_tmp3B3;struct Cyc_Absyn_Datatypefield*_tmp3B2;union Cyc_Absyn_AggrInfo _tmp3B1;switch(*((int*)_tmp3A6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A6)->f1)){case 20U: _LLCA: _tmp3B1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A6)->f1)->f1;_LLCB: {
# 2521
struct Cyc_Absyn_Aggrdecl*_tmp3A7=Cyc_Absyn_get_known_aggrdecl(_tmp3B1);
if(_tmp3A7->impl == 0)
({void*_tmp3A8=0U;({struct _dyneither_ptr _tmp91F=({const char*_tmp3A9="struct fields must be known";_tag_dyneither(_tmp3A9,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp91F,_tag_dyneither(_tmp3A8,sizeof(void*),0U));});});
_tmp3B3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A7->impl))->fields;goto _LLCD;}case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A6)->f1)->f1).KnownDatatypefield).tag == 2){_LLD0: _tmp3B2=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A6)->f1)->f1).KnownDatatypefield).val).f2;_LLD1:
# 2535
 if(_tmp3B6 == 0)
({void*_tmp920=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->tag=0U,_tmp3AD->f1=Cyc_Toc_tag_sp;_tmp3AD;}));_tmp3A2->hd=_tmp920;});else{
# 2538
({void*_tmp921=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3B2->typs,(int)(_tmp3B6 - 1))).f2;_tmp3A1=_tmp921;});
({void*_tmp923=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->tag=0U,({struct _dyneither_ptr*_tmp922=Cyc_Absyn_fieldname((int)_tmp3B6);_tmp3AE->f1=_tmp922;});_tmp3AE;}));_tmp3A2->hd=_tmp923;});}
# 2541
goto _LLC9;}else{goto _LLD2;}default: goto _LLD2;}case 7U: _LLCC: _tmp3B3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A6)->f2;_LLCD: {
# 2526
struct Cyc_Absyn_Aggrfield*_tmp3AA=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3B3,(int)_tmp3B6);
({void*_tmp924=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->tag=0U,_tmp3AB->f1=_tmp3AA->name;_tmp3AB;}));_tmp3A2->hd=_tmp924;});
_tmp3A1=_tmp3AA->type;
goto _LLC9;}case 6U: _LLCE: _tmp3B4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A6)->f1;_LLCF:
# 2531
({void*_tmp926=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->tag=0U,({struct _dyneither_ptr*_tmp925=Cyc_Absyn_fieldname((int)(_tmp3B6 + 1));_tmp3AC->f1=_tmp925;});_tmp3AC;}));_tmp3A2->hd=_tmp926;});
({void*_tmp927=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3B4,(int)_tmp3B6)).f2;_tmp3A1=_tmp927;});
goto _LLC9;default: _LLD2: _LLD3:
# 2542
({void*_tmp3AF=0U;({struct _dyneither_ptr _tmp928=({const char*_tmp3B0="impossible type for offsetof tuple index";_tag_dyneither(_tmp3B0,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp928,_tag_dyneither(_tmp3AF,sizeof(void*),0U));});});
goto _LLC9;}_LLC9:;}
# 2545
goto _LLC4;}_LLC4:;}
# 2548
({void*_tmp92A=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->tag=19U,({void*_tmp929=Cyc_Toc_typ_to_c(_tmp484);_tmp3B7->f1=_tmp929;}),_tmp3B7->f2=_tmp483;_tmp3B7;});e->r=_tmp92A;});
goto _LL0;}case 21U: _LL2F: _tmp488=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp487=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp486=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp485=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL30: {
# 2551
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp488->topt);
Cyc_Toc_exp_to_c(nv,_tmp488);
if(_tmp486  && _tmp485)
({void*_tmp92F=({struct Cyc_Absyn_Exp*_tmp92E=_tmp488;void*_tmp92D=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp92C=e1_cyc_type;struct _dyneither_ptr*_tmp92B=_tmp487;Cyc_Toc_check_tagged_union(_tmp92E,_tmp92D,_tmp92C,_tmp92B,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2555
e->r=_tmp92F;});
# 2557
if(is_poly)
({void*_tmp931=({void*_tmp930=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp930,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp931;});
goto _LL0;}case 22U: _LL31: _tmp48C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp48B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp48A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp489=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL32: {
# 2561
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=(void*)_check_null(_tmp48C->topt);
void*ta=Cyc_Toc_get_ptr_elt_type(e1typ);
Cyc_Toc_ptr_use_to_c(nv,_tmp48C,e->annot,0);
if(_tmp48A  && _tmp489)
({void*_tmp936=({struct Cyc_Absyn_Exp*_tmp935=_tmp48C;void*_tmp934=Cyc_Toc_typ_to_c(e1typ);void*_tmp933=ta;struct _dyneither_ptr*_tmp932=_tmp48B;Cyc_Toc_check_tagged_union(_tmp935,_tmp934,_tmp933,_tmp932,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp936;});
if(is_poly  && _tmp489)
({void*_tmp938=({void*_tmp937=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp937,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp938;});
goto _LL0;}case 20U: _LL33: _tmp48D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL34:
# 2571
 Cyc_Toc_ptr_use_to_c(nv,_tmp48D,e->annot,0);
goto _LL0;case 23U: _LL35: _tmp48F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp48E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL36:
# 2574
{void*_tmp3B8=Cyc_Tcutil_compress((void*)_check_null(_tmp48F->topt));void*_tmp3B9=_tmp3B8;struct Cyc_List_List*_tmp3C1;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B9)->tag == 6U){_LLD5: _tmp3C1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B9)->f1;_LLD6:
# 2576
 Cyc_Toc_exp_to_c(nv,_tmp48F);{
int _tmp3BA=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp48E);{
struct _tuple13 _tmp3BB=Cyc_Evexp_eval_const_uint_exp(_tmp48E);struct _tuple13 _tmp3BC=_tmp3BB;unsigned int _tmp3C0;int _tmp3BF;_LLDA: _tmp3C0=_tmp3BC.f1;_tmp3BF=_tmp3BC.f2;_LLDB:;
if(!_tmp3BF)
({void*_tmp3BD=0U;({struct _dyneither_ptr _tmp939=({const char*_tmp3BE="unknown tuple subscript in translation to C";_tag_dyneither(_tmp3BE,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp939,_tag_dyneither(_tmp3BD,sizeof(void*),0U));});});
({void*_tmp93B=({struct Cyc_Absyn_Exp*_tmp93A=_tmp48F;Cyc_Toc_aggrmember_exp_r(_tmp93A,Cyc_Absyn_fieldname((int)(_tmp3C0 + 1)));});e->r=_tmp93B;});
goto _LLD4;};};}else{_LLD7: _LLD8: {
# 2586
int index_used=Cyc_Toc_ptr_use_to_c(nv,_tmp48F,e->annot,_tmp48E);
if(index_used)
({void*_tmp93C=Cyc_Toc_deref_exp_r(_tmp48F);e->r=_tmp93C;});
goto _LLD4;}}_LLD4:;}
# 2591
goto _LL0;case 24U: _LL37: _tmp490=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL38:
# 2593
 if(!Cyc_Toc_is_toplevel(nv)){
# 2595
void*_tmp3C2=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp490));
{void*_tmp3C3=_tmp3C2;union Cyc_Absyn_AggrInfo _tmp3C8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->f2 == 0){_LLDD: _tmp3C8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C3)->f1)->f1;_LLDE: {
# 2598
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3C8);
{int i=1;for(0;_tmp490 != 0;(_tmp490=_tmp490->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp490->hd);
({struct Cyc_List_List*_tmp93F=({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct _tuple19*_tmp93E=({struct _dyneither_ptr*_tmp93D=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp93D,(struct Cyc_Absyn_Exp*)_tmp490->hd);});_tmp3C4->hd=_tmp93E;}),_tmp3C4->tl=dles;_tmp3C4;});dles=_tmp93F;});}}
# 2604
({void*_tmp941=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->tag=29U,_tmp3C5->f1=sd->name,_tmp3C5->f2=0,({struct Cyc_List_List*_tmp940=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp3C5->f3=_tmp940;}),_tmp3C5->f4=sd;_tmp3C5;});e->r=_tmp941;});
e->topt=_tmp3C2;
goto _LLDC;}}else{goto _LLDF;}}else{goto _LLDF;}}else{_LLDF: _LLE0:
({void*_tmp3C6=0U;({struct _dyneither_ptr _tmp942=({const char*_tmp3C7="tuple type not an aggregate";_tag_dyneither(_tmp3C7,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp942,_tag_dyneither(_tmp3C6,sizeof(void*),0U));});});}_LLDC:;}
# 2609
goto _LL0;}else{
# 2613
struct Cyc_List_List*dles=0;
for(0;_tmp490 != 0;_tmp490=_tmp490->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp490->hd);
({struct Cyc_List_List*_tmp944=({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct _tuple19*_tmp943=({struct _tuple19*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->f1=0,_tmp3C9->f2=(struct Cyc_Absyn_Exp*)_tmp490->hd;_tmp3C9;});_tmp3CA->hd=_tmp943;}),_tmp3CA->tl=dles;_tmp3CA;});dles=_tmp944;});}
# 2618
({void*_tmp945=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp945;});}
# 2620
goto _LL0;case 26U: _LL39: _tmp491=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL3A:
# 2626
{struct Cyc_List_List*_tmp3CB=_tmp491;for(0;_tmp3CB != 0;_tmp3CB=_tmp3CB->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3CB->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp946=Cyc_Toc_unresolvedmem_exp_r(0,_tmp491);e->r=_tmp946;});
goto _LL0;case 27U: _LL3B: _tmp495=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp494=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp493=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp492=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL3C:
# 2634
 Cyc_Toc_exp_to_c(nv,_tmp494);
Cyc_Toc_exp_to_c(nv,_tmp493);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp3CC=Cyc_Evexp_eval_const_uint_exp(_tmp494);struct _tuple13 _tmp3CD=_tmp3CC;unsigned int _tmp3D7;int _tmp3D6;_LLE2: _tmp3D7=_tmp3CD.f1;_tmp3D6=_tmp3CD.f2;_LLE3:;{
void*_tmp3CE=Cyc_Toc_typ_to_c((void*)_check_null(_tmp493->topt));
struct Cyc_List_List*es=0;
# 2641
if(!Cyc_Toc_is_zero(_tmp493)){
if(!_tmp3D6)
({void*_tmp3CF=0U;({unsigned int _tmp948=_tmp494->loc;struct _dyneither_ptr _tmp947=({const char*_tmp3D0="cannot determine value of constant";_tag_dyneither(_tmp3D0,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp948,_tmp947,_tag_dyneither(_tmp3CF,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp3D7;++ i){
({struct Cyc_List_List*_tmp94A=({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct _tuple19*_tmp949=({struct _tuple19*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=0,_tmp3D1->f2=_tmp493;_tmp3D1;});_tmp3D2->hd=_tmp949;}),_tmp3D2->tl=es;_tmp3D2;});es=_tmp94A;});}}
# 2647
if(_tmp492){
struct Cyc_Absyn_Exp*_tmp3D3=({void*_tmp94B=_tmp3CE;Cyc_Toc_cast_it(_tmp94B,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp94E=({struct Cyc_List_List*_tmp94D=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp94D,({struct Cyc_List_List*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct _tuple19*_tmp94C=({struct _tuple19*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->f1=0,_tmp3D4->f2=_tmp3D3;_tmp3D4;});_tmp3D5->hd=_tmp94C;}),_tmp3D5->tl=0;_tmp3D5;}));});es=_tmp94E;});}}
# 2652
({void*_tmp94F=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp94F;});};}
# 2654
goto _LL0;case 28U: _LL3D: _tmp498=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp497=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp496=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LL3E:
# 2661
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp950=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp950;});else{
# 2664
Cyc_Toc_exp_to_c(nv,_tmp498);}
goto _LL0;case 30U: _LL3F: _tmp49A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp499=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL40:
# 2668
{struct Cyc_List_List*_tmp3D8=_tmp499;for(0;_tmp3D8 != 0;_tmp3D8=_tmp3D8->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)_tmp3D8->hd)).f2);}}{
void*_tmp3D9=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp3D9;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3DA=Cyc_Tcutil_compress(_tmp3D9);void*_tmp3DB=_tmp3DA;union Cyc_Absyn_AggrInfo _tmp3DF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->f1)->tag == 20U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->f2 == 0){_LLE5: _tmp3DF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3DB)->f1)->f1;_LLE6: {
# 2675
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp3DF);
({void*_tmp951=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->tag=29U,_tmp3DC->f1=sd->name,_tmp3DC->f2=0,_tmp3DC->f3=_tmp499,_tmp3DC->f4=sd;_tmp3DC;});e->r=_tmp951;});
e->topt=_tmp3D9;
goto _LLE4;}}else{goto _LLE7;}}else{goto _LLE7;}}else{_LLE7: _LLE8:
({void*_tmp3DD=0U;({struct _dyneither_ptr _tmp952=({const char*_tmp3DE="anonStruct type not an aggregate";_tag_dyneither(_tmp3DE,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp952,_tag_dyneither(_tmp3DD,sizeof(void*),0U));});});}_LLE4:;}else{
# 2682
({void*_tmp953=Cyc_Toc_unresolvedmem_exp_r(0,_tmp499);e->r=_tmp953;});}
goto _LL0;};case 29U: _LL41: _tmp49E=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp49D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp49C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp49B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL42:
# 2688
 if(_tmp49B == 0  || _tmp49B->impl == 0)
({void*_tmp3E0=0U;({struct _dyneither_ptr _tmp954=({const char*_tmp3E1="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp3E1,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp954,_tag_dyneither(_tmp3E0,sizeof(void*),0U));});});{
void*_tmp3E2=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp3E3=Cyc_Tcutil_compress(_tmp3E2);void*_tmp3E4=_tmp3E3;union Cyc_Absyn_AggrInfo _tmp3E7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f1)->tag == 20U){_LLEA: _tmp3E7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E4)->f1)->f1;_LLEB:
({struct _tuple1*_tmp955=(Cyc_Absyn_aggr_kinded_name(_tmp3E7)).f2;*_tmp49E=_tmp955;});goto _LLE9;}else{goto _LLEC;}}else{_LLEC: _LLED:
({void*_tmp3E5=0U;({struct _dyneither_ptr _tmp956=({const char*_tmp3E6="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp3E6,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp956,_tag_dyneither(_tmp3E5,sizeof(void*),0U));});});}_LLE9:;}{
# 2699
struct Cyc_List_List*_tmp3E8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49B->impl))->fields;
if(_tmp3E8 == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp3E8))->tl != 0;_tmp3E8=_tmp3E8->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp3E9=(struct Cyc_Absyn_Aggrfield*)_tmp3E8->hd;
struct Cyc_List_List*_tmp3EA=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp49C,_tmp49B->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49B->impl))->fields);
# 2706
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp3EB=Cyc_Tcutil_compress(old_typ);void*_tmp3EC=_tmp3EB;struct Cyc_List_List*_tmp418;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EC)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EC)->f1)->tag == 20U){_LLEF: _tmp418=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3EC)->f2;_LLF0:
# 2710
{struct Cyc_List_List*_tmp3ED=_tmp3EA;for(0;_tmp3ED != 0;_tmp3ED=_tmp3ED->tl){
struct _tuple34*_tmp3EE=(struct _tuple34*)_tmp3ED->hd;struct _tuple34*_tmp3EF=_tmp3EE;struct Cyc_Absyn_Aggrfield*_tmp415;struct Cyc_Absyn_Exp*_tmp414;_LLF4: _tmp415=_tmp3EF->f1;_tmp414=_tmp3EF->f2;_LLF5:;{
void*_tmp3F0=_tmp414->topt;
Cyc_Toc_exp_to_c(nv,_tmp414);
# 2715
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp415->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp414->topt)))
({void*_tmp958=({void*_tmp957=Cyc_Toc_typ_to_c(_tmp415->type);Cyc_Toc_cast_it(_tmp957,
Cyc_Absyn_copy_exp(_tmp414));})->r;
# 2717
_tmp414->r=_tmp958;});
# 2720
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49B->impl))->tagged){
struct _dyneither_ptr*_tmp3F1=_tmp415->name;
struct Cyc_Absyn_Exp*_tmp3F2=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp49B,_tmp3F1),0U);
struct _tuple19*_tmp3F3=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp3F2);
struct _tuple19*_tmp3F4=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp414);
struct _tuple1*s=({struct _tuple1*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));({union Cyc_Absyn_Nmspace _tmp95C=Cyc_Absyn_Abs_n(0,1);_tmp3FF->f1=_tmp95C;}),({
struct _dyneither_ptr*_tmp95B=({struct _dyneither_ptr*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));({struct _dyneither_ptr _tmp95A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3FC=({struct Cyc_String_pa_PrintArg_struct _tmp72C;_tmp72C.tag=0U,_tmp72C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp49B->name).f2);_tmp72C;});struct Cyc_String_pa_PrintArg_struct _tmp3FD=({struct Cyc_String_pa_PrintArg_struct _tmp72B;_tmp72B.tag=0U,_tmp72B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3F1);_tmp72B;});void*_tmp3FA[2U];_tmp3FA[0]=& _tmp3FC,_tmp3FA[1]=& _tmp3FD;({struct _dyneither_ptr _tmp959=({const char*_tmp3FB="_union_%s_%s";_tag_dyneither(_tmp3FB,sizeof(char),13U);});Cyc_aprintf(_tmp959,_tag_dyneither(_tmp3FA,sizeof(void*),2U));});});*_tmp3FE=_tmp95A;});_tmp3FE;});_tmp3FF->f2=_tmp95B;});_tmp3FF;});
# 2728
struct _tuple19*_tmp3F5=({
struct _dyneither_ptr*_tmp95E=_tmp3F1;Cyc_Toc_make_field(_tmp95E,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9->tag=29U,_tmp3F9->f1=s,_tmp3F9->f2=0,({struct Cyc_List_List*_tmp95D=({struct _tuple19*_tmp3F8[2U];_tmp3F8[0]=_tmp3F3,_tmp3F8[1]=_tmp3F4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F8,sizeof(struct _tuple19*),2U));});_tmp3F9->f3=_tmp95D;}),_tmp3F9->f4=0;_tmp3F9;}),0U));});
# 2731
({void*_tmp960=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->tag=29U,_tmp3F7->f1=*_tmp49E,_tmp3F7->f2=0,({struct Cyc_List_List*_tmp95F=({struct _tuple19*_tmp3F6[1U];_tmp3F6[0]=_tmp3F5;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F6,sizeof(struct _tuple19*),1U));});_tmp3F7->f3=_tmp95F;}),_tmp3F7->f4=_tmp49B;_tmp3F7;});e->r=_tmp960;});}
# 2737
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp3E9 == _tmp415){
struct Cyc_List_List*_tmp400=({struct Cyc_List_List*_tmp961=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49B->impl))->exist_vars,_tmp49D);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp961,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp49B->tvs,_tmp418));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp401=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49B->impl))->fields;for(0;_tmp401 != 0;_tmp401=_tmp401->tl){
struct Cyc_Absyn_Aggrfield*_tmp402=(struct Cyc_Absyn_Aggrfield*)_tmp401->hd;
void*_tmp403=Cyc_Tcutil_substitute(_tmp400,_tmp402->type);
struct Cyc_Absyn_Aggrfield*_tmp404=({struct Cyc_Absyn_Aggrfield*_tmp962=_tmp402;Cyc_Toc_aggrfield_to_c(_tmp962,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp400,_tmp403)));});
({struct Cyc_List_List*_tmp963=({struct Cyc_List_List*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->hd=_tmp404,_tmp405->tl=new_fields;_tmp405;});new_fields=_tmp963;});
# 2752
if(_tmp401->tl == 0){
{void*_tmp406=Cyc_Tcutil_compress(_tmp404->type);void*_tmp407=_tmp406;struct Cyc_Absyn_ArrayInfo _tmp40C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp407)->tag == 4U){_LLF7: _tmp40C=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp407)->f1;_LLF8:
# 2755
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp40C.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp408=_tmp40C;
({struct Cyc_Absyn_Exp*_tmp964=Cyc_Absyn_uint_exp(0U,0U);_tmp408.num_elts=_tmp964;});
({void*_tmp965=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->tag=4U,_tmp409->f1=_tmp408;_tmp409;});_tmp404->type=_tmp965;});}
# 2760
goto _LLF6;}else{_LLF9: _LLFA:
# 2764
 if(_tmp414->topt == 0)
goto _LLF6;
{void*_tmp40A=Cyc_Tcutil_compress((void*)_check_null(_tmp414->topt));void*_tmp40B=_tmp40A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40B)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40B)->f1)->tag == 20U){_LLFC: _LLFD:
# 2768
 _tmp404->type=(void*)_check_null(_tmp414->topt);goto _LLFB;}else{goto _LLFE;}}else{_LLFE: _LLFF:
 goto _LLFB;}_LLFB:;}
# 2771
goto _LLF6;}_LLF6:;}
# 2775
if(!Cyc_Tcutil_is_array_type(_tmp402->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp402->type)))
({struct Cyc_List_List*_tmp967=({struct Cyc_List_List*_tmp40E=_cycalloc(sizeof(*_tmp40E));({void*_tmp966=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->tag=6U,_tmp40D->f1=0;_tmp40D;});_tmp40E->hd=_tmp966;}),_tmp40E->tl=_tmp404->attributes;_tmp40E;});_tmp404->attributes=_tmp967;});}}}
# 2781
({struct Cyc_Absyn_Aggrdecl*_tmp96B=({struct _dyneither_ptr*_tmp96A=({struct _dyneither_ptr*_tmp412=_cycalloc(sizeof(*_tmp412));({struct _dyneither_ptr _tmp969=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp411=({struct Cyc_Int_pa_PrintArg_struct _tmp72D;_tmp72D.tag=1U,_tmp72D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp72D;});void*_tmp40F[1U];_tmp40F[0]=& _tmp411;({struct _dyneither_ptr _tmp968=({const char*_tmp410="_genStruct%d";_tag_dyneither(_tmp410,sizeof(char),13U);});Cyc_aprintf(_tmp968,_tag_dyneither(_tmp40F,sizeof(void*),1U));});});*_tmp412=_tmp969;});_tmp412;});Cyc_Toc_make_c_struct_defn(_tmp96A,
# 2783
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 2781
_tmp49B=_tmp96B;});
# 2784
*_tmp49E=_tmp49B->name;
Cyc_Toc_aggrdecl_to_c(_tmp49B);
# 2787
({void*_tmp96C=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp413=_cycalloc(sizeof(*_tmp413));*_tmp413=_tmp49B;_tmp413;})),0);e->topt=_tmp96C;});}};}}
# 2790
goto _LLEE;}else{goto _LLF1;}}else{_LLF1: _LLF2:
({void*_tmp416=0U;({struct _dyneither_ptr _tmp96D=({const char*_tmp417="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp417,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp96D,_tag_dyneither(_tmp416,sizeof(void*),0U));});});}_LLEE:;}else{
# 2799
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49B->impl))->tagged){
# 2801
struct _tuple34*_tmp419=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp3EA))->hd;struct _tuple34*_tmp41A=_tmp419;struct Cyc_Absyn_Aggrfield*_tmp423;struct Cyc_Absyn_Exp*_tmp422;_LL101: _tmp423=_tmp41A->f1;_tmp422=_tmp41A->f2;_LL102:;{
void*_tmp41B=(void*)_check_null(_tmp422->topt);
void*_tmp41C=_tmp423->type;
Cyc_Toc_exp_to_c(nv,_tmp422);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp41C) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp41B))
({void*_tmp96F=({
void*_tmp96E=Cyc_Absyn_void_star_type();Cyc_Toc_cast_it_r(_tmp96E,Cyc_Absyn_new_exp(_tmp422->r,0U));});
# 2807
_tmp422->r=_tmp96F;});{
# 2810
int i=Cyc_Toc_get_member_offset(_tmp49B,_tmp423->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp41D=({struct _tuple19*_tmp41F[2U];({struct _tuple19*_tmp971=({struct _tuple19*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->f1=0,_tmp420->f2=field_tag_exp;_tmp420;});_tmp41F[0]=_tmp971;}),({struct _tuple19*_tmp970=({struct _tuple19*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->f1=0,_tmp421->f2=_tmp422;_tmp421;});_tmp41F[1]=_tmp970;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41F,sizeof(struct _tuple19*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp41D,0U);
({void*_tmp973=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_tmp41E[1U];({struct _tuple19*_tmp972=Cyc_Toc_make_field(_tmp423->name,umem);_tmp41E[0]=_tmp972;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41E,sizeof(struct _tuple19*),1U));}));e->r=_tmp973;});};};}else{
# 2817
struct Cyc_List_List*_tmp424=0;
struct Cyc_List_List*_tmp425=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49B->impl))->fields;
for(0;_tmp425 != 0;_tmp425=_tmp425->tl){
struct Cyc_List_List*_tmp426=_tmp3EA;for(0;_tmp426 != 0;_tmp426=_tmp426->tl){
if((*((struct _tuple34*)_tmp426->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp425->hd){
struct _tuple34*_tmp427=(struct _tuple34*)_tmp426->hd;struct _tuple34*_tmp428=_tmp427;struct Cyc_Absyn_Aggrfield*_tmp42E;struct Cyc_Absyn_Exp*_tmp42D;_LL104: _tmp42E=_tmp428->f1;_tmp42D=_tmp428->f2;_LL105:;{
void*_tmp429=Cyc_Toc_typ_to_c(_tmp42E->type);
void*_tmp42A=Cyc_Toc_typ_to_c((void*)_check_null(_tmp42D->topt));
Cyc_Toc_exp_to_c(nv,_tmp42D);
# 2827
if(!Cyc_Tcutil_unify(_tmp429,_tmp42A)){
# 2829
if(!Cyc_Tcutil_is_arithmetic_type(_tmp429) || !
Cyc_Tcutil_is_arithmetic_type(_tmp42A))
({struct Cyc_Absyn_Exp*_tmp975=({void*_tmp974=_tmp429;Cyc_Toc_cast_it(_tmp974,Cyc_Absyn_copy_exp(_tmp42D));});_tmp42D=_tmp975;});}
({struct Cyc_List_List*_tmp977=({struct Cyc_List_List*_tmp42C=_cycalloc(sizeof(*_tmp42C));({struct _tuple19*_tmp976=({struct _tuple19*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->f1=0,_tmp42B->f2=_tmp42D;_tmp42B;});_tmp42C->hd=_tmp976;}),_tmp42C->tl=_tmp424;_tmp42C;});_tmp424=_tmp977;});
break;};}}}
# 2836
({void*_tmp978=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp424));e->r=_tmp978;});}}
# 2839
goto _LL0;};};};case 31U: _LL43: _tmp4A1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4A0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp49F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LL44: {
# 2843
struct Cyc_List_List*_tmp42F=_tmp49F->typs;
{struct Cyc_List_List*_tmp430=_tmp4A1;for(0;_tmp430 != 0;(_tmp430=_tmp430->tl,_tmp42F=_tmp42F->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp430->hd;
void*_tmp431=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp42F))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp431))
({void*_tmp979=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmp979;});}}{
# 2854
struct Cyc_Absyn_Exp*_tmp432=_tmp4A0->is_extensible?Cyc_Absyn_var_exp(_tmp49F->name,0U):
 Cyc_Toc_datatype_tag(_tmp4A0,_tmp49F->name);
# 2857
if(!Cyc_Toc_is_toplevel(nv)){
# 2859
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4A1 != 0;(_tmp4A1=_tmp4A1->tl,++ i)){
({struct Cyc_List_List*_tmp97C=({struct Cyc_List_List*_tmp433=_cycalloc(sizeof(*_tmp433));({struct _tuple19*_tmp97B=({struct _dyneither_ptr*_tmp97A=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp97A,(struct Cyc_Absyn_Exp*)_tmp4A1->hd);});_tmp433->hd=_tmp97B;}),_tmp433->tl=dles;_tmp433;});dles=_tmp97C;});}}{
# 2865
struct _tuple19*_tmp434=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp432);
({void*_tmp980=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->tag=29U,({struct _tuple1*_tmp97F=Cyc_Toc_collapse_qvars(_tmp49F->name,_tmp4A0->name);_tmp436->f1=_tmp97F;}),_tmp436->f2=0,({
struct Cyc_List_List*_tmp97E=({struct Cyc_List_List*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->hd=_tmp434,({struct Cyc_List_List*_tmp97D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp435->tl=_tmp97D;});_tmp435;});_tmp436->f3=_tmp97E;}),_tmp436->f4=0;_tmp436;});
# 2866
e->r=_tmp980;});};}else{
# 2871
struct Cyc_List_List*_tmp437=0;
for(0;_tmp4A1 != 0;_tmp4A1=_tmp4A1->tl){
({struct Cyc_List_List*_tmp982=({struct Cyc_List_List*_tmp439=_cycalloc(sizeof(*_tmp439));({struct _tuple19*_tmp981=({struct _tuple19*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->f1=0,_tmp438->f2=(struct Cyc_Absyn_Exp*)_tmp4A1->hd;_tmp438;});_tmp439->hd=_tmp981;}),_tmp439->tl=_tmp437;_tmp439;});_tmp437=_tmp982;});}
({void*_tmp985=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp43B=_cycalloc(sizeof(*_tmp43B));({struct _tuple19*_tmp984=({struct _tuple19*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->f1=0,_tmp43A->f2=_tmp432;_tmp43A;});_tmp43B->hd=_tmp984;}),({
struct Cyc_List_List*_tmp983=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp437);_tmp43B->tl=_tmp983;});_tmp43B;}));
# 2874
e->r=_tmp985;});}
# 2877
goto _LL0;};}case 32U: _LL45: _LL46:
# 2879
 goto _LL48;case 33U: _LL47: _LL48:
 goto _LL0;case 34U: _LL49: _tmp4A7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).is_calloc;_tmp4A6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).rgn;_tmp4A5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).elt_type;_tmp4A4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).num_elts;_tmp4A3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).fat_result;_tmp4A2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).inline_call;_LL4A: {
# 2883
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4A5)));
Cyc_Toc_exp_to_c(nv,_tmp4A4);
if(_tmp4A3){
struct _tuple1*_tmp43C=Cyc_Toc_temp_var();
struct _tuple1*_tmp43D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp4A7){
xexp=_tmp4A4;
if(_tmp4A6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4A6;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmp988=({struct Cyc_Absyn_Exp*_tmp987=rgn;struct Cyc_Absyn_Exp*_tmp986=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmp987,_tmp986,Cyc_Absyn_var_exp(_tmp43C,0U));});pexp=_tmp988;});}else{
# 2896
({struct Cyc_Absyn_Exp*_tmp98B=({void*_tmp98A=*_tmp4A5;struct Cyc_Absyn_Exp*_tmp989=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp98A,_tmp989,Cyc_Absyn_var_exp(_tmp43C,0U));});pexp=_tmp98B;});}
# 2898
({struct Cyc_Absyn_Exp*_tmp990=({struct Cyc_Absyn_Exp*_tmp98F=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp98F,({struct Cyc_Absyn_Exp*_tmp43E[3U];({
struct Cyc_Absyn_Exp*_tmp98E=Cyc_Absyn_var_exp(_tmp43D,0U);_tmp43E[0]=_tmp98E;}),({struct Cyc_Absyn_Exp*_tmp98D=Cyc_Absyn_sizeoftype_exp(t_c,0U);_tmp43E[1]=_tmp98D;}),({
struct Cyc_Absyn_Exp*_tmp98C=Cyc_Absyn_var_exp(_tmp43C,0U);_tmp43E[2]=_tmp98C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2898
rexp=_tmp990;});}else{
# 2902
if(_tmp4A6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4A6;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp4A2)
({struct Cyc_Absyn_Exp*_tmp992=({struct Cyc_Absyn_Exp*_tmp991=rgn;Cyc_Toc_rmalloc_inline_exp(_tmp991,Cyc_Absyn_var_exp(_tmp43C,0U));});pexp=_tmp992;});else{
# 2908
({struct Cyc_Absyn_Exp*_tmp994=({struct Cyc_Absyn_Exp*_tmp993=rgn;Cyc_Toc_rmalloc_exp(_tmp993,Cyc_Absyn_var_exp(_tmp43C,0U));});pexp=_tmp994;});}}else{
# 2910
({struct Cyc_Absyn_Exp*_tmp996=({void*_tmp995=*_tmp4A5;Cyc_Toc_malloc_exp(_tmp995,Cyc_Absyn_var_exp(_tmp43C,0U));});pexp=_tmp996;});}
# 2912
({struct Cyc_Absyn_Exp*_tmp99B=({struct Cyc_Absyn_Exp*_tmp99A=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp99A,({struct Cyc_Absyn_Exp*_tmp43F[3U];({struct Cyc_Absyn_Exp*_tmp999=Cyc_Absyn_var_exp(_tmp43D,0U);_tmp43F[0]=_tmp999;}),({struct Cyc_Absyn_Exp*_tmp998=Cyc_Absyn_uint_exp(1U,0U);_tmp43F[1]=_tmp998;}),({
struct Cyc_Absyn_Exp*_tmp997=Cyc_Absyn_var_exp(_tmp43C,0U);_tmp43F[2]=_tmp997;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43F,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2912
rexp=_tmp99B;});}{
# 2915
struct Cyc_Absyn_Stmt*_tmp440=({struct _tuple1*_tmp9A1=_tmp43C;void*_tmp9A0=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_tmp99F=_tmp4A4;Cyc_Absyn_declare_stmt(_tmp9A1,_tmp9A0,_tmp99F,({
struct _tuple1*_tmp99E=_tmp43D;void*_tmp99D=Cyc_Absyn_cstar_type(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp99C=pexp;Cyc_Absyn_declare_stmt(_tmp99E,_tmp99D,_tmp99C,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmp9A2=Cyc_Toc_stmt_exp_r(_tmp440);e->r=_tmp9A2;});};}else{
# 2920
if(_tmp4A7){
if(_tmp4A6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4A6;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmp9A5=({struct Cyc_Absyn_Exp*_tmp9A4=rgn;struct Cyc_Absyn_Exp*_tmp9A3=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmp9A4,_tmp9A3,_tmp4A4);})->r;e->r=_tmp9A5;});}else{
# 2926
({void*_tmp9A8=({void*_tmp9A7=*_tmp4A5;struct Cyc_Absyn_Exp*_tmp9A6=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmp9A7,_tmp9A6,_tmp4A4);})->r;e->r=_tmp9A8;});}}else{
# 2929
if(_tmp4A6 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp4A6;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp4A2)
({void*_tmp9A9=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp4A4))->r;e->r=_tmp9A9;});else{
# 2935
({void*_tmp9AA=(Cyc_Toc_rmalloc_exp(rgn,_tmp4A4))->r;e->r=_tmp9AA;});}}else{
# 2937
({void*_tmp9AB=(Cyc_Toc_malloc_exp(*_tmp4A5,_tmp4A4))->r;e->r=_tmp9AB;});}}}
# 2941
goto _LL0;}case 35U: _LL4B: _tmp4A9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4A8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL4C: {
# 2950
void*e1_old_typ=(void*)_check_null(_tmp4A9->topt);
void*e2_old_typ=(void*)_check_null(_tmp4A8->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp441=0U;({struct _dyneither_ptr _tmp9AC=({const char*_tmp442="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp442,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9AC,_tag_dyneither(_tmp441,sizeof(void*),0U));});});{
# 2958
unsigned int _tmp443=e->loc;
struct _tuple1*_tmp444=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp445=Cyc_Absyn_var_exp(_tmp444,_tmp443);_tmp445->topt=e1_old_typ;{
struct _tuple1*_tmp446=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp447=Cyc_Absyn_var_exp(_tmp446,_tmp443);_tmp447->topt=e2_old_typ;{
# 2964
struct Cyc_Absyn_Exp*_tmp448=({struct Cyc_Absyn_Exp*_tmp9AE=Cyc_Tcutil_deep_copy_exp(1,_tmp4A9);struct Cyc_Absyn_Exp*_tmp9AD=_tmp447;Cyc_Absyn_assign_exp(_tmp9AE,_tmp9AD,_tmp443);});_tmp448->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp449=Cyc_Absyn_exp_stmt(_tmp448,_tmp443);
struct Cyc_Absyn_Exp*_tmp44A=({struct Cyc_Absyn_Exp*_tmp9B0=Cyc_Tcutil_deep_copy_exp(1,_tmp4A8);struct Cyc_Absyn_Exp*_tmp9AF=_tmp445;Cyc_Absyn_assign_exp(_tmp9B0,_tmp9AF,_tmp443);});_tmp44A->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp44B=Cyc_Absyn_exp_stmt(_tmp44A,_tmp443);
# 2969
struct Cyc_Absyn_Stmt*_tmp44C=({struct _tuple1*_tmp9B8=_tmp444;void*_tmp9B7=e1_old_typ;struct Cyc_Absyn_Exp*_tmp9B6=_tmp4A9;struct Cyc_Absyn_Stmt*_tmp9B5=({
struct _tuple1*_tmp9B4=_tmp446;void*_tmp9B3=e2_old_typ;struct Cyc_Absyn_Exp*_tmp9B2=_tmp4A8;struct Cyc_Absyn_Stmt*_tmp9B1=
Cyc_Absyn_seq_stmt(_tmp449,_tmp44B,_tmp443);
# 2970
Cyc_Absyn_declare_stmt(_tmp9B4,_tmp9B3,_tmp9B2,_tmp9B1,_tmp443);});
# 2969
Cyc_Absyn_declare_stmt(_tmp9B8,_tmp9B7,_tmp9B6,_tmp9B5,_tmp443);});
# 2972
Cyc_Toc_stmt_to_c(nv,_tmp44C);
({void*_tmp9B9=Cyc_Toc_stmt_exp_r(_tmp44C);e->r=_tmp9B9;});
goto _LL0;};};};};};}case 38U: _LL4D: _tmp4AB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4AA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL4E: {
# 2977
void*_tmp44D=Cyc_Tcutil_compress((void*)_check_null(_tmp4AB->topt));
Cyc_Toc_exp_to_c(nv,_tmp4AB);
{void*_tmp44E=_tmp44D;struct Cyc_Absyn_Aggrdecl*_tmp454;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->tag == 0U){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->f1)->f1).KnownAggr).tag == 2){_LL107: _tmp454=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44E)->f1)->f1).KnownAggr).val;_LL108: {
# 2981
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp454,_tmp4AA),0U);
struct Cyc_Absyn_Exp*_tmp450=Cyc_Toc_member_exp(_tmp4AB,_tmp4AA,0U);
struct Cyc_Absyn_Exp*_tmp451=Cyc_Toc_member_exp(_tmp450,Cyc_Toc_tag_sp,0U);
({void*_tmp9BA=(Cyc_Absyn_eq_exp(_tmp451,_tmp44F,0U))->r;e->r=_tmp9BA;});
goto _LL106;}}else{goto _LL109;}}else{goto _LL109;}}else{_LL109: _LL10A:
({void*_tmp452=0U;({struct _dyneither_ptr _tmp9BB=({const char*_tmp453="non-aggregate type in tagcheck";_tag_dyneither(_tmp453,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9BB,_tag_dyneither(_tmp452,sizeof(void*),0U));});});}_LL106:;}
# 2988
goto _LL0;}case 37U: _LL4F: _tmp4AC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL50:
 Cyc_Toc_stmt_to_c(nv,_tmp4AC);goto _LL0;case 36U: _LL51: _LL52:
# 2991
({void*_tmp455=0U;({struct _dyneither_ptr _tmp9BC=({const char*_tmp456="UnresolvedMem";_tag_dyneither(_tmp456,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9BC,_tag_dyneither(_tmp455,sizeof(void*),0U));});});case 25U: _LL53: _LL54:
({void*_tmp457=0U;({struct _dyneither_ptr _tmp9BD=({const char*_tmp458="compoundlit";_tag_dyneither(_tmp458,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp9BD,_tag_dyneither(_tmp457,sizeof(void*),0U));});});case 39U: _LL55: _LL56:
({void*_tmp459=0U;({struct _dyneither_ptr _tmp9BE=({const char*_tmp45A="valueof(-)";_tag_dyneither(_tmp45A,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9BE,_tag_dyneither(_tmp459,sizeof(void*),0U));});});default: _LL57: _LL58:
({void*_tmp45B=0U;({struct _dyneither_ptr _tmp9BF=({const char*_tmp45C="__asm__";_tag_dyneither(_tmp45C,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9BF,_tag_dyneither(_tmp45B,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3022 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3024
return({struct _tuple35*_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD));_tmp4AD->f1=0,({struct _dyneither_ptr*_tmp9C1=Cyc_Toc_fresh_label();_tmp4AD->f2=_tmp9C1;}),({struct _dyneither_ptr*_tmp9C0=Cyc_Toc_fresh_label();_tmp4AD->f3=_tmp9C0;}),_tmp4AD->f4=sc;_tmp4AD;});}
# 3029
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp4AE=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3035
if((int)(((_tmp4AE->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp4B1=_tmp4AE->orig_pat;if((_tmp4B1.pattern).tag != 1)_throw_match();(_tmp4B1.pattern).val;})->topt);
void*_tmp4AF=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp4B0=_tmp4AF;switch(*((int*)_tmp4B0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B0)->f1)){case 0U: _LL1: _LL2:
# 3040
 goto _LL4;case 20U: _LL3: _LL4:
 goto _LL6;default: goto _LL7;}case 7U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmp9C3=({void*_tmp9C2=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmp9C2,v);});v=_tmp9C3;});goto _LL0;}_LL0:;}{
# 3046
void*_tmp4B2=_tmp4AE->access;void*_tmp4B3=_tmp4B2;struct Cyc_Absyn_Datatypedecl*_tmp4BE;struct Cyc_Absyn_Datatypefield*_tmp4BD;unsigned int _tmp4BC;int _tmp4BB;struct _dyneither_ptr*_tmp4BA;unsigned int _tmp4B9;switch(*((int*)_tmp4B3)){case 0U: _LLA: _LLB:
# 3051
 goto _LL9;case 1U: _LLC: _LLD:
# 3056
 if(ps->tl != 0){
void*_tmp4B4=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp4B5=_tmp4B4;struct Cyc_Absyn_Datatypedecl*_tmp4B8;struct Cyc_Absyn_Datatypefield*_tmp4B7;unsigned int _tmp4B6;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4B5)->tag == 3U){_LL15: _tmp4B8=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4B5)->f1;_tmp4B7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4B5)->f2;_tmp4B6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4B5)->f3;_LL16:
# 3059
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmp9C6=({void*_tmp9C5=({void*_tmp9C4=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4B7->name,_tmp4B8->name));Cyc_Absyn_cstar_type(_tmp9C4,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9C5,v);});v=_tmp9C6;});
({struct Cyc_Absyn_Exp*_tmp9C8=({struct Cyc_Absyn_Exp*_tmp9C7=v;Cyc_Absyn_aggrarrow_exp(_tmp9C7,Cyc_Absyn_fieldname((int)(_tmp4B6 + 1)),0U);});v=_tmp9C8;});
continue;}else{_LL17: _LL18:
# 3064
 goto _LL14;}_LL14:;}
# 3067
({struct Cyc_Absyn_Exp*_tmp9C9=Cyc_Absyn_deref_exp(v,0U);v=_tmp9C9;});
goto _LL9;case 2U: _LLE: _tmp4B9=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp4B3)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmp9CB=({struct Cyc_Absyn_Exp*_tmp9CA=v;Cyc_Toc_member_exp(_tmp9CA,Cyc_Absyn_fieldname((int)(_tmp4B9 + 1)),0U);});v=_tmp9CB;});goto _LL9;case 4U: _LL10: _tmp4BB=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4B3)->f1;_tmp4BA=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp4B3)->f2;_LL11:
# 3071
({struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Toc_member_exp(v,_tmp4BA,0U);v=_tmp9CC;});
if(_tmp4BB)
({struct Cyc_Absyn_Exp*_tmp9CD=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmp9CD;});
goto _LL9;default: _LL12: _tmp4BE=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4B3)->f1;_tmp4BD=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4B3)->f2;_tmp4BC=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp4B3)->f3;_LL13:
# 3076
({struct Cyc_Absyn_Exp*_tmp9CF=({struct Cyc_Absyn_Exp*_tmp9CE=v;Cyc_Toc_member_exp(_tmp9CE,Cyc_Absyn_fieldname((int)(_tmp4BC + 1)),0U);});v=_tmp9CF;});
goto _LL9;}_LL9:;};}
# 3080
return v;}
# 3085
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp4BF=t;struct Cyc_Absyn_Datatypedecl*_tmp4D8;struct Cyc_Absyn_Datatypefield*_tmp4D7;struct _dyneither_ptr*_tmp4D6;int _tmp4D5;int _tmp4D4;struct Cyc_Absyn_Datatypedecl*_tmp4D3;struct Cyc_Absyn_Datatypefield*_tmp4D2;unsigned int _tmp4D1;struct _dyneither_ptr _tmp4D0;int _tmp4CF;void*_tmp4CE;struct Cyc_Absyn_Enumfield*_tmp4CD;struct Cyc_Absyn_Enumdecl*_tmp4CC;struct Cyc_Absyn_Enumfield*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CA;switch(*((int*)_tmp4BF)){case 0U: _LL1: _tmp4CA=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4BF)->f1;_LL2:
# 3089
 if(_tmp4CA == 0)return v;else{return _tmp4CA;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmp9D0=v;Cyc_Absyn_eq_exp(_tmp9D0,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmp9D1=v;Cyc_Absyn_neq_exp(_tmp9D1,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp4CC=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4BF)->f1;_tmp4CB=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4BF)->f2;_LL8:
# 3093
 return({struct Cyc_Absyn_Exp*_tmp9D2=v;Cyc_Absyn_eq_exp(_tmp9D2,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->tag=32U,_tmp4C0->f1=_tmp4CC,_tmp4C0->f2=_tmp4CB;_tmp4C0;}),0U),0U);});case 4U: _LL9: _tmp4CE=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4BF)->f1;_tmp4CD=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4BF)->f2;_LLA:
# 3095
 return({struct Cyc_Absyn_Exp*_tmp9D3=v;Cyc_Absyn_eq_exp(_tmp9D3,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->tag=33U,_tmp4C1->f1=_tmp4CE,_tmp4C1->f2=_tmp4CD;_tmp4C1;}),0U),0U);});case 5U: _LLB: _tmp4D0=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4BF)->f1;_tmp4CF=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp4BF)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmp9D4=v;Cyc_Absyn_eq_exp(_tmp9D4,Cyc_Absyn_float_exp(_tmp4D0,_tmp4CF,0U),0U);});case 6U: _LLD: _tmp4D1=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4BF)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmp9D5=v;Cyc_Absyn_eq_exp(_tmp9D5,Cyc_Absyn_signed_int_exp((int)_tmp4D1,0U),0U);});case 7U: _LLF: _tmp4D4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4BF)->f1;_tmp4D3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4BF)->f2;_tmp4D2=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4BF)->f3;_LL10:
# 3101
 LOOP1: {
void*_tmp4C2=v->r;void*_tmp4C3=_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C4;switch(*((int*)_tmp4C3)){case 14U: _LL16: _tmp4C4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C3)->f2;_LL17:
 v=_tmp4C4;goto LOOP1;case 20U: _LL18: _tmp4C5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4C3)->f1;_LL19:
 v=_tmp4C5;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3108
({struct Cyc_Absyn_Exp*_tmp9D8=({void*_tmp9D7=({void*_tmp9D6=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4D2->name,_tmp4D3->name));Cyc_Absyn_cstar_type(_tmp9D6,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9D7,v);});v=_tmp9D8;});
return({struct Cyc_Absyn_Exp*_tmp9D9=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp9D9,Cyc_Absyn_uint_exp((unsigned int)_tmp4D4,0U),0U);});case 8U: _LL11: _tmp4D6=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4BF)->f1;_tmp4D5=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4BF)->f2;_LL12:
# 3113
({struct Cyc_Absyn_Exp*_tmp9DB=({struct Cyc_Absyn_Exp*_tmp9DA=Cyc_Toc_member_exp(v,_tmp4D6,0U);Cyc_Toc_member_exp(_tmp9DA,Cyc_Toc_tag_sp,0U);});v=_tmp9DB;});
return({struct Cyc_Absyn_Exp*_tmp9DC=v;Cyc_Absyn_eq_exp(_tmp9DC,Cyc_Absyn_signed_int_exp(_tmp4D5,0U),0U);});default: _LL13: _tmp4D8=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4BF)->f1;_tmp4D7=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp4BF)->f2;_LL14:
# 3117
 LOOP2: {
void*_tmp4C6=v->r;void*_tmp4C7=_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4C8;switch(*((int*)_tmp4C7)){case 14U: _LL1D: _tmp4C8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C7)->f2;_LL1E:
 v=_tmp4C8;goto LOOP2;case 20U: _LL1F: _tmp4C9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4C7)->f1;_LL20:
 v=_tmp4C9;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3124
({struct Cyc_Absyn_Exp*_tmp9DF=({void*_tmp9DE=({void*_tmp9DD=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4D7->name,_tmp4D8->name));Cyc_Absyn_cstar_type(_tmp9DD,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9DE,v);});v=_tmp9DF;});
return({struct Cyc_Absyn_Exp*_tmp9E0=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmp9E0,Cyc_Absyn_var_exp(_tmp4D7->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3138
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3146
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3148
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple32 _tmp4D9=*((struct _tuple32*)ss->hd);struct _tuple32 _tmp4DA=_tmp4D9;void*_tmp4E0;_LL1: _tmp4E0=_tmp4DA.f1;_LL2:;{
void*_tmp4DB=_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;struct _dyneither_ptr*_tmp4DE;switch(*((int*)_tmp4DB)){case 3U: _LL4: _LL5:
# 3154
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3157
 continue;case 8U: _LLA: _tmp4DE=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4DB)->f1;_LLB:
# 3159
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmp9E1=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->tag=3U,_tmp4DC->f1=_tmp4DE;_tmp4DC;});k=_tmp9E1;});
continue;case 7U: _LLC: _LLD:
# 3163
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp4DF=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp4DB)->f1;if(_tmp4DF != 0){_LLF:
# 3167
({void*_tmp9E2=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->tag=2U,_tmp4DD->f1=_tmp4DF;_tmp4DD;});k=_tmp9E2;});
return({struct _tuple36 _tmp72E;_tmp72E.f1=0,_tmp72E.f2=k;_tmp72E;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp72F;_tmp72F.f1=0,_tmp72F.f2=k;_tmp72F;});}_LL3:;};}
# 3176
return({struct _tuple36 _tmp730;_tmp730.f1=c,_tmp730.f2=k;_tmp730;});}
# 3181
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp4E1=p;int _tmp4EC;int _tmp4EB;unsigned int _tmp4EA;void*_tmp4E9;struct Cyc_Absyn_Enumfield*_tmp4E8;struct Cyc_Absyn_Enumdecl*_tmp4E7;struct Cyc_Absyn_Enumfield*_tmp4E6;switch(*((int*)_tmp4E1)){case 3U: _LL1: _tmp4E7=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4E1)->f1;_tmp4E6=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp4E1)->f2;_LL2:
# 3185
({struct Cyc_Absyn_Exp*_tmp9E3=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->tag=32U,_tmp4E2->f1=_tmp4E7,_tmp4E2->f2=_tmp4E6;_tmp4E2;}),0U);e=_tmp9E3;});goto _LL0;case 4U: _LL3: _tmp4E9=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4E1)->f1;_tmp4E8=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp4E1)->f2;_LL4:
# 3187
({struct Cyc_Absyn_Exp*_tmp9E4=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->tag=33U,_tmp4E3->f1=_tmp4E9,_tmp4E3->f2=_tmp4E8;_tmp4E3;}),0U);e=_tmp9E4;});goto _LL0;case 6U: _LL5: _tmp4EA=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp4E1)->f1;_LL6:
 _tmp4EB=(int)_tmp4EA;goto _LL8;case 7U: _LL7: _tmp4EB=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp4E1)->f1;_LL8:
 _tmp4EC=_tmp4EB;goto _LLA;case 8U: _LL9: _tmp4EC=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp4E1)->f2;_LLA:
# 3191
({struct Cyc_Absyn_Exp*_tmp9E5=Cyc_Absyn_uint_exp((unsigned int)_tmp4EC,0U);e=_tmp9E5;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp4E4=0U;({struct _dyneither_ptr _tmp9E6=({const char*_tmp4E5="compile_pat_test_as_case!";_tag_dyneither(_tmp4E5,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E6,_tag_dyneither(_tmp4E4,sizeof(void*),0U));});});}_LL0:;}
# 3195
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->tag=17U,_tmp4ED->f1=e;_tmp4ED;}),0U);}
# 3199
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3208
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3211
void*t=(void*)_check_null(p->topt);
void*_tmp4EE=p->r;void*_tmp4EF=_tmp4EE;union Cyc_Absyn_AggrInfo _tmp520;struct Cyc_List_List*_tmp51F;struct Cyc_List_List*_tmp51E;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_Pat*_tmp51C;struct Cyc_Absyn_Datatypedecl*_tmp51B;struct Cyc_Absyn_Datatypefield*_tmp51A;struct Cyc_List_List*_tmp519;struct Cyc_Absyn_Vardecl*_tmp518;struct Cyc_Absyn_Pat*_tmp517;struct Cyc_Absyn_Vardecl*_tmp516;struct Cyc_Absyn_Vardecl*_tmp515;struct Cyc_Absyn_Pat*_tmp514;struct Cyc_Absyn_Vardecl*_tmp513;switch(*((int*)_tmp4EF)){case 2U: _LL1: _tmp513=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp4EF)->f2;_LL2: {
# 3214
struct Cyc_Absyn_Pat*_tmp4F0=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp4F0->topt=p->topt;
_tmp515=_tmp513;_tmp514=_tmp4F0;goto _LL4;}case 1U: _LL3: _tmp515=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1;_tmp514=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp4EF)->f2;_LL4:
# 3219
({struct Cyc_List_List*_tmp9E7=({struct Cyc_List_List*_tmp4F1=_region_malloc(rgn,sizeof(*_tmp4F1));_tmp4F1->hd=_tmp515,_tmp4F1->tl=*decls;_tmp4F1;});*decls=_tmp9E7;});{
struct Cyc_Absyn_Stmt*_tmp4F2=({struct Cyc_Absyn_Exp*_tmp9E8=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3->tag=4U,_tmp4F3->f1=_tmp515;_tmp4F3;}),0U);Cyc_Absyn_assign_stmt(_tmp9E8,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmp9E9=_tmp4F2;Cyc_Toc_seq_stmt_opt(_tmp9E9,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp514));});};case 4U: _LL5: _tmp516=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp4EF)->f2;_LL6:
# 3224
({struct Cyc_List_List*_tmp9EA=({struct Cyc_List_List*_tmp4F4=_region_malloc(rgn,sizeof(*_tmp4F4));_tmp4F4->hd=_tmp516,_tmp4F4->tl=*decls;_tmp4F4;});*decls=_tmp9EA;});
return({struct Cyc_Absyn_Exp*_tmp9EB=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->tag=4U,_tmp4F5->f1=_tmp516;_tmp4F5;}),0U);Cyc_Absyn_assign_stmt(_tmp9EB,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp518=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1;_tmp517=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp4EF)->f2;_LLA:
# 3229
({struct Cyc_List_List*_tmp9EC=({struct Cyc_List_List*_tmp4F6=_region_malloc(rgn,sizeof(*_tmp4F6));_tmp4F6->hd=_tmp518,_tmp4F6->tl=*decls;_tmp4F6;});*decls=_tmp9EC;});
({void*_tmp9ED=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_tmp518->type=_tmp9ED;});{
# 3232
struct Cyc_Absyn_Stmt*_tmp4F7=({struct Cyc_Absyn_Exp*_tmp9F0=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->tag=4U,_tmp4F8->f1=_tmp518;_tmp4F8;}),0U);Cyc_Absyn_assign_stmt(_tmp9F0,({
void*_tmp9EF=({void*_tmp9EE=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_tmp9EE,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmp9EF,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmp9F1=_tmp4F7;Cyc_Toc_seq_stmt_opt(_tmp9F1,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp517));});};case 9U: _LLB: _LLC:
# 3237
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1)->r)->tag == 8U){_LL17: _tmp51B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1)->r)->f1;_tmp51A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1)->r)->f2;_tmp519=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1)->r)->f3;_LL18:
# 3246
 if(_tmp519 == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp51A->name,_tmp51B->name);
void*_tmp4F9=({void*_tmp9F2=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_tmp9F2,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmp9F3=Cyc_Toc_cast_it(_tmp4F9,path);path=_tmp9F3;});{
int cnt=1;
struct Cyc_List_List*_tmp4FA=_tmp51A->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp519 != 0;(_tmp519=_tmp519->tl,_tmp4FA=((struct Cyc_List_List*)_check_null(_tmp4FA))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp4FB=(struct Cyc_Absyn_Pat*)_tmp519->hd;
if(_tmp4FB->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp4FC=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4FA))->hd)).f2;
void*_tmp4FD=(void*)_check_null(_tmp4FB->topt);
void*_tmp4FE=Cyc_Toc_typ_to_c_array(_tmp4FD);
struct Cyc_Absyn_Exp*_tmp4FF=({struct Cyc_Absyn_Exp*_tmp9F4=path;Cyc_Absyn_aggrarrow_exp(_tmp9F4,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp4FC)))
({struct Cyc_Absyn_Exp*_tmp9F5=Cyc_Toc_cast_it(_tmp4FE,_tmp4FF);_tmp4FF=_tmp9F5;});
({struct Cyc_Absyn_Stmt*_tmp9F7=({struct Cyc_Absyn_Stmt*_tmp9F6=s;Cyc_Toc_seq_stmt_opt(_tmp9F6,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp4FF,_tmp4FB));});s=_tmp9F7;});};}
# 3264
return s;};};}else{_LL21: _tmp51C=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1;_LL22:
# 3309
 return({struct _RegionHandle*_tmp9FB=rgn;struct Cyc_Toc_Env**_tmp9FA=nv;struct Cyc_List_List**_tmp9F9=decls;struct Cyc_Absyn_Exp*_tmp9F8=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmp9FB,_tmp9FA,_tmp9F9,_tmp9F8,_tmp51C);});}case 8U: _LL19: _tmp51D=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp4EF)->f3;_LL1A:
# 3266
 _tmp51E=_tmp51D;goto _LL1C;case 5U: _LL1B: _tmp51E=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1;_LL1C: {
# 3268
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp51E != 0;(_tmp51E=_tmp51E->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp500=(struct Cyc_Absyn_Pat*)_tmp51E->hd;
if(_tmp500->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp501=(void*)_check_null(_tmp500->topt);
struct _dyneither_ptr*_tmp502=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpA01=({struct Cyc_Absyn_Stmt*_tmpA00=s;Cyc_Toc_seq_stmt_opt(_tmpA00,({struct _RegionHandle*_tmp9FF=rgn;struct Cyc_Toc_Env**_tmp9FE=nv;struct Cyc_List_List**_tmp9FD=decls;struct Cyc_Absyn_Exp*_tmp9FC=Cyc_Toc_member_exp(path,_tmp502,0U);Cyc_Toc_extract_pattern_vars(_tmp9FF,_tmp9FE,_tmp9FD,_tmp9FC,_tmp500);}));});s=_tmpA01;});};}
# 3278
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1 == 0){_LL1D: _LL1E:
({void*_tmp503=0U;({struct _dyneither_ptr _tmpA02=({const char*_tmp504="unresolved aggregate pattern!";_tag_dyneither(_tmp504,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA02,_tag_dyneither(_tmp503,sizeof(void*),0U));});});}else{_LL1F: _tmp520=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4EF)->f1;_tmp51F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp4EF)->f3;_LL20: {
# 3282
struct Cyc_Absyn_Aggrdecl*_tmp505=Cyc_Absyn_get_known_aggrdecl(_tmp520);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp51F != 0;_tmp51F=_tmp51F->tl){
struct _tuple37*_tmp506=(struct _tuple37*)_tmp51F->hd;
struct Cyc_Absyn_Pat*_tmp507=(*_tmp506).f2;
if(_tmp507->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp508=Cyc_Absyn_designatorlist_to_fieldname((*_tmp506).f1);
void*_tmp509=(void*)_check_null(_tmp507->topt);
void*_tmp50A=Cyc_Toc_typ_to_c_array(_tmp509);
struct Cyc_Absyn_Exp*_tmp50B=Cyc_Toc_member_exp(path,_tmp508,0U);
# 3294
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp505->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpA03=Cyc_Toc_member_exp(_tmp50B,Cyc_Toc_val_sp,0U);_tmp50B=_tmpA03;});{
void*_tmp50C=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp505->impl))->fields,_tmp508)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp50C))
({struct Cyc_Absyn_Exp*_tmpA04=Cyc_Toc_cast_it(_tmp50A,_tmp50B);_tmp50B=_tmpA04;});else{
if(!Cyc_Tcutil_is_array_type(_tmp50C) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp50C)))
# 3301
({struct Cyc_Absyn_Exp*_tmpA06=Cyc_Absyn_deref_exp(({void*_tmpA05=Cyc_Absyn_cstar_type(_tmp50A,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA05,
Cyc_Absyn_address_exp(_tmp50B,0U));}),0U);
# 3301
_tmp50B=_tmpA06;});}
# 3304
({struct Cyc_Absyn_Stmt*_tmpA08=({struct Cyc_Absyn_Stmt*_tmpA07=s;Cyc_Toc_seq_stmt_opt(_tmpA07,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp50B,_tmp507));});s=_tmpA08;});};};}
# 3306
return s;}}case 15U: _LL23: _LL24:
# 3311
({void*_tmp50D=0U;({struct _dyneither_ptr _tmpA09=({const char*_tmp50E="unknownid pat";_tag_dyneither(_tmp50E,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA09,_tag_dyneither(_tmp50D,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp50F=0U;({struct _dyneither_ptr _tmpA0A=({const char*_tmp510="unknowncall pat";_tag_dyneither(_tmp510,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA0A,_tag_dyneither(_tmp50F,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp511=0U;({struct _dyneither_ptr _tmpA0B=({const char*_tmp512="exp pat";_tag_dyneither(_tmp512,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA0B,_tag_dyneither(_tmp511,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3322
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3330
void*_tmp521=dopt;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp55E;void*_tmp55D;struct Cyc_Tcpat_Rhs*_tmp55C;if(_tmp521 == 0){_LL1: _LL2:
# 3332
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp521)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp55C=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp521)->f1;_LL6:
# 3337
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp522=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp523=_tmp522;int*_tmp52A;struct _dyneither_ptr*_tmp529;struct _dyneither_ptr*_tmp528;struct Cyc_Absyn_Switch_clause*_tmp527;_LLA: _tmp52A=(int*)& _tmp523->f1;_tmp529=_tmp523->f2;_tmp528=_tmp523->f3;_tmp527=_tmp523->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp524=_tmp527->body;
if(_tmp524 == _tmp55C->rhs){
# 3342
if(*_tmp52A)
return Cyc_Absyn_goto_stmt(_tmp529,0U);
*_tmp52A=1;{
# 3346
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA0E=rgn;struct Cyc_List_List**_tmpA0D=decls;struct Cyc_Absyn_Exp*_tmpA0C=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA0E,& nv,_tmpA0D,_tmpA0C,_tmp527->pattern);});
# 3349
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp528,_tmp527->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpA0F=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpA0F;});
({struct Cyc_Absyn_Stmt*_tmpA10=Cyc_Absyn_label_stmt(_tmp529,res,0U);res=_tmpA10;});
({struct Cyc_List_List*_tmpA12=({struct Cyc_List_List*_tmp526=_region_malloc(rgn,sizeof(*_tmp526));({struct _tuple38*_tmpA11=({struct _tuple38*_tmp525=_region_malloc(rgn,sizeof(*_tmp525));_tmp525->f1=nv,_tmp525->f2=_tmp528,_tmp525->f3=_tmp524;_tmp525;});_tmp526->hd=_tmpA11;}),_tmp526->tl=*bodies;_tmp526;});*bodies=_tmpA12;});
return res;};}};}
# 3357
({void*_tmp52B=0U;({struct _dyneither_ptr _tmpA13=({const char*_tmp52C="couldn't find rhs!";_tag_dyneither(_tmp52C,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA13,_tag_dyneither(_tmp52B,sizeof(void*),0U));});});default: _LL7: _tmp55F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp521)->f1;_tmp55E=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp521)->f2;_tmp55D=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp521)->f3;_LL8: {
# 3360
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp55D,lscs,v);
# 3362
struct Cyc_Absyn_Exp*_tmp52D=({struct Cyc_List_List*_tmpA14=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp55F);Cyc_Toc_compile_path(_tmpA14,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp52E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp55E);
# 3365
struct _tuple36 _tmp52F=Cyc_Toc_admits_switch(_tmp52E);struct _tuple36 _tmp530=_tmp52F;int _tmp55B;void*_tmp55A;_LLD: _tmp55B=_tmp530.f1;_tmp55A=_tmp530.f2;_LLE:;
if(_tmp55B > 1){
# 3369
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp541=_cycalloc(sizeof(*_tmp541));
({struct Cyc_Absyn_Switch_clause*_tmpA16=({struct Cyc_Absyn_Switch_clause*_tmp540=_cycalloc(sizeof(*_tmp540));({struct Cyc_Absyn_Pat*_tmpA15=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp540->pattern=_tmpA15;}),_tmp540->pat_vars=0,_tmp540->where_clause=0,_tmp540->body=res,_tmp540->loc=0U;_tmp540;});_tmp541->hd=_tmpA16;}),_tmp541->tl=0;_tmp541;});
# 3372
for(0;_tmp52E != 0;_tmp52E=_tmp52E->tl){
struct _tuple32 _tmp531=*((struct _tuple32*)_tmp52E->hd);struct _tuple32 _tmp532=_tmp531;void*_tmp538;void*_tmp537;_LL10: _tmp538=_tmp532.f1;_tmp537=_tmp532.f2;_LL11:;{
# 3375
struct Cyc_Absyn_Pat*_tmp533=Cyc_Toc_compile_pat_test_as_case(_tmp538);
# 3377
struct Cyc_Absyn_Stmt*_tmp534=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp537,lscs,v);
# 3379
({struct Cyc_List_List*_tmpA18=({struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));({struct Cyc_Absyn_Switch_clause*_tmpA17=({struct Cyc_Absyn_Switch_clause*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->pattern=_tmp533,_tmp535->pat_vars=0,_tmp535->where_clause=0,_tmp535->body=_tmp534,_tmp535->loc=0U;_tmp535;});_tmp536->hd=_tmpA17;}),_tmp536->tl=new_lscs;_tmp536;});new_lscs=_tmpA18;});};}
# 3381
{void*_tmp539=_tmp55A;struct _dyneither_ptr*_tmp53E;switch(*((int*)_tmp539)){case 1U: _LL13: _LL14:
# 3384
 LOOP1: {
void*_tmp53A=_tmp52D->r;void*_tmp53B=_tmp53A;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53C;switch(*((int*)_tmp53B)){case 14U: _LL1C: _tmp53C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53B)->f2;_LL1D:
 _tmp52D=_tmp53C;goto LOOP1;case 20U: _LL1E: _tmp53D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp53B)->f1;_LL1F:
 _tmp52D=_tmp53D;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3390
({struct Cyc_Absyn_Exp*_tmpA1A=Cyc_Absyn_deref_exp(({void*_tmpA19=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA19,_tmp52D);}),0U);_tmp52D=_tmpA1A;});goto _LL12;case 3U: _LL15: _tmp53E=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp539)->f1;_LL16:
# 3393
({struct Cyc_Absyn_Exp*_tmpA1C=({struct Cyc_Absyn_Exp*_tmpA1B=Cyc_Toc_member_exp(_tmp52D,_tmp53E,0U);Cyc_Toc_member_exp(_tmpA1B,Cyc_Toc_tag_sp,0U);});_tmp52D=_tmpA1C;});goto _LL12;case 2U: _LL17: _LL18:
# 3395
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3398
({struct Cyc_Absyn_Stmt*_tmpA1D=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->tag=10U,_tmp53F->f1=_tmp52D,_tmp53F->f2=new_lscs,_tmp53F->f3=0;_tmp53F;}),0U);res=_tmpA1D;});}else{
# 3402
void*_tmp542=_tmp55A;struct Cyc_Absyn_Exp*_tmp559;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp542)->tag == 2U){_LL23: _tmp559=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp542)->f1;_LL24: {
# 3404
struct Cyc_List_List*_tmp543=_tmp52E;void*_tmp552;struct Cyc_Tcpat_Rhs*_tmp551;if(_tmp543 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)((struct Cyc_List_List*)_tmp543)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp543)->tl == 0){_LL28: _tmp552=((struct _tuple32*)_tmp543->hd)->f1;_tmp551=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple32*)_tmp543->hd)->f2)->f1;_LL29:
# 3410
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp544=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp545=_tmp544;int*_tmp54C;struct _dyneither_ptr*_tmp54B;struct _dyneither_ptr*_tmp54A;struct Cyc_Absyn_Switch_clause*_tmp549;_LL2D: _tmp54C=(int*)& _tmp545->f1;_tmp54B=_tmp545->f2;_tmp54A=_tmp545->f3;_tmp549=_tmp545->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp546=_tmp549->body;
if(_tmp546 == _tmp551->rhs){
# 3415
if(*_tmp54C)
return Cyc_Absyn_goto_stmt(_tmp54B,0U);
*_tmp54C=1;{
# 3419
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpA20=rgn;struct Cyc_List_List**_tmpA1F=decls;struct Cyc_Absyn_Exp*_tmpA1E=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpA20,& nv,_tmpA1F,_tmpA1E,_tmp549->pattern);});
# 3422
Cyc_Toc_exp_to_c(nv,_tmp559);{
# 3425
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp54A,_tmp549->body,0U);
({struct Cyc_Absyn_Stmt*_tmpA21=Cyc_Absyn_ifthenelse_stmt(_tmp559,r,res,0U);r=_tmpA21;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpA22=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpA22;});
({struct Cyc_Absyn_Stmt*_tmpA23=Cyc_Absyn_label_stmt(_tmp54B,r,0U);r=_tmpA23;});
({struct Cyc_List_List*_tmpA25=({struct Cyc_List_List*_tmp548=_region_malloc(rgn,sizeof(*_tmp548));({struct _tuple38*_tmpA24=({struct _tuple38*_tmp547=_region_malloc(rgn,sizeof(*_tmp547));_tmp547->f1=nv,_tmp547->f2=_tmp54A,_tmp547->f3=_tmp546;_tmp547;});_tmp548->hd=_tmpA24;}),_tmp548->tl=*bodies;_tmp548;});*bodies=_tmpA25;});
return r;};};}};}
# 3434
({void*_tmp54D=0U;({struct _dyneither_ptr _tmpA26=({const char*_tmp54E="couldn't find rhs!";_tag_dyneither(_tmp54E,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA26,_tag_dyneither(_tmp54D,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp54F=0U;({struct _dyneither_ptr _tmpA27=({const char*_tmp550="bad where clause in match compiler";_tag_dyneither(_tmp550,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA27,_tag_dyneither(_tmp54F,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3439
 for(0;_tmp52E != 0;_tmp52E=_tmp52E->tl){
struct _tuple32 _tmp553=*((struct _tuple32*)_tmp52E->hd);struct _tuple32 _tmp554=_tmp553;void*_tmp558;void*_tmp557;_LL30: _tmp558=_tmp554.f1;_tmp557=_tmp554.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp555=Cyc_Toc_compile_pat_test(_tmp52D,_tmp558);
struct Cyc_Absyn_Stmt*_tmp556=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp557,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpA28=Cyc_Absyn_ifthenelse_stmt(_tmp555,_tmp556,res,0U);res=_tmpA28;});};}}_LL22:;}
# 3447
return res;}}}_LL0:;}
# 3457
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3459
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp560=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp561=_tmp560;struct Cyc_Toc_Env**_tmp563;struct Cyc_Absyn_Stmt*_tmp562;_LL1: _tmp563=(struct Cyc_Toc_Env**)& _tmp561->f1;_tmp562=_tmp561->f3;_LL2:;
if(_tmp562 == s)return _tmp563;}
# 3465
return 0;}
# 3469
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3472
void*_tmp564=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3475
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp565=_new_region("rgn");struct _RegionHandle*rgn=& _tmp565;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp566=Cyc_Toc_share_env(rgn,nv);
# 3482
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3487
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp566,& mydecls,& mybodies,dopt,lscs,v);
# 3495
{struct Cyc_List_List*_tmp567=lscs;for(0;_tmp567 != 0;_tmp567=_tmp567->tl){
struct _tuple35*_tmp568=(struct _tuple35*)_tmp567->hd;struct _tuple35*_tmp569=_tmp568;struct _dyneither_ptr*_tmp571;struct Cyc_Absyn_Switch_clause*_tmp570;_LL1: _tmp571=_tmp569->f3;_tmp570=_tmp569->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp570->body;
# 3499
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3502
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp56A=*envp;
# 3505
if(_tmp567->tl != 0){
struct _tuple35*_tmp56B=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp567->tl))->hd;struct _tuple35*_tmp56C=_tmp56B;struct _dyneither_ptr*_tmp56F;struct Cyc_Absyn_Switch_clause*_tmp56E;_LL4: _tmp56F=_tmp56C->f3;_tmp56E=_tmp56C->f4;_LL5:;{
# 3509
struct Cyc_Toc_Env**_tmp56D=Cyc_Toc_find_case_env(mybodies,_tmp56E->body);
# 3514
if(_tmp56D == 0)
({struct Cyc_Toc_Env*_tmpA29=Cyc_Toc_last_switchclause_env(rgn,_tmp56A,end_l);Cyc_Toc_stmt_to_c(_tmpA29,s);});else{
# 3518
struct Cyc_List_List*vs=0;
if(_tmp56E->pat_vars != 0){
({struct Cyc_List_List*_tmpA2A=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp56E->pat_vars))->v)).f1);vs=_tmpA2A;});
({struct Cyc_List_List*_tmpA2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpA2B;});}
# 3523
({struct Cyc_Toc_Env*_tmpA2C=Cyc_Toc_non_last_switchclause_env(rgn,_tmp56A,end_l,_tmp56F,vs);Cyc_Toc_stmt_to_c(_tmpA2C,s);});}};}else{
# 3527
({struct Cyc_Toc_Env*_tmpA2D=Cyc_Toc_last_switchclause_env(rgn,_tmp56A,end_l);Cyc_Toc_stmt_to_c(_tmpA2D,s);});}};};}}{
# 3531
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpA2F=test_tree;Cyc_Absyn_seq_stmt(_tmpA2F,({struct _dyneither_ptr*_tmpA2E=end_l;Cyc_Absyn_label_stmt(_tmpA2E,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3533
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp572=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpA30=Cyc_Toc_temp_var();_tmp572->name=_tmpA30;});
({void*_tmpA31=Cyc_Toc_typ_to_c_array(_tmp572->type);_tmp572->type=_tmpA31;});
({struct Cyc_Absyn_Stmt*_tmpA33=({struct Cyc_Absyn_Decl*_tmpA32=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=0U,_tmp573->f1=_tmp572;_tmp573;}),0U);Cyc_Absyn_decl_stmt(_tmpA32,res,0U);});res=_tmpA33;});}
# 3540
({void*_tmpA37=({struct _tuple1*_tmpA36=v;void*_tmpA35=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpA34=e;Cyc_Absyn_declare_stmt(_tmpA36,_tmpA35,_tmpA34,res,0U);})->r;whole_s->r=_tmpA37;});
_npop_handler(0U);return;};}
# 3479
;_pop_region(rgn);};}
# 3546
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3548
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3553
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3555
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA39=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpA39,({struct Cyc_List_List*_tmp574=_cycalloc(sizeof(*_tmp574));
({struct Cyc_Absyn_Exp*_tmpA38=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp574->hd=_tmpA38;}),_tmp574->tl=0;_tmp574;}),0U);}),0U);}
# 3559
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpA3B=({struct Cyc_Absyn_Stmt*_tmpA3A=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpA3A,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpA3B;});}
# 3564
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3566
while(1){
void*_tmp575=s->r;void*_tmp576=_tmp575;struct Cyc_Absyn_Stmt*_tmp5CD;struct Cyc_List_List*_tmp5CC;void*_tmp5CB;struct Cyc_Absyn_Stmt*_tmp5CA;struct Cyc_Absyn_Exp*_tmp5C9;struct _dyneither_ptr*_tmp5C8;struct Cyc_Absyn_Stmt*_tmp5C7;struct Cyc_Absyn_Decl*_tmp5C6;struct Cyc_Absyn_Stmt*_tmp5C5;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_Switch_clause**_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C2;struct Cyc_List_List*_tmp5C1;void*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Stmt*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Stmt*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5B9;struct Cyc_Absyn_Stmt*_tmp5B8;struct Cyc_Absyn_Stmt*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Stmt*_tmp5B5;struct Cyc_Absyn_Stmt*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B3;switch(*((int*)_tmp576)){case 0U: _LL1: _LL2:
# 3569
 return;case 1U: _LL3: _tmp5B3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_LL4:
# 3571
 Cyc_Toc_exp_to_c(nv,_tmp5B3);
return;case 2U: _LL5: _tmp5B5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5B4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL6:
# 3574
 Cyc_Toc_stmt_to_c(nv,_tmp5B5);
s=_tmp5B4;
continue;case 3U: _LL7: _tmp5B6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_LL8: {
# 3578
void*topt=0;
if(_tmp5B6 != 0){
({void*_tmpA3C=Cyc_Toc_typ_to_c((void*)_check_null(_tmp5B6->topt));topt=_tmpA3C;});
Cyc_Toc_exp_to_c(nv,_tmp5B6);}{
# 3584
int _tmp577=Cyc_Toc_get_npop(s);
if(_tmp577 > 0){
if(topt != 0){
struct _tuple1*_tmp578=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp579=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp578,0U),0U);
({void*_tmpA41=({struct _tuple1*_tmpA40=_tmp578;void*_tmpA3F=topt;struct Cyc_Absyn_Exp*_tmpA3E=_tmp5B6;Cyc_Absyn_declare_stmt(_tmpA40,_tmpA3F,_tmpA3E,({
struct Cyc_Absyn_Stmt*_tmpA3D=Cyc_Toc_make_npop_handler(_tmp577);Cyc_Absyn_seq_stmt(_tmpA3D,_tmp579,0U);}),0U);})->r;
# 3589
s->r=_tmpA41;});}else{
# 3593
Cyc_Toc_do_npop_before(_tmp577,s);}}
# 3595
return;};}case 4U: _LL9: _tmp5B9=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5B8=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_tmp5B7=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp576)->f3;_LLA:
# 3597
 Cyc_Toc_exp_to_c(nv,_tmp5B9);
Cyc_Toc_stmt_to_c(nv,_tmp5B8);
s=_tmp5B7;
continue;case 5U: _LLB: _tmp5BB=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp576)->f1).f1;_tmp5BA=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LLC:
# 3602
 Cyc_Toc_exp_to_c(nv,_tmp5BB);{
struct _RegionHandle _tmp57A=_new_region("temp");struct _RegionHandle*temp=& _tmp57A;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA42=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA42,_tmp5BA);});
# 3606
_npop_handler(0U);return;
# 3603
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3608
struct Cyc_Toc_Env*_tmp57B=nv;struct _dyneither_ptr**_tmp57C;_LL22: _tmp57C=_tmp57B->break_lab;_LL23:;
if(_tmp57C != 0)
({void*_tmpA43=Cyc_Toc_goto_stmt_r(*_tmp57C);s->r=_tmpA43;});
# 3612
({int _tmpA44=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA44,s);});
return;}case 7U: _LLF: _LL10: {
# 3615
struct Cyc_Toc_Env*_tmp57D=nv;struct _dyneither_ptr**_tmp57E;_LL25: _tmp57E=_tmp57D->continue_lab;_LL26:;
if(_tmp57E != 0)
({void*_tmpA45=Cyc_Toc_goto_stmt_r(*_tmp57E);s->r=_tmpA45;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3621
({int _tmpA46=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA46,s);});
return;case 9U: _LL13: _tmp5BF=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5BE=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp576)->f2).f1;_tmp5BD=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp576)->f3).f1;_tmp5BC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp576)->f4;_LL14:
# 3625
 Cyc_Toc_exp_to_c(nv,_tmp5BF);Cyc_Toc_exp_to_c(nv,_tmp5BE);Cyc_Toc_exp_to_c(nv,_tmp5BD);{
struct _RegionHandle _tmp57F=_new_region("temp");struct _RegionHandle*temp=& _tmp57F;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA47=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA47,_tmp5BC);});
# 3629
_npop_handler(0U);return;
# 3626
;_pop_region(temp);};case 10U: _LL15: _tmp5C2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5C1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_tmp5C0=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp576)->f3;_LL16:
# 3631
 Cyc_Toc_xlate_switch(nv,s,_tmp5C2,_tmp5C1,_tmp5C0);
return;case 11U: _LL17: _tmp5C4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5C3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL18: {
# 3634
struct Cyc_Toc_Env*_tmp580=nv;struct Cyc_Toc_FallthruInfo*_tmp58A;_LL28: _tmp58A=_tmp580->fallthru_info;_LL29:;
if(_tmp58A == 0)
({void*_tmp581=0U;({struct _dyneither_ptr _tmpA48=({const char*_tmp582="fallthru in unexpected place";_tag_dyneither(_tmp582,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA48,_tag_dyneither(_tmp581,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp583=*_tmp58A;struct Cyc_Toc_FallthruInfo _tmp584=_tmp583;struct _dyneither_ptr*_tmp589;struct Cyc_List_List*_tmp588;_LL2B: _tmp589=_tmp584.label;_tmp588=_tmp584.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp589,0U);
# 3640
({int _tmpA49=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpA49,s2);});{
struct Cyc_List_List*_tmp585=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp588);
struct Cyc_List_List*_tmp586=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5C4);
for(0;_tmp585 != 0;(_tmp585=_tmp585->tl,_tmp586=_tmp586->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp586))->hd);
({struct Cyc_Absyn_Stmt*_tmpA4C=({struct Cyc_Absyn_Stmt*_tmpA4B=({struct Cyc_Absyn_Exp*_tmpA4A=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->tag=5U,_tmp587->f1=(struct Cyc_Absyn_Vardecl*)_tmp585->hd;_tmp587;}),0U);Cyc_Absyn_assign_stmt(_tmpA4A,(struct Cyc_Absyn_Exp*)_tmp586->hd,0U);});Cyc_Absyn_seq_stmt(_tmpA4B,s2,0U);});s2=_tmpA4C;});}
# 3648
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp5C6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5C5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL1A:
# 3654
{void*_tmp58B=_tmp5C6->r;void*_tmp58C=_tmp58B;struct Cyc_Absyn_Tvar*_tmp5A7;struct Cyc_Absyn_Vardecl*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Fndecl*_tmp5A4;struct Cyc_List_List*_tmp5A3;struct Cyc_Absyn_Pat*_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A1;void*_tmp5A0;struct Cyc_Absyn_Vardecl*_tmp59F;switch(*((int*)_tmp58C)){case 0U: _LL2E: _tmp59F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp58C)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp59F,_tmp5C5);goto _LL2D;case 2U: _LL30: _tmp5A2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp58C)->f1;_tmp5A1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp58C)->f3;_tmp5A0=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp58C)->f4;_LL31:
# 3662
{void*_tmp58D=_tmp5A2->r;void*_tmp58E=_tmp58D;struct Cyc_Absyn_Vardecl*_tmp590;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp58E)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp58E)->f2)->r)->tag == 0U){_LL3B: _tmp590=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp58E)->f1;_LL3C:
# 3664
({struct _tuple1*_tmpA4D=Cyc_Toc_temp_var();_tmp590->name=_tmpA4D;});
_tmp590->initializer=_tmp5A1;
({void*_tmpA4E=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->tag=0U,_tmp58F->f1=_tmp590;_tmp58F;});_tmp5C6->r=_tmpA4E;});
Cyc_Toc_local_decl_to_c(nv,_tmp590,_tmp5C5);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3673
({void*_tmpA4F=(Cyc_Toc_letdecl_to_c(nv,_tmp5A2,_tmp5A0,(void*)_check_null(_tmp5A1->topt),_tmp5A1,_tmp5C5))->r;s->r=_tmpA4F;});
goto _LL3A;}_LL3A:;}
# 3676
goto _LL2D;case 3U: _LL32: _tmp5A3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp58C)->f1;_LL33: {
# 3680
struct Cyc_List_List*_tmp591=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5A3);
if(_tmp591 == 0)
({void*_tmp592=0U;({struct _dyneither_ptr _tmpA50=({const char*_tmp593="empty Letv_d";_tag_dyneither(_tmp593,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA50,_tag_dyneither(_tmp592,sizeof(void*),0U));});});
({void*_tmpA51=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->tag=0U,_tmp594->f1=(struct Cyc_Absyn_Vardecl*)_tmp591->hd;_tmp594;});_tmp5C6->r=_tmpA51;});
_tmp591=_tmp591->tl;
for(0;_tmp591 != 0;_tmp591=_tmp591->tl){
struct Cyc_Absyn_Decl*_tmp595=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->tag=0U,_tmp596->f1=(struct Cyc_Absyn_Vardecl*)_tmp591->hd;_tmp596;}),0U);
({void*_tmpA53=({struct Cyc_Absyn_Decl*_tmpA52=_tmp595;Cyc_Absyn_decl_stmt(_tmpA52,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpA53;});}
# 3689
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp5A4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp58C)->f1;_LL35:
# 3692
 Cyc_Toc_fndecl_to_c(nv,_tmp5A4,0);
Cyc_Toc_stmt_to_c(nv,_tmp5C5);
goto _LL2D;case 4U: _LL36: _tmp5A7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp58C)->f1;_tmp5A6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp58C)->f2;_tmp5A5=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp58C)->f3;_LL37: {
# 3696
struct Cyc_Absyn_Stmt*_tmp597=_tmp5C5;
# 3698
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp5A6->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3705
Cyc_Toc_stmt_to_c(nv,_tmp597);
if(Cyc_Absyn_no_regions)
({void*_tmpA57=({struct _tuple1*_tmpA56=x_var;void*_tmpA55=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA54=
Cyc_Absyn_uint_exp(0U,0U);
# 3707
Cyc_Absyn_declare_stmt(_tmpA56,_tmpA55,_tmpA54,_tmp597,0U);})->r;s->r=_tmpA57;});else{
# 3709
if((unsigned int)_tmp5A5){
# 3711
Cyc_Toc_exp_to_c(nv,_tmp5A5);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpA59=_tmp5A5;Cyc_Absyn_aggrarrow_exp(_tmpA59,({struct _dyneither_ptr*_tmp599=_cycalloc(sizeof(*_tmp599));({struct _dyneither_ptr _tmpA58=({const char*_tmp598="h";_tag_dyneither(_tmp598,sizeof(char),2U);});*_tmp599=_tmpA58;});_tmp599;}),0U);}),0U);
({void*_tmpA5A=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp597,0U))->r;s->r=_tmpA5A;});};}else{
# 3721
({void*_tmpA67=({
struct _tuple1*_tmpA66=rh_var;void*_tmpA65=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpA64=({
struct Cyc_Absyn_Exp*_tmpA5C=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpA5C,({struct Cyc_List_List*_tmp59A=_cycalloc(sizeof(*_tmp59A));
({struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp59A->hd=_tmpA5B;}),_tmp59A->tl=0;_tmp59A;}),0U);});
# 3722
Cyc_Absyn_declare_stmt(_tmpA66,_tmpA65,_tmpA64,({
# 3726
struct _tuple1*_tmpA63=x_var;void*_tmpA62=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpA61=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpA63,_tmpA62,_tmpA61,({
struct Cyc_Absyn_Stmt*_tmpA60=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA5D=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpA5D,({struct Cyc_Absyn_Exp*_tmp59B[1U];_tmp59B[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59B,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpA60,({
struct Cyc_Absyn_Stmt*_tmpA5F=_tmp597;Cyc_Absyn_seq_stmt(_tmpA5F,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpA5E,({struct Cyc_Absyn_Exp*_tmp59C[1U];_tmp59C[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59C,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3721
s->r=_tmpA67;});}}
# 3732
return;}default: _LL38: _LL39:
({void*_tmp59D=0U;({struct _dyneither_ptr _tmpA68=({const char*_tmp59E="bad nested declaration within function";_tag_dyneither(_tmp59E,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpA68,_tag_dyneither(_tmp59D,sizeof(void*),0U));});});}_LL2D:;}
# 3735
return;case 13U: _LL1B: _tmp5C8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5C7=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL1C:
# 3737
 s=_tmp5C7;continue;case 14U: _LL1D: _tmp5CA=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5C9=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp576)->f2).f1;_LL1E: {
# 3739
struct _RegionHandle _tmp5A8=_new_region("temp");struct _RegionHandle*temp=& _tmp5A8;_push_region(temp);
({struct Cyc_Toc_Env*_tmpA69=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpA69,_tmp5CA);});
Cyc_Toc_exp_to_c(nv,_tmp5C9);
# 3743
_npop_handler(0U);return;
# 3739
;_pop_region(temp);}default: _LL1F: _tmp5CD=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp5CC=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_tmp5CB=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp576)->f3;_LL20: {
# 3757 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3767
e_exp->topt=e_typ;{
struct _RegionHandle _tmp5A9=_new_region("temp");struct _RegionHandle*temp=& _tmp5A9;_push_region(temp);
# 3770
Cyc_Toc_stmt_to_c(nv,_tmp5CD);{
struct Cyc_Absyn_Stmt*_tmp5AA=({struct Cyc_Absyn_Stmt*_tmpA6A=_tmp5CD;Cyc_Absyn_seq_stmt(_tmpA6A,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 3775
struct Cyc_Absyn_Stmt*_tmp5AB=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2->tag=10U,_tmp5B2->f1=e_exp,_tmp5B2->f2=_tmp5CC,_tmp5B2->f3=_tmp5CB;_tmp5B2;}),0U);
# 3777
Cyc_Toc_stmt_to_c(nv,_tmp5AB);{
# 3780
struct Cyc_Absyn_Exp*_tmp5AC=({
struct Cyc_Absyn_Exp*_tmpA6C=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpA6C,({struct Cyc_List_List*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));
({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp5B1->hd=_tmpA6B;}),_tmp5B1->tl=0;_tmp5B1;}),0U);});
# 3784
struct Cyc_Absyn_Stmt*_tmp5AD=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpA6E,({struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));
({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_Absyn_address_exp(h_exp,0U);_tmp5B0->hd=_tmpA6D;}),_tmp5B0->tl=0;_tmp5B0;}),0U);}),0U);
# 3788
struct Cyc_Absyn_Exp*_tmp5AE=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp5AF=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpA7D=({
struct _tuple1*_tmpA7C=h_var;void*_tmpA7B=h_typ;Cyc_Absyn_declare_stmt(_tmpA7C,_tmpA7B,0,({
struct Cyc_Absyn_Stmt*_tmpA7A=_tmp5AD;Cyc_Absyn_seq_stmt(_tmpA7A,({
struct _tuple1*_tmpA79=was_thrown_var;void*_tmpA78=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpA77=_tmp5AE;Cyc_Absyn_declare_stmt(_tmpA79,_tmpA78,_tmpA77,({
struct Cyc_Absyn_Stmt*_tmpA76=({struct Cyc_Absyn_Exp*_tmpA70=_tmp5AC;struct Cyc_Absyn_Stmt*_tmpA6F=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp5AF,0U);
# 3794
Cyc_Absyn_ifthenelse_stmt(_tmpA70,_tmpA6F,
# 3796
Cyc_Toc_skip_stmt_dl(),0U);});
# 3794
Cyc_Absyn_seq_stmt(_tmpA76,({
# 3797
struct Cyc_Absyn_Exp*_tmpA75=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpA74=_tmp5AA;Cyc_Absyn_ifthenelse_stmt(_tmpA75,_tmpA74,({
# 3799
struct _tuple1*_tmpA73=e_var;void*_tmpA72=e_typ;struct Cyc_Absyn_Exp*_tmpA71=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 3799
Cyc_Absyn_declare_stmt(_tmpA73,_tmpA72,_tmpA71,_tmp5AB,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3790
s->r=_tmpA7D;});};};
# 3803
_npop_handler(0U);return;
# 3768
;_pop_region(temp);};}}_LL0:;}}
# 3812
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpA7E=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpA7E;});{
struct _RegionHandle _tmp5CE=_new_region("frgn");struct _RegionHandle*frgn=& _tmp5CE;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp5CF=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp5D0=f->args;for(0;_tmp5D0 != 0;_tmp5D0=_tmp5D0->tl){
struct _tuple1*_tmp5D1=({struct _tuple1*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->f1=Cyc_Absyn_Loc_n,_tmp5D2->f2=(*((struct _tuple9*)_tmp5D0->hd)).f1;_tmp5D2;});
({void*_tmpA7F=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp5D0->hd)).f3);(*((struct _tuple9*)_tmp5D0->hd)).f3=_tmpA7F;});}}
# 3828
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 3832
({struct Cyc_Hashtable_Table**_tmpA81=({struct Cyc_Hashtable_Table**_tmp5D3=_cycalloc(sizeof(*_tmp5D3));({struct Cyc_Hashtable_Table*_tmpA80=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp5D3=_tmpA80;});_tmp5D3;});Cyc_Toc_fn_pop_table=_tmpA81;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp5D4=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp5D5=_tmp5D4;struct _dyneither_ptr*_tmp5DE;struct Cyc_Absyn_Tqual _tmp5DD;void*_tmp5DC;int _tmp5DB;_LL1: _tmp5DE=_tmp5D5.name;_tmp5DD=_tmp5D5.tq;_tmp5DC=_tmp5D5.type;_tmp5DB=_tmp5D5.inject;_LL2:;{
void*_tmp5D6=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(_tmp5DC,Cyc_Absyn_heap_rgn_type,_tmp5DD,Cyc_Absyn_false_type));
struct _tuple1*_tmp5D7=({struct _tuple1*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->f1=Cyc_Absyn_Loc_n,_tmp5DA->f2=(struct _dyneither_ptr*)_check_null(_tmp5DE);_tmp5DA;});
({struct Cyc_List_List*_tmpA84=({struct Cyc_List_List*_tmpA83=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA83,({struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct _tuple9*_tmpA82=({struct _tuple9*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->f1=_tmp5DE,_tmp5D8->f2=_tmp5DD,_tmp5D8->f3=_tmp5D6;_tmp5D8;});_tmp5D9->hd=_tmpA82;}),_tmp5D9->tl=0;_tmp5D9;}));});f->args=_tmpA84;});
f->cyc_varargs=0;};}
# 3841
{struct Cyc_List_List*_tmp5DF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp5DF != 0;_tmp5DF=_tmp5DF->tl){
({void*_tmpA85=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp5DF->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp5DF->hd)->type=_tmpA85;});}}
# 3844
({struct Cyc_Toc_Env*_tmpA86=Cyc_Toc_clear_toplevel(frgn,_tmp5CF);Cyc_Toc_stmt_to_c(_tmpA86,f->body);});}
# 3821
;_pop_region(frgn);};}
# 3848
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp5E0=s;switch(_tmp5E0){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Aggrdecl**f2;};
# 3865 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple39*env){
# 3867
struct _tuple39 _tmp5E1=*env;struct _tuple39 _tmp5E2=_tmp5E1;struct Cyc_Toc_TocState*_tmp616;struct Cyc_Absyn_Aggrdecl*_tmp615;_LL1: _tmp616=_tmp5E2.f1;_tmp615=*_tmp5E2.f2;_LL2:;{
struct _tuple1*_tmp5E3=_tmp615->name;
struct Cyc_Toc_TocState _tmp5E4=*_tmp616;struct Cyc_Toc_TocState _tmp5E5=_tmp5E4;struct Cyc_Dict_Dict*_tmp614;_LL4: _tmp614=_tmp5E5.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple18**_tmp5E6=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp614,_tmp5E3);
if(_tmp5E6 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp615->kind == Cyc_Absyn_StructA)
({struct _tuple18*_tmpA88=({struct _tuple18*_tmp5E7=_region_malloc(d,sizeof(*_tmp5E7));_tmp5E7->f1=_tmp615,({void*_tmpA87=Cyc_Absyn_strctq(_tmp5E3);_tmp5E7->f2=_tmpA87;});_tmp5E7;});v=_tmpA88;});else{
# 3878
({struct _tuple18*_tmpA8A=({struct _tuple18*_tmp5E8=_region_malloc(d,sizeof(*_tmp5E8));_tmp5E8->f1=_tmp615,({void*_tmpA89=Cyc_Absyn_unionq_type(_tmp5E3);_tmp5E8->f2=_tmpA89;});_tmp5E8;});v=_tmpA8A;});}
({struct Cyc_Dict_Dict _tmpA8B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp614,_tmp5E3,v);*_tmp614=_tmpA8B;});};}else{
# 3881
struct _tuple18*_tmp5E9=*_tmp5E6;struct _tuple18*_tmp5EA=_tmp5E9;struct Cyc_Absyn_Aggrdecl*_tmp5ED;void*_tmp5EC;_LL7: _tmp5ED=_tmp5EA->f1;_tmp5EC=_tmp5EA->f2;_LL8:;
if(_tmp5ED->impl == 0){
({struct Cyc_Dict_Dict _tmpA8E=({struct Cyc_Dict_Dict _tmpA8D=*_tmp614;struct _tuple1*_tmpA8C=_tmp5E3;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(_tmpA8D,_tmpA8C,({struct _tuple18*_tmp5EB=_region_malloc(d,sizeof(*_tmp5EB));_tmp5EB->f1=_tmp615,_tmp5EB->f2=_tmp5EC;_tmp5EB;}));});*_tmp614=_tmpA8E;});
seen_defn_before=0;}else{
# 3886
seen_defn_before=1;}}{
# 3888
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->kind=_tmp615->kind,_tmp613->sc=Cyc_Absyn_Public,_tmp613->name=_tmp615->name,_tmp613->tvs=0,_tmp613->impl=0,_tmp613->expected_mem_kind=0,_tmp613->attributes=_tmp615->attributes;_tmp613;});
# 3895
if(_tmp615->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpA8F=({struct Cyc_Absyn_AggrdeclImpl*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->exist_vars=0,_tmp5EE->rgn_po=0,_tmp5EE->fields=0,_tmp5EE->tagged=0;_tmp5EE;});new_ad->impl=_tmpA8F;});{
# 3900
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp5EF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp615->impl))->fields;for(0;_tmp5EF != 0;_tmp5EF=_tmp5EF->tl){
# 3904
struct Cyc_Absyn_Aggrfield*_tmp5F0=(struct Cyc_Absyn_Aggrfield*)_tmp5EF->hd;
void*_tmp5F1=_tmp5F0->type;
struct Cyc_List_List*_tmp5F2=_tmp5F0->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp5F1)) && (
_tmp615->kind == Cyc_Absyn_StructA  && _tmp5EF->tl == 0  || _tmp615->kind == Cyc_Absyn_UnionA)){
# 3918 "toc.cyc"
void*_tmp5F3=Cyc_Tcutil_compress(_tmp5F1);void*_tmp5F4=_tmp5F3;void*_tmp5FC;struct Cyc_Absyn_Tqual _tmp5FB;void*_tmp5FA;unsigned int _tmp5F9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->tag == 4U){_LLA: _tmp5FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).elt_type;_tmp5FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).tq;_tmp5FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zero_term;_tmp5F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zt_loc;_LLB:
# 3921
({void*_tmpA91=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=4U,(_tmp5F5->f1).elt_type=_tmp5FC,(_tmp5F5->f1).tq=_tmp5FB,({struct Cyc_Absyn_Exp*_tmpA90=Cyc_Absyn_uint_exp(0U,0U);(_tmp5F5->f1).num_elts=_tmpA90;}),(_tmp5F5->f1).zero_term=_tmp5FA,(_tmp5F5->f1).zt_loc=_tmp5F9;_tmp5F5;});_tmp5F1=_tmpA91;});
goto _LL9;}else{_LLC: _LLD:
# 3924
({struct Cyc_List_List*_tmpA93=({struct Cyc_List_List*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({void*_tmpA92=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=6U,_tmp5F6->f1=0;_tmp5F6;});_tmp5F7->hd=_tmpA92;}),_tmp5F7->tl=_tmp5F2;_tmp5F7;});_tmp5F2=_tmpA93;});
({void*_tmpA97=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->tag=4U,({void*_tmpA96=Cyc_Absyn_void_star_type();(_tmp5F8->f1).elt_type=_tmpA96;}),({
struct Cyc_Absyn_Tqual _tmpA95=Cyc_Absyn_empty_tqual(0U);(_tmp5F8->f1).tq=_tmpA95;}),({
struct Cyc_Absyn_Exp*_tmpA94=Cyc_Absyn_uint_exp(0U,0U);(_tmp5F8->f1).num_elts=_tmpA94;}),(_tmp5F8->f1).zero_term=Cyc_Absyn_false_type,(_tmp5F8->f1).zt_loc=0U;_tmp5F8;});
# 3925
_tmp5F1=_tmpA97;});}_LL9:;}{
# 3931
struct Cyc_Absyn_Aggrfield*_tmp5FD=({struct Cyc_Absyn_Aggrfield*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->name=_tmp5F0->name,_tmp60F->tq=Cyc_Toc_mt_tq,({
# 3933
void*_tmpA98=Cyc_Toc_typ_to_c(_tmp5F1);_tmp60F->type=_tmpA98;}),_tmp60F->width=_tmp5F0->width,_tmp60F->attributes=_tmp5F2,_tmp60F->requires_clause=0;_tmp60F;});
# 3941
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp615->impl))->tagged){
void*_tmp5FE=_tmp5FD->type;
struct _dyneither_ptr*_tmp5FF=_tmp5FD->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp60C=({struct Cyc_String_pa_PrintArg_struct _tmp732;_tmp732.tag=0U,_tmp732.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp615->name).f2);_tmp732;});struct Cyc_String_pa_PrintArg_struct _tmp60D=({struct Cyc_String_pa_PrintArg_struct _tmp731;_tmp731.tag=0U,_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5FF);_tmp731;});void*_tmp60A[2U];_tmp60A[0]=& _tmp60C,_tmp60A[1]=& _tmp60D;({struct _dyneither_ptr _tmpA99=({const char*_tmp60B="_union_%s_%s";_tag_dyneither(_tmp60B,sizeof(char),13U);});Cyc_aprintf(_tmpA99,_tag_dyneither(_tmp60A,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp609=_cycalloc(sizeof(*_tmp609));*_tmp609=s;_tmp609;});
struct Cyc_Absyn_Aggrfield*_tmp600=({struct Cyc_Absyn_Aggrfield*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->name=Cyc_Toc_val_sp,_tmp608->tq=Cyc_Toc_mt_tq,_tmp608->type=_tmp5FE,_tmp608->width=0,_tmp608->attributes=0,_tmp608->requires_clause=0;_tmp608;});
struct Cyc_Absyn_Aggrfield*_tmp601=({struct Cyc_Absyn_Aggrfield*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->name=Cyc_Toc_tag_sp,_tmp607->tq=Cyc_Toc_mt_tq,_tmp607->type=Cyc_Absyn_sint_type,_tmp607->width=0,_tmp607->attributes=0,_tmp607->requires_clause=0;_tmp607;});
struct Cyc_List_List*_tmp602=({struct Cyc_Absyn_Aggrfield*_tmp606[2U];_tmp606[0]=_tmp601,_tmp606[1]=_tmp600;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp606,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp603=Cyc_Toc_make_c_struct_defn(str,_tmp602);
({struct Cyc_List_List*_tmpA9B=({struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));({struct Cyc_Absyn_Decl*_tmpA9A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->tag=5U,_tmp604->f1=_tmp603;_tmp604;}),0U);_tmp605->hd=_tmpA9A;}),_tmp605->tl=Cyc_Toc_result_decls;_tmp605;});Cyc_Toc_result_decls=_tmpA9B;});
({void*_tmpA9C=Cyc_Absyn_strct(str);_tmp5FD->type=_tmpA9C;});}
# 3953
({struct Cyc_List_List*_tmpA9D=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->hd=_tmp5FD,_tmp60E->tl=new_fields;_tmp60E;});new_fields=_tmpA9D;});};}}
# 3955
({struct Cyc_List_List*_tmpA9E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpA9E;});};}
# 3958
if(!seen_defn_before)
({struct Cyc_List_List*_tmpAA1=({struct Cyc_List_List*_tmp612=_cycalloc(sizeof(*_tmp612));({struct Cyc_Absyn_Decl*_tmpAA0=({struct Cyc_Absyn_Decl*_tmp611=_cycalloc(sizeof(*_tmp611));({void*_tmpA9F=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=5U,_tmp610->f1=new_ad;_tmp610;});_tmp611->r=_tmpA9F;}),_tmp611->loc=0U;_tmp611;});_tmp612->hd=_tmpAA0;}),_tmp612->tl=Cyc_Toc_result_decls;_tmp612;});Cyc_Toc_result_decls=_tmpAA1;});
return 0;};};};}
# 3962
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
((int(*)(struct Cyc_Absyn_Aggrdecl**arg,int(*f)(struct _RegionHandle*,struct _tuple39*)))Cyc_Toc_use_toc_state)(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple40{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 3990 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 3993
struct _tuple40 _tmp617=*env;struct _tuple40 _tmp618=_tmp617;struct Cyc_Set_Set**_tmp629;struct Cyc_Absyn_Datatypedecl*_tmp628;_LL1: _tmp629=(_tmp618.f1)->datatypes_so_far;_tmp628=_tmp618.f2;_LL2:;{
struct _tuple1*_tmp619=_tmp628->name;
if(_tmp628->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp629,_tmp619))
return 0;
({struct Cyc_Set_Set*_tmpAA2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp629,_tmp619);*_tmp629=_tmpAA2;});
{struct Cyc_List_List*_tmp61A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp628->fields))->v;for(0;_tmp61A != 0;_tmp61A=_tmp61A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp61A->hd;
# 4001
struct Cyc_List_List*_tmp61B=0;
int i=1;
{struct Cyc_List_List*_tmp61C=f->typs;for(0;_tmp61C != 0;(_tmp61C=_tmp61C->tl,i ++)){
struct _dyneither_ptr*_tmp61D=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp61E=({struct Cyc_Absyn_Aggrfield*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->name=_tmp61D,_tmp620->tq=(*((struct _tuple12*)_tmp61C->hd)).f1,({
void*_tmpAA3=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp61C->hd)).f2);_tmp620->type=_tmpAA3;}),_tmp620->width=0,_tmp620->attributes=0,_tmp620->requires_clause=0;_tmp620;});
({struct Cyc_List_List*_tmpAA4=({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=_tmp61E,_tmp61F->tl=_tmp61B;_tmp61F;});_tmp61B=_tmpAA4;});}}
# 4009
({struct Cyc_List_List*_tmpAA7=({struct Cyc_List_List*_tmp622=_cycalloc(sizeof(*_tmp622));({struct Cyc_Absyn_Aggrfield*_tmpAA6=({struct Cyc_Absyn_Aggrfield*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->name=Cyc_Toc_tag_sp,_tmp621->tq=Cyc_Toc_mt_tq,_tmp621->type=Cyc_Absyn_sint_type,_tmp621->width=0,_tmp621->attributes=0,_tmp621->requires_clause=0;_tmp621;});_tmp622->hd=_tmpAA6;}),({
struct Cyc_List_List*_tmpAA5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp61B);_tmp622->tl=_tmpAA5;});_tmp622;});
# 4009
_tmp61B=_tmpAA7;});{
# 4011
struct Cyc_Absyn_Aggrdecl*_tmp623=({struct _dyneither_ptr*_tmpAA9=({struct _dyneither_ptr*_tmp627=_cycalloc(sizeof(*_tmp627));({struct _dyneither_ptr _tmpAA8=({const char*_tmp626="";_tag_dyneither(_tmp626,sizeof(char),1U);});*_tmp627=_tmpAA8;});_tmp627;});Cyc_Toc_make_c_struct_defn(_tmpAA9,_tmp61B);});
({struct _tuple1*_tmpAAA=Cyc_Toc_collapse_qvars(f->name,_tmp628->name);_tmp623->name=_tmpAAA;});
({struct Cyc_List_List*_tmpAAC=({struct Cyc_List_List*_tmp625=_cycalloc(sizeof(*_tmp625));({struct Cyc_Absyn_Decl*_tmpAAB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=5U,_tmp624->f1=_tmp623;_tmp624;}),0U);_tmp625->hd=_tmpAAB;}),_tmp625->tl=Cyc_Toc_result_decls;_tmp625;});Cyc_Toc_result_decls=_tmpAAC;});};}}
# 4015
return 0;};}
# 4018
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4037 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4040
struct _tuple40 _tmp62A=*env;struct _tuple40 _tmp62B=_tmp62A;struct Cyc_Toc_TocState*_tmp64D;struct Cyc_Absyn_Datatypedecl*_tmp64C;_LL1: _tmp64D=_tmp62B.f1;_tmp64C=_tmp62B.f2;_LL2:;
if(_tmp64C->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp62C=*_tmp64D;struct Cyc_Toc_TocState _tmp62D=_tmp62C;struct Cyc_Dict_Dict*_tmp64B;_LL4: _tmp64B=_tmp62D.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp62E=_tmp64C->name;
{struct Cyc_List_List*_tmp62F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp64C->fields))->v;for(0;_tmp62F != 0;_tmp62F=_tmp62F->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp62F->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp630=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp631=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,_tmp630,Cyc_Absyn_false_type,0U);
# 4051
int*_tmp632=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp64B,f->name);int*_tmp633=_tmp632;if(_tmp633 == 0){_LL7: _LL8: {
# 4053
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpAAD;});{
# 4057
struct Cyc_Absyn_Vardecl*_tmp634=Cyc_Absyn_new_vardecl(0U,f->name,_tmp631,initopt);
_tmp634->sc=f->sc;
({struct Cyc_List_List*_tmpAAF=({struct Cyc_List_List*_tmp636=_cycalloc(sizeof(*_tmp636));({struct Cyc_Absyn_Decl*_tmpAAE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->tag=0U,_tmp635->f1=_tmp634;_tmp635;}),0U);_tmp636->hd=_tmpAAE;}),_tmp636->tl=Cyc_Toc_result_decls;_tmp636;});Cyc_Toc_result_decls=_tmpAAF;});
({struct Cyc_Dict_Dict _tmpAB0=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp64B,f->name,f->sc != Cyc_Absyn_Extern);
# 4060
*_tmp64B=_tmpAB0;});{
# 4062
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp637=f->typs;for(0;_tmp637 != 0;(_tmp637=_tmp637->tl,i ++)){
struct _dyneither_ptr*_tmp638=({struct _dyneither_ptr*_tmp63F=_cycalloc(sizeof(*_tmp63F));({struct _dyneither_ptr _tmpAB2=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp63E=({struct Cyc_Int_pa_PrintArg_struct _tmp733;_tmp733.tag=1U,_tmp733.f1=(unsigned long)i;_tmp733;});void*_tmp63C[1U];_tmp63C[0]=& _tmp63E;({struct _dyneither_ptr _tmpAB1=({const char*_tmp63D="f%d";_tag_dyneither(_tmp63D,sizeof(char),4U);});Cyc_aprintf(_tmpAB1,_tag_dyneither(_tmp63C,sizeof(void*),1U));});});*_tmp63F=_tmpAB2;});_tmp63F;});
struct Cyc_Absyn_Aggrfield*_tmp639=({struct Cyc_Absyn_Aggrfield*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->name=_tmp638,_tmp63B->tq=(*((struct _tuple12*)_tmp637->hd)).f1,({
void*_tmpAB3=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp637->hd)).f2);_tmp63B->type=_tmpAB3;}),_tmp63B->width=0,_tmp63B->attributes=0,_tmp63B->requires_clause=0;_tmp63B;});
({struct Cyc_List_List*_tmpAB4=({struct Cyc_List_List*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->hd=_tmp639,_tmp63A->tl=fields;_tmp63A;});fields=_tmpAB4;});}}
# 4070
({struct Cyc_List_List*_tmpAB8=({struct Cyc_List_List*_tmp641=_cycalloc(sizeof(*_tmp641));({struct Cyc_Absyn_Aggrfield*_tmpAB7=({struct Cyc_Absyn_Aggrfield*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->name=Cyc_Toc_tag_sp,_tmp640->tq=Cyc_Toc_mt_tq,({
void*_tmpAB6=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_tmp640->type=_tmpAB6;}),_tmp640->width=0,_tmp640->attributes=0,_tmp640->requires_clause=0;_tmp640;});
# 4070
_tmp641->hd=_tmpAB7;}),({
# 4072
struct Cyc_List_List*_tmpAB5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp641->tl=_tmpAB5;});_tmp641;});
# 4070
fields=_tmpAB8;});{
# 4073
struct Cyc_Absyn_Aggrdecl*_tmp642=({struct _dyneither_ptr*_tmpABA=({struct _dyneither_ptr*_tmp646=_cycalloc(sizeof(*_tmp646));({struct _dyneither_ptr _tmpAB9=({const char*_tmp645="";_tag_dyneither(_tmp645,sizeof(char),1U);});*_tmp646=_tmpAB9;});_tmp646;});Cyc_Toc_make_c_struct_defn(_tmpABA,fields);});
({struct _tuple1*_tmpABB=Cyc_Toc_collapse_qvars(f->name,_tmp64C->name);_tmp642->name=_tmpABB;});
({struct Cyc_List_List*_tmpABD=({struct Cyc_List_List*_tmp644=_cycalloc(sizeof(*_tmp644));({struct Cyc_Absyn_Decl*_tmpABC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->tag=5U,_tmp643->f1=_tmp642;_tmp643;}),0U);_tmp644->hd=_tmpABC;}),_tmp644->tl=Cyc_Toc_result_decls;_tmp644;});Cyc_Toc_result_decls=_tmpABD;});
goto _LL6;};};};}}else{if(*((int*)_tmp633)== 0){_LL9: _LLA:
# 4078
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp647=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp648=Cyc_Absyn_new_vardecl(0U,f->name,_tmp631,_tmp647);
_tmp648->sc=f->sc;
({struct Cyc_List_List*_tmpABF=({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_Absyn_Decl*_tmpABE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->tag=0U,_tmp649->f1=_tmp648;_tmp649;}),0U);_tmp64A->hd=_tmpABE;}),_tmp64A->tl=Cyc_Toc_result_decls;_tmp64A;});Cyc_Toc_result_decls=_tmpABF;});
({struct Cyc_Dict_Dict _tmpAC0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp64B,f->name,1);*_tmp64B=_tmpAC0;});}
# 4085
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4089
return 0;};};}
# 4092
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4096
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4102
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpAC1=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpAC1;});
# 4106
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_type(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4115
struct _RegionHandle _tmp64E=_new_region("temp");struct _RegionHandle*temp=& _tmp64E;_push_region(temp);
{struct Cyc_Toc_Env*_tmp64F=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp64F,init);}
# 4116
;_pop_region(temp);}else{
# 4120
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4123
void*_tmp650=Cyc_Tcutil_compress(old_typ);void*_tmp651=_tmp650;void*_tmp658;struct Cyc_Absyn_Exp*_tmp657;void*_tmp656;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->tag == 4U){_LL1: _tmp658=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->f1).elt_type;_tmp657=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->f1).num_elts;_tmp656=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp651)->f1).zero_term;_LL2:
# 4125
 if(Cyc_Tcutil_force_type2bool(0,_tmp656)){
if(_tmp657 == 0)
({void*_tmp652=0U;({struct _dyneither_ptr _tmpAC2=({const char*_tmp653="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp653,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAC2,_tag_dyneither(_tmp652,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp657;
struct Cyc_Absyn_Exp*_tmp654=({struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpAC4,({
struct Cyc_Absyn_Exp*_tmpAC3=num_elts;Cyc_Absyn_add_exp(_tmpAC3,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4132
struct Cyc_Absyn_Exp*_tmp655=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpAC6=({struct Cyc_Absyn_Stmt*_tmpAC5=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp654,_tmp655,0U),0U);Cyc_Toc_seq_stmt_r(_tmpAC5,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4133
s->r=_tmpAC6;});};}
# 4136
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4144
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp659=d;struct Cyc_List_List*_tmp660;struct Cyc_List_List*_tmp65F;void**_tmp65E;struct Cyc_Tcpat_Rhs*_tmp65D;switch(*((int*)_tmp659)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp65D=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp659)->f1;_LL4:
 _tmp65D->rhs=success;return d;default: _LL5: _tmp660=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp659)->f1;_tmp65F=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp659)->f2;_tmp65E=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp659)->f3;_LL6:
# 4149
({void*_tmpAC7=Cyc_Toc_rewrite_decision(*_tmp65E,success);*_tmp65E=_tmpAC7;});
for(0;_tmp65F != 0;_tmp65F=_tmp65F->tl){
struct _tuple32*_tmp65A=(struct _tuple32*)_tmp65F->hd;struct _tuple32*_tmp65B=_tmp65A;void**_tmp65C;_LL8: _tmp65C=(void**)& _tmp65B->f2;_LL9:;
({void*_tmpAC8=Cyc_Toc_rewrite_decision(*_tmp65C,success);*_tmp65C=_tmpAC8;});}
# 4154
return d;}_LL0:;}
# 4165 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4167
struct _RegionHandle _tmp661=_new_region("rgn");struct _RegionHandle*rgn=& _tmp661;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp662=Cyc_Toc_share_env(rgn,nv);
void*_tmp663=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp662,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4177
struct Cyc_Absyn_Stmt*_tmp664=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4181
({void*_tmpAC9=Cyc_Toc_rewrite_decision(dopt,_tmp664);dopt=_tmpAC9;});{
# 4183
struct Cyc_Absyn_Switch_clause*_tmp665=({struct Cyc_Absyn_Switch_clause*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->pattern=p,_tmp671->pat_vars=0,_tmp671->where_clause=0,_tmp671->body=_tmp664,_tmp671->loc=0U;_tmp671;});
struct Cyc_List_List*_tmp666=({struct _RegionHandle*_tmpACC=rgn;struct _RegionHandle*_tmpACB=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpACC,Cyc_Toc_gen_labels,_tmpACB,({struct Cyc_Absyn_Switch_clause*_tmp670[1U];_tmp670[0]=_tmp665;({struct _RegionHandle*_tmpACA=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpACA,_tag_dyneither(_tmp670,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4186
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4190
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp662,& mydecls,& mybodies,dopt,_tmp666,v);
# 4193
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp667=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp668=_tmp667;struct Cyc_Toc_Env*_tmp66A;struct Cyc_Absyn_Stmt*_tmp669;_LL1: _tmp66A=_tmp668->f1;_tmp669=_tmp668->f3;_LL2:;
if(_tmp669 == _tmp664){senv=_tmp66A;goto FOUND_ENV;}}
# 4198
({void*_tmp66B=0U;({struct _dyneither_ptr _tmpACD=({const char*_tmp66C="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp66C,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpACD,_tag_dyneither(_tmp66B,sizeof(void*),0U));});});
FOUND_ENV:
# 4202
 Cyc_Toc_stmt_to_c(senv,s);{
# 4204
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4206
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp66D=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpACE=Cyc_Toc_temp_var();_tmp66D->name=_tmpACE;});
({void*_tmpACF=Cyc_Toc_typ_to_c_array(_tmp66D->type);_tmp66D->type=_tmpACF;});
({struct Cyc_Absyn_Stmt*_tmpAD1=({struct Cyc_Absyn_Decl*_tmpAD0=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->tag=0U,_tmp66E->f1=_tmp66D;_tmp66E;}),0U);Cyc_Absyn_decl_stmt(_tmpAD0,res,0U);});res=_tmpAD1;});}
# 4213
({struct Cyc_Absyn_Stmt*_tmpAD5=({struct _tuple1*_tmpAD4=v;void*_tmpAD3=Cyc_Toc_typ_to_c(_tmp663);struct Cyc_Absyn_Exp*_tmpAD2=e;Cyc_Absyn_declare_stmt(_tmpAD4,_tmpAD3,_tmpAD2,res,0U);});res=_tmpAD5;});{
struct Cyc_Absyn_Stmt*_tmp66F=res;_npop_handler(0U);return _tmp66F;};};};};}
# 4168
;_pop_region(rgn);}
# 4221
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp672=e->r;void*_tmp673=_tmp672;struct Cyc_Absyn_MallocInfo*_tmp69E;struct Cyc_Absyn_Stmt*_tmp69D;void**_tmp69C;void**_tmp69B;struct Cyc_List_List*_tmp69A;struct Cyc_List_List*_tmp699;void**_tmp698;struct Cyc_List_List*_tmp697;void**_tmp696;struct Cyc_Absyn_Exp*_tmp695;struct Cyc_Absyn_Exp*_tmp694;struct Cyc_List_List*_tmp693;struct Cyc_Absyn_Exp*_tmp692;struct Cyc_Absyn_Exp*_tmp691;struct Cyc_Absyn_Exp*_tmp690;struct Cyc_Absyn_Exp*_tmp68F;struct Cyc_Absyn_Exp*_tmp68E;struct Cyc_Absyn_Exp*_tmp68D;struct Cyc_Absyn_Exp*_tmp68C;struct Cyc_Absyn_Exp*_tmp68B;struct Cyc_Absyn_Exp*_tmp68A;struct Cyc_Absyn_Exp*_tmp689;struct Cyc_Absyn_Exp*_tmp688;struct Cyc_Absyn_Exp*_tmp687;struct Cyc_Absyn_Exp*_tmp686;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_List_List*_tmp683;struct Cyc_Absyn_Exp*_tmp682;struct Cyc_Absyn_Exp*_tmp681;struct Cyc_Absyn_Exp*_tmp680;struct Cyc_Absyn_Exp*_tmp67F;struct Cyc_Absyn_Exp*_tmp67E;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67B;struct Cyc_Absyn_Exp*_tmp67A;switch(*((int*)_tmp673)){case 41U: _LL1: _tmp67A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL2:
 _tmp67B=_tmp67A;goto _LL4;case 20U: _LL3: _tmp67B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL4:
 _tmp67C=_tmp67B;goto _LL6;case 21U: _LL5: _tmp67C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL6:
 _tmp67D=_tmp67C;goto _LL8;case 22U: _LL7: _tmp67D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL8:
 _tmp67E=_tmp67D;goto _LLA;case 15U: _LL9: _tmp67E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LLA:
 _tmp67F=_tmp67E;goto _LLC;case 11U: _LLB: _tmp67F=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LLC:
 _tmp680=_tmp67F;goto _LLE;case 12U: _LLD: _tmp680=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LLE:
 _tmp681=_tmp680;goto _LL10;case 18U: _LLF: _tmp681=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL10:
 _tmp682=_tmp681;goto _LL12;case 5U: _LL11: _tmp682=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL12:
 Cyc_Toc_exptypes_to_c(_tmp682);goto _LL0;case 3U: _LL13: _tmp683=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL14:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp683);goto _LL0;case 7U: _LL15: _tmp685=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp684=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL16:
 _tmp687=_tmp685;_tmp686=_tmp684;goto _LL18;case 8U: _LL17: _tmp687=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp686=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL18:
 _tmp689=_tmp687;_tmp688=_tmp686;goto _LL1A;case 9U: _LL19: _tmp689=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp688=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL1A:
 _tmp68B=_tmp689;_tmp68A=_tmp688;goto _LL1C;case 23U: _LL1B: _tmp68B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp68A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL1C:
 _tmp68D=_tmp68B;_tmp68C=_tmp68A;goto _LL1E;case 35U: _LL1D: _tmp68D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp68C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL1E:
 _tmp68F=_tmp68D;_tmp68E=_tmp68C;goto _LL20;case 4U: _LL1F: _tmp68F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp68E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp673)->f3;_LL20:
 Cyc_Toc_exptypes_to_c(_tmp68F);Cyc_Toc_exptypes_to_c(_tmp68E);goto _LL0;case 6U: _LL21: _tmp692=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp691=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_tmp690=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp673)->f3;_LL22:
# 4240
 Cyc_Toc_exptypes_to_c(_tmp692);Cyc_Toc_exptypes_to_c(_tmp691);Cyc_Toc_exptypes_to_c(_tmp690);goto _LL0;case 10U: _LL23: _tmp694=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp693=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL24:
# 4242
 Cyc_Toc_exptypes_to_c(_tmp694);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp693);goto _LL0;case 14U: _LL25: _tmp696=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_tmp695=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL26:
({void*_tmpAD6=Cyc_Toc_typ_to_c(*_tmp696);*_tmp696=_tmpAD6;});Cyc_Toc_exptypes_to_c(_tmp695);goto _LL0;case 25U: _LL27: _tmp698=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp673)->f1)->f3;_tmp697=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL28:
# 4245
({void*_tmpAD7=Cyc_Toc_typ_to_c(*_tmp698);*_tmp698=_tmpAD7;});
_tmp699=_tmp697;goto _LL2A;case 36U: _LL29: _tmp699=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp673)->f2;_LL2A:
 _tmp69A=_tmp699;goto _LL2C;case 26U: _LL2B: _tmp69A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL2C:
# 4249
 for(0;_tmp69A != 0;_tmp69A=_tmp69A->tl){
struct _tuple19 _tmp674=*((struct _tuple19*)_tmp69A->hd);struct _tuple19 _tmp675=_tmp674;struct Cyc_Absyn_Exp*_tmp676;_LL56: _tmp676=_tmp675.f2;_LL57:;
Cyc_Toc_exptypes_to_c(_tmp676);}
# 4253
goto _LL0;case 19U: _LL2D: _tmp69B=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL2E:
 _tmp69C=_tmp69B;goto _LL30;case 17U: _LL2F: _tmp69C=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL30:
({void*_tmpAD8=Cyc_Toc_typ_to_c(*_tmp69C);*_tmp69C=_tmpAD8;});goto _LL0;case 37U: _LL31: _tmp69D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL32:
 Cyc_Toc_stmttypes_to_c(_tmp69D);goto _LL0;case 34U: _LL33: _tmp69E=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp673)->f1;_LL34:
# 4258
 if(_tmp69E->elt_type != 0)
({void**_tmpADA=({void**_tmp677=_cycalloc(sizeof(*_tmp677));({void*_tmpAD9=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp69E->elt_type)));*_tmp677=_tmpAD9;});_tmp677;});_tmp69E->elt_type=_tmpADA;});
Cyc_Toc_exptypes_to_c(_tmp69E->num_elts);
goto _LL0;case 0U: _LL35: _LL36:
 goto _LL38;case 1U: _LL37: _LL38:
 goto _LL3A;case 32U: _LL39: _LL3A:
 goto _LL3C;case 40U: _LL3B: _LL3C:
 goto _LL3E;case 33U: _LL3D: _LL3E:
 goto _LL0;case 2U: _LL3F: _LL40:
# 4268
 goto _LL42;case 30U: _LL41: _LL42:
 goto _LL44;case 31U: _LL43: _LL44:
 goto _LL46;case 29U: _LL45: _LL46:
 goto _LL48;case 27U: _LL47: _LL48:
 goto _LL4A;case 28U: _LL49: _LL4A:
 goto _LL4C;case 24U: _LL4B: _LL4C:
 goto _LL4E;case 13U: _LL4D: _LL4E:
 goto _LL50;case 16U: _LL4F: _LL50:
 goto _LL52;case 39U: _LL51: _LL52:
 goto _LL54;default: _LL53: _LL54:
# 4279
({void*_tmp678=0U;({unsigned int _tmpADC=e->loc;struct _dyneither_ptr _tmpADB=({const char*_tmp679="Cyclone expression within C code";_tag_dyneither(_tmp679,sizeof(char),33U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpADC,_tmpADB,_tag_dyneither(_tmp678,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4283
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp69F=d->r;void*_tmp6A0=_tmp69F;struct Cyc_Absyn_Typedefdecl*_tmp6AA;struct Cyc_Absyn_Enumdecl*_tmp6A9;struct Cyc_Absyn_Aggrdecl*_tmp6A8;struct Cyc_Absyn_Fndecl*_tmp6A7;struct Cyc_Absyn_Vardecl*_tmp6A6;switch(*((int*)_tmp6A0)){case 0U: _LL1: _tmp6A6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL2:
# 4286
({void*_tmpADD=Cyc_Toc_typ_to_c(_tmp6A6->type);_tmp6A6->type=_tmpADD;});
if(_tmp6A6->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6A6->initializer));
goto _LL0;case 1U: _LL3: _tmp6A7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL4:
# 4290
({void*_tmpADE=Cyc_Toc_typ_to_c(_tmp6A7->ret_type);_tmp6A7->ret_type=_tmpADE;});
{struct Cyc_List_List*_tmp6A1=_tmp6A7->args;for(0;_tmp6A1 != 0;_tmp6A1=_tmp6A1->tl){
({void*_tmpADF=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp6A1->hd)).f3);(*((struct _tuple9*)_tmp6A1->hd)).f3=_tmpADF;});}}
# 4294
goto _LL0;case 5U: _LL5: _tmp6A8=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp6A8);goto _LL0;case 7U: _LL7: _tmp6A9=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LL8:
# 4297
 if(_tmp6A9->fields != 0){
struct Cyc_List_List*_tmp6A2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6A9->fields))->v;for(0;_tmp6A2 != 0;_tmp6A2=_tmp6A2->tl){
struct Cyc_Absyn_Enumfield*_tmp6A3=(struct Cyc_Absyn_Enumfield*)_tmp6A2->hd;
if(_tmp6A3->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6A3->tag));}}
# 4302
goto _LL0;case 8U: _LL9: _tmp6AA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6A0)->f1;_LLA:
({void*_tmpAE0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6AA->defn));_tmp6AA->defn=_tmpAE0;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4312
({void*_tmp6A4=0U;({unsigned int _tmpAE2=d->loc;struct _dyneither_ptr _tmpAE1=({const char*_tmp6A5="Cyclone declaration within C code";_tag_dyneither(_tmp6A5,sizeof(char),34U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAE2,_tmpAE1,_tag_dyneither(_tmp6A4,sizeof(void*),0U));});});goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
 goto _LL0;}_LL0:;}
# 4318
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp6AB=s->r;void*_tmp6AC=_tmp6AB;struct Cyc_Absyn_Stmt*_tmp6C4;struct Cyc_Absyn_Stmt*_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C2;struct Cyc_Absyn_Decl*_tmp6C1;struct Cyc_Absyn_Stmt*_tmp6C0;struct Cyc_Absyn_Exp*_tmp6BF;struct Cyc_List_List*_tmp6BE;void*_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BB;struct Cyc_Absyn_Exp*_tmp6BA;struct Cyc_Absyn_Stmt*_tmp6B9;struct Cyc_Absyn_Exp*_tmp6B8;struct Cyc_Absyn_Stmt*_tmp6B7;struct Cyc_Absyn_Exp*_tmp6B6;struct Cyc_Absyn_Stmt*_tmp6B5;struct Cyc_Absyn_Stmt*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Stmt*_tmp6B2;struct Cyc_Absyn_Stmt*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B0;switch(*((int*)_tmp6AC)){case 1U: _LL1: _tmp6B0=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp6B0);goto _LL0;case 2U: _LL3: _tmp6B2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6B1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp6B2);Cyc_Toc_stmttypes_to_c(_tmp6B1);goto _LL0;case 3U: _LL5: _tmp6B3=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_LL6:
 if(_tmp6B3 != 0)Cyc_Toc_exptypes_to_c(_tmp6B3);goto _LL0;case 4U: _LL7: _tmp6B6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6B5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_tmp6B4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f3;_LL8:
# 4324
 Cyc_Toc_exptypes_to_c(_tmp6B6);Cyc_Toc_stmttypes_to_c(_tmp6B5);Cyc_Toc_stmttypes_to_c(_tmp6B4);goto _LL0;case 5U: _LL9: _tmp6B8=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1).f1;_tmp6B7=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LLA:
# 4326
 Cyc_Toc_exptypes_to_c(_tmp6B8);Cyc_Toc_stmttypes_to_c(_tmp6B7);goto _LL0;case 9U: _LLB: _tmp6BC=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6BB=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2).f1;_tmp6BA=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f3).f1;_tmp6B9=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f4;_LLC:
# 4328
 Cyc_Toc_exptypes_to_c(_tmp6BC);Cyc_Toc_exptypes_to_c(_tmp6BB);Cyc_Toc_exptypes_to_c(_tmp6BA);
Cyc_Toc_stmttypes_to_c(_tmp6B9);goto _LL0;case 10U: _LLD: _tmp6BF=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6BE=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_tmp6BD=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f3;_LLE:
# 4331
 Cyc_Toc_exptypes_to_c(_tmp6BF);
for(0;_tmp6BE != 0;_tmp6BE=_tmp6BE->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp6BE->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp6C1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6C0=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp6C1);Cyc_Toc_stmttypes_to_c(_tmp6C0);goto _LL0;case 14U: _LL11: _tmp6C3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f1;_tmp6C2=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp6C3);Cyc_Toc_exptypes_to_c(_tmp6C2);goto _LL0;case 13U: _LL13: _tmp6C4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp6AC)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp6C4);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4341
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4344
({void*_tmpAE3=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->tag=0U;_tmp6AD;});s->r=_tmpAE3;});
goto _LL0;default: _LL1F: _LL20:
# 4347
({void*_tmp6AE=0U;({unsigned int _tmpAE5=s->loc;struct _dyneither_ptr _tmpAE4=({const char*_tmp6AF="Cyclone statement in C code";_tag_dyneither(_tmp6AF,sizeof(char),28U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos_loc)(_tmpAE5,_tmpAE4,_tag_dyneither(_tmp6AE,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4351
static void*Cyc_Toc_decl_to_enumtype(struct Cyc_Absyn_Decl*d){
void*_tmp6C5=d->r;void*_tmp6C6=_tmp6C5;struct Cyc_Absyn_Enumdecl*_tmp6CB;if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C6)->tag == 7U){_LL1: _tmp6CB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6C6)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8->tag=0U,({void*_tmpAE6=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->tag=15U,_tmp6C7->f1=_tmp6CB->name,_tmp6C7->f2=_tmp6CB;_tmp6C7;});_tmp6C8->f1=_tmpAE6;}),_tmp6C8->f2=0;_tmp6C8;});}else{_LL3: _LL4:
({void*_tmp6C9=0U;({struct _dyneither_ptr _tmpAE7=({const char*_tmp6CA="Toc::get_enumdecl_name";_tag_dyneither(_tmp6CA,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE7,_tag_dyneither(_tmp6C9,sizeof(void*),0U));});});}_LL0:;}
# 4358
static struct Cyc_Absyn_Decl*Cyc_Toc_type2enumdecl(void*t){
void*_tmp6CC=t;struct Cyc_Absyn_Enumdecl*_tmp6D0;unsigned int _tmp6CF;if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->tag == 10U){if(((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->f1)->r)->tag == 1U){_LL1: _tmp6D0=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->f1)->r)->f1;_tmp6CF=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6CC)->f1)->loc;_LL2:
# 4361
 return({struct Cyc_Absyn_Decl*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));({void*_tmpAE8=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->tag=7U,_tmp6CD->f1=_tmp6D0;_tmp6CD;});_tmp6CE->r=_tmpAE8;}),_tmp6CE->loc=_tmp6CF;_tmp6CE;});}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 4369
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp6D1=0U;({struct _dyneither_ptr _tmpAE9=({const char*_tmp6D2="decls_to_c: not at toplevel!";_tag_dyneither(_tmp6D2,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE9,_tag_dyneither(_tmp6D1,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp6D3=d->r;void*_tmp6D4=_tmp6D3;struct Cyc_List_List*_tmp6EF;struct Cyc_List_List*_tmp6EE;struct Cyc_List_List*_tmp6ED;struct Cyc_List_List*_tmp6EC;struct Cyc_Absyn_Typedefdecl*_tmp6EB;struct Cyc_Absyn_Enumdecl*_tmp6EA;struct Cyc_Absyn_Datatypedecl*_tmp6E9;struct Cyc_Absyn_Aggrdecl*_tmp6E8;struct Cyc_Absyn_Fndecl*_tmp6E7;struct Cyc_Absyn_Vardecl*_tmp6E6;switch(*((int*)_tmp6D4)){case 0U: _LL1: _tmp6E6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL2: {
# 4377
struct _tuple1*_tmp6D5=_tmp6E6->name;
# 4379
if(_tmp6E6->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpAEB=({struct _tuple1*_tmp6D6=_cycalloc(sizeof(*_tmp6D6));({union Cyc_Absyn_Nmspace _tmpAEA=Cyc_Absyn_Abs_n(0,1);_tmp6D6->f1=_tmpAEA;}),_tmp6D6->f2=(*_tmp6D5).f2;_tmp6D6;});_tmp6D5=_tmpAEB;});
if(_tmp6E6->initializer != 0){
if(_tmp6E6->sc == Cyc_Absyn_ExternC)_tmp6E6->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp6E6->initializer));else{
# 4386
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp6E6->initializer));}}
# 4388
_tmp6E6->name=_tmp6D5;
({enum Cyc_Absyn_Scope _tmpAEC=Cyc_Toc_scope_to_c(_tmp6E6->sc);_tmp6E6->sc=_tmpAEC;});
({void*_tmpAED=Cyc_Toc_typ_to_c(_tmp6E6->type);_tmp6E6->type=_tmpAED;});
({struct Cyc_List_List*_tmpAEE=({struct Cyc_List_List*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=d,_tmp6D7->tl=Cyc_Toc_result_decls;_tmp6D7;});Cyc_Toc_result_decls=_tmpAEE;});
goto _LL0;}case 1U: _LL3: _tmp6E7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL4: {
# 4394
struct _tuple1*_tmp6D8=_tmp6E7->name;
# 4396
if(_tmp6E7->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpAF0=({struct _tuple1*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));({union Cyc_Absyn_Nmspace _tmpAEF=Cyc_Absyn_Abs_n(0,1);_tmp6D9->f1=_tmpAEF;}),_tmp6D9->f2=(*_tmp6D8).f2;_tmp6D9;});_tmp6D8=_tmpAF0;});
_tmp6E7->sc=Cyc_Absyn_Public;}
# 4400
_tmp6E7->name=_tmp6D8;
Cyc_Toc_fndecl_to_c(nv,_tmp6E7,cinclude);
({struct Cyc_List_List*_tmpAF1=({struct Cyc_List_List*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA->hd=d,_tmp6DA->tl=Cyc_Toc_result_decls;_tmp6DA;});Cyc_Toc_result_decls=_tmpAF1;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp6DB=0U;({struct _dyneither_ptr _tmpAF2=({const char*_tmp6DC="letdecl at toplevel";_tag_dyneither(_tmp6DC,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAF2,_tag_dyneither(_tmp6DB,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp6DD=0U;({struct _dyneither_ptr _tmpAF3=({const char*_tmp6DE="region decl at toplevel";_tag_dyneither(_tmp6DE,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAF3,_tag_dyneither(_tmp6DD,sizeof(void*),0U));});});case 5U: _LLB: _tmp6E8=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LLC:
# 4408
 Cyc_Toc_aggrdecl_to_c(_tmp6E8);
goto _LL0;case 6U: _LLD: _tmp6E9=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LLE:
# 4411
 if(_tmp6E9->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp6E9);else{
# 4414
Cyc_Toc_datatypedecl_to_c(_tmp6E9);}
goto _LL0;case 7U: _LLF: _tmp6EA=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL10:
# 4417
 Cyc_Toc_enumdecl_to_c(_tmp6EA);
({struct Cyc_List_List*_tmpAF4=({struct Cyc_List_List*_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->hd=d,_tmp6DF->tl=Cyc_Toc_result_decls;_tmp6DF;});Cyc_Toc_result_decls=_tmpAF4;});
goto _LL0;case 8U: _LL11: _tmp6EB=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL12:
# 4421
 _tmp6EB->tvs=0;
if(_tmp6EB->defn != 0){
void*_tmp6E0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6EB->defn));
_tmp6EB->defn=_tmp6E0;{
# 4427
struct Cyc_Absyn_Decl*_tmp6E1=Cyc_Toc_type2enumdecl(_tmp6E0);
if(_tmp6E1 != 0){
({struct Cyc_List_List*_tmpAF5=({struct Cyc_List_List*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2->hd=_tmp6E1,_tmp6E2->tl=Cyc_Toc_result_decls;_tmp6E2;});Cyc_Toc_result_decls=_tmpAF5;});
({void*_tmpAF6=Cyc_Toc_decl_to_enumtype(_tmp6E1);_tmp6EB->defn=_tmpAF6;});}};}else{
# 4433
enum Cyc_Absyn_KindQual _tmp6E3=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp6EB->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp6E4=_tmp6E3;if(_tmp6E4 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpAF7=Cyc_Absyn_void_star_type();_tmp6EB->defn=_tmpAF7;});goto _LL1F;}else{_LL22: _LL23:
 _tmp6EB->defn=Cyc_Absyn_void_type;goto _LL1F;}_LL1F:;}
# 4441
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpAF8=({struct Cyc_List_List*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5->hd=d,_tmp6E5->tl=Cyc_Toc_result_decls;_tmp6E5;});Cyc_Toc_result_decls=_tmpAF8;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp6EC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp6D4)->f2;_LL18:
 _tmp6ED=_tmp6EC;goto _LL1A;case 10U: _LL19: _tmp6ED=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp6D4)->f2;_LL1A:
 _tmp6EE=_tmp6ED;goto _LL1C;case 11U: _LL1B: _tmp6EE=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpAF9=Cyc_Toc_decls_to_c(r,nv,_tmp6EE,top,cinclude);nv=_tmpAF9;});goto _LL0;default: _LL1D: _tmp6EF=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp6D4)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpAFA=Cyc_Toc_decls_to_c(r,nv,_tmp6EF,top,1);nv=_tmpAFA;});goto _LL0;}_LL0:;};}
# 4452
return nv;}
# 4456
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp6F0=Cyc_Core__new_rckey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp6F1=_tmp6F0;struct Cyc_Core_DynamicRegion*_tmp6F6;_LL1: _tmp6F6=_tmp6F1.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp6F6->h;
({struct Cyc_Toc_TocState*_tmpAFB=Cyc_Toc_empty_toc_state(h);ts=_tmpAFB;});;}
({struct Cyc_Toc_TocStateWrap*_tmpAFC=({struct Cyc_Toc_TocStateWrap*_tmp6F3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6F3));_tmp6F3->dyn=_tmp6F6,_tmp6F3->state=ts;_tmp6F3;});Cyc_Toc_toc_state=_tmpAFC;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
({struct _dyneither_ptr _tmpAFD=_tag_dyneither(({unsigned int _tmp6F5=35;struct _dyneither_ptr**_tmp6F4=_cycalloc(_check_times(_tmp6F5,sizeof(struct _dyneither_ptr*)));_tmp6F4[0]=& Cyc_Toc__throw_str,_tmp6F4[1]=& Cyc_Toc_setjmp_str,_tmp6F4[2]=& Cyc_Toc__push_handler_str,_tmp6F4[3]=& Cyc_Toc__pop_handler_str,_tmp6F4[4]=& Cyc_Toc__exn_thrown_str,_tmp6F4[5]=& Cyc_Toc__npop_handler_str,_tmp6F4[6]=& Cyc_Toc__check_null_str,_tmp6F4[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp6F4[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp6F4[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp6F4[10]=& Cyc_Toc__dyneither_ptr_str,_tmp6F4[11]=& Cyc_Toc__tag_dyneither_str,_tmp6F4[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp6F4[13]=& Cyc_Toc__get_dyneither_size_str,_tmp6F4[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp6F4[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp6F4[16]=& Cyc_Toc__zero_arr_plus_str,_tmp6F4[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp6F4[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp6F4[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp6F4[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp6F4[21]=& Cyc_Toc__cycalloc_str,_tmp6F4[22]=& Cyc_Toc__cyccalloc_str,_tmp6F4[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp6F4[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp6F4[25]=& Cyc_Toc__region_malloc_str,_tmp6F4[26]=& Cyc_Toc__region_calloc_str,_tmp6F4[27]=& Cyc_Toc__check_times_str,_tmp6F4[28]=& Cyc_Toc__new_region_str,_tmp6F4[29]=& Cyc_Toc__push_region_str,_tmp6F4[30]=& Cyc_Toc__pop_region_str,_tmp6F4[31]=& Cyc_Toc__throw_arraybounds_str,_tmp6F4[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp6F4[33]=& Cyc_Toc__throw_match_str,_tmp6F4[34]=& Cyc_Toc__fast_region_malloc_str;_tmp6F4;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpAFD;});};}
# 4505
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp6F7=ts;struct Cyc_Toc_TocStateWrap*_tmp6F8=Cyc_Toc_toc_state;ts=_tmp6F8;Cyc_Toc_toc_state=_tmp6F7;});{
struct Cyc_Toc_TocStateWrap _tmp6F9=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp6FA=_tmp6F9;struct Cyc_Core_DynamicRegion*_tmp700;struct Cyc_Toc_TocState*_tmp6FF;_LL1: _tmp700=_tmp6FA.dyn;_tmp6FF=_tmp6FA.state;_LL2:;
# 4510
{struct _RegionHandle*h=& _tmp700->h;
{struct Cyc_Toc_TocState _tmp6FC=*_tmp6FF;struct Cyc_Toc_TocState _tmp6FD=_tmp6FC;struct Cyc_Xarray_Xarray*_tmp6FE;_LL4: _tmp6FE=_tmp6FD.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp6FE);}
# 4511
;}
# 4514
Cyc_Core_free_rckey(_tmp700);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4517
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4523
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4525
({struct Cyc_Hashtable_Table**_tmpAFE=({struct Cyc_Hashtable_Table**_tmp701=_cycalloc(sizeof(*_tmp701));*_tmp701=pop_tables;_tmp701;});Cyc_Toc_gpop_tables=_tmpAFE;});
Cyc_Toc_init();
{struct _RegionHandle _tmp702=_new_region("start");struct _RegionHandle*start=& _tmp702;_push_region(start);
({struct _RegionHandle*_tmpB00=start;struct Cyc_Toc_Env*_tmpAFF=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB00,_tmpAFF,ds,1,0);});;_pop_region(start);}
# 4530
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
