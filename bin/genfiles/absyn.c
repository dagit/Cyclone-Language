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
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char *)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  if (!_cks_ptr) _throw_null(); \
  (_cks_ptr) + (elt_sz)*(index); })
#endif

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
 struct Cyc_Core_Opt{void*v;};
# 119 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr);
# 34 "position.h"
unsigned Cyc_Position_segment_join(unsigned,unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 323
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 505
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr);
# 508
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 510
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr);
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 673 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 881
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 896
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 902
void*Cyc_Absyn_compress_kb(void*);
# 910
void*Cyc_Absyn_app_type(void*,struct _dyneither_ptr);
# 913
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 918
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 920
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 922
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 925
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 929
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 931
void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);
# 946
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 956
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned);
void*Cyc_Absyn_bounds_one();
# 962
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 964
void*Cyc_Absyn_starb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*bounds,void*zero_term);
# 967
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 970
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 973
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 989
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 991
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1059
struct _tuple0*Cyc_Absyn_uniquergn_qvar();
# 1062
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned loc);
# 1086
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s);
# 1090
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc);
# 1095
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc);
# 1136
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1138
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1141
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1149
struct _dyneither_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);
# 1157
int Cyc_Absyn_attribute_cmp(void*att1,void*att2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
void*Cyc_Warn_impos_loc(unsigned loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 112 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 141
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 170
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 178
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 45 "evexp.h"
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 30 "absyn.cyc"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);
# 37
int Cyc_Cyclone_tovc_r=0;
# 39
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
# 46
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _dyneither_ptr*)ss1->hd,(struct _dyneither_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;};}
# 53
if(ss2 != 0)return - 1;
return 0;}
# 56
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple10{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 60
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;union Cyc_Absyn_Nmspace _tmpD;struct _dyneither_ptr*_tmpC;_LL1: _tmpD=_tmp0->f1;_tmpC=_tmp0->f2;_LL2:;{
struct _tuple0*_tmp1=q2;union Cyc_Absyn_Nmspace _tmpB;struct _dyneither_ptr*_tmpA;_LL4: _tmpB=_tmp1->f1;_tmpA=_tmp1->f2;_LL5:;{
int i=Cyc_strptrcmp(_tmpC,_tmpA);
if(i != 0)return i;{
struct _tuple10 _tmp2=({struct _tuple10 _tmp156;_tmp156.f1=_tmpD,_tmp156.f2=_tmpB;_tmp156;});struct _tuple10 _tmp3=_tmp2;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp4;switch(((_tmp3.f1).Abs_n).tag){case 4U: if(((_tmp3.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 72
 return - 1;}case 1U: switch(((_tmp3.f2).Loc_n).tag){case 1U: _LL9: _tmp5=((_tmp3.f1).Rel_n).val;_tmp4=((_tmp3.f2).Rel_n).val;_LLA:
# 68
 return Cyc_Absyn_strlist_cmp(_tmp5,_tmp4);case 4U: goto _LL11;default: _LL13: _LL14:
# 74
 return - 1;}case 2U: switch(((_tmp3.f2).Rel_n).tag){case 2U: _LLB: _tmp7=((_tmp3.f1).Abs_n).val;_tmp6=((_tmp3.f2).Abs_n).val;_LLC:
# 69
 return Cyc_Absyn_strlist_cmp(_tmp7,_tmp6);case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 76
 return - 1;}default: switch(((_tmp3.f2).Rel_n).tag){case 3U: _LLD: _tmp9=((_tmp3.f1).C_n).val;_tmp8=((_tmp3.f2).C_n).val;_LLE:
# 70
 return Cyc_Absyn_strlist_cmp(_tmp9,_tmp8);case 4U: _LL11: _LL12:
# 73
 return 1;case 1U: _LL15: _LL16:
# 75
 return 1;default: _LL19: _LL1A:
# 77
 return 1;}}_LL6:;};};};};}
# 81
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 87
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
return C_scope?({union Cyc_Absyn_Nmspace _tmp157;(_tmp157.C_n).tag=3U,(_tmp157.C_n).val=x;_tmp157;}):({union Cyc_Absyn_Nmspace _tmp158;(_tmp158.Abs_n).tag=2U,(_tmp158.Abs_n).val=x;_tmp158;});}
# 91
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp159;(_tmp159.Rel_n).tag=1U,(_tmp159.Rel_n).val=x;_tmp159;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 94
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _tmpF=_tmpE;switch((_tmpF.Loc_n).tag){case 1U: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 103
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1U,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 107
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 111
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp15A;_tmp15A.print_const=0,_tmp15A.q_volatile=0,_tmp15A.q_restrict=0,_tmp15A.real_const=0,_tmp15A.loc=loc;_tmp15A;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){return({struct Cyc_Absyn_Tqual _tmp15B;_tmp15B.print_const=1,_tmp15B.q_volatile=0,_tmp15B.q_restrict=0,_tmp15B.real_const=1,_tmp15B.loc=loc;_tmp15B;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp15C;_tmp15C.print_const=x.print_const || y.print_const,_tmp15C.q_volatile=
x.q_volatile || y.q_volatile,_tmp15C.q_restrict=
x.q_restrict || y.q_restrict,_tmp15C.real_const=
x.real_const || y.real_const,({
unsigned _tmp183=Cyc_Position_segment_join(x.loc,y.loc);_tmp15C.loc=_tmp183;});_tmp15C;});}
# 120
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const && tq1.q_volatile == tq2.q_volatile)&& tq1.q_restrict == tq2.q_restrict;}
# 126
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 128
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp15D;(_tmp15D.UnknownDatatype).tag=1U,(_tmp15D.UnknownDatatype).val=udi;_tmp15D;});}
# 131
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp15E;(_tmp15E.KnownDatatype).tag=2U,(_tmp15E.KnownDatatype).val=d;_tmp15E;});}
# 134
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp15F;(_tmp15F.UnknownDatatypefield).tag=1U,(_tmp15F.UnknownDatatypefield).val=s;_tmp15F;});}
# 137
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp160;(_tmp160.KnownDatatypefield).tag=2U,((_tmp160.KnownDatatypefield).val).f1=dd,((_tmp160.KnownDatatypefield).val).f2=df;_tmp160;});}
# 140
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp161;(_tmp161.UnknownAggr).tag=1U,((_tmp161.UnknownAggr).val).f1=ak,((_tmp161.UnknownAggr).val).f2=n,((_tmp161.UnknownAggr).val).f3=tagged;_tmp161;});}
# 143
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp162;(_tmp162.KnownAggr).tag=2U,(_tmp162.KnownAggr).val=ad;_tmp162;});}
# 146
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp11=k;void**_tmp13;void**_tmp12;switch(*((int*)_tmp11)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp12=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LL8:
# 151
 _tmp13=_tmp12;goto _LLA;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL5: _LL6:
# 150
 return k;}else{_LL9: _tmp13=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LLA:
# 153
({void*_tmp184=Cyc_Absyn_compress_kb(*_tmp13);*_tmp13=_tmp184;});
return*_tmp13;}}_LL0:;}
# 157
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp14=Cyc_Absyn_compress_kb(kb);void*_tmp15=_tmp14;struct Cyc_Core_Opt**_tmp1A;struct Cyc_Absyn_Kind*_tmp19;struct Cyc_Core_Opt**_tmp18;struct Cyc_Absyn_Kind*_tmp17;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp17=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL2:
 return _tmp17;case 1U: _LL3: _tmp18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL4:
 _tmp1A=_tmp18;_tmp19=& Cyc_Tcutil_bk;goto _LL6;default: _LL5: _tmp1A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f1;_tmp19=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f2;_LL6:
# 162
({struct Cyc_Core_Opt*_tmp186=({struct Cyc_Core_Opt*_tmp16=_cycalloc(sizeof(*_tmp16));({void*_tmp185=Cyc_Tcutil_kind_to_bound(_tmp19);_tmp16->v=_tmp185;});_tmp16;});*_tmp1A=_tmp186;});
return _tmp19;}_LL0:;}
# 168
void*Cyc_Absyn_app_type(void*c,struct _dyneither_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=c,({struct Cyc_List_List*_tmp187=((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(args);_tmp1B->f2=_tmp187;});_tmp1B;});}
# 171
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=9U,_tmp1C->f1=e;_tmp1C;});}
# 187 "absyn.cyc"
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_Absyn_void_type_cval={0U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_void_type_tval={0U,(void*)& Cyc_Absyn_void_type_cval,0};void*Cyc_Absyn_void_type=(void*)& Cyc_Absyn_void_type_tval;
static struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct Cyc_Absyn_heap_rgn_type_cval={5U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_heap_rgn_type_tval={0U,(void*)& Cyc_Absyn_heap_rgn_type_cval,0};void*Cyc_Absyn_heap_rgn_type=(void*)& Cyc_Absyn_heap_rgn_type_tval;
static struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct Cyc_Absyn_unique_rgn_type_cval={6U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_unique_rgn_type_tval={0U,(void*)& Cyc_Absyn_unique_rgn_type_cval,0};void*Cyc_Absyn_unique_rgn_type=(void*)& Cyc_Absyn_unique_rgn_type_tval;
static struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct Cyc_Absyn_refcnt_rgn_type_cval={7U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_refcnt_rgn_type_tval={0U,(void*)& Cyc_Absyn_refcnt_rgn_type_cval,0};void*Cyc_Absyn_refcnt_rgn_type=(void*)& Cyc_Absyn_refcnt_rgn_type_tval;
static struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct Cyc_Absyn_true_type_cval={11U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_true_type_tval={0U,(void*)& Cyc_Absyn_true_type_cval,0};void*Cyc_Absyn_true_type=(void*)& Cyc_Absyn_true_type_tval;
static struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct Cyc_Absyn_false_type_cval={12U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_false_type_tval={0U,(void*)& Cyc_Absyn_false_type_cval,0};void*Cyc_Absyn_false_type=(void*)& Cyc_Absyn_false_type_tval;
static struct Cyc_Absyn_FatCon_Absyn_TyCon_struct Cyc_Absyn_fat_bound_type_cval={14U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_fat_bound_type_tval={0U,(void*)& Cyc_Absyn_fat_bound_type_cval,0};void*Cyc_Absyn_fat_bound_type=(void*)& Cyc_Absyn_fat_bound_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_schar_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_schar_type_tval={0U,(void*)& Cyc_Absyn_schar_type_cval,0};void*Cyc_Absyn_schar_type=(void*)& Cyc_Absyn_schar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uchar_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uchar_type_tval={0U,(void*)& Cyc_Absyn_uchar_type_cval,0};void*Cyc_Absyn_uchar_type=(void*)& Cyc_Absyn_uchar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_char_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_char_type_tval={0U,(void*)& Cyc_Absyn_char_type_cval,0};void*Cyc_Absyn_char_type=(void*)& Cyc_Absyn_char_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sshort_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sshort_type_tval={0U,(void*)& Cyc_Absyn_sshort_type_cval,0};void*Cyc_Absyn_sshort_type=(void*)& Cyc_Absyn_sshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ushort_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ushort_type_tval={0U,(void*)& Cyc_Absyn_ushort_type_cval,0};void*Cyc_Absyn_ushort_type=(void*)& Cyc_Absyn_ushort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nshort_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nshort_type_tval={0U,(void*)& Cyc_Absyn_nshort_type_cval,0};void*Cyc_Absyn_nshort_type=(void*)& Cyc_Absyn_nshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sint_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sint_type_tval={0U,(void*)& Cyc_Absyn_sint_type_cval,0};void*Cyc_Absyn_sint_type=(void*)& Cyc_Absyn_sint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uint_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uint_type_tval={0U,(void*)& Cyc_Absyn_uint_type_cval,0};void*Cyc_Absyn_uint_type=(void*)& Cyc_Absyn_uint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nint_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nint_type_tval={0U,(void*)& Cyc_Absyn_nint_type_cval,0};void*Cyc_Absyn_nint_type=(void*)& Cyc_Absyn_nint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slong_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slong_type_tval={0U,(void*)& Cyc_Absyn_slong_type_cval,0};void*Cyc_Absyn_slong_type=(void*)& Cyc_Absyn_slong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulong_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulong_type_tval={0U,(void*)& Cyc_Absyn_ulong_type_cval,0};void*Cyc_Absyn_ulong_type=(void*)& Cyc_Absyn_ulong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlong_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlong_type_tval={0U,(void*)& Cyc_Absyn_nlong_type_cval,0};void*Cyc_Absyn_nlong_type=(void*)& Cyc_Absyn_nlong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slonglong_type_cval={1U,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slonglong_type_tval={0U,(void*)& Cyc_Absyn_slonglong_type_cval,0};void*Cyc_Absyn_slonglong_type=(void*)& Cyc_Absyn_slonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulonglong_type_cval={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulonglong_type_tval={0U,(void*)& Cyc_Absyn_ulonglong_type_cval,0};void*Cyc_Absyn_ulonglong_type=(void*)& Cyc_Absyn_ulonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlonglong_type_cval={1U,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlonglong_type_tval={0U,(void*)& Cyc_Absyn_nlonglong_type_cval,0};void*Cyc_Absyn_nlonglong_type=(void*)& Cyc_Absyn_nlonglong_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_float_type_cval={2U,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_float_type_tval={0U,(void*)& Cyc_Absyn_float_type_cval,0};void*Cyc_Absyn_float_type=(void*)& Cyc_Absyn_float_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_double_type_cval={2U,1};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_double_type_tval={0U,(void*)& Cyc_Absyn_double_type_cval,0};void*Cyc_Absyn_double_type=(void*)& Cyc_Absyn_double_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_long_double_type_cval={2U,2};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_long_double_type_tval={0U,(void*)& Cyc_Absyn_long_double_type_cval,0};void*Cyc_Absyn_long_double_type=(void*)& Cyc_Absyn_long_double_type_tval;
# 213
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9U};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0U,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 215
static struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct Cyc_Absyn_RgnHandleCon_val={3U};
static struct Cyc_Absyn_TagCon_Absyn_TyCon_struct Cyc_Absyn_TagCon_val={4U};
static struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct Cyc_Absyn_AccessCon_val={8U};
static struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct Cyc_Absyn_RgnsCon_val={10U};
static struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct Cyc_Absyn_ThinCon_val={13U};
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_JoinCon_val={9U};
void*Cyc_Absyn_rgn_handle_type(void*r){return({void*_tmp1D[1U];_tmp1D[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnHandleCon_val,_tag_dyneither(_tmp1D,sizeof(void*),1U));});}
void*Cyc_Absyn_tag_type(void*t){return({void*_tmp1E[1U];_tmp1E[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_TagCon_val,_tag_dyneither(_tmp1E,sizeof(void*),1U));});}
void*Cyc_Absyn_access_eff(void*r){return({void*_tmp1F[1U];_tmp1F[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_AccessCon_val,_tag_dyneither(_tmp1F,sizeof(void*),1U));});}
void*Cyc_Absyn_regionsof_eff(void*t){return({void*_tmp20[1U];_tmp20[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnsCon_val,_tag_dyneither(_tmp20,sizeof(void*),1U));});}
void*Cyc_Absyn_thin_bounds_type(void*t){return({void*_tmp21[1U];_tmp21[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_ThinCon_val,_tag_dyneither(_tmp21,sizeof(void*),1U));});}
void*Cyc_Absyn_join_eff(struct Cyc_List_List*ts){return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=0U,_tmp22->f1=(void*)& Cyc_Absyn_empty_effect_cval,_tmp22->f2=ts;_tmp22;});}
# 228
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return({void*_tmp23=0U;({void*_tmp188=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=15U,_tmp24->f1=n,_tmp24->f2=d;_tmp24;});Cyc_Absyn_app_type(_tmp188,_tag_dyneither(_tmp23,sizeof(void*),0U));});});}
# 231
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return({void*_tmp25=0U;({void*_tmp189=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=16U,_tmp26->f1=fs;_tmp26;});Cyc_Absyn_app_type(_tmp189,_tag_dyneither(_tmp25,sizeof(void*),0U));});});}
# 234
void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k){
return({void*_tmp27=0U;({void*_tmp18A=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=17U,_tmp28->f1=s,_tmp28->f2=k;_tmp28;});Cyc_Absyn_app_type(_tmp18A,_tag_dyneither(_tmp27,sizeof(void*),0U));});});}
# 237
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=0U,({void*_tmp18B=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=18U,_tmp29->f1=di;_tmp29;});_tmp2A->f1=_tmp18B;}),_tmp2A->f2=args;_tmp2A;});}
# 240
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,({void*_tmp18C=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=19U,_tmp2B->f1=di;_tmp2B;});_tmp2C->f1=_tmp18C;}),_tmp2C->f2=args;_tmp2C;});}
# 243
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=0U,({void*_tmp18D=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=20U,_tmp2D->f1=ai;_tmp2D;});_tmp2E->f1=_tmp18D;}),_tmp2E->f2=args;_tmp2E;});}
# 247
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=2U,_tmp2F->f1=x;_tmp2F;});}
void*Cyc_Absyn_gen_float_type(unsigned i){
unsigned _tmp30=i;switch(_tmp30){case 0U: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1U: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2U: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Int_pa_PrintArg_struct _tmp33=({struct Cyc_Int_pa_PrintArg_struct _tmp163;_tmp163.tag=1U,_tmp163.f1=(unsigned long)((int)i);_tmp163;});void*_tmp31[1U];_tmp31[0]=& _tmp33;({struct _dyneither_ptr _tmp18E=({const char*_tmp32="gen_float_type(%d)";_tag_dyneither(_tmp32,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp18E,_tag_dyneither(_tmp31,sizeof(void*),1U));});});}_LL0:;}
# 256
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp34=sn;switch(_tmp34){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 259
enum Cyc_Absyn_Size_of _tmp35=sz;switch(_tmp35){case Cyc_Absyn_Char_sz: _LLA: _LLB:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLC: _LLD:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLE: _LLF:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LL10: _LL11:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
 return Cyc_Absyn_slonglong_type;}_LL9:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 268
enum Cyc_Absyn_Size_of _tmp36=sz;switch(_tmp36){case Cyc_Absyn_Char_sz: _LL17: _LL18:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL19: _LL1A:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL1B: _LL1C:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL1D: _LL1E:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return Cyc_Absyn_ulonglong_type;}_LL16:;}case Cyc_Absyn_None: _LL5: _LL6:
# 276
 goto _LL8;default: _LL7: _LL8: {
# 278
enum Cyc_Absyn_Size_of _tmp37=sz;switch(_tmp37){case Cyc_Absyn_Char_sz: _LL24: _LL25:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL26: _LL27:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL28: _LL29:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz: _LL2C: _LL2D:
 goto _LL2F;default: _LL2E: _LL2F:
 return Cyc_Absyn_nlonglong_type;}_LL23:;}}_LL0:;}
