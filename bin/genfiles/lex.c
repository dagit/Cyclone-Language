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
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 197 "cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(void*ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 233
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_ukey(const char*file,const char*func,int lineno);
# 253 "core.h"
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion*k);
# 263
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 84 "lexing.h"
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 127
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 73
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 108 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 42 "xarray.h"
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
# 52 "parse.h"
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0U,Cyc_Extern_sc  = 1U,Cyc_ExternC_sc  = 2U,Cyc_Static_sc  = 3U,Cyc_Auto_sc  = 4U,Cyc_Register_sc  = 5U,Cyc_Abstract_sc  = 6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 83 "parse_tab.h"
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 110 "dict.h"
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 30 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Lex_Trie;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple19{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple19 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 50 "lex.cyl"
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){return({union Cyc_Lex_TrieChildren _tmp13F;(_tmp13F.Zero).tag=1U,(_tmp13F.Zero).val=0;_tmp13F;});}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
return({union Cyc_Lex_TrieChildren _tmp140;(_tmp140.One).tag=2U,((_tmp140.One).val).f1=i,((_tmp140.One).val).f2=t;_tmp140;});}
# 54
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
return({union Cyc_Lex_TrieChildren _tmp141;(_tmp141.Many).tag=3U,(_tmp141.Many).val=ts;_tmp141;});}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};struct Cyc_Lex_DynTrie{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;};struct Cyc_Lex_DynTrieSymbols{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 78
static int Cyc_Lex_num_kws=0;struct Cyc_Lex_KeyWordInfo{int token_index;int is_c_keyword;};
# 80
static struct _dyneither_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};
# 82
int Cyc_Lex_compile_for_boot_r=0;
int Cyc_Lex_expand_specials=0;
# 85
static int Cyc_Lex_in_extern_c=0;
static struct Cyc_List_List*Cyc_Lex_prev_extern_c=0;
void Cyc_Lex_enter_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp0=x;struct Cyc_List_List*_tmp1=Cyc_Lex_prev_extern_c;x=_tmp1;Cyc_Lex_prev_extern_c=_tmp0;});
({struct Cyc_List_List*_tmp166=({struct Cyc_List_List*_tmp2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp2));_tmp2->hd=(void*)Cyc_Lex_in_extern_c,_tmp2->tl=x;_tmp2;});Cyc_Lex_prev_extern_c=_tmp166;});
Cyc_Lex_in_extern_c=1;}
# 93
void Cyc_Lex_leave_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp3=x;struct Cyc_List_List*_tmp4=Cyc_Lex_prev_extern_c;x=_tmp4;Cyc_Lex_prev_extern_c=_tmp3;});
Cyc_Lex_in_extern_c=(int)((struct Cyc_List_List*)_check_null(x))->hd;
Cyc_Lex_prev_extern_c=x->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);}
# 101
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 104
static int Cyc_Lex_comment_depth=0;
# 107
static union Cyc_Absyn_Cnst Cyc_Lex_token_int={.Int_c={5,{Cyc_Absyn_Signed,0}}};static char _tmp5[8U]="*bogus*";
static struct _dyneither_ptr Cyc_Lex_bogus_string={_tmp5,_tmp5,_tmp5 + 8U};
static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};
# 112
static char Cyc_Lex_token_char='\000';static char _tmp6[1U]="";
struct _dyneither_ptr Cyc_Lex_token_string={_tmp6,_tmp6,_tmp6 + 1U};
struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static char _tmp7[1U]="";
static struct _tuple12 Cyc_Lex_token_asm={0,{_tmp7,_tmp7,_tmp7 + 1U}};
# 117
static int Cyc_Lex_runaway_start=0;
static int Cyc_Lex_paren_depth=0;
# 120
static void Cyc_Lex_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lexing_lexeme_start(lb);
({void*_tmp8=0U;({unsigned int _tmp168=s;struct _dyneither_ptr _tmp167=msg;Cyc_Warn_err(_tmp168,_tmp167,_tag_dyneither(_tmp8,sizeof(void*),0U));});});}
# 124
static void Cyc_Lex_runaway_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned int s=(unsigned int)Cyc_Lex_runaway_start;
({void*_tmp9=0U;({unsigned int _tmp16A=s;struct _dyneither_ptr _tmp169=msg;Cyc_Warn_err(_tmp16A,_tmp169,_tag_dyneither(_tmp9,sizeof(void*),0U));});});}
# 128
static char Cyc_Lex_cnst2char(union Cyc_Absyn_Cnst cnst,struct Cyc_Lexing_lexbuf*lb){
union Cyc_Absyn_Cnst _tmpA=cnst;char _tmpE;long long _tmpD;int _tmpC;switch((_tmpA.Char_c).tag){case 5U: _LL1: _tmpC=((_tmpA.Int_c).val).f2;_LL2:
 return(char)_tmpC;case 6U: _LL3: _tmpD=((_tmpA.LongLong_c).val).f2;_LL4:
 return(char)_tmpD;case 2U: _LL5: _tmpE=((_tmpA.Char_c).val).f2;_LL6:
 return _tmpE;default: _LL7: _LL8:
({struct _dyneither_ptr _tmp16B=({const char*_tmpB="couldn't convert constant to char!";_tag_dyneither(_tmpB,sizeof(char),35U);});Cyc_Lex_err(_tmp16B,lb);});return'\000';}_LL0:;}static char _tmpF[14U]="__attribute__";static char _tmp10[9U]="abstract";static char _tmp11[5U]="auto";static char _tmp12[6U]="break";static char _tmp13[18U]="__builtin_va_list";static char _tmp14[7U]="calloc";static char _tmp15[5U]="case";static char _tmp16[6U]="catch";static char _tmp17[5U]="char";static char _tmp18[6U]="const";static char _tmp19[10U]="__const__";static char _tmp1A[9U]="continue";static char _tmp1B[9U]="datatype";static char _tmp1C[8U]="default";static char _tmp1D[3U]="do";static char _tmp1E[7U]="double";static char _tmp1F[12U]="dynregion_t";static char _tmp20[5U]="else";static char _tmp21[5U]="enum";static char _tmp22[7U]="export";static char _tmp23[7U]="extern";static char _tmp24[9U]="fallthru";static char _tmp25[6U]="float";static char _tmp26[4U]="for";static char _tmp27[5U]="goto";static char _tmp28[3U]="if";static char _tmp29[7U]="inline";static char _tmp2A[11U]="__inline__";static char _tmp2B[4U]="int";static char _tmp2C[4U]="let";static char _tmp2D[5U]="long";static char _tmp2E[7U]="malloc";static char _tmp2F[10U]="namespace";static char _tmp30[4U]="new";static char _tmp31[5U]="NULL";static char _tmp32[8U]="numelts";static char _tmp33[9U]="offsetof";static char _tmp34[20U]="__cyclone_port_on__";static char _tmp35[21U]="__cyclone_port_off__";static char _tmp36[19U]="__cyclone_pragma__";static char _tmp37[8U]="rcalloc";static char _tmp38[9U]="region_t";static char _tmp39[7U]="region";static char _tmp3A[8U]="regions";static char _tmp3B[9U]="register";static char _tmp3C[9U]="restrict";static char _tmp3D[7U]="return";static char _tmp3E[8U]="rmalloc";static char _tmp3F[15U]="rmalloc_inline";static char _tmp40[5U]="rnew";static char _tmp41[6U]="short";static char _tmp42[7U]="signed";static char _tmp43[11U]="__signed__";static char _tmp44[7U]="sizeof";static char _tmp45[7U]="static";static char _tmp46[7U]="struct";static char _tmp47[7U]="switch";static char _tmp48[9U]="tagcheck";static char _tmp49[6U]="tag_t";static char _tmp4A[6U]="throw";static char _tmp4B[4U]="try";static char _tmp4C[8U]="typedef";static char _tmp4D[7U]="typeof";static char _tmp4E[11U]="__typeof__";static char _tmp4F[6U]="union";static char _tmp50[9U]="unsigned";static char _tmp51[13U]="__unsigned__";static char _tmp52[6U]="using";static char _tmp53[8U]="valueof";static char _tmp54[10U]="valueof_t";static char _tmp55[5U]="void";static char _tmp56[9U]="volatile";static char _tmp57[13U]="__volatile__";static char _tmp58[6U]="while";struct _tuple20{struct _dyneither_ptr f1;short f2;int f3;};
# 139
static struct _tuple20 Cyc_Lex_rw_array[74U]={{{_tmpF,_tmpF,_tmpF + 14U},373,1},{{_tmp10,_tmp10,_tmp10 + 9U},302,0},{{_tmp11,_tmp11,_tmp11 + 5U},258,1},{{_tmp12,_tmp12,_tmp12 + 6U},290,1},{{_tmp13,_tmp13,_tmp13 + 18U},294,1},{{_tmp14,_tmp14,_tmp14 + 7U},310,0},{{_tmp15,_tmp15,_tmp15 + 5U},277,1},{{_tmp16,_tmp16,_tmp16 + 6U},299,1},{{_tmp17,_tmp17,_tmp17 + 5U},264,1},{{_tmp18,_tmp18,_tmp18 + 6U},272,1},{{_tmp19,_tmp19,_tmp19 + 10U},272,1},{{_tmp1A,_tmp1A,_tmp1A + 9U},289,1},{{_tmp1B,_tmp1B,_tmp1B + 9U},306,0},{{_tmp1C,_tmp1C,_tmp1C + 8U},278,1},{{_tmp1D,_tmp1D,_tmp1D + 3U},286,1},{{_tmp1E,_tmp1E,_tmp1E + 7U},269,1},{{_tmp1F,_tmp1F,_tmp1F + 12U},321,0},{{_tmp20,_tmp20,_tmp20 + 5U},283,1},{{_tmp21,_tmp21,_tmp21 + 5U},292,1},{{_tmp22,_tmp22,_tmp22 + 7U},300,0},{{_tmp23,_tmp23,_tmp23 + 7U},261,1},{{_tmp24,_tmp24,_tmp24 + 9U},303,0},{{_tmp25,_tmp25,_tmp25 + 6U},268,1},{{_tmp26,_tmp26,_tmp26 + 4U},287,1},{{_tmp27,_tmp27,_tmp27 + 5U},288,1},{{_tmp28,_tmp28,_tmp28 + 3U},282,1},{{_tmp29,_tmp29,_tmp29 + 7U},279,1},{{_tmp2A,_tmp2A,_tmp2A + 11U},279,1},{{_tmp2B,_tmp2B,_tmp2B + 4U},266,1},{{_tmp2C,_tmp2C,_tmp2C + 4U},296,0},{{_tmp2D,_tmp2D,_tmp2D + 5U},267,1},{{_tmp2E,_tmp2E,_tmp2E + 7U},307,0},{{_tmp2F,_tmp2F,_tmp2F + 10U},305,0},{{_tmp30,_tmp30,_tmp30 + 4U},301,0},{{_tmp31,_tmp31,_tmp31 + 5U},295,0},{{_tmp32,_tmp32,_tmp32 + 8U},322,0},{{_tmp33,_tmp33,_tmp33 + 9U},281,1},{{_tmp34,_tmp34,_tmp34 + 20U},318,0},{{_tmp35,_tmp35,_tmp35 + 21U},319,0},{{_tmp36,_tmp36,_tmp36 + 19U},320,0},{{_tmp37,_tmp37,_tmp37 + 8U},311,0},{{_tmp38,_tmp38,_tmp38 + 9U},313,0},{{_tmp39,_tmp39,_tmp39 + 7U},315,0},{{_tmp3A,_tmp3A,_tmp3A + 8U},317,0},{{_tmp3B,_tmp3B,_tmp3B + 9U},259,1},{{_tmp3C,_tmp3C,_tmp3C + 9U},274,1},{{_tmp3D,_tmp3D,_tmp3D + 7U},291,1},{{_tmp3E,_tmp3E,_tmp3E + 8U},308,0},{{_tmp3F,_tmp3F,_tmp3F + 15U},309,0},{{_tmp40,_tmp40,_tmp40 + 5U},316,0},{{_tmp41,_tmp41,_tmp41 + 6U},265,1},{{_tmp42,_tmp42,_tmp42 + 7U},270,1},{{_tmp43,_tmp43,_tmp43 + 11U},270,1},{{_tmp44,_tmp44,_tmp44 + 7U},280,1},{{_tmp45,_tmp45,_tmp45 + 7U},260,1},{{_tmp46,_tmp46,_tmp46 + 7U},275,1},{{_tmp47,_tmp47,_tmp47 + 7U},284,1},{{_tmp48,_tmp48,_tmp48 + 9U},325,0},{{_tmp49,_tmp49,_tmp49 + 6U},314,0},{{_tmp4A,_tmp4A,_tmp4A + 6U},297,0},{{_tmp4B,_tmp4B,_tmp4B + 4U},298,0},{{_tmp4C,_tmp4C,_tmp4C + 8U},262,1},{{_tmp4D,_tmp4D,_tmp4D + 7U},293,1},{{_tmp4E,_tmp4E,_tmp4E + 11U},293,1},{{_tmp4F,_tmp4F,_tmp4F + 6U},276,1},{{_tmp50,_tmp50,_tmp50 + 9U},271,1},{{_tmp51,_tmp51,_tmp51 + 13U},271,1},{{_tmp52,_tmp52,_tmp52 + 6U},304,0},{{_tmp53,_tmp53,_tmp53 + 8U},323,0},{{_tmp54,_tmp54,_tmp54 + 10U},324,0},{{_tmp55,_tmp55,_tmp55 + 5U},263,1},{{_tmp56,_tmp56,_tmp56 + 9U},273,1},{{_tmp57,_tmp57,_tmp57 + 13U},273,1},{{_tmp58,_tmp58,_tmp58 + 6U},285,1}};
# 216
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned int i=0U;for(0;i < 74U;++ i){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)
++ sum;}}
return sum;}
# 225
static struct Cyc_Lex_Trie*Cyc_Lex_empty_trie(struct _RegionHandle*d,int dummy){
return({struct Cyc_Lex_Trie*_tmp59=_region_malloc(d,sizeof(*_tmp59));({union Cyc_Lex_TrieChildren _tmp16C=Cyc_Lex_Zero();_tmp59->children=_tmp16C;}),_tmp59->shared_str=0;_tmp59;});}
# 229
static int Cyc_Lex_trie_char(int c){
# 231
if(c >= 95)return c - 59;else{
if(c > 64)return c - 55;}
return c - 48;}
# 236
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){
# 238
int i=offset;
int last=(offset + len)- 1;
while(i <= last){
union Cyc_Lex_TrieChildren _tmp5A=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _tmp5B=_tmp5A;int _tmp66;struct Cyc_Lex_Trie*_tmp65;struct Cyc_Lex_Trie**_tmp64;switch((_tmp5B.One).tag){case 3U: _LL1: _tmp64=(_tmp5B.Many).val;_LL2: {
# 244
int ch=Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));
if(_tmp64[_check_known_subscript_notnull(64U,ch)]== 0)
({struct Cyc_Lex_Trie*_tmp16E=({struct Cyc_Lex_Trie*_tmp5C=_region_malloc(r,sizeof(*_tmp5C));({union Cyc_Lex_TrieChildren _tmp16D=Cyc_Lex_Zero();_tmp5C->children=_tmp16D;}),_tmp5C->shared_str=0;_tmp5C;});_tmp64[ch]=_tmp16E;});
t=_tmp64[ch];
++ i;
goto _LL0;}case 2U: _LL3: _tmp66=((_tmp5B.One).val).f1;_tmp65=((_tmp5B.One).val).f2;_LL4:
# 252
 if(_tmp66 == *((const char*)_check_dyneither_subscript(buff,sizeof(char),i)))
t=_tmp65;else{
# 255
struct Cyc_Lex_Trie**_tmp5D=({unsigned int _tmp61=64U;struct Cyc_Lex_Trie**_tmp60=({struct _RegionHandle*_tmp16F=r;_region_malloc(_tmp16F,_check_times(_tmp61,sizeof(struct Cyc_Lex_Trie*)));});({{unsigned int _tmp142=64U;unsigned int j;for(j=0;j < _tmp142;++ j){_tmp60[j]=0;}}0;});_tmp60;});
_tmp5D[_check_known_subscript_notnull(64U,Cyc_Lex_trie_char(_tmp66))]=_tmp65;{
int _tmp5E=Cyc_Lex_trie_char((int)((const char*)buff.curr)[i]);
({struct Cyc_Lex_Trie*_tmp171=({struct Cyc_Lex_Trie*_tmp5F=_region_malloc(r,sizeof(*_tmp5F));({union Cyc_Lex_TrieChildren _tmp170=Cyc_Lex_Zero();_tmp5F->children=_tmp170;}),_tmp5F->shared_str=0;_tmp5F;});_tmp5D[_check_known_subscript_notnull(64U,_tmp5E)]=_tmp171;});
({union Cyc_Lex_TrieChildren _tmp172=Cyc_Lex_Many(_tmp5D);t->children=_tmp172;});
t=_tmp5D[_tmp5E];};}
# 262
++ i;
goto _LL0;default: _LL5: _LL6:
# 266
 while(i <= last){
struct Cyc_Lex_Trie*_tmp62=({struct Cyc_Lex_Trie*_tmp63=_region_malloc(r,sizeof(*_tmp63));({union Cyc_Lex_TrieChildren _tmp173=Cyc_Lex_Zero();_tmp63->children=_tmp173;}),_tmp63->shared_str=0;_tmp63;});
({union Cyc_Lex_TrieChildren _tmp174=Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i ++)),_tmp62);t->children=_tmp174;});
t=_tmp62;}
# 271
return t;}_LL0:;}
# 274
return t;}struct _tuple21{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;struct _dyneither_ptr f3;int f4;int f5;};
# 277
static int Cyc_Lex_str_index_body(struct _RegionHandle*d,struct _tuple21*env){
# 280
struct _tuple21 _tmp67=*env;struct _tuple21 _tmp68=_tmp67;struct Cyc_Lex_Trie*_tmp70;struct Cyc_Xarray_Xarray*_tmp6F;struct _dyneither_ptr _tmp6E;int _tmp6D;int _tmp6C;_LL1: _tmp70=_tmp68.f1;_tmp6F=_tmp68.f2;_tmp6E=_tmp68.f3;_tmp6D=_tmp68.f4;_tmp6C=_tmp68.f5;_LL2:;{
struct Cyc_Lex_Trie*_tmp69=Cyc_Lex_trie_lookup(d,_tmp70,_tmp6E,_tmp6D,_tmp6C);
# 283
if(((struct Cyc_Lex_Trie*)_check_null(_tmp69))->shared_str == 0){
struct _dyneither_ptr _tmp6A=Cyc_Core_new_string((unsigned int)(_tmp6C + 1));
({struct _dyneither_ptr _tmp176=_dyneither_ptr_decrease_size(_tmp6A,sizeof(char),1U);struct _dyneither_ptr _tmp175=(struct _dyneither_ptr)_dyneither_ptr_plus(_tmp6E,sizeof(char),_tmp6D);Cyc_zstrncpy(_tmp176,_tmp175,(unsigned long)_tmp6C);});{
int ans=({struct Cyc_Xarray_Xarray*_tmp177=_tmp6F;((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp177,({struct _dyneither_ptr*_tmp6B=_cycalloc(sizeof(*_tmp6B));*_tmp6B=(struct _dyneither_ptr)_tmp6A;_tmp6B;}));});
_tmp69->shared_str=ans;};}
# 289
return _tmp69->shared_str;};}
# 292
static int Cyc_Lex_str_index(struct _dyneither_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp71=idt;struct Cyc_Lex_DynTrieSymbols*_tmp72=Cyc_Lex_ids_trie;idt=_tmp72;Cyc_Lex_ids_trie=_tmp71;});{
struct Cyc_Lex_DynTrieSymbols _tmp73=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp74=_tmp73;struct Cyc_Core_DynamicRegion*_tmp7B;struct Cyc_Lex_Trie*_tmp7A;struct Cyc_Xarray_Xarray*_tmp79;_LL1: _tmp7B=_tmp74.dyn;_tmp7A=_tmp74.t;_tmp79=_tmp74.symbols;_LL2:;{
struct _tuple21 _tmp75=({struct _tuple21 _tmp144;_tmp144.f1=_tmp7A,_tmp144.f2=_tmp79,_tmp144.f3=buff,_tmp144.f4=offset,_tmp144.f5=len;_tmp144;});
int _tmp76=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple21*arg,int(*body)(struct _RegionHandle*h,struct _tuple21*arg)))Cyc_Core_open_region)(_tmp7B,& _tmp75,Cyc_Lex_str_index_body);
({struct Cyc_Lex_DynTrieSymbols _tmp178=({struct Cyc_Lex_DynTrieSymbols _tmp143;_tmp143.dyn=_tmp7B,_tmp143.t=_tmp7A,_tmp143.symbols=_tmp79;_tmp143;});*idt=_tmp178;});
({struct Cyc_Lex_DynTrieSymbols*_tmp77=idt;struct Cyc_Lex_DynTrieSymbols*_tmp78=Cyc_Lex_ids_trie;idt=_tmp78;Cyc_Lex_ids_trie=_tmp77;});
return _tmp76;};};}
# 303
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}struct _tuple22{struct Cyc_Lex_Trie*f1;struct _dyneither_ptr f2;};
# 309
static int Cyc_Lex_insert_typedef_body(struct _RegionHandle*h,struct _tuple22*arg){
# 311
struct _tuple22 _tmp7C=*arg;struct _tuple22 _tmp7D=_tmp7C;struct Cyc_Lex_Trie*_tmp80;struct _dyneither_ptr _tmp7F;_LL1: _tmp80=_tmp7D.f1;_tmp7F=_tmp7D.f2;_LL2:;{
struct Cyc_Lex_Trie*_tmp7E=Cyc_Lex_trie_lookup(h,_tmp80,_tmp7F,0,(int)(_get_dyneither_size(_tmp7F,sizeof(char))- 1));
((struct Cyc_Lex_Trie*)_check_null(_tmp7E))->shared_str=1;
return 0;};}
# 318
static void Cyc_Lex_insert_typedef(struct _dyneither_ptr*sptr){
struct _dyneither_ptr _tmp81=*sptr;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp82=tdefs;struct Cyc_Lex_DynTrie*_tmp83=Cyc_Lex_typedefs_trie;tdefs=_tmp83;Cyc_Lex_typedefs_trie=_tmp82;});{
struct Cyc_Lex_DynTrie _tmp84=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmp85=_tmp84;struct Cyc_Core_DynamicRegion*_tmp8A;struct Cyc_Lex_Trie*_tmp89;_LL1: _tmp8A=_tmp85.dyn;_tmp89=_tmp85.t;_LL2:;{
struct _tuple22 _tmp86=({struct _tuple22 _tmp146;_tmp146.f1=_tmp89,_tmp146.f2=_tmp81;_tmp146;});
((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple22*arg,int(*body)(struct _RegionHandle*h,struct _tuple22*arg)))Cyc_Core_open_region)(_tmp8A,& _tmp86,Cyc_Lex_insert_typedef_body);
({struct Cyc_Lex_DynTrie _tmp179=({struct Cyc_Lex_DynTrie _tmp145;_tmp145.dyn=_tmp8A,_tmp145.t=_tmp89;_tmp145;});*tdefs=_tmp179;});
({struct Cyc_Lex_DynTrie*_tmp87=tdefs;struct Cyc_Lex_DynTrie*_tmp88=Cyc_Lex_typedefs_trie;tdefs=_tmp88;Cyc_Lex_typedefs_trie=_tmp87;});
return;};};}struct _tuple23{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;int f3;};
# 330
static struct _dyneither_ptr*Cyc_Lex_get_symbol_body(struct _RegionHandle*dyn,struct _tuple23*env){
struct _tuple23 _tmp8B=*env;struct _tuple23 _tmp8C=_tmp8B;struct Cyc_Lex_Trie*_tmp8F;struct Cyc_Xarray_Xarray*_tmp8E;int _tmp8D;_LL1: _tmp8F=_tmp8C.f1;_tmp8E=_tmp8C.f2;_tmp8D=_tmp8C.f3;_LL2:;
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp8E,_tmp8D);}
# 335
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp90=idt;struct Cyc_Lex_DynTrieSymbols*_tmp91=Cyc_Lex_ids_trie;idt=_tmp91;Cyc_Lex_ids_trie=_tmp90;});{
struct Cyc_Lex_DynTrieSymbols _tmp92=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp93=_tmp92;struct Cyc_Core_DynamicRegion*_tmp9A;struct Cyc_Lex_Trie*_tmp99;struct Cyc_Xarray_Xarray*_tmp98;_LL1: _tmp9A=_tmp93.dyn;_tmp99=_tmp93.t;_tmp98=_tmp93.symbols;_LL2:;{
struct _tuple23 _tmp94=({struct _tuple23 _tmp148;_tmp148.f1=_tmp99,_tmp148.f2=_tmp98,_tmp148.f3=symbol_num;_tmp148;});
struct _dyneither_ptr*_tmp95=((struct _dyneither_ptr*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple23*arg,struct _dyneither_ptr*(*body)(struct _RegionHandle*h,struct _tuple23*arg)))Cyc_Core_open_region)(_tmp9A,& _tmp94,Cyc_Lex_get_symbol_body);
({struct Cyc_Lex_DynTrieSymbols _tmp17A=({struct Cyc_Lex_DynTrieSymbols _tmp147;_tmp147.dyn=_tmp9A,_tmp147.t=_tmp99,_tmp147.symbols=_tmp98;_tmp147;});*idt=_tmp17A;});
({struct Cyc_Lex_DynTrieSymbols*_tmp96=idt;struct Cyc_Lex_DynTrieSymbols*_tmp97=Cyc_Lex_ids_trie;idt=_tmp97;Cyc_Lex_ids_trie=_tmp96;});
return _tmp95;};};}
# 354 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if('0' <= c  && c <= '9')return c - '0';else{
if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{
if('A' <= c  && c <= 'F')return(10 + c)- 'A';else{
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp17B=({const char*_tmp9B="string to integer conversion";_tag_dyneither(_tmp9B,sizeof(char),29U);});_tmp9C->f1=_tmp17B;});_tmp9C;}));}}}}
# 362
static union Cyc_Absyn_Cnst Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
enum Cyc_Absyn_Sign sn=Cyc_Absyn_None;
start +=lbuf->lex_start_pos;{
struct _dyneither_ptr buff=_dyneither_ptr_plus(lbuf->lex_buffer,sizeof(char),start);
int end2=lbuf->lex_curr_pos - end;
int len=end2 - start;
enum Cyc_Absyn_Size_of size=Cyc_Absyn_Int_sz;
int declared_size=0;
union Cyc_Absyn_Cnst res;
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'l'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'L')){
# 373
len -=1;
declared_size=1;
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'l'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'L')){
len -=1;
size=Cyc_Absyn_LongLong_sz;}}
# 380
if(len >= 1  && (*((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'u'  || *((char*)_check_dyneither_subscript(buff,sizeof(char),len - 1))== 'U')){
len -=1;
sn=Cyc_Absyn_Unsigned;}
# 384
if(sn == Cyc_Absyn_Unsigned){
unsigned long long n=(unsigned long long)0;
{int i=0;for(0;i < len;++ i){
({unsigned long long _tmp17D=({unsigned long long _tmp17C=n * base;_tmp17C + (unsigned long long)Cyc_Lex_int_of_char(*((char*)_check_dyneither_subscript(buff,sizeof(char),i)));});n=_tmp17D;});}}
if(n > (unsigned long long)-1){
if(declared_size  && size == Cyc_Absyn_Int_sz)
({struct _dyneither_ptr _tmp17E=({const char*_tmp9D="integer constant too large";_tag_dyneither(_tmp9D,sizeof(char),27U);});Cyc_Lex_err(_tmp17E,lbuf);});
size=Cyc_Absyn_LongLong_sz;}
# 393
if(size == Cyc_Absyn_Int_sz)
({union Cyc_Absyn_Cnst _tmp17F=({union Cyc_Absyn_Cnst _tmp149;(_tmp149.Int_c).tag=5U,((_tmp149.Int_c).val).f1=sn,((_tmp149.Int_c).val).f2=(int)((unsigned int)n);_tmp149;});res=_tmp17F;});else{
# 396
({union Cyc_Absyn_Cnst _tmp180=({union Cyc_Absyn_Cnst _tmp14A;(_tmp14A.LongLong_c).tag=6U,((_tmp14A.LongLong_c).val).f1=sn,((_tmp14A.LongLong_c).val).f2=(long long)n;_tmp14A;});res=_tmp180;});}}else{
# 399
long long n=(long long)0;
{int i=0;for(0;i < len;++ i){
({long long _tmp182=({long long _tmp181=n * base;_tmp181 + (long long)Cyc_Lex_int_of_char(*((char*)_check_dyneither_subscript(buff,sizeof(char),i)));});n=_tmp182;});}}{
unsigned long long x=(unsigned long long)n >> 32;
if(x != -1  && x != 0){
if(declared_size  && size == Cyc_Absyn_Int_sz)
({struct _dyneither_ptr _tmp183=({const char*_tmp9E="integer constant too large";_tag_dyneither(_tmp9E,sizeof(char),27U);});Cyc_Lex_err(_tmp183,lbuf);});
size=Cyc_Absyn_LongLong_sz;}
# 408
if(size == Cyc_Absyn_Int_sz)
({union Cyc_Absyn_Cnst _tmp184=({union Cyc_Absyn_Cnst _tmp14B;(_tmp14B.Int_c).tag=5U,((_tmp14B.Int_c).val).f1=sn,((_tmp14B.Int_c).val).f2=(int)n;_tmp14B;});res=_tmp184;});else{
# 411
({union Cyc_Absyn_Cnst _tmp185=({union Cyc_Absyn_Cnst _tmp14C;(_tmp14C.LongLong_c).tag=6U,((_tmp14C.LongLong_c).val).f1=sn,((_tmp14C.LongLong_c).val).f2=n;_tmp14C;});res=_tmp185;});}};}
# 414
return res;};}
# 419
char Cyc_Lex_string_buffer_v[11U]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _dyneither_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11U)};
int Cyc_Lex_string_pos=0;
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
struct _dyneither_ptr str=({unsigned int _tmpA0=(unsigned int)newsz + 1U;char*_tmp9F=_cycalloc_atomic(_check_times(_tmpA0,sizeof(char)));({{unsigned int _tmp14D=(unsigned int)newsz;unsigned int i;for(i=0;i < _tmp14D;++ i){i < sz?_tmp9F[i]=*((char*)_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):(_tmp9F[i]='\000');}_tmp9F[_tmp14D]=0;}0;});_tag_dyneither(_tmp9F,sizeof(char),_tmpA0);});
Cyc_Lex_string_buffer=str;};}
# 430
({struct _dyneither_ptr _tmpA1=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);char _tmpA2=*((char*)_check_dyneither_subscript(_tmpA1,sizeof(char),0U));char _tmpA3=c;if(_get_dyneither_size(_tmpA1,sizeof(char))== 1U  && (_tmpA2 == '\000'  && _tmpA3 != '\000'))_throw_arraybounds();*((char*)_tmpA1.curr)=_tmpA3;});
++ Cyc_Lex_string_pos;}
# 433
void Cyc_Lex_store_string(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));}}
# 438
struct _dyneither_ptr Cyc_Lex_get_stored_string(){
struct _dyneither_ptr str=Cyc_substring((struct _dyneither_ptr)Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;};
# 456 "lex.cyl"
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 458
static struct Cyc_Lex_Ldecls*Cyc_Lex_mk_empty_ldecls(int ignore){
return({struct Cyc_Lex_Ldecls*_tmpA4=_cycalloc(sizeof(*_tmpA4));({struct Cyc_Set_Set*_tmp186=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpA4->typedefs=_tmp186;});_tmpA4;});}
# 462
static void Cyc_Lex_typedef_init(){
({struct Cyc_Core_Opt*_tmp188=({struct Cyc_Core_Opt*_tmpA5=_cycalloc(sizeof(*_tmpA5));({struct Cyc_Binding_NSCtxt*_tmp187=((struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Lex_mk_empty_ldecls);_tmpA5->v=_tmp187;});_tmpA5;});Cyc_Lex_lstate=_tmp188;});}
# 466
static void Cyc_Lex_recompute_typedefs(){
# 468
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpA6=tdefs;struct Cyc_Lex_DynTrie*_tmpA7=Cyc_Lex_typedefs_trie;tdefs=_tmpA7;Cyc_Lex_typedefs_trie=_tmpA6;});{
struct Cyc_Lex_DynTrie _tmpA8=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmpA9=_tmpA8;struct Cyc_Core_DynamicRegion*_tmpB6;struct Cyc_Lex_Trie*_tmpB5;_LL1: _tmpB6=_tmpA9.dyn;_tmpB5=_tmpA9.t;_LL2:;
Cyc_Core_free_ukey(_tmpB6);{
struct Cyc_Core_NewDynamicRegion _tmpAA=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmpAB=_tmpAA;struct Cyc_Core_DynamicRegion*_tmpB4;_LL4: _tmpB4=_tmpAB.key;_LL5:;{
struct Cyc_Lex_Trie*t2=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpB4,0,Cyc_Lex_empty_trie);
({struct Cyc_Lex_DynTrie _tmp189=({struct Cyc_Lex_DynTrie _tmp14E;_tmp14E.dyn=_tmpB4,_tmp14E.t=t2;_tmp14E;});*tdefs=_tmp189;});
({struct Cyc_Lex_DynTrie*_tmpAC=Cyc_Lex_typedefs_trie;struct Cyc_Lex_DynTrie*_tmpAD=tdefs;Cyc_Lex_typedefs_trie=_tmpAD;tdefs=_tmpAC;});{
# 478
struct Cyc_List_List*_tmpAE=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->availables;for(0;_tmpAE != 0;_tmpAE=_tmpAE->tl){
void*_tmpAF=(void*)_tmpAE->hd;void*_tmpB0=_tmpAF;struct Cyc_List_List*_tmpB3;struct Cyc_List_List*_tmpB2;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpB0)->tag == 1U){_LL7: _tmpB2=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpB0)->f1;_LL8:
 _tmpB3=_tmpB2;goto _LLA;}else{_LL9: _tmpB3=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpB0)->f1;_LLA: {
# 482
struct Cyc_Lex_Ldecls*_tmpB1=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,_tmpB3);
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpB1->typedefs);
goto _LL6;}}_LL6:;}};};};};}
# 490
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
struct Cyc_List_List*ans=((struct Cyc_List_List*(*)(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*))Cyc_Binding_resolve_rel_ns)(0U,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,ns);
struct Cyc_Lex_Ldecls*_tmpB7=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,ans);
return((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmpB7->typedefs,v);}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Lex_Trie*f2;struct _dyneither_ptr f3;};
# 496
static int Cyc_Lex_is_typedef_body(struct _RegionHandle*d,struct _tuple24*env){
# 499
struct _tuple24 _tmpB8=*env;struct _tuple24 _tmpB9=_tmpB8;struct Cyc_List_List*_tmpC3;struct Cyc_Lex_Trie*_tmpC2;struct _dyneither_ptr _tmpC1;_LL1: _tmpC3=_tmpB9.f1;_tmpC2=_tmpB9.f2;_tmpC1=_tmpB9.f3;_LL2:;{
int len=(int)(_get_dyneither_size(_tmpC1,sizeof(char))- 1);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmpBA=((struct Cyc_Lex_Trie*)_check_null(_tmpC2))->children;union Cyc_Lex_TrieChildren _tmpBB=_tmpBA;struct Cyc_Lex_Trie**_tmpC0;struct Cyc_Lex_Trie*_tmpBF;int _tmpBE;struct Cyc_Lex_Trie*_tmpBD;switch((_tmpBB.One).tag){case 1U: _LL4: _LL5:
 return 0;case 2U: _LL6: _tmpBE=((_tmpBB.One).val).f1;_tmpBD=((_tmpBB.One).val).f2;if(_tmpBE != *((const char*)_check_dyneither_subscript(_tmpC1,sizeof(char),i))){_LL7:
 return 0;}else{_LL8: _tmpBF=((_tmpBB.One).val).f2;_LL9:
 _tmpC2=_tmpBF;goto _LL3;}default: _LLA: _tmpC0=(_tmpBB.Many).val;_LLB: {
# 507
struct Cyc_Lex_Trie*_tmpBC=_tmpC0[_check_known_subscript_notnull(64U,Cyc_Lex_trie_char((int)((const char*)_tmpC1.curr)[i]))];
if(_tmpBC == 0)
return 0;
_tmpC2=_tmpBC;
goto _LL3;}}_LL3:;}}
# 513
return((struct Cyc_Lex_Trie*)_check_null(_tmpC2))->shared_str;};}
# 515
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _dyneither_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 520
struct _dyneither_ptr _tmpC4=*v;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpC5=tdefs;struct Cyc_Lex_DynTrie*_tmpC6=Cyc_Lex_typedefs_trie;tdefs=_tmpC6;Cyc_Lex_typedefs_trie=_tmpC5;});{
struct Cyc_Lex_DynTrie _tmpC7=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmpC8=_tmpC7;struct Cyc_Core_DynamicRegion*_tmpCE;struct Cyc_Lex_Trie*_tmpCD;_LL1: _tmpCE=_tmpC8.dyn;_tmpCD=_tmpC8.t;_LL2:;{
struct _tuple24 _tmpC9=({struct _tuple24 _tmp150;_tmp150.f1=ns,_tmp150.f2=_tmpCD,_tmp150.f3=_tmpC4;_tmp150;});
int _tmpCA=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple24*arg,int(*body)(struct _RegionHandle*h,struct _tuple24*arg)))Cyc_Core_open_region)(_tmpCE,& _tmpC9,Cyc_Lex_is_typedef_body);
({struct Cyc_Lex_DynTrie _tmp18A=({struct Cyc_Lex_DynTrie _tmp14F;_tmp14F.dyn=_tmpCE,_tmp14F.t=_tmpCD;_tmp14F;});*tdefs=_tmp18A;});
({struct Cyc_Lex_DynTrie*_tmpCB=tdefs;struct Cyc_Lex_DynTrie*_tmpCC=Cyc_Lex_typedefs_trie;tdefs=_tmpCC;Cyc_Lex_typedefs_trie=_tmpCB;});
return _tmpCA;};};};}
# 531
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
((void(*)(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_enter_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,s,1,Cyc_Lex_mk_empty_ldecls);{
struct Cyc_Lex_Ldecls*_tmpCF=({struct Cyc_Dict_Dict _tmp18B=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data;((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp18B,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);});
# 536
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpCF->typedefs);};}
# 538
void Cyc_Lex_leave_namespace(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
Cyc_Lex_recompute_typedefs();}
# 542
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*_tmpD0=((struct Cyc_List_List*(*)(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(0U,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,q);
struct Cyc_Lex_Ldecls*_tmpD1=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,_tmpD0);
# 547
((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpD1->typedefs);}
# 549
void Cyc_Lex_leave_using(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
# 552
Cyc_Lex_recompute_typedefs();}
# 555
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 557
struct Cyc_Lex_Ldecls*_tmpD2=({struct Cyc_Dict_Dict _tmp18C=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data;((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp18C,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);});
({struct Cyc_Set_Set*_tmp18D=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(_tmpD2->typedefs,(*q).f2);_tmpD2->typedefs=_tmp18D;});
# 562
Cyc_Lex_insert_typedef((*q).f2);}
# 565
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 569
if(symbol_num <= Cyc_Lex_num_kws){
# 571
if(!Cyc_Lex_in_extern_c  || (*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).is_c_keyword){
short res=(short)(*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).token_index;
return res;}}{
# 577
struct _dyneither_ptr*_tmpD3=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*_tmpD3;
# 581
if(Cyc_Lex_is_typedef(0,_tmpD3))
return 370;
# 585
return 362;};}
# 588
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
if(Cyc_Lex_in_extern_c)
({struct _dyneither_ptr _tmp18E=({const char*_tmpD4="qualified identifiers not allowed in C code";_tag_dyneither(_tmpD4,sizeof(char),44U);});Cyc_Lex_err(_tmp18E,lbuf);});{
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _dyneither_ptr s=lbuf->lex_buffer;
# 595
struct _dyneither_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 598
while(i < end){
int start=i;
for(0;i < end  && *((char*)_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){
;}
if(start == i)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp18F=({const char*_tmpD5="bad namespace";_tag_dyneither(_tmpD5,sizeof(char),14U);});_tmpD6->f1=_tmp18F;});_tmpD6;}));{
int vlen=i - start;
if(v != 0)
({struct Cyc_List_List*_tmp190=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=v,_tmpD7->tl=rev_vs;_tmpD7;});rev_vs=_tmp190;});
({struct _dyneither_ptr*_tmp191=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));v=_tmp191;});
i +=2;};}
# 610
if(v == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp192=({const char*_tmpD8="bad namespace";_tag_dyneither(_tmpD8,sizeof(char),14U);});_tmpD9->f1=_tmp192;});_tmpD9;}));{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 615
if(vs != 0  && ({struct _dyneither_ptr _tmp193=(struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd);Cyc_strcmp(_tmp193,({const char*_tmpDA="Cyc";_tag_dyneither(_tmpDA,sizeof(char),4U);}));})== 0){
vs=vs->tl;
({struct _tuple0*_tmp195=({struct _tuple0*_tmpDB=_cycalloc(sizeof(*_tmpDB));({union Cyc_Absyn_Nmspace _tmp194=Cyc_Absyn_Abs_n(vs,0);_tmpDB->f1=_tmp194;}),_tmpDB->f2=v;_tmpDB;});Cyc_Lex_token_qvar=_tmp195;});}else{
# 619
if(vs != 0  && ({struct _dyneither_ptr _tmp196=(struct _dyneither_ptr)*((struct _dyneither_ptr*)vs->hd);Cyc_strcmp(_tmp196,({const char*_tmpDC="C";_tag_dyneither(_tmpDC,sizeof(char),2U);}));})== 0){
vs=vs->tl;
({struct _tuple0*_tmp198=({struct _tuple0*_tmpDD=_cycalloc(sizeof(*_tmpDD));({union Cyc_Absyn_Nmspace _tmp197=Cyc_Absyn_Abs_n(vs,1);_tmpDD->f1=_tmp197;}),_tmpDD->f2=v;_tmpDD;});Cyc_Lex_token_qvar=_tmp198;});}else{
# 623
({struct _tuple0*_tmp19A=({struct _tuple0*_tmpDE=_cycalloc(sizeof(*_tmpDE));({union Cyc_Absyn_Nmspace _tmp199=Cyc_Absyn_Rel_n(vs);_tmpDE->f1=_tmp199;}),_tmpDE->f2=v;_tmpDE;});Cyc_Lex_token_qvar=_tmp19A;});}}
if(Cyc_Lex_is_typedef(vs,v))
return 372;
return 371;};};}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned int starting_line;struct _dyneither_ptr filename;struct _dyneither_ptr linenumpos;unsigned int linenumpos_offset;};
# 642
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 644
static int Cyc_Lex_linenumber=1;
# 647
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _dyneither_ptr filename,unsigned int starting_line,struct Cyc_Lex_PosInfo*next){
# 651
struct _dyneither_ptr linenumpos=({unsigned int _tmpE0=10U;unsigned int*_tmpE1=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmpE0);_tag_dyneither(_tmpE1,sizeof(unsigned int),_tmpE0);});
*((unsigned int*)_check_dyneither_subscript(linenumpos,sizeof(unsigned int),0))=(unsigned int)Cyc_yylloc.first_line;
return({struct Cyc_Lex_PosInfo*_tmpDF=_region_malloc(r,sizeof(*_tmpDF));_tmpDF->next=next,_tmpDF->starting_line=starting_line,_tmpDF->filename=(struct _dyneither_ptr)filename,_tmpDF->linenumpos=linenumpos,_tmpDF->linenumpos_offset=1U;_tmpDF;});}
# 660
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0)(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp19B=({const char*_tmpE2="empty position info!";_tag_dyneither(_tmpE2,sizeof(char),21U);});_tmpE3->f1=_tmp19B;});_tmpE3;}));{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _dyneither_ptr linenums=_tag_dyneither(0,0,0);
({struct _dyneither_ptr _tmpE4=p->linenumpos;struct _dyneither_ptr _tmpE5=linenums;p->linenumpos=_tmpE5;linenums=_tmpE4;});{
unsigned int offset=p->linenumpos_offset;
unsigned int n=_get_dyneither_size(linenums,sizeof(unsigned int));
# 668
if(offset >= n){
# 670
struct _dyneither_ptr newlinenums=({unsigned int _tmpE8=n * 2;unsigned int*_tmpE9=_region_calloc(Cyc_Core_unique_region,sizeof(unsigned int),_tmpE8);_tag_dyneither(_tmpE9,sizeof(unsigned int),_tmpE8);});
{unsigned int i=0U;for(0;i < n;++ i){
*((unsigned int*)_check_dyneither_subscript(newlinenums,sizeof(unsigned int),(int)i))=((unsigned int*)linenums.curr)[(int)i];}}
({struct _dyneither_ptr _tmpE6=linenums;struct _dyneither_ptr _tmpE7=newlinenums;linenums=_tmpE7;newlinenums=_tmpE6;});
# 675
((void(*)(unsigned int*ptr))Cyc_Core_ufree)((unsigned int*)_untag_dyneither_ptr(newlinenums,sizeof(unsigned int),1U));}
# 678
*((unsigned int*)_check_dyneither_subscript(linenums,sizeof(unsigned int),(int)offset))=(unsigned int)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + 1;
++ Cyc_Lex_linenumber;
({struct _dyneither_ptr _tmpEA=p->linenumpos;struct _dyneither_ptr _tmpEB=linenums;p->linenumpos=_tmpEB;linenums=_tmpEA;});};};}
# 685
static void Cyc_Lex_process_directive(struct _dyneither_ptr line){
int i;
char buf[100U];
struct _dyneither_ptr filename=({const char*_tmpF9="";_tag_dyneither(_tmpF9,sizeof(char),1U);});
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmpEE=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp152;_tmp152.tag=2U,_tmp152.f1=& i;_tmp152;});struct Cyc_CharPtr_sa_ScanfArg_struct _tmpEF=({struct Cyc_CharPtr_sa_ScanfArg_struct _tmp151;_tmp151.tag=7U,({struct _dyneither_ptr _tmp19C=_tag_dyneither(buf,sizeof(char),100U);_tmp151.f1=_tmp19C;});_tmp151;});void*_tmpEC[2U];_tmpEC[0]=& _tmpEE,_tmpEC[1]=& _tmpEF;({struct _dyneither_ptr _tmp19E=(struct _dyneither_ptr)line;struct _dyneither_ptr _tmp19D=({const char*_tmpED="# %d \"%s";_tag_dyneither(_tmpED,sizeof(char),9U);});Cyc_sscanf(_tmp19E,_tmp19D,_tag_dyneither(_tmpEC,sizeof(void*),2U));});})== 2){
if(Cyc_Lex_compile_for_boot_r){
# 693
int i=(int)(Cyc_strlen(_tag_dyneither(buf,sizeof(char),100U))- 1);
int last_slash=-1;
while(i >= 0){
if(buf[_check_known_subscript_notnull(100U,i)]== '/'){last_slash=i;break;}
-- i;}
# 699
({struct _dyneither_ptr _tmp1A2=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,({struct _dyneither_ptr _tmp1A0=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(({struct _dyneither_ptr _tmp19F=_tag_dyneither(buf,sizeof(char),100U);_dyneither_ptr_plus(_tmp19F,sizeof(char),last_slash);}),sizeof(char),1));_tmp153.f1=_tmp1A0;});_tmp153;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF2;({struct _dyneither_ptr _tmp1A1=({const char*_tmpF1="\"%s";_tag_dyneither(_tmpF1,sizeof(char),4U);});Cyc_aprintf(_tmp1A1,_tag_dyneither(_tmpF0,sizeof(void*),1U));});});filename=_tmp1A2;});}else{
# 701
({struct _dyneither_ptr _tmp1A5=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,({struct _dyneither_ptr _tmp1A3=(struct _dyneither_ptr)_tag_dyneither(buf,sizeof(char),100U);_tmp154.f1=_tmp1A3;});_tmp154;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({struct _dyneither_ptr _tmp1A4=({const char*_tmpF4="\"%s";_tag_dyneither(_tmpF4,sizeof(char),4U);});Cyc_aprintf(_tmp1A4,_tag_dyneither(_tmpF3,sizeof(void*),1U));});});filename=_tmp1A5;});}
if((Cyc_Lex_linenumber == i  && Cyc_Lex_pos_info != 0) && Cyc_strcmp((struct _dyneither_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _dyneither_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmpF8=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp155;_tmp155.tag=2U,_tmp155.f1=& i;_tmp155;});void*_tmpF6[1U];_tmpF6[0]=& _tmpF8;({struct _dyneither_ptr _tmp1A7=(struct _dyneither_ptr)line;struct _dyneither_ptr _tmp1A6=({const char*_tmpF7="# %d";_tag_dyneither(_tmpF7,sizeof(char),5U);});Cyc_sscanf(_tmp1A7,_tmp1A6,_tag_dyneither(_tmpF6,sizeof(void*),1U));});})== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 709
++ Cyc_Lex_linenumber;
return;}}
# 712
({struct Cyc_Lex_PosInfo*_tmp1A8=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned int)Cyc_Lex_linenumber,Cyc_Lex_pos_info);Cyc_Lex_pos_info=_tmp1A8;});}struct _tuple25{struct _dyneither_ptr f1;unsigned int f2;};
# 718
struct _tuple25 Cyc_Lex_xlate_pos(unsigned int char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p != 0;p=p->next){
unsigned int first_char_offset=*((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),0));
if(char_offset < first_char_offset  && p->next != 0)continue;{
# 724
unsigned int base=0U;
unsigned int size=p->linenumpos_offset;
while(size > 1){
int half=(int)(size / 2);
int mid=(int)(base + half);
if(char_offset > *((unsigned int*)_check_dyneither_subscript(p->linenumpos,sizeof(unsigned int),mid))){
base=base + half;
size=size - half;}else{
# 733
size=(unsigned int)half;}}
# 736
return({struct _tuple25 _tmp156;_tmp156.f1=p->filename,_tmp156.f2=p->starting_line + base;_tmp156;});};}}
# 738
return({struct _tuple25 _tmp157;({struct _dyneither_ptr _tmp1A9=_tag_dyneither(0,0,0);_tmp157.f1=_tmp1A9;}),_tmp157.f2=0U;_tmp157;});}
# 743
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);
# 755
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 757
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);
({int _tmp1AB=({int _tmp1AA=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf);yyllocptr->first_line=_tmp1AA;});Cyc_yylloc.first_line=_tmp1AB;});
({int _tmp1AD=({int _tmp1AC=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf);yyllocptr->last_line=_tmp1AC;});Cyc_yylloc.last_line=_tmp1AD;});
{int _tmpFA=ans;switch(_tmpFA){case 362U: _LL1: _LL2:
# 762
({union Cyc_YYSTYPE _tmp1AE=({union Cyc_YYSTYPE _tmp158;(_tmp158.String_tok).tag=3U,(_tmp158.String_tok).val=Cyc_Lex_token_string;_tmp158;});*yylval=_tmp1AE;});goto _LL0;case 370U: _LL3: _LL4:
# 764
({union Cyc_YYSTYPE _tmp1AF=({union Cyc_YYSTYPE _tmp159;(_tmp159.String_tok).tag=3U,(_tmp159.String_tok).val=Cyc_Lex_token_string;_tmp159;});*yylval=_tmp1AF;});goto _LL0;case 371U: _LL5: _LL6:
# 766
({union Cyc_YYSTYPE _tmp1B0=({union Cyc_YYSTYPE _tmp15A;(_tmp15A.QualId_tok).tag=5U,(_tmp15A.QualId_tok).val=Cyc_Lex_token_qvar;_tmp15A;});*yylval=_tmp1B0;});goto _LL0;case 372U: _LL7: _LL8:
# 768
({union Cyc_YYSTYPE _tmp1B1=({union Cyc_YYSTYPE _tmp15B;(_tmp15B.QualId_tok).tag=5U,(_tmp15B.QualId_tok).val=Cyc_Lex_token_qvar;_tmp15B;});*yylval=_tmp1B1;});goto _LL0;case 369U: _LL9: _LLA:
# 770
({union Cyc_YYSTYPE _tmp1B2=({union Cyc_YYSTYPE _tmp15C;(_tmp15C.String_tok).tag=3U,(_tmp15C.String_tok).val=Cyc_Lex_token_string;_tmp15C;});*yylval=_tmp1B2;});goto _LL0;case 363U: _LLB: _LLC:
# 772
({union Cyc_YYSTYPE _tmp1B3=({union Cyc_YYSTYPE _tmp15D;(_tmp15D.Int_tok).tag=1U,(_tmp15D.Int_tok).val=Cyc_Lex_token_int;_tmp15D;});*yylval=_tmp1B3;});goto _LL0;case 366U: _LLD: _LLE:
# 774
({union Cyc_YYSTYPE _tmp1B4=({union Cyc_YYSTYPE _tmp15E;(_tmp15E.Char_tok).tag=2U,(_tmp15E.Char_tok).val=Cyc_Lex_token_char;_tmp15E;});*yylval=_tmp1B4;});goto _LL0;case 367U: _LLF: _LL10:
# 776
({union Cyc_YYSTYPE _tmp1B5=({union Cyc_YYSTYPE _tmp15F;(_tmp15F.String_tok).tag=3U,(_tmp15F.String_tok).val=Cyc_Lex_token_string;_tmp15F;});*yylval=_tmp1B5;});goto _LL0;case 368U: _LL11: _LL12:
# 778
({union Cyc_YYSTYPE _tmp1B6=({union Cyc_YYSTYPE _tmp160;(_tmp160.String_tok).tag=3U,(_tmp160.String_tok).val=Cyc_Lex_token_string;_tmp160;});*yylval=_tmp1B6;});goto _LL0;case 364U: _LL13: _LL14:
# 780
({union Cyc_YYSTYPE _tmp1B7=({union Cyc_YYSTYPE _tmp161;(_tmp161.String_tok).tag=3U,(_tmp161.String_tok).val=Cyc_Lex_token_string;_tmp161;});*yylval=_tmp1B7;});goto _LL0;case 365U: _LL15: _LL16:
# 782
({union Cyc_YYSTYPE _tmp1B8=({union Cyc_YYSTYPE _tmp162;(_tmp162.String_tok).tag=3U,(_tmp162.String_tok).val=Cyc_Lex_token_string;_tmp162;});*yylval=_tmp1B8;});goto _LL0;case 374U: _LL17: _LL18:
# 784
({union Cyc_YYSTYPE _tmp1B9=({union Cyc_YYSTYPE _tmp163;(_tmp163.Asm_tok).tag=6U,(_tmp163.Asm_tok).val=Cyc_Lex_token_asm;_tmp163;});*yylval=_tmp1B9;});goto _LL0;default: _LL19: _LL1A:
 goto _LL0;}_LL0:;}
