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
  _check_dyneither_subscript_fn(a,s,i,__FILE__,__LINE__)
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
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 107 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
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
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 246
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 366
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 370
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
# 41
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 80 "absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 176
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 181
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 188
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 208
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 210
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 219
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
# 281
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 507
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 533
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 540
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 558
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 719 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 924
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 932
void*Cyc_Absyn_compress_kb(void*);
# 937
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 967
extern void*Cyc_Absyn_bounds_one;
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1151
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1157
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 29 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 30
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 51
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 85
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 96
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 65 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 297 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 56
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 62
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 64
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 71
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
# 72
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);
# 73
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
# 74
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);
# 77
int Cyc_Absynpp_is_anon_aggrtype(void*t);
# 78
extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;
# 79
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
# 80
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
# 81
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
# 82
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
# 83
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
# 84
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
# 85
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 87
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);
# 27 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 43 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp);
# 44
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);
# 45
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
# 46
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
# 47
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
# 48
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);
# 49
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
# 50
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
# 51
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
# 52
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
# 53
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
# 54
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
# 55
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
# 56
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 57
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
# 58
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po);
# 63
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
# 64
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
# 65
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
# 66
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
# 67
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
# 68
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 70
static int Cyc_Absynpp_expand_typedefs;
# 74
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4]="Cyc";
# 76
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4};
# 77
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 79
static int Cyc_Absynpp_add_cyc_prefix;
# 83
static int Cyc_Absynpp_to_VC;
# 86
static int Cyc_Absynpp_decls_first;
# 90
static int Cyc_Absynpp_rewrite_temp_tvars;
# 93
static int Cyc_Absynpp_print_all_tvars;
# 96
static int Cyc_Absynpp_print_all_kinds;
# 99
static int Cyc_Absynpp_print_all_effects;
# 102
static int Cyc_Absynpp_print_using_stmts;
# 107
static int Cyc_Absynpp_print_externC_stmts;
# 111
static int Cyc_Absynpp_print_full_evars;
# 114
static int Cyc_Absynpp_generate_line_directives;
# 117
static int Cyc_Absynpp_use_curr_namespace;
# 120
static int Cyc_Absynpp_print_zeroterm;
# 123
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 126
int Cyc_Absynpp_print_for_cycdoc=0;struct Cyc_Absynpp_Params;
# 147
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
# 148
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
# 149
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
# 150
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
# 151
Cyc_Absynpp_to_VC=fs->to_VC;
# 152
Cyc_Absynpp_decls_first=fs->decls_first;
# 153
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
# 154
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
# 155
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
# 156
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
# 157
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
# 158
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
# 159
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
# 160
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
# 161
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
# 162
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
# 163
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 166
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 186
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,0,0,1,0,0,1,1,1,0,1,0,1,0};
# 206
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 226
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0};
# 247
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
# 248
struct Cyc_List_List*_tmp60E;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp60E=_cycalloc(sizeof(*_tmp60E)),((_tmp60E->hd=v,((_tmp60E->tl=0,_tmp60E)))))));}
# 251
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
# 252
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
# 253
*l=0;else{
# 255
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 259
static void Cyc_Absynpp_curr_namespace_drop(){
# 260
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 263
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){
# 264
switch(c){case '\a': _LL0: {
# 265
const char*_tmp60F;return(_tmp60F="\\a",_tag_dyneither(_tmp60F,sizeof(char),3));}case '\b': _LL1: {
# 266
const char*_tmp610;return(_tmp610="\\b",_tag_dyneither(_tmp610,sizeof(char),3));}case '\f': _LL2: {
# 267
const char*_tmp611;return(_tmp611="\\f",_tag_dyneither(_tmp611,sizeof(char),3));}case '\n': _LL3: {
# 268
const char*_tmp612;return(_tmp612="\\n",_tag_dyneither(_tmp612,sizeof(char),3));}case '\r': _LL4: {
# 269
const char*_tmp613;return(_tmp613="\\r",_tag_dyneither(_tmp613,sizeof(char),3));}case '\t': _LL5: {
# 270
const char*_tmp614;return(_tmp614="\\t",_tag_dyneither(_tmp614,sizeof(char),3));}case '\v': _LL6: {
# 271
const char*_tmp615;return(_tmp615="\\v",_tag_dyneither(_tmp615,sizeof(char),3));}case '\\': _LL7: {
# 272
const char*_tmp616;return(_tmp616="\\\\",_tag_dyneither(_tmp616,sizeof(char),3));}case '"': _LL8: {
# 273
const char*_tmp617;return(_tmp617="\"",_tag_dyneither(_tmp617,sizeof(char),2));}case '\'': _LL9: {
# 274
const char*_tmp618;return(_tmp618="\\'",_tag_dyneither(_tmp618,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
# 277
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
# 278
{char _tmp61B;char _tmp61A;struct _dyneither_ptr _tmp619;(_tmp619=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp61A=*((char*)_check_dyneither_subscript(_tmp619,sizeof(char),0)),((_tmp61B=c,((_get_dyneither_size(_tmp619,sizeof(char))== 1  && (_tmp61A == '\000'  && _tmp61B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp619.curr)=_tmp61B)))))));}
# 279
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
# 282
int j=0;
# 283
{char _tmp61E;char _tmp61D;struct _dyneither_ptr _tmp61C;(_tmp61C=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp61D=*((char*)_check_dyneither_subscript(_tmp61C,sizeof(char),0)),((_tmp61E='\\',((_get_dyneither_size(_tmp61C,sizeof(char))== 1  && (_tmp61D == '\000'  && _tmp61E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61C.curr)=_tmp61E)))))));}
# 284
{char _tmp621;char _tmp620;struct _dyneither_ptr _tmp61F;(_tmp61F=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp620=*((char*)_check_dyneither_subscript(_tmp61F,sizeof(char),0)),((_tmp621=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp61F,sizeof(char))== 1  && (_tmp620 == '\000'  && _tmp621 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61F.curr)=_tmp621)))))));}
# 285
{char _tmp624;char _tmp623;struct _dyneither_ptr _tmp622;(_tmp622=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp623=*((char*)_check_dyneither_subscript(_tmp622,sizeof(char),0)),((_tmp624=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp622,sizeof(char))== 1  && (_tmp623 == '\000'  && _tmp624 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp622.curr)=_tmp624)))))));}
# 286
{char _tmp627;char _tmp626;struct _dyneither_ptr _tmp625;(_tmp625=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp626=*((char*)_check_dyneither_subscript(_tmp625,sizeof(char),0)),((_tmp627=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp625,sizeof(char))== 1  && (_tmp626 == '\000'  && _tmp627 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp625.curr)=_tmp627)))))));}
# 287
return(struct _dyneither_ptr)_tmp10;}}}
# 292
static int Cyc_Absynpp_special(struct _dyneither_ptr s){
# 293
int sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);
# 294
{int i=0;for(0;i < sz;++ i){
# 295
char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 296
if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')
# 297
return 1;}}
# 299
return 0;}
# 302
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){
# 303
if(!Cyc_Absynpp_special(s))return s;{
# 305
int n=(int)(_get_dyneither_size(s,sizeof(char))- 1);
# 307
if(n > 0  && *((const char*)_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{
# 309
int len=0;
# 310
{int i=0;for(0;i <= n;++ i){
# 311
char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE:
# 312
 goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10:
# 313
 goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12:
# 314
 goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14:
# 315
 goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;_LL16:
# 316
 goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18:
# 317
 goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A:
# 318
 goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C:
# 319
 goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E:
# 320
 len +=2;goto _LLC;_LL1F:;_LL20:
# 322
 if(_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{
# 323
len +=4;}
# 324
goto _LLC;_LLC:;}}{
# 327
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
# 328
int j=0;
# 329
{int i=0;for(0;i <= n;++ i){
# 330
char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:
# 331
{char _tmp62A;char _tmp629;struct _dyneither_ptr _tmp628;(_tmp628=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp629=*((char*)_check_dyneither_subscript(_tmp628,sizeof(char),0)),((_tmp62A='\\',((_get_dyneither_size(_tmp628,sizeof(char))== 1  && (_tmp629 == '\000'  && _tmp62A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp628.curr)=_tmp62A)))))));}{char _tmp62D;char _tmp62C;struct _dyneither_ptr _tmp62B;(_tmp62B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62C=*((char*)_check_dyneither_subscript(_tmp62B,sizeof(char),0)),((_tmp62D='a',((_get_dyneither_size(_tmp62B,sizeof(char))== 1  && (_tmp62C == '\000'  && _tmp62D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62B.curr)=_tmp62D)))))));}goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:
# 332
{char _tmp630;char _tmp62F;struct _dyneither_ptr _tmp62E;(_tmp62E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62F=*((char*)_check_dyneither_subscript(_tmp62E,sizeof(char),0)),((_tmp630='\\',((_get_dyneither_size(_tmp62E,sizeof(char))== 1  && (_tmp62F == '\000'  && _tmp630 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62E.curr)=_tmp630)))))));}{char _tmp633;char _tmp632;struct _dyneither_ptr _tmp631;(_tmp631=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp632=*((char*)_check_dyneither_subscript(_tmp631,sizeof(char),0)),((_tmp633='b',((_get_dyneither_size(_tmp631,sizeof(char))== 1  && (_tmp632 == '\000'  && _tmp633 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp631.curr)=_tmp633)))))));}goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:
# 333
{char _tmp636;char _tmp635;struct _dyneither_ptr _tmp634;(_tmp634=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp635=*((char*)_check_dyneither_subscript(_tmp634,sizeof(char),0)),((_tmp636='\\',((_get_dyneither_size(_tmp634,sizeof(char))== 1  && (_tmp635 == '\000'  && _tmp636 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp634.curr)=_tmp636)))))));}{char _tmp639;char _tmp638;struct _dyneither_ptr _tmp637;(_tmp637=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp638=*((char*)_check_dyneither_subscript(_tmp637,sizeof(char),0)),((_tmp639='f',((_get_dyneither_size(_tmp637,sizeof(char))== 1  && (_tmp638 == '\000'  && _tmp639 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp637.curr)=_tmp639)))))));}goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:
# 334
{char _tmp63C;char _tmp63B;struct _dyneither_ptr _tmp63A;(_tmp63A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63B=*((char*)_check_dyneither_subscript(_tmp63A,sizeof(char),0)),((_tmp63C='\\',((_get_dyneither_size(_tmp63A,sizeof(char))== 1  && (_tmp63B == '\000'  && _tmp63C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63A.curr)=_tmp63C)))))));}{char _tmp63F;char _tmp63E;struct _dyneither_ptr _tmp63D;(_tmp63D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63E=*((char*)_check_dyneither_subscript(_tmp63D,sizeof(char),0)),((_tmp63F='n',((_get_dyneither_size(_tmp63D,sizeof(char))== 1  && (_tmp63E == '\000'  && _tmp63F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63D.curr)=_tmp63F)))))));}goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:
# 335
{char _tmp642;char _tmp641;struct _dyneither_ptr _tmp640;(_tmp640=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp641=*((char*)_check_dyneither_subscript(_tmp640,sizeof(char),0)),((_tmp642='\\',((_get_dyneither_size(_tmp640,sizeof(char))== 1  && (_tmp641 == '\000'  && _tmp642 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp640.curr)=_tmp642)))))));}{char _tmp645;char _tmp644;struct _dyneither_ptr _tmp643;(_tmp643=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp644=*((char*)_check_dyneither_subscript(_tmp643,sizeof(char),0)),((_tmp645='r',((_get_dyneither_size(_tmp643,sizeof(char))== 1  && (_tmp644 == '\000'  && _tmp645 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp643.curr)=_tmp645)))))));}goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:
# 336
{char _tmp648;char _tmp647;struct _dyneither_ptr _tmp646;(_tmp646=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp647=*((char*)_check_dyneither_subscript(_tmp646,sizeof(char),0)),((_tmp648='\\',((_get_dyneither_size(_tmp646,sizeof(char))== 1  && (_tmp647 == '\000'  && _tmp648 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp646.curr)=_tmp648)))))));}{char _tmp64B;char _tmp64A;struct _dyneither_ptr _tmp649;(_tmp649=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64A=*((char*)_check_dyneither_subscript(_tmp649,sizeof(char),0)),((_tmp64B='t',((_get_dyneither_size(_tmp649,sizeof(char))== 1  && (_tmp64A == '\000'  && _tmp64B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp649.curr)=_tmp64B)))))));}goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:
# 337
{char _tmp64E;char _tmp64D;struct _dyneither_ptr _tmp64C;(_tmp64C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64D=*((char*)_check_dyneither_subscript(_tmp64C,sizeof(char),0)),((_tmp64E='\\',((_get_dyneither_size(_tmp64C,sizeof(char))== 1  && (_tmp64D == '\000'  && _tmp64E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64C.curr)=_tmp64E)))))));}{char _tmp651;char _tmp650;struct _dyneither_ptr _tmp64F;(_tmp64F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp650=*((char*)_check_dyneither_subscript(_tmp64F,sizeof(char),0)),((_tmp651='v',((_get_dyneither_size(_tmp64F,sizeof(char))== 1  && (_tmp650 == '\000'  && _tmp651 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64F.curr)=_tmp651)))))));}goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:
# 338
{char _tmp654;char _tmp653;struct _dyneither_ptr _tmp652;(_tmp652=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp653=*((char*)_check_dyneither_subscript(_tmp652,sizeof(char),0)),((_tmp654='\\',((_get_dyneither_size(_tmp652,sizeof(char))== 1  && (_tmp653 == '\000'  && _tmp654 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp652.curr)=_tmp654)))))));}{char _tmp657;char _tmp656;struct _dyneither_ptr _tmp655;(_tmp655=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp656=*((char*)_check_dyneither_subscript(_tmp655,sizeof(char),0)),((_tmp657='\\',((_get_dyneither_size(_tmp655,sizeof(char))== 1  && (_tmp656 == '\000'  && _tmp657 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp655.curr)=_tmp657)))))));}goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:
# 339
{char _tmp65A;char _tmp659;struct _dyneither_ptr _tmp658;(_tmp658=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp659=*((char*)_check_dyneither_subscript(_tmp658,sizeof(char),0)),((_tmp65A='\\',((_get_dyneither_size(_tmp658,sizeof(char))== 1  && (_tmp659 == '\000'  && _tmp65A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp658.curr)=_tmp65A)))))));}{char _tmp65D;char _tmp65C;struct _dyneither_ptr _tmp65B;(_tmp65B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65C=*((char*)_check_dyneither_subscript(_tmp65B,sizeof(char),0)),((_tmp65D='"',((_get_dyneither_size(_tmp65B,sizeof(char))== 1  && (_tmp65C == '\000'  && _tmp65D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65B.curr)=_tmp65D)))))));}goto _LL21;_LL34:;_LL35:
# 341
 if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp660;char _tmp65F;struct _dyneither_ptr _tmp65E;(_tmp65E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp65F=*((char*)_check_dyneither_subscript(_tmp65E,sizeof(char),0)),((_tmp660=_tmp1E,((_get_dyneither_size(_tmp65E,sizeof(char))== 1  && (_tmp65F == '\000'  && _tmp660 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp65E.curr)=_tmp660)))))));}else{
# 343
{char _tmp663;char _tmp662;struct _dyneither_ptr _tmp661;(_tmp661=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp662=*((char*)_check_dyneither_subscript(_tmp661,sizeof(char),0)),((_tmp663='\\',((_get_dyneither_size(_tmp661,sizeof(char))== 1  && (_tmp662 == '\000'  && _tmp663 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp661.curr)=_tmp663)))))));}
# 344
{char _tmp666;char _tmp665;struct _dyneither_ptr _tmp664;(_tmp664=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp665=*((char*)_check_dyneither_subscript(_tmp664,sizeof(char),0)),((_tmp666=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp664,sizeof(char))== 1  && (_tmp665 == '\000'  && _tmp666 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp664.curr)=_tmp666)))))));}
# 345
{char _tmp669;char _tmp668;struct _dyneither_ptr _tmp667;(_tmp667=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp668=*((char*)_check_dyneither_subscript(_tmp667,sizeof(char),0)),((_tmp669=(char)('0' + (_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp667,sizeof(char))== 1  && (_tmp668 == '\000'  && _tmp669 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp667.curr)=_tmp669)))))));}{
# 346
char _tmp66C;char _tmp66B;struct _dyneither_ptr _tmp66A;(_tmp66A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp66B=*((char*)_check_dyneither_subscript(_tmp66A,sizeof(char),0)),((_tmp66C=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(_tmp66A,sizeof(char))== 1  && (_tmp66B == '\000'  && _tmp66C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp66A.curr)=_tmp66C)))))));};}
# 348
goto _LL21;_LL21:;}}
# 350
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";
# 353
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char _tmp65[9]="volatile";
# 354
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";
# 355
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};
# 356
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
# 357
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
# 358
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 360
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
# 361
struct Cyc_List_List*l=0;
# 363
if(tq.q_restrict){struct Cyc_List_List*_tmp66D;l=((_tmp66D=_cycalloc(sizeof(*_tmp66D)),((_tmp66D->hd=Cyc_Absynpp_restrict_sp,((_tmp66D->tl=l,_tmp66D))))));}
# 364
if(tq.q_volatile){struct Cyc_List_List*_tmp66E;l=((_tmp66E=_cycalloc(sizeof(*_tmp66E)),((_tmp66E->hd=Cyc_Absynpp_volatile_sp,((_tmp66E->tl=l,_tmp66E))))));}
# 365
if(tq.print_const){struct Cyc_List_List*_tmp66F;l=((_tmp66F=_cycalloc(sizeof(*_tmp66F)),((_tmp66F->hd=Cyc_Absynpp_const_sp,((_tmp66F->tl=l,_tmp66F))))));}{
# 366
const char*_tmp672;const char*_tmp671;const char*_tmp670;return Cyc_PP_egroup(((_tmp670="",_tag_dyneither(_tmp670,sizeof(char),1))),((_tmp671=" ",_tag_dyneither(_tmp671,sizeof(char),2))),((_tmp672=" ",_tag_dyneither(_tmp672,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
# 370
struct Cyc_Absyn_Kind _tmp6E;enum Cyc_Absyn_KindQual _tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*_tmp6D=ka;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;
# 371
switch(_tmp6F){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {
# 374
const char*_tmp673;return(_tmp673="A",_tag_dyneither(_tmp673,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
# 375
const char*_tmp674;return(_tmp674="UA",_tag_dyneither(_tmp674,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
# 376
const char*_tmp675;return(_tmp675="TA",_tag_dyneither(_tmp675,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL3D: {
# 380
const char*_tmp676;return(_tmp676="M",_tag_dyneither(_tmp676,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
# 381
const char*_tmp677;return(_tmp677="UM",_tag_dyneither(_tmp677,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
# 382
const char*_tmp678;return(_tmp678="TM",_tag_dyneither(_tmp678,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL42: {
# 386
const char*_tmp679;return(_tmp679="B",_tag_dyneither(_tmp679,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
# 387
const char*_tmp67A;return(_tmp67A="UB",_tag_dyneither(_tmp67A,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
# 388
const char*_tmp67B;return(_tmp67B="TB",_tag_dyneither(_tmp67B,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp70){case Cyc_Absyn_Aliasable: _LL47: {
# 392
const char*_tmp67C;return(_tmp67C="R",_tag_dyneither(_tmp67C,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
# 393
const char*_tmp67D;return(_tmp67D="UR",_tag_dyneither(_tmp67D,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
# 394
const char*_tmp67E;return(_tmp67E="TR",_tag_dyneither(_tmp67E,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp67F;return(_tmp67F="E",_tag_dyneither(_tmp67F,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
# 397
const char*_tmp680;return(_tmp680="I",_tag_dyneither(_tmp680,sizeof(char),2));}}}
# 400
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 402
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
# 403
void*_tmp7F=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp80=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{_tmp81=_tmp80->f1;}}_LL4F:
# 404
 return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7F;if(_tmp82->tag != 1)goto _LL52;}_LL51:
# 406
 if(Cyc_PP_tex_output){
# 407
const char*_tmp681;return(_tmp681="{?}",_tag_dyneither(_tmp681,sizeof(char),4));}else{
# 408
const char*_tmp682;return(_tmp682="?",_tag_dyneither(_tmp682,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp83=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F;if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {
# 409
const char*_tmp686;void*_tmp685[1];struct Cyc_String_pa_PrintArg_struct _tmp684;return(struct _dyneither_ptr)((_tmp684.tag=0,((_tmp684.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp84)),((_tmp685[0]=& _tmp684,Cyc_aprintf(((_tmp686="<=%s",_tag_dyneither(_tmp686,sizeof(char),5))),_tag_dyneither(_tmp685,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
# 414
void*_tmp8A=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{_tmp8C=_tmp8B->f1;}}_LL56:
# 415
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8C));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
# 418
const char*_tmp687;return Cyc_PP_text_width(((_tmp687="{?}",_tag_dyneither(_tmp687,sizeof(char),4))),1);}else{
# 419
const char*_tmp688;return Cyc_PP_text(((_tmp688="?",_tag_dyneither(_tmp688,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;else{_tmp8F=_tmp8E->f2;}}_LL5A:
# 420
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
# 425
const char*_tmp68B;const char*_tmp68A;const char*_tmp689;return Cyc_PP_egroup(((_tmp689="<",_tag_dyneither(_tmp689,sizeof(char),2))),((_tmp68A=">",_tag_dyneither(_tmp68A,sizeof(char),2))),((_tmp68B=",",_tag_dyneither(_tmp68B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
# 429
void*_tmp95=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp98;struct Cyc_Absyn_Kind _tmp99;enum Cyc_Absyn_KindQual _tmp9A;enum Cyc_Absyn_AliasQual _tmp9B;struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp96=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp95;if(_tmp96->tag != 1)goto _LL5E;}_LL5D:
# 430
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp97=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95;if(_tmp97->tag != 0)goto _LL60;else{_tmp98=_tmp97->f1;_tmp99=*_tmp98;_tmp9A=_tmp99.kind;if(_tmp9A != Cyc_Absyn_BoxKind)goto _LL60;_tmp9B=_tmp99.aliasqual;if(_tmp9B != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F:
# 431
 return Cyc_PP_textptr(tv->name);_LL60: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp95;if(_tmp9C->tag != 2)goto _LL62;else{_tmp9D=_tmp9C->f2;}}_LL61:
# 432
 _tmp9F=_tmp9D;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp95;if(_tmp9E->tag != 0)goto _LL5B;else{_tmp9F=_tmp9E->f1;}}_LL63: {
# 433
const char*_tmp68E;struct Cyc_PP_Doc*_tmp68D[3];return(_tmp68D[2]=Cyc_Absynpp_kind2doc(_tmp9F),((_tmp68D[1]=Cyc_PP_text(((_tmp68E="::",_tag_dyneither(_tmp68E,sizeof(char),3)))),((_tmp68D[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(_tmp68D,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
# 437
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
# 438
const char*_tmp691;const char*_tmp690;const char*_tmp68F;return Cyc_PP_egroup(((_tmp68F="<",_tag_dyneither(_tmp68F,sizeof(char),2))),((_tmp690=">",_tag_dyneither(_tmp690,sizeof(char),2))),((_tmp691=",",_tag_dyneither(_tmp691,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 441
static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
# 442
return tv->name;}
# 445
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
# 446
if(Cyc_Absynpp_print_all_kinds)
# 447
return Cyc_Absynpp_ktvars2doc(tvs);{
# 448
const char*_tmp694;const char*_tmp693;const char*_tmp692;return Cyc_PP_egroup(((_tmp692="<",_tag_dyneither(_tmp692,sizeof(char),2))),((_tmp693=">",_tag_dyneither(_tmp693,sizeof(char),2))),((_tmp694=",",_tag_dyneither(_tmp694,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 451
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
# 452
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 455
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
# 456
const char*_tmp697;const char*_tmp696;const char*_tmp695;return Cyc_PP_group(((_tmp695="(",_tag_dyneither(_tmp695,sizeof(char),2))),((_tmp696=")",_tag_dyneither(_tmp696,sizeof(char),2))),((_tmp697=",",_tag_dyneither(_tmp697,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 459
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
# 460
void*_tmpAB=att;_LL65: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAC=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAC->tag != 1)goto _LL67;}_LL66:
# 461
 goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpAD=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAD->tag != 2)goto _LL69;}_LL68:
# 462
 goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpAE=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAE->tag != 3)goto _LL6B;}_LL6A:
# 463
 return Cyc_PP_nil_doc();_LL6B:;_LL6C:
# 464
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}
# 468
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
# 469
for(0;atts != 0;atts=atts->tl){
# 470
void*_tmpAF=(void*)atts->hd;_LL6E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB0=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {
# 471
const char*_tmp698;return Cyc_PP_text(((_tmp698=" _stdcall ",_tag_dyneither(_tmp698,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL72;}_LL71: {
# 472
const char*_tmp699;return Cyc_PP_text(((_tmp699=" _cdecl ",_tag_dyneither(_tmp699,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}_LL73: {
# 473
const char*_tmp69A;return Cyc_PP_text(((_tmp69A=" _fastcall ",_tag_dyneither(_tmp69A,sizeof(char),12))));}_LL74:;_LL75:
# 474
 goto _LL6D;_LL6D:;}
# 476
return Cyc_PP_nil_doc();}
# 479
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
# 480
int hasatt=0;
# 481
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
# 482
void*_tmpB6=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB7=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
# 483
 goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A:
# 484
 goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB9=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}_LL7C:
# 485
 goto _LL76;_LL7D:;_LL7E:
# 486
 hasatt=1;goto _LL76;_LL76:;}}
# 488
if(!hasatt)
# 489
return Cyc_PP_nil_doc();{
# 490
const char*_tmp6A5;const char*_tmp6A4;const char*_tmp6A3;const char*_tmp6A2;const char*_tmp6A1;struct Cyc_PP_Doc*_tmp6A0[3];return(_tmp6A0[2]=
# 492
Cyc_PP_text(((_tmp6A1=")",_tag_dyneither(_tmp6A1,sizeof(char),2)))),((_tmp6A0[1]=
# 491
Cyc_PP_group(((_tmp6A4="",_tag_dyneither(_tmp6A4,sizeof(char),1))),((_tmp6A3="",_tag_dyneither(_tmp6A3,sizeof(char),1))),((_tmp6A2=" ",_tag_dyneither(_tmp6A2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp6A0[0]=
# 490
Cyc_PP_text(((_tmp6A5=" __declspec(",_tag_dyneither(_tmp6A5,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp6A0,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 495
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
# 496
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 499
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
# 500
if(atts == 0)return Cyc_PP_nil_doc();
# 501
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F:
# 502
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
# 503
const char*_tmp6AE;const char*_tmp6AD;const char*_tmp6AC;const char*_tmp6AB;struct Cyc_PP_Doc*_tmp6AA[2];return(_tmp6AA[1]=
# 504
Cyc_PP_group(((_tmp6AD="((",_tag_dyneither(_tmp6AD,sizeof(char),3))),((_tmp6AC="))",_tag_dyneither(_tmp6AC,sizeof(char),3))),((_tmp6AB=",",_tag_dyneither(_tmp6AB,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp6AA[0]=
# 503
Cyc_PP_text(((_tmp6AE=" __attribute__",_tag_dyneither(_tmp6AE,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp6AA,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 508
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
# 509
if(tms == 0)return 0;{
# 510
void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC6=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}_LL84:
# 511
 return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86:
# 513
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL89:
# 514
 return 0;default: _LL8A:
# 515
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL87:;_LL88:
# 517
 return 0;_LL82:;};}
# 521
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 524
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
# 525
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
# 526
if(!((unsigned int)Cyc_Absynpp_cache_question)){
# 527
if(Cyc_PP_tex_output){
# 528
const char*_tmp6AF;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6AF="{?}",_tag_dyneither(_tmp6AF,sizeof(char),4))),1);}else{
# 529
const char*_tmp6B0;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B0="?",_tag_dyneither(_tmp6B0,sizeof(char),2))));}}
# 531
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 533
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
# 534
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
# 535
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
# 536
if(Cyc_PP_tex_output){
# 537
const char*_tmp6B1;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6B1="{\\lb}",_tag_dyneither(_tmp6B1,sizeof(char),6))),1);}else{
# 538
const char*_tmp6B2;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B2="{",_tag_dyneither(_tmp6B2,sizeof(char),2))));}}
# 540
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 542
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
# 543
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
# 544
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
# 545
if(Cyc_PP_tex_output){
# 546
const char*_tmp6B3;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6B3="{\\rb}",_tag_dyneither(_tmp6B3,sizeof(char),6))),1);}else{
# 547
const char*_tmp6B4;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B4="}",_tag_dyneither(_tmp6B4,sizeof(char),2))));}}
# 549
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 551
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
# 552
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
# 553
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
# 554
if(Cyc_PP_tex_output){
# 555
const char*_tmp6B5;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp6B5="\\$",_tag_dyneither(_tmp6B5,sizeof(char),3))),1);}else{
# 556
const char*_tmp6B6;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp6B6="$",_tag_dyneither(_tmp6B6,sizeof(char),2))));}}
# 558
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 560
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
# 561
struct Cyc_PP_Doc*_tmp6B7[3];return(_tmp6B7[2]=Cyc_Absynpp_rb(),((_tmp6B7[1]=Cyc_PP_seq(sep,ss),((_tmp6B7[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6B7,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 565
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
# 566
for(0;tms != 0;tms=tms->tl){
# 567
void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*_tmpD7;_LL8D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:
# 568
{const char*_tmp6BA;void*_tmp6B9;(_tmp6B9=0,Cyc_fprintf(Cyc_stderr,((_tmp6BA="Carray_mod ",_tag_dyneither(_tmp6BA,sizeof(char),12))),_tag_dyneither(_tmp6B9,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:
# 569
{const char*_tmp6BD;void*_tmp6BC;(_tmp6BC=0,Cyc_fprintf(Cyc_stderr,((_tmp6BD="ConstArray_mod ",_tag_dyneither(_tmp6BD,sizeof(char),16))),_tag_dyneither(_tmp6BC,sizeof(void*),0)));}goto _LL8C;_LL91: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD6=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;else{_tmpD7=_tmpD6->f1;}};}}_LL92:
# 571
{const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_fprintf(Cyc_stderr,((_tmp6C0="Function_mod(",_tag_dyneither(_tmp6C0,sizeof(char),14))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));}
# 572
for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
# 573
struct _dyneither_ptr*_tmpE2=(*((struct _tuple8*)_tmpD7->hd)).f1;
# 574
if(_tmpE2 == 0){const char*_tmp6C3;void*_tmp6C2;(_tmp6C2=0,Cyc_fprintf(Cyc_stderr,((_tmp6C3="?",_tag_dyneither(_tmp6C3,sizeof(char),2))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));}else{
# 575
void*_tmp6C4;(_tmp6C4=0,Cyc_fprintf(Cyc_stderr,*_tmpE2,_tag_dyneither(_tmp6C4,sizeof(void*),0)));}
# 576
if(_tmpD7->tl != 0){const char*_tmp6C7;void*_tmp6C6;(_tmp6C6=0,Cyc_fprintf(Cyc_stderr,((_tmp6C7=",",_tag_dyneither(_tmp6C7,sizeof(char),2))),_tag_dyneither(_tmp6C6,sizeof(void*),0)));}}
# 578
{const char*_tmp6CA;void*_tmp6C9;(_tmp6C9=0,Cyc_fprintf(Cyc_stderr,((_tmp6CA=") ",_tag_dyneither(_tmp6CA,sizeof(char),3))),_tag_dyneither(_tmp6C9,sizeof(void*),0)));}
# 579
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:
# 581
{const char*_tmp6CD;void*_tmp6CC;(_tmp6CC=0,Cyc_fprintf(Cyc_stderr,((_tmp6CD="Function_mod()",_tag_dyneither(_tmp6CD,sizeof(char),15))),_tag_dyneither(_tmp6CC,sizeof(void*),0)));}goto _LL8C;_LL95: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:
# 582
{const char*_tmp6D0;void*_tmp6CF;(_tmp6CF=0,Cyc_fprintf(Cyc_stderr,((_tmp6D0="Attributes_mod ",_tag_dyneither(_tmp6D0,sizeof(char),16))),_tag_dyneither(_tmp6CF,sizeof(void*),0)));}goto _LL8C;_LL97: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:
# 583
{const char*_tmp6D3;void*_tmp6D2;(_tmp6D2=0,Cyc_fprintf(Cyc_stderr,((_tmp6D3="TypeParams_mod ",_tag_dyneither(_tmp6D3,sizeof(char),16))),_tag_dyneither(_tmp6D2,sizeof(void*),0)));}goto _LL8C;_LL99: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:
# 584
{const char*_tmp6D6;void*_tmp6D5;(_tmp6D5=0,Cyc_fprintf(Cyc_stderr,((_tmp6D6="Pointer_mod ",_tag_dyneither(_tmp6D6,sizeof(char),13))),_tag_dyneither(_tmp6D5,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
# 586
const char*_tmp6D9;void*_tmp6D8;(_tmp6D8=0,Cyc_fprintf(Cyc_stderr,((_tmp6D9="\n",_tag_dyneither(_tmp6D9,sizeof(char),2))),_tag_dyneither(_tmp6D8,sizeof(void*),0)));};}
# 589
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 591
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
# 592
if(tms == 0)return d;{
# 593
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
# 594
const char*_tmp6DE;const char*_tmp6DD;struct Cyc_PP_Doc*_tmp6DC[3];struct Cyc_PP_Doc*p_rest=(_tmp6DC[2]=Cyc_PP_text(((_tmp6DD=")",_tag_dyneither(_tmp6DD,sizeof(char),2)))),((_tmp6DC[1]=rest,((_tmp6DC[0]=Cyc_PP_text(((_tmp6DE="(",_tag_dyneither(_tmp6DE,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6DC,sizeof(struct Cyc_PP_Doc*),3)))))));
# 595
void*_tmpF4=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFF;unsigned int _tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*_tmp104;union Cyc_Absyn_Constraint*_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpF5=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;else{_tmpF6=_tmpF5->f1;}}_LL9D:
# 597
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 598
const char*_tmp6E3;const char*_tmp6E2;struct Cyc_PP_Doc*_tmp6E1[2];return(_tmp6E1[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?
# 599
Cyc_PP_text(((_tmp6E2="[]@zeroterm",_tag_dyneither(_tmp6E2,sizeof(char),12)))): Cyc_PP_text(((_tmp6E3="[]",_tag_dyneither(_tmp6E3,sizeof(char),3)))),((_tmp6E1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E1,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpF7=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF4;if(_tmpF7->tag != 1)goto _LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}_LL9F:
# 601
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 602
const char*_tmp6EA;const char*_tmp6E9;const char*_tmp6E8;struct Cyc_PP_Doc*_tmp6E7[4];return(_tmp6E7[3]=
# 603
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((_tmp6E8="]@zeroterm",_tag_dyneither(_tmp6E8,sizeof(char),11)))): Cyc_PP_text(((_tmp6E9="]",_tag_dyneither(_tmp6E9,sizeof(char),2)))),((_tmp6E7[2]=
# 602
Cyc_Absynpp_exp2doc(_tmpF8),((_tmp6E7[1]=Cyc_PP_text(((_tmp6EA="[",_tag_dyneither(_tmp6EA,sizeof(char),2)))),((_tmp6E7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E7,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1:
# 605
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 606
void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;struct Cyc_Absyn_VarargInfo*_tmp114;void*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_List_List*_tmp118;unsigned int _tmp119;_LLA9: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp111=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp110;if(_tmp111->tag != 1)goto _LLAB;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;_tmp114=_tmp111->f3;_tmp115=(void*)_tmp111->f4;_tmp116=_tmp111->f5;}}_LLAA: {
# 608
struct Cyc_PP_Doc*_tmp6EB[2];return(_tmp6EB[1]=Cyc_Absynpp_funargs2doc(_tmp112,_tmp113,_tmp114,_tmp115,_tmp116),((_tmp6EB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EB,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp117=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {
# 610
const char*_tmp6F2;const char*_tmp6F1;const char*_tmp6F0;struct Cyc_PP_Doc*_tmp6EF[2];return(_tmp6EF[1]=Cyc_PP_group(((_tmp6F2="(",_tag_dyneither(_tmp6F2,sizeof(char),2))),((_tmp6F1=")",_tag_dyneither(_tmp6F1,sizeof(char),2))),((_tmp6F0=",",_tag_dyneither(_tmp6F0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((_tmp6EF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6EF,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3:
# 613
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLAD:
# 615
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
# 616
struct Cyc_PP_Doc*_tmp6F3[2];return(_tmp6F3[1]=Cyc_Absynpp_atts2doc(_tmpFD),((_tmp6F3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F3,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLAE:
# 618
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
# 619
struct Cyc_PP_Doc*_tmp6F4[2];return(_tmp6F4[1]=rest,((_tmp6F4[0]=Cyc_Absynpp_callconv2doc(_tmpFD),Cyc_PP_cat(_tag_dyneither(_tmp6F4,sizeof(struct Cyc_PP_Doc*),2)))));}
# 620
return rest;}_LLA4: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;_tmp101=_tmpFE->f3;}}_LLA5:
# 623
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
# 624
if(_tmp101){
# 625
struct Cyc_PP_Doc*_tmp6F5[2];return(_tmp6F5[1]=Cyc_Absynpp_ktvars2doc(_tmpFF),((_tmp6F5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F5,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 627
struct Cyc_PP_Doc*_tmp6F6[2];return(_tmp6F6[1]=Cyc_Absynpp_tvars2doc(_tmpFF),((_tmp6F6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6F6,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;_tmp108=_tmp102->f2;}}_LLA7: {
# 631
struct Cyc_PP_Doc*ptr;
# 632
{void*_tmp123=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp124=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp123;if(_tmp124->tag != 0)goto _LLB3;}_LLB2:
# 633
 ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp125=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;else{_tmp126=_tmp125->f1;}}_LLB4:
# 635
{const char*_tmp6F8;const char*_tmp6F7;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(_tmp6F8="*",_tag_dyneither(_tmp6F8,sizeof(char),2)):((_tmp6F7="@",_tag_dyneither(_tmp6F7,sizeof(char),2))));}{
# 636
unsigned int _tmp12A;int _tmp12B;struct _tuple11 _tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;
# 637
if(!_tmp12B  || _tmp12A != 1){
# 638
struct Cyc_PP_Doc*_tmp6F9[4];ptr=((_tmp6F9[3]=Cyc_Absynpp_rb(),((_tmp6F9[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp6F9[1]=Cyc_Absynpp_lb(),((_tmp6F9[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6F9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 639
goto _LLB0;};_LLB0:;}
# 641
if(Cyc_Absynpp_print_zeroterm){
# 642
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
# 643
const char*_tmp6FC;struct Cyc_PP_Doc*_tmp6FB[2];ptr=((_tmp6FB[1]=Cyc_PP_text(((_tmp6FC="@zeroterm ",_tag_dyneither(_tmp6FC,sizeof(char),11)))),((_tmp6FB[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6FB,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 644
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
# 645
const char*_tmp6FF;struct Cyc_PP_Doc*_tmp6FE[2];ptr=((_tmp6FE[1]=Cyc_PP_text(((_tmp6FF="@nozeroterm ",_tag_dyneither(_tmp6FF,sizeof(char),13)))),((_tmp6FE[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6FE,sizeof(struct Cyc_PP_Doc*),2))))));}}}
# 647
{void*_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp132=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp131;if(_tmp132->tag != 20)goto _LLB8;}_LLB7:
# 648
 goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp133=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLBA;_LLB9:
# 649
 goto _LLB5;_LLBA:;_LLBB: {
# 650
const char*_tmp702;struct Cyc_PP_Doc*_tmp701[3];ptr=((_tmp701[2]=Cyc_PP_text(((_tmp702=" ",_tag_dyneither(_tmp702,sizeof(char),2)))),((_tmp701[1]=Cyc_Absynpp_rgn2doc(_tmp104),((_tmp701[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp701,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}
# 652
{struct Cyc_PP_Doc*_tmp703[2];ptr=((_tmp703[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp703[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp703,sizeof(struct Cyc_PP_Doc*),2))))));}{
# 653
struct Cyc_PP_Doc*_tmp704[2];return(_tmp704[1]=rest,((_tmp704[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp704,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}
# 657
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
# 658
void*_tmp13B=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp13C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp13B;if(_tmp13C->tag != 20)goto _LLBF;}_LLBE: {
# 659
const char*_tmp705;return Cyc_PP_text(((_tmp705="`H",_tag_dyneither(_tmp705,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp13B;if(_tmp13D->tag != 21)goto _LLC1;}_LLC0: {
# 660
const char*_tmp706;return Cyc_PP_text(((_tmp706="`U",_tag_dyneither(_tmp706,sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp13E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp13B;if(_tmp13E->tag != 22)goto _LLC3;}_LLC2: {
# 661
const char*_tmp707;return Cyc_PP_text(((_tmp707="`RC",_tag_dyneither(_tmp707,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13B;if(_tmp13F->tag != 2)goto _LLC5;}_LLC4:
# 662
 return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {
# 663
const char*_tmp70C;const char*_tmp70B;struct Cyc_PP_Doc*_tmp70A[3];return(_tmp70A[2]=Cyc_PP_text(((_tmp70B=")",_tag_dyneither(_tmp70B,sizeof(char),2)))),((_tmp70A[1]=Cyc_Absynpp_ntyp2doc(t),((_tmp70A[0]=Cyc_PP_text(((_tmp70C="@region(",_tag_dyneither(_tmp70C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp70A,sizeof(struct Cyc_PP_Doc*),3)))))));}_LLBC:;}
# 667
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 671
void*_tmp146=Cyc_Tcutil_compress(t);void*_tmp148;struct Cyc_List_List*_tmp14A;_LLC8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp146;if(_tmp147->tag != 23)goto _LLCA;else{_tmp148=(void*)_tmp147->f1;}}_LLC9:
# 672
{struct Cyc_List_List*_tmp70D;*rgions=((_tmp70D=_cycalloc(sizeof(*_tmp70D)),((_tmp70D->hd=Cyc_Absynpp_rgn2doc(_tmp148),((_tmp70D->tl=*rgions,_tmp70D))))));}goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp149=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp146;if(_tmp149->tag != 24)goto _LLCC;else{_tmp14A=_tmp149->f1;}}_LLCB:
# 674
 for(0;_tmp14A != 0;_tmp14A=_tmp14A->tl){
# 675
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp14A->hd);}
# 677
goto _LLC7;_LLCC:;_LLCD:
# 678
{struct Cyc_List_List*_tmp70E;*effects=((_tmp70E=_cycalloc(sizeof(*_tmp70E)),((_tmp70E->hd=Cyc_Absynpp_typ2doc(t),((_tmp70E->tl=*effects,_tmp70E))))));}goto _LLC7;_LLC7:;}
# 682
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
# 683
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
# 684
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
# 685
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
# 686
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
# 687
if(rgions == 0  && effects != 0){
# 688
const char*_tmp711;const char*_tmp710;const char*_tmp70F;return Cyc_PP_group(((_tmp70F="",_tag_dyneither(_tmp70F,sizeof(char),1))),((_tmp710="",_tag_dyneither(_tmp710,sizeof(char),1))),((_tmp711="+",_tag_dyneither(_tmp711,sizeof(char),2))),effects);}else{
# 690
const char*_tmp712;struct Cyc_PP_Doc*_tmp150=Cyc_Absynpp_group_braces(((_tmp712=",",_tag_dyneither(_tmp712,sizeof(char),2))),rgions);
# 691
struct Cyc_List_List*_tmp716;const char*_tmp715;const char*_tmp714;const char*_tmp713;return Cyc_PP_group(((_tmp713="",_tag_dyneither(_tmp713,sizeof(char),1))),((_tmp714="",_tag_dyneither(_tmp714,sizeof(char),1))),((_tmp715="+",_tag_dyneither(_tmp715,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp716=_cycalloc(sizeof(*_tmp716)),((_tmp716->hd=_tmp150,((_tmp716->tl=0,_tmp716))))))));}}
# 695
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
# 696
switch(k){case Cyc_Absyn_StructA: _LLCE: {
# 697
const char*_tmp717;return Cyc_PP_text(((_tmp717="struct ",_tag_dyneither(_tmp717,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCF: {
# 698
const char*_tmp718;return Cyc_PP_text(((_tmp718="union ",_tag_dyneither(_tmp718,sizeof(char),7))));}}}
# 703
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
# 704
struct Cyc_PP_Doc*s;
# 705
{void*_tmp158=t;struct Cyc_Core_Opt*_tmp15E;void*_tmp15F;int _tmp160;struct Cyc_Core_Opt*_tmp161;struct Cyc_Absyn_Tvar*_tmp163;struct Cyc_Absyn_DatatypeInfo _tmp165;union Cyc_Absyn_DatatypeInfoU _tmp166;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_DatatypeFieldInfo _tmp169;union Cyc_Absyn_DatatypeFieldInfoU _tmp16A;struct Cyc_List_List*_tmp16B;enum Cyc_Absyn_Sign _tmp16D;enum Cyc_Absyn_Size_of _tmp16E;int _tmp170;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_AggrInfo _tmp174;union Cyc_Absyn_AggrInfoU _tmp175;struct Cyc_List_List*_tmp176;enum Cyc_Absyn_AggrKind _tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17B;struct _tuple0*_tmp17D;struct Cyc_Absyn_Exp*_tmp17F;struct _tuple0*_tmp181;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Typedefdecl*_tmp183;void*_tmp185;void*_tmp187;void*_tmp188;void*_tmp18A;void*_tmp18F;struct Cyc_Absyn_TypeDecl*_tmp193;struct Cyc_Absyn_TypeDecl _tmp194;void*_tmp195;struct Cyc_Absyn_Aggrdecl*_tmp197;struct Cyc_Absyn_TypeDecl*_tmp199;struct Cyc_Absyn_TypeDecl _tmp19A;void*_tmp19B;struct Cyc_Absyn_Enumdecl*_tmp19D;struct Cyc_Absyn_TypeDecl*_tmp19F;struct Cyc_Absyn_TypeDecl _tmp1A0;void*_tmp1A1;struct Cyc_Absyn_Datatypedecl*_tmp1A3;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158;if(_tmp159->tag != 8)goto _LLD4;}_LLD3: {
# 707
const char*_tmp719;return Cyc_PP_text(((_tmp719="[[[array]]]",_tag_dyneither(_tmp719,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp158;if(_tmp15A->tag != 9)goto _LLD6;}_LLD5:
# 708
 return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp158;if(_tmp15B->tag != 5)goto _LLD8;}_LLD7:
# 709
 return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp158;if(_tmp15C->tag != 0)goto _LLDA;}_LLD9:
# 711
{const char*_tmp71A;s=Cyc_PP_text(((_tmp71A="void",_tag_dyneither(_tmp71A,sizeof(char),5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp158;if(_tmp15D->tag != 1)goto _LLDC;else{_tmp15E=_tmp15D->f1;_tmp15F=(void*)_tmp15D->f2;_tmp160=_tmp15D->f3;_tmp161=_tmp15D->f4;}}_LLDB:
# 713
 if(_tmp15F != 0)
# 715
return Cyc_Absynpp_ntyp2doc((void*)_tmp15F);else{
# 723
const char*_tmp72A;const char*_tmp729;const char*_tmp728;struct Cyc_Int_pa_PrintArg_struct _tmp727;void*_tmp726[1];const char*_tmp725;const char*_tmp724;struct Cyc_PP_Doc*_tmp723[6];s=((_tmp723[5]=
# 728
_tmp15E == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15E->v),((_tmp723[4]=Cyc_PP_text(((_tmp724=")::",_tag_dyneither(_tmp724,sizeof(char),4)))),((_tmp723[3]=
# 726
(!Cyc_Absynpp_print_full_evars  || _tmp161 == 0)?
# 727
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp161->v),((_tmp723[2]=
# 725
Cyc_PP_text((struct _dyneither_ptr)((_tmp727.tag=1,((_tmp727.f1=(unsigned long)_tmp160,((_tmp726[0]=& _tmp727,Cyc_aprintf(((_tmp725="%d",_tag_dyneither(_tmp725,sizeof(char),3))),_tag_dyneither(_tmp726,sizeof(void*),1))))))))),((_tmp723[1]=
# 724
Cyc_PP_text(((_tmp728="(",_tag_dyneither(_tmp728,sizeof(char),2)))),((_tmp723[0]=
# 723
Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp729="\\%",_tag_dyneither(_tmp729,sizeof(char),3)))): Cyc_PP_text(((_tmp72A="%",_tag_dyneither(_tmp72A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp723,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 731
goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp158;if(_tmp162->tag != 2)goto _LLDE;else{_tmp163=_tmp162->f1;}}_LLDD:
# 733
 s=Cyc_PP_textptr(_tmp163->name);
# 734
if(Cyc_Absynpp_print_all_kinds){
# 735
const char*_tmp72D;struct Cyc_PP_Doc*_tmp72C[3];s=((_tmp72C[2]=Cyc_Absynpp_kindbound2doc(_tmp163->kind),((_tmp72C[1]=Cyc_PP_text(((_tmp72D="::",_tag_dyneither(_tmp72D,sizeof(char),3)))),((_tmp72C[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp72C,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 739
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp163)){
# 742
const char*_tmp732;const char*_tmp731;struct Cyc_PP_Doc*_tmp730[3];s=((_tmp730[2]=Cyc_PP_text(((_tmp731=" */",_tag_dyneither(_tmp731,sizeof(char),4)))),((_tmp730[1]=s,((_tmp730[0]=Cyc_PP_text(((_tmp732="_ /* ",_tag_dyneither(_tmp732,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 745
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp158;if(_tmp164->tag != 3)goto _LLE0;else{_tmp165=_tmp164->f1;_tmp166=_tmp165.datatype_info;_tmp167=_tmp165.targs;}}_LLDF:
# 747
{union Cyc_Absyn_DatatypeInfoU _tmp1B3=_tmp166;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1B4;struct _tuple0*_tmp1B5;int _tmp1B6;struct Cyc_Absyn_Datatypedecl**_tmp1B7;struct Cyc_Absyn_Datatypedecl*_tmp1B8;struct Cyc_Absyn_Datatypedecl _tmp1B9;struct _tuple0*_tmp1BA;int _tmp1BB;_LL10D: if((_tmp1B3.UnknownDatatype).tag != 1)goto _LL10F;_tmp1B4=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B3.UnknownDatatype).val;_tmp1B5=_tmp1B4.name;_tmp1B6=_tmp1B4.is_extensible;_LL10E:
# 749
 _tmp1BA=_tmp1B5;_tmp1BB=_tmp1B6;goto _LL110;_LL10F: if((_tmp1B3.KnownDatatype).tag != 2)goto _LL10C;_tmp1B7=(struct Cyc_Absyn_Datatypedecl**)(_tmp1B3.KnownDatatype).val;_tmp1B8=*_tmp1B7;_tmp1B9=*_tmp1B8;_tmp1BA=_tmp1B9.name;_tmp1BB=_tmp1B9.is_extensible;_LL110: {
# 751
const char*_tmp733;struct Cyc_PP_Doc*_tmp1BC=Cyc_PP_text(((_tmp733="datatype ",_tag_dyneither(_tmp733,sizeof(char),10))));
# 752
const char*_tmp734;struct Cyc_PP_Doc*_tmp1BD=_tmp1BB?Cyc_PP_text(((_tmp734="@extensible ",_tag_dyneither(_tmp734,sizeof(char),13)))): Cyc_PP_nil_doc();
# 753
{struct Cyc_PP_Doc*_tmp735[4];s=((_tmp735[3]=Cyc_Absynpp_tps2doc(_tmp167),((_tmp735[2]=Cyc_Absynpp_qvar2doc(_tmp1BA),((_tmp735[1]=_tmp1BC,((_tmp735[0]=_tmp1BD,Cyc_PP_cat(_tag_dyneither(_tmp735,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 754
goto _LL10C;}_LL10C:;}
# 756
goto _LLD1;_LLE0: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp158;if(_tmp168->tag != 4)goto _LLE2;else{_tmp169=_tmp168->f1;_tmp16A=_tmp169.field_info;_tmp16B=_tmp169.targs;}}_LLE1:
# 758
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1C1=_tmp16A;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1C2;struct _tuple0*_tmp1C3;struct _tuple0*_tmp1C4;int _tmp1C5;struct _tuple1 _tmp1C6;struct Cyc_Absyn_Datatypedecl*_tmp1C7;struct Cyc_Absyn_Datatypedecl _tmp1C8;struct _tuple0*_tmp1C9;int _tmp1CA;struct Cyc_Absyn_Datatypefield*_tmp1CB;struct Cyc_Absyn_Datatypefield _tmp1CC;struct _tuple0*_tmp1CD;_LL112: if((_tmp1C1.UnknownDatatypefield).tag != 1)goto _LL114;_tmp1C2=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1C1.UnknownDatatypefield).val;_tmp1C3=_tmp1C2.datatype_name;_tmp1C4=_tmp1C2.field_name;_tmp1C5=_tmp1C2.is_extensible;_LL113:
# 760
 _tmp1C9=_tmp1C3;_tmp1CA=_tmp1C5;_tmp1CD=_tmp1C4;goto _LL115;_LL114: if((_tmp1C1.KnownDatatypefield).tag != 2)goto _LL111;_tmp1C6=(struct _tuple1)(_tmp1C1.KnownDatatypefield).val;_tmp1C7=_tmp1C6.f1;_tmp1C8=*_tmp1C7;_tmp1C9=_tmp1C8.name;_tmp1CA=_tmp1C8.is_extensible;_tmp1CB=_tmp1C6.f2;_tmp1CC=*_tmp1CB;_tmp1CD=_tmp1CC.name;_LL115: {
# 763
const char*_tmp737;const char*_tmp736;struct Cyc_PP_Doc*_tmp1CE=Cyc_PP_text(_tmp1CA?(_tmp737="@extensible datatype ",_tag_dyneither(_tmp737,sizeof(char),22)):((_tmp736="datatype ",_tag_dyneither(_tmp736,sizeof(char),10))));
# 764
{const char*_tmp73A;struct Cyc_PP_Doc*_tmp739[4];s=((_tmp739[3]=Cyc_Absynpp_qvar2doc(_tmp1CD),((_tmp739[2]=Cyc_PP_text(((_tmp73A=".",_tag_dyneither(_tmp73A,sizeof(char),2)))),((_tmp739[1]=Cyc_Absynpp_qvar2doc(_tmp1C9),((_tmp739[0]=_tmp1CE,Cyc_PP_cat(_tag_dyneither(_tmp739,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 765
goto _LL111;}_LL111:;}
# 767
goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp158;if(_tmp16C->tag != 6)goto _LLE4;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LLE3: {
# 769
struct _dyneither_ptr sns;
# 770
struct _dyneither_ptr ts;
# 771
switch(_tmp16D){case Cyc_Absyn_None: _LL116:
# 772
 goto _LL117;case Cyc_Absyn_Signed: _LL117:
# 773
{const char*_tmp73B;sns=((_tmp73B="",_tag_dyneither(_tmp73B,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:
# 774
{const char*_tmp73C;sns=((_tmp73C="unsigned ",_tag_dyneither(_tmp73C,sizeof(char),10)));}break;}
# 776
switch(_tmp16E){case Cyc_Absyn_Char_sz: _LL11A:
# 778
 switch(_tmp16D){case Cyc_Absyn_None: _LL11C:
# 779
{const char*_tmp73D;sns=((_tmp73D="",_tag_dyneither(_tmp73D,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL11D:
# 780
{const char*_tmp73E;sns=((_tmp73E="signed ",_tag_dyneither(_tmp73E,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:
# 781
{const char*_tmp73F;sns=((_tmp73F="unsigned ",_tag_dyneither(_tmp73F,sizeof(char),10)));}break;}
# 783
{const char*_tmp740;ts=((_tmp740="char",_tag_dyneither(_tmp740,sizeof(char),5)));}
# 784
break;case Cyc_Absyn_Short_sz: _LL11B:
# 785
{const char*_tmp741;ts=((_tmp741="short",_tag_dyneither(_tmp741,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL120:
# 786
{const char*_tmp742;ts=((_tmp742="int",_tag_dyneither(_tmp742,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL121:
# 787
{const char*_tmp743;ts=((_tmp743="long",_tag_dyneither(_tmp743,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
# 789
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:
# 790
{const char*_tmp744;ts=((_tmp744="long long",_tag_dyneither(_tmp744,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL125:
# 791
{const char*_tmp745;ts=((_tmp745="__int64",_tag_dyneither(_tmp745,sizeof(char),8)));}break;}
# 793
break;}
# 795
{const char*_tmp74A;void*_tmp749[2];struct Cyc_String_pa_PrintArg_struct _tmp748;struct Cyc_String_pa_PrintArg_struct _tmp747;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp747.tag=0,((_tmp747.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp748.tag=0,((_tmp748.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp749[0]=& _tmp748,((_tmp749[1]=& _tmp747,Cyc_aprintf(((_tmp74A="%s%s",_tag_dyneither(_tmp74A,sizeof(char),5))),_tag_dyneither(_tmp749,sizeof(void*),2)))))))))))))));}
# 796
goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp158;if(_tmp16F->tag != 7)goto _LLE6;else{_tmp170=_tmp16F->f1;}}_LLE5:
# 798
 if(_tmp170 == 0){
# 799
const char*_tmp74B;s=Cyc_PP_text(((_tmp74B="float",_tag_dyneither(_tmp74B,sizeof(char),6))));}else{
# 800
if(_tmp170 == 1){
# 801
const char*_tmp74C;s=Cyc_PP_text(((_tmp74C="double",_tag_dyneither(_tmp74C,sizeof(char),7))));}else{
# 803
const char*_tmp74D;s=Cyc_PP_text(((_tmp74D="long double",_tag_dyneither(_tmp74D,sizeof(char),12))));}}
# 804
goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp171=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp158;if(_tmp171->tag != 10)goto _LLE8;else{_tmp172=_tmp171->f1;}}_LLE7:
# 806
{struct Cyc_PP_Doc*_tmp74E[2];s=((_tmp74E[1]=Cyc_Absynpp_args2doc(_tmp172),((_tmp74E[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp74E,sizeof(struct Cyc_PP_Doc*),2))))));}
# 807
goto _LLD1;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp158;if(_tmp173->tag != 11)goto _LLEA;else{_tmp174=_tmp173->f1;_tmp175=_tmp174.aggr_info;_tmp176=_tmp174.targs;}}_LLE9: {
# 809
enum Cyc_Absyn_AggrKind _tmp1E7;struct _tuple0*_tmp1E8;struct _tuple10 _tmp1E6=Cyc_Absyn_aggr_kinded_name(_tmp175);_tmp1E7=_tmp1E6.f1;_tmp1E8=_tmp1E6.f2;
# 810
{struct Cyc_PP_Doc*_tmp74F[3];s=((_tmp74F[2]=Cyc_Absynpp_tps2doc(_tmp176),((_tmp74F[1]=Cyc_Absynpp_qvar2doc(_tmp1E8),((_tmp74F[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1E7),Cyc_PP_cat(_tag_dyneither(_tmp74F,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 811
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp158;if(_tmp177->tag != 12)goto _LLEC;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;}}_LLEB:
# 813
{struct Cyc_PP_Doc*_tmp750[4];s=((_tmp750[3]=
# 815
Cyc_Absynpp_rb(),((_tmp750[2]=
# 814
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp179)),((_tmp750[1]=
# 813
Cyc_Absynpp_lb(),((_tmp750[0]=Cyc_Absynpp_aggr_kind2doc(_tmp178),Cyc_PP_cat(_tag_dyneither(_tmp750,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 816
goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp158;if(_tmp17A->tag != 14)goto _LLEE;else{_tmp17B=_tmp17A->f1;}}_LLED:
# 818
{const char*_tmp753;struct Cyc_PP_Doc*_tmp752[4];s=((_tmp752[3]=Cyc_Absynpp_rb(),((_tmp752[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp17B)),((_tmp752[1]=Cyc_Absynpp_lb(),((_tmp752[0]=Cyc_PP_text(((_tmp753="enum ",_tag_dyneither(_tmp753,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp752,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp158;if(_tmp17C->tag != 13)goto _LLF0;else{_tmp17D=_tmp17C->f1;}}_LLEF:
# 820
{const char*_tmp756;struct Cyc_PP_Doc*_tmp755[2];s=((_tmp755[1]=Cyc_Absynpp_qvar2doc(_tmp17D),((_tmp755[0]=Cyc_PP_text(((_tmp756="enum ",_tag_dyneither(_tmp756,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp755,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp158;if(_tmp17E->tag != 18)goto _LLF2;else{_tmp17F=_tmp17E->f1;}}_LLF1:
# 822
{const char*_tmp75B;const char*_tmp75A;struct Cyc_PP_Doc*_tmp759[3];s=((_tmp759[2]=Cyc_PP_text(((_tmp75A=")",_tag_dyneither(_tmp75A,sizeof(char),2)))),((_tmp759[1]=Cyc_Absynpp_exp2doc(_tmp17F),((_tmp759[0]=Cyc_PP_text(((_tmp75B="valueof_t(",_tag_dyneither(_tmp75B,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp759,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp158;if(_tmp180->tag != 17)goto _LLF4;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;_tmp183=_tmp180->f3;}}_LLF3:
# 829
{struct Cyc_PP_Doc*_tmp75C[2];s=((_tmp75C[1]=Cyc_Absynpp_tps2doc(_tmp182),((_tmp75C[0]=Cyc_Absynpp_qvar2doc(_tmp181),Cyc_PP_cat(_tag_dyneither(_tmp75C,sizeof(struct Cyc_PP_Doc*),2))))));}
# 830
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp158;if(_tmp184->tag != 15)goto _LLF6;else{_tmp185=(void*)_tmp184->f1;}}_LLF5:
# 832
{const char*_tmp761;const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[3];s=((_tmp75F[2]=Cyc_PP_text(((_tmp760=">",_tag_dyneither(_tmp760,sizeof(char),2)))),((_tmp75F[1]=Cyc_Absynpp_rgn2doc(_tmp185),((_tmp75F[0]=Cyc_PP_text(((_tmp761="region_t<",_tag_dyneither(_tmp761,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp158;if(_tmp186->tag != 16)goto _LLF8;else{_tmp187=(void*)_tmp186->f1;_tmp188=(void*)_tmp186->f2;}}_LLF7:
# 834
{const char*_tmp768;const char*_tmp767;const char*_tmp766;struct Cyc_PP_Doc*_tmp765[5];s=((_tmp765[4]=
# 835
Cyc_PP_text(((_tmp766=">",_tag_dyneither(_tmp766,sizeof(char),2)))),((_tmp765[3]=
# 834
Cyc_Absynpp_rgn2doc(_tmp188),((_tmp765[2]=Cyc_PP_text(((_tmp767=",",_tag_dyneither(_tmp767,sizeof(char),2)))),((_tmp765[1]=Cyc_Absynpp_rgn2doc(_tmp187),((_tmp765[0]=Cyc_PP_text(((_tmp768="dynregion_t<",_tag_dyneither(_tmp768,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp765,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 835
goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp158;if(_tmp189->tag != 19)goto _LLFA;else{_tmp18A=(void*)_tmp189->f1;}}_LLF9:
# 837
{const char*_tmp76D;const char*_tmp76C;struct Cyc_PP_Doc*_tmp76B[3];s=((_tmp76B[2]=Cyc_PP_text(((_tmp76C=">",_tag_dyneither(_tmp76C,sizeof(char),2)))),((_tmp76B[1]=Cyc_Absynpp_typ2doc(_tmp18A),((_tmp76B[0]=Cyc_PP_text(((_tmp76D="tag_t<",_tag_dyneither(_tmp76D,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp76B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp158;if(_tmp18B->tag != 21)goto _LLFC;}_LLFB:
# 838
 goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp158;if(_tmp18C->tag != 20)goto _LLFE;}_LLFD:
# 839
 goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp158;if(_tmp18D->tag != 22)goto _LL100;}_LLFF:
# 841
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp18E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp158;if(_tmp18E->tag != 25)goto _LL102;else{_tmp18F=(void*)_tmp18E->f1;}}_LL101:
# 843
{const char*_tmp772;const char*_tmp771;struct Cyc_PP_Doc*_tmp770[3];s=((_tmp770[2]=Cyc_PP_text(((_tmp771=")",_tag_dyneither(_tmp771,sizeof(char),2)))),((_tmp770[1]=Cyc_Absynpp_typ2doc(_tmp18F),((_tmp770[0]=Cyc_PP_text(((_tmp772="regions(",_tag_dyneither(_tmp772,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp770,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp158;if(_tmp190->tag != 23)goto _LL104;}_LL103:
# 844
 goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp191=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp158;if(_tmp191->tag != 24)goto _LL106;}_LL105:
# 846
 s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LL106: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp192=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp158;if(_tmp192->tag != 26)goto _LL108;else{_tmp193=_tmp192->f1;_tmp194=*_tmp193;_tmp195=_tmp194.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp196=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp195;if(_tmp196->tag != 0)goto _LL108;else{_tmp197=_tmp196->f1;}};}}_LL107:
# 848
 s=Cyc_Absynpp_aggrdecl2doc(_tmp197);goto _LLD1;_LL108: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp158;if(_tmp198->tag != 26)goto _LL10A;else{_tmp199=_tmp198->f1;_tmp19A=*_tmp199;_tmp19B=_tmp19A.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp19C=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp19B;if(_tmp19C->tag != 1)goto _LL10A;else{_tmp19D=_tmp19C->f1;}};}}_LL109:
# 850
 s=Cyc_Absynpp_enumdecl2doc(_tmp19D);goto _LLD1;_LL10A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp158;if(_tmp19E->tag != 26)goto _LLD1;else{_tmp19F=_tmp19E->f1;_tmp1A0=*_tmp19F;_tmp1A1=_tmp1A0.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1A2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1A1;if(_tmp1A2->tag != 2)goto _LLD1;else{_tmp1A3=_tmp1A2->f1;}};}}_LL10B:
# 852
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1A3);goto _LLD1;_LLD1:;}
# 854
return s;}
# 857
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
# 858
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo));}struct _tuple16{void*f1;void*f2;};
# 861
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
# 862
struct _tuple16 _tmp201;void*_tmp202;void*_tmp203;struct _tuple16*_tmp200=cmp;_tmp201=*_tmp200;_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;{
# 863
const char*_tmp775;struct Cyc_PP_Doc*_tmp774[3];return(_tmp774[2]=Cyc_Absynpp_rgn2doc(_tmp203),((_tmp774[1]=Cyc_PP_text(((_tmp775=" > ",_tag_dyneither(_tmp775,sizeof(char),4)))),((_tmp774[0]=Cyc_Absynpp_rgn2doc(_tmp202),Cyc_PP_cat(_tag_dyneither(_tmp774,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 866
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
# 867
const char*_tmp778;const char*_tmp777;const char*_tmp776;return Cyc_PP_group(((_tmp776="",_tag_dyneither(_tmp776,sizeof(char),1))),((_tmp777="",_tag_dyneither(_tmp777,sizeof(char),1))),((_tmp778=",",_tag_dyneither(_tmp778,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 870
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
# 871
struct _dyneither_ptr*_tmp209=(*t).f1;
# 872
struct Cyc_Core_Opt*_tmp779;struct Cyc_Core_Opt*dopt=_tmp209 == 0?0:((_tmp779=_cycalloc(sizeof(*_tmp779)),((_tmp779->v=Cyc_PP_text(*_tmp209),_tmp779))));
# 873
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 876
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 879
struct Cyc_List_List*_tmp20B=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
# 880
struct Cyc_PP_Doc*eff_doc;
# 881
if(c_varargs){
# 882
const char*_tmp77C;struct Cyc_List_List*_tmp77B;_tmp20B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20B,((_tmp77B=_cycalloc(sizeof(*_tmp77B)),((_tmp77B->hd=Cyc_PP_text(((_tmp77C="...",_tag_dyneither(_tmp77C,sizeof(char),4)))),((_tmp77B->tl=0,_tmp77B)))))));}else{
# 883
if(cyc_varargs != 0){
# 884
const char*_tmp785;const char*_tmp784;const char*_tmp783;struct _tuple8*_tmp782;struct Cyc_PP_Doc*_tmp781[3];struct Cyc_PP_Doc*_tmp20E=(_tmp781[2]=
# 886
Cyc_Absynpp_funarg2doc(((_tmp782=_cycalloc(sizeof(*_tmp782)),((_tmp782->f1=cyc_varargs->name,((_tmp782->f2=cyc_varargs->tq,((_tmp782->f3=cyc_varargs->type,_tmp782))))))))),((_tmp781[1]=
# 885
cyc_varargs->inject?Cyc_PP_text(((_tmp783=" inject ",_tag_dyneither(_tmp783,sizeof(char),9)))): Cyc_PP_text(((_tmp784=" ",_tag_dyneither(_tmp784,sizeof(char),2)))),((_tmp781[0]=
# 884
Cyc_PP_text(((_tmp785="...",_tag_dyneither(_tmp785,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),3)))))));
# 888
struct Cyc_List_List*_tmp786;_tmp20B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20B,((_tmp786=_cycalloc(sizeof(*_tmp786)),((_tmp786->hd=_tmp20E,((_tmp786->tl=0,_tmp786)))))));}}{
# 890
const char*_tmp789;const char*_tmp788;const char*_tmp787;struct Cyc_PP_Doc*_tmp215=Cyc_PP_group(((_tmp787="",_tag_dyneither(_tmp787,sizeof(char),1))),((_tmp788="",_tag_dyneither(_tmp788,sizeof(char),1))),((_tmp789=",",_tag_dyneither(_tmp789,sizeof(char),2))),_tmp20B);
# 891
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
# 892
const char*_tmp78C;struct Cyc_PP_Doc*_tmp78B[3];_tmp215=((_tmp78B[2]=Cyc_Absynpp_eff2doc((void*)effopt),((_tmp78B[1]=Cyc_PP_text(((_tmp78C=";",_tag_dyneither(_tmp78C,sizeof(char),2)))),((_tmp78B[0]=_tmp215,Cyc_PP_cat(_tag_dyneither(_tmp78B,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 893
if(rgn_po != 0){
# 894
const char*_tmp78F;struct Cyc_PP_Doc*_tmp78E[3];_tmp215=((_tmp78E[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp78E[1]=Cyc_PP_text(((_tmp78F=":",_tag_dyneither(_tmp78F,sizeof(char),2)))),((_tmp78E[0]=_tmp215,Cyc_PP_cat(_tag_dyneither(_tmp78E,sizeof(struct Cyc_PP_Doc*),3))))))));}{
# 895
const char*_tmp794;const char*_tmp793;struct Cyc_PP_Doc*_tmp792[3];return(_tmp792[2]=Cyc_PP_text(((_tmp793=")",_tag_dyneither(_tmp793,sizeof(char),2)))),((_tmp792[1]=_tmp215,((_tmp792[0]=Cyc_PP_text(((_tmp794="(",_tag_dyneither(_tmp794,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp792,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 898
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
# 899
struct _tuple8 _tmp221;struct _dyneither_ptr*_tmp222;struct Cyc_Absyn_Tqual _tmp223;void*_tmp224;struct _tuple8*_tmp220=arg;_tmp221=*_tmp220;_tmp222=_tmp221.f1;_tmp223=_tmp221.f2;_tmp224=_tmp221.f3;{
# 900
struct _tuple8*_tmp795;return(_tmp795=_cycalloc(sizeof(*_tmp795)),((_tmp795->f1=(struct _dyneither_ptr*)_tmp222,((_tmp795->f2=_tmp223,((_tmp795->f3=_tmp224,_tmp795)))))));};}
# 903
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 905
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
# 906
struct Cyc_List_List*_tmp226=0;
# 907
int match;
# 908
{union Cyc_Absyn_Nmspace _tmp227=(*q).f1;int _tmp228;struct Cyc_List_List*_tmp229;struct Cyc_List_List*_tmp22A;struct Cyc_List_List*_tmp22B;_LL128: if((_tmp227.Loc_n).tag != 4)goto _LL12A;_tmp228=(int)(_tmp227.Loc_n).val;_LL129:
# 910
 _tmp229=0;goto _LL12B;_LL12A: if((_tmp227.Rel_n).tag != 1)goto _LL12C;_tmp229=(struct Cyc_List_List*)(_tmp227.Rel_n).val;_LL12B:
# 912
 match=0;
# 913
_tmp226=_tmp229;
# 914
goto _LL127;_LL12C: if((_tmp227.C_n).tag != 3)goto _LL12E;_tmp22A=(struct Cyc_List_List*)(_tmp227.C_n).val;_LL12D:
# 916
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22A,Cyc_Absynpp_curr_namespace);
# 918
goto _LL127;_LL12E: if((_tmp227.Abs_n).tag != 2)goto _LL127;_tmp22B=(struct Cyc_List_List*)(_tmp227.Abs_n).val;_LL12F:
# 920
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp22B,Cyc_Absynpp_curr_namespace);
# 921
{struct Cyc_List_List*_tmp796;_tmp226=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp796=_cycalloc(sizeof(*_tmp796)),((_tmp796->hd=Cyc_Absynpp_cyc_stringptr,((_tmp796->tl=_tmp22B,_tmp796))))): _tmp22B;}
# 922
goto _LL127;_LL127:;}
# 924
if(Cyc_Absynpp_qvar_to_Cids){
# 925
const char*_tmp798;struct Cyc_List_List*_tmp797;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp226,(
# 926
(_tmp797=_cycalloc(sizeof(*_tmp797)),((_tmp797->hd=(*q).f2,((_tmp797->tl=0,_tmp797))))))),((_tmp798="_",_tag_dyneither(_tmp798,sizeof(char),2))));}else{
# 929
if(match)
# 930
return*(*q).f2;else{
# 932
const char*_tmp79A;struct Cyc_List_List*_tmp799;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp226,(
# 933
(_tmp799=_cycalloc(sizeof(*_tmp799)),((_tmp799->hd=(*q).f2,((_tmp799->tl=0,_tmp799))))))),((_tmp79A="::",_tag_dyneither(_tmp79A,sizeof(char),3))));}}}
# 937
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
# 938
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 941
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
# 942
struct _dyneither_ptr _tmp231=Cyc_Absynpp_qvar2string(q);
# 943
if(Cyc_PP_tex_output){
# 945
const char*_tmp79C;const char*_tmp79B;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp79B="\\textbf{",_tag_dyneither(_tmp79B,sizeof(char),9))),(struct _dyneither_ptr)_tmp231),((_tmp79C="}",_tag_dyneither(_tmp79C,sizeof(char),2)))),(int)
# 946
Cyc_strlen((struct _dyneither_ptr)_tmp231));}else{
# 947
return Cyc_PP_text(_tmp231);}}
# 950
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 952
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 955
if(Cyc_Absynpp_use_curr_namespace){
# 956
union Cyc_Absyn_Nmspace _tmp234=(*v).f1;int _tmp235;struct Cyc_List_List*_tmp236;struct Cyc_List_List*_tmp237;struct Cyc_List_List*_tmp238;_LL131: if((_tmp234.Loc_n).tag != 4)goto _LL133;_tmp235=(int)(_tmp234.Loc_n).val;_LL132:
# 957
 goto _LL134;_LL133: if((_tmp234.Rel_n).tag != 1)goto _LL135;_tmp236=(struct Cyc_List_List*)(_tmp234.Rel_n).val;if(_tmp236 != 0)goto _LL135;_LL134:
# 958
 return*(*v).f2;_LL135: if((_tmp234.C_n).tag != 3)goto _LL137;_tmp237=(struct Cyc_List_List*)(_tmp234.C_n).val;_LL136:
# 959
 _tmp238=_tmp237;goto _LL138;_LL137: if((_tmp234.Abs_n).tag != 2)goto _LL139;_tmp238=(struct Cyc_List_List*)(_tmp234.Abs_n).val;_LL138:
# 961
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp238,Cyc_Absynpp_curr_namespace)== 0)
# 962
return*(*v).f2;else{
# 964
goto _LL13A;}_LL139:;_LL13A: {
# 966
const char*_tmp79D;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp79D="/* bad namespace : */ ",_tag_dyneither(_tmp79D,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{
# 969
return*(*v).f2;}}
# 972
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
# 973
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 975
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
# 976
struct _dyneither_ptr _tmp23A=Cyc_Absynpp_typedef_name2string(v);
# 977
if(Cyc_PP_tex_output){
# 979
const char*_tmp79F;const char*_tmp79E;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp79E="\\textbf{",_tag_dyneither(_tmp79E,sizeof(char),9))),(struct _dyneither_ptr)_tmp23A),((_tmp79F="}",_tag_dyneither(_tmp79F,sizeof(char),2)))),(int)
# 980
Cyc_strlen((struct _dyneither_ptr)_tmp23A));}else{
# 981
return Cyc_PP_text(_tmp23A);}}
# 984
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
# 985
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 995 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
# 996
void*_tmp23D=e->r;enum Cyc_Absyn_Primop _tmp241;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_Absyn_Exp*_tmp24D;_LL13C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp23E->tag != 0)goto _LL13E;}_LL13D:
# 997
 goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp23F->tag != 1)goto _LL140;}_LL13F:
# 998
 return 10000;_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp240->tag != 2)goto _LL142;else{_tmp241=_tmp240->f1;}}_LL141:
# 1000
 switch(_tmp241){case Cyc_Absyn_Plus: _LL18A:
# 1001
 return 100;case Cyc_Absyn_Times: _LL18B:
# 1002
 return 110;case Cyc_Absyn_Minus: _LL18C:
# 1003
 return 100;case Cyc_Absyn_Div: _LL18D:
# 1004
 goto _LL18E;case Cyc_Absyn_Mod: _LL18E:
# 1005
 return 110;case Cyc_Absyn_Eq: _LL18F:
# 1006
 goto _LL190;case Cyc_Absyn_Neq: _LL190:
# 1007
 return 70;case Cyc_Absyn_Gt: _LL191:
# 1008
 goto _LL192;case Cyc_Absyn_Lt: _LL192:
# 1009
 goto _LL193;case Cyc_Absyn_Gte: _LL193:
# 1010
 goto _LL194;case Cyc_Absyn_Lte: _LL194:
# 1011
 return 80;case Cyc_Absyn_Not: _LL195:
# 1012
 goto _LL196;case Cyc_Absyn_Bitnot: _LL196:
# 1013
 return 130;case Cyc_Absyn_Bitand: _LL197:
# 1014
 return 60;case Cyc_Absyn_Bitor: _LL198:
# 1015
 return 40;case Cyc_Absyn_Bitxor: _LL199:
# 1016
 return 50;case Cyc_Absyn_Bitlshift: _LL19A:
# 1017
 return 90;case Cyc_Absyn_Bitlrshift: _LL19B:
# 1018
 return 80;case Cyc_Absyn_Bitarshift: _LL19C:
# 1019
 return 80;case Cyc_Absyn_Numelts: _LL19D:
# 1020
 return 140;}_LL142: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp242->tag != 3)goto _LL144;}_LL143:
# 1022
 return 20;_LL144: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp243->tag != 4)goto _LL146;}_LL145:
# 1023
 return 130;_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp244->tag != 5)goto _LL148;}_LL147:
# 1024
 return 30;_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp245->tag != 6)goto _LL14A;}_LL149:
# 1025
 return 35;_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp246->tag != 7)goto _LL14C;}_LL14B:
# 1026
 return 30;_LL14C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp247->tag != 8)goto _LL14E;}_LL14D:
# 1027
 return 10;_LL14E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp248->tag != 9)goto _LL150;}_LL14F:
# 1028
 return 140;_LL150: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp249=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp249->tag != 10)goto _LL152;}_LL151:
# 1029
 return 130;_LL152: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24A->tag != 11)goto _LL154;else{_tmp24B=_tmp24A->f1;}}_LL153:
# 1030
 return Cyc_Absynpp_exp_prec(_tmp24B);_LL154: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24C->tag != 12)goto _LL156;else{_tmp24D=_tmp24C->f1;}}_LL155:
# 1031
 return Cyc_Absynpp_exp_prec(_tmp24D);_LL156: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24E->tag != 13)goto _LL158;}_LL157:
# 1032
 return 120;_LL158: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp24F=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp24F->tag != 15)goto _LL15A;}_LL159:
# 1033
 return 15;_LL15A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp250->tag != 14)goto _LL15C;}_LL15B:
# 1034
 goto _LL15D;_LL15C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp251->tag != 16)goto _LL15E;}_LL15D:
# 1035
 goto _LL15F;_LL15E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp252=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp252->tag != 17)goto _LL160;}_LL15F:
# 1036
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp253->tag != 37)goto _LL162;}_LL161:
# 1037
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp254=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp254->tag != 38)goto _LL164;}_LL163:
# 1038
 goto _LL165;_LL164: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp255->tag != 36)goto _LL166;}_LL165:
# 1039
 goto _LL167;_LL166: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp256=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp256->tag != 18)goto _LL168;}_LL167:
# 1040
 goto _LL169;_LL168: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp257->tag != 19)goto _LL16A;}_LL169:
# 1041
 return 130;_LL16A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp258->tag != 20)goto _LL16C;}_LL16B:
# 1042
 goto _LL16D;_LL16C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp259=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp259->tag != 21)goto _LL16E;}_LL16D:
# 1043
 goto _LL16F;_LL16E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25A->tag != 22)goto _LL170;}_LL16F:
# 1044
 return 140;_LL170: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp25B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25B->tag != 23)goto _LL172;}_LL171:
# 1045
 return 150;_LL172: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25C->tag != 24)goto _LL174;}_LL173:
# 1046
 goto _LL175;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25D->tag != 25)goto _LL176;}_LL175:
# 1047
 goto _LL177;_LL176: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25E->tag != 26)goto _LL178;}_LL177:
# 1048
 goto _LL179;_LL178: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp25F->tag != 27)goto _LL17A;}_LL179:
# 1049
 goto _LL17B;_LL17A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp260=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp260->tag != 28)goto _LL17C;}_LL17B:
# 1050
 goto _LL17D;_LL17C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp261->tag != 29)goto _LL17E;}_LL17D:
# 1051
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp262->tag != 30)goto _LL180;}_LL17F:
# 1052
 goto _LL181;_LL180: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp263=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp263->tag != 31)goto _LL182;}_LL181:
# 1053
 goto _LL183;_LL182: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp264->tag != 32)goto _LL184;}_LL183:
# 1054
 goto _LL185;_LL184: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp265=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp265->tag != 33)goto _LL186;}_LL185:
# 1055
 goto _LL187;_LL186: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp266=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp266->tag != 34)goto _LL188;}_LL187:
# 1056
 return 140;_LL188: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp267=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp23D;if(_tmp267->tag != 35)goto _LL13B;}_LL189:
# 1057
 return 10000;_LL13B:;}
# 1061
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
# 1062
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1065
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
# 1066
int myprec=Cyc_Absynpp_exp_prec(e);
# 1067
struct Cyc_PP_Doc*s;
# 1068
{void*_tmp268=e->r;union Cyc_Absyn_Cnst _tmp26A;struct _tuple0*_tmp26C;enum Cyc_Absyn_Primop _tmp26E;struct Cyc_List_List*_tmp26F;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Core_Opt*_tmp272;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp275;enum Cyc_Absyn_Incrementor _tmp276;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_Absyn_Exp*_tmp27A;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_Absyn_Exp*_tmp27F;struct Cyc_Absyn_Exp*_tmp280;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_List_List*_tmp286;struct Cyc_Absyn_Exp*_tmp288;struct Cyc_Absyn_Exp*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;void*_tmp28E;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp294;void*_tmp296;struct Cyc_Absyn_Exp*_tmp298;void*_tmp29A;int _tmp29C;struct _dyneither_ptr _tmp29D;struct Cyc_Absyn_Exp*_tmp29F;struct _dyneither_ptr*_tmp2A0;void*_tmp2A2;void*_tmp2A3;struct _dyneither_ptr*_tmp2A5;void*_tmp2A7;void*_tmp2A8;unsigned int _tmp2AA;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AE;struct _dyneither_ptr*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B1;struct _dyneither_ptr*_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_List_List*_tmp2B7;struct _tuple8*_tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Vardecl*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;struct _tuple0*_tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*_tmp2C6;struct Cyc_List_List*_tmp2C8;struct Cyc_Absyn_Datatypefield*_tmp2C9;struct _tuple0*_tmp2CB;struct _tuple0*_tmp2CD;struct Cyc_Absyn_MallocInfo _tmp2CF;int _tmp2D0;struct Cyc_Absyn_Exp*_tmp2D1;void**_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D6;struct Cyc_Core_Opt*_tmp2D8;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Stmt*_tmp2DB;_LL1A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp269=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp269->tag != 0)goto _LL1A2;else{_tmp26A=_tmp269->f1;}}_LL1A1:
# 1069
 s=Cyc_Absynpp_cnst2doc(_tmp26A);goto _LL19F;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp26B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp26B->tag != 1)goto _LL1A4;else{_tmp26C=_tmp26B->f1;}}_LL1A3:
# 1070
 s=Cyc_Absynpp_qvar2doc(_tmp26C);goto _LL19F;_LL1A4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp26D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp26D->tag != 2)goto _LL1A6;else{_tmp26E=_tmp26D->f1;_tmp26F=_tmp26D->f2;}}_LL1A5:
# 1071
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp26E,_tmp26F);goto _LL19F;_LL1A6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp270=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp270->tag != 3)goto _LL1A8;else{_tmp271=_tmp270->f1;_tmp272=_tmp270->f2;_tmp273=_tmp270->f3;}}_LL1A7:
# 1073
{const char*_tmp7A4;const char*_tmp7A3;struct Cyc_PP_Doc*_tmp7A2[5];s=((_tmp7A2[4]=
# 1077
Cyc_Absynpp_exp2doc_prec(myprec,_tmp273),((_tmp7A2[3]=
# 1076
Cyc_PP_text(((_tmp7A3="= ",_tag_dyneither(_tmp7A3,sizeof(char),3)))),((_tmp7A2[2]=
# 1075
_tmp272 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp272->v),((_tmp7A2[1]=
# 1074
Cyc_PP_text(((_tmp7A4=" ",_tag_dyneither(_tmp7A4,sizeof(char),2)))),((_tmp7A2[0]=
# 1073
Cyc_Absynpp_exp2doc_prec(myprec,_tmp271),Cyc_PP_cat(_tag_dyneither(_tmp7A2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1078
goto _LL19F;_LL1A8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp274=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp274->tag != 4)goto _LL1AA;else{_tmp275=_tmp274->f1;_tmp276=_tmp274->f2;}}_LL1A9: {
# 1080
struct Cyc_PP_Doc*_tmp2DF=Cyc_Absynpp_exp2doc_prec(myprec,_tmp275);
# 1081
switch(_tmp276){case Cyc_Absyn_PreInc: _LL1F0:
# 1082
{const char*_tmp7A7;struct Cyc_PP_Doc*_tmp7A6[2];s=((_tmp7A6[1]=_tmp2DF,((_tmp7A6[0]=Cyc_PP_text(((_tmp7A7="++",_tag_dyneither(_tmp7A7,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A6,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F1:
# 1083
{const char*_tmp7AA;struct Cyc_PP_Doc*_tmp7A9[2];s=((_tmp7A9[1]=_tmp2DF,((_tmp7A9[0]=Cyc_PP_text(((_tmp7AA="--",_tag_dyneither(_tmp7AA,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp7A9,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F2:
# 1084
{const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[2];s=((_tmp7AC[1]=Cyc_PP_text(((_tmp7AD="++",_tag_dyneither(_tmp7AD,sizeof(char),3)))),((_tmp7AC[0]=_tmp2DF,Cyc_PP_cat(_tag_dyneither(_tmp7AC,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F3:
# 1085
{const char*_tmp7B0;struct Cyc_PP_Doc*_tmp7AF[2];s=((_tmp7AF[1]=Cyc_PP_text(((_tmp7B0="--",_tag_dyneither(_tmp7B0,sizeof(char),3)))),((_tmp7AF[0]=_tmp2DF,Cyc_PP_cat(_tag_dyneither(_tmp7AF,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1087
goto _LL19F;}_LL1AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp277=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp277->tag != 5)goto _LL1AC;else{_tmp278=_tmp277->f1;_tmp279=_tmp277->f2;_tmp27A=_tmp277->f3;}}_LL1AB:
# 1089
{const char*_tmp7B5;const char*_tmp7B4;struct Cyc_PP_Doc*_tmp7B3[5];s=((_tmp7B3[4]=
# 1090
Cyc_Absynpp_exp2doc_prec(myprec,_tmp27A),((_tmp7B3[3]=Cyc_PP_text(((_tmp7B4=" : ",_tag_dyneither(_tmp7B4,sizeof(char),4)))),((_tmp7B3[2]=
# 1089
Cyc_Absynpp_exp2doc_prec(0,_tmp279),((_tmp7B3[1]=Cyc_PP_text(((_tmp7B5=" ? ",_tag_dyneither(_tmp7B5,sizeof(char),4)))),((_tmp7B3[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp278),Cyc_PP_cat(_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1091
goto _LL19F;_LL1AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp27B=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp27B->tag != 6)goto _LL1AE;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27B->f2;}}_LL1AD:
# 1093
{const char*_tmp7B8;struct Cyc_PP_Doc*_tmp7B7[3];s=((_tmp7B7[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27D),((_tmp7B7[1]=Cyc_PP_text(((_tmp7B8=" && ",_tag_dyneither(_tmp7B8,sizeof(char),5)))),((_tmp7B7[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27C),Cyc_PP_cat(_tag_dyneither(_tmp7B7,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1094
goto _LL19F;_LL1AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp27E=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp27E->tag != 7)goto _LL1B0;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;}}_LL1AF:
# 1096
{const char*_tmp7BB;struct Cyc_PP_Doc*_tmp7BA[3];s=((_tmp7BA[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp280),((_tmp7BA[1]=Cyc_PP_text(((_tmp7BB=" || ",_tag_dyneither(_tmp7BB,sizeof(char),5)))),((_tmp7BA[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27F),Cyc_PP_cat(_tag_dyneither(_tmp7BA,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1097
goto _LL19F;_LL1B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp281=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp281->tag != 8)goto _LL1B2;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}_LL1B1:
# 1101
{const char*_tmp7C2;const char*_tmp7C1;const char*_tmp7C0;struct Cyc_PP_Doc*_tmp7BF[5];s=((_tmp7BF[4]=Cyc_PP_text(((_tmp7C0=")",_tag_dyneither(_tmp7C0,sizeof(char),2)))),((_tmp7BF[3]=Cyc_Absynpp_exp2doc(_tmp283),((_tmp7BF[2]=Cyc_PP_text(((_tmp7C1=", ",_tag_dyneither(_tmp7C1,sizeof(char),3)))),((_tmp7BF[1]=Cyc_Absynpp_exp2doc(_tmp282),((_tmp7BF[0]=Cyc_PP_text(((_tmp7C2="(",_tag_dyneither(_tmp7C2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7BF,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1102
goto _LL19F;_LL1B2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp284=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp284->tag != 9)goto _LL1B4;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}_LL1B3:
# 1104
{const char*_tmp7C7;const char*_tmp7C6;struct Cyc_PP_Doc*_tmp7C5[4];s=((_tmp7C5[3]=
# 1107
Cyc_PP_text(((_tmp7C6=")",_tag_dyneither(_tmp7C6,sizeof(char),2)))),((_tmp7C5[2]=
# 1106
Cyc_Absynpp_exps2doc_prec(20,_tmp286),((_tmp7C5[1]=
# 1105
Cyc_PP_text(((_tmp7C7="(",_tag_dyneither(_tmp7C7,sizeof(char),2)))),((_tmp7C5[0]=
# 1104
Cyc_Absynpp_exp2doc_prec(myprec,_tmp285),Cyc_PP_cat(_tag_dyneither(_tmp7C5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1108
goto _LL19F;_LL1B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp287=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp287->tag != 10)goto _LL1B6;else{_tmp288=_tmp287->f1;}}_LL1B5:
# 1110
{const char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[2];s=((_tmp7C9[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp288),((_tmp7C9[0]=Cyc_PP_text(((_tmp7CA="throw ",_tag_dyneither(_tmp7CA,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1111
goto _LL19F;_LL1B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp289=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp289->tag != 11)goto _LL1B8;else{_tmp28A=_tmp289->f1;}}_LL1B7:
# 1113
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28A);
# 1114
goto _LL19F;_LL1B8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp28B=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp28B->tag != 12)goto _LL1BA;else{_tmp28C=_tmp28B->f1;}}_LL1B9:
# 1116
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp28C);
# 1117
goto _LL19F;_LL1BA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp28D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp28D->tag != 13)goto _LL1BC;else{_tmp28E=(void*)_tmp28D->f1;_tmp28F=_tmp28D->f2;}}_LL1BB:
# 1119
{const char*_tmp7CF;const char*_tmp7CE;struct Cyc_PP_Doc*_tmp7CD[4];s=((_tmp7CD[3]=
# 1122
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28F),((_tmp7CD[2]=
# 1121
Cyc_PP_text(((_tmp7CE=")",_tag_dyneither(_tmp7CE,sizeof(char),2)))),((_tmp7CD[1]=
# 1120
Cyc_Absynpp_typ2doc(_tmp28E),((_tmp7CD[0]=
# 1119
Cyc_PP_text(((_tmp7CF="(",_tag_dyneither(_tmp7CF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7CD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1123
goto _LL19F;_LL1BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp290=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp290->tag != 14)goto _LL1BE;else{_tmp291=_tmp290->f1;}}_LL1BD:
# 1125
{const char*_tmp7D2;struct Cyc_PP_Doc*_tmp7D1[2];s=((_tmp7D1[1]=
# 1126
Cyc_Absynpp_exp2doc_prec(myprec,_tmp291),((_tmp7D1[0]=
# 1125
Cyc_PP_text(((_tmp7D2="&",_tag_dyneither(_tmp7D2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D1,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1127
goto _LL19F;_LL1BE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp292->tag != 15)goto _LL1C0;else{_tmp293=_tmp292->f1;_tmp294=_tmp292->f2;}}_LL1BF:
# 1129
 if(_tmp293 == 0){
# 1130
const char*_tmp7D5;struct Cyc_PP_Doc*_tmp7D4[2];s=((_tmp7D4[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp294),((_tmp7D4[0]=Cyc_PP_text(((_tmp7D5="new ",_tag_dyneither(_tmp7D5,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7D4,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1132
const char*_tmp7DA;const char*_tmp7D9;struct Cyc_PP_Doc*_tmp7D8[4];s=((_tmp7D8[3]=
# 1133
Cyc_Absynpp_exp2doc_prec(myprec,_tmp294),((_tmp7D8[2]=
# 1132
Cyc_PP_text(((_tmp7D9=") ",_tag_dyneither(_tmp7D9,sizeof(char),3)))),((_tmp7D8[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp293),((_tmp7D8[0]=Cyc_PP_text(((_tmp7DA="rnew(",_tag_dyneither(_tmp7DA,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp7D8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1134
goto _LL19F;_LL1C0: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp295->tag != 16)goto _LL1C2;else{_tmp296=(void*)_tmp295->f1;}}_LL1C1:
# 1135
{const char*_tmp7DF;const char*_tmp7DE;struct Cyc_PP_Doc*_tmp7DD[3];s=((_tmp7DD[2]=Cyc_PP_text(((_tmp7DE=")",_tag_dyneither(_tmp7DE,sizeof(char),2)))),((_tmp7DD[1]=Cyc_Absynpp_typ2doc(_tmp296),((_tmp7DD[0]=Cyc_PP_text(((_tmp7DF="sizeof(",_tag_dyneither(_tmp7DF,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7DD,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp297=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp297->tag != 17)goto _LL1C4;else{_tmp298=_tmp297->f1;}}_LL1C3:
# 1136
{const char*_tmp7E4;const char*_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[3];s=((_tmp7E2[2]=Cyc_PP_text(((_tmp7E3=")",_tag_dyneither(_tmp7E3,sizeof(char),2)))),((_tmp7E2[1]=Cyc_Absynpp_exp2doc(_tmp298),((_tmp7E2[0]=Cyc_PP_text(((_tmp7E4="sizeof(",_tag_dyneither(_tmp7E4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7E2,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C4: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp299=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp299->tag != 37)goto _LL1C6;else{_tmp29A=(void*)_tmp299->f1;}}_LL1C5:
# 1137
{const char*_tmp7E9;const char*_tmp7E8;struct Cyc_PP_Doc*_tmp7E7[3];s=((_tmp7E7[2]=Cyc_PP_text(((_tmp7E8=")",_tag_dyneither(_tmp7E8,sizeof(char),2)))),((_tmp7E7[1]=Cyc_Absynpp_typ2doc(_tmp29A),((_tmp7E7[0]=Cyc_PP_text(((_tmp7E9="valueof(",_tag_dyneither(_tmp7E9,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E7,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C6: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp29B=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp29B->tag != 38)goto _LL1C8;else{_tmp29C=_tmp29B->f1;_tmp29D=_tmp29B->f2;}}_LL1C7:
# 1139
 if(_tmp29C){
# 1140
const char*_tmp7F0;const char*_tmp7EF;const char*_tmp7EE;struct Cyc_PP_Doc*_tmp7ED[4];s=((_tmp7ED[3]=Cyc_PP_text(((_tmp7EE=")",_tag_dyneither(_tmp7EE,sizeof(char),2)))),((_tmp7ED[2]=Cyc_PP_text(_tmp29D),((_tmp7ED[1]=Cyc_PP_text(((_tmp7EF=" volatile (",_tag_dyneither(_tmp7EF,sizeof(char),12)))),((_tmp7ED[0]=Cyc_PP_text(((_tmp7F0="__asm__",_tag_dyneither(_tmp7F0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7ED,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1142
const char*_tmp7F5;const char*_tmp7F4;struct Cyc_PP_Doc*_tmp7F3[3];s=((_tmp7F3[2]=Cyc_PP_text(((_tmp7F4=")",_tag_dyneither(_tmp7F4,sizeof(char),2)))),((_tmp7F3[1]=Cyc_PP_text(_tmp29D),((_tmp7F3[0]=Cyc_PP_text(((_tmp7F5="__asm__(",_tag_dyneither(_tmp7F5,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7F3,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1143
goto _LL19F;_LL1C8: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp29E=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp29E->tag != 36)goto _LL1CA;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;}}_LL1C9:
# 1145
{const char*_tmp7FC;const char*_tmp7FB;const char*_tmp7FA;struct Cyc_PP_Doc*_tmp7F9[5];s=((_tmp7F9[4]=Cyc_PP_text(((_tmp7FA=")",_tag_dyneither(_tmp7FA,sizeof(char),2)))),((_tmp7F9[3]=Cyc_PP_textptr(_tmp2A0),((_tmp7F9[2]=Cyc_PP_text(((_tmp7FB=".",_tag_dyneither(_tmp7FB,sizeof(char),2)))),((_tmp7F9[1]=Cyc_Absynpp_exp2doc(_tmp29F),((_tmp7F9[0]=Cyc_PP_text(((_tmp7FC="tagcheck(",_tag_dyneither(_tmp7FC,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7F9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1146
goto _LL19F;_LL1CA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2A1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2A1->tag != 18)goto _LL1CC;else{_tmp2A2=(void*)_tmp2A1->f1;_tmp2A3=(void*)_tmp2A1->f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2A4=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2A3;if(_tmp2A4->tag != 0)goto _LL1CC;else{_tmp2A5=_tmp2A4->f1;}};}}_LL1CB:
# 1148
{const char*_tmp803;const char*_tmp802;const char*_tmp801;struct Cyc_PP_Doc*_tmp800[5];s=((_tmp800[4]=Cyc_PP_text(((_tmp801=")",_tag_dyneither(_tmp801,sizeof(char),2)))),((_tmp800[3]=Cyc_PP_textptr(_tmp2A5),((_tmp800[2]=Cyc_PP_text(((_tmp802=",",_tag_dyneither(_tmp802,sizeof(char),2)))),((_tmp800[1]=Cyc_Absynpp_typ2doc(_tmp2A2),((_tmp800[0]=Cyc_PP_text(((_tmp803="offsetof(",_tag_dyneither(_tmp803,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp800,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1149
goto _LL19F;_LL1CC: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2A6=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2A6->tag != 18)goto _LL1CE;else{_tmp2A7=(void*)_tmp2A6->f1;_tmp2A8=(void*)_tmp2A6->f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2A9=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2A8;if(_tmp2A9->tag != 1)goto _LL1CE;else{_tmp2AA=_tmp2A9->f1;}};}}_LL1CD:
# 1151
{const char*_tmp811;const char*_tmp810;struct Cyc_Int_pa_PrintArg_struct _tmp80F;void*_tmp80E[1];const char*_tmp80D;const char*_tmp80C;struct Cyc_PP_Doc*_tmp80B[5];s=((_tmp80B[4]=Cyc_PP_text(((_tmp80C=")",_tag_dyneither(_tmp80C,sizeof(char),2)))),((_tmp80B[3]=Cyc_PP_text((struct _dyneither_ptr)((_tmp80F.tag=1,((_tmp80F.f1=(unsigned long)((int)_tmp2AA),((_tmp80E[0]=& _tmp80F,Cyc_aprintf(((_tmp80D="%d",_tag_dyneither(_tmp80D,sizeof(char),3))),_tag_dyneither(_tmp80E,sizeof(void*),1))))))))),((_tmp80B[2]=Cyc_PP_text(((_tmp810=",",_tag_dyneither(_tmp810,sizeof(char),2)))),((_tmp80B[1]=Cyc_Absynpp_typ2doc(_tmp2A7),((_tmp80B[0]=Cyc_PP_text(((_tmp811="offsetof(",_tag_dyneither(_tmp811,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp80B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1152
goto _LL19F;_LL1CE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2AB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2AB->tag != 19)goto _LL1D0;else{_tmp2AC=_tmp2AB->f1;}}_LL1CF:
# 1154
{const char*_tmp814;struct Cyc_PP_Doc*_tmp813[2];s=((_tmp813[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AC),((_tmp813[0]=Cyc_PP_text(((_tmp814="*",_tag_dyneither(_tmp814,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp813,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1155
goto _LL19F;_LL1D0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2AD=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2AD->tag != 20)goto _LL1D2;else{_tmp2AE=_tmp2AD->f1;_tmp2AF=_tmp2AD->f2;}}_LL1D1:
# 1157
{const char*_tmp817;struct Cyc_PP_Doc*_tmp816[3];s=((_tmp816[2]=Cyc_PP_textptr(_tmp2AF),((_tmp816[1]=Cyc_PP_text(((_tmp817=".",_tag_dyneither(_tmp817,sizeof(char),2)))),((_tmp816[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AE),Cyc_PP_cat(_tag_dyneither(_tmp816,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1158
goto _LL19F;_LL1D2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2B0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2B0->tag != 21)goto _LL1D4;else{_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B0->f2;}}_LL1D3:
# 1160
{const char*_tmp81A;struct Cyc_PP_Doc*_tmp819[3];s=((_tmp819[2]=Cyc_PP_textptr(_tmp2B2),((_tmp819[1]=Cyc_PP_text(((_tmp81A="->",_tag_dyneither(_tmp81A,sizeof(char),3)))),((_tmp819[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B1),Cyc_PP_cat(_tag_dyneither(_tmp819,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1161
goto _LL19F;_LL1D4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2B3->tag != 22)goto _LL1D6;else{_tmp2B4=_tmp2B3->f1;_tmp2B5=_tmp2B3->f2;}}_LL1D5:
# 1163
{const char*_tmp81F;const char*_tmp81E;struct Cyc_PP_Doc*_tmp81D[4];s=((_tmp81D[3]=
# 1166
Cyc_PP_text(((_tmp81E="]",_tag_dyneither(_tmp81E,sizeof(char),2)))),((_tmp81D[2]=
# 1165
Cyc_Absynpp_exp2doc(_tmp2B5),((_tmp81D[1]=
# 1164
Cyc_PP_text(((_tmp81F="[",_tag_dyneither(_tmp81F,sizeof(char),2)))),((_tmp81D[0]=
# 1163
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B4),Cyc_PP_cat(_tag_dyneither(_tmp81D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1167
goto _LL19F;_LL1D6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2B6=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2B6->tag != 23)goto _LL1D8;else{_tmp2B7=_tmp2B6->f1;}}_LL1D7:
# 1169
{const char*_tmp824;const char*_tmp823;struct Cyc_PP_Doc*_tmp822[4];s=((_tmp822[3]=
# 1172
Cyc_PP_text(((_tmp823=")",_tag_dyneither(_tmp823,sizeof(char),2)))),((_tmp822[2]=
# 1171
Cyc_Absynpp_exps2doc_prec(20,_tmp2B7),((_tmp822[1]=
# 1170
Cyc_PP_text(((_tmp824="(",_tag_dyneither(_tmp824,sizeof(char),2)))),((_tmp822[0]=
# 1169
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp822,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1173
goto _LL19F;_LL1D8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp2B8=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2B8->tag != 24)goto _LL1DA;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B8->f2;}}_LL1D9:
# 1175
{const char*_tmp82B;const char*_tmp82A;const char*_tmp829;struct Cyc_PP_Doc*_tmp828[4];s=((_tmp828[3]=
# 1178
Cyc_Absynpp_group_braces(((_tmp829=",",_tag_dyneither(_tmp829,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BA)),((_tmp828[2]=
# 1177
Cyc_PP_text(((_tmp82A=")",_tag_dyneither(_tmp82A,sizeof(char),2)))),((_tmp828[1]=
# 1176
Cyc_Absynpp_typ2doc((*_tmp2B9).f3),((_tmp828[0]=
# 1175
Cyc_PP_text(((_tmp82B="(",_tag_dyneither(_tmp82B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp828,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1179
goto _LL19F;_LL1DA: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2BB=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2BB->tag != 25)goto _LL1DC;else{_tmp2BC=_tmp2BB->f1;}}_LL1DB:
# 1181
{const char*_tmp82C;s=Cyc_Absynpp_group_braces(((_tmp82C=",",_tag_dyneither(_tmp82C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2BC));}
# 1182
goto _LL19F;_LL1DC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp2BD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2BD->tag != 26)goto _LL1DE;else{_tmp2BE=_tmp2BD->f1;_tmp2BF=_tmp2BD->f2;_tmp2C0=_tmp2BD->f3;}}_LL1DD:
# 1184
{const char*_tmp833;const char*_tmp832;const char*_tmp831;struct Cyc_PP_Doc*_tmp830[8];s=((_tmp830[7]=
# 1190
Cyc_Absynpp_rb(),((_tmp830[6]=
# 1189
Cyc_Absynpp_exp2doc(_tmp2C0),((_tmp830[5]=
# 1188
Cyc_PP_text(((_tmp831=" : ",_tag_dyneither(_tmp831,sizeof(char),4)))),((_tmp830[4]=
# 1187
Cyc_Absynpp_exp2doc(_tmp2BF),((_tmp830[3]=
# 1186
Cyc_PP_text(((_tmp832=" < ",_tag_dyneither(_tmp832,sizeof(char),4)))),((_tmp830[2]=
# 1185
Cyc_PP_text(*(*_tmp2BE->name).f2),((_tmp830[1]=
# 1184
Cyc_PP_text(((_tmp833="for ",_tag_dyneither(_tmp833,sizeof(char),5)))),((_tmp830[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp830,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1191
goto _LL19F;_LL1DE: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2C1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2C1->tag != 27)goto _LL1E0;else{_tmp2C2=_tmp2C1->f1;_tmp2C3=_tmp2C1->f2;_tmp2C4=_tmp2C1->f3;}}_LL1DF: {
# 1193
struct Cyc_List_List*_tmp336=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C4);
# 1194
{const char*_tmp838;struct Cyc_List_List*_tmp837;struct Cyc_PP_Doc*_tmp836[2];s=((_tmp836[1]=
# 1195
Cyc_Absynpp_group_braces(((_tmp838=",",_tag_dyneither(_tmp838,sizeof(char),2))),
# 1196
_tmp2C3 != 0?(_tmp837=_cycalloc(sizeof(*_tmp837)),((_tmp837->hd=Cyc_Absynpp_tps2doc(_tmp2C3),((_tmp837->tl=_tmp336,_tmp837))))): _tmp336),((_tmp836[0]=
# 1194
Cyc_Absynpp_qvar2doc(_tmp2C2),Cyc_PP_cat(_tag_dyneither(_tmp836,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1197
goto _LL19F;}_LL1E0: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2C5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2C5->tag != 28)goto _LL1E2;else{_tmp2C6=_tmp2C5->f2;}}_LL1E1:
# 1199
{const char*_tmp839;s=Cyc_Absynpp_group_braces(((_tmp839=",",_tag_dyneither(_tmp839,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C6));}
# 1200
goto _LL19F;_LL1E2: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2C7=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2C7->tag != 29)goto _LL1E4;else{_tmp2C8=_tmp2C7->f1;_tmp2C9=_tmp2C7->f3;}}_LL1E3:
# 1202
 if(_tmp2C8 == 0)
# 1204
s=Cyc_Absynpp_qvar2doc(_tmp2C9->name);else{
# 1206
const char*_tmp840;const char*_tmp83F;const char*_tmp83E;struct Cyc_PP_Doc*_tmp83D[2];s=((_tmp83D[1]=
# 1207
Cyc_PP_egroup(((_tmp840="(",_tag_dyneither(_tmp840,sizeof(char),2))),((_tmp83F=")",_tag_dyneither(_tmp83F,sizeof(char),2))),((_tmp83E=",",_tag_dyneither(_tmp83E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2C8)),((_tmp83D[0]=
# 1206
Cyc_Absynpp_qvar2doc(_tmp2C9->name),Cyc_PP_cat(_tag_dyneither(_tmp83D,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1208
goto _LL19F;_LL1E4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2CA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2CA->tag != 30)goto _LL1E6;else{_tmp2CB=_tmp2CA->f1;}}_LL1E5:
# 1210
 s=Cyc_Absynpp_qvar2doc(_tmp2CB);
# 1211
goto _LL19F;_LL1E6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2CC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2CC->tag != 31)goto _LL1E8;else{_tmp2CD=_tmp2CC->f1;}}_LL1E7:
# 1213
 s=Cyc_Absynpp_qvar2doc(_tmp2CD);
# 1214
goto _LL19F;_LL1E8: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2CE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2CE->tag != 32)goto _LL1EA;else{_tmp2CF=_tmp2CE->f1;_tmp2D0=_tmp2CF.is_calloc;_tmp2D1=_tmp2CF.rgn;_tmp2D2=_tmp2CF.elt_type;_tmp2D3=_tmp2CF.num_elts;}}_LL1E9:
# 1218
 if(_tmp2D0){
# 1220
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2D2)),0);
# 1221
if(_tmp2D1 == 0){
# 1222
const char*_tmp847;const char*_tmp846;const char*_tmp845;struct Cyc_PP_Doc*_tmp844[5];s=((_tmp844[4]=Cyc_PP_text(((_tmp845=")",_tag_dyneither(_tmp845,sizeof(char),2)))),((_tmp844[3]=Cyc_Absynpp_exp2doc(st),((_tmp844[2]=Cyc_PP_text(((_tmp846=",",_tag_dyneither(_tmp846,sizeof(char),2)))),((_tmp844[1]=Cyc_Absynpp_exp2doc(_tmp2D3),((_tmp844[0]=Cyc_PP_text(((_tmp847="calloc(",_tag_dyneither(_tmp847,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp844,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1224
const char*_tmp850;const char*_tmp84F;const char*_tmp84E;const char*_tmp84D;struct Cyc_PP_Doc*_tmp84C[7];s=((_tmp84C[6]=
# 1225
Cyc_PP_text(((_tmp84D=")",_tag_dyneither(_tmp84D,sizeof(char),2)))),((_tmp84C[5]=Cyc_Absynpp_exp2doc(st),((_tmp84C[4]=Cyc_PP_text(((_tmp84E=",",_tag_dyneither(_tmp84E,sizeof(char),2)))),((_tmp84C[3]=Cyc_Absynpp_exp2doc(_tmp2D3),((_tmp84C[2]=
# 1224
Cyc_PP_text(((_tmp84F=",",_tag_dyneither(_tmp84F,sizeof(char),2)))),((_tmp84C[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2D1),((_tmp84C[0]=Cyc_PP_text(((_tmp850="rcalloc(",_tag_dyneither(_tmp850,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp84C,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1227
struct Cyc_Absyn_Exp*new_e;
# 1229
if(_tmp2D2 == 0)
# 1230
new_e=_tmp2D3;else{
# 1232
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2D2,0),_tmp2D3,0);}
# 1234
if(_tmp2D1 == 0){
# 1235
const char*_tmp855;const char*_tmp854;struct Cyc_PP_Doc*_tmp853[3];s=((_tmp853[2]=Cyc_PP_text(((_tmp854=")",_tag_dyneither(_tmp854,sizeof(char),2)))),((_tmp853[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp853[0]=Cyc_PP_text(((_tmp855="malloc(",_tag_dyneither(_tmp855,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp853,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1237
const char*_tmp85C;const char*_tmp85B;const char*_tmp85A;struct Cyc_PP_Doc*_tmp859[5];s=((_tmp859[4]=
# 1238
Cyc_PP_text(((_tmp85A=")",_tag_dyneither(_tmp85A,sizeof(char),2)))),((_tmp859[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp859[2]=
# 1237
Cyc_PP_text(((_tmp85B=",",_tag_dyneither(_tmp85B,sizeof(char),2)))),((_tmp859[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2D1),((_tmp859[0]=Cyc_PP_text(((_tmp85C="rmalloc(",_tag_dyneither(_tmp85C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp859,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1240
goto _LL19F;_LL1EA: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2D4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2D4->tag != 33)goto _LL1EC;else{_tmp2D5=_tmp2D4->f1;_tmp2D6=_tmp2D4->f2;}}_LL1EB:
# 1242
{const char*_tmp85F;struct Cyc_PP_Doc*_tmp85E[3];s=((_tmp85E[2]=
# 1244
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D6),((_tmp85E[1]=
# 1243
Cyc_PP_text(((_tmp85F=" :=: ",_tag_dyneither(_tmp85F,sizeof(char),6)))),((_tmp85E[0]=
# 1242
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2D5),Cyc_PP_cat(_tag_dyneither(_tmp85E,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1245
goto _LL19F;_LL1EC: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2D7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2D7->tag != 34)goto _LL1EE;else{_tmp2D8=_tmp2D7->f1;_tmp2D9=_tmp2D7->f2;}}_LL1ED:
# 1248
{const char*_tmp860;s=Cyc_Absynpp_group_braces(((_tmp860=",",_tag_dyneither(_tmp860,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2D9));}
# 1249
goto _LL19F;_LL1EE: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2DA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp268;if(_tmp2DA->tag != 35)goto _LL19F;else{_tmp2DB=_tmp2DA->f1;}}_LL1EF:
# 1251
{const char*_tmp865;const char*_tmp864;struct Cyc_PP_Doc*_tmp863[7];s=((_tmp863[6]=
# 1253
Cyc_PP_text(((_tmp864=")",_tag_dyneither(_tmp864,sizeof(char),2)))),((_tmp863[5]=Cyc_Absynpp_rb(),((_tmp863[4]=Cyc_PP_blank_doc(),((_tmp863[3]=
# 1252
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2DB,1)),((_tmp863[2]=
# 1251
Cyc_PP_blank_doc(),((_tmp863[1]=Cyc_Absynpp_lb(),((_tmp863[0]=Cyc_PP_text(((_tmp865="(",_tag_dyneither(_tmp865,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp863,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1254
goto _LL19F;_LL19F:;}
# 1256
if(inprec >= myprec){
# 1257
const char*_tmp86A;const char*_tmp869;struct Cyc_PP_Doc*_tmp868[3];s=((_tmp868[2]=Cyc_PP_text(((_tmp869=")",_tag_dyneither(_tmp869,sizeof(char),2)))),((_tmp868[1]=s,((_tmp868[0]=Cyc_PP_text(((_tmp86A="(",_tag_dyneither(_tmp86A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp868,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1258
return s;}
# 1261
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
# 1262
void*_tmp358=d;struct Cyc_Absyn_Exp*_tmp35A;struct _dyneither_ptr*_tmp35C;_LL1F6: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp359=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp358;if(_tmp359->tag != 0)goto _LL1F8;else{_tmp35A=_tmp359->f1;}}_LL1F7: {
# 1263
const char*_tmp86F;const char*_tmp86E;struct Cyc_PP_Doc*_tmp86D[3];return(_tmp86D[2]=Cyc_PP_text(((_tmp86E="]",_tag_dyneither(_tmp86E,sizeof(char),2)))),((_tmp86D[1]=Cyc_Absynpp_exp2doc(_tmp35A),((_tmp86D[0]=Cyc_PP_text(((_tmp86F=".[",_tag_dyneither(_tmp86F,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp86D,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp35B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp358;if(_tmp35B->tag != 1)goto _LL1F5;else{_tmp35C=_tmp35B->f1;}}_LL1F9: {
# 1264
const char*_tmp872;struct Cyc_PP_Doc*_tmp871[2];return(_tmp871[1]=Cyc_PP_textptr(_tmp35C),((_tmp871[0]=Cyc_PP_text(((_tmp872=".",_tag_dyneither(_tmp872,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp871,sizeof(struct Cyc_PP_Doc*),2)))));}_LL1F5:;}
# 1268
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
# 1269
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
# 1270
const char*_tmp879;const char*_tmp878;const char*_tmp877;struct Cyc_PP_Doc*_tmp876[2];return(_tmp876[1]=
# 1271
Cyc_Absynpp_exp2doc((*de).f2),((_tmp876[0]=
# 1270
Cyc_PP_egroup(((_tmp879="",_tag_dyneither(_tmp879,sizeof(char),1))),((_tmp878="=",_tag_dyneither(_tmp878,sizeof(char),2))),((_tmp877="=",_tag_dyneither(_tmp877,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp876,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1274
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
# 1275
const char*_tmp87C;const char*_tmp87B;const char*_tmp87A;return Cyc_PP_group(((_tmp87A="",_tag_dyneither(_tmp87A,sizeof(char),1))),((_tmp87B="",_tag_dyneither(_tmp87B,sizeof(char),1))),((_tmp87C=",",_tag_dyneither(_tmp87C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1279
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
# 1280
union Cyc_Absyn_Cnst _tmp369=c;struct _tuple3 _tmp36A;enum Cyc_Absyn_Sign _tmp36B;char _tmp36C;struct _dyneither_ptr _tmp36D;struct _tuple4 _tmp36E;enum Cyc_Absyn_Sign _tmp36F;short _tmp370;struct _tuple5 _tmp371;enum Cyc_Absyn_Sign _tmp372;int _tmp373;struct _tuple5 _tmp374;enum Cyc_Absyn_Sign _tmp375;int _tmp376;struct _tuple5 _tmp377;enum Cyc_Absyn_Sign _tmp378;int _tmp379;struct _tuple6 _tmp37A;enum Cyc_Absyn_Sign _tmp37B;long long _tmp37C;struct _tuple7 _tmp37D;struct _dyneither_ptr _tmp37E;int _tmp37F;struct _dyneither_ptr _tmp380;struct _dyneither_ptr _tmp381;_LL1FB: if((_tmp369.Char_c).tag != 2)goto _LL1FD;_tmp36A=(struct _tuple3)(_tmp369.Char_c).val;_tmp36B=_tmp36A.f1;_tmp36C=_tmp36A.f2;_LL1FC: {
# 1281
const char*_tmp880;void*_tmp87F[1];struct Cyc_String_pa_PrintArg_struct _tmp87E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp87E.tag=0,((_tmp87E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp36C)),((_tmp87F[0]=& _tmp87E,Cyc_aprintf(((_tmp880="'%s'",_tag_dyneither(_tmp880,sizeof(char),5))),_tag_dyneither(_tmp87F,sizeof(void*),1)))))))));}_LL1FD: if((_tmp369.Wchar_c).tag != 3)goto _LL1FF;_tmp36D=(struct _dyneither_ptr)(_tmp369.Wchar_c).val;_LL1FE: {
# 1282
const char*_tmp884;void*_tmp883[1];struct Cyc_String_pa_PrintArg_struct _tmp882;return Cyc_PP_text((struct _dyneither_ptr)((_tmp882.tag=0,((_tmp882.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36D),((_tmp883[0]=& _tmp882,Cyc_aprintf(((_tmp884="L'%s'",_tag_dyneither(_tmp884,sizeof(char),6))),_tag_dyneither(_tmp883,sizeof(void*),1)))))))));}_LL1FF: if((_tmp369.Short_c).tag != 4)goto _LL201;_tmp36E=(struct _tuple4)(_tmp369.Short_c).val;_tmp36F=_tmp36E.f1;_tmp370=_tmp36E.f2;_LL200: {
# 1283
const char*_tmp888;void*_tmp887[1];struct Cyc_Int_pa_PrintArg_struct _tmp886;return Cyc_PP_text((struct _dyneither_ptr)((_tmp886.tag=1,((_tmp886.f1=(unsigned long)((int)_tmp370),((_tmp887[0]=& _tmp886,Cyc_aprintf(((_tmp888="%d",_tag_dyneither(_tmp888,sizeof(char),3))),_tag_dyneither(_tmp887,sizeof(void*),1)))))))));}_LL201: if((_tmp369.Int_c).tag != 5)goto _LL203;_tmp371=(struct _tuple5)(_tmp369.Int_c).val;_tmp372=_tmp371.f1;if(_tmp372 != Cyc_Absyn_None)goto _LL203;_tmp373=_tmp371.f2;_LL202:
# 1284
 _tmp376=_tmp373;goto _LL204;_LL203: if((_tmp369.Int_c).tag != 5)goto _LL205;_tmp374=(struct _tuple5)(_tmp369.Int_c).val;_tmp375=_tmp374.f1;if(_tmp375 != Cyc_Absyn_Signed)goto _LL205;_tmp376=_tmp374.f2;_LL204: {
# 1285
const char*_tmp88C;void*_tmp88B[1];struct Cyc_Int_pa_PrintArg_struct _tmp88A;return Cyc_PP_text((struct _dyneither_ptr)((_tmp88A.tag=1,((_tmp88A.f1=(unsigned long)_tmp376,((_tmp88B[0]=& _tmp88A,Cyc_aprintf(((_tmp88C="%d",_tag_dyneither(_tmp88C,sizeof(char),3))),_tag_dyneither(_tmp88B,sizeof(void*),1)))))))));}_LL205: if((_tmp369.Int_c).tag != 5)goto _LL207;_tmp377=(struct _tuple5)(_tmp369.Int_c).val;_tmp378=_tmp377.f1;if(_tmp378 != Cyc_Absyn_Unsigned)goto _LL207;_tmp379=_tmp377.f2;_LL206: {
# 1286
const char*_tmp890;void*_tmp88F[1];struct Cyc_Int_pa_PrintArg_struct _tmp88E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp88E.tag=1,((_tmp88E.f1=(unsigned int)_tmp379,((_tmp88F[0]=& _tmp88E,Cyc_aprintf(((_tmp890="%u",_tag_dyneither(_tmp890,sizeof(char),3))),_tag_dyneither(_tmp88F,sizeof(void*),1)))))))));}_LL207: if((_tmp369.LongLong_c).tag != 6)goto _LL209;_tmp37A=(struct _tuple6)(_tmp369.LongLong_c).val;_tmp37B=_tmp37A.f1;_tmp37C=_tmp37A.f2;_LL208: {
# 1288
const char*_tmp891;return Cyc_PP_text(((_tmp891="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp891,sizeof(char),27))));}_LL209: if((_tmp369.Float_c).tag != 7)goto _LL20B;_tmp37D=(struct _tuple7)(_tmp369.Float_c).val;_tmp37E=_tmp37D.f1;_LL20A:
# 1289
 return Cyc_PP_text(_tmp37E);_LL20B: if((_tmp369.Null_c).tag != 1)goto _LL20D;_tmp37F=(int)(_tmp369.Null_c).val;_LL20C: {
# 1290
const char*_tmp892;return Cyc_PP_text(((_tmp892="NULL",_tag_dyneither(_tmp892,sizeof(char),5))));}_LL20D: if((_tmp369.String_c).tag != 8)goto _LL20F;_tmp380=(struct _dyneither_ptr)(_tmp369.String_c).val;_LL20E: {
# 1292
const char*_tmp897;const char*_tmp896;struct Cyc_PP_Doc*_tmp895[3];return(_tmp895[2]=Cyc_PP_text(((_tmp896="\"",_tag_dyneither(_tmp896,sizeof(char),2)))),((_tmp895[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp380)),((_tmp895[0]=Cyc_PP_text(((_tmp897="\"",_tag_dyneither(_tmp897,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp895,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL20F: if((_tmp369.Wstring_c).tag != 9)goto _LL1FA;_tmp381=(struct _dyneither_ptr)(_tmp369.Wstring_c).val;_LL210: {
# 1294
const char*_tmp89C;const char*_tmp89B;struct Cyc_PP_Doc*_tmp89A[3];return(_tmp89A[2]=Cyc_PP_text(((_tmp89B="\"",_tag_dyneither(_tmp89B,sizeof(char),2)))),((_tmp89A[1]=Cyc_PP_text(_tmp381),((_tmp89A[0]=Cyc_PP_text(((_tmp89C="L\"",_tag_dyneither(_tmp89C,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp89A,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FA:;}
# 1298
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1299
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
# 1300
if(p == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){
# 1301
if(es == 0  || es->tl != 0){
# 1302
struct Cyc_Core_Failure_exn_struct _tmp8A9;const char*_tmp8A8;void*_tmp8A7[1];struct Cyc_String_pa_PrintArg_struct _tmp8A6;struct Cyc_Core_Failure_exn_struct*_tmp8A5;(int)_throw((void*)((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5[0]=((_tmp8A9.tag=Cyc_Core_Failure,((_tmp8A9.f1=(struct _dyneither_ptr)((_tmp8A6.tag=0,((_tmp8A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8A7[0]=& _tmp8A6,Cyc_aprintf(((_tmp8A8="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp8A8,sizeof(char),47))),_tag_dyneither(_tmp8A7,sizeof(void*),1)))))))),_tmp8A9)))),_tmp8A5)))));}{
# 1304
const char*_tmp8AE;const char*_tmp8AD;struct Cyc_PP_Doc*_tmp8AC[3];return(_tmp8AC[2]=Cyc_PP_text(((_tmp8AD=")",_tag_dyneither(_tmp8AD,sizeof(char),2)))),((_tmp8AC[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp8AC[0]=Cyc_PP_text(((_tmp8AE="numelts(",_tag_dyneither(_tmp8AE,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8AC,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1306
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
# 1307
if(ds == 0){
# 1308
struct Cyc_Core_Failure_exn_struct _tmp8BB;const char*_tmp8BA;void*_tmp8B9[1];struct Cyc_String_pa_PrintArg_struct _tmp8B8;struct Cyc_Core_Failure_exn_struct*_tmp8B7;(int)_throw((void*)((_tmp8B7=_cycalloc(sizeof(*_tmp8B7)),((_tmp8B7[0]=((_tmp8BB.tag=Cyc_Core_Failure,((_tmp8BB.f1=(struct _dyneither_ptr)((_tmp8B8.tag=0,((_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8B9[0]=& _tmp8B8,Cyc_aprintf(((_tmp8BA="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp8BA,sizeof(char),38))),_tag_dyneither(_tmp8B9,sizeof(void*),1)))))))),_tmp8BB)))),_tmp8B7)))));}else{
# 1310
if(ds->tl == 0){
# 1311
const char*_tmp8BE;struct Cyc_PP_Doc*_tmp8BD[3];return(_tmp8BD[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8BD[1]=Cyc_PP_text(((_tmp8BE=" ",_tag_dyneither(_tmp8BE,sizeof(char),2)))),((_tmp8BD[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp8BD,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1312
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
# 1313
struct Cyc_Core_Failure_exn_struct _tmp8CB;const char*_tmp8CA;void*_tmp8C9[1];struct Cyc_String_pa_PrintArg_struct _tmp8C8;struct Cyc_Core_Failure_exn_struct*_tmp8C7;(int)_throw((void*)((_tmp8C7=_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7[0]=((_tmp8CB.tag=Cyc_Core_Failure,((_tmp8CB.f1=(struct _dyneither_ptr)((_tmp8C8.tag=0,((_tmp8C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8C9[0]=& _tmp8C8,Cyc_aprintf(((_tmp8CA="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp8CA,sizeof(char),47))),_tag_dyneither(_tmp8C9,sizeof(void*),1)))))))),_tmp8CB)))),_tmp8C7)))));}else{
# 1316
const char*_tmp8D0;const char*_tmp8CF;struct Cyc_PP_Doc*_tmp8CE[5];return(_tmp8CE[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp8CE[3]=Cyc_PP_text(((_tmp8CF=" ",_tag_dyneither(_tmp8CF,sizeof(char),2)))),((_tmp8CE[2]=ps,((_tmp8CE[1]=Cyc_PP_text(((_tmp8D0=" ",_tag_dyneither(_tmp8D0,sizeof(char),2)))),((_tmp8CE[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1320
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
# 1321
switch(p){case Cyc_Absyn_Plus: _LL211: {
# 1322
const char*_tmp8D1;return(_tmp8D1="+",_tag_dyneither(_tmp8D1,sizeof(char),2));}case Cyc_Absyn_Times: _LL212: {
# 1323
const char*_tmp8D2;return(_tmp8D2="*",_tag_dyneither(_tmp8D2,sizeof(char),2));}case Cyc_Absyn_Minus: _LL213: {
# 1324
const char*_tmp8D3;return(_tmp8D3="-",_tag_dyneither(_tmp8D3,sizeof(char),2));}case Cyc_Absyn_Div: _LL214: {
# 1325
const char*_tmp8D4;return(_tmp8D4="/",_tag_dyneither(_tmp8D4,sizeof(char),2));}case Cyc_Absyn_Mod: _LL215: {
# 1326
const char*_tmp8D6;const char*_tmp8D5;return Cyc_Absynpp_print_for_cycdoc?(_tmp8D6="\\%",_tag_dyneither(_tmp8D6,sizeof(char),3)):((_tmp8D5="%",_tag_dyneither(_tmp8D5,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL216: {
# 1327
const char*_tmp8D7;return(_tmp8D7="==",_tag_dyneither(_tmp8D7,sizeof(char),3));}case Cyc_Absyn_Neq: _LL217: {
# 1328
const char*_tmp8D8;return(_tmp8D8="!=",_tag_dyneither(_tmp8D8,sizeof(char),3));}case Cyc_Absyn_Gt: _LL218: {
# 1329
const char*_tmp8D9;return(_tmp8D9=">",_tag_dyneither(_tmp8D9,sizeof(char),2));}case Cyc_Absyn_Lt: _LL219: {
# 1330
const char*_tmp8DA;return(_tmp8DA="<",_tag_dyneither(_tmp8DA,sizeof(char),2));}case Cyc_Absyn_Gte: _LL21A: {
# 1331
const char*_tmp8DB;return(_tmp8DB=">=",_tag_dyneither(_tmp8DB,sizeof(char),3));}case Cyc_Absyn_Lte: _LL21B: {
# 1332
const char*_tmp8DC;return(_tmp8DC="<=",_tag_dyneither(_tmp8DC,sizeof(char),3));}case Cyc_Absyn_Not: _LL21C: {
# 1333
const char*_tmp8DD;return(_tmp8DD="!",_tag_dyneither(_tmp8DD,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL21D: {
# 1334
const char*_tmp8DE;return(_tmp8DE="~",_tag_dyneither(_tmp8DE,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL21E: {
# 1335
const char*_tmp8DF;return(_tmp8DF="&",_tag_dyneither(_tmp8DF,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL21F: {
# 1336
const char*_tmp8E0;return(_tmp8E0="|",_tag_dyneither(_tmp8E0,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL220: {
# 1337
const char*_tmp8E1;return(_tmp8E1="^",_tag_dyneither(_tmp8E1,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL221: {
# 1338
const char*_tmp8E2;return(_tmp8E2="<<",_tag_dyneither(_tmp8E2,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL222: {
# 1339
const char*_tmp8E3;return(_tmp8E3=">>",_tag_dyneither(_tmp8E3,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL223: {
# 1340
const char*_tmp8E4;return(_tmp8E4=">>>",_tag_dyneither(_tmp8E4,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL224: {
# 1341
const char*_tmp8E5;return(_tmp8E5="numelts",_tag_dyneither(_tmp8E5,sizeof(char),8));}}}
# 1345
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
# 1346
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1349
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
# 1350
void*_tmp3C5=s->r;_LL227: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3C6=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C5;if(_tmp3C6->tag != 12)goto _LL229;}_LL228:
# 1351
 return 1;_LL229:;_LL22A:
# 1352
 return 0;_LL226:;}
# 1356
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
# 1357
struct Cyc_PP_Doc*s;
# 1358
{void*_tmp3C7=st->r;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_Absyn_Stmt*_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D2;struct Cyc_Absyn_Stmt*_tmp3D3;struct _tuple9 _tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Stmt*_tmp3D7;struct _dyneither_ptr*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DD;struct _tuple9 _tmp3DE;struct Cyc_Absyn_Exp*_tmp3DF;struct _tuple9 _tmp3E0;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Stmt*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_List_List*_tmp3E5;struct Cyc_List_List*_tmp3E7;struct Cyc_List_List*_tmp3E9;struct Cyc_Absyn_Decl*_tmp3EB;struct Cyc_Absyn_Stmt*_tmp3EC;struct _dyneither_ptr*_tmp3EE;struct Cyc_Absyn_Stmt*_tmp3EF;struct Cyc_Absyn_Stmt*_tmp3F1;struct _tuple9 _tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;struct Cyc_Absyn_Stmt*_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F8;_LL22C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3C8=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3C8->tag != 0)goto _LL22E;}_LL22D:
# 1359
{const char*_tmp8E6;s=Cyc_PP_text(((_tmp8E6=";",_tag_dyneither(_tmp8E6,sizeof(char),2))));}goto _LL22B;_LL22E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3C9=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3C9->tag != 1)goto _LL230;else{_tmp3CA=_tmp3C9->f1;}}_LL22F:
# 1360
{const char*_tmp8E9;struct Cyc_PP_Doc*_tmp8E8[2];s=((_tmp8E8[1]=Cyc_PP_text(((_tmp8E9=";",_tag_dyneither(_tmp8E9,sizeof(char),2)))),((_tmp8E8[0]=Cyc_Absynpp_exp2doc(_tmp3CA),Cyc_PP_cat(_tag_dyneither(_tmp8E8,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL22B;_LL230: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3CB=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3CB->tag != 2)goto _LL232;else{_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;}}_LL231:
# 1362
 if(Cyc_Absynpp_decls_first){
# 1363
if(Cyc_Absynpp_is_declaration(_tmp3CC)){
# 1364
struct Cyc_PP_Doc*_tmp8F4[5];struct Cyc_PP_Doc*_tmp8F3[7];const char*_tmp8F2;const char*_tmp8F1;struct Cyc_PP_Doc*_tmp8F0[7];s=((_tmp8F0[6]=
# 1370
Cyc_Absynpp_is_declaration(_tmp3CD)?
# 1371
stmtexp?
# 1372
(_tmp8F3[6]=
# 1375
Cyc_PP_line_doc(),((_tmp8F3[5]=
# 1374
Cyc_PP_text(((_tmp8F2=");",_tag_dyneither(_tmp8F2,sizeof(char),3)))),((_tmp8F3[4]=Cyc_Absynpp_rb(),((_tmp8F3[3]=
# 1373
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CD,stmtexp)),((_tmp8F3[2]=
# 1372
Cyc_PP_blank_doc(),((_tmp8F3[1]=Cyc_Absynpp_lb(),((_tmp8F3[0]=Cyc_PP_text(((_tmp8F1="(",_tag_dyneither(_tmp8F1,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1376
(_tmp8F4[4]=
# 1379
Cyc_PP_line_doc(),((_tmp8F4[3]=
# 1378
Cyc_Absynpp_rb(),((_tmp8F4[2]=
# 1377
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CD,stmtexp)),((_tmp8F4[1]=
# 1376
Cyc_PP_blank_doc(),((_tmp8F4[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8F4,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1380
 Cyc_Absynpp_stmt2doc(_tmp3CD,stmtexp),((_tmp8F0[5]=
# 1369
Cyc_PP_line_doc(),((_tmp8F0[4]=
# 1368
Cyc_Absynpp_rb(),((_tmp8F0[3]=
# 1367
Cyc_PP_line_doc(),((_tmp8F0[2]=
# 1366
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CC,0)),((_tmp8F0[1]=
# 1365
Cyc_PP_blank_doc(),((_tmp8F0[0]=
# 1364
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8F0,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1381
if(Cyc_Absynpp_is_declaration(_tmp3CD)){
# 1382
struct Cyc_PP_Doc*_tmp8FF[4];struct Cyc_PP_Doc*_tmp8FE[6];const char*_tmp8FD;const char*_tmp8FC;struct Cyc_PP_Doc*_tmp8FB[4];s=((_tmp8FB[3]=
# 1392
Cyc_PP_line_doc(),((_tmp8FB[2]=
# 1384
stmtexp?
# 1385
(_tmp8FE[5]=
# 1387
Cyc_PP_text(((_tmp8FD=");",_tag_dyneither(_tmp8FD,sizeof(char),3)))),((_tmp8FE[4]=Cyc_Absynpp_rb(),((_tmp8FE[3]=
# 1386
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CD,stmtexp)),((_tmp8FE[2]=
# 1385
Cyc_PP_blank_doc(),((_tmp8FE[1]=Cyc_Absynpp_lb(),((_tmp8FE[0]=Cyc_PP_text(((_tmp8FC="(",_tag_dyneither(_tmp8FC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8FE,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1389
(_tmp8FF[3]=
# 1391
Cyc_Absynpp_rb(),((_tmp8FF[2]=
# 1390
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CD,stmtexp)),((_tmp8FF[1]=
# 1389
Cyc_PP_blank_doc(),((_tmp8FF[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8FF,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8FB[1]=
# 1383
Cyc_PP_line_doc(),((_tmp8FB[0]=
# 1382
Cyc_Absynpp_stmt2doc(_tmp3CC,0),Cyc_PP_cat(_tag_dyneither(_tmp8FB,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1394
struct Cyc_PP_Doc*_tmp900[3];s=((_tmp900[2]=Cyc_Absynpp_stmt2doc(_tmp3CD,stmtexp),((_tmp900[1]=Cyc_PP_line_doc(),((_tmp900[0]=Cyc_Absynpp_stmt2doc(_tmp3CC,0),Cyc_PP_cat(_tag_dyneither(_tmp900,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1397
struct Cyc_PP_Doc*_tmp901[3];s=((_tmp901[2]=Cyc_Absynpp_stmt2doc(_tmp3CD,stmtexp),((_tmp901[1]=Cyc_PP_line_doc(),((_tmp901[0]=Cyc_Absynpp_stmt2doc(_tmp3CC,0),Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1398
goto _LL22B;_LL232: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3CE=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3CE->tag != 3)goto _LL234;else{_tmp3CF=_tmp3CE->f1;}}_LL233:
# 1400
 if(_tmp3CF == 0){
# 1401
const char*_tmp902;s=Cyc_PP_text(((_tmp902="return;",_tag_dyneither(_tmp902,sizeof(char),8))));}else{
# 1403
const char*_tmp907;const char*_tmp906;struct Cyc_PP_Doc*_tmp905[3];s=((_tmp905[2]=
# 1405
Cyc_PP_text(((_tmp906=";",_tag_dyneither(_tmp906,sizeof(char),2)))),((_tmp905[1]=
# 1404
_tmp3CF == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3CF),((_tmp905[0]=
# 1403
Cyc_PP_text(((_tmp907="return ",_tag_dyneither(_tmp907,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp905,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1406
goto _LL22B;_LL234: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3D0=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3D0->tag != 4)goto _LL236;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;_tmp3D3=_tmp3D0->f3;}}_LL235: {
# 1408
int print_else;
# 1409
{void*_tmp40C=_tmp3D3->r;_LL251: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp40D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp40C;if(_tmp40D->tag != 0)goto _LL253;}_LL252:
# 1410
 print_else=0;goto _LL250;_LL253:;_LL254:
# 1411
 print_else=1;goto _LL250;_LL250:;}
# 1413
{const char*_tmp916;const char*_tmp915;struct Cyc_PP_Doc*_tmp914[2];struct Cyc_PP_Doc*_tmp913[6];struct Cyc_PP_Doc*_tmp912[2];const char*_tmp911;struct Cyc_PP_Doc*_tmp910[8];s=((_tmp910[7]=
# 1420
print_else?
# 1421
(_tmp913[5]=
# 1426
Cyc_Absynpp_rb(),((_tmp913[4]=
# 1425
Cyc_PP_line_doc(),((_tmp913[3]=
# 1424
Cyc_PP_nest(2,((_tmp912[1]=Cyc_Absynpp_stmt2doc(_tmp3D3,0),((_tmp912[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp912,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp913[2]=
# 1423
Cyc_Absynpp_lb(),((_tmp913[1]=
# 1422
Cyc_PP_text(((_tmp911="else ",_tag_dyneither(_tmp911,sizeof(char),6)))),((_tmp913[0]=
# 1421
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp913,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1427
 Cyc_PP_nil_doc(),((_tmp910[6]=
# 1419
Cyc_Absynpp_rb(),((_tmp910[5]=
# 1418
Cyc_PP_line_doc(),((_tmp910[4]=
# 1417
Cyc_PP_nest(2,((_tmp914[1]=Cyc_Absynpp_stmt2doc(_tmp3D2,0),((_tmp914[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp914,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp910[3]=
# 1416
Cyc_Absynpp_lb(),((_tmp910[2]=
# 1415
Cyc_PP_text(((_tmp915=") ",_tag_dyneither(_tmp915,sizeof(char),3)))),((_tmp910[1]=
# 1414
Cyc_Absynpp_exp2doc(_tmp3D1),((_tmp910[0]=
# 1413
Cyc_PP_text(((_tmp916="if (",_tag_dyneither(_tmp916,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp910,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1428
goto _LL22B;}_LL236: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3D4=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3D4->tag != 5)goto _LL238;else{_tmp3D5=_tmp3D4->f1;_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D4->f2;}}_LL237:
# 1430
{const char*_tmp91D;const char*_tmp91C;struct Cyc_PP_Doc*_tmp91B[2];struct Cyc_PP_Doc*_tmp91A[7];s=((_tmp91A[6]=
# 1436
Cyc_Absynpp_rb(),((_tmp91A[5]=
# 1435
Cyc_PP_line_doc(),((_tmp91A[4]=
# 1434
Cyc_PP_nest(2,((_tmp91B[1]=Cyc_Absynpp_stmt2doc(_tmp3D7,0),((_tmp91B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp91A[3]=
# 1433
Cyc_Absynpp_lb(),((_tmp91A[2]=
# 1432
Cyc_PP_text(((_tmp91C=") ",_tag_dyneither(_tmp91C,sizeof(char),3)))),((_tmp91A[1]=
# 1431
Cyc_Absynpp_exp2doc(_tmp3D6),((_tmp91A[0]=
# 1430
Cyc_PP_text(((_tmp91D="while (",_tag_dyneither(_tmp91D,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp91A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1437
goto _LL22B;_LL238: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3D8=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3D8->tag != 6)goto _LL23A;}_LL239:
# 1438
{const char*_tmp91E;s=Cyc_PP_text(((_tmp91E="break;",_tag_dyneither(_tmp91E,sizeof(char),7))));}goto _LL22B;_LL23A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3D9=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3D9->tag != 7)goto _LL23C;}_LL23B:
# 1439
{const char*_tmp91F;s=Cyc_PP_text(((_tmp91F="continue;",_tag_dyneither(_tmp91F,sizeof(char),10))));}goto _LL22B;_LL23C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3DA=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3DA->tag != 8)goto _LL23E;else{_tmp3DB=_tmp3DA->f1;}}_LL23D:
# 1440
{const char*_tmp923;void*_tmp922[1];struct Cyc_String_pa_PrintArg_struct _tmp921;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp921.tag=0,((_tmp921.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3DB),((_tmp922[0]=& _tmp921,Cyc_aprintf(((_tmp923="goto %s;",_tag_dyneither(_tmp923,sizeof(char),9))),_tag_dyneither(_tmp922,sizeof(void*),1)))))))));}goto _LL22B;_LL23E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3DC=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3DC->tag != 9)goto _LL240;else{_tmp3DD=_tmp3DC->f1;_tmp3DE=_tmp3DC->f2;_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DC->f3;_tmp3E1=_tmp3E0.f1;_tmp3E2=_tmp3DC->f4;}}_LL23F:
# 1442
{const char*_tmp92E;const char*_tmp92D;const char*_tmp92C;const char*_tmp92B;struct Cyc_PP_Doc*_tmp92A[2];struct Cyc_PP_Doc*_tmp929[11];s=((_tmp929[10]=
# 1452
Cyc_Absynpp_rb(),((_tmp929[9]=
# 1451
Cyc_PP_line_doc(),((_tmp929[8]=
# 1450
Cyc_PP_nest(2,((_tmp92A[1]=Cyc_Absynpp_stmt2doc(_tmp3E2,0),((_tmp92A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp92A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp929[7]=
# 1449
Cyc_Absynpp_lb(),((_tmp929[6]=
# 1448
Cyc_PP_text(((_tmp92B=") ",_tag_dyneither(_tmp92B,sizeof(char),3)))),((_tmp929[5]=
# 1447
Cyc_Absynpp_exp2doc(_tmp3E1),((_tmp929[4]=
# 1446
Cyc_PP_text(((_tmp92C="; ",_tag_dyneither(_tmp92C,sizeof(char),3)))),((_tmp929[3]=
# 1445
Cyc_Absynpp_exp2doc(_tmp3DF),((_tmp929[2]=
# 1444
Cyc_PP_text(((_tmp92D="; ",_tag_dyneither(_tmp92D,sizeof(char),3)))),((_tmp929[1]=
# 1443
Cyc_Absynpp_exp2doc(_tmp3DD),((_tmp929[0]=
# 1442
Cyc_PP_text(((_tmp92E="for(",_tag_dyneither(_tmp92E,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp929,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1453
goto _LL22B;_LL240: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3E3=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3E3->tag != 10)goto _LL242;else{_tmp3E4=_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;}}_LL241:
# 1455
{const char*_tmp933;const char*_tmp932;struct Cyc_PP_Doc*_tmp931[8];s=((_tmp931[7]=
# 1462
Cyc_Absynpp_rb(),((_tmp931[6]=
# 1461
Cyc_PP_line_doc(),((_tmp931[5]=
# 1460
Cyc_Absynpp_switchclauses2doc(_tmp3E5),((_tmp931[4]=
# 1459
Cyc_PP_line_doc(),((_tmp931[3]=
# 1458
Cyc_Absynpp_lb(),((_tmp931[2]=
# 1457
Cyc_PP_text(((_tmp932=") ",_tag_dyneither(_tmp932,sizeof(char),3)))),((_tmp931[1]=
# 1456
Cyc_Absynpp_exp2doc(_tmp3E4),((_tmp931[0]=
# 1455
Cyc_PP_text(((_tmp933="switch (",_tag_dyneither(_tmp933,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp931,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1463
goto _LL22B;_LL242: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3E6=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3E6->tag != 11)goto _LL244;else{_tmp3E7=_tmp3E6->f1;if(_tmp3E7 != 0)goto _LL244;}}_LL243:
# 1464
{const char*_tmp934;s=Cyc_PP_text(((_tmp934="fallthru;",_tag_dyneither(_tmp934,sizeof(char),10))));}goto _LL22B;_LL244: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3E8=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3E8->tag != 11)goto _LL246;else{_tmp3E9=_tmp3E8->f1;}}_LL245:
# 1466
{const char*_tmp939;const char*_tmp938;struct Cyc_PP_Doc*_tmp937[3];s=((_tmp937[2]=Cyc_PP_text(((_tmp938=");",_tag_dyneither(_tmp938,sizeof(char),3)))),((_tmp937[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3E9),((_tmp937[0]=Cyc_PP_text(((_tmp939="fallthru(",_tag_dyneither(_tmp939,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp937,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22B;_LL246: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3EA=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3EA->tag != 12)goto _LL248;else{_tmp3EB=_tmp3EA->f1;_tmp3EC=_tmp3EA->f2;}}_LL247:
# 1468
{struct Cyc_PP_Doc*_tmp93A[3];s=((_tmp93A[2]=
# 1470
Cyc_Absynpp_stmt2doc(_tmp3EC,stmtexp),((_tmp93A[1]=
# 1469
Cyc_PP_line_doc(),((_tmp93A[0]=
# 1468
Cyc_Absynpp_decl2doc(_tmp3EB),Cyc_PP_cat(_tag_dyneither(_tmp93A,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1471
goto _LL22B;_LL248: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3ED=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3ED->tag != 13)goto _LL24A;else{_tmp3EE=_tmp3ED->f1;_tmp3EF=_tmp3ED->f2;}}_LL249:
# 1473
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3EF)){
# 1474
if(stmtexp){
# 1475
const char*_tmp93F;const char*_tmp93E;struct Cyc_PP_Doc*_tmp93D[8];s=((_tmp93D[7]=
# 1481
Cyc_PP_text(((_tmp93E=");",_tag_dyneither(_tmp93E,sizeof(char),3)))),((_tmp93D[6]=Cyc_Absynpp_rb(),((_tmp93D[5]=
# 1480
Cyc_PP_line_doc(),((_tmp93D[4]=
# 1479
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3EF,1)),((_tmp93D[3]=
# 1478
Cyc_PP_line_doc(),((_tmp93D[2]=
# 1477
Cyc_Absynpp_lb(),((_tmp93D[1]=
# 1476
Cyc_PP_text(((_tmp93F=": (",_tag_dyneither(_tmp93F,sizeof(char),4)))),((_tmp93D[0]=
# 1475
Cyc_PP_textptr(_tmp3EE),Cyc_PP_cat(_tag_dyneither(_tmp93D,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1483
const char*_tmp942;struct Cyc_PP_Doc*_tmp941[7];s=((_tmp941[6]=
# 1489
Cyc_Absynpp_rb(),((_tmp941[5]=
# 1488
Cyc_PP_line_doc(),((_tmp941[4]=
# 1487
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3EF,0)),((_tmp941[3]=
# 1486
Cyc_PP_line_doc(),((_tmp941[2]=
# 1485
Cyc_Absynpp_lb(),((_tmp941[1]=
# 1484
Cyc_PP_text(((_tmp942=": ",_tag_dyneither(_tmp942,sizeof(char),3)))),((_tmp941[0]=
# 1483
Cyc_PP_textptr(_tmp3EE),Cyc_PP_cat(_tag_dyneither(_tmp941,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1491
const char*_tmp945;struct Cyc_PP_Doc*_tmp944[3];s=((_tmp944[2]=Cyc_Absynpp_stmt2doc(_tmp3EF,stmtexp),((_tmp944[1]=Cyc_PP_text(((_tmp945=": ",_tag_dyneither(_tmp945,sizeof(char),3)))),((_tmp944[0]=Cyc_PP_textptr(_tmp3EE),Cyc_PP_cat(_tag_dyneither(_tmp944,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1492
goto _LL22B;_LL24A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3F0=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3F0->tag != 14)goto _LL24C;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F0->f2;_tmp3F3=_tmp3F2.f1;}}_LL24B:
# 1494
{const char*_tmp94C;const char*_tmp94B;const char*_tmp94A;struct Cyc_PP_Doc*_tmp949[9];s=((_tmp949[8]=
# 1502
Cyc_PP_text(((_tmp94A=");",_tag_dyneither(_tmp94A,sizeof(char),3)))),((_tmp949[7]=
# 1501
Cyc_Absynpp_exp2doc(_tmp3F3),((_tmp949[6]=
# 1500
Cyc_PP_text(((_tmp94B=" while (",_tag_dyneither(_tmp94B,sizeof(char),9)))),((_tmp949[5]=
# 1499
Cyc_Absynpp_rb(),((_tmp949[4]=
# 1498
Cyc_PP_line_doc(),((_tmp949[3]=
# 1497
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F1,0)),((_tmp949[2]=
# 1496
Cyc_PP_line_doc(),((_tmp949[1]=
# 1495
Cyc_Absynpp_lb(),((_tmp949[0]=
# 1494
Cyc_PP_text(((_tmp94C="do ",_tag_dyneither(_tmp94C,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp949,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1503
goto _LL22B;_LL24C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3F4=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3F4->tag != 15)goto _LL24E;else{_tmp3F5=_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;}}_LL24D:
# 1505
{const char*_tmp951;const char*_tmp950;struct Cyc_PP_Doc*_tmp94F[12];s=((_tmp94F[11]=
# 1516
Cyc_Absynpp_rb(),((_tmp94F[10]=
# 1515
Cyc_PP_line_doc(),((_tmp94F[9]=
# 1514
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3F6)),((_tmp94F[8]=
# 1513
Cyc_PP_line_doc(),((_tmp94F[7]=
# 1512
Cyc_Absynpp_lb(),((_tmp94F[6]=
# 1511
Cyc_PP_text(((_tmp950=" catch ",_tag_dyneither(_tmp950,sizeof(char),8)))),((_tmp94F[5]=
# 1510
Cyc_Absynpp_rb(),((_tmp94F[4]=
# 1509
Cyc_PP_line_doc(),((_tmp94F[3]=
# 1508
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3F5,0)),((_tmp94F[2]=
# 1507
Cyc_PP_line_doc(),((_tmp94F[1]=
# 1506
Cyc_Absynpp_lb(),((_tmp94F[0]=
# 1505
Cyc_PP_text(((_tmp951="try ",_tag_dyneither(_tmp951,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp94F,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1517
goto _LL22B;_LL24E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3F7=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3C7;if(_tmp3F7->tag != 16)goto _LL22B;else{_tmp3F8=_tmp3F7->f1;}}_LL24F:
# 1519
{const char*_tmp956;const char*_tmp955;struct Cyc_PP_Doc*_tmp954[3];s=((_tmp954[2]=Cyc_PP_text(((_tmp955=");",_tag_dyneither(_tmp955,sizeof(char),3)))),((_tmp954[1]=Cyc_Absynpp_exp2doc(_tmp3F8),((_tmp954[0]=Cyc_PP_text(((_tmp956="reset_region(",_tag_dyneither(_tmp956,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp954,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1520
goto _LL22B;_LL22B:;}
# 1522
return s;}
# 1525
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
# 1526
struct Cyc_PP_Doc*s;
# 1527
{void*_tmp43D=p->r;enum Cyc_Absyn_Sign _tmp441;int _tmp442;char _tmp444;struct _dyneither_ptr _tmp446;struct Cyc_Absyn_Vardecl*_tmp448;struct Cyc_Absyn_Pat*_tmp449;struct Cyc_Absyn_Pat _tmp44A;void*_tmp44B;struct Cyc_Absyn_Vardecl*_tmp44E;struct Cyc_Absyn_Pat*_tmp44F;struct Cyc_Absyn_Tvar*_tmp451;struct Cyc_Absyn_Vardecl*_tmp452;struct Cyc_List_List*_tmp454;int _tmp455;struct Cyc_Absyn_Pat*_tmp457;struct Cyc_Absyn_Vardecl*_tmp459;struct Cyc_Absyn_Pat*_tmp45A;struct Cyc_Absyn_Pat _tmp45B;void*_tmp45C;struct Cyc_Absyn_Vardecl*_tmp45F;struct Cyc_Absyn_Pat*_tmp460;struct _tuple0*_tmp462;struct _tuple0*_tmp464;struct Cyc_List_List*_tmp465;int _tmp466;struct Cyc_Absyn_AggrInfo*_tmp468;struct Cyc_Absyn_AggrInfo _tmp469;union Cyc_Absyn_AggrInfoU _tmp46A;struct Cyc_List_List*_tmp46B;struct Cyc_List_List*_tmp46C;int _tmp46D;struct Cyc_Absyn_AggrInfo*_tmp46F;struct Cyc_List_List*_tmp470;struct Cyc_List_List*_tmp471;int _tmp472;struct Cyc_Absyn_Enumfield*_tmp474;struct Cyc_Absyn_Enumfield*_tmp476;struct Cyc_Absyn_Datatypefield*_tmp478;struct Cyc_List_List*_tmp479;struct Cyc_Absyn_Datatypefield*_tmp47B;struct Cyc_List_List*_tmp47C;int _tmp47D;struct Cyc_Absyn_Exp*_tmp47F;_LL256: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp43E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp43E->tag != 0)goto _LL258;}_LL257:
# 1528
{const char*_tmp957;s=Cyc_PP_text(((_tmp957="_",_tag_dyneither(_tmp957,sizeof(char),2))));}goto _LL255;_LL258: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp43F=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp43F->tag != 8)goto _LL25A;}_LL259:
# 1529
{const char*_tmp958;s=Cyc_PP_text(((_tmp958="NULL",_tag_dyneither(_tmp958,sizeof(char),5))));}goto _LL255;_LL25A: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp440=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp440->tag != 9)goto _LL25C;else{_tmp441=_tmp440->f1;_tmp442=_tmp440->f2;}}_LL25B:
# 1531
 if(_tmp441 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){
# 1532
const char*_tmp95C;void*_tmp95B[1];struct Cyc_Int_pa_PrintArg_struct _tmp95A;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp95A.tag=1,((_tmp95A.f1=(unsigned long)_tmp442,((_tmp95B[0]=& _tmp95A,Cyc_aprintf(((_tmp95C="%d",_tag_dyneither(_tmp95C,sizeof(char),3))),_tag_dyneither(_tmp95B,sizeof(void*),1)))))))));}else{
# 1533
const char*_tmp960;void*_tmp95F[1];struct Cyc_Int_pa_PrintArg_struct _tmp95E;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp95E.tag=1,((_tmp95E.f1=(unsigned int)_tmp442,((_tmp95F[0]=& _tmp95E,Cyc_aprintf(((_tmp960="%u",_tag_dyneither(_tmp960,sizeof(char),3))),_tag_dyneither(_tmp95F,sizeof(void*),1)))))))));}
# 1534
goto _LL255;_LL25C: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp443=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp443->tag != 10)goto _LL25E;else{_tmp444=_tmp443->f1;}}_LL25D:
# 1535
{const char*_tmp964;void*_tmp963[1];struct Cyc_String_pa_PrintArg_struct _tmp962;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp962.tag=0,((_tmp962.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp444)),((_tmp963[0]=& _tmp962,Cyc_aprintf(((_tmp964="'%s'",_tag_dyneither(_tmp964,sizeof(char),5))),_tag_dyneither(_tmp963,sizeof(void*),1)))))))));}goto _LL255;_LL25E: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp445=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp445->tag != 11)goto _LL260;else{_tmp446=_tmp445->f1;}}_LL25F:
# 1536
 s=Cyc_PP_text(_tmp446);goto _LL255;_LL260: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp447=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp447->tag != 1)goto _LL262;else{_tmp448=_tmp447->f1;_tmp449=_tmp447->f2;_tmp44A=*_tmp449;_tmp44B=_tmp44A.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp44C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp44B;if(_tmp44C->tag != 0)goto _LL262;};}}_LL261:
# 1538
 s=Cyc_Absynpp_qvar2doc(_tmp448->name);goto _LL255;_LL262: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp44D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp44D->tag != 1)goto _LL264;else{_tmp44E=_tmp44D->f1;_tmp44F=_tmp44D->f2;}}_LL263:
# 1541
{const char*_tmp967;struct Cyc_PP_Doc*_tmp966[3];s=((_tmp966[2]=Cyc_Absynpp_pat2doc(_tmp44F),((_tmp966[1]=Cyc_PP_text(((_tmp967=" as ",_tag_dyneither(_tmp967,sizeof(char),5)))),((_tmp966[0]=Cyc_Absynpp_qvar2doc(_tmp44E->name),Cyc_PP_cat(_tag_dyneither(_tmp966,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL255;_LL264: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp450=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp450->tag != 3)goto _LL266;else{_tmp451=_tmp450->f1;_tmp452=_tmp450->f2;}}_LL265:
# 1543
{const char*_tmp96C;const char*_tmp96B;struct Cyc_PP_Doc*_tmp96A[4];s=((_tmp96A[3]=Cyc_PP_text(((_tmp96B=">",_tag_dyneither(_tmp96B,sizeof(char),2)))),((_tmp96A[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp451)),((_tmp96A[1]=Cyc_PP_text(((_tmp96C="<",_tag_dyneither(_tmp96C,sizeof(char),2)))),((_tmp96A[0]=Cyc_Absynpp_qvar2doc(_tmp452->name),Cyc_PP_cat(_tag_dyneither(_tmp96A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1544
goto _LL255;_LL266: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp453=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp453->tag != 4)goto _LL268;else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}}_LL267:
# 1546
{const char*_tmp975;const char*_tmp974;const char*_tmp973;const char*_tmp972;struct Cyc_PP_Doc*_tmp971[4];s=((_tmp971[3]=
# 1547
_tmp455?Cyc_PP_text(((_tmp972=", ...)",_tag_dyneither(_tmp972,sizeof(char),7)))): Cyc_PP_text(((_tmp973=")",_tag_dyneither(_tmp973,sizeof(char),2)))),((_tmp971[2]=
# 1546
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp974=",",_tag_dyneither(_tmp974,sizeof(char),2))),_tmp454),((_tmp971[1]=Cyc_PP_text(((_tmp975="(",_tag_dyneither(_tmp975,sizeof(char),2)))),((_tmp971[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp971,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1548
goto _LL255;_LL268: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp456=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp456->tag != 5)goto _LL26A;else{_tmp457=_tmp456->f1;}}_LL269:
# 1550
{const char*_tmp978;struct Cyc_PP_Doc*_tmp977[2];s=((_tmp977[1]=Cyc_Absynpp_pat2doc(_tmp457),((_tmp977[0]=Cyc_PP_text(((_tmp978="&",_tag_dyneither(_tmp978,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp977,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1551
goto _LL255;_LL26A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp458=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp458->tag != 2)goto _LL26C;else{_tmp459=_tmp458->f1;_tmp45A=_tmp458->f2;_tmp45B=*_tmp45A;_tmp45C=_tmp45B.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp45D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp45C;if(_tmp45D->tag != 0)goto _LL26C;};}}_LL26B:
# 1553
{const char*_tmp97B;struct Cyc_PP_Doc*_tmp97A[2];s=((_tmp97A[1]=Cyc_Absynpp_qvar2doc(_tmp459->name),((_tmp97A[0]=Cyc_PP_text(((_tmp97B="*",_tag_dyneither(_tmp97B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp97A,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1554
goto _LL255;_LL26C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp45E=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp45E->tag != 2)goto _LL26E;else{_tmp45F=_tmp45E->f1;_tmp460=_tmp45E->f2;}}_LL26D:
# 1556
{const char*_tmp980;const char*_tmp97F;struct Cyc_PP_Doc*_tmp97E[4];s=((_tmp97E[3]=Cyc_Absynpp_pat2doc(_tmp460),((_tmp97E[2]=Cyc_PP_text(((_tmp97F=" as ",_tag_dyneither(_tmp97F,sizeof(char),5)))),((_tmp97E[1]=Cyc_Absynpp_qvar2doc(_tmp45F->name),((_tmp97E[0]=Cyc_PP_text(((_tmp980="*",_tag_dyneither(_tmp980,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp97E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1557
goto _LL255;_LL26E: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp461=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp461->tag != 14)goto _LL270;else{_tmp462=_tmp461->f1;}}_LL26F:
# 1559
 s=Cyc_Absynpp_qvar2doc(_tmp462);
# 1560
goto _LL255;_LL270: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp463=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp463->tag != 15)goto _LL272;else{_tmp464=_tmp463->f1;_tmp465=_tmp463->f2;_tmp466=_tmp463->f3;}}_LL271: {
# 1562
const char*_tmp982;const char*_tmp981;struct _dyneither_ptr term=_tmp466?(_tmp982=", ...)",_tag_dyneither(_tmp982,sizeof(char),7)):((_tmp981=")",_tag_dyneither(_tmp981,sizeof(char),2)));
# 1563
{const char*_tmp987;const char*_tmp986;struct Cyc_PP_Doc*_tmp985[2];s=((_tmp985[1]=Cyc_PP_group(((_tmp987="(",_tag_dyneither(_tmp987,sizeof(char),2))),term,((_tmp986=",",_tag_dyneither(_tmp986,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp465)),((_tmp985[0]=Cyc_Absynpp_qvar2doc(_tmp464),Cyc_PP_cat(_tag_dyneither(_tmp985,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1564
goto _LL255;}_LL272: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp467=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp467->tag != 6)goto _LL274;else{_tmp468=_tmp467->f1;if(_tmp468 == 0)goto _LL274;_tmp469=*_tmp468;_tmp46A=_tmp469.aggr_info;_tmp46B=_tmp467->f2;_tmp46C=_tmp467->f3;_tmp46D=_tmp467->f4;}}_LL273: {
# 1566
const char*_tmp989;const char*_tmp988;struct _dyneither_ptr term=_tmp46D?(_tmp989=", ...}",_tag_dyneither(_tmp989,sizeof(char),7)):((_tmp988="}",_tag_dyneither(_tmp988,sizeof(char),2)));
# 1567
struct _tuple0*_tmp4A2;struct _tuple10 _tmp4A1=Cyc_Absyn_aggr_kinded_name(_tmp46A);_tmp4A2=_tmp4A1.f2;
# 1568
{const char*_tmp994;const char*_tmp993;const char*_tmp992;const char*_tmp991;const char*_tmp990;struct Cyc_PP_Doc*_tmp98F[4];s=((_tmp98F[3]=
# 1570
Cyc_PP_group(((_tmp991="",_tag_dyneither(_tmp991,sizeof(char),1))),term,((_tmp990=",",_tag_dyneither(_tmp990,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp46C)),((_tmp98F[2]=
# 1569
Cyc_PP_egroup(((_tmp994="[",_tag_dyneither(_tmp994,sizeof(char),2))),((_tmp993="]",_tag_dyneither(_tmp993,sizeof(char),2))),((_tmp992=",",_tag_dyneither(_tmp992,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp46B))),((_tmp98F[1]=
# 1568
Cyc_Absynpp_lb(),((_tmp98F[0]=Cyc_Absynpp_qvar2doc(_tmp4A2),Cyc_PP_cat(_tag_dyneither(_tmp98F,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1571
goto _LL255;}_LL274: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp46E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp46E->tag != 6)goto _LL276;else{_tmp46F=_tmp46E->f1;if(_tmp46F != 0)goto _LL276;_tmp470=_tmp46E->f2;_tmp471=_tmp46E->f3;_tmp472=_tmp46E->f4;}}_LL275: {
# 1573
const char*_tmp996;const char*_tmp995;struct _dyneither_ptr term=_tmp472?(_tmp996=", ...}",_tag_dyneither(_tmp996,sizeof(char),7)):((_tmp995="}",_tag_dyneither(_tmp995,sizeof(char),2)));
# 1574
{const char*_tmp9A1;const char*_tmp9A0;const char*_tmp99F;const char*_tmp99E;const char*_tmp99D;struct Cyc_PP_Doc*_tmp99C[3];s=((_tmp99C[2]=
# 1576
Cyc_PP_group(((_tmp99E="",_tag_dyneither(_tmp99E,sizeof(char),1))),term,((_tmp99D=",",_tag_dyneither(_tmp99D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp471)),((_tmp99C[1]=
# 1575
Cyc_PP_egroup(((_tmp9A1="[",_tag_dyneither(_tmp9A1,sizeof(char),2))),((_tmp9A0="]",_tag_dyneither(_tmp9A0,sizeof(char),2))),((_tmp99F=",",_tag_dyneither(_tmp99F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp470))),((_tmp99C[0]=
# 1574
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1577
goto _LL255;}_LL276: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp473=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp473->tag != 12)goto _LL278;else{_tmp474=_tmp473->f2;}}_LL277:
# 1578
 _tmp476=_tmp474;goto _LL279;_LL278: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp475=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp475->tag != 13)goto _LL27A;else{_tmp476=_tmp475->f2;}}_LL279:
# 1579
 s=Cyc_Absynpp_qvar2doc(_tmp476->name);goto _LL255;_LL27A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp477=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp477->tag != 7)goto _LL27C;else{_tmp478=_tmp477->f2;_tmp479=_tmp477->f3;if(_tmp479 != 0)goto _LL27C;}}_LL27B:
# 1580
 s=Cyc_Absynpp_qvar2doc(_tmp478->name);goto _LL255;_LL27C: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp47A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp47A->tag != 7)goto _LL27E;else{_tmp47B=_tmp47A->f2;_tmp47C=_tmp47A->f3;_tmp47D=_tmp47A->f4;}}_LL27D: {
# 1582
const char*_tmp9A3;const char*_tmp9A2;struct _dyneither_ptr term=_tmp47D?(_tmp9A3=", ...)",_tag_dyneither(_tmp9A3,sizeof(char),7)):((_tmp9A2=")",_tag_dyneither(_tmp9A2,sizeof(char),2)));
# 1583
{const char*_tmp9A8;const char*_tmp9A7;struct Cyc_PP_Doc*_tmp9A6[2];s=((_tmp9A6[1]=Cyc_PP_egroup(((_tmp9A8="(",_tag_dyneither(_tmp9A8,sizeof(char),2))),term,((_tmp9A7=",",_tag_dyneither(_tmp9A7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp47C)),((_tmp9A6[0]=Cyc_Absynpp_qvar2doc(_tmp47B->name),Cyc_PP_cat(_tag_dyneither(_tmp9A6,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1584
goto _LL255;}_LL27E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp47E=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp43D;if(_tmp47E->tag != 16)goto _LL255;else{_tmp47F=_tmp47E->f1;}}_LL27F:
# 1586
 s=Cyc_Absynpp_exp2doc(_tmp47F);goto _LL255;_LL255:;}
# 1588
return s;}
# 1591
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
# 1592
const char*_tmp9AF;const char*_tmp9AE;const char*_tmp9AD;struct Cyc_PP_Doc*_tmp9AC[2];return(_tmp9AC[1]=
# 1593
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp9AC[0]=
# 1592
Cyc_PP_egroup(((_tmp9AF="",_tag_dyneither(_tmp9AF,sizeof(char),1))),((_tmp9AE="=",_tag_dyneither(_tmp9AE,sizeof(char),2))),((_tmp9AD="=",_tag_dyneither(_tmp9AD,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1596
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1597
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
# 1598
const char*_tmp9B4;struct Cyc_PP_Doc*_tmp9B3[2];struct Cyc_PP_Doc*_tmp9B2[2];return(_tmp9B2[1]=
# 1599
Cyc_PP_nest(2,((_tmp9B3[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9B3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B2[0]=
# 1598
Cyc_PP_text(((_tmp9B4="default: ",_tag_dyneither(_tmp9B4,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp9B2,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1600
if(c->where_clause == 0){
# 1601
const char*_tmp9BB;const char*_tmp9BA;struct Cyc_PP_Doc*_tmp9B9[2];struct Cyc_PP_Doc*_tmp9B8[4];return(_tmp9B8[3]=
# 1604
Cyc_PP_nest(2,((_tmp9B9[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9B9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B9,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B8[2]=
# 1603
Cyc_PP_text(((_tmp9BA=": ",_tag_dyneither(_tmp9BA,sizeof(char),3)))),((_tmp9B8[1]=
# 1602
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9B8[0]=
# 1601
Cyc_PP_text(((_tmp9BB="case ",_tag_dyneither(_tmp9BB,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9B8,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1606
const char*_tmp9C4;const char*_tmp9C3;const char*_tmp9C2;struct Cyc_PP_Doc*_tmp9C1[2];struct Cyc_PP_Doc*_tmp9C0[6];return(_tmp9C0[5]=
# 1611
Cyc_PP_nest(2,((_tmp9C1[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp9C1[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C0[4]=
# 1610
Cyc_PP_text(((_tmp9C2=": ",_tag_dyneither(_tmp9C2,sizeof(char),3)))),((_tmp9C0[3]=
# 1609
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9C0[2]=
# 1608
Cyc_PP_text(((_tmp9C3=" && ",_tag_dyneither(_tmp9C3,sizeof(char),5)))),((_tmp9C0[1]=
# 1607
Cyc_Absynpp_pat2doc(c->pattern),((_tmp9C0[0]=
# 1606
Cyc_PP_text(((_tmp9C4="case ",_tag_dyneither(_tmp9C4,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9C0,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1614
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
# 1615
const char*_tmp9C5;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp9C5="",_tag_dyneither(_tmp9C5,sizeof(char),1))),cs);}
# 1618
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
# 1619
if(f->tag == 0)
# 1620
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1622
const char*_tmp9C8;struct Cyc_PP_Doc*_tmp9C7[3];return(_tmp9C7[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9C7[1]=Cyc_PP_text(((_tmp9C8=" = ",_tag_dyneither(_tmp9C8,sizeof(char),4)))),((_tmp9C7[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp9C7,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1625
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
# 1626
const char*_tmp9C9;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp9C9=",",_tag_dyneither(_tmp9C9,sizeof(char),2))),fs);}
# 1629
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
# 1630
return Cyc_Absynpp_qvar2doc(v->name);}
# 1633
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
# 1634
const char*_tmp9CA;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp9CA=",",_tag_dyneither(_tmp9CA,sizeof(char),2))),vds);}
# 1637
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
# 1638
struct Cyc_Absyn_Vardecl _tmp4CE;enum Cyc_Absyn_Scope _tmp4CF;struct _tuple0*_tmp4D0;struct Cyc_Absyn_Tqual _tmp4D1;void*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_Vardecl*_tmp4CD=vd;_tmp4CE=*_tmp4CD;_tmp4CF=_tmp4CE.sc;_tmp4D0=_tmp4CE.name;_tmp4D1=_tmp4CE.tq;_tmp4D2=_tmp4CE.type;_tmp4D3=_tmp4CE.initializer;_tmp4D4=_tmp4CE.attributes;{
# 1639
struct Cyc_PP_Doc*s;
# 1640
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4D0);
# 1641
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4D4);
# 1642
struct Cyc_PP_Doc*beforenamedoc;
# 1643
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL280:
# 1644
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL281:
# 1646
{void*_tmp4D5=Cyc_Tcutil_compress(_tmp4D2);struct Cyc_Absyn_FnInfo _tmp4D7;struct Cyc_List_List*_tmp4D8;_LL284: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4D6=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D5;if(_tmp4D6->tag != 9)goto _LL286;else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D7.attributes;}}_LL285:
# 1648
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4D8);
# 1649
goto _LL283;_LL286:;_LL287:
# 1650
 beforenamedoc=Cyc_PP_nil_doc();goto _LL283;_LL283:;}
# 1652
break;}{
# 1655
struct Cyc_PP_Doc*tmp_doc;
# 1656
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL288:
# 1657
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL289:
# 1658
 tmp_doc=attsdoc;break;}
# 1660
{struct Cyc_Core_Opt*_tmp9D7;struct Cyc_PP_Doc*_tmp9D6[2];struct Cyc_PP_Doc*_tmp9D5[2];const char*_tmp9D4;const char*_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[5];s=((_tmp9D2[4]=
# 1667
Cyc_PP_text(((_tmp9D3=";",_tag_dyneither(_tmp9D3,sizeof(char),2)))),((_tmp9D2[3]=
# 1664
_tmp4D3 == 0?
# 1665
Cyc_PP_nil_doc():(
# 1666
(_tmp9D5[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4D3),((_tmp9D5[0]=Cyc_PP_text(((_tmp9D4=" = ",_tag_dyneither(_tmp9D4,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp9D5,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9D2[2]=
# 1663
Cyc_Absynpp_tqtd2doc(_tmp4D1,_tmp4D2,((_tmp9D7=_cycalloc(sizeof(*_tmp9D7)),((_tmp9D7->v=((_tmp9D6[1]=sn,((_tmp9D6[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9D6,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9D7))))),((_tmp9D2[1]=
# 1662
Cyc_Absynpp_scope2doc(_tmp4CF),((_tmp9D2[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1668
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1671
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
# 1672
struct _tuple0*_tmp4E0;struct _tuple17 _tmp4DF=*x;_tmp4E0=_tmp4DF.f2;
# 1673
return Cyc_Absynpp_qvar2doc(_tmp4E0);}
# 1676
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
# 1677
if(ad->impl == 0){
# 1678
struct Cyc_PP_Doc*_tmp9D8[4];return(_tmp9D8[3]=
# 1681
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9D8[2]=
# 1680
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9D8[1]=
# 1679
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9D8[0]=
# 1678
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9D8,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1683
struct Cyc_PP_Doc*_tmp9E0[2];const char*_tmp9DF;struct Cyc_PP_Doc*_tmp9DE[2];struct Cyc_PP_Doc*_tmp9DD[12];return(_tmp9DD[11]=
# 1694
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9DD[10]=
# 1693
Cyc_Absynpp_rb(),((_tmp9DD[9]=
# 1692
Cyc_PP_line_doc(),((_tmp9DD[8]=
# 1691
Cyc_PP_nest(2,((_tmp9DE[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp9DE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9DE,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9DD[7]=
# 1689
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
# 1690
(_tmp9E0[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp9E0[0]=Cyc_PP_text(((_tmp9DF=":",_tag_dyneither(_tmp9DF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9E0,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9DD[6]=
# 1688
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp9DD[5]=
# 1687
Cyc_Absynpp_lb(),((_tmp9DD[4]=Cyc_PP_blank_doc(),((_tmp9DD[3]=
# 1686
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9DD[2]=
# 1685
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9DD[1]=
# 1684
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9DD[0]=
# 1683
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9DD,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1697
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
# 1698
struct Cyc_Absyn_Datatypedecl _tmp4E7;enum Cyc_Absyn_Scope _tmp4E8;struct _tuple0*_tmp4E9;struct Cyc_List_List*_tmp4EA;struct Cyc_Core_Opt*_tmp4EB;int _tmp4EC;struct Cyc_Absyn_Datatypedecl*_tmp4E6=dd;_tmp4E7=*_tmp4E6;_tmp4E8=_tmp4E7.sc;_tmp4E9=_tmp4E7.name;_tmp4EA=_tmp4E7.tvs;_tmp4EB=_tmp4E7.fields;_tmp4EC=_tmp4E7.is_extensible;
# 1699
if(_tmp4EB == 0){
# 1700
const char*_tmp9E5;const char*_tmp9E4;struct Cyc_PP_Doc*_tmp9E3[5];return(_tmp9E3[4]=
# 1704
Cyc_Absynpp_ktvars2doc(_tmp4EA),((_tmp9E3[3]=
# 1703
_tmp4EC?Cyc_Absynpp_qvar2bolddoc(_tmp4E9): Cyc_Absynpp_typedef_name2bolddoc(_tmp4E9),((_tmp9E3[2]=
# 1702
Cyc_PP_text(((_tmp9E4="datatype ",_tag_dyneither(_tmp9E4,sizeof(char),10)))),((_tmp9E3[1]=
# 1701
_tmp4EC?Cyc_PP_text(((_tmp9E5="@extensible ",_tag_dyneither(_tmp9E5,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9E3[0]=
# 1700
Cyc_Absynpp_scope2doc(_tmp4E8),Cyc_PP_cat(_tag_dyneither(_tmp9E3,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1706
const char*_tmp9EC;const char*_tmp9EB;struct Cyc_PP_Doc*_tmp9EA[2];struct Cyc_PP_Doc*_tmp9E9[10];return(_tmp9E9[9]=
# 1714
Cyc_Absynpp_rb(),((_tmp9E9[8]=
# 1713
Cyc_PP_line_doc(),((_tmp9E9[7]=
# 1712
Cyc_PP_nest(2,((_tmp9EA[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4EB->v),((_tmp9EA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9EA,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9E9[6]=
# 1711
Cyc_Absynpp_lb(),((_tmp9E9[5]=Cyc_PP_blank_doc(),((_tmp9E9[4]=
# 1710
Cyc_Absynpp_ktvars2doc(_tmp4EA),((_tmp9E9[3]=
# 1709
_tmp4EC?Cyc_Absynpp_qvar2bolddoc(_tmp4E9): Cyc_Absynpp_typedef_name2bolddoc(_tmp4E9),((_tmp9E9[2]=
# 1708
Cyc_PP_text(((_tmp9EB="datatype ",_tag_dyneither(_tmp9EB,sizeof(char),10)))),((_tmp9E9[1]=
# 1707
_tmp4EC?Cyc_PP_text(((_tmp9EC="@extensible ",_tag_dyneither(_tmp9EC,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9E9[0]=
# 1706
Cyc_Absynpp_scope2doc(_tmp4E8),Cyc_PP_cat(_tag_dyneither(_tmp9E9,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1717
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
# 1718
struct Cyc_Absyn_Enumdecl _tmp4F5;enum Cyc_Absyn_Scope _tmp4F6;struct _tuple0*_tmp4F7;struct Cyc_Core_Opt*_tmp4F8;struct Cyc_Absyn_Enumdecl*_tmp4F4=ed;_tmp4F5=*_tmp4F4;_tmp4F6=_tmp4F5.sc;_tmp4F7=_tmp4F5.name;_tmp4F8=_tmp4F5.fields;
# 1719
if(_tmp4F8 == 0){
# 1720
const char*_tmp9EF;struct Cyc_PP_Doc*_tmp9EE[3];return(_tmp9EE[2]=
# 1722
Cyc_Absynpp_typedef_name2bolddoc(_tmp4F7),((_tmp9EE[1]=
# 1721
Cyc_PP_text(((_tmp9EF="enum ",_tag_dyneither(_tmp9EF,sizeof(char),6)))),((_tmp9EE[0]=
# 1720
Cyc_Absynpp_scope2doc(_tmp4F6),Cyc_PP_cat(_tag_dyneither(_tmp9EE,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1725
const char*_tmp9F4;struct Cyc_PP_Doc*_tmp9F3[2];struct Cyc_PP_Doc*_tmp9F2[8];return(_tmp9F2[7]=
# 1731
Cyc_Absynpp_rb(),((_tmp9F2[6]=
# 1730
Cyc_PP_line_doc(),((_tmp9F2[5]=
# 1729
Cyc_PP_nest(2,((_tmp9F3[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4F8->v),((_tmp9F3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9F3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9F2[4]=
# 1728
Cyc_Absynpp_lb(),((_tmp9F2[3]=Cyc_PP_blank_doc(),((_tmp9F2[2]=
# 1727
Cyc_Absynpp_qvar2bolddoc(_tmp4F7),((_tmp9F2[1]=
# 1726
Cyc_PP_text(((_tmp9F4="enum ",_tag_dyneither(_tmp9F4,sizeof(char),6)))),((_tmp9F2[0]=
# 1725
Cyc_Absynpp_scope2doc(_tmp4F6),Cyc_PP_cat(_tag_dyneither(_tmp9F2,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1734
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
# 1735
struct Cyc_PP_Doc*s;
# 1736
{void*_tmp4FE=d->r;struct Cyc_Absyn_Fndecl*_tmp500;struct Cyc_Absyn_Aggrdecl*_tmp502;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Tvar*_tmp506;struct Cyc_Absyn_Vardecl*_tmp507;int _tmp508;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Tvar*_tmp50C;struct Cyc_Absyn_Vardecl*_tmp50D;struct Cyc_Absyn_Datatypedecl*_tmp50F;struct Cyc_Absyn_Pat*_tmp511;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_List_List*_tmp514;struct Cyc_Absyn_Enumdecl*_tmp516;struct Cyc_Absyn_Typedefdecl*_tmp518;struct _dyneither_ptr*_tmp51A;struct Cyc_List_List*_tmp51B;struct _tuple0*_tmp51D;struct Cyc_List_List*_tmp51E;struct Cyc_List_List*_tmp520;struct Cyc_List_List*_tmp522;struct Cyc_List_List*_tmp523;_LL28C: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4FF=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp4FF->tag != 1)goto _LL28E;else{_tmp500=_tmp4FF->f1;}}_LL28D: {
# 1738
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9FA;struct Cyc_Absyn_FnInfo _tmp9F9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9F8;void*t=(void*)((_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9FA.tag=9,((_tmp9FA.f1=((_tmp9F9.tvars=_tmp500->tvs,((_tmp9F9.effect=_tmp500->effect,((_tmp9F9.ret_tqual=_tmp500->ret_tqual,((_tmp9F9.ret_typ=_tmp500->ret_type,((_tmp9F9.args=
# 1742
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp500->args),((_tmp9F9.c_varargs=_tmp500->c_varargs,((_tmp9F9.cyc_varargs=_tmp500->cyc_varargs,((_tmp9F9.rgn_po=_tmp500->rgn_po,((_tmp9F9.attributes=0,_tmp9F9)))))))))))))))))),_tmp9FA)))),_tmp9F8))));
# 1746
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp500->attributes);
# 1747
struct Cyc_PP_Doc*inlinedoc;
# 1748
if(_tmp500->is_inline)
# 1749
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AC:
# 1750
{const char*_tmp9FB;inlinedoc=Cyc_PP_text(((_tmp9FB="inline ",_tag_dyneither(_tmp9FB,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2AD:
# 1751
{const char*_tmp9FC;inlinedoc=Cyc_PP_text(((_tmp9FC="__inline ",_tag_dyneither(_tmp9FC,sizeof(char),10))));}break;}else{
# 1754
inlinedoc=Cyc_PP_nil_doc();}{
# 1755
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp500->sc);
# 1756
struct Cyc_PP_Doc*beforenamedoc;
# 1757
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AF:
# 1758
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B0:
# 1759
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp500->attributes);break;}{
# 1761
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp500->name);
# 1762
struct Cyc_PP_Doc*_tmp9FF[2];struct Cyc_Core_Opt*_tmp9FE;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
# 1763
(_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE->v=((_tmp9FF[1]=namedoc,((_tmp9FF[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9FF,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9FE)))));
# 1764
struct Cyc_PP_Doc*_tmpA02[2];struct Cyc_PP_Doc*_tmpA01[5];struct Cyc_PP_Doc*bodydoc=(_tmpA01[4]=
# 1767
Cyc_Absynpp_rb(),((_tmpA01[3]=
# 1766
Cyc_PP_line_doc(),((_tmpA01[2]=
# 1765
Cyc_PP_nest(2,((_tmpA02[1]=Cyc_Absynpp_stmt2doc(_tmp500->body,0),((_tmpA02[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA01[1]=
# 1764
Cyc_Absynpp_lb(),((_tmpA01[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA01,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1768
{struct Cyc_PP_Doc*_tmpA03[4];s=((_tmpA03[3]=bodydoc,((_tmpA03[2]=tqtddoc,((_tmpA03[1]=scopedoc,((_tmpA03[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmpA03,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1770
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B2:
# 1771
{struct Cyc_PP_Doc*_tmpA04[2];s=((_tmpA04[1]=s,((_tmpA04[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmpA04,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B3:
# 1772
 break;}
# 1775
goto _LL28B;};};}_LL28E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp501=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp501->tag != 6)goto _LL290;else{_tmp502=_tmp501->f1;}}_LL28F:
# 1778
{const char*_tmpA07;struct Cyc_PP_Doc*_tmpA06[2];s=((_tmpA06[1]=Cyc_PP_text(((_tmpA07=";",_tag_dyneither(_tmpA07,sizeof(char),2)))),((_tmpA06[0]=Cyc_Absynpp_aggrdecl2doc(_tmp502),Cyc_PP_cat(_tag_dyneither(_tmpA06,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1779
goto _LL28B;_LL290: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp503=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp503->tag != 0)goto _LL292;else{_tmp504=_tmp503->f1;}}_LL291:
# 1781
 s=Cyc_Absynpp_vardecl2doc(_tmp504);
# 1782
goto _LL28B;_LL292: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp505=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp505->tag != 4)goto _LL294;else{_tmp506=_tmp505->f1;_tmp507=_tmp505->f2;_tmp508=_tmp505->f3;_tmp509=_tmp505->f4;}}_LL293:
# 1784
{const char*_tmpA1A;const char*_tmpA19;const char*_tmpA18;const char*_tmpA17;struct Cyc_PP_Doc*_tmpA16[3];const char*_tmpA15;const char*_tmpA14;const char*_tmpA13;struct Cyc_PP_Doc*_tmpA12[8];s=((_tmpA12[7]=
# 1792
Cyc_PP_text(((_tmpA13=";",_tag_dyneither(_tmpA13,sizeof(char),2)))),((_tmpA12[6]=
# 1790
_tmp509 != 0?
# 1791
(_tmpA16[2]=Cyc_PP_text(((_tmpA15=")",_tag_dyneither(_tmpA15,sizeof(char),2)))),((_tmpA16[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp509),((_tmpA16[0]=Cyc_PP_text(((_tmpA14=" = open(",_tag_dyneither(_tmpA14,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA16,sizeof(struct Cyc_PP_Doc*),3))))))):
# 1792
 Cyc_PP_nil_doc(),((_tmpA12[5]=
# 1789
_tmp508?Cyc_PP_text(((_tmpA17=" @resetable",_tag_dyneither(_tmpA17,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmpA12[4]=
# 1788
Cyc_Absynpp_qvar2doc(_tmp507->name),((_tmpA12[3]=
# 1787
Cyc_PP_text(((_tmpA18=">",_tag_dyneither(_tmpA18,sizeof(char),2)))),((_tmpA12[2]=
# 1786
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp506)),((_tmpA12[1]=
# 1785
Cyc_PP_text(((_tmpA19="<",_tag_dyneither(_tmpA19,sizeof(char),2)))),((_tmpA12[0]=
# 1784
Cyc_PP_text(((_tmpA1A="region",_tag_dyneither(_tmpA1A,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA12,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1793
goto _LL28B;_LL294: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp50A=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp50A->tag != 5)goto _LL296;else{_tmp50B=_tmp50A->f1;_tmp50C=_tmp50A->f2;_tmp50D=_tmp50A->f3;}}_LL295:
# 1795
{const char*_tmpA25;const char*_tmpA24;const char*_tmpA23;const char*_tmpA22;const char*_tmpA21;struct Cyc_PP_Doc*_tmpA20[8];s=((_tmpA20[7]=
# 1796
Cyc_PP_text(((_tmpA21=";",_tag_dyneither(_tmpA21,sizeof(char),2)))),((_tmpA20[6]=Cyc_Absynpp_exp2doc(_tmp50B),((_tmpA20[5]=Cyc_PP_text(((_tmpA22=" = ",_tag_dyneither(_tmpA22,sizeof(char),4)))),((_tmpA20[4]=Cyc_Absynpp_qvar2doc(_tmp50D->name),((_tmpA20[3]=
# 1795
Cyc_PP_text(((_tmpA23="> ",_tag_dyneither(_tmpA23,sizeof(char),3)))),((_tmpA20[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp50C)),((_tmpA20[1]=Cyc_PP_text(((_tmpA24=" <",_tag_dyneither(_tmpA24,sizeof(char),3)))),((_tmpA20[0]=Cyc_PP_text(((_tmpA25="alias",_tag_dyneither(_tmpA25,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmpA20,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1797
goto _LL28B;_LL296: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp50E=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp50E->tag != 7)goto _LL298;else{_tmp50F=_tmp50E->f1;}}_LL297:
# 1799
{const char*_tmpA28;struct Cyc_PP_Doc*_tmpA27[2];s=((_tmpA27[1]=Cyc_PP_text(((_tmpA28=";",_tag_dyneither(_tmpA28,sizeof(char),2)))),((_tmpA27[0]=Cyc_Absynpp_datatypedecl2doc(_tmp50F),Cyc_PP_cat(_tag_dyneither(_tmpA27,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1800
goto _LL28B;_LL298: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp510=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp510->tag != 2)goto _LL29A;else{_tmp511=_tmp510->f1;_tmp512=_tmp510->f3;}}_LL299:
# 1802
{const char*_tmpA2F;const char*_tmpA2E;const char*_tmpA2D;struct Cyc_PP_Doc*_tmpA2C[5];s=((_tmpA2C[4]=
# 1806
Cyc_PP_text(((_tmpA2D=";",_tag_dyneither(_tmpA2D,sizeof(char),2)))),((_tmpA2C[3]=
# 1805
Cyc_Absynpp_exp2doc(_tmp512),((_tmpA2C[2]=
# 1804
Cyc_PP_text(((_tmpA2E=" = ",_tag_dyneither(_tmpA2E,sizeof(char),4)))),((_tmpA2C[1]=
# 1803
Cyc_Absynpp_pat2doc(_tmp511),((_tmpA2C[0]=
# 1802
Cyc_PP_text(((_tmpA2F="let ",_tag_dyneither(_tmpA2F,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA2C,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1807
goto _LL28B;_LL29A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp513=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp513->tag != 3)goto _LL29C;else{_tmp514=_tmp513->f1;}}_LL29B:
# 1809
{const char*_tmpA34;const char*_tmpA33;struct Cyc_PP_Doc*_tmpA32[3];s=((_tmpA32[2]=Cyc_PP_text(((_tmpA33=";",_tag_dyneither(_tmpA33,sizeof(char),2)))),((_tmpA32[1]=Cyc_Absynpp_ids2doc(_tmp514),((_tmpA32[0]=Cyc_PP_text(((_tmpA34="let ",_tag_dyneither(_tmpA34,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA32,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1810
goto _LL28B;_LL29C: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp515=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp515->tag != 8)goto _LL29E;else{_tmp516=_tmp515->f1;}}_LL29D:
# 1812
{const char*_tmpA37;struct Cyc_PP_Doc*_tmpA36[2];s=((_tmpA36[1]=Cyc_PP_text(((_tmpA37=";",_tag_dyneither(_tmpA37,sizeof(char),2)))),((_tmpA36[0]=Cyc_Absynpp_enumdecl2doc(_tmp516),Cyc_PP_cat(_tag_dyneither(_tmpA36,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1813
goto _LL28B;_LL29E: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp517=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp517->tag != 9)goto _LL2A0;else{_tmp518=_tmp517->f1;}}_LL29F: {
# 1815
void*t;
# 1816
if(_tmp518->defn != 0)
# 1817
t=(void*)_check_null(_tmp518->defn);else{
# 1819
t=Cyc_Absyn_new_evar(_tmp518->kind,0);}
# 1820
{const char*_tmpA41;struct Cyc_Core_Opt*_tmpA40;struct Cyc_PP_Doc*_tmpA3F[2];const char*_tmpA3E;struct Cyc_PP_Doc*_tmpA3D[4];s=((_tmpA3D[3]=
# 1827
Cyc_PP_text(((_tmpA3E=";",_tag_dyneither(_tmpA3E,sizeof(char),2)))),((_tmpA3D[2]=
# 1826
Cyc_Absynpp_atts2doc(_tmp518->atts),((_tmpA3D[1]=
# 1821
Cyc_Absynpp_tqtd2doc(_tmp518->tq,t,(
# 1823
(_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40->v=((_tmpA3F[1]=
# 1824
Cyc_Absynpp_tvars2doc(_tmp518->tvs),((_tmpA3F[0]=
# 1823
Cyc_Absynpp_typedef_name2bolddoc(_tmp518->name),Cyc_PP_cat(_tag_dyneither(_tmpA3F,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA40))))),((_tmpA3D[0]=
# 1820
Cyc_PP_text(((_tmpA41="typedef ",_tag_dyneither(_tmpA41,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA3D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1829
goto _LL28B;}_LL2A0: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp519=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp519->tag != 10)goto _LL2A2;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp519->f2;}}_LL2A1:
# 1831
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp51A);
# 1832
{const char*_tmpA46;const char*_tmpA45;struct Cyc_PP_Doc*_tmpA44[8];s=((_tmpA44[7]=
# 1838
Cyc_Absynpp_rb(),((_tmpA44[6]=
# 1837
Cyc_PP_line_doc(),((_tmpA44[5]=
# 1836
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA45="",_tag_dyneither(_tmpA45,sizeof(char),1))),_tmp51B),((_tmpA44[4]=
# 1835
Cyc_PP_line_doc(),((_tmpA44[3]=
# 1834
Cyc_Absynpp_lb(),((_tmpA44[2]=Cyc_PP_blank_doc(),((_tmpA44[1]=
# 1833
Cyc_PP_textptr(_tmp51A),((_tmpA44[0]=
# 1832
Cyc_PP_text(((_tmpA46="namespace ",_tag_dyneither(_tmpA46,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA44,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1839
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
# 1840
goto _LL28B;_LL2A2: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp51C=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp51C->tag != 11)goto _LL2A4;else{_tmp51D=_tmp51C->f1;_tmp51E=_tmp51C->f2;}}_LL2A3:
# 1842
 if(Cyc_Absynpp_print_using_stmts){
# 1843
const char*_tmpA4B;const char*_tmpA4A;struct Cyc_PP_Doc*_tmpA49[8];s=((_tmpA49[7]=
# 1849
Cyc_Absynpp_rb(),((_tmpA49[6]=
# 1848
Cyc_PP_line_doc(),((_tmpA49[5]=
# 1847
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA4A="",_tag_dyneither(_tmpA4A,sizeof(char),1))),_tmp51E),((_tmpA49[4]=
# 1846
Cyc_PP_line_doc(),((_tmpA49[3]=
# 1845
Cyc_Absynpp_lb(),((_tmpA49[2]=Cyc_PP_blank_doc(),((_tmpA49[1]=
# 1844
Cyc_Absynpp_qvar2doc(_tmp51D),((_tmpA49[0]=
# 1843
Cyc_PP_text(((_tmpA4B="using ",_tag_dyneither(_tmpA4B,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA49,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1851
const char*_tmpA56;const char*_tmpA55;const char*_tmpA54;const char*_tmpA53;const char*_tmpA52;struct Cyc_PP_Doc*_tmpA51[11];s=((_tmpA51[10]=
# 1861
Cyc_PP_text(((_tmpA52=" */",_tag_dyneither(_tmpA52,sizeof(char),4)))),((_tmpA51[9]=
# 1860
Cyc_Absynpp_rb(),((_tmpA51[8]=
# 1859
Cyc_PP_text(((_tmpA53="/* ",_tag_dyneither(_tmpA53,sizeof(char),4)))),((_tmpA51[7]=
# 1858
Cyc_PP_line_doc(),((_tmpA51[6]=
# 1857
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA54="",_tag_dyneither(_tmpA54,sizeof(char),1))),_tmp51E),((_tmpA51[5]=
# 1856
Cyc_PP_line_doc(),((_tmpA51[4]=
# 1855
Cyc_PP_text(((_tmpA55=" */",_tag_dyneither(_tmpA55,sizeof(char),4)))),((_tmpA51[3]=
# 1854
Cyc_Absynpp_lb(),((_tmpA51[2]=
# 1853
Cyc_PP_blank_doc(),((_tmpA51[1]=
# 1852
Cyc_Absynpp_qvar2doc(_tmp51D),((_tmpA51[0]=
# 1851
Cyc_PP_text(((_tmpA56="/* using ",_tag_dyneither(_tmpA56,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA51,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1862
goto _LL28B;_LL2A4: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp51F=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp51F->tag != 12)goto _LL2A6;else{_tmp520=_tmp51F->f1;}}_LL2A5:
# 1864
 if(Cyc_Absynpp_print_externC_stmts){
# 1865
const char*_tmpA5B;const char*_tmpA5A;struct Cyc_PP_Doc*_tmpA59[6];s=((_tmpA59[5]=
# 1870
Cyc_Absynpp_rb(),((_tmpA59[4]=
# 1869
Cyc_PP_line_doc(),((_tmpA59[3]=
# 1868
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA5A="",_tag_dyneither(_tmpA5A,sizeof(char),1))),_tmp520),((_tmpA59[2]=
# 1867
Cyc_PP_line_doc(),((_tmpA59[1]=
# 1866
Cyc_Absynpp_lb(),((_tmpA59[0]=
# 1865
Cyc_PP_text(((_tmpA5B="extern \"C\" ",_tag_dyneither(_tmpA5B,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA59,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1872
const char*_tmpA66;const char*_tmpA65;const char*_tmpA64;const char*_tmpA63;const char*_tmpA62;struct Cyc_PP_Doc*_tmpA61[9];s=((_tmpA61[8]=
# 1880
Cyc_PP_text(((_tmpA62=" */",_tag_dyneither(_tmpA62,sizeof(char),4)))),((_tmpA61[7]=
# 1879
Cyc_Absynpp_rb(),((_tmpA61[6]=
# 1878
Cyc_PP_text(((_tmpA63="/* ",_tag_dyneither(_tmpA63,sizeof(char),4)))),((_tmpA61[5]=
# 1877
Cyc_PP_line_doc(),((_tmpA61[4]=
# 1876
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA64="",_tag_dyneither(_tmpA64,sizeof(char),1))),_tmp520),((_tmpA61[3]=
# 1875
Cyc_PP_line_doc(),((_tmpA61[2]=
# 1874
Cyc_PP_text(((_tmpA65=" */",_tag_dyneither(_tmpA65,sizeof(char),4)))),((_tmpA61[1]=
# 1873
Cyc_Absynpp_lb(),((_tmpA61[0]=
# 1872
Cyc_PP_text(((_tmpA66="/* extern \"C\" ",_tag_dyneither(_tmpA66,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA61,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1881
goto _LL28B;_LL2A6: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp521=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp521->tag != 13)goto _LL2A8;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;}}_LL2A7:
# 1883
 if(Cyc_Absynpp_print_externC_stmts){
# 1884
struct Cyc_PP_Doc*exs_doc;
# 1885
if(_tmp523 != 0){
# 1886
const char*_tmpA6B;const char*_tmpA6A;struct Cyc_PP_Doc*_tmpA69[7];exs_doc=((_tmpA69[6]=
# 1888
Cyc_Absynpp_rb(),((_tmpA69[5]=Cyc_PP_line_doc(),((_tmpA69[4]=
# 1887
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA6A=",",_tag_dyneither(_tmpA6A,sizeof(char),2))),_tmp523),((_tmpA69[3]=Cyc_PP_line_doc(),((_tmpA69[2]=
# 1886
Cyc_Absynpp_lb(),((_tmpA69[1]=Cyc_PP_text(((_tmpA6B=" export ",_tag_dyneither(_tmpA6B,sizeof(char),9)))),((_tmpA69[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA69,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1890
exs_doc=Cyc_Absynpp_rb();}{
# 1891
const char*_tmpA70;const char*_tmpA6F;struct Cyc_PP_Doc*_tmpA6E[6];s=((_tmpA6E[5]=exs_doc,((_tmpA6E[4]=
# 1895
Cyc_PP_line_doc(),((_tmpA6E[3]=
# 1894
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA6F="",_tag_dyneither(_tmpA6F,sizeof(char),1))),_tmp522),((_tmpA6E[2]=
# 1893
Cyc_PP_line_doc(),((_tmpA6E[1]=
# 1892
Cyc_Absynpp_lb(),((_tmpA6E[0]=
# 1891
Cyc_PP_text(((_tmpA70="extern \"C include\" ",_tag_dyneither(_tmpA70,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA6E,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1898
const char*_tmpA7B;const char*_tmpA7A;const char*_tmpA79;const char*_tmpA78;const char*_tmpA77;struct Cyc_PP_Doc*_tmpA76[9];s=((_tmpA76[8]=
# 1906
Cyc_PP_text(((_tmpA77=" */",_tag_dyneither(_tmpA77,sizeof(char),4)))),((_tmpA76[7]=
# 1905
Cyc_Absynpp_rb(),((_tmpA76[6]=
# 1904
Cyc_PP_text(((_tmpA78="/* ",_tag_dyneither(_tmpA78,sizeof(char),4)))),((_tmpA76[5]=
# 1903
Cyc_PP_line_doc(),((_tmpA76[4]=
# 1902
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA79="",_tag_dyneither(_tmpA79,sizeof(char),1))),_tmp522),((_tmpA76[3]=
# 1901
Cyc_PP_line_doc(),((_tmpA76[2]=
# 1900
Cyc_PP_text(((_tmpA7A=" */",_tag_dyneither(_tmpA7A,sizeof(char),4)))),((_tmpA76[1]=
# 1899
Cyc_Absynpp_lb(),((_tmpA76[0]=
# 1898
Cyc_PP_text(((_tmpA7B="/* extern \"C include\" ",_tag_dyneither(_tmpA7B,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA76,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1907
goto _LL28B;_LL2A8: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp524=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp524->tag != 14)goto _LL2AA;}_LL2A9:
# 1909
{const char*_tmpA7E;struct Cyc_PP_Doc*_tmpA7D[2];s=((_tmpA7D[1]=Cyc_Absynpp_lb(),((_tmpA7D[0]=Cyc_PP_text(((_tmpA7E="__cyclone_port_on__;",_tag_dyneither(_tmpA7E,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA7D,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1910
goto _LL28B;_LL2AA: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp525=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4FE;if(_tmp525->tag != 15)goto _LL28B;}_LL2AB:
# 1912
{const char*_tmpA81;struct Cyc_PP_Doc*_tmpA80[2];s=((_tmpA80[1]=Cyc_Absynpp_lb(),((_tmpA80[0]=Cyc_PP_text(((_tmpA81="__cyclone_port_off__;",_tag_dyneither(_tmpA81,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA80,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1913
goto _LL28B;_LL28B:;}
# 1916
return s;}
# 1919
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
# 1920
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
# 1921
switch(sc){case Cyc_Absyn_Static: _LL2B5: {
# 1922
const char*_tmpA82;return Cyc_PP_text(((_tmpA82="static ",_tag_dyneither(_tmpA82,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2B6:
# 1923
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2B7: {
# 1924
const char*_tmpA83;return Cyc_PP_text(((_tmpA83="extern ",_tag_dyneither(_tmpA83,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2B8: {
# 1925
const char*_tmpA84;return Cyc_PP_text(((_tmpA84="extern \"C\" ",_tag_dyneither(_tmpA84,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2B9: {
# 1926
const char*_tmpA85;return Cyc_PP_text(((_tmpA85="abstract ",_tag_dyneither(_tmpA85,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2BA: {
# 1927
const char*_tmpA86;return Cyc_PP_text(((_tmpA86="register ",_tag_dyneither(_tmpA86,sizeof(char),10))));}}}
# 1932
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
# 1933
void*_tmp57C=t;struct Cyc_Absyn_Tvar*_tmp57E;struct Cyc_List_List*_tmp580;_LL2BD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp57C;if(_tmp57D->tag != 2)goto _LL2BF;else{_tmp57E=_tmp57D->f1;}}_LL2BE:
# 1934
 return Cyc_Tcutil_is_temp_tvar(_tmp57E);_LL2BF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp57F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp57C;if(_tmp57F->tag != 24)goto _LL2C1;else{_tmp580=_tmp57F->f1;}}_LL2C0:
# 1935
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp580);_LL2C1:;_LL2C2:
# 1936
 return 0;_LL2BC:;}
# 1944
int Cyc_Absynpp_is_anon_aggrtype(void*t){
# 1945
void*_tmp581=t;struct Cyc_Absyn_Typedefdecl*_tmp585;void*_tmp586;_LL2C4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp582=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp581;if(_tmp582->tag != 12)goto _LL2C6;}_LL2C5:
# 1946
 return 1;_LL2C6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp583=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp581;if(_tmp583->tag != 14)goto _LL2C8;}_LL2C7:
# 1947
 return 1;_LL2C8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp584=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp581;if(_tmp584->tag != 17)goto _LL2CA;else{_tmp585=_tmp584->f3;_tmp586=(void*)_tmp584->f4;}}if(!(_tmp586 != 0))goto _LL2CA;_LL2C9:
# 1951
 return Cyc_Absynpp_is_anon_aggrtype((void*)_tmp586);_LL2CA:;_LL2CB:
# 1952
 return 0;_LL2C3:;}
# 1958
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1961
if(tms != 0  && tms->tl != 0){
# 1962
struct _tuple16 _tmpA87;struct _tuple16 _tmp588=(_tmpA87.f1=(void*)tms->hd,((_tmpA87.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA87)));void*_tmp589;void*_tmp58B;_LL2CD: _tmp589=_tmp588.f1;{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp58A=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp589;if(_tmp58A->tag != 2)goto _LL2CF;};_tmp58B=_tmp588.f2;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp58C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp58B;if(_tmp58C->tag != 3)goto _LL2CF;};_LL2CE: {
# 1964
struct Cyc_List_List*_tmpA8A;struct Cyc_List_List*_tmpA89;return(_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89->hd=(void*)tms->hd,((_tmpA89->tl=((_tmpA8A=_region_malloc(r,sizeof(*_tmpA8A)),((_tmpA8A->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA8A->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA8A)))))),_tmpA89)))));}_LL2CF:;_LL2D0: {
# 1965
struct Cyc_List_List*_tmpA8B;return(_tmpA8B=_region_malloc(r,sizeof(*_tmpA8B)),((_tmpA8B->hd=atts,((_tmpA8B->tl=tms,_tmpA8B)))));}_LL2CC:;}else{
# 1967
struct Cyc_List_List*_tmpA8C;return(_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C->hd=atts,((_tmpA8C->tl=tms,_tmpA8C)))));}}
# 1972
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1974
void*_tmp591=t;struct Cyc_Absyn_ArrayInfo _tmp593;void*_tmp594;struct Cyc_Absyn_Tqual _tmp595;struct Cyc_Absyn_Exp*_tmp596;union Cyc_Absyn_Constraint*_tmp597;unsigned int _tmp598;struct Cyc_Absyn_PtrInfo _tmp59A;void*_tmp59B;struct Cyc_Absyn_Tqual _tmp59C;struct Cyc_Absyn_PtrAtts _tmp59D;struct Cyc_Absyn_FnInfo _tmp59F;struct Cyc_List_List*_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;struct Cyc_List_List*_tmp5A4;int _tmp5A5;struct Cyc_Absyn_VarargInfo*_tmp5A6;struct Cyc_List_List*_tmp5A7;struct Cyc_List_List*_tmp5A8;struct Cyc_Core_Opt*_tmp5AA;void*_tmp5AB;int _tmp5AC;struct _tuple0*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_Absyn_Typedefdecl*_tmp5B0;void*_tmp5B1;_LL2D2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp592=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp591;if(_tmp592->tag != 8)goto _LL2D4;else{_tmp593=_tmp592->f1;_tmp594=_tmp593.elt_type;_tmp595=_tmp593.tq;_tmp596=_tmp593.num_elts;_tmp597=_tmp593.zero_term;_tmp598=_tmp593.zt_loc;}}_LL2D3: {
# 1977
struct Cyc_Absyn_Tqual _tmp5B3;void*_tmp5B4;struct Cyc_List_List*_tmp5B5;struct _tuple12 _tmp5B2=Cyc_Absynpp_to_tms(r,_tmp595,_tmp594);_tmp5B3=_tmp5B2.f1;_tmp5B4=_tmp5B2.f2;_tmp5B5=_tmp5B2.f3;{
# 1978
void*tm;
# 1979
if(_tmp596 == 0){
# 1980
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA8F;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA8E;tm=(void*)((_tmpA8E=_region_malloc(r,sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=0,((_tmpA8F.f1=_tmp597,((_tmpA8F.f2=_tmp598,_tmpA8F)))))),_tmpA8E))));}else{
# 1982
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA92;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA91;tm=(void*)((_tmpA91=_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA92.tag=1,((_tmpA92.f1=(struct Cyc_Absyn_Exp*)_tmp596,((_tmpA92.f2=_tmp597,((_tmpA92.f3=_tmp598,_tmpA92)))))))),_tmpA91))));}{
# 1983
struct Cyc_List_List*_tmpA95;struct _tuple12 _tmpA94;return(_tmpA94.f1=_tmp5B3,((_tmpA94.f2=_tmp5B4,((_tmpA94.f3=((_tmpA95=_region_malloc(r,sizeof(*_tmpA95)),((_tmpA95->hd=tm,((_tmpA95->tl=_tmp5B5,_tmpA95)))))),_tmpA94)))));};};}_LL2D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591;if(_tmp599->tag != 5)goto _LL2D6;else{_tmp59A=_tmp599->f1;_tmp59B=_tmp59A.elt_typ;_tmp59C=_tmp59A.elt_tq;_tmp59D=_tmp59A.ptr_atts;}}_LL2D5: {
# 1986
struct Cyc_Absyn_Tqual _tmp5BD;void*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct _tuple12 _tmp5BC=Cyc_Absynpp_to_tms(r,_tmp59C,_tmp59B);_tmp5BD=_tmp5BC.f1;_tmp5BE=_tmp5BC.f2;_tmp5BF=_tmp5BC.f3;
# 1987
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA9B;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA9A;struct Cyc_List_List*_tmpA99;_tmp5BF=((_tmpA99=_region_malloc(r,sizeof(*_tmpA99)),((_tmpA99->hd=(void*)((_tmpA9B=_region_malloc(r,sizeof(*_tmpA9B)),((_tmpA9B[0]=((_tmpA9A.tag=2,((_tmpA9A.f1=_tmp59D,((_tmpA9A.f2=tq,_tmpA9A)))))),_tmpA9B)))),((_tmpA99->tl=_tmp5BF,_tmpA99))))));}{
# 1988
struct _tuple12 _tmpA9C;return(_tmpA9C.f1=_tmp5BD,((_tmpA9C.f2=_tmp5BE,((_tmpA9C.f3=_tmp5BF,_tmpA9C)))));};}_LL2D6: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp591;if(_tmp59E->tag != 9)goto _LL2D8;else{_tmp59F=_tmp59E->f1;_tmp5A0=_tmp59F.tvars;_tmp5A1=_tmp59F.effect;_tmp5A2=_tmp59F.ret_tqual;_tmp5A3=_tmp59F.ret_typ;_tmp5A4=_tmp59F.args;_tmp5A5=_tmp59F.c_varargs;_tmp5A6=_tmp59F.cyc_varargs;_tmp5A7=_tmp59F.rgn_po;_tmp5A8=_tmp59F.attributes;}}_LL2D7:
# 1992
 if(!Cyc_Absynpp_print_all_tvars){
# 1996
if(_tmp5A1 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp5A1)){
# 1997
_tmp5A1=0;
# 1998
_tmp5A0=0;}}else{
# 2001
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2004
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp5A0);}{
# 2009
struct Cyc_Absyn_Tqual _tmp5C5;void*_tmp5C6;struct Cyc_List_List*_tmp5C7;struct _tuple12 _tmp5C4=Cyc_Absynpp_to_tms(r,_tmp5A2,_tmp5A3);_tmp5C5=_tmp5C4.f1;_tmp5C6=_tmp5C4.f2;_tmp5C7=_tmp5C4.f3;{
# 2010
struct Cyc_List_List*tms=_tmp5C7;
# 2020 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2DE:
# 2022
 if(_tmp5A8 != 0){
# 2025
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA9F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA9E;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA9E=_region_malloc(r,sizeof(*_tmpA9E)),((_tmpA9E[0]=((_tmpA9F.tag=5,((_tmpA9F.f1=0,((_tmpA9F.f2=_tmp5A8,_tmpA9F)))))),_tmpA9E)))),tms);}
# 2027
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpAAE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpAAD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpAAC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpAAB;struct Cyc_List_List*_tmpAAA;tms=((_tmpAAA=_region_malloc(r,sizeof(*_tmpAAA)),((_tmpAAA->hd=(void*)((_tmpAAE=_region_malloc(r,sizeof(*_tmpAAE)),((_tmpAAE[0]=(
# 2028
(_tmpAAB.tag=3,((_tmpAAB.f1=(void*)((void*)((_tmpAAD=_region_malloc(r,sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAC.tag=1,((_tmpAAC.f1=_tmp5A4,((_tmpAAC.f2=_tmp5A5,((_tmpAAC.f3=_tmp5A6,((_tmpAAC.f4=(void*)_tmp5A1,((_tmpAAC.f5=_tmp5A7,_tmpAAC)))))))))))),_tmpAAD))))),_tmpAAB)))),_tmpAAE)))),((_tmpAAA->tl=tms,_tmpAAA))))));}
# 2031
break;case Cyc_Cyclone_Vc_c: _LL2DF:
# 2033
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpABD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpABC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpABB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpABA;struct Cyc_List_List*_tmpAB9;tms=((_tmpAB9=_region_malloc(r,sizeof(*_tmpAB9)),((_tmpAB9->hd=(void*)((_tmpABD=_region_malloc(r,sizeof(*_tmpABD)),((_tmpABD[0]=(
# 2034
(_tmpABA.tag=3,((_tmpABA.f1=(void*)((void*)((_tmpABC=_region_malloc(r,sizeof(*_tmpABC)),((_tmpABC[0]=((_tmpABB.tag=1,((_tmpABB.f1=_tmp5A4,((_tmpABB.f2=_tmp5A5,((_tmpABB.f3=_tmp5A6,((_tmpABB.f4=(void*)_tmp5A1,((_tmpABB.f5=_tmp5A7,_tmpABB)))))))))))),_tmpABC))))),_tmpABA)))),_tmpABD)))),((_tmpAB9->tl=tms,_tmpAB9))))));}
# 2037
for(0;_tmp5A8 != 0;_tmp5A8=_tmp5A8->tl){
# 2038
void*_tmp5D4=(void*)_tmp5A8->hd;_LL2E2: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp5D5=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp5D4;if(_tmp5D5->tag != 1)goto _LL2E4;}_LL2E3:
# 2039
 goto _LL2E5;_LL2E4: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5D6=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp5D4;if(_tmp5D6->tag != 2)goto _LL2E6;}_LL2E5:
# 2040
 goto _LL2E7;_LL2E6: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5D7=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp5D4;if(_tmp5D7->tag != 3)goto _LL2E8;}_LL2E7:
# 2042
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpAC7;struct Cyc_List_List*_tmpAC6;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpAC5;struct Cyc_List_List*_tmpAC4;tms=((_tmpAC4=_region_malloc(r,sizeof(*_tmpAC4)),((_tmpAC4->hd=(void*)((_tmpAC7=_region_malloc(r,sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC5.tag=5,((_tmpAC5.f1=0,((_tmpAC5.f2=((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->hd=(void*)_tmp5A8->hd,((_tmpAC6->tl=0,_tmpAC6)))))),_tmpAC5)))))),_tmpAC7)))),((_tmpAC4->tl=tms,_tmpAC4))))));}
# 2043
goto AfterAtts;_LL2E8:;_LL2E9:
# 2044
 goto _LL2E1;_LL2E1:;}
# 2046
break;}
# 2049
AfterAtts:
# 2050
 if(_tmp5A0 != 0){
# 2051
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpACD;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpACC;struct Cyc_List_List*_tmpACB;tms=((_tmpACB=_region_malloc(r,sizeof(*_tmpACB)),((_tmpACB->hd=(void*)((_tmpACD=_region_malloc(r,sizeof(*_tmpACD)),((_tmpACD[0]=((_tmpACC.tag=4,((_tmpACC.f1=_tmp5A0,((_tmpACC.f2=0,((_tmpACC.f3=1,_tmpACC)))))))),_tmpACD)))),((_tmpACB->tl=tms,_tmpACB))))));}{
# 2052
struct _tuple12 _tmpACE;return(_tmpACE.f1=_tmp5C5,((_tmpACE.f2=_tmp5C6,((_tmpACE.f3=tms,_tmpACE)))));};};};_LL2D8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp591;if(_tmp5A9->tag != 1)goto _LL2DA;else{_tmp5AA=_tmp5A9->f1;_tmp5AB=(void*)_tmp5A9->f2;_tmp5AC=_tmp5A9->f3;}}_LL2D9:
# 2055
 if(_tmp5AB == 0){
# 2056
struct _tuple12 _tmpACF;return(_tmpACF.f1=tq,((_tmpACF.f2=t,((_tmpACF.f3=0,_tmpACF)))));}else{
# 2058
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp5AB);}_LL2DA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp591;if(_tmp5AD->tag != 17)goto _LL2DC;else{_tmp5AE=_tmp5AD->f1;_tmp5AF=_tmp5AD->f2;_tmp5B0=_tmp5AD->f3;_tmp5B1=(void*)_tmp5AD->f4;}}_LL2DB:
# 2063
 if((_tmp5B1 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype((void*)_tmp5B1)){
# 2064
struct _tuple12 _tmpAD0;return(_tmpAD0.f1=tq,((_tmpAD0.f2=t,((_tmpAD0.f3=0,_tmpAD0)))));}else{
# 2066
if(tq.real_const)
# 2067
tq.print_const=tq.real_const;
# 2068
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp5B1);}_LL2DC:;_LL2DD: {
# 2071
struct _tuple12 _tmpAD1;return(_tmpAD1.f1=tq,((_tmpAD1.f2=t,((_tmpAD1.f3=0,_tmpAD1)))));}_LL2D1:;}
# 2075
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2077
void*_tmp5E3=t;void*_tmp5E5;struct Cyc_Absyn_PtrInfo _tmp5E7;void*_tmp5E8;_LL2EB:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E3;if(_tmp5E4->tag != 1)goto _LL2ED;else{_tmp5E5=(void*)_tmp5E4->f2;}}if(!(_tmp5E5 != 0))goto _LL2ED;_LL2EC:
# 2078
 return Cyc_Absynpp_is_char_ptr((void*)_tmp5E5);_LL2ED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E6->tag != 5)goto _LL2EF;else{_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E7.elt_typ;}}_LL2EE:
# 2080
 L: {
# 2081
void*_tmp5E9=_tmp5E8;void*_tmp5EB;void*_tmp5ED;enum Cyc_Absyn_Size_of _tmp5EF;_LL2F2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5EA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5E9;if(_tmp5EA->tag != 1)goto _LL2F4;else{_tmp5EB=(void*)_tmp5EA->f2;}}if(!(_tmp5EB != 0))goto _LL2F4;_LL2F3:
# 2082
 _tmp5E8=(void*)_tmp5EB;goto L;_LL2F4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E9;if(_tmp5EC->tag != 17)goto _LL2F6;else{_tmp5ED=(void*)_tmp5EC->f4;}}if(!(_tmp5ED != 0))goto _LL2F6;_LL2F5:
# 2083
 _tmp5E8=(void*)_tmp5ED;goto L;_LL2F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E9;if(_tmp5EE->tag != 6)goto _LL2F8;else{_tmp5EF=_tmp5EE->f2;if(_tmp5EF != Cyc_Absyn_Char_sz)goto _LL2F8;}}_LL2F7:
# 2084
 return 1;_LL2F8:;_LL2F9:
# 2085
 return 0;_LL2F1:;}_LL2EF:;_LL2F0:
# 2087
 return 0;_LL2EA:;}
# 2091
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
# 2092
struct _RegionHandle _tmp5F0=_new_region("temp");struct _RegionHandle*temp=& _tmp5F0;_push_region(temp);
# 2093
{struct Cyc_Absyn_Tqual _tmp5F2;void*_tmp5F3;struct Cyc_List_List*_tmp5F4;struct _tuple12 _tmp5F1=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5F2=_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5F1.f3;
# 2094
_tmp5F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F4);
# 2095
if(_tmp5F4 == 0  && dopt == 0){
# 2096
struct Cyc_PP_Doc*_tmpAD2[2];struct Cyc_PP_Doc*_tmp5F6=(_tmpAD2[1]=Cyc_Absynpp_ntyp2doc(_tmp5F3),((_tmpAD2[0]=Cyc_Absynpp_tqual2doc(_tmp5F2),Cyc_PP_cat(_tag_dyneither(_tmpAD2,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5F6;}else{
# 2098
const char*_tmpAD5;struct Cyc_PP_Doc*_tmpAD4[4];struct Cyc_PP_Doc*_tmp5F9=
# 2099
(_tmpAD4[3]=
# 2102
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5F4),((_tmpAD4[2]=
# 2101
Cyc_PP_text(((_tmpAD5=" ",_tag_dyneither(_tmpAD5,sizeof(char),2)))),((_tmpAD4[1]=
# 2100
Cyc_Absynpp_ntyp2doc(_tmp5F3),((_tmpAD4[0]=
# 2099
Cyc_Absynpp_tqual2doc(_tmp5F2),Cyc_PP_cat(_tag_dyneither(_tmpAD4,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5F9;}}
# 2093
;_pop_region(temp);}
# 2106
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
# 2107
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FA:
# 2109
 if(f->width != 0){
# 2110
struct Cyc_Core_Opt*_tmpADC;const char*_tmpADB;const char*_tmpADA;struct Cyc_PP_Doc*_tmpAD9[5];return(_tmpAD9[4]=
# 2112
Cyc_PP_text(((_tmpADA=";",_tag_dyneither(_tmpADA,sizeof(char),2)))),((_tmpAD9[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAD9[2]=
# 2111
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAD9[1]=Cyc_PP_text(((_tmpADB=":",_tag_dyneither(_tmpADB,sizeof(char),2)))),((_tmpAD9[0]=
# 2110
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC->v=Cyc_PP_textptr(f->name),_tmpADC))))),Cyc_PP_cat(_tag_dyneither(_tmpAD9,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2114
struct Cyc_Core_Opt*_tmpAE1;const char*_tmpAE0;struct Cyc_PP_Doc*_tmpADF[3];return(_tmpADF[2]=
# 2115
Cyc_PP_text(((_tmpAE0=";",_tag_dyneither(_tmpAE0,sizeof(char),2)))),((_tmpADF[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpADF[0]=
# 2114
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->v=Cyc_PP_textptr(f->name),_tmpAE1))))),Cyc_PP_cat(_tag_dyneither(_tmpADF,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c: _LL2FB:
# 2117
 if(f->width != 0){
# 2118
struct Cyc_Core_Opt*_tmpAE8;const char*_tmpAE7;const char*_tmpAE6;struct Cyc_PP_Doc*_tmpAE5[5];return(_tmpAE5[4]=
# 2120
Cyc_PP_text(((_tmpAE6=";",_tag_dyneither(_tmpAE6,sizeof(char),2)))),((_tmpAE5[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAE5[2]=Cyc_PP_text(((_tmpAE7=":",_tag_dyneither(_tmpAE7,sizeof(char),2)))),((_tmpAE5[1]=
# 2119
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->v=Cyc_PP_textptr(f->name),_tmpAE8))))),((_tmpAE5[0]=
# 2118
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAE5,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2122
struct Cyc_Core_Opt*_tmpAED;const char*_tmpAEC;struct Cyc_PP_Doc*_tmpAEB[3];return(_tmpAEB[2]=
# 2123
Cyc_PP_text(((_tmpAEC=";",_tag_dyneither(_tmpAEC,sizeof(char),2)))),((_tmpAEB[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->v=Cyc_PP_textptr(f->name),_tmpAED))))),((_tmpAEB[0]=
# 2122
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAEB,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
# 2128
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
# 2129
const char*_tmpAEE;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAEE="",_tag_dyneither(_tmpAEE,sizeof(char),1))),fields);}
# 2132
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
# 2133
struct Cyc_PP_Doc*_tmpAEF[3];return(_tmpAEF[2]=
# 2134
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAEF[1]=
# 2133
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAEF[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAEF,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2137
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
# 2138
const char*_tmpAF0;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAF0=",",_tag_dyneither(_tmpAF0,sizeof(char),2))),fields);}
# 2146
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 2147
for(0;tdl != 0;tdl=tdl->tl){
# 2148
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
# 2149
const char*_tmpAF3;void*_tmpAF2;(_tmpAF2=0,Cyc_fprintf(f,((_tmpAF3="\n",_tag_dyneither(_tmpAF3,sizeof(char),2))),_tag_dyneither(_tmpAF2,sizeof(void*),0)));};}}
# 2153
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
# 2154
const char*_tmpAF4;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAF4="",_tag_dyneither(_tmpAF4,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2156
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
# 2157
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
# 2158
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2160
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
# 2161
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
# 2162
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
# 2163
Cyc_Absynpp_qvar_to_Cids=1;
# 2164
Cyc_Absynpp_add_cyc_prefix=0;{
# 2165
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
# 2166
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
# 2167
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
# 2168
return s;};}
# 2170
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
# 2171
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
# 2172
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
