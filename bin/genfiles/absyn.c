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
 struct Cyc_Core_Opt{void*v;};
# 119 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133 "list.h"
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr);
# 34 "position.h"
unsigned int Cyc_Position_segment_join(unsigned int,unsigned int);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 315
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 463
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
extern struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val;
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
# 506
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr);
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 675 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 866
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 883
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 890
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 895
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 898
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 903
void*Cyc_Absyn_compress_kb(void*);
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 909
int Cyc_Absyn_type2bool(int def,void*);
# 911
void*Cyc_Absyn_app_type(void*,struct _dyneither_ptr);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 916
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 919
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 921
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 923
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;void*Cyc_Absyn_wchar_type();
void*Cyc_Absyn_gen_float_type(unsigned int i);
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 928
extern void*Cyc_Absyn_empty_effect;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 947
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_exn_type();
# 951
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
# 954
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 957
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
void*Cyc_Absyn_thin_bounds_int(unsigned int);
void*Cyc_Absyn_bounds_one();
# 963
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 965
void*Cyc_Absyn_starb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*bounds,void*zero_term);
# 968
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 971
void*Cyc_Absyn_star_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 974
void*Cyc_Absyn_at_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zero_term);
# 977
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq);
# 979
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 981
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_type(struct _tuple0*name);
# 987
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 990
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1065
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1068
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1092
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1096
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1107
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1110
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1116
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1119
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1124
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1130
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1142
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1144
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1151
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1153
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*name);
# 1155
struct _dyneither_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl);
# 1157
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1159
int Cyc_Absyn_fntype_att(void*);
# 1161
int Cyc_Absyn_equal_att(void*a1,void*a2);
# 1163
int Cyc_Absyn_attribute_cmp(void*att1,void*att2);
# 1165
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1171
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1173
int Cyc_Absyn_is_require_union_type(void*);
# 1175
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 1178
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1181
extern int Cyc_Absyn_porting_c_code;
# 1183
extern int Cyc_Absyn_no_regions;struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
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
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
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
# 47
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _dyneither_ptr*)ss1->hd,(struct _dyneither_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;};}
# 54
if(ss2 != 0)return - 1;
return 0;}
# 57
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 61
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
struct _tuple0*_tmp0=q1;union Cyc_Absyn_Nmspace _tmpD;struct _dyneither_ptr*_tmpC;_LL1: _tmpD=_tmp0->f1;_tmpC=_tmp0->f2;_LL2:;{
struct _tuple0*_tmp1=q2;union Cyc_Absyn_Nmspace _tmpB;struct _dyneither_ptr*_tmpA;_LL4: _tmpB=_tmp1->f1;_tmpA=_tmp1->f2;_LL5:;{
int i=Cyc_strptrcmp(_tmpC,_tmpA);
if(i != 0)return i;{
struct _tuple12 _tmp2=({struct _tuple12 _tmp151;_tmp151.f1=_tmpD,_tmp151.f2=_tmpB;_tmp151;});struct _tuple12 _tmp3=_tmp2;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp4;switch(((_tmp3.f1).Abs_n).tag){case 4U: if(((_tmp3.f2).Loc_n).tag == 4){_LL7: _LL8:
 return 0;}else{_LLF: _LL10:
# 73
 return - 1;}case 1U: switch(((_tmp3.f2).Loc_n).tag){case 1U: _LL9: _tmp5=((_tmp3.f1).Rel_n).val;_tmp4=((_tmp3.f2).Rel_n).val;_LLA:
# 69
 return Cyc_Absyn_strlist_cmp(_tmp5,_tmp4);case 4U: goto _LL11;default: _LL13: _LL14:
# 75
 return - 1;}case 2U: switch(((_tmp3.f2).Rel_n).tag){case 2U: _LLB: _tmp7=((_tmp3.f1).Abs_n).val;_tmp6=((_tmp3.f2).Abs_n).val;_LLC:
# 70
 return Cyc_Absyn_strlist_cmp(_tmp7,_tmp6);case 4U: goto _LL11;case 1U: goto _LL15;default: _LL17: _LL18:
# 77
 return - 1;}default: switch(((_tmp3.f2).Rel_n).tag){case 3U: _LLD: _tmp9=((_tmp3.f1).C_n).val;_tmp8=((_tmp3.f2).C_n).val;_LLE:
# 71
 return Cyc_Absyn_strlist_cmp(_tmp9,_tmp8);case 4U: _LL11: _LL12:
# 74
 return 1;case 1U: _LL15: _LL16:
# 76
 return 1;default: _LL19: _LL1A:
# 78
 return 1;}}_LL6:;};};};};}
# 82
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 88
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
if(C_scope)
return({union Cyc_Absyn_Nmspace _tmp152;(_tmp152.C_n).tag=3U,(_tmp152.C_n).val=x;_tmp152;});else{
# 93
return({union Cyc_Absyn_Nmspace _tmp153;(_tmp153.Abs_n).tag=2U,(_tmp153.Abs_n).val=x;_tmp153;});}}
# 95
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp154;(_tmp154.Rel_n).tag=1U,(_tmp154.Rel_n).val=x;_tmp154;});}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 98
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmpE=(*qv).f1;union Cyc_Absyn_Nmspace _tmpF=_tmpE;switch((_tmpF.Loc_n).tag){case 1U: if((_tmpF.Rel_n).val == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LL7;}case 2U: if((_tmpF.Abs_n).val == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LL7;}case 4U: _LL5: _LL6:
 return 0;default: _LL7: _LL8:
 return 1;}_LL0:;}
# 107
static int Cyc_Absyn_new_type_counter=0;
# 109
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1U,_tmp10->f1=k,_tmp10->f2=0,_tmp10->f3=Cyc_Absyn_new_type_counter ++,_tmp10->f4=env;_tmp10;});}
# 112
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 116
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
return({struct Cyc_Absyn_Tqual _tmp155;_tmp155.print_const=x.print_const  || y.print_const,_tmp155.q_volatile=
x.q_volatile  || y.q_volatile,_tmp155.q_restrict=
x.q_restrict  || y.q_restrict,_tmp155.real_const=
x.real_const  || y.real_const,({
unsigned int _tmp181=Cyc_Position_segment_join(x.loc,y.loc);_tmp155.loc=_tmp181;});_tmp155;});}
# 124
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp156;_tmp156.print_const=0,_tmp156.q_volatile=0,_tmp156.q_restrict=0,_tmp156.real_const=0,_tmp156.loc=loc;_tmp156;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){return({struct Cyc_Absyn_Tqual _tmp157;_tmp157.print_const=1,_tmp157.q_volatile=0,_tmp157.q_restrict=0,_tmp157.real_const=1,_tmp157.loc=loc;_tmp157;});}
# 127
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 129
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
return({union Cyc_Absyn_DatatypeInfo _tmp158;(_tmp158.UnknownDatatype).tag=1U,(_tmp158.UnknownDatatype).val=udi;_tmp158;});}
# 132
extern union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
return({union Cyc_Absyn_DatatypeInfo _tmp159;(_tmp159.KnownDatatype).tag=2U,(_tmp159.KnownDatatype).val=d;_tmp159;});}
# 135
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp15A;(_tmp15A.UnknownDatatypefield).tag=1U,(_tmp15A.UnknownDatatypefield).val=s;_tmp15A;});}
# 138
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
return({union Cyc_Absyn_DatatypeFieldInfo _tmp15B;(_tmp15B.KnownDatatypefield).tag=2U,((_tmp15B.KnownDatatypefield).val).f1=dd,((_tmp15B.KnownDatatypefield).val).f2=df;_tmp15B;});}
# 141
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
return({union Cyc_Absyn_AggrInfo _tmp15C;(_tmp15C.UnknownAggr).tag=1U,((_tmp15C.UnknownAggr).val).f1=ak,((_tmp15C.UnknownAggr).val).f2=n,((_tmp15C.UnknownAggr).val).f3=tagged;_tmp15C;});}
# 144
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
return({union Cyc_Absyn_AggrInfo _tmp15D;(_tmp15D.KnownAggr).tag=2U,(_tmp15D.KnownAggr).val=ad;_tmp15D;});}
# 147
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp11=k;void**_tmp13;void**_tmp12;switch(*((int*)_tmp11)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL3: _LL4:
 goto _LL6;}else{_LL7: _tmp12=(void**)&(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LL8:
# 152
 _tmp13=_tmp12;goto _LLA;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1 == 0){_LL5: _LL6:
# 151
 return k;}else{_LL9: _tmp13=(void**)&(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11)->f1)->v;_LLA:
# 154
({void*_tmp182=Cyc_Absyn_compress_kb(*_tmp13);*_tmp13=_tmp182;});
return*_tmp13;}}_LL0:;}
# 158
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp14=Cyc_Absyn_compress_kb(kb);void*_tmp15=_tmp14;struct Cyc_Core_Opt**_tmp1A;struct Cyc_Absyn_Kind*_tmp19;struct Cyc_Core_Opt**_tmp18;struct Cyc_Absyn_Kind*_tmp17;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp17=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL2:
 return _tmp17;case 1U: _LL3: _tmp18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15)->f1;_LL4:
 _tmp1A=_tmp18;_tmp19=& Cyc_Tcutil_bk;goto _LL6;default: _LL5: _tmp1A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f1;_tmp19=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15)->f2;_LL6:
# 163
({struct Cyc_Core_Opt*_tmp184=({struct Cyc_Core_Opt*_tmp16=_cycalloc(sizeof(*_tmp16));({void*_tmp183=Cyc_Tcutil_kind_to_bound(_tmp19);_tmp16->v=_tmp183;});_tmp16;});*_tmp1A=_tmp184;});
return _tmp19;}_LL0:;}
# 169
void*Cyc_Absyn_app_type(void*c,struct _dyneither_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0U,_tmp1B->f1=c,({struct Cyc_List_List*_tmp185=((struct Cyc_List_List*(*)(struct _dyneither_ptr arr))Cyc_List_from_array)(args);_tmp1B->f2=_tmp185;});_tmp1B;});}
# 172
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->tag=9U,_tmp1C->f1=e;_tmp1C;});}
# 175
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_Absyn_VoidCon_val={0U};
static struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct Cyc_Absyn_RgnHandleCon_val={3U};
static struct Cyc_Absyn_TagCon_Absyn_TyCon_struct Cyc_Absyn_TagCon_val={4U};
static struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct Cyc_Absyn_HeapCon_val={5U};
static struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct Cyc_Absyn_UniqueCon_val={6U};
static struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct Cyc_Absyn_RefCntCon_val={7U};
static struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct Cyc_Absyn_AccessCon_val={8U};
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_JoinCon_val={9U};
static struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct Cyc_Absyn_RgnsCon_val={10U};
static struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct Cyc_Absyn_TrueCon_val={11U};
static struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct Cyc_Absyn_FalseCon_val={12U};
static struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct Cyc_Absyn_ThinCon_val={13U};
static struct Cyc_Absyn_FatCon_Absyn_TyCon_struct Cyc_Absyn_FatCon_val={14U};
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SCharCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_UCharCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NCharCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SShortCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_UShortCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NShortCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SIntCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_UIntCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NIntCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SLongCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ULongCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NLongCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_SLongLongCon_val={1U,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ULongLongCon_val={1U,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_NLongLongCon_val={1U,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
# 204
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_FloatCon_val={2U,0};static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_DoubleCon_val={2U,1};static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_LongDoubleCon_val={2U,2};
# 209
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0U,(void*)& Cyc_Absyn_VoidCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={0U,(void*)& Cyc_Absyn_HeapCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={0U,(void*)& Cyc_Absyn_UniqueCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={0U,(void*)& Cyc_Absyn_RefCntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_TrueType_val={0U,(void*)& Cyc_Absyn_TrueCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_FalseType_val={0U,(void*)& Cyc_Absyn_FalseCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_FatType_val={0U,(void*)& Cyc_Absyn_FatCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NCharType_val={0U,(void*)& Cyc_Absyn_NCharCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SCharType_val={0U,(void*)& Cyc_Absyn_SCharCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UCharType_val={0U,(void*)& Cyc_Absyn_UCharCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NShortType_val={0U,(void*)& Cyc_Absyn_NShortCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SShortType_val={0U,(void*)& Cyc_Absyn_SShortCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UShortType_val={0U,(void*)& Cyc_Absyn_UShortCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NIntType_val={0U,(void*)& Cyc_Absyn_NIntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SIntType_val={0U,(void*)& Cyc_Absyn_SIntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_UIntType_val={0U,(void*)& Cyc_Absyn_UIntCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NLongType_val={0U,(void*)& Cyc_Absyn_NLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SLongType_val={0U,(void*)& Cyc_Absyn_SLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ULongType_val={0U,(void*)& Cyc_Absyn_ULongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_NLongLongType_val={0U,(void*)& Cyc_Absyn_NLongLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_SLongLongType_val={0U,(void*)& Cyc_Absyn_SLongLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ULongLongType_val={0U,(void*)& Cyc_Absyn_ULongLongCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_FloatType_val={0U,(void*)& Cyc_Absyn_FloatCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_DoubleType_val={0U,(void*)& Cyc_Absyn_DoubleCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_LongDoubleType_val={0U,(void*)& Cyc_Absyn_LongDoubleCon_val,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_EmptyEffType_val={0U,(void*)& Cyc_Absyn_JoinCon_val,0};
# 237
void*Cyc_Absyn_void_type=(void*)& Cyc_Absyn_VoidType_val;
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->tag=2U,_tmp1D->f1=x;_tmp1D;});}
# 240
void*Cyc_Absyn_uchar_type=(void*)& Cyc_Absyn_UCharType_val;
void*Cyc_Absyn_ushort_type=(void*)& Cyc_Absyn_UShortType_val;
void*Cyc_Absyn_uint_type=(void*)& Cyc_Absyn_UIntType_val;
void*Cyc_Absyn_ulong_type=(void*)& Cyc_Absyn_ULongType_val;
void*Cyc_Absyn_ulonglong_type=(void*)& Cyc_Absyn_ULongLongType_val;
# 246
void*Cyc_Absyn_schar_type=(void*)& Cyc_Absyn_SCharType_val;
void*Cyc_Absyn_sshort_type=(void*)& Cyc_Absyn_SShortType_val;
void*Cyc_Absyn_sint_type=(void*)& Cyc_Absyn_SIntType_val;
void*Cyc_Absyn_slong_type=(void*)& Cyc_Absyn_SLongType_val;
void*Cyc_Absyn_slonglong_type=(void*)& Cyc_Absyn_SLongLongType_val;
# 252
void*Cyc_Absyn_char_type=(void*)& Cyc_Absyn_NCharType_val;
void*Cyc_Absyn_nchar_type=(void*)& Cyc_Absyn_NCharType_val;
void*Cyc_Absyn_nshort_type=(void*)& Cyc_Absyn_NShortType_val;
void*Cyc_Absyn_nint_type=(void*)& Cyc_Absyn_NIntType_val;
void*Cyc_Absyn_nlong_type=(void*)& Cyc_Absyn_NLongType_val;
void*Cyc_Absyn_nlonglong_type=(void*)& Cyc_Absyn_NLongLongType_val;
# 259
void*Cyc_Absyn_float_type=(void*)& Cyc_Absyn_FloatType_val;
void*Cyc_Absyn_double_type=(void*)& Cyc_Absyn_DoubleType_val;
void*Cyc_Absyn_long_double_type=(void*)& Cyc_Absyn_LongDoubleType_val;
void*Cyc_Absyn_gen_float_type(unsigned int i){
unsigned int _tmp1E=i;switch(_tmp1E){case 0U: _LL1: _LL2:
 return Cyc_Absyn_float_type;case 1U: _LL3: _LL4:
 return Cyc_Absyn_double_type;case 2U: _LL5: _LL6:
 return Cyc_Absyn_long_double_type;default: _LL7: _LL8:
({struct Cyc_Int_pa_PrintArg_struct _tmp21=({struct Cyc_Int_pa_PrintArg_struct _tmp15E;_tmp15E.tag=1U,_tmp15E.f1=(unsigned long)((int)i);_tmp15E;});void*_tmp1F[1U];_tmp1F[0]=& _tmp21;({struct _dyneither_ptr _tmp186=({const char*_tmp20="gen_float_type(%d)";_tag_dyneither(_tmp20,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp186,_tag_dyneither(_tmp1F,sizeof(void*),1U));});});}_LL0:;}
# 271
void*Cyc_Absyn_heap_rgn_type=(void*)& Cyc_Absyn_HeapRgn_val;
void*Cyc_Absyn_unique_rgn_type=(void*)& Cyc_Absyn_UniqueRgn_val;
void*Cyc_Absyn_refcnt_rgn_type=(void*)& Cyc_Absyn_RefCntRgn_val;
# 275
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_EmptyEffType_val;
# 277
void*Cyc_Absyn_true_type=(void*)& Cyc_Absyn_TrueType_val;
void*Cyc_Absyn_false_type=(void*)& Cyc_Absyn_FalseType_val;
# 280
void*Cyc_Absyn_fat_bound_type=(void*)& Cyc_Absyn_FatType_val;
void*Cyc_Absyn_thin_bounds_type(void*t){
return({void*_tmp22[1U];_tmp22[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_ThinCon_val,_tag_dyneither(_tmp22,sizeof(void*),1U));});}
# 284
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 287
void*Cyc_Absyn_thin_bounds_int(unsigned int i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 292
void*Cyc_Absyn_bounds_one(){
static void*bone=0;
void*_tmp23=bone;
if(_tmp23 == 0){
_tmp23=Cyc_Absyn_thin_bounds_int(1U);
bone=_tmp23;}
# 299
return _tmp23;}
# 301
void*Cyc_Absyn_tag_type(void*t){
return({void*_tmp24[1U];_tmp24[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_TagCon_val,_tag_dyneither(_tmp24,sizeof(void*),1U));});}
# 304
void*Cyc_Absyn_rgn_handle_type(void*r){
return({void*_tmp25[1U];_tmp25[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnHandleCon_val,_tag_dyneither(_tmp25,sizeof(void*),1U));});}
# 307
void*Cyc_Absyn_access_eff(void*r){
return({void*_tmp26[1U];_tmp26[0]=r;Cyc_Absyn_app_type((void*)& Cyc_Absyn_AccessCon_val,_tag_dyneither(_tmp26,sizeof(void*),1U));});}
# 310
void*Cyc_Absyn_join_eff(struct Cyc_List_List*ts){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->tag=0U,_tmp27->f1=(void*)& Cyc_Absyn_JoinCon_val,_tmp27->f2=ts;_tmp27;});}
# 313
void*Cyc_Absyn_regionsof_eff(void*t){
return({void*_tmp28[1U];_tmp28[0]=t;Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnsCon_val,_tag_dyneither(_tmp28,sizeof(void*),1U));});}
# 316
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=0U,({void*_tmp187=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=15U,_tmp29->f1=n,_tmp29->f2=d;_tmp29;});_tmp2A->f1=_tmp187;}),_tmp2A->f2=0;_tmp2A;});}
# 319
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,({void*_tmp188=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=16U,_tmp2B->f1=fs;_tmp2B;});_tmp2C->f1=_tmp188;}),_tmp2C->f2=0;_tmp2C;});}
# 322
void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=0U,({void*_tmp189=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=17U,_tmp2D->f1=s,_tmp2D->f2=k;_tmp2D;});_tmp2E->f1=_tmp189;}),_tmp2E->f2=0;_tmp2E;});}
# 325
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=0U,({void*_tmp18A=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=18U,_tmp2F->f1=di;_tmp2F;});_tmp30->f1=_tmp18A;}),_tmp30->f2=args;_tmp30;});}
# 328
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=0U,({void*_tmp18B=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=19U,_tmp31->f1=di;_tmp31;});_tmp32->f1=_tmp18B;}),_tmp32->f2=args;_tmp32;});}
# 331
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=0U,({void*_tmp18C=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=20U,_tmp33->f1=ai;_tmp33;});_tmp34->f1=_tmp18C;}),_tmp34->f2=args;_tmp34;});}
# 336
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
enum Cyc_Absyn_Sign _tmp35=sn;switch(_tmp35){case Cyc_Absyn_Signed: _LL1: _LL2: {
# 339
enum Cyc_Absyn_Size_of _tmp36=sz;switch(_tmp36){case Cyc_Absyn_Char_sz: _LLA: _LLB:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz: _LLC: _LLD:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz: _LLE: _LLF:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz: _LL10: _LL11:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
 goto _LL15;default: _LL14: _LL15:
 return Cyc_Absyn_slonglong_type;}_LL9:;}case Cyc_Absyn_Unsigned: _LL3: _LL4: {
# 348
enum Cyc_Absyn_Size_of _tmp37=sz;switch(_tmp37){case Cyc_Absyn_Char_sz: _LL17: _LL18:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz: _LL19: _LL1A:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz: _LL1B: _LL1C:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz: _LL1D: _LL1E:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 return Cyc_Absyn_ulonglong_type;}_LL16:;}case Cyc_Absyn_None: _LL5: _LL6:
# 356
 goto _LL8;default: _LL7: _LL8: {
# 358
enum Cyc_Absyn_Size_of _tmp38=sz;switch(_tmp38){case Cyc_Absyn_Char_sz: _LL24: _LL25:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz: _LL26: _LL27:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz: _LL28: _LL29:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz: _LL2A: _LL2B:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz: _LL2C: _LL2D:
 goto _LL2F;default: _LL2E: _LL2F:
 return Cyc_Absyn_nlonglong_type;}_LL23:;}}_LL0:;}