# 787
return ans;}
# 794
const int Cyc_Lex_lex_base[322U]={0,113,119,120,125,126,127,132,136,- 8,- 7,4,- 1,5,- 4,- 5,2,- 6,- 2,6,- 3,7,- 7,175,- 1,30,16,23,50,32,45,47,55,181,40,48,53,65,47,60,58,66,73,78,187,169,10,12,21,168,- 4,194,- 1,186,- 5,259,287,345,267,198,- 4,- 3,- 2,11,2,199,- 17,213,- 1,422,- 18,12,- 12,- 11,275,- 13,- 10,- 9,495,518,310,- 14,240,- 17,13,- 1,- 15,- 16,14,- 1,573,368,646,721,438,- 16,- 69,233,- 49,15,30,- 51,16,31,142,171,32,163,169,449,206,798,841,168,197,114,190,407,911,1000,128,1075,1133,1208,156,- 68,- 33,- 39,1283,137,1358,1433,1491,1566,1641,164,1716,1791,1866,1941,2016,2091,2166,2241,- 34,- 52,- 53,144,146,158,145,169,154,154,160,160,176,165,172,2316,179,180,177,2391,186,191,194,2476,264,190,180,193,185,200,188,2551,- 15,220,223,2636,229,238,226,247,241,234,251,2711,271,272,263,271,2796,263,273,264,268,264,289,277,294,2871,287,297,298,2956,291,3031,293,293,325,318,314,325,333,325,333,3116,319,323,337,324,3191,- 25,379,- 37,- 22,380,- 24,- 42,- 36,- 45,390,- 46,3266,3295,589,- 21,571,373,375,- 20,367,808,3305,3335,3369,3409,573,386,3479,3517,795,402,403,395,431,437,796,449,450,- 19,442,797,500,501,- 18,493,- 50,450,- 31,- 49,17,407,3449,- 44,- 27,- 29,- 43,- 26,- 28,- 30,1,3557,2,418,550,419,420,425,426,427,428,429,443,474,3630,3714,- 66,- 60,- 59,- 58,- 57,- 56,- 55,- 54,- 61,- 64,- 65,880,476,- 62,- 63,- 67,- 38,- 35,- 32,524,- 47,18,- 23,641};
const int Cyc_Lex_lex_backtrk[322U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,7,- 1,- 1,7,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,5,3,4,2,- 1,2,- 1,5,- 1,2,- 1,2,2,2,- 1,- 1,- 1,1,3,15,- 1,15,- 1,18,- 1,1,- 1,- 1,13,- 1,- 1,- 1,- 1,14,13,- 1,15,- 1,1,- 1,- 1,- 1,14,- 1,17,12,- 1,13,12,- 1,- 1,47,- 1,48,68,- 1,68,68,68,68,68,68,68,68,68,17,19,68,68,68,68,68,1,1,68,68,68,1,68,- 1,- 1,- 1,1,- 1,1,0,- 1,2,2,- 1,3,1,1,1,1,1,1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,- 1,39,- 1,- 1,- 1,- 1,- 1,- 1,20,19,- 1,- 1,19,19,19,- 1,19,- 1,20,20,17,18,17,17,- 1,16,16,16,16,16,17,17,18,18,18,- 1,18,17,17,17,- 1,17,- 1,- 1,- 1,- 1,48,- 1,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,46,- 1,47};
const int Cyc_Lex_lex_default[322U]={96,86,60,65,60,49,45,- 1,9,0,0,- 1,0,19,0,0,- 1,0,0,19,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,59,0,59,0,- 1,0,- 1,- 1,- 1,- 1,59,0,0,0,- 1,- 1,82,0,82,0,- 1,0,- 1,0,0,- 1,0,0,0,- 1,- 1,- 1,0,82,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,- 1,0,- 1,- 1,0,317,- 1,- 1,282,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,22,- 1,- 1,- 1,72,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,176,0,- 1,- 1,76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,9,- 1,- 1,- 1,17,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,77,- 1,- 1,- 1,- 1,95,0,- 1,0,0,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,0,- 1,0,269,0,0,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,317,0,- 1,0,- 1};
const int Cyc_Lex_lex_trans[3971U]={0,0,0,0,0,0,0,0,0,97,98,97,97,99,22,20,20,20,21,21,17,18,18,18,95,271,318,271,318,319,0,0,97,100,101,102,24,103,104,105,313,312,106,107,18,108,109,110,111,112,112,112,112,112,112,112,112,112,113,18,114,115,116,24,117,118,118,118,118,118,118,118,118,118,118,118,119,118,118,118,118,118,118,118,118,118,118,118,118,118,118,320,316,281,120,121,122,123,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,87,124,34,88,27,61,62,66,28,84,67,61,62,50,17,63,51,46,23,23,23,23,23,10,29,30,11,31,61,89,85,68,32,33,61,35,36,52,37,38,39,23,40,41,42,43,47,12,13,24,44,48,226,14,15,- 1,- 1,314,- 1,- 1,16,23,23,23,23,23,144,33,33,33,33,33,132,44,44,44,44,44,64,- 1,315,60,90,279,23,- 1,- 1,- 1,- 1,69,33,276,24,- 1,126,53,44,18,18,132,83,280,18,231,20,- 1,232,277,278,- 1,- 1,55,55,55,55,55,55,55,55,321,206,321,321,168,- 1,268,18,- 1,223,224,269,208,153,125,227,228,229,- 1,- 1,- 1,283,209,321,160,270,154,180,25,155,- 1,161,- 1,181,156,157,18,158,127,159,164,18,18,162,- 1,163,18,165,- 1,- 1,166,26,167,177,18,171,172,173,18,174,18,175,18,- 1,56,58,58,58,58,58,58,58,58,20,20,20,20,20,20,20,20,80,80,80,80,80,80,80,80,178,- 1,179,188,57,57,57,57,57,57,57,57,57,57,193,182,183,184,185,186,194,57,57,57,57,57,57,81,81,81,81,81,81,81,81,187,169,189,83,190,191,192,202,195,- 1,70,170,196,197,198,- 1,54,20,57,57,57,57,57,57,199,200,17,57,57,57,57,57,57,57,57,57,57,201,203,204,205,207,218,210,57,57,57,57,57,57,94,94,94,94,94,94,94,94,- 1,- 1,211,- 1,212,213,214,215,18,216,217,71,219,220,221,222,225,230,57,57,57,57,57,57,233,240,- 1,241,241,275,- 1,- 1,72,311,307,306,271,73,256,272,305,304,303,302,301,- 1,74,74,74,74,74,74,74,74,254,83,83,240,300,241,241,75,75,75,75,75,75,75,75,75,256,273,- 1,274,274,274,274,274,274,274,274,274,274,70,147,148,254,83,83,299,76,308,0,149,0,20,60,150,70,151,54,260,261,261,0,152,0,0,17,0,318,0,22,319,9,70,77,0,78,79,79,79,79,79,79,79,79,79,79,0,70,0,0,260,261,261,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,265,266,266,0,0,0,0,79,79,79,79,79,79,308,0,0,79,79,79,79,79,79,309,309,309,309,309,309,309,309,0,73,265,266,266,0,76,0,0,79,79,79,79,79,79,91,91,91,91,91,91,91,91,0,0,0,243,0,243,0,72,244,244,244,244,244,244,244,244,244,244,242,0,257,321,0,321,321,0,0,241,0,70,0,0,0,0,0,0,77,0,0,0,0,18,20,0,321,0,60,0,0,0,242,0,257,0,54,0,0,0,17,241,22,70,9,0,92,93,93,93,93,93,93,93,93,93,93,0,0,- 1,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,- 1,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,234,0,246,246,246,246,246,246,246,246,247,247,244,244,244,244,244,244,244,244,244,244,0,236,237,0,0,255,262,267,248,0,0,0,0,0,83,261,266,249,0,0,250,234,0,235,235,235,235,235,235,235,235,235,235,236,237,0,0,255,262,267,248,0,0,0,236,237,83,261,266,249,0,238,250,308,0,0,0,0,0,0,239,0,310,310,310,310,310,310,310,310,0,0,0,0,0,0,236,237,0,0,0,0,0,238,0,0,0,0,0,0,0,0,239,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,145,0,0,0,0,146,0,0,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,138,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,136,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,130,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,131,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,134,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,135,0,0,0,0,0,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,133,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,0,0,0,0,134,0,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,136,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,139,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,140,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,141,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,142,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,143,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,245,245,245,245,245,245,245,245,245,245,0,0,0,0,0,0,0,0,0,0,0,236,237,0,0,0,0,234,237,235,235,235,235,235,235,235,235,235,235,244,244,244,244,244,244,244,244,244,244,0,236,237,0,236,237,0,0,238,- 1,0,237,237,0,0,0,0,239,237,0,245,245,245,245,245,245,245,245,245,245,0,0,0,236,237,0,0,0,0,0,238,236,237,0,237,0,0,0,237,239,237,0,234,0,246,246,246,246,246,246,246,246,247,247,0,0,0,0,0,0,0,0,0,236,237,236,237,0,0,0,237,0,263,0,- 1,0,0,0,0,0,0,264,234,0,247,247,247,247,247,247,247,247,247,247,0,0,0,236,237,0,0,0,0,0,263,236,237,0,0,0,0,0,258,264,0,0,0,0,0,0,0,259,0,0,274,274,274,274,274,274,274,274,274,274,0,0,0,236,237,0,0,0,0,0,258,236,237,0,0,0,0,0,237,259,251,251,251,251,251,251,251,251,251,251,0,0,0,0,0,0,0,251,251,251,251,251,251,236,237,0,0,0,0,0,237,0,0,0,0,0,0,0,251,251,251,251,251,251,251,251,251,251,0,251,251,251,251,251,251,251,251,251,251,251,251,0,0,0,284,0,252,0,0,285,0,0,0,0,0,253,0,0,286,286,286,286,286,286,286,286,0,251,251,251,251,251,251,287,0,0,0,0,252,0,0,0,0,0,0,0,0,253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,0,0,0,0,289,290,0,0,0,291,0,0,0,0,0,0,0,292,0,0,0,293,0,294,0,295,0,296,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,298,0,0,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[3971U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,11,13,19,21,13,19,46,63,71,84,88,99,102,272,319,102,- 1,- 1,0,0,0,0,64,0,0,0,282,284,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,47,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,103,106,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,25,1,26,2,2,3,27,2,3,4,4,5,6,4,5,6,7,7,7,7,7,8,28,29,8,30,2,1,2,3,31,32,4,34,35,5,36,37,38,7,39,40,41,42,6,8,8,7,43,6,115,8,8,49,45,104,105,45,8,23,23,23,23,23,120,33,33,33,33,33,129,44,44,44,44,44,4,49,104,51,1,107,23,59,65,105,45,3,33,108,23,45,124,5,44,53,33,135,67,107,53,113,44,51,113,108,108,59,65,53,53,53,53,53,53,53,53,97,148,97,97,150,67,110,53,82,116,116,110,147,152,0,114,114,114,49,13,19,105,147,97,151,110,153,149,23,154,102,151,82,149,155,156,53,157,124,158,160,53,53,161,51,162,53,164,59,65,165,23,166,169,53,170,171,172,53,173,53,174,53,67,53,55,55,55,55,55,55,55,55,58,58,58,58,58,58,58,58,74,74,74,74,74,74,74,74,177,82,178,182,56,56,56,56,56,56,56,56,56,56,180,181,181,183,184,185,180,56,56,56,56,56,56,80,80,80,80,80,80,80,80,186,168,188,1,189,190,191,193,194,2,3,168,195,196,197,4,5,6,56,56,56,56,56,56,198,199,8,57,57,57,57,57,57,57,57,57,57,200,202,203,204,206,208,209,57,57,57,57,57,57,91,91,91,91,91,91,91,91,49,45,210,105,211,212,213,214,69,215,216,69,218,219,220,221,224,227,57,57,57,57,57,57,232,239,51,240,242,273,59,65,69,285,287,288,269,69,249,269,289,290,291,292,293,67,69,69,69,69,69,69,69,69,253,254,255,239,294,240,242,69,94,94,94,94,94,94,94,94,249,109,82,109,109,109,109,109,109,109,109,109,109,256,117,117,253,254,255,295,69,310,- 1,117,- 1,69,69,117,257,117,69,259,260,262,- 1,117,- 1,- 1,69,- 1,317,- 1,69,317,69,256,69,- 1,69,78,78,78,78,78,78,78,78,78,78,- 1,257,- 1,- 1,259,260,262,78,78,78,78,78,78,79,79,79,79,79,79,79,79,79,79,264,265,267,- 1,- 1,- 1,- 1,79,79,79,79,79,79,286,- 1,- 1,78,78,78,78,78,78,286,286,286,286,286,286,286,286,- 1,90,264,265,267,- 1,90,- 1,- 1,79,79,79,79,79,79,90,90,90,90,90,90,90,90,- 1,- 1,- 1,236,- 1,236,- 1,90,236,236,236,236,236,236,236,236,236,236,238,- 1,248,321,- 1,321,321,- 1,- 1,238,- 1,248,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,90,90,- 1,321,- 1,90,- 1,- 1,- 1,238,- 1,248,- 1,90,- 1,- 1,- 1,90,238,90,248,90,- 1,90,92,92,92,92,92,92,92,92,92,92,- 1,- 1,269,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,93,93,93,93,93,93,93,93,- 1,317,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,111,- 1,111,111,111,111,111,111,111,111,111,111,243,243,243,243,243,243,243,243,243,243,- 1,111,111,- 1,- 1,252,258,263,111,- 1,- 1,- 1,- 1,- 1,252,258,263,111,- 1,- 1,111,112,- 1,112,112,112,112,112,112,112,112,112,112,111,111,- 1,- 1,252,258,263,111,- 1,- 1,- 1,112,112,252,258,263,111,- 1,112,111,309,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,309,309,309,309,309,309,309,309,- 1,- 1,- 1,- 1,- 1,- 1,112,112,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,118,118,118,118,118,118,118,118,118,118,118,- 1,- 1,- 1,- 1,- 1,- 1,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,- 1,- 1,- 1,- 1,118,- 1,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,119,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,119,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,121,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,- 1,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,128,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,130,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,- 1,- 1,- 1,- 1,130,- 1,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,131,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,- 1,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,131,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,132,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,133,133,133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,- 1,- 1,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,133,- 1,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,- 1,- 1,- 1,- 1,134,- 1,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,136,- 1,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,137,- 1,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,138,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,139,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,140,140,140,140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,140,- 1,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,141,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,142,142,142,142,142,142,142,142,142,142,- 1,- 1,- 1,- 1,- 1,- 1,- 1,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,- 1,- 1,- 1,- 1,142,- 1,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,143,143,143,143,143,143,143,143,143,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,- 1,- 1,- 1,- 1,143,- 1,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,- 1,- 1,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,159,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,- 1,- 1,- 1,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,163,- 1,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,167,167,167,167,167,167,167,167,167,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,- 1,- 1,- 1,- 1,167,159,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,- 1,- 1,- 1,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,175,163,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,179,179,179,179,179,179,179,179,179,179,- 1,- 1,- 1,- 1,- 1,- 1,- 1,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,- 1,- 1,- 1,- 1,179,167,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,- 1,- 1,- 1,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,187,175,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,- 1,- 1,- 1,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,192,179,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,- 1,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,201,187,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,205,205,205,205,205,205,205,205,205,205,- 1,- 1,- 1,- 1,- 1,- 1,- 1,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,- 1,- 1,- 1,- 1,205,192,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,- 1,- 1,- 1,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,207,201,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,217,217,217,217,217,217,217,217,217,217,- 1,- 1,- 1,- 1,- 1,- 1,- 1,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,- 1,- 1,- 1,- 1,217,205,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,- 1,- 1,- 1,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,222,207,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,234,234,234,234,234,234,234,234,234,234,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,234,234,- 1,- 1,- 1,- 1,235,234,235,235,235,235,235,235,235,235,235,235,244,244,244,244,244,244,244,244,244,244,- 1,235,235,- 1,234,234,- 1,- 1,235,217,- 1,234,244,- 1,- 1,- 1,- 1,235,244,- 1,245,245,245,245,245,245,245,245,245,245,- 1,- 1,- 1,235,235,- 1,- 1,- 1,- 1,- 1,235,245,245,- 1,244,- 1,- 1,- 1,245,235,244,- 1,246,- 1,246,246,246,246,246,246,246,246,246,246,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,245,245,246,246,- 1,- 1,- 1,245,- 1,246,- 1,222,- 1,- 1,- 1,- 1,- 1,- 1,246,247,- 1,247,247,247,247,247,247,247,247,247,247,- 1,- 1,- 1,246,246,- 1,- 1,- 1,- 1,- 1,246,247,247,- 1,- 1,- 1,- 1,- 1,247,246,- 1,- 1,- 1,- 1,- 1,- 1,- 1,247,- 1,- 1,274,274,274,274,274,274,274,274,274,274,- 1,- 1,- 1,247,247,- 1,- 1,- 1,- 1,- 1,247,274,274,- 1,- 1,- 1,- 1,- 1,274,247,250,250,250,250,250,250,250,250,250,250,- 1,- 1,- 1,- 1,- 1,- 1,- 1,250,250,250,250,250,250,274,274,- 1,- 1,- 1,- 1,- 1,274,- 1,- 1,- 1,- 1,- 1,- 1,- 1,251,251,251,251,251,251,251,251,251,251,- 1,250,250,250,250,250,250,251,251,251,251,251,251,- 1,- 1,- 1,283,- 1,251,- 1,- 1,283,- 1,- 1,- 1,- 1,- 1,251,- 1,- 1,283,283,283,283,283,283,283,283,- 1,251,251,251,251,251,251,283,- 1,- 1,- 1,- 1,251,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,251,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,283,- 1,- 1,- 1,- 1,283,283,- 1,- 1,- 1,283,- 1,- 1,- 1,- 1,- 1,- 1,- 1,283,- 1,- 1,- 1,283,- 1,283,- 1,283,- 1,283,296,296,296,296,296,296,296,296,296,296,- 1,- 1,- 1,- 1,- 1,- 1,- 1,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,- 1,- 1,- 1,- 1,- 1,- 1,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,297,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,297,297,297,297,297,297,297,297,297,297,- 1,- 1,- 1,- 1,- 1,- 1,- 1,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,- 1,- 1,- 1,- 1,- 1,- 1,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 801
int state;int base;int backtrk;
int c;
state=start_state;
# 805
if(state >= 0){
({int _tmp1BA=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp1BA;});
lbuf->lex_last_action=- 1;}else{
# 809
state=(- state)- 1;}
# 811
while(1){
base=Cyc_Lex_lex_base[_check_known_subscript_notnull(322U,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 819
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 823
c=256;}}else{
# 825
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 828
if(Cyc_Lex_lex_check[_check_known_subscript_notnull(3971U,base + c)]== state)
state=Cyc_Lex_lex_trans[_check_known_subscript_notnull(3971U,base + c)];else{
# 831
state=Cyc_Lex_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1BB=({const char*_tmpFB="empty token";_tag_dyneither(_tmpFB,sizeof(char),12U);});_tmpFC->f1=_tmp1BB;});_tmpFC;}));else{
# 837
return lbuf->lex_last_action;}}else{
# 840
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 844
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1BC=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1BC;});
{int _tmpFD=lexstate;switch(_tmpFD){case 0U: _LL1: _LL2:
# 803 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 1U: _LL3: _LL4:
# 805
 return(int)Cyc_Lex_process_id(lexbuf);case 2U: _LL5: _LL6:
# 808 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 3U: _LL7: _LL8:
# 811 "lex.cyl"
({struct _dyneither_ptr _tmp1BD=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));Cyc_Lex_token_string=_tmp1BD;});
return 369;case 4U: _LL9: _LLA:
# 818 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 336;case 5U: _LLB: _LLC:
# 819 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 329;case 6U: _LLD: _LLE:
# 820 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 335;case 7U: _LLF: _LL10:
# 821 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 334;case 8U: _LL11: _LL12:
# 822 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 337;case 9U: _LL13: _LL14:
# 823 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 333;case 10U: _LL15: _LL16:
# 824 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 326;case 11U: _LL17: _LL18:
# 825 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 327;case 12U: _LL19: _LL1A:
# 826 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 328;case 13U: _LL1B: _LL1C:
# 827 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 330;case 14U: _LL1D: _LL1E:
# 828 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 331;case 15U: _LL1F: _LL20:
# 829 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 332;case 16U: _LL21: _LL22:
# 833 "lex.cyl"
({union Cyc_Absyn_Cnst _tmp1BE=Cyc_Lex_intconst(lexbuf,2,0,16);Cyc_Lex_token_int=_tmp1BE;});
return 363;case 17U: _LL23: _LL24:
# 836
({union Cyc_Absyn_Cnst _tmp1BF=Cyc_Lex_intconst(lexbuf,0,0,8);Cyc_Lex_token_int=_tmp1BF;});
return 363;case 18U: _LL25: _LL26:
# 842 "lex.cyl"
({union Cyc_Absyn_Cnst _tmp1C0=Cyc_Lex_intconst(lexbuf,0,0,10);Cyc_Lex_token_int=_tmp1C0;});
return 363;case 19U: _LL27: _LL28:
# 845
({union Cyc_Absyn_Cnst _tmp1C1=Cyc_Lex_intconst(lexbuf,0,0,10);Cyc_Lex_token_int=_tmp1C1;});
return 363;case 20U: _LL29: _LL2A:
# 850 "lex.cyl"
({struct _dyneither_ptr _tmp1C2=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_Lex_token_string=_tmp1C2;});
return 368;case 21U: _LL2B: _LL2C:
# 853
 return 345;case 22U: _LL2D: _LL2E:
# 854 "lex.cyl"
 return 346;case 23U: _LL2F: _LL30:
# 855 "lex.cyl"
 return 343;case 24U: _LL31: _LL32:
# 856 "lex.cyl"
 return 344;case 25U: _LL33: _LL34:
# 857 "lex.cyl"
 return 339;case 26U: _LL35: _LL36:
# 858 "lex.cyl"
 return 340;case 27U: _LL37: _LL38:
# 859 "lex.cyl"
 return 352;case 28U: _LL39: _LL3A:
# 860 "lex.cyl"
 return 353;case 29U: _LL3B: _LL3C:
# 861 "lex.cyl"
 return 349;case 30U: _LL3D: _LL3E:
# 862 "lex.cyl"
 return 350;case 31U: _LL3F: _LL40:
# 863 "lex.cyl"
 return 351;case 32U: _LL41: _LL42:
# 864 "lex.cyl"
 return 358;case 33U: _LL43: _LL44:
# 865 "lex.cyl"
 return 357;case 34U: _LL45: _LL46:
# 866 "lex.cyl"
 return 356;case 35U: _LL47: _LL48:
# 867 "lex.cyl"
 return 354;case 36U: _LL49: _LL4A:
# 868 "lex.cyl"
 return 355;case 37U: _LL4B: _LL4C:
# 869 "lex.cyl"
 return 347;case 38U: _LL4D: _LL4E:
# 870 "lex.cyl"
 return 348;case 39U: _LL4F: _LL50:
# 871 "lex.cyl"
 return 341;case 40U: _LL51: _LL52:
# 873
 return 342;case 41U: _LL53: _LL54:
# 874 "lex.cyl"
 return 360;case 42U: _LL55: _LL56:
# 875 "lex.cyl"
 return 338;case 43U: _LL57: _LL58:
# 876 "lex.cyl"
 return 359;case 44U: _LL59: _LL5A:
# 877 "lex.cyl"
 return 361;case 45U: _LL5B: _LL5C:
# 879
 return 312;case 46U: _LL5D: _LL5E:
# 882 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 47U: _LL5F: _LL60:
# 883 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 48U: _LL61: _LL62:
# 884 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 49U: _LL63: _LL64:
# 885 "lex.cyl"
 Cyc_Lex_comment_depth=1;
({int _tmp1C3=Cyc_Lexing_lexeme_start(lexbuf);Cyc_Lex_runaway_start=_tmp1C3;});
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 50U: _LL65: _LL66:
# 891 "lex.cyl"
 Cyc_Lex_string_pos=0;
({int _tmp1C4=Cyc_Lexing_lexeme_start(lexbuf);Cyc_Lex_runaway_start=_tmp1C4;});
while(Cyc_Lex_strng(lexbuf)){
;}
({struct _dyneither_ptr _tmp1C5=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_string=_tmp1C5;});
return 364;case 51U: _LL67: _LL68:
# 899 "lex.cyl"
 Cyc_Lex_string_pos=0;