# 289
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 292
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 297
void*Cyc_Absyn_bounds_one(){
static void*bone=0;
void*_tmp38=bone;
if(_tmp38 == 0){
_tmp38=Cyc_Absyn_thin_bounds_int(1U);
bone=_tmp38;}
# 304
return _tmp38;}
# 310
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 313
void*Cyc_Absyn_wchar_type(){
int _tmp39=Sizeof_wchar_t;switch(_tmp39){case 1U: _LL1: _LL2:
# 324 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 328
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp3A[4U]="exn";
# 333
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp3A,_tmp3A,_tmp3A + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp41[15U]="Null_Exception";static char _tmp42[13U]="Array_bounds";static char _tmp43[16U]="Match_Exception";static char _tmp44[10U]="Bad_alloc";
# 337
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud(){
static struct _dyneither_ptr builtin_exns[4U]={{_tmp41,_tmp41,_tmp41 + 15U},{_tmp42,_tmp42,_tmp42 + 13U},{_tmp43,_tmp43,_tmp43 + 16U},{_tmp44,_tmp44,_tmp44 + 10U}};
# 340
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt == 0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct Cyc_Absyn_Datatypefield*_tmp191=({struct Cyc_Absyn_Datatypefield*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct _tuple0*_tmp190=({struct _tuple0*_tmp3C=_cycalloc(sizeof(*_tmp3C));((_tmp3C->f1).Abs_n).tag=2U,((_tmp3C->f1).Abs_n).val=0,({
struct _dyneither_ptr*_tmp18F=({struct _dyneither_ptr*_tmp3B=_cycalloc(sizeof(*_tmp3B));*_tmp3B=*((struct _dyneither_ptr*)_check_known_subscript_notnull(builtin_exns,4U,sizeof(struct _dyneither_ptr),i));_tmp3B;});_tmp3C->f2=_tmp18F;});_tmp3C;});
# 344
_tmp3D->name=_tmp190;}),_tmp3D->typs=0,_tmp3D->loc=0U,_tmp3D->sc=Cyc_Absyn_Extern;_tmp3D;});_tmp3E->hd=_tmp191;}),_tmp3E->tl=tufs;_tmp3E;});}}
# 348
tud_opt=({struct Cyc_Absyn_Datatypedecl*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->sc=Cyc_Absyn_Extern,_tmp40->name=Cyc_Absyn_exn_name,_tmp40->tvs=0,({struct Cyc_Core_Opt*_tmp192=({struct Cyc_Core_Opt*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->v=tufs;_tmp3F;});_tmp40->fields=_tmp192;}),_tmp40->is_extensible=1;_tmp40;});}
# 350
return(struct Cyc_Absyn_Datatypedecl*)_check_null(tud_opt);}
# 353
void*Cyc_Absyn_exn_type(){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ == 0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp164;(_tmp164.KnownDatatype).tag=2U,({struct Cyc_Absyn_Datatypedecl**_tmp194=({struct Cyc_Absyn_Datatypedecl**_tmp45=_cycalloc(sizeof(*_tmp45));({struct Cyc_Absyn_Datatypedecl*_tmp193=Cyc_Absyn_exn_tud();*_tmp45=_tmp193;});_tmp45;});(_tmp164.KnownDatatype).val=_tmp194;});_tmp164;}),0);
exn_typ=({void*_tmp197=(void*)_check_null(eopt);void*_tmp196=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp195=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_type(_tmp197,_tmp196,_tmp195,Cyc_Absyn_false_type);});}
# 360
return(void*)_check_null(exn_typ);}
# 363
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar(){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp48=_cycalloc(sizeof(*_tmp48));({union Cyc_Absyn_Nmspace _tmp19A=Cyc_Absyn_Abs_n(0,0);_tmp48->f1=_tmp19A;}),({struct _dyneither_ptr*_tmp199=({struct _dyneither_ptr*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _dyneither_ptr _tmp198=({const char*_tmp46="PrintArg";_tag_dyneither(_tmp46,sizeof(char),9U);});*_tmp47=_tmp198;});_tmp47;});_tmp48->f2=_tmp199;});_tmp48;});
return(struct _tuple0*)_check_null(q);}
# 369
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar(){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp4B=_cycalloc(sizeof(*_tmp4B));({union Cyc_Absyn_Nmspace _tmp19D=Cyc_Absyn_Abs_n(0,0);_tmp4B->f1=_tmp19D;}),({struct _dyneither_ptr*_tmp19C=({struct _dyneither_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct _dyneither_ptr _tmp19B=({const char*_tmp49="ScanfArg";_tag_dyneither(_tmp49,sizeof(char),9U);});*_tmp4A=_tmp19B;});_tmp4A;});_tmp4B->f2=_tmp19C;});_tmp4B;});
return(struct _tuple0*)_check_null(q);}
# 381
struct _tuple0*Cyc_Absyn_uniquergn_qvar(){
static struct _tuple0*q=0;
if(q == 0)
q=({struct _tuple0*_tmp51=_cycalloc(sizeof(*_tmp51));({union Cyc_Absyn_Nmspace _tmp1A2=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));({struct _dyneither_ptr*_tmp1A1=({struct _dyneither_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct _dyneither_ptr _tmp1A0=({const char*_tmp4C="Core";_tag_dyneither(_tmp4C,sizeof(char),5U);});*_tmp4D=_tmp1A0;});_tmp4D;});_tmp4E->hd=_tmp1A1;}),_tmp4E->tl=0;_tmp4E;}),0);_tmp51->f1=_tmp1A2;}),({struct _dyneither_ptr*_tmp19F=({struct _dyneither_ptr*_tmp50=_cycalloc(sizeof(*_tmp50));({struct _dyneither_ptr _tmp19E=({const char*_tmp4F="unique_region";_tag_dyneither(_tmp4F,sizeof(char),14U);});*_tmp50=_tmp19E;});_tmp50;});_tmp51->f2=_tmp19F;});_tmp51;});
return(struct _tuple0*)_check_null(q);}
# 387
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp(){
static struct Cyc_Absyn_Exp*e=0;
if(e == 0){
void*_tmp52=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_unique_rgn_type);
e=({struct Cyc_Absyn_Exp*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->topt=_tmp52,_tmp56->loc=0U,_tmp56->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_tmp1A7=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=1U,({void*_tmp1A6=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=1U,({struct Cyc_Absyn_Vardecl*_tmp1A5=({struct Cyc_Absyn_Vardecl*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->sc=Cyc_Absyn_Extern,({struct _tuple0*_tmp1A4=Cyc_Absyn_uniquergn_qvar();_tmp53->name=_tmp1A4;}),_tmp53->varloc=0U,({
struct Cyc_Absyn_Tqual _tmp1A3=Cyc_Absyn_empty_tqual(0U);_tmp53->tq=_tmp1A3;}),_tmp53->type=_tmp52,_tmp53->initializer=0,_tmp53->rgn=0,_tmp53->attributes=0,_tmp53->escapes=0;_tmp53;});
# 392
_tmp54->f1=_tmp1A5;});_tmp54;});_tmp55->f1=_tmp1A6;});_tmp55;});_tmp56->r=_tmp1A7;});_tmp56;});}
# 396
return(struct Cyc_Absyn_Exp*)_check_null(e);}
# 400
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=3U,_tmp57->f1=s;_tmp57;});}
# 404
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp165;_tmp165.elt_type=t,_tmp165.elt_tq=tq,(_tmp165.ptr_atts).rgn=r,(_tmp165.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp165.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp165.ptr_atts).zero_term=zt,(_tmp165.ptr_atts).ptrloc=0;_tmp165;}));}
# 409
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp166;_tmp166.elt_type=t,_tmp166.elt_tq=tq,(_tmp166.ptr_atts).rgn=r,(_tmp166.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp166.ptr_atts).bounds=b,(_tmp166.ptr_atts).zero_term=zt,(_tmp166.ptr_atts).ptrloc=0;_tmp166;}));}
# 414
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zt){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp167;_tmp167.elt_type=t,_tmp167.elt_tq=tq,(_tmp167.ptr_atts).rgn=r,(_tmp167.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp167.ptr_atts).bounds=b,(_tmp167.ptr_atts).zero_term=zt,(_tmp167.ptr_atts).ptrloc=0;_tmp167;}));}
# 419
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp1AB=t;void*_tmp1AA=r;struct Cyc_Absyn_Tqual _tmp1A9=tq;void*_tmp1A8=Cyc_Absyn_bounds_one();Cyc_Absyn_starb_type(_tmp1AB,_tmp1AA,_tmp1A9,_tmp1A8,zeroterm);});}
# 422
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);}
# 425
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return({void*_tmp1AF=t;void*_tmp1AE=r;struct Cyc_Absyn_Tqual _tmp1AD=tq;void*_tmp1AC=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp1AF,_tmp1AE,_tmp1AD,_tmp1AC,zeroterm);});}
# 428
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp1B3=Cyc_Absyn_char_type;void*_tmp1B2=rgn;struct Cyc_Absyn_Tqual _tmp1B1=Cyc_Absyn_empty_tqual(0U);void*_tmp1B0=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp1B3,_tmp1B2,_tmp1B1,_tmp1B0,Cyc_Absyn_true_type);});}
# 431
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp1B7=Cyc_Absyn_char_type;void*_tmp1B6=rgn;struct Cyc_Absyn_Tqual _tmp1B5=Cyc_Absyn_const_tqual(0U);void*_tmp1B4=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp1B7,_tmp1B6,_tmp1B5,_tmp1B4,Cyc_Absyn_true_type);});}
# 435
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 437
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=4U,(_tmp58->f1).elt_type=elt_type,(_tmp58->f1).tq=tq,(_tmp58->f1).num_elts=num_elts,(_tmp58->f1).zero_term=zero_term,(_tmp58->f1).zt_loc=ztloc;_tmp58;});}
# 440
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=11U,_tmp59->f1=e;_tmp59;});}
# 445
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 447
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=8U,_tmp5A->f1=n,_tmp5A->f2=args,_tmp5A->f3=d,_tmp5A->f4=defn;_tmp5A;});}
# 453
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=0U,({void*_tmp1BA=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=20U,({union Cyc_Absyn_AggrInfo _tmp1B9=({enum Cyc_Absyn_AggrKind _tmp1B8=k;Cyc_Absyn_UnknownAggr(_tmp1B8,({struct _tuple0*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->f1=Cyc_Absyn_rel_ns_null,_tmp5B->f2=name;_tmp5B;}),0);});_tmp5C->f1=_tmp1B9;});_tmp5C;});_tmp5D->f1=_tmp1BA;}),_tmp5D->f2=0;_tmp5D;});}
# 456
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 459
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 462
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 466
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp168;(_tmp168.Char_c).tag=2U,((_tmp168.Char_c).val).f1=sn,((_tmp168.Char_c).val).f2=c;_tmp168;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp169;(_tmp169.Wchar_c).tag=3U,(_tmp169.Wchar_c).val=s;_tmp169;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp16A;(_tmp16A.Short_c).tag=4U,((_tmp16A.Short_c).val).f1=sn,((_tmp16A.Short_c).val).f2=s;_tmp16A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp16B;(_tmp16B.Int_c).tag=5U,((_tmp16B.Int_c).val).f1=sn,((_tmp16B.Int_c).val).f2=i;_tmp16B;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp16C;(_tmp16C.LongLong_c).tag=6U,((_tmp16C.LongLong_c).val).f1=sn,((_tmp16C.LongLong_c).val).f2=l;_tmp16C;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp16D;(_tmp16D.Float_c).tag=7U,((_tmp16D.Float_c).val).f1=s,((_tmp16D.Float_c).val).f2=i;_tmp16D;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp16E;(_tmp16E.String_c).tag=8U,(_tmp16E.String_c).val=s;_tmp16E;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp16F;(_tmp16F.Wstring_c).tag=9U,(_tmp16F.Wstring_c).val=s;_tmp16F;});}
# 476
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->topt=0,_tmp5E->r=r,_tmp5E->loc=loc,_tmp5E->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp5E;});}
# 479
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1BB=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=16U,_tmp5F->f1=rgn_handle,_tmp5F->f2=e;_tmp5F;});Cyc_Absyn_new_exp(_tmp1BB,loc);});}
# 482
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp60=_cycalloc(sizeof(*_tmp60));*_tmp60=*e;_tmp60;});}
# 485
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
return({void*_tmp1BC=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=0U,_tmp61->f1=c;_tmp61;});Cyc_Absyn_new_exp(_tmp1BC,loc);});}
# 488
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 492
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){return({union Cyc_Absyn_Cnst _tmp1BD=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp1BD,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 500
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 507
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){return({union Cyc_Absyn_Cnst _tmp1BE=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp1BE,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp1BF=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp1BF,loc);});}
# 514
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _dyneither_ptr),struct _dyneither_ptr s,unsigned loc){
return({union Cyc_Absyn_Cnst _tmp1C0=f(s);Cyc_Absyn_const_exp(_tmp1C0,loc);});}
# 517
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 521
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
return({void*_tmp1C2=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=1U,({void*_tmp1C1=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=0U,_tmp62->f1=q;_tmp62;});_tmp63->f1=_tmp1C1;});_tmp63;});Cyc_Absyn_new_exp(_tmp1C2,loc);});}
# 524
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
return({void*_tmp1C3=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=1U,_tmp64->f1=b;_tmp64;});Cyc_Absyn_new_exp(_tmp1C3,loc);});}
# 528
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 531
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned loc){
return({void*_tmp1C4=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=2U,_tmp65->f1=s;_tmp65;});Cyc_Absyn_new_exp(_tmp1C4,loc);});}
# 534
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1C5=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=3U,_tmp66->f1=p,_tmp66->f2=es;_tmp66;});Cyc_Absyn_new_exp(_tmp1C5,loc);});}
# 537
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1C7=p;struct Cyc_List_List*_tmp1C6=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=e,_tmp67->tl=0;_tmp67;});Cyc_Absyn_primop_exp(_tmp1C7,_tmp1C6,loc);});}
# 540
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({enum Cyc_Absyn_Primop _tmp1CA=p;struct Cyc_List_List*_tmp1C9=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->hd=e1,({struct Cyc_List_List*_tmp1C8=({struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=e2,_tmp68->tl=0;_tmp68;});_tmp69->tl=_tmp1C8;});_tmp69;});Cyc_Absyn_primop_exp(_tmp1CA,_tmp1C9,loc);});}
# 543
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1CB=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=35U,_tmp6A->f1=e1,_tmp6A->f2=e2;_tmp6A;});Cyc_Absyn_new_exp(_tmp1CB,loc);});}
# 546
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 556
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1CC=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=4U,_tmp6B->f1=e1,_tmp6B->f2=popt,_tmp6B->f3=e2;_tmp6B;});Cyc_Absyn_new_exp(_tmp1CC,loc);});}
# 559
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 562
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
return({void*_tmp1CD=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=5U,_tmp6C->f1=e,_tmp6C->f2=i;_tmp6C;});Cyc_Absyn_new_exp(_tmp1CD,loc);});}
# 565
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
return({void*_tmp1CE=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=6U,_tmp6D->f1=e1,_tmp6D->f2=e2,_tmp6D->f3=e3;_tmp6D;});Cyc_Absyn_new_exp(_tmp1CE,loc);});}
# 568
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1CF=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=7U,_tmp6E->f1=e1,_tmp6E->f2=e2;_tmp6E;});Cyc_Absyn_new_exp(_tmp1CF,loc);});}
# 571
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1D0=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=8U,_tmp6F->f1=e1,_tmp6F->f2=e2;_tmp6F;});Cyc_Absyn_new_exp(_tmp1D0,loc);});}
# 574
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1D1=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=9U,_tmp70->f1=e1,_tmp70->f2=e2;_tmp70;});Cyc_Absyn_new_exp(_tmp1D1,loc);});}
# 577
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1D2=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=10U,_tmp71->f1=e,_tmp71->f2=es,_tmp71->f3=0,_tmp71->f4=0;_tmp71;});Cyc_Absyn_new_exp(_tmp1D2,loc);});}
# 580
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1D3=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=10U,_tmp72->f1=e,_tmp72->f2=es,_tmp72->f3=0,_tmp72->f4=1;_tmp72;});Cyc_Absyn_new_exp(_tmp1D3,loc);});}
# 583
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1D4=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=12U,_tmp73->f1=e;_tmp73;});Cyc_Absyn_new_exp(_tmp1D4,loc);});}
# 586
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
return({void*_tmp1D5=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=13U,_tmp74->f1=e,_tmp74->f2=ts;_tmp74;});Cyc_Absyn_new_exp(_tmp1D5,loc);});}
# 589
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
return({void*_tmp1D6=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=14U,_tmp75->f1=t,_tmp75->f2=e,_tmp75->f3=user_cast,_tmp75->f4=c;_tmp75;});Cyc_Absyn_new_exp(_tmp1D6,loc);});}
# 592
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1D7=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=11U,_tmp76->f1=e,_tmp76->f2=0;_tmp76;});Cyc_Absyn_new_exp(_tmp1D7,loc);});}
# 595
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1D8=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=11U,_tmp77->f1=e,_tmp77->f2=1;_tmp77;});Cyc_Absyn_new_exp(_tmp1D8,loc);});}
# 598
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1D9=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=15U,_tmp78->f1=e;_tmp78;});Cyc_Absyn_new_exp(_tmp1D9,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
return({void*_tmp1DA=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=17U,_tmp79->f1=t;_tmp79;});Cyc_Absyn_new_exp(_tmp1DA,loc);});}
# 602
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1DB=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=18U,_tmp7A->f1=e;_tmp7A;});Cyc_Absyn_new_exp(_tmp1DB,loc);});}
# 605
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
return({void*_tmp1DC=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=19U,_tmp7B->f1=t,_tmp7B->f2=ofs;_tmp7B;});Cyc_Absyn_new_exp(_tmp1DC,loc);});}
# 608
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1DD=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=20U,_tmp7C->f1=e;_tmp7C;});Cyc_Absyn_new_exp(_tmp1DD,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned loc){
return({void*_tmp1DE=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=21U,_tmp7D->f1=e,_tmp7D->f2=n,_tmp7D->f3=0,_tmp7D->f4=0;_tmp7D;});Cyc_Absyn_new_exp(_tmp1DE,loc);});}
# 612
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned loc){
return({void*_tmp1DF=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=22U,_tmp7E->f1=e,_tmp7E->f2=n,_tmp7E->f3=0,_tmp7E->f4=0;_tmp7E;});Cyc_Absyn_new_exp(_tmp1DF,loc);});}
# 615
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({void*_tmp1E0=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=23U,_tmp7F->f1=e1,_tmp7F->f2=e2;_tmp7F;});Cyc_Absyn_new_exp(_tmp1E0,loc);});}
# 618
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
return({void*_tmp1E1=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=24U,_tmp80->f1=es;_tmp80;});Cyc_Absyn_new_exp(_tmp1E1,loc);});}
# 621
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1E2=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=37U,_tmp81->f1=s;_tmp81;});Cyc_Absyn_new_exp(_tmp1E2,loc);});}
# 624
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
return({void*_tmp1E3=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=39U,_tmp82->f1=t;_tmp82;});Cyc_Absyn_new_exp(_tmp1E3,loc);});}
# 627
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned loc){
return({void*_tmp1E4=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=40U,_tmp83->f1=volatile_kw,_tmp83->f2=body;_tmp83;});Cyc_Absyn_new_exp(_tmp1E4,loc);});}
# 630
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1E5=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=41U,_tmp84->f1=e;_tmp84;});Cyc_Absyn_new_exp(_tmp1E5,loc);});}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 634
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*_tmp85=0;
for(0;es != 0;es=es->tl){
_tmp85=({struct Cyc_List_List*_tmp87=_cycalloc(sizeof(*_tmp87));({struct _tuple11*_tmp1E6=({struct _tuple11*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->f1=0,_tmp86->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp86;});_tmp87->hd=_tmp1E6;}),_tmp87->tl=_tmp85;_tmp87;});}
_tmp85=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp85);
return({void*_tmp1E7=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=26U,_tmp88->f1=_tmp85;_tmp88;});Cyc_Absyn_new_exp(_tmp1E7,loc);});}
# 641
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 644
return({void*_tmp1E8=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=36U,_tmp89->f1=n,_tmp89->f2=dles;_tmp89;});Cyc_Absyn_new_exp(_tmp1E8,loc);});}
# 647
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
return({struct Cyc_Absyn_Stmt*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->r=s,_tmp8A->loc=loc,_tmp8A->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp8A;});}
# 650
static struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1E9=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=1U,_tmp8B->f1=e;_tmp8B;});Cyc_Absyn_new_stmt(_tmp1E9,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){return({void*_tmp1EA=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=3U,_tmp8C->f1=e;_tmp8C;});Cyc_Absyn_new_stmt(_tmp1EA,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp1EC=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp1EB=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp1EC,_tmp1EB,loc);});}}}struct _tuple12{void*f1;void*f2;};
# 663
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple12 _tmp8D=({struct _tuple12 _tmp170;_tmp170.f1=s1->r,_tmp170.f2=s2->r;_tmp170;});struct _tuple12 _tmp8E=_tmp8D;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp8E.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp8E.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp1ED=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=2U,_tmp8F->f1=s1,_tmp8F->f2=s2;_tmp8F;});Cyc_Absyn_new_stmt(_tmp1ED,loc);});}}_LL0:;}
# 670
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
return({void*_tmp1EE=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=4U,_tmp90->f1=e,_tmp90->f2=s1,_tmp90->f3=s2;_tmp90;});Cyc_Absyn_new_stmt(_tmp1EE,loc);});}
# 673
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1F0=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=5U,(_tmp91->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp1EF=Cyc_Absyn_skip_stmt(e->loc);(_tmp91->f1).f2=_tmp1EF;}),_tmp91->f2=s;_tmp91;});Cyc_Absyn_new_stmt(_tmp1F0,loc);});}
# 676
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1F3=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=9U,_tmp92->f1=e1,(_tmp92->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp1F2=Cyc_Absyn_skip_stmt(e3->loc);(_tmp92->f2).f2=_tmp1F2;}),
(_tmp92->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp1F1=Cyc_Absyn_skip_stmt(e3->loc);(_tmp92->f3).f2=_tmp1F1;}),_tmp92->f4=s;_tmp92;});
# 677
Cyc_Absyn_new_stmt(_tmp1F3,loc);});}
# 681
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp1F5=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=14U,_tmp93->f1=s,(_tmp93->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1F4=Cyc_Absyn_skip_stmt(e->loc);(_tmp93->f2).f2=_tmp1F4;});_tmp93;});Cyc_Absyn_new_stmt(_tmp1F5,loc);});}
# 684
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1F6=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->tag=10U,_tmp94->f1=e,_tmp94->f2=scs,_tmp94->f3=0;_tmp94;});Cyc_Absyn_new_stmt(_tmp1F6,loc);});}
# 687
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
return({void*_tmp1F7=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=15U,_tmp95->f1=s,_tmp95->f2=scs,_tmp95->f3=0;_tmp95;});Cyc_Absyn_new_stmt(_tmp1F7,loc);});}
# 690
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
return({void*_tmp1F8=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=11U,_tmp96->f1=el,_tmp96->f2=0;_tmp96;});Cyc_Absyn_new_stmt(_tmp1F8,loc);});}
# 693
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned loc){
return({void*_tmp1F9=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=8U,_tmp97->f1=lab;_tmp97;});Cyc_Absyn_new_stmt(_tmp1F9,loc);});}
# 696
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1FA=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=13U,_tmp98->f1=v,_tmp98->f2=s;_tmp98;});Cyc_Absyn_new_stmt(_tmp1FA,loc);});}
# 699
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
return({void*_tmp1FB=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=12U,_tmp99->f1=d,_tmp99->f2=s;_tmp99;});Cyc_Absyn_new_stmt(_tmp1FB,loc);});}
# 702
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_tmp1FD=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=0U,({struct Cyc_Absyn_Vardecl*_tmp1FC=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp9B->f1=_tmp1FC;});_tmp9B;});Cyc_Absyn_new_decl(_tmp1FD,loc);});
return({void*_tmp1FE=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=12U,_tmp9A->f1=d,_tmp9A->f2=s;_tmp9A;});Cyc_Absyn_new_stmt(_tmp1FE,loc);});}
# 706
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return({struct Cyc_Absyn_Exp*_tmp1FF=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1FF,loc);});}
# 710
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){return({struct Cyc_Absyn_Pat*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->r=p,_tmp9C->topt=0,_tmp9C->loc=s;_tmp9C;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp200=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=17U,_tmp9D->f1=e;_tmp9D;});Cyc_Absyn_new_pat(_tmp200,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 716
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){return({struct Cyc_Absyn_Decl*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->r=r,_tmp9E->loc=loc;_tmp9E;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
return({void*_tmp201=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=2U,_tmp9F->f1=p,_tmp9F->f2=0,_tmp9F->f3=e,_tmp9F->f4=0;_tmp9F;});Cyc_Absyn_new_decl(_tmp201,loc);});}
# 720
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
return({void*_tmp202=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=3U,_tmpA0->f1=vds;_tmpA0;});Cyc_Absyn_new_decl(_tmp202,loc);});}
# 723
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
return({void*_tmp203=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=4U,_tmpA1->f1=tv,_tmpA1->f2=vd,_tmpA1->f3=open_exp;_tmpA1;});Cyc_Absyn_new_decl(_tmp203,loc);});}
# 726
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 728
return({void*_tmp206=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->tag=2U,({struct Cyc_Absyn_Pat*_tmp205=({void*_tmp204=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=2U,_tmpA2->f1=tv,_tmpA2->f2=vd;_tmpA2;});Cyc_Absyn_new_pat(_tmp204,loc);});_tmpA3->f1=_tmp205;}),_tmpA3->f2=0,_tmpA3->f3=e,_tmpA3->f4=0;_tmpA3;});Cyc_Absyn_new_decl(_tmp206,loc);});}
# 731
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->sc=Cyc_Absyn_Public,_tmpA4->name=x,_tmpA4->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp207=Cyc_Absyn_empty_tqual(0U);_tmpA4->tq=_tmp207;}),_tmpA4->type=t,_tmpA4->initializer=init,_tmpA4->rgn=0,_tmpA4->attributes=0,_tmpA4->escapes=0;_tmpA4;});}
# 737
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->sc=Cyc_Absyn_Static,_tmpA5->name=x,_tmpA5->varloc=0U,({struct Cyc_Absyn_Tqual _tmp208=Cyc_Absyn_empty_tqual(0U);_tmpA5->tq=_tmp208;}),_tmpA5->type=t,_tmpA5->initializer=init,_tmpA5->rgn=0,_tmpA5->attributes=0,_tmpA5->escapes=0;_tmpA5;});}
# 742
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 746
return({struct Cyc_Absyn_AggrdeclImpl*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->exist_vars=exists,_tmpA6->rgn_po=po,_tmpA6->fields=fs,_tmpA6->tagged=tagged;_tmpA6;});}
# 749
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 752
return({void*_tmp20A=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp209=({struct Cyc_Absyn_Aggrdecl*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->kind=k,_tmpA7->sc=s,_tmpA7->name=n,_tmpA7->tvs=ts,_tmpA7->impl=i,_tmpA7->attributes=atts,_tmpA7->expected_mem_kind=0;_tmpA7;});_tmpA8->f1=_tmp209;});_tmpA8;});Cyc_Absyn_new_decl(_tmp20A,loc);});}
# 757
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 760
return({struct Cyc_Absyn_TypeDecl*_tmpAB=_cycalloc(sizeof(*_tmpAB));({void*_tmp20C=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp20B=({struct Cyc_Absyn_Aggrdecl*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->kind=k,_tmpA9->sc=s,_tmpA9->name=n,_tmpA9->tvs=ts,_tmpA9->impl=i,_tmpA9->attributes=atts,_tmpA9->expected_mem_kind=0;_tmpA9;});_tmpAA->f1=_tmp20B;});_tmpAA;});_tmpAB->r=_tmp20C;}),_tmpAB->loc=loc;_tmpAB;});}
# 766
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 769
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 771
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 774
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 776
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 779
return({void*_tmp20E=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp20D=({struct Cyc_Absyn_Datatypedecl*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->sc=s,_tmpAC->name=n,_tmpAC->tvs=ts,_tmpAC->fields=fs,_tmpAC->is_extensible=is_extensible;_tmpAC;});_tmpAD->f1=_tmp20D;});_tmpAD;});Cyc_Absyn_new_decl(_tmp20E,loc);});}
# 782
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 785
return({struct Cyc_Absyn_TypeDecl*_tmpB0=_cycalloc(sizeof(*_tmpB0));({void*_tmp210=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp20F=({struct Cyc_Absyn_Datatypedecl*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->sc=s,_tmpAE->name=n,_tmpAE->tvs=ts,_tmpAE->fields=fs,_tmpAE->is_extensible=is_extensible;_tmpAE;});_tmpAF->f1=_tmp20F;});_tmpAF;});_tmpB0->r=_tmp210;}),_tmpB0->loc=loc;_tmpB0;});}
# 797 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 803
{struct Cyc_List_List*_tmpB1=args;for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){
({void*_tmp211=Cyc_Absyn_pointer_expand((*((struct _tuple8*)_tmpB1->hd)).f3,1);(*((struct _tuple8*)_tmpB1->hd)).f3=_tmp211;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=5U,(_tmpB2->f1).tvars=tvs,(_tmpB2->f1).ret_tqual=ret_tqual,({
# 807
void*_tmp212=Cyc_Absyn_pointer_expand(ret_type,0);(_tmpB2->f1).ret_type=_tmp212;}),(_tmpB2->f1).effect=eff_type,(_tmpB2->f1).args=args,(_tmpB2->f1).c_varargs=c_varargs,(_tmpB2->f1).cyc_varargs=cyc_varargs,(_tmpB2->f1).rgn_po=rgn_po,(_tmpB2->f1).attributes=atts,(_tmpB2->f1).requires_clause=req,(_tmpB2->f1).requires_relns=0,(_tmpB2->f1).ensures_clause=ens,(_tmpB2->f1).ensures_relns=0;_tmpB2;});}
# 821
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmpB3=Cyc_Tcutil_compress(t);void*_tmpB4=_tmpB3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB4)->tag == 5U){_LL1: _LL2:
 return({void*_tmp215=t;void*_tmp214=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->v=& Cyc_Tcutil_rk;_tmpB5;}),0): Cyc_Absyn_heap_rgn_type;
