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
   (((char *)ptr) + (elt_sz)*(index))
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char *)ptr) + (elt_sz)*(index))

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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 86 "list.h"
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 931 "absyn.h"
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 956
extern void*Cyc_Absyn_fat_bound_type;
# 960
void*Cyc_Absyn_bounds_one();struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RType(void*);union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple10{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 64
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 68
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 95
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns);
# 100
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 103
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 106
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 108
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 112
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 114
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 116
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 120
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r);
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr,struct _dyneither_ptr);
# 33
void Cyc_Tcutil_terr(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 35
void Cyc_Tcutil_warn(unsigned,struct _dyneither_ptr,struct _dyneither_ptr);
# 44
int Cyc_Tcutil_is_integral_type(void*);
# 76
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 89
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 112
void*Cyc_Tcutil_compress(void*t);
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 178
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 194
int Cyc_Tcutil_unify(void*,void*);
# 313 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 346
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 8 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
struct Cyc_AP_T*Cyc_AP_fromint(long x);
# 14
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x);
# 16
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned val;};struct _union_Node_NParamNumelts{int tag;unsigned val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 61 "pratt-ap.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
# 63
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NType(void*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned);
union Cyc_Pratt_Node Cyc_Pratt_NReturn();struct Cyc_Pratt_Graph;
# 76
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph();
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*);
# 81
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,struct Cyc_AP_T*a);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 64 "string.h"
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 32 "relations-ap.cyc"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned i){
return({union Cyc_Relations_RelnOp _tmpE1;(_tmpE1.RConst).tag=1U,(_tmpE1.RConst).val=i;_tmpE1;});}
# 35
union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*v){
return({union Cyc_Relations_RelnOp _tmpE2;(_tmpE2.RVar).tag=2U,(_tmpE2.RVar).val=v;_tmpE2;});}
# 38
union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*v){
return({union Cyc_Relations_RelnOp _tmpE3;(_tmpE3.RNumelts).tag=3U,(_tmpE3.RNumelts).val=v;_tmpE3;});}
# 41
union Cyc_Relations_RelnOp Cyc_Relations_RType(void*t){
return({union Cyc_Relations_RelnOp _tmpE4;(_tmpE4.RType).tag=4U,({void*_tmpF9=Cyc_Tcutil_compress(t);(_tmpE4.RType).val=_tmpF9;});_tmpE4;});}
# 44
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned i){
return({union Cyc_Relations_RelnOp _tmpE5;(_tmpE5.RParam).tag=5U,(_tmpE5.RParam).val=i;_tmpE5;});}
# 47
union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned i){
return({union Cyc_Relations_RelnOp _tmpE6;(_tmpE6.RParamNumelts).tag=6U,(_tmpE6.RParamNumelts).val=i;_tmpE6;});}
# 50
union Cyc_Relations_RelnOp Cyc_Relations_RReturn(){
return({union Cyc_Relations_RelnOp _tmpE7;(_tmpE7.RReturn).tag=7U,(_tmpE7.RReturn).val=0U;_tmpE7;});}struct _tuple12{union Cyc_Relations_RelnOp f1;union Cyc_Relations_RelnOp f2;};
# 54
static int Cyc_Relations_same_relop(union Cyc_Relations_RelnOp r1,union Cyc_Relations_RelnOp r2){
struct _tuple12 _tmp0=({struct _tuple12 _tmpE8;_tmpE8.f1=r1,_tmpE8.f2=r2;_tmpE8;});struct _tuple12 _tmp1=_tmp0;unsigned _tmpD;unsigned _tmpC;unsigned _tmpB;unsigned _tmpA;void*_tmp9;void*_tmp8;struct Cyc_Absyn_Vardecl*_tmp7;struct Cyc_Absyn_Vardecl*_tmp6;struct Cyc_Absyn_Vardecl*_tmp5;struct Cyc_Absyn_Vardecl*_tmp4;unsigned _tmp3;unsigned _tmp2;switch(((_tmp1.f1).RParamNumelts).tag){case 1U: if(((_tmp1.f2).RConst).tag == 1){_LL1: _tmp3=((_tmp1.f1).RConst).val;_tmp2=((_tmp1.f2).RConst).val;_LL2:
 return _tmp3 == _tmp2;}else{goto _LLF;}case 2U: if(((_tmp1.f2).RVar).tag == 2){_LL3: _tmp5=((_tmp1.f1).RVar).val;_tmp4=((_tmp1.f2).RVar).val;_LL4:
 return _tmp5 == _tmp4;}else{goto _LLF;}case 3U: if(((_tmp1.f2).RNumelts).tag == 3){_LL5: _tmp7=((_tmp1.f1).RNumelts).val;_tmp6=((_tmp1.f2).RNumelts).val;_LL6:
 return _tmp7 == _tmp6;}else{goto _LLF;}case 4U: if(((_tmp1.f2).RType).tag == 4){_LL7: _tmp9=((_tmp1.f1).RType).val;_tmp8=((_tmp1.f2).RType).val;_LL8:
 return Cyc_Tcutil_unify(_tmp9,_tmp8);}else{goto _LLF;}case 5U: if(((_tmp1.f2).RParam).tag == 5){_LL9: _tmpB=((_tmp1.f1).RParam).val;_tmpA=((_tmp1.f2).RParam).val;_LLA:
 return _tmpB == _tmpA;}else{goto _LLF;}case 6U: if(((_tmp1.f2).RParamNumelts).tag == 6){_LLB: _tmpD=((_tmp1.f1).RParamNumelts).val;_tmpC=((_tmp1.f2).RParamNumelts).val;_LLC:
 return _tmpD == _tmpC;}else{goto _LLF;}default: if(((_tmp1.f2).RReturn).tag == 7){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
 return 0;}}_LL0:;}
