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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
# 300
int isspace(int);
# 313
char* getenv(const char*);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 364
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 103 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 108
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 69 "arg.h"
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 29 "specsfile.h"
struct _dyneither_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file);
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline);
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path;
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv);
# 44
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s);
# 31 "specsfile.cyc"
extern char* Cdef_lib_path;
extern char* Carch;
# 38
struct _dyneither_ptr Cyc_Specsfile_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s){
Cyc_Specsfile_target_arch=s;}
# 43
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path=0;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp0=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp0 <= (unsigned long)2)return;{
struct _dyneither_ptr _tmp1=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp0 - (unsigned long)2);
Cyc_Specsfile_cyclone_exec_path=({struct Cyc_List_List*_tmp3=_cycalloc(sizeof(*_tmp3));({struct _dyneither_ptr*_tmp64=({struct _dyneither_ptr*_tmp2=_cycalloc(sizeof(*_tmp2));*_tmp2=_tmp1;_tmp2;});_tmp3->hd=_tmp64;}),_tmp3->tl=Cyc_Specsfile_cyclone_exec_path;_tmp3;});};}struct _tuple0{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 57
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp4=0;
int c;
int i;
char strname[256U];
char strvalue[4096U];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(file,sizeof(char),1U)),"r");
# 65
if(spec_file == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp57;_tmp57.tag=0U,_tmp57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp57;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct Cyc___cycFILE*_tmp66=Cyc_stderr;struct _dyneither_ptr _tmp65=({const char*_tmp6="Error opening spec file %s\n";_tag_dyneither(_tmp6,sizeof(char),28U);});Cyc_fprintf(_tmp66,_tmp65,_tag_dyneither(_tmp5,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 0;}
# 71
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if( isspace(c))continue;
if(c == (int)'*')break;
if(c != -1){
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0U,_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp59;});struct Cyc_Int_pa_PrintArg_struct _tmpB=({struct Cyc_Int_pa_PrintArg_struct _tmp58;_tmp58.tag=1U,_tmp58.f1=(unsigned long)c;_tmp58;});void*_tmp8[2U];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB;({struct Cyc___cycFILE*_tmp68=Cyc_stderr;struct _dyneither_ptr _tmp67=({const char*_tmp9="Error reading spec file %s: unexpected character '%c'\n";_tag_dyneither(_tmp9,sizeof(char),55U);});Cyc_fprintf(_tmp68,_tmp67,_tag_dyneither(_tmp8,sizeof(void*),2U));});});
# 80
Cyc_fflush(Cyc_stderr);}
# 82
goto CLEANUP_AND_RETURN;}
# 84
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == -1){
({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0U,_tmp5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp5A;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({struct Cyc___cycFILE*_tmp6A=Cyc_stderr;struct _dyneither_ptr _tmp69=({const char*_tmpD="Error reading spec file %s: unexpected EOF\n";_tag_dyneither(_tmpD,sizeof(char),44U);});Cyc_fprintf(_tmp6A,_tmp69,_tag_dyneither(_tmpC,sizeof(void*),1U));});});
# 92
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 95
if(c == (int)':'){
*((char*)_check_known_subscript_notnull(strname,256U,sizeof(char),i))='\000';
break;}
# 99
*((char*)_check_known_subscript_notnull(strname,256U,sizeof(char),i))=(char)c;
++ i;
if(i >= 256){
({struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0U,_tmp5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp5B;});void*_tmpF[1U];_tmpF[0]=& _tmp11;({struct Cyc___cycFILE*_tmp6C=Cyc_stderr;struct _dyneither_ptr _tmp6B=({const char*_tmp10="Error reading spec file %s: string name too long\n";_tag_dyneither(_tmp10,sizeof(char),50U);});Cyc_fprintf(_tmp6C,_tmp6B,_tag_dyneither(_tmpF,sizeof(void*),1U));});});
# 105
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 109
while(1){
c=Cyc_fgetc(spec_file);
if( isspace(c))continue;
break;}
# 114
if(c == (int)'*'){
# 116
_tmp4=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct _tuple0*_tmp71=({struct _tuple0*_tmp15=_cycalloc(sizeof(*_tmp15));({struct _dyneither_ptr*_tmp70=({struct _dyneither_ptr*_tmp12=_cycalloc(sizeof(*_tmp12));({struct _dyneither_ptr _tmp6F=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256U));*_tmp12=_tmp6F;});_tmp12;});_tmp15->f1=_tmp70;}),({
struct _dyneither_ptr*_tmp6E=({struct _dyneither_ptr*_tmp14=_cycalloc(sizeof(*_tmp14));({struct _dyneither_ptr _tmp6D=(struct _dyneither_ptr)Cyc_strdup(({const char*_tmp13="";_tag_dyneither(_tmp13,sizeof(char),1U);}));*_tmp14=_tmp6D;});_tmp14;});_tmp15->f2=_tmp6E;});_tmp15;});
# 116
_tmp16->hd=_tmp71;}),_tmp16->tl=_tmp4;_tmp16;});
# 118
goto JUST_AFTER_STAR;}
# 121
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == -1  || c == (int)'\n') || c == (int)'\r'){*((char*)_check_known_subscript_notnull(strvalue,4096U,sizeof(char),i))='\000';break;}
*((char*)_check_known_subscript_notnull(strvalue,4096U,sizeof(char),i))=(char)c;
++ i;
if(i >= 4096){
({struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0U,_tmp5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp5D;});struct Cyc_String_pa_PrintArg_struct _tmp1A=({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0U,({
# 131
struct _dyneither_ptr _tmp72=(struct _dyneither_ptr)_tag_dyneither(strname,sizeof(char),256U);_tmp5C.f1=_tmp72;});_tmp5C;});void*_tmp17[2U];_tmp17[0]=& _tmp19,_tmp17[1]=& _tmp1A;({struct Cyc___cycFILE*_tmp74=Cyc_stderr;struct _dyneither_ptr _tmp73=({const char*_tmp18="Error reading spec file %s: value of %s too long\n";_tag_dyneither(_tmp18,sizeof(char),50U);});Cyc_fprintf(_tmp74,_tmp73,_tag_dyneither(_tmp17,sizeof(void*),2U));});});
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 137
_tmp4=({struct Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct _tuple0*_tmp79=({struct _tuple0*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct _dyneither_ptr*_tmp78=({struct _dyneither_ptr*_tmp1B=_cycalloc(sizeof(*_tmp1B));({struct _dyneither_ptr _tmp77=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256U));*_tmp1B=_tmp77;});_tmp1B;});_tmp1D->f1=_tmp78;}),({
struct _dyneither_ptr*_tmp76=({struct _dyneither_ptr*_tmp1C=_cycalloc(sizeof(*_tmp1C));({struct _dyneither_ptr _tmp75=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096U));*_tmp1C=_tmp75;});_tmp1C;});_tmp1D->f2=_tmp76;});_tmp1D;});
# 137
_tmp1E->hd=_tmp79;}),_tmp1E->tl=_tmp4;_tmp1E;});
# 139
if(c == -1)goto CLEANUP_AND_RETURN;}
# 142
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp4;}
# 148
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline){
if(({char*_tmp7A=(char*)cmdline.curr;_tmp7A == (char*)(_tag_dyneither(0,0,0)).curr;}))return _tag_dyneither(0,0,0);{
unsigned long _tmp1F=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp20=0;
char buf[4096U];
int i=0;
int j=0;
if(_tmp1F > (unsigned long)4096)goto DONE;
while(1){
# 158
while(1){
if((unsigned long)i >= _tmp1F)goto DONE;
if((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(! isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 164
j=0;
# 169
while(1){
if((unsigned long)i >= _tmp1F)break;
if((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if( isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== (int)'\\'){
# 175
++ i;
if((unsigned long)i >= _tmp1F)break;
if((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
({char _tmp7B=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));*((char*)_check_known_subscript_notnull(buf,4096U,sizeof(char),j))=_tmp7B;});
++ j;}else{
# 182
({char _tmp7C=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));*((char*)_check_known_subscript_notnull(buf,4096U,sizeof(char),j))=_tmp7C;});
++ j;}
# 185
++ i;}
# 187
if(j < 4096)
*((char*)_check_known_subscript_notnull(buf,4096U,sizeof(char),j))='\000';
# 190
_tmp20=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));({struct _dyneither_ptr*_tmp7E=({struct _dyneither_ptr*_tmp21=_cycalloc(sizeof(*_tmp21));({struct _dyneither_ptr _tmp7D=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096U));*_tmp21=_tmp7D;});_tmp21;});_tmp22->hd=_tmp7E;}),_tmp22->tl=_tmp20;_tmp22;});}
# 192
DONE:
 _tmp20=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp20);
