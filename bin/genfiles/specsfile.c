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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
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
# 170 "core.h"
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
struct _dyneither_ptr Cyc_Specsfile_btarget;
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s);
# 35
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file);
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline);
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path;
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv);
# 45
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s);
# 31 "specsfile.cyc"
extern char* Cdef_lib_path;
extern char* Carch;
# 38
struct _dyneither_ptr Cyc_Specsfile_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
struct _dyneither_ptr Cyc_Specsfile_btarget={(void*)0,(void*)0,(void*)(0 + 0)};
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s){
Cyc_Specsfile_target_arch=s;
({struct _dyneither_ptr _tmp69=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2=({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0U,_tmp5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp5A;});void*_tmp0[1U];_tmp0[0]=& _tmp2;({struct _dyneither_ptr _tmp68=({const char*_tmp1="-b %s";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_aprintf(_tmp68,_tag_dyneither(_tmp0,sizeof(void*),1U));});});Cyc_Specsfile_btarget=_tmp69;});}
# 45
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path=0;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp3=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp3 <= 2)return;{
struct _dyneither_ptr _tmp4=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp3 - 2);
({struct Cyc_List_List*_tmp6B=({struct Cyc_List_List*_tmp6=_cycalloc(sizeof(*_tmp6));({struct _dyneither_ptr*_tmp6A=({struct _dyneither_ptr*_tmp5=_cycalloc(sizeof(*_tmp5));*_tmp5=_tmp4;_tmp5;});_tmp6->hd=_tmp6A;}),_tmp6->tl=Cyc_Specsfile_cyclone_exec_path;_tmp6;});Cyc_Specsfile_cyclone_exec_path=_tmp6B;});};}struct _tuple0{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 59
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp7=0;
int c;
int i;
char strname[256U];
char strvalue[4096U];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(file,sizeof(char),1U)),"r");
# 67
if(spec_file == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0U,_tmp5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp5B;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp6D=Cyc_stderr;struct _dyneither_ptr _tmp6C=({const char*_tmp9="Error opening spec file %s\n";_tag_dyneither(_tmp9,sizeof(char),28U);});Cyc_fprintf(_tmp6D,_tmp6C,_tag_dyneither(_tmp8,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 0;}
# 73
while(1){
while(1){
({int _tmp6E=Cyc_fgetc(spec_file);c=_tmp6E;});
if( isspace(c))continue;
if(c == '*')break;
if(c != - 1){
({struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0U,_tmp5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp5D;});struct Cyc_Int_pa_PrintArg_struct _tmpE=({struct Cyc_Int_pa_PrintArg_struct _tmp5C;_tmp5C.tag=1U,_tmp5C.f1=(unsigned long)c;_tmp5C;});void*_tmpB[2U];_tmpB[0]=& _tmpD,_tmpB[1]=& _tmpE;({struct Cyc___cycFILE*_tmp70=Cyc_stderr;struct _dyneither_ptr _tmp6F=({const char*_tmpC="Error reading spec file %s: unexpected character '%c'\n";_tag_dyneither(_tmpC,sizeof(char),55U);});Cyc_fprintf(_tmp70,_tmp6F,_tag_dyneither(_tmpB,sizeof(void*),2U));});});
# 82
Cyc_fflush(Cyc_stderr);}
# 84
goto CLEANUP_AND_RETURN;}
# 86
JUST_AFTER_STAR:
 i=0;
while(1){
({int _tmp71=Cyc_fgetc(spec_file);c=_tmp71;});
if(c == - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0U,_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp5E;});void*_tmpF[1U];_tmpF[0]=& _tmp11;({struct Cyc___cycFILE*_tmp73=Cyc_stderr;struct _dyneither_ptr _tmp72=({const char*_tmp10="Error reading spec file %s: unexpected EOF\n";_tag_dyneither(_tmp10,sizeof(char),44U);});Cyc_fprintf(_tmp73,_tmp72,_tag_dyneither(_tmpF,sizeof(void*),1U));});});
# 94
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 97
if(c == ':'){
strname[_check_known_subscript_notnull(256U,i)]='\000';
break;}
# 101
strname[_check_known_subscript_notnull(256U,i)]=(char)c;
++ i;
if(i >= 256){
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0U,_tmp5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp5F;});void*_tmp12[1U];_tmp12[0]=& _tmp14;({struct Cyc___cycFILE*_tmp75=Cyc_stderr;struct _dyneither_ptr _tmp74=({const char*_tmp13="Error reading spec file %s: string name too long\n";_tag_dyneither(_tmp13,sizeof(char),50U);});Cyc_fprintf(_tmp75,_tmp74,_tag_dyneither(_tmp12,sizeof(void*),1U));});});
# 107
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 111
while(1){
({int _tmp76=Cyc_fgetc(spec_file);c=_tmp76;});
if( isspace(c))continue;
break;}
# 116
if(c == '*'){
# 118
({struct Cyc_List_List*_tmp7C=({struct Cyc_List_List*_tmp19=_cycalloc(sizeof(*_tmp19));({struct _tuple0*_tmp7B=({struct _tuple0*_tmp18=_cycalloc(sizeof(*_tmp18));({struct _dyneither_ptr*_tmp7A=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));({struct _dyneither_ptr _tmp79=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256U));*_tmp15=_tmp79;});_tmp15;});_tmp18->f1=_tmp7A;}),({
struct _dyneither_ptr*_tmp78=({struct _dyneither_ptr*_tmp17=_cycalloc(sizeof(*_tmp17));({struct _dyneither_ptr _tmp77=(struct _dyneither_ptr)Cyc_strdup(({const char*_tmp16="";_tag_dyneither(_tmp16,sizeof(char),1U);}));*_tmp17=_tmp77;});_tmp17;});_tmp18->f2=_tmp78;});_tmp18;});
# 118
_tmp19->hd=_tmp7B;}),_tmp19->tl=_tmp7;_tmp19;});_tmp7=_tmp7C;});
# 120
goto JUST_AFTER_STAR;}
# 123
strvalue[0]=(char)c;
i=1;
while(1){
({int _tmp7D=Cyc_fgetc(spec_file);c=_tmp7D;});
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096U,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096U,i)]=(char)c;
++ i;
if(i >= 4096){
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0U,_tmp61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp61;});struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp60;_tmp60.tag=0U,({
# 133
struct _dyneither_ptr _tmp7E=(struct _dyneither_ptr)_tag_dyneither(strname,sizeof(char),256U);_tmp60.f1=_tmp7E;});_tmp60;});void*_tmp1A[2U];_tmp1A[0]=& _tmp1C,_tmp1A[1]=& _tmp1D;({struct Cyc___cycFILE*_tmp80=Cyc_stderr;struct _dyneither_ptr _tmp7F=({const char*_tmp1B="Error reading spec file %s: value of %s too long\n";_tag_dyneither(_tmp1B,sizeof(char),50U);});Cyc_fprintf(_tmp80,_tmp7F,_tag_dyneither(_tmp1A,sizeof(void*),2U));});});
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 139
({struct Cyc_List_List*_tmp86=({struct Cyc_List_List*_tmp21=_cycalloc(sizeof(*_tmp21));({struct _tuple0*_tmp85=({struct _tuple0*_tmp20=_cycalloc(sizeof(*_tmp20));({struct _dyneither_ptr*_tmp84=({struct _dyneither_ptr*_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct _dyneither_ptr _tmp83=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256U));*_tmp1E=_tmp83;});_tmp1E;});_tmp20->f1=_tmp84;}),({
struct _dyneither_ptr*_tmp82=({struct _dyneither_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));({struct _dyneither_ptr _tmp81=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096U));*_tmp1F=_tmp81;});_tmp1F;});_tmp20->f2=_tmp82;});_tmp20;});
# 139
_tmp21->hd=_tmp85;}),_tmp21->tl=_tmp7;_tmp21;});_tmp7=_tmp86;});
# 141
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 144
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp7;}
# 150
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline){
if(({char*_tmp87=(char*)cmdline.curr;_tmp87 == (char*)(_tag_dyneither(0,0,0)).curr;}))return _tag_dyneither(0,0,0);{
unsigned long _tmp22=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp23=0;
char buf[4096U];
int i=0;
int j=0;
if(_tmp22 > 4096)goto DONE;
while(1){
# 160
while(1){
if(i >= _tmp22)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(! isspace((int)((const char*)cmdline.curr)[i]))break;
++ i;}
# 166
j=0;
# 171
while(1){
if(i >= _tmp22)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if( isspace((int)((const char*)cmdline.curr)[i]))break;
if(((const char*)cmdline.curr)[i]== '\\'){
# 177
++ i;
if(i >= _tmp22)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096U,j)]=((const char*)cmdline.curr)[i];
++ j;}else{
# 184
buf[_check_known_subscript_notnull(4096U,j)]=((const char*)cmdline.curr)[i];
++ j;}
# 187
++ i;}
# 189
if(j < 4096)
buf[_check_known_subscript_notnull(4096U,j)]='\000';
# 192
({struct Cyc_List_List*_tmp8A=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));({struct _dyneither_ptr*_tmp89=({struct _dyneither_ptr*_tmp24=_cycalloc(sizeof(*_tmp24));({struct _dyneither_ptr _tmp88=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096U));*_tmp24=_tmp88;});_tmp24;});_tmp25->hd=_tmp89;}),_tmp25->tl=_tmp23;_tmp25;});_tmp23=_tmp8A;});}
# 194
DONE:
({struct Cyc_List_List*_tmp8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp23);_tmp23=_tmp8B;});
({struct Cyc_List_List*_tmp8E=({struct Cyc_List_List*_tmp28=_cycalloc(sizeof(*_tmp28));({struct _dyneither_ptr*_tmp8D=({struct _dyneither_ptr*_tmp27=_cycalloc(sizeof(*_tmp27));({struct _dyneither_ptr _tmp8C=({const char*_tmp26="";_tag_dyneither(_tmp26,sizeof(char),1U);});*_tmp27=_tmp8C;});_tmp27;});_tmp28->hd=_tmp8D;}),_tmp28->tl=_tmp23;_tmp28;});_tmp23=_tmp8E;});{
struct _dyneither_ptr _tmp29=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp23);
struct _dyneither_ptr _tmp2A=({unsigned int _tmp2C=_get_dyneither_size(_tmp29,sizeof(struct _dyneither_ptr*));struct _dyneither_ptr*_tmp2B=_cycalloc(_check_times(_tmp2C,sizeof(struct _dyneither_ptr)));({{unsigned int _tmp62=_get_dyneither_size(_tmp29,sizeof(struct _dyneither_ptr*));unsigned int k;for(k=0;k < _tmp62;++ k){_tmp2B[k]=*((struct _dyneither_ptr**)_tmp29.curr)[(int)k];}}0;});_tag_dyneither(_tmp2B,sizeof(struct _dyneither_ptr),_tmp2C);});
return _tmp2A;};};}
# 202
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name){
struct _handler_cons _tmp2D;_push_handler(& _tmp2D);{int _tmp2F=0;if(setjmp(_tmp2D.handler))_tmp2F=1;if(!_tmp2F){
{struct _dyneither_ptr _tmp30=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,specs,& spec_name);_npop_handler(0U);return _tmp30;};_pop_handler();}else{void*_tmp2E=(void*)_exn_thrown;void*_tmp31=_tmp2E;void*_tmp32;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp31)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 207
 return _tag_dyneither(0,0,0);}else{_LL3: _tmp32=_tmp31;_LL4:(int)_rethrow(_tmp32);}_LL0:;}};}