# 67
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation relation,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns){
# 71
{struct Cyc_List_List*_tmpE=relns;for(0;_tmpE != 0;_tmpE=_tmpE->tl){
struct Cyc_Relations_Reln*_tmpF=(struct Cyc_Relations_Reln*)_tmpE->hd;
if((Cyc_Relations_same_relop(_tmpF->rop1,rop1)&&(int)_tmpF->relation == (int)relation)&&
 Cyc_Relations_same_relop(_tmpF->rop2,rop2))return relns;}}
# 76
return({struct Cyc_List_List*_tmp11=_region_malloc(rgn,sizeof(*_tmp11));({struct Cyc_Relations_Reln*_tmpFA=({struct Cyc_Relations_Reln*_tmp10=_region_malloc(rgn,sizeof(*_tmp10));_tmp10->rop1=rop1,_tmp10->relation=relation,_tmp10->rop2=rop2;_tmp10;});_tmp11->hd=_tmpFA;}),_tmp11->tl=relns;_tmp11;});}
# 79
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns){
# 82
union Cyc_Relations_RelnOp rop_old=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop_new=Cyc_Relations_RConst(0U);
if(!Cyc_Relations_exp2relnop(e_old,& rop_old))return relns;
if(!Cyc_Relations_exp2relnop(e_new,& rop_new)){
({void*_tmp12=0U;({unsigned _tmpFC=e_new->loc;struct _dyneither_ptr _tmpFB=({const char*_tmp13="Could not construct relation";_tag_dyneither(_tmp13,sizeof(char),29U);});Cyc_Tcutil_warn(_tmpFC,_tmpFB,_tag_dyneither(_tmp12,sizeof(void*),0U));});});
return relns;}
# 89
{struct Cyc_List_List*_tmp14=relns;for(0;_tmp14 != 0;_tmp14=_tmp14->tl){
struct Cyc_Relations_Reln*_tmp15=(struct Cyc_Relations_Reln*)_tmp14->hd;
union Cyc_Relations_RelnOp _tmp16=_tmp15->rop1;
union Cyc_Relations_RelnOp _tmp17=_tmp15->rop2;
int addIt=0;
if(Cyc_Relations_same_relop(_tmp16,rop_old)){
addIt=1;
_tmp16=rop_new;}
# 98
if(Cyc_Relations_same_relop(_tmp17,rop_old)){
addIt=1;
_tmp17=rop_new;}
# 102
if(addIt)
relns=({struct Cyc_List_List*_tmp19=_region_malloc(rgn,sizeof(*_tmp19));({struct Cyc_Relations_Reln*_tmpFD=({struct Cyc_Relations_Reln*_tmp18=_region_malloc(rgn,sizeof(*_tmp18));_tmp18->rop1=_tmp16,_tmp18->relation=_tmp15->relation,_tmp18->rop2=_tmp17;_tmp18;});_tmp19->hd=_tmpFD;}),_tmp19->tl=relns;_tmp19;});}}
# 105
return relns;}
# 108
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 112
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_Relations_Reln*_tmp1A=(struct Cyc_Relations_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp1B=r2s;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){
struct Cyc_Relations_Reln*_tmp1C=(struct Cyc_Relations_Reln*)_tmp1B->hd;
if(_tmp1A == _tmp1C ||(Cyc_Relations_same_relop(_tmp1A->rop1,_tmp1C->rop1)&&(int)_tmp1A->relation == (int)_tmp1C->relation)&&
# 119
 Cyc_Relations_same_relop(_tmp1A->rop2,_tmp1C->rop2)){
found=1;
break;}}}
# 124
if(!found)
return 0;}
# 127
return 1;}
# 130
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp1D=r1s;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
struct Cyc_Relations_Reln*_tmp1E=(struct Cyc_Relations_Reln*)_tmp1D->hd;
int found=0;
{struct Cyc_List_List*_tmp1F=r2s;for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){
struct Cyc_Relations_Reln*_tmp20=(struct Cyc_Relations_Reln*)_tmp1F->hd;
if(_tmp1E == _tmp20 ||(Cyc_Relations_same_relop(_tmp1E->rop1,_tmp20->rop1)&&(int)_tmp1E->relation == (int)_tmp20->relation)&&
# 141
 Cyc_Relations_same_relop(_tmp1E->rop2,_tmp20->rop2)){
res=({struct Cyc_List_List*_tmp21=_region_malloc(r,sizeof(*_tmp21));_tmp21->hd=_tmp1E,_tmp21->tl=res;_tmp21;});
found=1;
break;}}}
# 147
if(!found)diff=1;}}
# 149
if(!diff)res=r1s;
return res;};}
# 153
static int Cyc_Relations_rop_contains_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmp22=r;struct Cyc_Absyn_Vardecl*_tmp24;struct Cyc_Absyn_Vardecl*_tmp23;switch((_tmp22.RReturn).tag){case 2U: _LL1: _tmp23=(_tmp22.RVar).val;_LL2:
 return v == _tmp23;case 3U: _LL3: _tmp24=(_tmp22.RNumelts).val;_LL4:
 return v == _tmp24;case 4U: _LL5: _LL6:
 goto _LL8;case 5U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
 return 0;}_LL0:;}
# 165
union Cyc_Relations_RelnOp Cyc_Relations_subst_rop_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
union Cyc_Relations_RelnOp _tmp25=r;struct Cyc_Absyn_Vardecl*_tmp27;struct Cyc_Absyn_Vardecl*_tmp26;switch((_tmp25.RReturn).tag){case 2U: _LL1: _tmp26=(_tmp25.RVar).val;_LL2:
 return v == _tmp26?new_rop: r;case 3U: _LL3: _tmp27=(_tmp25.RNumelts).val;_LL4:
 return v == _tmp27?new_rop: r;case 4U: _LL5: _LL6:
 goto _LL8;case 5U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
 return r;}_LL0:;}
