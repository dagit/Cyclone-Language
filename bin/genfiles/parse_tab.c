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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 80 "lexing.h"
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 108 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Position_Error;
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 301
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 314
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 462
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 469
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 705 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 864
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 893
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 897
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
# 902
void*Cyc_Absyn_compress_kb(void*);
# 913
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 915
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 918
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 920
extern void*Cyc_Absyn_sint_type;
# 922
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 925
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 929
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 931
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 956
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 960
void*Cyc_Absyn_bounds_one();
# 962
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 986
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 989
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1087
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1090
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1094
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1099
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1101
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1108
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1117
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1122
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1128
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1157
int Cyc_Absyn_fntype_att(void*);
# 1179
extern int Cyc_Absyn_porting_c_code;struct Cyc_RgnOrder_RgnPO;
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
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 49 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*t);
# 99
void*Cyc_Tcutil_copy_type(void*t);
# 112
void*Cyc_Tcutil_compress(void*t);
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
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
# 160
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 162
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 167
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 177
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 313 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 353
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 363
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 66 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();
void Cyc_Lex_enter_extern_c();
void Cyc_Lex_leave_extern_c();
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _dyneither_ptr Cyc_Lex_token_string;
# 94 "parse.y"
int Cyc_Parse_parsing_tempest=0;struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};
# 100
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 103
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 112
return first;}}
# 117
int Cyc_Parse_no_register=0;char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 134
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned int varloc;struct Cyc_List_List*tms;};struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 170
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 174
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp5;struct Cyc_Absyn_Exp*_tmp4;_LL1: _tmp5=_tmp1.f1;_tmp4=_tmp1.f2;_LL2:;
declarators=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators,_tmp2->hd=_tmp5;_tmp2;});
exprs=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4,_tmp3->tl=exprs;_tmp3;});}
# 181
({struct Cyc_List_List*_tmp738=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp738;});
({struct _tuple10*_tmp739=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp739;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 191
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 197
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 199
static void*Cyc_Parse_parse_abort(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 201
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=Cyc_Parse_Exit;_tmp6;}));}
# 205
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 207
struct _tuple8*_tmp7=tqt;struct Cyc_Absyn_Tqual _tmpB;void*_tmpA;_LL1: _tmpB=_tmp7->f2;_tmpA=_tmp7->f3;_LL2:;
if((_tmpB.print_const  || _tmpB.q_volatile) || _tmpB.q_restrict){
if(_tmpB.loc != (unsigned int)0)loc=_tmpB.loc;
({void*_tmp8=0U;({unsigned int _tmp73B=loc;struct _dyneither_ptr _tmp73A=({const char*_tmp9="qualifier on type is ignored";_tag_dyneither(_tmp9,sizeof(char),29U);});Cyc_Warn_warn(_tmp73B,_tmp73A,_tag_dyneither(_tmp8,sizeof(void*),0U));});});}
# 212
return _tmpA;}struct _tuple14{void*f1;void*f2;void*f3;void*f4;};
# 215
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 221
void*zeroterm=Cyc_Tcutil_any_bool(0);
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;void*_tmpD=_tmpC;void*_tmpF;struct Cyc_Absyn_Exp*_tmpE;switch(*((int*)_tmpD)){case 4U: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5U: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7U: _LL5: _LL6:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 6U: _LL7: _LL8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3U: _LL9: _LLA:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2U: _LLB: _LLC:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0U: _LLD: _tmpE=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LLE:
 bound=Cyc_Absyn_thin_bounds_exp(_tmpE);goto _LL0;default: _LLF: _tmpF=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10:
 rgn=_tmpF;goto _LL0;}_LL0:;}
# 233
return({struct _tuple14 _tmp6A0;_tmp6A0.f1=nullable,_tmp6A0.f2=bound,_tmp6A0.f3=zeroterm,_tmp6A0.f4=rgn;_tmp6A0;});}
# 239
struct _tuple0*Cyc_Parse_gensym_enum(){
# 241
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp73F=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp73F;}),({
struct _dyneither_ptr*_tmp73E=({struct _dyneither_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _dyneither_ptr _tmp73D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6A1;_tmp6A1.tag=1U,_tmp6A1.f1=(unsigned long)enum_counter ++;_tmp6A1;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _dyneither_ptr _tmp73C=({const char*_tmp11="__anonymous_enum_%d__";_tag_dyneither(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp73C,_tag_dyneither(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp73D;});_tmp13;});_tmp14->f2=_tmp73E;});_tmp14;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 251
struct _tuple17*_tmp15=field_info;unsigned int _tmp22;struct _tuple0*_tmp21;struct Cyc_Absyn_Tqual _tmp20;void*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;_LL1: _tmp22=(_tmp15->f1)->f1;_tmp21=(_tmp15->f1)->f2;_tmp20=(_tmp15->f1)->f3;_tmp1F=(_tmp15->f1)->f4;_tmp1E=(_tmp15->f1)->f5;_tmp1D=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1B=(_tmp15->f2)->f2;_LL2:;
if(_tmp1E != 0)
({void*_tmp16=0U;({unsigned int _tmp741=loc;struct _dyneither_ptr _tmp740=({const char*_tmp17="bad type params in struct field";_tag_dyneither(_tmp17,sizeof(char),32U);});Cyc_Warn_err(_tmp741,_tmp740,_tag_dyneither(_tmp16,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp21))
({void*_tmp18=0U;({unsigned int _tmp743=loc;struct _dyneither_ptr _tmp742=({const char*_tmp19="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp19,sizeof(char),59U);});Cyc_Warn_err(_tmp743,_tmp742,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->name=(*_tmp21).f2,_tmp1A->tq=_tmp20,_tmp1A->type=_tmp1F,_tmp1A->width=_tmp1C,_tmp1A->attributes=_tmp1D,_tmp1A->requires_clause=_tmp1B;_tmp1A;});}
# 261
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp6A2;_tmp6A2.Signed_spec=0,_tmp6A2.Unsigned_spec=0,_tmp6A2.Short_spec=0,_tmp6A2.Long_spec=0,_tmp6A2.Long_Long_spec=0,_tmp6A2.Valid_type_spec=0,_tmp6A2.Type_spec=Cyc_Absyn_sint_type,_tmp6A2.loc=loc;_tmp6A2;});}
# 272
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);
_tmp23.Type_spec=t;
_tmp23.Valid_type_spec=1;
return _tmp23;}
# 278
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);
_tmp24.Signed_spec=1;
return _tmp24;}
# 283
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);
_tmp25.Unsigned_spec=1;
return _tmp25;}
# 288
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);
_tmp26.Short_spec=1;
return _tmp26;}
# 293
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);
_tmp27.Long_spec=1;
return _tmp27;}
# 300
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 302
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp744=t;Cyc_Tcutil_promote_array(_tmp744,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 315 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_tmp2A=_tmp29;struct _dyneither_ptr _tmp3E;void**_tmp3D;struct _dyneither_ptr*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp3C=_tmp2A->f1;_tmp3B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp3C != 0){_LL2:
# 320
{void*_tmp2B=_tmp3B;void**_tmp32;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->tag == 1U){_LL8: _tmp32=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2;_LL9: {
# 324
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _dyneither_ptr _tmp746=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp6A3;_tmp6A3.tag=0U,_tmp6A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3C);_tmp6A3;});void*_tmp2E[1U];_tmp2E[0]=& _tmp30;({struct _dyneither_ptr _tmp745=({const char*_tmp2F="`%s";_tag_dyneither(_tmp2F,sizeof(char),4U);});Cyc_aprintf(_tmp745,_tag_dyneither(_tmp2E,sizeof(void*),1U));});});*_tmp31=_tmp746;});_tmp31;});
({void*_tmp748=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->name=nm,_tmp2D->identity=- 1,({void*_tmp747=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,_tmp2C->f1=& Cyc_Tcutil_ik;_tmp2C;});_tmp2D->kind=_tmp747;});_tmp2D;}));*_tmp32=_tmp748;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 329
_tmp28=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));({struct _tuple18*_tmp749=({struct _tuple18*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=_tmp3C,_tmp33->f2=_tmp3B;_tmp33;});_tmp34->hd=_tmp749;}),_tmp34->tl=_tmp28;_tmp34;});goto _LL0;}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp3E=*_tmp2A->f1;_tmp3D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {
# 333
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct _dyneither_ptr _tmp74B=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp6A4;_tmp6A4.tag=0U,_tmp6A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E);_tmp6A4;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({struct _dyneither_ptr _tmp74A=({const char*_tmp38="`%s";_tag_dyneither(_tmp38,sizeof(char),4U);});Cyc_aprintf(_tmp74A,_tag_dyneither(_tmp37,sizeof(void*),1U));});});*_tmp3A=_tmp74B;});_tmp3A;});
({void*_tmp74D=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->name=nm,_tmp36->identity=- 1,({void*_tmp74C=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=0U,_tmp35->f1=& Cyc_Tcutil_rk;_tmp35;});_tmp36->kind=_tmp74C;});_tmp36;}));*_tmp3D=_tmp74D;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 339
return _tmp28;}
# 343
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3F=0;
for(0;x != 0;x=x->tl){
void*_tmp40=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp41=_tmp40;void*_tmp44;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->tl == 0){_LL1: _tmp44=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd;_LL2:
# 348
 _tmp3F=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));({struct _tuple18*_tmp74E=({struct _tuple18*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp42->f2=_tmp44;_tmp42;});_tmp43->hd=_tmp74E;}),_tmp43->tl=_tmp3F;_tmp43;});goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 352
return _tmp3F;}
# 356
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp45=e->r;void*_tmp46=_tmp45;struct _dyneither_ptr*_tmp4D;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp4D=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f2;_LL2:
# 359
{struct Cyc_List_List*_tmp47=tags;for(0;_tmp47 != 0;_tmp47=_tmp47->tl){
struct _tuple18*_tmp48=(struct _tuple18*)_tmp47->hd;struct _tuple18*_tmp49=_tmp48;struct _dyneither_ptr*_tmp4C;void*_tmp4B;_LL6: _tmp4C=_tmp49->f1;_tmp4B=_tmp49->f2;_LL7:;
if(Cyc_strptrcmp(_tmp4C,_tmp4D)== 0)
return({void*_tmp750=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=39U,({void*_tmp74F=Cyc_Tcutil_copy_type(_tmp4B);_tmp4A->f1=_tmp74F;});_tmp4A;});Cyc_Absyn_new_exp(_tmp750,e->loc);});}}
# 364
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 367
return e;}
# 372
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4E=t;struct Cyc_Absyn_Exp*_tmp61;void*_tmp60;void*_tmp5F;struct Cyc_Absyn_Tqual _tmp5E;void*_tmp5D;void*_tmp5C;void*_tmp5B;void*_tmp5A;struct Cyc_Absyn_PtrLoc*_tmp59;void*_tmp58;struct Cyc_Absyn_Tqual _tmp57;struct Cyc_Absyn_Exp*_tmp56;void*_tmp55;unsigned int _tmp54;switch(*((int*)_tmp4E)){case 4U: _LL1: _tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).tq;_tmp56=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).num_elts;_tmp55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zero_term;_tmp54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zt_loc;_LL2: {
# 375
struct Cyc_Absyn_Exp*nelts2=_tmp56;
if(_tmp56 != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp56);{
# 379
void*_tmp4F=Cyc_Parse_substitute_tags(tags,_tmp58);
if(_tmp56 != nelts2  || _tmp58 != _tmp4F)
return Cyc_Absyn_array_type(_tmp4F,_tmp57,nelts2,_tmp55,_tmp54);
goto _LL0;};}case 3U: _LL3: _tmp5F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp5E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_tq;_tmp5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).rgn;_tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).nullable;_tmp5B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).bounds;_tmp5A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).zero_term;_tmp59=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).ptrloc;_LL4: {
# 384
void*_tmp50=Cyc_Parse_substitute_tags(tags,_tmp5F);
void*_tmp51=Cyc_Parse_substitute_tags(tags,_tmp5B);
if(_tmp50 != _tmp5F  || _tmp51 != _tmp5B)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6A5;_tmp6A5.elt_type=_tmp50,_tmp6A5.elt_tq=_tmp5E,(_tmp6A5.ptr_atts).rgn=_tmp5D,(_tmp6A5.ptr_atts).nullable=_tmp5C,(_tmp6A5.ptr_atts).bounds=_tmp51,(_tmp6A5.ptr_atts).zero_term=_tmp5A,(_tmp6A5.ptr_atts).ptrloc=_tmp59;_tmp6A5;}));
goto _LL0;}case 0U: if(((struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f1)->tag == 13U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->tl == 0){_LL5: _tmp60=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->hd;_LL6: {
# 390
void*_tmp52=Cyc_Parse_substitute_tags(tags,_tmp60);
if(_tmp60 != _tmp52)return Cyc_Absyn_thin_bounds_type(_tmp52);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9U: _LL7: _tmp61=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E)->f1;_LL8: {
# 394
struct Cyc_Absyn_Exp*_tmp53=Cyc_Parse_substitute_tags_exp(tags,_tmp61);
if(_tmp53 != _tmp61)return Cyc_Absyn_valueof_type(_tmp53);
goto _LL0;}default: _LL9: _LLA:
# 400
 goto _LL0;}_LL0:;}
# 402
return t;}
# 407
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp751=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp751;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 414
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 416
return({struct _tuple19*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=(*t).f2,_tmp62->f2=(*t).f3;_tmp62;});}
# 419
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp63=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp63)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 428
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(({struct _dyneither_ptr _tmp752=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp752,({const char*_tmp64="`H";_tag_dyneither(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;else{
if(({struct _dyneither_ptr _tmp753=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp753,({const char*_tmp65="`U";_tag_dyneither(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;else{
if(({struct _dyneither_ptr _tmp754=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp754,({const char*_tmp66="`RC";_tag_dyneither(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;else{
# 436
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _dyneither_ptr*_tmp755=({struct _dyneither_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp755;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}}}}
# 443
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp69=t;struct Cyc_Absyn_Tvar*_tmp6C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->tag == 2U){_LL1: _tmp6C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;_LL2:
 return _tmp6C;}else{_LL3: _LL4:
({void*_tmp6A=0U;({unsigned int _tmp757=loc;struct _dyneither_ptr _tmp756=({const char*_tmp6B="expecting a list of type variables, not types";_tag_dyneither(_tmp6B,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp757,_tmp756,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});}_LL0:;}
# 451
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_tmp6E=_tmp6D;void**_tmp72;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->tag == 2U){_LL1: _tmp72=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1)->kind;_LL2: {
# 454
void*_tmp6F=Cyc_Absyn_compress_kb(*_tmp72);void*_tmp70=_tmp6F;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp70)->tag == 1U){_LL6: _LL7:
# 456
 if(!leq)({void*_tmp758=Cyc_Tcutil_kind_to_bound(k);*_tmp72=_tmp758;});else{
({void*_tmp759=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2U,_tmp71->f1=0,_tmp71->f2=k;_tmp71;});*_tmp72=_tmp759;});}
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 461
 return;}_LL0:;}
# 466
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 472
if(tds == 0)return tms;
# 477
if(tms == 0)return 0;{
# 479
void*_tmp73=(void*)tms->hd;void*_tmp74=_tmp73;void*_tmp8F;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->tag == 3U){_LL1: _tmp8F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->f1;_LL2:
# 482
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 485
void*_tmp75=_tmp8F;struct Cyc_List_List*_tmp8D;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp75)->tag == 1U){_LL6: _LL7:
# 487
({void*_tmp76=0U;({unsigned int _tmp75B=loc;struct _dyneither_ptr _tmp75A=({const char*_tmp77="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp77,sizeof(char),93U);});Cyc_Warn_warn(_tmp75B,_tmp75A,_tag_dyneither(_tmp76,sizeof(void*),0U));});});
# 489
return tms;}else{_LL8: _tmp8D=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp75)->f1;_LL9:
# 491
 if(({int _tmp75C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);_tmp75C != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp78=0U;({unsigned int _tmp75E=loc;struct _dyneither_ptr _tmp75D=({const char*_tmp79="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp79,sizeof(char),73U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp75E,_tmp75D,_tag_dyneither(_tmp78,sizeof(void*),0U));});});{
# 495
struct Cyc_List_List*rev_new_params=0;
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
struct Cyc_List_List*_tmp7A=tds;
for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){
struct Cyc_Absyn_Decl*_tmp7B=(struct Cyc_Absyn_Decl*)_tmp7A->hd;
void*_tmp7C=_tmp7B->r;void*_tmp7D=_tmp7C;struct Cyc_Absyn_Vardecl*_tmp86;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->tag == 0U){_LLB: _tmp86=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LLC:
# 502
 if(Cyc_zstrptrcmp((*_tmp86->name).f2,(struct _dyneither_ptr*)_tmp8D->hd)!= 0)
continue;
if(_tmp86->initializer != 0)
({void*_tmp7E=0U;({unsigned int _tmp760=_tmp7B->loc;struct _dyneither_ptr _tmp75F=({const char*_tmp7F="initializer found in parameter declaration";_tag_dyneither(_tmp7F,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp760,_tmp75F,_tag_dyneither(_tmp7E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp86->name))
({void*_tmp80=0U;({unsigned int _tmp762=_tmp7B->loc;struct _dyneither_ptr _tmp761=({const char*_tmp81="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp81,sizeof(char),47U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp762,_tmp761,_tag_dyneither(_tmp80,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));
({struct _tuple8*_tmp763=({struct _tuple8*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(*_tmp86->name).f2,_tmp82->f2=_tmp86->tq,_tmp82->f3=_tmp86->type;_tmp82;});_tmp83->hd=_tmp763;}),_tmp83->tl=rev_new_params;_tmp83;});
# 511
goto L;}else{_LLD: _LLE:
({void*_tmp84=0U;({unsigned int _tmp765=_tmp7B->loc;struct _dyneither_ptr _tmp764=({const char*_tmp85="nonvariable declaration in parameter type";_tag_dyneither(_tmp85,sizeof(char),42U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp765,_tmp764,_tag_dyneither(_tmp84,sizeof(void*),0U));});});}_LLA:;}
# 515
L: if(_tmp7A == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp6A6;_tmp6A6.tag=0U,_tmp6A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp8D->hd));_tmp6A6;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({unsigned int _tmp767=loc;struct _dyneither_ptr _tmp766=({const char*_tmp88="%s is not given a type";_tag_dyneither(_tmp88,sizeof(char),23U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp767,_tmp766,_tag_dyneither(_tmp87,sizeof(void*),1U));});});}
# 518
return({struct Cyc_List_List*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));
({void*_tmp76A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8B=_region_malloc(yy,sizeof(*_tmp8B));_tmp8B->tag=3U,({void*_tmp769=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8A=_region_malloc(yy,sizeof(*_tmp8A));_tmp8A->tag=1U,({struct Cyc_List_List*_tmp768=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp8A->f1=_tmp768;}),_tmp8A->f2=0,_tmp8A->f3=0,_tmp8A->f4=0,_tmp8A->f5=0,_tmp8A->f6=0,_tmp8A->f7=0;_tmp8A;});_tmp8B->f1=_tmp769;});_tmp8B;});_tmp8C->hd=_tmp76A;}),_tmp8C->tl=0;_tmp8C;});};}_LL5:;}
# 525
goto _LL4;}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));_tmp8E->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp76B=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8E->tl=_tmp76B;});_tmp8E;});}_LL0:;};}
# 533
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 537
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6A7;_tmp6A7.id=d.id,_tmp6A7.varloc=d.varloc,({struct Cyc_List_List*_tmp76C=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6A7.tms=_tmp76C;});_tmp6A7;});{
# 540
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 546
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 552
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmp90=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmp91=_tmp90;switch(_tmp91){case Cyc_Parse_Extern_sc: _LL1: _LL2:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL3: _LL4:
 sc=0U;goto _LL0;default: _LL5: _LL6:
({void*_tmp92=0U;({unsigned int _tmp76E=loc;struct _dyneither_ptr _tmp76D=({const char*_tmp93="bad storage class on function";_tag_dyneither(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp76E,_tmp76D,_tag_dyneither(_tmp92,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 559
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmp95=Cyc_Parse_apply_tms(tq,_tmp94,atts,d.tms);struct _tuple13 _tmp96=_tmp95;struct Cyc_Absyn_Tqual _tmpA6;void*_tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA3;_LL8: _tmpA6=_tmp96.f1;_tmpA5=_tmp96.f2;_tmpA4=_tmp96.f3;_tmpA3=_tmp96.f4;_LL9:;
# 564
if(_tmpA4 != 0)
# 567
({void*_tmp97=0U;({unsigned int _tmp770=loc;struct _dyneither_ptr _tmp76F=({const char*_tmp98="bad type params, ignoring";_tag_dyneither(_tmp98,sizeof(char),26U);});Cyc_Warn_warn(_tmp770,_tmp76F,_tag_dyneither(_tmp97,sizeof(void*),0U));});});{
# 569
void*_tmp99=_tmpA5;struct Cyc_Absyn_FnInfo _tmpA2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->tag == 5U){_LLB: _tmpA2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1;_LLC:
# 571
{struct Cyc_List_List*_tmp9A=_tmpA2.args;for(0;_tmp9A != 0;_tmp9A=_tmp9A->tl){
if((*((struct _tuple8*)_tmp9A->hd)).f1 == 0){
({void*_tmp9B=0U;({unsigned int _tmp772=loc;struct _dyneither_ptr _tmp771=({const char*_tmp9C="missing argument variable in function prototype";_tag_dyneither(_tmp9C,sizeof(char),48U);});Cyc_Warn_err(_tmp772,_tmp771,_tag_dyneither(_tmp9B,sizeof(void*),0U));});});
({struct _dyneither_ptr*_tmp774=({struct _dyneither_ptr*_tmp9E=_cycalloc(sizeof(*_tmp9E));({struct _dyneither_ptr _tmp773=({const char*_tmp9D="?";_tag_dyneither(_tmp9D,sizeof(char),2U);});*_tmp9E=_tmp773;});_tmp9E;});(*((struct _tuple8*)_tmp9A->hd)).f1=_tmp774;});}}}
# 578
({struct Cyc_List_List*_tmp775=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA2.attributes,_tmpA3);_tmpA2.attributes=_tmp775;});
return({struct Cyc_Absyn_Fndecl*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->sc=sc,_tmp9F->is_inline=is_inline,_tmp9F->name=d.id,_tmp9F->body=body,_tmp9F->i=_tmpA2,_tmp9F->cached_type=0,_tmp9F->param_vardecls=0,_tmp9F->fn_vardecl=0;_tmp9F;});}else{_LLD: _LLE:
# 582
({void*_tmpA0=0U;({unsigned int _tmp777=loc;struct _dyneither_ptr _tmp776=({const char*_tmpA1="declarator is not a function prototype";_tag_dyneither(_tmpA1,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp777,_tmp776,_tag_dyneither(_tmpA0,sizeof(void*),0U));});});}_LLA:;};};};}static char _tmpA7[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 586
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpA7,_tmpA7,_tmpA7 + 76U};static char _tmpA8[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 588
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpA8,_tmpA8,_tmpA8 + 87U};static char _tmpA9[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 590
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpA9,_tmpA9,_tmpA9 + 74U};static char _tmpAA[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 592
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpAA,_tmpAA,_tmpAA + 84U};
# 599
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 602
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpAB=0U;({unsigned int _tmp779=loc;struct _dyneither_ptr _tmp778=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp779,_tmp778,_tag_dyneither(_tmpAB,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpAC=0U;({unsigned int _tmp77B=loc;struct _dyneither_ptr _tmp77A=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp77B,_tmp77A,_tag_dyneither(_tmpAC,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpAD=0U;({unsigned int _tmp77D=loc;struct _dyneither_ptr _tmp77C=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp77D,_tmp77C,_tag_dyneither(_tmpAD,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpAE=0U;({unsigned int _tmp77F=loc;struct _dyneither_ptr _tmp77E=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp77F,_tmp77E,_tag_dyneither(_tmpAE,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpAF=0U;({unsigned int _tmp781=loc;struct _dyneither_ptr _tmp780=Cyc_Parse_msg1;Cyc_Warn_err(_tmp781,_tmp780,_tag_dyneither(_tmpAF,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 624
return s1;}
# 630
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 633
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 640
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpB0=0U;({unsigned int _tmp783=loc;struct _dyneither_ptr _tmp782=Cyc_Parse_msg4;Cyc_Warn_err(_tmp783,_tmp782,_tag_dyneither(_tmpB0,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpB1=0U;({unsigned int _tmp785=loc;struct _dyneither_ptr _tmp784=Cyc_Parse_msg4;Cyc_Warn_err(_tmp785,_tmp784,_tag_dyneither(_tmpB1,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 652
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpB2=0U;({unsigned int _tmp787=loc;struct _dyneither_ptr _tmp786=({const char*_tmpB3="missing type within specifier";_tag_dyneither(_tmpB3,sizeof(char),30U);});Cyc_Warn_warn(_tmp787,_tmp786,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});
t=Cyc_Absyn_int_type(sgn,sz);}else{
# 657
if(seen_sign){
void*_tmpB4=t;enum Cyc_Absyn_Sign _tmpB8;enum Cyc_Absyn_Size_of _tmpB7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->tag == 1U){_LL1: _tmpB8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f1;_tmpB7=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f2;_LL2:
# 660
 if((int)_tmpB8 != (int)sgn)
t=Cyc_Absyn_int_type(sgn,_tmpB7);
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpB5=0U;({unsigned int _tmp789=loc;struct _dyneither_ptr _tmp788=({const char*_tmpB6="sign specification on non-integral type";_tag_dyneither(_tmpB6,sizeof(char),40U);});Cyc_Warn_err(_tmp789,_tmp788,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 665
if(seen_size){
void*_tmpB9=t;enum Cyc_Absyn_Sign _tmpBD;enum Cyc_Absyn_Size_of _tmpBC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)){case 1U: _LL6: _tmpBD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f1;_tmpBC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f2;_LL7:
# 668
 if((int)_tmpBC != (int)sz)
t=Cyc_Absyn_int_type(_tmpBD,sz);
goto _LL5;case 2U: _LL8: _LL9:
# 672
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpBA=0U;({unsigned int _tmp78B=loc;struct _dyneither_ptr _tmp78A=({const char*_tmpBB="size qualifier on non-integral type";_tag_dyneither(_tmpBB,sizeof(char),36U);});Cyc_Warn_err(_tmp78B,_tmp78A,_tag_dyneither(_tmpBA,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 676
return t;}
# 679
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 683
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpBE=d.id;
unsigned int _tmpBF=d.varloc;
struct _tuple13 _tmpC0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpC1=_tmpC0;struct Cyc_Absyn_Tqual _tmpC9;void*_tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*_tmpC6;_LL1: _tmpC9=_tmpC1.f1;_tmpC8=_tmpC1.f2;_tmpC7=_tmpC1.f3;_tmpC6=_tmpC1.f4;_LL2:;
# 690
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpC3=_region_malloc(r,sizeof(*_tmpC3));({struct _tuple15*_tmp78C=({struct _tuple15*_tmpC2=_region_malloc(r,sizeof(*_tmpC2));_tmpC2->f1=_tmpBF,_tmpC2->f2=_tmpBE,_tmpC2->f3=_tmpC9,_tmpC2->f4=_tmpC8,_tmpC2->f5=_tmpC7,_tmpC2->f6=_tmpC6;_tmpC2;});_tmpC3->hd=_tmp78C;}),_tmpC3->tl=0;_tmpC3;});else{
# 693
return({struct Cyc_List_List*_tmpC5=_region_malloc(r,sizeof(*_tmpC5));({struct _tuple15*_tmp792=({struct _tuple15*_tmpC4=_region_malloc(r,sizeof(*_tmpC4));_tmpC4->f1=_tmpBF,_tmpC4->f2=_tmpBE,_tmpC4->f3=_tmpC9,_tmpC4->f4=_tmpC8,_tmpC4->f5=_tmpC7,_tmpC4->f6=_tmpC6;_tmpC4;});_tmpC5->hd=_tmp792;}),({
struct Cyc_List_List*_tmp791=({struct _RegionHandle*_tmp790=r;struct Cyc_Absyn_Tqual _tmp78F=tq;void*_tmp78E=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp78D=ds->tl;Cyc_Parse_apply_tmss(_tmp790,_tmp78F,_tmp78E,_tmp78D,shared_atts);});_tmpC5->tl=_tmp791;});_tmpC5;});}};}
# 697
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 700
if(tms == 0)return({struct _tuple13 _tmp6A8;_tmp6A8.f1=tq,_tmp6A8.f2=t,_tmp6A8.f3=0,_tmp6A8.f4=atts;_tmp6A8;});{
void*_tmpCA=(void*)tms->hd;void*_tmpCB=_tmpCA;unsigned int _tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_PtrAtts _tmpEE;struct Cyc_Absyn_Tqual _tmpED;struct Cyc_List_List*_tmpEC;unsigned int _tmpEB;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpE8;unsigned int _tmpE7;void*_tmpE6;unsigned int _tmpE5;switch(*((int*)_tmpCB)){case 0U: _LL1: _tmpE6=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpE5=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL2:
# 703
 return({struct Cyc_Absyn_Tqual _tmp795=Cyc_Absyn_empty_tqual(0U);void*_tmp794=
Cyc_Absyn_array_type(t,tq,0,_tmpE6,_tmpE5);
# 703
struct Cyc_List_List*_tmp793=atts;Cyc_Parse_apply_tms(_tmp795,_tmp794,_tmp793,tms->tl);});case 1U: _LL3: _tmpE9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpE8=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_tmpE7=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f3;_LL4:
# 706
 return({struct Cyc_Absyn_Tqual _tmp798=Cyc_Absyn_empty_tqual(0U);void*_tmp797=
Cyc_Absyn_array_type(t,tq,_tmpE9,_tmpE8,_tmpE7);
# 706
struct Cyc_List_List*_tmp796=atts;Cyc_Parse_apply_tms(_tmp798,_tmp797,_tmp796,tms->tl);});case 3U: _LL5: _tmpEA=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_LL6: {
# 709
void*_tmpCC=_tmpEA;unsigned int _tmpE2;struct Cyc_List_List*_tmpE1;int _tmpE0;struct Cyc_Absyn_VarargInfo*_tmpDF;void*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->tag == 1U){_LLE: _tmpE1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f1;_tmpE0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f2;_tmpDF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f3;_tmpDE=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f4;_tmpDD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f5;_tmpDC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f6;_tmpDB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f7;_LLF: {
# 711
struct Cyc_List_List*typvars=0;
# 713
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(void*)as->hd,_tmpCD->tl=fn_atts;_tmpCD;});else{
# 718
new_atts=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=(void*)as->hd,_tmpCE->tl=new_atts;_tmpCE;});}}}
# 721
if(tms->tl != 0){
void*_tmpCF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmpD0=_tmpCF;struct Cyc_List_List*_tmpD1;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->tag == 4U){_LL13: _tmpD1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->f1;_LL14:
# 724
 typvars=_tmpD1;
tms=tms->tl;
goto _LL12;}else{_LL15: _LL16:
# 728
 goto _LL12;}_LL12:;}
# 732
if(((((!_tmpE0  && _tmpDF == 0) && _tmpE1 != 0) && _tmpE1->tl == 0) && (*((struct _tuple8*)_tmpE1->hd)).f1 == 0) && (*((struct _tuple8*)_tmpE1->hd)).f3 == Cyc_Absyn_void_type)
# 737
_tmpE1=0;{
# 740
struct Cyc_List_List*_tmpD2=Cyc_Parse_get_arg_tags(_tmpE1);
# 742
if(_tmpD2 != 0)
t=Cyc_Parse_substitute_tags(_tmpD2,t);
t=Cyc_Parse_array2ptr(t,0);
# 747
{struct Cyc_List_List*_tmpD3=_tmpE1;for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
struct _tuple8*_tmpD4=(struct _tuple8*)_tmpD3->hd;struct _tuple8*_tmpD5=_tmpD4;struct _dyneither_ptr*_tmpD8;struct Cyc_Absyn_Tqual _tmpD7;void**_tmpD6;_LL18: _tmpD8=_tmpD5->f1;_tmpD7=_tmpD5->f2;_tmpD6=(void**)& _tmpD5->f3;_LL19:;
if(_tmpD2 != 0)
({void*_tmp799=Cyc_Parse_substitute_tags(_tmpD2,*_tmpD6);*_tmpD6=_tmp799;});
({void*_tmp79A=Cyc_Parse_array2ptr(*_tmpD6,1);*_tmpD6=_tmp79A;});}}
# 759
return({struct Cyc_Absyn_Tqual _tmp79D=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp79C=
Cyc_Absyn_function_type(typvars,_tmpDE,tq,t,_tmpE1,_tmpE0,_tmpDF,_tmpDD,fn_atts,_tmpDC,_tmpDB);
# 759
struct Cyc_List_List*_tmp79B=new_atts;Cyc_Parse_apply_tms(_tmp79D,_tmp79C,_tmp79B,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL10: _tmpE2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpCC)->f2;_LL11:
# 766
({void*_tmpD9=0U;({unsigned int _tmp79F=_tmpE2;struct _dyneither_ptr _tmp79E=({const char*_tmpDA="function declaration without parameter types";_tag_dyneither(_tmpDA,sizeof(char),45U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp79F,_tmp79E,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});}_LLD:;}case 4U: _LL7: _tmpEC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpEB=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL8:
# 773
 if(tms->tl == 0)
return({struct _tuple13 _tmp6A9;_tmp6A9.f1=tq,_tmp6A9.f2=t,_tmp6A9.f3=_tmpEC,_tmp6A9.f4=atts;_tmp6A9;});
# 778
({void*_tmpE3=0U;({unsigned int _tmp7A1=_tmpEB;struct _dyneither_ptr _tmp7A0=({const char*_tmpE4="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmpE4,sizeof(char),68U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7A1,_tmp7A0,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});case 2U: _LL9: _tmpEE=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpED=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLA:
# 781
 return({struct Cyc_Absyn_Tqual _tmp7A4=_tmpED;void*_tmp7A3=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6AA;_tmp6AA.elt_type=t,_tmp6AA.elt_tq=tq,_tmp6AA.ptr_atts=_tmpEE;_tmp6AA;}));struct Cyc_List_List*_tmp7A2=atts;Cyc_Parse_apply_tms(_tmp7A4,_tmp7A3,_tmp7A2,tms->tl);});default: _LLB: _tmpF0=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpEF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLC:
# 786
 return({struct Cyc_Absyn_Tqual _tmp7A7=tq;void*_tmp7A6=t;struct Cyc_List_List*_tmp7A5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmpEF);Cyc_Parse_apply_tms(_tmp7A7,_tmp7A6,_tmp7A5,tms->tl);});}_LL0:;};}
# 792
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 801
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmpF1=t;unsigned int _tmpFB;struct _tuple0*_tmpFA;struct Cyc_Absyn_Tqual _tmpF9;void*_tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF6;_LL1: _tmpFB=_tmpF1->f1;_tmpFA=_tmpF1->f2;_tmpF9=_tmpF1->f3;_tmpF8=_tmpF1->f4;_tmpF7=_tmpF1->f5;_tmpF6=_tmpF1->f6;_LL2:;
# 804
Cyc_Lex_register_typedef(_tmpFA);{
# 806
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpF2=_tmpF8;struct Cyc_Core_Opt*_tmpF3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF2)->tag == 1U){_LL4: _tmpF3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF2)->f1;_LL5:
# 810
 type=0;
if(_tmpF3 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmpF3;}
goto _LL3;}else{_LL6: _LL7:
 kind=0;type=_tmpF8;goto _LL3;}_LL3:;}
# 816
return({void*_tmp7A9=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7A8=({struct Cyc_Absyn_Typedefdecl*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->name=_tmpFA,_tmpF4->tvs=_tmpF7,_tmpF4->kind=kind,_tmpF4->defn=type,_tmpF4->atts=_tmpF6,_tmpF4->tq=_tmpF9,_tmpF4->extern_c=0;_tmpF4;});_tmpF5->f1=_tmp7A8;});_tmpF5;});Cyc_Absyn_new_decl(_tmp7A9,loc);});};}
# 823
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7AA=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=12U,_tmpFC->f1=d,_tmpFC->f2=s;_tmpFC;});Cyc_Absyn_new_stmt(_tmp7AA,d->loc);});}
# 829
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 838
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 842
struct _RegionHandle _tmpFD=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpFD;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpFE=ds;struct Cyc_Absyn_Tqual _tmp15F;struct Cyc_Parse_Type_specifier _tmp15E;struct Cyc_List_List*_tmp15D;_LL1: _tmp15F=_tmpFE.tq;_tmp15E=_tmpFE.type_specs;_tmp15D=_tmpFE.attributes;_LL2:;
if(_tmp15F.loc == (unsigned int)0)_tmp15F.loc=tqual_loc;
if(ds.is_inline)
({void*_tmpFF=0U;({unsigned int _tmp7AC=loc;struct _dyneither_ptr _tmp7AB=({const char*_tmp100="inline qualifier on non-function definition";_tag_dyneither(_tmp100,sizeof(char),44U);});Cyc_Warn_warn(_tmp7AC,_tmp7AB,_tag_dyneither(_tmpFF,sizeof(void*),0U));});});{
# 848
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp101=*ds.sc;enum Cyc_Parse_Storage_class _tmp102=_tmp101;switch(_tmp102){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 if(Cyc_Parse_no_register)s=2U;else{s=5U;}goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 goto _LL13;default: _LL12: _LL13:
 s=1U;goto _LL3;}_LL3:;}{
# 865
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 869
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 877
void*_tmp103=Cyc_Parse_collapse_type_specifiers(_tmp15E,loc);
if(declarators == 0){
# 881
void*_tmp104=_tmp103;struct Cyc_List_List*_tmp144;struct _tuple0*_tmp143;struct _tuple0*_tmp142;int _tmp141;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Datatypedecl**_tmp13F;enum Cyc_Absyn_AggrKind _tmp13E;struct _tuple0*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Datatypedecl*_tmp13B;struct Cyc_Absyn_Enumdecl*_tmp13A;struct Cyc_Absyn_Aggrdecl*_tmp139;switch(*((int*)_tmp104)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)){case 0U: _LL15: _tmp139=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL16:
# 883
({struct Cyc_List_List*_tmp7AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp139->attributes,_tmp15D);_tmp139->attributes=_tmp7AD;});
_tmp139->sc=s;{
struct Cyc_List_List*_tmp107=({struct Cyc_List_List*_tmp106=_cycalloc(sizeof(*_tmp106));({struct Cyc_Absyn_Decl*_tmp7AF=({void*_tmp7AE=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=5U,_tmp105->f1=_tmp139;_tmp105;});Cyc_Absyn_new_decl(_tmp7AE,loc);});_tmp106->hd=_tmp7AF;}),_tmp106->tl=0;_tmp106;});_npop_handler(0U);return _tmp107;};case 1U: _LL17: _tmp13A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL18:
# 887
 if(_tmp15D != 0)({void*_tmp108=0U;({unsigned int _tmp7B1=loc;struct _dyneither_ptr _tmp7B0=({const char*_tmp109="attributes on enum not supported";_tag_dyneither(_tmp109,sizeof(char),33U);});Cyc_Warn_err(_tmp7B1,_tmp7B0,_tag_dyneither(_tmp108,sizeof(void*),0U));});});
_tmp13A->sc=s;{
struct Cyc_List_List*_tmp10C=({struct Cyc_List_List*_tmp10B=_cycalloc(sizeof(*_tmp10B));({struct Cyc_Absyn_Decl*_tmp7B3=({void*_tmp7B2=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->tag=7U,_tmp10A->f1=_tmp13A;_tmp10A;});Cyc_Absyn_new_decl(_tmp7B2,loc);});_tmp10B->hd=_tmp7B3;}),_tmp10B->tl=0;_tmp10B;});_npop_handler(0U);return _tmp10C;};default: _LL19: _tmp13B=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL1A:
# 891
 if(_tmp15D != 0)({void*_tmp10D=0U;({unsigned int _tmp7B5=loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp10E="attributes on datatypes not supported";_tag_dyneither(_tmp10E,sizeof(char),38U);});Cyc_Warn_err(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp10D,sizeof(void*),0U));});});
_tmp13B->sc=s;{
struct Cyc_List_List*_tmp111=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Decl*_tmp7B7=({void*_tmp7B6=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=_tmp13B;_tmp10F;});Cyc_Absyn_new_decl(_tmp7B6,loc);});_tmp110->hd=_tmp7B7;}),_tmp110->tl=0;_tmp110;});_npop_handler(0U);return _tmp111;};}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp13E=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).val).f1;_tmp13D=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).val).f2;_tmp13C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f2;_LL1C: {
# 895
struct Cyc_List_List*_tmp112=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp13C);
struct Cyc_Absyn_Aggrdecl*_tmp113=({struct Cyc_Absyn_Aggrdecl*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->kind=_tmp13E,_tmp119->sc=s,_tmp119->name=_tmp13D,_tmp119->tvs=_tmp112,_tmp119->impl=0,_tmp119->attributes=0,_tmp119->expected_mem_kind=0;_tmp119;});
if(_tmp15D != 0)({void*_tmp114=0U;({unsigned int _tmp7B9=loc;struct _dyneither_ptr _tmp7B8=({const char*_tmp115="bad attributes on type declaration";_tag_dyneither(_tmp115,sizeof(char),35U);});Cyc_Warn_err(_tmp7B9,_tmp7B8,_tag_dyneither(_tmp114,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp118=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_Decl*_tmp7BB=({void*_tmp7BA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=5U,_tmp116->f1=_tmp113;_tmp116;});Cyc_Absyn_new_decl(_tmp7BA,loc);});_tmp117->hd=_tmp7BB;}),_tmp117->tl=0;_tmp117;});_npop_handler(0U);return _tmp118;};}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp13F=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).KnownDatatype).val;_LL1E:
# 900
 if(_tmp15D != 0)({void*_tmp11A=0U;({unsigned int _tmp7BD=loc;struct _dyneither_ptr _tmp7BC=({const char*_tmp11B="bad attributes on datatype";_tag_dyneither(_tmp11B,sizeof(char),27U);});Cyc_Warn_err(_tmp7BD,_tmp7BC,_tag_dyneither(_tmp11A,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp11E=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Decl*_tmp7BF=({void*_tmp7BE=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=6U,_tmp11C->f1=*_tmp13F;_tmp11C;});Cyc_Absyn_new_decl(_tmp7BE,loc);});_tmp11D->hd=_tmp7BF;}),_tmp11D->tl=0;_tmp11D;});_npop_handler(0U);return _tmp11E;};}else{_LL1F: _tmp142=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownDatatype).val).name;_tmp141=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp140=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f2;_LL20: {
# 903
struct Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp140);
struct Cyc_Absyn_Decl*_tmp120=Cyc_Absyn_datatype_decl(s,_tmp142,_tmp11F,0,_tmp141,loc);
if(_tmp15D != 0)({void*_tmp121=0U;({unsigned int _tmp7C1=loc;struct _dyneither_ptr _tmp7C0=({const char*_tmp122="bad attributes on datatype";_tag_dyneither(_tmp122,sizeof(char),27U);});Cyc_Warn_err(_tmp7C1,_tmp7C0,_tag_dyneither(_tmp121,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp124=({struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->hd=_tmp120,_tmp123->tl=0;_tmp123;});_npop_handler(0U);return _tmp124;};}}case 15U: _LL21: _tmp143=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1;_LL22: {
# 908
struct Cyc_Absyn_Enumdecl*_tmp125=({struct Cyc_Absyn_Enumdecl*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->sc=s,_tmp12C->name=_tmp143,_tmp12C->fields=0;_tmp12C;});
if(_tmp15D != 0)({void*_tmp126=0U;({unsigned int _tmp7C3=loc;struct _dyneither_ptr _tmp7C2=({const char*_tmp127="bad attributes on enum";_tag_dyneither(_tmp127,sizeof(char),23U);});Cyc_Warn_err(_tmp7C3,_tmp7C2,_tag_dyneither(_tmp126,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12B=({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));({struct Cyc_Absyn_Decl*_tmp7C5=({struct Cyc_Absyn_Decl*_tmp129=_cycalloc(sizeof(*_tmp129));({void*_tmp7C4=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=7U,_tmp128->f1=_tmp125;_tmp128;});_tmp129->r=_tmp7C4;}),_tmp129->loc=loc;_tmp129;});_tmp12A->hd=_tmp7C5;}),_tmp12A->tl=0;_tmp12A;});_npop_handler(0U);return _tmp12B;};}case 16U: _LL23: _tmp144=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1;_LL24: {
# 914
struct Cyc_Absyn_Enumdecl*_tmp12D=({struct Cyc_Absyn_Enumdecl*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->sc=s,({struct _tuple0*_tmp7C7=Cyc_Parse_gensym_enum();_tmp135->name=_tmp7C7;}),({struct Cyc_Core_Opt*_tmp7C6=({struct Cyc_Core_Opt*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->v=_tmp144;_tmp134;});_tmp135->fields=_tmp7C6;});_tmp135;});
if(_tmp15D != 0)({void*_tmp12E=0U;({unsigned int _tmp7C9=loc;struct _dyneither_ptr _tmp7C8=({const char*_tmp12F="bad attributes on enum";_tag_dyneither(_tmp12F,sizeof(char),23U);});Cyc_Warn_err(_tmp7C9,_tmp7C8,_tag_dyneither(_tmp12E,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));({struct Cyc_Absyn_Decl*_tmp7CB=({struct Cyc_Absyn_Decl*_tmp131=_cycalloc(sizeof(*_tmp131));({void*_tmp7CA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=7U,_tmp130->f1=_tmp12D;_tmp130;});_tmp131->r=_tmp7CA;}),_tmp131->loc=loc;_tmp131;});_tmp132->hd=_tmp7CB;}),_tmp132->tl=0;_tmp132;});_npop_handler(0U);return _tmp133;};}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp136=0U;({unsigned int _tmp7CD=loc;struct _dyneither_ptr _tmp7CC=({const char*_tmp137="missing declarator";_tag_dyneither(_tmp137,sizeof(char),19U);});Cyc_Warn_err(_tmp7CD,_tmp7CC,_tag_dyneither(_tmp136,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp138=0;_npop_handler(0U);return _tmp138;};}_LL14:;}else{
# 921
struct Cyc_List_List*_tmp145=Cyc_Parse_apply_tmss(mkrgn,_tmp15F,_tmp103,declarators,_tmp15D);
if(istypedef){
# 926
if(!exps_empty)
({void*_tmp146=0U;({unsigned int _tmp7CF=loc;struct _dyneither_ptr _tmp7CE=({const char*_tmp147="initializer in typedef declaration";_tag_dyneither(_tmp147,sizeof(char),35U);});Cyc_Warn_err(_tmp7CF,_tmp7CE,_tag_dyneither(_tmp146,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp145);
struct Cyc_List_List*_tmp148=decls;_npop_handler(0U);return _tmp148;};}else{
# 932
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp149=_tmp145;for(0;_tmp149 != 0;(_tmp149=_tmp149->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp14A=(struct _tuple15*)_tmp149->hd;struct _tuple15*_tmp14B=_tmp14A;unsigned int _tmp15B;struct _tuple0*_tmp15A;struct Cyc_Absyn_Tqual _tmp159;void*_tmp158;struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp156;_LL28: _tmp15B=_tmp14B->f1;_tmp15A=_tmp14B->f2;_tmp159=_tmp14B->f3;_tmp158=_tmp14B->f4;_tmp157=_tmp14B->f5;_tmp156=_tmp14B->f6;_LL29:;
if(_tmp157 != 0)
({void*_tmp14C=0U;({unsigned int _tmp7D1=loc;struct _dyneither_ptr _tmp7D0=({const char*_tmp14D="bad type params, ignoring";_tag_dyneither(_tmp14D,sizeof(char),26U);});Cyc_Warn_warn(_tmp7D1,_tmp7D0,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp14E=0U;({unsigned int _tmp7D3=loc;struct _dyneither_ptr _tmp7D2=({const char*_tmp14F="unexpected NULL in parse!";_tag_dyneither(_tmp14F,sizeof(char),26U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7D3,_tmp7D2,_tag_dyneither(_tmp14E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp150=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp151=Cyc_Absyn_new_vardecl(_tmp15B,_tmp15A,_tmp158,_tmp150);
_tmp151->tq=_tmp159;
_tmp151->sc=s;
_tmp151->attributes=_tmp156;{
struct Cyc_Absyn_Decl*_tmp152=({struct Cyc_Absyn_Decl*_tmp155=_cycalloc(sizeof(*_tmp155));({void*_tmp7D4=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->tag=0U,_tmp154->f1=_tmp151;_tmp154;});_tmp155->r=_tmp7D4;}),_tmp155->loc=loc;_tmp155;});
decls=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=_tmp152,_tmp153->tl=decls;_tmp153;});};};}}{
# 947
struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp15C;};}}};};};};}
# 843
;_pop_region(mkrgn);}
# 953
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== (unsigned long)1  || Cyc_strlen((struct _dyneither_ptr)s)== (unsigned long)2){
char _tmp160=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp161=_tmp160;switch(_tmp161){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 963
{char _tmp162=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp163=_tmp162;switch(_tmp163){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 970
goto _LL0;case 84U: _LLF: _LL10:
# 972
{char _tmp164=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp165=_tmp164;switch(_tmp165){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 979
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 982
({struct Cyc_String_pa_PrintArg_struct _tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp6AC;_tmp6AC.tag=0U,_tmp6AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp6AC;});struct Cyc_Int_pa_PrintArg_struct _tmp169=({struct Cyc_Int_pa_PrintArg_struct _tmp6AB;_tmp6AB.tag=1U,({unsigned long _tmp7D5=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp6AB.f1=_tmp7D5;});_tmp6AB;});void*_tmp166[2U];_tmp166[0]=& _tmp168,_tmp166[1]=& _tmp169;({unsigned int _tmp7D7=loc;struct _dyneither_ptr _tmp7D6=({const char*_tmp167="bad kind: %s; strlen=%d";_tag_dyneither(_tmp167,sizeof(char),24U);});Cyc_Warn_err(_tmp7D7,_tmp7D6,_tag_dyneither(_tmp166,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 987
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp16A=e->r;void*_tmp16B=_tmp16A;int _tmp16E;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).tag == 5){_LL1: _tmp16E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).val).f2;_LL2:
 return _tmp16E;}else{goto _LL3;}}else{_LL3: _LL4:
# 991
({void*_tmp16C=0U;({unsigned int _tmp7D9=loc;struct _dyneither_ptr _tmp7D8=({const char*_tmp16D="expecting integer constant";_tag_dyneither(_tmp16D,sizeof(char),27U);});Cyc_Warn_err(_tmp7D9,_tmp7D8,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 997
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp16F=e->r;void*_tmp170=_tmp16F;struct _dyneither_ptr _tmp173;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).tag == 8){_LL1: _tmp173=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).val;_LL2:
 return _tmp173;}else{goto _LL3;}}else{_LL3: _LL4:
# 1001
({void*_tmp171=0U;({unsigned int _tmp7DB=loc;struct _dyneither_ptr _tmp7DA=({const char*_tmp172="expecting string constant";_tag_dyneither(_tmp172,sizeof(char),26U);});Cyc_Warn_err(_tmp7DB,_tmp7DA,_tag_dyneither(_tmp171,sizeof(void*),0U));});});
return _tag_dyneither(0,0,0);}_LL0:;}
# 1007
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp174=x;long long _tmp17C;char _tmp17B;int _tmp17A;switch((_tmp174.LongLong_c).tag){case 5U: _LL1: _tmp17A=((_tmp174.Int_c).val).f2;_LL2:
 return(unsigned int)_tmp17A;case 2U: _LL3: _tmp17B=((_tmp174.Char_c).val).f2;_LL4:
 return(unsigned int)_tmp17B;case 6U: _LL5: _tmp17C=((_tmp174.LongLong_c).val).f2;_LL6: {
# 1012
unsigned long long y=(unsigned long long)_tmp17C;
if(y > (unsigned long long)-1)
({void*_tmp175=0U;({unsigned int _tmp7DD=loc;struct _dyneither_ptr _tmp7DC=({const char*_tmp176="integer constant too large";_tag_dyneither(_tmp176,sizeof(char),27U);});Cyc_Warn_err(_tmp7DD,_tmp7DC,_tag_dyneither(_tmp175,sizeof(void*),0U));});});
return(unsigned int)_tmp17C;}default: _LL7: _LL8:
# 1017
({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp6AD;_tmp6AD.tag=0U,({struct _dyneither_ptr _tmp7DE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp6AD.f1=_tmp7DE;});_tmp6AD;});void*_tmp177[1U];_tmp177[0]=& _tmp179;({unsigned int _tmp7E0=loc;struct _dyneither_ptr _tmp7DF=({const char*_tmp178="expected integer constant but found %s";_tag_dyneither(_tmp178,sizeof(char),39U);});Cyc_Warn_err(_tmp7E0,_tmp7DF,_tag_dyneither(_tmp177,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17D=p->r;void*_tmp17E=_tmp17D;struct Cyc_Absyn_Exp*_tmp18B;struct _tuple0*_tmp18A;struct Cyc_List_List*_tmp189;struct _dyneither_ptr _tmp188;int _tmp187;char _tmp186;enum Cyc_Absyn_Sign _tmp185;int _tmp184;struct Cyc_Absyn_Pat*_tmp183;struct Cyc_Absyn_Vardecl*_tmp182;struct _tuple0*_tmp181;switch(*((int*)_tmp17E)){case 15U: _LL1: _tmp181=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL2:
 return Cyc_Absyn_unknownid_exp(_tmp181,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f2)->r)->tag == 0U){_LL3: _tmp182=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL4:
# 1027
 return({struct Cyc_Absyn_Exp*_tmp7E1=Cyc_Absyn_unknownid_exp(_tmp182->name,p->loc);Cyc_Absyn_deref_exp(_tmp7E1,p->loc);});}else{goto _LL13;}case 6U: _LL5: _tmp183=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL6:
 return({struct Cyc_Absyn_Exp*_tmp7E2=Cyc_Parse_pat2exp(_tmp183);Cyc_Absyn_address_exp(_tmp7E2,p->loc);});case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp185=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp184=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLA:
 return Cyc_Absyn_int_exp(_tmp185,_tmp184,p->loc);case 11U: _LLB: _tmp186=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LLC:
 return Cyc_Absyn_char_exp(_tmp186,p->loc);case 12U: _LLD: _tmp188=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp187=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLE:
 return Cyc_Absyn_float_exp(_tmp188,_tmp187,p->loc);case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f3 == 0){_LLF: _tmp18A=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp189=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LL10: {
# 1034
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp18A,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp189);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp18B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL12:
 return _tmp18B;default: _LL13: _LL14:
# 1039
({void*_tmp17F=0U;({unsigned int _tmp7E4=p->loc;struct _dyneither_ptr _tmp7E3=({const char*_tmp180="cannot mix patterns and expressions in case";_tag_dyneither(_tmp180,sizeof(char),44U);});Cyc_Warn_err(_tmp7E4,_tmp7E3,_tag_dyneither(_tmp17F,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp18C[7U]="cnst_t";
# 1096 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18C,_tmp18C,_tmp18C + 7U}};
# 1098
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp18D=yy1;union Cyc_Absyn_Cnst _tmp18E;if((_tmp18D.Int_tok).tag == 1){_LL1: _tmp18E=(_tmp18D.Int_tok).val;_LL2:
# 1102
 yyzzz=_tmp18E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL0:;}
# 1106
return yyzzz;}
# 1108
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6AE;(_tmp6AE.Int_tok).tag=1U,(_tmp6AE.Int_tok).val=yy1;_tmp6AE;});}static char _tmp18F[5U]="char";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp18F,_tmp18F,_tmp18F + 5U}};
# 1099
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp190=yy1;char _tmp191;if((_tmp190.Char_tok).tag == 2){_LL1: _tmp191=(_tmp190.Char_tok).val;_LL2:
# 1103
 yyzzz=_tmp191;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL0:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6AF;(_tmp6AF.Char_tok).tag=2U,(_tmp6AF.Char_tok).val=yy1;_tmp6AF;});}static char _tmp192[13U]="string_t<`H>";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp192,_tmp192,_tmp192 + 13U}};
# 1100
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp193=yy1;struct _dyneither_ptr _tmp194;if((_tmp193.String_tok).tag == 3){_LL1: _tmp194=(_tmp193.String_tok).val;_LL2:
# 1104
 yyzzz=_tmp194;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL0:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp6B0;(_tmp6B0.String_tok).tag=3U,(_tmp6B0.String_tok).val=yy1;_tmp6B0;});}static char _tmp195[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1101 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp195,_tmp195,_tmp195 + 45U}};
# 1103
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp196=yy1;struct _tuple21*_tmp197;if((_tmp196.YY1).tag == 9){_LL1: _tmp197=(_tmp196.YY1).val;_LL2:
# 1107
 yyzzz=_tmp197;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL0:;}
# 1111
return yyzzz;}
# 1113
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp6B1;(_tmp6B1.YY1).tag=9U,(_tmp6B1.YY1).val=yy1;_tmp6B1;});}static char _tmp198[11U]="ptrbound_t";
# 1102 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp198,_tmp198,_tmp198 + 11U}};
# 1104
void*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp199=yy1;void*_tmp19A;if((_tmp199.YY2).tag == 10){_LL1: _tmp19A=(_tmp199.YY2).val;_LL2:
# 1108
 yyzzz=_tmp19A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL0:;}
# 1112
return yyzzz;}
# 1114
union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6B2;(_tmp6B2.YY2).tag=10U,(_tmp6B2.YY2).val=yy1;_tmp6B2;});}static char _tmp19B[28U]="list_t<offsetof_field_t,`H>";
# 1103 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp19B,_tmp19B,_tmp19B + 28U}};
# 1105
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp19C=yy1;struct Cyc_List_List*_tmp19D;if((_tmp19C.YY3).tag == 11){_LL1: _tmp19D=(_tmp19C.YY3).val;_LL2:
# 1109
 yyzzz=_tmp19D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL0:;}
# 1113
return yyzzz;}
# 1115
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B3;(_tmp6B3.YY3).tag=11U,(_tmp6B3.YY3).val=yy1;_tmp6B3;});}static char _tmp19E[6U]="exp_t";
# 1104 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp19E,_tmp19E,_tmp19E + 6U}};
# 1106
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp19F=yy1;struct Cyc_Absyn_Exp*_tmp1A0;if((_tmp19F.Exp_tok).tag == 7){_LL1: _tmp1A0=(_tmp19F.Exp_tok).val;_LL2:
# 1110
 yyzzz=_tmp1A0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL0:;}
# 1114
return yyzzz;}
# 1116
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6B4;(_tmp6B4.Exp_tok).tag=7U,(_tmp6B4.Exp_tok).val=yy1;_tmp6B4;});}static char _tmp1A1[17U]="list_t<exp_t,`H>";
# 1112 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A1,_tmp1A1,_tmp1A1 + 17U}};
# 1114
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1A2=yy1;struct Cyc_List_List*_tmp1A3;if((_tmp1A2.YY4).tag == 12){_LL1: _tmp1A3=(_tmp1A2.YY4).val;_LL2:
# 1118
 yyzzz=_tmp1A3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL0:;}
# 1122
return yyzzz;}
# 1124
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B5;(_tmp6B5.YY4).tag=12U,(_tmp6B5.YY4).val=yy1;_tmp6B5;});}static char _tmp1A4[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1A4,_tmp1A4,_tmp1A4 + 47U}};
# 1115
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1A5=yy1;struct Cyc_List_List*_tmp1A6;if((_tmp1A5.YY5).tag == 13){_LL1: _tmp1A6=(_tmp1A5.YY5).val;_LL2:
# 1119
 yyzzz=_tmp1A6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL0:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B6;(_tmp6B6.YY5).tag=13U,(_tmp6B6.YY5).val=yy1;_tmp6B6;});}static char _tmp1A7[9U]="primop_t";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1A7,_tmp1A7,_tmp1A7 + 9U}};
# 1116
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1A8=yy1;enum Cyc_Absyn_Primop _tmp1A9;if((_tmp1A8.YY6).tag == 14){_LL1: _tmp1A9=(_tmp1A8.YY6).val;_LL2:
# 1120
 yyzzz=_tmp1A9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL0:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6B7;(_tmp6B7.YY6).tag=14U,(_tmp6B7.YY6).val=yy1;_tmp6B7;});}static char _tmp1AA[19U]="opt_t<primop_t,`H>";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 19U}};
# 1117
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1AB=yy1;struct Cyc_Core_Opt*_tmp1AC;if((_tmp1AB.YY7).tag == 15){_LL1: _tmp1AC=(_tmp1AB.YY7).val;_LL2:
# 1121
 yyzzz=_tmp1AC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL0:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6B8;(_tmp6B8.YY7).tag=15U,(_tmp6B8.YY7).val=yy1;_tmp6B8;});}static char _tmp1AD[7U]="qvar_t";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1AD,_tmp1AD,_tmp1AD + 7U}};
# 1118
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1AE=yy1;struct _tuple0*_tmp1AF;if((_tmp1AE.QualId_tok).tag == 5){_LL1: _tmp1AF=(_tmp1AE.QualId_tok).val;_LL2:
# 1122
 yyzzz=_tmp1AF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL0:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6B9;(_tmp6B9.QualId_tok).tag=5U,(_tmp6B9.QualId_tok).val=yy1;_tmp6B9;});}static char _tmp1B0[7U]="stmt_t";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp1B0,_tmp1B0,_tmp1B0 + 7U}};
# 1121
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp1B1=yy1;struct Cyc_Absyn_Stmt*_tmp1B2;if((_tmp1B1.Stmt_tok).tag == 8){_LL1: _tmp1B2=(_tmp1B1.Stmt_tok).val;_LL2:
# 1125
 yyzzz=_tmp1B2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL0:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6BA;(_tmp6BA.Stmt_tok).tag=8U,(_tmp6BA.Stmt_tok).val=yy1;_tmp6BA;});}static char _tmp1B3[27U]="list_t<switch_clause_t,`H>";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 27U}};
# 1125
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1B4=yy1;struct Cyc_List_List*_tmp1B5;if((_tmp1B4.YY8).tag == 16){_LL1: _tmp1B5=(_tmp1B4.YY8).val;_LL2:
# 1129
 yyzzz=_tmp1B5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL0:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BB;(_tmp6BB.YY8).tag=16U,(_tmp6BB.YY8).val=yy1;_tmp6BB;});}static char _tmp1B6[6U]="pat_t";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1B6,_tmp1B6,_tmp1B6 + 6U}};
# 1126
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp1B7=yy1;struct Cyc_Absyn_Pat*_tmp1B8;if((_tmp1B7.YY9).tag == 17){_LL1: _tmp1B8=(_tmp1B7.YY9).val;_LL2:
# 1130
 yyzzz=_tmp1B8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL0:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6BC;(_tmp6BC.YY9).tag=17U,(_tmp6BC.YY9).val=yy1;_tmp6BC;});}static char _tmp1B9[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1B9,_tmp1B9,_tmp1B9 + 28U}};
# 1131
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1BA=yy1;struct _tuple22*_tmp1BB;if((_tmp1BA.YY10).tag == 18){_LL1: _tmp1BB=(_tmp1BA.YY10).val;_LL2:
# 1135
 yyzzz=_tmp1BB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL0:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6BD;(_tmp6BD.YY10).tag=18U,(_tmp6BD.YY10).val=yy1;_tmp6BD;});}static char _tmp1BC[17U]="list_t<pat_t,`H>";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1BC,_tmp1BC,_tmp1BC + 17U}};
# 1132
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1BD=yy1;struct Cyc_List_List*_tmp1BE;if((_tmp1BD.YY11).tag == 19){_LL1: _tmp1BE=(_tmp1BD.YY11).val;_LL2:
# 1136
 yyzzz=_tmp1BE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL0:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BE;(_tmp6BE.YY11).tag=19U,(_tmp6BE.YY11).val=yy1;_tmp6BE;});}static char _tmp1BF[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1BF,_tmp1BF,_tmp1BF + 36U}};
# 1133
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp1C0=yy1;struct _tuple23*_tmp1C1;if((_tmp1C0.YY12).tag == 20){_LL1: _tmp1C1=(_tmp1C0.YY12).val;_LL2:
# 1137
 yyzzz=_tmp1C1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL0:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6BF;(_tmp6BF.YY12).tag=20U,(_tmp6BF.YY12).val=yy1;_tmp6BF;});}static char _tmp1C2[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 47U}};
# 1134
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1C3=yy1;struct Cyc_List_List*_tmp1C4;if((_tmp1C3.YY13).tag == 21){_LL1: _tmp1C4=(_tmp1C3.YY13).val;_LL2:
# 1138
 yyzzz=_tmp1C4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL0:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C0;(_tmp6C0.YY13).tag=21U,(_tmp6C0.YY13).val=yy1;_tmp6C0;});}static char _tmp1C5[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 58U}};
# 1135
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1C6=yy1;struct _tuple22*_tmp1C7;if((_tmp1C6.YY14).tag == 22){_LL1: _tmp1C7=(_tmp1C6.YY14).val;_LL2:
# 1139
 yyzzz=_tmp1C7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL0:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6C1;(_tmp6C1.YY14).tag=22U,(_tmp6C1.YY14).val=yy1;_tmp6C1;});}static char _tmp1C8[9U]="fndecl_t";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1C8,_tmp1C8,_tmp1C8 + 9U}};
# 1136
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp1C9=yy1;struct Cyc_Absyn_Fndecl*_tmp1CA;if((_tmp1C9.YY15).tag == 23){_LL1: _tmp1CA=(_tmp1C9.YY15).val;_LL2:
# 1140
 yyzzz=_tmp1CA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL0:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6C2;(_tmp6C2.YY15).tag=23U,(_tmp6C2.YY15).val=yy1;_tmp6C2;});}static char _tmp1CB[18U]="list_t<decl_t,`H>";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 18U}};
# 1137
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;if((_tmp1CC.YY16).tag == 24){_LL1: _tmp1CD=(_tmp1CC.YY16).val;_LL2:
# 1141
 yyzzz=_tmp1CD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL0:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C3;(_tmp6C3.YY16).tag=24U,(_tmp6C3.YY16).val=yy1;_tmp6C3;});}static char _tmp1CE[12U]="decl_spec_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 12U}};
# 1140
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp1CF=yy1;struct Cyc_Parse_Declaration_spec _tmp1D0;if((_tmp1CF.YY17).tag == 25){_LL1: _tmp1D0=(_tmp1CF.YY17).val;_LL2:
# 1144
 yyzzz=_tmp1D0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL0:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6C4;(_tmp6C4.YY17).tag=25U,(_tmp6C4.YY17).val=yy1;_tmp6C4;});}static char _tmp1D1[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1D1,_tmp1D1,_tmp1D1 + 31U}};
# 1141
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp1D2=yy1;struct _tuple11 _tmp1D3;if((_tmp1D2.YY18).tag == 26){_LL1: _tmp1D3=(_tmp1D2.YY18).val;_LL2:
# 1145
 yyzzz=_tmp1D3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL0:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp6C5;(_tmp6C5.YY18).tag=26U,(_tmp6C5.YY18).val=yy1;_tmp6C5;});}static char _tmp1D4[23U]="declarator_list_t<`yy>";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,_tmp1D4 + 23U}};
# 1142
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp1D5=yy1;struct _tuple12*_tmp1D6;if((_tmp1D5.YY19).tag == 27){_LL1: _tmp1D6=(_tmp1D5.YY19).val;_LL2:
# 1146
 yyzzz=_tmp1D6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL0:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp6C6;(_tmp6C6.YY19).tag=27U,(_tmp6C6.YY19).val=yy1;_tmp6C6;});}static char _tmp1D7[19U]="storage_class_t@`H";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1D7,_tmp1D7,_tmp1D7 + 19U}};
# 1143
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp1D8=yy1;enum Cyc_Parse_Storage_class*_tmp1D9;if((_tmp1D8.YY20).tag == 28){_LL1: _tmp1D9=(_tmp1D8.YY20).val;_LL2:
# 1147
 yyzzz=_tmp1D9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL0:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6C7;(_tmp6C7.YY20).tag=28U,(_tmp6C7.YY20).val=yy1;_tmp6C7;});}static char _tmp1DA[17U]="type_specifier_t";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 17U}};
# 1144
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp1DB=yy1;struct Cyc_Parse_Type_specifier _tmp1DC;if((_tmp1DB.YY21).tag == 29){_LL1: _tmp1DC=(_tmp1DB.YY21).val;_LL2:
# 1148
 yyzzz=_tmp1DC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL0:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6C8;(_tmp6C8.YY21).tag=29U,(_tmp6C8.YY21).val=yy1;_tmp6C8;});}static char _tmp1DD[12U]="aggr_kind_t";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1DD,_tmp1DD,_tmp1DD + 12U}};
# 1146
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp1DE=yy1;enum Cyc_Absyn_AggrKind _tmp1DF;if((_tmp1DE.YY22).tag == 30){_LL1: _tmp1DF=(_tmp1DE.YY22).val;_LL2:
# 1150
 yyzzz=_tmp1DF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL0:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6C9;(_tmp6C9.YY22).tag=30U,(_tmp6C9.YY22).val=yy1;_tmp6C9;});}static char _tmp1E0[8U]="tqual_t";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1E0,_tmp1E0,_tmp1E0 + 8U}};
# 1147
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp1E1=yy1;struct Cyc_Absyn_Tqual _tmp1E2;if((_tmp1E1.YY23).tag == 31){_LL1: _tmp1E2=(_tmp1E1.YY23).val;_LL2:
# 1151
 yyzzz=_tmp1E2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL0:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6CA;(_tmp6CA.YY23).tag=31U,(_tmp6CA.YY23).val=yy1;_tmp6CA;});}static char _tmp1E3[23U]="list_t<aggrfield_t,`H>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 23U}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E4=yy1;struct Cyc_List_List*_tmp1E5;if((_tmp1E4.YY24).tag == 32){_LL1: _tmp1E5=(_tmp1E4.YY24).val;_LL2:
# 1152
 yyzzz=_tmp1E5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL0:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CB;(_tmp6CB.YY24).tag=32U,(_tmp6CB.YY24).val=yy1;_tmp6CB;});}static char _tmp1E6[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1E6,_tmp1E6,_tmp1E6 + 34U}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;if((_tmp1E7.YY25).tag == 33){_LL1: _tmp1E8=(_tmp1E7.YY25).val;_LL2:
# 1153
 yyzzz=_tmp1E8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL0:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CC;(_tmp6CC.YY25).tag=33U,(_tmp6CC.YY25).val=yy1;_tmp6CC;});}static char _tmp1E9[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1E9,_tmp1E9,_tmp1E9 + 33U}};
# 1150
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EA=yy1;struct Cyc_List_List*_tmp1EB;if((_tmp1EA.YY26).tag == 34){_LL1: _tmp1EB=(_tmp1EA.YY26).val;_LL2:
# 1154
 yyzzz=_tmp1EB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL0:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CD;(_tmp6CD.YY26).tag=34U,(_tmp6CD.YY26).val=yy1;_tmp6CD;});}static char _tmp1EC[18U]="declarator_t<`yy>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp1EC,_tmp1EC,_tmp1EC + 18U}};
# 1151
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp1ED=yy1;struct Cyc_Parse_Declarator _tmp1EE;if((_tmp1ED.YY27).tag == 35){_LL1: _tmp1EE=(_tmp1ED.YY27).val;_LL2:
# 1155
 yyzzz=_tmp1EE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL0:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6CE;(_tmp6CE.YY27).tag=35U,(_tmp6CE.YY27).val=yy1;_tmp6CE;});}static char _tmp1EF[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 45U}};
# 1152
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp1F0=yy1;struct _tuple24*_tmp1F1;if((_tmp1F0.YY28).tag == 36){_LL1: _tmp1F1=(_tmp1F0.YY28).val;_LL2:
# 1156
 yyzzz=_tmp1F1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL0:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6CF;(_tmp6CF.YY28).tag=36U,(_tmp6CF.YY28).val=yy1;_tmp6CF;});}static char _tmp1F2[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 57U}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;struct Cyc_List_List*_tmp1F4;if((_tmp1F3.YY29).tag == 37){_LL1: _tmp1F4=(_tmp1F3.YY29).val;_LL2:
# 1157
 yyzzz=_tmp1F4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL0:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D0;(_tmp6D0.YY29).tag=37U,(_tmp6D0.YY29).val=yy1;_tmp6D0;});}static char _tmp1F5[26U]="abstractdeclarator_t<`yy>";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp1F5,_tmp1F5,_tmp1F5 + 26U}};
# 1154
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp1F6=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F7;if((_tmp1F6.YY30).tag == 38){_LL1: _tmp1F7=(_tmp1F6.YY30).val;_LL2:
# 1158
 yyzzz=_tmp1F7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL0:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6D1;(_tmp6D1.YY30).tag=38U,(_tmp6D1.YY30).val=yy1;_tmp6D1;});}static char _tmp1F8[5U]="bool";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 5U}};
# 1155
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp1F9=yy1;int _tmp1FA;if((_tmp1F9.YY31).tag == 39){_LL1: _tmp1FA=(_tmp1F9.YY31).val;_LL2:
# 1159
 yyzzz=_tmp1FA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL0:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6D2;(_tmp6D2.YY31).tag=39U,(_tmp6D2.YY31).val=yy1;_tmp6D2;});}static char _tmp1FB[8U]="scope_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp1FB,_tmp1FB,_tmp1FB + 8U}};
# 1156
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp1FC=yy1;enum Cyc_Absyn_Scope _tmp1FD;if((_tmp1FC.YY32).tag == 40){_LL1: _tmp1FD=(_tmp1FC.YY32).val;_LL2:
# 1160
 yyzzz=_tmp1FD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL0:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6D3;(_tmp6D3.YY32).tag=40U,(_tmp6D3.YY32).val=yy1;_tmp6D3;});}static char _tmp1FE[16U]="datatypefield_t";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp1FE,_tmp1FE,_tmp1FE + 16U}};
# 1157
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp1FF=yy1;struct Cyc_Absyn_Datatypefield*_tmp200;if((_tmp1FF.YY33).tag == 41){_LL1: _tmp200=(_tmp1FF.YY33).val;_LL2:
# 1161
 yyzzz=_tmp200;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL0:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6D4;(_tmp6D4.YY33).tag=41U,(_tmp6D4.YY33).val=yy1;_tmp6D4;});}static char _tmp201[27U]="list_t<datatypefield_t,`H>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 27U}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_List_List*_tmp203;if((_tmp202.YY34).tag == 42){_LL1: _tmp203=(_tmp202.YY34).val;_LL2:
# 1162
 yyzzz=_tmp203;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL0:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D5;(_tmp6D5.YY34).tag=42U,(_tmp6D5.YY34).val=yy1;_tmp6D5;});}static char _tmp204[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp204,_tmp204,_tmp204 + 41U}};
# 1159
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp205=yy1;struct _tuple25 _tmp206;if((_tmp205.YY35).tag == 43){_LL1: _tmp206=(_tmp205.YY35).val;_LL2:
# 1163
 yyzzz=_tmp206;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL0:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp6D6;(_tmp6D6.YY35).tag=43U,(_tmp6D6.YY35).val=yy1;_tmp6D6;});}static char _tmp207[17U]="list_t<var_t,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 17U}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp208=yy1;struct Cyc_List_List*_tmp209;if((_tmp208.YY36).tag == 44){_LL1: _tmp209=(_tmp208.YY36).val;_LL2:
# 1164
 yyzzz=_tmp209;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL0:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D7;(_tmp6D7.YY36).tag=44U,(_tmp6D7.YY36).val=yy1;_tmp6D7;});}static char _tmp20A[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 31U}};
# 1161
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp20B=yy1;struct _tuple8*_tmp20C;if((_tmp20B.YY37).tag == 45){_LL1: _tmp20C=(_tmp20B.YY37).val;_LL2:
# 1165
 yyzzz=_tmp20C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL0:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.YY37).tag=45U,(_tmp6D8.YY37).val=yy1;_tmp6D8;});}static char _tmp20D[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 42U}};
# 1162
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp20E=yy1;struct Cyc_List_List*_tmp20F;if((_tmp20E.YY38).tag == 46){_LL1: _tmp20F=(_tmp20E.YY38).val;_LL2:
# 1166
 yyzzz=_tmp20F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL0:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.YY38).tag=46U,(_tmp6D9.YY38).val=yy1;_tmp6D9;});}static char _tmp210[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 115U}};
# 1163
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct _tuple26*_tmp212;if((_tmp211.YY39).tag == 47){_LL1: _tmp212=(_tmp211.YY39).val;_LL2:
# 1167
 yyzzz=_tmp212;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL0:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.YY39).tag=47U,(_tmp6DA.YY39).val=yy1;_tmp6DA;});}static char _tmp213[8U]="types_t";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp213,_tmp213,_tmp213 + 8U}};
# 1164
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp214=yy1;struct Cyc_List_List*_tmp215;if((_tmp214.YY40).tag == 48){_LL1: _tmp215=(_tmp214.YY40).val;_LL2:
# 1168
 yyzzz=_tmp215;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL0:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.YY40).tag=48U,(_tmp6DB.YY40).val=yy1;_tmp6DB;});}static char _tmp216[24U]="list_t<designator_t,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 24U}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp217=yy1;struct Cyc_List_List*_tmp218;if((_tmp217.YY41).tag == 49){_LL1: _tmp218=(_tmp217.YY41).val;_LL2:
# 1170
 yyzzz=_tmp218;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL0:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.YY41).tag=49U,(_tmp6DC.YY41).val=yy1;_tmp6DC;});}static char _tmp219[13U]="designator_t";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp219,_tmp219,_tmp219 + 13U}};
# 1167
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp21A=yy1;void*_tmp21B;if((_tmp21A.YY42).tag == 50){_LL1: _tmp21B=(_tmp21A.YY42).val;_LL2:
# 1171
 yyzzz=_tmp21B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL0:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.YY42).tag=50U,(_tmp6DD.YY42).val=yy1;_tmp6DD;});}static char _tmp21C[7U]="kind_t";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp21C,_tmp21C,_tmp21C + 7U}};
# 1168
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp21D=yy1;struct Cyc_Absyn_Kind*_tmp21E;if((_tmp21D.YY43).tag == 51){_LL1: _tmp21E=(_tmp21D.YY43).val;_LL2:
# 1172
 yyzzz=_tmp21E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL0:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY43).tag=51U,(_tmp6DE.YY43).val=yy1;_tmp6DE;});}static char _tmp21F[7U]="type_t";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 7U}};
# 1169
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;void*_tmp221;if((_tmp220.YY44).tag == 52){_LL1: _tmp221=(_tmp220.YY44).val;_LL2:
# 1173
 yyzzz=_tmp221;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL0:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY44).tag=52U,(_tmp6DF.YY44).val=yy1;_tmp6DF;});}static char _tmp222[23U]="list_t<attribute_t,`H>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp222,_tmp222,_tmp222 + 23U}};
# 1170
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp223=yy1;struct Cyc_List_List*_tmp224;if((_tmp223.YY45).tag == 53){_LL1: _tmp224=(_tmp223.YY45).val;_LL2:
# 1174
 yyzzz=_tmp224;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL0:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY45).tag=53U,(_tmp6E0.YY45).val=yy1;_tmp6E0;});}static char _tmp225[12U]="attribute_t";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp225,_tmp225,_tmp225 + 12U}};
# 1171
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp226=yy1;void*_tmp227;if((_tmp226.YY46).tag == 54){_LL1: _tmp227=(_tmp226.YY46).val;_LL2:
# 1175
 yyzzz=_tmp227;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL0:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY46).tag=54U,(_tmp6E1.YY46).val=yy1;_tmp6E1;});}static char _tmp228[12U]="enumfield_t";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp228,_tmp228,_tmp228 + 12U}};
# 1172
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp229=yy1;struct Cyc_Absyn_Enumfield*_tmp22A;if((_tmp229.YY47).tag == 55){_LL1: _tmp22A=(_tmp229.YY47).val;_LL2:
# 1176
 yyzzz=_tmp22A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL0:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY47).tag=55U,(_tmp6E2.YY47).val=yy1;_tmp6E2;});}static char _tmp22B[23U]="list_t<enumfield_t,`H>";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp22B,_tmp22B,_tmp22B + 23U}};
# 1173
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22C=yy1;struct Cyc_List_List*_tmp22D;if((_tmp22C.YY48).tag == 56){_LL1: _tmp22D=(_tmp22C.YY48).val;_LL2:
# 1177
 yyzzz=_tmp22D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL0:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY48).tag=56U,(_tmp6E3.YY48).val=yy1;_tmp6E3;});}static char _tmp22E[11U]="type_opt_t";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 11U}};
# 1174
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;void*_tmp230;if((_tmp22F.YY49).tag == 57){_LL1: _tmp230=(_tmp22F.YY49).val;_LL2:
# 1178
 yyzzz=_tmp230;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL0:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY49).tag=57U,(_tmp6E4.YY49).val=yy1;_tmp6E4;});}static char _tmp231[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp231,_tmp231,_tmp231 + 31U}};
# 1175
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp232=yy1;struct Cyc_List_List*_tmp233;if((_tmp232.YY50).tag == 58){_LL1: _tmp233=(_tmp232.YY50).val;_LL2:
# 1179
 yyzzz=_tmp233;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL0:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY50).tag=58U,(_tmp6E5.YY50).val=yy1;_tmp6E5;});}static char _tmp234[11U]="booltype_t";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 11U}};
# 1176
void*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp235=yy1;void*_tmp236;if((_tmp235.YY51).tag == 59){_LL1: _tmp236=(_tmp235.YY51).val;_LL2:
# 1180
 yyzzz=_tmp236;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL0:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY51).tag=59U,(_tmp6E6.YY51).val=yy1;_tmp6E6;});}static char _tmp237[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 45U}};
# 1177
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp238=yy1;struct Cyc_List_List*_tmp239;if((_tmp238.YY52).tag == 60){_LL1: _tmp239=(_tmp238.YY52).val;_LL2:
# 1181
 yyzzz=_tmp239;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL0:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY52).tag=60U,(_tmp6E7.YY52).val=yy1;_tmp6E7;});}static char _tmp23A[20U]="pointer_qual_t<`yy>";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp23A,_tmp23A,_tmp23A + 20U}};
# 1178
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp23B=yy1;void*_tmp23C;if((_tmp23B.YY53).tag == 61){_LL1: _tmp23C=(_tmp23B.YY53).val;_LL2:
# 1182
 yyzzz=_tmp23C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL0:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY53).tag=61U,(_tmp6E8.YY53).val=yy1;_tmp6E8;});}static char _tmp23D[21U]="pointer_quals_t<`yy>";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 21U}};
# 1179
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct Cyc_List_List*_tmp23F;if((_tmp23E.YY54).tag == 62){_LL1: _tmp23F=(_tmp23E.YY54).val;_LL2:
# 1183
 yyzzz=_tmp23F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL0:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY54).tag=62U,(_tmp6E9.YY54).val=yy1;_tmp6E9;});}static char _tmp240[21U]="$(bool,string_t<`H>)";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp240,_tmp240,_tmp240 + 21U}};
# 1180
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp241=yy1;struct _tuple20 _tmp242;if((_tmp241.Asm_tok).tag == 6){_LL1: _tmp242=(_tmp241.Asm_tok).val;_LL2:
# 1184
 yyzzz=_tmp242;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL0:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.Asm_tok).tag=6U,(_tmp6EA.Asm_tok).val=yy1;_tmp6EA;});}static char _tmp243[10U]="exp_opt_t";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 10U}};
# 1181
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp244=yy1;struct Cyc_Absyn_Exp*_tmp245;if((_tmp244.YY55).tag == 63){_LL1: _tmp245=(_tmp244.YY55).val;_LL2:
# 1185
 yyzzz=_tmp245;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL0:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY55).tag=63U,(_tmp6EB.YY55).val=yy1;_tmp6EB;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1207
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp6EC;_tmp6EC.timestamp=0,_tmp6EC.first_line=0,_tmp6EC.first_column=0,_tmp6EC.last_line=0,_tmp6EC.last_column=0;_tmp6EC;});}
# 1210
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1221 "parse.y"
static short Cyc_yytranslate[378U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,149,2,2,133,147,144,2,130,131,138,141,126,145,135,146,2,2,2,2,2,2,2,2,2,2,134,123,128,127,129,140,139,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,136,2,137,143,132,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,124,142,125,148,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122};static char _tmp246[2U]="$";static char _tmp247[6U]="error";static char _tmp248[12U]="$undefined.";static char _tmp249[5U]="AUTO";static char _tmp24A[9U]="REGISTER";static char _tmp24B[7U]="STATIC";static char _tmp24C[7U]="EXTERN";static char _tmp24D[8U]="TYPEDEF";static char _tmp24E[5U]="VOID";static char _tmp24F[5U]="CHAR";static char _tmp250[6U]="SHORT";static char _tmp251[4U]="INT";static char _tmp252[5U]="LONG";static char _tmp253[6U]="FLOAT";static char _tmp254[7U]="DOUBLE";static char _tmp255[7U]="SIGNED";static char _tmp256[9U]="UNSIGNED";static char _tmp257[6U]="CONST";static char _tmp258[9U]="VOLATILE";static char _tmp259[9U]="RESTRICT";static char _tmp25A[7U]="STRUCT";static char _tmp25B[6U]="UNION";static char _tmp25C[5U]="CASE";static char _tmp25D[8U]="DEFAULT";static char _tmp25E[7U]="INLINE";static char _tmp25F[7U]="SIZEOF";static char _tmp260[9U]="OFFSETOF";static char _tmp261[3U]="IF";static char _tmp262[5U]="ELSE";static char _tmp263[7U]="SWITCH";static char _tmp264[6U]="WHILE";static char _tmp265[3U]="DO";static char _tmp266[4U]="FOR";static char _tmp267[5U]="GOTO";static char _tmp268[9U]="CONTINUE";static char _tmp269[6U]="BREAK";static char _tmp26A[7U]="RETURN";static char _tmp26B[5U]="ENUM";static char _tmp26C[7U]="TYPEOF";static char _tmp26D[16U]="BUILTIN_VA_LIST";static char _tmp26E[10U]="EXTENSION";static char _tmp26F[8U]="NULL_kw";static char _tmp270[4U]="LET";static char _tmp271[6U]="THROW";static char _tmp272[4U]="TRY";static char _tmp273[6U]="CATCH";static char _tmp274[7U]="EXPORT";static char _tmp275[9U]="OVERRIDE";static char _tmp276[4U]="NEW";static char _tmp277[9U]="ABSTRACT";static char _tmp278[9U]="FALLTHRU";static char _tmp279[6U]="USING";static char _tmp27A[10U]="NAMESPACE";static char _tmp27B[9U]="DATATYPE";static char _tmp27C[7U]="MALLOC";static char _tmp27D[8U]="RMALLOC";static char _tmp27E[15U]="RMALLOC_INLINE";static char _tmp27F[7U]="CALLOC";static char _tmp280[8U]="RCALLOC";static char _tmp281[5U]="SWAP";static char _tmp282[9U]="REGION_T";static char _tmp283[6U]="TAG_T";static char _tmp284[7U]="REGION";static char _tmp285[5U]="RNEW";static char _tmp286[8U]="REGIONS";static char _tmp287[7U]="PORTON";static char _tmp288[8U]="PORTOFF";static char _tmp289[7U]="PRAGMA";static char _tmp28A[10U]="TEMPESTON";static char _tmp28B[11U]="TEMPESTOFF";static char _tmp28C[8U]="NUMELTS";static char _tmp28D[8U]="VALUEOF";static char _tmp28E[10U]="VALUEOF_T";static char _tmp28F[9U]="TAGCHECK";static char _tmp290[13U]="NUMELTS_QUAL";static char _tmp291[10U]="THIN_QUAL";static char _tmp292[9U]="FAT_QUAL";static char _tmp293[13U]="NOTNULL_QUAL";static char _tmp294[14U]="NULLABLE_QUAL";static char _tmp295[14U]="REQUIRES_QUAL";static char _tmp296[13U]="ENSURES_QUAL";static char _tmp297[12U]="REGION_QUAL";static char _tmp298[16U]="NOZEROTERM_QUAL";static char _tmp299[14U]="ZEROTERM_QUAL";static char _tmp29A[12U]="TAGGED_QUAL";static char _tmp29B[16U]="EXTENSIBLE_QUAL";static char _tmp29C[7U]="PTR_OP";static char _tmp29D[7U]="INC_OP";static char _tmp29E[7U]="DEC_OP";static char _tmp29F[8U]="LEFT_OP";static char _tmp2A0[9U]="RIGHT_OP";static char _tmp2A1[6U]="LE_OP";static char _tmp2A2[6U]="GE_OP";static char _tmp2A3[6U]="EQ_OP";static char _tmp2A4[6U]="NE_OP";static char _tmp2A5[7U]="AND_OP";static char _tmp2A6[6U]="OR_OP";static char _tmp2A7[11U]="MUL_ASSIGN";static char _tmp2A8[11U]="DIV_ASSIGN";static char _tmp2A9[11U]="MOD_ASSIGN";static char _tmp2AA[11U]="ADD_ASSIGN";static char _tmp2AB[11U]="SUB_ASSIGN";static char _tmp2AC[12U]="LEFT_ASSIGN";static char _tmp2AD[13U]="RIGHT_ASSIGN";static char _tmp2AE[11U]="AND_ASSIGN";static char _tmp2AF[11U]="XOR_ASSIGN";static char _tmp2B0[10U]="OR_ASSIGN";static char _tmp2B1[9U]="ELLIPSIS";static char _tmp2B2[11U]="LEFT_RIGHT";static char _tmp2B3[12U]="COLON_COLON";static char _tmp2B4[11U]="IDENTIFIER";static char _tmp2B5[17U]="INTEGER_CONSTANT";static char _tmp2B6[7U]="STRING";static char _tmp2B7[8U]="WSTRING";static char _tmp2B8[19U]="CHARACTER_CONSTANT";static char _tmp2B9[20U]="WCHARACTER_CONSTANT";static char _tmp2BA[18U]="FLOATING_CONSTANT";static char _tmp2BB[9U]="TYPE_VAR";static char _tmp2BC[13U]="TYPEDEF_NAME";static char _tmp2BD[16U]="QUAL_IDENTIFIER";static char _tmp2BE[18U]="QUAL_TYPEDEF_NAME";static char _tmp2BF[10U]="ATTRIBUTE";static char _tmp2C0[4U]="ASM";static char _tmp2C1[4U]="';'";static char _tmp2C2[4U]="'{'";static char _tmp2C3[4U]="'}'";static char _tmp2C4[4U]="','";static char _tmp2C5[4U]="'='";static char _tmp2C6[4U]="'<'";static char _tmp2C7[4U]="'>'";static char _tmp2C8[4U]="'('";static char _tmp2C9[4U]="')'";static char _tmp2CA[4U]="'_'";static char _tmp2CB[4U]="'$'";static char _tmp2CC[4U]="':'";static char _tmp2CD[4U]="'.'";static char _tmp2CE[4U]="'['";static char _tmp2CF[4U]="']'";static char _tmp2D0[4U]="'*'";static char _tmp2D1[4U]="'@'";static char _tmp2D2[4U]="'?'";static char _tmp2D3[4U]="'+'";static char _tmp2D4[4U]="'|'";static char _tmp2D5[4U]="'^'";static char _tmp2D6[4U]="'&'";static char _tmp2D7[4U]="'-'";static char _tmp2D8[4U]="'/'";static char _tmp2D9[4U]="'%'";static char _tmp2DA[4U]="'~'";static char _tmp2DB[4U]="'!'";static char _tmp2DC[5U]="prog";static char _tmp2DD[17U]="translation_unit";static char _tmp2DE[18U]="tempest_on_action";static char _tmp2DF[19U]="tempest_off_action";static char _tmp2E0[16U]="extern_c_action";static char _tmp2E1[13U]="end_extern_c";static char _tmp2E2[16U]="export_list_opt";static char _tmp2E3[12U]="export_list";static char _tmp2E4[19U]="export_list_values";static char _tmp2E5[13U]="override_opt";static char _tmp2E6[21U]="external_declaration";static char _tmp2E7[15U]="optional_comma";static char _tmp2E8[20U]="function_definition";static char _tmp2E9[21U]="function_definition2";static char _tmp2EA[13U]="using_action";static char _tmp2EB[15U]="unusing_action";static char _tmp2EC[17U]="namespace_action";static char _tmp2ED[19U]="unnamespace_action";static char _tmp2EE[12U]="declaration";static char _tmp2EF[17U]="declaration_list";static char _tmp2F0[23U]="declaration_specifiers";static char _tmp2F1[24U]="storage_class_specifier";static char _tmp2F2[15U]="attributes_opt";static char _tmp2F3[11U]="attributes";static char _tmp2F4[15U]="attribute_list";static char _tmp2F5[10U]="attribute";static char _tmp2F6[15U]="type_specifier";static char _tmp2F7[25U]="type_specifier_notypedef";static char _tmp2F8[5U]="kind";static char _tmp2F9[15U]="type_qualifier";static char _tmp2FA[15U]="enum_specifier";static char _tmp2FB[11U]="enum_field";static char _tmp2FC[22U]="enum_declaration_list";static char _tmp2FD[26U]="struct_or_union_specifier";static char _tmp2FE[16U]="type_params_opt";static char _tmp2FF[16U]="struct_or_union";static char _tmp300[24U]="struct_declaration_list";static char _tmp301[25U]="struct_declaration_list0";static char _tmp302[21U]="init_declarator_list";static char _tmp303[22U]="init_declarator_list0";static char _tmp304[16U]="init_declarator";static char _tmp305[19U]="struct_declaration";static char _tmp306[25U]="specifier_qualifier_list";static char _tmp307[35U]="notypedef_specifier_qualifier_list";static char _tmp308[23U]="struct_declarator_list";static char _tmp309[24U]="struct_declarator_list0";static char _tmp30A[18U]="struct_declarator";static char _tmp30B[20U]="requires_clause_opt";static char _tmp30C[19U]="ensures_clause_opt";static char _tmp30D[19U]="datatype_specifier";static char _tmp30E[14U]="qual_datatype";static char _tmp30F[19U]="datatypefield_list";static char _tmp310[20U]="datatypefield_scope";static char _tmp311[14U]="datatypefield";static char _tmp312[11U]="declarator";static char _tmp313[23U]="declarator_withtypedef";static char _tmp314[18U]="direct_declarator";static char _tmp315[30U]="direct_declarator_withtypedef";static char _tmp316[8U]="pointer";static char _tmp317[12U]="one_pointer";static char _tmp318[14U]="pointer_quals";static char _tmp319[13U]="pointer_qual";static char _tmp31A[23U]="pointer_null_and_bound";static char _tmp31B[14U]="pointer_bound";static char _tmp31C[18U]="zeroterm_qual_opt";static char _tmp31D[8U]="rgn_opt";static char _tmp31E[11U]="tqual_list";static char _tmp31F[20U]="parameter_type_list";static char _tmp320[9U]="type_var";static char _tmp321[16U]="optional_effect";static char _tmp322[19U]="optional_rgn_order";static char _tmp323[10U]="rgn_order";static char _tmp324[16U]="optional_inject";static char _tmp325[11U]="effect_set";static char _tmp326[14U]="atomic_effect";static char _tmp327[11U]="region_set";static char _tmp328[15U]="parameter_list";static char _tmp329[22U]="parameter_declaration";static char _tmp32A[16U]="identifier_list";static char _tmp32B[17U]="identifier_list0";static char _tmp32C[12U]="initializer";static char _tmp32D[18U]="array_initializer";static char _tmp32E[17U]="initializer_list";static char _tmp32F[12U]="designation";static char _tmp330[16U]="designator_list";static char _tmp331[11U]="designator";static char _tmp332[10U]="type_name";static char _tmp333[14U]="any_type_name";static char _tmp334[15U]="type_name_list";static char _tmp335[20U]="abstract_declarator";static char _tmp336[27U]="direct_abstract_declarator";static char _tmp337[10U]="statement";static char _tmp338[18U]="labeled_statement";static char _tmp339[21U]="expression_statement";static char _tmp33A[19U]="compound_statement";static char _tmp33B[16U]="block_item_list";static char _tmp33C[20U]="selection_statement";static char _tmp33D[15U]="switch_clauses";static char _tmp33E[20U]="iteration_statement";static char _tmp33F[15U]="jump_statement";static char _tmp340[12U]="exp_pattern";static char _tmp341[20U]="conditional_pattern";static char _tmp342[19U]="logical_or_pattern";static char _tmp343[20U]="logical_and_pattern";static char _tmp344[21U]="inclusive_or_pattern";static char _tmp345[21U]="exclusive_or_pattern";static char _tmp346[12U]="and_pattern";static char _tmp347[17U]="equality_pattern";static char _tmp348[19U]="relational_pattern";static char _tmp349[14U]="shift_pattern";static char _tmp34A[17U]="additive_pattern";static char _tmp34B[23U]="multiplicative_pattern";static char _tmp34C[13U]="cast_pattern";static char _tmp34D[14U]="unary_pattern";static char _tmp34E[16U]="postfix_pattern";static char _tmp34F[16U]="primary_pattern";static char _tmp350[8U]="pattern";static char _tmp351[19U]="tuple_pattern_list";static char _tmp352[20U]="tuple_pattern_list0";static char _tmp353[14U]="field_pattern";static char _tmp354[19U]="field_pattern_list";static char _tmp355[20U]="field_pattern_list0";static char _tmp356[11U]="expression";static char _tmp357[22U]="assignment_expression";static char _tmp358[20U]="assignment_operator";static char _tmp359[23U]="conditional_expression";static char _tmp35A[20U]="constant_expression";static char _tmp35B[22U]="logical_or_expression";static char _tmp35C[23U]="logical_and_expression";static char _tmp35D[24U]="inclusive_or_expression";static char _tmp35E[24U]="exclusive_or_expression";static char _tmp35F[15U]="and_expression";static char _tmp360[20U]="equality_expression";static char _tmp361[22U]="relational_expression";static char _tmp362[17U]="shift_expression";static char _tmp363[20U]="additive_expression";static char _tmp364[26U]="multiplicative_expression";static char _tmp365[16U]="cast_expression";static char _tmp366[17U]="unary_expression";static char _tmp367[15U]="unary_operator";static char _tmp368[19U]="postfix_expression";static char _tmp369[17U]="field_expression";static char _tmp36A[19U]="primary_expression";static char _tmp36B[25U]="argument_expression_list";static char _tmp36C[26U]="argument_expression_list0";static char _tmp36D[9U]="constant";static char _tmp36E[20U]="qual_opt_identifier";static char _tmp36F[17U]="qual_opt_typedef";static char _tmp370[18U]="struct_union_name";static char _tmp371[11U]="field_name";static char _tmp372[12U]="right_angle";
# 1578 "parse.y"
static struct _dyneither_ptr Cyc_yytname[301U]={{_tmp246,_tmp246,_tmp246 + 2U},{_tmp247,_tmp247,_tmp247 + 6U},{_tmp248,_tmp248,_tmp248 + 12U},{_tmp249,_tmp249,_tmp249 + 5U},{_tmp24A,_tmp24A,_tmp24A + 9U},{_tmp24B,_tmp24B,_tmp24B + 7U},{_tmp24C,_tmp24C,_tmp24C + 7U},{_tmp24D,_tmp24D,_tmp24D + 8U},{_tmp24E,_tmp24E,_tmp24E + 5U},{_tmp24F,_tmp24F,_tmp24F + 5U},{_tmp250,_tmp250,_tmp250 + 6U},{_tmp251,_tmp251,_tmp251 + 4U},{_tmp252,_tmp252,_tmp252 + 5U},{_tmp253,_tmp253,_tmp253 + 6U},{_tmp254,_tmp254,_tmp254 + 7U},{_tmp255,_tmp255,_tmp255 + 7U},{_tmp256,_tmp256,_tmp256 + 9U},{_tmp257,_tmp257,_tmp257 + 6U},{_tmp258,_tmp258,_tmp258 + 9U},{_tmp259,_tmp259,_tmp259 + 9U},{_tmp25A,_tmp25A,_tmp25A + 7U},{_tmp25B,_tmp25B,_tmp25B + 6U},{_tmp25C,_tmp25C,_tmp25C + 5U},{_tmp25D,_tmp25D,_tmp25D + 8U},{_tmp25E,_tmp25E,_tmp25E + 7U},{_tmp25F,_tmp25F,_tmp25F + 7U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 3U},{_tmp262,_tmp262,_tmp262 + 5U},{_tmp263,_tmp263,_tmp263 + 7U},{_tmp264,_tmp264,_tmp264 + 6U},{_tmp265,_tmp265,_tmp265 + 3U},{_tmp266,_tmp266,_tmp266 + 4U},{_tmp267,_tmp267,_tmp267 + 5U},{_tmp268,_tmp268,_tmp268 + 9U},{_tmp269,_tmp269,_tmp269 + 6U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 5U},{_tmp26C,_tmp26C,_tmp26C + 7U},{_tmp26D,_tmp26D,_tmp26D + 16U},{_tmp26E,_tmp26E,_tmp26E + 10U},{_tmp26F,_tmp26F,_tmp26F + 8U},{_tmp270,_tmp270,_tmp270 + 4U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 4U},{_tmp273,_tmp273,_tmp273 + 6U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 9U},{_tmp276,_tmp276,_tmp276 + 4U},{_tmp277,_tmp277,_tmp277 + 9U},{_tmp278,_tmp278,_tmp278 + 9U},{_tmp279,_tmp279,_tmp279 + 6U},{_tmp27A,_tmp27A,_tmp27A + 10U},{_tmp27B,_tmp27B,_tmp27B + 9U},{_tmp27C,_tmp27C,_tmp27C + 7U},{_tmp27D,_tmp27D,_tmp27D + 8U},{_tmp27E,_tmp27E,_tmp27E + 15U},{_tmp27F,_tmp27F,_tmp27F + 7U},{_tmp280,_tmp280,_tmp280 + 8U},{_tmp281,_tmp281,_tmp281 + 5U},{_tmp282,_tmp282,_tmp282 + 9U},{_tmp283,_tmp283,_tmp283 + 6U},{_tmp284,_tmp284,_tmp284 + 7U},{_tmp285,_tmp285,_tmp285 + 5U},{_tmp286,_tmp286,_tmp286 + 8U},{_tmp287,_tmp287,_tmp287 + 7U},{_tmp288,_tmp288,_tmp288 + 8U},{_tmp289,_tmp289,_tmp289 + 7U},{_tmp28A,_tmp28A,_tmp28A + 10U},{_tmp28B,_tmp28B,_tmp28B + 11U},{_tmp28C,_tmp28C,_tmp28C + 8U},{_tmp28D,_tmp28D,_tmp28D + 8U},{_tmp28E,_tmp28E,_tmp28E + 10U},{_tmp28F,_tmp28F,_tmp28F + 9U},{_tmp290,_tmp290,_tmp290 + 13U},{_tmp291,_tmp291,_tmp291 + 10U},{_tmp292,_tmp292,_tmp292 + 9U},{_tmp293,_tmp293,_tmp293 + 13U},{_tmp294,_tmp294,_tmp294 + 14U},{_tmp295,_tmp295,_tmp295 + 14U},{_tmp296,_tmp296,_tmp296 + 13U},{_tmp297,_tmp297,_tmp297 + 12U},{_tmp298,_tmp298,_tmp298 + 16U},{_tmp299,_tmp299,_tmp299 + 14U},{_tmp29A,_tmp29A,_tmp29A + 12U},{_tmp29B,_tmp29B,_tmp29B + 16U},{_tmp29C,_tmp29C,_tmp29C + 7U},{_tmp29D,_tmp29D,_tmp29D + 7U},{_tmp29E,_tmp29E,_tmp29E + 7U},{_tmp29F,_tmp29F,_tmp29F + 8U},{_tmp2A0,_tmp2A0,_tmp2A0 + 9U},{_tmp2A1,_tmp2A1,_tmp2A1 + 6U},{_tmp2A2,_tmp2A2,_tmp2A2 + 6U},{_tmp2A3,_tmp2A3,_tmp2A3 + 6U},{_tmp2A4,_tmp2A4,_tmp2A4 + 6U},{_tmp2A5,_tmp2A5,_tmp2A5 + 7U},{_tmp2A6,_tmp2A6,_tmp2A6 + 6U},{_tmp2A7,_tmp2A7,_tmp2A7 + 11U},{_tmp2A8,_tmp2A8,_tmp2A8 + 11U},{_tmp2A9,_tmp2A9,_tmp2A9 + 11U},{_tmp2AA,_tmp2AA,_tmp2AA + 11U},{_tmp2AB,_tmp2AB,_tmp2AB + 11U},{_tmp2AC,_tmp2AC,_tmp2AC + 12U},{_tmp2AD,_tmp2AD,_tmp2AD + 13U},{_tmp2AE,_tmp2AE,_tmp2AE + 11U},{_tmp2AF,_tmp2AF,_tmp2AF + 11U},{_tmp2B0,_tmp2B0,_tmp2B0 + 10U},{_tmp2B1,_tmp2B1,_tmp2B1 + 9U},{_tmp2B2,_tmp2B2,_tmp2B2 + 11U},{_tmp2B3,_tmp2B3,_tmp2B3 + 12U},{_tmp2B4,_tmp2B4,_tmp2B4 + 11U},{_tmp2B5,_tmp2B5,_tmp2B5 + 17U},{_tmp2B6,_tmp2B6,_tmp2B6 + 7U},{_tmp2B7,_tmp2B7,_tmp2B7 + 8U},{_tmp2B8,_tmp2B8,_tmp2B8 + 19U},{_tmp2B9,_tmp2B9,_tmp2B9 + 20U},{_tmp2BA,_tmp2BA,_tmp2BA + 18U},{_tmp2BB,_tmp2BB,_tmp2BB + 9U},{_tmp2BC,_tmp2BC,_tmp2BC + 13U},{_tmp2BD,_tmp2BD,_tmp2BD + 16U},{_tmp2BE,_tmp2BE,_tmp2BE + 18U},{_tmp2BF,_tmp2BF,_tmp2BF + 10U},{_tmp2C0,_tmp2C0,_tmp2C0 + 4U},{_tmp2C1,_tmp2C1,_tmp2C1 + 4U},{_tmp2C2,_tmp2C2,_tmp2C2 + 4U},{_tmp2C3,_tmp2C3,_tmp2C3 + 4U},{_tmp2C4,_tmp2C4,_tmp2C4 + 4U},{_tmp2C5,_tmp2C5,_tmp2C5 + 4U},{_tmp2C6,_tmp2C6,_tmp2C6 + 4U},{_tmp2C7,_tmp2C7,_tmp2C7 + 4U},{_tmp2C8,_tmp2C8,_tmp2C8 + 4U},{_tmp2C9,_tmp2C9,_tmp2C9 + 4U},{_tmp2CA,_tmp2CA,_tmp2CA + 4U},{_tmp2CB,_tmp2CB,_tmp2CB + 4U},{_tmp2CC,_tmp2CC,_tmp2CC + 4U},{_tmp2CD,_tmp2CD,_tmp2CD + 4U},{_tmp2CE,_tmp2CE,_tmp2CE + 4U},{_tmp2CF,_tmp2CF,_tmp2CF + 4U},{_tmp2D0,_tmp2D0,_tmp2D0 + 4U},{_tmp2D1,_tmp2D1,_tmp2D1 + 4U},{_tmp2D2,_tmp2D2,_tmp2D2 + 4U},{_tmp2D3,_tmp2D3,_tmp2D3 + 4U},{_tmp2D4,_tmp2D4,_tmp2D4 + 4U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 5U},{_tmp2DD,_tmp2DD,_tmp2DD + 17U},{_tmp2DE,_tmp2DE,_tmp2DE + 18U},{_tmp2DF,_tmp2DF,_tmp2DF + 19U},{_tmp2E0,_tmp2E0,_tmp2E0 + 16U},{_tmp2E1,_tmp2E1,_tmp2E1 + 13U},{_tmp2E2,_tmp2E2,_tmp2E2 + 16U},{_tmp2E3,_tmp2E3,_tmp2E3 + 12U},{_tmp2E4,_tmp2E4,_tmp2E4 + 19U},{_tmp2E5,_tmp2E5,_tmp2E5 + 13U},{_tmp2E6,_tmp2E6,_tmp2E6 + 21U},{_tmp2E7,_tmp2E7,_tmp2E7 + 15U},{_tmp2E8,_tmp2E8,_tmp2E8 + 20U},{_tmp2E9,_tmp2E9,_tmp2E9 + 21U},{_tmp2EA,_tmp2EA,_tmp2EA + 13U},{_tmp2EB,_tmp2EB,_tmp2EB + 15U},{_tmp2EC,_tmp2EC,_tmp2EC + 17U},{_tmp2ED,_tmp2ED,_tmp2ED + 19U},{_tmp2EE,_tmp2EE,_tmp2EE + 12U},{_tmp2EF,_tmp2EF,_tmp2EF + 17U},{_tmp2F0,_tmp2F0,_tmp2F0 + 23U},{_tmp2F1,_tmp2F1,_tmp2F1 + 24U},{_tmp2F2,_tmp2F2,_tmp2F2 + 15U},{_tmp2F3,_tmp2F3,_tmp2F3 + 11U},{_tmp2F4,_tmp2F4,_tmp2F4 + 15U},{_tmp2F5,_tmp2F5,_tmp2F5 + 10U},{_tmp2F6,_tmp2F6,_tmp2F6 + 15U},{_tmp2F7,_tmp2F7,_tmp2F7 + 25U},{_tmp2F8,_tmp2F8,_tmp2F8 + 5U},{_tmp2F9,_tmp2F9,_tmp2F9 + 15U},{_tmp2FA,_tmp2FA,_tmp2FA + 15U},{_tmp2FB,_tmp2FB,_tmp2FB + 11U},{_tmp2FC,_tmp2FC,_tmp2FC + 22U},{_tmp2FD,_tmp2FD,_tmp2FD + 26U},{_tmp2FE,_tmp2FE,_tmp2FE + 16U},{_tmp2FF,_tmp2FF,_tmp2FF + 16U},{_tmp300,_tmp300,_tmp300 + 24U},{_tmp301,_tmp301,_tmp301 + 25U},{_tmp302,_tmp302,_tmp302 + 21U},{_tmp303,_tmp303,_tmp303 + 22U},{_tmp304,_tmp304,_tmp304 + 16U},{_tmp305,_tmp305,_tmp305 + 19U},{_tmp306,_tmp306,_tmp306 + 25U},{_tmp307,_tmp307,_tmp307 + 35U},{_tmp308,_tmp308,_tmp308 + 23U},{_tmp309,_tmp309,_tmp309 + 24U},{_tmp30A,_tmp30A,_tmp30A + 18U},{_tmp30B,_tmp30B,_tmp30B + 20U},{_tmp30C,_tmp30C,_tmp30C + 19U},{_tmp30D,_tmp30D,_tmp30D + 19U},{_tmp30E,_tmp30E,_tmp30E + 14U},{_tmp30F,_tmp30F,_tmp30F + 19U},{_tmp310,_tmp310,_tmp310 + 20U},{_tmp311,_tmp311,_tmp311 + 14U},{_tmp312,_tmp312,_tmp312 + 11U},{_tmp313,_tmp313,_tmp313 + 23U},{_tmp314,_tmp314,_tmp314 + 18U},{_tmp315,_tmp315,_tmp315 + 30U},{_tmp316,_tmp316,_tmp316 + 8U},{_tmp317,_tmp317,_tmp317 + 12U},{_tmp318,_tmp318,_tmp318 + 14U},{_tmp319,_tmp319,_tmp319 + 13U},{_tmp31A,_tmp31A,_tmp31A + 23U},{_tmp31B,_tmp31B,_tmp31B + 14U},{_tmp31C,_tmp31C,_tmp31C + 18U},{_tmp31D,_tmp31D,_tmp31D + 8U},{_tmp31E,_tmp31E,_tmp31E + 11U},{_tmp31F,_tmp31F,_tmp31F + 20U},{_tmp320,_tmp320,_tmp320 + 9U},{_tmp321,_tmp321,_tmp321 + 16U},{_tmp322,_tmp322,_tmp322 + 19U},{_tmp323,_tmp323,_tmp323 + 10U},{_tmp324,_tmp324,_tmp324 + 16U},{_tmp325,_tmp325,_tmp325 + 11U},{_tmp326,_tmp326,_tmp326 + 14U},{_tmp327,_tmp327,_tmp327 + 11U},{_tmp328,_tmp328,_tmp328 + 15U},{_tmp329,_tmp329,_tmp329 + 22U},{_tmp32A,_tmp32A,_tmp32A + 16U},{_tmp32B,_tmp32B,_tmp32B + 17U},{_tmp32C,_tmp32C,_tmp32C + 12U},{_tmp32D,_tmp32D,_tmp32D + 18U},{_tmp32E,_tmp32E,_tmp32E + 17U},{_tmp32F,_tmp32F,_tmp32F + 12U},{_tmp330,_tmp330,_tmp330 + 16U},{_tmp331,_tmp331,_tmp331 + 11U},{_tmp332,_tmp332,_tmp332 + 10U},{_tmp333,_tmp333,_tmp333 + 14U},{_tmp334,_tmp334,_tmp334 + 15U},{_tmp335,_tmp335,_tmp335 + 20U},{_tmp336,_tmp336,_tmp336 + 27U},{_tmp337,_tmp337,_tmp337 + 10U},{_tmp338,_tmp338,_tmp338 + 18U},{_tmp339,_tmp339,_tmp339 + 21U},{_tmp33A,_tmp33A,_tmp33A + 19U},{_tmp33B,_tmp33B,_tmp33B + 16U},{_tmp33C,_tmp33C,_tmp33C + 20U},{_tmp33D,_tmp33D,_tmp33D + 15U},{_tmp33E,_tmp33E,_tmp33E + 20U},{_tmp33F,_tmp33F,_tmp33F + 15U},{_tmp340,_tmp340,_tmp340 + 12U},{_tmp341,_tmp341,_tmp341 + 20U},{_tmp342,_tmp342,_tmp342 + 19U},{_tmp343,_tmp343,_tmp343 + 20U},{_tmp344,_tmp344,_tmp344 + 21U},{_tmp345,_tmp345,_tmp345 + 21U},{_tmp346,_tmp346,_tmp346 + 12U},{_tmp347,_tmp347,_tmp347 + 17U},{_tmp348,_tmp348,_tmp348 + 19U},{_tmp349,_tmp349,_tmp349 + 14U},{_tmp34A,_tmp34A,_tmp34A + 17U},{_tmp34B,_tmp34B,_tmp34B + 23U},{_tmp34C,_tmp34C,_tmp34C + 13U},{_tmp34D,_tmp34D,_tmp34D + 14U},{_tmp34E,_tmp34E,_tmp34E + 16U},{_tmp34F,_tmp34F,_tmp34F + 16U},{_tmp350,_tmp350,_tmp350 + 8U},{_tmp351,_tmp351,_tmp351 + 19U},{_tmp352,_tmp352,_tmp352 + 20U},{_tmp353,_tmp353,_tmp353 + 14U},{_tmp354,_tmp354,_tmp354 + 19U},{_tmp355,_tmp355,_tmp355 + 20U},{_tmp356,_tmp356,_tmp356 + 11U},{_tmp357,_tmp357,_tmp357 + 22U},{_tmp358,_tmp358,_tmp358 + 20U},{_tmp359,_tmp359,_tmp359 + 23U},{_tmp35A,_tmp35A,_tmp35A + 20U},{_tmp35B,_tmp35B,_tmp35B + 22U},{_tmp35C,_tmp35C,_tmp35C + 23U},{_tmp35D,_tmp35D,_tmp35D + 24U},{_tmp35E,_tmp35E,_tmp35E + 24U},{_tmp35F,_tmp35F,_tmp35F + 15U},{_tmp360,_tmp360,_tmp360 + 20U},{_tmp361,_tmp361,_tmp361 + 22U},{_tmp362,_tmp362,_tmp362 + 17U},{_tmp363,_tmp363,_tmp363 + 20U},{_tmp364,_tmp364,_tmp364 + 26U},{_tmp365,_tmp365,_tmp365 + 16U},{_tmp366,_tmp366,_tmp366 + 17U},{_tmp367,_tmp367,_tmp367 + 15U},{_tmp368,_tmp368,_tmp368 + 19U},{_tmp369,_tmp369,_tmp369 + 17U},{_tmp36A,_tmp36A,_tmp36A + 19U},{_tmp36B,_tmp36B,_tmp36B + 25U},{_tmp36C,_tmp36C,_tmp36C + 26U},{_tmp36D,_tmp36D,_tmp36D + 9U},{_tmp36E,_tmp36E,_tmp36E + 20U},{_tmp36F,_tmp36F,_tmp36F + 17U},{_tmp370,_tmp370,_tmp370 + 18U},{_tmp371,_tmp371,_tmp371 + 11U},{_tmp372,_tmp372,_tmp372 + 12U}};
# 1635
static short Cyc_yyr1[536U]={0,150,151,151,151,151,151,151,151,151,151,151,151,152,153,154,155,156,156,157,157,158,158,158,159,159,160,160,160,161,161,162,162,162,162,163,163,164,165,166,167,168,168,168,168,168,168,168,169,169,170,170,170,170,170,170,170,170,170,170,170,171,171,171,171,171,171,171,172,172,173,174,174,175,175,175,175,176,176,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,177,178,179,179,179,180,180,180,181,181,182,182,182,183,183,183,183,183,184,184,185,185,186,186,187,187,188,189,189,190,190,191,192,192,192,192,192,192,193,193,193,193,193,193,194,195,195,196,196,196,196,197,197,198,198,199,199,199,200,200,201,201,201,201,202,202,202,203,203,204,204,205,205,206,206,206,206,206,206,206,206,206,206,207,207,207,207,207,207,207,207,207,207,207,208,208,209,210,210,211,211,211,211,211,211,211,211,212,212,212,213,213,214,214,214,215,215,215,216,216,217,217,217,217,218,218,219,219,220,220,221,221,222,222,223,223,224,224,224,224,225,225,226,226,227,227,227,228,229,229,230,230,231,231,231,231,231,232,232,232,232,233,233,234,234,235,235,236,236,237,237,237,237,237,238,238,239,239,239,240,240,240,240,240,240,240,240,240,240,240,241,241,241,241,241,241,242,243,243,244,244,245,245,245,245,245,245,245,245,246,246,246,246,246,246,247,247,247,247,247,247,248,248,248,248,248,248,248,248,248,248,248,248,248,248,249,249,249,249,249,249,249,249,250,251,251,252,252,253,253,254,254,255,255,256,256,257,257,257,258,258,258,258,258,259,259,259,260,260,260,261,261,261,261,262,262,263,263,263,263,263,263,264,265,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,266,267,267,267,268,268,269,269,270,270,270,271,271,272,272,273,273,273,274,274,274,274,274,274,274,274,274,274,274,275,275,275,275,275,275,275,276,277,277,278,278,279,279,280,280,281,281,282,282,282,283,283,283,283,283,284,284,284,285,285,285,286,286,286,286,287,287,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,288,289,289,289,290,290,290,290,290,290,290,290,290,290,290,291,291,291,291,292,292,292,292,292,292,292,292,292,292,292,293,294,294,295,295,295,295,295,296,296,297,297,298,298,299,299,300,300};
# 1692
static short Cyc_yyr2[536U]={0,1,2,3,5,3,5,7,3,3,3,3,0,1,1,2,1,0,1,4,3,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,2,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1749
static short Cyc_yydefact[1092U]={0,28,61,62,63,64,66,79,80,81,82,83,84,85,86,87,103,104,105,121,122,57,0,0,91,0,0,67,0,0,159,98,100,0,0,0,13,14,0,0,0,526,225,528,527,529,0,0,94,0,211,211,210,1,0,0,0,0,26,0,0,27,0,50,59,53,77,55,88,89,0,92,0,0,170,0,195,198,93,174,119,65,64,58,0,107,0,52,525,0,526,521,522,523,524,0,119,0,385,0,0,0,0,248,0,387,388,37,39,0,0,0,0,0,0,0,0,160,0,0,0,0,0,0,0,208,209,0,0,0,2,0,0,0,0,41,0,127,128,130,51,60,54,56,123,530,531,119,119,0,48,0,0,30,0,227,0,183,171,196,0,202,203,206,207,0,205,204,216,198,0,78,65,111,0,109,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,511,487,0,0,0,0,0,491,489,490,0,414,416,430,438,440,442,444,446,448,451,456,459,462,466,0,468,492,509,507,526,397,0,0,0,0,0,0,398,396,44,0,0,119,0,0,0,137,133,135,268,270,0,0,46,0,0,8,9,0,119,532,533,226,102,0,0,175,95,246,0,243,0,10,11,0,3,0,5,0,42,0,0,0,30,0,124,125,150,118,0,157,0,0,0,0,0,0,0,0,0,0,0,0,526,298,300,0,308,302,0,306,291,292,293,0,294,295,296,0,49,30,130,29,31,275,0,233,249,0,0,229,227,0,213,0,0,0,218,68,217,199,0,112,108,0,0,0,476,0,0,488,432,466,0,433,434,0,0,0,0,0,0,0,0,0,0,469,470,0,0,0,0,472,473,471,0,90,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,420,421,422,423,424,425,426,427,428,429,419,0,474,0,498,499,0,0,0,513,0,119,389,0,0,0,411,526,533,0,0,0,0,264,407,412,0,409,0,386,404,405,0,402,0,250,0,0,0,0,271,0,241,138,143,139,141,134,136,227,0,277,269,278,535,534,0,97,99,0,0,101,117,74,73,0,71,176,227,245,172,277,247,184,185,0,96,212,16,24,38,0,40,0,129,131,252,251,30,32,114,126,0,0,0,145,146,153,0,119,119,165,0,0,0,0,0,526,0,0,0,337,338,339,0,0,341,0,0,0,309,303,130,307,301,299,33,0,182,234,0,0,0,240,228,235,153,0,0,0,229,181,215,214,177,213,0,0,219,69,120,113,437,110,106,0,0,0,0,526,253,258,0,0,0,0,0,0,0,0,0,0,0,0,0,0,512,519,0,518,415,439,0,441,443,445,447,449,450,454,455,452,453,457,458,460,461,463,464,465,418,417,497,494,0,496,0,0,0,400,401,0,267,0,408,262,265,395,0,263,392,0,399,43,0,393,0,272,0,144,140,142,0,229,0,213,0,279,0,227,0,290,274,0,0,119,0,0,0,137,0,119,0,227,0,194,173,244,0,17,4,6,34,0,149,132,150,0,0,148,229,158,167,166,0,0,161,0,0,0,316,0,0,0,0,0,0,336,340,0,0,0,304,297,0,35,276,227,0,237,0,0,155,230,0,153,233,221,178,200,201,219,197,475,0,0,0,254,0,259,478,0,0,0,0,0,508,483,486,0,0,0,0,467,515,0,0,495,493,0,0,0,0,266,410,413,403,406,394,273,242,153,0,280,281,213,0,0,229,213,0,0,45,229,526,0,70,72,0,186,0,0,229,0,213,0,0,0,0,18,147,0,151,123,156,168,165,165,0,0,0,0,0,0,0,0,0,0,0,0,0,316,342,0,305,36,229,0,238,236,0,179,0,155,229,0,220,504,0,503,0,255,260,0,0,0,0,0,435,436,497,496,517,500,0,520,431,514,516,0,390,391,155,153,283,289,153,0,282,213,0,123,0,75,187,193,153,0,192,188,213,0,0,7,0,0,0,164,163,310,316,0,0,0,0,0,0,344,345,347,349,351,353,355,357,360,365,368,371,375,377,383,384,0,0,313,322,0,0,0,0,0,0,0,0,0,0,343,223,239,0,231,180,222,227,477,0,0,261,479,480,0,0,485,484,501,0,286,155,155,153,284,47,0,0,155,153,189,25,20,0,21,152,116,0,0,0,316,0,381,0,0,378,316,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,379,316,0,324,0,0,0,332,0,0,0,0,0,0,315,0,0,229,506,505,0,0,0,502,285,288,155,115,0,190,155,19,22,0,169,311,312,0,0,0,0,316,318,348,0,350,352,354,356,358,359,363,364,361,362,366,367,369,370,372,373,374,0,317,323,325,326,0,334,333,0,328,0,0,0,154,232,224,0,0,0,0,287,0,191,23,314,380,0,376,319,0,316,327,335,329,330,0,257,256,481,0,76,0,346,316,320,331,0,382,321,482,0,0,0};
# 1862
static short Cyc_yydefgoto[151U]={1089,53,54,55,56,482,781,782,943,662,57,319,58,303,59,484,60,486,61,151,62,63,551,242,468,469,243,66,258,244,68,173,174,69,171,70,280,281,136,137,138,282,245,450,497,498,499,672,814,71,72,677,678,679,73,500,74,473,75,76,168,169,77,125,547,334,715,635,78,636,541,706,533,537,538,444,327,266,102,103,564,489,565,424,425,426,246,320,321,637,456,306,307,308,309,310,311,796,312,313,879,880,881,882,883,884,885,886,887,888,889,890,891,892,893,894,427,435,436,428,429,430,314,206,404,207,556,208,209,210,211,212,213,214,215,216,217,218,219,220,221,821,222,581,582,223,224,80,944,431,460};
# 1880
static short Cyc_yypact[1092U]={2924,- -32768,- -32768,- -32768,- -32768,- 18,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3820,94,- 9,- -32768,3820,3395,- -32768,119,30,- -32768,28,55,26,62,86,- -32768,- -32768,100,445,196,- -32768,170,- -32768,- -32768,- -32768,187,756,234,215,176,176,- -32768,- -32768,258,275,298,2783,- -32768,142,466,- -32768,1021,3820,3820,3820,- -32768,3820,- -32768,- -32768,463,- -32768,119,3732,272,59,652,1071,- -32768,- -32768,313,351,402,- -32768,119,365,6901,- -32768,- -32768,3426,295,- -32768,- -32768,- -32768,- -32768,395,313,6901,- -32768,407,492,3426,420,480,501,- -32768,- 42,- -32768,- -32768,3905,3905,193,505,2783,2783,6901,513,- -32768,61,526,244,527,61,4374,6901,- -32768,- -32768,2783,2783,3064,- -32768,2783,3064,2783,3064,- -32768,544,552,- -32768,3601,- -32768,- -32768,- -32768,- -32768,4374,- -32768,- -32768,313,73,1761,- -32768,3732,1021,561,3905,1160,5207,- -32768,272,- -32768,562,- -32768,- -32768,- -32768,- -32768,564,- -32768,- -32768,- 15,1071,3905,- -32768,- -32768,578,581,589,119,7303,608,7402,6901,7099,612,616,628,630,636,648,653,655,658,667,7402,7402,- -32768,- -32768,- -32768,2491,670,7501,7501,7501,- -32768,- -32768,- -32768,218,- -32768,- -32768,3,624,660,663,669,545,169,556,266,231,- -32768,895,7501,112,- 28,- -32768,699,31,- -32768,3426,- 13,714,1244,359,3130,715,- -32768,- -32768,717,6901,313,3130,707,4119,4374,4459,4374,408,- -32768,- 3,- 3,- -32768,741,735,- -32768,- -32768,370,313,- -32768,- -32768,- -32768,- -32768,43,734,- -32768,- -32768,751,373,- -32768,743,- -32768,- -32768,749,- -32768,757,- -32768,767,- -32768,244,5306,3732,561,768,4374,- -32768,625,774,119,775,750,222,770,4527,771,796,801,803,5416,2349,4527,286,794,- -32768,- -32768,804,1908,1908,1021,1908,- -32768,- -32768,- -32768,806,- -32768,- -32768,- -32768,205,- -32768,561,808,- -32768,- -32768,797,85,830,- -32768,- 2,810,809,245,814,644,811,6901,3905,- -32768,825,- -32768,- -32768,85,119,- -32768,6901,826,2491,- -32768,4374,2491,- -32768,- -32768,- -32768,4637,- -32768,854,6901,6901,6901,6901,6901,6901,842,6901,4374,246,- -32768,- -32768,1908,827,377,6901,- -32768,- -32768,- -32768,6901,- -32768,7501,6901,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,6901,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6901,- -32768,61,- -32768,- -32768,5515,61,6901,- -32768,829,313,- -32768,831,832,834,- -32768,40,395,61,6901,3426,297,- -32768,- -32768,- -32768,841,833,835,- -32768,- -32768,- -32768,836,844,3426,- -32768,312,1244,840,3905,- -32768,847,848,- -32768,4459,4459,4459,- -32768,- -32768,3330,5614,158,- -32768,333,- -32768,- -32768,- 2,- -32768,- -32768,843,865,- -32768,852,- -32768,853,851,858,- -32768,3197,- -32768,593,358,- -32768,- -32768,- -32768,4374,- -32768,- -32768,- -32768,939,- -32768,2783,- -32768,2783,- -32768,- -32768,- -32768,- -32768,561,- -32768,- -32768,- -32768,911,6901,864,862,- -32768,- 4,569,313,313,805,6901,6901,860,867,6901,868,973,2202,881,- -32768,- -32768,- -32768,327,960,- -32768,5713,2055,2633,- -32768,- -32768,3601,- -32768,- -32768,- -32768,- -32768,3905,- -32768,- -32768,4374,876,4204,- -32768,- -32768,866,929,- 2,879,4289,809,- -32768,- -32768,- -32768,- -32768,644,880,75,777,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,882,888,885,907,884,- -32768,- -32768,690,5306,892,893,900,908,909,434,902,906,912,260,913,7201,- -32768,- -32768,914,918,- -32768,624,157,660,663,669,545,169,169,556,556,556,556,266,266,231,231,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,915,- -32768,44,3905,5093,4374,- -32768,4374,- -32768,903,- -32768,- -32768,- -32768,- -32768,1469,- -32768,- -32768,3311,- -32768,- -32768,922,- -32768,132,- -32768,4374,- -32768,- -32768,- -32768,917,809,921,644,916,333,3905,3993,5812,- -32768,- -32768,6901,931,313,7000,925,43,3463,928,313,3905,1160,5911,- -32768,593,- -32768,936,993,- -32768,- -32768,- -32768,999,- -32768,- -32768,625,935,6901,- -32768,809,- -32768,- -32768,- -32768,942,119,341,435,454,6901,799,495,4527,940,6010,6109,379,- -32768,- -32768,947,949,943,1908,- -32768,3732,- -32768,797,953,3905,- -32768,952,- 2,998,- -32768,951,929,204,- -32768,- -32768,- -32768,- -32768,777,- -32768,958,276,958,955,- -32768,4751,- -32768,- -32768,6901,6901,1064,6901,7099,- -32768,- -32768,- -32768,61,61,959,4865,- -32768,- -32768,6901,6901,- -32768,- -32768,85,708,982,983,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,929,963,- -32768,- -32768,644,85,964,809,644,961,503,- -32768,809,970,966,- -32768,- -32768,969,- -32768,85,975,809,976,644,967,3064,977,2783,- -32768,- -32768,6901,- -32768,4374,- -32768,978,80,805,4527,987,981,1422,979,991,4527,6901,6208,413,6307,540,6406,799,- -32768,1004,- -32768,- -32768,809,164,- -32768,- -32768,992,- -32768,1011,998,809,4374,- -32768,- -32768,385,- -32768,6901,- -32768,- -32768,5306,990,1001,1000,1007,- -32768,854,1003,1012,- -32768,- -32768,710,- -32768,- -32768,- -32768,- -32768,5093,- -32768,- -32768,998,929,- -32768,- -32768,929,1024,- -32768,644,1019,4374,1045,- -32768,- -32768,- -32768,929,1026,- -32768,- -32768,644,1010,115,- -32768,1027,1038,4374,- -32768,- -32768,1136,799,1042,7600,1037,2633,7501,1048,- -32768,12,- -32768,1088,1044,697,754,194,755,381,299,- -32768,- -32768,- -32768,- -32768,1089,7501,1908,- -32768,- -32768,551,4527,559,6505,4527,565,6604,6703,582,1062,- -32768,- -32768,- -32768,6901,1063,- -32768,- -32768,953,- -32768,422,265,- -32768,- -32768,- -32768,4374,1163,- -32768,- -32768,- -32768,4979,- -32768,998,998,929,- -32768,- -32768,1065,1068,998,929,- -32768,- -32768,- -32768,1066,592,- -32768,- -32768,576,4527,1070,799,2491,- -32768,4374,1069,- -32768,1614,7501,6901,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,7501,6901,- -32768,799,1073,- -32768,4527,4527,610,- -32768,4527,4527,622,4527,623,6802,- -32768,1074,- 2,809,- -32768,- -32768,2633,1075,1072,- -32768,- -32768,- -32768,998,- -32768,1090,- -32768,998,- -32768,- -32768,513,- -32768,- -32768,- -32768,1079,1077,1083,7501,799,- -32768,624,325,660,663,663,545,169,169,556,556,556,556,266,266,231,231,- -32768,- -32768,- -32768,336,- -32768,- -32768,- -32768,- -32768,4527,- -32768,- -32768,4527,- -32768,4527,4527,631,- -32768,- -32768,- -32768,1086,724,1081,4374,- -32768,1084,- -32768,- -32768,- -32768,958,276,- -32768,- -32768,6901,1614,- -32768,- -32768,- -32768,- -32768,4527,- -32768,- -32768,- -32768,1085,- -32768,433,- -32768,799,- -32768,- -32768,1087,- -32768,- -32768,- -32768,1214,1217,- -32768};
# 1993
static short Cyc_yypgoto[151U]={- -32768,443,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,213,- -32768,- -32768,- 233,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6,- 95,- 14,- -32768,- -32768,0,580,- -32768,123,- 203,1105,33,- -32768,- -32768,- 110,- -32768,296,1190,- 745,- -32768,- -32768,- -32768,957,954,824,360,- -32768,- -32768,567,- 199,- 145,- -32768,- -32768,95,- -32768,- -32768,155,- 194,1159,- 406,191,- -32768,1078,- -32768,- -32768,1186,101,- -32768,524,- 127,- 151,- 150,- 442,247,530,537,- 398,- 515,- 119,- 430,- 126,- -32768,- 264,- 149,- 540,- 293,- -32768,817,- 121,- 91,- 158,- 154,- 382,76,- -32768,- -32768,- 46,- 280,- -32768,- 697,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 12,1009,- -32768,632,815,- -32768,427,732,- -32768,- 143,- 378,- 142,- 363,- 339,- 368,869,- 335,- 364,- 113,- 355,- 353,- 198,532,449,890,184,- -32768,- 357,- -32768,- 20,541,- 61,8,- 80,- 22};
# 2015
static short Cyc_yytable[7750U]={64,368,369,370,265,326,105,83,587,146,584,87,337,488,104,590,591,335,247,248,703,64,405,523,524,64,526,153,325,328,596,597,350,67,598,599,586,347,259,351,448,868,259,589,278,616,492,120,660,140,141,142,607,143,67,146,566,64,67,152,466,645,534,64,64,64,341,64,659,105,472,743,640,64,157,670,365,226,147,150,412,105,238,529,577,675,676,457,239,234,67,455,640,279,81,659,67,67,67,373,67,710,42,700,416,316,67,909,957,936,475,413,660,64,64,42,753,333,667,417,445,86,535,65,255,152,458,64,64,64,671,64,64,64,64,305,111,152,459,64,108,227,707,374,65,150,67,67,65,64,227,64,958,467,112,304,109,315,157,228,67,67,67,694,67,67,67,67,228,41,371,256,67,536,- 532,457,949,543,44,257,65,741,67,110,67,113,65,65,65,47,65,600,601,602,755,837,65,555,406,407,408,170,121,477,41,- 162,713,105,285,114,105,530,105,44,458,415,459,139,84,105,434,558,477,559,560,41,461,434,554,41,115,786,491,43,44,45,65,65,44,575,942,550,409,447,448,448,448,410,411,117,65,65,65,1014,65,65,65,65,665,1019,381,382,65,752,152,131,132,159,592,593,594,595,65,459,65,261,449,653,64,118,555,1040,87,371,315,965,966,88,452,305,305,739,305,785,453,912,64,383,384,531,124,653,722,64,64,459,64,317,536,304,304,67,304,188,532,553,249,119,566,850,250,1067,967,968,854,793,605,324,528,67,608,371,41,860,552,- 227,67,67,- 227,67,705,44,615,122,371,123,732,407,408,372,305,629,506,555,41,507,41,91,194,195,92,93,94,44,64,44,511,911,324,389,304,542,1083,518,47,916,345,390,391,198,736,127,50,51,52,1087,256,820,917,536,409,371,230,46,257,733,411,67,128,997,154,65,155,265,105,227,867,387,156,519,654,388,617,477,445,807,520,105,- 249,65,105,- 249,129,228,618,625,65,65,826,65,697,317,422,423,477,626,454,973,371,699,477,170,566,284,286,974,975,447,447,447,691,371,742,371,46,474,644,825,771,1068,525,641,371,642,789,19,20,790,41,643,1069,652,771,658,501,- 15,43,44,45,698,449,449,449,759,64,371,64,65,471,176,432,744,761,745,453,666,371,773,152,478,130,464,803,371,479,371,775,152,579,816,445,152,64,205,172,760,918,67,688,67,919,64,971,229,231,64,972,695,555,774,776,150,256,995,440,992,903,232,452,371,257,79,254,235,453,67,50,51,52,826,809,465,1054,536,67,845,252,253,67,817,371,371,921,85,1086,728,791,106,919,107,268,269,270,41,271,272,273,274,825,831,371,43,44,45,714,792,832,144,79,133,134,654,1023,1024,1020,707,839,696,79,1026,1027,105,233,79,105,477,236,65,477,65,810,145,750,148,1032,1033,79,978,1034,1035,1022,371,251,41,366,175,797,1025,237,371,106,43,44,45,853,65,826,822,379,380,644,555,106,454,65,385,386,931,65,711,932,808,833,834,79,79,260,145,262,658,938,79,474,906,439,825,371,275,79,79,79,915,79,79,79,79,1018,371,276,41,955,305,979,152,318,371,501,43,44,45,981,371,331,79,332,64,985,64,977,495,930,304,478,315,338,990,339,1011,371,343,611,346,348,348,46,1009,340,175,1010,375,840,655,517,656,362,363,545,546,67,657,67,348,348,348,1003,41,371,848,344,757,1007,1044,352,43,44,45,353,714,371,371,947,858,348,1047,1049,495,954,371,354,496,355,696,1074,50,51,52,356,994,106,366,555,106,366,106,105,1036,1037,1038,357,64,106,64,895,358,572,359,1001,1002,360,1082,50,51,52,1006,16,17,18,361,673,674,367,585,376,998,146,476,377,632,633,634,675,676,67,378,67,720,721,79,65,1066,65,794,795,414,476,437,503,438,346,508,1015,418,1016,841,842,928,929,442,609,996,961,962,536,501,969,970,79,963,964,1076,371,462,1028,1029,1030,1031,267,501,1059,847,501,41,1061,851,463,470,41,872,480,43,44,45,348,899,481,44,1055,46,862,175,505,471,483,305,870,871,47,453,330,50,51,52,485,493,50,51,52,64,502,504,509,512,65,304,65,348,513,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,514,1081,515,663,521,664,67,527,680,681,522,277,684,1078,459,689,532,539,305,540,766,544,46,264,548,146,373,557,573,934,392,348,64,610,578,621,612,613,304,614,940,106,620,623,283,622,624,628,630,646,631,647,648,980,106,264,984,106,650,649,651,822,661,668,669,67,682,683,393,394,395,396,397,398,399,400,401,402,685,686,690,692,701,704,670,490,708,712,476,716,717,476,718,719,- 532,724,65,41,403,723,1012,79,725,79,348,43,44,45,46,729,726,727,476,730,734,780,746,495,476,731,738,737,740,751,754,50,51,52,756,758,765,305,769,1042,1043,772,779,1045,1046,549,1048,784,446,787,451,64,798,804,805,764,806,304,324,811,813,65,815,490,735,823,567,568,569,570,571,829,835,574,843,844,846,849,855,856,852,580,857,865,67,583,863,283,859,861,869,41,348,873,874,897,800,802,898,43,44,45,1070,922,913,1071,603,1072,1073,910,914,495,924,41,923,925,926,941,604,50,51,52,44,580,935,927,135,160,161,162,163,164,1084,47,165,166,167,933,937,939,945,50,51,52,106,946,948,106,950,953,7,8,9,10,11,12,13,14,15,16,17,18,19,20,956,959,976,639,960,991,999,993,1004,1008,65,476,1005,1013,1041,22,23,24,1017,1060,1058,348,1063,1052,1057,476,1064,1065,476,1075,1077,30,1090,1079,1085,1091,1088,788,31,32,864,1062,866,900,902,263,905,116,908,770,38,487,158,494,783,126,819,818,1053,812,619,896,39,40,588,336,441,1080,920,576,580,748,580,627,0,0,0,0,348,0,0,0,0,0,0,322,0,0,323,348,0,0,0,0,264,42,43,0,45,46,0,324,0,88,0,0,0,0,0,0,48,49,0,264,0,0,490,0,0,0,264,0,231,0,0,0,0,0,0,0,0,0,0,0,348,0,0,0,79,0,79,0,0,0,0,0,0,0,983,0,0,987,989,106,0,0,0,89,0,0,0,490,0,0,264,0,0,0,0,419,0,0,420,91,0,264,92,93,94,0,421,44,0,0,264,0,96,0,0,0,0,0,97,763,98,99,366,422,423,768,100,0,0,1021,0,0,101,778,0,0,0,0,0,0,0,0,0,0,0,0,0,1039,0,0,145,952,0,0,348,0,0,0,580,0,0,1051,0,0,0,0,0,0,1056,0,0,0,348,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,348,0,875,876,0,0,0,0,490,0,0,827,828,0,830,0,0,0,88,0,0,264,490,0,0,838,0,0,0,0,0,446,0,0,0,264,0,0,0,0,0,0,0,0,348,0,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,0,0,88,0,0,0,0,0,0,89,0,0,0,0,0,0,0,0,0,0,0,0,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,348,0,145,877,0,98,99,0,0,490,0,100,0,0,878,89,0,101,202,0,0,203,204,0,0,490,0,747,0,0,420,91,0,0,92,93,94,0,421,44,0,0,0,0,96,0,0,0,0,0,97,348,98,99,0,422,423,0,100,0,0,283,0,0,101,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,794,795,21,177,178,287,264,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,490,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,283,0,0,188,0,0,189,190,38,191,0,0,0,0,0,264,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,301,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,296,88,26,180,0,0,0,0,181,27,0,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,687,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,296,88,0,0,0,0,0,0,0,27,0,0,0,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,0,0,0,345,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,364,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,89,0,0,0,0,0,0,0,0,22,23,24,433,0,0,225,91,0,0,92,93,94,0,95,44,30,0,0,0,96,0,0,31,32,0,97,0,98,99,0,0,0,0,100,38,0,0,0,0,101,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,324,0,0,0,0,0,0,471,0,48,49,0,0,453,0,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,89,0,0,0,0,0,0,0,39,40,0,0,749,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,88,322,0,0,0,97,0,98,99,0,0,42,43,100,45,46,0,324,0,101,0,0,0,0,452,0,48,49,0,0,453,88,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,89,0,0,0,0,0,0,0,0,0,22,23,24,0,0,90,91,0,0,92,93,94,0,95,44,0,30,0,0,96,0,89,0,31,32,97,0,98,99,0,0,0,0,100,0,38,225,91,0,101,92,93,94,0,95,44,0,39,40,0,96,0,0,0,0,0,97,0,98,99,0,0,0,0,100,0,0,0,0,0,101,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,495,0,48,49,0,0,0,0,50,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,277,0,0,0,0,48,49,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,48,49,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,240,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,241,22,23,24,0,0,0,0,48,49,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,324,0,0,0,0,0,0,0,0,48,49,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,443,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,702,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,709,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,177,178,287,0,288,289,290,291,292,293,294,295,0,0,0,179,88,0,180,297,0,0,0,181,42,298,0,0,46,182,183,184,185,186,0,0,0,0,187,48,49,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,0,0,44,0,0,196,300,149,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,561,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,563,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,824,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,836,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,1000,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,329,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,516,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,606,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,638,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,693,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,762,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,777,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,799,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,801,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,901,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,904,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,907,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,982,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,986,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,988,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,1050,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,0,181,203,204,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,767,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,349,0,0,177,178,0,197,0,0,198,0,0,0,0,199,0,0,200,179,88,201,202,0,0,203,204,0,0,0,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,735,0,0,177,178,0,197,0,0,198,0,0,0,0,199,0,0,200,179,88,201,202,0,0,203,204,0,0,0,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,342,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,345,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,0,0,0,951,0,0,198,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204};
# 2793
static short Cyc_yycheck[7750U]={0,199,200,201,123,155,26,21,376,70,373,25,170,277,26,379,380,168,109,110,535,21,220,303,304,25,306,73,155,155,385,386,181,0,387,388,375,180,118,181,243,786,122,378,139,423,279,47,478,63,64,65,409,67,21,116,349,57,25,73,17,459,64,63,64,65,176,67,474,89,264,611,454,73,74,79,197,89,70,73,108,101,124,316,364,5,6,90,130,101,57,245,474,139,112,501,63,64,65,96,67,543,117,533,117,151,73,804,96,854,264,139,542,113,114,117,631,132,496,132,241,130,124,0,116,139,129,127,128,129,134,131,132,133,134,149,110,151,141,139,110,110,540,140,21,139,113,114,25,149,110,151,140,110,128,149,128,151,158,128,127,128,129,520,131,132,133,134,128,110,126,110,139,324,134,90,873,327,119,118,57,137,149,128,151,123,63,64,65,130,67,389,390,391,636,735,73,340,86,87,88,128,47,264,110,125,131,227,135,123,230,126,232,119,129,227,141,62,124,239,232,342,283,344,345,110,248,239,338,110,130,673,278,118,119,120,113,114,119,360,125,332,130,243,447,448,449,135,136,53,127,128,129,950,131,132,133,134,491,956,91,92,139,131,278,123,124,76,381,382,383,384,149,141,151,120,243,471,278,109,423,978,296,126,278,91,92,41,130,303,304,134,306,671,136,131,296,128,129,321,124,495,566,303,304,141,306,152,459,303,304,278,306,67,110,337,123,130,611,761,127,1018,128,129,766,682,406,123,123,296,410,126,110,775,334,131,303,304,134,306,539,119,422,109,126,130,86,87,88,131,364,442,130,496,110,133,110,111,112,113,114,115,116,119,364,119,290,809,123,138,364,126,1069,297,130,817,130,146,147,133,578,123,138,139,140,1082,110,111,818,540,130,126,96,121,118,135,136,364,123,134,128,278,130,522,424,110,784,141,136,123,471,145,424,474,535,695,130,437,123,296,440,126,124,128,127,437,303,304,721,306,525,276,135,136,495,123,245,138,126,530,501,128,735,147,148,146,147,447,448,449,123,126,610,126,121,264,456,721,652,134,305,128,126,130,123,20,21,126,110,136,134,471,666,473,283,124,118,119,120,525,447,448,449,641,484,126,486,364,130,124,131,612,642,614,136,495,126,655,512,126,57,131,123,126,131,126,656,521,131,708,631,525,512,86,112,642,131,484,512,486,135,521,141,128,97,525,145,521,671,656,656,525,110,111,238,913,123,130,130,126,118,0,115,123,136,512,138,139,140,842,700,255,994,704,521,754,113,114,525,709,126,126,826,22,131,131,131,26,135,28,127,128,129,110,131,132,133,134,842,728,126,118,119,120,551,131,728,124,47,123,124,652,960,961,957,993,739,521,57,963,964,621,110,62,624,666,126,484,669,486,701,70,624,72,969,970,75,897,971,972,959,126,117,110,197,84,131,962,127,126,89,118,119,120,131,512,929,717,93,94,640,784,101,452,521,89,90,846,525,548,849,697,732,733,113,114,130,116,131,659,859,120,471,123,237,929,126,123,127,128,129,816,131,132,133,134,956,126,126,110,878,695,131,697,123,126,495,118,119,120,131,126,130,152,130,695,131,697,896,130,845,695,126,697,126,123,125,131,126,177,414,179,180,181,121,123,127,176,126,95,742,128,295,130,192,193,82,83,695,136,697,199,200,201,933,110,126,759,130,638,939,131,130,118,119,120,130,714,126,126,869,773,220,131,131,130,877,126,130,134,130,685,131,138,139,140,130,917,227,342,913,230,345,232,794,973,974,975,130,779,239,781,794,130,357,130,931,932,130,1069,138,139,140,938,17,18,19,130,502,503,130,374,142,924,865,264,143,447,448,449,5,6,779,144,781,125,126,276,695,1017,697,22,23,124,283,110,285,110,296,288,951,117,953,125,126,125,126,130,411,919,143,144,993,652,89,90,305,93,94,125,126,110,965,966,967,968,124,666,1003,758,669,110,1007,762,129,131,110,791,125,118,119,120,340,797,125,119,997,121,777,338,130,130,125,897,789,790,130,136,156,138,139,140,125,125,138,139,140,897,124,124,130,130,779,897,781,373,110,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,123,1068,123,484,134,486,897,125,505,506,130,127,509,1058,141,512,110,131,956,134,648,131,121,123,137,1010,96,125,110,852,59,423,956,128,131,126,129,129,956,129,863,424,125,131,144,134,126,131,125,130,126,110,124,901,437,155,904,440,131,130,126,1065,47,123,126,956,130,124,97,98,99,100,101,102,103,104,105,106,134,30,123,45,130,141,79,277,131,131,471,131,126,474,131,110,134,126,897,110,127,131,948,484,126,486,496,118,119,120,121,131,126,126,495,131,125,46,137,130,501,131,126,131,131,125,131,138,139,140,131,137,123,1069,131,981,982,131,124,985,986,331,988,130,242,125,244,1069,130,124,123,646,131,1069,123,125,80,956,129,349,124,128,352,353,354,355,356,25,131,359,110,110,131,131,126,131,137,367,131,124,1069,371,137,281,131,131,130,110,578,124,131,134,687,688,125,118,119,120,1044,131,130,1047,392,1049,1050,123,117,130,130,110,131,126,131,125,404,138,139,140,119,409,123,131,123,74,75,76,77,78,1074,130,81,82,83,131,111,131,131,138,139,140,621,125,28,624,124,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,134,95,95,453,142,125,25,126,125,125,1069,652,126,125,123,37,38,39,131,111,130,671,125,131,131,666,131,126,669,125,131,53,0,131,131,0,131,678,60,61,779,1010,781,798,799,122,801,39,803,651,72,276,75,281,669,51,714,709,993,704,425,794,84,85,377,169,239,1065,823,361,520,621,522,440,- 1,- 1,- 1,- 1,728,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,739,- 1,- 1,- 1,- 1,452,117,118,- 1,120,121,- 1,123,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,471,- 1,- 1,566,- 1,- 1,- 1,478,- 1,877,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,784,- 1,- 1,- 1,779,- 1,781,- 1,- 1,- 1,- 1,- 1,- 1,- 1,903,- 1,- 1,906,907,794,- 1,- 1,- 1,95,- 1,- 1,- 1,611,- 1,- 1,522,- 1,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,533,114,115,116,- 1,118,119,- 1,- 1,542,- 1,124,- 1,- 1,- 1,- 1,- 1,130,643,132,133,951,135,136,649,138,- 1,- 1,958,- 1,- 1,144,657,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,976,- 1,- 1,865,875,- 1,- 1,878,- 1,- 1,- 1,682,- 1,- 1,990,- 1,- 1,- 1,- 1,- 1,- 1,997,- 1,- 1,- 1,896,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,913,- 1,25,26,- 1,- 1,- 1,- 1,721,- 1,- 1,724,725,- 1,727,- 1,- 1,- 1,41,- 1,- 1,642,735,- 1,- 1,738,- 1,- 1,- 1,- 1,- 1,652,- 1,- 1,- 1,656,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,957,- 1,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,974,975,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,- 1,- 1,- 1,124,- 1,- 1,1017,- 1,1010,130,- 1,132,133,- 1,- 1,826,- 1,138,- 1,- 1,141,95,- 1,144,145,- 1,- 1,148,149,- 1,- 1,842,- 1,107,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,- 1,130,1068,132,133,- 1,135,136,- 1,138,- 1,- 1,786,- 1,- 1,144,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,818,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,929,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,854,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,869,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,48,49,50,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,48,49,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,62,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,53,54,55,56,57,58,- 1,60,61,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,51,52,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,65,66,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,51,52,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,65,66,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,51,52,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,65,66,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,107,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,53,- 1,- 1,- 1,124,- 1,- 1,60,61,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,72,- 1,- 1,- 1,- 1,144,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,136,- 1,138,139,140,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,- 1,- 1,- 1,124,41,107,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,117,118,138,120,121,- 1,123,- 1,144,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,136,41,138,139,140,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,110,111,- 1,- 1,114,115,116,- 1,118,119,- 1,53,- 1,- 1,124,- 1,95,- 1,60,61,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,72,110,111,- 1,144,114,115,116,- 1,118,119,- 1,84,85,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,132,133,- 1,- 1,- 1,- 1,138,139,140,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,132,133,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,- 1,124,37,38,39,40,- 1,- 1,- 1,132,133,- 1,- 1,- 1,49,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,- 1,124,37,38,39,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,- 1,120,121,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,- 1,- 1,- 1,53,- 1,- 1,- 1,- 1,- 1,- 1,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,48,117,50,- 1,- 1,121,54,55,56,57,58,- 1,- 1,- 1,- 1,63,132,133,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,25,26,- 1,138,- 1,- 1,141,32,- 1,144,145,- 1,- 1,148,149,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,118,119,- 1,- 1,122,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,135,136,- 1,138,25,26,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,25,26,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149,40,41,- 1,43,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,137,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,123,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,131,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,43,- 1,144,145,- 1,48,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,25,26,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,40,41,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,124,- 1,- 1,25,26,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,40,41,144,145,- 1,- 1,148,149,- 1,- 1,- 1,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,25,26,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,40,41,141,- 1,- 1,144,145,- 1,- 1,148,149,- 1,- 1,- 1,54,55,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,111,112,113,114,115,116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,138,- 1,- 1,141,- 1,- 1,144,145,- 1,- 1,148,149};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp40F[8U]="stdcall";static char _tmp410[6U]="cdecl";static char _tmp411[9U]="fastcall";static char _tmp412[9U]="noreturn";static char _tmp413[6U]="const";static char _tmp414[8U]="aligned";static char _tmp415[7U]="packed";static char _tmp416[7U]="shared";static char _tmp417[7U]="unused";static char _tmp418[5U]="weak";static char _tmp419[10U]="dllimport";static char _tmp41A[10U]="dllexport";static char _tmp41B[23U]="no_instrument_function";static char _tmp41C[12U]="constructor";static char _tmp41D[11U]="destructor";static char _tmp41E[22U]="no_check_memory_usage";static char _tmp41F[5U]="pure";static char _tmp420[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp373=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp373;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp731;(_tmp731.YYINITIALSVAL).tag=64U,(_tmp731.YYINITIALSVAL).val=0;_tmp731;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp681=200U;_tag_dyneither(_region_calloc(yyregion,sizeof(short),_tmp681),sizeof(short),_tmp681);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp680=200U;union Cyc_YYSTYPE*_tmp67F=({struct _RegionHandle*_tmp7E5=yyregion;_region_malloc(_tmp7E5,_check_times(_tmp680,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp730=200U;unsigned int i;for(i=0;i < _tmp730;++ i){_tmp67F[i]=yylval;}}0;});_tmp67F;}),sizeof(union Cyc_YYSTYPE),200U);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp67E=200U;struct Cyc_Yyltype*_tmp67D=({struct _RegionHandle*_tmp7E6=yyregion;_region_malloc(_tmp7E6,_check_times(_tmp67E,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp72F=200U;unsigned int i;for(i=0;i < _tmp72F;++ i){({struct Cyc_Yyltype _tmp7E7=Cyc_yynewloc();_tmp67D[i]=_tmp7E7;});}}0;});_tmp67D;}),sizeof(struct Cyc_Yyltype),200U);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 193
yyssp_offset=-1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
({struct _dyneither_ptr _tmp7E9=({const char*_tmp374="parser stack overflow";_tag_dyneither(_tmp374,sizeof(char),22U);});int _tmp7E8=yystate;Cyc_yyerror(_tmp7E9,_tmp7E8,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp37A=(unsigned int)yystacksize;short*_tmp379=({struct _RegionHandle*_tmp7EA=yyregion;_region_malloc(_tmp7EA,_check_times(_tmp37A,sizeof(short)));});({{unsigned int _tmp6EF=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6EF;++ i){
i <= (unsigned int)yyssp_offset?_tmp379[i]=*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)):(_tmp379[i]=0);}}0;});_tag_dyneither(_tmp379,sizeof(short),_tmp37A);});
struct _dyneither_ptr yyvs1=({unsigned int _tmp378=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp377=({struct _RegionHandle*_tmp7EB=yyregion;_region_malloc(_tmp7EB,_check_times(_tmp378,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp6EE=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6EE;++ i){
# 219
i <= (unsigned int)yyssp_offset?_tmp377[i]=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)):(_tmp377[i]=yylval);}}0;});_tag_dyneither(_tmp377,sizeof(union Cyc_YYSTYPE),_tmp378);});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp376=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp375=({struct _RegionHandle*_tmp7EC=yyregion;_region_malloc(_tmp7EC,_check_times(_tmp376,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp6ED=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6ED;++ i){
i <= (unsigned int)yyssp_offset?_tmp375[i]=*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):({
struct Cyc_Yyltype _tmp7ED=Cyc_yynewloc();_tmp375[i]=_tmp7ED;});}}0;});_tag_dyneither(_tmp375,sizeof(struct Cyc_Yyltype),_tmp376);});
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1092U,sizeof(short),yystate));
if(yyn == -32768)goto yydefault;
# 259
if(yychar == -2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 377?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,378U,sizeof(short),yychar)): 301;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7749) || (int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),yyn))!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[yyn];
# 307
if(yyn < 0){
# 309
if(yyn == -32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1091){
int _tmp37B=0;_npop_handler(0U);return _tmp37B;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[yystate];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,536U,sizeof(short),yyn));
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp37C=yyn;switch(_tmp37C){case 1U: _LL1: _LL2: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1186 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1189
goto _LL0;}case 2U: _LL3: _LL4: {
# 1191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1192 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp37D;_push_handler(& _tmp37D);{int _tmp37F=0;if(setjmp(_tmp37D.handler))_tmp37F=1;if(!_tmp37F){x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp37E=(void*)Cyc_Core_get_exn_thrown();void*_tmp380=_tmp37E;void*_tmp381;if(((struct Cyc_Core_Failure_exn_struct*)_tmp380)->tag == Cyc_Core_Failure){_LL42E: _LL42F:
 x=0;goto _LL42D;}else{_LL430: _tmp381=_tmp380;_LL431:(int)_rethrow(_tmp381);}_LL42D:;}};}
{struct _handler_cons _tmp382;_push_handler(& _tmp382);{int _tmp384=0;if(setjmp(_tmp382.handler))_tmp384=1;if(!_tmp384){y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp383=(void*)Cyc_Core_get_exn_thrown();void*_tmp385=_tmp383;void*_tmp386;if(((struct Cyc_Core_Failure_exn_struct*)_tmp385)->tag == Cyc_Core_Failure){_LL433: _LL434:
 y=0;goto _LL432;}else{_LL435: _tmp386=_tmp385;_LL436:(int)_rethrow(_tmp386);}_LL432:;}};}
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1199
goto _LL0;}case 3U: _LL5: _LL6: {
# 1201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1202 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));({struct Cyc_Absyn_Decl*_tmp7F1=({struct Cyc_Absyn_Decl*_tmp388=_cycalloc(sizeof(*_tmp388));({void*_tmp7F0=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->tag=10U,({struct _tuple0*_tmp7EF=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp387->f1=_tmp7EF;}),({struct Cyc_List_List*_tmp7EE=Cyc_yyget_YY16(yyyyvsp[2]);_tmp387->f2=_tmp7EE;});_tmp387;});_tmp388->r=_tmp7F0;}),_tmp388->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp388;});_tmp389->hd=_tmp7F1;}),_tmp389->tl=0;_tmp389;}));
Cyc_Lex_leave_using();
# 1205
goto _LL0;}case 4U: _LL7: _LL8: {
# 1207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1206 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp38C=_cycalloc(sizeof(*_tmp38C));({struct Cyc_Absyn_Decl*_tmp7F6=({struct Cyc_Absyn_Decl*_tmp38B=_cycalloc(sizeof(*_tmp38B));({void*_tmp7F5=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A->tag=10U,({struct _tuple0*_tmp7F4=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp38A->f1=_tmp7F4;}),({struct Cyc_List_List*_tmp7F3=Cyc_yyget_YY16(yyyyvsp[2]);_tmp38A->f2=_tmp7F3;});_tmp38A;});_tmp38B->r=_tmp7F5;}),_tmp38B->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp38B;});_tmp38C->hd=_tmp7F6;}),({struct Cyc_List_List*_tmp7F2=Cyc_yyget_YY16(yyyyvsp[4]);_tmp38C->tl=_tmp7F2;});_tmp38C;}));
goto _LL0;}case 5U: _LL9: _LLA: {
# 1209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1209 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp390=_cycalloc(sizeof(*_tmp390));({struct Cyc_Absyn_Decl*_tmp7FB=({struct Cyc_Absyn_Decl*_tmp38F=_cycalloc(sizeof(*_tmp38F));({void*_tmp7FA=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->tag=9U,({struct _dyneither_ptr*_tmp7F9=({struct _dyneither_ptr*_tmp38D=_cycalloc(sizeof(*_tmp38D));({struct _dyneither_ptr _tmp7F8=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp38D=_tmp7F8;});_tmp38D;});_tmp38E->f1=_tmp7F9;}),({struct Cyc_List_List*_tmp7F7=Cyc_yyget_YY16(yyyyvsp[2]);_tmp38E->f2=_tmp7F7;});_tmp38E;});_tmp38F->r=_tmp7FA;}),_tmp38F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp38F;});_tmp390->hd=_tmp7FB;}),_tmp390->tl=0;_tmp390;}));
Cyc_Lex_leave_namespace();
# 1212
goto _LL0;}case 6U: _LLB: _LLC: {
# 1214
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1214 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp391;_push_handler(& _tmp391);{int _tmp393=0;if(setjmp(_tmp391.handler))_tmp393=1;if(!_tmp393){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp392=(void*)Cyc_Core_get_exn_thrown();void*_tmp394=_tmp392;void*_tmp396;if(((struct Cyc_Core_Failure_exn_struct*)_tmp394)->tag == Cyc_Core_Failure){_LL438: _LL439:
 nspace=({const char*_tmp395="";_tag_dyneither(_tmp395,sizeof(char),1U);});goto _LL437;}else{_LL43A: _tmp396=_tmp394;_LL43B:(int)_rethrow(_tmp396);}_LL437:;}};}
# 1219
{struct _handler_cons _tmp397;_push_handler(& _tmp397);{int _tmp399=0;if(setjmp(_tmp397.handler))_tmp399=1;if(!_tmp399){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp398=(void*)Cyc_Core_get_exn_thrown();void*_tmp39A=_tmp398;void*_tmp39B;if(((struct Cyc_Core_Failure_exn_struct*)_tmp39A)->tag == Cyc_Core_Failure){_LL43D: _LL43E:
 x=0;goto _LL43C;}else{_LL43F: _tmp39B=_tmp39A;_LL440:(int)_rethrow(_tmp39B);}_LL43C:;}};}
# 1222
{struct _handler_cons _tmp39C;_push_handler(& _tmp39C);{int _tmp39E=0;if(setjmp(_tmp39C.handler))_tmp39E=1;if(!_tmp39E){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp39D=(void*)Cyc_Core_get_exn_thrown();void*_tmp39F=_tmp39D;void*_tmp3A0;if(((struct Cyc_Core_Failure_exn_struct*)_tmp39F)->tag == Cyc_Core_Failure){_LL442: _LL443:
 y=0;goto _LL441;}else{_LL444: _tmp3A0=_tmp39F;_LL445:(int)_rethrow(_tmp3A0);}_LL441:;}};}
# 1225
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct Cyc_Absyn_Decl*_tmp7FE=({struct Cyc_Absyn_Decl*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({void*_tmp7FD=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->tag=9U,({struct _dyneither_ptr*_tmp7FC=({struct _dyneither_ptr*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));*_tmp3A1=nspace;_tmp3A1;});_tmp3A2->f1=_tmp7FC;}),_tmp3A2->f2=x;_tmp3A2;});_tmp3A3->r=_tmp7FD;}),_tmp3A3->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A3;});_tmp3A4->hd=_tmp7FE;}),_tmp3A4->tl=y;_tmp3A4;}));
# 1227
goto _LL0;}case 7U: _LLD: _LLE: {
# 1229
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1231
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1228 "parse.y"
int _tmp3A5=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(yyyyvsp[4]);
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[5]);
if(!_tmp3A5){
if(exs != 0  || cycdecls != 0){
({void*_tmp3A6=0U;({unsigned int _tmp800=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp7FF=({const char*_tmp3A7="expecting \"C include\"";_tag_dyneither(_tmp3A7,sizeof(char),22U);});Cyc_Warn_err(_tmp800,_tmp7FF,_tag_dyneither(_tmp3A6,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct Cyc_Absyn_Decl*_tmp804=({struct Cyc_Absyn_Decl*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({void*_tmp803=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->tag=12U,({struct Cyc_List_List*_tmp802=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A8->f1=_tmp802;}),_tmp3A8->f2=cycdecls,_tmp3A8->f3=exs;_tmp3A8;});_tmp3A9->r=_tmp803;}),_tmp3A9->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A9;});_tmp3AA->hd=_tmp804;}),({struct Cyc_List_List*_tmp801=Cyc_yyget_YY16(yyyyvsp[6]);_tmp3AA->tl=_tmp801;});_tmp3AA;}));}else{
# 1237
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct Cyc_Absyn_Decl*_tmp808=({struct Cyc_Absyn_Decl*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({void*_tmp807=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->tag=11U,({struct Cyc_List_List*_tmp806=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3AB->f1=_tmp806;});_tmp3AB;});_tmp3AC->r=_tmp807;}),_tmp3AC->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3AC;});_tmp3AD->hd=_tmp808;}),({struct Cyc_List_List*_tmp805=Cyc_yyget_YY16(yyyyvsp[6]);_tmp3AD->tl=_tmp805;});_tmp3AD;}));}}else{
# 1241
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));({struct Cyc_Absyn_Decl*_tmp80C=({struct Cyc_Absyn_Decl*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));({void*_tmp80B=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->tag=12U,({struct Cyc_List_List*_tmp80A=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3AE->f1=_tmp80A;}),_tmp3AE->f2=cycdecls,_tmp3AE->f3=exs;_tmp3AE;});_tmp3AF->r=_tmp80B;}),_tmp3AF->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3AF;});_tmp3B0->hd=_tmp80C;}),({struct Cyc_List_List*_tmp809=Cyc_yyget_YY16(yyyyvsp[6]);_tmp3B0->tl=_tmp809;});_tmp3B0;}));}
# 1244
goto _LL0;}case 8U: _LLF: _LL10: {
# 1246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1245 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct Cyc_Absyn_Decl*_tmp80E=({struct Cyc_Absyn_Decl*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3B1->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B1;});_tmp3B2->hd=_tmp80E;}),({struct Cyc_List_List*_tmp80D=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B2->tl=_tmp80D;});_tmp3B2;}));
goto _LL0;}case 9U: _LL11: _LL12: {
# 1248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1247 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));({struct Cyc_Absyn_Decl*_tmp810=({struct Cyc_Absyn_Decl*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3B3->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B3;});_tmp3B4->hd=_tmp810;}),({struct Cyc_List_List*_tmp80F=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B4->tl=_tmp80F;});_tmp3B4;}));
goto _LL0;}case 10U: _LL13: _LL14: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1249 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));({struct Cyc_Absyn_Decl*_tmp812=({struct Cyc_Absyn_Decl*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3B5->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B5;});_tmp3B6->hd=_tmp812;}),({struct Cyc_List_List*_tmp811=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B6->tl=_tmp811;});_tmp3B6;}));
goto _LL0;}case 11U: _LL15: _LL16: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1251 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct Cyc_Absyn_Decl*_tmp814=({struct Cyc_Absyn_Decl*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3B7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B7;});_tmp3B8->hd=_tmp814;}),({struct Cyc_List_List*_tmp813=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B8->tl=_tmp813;});_tmp3B8;}));
goto _LL0;}case 12U: _LL17: _LL18:
# 1254
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A: {
# 1257
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1259
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1257 "parse.y"
Cyc_Parse_parsing_tempest=1;
goto _LL0;}case 14U: _LL1B: _LL1C: {
# 1260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1262 "parse.y"
Cyc_Parse_parsing_tempest=0;
goto _LL0;}case 15U: _LL1D: _LL1E: {
# 1265
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1267 "parse.y"
struct _dyneither_ptr _tmp3B9=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({struct _dyneither_ptr _tmp815=(struct _dyneither_ptr)_tmp3B9;Cyc_strcmp(_tmp815,({const char*_tmp3BA="C";_tag_dyneither(_tmp3BA,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _dyneither_ptr _tmp816=(struct _dyneither_ptr)_tmp3B9;Cyc_strcmp(_tmp816,({const char*_tmp3BB="C include";_tag_dyneither(_tmp3BB,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1274
({void*_tmp3BC=0U;({unsigned int _tmp818=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp817=({const char*_tmp3BD="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3BD,sizeof(char),29U);});Cyc_Warn_err(_tmp818,_tmp817,_tag_dyneither(_tmp3BC,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1278
goto _LL0;}case 16U: _LL1F: _LL20: {
# 1280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1281 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL0;}case 17U: _LL21: _LL22:
# 1284
 yyval=Cyc_YY52(0);
goto _LL0;case 18U: _LL23: _LL24: {
# 1287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1286 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 19U: _LL25: _LL26: {
# 1289
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1291
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1290 "parse.y"
yyval=yyyyvsp[2];
goto _LL0;}case 20U: _LL27: _LL28: {
# 1293
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1291 "parse.y"
yyval=Cyc_YY52(0);
goto _LL0;}case 21U: _LL29: _LL2A: {
# 1294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1295 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct _tuple27*_tmp81A=({struct _tuple27*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp819=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3BE->f2=_tmp819;}),_tmp3BE->f3=0;_tmp3BE;});_tmp3BF->hd=_tmp81A;}),_tmp3BF->tl=0;_tmp3BF;}));
goto _LL0;}case 22U: _LL2B: _LL2C: {
# 1298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1296 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));({struct _tuple27*_tmp81C=({struct _tuple27*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp81B=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3C0->f2=_tmp81B;}),_tmp3C0->f3=0;_tmp3C0;});_tmp3C1->hd=_tmp81C;}),_tmp3C1->tl=0;_tmp3C1;}));
goto _LL0;}case 23U: _LL2D: _LL2E: {
# 1299
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1298 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct _tuple27*_tmp81F=({struct _tuple27*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp81E=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3C2->f2=_tmp81E;}),_tmp3C2->f3=0;_tmp3C2;});_tmp3C3->hd=_tmp81F;}),({struct Cyc_List_List*_tmp81D=Cyc_yyget_YY52(yyyyvsp[2]);_tmp3C3->tl=_tmp81D;});_tmp3C3;}));
goto _LL0;}case 24U: _LL2F: _LL30:
# 1301
 yyval=Cyc_YY16(0);
goto _LL0;case 25U: _LL31: _LL32: {
# 1304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1303 "parse.y"
yyval=yyyyvsp[2];
goto _LL0;}case 26U: _LL33: _LL34: {
# 1306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1307 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct Cyc_Absyn_Decl*_tmp822=({void*_tmp821=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp820=Cyc_yyget_YY15(yyyyvsp[0]);_tmp3C4->f1=_tmp820;});_tmp3C4;});Cyc_Absyn_new_decl(_tmp821,(unsigned int)(yyyylsp[0]).first_line);});_tmp3C5->hd=_tmp822;}),_tmp3C5->tl=0;_tmp3C5;}));
goto _LL0;}case 27U: _LL35: _LL36: {
# 1310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1308 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 28U: _LL37: _LL38: {
# 1311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1309 "parse.y"
yyval=Cyc_YY16(0);
goto _LL0;}case 31U: _LL39: _LL3A: {
# 1312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1318 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp825=yyr;struct Cyc_Parse_Declarator _tmp824=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp823=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp825,0,_tmp824,0,_tmp823,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 32U: _LL3B: _LL3C: {
# 1321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1320 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C6=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp828=yyr;struct Cyc_Parse_Declarator _tmp827=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp826=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp828,& _tmp3C6,_tmp827,0,_tmp826,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 33U: _LL3D: _LL3E: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1333 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp82C=yyr;struct Cyc_Parse_Declarator _tmp82B=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp82A=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp829=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp82C,0,_tmp82B,_tmp82A,_tmp829,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 34U: _LL3F: _LL40: {
# 1336
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1335 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C7=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp830=yyr;struct Cyc_Parse_Declarator _tmp82F=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp82E=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp82D=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp830,& _tmp3C7,_tmp82F,_tmp82E,_tmp82D,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 35U: _LL41: _LL42: {
# 1339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1343 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C8=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp833=yyr;struct Cyc_Parse_Declarator _tmp832=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp831=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp833,& _tmp3C8,_tmp832,0,_tmp831,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 36U: _LL43: _LL44: {
# 1347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1346 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C9=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp837=yyr;struct Cyc_Parse_Declarator _tmp836=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp835=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp834=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp837,& _tmp3C9,_tmp836,_tmp835,_tmp834,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 37U: _LL45: _LL46: {
# 1350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1351 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL0;}case 38U: _LL47: _LL48: {
# 1354
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1354 "parse.y"
Cyc_Lex_leave_using();
goto _LL0;}case 39U: _LL49: _LL4A: {
# 1357
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1359
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1357 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct _dyneither_ptr _tmp838=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp3CA=_tmp838;});_tmp3CA;}));yyval=yyyyvsp[1];
goto _LL0;}case 40U: _LL4B: _LL4C: {
# 1360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1360 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
int _tmp3CB=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp83A=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp839=(unsigned int)_tmp3CB;Cyc_Parse_make_declarations(_tmp83A,0,_tmp839,(unsigned int)_tmp3CB);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1369 "parse.y"
int _tmp3CC=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp83D=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp83C=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp83B=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp83D,_tmp83C,_tmp83B,(unsigned int)_tmp3CC);}));
goto _LL0;}case 43U: _LL51: _LL52: {
# 1373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1373 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));({struct Cyc_Absyn_Decl*_tmp840=({struct Cyc_Absyn_Pat*_tmp83F=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp83E=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp83F,_tmp83E,(unsigned int)(yyyylsp[0]).first_line);});_tmp3CD->hd=_tmp840;}),_tmp3CD->tl=0;_tmp3CD;}));
goto _LL0;}case 44U: _LL53: _LL54: {
# 1376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1375 "parse.y"
struct Cyc_List_List*_tmp3CE=0;
{struct Cyc_List_List*_tmp3CF=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp3CF != 0;_tmp3CF=_tmp3CF->tl){
struct _dyneither_ptr*_tmp3D0=(struct _dyneither_ptr*)_tmp3CF->hd;
struct _tuple0*qv=({struct _tuple0*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({union Cyc_Absyn_Nmspace _tmp841=Cyc_Absyn_Rel_n(0);_tmp3D3->f1=_tmp841;}),_tmp3D3->f2=_tmp3D0;_tmp3D3;});
struct Cyc_Absyn_Vardecl*_tmp3D1=({struct _tuple0*_tmp842=qv;Cyc_Absyn_new_vardecl(0U,_tmp842,Cyc_Absyn_wildtyp(0),0);});
_tmp3CE=({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->hd=_tmp3D1,_tmp3D2->tl=_tmp3CE;_tmp3D2;});}}
# 1382
_tmp3CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3CE);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct Cyc_Absyn_Decl*_tmp843=Cyc_Absyn_letv_decl(_tmp3CE,(unsigned int)(yyyylsp[0]).first_line);_tmp3D4->hd=_tmp843;}),_tmp3D4->tl=0;_tmp3D4;}));
# 1385
goto _LL0;}case 45U: _LL55: _LL56: {
# 1387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1388 "parse.y"
struct _dyneither_ptr _tmp3D5=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1390
if(({struct _dyneither_ptr _tmp844=(struct _dyneither_ptr)_tmp3D5;Cyc_zstrcmp(_tmp844,({const char*_tmp3D6="`H";_tag_dyneither(_tmp3D6,sizeof(char),3U);}));})== 0)
({void*_tmp3D7=0U;({unsigned int _tmp846=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp845=({const char*_tmp3D8="bad occurrence of heap region";_tag_dyneither(_tmp3D8,sizeof(char),30U);});Cyc_Warn_err(_tmp846,_tmp845,_tag_dyneither(_tmp3D7,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp847=(struct _dyneither_ptr)_tmp3D5;Cyc_zstrcmp(_tmp847,({const char*_tmp3D9="`U";_tag_dyneither(_tmp3D9,sizeof(char),3U);}));})== 0)
({void*_tmp3DA=0U;({unsigned int _tmp849=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp848=({const char*_tmp3DB="bad occurrence of unique region";_tag_dyneither(_tmp3DB,sizeof(char),32U);});Cyc_Warn_err(_tmp849,_tmp848,_tag_dyneither(_tmp3DA,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct _dyneither_ptr*_tmp84B=({struct _dyneither_ptr*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));*_tmp3DF=_tmp3D5;_tmp3DF;});_tmp3E0->name=_tmp84B;}),_tmp3E0->identity=- 1,({void*_tmp84A=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3E0->kind=_tmp84A;});_tmp3E0;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp84F=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp84E=({struct _tuple0*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp84D=({struct _dyneither_ptr*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct _dyneither_ptr _tmp84C=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp3DD=_tmp84C;});_tmp3DD;});_tmp3DE->f2=_tmp84D;});_tmp3DE;});Cyc_Absyn_new_vardecl(_tmp84F,_tmp84E,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));({struct Cyc_Absyn_Decl*_tmp850=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3DC->hd=_tmp850;}),_tmp3DC->tl=0;_tmp3DC;}));
# 1399
goto _LL0;};}case 46U: _LL57: _LL58: {
# 1401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1401 "parse.y"
struct _dyneither_ptr _tmp3E1=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({struct _dyneither_ptr _tmp851=(struct _dyneither_ptr)_tmp3E1;Cyc_zstrcmp(_tmp851,({const char*_tmp3E2="H";_tag_dyneither(_tmp3E2,sizeof(char),2U);}));})== 0)
({void*_tmp3E3=0U;({unsigned int _tmp853=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp852=({const char*_tmp3E4="bad occurrence of heap region `H";_tag_dyneither(_tmp3E4,sizeof(char),33U);});Cyc_Warn_err(_tmp853,_tmp852,_tag_dyneither(_tmp3E3,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp854=(struct _dyneither_ptr)_tmp3E1;Cyc_zstrcmp(_tmp854,({const char*_tmp3E5="U";_tag_dyneither(_tmp3E5,sizeof(char),2U);}));})== 0)
({void*_tmp3E6=0U;({unsigned int _tmp856=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp855=({const char*_tmp3E7="bad occurrence of unique region `U";_tag_dyneither(_tmp3E7,sizeof(char),35U);});Cyc_Warn_err(_tmp856,_tmp855,_tag_dyneither(_tmp3E6,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));({struct _dyneither_ptr*_tmp85A=({struct _dyneither_ptr*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));({struct _dyneither_ptr _tmp859=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3ED=({struct Cyc_String_pa_PrintArg_struct _tmp6F0;_tmp6F0.tag=0U,_tmp6F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E1);_tmp6F0;});void*_tmp3EB[1U];_tmp3EB[0]=& _tmp3ED;({struct _dyneither_ptr _tmp858=({const char*_tmp3EC="`%s";_tag_dyneither(_tmp3EC,sizeof(char),4U);});Cyc_aprintf(_tmp858,_tag_dyneither(_tmp3EB,sizeof(void*),1U));});});*_tmp3EE=_tmp859;});_tmp3EE;});_tmp3EF->name=_tmp85A;}),_tmp3EF->identity=- 1,({
void*_tmp857=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3EF->kind=_tmp857;});_tmp3EF;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp85D=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp85C=({struct _tuple0*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp85B=({struct _dyneither_ptr*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));*_tmp3E9=_tmp3E1;_tmp3E9;});_tmp3EA->f2=_tmp85B;});_tmp3EA;});Cyc_Absyn_new_vardecl(_tmp85D,_tmp85C,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));({struct Cyc_Absyn_Decl*_tmp85E=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3E8->hd=_tmp85E;}),_tmp3E8->tl=0;_tmp3E8;}));
# 1412
goto _LL0;};}case 47U: _LL59: _LL5A: {
# 1414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1414 "parse.y"
struct _dyneither_ptr _tmp3F0=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp3F1=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp3F2=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({struct _dyneither_ptr _tmp85F=(struct _dyneither_ptr)_tmp3F1;Cyc_strcmp(_tmp85F,({const char*_tmp3F3="open";_tag_dyneither(_tmp3F3,sizeof(char),5U);}));})!= 0)({void*_tmp3F4=0U;({unsigned int _tmp861=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp860=({const char*_tmp3F5="expecting `open'";_tag_dyneither(_tmp3F5,sizeof(char),17U);});Cyc_Warn_err(_tmp861,_tmp860,_tag_dyneither(_tmp3F4,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));({struct _dyneither_ptr*_tmp865=({struct _dyneither_ptr*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));({struct _dyneither_ptr _tmp864=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3FB=({struct Cyc_String_pa_PrintArg_struct _tmp6F1;_tmp6F1.tag=0U,_tmp6F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F0);_tmp6F1;});void*_tmp3F9[1U];_tmp3F9[0]=& _tmp3FB;({struct _dyneither_ptr _tmp863=({const char*_tmp3FA="`%s";_tag_dyneither(_tmp3FA,sizeof(char),4U);});Cyc_aprintf(_tmp863,_tag_dyneither(_tmp3F9,sizeof(void*),1U));});});*_tmp3FC=_tmp864;});_tmp3FC;});_tmp3FD->name=_tmp865;}),_tmp3FD->identity=- 1,({
void*_tmp862=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3FD->kind=_tmp862;});_tmp3FD;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp868=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp867=({struct _tuple0*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp866=({struct _dyneither_ptr*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));*_tmp3F7=_tmp3F0;_tmp3F7;});_tmp3F8->f2=_tmp866;});_tmp3F8;});Cyc_Absyn_new_vardecl(_tmp868,_tmp867,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));({struct Cyc_Absyn_Decl*_tmp869=Cyc_Absyn_region_decl(tv,vd,_tmp3F2,(unsigned int)(yyyylsp[0]).first_line);_tmp3F6->hd=_tmp869;}),_tmp3F6->tl=0;_tmp3F6;}));
# 1424
goto _LL0;};}case 48U: _LL5B: _LL5C: {
# 1426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1428 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 49U: _LL5D: _LL5E: {
# 1431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1430 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp86A=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp86A,Cyc_yyget_YY16(yyyyvsp[1]));}));
goto _LL0;}case 50U: _LL5F: _LL60: {
# 1433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F2;({enum Cyc_Parse_Storage_class*_tmp86D=Cyc_yyget_YY20(yyyyvsp[0]);_tmp6F2.sc=_tmp86D;}),({struct Cyc_Absyn_Tqual _tmp86C=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6F2.tq=_tmp86C;}),({
struct Cyc_Parse_Type_specifier _tmp86B=Cyc_Parse_empty_spec(0U);_tmp6F2.type_specs=_tmp86B;}),_tmp6F2.is_inline=0,_tmp6F2.attributes=0;_tmp6F2;}));
goto _LL0;}case 51U: _LL61: _LL62: {
# 1440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1439 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3FE=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp3FE.sc != 0)
({void*_tmp3FF=0U;({unsigned int _tmp86F=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp86E=({const char*_tmp400="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp400,sizeof(char),73U);});Cyc_Warn_warn(_tmp86F,_tmp86E,_tag_dyneither(_tmp3FF,sizeof(void*),0U));});});
# 1443
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F3;({enum Cyc_Parse_Storage_class*_tmp870=Cyc_yyget_YY20(yyyyvsp[0]);_tmp6F3.sc=_tmp870;}),_tmp6F3.tq=_tmp3FE.tq,_tmp6F3.type_specs=_tmp3FE.type_specs,_tmp6F3.is_inline=_tmp3FE.is_inline,_tmp6F3.attributes=_tmp3FE.attributes;_tmp6F3;}));
# 1447
goto _LL0;}case 52U: _LL63: _LL64: {
# 1449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1448 "parse.y"
({void*_tmp401=0U;({unsigned int _tmp872=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp871=({const char*_tmp402="__extension__ keyword ignored in declaration";_tag_dyneither(_tmp402,sizeof(char),45U);});Cyc_Warn_warn(_tmp872,_tmp871,_tag_dyneither(_tmp401,sizeof(void*),0U));});});
yyval=yyyyvsp[1];
# 1451
goto _LL0;}case 53U: _LL65: _LL66: {
# 1453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1452 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F4;_tmp6F4.sc=0,({struct Cyc_Absyn_Tqual _tmp874=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6F4.tq=_tmp874;}),({
struct Cyc_Parse_Type_specifier _tmp873=Cyc_yyget_YY21(yyyyvsp[0]);_tmp6F4.type_specs=_tmp873;}),_tmp6F4.is_inline=0,_tmp6F4.attributes=0;_tmp6F4;}));
goto _LL0;}case 54U: _LL67: _LL68: {
# 1456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1455 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp403=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F5;_tmp6F5.sc=_tmp403.sc,_tmp6F5.tq=_tmp403.tq,({
struct Cyc_Parse_Type_specifier _tmp877=({unsigned int _tmp876=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp875=_tmp403.type_specs;Cyc_Parse_combine_specifiers(_tmp876,_tmp875,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp6F5.type_specs=_tmp877;}),_tmp6F5.is_inline=_tmp403.is_inline,_tmp6F5.attributes=_tmp403.attributes;_tmp6F5;}));
# 1461
goto _LL0;}case 55U: _LL69: _LL6A: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1462 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F6;_tmp6F6.sc=0,({struct Cyc_Absyn_Tqual _tmp879=Cyc_yyget_YY23(yyyyvsp[0]);_tmp6F6.tq=_tmp879;}),({struct Cyc_Parse_Type_specifier _tmp878=Cyc_Parse_empty_spec(0U);_tmp6F6.type_specs=_tmp878;}),_tmp6F6.is_inline=0,_tmp6F6.attributes=0;_tmp6F6;}));
goto _LL0;}case 56U: _LL6B: _LL6C: {
# 1465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1464 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp404=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F7;_tmp6F7.sc=_tmp404.sc,({struct Cyc_Absyn_Tqual _tmp87B=({struct Cyc_Absyn_Tqual _tmp87A=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp87A,_tmp404.tq);});_tmp6F7.tq=_tmp87B;}),_tmp6F7.type_specs=_tmp404.type_specs,_tmp6F7.is_inline=_tmp404.is_inline,_tmp6F7.attributes=_tmp404.attributes;_tmp6F7;}));
# 1469
goto _LL0;}case 57U: _LL6D: _LL6E: {
# 1471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1470 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F8;_tmp6F8.sc=0,({struct Cyc_Absyn_Tqual _tmp87D=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6F8.tq=_tmp87D;}),({
struct Cyc_Parse_Type_specifier _tmp87C=Cyc_Parse_empty_spec(0U);_tmp6F8.type_specs=_tmp87C;}),_tmp6F8.is_inline=1,_tmp6F8.attributes=0;_tmp6F8;}));
goto _LL0;}case 58U: _LL6F: _LL70: {
# 1474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1473 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp405=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F9;_tmp6F9.sc=_tmp405.sc,_tmp6F9.tq=_tmp405.tq,_tmp6F9.type_specs=_tmp405.type_specs,_tmp6F9.is_inline=1,_tmp6F9.attributes=_tmp405.attributes;_tmp6F9;}));
# 1477
goto _LL0;}case 59U: _LL71: _LL72: {
# 1479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1478 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6FA;_tmp6FA.sc=0,({struct Cyc_Absyn_Tqual _tmp880=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6FA.tq=_tmp880;}),({
struct Cyc_Parse_Type_specifier _tmp87F=Cyc_Parse_empty_spec(0U);_tmp6FA.type_specs=_tmp87F;}),_tmp6FA.is_inline=0,({struct Cyc_List_List*_tmp87E=Cyc_yyget_YY45(yyyyvsp[0]);_tmp6FA.attributes=_tmp87E;});_tmp6FA;}));
goto _LL0;}case 60U: _LL73: _LL74: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1481 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp406=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6FB;_tmp6FB.sc=_tmp406.sc,_tmp6FB.tq=_tmp406.tq,_tmp6FB.type_specs=_tmp406.type_specs,_tmp6FB.is_inline=_tmp406.is_inline,({
# 1484
struct Cyc_List_List*_tmp882=({struct Cyc_List_List*_tmp881=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp881,_tmp406.attributes);});_tmp6FB.attributes=_tmp882;});_tmp6FB;}));
goto _LL0;}case 61U: _LL75: _LL76: {
# 1487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1488 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL0;}case 62U: _LL77: _LL78: {
# 1492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1490 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL0;}case 63U: _LL79: _LL7A: {
# 1494
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1496
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1492 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL0;}case 64U: _LL7B: _LL7C: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1494 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL0;}case 65U: _LL7D: _LL7E: {
# 1498
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1497 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _dyneither_ptr _tmp883=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmp883,({const char*_tmp407="C";_tag_dyneither(_tmp407,sizeof(char),2U);}));})!= 0)
({void*_tmp408=0U;({unsigned int _tmp885=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp884=({const char*_tmp409="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp409,sizeof(char),37U);});Cyc_Warn_err(_tmp885,_tmp884,_tag_dyneither(_tmp408,sizeof(void*),0U));});});
yyval=Cyc_YY20(& externC_sc);
# 1502
goto _LL0;}case 66U: _LL7F: _LL80: {
# 1504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1502 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL0;}case 67U: _LL81: _LL82: {
# 1506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1505 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL0;}case 68U: _LL83: _LL84:
# 1509
 yyval=Cyc_YY45(0);
goto _LL0;case 69U: _LL85: _LL86: {
# 1512
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1514
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1512 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 70U: _LL87: _LL88: {
# 1515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1517 "parse.y"
yyval=yyyyvsp[3];
goto _LL0;}case 71U: _LL89: _LL8A: {
# 1520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1521 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp40A=_cycalloc(sizeof(*_tmp40A));({void*_tmp886=Cyc_yyget_YY46(yyyyvsp[0]);_tmp40A->hd=_tmp886;}),_tmp40A->tl=0;_tmp40A;}));
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1524
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1522 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp40B=_cycalloc(sizeof(*_tmp40B));({void*_tmp888=Cyc_yyget_YY46(yyyyvsp[0]);_tmp40B->hd=_tmp888;}),({struct Cyc_List_List*_tmp887=Cyc_yyget_YY45(yyyyvsp[2]);_tmp40B->tl=_tmp887;});_tmp40B;}));
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1527 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp40F,_tmp40F,_tmp40F + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp410,_tmp410,_tmp410 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp411,_tmp411,_tmp411 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp412,_tmp412,_tmp412 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp413,_tmp413,_tmp413 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp414,_tmp414,_tmp414 + 8U},(void*)& att_aligned},{{_tmp415,_tmp415,_tmp415 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp416,_tmp416,_tmp416 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp417,_tmp417,_tmp417 + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp418,_tmp418,_tmp418 + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp419,_tmp419,_tmp419 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp41A,_tmp41A,_tmp41A + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp41B,_tmp41B,_tmp41B + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp41C,_tmp41C,_tmp41C + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp41D,_tmp41D,_tmp41D + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp41E,_tmp41E,_tmp41E + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp41F,_tmp41F,_tmp41F + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp420,_tmp420,_tmp420 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1548
struct _dyneither_ptr _tmp40C=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1550
if((((_get_dyneither_size(_tmp40C,sizeof(char))> (unsigned int)4  && (int)*((const char*)_check_dyneither_subscript(_tmp40C,sizeof(char),0))== (int)'_') && (int)*((const char*)_check_dyneither_subscript(_tmp40C,sizeof(char),1))== (int)'_') && (int)*((const char*)_check_dyneither_subscript(_tmp40C,sizeof(char),(int)(_get_dyneither_size(_tmp40C,sizeof(char))- (unsigned int)2)))== (int)'_') && (int)*((const char*)_check_dyneither_subscript(_tmp40C,sizeof(char),(int)(_get_dyneither_size(_tmp40C,sizeof(char))- (unsigned int)3)))== (int)'_')
# 1552
_tmp40C=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp40C,2,_get_dyneither_size(_tmp40C,sizeof(char))- (unsigned int)5);{
int i=0;
for(0;(unsigned int)i < 18U;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp40C,(struct _dyneither_ptr)(*((struct _tuple28*)_check_known_subscript_notnull(att_map,18U,sizeof(struct _tuple28),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1559
if((unsigned int)i == 18U){
({void*_tmp40D=0U;({unsigned int _tmp88A=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp889=({const char*_tmp40E="unrecognized attribute";_tag_dyneither(_tmp40E,sizeof(char),23U);});Cyc_Warn_err(_tmp88A,_tmp889,_tag_dyneither(_tmp40D,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1564
goto _LL0;};}case 74U: _LL8F: _LL90: {
# 1566
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1564 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;}case 75U: _LL91: _LL92: {
# 1567
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1569
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1566 "parse.y"
struct _dyneither_ptr _tmp421=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp422=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({struct _dyneither_ptr _tmp88C=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp88C,({const char*_tmp423="aligned";_tag_dyneither(_tmp423,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp88B=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp88B,({const char*_tmp424="__aligned__";_tag_dyneither(_tmp424,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->tag=6U,_tmp425->f1=_tmp422;_tmp425;});else{
if(({struct _dyneither_ptr _tmp88E=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp88E,({const char*_tmp426="section";_tag_dyneither(_tmp426,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp88D=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp88D,({const char*_tmp427="__section__";_tag_dyneither(_tmp427,sizeof(char),12U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp422);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->tag=8U,_tmp428->f1=str;_tmp428;});}else{
if(({struct _dyneither_ptr _tmp88F=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp88F,({const char*_tmp429="__mode__";_tag_dyneither(_tmp429,sizeof(char),9U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp422);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->tag=24U,_tmp42A->f1=str;_tmp42A;});}else{
if(({struct _dyneither_ptr _tmp890=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp890,({const char*_tmp42B="alias";_tag_dyneither(_tmp42B,sizeof(char),6U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp422);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->tag=25U,_tmp42C->f1=str;_tmp42C;});}else{
# 1582
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp422);
if(({struct _dyneither_ptr _tmp892=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp892,({const char*_tmp42D="regparm";_tag_dyneither(_tmp42D,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp891=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp891,({const char*_tmp42E="__regparm__";_tag_dyneither(_tmp42E,sizeof(char),12U);}));})== 0){
if(n < 0  || n > 3)
({void*_tmp42F=0U;({unsigned int _tmp894=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp893=({const char*_tmp430="regparm requires value between 0 and 3";_tag_dyneither(_tmp430,sizeof(char),39U);});Cyc_Warn_err(_tmp894,_tmp893,_tag_dyneither(_tmp42F,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->tag=0U,_tmp431->f1=n;_tmp431;});}else{
if(({struct _dyneither_ptr _tmp896=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp896,({const char*_tmp432="initializes";_tag_dyneither(_tmp432,sizeof(char),12U);}));})== 0  || ({struct _dyneither_ptr _tmp895=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp895,({const char*_tmp433="__initializes__";_tag_dyneither(_tmp433,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->tag=20U,_tmp434->f1=n;_tmp434;});else{
if(({struct _dyneither_ptr _tmp898=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp898,({const char*_tmp435="noliveunique";_tag_dyneither(_tmp435,sizeof(char),13U);}));})== 0  || ({struct _dyneither_ptr _tmp897=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp897,({const char*_tmp436="__noliveunique__";_tag_dyneither(_tmp436,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=21U,_tmp437->f1=n;_tmp437;});else{
if(({struct _dyneither_ptr _tmp89A=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp89A,({const char*_tmp438="consume";_tag_dyneither(_tmp438,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp899=(struct _dyneither_ptr)_tmp421;Cyc_zstrcmp(_tmp899,({const char*_tmp439="__consume__";_tag_dyneither(_tmp439,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=22U,_tmp43A->f1=n;_tmp43A;});else{
# 1594
({void*_tmp43B=0U;({unsigned int _tmp89C=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp89B=({const char*_tmp43C="unrecognized attribute";_tag_dyneither(_tmp43C,sizeof(char),23U);});Cyc_Warn_err(_tmp89C,_tmp89B,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1598
yyval=Cyc_YY46(a);
# 1600
goto _LL0;}case 76U: _LL93: _LL94: {
# 1602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1601 "parse.y"
struct _dyneither_ptr _tmp43D=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp43E=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp43F=({unsigned int _tmp89D=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmp89D,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp440=({unsigned int _tmp89E=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmp89E,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _dyneither_ptr _tmp8A0=(struct _dyneither_ptr)_tmp43D;Cyc_zstrcmp(_tmp8A0,({const char*_tmp441="format";_tag_dyneither(_tmp441,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp89F=(struct _dyneither_ptr)_tmp43D;Cyc_zstrcmp(_tmp89F,({const char*_tmp442="__format__";_tag_dyneither(_tmp442,sizeof(char),11U);}));})== 0){
if(({struct _dyneither_ptr _tmp8A2=(struct _dyneither_ptr)_tmp43E;Cyc_zstrcmp(_tmp8A2,({const char*_tmp443="printf";_tag_dyneither(_tmp443,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp8A1=(struct _dyneither_ptr)_tmp43E;Cyc_zstrcmp(_tmp8A1,({const char*_tmp444="__printf__";_tag_dyneither(_tmp444,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=19U,_tmp445->f1=Cyc_Absyn_Printf_ft,_tmp445->f2=(int)_tmp43F,_tmp445->f3=(int)_tmp440;_tmp445;});else{
if(({struct _dyneither_ptr _tmp8A4=(struct _dyneither_ptr)_tmp43E;Cyc_zstrcmp(_tmp8A4,({const char*_tmp446="scanf";_tag_dyneither(_tmp446,sizeof(char),6U);}));})== 0  || ({struct _dyneither_ptr _tmp8A3=(struct _dyneither_ptr)_tmp43E;Cyc_zstrcmp(_tmp8A3,({const char*_tmp447="__scanf__";_tag_dyneither(_tmp447,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->tag=19U,_tmp448->f1=Cyc_Absyn_Scanf_ft,_tmp448->f2=(int)_tmp43F,_tmp448->f3=(int)_tmp440;_tmp448;});else{
# 1612
({void*_tmp449=0U;({unsigned int _tmp8A6=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8A5=({const char*_tmp44A="unrecognized format type";_tag_dyneither(_tmp44A,sizeof(char),25U);});Cyc_Warn_err(_tmp8A6,_tmp8A5,_tag_dyneither(_tmp449,sizeof(void*),0U));});});}}}else{
# 1614
({void*_tmp44B=0U;({unsigned int _tmp8A8=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8A7=({const char*_tmp44C="unrecognized attribute";_tag_dyneither(_tmp44C,sizeof(char),23U);});Cyc_Warn_err(_tmp8A8,_tmp8A7,_tag_dyneither(_tmp44B,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1617
goto _LL0;}case 77U: _LL95: _LL96: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1626 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 78U: _LL97: _LL98: {
# 1629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1628 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AA=({struct _tuple0*_tmp8A9=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_typedef_type(_tmp8A9,Cyc_yyget_YY40(yyyyvsp[1]),0,0);});Cyc_Parse_type_spec(_tmp8AA,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 79U: _LL99: _LL9A: {
# 1631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1632 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 80U: _LL9B: _LL9C: {
# 1635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1633 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 81U: _LL9D: _LL9E: {
# 1636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1634 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1635 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1636 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 84U: _LLA3: _LLA4: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1637 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 85U: _LLA5: _LLA6: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1638 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 86U: _LLA7: _LLA8: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1639 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 87U: _LLA9: _LLAA: {
# 1642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1640 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 88U: _LLAB: _LLAC: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1641 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 89U: _LLAD: _LLAE: {
# 1644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1642 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 90U: _LLAF: _LLB0: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1645 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AB=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8AB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 91U: _LLB1: _LLB2: {
# 1648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1647 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AC=Cyc_Absyn_builtin_type(({const char*_tmp44D="__builtin_va_list";_tag_dyneither(_tmp44D,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp8AC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 92U: _LLB3: _LLB4: {
# 1650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1649 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 93U: _LLB5: _LLB6: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1651 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AD=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmp8AD,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 94U: _LLB7: _LLB8: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1653 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AE=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8AE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 95U: _LLB9: _LLBA: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1655 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AF=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);Cyc_Parse_type_spec(_tmp8AF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 96U: _LLBB: _LLBC: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1657 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B2=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->tag=6U,({struct Cyc_List_List*_tmp8B1=({unsigned int _tmp8B0=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8B0,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});
# 1657
_tmp44E->f1=_tmp8B1;});_tmp44E;});Cyc_Parse_type_spec(_tmp8B2,(unsigned int)(yyyylsp[0]).first_line);}));
# 1660
goto _LL0;}case 97U: _LLBD: _LLBE: {
# 1662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1661 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B3=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8B3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 98U: _LLBF: _LLC0: {
# 1664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1663 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B4=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp8B4,(unsigned int)(yyyylsp[0]).first_line);}));
# 1665
goto _LL0;}case 99U: _LLC1: _LLC2: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1666 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B5=Cyc_Absyn_tag_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8B5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 100U: _LLC3: _LLC4: {
# 1669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1668 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B6=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp8B6,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 101U: _LLC5: _LLC6: {
# 1671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1670 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B7=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8B7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 102U: _LLC7: _LLC8: {
# 1673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY43(({struct _dyneither_ptr _tmp8B8=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmp8B8,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 103U: _LLC9: _LLCA: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1680 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6FC;_tmp6FC.print_const=1,_tmp6FC.q_volatile=0,_tmp6FC.q_restrict=0,_tmp6FC.real_const=1,_tmp6FC.loc=loc;_tmp6FC;}));
goto _LL0;}case 104U: _LLCB: _LLCC: {
# 1684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1682 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6FD;_tmp6FD.print_const=0,_tmp6FD.q_volatile=1,_tmp6FD.q_restrict=0,_tmp6FD.real_const=0,_tmp6FD.loc=0U;_tmp6FD;}));
goto _LL0;}case 105U: _LLCD: _LLCE: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1683 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6FE;_tmp6FE.print_const=0,_tmp6FE.q_volatile=0,_tmp6FE.q_restrict=1,_tmp6FE.real_const=0,_tmp6FE.loc=0U;_tmp6FE;}));
goto _LL0;}case 106U: _LLCF: _LLD0: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
struct Cyc_Absyn_TypeDecl*_tmp44F=({struct Cyc_Absyn_TypeDecl*_tmp454=_cycalloc(sizeof(*_tmp454));({void*_tmp8BD=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8BC=({struct Cyc_Absyn_Enumdecl*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8BB=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp452->name=_tmp8BB;}),({struct Cyc_Core_Opt*_tmp8BA=({struct Cyc_Core_Opt*_tmp451=_cycalloc(sizeof(*_tmp451));({struct Cyc_List_List*_tmp8B9=Cyc_yyget_YY48(yyyyvsp[3]);_tmp451->v=_tmp8B9;});_tmp451;});_tmp452->fields=_tmp8BA;});_tmp452;});_tmp453->f1=_tmp8BC;});_tmp453;});_tmp454->r=_tmp8BD;}),_tmp454->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp454;});
# 1691
yyval=Cyc_YY21(({void*_tmp8BE=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->tag=10U,_tmp450->f1=_tmp44F,_tmp450->f2=0;_tmp450;});Cyc_Parse_type_spec(_tmp8BE,(unsigned int)(yyyylsp[0]).first_line);}));
# 1693
goto _LL0;}case 107U: _LLD1: _LLD2: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1694 "parse.y"
yyval=Cyc_YY21(({void*_tmp8BF=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(yyyyvsp[1]),0);Cyc_Parse_type_spec(_tmp8BF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 108U: _LLD3: _LLD4: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1696 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C0=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8C0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 109U: _LLD5: _LLD6: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp455=_cycalloc(sizeof(*_tmp455));({struct _tuple0*_tmp8C1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp455->name=_tmp8C1;}),_tmp455->tag=0,_tmp455->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp455;}));
goto _LL0;}case 110U: _LLD7: _LLD8: {
# 1705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1704 "parse.y"
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp456=_cycalloc(sizeof(*_tmp456));({struct _tuple0*_tmp8C3=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp456->name=_tmp8C3;}),({struct Cyc_Absyn_Exp*_tmp8C2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp456->tag=_tmp8C2;}),_tmp456->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp456;}));
goto _LL0;}case 111U: _LLD9: _LLDA: {
# 1707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1708 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp457=_cycalloc(sizeof(*_tmp457));({struct Cyc_Absyn_Enumfield*_tmp8C4=Cyc_yyget_YY47(yyyyvsp[0]);_tmp457->hd=_tmp8C4;}),_tmp457->tl=0;_tmp457;}));
goto _LL0;}case 112U: _LLDB: _LLDC: {
# 1711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1709 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp458=_cycalloc(sizeof(*_tmp458));({struct Cyc_Absyn_Enumfield*_tmp8C5=Cyc_yyget_YY47(yyyyvsp[0]);_tmp458->hd=_tmp8C5;}),_tmp458->tl=0;_tmp458;}));
goto _LL0;}case 113U: _LLDD: _LLDE: {
# 1712
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1714
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1710 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp459=_cycalloc(sizeof(*_tmp459));({struct Cyc_Absyn_Enumfield*_tmp8C7=Cyc_yyget_YY47(yyyyvsp[0]);_tmp459->hd=_tmp8C7;}),({struct Cyc_List_List*_tmp8C6=Cyc_yyget_YY48(yyyyvsp[2]);_tmp459->tl=_tmp8C6;});_tmp459;}));
goto _LL0;}case 114U: _LLDF: _LLE0: {
# 1713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
yyval=Cyc_YY21(({void*_tmp8CA=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->tag=7U,({enum Cyc_Absyn_AggrKind _tmp8C9=Cyc_yyget_YY22(yyyyvsp[0]);_tmp45A->f1=_tmp8C9;}),({struct Cyc_List_List*_tmp8C8=Cyc_yyget_YY24(yyyyvsp[2]);_tmp45A->f2=_tmp8C8;});_tmp45A;});Cyc_Parse_type_spec(_tmp8CA,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 115U: _LLE1: _LLE2: {
# 1719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 1722 "parse.y"
struct Cyc_List_List*_tmp45B=({unsigned int _tmp8CB=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8CB,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp45C=({unsigned int _tmp8CC=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8CC,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp45D=({enum Cyc_Absyn_AggrKind _tmp8D2=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp8D1=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmp8D0=_tmp45B;struct Cyc_Absyn_AggrdeclImpl*_tmp8CF=({
struct Cyc_List_List*_tmp8CE=_tmp45C;struct Cyc_List_List*_tmp8CD=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmp8CE,_tmp8CD,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1724
Cyc_Absyn_aggr_tdecl(_tmp8D2,Cyc_Absyn_Public,_tmp8D1,_tmp8D0,_tmp8CF,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1727
yyval=Cyc_YY21(({void*_tmp8D3=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->tag=10U,_tmp45E->f1=_tmp45D,_tmp45E->f2=0;_tmp45E;});Cyc_Parse_type_spec(_tmp8D3,(unsigned int)(yyyylsp[0]).first_line);}));
# 1729
goto _LL0;}case 116U: _LLE3: _LLE4: {
# 1731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1733 "parse.y"
struct Cyc_List_List*_tmp45F=({unsigned int _tmp8D4=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8D4,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp460=({unsigned int _tmp8D5=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8D5,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp461=({enum Cyc_Absyn_AggrKind _tmp8DB=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmp8DA=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp8D9=_tmp45F;struct Cyc_Absyn_AggrdeclImpl*_tmp8D8=({
struct Cyc_List_List*_tmp8D7=_tmp460;struct Cyc_List_List*_tmp8D6=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmp8D7,_tmp8D6,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1735
Cyc_Absyn_aggr_tdecl(_tmp8DB,Cyc_Absyn_Public,_tmp8DA,_tmp8D9,_tmp8D8,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1738
yyval=Cyc_YY21(({void*_tmp8DC=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->tag=10U,_tmp462->f1=_tmp461,_tmp462->f2=0;_tmp462;});Cyc_Parse_type_spec(_tmp8DC,(unsigned int)(yyyylsp[0]).first_line);}));
# 1740
goto _LL0;}case 117U: _LLE5: _LLE6: {
# 1742
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1741 "parse.y"
yyval=Cyc_YY21(({void*_tmp8E0=({union Cyc_Absyn_AggrInfo _tmp8DF=({enum Cyc_Absyn_AggrKind _tmp8DE=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp8DD=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmp8DE,_tmp8DD,({struct Cyc_Core_Opt*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->v=(void*)1;_tmp463;}));});Cyc_Absyn_aggr_type(_tmp8DF,Cyc_yyget_YY40(yyyyvsp[3]));});Cyc_Parse_type_spec(_tmp8E0,(unsigned int)(yyyylsp[0]).first_line);}));
# 1744
goto _LL0;}case 118U: _LLE7: _LLE8: {
# 1746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1745 "parse.y"
yyval=Cyc_YY21(({void*_tmp8E3=({union Cyc_Absyn_AggrInfo _tmp8E2=({enum Cyc_Absyn_AggrKind _tmp8E1=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmp8E1,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});Cyc_Absyn_aggr_type(_tmp8E2,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp8E3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 119U: _LLE9: _LLEA:
# 1748
 yyval=Cyc_YY40(0);
goto _LL0;case 120U: _LLEB: _LLEC: {
# 1751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1752 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 121U: _LLED: _LLEE: {
# 1755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1756 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1757 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;}case 123U: _LLF1: _LLF2:
# 1760
 yyval=Cyc_YY24(0);
goto _LL0;case 124U: _LLF3: _LLF4: {
# 1763
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1765
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1764 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp464=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp464 != 0;_tmp464=_tmp464->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp464->hd,decls);}}{
# 1768
struct Cyc_List_List*_tmp465=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp465 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp465,decls);
yyval=Cyc_YY24(decls);
# 1773
goto _LL0;};}case 125U: _LLF5: _LLF6: {
# 1775
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp466=_cycalloc(sizeof(*_tmp466));({struct Cyc_List_List*_tmp8E4=Cyc_yyget_YY24(yyyyvsp[0]);_tmp466->hd=_tmp8E4;}),_tmp466->tl=0;_tmp466;}));
goto _LL0;}case 126U: _LLF7: _LLF8: {
# 1781
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1783
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1780 "parse.y"
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_List_List*_tmp8E6=Cyc_yyget_YY24(yyyyvsp[1]);_tmp467->hd=_tmp8E6;}),({struct Cyc_List_List*_tmp8E5=Cyc_yyget_YY25(yyyyvsp[0]);_tmp467->tl=_tmp8E5;});_tmp467;}));
goto _LL0;}case 127U: _LLF9: _LLFA: {
# 1783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1784 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
goto _LL0;}case 128U: _LLFB: _LLFC: {
# 1787
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1789
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY19(({struct _tuple12*_tmp468=_region_malloc(yyr,sizeof(*_tmp468));_tmp468->tl=0,({struct _tuple11 _tmp8E7=Cyc_yyget_YY18(yyyyvsp[0]);_tmp468->hd=_tmp8E7;});_tmp468;}));
goto _LL0;}case 129U: _LLFD: _LLFE: {
# 1793
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1795
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1792 "parse.y"
yyval=Cyc_YY19(({struct _tuple12*_tmp469=_region_malloc(yyr,sizeof(*_tmp469));({struct _tuple12*_tmp8E9=Cyc_yyget_YY19(yyyyvsp[0]);_tmp469->tl=_tmp8E9;}),({struct _tuple11 _tmp8E8=Cyc_yyget_YY18(yyyyvsp[2]);_tmp469->hd=_tmp8E8;});_tmp469;}));
goto _LL0;}case 130U: _LLFF: _LL100: {
# 1795
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1797
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1797 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp6FF;({struct Cyc_Parse_Declarator _tmp8EA=Cyc_yyget_YY27(yyyyvsp[0]);_tmp6FF.f1=_tmp8EA;}),_tmp6FF.f2=0;_tmp6FF;}));
goto _LL0;}case 131U: _LL101: _LL102: {
# 1800
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1799 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp700;({struct Cyc_Parse_Declarator _tmp8EC=Cyc_yyget_YY27(yyyyvsp[0]);_tmp700.f1=_tmp8EC;}),({struct Cyc_Absyn_Exp*_tmp8EB=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp700.f2=_tmp8EB;});_tmp700;}));
goto _LL0;}case 132U: _LL103: _LL104: {
# 1802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1805 "parse.y"
struct _RegionHandle _tmp46A=_new_region("temp");struct _RegionHandle*temp=& _tmp46A;_push_region(temp);
{struct _tuple25 _tmp46B=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp46C=_tmp46B;struct Cyc_Absyn_Tqual _tmp47A;struct Cyc_Parse_Type_specifier _tmp479;struct Cyc_List_List*_tmp478;_LL447: _tmp47A=_tmp46C.f1;_tmp479=_tmp46C.f2;_tmp478=_tmp46C.f3;_LL448:;
if(_tmp47A.loc == (unsigned int)0)_tmp47A.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp46D=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp46D != 0;_tmp46D=_tmp46D->tl){
struct _tuple24*_tmp46E=(struct _tuple24*)_tmp46D->hd;struct _tuple24*_tmp46F=_tmp46E;struct Cyc_Parse_Declarator _tmp475;struct Cyc_Absyn_Exp*_tmp474;struct Cyc_Absyn_Exp*_tmp473;_LL44A: _tmp475=_tmp46F->f1;_tmp474=_tmp46F->f2;_tmp473=_tmp46F->f3;_LL44B:;
decls=({struct _tuple10*_tmp470=_region_malloc(temp,sizeof(*_tmp470));_tmp470->tl=decls,_tmp470->hd=_tmp475;_tmp470;});
widths_and_reqs=({struct Cyc_List_List*_tmp472=_region_malloc(temp,sizeof(*_tmp472));
({struct _tuple16*_tmp8ED=({struct _tuple16*_tmp471=_region_malloc(temp,sizeof(*_tmp471));_tmp471->f1=_tmp474,_tmp471->f2=_tmp473;_tmp471;});_tmp472->hd=_tmp8ED;}),_tmp472->tl=widths_and_reqs;_tmp472;});}}
# 1816
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp476=Cyc_Parse_speclist2typ(_tmp479,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp477=({struct _RegionHandle*_tmp8F0=temp;struct _RegionHandle*_tmp8EF=temp;struct Cyc_List_List*_tmp8EE=
Cyc_Parse_apply_tmss(temp,_tmp47A,_tmp476,decls,_tmp478);
# 1819
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp8F0,_tmp8EF,_tmp8EE,widths_and_reqs);});
# 1822
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp477));
# 1824
_npop_handler(0U);goto _LL0;};};}
# 1806
;_pop_region(temp);}case 133U: _LL105: _LL106: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1832 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp701;({struct Cyc_Absyn_Tqual _tmp8F2=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp701.f1=_tmp8F2;}),({struct Cyc_Parse_Type_specifier _tmp8F1=Cyc_yyget_YY21(yyyyvsp[0]);_tmp701.f2=_tmp8F1;}),_tmp701.f3=0;_tmp701;}));
goto _LL0;}case 134U: _LL107: _LL108: {
# 1835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1834 "parse.y"
struct _tuple25 _tmp47B=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp702;_tmp702.f1=_tmp47B.f1,({struct Cyc_Parse_Type_specifier _tmp8F5=({unsigned int _tmp8F4=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp8F3=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp8F4,_tmp8F3,_tmp47B.f2);});_tmp702.f2=_tmp8F5;}),_tmp702.f3=_tmp47B.f3;_tmp702;}));
goto _LL0;}case 135U: _LL109: _LL10A: {
# 1837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1836 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp703;({struct Cyc_Absyn_Tqual _tmp8F7=Cyc_yyget_YY23(yyyyvsp[0]);_tmp703.f1=_tmp8F7;}),({struct Cyc_Parse_Type_specifier _tmp8F6=Cyc_Parse_empty_spec(0U);_tmp703.f2=_tmp8F6;}),_tmp703.f3=0;_tmp703;}));
goto _LL0;}case 136U: _LL10B: _LL10C: {
# 1839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1838 "parse.y"
struct _tuple25 _tmp47C=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp704;({struct Cyc_Absyn_Tqual _tmp8F9=({struct Cyc_Absyn_Tqual _tmp8F8=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp8F8,_tmp47C.f1);});_tmp704.f1=_tmp8F9;}),_tmp704.f2=_tmp47C.f2,_tmp704.f3=_tmp47C.f3;_tmp704;}));
goto _LL0;}case 137U: _LL10D: _LL10E: {
# 1842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1841 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp705;({struct Cyc_Absyn_Tqual _tmp8FC=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp705.f1=_tmp8FC;}),({struct Cyc_Parse_Type_specifier _tmp8FB=Cyc_Parse_empty_spec(0U);_tmp705.f2=_tmp8FB;}),({struct Cyc_List_List*_tmp8FA=Cyc_yyget_YY45(yyyyvsp[0]);_tmp705.f3=_tmp8FA;});_tmp705;}));
goto _LL0;}case 138U: _LL10F: _LL110: {
# 1844
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1843 "parse.y"
struct _tuple25 _tmp47D=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp706;_tmp706.f1=_tmp47D.f1,_tmp706.f2=_tmp47D.f2,({struct Cyc_List_List*_tmp8FE=({struct Cyc_List_List*_tmp8FD=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8FD,_tmp47D.f3);});_tmp706.f3=_tmp8FE;});_tmp706;}));
goto _LL0;}case 139U: _LL111: _LL112: {
# 1846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY35(({struct _tuple25 _tmp707;({struct Cyc_Absyn_Tqual _tmp900=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp707.f1=_tmp900;}),({struct Cyc_Parse_Type_specifier _tmp8FF=Cyc_yyget_YY21(yyyyvsp[0]);_tmp707.f2=_tmp8FF;}),_tmp707.f3=0;_tmp707;}));
goto _LL0;}case 140U: _LL113: _LL114: {
# 1852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1851 "parse.y"
struct _tuple25 _tmp47E=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp708;_tmp708.f1=_tmp47E.f1,({struct Cyc_Parse_Type_specifier _tmp903=({unsigned int _tmp902=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp901=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp902,_tmp901,_tmp47E.f2);});_tmp708.f2=_tmp903;}),_tmp708.f3=_tmp47E.f3;_tmp708;}));
goto _LL0;}case 141U: _LL115: _LL116: {
# 1854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1853 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp709;({struct Cyc_Absyn_Tqual _tmp905=Cyc_yyget_YY23(yyyyvsp[0]);_tmp709.f1=_tmp905;}),({struct Cyc_Parse_Type_specifier _tmp904=Cyc_Parse_empty_spec(0U);_tmp709.f2=_tmp904;}),_tmp709.f3=0;_tmp709;}));
goto _LL0;}case 142U: _LL117: _LL118: {
# 1856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1855 "parse.y"
struct _tuple25 _tmp47F=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp70A;({struct Cyc_Absyn_Tqual _tmp907=({struct Cyc_Absyn_Tqual _tmp906=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp906,_tmp47F.f1);});_tmp70A.f1=_tmp907;}),_tmp70A.f2=_tmp47F.f2,_tmp70A.f3=_tmp47F.f3;_tmp70A;}));
goto _LL0;}case 143U: _LL119: _LL11A: {
# 1859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1858 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp70B;({struct Cyc_Absyn_Tqual _tmp90A=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp70B.f1=_tmp90A;}),({struct Cyc_Parse_Type_specifier _tmp909=Cyc_Parse_empty_spec(0U);_tmp70B.f2=_tmp909;}),({struct Cyc_List_List*_tmp908=Cyc_yyget_YY45(yyyyvsp[0]);_tmp70B.f3=_tmp908;});_tmp70B;}));
goto _LL0;}case 144U: _LL11B: _LL11C: {
# 1861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1860 "parse.y"
struct _tuple25 _tmp480=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp70C;_tmp70C.f1=_tmp480.f1,_tmp70C.f2=_tmp480.f2,({struct Cyc_List_List*_tmp90C=({struct Cyc_List_List*_tmp90B=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp90B,_tmp480.f3);});_tmp70C.f3=_tmp90C;});_tmp70C;}));
goto _LL0;}case 145U: _LL11D: _LL11E: {
# 1863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1864 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
goto _LL0;}case 146U: _LL11F: _LL120: {
# 1867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp481=_region_malloc(yyr,sizeof(*_tmp481));({struct _tuple24*_tmp90D=Cyc_yyget_YY28(yyyyvsp[0]);_tmp481->hd=_tmp90D;}),_tmp481->tl=0;_tmp481;}));
goto _LL0;}case 147U: _LL121: _LL122: {
# 1873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1872 "parse.y"
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp482=_region_malloc(yyr,sizeof(*_tmp482));({struct _tuple24*_tmp90F=Cyc_yyget_YY28(yyyyvsp[2]);_tmp482->hd=_tmp90F;}),({struct Cyc_List_List*_tmp90E=Cyc_yyget_YY29(yyyyvsp[0]);_tmp482->tl=_tmp90E;});_tmp482;}));
goto _LL0;}case 148U: _LL123: _LL124: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1877 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp483=_region_malloc(yyr,sizeof(*_tmp483));({struct Cyc_Parse_Declarator _tmp911=Cyc_yyget_YY27(yyyyvsp[0]);_tmp483->f1=_tmp911;}),_tmp483->f2=0,({struct Cyc_Absyn_Exp*_tmp910=Cyc_yyget_YY55(yyyyvsp[1]);_tmp483->f3=_tmp910;});_tmp483;}));
goto _LL0;}case 149U: _LL125: _LL126: {
# 1880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1881 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp487=_region_malloc(yyr,sizeof(*_tmp487));({struct _tuple0*_tmp916=({struct _tuple0*_tmp486=_cycalloc(sizeof(*_tmp486));({union Cyc_Absyn_Nmspace _tmp915=Cyc_Absyn_Rel_n(0);_tmp486->f1=_tmp915;}),({struct _dyneither_ptr*_tmp914=({struct _dyneither_ptr*_tmp485=_cycalloc(sizeof(*_tmp485));({struct _dyneither_ptr _tmp913=({const char*_tmp484="";_tag_dyneither(_tmp484,sizeof(char),1U);});*_tmp485=_tmp913;});_tmp485;});_tmp486->f2=_tmp914;});_tmp486;});(_tmp487->f1).id=_tmp916;}),(_tmp487->f1).varloc=0U,(_tmp487->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp912=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp487->f2=_tmp912;}),_tmp487->f3=0;_tmp487;}));
# 1883
goto _LL0;}case 150U: _LL127: _LL128:
# 1887
 yyval=Cyc_YY28(({struct _tuple24*_tmp48B=_region_malloc(yyr,sizeof(*_tmp48B));({struct _tuple0*_tmp91A=({struct _tuple0*_tmp48A=_cycalloc(sizeof(*_tmp48A));({union Cyc_Absyn_Nmspace _tmp919=Cyc_Absyn_Rel_n(0);_tmp48A->f1=_tmp919;}),({struct _dyneither_ptr*_tmp918=({struct _dyneither_ptr*_tmp489=_cycalloc(sizeof(*_tmp489));({struct _dyneither_ptr _tmp917=({const char*_tmp488="";_tag_dyneither(_tmp488,sizeof(char),1U);});*_tmp489=_tmp917;});_tmp489;});_tmp48A->f2=_tmp918;});_tmp48A;});(_tmp48B->f1).id=_tmp91A;}),(_tmp48B->f1).varloc=0U,(_tmp48B->f1).tms=0,_tmp48B->f2=0,_tmp48B->f3=0;_tmp48B;}));
# 1889
goto _LL0;case 151U: _LL129: _LL12A: {
# 1891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1889 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp48C=_region_malloc(yyr,sizeof(*_tmp48C));({struct Cyc_Parse_Declarator _tmp91C=Cyc_yyget_YY27(yyyyvsp[0]);_tmp48C->f1=_tmp91C;}),({struct Cyc_Absyn_Exp*_tmp91B=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp48C->f2=_tmp91B;}),_tmp48C->f3=0;_tmp48C;}));
goto _LL0;}case 152U: _LL12B: _LL12C: {
# 1892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1893 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL0;}case 153U: _LL12D: _LL12E:
# 1896
 yyval=Cyc_YY55(0);
goto _LL0;case 154U: _LL12F: _LL130: {
# 1899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1898 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL0;}case 155U: _LL131: _LL132:
# 1901
 yyval=Cyc_YY55(0);
goto _LL0;case 156U: _LL133: _LL134: {
# 1904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1905 "parse.y"
int _tmp48D=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp48E=({unsigned int _tmp91D=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp91D,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp48F=({struct _tuple0*_tmp922=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp921=_tmp48E;struct Cyc_Core_Opt*_tmp920=({struct Cyc_Core_Opt*_tmp491=_cycalloc(sizeof(*_tmp491));({struct Cyc_List_List*_tmp91E=Cyc_yyget_YY34(yyyyvsp[4]);_tmp491->v=_tmp91E;});_tmp491;});int _tmp91F=_tmp48D;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp922,_tmp921,_tmp920,_tmp91F,(unsigned int)(yyyylsp[0]).first_line);});
# 1909
yyval=Cyc_YY21(({void*_tmp923=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->tag=10U,_tmp490->f1=_tmp48F,_tmp490->f2=0;_tmp490;});Cyc_Parse_type_spec(_tmp923,(unsigned int)(yyyylsp[0]).first_line);}));
# 1911
goto _LL0;}case 157U: _LL135: _LL136: {
# 1913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1912 "parse.y"
int _tmp492=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(({void*_tmp926=({union Cyc_Absyn_DatatypeInfo _tmp925=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp70D;({struct _tuple0*_tmp924=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp70D.name=_tmp924;}),_tmp70D.is_extensible=_tmp492;_tmp70D;}));Cyc_Absyn_datatype_type(_tmp925,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp926,(unsigned int)(yyyylsp[0]).first_line);}));
# 1915
goto _LL0;}case 158U: _LL137: _LL138: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1916 "parse.y"
int _tmp493=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(({void*_tmp92A=({union Cyc_Absyn_DatatypeFieldInfo _tmp929=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp70E;({struct _tuple0*_tmp928=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp70E.datatype_name=_tmp928;}),({struct _tuple0*_tmp927=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp70E.field_name=_tmp927;}),_tmp70E.is_extensible=_tmp493;_tmp70E;}));Cyc_Absyn_datatype_field_type(_tmp929,Cyc_yyget_YY40(yyyyvsp[4]));});Cyc_Parse_type_spec(_tmp92A,(unsigned int)(yyyylsp[0]).first_line);}));
# 1919
goto _LL0;}case 159U: _LL139: _LL13A: {
# 1921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1922 "parse.y"
yyval=Cyc_YY31(0);
goto _LL0;}case 160U: _LL13B: _LL13C: {
# 1925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1923 "parse.y"
yyval=Cyc_YY31(1);
goto _LL0;}case 161U: _LL13D: _LL13E: {
# 1926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1927 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp494=_cycalloc(sizeof(*_tmp494));({struct Cyc_Absyn_Datatypefield*_tmp92B=Cyc_yyget_YY33(yyyyvsp[0]);_tmp494->hd=_tmp92B;}),_tmp494->tl=0;_tmp494;}));
goto _LL0;}case 162U: _LL13F: _LL140: {
# 1930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1928 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp495=_cycalloc(sizeof(*_tmp495));({struct Cyc_Absyn_Datatypefield*_tmp92C=Cyc_yyget_YY33(yyyyvsp[0]);_tmp495->hd=_tmp92C;}),_tmp495->tl=0;_tmp495;}));
goto _LL0;}case 163U: _LL141: _LL142: {
# 1931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1929 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));({struct Cyc_Absyn_Datatypefield*_tmp92E=Cyc_yyget_YY33(yyyyvsp[0]);_tmp496->hd=_tmp92E;}),({struct Cyc_List_List*_tmp92D=Cyc_yyget_YY34(yyyyvsp[2]);_tmp496->tl=_tmp92D;});_tmp496;}));
goto _LL0;}case 164U: _LL143: _LL144: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1930 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp497=_cycalloc(sizeof(*_tmp497));({struct Cyc_Absyn_Datatypefield*_tmp930=Cyc_yyget_YY33(yyyyvsp[0]);_tmp497->hd=_tmp930;}),({struct Cyc_List_List*_tmp92F=Cyc_yyget_YY34(yyyyvsp[2]);_tmp497->tl=_tmp92F;});_tmp497;}));
goto _LL0;}case 165U: _LL145: _LL146:
# 1933
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 166U: _LL147: _LL148: {
# 1936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1935 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;}case 167U: _LL149: _LL14A: {
# 1938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1936 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;}case 168U: _LL14B: _LL14C: {
# 1939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1940 "parse.y"
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp498=_cycalloc(sizeof(*_tmp498));({struct _tuple0*_tmp932=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp498->name=_tmp932;}),_tmp498->typs=0,_tmp498->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp931=Cyc_yyget_YY32(yyyyvsp[0]);_tmp498->sc=_tmp931;});_tmp498;}));
goto _LL0;}case 169U: _LL14D: _LL14E: {
# 1943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1942 "parse.y"
struct Cyc_List_List*_tmp499=({unsigned int _tmp933=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp933,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp49A=_cycalloc(sizeof(*_tmp49A));({struct _tuple0*_tmp935=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp49A->name=_tmp935;}),_tmp49A->typs=_tmp499,_tmp49A->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp934=Cyc_yyget_YY32(yyyyvsp[0]);_tmp49A->sc=_tmp934;});_tmp49A;}));
goto _LL0;}case 170U: _LL14F: _LL150: {
# 1946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1948 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 171U: _LL151: _LL152: {
# 1951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1950 "parse.y"
struct Cyc_Parse_Declarator _tmp49B=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp70F;_tmp70F.id=_tmp49B.id,_tmp70F.varloc=_tmp49B.varloc,({struct Cyc_List_List*_tmp937=({struct Cyc_List_List*_tmp936=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp936,_tmp49B.tms);});_tmp70F.tms=_tmp937;});_tmp70F;}));
goto _LL0;}case 172U: _LL153: _LL154: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 173U: _LL155: _LL156: {
# 1960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1959 "parse.y"
struct Cyc_Parse_Declarator _tmp49C=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp710;_tmp710.id=_tmp49C.id,_tmp710.varloc=_tmp49C.varloc,({struct Cyc_List_List*_tmp939=({struct Cyc_List_List*_tmp938=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp938,_tmp49C.tms);});_tmp710.tms=_tmp939;});_tmp710;}));
goto _LL0;}case 174U: _LL157: _LL158: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1965 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp711;({struct _tuple0*_tmp93A=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp711.id=_tmp93A;}),_tmp711.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp711.tms=0;_tmp711;}));
goto _LL0;}case 175U: _LL159: _LL15A: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1967 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 176U: _LL15B: _LL15C: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1971 "parse.y"
struct Cyc_Parse_Declarator _tmp49D=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmp93D=({struct Cyc_List_List*_tmp49F=_region_malloc(yyr,sizeof(*_tmp49F));({void*_tmp93C=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp49E=_region_malloc(yyr,sizeof(*_tmp49E));_tmp49E->tag=5U,_tmp49E->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp93B=Cyc_yyget_YY45(yyyyvsp[1]);_tmp49E->f2=_tmp93B;});_tmp49E;});_tmp49F->hd=_tmp93C;}),_tmp49F->tl=_tmp49D.tms;_tmp49F;});_tmp49D.tms=_tmp93D;});
yyval=yyyyvsp[2];
# 1975
goto _LL0;}case 177U: _LL15D: _LL15E: {
# 1977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1976 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp712;({struct _tuple0*_tmp943=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp712.id=_tmp943;}),({unsigned int _tmp942=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp712.varloc=_tmp942;}),({struct Cyc_List_List*_tmp941=({struct Cyc_List_List*_tmp4A1=_region_malloc(yyr,sizeof(*_tmp4A1));({void*_tmp940=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4A0=_region_malloc(yyr,sizeof(*_tmp4A0));_tmp4A0->tag=0U,({void*_tmp93F=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4A0->f1=_tmp93F;}),_tmp4A0->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4A0;});_tmp4A1->hd=_tmp940;}),({struct Cyc_List_List*_tmp93E=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4A1->tl=_tmp93E;});_tmp4A1;});_tmp712.tms=_tmp941;});_tmp712;}));
goto _LL0;}case 178U: _LL15F: _LL160: {
# 1979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1978 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp713;({struct _tuple0*_tmp94A=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp713.id=_tmp94A;}),({unsigned int _tmp949=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp713.varloc=_tmp949;}),({
struct Cyc_List_List*_tmp948=({struct Cyc_List_List*_tmp4A3=_region_malloc(yyr,sizeof(*_tmp4A3));({void*_tmp947=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4A2=_region_malloc(yyr,sizeof(*_tmp4A2));_tmp4A2->tag=1U,({struct Cyc_Absyn_Exp*_tmp946=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4A2->f1=_tmp946;}),({void*_tmp945=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4A2->f2=_tmp945;}),_tmp4A2->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4A2;});_tmp4A3->hd=_tmp947;}),({struct Cyc_List_List*_tmp944=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4A3->tl=_tmp944;});_tmp4A3;});_tmp713.tms=_tmp948;});_tmp713;}));
goto _LL0;}case 179U: _LL161: _LL162: {
# 1982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1981 "parse.y"
struct _tuple26*_tmp4A4=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4A5=_tmp4A4;struct Cyc_List_List*_tmp4AF;int _tmp4AE;struct Cyc_Absyn_VarargInfo*_tmp4AD;void*_tmp4AC;struct Cyc_List_List*_tmp4AB;_LL44D: _tmp4AF=_tmp4A5->f1;_tmp4AE=_tmp4A5->f2;_tmp4AD=_tmp4A5->f3;_tmp4AC=_tmp4A5->f4;_tmp4AB=_tmp4A5->f5;_LL44E:;{
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4A7=Cyc_yyget_YY55(yyyyvsp[5]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp714;({struct _tuple0*_tmp950=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp714.id=_tmp950;}),({unsigned int _tmp94F=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp714.varloc=_tmp94F;}),({struct Cyc_List_List*_tmp94E=({struct Cyc_List_List*_tmp4AA=_region_malloc(yyr,sizeof(*_tmp4AA));({void*_tmp94D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4A9=_region_malloc(yyr,sizeof(*_tmp4A9));_tmp4A9->tag=3U,({void*_tmp94C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4A8=_region_malloc(yyr,sizeof(*_tmp4A8));_tmp4A8->tag=1U,_tmp4A8->f1=_tmp4AF,_tmp4A8->f2=_tmp4AE,_tmp4A8->f3=_tmp4AD,_tmp4A8->f4=_tmp4AC,_tmp4A8->f5=_tmp4AB,_tmp4A8->f6=_tmp4A6,_tmp4A8->f7=_tmp4A7;_tmp4A8;});_tmp4A9->f1=_tmp94C;});_tmp4A9;});_tmp4AA->hd=_tmp94D;}),({struct Cyc_List_List*_tmp94B=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4AA->tl=_tmp94B;});_tmp4AA;});_tmp714.tms=_tmp94E;});_tmp714;}));
# 1986
goto _LL0;};}case 180U: _LL163: _LL164: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1987 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp715;({struct _tuple0*_tmp95A=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp715.id=_tmp95A;}),({unsigned int _tmp959=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp715.varloc=_tmp959;}),({
struct Cyc_List_List*_tmp958=({struct Cyc_List_List*_tmp4B2=_region_malloc(yyr,sizeof(*_tmp4B2));({void*_tmp957=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4B1=_region_malloc(yyr,sizeof(*_tmp4B1));_tmp4B1->tag=3U,({void*_tmp956=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4B0=_region_malloc(yyr,sizeof(*_tmp4B0));_tmp4B0->tag=1U,_tmp4B0->f1=0,_tmp4B0->f2=0,_tmp4B0->f3=0,({
# 1990
void*_tmp955=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4B0->f4=_tmp955;}),({struct Cyc_List_List*_tmp954=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4B0->f5=_tmp954;}),({struct Cyc_Absyn_Exp*_tmp953=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4B0->f6=_tmp953;}),({struct Cyc_Absyn_Exp*_tmp952=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4B0->f7=_tmp952;});_tmp4B0;});
# 1988
_tmp4B1->f1=_tmp956;});_tmp4B1;});_tmp4B2->hd=_tmp957;}),({
# 1991
struct Cyc_List_List*_tmp951=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B2->tl=_tmp951;});_tmp4B2;});
# 1988
_tmp715.tms=_tmp958;});_tmp715;}));
# 1993
goto _LL0;}case 181U: _LL165: _LL166: {
# 1995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1994 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp716;({struct _tuple0*_tmp961=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp716.id=_tmp961;}),({unsigned int _tmp960=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp716.varloc=_tmp960;}),({struct Cyc_List_List*_tmp95F=({struct Cyc_List_List*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));({void*_tmp95E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4B4=_region_malloc(yyr,sizeof(*_tmp4B4));_tmp4B4->tag=3U,({void*_tmp95D=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4B3=_region_malloc(yyr,sizeof(*_tmp4B3));_tmp4B3->tag=0U,({struct Cyc_List_List*_tmp95C=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4B3->f1=_tmp95C;}),_tmp4B3->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4B3;});_tmp4B4->f1=_tmp95D;});_tmp4B4;});_tmp4B5->hd=_tmp95E;}),({struct Cyc_List_List*_tmp95B=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B5->tl=_tmp95B;});_tmp4B5;});_tmp716.tms=_tmp95F;});_tmp716;}));
goto _LL0;}case 182U: _LL167: _LL168: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1997 "parse.y"
struct Cyc_List_List*_tmp4B6=({unsigned int _tmp962=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp962,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp717;({struct _tuple0*_tmp967=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp717.id=_tmp967;}),({unsigned int _tmp966=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp717.varloc=_tmp966;}),({struct Cyc_List_List*_tmp965=({struct Cyc_List_List*_tmp4B8=_region_malloc(yyr,sizeof(*_tmp4B8));({void*_tmp964=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4B7=_region_malloc(yyr,sizeof(*_tmp4B7));_tmp4B7->tag=4U,_tmp4B7->f1=_tmp4B6,_tmp4B7->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4B7->f3=0;_tmp4B7;});_tmp4B8->hd=_tmp964;}),({struct Cyc_List_List*_tmp963=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B8->tl=_tmp963;});_tmp4B8;});_tmp717.tms=_tmp965;});_tmp717;}));
# 2000
goto _LL0;}case 183U: _LL169: _LL16A: {
# 2002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2001 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp718;({struct _tuple0*_tmp96D=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp718.id=_tmp96D;}),({unsigned int _tmp96C=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp718.varloc=_tmp96C;}),({struct Cyc_List_List*_tmp96B=({struct Cyc_List_List*_tmp4BA=_region_malloc(yyr,sizeof(*_tmp4BA));({void*_tmp96A=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B9=_region_malloc(yyr,sizeof(*_tmp4B9));_tmp4B9->tag=5U,_tmp4B9->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp969=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4B9->f2=_tmp969;});_tmp4B9;});_tmp4BA->hd=_tmp96A;}),({
struct Cyc_List_List*_tmp968=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4BA->tl=_tmp968;});_tmp4BA;});
# 2001
_tmp718.tms=_tmp96B;});_tmp718;}));
# 2004
goto _LL0;}case 184U: _LL16B: _LL16C: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp719;({struct _tuple0*_tmp96E=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp719.id=_tmp96E;}),_tmp719.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp719.tms=0;_tmp719;}));
goto _LL0;}case 185U: _LL16D: _LL16E: {
# 2012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2011 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71A;({struct _tuple0*_tmp96F=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp71A.id=_tmp96F;}),_tmp71A.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp71A.tms=0;_tmp71A;}));
goto _LL0;}case 186U: _LL16F: _LL170: {
# 2014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2013 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 187U: _LL171: _LL172: {
# 2016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2017 "parse.y"
struct Cyc_Parse_Declarator _tmp4BB=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmp972=({struct Cyc_List_List*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));({void*_tmp971=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4BC=_region_malloc(yyr,sizeof(*_tmp4BC));_tmp4BC->tag=5U,_tmp4BC->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp970=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4BC->f2=_tmp970;});_tmp4BC;});_tmp4BD->hd=_tmp971;}),_tmp4BD->tl=_tmp4BB.tms;_tmp4BD;});_tmp4BB.tms=_tmp972;});
yyval=yyyyvsp[2];
# 2021
goto _LL0;}case 188U: _LL173: _LL174: {
# 2023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2022 "parse.y"
struct Cyc_Parse_Declarator _tmp4BE=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71B;_tmp71B.id=_tmp4BE.id,_tmp71B.varloc=_tmp4BE.varloc,({
struct Cyc_List_List*_tmp975=({struct Cyc_List_List*_tmp4C0=_region_malloc(yyr,sizeof(*_tmp4C0));({void*_tmp974=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4BF=_region_malloc(yyr,sizeof(*_tmp4BF));_tmp4BF->tag=0U,({void*_tmp973=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4BF->f1=_tmp973;}),_tmp4BF->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4BF;});_tmp4C0->hd=_tmp974;}),_tmp4C0->tl=_tmp4BE.tms;_tmp4C0;});_tmp71B.tms=_tmp975;});_tmp71B;}));
goto _LL0;}case 189U: _LL175: _LL176: {
# 2027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2026 "parse.y"
struct Cyc_Parse_Declarator _tmp4C1=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71C;_tmp71C.id=_tmp4C1.id,_tmp71C.varloc=_tmp4C1.varloc,({
struct Cyc_List_List*_tmp979=({struct Cyc_List_List*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));({void*_tmp978=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4C2=_region_malloc(yyr,sizeof(*_tmp4C2));_tmp4C2->tag=1U,({struct Cyc_Absyn_Exp*_tmp977=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4C2->f1=_tmp977;}),({void*_tmp976=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4C2->f2=_tmp976;}),_tmp4C2->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4C2;});_tmp4C3->hd=_tmp978;}),_tmp4C3->tl=_tmp4C1.tms;_tmp4C3;});_tmp71C.tms=_tmp979;});_tmp71C;}));
# 2030
goto _LL0;}case 190U: _LL177: _LL178: {
# 2032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2031 "parse.y"
struct _tuple26*_tmp4C4=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4C5=_tmp4C4;struct Cyc_List_List*_tmp4D0;int _tmp4CF;struct Cyc_Absyn_VarargInfo*_tmp4CE;void*_tmp4CD;struct Cyc_List_List*_tmp4CC;_LL450: _tmp4D0=_tmp4C5->f1;_tmp4CF=_tmp4C5->f2;_tmp4CE=_tmp4C5->f3;_tmp4CD=_tmp4C5->f4;_tmp4CC=_tmp4C5->f5;_LL451:;{
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp4C8=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71D;_tmp71D.id=_tmp4C8.id,_tmp71D.varloc=_tmp4C8.varloc,({struct Cyc_List_List*_tmp97C=({struct Cyc_List_List*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));({void*_tmp97B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=3U,({void*_tmp97A=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));_tmp4C9->tag=1U,_tmp4C9->f1=_tmp4D0,_tmp4C9->f2=_tmp4CF,_tmp4C9->f3=_tmp4CE,_tmp4C9->f4=_tmp4CD,_tmp4C9->f5=_tmp4CC,_tmp4C9->f6=_tmp4C6,_tmp4C9->f7=_tmp4C7;_tmp4C9;});_tmp4CA->f1=_tmp97A;});_tmp4CA;});_tmp4CB->hd=_tmp97B;}),_tmp4CB->tl=_tmp4C8.tms;_tmp4CB;});_tmp71D.tms=_tmp97C;});_tmp71D;}));
# 2037
goto _LL0;};}case 191U: _LL179: _LL17A: {
# 2039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2038 "parse.y"
struct Cyc_Parse_Declarator _tmp4D1=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71E;_tmp71E.id=_tmp4D1.id,_tmp71E.varloc=_tmp4D1.varloc,({
struct Cyc_List_List*_tmp983=({struct Cyc_List_List*_tmp4D4=_region_malloc(yyr,sizeof(*_tmp4D4));({void*_tmp982=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));_tmp4D3->tag=3U,({void*_tmp981=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4D2=_region_malloc(yyr,sizeof(*_tmp4D2));_tmp4D2->tag=1U,_tmp4D2->f1=0,_tmp4D2->f2=0,_tmp4D2->f3=0,({
# 2042
void*_tmp980=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4D2->f4=_tmp980;}),({struct Cyc_List_List*_tmp97F=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4D2->f5=_tmp97F;}),({struct Cyc_Absyn_Exp*_tmp97E=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4D2->f6=_tmp97E;}),({struct Cyc_Absyn_Exp*_tmp97D=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4D2->f7=_tmp97D;});_tmp4D2;});
# 2040
_tmp4D3->f1=_tmp981;});_tmp4D3;});_tmp4D4->hd=_tmp982;}),_tmp4D4->tl=_tmp4D1.tms;_tmp4D4;});_tmp71E.tms=_tmp983;});_tmp71E;}));
# 2045
goto _LL0;}case 192U: _LL17B: _LL17C: {
# 2047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2046 "parse.y"
struct Cyc_Parse_Declarator _tmp4D5=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71F;_tmp71F.id=_tmp4D5.id,_tmp71F.varloc=_tmp4D5.varloc,({struct Cyc_List_List*_tmp987=({struct Cyc_List_List*_tmp4D8=_region_malloc(yyr,sizeof(*_tmp4D8));({void*_tmp986=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D7=_region_malloc(yyr,sizeof(*_tmp4D7));_tmp4D7->tag=3U,({void*_tmp985=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));_tmp4D6->tag=0U,({struct Cyc_List_List*_tmp984=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4D6->f1=_tmp984;}),_tmp4D6->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4D6;});_tmp4D7->f1=_tmp985;});_tmp4D7;});_tmp4D8->hd=_tmp986;}),_tmp4D8->tl=_tmp4D5.tms;_tmp4D8;});_tmp71F.tms=_tmp987;});_tmp71F;}));
goto _LL0;}case 193U: _LL17D: _LL17E: {
# 2050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2050 "parse.y"
struct Cyc_List_List*_tmp4D9=({unsigned int _tmp988=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp988,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp4DA=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp720;_tmp720.id=_tmp4DA.id,_tmp720.varloc=_tmp4DA.varloc,({struct Cyc_List_List*_tmp98A=({struct Cyc_List_List*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));({void*_tmp989=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4DB=_region_malloc(yyr,sizeof(*_tmp4DB));_tmp4DB->tag=4U,_tmp4DB->f1=_tmp4D9,_tmp4DB->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4DB->f3=0;_tmp4DB;});_tmp4DC->hd=_tmp989;}),_tmp4DC->tl=_tmp4DA.tms;_tmp4DC;});_tmp720.tms=_tmp98A;});_tmp720;}));
# 2054
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2055 "parse.y"
struct Cyc_Parse_Declarator _tmp4DD=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp721;_tmp721.id=_tmp4DD.id,_tmp721.varloc=_tmp4DD.varloc,({struct Cyc_List_List*_tmp98D=({struct Cyc_List_List*_tmp4DF=_region_malloc(yyr,sizeof(*_tmp4DF));({void*_tmp98C=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4DE=_region_malloc(yyr,sizeof(*_tmp4DE));_tmp4DE->tag=5U,_tmp4DE->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp98B=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4DE->f2=_tmp98B;});_tmp4DE;});_tmp4DF->hd=_tmp98C;}),_tmp4DF->tl=_tmp4DD.tms;_tmp4DF;});_tmp721.tms=_tmp98D;});_tmp721;}));
# 2058
goto _LL0;}case 195U: _LL181: _LL182: {
# 2060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2062 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 196U: _LL183: _LL184: {
# 2065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2063 "parse.y"
yyval=Cyc_YY26(({struct Cyc_List_List*_tmp98E=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp98E,Cyc_yyget_YY26(yyyyvsp[1]));}));
goto _LL0;}case 197U: _LL185: _LL186: {
# 2066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2067 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
ans=({struct Cyc_List_List*_tmp4E1=_region_malloc(yyr,sizeof(*_tmp4E1));({void*_tmp990=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E0=_region_malloc(yyr,sizeof(*_tmp4E0));_tmp4E0->tag=5U,_tmp4E0->f1=(unsigned int)(yyyylsp[3]).first_line,({struct Cyc_List_List*_tmp98F=Cyc_yyget_YY45(yyyyvsp[3]);_tmp4E0->f2=_tmp98F;});_tmp4E0;});_tmp4E1->hd=_tmp990;}),_tmp4E1->tl=ans;_tmp4E1;});{
# 2071
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp4E2=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp4E3=_tmp4E2;unsigned int _tmp4EF;void*_tmp4EE;void*_tmp4ED;_LL453: _tmp4EF=_tmp4E3.f1;_tmp4EE=_tmp4E3.f2;_tmp4ED=_tmp4E3.f3;_LL454:;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->ptr_loc=_tmp4EF,_tmp4E4->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,_tmp4E4->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp4E4;});{
# 2076
struct _tuple14 _tmp4E5=({unsigned int _tmp994=_tmp4EF;void*_tmp993=_tmp4EE;void*_tmp992=_tmp4ED;void*_tmp991=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmp994,_tmp993,_tmp992,_tmp991,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp4E6=_tmp4E5;void*_tmp4EC;void*_tmp4EB;void*_tmp4EA;void*_tmp4E9;_LL456: _tmp4EC=_tmp4E6.f1;_tmp4EB=_tmp4E6.f2;_tmp4EA=_tmp4E6.f3;_tmp4E9=_tmp4E6.f4;_LL457:;
ans=({struct Cyc_List_List*_tmp4E8=_region_malloc(yyr,sizeof(*_tmp4E8));({void*_tmp996=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));_tmp4E7->tag=2U,(_tmp4E7->f1).rgn=_tmp4E9,(_tmp4E7->f1).nullable=_tmp4EC,(_tmp4E7->f1).bounds=_tmp4EB,(_tmp4E7->f1).zero_term=_tmp4EA,(_tmp4E7->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp995=Cyc_yyget_YY23(yyyyvsp[4]);_tmp4E7->f2=_tmp995;});_tmp4E7;});_tmp4E8->hd=_tmp996;}),_tmp4E8->tl=ans;_tmp4E8;});
yyval=Cyc_YY26(ans);
# 2080
goto _LL0;};};}case 198U: _LL187: _LL188:
# 2082
 yyval=Cyc_YY54(0);
goto _LL0;case 199U: _LL189: _LL18A: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2083 "parse.y"
yyval=Cyc_YY54(({struct Cyc_List_List*_tmp4F0=_region_malloc(yyr,sizeof(*_tmp4F0));({void*_tmp998=Cyc_yyget_YY53(yyyyvsp[0]);_tmp4F0->hd=_tmp998;}),({struct Cyc_List_List*_tmp997=Cyc_yyget_YY54(yyyyvsp[1]);_tmp4F0->tl=_tmp997;});_tmp4F0;}));
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2086
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2088
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2088 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));_tmp4F1->tag=0U,({struct Cyc_Absyn_Exp*_tmp999=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4F1->f1=_tmp999;});_tmp4F1;}));
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2090 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp4F2=_region_malloc(yyr,sizeof(*_tmp4F2));_tmp4F2->tag=1U,({void*_tmp99A=Cyc_yyget_YY44(yyyyvsp[2]);_tmp4F2->f1=_tmp99A;});_tmp4F2;}));
goto _LL0;}case 202U: _LL18F: _LL190: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2092 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));_tmp4F3->tag=2U;_tmp4F3;}));
goto _LL0;}case 203U: _LL191: _LL192: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2094 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));_tmp4F4->tag=3U;_tmp4F4;}));
goto _LL0;}case 204U: _LL193: _LL194: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2096 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));_tmp4F5->tag=4U;_tmp4F5;}));
goto _LL0;}case 205U: _LL195: _LL196: {
# 2099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2098 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp4F6=_region_malloc(yyr,sizeof(*_tmp4F6));_tmp4F6->tag=5U;_tmp4F6;}));
goto _LL0;}case 206U: _LL197: _LL198: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2100 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp4F7=_region_malloc(yyr,sizeof(*_tmp4F7));_tmp4F7->tag=6U;_tmp4F7;}));
goto _LL0;}case 207U: _LL199: _LL19A: {
# 2103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2102 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp4F8=_region_malloc(yyr,sizeof(*_tmp4F8));_tmp4F8->tag=7U;_tmp4F8;}));
goto _LL0;}case 208U: _LL19B: _LL19C: {
# 2105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2108 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
if(!Cyc_Parse_parsing_tempest)
yyval=Cyc_YY1(({struct _tuple21*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->f1=loc,_tmp4F9->f2=Cyc_Absyn_true_type,({void*_tmp99B=Cyc_yyget_YY2(yyyyvsp[1]);_tmp4F9->f3=_tmp99B;});_tmp4F9;}));else{
# 2112
yyval=Cyc_YY1(({struct _tuple21*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->f1=loc,_tmp4FA->f2=Cyc_Absyn_true_type,_tmp4FA->f3=Cyc_Absyn_fat_bound_type;_tmp4FA;}));}
# 2114
goto _LL0;}case 209U: _LL19D: _LL19E: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2115 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->f1=loc,_tmp4FB->f2=Cyc_Absyn_false_type,({void*_tmp99C=Cyc_yyget_YY2(yyyyvsp[1]);_tmp4FB->f3=_tmp99C;});_tmp4FB;}));
# 2118
goto _LL0;}case 210U: _LL19F: _LL1A0: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2119 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->f1=loc,_tmp4FC->f2=Cyc_Absyn_true_type,_tmp4FC->f3=Cyc_Absyn_fat_bound_type;_tmp4FC;}));
# 2122
goto _LL0;}case 211U: _LL1A1: _LL1A2:
# 2124
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 212U: _LL1A3: _LL1A4: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2125 "parse.y"
yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 213U: _LL1A5: _LL1A6:
# 2128
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 214U: _LL1A7: _LL1A8: {
# 2131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2129 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;}case 215U: _LL1A9: _LL1AA: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2130 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;}case 216U: _LL1AB: _LL1AC:
# 2133
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 217U: _LL1AD: _LL1AE: {
# 2136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2136 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL0;}case 218U: _LL1AF: _LL1B0: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2137 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;}case 219U: _LL1B1: _LL1B2:
# 2140
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
goto _LL0;case 220U: _LL1B3: _LL1B4: {
# 2143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2142 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp99D=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp99D,Cyc_yyget_YY23(yyyyvsp[1]));}));
goto _LL0;}case 221U: _LL1B5: _LL1B6: {
# 2145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2147 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));({struct Cyc_List_List*_tmp9A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp4FD->f1=_tmp9A0;}),_tmp4FD->f2=0,_tmp4FD->f3=0,({void*_tmp99F=Cyc_yyget_YY49(yyyyvsp[1]);_tmp4FD->f4=_tmp99F;}),({struct Cyc_List_List*_tmp99E=Cyc_yyget_YY50(yyyyvsp[2]);_tmp4FD->f5=_tmp99E;});_tmp4FD;}));
goto _LL0;}case 222U: _LL1B7: _LL1B8: {
# 2150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2149 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));({struct Cyc_List_List*_tmp9A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp4FE->f1=_tmp9A3;}),_tmp4FE->f2=1,_tmp4FE->f3=0,({void*_tmp9A2=Cyc_yyget_YY49(yyyyvsp[3]);_tmp4FE->f4=_tmp9A2;}),({struct Cyc_List_List*_tmp9A1=Cyc_yyget_YY50(yyyyvsp[4]);_tmp4FE->f5=_tmp9A1;});_tmp4FE;}));
goto _LL0;}case 223U: _LL1B9: _LL1BA: {
# 2152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2152 "parse.y"
struct _tuple8*_tmp4FF=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp500=_tmp4FF;struct _dyneither_ptr*_tmp506;struct Cyc_Absyn_Tqual _tmp505;void*_tmp504;_LL459: _tmp506=_tmp500->f1;_tmp505=_tmp500->f2;_tmp504=_tmp500->f3;_LL45A:;{
struct Cyc_Absyn_VarargInfo*_tmp501=({struct Cyc_Absyn_VarargInfo*_tmp503=_cycalloc(sizeof(*_tmp503));_tmp503->name=_tmp506,_tmp503->tq=_tmp505,_tmp503->type=_tmp504,({int _tmp9A4=Cyc_yyget_YY31(yyyyvsp[1]);_tmp503->inject=_tmp9A4;});_tmp503;});
yyval=Cyc_YY39(({struct _tuple26*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->f1=0,_tmp502->f2=0,_tmp502->f3=_tmp501,({void*_tmp9A6=Cyc_yyget_YY49(yyyyvsp[3]);_tmp502->f4=_tmp9A6;}),({struct Cyc_List_List*_tmp9A5=Cyc_yyget_YY50(yyyyvsp[4]);_tmp502->f5=_tmp9A5;});_tmp502;}));
# 2156
goto _LL0;};}case 224U: _LL1BB: _LL1BC: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2158 "parse.y"
struct _tuple8*_tmp507=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp508=_tmp507;struct _dyneither_ptr*_tmp50E;struct Cyc_Absyn_Tqual _tmp50D;void*_tmp50C;_LL45C: _tmp50E=_tmp508->f1;_tmp50D=_tmp508->f2;_tmp50C=_tmp508->f3;_LL45D:;{
struct Cyc_Absyn_VarargInfo*_tmp509=({struct Cyc_Absyn_VarargInfo*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->name=_tmp50E,_tmp50B->tq=_tmp50D,_tmp50B->type=_tmp50C,({int _tmp9A7=Cyc_yyget_YY31(yyyyvsp[3]);_tmp50B->inject=_tmp9A7;});_tmp50B;});
yyval=Cyc_YY39(({struct _tuple26*_tmp50A=_cycalloc(sizeof(*_tmp50A));({struct Cyc_List_List*_tmp9AA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp50A->f1=_tmp9AA;}),_tmp50A->f2=0,_tmp50A->f3=_tmp509,({void*_tmp9A9=Cyc_yyget_YY49(yyyyvsp[5]);_tmp50A->f4=_tmp9A9;}),({struct Cyc_List_List*_tmp9A8=Cyc_yyget_YY50(yyyyvsp[6]);_tmp50A->f5=_tmp9A8;});_tmp50A;}));
# 2162
goto _LL0;};}case 225U: _LL1BD: _LL1BE: {
# 2164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2166 "parse.y"
yyval=Cyc_YY44(({struct _dyneither_ptr _tmp9AB=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmp9AB,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->tag=1U,_tmp50F->f1=0;_tmp50F;}));}));
goto _LL0;}case 226U: _LL1BF: _LL1C0: {
# 2169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2167 "parse.y"
yyval=Cyc_YY44(({struct _dyneither_ptr _tmp9AC=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmp9AC,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));
goto _LL0;}case 227U: _LL1C1: _LL1C2:
# 2170
 yyval=Cyc_YY49(0);
goto _LL0;case 228U: _LL1C3: _LL1C4: {
# 2173
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2171 "parse.y"
yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 229U: _LL1C5: _LL1C6:
# 2174
 yyval=Cyc_YY50(0);
goto _LL0;case 230U: _LL1C7: _LL1C8: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2176 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 231U: _LL1C9: _LL1CA: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2184 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp510=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->tag=2U,_tmp515->f1=0,_tmp515->f2=& Cyc_Tcutil_trk;_tmp515;});
struct _dyneither_ptr _tmp511=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp512=Cyc_Parse_id2type(_tmp511,(void*)_tmp510);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp514=_cycalloc(sizeof(*_tmp514));({struct _tuple29*_tmp9AE=({struct _tuple29*_tmp513=_cycalloc(sizeof(*_tmp513));({void*_tmp9AD=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp513->f1=_tmp9AD;}),_tmp513->f2=_tmp512;_tmp513;});_tmp514->hd=_tmp9AE;}),_tmp514->tl=0;_tmp514;}));
# 2189
goto _LL0;}case 232U: _LL1CB: _LL1CC: {
# 2191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2191 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp516=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->tag=2U,_tmp51B->f1=0,_tmp51B->f2=& Cyc_Tcutil_trk;_tmp51B;});
struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp518=Cyc_Parse_id2type(_tmp517,(void*)_tmp516);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp51A=_cycalloc(sizeof(*_tmp51A));({struct _tuple29*_tmp9B1=({struct _tuple29*_tmp519=_cycalloc(sizeof(*_tmp519));({void*_tmp9B0=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp519->f1=_tmp9B0;}),_tmp519->f2=_tmp518;_tmp519;});_tmp51A->hd=_tmp9B1;}),({struct Cyc_List_List*_tmp9AF=Cyc_yyget_YY50(yyyyvsp[4]);_tmp51A->tl=_tmp9AF;});_tmp51A;}));
# 2196
goto _LL0;}case 233U: _LL1CD: _LL1CE:
# 2198
 yyval=Cyc_YY31(0);
goto _LL0;case 234U: _LL1CF: _LL1D0: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2202 "parse.y"
if(({struct _dyneither_ptr _tmp9B2=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmp9B2,({const char*_tmp51C="inject";_tag_dyneither(_tmp51C,sizeof(char),7U);}));})!= 0)
({void*_tmp51D=0U;({unsigned int _tmp9B4=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9B3=({const char*_tmp51E="missing type in function declaration";_tag_dyneither(_tmp51E,sizeof(char),37U);});Cyc_Warn_err(_tmp9B4,_tmp9B3,_tag_dyneither(_tmp51D,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);
# 2206
goto _LL0;}case 235U: _LL1D1: _LL1D2: {
# 2208
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2209 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 236U: _LL1D3: _LL1D4: {
# 2212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2210 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp9B5=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9B5,Cyc_yyget_YY40(yyyyvsp[2]));}));
goto _LL0;}case 237U: _LL1D5: _LL1D6: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2214 "parse.y"
yyval=Cyc_YY40(0);
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2215 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 239U: _LL1D9: _LL1DA: {
# 2218
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2220
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2217 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp51F=_cycalloc(sizeof(*_tmp51F));({void*_tmp9B6=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2]));_tmp51F->hd=_tmp9B6;}),_tmp51F->tl=0;_tmp51F;}));
goto _LL0;}case 240U: _LL1DB: _LL1DC: {
# 2220
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2219 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp520=_cycalloc(sizeof(*_tmp520));({void*_tmp9B7=Cyc_yyget_YY44(yyyyvsp[0]);_tmp520->hd=_tmp9B7;}),_tmp520->tl=0;_tmp520;}));
# 2222
goto _LL0;}case 241U: _LL1DD: _LL1DE: {
# 2224
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2239 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp521=_cycalloc(sizeof(*_tmp521));({void*_tmp9B9=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9B8=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9B8,(unsigned int)(yyyylsp[0]).first_line);}));_tmp521->hd=_tmp9B9;}),_tmp521->tl=0;_tmp521;}));
goto _LL0;}case 242U: _LL1DF: _LL1E0: {
# 2242
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2244
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2241 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp522=_cycalloc(sizeof(*_tmp522));({void*_tmp9BC=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9BB=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9BB,(unsigned int)(yyyylsp[0]).first_line);}));_tmp522->hd=_tmp9BC;}),({struct Cyc_List_List*_tmp9BA=Cyc_yyget_YY40(yyyyvsp[2]);_tmp522->tl=_tmp9BA;});_tmp522;}));
goto _LL0;}case 243U: _LL1E1: _LL1E2: {
# 2244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp523=_cycalloc(sizeof(*_tmp523));({struct _tuple8*_tmp9BD=Cyc_yyget_YY37(yyyyvsp[0]);_tmp523->hd=_tmp9BD;}),_tmp523->tl=0;_tmp523;}));
goto _LL0;}case 244U: _LL1E3: _LL1E4: {
# 2250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2249 "parse.y"
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp524=_cycalloc(sizeof(*_tmp524));({struct _tuple8*_tmp9BF=Cyc_yyget_YY37(yyyyvsp[2]);_tmp524->hd=_tmp9BF;}),({struct Cyc_List_List*_tmp9BE=Cyc_yyget_YY38(yyyyvsp[0]);_tmp524->tl=_tmp9BE;});_tmp524;}));
goto _LL0;}case 245U: _LL1E5: _LL1E6: {
# 2252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
struct _tuple25 _tmp525=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp526=_tmp525;struct Cyc_Absyn_Tqual _tmp53C;struct Cyc_Parse_Type_specifier _tmp53B;struct Cyc_List_List*_tmp53A;_LL45F: _tmp53C=_tmp526.f1;_tmp53B=_tmp526.f2;_tmp53A=_tmp526.f3;_LL460:;
if(_tmp53C.loc == (unsigned int)0)_tmp53C.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp527=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp528=_tmp527;struct _tuple0*_tmp539;unsigned int _tmp538;struct Cyc_List_List*_tmp537;_LL462: _tmp539=_tmp528.id;_tmp538=_tmp528.varloc;_tmp537=_tmp528.tms;_LL463:;{
void*_tmp529=Cyc_Parse_speclist2typ(_tmp53B,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp52A=Cyc_Parse_apply_tms(_tmp53C,_tmp529,_tmp53A,_tmp537);struct _tuple13 _tmp52B=_tmp52A;struct Cyc_Absyn_Tqual _tmp536;void*_tmp535;struct Cyc_List_List*_tmp534;struct Cyc_List_List*_tmp533;_LL465: _tmp536=_tmp52B.f1;_tmp535=_tmp52B.f2;_tmp534=_tmp52B.f3;_tmp533=_tmp52B.f4;_LL466:;
if(_tmp534 != 0)
({void*_tmp52C=0U;({unsigned int _tmp9C1=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9C0=({const char*_tmp52D="parameter with bad type params";_tag_dyneither(_tmp52D,sizeof(char),31U);});Cyc_Warn_err(_tmp9C1,_tmp9C0,_tag_dyneither(_tmp52C,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp539))
({void*_tmp52E=0U;({unsigned int _tmp9C3=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C2=({const char*_tmp52F="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp52F,sizeof(char),47U);});Cyc_Warn_err(_tmp9C3,_tmp9C2,_tag_dyneither(_tmp52E,sizeof(void*),0U));});});{
struct _dyneither_ptr*idopt=(*_tmp539).f2;
if(_tmp533 != 0)
({void*_tmp530=0U;({unsigned int _tmp9C5=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C4=({const char*_tmp531="extra attributes on parameter, ignoring";_tag_dyneither(_tmp531,sizeof(char),40U);});Cyc_Warn_warn(_tmp9C5,_tmp9C4,_tag_dyneither(_tmp530,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->f1=idopt,_tmp532->f2=_tmp536,_tmp532->f3=_tmp535;_tmp532;}));
# 2269
goto _LL0;};};};}case 246U: _LL1E7: _LL1E8: {
# 2271
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2270 "parse.y"
struct _tuple25 _tmp53D=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp53E=_tmp53D;struct Cyc_Absyn_Tqual _tmp545;struct Cyc_Parse_Type_specifier _tmp544;struct Cyc_List_List*_tmp543;_LL468: _tmp545=_tmp53E.f1;_tmp544=_tmp53E.f2;_tmp543=_tmp53E.f3;_LL469:;
if(_tmp545.loc == (unsigned int)0)_tmp545.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp53F=Cyc_Parse_speclist2typ(_tmp544,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp543 != 0)
({void*_tmp540=0U;({unsigned int _tmp9C7=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C6=({const char*_tmp541="bad attributes on parameter, ignoring";_tag_dyneither(_tmp541,sizeof(char),38U);});Cyc_Warn_warn(_tmp9C7,_tmp9C6,_tag_dyneither(_tmp540,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->f1=0,_tmp542->f2=_tmp545,_tmp542->f3=_tmp53F;_tmp542;}));
# 2277
goto _LL0;};}case 247U: _LL1E9: _LL1EA: {
# 2279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2278 "parse.y"
struct _tuple25 _tmp546=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp547=_tmp546;struct Cyc_Absyn_Tqual _tmp557;struct Cyc_Parse_Type_specifier _tmp556;struct Cyc_List_List*_tmp555;_LL46B: _tmp557=_tmp547.f1;_tmp556=_tmp547.f2;_tmp555=_tmp547.f3;_LL46C:;
if(_tmp557.loc == (unsigned int)0)_tmp557.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp548=Cyc_Parse_speclist2typ(_tmp556,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp549=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp54A=Cyc_Parse_apply_tms(_tmp557,_tmp548,_tmp555,_tmp549);struct _tuple13 _tmp54B=_tmp54A;struct Cyc_Absyn_Tqual _tmp554;void*_tmp553;struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp551;_LL46E: _tmp554=_tmp54B.f1;_tmp553=_tmp54B.f2;_tmp552=_tmp54B.f3;_tmp551=_tmp54B.f4;_LL46F:;
if(_tmp552 != 0)
({void*_tmp54C=0U;({unsigned int _tmp9C9=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C8=({const char*_tmp54D="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp54D,sizeof(char),49U);});Cyc_Warn_warn(_tmp9C9,_tmp9C8,_tag_dyneither(_tmp54C,sizeof(void*),0U));});});
# 2286
if(_tmp551 != 0)
({void*_tmp54E=0U;({unsigned int _tmp9CB=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9CA=({const char*_tmp54F="bad attributes on parameter, ignoring";_tag_dyneither(_tmp54F,sizeof(char),38U);});Cyc_Warn_warn(_tmp9CB,_tmp9CA,_tag_dyneither(_tmp54E,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->f1=0,_tmp550->f2=_tmp554,_tmp550->f3=_tmp553;_tmp550;}));
# 2290
goto _LL0;};}case 248U: _LL1EB: _LL1EC: {
# 2292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2294 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
goto _LL0;}case 249U: _LL1ED: _LL1EE: {
# 2297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2298 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp559=_cycalloc(sizeof(*_tmp559));({struct _dyneither_ptr*_tmp9CD=({struct _dyneither_ptr*_tmp558=_cycalloc(sizeof(*_tmp558));({struct _dyneither_ptr _tmp9CC=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp558=_tmp9CC;});_tmp558;});_tmp559->hd=_tmp9CD;}),_tmp559->tl=0;_tmp559;}));
goto _LL0;}case 250U: _LL1EF: _LL1F0: {
# 2301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2300 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp55B=_cycalloc(sizeof(*_tmp55B));({struct _dyneither_ptr*_tmp9D0=({struct _dyneither_ptr*_tmp55A=_cycalloc(sizeof(*_tmp55A));({struct _dyneither_ptr _tmp9CF=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp55A=_tmp9CF;});_tmp55A;});_tmp55B->hd=_tmp9D0;}),({struct Cyc_List_List*_tmp9CE=Cyc_yyget_YY36(yyyyvsp[0]);_tmp55B->tl=_tmp9CE;});_tmp55B;}));
goto _LL0;}case 251U: _LL1F1: _LL1F2: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2304 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 252U: _LL1F3: _LL1F4: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2305 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 253U: _LL1F5: _LL1F6: {
# 2308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2310 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9D1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->tag=36U,_tmp55C->f1=0,_tmp55C->f2=0;_tmp55C;});Cyc_Absyn_new_exp(_tmp9D1,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 254U: _LL1F7: _LL1F8: {
# 2313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2312 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9D3=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=36U,_tmp55D->f1=0,({struct Cyc_List_List*_tmp9D2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp55D->f2=_tmp9D2;});_tmp55D;});Cyc_Absyn_new_exp(_tmp9D3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 255U: _LL1F9: _LL1FA: {
# 2315
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2314 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9D5=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->tag=36U,_tmp55E->f1=0,({struct Cyc_List_List*_tmp9D4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp55E->f2=_tmp9D4;});_tmp55E;});Cyc_Absyn_new_exp(_tmp9D5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 256U: _LL1FB: _LL1FC: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2316 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp55F=({unsigned int _tmp9DA=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp9D9=({struct _tuple0*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp9D7=({struct _dyneither_ptr*_tmp561=_cycalloc(sizeof(*_tmp561));({struct _dyneither_ptr _tmp9D6=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp561=_tmp9D6;});_tmp561;});_tmp562->f2=_tmp9D7;});_tmp562;});void*_tmp9D8=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmp9DA,_tmp9D9,_tmp9D8,
Cyc_Absyn_uint_exp(0U,(unsigned int)(yyyylsp[2]).first_line));});
# 2319
(_tmp55F->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmp9DD=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->tag=27U,_tmp560->f1=_tmp55F,({struct Cyc_Absyn_Exp*_tmp9DC=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp560->f2=_tmp9DC;}),({struct Cyc_Absyn_Exp*_tmp9DB=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp560->f3=_tmp9DB;}),_tmp560->f4=0;_tmp560;});Cyc_Absyn_new_exp(_tmp9DD,(unsigned int)(yyyylsp[0]).first_line);}));
# 2322
goto _LL0;}case 257U: _LL1FD: _LL1FE: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2324 "parse.y"
void*_tmp563=({struct _tuple8*_tmp9DE=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmp9DE,(unsigned int)(yyyylsp[6]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmp9E0=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->tag=28U,({struct Cyc_Absyn_Exp*_tmp9DF=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp564->f1=_tmp9DF;}),_tmp564->f2=_tmp563,_tmp564->f3=0;_tmp564;});Cyc_Absyn_new_exp(_tmp9E0,(unsigned int)(yyyylsp[0]).first_line);}));
# 2327
goto _LL0;}case 258U: _LL1FF: _LL200: {
# 2329
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp566=_cycalloc(sizeof(*_tmp566));({struct _tuple30*_tmp9E2=({struct _tuple30*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->f1=0,({struct Cyc_Absyn_Exp*_tmp9E1=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp565->f2=_tmp9E1;});_tmp565;});_tmp566->hd=_tmp9E2;}),_tmp566->tl=0;_tmp566;}));
goto _LL0;}case 259U: _LL201: _LL202: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2334 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp568=_cycalloc(sizeof(*_tmp568));({struct _tuple30*_tmp9E5=({struct _tuple30*_tmp567=_cycalloc(sizeof(*_tmp567));({struct Cyc_List_List*_tmp9E4=Cyc_yyget_YY41(yyyyvsp[0]);_tmp567->f1=_tmp9E4;}),({struct Cyc_Absyn_Exp*_tmp9E3=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp567->f2=_tmp9E3;});_tmp567;});_tmp568->hd=_tmp9E5;}),_tmp568->tl=0;_tmp568;}));
goto _LL0;}case 260U: _LL203: _LL204: {
# 2337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2336 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp56A=_cycalloc(sizeof(*_tmp56A));({struct _tuple30*_tmp9E8=({struct _tuple30*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->f1=0,({struct Cyc_Absyn_Exp*_tmp9E7=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp569->f2=_tmp9E7;});_tmp569;});_tmp56A->hd=_tmp9E8;}),({struct Cyc_List_List*_tmp9E6=Cyc_yyget_YY5(yyyyvsp[0]);_tmp56A->tl=_tmp9E6;});_tmp56A;}));
goto _LL0;}case 261U: _LL205: _LL206: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2338 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));({struct _tuple30*_tmp9EC=({struct _tuple30*_tmp56B=_cycalloc(sizeof(*_tmp56B));({struct Cyc_List_List*_tmp9EB=Cyc_yyget_YY41(yyyyvsp[2]);_tmp56B->f1=_tmp9EB;}),({struct Cyc_Absyn_Exp*_tmp9EA=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp56B->f2=_tmp9EA;});_tmp56B;});_tmp56C->hd=_tmp9EC;}),({struct Cyc_List_List*_tmp9E9=Cyc_yyget_YY5(yyyyvsp[0]);_tmp56C->tl=_tmp9E9;});_tmp56C;}));
goto _LL0;}case 262U: _LL207: _LL208: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2342 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2343 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp56F=_cycalloc(sizeof(*_tmp56F));({void*_tmp9EF=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->tag=1U,({struct _dyneither_ptr*_tmp9EE=({struct _dyneither_ptr*_tmp56D=_cycalloc(sizeof(*_tmp56D));({struct _dyneither_ptr _tmp9ED=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp56D=_tmp9ED;});_tmp56D;});_tmp56E->f1=_tmp9EE;});_tmp56E;});_tmp56F->hd=_tmp9EF;}),_tmp56F->tl=0;_tmp56F;}));
goto _LL0;}case 264U: _LL20B: _LL20C: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2348 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp570=_cycalloc(sizeof(*_tmp570));({void*_tmp9F0=Cyc_yyget_YY42(yyyyvsp[0]);_tmp570->hd=_tmp9F0;}),_tmp570->tl=0;_tmp570;}));
goto _LL0;}case 265U: _LL20D: _LL20E: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2349 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));({void*_tmp9F2=Cyc_yyget_YY42(yyyyvsp[1]);_tmp571->hd=_tmp9F2;}),({struct Cyc_List_List*_tmp9F1=Cyc_yyget_YY41(yyyyvsp[0]);_tmp571->tl=_tmp9F1;});_tmp571;}));
goto _LL0;}case 266U: _LL20F: _LL210: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2353 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=0U,({struct Cyc_Absyn_Exp*_tmp9F3=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp572->f1=_tmp9F3;});_tmp572;}));
goto _LL0;}case 267U: _LL211: _LL212: {
# 2356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2354 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->tag=1U,({struct _dyneither_ptr*_tmp9F5=({struct _dyneither_ptr*_tmp573=_cycalloc(sizeof(*_tmp573));({struct _dyneither_ptr _tmp9F4=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp573=_tmp9F4;});_tmp573;});_tmp574->f1=_tmp9F5;});_tmp574;}));
goto _LL0;}case 268U: _LL213: _LL214: {
# 2357
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2359
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2359 "parse.y"
struct _tuple25 _tmp575=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp576=_tmp575;struct Cyc_Absyn_Tqual _tmp57D;struct Cyc_Parse_Type_specifier _tmp57C;struct Cyc_List_List*_tmp57B;_LL471: _tmp57D=_tmp576.f1;_tmp57C=_tmp576.f2;_tmp57B=_tmp576.f3;_LL472:;{
void*_tmp577=Cyc_Parse_speclist2typ(_tmp57C,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp57B != 0)
({void*_tmp578=0U;({unsigned int _tmp9F7=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9F6=({const char*_tmp579="ignoring attributes in type";_tag_dyneither(_tmp579,sizeof(char),28U);});Cyc_Warn_warn(_tmp9F7,_tmp9F6,_tag_dyneither(_tmp578,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->f1=0,_tmp57A->f2=_tmp57D,_tmp57A->f3=_tmp577;_tmp57A;}));
# 2365
goto _LL0;};}case 269U: _LL215: _LL216: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2366 "parse.y"
struct _tuple25 _tmp57E=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp57F=_tmp57E;struct Cyc_Absyn_Tqual _tmp58A;struct Cyc_Parse_Type_specifier _tmp589;struct Cyc_List_List*_tmp588;_LL474: _tmp58A=_tmp57F.f1;_tmp589=_tmp57F.f2;_tmp588=_tmp57F.f3;_LL475:;{
void*_tmp580=Cyc_Parse_speclist2typ(_tmp589,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp581=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp582=Cyc_Parse_apply_tms(_tmp58A,_tmp580,_tmp588,_tmp581);
if(_tmp582.f3 != 0)
# 2372
({void*_tmp583=0U;({unsigned int _tmp9F9=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9F8=({const char*_tmp584="bad type params, ignoring";_tag_dyneither(_tmp584,sizeof(char),26U);});Cyc_Warn_warn(_tmp9F9,_tmp9F8,_tag_dyneither(_tmp583,sizeof(void*),0U));});});
if(_tmp582.f4 != 0)
({void*_tmp585=0U;({unsigned int _tmp9FB=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9FA=({const char*_tmp586="bad specifiers, ignoring";_tag_dyneither(_tmp586,sizeof(char),25U);});Cyc_Warn_warn(_tmp9FB,_tmp9FA,_tag_dyneither(_tmp585,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->f1=0,_tmp587->f2=_tmp582.f1,_tmp587->f3=_tmp582.f2;_tmp587;}));
# 2377
goto _LL0;};}case 270U: _LL217: _LL218: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2380 "parse.y"
yyval=Cyc_YY44(({struct _tuple8*_tmp9FC=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9FC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 271U: _LL219: _LL21A: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2381 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;}case 272U: _LL21B: _LL21C: {
# 2384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2382 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 273U: _LL21D: _LL21E: {
# 2385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2383 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2])));
goto _LL0;}case 274U: _LL21F: _LL220: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2384 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));({void*_tmp9FE=Cyc_yyget_YY44(yyyyvsp[0]);_tmp58B->hd=_tmp9FE;}),({struct Cyc_List_List*_tmp9FD=Cyc_yyget_YY40(yyyyvsp[2]);_tmp58B->tl=_tmp9FD;});_tmp58B;})));
goto _LL0;}case 275U: _LL221: _LL222: {
# 2387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp58C=_cycalloc(sizeof(*_tmp58C));({void*_tmp9FF=Cyc_yyget_YY44(yyyyvsp[0]);_tmp58C->hd=_tmp9FF;}),_tmp58C->tl=0;_tmp58C;}));
goto _LL0;}case 276U: _LL223: _LL224: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2391 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp58D=_cycalloc(sizeof(*_tmp58D));({void*_tmpA01=Cyc_yyget_YY44(yyyyvsp[2]);_tmp58D->hd=_tmpA01;}),({struct Cyc_List_List*_tmpA00=Cyc_yyget_YY40(yyyyvsp[0]);_tmp58D->tl=_tmpA00;});_tmp58D;}));
goto _LL0;}case 277U: _LL225: _LL226: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2396 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp722;({struct Cyc_List_List*_tmpA02=Cyc_yyget_YY26(yyyyvsp[0]);_tmp722.tms=_tmpA02;});_tmp722;}));
goto _LL0;}case 278U: _LL227: _LL228: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2398 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 279U: _LL229: _LL22A: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2400 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp723;({struct Cyc_List_List*_tmpA04=({struct Cyc_List_List*_tmpA03=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA03,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp723.tms=_tmpA04;});_tmp723;}));
goto _LL0;}case 280U: _LL22B: _LL22C: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2405 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 281U: _LL22D: _LL22E: {
# 2408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2407 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp724;({struct Cyc_List_List*_tmpA07=({struct Cyc_List_List*_tmp58F=_region_malloc(yyr,sizeof(*_tmp58F));({void*_tmpA06=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp58E=_region_malloc(yyr,sizeof(*_tmp58E));_tmp58E->tag=0U,({void*_tmpA05=Cyc_yyget_YY51(yyyyvsp[2]);_tmp58E->f1=_tmpA05;}),_tmp58E->f2=(unsigned int)(yyyylsp[2]).first_line;_tmp58E;});_tmp58F->hd=_tmpA06;}),_tmp58F->tl=0;_tmp58F;});_tmp724.tms=_tmpA07;});_tmp724;}));
goto _LL0;}case 282U: _LL22F: _LL230: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2409 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp725;({struct Cyc_List_List*_tmpA0B=({struct Cyc_List_List*_tmp591=_region_malloc(yyr,sizeof(*_tmp591));({void*_tmpA0A=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp590=_region_malloc(yyr,sizeof(*_tmp590));_tmp590->tag=0U,({void*_tmpA09=Cyc_yyget_YY51(yyyyvsp[3]);_tmp590->f1=_tmpA09;}),_tmp590->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp590;});_tmp591->hd=_tmpA0A;}),({struct Cyc_List_List*_tmpA08=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp591->tl=_tmpA08;});_tmp591;});_tmp725.tms=_tmpA0B;});_tmp725;}));
goto _LL0;}case 283U: _LL231: _LL232: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2411 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp726;({struct Cyc_List_List*_tmpA0F=({struct Cyc_List_List*_tmp593=_region_malloc(yyr,sizeof(*_tmp593));({void*_tmpA0E=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp592=_region_malloc(yyr,sizeof(*_tmp592));_tmp592->tag=1U,({struct Cyc_Absyn_Exp*_tmpA0D=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp592->f1=_tmpA0D;}),({void*_tmpA0C=Cyc_yyget_YY51(yyyyvsp[3]);_tmp592->f2=_tmpA0C;}),_tmp592->f3=(unsigned int)(yyyylsp[3]).first_line;_tmp592;});_tmp593->hd=_tmpA0E;}),_tmp593->tl=0;_tmp593;});_tmp726.tms=_tmpA0F;});_tmp726;}));
goto _LL0;}case 284U: _LL233: _LL234: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2413 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp727;({struct Cyc_List_List*_tmpA14=({struct Cyc_List_List*_tmp595=_region_malloc(yyr,sizeof(*_tmp595));({void*_tmpA13=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp594=_region_malloc(yyr,sizeof(*_tmp594));_tmp594->tag=1U,({struct Cyc_Absyn_Exp*_tmpA12=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp594->f1=_tmpA12;}),({void*_tmpA11=Cyc_yyget_YY51(yyyyvsp[4]);_tmp594->f2=_tmpA11;}),_tmp594->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp594;});_tmp595->hd=_tmpA13;}),({
struct Cyc_List_List*_tmpA10=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp595->tl=_tmpA10;});_tmp595;});
# 2413
_tmp727.tms=_tmpA14;});_tmp727;}));
# 2416
goto _LL0;}case 285U: _LL235: _LL236: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2417 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp728;({struct Cyc_List_List*_tmpA1B=({struct Cyc_List_List*_tmp598=_region_malloc(yyr,sizeof(*_tmp598));({void*_tmpA1A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp597=_region_malloc(yyr,sizeof(*_tmp597));_tmp597->tag=3U,({void*_tmpA19=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp596=_region_malloc(yyr,sizeof(*_tmp596));_tmp596->tag=1U,_tmp596->f1=0,_tmp596->f2=0,_tmp596->f3=0,({void*_tmpA18=Cyc_yyget_YY49(yyyyvsp[1]);_tmp596->f4=_tmpA18;}),({struct Cyc_List_List*_tmpA17=Cyc_yyget_YY50(yyyyvsp[2]);_tmp596->f5=_tmpA17;}),({struct Cyc_Absyn_Exp*_tmpA16=Cyc_yyget_YY55(yyyyvsp[4]);_tmp596->f6=_tmpA16;}),({struct Cyc_Absyn_Exp*_tmpA15=Cyc_yyget_YY55(yyyyvsp[5]);_tmp596->f7=_tmpA15;});_tmp596;});_tmp597->f1=_tmpA19;});_tmp597;});_tmp598->hd=_tmpA1A;}),_tmp598->tl=0;_tmp598;});_tmp728.tms=_tmpA1B;});_tmp728;}));
# 2419
goto _LL0;}case 286U: _LL237: _LL238: {
# 2421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2420 "parse.y"
struct _tuple26*_tmp599=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp59A=_tmp599;struct Cyc_List_List*_tmp5A2;int _tmp5A1;struct Cyc_Absyn_VarargInfo*_tmp5A0;void*_tmp59F;struct Cyc_List_List*_tmp59E;_LL477: _tmp5A2=_tmp59A->f1;_tmp5A1=_tmp59A->f2;_tmp5A0=_tmp59A->f3;_tmp59F=_tmp59A->f4;_tmp59E=_tmp59A->f5;_LL478:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp729;({struct Cyc_List_List*_tmpA20=({struct Cyc_List_List*_tmp59D=_region_malloc(yyr,sizeof(*_tmp59D));({void*_tmpA1F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp59C=_region_malloc(yyr,sizeof(*_tmp59C));_tmp59C->tag=3U,({void*_tmpA1E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp59B=_region_malloc(yyr,sizeof(*_tmp59B));_tmp59B->tag=1U,_tmp59B->f1=_tmp5A2,_tmp59B->f2=_tmp5A1,_tmp59B->f3=_tmp5A0,_tmp59B->f4=_tmp59F,_tmp59B->f5=_tmp59E,({struct Cyc_Absyn_Exp*_tmpA1D=Cyc_yyget_YY55(yyyyvsp[3]);_tmp59B->f6=_tmpA1D;}),({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_yyget_YY55(yyyyvsp[4]);_tmp59B->f7=_tmpA1C;});_tmp59B;});_tmp59C->f1=_tmpA1E;});_tmp59C;});_tmp59D->hd=_tmpA1F;}),_tmp59D->tl=0;_tmp59D;});_tmp729.tms=_tmpA20;});_tmp729;}));
# 2423
goto _LL0;}case 287U: _LL239: _LL23A: {
# 2425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2424 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72A;({struct Cyc_List_List*_tmpA28=({struct Cyc_List_List*_tmp5A5=_region_malloc(yyr,sizeof(*_tmp5A5));({void*_tmpA27=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5A4=_region_malloc(yyr,sizeof(*_tmp5A4));_tmp5A4->tag=3U,({void*_tmpA26=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A3=_region_malloc(yyr,sizeof(*_tmp5A3));_tmp5A3->tag=1U,_tmp5A3->f1=0,_tmp5A3->f2=0,_tmp5A3->f3=0,({void*_tmpA25=Cyc_yyget_YY49(yyyyvsp[2]);_tmp5A3->f4=_tmpA25;}),({struct Cyc_List_List*_tmpA24=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5A3->f5=_tmpA24;}),({struct Cyc_Absyn_Exp*_tmpA23=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5A3->f6=_tmpA23;}),({struct Cyc_Absyn_Exp*_tmpA22=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5A3->f7=_tmpA22;});_tmp5A3;});_tmp5A4->f1=_tmpA26;});_tmp5A4;});_tmp5A5->hd=_tmpA27;}),({
struct Cyc_List_List*_tmpA21=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5A5->tl=_tmpA21;});_tmp5A5;});
# 2424
_tmp72A.tms=_tmpA28;});_tmp72A;}));
# 2427
goto _LL0;}case 288U: _LL23B: _LL23C: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2428 "parse.y"
struct _tuple26*_tmp5A6=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5A7=_tmp5A6;struct Cyc_List_List*_tmp5AF;int _tmp5AE;struct Cyc_Absyn_VarargInfo*_tmp5AD;void*_tmp5AC;struct Cyc_List_List*_tmp5AB;_LL47A: _tmp5AF=_tmp5A7->f1;_tmp5AE=_tmp5A7->f2;_tmp5AD=_tmp5A7->f3;_tmp5AC=_tmp5A7->f4;_tmp5AB=_tmp5A7->f5;_LL47B:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72B;({struct Cyc_List_List*_tmpA2E=({struct Cyc_List_List*_tmp5AA=_region_malloc(yyr,sizeof(*_tmp5AA));({void*_tmpA2D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5A9=_region_malloc(yyr,sizeof(*_tmp5A9));_tmp5A9->tag=3U,({void*_tmpA2C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));_tmp5A8->tag=1U,_tmp5A8->f1=_tmp5AF,_tmp5A8->f2=_tmp5AE,_tmp5A8->f3=_tmp5AD,_tmp5A8->f4=_tmp5AC,_tmp5A8->f5=_tmp5AB,({
struct Cyc_Absyn_Exp*_tmpA2B=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5A8->f6=_tmpA2B;}),({struct Cyc_Absyn_Exp*_tmpA2A=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5A8->f7=_tmpA2A;});_tmp5A8;});
# 2429
_tmp5A9->f1=_tmpA2C;});_tmp5A9;});_tmp5AA->hd=_tmpA2D;}),({
struct Cyc_List_List*_tmpA29=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5AA->tl=_tmpA29;});_tmp5AA;});
# 2429
_tmp72B.tms=_tmpA2E;});_tmp72B;}));
# 2432
goto _LL0;}case 289U: _LL23D: _LL23E: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2434 "parse.y"
struct Cyc_List_List*_tmp5B0=({unsigned int _tmpA2F=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA2F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72C;({struct Cyc_List_List*_tmpA32=({struct Cyc_List_List*_tmp5B2=_region_malloc(yyr,sizeof(*_tmp5B2));({void*_tmpA31=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5B1=_region_malloc(yyr,sizeof(*_tmp5B1));_tmp5B1->tag=4U,_tmp5B1->f1=_tmp5B0,_tmp5B1->f2=(unsigned int)(yyyylsp[1]).first_line,_tmp5B1->f3=0;_tmp5B1;});_tmp5B2->hd=_tmpA31;}),({
struct Cyc_List_List*_tmpA30=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5B2->tl=_tmpA30;});_tmp5B2;});
# 2435
_tmp72C.tms=_tmpA32;});_tmp72C;}));
# 2438
goto _LL0;}case 290U: _LL23F: _LL240: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2439 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72D;({struct Cyc_List_List*_tmpA36=({struct Cyc_List_List*_tmp5B4=_region_malloc(yyr,sizeof(*_tmp5B4));({void*_tmpA35=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5B3=_region_malloc(yyr,sizeof(*_tmp5B3));_tmp5B3->tag=5U,_tmp5B3->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA34=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5B3->f2=_tmpA34;});_tmp5B3;});_tmp5B4->hd=_tmpA35;}),({struct Cyc_List_List*_tmpA33=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5B4->tl=_tmpA33;});_tmp5B4;});_tmp72D.tms=_tmpA36;});_tmp72D;}));
# 2441
goto _LL0;}case 291U: _LL241: _LL242: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2445 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 292U: _LL243: _LL244: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2446 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 293U: _LL245: _LL246: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2447 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 294U: _LL247: _LL248: {
# 2450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2448 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 295U: _LL249: _LL24A: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2449 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 296U: _LL24B: _LL24C: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2450 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 297U: _LL24D: _LL24E: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
yyval=Cyc_Stmt_tok(({void*_tmpA3A=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->tag=13U,({struct _dyneither_ptr*_tmpA39=({struct _dyneither_ptr*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));({struct _dyneither_ptr _tmpA38=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5B5=_tmpA38;});_tmp5B5;});_tmp5B6->f1=_tmpA39;}),({struct Cyc_Absyn_Stmt*_tmpA37=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5B6->f2=_tmpA37;});_tmp5B6;});Cyc_Absyn_new_stmt(_tmpA3A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 298U: _LL24F: _LL250: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2460 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 299U: _LL251: _LL252: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2461 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA3B=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpA3B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 300U: _LL253: _LL254: {
# 2464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2466 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 301U: _LL255: _LL256: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2467 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 302U: _LL257: _LL258: {
# 2470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2472 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA3C=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpA3C,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));
goto _LL0;}case 303U: _LL259: _LL25A: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2473 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA3D=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpA3D,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));
goto _LL0;}case 304U: _LL25B: _LL25C: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2474 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA42=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->tag=13U,({struct _dyneither_ptr*_tmpA41=({struct _dyneither_ptr*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));({struct _dyneither_ptr _tmpA40=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5B7=_tmpA40;});_tmp5B7;});_tmp5B8->f1=_tmpA41;}),({struct Cyc_Absyn_Stmt*_tmpA3F=({struct Cyc_List_List*_tmpA3E=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpA3E,Cyc_Absyn_skip_stmt(0U));});_tmp5B8->f2=_tmpA3F;});_tmp5B8;});Cyc_Absyn_new_stmt(_tmpA42,(unsigned int)(yyyylsp[0]).first_line);}));
# 2476
goto _LL0;}case 305U: _LL25D: _LL25E: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2476 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA47=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->tag=13U,({struct _dyneither_ptr*_tmpA46=({struct _dyneither_ptr*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));({struct _dyneither_ptr _tmpA45=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5B9=_tmpA45;});_tmp5B9;});_tmp5BA->f1=_tmpA46;}),({struct Cyc_Absyn_Stmt*_tmpA44=({struct Cyc_List_List*_tmpA43=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpA43,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp5BA->f2=_tmpA44;});_tmp5BA;});Cyc_Absyn_new_stmt(_tmpA47,(unsigned int)(yyyylsp[0]).first_line);}));
# 2478
goto _LL0;}case 306U: _LL25F: _LL260: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2478 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 307U: _LL261: _LL262: {
# 2481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2479 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA49=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpA48=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpA49,_tmpA48,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 308U: _LL263: _LL264: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2480 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA4C=({void*_tmpA4B=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA4A=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5BB->f1=_tmpA4A;});_tmp5BB;});Cyc_Absyn_new_decl(_tmpA4B,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpA4C,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;}case 309U: _LL265: _LL266: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2483 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA4F=({void*_tmpA4E=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA4D=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5BC->f1=_tmpA4D;});_tmp5BC;});Cyc_Absyn_new_decl(_tmpA4E,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpA4F,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));
goto _LL0;}case 310U: _LL267: _LL268: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2488 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA52=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA51=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA50=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA52,_tmpA51,_tmpA50,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 311U: _LL269: _LL26A: {
# 2491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2490 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA55=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA54=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA53=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpA55,_tmpA54,_tmpA53,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 312U: _LL26B: _LL26C: {
# 2493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA57=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpA56=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpA57,_tmpA56,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 313U: _LL26D: _LL26E: {
# 2499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2499 "parse.y"
struct Cyc_Absyn_Exp*_tmp5BD=({struct _tuple0*_tmpA58=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpA58,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA5A=_tmp5BD;struct Cyc_List_List*_tmpA59=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpA5A,_tmpA59,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 314U: _LL26F: _LL270: {
# 2503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2503 "parse.y"
struct Cyc_Absyn_Exp*_tmp5BE=({struct Cyc_List_List*_tmpA5B=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpA5B,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA5D=_tmp5BE;struct Cyc_List_List*_tmpA5C=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpA5D,_tmpA5C,(unsigned int)(yyyylsp[0]).first_line);}));
# 2506
goto _LL0;}case 315U: _LL271: _LL272: {
# 2508
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2509 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA5F=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpA5E=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpA5F,_tmpA5E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 316U: _LL273: _LL274:
# 2512
 yyval=Cyc_YY8(0);
goto _LL0;case 317U: _LL275: _LL276: {
# 2515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2527 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));({struct Cyc_Absyn_Switch_clause*_tmpA63=({struct Cyc_Absyn_Switch_clause*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));({struct Cyc_Absyn_Pat*_tmpA62=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp5BF->pattern=_tmpA62;}),_tmp5BF->pat_vars=0,_tmp5BF->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA61=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5BF->body=_tmpA61;}),_tmp5BF->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5BF;});
# 2527
_tmp5C0->hd=_tmpA63;}),({
# 2529
struct Cyc_List_List*_tmpA60=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5C0->tl=_tmpA60;});_tmp5C0;}));
goto _LL0;}case 318U: _LL277: _LL278: {
# 2532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2531 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));({struct Cyc_Absyn_Switch_clause*_tmpA67=({struct Cyc_Absyn_Switch_clause*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));({struct Cyc_Absyn_Pat*_tmpA66=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5C1->pattern=_tmpA66;}),_tmp5C1->pat_vars=0,_tmp5C1->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA65=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp5C1->body=_tmpA65;}),_tmp5C1->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5C1;});
# 2531
_tmp5C2->hd=_tmpA67;}),({
# 2533
struct Cyc_List_List*_tmpA64=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5C2->tl=_tmpA64;});_tmp5C2;}));
goto _LL0;}case 319U: _LL279: _LL27A: {
# 2536
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2535 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));({struct Cyc_Absyn_Switch_clause*_tmpA6B=({struct Cyc_Absyn_Switch_clause*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));({struct Cyc_Absyn_Pat*_tmpA6A=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5C3->pattern=_tmpA6A;}),_tmp5C3->pat_vars=0,_tmp5C3->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpA69=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp5C3->body=_tmpA69;}),_tmp5C3->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5C3;});_tmp5C4->hd=_tmpA6B;}),({struct Cyc_List_List*_tmpA68=Cyc_yyget_YY8(yyyyvsp[4]);_tmp5C4->tl=_tmpA68;});_tmp5C4;}));
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2538
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2537 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));({struct Cyc_Absyn_Switch_clause*_tmpA70=({struct Cyc_Absyn_Switch_clause*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));({struct Cyc_Absyn_Pat*_tmpA6F=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5C5->pattern=_tmpA6F;}),_tmp5C5->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA6E=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5C5->where_clause=_tmpA6E;}),({
struct Cyc_Absyn_Stmt*_tmpA6D=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp5C5->body=_tmpA6D;}),_tmp5C5->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5C5;});
# 2537
_tmp5C6->hd=_tmpA70;}),({
# 2539
struct Cyc_List_List*_tmpA6C=Cyc_yyget_YY8(yyyyvsp[5]);_tmp5C6->tl=_tmpA6C;});_tmp5C6;}));
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2541 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));({struct Cyc_Absyn_Switch_clause*_tmpA75=({struct Cyc_Absyn_Switch_clause*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));({struct Cyc_Absyn_Pat*_tmpA74=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5C7->pattern=_tmpA74;}),_tmp5C7->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA73=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5C7->where_clause=_tmpA73;}),({struct Cyc_Absyn_Stmt*_tmpA72=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp5C7->body=_tmpA72;}),_tmp5C7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5C7;});_tmp5C8->hd=_tmpA75;}),({struct Cyc_List_List*_tmpA71=Cyc_yyget_YY8(yyyyvsp[6]);_tmp5C8->tl=_tmpA71;});_tmp5C8;}));
goto _LL0;}case 322U: _LL27F: _LL280: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2548 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA77=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA76=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpA77,_tmpA76,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 323U: _LL281: _LL282: {
# 2551
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2552 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA79=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpA78=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpA79,_tmpA78,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 324U: _LL283: _LL284: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2556 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA7C=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA7A=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2556
Cyc_Absyn_for_stmt(_tmpA7D,_tmpA7C,_tmpA7B,_tmpA7A,(unsigned int)(yyyylsp[0]).first_line);}));
# 2558
goto _LL0;}case 325U: _LL285: _LL286: {
# 2560
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2562
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2559 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA81=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA80=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA7F=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA7E=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2559
Cyc_Absyn_for_stmt(_tmpA81,_tmpA80,_tmpA7F,_tmpA7E,(unsigned int)(yyyylsp[0]).first_line);}));
# 2561
goto _LL0;}case 326U: _LL287: _LL288: {
# 2563
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2562 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA85=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA84=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA83=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA82=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2562
Cyc_Absyn_for_stmt(_tmpA85,_tmpA84,_tmpA83,_tmpA82,(unsigned int)(yyyylsp[0]).first_line);}));
# 2564
goto _LL0;}case 327U: _LL289: _LL28A: {
# 2566
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2565 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA89=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA88=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA87=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpA86=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2565
Cyc_Absyn_for_stmt(_tmpA89,_tmpA88,_tmpA87,_tmpA86,(unsigned int)(yyyylsp[0]).first_line);}));
# 2567
goto _LL0;}case 328U: _LL28B: _LL28C: {
# 2569
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2571
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2568 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA8D=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA8C=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA8B=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA8A=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2568
Cyc_Absyn_for_stmt(_tmpA8D,_tmpA8C,_tmpA8B,_tmpA8A,(unsigned int)(yyyylsp[0]).first_line);}));
# 2570
goto _LL0;}case 329U: _LL28D: _LL28E: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2571 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA91=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA90=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA8F=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpA8E=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2571
Cyc_Absyn_for_stmt(_tmpA91,_tmpA90,_tmpA8F,_tmpA8E,(unsigned int)(yyyylsp[0]).first_line);}));
# 2573
goto _LL0;}case 330U: _LL28F: _LL290: {
# 2575
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2574 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA95=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA94=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpA93=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA92=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2574
Cyc_Absyn_for_stmt(_tmpA95,_tmpA94,_tmpA93,_tmpA92,(unsigned int)(yyyylsp[0]).first_line);}));
# 2576
goto _LL0;}case 331U: _LL291: _LL292: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 2577 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA99=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA98=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpA97=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpA96=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2577
Cyc_Absyn_for_stmt(_tmpA99,_tmpA98,_tmpA97,_tmpA96,(unsigned int)(yyyylsp[0]).first_line);}));
# 2579
goto _LL0;}case 332U: _LL293: _LL294: {
# 2581
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2583
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2580 "parse.y"
struct Cyc_List_List*_tmp5C9=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5CA=({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA9C=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA9A=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2581
Cyc_Absyn_for_stmt(_tmpA9D,_tmpA9C,_tmpA9B,_tmpA9A,(unsigned int)(yyyylsp[0]).first_line);});
# 2583
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5C9,_tmp5CA));
# 2585
goto _LL0;}case 333U: _LL295: _LL296: {
# 2587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2586 "parse.y"
struct Cyc_List_List*_tmp5CB=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5CC=({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAA0=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA9F=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA9E=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2587
Cyc_Absyn_for_stmt(_tmpAA1,_tmpAA0,_tmpA9F,_tmpA9E,(unsigned int)(yyyylsp[0]).first_line);});
# 2589
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5CB,_tmp5CC));
# 2591
goto _LL0;}case 334U: _LL297: _LL298: {
# 2593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2592 "parse.y"
struct Cyc_List_List*_tmp5CD=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5CE=({struct Cyc_Absyn_Exp*_tmpAA5=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAA4=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAA3=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpAA2=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2593
Cyc_Absyn_for_stmt(_tmpAA5,_tmpAA4,_tmpAA3,_tmpAA2,(unsigned int)(yyyylsp[0]).first_line);});
# 2595
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5CD,_tmp5CE));
# 2597
goto _LL0;}case 335U: _LL299: _LL29A: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2598 "parse.y"
struct Cyc_List_List*_tmp5CF=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5D0=({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAA8=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpAA7=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpAA6=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2599
Cyc_Absyn_for_stmt(_tmpAA9,_tmpAA8,_tmpAA7,_tmpAA6,(unsigned int)(yyyylsp[0]).first_line);});
# 2601
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5CF,_tmp5D0));
# 2603
goto _LL0;}case 336U: _LL29B: _LL29C: {
# 2605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
yyval=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpAAB=({struct _dyneither_ptr*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));({struct _dyneither_ptr _tmpAAA=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp5D1=_tmpAAA;});_tmp5D1;});Cyc_Absyn_goto_stmt(_tmpAAB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 337U: _LL29D: _LL29E: {
# 2611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2609 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 338U: _LL29F: _LL2A0: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2610 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2611 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2612 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpAAC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2614 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2615 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 343U: _LL2A9: _LL2AA: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2617 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAAD=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpAAD,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 344U: _LL2AB: _LL2AC: {
# 2620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2626 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 345U: _LL2AD: _LL2AE: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2629 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 346U: _LL2AF: _LL2B0: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2631 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB0=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAAF=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpAAE=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpAB0,_tmpAAF,_tmpAAE,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 347U: _LL2B1: _LL2B2: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2634 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 348U: _LL2B3: _LL2B4: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2636 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpAB2,_tmpAB1,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 349U: _LL2B5: _LL2B6: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2639 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 350U: _LL2B7: _LL2B8: {
# 2642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2641 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB4=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpAB4,_tmpAB3,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 351U: _LL2B9: _LL2BA: {
# 2644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2644 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 352U: _LL2BB: _LL2BC: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2646 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB6=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB5=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpAB6,_tmpAB5,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 353U: _LL2BD: _LL2BE: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2649 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 354U: _LL2BF: _LL2C0: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2651 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB7=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpAB8,_tmpAB7,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 355U: _LL2C1: _LL2C2: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2654 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 356U: _LL2C3: _LL2C4: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2656 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpABA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpABA,_tmpAB9,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 357U: _LL2C5: _LL2C6: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2659 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 358U: _LL2C7: _LL2C8: {
# 2662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2661 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpABC=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpABB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpABC,_tmpABB,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 359U: _LL2C9: _LL2CA: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2663 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpABE=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpABD=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpABE,_tmpABD,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 360U: _LL2CB: _LL2CC: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2666 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 361U: _LL2CD: _LL2CE: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2668 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpABF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpAC0,_tmpABF,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 362U: _LL2CF: _LL2D0: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2670 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpAC2,_tmpAC1,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 363U: _LL2D1: _LL2D2: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2672 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpAC4,_tmpAC3,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 364U: _LL2D3: _LL2D4: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2674 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC5=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpAC6,_tmpAC5,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 365U: _LL2D5: _LL2D6: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2677 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 366U: _LL2D7: _LL2D8: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2679 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpAC8,_tmpAC7,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 367U: _LL2D9: _LL2DA: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2681 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpACA,_tmpAC9,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 368U: _LL2DB: _LL2DC: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2684 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 369U: _LL2DD: _LL2DE: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2686 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACC=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpACC,_tmpACB,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 370U: _LL2DF: _LL2E0: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2688 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACE=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACD=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpACE,_tmpACD,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 371U: _LL2E1: _LL2E2: {
# 2691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2691 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 372U: _LL2E3: _LL2E4: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2693 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpAD0,_tmpACF,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 373U: _LL2E5: _LL2E6: {
# 2696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2695 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpAD2,_tmpAD1,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 374U: _LL2E7: _LL2E8: {
# 2698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2697 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD4=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpAD4,_tmpAD3,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 375U: _LL2E9: _LL2EA: {
# 2700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2700 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 376U: _LL2EB: _LL2EC: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2702 "parse.y"
void*_tmp5D2=({struct _tuple8*_tmpAD5=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpAD5,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpAD7=_tmp5D2;struct Cyc_Absyn_Exp*_tmpAD6=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpAD7,_tmpAD6,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));
# 2705
goto _LL0;}case 377U: _LL2ED: _LL2EE: {
# 2707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2708 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 378U: _LL2EF: _LL2F0: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2711 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 379U: _LL2F1: _LL2F2: {
# 2714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2713 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpAD9=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpAD9,_tmpAD8,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 380U: _LL2F3: _LL2F4: {
# 2716
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2718
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2715 "parse.y"
void*_tmp5D3=({struct _tuple8*_tmpADA=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpADA,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(_tmp5D3,(unsigned int)(yyyylsp[0]).first_line)));
# 2718
goto _LL0;}case 381U: _LL2F5: _LL2F6: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2719 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpADB=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpADB,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 382U: _LL2F7: _LL2F8: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2721 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpADD=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpADC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpADD,_tmpADC,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 383U: _LL2F9: _LL2FA: {
# 2724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2726 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 384U: _LL2FB: _LL2FC: {
# 2729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2734 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 385U: _LL2FD: _LL2FE: {
# 2737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2739 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 386U: _LL2FF: _LL300: {
# 2742
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2741 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 387U: _LL301: _LL302: {
# 2744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2743 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp5D4=e->r;void*_tmp5D5=_tmp5D4;struct _dyneither_ptr _tmp5E9;int _tmp5E8;enum Cyc_Absyn_Sign _tmp5E7;int _tmp5E6;enum Cyc_Absyn_Sign _tmp5E5;short _tmp5E4;enum Cyc_Absyn_Sign _tmp5E3;char _tmp5E2;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).LongLong_c).tag){case 2U: _LL47D: _tmp5E3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Char_c).val).f1;_tmp5E2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Char_c).val).f2;_LL47E:
# 2747
 yyval=Cyc_YY9(({void*_tmpADE=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->tag=11U,_tmp5D6->f1=_tmp5E2;_tmp5D6;});Cyc_Absyn_new_pat(_tmpADE,e->loc);}));goto _LL47C;case 4U: _LL47F: _tmp5E5=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Short_c).val).f1;_tmp5E4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Short_c).val).f2;_LL480:
# 2749
 yyval=Cyc_YY9(({void*_tmpADF=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->tag=10U,_tmp5D7->f1=_tmp5E5,_tmp5D7->f2=(int)_tmp5E4;_tmp5D7;});Cyc_Absyn_new_pat(_tmpADF,e->loc);}));goto _LL47C;case 5U: _LL481: _tmp5E7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Int_c).val).f1;_tmp5E6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Int_c).val).f2;_LL482:
# 2751
 yyval=Cyc_YY9(({void*_tmpAE0=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->tag=10U,_tmp5D8->f1=_tmp5E7,_tmp5D8->f2=_tmp5E6;_tmp5D8;});Cyc_Absyn_new_pat(_tmpAE0,e->loc);}));goto _LL47C;case 7U: _LL483: _tmp5E9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Float_c).val).f1;_tmp5E8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D5)->f1).Float_c).val).f2;_LL484:
# 2753
 yyval=Cyc_YY9(({void*_tmpAE1=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->tag=12U,_tmp5D9->f1=_tmp5E9,_tmp5D9->f2=_tmp5E8;_tmp5D9;});Cyc_Absyn_new_pat(_tmpAE1,e->loc);}));goto _LL47C;case 1U: _LL485: _LL486:
# 2755
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL47C;case 8U: _LL487: _LL488:
# 2757
({void*_tmp5DA=0U;({unsigned int _tmpAE3=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAE2=({const char*_tmp5DB="strings cannot occur within patterns";_tag_dyneither(_tmp5DB,sizeof(char),37U);});Cyc_Warn_err(_tmpAE3,_tmpAE2,_tag_dyneither(_tmp5DA,sizeof(void*),0U));});});goto _LL47C;case 9U: _LL489: _LL48A:
# 2759
({void*_tmp5DC=0U;({unsigned int _tmpAE5=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAE4=({const char*_tmp5DD="strings cannot occur within patterns";_tag_dyneither(_tmp5DD,sizeof(char),37U);});Cyc_Warn_err(_tmpAE5,_tmpAE4,_tag_dyneither(_tmp5DC,sizeof(void*),0U));});});goto _LL47C;case 6U: _LL48B: _LL48C:
# 2761
({void*_tmp5DE=0U;({unsigned int _tmpAE7=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAE6=({const char*_tmp5DF="long long's in patterns not yet implemented";_tag_dyneither(_tmp5DF,sizeof(char),44U);});Cyc_Warn_err(_tmpAE7,_tmpAE6,_tag_dyneither(_tmp5DE,sizeof(void*),0U));});});goto _LL47C;default: goto _LL48D;}else{_LL48D: _LL48E:
# 2763
({void*_tmp5E0=0U;({unsigned int _tmpAE9=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAE8=({const char*_tmp5E1="bad constant in case";_tag_dyneither(_tmp5E1,sizeof(char),21U);});Cyc_Warn_err(_tmpAE9,_tmpAE8,_tag_dyneither(_tmp5E0,sizeof(void*),0U));});});}_LL47C:;}
# 2766
goto _LL0;}case 388U: _LL303: _LL304: {
# 2768
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2767 "parse.y"
yyval=Cyc_YY9(({void*_tmpAEB=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->tag=15U,({struct _tuple0*_tmpAEA=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5EA->f1=_tmpAEA;});_tmp5EA;});Cyc_Absyn_new_pat(_tmpAEB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 389U: _LL305: _LL306: {
# 2770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2769 "parse.y"
if(({struct _dyneither_ptr _tmpAEC=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpAEC,({const char*_tmp5EB="as";_tag_dyneither(_tmp5EB,sizeof(char),3U);}));})!= 0)
({void*_tmp5EC=0U;({unsigned int _tmpAEE=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpAED=({const char*_tmp5ED="expecting `as'";_tag_dyneither(_tmp5ED,sizeof(char),15U);});Cyc_Warn_err(_tmpAEE,_tmpAED,_tag_dyneither(_tmp5EC,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpAF5=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpAF4=({unsigned int _tmpAF3=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpAF2=({struct _tuple0*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpAF1=({struct _dyneither_ptr*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));({struct _dyneither_ptr _tmpAF0=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5EE=_tmpAF0;});_tmp5EE;});_tmp5EF->f2=_tmpAF1;});_tmp5EF;});Cyc_Absyn_new_vardecl(_tmpAF3,_tmpAF2,Cyc_Absyn_void_type,0);});_tmp5F0->f1=_tmpAF4;}),({
struct Cyc_Absyn_Pat*_tmpAEF=Cyc_yyget_YY9(yyyyvsp[2]);_tmp5F0->f2=_tmpAEF;});_tmp5F0;});
# 2771
Cyc_Absyn_new_pat(_tmpAF5,(unsigned int)(yyyylsp[0]).first_line);}));
# 2774
goto _LL0;}case 390U: _LL307: _LL308: {
# 2776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2775 "parse.y"
if(({struct _dyneither_ptr _tmpAF6=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpAF6,({const char*_tmp5F1="alias";_tag_dyneither(_tmp5F1,sizeof(char),6U);}));})!= 0)
({void*_tmp5F2=0U;({unsigned int _tmpAF8=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpAF7=({const char*_tmp5F3="expecting `alias'";_tag_dyneither(_tmp5F3,sizeof(char),18U);});Cyc_Warn_err(_tmpAF8,_tmpAF7,_tag_dyneither(_tmp5F2,sizeof(void*),0U));});});{
int _tmp5F4=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({struct _dyneither_ptr*_tmpAFB=({struct _dyneither_ptr*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));({struct _dyneither_ptr _tmpAFA=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp5F8=_tmpAFA;});_tmp5F8;});_tmp5FA->name=_tmpAFB;}),_tmp5FA->identity=- 1,({void*_tmpAF9=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->tag=0U,_tmp5F9->f1=& Cyc_Tcutil_rk;_tmp5F9;});_tmp5FA->kind=_tmpAF9;});_tmp5FA;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpB00=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpAFF=({struct _tuple0*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpAFD=({struct _dyneither_ptr*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));({struct _dyneither_ptr _tmpAFC=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp5F6=_tmpAFC;});_tmp5F6;});_tmp5F7->f2=_tmpAFD;});_tmp5F7;});Cyc_Absyn_new_vardecl(_tmpB00,_tmpAFF,({
struct _tuple8*_tmpAFE=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpAFE,(unsigned int)(yyyylsp[4]).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB01=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=2U,_tmp5F5->f1=tv,_tmp5F5->f2=vd;_tmp5F5;});Cyc_Absyn_new_pat(_tmpB01,(unsigned int)_tmp5F4);}));
# 2783
goto _LL0;};}case 391U: _LL309: _LL30A: {
# 2785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2784 "parse.y"
if(({struct _dyneither_ptr _tmpB02=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpB02,({const char*_tmp5FB="alias";_tag_dyneither(_tmp5FB,sizeof(char),6U);}));})!= 0)
({void*_tmp5FC=0U;({unsigned int _tmpB04=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB03=({const char*_tmp5FD="expecting `alias'";_tag_dyneither(_tmp5FD,sizeof(char),18U);});Cyc_Warn_err(_tmpB04,_tmpB03,_tag_dyneither(_tmp5FC,sizeof(void*),0U));});});{
int _tmp5FE=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp604=_cycalloc(sizeof(*_tmp604));({struct _dyneither_ptr*_tmpB07=({struct _dyneither_ptr*_tmp602=_cycalloc(sizeof(*_tmp602));({struct _dyneither_ptr _tmpB06=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp602=_tmpB06;});_tmp602;});_tmp604->name=_tmpB07;}),_tmp604->identity=- 1,({void*_tmpB05=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->tag=0U,_tmp603->f1=& Cyc_Tcutil_rk;_tmp603;});_tmp604->kind=_tmpB05;});_tmp604;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpB0C=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB0B=({struct _tuple0*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB09=({struct _dyneither_ptr*_tmp600=_cycalloc(sizeof(*_tmp600));({struct _dyneither_ptr _tmpB08=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp600=_tmpB08;});_tmp600;});_tmp601->f2=_tmpB09;});_tmp601;});Cyc_Absyn_new_vardecl(_tmpB0C,_tmpB0B,({
struct _tuple8*_tmpB0A=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpB0A,(unsigned int)(yyyylsp[4]).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB0D=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));_tmp5FF->tag=2U,_tmp5FF->f1=tv,_tmp5FF->f2=vd;_tmp5FF;});Cyc_Absyn_new_pat(_tmpB0D,(unsigned int)_tmp5FE);}));
# 2792
goto _LL0;};}case 392U: _LL30B: _LL30C: {
# 2794
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2793 "parse.y"
struct _tuple22 _tmp605=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp606=_tmp605;struct Cyc_List_List*_tmp609;int _tmp608;_LL490: _tmp609=_tmp606.f1;_tmp608=_tmp606.f2;_LL491:;
yyval=Cyc_YY9(({void*_tmpB0E=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->tag=5U,_tmp607->f1=_tmp609,_tmp607->f2=_tmp608;_tmp607;});Cyc_Absyn_new_pat(_tmpB0E,(unsigned int)(yyyylsp[0]).first_line);}));
# 2796
goto _LL0;}case 393U: _LL30D: _LL30E: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2797 "parse.y"
struct _tuple22 _tmp60A=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp60B=_tmp60A;struct Cyc_List_List*_tmp60E;int _tmp60D;_LL493: _tmp60E=_tmp60B.f1;_tmp60D=_tmp60B.f2;_LL494:;
yyval=Cyc_YY9(({void*_tmpB10=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=16U,({struct _tuple0*_tmpB0F=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp60C->f1=_tmpB0F;}),_tmp60C->f2=_tmp60E,_tmp60C->f3=_tmp60D;_tmp60C;});Cyc_Absyn_new_pat(_tmpB10,(unsigned int)(yyyylsp[0]).first_line);}));
# 2800
goto _LL0;}case 394U: _LL30F: _LL310: {
# 2802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2801 "parse.y"
struct _tuple22 _tmp60F=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp610=_tmp60F;struct Cyc_List_List*_tmp615;int _tmp614;_LL496: _tmp615=_tmp610.f1;_tmp614=_tmp610.f2;_LL497:;{
struct Cyc_List_List*_tmp611=({unsigned int _tmpB11=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB11,Cyc_yyget_YY40(yyyyvsp[2]));});
yyval=Cyc_YY9(({void*_tmpB14=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB13=({union Cyc_Absyn_AggrInfo*_tmp612=_cycalloc(sizeof(*_tmp612));({union Cyc_Absyn_AggrInfo _tmpB12=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);*_tmp612=_tmpB12;});_tmp612;});_tmp613->f1=_tmpB13;}),_tmp613->f2=_tmp611,_tmp613->f3=_tmp615,_tmp613->f4=_tmp614;_tmp613;});Cyc_Absyn_new_pat(_tmpB14,(unsigned int)(yyyylsp[0]).first_line);}));
# 2806
goto _LL0;};}case 395U: _LL311: _LL312: {
# 2808
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2807 "parse.y"
struct _tuple22 _tmp616=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp617=_tmp616;struct Cyc_List_List*_tmp61B;int _tmp61A;_LL499: _tmp61B=_tmp617.f1;_tmp61A=_tmp617.f2;_LL49A:;{
struct Cyc_List_List*_tmp618=({unsigned int _tmpB15=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB15,Cyc_yyget_YY40(yyyyvsp[1]));});
yyval=Cyc_YY9(({void*_tmpB16=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=7U,_tmp619->f1=0,_tmp619->f2=_tmp618,_tmp619->f3=_tmp61B,_tmp619->f4=_tmp61A;_tmp619;});Cyc_Absyn_new_pat(_tmpB16,(unsigned int)(yyyylsp[0]).first_line);}));
# 2811
goto _LL0;};}case 396U: _LL313: _LL314: {
# 2813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2812 "parse.y"
yyval=Cyc_YY9(({void*_tmpB18=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tag=6U,({struct Cyc_Absyn_Pat*_tmpB17=Cyc_yyget_YY9(yyyyvsp[1]);_tmp61C->f1=_tmpB17;});_tmp61C;});Cyc_Absyn_new_pat(_tmpB18,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 397U: _LL315: _LL316: {
# 2815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2814 "parse.y"
yyval=Cyc_YY9(({void*_tmpB1C=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->tag=6U,({struct Cyc_Absyn_Pat*_tmpB1B=({void*_tmpB1A=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->tag=6U,({struct Cyc_Absyn_Pat*_tmpB19=Cyc_yyget_YY9(yyyyvsp[1]);_tmp61D->f1=_tmpB19;});_tmp61D;});Cyc_Absyn_new_pat(_tmpB1A,(unsigned int)(yyyylsp[0]).first_line);});_tmp61E->f1=_tmpB1B;});_tmp61E;});Cyc_Absyn_new_pat(_tmpB1C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 398U: _LL317: _LL318: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2816 "parse.y"
yyval=Cyc_YY9(({void*_tmpB23=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB22=({unsigned int _tmpB21=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB20=({struct _tuple0*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB1F=({struct _dyneither_ptr*_tmp61F=_cycalloc(sizeof(*_tmp61F));({struct _dyneither_ptr _tmpB1E=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp61F=_tmpB1E;});_tmp61F;});_tmp620->f2=_tmpB1F;});_tmp620;});Cyc_Absyn_new_vardecl(_tmpB21,_tmpB20,Cyc_Absyn_void_type,0);});_tmp621->f1=_tmpB22;}),({
# 2818
struct Cyc_Absyn_Pat*_tmpB1D=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp621->f2=_tmpB1D;});_tmp621;});
# 2816
Cyc_Absyn_new_pat(_tmpB23,(unsigned int)(yyyylsp[0]).first_line);}));
# 2820
goto _LL0;}case 399U: _LL319: _LL31A: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2821 "parse.y"
if(({struct _dyneither_ptr _tmpB24=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpB24,({const char*_tmp622="as";_tag_dyneither(_tmp622,sizeof(char),3U);}));})!= 0)
({void*_tmp623=0U;({unsigned int _tmpB26=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpB25=({const char*_tmp624="expecting `as'";_tag_dyneither(_tmp624,sizeof(char),15U);});Cyc_Warn_err(_tmpB26,_tmpB25,_tag_dyneither(_tmp623,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB2D=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB2C=({unsigned int _tmpB2B=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB2A=({struct _tuple0*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB29=({struct _dyneither_ptr*_tmp625=_cycalloc(sizeof(*_tmp625));({struct _dyneither_ptr _tmpB28=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp625=_tmpB28;});_tmp625;});_tmp626->f2=_tmpB29;});_tmp626;});Cyc_Absyn_new_vardecl(_tmpB2B,_tmpB2A,Cyc_Absyn_void_type,0);});_tmp627->f1=_tmpB2C;}),({
# 2825
struct Cyc_Absyn_Pat*_tmpB27=Cyc_yyget_YY9(yyyyvsp[3]);_tmp627->f2=_tmpB27;});_tmp627;});
# 2823
Cyc_Absyn_new_pat(_tmpB2D,(unsigned int)(yyyylsp[0]).first_line);}));
# 2827
goto _LL0;}case 400U: _LL31B: _LL31C: {
# 2829
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2828 "parse.y"
void*_tmp628=({struct _dyneither_ptr _tmpB2E=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpB2E,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
yyval=Cyc_YY9(({void*_tmpB35=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB34=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp628);_tmp62B->f1=_tmpB34;}),({
struct Cyc_Absyn_Vardecl*_tmpB33=({unsigned int _tmpB32=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB31=({struct _tuple0*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB30=({struct _dyneither_ptr*_tmp629=_cycalloc(sizeof(*_tmp629));({struct _dyneither_ptr _tmpB2F=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp629=_tmpB2F;});_tmp629;});_tmp62A->f2=_tmpB30;});_tmp62A;});Cyc_Absyn_new_vardecl(_tmpB32,_tmpB31,
Cyc_Absyn_tag_type(_tmp628),0);});
# 2830
_tmp62B->f2=_tmpB33;});_tmp62B;});
# 2829
Cyc_Absyn_new_pat(_tmpB35,(unsigned int)(yyyylsp[0]).first_line);}));
# 2833
goto _LL0;}case 401U: _LL31D: _LL31E: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2834 "parse.y"
struct Cyc_Absyn_Tvar*_tmp62C=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(({void*_tmpB3B=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=4U,_tmp62F->f1=_tmp62C,({
struct Cyc_Absyn_Vardecl*_tmpB3A=({unsigned int _tmpB39=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB38=({struct _tuple0*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB37=({struct _dyneither_ptr*_tmp62D=_cycalloc(sizeof(*_tmp62D));({struct _dyneither_ptr _tmpB36=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp62D=_tmpB36;});_tmp62D;});_tmp62E->f2=_tmpB37;});_tmp62E;});Cyc_Absyn_new_vardecl(_tmpB39,_tmpB38,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(_tmp62C)),0);});
# 2836
_tmp62F->f2=_tmpB3A;});_tmp62F;});
# 2835
Cyc_Absyn_new_pat(_tmpB3B,(unsigned int)(yyyylsp[0]).first_line);}));
# 2839
goto _LL0;}case 402U: _LL31F: _LL320: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2842 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp630=_cycalloc(sizeof(*_tmp630));({struct Cyc_List_List*_tmpB3C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp630->f1=_tmpB3C;}),_tmp630->f2=0;_tmp630;}));
goto _LL0;}case 403U: _LL321: _LL322: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2843 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp631=_cycalloc(sizeof(*_tmp631));({struct Cyc_List_List*_tmpB3D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp631->f1=_tmpB3D;}),_tmp631->f2=1;_tmp631;}));
goto _LL0;}case 404U: _LL323: _LL324: {
# 2846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2844 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->f1=0,_tmp632->f2=1;_tmp632;}));
goto _LL0;}case 405U: _LL325: _LL326: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2849 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp633=_cycalloc(sizeof(*_tmp633));({struct Cyc_Absyn_Pat*_tmpB3E=Cyc_yyget_YY9(yyyyvsp[0]);_tmp633->hd=_tmpB3E;}),_tmp633->tl=0;_tmp633;}));
goto _LL0;}case 406U: _LL327: _LL328: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2851 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp634=_cycalloc(sizeof(*_tmp634));({struct Cyc_Absyn_Pat*_tmpB40=Cyc_yyget_YY9(yyyyvsp[2]);_tmp634->hd=_tmpB40;}),({struct Cyc_List_List*_tmpB3F=Cyc_yyget_YY11(yyyyvsp[0]);_tmp634->tl=_tmpB3F;});_tmp634;}));
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2856 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->f1=0,({struct Cyc_Absyn_Pat*_tmpB41=Cyc_yyget_YY9(yyyyvsp[0]);_tmp635->f2=_tmpB41;});_tmp635;}));
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2858 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp636=_cycalloc(sizeof(*_tmp636));({struct Cyc_List_List*_tmpB43=Cyc_yyget_YY41(yyyyvsp[0]);_tmp636->f1=_tmpB43;}),({struct Cyc_Absyn_Pat*_tmpB42=Cyc_yyget_YY9(yyyyvsp[1]);_tmp636->f2=_tmpB42;});_tmp636;}));
goto _LL0;}case 409U: _LL32D: _LL32E: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2861 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp637=_cycalloc(sizeof(*_tmp637));({struct Cyc_List_List*_tmpB44=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp637->f1=_tmpB44;}),_tmp637->f2=0;_tmp637;}));
goto _LL0;}case 410U: _LL32F: _LL330: {
# 2864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2862 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp638=_cycalloc(sizeof(*_tmp638));({struct Cyc_List_List*_tmpB45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp638->f1=_tmpB45;}),_tmp638->f2=1;_tmp638;}));
goto _LL0;}case 411U: _LL331: _LL332: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2863 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->f1=0,_tmp639->f2=1;_tmp639;}));
goto _LL0;}case 412U: _LL333: _LL334: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2868 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp63A=_cycalloc(sizeof(*_tmp63A));({struct _tuple23*_tmpB46=Cyc_yyget_YY12(yyyyvsp[0]);_tmp63A->hd=_tmpB46;}),_tmp63A->tl=0;_tmp63A;}));
goto _LL0;}case 413U: _LL335: _LL336: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2870 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp63B=_cycalloc(sizeof(*_tmp63B));({struct _tuple23*_tmpB48=Cyc_yyget_YY12(yyyyvsp[2]);_tmp63B->hd=_tmpB48;}),({struct Cyc_List_List*_tmpB47=Cyc_yyget_YY13(yyyyvsp[0]);_tmp63B->tl=_tmpB47;});_tmp63B;}));
goto _LL0;}case 414U: _LL337: _LL338: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 415U: _LL339: _LL33A: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2878 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB49=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpB4A,_tmpB49,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 416U: _LL33B: _LL33C: {
# 2881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2883 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 417U: _LL33D: _LL33E: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2885 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpB4C=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpB4B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpB4D,_tmpB4C,_tmpB4B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 418U: _LL33F: _LL340: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2887 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB4E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpB4F,_tmpB4E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 419U: _LL341: _LL342: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2891 "parse.y"
yyval=Cyc_YY7(0);
goto _LL0;}case 420U: _LL343: _LL344: {
# 2894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2892 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->v=(void*)Cyc_Absyn_Times;_tmp63C;}));
goto _LL0;}case 421U: _LL345: _LL346: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2893 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->v=(void*)Cyc_Absyn_Div;_tmp63D;}));
goto _LL0;}case 422U: _LL347: _LL348: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2894 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->v=(void*)Cyc_Absyn_Mod;_tmp63E;}));
goto _LL0;}case 423U: _LL349: _LL34A: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2895 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->v=(void*)Cyc_Absyn_Plus;_tmp63F;}));
goto _LL0;}case 424U: _LL34B: _LL34C: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2896 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->v=(void*)Cyc_Absyn_Minus;_tmp640;}));
goto _LL0;}case 425U: _LL34D: _LL34E: {
# 2899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2897 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->v=(void*)Cyc_Absyn_Bitlshift;_tmp641;}));
goto _LL0;}case 426U: _LL34F: _LL350: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2898 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->v=(void*)Cyc_Absyn_Bitlrshift;_tmp642;}));
goto _LL0;}case 427U: _LL351: _LL352: {
# 2901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2899 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp643=_cycalloc(sizeof(*_tmp643));_tmp643->v=(void*)Cyc_Absyn_Bitand;_tmp643;}));
goto _LL0;}case 428U: _LL353: _LL354: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2900 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->v=(void*)Cyc_Absyn_Bitxor;_tmp644;}));
goto _LL0;}case 429U: _LL355: _LL356: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2901 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->v=(void*)Cyc_Absyn_Bitor;_tmp645;}));
goto _LL0;}case 430U: _LL357: _LL358: {
# 2904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2906 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 431U: _LL359: _LL35A: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2908 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB52=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB51=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB50=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpB52,_tmpB51,_tmpB50,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 432U: _LL35B: _LL35C: {
# 2911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2911 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB53=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpB53,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 433U: _LL35D: _LL35E: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2914 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB54=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpB54,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 434U: _LL35F: _LL360: {
# 2917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2916 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB55=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpB55,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 435U: _LL361: _LL362: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2918 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB57=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB56=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpB57,_tmpB56,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 436U: _LL363: _LL364: {
# 2921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2920 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB59=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB58=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpB59,_tmpB58,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 437U: _LL365: _LL366: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2924 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 438U: _LL367: _LL368: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2928 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 439U: _LL369: _LL36A: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2930 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB5A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpB5B,_tmpB5A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 440U: _LL36B: _LL36C: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2934 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 441U: _LL36D: _LL36E: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2936 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB5C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpB5D,_tmpB5C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 442U: _LL36F: _LL370: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2940 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 443U: _LL371: _LL372: {
# 2943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2942 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB5E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB5F,_tmpB5E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 444U: _LL373: _LL374: {
# 2945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2946 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 445U: _LL375: _LL376: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2948 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB61=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB60=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpB61,_tmpB60,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 446U: _LL377: _LL378: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2952 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 447U: _LL379: _LL37A: {
# 2955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2954 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB63=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB62=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB63,_tmpB62,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 448U: _LL37B: _LL37C: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2958 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 449U: _LL37D: _LL37E: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2960 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB65=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB64=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpB65,_tmpB64,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 450U: _LL37F: _LL380: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2962 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB67=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB66=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpB67,_tmpB66,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 451U: _LL381: _LL382: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2966 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 452U: _LL383: _LL384: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2968 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB69=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB68=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpB69,_tmpB68,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 453U: _LL385: _LL386: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2970 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB6A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpB6B,_tmpB6A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 454U: _LL387: _LL388: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2972 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB6C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpB6D,_tmpB6C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 455U: _LL389: _LL38A: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2974 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB6E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpB6F,_tmpB6E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 456U: _LL38B: _LL38C: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2978 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 457U: _LL38D: _LL38E: {
# 2981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2980 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB71=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB70=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB71,_tmpB70,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 458U: _LL38F: _LL390: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2982 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB73=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB72=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB73,_tmpB72,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 459U: _LL391: _LL392: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2986 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 460U: _LL393: _LL394: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2988 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB75=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB74=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB75,_tmpB74,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 461U: _LL395: _LL396: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2990 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB77=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB76=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB77,_tmpB76,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 462U: _LL397: _LL398: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2994 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 463U: _LL399: _LL39A: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2996 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB79=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB78=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB79,_tmpB78,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 464U: _LL39B: _LL39C: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2998 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB7A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB7B,_tmpB7A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 465U: _LL39D: _LL39E: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3000 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB7C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB7D,_tmpB7C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 466U: _LL39F: _LL3A0: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3004 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 467U: _LL3A1: _LL3A2: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3006 "parse.y"
void*_tmp646=({struct _tuple8*_tmpB7E=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpB7E,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB80=_tmp646;struct Cyc_Absyn_Exp*_tmpB7F=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpB80,_tmpB7F,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));
# 3009
goto _LL0;}case 468U: _LL3A3: _LL3A4: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3012 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 469U: _LL3A5: _LL3A6: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3013 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB81=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpB81,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 470U: _LL3A7: _LL3A8: {
# 3016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3014 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB82=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpB82,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 471U: _LL3A9: _LL3AA: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3015 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB83=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpB83,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 472U: _LL3AB: _LL3AC: {
# 3018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3016 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB84=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpB84,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 473U: _LL3AD: _LL3AE: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3017 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB85=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpB85,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 474U: _LL3AF: _LL3B0: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3018 "parse.y"
yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpB87=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB86=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpB87,_tmpB86,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 475U: _LL3B1: _LL3B2: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3020 "parse.y"
void*_tmp647=({struct _tuple8*_tmpB88=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpB88,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(_tmp647,(unsigned int)(yyyylsp[0]).first_line));
# 3023
goto _LL0;}case 476U: _LL3B3: _LL3B4: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3023 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB89=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpB89,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 477U: _LL3B5: _LL3B6: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3025 "parse.y"
void*_tmp648=({struct _tuple8*_tmpB8A=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpB8A,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB8C=_tmp648;struct Cyc_List_List*_tmpB8B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpB8C,_tmpB8B,(unsigned int)(yyyylsp[0]).first_line);}));
# 3028
goto _LL0;}case 478U: _LL3B7: _LL3B8: {
# 3030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3030 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB8E=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->tag=34U,(_tmp649->f1).is_calloc=0,(_tmp649->f1).rgn=0,(_tmp649->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp649->f1).num_elts=_tmpB8D;}),(_tmp649->f1).fat_result=0,(_tmp649->f1).inline_call=0;_tmp649;});Cyc_Absyn_new_exp(_tmpB8E,(unsigned int)(yyyylsp[0]).first_line);}));
# 3032
goto _LL0;}case 479U: _LL3B9: _LL3BA: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3033 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB91=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->tag=34U,(_tmp64A->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp64A->f1).rgn=_tmpB90;}),(_tmp64A->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB8F=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp64A->f1).num_elts=_tmpB8F;}),(_tmp64A->f1).fat_result=0,(_tmp64A->f1).inline_call=0;_tmp64A;});Cyc_Absyn_new_exp(_tmpB91,(unsigned int)(yyyylsp[0]).first_line);}));
# 3035
goto _LL0;}case 480U: _LL3BB: _LL3BC: {
# 3037
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3036 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB94=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->tag=34U,(_tmp64B->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpB93=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp64B->f1).rgn=_tmpB93;}),(_tmp64B->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB92=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp64B->f1).num_elts=_tmpB92;}),(_tmp64B->f1).fat_result=0,(_tmp64B->f1).inline_call=1;_tmp64B;});Cyc_Absyn_new_exp(_tmpB94,(unsigned int)(yyyylsp[0]).first_line);}));
# 3038
goto _LL0;}case 481U: _LL3BD: _LL3BE: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 3039 "parse.y"
void*_tmp64C=({struct _tuple8*_tmpB95=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpB95,(unsigned int)(yyyylsp[6]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB98=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->tag=34U,(_tmp64E->f1).is_calloc=1,(_tmp64E->f1).rgn=0,({void**_tmpB97=({void**_tmp64D=_cycalloc(sizeof(*_tmp64D));*_tmp64D=_tmp64C;_tmp64D;});(_tmp64E->f1).elt_type=_tmpB97;}),({struct Cyc_Absyn_Exp*_tmpB96=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp64E->f1).num_elts=_tmpB96;}),(_tmp64E->f1).fat_result=0,(_tmp64E->f1).inline_call=0;_tmp64E;});Cyc_Absyn_new_exp(_tmpB98,(unsigned int)(yyyylsp[0]).first_line);}));
# 3042
goto _LL0;}case 482U: _LL3BF: _LL3C0: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11U));
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11U));
# 3044 "parse.y"
void*_tmp64F=({struct _tuple8*_tmpB99=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpB99,(unsigned int)(yyyylsp[8]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB9D=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->tag=34U,(_tmp651->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp651->f1).rgn=_tmpB9C;}),({void**_tmpB9B=({void**_tmp650=_cycalloc(sizeof(*_tmp650));*_tmp650=_tmp64F;_tmp650;});(_tmp651->f1).elt_type=_tmpB9B;}),({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp651->f1).num_elts=_tmpB9A;}),(_tmp651->f1).fat_result=0,(_tmp651->f1).inline_call=0;_tmp651;});Cyc_Absyn_new_exp(_tmpB9D,(unsigned int)(yyyylsp[0]).first_line);}));
# 3047
goto _LL0;}case 483U: _LL3C1: _LL3C2: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3048 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpB9F=({struct Cyc_Absyn_Exp*_tmp652[1U];({struct Cyc_Absyn_Exp*_tmpB9E=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp652[0]=_tmpB9E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp652,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpB9F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 484U: _LL3C3: _LL3C4: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3050 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA3=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->tag=38U,({struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp654->f1=_tmpBA2;}),({struct _dyneither_ptr*_tmpBA1=({struct _dyneither_ptr*_tmp653=_cycalloc(sizeof(*_tmp653));({struct _dyneither_ptr _tmpBA0=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp653=_tmpBA0;});_tmp653;});_tmp654->f2=_tmpBA1;});_tmp654;});Cyc_Absyn_new_exp(_tmpBA3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 485U: _LL3C5: _LL3C6: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3052 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA8=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->tag=38U,({struct Cyc_Absyn_Exp*_tmpBA7=({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpBA6,(unsigned int)(yyyylsp[2]).first_line);});_tmp656->f1=_tmpBA7;}),({struct _dyneither_ptr*_tmpBA5=({struct _dyneither_ptr*_tmp655=_cycalloc(sizeof(*_tmp655));({struct _dyneither_ptr _tmpBA4=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp655=_tmpBA4;});_tmp655;});_tmp656->f2=_tmpBA5;});_tmp656;});Cyc_Absyn_new_exp(_tmpBA8,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 486U: _LL3C7: _LL3C8: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3054 "parse.y"
void*_tmp657=({struct _tuple8*_tmpBA9=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpBA9,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp657,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 487U: _LL3C9: _LL3CA: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3057 "parse.y"
struct _tuple20 _tmp658=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp659=_tmp658;int _tmp65B;struct _dyneither_ptr _tmp65A;_LL49C: _tmp65B=_tmp659.f1;_tmp65A=_tmp659.f2;_LL49D:;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp65B,_tmp65A,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3061
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3063
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3059 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_extension_exp(_tmpBAA,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3063 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;}case 490U: _LL3CF: _LL3D0: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3064 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;}case 491U: _LL3D1: _LL3D2: {
# 3067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3065 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;}case 492U: _LL3D3: _LL3D4: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3070 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 493U: _LL3D5: _LL3D6: {
# 3073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3072 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpBAC,_tmpBAB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 494U: _LL3D7: _LL3D8: {
# 3075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3074 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpBAD,0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 495U: _LL3D9: _LL3DA: {
# 3077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3076 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpBAE=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpBAF,_tmpBAE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 496U: _LL3DB: _LL3DC: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3078 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpBB1=({struct _dyneither_ptr*_tmp65C=_cycalloc(sizeof(*_tmp65C));({struct _dyneither_ptr _tmpBB0=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp65C=_tmpBB0;});_tmp65C;});Cyc_Absyn_aggrmember_exp(_tmpBB2,_tmpBB1,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 497U: _LL3DD: _LL3DE: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3080 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpBB4=({struct _dyneither_ptr*_tmp65D=_cycalloc(sizeof(*_tmp65D));({struct _dyneither_ptr _tmpBB3=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp65D=_tmpBB3;});_tmp65D;});Cyc_Absyn_aggrarrow_exp(_tmpBB5,_tmpBB4,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 498U: _LL3DF: _LL3E0: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3082 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpBB6,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 499U: _LL3E1: _LL3E2: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3084 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpBB7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 500U: _LL3E3: _LL3E4: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3086 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBB9=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->tag=25U,({struct _tuple8*_tmpBB8=Cyc_yyget_YY37(yyyyvsp[1]);_tmp65E->f1=_tmpBB8;}),_tmp65E->f2=0;_tmp65E;});Cyc_Absyn_new_exp(_tmpBB9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 501U: _LL3E5: _LL3E6: {
# 3089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3088 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBBC=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->tag=25U,({struct _tuple8*_tmpBBB=Cyc_yyget_YY37(yyyyvsp[1]);_tmp65F->f1=_tmpBBB;}),({struct Cyc_List_List*_tmpBBA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp65F->f2=_tmpBBA;});_tmp65F;});Cyc_Absyn_new_exp(_tmpBBC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 502U: _LL3E7: _LL3E8: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 3090 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBBF=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->tag=25U,({struct _tuple8*_tmpBBE=Cyc_yyget_YY37(yyyyvsp[1]);_tmp660->f1=_tmpBBE;}),({struct Cyc_List_List*_tmpBBD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp660->f2=_tmpBBD;});_tmp660;});Cyc_Absyn_new_exp(_tmpBBF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 503U: _LL3E9: _LL3EA: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3095 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp663=_cycalloc(sizeof(*_tmp663));({void*_tmpBC2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->tag=0U,({struct _dyneither_ptr*_tmpBC1=({struct _dyneither_ptr*_tmp661=_cycalloc(sizeof(*_tmp661));({struct _dyneither_ptr _tmpBC0=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp661=_tmpBC0;});_tmp661;});_tmp662->f1=_tmpBC1;});_tmp662;});_tmp663->hd=_tmpBC2;}),_tmp663->tl=0;_tmp663;}));
goto _LL0;}case 504U: _LL3EB: _LL3EC: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3098 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp665=_cycalloc(sizeof(*_tmp665));({void*_tmpBC5=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->tag=1U,({unsigned int _tmpBC4=({unsigned int _tmpBC3=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpBC3,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp664->f1=_tmpBC4;});_tmp664;});_tmp665->hd=_tmpBC5;}),_tmp665->tl=0;_tmp665;}));
goto _LL0;}case 505U: _LL3ED: _LL3EE: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3100 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp668=_cycalloc(sizeof(*_tmp668));({void*_tmpBC9=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tag=0U,({struct _dyneither_ptr*_tmpBC8=({struct _dyneither_ptr*_tmp666=_cycalloc(sizeof(*_tmp666));({struct _dyneither_ptr _tmpBC7=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp666=_tmpBC7;});_tmp666;});_tmp667->f1=_tmpBC8;});_tmp667;});_tmp668->hd=_tmpBC9;}),({struct Cyc_List_List*_tmpBC6=Cyc_yyget_YY3(yyyyvsp[0]);_tmp668->tl=_tmpBC6;});_tmp668;}));
goto _LL0;}case 506U: _LL3EF: _LL3F0: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3103 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp66A=_cycalloc(sizeof(*_tmp66A));({void*_tmpBCD=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tag=1U,({unsigned int _tmpBCC=({unsigned int _tmpBCB=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpBCB,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp669->f1=_tmpBCC;});_tmp669;});_tmp66A->hd=_tmpBCD;}),({struct Cyc_List_List*_tmpBCA=Cyc_yyget_YY3(yyyyvsp[0]);_tmp66A->tl=_tmpBCA;});_tmp66A;}));
goto _LL0;}case 507U: _LL3F1: _LL3F2: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_Exp_tok(({struct _tuple0*_tmpBCE=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpBCE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 508U: _LL3F3: _LL3F4: {
# 3112
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3111 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBCF=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Absyn_pragma_exp(_tmpBCF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 509U: _LL3F5: _LL3F6: {
# 3114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3113 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 510U: _LL3F7: _LL3F8: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3115 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBD0=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpBD0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 511U: _LL3F9: _LL3FA: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3117 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBD1=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpBD1,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 512U: _LL3FB: _LL3FC: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3119 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 513U: _LL3FD: _LL3FE: {
# 3122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3124 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD2=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpBD2,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 514U: _LL3FF: _LL400: {
# 3127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3126 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD4=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpBD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpBD4,_tmpBD3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 515U: _LL401: _LL402: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3129 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBD5=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpBD5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 516U: _LL403: _LL404: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3132 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBD9=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->tag=29U,({struct _tuple0*_tmpBD8=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp66B->f1=_tmpBD8;}),({struct Cyc_List_List*_tmpBD7=Cyc_yyget_YY40(yyyyvsp[2]);_tmp66B->f2=_tmpBD7;}),({struct Cyc_List_List*_tmpBD6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp66B->f3=_tmpBD6;}),_tmp66B->f4=0;_tmp66B;});Cyc_Absyn_new_exp(_tmpBD9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 517U: _LL405: _LL406: {
# 3135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3135 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpBDA=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpBDA,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 518U: _LL407: _LL408: {
# 3138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3139 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
goto _LL0;}case 519U: _LL409: _LL40A: {
# 3142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct Cyc_Absyn_Exp*_tmpBDB=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp66C->hd=_tmpBDB;}),_tmp66C->tl=0;_tmp66C;}));
goto _LL0;}case 520U: _LL40B: _LL40C: {
# 3148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3147 "parse.y"
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp66D=_cycalloc(sizeof(*_tmp66D));({struct Cyc_Absyn_Exp*_tmpBDD=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp66D->hd=_tmpBDD;}),({struct Cyc_List_List*_tmpBDC=Cyc_yyget_YY4(yyyyvsp[0]);_tmp66D->tl=_tmpBDC;});_tmp66D;}));
goto _LL0;}case 521U: _LL40D: _LL40E: {
# 3150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpBDE=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpBDE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 522U: _LL40F: _LL410: {
# 3156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3154 "parse.y"
yyval=Cyc_Exp_tok(({char _tmpBDF=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpBDF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 523U: _LL411: _LL412: {
# 3157
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3159
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3155 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBE0=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpBE0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 524U: _LL413: _LL414: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3157 "parse.y"
struct _dyneither_ptr _tmp66E=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp66E);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp66E,sizeof(char),l - 1));
if((int)c == (int)'f'  || (int)c == (int)'F')i=0;else{
if((int)c == (int)'l'  || (int)c == (int)'L')i=2;}}
# 3165
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp66E,i,(unsigned int)(yyyylsp[0]).first_line));
# 3167
goto _LL0;}case 525U: _LL415: _LL416: {
# 3169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3168 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 526U: _LL417: _LL418: {
# 3171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3172 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp670=_cycalloc(sizeof(*_tmp670));({union Cyc_Absyn_Nmspace _tmpBE3=Cyc_Absyn_Rel_n(0);_tmp670->f1=_tmpBE3;}),({struct _dyneither_ptr*_tmpBE2=({struct _dyneither_ptr*_tmp66F=_cycalloc(sizeof(*_tmp66F));({struct _dyneither_ptr _tmpBE1=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp66F=_tmpBE1;});_tmp66F;});_tmp670->f2=_tmpBE2;});_tmp670;}));
goto _LL0;}case 527U: _LL419: _LL41A: {
# 3175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3173 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 528U: _LL41B: _LL41C: {
# 3176
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3176 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp672=_cycalloc(sizeof(*_tmp672));({union Cyc_Absyn_Nmspace _tmpBE6=Cyc_Absyn_Rel_n(0);_tmp672->f1=_tmpBE6;}),({struct _dyneither_ptr*_tmpBE5=({struct _dyneither_ptr*_tmp671=_cycalloc(sizeof(*_tmp671));({struct _dyneither_ptr _tmpBE4=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp671=_tmpBE4;});_tmp671;});_tmp672->f2=_tmpBE5;});_tmp672;}));
goto _LL0;}case 529U: _LL41D: _LL41E: {
# 3179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3177 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 530U: _LL41F: _LL420: {
# 3180
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3182 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 531U: _LL421: _LL422: {
# 3185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3183 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 532U: _LL423: _LL424: {
# 3186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3186 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 533U: _LL425: _LL426: {
# 3189
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3191
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3187 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 534U: _LL427: _LL428: {
# 3190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3192 "parse.y"
goto _LL0;}case 535U: _LL429: _LL42A: {
# 3194
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3192 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL0;}default: _LL42B: _LL42C:
# 3197
 goto _LL0;}_LL0:;}
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 378
yylsp_offset -=yylen;
# 392 "cycbison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 395
++ yylsp_offset;
if(yylen == 0){
# 398
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
({int _tmpBE7=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpBE7;});
({int _tmpBE8=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=_tmpBE8;});}else{
# 405
({int _tmpBE9=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpBE9;});
({int _tmpBEA=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=_tmpBEA;});}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
yystate=({int _tmpBEB=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,151U,sizeof(short),yyn - 150));_tmpBEB + (int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0  && yystate <= 7749) && ({int _tmpBEC=(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),yystate));_tmpBEC == (int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));}))
yystate=(int)Cyc_yytable[yystate];else{
# 421
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,151U,sizeof(short),yyn - 150));}
# 423
goto yynewstate;
# 425
yyerrlab:
# 427
 if(yyerrstatus == 0){
# 430
++ yynerrs;
# 433
yyn=(int)Cyc_yypact[yystate];
# 435
if(yyn > - 32768  && yyn < 7749){
# 437
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 441
count=0;
# 443
for(x=yyn < 0?- yyn: 0;(unsigned int)x < 301U / sizeof(char*);++ x){
# 445
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),x + yyn))== x)
({unsigned long _tmpBED=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_known_subscript_notnull(Cyc_yytname,301U,sizeof(struct _dyneither_ptr),x)))+ (unsigned long)15;sze +=_tmpBED;}),count ++;}
msg=({unsigned int _tmp674=(unsigned int)(sze + 15)+ 1U;char*_tmp673=({struct _RegionHandle*_tmpBEE=yyregion;_region_malloc(_tmpBEE,_check_times(_tmp674,sizeof(char)));});({{unsigned int _tmp72E=(unsigned int)(sze + 15);unsigned int i;for(i=0;i < _tmp72E;++ i){_tmp673[i]='\000';}_tmp673[_tmp72E]=0;}0;});_tag_dyneither(_tmp673,sizeof(char),_tmp674);});
({struct _dyneither_ptr _tmpBEF=msg;Cyc_strcpy(_tmpBEF,({const char*_tmp675="parse error";_tag_dyneither(_tmp675,sizeof(char),12U);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned int)x < 301U / sizeof(char*);++ x){
# 455
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),x + yyn))== x){
# 457
({struct _dyneither_ptr _tmpBF0=msg;Cyc_strcat(_tmpBF0,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp676=", expecting `";_tag_dyneither(_tmp676,sizeof(char),14U);}):(struct _dyneither_ptr)({const char*_tmp677=" or `";_tag_dyneither(_tmp677,sizeof(char),6U);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_known_subscript_notnull(Cyc_yytname,301U,sizeof(struct _dyneither_ptr),x)));
({struct _dyneither_ptr _tmpBF1=msg;Cyc_strcat(_tmpBF1,({const char*_tmp678="'";_tag_dyneither(_tmp678,sizeof(char),2U);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpBF3=({const char*_tmp679="parse error";_tag_dyneither(_tmp679,sizeof(char),12U);});int _tmpBF2=yystate;Cyc_yyerror(_tmpBF3,_tmpBF2,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp67A=1;_npop_handler(0U);return _tmp67A;}
# 489
yychar=-2;}
# 495
yyerrstatus=3;
# 497
goto yyerrhandle;
# 499
yyerrdefault:
# 509 "cycbison.simple"
 yyerrpop:
# 511
 if(yyssp_offset == 0){int _tmp67B=1;_npop_handler(0U);return _tmp67B;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1092U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7749) || (int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7750U,sizeof(short),yyn))!= 1)goto yyerrdefault;
# 537
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 540
if(yyn == -32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 544
if(yyn == 0)goto yyerrpop;}
# 546
if(yyn == 1091){
int _tmp67C=0;_npop_handler(0U);return _tmp67C;}
# 554
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 556
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 559
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3195 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp682=v;struct Cyc_Absyn_Stmt*_tmp69C;struct Cyc_Absyn_Exp*_tmp69B;struct _tuple0*_tmp69A;struct _dyneither_ptr _tmp699;char _tmp698;union Cyc_Absyn_Cnst _tmp697;switch((_tmp682.Stmt_tok).tag){case 1U: _LL1: _tmp697=(_tmp682.Int_tok).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp685=({struct Cyc_String_pa_PrintArg_struct _tmp732;_tmp732.tag=0U,({struct _dyneither_ptr _tmpBF4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp697));_tmp732.f1=_tmpBF4;});_tmp732;});void*_tmp683[1U];_tmp683[0]=& _tmp685;({struct Cyc___cycFILE*_tmpBF6=Cyc_stderr;struct _dyneither_ptr _tmpBF5=({const char*_tmp684="%s";_tag_dyneither(_tmp684,sizeof(char),3U);});Cyc_fprintf(_tmpBF6,_tmpBF5,_tag_dyneither(_tmp683,sizeof(void*),1U));});});goto _LL0;case 2U: _LL3: _tmp698=(_tmp682.Char_tok).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp688=({struct Cyc_Int_pa_PrintArg_struct _tmp733;_tmp733.tag=1U,_tmp733.f1=(unsigned long)((int)_tmp698);_tmp733;});void*_tmp686[1U];_tmp686[0]=& _tmp688;({struct Cyc___cycFILE*_tmpBF8=Cyc_stderr;struct _dyneither_ptr _tmpBF7=({const char*_tmp687="%c";_tag_dyneither(_tmp687,sizeof(char),3U);});Cyc_fprintf(_tmpBF8,_tmpBF7,_tag_dyneither(_tmp686,sizeof(void*),1U));});});goto _LL0;case 3U: _LL5: _tmp699=(_tmp682.String_tok).val;_LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp68B=({struct Cyc_String_pa_PrintArg_struct _tmp734;_tmp734.tag=0U,_tmp734.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp699);_tmp734;});void*_tmp689[1U];_tmp689[0]=& _tmp68B;({struct Cyc___cycFILE*_tmpBFA=Cyc_stderr;struct _dyneither_ptr _tmpBF9=({const char*_tmp68A="\"%s\"";_tag_dyneither(_tmp68A,sizeof(char),5U);});Cyc_fprintf(_tmpBFA,_tmpBF9,_tag_dyneither(_tmp689,sizeof(void*),1U));});});goto _LL0;case 5U: _LL7: _tmp69A=(_tmp682.QualId_tok).val;_LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp68E=({struct Cyc_String_pa_PrintArg_struct _tmp735;_tmp735.tag=0U,({struct _dyneither_ptr _tmpBFB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69A));_tmp735.f1=_tmpBFB;});_tmp735;});void*_tmp68C[1U];_tmp68C[0]=& _tmp68E;({struct Cyc___cycFILE*_tmpBFD=Cyc_stderr;struct _dyneither_ptr _tmpBFC=({const char*_tmp68D="%s";_tag_dyneither(_tmp68D,sizeof(char),3U);});Cyc_fprintf(_tmpBFD,_tmpBFC,_tag_dyneither(_tmp68C,sizeof(void*),1U));});});goto _LL0;case 7U: _LL9: _tmp69B=(_tmp682.Exp_tok).val;_LLA:
({struct Cyc_String_pa_PrintArg_struct _tmp691=({struct Cyc_String_pa_PrintArg_struct _tmp736;_tmp736.tag=0U,({struct _dyneither_ptr _tmpBFE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp69B));_tmp736.f1=_tmpBFE;});_tmp736;});void*_tmp68F[1U];_tmp68F[0]=& _tmp691;({struct Cyc___cycFILE*_tmpC00=Cyc_stderr;struct _dyneither_ptr _tmpBFF=({const char*_tmp690="%s";_tag_dyneither(_tmp690,sizeof(char),3U);});Cyc_fprintf(_tmpC00,_tmpBFF,_tag_dyneither(_tmp68F,sizeof(void*),1U));});});goto _LL0;case 8U: _LLB: _tmp69C=(_tmp682.Stmt_tok).val;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp694=({struct Cyc_String_pa_PrintArg_struct _tmp737;_tmp737.tag=0U,({struct _dyneither_ptr _tmpC01=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp69C));_tmp737.f1=_tmpC01;});_tmp737;});void*_tmp692[1U];_tmp692[0]=& _tmp694;({struct Cyc___cycFILE*_tmpC03=Cyc_stderr;struct _dyneither_ptr _tmpC02=({const char*_tmp693="%s";_tag_dyneither(_tmp693,sizeof(char),3U);});Cyc_fprintf(_tmpC03,_tmpC02,_tag_dyneither(_tmp692,sizeof(void*),1U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp695=0U;({struct Cyc___cycFILE*_tmpC05=Cyc_stderr;struct _dyneither_ptr _tmpC04=({const char*_tmp696="?";_tag_dyneither(_tmp696,sizeof(char),2U);});Cyc_fprintf(_tmpC05,_tmpC04,_tag_dyneither(_tmp695,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3207
struct _dyneither_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp69D="end-of-file";_tag_dyneither(_tmp69D,sizeof(char),12U);});
if(token == 365)
return Cyc_Lex_token_string;else{
if(token == 374)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0  && token <= 377?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,378U,sizeof(short),token)): 301;
if((unsigned int)z < 301U)
return Cyc_yytname[z];else{
return _tag_dyneither(0,0,0);}};}
# 3221
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp69E=_new_region("yyr");struct _RegionHandle*yyr=& _tmp69E;_push_region(yyr);
({struct _RegionHandle*_tmpC06=yyr;Cyc_yyparse(_tmpC06,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp69F=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp69F;};
# 3224
;_pop_region(yyr);};}