# 371
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 374
void*Cyc_Absyn_wchar_type(){
int _tmp39= Sizeof_wchar_t;switch(_tmp39){case 1U: _LL1: _LL2:
# 385 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_type: Cyc_Absyn_schar_type;case 2U: _LL3: _LL4:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_type: Cyc_Absyn_sshort_type;default: _LL5: _LL6:
# 389
 return Wchar_t_unsigned?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;}_LL0:;}static char _tmp3A[4U]="exn";
# 393
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp3A,_tmp3A,_tmp3A + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp3B[15U]="Null_Exception";
# 405 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp3B,_tmp3B,_tmp3B + 15U};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp3C[13U]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp3C,_tmp3C,_tmp3C + 13U};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp3D[16U]="Match_Exception";
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp3D,_tmp3D,_tmp3D + 16U};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp3E[10U]="Bad_alloc";
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp3E,_tmp3E,_tmp3E + 10U};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0U,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
# 411
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,& Cyc_Absyn_exn_l0};
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,& Cyc_Absyn_exn_l2};
# 416
static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)& Cyc_Absyn_exn_l3};
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,& Cyc_Absyn_exn_ol,1};
# 419
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
# 421
static void*Cyc_Absyn_exn_type_tt(){
static void*eopt=0;
if(eopt == 0)
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _tmp15F;(_tmp15F.KnownDatatype).tag=2U,(_tmp15F.KnownDatatype).val=& Cyc_Absyn_exn_tud;_tmp15F;}),0);
# 426
return eopt;}
# 429
void*Cyc_Absyn_exn_type(){
static void*exn_type_val=0;
if(exn_type_val == 0)
exn_type_val=({void*_tmp18F=Cyc_Absyn_exn_type_tt();void*_tmp18E=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp18D=
Cyc_Absyn_empty_tqual(0U);
# 432
Cyc_Absyn_at_type(_tmp18F,_tmp18E,_tmp18D,Cyc_Absyn_false_type);});
# 435
return exn_type_val;}static char _tmp3F[9U]="PrintArg";
# 439
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp3F,_tmp3F,_tmp3F + 9U};static char _tmp40[9U]="ScanfArg";
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp40,_tmp40,_tmp40 + 9U};
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 443
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 445
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp41[14U]="unique_region";
# 453
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp41,_tmp41,_tmp41 + 14U};static char _tmp42[5U]="Core";
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp42,_tmp42,_tmp42 + 5U};
static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)& Cyc_Absyn_Core_str,0};
# 457
static struct _tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,& Cyc_Absyn_uniquergn_nmspace}},& Cyc_Absyn_uniquergn_str};
# 459
struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
# 461
static struct Cyc_List_List Cyc_Absyn_unique_list={(void*)& Cyc_Absyn_UniqueRgn_val,0};static struct Cyc_List_List Cyc_Absyn_heap_list={(void*)& Cyc_Absyn_UniqueRgn_val,0};static struct Cyc_List_List Cyc_Absyn_refcnt_list={(void*)& Cyc_Absyn_RefCntRgn_val,0};
# 466
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uniquergnhandle_type={0U,(void*)& Cyc_Absyn_RgnHandleCon_val,& Cyc_Absyn_unique_list};
# 469
static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_uniquergnhandle_type,0,0,0,0};
# 472
static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={1U,& Cyc_Absyn_uniquergn_var_d};
# 474
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Absyn_uniquergn_exp_r={1U,(void*)& Cyc_Absyn_uniquergn_bind_p};
# 476
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergnhandle_type,(void*)& Cyc_Absyn_uniquergn_exp_r,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 479
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;
# 482
void*Cyc_Absyn_string_type(void*rgn){
return({void*_tmp193=Cyc_Absyn_char_type;void*_tmp192=rgn;struct Cyc_Absyn_Tqual _tmp191=Cyc_Absyn_empty_tqual(0U);void*_tmp190=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp193,_tmp192,_tmp191,_tmp190,Cyc_Absyn_true_type);});}
# 485
void*Cyc_Absyn_const_string_type(void*rgn){
return({void*_tmp197=Cyc_Absyn_char_type;void*_tmp196=rgn;struct Cyc_Absyn_Tqual _tmp195=Cyc_Absyn_const_tqual(0U);void*_tmp194=Cyc_Absyn_fat_bound_type;Cyc_Absyn_starb_type(_tmp197,_tmp196,_tmp195,_tmp194,Cyc_Absyn_true_type);});}
# 489
extern void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->tag=3U,_tmp43->f1=s;_tmp43;});}
# 494
void*Cyc_Absyn_starb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zeroterm){
# 496
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp160;_tmp160.elt_type=t,_tmp160.elt_tq=tq,
(_tmp160.ptr_atts).rgn=r,(_tmp160.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp160.ptr_atts).bounds=b,(_tmp160.ptr_atts).zero_term=zeroterm,(_tmp160.ptr_atts).ptrloc=0;_tmp160;}));}
# 502
void*Cyc_Absyn_atb_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,void*zeroterm){
# 504
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp161;_tmp161.elt_type=t,_tmp161.elt_tq=tq,
(_tmp161.ptr_atts).rgn=r,(_tmp161.ptr_atts).nullable=Cyc_Absyn_false_type,(_tmp161.ptr_atts).bounds=b,(_tmp161.ptr_atts).zero_term=zeroterm,(_tmp161.ptr_atts).ptrloc=0;_tmp161;}));}
# 511
void*Cyc_Absyn_star_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp162;_tmp162.elt_type=t,_tmp162.elt_tq=tq,
(_tmp162.ptr_atts).rgn=r,(_tmp162.ptr_atts).nullable=Cyc_Absyn_true_type,({
void*_tmp198=Cyc_Absyn_bounds_one();(_tmp162.ptr_atts).bounds=_tmp198;}),(_tmp162.ptr_atts).zero_term=zeroterm,(_tmp162.ptr_atts).ptrloc=0;_tmp162;}));}
# 518
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp163;_tmp163.elt_type=t,_tmp163.elt_tq=tq,
(_tmp163.ptr_atts).rgn=Cyc_Absyn_heap_rgn_type,(_tmp163.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 522
void*_tmp199=Cyc_Absyn_bounds_one();(_tmp163.ptr_atts).bounds=_tmp199;}),(_tmp163.ptr_atts).zero_term=Cyc_Absyn_false_type,(_tmp163.ptr_atts).ptrloc=0;_tmp163;}));}
# 526
void*Cyc_Absyn_at_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp164;_tmp164.elt_type=t,_tmp164.elt_tq=tq,
(_tmp164.ptr_atts).rgn=r,(_tmp164.ptr_atts).nullable=Cyc_Absyn_false_type,({
void*_tmp19A=Cyc_Absyn_bounds_one();(_tmp164.ptr_atts).bounds=_tmp19A;}),(_tmp164.ptr_atts).zero_term=zeroterm,(_tmp164.ptr_atts).ptrloc=0;_tmp164;}));}
# 533
void*Cyc_Absyn_fatptr_type(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*zeroterm){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp165;_tmp165.elt_type=t,_tmp165.elt_tq=tq,
(_tmp165.ptr_atts).rgn=r,(_tmp165.ptr_atts).nullable=Cyc_Absyn_true_type,(_tmp165.ptr_atts).bounds=Cyc_Absyn_fat_bound_type,(_tmp165.ptr_atts).zero_term=zeroterm,(_tmp165.ptr_atts).ptrloc=0;_tmp165;}));}
# 541
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc){
# 543
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=4U,(_tmp44->f1).elt_type=elt_type,(_tmp44->f1).tq=tq,(_tmp44->f1).num_elts=num_elts,(_tmp44->f1).zero_term=zero_term,(_tmp44->f1).zt_loc=ztloc;_tmp44;});}
# 546
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->tag=11U,_tmp45->f1=e;_tmp45;});}
# 551
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 553
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->tag=8U,_tmp46->f1=n,_tmp46->f2=args,_tmp46->f3=d,_tmp46->f4=defn;_tmp46;});}
# 559
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=0U,({void*_tmp19D=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=20U,({union Cyc_Absyn_AggrInfo _tmp19C=({enum Cyc_Absyn_AggrKind _tmp19B=k;Cyc_Absyn_UnknownAggr(_tmp19B,({struct _tuple0*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->f1=Cyc_Absyn_rel_ns_null,_tmp47->f2=name;_tmp47;}),0);});_tmp48->f1=_tmp19C;});_tmp48;});_tmp49->f1=_tmp19D;}),_tmp49->f2=0;_tmp49;});}
# 562
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggregate_type(Cyc_Absyn_UnionA,name);}
# 565
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),0);}
# 568
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),0);}
# 572
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){return({union Cyc_Absyn_Cnst _tmp166;(_tmp166.Char_c).tag=2U,((_tmp166.Char_c).val).f1=sn,((_tmp166.Char_c).val).f2=c;_tmp166;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp167;(_tmp167.Wchar_c).tag=3U,(_tmp167.Wchar_c).val=s;_tmp167;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){return({union Cyc_Absyn_Cnst _tmp168;(_tmp168.Short_c).tag=4U,((_tmp168.Short_c).val).f1=sn,((_tmp168.Short_c).val).f2=s;_tmp168;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){return({union Cyc_Absyn_Cnst _tmp169;(_tmp169.Int_c).tag=5U,((_tmp169.Int_c).val).f1=sn,((_tmp169.Int_c).val).f2=i;_tmp169;});}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){return({union Cyc_Absyn_Cnst _tmp16A;(_tmp16A.LongLong_c).tag=6U,((_tmp16A.LongLong_c).val).f1=sn,((_tmp16A.LongLong_c).val).f2=l;_tmp16A;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){return({union Cyc_Absyn_Cnst _tmp16B;(_tmp16B.Float_c).tag=7U,((_tmp16B.Float_c).val).f1=s,((_tmp16B.Float_c).val).f2=i;_tmp16B;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp16C;(_tmp16C.String_c).tag=8U,(_tmp16C.String_c).val=s;_tmp16C;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst _tmp16D;(_tmp16D.Wstring_c).tag=9U,(_tmp16D.Wstring_c).val=s;_tmp16D;});}
# 583
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->topt=0,_tmp4A->r=r,_tmp4A->loc=loc,_tmp4A->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp4A;});}
# 586
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp19E=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->tag=16U,_tmp4B->f1=rgn_handle,_tmp4B->f2=e;_tmp4B;});Cyc_Absyn_new_exp(_tmp19E,loc);});}
# 589
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp4C=_cycalloc(sizeof(*_tmp4C));*_tmp4C=*e;_tmp4C;});}
# 592
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
return({void*_tmp19F=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=0U,_tmp4D->f1=c;_tmp4D;});Cyc_Absyn_new_exp(_tmp19F,loc);});}
# 595
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0U,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 599
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){return({union Cyc_Absyn_Cnst _tmp1A0=Cyc_Absyn_Int_c(s,i);Cyc_Absyn_const_exp(_tmp1A0,seg);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0U,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 607
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0U,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == (unsigned int)0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == (unsigned int)1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 614
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp1A1=Cyc_Absyn_Char_c(Cyc_Absyn_None,c);Cyc_Absyn_const_exp(_tmp1A1,loc);});}
# 620
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int loc){
return({union Cyc_Absyn_Cnst _tmp1A2=Cyc_Absyn_Wchar_c(s);Cyc_Absyn_const_exp(_tmp1A2,loc);});}
# 623
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp1A3=Cyc_Absyn_Float_c(f,i);Cyc_Absyn_const_exp(_tmp1A3,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp1A4=Cyc_Absyn_String_c(s);Cyc_Absyn_const_exp(_tmp1A4,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int loc){return({union Cyc_Absyn_Cnst _tmp1A5=Cyc_Absyn_Wstring_c(s);Cyc_Absyn_const_exp(_tmp1A5,loc);});}
# 627
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){
return({void*_tmp1A7=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=1U,({void*_tmp1A6=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->tag=0U,_tmp4E->f1=q;_tmp4E;});_tmp4F->f1=_tmp1A6;});_tmp4F;});Cyc_Absyn_new_exp(_tmp1A7,loc);});}
# 630
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned int loc){
return({void*_tmp1A8=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=1U,_tmp50->f1=b;_tmp50;});Cyc_Absyn_new_exp(_tmp1A8,loc);});}
# 634
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
return Cyc_Absyn_var_exp(q,loc);}
# 637
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc){
return({void*_tmp1A9=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->tag=2U,_tmp51->f1=s;_tmp51;});Cyc_Absyn_new_exp(_tmp1A9,loc);});}
# 640
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp1AA=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=3U,_tmp52->f1=p,_tmp52->f2=es;_tmp52;});Cyc_Absyn_new_exp(_tmp1AA,loc);});}
# 643
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp1AC=p;struct Cyc_List_List*_tmp1AB=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=e,_tmp53->tl=0;_tmp53;});Cyc_Absyn_primop_exp(_tmp1AC,_tmp1AB,loc);});}
# 646
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({enum Cyc_Absyn_Primop _tmp1AF=p;struct Cyc_List_List*_tmp1AE=({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=e1,({struct Cyc_List_List*_tmp1AD=({struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=e2,_tmp54->tl=0;_tmp54;});_tmp55->tl=_tmp1AD;});_tmp55;});Cyc_Absyn_primop_exp(_tmp1AF,_tmp1AE,loc);});}
# 649
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp1B0=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=35U,_tmp56->f1=e1,_tmp56->f2=e2;_tmp56;});Cyc_Absyn_new_exp(_tmp1B0,loc);});}
# 652
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 662
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp1B1=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=4U,_tmp57->f1=e1,_tmp57->f2=popt,_tmp57->f3=e2;_tmp57;});Cyc_Absyn_new_exp(_tmp1B1,loc);});}
# 665
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 668
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
return({void*_tmp1B2=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=5U,_tmp58->f1=e,_tmp58->f2=i;_tmp58;});Cyc_Absyn_new_exp(_tmp1B2,loc);});}
# 671
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
return({void*_tmp1B3=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=6U,_tmp59->f1=e1,_tmp59->f2=e2,_tmp59->f3=e3;_tmp59;});Cyc_Absyn_new_exp(_tmp1B3,loc);});}
# 678
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp1B4=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=7U,_tmp5A->f1=e1,_tmp5A->f2=e2;_tmp5A;});Cyc_Absyn_new_exp(_tmp1B4,loc);});}
# 681
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp1B5=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->tag=8U,_tmp5B->f1=e1,_tmp5B->f2=e2;_tmp5B;});Cyc_Absyn_new_exp(_tmp1B5,loc);});}
# 684
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp1B6=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->tag=9U,_tmp5C->f1=e1,_tmp5C->f2=e2;_tmp5C;});Cyc_Absyn_new_exp(_tmp1B6,loc);});}
# 687
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp1B7=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=10U,_tmp5D->f1=e,_tmp5D->f2=es,_tmp5D->f3=0,_tmp5D->f4=0;_tmp5D;});Cyc_Absyn_new_exp(_tmp1B7,loc);});}
# 690
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp1B8=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=10U,_tmp5E->f1=e,_tmp5E->f2=es,_tmp5E->f3=0,_tmp5E->f4=1;_tmp5E;});Cyc_Absyn_new_exp(_tmp1B8,loc);});}
# 693
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1B9=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=12U,_tmp5F->f1=e;_tmp5F;});Cyc_Absyn_new_exp(_tmp1B9,loc);});}
# 696
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
return({void*_tmp1BA=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=13U,_tmp60->f1=e,_tmp60->f2=ts;_tmp60;});Cyc_Absyn_new_exp(_tmp1BA,loc);});}
# 699
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
return({void*_tmp1BB=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=14U,_tmp61->f1=t,_tmp61->f2=e,_tmp61->f3=user_cast,_tmp61->f4=c;_tmp61;});Cyc_Absyn_new_exp(_tmp1BB,loc);});}
# 702
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1BC=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->tag=11U,_tmp62->f1=e,_tmp62->f2=0;_tmp62;});Cyc_Absyn_new_exp(_tmp1BC,loc);});}
# 705
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1BD=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63->tag=11U,_tmp63->f1=e,_tmp63->f2=1;_tmp63;});Cyc_Absyn_new_exp(_tmp1BD,loc);});}
# 708
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp1BE=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->tag=15U,_tmp64->f1=e;_tmp64;});Cyc_Absyn_new_exp(_tmp1BE,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int loc){
return({void*_tmp1BF=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->tag=17U,_tmp65->f1=t;_tmp65;});Cyc_Absyn_new_exp(_tmp1BF,loc);});}
# 712
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1C0=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->tag=18U,_tmp66->f1=e;_tmp66;});Cyc_Absyn_new_exp(_tmp1C0,loc);});}
# 715
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned int loc){
return({void*_tmp1C1=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=19U,_tmp67->f1=t,_tmp67->f2=ofs;_tmp67;});Cyc_Absyn_new_exp(_tmp1C1,loc);});}
# 718
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp1C2=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->tag=20U,_tmp68->f1=e;_tmp68;});Cyc_Absyn_new_exp(_tmp1C2,loc);});}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp1C3=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->tag=21U,_tmp69->f1=e,_tmp69->f2=n,_tmp69->f3=0,_tmp69->f4=0;_tmp69;});Cyc_Absyn_new_exp(_tmp1C3,loc);});}
# 722
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
return({void*_tmp1C4=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=22U,_tmp6A->f1=e,_tmp6A->f2=n,_tmp6A->f3=0,_tmp6A->f4=0;_tmp6A;});Cyc_Absyn_new_exp(_tmp1C4,loc);});}
# 725
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({void*_tmp1C5=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->tag=23U,_tmp6B->f1=e1,_tmp6B->f2=e2;_tmp6B;});Cyc_Absyn_new_exp(_tmp1C5,loc);});}
# 728
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
return({void*_tmp1C6=(void*)({struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->tag=24U,_tmp6C->f1=es;_tmp6C;});Cyc_Absyn_new_exp(_tmp1C6,loc);});}
# 731
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){return({void*_tmp1C7=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));_tmp6D->tag=37U,_tmp6D->f1=s;_tmp6D;});Cyc_Absyn_new_exp(_tmp1C7,loc);});}
# 733
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 737
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
return({void*_tmp1C8=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=39U,_tmp6E->f1=t;_tmp6E;});Cyc_Absyn_new_exp(_tmp1C8,loc);});}
# 740
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
return({void*_tmp1C9=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=40U,_tmp6F->f1=volatile_kw,_tmp6F->f2=body;_tmp6F;});Cyc_Absyn_new_exp(_tmp1C9,loc);});}
# 743
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1CA=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=41U,_tmp70->f1=e;_tmp70;});Cyc_Absyn_new_exp(_tmp1CA,loc);});}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 747
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 749
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));({struct _tuple13*_tmp1CB=({struct _tuple13*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=0,_tmp71->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp71;});_tmp72->hd=_tmp1CB;}),_tmp72->tl=dles;_tmp72;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return({void*_tmp1CC=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=26U,_tmp73->f1=dles;_tmp73;});Cyc_Absyn_new_exp(_tmp1CC,loc);});}
# 755
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 758
return({void*_tmp1CD=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=36U,_tmp74->f1=n,_tmp74->f2=dles;_tmp74;});Cyc_Absyn_new_exp(_tmp1CD,loc);});}
# 761
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
return({struct Cyc_Absyn_Stmt*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->r=s,_tmp75->loc=loc,_tmp75->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_tmp75;});}
# 764
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0U};
struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6U};
struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7U};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){return({void*_tmp1CE=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=1U,_tmp76->f1=e;_tmp76;});Cyc_Absyn_new_stmt(_tmp1CE,loc);});}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return({struct Cyc_Absyn_Stmt*_tmp1D0=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_tmp1CF=Cyc_Absyn_seq_stmts(ss->tl,loc);Cyc_Absyn_seq_stmt(_tmp1D0,_tmp1CF,loc);});}}}
# 774
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1D1=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=3U,_tmp77->f1=e;_tmp77;});Cyc_Absyn_new_stmt(_tmp1D1,loc);});}
# 777
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
return({void*_tmp1D2=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=4U,_tmp78->f1=e,_tmp78->f2=s1,_tmp78->f3=s2;_tmp78;});Cyc_Absyn_new_stmt(_tmp1D2,loc);});}
# 780
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1D4=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=5U,(_tmp79->f1).f1=e,({struct Cyc_Absyn_Stmt*_tmp1D3=Cyc_Absyn_skip_stmt(e->loc);(_tmp79->f1).f2=_tmp1D3;}),_tmp79->f2=s;_tmp79;});Cyc_Absyn_new_stmt(_tmp1D4,loc);});}
# 783
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1D7=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=9U,_tmp7A->f1=e1,(_tmp7A->f2).f1=e2,({struct Cyc_Absyn_Stmt*_tmp1D6=Cyc_Absyn_skip_stmt(e3->loc);(_tmp7A->f2).f2=_tmp1D6;}),
(_tmp7A->f3).f1=e3,({struct Cyc_Absyn_Stmt*_tmp1D5=Cyc_Absyn_skip_stmt(e3->loc);(_tmp7A->f3).f2=_tmp1D5;}),_tmp7A->f4=s;_tmp7A;});
# 786
Cyc_Absyn_new_stmt(_tmp1D7,loc);});}
# 790
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp1D8=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=10U,_tmp7B->f1=e,_tmp7B->f2=scs,_tmp7B->f3=0;_tmp7B;});Cyc_Absyn_new_stmt(_tmp1D8,loc);});}struct _tuple14{void*f1;void*f2;};
# 793
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct _tuple14 _tmp7C=({struct _tuple14 _tmp16E;_tmp16E.f1=s1->r,_tmp16E.f2=s2->r;_tmp16E;});struct _tuple14 _tmp7D=_tmp7C;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7D.f1)->tag == 0U){_LL1: _LL2:
 return s2;}else{if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7D.f2)->tag == 0U){_LL3: _LL4:
 return s1;}else{_LL5: _LL6:
 return({void*_tmp1D9=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=2U,_tmp7E->f1=s1,_tmp7E->f2=s2;_tmp7E;});Cyc_Absyn_new_stmt(_tmp1D9,loc);});}}_LL0:;}