# 823
struct Cyc_Absyn_Tqual _tmp213=
# 827
Cyc_Absyn_empty_tqual(0U);
# 823
Cyc_Absyn_at_type(_tmp215,_tmp214,_tmp213,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 828
 return t;}_LL0:;}
# 843 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpB6=e->r;void*_tmpB7=_tmpB6;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Vardecl*_tmpBB;struct Cyc_Absyn_Vardecl*_tmpBA;switch(*((int*)_tmpB7)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB7)->f1)){case 2U: _LL1: _LL2:
 return 0;case 1U: _LL3: _tmpBA=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB7)->f1)->f1;_LL4:
 _tmpBB=_tmpBA;goto _LL6;case 4U: _LL5: _tmpBB=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB7)->f1)->f1;_LL6: {
# 848
void*_tmpB8=Cyc_Tcutil_compress(_tmpBB->type);void*_tmpB9=_tmpB8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB9)->tag == 4U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 852
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpBC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL10:
 return Cyc_Absyn_is_lvalue(_tmpBC);case 13U: _LL11: _tmpBD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL12:
 return Cyc_Absyn_is_lvalue(_tmpBD);case 12U: _LL13: _tmpBE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL14:
 return Cyc_Absyn_is_lvalue(_tmpBE);default: _LL15: _LL16:
 return 0;}_LL0:;}
