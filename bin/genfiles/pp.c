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
#define _check_null(ptr) (ptr ? : (void*)_throw_null())
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index)\
   ((char *)ptr) + (elt_sz)*(index))
#define _check_known_subscript_notnull(bound,index) (index)

#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })

#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })

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

/* FIX?  Not sure if we want to pass filename and lineno in here... */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n)            (GC_malloc(n)          ? : _throw_badalloc())
#define _cycalloc_atomic(n)     (GC_malloc_atomic(n)   ? : _throw_badalloc())
#define _cyccalloc(n,s)         (GC_calloc(n,s)        ? : _throw_badalloc())
#define _cyccalloc_atomic(n,s)  (GC_calloc_atomic(n,s) ? : _throw_badalloc())
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
# 100 "cycboot.h"
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 119
void*Cyc_Core_identity(void*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 54 "string.h"
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
# 48 "fn.h"
struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(void*,void*),void*x);
# 51
struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*f)(void*));
# 54
void*Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct _tuple0{struct _dyneither_ptr f1;struct Cyc_List_List*f2;};
# 56
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 74
struct Cyc_PP_Doc*Cyc_PP_oline_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 88
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 97
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);
# 100
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs);
# 103
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);
# 105
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d);
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
# 126
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_PP_Empty_PP_Alist_struct{int tag;int f1;};struct Cyc_PP_Single_PP_Alist_struct{int tag;void*f1;};struct Cyc_PP_Append_PP_Alist_struct{int tag;void*f1;void*f2;};
# 51 "pp.cyc"
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_stringptr={0U,0};
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_link={0U,0};struct _tuple1{void*f1;void*f2;};
# 54
void*Cyc_PP_append(void*a1,void*a2){
struct _tuple1 _tmp0=({struct _tuple1 _tmp9E;_tmp9E.f1=a1,_tmp9E.f2=a2;_tmp9E;});struct _tuple1 _tmp1=_tmp0;if(((struct Cyc_PP_Empty_PP_Alist_struct*)_tmp1.f1)->tag == 0U){_LL1: _LL2:
 return a2;}else{if(((struct Cyc_PP_Empty_PP_Alist_struct*)_tmp1.f2)->tag == 0U){_LL3: _LL4:
 return a1;}else{_LL5: _LL6:
 return(void*)({struct Cyc_PP_Append_PP_Alist_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=2U,_tmp2->f1=a1,_tmp2->f2=a2;_tmp2;});}}_LL0:;}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){
void*_tmp3=y;void*_tmp7;void*_tmp6;void*_tmp5;switch(*((int*)_tmp3)){case 0U: _LL1: _LL2:
 return l;case 1U: _LL3: _tmp5=(void*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmp3)->f1;_LL4:
 return({struct Cyc_List_List*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->hd=_tmp5,_tmp4->tl=l;_tmp4;});default: _LL5: _tmp7=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp3)->f1;_tmp6=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmp3)->f2;_LL6:
 return({void*_tmpA0=_tmp7;Cyc_PP_list_of_alist_f(_tmpA0,Cyc_PP_list_of_alist_f(_tmp6,l));});}_LL0:;}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){
return Cyc_PP_list_of_alist_f(x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){
struct Cyc_Xarray_Xarray*_tmp8=((struct Cyc_Xarray_Xarray*(*)(int,void*))Cyc_Xarray_create)(16,al);
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmp8,al);{
int last=0;
while(last >= 0){
void*_tmp9=((void*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp8,last);void*_tmpA=_tmp9;void*_tmp10;void*_tmpF;struct _dyneither_ptr*_tmpE;switch(*((int*)_tmpA)){case 0U: _LL1: _LL2:
 -- last;goto _LL0;case 1U: _LL3: _tmpE=(struct _dyneither_ptr*)((struct Cyc_PP_Single_PP_Alist_struct*)_tmpA)->f1;_LL4:
 -- last;({struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp9F;_tmp9F.tag=0U,_tmp9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE);_tmp9F;});void*_tmpB[1U];_tmpB[0]=& _tmpD;({struct Cyc___cycFILE*_tmpA2=f;struct _dyneither_ptr _tmpA1=({const char*_tmpC="%s";_tag_dyneither(_tmpC,sizeof(char),3U);});Cyc_fprintf(_tmpA2,_tmpA1,_tag_dyneither(_tmpB,sizeof(void*),1U));});});goto _LL0;default: _LL5: _tmp10=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmpA)->f1;_tmpF=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_tmpA)->f2;_LL6:
# 107
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmp8,last,_tmpF);
if(({int _tmpA3=last;_tmpA3 == ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp8)- 1;}))
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmp8,_tmp10);else{
# 111
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmp8,last + 1,_tmp10);}
++ last;
goto _LL0;}_LL0:;}
# 116
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp8);};}
# 120
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_tmpA4=d->f;((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpA4,({struct Cyc_PP_Ppstate*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->ci=0,_tmp11->cc=0,_tmp11->cl=1,_tmp11->pw=w,_tmp11->epw=w;_tmp11;}));});
Cyc_PP_dump_out(f,o->ppout);}
# 126
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_tmpA5=d->f;((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpA5,({struct Cyc_PP_Ppstate*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->ci=0,_tmp13->cc=0,_tmp13->cl=1,_tmp13->pw=w,_tmp13->epw=w;_tmp13;}));});
return(struct _dyneither_ptr)({struct Cyc_List_List*_tmpA6=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout);Cyc_str_sepstr(_tmpA6,({const char*_tmp12="";_tag_dyneither(_tmp12,sizeof(char),1U);}));});}
# 133
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_tmpA7=d->f;((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpA7,({struct Cyc_PP_Ppstate*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->ci=0,_tmp16->cc=0,_tmp16->cl=1,_tmp16->pw=w,_tmp16->epw=w;_tmp16;}));});
return({struct _tuple0*_tmp15=_cycalloc(sizeof(*_tmp15));({struct _dyneither_ptr _tmpAA=(struct _dyneither_ptr)({
struct Cyc_List_List*_tmpA9=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout);Cyc_str_sepstr(_tmpA9,({const char*_tmp14="";_tag_dyneither(_tmp14,sizeof(char),1U);}));});
# 135
_tmp15->f1=_tmpAA;}),({
# 137
struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->links);_tmp15->f2=_tmpA8;});_tmp15;});}
# 140
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 142
int Cyc_PP_tex_output=0;
# 144
struct _dyneither_ptr Cyc_PP_nlblanks(int i){
if(Cyc_PP_bhashtbl == 0)
({struct Cyc_Core_Opt*_tmpAC=({struct Cyc_Core_Opt*_tmp17=_cycalloc(sizeof(*_tmp17));({struct Cyc_Hashtable_Table*_tmpAB=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(int(*)(int))Cyc_Core_identity);_tmp17->v=_tmpAB;});_tmp17;});Cyc_PP_bhashtbl=_tmpAC;});
if(i < 0)(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpAD=({const char*_tmp18="nlblanks";_tag_dyneither(_tmp18,sizeof(char),9U);});_tmp19->f1=_tmpAD;});_tmp19;}));{
struct _handler_cons _tmp1A;_push_handler(& _tmp1A);{int _tmp1C=0;if(setjmp(_tmp1A.handler))_tmp1C=1;if(!_tmp1C){
{struct _dyneither_ptr _tmp1D=*((struct _dyneither_ptr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i);_npop_handler(0U);return _tmp1D;};_pop_handler();}else{void*_tmp1B=(void*)_exn_thrown;void*_tmp1E=_tmp1B;void*_tmp3C;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp1E)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 152
 if(!Cyc_PP_tex_output){
int _tmp1F=i / 8;
int _tmp20=i % 8;
int _tmp21=(2 + _tmp1F)+ _tmp20;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)_tmp21);
({struct _dyneither_ptr _tmp22=_dyneither_ptr_plus(nlb,sizeof(char),0);char _tmp23=*((char*)_check_dyneither_subscript(_tmp22,sizeof(char),0U));char _tmp24='\n';if(_get_dyneither_size(_tmp22,sizeof(char))== 1U  && (_tmp23 == '\000'  && _tmp24 != '\000'))_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});
{int j=0;for(0;j < _tmp1F;++ j){
({struct _dyneither_ptr _tmp25=_dyneither_ptr_plus(nlb,sizeof(char),j + 1);char _tmp26=*((char*)_check_dyneither_subscript(_tmp25,sizeof(char),0U));char _tmp27='\t';if(_get_dyneither_size(_tmp25,sizeof(char))== 1U  && (_tmp26 == '\000'  && _tmp27 != '\000'))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});}}
{int j=0;for(0;j < _tmp20;++ j){
({struct _dyneither_ptr _tmp28=_dyneither_ptr_plus(nlb,sizeof(char),(j + 1)+ _tmp1F);char _tmp29=*((char*)_check_dyneither_subscript(_tmp28,sizeof(char),0U));char _tmp2A=' ';if(_get_dyneither_size(_tmp28,sizeof(char))== 1U  && (_tmp29 == '\000'  && _tmp2A != '\000'))_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});}}
({struct Cyc_Hashtable_Table*_tmpAF=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v;int _tmpAE=i;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)(_tmpAF,_tmpAE,({unsigned int _tmp2C=1;struct _dyneither_ptr*_tmp2B=_cycalloc(_check_times(_tmp2C,sizeof(struct _dyneither_ptr)));_tmp2B[0]=(struct _dyneither_ptr)nlb;_tmp2B;}));});
return(struct _dyneither_ptr)nlb;}else{
# 168
int _tmp2D=3 + i;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)(_tmp2D + 1));
({struct _dyneither_ptr _tmp2E=_dyneither_ptr_plus(nlb,sizeof(char),0);char _tmp2F=*((char*)_check_dyneither_subscript(_tmp2E,sizeof(char),0U));char _tmp30='\\';if(_get_dyneither_size(_tmp2E,sizeof(char))== 1U  && (_tmp2F == '\000'  && _tmp30 != '\000'))_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});
({struct _dyneither_ptr _tmp31=_dyneither_ptr_plus(nlb,sizeof(char),1);char _tmp32=*((char*)_check_dyneither_subscript(_tmp31,sizeof(char),0U));char _tmp33='\\';if(_get_dyneither_size(_tmp31,sizeof(char))== 1U  && (_tmp32 == '\000'  && _tmp33 != '\000'))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});
({struct _dyneither_ptr _tmp34=_dyneither_ptr_plus(nlb,sizeof(char),2);char _tmp35=*((char*)_check_dyneither_subscript(_tmp34,sizeof(char),0U));char _tmp36='\n';if(_get_dyneither_size(_tmp34,sizeof(char))== 1U  && (_tmp35 == '\000'  && _tmp36 != '\000'))_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});
{int j=3;for(0;j < _tmp2D;++ j){
({struct _dyneither_ptr _tmp37=_dyneither_ptr_plus(nlb,sizeof(char),j);char _tmp38=*((char*)_check_dyneither_subscript(_tmp37,sizeof(char),0U));char _tmp39='~';if(_get_dyneither_size(_tmp37,sizeof(char))== 1U  && (_tmp38 == '\000'  && _tmp39 != '\000'))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});}}
({struct Cyc_Hashtable_Table*_tmpB1=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v;int _tmpB0=i;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)(_tmpB1,_tmpB0,({unsigned int _tmp3B=1;struct _dyneither_ptr*_tmp3A=_cycalloc(_check_times(_tmp3B,sizeof(struct _dyneither_ptr)));_tmp3A[0]=(struct _dyneither_ptr)nlb;_tmp3A;}));});
return(struct _dyneither_ptr)nlb;}}else{_LL3: _tmp3C=_tmp1E;_LL4:(int)_rethrow(_tmp3C);}_LL0:;}};};}
# 182
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 184
int Cyc_PP_infinity=9999999;struct _tuple2{int f1;struct _dyneither_ptr f2;};
# 186
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){
struct _tuple2*_tmp3D=clo;int _tmp43;struct _dyneither_ptr _tmp42;_LL1: _tmp43=_tmp3D->f1;_tmp42=_tmp3D->f2;_LL2:;
return({struct Cyc_PP_Out*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->newcc=st->cc + _tmp43,_tmp41->newcl=st->cl,({
# 190
void*_tmpB3=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->tag=1U,({struct _dyneither_ptr*_tmpB2=({unsigned int _tmp3F=1;struct _dyneither_ptr*_tmp3E=_cycalloc(_check_times(_tmp3F,sizeof(struct _dyneither_ptr)));_tmp3E[0]=_tmp42;_tmp3E;});_tmp40->f1=_tmpB2;});_tmp40;});_tmp41->ppout=_tmpB3;}),_tmp41->links=(void*)& Cyc_PP_Empty_link;_tmp41;});}
# 193
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _dyneither_ptr s){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)s);
return({struct Cyc_PP_Doc*_tmp45=_cycalloc(sizeof(*_tmp45));
_tmp45->mwo=slen,_tmp45->mw=Cyc_PP_infinity,({
# 198
struct Cyc_Fn_Function*_tmpB4=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,({struct _tuple2*_tmp44=_cycalloc(sizeof(*_tmp44));
# 200
_tmp44->f1=slen,_tmp44->f2=s;_tmp44;}));
# 198
_tmp45->f=_tmpB4;});_tmp45;});}
# 202
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl == 0){
({struct Cyc_Hashtable_Table*_tmpB5=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);t=_tmpB5;});
({struct Cyc_Core_Opt*_tmpB6=({struct Cyc_Core_Opt*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->v=t;_tmp46;});Cyc_PP_str_hashtbl=_tmpB6;});}else{
# 208
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{
# 210
struct _handler_cons _tmp47;_push_handler(& _tmp47);{int _tmp49=0;if(setjmp(_tmp47.handler))_tmp49=1;if(!_tmp49){
{struct Cyc_PP_Doc*_tmp4B=({struct Cyc_Hashtable_Table*_tmpB7=t;((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(_tmpB7,({struct _dyneither_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=s;_tmp4A;}));});_npop_handler(0U);return _tmp4B;};_pop_handler();}else{void*_tmp48=(void*)_exn_thrown;void*_tmp4C=_tmp48;void*_tmp4E;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp4C)->tag == Cyc_Core_Not_found){_LL1: _LL2: {
# 214
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);
({struct Cyc_Hashtable_Table*_tmpB9=t;struct _dyneither_ptr*_tmpB8=({struct _dyneither_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));*_tmp4D=s;_tmp4D;});((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(_tmpB9,_tmpB8,d);});
return d;}}else{_LL3: _tmp4E=_tmp4C;_LL4:(int)_rethrow(_tmp4E);}_LL0:;}};};}
# 219
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*s){return Cyc_PP_text(*s);}
# 224
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _dyneither_ptr s,int slen){
return({struct Cyc_PP_Doc*_tmp50=_cycalloc(sizeof(*_tmp50));
_tmp50->mwo=slen,_tmp50->mw=Cyc_PP_infinity,({
# 228
struct Cyc_Fn_Function*_tmpBA=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,({struct _tuple2*_tmp4F=_cycalloc(sizeof(*_tmp4F));
# 230
_tmp4F->f1=slen,_tmp4F->f2=s;_tmp4F;}));
# 228
_tmp50->f=_tmpBA;});_tmp50;});}
# 232
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int slen){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2 == 0){
({struct Cyc_Hashtable_Table*_tmpBB=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);t=_tmpBB;});
({struct Cyc_Core_Opt*_tmpBC=({struct Cyc_Core_Opt*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->v=t;_tmp51;});Cyc_PP_str_hashtbl2=_tmpBC;});}else{
# 239
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{
# 241
struct _handler_cons _tmp52;_push_handler(& _tmp52);{int _tmp54=0;if(setjmp(_tmp52.handler))_tmp54=1;if(!_tmp54){
{struct Cyc_PP_Doc*_tmp56=({struct Cyc_Hashtable_Table*_tmpBD=t;((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(_tmpBD,({struct _dyneither_ptr*_tmp55=_cycalloc(sizeof(*_tmp55));*_tmp55=s;_tmp55;}));});_npop_handler(0U);return _tmp56;};_pop_handler();}else{void*_tmp53=(void*)_exn_thrown;void*_tmp57=_tmp53;void*_tmp59;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp57)->tag == Cyc_Core_Not_found){_LL1: _LL2: {
# 245
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);
({struct Cyc_Hashtable_Table*_tmpBF=t;struct _dyneither_ptr*_tmpBE=({struct _dyneither_ptr*_tmp58=_cycalloc(sizeof(*_tmp58));*_tmp58=s;_tmp58;});((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(_tmpBF,_tmpBE,d);});
return d;}}else{_LL3: _tmp59=_tmp57;_LL4:(int)_rethrow(_tmp59);}_LL0:;}};};}
# 251
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 257
struct Cyc_PP_Doc*Cyc_PP_nil_doc(){
if(Cyc_PP_nil_doc_opt == 0)
({struct Cyc_Core_Opt*_tmpC1=({struct Cyc_Core_Opt*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_PP_Doc*_tmpC0=Cyc_PP_text(({const char*_tmp5A="";_tag_dyneither(_tmp5A,sizeof(char),1U);}));_tmp5B->v=_tmpC0;});_tmp5B;});Cyc_PP_nil_doc_opt=_tmpC1;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}
# 264
struct Cyc_PP_Doc*Cyc_PP_blank_doc(){
if(Cyc_PP_blank_doc_opt == 0)
({struct Cyc_Core_Opt*_tmpC3=({struct Cyc_Core_Opt*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct Cyc_PP_Doc*_tmpC2=Cyc_PP_text(({const char*_tmp5C="";_tag_dyneither(_tmp5C,sizeof(char),1U);}));_tmp5D->v=_tmpC2;});_tmp5D;});Cyc_PP_blank_doc_opt=_tmpC3;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple3{int f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple4{int f1;int f2;int f3;struct _dyneither_ptr f4;};
# 270
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){
struct _tuple3*_tmp5E=clo;int _tmp67;struct _dyneither_ptr _tmp66;struct _dyneither_ptr _tmp65;_LL1: _tmp67=_tmp5E->f1;_tmp66=_tmp5E->f2;_tmp65=_tmp5E->f3;_LL2:;
return({struct Cyc_PP_Out*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->newcc=st->cc + _tmp67,_tmp64->newcl=st->cl,({
# 274
void*_tmpC7=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=1U,({struct _dyneither_ptr*_tmpC6=({unsigned int _tmp60=1;struct _dyneither_ptr*_tmp5F=_cycalloc(_check_times(_tmp60,sizeof(struct _dyneither_ptr)));_tmp5F[0]=_tmp66;_tmp5F;});_tmp61->f1=_tmpC6;});_tmp61;});_tmp64->ppout=_tmpC7;}),({
void*_tmpC5=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=1U,({struct _tuple4*_tmpC4=({struct _tuple4*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=st->cl,_tmp62->f2=st->cc,_tmp62->f3=_tmp67,_tmp62->f4=_tmp65;_tmp62;});_tmp63->f1=_tmpC4;});_tmp63;});_tmp64->links=_tmpC5;});_tmp64;});}
# 277
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)shrt);
return({struct Cyc_PP_Doc*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->mwo=slen,_tmp69->mw=Cyc_PP_infinity,({
# 281
struct Cyc_Fn_Function*_tmpC8=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,({struct _tuple3*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=slen,_tmp68->f2=shrt,_tmp68->f3=full;_tmp68;}));_tmp69->f=_tmpC8;});_tmp69;});}
# 285
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
return({struct Cyc_PP_Out*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->newcc=st->ci,_tmp6D->newcl=st->cl + 1,({
# 288
void*_tmpCB=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=1U,({struct _dyneither_ptr*_tmpCA=({unsigned int _tmp6B=1;struct _dyneither_ptr*_tmp6A=_cycalloc(_check_times(_tmp6B,sizeof(struct _dyneither_ptr)));({struct _dyneither_ptr _tmpC9=Cyc_PP_nlblanks(st->ci);_tmp6A[0]=_tmpC9;});_tmp6A;});_tmp6C->f1=_tmpCA;});_tmp6C;});_tmp6D->ppout=_tmpCB;}),_tmp6D->links=(void*)& Cyc_PP_Empty_link;_tmp6D;});}
# 291
struct Cyc_PP_Doc*Cyc_PP_line_doc(){
if(Cyc_PP_line_doc_opt == 0)
({struct Cyc_Core_Opt*_tmpCE=({struct Cyc_Core_Opt*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_PP_Doc*_tmpCD=({struct Cyc_PP_Doc*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->mwo=0,_tmp6E->mw=0,({struct Cyc_Fn_Function*_tmpCC=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(Cyc_PP_line_f);_tmp6E->f=_tmpCC;});_tmp6E;});_tmp6F->v=_tmpCD;});_tmp6F;});Cyc_PP_line_doc_opt=_tmpCE;});
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple5{int f1;struct Cyc_PP_Doc*f2;};
# 297
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){
struct _tuple5*_tmp70=clo;int _tmp73;struct Cyc_PP_Doc*_tmp72;_LL1: _tmp73=_tmp70->f1;_tmp72=_tmp70->f2;_LL2:;{
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp71=_cycalloc(sizeof(*_tmp71));
_tmp71->ci=st->ci + _tmp73,_tmp71->cc=st->cc,_tmp71->cl=st->cl,_tmp71->pw=st->pw,_tmp71->epw=st->epw;_tmp71;});
# 305
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp72->f,st2);};}
# 307
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
return({struct Cyc_PP_Doc*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->mwo=d->mwo,_tmp75->mw=d->mw,({
# 310
struct Cyc_Fn_Function*_tmpCF=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,({struct _tuple5*_tmp74=_cycalloc(sizeof(*_tmp74));
# 312
_tmp74->f1=k,_tmp74->f2=d;_tmp74;}));
# 310
_tmp75->f=_tmpCF;});_tmp75;});}
# 315
int Cyc_PP_min(int x,int y){
if(x <= y)return x;else{
return y;}}
# 320
int Cyc_PP_max(int x,int y){
if(x >= y)return x;else{
return y;}}struct _tuple6{struct Cyc_PP_Doc*f1;struct Cyc_PP_Doc*f2;};
# 325
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp76=clo;struct Cyc_PP_Doc*_tmp7B;struct Cyc_PP_Doc*_tmp7A;_LL1: _tmp7B=_tmp76->f1;_tmp7A=_tmp76->f2;_LL2:;{
int epw2=Cyc_PP_max(st->pw - _tmp7A->mw,st->epw - _tmp7B->mwo);
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->ci=st->ci,_tmp79->cc=st->cc,_tmp79->cl=st->cl,_tmp79->pw=st->pw,_tmp79->epw=epw2;_tmp79;});
struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp7B->f,st2);
struct Cyc_PP_Ppstate*st3=({struct Cyc_PP_Ppstate*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->ci=st->ci,_tmp78->cc=o1->newcc,_tmp78->cl=o1->newcl,_tmp78->pw=st->pw,_tmp78->epw=epw2;_tmp78;});
struct Cyc_PP_Out*o2=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp7A->f,st3);
struct Cyc_PP_Out*o3=({struct Cyc_PP_Out*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->newcc=o2->newcc,_tmp77->newcl=o2->newcl,({
# 334
void*_tmpD1=((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->ppout,o2->ppout);_tmp77->ppout=_tmpD1;}),({
void*_tmpD0=((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->links,o2->links);_tmp77->links=_tmpD0;});_tmp77;});
return o3;};}
# 338
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp7D=_cycalloc(sizeof(*_tmp7D));({int _tmpD4=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo);_tmp7D->mwo=_tmpD4;}),({
int _tmpD3=Cyc_PP_min(d1->mw,d1->mwo + d2->mw);_tmp7D->mw=_tmpD3;}),({
struct Cyc_Fn_Function*_tmpD2=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,({struct _tuple6*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->f1=d1,_tmp7C->f2=d2;_tmp7C;}));_tmp7D->f=_tmpD2;});_tmp7D;});}
# 343
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr l){
struct Cyc_PP_Doc*_tmp7E=Cyc_PP_nil_doc();
{int i=(int)(_get_dyneither_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;-- i){
({struct Cyc_PP_Doc*_tmpD5=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_dyneither_subscript(l,sizeof(struct Cyc_PP_Doc*),i)),_tmp7E);_tmp7E=_tmpD5;});}}
# 348
return _tmp7E;}
# 353
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
{struct Cyc_List_List*_tmp7F=ds0;for(0;_tmp7F != 0;_tmp7F=_tmp7F->tl){
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_tmp7F->hd;
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
({struct Cyc_PP_Ppstate*_tmpD6=({struct Cyc_PP_Ppstate*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->ci=st->ci,_tmp80->cc=o->newcc,_tmp80->cl=o->newcl,_tmp80->pw=st->pw,_tmp80->epw=st->epw - d->mwo;_tmp80;});st=_tmpD6;});
({struct Cyc_List_List*_tmpD7=({struct Cyc_List_List*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->hd=o,_tmp81->tl=os;_tmp81;});os=_tmpD7;});}}{
# 361
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
({void*_tmpD8=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);s=_tmpD8;});
({void*_tmpD9=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);links=_tmpD9;});}
# 369
return({struct Cyc_PP_Out*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->newcc=newcc,_tmp82->newcl=newcl,_tmp82->ppout=s,_tmp82->links=links;_tmp82;});};}
# 371
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){
# 375
struct Cyc_List_List*orig=doclist;
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;
doclist=doclist->tl;{
int mw=d->mw;
int mwo=d->mw;
# 381
{struct Cyc_List_List*_tmp83=doclist;for(0;_tmp83 != 0;_tmp83=_tmp83->tl){
int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp83->hd)->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp83->hd)->mw);
mw=mw2;
mwo=mwo2;}}
# 387
return({struct Cyc_PP_Doc*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->mwo=mw,_tmp84->mw=mwo,({struct Cyc_Fn_Function*_tmpDA=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(Cyc_PP_long_cats_f,orig);_tmp84->f=_tmpDA;});_tmp84;});};}
# 390
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){
if(doclist == 0)return Cyc_PP_nil_doc();else{
if(doclist->tl == 0)return(struct Cyc_PP_Doc*)doclist->hd;else{
# 394
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(doclist)> 30)return Cyc_PP_long_cats(doclist);else{
return({struct Cyc_PP_Doc*_tmpDB=(struct Cyc_PP_Doc*)doclist->hd;Cyc_PP_concat(_tmpDB,Cyc_PP_cats(doclist->tl));});}}}}
# 398
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _dyneither_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
struct _dyneither_ptr docs=*docs_ptr;
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
{int i=0;for(0;i < sz;++ i){
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
({struct Cyc_PP_Ppstate*_tmpDC=({struct Cyc_PP_Ppstate*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->ci=st->ci,_tmp85->cc=o->newcc,_tmp85->cl=o->newcl,_tmp85->pw=st->pw,_tmp85->epw=st->epw - d->mwo;_tmp85;});st=_tmpDC;});
({struct Cyc_List_List*_tmpDD=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->hd=o,_tmp86->tl=os;_tmp86;});os=_tmpDD;});}}{
# 408
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
({void*_tmpDE=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);s=_tmpDE;});
({void*_tmpDF=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);links=_tmpDF;});}
# 416
return({struct Cyc_PP_Out*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->newcc=newcc,_tmp87->newcl=newcl,_tmp87->ppout=s,_tmp87->links=links;_tmp87;});};}
# 419
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs){
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz == 0)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpE0=({const char*_tmp88="cats_arr -- size zero array";_tag_dyneither(_tmp88,sizeof(char),28U);});_tmp89->f1=_tmpE0;});_tmp89;}));{
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),0));
int mw=d->mw;
int mwo=d->mw;
{int i=1;for(0;i < sz;++ i){
int mw2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);
int mwo2=Cyc_PP_min(mw,mwo + (((struct Cyc_PP_Doc**)docs.curr)[i])->mw);
mw=mw2;
mwo=mwo2;}}
# 432
return({struct Cyc_PP_Doc*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->mwo=mw,_tmp8C->mw=mwo,({struct Cyc_Fn_Function*_tmpE1=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _dyneither_ptr*,struct Cyc_PP_Ppstate*),struct _dyneither_ptr*x))Cyc_Fn_make_fn)(Cyc_PP_cats_arr_f,({unsigned int _tmp8B=1;struct _dyneither_ptr*_tmp8A=_cycalloc(_check_times(_tmp8B,sizeof(struct _dyneither_ptr)));_tmp8A[0]=docs;_tmp8A;}));_tmp8C->f=_tmpE1;});_tmp8C;});};}
# 435
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct _tuple6*_tmp8D=clo;struct Cyc_PP_Doc*_tmp8F;struct Cyc_PP_Doc*_tmp8E;_LL1: _tmp8F=_tmp8D->f1;_tmp8E=_tmp8D->f2;_LL2:;{
int dfits=st->cc + _tmp8F->mwo <= st->epw  || st->cc + _tmp8F->mw <= st->pw;
# 439
if(dfits)return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp8F->f,st);else{
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp8E->f,st);}};}
# 442
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_tmp91=_cycalloc(sizeof(*_tmp91));({int _tmpE4=Cyc_PP_min(d->mwo,d2->mwo);_tmp91->mwo=_tmpE4;}),({
int _tmpE3=Cyc_PP_min(d->mw,d2->mw);_tmp91->mw=_tmpE3;}),({
struct Cyc_Fn_Function*_tmpE2=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,({struct _tuple6*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=d,_tmp90->f2=d2;_tmp90;}));_tmp91->f=_tmpE2;});_tmp91;});}
# 449
struct Cyc_PP_Doc*Cyc_PP_oline_doc(){
return({struct Cyc_PP_Doc*_tmpE5=Cyc_PP_nil_doc();Cyc_PP_doc_union(_tmpE5,Cyc_PP_line_doc());});}
# 453
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->ci=st->cc,_tmp92->cc=st->cc,_tmp92->cl=st->cl,_tmp92->pw=st->pw,_tmp92->epw=st->epw;_tmp92;});
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st2);}
# 457
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){
struct Cyc_PP_Doc*d2=({struct Cyc_PP_Doc*_tmp93=_cycalloc(sizeof(*_tmp93));
_tmp93->mwo=d->mwo,_tmp93->mw=d->mw,({
# 461
struct Cyc_Fn_Function*_tmpE6=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d);_tmp93->f=_tmpE6;});_tmp93;});
return d2;}
# 467
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_tmp94[4U];({struct Cyc_PP_Doc*_tmpEA=pp(l->hd);_tmp94[0]=_tmpEA;}),({struct Cyc_PP_Doc*_tmpE9=Cyc_PP_text(sep);_tmp94[1]=_tmpE9;}),({struct Cyc_PP_Doc*_tmpE8=Cyc_PP_oline_doc();_tmp94[2]=_tmpE8;}),({struct Cyc_PP_Doc*_tmpE7=Cyc_PP_ppseq_f(pp,sep,l->tl);_tmp94[3]=_tmpE7;});Cyc_PP_cat(_tag_dyneither(_tmp94,sizeof(struct Cyc_PP_Doc*),4U));});}}}
# 473
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}
# 477
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
# 481
struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmp95=l;
while(((struct Cyc_List_List*)_check_null(_tmp95))->tl != 0){
struct Cyc_List_List*_tmp96=_tmp95->tl;
({struct Cyc_List_List*_tmpEC=({struct Cyc_List_List*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->hd=sep2,({struct Cyc_List_List*_tmpEB=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=oline,_tmp97->tl=_tmp96;_tmp97;});_tmp98->tl=_tmpEB;});_tmp98;});_tmp95->tl=_tmpEC;});
_tmp95=_tmp96;}
# 489
return Cyc_PP_cats(l);}}}
# 497
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}
# 502
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_tmp99[4U];({struct Cyc_PP_Doc*_tmpF0=pp(l->hd);_tmp99[0]=_tmpF0;}),({struct Cyc_PP_Doc*_tmpEF=Cyc_PP_text(sep);_tmp99[1]=_tmpEF;}),({struct Cyc_PP_Doc*_tmpEE=Cyc_PP_line_doc();_tmp99[2]=_tmpEE;}),({struct Cyc_PP_Doc*_tmpED=Cyc_PP_ppseql_f(pp,sep,l->tl);_tmp99[3]=_tmpED;});Cyc_PP_cat(_tag_dyneither(_tmp99,sizeof(struct Cyc_PP_Doc*),4U));});}}}
# 507
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,l));}
# 511
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
return({struct Cyc_PP_Doc*_tmp9A[4U];_tmp9A[0]=(struct Cyc_PP_Doc*)l->hd,({struct Cyc_PP_Doc*_tmpF3=Cyc_PP_text(sep);_tmp9A[1]=_tmpF3;}),({struct Cyc_PP_Doc*_tmpF2=Cyc_PP_line_doc();_tmp9A[2]=_tmpF2;}),({struct Cyc_PP_Doc*_tmpF1=Cyc_PP_seql_f(sep,l->tl);_tmp9A[3]=_tmpF1;});Cyc_PP_cat(_tag_dyneither(_tmp9A,sizeof(struct Cyc_PP_Doc*),4U));});}}}
# 516
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}
# 521
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp9B[3U];({struct Cyc_PP_Doc*_tmpF6=Cyc_PP_text(start);_tmp9B[0]=_tmpF6;}),({
struct Cyc_PP_Doc*_tmpF5=Cyc_PP_seq(sep,ss);_tmp9B[1]=_tmpF5;}),({
struct Cyc_PP_Doc*_tmpF4=Cyc_PP_text(stop);_tmp9B[2]=_tmpF4;});Cyc_PP_cat(_tag_dyneither(_tmp9B,sizeof(struct Cyc_PP_Doc*),3U));});}
# 528
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
if(ss == 0)return Cyc_PP_nil_doc();else{
return({struct Cyc_PP_Doc*_tmp9C[3U];({struct Cyc_PP_Doc*_tmpF9=Cyc_PP_text(start);_tmp9C[0]=_tmpF9;}),({
struct Cyc_PP_Doc*_tmpF8=Cyc_PP_seq(sep,ss);_tmp9C[1]=_tmpF8;}),({
struct Cyc_PP_Doc*_tmpF7=Cyc_PP_text(stop);_tmp9C[2]=_tmpF7;});Cyc_PP_cat(_tag_dyneither(_tmp9C,sizeof(struct Cyc_PP_Doc*),3U));});}}
# 535
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_tmp9D[3U];({struct Cyc_PP_Doc*_tmpFC=Cyc_PP_text(start);_tmp9D[0]=_tmpFC;}),({
struct Cyc_PP_Doc*_tmpFB=Cyc_PP_seql(sep,ss);_tmp9D[1]=_tmpFB;}),({
struct Cyc_PP_Doc*_tmpFA=Cyc_PP_text(stop);_tmp9D[2]=_tmpFA;});Cyc_PP_cat(_tag_dyneither(_tmp9D,sizeof(struct Cyc_PP_Doc*),3U));});}