# 800
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
return({void*_tmp1DA=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=11U,_tmp7F->f1=el,_tmp7F->f2=0;_tmp7F;});Cyc_Absyn_new_stmt(_tmp1DA,loc);});}
# 803
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1DB=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=12U,_tmp80->f1=d,_tmp80->f2=s;_tmp80;});Cyc_Absyn_new_stmt(_tmp1DB,loc);});}
# 806
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Decl*d=({void*_tmp1DD=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=0U,({struct Cyc_Absyn_Vardecl*_tmp1DC=Cyc_Absyn_new_vardecl(0U,x,t,init);_tmp82->f1=_tmp1DC;});_tmp82;});Cyc_Absyn_new_decl(_tmp1DD,loc);});
return({void*_tmp1DE=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=12U,_tmp81->f1=d,_tmp81->f2=s;_tmp81;});Cyc_Absyn_new_stmt(_tmp1DE,loc);});}
# 810
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
return({void*_tmp1DF=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=13U,_tmp83->f1=v,_tmp83->f2=s;_tmp83;});Cyc_Absyn_new_stmt(_tmp1DF,loc);});}
# 813
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1E1=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=14U,_tmp84->f1=s,(_tmp84->f2).f1=e,({struct Cyc_Absyn_Stmt*_tmp1E0=Cyc_Absyn_skip_stmt(e->loc);(_tmp84->f2).f2=_tmp1E0;});_tmp84;});Cyc_Absyn_new_stmt(_tmp1E1,loc);});}
# 816
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
return({void*_tmp1E2=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=15U,_tmp85->f1=s,_tmp85->f2=scs,_tmp85->f3=0;_tmp85;});Cyc_Absyn_new_stmt(_tmp1E2,loc);});}
# 819
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
return({void*_tmp1E3=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=8U,_tmp86->f1=lab;_tmp86;});Cyc_Absyn_new_stmt(_tmp1E3,loc);});}
# 822
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return({struct Cyc_Absyn_Exp*_tmp1E4=Cyc_Absyn_assign_exp(e1,e2,loc);Cyc_Absyn_exp_stmt(_tmp1E4,loc);});}
# 826
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){return({struct Cyc_Absyn_Pat*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->r=p,_tmp87->topt=0,_tmp87->loc=s;_tmp87;});}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return({void*_tmp1E5=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=17U,_tmp88->f1=e;_tmp88;});Cyc_Absyn_new_pat(_tmp1E5,e->loc);});}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0U};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={9U};
# 832
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){return({struct Cyc_Absyn_Decl*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->r=r,_tmp89->loc=loc;_tmp89;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
return({void*_tmp1E6=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=2U,_tmp8A->f1=p,_tmp8A->f2=0,_tmp8A->f3=e,_tmp8A->f4=0;_tmp8A;});Cyc_Absyn_new_decl(_tmp1E6,loc);});}
# 836
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
return({void*_tmp1E7=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=3U,_tmp8B->f1=vds;_tmp8B;});Cyc_Absyn_new_decl(_tmp1E7,loc);});}
# 839
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned int loc){
return({void*_tmp1E8=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=4U,_tmp8C->f1=tv,_tmp8C->f2=vd,_tmp8C->f3=open_exp;_tmp8C;});Cyc_Absyn_new_decl(_tmp1E8,loc);});}
# 842
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 844
return({void*_tmp1EB=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->tag=2U,({struct Cyc_Absyn_Pat*_tmp1EA=({void*_tmp1E9=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->tag=2U,_tmp8D->f1=tv,_tmp8D->f2=vd;_tmp8D;});Cyc_Absyn_new_pat(_tmp1E9,loc);});_tmp8E->f1=_tmp1EA;}),_tmp8E->f2=0,_tmp8E->f3=e,_tmp8E->f4=0;_tmp8E;});Cyc_Absyn_new_decl(_tmp1EB,loc);});}
# 847
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->sc=Cyc_Absyn_Public,_tmp8F->name=x,_tmp8F->varloc=varloc,({
struct Cyc_Absyn_Tqual _tmp1EC=Cyc_Absyn_empty_tqual(0U);_tmp8F->tq=_tmp1EC;}),_tmp8F->type=t,_tmp8F->initializer=init,_tmp8F->rgn=0,_tmp8F->attributes=0,_tmp8F->escapes=0;_tmp8F;});}
# 853
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
return({struct Cyc_Absyn_Vardecl*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->sc=Cyc_Absyn_Static,_tmp90->name=x,_tmp90->varloc=0U,({struct Cyc_Absyn_Tqual _tmp1ED=Cyc_Absyn_empty_tqual(0U);_tmp90->tq=_tmp1ED;}),_tmp90->type=t,_tmp90->initializer=init,_tmp90->rgn=0,_tmp90->attributes=0,_tmp90->escapes=0;_tmp90;});}
# 858
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 862
return({struct Cyc_Absyn_AggrdeclImpl*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->exist_vars=exists,_tmp91->rgn_po=po,_tmp91->fields=fs,_tmp91->tagged=tagged;_tmp91;});}
# 865
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 868
return({void*_tmp1EF=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->tag=5U,({struct Cyc_Absyn_Aggrdecl*_tmp1EE=({struct Cyc_Absyn_Aggrdecl*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->kind=k,_tmp92->sc=s,_tmp92->name=n,_tmp92->tvs=ts,_tmp92->impl=i,_tmp92->attributes=atts,_tmp92->expected_mem_kind=0;_tmp92;});_tmp93->f1=_tmp1EE;});_tmp93;});Cyc_Absyn_new_decl(_tmp1EF,loc);});}
# 873
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 876
return({struct Cyc_Absyn_TypeDecl*_tmp96=_cycalloc(sizeof(*_tmp96));({void*_tmp1F1=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->tag=0U,({struct Cyc_Absyn_Aggrdecl*_tmp1F0=({struct Cyc_Absyn_Aggrdecl*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->kind=k,_tmp94->sc=s,_tmp94->name=n,_tmp94->tvs=ts,_tmp94->impl=i,_tmp94->attributes=atts,_tmp94->expected_mem_kind=0;_tmp94;});_tmp95->f1=_tmp1F0;});_tmp95;});_tmp96->r=_tmp1F1;}),_tmp96->loc=loc;_tmp96;});}
# 882
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 885
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 887
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 890
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 892
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 895
return({void*_tmp1F3=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp1F2=({struct Cyc_Absyn_Datatypedecl*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->sc=s,_tmp97->name=n,_tmp97->tvs=ts,_tmp97->fields=fs,_tmp97->is_extensible=is_extensible;_tmp97;});_tmp98->f1=_tmp1F2;});_tmp98;});Cyc_Absyn_new_decl(_tmp1F3,loc);});}
# 898
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 901
return({struct Cyc_Absyn_TypeDecl*_tmp9B=_cycalloc(sizeof(*_tmp9B));({void*_tmp1F5=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=2U,({struct Cyc_Absyn_Datatypedecl*_tmp1F4=({struct Cyc_Absyn_Datatypedecl*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->sc=s,_tmp99->name=n,_tmp99->tvs=ts,_tmp99->fields=fs,_tmp99->is_extensible=is_extensible;_tmp99;});_tmp9A->f1=_tmp1F4;});_tmp9A;});_tmp9B->r=_tmp1F5;}),_tmp9B->loc=loc;_tmp9B;});}
# 913 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
void*_tmp9C=(*a).f3;
void*_tmp9D=Cyc_Absyn_pointer_expand(_tmp9C,1);
if(_tmp9C != _tmp9D)
(*a).f3=_tmp9D;}
# 920
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 927
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=5U,(_tmp9E->f1).tvars=tvs,(_tmp9E->f1).ret_tqual=ret_tqual,({
# 930
void*_tmp1F6=Cyc_Absyn_pointer_expand(ret_type,0);(_tmp9E->f1).ret_type=_tmp1F6;}),(_tmp9E->f1).effect=eff_type,(_tmp9E->f1).args=args,(_tmp9E->f1).c_varargs=c_varargs,(_tmp9E->f1).cyc_varargs=cyc_varargs,(_tmp9E->f1).rgn_po=rgn_po,(_tmp9E->f1).attributes=atts,(_tmp9E->f1).requires_clause=req,(_tmp9E->f1).requires_relns=0,(_tmp9E->f1).ensures_clause=ens,(_tmp9E->f1).ensures_relns=0;_tmp9E;});}
# 944
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmp9F=Cyc_Tcutil_compress(t);void*_tmpA0=_tmp9F;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->tag == 5U){_LL1: _LL2:
 return({void*_tmp1F9=t;void*_tmp1F8=
fresh_evar?
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->v=& Cyc_Tcutil_rk;_tmpA1;}),0): Cyc_Absyn_heap_rgn_type;
# 946
struct Cyc_Absyn_Tqual _tmp1F7=
# 950
Cyc_Absyn_empty_tqual(0U);
# 946
Cyc_Absyn_at_type(_tmp1F9,_tmp1F8,_tmp1F7,Cyc_Absyn_false_type);});}else{_LL3: _LL4:
# 951
 return t;}_LL0:;}