# 177
struct Cyc_Relations_Reln*Cyc_Relations_substitute_rop_var(struct _RegionHandle*rgn,struct Cyc_Relations_Reln*reln,struct Cyc_Absyn_Vardecl*v,union Cyc_Relations_RelnOp new_rop){
# 179
return({struct Cyc_Relations_Reln*_tmp28=_region_malloc(rgn,sizeof(*_tmp28));({union Cyc_Relations_RelnOp _tmpFF=Cyc_Relations_subst_rop_var(reln->rop1,v,new_rop);_tmp28->rop1=_tmpFF;}),_tmp28->relation=reln->relation,({
union Cyc_Relations_RelnOp _tmpFE=Cyc_Relations_subst_rop_var(reln->rop2,v,new_rop);_tmp28->rop2=_tmpFE;});_tmp28;});}
# 182
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found && p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp29=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp29->rop1,v)|| Cyc_Relations_rop_contains_var(_tmp29->rop2,v)){
found=1;
break;}}
# 192
if(!found)return rs;{
# 194
struct Cyc_Absyn_Tvar*_tmp2A=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
union Cyc_Relations_RelnOp _tmp2B=Cyc_Relations_RType(Cyc_Absyn_var_type(_tmp2A));
struct Cyc_List_List*_tmp2C=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp2D=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp2D->rop1,v)|| Cyc_Relations_rop_contains_var(_tmp2D->rop2,v))
_tmp2C=({struct Cyc_List_List*_tmp2E=_region_malloc(rgn,sizeof(*_tmp2E));({struct Cyc_Relations_Reln*_tmp100=Cyc_Relations_substitute_rop_var(rgn,_tmp2D,v,_tmp2B);_tmp2E->hd=_tmp100;}),_tmp2E->tl=_tmp2C;_tmp2E;});else{
# 202
_tmp2C=({struct Cyc_List_List*_tmp2F=_region_malloc(rgn,sizeof(*_tmp2F));_tmp2F->hd=_tmp2D,_tmp2F->tl=_tmp2C;_tmp2F;});}}
# 205
return _tmp2C;};}
# 211
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p){
# 214
RETRY:
 if(e->topt != 0){
void*_tmp30=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp31=_tmp30;struct Cyc_Absyn_Exp*_tmp33;void*_tmp32;switch(*((int*)_tmp31)){case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2 != 0){_LL1: _tmp32=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp31)->f2)->hd;_LL2:
# 218
({union Cyc_Relations_RelnOp _tmp101=Cyc_Relations_RType(_tmp32);*p=_tmp101;});return 1;
goto _LL0;}else{goto _LL5;}}else{goto _LL5;}case 9U: _LL3: _tmp33=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp31)->f1;_LL4:
# 221
 e=_tmp33;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 226
{void*_tmp34=e->r;void*_tmp35=_tmp34;void*_tmp3F;void*_tmp3E;int _tmp3D;void*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;switch(*((int*)_tmp35)){case 14U: _LL8: _tmp3B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp35)->f2;_LL9:
 return Cyc_Relations_exp2relnop(_tmp3B,p);case 1U: _LLA: _tmp3C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp35)->f1;_LLB: {
# 229
struct Cyc_Absyn_Vardecl*_tmp36=Cyc_Tcutil_nonesc_vardecl(_tmp3C);
if(_tmp36 != 0){
({union Cyc_Relations_RelnOp _tmp102=Cyc_Relations_RVar(_tmp36);*p=_tmp102;});
return 1;}
# 234
goto _LL7;}case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35)->f1).Int_c).tag == 5){_LLC: _tmp3D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp35)->f1).Int_c).val).f2;_LLD:
# 236
({union Cyc_Relations_RelnOp _tmp103=Cyc_Relations_RConst((unsigned)_tmp3D);*p=_tmp103;});
return 1;}else{goto _LL12;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f2 != 0){if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f2)->hd)->r)->tag == 1U){_LLE: _tmp3E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp35)->f2)->hd)->r)->f1;_LLF: {
# 239
struct Cyc_Absyn_Vardecl*_tmp37=Cyc_Tcutil_nonesc_vardecl(_tmp3E);
if(_tmp37 != 0){
({union Cyc_Relations_RelnOp _tmp104=Cyc_Relations_RNumelts(_tmp37);*p=_tmp104;});
return 1;}
# 244
goto _LL7;}}else{goto _LL12;}}else{goto _LL12;}}else{goto _LL12;}case 39U: _LL10: _tmp3F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp35)->f1;_LL11:
# 246
{void*_tmp38=Cyc_Tcutil_compress(_tmp3F);void*_tmp39=_tmp38;struct Cyc_Absyn_Exp*_tmp3A;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp39)->tag == 9U){_LL15: _tmp3A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp39)->f1;_LL16:
# 248
 e=_tmp3A;
goto RETRY;}else{_LL17: _LL18:
# 251
({union Cyc_Relations_RelnOp _tmp105=Cyc_Relations_RType(_tmp3F);*p=_tmp105;});
return 1;}_LL14:;}
# 254
goto _LL7;default: _LL12: _LL13:
 goto _LL7;}_LL7:;}
# 258
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmp40=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmp41=_tmp40;unsigned _tmp43;int _tmp42;_LL1A: _tmp43=_tmp41.f1;_tmp42=_tmp41.f2;_LL1B:;
if(_tmp42){
({union Cyc_Relations_RelnOp _tmp106=Cyc_Relations_RConst(_tmp43);*p=_tmp106;});
return 1;}}
# 265
return 0;}
# 268
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp44=e->r;void*_tmp45=_tmp44;void*_tmp47;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp45)->tag == 1U){_LL1: _tmp47=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp45)->f1;_LL2: {
# 271
struct Cyc_Absyn_Vardecl*_tmp46=Cyc_Tcutil_nonesc_vardecl(_tmp47);
if(_tmp46 != 0)
return Cyc_Relations_reln_kill_var(rgn,r,_tmp46);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 279
struct Cyc_Relations_Reln*Cyc_Relations_copy_reln(struct _RegionHandle*r2,struct Cyc_Relations_Reln*r){
return({struct Cyc_Relations_Reln*_tmp48=_region_malloc(r2,sizeof(*_tmp48));*_tmp48=*r;_tmp48;});}
# 283
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*(*f)(struct _RegionHandle*,struct Cyc_Relations_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_Relations_copy_reln,r2,relns);}
# 287
int Cyc_Relations_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0 && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_Relations_Reln*_tmp49=(struct Cyc_Relations_Reln*)r1->hd;
struct Cyc_Relations_Reln*_tmp4A=(struct Cyc_Relations_Reln*)r2->hd;
if((!Cyc_Relations_same_relop(_tmp49->rop1,_tmp4A->rop1)||(int)_tmp49->relation != (int)_tmp4A->relation)|| !
# 293
Cyc_Relations_same_relop(_tmp49->rop2,_tmp4A->rop2))return 0;}
# 295
if(r1 != 0 || r2 != 0)return 0;else{
return 1;}}
# 299
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 303
if(v->escapes)return r;
# 306
r=Cyc_Relations_reln_kill_var(rgn,r,v);
# 311
{void*_tmp4B=e->r;void*_tmp4C=_tmp4B;struct Cyc_Absyn_Exp*_tmp52;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C)->tag == 34U){if((((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C)->f1).fat_result == 1){_LL1: _tmp52=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C)->f1).num_elts;_LL2:
# 314
 malloc_loop: {
void*_tmp4D=_tmp52->r;void*_tmp4E=_tmp4D;void*_tmp51;struct Cyc_Absyn_Exp*_tmp50;switch(*((int*)_tmp4E)){case 14U: _LL6: _tmp50=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E)->f2;_LL7:
 _tmp52=_tmp50;goto malloc_loop;case 1U: _LL8: _tmp51=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4E)->f1;_LL9: {
# 318
struct Cyc_Absyn_Vardecl*_tmp4F=Cyc_Tcutil_nonesc_vardecl(_tmp51);
if(_tmp4F != 0)
return({struct _RegionHandle*_tmp109=rgn;union Cyc_Relations_RelnOp _tmp108=Cyc_Relations_RVar(_tmp4F);union Cyc_Relations_RelnOp _tmp107=Cyc_Relations_RNumelts(v);Cyc_Relations_add_relation(_tmp109,_tmp108,Cyc_Relations_Req,_tmp107,r);});else{
return r;}}default: _LLA: _LLB:
 return r;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 325
 goto _LL0;}_LL0:;}