({int _tmp1C6=Cyc_Lexing_lexeme_start(lexbuf);Cyc_Lex_runaway_start=_tmp1C6;});
while(Cyc_Lex_wstrng(lexbuf)){
;}
({struct _dyneither_ptr _tmp1C7=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_string=_tmp1C7;});
return 365;case 52U: _LL69: _LL6A:
# 907 "lex.cyl"
 Cyc_Lex_string_pos=0;
({int _tmp1C8=Cyc_Lexing_lexeme_start(lexbuf);Cyc_Lex_runaway_start=_tmp1C8;});
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
({struct _dyneither_ptr _tmp1C9=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_string=_tmp1C9;});
return 367;case 53U: _LL6B: _LL6C:
# 914
 Cyc_Lex_token_char='\a';return 366;case 54U: _LL6D: _LL6E:
# 915 "lex.cyl"
 Cyc_Lex_token_char='\b';return 366;case 55U: _LL6F: _LL70:
# 916 "lex.cyl"
 Cyc_Lex_token_char='\f';return 366;case 56U: _LL71: _LL72:
# 917 "lex.cyl"
 Cyc_Lex_token_char='\n';return 366;case 57U: _LL73: _LL74:
# 918 "lex.cyl"
 Cyc_Lex_token_char='\r';return 366;case 58U: _LL75: _LL76:
# 919 "lex.cyl"
 Cyc_Lex_token_char='\t';return 366;case 59U: _LL77: _LL78:
# 920 "lex.cyl"
 Cyc_Lex_token_char='\v';return 366;case 60U: _LL79: _LL7A:
# 921 "lex.cyl"
 Cyc_Lex_token_char='\\';return 366;case 61U: _LL7B: _LL7C:
# 922 "lex.cyl"
 Cyc_Lex_token_char='\'';return 366;case 62U: _LL7D: _LL7E:
# 923 "lex.cyl"
 Cyc_Lex_token_char='"';return 366;case 63U: _LL7F: _LL80:
# 924 "lex.cyl"
 Cyc_Lex_token_char='?';return 366;case 64U: _LL81: _LL82:
# 927 "lex.cyl"
({char _tmp1CB=({union Cyc_Absyn_Cnst _tmp1CA=Cyc_Lex_intconst(lexbuf,2,1,8);Cyc_Lex_cnst2char(_tmp1CA,lexbuf);});Cyc_Lex_token_char=_tmp1CB;});
return 366;case 65U: _LL83: _LL84:
# 932 "lex.cyl"
({char _tmp1CD=({union Cyc_Absyn_Cnst _tmp1CC=Cyc_Lex_intconst(lexbuf,3,1,16);Cyc_Lex_cnst2char(_tmp1CC,lexbuf);});Cyc_Lex_token_char=_tmp1CD;});
return 366;case 66U: _LL85: _LL86:
# 936
({char _tmp1CE=Cyc_Lexing_lexeme_char(lexbuf,1);Cyc_Lex_token_char=_tmp1CE;});
return 366;case 67U: _LL87: _LL88:
# 940
 return - 1;case 68U: _LL89: _LL8A:
# 942
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL8B: _LL8C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}_LL0:;}
# 946
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1CF=({const char*_tmpFE="some action didn't return!";_tag_dyneither(_tmpFE,sizeof(char),27U);});_tmpFF->f1=_tmp1CF;});_tmpFF;}));}
# 948
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1D0=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1D0;});
{int _tmp100=lexstate;switch(_tmp100){case 0U: _LL1: _LL2:
# 945 "lex.cyl"
 return 0;case 1U: _LL3: _LL4:
# 947
 Cyc_Lex_store_string_char('\a');return 1;case 2U: _LL5: _LL6:
# 948 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3U: _LL7: _LL8:
# 949 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4U: _LL9: _LLA:
# 950 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5U: _LLB: _LLC:
# 951 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6U: _LLD: _LLE:
# 952 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7U: _LLF: _LL10:
# 953 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8U: _LL11: _LL12:
# 954 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9U: _LL13: _LL14:
# 955 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10U: _LL15: _LL16:
# 956 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11U: _LL17: _LL18:
# 957 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12U: _LL19: _LL1A:
# 960 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13U: _LL1B: _LL1C:
# 963 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14U: _LL1D: _LL1E:
# 965
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15U: _LL1F: _LL20:
# 967
 Cyc_Lex_inc_linenumber();({struct _dyneither_ptr _tmp1D1=({const char*_tmp101="wide character ends in newline";_tag_dyneither(_tmp101,sizeof(char),31U);});Cyc_Lex_runaway_err(_tmp1D1,lexbuf);});return 0;case 16U: _LL21: _LL22:
# 968 "lex.cyl"
({struct _dyneither_ptr _tmp1D2=({const char*_tmp102="unterminated wide character";_tag_dyneither(_tmp102,sizeof(char),28U);});Cyc_Lex_runaway_err(_tmp1D2,lexbuf);});return 0;case 17U: _LL23: _LL24:
# 969 "lex.cyl"
({struct _dyneither_ptr _tmp1D3=({const char*_tmp103="bad character following backslash in wide character";_tag_dyneither(_tmp103,sizeof(char),52U);});Cyc_Lex_err(_tmp1D3,lexbuf);});return 1;default: _LL25: _LL26:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}_LL0:;}
# 973
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1D4=({const char*_tmp104="some action didn't return!";_tag_dyneither(_tmp104,sizeof(char),27U);});_tmp105->f1=_tmp1D4;});_tmp105;}));}
# 975
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,1);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1D5=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1D5;});
{int _tmp106=lexstate;switch(_tmp106){case 0U: _LL1: _LL2:
# 974 "lex.cyl"
 return 1;case 1U: _LL3: _LL4:
# 975 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2U: _LL5: _LL6:
# 976 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3U: _LL7: _LL8:
# 978
 lexbuf->lex_curr_pos -=1;return 0;default: _LL9: _LLA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}_LL0:;}
# 982
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1D6=({const char*_tmp107="some action didn't return!";_tag_dyneither(_tmp107,sizeof(char),27U);});_tmp108->f1=_tmp1D6;});_tmp108;}));}
# 984
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,2);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1D7=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1D7;});
{int _tmp109=lexstate;switch(_tmp109){case 0U: _LL1: _LL2:
# 982 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1U: _LL3: _LL4:
# 983 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2U: _LL5: _LL6:
# 984 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3U: _LL7: _LL8:
# 985 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4U: _LL9: _LLA:
# 986 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5U: _LLB: _LLC:
# 987 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('n');}else{
# 991
Cyc_Lex_store_string_char('\n');}
return 1;case 6U: _LLD: _LLE:
# 994 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7U: _LLF: _LL10:
# 995 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('t');}else{
# 999
Cyc_Lex_store_string_char('\t');}
return 1;case 8U: _LL11: _LL12:
# 1001 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9U: _LL13: _LL14:
# 1002 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10U: _LL15: _LL16:
# 1003 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11U: _LL17: _LL18:
# 1004 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12U: _LL19: _LL1A:
# 1005 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13U: _LL1B: _LL1C:
# 1008 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _tmp1D8=Cyc_Lex_intconst(lexbuf,1,0,8);Cyc_Lex_cnst2char(_tmp1D8,lexbuf);}));
return 1;case 14U: _LL1D: _LL1E:
# 1013 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _tmp1D9=Cyc_Lex_intconst(lexbuf,2,0,16);Cyc_Lex_cnst2char(_tmp1D9,lexbuf);}));
return 1;case 15U: _LL1F: _LL20:
# 1017
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 16U: _LL21: _LL22:
# 1019 "lex.cyl"
 Cyc_Lex_inc_linenumber();