# 966 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmpA2=e->r;void*_tmpA3=_tmpA2;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpA9;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Vardecl*_tmpA7;struct Cyc_Absyn_Vardecl*_tmpA6;switch(*((int*)_tmpA3)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA3)->f1)){case 2U: _LL1: _LL2:
# 969
 return 0;case 1U: _LL3: _tmpA6=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA3)->f1)->f1;_LL4:
 _tmpA7=_tmpA6;goto _LL6;case 4U: _LL5: _tmpA7=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA3)->f1)->f1;_LL6: {
# 972
void*_tmpA4=Cyc_Tcutil_compress(_tmpA7->type);void*_tmpA5=_tmpA4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA5)->tag == 4U){_LL18: _LL19:
 return 0;}else{_LL1A: _LL1B:
 return 1;}_LL17:;}default: _LL7: _LL8:
# 976
 goto _LLA;}case 22U: _LL9: _LLA:
 goto _LLC;case 20U: _LLB: _LLC:
 goto _LLE;case 23U: _LLD: _LLE:
 return 1;case 21U: _LLF: _tmpA8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA3)->f1;_LL10:
 return Cyc_Absyn_is_lvalue(_tmpA8);case 13U: _LL11: _tmpA9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA3)->f1;_LL12:
 return Cyc_Absyn_is_lvalue(_tmpA9);case 12U: _LL13: _tmpAA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA3)->f1;_LL14:
 return Cyc_Absyn_is_lvalue(_tmpAA);default: _LL15: _LL16:
 return 0;}_LL0:;}
