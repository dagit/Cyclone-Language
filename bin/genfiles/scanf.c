#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

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

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
};

/* Regions */
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);
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
extern int _throw_null_fn(const char *filename, unsigned lineno);
extern int _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern int _throw_badalloc_fn(const char *filename, unsigned lineno);
extern int _throw_match_fn(const char *filename, unsigned lineno);
extern int _throw_fn(void* e, const char *filename, unsigned lineno);
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
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#define _INLINE_FUNCTIONS
#else
#define _INLINE inline
#endif

#ifdef VC_C
#define _CYC_U_LONG_LONG_T __int64
#else
#ifdef GCC_C
#define _CYC_U_LONG_LONG_T unsigned long long
#else
#define _CYC_U_LONG_LONG_T unsigned long long
#endif
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE void *
_check_null_fn(const void *ptr, const char *filename, unsigned lineno) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null_fn(filename,lineno);
  return _check_null_temp;
}
#define _check_null(p) (_check_null_fn((p),__FILE__,__LINE__))
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE char *
_check_known_subscript_null_fn(void *ptr, unsigned bound, unsigned elt_sz, unsigned index, const char *filename, unsigned lineno) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null_fn(filename,lineno);
  if (_cks_index >= _cks_bound) _throw_arraybounds_fn(filename,lineno);
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#define _check_known_subscript_null(p,b,e) (_check_known_subscript_null_fn(p,b,e,__FILE__,__LINE__))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_check_known_subscript_notnull_fn(unsigned bound,unsigned index,const char *filename,unsigned lineno) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds_fn(filename,lineno); 
  return _cksnn_index;
}
#define _check_known_subscript_notnull(b,i) (_check_known_subscript_notnull_fn(b,i,__FILE__,__LINE__))
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char_fn(char *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
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
static _INLINE int
_get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset) {
  const char *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset) {
  const short *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset) {
  const int *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset) {
  const float *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset) {
  const double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset) {
  const long double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset) {
  const void **_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}