_tmp20=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));({struct _dyneither_ptr*_tmp80=({struct _dyneither_ptr*_tmp24=_cycalloc(sizeof(*_tmp24));({struct _dyneither_ptr _tmp7F=({const char*_tmp23="";_tag_dyneither(_tmp23,sizeof(char),1U);});*_tmp24=_tmp7F;});_tmp24;});_tmp25->hd=_tmp80;}),_tmp25->tl=_tmp20;_tmp25;});{
struct _dyneither_ptr _tmp26=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp20);
struct _dyneither_ptr _tmp27=({unsigned int _tmp29=_get_dyneither_size(_tmp26,sizeof(struct _dyneither_ptr*));struct _dyneither_ptr*_tmp28=_cycalloc(_check_times(_tmp29,sizeof(struct _dyneither_ptr)));({{unsigned int _tmp5E=_get_dyneither_size(_tmp26,sizeof(struct _dyneither_ptr*));unsigned int k;for(k=0;k < _tmp5E;++ k){_tmp28[k]=*(*((struct _dyneither_ptr**)_check_dyneither_subscript(_tmp26,sizeof(struct _dyneither_ptr*),(int)k)));}}0;});_tag_dyneither(_tmp28,sizeof(struct _dyneither_ptr),_tmp29);});
return _tmp27;};};}
# 200
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name){
struct _handler_cons _tmp2A;_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!_tmp2C){
{struct _dyneither_ptr _tmp2D=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,specs,& spec_name);_npop_handler(0U);return _tmp2D;};_pop_handler();}else{void*_tmp2B=(void*)_exn_thrown;void*_tmp2E=_tmp2B;void*_tmp2F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2E)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 205
 return _tag_dyneither(0,0,0);}else{_LL3: _tmp2F=_tmp2E;_LL4:(int)_rethrow(_tmp2F);}_LL0:;}};}