# 987
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmpAB=fields;for(0;_tmpAB != 0;_tmpAB=_tmpAB->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmpAB->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmpAB->hd;}}
return 0;}
# 993
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}
# 997
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 1003
if(ts == 0)
return 0;
return(struct _tuple10*)ts->hd;}
# 1008
struct _dyneither_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
struct _dyneither_ptr*name;
{void*_tmpAC=decl->r;void*_tmpAD=_tmpAC;struct Cyc_Absyn_Fndecl*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC0;struct Cyc_Absyn_Typedefdecl*_tmpBF;struct Cyc_Absyn_Enumdecl*_tmpBE;struct Cyc_Absyn_Aggrdecl*_tmpBD;switch(*((int*)_tmpAD)){case 5U: _LL1: _tmpBD=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpAD)->f1;_LL2: {
# 1012
struct _tuple0*_tmpAE=_tmpBD->name;struct _tuple0*_tmpAF=_tmpAE;struct _dyneither_ptr*_tmpB0;_LL24: _tmpB0=_tmpAF->f2;_LL25:;
name=_tmpB0;
goto _LL0;}case 7U: _LL3: _tmpBE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpAD)->f1;_LL4: {
# 1016
struct _tuple0*_tmpB1=_tmpBE->name;struct _tuple0*_tmpB2=_tmpB1;struct _dyneither_ptr*_tmpB3;_LL27: _tmpB3=_tmpB2->f2;_LL28:;
name=_tmpB3;
goto _LL0;}case 8U: _LL5: _tmpBF=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpAD)->f1;_LL6: {
# 1020
struct _tuple0*_tmpB4=_tmpBF->name;struct _tuple0*_tmpB5=_tmpB4;struct _dyneither_ptr*_tmpB6;_LL2A: _tmpB6=_tmpB5->f2;_LL2B:;
name=_tmpB6;
goto _LL0;}case 0U: _LL7: _tmpC0=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpAD)->f1;_LL8: {
# 1024
struct _tuple0*_tmpB7=_tmpC0->name;struct _tuple0*_tmpB8=_tmpB7;struct _dyneither_ptr*_tmpB9;_LL2D: _tmpB9=_tmpB8->f2;_LL2E:;
name=_tmpB9;
goto _LL0;}case 1U: _LL9: _tmpC1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpAD)->f1;_LLA: {
# 1028
struct _tuple0*_tmpBA=_tmpC1->name;struct _tuple0*_tmpBB=_tmpBA;struct _dyneither_ptr*_tmpBC;_LL30: _tmpBC=_tmpBB->f2;_LL31:;
name=_tmpBC;
goto _LL0;}case 13U: _LLB: _LLC:
# 1032
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
# 1044
 name=0;
goto _LL0;}_LL0:;}
# 1047
return name;}
# 1051
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*name){
struct _dyneither_ptr*dname;
for(0;decls != 0;decls=decls->tl){
dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if((unsigned int)dname  && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 1058
return 0;}
# 1061
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
# 1080
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmpC2=a;struct _dyneither_ptr _tmp105;struct _dyneither_ptr _tmp104;int _tmp103;int _tmp102;int _tmp101;int _tmp100;int _tmpFF;int _tmpFE;int _tmpFD;struct _dyneither_ptr _tmpFC;struct Cyc_Absyn_Exp*_tmpFB;int _tmpFA;switch(*((int*)_tmpC2)){case 0U: _LL1: _tmpFA=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpC5=({struct Cyc_Int_pa_PrintArg_struct _tmp16F;_tmp16F.tag=1U,_tmp16F.f1=(unsigned long)_tmpFA;_tmp16F;});void*_tmpC3[1U];_tmpC3[0]=& _tmpC5;({struct _dyneither_ptr _tmp1FA=({const char*_tmpC4="regparm(%d)";_tag_dyneither(_tmpC4,sizeof(char),12U);});Cyc_aprintf(_tmp1FA,_tag_dyneither(_tmpC3,sizeof(void*),1U));});});case 1U: _LL3: _LL4:
 return({const char*_tmpC6="stdcall";_tag_dyneither(_tmpC6,sizeof(char),8U);});case 2U: _LL5: _LL6:
 return({const char*_tmpC7="cdecl";_tag_dyneither(_tmpC7,sizeof(char),6U);});case 3U: _LL7: _LL8:
 return({const char*_tmpC8="fastcall";_tag_dyneither(_tmpC8,sizeof(char),9U);});case 4U: _LL9: _LLA:
 return({const char*_tmpC9="noreturn";_tag_dyneither(_tmpC9,sizeof(char),9U);});case 5U: _LLB: _LLC:
 return({const char*_tmpCA="const";_tag_dyneither(_tmpCA,sizeof(char),6U);});case 6U: _LLD: _tmpFB=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LLE:
# 1089
 if(_tmpFB == 0)return({const char*_tmpCB="aligned";_tag_dyneither(_tmpCB,sizeof(char),8U);});else{
# 1091
enum Cyc_Cyclone_C_Compilers _tmpCC=Cyc_Cyclone_c_compiler;switch(_tmpCC){case Cyc_Cyclone_Gcc_c: _LL3A: _LL3B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpCF=({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0U,({struct _dyneither_ptr _tmp1FB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmpFB));_tmp170.f1=_tmp1FB;});_tmp170;});void*_tmpCD[1U];_tmpCD[0]=& _tmpCF;({struct _dyneither_ptr _tmp1FC=({const char*_tmpCE="aligned(%s)";_tag_dyneither(_tmpCE,sizeof(char),12U);});Cyc_aprintf(_tmp1FC,_tag_dyneither(_tmpCD,sizeof(void*),1U));});});case Cyc_Cyclone_Vc_c: _LL3C: _LL3D:
 goto _LL3F;default: _LL3E: _LL3F:
# 1095
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD2=({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0U,({struct _dyneither_ptr _tmp1FD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmpFB));_tmp171.f1=_tmp1FD;});_tmp171;});void*_tmpD0[1U];_tmpD0[0]=& _tmpD2;({struct _dyneither_ptr _tmp1FE=({const char*_tmpD1="align(%s)";_tag_dyneither(_tmpD1,sizeof(char),10U);});Cyc_aprintf(_tmp1FE,_tag_dyneither(_tmpD0,sizeof(void*),1U));});});}_LL39:;}case 7U: _LLF: _LL10:
# 1097
 return({const char*_tmpD3="packed";_tag_dyneither(_tmpD3,sizeof(char),7U);});case 8U: _LL11: _tmpFC=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LL12:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0U,_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFC);_tmp172;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({struct _dyneither_ptr _tmp1FF=({const char*_tmpD5="section(\"%s\")";_tag_dyneither(_tmpD5,sizeof(char),14U);});Cyc_aprintf(_tmp1FF,_tag_dyneither(_tmpD4,sizeof(void*),1U));});});case 9U: _LL13: _LL14:
 return({const char*_tmpD7="nocommon";_tag_dyneither(_tmpD7,sizeof(char),9U);});case 10U: _LL15: _LL16:
 return({const char*_tmpD8="shared";_tag_dyneither(_tmpD8,sizeof(char),7U);});case 11U: _LL17: _LL18:
 return({const char*_tmpD9="unused";_tag_dyneither(_tmpD9,sizeof(char),7U);});case 12U: _LL19: _LL1A:
 return({const char*_tmpDA="weak";_tag_dyneither(_tmpDA,sizeof(char),5U);});case 13U: _LL1B: _LL1C:
 return({const char*_tmpDB="dllimport";_tag_dyneither(_tmpDB,sizeof(char),10U);});case 14U: _LL1D: _LL1E:
 return({const char*_tmpDC="dllexport";_tag_dyneither(_tmpDC,sizeof(char),10U);});case 15U: _LL1F: _LL20:
 return({const char*_tmpDD="no_instrument_function";_tag_dyneither(_tmpDD,sizeof(char),23U);});case 16U: _LL21: _LL22:
 return({const char*_tmpDE="constructor";_tag_dyneither(_tmpDE,sizeof(char),12U);});case 17U: _LL23: _LL24:
 return({const char*_tmpDF="destructor";_tag_dyneither(_tmpDF,sizeof(char),11U);});case 18U: _LL25: _LL26:
 return({const char*_tmpE0="no_check_memory_usage";_tag_dyneither(_tmpE0,sizeof(char),22U);});case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC2)->f1 == Cyc_Absyn_Printf_ft){_LL27: _tmpFE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC2)->f2;_tmpFD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC2)->f3;_LL28:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE3=({struct Cyc_Int_pa_PrintArg_struct _tmp174;_tmp174.tag=1U,_tmp174.f1=(unsigned int)_tmpFE;_tmp174;});struct Cyc_Int_pa_PrintArg_struct _tmpE4=({struct Cyc_Int_pa_PrintArg_struct _tmp173;_tmp173.tag=1U,_tmp173.f1=(unsigned int)_tmpFD;_tmp173;});void*_tmpE1[2U];_tmpE1[0]=& _tmpE3,_tmpE1[1]=& _tmpE4;({struct _dyneither_ptr _tmp200=({const char*_tmpE2="format(printf,%u,%u)";_tag_dyneither(_tmpE2,sizeof(char),21U);});Cyc_aprintf(_tmp200,_tag_dyneither(_tmpE1,sizeof(void*),2U));});});}else{_LL29: _tmp100=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC2)->f2;_tmpFF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpC2)->f3;_LL2A:
# 1111
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE7=({struct Cyc_Int_pa_PrintArg_struct _tmp176;_tmp176.tag=1U,_tmp176.f1=(unsigned int)_tmp100;_tmp176;});struct Cyc_Int_pa_PrintArg_struct _tmpE8=({struct Cyc_Int_pa_PrintArg_struct _tmp175;_tmp175.tag=1U,_tmp175.f1=(unsigned int)_tmpFF;_tmp175;});void*_tmpE5[2U];_tmpE5[0]=& _tmpE7,_tmpE5[1]=& _tmpE8;({struct _dyneither_ptr _tmp201=({const char*_tmpE6="format(scanf,%u,%u)";_tag_dyneither(_tmpE6,sizeof(char),20U);});Cyc_aprintf(_tmp201,_tag_dyneither(_tmpE5,sizeof(void*),2U));});});}case 20U: _LL2B: _tmp101=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LL2C:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpEB=({struct Cyc_Int_pa_PrintArg_struct _tmp177;_tmp177.tag=1U,_tmp177.f1=(unsigned long)_tmp101;_tmp177;});void*_tmpE9[1U];_tmpE9[0]=& _tmpEB;({struct _dyneither_ptr _tmp202=({const char*_tmpEA="initializes(%d)";_tag_dyneither(_tmpEA,sizeof(char),16U);});Cyc_aprintf(_tmp202,_tag_dyneither(_tmpE9,sizeof(void*),1U));});});case 21U: _LL2D: _tmp102=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LL2E:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpEE=({struct Cyc_Int_pa_PrintArg_struct _tmp178;_tmp178.tag=1U,_tmp178.f1=(unsigned long)_tmp102;_tmp178;});void*_tmpEC[1U];_tmpEC[0]=& _tmpEE;({struct _dyneither_ptr _tmp203=({const char*_tmpED="noliveunique(%d)";_tag_dyneither(_tmpED,sizeof(char),17U);});Cyc_aprintf(_tmp203,_tag_dyneither(_tmpEC,sizeof(void*),1U));});});case 22U: _LL2F: _tmp103=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LL30:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF1=({struct Cyc_Int_pa_PrintArg_struct _tmp179;_tmp179.tag=1U,_tmp179.f1=(unsigned long)_tmp103;_tmp179;});void*_tmpEF[1U];_tmpEF[0]=& _tmpF1;({struct _dyneither_ptr _tmp204=({const char*_tmpF0="consume(%d)";_tag_dyneither(_tmpF0,sizeof(char),12U);});Cyc_aprintf(_tmp204,_tag_dyneither(_tmpEF,sizeof(void*),1U));});});case 23U: _LL31: _LL32:
 return({const char*_tmpF2="pure";_tag_dyneither(_tmpF2,sizeof(char),5U);});case 26U: _LL33: _LL34:
 return({const char*_tmpF3="always_inline";_tag_dyneither(_tmpF3,sizeof(char),14U);});case 24U: _LL35: _tmp104=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LL36:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF6=({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0U,_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp104);_tmp17A;});void*_tmpF4[1U];_tmpF4[0]=& _tmpF6;({struct _dyneither_ptr _tmp205=({const char*_tmpF5="__mode__(\"%s\")";_tag_dyneither(_tmpF5,sizeof(char),15U);});Cyc_aprintf(_tmp205,_tag_dyneither(_tmpF4,sizeof(void*),1U));});});default: _LL37: _tmp105=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)_tmpC2)->f1;_LL38:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF9=({struct Cyc_String_pa_PrintArg_struct _tmp17B;_tmp17B.tag=0U,_tmp17B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp105);_tmp17B;});void*_tmpF7[1U];_tmpF7[0]=& _tmpF9;({struct _dyneither_ptr _tmp206=({const char*_tmpF8="alias(\"%s\")";_tag_dyneither(_tmpF8,sizeof(char),12U);});Cyc_aprintf(_tmp206,_tag_dyneither(_tmpF7,sizeof(void*),1U));});});}_LL0:;}
# 1123
int Cyc_Absyn_fntype_att(void*a){
void*_tmp106=a;int _tmp107;switch(*((int*)_tmp106)){case 0U: _LL1: _tmp107=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp106)->f1;_LL2:
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
 return 0;}_LL0:;}
# 1140
static int Cyc_Absyn_attribute_case_number(void*att){
void*_tmp108=att;switch(*((int*)_tmp108)){case 0U: _LL1: _LL2:
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
# 1167
int Cyc_Absyn_attribute_cmp(void*att1,void*att2){
struct _tuple14 _tmp109=({struct _tuple14 _tmp17C;_tmp17C.f1=att1,_tmp17C.f2=att2;_tmp17C;});struct _tuple14 _tmp10A=_tmp109;enum Cyc_Absyn_Format_Type _tmp11A;int _tmp119;int _tmp118;enum Cyc_Absyn_Format_Type _tmp117;int _tmp116;int _tmp115;struct _dyneither_ptr _tmp114;struct _dyneither_ptr _tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;int _tmp110;int _tmp10F;int _tmp10E;int _tmp10D;switch(*((int*)_tmp10A.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10A.f2)->tag == 0U){_LL1: _tmp10E=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10A.f1)->f1;_tmp10D=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp10A.f2)->f1;_LL2:
 _tmp110=_tmp10E;_tmp10F=_tmp10D;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10A.f2)->tag == 20U){_LL3: _tmp110=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10A.f1)->f1;_tmp10F=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp10A.f2)->f1;_LL4:
# 1171
 return Cyc_Core_intcmp(_tmp110,_tmp10F);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10A.f2)->tag == 6U){_LL5: _tmp112=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10A.f1)->f1;_tmp111=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp10A.f2)->f1;_LL6:
# 1173
 if(_tmp112 == _tmp111)return 0;