/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus_<type>_fn. */
static _INLINE void 
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn(x,i,__FILE__,__LINE__)

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
static _INLINE char *
_zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript_fn(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index,const char *filename, unsigned lineno) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _cus_ans;
}
#define _check_dyneither_subscript(a,s,i) \
  _check_dyneither_subscript(a,s,i,__FILE__,__LINE__)
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
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_untag_dyneither_ptr_fn(struct _dyneither_ptr arr, 
                        unsigned elt_sz,unsigned num_elts,
                        const char *filename, unsigned lineno) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

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
static _INLINE void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  _CYC_U_LONG_LONG_T whole_ans = 
    ((_CYC_U_LONG_LONG_T)x)*((_CYC_U_LONG_LONG_T)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
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
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif

/* the next two routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}

# 35 "core.h"
 typedef char*Cyc_Cstring;
# 36
typedef char*Cyc_CstringNN;
# 37
typedef struct _dyneither_ptr Cyc_string_t;
# 40
typedef struct _dyneither_ptr Cyc_mstring_t;
# 43
typedef struct _dyneither_ptr*Cyc_stringptr_t;
# 47
typedef struct _dyneither_ptr*Cyc_mstringptr_t;
# 50
typedef char*Cyc_Cbuffer_t;
# 52
typedef char*Cyc_CbufferNN_t;
# 54
typedef struct _dyneither_ptr Cyc_buffer_t;
# 56
typedef struct _dyneither_ptr Cyc_mbuffer_t;
# 59
typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 26 "/home/jgm/cyclone/include/cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 52
extern struct Cyc___cycFILE*Cyc_stdin;struct Cyc_String_pa_PrintArg_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};
struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 142 "/home/jgm/cyclone/include/cycboot.h"
int Cyc_getc(struct Cyc___cycFILE*);
# 197 "/home/jgm/cyclone/include/cycboot.h"
int Cyc_sscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);
# 222 "/home/jgm/cyclone/include/cycboot.h"
int Cyc_ungetc(int,struct Cyc___cycFILE*);
# 247
int Cyc_vsscanf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;
struct _dyneither_ptr f1;};
# 300 "/home/jgm/cyclone/include/cycboot.h"
int isspace(int);
# 302
int isupper(int);
# 314
double atof(const char*);
# 315
long strtol(char*,char**,int);
# 317
unsigned long strtoul(char*,char**,int);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 249
if(bd >> 20  || sz >> 12)
# 250
return 0;{
# 251
unsigned char*ptrbd=dyn.curr + bd * sz;
# 252
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
# 257
return dyn.curr;};}
# 80 "/home/jgm/cyclone/lib/scanf.cyc"
typedef long long Cyc__IO_ssize_t;
# 126 "/home/jgm/cyclone/lib/scanf.cyc"
static struct _dyneither_ptr Cyc___sccl(char*tab,struct _dyneither_ptr fmt);
# 139
static short*Cyc_va_arg_short_ptr(void*a){
# 140
void*_tmp0=a;short*_tmp2;unsigned short*_tmp4;_LL1: {struct Cyc_ShortPtr_sa_ScanfArg_struct*
_tmp1=(struct Cyc_ShortPtr_sa_ScanfArg_struct*)_tmp0;if(_tmp1->tag != 0)goto _LL3;
else{_tmp2=_tmp1->f1;}}_LL2:
# 141
 return _tmp2;_LL3: {struct Cyc_UShortPtr_sa_ScanfArg_struct*_tmp3=(struct Cyc_UShortPtr_sa_ScanfArg_struct*)
_tmp0;if(_tmp3->tag != 1)goto _LL5;else{_tmp4=_tmp3->f1;}}_LL4:
# 142
 return(short*)_tmp4;_LL5:;_LL6: {
# 143
struct Cyc_Core_Invalid_argument_exn_struct _tmp48;const char*_tmp47;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp46;(int)_throw((void*)((_tmp46=_cycalloc(sizeof(*_tmp46)),((_tmp46[0]=((
_tmp48.tag=Cyc_Core_Invalid_argument,((_tmp48.f1=((_tmp47="scan expects short pointer",
_tag_dyneither(_tmp47,sizeof(char),27))),_tmp48)))),_tmp46)))));}_LL0:;}
# 147
static int*Cyc_va_arg_int_ptr(void*a){
# 148
void*_tmp8=a;int*_tmpA;unsigned int*_tmpC;_LL8: {struct Cyc_IntPtr_sa_ScanfArg_struct*
_tmp9=(struct Cyc_IntPtr_sa_ScanfArg_struct*)_tmp8;if(_tmp9->tag != 2)goto _LLA;
else{_tmpA=_tmp9->f1;}}_LL9:
# 149
 return _tmpA;_LLA: {struct Cyc_UIntPtr_sa_ScanfArg_struct*_tmpB=(struct Cyc_UIntPtr_sa_ScanfArg_struct*)
_tmp8;if(_tmpB->tag != 3)goto _LLC;else{_tmpC=_tmpB->f1;}}_LLB:
# 150
 return(int*)_tmpC;_LLC:;_LLD: {
# 151
struct Cyc_Core_Invalid_argument_exn_struct _tmp4E;const char*_tmp4D;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp4C;(int)_throw((void*)((_tmp4C=_cycalloc(sizeof(*_tmp4C)),((_tmp4C[0]=((
_tmp4E.tag=Cyc_Core_Invalid_argument,((_tmp4E.f1=((_tmp4D="scan expects int pointer",
_tag_dyneither(_tmp4D,sizeof(char),25))),_tmp4E)))),_tmp4C)))));}_LL7:;}
# 155
static struct _dyneither_ptr Cyc_va_arg_string_ptr(void*a){
# 156
void*_tmp10=a;struct _dyneither_ptr _tmp12;struct _dyneither_ptr _tmp14;_LLF: {
struct Cyc_StringPtr_sa_ScanfArg_struct*_tmp11=(struct Cyc_StringPtr_sa_ScanfArg_struct*)
_tmp10;if(_tmp11->tag != 4)goto _LL11;else{_tmp12=_tmp11->f1;}}_LL10:
# 157
 return _dyneither_ptr_decrease_size(_tmp12,sizeof(char),1);_LL11: {struct Cyc_CharPtr_sa_ScanfArg_struct*
_tmp13=(struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmp10;if(_tmp13->tag != 7)goto
_LL13;else{_tmp14=_tmp13->f1;}}_LL12:
# 158
 return _tmp14;_LL13:;_LL14: {
# 159
struct Cyc_Core_Invalid_argument_exn_struct _tmp54;const char*_tmp53;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp52;(int)_throw((void*)((_tmp52=_cycalloc(sizeof(*_tmp52)),((_tmp52[0]=((
_tmp54.tag=Cyc_Core_Invalid_argument,((_tmp54.f1=((_tmp53="scan expects char pointer",
_tag_dyneither(_tmp53,sizeof(char),26))),_tmp54)))),_tmp52)))));}_LLE:;}
# 163
static double*Cyc_va_arg_double_ptr(void*a){
# 164
void*_tmp18=a;double*_tmp1A;_LL16: {struct Cyc_DoublePtr_sa_ScanfArg_struct*
_tmp19=(struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmp18;if(_tmp19->tag != 5)goto
_LL18;else{_tmp1A=_tmp19->f1;}}_LL17:
# 165
 return _tmp1A;_LL18:;_LL19: {
# 166
struct Cyc_Core_Invalid_argument_exn_struct _tmp5A;const char*_tmp59;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp58;(int)_throw((void*)((_tmp58=_cycalloc(sizeof(*_tmp58)),((_tmp58[0]=((
_tmp5A.tag=Cyc_Core_Invalid_argument,((_tmp5A.f1=((_tmp59="scan expects double pointer",
_tag_dyneither(_tmp59,sizeof(char),28))),_tmp5A)))),_tmp58)))));}_LL15:;}
# 170
static float*Cyc_va_arg_float_ptr(void*a){
# 171
void*_tmp1E=a;float*_tmp20;_LL1B: {struct Cyc_FloatPtr_sa_ScanfArg_struct*_tmp1F=(
struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp1E;if(_tmp1F->tag != 6)goto _LL1D;else{
_tmp20=_tmp1F->f1;}}_LL1C:
# 172
 return _tmp20;_LL1D:;_LL1E: {
# 173
struct Cyc_Core_Invalid_argument_exn_struct _tmp60;const char*_tmp5F;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp5E;(int)_throw((void*)((_tmp5E=_cycalloc(sizeof(*_tmp5E)),((_tmp5E[0]=((
_tmp60.tag=Cyc_Core_Invalid_argument,((_tmp60.f1=((_tmp5F="scan expects float pointer",
_tag_dyneither(_tmp5F,sizeof(char),27))),_tmp60)))),_tmp5E)))));}_LL1A:;}
# 177
static struct _dyneither_ptr Cyc_va_arg_char_ptr(void*a){
# 178
void*_tmp24=a;struct _dyneither_ptr _tmp26;struct _dyneither_ptr _tmp28;_LL20: {
struct Cyc_CharPtr_sa_ScanfArg_struct*_tmp25=(struct Cyc_CharPtr_sa_ScanfArg_struct*)
_tmp24;if(_tmp25->tag != 7)goto _LL22;else{_tmp26=_tmp25->f1;}}_LL21:
# 179
 return _tmp26;_LL22: {struct Cyc_StringPtr_sa_ScanfArg_struct*_tmp27=(struct Cyc_StringPtr_sa_ScanfArg_struct*)
_tmp24;if(_tmp27->tag != 4)goto _LL24;else{_tmp28=_tmp27->f1;}}_LL23:
# 180
 return _dyneither_ptr_decrease_size(_tmp28,sizeof(char),1);_LL24:;_LL25: {
# 181
struct Cyc_Core_Invalid_argument_exn_struct _tmp66;const char*_tmp65;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp64;(int)_throw((void*)((_tmp64=_cycalloc(sizeof(*_tmp64)),((_tmp64[0]=((
_tmp66.tag=Cyc_Core_Invalid_argument,((_tmp66.f1=((_tmp65="scan expects char pointer",
_tag_dyneither(_tmp65,sizeof(char),26))),_tmp66)))),_tmp64)))));}_LL1F:;}
# 188
int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*
_IO_peekc)(void*),void*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*
errp){
# 198
struct _dyneither_ptr fmt=fmt0;
# 199
int c;
# 200
long long width;
# 201
struct _dyneither_ptr p=_tag_dyneither(0,0,0);
# 202
int n;
# 203
int flags=0;
# 205
struct _dyneither_ptr p0=_tag_dyneither(0,0,0);
# 206
int nassigned;
# 207
int nread;
# 209
int base=0;
# 210
int use_strtoul=0;
# 213
char ccltab[256];
# 215
char buf[351];
# 217
{unsigned int _tmp42=350;unsigned int i;for(i=0;i < _tmp42;i ++){buf[i]='0';}buf[
_tmp42]=(char)0;}{int seen_eof=0;
# 220
static short basefix[17]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
# 223
nassigned=0;
# 224
nread=0;
# 225
for(0;1;0){
# 226
c=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));
# 227
if(c == 0)
# 228
goto done;
# 229
if(isspace(c)){
# 230
for(0;1;0){
# 231
c=_IO_getc(fp);
# 232
if(c == - 1){
# 233
++ seen_eof;
# 234
break;}
# 236
if(!isspace(c)){
# 237
_IO_ungetc(c,fp);
# 238
break;}
# 240
++ nread;}
# 242
continue;}
# 244
if(c != '%')
# 245
goto literal;
# 246
width=(long long)0;
# 247
flags=0;
# 252
again: c=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));
# 253
switch(c){case '%': _LL26:
# 255
 literal:
# 256
 n=_IO_getc(fp);
# 257
if(n == - 1)
# 258
goto eof_failure;
# 259
if(n != c){
# 260
_IO_ungetc(n,fp);
# 261
goto match_failure;}
# 263
++ nread;
# 264
continue;case '*': _LL27:
# 267
 if(flags)goto control_failure;
# 268
flags=8;
# 269
goto again;case 'l': _LL28:
# 271
 if(flags & ~(8 | 64))goto control_failure;
# 272
flags |=1;
# 273
goto again;case 'L': _LL29:
# 275
 if(flags & ~(8 | 64))goto control_failure;
# 276
flags |=2;
# 277
goto again;case 'h': _LL2A:
# 279
 if(flags & ~(8 | 64))goto control_failure;
# 280
flags |=4;
# 281
goto again;case '0': _LL2B:
# 283
 goto _LL2C;case '1': _LL2C: goto _LL2D;case '2': _LL2D: goto _LL2E;case '3': _LL2E: goto
_LL2F;case '4': _LL2F: goto _LL30;case '5': _LL30:
# 284
 goto _LL31;case '6': _LL31: goto _LL32;case '7': _LL32: goto _LL33;case '8': _LL33: goto
_LL34;case '9': _LL34:
# 285
 if(flags & ~(8 | 64))goto control_failure;
# 286
flags |=64;
# 287
width=(width * 10 + c)- '0';
# 288
goto again;case 'D': _LL35:
# 298 "/home/jgm/cyclone/lib/scanf.cyc"
 flags |=1;
# 299
goto _LL36;case 'd': _LL36:
# 301
 c=3;
# 302
use_strtoul=0;
# 303
base=10;
# 304
break;case 'i': _LL37:
# 307
 c=3;
# 308
use_strtoul=0;
# 309
base=0;
# 310
break;case 'O': _LL38:
# 313
 flags |=1;
# 314
goto _LL39;case 'o': _LL39:
# 316
 c=3;
# 317
use_strtoul=1;
# 318
base=8;
# 319
break;case 'u': _LL3A:
# 322
 c=3;
# 323
use_strtoul=1;
# 324
base=10;
# 325
break;case 'X': _LL3B:
# 327
 goto _LL3C;case 'x': _LL3C:
# 328
 flags |=256;
# 329
c=3;
# 330
use_strtoul=1;
# 331
base=16;
# 332
break;case 'E': _LL3D:
# 334
 goto _LL3E;case 'F': _LL3E: goto _LL3F;case 'e': _LL3F: goto _LL40;case 'f': _LL40: goto
_LL41;case 'g': _LL41:
# 335
 c=4;
# 336
break;case 's': _LL42:
# 339
 c=2;
# 340
break;case '[': _LL43:
# 343
 fmt=Cyc___sccl(ccltab,fmt);
# 344
flags |=32;
# 345
c=1;
# 346
break;case 'c': _LL44:
# 349
 flags |=32;
# 350
c=0;
# 351
break;case 'p': _LL45:
# 354
 flags |=16 | 256;
# 355
c=3;
# 356
use_strtoul=1;
# 357
base=16;
# 358
break;case 'n': _LL46:
# 361
 if(flags & 8)
# 362
continue;
# 363
if(flags & 4)
# 364
*Cyc_va_arg_short_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
short)nread;else{
# 365
if(flags & 1)
# 366
*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
long)nread;else{
# 368
*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
int)nread;}}
# 369
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
# 370
continue;case '\000': _LL47:
# 376
 nassigned=- 1;
# 377
goto done;default: _LL48:
# 380
 if(isupper(c))
# 381
flags |=1;
# 382
c=3;
# 383
use_strtoul=0;
# 384
base=10;
# 385
break;}
# 391
if(_IO_peekc(fp)== - 1)
# 392
goto eof_failure;
# 398
if((flags & 32)== 0){
# 399
n=_IO_peekc(fp);
# 400
while(isspace(n)){
# 401
n=_IO_getc(fp);
# 402
++ nread;
# 403
n=_IO_peekc(fp);
# 404
if(n == - 1)
# 405
goto eof_failure;}}
# 415
switch(c){case 0: _LL4A:
# 421
 if(width == 0)
# 422
width=(long long)1;
# 423
if(flags & 8){
# 424
long long sum=(long long)0;
# 425
for(0;width > 0;0){
# 426
n=_IO_getc(fp);
# 427
if(n == - 1  && width != 0)
# 428
goto eof_failure;else{
# 429
if(n == - 1){
# 430
++ seen_eof;
# 431
break;}}
# 433
++ sum;
# 434
-- width;}
# 436
nread +=sum;}else{
# 438
long long sum=(long long)0;
# 439
struct _dyneither_ptr _tmp2C=Cyc_va_arg_char_ptr(*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)));_dyneither_ptr_inplace_plus(& ap,
sizeof(void*),1);
# 440
for(0;width > 0;0){
# 441
n=_IO_getc(fp);
# 442
if(n == - 1  && width != 0)
# 443
goto eof_failure;else{
# 444
if(n == - 1){
# 445
++ seen_eof;
# 446
break;}}
# 448
*((char*)_check_dyneither_subscript(_tmp2C,sizeof(char),0))=(char)n;
# 449
_dyneither_ptr_inplace_plus(& _tmp2C,sizeof(char),1);
# 450
++ sum;
# 451
-- width;}
# 453
nread +=sum;
# 454
++ nassigned;}
# 456
break;case 1: _LL4B:
# 460
 if(width == 0)
# 461
width=(long long)~ 0;
# 463
if(flags & 8){
# 464
n=0;{
# 465
int c=_IO_peekc(fp);
# 466
while((int)ccltab[_check_known_subscript_notnull(256,(int)((char)c))]){
# 467
++ n;
# 468
_IO_getc(fp);
# 469
if(-- width == 0)
# 470
break;
# 471
if((c=_IO_peekc(fp))== - 1){
# 472
if(n == 0)
# 473
goto eof_failure;
# 474
++ seen_eof;
# 475
break;}}
# 478
if(n == 0)
# 479
goto match_failure;};}else{
# 481
struct _dyneither_ptr p4=(struct _dyneither_ptr)Cyc_va_arg_string_ptr(*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)));_dyneither_ptr_inplace_plus(& ap,
sizeof(void*),1);{
# 482
struct _dyneither_ptr p5=p4;
# 483
int c=_IO_peekc(fp);
# 484
while((int)ccltab[_check_known_subscript_notnull(256,(int)((char)c))]){
# 485
if(_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;
# 486
*((char*)_check_dyneither_subscript(p5,sizeof(char),0))=(char)c;
# 487
_dyneither_ptr_inplace_plus(& p5,sizeof(char),1);
# 488
_IO_getc(fp);
# 489
if(-- width == 0)
# 490
break;
# 491
if((c=_IO_peekc(fp))== - 1){
# 492
if((char*)p5.curr == (char*)p0.curr)
# 493
goto eof_failure;
# 494
++ seen_eof;
# 495
break;}}
# 498
n=(p5.curr - p4.curr)/ sizeof(char);
# 499
if(n == 0)
# 500
goto match_failure;
# 501
if(_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;
# 502
*((char*)_check_dyneither_subscript(p5,sizeof(char),0))='\000';
# 503
++ nassigned;};}
# 505
nread +=n;
# 506
break;case 2: _LL4C:
# 510
 if(width == 0)
# 511
width=(long long)~ 0;
# 512
if(flags & 8){
# 513
n=0;{
# 514
int c=_IO_peekc(fp);
# 515
while(!isspace((int)((unsigned char)c))){
# 516
++ n;
# 517
_IO_getc(fp);
# 518
if(-- width == 0)
# 519
break;
# 520
if((c=_IO_peekc(fp))== - 1){
# 521
++ seen_eof;
# 522
break;}}
# 525
nread +=n;};}else{
# 527
struct _dyneither_ptr _tmp2D=Cyc_va_arg_string_ptr(*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0)));_dyneither_ptr_inplace_plus(& ap,
sizeof(void*),1);{
# 528
struct _dyneither_ptr _tmp2E=_tmp2D;
# 529
int c=_IO_peekc(fp);
# 530
while(!isspace((int)((unsigned char)c))){
# 531
c=_IO_getc(fp);
# 532
if(_get_dyneither_size(_tmp2E,sizeof(char))== 0)goto eof_failure;
# 533
*((char*)_check_dyneither_subscript(_tmp2E,sizeof(char),0))=(char)c;
# 534
_dyneither_ptr_inplace_plus(& _tmp2E,sizeof(char),1);
# 535
if(-- width == 0)
# 536
break;
# 537
if((c=_IO_peekc(fp))== - 1){
# 538
++ seen_eof;
# 539
break;}}
# 542
if(_get_dyneither_size(_tmp2E,sizeof(char))== 0)goto eof_failure;
# 543
*((char*)_check_dyneither_subscript(_tmp2E,sizeof(char),0))='\000';
# 544
nread +=(_tmp2E.curr - _tmp2D.curr)/ sizeof(char);
# 545
++ nassigned;};}
# 547
continue;case 3: _LL4D:
# 551
 if(width == 0  || width > sizeof(buf)- 1)
# 552
width=(long long)(sizeof(buf)- 1);
# 553
flags |=(64 | 128)| 512;
# 554
{char*_tmp67;for(p=((_tmp67=buf,_tag_dyneither(_tmp67,sizeof(char),
_get_zero_arr_size_char((void*)_tmp67,351))));width != 0;-- width){
# 555
c=(int)((unsigned char)_IO_peekc(fp));
# 560
switch(c){case '0': _LL4F:
# 575 "/home/jgm/cyclone/lib/scanf.cyc"
 if(base == 0){
# 576
base=8;
# 577
flags |=256;}
# 579
if(flags & 512)
# 580
flags &=~((64 | 512)| 128);else{
# 582
flags &=~((64 | 256)| 128);}
# 583
goto ok;case '1': _LL50:
# 586
 goto _LL51;case '2': _LL51: goto _LL52;case '3': _LL52: goto _LL53;case '4': _LL53: goto
_LL54;case '5': _LL54:
# 587
 goto _LL55;case '6': _LL55: goto _LL56;case '7': _LL56:
# 588
 base=(int)basefix[_check_known_subscript_notnull(17,base)];
# 589
flags &=~((64 | 256)| 128);
# 590
goto ok;case '8': _LL57:
# 593
 goto _LL58;case '9': _LL58:
# 594
 base=(int)basefix[_check_known_subscript_notnull(17,base)];
# 595
if(base <= 8)
# 596
break;
# 597
flags &=~((64 | 256)| 128);
# 598
goto ok;case 'A': _LL59:
# 601
 goto _LL5A;case 'B': _LL5A: goto _LL5B;case 'C': _LL5B: goto _LL5C;case 'D': _LL5C: goto
_LL5D;case 'E': _LL5D:
# 602
 goto _LL5E;case 'F': _LL5E: goto _LL5F;case 'a': _LL5F: goto _LL60;case 'b': _LL60: goto
_LL61;case 'c': _LL61:
# 603
 goto _LL62;case 'd': _LL62: goto _LL63;case 'e': _LL63: goto _LL64;case 'f': _LL64:
# 605
 if(base <= 10)
# 606
break;
# 607
flags &=~((64 | 256)| 128);
# 608
goto ok;case '+': _LL65:
# 611
 goto _LL66;case '-': _LL66:
# 612
 if(flags & 64){
# 613
flags &=~ 64;
# 614
goto ok;}
# 616
break;case 'x': _LL67:
# 619
 goto _LL68;case 'X': _LL68:
# 620
 if(flags & 256  && (char*)(_dyneither_ptr_plus(p,sizeof(char),- 1)).curr == buf){
# 621
base=16;
# 622
flags &=~ 256;
# 623
goto ok;}
# 625
break;default: _LL69:
# 627
 break;}
# 635
break;
# 636
ok: {
# 640
char _tmp6A;char _tmp69;struct _dyneither_ptr _tmp68;(_tmp68=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp69=*((char*)
_check_dyneither_subscript(_tmp68,sizeof(char),0)),((_tmp6A=(char)c,((
_get_dyneither_size(_tmp68,sizeof(char))== 1  && (_tmp69 == '\000'  && _tmp6A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp68.curr)=_tmp6A)))))));}
# 641
_IO_getc(fp);
# 642
if(_IO_peekc(fp)== - 1){
# 643
++ seen_eof;
# 644
break;}}}
# 653
if(flags & 128){
# 654
if((char*)p.curr > buf){
# 655
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
# 656
_IO_ungetc((int)*((char*)_check_dyneither_subscript(p,sizeof(char),0)),fp);}
# 658
goto match_failure;}
# 660
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),- 1));
# 661
if(c == 'x'  || c == 'X'){
# 662
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
# 663
_IO_ungetc(c,fp);}
# 665
if((flags & 8)== 0){
# 666
unsigned long res;
# 668
{char _tmp6D;char _tmp6C;struct _dyneither_ptr _tmp6B;(_tmp6B=p,((_tmp6C=*((char*)
_check_dyneither_subscript(_tmp6B,sizeof(char),0)),((_tmp6D='\000',((
_get_dyneither_size(_tmp6B,sizeof(char))== 1  && (_tmp6C == '\000'  && _tmp6D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp6B.curr)=_tmp6D)))))));}
# 669
if(use_strtoul)
# 670
res=strtoul((char*)buf,0,base);else{
# 672
res=(unsigned long)strtol((char*)buf,0,base);}
# 673
if(flags & 16)
# 674
*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
int)res;else{
# 675
if(flags & 4)
# 676
*Cyc_va_arg_short_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
short)res;else{
# 677
if(flags & 1)
# 678
*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
int)res;else{
# 680
*Cyc_va_arg_int_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
int)res;}}}
# 681
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
# 682
++ nassigned;}
# 684
{char*_tmp6E;nread +=(p.curr - ((_tmp6E=buf,_tag_dyneither(_tmp6E,sizeof(char),
_get_zero_arr_size_char((void*)_tmp6E,351)))).curr)/ sizeof(char);}
# 685
break;case 4: _LL4E:
# 689
 if(width == 0  || width > sizeof(buf)- 1)
# 690
width=(long long)(sizeof(buf)- 1);
# 691
flags |=((64 | 128)| 256)| 512;
# 692
{char*_tmp6F;for(p=((_tmp6F=buf,_tag_dyneither(_tmp6F,sizeof(char),
_get_zero_arr_size_char((void*)_tmp6F,351))));width != 0;-- width){
# 693
c=_IO_peekc(fp);
# 698
switch(c){case '0': _LL6C:
# 700
 goto _LL6D;case '1': _LL6D: goto _LL6E;case '2': _LL6E: goto _LL6F;case '3': _LL6F: goto
_LL70;case '4': _LL70:
# 701
 goto _LL71;case '5': _LL71: goto _LL72;case '6': _LL72: goto _LL73;case '7': _LL73: goto
_LL74;case '8': _LL74:
# 702
 goto _LL75;case '9': _LL75:
# 703
 flags &=~(64 | 128);
# 704
goto fok;case '+': _LL76:
# 706
 goto _LL77;case '-': _LL77:
# 707
 if(flags & 64){
# 708
flags &=~ 64;
# 709
goto fok;}
# 711
break;case '.': _LL78:
# 713
 if(flags & 256){
# 714
flags &=~(64 | 256);
# 715
goto fok;}
# 717
break;case 'e': _LL79:
# 718
 goto _LL7A;case 'E': _LL7A:
# 720
 if((flags & (128 | 512))== 512){
# 721
flags=(flags & ~(512 | 256)| 64)| 128;
# 724
goto fok;}
# 726
break;default: _LL7B:
# 728
 break;}
# 730
break;
# 731
fok: {
# 732
char _tmp72;char _tmp71;struct _dyneither_ptr _tmp70;(_tmp70=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp71=*((char*)
_check_dyneither_subscript(_tmp70,sizeof(char),0)),((_tmp72=(char)c,((
_get_dyneither_size(_tmp70,sizeof(char))== 1  && (_tmp71 == '\000'  && _tmp72 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp70.curr)=_tmp72)))))));}
# 733
_IO_getc(fp);
# 734
if(_IO_peekc(fp)== - 1){
# 735
++ seen_eof;
# 736
break;}}}
# 744
if(flags & 128){
# 745
if(flags & 512){
# 747
while((char*)p.curr > buf){
# 748
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
# 749
_IO_ungetc((int)*((char*)_check_dyneither_subscript(p,sizeof(char),0)),fp);}
# 751
goto match_failure;}
# 754
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
# 755
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),0));
# 756
if(c != 'e'  && c != 'E'){
# 757
_IO_ungetc(c,fp);
# 758
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);
# 759
c=(int)*((char*)_check_dyneither_subscript(p,sizeof(char),0));}
# 761
_IO_ungetc(c,fp);}
# 763
if((flags & 8)== 0){
# 764
double res;
# 765
{char _tmp75;char _tmp74;struct _dyneither_ptr _tmp73;(_tmp73=p,((_tmp74=*((char*)
_check_dyneither_subscript(_tmp73,sizeof(char),0)),((_tmp75='\000',((
_get_dyneither_size(_tmp73,sizeof(char))== 1  && (_tmp74 == '\000'  && _tmp75 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp73.curr)=_tmp75)))))));}
# 766
res=atof((const char*)buf);
# 767
if(flags & 1)
# 768
*Cyc_va_arg_double_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=
res;else{
# 770
*Cyc_va_arg_float_ptr(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))=(
float)res;}
# 771
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
# 772
++ nassigned;}
# 774
{char*_tmp76;nread +=(p.curr - ((_tmp76=buf,_tag_dyneither(_tmp76,sizeof(char),
_get_zero_arr_size_char((void*)_tmp76,351)))).curr)/ sizeof(char);}
# 775
break;default: _LL6B: {
# 777
struct Cyc_Core_Impossible_exn_struct _tmp7C;const char*_tmp7B;struct Cyc_Core_Impossible_exn_struct*
_tmp7A;(int)_throw((void*)((_tmp7A=_cycalloc(sizeof(*_tmp7A)),((_tmp7A[0]=((
_tmp7C.tag=Cyc_Core_Impossible,((_tmp7C.f1=((_tmp7B="scanf3",_tag_dyneither(
_tmp7B,sizeof(char),7))),_tmp7C)))),_tmp7A)))));}}}
# 781
eof_failure:
# 782
 ++ seen_eof;
# 783
input_failure:
# 784
 if(nassigned == 0)
# 785
nassigned=- 1;
# 786
control_failure:
# 787
 match_failure:
# 788
 if((unsigned int)errp)
# 789
*errp |=2;
# 790
done:
# 791
 if((unsigned int)errp  && seen_eof)
# 792
*errp |=1;
# 793
return nassigned;};}
# 802
static struct _dyneither_ptr Cyc___sccl(char*tab,struct _dyneither_ptr fmt){
# 807
int c;int n;int v;
# 810
c=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));
# 811
if(c == '^'){
# 812
v=1;
# 813
c=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}else{
# 815
v=0;}
# 817
for(n=0;n < 256;++ n){
# 818
tab[_check_known_subscript_notnull(256,n)]=(char)v;}
# 819
if(c == 0)
# 820
return _dyneither_ptr_plus(fmt,sizeof(char),- 1);
# 829 "/home/jgm/cyclone/lib/scanf.cyc"
v=1 - v;
# 830
for(0;1;0){
# 831
tab[_check_known_subscript_notnull(256,c)]=(char)v;
# 832
doswitch:
# 833
 n=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));
# 834
switch(n){case 0: _LL7E:
# 837
 return _dyneither_ptr_plus(fmt,sizeof(char),- 1);case '-': _LL7F:
# 858 "/home/jgm/cyclone/lib/scanf.cyc"
 n=(int)*((const char*)_check_dyneither_subscript(fmt,sizeof(char),0));
# 859
if(n == ']'  || n < c){
# 860
c=(int)'-';
# 861
break;}
# 863
_dyneither_ptr_inplace_plus(& fmt,sizeof(char),1);
# 864
do{
# 865
tab[_check_known_subscript_notnull(256,++ c)]=(char)v;}while(c < n);
# 873
goto doswitch;
# 881
break;case ']': _LL80:
# 884
 return fmt;default: _LL81:
# 887
 c=n;
# 888
break;}}}
# 896
static int Cyc_string_getc(struct _dyneither_ptr*sptr){
# 897
char c;
# 898
struct _dyneither_ptr s=*sptr;
# 899
if(((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(
s,sizeof(char))== 0) || (c=*((const char*)_check_dyneither_subscript(s,sizeof(
char),0)))== '\000')return - 1;
# 900
*sptr=_dyneither_ptr_plus(s,sizeof(char),1);
# 901
return(int)c;}
# 904
static int Cyc_string_ungetc(int ignore,struct _dyneither_ptr*sptr){
# 905
*sptr=_dyneither_ptr_plus(*sptr,sizeof(char),- 1);
# 907
return 0;}
# 910
static int Cyc_string_peekc(struct _dyneither_ptr*sptr){
# 911
char c;
# 912
struct _dyneither_ptr s=*sptr;
# 913
if(((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(
s,sizeof(char))== 0) || (c=*((const char*)_check_dyneither_subscript(s,sizeof(
char),0)))== '\000')return - 1;
# 914
return(int)c;}
# 917
int Cyc_vsscanf(struct _dyneither_ptr src1,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){
# 920
struct _dyneither_ptr src=(struct _dyneither_ptr)src1;
# 921
int err=0;
# 922
return((int(*)(int(*_IO_getc)(struct _dyneither_ptr*),int(*_IO_ungetc)(int,
struct _dyneither_ptr*),int(*_IO_peekc)(struct _dyneither_ptr*),struct
_dyneither_ptr*fp,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp))Cyc__IO_vfscanf)(
Cyc_string_getc,Cyc_string_ungetc,Cyc_string_peekc,& src,fmt,ap,(int*)& err);}
# 926
int Cyc_sscanf(struct _dyneither_ptr src,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){
# 929
return Cyc_vsscanf(src,fmt,ap);}
# 933
int Cyc_peekc(struct Cyc___cycFILE*stream){
# 934
int c=Cyc_fgetc(stream);
# 935
Cyc_ungetc(c,stream);
# 936
return c;}
# 939
int Cyc_vfscanf(struct Cyc___cycFILE*stream,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){
# 942
int err=0;
# 943
return((int(*)(int(*_IO_getc)(struct Cyc___cycFILE*),int(*_IO_ungetc)(int,struct
Cyc___cycFILE*),int(*_IO_peekc)(struct Cyc___cycFILE*),struct Cyc___cycFILE*fp,
struct _dyneither_ptr fmt0,struct _dyneither_ptr ap,int*errp))Cyc__IO_vfscanf)(Cyc_getc,
Cyc_ungetc,Cyc_peekc,stream,fmt,ap,(int*)& err);}
# 946
int Cyc_fscanf(struct Cyc___cycFILE*stream,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){
# 949
return Cyc_vfscanf(stream,fmt,ap);}
# 952
int Cyc_scanf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 955
return Cyc_vfscanf(Cyc_stdin,fmt,ap);}
