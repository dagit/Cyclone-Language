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

# 26 "/home/jgm/cyclone/include/cycboot.h"
 typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};
struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};
struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;
double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "/home/jgm/cyclone/include/cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{
char*tag;struct _dyneither_ptr f1;};
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
# 160 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "/home/jgm/cyclone/include/list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "/home/jgm/cyclone/include/list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{
char*tag;};
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "/home/jgm/cyclone/include/lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "/home/jgm/cyclone/include/position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{
int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct
_dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{
char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);
# 80 "/home/jgm/cyclone/src/absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int
tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
# 105
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
# 106
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
# 107
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
# 108
typedef void*Cyc_Absyn_kindbound_t;
# 109
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
# 110
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
# 111
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
# 112
typedef void*Cyc_Absyn_bounds_t;
# 113
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
# 114
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
# 115
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
# 116
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
# 117
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
# 118
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
# 119
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
# 120
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
# 121
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
# 122
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
# 123
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
# 124
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
# 125
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
# 126
typedef void*Cyc_Absyn_raw_exp_t;
# 127
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
# 128
typedef void*Cyc_Absyn_raw_stmt_t;
# 129
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
# 130
typedef void*Cyc_Absyn_raw_pat_t;
# 131
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
# 132
typedef void*Cyc_Absyn_binding_t;
# 133
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
# 134
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
# 135
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
# 136
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
# 137
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
# 138
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
# 139
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
# 140
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
# 141
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
# 142
typedef void*Cyc_Absyn_raw_decl_t;
# 143
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
# 144
typedef void*Cyc_Absyn_designator_t;
# 145
typedef void*Cyc_Absyn_absyn_annot_t;
# 146
typedef void*Cyc_Absyn_attribute_t;
# 147
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
# 148
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
# 149
typedef void*Cyc_Absyn_offsetof_field_t;
# 150
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
# 151
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
# 152
typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;
# 155
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public
 = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
unsigned int loc;};
# 176
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz
 = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 181
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top
 = 2};
# 188
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct
Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 208
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None
 = 2};