if(_tmp112 == 0)return - 1;
if(_tmp111 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp112,_tmp111);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10A.f2)->tag == 8U){_LL7: _tmp114=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10A.f1)->f1;_tmp113=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp10A.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp114,(struct _dyneither_ptr)_tmp113);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10A.f2)->tag == 19U){_LL9: _tmp11A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10A.f1)->f1;_tmp119=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10A.f1)->f2;_tmp118=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10A.f1)->f3;_tmp117=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10A.f2)->f1;_tmp116=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10A.f2)->f2;_tmp115=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp10A.f2)->f3;_LLA: {
# 1179
int _tmp10B=Cyc_Core_intcmp((int)((unsigned int)_tmp11A),(int)((unsigned int)_tmp117));
if(_tmp10B != 0)return _tmp10B;{
int _tmp10C=Cyc_Core_intcmp(_tmp119,_tmp116);
if(_tmp10C != 0)return _tmp10C;
return Cyc_Core_intcmp(_tmp118,_tmp115);};}}else{goto _LLB;}default: _LLB: _LLC:
# 1185
 return({int _tmp207=Cyc_Absyn_attribute_case_number(att1);Cyc_Core_intcmp(_tmp207,Cyc_Absyn_attribute_case_number(att2));});}_LL0:;}
# 1189
int Cyc_Absyn_equal_att(void*a1,void*a2){
return Cyc_Absyn_attribute_cmp(a1,a2)== 0;}static char _tmp11B[3U]="f0";
# 1194
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp11B,_tmp11B,_tmp11B + 3U};
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1U]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1U)};
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
if((unsigned int)i >= fsz)
Cyc_Absyn_field_names=({unsigned int _tmp121=(unsigned int)(i + 1);struct _dyneither_ptr**_tmp120=_cycalloc(_check_times(_tmp121,sizeof(struct _dyneither_ptr*)));({{unsigned int _tmp17D=(unsigned int)(i + 1);unsigned int j;for(j=0;j < _tmp17D;++ j){
# 1202
j < fsz?_tmp120[j]=*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp20A=({struct _dyneither_ptr*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct _dyneither_ptr _tmp209=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11E=({struct Cyc_Int_pa_PrintArg_struct _tmp17E;_tmp17E.tag=1U,_tmp17E.f1=(unsigned long)((int)j);_tmp17E;});void*_tmp11C[1U];_tmp11C[0]=& _tmp11E;({struct _dyneither_ptr _tmp208=({const char*_tmp11D="f%d";_tag_dyneither(_tmp11D,sizeof(char),4U);});Cyc_aprintf(_tmp208,_tag_dyneither(_tmp11C,sizeof(void*),1U));});});*_tmp11F=_tmp209;});_tmp11F;});_tmp120[j]=_tmp20A;});}}0;});_tag_dyneither(_tmp120,sizeof(struct _dyneither_ptr*),_tmp121);});
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1206
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmp122=info;enum Cyc_Absyn_AggrKind _tmp126;struct _tuple0*_tmp125;enum Cyc_Absyn_AggrKind _tmp124;struct _tuple0*_tmp123;if((_tmp122.UnknownAggr).tag == 1){_LL1: _tmp124=((_tmp122.UnknownAggr).val).f1;_tmp123=((_tmp122.UnknownAggr).val).f2;_LL2:
 return({struct _tuple11 _tmp17F;_tmp17F.f1=_tmp124,_tmp17F.f2=_tmp123;_tmp17F;});}else{_LL3: _tmp126=(*(_tmp122.KnownAggr).val)->kind;_tmp125=(*(_tmp122.KnownAggr).val)->name;_LL4:
 return({struct _tuple11 _tmp180;_tmp180.f1=_tmp126,_tmp180.f2=_tmp125;_tmp180;});}_LL0:;}
# 1212
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
union Cyc_Absyn_AggrInfo _tmp127=info;struct Cyc_Absyn_Aggrdecl*_tmp12A;if((_tmp127.UnknownAggr).tag == 1){_LL1: _LL2:
({void*_tmp128=0U;({struct _dyneither_ptr _tmp20B=({const char*_tmp129="unchecked aggrdecl";_tag_dyneither(_tmp129,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp20B,_tag_dyneither(_tmp128,sizeof(void*),0U));});});}else{_LL3: _tmp12A=*(_tmp127.KnownAggr).val;_LL4:
 return _tmp12A;}_LL0:;}
# 1218
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp12B=Cyc_Tcutil_compress(t);void*_tmp12C=_tmp12B;union Cyc_Absyn_AggrInfo _tmp135;struct Cyc_List_List*_tmp134;switch(*((int*)_tmp12C)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12C)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp134=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12C)->f2;_LL2:
# 1221
 if(_tmp134 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp134->hd)->requires_clause == 0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12C)->f1)->tag == 20U){_LL3: _tmp135=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12C)->f1)->f1;_LL4: {
# 1224
union Cyc_Absyn_AggrInfo _tmp12D=_tmp135;enum Cyc_Absyn_AggrKind _tmp133;int _tmp132;enum Cyc_Absyn_AggrKind _tmp131;struct Cyc_Absyn_Aggrdecl*_tmp130;if((_tmp12D.KnownAggr).tag == 2){_LL8: _tmp130=*(_tmp12D.KnownAggr).val;_LL9:
# 1226
 if((int)_tmp130->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp12E=_tmp130->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp12E))->tagged)return 0;{
struct Cyc_List_List*_tmp12F=_tmp12E->fields;
if(_tmp12F == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp12F->hd)->requires_clause == 0;};};}else{if(((_tmp12D.UnknownAggr).val).f3 == 0){_LLA: _tmp131=((_tmp12D.UnknownAggr).val).f1;_LLB:
 return(int)_tmp131 == (int)Cyc_Absyn_UnionA;}else{_LLC: _tmp133=((_tmp12D.UnknownAggr).val).f1;_tmp132=(int)(((_tmp12D.UnknownAggr).val).f3)->v;_LLD:
 return(int)_tmp133 == (int)1U  && !_tmp132;}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1235
 return 0;}_LL0:;}
# 1238
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp136=Cyc_Tcutil_compress(t);void*_tmp137=_tmp136;union Cyc_Absyn_AggrInfo _tmp140;struct Cyc_List_List*_tmp13F;switch(*((int*)_tmp137)){case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp137)->f1 == Cyc_Absyn_UnionA){_LL1: _tmp13F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp137)->f2;_LL2:
# 1241
 if(_tmp13F == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp13F->hd)->requires_clause != 0;}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)->tag == 20U){_LL3: _tmp140=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp137)->f1)->f1;_LL4: {
# 1244
union Cyc_Absyn_AggrInfo _tmp138=_tmp140;enum Cyc_Absyn_AggrKind _tmp13E;int _tmp13D;enum Cyc_Absyn_AggrKind _tmp13C;struct Cyc_Absyn_Aggrdecl*_tmp13B;if((_tmp138.KnownAggr).tag == 2){_LL8: _tmp13B=*(_tmp138.KnownAggr).val;_LL9:
# 1246
 if((int)_tmp13B->kind != (int)Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp139=_tmp13B->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp139))->tagged)return 0;{
struct Cyc_List_List*_tmp13A=_tmp139->fields;
if(_tmp13A == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp13A->hd)->requires_clause != 0;};};}else{if(((_tmp138.UnknownAggr).val).f3 == 0){_LLA: _tmp13C=((_tmp138.UnknownAggr).val).f1;_LLB:
 return 0;}else{_LLC: _tmp13E=((_tmp138.UnknownAggr).val).f1;_tmp13D=(int)(((_tmp138.UnknownAggr).val).f3)->v;_LLD:
 return 0;}}_LL7:;}}else{goto _LL5;}default: _LL5: _LL6:
# 1255
 return 0;}_LL0:;}
# 1259
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_tmp141=b;struct Cyc_Absyn_Fndecl*_tmp147;struct Cyc_Absyn_Vardecl*_tmp146;struct Cyc_Absyn_Vardecl*_tmp145;struct Cyc_Absyn_Vardecl*_tmp144;struct Cyc_Absyn_Vardecl*_tmp143;struct _tuple0*_tmp142;switch(*((int*)_tmp141)){case 0U: _LL1: _tmp142=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp141)->f1;_LL2:
 return _tmp142;case 1U: _LL3: _tmp143=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp141)->f1;_LL4:
 _tmp144=_tmp143;goto _LL6;case 3U: _LL5: _tmp144=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp141)->f1;_LL6:
 _tmp145=_tmp144;goto _LL8;case 4U: _LL7: _tmp145=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp141)->f1;_LL8:
 _tmp146=_tmp145;goto _LLA;case 5U: _LL9: _tmp146=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp141)->f1;_LLA:
 return _tmp146->name;default: _LLB: _tmp147=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp141)->f1;_LLC:
 return _tmp147->name;}_LL0:;}
# 1270
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds == 0  || ds->tl != 0)
({void*_tmp148=0U;({struct _dyneither_ptr _tmp20C=({const char*_tmp149="designator list not of length 1";_tag_dyneither(_tmp149,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp20C,_tag_dyneither(_tmp148,sizeof(void*),0U));});});{
void*_tmp14A=(void*)ds->hd;void*_tmp14B=_tmp14A;struct _dyneither_ptr*_tmp14E;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp14B)->tag == 1U){_LL1: _tmp14E=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp14B)->f1;_LL2:
 return _tmp14E;}else{_LL3: _LL4:
({void*_tmp14C=0U;({struct _dyneither_ptr _tmp20D=({const char*_tmp14D="array designator in struct";_tag_dyneither(_tmp14D,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp20D,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});}_LL0:;};}
# 1279
int Cyc_Absyn_type2bool(int def,void*t){
void*_tmp14F=Cyc_Tcutil_compress(t);void*_tmp150=_tmp14F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp150)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp150)->f1)){case 11U: _LL1: _LL2:
 return 1;case 12U: _LL3: _LL4:
 return 0;default: goto _LL5;}else{_LL5: _LL6:
 return def;}_LL0:;}
# 1287
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13U};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14U};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15U};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16U};
# 1292
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;