# 863
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmpBF=fields;for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmpBF->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmpBF->hd;}}
return 0;}
# 869
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 873
struct _tuple13*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 879
if(ts == 0)
return 0;
return(struct _tuple13*)ts->hd;}
# 884
struct _dyneither_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
void*_tmpC0=decl->r;void*_tmpC1=_tmpC0;struct Cyc_Absyn_Fndecl*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Typedefdecl*_tmpC4;struct Cyc_Absyn_Enumdecl*_tmpC3;struct Cyc_Absyn_Aggrdecl*_tmpC2;switch(*((int*)_tmpC1)){case 5U: _LL1: _tmpC2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC1)->f1;_LL2:
 return(*_tmpC2->name).f2;case 7U: _LL3: _tmpC3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC1)->f1;_LL4:
 return(*_tmpC3->name).f2;case 8U: _LL5: _tmpC4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC1)->f1;_LL6:
 return(*_tmpC4->name).f2;case 0U: _LL7: _tmpC5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC1)->f1;_LL8:
 return(*_tmpC5->name).f2;case 1U: _LL9: _tmpC6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpC1)->f1;_LLA:
 return(*_tmpC6->name).f2;case 13U: _LLB: _LLC:
 goto _LLE;case 14U: _LLD: _LLE:
 goto _LL10;case 15U: _LLF: _LL10:
 goto _LL12;case 16U: _LL11: _LL12:
 goto _LL14;case 2U: _LL13: _LL14:
 goto _LL16;case 6U: _LL15: _LL16:
 goto _LL18;case 3U: _LL17: _LL18:
 goto _LL1A;case 9U: _LL19: _LL1A:
 goto _LL1C;case 10U: _LL1B: _LL1C:
 goto _LL1E;case 11U: _LL1D: _LL1E:
 goto _LL20;case 12U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return 0;}_LL0:;}