# 328
if(!Cyc_Tcutil_is_integral_type(v->type))return r;{
# 330
union Cyc_Relations_RelnOp eop=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e,& eop))
return({struct _RegionHandle*_tmp10C=rgn;union Cyc_Relations_RelnOp _tmp10B=Cyc_Relations_RVar(v);union Cyc_Relations_RelnOp _tmp10A=eop;Cyc_Relations_add_relation(_tmp10C,_tmp10B,Cyc_Relations_Req,_tmp10A,r);});
# 334
return r;};}
# 337
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp53=e1->r;void*_tmp54=_tmp53;void*_tmp56;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54)->tag == 1U){_LL1: _tmp56=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54)->f1;_LL2: {
# 340
struct Cyc_Absyn_Vardecl*_tmp55=Cyc_Tcutil_nonesc_vardecl(_tmp56);
if(_tmp55 != 0)
return Cyc_Relations_reln_assign_var(rgn,r,_tmp55,e2);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 349
static union Cyc_Pratt_Node Cyc_Relations_rop2node(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp57=r;unsigned _tmp5E;unsigned _tmp5D;void*_tmp5C;struct Cyc_Absyn_Vardecl*_tmp5B;struct Cyc_Absyn_Vardecl*_tmp5A;switch((_tmp57.RReturn).tag){case 2U: _LL1: _tmp5A=(_tmp57.RVar).val;_LL2:
 return Cyc_Pratt_NVar(_tmp5A);case 3U: _LL3: _tmp5B=(_tmp57.RNumelts).val;_LL4:
 return Cyc_Pratt_NNumelts(_tmp5B);case 4U: _LL5: _tmp5C=(_tmp57.RType).val;_LL6:
 return Cyc_Pratt_NType(_tmp5C);case 5U: _LL7: _tmp5D=(_tmp57.RParam).val;_LL8:
 return Cyc_Pratt_NParam(_tmp5D);case 6U: _LL9: _tmp5E=(_tmp57.RParamNumelts).val;_LLA:
 return Cyc_Pratt_NParamNumelts(_tmp5E);case 7U: _LLB: _LLC:
 return Cyc_Pratt_NReturn();default: _LLD: _LLE:
({void*_tmp58=0U;({struct _dyneither_ptr _tmp10D=({const char*_tmp59="rop2node";_tag_dyneither(_tmp59,sizeof(char),9U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp10D,_tag_dyneither(_tmp58,sizeof(void*),0U));});});}_LL0:;}
# 361
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmp5F=r;switch(_tmp5F){case Cyc_Relations_Req: _LL1: _LL2:
 return({const char*_tmp60="==";_tag_dyneither(_tmp60,sizeof(char),3U);});case Cyc_Relations_Rneq: _LL3: _LL4:
 return({const char*_tmp61="!=";_tag_dyneither(_tmp61,sizeof(char),3U);});case Cyc_Relations_Rlt: _LL5: _LL6:
 return({const char*_tmp62="<";_tag_dyneither(_tmp62,sizeof(char),2U);});case Cyc_Relations_Rlte: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA: return({const char*_tmp63="<=";_tag_dyneither(_tmp63,sizeof(char),3U);});}_LL0:;}
# 370
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp64=r;unsigned _tmp7B;unsigned _tmp7A;struct Cyc_Absyn_Vardecl*_tmp79;void*_tmp78;struct Cyc_Absyn_Vardecl*_tmp77;unsigned _tmp76;switch((_tmp64.RParamNumelts).tag){case 1U: _LL1: _tmp76=(_tmp64.RConst).val;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp67=({struct Cyc_Int_pa_PrintArg_struct _tmpE9;_tmpE9.tag=1U,_tmpE9.f1=(unsigned long)((int)_tmp76);_tmpE9;});void*_tmp65[1U];_tmp65[0]=& _tmp67;({struct _dyneither_ptr _tmp10E=({const char*_tmp66="%d";_tag_dyneither(_tmp66,sizeof(char),3U);});Cyc_aprintf(_tmp10E,_tag_dyneither(_tmp65,sizeof(void*),1U));});});case 2U: _LL3: _tmp77=(_tmp64.RVar).val;_LL4:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6A=({struct Cyc_String_pa_PrintArg_struct _tmpEA;_tmpEA.tag=0U,({struct _dyneither_ptr _tmp10F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp77->name));_tmpEA.f1=_tmp10F;});_tmpEA;});void*_tmp68[1U];_tmp68[0]=& _tmp6A;({struct _dyneither_ptr _tmp110=({const char*_tmp69="%s";_tag_dyneither(_tmp69,sizeof(char),3U);});Cyc_aprintf(_tmp110,_tag_dyneither(_tmp68,sizeof(void*),1U));});});case 4U: _LL5: _tmp78=(_tmp64.RType).val;_LL6:
 return Cyc_Absynpp_typ2string(_tmp78);case 3U: _LL7: _tmp79=(_tmp64.RNumelts).val;_LL8:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmpEB;_tmpEB.tag=0U,({struct _dyneither_ptr _tmp111=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp79->name));_tmpEB.f1=_tmp111;});_tmpEB;});void*_tmp6B[1U];_tmp6B[0]=& _tmp6D;({struct _dyneither_ptr _tmp112=({const char*_tmp6C="numelts(%s)";_tag_dyneither(_tmp6C,sizeof(char),12U);});Cyc_aprintf(_tmp112,_tag_dyneither(_tmp6B,sizeof(void*),1U));});});case 5U: _LL9: _tmp7A=(_tmp64.RParam).val;_LLA:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp70=({struct Cyc_Int_pa_PrintArg_struct _tmpEC;_tmpEC.tag=1U,_tmpEC.f1=(unsigned long)((int)_tmp7A);_tmpEC;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({struct _dyneither_ptr _tmp113=({const char*_tmp6F="param(%d)";_tag_dyneither(_tmp6F,sizeof(char),10U);});Cyc_aprintf(_tmp113,_tag_dyneither(_tmp6E,sizeof(void*),1U));});});case 6U: _LLB: _tmp7B=(_tmp64.RParamNumelts).val;_LLC:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp73=({struct Cyc_Int_pa_PrintArg_struct _tmpED;_tmpED.tag=1U,_tmpED.f1=(unsigned long)((int)_tmp7B);_tmpED;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({struct _dyneither_ptr _tmp114=({const char*_tmp72="numelts(param(%d))";_tag_dyneither(_tmp72,sizeof(char),19U);});Cyc_aprintf(_tmp114,_tag_dyneither(_tmp71,sizeof(void*),1U));});});default: _LLD: _LLE:
 return(struct _dyneither_ptr)({void*_tmp74=0U;({struct _dyneither_ptr _tmp115=({const char*_tmp75="return_value";_tag_dyneither(_tmp75,sizeof(char),13U);});Cyc_aprintf(_tmp115,_tag_dyneither(_tmp74,sizeof(void*),0U));});});}_LL0:;}
