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
struct _RegionPage; // abstract -- defined in runtime_memory.c

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
#endif

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
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
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
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */

extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

/* FIX?  Not sure if we want to pass filename and lineno in here... */
#define _cycalloc(n)            (GC_malloc(n)          ? : _throw_badalloc())
#define _cycalloc_atomic(n)     (GC_malloc_atomic(n)   ? : _throw_badalloc())
#define _cyccalloc(n,s)         (GC_calloc(n,s)        ? : _throw_badalloc())
#define _cyccalloc_atomic(n,s)  (GC_calloc_atomic(n,s) ? : _throw_badalloc())

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
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey();
# 245
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 916
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_uint_typ;
# 956
extern void*Cyc_Absyn_sint_typ;
# 964
void*Cyc_Absyn_exn_typ();
# 973
extern void*Cyc_Absyn_bounds_one;
# 981
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 992
void*Cyc_Absyn_void_star_typ();
# 994
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1095
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1152
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1165
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
# 1178
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1183
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 65
int Cyc_Tcutil_is_arithmetic_type(void*);
# 86
int Cyc_Tcutil_is_pointer_type(void*t);
# 88
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
# 90
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 147
int Cyc_Tcutil_unify(void*,void*);
# 150
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 241 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 319
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_Hashtable_Table;
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
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 61 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 76 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 78
({struct _dyneither_ptr _tmp79D=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp79C=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp79C,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp79D,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp79F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp79E=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp79E,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp79F,ap);});}
# 86
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0U;
unsigned int Cyc_Toc_bounds_checks_eliminated=0U;
# 91
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 94
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 104
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 106
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple15{struct _tuple1*f1;struct _tuple1*f2;};
# 132
static int Cyc_Toc_qvar_tag_cmp(struct _tuple15*x,struct _tuple15*y){
struct _tuple15 _tmp2=*x;struct _tuple15 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple15 _tmp4=*y;struct _tuple15 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 141
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp7AB=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp7AB;}),({
struct Cyc_List_List**_tmp7AA=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp7AA;}),({
struct Cyc_Dict_Dict*_tmp7A9=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7A8=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp7A8;});_tmpC;});_tmp11->aggrs_so_far=_tmp7A9;}),({
struct Cyc_List_List**_tmp7A7=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp7A7;}),({
struct Cyc_Set_Set**_tmp7A6=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp7A5=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp7A5;});_tmpE;});_tmp11->datatypes_so_far=_tmp7A6;}),({
struct Cyc_Dict_Dict*_tmp7A4=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp7A3=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp7A3;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp7A4;}),({
struct Cyc_Dict_Dict*_tmp7A2=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp7A1=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp7A1;});_tmp10;});_tmp11->qvar_tags=_tmp7A2;}),({
struct Cyc_Xarray_Xarray*_tmp7A0=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp7A0;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp7AC=({struct Cyc_Toc_TocStateWrap _tmp767;_tmp767.dyn=_tmp1C,_tmp767.state=_tmp1B;_tmp767;});*ts=_tmp7AC;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp768;_tmp768.f1=_tmp1B,_tmp768.f2=arg;_tmp768;});
({void*_tmp7AD=f(h,& _tmp1A);res=_tmp7AD;});}
# 177
;}
# 179
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple17{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple18{struct Cyc_Toc_TocState*f1;struct _tuple17*f2;};struct _tuple19{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 183
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple18*env){
# 186
struct _tuple18 _tmp1D=*env;struct _tuple18 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp24;struct _tuple1*_tmp23;void*(*_tmp22)(struct _tuple1*);_LL1: _tmp24=_tmp1E.f1;_tmp23=(_tmp1E.f2)->f1;_tmp22=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple19**v=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp24->aggrs_so_far,_tmp23);
if(v == 0)
return _tmp22(_tmp23);else{
# 191
struct _tuple19*_tmp1F=*v;struct _tuple19*_tmp20=_tmp1F;void*_tmp21;_LL4: _tmp21=_tmp20->f2;_LL5:;
return _tmp21;}};}
# 196
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple17 env=({struct _tuple17 _tmp769;_tmp769.f1=q,_tmp769.f2=type_maker;_tmp769;});
return((void*(*)(struct _tuple17*arg,void*(*f)(struct _RegionHandle*,struct _tuple18*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp25[5U]="curr";
# 209 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp25,_tmp25,_tmp25 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp26[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp26,_tmp26,_tmp26 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp27[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp27,_tmp27,_tmp27 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp28[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp28,_tmp28,_tmp28 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp29[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp29,_tmp29,_tmp29 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2A[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2B[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2B,_tmp2B,_tmp2B + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2C[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2C,_tmp2C,_tmp2C + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2D[7U]="_throw";
# 230 "toc.cyc"
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
# 298
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11U,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 301
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 303
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 305
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 309
static struct _tuple20*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_List_List*_tmp7AF=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));({void*_tmp7AE=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=1U,_tmp6D->f1=name;_tmp6D;});_tmp6E->hd=_tmp7AE;}),_tmp6E->tl=0;_tmp6E;});_tmp6F->f1=_tmp7AF;}),_tmp6F->f2=e;_tmp6F;});}
# 315
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp70=e->r;void*_tmp71=_tmp70;struct Cyc_Absyn_Exp*_tmp72;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp72=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp72);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 321
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=14U,_tmp73->f1=t,_tmp73->f2=e,_tmp73->f3=0,_tmp73->f4=Cyc_Absyn_No_coercion;_tmp73;});}
# 324
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=20U,_tmp74->f1=e;_tmp74;});}
# 327
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=23U,_tmp75->f1=e1,_tmp75->f2=e2;_tmp75;});}
# 330
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=37U,_tmp76->f1=s;_tmp76;});}
# 333
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=17U,_tmp77->f1=t;_tmp77;});}
# 336
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=10U,_tmp78->f1=e,_tmp78->f2=es,_tmp78->f3=0,_tmp78->f4=1;_tmp78;});}
# 339
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=1U,_tmp79->f1=e;_tmp79;});}
# 342
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=2U,_tmp7A->f1=s1,_tmp7A->f2=s2;_tmp7A;});}
# 345
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=6U,_tmp7B->f1=e1,_tmp7B->f2=e2,_tmp7B->f3=e3;_tmp7B;});}
# 348
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=21U,_tmp7C->f1=e,_tmp7C->f2=n,_tmp7C->f3=0,_tmp7C->f4=0;_tmp7C;});}
# 351
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=22U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});}
# 354
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 357
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=36U,_tmp7E->f1=tdopt,_tmp7E->f2=ds;_tmp7E;});}
# 359
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=8U,_tmp7F->f1=v;_tmp7F;});}
# 362
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 366
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp80=e->r;void*_tmp81=_tmp80;struct Cyc_Absyn_Exp*_tmp82;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->tag == 20U){_LL1: _tmp82=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp81)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp82,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 385
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 394
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 403
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 412
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 423
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp83=Cyc_Tcutil_compress(t);void*_tmp84=_tmp83;enum Cyc_Absyn_Size_of _tmp8B;switch(*((int*)_tmp84)){case 6U: _LL1: _tmp8B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp84)->f2;_LL2:
# 427
{enum Cyc_Absyn_Size_of _tmp85=_tmp8B;switch(_tmp85){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp86=0U;({struct _dyneither_ptr _tmp7B0=({const char*_tmp87="impossible IntType (not char, short or int)";_tag_dyneither(_tmp87,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B0,_tag_dyneither(_tmp86,sizeof(void*),0U));});});}_LLD:;}
# 433
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp84)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0U,({struct _dyneither_ptr _tmp7B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp76A.f1=_tmp7B1;});_tmp76A;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({struct _dyneither_ptr _tmp7B2=({const char*_tmp89="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp89,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B2,_tag_dyneither(_tmp88,sizeof(void*),1U));});});}_LL0:;}
# 440
return function;}
# 442
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp8C=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp8D=_tmp8C;void*_tmp90;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->tag == 5U){_LL1: _tmp90=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp90);}else{_LL3: _LL4:
({void*_tmp8E=0U;({struct _dyneither_ptr _tmp7B3=({const char*_tmp8F="impossible type (not pointer)";_tag_dyneither(_tmp8F,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B3,_tag_dyneither(_tmp8E,sizeof(void*),0U));});});}_LL0:;}
# 451
static int Cyc_Toc_is_array_type(void*t){
void*_tmp91=Cyc_Tcutil_compress(t);void*_tmp92=_tmp91;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 459
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp93=e->r;void*_tmp94=_tmp93;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;long long _tmp9A;int _tmp99;short _tmp98;struct _dyneither_ptr _tmp97;char _tmp96;switch(*((int*)_tmp94)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Null_c).tag){case 2U: _LL1: _tmp96=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Char_c).val).f2;_LL2:
 return _tmp96 == '\000';case 3U: _LL3: _tmp97=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Wchar_c).val;_LL4: {
# 463
unsigned long _tmp95=Cyc_strlen((struct _dyneither_ptr)_tmp97);
int i=0;
if(_tmp95 >= 2  && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp97.curr)[1]== 'x'  && _tmp95 >= 3) && *((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp95;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp97,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 473
return 0;}}case 4U: _LL5: _tmp98=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Short_c).val).f2;_LL6:
 return _tmp98 == 0;case 5U: _LL7: _tmp99=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).Int_c).val).f2;_LL8:
 return _tmp99 == 0;case 6U: _LL9: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp94)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9A == 0;case 1U: _LLD: _LLE:
# 478
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 477
 goto _LLE;case 14U: _LLF: _tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL10:
# 479
 return Cyc_Toc_is_zero(_tmp9B);case 24U: _LL11: _tmp9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp9C);case 26U: _LL13: _tmp9D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp94)->f1;_LL14:
 _tmp9E=_tmp9D;goto _LL16;case 29U: _LL15: _tmp9E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp94)->f3;_LL16:
 _tmp9F=_tmp9E;goto _LL18;case 25U: _LL17: _tmp9F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL18:
 _tmpA0=_tmp9F;goto _LL1A;case 36U: _LL19: _tmpA0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp94)->f2;_LL1A:
# 485
 for(0;_tmpA0 != 0;_tmpA0=_tmpA0->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmpA0->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 494
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA1=x;union Cyc_Absyn_Nmspace _tmpB3;struct _dyneither_ptr*_tmpB2;_LL1: _tmpB3=_tmpA1->f1;_tmpB2=_tmpA1->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA2=_tmpB3;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;switch((_tmpA2.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpAF=0;goto _LL7;case 1U: _LL6: _tmpAF=(_tmpA2.Rel_n).val;_LL7:
 _tmpB0=_tmpAF;goto _LL9;case 2U: _LL8: _tmpB0=(_tmpA2.Abs_n).val;_LL9:
 _tmpB1=_tmpB0;goto _LLB;default: _LLA: _tmpB1=(_tmpA2.C_n).val;_LLB:
# 503
 if(_tmpB1 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA5=({struct Cyc_String_pa_PrintArg_struct _tmp76C;_tmp76C.tag=0U,_tmp76C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp76C;});struct Cyc_String_pa_PrintArg_struct _tmpA6=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp76B;});void*_tmpA3[2U];_tmpA3[0]=& _tmpA5,_tmpA3[1]=& _tmpA6;({struct _dyneither_ptr _tmp7B4=({const char*_tmpA4="%s_%s_struct";_tag_dyneither(_tmpA4,sizeof(char),13U);});Cyc_aprintf(_tmp7B4,_tag_dyneither(_tmpA3,sizeof(void*),2U));});});{
struct _RegionHandle _tmpA7=_new_region("r");struct _RegionHandle*r=& _tmpA7;_push_region(r);
{struct _dyneither_ptr _tmpAE=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp76F;_tmp76F.tag=0U,_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp76F;});struct Cyc_String_pa_PrintArg_struct _tmpAB=({struct Cyc_String_pa_PrintArg_struct _tmp76E;_tmp76E.tag=0U,({struct _dyneither_ptr _tmp7B7=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7B6=r;struct Cyc_List_List*_tmp7B5=_tmpB1;Cyc_rstr_sepstr(_tmp7B6,_tmp7B5,({const char*_tmpAD="_";_tag_dyneither(_tmpAD,sizeof(char),2U);}));}));_tmp76E.f1=_tmp7B7;});_tmp76E;});struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0U,_tmp76D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB2);_tmp76D;});void*_tmpA8[3U];_tmpA8[0]=& _tmpAA,_tmpA8[1]=& _tmpAB,_tmpA8[2]=& _tmpAC;({struct _dyneither_ptr _tmp7B8=({const char*_tmpA9="%s_%s_%s_struct";_tag_dyneither(_tmpA9,sizeof(char),16U);});Cyc_aprintf(_tmp7B8,_tag_dyneither(_tmpA8,sizeof(void*),3U));});});_npop_handler(0U);return _tmpAE;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 516
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 519
struct _tuple21 _tmpB4=*env;struct _tuple21 _tmpB5=_tmpB4;struct Cyc_Dict_Dict*_tmpC8;struct _tuple15*_tmpC7;_LL1: _tmpC8=(_tmpB5.f1)->qvar_tags;_tmpC7=_tmpB5.f2;_LL2:;{
struct _tuple15 _tmpB6=*_tmpC7;struct _tuple15 _tmpB7=_tmpB6;struct _tuple1*_tmpC6;struct _tuple1*_tmpC5;_LL4: _tmpC6=_tmpB7.f1;_tmpC5=_tmpB7.f2;_LL5:;{
struct _handler_cons _tmpB8;_push_handler(& _tmpB8);{int _tmpBA=0;if(setjmp(_tmpB8.handler))_tmpBA=1;if(!_tmpBA){{struct _tuple1*_tmpBB=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpC8,Cyc_Toc_qvar_tag_cmp,_tmpC7);_npop_handler(0U);return _tmpBB;};_pop_handler();}else{void*_tmpB9=(void*)_exn_thrown;void*_tmpBC=_tmpB9;void*_tmpC4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpBC)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 523
struct _tuple15*_tmpBD=({struct _tuple15*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->f1=_tmpC6,_tmpC3->f2=_tmpC5;_tmpC3;});
struct _tuple1*_tmpBE=_tmpC6;union Cyc_Absyn_Nmspace _tmpC2;struct _dyneither_ptr*_tmpC1;_LLC: _tmpC2=_tmpBE->f1;_tmpC1=_tmpBE->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC1,_tmpC5);
struct _tuple1*res=({struct _tuple1*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=_tmpC2,({struct _dyneither_ptr*_tmp7B9=({struct _dyneither_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));*_tmpBF=newvar;_tmpBF;});_tmpC0->f2=_tmp7B9;});_tmpC0;});
({struct Cyc_Dict_Dict _tmp7BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpC8,_tmpBD,res);*_tmpC8=_tmp7BA;});
return res;};}}else{_LL9: _tmpC4=_tmpBC;_LLA:(int)_rethrow(_tmpC4);}_LL6:;}};};};}
# 532
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp770;_tmp770.f1=fieldname,_tmp770.f2=dtname;_tmp770;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 539
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->kind=Cyc_Absyn_StructA,_tmpCB->sc=Cyc_Absyn_Public,_tmpCB->tvs=0,_tmpCB->attributes=0,_tmpCB->expected_mem_kind=0,({
# 542
struct _tuple1*_tmp7BD=({struct _tuple1*_tmpC9=_cycalloc(sizeof(*_tmpC9));({union Cyc_Absyn_Nmspace _tmp7BC=Cyc_Absyn_Rel_n(0);_tmpC9->f1=_tmp7BC;}),_tmpC9->f2=name;_tmpC9;});_tmpCB->name=_tmp7BD;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7BB=({struct Cyc_Absyn_AggrdeclImpl*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->exist_vars=0,_tmpCA->rgn_po=0,_tmpCA->tagged=0,_tmpCA->fields=fs;_tmpCA;});_tmpCB->impl=_tmp7BB;});_tmpCB;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 551
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 554
struct _tuple22 _tmpCC=*env;struct _tuple22 _tmpCD=_tmpCC;struct Cyc_List_List**_tmpE4;struct Cyc_List_List*_tmpE3;_LL1: _tmpE4=(_tmpCD.f1)->tuple_types;_tmpE3=_tmpCD.f2;_LL2:;
# 556
{struct Cyc_List_List*_tmpCE=*_tmpE4;for(0;_tmpCE != 0;_tmpCE=_tmpCE->tl){
struct _tuple23*_tmpCF=(struct _tuple23*)_tmpCE->hd;struct _tuple23*_tmpD0=_tmpCF;void*_tmpD3;struct Cyc_List_List*_tmpD2;_LL4: _tmpD3=_tmpD0->f1;_tmpD2=_tmpD0->f2;_LL5:;{
struct Cyc_List_List*_tmpD1=_tmpE3;
for(0;_tmpD1 != 0  && _tmpD2 != 0;(_tmpD1=_tmpD1->tl,_tmpD2=_tmpD2->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD1->hd)).f2,(void*)_tmpD2->hd))
break;}
if(_tmpD1 == 0  && _tmpD2 == 0)
return _tmpD3;};}}{
# 567
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE2=_cycalloc(sizeof(*_tmpE2));({struct _dyneither_ptr _tmp7BF=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE1=({struct Cyc_Int_pa_PrintArg_struct _tmp771;_tmp771.tag=1U,_tmp771.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp771;});void*_tmpDF[1U];_tmpDF[0]=& _tmpE1;({struct _dyneither_ptr _tmp7BE=({const char*_tmpE0="_tuple%d";_tag_dyneither(_tmpE0,sizeof(char),9U);});Cyc_aprintf(_tmp7BE,_tag_dyneither(_tmpDF,sizeof(void*),1U));});});*_tmpE2=_tmp7BF;});_tmpE2;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE3);
struct Cyc_List_List*_tmpD4=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7C2=({struct Cyc_List_List*_tmpD6=_cycalloc(sizeof(*_tmpD6));({struct Cyc_Absyn_Aggrfield*_tmp7C1=({struct Cyc_Absyn_Aggrfield*_tmpD5=_cycalloc(sizeof(*_tmpD5));({struct _dyneither_ptr*_tmp7C0=Cyc_Absyn_fieldname(i);_tmpD5->name=_tmp7C0;}),_tmpD5->tq=Cyc_Toc_mt_tq,_tmpD5->type=(void*)ts2->hd,_tmpD5->width=0,_tmpD5->attributes=0,_tmpD5->requires_clause=0;_tmpD5;});_tmpD6->hd=_tmp7C1;}),_tmpD6->tl=_tmpD4;_tmpD6;});_tmpD4=_tmp7C2;});}}
({struct Cyc_List_List*_tmp7C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD4);_tmpD4=_tmp7C3;});{
struct Cyc_Absyn_Aggrdecl*_tmpD7=Cyc_Toc_make_c_struct_defn(xname,_tmpD4);
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD8=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7C4=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpDD=_cycalloc(sizeof(*_tmpDD));*_tmpDD=_tmpD7;_tmpDD;}));(_tmpDE->f1).aggr_info=_tmp7C4;}),(_tmpDE->f1).targs=0;_tmpDE;});
({struct Cyc_List_List*_tmp7C6=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_Decl*_tmp7C5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=5U,_tmpD9->f1=_tmpD7;_tmpD9;}),0U);_tmpDA->hd=_tmp7C5;}),_tmpDA->tl=Cyc_Toc_result_decls;_tmpDA;});Cyc_Toc_result_decls=_tmp7C6;});
({struct Cyc_List_List*_tmp7C8=({struct Cyc_List_List*_tmpDC=_region_malloc(d,sizeof(*_tmpDC));({struct _tuple23*_tmp7C7=({struct _tuple23*_tmpDB=_region_malloc(d,sizeof(*_tmpDB));_tmpDB->f1=(void*)_tmpD8,_tmpDB->f2=ts;_tmpDB;});_tmpDC->hd=_tmp7C7;}),_tmpDC->tl=*_tmpE4;_tmpDC;});*_tmpE4=_tmp7C8;});
return(void*)_tmpD8;};};}
# 580
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 585
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 588
struct _tuple25*_tmpE5=env;struct Cyc_List_List**_tmpFA;enum Cyc_Absyn_AggrKind _tmpF9;struct Cyc_List_List*_tmpF8;_LL1: _tmpFA=(_tmpE5->f1)->anon_aggr_types;_tmpF9=(_tmpE5->f2)->f1;_tmpF8=(_tmpE5->f2)->f2;_LL2:;
# 590
{struct Cyc_List_List*_tmpE6=*_tmpFA;for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
struct _tuple26*_tmpE7=(struct _tuple26*)_tmpE6->hd;struct _tuple26*_tmpE8=_tmpE7;void*_tmpEB;enum Cyc_Absyn_AggrKind _tmpEA;struct Cyc_List_List*_tmpE9;_LL4: _tmpEB=_tmpE8->f1;_tmpEA=_tmpE8->f2;_tmpE9=_tmpE8->f3;_LL5:;
if(_tmpF9 != _tmpEA)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpE9,_tmpF8))
return _tmpEB;}}{
# 599
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct _dyneither_ptr _tmp7CA=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp772;_tmp772.tag=1U,_tmp772.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp772;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp7C9=({const char*_tmpF5="_tuple%d";_tag_dyneither(_tmpF5,sizeof(char),9U);});Cyc_aprintf(_tmp7C9,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});*_tmpF7=_tmp7CA;});_tmpF7;});
struct Cyc_Absyn_Aggrdecl*_tmpEC=Cyc_Toc_make_c_struct_defn(xname,_tmpF8);
_tmpEC->kind=_tmpF9;{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpED=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7CB=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF2=_cycalloc(sizeof(*_tmpF2));*_tmpF2=_tmpEC;_tmpF2;}));(_tmpF3->f1).aggr_info=_tmp7CB;}),(_tmpF3->f1).targs=0;_tmpF3;});
({struct Cyc_List_List*_tmp7CD=({struct Cyc_List_List*_tmpEF=_cycalloc(sizeof(*_tmpEF));({struct Cyc_Absyn_Decl*_tmp7CC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=5U,_tmpEE->f1=_tmpEC;_tmpEE;}),0U);_tmpEF->hd=_tmp7CC;}),_tmpEF->tl=Cyc_Toc_result_decls;_tmpEF;});Cyc_Toc_result_decls=_tmp7CD;});
({struct Cyc_List_List*_tmp7CF=({struct Cyc_List_List*_tmpF1=_region_malloc(d,sizeof(*_tmpF1));({struct _tuple26*_tmp7CE=({struct _tuple26*_tmpF0=_region_malloc(d,sizeof(*_tmpF0));_tmpF0->f1=(void*)_tmpED,_tmpF0->f2=_tmpF9,_tmpF0->f3=_tmpF8;_tmpF0;});_tmpF1->hd=_tmp7CE;}),_tmpF1->tl=*_tmpFA;_tmpF1;});*_tmpFA=_tmp7CF;});
return(void*)_tmpED;};};}
# 607
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=ak,_tmpFB->f2=fs;_tmpFB;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 616
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 625
struct _tuple28 _tmpFC=*env;struct _tuple28 _tmpFD=_tmpFC;struct Cyc_List_List**_tmp120;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp11E;struct Cyc_List_List*_tmp11D;struct Cyc_List_List*_tmp11C;_LL1: _tmp120=(_tmpFD.f1)->abs_struct_types;_tmp11F=(_tmpFD.f2)->f1;_tmp11E=(_tmpFD.f2)->f2;_tmp11D=(_tmpFD.f2)->f3;_tmp11C=(_tmpFD.f2)->f4;_LL2:;
# 629
{struct Cyc_List_List*_tmpFE=*_tmp120;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple29*_tmpFF=(struct _tuple29*)_tmpFE->hd;struct _tuple29*_tmp100=_tmpFF;struct _tuple1*_tmp108;struct Cyc_List_List*_tmp107;void*_tmp106;_LL4: _tmp108=_tmp100->f1;_tmp107=_tmp100->f2;_tmp106=_tmp100->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp108,_tmp11F)== 0  && ({
int _tmp7D0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp11D);_tmp7D0 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp107);})){
int okay=1;
{struct Cyc_List_List*_tmp101=_tmp11D;for(0;_tmp101 != 0;(_tmp101=_tmp101->tl,_tmp107=_tmp107->tl)){
void*_tmp102=(void*)_tmp101->hd;
void*_tmp103=(void*)((struct Cyc_List_List*)_check_null(_tmp107))->hd;
{struct Cyc_Absyn_Kind*_tmp104=Cyc_Tcutil_typ_kind(_tmp102);struct Cyc_Absyn_Kind*_tmp105=_tmp104;switch(((struct Cyc_Absyn_Kind*)_tmp105)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 640
 continue;default: _LLB: _LLC:
# 643
 if(Cyc_Tcutil_unify(_tmp102,_tmp103) || ({void*_tmp7D1=Cyc_Toc_typ_to_c(_tmp102);Cyc_Tcutil_unify(_tmp7D1,Cyc_Toc_typ_to_c(_tmp103));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 648
break;}}
# 650
if(okay)
return _tmp106;}}}{
# 657
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct _dyneither_ptr _tmp7D3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp773;_tmp773.tag=1U,_tmp773.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp773;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct _dyneither_ptr _tmp7D2=({const char*_tmp119="_tuple%d";_tag_dyneither(_tmp119,sizeof(char),9U);});Cyc_aprintf(_tmp7D2,_tag_dyneither(_tmp118,sizeof(void*),1U));});});*_tmp11B=_tmp7D3;});_tmp11B;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp109=0;
# 661
({struct Cyc_List_List*_tmp7D5=({struct Cyc_List_List*_tmp10B=_region_malloc(d,sizeof(*_tmp10B));({struct _tuple29*_tmp7D4=({struct _tuple29*_tmp10A=_region_malloc(d,sizeof(*_tmp10A));_tmp10A->f1=_tmp11F,_tmp10A->f2=_tmp11D,_tmp10A->f3=x;_tmp10A;});_tmp10B->hd=_tmp7D4;}),_tmp10B->tl=*_tmp120;_tmp10B;});*_tmp120=_tmp7D5;});{
# 664
struct _RegionHandle _tmp10C=_new_region("r");struct _RegionHandle*r=& _tmp10C;_push_region(r);
{struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp11E,_tmp11D);
for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){
struct Cyc_Absyn_Aggrfield*_tmp10E=(struct Cyc_Absyn_Aggrfield*)_tmp11C->hd;
void*t=_tmp10E->type;
struct Cyc_List_List*atts=_tmp10E->attributes;
# 674
if((_tmp11C->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))) && !
Cyc_Toc_is_array_type(t))
({struct Cyc_List_List*_tmp7D7=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({void*_tmp7D6=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=0;_tmp10F;});_tmp110->hd=_tmp7D6;}),_tmp110->tl=atts;_tmp110;});atts=_tmp7D7;});
({void*_tmp7D8=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp10D,t));t=_tmp7D8;});
# 681
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp7DD=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=8U,({void*_tmp7DC=Cyc_Absyn_void_star_typ();(_tmp111->f1).elt_type=_tmp7DC;}),({struct Cyc_Absyn_Tqual _tmp7DB=Cyc_Absyn_empty_tqual(0U);(_tmp111->f1).tq=_tmp7DB;}),({
struct Cyc_Absyn_Exp*_tmp7DA=Cyc_Absyn_uint_exp(0U,0U);(_tmp111->f1).num_elts=_tmp7DA;}),({union Cyc_Absyn_Constraint*_tmp7D9=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp111->f1).zero_term=_tmp7D9;}),(_tmp111->f1).zt_loc=0U;_tmp111;});
# 682
t=_tmp7DD;});
# 685
({struct Cyc_List_List*_tmp7DF=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(*_tmp113));({struct Cyc_Absyn_Aggrfield*_tmp7DE=({struct Cyc_Absyn_Aggrfield*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp10E->name,_tmp112->tq=Cyc_Toc_mt_tq,_tmp112->type=t,_tmp112->width=_tmp10E->width,_tmp112->attributes=atts,_tmp112->requires_clause=0;_tmp112;});_tmp113->hd=_tmp7DE;}),_tmp113->tl=_tmp109;_tmp113;});_tmp109=_tmp7DF;});}
# 687
({struct Cyc_List_List*_tmp7E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp109);_tmp109=_tmp7E0;});{
struct Cyc_Absyn_Aggrdecl*_tmp114=Cyc_Toc_make_c_struct_defn(xname,_tmp109);
({struct Cyc_List_List*_tmp7E2=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct Cyc_Absyn_Decl*_tmp7E1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=5U,_tmp115->f1=_tmp114;_tmp115;}),0U);_tmp116->hd=_tmp7E1;}),_tmp116->tl=Cyc_Toc_result_decls;_tmp116;});Cyc_Toc_result_decls=_tmp7E2;});{
void*_tmp117=x;_npop_handler(0U);return _tmp117;};};}
# 665
;_pop_region(r);};};}
# 693
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 697
struct _tuple27 env=({struct _tuple27 _tmp774;_tmp774.f1=struct_name,_tmp774.f2=type_vars,_tmp774.f3=type_args,_tmp774.f4=fields;_tmp774;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 705
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp7E5=({struct _dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));({struct _dyneither_ptr _tmp7E4=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp123=({struct Cyc_Int_pa_PrintArg_struct _tmp775;_tmp775.tag=1U,_tmp775.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp775;});void*_tmp121[1U];_tmp121[0]=& _tmp123;({struct _dyneither_ptr _tmp7E3=({const char*_tmp122="_tmp%X";_tag_dyneither(_tmp122,sizeof(char),7U);});Cyc_aprintf(_tmp7E3,_tag_dyneither(_tmp121,sizeof(void*),1U));});});*_tmp124=_tmp7E4;});_tmp124;});_tmp125->f2=_tmp7E5;});_tmp125;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 711
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp126=*env;struct _tuple30 _tmp127=_tmp126;struct Cyc_Xarray_Xarray*_tmp12F;_LL1: _tmp12F=(_tmp127.f1)->temp_labels;_LL2:;{
int _tmp128=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7E6=_tmp128;_tmp7E6 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp12F);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp12F,_tmp128);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct _dyneither_ptr _tmp7E8=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12D=({struct Cyc_Int_pa_PrintArg_struct _tmp776;_tmp776.tag=1U,_tmp776.f1=(unsigned int)_tmp128;_tmp776;});void*_tmp12B[1U];_tmp12B[0]=& _tmp12D;({struct _dyneither_ptr _tmp7E7=({const char*_tmp12C="_LL%X";_tag_dyneither(_tmp12C,sizeof(char),6U);});Cyc_aprintf(_tmp7E7,_tag_dyneither(_tmp12B,sizeof(void*),1U));});});*_tmp12E=_tmp7E8;});_tmp12E;});
if(({int _tmp7E9=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp12F,res);_tmp7E9 != _tmp128;}))
({void*_tmp129=0U;({struct _dyneither_ptr _tmp7EA=({const char*_tmp12A="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp12A,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7EA,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
return res;};};}
# 722
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 728
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp130=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp130))->hd)->name)!= 0){
++ ans;
_tmp130=_tmp130->tl;}
# 735
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 741
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp131=*a;struct _tuple9 _tmp132=_tmp131;struct _dyneither_ptr*_tmp136;struct Cyc_Absyn_Tqual _tmp135;void*_tmp134;_LL1: _tmp136=_tmp132.f1;_tmp135=_tmp132.f2;_tmp134=_tmp132.f3;_LL2:;
return({struct _tuple9*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->f1=_tmp136,_tmp133->f2=_tmp135,({void*_tmp7EB=Cyc_Toc_typ_to_c(_tmp134);_tmp133->f3=_tmp7EB;});_tmp133;});}
# 764 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);void*_tmp138=_tmp137;void*_tmp13E;void*_tmp13D;struct Cyc_Absyn_Tqual _tmp13C;struct Cyc_Absyn_Exp*_tmp13B;union Cyc_Absyn_Constraint*_tmp13A;unsigned int _tmp139;switch(*((int*)_tmp138)){case 8U: _LL1: _tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).elt_type;_tmp13C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).tq;_tmp13B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).num_elts;_tmp13A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zero_term;_tmp139=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp138)->f1).zt_loc;_LL2:
# 767
 return({void*_tmp7EC=Cyc_Toc_typ_to_c_array(_tmp13D);Cyc_Absyn_cstar_typ(_tmp7EC,_tmp13C);});case 1U: _LL3: _tmp13E=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp138)->f2;if(_tmp13E != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp13E);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 773
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 775
struct Cyc_Absyn_Aggrfield*_tmp13F=({struct Cyc_Absyn_Aggrfield*_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=*f;_tmp140;});
_tmp13F->type=new_type;
_tmp13F->requires_clause=0;
_tmp13F->tq=Cyc_Toc_mt_tq;
return _tmp13F;}
# 782
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 784
return;}
# 787
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp7EE=({void**_tmp141=_cycalloc(sizeof(*_tmp141));({void*_tmp7ED=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);*_tmp141=_tmp7ED;});_tmp141;});cs=_tmp7EE;});
return*cs;}
# 793
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp7F1=({void**_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp7F0=({void*_tmp7EF=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7EF,Cyc_Toc_mt_tq);});*_tmp142=_tmp7F0;});_tmp142;});r=_tmp7F1;});
return*r;}
# 799
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp7F4=({void**_tmp143=_cycalloc(sizeof(*_tmp143));({void*_tmp7F3=({void*_tmp7F2=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7F2,Cyc_Toc_mt_tq);});*_tmp143=_tmp7F3;});_tmp143;});r=_tmp7F4;});
return*r;}
# 805
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp144=Cyc_Tcutil_compress(t);void*_tmp145=_tmp144;struct Cyc_Absyn_Tvar*_tmp146;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->tag == 2U){_LL1: _tmp146=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp145)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 811
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp147=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp148=_tmp147;if(((struct Cyc_Absyn_Kind*)_tmp148)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 818
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp149=t;struct Cyc_Absyn_Datatypedecl*_tmp192;void**_tmp191;struct Cyc_Absyn_Enumdecl*_tmp190;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Typedefdecl*_tmp18A;void*_tmp189;struct Cyc_List_List*_tmp188;struct _tuple1*_tmp187;union Cyc_Absyn_AggrInfoU _tmp186;struct Cyc_List_List*_tmp185;enum Cyc_Absyn_AggrKind _tmp184;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Tqual _tmp181;void*_tmp180;struct Cyc_List_List*_tmp17F;int _tmp17E;struct Cyc_Absyn_VarargInfo*_tmp17D;struct Cyc_List_List*_tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp179;unsigned int _tmp178;void*_tmp177;struct Cyc_Absyn_Tqual _tmp176;union Cyc_Absyn_Constraint*_tmp175;struct Cyc_Absyn_Datatypedecl*_tmp174;struct Cyc_Absyn_Datatypefield*_tmp173;struct Cyc_Absyn_Tvar*_tmp172;void**_tmp171;switch(*((int*)_tmp149)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmp171=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp149)->f2;_LL4:
# 822
 if(*_tmp171 == 0){
*_tmp171=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 826
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp171));case 2U: _LL5: _tmp172=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL6:
# 828
 if((Cyc_Tcutil_tvar_kind(_tmp172,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 830
return(void*)& Cyc_Absyn_VoidType_val;else{
# 832
return Cyc_Absyn_void_star_typ();}case 3U: _LL7: _LL8:
# 834
 return(void*)& Cyc_Absyn_VoidType_val;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).tag == 2){_LL9: _tmp174=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f1;_tmp173=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp149)->f1).field_info).KnownDatatypefield).val).f2;_LLA:
# 836
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp173->name,_tmp174->name));}else{_LLB: _LLC:
({void*_tmp14A=0U;({struct _dyneither_ptr _tmp7F5=({const char*_tmp14B="unresolved DatatypeFieldType";_tag_dyneither(_tmp14B,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F5,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});}case 5U: _LLD: _tmp177=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_typ;_tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).elt_tq;_tmp175=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp149)->f1).ptr_atts).bounds;_LLE:
# 841
({void*_tmp7F6=Cyc_Toc_typ_to_c(_tmp177);_tmp177=_tmp7F6;});{
void*_tmp14C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp175);void*_tmp14D=_tmp14C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp14D)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp177,(void*)& Cyc_Absyn_HeapRgn_val,_tmp176,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 846
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp17B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).elt_type;_tmp17A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).tq;_tmp179=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).num_elts;_tmp178=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp149)->f1).zt_loc;_LL14:
# 849
 return({void*_tmp7FA=Cyc_Toc_typ_to_c(_tmp17B);struct Cyc_Absyn_Tqual _tmp7F9=_tmp17A;struct Cyc_Absyn_Exp*_tmp7F8=_tmp179;union Cyc_Absyn_Constraint*_tmp7F7=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp7FA,_tmp7F9,_tmp7F8,_tmp7F7,_tmp178);});case 9U: _LL15: _tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_tqual;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).ret_typ;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).args;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).c_varargs;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).cyc_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp149)->f1).attributes;_LL16: {
# 855
struct Cyc_List_List*_tmp14E=0;
for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
void*_tmp14F=(void*)_tmp17C->hd;void*_tmp150=_tmp14F;switch(*((int*)_tmp150)){case 4U: _LL47: _LL48:
 goto _LL4A;case 5U: _LL49: _LL4A:
 goto _LL4C;case 19U: _LL4B: _LL4C:
 continue;case 22U: _LL4D: _LL4E:
 continue;case 21U: _LL4F: _LL50:
 continue;case 20U: _LL51: _LL52:
 continue;default: _LL53: _LL54:
({struct Cyc_List_List*_tmp7FB=({struct Cyc_List_List*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->hd=(void*)_tmp17C->hd,_tmp151->tl=_tmp14E;_tmp151;});_tmp14E=_tmp7FB;});goto _LL46;}_LL46:;}{
# 866
struct Cyc_List_List*_tmp152=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp17F);
if(_tmp17D != 0){
# 869
void*_tmp153=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp17D->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp154=({struct _tuple9*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->f1=_tmp17D->name,_tmp156->f2=_tmp17D->tq,_tmp156->f3=_tmp153;_tmp156;});
({struct Cyc_List_List*_tmp7FD=({struct Cyc_List_List*_tmp7FC=_tmp152;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp7FC,({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=_tmp154,_tmp155->tl=0;_tmp155;}));});_tmp152=_tmp7FD;});}
# 873
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->tag=9U,(_tmp157->f1).tvars=0,(_tmp157->f1).effect=0,(_tmp157->f1).ret_tqual=_tmp181,({void*_tmp7FE=Cyc_Toc_typ_to_c(_tmp180);(_tmp157->f1).ret_typ=_tmp7FE;}),(_tmp157->f1).args=_tmp152,(_tmp157->f1).c_varargs=_tmp17E,(_tmp157->f1).cyc_varargs=0,(_tmp157->f1).rgn_po=0,(_tmp157->f1).attributes=_tmp14E,(_tmp157->f1).requires_clause=0,(_tmp157->f1).requires_relns=0,(_tmp157->f1).ensures_clause=0,(_tmp157->f1).ensures_relns=0;_tmp157;});};}case 10U: _LL17: _tmp182=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp149)->f1;_LL18: {
# 878
struct Cyc_List_List*_tmp158=0;
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
({struct Cyc_List_List*_tmp801=({struct Cyc_List_List*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct _tuple12*_tmp800=({struct _tuple12*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->f1=(*((struct _tuple12*)_tmp182->hd)).f1,({void*_tmp7FF=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp182->hd)).f2);_tmp159->f2=_tmp7FF;});_tmp159;});_tmp15A->hd=_tmp800;}),_tmp15A->tl=_tmp158;_tmp15A;});_tmp158=_tmp801;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp158));}case 12U: _LL19: _tmp184=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f1;_tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp149)->f2;_LL1A: {
# 887
struct Cyc_List_List*_tmp15B=0;
for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
({struct Cyc_List_List*_tmp804=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));({struct Cyc_Absyn_Aggrfield*_tmp803=({struct Cyc_Absyn_Aggrfield*_tmp802=(struct Cyc_Absyn_Aggrfield*)_tmp183->hd;Cyc_Toc_aggrfield_to_c(_tmp802,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp183->hd)->type));});_tmp15C->hd=_tmp803;}),_tmp15C->tl=_tmp15B;_tmp15C;});_tmp15B=_tmp804;});}
return({enum Cyc_Absyn_AggrKind _tmp805=_tmp184;Cyc_Toc_add_anon_aggr_type(_tmp805,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15B));});}case 11U: _LL1B: _tmp186=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).aggr_info;_tmp185=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp149)->f1).targs;_LL1C:
# 893
{union Cyc_Absyn_AggrInfoU _tmp15D=_tmp186;if((_tmp15D.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 897
struct Cyc_Absyn_Aggrdecl*_tmp15E=Cyc_Absyn_get_known_aggrdecl(_tmp186);
if(_tmp15E->expected_mem_kind){
# 900
if(_tmp15E->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp161=({struct Cyc_String_pa_PrintArg_struct _tmp778;_tmp778.tag=0U,({
struct _dyneither_ptr _tmp806=(struct _dyneither_ptr)(_tmp15E->kind == Cyc_Absyn_UnionA?({const char*_tmp163="union";_tag_dyneither(_tmp163,sizeof(char),6U);}):({const char*_tmp164="struct";_tag_dyneither(_tmp164,sizeof(char),7U);}));_tmp778.f1=_tmp806;});_tmp778;});struct Cyc_String_pa_PrintArg_struct _tmp162=({struct Cyc_String_pa_PrintArg_struct _tmp777;_tmp777.tag=0U,({
struct _dyneither_ptr _tmp807=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp777.f1=_tmp807;});_tmp777;});void*_tmp15F[2U];_tmp15F[0]=& _tmp161,_tmp15F[1]=& _tmp162;({struct _dyneither_ptr _tmp808=({const char*_tmp160="%s %s was never defined.";_tag_dyneither(_tmp160,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp808,_tag_dyneither(_tmp15F,sizeof(void*),2U));});});}
# 908
if(_tmp15E->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp165=_tmp15E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields;
if(_tmp165 == 0)return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp165))->tl != 0;_tmp165=_tmp165->tl){;}{
void*_tmp166=((struct Cyc_Absyn_Aggrfield*)_tmp165->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp166))){
if(_tmp15E->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp169=({struct Cyc_String_pa_PrintArg_struct _tmp779;_tmp779.tag=0U,({struct _dyneither_ptr _tmp809=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name));_tmp779.f1=_tmp809;});_tmp779;});void*_tmp167[1U];_tmp167[0]=& _tmp169;({struct _dyneither_ptr _tmp80A=({const char*_tmp168="struct %s ended up being abstract.";_tag_dyneither(_tmp168,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp80A,_tag_dyneither(_tmp167,sizeof(void*),1U));});});{
# 921
struct _RegionHandle _tmp16A=_new_region("r");struct _RegionHandle*r=& _tmp16A;_push_region(r);
{struct Cyc_List_List*_tmp16B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15E->tvs,_tmp185);
void*_tmp16C=Cyc_Tcutil_rsubstitute(r,_tmp16B,_tmp166);
if(Cyc_Toc_is_abstract_type(_tmp16C)){void*_tmp16D=Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp16D;}{
void*_tmp16E=Cyc_Toc_add_struct_type(_tmp15E->name,_tmp15E->tvs,_tmp185,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields);_npop_handler(0U);return _tmp16E;};}
# 922
;_pop_region(r);};}
# 927
return Cyc_Toc_aggrdecl_type(_tmp15E->name,Cyc_Absyn_strctq);};};};case 13U: _LL1D: _tmp187=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp149)->f1;_LL1E:
 return t;case 14U: _LL1F: _tmp188=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp149)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp188);return t;case 17U: _LL21: _tmp18C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f1;_tmp18B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f2;_tmp18A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f3;_tmp189=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp149)->f4;_LL22:
# 933
 if(_tmp189 == 0){
if(_tmp18B != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->tag=17U,_tmp16F->f1=_tmp18C,_tmp16F->f2=0,_tmp16F->f3=_tmp18A,_tmp16F->f4=0;_tmp16F;});else{
return t;}}else{
# 938
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=17U,_tmp170->f1=_tmp18C,_tmp170->f2=0,_tmp170->f3=_tmp18A,({void*_tmp80B=Cyc_Toc_typ_to_c(_tmp189);_tmp170->f4=_tmp80B;});_tmp170;});}case 19U: _LL23: _LL24:
 return Cyc_Absyn_uint_typ;case 15U: _LL25: _LL26:
 return Cyc_Toc_rgn_typ();case 16U: _LL27: _LL28:
 return Cyc_Toc_dyn_rgn_typ();case 20U: _LL29: _LL2A:
# 944
 goto _LL2C;case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 23U: _LL2F: _LL30:
 goto _LL32;case 24U: _LL31: _LL32:
 goto _LL34;case 25U: _LL33: _LL34:
 return Cyc_Absyn_void_star_typ();case 18U: _LL35: _tmp18D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp149)->f1;_LL36:
# 954
 return t;case 27U: _LL37: _tmp18E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp149)->f1;_LL38:
# 956
 return t;case 28U: _LL39: _LL3A:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)){case 0U: _LL3B: _tmp18F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3C:
# 959
 Cyc_Toc_aggrdecl_to_c(_tmp18F,1);
if(_tmp18F->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp18F->name,Cyc_Absyn_strctq);}case 1U: _LL3D: _tmp190=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_LL3E:
# 964
 Cyc_Toc_enumdecl_to_c(_tmp190);
return t;default: _LL3F: _tmp192=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f1)->r)->f1;_tmp191=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp149)->f2;_LL40:
# 967
 Cyc_Toc_datatypedecl_to_c(_tmp192);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp191)));}}_LL0:;}
# 972
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp193=t;void*_tmp195;struct Cyc_Absyn_Tqual _tmp194;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->tag == 8U){_LL1: _tmp195=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).elt_type;_tmp194=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp193)->f1).tq;_LL2:
# 975
 return({void*_tmp80C=Cyc_Absyn_star_typ(_tmp195,(void*)& Cyc_Absyn_HeapRgn_val,_tmp194,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp80C,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 982
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp196=Cyc_Tcutil_compress(t);void*_tmp197=_tmp196;struct Cyc_List_List*_tmp1A3;struct Cyc_Absyn_Datatypedecl*_tmp1A2;struct Cyc_Absyn_Datatypefield*_tmp1A1;struct Cyc_List_List*_tmp1A0;union Cyc_Absyn_AggrInfoU _tmp19F;void*_tmp19E;switch(*((int*)_tmp197)){case 0U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 0;case 6U: _LL5: _LL6:
 goto _LL8;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 9U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmp19E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).elt_type;_LL12:
 return Cyc_Toc_atomic_typ(_tmp19E);case 11U: _LL13: _tmp19F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp197)->f1).aggr_info;_LL14:
# 999
{union Cyc_Absyn_AggrInfoU _tmp198=_tmp19F;if((_tmp198.UnknownAggr).tag == 1){_LL26: _LL27:
 return 0;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 1003
struct Cyc_Absyn_Aggrdecl*_tmp199=Cyc_Absyn_get_known_aggrdecl(_tmp19F);
if(_tmp199->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp199->impl))->fields;for(0;_tmp19A != 0;_tmp19A=_tmp19A->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp19A->hd)->type))return 0;}}
return 1;};case 12U: _LL15: _tmp1A0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp197)->f2;_LL16:
# 1010
 for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1A0->hd)->type))return 0;}
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).tag == 2){_LL17: _tmp1A2=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f1;_tmp1A1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp197)->f1).field_info).KnownDatatypefield).val).f2;_LL18:
# 1014
 _tmp1A3=_tmp1A1->typs;goto _LL1A;}else{goto _LL23;}case 10U: _LL19: _tmp1A3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp197)->f1;_LL1A:
# 1016
 for(0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple12*)_tmp1A3->hd)).f2))return 0;}
return 1;case 3U: _LL1B: _LL1C:
# 1021
 goto _LL1E;case 5U: _LL1D: _LL1E:
 goto _LL20;case 16U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return 0;default: _LL23: _LL24:
({struct Cyc_String_pa_PrintArg_struct _tmp19D=({struct Cyc_String_pa_PrintArg_struct _tmp77A;_tmp77A.tag=0U,({struct _dyneither_ptr _tmp80D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp77A.f1=_tmp80D;});_tmp77A;});void*_tmp19B[1U];_tmp19B[0]=& _tmp19D;({struct _dyneither_ptr _tmp80E=({const char*_tmp19C="atomic_typ:  bad type %s";_tag_dyneither(_tmp19C,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80E,_tag_dyneither(_tmp19B,sizeof(void*),1U));});});}_LL0:;}
# 1029
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A4=Cyc_Tcutil_compress(t);void*_tmp1A5=_tmp1A4;void*_tmp1A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->tag == 5U){_LL1: _tmp1A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5)->f1).elt_typ;_LL2: {
# 1032
void*_tmp1A6=Cyc_Tcutil_compress(_tmp1A8);void*_tmp1A7=_tmp1A6;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A7)->tag == 0U){_LL6: _LL7:
 return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 1036
 return 0;}_LL0:;}