# 210
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 219
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{
int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{
int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{
int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{
int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
# 281
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{
struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct
Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;
unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;
void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int
tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};
struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;
void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct
Cyc_List_List*f5;};
# 444 "/home/jgm/cyclone/src/absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct
Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type
f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;
int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{
int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int
tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 507
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};
struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;
struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};
struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;
struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct
_union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;
long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct
_tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct
_tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};
# 533
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 
2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};
# 540
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec
 = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct
Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{
int tag;unsigned int f1;};
# 558
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 
1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst
f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;
};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;
void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*
f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct
_dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*
annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;
struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;
int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{
int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;
};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};
# 747 "/home/jgm/cyclone/src/absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;
struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;
struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*
f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};
# 904
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 933
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1151
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[
13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "/home/jgm/cyclone/src/tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{enum Cyc_Absyn_Scope f1;
int f2;};
# 61 "/home/jgm/cyclone/src/tcdecl.h"
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope
s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct
_dyneither_ptr*msg,int allow_externC_extern_merge);
# 66
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,
struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,
struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 73
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,
struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct
_dyneither_ptr*msg);
# 82
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct
Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*
res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};
# 29 "/home/jgm/cyclone/include/iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 30
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "/home/jgm/cyclone/include/dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{
char*tag;};
# 50 "/home/jgm/cyclone/include/string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct Cyc_Set_Set;
# 40 "/home/jgm/cyclone/include/set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{
char*tag;};struct Cyc_RgnOrder_RgnPO;
# 33 "/home/jgm/cyclone/src/rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*
eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{
void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "/home/jgm/cyclone/src/tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 51
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};
# 85
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 96
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 135 "/home/jgm/cyclone/src/tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 153
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 230 "/home/jgm/cyclone/src/tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 302
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_PP_Ppstate;
# 41 "/home/jgm/cyclone/include/pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int
expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};
# 64 "/home/jgm/cyclone/src/absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 74
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[
13]="Incompatible";
# 43 "/home/jgm/cyclone/src/tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);static void _tmp1A2(struct _dyneither_ptr**
msg1,struct _dyneither_ptr*ap,unsigned int*_tmp1A1,unsigned int*_tmp1A0,void***
_tmp19E){for(*_tmp1A1=0;*_tmp1A1 < *_tmp1A0;(*_tmp1A1)++){struct Cyc_String_pa_PrintArg_struct
_tmp19C;struct Cyc_String_pa_PrintArg_struct*_tmp19B;(*_tmp19E)[*_tmp1A1]=
# 53
*_tmp1A1 == 0?(void*)((void*)((_tmp19B=_cycalloc(sizeof(*_tmp19B)),((_tmp19B[0]=((
_tmp19C.tag=0,((_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*msg1)),
_tmp19C)))),_tmp19B))))):*((void**)_check_dyneither_subscript(*ap,sizeof(void*),(
int)(*_tmp1A1 - 1)));}}
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){
# 49
if(msg1 == 0)
# 50
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
# 51
const char*_tmp199;struct _dyneither_ptr fmt2=(struct _dyneither_ptr)Cyc_strconcat(((
_tmp199="%s ",_tag_dyneither(_tmp199,sizeof(char),4))),(struct _dyneither_ptr)fmt);
# 52
unsigned int _tmp1A1;unsigned int _tmp1A0;struct _dyneither_ptr _tmp19F;void**
_tmp19E;unsigned int _tmp19D;struct _dyneither_ptr ap2=(_tmp19D=_get_dyneither_size(
ap,sizeof(void*))+ 1,((_tmp19E=(void**)_cycalloc(_check_times(sizeof(void*),
_tmp19D)),((_tmp19F=_tag_dyneither(_tmp19E,sizeof(void*),_tmp19D),((((_tmp1A0=
_tmp19D,_tmp1A2(& msg1,& ap,& _tmp1A1,& _tmp1A0,& _tmp19E))),_tmp19F)))))));
# 54
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)
# 55
Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));};}
# 60
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope
s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct
_dyneither_ptr*msg){
# 62
const char*_tmp1A3;struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?(_tmp1A3="public",
_tag_dyneither(_tmp1A3,sizeof(char),7)): Cyc_Absynpp_scope2string(s0);
# 63
const char*_tmp1A4;struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?(_tmp1A4="public",
_tag_dyneither(_tmp1A4,sizeof(char),7)): Cyc_Absynpp_scope2string(s1);
# 64
const char*_tmp1AB;void*_tmp1AA[4];struct Cyc_String_pa_PrintArg_struct _tmp1A9;
struct Cyc_String_pa_PrintArg_struct _tmp1A8;struct Cyc_String_pa_PrintArg_struct
_tmp1A7;struct Cyc_String_pa_PrintArg_struct _tmp1A6;(_tmp1A6.tag=0,((_tmp1A6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)
# 65
Cyc_Absynpp_scope2string(s0)),((_tmp1A7.tag=0,((_tmp1A7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(s1)),((_tmp1A8.tag=
0,((_tmp1A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1A9.tag=0,((
_tmp1A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1AA[0]=& _tmp1A9,((
_tmp1AA[1]=& _tmp1A8,((_tmp1AA[2]=& _tmp1A7,((_tmp1AA[3]=& _tmp1A6,Cyc_Tcdecl_merr(
loc,msg,((_tmp1AB="%s %s is %s whereas expected scope is %s",_tag_dyneither(
_tmp1AB,sizeof(char),41))),_tag_dyneither(_tmp1AA,sizeof(void*),4)))))))))))))))))))))))));}
struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 68
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope
s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct
_dyneither_ptr*msg,int externCmerge){
# 70
{struct _tuple11 _tmp1AC;struct _tuple11 _tmp11=(_tmp1AC.f1=s0,((_tmp1AC.f2=s1,
_tmp1AC)));enum Cyc_Absyn_Scope _tmp12;enum Cyc_Absyn_Scope _tmp13;enum Cyc_Absyn_Scope
_tmp14;enum Cyc_Absyn_Scope _tmp15;enum Cyc_Absyn_Scope _tmp16;enum Cyc_Absyn_Scope
_tmp17;enum Cyc_Absyn_Scope _tmp18;enum Cyc_Absyn_Scope _tmp19;enum Cyc_Absyn_Scope
_tmp1A;enum Cyc_Absyn_Scope _tmp1B;enum Cyc_Absyn_Scope _tmp1C;enum Cyc_Absyn_Scope
_tmp1D;enum Cyc_Absyn_Scope _tmp1E;enum Cyc_Absyn_Scope _tmp1F;enum Cyc_Absyn_Scope
_tmp20;enum Cyc_Absyn_Scope _tmp21;enum Cyc_Absyn_Scope _tmp22;enum Cyc_Absyn_Scope
_tmp23;enum Cyc_Absyn_Scope _tmp24;enum Cyc_Absyn_Scope _tmp25;enum Cyc_Absyn_Scope
_tmp26;enum Cyc_Absyn_Scope _tmp27;_LL1: _tmp12=_tmp11.f1;if(_tmp12 != Cyc_Absyn_ExternC)
goto _LL3;_tmp13=_tmp11.f2;if(_tmp13 != Cyc_Absyn_ExternC)goto _LL3;_LL2:
# 72
 goto _LL0;_LL3: _tmp14=_tmp11.f1;if(_tmp14 != Cyc_Absyn_ExternC)goto _LL5;_tmp15=
_tmp11.f2;if(_tmp15 != Cyc_Absyn_Public)goto _LL5;_LL4:
# 74
 goto _LL6;_LL5: _tmp16=_tmp11.f1;if(_tmp16 != Cyc_Absyn_ExternC)goto _LL7;_tmp17=
_tmp11.f2;if(_tmp17 != Cyc_Absyn_Extern)goto _LL7;_LL6:
# 75
 goto _LL8;_LL7: _tmp18=_tmp11.f1;if(_tmp18 != Cyc_Absyn_Public)goto _LL9;_tmp19=
_tmp11.f2;if(_tmp19 != Cyc_Absyn_ExternC)goto _LL9;_LL8:
# 76
 goto _LLA;_LL9: _tmp1A=_tmp11.f1;if(_tmp1A != Cyc_Absyn_Extern)goto _LLB;_tmp1B=
_tmp11.f2;if(_tmp1B != Cyc_Absyn_ExternC)goto _LLB;_LLA:
# 77
 if(externCmerge)goto _LL0;
# 78
goto _LLC;_LLB: _tmp1C=_tmp11.f1;if(_tmp1C != Cyc_Absyn_ExternC)goto _LLD;_LLC:
# 79
 goto _LLE;_LLD: _tmp1D=_tmp11.f2;if(_tmp1D != Cyc_Absyn_ExternC)goto _LLF;_LLE:
# 80
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp1AD;return(
_tmp1AD.f1=s1,((_tmp1AD.f2=0,_tmp1AD)));};_LLF: _tmp1E=_tmp11.f2;if(_tmp1E != Cyc_Absyn_Extern)
goto _LL11;_LL10:
# 82
 s1=s0;goto _LL0;_LL11: _tmp1F=_tmp11.f1;if(_tmp1F != Cyc_Absyn_Extern)goto _LL13;
_LL12:
# 83
 goto _LL0;_LL13: _tmp20=_tmp11.f1;if(_tmp20 != Cyc_Absyn_Static)goto _LL15;_tmp21=
_tmp11.f2;if(_tmp21 != Cyc_Absyn_Static)goto _LL15;_LL14:
# 85
 goto _LL16;_LL15: _tmp22=_tmp11.f1;if(_tmp22 != Cyc_Absyn_Public)goto _LL17;_tmp23=
_tmp11.f2;if(_tmp23 != Cyc_Absyn_Public)goto _LL17;_LL16:
# 86
 goto _LL18;_LL17: _tmp24=_tmp11.f1;if(_tmp24 != Cyc_Absyn_Abstract)goto _LL19;
_tmp25=_tmp11.f2;if(_tmp25 != Cyc_Absyn_Abstract)goto _LL19;_LL18:
# 87
 goto _LL0;_LL19: _tmp26=_tmp11.f1;if(_tmp26 != Cyc_Absyn_Register)goto _LL1B;_tmp27=
_tmp11.f2;if(_tmp27 != Cyc_Absyn_Register)goto _LL1B;_LL1A:
# 88
 goto _LL0;_LL1B:;_LL1C:
# 89
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp1AE;return(
_tmp1AE.f1=s1,((_tmp1AE.f2=0,_tmp1AE)));};_LL0:;}{
# 91
struct _tuple10 _tmp1AF;return(_tmp1AF.f1=s1,((_tmp1AF.f2=1,_tmp1AF)));};}
# 94
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 96
return Cyc_Tcutil_unify(t0,t1);}
# 100
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
# 101
void*_tmp2B=e->r;union Cyc_Absyn_Cnst _tmp2D;struct _tuple5 _tmp2E;int _tmp2F;_LL1E: {
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmp2B;if(_tmp2C->tag != 0)goto _LL20;else{_tmp2D=_tmp2C->f1;if((_tmp2D.Int_c).tag
!= 5)goto _LL20;_tmp2E=(struct _tuple5)(_tmp2D.Int_c).val;_tmp2F=_tmp2E.f2;}}_LL1F:
# 102
 return(unsigned int)_tmp2F;_LL20:;_LL21: {
# 103
struct Cyc_Core_Invalid_argument_exn_struct _tmp1B5;const char*_tmp1B4;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp1B3;(int)_throw((void*)((_tmp1B3=_cycalloc(sizeof(*_tmp1B3)),((_tmp1B3[0]=((
_tmp1B5.tag=Cyc_Core_Invalid_argument,((_tmp1B5.f1=((_tmp1B4="Tcdecl::get_uint_const_value",
_tag_dyneither(_tmp1B4,sizeof(char),29))),_tmp1B5)))),_tmp1B3)))));}_LL1D:;}
# 107
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*
tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct
_dyneither_ptr*msg){
# 110
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tvs1)){
# 111
{const char*_tmp1BA;void*_tmp1B9[2];struct Cyc_String_pa_PrintArg_struct _tmp1B8;
struct Cyc_String_pa_PrintArg_struct _tmp1B7;(_tmp1B7.tag=0,((_tmp1B7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1B8.tag=0,((_tmp1B8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1B9[0]=& _tmp1B8,((_tmp1B9[1]=&
_tmp1B7,Cyc_Tcdecl_merr(loc,msg,((_tmp1BA="%s %s has a different number of type parameters",
_tag_dyneither(_tmp1BA,sizeof(char),48))),_tag_dyneither(_tmp1B9,sizeof(void*),2)))))))))))));}
# 112
return 0;}{
# 114
struct Cyc_List_List*_tmp37=tvs0;
# 115
struct Cyc_List_List*_tmp38=tvs1;
# 116
for(0;_tmp37 != 0;(_tmp37=_tmp37->tl,_tmp38=_tmp38->tl)){
# 117
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp37->hd)->kind,((struct
Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp38))->hd)->kind);{
# 118
struct Cyc_Absyn_Kind*_tmp39=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp37->hd)->kind);
# 119
struct Cyc_Absyn_Kind*_tmp3A=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp38->hd)->kind);
# 120
if(_tmp39 == _tmp3A)continue;
# 121
{const char*_tmp1C2;void*_tmp1C1[5];struct Cyc_String_pa_PrintArg_struct _tmp1C0;
struct Cyc_String_pa_PrintArg_struct _tmp1BF;struct Cyc_String_pa_PrintArg_struct
_tmp1BE;struct Cyc_String_pa_PrintArg_struct _tmp1BD;struct Cyc_String_pa_PrintArg_struct
_tmp1BC;(_tmp1BC.tag=0,((_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 122
Cyc_Absynpp_kind2string(_tmp3A)),((_tmp1BD.tag=0,((_tmp1BD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp37->hd)->name),((
_tmp1BE.tag=0,((_tmp1BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp39)),((_tmp1BF.tag=0,((_tmp1BF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)v),((_tmp1C0.tag=0,((_tmp1C0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)t),((_tmp1C1[0]=& _tmp1C0,((_tmp1C1[1]=& _tmp1BF,((_tmp1C1[2]=&
_tmp1BE,((_tmp1C1[3]=& _tmp1BD,((_tmp1C1[4]=& _tmp1BC,Cyc_Tcdecl_merr(loc,msg,((
_tmp1C2="%s %s has a different kind (%s) for type parameter %s (%s)",
_tag_dyneither(_tmp1C2,sizeof(char),59))),_tag_dyneither(_tmp1C1,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 123
return 0;};}
# 125
return 1;};}
# 128
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*
atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct
_dyneither_ptr*msg){
# 130
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
# 131
{const char*_tmp1C7;void*_tmp1C6[2];struct Cyc_String_pa_PrintArg_struct _tmp1C5;
struct Cyc_String_pa_PrintArg_struct _tmp1C4;(_tmp1C4.tag=0,((_tmp1C4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1C5.tag=0,((_tmp1C5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1C6[0]=& _tmp1C5,((_tmp1C6[1]=&
_tmp1C4,Cyc_Tcdecl_merr(loc,msg,((_tmp1C7="%s %s has different attributes",
_tag_dyneither(_tmp1C7,sizeof(char),31))),_tag_dyneither(_tmp1C6,sizeof(void*),2)))))))))))));}
# 132
return 0;}
# 134
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 137
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,
struct Cyc_List_List*tvs1){
# 139
struct Cyc_List_List*inst=0;
# 140
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
# 141
struct _tuple12*_tmp1D1;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1D0;struct
Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1CF;struct Cyc_List_List*_tmp1CE;inst=((
_tmp1CE=_cycalloc(sizeof(*_tmp1CE)),((_tmp1CE->hd=((_tmp1D1=_cycalloc(sizeof(*
_tmp1D1)),((_tmp1D1->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(tvs1))->hd,((_tmp1D1->f2=(void*)((_tmp1CF=_cycalloc(sizeof(*_tmp1CF)),((
_tmp1CF[0]=((_tmp1D0.tag=2,((_tmp1D0.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp1D0)))),
_tmp1CF)))),_tmp1D1)))))),((_tmp1CE->tl=inst,_tmp1CE))))));}
# 142
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*
f2;};struct _tuple14{void*f1;void*f2;};
# 145
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,
struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 147
struct _dyneither_ptr _tmp4A=Cyc_Absynpp_qvar2string(d0->name);
# 148
int _tmp4B=1;
# 153
if(!(d0->kind == d1->kind))return 0;
# 156
{const char*_tmp1D2;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp1D2="type",
_tag_dyneither(_tmp1D2,sizeof(char),5))),_tmp4A,loc,msg))return 0;}{
# 159
enum Cyc_Absyn_Scope _tmp4F;int _tmp50;const char*_tmp1D3;struct _tuple10 _tmp4E=
# 160
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp1D3="type",_tag_dyneither(_tmp1D3,
sizeof(char),5))),_tmp4A,loc,msg,1);_tmp4F=_tmp4E.f1;_tmp50=_tmp4E.f2;
# 161
if(!_tmp50)_tmp4B=0;
# 164
{const char*_tmp1D4;if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,((
_tmp1D4="type",_tag_dyneither(_tmp1D4,sizeof(char),5))),_tmp4A,loc,msg))_tmp4B=0;}{
# 166
struct Cyc_Absyn_Aggrdecl*d2;
# 168
{struct _tuple13 _tmp1D5;struct _tuple13 _tmp53=(_tmp1D5.f1=d0->impl,((_tmp1D5.f2=
d1->impl,_tmp1D5)));struct Cyc_Absyn_AggrdeclImpl*_tmp54;struct Cyc_Absyn_AggrdeclImpl*
_tmp55;struct Cyc_Absyn_AggrdeclImpl*_tmp56;struct Cyc_Absyn_AggrdeclImpl _tmp57;
struct Cyc_List_List*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;
int _tmp5B;struct Cyc_Absyn_AggrdeclImpl*_tmp5C;struct Cyc_Absyn_AggrdeclImpl _tmp5D;
struct Cyc_List_List*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_List_List*_tmp60;
int _tmp61;_LL23: _tmp54=_tmp53.f2;if(_tmp54 != 0)goto _LL25;_LL24:
# 169
 d2=d0;goto _LL22;_LL25: _tmp55=_tmp53.f1;if(_tmp55 != 0)goto _LL27;_LL26:
# 170
 d2=d1;goto _LL22;_LL27: _tmp56=_tmp53.f1;if(_tmp56 == 0)goto _LL22;_tmp57=*_tmp56;
_tmp58=_tmp57.exist_vars;_tmp59=_tmp57.rgn_po;_tmp5A=_tmp57.fields;_tmp5B=_tmp57.tagged;
_tmp5C=_tmp53.f2;if(_tmp5C == 0)goto _LL22;_tmp5D=*_tmp5C;_tmp5E=_tmp5D.exist_vars;
_tmp5F=_tmp5D.rgn_po;_tmp60=_tmp5D.fields;_tmp61=_tmp5D.tagged;_LL28:
# 175
{const char*_tmp1D6;if(!Cyc_Tcdecl_check_tvs(_tmp58,_tmp5E,((_tmp1D6="type",
_tag_dyneither(_tmp1D6,sizeof(char),5))),_tmp4A,loc,msg))
# 176
return 0;}{
# 179
struct Cyc_List_List*_tmp63=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp58),
# 180
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
d1->tvs,_tmp5E));
# 184
for(0;_tmp59 != 0  && _tmp5F != 0;
# 185
(_tmp59=_tmp59->tl,_tmp5F=_tmp5F->tl)){
# 186
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp59->hd)).f1,(*((struct _tuple14*)
_tmp5F->hd)).f1);
# 187
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp59->hd)).f2,(*((struct _tuple14*)
_tmp5F->hd)).f2);}
# 191
for(0;_tmp5A != 0  && _tmp60 != 0;(_tmp5A=_tmp5A->tl,_tmp60=_tmp60->tl)){
# 192
struct Cyc_Absyn_Aggrfield _tmp65;struct _dyneither_ptr*_tmp66;struct Cyc_Absyn_Tqual
_tmp67;void*_tmp68;struct Cyc_Absyn_Exp*_tmp69;struct Cyc_List_List*_tmp6A;struct
Cyc_Absyn_Aggrfield*_tmp64=(struct Cyc_Absyn_Aggrfield*)_tmp5A->hd;_tmp65=*_tmp64;
_tmp66=_tmp65.name;_tmp67=_tmp65.tq;_tmp68=_tmp65.type;_tmp69=_tmp65.width;
_tmp6A=_tmp65.attributes;{
# 193
struct Cyc_Absyn_Aggrfield _tmp6C;struct _dyneither_ptr*_tmp6D;struct Cyc_Absyn_Tqual
_tmp6E;void*_tmp6F;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_List_List*_tmp71;struct
Cyc_Absyn_Aggrfield*_tmp6B=(struct Cyc_Absyn_Aggrfield*)_tmp60->hd;_tmp6C=*_tmp6B;
_tmp6D=_tmp6C.name;_tmp6E=_tmp6C.tq;_tmp6F=_tmp6C.type;_tmp70=_tmp6C.width;
_tmp71=_tmp6C.attributes;
# 195
if(Cyc_strptrcmp(_tmp66,_tmp6D)!= 0){
# 196
{const char*_tmp1DF;void*_tmp1DE[4];const char*_tmp1DD;struct Cyc_String_pa_PrintArg_struct
_tmp1DC;struct Cyc_String_pa_PrintArg_struct _tmp1DB;struct Cyc_String_pa_PrintArg_struct
_tmp1DA;struct Cyc_String_pa_PrintArg_struct _tmp1D9;(_tmp1D9.tag=0,((_tmp1D9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6D),((_tmp1DA.tag=0,((_tmp1DA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp66),((_tmp1DB.tag=0,((_tmp1DB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A),((_tmp1DC.tag=0,((_tmp1DC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1DD="type",_tag_dyneither(
_tmp1DD,sizeof(char),5)))),((_tmp1DE[0]=& _tmp1DC,((_tmp1DE[1]=& _tmp1DB,((_tmp1DE[
2]=& _tmp1DA,((_tmp1DE[3]=& _tmp1D9,Cyc_Tcdecl_merr(loc,msg,((_tmp1DF="%s %s : field name mismatch %s != %s",
_tag_dyneither(_tmp1DF,sizeof(char),37))),_tag_dyneither(_tmp1DE,sizeof(void*),4)))))))))))))))))))))))));}
# 198
return 0;}
# 200
if(!Cyc_Tcutil_same_atts(_tmp6A,_tmp71)){
# 201
{const char*_tmp1E7;void*_tmp1E6[3];const char*_tmp1E5;struct Cyc_String_pa_PrintArg_struct
_tmp1E4;struct Cyc_String_pa_PrintArg_struct _tmp1E3;struct Cyc_String_pa_PrintArg_struct
_tmp1E2;(_tmp1E2.tag=0,((_tmp1E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp66),((_tmp1E3.tag=0,((_tmp1E3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp4A),((_tmp1E4.tag=0,((_tmp1E4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)((_tmp1E5="type",_tag_dyneither(_tmp1E5,sizeof(char),5)))),((
_tmp1E6[0]=& _tmp1E4,((_tmp1E6[1]=& _tmp1E3,((_tmp1E6[2]=& _tmp1E2,Cyc_Tcdecl_merr(
loc,msg,((_tmp1E7="%s %s : attribute mismatch on field %s",_tag_dyneither(
_tmp1E7,sizeof(char),39))),_tag_dyneither(_tmp1E6,sizeof(void*),3)))))))))))))))))));}
# 202
_tmp4B=0;}
# 204
if(!Cyc_Tcutil_equal_tqual(_tmp67,_tmp6E)){
# 205
{const char*_tmp1EF;void*_tmp1EE[3];const char*_tmp1ED;struct Cyc_String_pa_PrintArg_struct
_tmp1EC;struct Cyc_String_pa_PrintArg_struct _tmp1EB;struct Cyc_String_pa_PrintArg_struct
_tmp1EA;(_tmp1EA.tag=0,((_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp66),((_tmp1EB.tag=0,((_tmp1EB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp4A),((_tmp1EC.tag=0,((_tmp1EC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)((_tmp1ED="type",_tag_dyneither(_tmp1ED,sizeof(char),5)))),((
_tmp1EE[0]=& _tmp1EC,((_tmp1EE[1]=& _tmp1EB,((_tmp1EE[2]=& _tmp1EA,Cyc_Tcdecl_merr(
loc,msg,((_tmp1EF="%s %s : qualifier mismatch on field %s",_tag_dyneither(
_tmp1EF,sizeof(char),39))),_tag_dyneither(_tmp1EE,sizeof(void*),3)))))))))))))))))));}
# 206
_tmp4B=0;}
# 208
if(((_tmp69 != 0  && _tmp70 != 0) && 
# 209
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_tmp69)!= 
# 210
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_tmp70) || 
# 211
_tmp69 == 0  && _tmp70 != 0) || 
# 212
_tmp69 != 0  && _tmp70 == 0){
# 213
{const char*_tmp1F7;void*_tmp1F6[3];const char*_tmp1F5;struct Cyc_String_pa_PrintArg_struct
_tmp1F4;struct Cyc_String_pa_PrintArg_struct _tmp1F3;struct Cyc_String_pa_PrintArg_struct
_tmp1F2;(_tmp1F2.tag=0,((_tmp1F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp66),((_tmp1F3.tag=0,((_tmp1F3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp4A),((_tmp1F4.tag=0,((_tmp1F4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)((_tmp1F5="type",_tag_dyneither(_tmp1F5,sizeof(char),5)))),((
_tmp1F6[0]=& _tmp1F4,((_tmp1F6[1]=& _tmp1F3,((_tmp1F6[2]=& _tmp1F2,Cyc_Tcdecl_merr(
loc,msg,((_tmp1F7="%s %s : bitfield mismatch on field %s",_tag_dyneither(_tmp1F7,
sizeof(char),38))),_tag_dyneither(_tmp1F6,sizeof(void*),3)))))))))))))))))));}
# 214
_tmp4B=0;}{
# 217
void*subst_t1=Cyc_Tcutil_substitute(_tmp63,_tmp6F);
# 218
if(!Cyc_Tcdecl_check_type(_tmp68,subst_t1)){
# 219
{const char*_tmp1FE;void*_tmp1FD[4];struct Cyc_String_pa_PrintArg_struct _tmp1FC;
struct Cyc_String_pa_PrintArg_struct _tmp1FB;struct Cyc_String_pa_PrintArg_struct
_tmp1FA;struct Cyc_String_pa_PrintArg_struct _tmp1F9;(_tmp1F9.tag=0,((_tmp1F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)
# 220
Cyc_Absynpp_typ2string(subst_t1)),((_tmp1FA.tag=0,((_tmp1FA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp68)),((_tmp1FB.tag=
0,((_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp66),((_tmp1FC.tag=
0,((_tmp1FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A),((_tmp1FD[0]=&
_tmp1FC,((_tmp1FD[1]=& _tmp1FB,((_tmp1FD[2]=& _tmp1FA,((_tmp1FD[3]=& _tmp1F9,Cyc_Tcdecl_merr(
loc,msg,((_tmp1FE="type %s : type mismatch on field %s: %s != %s",_tag_dyneither(
_tmp1FE,sizeof(char),46))),_tag_dyneither(_tmp1FD,sizeof(void*),4)))))))))))))))))))))))));}
# 221
Cyc_Tcutil_explain_failure();
# 222
_tmp4B=0;}};};}
# 226
if(_tmp5A != 0){
# 227
{const char*_tmp203;void*_tmp202[2];struct Cyc_String_pa_PrintArg_struct _tmp201;
struct Cyc_String_pa_PrintArg_struct _tmp200;(_tmp200.tag=0,((_tmp200.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp5A->hd)->name),((
_tmp201.tag=0,((_tmp201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A),((
_tmp202[0]=& _tmp201,((_tmp202[1]=& _tmp200,Cyc_Tcdecl_merr(loc,msg,((_tmp203="type %s is missing field %s",
_tag_dyneither(_tmp203,sizeof(char),28))),_tag_dyneither(_tmp202,sizeof(void*),2)))))))))))));}
# 228
_tmp4B=0;}
# 230
if(_tmp60 != 0){
# 231
{const char*_tmp208;void*_tmp207[2];struct Cyc_String_pa_PrintArg_struct _tmp206;
struct Cyc_String_pa_PrintArg_struct _tmp205;(_tmp205.tag=0,((_tmp205.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp60->hd)->name),((
_tmp206.tag=0,((_tmp206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A),((
_tmp207[0]=& _tmp206,((_tmp207[1]=& _tmp205,Cyc_Tcdecl_merr(loc,msg,((_tmp208="type %s has extra field %s",
_tag_dyneither(_tmp208,sizeof(char),27))),_tag_dyneither(_tmp207,sizeof(void*),2)))))))))))));}
# 232
_tmp4B=0;}
# 235
if(_tmp5B != _tmp61){
# 236
{const char*_tmp20C;void*_tmp20B[1];struct Cyc_String_pa_PrintArg_struct _tmp20A;(
_tmp20A.tag=0,((_tmp20A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A),((
_tmp20B[0]=& _tmp20A,Cyc_Tcdecl_merr(loc,msg,((_tmp20C="%s : previous declaration disagrees with @tagged qualifier",
_tag_dyneither(_tmp20C,sizeof(char),59))),_tag_dyneither(_tmp20B,sizeof(void*),1)))))));}
# 237
_tmp4B=0;}
# 240
d2=d0;
# 241
goto _LL22;};_LL22:;}
# 244
if(!_tmp4B)return 0;
# 246
if(_tmp4F == d2->sc)
# 247
return(struct Cyc_Absyn_Aggrdecl*)d2;else{
# 249
{struct Cyc_Absyn_Aggrdecl*_tmp20D;d2=((_tmp20D=_cycalloc(sizeof(*_tmp20D)),((
_tmp20D[0]=*d2,_tmp20D))));}
# 250
d2->sc=_tmp4F;
# 251
return(struct Cyc_Absyn_Aggrdecl*)d2;}};};}
# 255
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
# 256
const char*_tmp20F;const char*_tmp20E;return is_x?(_tmp20F="@extensible datatype",
_tag_dyneither(_tmp20F,sizeof(char),21)):((_tmp20E="datatype",_tag_dyneither(
_tmp20E,sizeof(char),9)));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 261
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*
f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 265
unsigned int loc=f1->loc;
# 268
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
# 269
{const char*_tmp216;void*_tmp215[4];struct Cyc_String_pa_PrintArg_struct _tmp214;
struct Cyc_String_pa_PrintArg_struct _tmp213;struct Cyc_String_pa_PrintArg_struct
_tmp212;struct Cyc_String_pa_PrintArg_struct _tmp211;(_tmp211.tag=0,((_tmp211.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2),((_tmp212.tag=0,((
_tmp212.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2),((
_tmp213.tag=0,((_tmp213.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp214.tag=0,((_tmp214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp215[0]=& _tmp214,((_tmp215[1]=& _tmp213,((_tmp215[2]=& _tmp212,((_tmp215[3]=&
_tmp211,Cyc_Tcdecl_merr(loc,msg,((_tmp216="%s %s: field name mismatch %s != %s",
_tag_dyneither(_tmp216,sizeof(char),36))),_tag_dyneither(_tmp215,sizeof(void*),4)))))))))))))))))))))))));}
# 271
return 0;}{
# 274
struct _dyneither_ptr _tmpA5=*(*f0->name).f2;
# 277
enum Cyc_Absyn_Scope _tmpAB;int _tmpAC;const char*_tmp21B;void*_tmp21A[2];struct
Cyc_String_pa_PrintArg_struct _tmp219;struct Cyc_String_pa_PrintArg_struct _tmp218;
struct _tuple10 _tmpAA=
# 278
Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct _dyneither_ptr)((_tmp218.tag=0,((
_tmp218.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp219.tag=0,((
_tmp219.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp21A[0]=& _tmp219,((
_tmp21A[1]=& _tmp218,Cyc_aprintf(((_tmp21B="in %s %s, field",_tag_dyneither(
_tmp21B,sizeof(char),16))),_tag_dyneither(_tmp21A,sizeof(void*),2)))))))))))))),
_tmpA5,loc,msg,0);_tmpAB=_tmpAA.f1;_tmpAC=_tmpAA.f2;{
# 281
struct Cyc_List_List*_tmpAD=f0->typs;
# 282
struct Cyc_List_List*_tmpAE=f1->typs;
# 283
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAD)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmpAE)){
# 284
{const char*_tmp221;void*_tmp220[3];struct Cyc_String_pa_PrintArg_struct _tmp21F;
struct Cyc_String_pa_PrintArg_struct _tmp21E;struct Cyc_String_pa_PrintArg_struct
_tmp21D;(_tmp21D.tag=0,((_tmp21D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpA5),((_tmp21E.tag=0,((_tmp21E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)v),((_tmp21F.tag=0,((_tmp21F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)t),((_tmp220[0]=& _tmp21F,((_tmp220[1]=& _tmp21E,((_tmp220[2]=&
_tmp21D,Cyc_Tcdecl_merr(loc,msg,((_tmp221="%s %s, field %s: parameter number mismatch",
_tag_dyneither(_tmp221,sizeof(char),43))),_tag_dyneither(_tmp220,sizeof(void*),3)))))))))))))))))));}
# 285
_tmpAC=0;}
# 288
for(0;_tmpAD != 0;(_tmpAD=_tmpAD->tl,_tmpAE=_tmpAE->tl)){
# 290
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmpAD->hd)).f1,(*((struct
_tuple15*)((struct Cyc_List_List*)_check_null(_tmpAE))->hd)).f1)){
# 291
{const char*_tmp227;void*_tmp226[3];struct Cyc_String_pa_PrintArg_struct _tmp225;
struct Cyc_String_pa_PrintArg_struct _tmp224;struct Cyc_String_pa_PrintArg_struct
_tmp223;(_tmp223.tag=0,((_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpA5),((_tmp224.tag=0,((_tmp224.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)v),((_tmp225.tag=0,((_tmp225.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)t),((_tmp226[0]=& _tmp225,((_tmp226[1]=& _tmp224,((_tmp226[2]=&
_tmp223,Cyc_Tcdecl_merr(loc,msg,((_tmp227="%s %s, field %s: parameter qualifier",
_tag_dyneither(_tmp227,sizeof(char),37))),_tag_dyneither(_tmp226,sizeof(void*),3)))))))))))))))))));}
# 292
_tmpAC=0;}{
# 294
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmpAE->hd)).f2);
# 296
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmpAD->hd)).f2,subst_t1)){
# 297
{const char*_tmp22F;void*_tmp22E[5];struct Cyc_String_pa_PrintArg_struct _tmp22D;
struct Cyc_String_pa_PrintArg_struct _tmp22C;struct Cyc_String_pa_PrintArg_struct
_tmp22B;struct Cyc_String_pa_PrintArg_struct _tmp22A;struct Cyc_String_pa_PrintArg_struct
_tmp229;(_tmp229.tag=0,((_tmp229.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 298
Cyc_Absynpp_typ2string(subst_t1)),((_tmp22A.tag=0,((_tmp22A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)
_tmpAD->hd)).f2)),((_tmp22B.tag=0,((_tmp22B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmpA5),((_tmp22C.tag=0,((_tmp22C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)v),((_tmp22D.tag=0,((_tmp22D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t),((_tmp22E[0]=& _tmp22D,((_tmp22E[1]=& _tmp22C,((_tmp22E[2]=&
_tmp22B,((_tmp22E[3]=& _tmp22A,((_tmp22E[4]=& _tmp229,Cyc_Tcdecl_merr(loc,msg,((
_tmp22F="%s %s, field %s: parameter type mismatch %s != %s",_tag_dyneither(
_tmp22F,sizeof(char),50))),_tag_dyneither(_tmp22E,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 299
Cyc_Tcutil_explain_failure();
# 300
_tmpAC=0;}};}
# 304
if(!_tmpAC)return 0;
# 305
if(f0->sc != _tmpAB){
# 306
struct Cyc_Absyn_Datatypefield*_tmp230;struct Cyc_Absyn_Datatypefield*_tmpC0=(
_tmp230=_cycalloc(sizeof(*_tmp230)),((_tmp230[0]=*f0,_tmp230)));
# 307
((struct Cyc_Absyn_Datatypefield*)_check_null(_tmpC0))->sc=_tmpAB;
# 308
return _tmpC0;}else{
# 309
return(struct Cyc_Absyn_Datatypefield*)f0;}};};}
# 312
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*
inst,struct _tuple15*x){
# 315
struct _tuple15 _tmpC3;struct Cyc_Absyn_Tqual _tmpC4;void*_tmpC5;struct _tuple15*
_tmpC2=x;_tmpC3=*_tmpC2;_tmpC4=_tmpC3.f1;_tmpC5=_tmpC3.f2;{
# 316
struct _tuple15*_tmp231;return(_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231->f1=
_tmpC4,((_tmp231->f2=Cyc_Tcutil_substitute(inst,_tmpC5),_tmp231)))));};}
# 321
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct
Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 323
struct Cyc_Absyn_Datatypefield*_tmp232;struct Cyc_Absyn_Datatypefield*_tmpC7=(
_tmp232=_cycalloc(sizeof(*_tmp232)),((_tmp232[0]=*f1,_tmp232)));
# 324
_tmpC7->typs=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,
struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);
# 326
return _tmpC7;}
# 333
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*
f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,
struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct
_dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 342
struct Cyc_List_List**_tmp233;struct Cyc_List_List**f2sp=(_tmp233=_cycalloc(
sizeof(*_tmp233)),((_tmp233[0]=0,_tmp233)));
# 343
struct Cyc_List_List**_tmpC9=f2sp;
# 344
int cmp=- 1;
# 346
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
# 347
while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((
struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
# 348
struct Cyc_List_List*_tmp234;struct Cyc_List_List*_tmpCA=(_tmp234=_cycalloc(
sizeof(*_tmp234)),((_tmp234->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,((
_tmp234->tl=0,_tmp234)))));
# 349
*_tmpC9=_tmpCA;
# 350
_tmpC9=&((struct Cyc_List_List*)_check_null(_tmpCA))->tl;
# 351
f0s=f0s->tl;}
# 353
if(f0s == 0  || cmp > 0){
# 355
*incl=0;{
# 356
struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmpCC=(_tmp235=_cycalloc(
sizeof(*_tmp235)),((_tmp235->hd=Cyc_Tcdecl_substitute_datatypefield(inst,(struct
Cyc_Absyn_Datatypefield*)f1s->hd),((_tmp235->tl=0,_tmp235)))));
# 357
*_tmpC9=_tmpCC;
# 358
_tmpC9=&((struct Cyc_List_List*)_check_null(_tmpCC))->tl;};}else{
# 361
struct Cyc_Absyn_Datatypefield*_tmpCE=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)
f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
# 362
if(_tmpCE != 0){
# 363
if(_tmpCE != (struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)
f0s->hd))*incl=0;{
# 364
struct Cyc_List_List*_tmp236;struct Cyc_List_List*_tmpCF=(_tmp236=_cycalloc(
sizeof(*_tmp236)),((_tmp236->hd=(struct Cyc_Absyn_Datatypefield*)_tmpCE,((_tmp236->tl=
0,_tmp236)))));
# 365
*_tmpC9=_tmpCF;
# 366
_tmpC9=&((struct Cyc_List_List*)_check_null(_tmpCF))->tl;};}else{
# 368
*res=0;}
# 370
f0s=f0s->tl;}}
# 375
if(f1s != 0){
# 376
*incl=0;
# 377
*_tmpC9=f1s;}else{
# 379
*_tmpC9=f0s;}
# 382
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 386
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
# 387
if(f == 0){struct _tuple16 _tmp237;return(_tmp237.f1=0,((_tmp237.f2=0,_tmp237)));}
# 388
if(f->tl == 0){struct _tuple16 _tmp238;return(_tmp238.f1=f,((_tmp238.f2=0,_tmp238)));}{
# 389
struct Cyc_List_List*_tmpD5;struct Cyc_List_List*_tmpD6;struct _tuple16 _tmpD4=Cyc_Tcdecl_split(((
struct Cyc_List_List*)_check_null(f->tl))->tl);_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;{
# 390
struct Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23C;struct _tuple16 _tmp23B;
return(_tmp23B.f1=((_tmp23C=_cycalloc(sizeof(*_tmp23C)),((_tmp23C->hd=(void*)((
void*)f->hd),((_tmp23C->tl=_tmpD5,_tmp23C)))))),((_tmp23B.f2=((_tmp23D=_cycalloc(
sizeof(*_tmp23D)),((_tmp23D->hd=(void*)((void*)((struct Cyc_List_List*)
_check_null(f->tl))->hd),((_tmp23D->tl=_tmpD6,_tmp23D)))))),_tmp23B)));};};}
# 395
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*
res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 398
struct Cyc_List_List*_tmpDB;struct Cyc_List_List*_tmpDC;struct _tuple16 _tmpDA=((
struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_tmpDB=_tmpDA.f1;
_tmpDC=_tmpDA.f2;
# 399
if(_tmpDB != 0  && _tmpDB->tl != 0)
# 400
_tmpDB=Cyc_Tcdecl_sort_xdatatype_fields(_tmpDB,res,v,loc,msg);
# 401
if(_tmpDC != 0  && _tmpDC->tl != 0)
# 402
_tmpDC=Cyc_Tcdecl_sort_xdatatype_fields(_tmpDC,res,v,loc,msg);{
# 403
const char*_tmp23F;int*_tmp23E;return Cyc_Tcdecl_merge_xdatatype_fields(_tmpDB,
_tmpDC,0,0,0,res,((_tmp23E=_cycalloc_atomic(sizeof(*_tmp23E)),((_tmp23E[0]=1,
_tmp23E)))),(
# 404
(_tmp23F="@extensible datatype",_tag_dyneither(_tmp23F,sizeof(char),21))),*v,
loc,msg);};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 407
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 409
struct _dyneither_ptr _tmpDF=Cyc_Absynpp_qvar2string(d0->name);
# 410
const char*_tmp240;struct _dyneither_ptr t=(_tmp240="datatype",_tag_dyneither(
_tmp240,sizeof(char),9));
# 411
int _tmpE0=1;
# 416
if(d0->is_extensible != d1->is_extensible){
# 417
{const char*_tmp246;void*_tmp245[3];struct Cyc_String_pa_PrintArg_struct _tmp244;
struct Cyc_String_pa_PrintArg_struct _tmp243;struct Cyc_String_pa_PrintArg_struct
_tmp242;(_tmp242.tag=0,((_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 418
Cyc_Tcdecl_is_x2string(d1->is_extensible)),((_tmp243.tag=0,((_tmp243.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible)),((
_tmp244.tag=0,((_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDF),((
_tmp245[0]=& _tmp244,((_tmp245[1]=& _tmp243,((_tmp245[2]=& _tmp242,Cyc_Tcdecl_merr(
loc,msg,((_tmp246="expected %s to be a %s instead of a %s",_tag_dyneither(
_tmp246,sizeof(char),39))),_tag_dyneither(_tmp245,sizeof(void*),3)))))))))))))))))));}
# 419
_tmpE0=0;}else{
# 421
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 425
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpDF,loc,msg))return 0;{
# 428
enum Cyc_Absyn_Scope _tmpE7;int _tmpE8;struct _tuple10 _tmpE6=Cyc_Tcdecl_merge_scope(
d0->sc,d1->sc,t,_tmpDF,loc,msg,0);_tmpE7=_tmpE6.f1;_tmpE8=_tmpE6.f2;
# 429
if(!_tmpE8)_tmpE0=0;{
# 431
struct Cyc_Absyn_Datatypedecl*d2;
# 434
{struct _tuple17 _tmp247;struct _tuple17 _tmpEA=(_tmp247.f1=d0->fields,((_tmp247.f2=
d1->fields,_tmp247)));struct Cyc_Core_Opt*_tmpEB;struct Cyc_Core_Opt*_tmpEC;struct
Cyc_Core_Opt*_tmpED;struct Cyc_Core_Opt _tmpEE;struct Cyc_List_List*_tmpEF;struct
Cyc_Core_Opt*_tmpF0;struct Cyc_Core_Opt _tmpF1;struct Cyc_List_List*_tmpF2;_LL2A:
_tmpEB=_tmpEA.f2;if(_tmpEB != 0)goto _LL2C;_LL2B:
# 435
 d2=d0;goto _LL29;_LL2C: _tmpEC=_tmpEA.f1;if(_tmpEC != 0)goto _LL2E;_LL2D:
# 436
 d2=d1;goto _LL29;_LL2E: _tmpED=_tmpEA.f1;if(_tmpED == 0)goto _LL29;_tmpEE=*_tmpED;
_tmpEF=(struct Cyc_List_List*)_tmpEE.v;_tmpF0=_tmpEA.f2;if(_tmpF0 == 0)goto _LL29;
_tmpF1=*_tmpF0;_tmpF2=(struct Cyc_List_List*)_tmpF1.v;_LL2F: {
# 448 "/home/jgm/cyclone/src/tcdecl.cyc"
struct Cyc_List_List*_tmpF3=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 451
if(d0->is_extensible){
# 455
int _tmpF4=1;
# 456
struct Cyc_List_List*_tmpF5=Cyc_Tcdecl_merge_xdatatype_fields(_tmpEF,_tmpF2,
_tmpF3,d0->tvs,d1->tvs,& _tmpE0,& _tmpF4,t,_tmpDF,loc,msg);
# 459
if(_tmpF4)
# 460
d2=d0;else{
# 462
{struct Cyc_Absyn_Datatypedecl*_tmp248;d2=((_tmp248=_cycalloc(sizeof(*_tmp248)),((
_tmp248[0]=*d0,_tmp248))));}
# 463
d2->sc=_tmpE7;{
# 464
struct Cyc_Core_Opt*_tmp249;d2->fields=((_tmp249=_cycalloc(sizeof(*_tmp249)),((
_tmp249->v=_tmpF5,_tmp249))));};}}else{
# 468
for(0;_tmpEF != 0  && _tmpF2 != 0;(_tmpEF=_tmpEF->tl,_tmpF2=_tmpF2->tl)){
# 469
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpEF->hd,(
struct Cyc_Absyn_Datatypefield*)_tmpF2->hd,_tmpF3,t,_tmpDF,msg);}
# 471
if(_tmpEF != 0){
# 472
{const char*_tmp24F;void*_tmp24E[3];struct Cyc_String_pa_PrintArg_struct _tmp24D;
struct Cyc_String_pa_PrintArg_struct _tmp24C;struct Cyc_String_pa_PrintArg_struct
_tmp24B;(_tmp24B.tag=0,((_tmp24B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmpF2))->hd)->name).f2),((
_tmp24C.tag=0,((_tmp24C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDF),((
_tmp24D.tag=0,((_tmp24D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp24E[0]=& _tmp24D,((_tmp24E[1]=& _tmp24C,((_tmp24E[2]=& _tmp24B,Cyc_Tcdecl_merr(
loc,msg,((_tmp24F="%s %s has extra field %s",_tag_dyneither(_tmp24F,sizeof(char),
25))),_tag_dyneither(_tmp24E,sizeof(void*),3)))))))))))))))))));}
# 473
_tmpE0=0;}
# 475
if(_tmpF2 != 0){
# 476
{const char*_tmp255;void*_tmp254[3];struct Cyc_String_pa_PrintArg_struct _tmp253;
struct Cyc_String_pa_PrintArg_struct _tmp252;struct Cyc_String_pa_PrintArg_struct
_tmp251;(_tmp251.tag=0,((_tmp251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((
struct Cyc_Absyn_Datatypefield*)_tmpF2->hd)->name).f2),((_tmp252.tag=0,((_tmp252.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDF),((_tmp253.tag=0,((_tmp253.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp254[0]=& _tmp253,((_tmp254[1]=&
_tmp252,((_tmp254[2]=& _tmp251,Cyc_Tcdecl_merr(loc,msg,((_tmp255="%s %s is missing field %s",
_tag_dyneither(_tmp255,sizeof(char),26))),_tag_dyneither(_tmp254,sizeof(void*),3)))))))))))))))))));}
# 477
_tmpE0=0;}
# 479
d2=d0;}
# 481
goto _LL29;}_LL29:;}
# 484
if(!_tmpE0)return 0;
# 486
if(_tmpE7 == d2->sc)
# 487
return(struct Cyc_Absyn_Datatypedecl*)d2;else{
# 489
{struct Cyc_Absyn_Datatypedecl*_tmp256;d2=((_tmp256=_cycalloc(sizeof(*_tmp256)),((
_tmp256[0]=*d2,_tmp256))));}
# 490
d2->sc=_tmpE7;
# 491
return(struct Cyc_Absyn_Datatypedecl*)d2;}};};}
# 495
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,
struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 497
struct _dyneither_ptr _tmp104=Cyc_Absynpp_qvar2string(d0->name);
# 498
int _tmp105=1;
# 501
enum Cyc_Absyn_Scope _tmp108;int _tmp109;const char*_tmp257;struct _tuple10 _tmp107=
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp257="enum",_tag_dyneither(_tmp257,
sizeof(char),5))),_tmp104,loc,msg,1);_tmp108=_tmp107.f1;_tmp109=_tmp107.f2;
# 502
if(!_tmp109)_tmp105=0;{
# 504
struct Cyc_Absyn_Enumdecl*d2;
# 507
{struct _tuple17 _tmp258;struct _tuple17 _tmp10B=(_tmp258.f1=d0->fields,((_tmp258.f2=
d1->fields,_tmp258)));struct Cyc_Core_Opt*_tmp10C;struct Cyc_Core_Opt*_tmp10D;
struct Cyc_Core_Opt*_tmp10E;struct Cyc_Core_Opt _tmp10F;struct Cyc_List_List*_tmp110;
struct Cyc_Core_Opt*_tmp111;struct Cyc_Core_Opt _tmp112;struct Cyc_List_List*_tmp113;
_LL31: _tmp10C=_tmp10B.f2;if(_tmp10C != 0)goto _LL33;_LL32:
# 508
 d2=d0;goto _LL30;_LL33: _tmp10D=_tmp10B.f1;if(_tmp10D != 0)goto _LL35;_LL34:
# 509
 d2=d1;goto _LL30;_LL35: _tmp10E=_tmp10B.f1;if(_tmp10E == 0)goto _LL30;_tmp10F=*
_tmp10E;_tmp110=(struct Cyc_List_List*)_tmp10F.v;_tmp111=_tmp10B.f2;if(_tmp111 == 
0)goto _LL30;_tmp112=*_tmp111;_tmp113=(struct Cyc_List_List*)_tmp112.v;_LL36:
# 512
 for(0;_tmp110 != 0  && _tmp113 != 0;(_tmp110=_tmp110->tl,_tmp113=_tmp113->tl)){
# 513
struct Cyc_Absyn_Enumfield _tmp115;struct _tuple0*_tmp116;struct _tuple0 _tmp117;
struct _dyneither_ptr*_tmp118;struct Cyc_Absyn_Exp*_tmp119;unsigned int _tmp11A;
struct Cyc_Absyn_Enumfield*_tmp114=(struct Cyc_Absyn_Enumfield*)_tmp110->hd;
_tmp115=*_tmp114;_tmp116=_tmp115.name;_tmp117=*_tmp116;_tmp118=_tmp117.f2;
_tmp119=_tmp115.tag;_tmp11A=_tmp115.loc;{
# 514
struct Cyc_Absyn_Enumfield _tmp11C;struct _tuple0*_tmp11D;struct _tuple0 _tmp11E;
struct _dyneither_ptr*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;unsigned int _tmp121;
struct Cyc_Absyn_Enumfield*_tmp11B=(struct Cyc_Absyn_Enumfield*)_tmp113->hd;
_tmp11C=*_tmp11B;_tmp11D=_tmp11C.name;_tmp11E=*_tmp11D;_tmp11F=_tmp11E.f2;
_tmp120=_tmp11C.tag;_tmp121=_tmp11C.loc;
# 517
if(Cyc_strptrcmp(_tmp118,_tmp11F)!= 0){
# 518
{const char*_tmp25E;void*_tmp25D[3];struct Cyc_String_pa_PrintArg_struct _tmp25C;
struct Cyc_String_pa_PrintArg_struct _tmp25B;struct Cyc_String_pa_PrintArg_struct
_tmp25A;(_tmp25A.tag=0,((_tmp25A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp11F),((_tmp25B.tag=0,((_tmp25B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp118),((_tmp25C.tag=0,((_tmp25C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp104),((_tmp25D[0]=& _tmp25C,((_tmp25D[1]=& _tmp25B,((
_tmp25D[2]=& _tmp25A,Cyc_Tcdecl_merr(loc,msg,((_tmp25E="enum %s: field name mismatch %s != %s",
_tag_dyneither(_tmp25E,sizeof(char),38))),_tag_dyneither(_tmp25D,sizeof(void*),3)))))))))))))))))));}
# 519
_tmp105=0;}
# 522
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp119))
!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp120))){
# 523
{const char*_tmp263;void*_tmp262[2];struct Cyc_String_pa_PrintArg_struct _tmp261;
struct Cyc_String_pa_PrintArg_struct _tmp260;(_tmp260.tag=0,((_tmp260.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp11F),((_tmp261.tag=0,((_tmp261.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp104),((_tmp262[0]=& _tmp261,((
_tmp262[1]=& _tmp260,Cyc_Tcdecl_merr(loc,msg,((_tmp263="enum %s, field %s, value mismatch",
_tag_dyneither(_tmp263,sizeof(char),34))),_tag_dyneither(_tmp262,sizeof(void*),2)))))))))))));}
# 524
_tmp105=0;}};}
# 527
d2=d0;
# 528
goto _LL30;_LL30:;}
# 531
if(!_tmp105)return 0;
# 533
if(d2->sc == _tmp108)
# 534
return(struct Cyc_Absyn_Enumdecl*)d2;else{
# 536
{struct Cyc_Absyn_Enumdecl*_tmp264;d2=((_tmp264=_cycalloc(sizeof(*_tmp264)),((
_tmp264[0]=*d2,_tmp264))));}
# 537
d2->sc=_tmp108;
# 538
return(struct Cyc_Absyn_Enumdecl*)d2;}};}
# 542
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,
void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope
sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct
_dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 546
int _tmp12C=1;
# 549
enum Cyc_Absyn_Scope _tmp12E;int _tmp12F;struct _tuple10 _tmp12D=Cyc_Tcdecl_merge_scope(
sc0,sc1,t,v,loc,msg,0);_tmp12E=_tmp12D.f1;_tmp12F=_tmp12D.f2;
# 550
if(!_tmp12F)_tmp12C=0;
# 553
if(!Cyc_Tcdecl_check_type(t0,t1)){
# 554
{const char*_tmp26B;void*_tmp26A[4];struct Cyc_String_pa_PrintArg_struct _tmp269;
struct Cyc_String_pa_PrintArg_struct _tmp268;struct Cyc_String_pa_PrintArg_struct
_tmp267;struct Cyc_String_pa_PrintArg_struct _tmp266;(_tmp266.tag=0,((_tmp266.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)
# 555
Cyc_Absynpp_typ2string(t0)),((_tmp267.tag=0,((_tmp267.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp268.tag=0,((_tmp268.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp269.tag=0,((_tmp269.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp26A[0]=& _tmp269,((_tmp26A[1]=&
_tmp268,((_tmp26A[2]=& _tmp267,((_tmp26A[3]=& _tmp266,Cyc_Tcdecl_merr(loc,msg,((
_tmp26B="%s %s has type \n%s\n instead of \n%s\n",_tag_dyneither(_tmp26B,sizeof(
char),36))),_tag_dyneither(_tmp26A,sizeof(void*),4)))))))))))))))))))))))));}
# 556
Cyc_Tcutil_explain_failure();
# 557
_tmp12C=0;}
# 561
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
# 562
{const char*_tmp270;void*_tmp26F[2];struct Cyc_String_pa_PrintArg_struct _tmp26E;
struct Cyc_String_pa_PrintArg_struct _tmp26D;(_tmp26D.tag=0,((_tmp26D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp26E.tag=0,((_tmp26E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp26F[0]=& _tmp26E,((_tmp26F[1]=&
_tmp26D,Cyc_Tcdecl_merr(loc,msg,((_tmp270="%s %s has different type qualifiers",
_tag_dyneither(_tmp270,sizeof(char),36))),_tag_dyneither(_tmp26F,sizeof(void*),2)))))))))))));}
# 563
_tmp12C=0;}
# 567
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
# 568
{const char*_tmp275;void*_tmp274[2];struct Cyc_String_pa_PrintArg_struct _tmp273;
struct Cyc_String_pa_PrintArg_struct _tmp272;(_tmp272.tag=0,((_tmp272.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp273.tag=0,((_tmp273.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp274[0]=& _tmp273,((_tmp274[1]=&
_tmp272,Cyc_Tcdecl_merr(loc,msg,((_tmp275="%s %s has different attributes",
_tag_dyneither(_tmp275,sizeof(char),31))),_tag_dyneither(_tmp274,sizeof(void*),2)))))))))))));}
# 569
{const char*_tmp278;void*_tmp277;(_tmp277=0,Cyc_fprintf(Cyc_stderr,((_tmp278="previous attributes: ",
_tag_dyneither(_tmp278,sizeof(char),22))),_tag_dyneither(_tmp277,sizeof(void*),0)));}
# 570
for(0;atts0 != 0;atts0=atts0->tl){
# 571
const char*_tmp27C;void*_tmp27B[1];struct Cyc_String_pa_PrintArg_struct _tmp27A;(
_tmp27A.tag=0,((_tmp27A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts0->hd)),((_tmp27B[0]=& _tmp27A,Cyc_fprintf(Cyc_stderr,((_tmp27C="%s ",
_tag_dyneither(_tmp27C,sizeof(char),4))),_tag_dyneither(_tmp27B,sizeof(void*),1)))))));}
# 572
{const char*_tmp27F;void*_tmp27E;(_tmp27E=0,Cyc_fprintf(Cyc_stderr,((_tmp27F="\ncurrent attributes: ",
_tag_dyneither(_tmp27F,sizeof(char),22))),_tag_dyneither(_tmp27E,sizeof(void*),0)));}
# 573
for(0;atts1 != 0;atts1=atts1->tl){
# 574
const char*_tmp283;void*_tmp282[1];struct Cyc_String_pa_PrintArg_struct _tmp281;(
_tmp281.tag=0,((_tmp281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts1->hd)),((_tmp282[0]=& _tmp281,Cyc_fprintf(Cyc_stderr,((_tmp283="%s ",
_tag_dyneither(_tmp283,sizeof(char),4))),_tag_dyneither(_tmp282,sizeof(void*),1)))))));}
# 575
{const char*_tmp286;void*_tmp285;(_tmp285=0,Cyc_fprintf(Cyc_stderr,((_tmp286="\n",
_tag_dyneither(_tmp286,sizeof(char),2))),_tag_dyneither(_tmp285,sizeof(void*),0)));}
# 576
_tmp12C=0;}{
# 579
struct _tuple10 _tmp287;return(_tmp287.f1=_tmp12E,((_tmp287.f2=_tmp12C,_tmp287)));};}
# 582
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,
struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 584
struct _dyneither_ptr _tmp14B=Cyc_Absynpp_qvar2string(d0->name);
# 587
enum Cyc_Absyn_Scope _tmp14E;int _tmp14F;const char*_tmp288;struct _tuple10 _tmp14D=
# 588
Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,d1->type,
d1->tq,d1->attributes,(
# 590
(_tmp288="variable",_tag_dyneither(_tmp288,sizeof(char),9))),_tmp14B,loc,msg);
_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;
# 591
if(!_tmp14F)return 0;
# 593
if(d0->sc == _tmp14E)
# 594
return(struct Cyc_Absyn_Vardecl*)d0;else{
# 596
struct Cyc_Absyn_Vardecl*_tmp289;struct Cyc_Absyn_Vardecl*_tmp150=(_tmp289=
_cycalloc(sizeof(*_tmp289)),((_tmp289[0]=*d0,_tmp289)));
# 597
((struct Cyc_Absyn_Vardecl*)_check_null(_tmp150))->sc=_tmp14E;
# 598
return _tmp150;}}
# 602
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 604
struct _dyneither_ptr _tmp152=Cyc_Absynpp_qvar2string(d0->name);
# 607
{const char*_tmp28A;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp28A="typedef",
_tag_dyneither(_tmp28A,sizeof(char),8))),_tmp152,loc,msg))return 0;}{
# 610
struct Cyc_List_List*_tmp154=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 613
if(d0->defn != 0  && d1->defn != 0){
# 614
void*subst_defn1=Cyc_Tcutil_substitute(_tmp154,(void*)_check_null(d1->defn));
# 615
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
# 616
{const char*_tmp290;void*_tmp28F[3];struct Cyc_String_pa_PrintArg_struct _tmp28E;
struct Cyc_String_pa_PrintArg_struct _tmp28D;struct Cyc_String_pa_PrintArg_struct
_tmp28C;(_tmp28C.tag=0,((_tmp28C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 617
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn))),((_tmp28D.tag=0,((_tmp28D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1)),((
_tmp28E.tag=0,((_tmp28E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp152),((
_tmp28F[0]=& _tmp28E,((_tmp28F[1]=& _tmp28D,((_tmp28F[2]=& _tmp28C,Cyc_Tcdecl_merr(
loc,msg,((_tmp290="typedef %s does not refer to the same type: %s != %s",
_tag_dyneither(_tmp290,sizeof(char),53))),_tag_dyneither(_tmp28F,sizeof(void*),3)))))))))))))))))));}
# 618
return 0;}}
# 621
return(struct Cyc_Absyn_Typedefdecl*)d0;};}
# 624
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct
_dyneither_ptr*msg){
# 625
struct _tuple14 _tmp291;struct _tuple14 _tmp15B=(_tmp291.f1=b0,((_tmp291.f2=b1,
_tmp291)));void*_tmp15C;void*_tmp15E;void*_tmp160;struct Cyc_Absyn_Vardecl*
_tmp162;void*_tmp163;struct Cyc_Absyn_Vardecl*_tmp165;void*_tmp166;struct Cyc_Absyn_Vardecl*
_tmp168;void*_tmp169;struct Cyc_Absyn_Fndecl*_tmp16B;void*_tmp16C;void*_tmp16E;
struct Cyc_Absyn_Fndecl*_tmp170;void*_tmp171;struct Cyc_Absyn_Fndecl*_tmp173;void*
_tmp174;struct Cyc_Absyn_Vardecl*_tmp176;_LL38: _tmp15C=_tmp15B.f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*
_tmp15D=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp15C;if(_tmp15D->tag
!= 0)goto _LL3A;};_tmp15E=_tmp15B.f2;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*
_tmp15F=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp15E;if(_tmp15F->tag
!= 0)goto _LL3A;};_LL39:
# 626
 return(void*)& Cyc_Absyn_Unresolved_b_val;_LL3A: _tmp160=_tmp15B.f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp161=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp160;if(_tmp161->tag
!= 1)goto _LL3C;else{_tmp162=_tmp161->f1;}};_tmp163=_tmp15B.f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp164=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp163;if(_tmp164->tag
!= 1)goto _LL3C;else{_tmp165=_tmp164->f1;}};_LL3B: {
# 628
struct Cyc_Absyn_Vardecl*_tmp177=Cyc_Tcdecl_merge_vardecl(_tmp162,_tmp165,loc,
msg);
# 629
if(_tmp177 == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
# 630
if(_tmp177 == (struct Cyc_Absyn_Vardecl*)_tmp162)return b0;
# 631
if(_tmp177 == (struct Cyc_Absyn_Vardecl*)_tmp165)return b1;{
# 632
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp294;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp293;return(void*)((_tmp293=_cycalloc(sizeof(*_tmp293)),((_tmp293[0]=((
_tmp294.tag=1,((_tmp294.f1=(struct Cyc_Absyn_Vardecl*)_tmp177,_tmp294)))),_tmp293))));};}
_LL3C: _tmp166=_tmp15B.f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp167=(
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp166;if(_tmp167->tag != 1)goto
_LL3E;else{_tmp168=_tmp167->f1;}};_tmp169=_tmp15B.f2;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmp16A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp169;if(_tmp16A->tag
!= 2)goto _LL3E;else{_tmp16B=_tmp16A->f1;}};_LL3D: {
# 634
int _tmp17C;const char*_tmp295;struct _tuple10 _tmp17B=Cyc_Tcdecl_check_var_or_fn_decl(
_tmp168->sc,_tmp168->type,_tmp168->tq,_tmp168->attributes,_tmp16B->sc,(void*)
_check_null(_tmp16B->cached_typ),
# 635
Cyc_Absyn_empty_tqual(0),_tmp16B->attributes,(
# 636
(_tmp295="function",_tag_dyneither(_tmp295,sizeof(char),9))),
# 637
Cyc_Absynpp_qvar2string(_tmp168->name),loc,msg);_tmp17C=_tmp17B.f2;
# 638
return _tmp17C?b1:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL3E: _tmp16C=_tmp15B.f1;{
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp16D=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)
_tmp16C;if(_tmp16D->tag != 2)goto _LL40;};_tmp16E=_tmp15B.f2;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmp16F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp16E;if(_tmp16F->tag
!= 2)goto _LL40;else{_tmp170=_tmp16F->f1;}};_LL3F:
# 640
{const char*_tmp299;void*_tmp298[1];struct Cyc_String_pa_PrintArg_struct _tmp297;(
_tmp297.tag=0,((_tmp297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp170->name)),((_tmp298[0]=& _tmp297,Cyc_Tcdecl_merr(loc,msg,((_tmp299="redefinition of function %s",
_tag_dyneither(_tmp299,sizeof(char),28))),_tag_dyneither(_tmp298,sizeof(void*),1)))))));}
# 641
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL40: _tmp171=_tmp15B.f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*
_tmp172=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp171;if(_tmp172->tag
!= 2)goto _LL42;else{_tmp173=_tmp172->f1;}};_tmp174=_tmp15B.f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*
_tmp175=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp174;if(_tmp175->tag
!= 1)goto _LL42;else{_tmp176=_tmp175->f1;}};_LL41: {
# 643
int _tmp182;const char*_tmp29A;struct _tuple10 _tmp181=Cyc_Tcdecl_check_var_or_fn_decl(
_tmp173->sc,(void*)_check_null(_tmp173->cached_typ),Cyc_Absyn_empty_tqual(0),
_tmp173->attributes,_tmp176->sc,_tmp176->type,_tmp176->tq,_tmp176->attributes,(
# 645
(_tmp29A="variable",_tag_dyneither(_tmp29A,sizeof(char),9))),
# 646
Cyc_Absynpp_qvar2string(_tmp173->name),loc,msg);_tmp182=_tmp181.f2;
# 647
return _tmp182?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL42:;_LL43: {
# 648
struct Cyc_Core_Invalid_argument_exn_struct _tmp2A0;const char*_tmp29F;struct Cyc_Core_Invalid_argument_exn_struct*
_tmp29E;(int)_throw((void*)((_tmp29E=_cycalloc(sizeof(*_tmp29E)),((_tmp29E[0]=((
_tmp2A0.tag=Cyc_Core_Invalid_argument,((_tmp2A0.f1=((_tmp29F="Tcdecl::merge_binding",
_tag_dyneither(_tmp29F,sizeof(char),22))),_tmp2A0)))),_tmp29E)))));}_LL37:;}
# 652
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct
Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
unsigned int loc,struct _dyneither_ptr*msg){
# 655
struct Cyc_Tcdecl_Xdatatypefielddecl _tmp187;struct Cyc_Absyn_Datatypedecl*_tmp188;
struct Cyc_Absyn_Datatypefield*_tmp189;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp186=d0;_tmp187=*_tmp186;_tmp188=_tmp187.base;_tmp189=_tmp187.field;{
# 656
struct Cyc_Tcdecl_Xdatatypefielddecl _tmp18B;struct Cyc_Absyn_Datatypedecl*_tmp18C;
struct Cyc_Absyn_Datatypefield*_tmp18D;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp18A=d1;_tmp18B=*_tmp18A;_tmp18C=_tmp18B.base;_tmp18D=_tmp18B.field;{
# 657
struct _dyneither_ptr _tmp18E=Cyc_Absynpp_qvar2string(_tmp189->name);
# 660
if(Cyc_Absyn_qvar_cmp(_tmp188->name,_tmp18C->name)!= 0){
# 661
{const char*_tmp2A6;void*_tmp2A5[3];struct Cyc_String_pa_PrintArg_struct _tmp2A4;
struct Cyc_String_pa_PrintArg_struct _tmp2A3;struct Cyc_String_pa_PrintArg_struct
_tmp2A2;(_tmp2A2.tag=0,((_tmp2A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp18E),((_tmp2A3.tag=0,((_tmp2A3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)
# 662
Cyc_Absynpp_qvar2string(_tmp18C->name)),((_tmp2A4.tag=0,((_tmp2A4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp188->name)),((
_tmp2A5[0]=& _tmp2A4,((_tmp2A5[1]=& _tmp2A3,((_tmp2A5[2]=& _tmp2A2,Cyc_Tcdecl_merr(
loc,msg,((_tmp2A6="@extensible datatype %s and %s have both a field named %s",
_tag_dyneither(_tmp2A6,sizeof(char),58))),_tag_dyneither(_tmp2A5,sizeof(void*),3)))))))))))))))))));}
# 663
return 0;}
# 667
{const char*_tmp2A7;if(!Cyc_Tcdecl_check_tvs(_tmp188->tvs,_tmp18C->tvs,((_tmp2A7="@extensible datatype",
_tag_dyneither(_tmp2A7,sizeof(char),21))),Cyc_Absynpp_qvar2string(_tmp188->name),
loc,msg))
# 668
return 0;}{
# 670
struct Cyc_List_List*_tmp195=Cyc_Tcdecl_build_tvs_map(_tmp188->tvs,_tmp18C->tvs);
# 673
const char*_tmp2A8;struct Cyc_Absyn_Datatypefield*_tmp196=Cyc_Tcdecl_merge_datatypefield(
_tmp189,_tmp18D,_tmp195,((_tmp2A8="@extensible datatype field",_tag_dyneither(
_tmp2A8,sizeof(char),27))),_tmp18E,msg);
# 674
if(_tmp196 == 0)return 0;
# 675
if(_tmp196 == (struct Cyc_Absyn_Datatypefield*)_tmp189)return(struct Cyc_Tcdecl_Xdatatypefielddecl*)
d0;{
# 676
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp2A9;return(_tmp2A9=_cycalloc(sizeof(*
_tmp2A9)),((_tmp2A9->base=_tmp188,((_tmp2A9->field=(struct Cyc_Absyn_Datatypefield*)
_tmp196,_tmp2A9)))));};};};};}