# 211
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path=0;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path={(void*)0,(void*)0,(void*)(0 + 0)};
# 216
static struct Cyc_List_List*Cyc_Specsfile_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 218
struct Cyc_List_List*_tmp33=0;
for(0;dirs != 0;dirs=dirs->tl){
({struct Cyc_List_List*_tmp8F=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->hd=(struct _dyneither_ptr*)dirs->hd,_tmp34->tl=_tmp33;_tmp34;});_tmp33=_tmp8F;});
({struct Cyc_List_List*_tmp92=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _dyneither_ptr*_tmp91=({struct _dyneither_ptr*_tmp35=_cycalloc(sizeof(*_tmp35));({struct _dyneither_ptr _tmp90=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir);*_tmp35=_tmp90;});_tmp35;});_tmp36->hd=_tmp91;}),_tmp36->tl=_tmp33;_tmp36;});_tmp33=_tmp92;});}
# 223
({struct Cyc_List_List*_tmp93=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp33);_tmp33=_tmp93;});
return _tmp33;}
# 238 "specsfile.cyc"
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv){
# 246
int argc=(int)_get_dyneither_size(argv,sizeof(struct _dyneither_ptr));
struct _dyneither_ptr _tmp37=({unsigned int _tmp4A=(unsigned int)argc;int*_tmp49=_cycalloc_atomic(_check_times(_tmp4A,sizeof(int)));({{unsigned int _tmp65=(unsigned int)argc;unsigned int i;for(i=0;i < _tmp65;++ i){_tmp49[i]=0;}}0;});_tag_dyneither(_tmp49,sizeof(int),_tmp4A);});
int _tmp38=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
if(({struct _dyneither_ptr _tmp94=({const char*_tmp39="-B";_tag_dyneither(_tmp39,sizeof(char),3U);});Cyc_strncmp(_tmp94,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2U);})== 0){
*((int*)_check_dyneither_subscript(_tmp37,sizeof(int),i))=1;
++ _tmp38;}else{
# 255
if(({struct _dyneither_ptr _tmp95=({const char*_tmp3A="-b";_tag_dyneither(_tmp3A,sizeof(char),3U);});Cyc_strcmp(_tmp95,(struct _dyneither_ptr)((struct _dyneither_ptr*)argv.curr)[i]);})== 0){
*((int*)_check_dyneither_subscript(_tmp37,sizeof(int),i))=1;
++ _tmp38;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp37,sizeof(int),i))=1;
++ _tmp38;}}}{
# 267
struct _dyneither_ptr _tmp3B=({unsigned int _tmp48=(unsigned int)(_tmp38 + 1);struct _dyneither_ptr*_tmp47=_cycalloc(_check_times(_tmp48,sizeof(struct _dyneither_ptr)));({{unsigned int _tmp64=(unsigned int)(_tmp38 + 1);unsigned int n;for(n=0;n < _tmp64;++ n){({struct _dyneither_ptr _tmp96=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp47[n]=_tmp96;});}}0;});_tag_dyneither(_tmp47,sizeof(struct _dyneither_ptr),_tmp48);});
struct _dyneither_ptr _tmp3C=({unsigned int _tmp46=(unsigned int)(argc - _tmp38);struct _dyneither_ptr*_tmp45=_cycalloc(_check_times(_tmp46,sizeof(struct _dyneither_ptr)));({{unsigned int _tmp63=(unsigned int)(argc - _tmp38);unsigned int n;for(n=0;n < _tmp63;++ n){({struct _dyneither_ptr _tmp97=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp45[n]=_tmp97;});}}0;});_tag_dyneither(_tmp45,sizeof(struct _dyneither_ptr),_tmp46);});
({struct _dyneither_ptr _tmp99=({struct _dyneither_ptr _tmp98=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp3C,sizeof(struct _dyneither_ptr),0))=_tmp98;});*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp3B,sizeof(struct _dyneither_ptr),0))=_tmp99;});
for(({int _tmp9B=({int _tmp9A=k=1;j=_tmp9A;});i=_tmp9B;});i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp37,sizeof(int),i)))({struct _dyneither_ptr _tmp9C=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp3B,sizeof(struct _dyneither_ptr),j ++))=_tmp9C;});else{
({struct _dyneither_ptr _tmp9D=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp3C,sizeof(struct _dyneither_ptr),k ++))=_tmp9D;});}}
# 275
Cyc_Arg_current=0;
Cyc_Arg_parse(specs,anonfun,anonflagfun,errmsg,_tmp3B);
# 280
if(({char*_tmp9E=(char*)Cyc_Specsfile_target_arch.curr;_tmp9E == (char*)(_tag_dyneither(0,0,0)).curr;}))({struct _dyneither_ptr _tmp9F=({char*_tmp3D= Carch;_tag_dyneither(_tmp3D,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D,1U));});Cyc_Specsfile_target_arch=_tmp9F;});{
# 289 "specsfile.cyc"
struct _dyneither_ptr cyclone_exec_prefix=({char*_tmp44= getenv("CYCLONE_EXEC_PREFIX");_tag_dyneither(_tmp44,sizeof(char),_get_zero_arr_size_char((void*)_tmp44,1U));});
if(({char*_tmpA0=(char*)cyclone_exec_prefix.curr;_tmpA0 != (char*)(_tag_dyneither(0,0,0)).curr;}))
({struct Cyc_List_List*_tmpA2=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(*_tmp3F));({struct _dyneither_ptr*_tmpA1=({struct _dyneither_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));*_tmp3E=cyclone_exec_prefix;_tmp3E;});_tmp3F->hd=_tmpA1;}),_tmp3F->tl=Cyc_Specsfile_cyclone_exec_path;_tmp3F;});Cyc_Specsfile_cyclone_exec_path=_tmpA2;});
({struct _dyneither_ptr _tmpA3=({char*_tmp40= Cdef_lib_path;_tag_dyneither(_tmp40,sizeof(char),_get_zero_arr_size_char((void*)_tmp40,1U));});Cyc_Specsfile_def_lib_path=_tmpA3;});
if(Cyc_strlen((struct _dyneither_ptr)Cyc_Specsfile_def_lib_path)> 0)
({struct Cyc_List_List*_tmpA7=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));
({struct _dyneither_ptr*_tmpA6=({struct _dyneither_ptr*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _dyneither_ptr _tmpA5=(struct _dyneither_ptr)({struct _dyneither_ptr _tmpA4=Cyc_Specsfile_def_lib_path;Cyc_Filename_concat(_tmpA4,({const char*_tmp41="cyc-lib";_tag_dyneither(_tmp41,sizeof(char),8U);}));});*_tmp42=_tmpA5;});_tmp42;});_tmp43->hd=_tmpA6;}),_tmp43->tl=Cyc_Specsfile_cyclone_exec_path;_tmp43;});
# 294
Cyc_Specsfile_cyclone_exec_path=_tmpA7;});
# 297
({struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Specsfile_cyclone_exec_path);Cyc_Specsfile_cyclone_exec_path=_tmpA8;});
({struct Cyc_List_List*_tmpA9=Cyc_Specsfile_also_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);Cyc_Specsfile_cyclone_arch_path=_tmpA9;});
return _tmp3C;};};}
# 303
static int Cyc_Specsfile_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmp4B;_push_handler(& _tmp4B);{int _tmp4D=0;if(setjmp(_tmp4B.handler))_tmp4D=1;if(!_tmp4D){({struct Cyc___cycFILE*_tmpAB=({struct _dyneither_ptr _tmpAA=file;Cyc_file_open(_tmpAA,({const char*_tmp4E="r";_tag_dyneither(_tmp4E,sizeof(char),2U);}));});f=_tmpAB;});;_pop_handler();}else{void*_tmp4C=(void*)_exn_thrown;void*_tmp4F=_tmp4C;_LL1: _LL2: goto _LL0;_LL0:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 312
static struct _dyneither_ptr*Cyc_Specsfile_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if(({char*_tmpAC=(char*)file.curr;_tmpAC == (char*)(_tag_dyneither(0,0,0)).curr;}))return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmp50=*((struct _dyneither_ptr*)dirs->hd);
if(({char*_tmpAD=(char*)_tmp50.curr;_tmpAD == (char*)(_tag_dyneither(0,0,0)).curr;}) || Cyc_strlen((struct _dyneither_ptr)_tmp50)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmp50,file);
if(Cyc_Specsfile_file_exists(s))return({struct _dyneither_ptr*_tmp51=_cycalloc(sizeof(*_tmp51));*_tmp51=s;_tmp51;});};}
# 320
return 0;}
# 324
static struct _dyneither_ptr Cyc_Specsfile_sprint_list(struct Cyc_List_List*dirs){
struct _dyneither_ptr tmp=({const char*_tmp54="";_tag_dyneither(_tmp54,sizeof(char),1U);});
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmp52=*((struct _dyneither_ptr*)dirs->hd);
if(({char*_tmpAE=(char*)_tmp52.curr;_tmpAE == (char*)(_tag_dyneither(0,0,0)).curr;}) || Cyc_strlen((struct _dyneither_ptr)_tmp52)== 0)continue;
({struct _dyneither_ptr _tmpB0=(struct _dyneither_ptr)({struct _dyneither_ptr _tmpAF=(struct _dyneither_ptr)_tmp52;Cyc_strconcat(_tmpAF,({const char*_tmp53=":";_tag_dyneither(_tmp53,sizeof(char),2U);}));});_tmp52=_tmpB0;});
({struct _dyneither_ptr _tmpB1=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmp52,(struct _dyneither_ptr)tmp);tmp=_tmpB1;});}
# 332
return tmp;}
# 335
static struct _dyneither_ptr Cyc_Specsfile_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmp55=Cyc_Specsfile_find(dirs,file);
if(_tmp55 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp58=({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0U,_tmp67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp67;});struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0U,({
struct _dyneither_ptr _tmpB2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Specsfile_sprint_list(dirs));_tmp66.f1=_tmpB2;});_tmp66;});void*_tmp56[2U];_tmp56[0]=& _tmp58,_tmp56[1]=& _tmp59;({struct Cyc___cycFILE*_tmpB4=Cyc_stderr;struct _dyneither_ptr _tmpB3=({const char*_tmp57="Error: can't find internal compiler file %s in path %s\n";_tag_dyneither(_tmp57,sizeof(char),56U);});Cyc_fprintf(_tmpB4,_tmpB3,_tag_dyneither(_tmp56,sizeof(void*),2U));});});
Cyc_fflush(Cyc_stderr);
return _tag_dyneither(0,0,0);}
# 343
return*_tmp55;}
# 346
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s){
return Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_arch_path,s);}
# 350
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s){
return Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_exec_path,s);}