# 382
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r){
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmpF0;_tmpF0.tag=0U,({struct _dyneither_ptr _tmp116=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_rop2string(r->rop1));_tmpF0.f1=_tmp116;});_tmpF0;});struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0U,({struct _dyneither_ptr _tmp117=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_relation2string(r->relation));_tmpEF.f1=_tmp117;});_tmpEF;});struct Cyc_String_pa_PrintArg_struct _tmp80=({struct Cyc_String_pa_PrintArg_struct _tmpEE;_tmpEE.tag=0U,({
struct _dyneither_ptr _tmp118=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_rop2string(r->rop2));_tmpEE.f1=_tmp118;});_tmpEE;});void*_tmp7C[3U];_tmp7C[0]=& _tmp7E,_tmp7C[1]=& _tmp7F,_tmp7C[2]=& _tmp80;({struct _dyneither_ptr _tmp119=({const char*_tmp7D="%s %s %s";_tag_dyneither(_tmp7D,sizeof(char),9U);});Cyc_aprintf(_tmp119,_tag_dyneither(_tmp7C,sizeof(void*),3U));});});}
# 387
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*rs){
if(rs == 0)return({const char*_tmp81="true";_tag_dyneither(_tmp81,sizeof(char),5U);});{
struct Cyc_List_List*ss=0;
for(0;rs != 0;rs=rs->tl){
ss=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));({struct _dyneither_ptr*_tmp11B=({struct _dyneither_ptr*_tmp82=_cycalloc(sizeof(*_tmp82));({struct _dyneither_ptr _tmp11A=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)rs->hd);*_tmp82=_tmp11A;});_tmp82;});_tmp83->hd=_tmp11B;}),_tmp83->tl=ss;_tmp83;});
if(rs->tl != 0)ss=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*_tmp86));({struct _dyneither_ptr*_tmp11D=({struct _dyneither_ptr*_tmp85=_cycalloc(sizeof(*_tmp85));({struct _dyneither_ptr _tmp11C=({const char*_tmp84=" && ";_tag_dyneither(_tmp84,sizeof(char),5U);});*_tmp85=_tmp11C;});_tmp85;});_tmp86->hd=_tmp11D;}),_tmp86->tl=ss;_tmp86;});}
# 394
return(struct _dyneither_ptr)Cyc_strconcat_l(ss);};}
# 397
void Cyc_Relations_print_relns(struct Cyc___cycFILE*f,struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0U,({struct _dyneither_ptr _tmp11E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)r->hd));_tmpF1.f1=_tmp11E;});_tmpF1;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({struct Cyc___cycFILE*_tmp120=f;struct _dyneither_ptr _tmp11F=({const char*_tmp88="%s";_tag_dyneither(_tmp88,sizeof(char),3U);});Cyc_fprintf(_tmp120,_tmp11F,_tag_dyneither(_tmp87,sizeof(void*),1U));});});
if(r->tl != 0)({void*_tmp8A=0U;({struct Cyc___cycFILE*_tmp122=f;struct _dyneither_ptr _tmp121=({const char*_tmp8B=",";_tag_dyneither(_tmp8B,sizeof(char),2U);});Cyc_fprintf(_tmp122,_tmp121,_tag_dyneither(_tmp8A,sizeof(void*),0U));});});}}
# 404
static union Cyc_Relations_RelnOp Cyc_Relations_reduce_relnop(union Cyc_Relations_RelnOp rop){
{union Cyc_Relations_RelnOp _tmp8C=rop;struct Cyc_Absyn_Vardecl*_tmp96;void*_tmp95;switch((_tmp8C.RNumelts).tag){case 4U: _LL1: _tmp95=(_tmp8C.RType).val;_LL2:
# 407
{void*_tmp8D=Cyc_Tcutil_compress(_tmp95);void*_tmp8E=_tmp8D;struct Cyc_Absyn_Exp*_tmp8F;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8E)->tag == 9U){_LL8: _tmp8F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8E)->f1;_LL9: {
# 409
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp8F,& rop2))return rop2;
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 414
goto _LL0;case 3U: _LL3: _tmp96=(_tmp8C.RNumelts).val;_LL4:
# 416
{void*_tmp90=Cyc_Tcutil_compress(_tmp96->type);void*_tmp91=_tmp90;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 3U){_LLD: _tmp94=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).bounds;_LLE:
# 420
 if(({void*_tmp123=_tmp94;_tmp123 == Cyc_Absyn_bounds_one();}))goto _LLC;{
struct Cyc_Absyn_Exp*_tmp92=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp94);
if(_tmp92 != 0){
struct Cyc_Absyn_Exp*_tmp93=_tmp92;
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp93,& rop2))return rop2;}
# 427
goto _LLC;};}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 430
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 433
return rop;}
# 438
static int Cyc_Relations_consist_relations(struct Cyc_Pratt_Graph*G,struct Cyc_List_List*rlns){
for(0;(unsigned)G && rlns != 0;rlns=rlns->tl){
struct Cyc_Relations_Reln*_tmp97=(struct Cyc_Relations_Reln*)rlns->hd;
({union Cyc_Relations_RelnOp _tmp124=Cyc_Relations_reduce_relnop(_tmp97->rop1);_tmp97->rop1=_tmp124;});
({union Cyc_Relations_RelnOp _tmp125=Cyc_Relations_reduce_relnop(_tmp97->rop2);_tmp97->rop2=_tmp125;});{
# 448
struct Cyc_Relations_Reln*_tmp98=_tmp97;union Cyc_Relations_RelnOp _tmpB2;enum Cyc_Relations_Relation _tmpB1;union Cyc_Relations_RelnOp _tmpB0;union Cyc_Relations_RelnOp _tmpAF;enum Cyc_Relations_Relation _tmpAE;unsigned _tmpAD;union Cyc_Relations_RelnOp _tmpAC;union Cyc_Relations_RelnOp _tmpAB;union Cyc_Relations_RelnOp _tmpAA;unsigned _tmpA9;unsigned _tmpA8;enum Cyc_Relations_Relation _tmpA7;union Cyc_Relations_RelnOp _tmpA6;unsigned _tmpA5;union Cyc_Relations_RelnOp _tmpA4;unsigned _tmpA3;enum Cyc_Relations_Relation _tmpA2;unsigned _tmpA1;if(((((struct Cyc_Relations_Reln*)_tmp98)->rop1).RConst).tag == 1){if(((((struct Cyc_Relations_Reln*)_tmp98)->rop2).RConst).tag == 1){_LL1: _tmpA3=((_tmp98->rop1).RConst).val;_tmpA2=_tmp98->relation;_tmpA1=((_tmp98->rop2).RConst).val;_LL2:
# 450
{enum Cyc_Relations_Relation _tmp99=_tmpA2;switch(_tmp99){case Cyc_Relations_Req: _LL10: _LL11:
 if(_tmpA3 != _tmpA1)return 0;goto _LLF;case Cyc_Relations_Rneq: _LL12: _LL13:
 if(_tmpA3 == _tmpA1)return 0;goto _LLF;case Cyc_Relations_Rlt: _LL14: _LL15:
 if(_tmpA3 >= _tmpA1)return 0;goto _LLF;case Cyc_Relations_Rlte: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19: if(_tmpA3 > _tmpA1)return 0;goto _LLF;}_LLF:;}
# 456
goto _LL0;}else{if(((struct Cyc_Relations_Reln*)_tmp98)->relation == Cyc_Relations_Rneq){_LL5: _tmpA5=((_tmp98->rop1).RConst).val;_tmpA4=_tmp98->rop2;_LL6: {
# 463
union Cyc_Pratt_Node _tmp9A=Cyc_Relations_rop2node(_tmpA4);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_tmp129=G2;union Cyc_Pratt_Node _tmp128=_tmp9A;union Cyc_Pratt_Node _tmp127=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp129,_tmp128,_tmp127,({struct Cyc_AP_T*_tmp126=Cyc_AP_fromint((long)_tmpA5);Cyc_AP_sub(_tmp126,Cyc_AP_one);}));});
G=({struct Cyc_Pratt_Graph*_tmp12D=G;union Cyc_Pratt_Node _tmp12C=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp12B=_tmp9A;Cyc_Pratt_add_edge(_tmp12D,_tmp12C,_tmp12B,Cyc_AP_neg(({struct Cyc_AP_T*_tmp12A=Cyc_AP_fromint((long)_tmpA5);Cyc_AP_add(_tmp12A,Cyc_AP_one);})));});
# 473
if(G != 0 && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}else{_LLB: _tmpA8=((_tmp98->rop1).RConst).val;_tmpA7=_tmp98->relation;_tmpA6=_tmp98->rop2;_LLC: {
# 507
union Cyc_Pratt_Node _tmp9E=Cyc_Relations_rop2node(_tmpA6);
if((int)_tmpA7 == (int)3U)_tmpA8=_tmpA8 + (unsigned)1;
G=({struct Cyc_Pratt_Graph*_tmp130=G;union Cyc_Pratt_Node _tmp12F=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp12E=_tmp9E;Cyc_Pratt_add_edge(_tmp130,_tmp12F,_tmp12E,Cyc_AP_neg(Cyc_AP_fromint((long)_tmpA8)));});
# 512
if((unsigned)G &&(int)_tmpA7 == (int)0U)
# 517
G=({struct Cyc_Pratt_Graph*_tmp133=G;union Cyc_Pratt_Node _tmp132=_tmp9E;union Cyc_Pratt_Node _tmp131=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp133,_tmp132,_tmp131,Cyc_AP_fromint((long)_tmpA8));});
# 524
goto _LL0;}}}}else{if(((struct Cyc_Relations_Reln*)_tmp98)->relation == Cyc_Relations_Rneq){if(((((struct Cyc_Relations_Reln*)_tmp98)->rop2).RConst).tag == 1){_LL3: _tmpAA=_tmp98->rop1;_tmpA9=((_tmp98->rop2).RConst).val;_LL4:
# 461
 _tmpA5=_tmpA9;_tmpA4=_tmpAA;goto _LL6;}else{_LL7: _tmpAC=_tmp98->rop1;_tmpAB=_tmp98->rop2;_LL8: {
# 480
union Cyc_Pratt_Node _tmp9B=Cyc_Relations_rop2node(_tmpAC);
union Cyc_Pratt_Node _tmp9C=Cyc_Relations_rop2node(_tmpAB);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
G2=({struct Cyc_Pratt_Graph*_tmp136=G2;union Cyc_Pratt_Node _tmp135=_tmp9B;union Cyc_Pratt_Node _tmp134=_tmp9C;Cyc_Pratt_add_edge(_tmp136,_tmp135,_tmp134,Cyc_AP_neg(Cyc_AP_one));});
G=({struct Cyc_Pratt_Graph*_tmp139=G;union Cyc_Pratt_Node _tmp138=_tmp9C;union Cyc_Pratt_Node _tmp137=_tmp9B;Cyc_Pratt_add_edge(_tmp139,_tmp138,_tmp137,Cyc_AP_neg(Cyc_AP_one));});
if(G != 0 && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl)||
 Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}}else{if(((((struct Cyc_Relations_Reln*)_tmp98)->rop2).RConst).tag == 1){_LL9: _tmpAF=_tmp98->rop1;_tmpAE=_tmp98->relation;_tmpAD=((_tmp98->rop2).RConst).val;_LLA: {
# 494
union Cyc_Pratt_Node _tmp9D=Cyc_Relations_rop2node(_tmpAF);
if((int)_tmpAE == (int)3U)_tmpAD=_tmpAD - (unsigned)1;
G=({struct Cyc_Pratt_Graph*_tmp13C=G;union Cyc_Pratt_Node _tmp13B=_tmp9D;union Cyc_Pratt_Node _tmp13A=Cyc_Pratt_zero_node;Cyc_Pratt_add_edge(_tmp13C,_tmp13B,_tmp13A,Cyc_AP_fromint((long)_tmpAD));});
# 498
if((unsigned)G &&(int)_tmpAE == (int)0U)
G=({struct Cyc_Pratt_Graph*_tmp13F=G;union Cyc_Pratt_Node _tmp13E=Cyc_Pratt_zero_node;union Cyc_Pratt_Node _tmp13D=_tmp9D;Cyc_Pratt_add_edge(_tmp13F,_tmp13E,_tmp13D,Cyc_AP_neg(Cyc_AP_fromint((long)_tmpAD)));});
goto _LL0;}}else{_LLD: _tmpB2=_tmp98->rop1;_tmpB1=_tmp98->relation;_tmpB0=_tmp98->rop2;_LLE: {
# 527
union Cyc_Pratt_Node _tmp9F=Cyc_Relations_rop2node(_tmpB2);
union Cyc_Pratt_Node _tmpA0=Cyc_Relations_rop2node(_tmpB0);
int i=(int)_tmpB1 == (int)3U?- 1: 0;
# 532
G=({struct Cyc_Pratt_Graph*_tmp142=G;union Cyc_Pratt_Node _tmp141=_tmp9F;union Cyc_Pratt_Node _tmp140=_tmpA0;Cyc_Pratt_add_edge(_tmp142,_tmp141,_tmp140,Cyc_AP_fromint(i));});
# 534
if((unsigned)G &&(int)_tmpB1 == (int)0U)
G=({struct Cyc_Pratt_Graph*_tmp145=G;union Cyc_Pratt_Node _tmp144=_tmpA0;union Cyc_Pratt_Node _tmp143=_tmp9F;Cyc_Pratt_add_edge(_tmp145,_tmp144,_tmp143,Cyc_AP_fromint(i));});
goto _LL0;}}}}_LL0:;};}
# 539
if(G != 0)return 1;else{return 0;}}
# 547
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns){
# 553
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
return Cyc_Relations_consist_relations(G,rlns);}
# 561
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 563
enum Cyc_Absyn_Primop _tmpB3=p;switch(_tmpB3){case Cyc_Absyn_Eq: _LL1: _LL2:
 return({struct _tuple10 _tmpF2;_tmpF2.f1=e1,_tmpF2.f2=Cyc_Relations_Req,_tmpF2.f3=e2;_tmpF2;});case Cyc_Absyn_Neq: _LL3: _LL4:
 return({struct _tuple10 _tmpF3;_tmpF3.f1=e1,_tmpF3.f2=Cyc_Relations_Rneq,_tmpF3.f3=e2;_tmpF3;});case Cyc_Absyn_Lt: _LL5: _LL6:
 return({struct _tuple10 _tmpF4;_tmpF4.f1=e1,_tmpF4.f2=Cyc_Relations_Rlt,_tmpF4.f3=e2;_tmpF4;});case Cyc_Absyn_Lte: _LL7: _LL8:
 return({struct _tuple10 _tmpF5;_tmpF5.f1=e1,_tmpF5.f2=Cyc_Relations_Rlte,_tmpF5.f3=e2;_tmpF5;});case Cyc_Absyn_Gt: _LL9: _LLA:
 return({struct _tuple10 _tmpF6;_tmpF6.f1=e2,_tmpF6.f2=Cyc_Relations_Rlt,_tmpF6.f3=e1;_tmpF6;});case Cyc_Absyn_Gte: _LLB: _LLC:
 return({struct _tuple10 _tmpF7;_tmpF7.f1=e2,_tmpF7.f2=Cyc_Relations_Rlte,_tmpF7.f3=e1;_tmpF7;});default: _LLD: _LLE:
({void*_tmpB4=0U;({struct _dyneither_ptr _tmp146=({const char*_tmpB5="primop2relation";_tag_dyneither(_tmpB5,sizeof(char),16U);});((int(*)(struct _dyneither_ptr,struct _dyneither_ptr))Cyc_Tcutil_impos)(_tmp146,_tag_dyneither(_tmpB4,sizeof(void*),0U));});});}_LL0:;}
# 575
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmpB6=r;switch(_tmpB6){case Cyc_Relations_Req: _LL1: _LL2:
 return Cyc_Relations_Rneq;case Cyc_Relations_Rneq: _LL3: _LL4:
 return Cyc_Relations_Req;case Cyc_Relations_Rlt: _LL5: _LL6:
 return Cyc_Relations_Rlte;case Cyc_Relations_Rlte: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA: return Cyc_Relations_Rlt;}_LL0:;}