# 1040
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1044
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1049
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1A9=Cyc_Tcutil_compress(t);void*_tmp1AA=_tmp1A9;struct Cyc_List_List*_tmp1B6;union Cyc_Absyn_AggrInfoU _tmp1B5;switch(*((int*)_tmp1AA)){case 11U: _LL1: _tmp1B5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AA)->f1).aggr_info;_LL2: {
# 1052
struct Cyc_Absyn_Aggrdecl*_tmp1AB=Cyc_Absyn_get_known_aggrdecl(_tmp1B5);
if(_tmp1AB->impl == 0)
({void*_tmp1AC=0U;({struct _dyneither_ptr _tmp80F=({const char*_tmp1AD="is_poly_field: type missing fields";_tag_dyneither(_tmp1AD,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp80F,_tag_dyneither(_tmp1AC,sizeof(void*),0U));});});
_tmp1B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AB->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp1B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AA)->f2;_LL4: {
# 1057
struct Cyc_Absyn_Aggrfield*_tmp1AE=Cyc_Absyn_lookup_field(_tmp1B6,f);
if(_tmp1AE == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B1=({struct Cyc_String_pa_PrintArg_struct _tmp77B;_tmp77B.tag=0U,_tmp77B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp77B;});void*_tmp1AF[1U];_tmp1AF[0]=& _tmp1B1;({struct _dyneither_ptr _tmp810=({const char*_tmp1B0="is_poly_field: bad field %s";_tag_dyneither(_tmp1B0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp810,_tag_dyneither(_tmp1AF,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1AE->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U,({struct _dyneither_ptr _tmp811=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp77C.f1=_tmp811;});_tmp77C;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct _dyneither_ptr _tmp812=({const char*_tmp1B3="is_poly_field: bad type %s";_tag_dyneither(_tmp1B3,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp812,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});}_LL0:;}
# 1068
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1B7=e->r;void*_tmp1B8=_tmp1B7;struct Cyc_Absyn_Exp*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct _dyneither_ptr*_tmp1BF;switch(*((int*)_tmp1B8)){case 21U: _LL1: _tmp1C0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1BF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL2:
# 1071
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C0->topt),_tmp1BF) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f1;_tmp1C1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1B8)->f2;_LL4: {
# 1074
void*_tmp1B9=Cyc_Tcutil_compress((void*)_check_null(_tmp1C2->topt));void*_tmp1BA=_tmp1B9;void*_tmp1BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->tag == 5U){_LL8: _tmp1BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BA)->f1).elt_typ;_LL9:
# 1076
 return Cyc_Toc_is_poly_field(_tmp1BE,_tmp1C1) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp77D;_tmp77D.tag=0U,({struct _dyneither_ptr _tmp813=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C2->topt)));_tmp77D.f1=_tmp813;});_tmp77D;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct _dyneither_ptr _tmp814=({const char*_tmp1BC="is_poly_project: bad type %s";_tag_dyneither(_tmp1BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp814,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1079
 return 0;}_LL0:;}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp815=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp815,({struct Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=s,_tmp1C3->tl=0;_tmp1C3;}),0U);});}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp816=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp816,({struct Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=s,_tmp1C4->tl=0;_tmp1C4;}),0U);});}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1098
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp817=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp817,({struct Cyc_Absyn_Exp*_tmp1C5[2U];_tmp1C5[0]=rgn,_tmp1C5[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C5,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp818=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp818,({struct Cyc_Absyn_Exp*_tmp1C6[2U];_tmp1C6[0]=rgn,_tmp1C6[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C6,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp819=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp819,({struct Cyc_Absyn_Exp*_tmp1C7[2U];_tmp1C7[0]=s,_tmp1C7[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1110
return({struct Cyc_Absyn_Exp*_tmp81A=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp81A,({struct Cyc_Absyn_Exp*_tmp1C8[2U];_tmp1C8[0]=s,_tmp1C8[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1113
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp81B=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp81B,({struct Cyc_Absyn_Exp*_tmp1C9[3U];_tmp1C9[0]=rgn,_tmp1C9[1]=s,_tmp1C9[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp81C=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp81C,({struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=e,_tmp1CA->tl=0;_tmp1CA;}),0U);});}
# 1120
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp81D=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp81D,({struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e,_tmp1CB->tl=0;_tmp1CB;}),0U);});}
# 1124
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp81E=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp81E,({void*_tmp1CC=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1129
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1137
int is_string=0;
{void*_tmp1CD=e->r;void*_tmp1CE=_tmp1CD;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1CE)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1143
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp820=({struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Absyn_Decl*_tmp81F=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->tag=0U,_tmp1CF->f1=vd;_tmp1CF;}),0U);_tmp1D0->hd=_tmp81F;}),_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});Cyc_Toc_result_decls=_tmp820;});
({struct Cyc_Absyn_Exp*_tmp821=Cyc_Absyn_var_exp(x,0U);xexp=_tmp821;});
({struct Cyc_Absyn_Exp*_tmp822=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp822;});}else{
# 1153
({struct Cyc_Absyn_Exp*_tmp824=({void*_tmp823=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp823,e);});xexp=_tmp824;});
# 1155
({struct Cyc_Absyn_Exp*_tmp826=({void*_tmp825=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp825,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp826;});}
# 1157
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D1[3U];({struct _tuple20*_tmp829=({struct _tuple20*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->f1=0,_tmp1D2->f2=xexp;_tmp1D2;});_tmp1D1[0]=_tmp829;}),({
struct _tuple20*_tmp828=({struct _tuple20*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=0,_tmp1D3->f2=xexp;_tmp1D3;});_tmp1D1[1]=_tmp828;}),({
struct _tuple20*_tmp827=({struct _tuple20*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->f1=0,_tmp1D4->f2=xplussz;_tmp1D4;});_tmp1D1[2]=_tmp827;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D1,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1194 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D5=nv;int _tmp1D6;_LL1: _tmp1D6=_tmp1D5->toplevel;_LL2:;
return _tmp1D6;}
# 1198
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D7=nv;int*_tmp1D8;_LL1: _tmp1D8=_tmp1D7->in_lhs;_LL2:;
return*_tmp1D8;}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DB=_region_malloc(r,sizeof(*_tmp1DB));_tmp1DB->break_lab=0,_tmp1DB->continue_lab=0,_tmp1DB->fallthru_info=0,_tmp1DB->toplevel=1,({
# 1209
int*_tmp82B=({int*_tmp1D9=_region_malloc(r,sizeof(*_tmp1D9));*_tmp1D9=0;_tmp1D9;});_tmp1DB->in_lhs=_tmp82B;}),({
int*_tmp82A=({int*_tmp1DA=_region_malloc(r,sizeof(*_tmp1DA));*_tmp1DA=0;_tmp1DA;});_tmp1DB->in_sizeof=_tmp82A;}),_tmp1DB->rgn=r;_tmp1DB;});}
# 1214
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1DC=e;struct _dyneither_ptr**_tmp1E3;struct _dyneither_ptr**_tmp1E2;struct Cyc_Toc_FallthruInfo*_tmp1E1;int _tmp1E0;int*_tmp1DF;int*_tmp1DE;_LL1: _tmp1E3=_tmp1DC->break_lab;_tmp1E2=_tmp1DC->continue_lab;_tmp1E1=_tmp1DC->fallthru_info;_tmp1E0=_tmp1DC->toplevel;_tmp1DF=_tmp1DC->in_lhs;_tmp1DE=_tmp1DC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=_tmp1E3,_tmp1DD->continue_lab=_tmp1E2,_tmp1DD->fallthru_info=_tmp1E1,_tmp1DD->toplevel=_tmp1E0,_tmp1DD->in_lhs=_tmp1DF,_tmp1DD->in_sizeof=_tmp1DE,_tmp1DD->rgn=r;_tmp1DD;});}
# 1219
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E4=e;struct _dyneither_ptr**_tmp1EB;struct _dyneither_ptr**_tmp1EA;struct Cyc_Toc_FallthruInfo*_tmp1E9;int _tmp1E8;int*_tmp1E7;int*_tmp1E6;_LL1: _tmp1EB=_tmp1E4->break_lab;_tmp1EA=_tmp1E4->continue_lab;_tmp1E9=_tmp1E4->fallthru_info;_tmp1E8=_tmp1E4->toplevel;_tmp1E7=_tmp1E4->in_lhs;_tmp1E6=_tmp1E4->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E5=_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->break_lab=_tmp1EB,_tmp1E5->continue_lab=_tmp1EA,_tmp1E5->fallthru_info=_tmp1E9,_tmp1E5->toplevel=0,_tmp1E5->in_lhs=_tmp1E7,_tmp1E5->in_sizeof=_tmp1E6,_tmp1E5->rgn=r;_tmp1E5;});}
# 1223
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1EC=e;struct _dyneither_ptr**_tmp1F3;struct _dyneither_ptr**_tmp1F2;struct Cyc_Toc_FallthruInfo*_tmp1F1;int _tmp1F0;int*_tmp1EF;int*_tmp1EE;_LL1: _tmp1F3=_tmp1EC->break_lab;_tmp1F2=_tmp1EC->continue_lab;_tmp1F1=_tmp1EC->fallthru_info;_tmp1F0=_tmp1EC->toplevel;_tmp1EF=_tmp1EC->in_lhs;_tmp1EE=_tmp1EC->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=_tmp1F3,_tmp1ED->continue_lab=_tmp1F2,_tmp1ED->fallthru_info=_tmp1F1,_tmp1ED->toplevel=1,_tmp1ED->in_lhs=_tmp1EF,_tmp1ED->in_sizeof=_tmp1EE,_tmp1ED->rgn=r;_tmp1ED;});}
# 1227
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F4=e;int*_tmp1F5;_LL1: _tmp1F5=_tmp1F4->in_lhs;_LL2:;
*_tmp1F5=b;}
# 1231
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F6=e;int*_tmp1F8;_LL1: _tmp1F8=_tmp1F6->in_sizeof;_LL2:;{
int _tmp1F7=*_tmp1F8;
*_tmp1F8=b;
return _tmp1F7;};}
# 1237
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F9=e;int*_tmp1FA;_LL1: _tmp1FA=_tmp1F9->in_sizeof;_LL2:;
return*_tmp1FA;}
# 1244
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FB=e;struct _dyneither_ptr**_tmp202;struct _dyneither_ptr**_tmp201;struct Cyc_Toc_FallthruInfo*_tmp200;int _tmp1FF;int*_tmp1FE;int*_tmp1FD;_LL1: _tmp202=_tmp1FB->break_lab;_tmp201=_tmp1FB->continue_lab;_tmp200=_tmp1FB->fallthru_info;_tmp1FF=_tmp1FB->toplevel;_tmp1FE=_tmp1FB->in_lhs;_tmp1FD=_tmp1FB->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));_tmp1FC->break_lab=0,_tmp1FC->continue_lab=0,_tmp1FC->fallthru_info=_tmp200,_tmp1FC->toplevel=_tmp1FF,_tmp1FC->in_lhs=_tmp1FE,_tmp1FC->in_sizeof=_tmp1FD,_tmp1FC->rgn=r;_tmp1FC;});}
# 1250
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1254
struct Cyc_Toc_Env*_tmp203=e;struct _dyneither_ptr**_tmp20C;struct _dyneither_ptr**_tmp20B;struct Cyc_Toc_FallthruInfo*_tmp20A;int _tmp209;int*_tmp208;int*_tmp207;_LL1: _tmp20C=_tmp203->break_lab;_tmp20B=_tmp203->continue_lab;_tmp20A=_tmp203->fallthru_info;_tmp209=_tmp203->toplevel;_tmp208=_tmp203->in_lhs;_tmp207=_tmp203->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp206=_region_malloc(r,sizeof(*_tmp206));
_tmp206->label=fallthru_l,_tmp206->binders=fallthru_binders;_tmp206;});
return({struct Cyc_Toc_Env*_tmp205=_region_malloc(r,sizeof(*_tmp205));({struct _dyneither_ptr**_tmp82C=({struct _dyneither_ptr**_tmp204=_region_malloc(r,sizeof(*_tmp204));*_tmp204=break_l;_tmp204;});_tmp205->break_lab=_tmp82C;}),_tmp205->continue_lab=_tmp20B,_tmp205->fallthru_info=fi,_tmp205->toplevel=_tmp209,_tmp205->in_lhs=_tmp208,_tmp205->in_sizeof=_tmp207,_tmp205->rgn=r;_tmp205;});};}
# 1260
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1262
struct Cyc_Toc_Env*_tmp20D=e;struct _dyneither_ptr**_tmp215;struct _dyneither_ptr**_tmp214;struct Cyc_Toc_FallthruInfo*_tmp213;int _tmp212;int*_tmp211;int*_tmp210;_LL1: _tmp215=_tmp20D->break_lab;_tmp214=_tmp20D->continue_lab;_tmp213=_tmp20D->fallthru_info;_tmp212=_tmp20D->toplevel;_tmp211=_tmp20D->in_lhs;_tmp210=_tmp20D->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp20F=_region_malloc(r,sizeof(*_tmp20F));({struct _dyneither_ptr**_tmp82D=({struct _dyneither_ptr**_tmp20E=_region_malloc(r,sizeof(*_tmp20E));*_tmp20E=break_l;_tmp20E;});_tmp20F->break_lab=_tmp82D;}),_tmp20F->continue_lab=_tmp214,_tmp20F->fallthru_info=0,_tmp20F->toplevel=_tmp212,_tmp20F->in_lhs=_tmp211,_tmp20F->in_sizeof=_tmp210,_tmp20F->rgn=r;_tmp20F;});}
# 1269
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1272
struct Cyc_Toc_Env*_tmp216=e;struct _dyneither_ptr**_tmp21E;struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21C;int _tmp21B;int*_tmp21A;int*_tmp219;_LL1: _tmp21E=_tmp216->break_lab;_tmp21D=_tmp216->continue_lab;_tmp21C=_tmp216->fallthru_info;_tmp21B=_tmp216->toplevel;_tmp21A=_tmp216->in_lhs;_tmp219=_tmp216->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp218=_region_malloc(r,sizeof(*_tmp218));_tmp218->break_lab=0,_tmp218->continue_lab=_tmp21D,({struct Cyc_Toc_FallthruInfo*_tmp82E=({struct Cyc_Toc_FallthruInfo*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->label=next_l,_tmp217->binders=0;_tmp217;});_tmp218->fallthru_info=_tmp82E;}),_tmp218->toplevel=_tmp21B,_tmp218->in_lhs=_tmp21A,_tmp218->in_sizeof=_tmp219,_tmp218->rgn=r;_tmp218;});}
# 1285 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1288
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1290
void*_tmp21F=e->annot;void*_tmp220=_tmp21F;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1293
void*_tmp221=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp222=_tmp221;union Cyc_Absyn_Constraint*_tmp225;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->tag == 5U){_LLC: _tmp225=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp222)->f1).ptr_atts).nullable;_LLD:
# 1296
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp225);}else{_LLE: _LLF:
({void*_tmp223=0U;({struct _dyneither_ptr _tmp82F=({const char*_tmp224="Toc::is_nullable";_tag_dyneither(_tmp224,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp82F,_tag_dyneither(_tmp223,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1301
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1303
({void*_tmp226=0U;({unsigned int _tmp831=e->loc;struct _dyneither_ptr _tmp830=({const char*_tmp227="NULL pointer check will definitely fail";_tag_dyneither(_tmp227,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp831,_tmp830,_tag_dyneither(_tmp226,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp220)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1309
 return 1;}else{_LL9: _LLA:
({void*_tmp228=0U;({struct _dyneither_ptr _tmp832=({const char*_tmp229="need_null_check";_tag_dyneither(_tmp229,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp832,_tag_dyneither(_tmp228,sizeof(void*),0U));});});}}}}_LL0:;}
# 1314
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp22A=e->annot;void*_tmp22B=_tmp22A;struct Cyc_List_List*_tmp231;struct Cyc_List_List*_tmp230;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp230=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL2:
 return _tmp230;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp231=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22B)->f1;_LL4:
 return _tmp231;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1319
({void*_tmp22C=0U;({unsigned int _tmp834=e->loc;struct _dyneither_ptr _tmp833=({const char*_tmp22D="dereference of NULL pointer";_tag_dyneither(_tmp22D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp834,_tmp833,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp22B)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp22E=0U;({struct _dyneither_ptr _tmp835=({const char*_tmp22F="get_relns";_tag_dyneither(_tmp22F,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp835,_tag_dyneither(_tmp22E,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp235[8U]="*bogus*";
# 1329
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1339 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1342
struct Cyc_Absyn_Vardecl*x;
{void*_tmp232=a->r;void*_tmp233=_tmp232;void*_tmp236;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->tag == 1U){_LL1: _tmp236=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp233)->f1;_LL2: {
# 1345
struct Cyc_Absyn_Vardecl*_tmp234=Cyc_Tcutil_nonesc_vardecl(_tmp236);
if(_tmp234 == 0)goto _LL4;
x=_tmp234;
goto _LL0;}}else{_LL3: _LL4: {
# 1351
static struct _dyneither_ptr bogus_string={_tmp235,_tmp235,_tmp235 + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1354
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1360
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1363
void*_tmp237=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1368
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp237);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp836=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp836;});}
# 1377
if(valid_rop_i){
struct Cyc_List_List*_tmp238=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1385
if(!Cyc_Relations_consistent_relations(_tmp238))return 1;}
# 1388
inner_loop: {
void*_tmp239=i->r;void*_tmp23A=_tmp239;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_Exp*_tmp23E;void*_tmp23D;struct Cyc_Absyn_Exp*_tmp23C;switch(*((int*)_tmp23A)){case 14U: _LL6: _tmp23D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23A)->f1;_tmp23C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23A)->f2;_LL7:
 i=_tmp23C;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23A)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23A)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23A)->f2)->tl != 0){_LL8: _tmp23F=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23A)->f2)->hd;_tmp23E=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23A)->f2)->tl)->hd;_LL9: {
# 1394
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp23E,& rop_z)){
# 1397
struct Cyc_List_List*_tmp23B=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1399
return !Cyc_Relations_consistent_relations(_tmp23B);}
# 1401
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1404
return 0;};}
# 1407
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp240=0U;({struct _dyneither_ptr _tmp837=({const char*_tmp241="Missing type in primop ";_tag_dyneither(_tmp241,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp837,_tag_dyneither(_tmp240,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1411
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->f1=Cyc_Toc_mt_tq,({void*_tmp838=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp242->f2=_tmp838;});_tmp242;});}
# 1416
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp243=e->r;void*_tmp244=_tmp243;struct Cyc_Absyn_Exp*_tmp24C;int _tmp24B;struct Cyc_Absyn_Exp*_tmp24A;int _tmp249;struct Cyc_List_List*_tmp248;switch(*((int*)_tmp244)){case 26U: _LL1: _tmp248=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp244)->f1;_LL2:
# 1419
{struct Cyc_List_List*_tmp245=_tmp248;for(0;_tmp245 != 0;_tmp245=_tmp245->tl){
if((*((struct _tuple20*)_tmp245->hd)).f1 != 0)
({void*_tmp246=0U;({struct _dyneither_ptr _tmp839=({const char*_tmp247="array designators for abstract-field initialization";_tag_dyneither(_tmp247,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp839,_tag_dyneither(_tmp246,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp83A=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp248),0U);_tmp24A=_tmp83A;});_tmp249=0;goto _LL4;case 27U: _LL3: _tmp24A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp244)->f2;_tmp249=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp244)->f4;_LL4:
 _tmp24C=_tmp24A;_tmp24B=_tmp249;goto _LL6;case 28U: _LL5: _tmp24C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp244)->f1;_tmp24B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp244)->f3;_LL6:
# 1425
({struct Cyc_Absyn_Exp*_tmp83B=Cyc_Absyn_copy_exp(_tmp24C);_tmp24C=_tmp83B;});
return _tmp24B?({struct Cyc_Absyn_Exp*_tmp83C=_tmp24C;Cyc_Absyn_add_exp(_tmp83C,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp24C;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1434
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1442
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp24D=e->r;void*_tmp24E=_tmp24D;struct Cyc_List_List*_tmp24F;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24E)->tag == 29U){_LL1: _tmp24F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24E)->f3;_LL2:
 dles=_tmp24F;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1448
{void*_tmp250=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp251=_tmp250;struct Cyc_Absyn_Aggrdecl*_tmp252;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp252=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).val;_LL7:
# 1450
 if(_tmp252->impl == 0)
return 0;
if(_tmp252->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp252->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{_LL8: _LL9:
# 1458
 return 0;}_LL5:;}
# 1460
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp253=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp254=(struct _tuple20*)dles->hd;struct _tuple20*_tmp255=_tmp254;struct Cyc_List_List*_tmp25E;struct Cyc_Absyn_Exp*_tmp25D;_LLB: _tmp25E=_tmp255->f1;_tmp25D=_tmp255->f2;_LLC:;{
struct _dyneither_ptr*_tmp256=Cyc_Absyn_designatorlist_to_fieldname(_tmp25E);
if(!Cyc_strptrcmp(_tmp256,_tmp253->name)){
struct Cyc_Absyn_Exp*_tmp257=Cyc_Toc_get_varsizeexp(_tmp25D);
if(_tmp257 != 0)
return _tmp257;{
void*_tmp258=Cyc_Tcutil_compress(_tmp253->type);void*_tmp259=_tmp258;void*_tmp25C;struct Cyc_Absyn_Exp*_tmp25B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp259)->tag == 8U){_LLE: _tmp25C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp259)->f1).elt_type;_tmp25B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp259)->f1).num_elts;_LLF:
# 1475
 if(_tmp25B == 0  || !Cyc_Toc_is_zero(_tmp25B))
return 0;
# 1481
return({struct Cyc_Absyn_Exp*_tmp83F=({struct Cyc_Absyn_Exp*_tmp83E=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp83E,({struct Cyc_Absyn_Exp*_tmp25A[2U];_tmp25A[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp25D)),({
# 1483
struct Cyc_Absyn_Exp*_tmp83D=Cyc_Absyn_sizeoftyp_exp(_tmp25C,0U);_tmp25A[1]=_tmp83D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25A,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1481
Cyc_Absyn_add_exp(_tmp83F,
# 1484
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1489
({void*_tmp25F=0U;({struct _dyneither_ptr _tmp840=({const char*_tmp260="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp260,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp840,_tag_dyneither(_tmp25F,sizeof(void*),0U));});});};}
# 1492
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp261=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
struct Cyc_Absyn_Aggrfield*_tmp262=(struct Cyc_Absyn_Aggrfield*)_tmp261->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp262->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1499
({void*_tmp263=0U;({struct _dyneither_ptr _tmp842=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp266=({struct Cyc_String_pa_PrintArg_struct _tmp77E;_tmp77E.tag=0U,_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp77E;});void*_tmp264[1U];_tmp264[0]=& _tmp266;({struct _dyneither_ptr _tmp841=({const char*_tmp265="get_member_offset %s failed";_tag_dyneither(_tmp265,sizeof(char),28U);});Cyc_aprintf(_tmp841,_tag_dyneither(_tmp264,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp842,_tag_dyneither(_tmp263,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1503
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1506
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1509
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->tag=5U,_tmp267->f1=e1,_tmp267->f2=incr;_tmp267;}),0U);}
# 1513
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1522
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1524
void*_tmp268=e1->r;void*_tmp269=_tmp268;struct Cyc_Absyn_Exp*_tmp271;struct _dyneither_ptr*_tmp270;int _tmp26F;int _tmp26E;void*_tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Stmt*_tmp26B;switch(*((int*)_tmp269)){case 37U: _LL1: _tmp26B=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26B,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp26D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_tmp26C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp269)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp26C,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp271=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f1;_tmp270=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f2;_tmp26F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f3;_tmp26E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp269)->f4;_LL6:
# 1529
 e1->r=_tmp271->r;
({struct Cyc_Absyn_Exp*_tmp845=e1;struct Cyc_List_List*_tmp844=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=_tmp270,_tmp26A->tl=fs;_tmp26A;});struct Cyc_Absyn_Exp*(*_tmp843)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp845,_tmp844,_tmp843,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1537
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1539
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp846=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp846;});}
({void*_tmp847=(f(e1_copy,f_env))->r;e1->r=_tmp847;});
goto _LL0;}}_LL0:;}
# 1545
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1547
void*_tmp272=s->r;void*_tmp273=_tmp272;struct Cyc_Absyn_Stmt*_tmp27A;struct Cyc_Absyn_Decl*_tmp279;struct Cyc_Absyn_Stmt*_tmp278;struct Cyc_Absyn_Exp*_tmp277;switch(*((int*)_tmp273)){case 1U: _LL1: _tmp277=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp273)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp277,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp279=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp273)->f1;_tmp278=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp273)->f2;_LL4:
# 1550
 Cyc_Toc_lvalue_assign_stmt(_tmp278,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp273)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27A,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp276=({struct Cyc_String_pa_PrintArg_struct _tmp77F;_tmp77F.tag=0U,({struct _dyneither_ptr _tmp848=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp77F.f1=_tmp848;});_tmp77F;});void*_tmp274[1U];_tmp274[0]=& _tmp276;({struct _dyneither_ptr _tmp849=({const char*_tmp275="lvalue_assign_stmt: %s";_tag_dyneither(_tmp275,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp849,_tag_dyneither(_tmp274,sizeof(void*),1U));});});}_LL0:;}
# 1556
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1560
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27B=e->r;void*_tmp27C=_tmp27B;struct Cyc_Absyn_Stmt*_tmp283;struct Cyc_Absyn_Exp*_tmp282;void**_tmp281;struct Cyc_Absyn_Exp**_tmp280;switch(*((int*)_tmp27C)){case 14U: _LL1: _tmp281=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27C)->f1;_tmp280=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp27C)->f2;_LL2:
# 1563
({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Toc_push_address_exp(*_tmp280);*_tmp280=_tmp84A;});
({void*_tmp84B=Cyc_Absyn_cstar_typ(*_tmp281,Cyc_Toc_mt_tq);*_tmp281=_tmp84B;});
return e;case 20U: _LL3: _tmp282=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp27C)->f1;_LL4:
# 1567
 return _tmp282;case 37U: _LL5: _tmp283=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp27C)->f1;_LL6:
# 1571
 Cyc_Toc_push_address_stmt(_tmp283);
return e;default: _LL7: _LL8:
# 1574
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp780;_tmp780.tag=0U,({struct _dyneither_ptr _tmp84C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp780.f1=_tmp84C;});_tmp780;});void*_tmp27D[1U];_tmp27D[0]=& _tmp27F;({struct _dyneither_ptr _tmp84D=({const char*_tmp27E="can't take & of exp %s";_tag_dyneither(_tmp27E,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp84D,_tag_dyneither(_tmp27D,sizeof(void*),1U));});});}_LL0:;}
# 1579
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp284=s->r;void*_tmp285=_tmp284;struct Cyc_Absyn_Exp**_tmp28B;struct Cyc_Absyn_Stmt*_tmp28A;struct Cyc_Absyn_Stmt*_tmp289;switch(*((int*)_tmp285)){case 2U: _LL1: _tmp289=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp285)->f2;_LL2:
 _tmp28A=_tmp289;goto _LL4;case 12U: _LL3: _tmp28A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp285)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28A);goto _LL0;case 1U: _LL5: _tmp28B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp285)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp84E=Cyc_Toc_push_address_exp(*_tmp28B);*_tmp28B=_tmp84E;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp781;_tmp781.tag=0U,({struct _dyneither_ptr _tmp84F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp781.f1=_tmp84F;});_tmp781;});void*_tmp286[1U];_tmp286[0]=& _tmp288;({struct _dyneither_ptr _tmp850=({const char*_tmp287="can't take & of stmt %s";_tag_dyneither(_tmp287,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp850,_tag_dyneither(_tmp286,sizeof(void*),1U));});});}_LL0:;}
# 1588
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp28C=Cyc_Tcutil_compress(t);void*_tmp28D=_tmp28C;struct Cyc_Absyn_PtrInfo _tmp290;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28D)->tag == 5U){_LL1: _tmp290=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28D)->f1;_LL2:
 return _tmp290;}else{_LL3: _LL4:
({void*_tmp28E=0U;({struct _dyneither_ptr _tmp851=({const char*_tmp28F="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp28F,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp851,_tag_dyneither(_tmp28E,sizeof(void*),0U));});});}_LL0:;}
# 1598
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp291=Cyc_Tcutil_compress(t);void*_tmp292=_tmp291;int _tmp29E;enum Cyc_Absyn_Sign _tmp29D;enum Cyc_Absyn_Sign _tmp29C;enum Cyc_Absyn_Sign _tmp29B;enum Cyc_Absyn_Sign _tmp29A;enum Cyc_Absyn_Sign _tmp299;switch(*((int*)_tmp292)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp852=Cyc_Absyn_null_exp(0U);res=_tmp852;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp292)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp299=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp292)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmp853=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp299,'\000'),0U);res=_tmp853;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp29A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp292)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp854=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29A,0),0U);res=_tmp854;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp29B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp292)->f1;_LLC:
# 1606
 _tmp29C=_tmp29B;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp29C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp292)->f1;_LLE:
# 1608
({struct Cyc_Absyn_Exp*_tmp855=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp29C,0),0U);res=_tmp855;});goto _LL0;default: _LLF: _tmp29D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp292)->f1;_LL10:
# 1610
({struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp29D,(long long)0),0U);res=_tmp856;});goto _LL0;}case 13U: _LL7: _LL8:
# 1604
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp29B=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp292)->f1){case 0U: _LL11: _LL12:
# 1611
({struct Cyc_Absyn_Exp*_tmp857=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp293="0.0F";_tag_dyneither(_tmp293,sizeof(char),5U);}),0),0U);res=_tmp857;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmp858=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp294="0.0";_tag_dyneither(_tmp294,sizeof(char),4U);}),1),0U);res=_tmp858;});goto _LL0;default: _LL15: _tmp29E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp292)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmp85A=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp859=({const char*_tmp295="0.0L";_tag_dyneither(_tmp295,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp859,_tmp29E);}),0U);res=_tmp85A;});goto _LL0;}default: _LL17: _LL18:
# 1615
({struct Cyc_String_pa_PrintArg_struct _tmp298=({struct Cyc_String_pa_PrintArg_struct _tmp782;_tmp782.tag=0U,({struct _dyneither_ptr _tmp85B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp782.f1=_tmp85B;});_tmp782;});void*_tmp296[1U];_tmp296[0]=& _tmp298;({struct _dyneither_ptr _tmp85C=({const char*_tmp297="found non-zero type %s in generate_zero";_tag_dyneither(_tmp297,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp85C,_tag_dyneither(_tmp296,sizeof(void*),1U));});});}_LL0:;}
# 1617
res->topt=t;
return res;}
# 1624
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1637 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp29F=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A0=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A1=Cyc_Absyn_cstar_typ(_tmp29F,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A2=({struct Cyc_Core_Opt*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->v=_tmp2A1;_tmp2D3;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A3=e1->r;void*_tmp2A4=_tmp2A3;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2A9;struct Cyc_Absyn_Exp*_tmp2A8;switch(*((int*)_tmp2A4)){case 20U: _LL1: _tmp2A8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A4)->f1;_LL2:
# 1645
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp85D=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2A8,0,Cyc_Absyn_Other_coercion,0U);_tmp2A8=_tmp85D;});
_tmp2A8->topt=fat_ptr_type;}
# 1649
Cyc_Toc_exp_to_c(nv,_tmp2A8);xinit=_tmp2A8;goto _LL0;case 23U: _LL3: _tmp2AA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A4)->f1;_tmp2A9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A4)->f2;_LL4:
# 1651
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp85E=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AA,0,Cyc_Absyn_Other_coercion,0U);_tmp2AA=_tmp85E;});
_tmp2AA->topt=fat_ptr_type;}
# 1655
Cyc_Toc_exp_to_c(nv,_tmp2AA);Cyc_Toc_exp_to_c(nv,_tmp2A9);
({struct Cyc_Absyn_Exp*_tmp861=({struct Cyc_Absyn_Exp*_tmp860=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp860,({struct Cyc_Absyn_Exp*_tmp2A5[3U];_tmp2A5[0]=_tmp2AA,({
struct Cyc_Absyn_Exp*_tmp85F=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A5[1]=_tmp85F;}),_tmp2A5[2]=_tmp2A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A5,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1656
xinit=_tmp861;});
# 1658
goto _LL0;default: _LL5: _LL6:
({void*_tmp2A6=0U;({struct _dyneither_ptr _tmp862=({const char*_tmp2A7="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2A7,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp862,_tag_dyneither(_tmp2A6,sizeof(void*),0U));});});}_LL0:;}{
# 1661
struct _tuple1*_tmp2AB=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2AC=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2AC;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2AD=({struct Cyc_Absyn_Vardecl*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->sc=Cyc_Absyn_Public,_tmp2D2->name=_tmp2AB,_tmp2D2->varloc=0U,_tmp2D2->tq=Cyc_Toc_mt_tq,_tmp2D2->type=_tmp2A0,_tmp2D2->initializer=xinit,_tmp2D2->rgn=0,_tmp2D2->attributes=0,_tmp2D2->escapes=0;_tmp2D2;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2AE=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->tag=4U,_tmp2D1->f1=_tmp2AD;_tmp2D1;});
struct Cyc_Absyn_Exp*_tmp2AF=Cyc_Absyn_varb_exp((void*)_tmp2AE,0U);
_tmp2AF->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_deref_exp(_tmp2AF,0U);
_tmp2B0->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp2B0);{
struct _tuple1*_tmp2B1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B2=({struct Cyc_Absyn_Vardecl*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->sc=Cyc_Absyn_Public,_tmp2D0->name=_tmp2B1,_tmp2D0->varloc=0U,_tmp2D0->tq=Cyc_Toc_mt_tq,_tmp2D0->type=_tmp29F,_tmp2D0->initializer=_tmp2B0,_tmp2D0->rgn=0,_tmp2D0->attributes=0,_tmp2D0->escapes=0;_tmp2D0;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B3=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF->tag=4U,_tmp2CF->f1=_tmp2B2;_tmp2CF;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_varb_exp((void*)_tmp2B3,0U);
_tmp2B4->topt=_tmp2B0->topt;
({struct Cyc_Absyn_Exp*_tmp863=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B4,e2,0U);z_init=_tmp863;});
z_init->topt=_tmp2B4->topt;}
# 1680
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2B5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B6=({struct Cyc_Absyn_Vardecl*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->sc=Cyc_Absyn_Public,_tmp2CE->name=_tmp2B5,_tmp2CE->varloc=0U,_tmp2CE->tq=Cyc_Toc_mt_tq,_tmp2CE->type=_tmp29F,_tmp2CE->initializer=z_init,_tmp2CE->rgn=0,_tmp2CE->attributes=0,_tmp2CE->escapes=0;_tmp2CE;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B7=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD->tag=4U,_tmp2CD->f1=_tmp2B6;_tmp2CD;});
# 1686
struct Cyc_Absyn_Exp*_tmp2B8=Cyc_Absyn_varb_exp((void*)_tmp2B3,0U);_tmp2B8->topt=_tmp2B0->topt;{
struct Cyc_Absyn_Exp*_tmp2B9=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BA=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2B8,_tmp2B9,0U);
_tmp2BA->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BA);{
# 1692
struct Cyc_Absyn_Exp*_tmp2BB=Cyc_Absyn_varb_exp((void*)_tmp2B7,0U);_tmp2BB->topt=_tmp2B0->topt;{
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2BB,_tmp2BC,0U);
_tmp2BD->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BD);{
# 1698
struct Cyc_List_List*_tmp2BE=({struct Cyc_Absyn_Exp*_tmp2CC[2U];({struct Cyc_Absyn_Exp*_tmp865=Cyc_Absyn_varb_exp((void*)_tmp2AE,0U);_tmp2CC[0]=_tmp865;}),({
struct Cyc_Absyn_Exp*_tmp864=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2CC[1]=_tmp864;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2CC,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp867=({struct Cyc_Absyn_Exp*_tmp866=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2BE,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp866,_tmp2BF,0U);});xsize=_tmp867;});{
# 1705
struct Cyc_Absyn_Exp*_tmp2C0=({struct Cyc_Absyn_Exp*_tmp868=xsize;Cyc_Absyn_and_exp(_tmp868,Cyc_Absyn_and_exp(_tmp2BA,_tmp2BD,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C1=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C2=({struct Cyc_Absyn_Exp*_tmp869=Cyc_Absyn_varb_exp((void*)_tmp2AE,0U);Cyc_Toc_member_exp(_tmp869,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp86A=Cyc_Toc_cast_it(_tmp2A1,_tmp2C2);_tmp2C2=_tmp86A;});{
struct Cyc_Absyn_Exp*_tmp2C3=Cyc_Absyn_deref_exp(_tmp2C2,0U);
struct Cyc_Absyn_Exp*_tmp2C4=({struct Cyc_Absyn_Exp*_tmp86B=_tmp2C3;Cyc_Absyn_assign_exp(_tmp86B,Cyc_Absyn_var_exp(_tmp2B5,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C5=Cyc_Absyn_exp_stmt(_tmp2C4,0U);
({struct Cyc_Absyn_Stmt*_tmp86F=({struct Cyc_Absyn_Stmt*_tmp86E=({struct Cyc_Absyn_Exp*_tmp86D=_tmp2C0;struct Cyc_Absyn_Stmt*_tmp86C=_tmp2C1;Cyc_Absyn_ifthenelse_stmt(_tmp86D,_tmp86C,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp86E,_tmp2C5,0U);});_tmp2C5=_tmp86F;});
({struct Cyc_Absyn_Stmt*_tmp872=({struct Cyc_Absyn_Decl*_tmp871=({struct Cyc_Absyn_Decl*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));({void*_tmp870=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6->tag=0U,_tmp2C6->f1=_tmp2B6;_tmp2C6;});_tmp2C7->r=_tmp870;}),_tmp2C7->loc=0U;_tmp2C7;});Cyc_Absyn_decl_stmt(_tmp871,_tmp2C5,0U);});_tmp2C5=_tmp872;});
({struct Cyc_Absyn_Stmt*_tmp875=({struct Cyc_Absyn_Decl*_tmp874=({struct Cyc_Absyn_Decl*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));({void*_tmp873=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->tag=0U,_tmp2C8->f1=_tmp2B2;_tmp2C8;});_tmp2C9->r=_tmp873;}),_tmp2C9->loc=0U;_tmp2C9;});Cyc_Absyn_decl_stmt(_tmp874,_tmp2C5,0U);});_tmp2C5=_tmp875;});
({struct Cyc_Absyn_Stmt*_tmp878=({struct Cyc_Absyn_Decl*_tmp877=({struct Cyc_Absyn_Decl*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));({void*_tmp876=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->tag=0U,_tmp2CA->f1=_tmp2AD;_tmp2CA;});_tmp2CB->r=_tmp876;}),_tmp2CB->loc=0U;_tmp2CB;});Cyc_Absyn_decl_stmt(_tmp877,_tmp2C5,0U);});_tmp2C5=_tmp878;});
({void*_tmp879=Cyc_Toc_stmt_exp_r(_tmp2C5);e->r=_tmp879;});};};};};};};};};};}
# 1663
;_pop_region(rgn2);};}
# 1731 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1735
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D4=Cyc_Tcutil_compress(aggrtype);void*_tmp2D5=_tmp2D4;union Cyc_Absyn_AggrInfoU _tmp2DA;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->tag == 11U){_LL1: _tmp2DA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D5)->f1).aggr_info;_LL2:
# 1738
({struct Cyc_Absyn_Aggrdecl*_tmp87A=Cyc_Absyn_get_known_aggrdecl(_tmp2DA);ad=_tmp87A;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp2D6=0U;({struct _dyneither_ptr _tmp87D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D9=({struct Cyc_String_pa_PrintArg_struct _tmp783;_tmp783.tag=0U,({
struct _dyneither_ptr _tmp87B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp783.f1=_tmp87B;});_tmp783;});void*_tmp2D7[1U];_tmp2D7[0]=& _tmp2D9;({struct _dyneither_ptr _tmp87C=({const char*_tmp2D8="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2D8,sizeof(char),51U);});Cyc_aprintf(_tmp87C,_tag_dyneither(_tmp2D7,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp87D,_tag_dyneither(_tmp2D6,sizeof(void*),0U));});});}_LL0:;}{
# 1742
struct _tuple1*_tmp2DB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2DC=Cyc_Absyn_var_exp(_tmp2DB,0U);
struct Cyc_Absyn_Exp*_tmp2DD=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2DE=Cyc_Absyn_aggrarrow_exp(_tmp2DC,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2DF=Cyc_Absyn_neq_exp(_tmp2DE,_tmp2DD,0U);
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_aggrarrow_exp(_tmp2DC,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E1=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E0,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E2=({struct Cyc_Absyn_Exp*_tmp87F=_tmp2DF;struct Cyc_Absyn_Stmt*_tmp87E=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp87F,_tmp87E,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E3=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E4=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E5=({struct _tuple1*_tmp882=_tmp2DB;void*_tmp881=_tmp2E3;struct Cyc_Absyn_Exp*_tmp880=_tmp2E4;Cyc_Absyn_declare_stmt(_tmp882,_tmp881,_tmp880,Cyc_Absyn_seq_stmt(_tmp2E2,_tmp2E1,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2E5);}else{
# 1756
struct Cyc_Absyn_Exp*_tmp2E6=({struct Cyc_Absyn_Exp*_tmp883=aggrproj(_tmp2DC,f,0U);Cyc_Toc_member_exp(_tmp883,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2E7=Cyc_Absyn_neq_exp(_tmp2E6,_tmp2DD,0U);
struct Cyc_Absyn_Exp*_tmp2E8=({struct Cyc_Absyn_Exp*_tmp884=aggrproj(_tmp2DC,f,0U);Cyc_Toc_member_exp(_tmp884,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2E9=Cyc_Absyn_exp_stmt(_tmp2E8,0U);
struct Cyc_Absyn_Stmt*_tmp2EA=({struct Cyc_Absyn_Exp*_tmp886=_tmp2E7;struct Cyc_Absyn_Stmt*_tmp885=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp886,_tmp885,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2EB=({struct _tuple1*_tmp889=_tmp2DB;void*_tmp888=e1_c_type;struct Cyc_Absyn_Exp*_tmp887=e1;Cyc_Absyn_declare_stmt(_tmp889,_tmp888,_tmp887,Cyc_Absyn_seq_stmt(_tmp2EA,_tmp2E9,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EB);}};}
# 1766
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1769
void*_tmp2EC=Cyc_Tcutil_compress(t);void*_tmp2ED=_tmp2EC;union Cyc_Absyn_AggrInfoU _tmp2F4;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2ED)->tag == 11U){_LL1: _tmp2F4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2ED)->f1).aggr_info;_LL2: {
# 1771
struct Cyc_Absyn_Aggrdecl*_tmp2EE=Cyc_Absyn_get_known_aggrdecl(_tmp2F4);
({int _tmp88A=Cyc_Toc_get_member_offset(_tmp2EE,f);*f_tag=_tmp88A;});{
# 1774
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F2=({struct Cyc_String_pa_PrintArg_struct _tmp785;_tmp785.tag=0U,_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2EE->name).f2);_tmp785;});struct Cyc_String_pa_PrintArg_struct _tmp2F3=({struct Cyc_String_pa_PrintArg_struct _tmp784;_tmp784.tag=0U,_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp784;});void*_tmp2F0[2U];_tmp2F0[0]=& _tmp2F2,_tmp2F0[1]=& _tmp2F3;({struct _dyneither_ptr _tmp88B=({const char*_tmp2F1="_union_%s_%s";_tag_dyneither(_tmp2F1,sizeof(char),13U);});Cyc_aprintf(_tmp88B,_tag_dyneither(_tmp2F0,sizeof(void*),2U));});});
({void*_tmp88C=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2EF=_cycalloc(sizeof(*_tmp2EF));*_tmp2EF=str;_tmp2EF;}));*tagged_member_type=_tmp88C;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2EE->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1785
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1788
void*_tmp2F5=e->r;void*_tmp2F6=_tmp2F5;struct Cyc_Absyn_Exp*_tmp302;struct _dyneither_ptr*_tmp301;int*_tmp300;struct Cyc_Absyn_Exp*_tmp2FF;struct _dyneither_ptr*_tmp2FE;int*_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FC;switch(*((int*)_tmp2F6)){case 14U: _LL1: _tmp2FC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F6)->f2;_LL2:
({void*_tmp2F7=0U;({struct _dyneither_ptr _tmp88D=({const char*_tmp2F8="cast on lhs!";_tag_dyneither(_tmp2F8,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp88D,_tag_dyneither(_tmp2F7,sizeof(void*),0U));});});case 21U: _LL3: _tmp2FF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F6)->f1;_tmp2FE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F6)->f2;_tmp2FD=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F6)->f4;_LL4:
# 1791
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp2FF->topt),_tmp2FE,f_tag,tagged_member_type,clear_read,_tmp2FD);case 22U: _LL5: _tmp302=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F6)->f1;_tmp301=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F6)->f2;_tmp300=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F6)->f4;_LL6: {
# 1794
void*_tmp2F9=Cyc_Tcutil_compress((void*)_check_null(_tmp302->topt));void*_tmp2FA=_tmp2F9;void*_tmp2FB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->tag == 5U){_LLA: _tmp2FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_typ;_LLB:
# 1796
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2FB,_tmp301,f_tag,tagged_member_type,clear_read,_tmp300);}else{_LLC: _LLD:
# 1798
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1800
 return 0;}_LL0:;}
# 1813 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1817
struct _tuple1*_tmp303=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp303,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp88E=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp88E;});else{
# 1827
struct Cyc_Absyn_Exp*_tmp304=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp891=({struct Cyc_Absyn_Exp*_tmp890=_tmp304;struct Cyc_Absyn_Stmt*_tmp88F=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp890,_tmp88F,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp891;});}{
# 1830
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp894=_tmp303;void*_tmp893=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp892=
Cyc_Toc_push_address_exp(e1);
# 1830
Cyc_Absyn_declare_stmt(_tmp894,_tmp893,_tmp892,
# 1832
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1836
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp305=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0U,({struct _dyneither_ptr _tmp895=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp786.f1=_tmp895;});_tmp786;});void*_tmp306[1U];_tmp306[0]=& _tmp308;({struct _dyneither_ptr _tmp896=({const char*_tmp307="exp_to_c: no type for %s";_tag_dyneither(_tmp307,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp896,_tag_dyneither(_tmp306,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp309=_tmp305;struct Cyc_Absyn_Stmt*_tmp51D;struct Cyc_Absyn_Exp*_tmp51C;struct _dyneither_ptr*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp519;int _tmp518;struct Cyc_Absyn_Exp*_tmp517;void**_tmp516;struct Cyc_Absyn_Exp*_tmp515;int _tmp514;int _tmp513;struct Cyc_List_List*_tmp512;struct Cyc_Absyn_Datatypedecl*_tmp511;struct Cyc_Absyn_Datatypefield*_tmp510;struct _tuple1**_tmp50F;struct Cyc_List_List*_tmp50E;struct Cyc_List_List*_tmp50D;struct Cyc_Absyn_Aggrdecl*_tmp50C;void*_tmp50B;struct Cyc_List_List*_tmp50A;struct Cyc_Absyn_Exp*_tmp509;void*_tmp508;int _tmp507;struct Cyc_Absyn_Vardecl*_tmp506;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp504;int _tmp503;struct Cyc_List_List*_tmp502;struct Cyc_List_List*_tmp501;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp4FE;struct _dyneither_ptr*_tmp4FD;int _tmp4FC;int _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;struct _dyneither_ptr*_tmp4F9;int _tmp4F8;int _tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;void*_tmp4F5;struct Cyc_List_List*_tmp4F4;void*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4EF;void**_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;int _tmp4EC;enum Cyc_Absyn_Coercion _tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_List_List*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;int _tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_List_List*_tmp4E4;int _tmp4E3;struct Cyc_List_List*_tmp4E2;struct Cyc_Absyn_VarargInfo*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_List_List*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Core_Opt*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D2;enum Cyc_Absyn_Incrementor _tmp4D1;enum Cyc_Absyn_Primop _tmp4D0;struct Cyc_List_List*_tmp4CF;switch(*((int*)_tmp309)){case 2U: _LL1: _LL2:
# 1843
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp309)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1850
struct Cyc_Absyn_Exp*_tmp30A=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp897=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30A,_tmp30A))->r;e->r=_tmp897;});else{
# 1855
({void*_tmp899=({struct Cyc_Absyn_Exp*_tmp898=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp898,({struct Cyc_Absyn_Exp*_tmp30B[3U];_tmp30B[0]=_tmp30A,_tmp30B[1]=_tmp30A,_tmp30B[2]=_tmp30A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp30B,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp899;});}}else{
# 1857
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1859
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 3U: _LL9: _tmp4D0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4CF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LLA: {
# 1864
struct Cyc_List_List*_tmp30C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4CF);
# 1866
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4CF);
{enum Cyc_Absyn_Primop _tmp30D=_tmp4D0;switch(_tmp30D){case Cyc_Absyn_Numelts: _LL58: _LL59: {
# 1869
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
{void*_tmp30E=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp30F=_tmp30E;void*_tmp321;union Cyc_Absyn_Constraint*_tmp320;union Cyc_Absyn_Constraint*_tmp31F;union Cyc_Absyn_Constraint*_tmp31E;struct Cyc_Absyn_Exp*_tmp31D;switch(*((int*)_tmp30F)){case 8U: _LL6D: _tmp31D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp30F)->f1).num_elts;_LL6E:
# 1873
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp31D)))
({void*_tmp310=0U;({unsigned int _tmp89B=e->loc;struct _dyneither_ptr _tmp89A=({const char*_tmp311="can't calculate numelts";_tag_dyneither(_tmp311,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp89B,_tmp89A,_tag_dyneither(_tmp310,sizeof(void*),0U));});});
e->r=_tmp31D->r;goto _LL6C;case 5U: _LL6F: _tmp321=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).elt_typ;_tmp320=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).ptr_atts).nullable;_tmp31F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).ptr_atts).bounds;_tmp31E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F)->f1).ptr_atts).zero_term;_LL70:
# 1877
{void*_tmp312=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp31F);void*_tmp313=_tmp312;struct Cyc_Absyn_Exp*_tmp318;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp313)->tag == 0U){_LL74: _LL75:
# 1879
({void*_tmp89E=({struct Cyc_Absyn_Exp*_tmp89D=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp89D,({struct Cyc_Absyn_Exp*_tmp314[2U];_tmp314[0]=(struct Cyc_Absyn_Exp*)_tmp4CF->hd,({
# 1881
struct Cyc_Absyn_Exp*_tmp89C=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp321),0U);_tmp314[1]=_tmp89C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp314,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1879
e->r=_tmp89E;});
# 1882
goto _LL73;}else{_LL76: _tmp318=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp313)->f1;_LL77:
# 1885
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp31E)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4CF->hd);
# 1889
({void*_tmp8A0=({struct Cyc_Absyn_Exp*_tmp89F=function_e;Cyc_Toc_fncall_exp_r(_tmp89F,({struct Cyc_Absyn_Exp*_tmp315[2U];_tmp315[0]=(struct Cyc_Absyn_Exp*)_tmp4CF->hd,_tmp315[1]=_tmp318;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp315,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8A0;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp320)){
if(!Cyc_Evexp_c_can_eval(_tmp318))
({void*_tmp316=0U;({unsigned int _tmp8A2=e->loc;struct _dyneither_ptr _tmp8A1=({const char*_tmp317="can't calculate numelts";_tag_dyneither(_tmp317,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8A2,_tmp8A1,_tag_dyneither(_tmp316,sizeof(void*),0U));});});
# 1894
({void*_tmp8A5=({struct Cyc_Absyn_Exp*_tmp8A4=arg;struct Cyc_Absyn_Exp*_tmp8A3=_tmp318;Cyc_Toc_conditional_exp_r(_tmp8A4,_tmp8A3,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8A5;});}else{
# 1896
e->r=_tmp318->r;goto _LL73;}}
# 1898
goto _LL73;}_LL73:;}
# 1900
goto _LL6C;default: _LL71: _LL72:
# 1902
({struct Cyc_String_pa_PrintArg_struct _tmp31B=({struct Cyc_String_pa_PrintArg_struct _tmp788;_tmp788.tag=0U,({
struct _dyneither_ptr _tmp8A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp788.f1=_tmp8A6;});_tmp788;});struct Cyc_String_pa_PrintArg_struct _tmp31C=({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0U,({struct _dyneither_ptr _tmp8A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp787.f1=_tmp8A7;});_tmp787;});void*_tmp319[2U];_tmp319[0]=& _tmp31B,_tmp319[1]=& _tmp31C;({struct _dyneither_ptr _tmp8A8=({const char*_tmp31A="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp31A,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8A8,_tag_dyneither(_tmp319,sizeof(void*),2U));});});}_LL6C:;}
# 1905
goto _LL57;}case Cyc_Absyn_Plus: _LL5A: _LL5B:
# 1910
{void*_tmp322=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp30C))->hd);void*_tmp323=_tmp322;void*_tmp32B;union Cyc_Absyn_Constraint*_tmp32A;union Cyc_Absyn_Constraint*_tmp329;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp323)->tag == 5U){_LL79: _tmp32B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp323)->f1).elt_typ;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp323)->f1).ptr_atts).bounds;_tmp329=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp323)->f1).ptr_atts).zero_term;_LL7A:
# 1912
{void*_tmp324=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32A);void*_tmp325=_tmp324;struct Cyc_Absyn_Exp*_tmp328;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp325)->tag == 0U){_LL7E: _LL7F: {
# 1914
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
({void*_tmp8AB=({struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8AA,({struct Cyc_Absyn_Exp*_tmp326[3U];_tmp326[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8A9=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp32B),0U);_tmp326[1]=_tmp8A9;}),_tmp326[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp326,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1916
e->r=_tmp8AB;});
# 1918
goto _LL7D;}}else{_LL80: _tmp328=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp325)->f1;_LL81:
# 1920
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp329)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
({void*_tmp8AD=({struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp8AC,({struct Cyc_Absyn_Exp*_tmp327[3U];_tmp327[0]=e1,_tmp327[1]=_tmp328,_tmp327[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp327,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp8AD;});}
# 1925
goto _LL7D;}_LL7D:;}
# 1927
goto _LL78;}else{_LL7B: _LL7C:
 goto _LL78;}_LL78:;}
# 1930
goto _LL57;case Cyc_Absyn_Minus: _LL5C: _LL5D: {
# 1935
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp30C))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp30C->tl))->hd)){
({void*_tmp8AF=({struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8AE,Cyc_Toc_curr_sp);});e1->r=_tmp8AF;});
({void*_tmp8B1=({struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8B0,Cyc_Toc_curr_sp);});e2->r=_tmp8B1;});
({void*_tmp8B3=({void*_tmp8B2=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmp8B2;});e1->topt=_tmp8B3;});
({void*_tmp8B5=({struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8B4,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 1943
e->r=_tmp8B5;});}else{
# 1946
({void*_tmp8B9=({struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8B8,({struct Cyc_Absyn_Exp*_tmp32C[3U];_tmp32C[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8B7=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp32C[1]=_tmp8B7;}),({
struct Cyc_Absyn_Exp*_tmp8B6=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp32C[2]=_tmp8B6;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32C,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1946
e->r=_tmp8B9;});}}
# 1951
goto _LL57;}case Cyc_Absyn_Eq: _LL5E: _LL5F:
 goto _LL61;case Cyc_Absyn_Neq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Gt: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gte: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lte: _LL68: _LL69: {
# 1959
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4CF->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp30C))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp30C->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp32D=({void*_tmp8BA=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8BA,Cyc_Toc_mt_tq);});
({void*_tmp8BD=({void*_tmp8BC=_tmp32D;Cyc_Toc_cast_it_r(_tmp8BC,({struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8BB,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8BD;});
e1->topt=_tmp32D;}
# 1969
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp32E=({void*_tmp8BE=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8BE,Cyc_Toc_mt_tq);});
({void*_tmp8C1=({void*_tmp8C0=_tmp32E;Cyc_Toc_cast_it_r(_tmp8C0,({struct Cyc_Absyn_Exp*_tmp8BF=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8BF,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8C1;});
e2->topt=_tmp32E;}
# 1974
goto _LL57;}default: _LL6A: _LL6B:
 goto _LL57;}_LL57:;}
# 1977
goto _LL0;}case 5U: _LLB: _tmp4D2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4D1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LLC: {
# 1979
void*e2_cyc_typ=(void*)_check_null(_tmp4D2->topt);
# 1988 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp348="increment";_tag_dyneither(_tmp348,sizeof(char),10U);});
if(_tmp4D1 == Cyc_Absyn_PreDec  || _tmp4D1 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8C2=({const char*_tmp32F="decrement";_tag_dyneither(_tmp32F,sizeof(char),10U);});incr_str=_tmp8C2;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4D2,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp332=({struct Cyc_String_pa_PrintArg_struct _tmp789;_tmp789.tag=0U,_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp789;});void*_tmp330[1U];_tmp330[0]=& _tmp332;({unsigned int _tmp8C4=e->loc;struct _dyneither_ptr _tmp8C3=({const char*_tmp331="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp331,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp8C4,_tmp8C3,_tag_dyneither(_tmp330,sizeof(void*),1U));});});
({void*_tmp333=0U;({struct _dyneither_ptr _tmp8C5=({const char*_tmp334="in-place inc/dec on zero-term";_tag_dyneither(_tmp334,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8C5,_tag_dyneither(_tmp333,sizeof(void*),0U));});});}{
# 1997
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4D2,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp335=Cyc_Absyn_signed_int_exp(1,0U);
_tmp335->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp336=_tmp4D1;switch(_tmp336){case Cyc_Absyn_PreInc: _LL83: _LL84:
# 2004
({void*_tmp8C7=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp338=_cycalloc(sizeof(*_tmp338));_tmp338->tag=4U,_tmp338->f1=_tmp4D2,({struct Cyc_Core_Opt*_tmp8C6=({struct Cyc_Core_Opt*_tmp337=_cycalloc(sizeof(*_tmp337));_tmp337->v=(void*)Cyc_Absyn_Plus;_tmp337;});_tmp338->f2=_tmp8C6;}),_tmp338->f3=_tmp335;_tmp338;});e->r=_tmp8C7;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL85: _LL86:
# 2008
({void*_tmp8C9=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33A=_cycalloc(sizeof(*_tmp33A));_tmp33A->tag=4U,_tmp33A->f1=_tmp4D2,({struct Cyc_Core_Opt*_tmp8C8=({struct Cyc_Core_Opt*_tmp339=_cycalloc(sizeof(*_tmp339));_tmp339->v=(void*)Cyc_Absyn_Minus;_tmp339;});_tmp33A->f2=_tmp8C8;}),_tmp33A->f3=_tmp335;_tmp33A;});e->r=_tmp8C9;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL87: _LL88:
# 2012
({struct Cyc_String_pa_PrintArg_struct _tmp33D=({struct Cyc_String_pa_PrintArg_struct _tmp78A;_tmp78A.tag=0U,_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp78A;});void*_tmp33B[1U];_tmp33B[0]=& _tmp33D;({unsigned int _tmp8CB=e->loc;struct _dyneither_ptr _tmp8CA=({const char*_tmp33C="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp33C,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp8CB,_tmp8CA,_tag_dyneither(_tmp33B,sizeof(void*),1U));});});
# 2014
({void*_tmp33E=0U;({struct _dyneither_ptr _tmp8CC=({const char*_tmp33F="in-place inc/dec on @tagged union";_tag_dyneither(_tmp33F,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8CC,_tag_dyneither(_tmp33E,sizeof(void*),0U));});});}_LL82:;};}{
# 2017
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp4D2);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D2);
Cyc_Toc_set_lhs(nv,0);{
# 2024
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4D1 == Cyc_Absyn_PostInc  || _tmp4D1 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2030
if(_tmp4D1 == Cyc_Absyn_PreDec  || _tmp4D1 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8D1=({struct Cyc_Absyn_Exp*_tmp8D0=fn_e;Cyc_Toc_fncall_exp_r(_tmp8D0,({struct Cyc_Absyn_Exp*_tmp340[3U];({struct Cyc_Absyn_Exp*_tmp8CF=Cyc_Toc_push_address_exp(_tmp4D2);_tmp340[0]=_tmp8CF;}),({
struct Cyc_Absyn_Exp*_tmp8CE=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp340[1]=_tmp8CE;}),({
struct Cyc_Absyn_Exp*_tmp8CD=Cyc_Absyn_signed_int_exp(change,0U);_tmp340[2]=_tmp8CD;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp340,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2032
e->r=_tmp8D1;});}else{
# 2035
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2041
if(_tmp4D1 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp341=Cyc_Toc_temp_var();
void*_tmp342=({void*_tmp8D2=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_typ(_tmp8D2,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp343=Cyc_Toc_push_address_exp(_tmp4D2);
struct Cyc_Absyn_Exp*_tmp344=({struct Cyc_Absyn_Exp*_tmp8D3=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp341,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8D3,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmp8D6=({struct Cyc_Absyn_Exp*_tmp8D5=({struct Cyc_Absyn_Exp*_tmp8D4=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp341,0U),0U);Cyc_Absyn_neq_exp(_tmp8D4,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2048
Cyc_Absyn_and_exp(_tmp8D5,_tmp344,0U);});_tmp344=_tmp8D6;});{
# 2050
struct Cyc_Absyn_Stmt*_tmp345=({struct Cyc_Absyn_Exp*_tmp8D8=_tmp344;struct Cyc_Absyn_Stmt*_tmp8D7=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp341,0U),0U),0U),0U);
# 2050
Cyc_Absyn_ifthenelse_stmt(_tmp8D8,_tmp8D7,
# 2052
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8DA=({struct Cyc_Absyn_Stmt*_tmp8D9=_tmp345;Cyc_Absyn_seq_stmt(_tmp8D9,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp341,0U),0U),0U),0U);});_tmp345=_tmp8DA;});
({void*_tmp8DB=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp341,_tmp342,_tmp343,_tmp345,0U),0U))->r;e->r=_tmp8DB;});};}else{
# 2057
struct Cyc_Toc_functionSet*_tmp346=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp346,_tmp4D2);
({void*_tmp8DF=({struct Cyc_Absyn_Exp*_tmp8DE=fn_e;Cyc_Toc_fncall_exp_r(_tmp8DE,({struct Cyc_Absyn_Exp*_tmp347[2U];({struct Cyc_Absyn_Exp*_tmp8DD=Cyc_Toc_push_address_exp(_tmp4D2);_tmp347[0]=_tmp8DD;}),({
struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Absyn_signed_int_exp(1,0U);_tmp347[1]=_tmp8DC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp347,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2059
e->r=_tmp8DF;});}}else{
# 2062
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4D2)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4D2,0,Cyc_Toc_incr_lvalue,_tmp4D1);
e->r=_tmp4D2->r;}}}
# 2066
goto _LL0;};};};}case 4U: _LLD: _tmp4D5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4D4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp4D3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LLE: {
# 2085 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4D5->topt);
void*e2_old_typ=(void*)_check_null(_tmp4D3->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4D5,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D5);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4D3);
({void*_tmp8E0=Cyc_Toc_tagged_union_assignop(_tmp4D5,e1_old_typ,_tmp4D4,_tmp4D3,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8E0;});
# 2096
return;}{
# 2098
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4D5,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4D5,_tmp4D4,_tmp4D3,ptr_type,is_dyneither,elt_type);
# 2104
return;}{
# 2108
int e1_poly=Cyc_Toc_is_poly_project(_tmp4D5);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D5);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4D3);{
# 2114
int done=0;
if(_tmp4D4 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp349=(enum Cyc_Absyn_Primop)_tmp4D4->v;enum Cyc_Absyn_Primop _tmp34A=_tmp349;switch(_tmp34A){case Cyc_Absyn_Plus: _LL8A: _LL8B:
 change=_tmp4D3;goto _LL89;case Cyc_Absyn_Minus: _LL8C: _LL8D:
# 2122
({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4D3,0U);change=_tmp8E1;});goto _LL89;default: _LL8E: _LL8F:
({void*_tmp34B=0U;({struct _dyneither_ptr _tmp8E2=({const char*_tmp34C="bad t ? pointer arithmetic";_tag_dyneither(_tmp34C,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E2,_tag_dyneither(_tmp34B,sizeof(void*),0U));});});}_LL89:;}
# 2125
done=1;{
# 2127
struct Cyc_Absyn_Exp*_tmp34D=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8E6=({struct Cyc_Absyn_Exp*_tmp8E5=_tmp34D;Cyc_Toc_fncall_exp_r(_tmp8E5,({struct Cyc_Absyn_Exp*_tmp34E[3U];({struct Cyc_Absyn_Exp*_tmp8E4=Cyc_Toc_push_address_exp(_tmp4D5);_tmp34E[0]=_tmp8E4;}),({
struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp34E[1]=_tmp8E3;}),_tmp34E[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34E,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2128
e->r=_tmp8E6;});};}else{
# 2131
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2134
enum Cyc_Absyn_Primop _tmp34F=(enum Cyc_Absyn_Primop)_tmp4D4->v;enum Cyc_Absyn_Primop _tmp350=_tmp34F;if(_tmp350 == Cyc_Absyn_Plus){_LL91: _LL92:
# 2136
 done=1;
({void*_tmp8E8=({struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4D5);Cyc_Toc_fncall_exp_r(_tmp8E7,({struct Cyc_Absyn_Exp*_tmp351[2U];_tmp351[0]=_tmp4D5,_tmp351[1]=_tmp4D3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp351,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8E8;});
goto _LL90;}else{_LL93: _LL94:
({void*_tmp352=0U;({struct _dyneither_ptr _tmp8E9=({const char*_tmp353="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp353,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E9,_tag_dyneither(_tmp352,sizeof(void*),0U));});});}_LL90:;}}}
# 2143
if(!done){
# 2145
if(e1_poly)
({void*_tmp8EB=({void*_tmp8EA=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmp8EA,Cyc_Absyn_new_exp(_tmp4D3->r,0U));});_tmp4D3->r=_tmp8EB;});
# 2151
if(!Cyc_Absyn_is_lvalue(_tmp4D5)){
({struct Cyc_Absyn_Exp*_tmp8EC=_tmp4D5;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8EC,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp355=1;struct _tuple31*_tmp354=_cycalloc(_check_times(_tmp355,sizeof(struct _tuple31)));(_tmp354[0]).f1=_tmp4D4,(_tmp354[0]).f2=_tmp4D3;_tmp354;}));});
e->r=_tmp4D5->r;}}
# 2156
goto _LL0;};};};}case 6U: _LLF: _tmp4D8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4D7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp4D6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LL10:
# 2158
 Cyc_Toc_exp_to_c(nv,_tmp4D8);
Cyc_Toc_exp_to_c(nv,_tmp4D7);
Cyc_Toc_exp_to_c(nv,_tmp4D6);
goto _LL0;case 7U: _LL11: _tmp4DA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4D9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL12:
 _tmp4DC=_tmp4DA;_tmp4DB=_tmp4D9;goto _LL14;case 8U: _LL13: _tmp4DC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4DB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL14:
 _tmp4DE=_tmp4DC;_tmp4DD=_tmp4DB;goto _LL16;case 9U: _LL15: _tmp4DE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4DD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL16:
# 2165
 Cyc_Toc_exp_to_c(nv,_tmp4DE);
Cyc_Toc_exp_to_c(nv,_tmp4DD);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3 == 0){_LL17: _tmp4E0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4DF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL18:
# 2169
 Cyc_Toc_exp_to_c(nv,_tmp4E0);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4DF);
goto _LL0;}else{_LL19: _tmp4E5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4E4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp4E3=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3)->num_varargs;_tmp4E2=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3)->injectors;_tmp4E1=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp309)->f3)->vai;_LL1A: {
# 2181 "toc.cyc"
struct _RegionHandle _tmp356=_new_region("r");struct _RegionHandle*r=& _tmp356;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4E3,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4E1->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2189
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E4);
int num_normargs=num_args - _tmp4E3;
# 2193
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4E4=_tmp4E4->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E4))->hd);
({struct Cyc_List_List*_tmp8ED=({struct Cyc_List_List*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357->hd=(struct Cyc_Absyn_Exp*)_tmp4E4->hd,_tmp357->tl=new_args;_tmp357;});new_args=_tmp8ED;});}}
# 2198
({struct Cyc_List_List*_tmp8F1=({struct Cyc_List_List*_tmp359=_cycalloc(sizeof(*_tmp359));({struct Cyc_Absyn_Exp*_tmp8F0=({struct Cyc_Absyn_Exp*_tmp8EF=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8EF,({struct Cyc_Absyn_Exp*_tmp358[3U];_tmp358[0]=argvexp,({
# 2200
struct Cyc_Absyn_Exp*_tmp8EE=Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp358[1]=_tmp8EE;}),_tmp358[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp358,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2198
_tmp359->hd=_tmp8F0;}),_tmp359->tl=new_args;_tmp359;});new_args=_tmp8F1;});
# 2203
({struct Cyc_List_List*_tmp8F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp8F2;});
# 2205
Cyc_Toc_exp_to_c(nv,_tmp4E5);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4E5,new_args,0U),0U);
# 2209
if(_tmp4E1->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp35A=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4E1->type));void*_tmp35B=_tmp35A;struct Cyc_Absyn_Datatypedecl*_tmp35E;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35B)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35B)->f1).datatype_info).KnownDatatype).tag == 2){_LL96: _tmp35E=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35B)->f1).datatype_info).KnownDatatype).val;_LL97:
 tud=_tmp35E;goto _LL95;}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp35C=0U;({struct _dyneither_ptr _tmp8F3=({const char*_tmp35D="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp35D,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F3,_tag_dyneither(_tmp35C,sizeof(void*),0U));});});}_LL95:;}{
# 2215
struct _dyneither_ptr vs=({unsigned int _tmp36F=(unsigned int)_tmp4E3;struct _tuple1**_tmp36E=({struct _RegionHandle*_tmp8F4=r;_region_malloc(_tmp8F4,_check_times(_tmp36F,sizeof(struct _tuple1*)));});({{unsigned int _tmp78B=(unsigned int)_tmp4E3;unsigned int i;for(i=0;i < _tmp78B;++ i){({struct _tuple1*_tmp8F5=Cyc_Toc_temp_var();_tmp36E[i]=_tmp8F5;});}}0;});_tag_dyneither(_tmp36E,sizeof(struct _tuple1*),_tmp36F);});
# 2217
if(_tmp4E3 != 0){
# 2221
struct Cyc_List_List*_tmp35F=0;
{int i=_tmp4E3 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp8F7=({struct Cyc_List_List*_tmp360=_cycalloc(sizeof(*_tmp360));({struct Cyc_Absyn_Exp*_tmp8F6=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp360->hd=_tmp8F6;}),_tmp360->tl=_tmp35F;_tmp360;});_tmp35F=_tmp8F7;});}}
({struct Cyc_Absyn_Stmt*_tmp8FB=({struct _tuple1*_tmp8FA=argv;void*_tmp8F9=arr_type;struct Cyc_Absyn_Exp*_tmp8F8=Cyc_Absyn_array_exp(_tmp35F,0U);Cyc_Absyn_declare_stmt(_tmp8FA,_tmp8F9,_tmp8F8,s,0U);});s=_tmp8FB;});
# 2226
({struct Cyc_List_List*_tmp8FC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4E4);_tmp4E4=_tmp8FC;});
({struct Cyc_List_List*_tmp8FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4E2);_tmp4E2=_tmp8FD;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E4)- 1;
for(0;_tmp4E4 != 0;(_tmp4E4=_tmp4E4->tl,_tmp4E2=_tmp4E2->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4E4->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp361=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4E2))->hd;struct Cyc_Absyn_Datatypefield*_tmp362=_tmp361;struct _tuple1*_tmp368;struct Cyc_List_List*_tmp367;_LL9B: _tmp368=_tmp362->name;_tmp367=_tmp362->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp367))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp8FE;});{
# 2240
struct _tuple1*_tmp363=Cyc_Toc_collapse_qvars(_tmp368,tud->name);
struct Cyc_List_List*_tmp364=({struct _tuple20*_tmp366[2U];({struct _tuple20*_tmp902=({struct _dyneither_ptr*_tmp901=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp901,Cyc_Toc_datatype_tag(tud,_tmp368));});_tmp366[0]=_tmp902;}),({
struct _tuple20*_tmp900=({struct _dyneither_ptr*_tmp8FF=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp8FF,arg);});_tmp366[1]=_tmp900;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp366,sizeof(struct _tuple20*),2U));});
({struct Cyc_Absyn_Stmt*_tmp906=({struct _tuple1*_tmp905=var;void*_tmp904=Cyc_Absyn_strctq(_tmp363);struct Cyc_Absyn_Exp*_tmp903=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365->tag=29U,_tmp365->f1=_tmp363,_tmp365->f2=0,_tmp365->f3=_tmp364,_tmp365->f4=0;_tmp365;}),0U);
# 2243
Cyc_Absyn_declare_stmt(_tmp905,_tmp904,_tmp903,s,0U);});s=_tmp906;});};};}};}else{
# 2251
struct Cyc_List_List*_tmp369=({struct _tuple32*_tmp36A[3U];({struct _tuple32*_tmp90F=({struct _tuple32*_tmp36B=_cycalloc(sizeof(*_tmp36B));({struct _dyneither_ptr _tmp90E=_tag_dyneither(0,0,0);_tmp36B->f1=_tmp90E;}),({struct Cyc_Absyn_Exp*_tmp90D=Cyc_Absyn_uint_exp(0U,0U);_tmp36B->f2=_tmp90D;});_tmp36B;});_tmp36A[0]=_tmp90F;}),({
struct _tuple32*_tmp90C=({struct _tuple32*_tmp36C=_cycalloc(sizeof(*_tmp36C));({struct _dyneither_ptr _tmp90B=_tag_dyneither(0,0,0);_tmp36C->f1=_tmp90B;}),({struct Cyc_Absyn_Exp*_tmp90A=Cyc_Absyn_uint_exp(0U,0U);_tmp36C->f2=_tmp90A;});_tmp36C;});_tmp36A[1]=_tmp90C;}),({
struct _tuple32*_tmp909=({struct _tuple32*_tmp36D=_cycalloc(sizeof(*_tmp36D));({struct _dyneither_ptr _tmp908=_tag_dyneither(0,0,0);_tmp36D->f1=_tmp908;}),({struct Cyc_Absyn_Exp*_tmp907=Cyc_Absyn_uint_exp(0U,0U);_tmp36D->f2=_tmp907;});_tmp36D;});_tmp36A[2]=_tmp909;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36A,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp913=({struct _tuple1*_tmp912=argv;void*_tmp911=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp910=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp912,_tmp911,_tmp910,s,0U);});s=_tmp913;});}};}else{
# 2259
if(_tmp4E3 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4E4 != 0;_tmp4E4=_tmp4E4->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E4->hd);
({struct Cyc_List_List*_tmp914=({struct Cyc_List_List*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->hd=(struct Cyc_Absyn_Exp*)_tmp4E4->hd,_tmp370->tl=array_args;_tmp370;});array_args=_tmp914;});}{
# 2265
struct Cyc_Absyn_Exp*_tmp371=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp915=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp371,s,0U);s=_tmp915;});};}else{
# 2268
({struct Cyc_Absyn_Stmt*_tmp919=({struct _tuple1*_tmp918=argv;void*_tmp917=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp916=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp918,_tmp917,_tmp916,s,0U);});s=_tmp919;});}}
# 2271
({void*_tmp91A=Cyc_Toc_stmt_exp_r(s);e->r=_tmp91A;});};}
# 2273
_npop_handler(0U);goto _LL0;
# 2181
;_pop_region(r);}}case 11U: _LL1B: _tmp4E7=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4E6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL1C:
# 2276
 Cyc_Toc_exp_to_c(nv,_tmp4E7);{
struct Cyc_Absyn_Exp*_tmp372=_tmp4E6?Cyc_Toc_newrethrow_exp(_tmp4E7): Cyc_Toc_newthrow_exp(_tmp4E7);
({void*_tmp91C=({void*_tmp91B=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp91B,_tmp372,0U);})->r;e->r=_tmp91C;});
goto _LL0;};case 12U: _LL1D: _tmp4E8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL1E:
 Cyc_Toc_exp_to_c(nv,_tmp4E8);goto _LL0;case 13U: _LL1F: _tmp4EA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4E9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL20:
# 2282
 Cyc_Toc_exp_to_c(nv,_tmp4EA);
# 2291 "toc.cyc"
for(0;_tmp4E9 != 0;_tmp4E9=_tmp4E9->tl){
enum Cyc_Absyn_KindQual _tmp373=(Cyc_Tcutil_typ_kind((void*)_tmp4E9->hd))->kind;
if(_tmp373 != Cyc_Absyn_EffKind  && _tmp373 != Cyc_Absyn_RgnKind){
{void*_tmp374=Cyc_Tcutil_compress((void*)_tmp4E9->hd);void*_tmp375=_tmp374;switch(*((int*)_tmp375)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 3U: _LLA0: _LLA1:
 continue;default: _LLA2: _LLA3:
# 2298
({void*_tmp91E=({void*_tmp91D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp91D,_tmp4EA,0U);})->r;e->r=_tmp91E;});
goto _LL9D;}_LL9D:;}
# 2301
break;}}
# 2304
goto _LL0;case 14U: _LL21: _tmp4EE=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4ED=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp4EC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp4EB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL22: {
# 2306
void*old_t2=(void*)_check_null(_tmp4ED->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4EE;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4EE=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4ED);
# 2313
{struct _tuple33 _tmp376=({struct _tuple33 _tmp78F;({void*_tmp920=Cyc_Tcutil_compress(old_t2);_tmp78F.f1=_tmp920;}),({void*_tmp91F=Cyc_Tcutil_compress(new_typ);_tmp78F.f2=_tmp91F;});_tmp78F;});struct _tuple33 _tmp377=_tmp376;struct Cyc_Absyn_PtrInfo _tmp3AF;struct Cyc_Absyn_PtrInfo _tmp3AE;struct Cyc_Absyn_PtrInfo _tmp3AD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->tag == 5U)switch(*((int*)_tmp377.f2)){case 5U: _LLA5: _tmp3AE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1;_tmp3AD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->f1;_LLA6: {
# 2315
int _tmp378=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AE.ptr_atts).nullable);
int _tmp379=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AD.ptr_atts).nullable);
void*_tmp37A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3AE.ptr_atts).bounds);
void*_tmp37B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3AD.ptr_atts).bounds);
int _tmp37C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AE.ptr_atts).zero_term);
int _tmp37D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3AD.ptr_atts).zero_term);
{struct _tuple33 _tmp37E=({struct _tuple33 _tmp78E;_tmp78E.f1=_tmp37A,_tmp78E.f2=_tmp37B;_tmp78E;});struct _tuple33 _tmp37F=_tmp37E;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3A7;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37F.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37F.f2)->tag == 1U){_LLAC: _tmp3A8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37F.f1)->f1;_tmp3A7=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37F.f2)->f1;_LLAD:
# 2323
 if((!Cyc_Evexp_c_can_eval(_tmp3A8) || !Cyc_Evexp_c_can_eval(_tmp3A7)) && !
Cyc_Evexp_same_const_exp(_tmp3A8,_tmp3A7))
({void*_tmp380=0U;({unsigned int _tmp922=e->loc;struct _dyneither_ptr _tmp921=({const char*_tmp381="can't validate cast due to potential size differences";_tag_dyneither(_tmp381,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp922,_tmp921,_tag_dyneither(_tmp380,sizeof(void*),0U));});});
if(_tmp378  && !_tmp379){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp382=0U;({struct _dyneither_ptr _tmp923=({const char*_tmp383="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp383,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp923,_tag_dyneither(_tmp382,sizeof(void*),0U));});});
# 2332
if(_tmp4EB != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp386=({struct Cyc_String_pa_PrintArg_struct _tmp78C;_tmp78C.tag=0U,({struct _dyneither_ptr _tmp924=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp78C.f1=_tmp924;});_tmp78C;});void*_tmp384[1U];_tmp384[0]=& _tmp386;({struct _dyneither_ptr _tmp925=({const char*_tmp385="null-check conversion mis-classified: %s";_tag_dyneither(_tmp385,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp925,_tag_dyneither(_tmp384,sizeof(void*),1U));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp4ED);
if(do_null_check){
if(!_tmp4EC)
({void*_tmp387=0U;({unsigned int _tmp927=e->loc;struct _dyneither_ptr _tmp926=({const char*_tmp388="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp388,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp927,_tmp926,_tag_dyneither(_tmp387,sizeof(void*),0U));});});
# 2339
({void*_tmp92A=({void*_tmp929=*_tmp4EE;Cyc_Toc_cast_it_r(_tmp929,({struct Cyc_Absyn_Exp*_tmp928=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp928,({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));
_tmp389->hd=_tmp4ED,_tmp389->tl=0;_tmp389;}),0U);}));});
# 2339
e->r=_tmp92A;});}else{
# 2343
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4ED->r;}};}else{
# 2348
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4ED->r;}
# 2355
goto _LLAB;}else{_LLAE: _tmp3A9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37F.f1)->f1;_LLAF:
# 2357
 if(!Cyc_Evexp_c_can_eval(_tmp3A9))
({void*_tmp38A=0U;({unsigned int _tmp92C=e->loc;struct _dyneither_ptr _tmp92B=({const char*_tmp38B="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp38B,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp92C,_tmp92B,_tag_dyneither(_tmp38A,sizeof(void*),0U));});});
# 2360
if(_tmp4EB == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp38E=({struct Cyc_String_pa_PrintArg_struct _tmp78D;_tmp78D.tag=0U,({struct _dyneither_ptr _tmp92D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp78D.f1=_tmp92D;});_tmp78D;});void*_tmp38C[1U];_tmp38C[0]=& _tmp38E;({struct _dyneither_ptr _tmp92E=({const char*_tmp38D="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp38D,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp92E,_tag_dyneither(_tmp38C,sizeof(void*),1U));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2364
if((_tmp37C  && !(_tmp3AD.elt_tq).real_const) && !_tmp37D)
# 2367
({struct Cyc_Absyn_Exp*_tmp930=({struct Cyc_Absyn_Exp*_tmp92F=_tmp3A9;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp92F,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3A9=_tmp930;});
# 2369
({void*_tmp931=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3A9,_tmp4ED))->r;e->r=_tmp931;});}else{
# 2371
struct Cyc_Absyn_Exp*_tmp38F=Cyc_Toc__tag_dyneither_e;
# 2373
if(_tmp37C){
# 2378
struct _tuple1*_tmp390=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp391=Cyc_Absyn_var_exp(_tmp390,0U);
struct Cyc_Absyn_Exp*arg3;
# 2383
{void*_tmp392=_tmp4ED->r;void*_tmp393=_tmp392;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp393)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp393)->f1).Wstring_c).tag){case 8U: _LLB5: _LLB6:
# 2385
 arg3=_tmp3A9;goto _LLB4;case 9U: _LLB7: _LLB8:
# 2387
 arg3=_tmp3A9;goto _LLB4;default: goto _LLB9;}else{_LLB9: _LLBA:
# 2389
({struct Cyc_Absyn_Exp*_tmp935=({struct Cyc_Absyn_Exp*_tmp934=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4ED);Cyc_Absyn_fncall_exp(_tmp934,({struct Cyc_Absyn_Exp*_tmp394[2U];({
# 2391
struct Cyc_Absyn_Exp*_tmp933=({void*_tmp932=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp932,_tmp391);});_tmp394[0]=_tmp933;}),_tmp394[1]=_tmp3A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp394,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2389
arg3=_tmp935;});
# 2392
goto _LLB4;}_LLB4:;}
# 2394
if(!_tmp37D  && !(_tmp3AD.elt_tq).real_const)
# 2397
({struct Cyc_Absyn_Exp*_tmp937=({struct Cyc_Absyn_Exp*_tmp936=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp936,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp937;});{
# 2399
struct Cyc_Absyn_Exp*_tmp395=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AD.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp396=({struct Cyc_Absyn_Exp*_tmp938=_tmp38F;Cyc_Absyn_fncall_exp(_tmp938,({struct Cyc_Absyn_Exp*_tmp398[3U];_tmp398[0]=_tmp391,_tmp398[1]=_tmp395,_tmp398[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp398,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp397=Cyc_Absyn_exp_stmt(_tmp396,0U);
({struct Cyc_Absyn_Stmt*_tmp93C=({struct _tuple1*_tmp93B=_tmp390;void*_tmp93A=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp939=_tmp4ED;Cyc_Absyn_declare_stmt(_tmp93B,_tmp93A,_tmp939,_tmp397,0U);});_tmp397=_tmp93C;});
({void*_tmp93D=Cyc_Toc_stmt_exp_r(_tmp397);e->r=_tmp93D;});};}else{
# 2406
({void*_tmp940=({struct Cyc_Absyn_Exp*_tmp93F=_tmp38F;Cyc_Toc_fncall_exp_r(_tmp93F,({struct Cyc_Absyn_Exp*_tmp399[3U];_tmp399[0]=_tmp4ED,({
# 2408
struct Cyc_Absyn_Exp*_tmp93E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AD.elt_typ),0U);_tmp399[1]=_tmp93E;}),_tmp399[2]=_tmp3A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp399,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2406
e->r=_tmp940;});}}
# 2412
goto _LLAB;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37F.f2)->tag == 1U){_LLB0: _tmp3AA=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp37F.f2)->f1;_LLB1:
# 2414
 if(!Cyc_Evexp_c_can_eval(_tmp3AA))
({void*_tmp39A=0U;({unsigned int _tmp942=e->loc;struct _dyneither_ptr _tmp941=({const char*_tmp39B="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp39B,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp942,_tmp941,_tag_dyneither(_tmp39A,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp39C=0U;({struct _dyneither_ptr _tmp943=({const char*_tmp39D="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp39D,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp943,_tag_dyneither(_tmp39C,sizeof(void*),0U));});});{
# 2426 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp39E=_tmp3AA;
if(_tmp37C  && !_tmp37D)
({struct Cyc_Absyn_Exp*_tmp945=({struct Cyc_Absyn_Exp*_tmp944=_tmp3AA;Cyc_Absyn_add_exp(_tmp944,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp39E=_tmp945;});{
# 2433
struct Cyc_Absyn_Exp*_tmp39F=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp3A0=({struct Cyc_Absyn_Exp*_tmp947=_tmp39F;Cyc_Absyn_fncall_exp(_tmp947,({struct Cyc_Absyn_Exp*_tmp3A2[3U];_tmp3A2[0]=_tmp4ED,({
# 2436
struct Cyc_Absyn_Exp*_tmp946=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AE.elt_typ),0U);_tmp3A2[1]=_tmp946;}),_tmp3A2[2]=_tmp39E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A2,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2438
if(!_tmp379)
({void*_tmp94A=({struct Cyc_Absyn_Exp*_tmp949=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp949,({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));
({struct Cyc_Absyn_Exp*_tmp948=Cyc_Absyn_copy_exp(_tmp3A0);_tmp3A1->hd=_tmp948;}),_tmp3A1->tl=0;_tmp3A1;}));});
# 2439
_tmp3A0->r=_tmp94A;});
# 2441
({void*_tmp94B=Cyc_Toc_cast_it_r(*_tmp4EE,_tmp3A0);e->r=_tmp94B;});
goto _LLAB;};};}else{_LLB2: _LLB3:
# 2446
 DynCast:
 if((_tmp37C  && !_tmp37D) && !(_tmp3AD.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3A3=0U;({struct _dyneither_ptr _tmp94C=({const char*_tmp3A4="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp3A4,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp94C,_tag_dyneither(_tmp3A3,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp3A5=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmp950=({struct Cyc_Absyn_Exp*_tmp94F=_tmp3A5;Cyc_Toc_fncall_exp_r(_tmp94F,({struct Cyc_Absyn_Exp*_tmp3A6[3U];_tmp3A6[0]=_tmp4ED,({
struct Cyc_Absyn_Exp*_tmp94E=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3AE.elt_typ),0U);_tmp3A6[1]=_tmp94E;}),({
struct Cyc_Absyn_Exp*_tmp94D=Cyc_Absyn_uint_exp(1U,0U);_tmp3A6[2]=_tmp94D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A6,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2451
e->r=_tmp950;});};}
# 2455
goto _LLAB;}}_LLAB:;}
# 2457
goto _LLA4;}case 6U: _LLA7: _tmp3AF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1;_LLA8:
# 2459
{void*_tmp3AB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3AF.ptr_atts).bounds);void*_tmp3AC=_tmp3AB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3AC)->tag == 0U){_LLBC: _LLBD:
# 2461
({void*_tmp952=({struct Cyc_Absyn_Exp*_tmp951=Cyc_Absyn_new_exp(_tmp4ED->r,_tmp4ED->loc);Cyc_Toc_aggrmember_exp_r(_tmp951,Cyc_Toc_curr_sp);});_tmp4ED->r=_tmp952;});
_tmp4ED->topt=new_typ_c;
goto _LLBB;}else{_LLBE: _LLBF:
 goto _LLBB;}_LLBB:;}
# 2466
goto _LLA4;default: goto _LLA9;}else{_LLA9: _LLAA:
# 2468
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4ED->r;
goto _LLA4;}_LLA4:;}
# 2472
goto _LL0;}case 15U: _LL23: _tmp4EF=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL24:
# 2475
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4EF);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4EF)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4EF,0,Cyc_Toc_address_lvalue,1);
# 2481
({void*_tmp954=({void*_tmp953=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp953,_tmp4EF);});e->r=_tmp954;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp4EF->topt))))
# 2485
({void*_tmp956=({void*_tmp955=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp955,_tmp4EF);});e->r=_tmp956;});}
# 2487
goto _LL0;case 16U: _LL25: _tmp4F1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4F0=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL26:
# 2494
 Cyc_Toc_exp_to_c(nv,_tmp4F0);{
# 2496
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4F0->topt));
struct _tuple1*_tmp3B0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3B1=Cyc_Absyn_var_exp(_tmp3B0,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3B2=Cyc_Tcutil_compress(elt_typ);void*_tmp3B3=_tmp3B2;void*_tmp3B4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->tag == 8U){_LLC1: _tmp3B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->f1).elt_type;_LLC2:
# 2502
({void*_tmp957=Cyc_Toc_typ_to_c(_tmp3B4);elt_typ=_tmp957;});
({struct Cyc_Absyn_Exp*_tmp958=Cyc_Absyn_copy_exp(_tmp3B1);lhs=_tmp958;});
goto _LLC0;}else{_LLC3: _LLC4:
# 2506
({struct Cyc_Absyn_Exp*_tmp959=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp3B1),0U);lhs=_tmp959;});
goto _LLC0;}_LLC0:;}{
# 2509
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp4F0);
struct _tuple1*_tmp3B5=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3B6=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3B5),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp95C=({struct Cyc_Absyn_Exp*_tmp95B=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp95B,({struct Cyc_Absyn_Exp*_tmp3B7[2U];_tmp3B7[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3B6),({
struct Cyc_Absyn_Exp*_tmp95A=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3B7[1]=_tmp95A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B7,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2514
mexp=_tmp95C;});else{
# 2517
({struct Cyc_Absyn_Exp*_tmp95D=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp3B1,0U),0U);mexp=_tmp95D;});}{
# 2520
struct Cyc_Absyn_Exp*_tmp3B8=Cyc_Toc_get_varsizeexp(_tmp4F0);
if(_tmp3B8 != 0)
({struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_add_exp(mexp,_tmp3B8,0U);mexp=_tmp95E;});
# 2524
if(_tmp4F1 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp95F=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp95F;});else{
# 2527
struct Cyc_Absyn_Exp*r=_tmp4F1;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp960=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp960;});}{
# 2531
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp965=({struct Cyc_Absyn_Exp*_tmp964=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp964,({struct Cyc_Absyn_Exp*_tmp3B9[3U];({
struct Cyc_Absyn_Exp*_tmp963=Cyc_Absyn_copy_exp(_tmp3B1);_tmp3B9[0]=_tmp963;}),({
struct Cyc_Absyn_Exp*_tmp962=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3B9[1]=_tmp962;}),
(unsigned int)_tmp3B6?_tmp3B9[2]=_tmp3B6:({struct Cyc_Absyn_Exp*_tmp961=Cyc_Absyn_uint_exp(1U,0U);_tmp3B9[2]=_tmp961;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3B9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2533
answer=_tmp965;});else{
# 2538
({struct Cyc_Absyn_Exp*_tmp966=Cyc_Absyn_copy_exp(_tmp3B1);answer=_tmp966;});}
({void*_tmp967=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->tag=Cyc_Toc_Dest,_tmp3BA->f1=_tmp3B1;_tmp3BA;});e->annot=_tmp967;});{
struct Cyc_Absyn_Stmt*_tmp3BB=({struct _tuple1*_tmp96B=_tmp3B0;void*_tmp96A=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp969=mexp;Cyc_Absyn_declare_stmt(_tmp96B,_tmp96A,_tmp969,({
struct Cyc_Absyn_Stmt*_tmp968=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp968,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2544
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp96C=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3B5),Cyc_Absyn_uint_typ,array_len,_tmp3BB,0U);_tmp3BB=_tmp96C;});
({void*_tmp96D=Cyc_Toc_stmt_exp_r(_tmp3BB);e->r=_tmp96D;});
if(_tmp3B8 != 0)
({void*_tmp96F=({void*_tmp96E=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp96E,Cyc_Absyn_copy_exp(e));});e->r=_tmp96F;});
goto _LL0;};};};};};case 18U: _LL27: _tmp4F2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL28: {
# 2553
int _tmp3BC=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4F2);
Cyc_Toc_set_in_sizeof(nv,_tmp3BC);
goto _LL0;}case 17U: _LL29: _tmp4F3=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL2A:
({void*_tmp971=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=17U,({void*_tmp970=Cyc_Toc_typ_to_c(_tmp4F3);_tmp3BD->f1=_tmp970;});_tmp3BD;});e->r=_tmp971;});goto _LL0;case 19U: _LL2B: _tmp4F5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4F4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL2C: {
# 2559
void*_tmp3BE=_tmp4F5;
struct Cyc_List_List*_tmp3BF=_tmp4F4;
for(0;_tmp3BF != 0;_tmp3BF=_tmp3BF->tl){
void*_tmp3C0=(void*)_tmp3BF->hd;void*_tmp3C1=_tmp3C0;unsigned int _tmp3D3;struct _dyneither_ptr*_tmp3D2;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C1)->tag == 0U){_LLC6: _tmp3D2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C1)->f1;_LLC7:
 goto _LLC5;}else{_LLC8: _tmp3D3=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3C1)->f1;_LLC9:
# 2565
{void*_tmp3C2=Cyc_Tcutil_compress(_tmp3BE);void*_tmp3C3=_tmp3C2;struct Cyc_Absyn_Datatypefield*_tmp3D1;struct Cyc_List_List*_tmp3D0;struct Cyc_List_List*_tmp3CF;union Cyc_Absyn_AggrInfoU _tmp3CE;switch(*((int*)_tmp3C3)){case 11U: _LLCB: _tmp3CE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C3)->f1).aggr_info;_LLCC: {
# 2567
struct Cyc_Absyn_Aggrdecl*_tmp3C4=Cyc_Absyn_get_known_aggrdecl(_tmp3CE);
if(_tmp3C4->impl == 0)
({void*_tmp3C5=0U;({struct _dyneither_ptr _tmp972=({const char*_tmp3C6="struct fields must be known";_tag_dyneither(_tmp3C6,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp972,_tag_dyneither(_tmp3C5,sizeof(void*),0U));});});
_tmp3CF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C4->impl))->fields;goto _LLCE;}case 12U: _LLCD: _tmp3CF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C3)->f2;_LLCE: {
# 2572
struct Cyc_Absyn_Aggrfield*_tmp3C7=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3CF,(int)_tmp3D3);
({void*_tmp973=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=0U,_tmp3C8->f1=_tmp3C7->name;_tmp3C8;}));_tmp3BF->hd=_tmp973;});
_tmp3BE=_tmp3C7->type;
goto _LLCA;}case 10U: _LLCF: _tmp3D0=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C3)->f1;_LLD0:
# 2577
({void*_tmp975=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->tag=0U,({struct _dyneither_ptr*_tmp974=Cyc_Absyn_fieldname((int)(_tmp3D3 + 1));_tmp3C9->f1=_tmp974;});_tmp3C9;}));_tmp3BF->hd=_tmp975;});
({void*_tmp976=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D0,(int)_tmp3D3)).f2;_tmp3BE=_tmp976;});
goto _LLCA;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C3)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp3D1=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C3)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 2581
 if(_tmp3D3 == 0)