# 907
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*name){
struct _dyneither_ptr*dname;
for(0;decls != 0;decls=decls->tl){
dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 914
return 0;}
# 917
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val={1U};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val={2U};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val={3U};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val={4U};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val={5U};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val={7U};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val={9U};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val={10U};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val={11U};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val={12U};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val={13U};
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val={14U};
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val={15U};
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val={16U};
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val={17U};
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val={18U};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val={23U};
struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val={26U};
# 936
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmpC7=a;struct _dyneither_ptr _tmp10A;struct _dyneither_ptr _tmp109;int _tmp108;int _tmp107;int _tmp106;int _tmp105;int _tmp104;int _tmp103;int _tmp102;struct _dyneither_ptr _tmp101;struct Cyc_Absyn_Exp*_tmp100;int _tmpFF;switch(*((int*)_tmpC7)){case 0U: _LL1: _tmpFF=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpCA=({struct Cyc_Int_pa_PrintArg_struct _tmp171;_tmp171.tag=1U,_tmp171.f1=(unsigned long)_tmpFF;_tmp171;});void*_tmpC8[1U];_tmpC8[0]=& _tmpCA;({struct _dyneither_ptr _tmp216=({const char*_tmpC9="regparm(%d)";_tag_dyneither(_tmpC9,sizeof(char),12U);});Cyc_aprintf(_tmp216,_tag_dyneither(_tmpC8,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return({const char*_tmpCB="stdcall";_tag_dyneither(_tmpCB,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpCC="cdecl";_tag_dyneither(_tmpCC,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpCD="fastcall";_tag_dyneither(_tmpCD,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpCE="noreturn";_tag_dyneither(_tmpCE,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpCF="const";_tag_dyneither(_tmpCF,sizeof(char),6U);});case 6U: _LLD: _tmp100=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LLE:
# 945
 if(_tmp100 == 0)return({const char*_tmpD0="aligned";_tag_dyneither(_tmpD0,sizeof(char),8U);});else{
# 947
enum Cyc_Cyclone_C_Compilers _tmpD1=Cyc_Cyclone_c_compiler;switch(_tmpD1){case Cyc_Cyclone_Gcc_c: _LL3A: _LL3B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD4=({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0U,({struct _dyneither_ptr _tmp217=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp100));_tmp172.f1=_tmp217;});_tmp172;});void*_tmpD2[1U];_tmpD2[0]=& _tmpD4;({struct _dyneither_ptr _tmp218=({const char*_tmpD3="aligned(%s)";_tag_dyneither(_tmpD3,sizeof(char),12U);});Cyc_aprintf(_tmp218,_tag_dyneither(_tmpD2,sizeof(void*),1U));});});case Cyc_Cyclone_Vc_c: _LL3C: _LL3D:
 goto _LL3F;default: _LL3E: _LL3F:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD7=({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0U,({struct _dyneither_ptr _tmp219=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp100));_tmp173.f1=_tmp219;});_tmp173;});void*_tmpD5[1U];_tmpD5[0]=& _tmpD7;({struct _dyneither_ptr _tmp21A=({const char*_tmpD6="align(%s)";_tag_dyneither(_tmpD6,sizeof(char),10U);});Cyc_aprintf(_tmp21A,_tag_dyneither(_tmpD5,sizeof(void*),1U));});});}_LL39:;}case 7U: _LLF: _LL10:
# 952
 return({const char*_tmpD8="packed";_tag_dyneither(_tmpD8,sizeof(char),7U);});case 8U: _LL11: _tmp101=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LL12:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp174;_tmp174.tag=0U,_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp101);_tmp174;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({struct _dyneither_ptr _tmp21B=({const char*_tmpDA="section(\"%s\")";_tag_dyneither(_tmpDA,sizeof(char),14U);});Cyc_aprintf(_tmp21B,_tag_dyneither(_tmpD9,sizeof(void*),1U));});});case 9U: _LL13: _LL14:
 return({const char*_tmpDC="nocommon";_tag_dyneither(_tmpDC,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpDD="shared";_tag_dyneither(_tmpDD,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpDE="unused";_tag_dyneither(_tmpDE,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpDF="weak";_tag_dyneither(_tmpDF,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpE0="dllimport";_tag_dyneither(_tmpE0,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpE1="dllexport";_tag_dyneither(_tmpE1,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpE2="no_instrument_function";_tag_dyneither(_tmpE2,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpE3="constructor";_tag_dyneither(_tmpE3,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpE4="destructor";_tag_dyneither(_tmpE4,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpE5="no_check_memory_usage";_tag_dyneither(_tmpE5,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC7)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmp103=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC7)->f2;_tmp102=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC7)->f3;_LL28:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE8=({struct Cyc_Int_pa_PrintArg_struct _tmp176;_tmp176.tag=1U,_tmp176.f1=(unsigned)_tmp103;_tmp176;});struct Cyc_Int_pa_PrintArg_struct _tmpE9=({struct Cyc_Int_pa_PrintArg_struct _tmp175;_tmp175.tag=1U,_tmp175.f1=(unsigned)_tmp102;_tmp175;});void*_tmpE6[2U];_tmpE6[0]=& _tmpE8,_tmpE6[1]=& _tmpE9;({struct _dyneither_ptr _tmp21C=({const char*_tmpE7="format(printf,%u,%u)";_tag_dyneither(_tmpE7,sizeof(char),21U);});Cyc_aprintf(_tmp21C,_tag_dyneither(_tmpE6,sizeof(void*),2U));});});}else{_LL29: _tmp105=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC7)->f2;_tmp104=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC7)->f3;_LL2A:
# 966
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpEC=({struct Cyc_Int_pa_PrintArg_struct _tmp178;_tmp178.tag=1U,_tmp178.f1=(unsigned)_tmp105;_tmp178;});struct Cyc_Int_pa_PrintArg_struct _tmpED=({struct Cyc_Int_pa_PrintArg_struct _tmp177;_tmp177.tag=1U,_tmp177.f1=(unsigned)_tmp104;_tmp177;});void*_tmpEA[2U];_tmpEA[0]=& _tmpEC,_tmpEA[1]=& _tmpED;({struct _dyneither_ptr _tmp21D=({const char*_tmpEB="format(scanf,%u,%u)";_tag_dyneither(_tmpEB,sizeof(char),20U);});Cyc_aprintf(_tmp21D,_tag_dyneither(_tmpEA,sizeof(void*),2U));});});}case 20U: _LL2B: _tmp106=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LL2C:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF0=({struct Cyc_Int_pa_PrintArg_struct _tmp179;_tmp179.tag=1U,_tmp179.f1=(unsigned long)_tmp106;_tmp179;});void*_tmpEE[1U];_tmpEE[0]=& _tmpF0;({struct _dyneither_ptr _tmp21E=({const char*_tmpEF="initializes(%d)";_tag_dyneither(_tmpEF,sizeof(char),16U);});Cyc_aprintf(_tmp21E,_tag_dyneither(_tmpEE,sizeof(void*),1U));});});case 21U: _LL2D: _tmp107=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LL2E:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF3=({struct Cyc_Int_pa_PrintArg_struct _tmp17A;_tmp17A.tag=1U,_tmp17A.f1=(unsigned long)_tmp107;_tmp17A;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({struct _dyneither_ptr _tmp21F=({const char*_tmpF2="noliveunique(%d)";_tag_dyneither(_tmpF2,sizeof(char),17U);});Cyc_aprintf(_tmp21F,_tag_dyneither(_tmpF1,sizeof(void*),1U));});});case 22U: _LL2F: _tmp108=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LL30:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF6=({struct Cyc_Int_pa_PrintArg_struct _tmp17B;_tmp17B.tag=1U,_tmp17B.f1=(unsigned long)_tmp108;_tmp17B;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp220=({const char*_tmpF5="consume(%d)";_tag_dyneither(_tmpF5,sizeof(char),12U);});Cyc_aprintf(_tmp220,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});case 23U: _LL31: _LL32:
 return({const char*_tmpF7="pure";_tag_dyneither(_tmpF7,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmpF8="always_inline";_tag_dyneither(_tmpF8,sizeof(char),14U);});case 24U: _LL35: _tmp109=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LL36:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpFB=({struct Cyc_String_pa_PrintArg_struct _tmp17C;_tmp17C.tag=0U,_tmp17C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp109);_tmp17C;});void*_tmpF9[1U];_tmpF9[0]=& _tmpFB;({struct _dyneither_ptr _tmp221=({const char*_tmpFA="__mode__(\"%s\")";_tag_dyneither(_tmpFA,sizeof(char),15U);});Cyc_aprintf(_tmp221,_tag_dyneither(_tmpF9,sizeof(void*),1U));});});default: _LL37: _tmp10A=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpC7)->f1;_LL38:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpFE=({struct Cyc_String_pa_PrintArg_struct _tmp17D;_tmp17D.tag=0U,_tmp17D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10A);_tmp17D;});void*_tmpFC[1U];_tmpFC[0]=& _tmpFE;({struct _dyneither_ptr _tmp222=({const char*_tmpFD="alias(\"%s\")";_tag_dyneither(_tmpFD,sizeof(char),12U);});Cyc_aprintf(_tmp222,_tag_dyneither(_tmpFC,sizeof(void*),1U));});});}_LL0:;}