# 584
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*r,struct Cyc_Relations_Reln*rln){
return({struct Cyc_Relations_Reln*_tmpB7=_region_malloc(r,sizeof(*_tmpB7));_tmpB7->rop1=rln->rop2,({enum Cyc_Relations_Relation _tmp147=Cyc_Relations_flip_relation(rln->relation);_tmpB7->relation=_tmp147;}),_tmpB7->rop2=rln->rop1;_tmpB7;});}
# 595
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 597
{void*_tmpB8=e->r;void*_tmpB9=_tmpB8;enum Cyc_Absyn_Primop _tmpD5;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;enum Cyc_Absyn_Primop _tmpD2;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;void*_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;switch(*((int*)_tmpB9)){case 14U: _LL1: _tmpCD=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpCC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL2:
# 600
{void*_tmpBA=Cyc_Tcutil_compress(_tmpCD);void*_tmpBB=_tmpBA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBB)->f1)->f2 == Cyc_Absyn_Int_sz){_LLC: _LLD:
 return Cyc_Relations_exp2relns(r,_tmpCC);}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _LLF:
 goto _LLB;}_LLB:;}
# 604
goto _LL0;case 7U: _LL3: _tmpCF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpCE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4:
 return({struct _RegionHandle*_tmp149=r;struct Cyc_List_List*_tmp148=Cyc_Relations_exp2relns(r,_tmpCF);((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp149,_tmp148,Cyc_Relations_exp2relns(r,_tmpCE));});case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2 != 0){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd)->r)->tag == 3U){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd)->r)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd)->r)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd)->r)->f2)->tl)->tl == 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl == 0){_LL5: _tmpD2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd)->r)->f1;_tmpD1=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd)->r)->f2)->hd;_tmpD0=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd)->r)->f2)->tl)->hd;_LL6:
# 607
{enum Cyc_Absyn_Primop _tmpBC=_tmpD2;switch(_tmpBC){case Cyc_Absyn_Eq: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Neq: _LL13: _LL14: goto _LL16;case Cyc_Absyn_Lt: _LL15: _LL16: goto _LL18;case Cyc_Absyn_Lte: _LL17: _LL18: goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A: goto _LL1C;case Cyc_Absyn_Gte: _LL1B: _LL1C: {
struct _tuple10 _tmpBD=Cyc_Relations_primop2relation(_tmpD1,_tmpD2,_tmpD0);struct _tuple10 _tmpBE=_tmpBD;struct Cyc_Absyn_Exp*_tmpC3;enum Cyc_Relations_Relation _tmpC2;struct Cyc_Absyn_Exp*_tmpC1;_LL20: _tmpC3=_tmpBE.f1;_tmpC2=_tmpBE.f2;_tmpC1=_tmpBE.f3;_LL21:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpC3,& rop1)&& Cyc_Relations_exp2relnop(_tmpC1,& rop2))
return({struct Cyc_List_List*_tmpC0=_region_malloc(r,sizeof(*_tmpC0));({struct Cyc_Relations_Reln*_tmp14B=({struct Cyc_Relations_Reln*_tmpBF=_region_malloc(r,sizeof(*_tmpBF));_tmpBF->rop1=rop2,({enum Cyc_Relations_Relation _tmp14A=Cyc_Relations_flip_relation(_tmpC2);_tmpBF->relation=_tmp14A;}),_tmpBF->rop2=rop1;_tmpBF;});_tmpC0->hd=_tmp14B;}),_tmpC0->tl=0;_tmpC0;});
goto _LL10;};}default: _LL1D: _LL1E:
 goto _LL10;}_LL10:;}
