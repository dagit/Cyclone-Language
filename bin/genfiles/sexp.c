#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_*.c */
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/* Need one of these per thread (see runtime_stack.c). The runtime maintains 
   a stack that contains either _handler_cons structs or _RegionHandle structs.
   The tag is 0 for a handler_cons and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; 
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Regions */
struct _RegionPage
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
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
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons *);
void _push_region(struct _RegionHandle *);
void _npop_handler(int);
void _pop_handler();
void _pop_region();

#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
     if (!_cks_null) _throw_null(); \
     _cks_null; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char*)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr);\
  int _index = (index);\
  if (!_cks_ptr) _throw_null(); \
  _cks_ptr + (elt_sz)*_index; })
#endif
#define _zero_arr_plus_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_char_fn _zero_arr_plus_fn
#define _zero_arr_plus_short_fn _zero_arr_plus_fn
#define _zero_arr_plus_int_fn _zero_arr_plus_fn
#define _zero_arr_plus_float_fn _zero_arr_plus_fn
#define _zero_arr_plus_double_fn _zero_arr_plus_fn
#define _zero_arr_plus_longdouble_fn _zero_arr_plus_fn
#define _zero_arr_plus_voidstar_fn _zero_arr_plus_fn
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })

/* _zero_arr_plus_*_fn(x,sz,i,filename,lineno) adds i to zero-terminated ptr
   x that has at least sz elements */
char* _zero_arr_plus_char_fn(char*,unsigned,int,const char*,unsigned);
short* _zero_arr_plus_short_fn(short*,unsigned,int,const char*,unsigned);
int* _zero_arr_plus_int_fn(int*,unsigned,int,const char*,unsigned);
float* _zero_arr_plus_float_fn(float*,unsigned,int,const char*,unsigned);
double* _zero_arr_plus_double_fn(double*,unsigned,int,const char*,unsigned);
long double* _zero_arr_plus_longdouble_fn(long double*,unsigned,int,const char*, unsigned);
void** _zero_arr_plus_voidstar_fn(void**,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
int _get_zero_arr_size_char(const char*,unsigned);
int _get_zero_arr_size_short(const short*,unsigned);
int _get_zero_arr_size_int(const int*,unsigned);
int _get_zero_arr_size_float(const float*,unsigned);
int _get_zero_arr_size_double(const double*,unsigned);
int _get_zero_arr_size_longdouble(const long double*,unsigned);
int _get_zero_arr_size_voidstar(const void**,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_short_fn(short**,int,const char*,unsigned);
int* _zero_arr_inplace_plus_int(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_longdouble_fn(long double**,int,const char*,unsigned);
void** _zero_arr_inplace_plus_voidstar_fn(void***,int,const char*,unsigned);
/* like the previous functions, but does post-addition (as in e++) */
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_post_short_fn(short**x,int,const char*,unsigned);
int* _zero_arr_inplace_plus_post_int_fn(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_post_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_post_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_post_longdouble_fn(long double**,int,const char *,unsigned);
void** _zero_arr_inplace_plus_post_voidstar_fn(void***,int,const char*,unsigned);
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

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
#endif

#define _tag_fat(tcurr,elt_sz,num_elts) ({ \
  struct _fat_ptr _ans; \
  unsigned _num_elts = (num_elts);\
  _ans.base = _ans.curr = (void*)(tcurr); \
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _ans.last_plus_one = _ans.base ? (_ans.base + (elt_sz) * _num_elts) : 0; \
  _ans; })

#define _get_fat_size(arr,elt_sz) \
  ({struct _fat_ptr _arr = (arr); \
    unsigned char *_arr_curr=_arr.curr; \
    unsigned char *_arr_last=_arr.last_plus_one; \
    (_arr_curr < _arr.base || _arr_curr >= _arr_last) ? 0 : \
    ((_arr_last - _arr_curr) / (elt_sz));})

#define _fat_ptr_plus(arr,elt_sz,change) ({ \
  struct _fat_ptr _ans = (arr); \
  int _change = (change);\
  _ans.curr += (elt_sz) * _change;\
  _ans; })
#define _fat_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += (elt_sz) * (change);\
  *_arr_ptr; })
#define _fat_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  struct _fat_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += (elt_sz) * (change);\
  _ans; })

//Not a macro since initialization order matters. Defined in runtime_zeroterm.c.
struct _fat_ptr _fat_ptr_decrease_size(struct _fat_ptr,unsigned sz,unsigned numelts);

/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);
// bound the allocation size to be < MAX_ALLOC_SIZE. See macros below for usage.
#define MAX_MALLOC_SIZE (1 << 28)
void* _bounded_GC_malloc(int,const char*,int);
void* _bounded_GC_malloc_atomic(int,const char*,int);
void* _bounded_GC_calloc(unsigned,unsigned,const char*,int);
void* _bounded_GC_calloc_atomic(unsigned,unsigned,const char*,int);
/* these macros are overridden below ifdef CYC_REGION_PROFILE */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)
#endif