# 977
static int Cyc_Absyn_attribute_case_number(void*att){
void*_tmp10B=att;switch(*((int*)_tmp10B)){case 0U: _LL1: _LL2:
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
# 1003
int Cyc_Absyn_attribute_cmp(void*att1,void*att2){
struct _tuple12 _tmp10C=({struct _tuple12 _tmp17E;_tmp17E.f1=att1,_tmp17E.f2=att2;_tmp17E;});struct _tuple12 _tmp10D=_tmp10C;enum Cyc_Absyn_Format_Type _tmp11D;int _tmp11C;int _tmp11B;enum Cyc_Absyn_Format_Type _tmp11A;int _tmp119;int _tmp118;struct _dyneither_ptr _tmp117;struct _dyneither_ptr _tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;int _tmp113;int _tmp112;int _tmp111;int _tmp110;switch(*((int*)_tmp10D.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10D.f2)->tag == 0U){_LL1: _tmp111=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10D.f1)->f1;_tmp110=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10D.f2)->f1;_LL2:
 _tmp113=_tmp111;_tmp112=_tmp110;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10D.f2)->tag == 20U){_LL3: _tmp113=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10D.f1)->f1;_tmp112=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10D.f2)->f1;_LL4:
# 1007
 return Cyc_Core_intcmp(_tmp113,_tmp112);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10D.f2)->tag == 6U){_LL5: _tmp115=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10D.f1)->f1;_tmp114=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10D.f2)->f1;_LL6:
# 1009
 if(_tmp115 == _tmp114)return 0;
if(_tmp115 == 0)return - 1;
if(_tmp114 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp115,_tmp114);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10D.f2)->tag == 8U){_LL7: _tmp117=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10D.f1)->f1;_tmp116=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10D.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp117,(struct _dyneither_ptr)_tmp116);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10D.f2)->tag == 19U){_LL9: _tmp11D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10D.f1)->f1;_tmp11C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10D.f1)->f2;_tmp11B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10D.f1)->f3;_tmp11A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10D.f2)->f1;_tmp119=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10D.f2)->f2;_tmp118=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10D.f2)->f3;_LLA: {
# 1015
int _tmp10E=Cyc_Core_intcmp((int)((unsigned)_tmp11D),(int)((unsigned)_tmp11A));
if(_tmp10E != 0)return _tmp10E;{
int _tmp10F=Cyc_Core_intcmp(_tmp11C,_tmp119);
if(_tmp10F != 0)return _tmp10F;
return Cyc_Core_intcmp(_tmp11B,_tmp118);};}}else{goto _LLB;}default: _LLB: _LLC:
# 1021
 return({int _tmp223=Cyc_Absyn_attribute_case_number(att1);Cyc_Core_intcmp(_tmp223,Cyc_Absyn_attribute_case_number(att2));});}_LL0:;}