# 617
goto _LL0;}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl)->tl == 0){_LL7: _tmpD5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD4=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->hd;_tmpD3=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2)->tl)->hd;_LL8:
# 619
{enum Cyc_Absyn_Primop _tmpC4=_tmpD5;switch(_tmpC4){case Cyc_Absyn_Eq: _LL23: _LL24:
 goto _LL26;case Cyc_Absyn_Neq: _LL25: _LL26: goto _LL28;case Cyc_Absyn_Lt: _LL27: _LL28: goto _LL2A;case Cyc_Absyn_Lte: _LL29: _LL2A: goto _LL2C;case Cyc_Absyn_Gt: _LL2B: _LL2C: goto _LL2E;case Cyc_Absyn_Gte: _LL2D: _LL2E: {
struct _tuple10 _tmpC5=Cyc_Relations_primop2relation(_tmpD4,_tmpD5,_tmpD3);struct _tuple10 _tmpC6=_tmpC5;struct Cyc_Absyn_Exp*_tmpCB;enum Cyc_Relations_Relation _tmpCA;struct Cyc_Absyn_Exp*_tmpC9;_LL32: _tmpCB=_tmpC6.f1;_tmpCA=_tmpC6.f2;_tmpC9=_tmpC6.f3;_LL33:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpCB,& rop1)&& Cyc_Relations_exp2relnop(_tmpC9,& rop2))
return({struct Cyc_List_List*_tmpC8=_region_malloc(r,sizeof(*_tmpC8));({struct Cyc_Relations_Reln*_tmp14C=({struct Cyc_Relations_Reln*_tmpC7=_region_malloc(r,sizeof(*_tmpC7));_tmpC7->rop1=rop1,_tmpC7->relation=_tmpCA,_tmpC7->rop2=rop2;_tmpC7;});_tmpC8->hd=_tmp14C;}),_tmpC8->tl=0;_tmpC8;});
goto _LL22;};}default: _LL2F: _LL30:
 goto _LL22;}_LL22:;}
# 629
goto _LL0;}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}}default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 633
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmpD6=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmpD7=_tmpD6;unsigned _tmpDE;int _tmpDD;_LL35: _tmpDE=_tmpD7.f1;_tmpDD=_tmpD7.f2;_LL36:;
if(_tmpDD){
if((int)_tmpDE)return 0;else{
# 638
return({struct Cyc_List_List*_tmpD9=_region_malloc(r,sizeof(*_tmpD9));({struct Cyc_Relations_Reln*_tmp14F=({struct Cyc_Relations_Reln*_tmpD8=_region_malloc(r,sizeof(*_tmpD8));({union Cyc_Relations_RelnOp _tmp14E=Cyc_Relations_RConst(0U);_tmpD8->rop1=_tmp14E;}),_tmpD8->relation=Cyc_Relations_Rlt,({union Cyc_Relations_RelnOp _tmp14D=Cyc_Relations_RConst(0U);_tmpD8->rop2=_tmp14D;});_tmpD8;});_tmpD9->hd=_tmp14F;}),_tmpD9->tl=0;_tmpD9;});}}
# 640
({struct Cyc_String_pa_PrintArg_struct _tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0U,({
struct _dyneither_ptr _tmp150=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpF8.f1=_tmp150;});_tmpF8;});void*_tmpDA[1U];_tmpDA[0]=& _tmpDC;({unsigned _tmp152=e->loc;struct _dyneither_ptr _tmp151=({const char*_tmpDB="unable to convert `%s' to static relation";_tag_dyneither(_tmpDB,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp152,_tmp151,_tag_dyneither(_tmpDA,sizeof(void*),1U));});});}
# 644
return({struct Cyc_List_List*_tmpE0=_region_malloc(r,sizeof(*_tmpE0));({struct Cyc_Relations_Reln*_tmp155=({struct Cyc_Relations_Reln*_tmpDF=_region_malloc(r,sizeof(*_tmpDF));({union Cyc_Relations_RelnOp _tmp154=Cyc_Relations_RConst(0U);_tmpDF->rop1=_tmp154;}),_tmpDF->relation=Cyc_Relations_Rlt,({union Cyc_Relations_RelnOp _tmp153=Cyc_Relations_RConst(0U);_tmpDF->rop2=_tmp153;});_tmpDF;});_tmpE0->hd=_tmp155;}),_tmpE0->tl=0;_tmpE0;});}