# 209
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path=0;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path={(void*)0,(void*)0,(void*)(0 + 0)};
# 214
static struct Cyc_List_List*Cyc_Specsfile_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 216
struct Cyc_List_List*_tmp30=0;
for(0;dirs != 0;dirs=dirs->tl){
_tmp30=({struct Cyc_List_List*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->hd=(struct _dyneither_ptr*)dirs->hd,_tmp31->tl=_tmp30;_tmp31;});
_tmp30=({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));({struct _dyneither_ptr*_tmp82=({struct _dyneither_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));({struct _dyneither_ptr _tmp81=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir);*_tmp32=_tmp81;});_tmp32;});_tmp33->hd=_tmp82;}),_tmp33->tl=_tmp30;_tmp33;});}
# 221
_tmp30=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp30);
return _tmp30;}
# 236 "specsfile.cyc"
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv){
# 244
int argc=(int)_get_dyneither_size(argv,sizeof(struct _dyneither_ptr));
struct _dyneither_ptr _tmp34=({unsigned int _tmp47=(unsigned int)argc;int*_tmp46=_cycalloc_atomic(_check_times(_tmp47,sizeof(int)));({{unsigned int _tmp61=(unsigned int)argc;unsigned int i;for(i=0;i < _tmp61;++ i){_tmp46[i]=0;}}0;});_tag_dyneither(_tmp46,sizeof(int),_tmp47);});
int _tmp35=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
if(({struct _dyneither_ptr _tmp83=({const char*_tmp36="-B";_tag_dyneither(_tmp36,sizeof(char),3U);});Cyc_strncmp(_tmp83,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2U);})== 0){
*((int*)_check_dyneither_subscript(_tmp34,sizeof(int),i))=1;
++ _tmp35;}else{
# 253
if(({struct _dyneither_ptr _tmp84=({const char*_tmp37="-b";_tag_dyneither(_tmp37,sizeof(char),3U);});Cyc_strcmp(_tmp84,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)));})== 0){
*((int*)_check_dyneither_subscript(_tmp34,sizeof(int),i))=1;
++ _tmp35;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp34,sizeof(int),i))=1;
++ _tmp35;}}}{
# 265
struct _dyneither_ptr _tmp38=({unsigned int _tmp45=(unsigned int)(_tmp35 + 1);struct _dyneither_ptr*_tmp44=_cycalloc(_check_times(_tmp45,sizeof(struct _dyneither_ptr)));({{unsigned int _tmp60=(unsigned int)(_tmp35 + 1);unsigned int n;for(n=0;n < _tmp60;++ n){({struct _dyneither_ptr _tmp85=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp44[n]=_tmp85;});}}0;});_tag_dyneither(_tmp44,sizeof(struct _dyneither_ptr),_tmp45);});
struct _dyneither_ptr _tmp39=({unsigned int _tmp43=(unsigned int)(argc - _tmp35);struct _dyneither_ptr*_tmp42=_cycalloc(_check_times(_tmp43,sizeof(struct _dyneither_ptr)));({{unsigned int _tmp5F=(unsigned int)(argc - _tmp35);unsigned int n;for(n=0;n < _tmp5F;++ n){({struct _dyneither_ptr _tmp86=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp42[n]=_tmp86;});}}0;});_tag_dyneither(_tmp42,sizeof(struct _dyneither_ptr),_tmp43);});
({struct _dyneither_ptr _tmp88=({struct _dyneither_ptr _tmp87=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp39,sizeof(struct _dyneither_ptr),0))=_tmp87;});*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp38,sizeof(struct _dyneither_ptr),0))=_tmp88;});
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp34,sizeof(int),i)))({struct _dyneither_ptr _tmp89=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp38,sizeof(struct _dyneither_ptr),j ++))=_tmp89;});else{
({struct _dyneither_ptr _tmp8A=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp39,sizeof(struct _dyneither_ptr),k ++))=_tmp8A;});}}
# 273
Cyc_Arg_current=0;
Cyc_Arg_parse(specs,anonfun,anonflagfun,errmsg,_tmp38);
# 278
if(({char*_tmp8B=(char*)Cyc_Specsfile_target_arch.curr;_tmp8B == (char*)(_tag_dyneither(0,0,0)).curr;}))Cyc_Specsfile_target_arch=({char*_tmp3A= Carch;_tag_dyneither(_tmp3A,sizeof(char),_get_zero_arr_size_char((void*)_tmp3A,1U));});{
# 287 "specsfile.cyc"
struct _dyneither_ptr cyclone_exec_prefix=({char*_tmp41= getenv("CYCLONE_EXEC_PREFIX");_tag_dyneither(_tmp41,sizeof(char),_get_zero_arr_size_char((void*)_tmp41,1U));});
if(({char*_tmp8C=(char*)cyclone_exec_prefix.curr;_tmp8C != (char*)(_tag_dyneither(0,0,0)).curr;}))
Cyc_Specsfile_cyclone_exec_path=({struct Cyc_List_List*_tmp3C=_cycalloc(sizeof(*_tmp3C));({struct _dyneither_ptr*_tmp8D=({struct _dyneither_ptr*_tmp3B=_cycalloc(sizeof(*_tmp3B));*_tmp3B=cyclone_exec_prefix;_tmp3B;});_tmp3C->hd=_tmp8D;}),_tmp3C->tl=Cyc_Specsfile_cyclone_exec_path;_tmp3C;});
Cyc_Specsfile_def_lib_path=({char*_tmp3D= Cdef_lib_path;_tag_dyneither(_tmp3D,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D,1U));});
if(Cyc_strlen((struct _dyneither_ptr)Cyc_Specsfile_def_lib_path)> (unsigned long)0)
Cyc_Specsfile_cyclone_exec_path=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));
({struct _dyneither_ptr*_tmp90=({struct _dyneither_ptr*_tmp3F=_cycalloc(sizeof(*_tmp3F));({struct _dyneither_ptr _tmp8F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp8E=Cyc_Specsfile_def_lib_path;Cyc_Filename_concat(_tmp8E,({const char*_tmp3E="cyc-lib";_tag_dyneither(_tmp3E,sizeof(char),8U);}));});*_tmp3F=_tmp8F;});_tmp3F;});_tmp40->hd=_tmp90;}),_tmp40->tl=Cyc_Specsfile_cyclone_exec_path;_tmp40;});
# 295
Cyc_Specsfile_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Specsfile_cyclone_exec_path);
Cyc_Specsfile_cyclone_arch_path=Cyc_Specsfile_also_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);
return _tmp39;};};}
# 301
static int Cyc_Specsfile_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmp48;_push_handler(& _tmp48);{int _tmp4A=0;if(setjmp(_tmp48.handler))_tmp4A=1;if(!_tmp4A){f=({struct _dyneither_ptr _tmp91=file;Cyc_file_open(_tmp91,({const char*_tmp4B="r";_tag_dyneither(_tmp4B,sizeof(char),2U);}));});;_pop_handler();}else{void*_tmp49=(void*)_exn_thrown;void*_tmp4C=_tmp49;_LL1: _LL2: goto _LL0;_LL0:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 310
static struct _dyneither_ptr*Cyc_Specsfile_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if(({char*_tmp92=(char*)file.curr;_tmp92 == (char*)(_tag_dyneither(0,0,0)).curr;}))return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmp4D=*((struct _dyneither_ptr*)dirs->hd);
if(({char*_tmp93=(char*)_tmp4D.curr;_tmp93 == (char*)(_tag_dyneither(0,0,0)).curr;}) || Cyc_strlen((struct _dyneither_ptr)_tmp4D)== (unsigned long)0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmp4D,file);
if(Cyc_Specsfile_file_exists(s))return({struct _dyneither_ptr*_tmp4E=_cycalloc(sizeof(*_tmp4E));*_tmp4E=s;_tmp4E;});};}
# 318
return 0;}
# 322
static struct _dyneither_ptr Cyc_Specsfile_sprint_list(struct Cyc_List_List*dirs){
struct _dyneither_ptr tmp=({const char*_tmp51="";_tag_dyneither(_tmp51,sizeof(char),1U);});
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmp4F=*((struct _dyneither_ptr*)dirs->hd);
if(({char*_tmp94=(char*)_tmp4F.curr;_tmp94 == (char*)(_tag_dyneither(0,0,0)).curr;}) || Cyc_strlen((struct _dyneither_ptr)_tmp4F)== (unsigned long)0)continue;
_tmp4F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp95=(struct _dyneither_ptr)_tmp4F;Cyc_strconcat(_tmp95,({const char*_tmp50=":";_tag_dyneither(_tmp50,sizeof(char),2U);}));});
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmp4F,(struct _dyneither_ptr)tmp);}
# 330
return tmp;}
# 333
static struct _dyneither_ptr Cyc_Specsfile_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmp52=Cyc_Specsfile_find(dirs,file);
if(_tmp52 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0U,_tmp63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp63;});struct Cyc_String_pa_PrintArg_struct _tmp56=({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0U,({
struct _dyneither_ptr _tmp96=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_sprint_list(dirs));_tmp62.f1=_tmp96;});_tmp62;});void*_tmp53[2U];_tmp53[0]=& _tmp55,_tmp53[1]=& _tmp56;({struct Cyc___cycFILE*_tmp98=Cyc_stderr;struct _dyneither_ptr _tmp97=({const char*_tmp54="Error: can't find internal compiler file %s in path %s\n";_tag_dyneither(_tmp54,sizeof(char),56U);});Cyc_fprintf(_tmp98,_tmp97,_tag_dyneither(_tmp53,sizeof(void*),2U));});});
Cyc_fflush(Cyc_stderr);
return _tag_dyneither(0,0,0);}
# 341
return*_tmp52;}
# 344
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s){
return Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_arch_path,s);}
# 348
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s){
return Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_exec_path,s);}