# 1024
int Cyc_Absyn_equal_att(void*a1,void*a2){
return Cyc_Absyn_attribute_cmp(a1,a2)== 0;}
# 1029
int Cyc_Absyn_fntype_att(void*a){
void*_tmp11E=a;int _tmp11F;switch(*((int*)_tmp11E)){case 0U: _LL1: _tmp11F=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp11E)->f1;_LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 1U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 23U: _LLB: _LLC:
 goto _LLE;case 19U: _LLD: _LLE:
 goto _LL10;case 5U: _LLF: _LL10:
 goto _LL12;case 21U: _LL11: _LL12:
 goto _LL14;case 20U: _LL13: _LL14:
 goto _LL16;case 22U: _LL15: _LL16:
 return 1;default: _LL17: _LL18:
 return 0;}_LL0:;}static char _tmp126[3U]="f0";
# 1046
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
# 1048
static struct _dyneither_ptr f0={_tmp126,_tmp126,_tmp126 + 3U};
static struct _dyneither_ptr*field_names_v[1U]={& f0};
static struct _dyneither_ptr field_names={(void*)((struct _dyneither_ptr**)field_names_v),(void*)((struct _dyneither_ptr**)field_names_v),(void*)((struct _dyneither_ptr**)field_names_v + 1U)};
unsigned fsz=_get_dyneither_size(field_names,sizeof(struct _dyneither_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _tmp125=(unsigned)(i + 1);struct _dyneither_ptr**_tmp124=_cycalloc(_check_times(_tmp125,sizeof(struct _dyneither_ptr*)));({{unsigned _tmp17F=(unsigned)(i + 1);unsigned j;for(j=0;j < _tmp17F;++ j){
# 1055
j < fsz?_tmp124[j]=*((struct _dyneither_ptr**)_check_dyneither_subscript(field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp226=({struct _dyneither_ptr*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _dyneither_ptr _tmp225=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp122=({struct Cyc_Int_pa_PrintArg_struct _tmp180;_tmp180.tag=1U,_tmp180.f1=(unsigned long)((int)j);_tmp180;});void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct _dyneither_ptr _tmp224=({const char*_tmp121="f%d";_tag_dyneither(_tmp121,sizeof(char),4U);});Cyc_aprintf(_tmp224,_tag_dyneither(_tmp120,sizeof(void*),1U));});});*_tmp123=_tmp225;});_tmp123;});_tmp124[j]=_tmp226;});}}0;});_tag_dyneither(_tmp124,sizeof(struct _dyneither_ptr*),_tmp125);});
return*((struct _dyneither_ptr**)_check_dyneither_subscript(field_names,sizeof(struct _dyneither_ptr*),i));}struct _tuple14{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1059
struct _tuple14 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmp127=info;enum Cyc_Absyn_AggrKind _tmp12B;struct _tuple0*_tmp12A;enum Cyc_Absyn_AggrKind _tmp129;struct _tuple0*_tmp128;if((_tmp127.UnknownAggr).tag == 1){_LL1: _tmp129=((_tmp127.UnknownAggr).val).f1;_tmp128=((_tmp127.UnknownAggr).val).f2;_LL2:
 return({struct _tuple14 _tmp181;_tmp181.f1=_tmp129,_tmp181.f2=_tmp128;_tmp181;});}else{_LL3: _tmp12B=(*(_tmp127.KnownAggr).val)->kind;_tmp12A=(*(_tmp127.KnownAggr).val)->name;_LL4:
 return({struct _tuple14 _tmp182;_tmp182.f1=_tmp12B,_tmp182.f2=_tmp12A;_tmp182;});}_LL0:;}
# 1065
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmp12C=info;struct Cyc_Absyn_Aggrdecl*_tmp12F;if((_tmp12C.UnknownAggr).tag == 1){_LL1: _LL2:
({void*_tmp12D=0U;({struct _dyneither_ptr _tmp227=({const char*_tmp12E="unchecked aggrdecl";_tag_dyneither(_tmp12E,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp227,_tag_dyneither(_tmp12D,sizeof(void*),0U));});});}else{_LL3: _tmp12F=*(_tmp12C.KnownAggr).val;_LL4:
 return _tmp12F;}_LL0:;}
# 1071
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp130=Cyc_Tcutil_compress(t);void*_tmp131=_tmp130;union Cyc_Absyn_AggrInfo _tmp13A;struct Cyc_List_List*_tmp139;switch(*((int*)_tmp131)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp131)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp139=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp131)->f2;_LL2:
# 1074
 if(_tmp139 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp139->hd)->requires_clause == 0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->tag == 20U){_LL3: _tmp13A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1)->f1;_LL4: {
# 1077
union Cyc_Absyn_AggrInfo _tmp132=_tmp13A;enum Cyc_Absyn_AggrKind _tmp138;int _tmp137;enum Cyc_Absyn_AggrKind _tmp136;struct Cyc_Absyn_Aggrdecl*_tmp135;if((_tmp132.KnownAggr).tag == 2){_LL8: _tmp135=*(_tmp132.KnownAggr).val;_LL9:
# 1079
 if((int)_tmp135->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp133=_tmp135->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp133))->tagged)return 0;{
struct Cyc_List_List*_tmp134=_tmp133->fields;
if(_tmp134 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp134->hd)->requires_clause == 0;};};}else{if(((_tmp132.UnknownAggr).val).f3 == 0){_LLA: _tmp136=((_tmp132.UnknownAggr).val).f1;_LLB:
 return(int)_tmp136 == (int)Cyc_Absyn_UnionA;}else{_LLC: _tmp138=((_tmp132.UnknownAggr).val).f1;_tmp137=(int)(((_tmp132.UnknownAggr).val).f3)->v;_LLD:
 return(int)_tmp138 == (int)1U && !_tmp137;}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1088
 return 0;}_LL0:;}
# 1091
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp13B=Cyc_Tcutil_compress(t);void*_tmp13C=_tmp13B;union Cyc_Absyn_AggrInfo _tmp145;struct Cyc_List_List*_tmp144;switch(*((int*)_tmp13C)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13C)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp144=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp13C)->f2;_LL2:
# 1094
 if(_tmp144 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp144->hd)->requires_clause != 0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13C)->f1)->tag == 20U){_LL3: _tmp145=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13C)->f1)->f1;_LL4: {
# 1097
union Cyc_Absyn_AggrInfo _tmp13D=_tmp145;enum Cyc_Absyn_AggrKind _tmp143;int _tmp142;enum Cyc_Absyn_AggrKind _tmp141;struct Cyc_Absyn_Aggrdecl*_tmp140;if((_tmp13D.KnownAggr).tag == 2){_LL8: _tmp140=*(_tmp13D.KnownAggr).val;_LL9:
# 1099
 if((int)_tmp140->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp13E=_tmp140->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp13E))->tagged)return 0;{
struct Cyc_List_List*_tmp13F=_tmp13E->fields;
if(_tmp13F == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp13F->hd)->requires_clause != 0;};};}else{if(((_tmp13D.UnknownAggr).val).f3 == 0){_LLA: _tmp141=((_tmp13D.UnknownAggr).val).f1;_LLB:
 return 0;}else{_LLC: _tmp143=((_tmp13D.UnknownAggr).val).f1;_tmp142=(int)(((_tmp13D.UnknownAggr).val).f3)->v;_LLD:
 return 0;}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1108
 return 0;}_LL0:;}
# 1112
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp146=b;struct Cyc_Absyn_Fndecl*_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14B;struct Cyc_Absyn_Vardecl*_tmp14A;struct Cyc_Absyn_Vardecl*_tmp149;struct Cyc_Absyn_Vardecl*_tmp148;struct _tuple0*_tmp147;switch(*((int*)_tmp146)){case 0U: _LL1: _tmp147=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp146)->f1;_LL2:
 return _tmp147;case 1U: _LL3: _tmp148=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp146)->f1;_LL4:
 _tmp149=_tmp148;goto _LL6;case 3U: _LL5: _tmp149=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp146)->f1;_LL6:
 _tmp14A=_tmp149;goto _LL8;case 4U: _LL7: _tmp14A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp146)->f1;_LL8:
 _tmp14B=_tmp14A;goto _LLA;case 5U: _LL9: _tmp14B=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp146)->f1;_LLA:
 return _tmp14B->name;default: _LLB: _tmp14C=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp146)->f1;_LLC:
 return _tmp14C->name;}_LL0:;}
# 1123
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0 || ds->tl != 0)
({void*_tmp14D=0U;({struct _dyneither_ptr _tmp228=({const char*_tmp14E="designator list not of length 1";_tag_dyneither(_tmp14E,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp228,_tag_dyneither(_tmp14D,sizeof(void*),0U));});});{
void*_tmp14F=(void*)ds->hd;void*_tmp150=_tmp14F;struct _dyneither_ptr*_tmp153;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp150)->tag == 1U){_LL1: _tmp153=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp150)->f1;_LL2:
 return _tmp153;}else{_LL3: _LL4:
({void*_tmp151=0U;({struct _dyneither_ptr _tmp229=({const char*_tmp152="array designator in struct";_tag_dyneither(_tmp152,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp229,_tag_dyneither(_tmp151,sizeof(void*),0U));});});}_LL0:;};}
# 1132
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp154=Cyc_Tcutil_compress(t);void*_tmp155=_tmp154;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f1)){case 11U: _LL1: _LL2:
 return 1;case 12U: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1140
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15U};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16U};
# 1145
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;