({struct _dyneither_ptr _tmp1DA=({const char*_tmp10A="string ends in newline";_tag_dyneither(_tmp10A,sizeof(char),23U);});Cyc_Lex_runaway_err(_tmp1DA,lexbuf);});
return 0;case 17U: _LL23: _LL24:
# 1023 "lex.cyl"
({struct _dyneither_ptr _tmp1DB=({const char*_tmp10B="unterminated string";_tag_dyneither(_tmp10B,sizeof(char),20U);});Cyc_Lex_runaway_err(_tmp1DB,lexbuf);});
return 0;case 18U: _LL25: _LL26:
# 1026 "lex.cyl"
({struct _dyneither_ptr _tmp1DC=({const char*_tmp10C="bad character following backslash in string";_tag_dyneither(_tmp10C,sizeof(char),44U);});Cyc_Lex_err(_tmp1DC,lexbuf);});
return 1;default: _LL27: _LL28:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}_LL0:;}
# 1031
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1DD=({const char*_tmp10D="some action didn't return!";_tag_dyneither(_tmp10D,sizeof(char),27U);});_tmp10E->f1=_tmp1DD;});_tmp10E;}));}
# 1033
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,3);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1DE=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1DE;});
{int _tmp10F=lexstate;switch(_tmp10F){case 0U: _LL1: _LL2:
# 1036 "lex.cyl"
 Cyc_Lex_store_string(({const char*_tmp110="\" L\"";_tag_dyneither(_tmp110,sizeof(char),5U);}));return 1;case 1U: _LL3: _LL4:
# 1037 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2U: _LL5: _LL6:
# 1038 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3U: _LL7: _LL8:
# 1040
 lexbuf->lex_curr_pos -=1;return 0;default: _LL9: _LLA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}_LL0:;}
# 1044
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1DF=({const char*_tmp111="some action didn't return!";_tag_dyneither(_tmp111,sizeof(char),27U);});_tmp112->f1=_tmp1DF;});_tmp112;}));}
# 1046
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,4);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1E0=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1E0;});
{int _tmp113=lexstate;switch(_tmp113){case 0U: _LL1: _LL2:
# 1043 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1U: _LL3: _LL4:
# 1045
 Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,1));
return 1;case 2U: _LL5: _LL6:
# 1051 "lex.cyl"
 Cyc_Lex_store_string((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3U: _LL7: _LL8:
# 1053 "lex.cyl"
 Cyc_Lex_inc_linenumber();
({struct _dyneither_ptr _tmp1E1=({const char*_tmp114="string ends in newline";_tag_dyneither(_tmp114,sizeof(char),23U);});Cyc_Lex_runaway_err(_tmp1E1,lexbuf);});
return 0;case 4U: _LL9: _LLA:
# 1056 "lex.cyl"
({struct _dyneither_ptr _tmp1E2=({const char*_tmp115="unterminated string";_tag_dyneither(_tmp115,sizeof(char),20U);});Cyc_Lex_runaway_err(_tmp1E2,lexbuf);});
return 0;case 5U: _LLB: _LLC:
# 1058 "lex.cyl"
({struct _dyneither_ptr _tmp1E3=({const char*_tmp116="bad character following backslash in string";_tag_dyneither(_tmp116,sizeof(char),44U);});Cyc_Lex_err(_tmp1E3,lexbuf);});
return 1;default: _LLD: _LLE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}_LL0:;}
# 1063
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1E4=({const char*_tmp117="some action didn't return!";_tag_dyneither(_tmp117,sizeof(char),27U);});_tmp118->f1=_tmp1E4;});_tmp118;}));}
# 1065
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,5);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1E5=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1E5;});
{int _tmp119=lexstate;switch(_tmp119){case 0U: _LL1: _LL2:
# 1061 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1U: _LL3: _LL4:
# 1062 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 1067 "lex.cyl"
({struct _dyneither_ptr _tmp1E6=({const char*_tmp11A="unterminated comment";_tag_dyneither(_tmp11A,sizeof(char),21U);});Cyc_Lex_runaway_err(_tmp1E6,lexbuf);});
return 0;case 3U: _LL7: _LL8:
# 1070 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4U: _LL9: _LLA:
# 1071 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5U: _LLB: _LLC:
# 1072 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6U: _LLD: _LLE:
# 1073 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LLF: _LL10:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}_LL0:;}
# 1077
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1E7=({const char*_tmp11B="some action didn't return!";_tag_dyneither(_tmp11B,sizeof(char),27U);});_tmp11C->f1=_tmp1E7;});_tmp11C;}));}
# 1079
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,6);}
int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1E8=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1E8;});
{int _tmp11D=lexstate;switch(_tmp11D){case 0U: _LL1: _LL2:
# 1080 "lex.cyl"
 Cyc_Lex_token_asm.f1=0;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
({int _tmp1E9=Cyc_Lexing_lexeme_start(lexbuf);Cyc_Lex_runaway_start=_tmp1E9;});
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
({struct _dyneither_ptr _tmp1EA=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_asm.f2=_tmp1EA;});
return 374;case 1U: _LL3: _LL4:
# 1090 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
({int _tmp1EB=Cyc_Lexing_lexeme_start(lexbuf);Cyc_Lex_runaway_start=_tmp1EB;});
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
({struct _dyneither_ptr _tmp1EC=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_asm.f2=_tmp1EC;});
return 374;case 2U: _LL5: _LL6:
# 1100 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
({int _tmp1ED=Cyc_Lexing_lexeme_start(lexbuf);Cyc_Lex_runaway_start=_tmp1ED;});
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
({struct _dyneither_ptr _tmp1EE=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_asm.f2=_tmp1EE;});
return 374;default: _LL7: _LL8:
# 1108
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}_LL0:;}
# 1111
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1EF=({const char*_tmp11E="some action didn't return!";_tag_dyneither(_tmp11E,sizeof(char),27U);});_tmp11F->f1=_tmp1EF;});_tmp11F;}));}
# 1113
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,7);}
int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp1F0=Cyc_Lex_lex_engine(lexstate,lexbuf);lexstate=_tmp1F0;});
{int _tmp120=lexstate;switch(_tmp120){case 0U: _LL1: _LL2:
# 1110 "lex.cyl"
 Cyc_Lex_store_string_char('"');
Cyc_Lex_expand_specials=1;
while(Cyc_Lex_strng(lexbuf)){;}
Cyc_Lex_expand_specials=0;
Cyc_Lex_store_string_char('"');
return 1;case 1U: _LL3: _LL4:
# 1116 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_comment(lexbuf);
Cyc_Lex_store_string_char(' ');
return 1;case 2U: _LL5: _LL6:
# 1120 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_store_string_char('\n');return 1;case 3U: _LL7: _LL8:
# 1122 "lex.cyl"
 Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return 1;case 4U: _LL9: _LLA:
# 1123 "lex.cyl"
 Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{
# 1126
Cyc_Lex_store_string_char(')');
return 1;}case 5U: _LLB: _LLC:
# 1129 "lex.cyl"
({struct _dyneither_ptr _tmp1F1=({const char*_tmp121="unterminated asm";_tag_dyneither(_tmp121,sizeof(char),17U);});Cyc_Lex_runaway_err(_tmp1F1,lexbuf);});return 0;case 6U: _LLD: _LLE:
# 1130 "lex.cyl"
 Cyc_Lex_inc_linenumber();Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 7U: _LLF: _LL10:
# 1131 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LL11: _LL12:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}_LL0:;}
# 1135
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp1F2=({const char*_tmp122="some action didn't return!";_tag_dyneither(_tmp122,sizeof(char),27U);});_tmp123->f1=_tmp1F2;});_tmp123;}));}
# 1137
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,8);}
# 1140 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}
# 1145
static struct Cyc_Xarray_Xarray*Cyc_Lex_empty_xarray(struct _RegionHandle*id_rgn,int dummy){
struct Cyc_Xarray_Xarray*symbols=({
struct _RegionHandle*_tmp1F4=id_rgn;((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(_tmp1F4,101,({struct _dyneither_ptr*_tmp125=_cycalloc(sizeof(*_tmp125));({struct _dyneither_ptr _tmp1F3=(struct _dyneither_ptr)({const char*_tmp124="";_tag_dyneither(_tmp124,sizeof(char),1U);});*_tmp125=_tmp1F3;});_tmp125;}));});
# 1149
((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);
return symbols;}
# 1153
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1155
Cyc_Lex_in_extern_c=0;{
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp126=Cyc_Lex_prev_extern_c;struct Cyc_List_List*_tmp127=x;Cyc_Lex_prev_extern_c=_tmp127;x=_tmp126;});
while(x != 0){
struct Cyc_List_List*t=x->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);
x=t;}
# 1164
if(Cyc_Lex_ids_trie != 0){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp128=idt;struct Cyc_Lex_DynTrieSymbols*_tmp129=Cyc_Lex_ids_trie;idt=_tmp129;Cyc_Lex_ids_trie=_tmp128;});{
struct Cyc_Lex_DynTrieSymbols _tmp12A=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp12B=_tmp12A;struct Cyc_Core_DynamicRegion*_tmp12C;_LL1: _tmp12C=_tmp12B.dyn;_LL2:;
Cyc_Core_free_ukey(_tmp12C);
((void(*)(struct Cyc_Lex_DynTrieSymbols*ptr))Cyc_Core_ufree)(idt);};}
# 1171
if(Cyc_Lex_typedefs_trie != 0){
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp12D=tdefs;struct Cyc_Lex_DynTrie*_tmp12E=Cyc_Lex_typedefs_trie;tdefs=_tmp12E;Cyc_Lex_typedefs_trie=_tmp12D;});{
struct Cyc_Lex_DynTrie _tmp12F=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmp130=_tmp12F;struct Cyc_Core_DynamicRegion*_tmp131;_LL4: _tmp131=_tmp130.dyn;_LL5:;
Cyc_Core_free_ukey(_tmp131);
((void(*)(struct Cyc_Lex_DynTrie*ptr))Cyc_Core_ufree)(tdefs);};}{
# 1179
struct Cyc_Core_NewDynamicRegion _tmp132=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp133=_tmp132;struct Cyc_Core_DynamicRegion*_tmp13E;_LL7: _tmp13E=_tmp133.key;_LL8:;{
struct Cyc_Lex_Trie*_tmp134=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp13E,0,Cyc_Lex_empty_trie);
struct Cyc_Xarray_Xarray*_tmp135=((struct Cyc_Xarray_Xarray*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Xarray_Xarray*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp13E,0,Cyc_Lex_empty_xarray);
({struct Cyc_Lex_DynTrieSymbols*_tmp1F5=({struct Cyc_Lex_DynTrieSymbols*_tmp136=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp136));_tmp136->dyn=_tmp13E,_tmp136->t=_tmp134,_tmp136->symbols=_tmp135;_tmp136;});Cyc_Lex_ids_trie=_tmp1F5;});{
# 1184
struct Cyc_Core_NewDynamicRegion _tmp137=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp138=_tmp137;struct Cyc_Core_DynamicRegion*_tmp13D;_LLA: _tmp13D=_tmp138.key;_LLB:;{
struct Cyc_Lex_Trie*t=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp13D,0,Cyc_Lex_empty_trie);
({struct Cyc_Lex_DynTrie*_tmp1F6=({struct Cyc_Lex_DynTrie*_tmp139=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp139));_tmp139->dyn=_tmp13D,_tmp139->t=t;_tmp139;});Cyc_Lex_typedefs_trie=_tmp1F6;});
({int _tmp1F7=Cyc_Lex_num_keywords(include_cyclone_keywords);Cyc_Lex_num_kws=_tmp1F7;});
({struct _dyneither_ptr _tmp1F8=({unsigned int _tmp13B=(unsigned int)Cyc_Lex_num_kws;struct Cyc_Lex_KeyWordInfo*_tmp13A=_cycalloc(_check_times(_tmp13B,sizeof(struct Cyc_Lex_KeyWordInfo)));({{unsigned int _tmp164=(unsigned int)Cyc_Lex_num_kws;unsigned int i;for(i=0;i < _tmp164;++ i){(_tmp13A[i]).token_index=0,(_tmp13A[i]).is_c_keyword=0;}}0;});_tag_dyneither(_tmp13A,sizeof(struct Cyc_Lex_KeyWordInfo),_tmp13B);});Cyc_Lex_kw_nums=_tmp1F8;});{
unsigned int i=0U;
unsigned int rwsze=74U;
{unsigned int j=0U;for(0;j < rwsze;++ j){
if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){
struct _dyneither_ptr _tmp13C=(Cyc_Lex_rw_array[(int)j]).f1;
({struct _dyneither_ptr _tmp1F9=_tmp13C;Cyc_Lex_str_index(_tmp1F9,0,(int)Cyc_strlen((struct _dyneither_ptr)_tmp13C));});
({struct Cyc_Lex_KeyWordInfo _tmp1FA=({struct Cyc_Lex_KeyWordInfo _tmp165;_tmp165.token_index=(int)(Cyc_Lex_rw_array[(int)j]).f2,_tmp165.is_c_keyword=(Cyc_Lex_rw_array[(int)j]).f3;_tmp165;});*((struct Cyc_Lex_KeyWordInfo*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),(int)i))=_tmp1FA;});
++ i;}}}
# 1199
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};};};};};};}