static inline unsigned int _check_times(unsigned x, unsigned y) {
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 142 "cycboot.h"
extern int Cyc_getc(struct Cyc___cycFILE*);
# 222 "cycboot.h"
extern int Cyc_ungetc(int,struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;
# 292 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);struct Cyc_Sexp_Class;struct Cyc_Sexp_Obj;struct Cyc_Sexp_Object;struct Cyc_Sexp_Visitor;struct Cyc_Sexp_Parser{void*env;int(*getc)(void*);int(*ungetc)(int,void*);void(*error)(void*,int,struct _fat_ptr);};struct Cyc_Sexp_Printer{void*env;void(*print)(void*,struct _fat_ptr);};struct Cyc_Sexp_Class{struct Cyc_Sexp_Obj*cast_value;char tag;struct _fat_ptr name;void(*print)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*);struct Cyc_Sexp_Obj*(*parse)(struct Cyc_Sexp_Parser*);int(*cmp)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*);int(*hash)(struct Cyc_Sexp_Obj*);void*(*accept)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*);struct Cyc_Hashtable_Table*hash_table;};struct Cyc_Sexp_Obj{struct Cyc_Sexp_Class*vtable;void*v[0U] __attribute__((aligned )) ;};struct Cyc_Sexp_Object{struct Cyc_Sexp_Obj*self;};
# 101 "sexp.h"
struct Cyc_Sexp_Object Cyc_Sexp_up(struct Cyc_Sexp_Obj*);
# 105
struct Cyc_Sexp_Obj*Cyc_Sexp_down(struct Cyc_Sexp_Class*,struct Cyc_Sexp_Object);
# 108
extern struct Cyc_Sexp_Class Cyc_Sexp_uchar_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_schar_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_ushort_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_sshort_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_uint_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_sint_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_ulonglong_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_slonglong_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_float_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_double_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_str_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_symbol_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_tuple_class;struct _tuple0{struct Cyc_Sexp_Class*vtable;unsigned char v  __attribute__((aligned )) ;};
# 123
struct _tuple0*Cyc_Sexp_mk_uchar(unsigned char);struct _tuple1{struct Cyc_Sexp_Class*vtable;signed char v  __attribute__((aligned )) ;};
struct _tuple1*Cyc_Sexp_mk_schar(signed char);struct _tuple2{struct Cyc_Sexp_Class*vtable;unsigned short v  __attribute__((aligned )) ;};
struct _tuple2*Cyc_Sexp_mk_ushort(unsigned short);struct _tuple3{struct Cyc_Sexp_Class*vtable;short v  __attribute__((aligned )) ;};
struct _tuple3*Cyc_Sexp_mk_sshort(short);struct _tuple4{struct Cyc_Sexp_Class*vtable;unsigned v  __attribute__((aligned )) ;};
struct _tuple4*Cyc_Sexp_mk_uint(unsigned);struct _tuple5{struct Cyc_Sexp_Class*vtable;int v  __attribute__((aligned )) ;};
struct _tuple5*Cyc_Sexp_mk_sint(int);struct _tuple6{struct Cyc_Sexp_Class*vtable;unsigned long long v  __attribute__((aligned )) ;};
struct _tuple6*Cyc_Sexp_mk_ulonglong(unsigned long long);struct _tuple7{struct Cyc_Sexp_Class*vtable;long long v  __attribute__((aligned )) ;};
struct _tuple7*Cyc_Sexp_mk_slonglong(long long);struct _tuple8{struct Cyc_Sexp_Class*vtable;float v  __attribute__((aligned )) ;};
struct _tuple8*Cyc_Sexp_mk_float(float);struct _tuple9{struct Cyc_Sexp_Class*vtable;double v  __attribute__((aligned )) ;};
struct _tuple9*Cyc_Sexp_mk_double(double);struct _tuple10{struct Cyc_Sexp_Class*vtable;struct _fat_ptr v  __attribute__((aligned )) ;};
struct _tuple10*Cyc_Sexp_mk_str(struct _fat_ptr);
struct _tuple10*Cyc_Sexp_mk_symbol(struct _fat_ptr);
struct _tuple10*Cyc_Sexp_mk_tuple(struct _fat_ptr);
# 150
struct Cyc_Sexp_Object Cyc_Sexp_obj_tuple(struct _fat_ptr);struct Cyc_Sexp_Visitor{void*(*visit_uchar)(void*,struct _tuple0*,struct Cyc_Sexp_Visitor*);void*(*visit_schar)(void*,struct _tuple1*,struct Cyc_Sexp_Visitor*);void*(*visit_ushort)(void*,struct _tuple2*,struct Cyc_Sexp_Visitor*);void*(*visit_sshort)(void*,struct _tuple3*,struct Cyc_Sexp_Visitor*);void*(*visit_uint)(void*,struct _tuple4*,struct Cyc_Sexp_Visitor*);void*(*visit_sint)(void*,struct _tuple5*,struct Cyc_Sexp_Visitor*);void*(*visit_ulonglong)(void*,struct _tuple6*,struct Cyc_Sexp_Visitor*);void*(*visit_slonglong)(void*,struct _tuple7*,struct Cyc_Sexp_Visitor*);void*(*visit_float)(void*,struct _tuple8*,struct Cyc_Sexp_Visitor*);void*(*visit_double)(void*,struct _tuple9*,struct Cyc_Sexp_Visitor*);void*(*visit_symbol)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*);void*(*visit_str)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*);void*(*visit_tuple)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*);void*(*visit_default)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*);};
# 181
struct Cyc_Sexp_Visitor Cyc_Sexp_empty_visitor (void);
# 187
int Cyc_Sexp_cmp(struct Cyc_Sexp_Object,struct Cyc_Sexp_Object);
# 192
void Cyc_Sexp_print(struct Cyc_Sexp_Printer*,struct Cyc_Sexp_Object);
# 195
struct Cyc_Sexp_Object Cyc_Sexp_parse(struct Cyc_Sexp_Parser*);
# 198
void Cyc_Sexp_tofile(struct Cyc___cycFILE*,struct Cyc_Sexp_Object);
# 203
struct Cyc_Sexp_Object Cyc_Sexp_fromfile(struct Cyc___cycFILE*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 54 "xarray.h"
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty (void);
# 66
extern void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 104
extern void Cyc_Xarray_iter_c(void(*)(void*,void*),void*,struct Cyc_Xarray_Xarray*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 55 "sexp.cyc"
struct Cyc_Sexp_Object Cyc_Sexp_up(struct Cyc_Sexp_Obj*self){
return({struct Cyc_Sexp_Object _tmpF9;_tmpF9.self=self;_tmpF9;});}
# 60
struct Cyc_Sexp_Obj*Cyc_Sexp_down(struct Cyc_Sexp_Class*B,struct Cyc_Sexp_Object x){
struct Cyc_Sexp_Object _tmp0=x;void*_tmp1;_tmp1=_tmp0.self;{struct Cyc_Sexp_Obj*self=_tmp1;
B->cast_value=0;
(self->vtable)->cast_value=self;
return B->cast_value;}}
# 68
int Cyc_Sexp_hash(struct Cyc_Sexp_Object self){
struct Cyc_Sexp_Object _tmp2=self;void*_tmp3;_tmp3=_tmp2.self;{struct Cyc_Sexp_Obj*self=_tmp3;
return(((self->vtable)->hash))(self);}}
# 74
int Cyc_Sexp_cmp(struct Cyc_Sexp_Object x,struct Cyc_Sexp_Object y){
struct Cyc_Sexp_Object _tmp4=x;void*_tmp5;_tmp5=_tmp4.self;{struct Cyc_Sexp_Obj*xself=_tmp5;
struct Cyc_Sexp_Object _tmp6=y;void*_tmp7;_tmp7=_tmp6.self;{struct Cyc_Sexp_Obj*yself=_tmp7;
if((void*)xself == (void*)yself)return 0;{
# 79
int diff=(int)xself->vtable - (int)yself->vtable;
if(diff != 0)return diff;{
# 82
struct Cyc_Sexp_Obj*_tmp8=Cyc_Sexp_down(xself->vtable,y);struct Cyc_Sexp_Obj*yasx=_tmp8;
return(((xself->vtable)->cmp))(xself,_check_null(yasx));}}}}}
# 87
void*Cyc_Sexp_visit(struct Cyc_Sexp_Object x,struct Cyc_Sexp_Visitor*v,void*env){
struct Cyc_Sexp_Object _tmp9=x;void*_tmpA;_tmpA=_tmp9.self;{struct Cyc_Sexp_Obj*xself=_tmpA;
return(((xself->vtable)->accept))(xself,v,env);}}
# 93
void Cyc_Sexp_print(struct Cyc_Sexp_Printer*p,struct Cyc_Sexp_Object x){
struct Cyc_Sexp_Object _tmpB=x;void*_tmpC;_tmpC=_tmpB.self;{struct Cyc_Sexp_Obj*xself=_tmpC;
({void(*_tmp134)(void*,struct _fat_ptr)=p->print;void*_tmp133=p->env;_tmp134(_tmp133,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmpFA;_tmpFA.tag=1,_tmpFA.f1=(unsigned long)((int)(xself->vtable)->tag);_tmpFA;});void*_tmpD[1];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp132=({const char*_tmpE="%c(";_tag_fat(_tmpE,sizeof(char),4U);});Cyc_aprintf(_tmp132,_tag_fat(_tmpD,sizeof(void*),1));});}));});
(((xself->vtable)->print))(xself,p);
({void(*_tmp136)(void*,struct _fat_ptr)=p->print;void*_tmp135=p->env;_tmp136(_tmp135,({const char*_tmp10=")";_tag_fat(_tmp10,sizeof(char),2U);}));});}}
# 102
static void Cyc_Sexp_printfile(struct Cyc___cycFILE*f,struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0,_tmpFB.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmpFB;});void*_tmp11[1];_tmp11[0]=& _tmp13;({struct Cyc___cycFILE*_tmp138=f;struct _fat_ptr _tmp137=({const char*_tmp12="%s";_tag_fat(_tmp12,sizeof(char),3U);});Cyc_fprintf(_tmp138,_tmp137,_tag_fat(_tmp11,sizeof(void*),1));});});}
# 107
void Cyc_Sexp_tofile(struct Cyc___cycFILE*f,struct Cyc_Sexp_Object x){
struct Cyc_Sexp_Printer _tmp14=({struct Cyc_Sexp_Printer _tmpFC;_tmpFC.env=f,_tmpFC.print=Cyc_Sexp_printfile;_tmpFC;});struct Cyc_Sexp_Printer p=_tmp14;
({(void(*)(struct Cyc_Sexp_Printer*,struct Cyc_Sexp_Object))Cyc_Sexp_print;})(& p,x);}
# 113
void Cyc_Sexp_tofilename(const char*filename,struct Cyc_Sexp_Object x){
struct Cyc___cycFILE*fopt=Cyc_fopen(filename,"w");
if(fopt == 0)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0,({struct _fat_ptr _tmp139=(struct _fat_ptr)({const char*_tmp18=filename;_tag_fat(_tmp18,sizeof(char),_get_zero_arr_size_char((void*)_tmp18,1U));});_tmpFD.f1=_tmp139;});_tmpFD;});void*_tmp15[1];_tmp15[0]=& _tmp17;({struct Cyc___cycFILE*_tmp13B=Cyc_stderr;struct _fat_ptr _tmp13A=({const char*_tmp16="unable to open file %s\n";_tag_fat(_tmp16,sizeof(char),24U);});Cyc_fprintf(_tmp13B,_tmp13A,_tag_fat(_tmp15,sizeof(void*),1));});});
Cyc_Sexp_tofile(_check_null(fopt),x);
Cyc_fclose(fopt);}
# 121
void Cyc_Sexp_printstring(struct Cyc_Xarray_Xarray*strings,struct _fat_ptr s){
# 123
({(void(*)(struct Cyc_Xarray_Xarray*,const char*))Cyc_Xarray_add;})(strings,(const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}
# 127
static void Cyc_Sexp_addlength(int*c,const char*s){
for(1;(int)*s != 0;({const char**_tmp19=& s;if(*(*_tmp19)!= 0)++(*_tmp19);else{_throw_arraybounds();}*_tmp19;})){*c=*c + 1;}}
# 131
static void Cyc_Sexp_addstring(struct _fat_ptr*p,const char*s){
for(1;(int)*s != 0;({const char**_tmp1A=& s;if(*(*_tmp1A)!= 0)++(*_tmp1A);else{_throw_arraybounds();}*_tmp1A;})){
({struct _fat_ptr _tmp1B=*p;char _tmp1C=*((char*)_check_fat_subscript(_tmp1B,sizeof(char),0U));char _tmp1D=*s;if(_get_fat_size(_tmp1B,sizeof(char))== 1U &&(_tmp1C == 0 && _tmp1D != 0))_throw_arraybounds();*((char*)_tmp1B.curr)=_tmp1D;});
({struct _fat_ptr _tmp13C=_fat_ptr_plus(*p,sizeof(char),1);*p=_tmp13C;});}}
# 139
struct _fat_ptr Cyc_Sexp_tostring(struct Cyc_Sexp_Object v){
struct Cyc_Xarray_Xarray*x=({(struct Cyc_Xarray_Xarray*(*)(void))Cyc_Xarray_create_empty;})();
struct Cyc_Sexp_Printer p=({struct Cyc_Sexp_Printer _tmpFE;_tmpFE.env=x,_tmpFE.print=Cyc_Sexp_printstring;_tmpFE;});
# 143
({(void(*)(struct Cyc_Sexp_Printer*,struct Cyc_Sexp_Object))Cyc_Sexp_print;})(& p,v);{
int len=1;
({(void(*)(void(*)(int*,const char*),int*,struct Cyc_Xarray_Xarray*))Cyc_Xarray_iter_c;})(Cyc_Sexp_addlength,& len,x);{
struct _fat_ptr res=({unsigned _tmp1E=len;_tag_fat(_cyccalloc_atomic(sizeof(char),_tmp1E),sizeof(char),_tmp1E);});
struct _fat_ptr p=res;
({(void(*)(void(*)(struct _fat_ptr*,const char*),struct _fat_ptr*,struct Cyc_Xarray_Xarray*))Cyc_Xarray_iter_c;})(Cyc_Sexp_addstring,& p,x);
return(struct _fat_ptr)res;}}}struct Cyc_Sexp_Cls{struct Cyc_Sexp_Class*vtable;};
# 159
extern struct Cyc_List_List*Cyc_Sexp_classes;
# 162
void Cyc_Sexp_register_class(struct Cyc_Sexp_Class*c){
Cyc_Sexp_classes=({struct Cyc_List_List*_tmp20=_cycalloc(sizeof(*_tmp20));({struct Cyc_Sexp_Cls*_tmp13D=({struct Cyc_Sexp_Cls*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->vtable=c;_tmp1F;});_tmp20->hd=_tmp13D;}),_tmp20->tl=Cyc_Sexp_classes;_tmp20;});}
# 168
static int Cyc_Sexp_pgetc(struct Cyc_Sexp_Parser*p){
int res=((p->getc))(p->env);
return res;}
# 174
static void Cyc_Sexp_pungetc(struct Cyc_Sexp_Parser*p,int ch){
((p->ungetc))(ch,p->env);}
# 179
static void Cyc_Sexp_perror(struct Cyc_Sexp_Parser*p,int ch,struct _fat_ptr s){
# 181
((p->error))(p->env,ch,s);}
# 185
static int Cyc_Sexp_whitespace(int ch){
return((ch == 32 || ch == 9)|| ch == 10)|| ch == 13;}
# 190
static void Cyc_Sexp_expectws(struct Cyc_Sexp_Parser*p,int expected_ch){
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);}
if(ch != expected_ch)({struct Cyc_Sexp_Parser*_tmp140=p;int _tmp13F=ch;Cyc_Sexp_perror(_tmp140,_tmp13F,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp23=({struct Cyc_Int_pa_PrintArg_struct _tmpFF;_tmpFF.tag=1,_tmpFF.f1=(unsigned long)expected_ch;_tmpFF;});void*_tmp21[1];_tmp21[0]=& _tmp23;({struct _fat_ptr _tmp13E=({const char*_tmp22="expected '%c'";_tag_fat(_tmp22,sizeof(char),14U);});Cyc_aprintf(_tmp13E,_tag_fat(_tmp21,sizeof(void*),1));});}));});}
# 197
static void Cyc_Sexp_expect(struct Cyc_Sexp_Parser*p,int expected_ch){
int ch=Cyc_Sexp_pgetc(p);
if(ch != expected_ch)({struct Cyc_Sexp_Parser*_tmp143=p;int _tmp142=ch;Cyc_Sexp_perror(_tmp143,_tmp142,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp26=({struct Cyc_Int_pa_PrintArg_struct _tmp100;_tmp100.tag=1,_tmp100.f1=(unsigned long)expected_ch;_tmp100;});void*_tmp24[1];_tmp24[0]=& _tmp26;({struct _fat_ptr _tmp141=({const char*_tmp25="expected '%c'";_tag_fat(_tmp25,sizeof(char),14U);});Cyc_aprintf(_tmp141,_tag_fat(_tmp24,sizeof(void*),1));});}));});}
# 205
struct Cyc_Sexp_Object Cyc_Sexp_parse(struct Cyc_Sexp_Parser*p){
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);}
{struct Cyc_List_List*_tmp27=Cyc_Sexp_classes;struct Cyc_List_List*cs=_tmp27;for(0;cs != 0;cs=cs->tl){
struct Cyc_Sexp_Cls*_tmp28=(struct Cyc_Sexp_Cls*)cs->hd;struct Cyc_Sexp_Cls*_stmttmp0=_tmp28;struct Cyc_Sexp_Cls*_tmp29=_stmttmp0;void*_tmp2A;_tmp2A=_tmp29->vtable;{struct Cyc_Sexp_Class*v=_tmp2A;
if(ch == (int)v->tag){
Cyc_Sexp_expectws(p,40);{
struct Cyc_Sexp_Obj*_tmp2B=((v->parse))(p);struct Cyc_Sexp_Obj*obj=_tmp2B;
Cyc_Sexp_expectws(p,41);
return Cyc_Sexp_up(obj);}}}}}
# 217
({void(*_tmp146)(void*,int,struct _fat_ptr)=p->error;void*_tmp145=p->env;int _tmp144=ch;_tmp146(_tmp145,_tmp144,({const char*_tmp2C="unexpected tag";_tag_fat(_tmp2C,sizeof(char),15U);}));});}
# 222
static void Cyc_Sexp_file_error(struct Cyc___cycFILE*f,int ch,struct _fat_ptr msg){
# 225
({struct Cyc_Int_pa_PrintArg_struct _tmp2F=({struct Cyc_Int_pa_PrintArg_struct _tmp102;_tmp102.tag=1,_tmp102.f1=(unsigned long)ch;_tmp102;});struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0,_tmp101.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp101;});void*_tmp2D[2];_tmp2D[0]=& _tmp2F,_tmp2D[1]=& _tmp30;({struct Cyc___cycFILE*_tmp148=Cyc_stderr;struct _fat_ptr _tmp147=({const char*_tmp2E="found '%c'.  %s\n";_tag_fat(_tmp2E,sizeof(char),17U);});Cyc_fprintf(_tmp148,_tmp147,_tag_fat(_tmp2D,sizeof(void*),2));});});
Cyc_fclose(f);
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp149=({const char*_tmp31="Sexp::file2object error";_tag_fat(_tmp31,sizeof(char),24U);});_tmp32->f1=_tmp149;});_tmp32;}));}
# 231
struct Cyc_Sexp_Object Cyc_Sexp_fromfile(struct Cyc___cycFILE*f){
struct Cyc_Sexp_Parser p=({struct Cyc_Sexp_Parser _tmp103;_tmp103.env=f,_tmp103.getc=Cyc_getc,_tmp103.ungetc=Cyc_ungetc,_tmp103.error=Cyc_Sexp_file_error;_tmp103;});
# 234
struct _handler_cons _tmp33;_push_handler(& _tmp33);{int _tmp35=0;if(setjmp(_tmp33.handler))_tmp35=1;if(!_tmp35){
{struct Cyc_Sexp_Object _tmp36=({(struct Cyc_Sexp_Object(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse;})(& p);struct Cyc_Sexp_Object s=_tmp36;
Cyc_fclose(f);{
struct Cyc_Sexp_Object _tmp37=s;_npop_handler(0);return _tmp37;}}
# 235
;_pop_handler();}else{void*_tmp34=(void*)Cyc_Core_get_exn_thrown();void*_tmp38=_tmp34;void*_tmp39;_tmp39=_tmp38;{void*e=_tmp39;
# 239
Cyc_Core_rethrow(e);};}}}
# 244
struct Cyc_Sexp_Object Cyc_Sexp_fromfilename(const char*filename){
struct Cyc___cycFILE*fopt=Cyc_fopen(filename,"r");
if(fopt == 0)(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp14A=({const char*_tmp3A="file not found";_tag_fat(_tmp3A,sizeof(char),15U);});_tmp3B->f1=_tmp14A;});_tmp3B;}));{
struct Cyc_Sexp_Object _tmp3C=Cyc_Sexp_fromfile(fopt);struct Cyc_Sexp_Object res=_tmp3C;
Cyc_fclose(fopt);
return res;}}struct Cyc_Sexp_StringEnv{struct _fat_ptr str;unsigned length;unsigned offset;};
# 260
static int Cyc_Sexp_string_getc(struct Cyc_Sexp_StringEnv*env){
if(env->offset >= env->length)return -1;{
int ch=(int)*((const char*)_check_fat_subscript(env->str,sizeof(char),(int)env->offset));
++ env->offset;
return ch;}}
# 268
static int Cyc_Sexp_string_ungetc(int ch,struct Cyc_Sexp_StringEnv*env){
if(ch == -1)return -1;
-- env->offset;
return 0;}
# 275
static void Cyc_Sexp_string_error(struct Cyc_Sexp_StringEnv*env,int ch,struct _fat_ptr msg){
# 278
({struct Cyc_Int_pa_PrintArg_struct _tmp3F=({struct Cyc_Int_pa_PrintArg_struct _tmp105;_tmp105.tag=1,_tmp105.f1=(unsigned long)ch;_tmp105;});struct Cyc_String_pa_PrintArg_struct _tmp40=({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0,_tmp104.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp104;});void*_tmp3D[2];_tmp3D[0]=& _tmp3F,_tmp3D[1]=& _tmp40;({struct Cyc___cycFILE*_tmp14C=Cyc_stderr;struct _fat_ptr _tmp14B=({const char*_tmp3E="found '%c', %s\n";_tag_fat(_tmp3E,sizeof(char),16U);});Cyc_fprintf(_tmp14C,_tmp14B,_tag_fat(_tmp3D,sizeof(void*),2));});});
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp14D=({const char*_tmp41="Sexp::string2object error";_tag_fat(_tmp41,sizeof(char),26U);});_tmp42->f1=_tmp14D;});_tmp42;}));}
# 283
struct Cyc_Sexp_Object Cyc_Sexp_fromstring(struct _fat_ptr str){
struct Cyc_Sexp_StringEnv env=({struct Cyc_Sexp_StringEnv _tmp107;_tmp107.str=str,({unsigned long _tmp14E=Cyc_strlen((struct _fat_ptr)str);_tmp107.length=_tmp14E;}),_tmp107.offset=0U;_tmp107;});
struct Cyc_Sexp_Parser p=({struct Cyc_Sexp_Parser _tmp106;_tmp106.env=& env,_tmp106.getc=Cyc_Sexp_string_getc,_tmp106.ungetc=Cyc_Sexp_string_ungetc,_tmp106.error=Cyc_Sexp_string_error;_tmp106;});
# 288
return({(struct Cyc_Sexp_Object(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse;})(& p);}
# 295
static int Cyc_Sexp_hash_ulonglong(struct _tuple6*self){return(int)self->v;}
static int Cyc_Sexp_hash_slonglong(struct _tuple7*self){return(int)self->v;}
static int Cyc_Sexp_hash_uint(struct _tuple4*self){return(int)self->v;}
static int Cyc_Sexp_hash_sint(struct _tuple5*self){return self->v;}
static int Cyc_Sexp_hash_ushort(struct _tuple2*self){return(int)self->v;}
static int Cyc_Sexp_hash_sshort(struct _tuple3*self){return(int)self->v;}
static int Cyc_Sexp_hash_uchar(struct _tuple0*self){return(int)self->v;}
static int Cyc_Sexp_hash_schar(struct _tuple1*self){return(int)self->v;}
# 310
static int Cyc_Sexp_cmp_uint(struct _tuple4*x,struct _tuple4*y){return(int)(x->v - y->v);}
static int Cyc_Sexp_cmp_sint(struct _tuple5*x,struct _tuple5*y){return x->v - y->v;}
static int Cyc_Sexp_cmp_ushort(struct _tuple2*x,struct _tuple2*y){return(int)x->v - (int)y->v;}
static int Cyc_Sexp_cmp_sshort(struct _tuple3*x,struct _tuple3*y){return(int)x->v - (int)y->v;}
static int Cyc_Sexp_cmp_uchar(struct _tuple0*x,struct _tuple0*y){return(int)x->v - (int)y->v;}
static int Cyc_Sexp_cmp_schar(struct _tuple1*x,struct _tuple1*y){return(int)x->v - (int)y->v;}
# 330 "sexp.cyc"
static void*Cyc_Sexp_accept_uchar(struct _tuple0*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp43)(void*,struct _tuple0*,struct Cyc_Sexp_Visitor*)=visitor->visit_uchar;void*(*f)(void*,struct _tuple0*,struct Cyc_Sexp_Visitor*)=_tmp43;if(f == 0)return({void*(*_tmp151)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp150=env;struct Cyc_Sexp_Object _tmp14F=({(struct Cyc_Sexp_Object(*)(struct _tuple0*))Cyc_Sexp_up;})(self);_tmp151(_tmp150,_tmp14F,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_schar(struct _tuple1*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp44)(void*,struct _tuple1*,struct Cyc_Sexp_Visitor*)=visitor->visit_schar;void*(*f)(void*,struct _tuple1*,struct Cyc_Sexp_Visitor*)=_tmp44;if(f == 0)return({void*(*_tmp154)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp153=env;struct Cyc_Sexp_Object _tmp152=({(struct Cyc_Sexp_Object(*)(struct _tuple1*))Cyc_Sexp_up;})(self);_tmp154(_tmp153,_tmp152,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_ushort(struct _tuple2*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp45)(void*,struct _tuple2*,struct Cyc_Sexp_Visitor*)=visitor->visit_ushort;void*(*f)(void*,struct _tuple2*,struct Cyc_Sexp_Visitor*)=_tmp45;if(f == 0)return({void*(*_tmp157)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp156=env;struct Cyc_Sexp_Object _tmp155=({(struct Cyc_Sexp_Object(*)(struct _tuple2*))Cyc_Sexp_up;})(self);_tmp157(_tmp156,_tmp155,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_sshort(struct _tuple3*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp46)(void*,struct _tuple3*,struct Cyc_Sexp_Visitor*)=visitor->visit_sshort;void*(*f)(void*,struct _tuple3*,struct Cyc_Sexp_Visitor*)=_tmp46;if(f == 0)return({void*(*_tmp15A)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp159=env;struct Cyc_Sexp_Object _tmp158=({(struct Cyc_Sexp_Object(*)(struct _tuple3*))Cyc_Sexp_up;})(self);_tmp15A(_tmp159,_tmp158,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_uint(struct _tuple4*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp47)(void*,struct _tuple4*,struct Cyc_Sexp_Visitor*)=visitor->visit_uint;void*(*f)(void*,struct _tuple4*,struct Cyc_Sexp_Visitor*)=_tmp47;if(f == 0)return({void*(*_tmp15D)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp15C=env;struct Cyc_Sexp_Object _tmp15B=({(struct Cyc_Sexp_Object(*)(struct _tuple4*))Cyc_Sexp_up;})(self);_tmp15D(_tmp15C,_tmp15B,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_sint(struct _tuple5*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp48)(void*,struct _tuple5*,struct Cyc_Sexp_Visitor*)=visitor->visit_sint;void*(*f)(void*,struct _tuple5*,struct Cyc_Sexp_Visitor*)=_tmp48;if(f == 0)return({void*(*_tmp160)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp15F=env;struct Cyc_Sexp_Object _tmp15E=({(struct Cyc_Sexp_Object(*)(struct _tuple5*))Cyc_Sexp_up;})(self);_tmp160(_tmp15F,_tmp15E,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_ulonglong(struct _tuple6*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp49)(void*,struct _tuple6*,struct Cyc_Sexp_Visitor*)=visitor->visit_ulonglong;void*(*f)(void*,struct _tuple6*,struct Cyc_Sexp_Visitor*)=_tmp49;if(f == 0)return({void*(*_tmp163)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp162=env;struct Cyc_Sexp_Object _tmp161=({(struct Cyc_Sexp_Object(*)(struct _tuple6*))Cyc_Sexp_up;})(self);_tmp163(_tmp162,_tmp161,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_slonglong(struct _tuple7*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp4A)(void*,struct _tuple7*,struct Cyc_Sexp_Visitor*)=visitor->visit_slonglong;void*(*f)(void*,struct _tuple7*,struct Cyc_Sexp_Visitor*)=_tmp4A;if(f == 0)return({void*(*_tmp166)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp165=env;struct Cyc_Sexp_Object _tmp164=({(struct Cyc_Sexp_Object(*)(struct _tuple7*))Cyc_Sexp_up;})(self);_tmp166(_tmp165,_tmp164,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_float(struct _tuple8*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp4B)(void*,struct _tuple8*,struct Cyc_Sexp_Visitor*)=visitor->visit_float;void*(*f)(void*,struct _tuple8*,struct Cyc_Sexp_Visitor*)=_tmp4B;if(f == 0)return({void*(*_tmp169)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp168=env;struct Cyc_Sexp_Object _tmp167=({(struct Cyc_Sexp_Object(*)(struct _tuple8*))Cyc_Sexp_up;})(self);_tmp169(_tmp168,_tmp167,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_double(struct _tuple9*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp4C)(void*,struct _tuple9*,struct Cyc_Sexp_Visitor*)=visitor->visit_double;void*(*f)(void*,struct _tuple9*,struct Cyc_Sexp_Visitor*)=_tmp4C;if(f == 0)return({void*(*_tmp16C)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp16B=env;struct Cyc_Sexp_Object _tmp16A=({(struct Cyc_Sexp_Object(*)(struct _tuple9*))Cyc_Sexp_up;})(self);_tmp16C(_tmp16B,_tmp16A,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_str(struct _tuple10*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp4D)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=visitor->visit_str;void*(*f)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=_tmp4D;if(f == 0)return({void*(*_tmp16F)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp16E=env;struct Cyc_Sexp_Object _tmp16D=({(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;})(self);_tmp16F(_tmp16E,_tmp16D,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_symbol(struct _tuple10*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp4E)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=visitor->visit_symbol;void*(*f)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=_tmp4E;if(f == 0)return({void*(*_tmp172)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp171=env;struct Cyc_Sexp_Object _tmp170=({(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;})(self);_tmp172(_tmp171,_tmp170,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_tuple(struct _tuple10*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*_tmp4F)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=visitor->visit_tuple;void*(*f)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=_tmp4F;if(f == 0)return({void*(*_tmp175)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_tmp174=env;struct Cyc_Sexp_Object _tmp173=({(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;})(self);_tmp175(_tmp174,_tmp173,visitor);});else{return f(env,self,visitor);}}
# 347
static unsigned Cyc_Sexp_hex2value(struct Cyc_Sexp_Parser*p,int ch){
if(ch >= 48 && ch <= 57)
return(unsigned)(ch - 48);
if(ch >= 65 && ch <= 70)
return(unsigned)(10 + (ch - 65));
if(ch >= 97 && ch <= 102)
return(unsigned)(10 + (ch - 97));
({struct Cyc_Sexp_Parser*_tmp177=p;int _tmp176=ch;Cyc_Sexp_perror(_tmp177,_tmp176,({const char*_tmp50="expecting hex digit";_tag_fat(_tmp50,sizeof(char),20U);}));});}
# 358
static unsigned Cyc_Sexp_nibble(unsigned i,unsigned x){
for(1;i > 0U;-- i){
x=x >> 4U;}
return x & 15U;}
# 365
static unsigned long long Cyc_Sexp_parse_longlong(struct Cyc_Sexp_Parser*p){
unsigned long long res=0U;
{unsigned i=0U;for(0;i < 16U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned long long v=(unsigned long long)Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 372
return res;}
# 376
static void Cyc_Sexp_print_ulonglong(struct _tuple6*self,struct Cyc_Sexp_Printer*p){
# 378
unsigned long long _tmp51=self->v;unsigned long long x=_tmp51;
({void(*_tmp17A)(void*,struct _fat_ptr)=p->print;void*_tmp179=p->env;_tmp17A(_tmp179,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp54=({struct Cyc_Int_pa_PrintArg_struct _tmp109;_tmp109.tag=1,_tmp109.f1=(unsigned)(x >> 32U);_tmp109;});struct Cyc_Int_pa_PrintArg_struct _tmp55=({struct Cyc_Int_pa_PrintArg_struct _tmp108;_tmp108.tag=1,_tmp108.f1=(unsigned)x;_tmp108;});void*_tmp52[2];_tmp52[0]=& _tmp54,_tmp52[1]=& _tmp55;({struct _fat_ptr _tmp178=({const char*_tmp53="%08x%08x";_tag_fat(_tmp53,sizeof(char),9U);});Cyc_aprintf(_tmp178,_tag_fat(_tmp52,sizeof(void*),2));});}));});}
# 382
static struct _tuple6*Cyc_Sexp_parse_ulonglong(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_ulonglong(Cyc_Sexp_parse_longlong(p));}
# 386
static void Cyc_Sexp_print_slonglong(struct _tuple7*self,struct Cyc_Sexp_Printer*p){
# 388
long long _tmp56=self->v;long long x=_tmp56;
({void(*_tmp17D)(void*,struct _fat_ptr)=p->print;void*_tmp17C=p->env;_tmp17D(_tmp17C,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp59=({struct Cyc_Int_pa_PrintArg_struct _tmp10B;_tmp10B.tag=1,_tmp10B.f1=(unsigned)(x >> 32);_tmp10B;});struct Cyc_Int_pa_PrintArg_struct _tmp5A=({struct Cyc_Int_pa_PrintArg_struct _tmp10A;_tmp10A.tag=1,_tmp10A.f1=(unsigned)x;_tmp10A;});void*_tmp57[2];_tmp57[0]=& _tmp59,_tmp57[1]=& _tmp5A;({struct _fat_ptr _tmp17B=({const char*_tmp58="%08x%08x";_tag_fat(_tmp58,sizeof(char),9U);});Cyc_aprintf(_tmp17B,_tag_fat(_tmp57,sizeof(void*),2));});}));});}
# 391
static struct _tuple7*Cyc_Sexp_parse_slonglong(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_slonglong((long long)Cyc_Sexp_parse_longlong(p));}
# 395
static int Cyc_Sexp_cmp_ulonglong(struct _tuple6*x,struct _tuple6*y){
# 397
if(x->v > y->v)return 1;else{
if(x->v < y->v)return -1;else{
return 0;}}}
# 402
static int Cyc_Sexp_cmp_slonglong(struct _tuple7*x,struct _tuple7*y){
# 404
if(x->v > y->v)return 1;else{
if(x->v < y->v)return -1;else{
return 0;}}}
# 410
static unsigned Cyc_Sexp_parse_int(struct Cyc_Sexp_Parser*p){
unsigned res=0U;
{unsigned i=0U;for(0;i < 8U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned v=Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 417
return res;}
# 420
static void Cyc_Sexp_print_uint(struct _tuple4*self,struct Cyc_Sexp_Printer*p){
unsigned _tmp5B=self->v;unsigned x=_tmp5B;
({void(*_tmp180)(void*,struct _fat_ptr)=p->print;void*_tmp17F=p->env;_tmp180(_tmp17F,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5E=({struct Cyc_Int_pa_PrintArg_struct _tmp10C;_tmp10C.tag=1,_tmp10C.f1=x;_tmp10C;});void*_tmp5C[1];_tmp5C[0]=& _tmp5E;({struct _fat_ptr _tmp17E=({const char*_tmp5D="%08x";_tag_fat(_tmp5D,sizeof(char),5U);});Cyc_aprintf(_tmp17E,_tag_fat(_tmp5C,sizeof(void*),1));});}));});}
# 425
static struct _tuple4*Cyc_Sexp_parse_uint(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_uint(Cyc_Sexp_parse_int(p));}
# 429
static void Cyc_Sexp_print_sint(struct _tuple5*self,struct Cyc_Sexp_Printer*p){
int _tmp5F=self->v;int x=_tmp5F;
({void(*_tmp183)(void*,struct _fat_ptr)=p->print;void*_tmp182=p->env;_tmp183(_tmp182,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp62=({struct Cyc_Int_pa_PrintArg_struct _tmp10D;_tmp10D.tag=1,_tmp10D.f1=(unsigned)x;_tmp10D;});void*_tmp60[1];_tmp60[0]=& _tmp62;({struct _fat_ptr _tmp181=({const char*_tmp61="%08x";_tag_fat(_tmp61,sizeof(char),5U);});Cyc_aprintf(_tmp181,_tag_fat(_tmp60,sizeof(void*),1));});}));});}
# 434
static struct _tuple5*Cyc_Sexp_parse_sint(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_sint((int)Cyc_Sexp_parse_int(p));}
# 439
static unsigned short Cyc_Sexp_parse_short(struct Cyc_Sexp_Parser*p){
unsigned res=0U;
{unsigned i=0U;for(0;i < 4U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned v=Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 446
return(unsigned short)res;}
# 450
static void Cyc_Sexp_print_ushort(struct _tuple2*self,struct Cyc_Sexp_Printer*p){
unsigned short _tmp63=self->v;unsigned short x=_tmp63;
({void(*_tmp18A)(void*,struct _fat_ptr)=p->print;void*_tmp189=p->env;_tmp18A(_tmp189,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp66=({struct Cyc_Int_pa_PrintArg_struct _tmp111;_tmp111.tag=1,({
unsigned _tmp184=Cyc_Sexp_nibble(3U,(unsigned)x);_tmp111.f1=_tmp184;});_tmp111;});struct Cyc_Int_pa_PrintArg_struct _tmp67=({struct Cyc_Int_pa_PrintArg_struct _tmp110;_tmp110.tag=1,({unsigned _tmp185=Cyc_Sexp_nibble(2U,(unsigned)x);_tmp110.f1=_tmp185;});_tmp110;});struct Cyc_Int_pa_PrintArg_struct _tmp68=({struct Cyc_Int_pa_PrintArg_struct _tmp10F;_tmp10F.tag=1,({unsigned _tmp186=Cyc_Sexp_nibble(1U,(unsigned)x);_tmp10F.f1=_tmp186;});_tmp10F;});struct Cyc_Int_pa_PrintArg_struct _tmp69=({struct Cyc_Int_pa_PrintArg_struct _tmp10E;_tmp10E.tag=1,({unsigned _tmp187=Cyc_Sexp_nibble(0U,(unsigned)x);_tmp10E.f1=_tmp187;});_tmp10E;});void*_tmp64[4];_tmp64[0]=& _tmp66,_tmp64[1]=& _tmp67,_tmp64[2]=& _tmp68,_tmp64[3]=& _tmp69;({struct _fat_ptr _tmp188=({const char*_tmp65="%x%x%x%x";_tag_fat(_tmp65,sizeof(char),9U);});Cyc_aprintf(_tmp188,_tag_fat(_tmp64,sizeof(void*),4));});}));});}
# 456
static struct _tuple2*Cyc_Sexp_parse_ushort(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_ushort(Cyc_Sexp_parse_short(p));}
# 460
static void Cyc_Sexp_print_sshort(struct _tuple3*self,struct Cyc_Sexp_Printer*p){
short _tmp6A=self->v;short x=_tmp6A;
({void(*_tmp191)(void*,struct _fat_ptr)=p->print;void*_tmp190=p->env;_tmp191(_tmp190,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6D=({struct Cyc_Int_pa_PrintArg_struct _tmp115;_tmp115.tag=1,({
unsigned _tmp18B=Cyc_Sexp_nibble(3U,(unsigned)x);_tmp115.f1=_tmp18B;});_tmp115;});struct Cyc_Int_pa_PrintArg_struct _tmp6E=({struct Cyc_Int_pa_PrintArg_struct _tmp114;_tmp114.tag=1,({unsigned _tmp18C=Cyc_Sexp_nibble(2U,(unsigned)x);_tmp114.f1=_tmp18C;});_tmp114;});struct Cyc_Int_pa_PrintArg_struct _tmp6F=({struct Cyc_Int_pa_PrintArg_struct _tmp113;_tmp113.tag=1,({unsigned _tmp18D=Cyc_Sexp_nibble(1U,(unsigned)x);_tmp113.f1=_tmp18D;});_tmp113;});struct Cyc_Int_pa_PrintArg_struct _tmp70=({struct Cyc_Int_pa_PrintArg_struct _tmp112;_tmp112.tag=1,({unsigned _tmp18E=Cyc_Sexp_nibble(0U,(unsigned)x);_tmp112.f1=_tmp18E;});_tmp112;});void*_tmp6B[4];_tmp6B[0]=& _tmp6D,_tmp6B[1]=& _tmp6E,_tmp6B[2]=& _tmp6F,_tmp6B[3]=& _tmp70;({struct _fat_ptr _tmp18F=({const char*_tmp6C="%x%x%x%x";_tag_fat(_tmp6C,sizeof(char),9U);});Cyc_aprintf(_tmp18F,_tag_fat(_tmp6B,sizeof(void*),4));});}));});}
# 466
static struct _tuple3*Cyc_Sexp_parse_sshort(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_sshort((short)Cyc_Sexp_parse_short(p));}
# 471
static unsigned char Cyc_Sexp_parse_char(struct Cyc_Sexp_Parser*p){
unsigned res=0U;
{unsigned i=0U;for(0;i < 2U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned v=Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 478
return(unsigned char)res;}
# 481
static void Cyc_Sexp_print_uchar(struct _tuple0*self,struct Cyc_Sexp_Printer*p){
unsigned char _tmp71=self->v;unsigned char x=_tmp71;
({void(*_tmp196)(void*,struct _fat_ptr)=p->print;void*_tmp195=p->env;_tmp196(_tmp195,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp74=({struct Cyc_Int_pa_PrintArg_struct _tmp117;_tmp117.tag=1,({unsigned _tmp192=Cyc_Sexp_nibble(1U,(unsigned)x);_tmp117.f1=_tmp192;});_tmp117;});struct Cyc_Int_pa_PrintArg_struct _tmp75=({struct Cyc_Int_pa_PrintArg_struct _tmp116;_tmp116.tag=1,({unsigned _tmp193=Cyc_Sexp_nibble(0U,(unsigned)x);_tmp116.f1=_tmp193;});_tmp116;});void*_tmp72[2];_tmp72[0]=& _tmp74,_tmp72[1]=& _tmp75;({struct _fat_ptr _tmp194=({const char*_tmp73="%x%x";_tag_fat(_tmp73,sizeof(char),5U);});Cyc_aprintf(_tmp194,_tag_fat(_tmp72,sizeof(void*),2));});}));});}
# 486
static struct _tuple0*Cyc_Sexp_parse_uchar(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_uchar(Cyc_Sexp_parse_char(p));}
# 490
static void Cyc_Sexp_print_schar(struct _tuple1*self,struct Cyc_Sexp_Printer*p){
signed char _tmp76=self->v;signed char x=_tmp76;
({void(*_tmp19B)(void*,struct _fat_ptr)=p->print;void*_tmp19A=p->env;_tmp19B(_tmp19A,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp79=({struct Cyc_Int_pa_PrintArg_struct _tmp119;_tmp119.tag=1,({unsigned _tmp197=Cyc_Sexp_nibble(1U,(unsigned)x);_tmp119.f1=_tmp197;});_tmp119;});struct Cyc_Int_pa_PrintArg_struct _tmp7A=({struct Cyc_Int_pa_PrintArg_struct _tmp118;_tmp118.tag=1,({unsigned _tmp198=Cyc_Sexp_nibble(0U,(unsigned)x);_tmp118.f1=_tmp198;});_tmp118;});void*_tmp77[2];_tmp77[0]=& _tmp79,_tmp77[1]=& _tmp7A;({struct _fat_ptr _tmp199=({const char*_tmp78="%x%x";_tag_fat(_tmp78,sizeof(char),5U);});Cyc_aprintf(_tmp199,_tag_fat(_tmp77,sizeof(void*),2));});}));});}
# 495
static struct _tuple1*Cyc_Sexp_parse_schar(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_schar((signed char)Cyc_Sexp_parse_char(p));}union Cyc_Sexp_IntFloat{unsigned i;float f;};
# 502
static float Cyc_Sexp_int2float(unsigned i){
union Cyc_Sexp_IntFloat u=({union Cyc_Sexp_IntFloat _tmp11A;_tmp11A.i=i;_tmp11A;});return u.f;}
# 505
static unsigned Cyc_Sexp_float2int(float f){
union Cyc_Sexp_IntFloat u=({union Cyc_Sexp_IntFloat _tmp11B;_tmp11B.f=f;_tmp11B;});return u.i;}union Cyc_Sexp_DoubleLongLong{unsigned long long x;double d;};
# 509
static double Cyc_Sexp_longlong2double(unsigned long long x){
union Cyc_Sexp_DoubleLongLong u=({union Cyc_Sexp_DoubleLongLong _tmp11C;_tmp11C.x=x;_tmp11C;});return u.d;}
# 512
static unsigned long long Cyc_Sexp_double2longlong(double d){
union Cyc_Sexp_DoubleLongLong u=({union Cyc_Sexp_DoubleLongLong _tmp11D;_tmp11D.d=d;_tmp11D;});return u.x;}
# 517
static int Cyc_Sexp_hash_float(struct _tuple8*self){return(int)Cyc_Sexp_float2int(self->v);}
# 519
static void Cyc_Sexp_print_float(struct _tuple8*self,struct Cyc_Sexp_Printer*p){
float _tmp7B=self->v;float x=_tmp7B;
({void(*_tmp19F)(void*,struct _fat_ptr)=p->print;void*_tmp19E=p->env;_tmp19F(_tmp19E,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp7E=({struct Cyc_Int_pa_PrintArg_struct _tmp11E;_tmp11E.tag=1,({unsigned _tmp19C=Cyc_Sexp_float2int(x);_tmp11E.f1=_tmp19C;});_tmp11E;});void*_tmp7C[1];_tmp7C[0]=& _tmp7E;({struct _fat_ptr _tmp19D=({const char*_tmp7D="%08x";_tag_fat(_tmp7D,sizeof(char),5U);});Cyc_aprintf(_tmp19D,_tag_fat(_tmp7C,sizeof(void*),1));});}));});}
# 524
static struct _tuple8*Cyc_Sexp_parse_float(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_float(Cyc_Sexp_int2float(Cyc_Sexp_parse_int(p)));}
# 528
static int Cyc_Sexp_cmp_float(struct _tuple8*x,struct _tuple8*y){
float diff=x->v - y->v;
if((double)diff < 0.0)return -1;else{
if((double)diff > 0.0)return 1;else{
return 0;}}}
# 536
static int Cyc_Sexp_hash_double(struct _tuple9*self){
return(int)((unsigned)Cyc_Sexp_double2longlong(self->v));}
# 540
static void Cyc_Sexp_print_double(struct _tuple9*self,struct Cyc_Sexp_Printer*p){
double _tmp7F=self->v;double d=_tmp7F;
unsigned long long _tmp80=Cyc_Sexp_double2longlong(d);unsigned long long x=_tmp80;
({void(*_tmp1A2)(void*,struct _fat_ptr)=p->print;void*_tmp1A1=p->env;_tmp1A2(_tmp1A1,(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp83=({struct Cyc_Int_pa_PrintArg_struct _tmp120;_tmp120.tag=1,_tmp120.f1=(unsigned)(x >> 32U);_tmp120;});struct Cyc_Int_pa_PrintArg_struct _tmp84=({struct Cyc_Int_pa_PrintArg_struct _tmp11F;_tmp11F.tag=1,_tmp11F.f1=(unsigned)x;_tmp11F;});void*_tmp81[2];_tmp81[0]=& _tmp83,_tmp81[1]=& _tmp84;({struct _fat_ptr _tmp1A0=({const char*_tmp82="%08x%08x";_tag_fat(_tmp82,sizeof(char),9U);});Cyc_aprintf(_tmp1A0,_tag_fat(_tmp81,sizeof(void*),2));});}));});}
# 547
static struct _tuple9*Cyc_Sexp_parse_double(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_double(Cyc_Sexp_longlong2double(Cyc_Sexp_parse_longlong(p)));}
# 551
static int Cyc_Sexp_cmp_double(struct _tuple9*x,struct _tuple9*y){
double diff=x->v - y->v;
if(diff < 0.0)return -1;else{
if(diff > 0.0)return 1;else{
return 0;}}}
# 559
static int Cyc_Sexp_hash_str(struct _tuple10*self){
return Cyc_Hashtable_hash_string(self->v);}
# 563
static int Cyc_Sexp_hash_symbol(struct _tuple10*self){
return Cyc_Hashtable_hash_string(self->v);}
# 567
static void Cyc_Sexp_print_symbol(struct _tuple10*self,struct Cyc_Sexp_Printer*p){
# 569
((p->print))(p->env,self->v);}
# 575
static struct _tuple10*Cyc_Sexp_parse_symbol(struct Cyc_Sexp_Parser*p){
struct Cyc_List_List*chars=0;
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);}
while(1){
if(ch == -1)({struct Cyc_Sexp_Parser*_tmp1A4=p;int _tmp1A3=ch;Cyc_Sexp_perror(_tmp1A4,_tmp1A3,({const char*_tmp85="unexpected end of file";_tag_fat(_tmp85,sizeof(char),23U);}));});
if(Cyc_Sexp_whitespace(ch)|| ch == 41){
Cyc_Sexp_pungetc(p,ch);
break;}
# 585
chars=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->hd=(void*)ch,_tmp86->tl=chars;_tmp86;});
ch=Cyc_Sexp_pgetc(p);}
# 588
chars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(chars);{
struct _fat_ptr buf=({unsigned _tmp8A=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(chars)+ 1;_tag_fat(_cyccalloc_atomic(sizeof(char),_tmp8A),sizeof(char),_tmp8A);});
{unsigned i=0U;for(0;chars != 0;(chars=chars->tl,++ i)){
({struct _fat_ptr _tmp87=_fat_ptr_plus(buf,sizeof(char),(int)i);char _tmp88=*((char*)_check_fat_subscript(_tmp87,sizeof(char),0U));char _tmp89=(char)((int)chars->hd);if(_get_fat_size(_tmp87,sizeof(char))== 1U &&(_tmp88 == 0 && _tmp89 != 0))_throw_arraybounds();*((char*)_tmp87.curr)=_tmp89;});}}
return Cyc_Sexp_mk_symbol((struct _fat_ptr)buf);}}
# 596
static struct _fat_ptr Cyc_Sexp_escape(struct _fat_ptr s){
unsigned n=Cyc_strlen((struct _fat_ptr)s);
int escapes=0;
{unsigned i=0U;for(0;i < n;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)i))== 34 ||(int)((const char*)s.curr)[(int)i]== 92)++ escapes;}}
# 602
if(escapes == 0)return s;{
struct _fat_ptr news=({unsigned _tmp92=((n + (unsigned)escapes)+ 1U)+ 1U;char*_tmp91=_cycalloc_atomic(_check_times(_tmp92,sizeof(char)));({{unsigned _tmp121=(n + (unsigned)escapes)+ 1U;unsigned i;for(i=0;i < _tmp121;++ i){_tmp91[i]='\000';}_tmp91[_tmp121]=0;}0;});_tag_fat(_tmp91,sizeof(char),_tmp92);});
unsigned pos=0U;
{unsigned i=0U;for(0;i < n;++ i){
char ch=((const char*)s.curr)[(int)i];
if((int)((const char*)s.curr)[(int)i]== 34 ||(int)((const char*)s.curr)[(int)i]== 92){
({struct _fat_ptr _tmp8B=_fat_ptr_plus(news,sizeof(char),(int)pos);char _tmp8C=*((char*)_check_fat_subscript(_tmp8B,sizeof(char),0U));char _tmp8D='\\';if(_get_fat_size(_tmp8B,sizeof(char))== 1U &&(_tmp8C == 0 && _tmp8D != 0))_throw_arraybounds();*((char*)_tmp8B.curr)=_tmp8D;});
++ pos;}
# 611
({struct _fat_ptr _tmp8E=_fat_ptr_plus(news,sizeof(char),(int)pos);char _tmp8F=*((char*)_check_fat_subscript(_tmp8E,sizeof(char),0U));char _tmp90=ch;if(_get_fat_size(_tmp8E,sizeof(char))== 1U &&(_tmp8F == 0 && _tmp90 != 0))_throw_arraybounds();*((char*)_tmp8E.curr)=_tmp90;});
++ pos;}}
# 614
return(struct _fat_ptr)news;}}
# 618
static void Cyc_Sexp_print_str(struct _tuple10*self,struct Cyc_Sexp_Printer*p){
({void(*_tmp1A6)(void*,struct _fat_ptr)=p->print;void*_tmp1A5=p->env;_tmp1A6(_tmp1A5,({const char*_tmp93="\"";_tag_fat(_tmp93,sizeof(char),2U);}));});
({void(*_tmp1A8)(void*,struct _fat_ptr)=p->print;void*_tmp1A7=p->env;_tmp1A8(_tmp1A7,Cyc_Sexp_escape(self->v));});
({void(*_tmp1AA)(void*,struct _fat_ptr)=p->print;void*_tmp1A9=p->env;_tmp1AA(_tmp1A9,({const char*_tmp94="\"";_tag_fat(_tmp94,sizeof(char),2U);}));});}
# 625
static int Cyc_Sexp_cmp_str(struct _tuple10*x,struct _tuple10*y){
return Cyc_strcmp((struct _fat_ptr)x->v,(struct _fat_ptr)y->v);}
# 629
static int Cyc_Sexp_cmp_symbol(struct _tuple10*x,struct _tuple10*y){
return Cyc_strcmp((struct _fat_ptr)x->v,(struct _fat_ptr)y->v);}
# 636
static struct _tuple10*Cyc_Sexp_parse_str(struct Cyc_Sexp_Parser*p){
Cyc_Sexp_expectws(p,34);{
struct Cyc_List_List*chars=0;
while(1){
int ch=Cyc_Sexp_pgetc(p);
if(ch == -1)({struct Cyc_Sexp_Parser*_tmp1AC=p;int _tmp1AB=ch;Cyc_Sexp_perror(_tmp1AC,_tmp1AB,({const char*_tmp95="unexpected end of file";_tag_fat(_tmp95,sizeof(char),23U);}));});
if(ch == 34)break;
if(ch == 92){
ch=Cyc_Sexp_pgetc(p);
if(ch != 34 && ch != 92)({struct Cyc_Sexp_Parser*_tmp1AE=p;int _tmp1AD=ch;Cyc_Sexp_perror(_tmp1AE,_tmp1AD,({const char*_tmp96="expected '\"' or '\\'";_tag_fat(_tmp96,sizeof(char),20U);}));});}
# 647
chars=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=(void*)ch,_tmp97->tl=chars;_tmp97;});}
# 649
chars=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(chars);{
struct _fat_ptr buf=({unsigned _tmp9B=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(chars)+ 1;_tag_fat(_cyccalloc_atomic(sizeof(char),_tmp9B),sizeof(char),_tmp9B);});
{unsigned i=0U;for(0;chars != 0;(chars=chars->tl,++ i)){
({struct _fat_ptr _tmp98=_fat_ptr_plus(buf,sizeof(char),(int)i);char _tmp99=*((char*)_check_fat_subscript(_tmp98,sizeof(char),0U));char _tmp9A=(char)((int)chars->hd);if(_get_fat_size(_tmp98,sizeof(char))== 1U &&(_tmp99 == 0 && _tmp9A != 0))_throw_arraybounds();*((char*)_tmp98.curr)=_tmp9A;});}}
return Cyc_Sexp_mk_str((struct _fat_ptr)buf);}}}
# 657
static void Cyc_Sexp_print_tuple(struct _tuple10*self,struct Cyc_Sexp_Printer*p){
# 659
struct _fat_ptr _tmp9C=self->v;struct _fat_ptr vs=_tmp9C;
unsigned _tmp9D=_get_fat_size(vs,sizeof(struct Cyc_Sexp_Object));unsigned n=_tmp9D;
unsigned i=0U;for(0;i < n;++ i){
Cyc_Sexp_print(p,((struct Cyc_Sexp_Object*)vs.curr)[(int)i]);}}
# 668
static int Cyc_Sexp_hash_tuple(struct _tuple10*x){
unsigned res=0U;
struct _fat_ptr _tmp9E=x->v;struct _fat_ptr vs=_tmp9E;
unsigned _tmp9F=_get_fat_size(vs,sizeof(struct Cyc_Sexp_Object));unsigned n=_tmp9F;
{unsigned i=0U;for(0;i < n;++ i){
struct Cyc_Sexp_Object _tmpA0=((struct Cyc_Sexp_Object*)vs.curr)[(int)i];struct Cyc_Sexp_Object _stmttmp1=_tmpA0;struct Cyc_Sexp_Object _tmpA1=_stmttmp1;void*_tmpA2;_tmpA2=_tmpA1.self;{struct Cyc_Sexp_Obj*v=_tmpA2;
res=res << 8U | (unsigned)v & 255U;}}}
# 676
return(int)res;}
# 680
static int Cyc_Sexp_cmp_tuple(struct _tuple10*x,struct _tuple10*y){
if(x == y)return 0;{
struct _fat_ptr _tmpA3=x->v;struct _fat_ptr xs=_tmpA3;
struct _fat_ptr _tmpA4=y->v;struct _fat_ptr ys=_tmpA4;
unsigned _tmpA5=_get_fat_size(xs,sizeof(struct Cyc_Sexp_Object));unsigned nx=_tmpA5;
unsigned _tmpA6=_get_fat_size(ys,sizeof(struct Cyc_Sexp_Object));unsigned ny=_tmpA6;
if(nx < ny)return -1;
if(nx > ny)return 1;
{unsigned i=0U;for(0;i < nx;++ i){
int c=Cyc_Sexp_cmp(((struct Cyc_Sexp_Object*)xs.curr)[(int)i],((struct Cyc_Sexp_Object*)ys.curr)[(int)i]);
if(c != 0)return c;}}
# 692
return 0;}}
# 696
static struct Cyc_Sexp_Object Cyc_Sexp_next_list(struct Cyc_List_List**xs){
struct Cyc_Sexp_Object*_tmpA7=(struct Cyc_Sexp_Object*)(_check_null(*xs))->hd;struct Cyc_Sexp_Object*h=_tmpA7;
*xs=(_check_null(*xs))->tl;
return*h;}
# 703
static struct _tuple10*Cyc_Sexp_parse_tuple(struct Cyc_Sexp_Parser*p){
struct Cyc_List_List*xs=0;
Next: {
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);}
if(ch == 41){
Cyc_Sexp_pungetc(p,ch);
xs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(xs);{
int _tmpA8=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(xs);int len=_tmpA8;
struct _fat_ptr _tmpA9=({unsigned _tmpAB=(unsigned)len;struct Cyc_Sexp_Object*_tmpAA=_cycalloc(_check_times(_tmpAB,sizeof(struct Cyc_Sexp_Object)));({{unsigned _tmp122=(unsigned)len;unsigned i;for(i=0;i < _tmp122;++ i){({struct Cyc_Sexp_Object _tmp1AF=Cyc_Sexp_next_list(& xs);_tmpAA[i]=_tmp1AF;});}}0;});_tag_fat(_tmpAA,sizeof(struct Cyc_Sexp_Object),_tmpAB);});struct _fat_ptr vs=_tmpA9;
return Cyc_Sexp_mk_tuple(vs);}}
# 715
{struct Cyc_List_List*_tmpAC=Cyc_Sexp_classes;struct Cyc_List_List*cs=_tmpAC;for(0;cs != 0;cs=cs->tl){
struct Cyc_Sexp_Cls*_tmpAD=(struct Cyc_Sexp_Cls*)cs->hd;struct Cyc_Sexp_Cls*_stmttmp2=_tmpAD;struct Cyc_Sexp_Cls*_tmpAE=_stmttmp2;void*_tmpAF;_tmpAF=_tmpAE->vtable;{struct Cyc_Sexp_Class*v=_tmpAF;
if(ch == (int)v->tag){
Cyc_Sexp_expectws(p,40);{
struct Cyc_Sexp_Obj*_tmpB0=((v->parse))(p);struct Cyc_Sexp_Obj*obj=_tmpB0;
Cyc_Sexp_expectws(p,41);
xs=({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct Cyc_Sexp_Object*_tmp1B1=({struct Cyc_Sexp_Object*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct Cyc_Sexp_Object _tmp1B0=Cyc_Sexp_up(obj);*_tmpB1=_tmp1B0;});_tmpB1;});_tmpB2->hd=_tmp1B1;}),_tmpB2->tl=xs;_tmpB2;});
goto Next;}}}}}
# 725
({void(*_tmp1B4)(void*,int,struct _fat_ptr)=p->error;void*_tmp1B3=p->env;int _tmp1B2=ch;_tmp1B4(_tmp1B3,_tmp1B2,({const char*_tmpB3="unexpected tag";_tag_fat(_tmpB3,sizeof(char),15U);}));});}}static char _tmpB4[14U]="unsigned char";
# 742 "sexp.cyc"
struct Cyc_Sexp_Class Cyc_Sexp_uchar_class={(struct Cyc_Sexp_Obj*)0,'C',{_tmpB4,_tmpB4,_tmpB4 + 14U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_uchar,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_uchar,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_uchar,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_uchar,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_uchar,0};static char _tmpB5[12U]="signed char";
struct Cyc_Sexp_Class Cyc_Sexp_schar_class={(struct Cyc_Sexp_Obj*)0,'c',{_tmpB5,_tmpB5,_tmpB5 + 12U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_schar,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_schar,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_schar,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_schar,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_schar,0};static char _tmpB6[15U]="unsigned short";
struct Cyc_Sexp_Class Cyc_Sexp_ushort_class={(struct Cyc_Sexp_Obj*)0,'S',{_tmpB6,_tmpB6,_tmpB6 + 15U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_ushort,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_ushort,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_ushort,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_ushort,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_ushort,0};static char _tmpB7[13U]="signed short";
struct Cyc_Sexp_Class Cyc_Sexp_sshort_class={(struct Cyc_Sexp_Obj*)0,'s',{_tmpB7,_tmpB7,_tmpB7 + 13U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_sshort,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_sshort,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_sshort,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_sshort,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_sshort,0};static char _tmpB8[13U]="unsigned int";
struct Cyc_Sexp_Class Cyc_Sexp_uint_class={(struct Cyc_Sexp_Obj*)0,'I',{_tmpB8,_tmpB8,_tmpB8 + 13U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_uint,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_uint,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_uint,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_uint,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_uint,0};static char _tmpB9[11U]="signed int";
struct Cyc_Sexp_Class Cyc_Sexp_sint_class={(struct Cyc_Sexp_Obj*)0,'i',{_tmpB9,_tmpB9,_tmpB9 + 11U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_sint,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_sint,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_sint,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_sint,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_sint,0};static char _tmpBA[19U]="unsigned long long";
struct Cyc_Sexp_Class Cyc_Sexp_ulonglong_class={(struct Cyc_Sexp_Obj*)0,'L',{_tmpBA,_tmpBA,_tmpBA + 19U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_ulonglong,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_ulonglong,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_ulonglong,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_ulonglong,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_ulonglong,0};static char _tmpBB[17U]="signed long long";
struct Cyc_Sexp_Class Cyc_Sexp_slonglong_class={(struct Cyc_Sexp_Obj*)0,'l',{_tmpBB,_tmpBB,_tmpBB + 17U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_slonglong,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_slonglong,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_slonglong,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_slonglong,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_slonglong,0};static char _tmpBC[6U]="float";
struct Cyc_Sexp_Class Cyc_Sexp_float_class={(struct Cyc_Sexp_Obj*)0,'f',{_tmpBC,_tmpBC,_tmpBC + 6U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_float,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_float,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_float,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_float,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_float,0};static char _tmpBD[7U]="double";
struct Cyc_Sexp_Class Cyc_Sexp_double_class={(struct Cyc_Sexp_Obj*)0,'d',{_tmpBD,_tmpBD,_tmpBD + 7U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_double,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_double,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_double,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_double,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_double,0};static char _tmpBE[7U]="string";
struct Cyc_Sexp_Class Cyc_Sexp_str_class={(struct Cyc_Sexp_Obj*)0,'r',{_tmpBE,_tmpBE,_tmpBE + 7U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_str,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_str,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_str,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_str,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_str,0};static char _tmpBF[7U]="symbol";
struct Cyc_Sexp_Class Cyc_Sexp_symbol_class={(struct Cyc_Sexp_Obj*)0,'y',{_tmpBF,_tmpBF,_tmpBF + 7U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_symbol,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_symbol,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_symbol,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_symbol,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_symbol,0};static char _tmpC0[6U]="tuple";
struct Cyc_Sexp_Class Cyc_Sexp_tuple_class={(struct Cyc_Sexp_Obj*)0,'t',{_tmpC0,_tmpC0,_tmpC0 + 6U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_tuple,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_tuple,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_tuple,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_tuple,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_tuple,0};
# 762
static struct Cyc_Sexp_Obj**Cyc_Sexp_hashcons(struct Cyc_Sexp_Obj*x){
struct Cyc_Sexp_Class*_tmpC1=x->vtable;struct Cyc_Sexp_Class*c=_tmpC1;
struct Cyc_Hashtable_Table*topt=c->hash_table;
struct Cyc_Hashtable_Table*t;
if(topt == 0){
t=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*),int(*)(struct Cyc_Sexp_Obj*)))Cyc_Hashtable_create;})(101,c->cmp,c->hash);
c->hash_table=t;}else{
# 770
t=topt;}
# 772
return({(struct Cyc_Sexp_Obj**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Sexp_Obj*,int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*),int(*)(struct Cyc_Sexp_Obj*)))Cyc_Hashtable_lookup_other_opt;})(t,x,c->cmp,c->hash);}
# 789 "sexp.cyc"
struct _tuple0*Cyc_Sexp_mk_uchar(unsigned char v){struct _tuple0 _tmpC2=({struct _tuple0 _tmp123;_tmp123.vtable=& Cyc_Sexp_uchar_class,_tmp123.v=v;_tmp123;});struct _tuple0 obj=_tmpC2;struct _tuple0**_tmpC3=({(struct _tuple0**(*)(struct _tuple0*))Cyc_Sexp_hashcons;})(& obj);struct _tuple0**objopt=_tmpC3;if(objopt != 0)return*objopt;{struct _tuple0*_tmpC4=({struct _tuple0*_tmpC5=_cycalloc(sizeof(*_tmpC5));*_tmpC5=obj;_tmpC5;});struct _tuple0*objp=_tmpC4;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct _tuple0*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_uchar(unsigned char v){return({struct Cyc_Sexp_Object(*_tmp1B5)(struct _tuple0*)=({(struct Cyc_Sexp_Object(*)(struct _tuple0*))Cyc_Sexp_up;});_tmp1B5(Cyc_Sexp_mk_uchar(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_uchar_class_w={& Cyc_Sexp_uchar_class};
struct _tuple1*Cyc_Sexp_mk_schar(signed char v){struct _tuple1 _tmpC6=({struct _tuple1 _tmp124;_tmp124.vtable=& Cyc_Sexp_schar_class,_tmp124.v=v;_tmp124;});struct _tuple1 obj=_tmpC6;struct _tuple1**_tmpC7=({(struct _tuple1**(*)(struct _tuple1*))Cyc_Sexp_hashcons;})(& obj);struct _tuple1**objopt=_tmpC7;if(objopt != 0)return*objopt;{struct _tuple1*_tmpC8=({struct _tuple1*_tmpC9=_cycalloc(sizeof(*_tmpC9));*_tmpC9=obj;_tmpC9;});struct _tuple1*objp=_tmpC8;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple1*,struct _tuple1*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_schar(signed char v){return({struct Cyc_Sexp_Object(*_tmp1B6)(struct _tuple1*)=({(struct Cyc_Sexp_Object(*)(struct _tuple1*))Cyc_Sexp_up;});_tmp1B6(Cyc_Sexp_mk_schar(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_schar_class_w={& Cyc_Sexp_schar_class};
struct _tuple2*Cyc_Sexp_mk_ushort(unsigned short v){struct _tuple2 _tmpCA=({struct _tuple2 _tmp125;_tmp125.vtable=& Cyc_Sexp_ushort_class,_tmp125.v=v;_tmp125;});struct _tuple2 obj=_tmpCA;struct _tuple2**_tmpCB=({(struct _tuple2**(*)(struct _tuple2*))Cyc_Sexp_hashcons;})(& obj);struct _tuple2**objopt=_tmpCB;if(objopt != 0)return*objopt;{struct _tuple2*_tmpCC=({struct _tuple2*_tmpCD=_cycalloc(sizeof(*_tmpCD));*_tmpCD=obj;_tmpCD;});struct _tuple2*objp=_tmpCC;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct _tuple2*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_ushort(unsigned short v){return({struct Cyc_Sexp_Object(*_tmp1B7)(struct _tuple2*)=({(struct Cyc_Sexp_Object(*)(struct _tuple2*))Cyc_Sexp_up;});_tmp1B7(Cyc_Sexp_mk_ushort(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_ushort_class_w={& Cyc_Sexp_ushort_class};
struct _tuple3*Cyc_Sexp_mk_sshort(short v){struct _tuple3 _tmpCE=({struct _tuple3 _tmp126;_tmp126.vtable=& Cyc_Sexp_sshort_class,_tmp126.v=v;_tmp126;});struct _tuple3 obj=_tmpCE;struct _tuple3**_tmpCF=({(struct _tuple3**(*)(struct _tuple3*))Cyc_Sexp_hashcons;})(& obj);struct _tuple3**objopt=_tmpCF;if(objopt != 0)return*objopt;{struct _tuple3*_tmpD0=({struct _tuple3*_tmpD1=_cycalloc(sizeof(*_tmpD1));*_tmpD1=obj;_tmpD1;});struct _tuple3*objp=_tmpD0;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple3*,struct _tuple3*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_sshort(short v){return({struct Cyc_Sexp_Object(*_tmp1B8)(struct _tuple3*)=({(struct Cyc_Sexp_Object(*)(struct _tuple3*))Cyc_Sexp_up;});_tmp1B8(Cyc_Sexp_mk_sshort(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_sshort_class_w={& Cyc_Sexp_sshort_class};
struct _tuple4*Cyc_Sexp_mk_uint(unsigned v){struct _tuple4 _tmpD2=({struct _tuple4 _tmp127;_tmp127.vtable=& Cyc_Sexp_uint_class,_tmp127.v=v;_tmp127;});struct _tuple4 obj=_tmpD2;struct _tuple4**_tmpD3=({(struct _tuple4**(*)(struct _tuple4*))Cyc_Sexp_hashcons;})(& obj);struct _tuple4**objopt=_tmpD3;if(objopt != 0)return*objopt;{struct _tuple4*_tmpD4=({struct _tuple4*_tmpD5=_cycalloc(sizeof(*_tmpD5));*_tmpD5=obj;_tmpD5;});struct _tuple4*objp=_tmpD4;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple4*,struct _tuple4*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_uint(unsigned v){return({struct Cyc_Sexp_Object(*_tmp1B9)(struct _tuple4*)=({(struct Cyc_Sexp_Object(*)(struct _tuple4*))Cyc_Sexp_up;});_tmp1B9(Cyc_Sexp_mk_uint(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_uint_class_w={& Cyc_Sexp_uint_class};
struct _tuple5*Cyc_Sexp_mk_sint(int v){struct _tuple5 _tmpD6=({struct _tuple5 _tmp128;_tmp128.vtable=& Cyc_Sexp_sint_class,_tmp128.v=v;_tmp128;});struct _tuple5 obj=_tmpD6;struct _tuple5**_tmpD7=({(struct _tuple5**(*)(struct _tuple5*))Cyc_Sexp_hashcons;})(& obj);struct _tuple5**objopt=_tmpD7;if(objopt != 0)return*objopt;{struct _tuple5*_tmpD8=({struct _tuple5*_tmpD9=_cycalloc(sizeof(*_tmpD9));*_tmpD9=obj;_tmpD9;});struct _tuple5*objp=_tmpD8;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple5*,struct _tuple5*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_sint(int v){return({struct Cyc_Sexp_Object(*_tmp1BA)(struct _tuple5*)=({(struct Cyc_Sexp_Object(*)(struct _tuple5*))Cyc_Sexp_up;});_tmp1BA(Cyc_Sexp_mk_sint(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_sint_class_w={& Cyc_Sexp_sint_class};
struct _tuple6*Cyc_Sexp_mk_ulonglong(unsigned long long v){struct _tuple6 _tmpDA=({struct _tuple6 _tmp129;_tmp129.vtable=& Cyc_Sexp_ulonglong_class,_tmp129.v=v;_tmp129;});struct _tuple6 obj=_tmpDA;struct _tuple6**_tmpDB=({(struct _tuple6**(*)(struct _tuple6*))Cyc_Sexp_hashcons;})(& obj);struct _tuple6**objopt=_tmpDB;if(objopt != 0)return*objopt;{struct _tuple6*_tmpDC=({struct _tuple6*_tmpDD=_cycalloc(sizeof(*_tmpDD));*_tmpDD=obj;_tmpDD;});struct _tuple6*objp=_tmpDC;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple6*,struct _tuple6*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_ulonglong(unsigned long long v){return({struct Cyc_Sexp_Object(*_tmp1BB)(struct _tuple6*)=({(struct Cyc_Sexp_Object(*)(struct _tuple6*))Cyc_Sexp_up;});_tmp1BB(Cyc_Sexp_mk_ulonglong(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_ulonglong_class_w={& Cyc_Sexp_ulonglong_class};
struct _tuple7*Cyc_Sexp_mk_slonglong(long long v){struct _tuple7 _tmpDE=({struct _tuple7 _tmp12A;_tmp12A.vtable=& Cyc_Sexp_slonglong_class,_tmp12A.v=v;_tmp12A;});struct _tuple7 obj=_tmpDE;struct _tuple7**_tmpDF=({(struct _tuple7**(*)(struct _tuple7*))Cyc_Sexp_hashcons;})(& obj);struct _tuple7**objopt=_tmpDF;if(objopt != 0)return*objopt;{struct _tuple7*_tmpE0=({struct _tuple7*_tmpE1=_cycalloc(sizeof(*_tmpE1));*_tmpE1=obj;_tmpE1;});struct _tuple7*objp=_tmpE0;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple7*,struct _tuple7*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_slonglong(long long v){return({struct Cyc_Sexp_Object(*_tmp1BC)(struct _tuple7*)=({(struct Cyc_Sexp_Object(*)(struct _tuple7*))Cyc_Sexp_up;});_tmp1BC(Cyc_Sexp_mk_slonglong(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_slonglong_class_w={& Cyc_Sexp_slonglong_class};
struct _tuple8*Cyc_Sexp_mk_float(float v){struct _tuple8 _tmpE2=({struct _tuple8 _tmp12B;_tmp12B.vtable=& Cyc_Sexp_float_class,_tmp12B.v=v;_tmp12B;});struct _tuple8 obj=_tmpE2;struct _tuple8**_tmpE3=({(struct _tuple8**(*)(struct _tuple8*))Cyc_Sexp_hashcons;})(& obj);struct _tuple8**objopt=_tmpE3;if(objopt != 0)return*objopt;{struct _tuple8*_tmpE4=({struct _tuple8*_tmpE5=_cycalloc(sizeof(*_tmpE5));*_tmpE5=obj;_tmpE5;});struct _tuple8*objp=_tmpE4;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple8*,struct _tuple8*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_float(float v){return({struct Cyc_Sexp_Object(*_tmp1BD)(struct _tuple8*)=({(struct Cyc_Sexp_Object(*)(struct _tuple8*))Cyc_Sexp_up;});_tmp1BD(Cyc_Sexp_mk_float(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_float_class_w={& Cyc_Sexp_float_class};
struct _tuple9*Cyc_Sexp_mk_double(double v){struct _tuple9 _tmpE6=({struct _tuple9 _tmp12C;_tmp12C.vtable=& Cyc_Sexp_double_class,_tmp12C.v=v;_tmp12C;});struct _tuple9 obj=_tmpE6;struct _tuple9**_tmpE7=({(struct _tuple9**(*)(struct _tuple9*))Cyc_Sexp_hashcons;})(& obj);struct _tuple9**objopt=_tmpE7;if(objopt != 0)return*objopt;{struct _tuple9*_tmpE8=({struct _tuple9*_tmpE9=_cycalloc(sizeof(*_tmpE9));*_tmpE9=obj;_tmpE9;});struct _tuple9*objp=_tmpE8;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple9*,struct _tuple9*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_double(double v){return({struct Cyc_Sexp_Object(*_tmp1BE)(struct _tuple9*)=({(struct Cyc_Sexp_Object(*)(struct _tuple9*))Cyc_Sexp_up;});_tmp1BE(Cyc_Sexp_mk_double(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_double_class_w={& Cyc_Sexp_double_class};
struct _tuple10*Cyc_Sexp_mk_str(struct _fat_ptr v){struct _tuple10 _tmpEA=({struct _tuple10 _tmp12D;_tmp12D.vtable=& Cyc_Sexp_str_class,_tmp12D.v=v;_tmp12D;});struct _tuple10 obj=_tmpEA;struct _tuple10**_tmpEB=({(struct _tuple10**(*)(struct _tuple10*))Cyc_Sexp_hashcons;})(& obj);struct _tuple10**objopt=_tmpEB;if(objopt != 0)return*objopt;{struct _tuple10*_tmpEC=({struct _tuple10*_tmpED=_cycalloc(sizeof(*_tmpED));*_tmpED=obj;_tmpED;});struct _tuple10*objp=_tmpEC;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_str(struct _fat_ptr v){return({struct Cyc_Sexp_Object(*_tmp1BF)(struct _tuple10*)=({(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;});_tmp1BF(Cyc_Sexp_mk_str(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_str_class_w={& Cyc_Sexp_str_class};
struct _tuple10*Cyc_Sexp_mk_symbol(struct _fat_ptr v){struct _tuple10 _tmpEE=({struct _tuple10 _tmp12E;_tmp12E.vtable=& Cyc_Sexp_symbol_class,_tmp12E.v=v;_tmp12E;});struct _tuple10 obj=_tmpEE;struct _tuple10**_tmpEF=({(struct _tuple10**(*)(struct _tuple10*))Cyc_Sexp_hashcons;})(& obj);struct _tuple10**objopt=_tmpEF;if(objopt != 0)return*objopt;{struct _tuple10*_tmpF0=({struct _tuple10*_tmpF1=_cycalloc(sizeof(*_tmpF1));*_tmpF1=obj;_tmpF1;});struct _tuple10*objp=_tmpF0;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_symbol(struct _fat_ptr v){return({struct Cyc_Sexp_Object(*_tmp1C0)(struct _tuple10*)=({(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;});_tmp1C0(Cyc_Sexp_mk_symbol(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_symbol_class_w={& Cyc_Sexp_symbol_class};
struct _tuple10*Cyc_Sexp_mk_tuple(struct _fat_ptr v){struct _tuple10 _tmpF2=({struct _tuple10 _tmp12F;_tmp12F.vtable=& Cyc_Sexp_tuple_class,_tmp12F.v=v;_tmp12F;});struct _tuple10 obj=_tmpF2;struct _tuple10**_tmpF3=({(struct _tuple10**(*)(struct _tuple10*))Cyc_Sexp_hashcons;})(& obj);struct _tuple10**objopt=_tmpF3;if(objopt != 0)return*objopt;{struct _tuple10*_tmpF4=({struct _tuple10*_tmpF5=_cycalloc(sizeof(*_tmpF5));*_tmpF5=obj;_tmpF5;});struct _tuple10*objp=_tmpF4;({(void(*)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*))Cyc_Hashtable_insert;})(_check_null((obj.vtable)->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_tuple(struct _fat_ptr v){return({struct Cyc_Sexp_Object(*_tmp1C1)(struct _tuple10*)=({(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;});_tmp1C1(Cyc_Sexp_mk_tuple(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_tuple_class_w={& Cyc_Sexp_tuple_class};
# 805
struct Cyc_Sexp_Object Cyc_Sexp_tuple(struct _fat_ptr objs){
return Cyc_Sexp_obj_tuple(({unsigned _tmpF7=_get_fat_size(objs,sizeof(struct Cyc_Sexp_Object));struct Cyc_Sexp_Object*_tmpF6=_cycalloc(_check_times(_tmpF7,sizeof(struct Cyc_Sexp_Object)));({{unsigned _tmp130=_get_fat_size(objs,sizeof(struct Cyc_Sexp_Object));unsigned i;for(i=0;i < _tmp130;++ i){_tmpF6[i]=((struct Cyc_Sexp_Object*)objs.curr)[(int)i];}}0;});_tag_fat(_tmpF6,sizeof(struct Cyc_Sexp_Object),_tmpF7);}));}
# 811
static struct Cyc_List_List Cyc_Sexp_c0={(void*)& Cyc_Sexp_uchar_class_w,0};
static struct Cyc_List_List Cyc_Sexp_c1={(void*)& Cyc_Sexp_schar_class_w,& Cyc_Sexp_c0};
static struct Cyc_List_List Cyc_Sexp_c2={(void*)& Cyc_Sexp_ushort_class_w,& Cyc_Sexp_c1};
static struct Cyc_List_List Cyc_Sexp_c3={(void*)& Cyc_Sexp_sshort_class_w,& Cyc_Sexp_c2};
static struct Cyc_List_List Cyc_Sexp_c4={(void*)& Cyc_Sexp_uint_class_w,& Cyc_Sexp_c3};
static struct Cyc_List_List Cyc_Sexp_c5={(void*)& Cyc_Sexp_sint_class_w,& Cyc_Sexp_c4};
static struct Cyc_List_List Cyc_Sexp_c6={(void*)& Cyc_Sexp_ulonglong_class_w,& Cyc_Sexp_c5};
static struct Cyc_List_List Cyc_Sexp_c7={(void*)& Cyc_Sexp_slonglong_class_w,& Cyc_Sexp_c6};
static struct Cyc_List_List Cyc_Sexp_c8={(void*)& Cyc_Sexp_float_class_w,& Cyc_Sexp_c7};
static struct Cyc_List_List Cyc_Sexp_c9={(void*)& Cyc_Sexp_double_class_w,& Cyc_Sexp_c8};
static struct Cyc_List_List Cyc_Sexp_c10={(void*)& Cyc_Sexp_symbol_class_w,& Cyc_Sexp_c9};
static struct Cyc_List_List Cyc_Sexp_c11={(void*)& Cyc_Sexp_str_class_w,& Cyc_Sexp_c10};
static struct Cyc_List_List Cyc_Sexp_c12={(void*)& Cyc_Sexp_tuple_class_w,& Cyc_Sexp_c11};
# 825
struct Cyc_List_List*Cyc_Sexp_classes=& Cyc_Sexp_c12;
# 828
struct Cyc_Sexp_Visitor Cyc_Sexp_empty_visitor (void){
return({struct Cyc_Sexp_Visitor _tmp131;_tmp131.visit_uchar=0,_tmp131.visit_schar=0,_tmp131.visit_ushort=0,_tmp131.visit_sshort=0,_tmp131.visit_uint=0,_tmp131.visit_sint=0,_tmp131.visit_ulonglong=0,_tmp131.visit_slonglong=0,_tmp131.visit_float=0,_tmp131.visit_double=0,_tmp131.visit_symbol=0,_tmp131.visit_str=0,_tmp131.visit_tuple=0,_tmp131.visit_default=0;_tmp131;});}
# 846
struct Cyc_Sexp_Visitor Cyc_Sexp_default_visitor(void*(*def)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)){
struct Cyc_Sexp_Visitor _tmpF8=Cyc_Sexp_empty_visitor();struct Cyc_Sexp_Visitor v=_tmpF8;
v.visit_default=def;
return v;}