({void*_tmp977=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->tag=0U,_tmp3CA->f1=Cyc_Toc_tag_sp;_tmp3CA;}));_tmp3BF->hd=_tmp977;});else{
# 2584
({void*_tmp978=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D1->typs,(int)(_tmp3D3 - 1))).f2;_tmp3BE=_tmp978;});
({void*_tmp97A=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->tag=0U,({struct _dyneither_ptr*_tmp979=Cyc_Absyn_fieldname((int)_tmp3D3);_tmp3CB->f1=_tmp979;});_tmp3CB;}));_tmp3BF->hd=_tmp97A;});}
# 2587
goto _LLCA;}else{goto _LLD3;}default: _LLD3: _LLD4:
({void*_tmp3CC=0U;({struct _dyneither_ptr _tmp97B=({const char*_tmp3CD="impossible type for offsetof tuple index";_tag_dyneither(_tmp3CD,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97B,_tag_dyneither(_tmp3CC,sizeof(void*),0U));});});
goto _LLCA;}_LLCA:;}
# 2591
goto _LLC5;}_LLC5:;}
# 2594
({void*_tmp97D=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->tag=19U,({void*_tmp97C=Cyc_Toc_typ_to_c(_tmp4F5);_tmp3D4->f1=_tmp97C;}),_tmp3D4->f2=_tmp4F4;_tmp3D4;});e->r=_tmp97D;});
goto _LL0;}case 20U: _LL2D: _tmp4F6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL2E: {
# 2597
int _tmp3D5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3D6=Cyc_Tcutil_compress((void*)_check_null(_tmp4F6->topt));
{void*_tmp3D7=_tmp3D6;void*_tmp3EC;struct Cyc_Absyn_Tqual _tmp3EB;void*_tmp3EA;union Cyc_Absyn_Constraint*_tmp3E9;union Cyc_Absyn_Constraint*_tmp3E8;union Cyc_Absyn_Constraint*_tmp3E7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->tag == 5U){_LLD6: _tmp3EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).elt_typ;_tmp3EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).elt_tq;_tmp3EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).rgn;_tmp3E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).nullable;_tmp3E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).bounds;_tmp3E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).ptr_atts).zero_term;_LLD7:
# 2602
{void*_tmp3D8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3E8);void*_tmp3D9=_tmp3D8;struct Cyc_Absyn_Exp*_tmp3E4;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3D9)->tag == 1U){_LLDB: _tmp3E4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3D9)->f1;_LLDC: {
# 2604
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp4F6);
Cyc_Toc_exp_to_c(nv,_tmp4F6);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3DA=0U;({unsigned int _tmp97F=e->loc;struct _dyneither_ptr _tmp97E=({const char*_tmp3DB="inserted null check due to dereference";_tag_dyneither(_tmp3DB,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp97F,_tmp97E,_tag_dyneither(_tmp3DA,sizeof(void*),0U));});});
# 2610
({void*_tmp983=({void*_tmp982=Cyc_Toc_typ_to_c(_tmp3D6);Cyc_Toc_cast_it_r(_tmp982,({
struct Cyc_Absyn_Exp*_tmp981=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp981,({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));
({struct Cyc_Absyn_Exp*_tmp980=Cyc_Absyn_copy_exp(_tmp4F6);_tmp3DC->hd=_tmp980;}),_tmp3DC->tl=0;_tmp3DC;}),0U);}));});
# 2610
_tmp4F6->r=_tmp983;});}
# 2618
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3E7)){
struct _tuple13 _tmp3DD=Cyc_Evexp_eval_const_uint_exp(_tmp3E4);struct _tuple13 _tmp3DE=_tmp3DD;unsigned int _tmp3E2;int _tmp3E1;_LLE0: _tmp3E2=_tmp3DE.f1;_tmp3E1=_tmp3DE.f2;_LLE1:;
# 2624
if(!_tmp3E1  || _tmp3E2 <= 0)
({void*_tmp3DF=0U;({unsigned int _tmp985=e->loc;struct _dyneither_ptr _tmp984=({const char*_tmp3E0="cannot determine dereference is in bounds";_tag_dyneither(_tmp3E0,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp985,_tmp984,_tag_dyneither(_tmp3DF,sizeof(void*),0U));});});}
# 2627
goto _LLDA;}}else{_LLDD: _LLDE: {
# 2630
struct Cyc_Absyn_Exp*_tmp3E3=Cyc_Absyn_uint_exp(0U,0U);
_tmp3E3->topt=Cyc_Absyn_uint_typ;
({void*_tmp986=Cyc_Toc_subscript_exp_r(_tmp4F6,_tmp3E3);e->r=_tmp986;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLDA;}}_LLDA:;}
# 2636
goto _LLD5;}else{_LLD8: _LLD9:
({void*_tmp3E5=0U;({struct _dyneither_ptr _tmp987=({const char*_tmp3E6="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3E6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp987,_tag_dyneither(_tmp3E5,sizeof(void*),0U));});});}_LLD5:;}
# 2639
Cyc_Toc_set_lhs(nv,_tmp3D5);
goto _LL0;};}case 21U: _LL2F: _tmp4FA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4F9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp4F8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp4F7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL30: {
# 2642
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4FA->topt);
Cyc_Toc_exp_to_c(nv,_tmp4FA);
if(_tmp4F8  && _tmp4F7)
({void*_tmp98C=({struct Cyc_Absyn_Exp*_tmp98B=_tmp4FA;void*_tmp98A=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp989=e1_cyc_type;struct _dyneither_ptr*_tmp988=_tmp4F9;Cyc_Toc_check_tagged_union(_tmp98B,_tmp98A,_tmp989,_tmp988,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2646
e->r=_tmp98C;});
# 2650
if(is_poly)
({void*_tmp98E=({void*_tmp98D=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp98D,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp98E;});
goto _LL0;}case 22U: _LL31: _tmp4FE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4FD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp4FC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp4FB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL32: {
# 2654
int _tmp3ED=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4FE->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4FE);
Cyc_Toc_exp_to_c(nv,_tmp4FE);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3EE=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3EF=_tmp3EE;void*_tmp405;struct Cyc_Absyn_Tqual _tmp404;void*_tmp403;union Cyc_Absyn_Constraint*_tmp402;union Cyc_Absyn_Constraint*_tmp401;union Cyc_Absyn_Constraint*_tmp400;_LLE3: _tmp405=_tmp3EF.elt_typ;_tmp404=_tmp3EF.elt_tq;_tmp403=(_tmp3EF.ptr_atts).rgn;_tmp402=(_tmp3EF.ptr_atts).nullable;_tmp401=(_tmp3EF.ptr_atts).bounds;_tmp400=(_tmp3EF.ptr_atts).zero_term;_LLE4:;
{void*_tmp3F0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp401);void*_tmp3F1=_tmp3F0;struct Cyc_Absyn_Exp*_tmp3FF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F1)->tag == 1U){_LLE6: _tmp3FF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F1)->f1;_LLE7: {
# 2665
struct _tuple13 _tmp3F2=Cyc_Evexp_eval_const_uint_exp(_tmp3FF);struct _tuple13 _tmp3F3=_tmp3F2;unsigned int _tmp3FD;int _tmp3FC;_LLEB: _tmp3FD=_tmp3F3.f1;_tmp3FC=_tmp3F3.f2;_LLEC:;
if(_tmp3FC){
if(_tmp3FD < 1)
({void*_tmp3F4=0U;({struct _dyneither_ptr _tmp98F=({const char*_tmp3F5="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp3F5,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp98F,_tag_dyneither(_tmp3F4,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3F6=0U;({unsigned int _tmp991=e->loc;struct _dyneither_ptr _tmp990=({const char*_tmp3F7="inserted null check due to dereference";_tag_dyneither(_tmp3F7,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp991,_tmp990,_tag_dyneither(_tmp3F6,sizeof(void*),0U));});});
# 2673
({void*_tmp995=({void*_tmp994=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp994,({
struct Cyc_Absyn_Exp*_tmp993=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp993,({struct Cyc_Absyn_Exp*_tmp3F8[1U];({struct Cyc_Absyn_Exp*_tmp992=
Cyc_Absyn_new_exp(_tmp4FE->r,0U);_tmp3F8[0]=_tmp992;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F8,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2673
_tmp4FE->r=_tmp995;});}}else{
# 2678
if(!Cyc_Evexp_c_can_eval(_tmp3FF))
({void*_tmp3F9=0U;({unsigned int _tmp997=e->loc;struct _dyneither_ptr _tmp996=({const char*_tmp3FA="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp3FA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp997,_tmp996,_tag_dyneither(_tmp3F9,sizeof(void*),0U));});});
# 2682
({void*_tmp99D=({void*_tmp99C=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp99C,({
struct Cyc_Absyn_Exp*_tmp99B=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp99B,({struct Cyc_Absyn_Exp*_tmp3FB[4U];({
struct Cyc_Absyn_Exp*_tmp99A=Cyc_Absyn_new_exp(_tmp4FE->r,0U);_tmp3FB[0]=_tmp99A;}),_tmp3FB[1]=_tmp3FF,({
struct Cyc_Absyn_Exp*_tmp999=Cyc_Absyn_sizeoftyp_exp(_tmp405,0U);_tmp3FB[2]=_tmp999;}),({
struct Cyc_Absyn_Exp*_tmp998=Cyc_Absyn_uint_exp(0U,0U);_tmp3FB[3]=_tmp998;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FB,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2682
_tmp4FE->r=_tmp99D;});}
# 2688
goto _LLE5;}}else{_LLE8: _LLE9: {
# 2691
void*ta1=Cyc_Toc_typ_to_c(_tmp405);
({void*_tmp9A3=({void*_tmp9A2=Cyc_Absyn_cstar_typ(ta1,_tmp404);Cyc_Toc_cast_it_r(_tmp9A2,({
struct Cyc_Absyn_Exp*_tmp9A1=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9A1,({struct Cyc_Absyn_Exp*_tmp3FE[3U];({
struct Cyc_Absyn_Exp*_tmp9A0=Cyc_Absyn_new_exp(_tmp4FE->r,0U);_tmp3FE[0]=_tmp9A0;}),({
struct Cyc_Absyn_Exp*_tmp99F=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp3FE[1]=_tmp99F;}),({
struct Cyc_Absyn_Exp*_tmp99E=Cyc_Absyn_uint_exp(0U,0U);_tmp3FE[2]=_tmp99E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FE,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2692
_tmp4FE->r=_tmp9A3;});
# 2699
goto _LLE5;}}_LLE5:;}
# 2701
if(_tmp4FC  && _tmp4FB)
({void*_tmp9A8=({struct Cyc_Absyn_Exp*_tmp9A7=_tmp4FE;void*_tmp9A6=Cyc_Toc_typ_to_c(e1typ);void*_tmp9A5=_tmp405;struct _dyneither_ptr*_tmp9A4=_tmp4FD;Cyc_Toc_check_tagged_union(_tmp9A7,_tmp9A6,_tmp9A5,_tmp9A4,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9A8;});
# 2704
if(is_poly  && _tmp4FB)
({void*_tmp9AA=({void*_tmp9A9=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9A9,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9AA;});
Cyc_Toc_set_lhs(nv,_tmp3ED);
goto _LL0;};};}case 23U: _LL33: _tmp500=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp4FF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL34: {
# 2709
int _tmp406=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp407=Cyc_Tcutil_compress((void*)_check_null(_tmp500->topt));
# 2713
{void*_tmp408=_tmp407;void*_tmp432;struct Cyc_Absyn_Tqual _tmp431;void*_tmp430;union Cyc_Absyn_Constraint*_tmp42F;union Cyc_Absyn_Constraint*_tmp42E;union Cyc_Absyn_Constraint*_tmp42D;struct Cyc_List_List*_tmp42C;switch(*((int*)_tmp408)){case 10U: _LLEE: _tmp42C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp408)->f1;_LLEF:
# 2716
 Cyc_Toc_exp_to_c(nv,_tmp500);
Cyc_Toc_exp_to_c(nv,_tmp4FF);{
struct _tuple13 _tmp409=Cyc_Evexp_eval_const_uint_exp(_tmp4FF);struct _tuple13 _tmp40A=_tmp409;unsigned int _tmp40E;int _tmp40D;_LLF5: _tmp40E=_tmp40A.f1;_tmp40D=_tmp40A.f2;_LLF6:;
if(!_tmp40D)
({void*_tmp40B=0U;({struct _dyneither_ptr _tmp9AB=({const char*_tmp40C="unknown tuple subscript in translation to C";_tag_dyneither(_tmp40C,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9AB,_tag_dyneither(_tmp40B,sizeof(void*),0U));});});
({void*_tmp9AD=({struct Cyc_Absyn_Exp*_tmp9AC=_tmp500;Cyc_Toc_aggrmember_exp_r(_tmp9AC,Cyc_Absyn_fieldname((int)(_tmp40E + 1)));});e->r=_tmp9AD;});
goto _LLED;};case 5U: _LLF0: _tmp432=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).elt_typ;_tmp431=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).elt_tq;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).rgn;_tmp42F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).nullable;_tmp42E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).bounds;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp408)->f1).ptr_atts).zero_term;_LLF1: {
# 2724
struct Cyc_List_List*_tmp40F=Cyc_Toc_get_relns(_tmp500);
int _tmp410=Cyc_Toc_need_null_check(_tmp500);
int _tmp411=Cyc_Toc_in_sizeof(nv);
# 2732
int in_bnds=_tmp411;
{void*_tmp412=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp42E);void*_tmp413=_tmp412;_LLF8: _LLF9:
# 2735
({int _tmp9AE=in_bnds  || Cyc_Toc_check_bounds(_tmp407,_tmp40F,_tmp500,_tmp4FF);in_bnds=_tmp9AE;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp416=({struct Cyc_String_pa_PrintArg_struct _tmp790;_tmp790.tag=0U,({struct _dyneither_ptr _tmp9AF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp790.f1=_tmp9AF;});_tmp790;});void*_tmp414[1U];_tmp414[0]=& _tmp416;({unsigned int _tmp9B1=e->loc;struct _dyneither_ptr _tmp9B0=({const char*_tmp415="bounds check necessary for %s";_tag_dyneither(_tmp415,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp9B1,_tmp9B0,_tag_dyneither(_tmp414,sizeof(void*),1U));});});_LLF7:;}
# 2742
Cyc_Toc_exp_to_c(nv,_tmp500);
Cyc_Toc_exp_to_c(nv,_tmp4FF);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp417=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp42E);void*_tmp418=_tmp417;struct Cyc_Absyn_Exp*_tmp429;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418)->tag == 1U){_LLFB: _tmp429=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp418)->f1;_LLFC: {
# 2747
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp42F) && _tmp410;
void*ta1=Cyc_Toc_typ_to_c(_tmp432);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp431);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp419=0U;({unsigned int _tmp9B3=e->loc;struct _dyneither_ptr _tmp9B2=({const char*_tmp41A="inserted null check due to dereference";_tag_dyneither(_tmp41A,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9B3,_tmp9B2,_tag_dyneither(_tmp419,sizeof(void*),0U));});});
({void*_tmp9B7=({void*_tmp9B6=ta2;Cyc_Toc_cast_it_r(_tmp9B6,({struct Cyc_Absyn_Exp*_tmp9B5=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9B5,({struct Cyc_Absyn_Exp*_tmp41B[1U];({struct Cyc_Absyn_Exp*_tmp9B4=
Cyc_Absyn_copy_exp(_tmp500);_tmp41B[0]=_tmp9B4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41B,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2756
_tmp500->r=_tmp9B7;});}else{
# 2759
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp42D)){
# 2761
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp41C=0U;({unsigned int _tmp9B9=e->loc;struct _dyneither_ptr _tmp9B8=({const char*_tmp41D="cannot determine subscript is in bounds";_tag_dyneither(_tmp41D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9B9,_tmp9B8,_tag_dyneither(_tmp41C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp500);
({void*_tmp9BC=Cyc_Toc_deref_exp_r(({void*_tmp9BB=ta2;Cyc_Toc_cast_it(_tmp9BB,({
struct Cyc_Absyn_Exp*_tmp9BA=function_e;Cyc_Absyn_fncall_exp(_tmp9BA,({struct Cyc_Absyn_Exp*_tmp41E[3U];_tmp41E[0]=_tmp500,_tmp41E[1]=_tmp429,_tmp41E[2]=_tmp4FF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41E,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2764
e->r=_tmp9BC;});};}else{
# 2767
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp41F=0U;({unsigned int _tmp9BE=e->loc;struct _dyneither_ptr _tmp9BD=({const char*_tmp420="cannot determine subscript is in bounds";_tag_dyneither(_tmp420,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9BE,_tmp9BD,_tag_dyneither(_tmp41F,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp421=0U;({unsigned int _tmp9C0=e->loc;struct _dyneither_ptr _tmp9BF=({const char*_tmp422="inserted null check due to dereference";_tag_dyneither(_tmp422,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9C0,_tmp9BF,_tag_dyneither(_tmp421,sizeof(void*),0U));});});
# 2773
({void*_tmp9C4=Cyc_Toc_deref_exp_r(({void*_tmp9C3=ta2;Cyc_Toc_cast_it(_tmp9C3,({
struct Cyc_Absyn_Exp*_tmp9C2=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9C2,({struct Cyc_Absyn_Exp*_tmp423[4U];_tmp423[0]=_tmp500,_tmp423[1]=_tmp429,({
# 2776
struct Cyc_Absyn_Exp*_tmp9C1=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp423[2]=_tmp9C1;}),_tmp423[3]=_tmp4FF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp423,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2773
e->r=_tmp9C4;});}else{
# 2779
if(!Cyc_Evexp_c_can_eval(_tmp429))
({void*_tmp424=0U;({unsigned int _tmp9C6=e->loc;struct _dyneither_ptr _tmp9C5=({const char*_tmp425="cannot determine subscript is in bounds";_tag_dyneither(_tmp425,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9C6,_tmp9C5,_tag_dyneither(_tmp424,sizeof(void*),0U));});});
# 2782
({void*_tmp9C9=({struct Cyc_Absyn_Exp*_tmp9C8=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9C8,({struct Cyc_Absyn_Exp*_tmp426[2U];_tmp426[0]=_tmp429,({
struct Cyc_Absyn_Exp*_tmp9C7=Cyc_Absyn_copy_exp(_tmp4FF);_tmp426[1]=_tmp9C7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp426,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2782
_tmp4FF->r=_tmp9C9;});}}}}
# 2785
goto _LLFA;}}else{_LLFD: _LLFE: {
# 2787
void*ta1=Cyc_Toc_typ_to_c(_tmp432);
if(in_bnds){
# 2791
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9CC=({struct Cyc_Absyn_Exp*_tmp9CB=({void*_tmp9CA=Cyc_Absyn_cstar_typ(ta1,_tmp431);Cyc_Toc_cast_it(_tmp9CA,
Cyc_Toc_member_exp(_tmp500,Cyc_Toc_curr_sp,0U));});
# 2792
Cyc_Toc_subscript_exp_r(_tmp9CB,_tmp4FF);});e->r=_tmp9CC;});}else{
# 2796
struct Cyc_Absyn_Exp*_tmp427=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmp9D0=Cyc_Toc_deref_exp_r(({void*_tmp9CF=Cyc_Absyn_cstar_typ(ta1,_tmp431);Cyc_Toc_cast_it(_tmp9CF,({
struct Cyc_Absyn_Exp*_tmp9CE=_tmp427;Cyc_Absyn_fncall_exp(_tmp9CE,({struct Cyc_Absyn_Exp*_tmp428[3U];_tmp428[0]=_tmp500,({
struct Cyc_Absyn_Exp*_tmp9CD=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp428[1]=_tmp9CD;}),_tmp428[2]=_tmp4FF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp428,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2797
e->r=_tmp9D0;});}
# 2802
goto _LLFA;}}_LLFA:;}
# 2804
goto _LLED;}default: _LLF2: _LLF3:
({void*_tmp42A=0U;({struct _dyneither_ptr _tmp9D1=({const char*_tmp42B="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp42B,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D1,_tag_dyneither(_tmp42A,sizeof(void*),0U));});});}_LLED:;}
# 2807
Cyc_Toc_set_lhs(nv,_tmp406);
goto _LL0;};}case 24U: _LL35: _tmp501=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL36:
# 2810
 if(!Cyc_Toc_is_toplevel(nv)){
# 2812
void*_tmp433=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp501));
{void*_tmp434=_tmp433;union Cyc_Absyn_AggrInfoU _tmp439;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp434)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp434)->f1).targs == 0){_LL100: _tmp439=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp434)->f1).aggr_info;_LL101: {
# 2815
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp439);
{int i=1;for(0;_tmp501 != 0;(_tmp501=_tmp501->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp501->hd);
({struct Cyc_List_List*_tmp9D4=({struct Cyc_List_List*_tmp435=_cycalloc(sizeof(*_tmp435));({struct _tuple20*_tmp9D3=({struct _dyneither_ptr*_tmp9D2=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9D2,(struct Cyc_Absyn_Exp*)_tmp501->hd);});_tmp435->hd=_tmp9D3;}),_tmp435->tl=dles;_tmp435;});dles=_tmp9D4;});}}
# 2821
({void*_tmp9D6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->tag=29U,_tmp436->f1=sd->name,_tmp436->f2=0,({struct Cyc_List_List*_tmp9D5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp436->f3=_tmp9D5;}),_tmp436->f4=sd;_tmp436;});e->r=_tmp9D6;});
e->topt=_tmp433;
goto _LLFF;}}else{goto _LL102;}}else{_LL102: _LL103:
({void*_tmp437=0U;({struct _dyneither_ptr _tmp9D7=({const char*_tmp438="tuple type not an aggregate";_tag_dyneither(_tmp438,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D7,_tag_dyneither(_tmp437,sizeof(void*),0U));});});}_LLFF:;}
# 2826
goto _LL0;}else{
# 2830
struct Cyc_List_List*dles=0;
for(0;_tmp501 != 0;_tmp501=_tmp501->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp501->hd);
({struct Cyc_List_List*_tmp9D9=({struct Cyc_List_List*_tmp43B=_cycalloc(sizeof(*_tmp43B));({struct _tuple20*_tmp9D8=({struct _tuple20*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->f1=0,_tmp43A->f2=(struct Cyc_Absyn_Exp*)_tmp501->hd;_tmp43A;});_tmp43B->hd=_tmp9D8;}),_tmp43B->tl=dles;_tmp43B;});dles=_tmp9D9;});}
# 2835
({void*_tmp9DA=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9DA;});}
# 2837
goto _LL0;case 26U: _LL37: _tmp502=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL38:
# 2843
{struct Cyc_List_List*_tmp43C=_tmp502;for(0;_tmp43C != 0;_tmp43C=_tmp43C->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp43C->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9DB=Cyc_Toc_unresolvedmem_exp_r(0,_tmp502);e->r=_tmp9DB;});
goto _LL0;case 27U: _LL39: _tmp506=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp505=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp504=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp503=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL3A:
# 2851
 Cyc_Toc_exp_to_c(nv,_tmp505);
Cyc_Toc_exp_to_c(nv,_tmp504);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp43D=Cyc_Evexp_eval_const_uint_exp(_tmp505);struct _tuple13 _tmp43E=_tmp43D;unsigned int _tmp448;int _tmp447;_LL105: _tmp448=_tmp43E.f1;_tmp447=_tmp43E.f2;_LL106:;{
void*_tmp43F=Cyc_Toc_typ_to_c((void*)_check_null(_tmp504->topt));
struct Cyc_List_List*es=0;
# 2858
if(!Cyc_Toc_is_zero(_tmp504)){
if(!_tmp447)
({void*_tmp440=0U;({unsigned int _tmp9DD=_tmp505->loc;struct _dyneither_ptr _tmp9DC=({const char*_tmp441="cannot determine value of constant";_tag_dyneither(_tmp441,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9DD,_tmp9DC,_tag_dyneither(_tmp440,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp448;++ i){
({struct Cyc_List_List*_tmp9DF=({struct Cyc_List_List*_tmp443=_cycalloc(sizeof(*_tmp443));({struct _tuple20*_tmp9DE=({struct _tuple20*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->f1=0,_tmp442->f2=_tmp504;_tmp442;});_tmp443->hd=_tmp9DE;}),_tmp443->tl=es;_tmp443;});es=_tmp9DF;});}}
# 2864
if(_tmp503){
struct Cyc_Absyn_Exp*_tmp444=({void*_tmp9E0=_tmp43F;Cyc_Toc_cast_it(_tmp9E0,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp9E3=({struct Cyc_List_List*_tmp9E2=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9E2,({struct Cyc_List_List*_tmp446=_cycalloc(sizeof(*_tmp446));({struct _tuple20*_tmp9E1=({struct _tuple20*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->f1=0,_tmp445->f2=_tmp444;_tmp445;});_tmp446->hd=_tmp9E1;}),_tmp446->tl=0;_tmp446;}));});es=_tmp9E3;});}}
# 2869
({void*_tmp9E4=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9E4;});};}
# 2871
goto _LL0;case 28U: _LL3B: _tmp509=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp508=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp507=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LL3C:
# 2878
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9E5=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9E5;});else{
# 2881
Cyc_Toc_exp_to_c(nv,_tmp509);}
goto _LL0;case 30U: _LL3D: _tmp50B=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp50A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL3E:
# 2885
{struct Cyc_List_List*_tmp449=_tmp50A;for(0;_tmp449 != 0;_tmp449=_tmp449->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp449->hd)).f2);}}{
void*_tmp44A=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp44A;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp44B=Cyc_Tcutil_compress(_tmp44A);void*_tmp44C=_tmp44B;union Cyc_Absyn_AggrInfoU _tmp450;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44C)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44C)->f1).targs == 0){_LL108: _tmp450=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44C)->f1).aggr_info;_LL109: {
# 2892
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp450);
({void*_tmp9E6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->tag=29U,_tmp44D->f1=sd->name,_tmp44D->f2=0,_tmp44D->f3=_tmp50A,_tmp44D->f4=sd;_tmp44D;});e->r=_tmp9E6;});
e->topt=_tmp44A;
goto _LL107;}}else{goto _LL10A;}}else{_LL10A: _LL10B:
({void*_tmp44E=0U;({struct _dyneither_ptr _tmp9E7=({const char*_tmp44F="anonStruct type not an aggregate";_tag_dyneither(_tmp44F,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E7,_tag_dyneither(_tmp44E,sizeof(void*),0U));});});}_LL107:;}else{
# 2899
({void*_tmp9E8=Cyc_Toc_unresolvedmem_exp_r(0,_tmp50A);e->r=_tmp9E8;});}
goto _LL0;};case 29U: _LL3F: _tmp50F=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp50E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp50D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_tmp50C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp309)->f4;_LL40:
# 2905
 if(_tmp50C == 0  || _tmp50C->impl == 0)
({void*_tmp451=0U;({struct _dyneither_ptr _tmp9E9=({const char*_tmp452="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp452,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9E9,_tag_dyneither(_tmp451,sizeof(void*),0U));});});{
void*_tmp453=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp454=Cyc_Tcutil_compress(_tmp453);void*_tmp455=_tmp454;union Cyc_Absyn_AggrInfoU _tmp458;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp455)->tag == 11U){_LL10D: _tmp458=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp455)->f1).aggr_info;_LL10E:
({struct _tuple1*_tmp9EA=(Cyc_Absyn_aggr_kinded_name(_tmp458)).f2;*_tmp50F=_tmp9EA;});goto _LL10C;}else{_LL10F: _LL110:
({void*_tmp456=0U;({struct _dyneither_ptr _tmp9EB=({const char*_tmp457="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp457,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9EB,_tag_dyneither(_tmp456,sizeof(void*),0U));});});}_LL10C:;}{
# 2916
struct Cyc_List_List*_tmp459=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->fields;
if(_tmp459 == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp459))->tl != 0;_tmp459=_tmp459->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp45A=(struct Cyc_Absyn_Aggrfield*)_tmp459->hd;
struct Cyc_List_List*_tmp45B=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp50D,_tmp50C->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->fields);
# 2923
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp45C=Cyc_Tcutil_compress(old_typ);void*_tmp45D=_tmp45C;struct Cyc_List_List*_tmp48A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45D)->tag == 11U){_LL112: _tmp48A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45D)->f1).targs;_LL113:
# 2927
{struct Cyc_List_List*_tmp45E=_tmp45B;for(0;_tmp45E != 0;_tmp45E=_tmp45E->tl){
struct _tuple34*_tmp45F=(struct _tuple34*)_tmp45E->hd;struct _tuple34*_tmp460=_tmp45F;struct Cyc_Absyn_Aggrfield*_tmp487;struct Cyc_Absyn_Exp*_tmp486;_LL117: _tmp487=_tmp460->f1;_tmp486=_tmp460->f2;_LL118:;{
void*_tmp461=_tmp486->topt;
Cyc_Toc_exp_to_c(nv,_tmp486);
# 2932
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp487->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp486->topt)))
({void*_tmp9ED=({void*_tmp9EC=Cyc_Toc_typ_to_c(_tmp487->type);Cyc_Toc_cast_it(_tmp9EC,
Cyc_Absyn_copy_exp(_tmp486));})->r;
# 2934
_tmp486->r=_tmp9ED;});
# 2937
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->tagged){
struct _dyneither_ptr*_tmp462=_tmp487->name;
struct Cyc_Absyn_Exp*_tmp463=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp50C,_tmp462),0U);
struct _tuple20*_tmp464=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp463);
struct _tuple20*_tmp465=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp486);
struct _tuple1*s=({struct _tuple1*_tmp470=_cycalloc(sizeof(*_tmp470));({union Cyc_Absyn_Nmspace _tmp9F1=Cyc_Absyn_Abs_n(0,1);_tmp470->f1=_tmp9F1;}),({
struct _dyneither_ptr*_tmp9F0=({struct _dyneither_ptr*_tmp46F=_cycalloc(sizeof(*_tmp46F));({struct _dyneither_ptr _tmp9EF=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp46D=({struct Cyc_String_pa_PrintArg_struct _tmp792;_tmp792.tag=0U,_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp50C->name).f2);_tmp792;});struct Cyc_String_pa_PrintArg_struct _tmp46E=({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0U,_tmp791.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp462);_tmp791;});void*_tmp46B[2U];_tmp46B[0]=& _tmp46D,_tmp46B[1]=& _tmp46E;({struct _dyneither_ptr _tmp9EE=({const char*_tmp46C="_union_%s_%s";_tag_dyneither(_tmp46C,sizeof(char),13U);});Cyc_aprintf(_tmp9EE,_tag_dyneither(_tmp46B,sizeof(void*),2U));});});*_tmp46F=_tmp9EF;});_tmp46F;});_tmp470->f2=_tmp9F0;});_tmp470;});
# 2945
struct _tuple20*_tmp466=({
struct _dyneither_ptr*_tmp9F3=_tmp462;Cyc_Toc_make_field(_tmp9F3,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=29U,_tmp46A->f1=s,_tmp46A->f2=0,({struct Cyc_List_List*_tmp9F2=({struct _tuple20*_tmp469[2U];_tmp469[0]=_tmp464,_tmp469[1]=_tmp465;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp469,sizeof(struct _tuple20*),2U));});_tmp46A->f3=_tmp9F2;}),_tmp46A->f4=0;_tmp46A;}),0U));});
# 2948
({void*_tmp9F5=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->tag=29U,_tmp468->f1=*_tmp50F,_tmp468->f2=0,({struct Cyc_List_List*_tmp9F4=({struct _tuple20*_tmp467[1U];_tmp467[0]=_tmp466;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp467,sizeof(struct _tuple20*),1U));});_tmp468->f3=_tmp9F4;}),_tmp468->f4=_tmp50C;_tmp468;});e->r=_tmp9F5;});}
# 2954
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp45A == _tmp487){
struct Cyc_List_List*_tmp471=({struct Cyc_List_List*_tmp9F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->exist_vars,_tmp50E);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9F6,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp50C->tvs,_tmp48A));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp472=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->fields;for(0;_tmp472 != 0;_tmp472=_tmp472->tl){
struct Cyc_Absyn_Aggrfield*_tmp473=(struct Cyc_Absyn_Aggrfield*)_tmp472->hd;
void*_tmp474=Cyc_Tcutil_substitute(_tmp471,_tmp473->type);
struct Cyc_Absyn_Aggrfield*_tmp475=({struct Cyc_Absyn_Aggrfield*_tmp9F7=_tmp473;Cyc_Toc_aggrfield_to_c(_tmp9F7,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp471,_tmp474)));});
({struct Cyc_List_List*_tmp9F8=({struct Cyc_List_List*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->hd=_tmp475,_tmp476->tl=new_fields;_tmp476;});new_fields=_tmp9F8;});
# 2969
if(_tmp472->tl == 0){
{void*_tmp477=Cyc_Tcutil_compress(_tmp475->type);void*_tmp478=_tmp477;struct Cyc_Absyn_ArrayInfo _tmp47D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp478)->tag == 8U){_LL11A: _tmp47D=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp478)->f1;_LL11B:
# 2972
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp47D.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp479=_tmp47D;
({struct Cyc_Absyn_Exp*_tmp9F9=Cyc_Absyn_uint_exp(0U,0U);_tmp479.num_elts=_tmp9F9;});
({void*_tmp9FA=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->tag=8U,_tmp47A->f1=_tmp479;_tmp47A;});_tmp475->type=_tmp9FA;});}
# 2977
goto _LL119;}else{_LL11C: _LL11D:
# 2981
 if(_tmp486->topt == 0)
goto _LL119;
{void*_tmp47B=Cyc_Tcutil_compress((void*)_check_null(_tmp486->topt));void*_tmp47C=_tmp47B;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47C)->tag == 11U){_LL11F: _LL120:
# 2985
 _tmp475->type=(void*)_check_null(_tmp486->topt);goto _LL11E;}else{_LL121: _LL122:
 goto _LL11E;}_LL11E:;}
# 2988
goto _LL119;}_LL119:;}
# 2992
if(!Cyc_Toc_is_array_type(_tmp473->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp473->type)))
({struct Cyc_List_List*_tmp9FC=({struct Cyc_List_List*_tmp47F=_cycalloc(sizeof(*_tmp47F));({void*_tmp9FB=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->tag=6U,_tmp47E->f1=0;_tmp47E;});_tmp47F->hd=_tmp9FB;}),_tmp47F->tl=_tmp475->attributes;_tmp47F;});_tmp475->attributes=_tmp9FC;});}}}
# 2998
({struct Cyc_Absyn_Aggrdecl*_tmpA00=({struct _dyneither_ptr*_tmp9FF=({struct _dyneither_ptr*_tmp483=_cycalloc(sizeof(*_tmp483));({struct _dyneither_ptr _tmp9FE=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp482=({struct Cyc_Int_pa_PrintArg_struct _tmp793;_tmp793.tag=1U,_tmp793.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp793;});void*_tmp480[1U];_tmp480[0]=& _tmp482;({struct _dyneither_ptr _tmp9FD=({const char*_tmp481="_genStruct%d";_tag_dyneither(_tmp481,sizeof(char),13U);});Cyc_aprintf(_tmp9FD,_tag_dyneither(_tmp480,sizeof(void*),1U));});});*_tmp483=_tmp9FE;});_tmp483;});Cyc_Toc_make_c_struct_defn(_tmp9FF,
# 3000
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 2998
_tmp50C=_tmpA00;});
# 3001
*_tmp50F=_tmp50C->name;
Cyc_Toc_aggrdecl_to_c(_tmp50C,1);
# 3004
({void*_tmpA02=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpA01=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp484=_cycalloc(sizeof(*_tmp484));*_tmp484=_tmp50C;_tmp484;}));(_tmp485->f1).aggr_info=_tmpA01;}),(_tmp485->f1).targs=0;_tmp485;});e->topt=_tmpA02;});}};}}
# 3007
goto _LL111;}else{_LL114: _LL115:
({void*_tmp488=0U;({struct _dyneither_ptr _tmpA03=({const char*_tmp489="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp489,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA03,_tag_dyneither(_tmp488,sizeof(void*),0U));});});}_LL111:;}else{
# 3016
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->tagged){
# 3018
struct _tuple34*_tmp48B=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp45B))->hd;struct _tuple34*_tmp48C=_tmp48B;struct Cyc_Absyn_Aggrfield*_tmp495;struct Cyc_Absyn_Exp*_tmp494;_LL124: _tmp495=_tmp48C->f1;_tmp494=_tmp48C->f2;_LL125:;{
void*_tmp48D=(void*)_check_null(_tmp494->topt);
void*_tmp48E=_tmp495->type;
Cyc_Toc_exp_to_c(nv,_tmp494);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp48E) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp48D))
({void*_tmpA05=({
void*_tmpA04=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpA04,Cyc_Absyn_new_exp(_tmp494->r,0U));});
# 3024
_tmp494->r=_tmpA05;});{
# 3027
int i=Cyc_Toc_get_member_offset(_tmp50C,_tmp495->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp48F=({struct _tuple20*_tmp491[2U];({struct _tuple20*_tmpA07=({struct _tuple20*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->f1=0,_tmp492->f2=field_tag_exp;_tmp492;});_tmp491[0]=_tmpA07;}),({struct _tuple20*_tmpA06=({struct _tuple20*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->f1=0,_tmp493->f2=_tmp494;_tmp493;});_tmp491[1]=_tmpA06;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp491,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp48F,0U);
({void*_tmpA09=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp490[1U];({struct _tuple20*_tmpA08=Cyc_Toc_make_field(_tmp495->name,umem);_tmp490[0]=_tmpA08;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp490,sizeof(struct _tuple20*),1U));}));e->r=_tmpA09;});};};}else{
# 3034
struct Cyc_List_List*_tmp496=0;
struct Cyc_List_List*_tmp497=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50C->impl))->fields;
for(0;_tmp497 != 0;_tmp497=_tmp497->tl){
struct Cyc_List_List*_tmp498=_tmp45B;for(0;_tmp498 != 0;_tmp498=_tmp498->tl){
if((*((struct _tuple34*)_tmp498->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp497->hd){
struct _tuple34*_tmp499=(struct _tuple34*)_tmp498->hd;struct _tuple34*_tmp49A=_tmp499;struct Cyc_Absyn_Aggrfield*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;_LL127: _tmp4A0=_tmp49A->f1;_tmp49F=_tmp49A->f2;_LL128:;{
void*_tmp49B=Cyc_Toc_typ_to_c(_tmp4A0->type);
void*_tmp49C=Cyc_Toc_typ_to_c((void*)_check_null(_tmp49F->topt));
Cyc_Toc_exp_to_c(nv,_tmp49F);
# 3044
if(!Cyc_Tcutil_unify(_tmp49B,_tmp49C)){
# 3046
if(!Cyc_Tcutil_is_arithmetic_type(_tmp49B) || !
Cyc_Tcutil_is_arithmetic_type(_tmp49C))
({struct Cyc_Absyn_Exp*_tmpA0B=({void*_tmpA0A=_tmp49B;Cyc_Toc_cast_it(_tmpA0A,Cyc_Absyn_copy_exp(_tmp49F));});_tmp49F=_tmpA0B;});}
({struct Cyc_List_List*_tmpA0D=({struct Cyc_List_List*_tmp49E=_cycalloc(sizeof(*_tmp49E));({struct _tuple20*_tmpA0C=({struct _tuple20*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->f1=0,_tmp49D->f2=_tmp49F;_tmp49D;});_tmp49E->hd=_tmpA0C;}),_tmp49E->tl=_tmp496;_tmp49E;});_tmp496=_tmpA0D;});
break;};}}}
# 3053
({void*_tmpA0E=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp496));e->r=_tmpA0E;});}}
# 3056
goto _LL0;};};};case 31U: _LL41: _tmp512=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp511=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_tmp510=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp309)->f3;_LL42: {
# 3060
struct Cyc_List_List*_tmp4A1=_tmp510->typs;
{struct Cyc_List_List*_tmp4A2=_tmp512;for(0;_tmp4A2 != 0;(_tmp4A2=_tmp4A2->tl,_tmp4A1=_tmp4A1->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4A2->hd;
void*_tmp4A3=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4A1))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp4A3))
({void*_tmpA0F=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA0F;});}}{
# 3071
struct Cyc_Absyn_Exp*_tmp4A4=_tmp511->is_extensible?Cyc_Absyn_var_exp(_tmp510->name,0U):
 Cyc_Toc_datatype_tag(_tmp511,_tmp510->name);
# 3074
if(!Cyc_Toc_is_toplevel(nv)){
# 3076
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp512 != 0;(_tmp512=_tmp512->tl,++ i)){
({struct Cyc_List_List*_tmpA12=({struct Cyc_List_List*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));({struct _tuple20*_tmpA11=({struct _dyneither_ptr*_tmpA10=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA10,(struct Cyc_Absyn_Exp*)_tmp512->hd);});_tmp4A5->hd=_tmpA11;}),_tmp4A5->tl=dles;_tmp4A5;});dles=_tmpA12;});}}{
# 3082
struct _tuple20*_tmp4A6=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp4A4);
({void*_tmpA16=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8->tag=29U,({struct _tuple1*_tmpA15=Cyc_Toc_collapse_qvars(_tmp510->name,_tmp511->name);_tmp4A8->f1=_tmpA15;}),_tmp4A8->f2=0,({
struct Cyc_List_List*_tmpA14=({struct Cyc_List_List*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->hd=_tmp4A6,({struct Cyc_List_List*_tmpA13=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp4A7->tl=_tmpA13;});_tmp4A7;});_tmp4A8->f3=_tmpA14;}),_tmp4A8->f4=0;_tmp4A8;});
# 3083
e->r=_tmpA16;});};}else{
# 3088
struct Cyc_List_List*_tmp4A9=0;
for(0;_tmp512 != 0;_tmp512=_tmp512->tl){
({struct Cyc_List_List*_tmpA18=({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct _tuple20*_tmpA17=({struct _tuple20*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->f1=0,_tmp4AA->f2=(struct Cyc_Absyn_Exp*)_tmp512->hd;_tmp4AA;});_tmp4AB->hd=_tmpA17;}),_tmp4AB->tl=_tmp4A9;_tmp4AB;});_tmp4A9=_tmpA18;});}
({void*_tmpA1B=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct _tuple20*_tmpA1A=({struct _tuple20*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->f1=0,_tmp4AC->f2=_tmp4A4;_tmp4AC;});_tmp4AD->hd=_tmpA1A;}),({
struct Cyc_List_List*_tmpA19=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4A9);_tmp4AD->tl=_tmpA19;});_tmp4AD;}));
# 3091
e->r=_tmpA1B;});}
# 3094
goto _LL0;};}case 32U: _LL43: _LL44:
# 3096
 goto _LL46;case 33U: _LL45: _LL46:
 goto _LL0;case 34U: _LL47: _tmp518=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).is_calloc;_tmp517=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).rgn;_tmp516=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).elt_type;_tmp515=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).num_elts;_tmp514=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).fat_result;_tmp513=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp309)->f1).inline_call;_LL48: {
# 3100
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp516)));
Cyc_Toc_exp_to_c(nv,_tmp515);
# 3104
if(_tmp514){
struct _tuple1*_tmp4AE=Cyc_Toc_temp_var();
struct _tuple1*_tmp4AF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp518){
xexp=_tmp515;
if(_tmp517 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp517;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA1E=({struct Cyc_Absyn_Exp*_tmpA1D=rgn;struct Cyc_Absyn_Exp*_tmpA1C=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA1D,_tmpA1C,Cyc_Absyn_var_exp(_tmp4AE,0U));});pexp=_tmpA1E;});}else{
# 3115
({struct Cyc_Absyn_Exp*_tmpA21=({void*_tmpA20=*_tmp516;struct Cyc_Absyn_Exp*_tmpA1F=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA20,_tmpA1F,Cyc_Absyn_var_exp(_tmp4AE,0U));});pexp=_tmpA21;});}
# 3117
({struct Cyc_Absyn_Exp*_tmpA26=({struct Cyc_Absyn_Exp*_tmpA25=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA25,({struct Cyc_Absyn_Exp*_tmp4B0[3U];({
struct Cyc_Absyn_Exp*_tmpA24=Cyc_Absyn_var_exp(_tmp4AF,0U);_tmp4B0[0]=_tmpA24;}),({struct Cyc_Absyn_Exp*_tmpA23=Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp4B0[1]=_tmpA23;}),({
struct Cyc_Absyn_Exp*_tmpA22=Cyc_Absyn_var_exp(_tmp4AE,0U);_tmp4B0[2]=_tmpA22;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B0,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3117
rexp=_tmpA26;});}else{
# 3121
if(_tmp517 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp517;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp513)
({struct Cyc_Absyn_Exp*_tmpA28=({struct Cyc_Absyn_Exp*_tmpA27=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA27,Cyc_Absyn_var_exp(_tmp4AE,0U));});pexp=_tmpA28;});else{
# 3127
({struct Cyc_Absyn_Exp*_tmpA2A=({struct Cyc_Absyn_Exp*_tmpA29=rgn;Cyc_Toc_rmalloc_exp(_tmpA29,Cyc_Absyn_var_exp(_tmp4AE,0U));});pexp=_tmpA2A;});}}else{
# 3129
({struct Cyc_Absyn_Exp*_tmpA2C=({void*_tmpA2B=*_tmp516;Cyc_Toc_malloc_exp(_tmpA2B,Cyc_Absyn_var_exp(_tmp4AE,0U));});pexp=_tmpA2C;});}
# 3131
({struct Cyc_Absyn_Exp*_tmpA31=({struct Cyc_Absyn_Exp*_tmpA30=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA30,({struct Cyc_Absyn_Exp*_tmp4B1[3U];({struct Cyc_Absyn_Exp*_tmpA2F=Cyc_Absyn_var_exp(_tmp4AF,0U);_tmp4B1[0]=_tmpA2F;}),({struct Cyc_Absyn_Exp*_tmpA2E=Cyc_Absyn_uint_exp(1U,0U);_tmp4B1[1]=_tmpA2E;}),({
struct Cyc_Absyn_Exp*_tmpA2D=Cyc_Absyn_var_exp(_tmp4AE,0U);_tmp4B1[2]=_tmpA2D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B1,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3131
rexp=_tmpA31;});}{
# 3134
struct Cyc_Absyn_Stmt*_tmp4B2=({struct _tuple1*_tmpA37=_tmp4AE;void*_tmpA36=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA35=_tmp515;Cyc_Absyn_declare_stmt(_tmpA37,_tmpA36,_tmpA35,({
struct _tuple1*_tmpA34=_tmp4AF;void*_tmpA33=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA32=pexp;Cyc_Absyn_declare_stmt(_tmpA34,_tmpA33,_tmpA32,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA38=Cyc_Toc_stmt_exp_r(_tmp4B2);e->r=_tmpA38;});};}else{
# 3139
if(_tmp518){
if(_tmp517 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp517;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA3B=({struct Cyc_Absyn_Exp*_tmpA3A=rgn;struct Cyc_Absyn_Exp*_tmpA39=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA3A,_tmpA39,_tmp515);})->r;e->r=_tmpA3B;});}else{
# 3145
({void*_tmpA3E=({void*_tmpA3D=*_tmp516;struct Cyc_Absyn_Exp*_tmpA3C=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA3D,_tmpA3C,_tmp515);})->r;e->r=_tmpA3E;});}}else{
# 3148
if(_tmp517 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp517;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp513)
({void*_tmpA3F=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp515))->r;e->r=_tmpA3F;});else{
# 3154
({void*_tmpA40=(Cyc_Toc_rmalloc_exp(rgn,_tmp515))->r;e->r=_tmpA40;});}}else{
# 3156
({void*_tmpA41=(Cyc_Toc_malloc_exp(*_tmp516,_tmp515))->r;e->r=_tmpA41;});}}}
# 3160
goto _LL0;}case 35U: _LL49: _tmp51A=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp519=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL4A: {
# 3169
void*e1_old_typ=(void*)_check_null(_tmp51A->topt);
void*e2_old_typ=(void*)_check_null(_tmp519->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4B3=0U;({struct _dyneither_ptr _tmpA42=({const char*_tmp4B4="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4B4,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA42,_tag_dyneither(_tmp4B3,sizeof(void*),0U));});});{
# 3177
unsigned int _tmp4B5=e->loc;
struct _tuple1*_tmp4B6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B7=Cyc_Absyn_var_exp(_tmp4B6,_tmp4B5);_tmp4B7->topt=e1_old_typ;{
struct _tuple1*_tmp4B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_Absyn_var_exp(_tmp4B8,_tmp4B5);_tmp4B9->topt=e2_old_typ;{
# 3183
struct Cyc_Absyn_Exp*_tmp4BA=({struct Cyc_Absyn_Exp*_tmpA44=Cyc_Tcutil_deep_copy_exp(1,_tmp51A);struct Cyc_Absyn_Exp*_tmpA43=_tmp4B9;Cyc_Absyn_assign_exp(_tmpA44,_tmpA43,_tmp4B5);});_tmp4BA->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4BB=Cyc_Absyn_exp_stmt(_tmp4BA,_tmp4B5);
struct Cyc_Absyn_Exp*_tmp4BC=({struct Cyc_Absyn_Exp*_tmpA46=Cyc_Tcutil_deep_copy_exp(1,_tmp519);struct Cyc_Absyn_Exp*_tmpA45=_tmp4B7;Cyc_Absyn_assign_exp(_tmpA46,_tmpA45,_tmp4B5);});_tmp4BC->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4BD=Cyc_Absyn_exp_stmt(_tmp4BC,_tmp4B5);
# 3188
struct Cyc_Absyn_Stmt*_tmp4BE=({struct _tuple1*_tmpA4E=_tmp4B6;void*_tmpA4D=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA4C=_tmp51A;struct Cyc_Absyn_Stmt*_tmpA4B=({
struct _tuple1*_tmpA4A=_tmp4B8;void*_tmpA49=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA48=_tmp519;struct Cyc_Absyn_Stmt*_tmpA47=
Cyc_Absyn_seq_stmt(_tmp4BB,_tmp4BD,_tmp4B5);
# 3189
Cyc_Absyn_declare_stmt(_tmpA4A,_tmpA49,_tmpA48,_tmpA47,_tmp4B5);});
# 3188
Cyc_Absyn_declare_stmt(_tmpA4E,_tmpA4D,_tmpA4C,_tmpA4B,_tmp4B5);});
# 3191
Cyc_Toc_stmt_to_c(nv,_tmp4BE);
({void*_tmpA4F=Cyc_Toc_stmt_exp_r(_tmp4BE);e->r=_tmpA4F;});
goto _LL0;};};};};};}case 38U: _LL4B: _tmp51C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_tmp51B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp309)->f2;_LL4C: {
# 3196
void*_tmp4BF=Cyc_Tcutil_compress((void*)_check_null(_tmp51C->topt));
Cyc_Toc_exp_to_c(nv,_tmp51C);
{void*_tmp4C0=_tmp4BF;struct Cyc_Absyn_Aggrdecl*_tmp4C6;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C0)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C0)->f1).aggr_info).KnownAggr).tag == 2){_LL12A: _tmp4C6=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C0)->f1).aggr_info).KnownAggr).val;_LL12B: {
# 3200
struct Cyc_Absyn_Exp*_tmp4C1=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4C6,_tmp51B),0U);
struct Cyc_Absyn_Exp*_tmp4C2=Cyc_Toc_member_exp(_tmp51C,_tmp51B,0U);
struct Cyc_Absyn_Exp*_tmp4C3=Cyc_Toc_member_exp(_tmp4C2,Cyc_Toc_tag_sp,0U);
({void*_tmpA50=(Cyc_Absyn_eq_exp(_tmp4C3,_tmp4C1,0U))->r;e->r=_tmpA50;});
goto _LL129;}}else{goto _LL12C;}}else{_LL12C: _LL12D:
({void*_tmp4C4=0U;({struct _dyneither_ptr _tmpA51=({const char*_tmp4C5="non-aggregate type in tagcheck";_tag_dyneither(_tmp4C5,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA51,_tag_dyneither(_tmp4C4,sizeof(void*),0U));});});}_LL129:;}
# 3207
goto _LL0;}case 37U: _LL4D: _tmp51D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp309)->f1;_LL4E:
 Cyc_Toc_stmt_to_c(nv,_tmp51D);goto _LL0;case 36U: _LL4F: _LL50:
({void*_tmp4C7=0U;({struct _dyneither_ptr _tmpA52=({const char*_tmp4C8="UnresolvedMem";_tag_dyneither(_tmp4C8,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA52,_tag_dyneither(_tmp4C7,sizeof(void*),0U));});});case 25U: _LL51: _LL52:
({void*_tmp4C9=0U;({struct _dyneither_ptr _tmpA53=({const char*_tmp4CA="compoundlit";_tag_dyneither(_tmp4CA,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA53,_tag_dyneither(_tmp4C9,sizeof(void*),0U));});});case 39U: _LL53: _LL54:
({void*_tmp4CB=0U;({struct _dyneither_ptr _tmpA54=({const char*_tmp4CC="valueof(-)";_tag_dyneither(_tmp4CC,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA54,_tag_dyneither(_tmp4CB,sizeof(void*),0U));});});default: _LL55: _LL56:
({void*_tmp4CD=0U;({struct _dyneither_ptr _tmpA55=({const char*_tmp4CE="__asm__";_tag_dyneither(_tmp4CE,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA55,_tag_dyneither(_tmp4CD,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3240 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3242
return({struct _tuple35*_tmp51E=_region_malloc(r,sizeof(*_tmp51E));_tmp51E->f1=0,({struct _dyneither_ptr*_tmpA57=Cyc_Toc_fresh_label();_tmp51E->f2=_tmpA57;}),({struct _dyneither_ptr*_tmpA56=Cyc_Toc_fresh_label();_tmp51E->f3=_tmpA56;}),_tmp51E->f4=sc;_tmp51E;});}
# 3247
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp51F=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3253
if((int)(((_tmp51F->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp522=_tmp51F->orig_pat;if((_tmp522.pattern).tag != 1)_throw_match();(_tmp522.pattern).val;})->topt);
void*_tmp520=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp521=_tmp520;switch(*((int*)_tmp521)){case 0U: _LL1: _LL2:
# 3258
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA59=({void*_tmpA58=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA58,v);});v=_tmpA59;});goto _LL0;}_LL0:;}{
# 3264
void*_tmp523=_tmp51F->access;void*_tmp524=_tmp523;struct Cyc_Absyn_Datatypedecl*_tmp52F;struct Cyc_Absyn_Datatypefield*_tmp52E;unsigned int _tmp52D;int _tmp52C;struct _dyneither_ptr*_tmp52B;unsigned int _tmp52A;switch(*((int*)_tmp524)){case 0U: _LLA: _LLB:
# 3269
 goto _LL9;case 1U: _LLC: _LLD:
# 3274
 if(ps->tl != 0){
void*_tmp525=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp526=_tmp525;struct Cyc_Absyn_Datatypedecl*_tmp529;struct Cyc_Absyn_Datatypefield*_tmp528;unsigned int _tmp527;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp526)->tag == 3U){_LL15: _tmp529=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp526)->f1;_tmp528=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp526)->f2;_tmp527=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp526)->f3;_LL16:
# 3277
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA5C=({void*_tmpA5B=({void*_tmpA5A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp528->name,_tmp529->name));Cyc_Absyn_cstar_typ(_tmpA5A,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA5B,v);});v=_tmpA5C;});
({struct Cyc_Absyn_Exp*_tmpA5E=({struct Cyc_Absyn_Exp*_tmpA5D=v;Cyc_Absyn_aggrarrow_exp(_tmpA5D,Cyc_Absyn_fieldname((int)(_tmp527 + 1)),0U);});v=_tmpA5E;});
continue;}else{_LL17: _LL18:
# 3282
 goto _LL14;}_LL14:;}
# 3285
({struct Cyc_Absyn_Exp*_tmpA5F=Cyc_Absyn_deref_exp(v,0U);v=_tmpA5F;});
goto _LL9;case 2U: _LLE: _tmp52A=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp524)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA61=({struct Cyc_Absyn_Exp*_tmpA60=v;Cyc_Toc_member_exp(_tmpA60,Cyc_Absyn_fieldname((int)(_tmp52A + 1)),0U);});v=_tmpA61;});goto _LL9;case 4U: _LL10: _tmp52C=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp524)->f1;_tmp52B=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp524)->f2;_LL11:
# 3289
({struct Cyc_Absyn_Exp*_tmpA62=Cyc_Toc_member_exp(v,_tmp52B,0U);v=_tmpA62;});
if(_tmp52C)
({struct Cyc_Absyn_Exp*_tmpA63=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA63;});
goto _LL9;default: _LL12: _tmp52F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp524)->f1;_tmp52E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp524)->f2;_tmp52D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp524)->f3;_LL13:
# 3294
({struct Cyc_Absyn_Exp*_tmpA65=({struct Cyc_Absyn_Exp*_tmpA64=v;Cyc_Toc_member_exp(_tmpA64,Cyc_Absyn_fieldname((int)(_tmp52D + 1)),0U);});v=_tmpA65;});
goto _LL9;}_LL9:;};}
# 3298
return v;}
# 3303
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp530=t;struct Cyc_Absyn_Datatypedecl*_tmp549;struct Cyc_Absyn_Datatypefield*_tmp548;struct _dyneither_ptr*_tmp547;int _tmp546;int _tmp545;struct Cyc_Absyn_Datatypedecl*_tmp544;struct Cyc_Absyn_Datatypefield*_tmp543;unsigned int _tmp542;struct _dyneither_ptr _tmp541;int _tmp540;void*_tmp53F;struct Cyc_Absyn_Enumfield*_tmp53E;struct Cyc_Absyn_Enumdecl*_tmp53D;struct Cyc_Absyn_Enumfield*_tmp53C;struct Cyc_Absyn_Exp*_tmp53B;switch(*((int*)_tmp530)){case 0U: _LL1: _tmp53B=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp530)->f1;_LL2:
# 3307
 if(_tmp53B == 0)return v;else{return _tmp53B;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA66=v;Cyc_Absyn_eq_exp(_tmpA66,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA67=v;Cyc_Absyn_neq_exp(_tmpA67,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp53D=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp530)->f1;_tmp53C=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp530)->f2;_LL8:
# 3311
 return({struct Cyc_Absyn_Exp*_tmpA68=v;Cyc_Absyn_eq_exp(_tmpA68,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->tag=32U,_tmp531->f1=_tmp53D,_tmp531->f2=_tmp53C;_tmp531;}),0U),0U);});case 4U: _LL9: _tmp53F=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp530)->f1;_tmp53E=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp530)->f2;_LLA:
# 3313
 return({struct Cyc_Absyn_Exp*_tmpA69=v;Cyc_Absyn_eq_exp(_tmpA69,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->tag=33U,_tmp532->f1=_tmp53F,_tmp532->f2=_tmp53E;_tmp532;}),0U),0U);});case 5U: _LLB: _tmp541=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp530)->f1;_tmp540=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp530)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA6A=v;Cyc_Absyn_eq_exp(_tmpA6A,Cyc_Absyn_float_exp(_tmp541,_tmp540,0U),0U);});case 6U: _LLD: _tmp542=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp530)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA6B=v;Cyc_Absyn_eq_exp(_tmpA6B,Cyc_Absyn_signed_int_exp((int)_tmp542,0U),0U);});case 7U: _LLF: _tmp545=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp530)->f1;_tmp544=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp530)->f2;_tmp543=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp530)->f3;_LL10:
# 3319
 LOOP1: {
void*_tmp533=v->r;void*_tmp534=_tmp533;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp535;switch(*((int*)_tmp534)){case 14U: _LL16: _tmp535=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp534)->f2;_LL17:
 v=_tmp535;goto LOOP1;case 20U: _LL18: _tmp536=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp534)->f1;_LL19:
 v=_tmp536;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3326
({struct Cyc_Absyn_Exp*_tmpA6E=({void*_tmpA6D=({void*_tmpA6C=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp543->name,_tmp544->name));Cyc_Absyn_cstar_typ(_tmpA6C,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA6D,v);});v=_tmpA6E;});
return({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA6F,Cyc_Absyn_uint_exp((unsigned int)_tmp545,0U),0U);});case 8U: _LL11: _tmp547=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp530)->f1;_tmp546=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp530)->f2;_LL12:
# 3331
({struct Cyc_Absyn_Exp*_tmpA71=({struct Cyc_Absyn_Exp*_tmpA70=Cyc_Toc_member_exp(v,_tmp547,0U);Cyc_Toc_member_exp(_tmpA70,Cyc_Toc_tag_sp,0U);});v=_tmpA71;});
return({struct Cyc_Absyn_Exp*_tmpA72=v;Cyc_Absyn_eq_exp(_tmpA72,Cyc_Absyn_signed_int_exp(_tmp546,0U),0U);});default: _LL13: _tmp549=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp530)->f1;_tmp548=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp530)->f2;_LL14:
# 3335
 LOOP2: {
void*_tmp537=v->r;void*_tmp538=_tmp537;struct Cyc_Absyn_Exp*_tmp53A;struct Cyc_Absyn_Exp*_tmp539;switch(*((int*)_tmp538)){case 14U: _LL1D: _tmp539=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp538)->f2;_LL1E:
 v=_tmp539;goto LOOP2;case 20U: _LL1F: _tmp53A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp538)->f1;_LL20:
 v=_tmp53A;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3342
({struct Cyc_Absyn_Exp*_tmpA75=({void*_tmpA74=({void*_tmpA73=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp548->name,_tmp549->name));Cyc_Absyn_cstar_typ(_tmpA73,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA74,v);});v=_tmpA75;});
return({struct Cyc_Absyn_Exp*_tmpA76=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA76,Cyc_Absyn_var_exp(_tmp548->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3356
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3364
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3366
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp54A=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp54B=_tmp54A;void*_tmp551;_LL1: _tmp551=_tmp54B.f1;_LL2:;{
void*_tmp54C=_tmp551;struct Cyc_Absyn_Exp*_tmp550;struct _dyneither_ptr*_tmp54F;switch(*((int*)_tmp54C)){case 3U: _LL4: _LL5:
# 3372
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3375
 continue;case 8U: _LLA: _tmp54F=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp54C)->f1;_LLB:
# 3377
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA77=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->tag=3U,_tmp54D->f1=_tmp54F;_tmp54D;});k=_tmpA77;});
continue;case 7U: _LLC: _LLD:
# 3381
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp550=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp54C)->f1;if(_tmp550 != 0){_LLF:
# 3385
({void*_tmpA78=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->tag=2U,_tmp54E->f1=_tmp550;_tmp54E;});k=_tmpA78;});
return({struct _tuple36 _tmp794;_tmp794.f1=0,_tmp794.f2=k;_tmp794;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp795;_tmp795.f1=0,_tmp795.f2=k;_tmp795;});}_LL3:;};}
# 3394
return({struct _tuple36 _tmp796;_tmp796.f1=c,_tmp796.f2=k;_tmp796;});}
# 3399
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp552=p;int _tmp55D;int _tmp55C;unsigned int _tmp55B;void*_tmp55A;struct Cyc_Absyn_Enumfield*_tmp559;struct Cyc_Absyn_Enumdecl*_tmp558;struct Cyc_Absyn_Enumfield*_tmp557;switch(*((int*)_tmp552)){case 3U: _LL1: _tmp558=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp552)->f1;_tmp557=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp552)->f2;_LL2:
# 3403
({struct Cyc_Absyn_Exp*_tmpA79=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=32U,_tmp553->f1=_tmp558,_tmp553->f2=_tmp557;_tmp553;}),0U);e=_tmpA79;});goto _LL0;case 4U: _LL3: _tmp55A=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp552)->f1;_tmp559=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp552)->f2;_LL4:
# 3405
({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->tag=33U,_tmp554->f1=_tmp55A,_tmp554->f2=_tmp559;_tmp554;}),0U);e=_tmpA7A;});goto _LL0;case 6U: _LL5: _tmp55B=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp552)->f1;_LL6:
 _tmp55C=(int)_tmp55B;goto _LL8;case 7U: _LL7: _tmp55C=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp552)->f1;_LL8:
 _tmp55D=_tmp55C;goto _LLA;case 8U: _LL9: _tmp55D=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp552)->f2;_LLA:
# 3409
({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Absyn_uint_exp((unsigned int)_tmp55D,0U);e=_tmpA7B;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp555=0U;({struct _dyneither_ptr _tmpA7C=({const char*_tmp556="compile_pat_test_as_case!";_tag_dyneither(_tmp556,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA7C,_tag_dyneither(_tmp555,sizeof(void*),0U));});});}_LL0:;}
# 3413
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->tag=17U,_tmp55E->f1=e;_tmp55E;}),0U);}
# 3417
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3426
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3429
void*t=(void*)_check_null(p->topt);
void*_tmp55F=p->r;void*_tmp560=_tmp55F;union Cyc_Absyn_AggrInfoU _tmp591;struct Cyc_List_List*_tmp590;struct Cyc_List_List*_tmp58F;struct Cyc_List_List*_tmp58E;struct Cyc_Absyn_Pat*_tmp58D;struct Cyc_Absyn_Datatypedecl*_tmp58C;struct Cyc_Absyn_Datatypefield*_tmp58B;struct Cyc_List_List*_tmp58A;struct Cyc_Absyn_Vardecl*_tmp589;struct Cyc_Absyn_Pat*_tmp588;struct Cyc_Absyn_Vardecl*_tmp587;struct Cyc_Absyn_Vardecl*_tmp586;struct Cyc_Absyn_Pat*_tmp585;struct Cyc_Absyn_Vardecl*_tmp584;switch(*((int*)_tmp560)){case 2U: _LL1: _tmp584=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp560)->f2;_LL2: {
# 3432
struct Cyc_Absyn_Pat*_tmp561=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp561->topt=p->topt;
_tmp586=_tmp584;_tmp585=_tmp561;goto _LL4;}case 1U: _LL3: _tmp586=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp560)->f1;_tmp585=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp560)->f2;_LL4:
# 3437
({struct Cyc_List_List*_tmpA7D=({struct Cyc_List_List*_tmp562=_region_malloc(rgn,sizeof(*_tmp562));_tmp562->hd=_tmp586,_tmp562->tl=*decls;_tmp562;});*decls=_tmpA7D;});{
struct Cyc_Absyn_Stmt*_tmp563=({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->tag=4U,_tmp564->f1=_tmp586;_tmp564;}),0U);Cyc_Absyn_assign_stmt(_tmpA7E,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA7F=_tmp563;Cyc_Toc_seq_stmt_opt(_tmpA7F,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp585));});};case 4U: _LL5: _tmp587=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp560)->f2;_LL6:
# 3442
({struct Cyc_List_List*_tmpA80=({struct Cyc_List_List*_tmp565=_region_malloc(rgn,sizeof(*_tmp565));_tmp565->hd=_tmp587,_tmp565->tl=*decls;_tmp565;});*decls=_tmpA80;});
return({struct Cyc_Absyn_Exp*_tmpA81=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566->tag=4U,_tmp566->f1=_tmp587;_tmp566;}),0U);Cyc_Absyn_assign_stmt(_tmpA81,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp589=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp560)->f1;_tmp588=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp560)->f2;_LLA:
# 3447
({struct Cyc_List_List*_tmpA82=({struct Cyc_List_List*_tmp567=_region_malloc(rgn,sizeof(*_tmp567));_tmp567->hd=_tmp589,_tmp567->tl=*decls;_tmp567;});*decls=_tmpA82;});
({void*_tmpA83=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp589->type=_tmpA83;});{
# 3450
struct Cyc_Absyn_Stmt*_tmp568=({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->tag=4U,_tmp569->f1=_tmp589;_tmp569;}),0U);Cyc_Absyn_assign_stmt(_tmpA86,({
void*_tmpA85=({void*_tmpA84=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpA84,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA85,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA87=_tmp568;Cyc_Toc_seq_stmt_opt(_tmpA87,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp588));});};case 9U: _LLB: _LLC:
# 3455
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp560)->f1)->r)->tag == 8U){_LL17: _tmp58C=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp560)->f1)->r)->f1;_tmp58B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp560)->f1)->r)->f2;_tmp58A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp560)->f1)->r)->f3;_LL18:
# 3464
 if(_tmp58A == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp58B->name,_tmp58C->name);
void*_tmp56A=({void*_tmpA88=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpA88,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA89=Cyc_Toc_cast_it(_tmp56A,path);path=_tmpA89;});{
int cnt=1;
struct Cyc_List_List*_tmp56B=_tmp58B->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp58A != 0;(_tmp58A=_tmp58A->tl,_tmp56B=((struct Cyc_List_List*)_check_null(_tmp56B))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp56C=(struct Cyc_Absyn_Pat*)_tmp58A->hd;
if(_tmp56C->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp56D=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp56B))->hd)).f2;
void*_tmp56E=(void*)_check_null(_tmp56C->topt);
void*_tmp56F=Cyc_Toc_typ_to_c_array(_tmp56E);
struct Cyc_Absyn_Exp*_tmp570=({struct Cyc_Absyn_Exp*_tmpA8A=path;Cyc_Absyn_aggrarrow_exp(_tmpA8A,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp56D)))
({struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Toc_cast_it(_tmp56F,_tmp570);_tmp570=_tmpA8B;});
({struct Cyc_Absyn_Stmt*_tmpA8D=({struct Cyc_Absyn_Stmt*_tmpA8C=s;Cyc_Toc_seq_stmt_opt(_tmpA8C,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp570,_tmp56C));});s=_tmpA8D;});};}
# 3482
return s;};};}else{_LL21: _tmp58D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp560)->f1;_LL22:
# 3527
 return({struct _RegionHandle*_tmpA91=rgn;struct Cyc_Toc_Env**_tmpA90=nv;struct Cyc_List_List**_tmpA8F=decls;struct Cyc_Absyn_Exp*_tmpA8E=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA91,_tmpA90,_tmpA8F,_tmpA8E,_tmp58D);});}case 8U: _LL19: _tmp58E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp560)->f3;_LL1A:
# 3484
 _tmp58F=_tmp58E;goto _LL1C;case 5U: _LL1B: _tmp58F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp560)->f1;_LL1C: {
# 3486
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp58F != 0;(_tmp58F=_tmp58F->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp571=(struct Cyc_Absyn_Pat*)_tmp58F->hd;
if(_tmp571->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp572=(void*)_check_null(_tmp571->topt);
struct _dyneither_ptr*_tmp573=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpA97=({struct Cyc_Absyn_Stmt*_tmpA96=s;Cyc_Toc_seq_stmt_opt(_tmpA96,({struct _RegionHandle*_tmpA95=rgn;struct Cyc_Toc_Env**_tmpA94=nv;struct Cyc_List_List**_tmpA93=decls;struct Cyc_Absyn_Exp*_tmpA92=Cyc_Toc_member_exp(path,_tmp573,0U);Cyc_Toc_extract_pattern_vars(_tmpA95,_tmpA94,_tmpA93,_tmpA92,_tmp571);}));});s=_tmpA97;});};}
# 3496
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp560)->f1 == 0){_LL1D: _LL1E:
({void*_tmp574=0U;({struct _dyneither_ptr _tmpA98=({const char*_tmp575="unresolved aggregate pattern!";_tag_dyneither(_tmp575,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA98,_tag_dyneither(_tmp574,sizeof(void*),0U));});});}else{_LL1F: _tmp591=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp560)->f1)->aggr_info;_tmp590=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp560)->f3;_LL20: {
# 3500
struct Cyc_Absyn_Aggrdecl*_tmp576=Cyc_Absyn_get_known_aggrdecl(_tmp591);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp590 != 0;_tmp590=_tmp590->tl){
struct _tuple37*_tmp577=(struct _tuple37*)_tmp590->hd;
struct Cyc_Absyn_Pat*_tmp578=(*_tmp577).f2;
if(_tmp578->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp579=Cyc_Absyn_designatorlist_to_fieldname((*_tmp577).f1);
void*_tmp57A=(void*)_check_null(_tmp578->topt);
void*_tmp57B=Cyc_Toc_typ_to_c_array(_tmp57A);
struct Cyc_Absyn_Exp*_tmp57C=Cyc_Toc_member_exp(path,_tmp579,0U);
# 3512
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp576->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpA99=Cyc_Toc_member_exp(_tmp57C,Cyc_Toc_val_sp,0U);_tmp57C=_tmpA99;});{
void*_tmp57D=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp576->impl))->fields,_tmp579)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp57D))
({struct Cyc_Absyn_Exp*_tmpA9A=Cyc_Toc_cast_it(_tmp57B,_tmp57C);_tmp57C=_tmpA9A;});else{
if(!Cyc_Toc_is_array_type(_tmp57D) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp57D)))
# 3519
({struct Cyc_Absyn_Exp*_tmpA9C=Cyc_Absyn_deref_exp(({void*_tmpA9B=Cyc_Absyn_cstar_typ(_tmp57B,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA9B,
Cyc_Absyn_address_exp(_tmp57C,0U));}),0U);
# 3519
_tmp57C=_tmpA9C;});}
# 3522
({struct Cyc_Absyn_Stmt*_tmpA9E=({struct Cyc_Absyn_Stmt*_tmpA9D=s;Cyc_Toc_seq_stmt_opt(_tmpA9D,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp57C,_tmp578));});s=_tmpA9E;});};};}
# 3524
return s;}}case 15U: _LL23: _LL24:
# 3529
({void*_tmp57E=0U;({struct _dyneither_ptr _tmpA9F=({const char*_tmp57F="unknownid pat";_tag_dyneither(_tmp57F,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA9F,_tag_dyneither(_tmp57E,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp580=0U;({struct _dyneither_ptr _tmpAA0=({const char*_tmp581="unknowncall pat";_tag_dyneither(_tmp581,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA0,_tag_dyneither(_tmp580,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp582=0U;({struct _dyneither_ptr _tmpAA1=({const char*_tmp583="exp pat";_tag_dyneither(_tmp583,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA1,_tag_dyneither(_tmp582,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3540
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3548
void*_tmp592=dopt;struct Cyc_List_List*_tmp5D0;struct Cyc_List_List*_tmp5CF;void*_tmp5CE;struct Cyc_Tcpat_Rhs*_tmp5CD;if(_tmp592 == 0){_LL1: _LL2:
# 3550
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp592)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5CD=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp592)->f1;_LL6:
# 3555
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp593=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp594=_tmp593;int*_tmp59B;struct _dyneither_ptr*_tmp59A;struct _dyneither_ptr*_tmp599;struct Cyc_Absyn_Switch_clause*_tmp598;_LLA: _tmp59B=(int*)& _tmp594->f1;_tmp59A=_tmp594->f2;_tmp599=_tmp594->f3;_tmp598=_tmp594->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp595=_tmp598->body;
if(_tmp595 == _tmp5CD->rhs){
# 3560
if(*_tmp59B)
return Cyc_Absyn_goto_stmt(_tmp59A,0U);
*_tmp59B=1;{
# 3564
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAA4=rgn;struct Cyc_List_List**_tmpAA3=decls;struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAA4,& nv,_tmpAA3,_tmpAA2,_tmp598->pattern);});
# 3567
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp599,_tmp598->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAA5=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpAA5;});
({struct Cyc_Absyn_Stmt*_tmpAA6=Cyc_Absyn_label_stmt(_tmp59A,res,0U);res=_tmpAA6;});
({struct Cyc_List_List*_tmpAA8=({struct Cyc_List_List*_tmp597=_region_malloc(rgn,sizeof(*_tmp597));({struct _tuple38*_tmpAA7=({struct _tuple38*_tmp596=_region_malloc(rgn,sizeof(*_tmp596));_tmp596->f1=nv,_tmp596->f2=_tmp599,_tmp596->f3=_tmp595;_tmp596;});_tmp597->hd=_tmpAA7;}),_tmp597->tl=*bodies;_tmp597;});*bodies=_tmpAA8;});
return res;};}};}
# 3575
({void*_tmp59C=0U;({struct _dyneither_ptr _tmpAA9=({const char*_tmp59D="couldn't find rhs!";_tag_dyneither(_tmp59D,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA9,_tag_dyneither(_tmp59C,sizeof(void*),0U));});});default: _LL7: _tmp5D0=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp592)->f1;_tmp5CF=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp592)->f2;_tmp5CE=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp592)->f3;_LL8: {
# 3578
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5CE,lscs,v);
# 3580
struct Cyc_Absyn_Exp*_tmp59E=({struct Cyc_List_List*_tmpAAA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5D0);Cyc_Toc_compile_path(_tmpAAA,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp59F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5CF);
# 3583
struct _tuple36 _tmp5A0=Cyc_Toc_admits_switch(_tmp59F);struct _tuple36 _tmp5A1=_tmp5A0;int _tmp5CC;void*_tmp5CB;_LLD: _tmp5CC=_tmp5A1.f1;_tmp5CB=_tmp5A1.f2;_LLE:;
if(_tmp5CC > 1){
# 3587
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));
({struct Cyc_Absyn_Switch_clause*_tmpAAC=({struct Cyc_Absyn_Switch_clause*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));({struct Cyc_Absyn_Pat*_tmpAAB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5B1->pattern=_tmpAAB;}),_tmp5B1->pat_vars=0,_tmp5B1->where_clause=0,_tmp5B1->body=res,_tmp5B1->loc=0U;_tmp5B1;});_tmp5B2->hd=_tmpAAC;}),_tmp5B2->tl=0;_tmp5B2;});
# 3590
for(0;_tmp59F != 0;_tmp59F=_tmp59F->tl){
struct _tuple33 _tmp5A2=*((struct _tuple33*)_tmp59F->hd);struct _tuple33 _tmp5A3=_tmp5A2;void*_tmp5A9;void*_tmp5A8;_LL10: _tmp5A9=_tmp5A3.f1;_tmp5A8=_tmp5A3.f2;_LL11:;{
# 3593
struct Cyc_Absyn_Pat*_tmp5A4=Cyc_Toc_compile_pat_test_as_case(_tmp5A9);
# 3595
struct Cyc_Absyn_Stmt*_tmp5A5=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5A8,lscs,v);
# 3597
({struct Cyc_List_List*_tmpAAE=({struct Cyc_List_List*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));({struct Cyc_Absyn_Switch_clause*_tmpAAD=({struct Cyc_Absyn_Switch_clause*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6->pattern=_tmp5A4,_tmp5A6->pat_vars=0,_tmp5A6->where_clause=0,_tmp5A6->body=_tmp5A5,_tmp5A6->loc=0U;_tmp5A6;});_tmp5A7->hd=_tmpAAD;}),_tmp5A7->tl=new_lscs;_tmp5A7;});new_lscs=_tmpAAE;});};}
# 3599
{void*_tmp5AA=_tmp5CB;struct _dyneither_ptr*_tmp5AF;switch(*((int*)_tmp5AA)){case 1U: _LL13: _LL14:
# 3602
 LOOP1: {
void*_tmp5AB=_tmp59E->r;void*_tmp5AC=_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;switch(*((int*)_tmp5AC)){case 14U: _LL1C: _tmp5AD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5AC)->f2;_LL1D:
 _tmp59E=_tmp5AD;goto LOOP1;case 20U: _LL1E: _tmp5AE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5AC)->f1;_LL1F:
 _tmp59E=_tmp5AE;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3608
({struct Cyc_Absyn_Exp*_tmpAB0=Cyc_Absyn_deref_exp(({void*_tmpAAF=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAAF,_tmp59E);}),0U);_tmp59E=_tmpAB0;});goto _LL12;case 3U: _LL15: _tmp5AF=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5AA)->f1;_LL16:
# 3611
({struct Cyc_Absyn_Exp*_tmpAB2=({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_Toc_member_exp(_tmp59E,_tmp5AF,0U);Cyc_Toc_member_exp(_tmpAB1,Cyc_Toc_tag_sp,0U);});_tmp59E=_tmpAB2;});goto _LL12;case 2U: _LL17: _LL18:
# 3613
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3616
({struct Cyc_Absyn_Stmt*_tmpAB3=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->tag=10U,_tmp5B0->f1=_tmp59E,_tmp5B0->f2=new_lscs,_tmp5B0->f3=0;_tmp5B0;}),0U);res=_tmpAB3;});}else{
# 3620
void*_tmp5B3=_tmp5CB;struct Cyc_Absyn_Exp*_tmp5CA;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5B3)->tag == 2U){_LL23: _tmp5CA=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5B3)->f1;_LL24: {
# 3622
struct Cyc_List_List*_tmp5B4=_tmp59F;void*_tmp5C3;struct Cyc_Tcpat_Rhs*_tmp5C2;if(_tmp5B4 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5B4)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5B4)->tl == 0){_LL28: _tmp5C3=((struct _tuple33*)_tmp5B4->hd)->f1;_tmp5C2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5B4->hd)->f2)->f1;_LL29:
# 3628
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5B5=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5B6=_tmp5B5;int*_tmp5BD;struct _dyneither_ptr*_tmp5BC;struct _dyneither_ptr*_tmp5BB;struct Cyc_Absyn_Switch_clause*_tmp5BA;_LL2D: _tmp5BD=(int*)& _tmp5B6->f1;_tmp5BC=_tmp5B6->f2;_tmp5BB=_tmp5B6->f3;_tmp5BA=_tmp5B6->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5B7=_tmp5BA->body;
if(_tmp5B7 == _tmp5C2->rhs){
# 3633
if(*_tmp5BD)
return Cyc_Absyn_goto_stmt(_tmp5BC,0U);
*_tmp5BD=1;{
# 3637
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAB6=rgn;struct Cyc_List_List**_tmpAB5=decls;struct Cyc_Absyn_Exp*_tmpAB4=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAB6,& nv,_tmpAB5,_tmpAB4,_tmp5BA->pattern);});
# 3640
Cyc_Toc_exp_to_c(nv,_tmp5CA);{
# 3643
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5BB,_tmp5BA->body,0U);
({struct Cyc_Absyn_Stmt*_tmpAB7=Cyc_Absyn_ifthenelse_stmt(_tmp5CA,r,res,0U);r=_tmpAB7;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAB8=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpAB8;});
({struct Cyc_Absyn_Stmt*_tmpAB9=Cyc_Absyn_label_stmt(_tmp5BC,r,0U);r=_tmpAB9;});
({struct Cyc_List_List*_tmpABB=({struct Cyc_List_List*_tmp5B9=_region_malloc(rgn,sizeof(*_tmp5B9));({struct _tuple38*_tmpABA=({struct _tuple38*_tmp5B8=_region_malloc(rgn,sizeof(*_tmp5B8));_tmp5B8->f1=nv,_tmp5B8->f2=_tmp5BB,_tmp5B8->f3=_tmp5B7;_tmp5B8;});_tmp5B9->hd=_tmpABA;}),_tmp5B9->tl=*bodies;_tmp5B9;});*bodies=_tmpABB;});
return r;};};}};}
# 3652
({void*_tmp5BE=0U;({struct _dyneither_ptr _tmpABC=({const char*_tmp5BF="couldn't find rhs!";_tag_dyneither(_tmp5BF,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpABC,_tag_dyneither(_tmp5BE,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5C0=0U;({struct _dyneither_ptr _tmpABD=({const char*_tmp5C1="bad where clause in match compiler";_tag_dyneither(_tmp5C1,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpABD,_tag_dyneither(_tmp5C0,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3657
 for(0;_tmp59F != 0;_tmp59F=_tmp59F->tl){
struct _tuple33 _tmp5C4=*((struct _tuple33*)_tmp59F->hd);struct _tuple33 _tmp5C5=_tmp5C4;void*_tmp5C9;void*_tmp5C8;_LL30: _tmp5C9=_tmp5C5.f1;_tmp5C8=_tmp5C5.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5C6=Cyc_Toc_compile_pat_test(_tmp59E,_tmp5C9);
struct Cyc_Absyn_Stmt*_tmp5C7=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5C8,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpABE=Cyc_Absyn_ifthenelse_stmt(_tmp5C6,_tmp5C7,res,0U);res=_tmpABE;});};}}_LL22:;}
# 3665
return res;}}}_LL0:;}
# 3675
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3677
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp5D1=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp5D2=_tmp5D1;struct Cyc_Toc_Env**_tmp5D4;struct Cyc_Absyn_Stmt*_tmp5D3;_LL1: _tmp5D4=(struct Cyc_Toc_Env**)& _tmp5D2->f1;_tmp5D3=_tmp5D2->f3;_LL2:;
if(_tmp5D3 == s)return _tmp5D4;}
# 3683
return 0;}
# 3687
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3690
void*_tmp5D5=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3693
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5D6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D6;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5D7=Cyc_Toc_share_env(rgn,nv);
# 3700
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3705
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5D7,& mydecls,& mybodies,dopt,lscs,v);
# 3713
{struct Cyc_List_List*_tmp5D8=lscs;for(0;_tmp5D8 != 0;_tmp5D8=_tmp5D8->tl){
struct _tuple35*_tmp5D9=(struct _tuple35*)_tmp5D8->hd;struct _tuple35*_tmp5DA=_tmp5D9;struct _dyneither_ptr*_tmp5E2;struct Cyc_Absyn_Switch_clause*_tmp5E1;_LL1: _tmp5E2=_tmp5DA->f3;_tmp5E1=_tmp5DA->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5E1->body;
# 3717
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3720
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5DB=*envp;
# 3723
if(_tmp5D8->tl != 0){
struct _tuple35*_tmp5DC=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp5D8->tl))->hd;struct _tuple35*_tmp5DD=_tmp5DC;struct _dyneither_ptr*_tmp5E0;struct Cyc_Absyn_Switch_clause*_tmp5DF;_LL4: _tmp5E0=_tmp5DD->f3;_tmp5DF=_tmp5DD->f4;_LL5:;{
# 3727
struct Cyc_Toc_Env**_tmp5DE=Cyc_Toc_find_case_env(mybodies,_tmp5DF->body);
# 3732
if(_tmp5DE == 0)
({struct Cyc_Toc_Env*_tmpABF=Cyc_Toc_last_switchclause_env(rgn,_tmp5DB,end_l);Cyc_Toc_stmt_to_c(_tmpABF,s);});else{
# 3736
struct Cyc_List_List*vs=0;
if(_tmp5DF->pat_vars != 0){
({struct Cyc_List_List*_tmpAC0=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5DF->pat_vars))->v)).f1);vs=_tmpAC0;});
({struct Cyc_List_List*_tmpAC1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpAC1;});}
# 3741
({struct Cyc_Toc_Env*_tmpAC2=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5DB,end_l,_tmp5E0,vs);Cyc_Toc_stmt_to_c(_tmpAC2,s);});}};}else{
# 3745
({struct Cyc_Toc_Env*_tmpAC3=Cyc_Toc_last_switchclause_env(rgn,_tmp5DB,end_l);Cyc_Toc_stmt_to_c(_tmpAC3,s);});}};};}}{
# 3749
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpAC5=test_tree;Cyc_Absyn_seq_stmt(_tmpAC5,({struct _dyneither_ptr*_tmpAC4=end_l;Cyc_Absyn_label_stmt(_tmpAC4,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3751
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5E3=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpAC6=Cyc_Toc_temp_var();_tmp5E3->name=_tmpAC6;});
({void*_tmpAC7=Cyc_Toc_typ_to_c_array(_tmp5E3->type);_tmp5E3->type=_tmpAC7;});
({struct Cyc_Absyn_Stmt*_tmpAC9=({struct Cyc_Absyn_Decl*_tmpAC8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->tag=0U,_tmp5E4->f1=_tmp5E3;_tmp5E4;}),0U);Cyc_Absyn_decl_stmt(_tmpAC8,res,0U);});res=_tmpAC9;});}
# 3758
({void*_tmpACD=({struct _tuple1*_tmpACC=v;void*_tmpACB=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpACA=e;Cyc_Absyn_declare_stmt(_tmpACC,_tmpACB,_tmpACA,res,0U);})->r;whole_s->r=_tmpACD;});
_npop_handler(0U);return;};}
# 3697
;_pop_region(rgn);};}
# 3764
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3766
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3771
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3773
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpACF=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpACF,({struct Cyc_List_List*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));
({struct Cyc_Absyn_Exp*_tmpACE=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp5E5->hd=_tmpACE;}),_tmp5E5->tl=0;_tmp5E5;}),0U);}),0U);}
# 3777
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpAD1=({struct Cyc_Absyn_Stmt*_tmpAD0=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpAD0,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpAD1;});}
# 3782
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3784
while(1){
void*_tmp5E6=s->r;void*_tmp5E7=_tmp5E6;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_List_List*_tmp63D;void*_tmp63C;struct Cyc_Absyn_Stmt*_tmp63B;struct Cyc_Absyn_Exp*_tmp63A;struct _dyneither_ptr*_tmp639;struct Cyc_Absyn_Stmt*_tmp638;struct Cyc_Absyn_Decl*_tmp637;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_List_List*_tmp635;struct Cyc_Absyn_Switch_clause**_tmp634;struct Cyc_Absyn_Exp*_tmp633;struct Cyc_List_List*_tmp632;void*_tmp631;struct Cyc_Absyn_Exp*_tmp630;struct Cyc_Absyn_Exp*_tmp62F;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Stmt*_tmp62D;struct Cyc_Absyn_Exp*_tmp62C;struct Cyc_Absyn_Stmt*_tmp62B;struct Cyc_Absyn_Exp*_tmp62A;struct Cyc_Absyn_Stmt*_tmp629;struct Cyc_Absyn_Stmt*_tmp628;struct Cyc_Absyn_Exp*_tmp627;struct Cyc_Absyn_Stmt*_tmp626;struct Cyc_Absyn_Stmt*_tmp625;struct Cyc_Absyn_Exp*_tmp624;switch(*((int*)_tmp5E7)){case 0U: _LL1: _LL2:
# 3787
 return;case 1U: _LL3: _tmp624=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_LL4:
# 3789
 Cyc_Toc_exp_to_c(nv,_tmp624);
return;case 2U: _LL5: _tmp626=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp625=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL6:
# 3792
 Cyc_Toc_stmt_to_c(nv,_tmp626);
s=_tmp625;
continue;case 3U: _LL7: _tmp627=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_LL8: {
# 3796
void*topt=0;
if(_tmp627 != 0){
({void*_tmpAD2=Cyc_Toc_typ_to_c((void*)_check_null(_tmp627->topt));topt=_tmpAD2;});
Cyc_Toc_exp_to_c(nv,_tmp627);}{
# 3802
int _tmp5E8=Cyc_Toc_get_npop(s);
if(_tmp5E8 > 0){
if(topt != 0){
struct _tuple1*_tmp5E9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5EA=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5E9,0U),0U);
({void*_tmpAD7=({struct _tuple1*_tmpAD6=_tmp5E9;void*_tmpAD5=topt;struct Cyc_Absyn_Exp*_tmpAD4=_tmp627;Cyc_Absyn_declare_stmt(_tmpAD6,_tmpAD5,_tmpAD4,({
struct Cyc_Absyn_Stmt*_tmpAD3=Cyc_Toc_make_npop_handler(_tmp5E8);Cyc_Absyn_seq_stmt(_tmpAD3,_tmp5EA,0U);}),0U);})->r;
# 3807
s->r=_tmpAD7;});}else{
# 3811
Cyc_Toc_do_npop_before(_tmp5E8,s);}}
# 3813
return;};}case 4U: _LL9: _tmp62A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp629=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_tmp628=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3;_LLA:
# 3815
 Cyc_Toc_exp_to_c(nv,_tmp62A);
Cyc_Toc_stmt_to_c(nv,_tmp629);
s=_tmp628;
continue;case 5U: _LLB: _tmp62C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1).f1;_tmp62B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LLC:
# 3820
 Cyc_Toc_exp_to_c(nv,_tmp62C);{
struct _RegionHandle _tmp5EB=_new_region("temp");struct _RegionHandle*temp=& _tmp5EB;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAD8=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAD8,_tmp62B);});
# 3824
_npop_handler(0U);return;
# 3821
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3826
struct Cyc_Toc_Env*_tmp5EC=nv;struct _dyneither_ptr**_tmp5ED;_LL22: _tmp5ED=_tmp5EC->break_lab;_LL23:;
if(_tmp5ED != 0)
({void*_tmpAD9=Cyc_Toc_goto_stmt_r(*_tmp5ED);s->r=_tmpAD9;});
# 3830
({int _tmpADA=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpADA,s);});
return;}case 7U: _LLF: _LL10: {
# 3833
struct Cyc_Toc_Env*_tmp5EE=nv;struct _dyneither_ptr**_tmp5EF;_LL25: _tmp5EF=_tmp5EE->continue_lab;_LL26:;
if(_tmp5EF != 0)
({void*_tmpADB=Cyc_Toc_goto_stmt_r(*_tmp5EF);s->r=_tmpADB;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3839
({int _tmpADC=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpADC,s);});
return;case 9U: _LL13: _tmp630=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp62F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2).f1;_tmp62E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3).f1;_tmp62D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f4;_LL14:
# 3843
 Cyc_Toc_exp_to_c(nv,_tmp630);Cyc_Toc_exp_to_c(nv,_tmp62F);Cyc_Toc_exp_to_c(nv,_tmp62E);{
struct _RegionHandle _tmp5F0=_new_region("temp");struct _RegionHandle*temp=& _tmp5F0;_push_region(temp);
({struct Cyc_Toc_Env*_tmpADD=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpADD,_tmp62D);});
# 3847
_npop_handler(0U);return;
# 3844
;_pop_region(temp);};case 10U: _LL15: _tmp633=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp632=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_tmp631=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3;_LL16:
# 3849
 Cyc_Toc_xlate_switch(nv,s,_tmp633,_tmp632,_tmp631);
return;case 11U: _LL17: _tmp635=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp634=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL18: {
# 3852
struct Cyc_Toc_Env*_tmp5F1=nv;struct Cyc_Toc_FallthruInfo*_tmp5FB;_LL28: _tmp5FB=_tmp5F1->fallthru_info;_LL29:;
if(_tmp5FB == 0)
({void*_tmp5F2=0U;({struct _dyneither_ptr _tmpADE=({const char*_tmp5F3="fallthru in unexpected place";_tag_dyneither(_tmp5F3,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpADE,_tag_dyneither(_tmp5F2,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp5F4=*_tmp5FB;struct Cyc_Toc_FallthruInfo _tmp5F5=_tmp5F4;struct _dyneither_ptr*_tmp5FA;struct Cyc_List_List*_tmp5F9;_LL2B: _tmp5FA=_tmp5F5.label;_tmp5F9=_tmp5F5.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp5FA,0U);
# 3858
({int _tmpADF=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpADF,s2);});{
struct Cyc_List_List*_tmp5F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5F9);
struct Cyc_List_List*_tmp5F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp635);
for(0;_tmp5F6 != 0;(_tmp5F6=_tmp5F6->tl,_tmp5F7=_tmp5F7->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5F7))->hd);
({struct Cyc_Absyn_Stmt*_tmpAE2=({struct Cyc_Absyn_Stmt*_tmpAE1=({struct Cyc_Absyn_Exp*_tmpAE0=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->tag=5U,_tmp5F8->f1=(struct Cyc_Absyn_Vardecl*)_tmp5F6->hd;_tmp5F8;}),0U);Cyc_Absyn_assign_stmt(_tmpAE0,(struct Cyc_Absyn_Exp*)_tmp5F7->hd,0U);});Cyc_Absyn_seq_stmt(_tmpAE1,s2,0U);});s2=_tmpAE2;});}
# 3866
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp637=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp636=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL1A:
# 3872
{void*_tmp5FC=_tmp637->r;void*_tmp5FD=_tmp5FC;struct Cyc_Absyn_Tvar*_tmp618;struct Cyc_Absyn_Vardecl*_tmp617;struct Cyc_Absyn_Exp*_tmp616;struct Cyc_Absyn_Fndecl*_tmp615;struct Cyc_List_List*_tmp614;struct Cyc_Absyn_Pat*_tmp613;struct Cyc_Absyn_Exp*_tmp612;void*_tmp611;struct Cyc_Absyn_Vardecl*_tmp610;switch(*((int*)_tmp5FD)){case 0U: _LL2E: _tmp610=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5FD)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp610,_tmp636);goto _LL2D;case 2U: _LL30: _tmp613=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5FD)->f1;_tmp612=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5FD)->f3;_tmp611=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5FD)->f4;_LL31:
# 3880
{void*_tmp5FE=_tmp613->r;void*_tmp5FF=_tmp5FE;struct Cyc_Absyn_Vardecl*_tmp601;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5FF)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5FF)->f2)->r)->tag == 0U){_LL3B: _tmp601=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp5FF)->f1;_LL3C:
# 3882
({struct _tuple1*_tmpAE3=Cyc_Toc_temp_var();_tmp601->name=_tmpAE3;});
_tmp601->initializer=_tmp612;
({void*_tmpAE4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=0U,_tmp600->f1=_tmp601;_tmp600;});_tmp637->r=_tmpAE4;});
Cyc_Toc_local_decl_to_c(nv,_tmp601,_tmp636);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3891
({void*_tmpAE5=(Cyc_Toc_letdecl_to_c(nv,_tmp613,_tmp611,(void*)_check_null(_tmp612->topt),_tmp612,_tmp636))->r;s->r=_tmpAE5;});
goto _LL3A;}_LL3A:;}
# 3894
goto _LL2D;case 3U: _LL32: _tmp614=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5FD)->f1;_LL33: {
# 3898
struct Cyc_List_List*_tmp602=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp614);
if(_tmp602 == 0)
({void*_tmp603=0U;({struct _dyneither_ptr _tmpAE6=({const char*_tmp604="empty Letv_d";_tag_dyneither(_tmp604,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAE6,_tag_dyneither(_tmp603,sizeof(void*),0U));});});
({void*_tmpAE7=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->tag=0U,_tmp605->f1=(struct Cyc_Absyn_Vardecl*)_tmp602->hd;_tmp605;});_tmp637->r=_tmpAE7;});
_tmp602=_tmp602->tl;
for(0;_tmp602 != 0;_tmp602=_tmp602->tl){
struct Cyc_Absyn_Decl*_tmp606=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->tag=0U,_tmp607->f1=(struct Cyc_Absyn_Vardecl*)_tmp602->hd;_tmp607;}),0U);
({void*_tmpAE9=({struct Cyc_Absyn_Decl*_tmpAE8=_tmp606;Cyc_Absyn_decl_stmt(_tmpAE8,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpAE9;});}
# 3907
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp615=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp5FD)->f1;_LL35:
# 3910
 Cyc_Toc_fndecl_to_c(nv,_tmp615,0);
Cyc_Toc_stmt_to_c(nv,_tmp636);
goto _LL2D;case 4U: _LL36: _tmp618=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5FD)->f1;_tmp617=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5FD)->f2;_tmp616=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp5FD)->f3;_LL37: {
# 3914
struct Cyc_Absyn_Stmt*_tmp608=_tmp636;
# 3916
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp617->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3923
Cyc_Toc_stmt_to_c(nv,_tmp608);
if(Cyc_Absyn_no_regions)
({void*_tmpAED=({struct _tuple1*_tmpAEC=x_var;void*_tmpAEB=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAEA=
Cyc_Absyn_uint_exp(0U,0U);
# 3925
Cyc_Absyn_declare_stmt(_tmpAEC,_tmpAEB,_tmpAEA,_tmp608,0U);})->r;s->r=_tmpAED;});else{
# 3927
if((unsigned int)_tmp616){
# 3929
Cyc_Toc_exp_to_c(nv,_tmp616);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpAEF=_tmp616;Cyc_Absyn_aggrarrow_exp(_tmpAEF,({struct _dyneither_ptr*_tmp60A=_cycalloc(sizeof(*_tmp60A));({struct _dyneither_ptr _tmpAEE=({const char*_tmp609="h";_tag_dyneither(_tmp609,sizeof(char),2U);});*_tmp60A=_tmpAEE;});_tmp60A;}),0U);}),0U);
({void*_tmpAF0=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp608,0U))->r;s->r=_tmpAF0;});};}else{
# 3939
({void*_tmpAFD=({
struct _tuple1*_tmpAFC=rh_var;void*_tmpAFB=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpAFA=({
struct Cyc_Absyn_Exp*_tmpAF2=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpAF2,({struct Cyc_List_List*_tmp60B=_cycalloc(sizeof(*_tmp60B));
({struct Cyc_Absyn_Exp*_tmpAF1=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp60B->hd=_tmpAF1;}),_tmp60B->tl=0;_tmp60B;}),0U);});
# 3940
Cyc_Absyn_declare_stmt(_tmpAFC,_tmpAFB,_tmpAFA,({
# 3944
struct _tuple1*_tmpAF9=x_var;void*_tmpAF8=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAF7=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpAF9,_tmpAF8,_tmpAF7,({
struct Cyc_Absyn_Stmt*_tmpAF6=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAF3=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpAF3,({struct Cyc_Absyn_Exp*_tmp60C[1U];_tmp60C[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60C,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpAF6,({
struct Cyc_Absyn_Stmt*_tmpAF5=_tmp608;Cyc_Absyn_seq_stmt(_tmpAF5,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAF4=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpAF4,({struct Cyc_Absyn_Exp*_tmp60D[1U];_tmp60D[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60D,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3939
s->r=_tmpAFD;});}}
# 3950
return;}default: _LL38: _LL39:
({void*_tmp60E=0U;({struct _dyneither_ptr _tmpAFE=({const char*_tmp60F="bad nested declaration within function";_tag_dyneither(_tmp60F,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAFE,_tag_dyneither(_tmp60E,sizeof(void*),0U));});});}_LL2D:;}
# 3953
return;case 13U: _LL1B: _tmp639=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp638=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_LL1C:
# 3955
 s=_tmp638;continue;case 14U: _LL1D: _tmp63B=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp63A=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2).f1;_LL1E: {
# 3957
struct _RegionHandle _tmp619=_new_region("temp");struct _RegionHandle*temp=& _tmp619;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAFF=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAFF,_tmp63B);});
Cyc_Toc_exp_to_c(nv,_tmp63A);
# 3961
_npop_handler(0U);return;
# 3957
;_pop_region(temp);}default: _LL1F: _tmp63E=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f1;_tmp63D=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f2;_tmp63C=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5E7)->f3;_LL20: {
# 3975 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 3985
e_exp->topt=e_typ;{
struct _RegionHandle _tmp61A=_new_region("temp");struct _RegionHandle*temp=& _tmp61A;_push_region(temp);
# 3988
Cyc_Toc_stmt_to_c(nv,_tmp63E);{
struct Cyc_Absyn_Stmt*_tmp61B=({struct Cyc_Absyn_Stmt*_tmpB00=_tmp63E;Cyc_Absyn_seq_stmt(_tmpB00,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 3993
struct Cyc_Absyn_Stmt*_tmp61C=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623->tag=10U,_tmp623->f1=e_exp,_tmp623->f2=_tmp63D,_tmp623->f3=_tmp63C;_tmp623;}),0U);
# 3995
Cyc_Toc_stmt_to_c(nv,_tmp61C);{
# 3998
struct Cyc_Absyn_Exp*_tmp61D=({
struct Cyc_Absyn_Exp*_tmpB02=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpB02,({struct Cyc_List_List*_tmp622=_cycalloc(sizeof(*_tmp622));
({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp622->hd=_tmpB01;}),_tmp622->tl=0;_tmp622;}),0U);});
# 4002
struct Cyc_Absyn_Stmt*_tmp61E=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB04=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpB04,({struct Cyc_List_List*_tmp621=_cycalloc(sizeof(*_tmp621));
({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Absyn_address_exp(h_exp,0U);_tmp621->hd=_tmpB03;}),_tmp621->tl=0;_tmp621;}),0U);}),0U);
# 4006
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp620=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB13=({
struct _tuple1*_tmpB12=h_var;void*_tmpB11=h_typ;Cyc_Absyn_declare_stmt(_tmpB12,_tmpB11,0,({
struct Cyc_Absyn_Stmt*_tmpB10=_tmp61E;Cyc_Absyn_seq_stmt(_tmpB10,({
struct _tuple1*_tmpB0F=was_thrown_var;void*_tmpB0E=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB0D=_tmp61F;Cyc_Absyn_declare_stmt(_tmpB0F,_tmpB0E,_tmpB0D,({
struct Cyc_Absyn_Stmt*_tmpB0C=({struct Cyc_Absyn_Exp*_tmpB06=_tmp61D;struct Cyc_Absyn_Stmt*_tmpB05=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp620,0U);
# 4012
Cyc_Absyn_ifthenelse_stmt(_tmpB06,_tmpB05,
# 4014
Cyc_Toc_skip_stmt_dl(),0U);});
# 4012
Cyc_Absyn_seq_stmt(_tmpB0C,({
# 4015
struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB0A=_tmp61B;Cyc_Absyn_ifthenelse_stmt(_tmpB0B,_tmpB0A,({
# 4017
struct _tuple1*_tmpB09=e_var;void*_tmpB08=e_typ;struct Cyc_Absyn_Exp*_tmpB07=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4017
Cyc_Absyn_declare_stmt(_tmpB09,_tmpB08,_tmpB07,_tmp61C,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4008
s->r=_tmpB13;});};};
# 4021
_npop_handler(0U);return;
# 3986
;_pop_region(temp);};}}_LL0:;}}
# 4030
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB14=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB14;});{
struct _RegionHandle _tmp63F=_new_region("frgn");struct _RegionHandle*frgn=& _tmp63F;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp640=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp641=f->args;for(0;_tmp641 != 0;_tmp641=_tmp641->tl){
struct _tuple1*_tmp642=({struct _tuple1*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->f1=Cyc_Absyn_Loc_n,_tmp643->f2=(*((struct _tuple9*)_tmp641->hd)).f1;_tmp643;});
({void*_tmpB15=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp641->hd)).f3);(*((struct _tuple9*)_tmp641->hd)).f3=_tmpB15;});}}
# 4046
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4050
({struct Cyc_Hashtable_Table**_tmpB17=({struct Cyc_Hashtable_Table**_tmp644=_cycalloc(sizeof(*_tmp644));({struct Cyc_Hashtable_Table*_tmpB16=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp644=_tmpB16;});_tmp644;});Cyc_Toc_fn_pop_table=_tmpB17;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp645=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp646=_tmp645;struct _dyneither_ptr*_tmp64F;struct Cyc_Absyn_Tqual _tmp64E;void*_tmp64D;int _tmp64C;_LL1: _tmp64F=_tmp646.name;_tmp64E=_tmp646.tq;_tmp64D=_tmp646.type;_tmp64C=_tmp646.inject;_LL2:;{
void*_tmp647=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp64D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp64E,Cyc_Absyn_false_conref));
struct _tuple1*_tmp648=({struct _tuple1*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->f1=Cyc_Absyn_Loc_n,_tmp64B->f2=(struct _dyneither_ptr*)_check_null(_tmp64F);_tmp64B;});
({struct Cyc_List_List*_tmpB1A=({struct Cyc_List_List*_tmpB19=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB19,({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct _tuple9*_tmpB18=({struct _tuple9*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=_tmp64F,_tmp649->f2=_tmp64E,_tmp649->f3=_tmp647;_tmp649;});_tmp64A->hd=_tmpB18;}),_tmp64A->tl=0;_tmp64A;}));});f->args=_tmpB1A;});
f->cyc_varargs=0;};}
# 4059
{struct Cyc_List_List*_tmp650=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp650 != 0;_tmp650=_tmp650->tl){
({void*_tmpB1B=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp650->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp650->hd)->type=_tmpB1B;});}}
# 4062
({struct Cyc_Toc_Env*_tmpB1C=Cyc_Toc_clear_toplevel(frgn,_tmp640);Cyc_Toc_stmt_to_c(_tmpB1C,f->body);});}
# 4039
;_pop_region(frgn);};}
# 4066
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp651=s;switch(_tmp651){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple40{struct Cyc_Toc_TocState*f1;struct _tuple39*f2;};
# 4083 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4086
struct _tuple40 _tmp652=*env;struct _tuple40 _tmp653=_tmp652;struct Cyc_Toc_TocState*_tmp688;struct Cyc_Absyn_Aggrdecl*_tmp687;int _tmp686;_LL1: _tmp688=_tmp653.f1;_tmp687=(_tmp653.f2)->f1;_tmp686=(_tmp653.f2)->f2;_LL2:;{
struct _tuple1*_tmp654=_tmp687->name;
struct Cyc_Toc_TocState _tmp655=*_tmp688;struct Cyc_Toc_TocState _tmp656=_tmp655;struct Cyc_Dict_Dict*_tmp685;_LL4: _tmp685=_tmp656.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp657=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp685,_tmp654);
if(_tmp657 == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp687->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB1E=({struct _tuple19*_tmp658=_region_malloc(d,sizeof(*_tmp658));_tmp658->f1=_tmp687,({void*_tmpB1D=Cyc_Absyn_strctq(_tmp654);_tmp658->f2=_tmpB1D;});_tmp658;});v=_tmpB1E;});else{
# 4097
({struct _tuple19*_tmpB20=({struct _tuple19*_tmp659=_region_malloc(d,sizeof(*_tmp659));_tmp659->f1=_tmp687,({void*_tmpB1F=Cyc_Absyn_unionq_typ(_tmp654);_tmp659->f2=_tmpB1F;});_tmp659;});v=_tmpB20;});}
({struct Cyc_Dict_Dict _tmpB21=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp685,_tmp654,v);*_tmp685=_tmpB21;});};}else{
# 4100
struct _tuple19*_tmp65A=*_tmp657;struct _tuple19*_tmp65B=_tmp65A;struct Cyc_Absyn_Aggrdecl*_tmp65E;void*_tmp65D;_LL7: _tmp65E=_tmp65B->f1;_tmp65D=_tmp65B->f2;_LL8:;
if(_tmp65E->impl == 0){
({struct Cyc_Dict_Dict _tmpB24=({struct Cyc_Dict_Dict _tmpB23=*_tmp685;struct _tuple1*_tmpB22=_tmp654;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB23,_tmpB22,({struct _tuple19*_tmp65C=_region_malloc(d,sizeof(*_tmp65C));_tmp65C->f1=_tmp687,_tmp65C->f2=_tmp65D;_tmp65C;}));});*_tmp685=_tmpB24;});
seen_defn_before=0;}else{
# 4105
seen_defn_before=1;}}{
# 4107
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->kind=_tmp687->kind,_tmp684->sc=Cyc_Absyn_Public,_tmp684->name=_tmp687->name,_tmp684->tvs=0,_tmp684->impl=0,_tmp684->expected_mem_kind=0,_tmp684->attributes=_tmp687->attributes;_tmp684;});
# 4114
if(_tmp687->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB25=({struct Cyc_Absyn_AggrdeclImpl*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->exist_vars=0,_tmp65F->rgn_po=0,_tmp65F->fields=0,_tmp65F->tagged=0;_tmp65F;});new_ad->impl=_tmpB25;});{
# 4119
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp660=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp687->impl))->fields;for(0;_tmp660 != 0;_tmp660=_tmp660->tl){
# 4123
struct Cyc_Absyn_Aggrfield*_tmp661=(struct Cyc_Absyn_Aggrfield*)_tmp660->hd;
void*_tmp662=_tmp661->type;
struct Cyc_List_List*_tmp663=_tmp661->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp662)) && (
_tmp687->kind == Cyc_Absyn_StructA  && _tmp660->tl == 0  || _tmp687->kind == Cyc_Absyn_UnionA)){
# 4137 "toc.cyc"
void*_tmp664=Cyc_Tcutil_compress(_tmp662);void*_tmp665=_tmp664;void*_tmp66D;struct Cyc_Absyn_Tqual _tmp66C;union Cyc_Absyn_Constraint*_tmp66B;unsigned int _tmp66A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp665)->tag == 8U){_LLA: _tmp66D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp665)->f1).elt_type;_tmp66C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp665)->f1).tq;_tmp66B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp665)->f1).zero_term;_tmp66A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp665)->f1).zt_loc;_LLB:
# 4140
({void*_tmpB27=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->tag=8U,(_tmp666->f1).elt_type=_tmp66D,(_tmp666->f1).tq=_tmp66C,({struct Cyc_Absyn_Exp*_tmpB26=Cyc_Absyn_uint_exp(0U,0U);(_tmp666->f1).num_elts=_tmpB26;}),(_tmp666->f1).zero_term=_tmp66B,(_tmp666->f1).zt_loc=_tmp66A;_tmp666;});_tmp662=_tmpB27;});
goto _LL9;}else{_LLC: _LLD:
# 4143
({struct Cyc_List_List*_tmpB29=({struct Cyc_List_List*_tmp668=_cycalloc(sizeof(*_tmp668));({void*_tmpB28=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tag=6U,_tmp667->f1=0;_tmp667;});_tmp668->hd=_tmpB28;}),_tmp668->tl=_tmp663;_tmp668;});_tmp663=_tmpB29;});
({void*_tmpB2E=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tag=8U,({void*_tmpB2D=Cyc_Absyn_void_star_typ();(_tmp669->f1).elt_type=_tmpB2D;}),({
struct Cyc_Absyn_Tqual _tmpB2C=Cyc_Absyn_empty_tqual(0U);(_tmp669->f1).tq=_tmpB2C;}),({
struct Cyc_Absyn_Exp*_tmpB2B=Cyc_Absyn_uint_exp(0U,0U);(_tmp669->f1).num_elts=_tmpB2B;}),({
union Cyc_Absyn_Constraint*_tmpB2A=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp669->f1).zero_term=_tmpB2A;}),(_tmp669->f1).zt_loc=0U;_tmp669;});
# 4144
_tmp662=_tmpB2E;});}_LL9:;}{
# 4150
struct Cyc_Absyn_Aggrfield*_tmp66E=({struct Cyc_Absyn_Aggrfield*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680->name=_tmp661->name,_tmp680->tq=Cyc_Toc_mt_tq,({
# 4152
void*_tmpB2F=Cyc_Toc_typ_to_c(_tmp662);_tmp680->type=_tmpB2F;}),_tmp680->width=_tmp661->width,_tmp680->attributes=_tmp663,_tmp680->requires_clause=0;_tmp680;});
# 4160
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp687->impl))->tagged){
void*_tmp66F=_tmp66E->type;
struct _dyneither_ptr*_tmp670=_tmp66E->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp67D=({struct Cyc_String_pa_PrintArg_struct _tmp798;_tmp798.tag=0U,_tmp798.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp687->name).f2);_tmp798;});struct Cyc_String_pa_PrintArg_struct _tmp67E=({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0U,_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp670);_tmp797;});void*_tmp67B[2U];_tmp67B[0]=& _tmp67D,_tmp67B[1]=& _tmp67E;({struct _dyneither_ptr _tmpB30=({const char*_tmp67C="_union_%s_%s";_tag_dyneither(_tmp67C,sizeof(char),13U);});Cyc_aprintf(_tmpB30,_tag_dyneither(_tmp67B,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp67A=_cycalloc(sizeof(*_tmp67A));*_tmp67A=s;_tmp67A;});
struct Cyc_Absyn_Aggrfield*_tmp671=({struct Cyc_Absyn_Aggrfield*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->name=Cyc_Toc_val_sp,_tmp679->tq=Cyc_Toc_mt_tq,_tmp679->type=_tmp66F,_tmp679->width=0,_tmp679->attributes=0,_tmp679->requires_clause=0;_tmp679;});
struct Cyc_Absyn_Aggrfield*_tmp672=({struct Cyc_Absyn_Aggrfield*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->name=Cyc_Toc_tag_sp,_tmp678->tq=Cyc_Toc_mt_tq,_tmp678->type=Cyc_Absyn_sint_typ,_tmp678->width=0,_tmp678->attributes=0,_tmp678->requires_clause=0;_tmp678;});
struct Cyc_List_List*_tmp673=({struct Cyc_Absyn_Aggrfield*_tmp677[2U];_tmp677[0]=_tmp672,_tmp677[1]=_tmp671;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp677,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp674=Cyc_Toc_make_c_struct_defn(str,_tmp673);
({struct Cyc_List_List*_tmpB32=({struct Cyc_List_List*_tmp676=_cycalloc(sizeof(*_tmp676));({struct Cyc_Absyn_Decl*_tmpB31=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->tag=5U,_tmp675->f1=_tmp674;_tmp675;}),0U);_tmp676->hd=_tmpB31;}),_tmp676->tl=Cyc_Toc_result_decls;_tmp676;});Cyc_Toc_result_decls=_tmpB32;});
({void*_tmpB33=Cyc_Absyn_strct(str);_tmp66E->type=_tmpB33;});}
# 4172
({struct Cyc_List_List*_tmpB34=({struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->hd=_tmp66E,_tmp67F->tl=new_fields;_tmp67F;});new_fields=_tmpB34;});};}}
# 4174
({struct Cyc_List_List*_tmpB35=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB35;});};}
# 4176
if(_tmp686)
({struct Cyc_List_List*_tmpB38=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));({struct Cyc_Absyn_Decl*_tmpB37=({struct Cyc_Absyn_Decl*_tmp682=_cycalloc(sizeof(*_tmp682));({void*_tmpB36=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681->tag=5U,_tmp681->f1=new_ad;_tmp681;});_tmp682->r=_tmpB36;}),_tmp682->loc=0U;_tmp682;});_tmp683->hd=_tmpB37;}),_tmp683->tl=Cyc_Toc_result_decls;_tmp683;});Cyc_Toc_result_decls=_tmpB38;});
return 0;};};};}
# 4181
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple39 p=({struct _tuple39 _tmp799;_tmp799.f1=ad,_tmp799.f2=add_to_decls;_tmp799;});
((int(*)(struct _tuple39*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4210 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4213
struct _tuple41 _tmp689=*env;struct _tuple41 _tmp68A=_tmp689;struct Cyc_Set_Set**_tmp69B;struct Cyc_Absyn_Datatypedecl*_tmp69A;_LL1: _tmp69B=(_tmp68A.f1)->datatypes_so_far;_tmp69A=_tmp68A.f2;_LL2:;{
struct _tuple1*_tmp68B=_tmp69A->name;
if(_tmp69A->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp69B,_tmp68B))
return 0;
({struct Cyc_Set_Set*_tmpB39=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp69B,_tmp68B);*_tmp69B=_tmpB39;});
{struct Cyc_List_List*_tmp68C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp69A->fields))->v;for(0;_tmp68C != 0;_tmp68C=_tmp68C->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp68C->hd;
# 4221
struct Cyc_List_List*_tmp68D=0;
int i=1;
{struct Cyc_List_List*_tmp68E=f->typs;for(0;_tmp68E != 0;(_tmp68E=_tmp68E->tl,i ++)){
struct _dyneither_ptr*_tmp68F=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp690=({struct Cyc_Absyn_Aggrfield*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->name=_tmp68F,_tmp692->tq=(*((struct _tuple12*)_tmp68E->hd)).f1,({
void*_tmpB3A=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp68E->hd)).f2);_tmp692->type=_tmpB3A;}),_tmp692->width=0,_tmp692->attributes=0,_tmp692->requires_clause=0;_tmp692;});
({struct Cyc_List_List*_tmpB3B=({struct Cyc_List_List*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->hd=_tmp690,_tmp691->tl=_tmp68D;_tmp691;});_tmp68D=_tmpB3B;});}}
# 4229
({struct Cyc_List_List*_tmpB3E=({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));({struct Cyc_Absyn_Aggrfield*_tmpB3D=({struct Cyc_Absyn_Aggrfield*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->name=Cyc_Toc_tag_sp,_tmp693->tq=Cyc_Toc_mt_tq,_tmp693->type=Cyc_Absyn_sint_typ,_tmp693->width=0,_tmp693->attributes=0,_tmp693->requires_clause=0;_tmp693;});_tmp694->hd=_tmpB3D;}),({
struct Cyc_List_List*_tmpB3C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp68D);_tmp694->tl=_tmpB3C;});_tmp694;});
# 4229
_tmp68D=_tmpB3E;});{
# 4231
struct Cyc_Absyn_Aggrdecl*_tmp695=({struct _dyneither_ptr*_tmpB40=({struct _dyneither_ptr*_tmp699=_cycalloc(sizeof(*_tmp699));({struct _dyneither_ptr _tmpB3F=({const char*_tmp698="";_tag_dyneither(_tmp698,sizeof(char),1U);});*_tmp699=_tmpB3F;});_tmp699;});Cyc_Toc_make_c_struct_defn(_tmpB40,_tmp68D);});
({struct _tuple1*_tmpB41=Cyc_Toc_collapse_qvars(f->name,_tmp69A->name);_tmp695->name=_tmpB41;});
({struct Cyc_List_List*_tmpB43=({struct Cyc_List_List*_tmp697=_cycalloc(sizeof(*_tmp697));({struct Cyc_Absyn_Decl*_tmpB42=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->tag=5U,_tmp696->f1=_tmp695;_tmp696;}),0U);_tmp697->hd=_tmpB42;}),_tmp697->tl=Cyc_Toc_result_decls;_tmp697;});Cyc_Toc_result_decls=_tmpB43;});};}}
# 4235
return 0;};}
# 4238
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4257 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4260
struct _tuple41 _tmp69C=*env;struct _tuple41 _tmp69D=_tmp69C;struct Cyc_Toc_TocState*_tmp6BF;struct Cyc_Absyn_Datatypedecl*_tmp6BE;_LL1: _tmp6BF=_tmp69D.f1;_tmp6BE=_tmp69D.f2;_LL2:;
if(_tmp6BE->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp69E=*_tmp6BF;struct Cyc_Toc_TocState _tmp69F=_tmp69E;struct Cyc_Dict_Dict*_tmp6BD;_LL4: _tmp6BD=_tmp69F.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp6A0=_tmp6BE->name;
{struct Cyc_List_List*_tmp6A1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6BE->fields))->v;for(0;_tmp6A1 != 0;_tmp6A1=_tmp6A1->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6A1->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6A2=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp6A3=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp6A2,Cyc_Absyn_false_conref,0U);
# 4271
int*_tmp6A4=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6BD,f->name);int*_tmp6A5=_tmp6A4;if(_tmp6A5 == 0){_LL7: _LL8: {
# 4273
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB44=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB44;});{
# 4277
struct Cyc_Absyn_Vardecl*_tmp6A6=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6A3,initopt);
_tmp6A6->sc=f->sc;
({struct Cyc_List_List*_tmpB46=({struct Cyc_List_List*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));({struct Cyc_Absyn_Decl*_tmpB45=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7->tag=0U,_tmp6A7->f1=_tmp6A6;_tmp6A7;}),0U);_tmp6A8->hd=_tmpB45;}),_tmp6A8->tl=Cyc_Toc_result_decls;_tmp6A8;});Cyc_Toc_result_decls=_tmpB46;});
({struct Cyc_Dict_Dict _tmpB47=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6BD,f->name,f->sc != Cyc_Absyn_Extern);
# 4280
*_tmp6BD=_tmpB47;});{
# 4282
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6A9=f->typs;for(0;_tmp6A9 != 0;(_tmp6A9=_tmp6A9->tl,i ++)){
struct _dyneither_ptr*_tmp6AA=({struct _dyneither_ptr*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));({struct _dyneither_ptr _tmpB49=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6B0=({struct Cyc_Int_pa_PrintArg_struct _tmp79A;_tmp79A.tag=1U,_tmp79A.f1=(unsigned long)i;_tmp79A;});void*_tmp6AE[1U];_tmp6AE[0]=& _tmp6B0;({struct _dyneither_ptr _tmpB48=({const char*_tmp6AF="f%d";_tag_dyneither(_tmp6AF,sizeof(char),4U);});Cyc_aprintf(_tmpB48,_tag_dyneither(_tmp6AE,sizeof(void*),1U));});});*_tmp6B1=_tmpB49;});_tmp6B1;});
struct Cyc_Absyn_Aggrfield*_tmp6AB=({struct Cyc_Absyn_Aggrfield*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));_tmp6AD->name=_tmp6AA,_tmp6AD->tq=(*((struct _tuple12*)_tmp6A9->hd)).f1,({
void*_tmpB4A=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6A9->hd)).f2);_tmp6AD->type=_tmpB4A;}),_tmp6AD->width=0,_tmp6AD->attributes=0,_tmp6AD->requires_clause=0;_tmp6AD;});
({struct Cyc_List_List*_tmpB4B=({struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC->hd=_tmp6AB,_tmp6AC->tl=fields;_tmp6AC;});fields=_tmpB4B;});}}
# 4290
({struct Cyc_List_List*_tmpB4F=({struct Cyc_List_List*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));({struct Cyc_Absyn_Aggrfield*_tmpB4E=({struct Cyc_Absyn_Aggrfield*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2->name=Cyc_Toc_tag_sp,_tmp6B2->tq=Cyc_Toc_mt_tq,({
void*_tmpB4D=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp6B2->type=_tmpB4D;}),_tmp6B2->width=0,_tmp6B2->attributes=0,_tmp6B2->requires_clause=0;_tmp6B2;});
# 4290
_tmp6B3->hd=_tmpB4E;}),({
# 4292
struct Cyc_List_List*_tmpB4C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6B3->tl=_tmpB4C;});_tmp6B3;});
# 4290
fields=_tmpB4F;});{
# 4293
struct Cyc_Absyn_Aggrdecl*_tmp6B4=({struct _dyneither_ptr*_tmpB51=({struct _dyneither_ptr*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));({struct _dyneither_ptr _tmpB50=({const char*_tmp6B7="";_tag_dyneither(_tmp6B7,sizeof(char),1U);});*_tmp6B8=_tmpB50;});_tmp6B8;});Cyc_Toc_make_c_struct_defn(_tmpB51,fields);});
({struct _tuple1*_tmpB52=Cyc_Toc_collapse_qvars(f->name,_tmp6BE->name);_tmp6B4->name=_tmpB52;});
({struct Cyc_List_List*_tmpB54=({struct Cyc_List_List*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));({struct Cyc_Absyn_Decl*_tmpB53=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->tag=5U,_tmp6B5->f1=_tmp6B4;_tmp6B5;}),0U);_tmp6B6->hd=_tmpB53;}),_tmp6B6->tl=Cyc_Toc_result_decls;_tmp6B6;});Cyc_Toc_result_decls=_tmpB54;});
goto _LL6;};};};}}else{if(*((int*)_tmp6A5)== 0){_LL9: _LLA:
# 4298
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6B9=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6BA=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6A3,_tmp6B9);
_tmp6BA->sc=f->sc;
({struct Cyc_List_List*_tmpB56=({struct Cyc_List_List*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));({struct Cyc_Absyn_Decl*_tmpB55=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->tag=0U,_tmp6BB->f1=_tmp6BA;_tmp6BB;}),0U);_tmp6BC->hd=_tmpB55;}),_tmp6BC->tl=Cyc_Toc_result_decls;_tmp6BC;});Cyc_Toc_result_decls=_tmpB56;});
({struct Cyc_Dict_Dict _tmpB57=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6BD,f->name,1);*_tmp6BD=_tmpB57;});}
# 4305
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4309
return 0;};};}
# 4312
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4316
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4322
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB58=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB58;});
# 4326
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4335
struct _RegionHandle _tmp6C0=_new_region("temp");struct _RegionHandle*temp=& _tmp6C0;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6C1=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6C1,init);}
# 4336
;_pop_region(temp);}else{
# 4340
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4343
void*_tmp6C2=Cyc_Tcutil_compress(old_typ);void*_tmp6C3=_tmp6C2;void*_tmp6CA;struct Cyc_Absyn_Exp*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6C8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C3)->tag == 8U){_LL1: _tmp6CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C3)->f1).elt_type;_tmp6C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C3)->f1).num_elts;_tmp6C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C3)->f1).zero_term;_LL2:
# 4345
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C8)){
if(_tmp6C9 == 0)
({void*_tmp6C4=0U;({struct _dyneither_ptr _tmpB59=({const char*_tmp6C5="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6C5,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB59,_tag_dyneither(_tmp6C4,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6C9;
struct Cyc_Absyn_Exp*_tmp6C6=({struct Cyc_Absyn_Exp*_tmpB5B=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB5B,({
struct Cyc_Absyn_Exp*_tmpB5A=num_elts;Cyc_Absyn_add_exp(_tmpB5A,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4352
struct Cyc_Absyn_Exp*_tmp6C7=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB5D=({struct Cyc_Absyn_Stmt*_tmpB5C=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6C6,_tmp6C7,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB5C,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4353
s->r=_tmpB5D;});};}
# 4356
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4364
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6CB=d;struct Cyc_List_List*_tmp6D2;struct Cyc_List_List*_tmp6D1;void**_tmp6D0;struct Cyc_Tcpat_Rhs*_tmp6CF;switch(*((int*)_tmp6CB)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6CF=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6CB)->f1;_LL4:
 _tmp6CF->rhs=success;return d;default: _LL5: _tmp6D2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CB)->f1;_tmp6D1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CB)->f2;_tmp6D0=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CB)->f3;_LL6:
# 4369
({void*_tmpB5E=Cyc_Toc_rewrite_decision(*_tmp6D0,success);*_tmp6D0=_tmpB5E;});
for(0;_tmp6D1 != 0;_tmp6D1=_tmp6D1->tl){
struct _tuple33*_tmp6CC=(struct _tuple33*)_tmp6D1->hd;struct _tuple33*_tmp6CD=_tmp6CC;void**_tmp6CE;_LL8: _tmp6CE=(void**)& _tmp6CD->f2;_LL9:;
({void*_tmpB5F=Cyc_Toc_rewrite_decision(*_tmp6CE,success);*_tmp6CE=_tmpB5F;});}
# 4374
return d;}_LL0:;}
# 4385 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4387
struct _RegionHandle _tmp6D3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6D3;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6D4=Cyc_Toc_share_env(rgn,nv);
void*_tmp6D5=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6D4,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4397
struct Cyc_Absyn_Stmt*_tmp6D6=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4401
({void*_tmpB60=Cyc_Toc_rewrite_decision(dopt,_tmp6D6);dopt=_tmpB60;});{
# 4403
struct Cyc_Absyn_Switch_clause*_tmp6D7=({struct Cyc_Absyn_Switch_clause*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3->pattern=p,_tmp6E3->pat_vars=0,_tmp6E3->where_clause=0,_tmp6E3->body=_tmp6D6,_tmp6E3->loc=0U;_tmp6E3;});
struct Cyc_List_List*_tmp6D8=({struct _RegionHandle*_tmpB63=rgn;struct _RegionHandle*_tmpB62=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB63,Cyc_Toc_gen_labels,_tmpB62,({struct Cyc_Absyn_Switch_clause*_tmp6E2[1U];_tmp6E2[0]=_tmp6D7;({struct _RegionHandle*_tmpB61=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB61,_tag_dyneither(_tmp6E2,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4406
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4410
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6D4,& mydecls,& mybodies,dopt,_tmp6D8,v);
# 4413
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp6D9=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp6DA=_tmp6D9;struct Cyc_Toc_Env*_tmp6DC;struct Cyc_Absyn_Stmt*_tmp6DB;_LL1: _tmp6DC=_tmp6DA->f1;_tmp6DB=_tmp6DA->f3;_LL2:;
if(_tmp6DB == _tmp6D6){senv=_tmp6DC;goto FOUND_ENV;}}
# 4418
({void*_tmp6DD=0U;({struct _dyneither_ptr _tmpB64=({const char*_tmp6DE="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6DE,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB64,_tag_dyneither(_tmp6DD,sizeof(void*),0U));});});
FOUND_ENV:
# 4422
 Cyc_Toc_stmt_to_c(senv,s);{
# 4424
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4426
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6DF=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB65=Cyc_Toc_temp_var();_tmp6DF->name=_tmpB65;});
({void*_tmpB66=Cyc_Toc_typ_to_c_array(_tmp6DF->type);_tmp6DF->type=_tmpB66;});
({struct Cyc_Absyn_Stmt*_tmpB68=({struct Cyc_Absyn_Decl*_tmpB67=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0->tag=0U,_tmp6E0->f1=_tmp6DF;_tmp6E0;}),0U);Cyc_Absyn_decl_stmt(_tmpB67,res,0U);});res=_tmpB68;});}
# 4433
({struct Cyc_Absyn_Stmt*_tmpB6C=({struct _tuple1*_tmpB6B=v;void*_tmpB6A=Cyc_Toc_typ_to_c(_tmp6D5);struct Cyc_Absyn_Exp*_tmpB69=e;Cyc_Absyn_declare_stmt(_tmpB6B,_tmpB6A,_tmpB69,res,0U);});res=_tmpB6C;});{
struct Cyc_Absyn_Stmt*_tmp6E1=res;_npop_handler(0U);return _tmp6E1;};};};};}
# 4388
;_pop_region(rgn);}
# 4441
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6E4=e->r;void*_tmp6E5=_tmp6E4;struct Cyc_Absyn_MallocInfo*_tmp70F;struct Cyc_Absyn_Stmt*_tmp70E;void**_tmp70D;void**_tmp70C;struct Cyc_List_List*_tmp70B;struct Cyc_List_List*_tmp70A;void**_tmp709;struct Cyc_List_List*_tmp708;void**_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_List_List*_tmp704;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp701;struct Cyc_Absyn_Exp*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FC;struct Cyc_Absyn_Exp*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FA;struct Cyc_Absyn_Exp*_tmp6F9;struct Cyc_Absyn_Exp*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F5;struct Cyc_List_List*_tmp6F4;struct Cyc_Absyn_Exp*_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F0;struct Cyc_Absyn_Exp*_tmp6EF;struct Cyc_Absyn_Exp*_tmp6EE;struct Cyc_Absyn_Exp*_tmp6ED;struct Cyc_Absyn_Exp*_tmp6EC;switch(*((int*)_tmp6E5)){case 20U: _LL1: _tmp6EC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL2:
 _tmp6ED=_tmp6EC;goto _LL4;case 21U: _LL3: _tmp6ED=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL4:
 _tmp6EE=_tmp6ED;goto _LL6;case 22U: _LL5: _tmp6EE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL6:
 _tmp6EF=_tmp6EE;goto _LL8;case 15U: _LL7: _tmp6EF=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL8:
 _tmp6F0=_tmp6EF;goto _LLA;case 11U: _LL9: _tmp6F0=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LLA:
 _tmp6F1=_tmp6F0;goto _LLC;case 12U: _LLB: _tmp6F1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LLC:
 _tmp6F2=_tmp6F1;goto _LLE;case 18U: _LLD: _tmp6F2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LLE:
 _tmp6F3=_tmp6F2;goto _LL10;case 5U: _LLF: _tmp6F3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL10:
 Cyc_Toc_exptypes_to_c(_tmp6F3);goto _LL0;case 3U: _LL11: _tmp6F4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL12:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6F4);goto _LL0;case 7U: _LL13: _tmp6F6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp6F5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL14:
 _tmp6F8=_tmp6F6;_tmp6F7=_tmp6F5;goto _LL16;case 8U: _LL15: _tmp6F8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp6F7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL16:
 _tmp6FA=_tmp6F8;_tmp6F9=_tmp6F7;goto _LL18;case 9U: _LL17: _tmp6FA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp6F9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL18:
 _tmp6FC=_tmp6FA;_tmp6FB=_tmp6F9;goto _LL1A;case 23U: _LL19: _tmp6FC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp6FB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL1A:
 _tmp6FE=_tmp6FC;_tmp6FD=_tmp6FB;goto _LL1C;case 35U: _LL1B: _tmp6FE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp6FD=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL1C:
 _tmp700=_tmp6FE;_tmp6FF=_tmp6FD;goto _LL1E;case 4U: _LL1D: _tmp700=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp6FF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6E5)->f3;_LL1E:
 Cyc_Toc_exptypes_to_c(_tmp700);Cyc_Toc_exptypes_to_c(_tmp6FF);goto _LL0;case 6U: _LL1F: _tmp703=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp702=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_tmp701=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E5)->f3;_LL20:
# 4459
 Cyc_Toc_exptypes_to_c(_tmp703);Cyc_Toc_exptypes_to_c(_tmp702);Cyc_Toc_exptypes_to_c(_tmp701);goto _LL0;case 10U: _LL21: _tmp705=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp704=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL22:
# 4461
 Cyc_Toc_exptypes_to_c(_tmp705);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp704);goto _LL0;case 14U: _LL23: _tmp707=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_tmp706=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL24:
({void*_tmpB6D=Cyc_Toc_typ_to_c(*_tmp707);*_tmp707=_tmpB6D;});Cyc_Toc_exptypes_to_c(_tmp706);goto _LL0;case 25U: _LL25: _tmp709=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1)->f3;_tmp708=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL26:
# 4464
({void*_tmpB6E=Cyc_Toc_typ_to_c(*_tmp709);*_tmp709=_tmpB6E;});
_tmp70A=_tmp708;goto _LL28;case 36U: _LL27: _tmp70A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6E5)->f2;_LL28:
 _tmp70B=_tmp70A;goto _LL2A;case 26U: _LL29: _tmp70B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL2A:
# 4468
 for(0;_tmp70B != 0;_tmp70B=_tmp70B->tl){
struct _tuple20 _tmp6E6=*((struct _tuple20*)_tmp70B->hd);struct _tuple20 _tmp6E7=_tmp6E6;struct Cyc_Absyn_Exp*_tmp6E8;_LL54: _tmp6E8=_tmp6E7.f2;_LL55:;
Cyc_Toc_exptypes_to_c(_tmp6E8);}
# 4472
goto _LL0;case 19U: _LL2B: _tmp70C=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL2C:
 _tmp70D=_tmp70C;goto _LL2E;case 17U: _LL2D: _tmp70D=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL2E:
({void*_tmpB6F=Cyc_Toc_typ_to_c(*_tmp70D);*_tmp70D=_tmpB6F;});goto _LL0;case 37U: _LL2F: _tmp70E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL30:
 Cyc_Toc_stmttypes_to_c(_tmp70E);goto _LL0;case 34U: _LL31: _tmp70F=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6E5)->f1;_LL32:
# 4477
 if(_tmp70F->elt_type != 0)
({void**_tmpB71=({void**_tmp6E9=_cycalloc(sizeof(*_tmp6E9));({void*_tmpB70=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp70F->elt_type)));*_tmp6E9=_tmpB70;});_tmp6E9;});_tmp70F->elt_type=_tmpB71;});
Cyc_Toc_exptypes_to_c(_tmp70F->num_elts);
goto _LL0;case 0U: _LL33: _LL34:
 goto _LL36;case 1U: _LL35: _LL36:
 goto _LL38;case 32U: _LL37: _LL38:
 goto _LL3A;case 40U: _LL39: _LL3A:
 goto _LL3C;case 33U: _LL3B: _LL3C:
 goto _LL0;case 2U: _LL3D: _LL3E:
# 4487
 goto _LL40;case 30U: _LL3F: _LL40:
 goto _LL42;case 31U: _LL41: _LL42:
 goto _LL44;case 29U: _LL43: _LL44:
 goto _LL46;case 27U: _LL45: _LL46:
 goto _LL48;case 28U: _LL47: _LL48:
 goto _LL4A;case 24U: _LL49: _LL4A:
 goto _LL4C;case 13U: _LL4B: _LL4C:
 goto _LL4E;case 16U: _LL4D: _LL4E:
 goto _LL50;case 39U: _LL4F: _LL50:
 goto _LL52;default: _LL51: _LL52:
({void*_tmp6EA=0U;({unsigned int _tmpB73=e->loc;struct _dyneither_ptr _tmpB72=({const char*_tmp6EB="Cyclone expression within C code";_tag_dyneither(_tmp6EB,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpB73,_tmpB72,_tag_dyneither(_tmp6EA,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4501
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp710=d->r;void*_tmp711=_tmp710;struct Cyc_Absyn_Typedefdecl*_tmp71B;struct Cyc_Absyn_Enumdecl*_tmp71A;struct Cyc_Absyn_Aggrdecl*_tmp719;struct Cyc_Absyn_Fndecl*_tmp718;struct Cyc_Absyn_Vardecl*_tmp717;switch(*((int*)_tmp711)){case 0U: _LL1: _tmp717=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp711)->f1;_LL2:
# 4504
({void*_tmpB74=Cyc_Toc_typ_to_c(_tmp717->type);_tmp717->type=_tmpB74;});
if(_tmp717->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp717->initializer));
goto _LL0;case 1U: _LL3: _tmp718=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp711)->f1;_LL4:
# 4508
({void*_tmpB75=Cyc_Toc_typ_to_c(_tmp718->ret_type);_tmp718->ret_type=_tmpB75;});
{struct Cyc_List_List*_tmp712=_tmp718->args;for(0;_tmp712 != 0;_tmp712=_tmp712->tl){
({void*_tmpB76=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp712->hd)).f3);(*((struct _tuple9*)_tmp712->hd)).f3=_tmpB76;});}}
# 4512
goto _LL0;case 5U: _LL5: _tmp719=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp711)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp719,1);goto _LL0;case 7U: _LL7: _tmp71A=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp711)->f1;_LL8:
# 4515
 if(_tmp71A->fields != 0){
struct Cyc_List_List*_tmp713=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp71A->fields))->v;for(0;_tmp713 != 0;_tmp713=_tmp713->tl){
struct Cyc_Absyn_Enumfield*_tmp714=(struct Cyc_Absyn_Enumfield*)_tmp713->hd;
if(_tmp714->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp714->tag));}}
# 4520
goto _LL0;case 8U: _LL9: _tmp71B=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp711)->f1;_LLA:
({void*_tmpB77=Cyc_Toc_typ_to_c((void*)_check_null(_tmp71B->defn));_tmp71B->defn=_tmpB77;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4530
({void*_tmp715=0U;({unsigned int _tmpB79=d->loc;struct _dyneither_ptr _tmpB78=({const char*_tmp716="Cyclone declaration within C code";_tag_dyneither(_tmp716,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpB79,_tmpB78,_tag_dyneither(_tmp715,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 4534
 goto _LL0;}_LL0:;}
# 4538
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp71C=s->r;void*_tmp71D=_tmp71C;struct Cyc_Absyn_Stmt*_tmp737;struct Cyc_Absyn_Stmt*_tmp736;struct Cyc_Absyn_Exp*_tmp735;struct Cyc_Absyn_Decl*_tmp734;struct Cyc_Absyn_Stmt*_tmp733;struct Cyc_Absyn_Exp*_tmp732;struct Cyc_List_List*_tmp731;void*_tmp730;struct Cyc_Absyn_Exp*_tmp72F;struct Cyc_Absyn_Exp*_tmp72E;struct Cyc_Absyn_Exp*_tmp72D;struct Cyc_Absyn_Stmt*_tmp72C;struct Cyc_Absyn_Exp*_tmp72B;struct Cyc_Absyn_Stmt*_tmp72A;struct Cyc_Absyn_Exp*_tmp729;struct Cyc_Absyn_Stmt*_tmp728;struct Cyc_Absyn_Stmt*_tmp727;struct Cyc_Absyn_Exp*_tmp726;struct Cyc_Absyn_Stmt*_tmp725;struct Cyc_Absyn_Stmt*_tmp724;struct Cyc_Absyn_Exp*_tmp723;switch(*((int*)_tmp71D)){case 1U: _LL1: _tmp723=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp723);goto _LL0;case 2U: _LL3: _tmp725=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_tmp724=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp725);Cyc_Toc_stmttypes_to_c(_tmp724);goto _LL0;case 3U: _LL5: _tmp726=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_LL6:
 if(_tmp726 != 0)Cyc_Toc_exptypes_to_c(_tmp726);goto _LL0;case 4U: _LL7: _tmp729=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_tmp728=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2;_tmp727=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp71D)->f3;_LL8:
# 4544
 Cyc_Toc_exptypes_to_c(_tmp729);Cyc_Toc_stmttypes_to_c(_tmp728);Cyc_Toc_stmttypes_to_c(_tmp727);goto _LL0;case 5U: _LL9: _tmp72B=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1).f1;_tmp72A=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2;_LLA:
# 4546
 Cyc_Toc_exptypes_to_c(_tmp72B);Cyc_Toc_stmttypes_to_c(_tmp72A);goto _LL0;case 9U: _LLB: _tmp72F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_tmp72E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2).f1;_tmp72D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp71D)->f3).f1;_tmp72C=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp71D)->f4;_LLC:
# 4548
 Cyc_Toc_exptypes_to_c(_tmp72F);Cyc_Toc_exptypes_to_c(_tmp72E);Cyc_Toc_exptypes_to_c(_tmp72D);
Cyc_Toc_stmttypes_to_c(_tmp72C);goto _LL0;case 10U: _LLD: _tmp732=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_tmp731=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2;_tmp730=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp71D)->f3;_LLE:
# 4551
 Cyc_Toc_exptypes_to_c(_tmp732);
for(0;_tmp731 != 0;_tmp731=_tmp731->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp731->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp734=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_tmp733=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp734);Cyc_Toc_stmttypes_to_c(_tmp733);goto _LL0;case 14U: _LL11: _tmp736=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp71D)->f1;_tmp735=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp736);Cyc_Toc_exptypes_to_c(_tmp735);goto _LL0;case 13U: _LL13: _tmp737=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp71D)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp737);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4561
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4564
({void*_tmpB7A=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->tag=0U;_tmp71E;});s->r=_tmpB7A;});
goto _LL0;default: _LL1F: _LL20:
# 4567
({void*_tmp71F=0U;({unsigned int _tmpB7E=s->loc;struct _dyneither_ptr _tmpB7D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp722=({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0U,({struct _dyneither_ptr _tmpB7B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp79B.f1=_tmpB7B;});_tmp79B;});void*_tmp720[1U];_tmp720[0]=& _tmp722;({struct _dyneither_ptr _tmpB7C=({const char*_tmp721="Cyclone statement in C code: %s";_tag_dyneither(_tmp721,sizeof(char),32U);});Cyc_aprintf(_tmpB7C,_tag_dyneither(_tmp720,sizeof(void*),1U));});});Cyc_Tcutil_terr(_tmpB7E,_tmpB7D,_tag_dyneither(_tmp71F,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 4575
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp738=0U;({struct _dyneither_ptr _tmpB7F=({const char*_tmp739="decls_to_c: not at toplevel!";_tag_dyneither(_tmp739,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB7F,_tag_dyneither(_tmp738,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp73A=d->r;void*_tmp73B=_tmp73A;struct Cyc_List_List*_tmp753;struct Cyc_List_List*_tmp752;struct Cyc_List_List*_tmp751;struct Cyc_List_List*_tmp750;struct Cyc_Absyn_Typedefdecl*_tmp74F;struct Cyc_Absyn_Enumdecl*_tmp74E;struct Cyc_Absyn_Datatypedecl*_tmp74D;struct Cyc_Absyn_Aggrdecl*_tmp74C;struct Cyc_Absyn_Fndecl*_tmp74B;struct Cyc_Absyn_Vardecl*_tmp74A;switch(*((int*)_tmp73B)){case 0U: _LL1: _tmp74A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LL2: {
# 4583
struct _tuple1*_tmp73C=_tmp74A->name;
# 4585
if(_tmp74A->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB81=({struct _tuple1*_tmp73D=_cycalloc(sizeof(*_tmp73D));({union Cyc_Absyn_Nmspace _tmpB80=Cyc_Absyn_Abs_n(0,1);_tmp73D->f1=_tmpB80;}),_tmp73D->f2=(*_tmp73C).f2;_tmp73D;});_tmp73C=_tmpB81;});
if(_tmp74A->initializer != 0){
if(_tmp74A->sc == Cyc_Absyn_ExternC)_tmp74A->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp74A->initializer));else{
# 4592
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp74A->initializer));}}
# 4594
_tmp74A->name=_tmp73C;
({enum Cyc_Absyn_Scope _tmpB82=Cyc_Toc_scope_to_c(_tmp74A->sc);_tmp74A->sc=_tmpB82;});
({void*_tmpB83=Cyc_Toc_typ_to_c(_tmp74A->type);_tmp74A->type=_tmpB83;});
({struct Cyc_List_List*_tmpB84=({struct Cyc_List_List*_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E->hd=d,_tmp73E->tl=Cyc_Toc_result_decls;_tmp73E;});Cyc_Toc_result_decls=_tmpB84;});
goto _LL0;}case 1U: _LL3: _tmp74B=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LL4: {
# 4600
struct _tuple1*_tmp73F=_tmp74B->name;
# 4602
if(_tmp74B->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB86=({struct _tuple1*_tmp740=_cycalloc(sizeof(*_tmp740));({union Cyc_Absyn_Nmspace _tmpB85=Cyc_Absyn_Abs_n(0,1);_tmp740->f1=_tmpB85;}),_tmp740->f2=(*_tmp73F).f2;_tmp740;});_tmp73F=_tmpB86;});
_tmp74B->sc=Cyc_Absyn_Public;}
# 4606
_tmp74B->name=_tmp73F;
Cyc_Toc_fndecl_to_c(nv,_tmp74B,cinclude);
({struct Cyc_List_List*_tmpB87=({struct Cyc_List_List*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->hd=d,_tmp741->tl=Cyc_Toc_result_decls;_tmp741;});Cyc_Toc_result_decls=_tmpB87;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp742=0U;({struct _dyneither_ptr _tmpB88=({const char*_tmp743="letdecl at toplevel";_tag_dyneither(_tmp743,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB88,_tag_dyneither(_tmp742,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp744=0U;({struct _dyneither_ptr _tmpB89=({const char*_tmp745="region decl at toplevel";_tag_dyneither(_tmp745,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB89,_tag_dyneither(_tmp744,sizeof(void*),0U));});});case 5U: _LLB: _tmp74C=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LLC:
# 4614
 Cyc_Toc_aggrdecl_to_c(_tmp74C,1);
goto _LL0;case 6U: _LLD: _tmp74D=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LLE:
# 4617
 if(_tmp74D->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp74D);else{
# 4620
Cyc_Toc_datatypedecl_to_c(_tmp74D);}
goto _LL0;case 7U: _LLF: _tmp74E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LL10:
# 4623
 Cyc_Toc_enumdecl_to_c(_tmp74E);
({struct Cyc_List_List*_tmpB8A=({struct Cyc_List_List*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->hd=d,_tmp746->tl=Cyc_Toc_result_decls;_tmp746;});Cyc_Toc_result_decls=_tmpB8A;});
goto _LL0;case 8U: _LL11: _tmp74F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LL12:
# 4627
 _tmp74F->tvs=0;
if(_tmp74F->defn != 0)
({void*_tmpB8B=Cyc_Toc_typ_to_c((void*)_check_null(_tmp74F->defn));_tmp74F->defn=_tmpB8B;});else{
# 4631
enum Cyc_Absyn_KindQual _tmp747=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp74F->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp748=_tmp747;if(_tmp748 == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB8C=Cyc_Absyn_void_star_typ();_tmp74F->defn=_tmpB8C;});goto _LL1F;}else{_LL22: _LL23:
 _tmp74F->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 4639
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB8D=({struct Cyc_List_List*_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749->hd=d,_tmp749->tl=Cyc_Toc_result_decls;_tmp749;});Cyc_Toc_result_decls=_tmpB8D;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp750=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp73B)->f2;_LL18:
 _tmp751=_tmp750;goto _LL1A;case 10U: _LL19: _tmp751=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp73B)->f2;_LL1A:
 _tmp752=_tmp751;goto _LL1C;case 11U: _LL1B: _tmp752=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpB8E=Cyc_Toc_decls_to_c(r,nv,_tmp752,top,cinclude);nv=_tmpB8E;});goto _LL0;default: _LL1D: _tmp753=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp73B)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpB8F=Cyc_Toc_decls_to_c(r,nv,_tmp753,top,1);nv=_tmpB8F;});goto _LL0;}_LL0:;};}
# 4650
return nv;}
# 4654
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp754=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp755=_tmp754;struct Cyc_Core_DynamicRegion*_tmp75A;_LL1: _tmp75A=_tmp755.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp75A->h;
({struct Cyc_Toc_TocState*_tmpB90=Cyc_Toc_empty_toc_state(h);ts=_tmpB90;});;}
({struct Cyc_Toc_TocStateWrap*_tmpB91=({struct Cyc_Toc_TocStateWrap*_tmp757=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp757));_tmp757->dyn=_tmp75A,_tmp757->state=ts;_tmp757;});Cyc_Toc_toc_state=_tmpB91;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpB92=_tag_dyneither(({unsigned int _tmp759=35;struct _dyneither_ptr**_tmp758=_cycalloc(_check_times(_tmp759,sizeof(struct _dyneither_ptr*)));_tmp758[0]=& Cyc_Toc__throw_str,_tmp758[1]=& Cyc_Toc_setjmp_str,_tmp758[2]=& Cyc_Toc__push_handler_str,_tmp758[3]=& Cyc_Toc__pop_handler_str,_tmp758[4]=& Cyc_Toc__exn_thrown_str,_tmp758[5]=& Cyc_Toc__npop_handler_str,_tmp758[6]=& Cyc_Toc__check_null_str,_tmp758[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp758[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp758[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp758[10]=& Cyc_Toc__dyneither_ptr_str,_tmp758[11]=& Cyc_Toc__tag_dyneither_str,_tmp758[12]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp758[13]=& Cyc_Toc__get_dyneither_size_str,_tmp758[14]=& Cyc_Toc__get_zero_arr_size_str,_tmp758[15]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp758[16]=& Cyc_Toc__zero_arr_plus_str,_tmp758[17]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp758[18]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp758[19]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp758[20]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp758[21]=& Cyc_Toc__cycalloc_str,_tmp758[22]=& Cyc_Toc__cyccalloc_str,_tmp758[23]=& Cyc_Toc__cycalloc_atomic_str,_tmp758[24]=& Cyc_Toc__cyccalloc_atomic_str,_tmp758[25]=& Cyc_Toc__region_malloc_str,_tmp758[26]=& Cyc_Toc__region_calloc_str,_tmp758[27]=& Cyc_Toc__check_times_str,_tmp758[28]=& Cyc_Toc__new_region_str,_tmp758[29]=& Cyc_Toc__push_region_str,_tmp758[30]=& Cyc_Toc__pop_region_str,_tmp758[31]=& Cyc_Toc__throw_arraybounds_str,_tmp758[32]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp758[33]=& Cyc_Toc__throw_match_str,_tmp758[34]=& Cyc_Toc__fast_region_malloc_str;_tmp758;}),sizeof(struct _dyneither_ptr*),35U);Cyc_Toc_globals=_tmpB92;});};}
# 4705
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp75B=ts;struct Cyc_Toc_TocStateWrap*_tmp75C=Cyc_Toc_toc_state;ts=_tmp75C;Cyc_Toc_toc_state=_tmp75B;});{
struct Cyc_Toc_TocStateWrap _tmp75D=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp75E=_tmp75D;struct Cyc_Core_DynamicRegion*_tmp764;struct Cyc_Toc_TocState*_tmp763;_LL1: _tmp764=_tmp75E.dyn;_tmp763=_tmp75E.state;_LL2:;
# 4710
{struct _RegionHandle*h=& _tmp764->h;
{struct Cyc_Toc_TocState _tmp760=*_tmp763;struct Cyc_Toc_TocState _tmp761=_tmp760;struct Cyc_Xarray_Xarray*_tmp762;_LL4: _tmp762=_tmp761.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp762);}
# 4711
;}
# 4714
Cyc_Core_free_rckey(_tmp764);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4717
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4723
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4725
({struct Cyc_Hashtable_Table**_tmpB93=({struct Cyc_Hashtable_Table**_tmp765=_cycalloc(sizeof(*_tmp765));*_tmp765=pop_tables;_tmp765;});Cyc_Toc_gpop_tables=_tmpB93;});
Cyc_Toc_init();
{struct _RegionHandle _tmp766=_new_region("start");struct _RegionHandle*start=& _tmp766;_push_region(start);
({struct _RegionHandle*_tmpB95=start;struct Cyc_Toc_Env*_tmpB94=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB95,_tmpB94,ds,1,0);});;_pop_region(start);}
# 4734
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
